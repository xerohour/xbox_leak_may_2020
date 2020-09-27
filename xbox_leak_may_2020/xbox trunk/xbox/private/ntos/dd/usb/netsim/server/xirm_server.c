/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xirm_server.c

Abstract:

    XInput remote server program

--*/

#include "precomp.h"
#include "xinput_remote.h"
#include "list.h"

// Our module name for debug spews
#define MYMODNAME "XIRM"

BOOL dosetup;
SOCKET sock, listensock;
HANDLE recvevt;
CHAR recvbuf[XIRM_MAX_PACKET_SIZE];
struct sockaddr_in clientaddr;
#define CLIENTADDR clientaddr.sin_addr.s_addr

BOOL autonetmode;
CHAR clientname[XIRM_MACHINE_NAME_MAXLEN];
DWORD connecttime;
DWORD clientisn, clientseq;
DWORD serverseq_una, serverseq_nxt;
INT fastrexmit, rexmit_timer, rexmit_count;
BOOL rexmit_disabled;
LIST_ENTRY sendq;

struct ConnectionRequest {
    DWORD clientisn;
    struct sockaddr_in clientaddr;
    CHAR clientname[XIRM_MACHINE_NAME_MAXLEN];
} connection_request;
CRITICAL_SECTION connection_request_lock;

#define ConnectReqLock()    EnterCriticalSection(&connection_request_lock)
#define ConnectReqUnlock()  LeaveCriticalSection(&connection_request_lock)
#define HasPendingConnectReq() (connection_request.clientaddr.sin_addr.s_addr != 0)

__inline BOOL GetPendingConnectReq(struct ConnectionRequest* connreq) {
    ConnectReqLock();
    *connreq = connection_request;
    connection_request.clientaddr.sin_addr.s_addr = 0;
    ConnectReqUnlock();

    return (connreq->clientaddr.sin_addr.s_addr != 0);
}

#define GetSendqHead() ((SendBuf*) sendq.Flink)
#define GetSendqLen() ((INT) (serverseq_nxt - serverseq_una))
#define IsSendqFull() (GetSendqLen() >= XIRM_SEND_WINDOW)
#define IsSendqEmpty() (serverseq_una == serverseq_nxt)

typedef struct _SendBuf {
    LIST_ENTRY links;
    INT datalen;
    XIrmPacketHeader data;
} SendBuf;

DWORD active_device_masks;
DWORD pending_insertions;
DWORD pending_removals;
struct DeviceData {
    HANDLE handle;
    XINPUT_STATE current_states;
    XINPUT_STATE last_states;
    XINPUT_CAPABILITIES devicecaps;
} devices[XIRM_PORTMAX];

//
// Main server loop sampling interval (10 msecs = 100Hz)
//
#define SAMPLING_INTERVAL 10

//
// Read the processor timestamp counter in milliseconds
//
__inline DWORD ReadTimestamp() {
    __asm {
        rdtsc
        mov ebx, 733000
        div ebx
    }
}

//
// Read the client configuration file
//
#define CONFIGFILENAME "d:\\xirmsrv.cfg"

BOOL
ReadConfig()
{
    CHAR buf[256];
    FILE* fin = fopen(CONFIGFILENAME, "r");

    if (!fin) {
        XDBGTRC(MYMODNAME, "cannot open config file");
        return FALSE;
    }

    while (fgets(buf, sizeof(buf), fin)) {
        CHAR *p, *val;
        
        p = strchr(buf, '=');
        if (!p) continue;

        *p++ = '\0';
        while (*p && isspace(*p)) p++;
        val = p;

        while (*p && *p != '\r' && *p != '\n' && !isspace(*p)) p++;
        *p = '\0';

        if (*val == '\0') continue;
        if (strcmp(buf, "name") == 0) {
            if (strlen(val) < XIRM_MACHINE_NAME_MAXLEN) {
                strcpy(clientname, val);
            }
        } else if (strcmp(buf, "ipaddr") == 0) {
            ULONG addr = inet_addr(val);
            if (addr != INADDR_NONE)
                CLIENTADDR = addr;
        }
    }
    fclose(fin);

    XDBGTRC(MYMODNAME, "client config: %s %u.%u.%u.%u", clientname, clientaddr.sin_addr.S_un.S_un_b.s_b1, clientaddr.sin_addr.S_un.S_un_b.s_b2, clientaddr.sin_addr.S_un.S_un_b.s_b3, clientaddr.sin_addr.S_un.S_un_b.s_b4);
    return (clientname[0] != '\0' || CLIENTADDR != 0);
}

//
// Write the client configuration file
//
BOOL
WriteConfig()
{
    FILE* fout = fopen(CONFIGFILENAME, "w");
    if (!fout) return FALSE;

    fprintf(fout, "name=%s\n", clientname[0] ? clientname : "<NONE>");
    fprintf(fout, "ipaddr=%u.%u.%u.%u\n", clientaddr.sin_addr.S_un.S_un_b.s_b1, clientaddr.sin_addr.S_un.S_un_b.s_b2, clientaddr.sin_addr.S_un.S_un_b.s_b3, clientaddr.sin_addr.S_un.S_un_b.s_b4);
    fclose(fout);
    return TRUE;
}

//
// Close the specified port
//
VOID ClosePort(DWORD port) {
    struct DeviceData* device = &devices[port];
    if (device->handle) {
        XInputClose(device->handle);
        memset(device, 0, sizeof(*device));
    }
}

//
// Open the specified port
//
BOOL OpenPort(DWORD port) {
    struct DeviceData* device = &devices[port];
    DWORD err;

    ASSERT(device->handle == NULL);
    device->handle = XInputOpen(
                        XDEVICE_TYPE_GAMEPAD,
                        port,
                        XDEVICE_NO_SLOT,
                        NULL);

    if (device->handle == NULL) {
        XDBGWRN(MYMODNAME, "XInputOpen failed: %d", GetLastError());
        return FALSE;
    }

    err = XInputGetCapabilities(device->handle, &device->devicecaps);
    if (err != ERROR_SUCCESS) {
        XDBGWRN(MYMODNAME, "XInputGetCapabilities failed: %d", err);
        return FALSE;
    }

    return TRUE;
}

//
// Poll the input states
//
VOID
PollInput()
{
    DWORD insertions, removals;
    DWORD port;

    // Check if there has been any device changes
    if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &insertions, &removals)) {
        // close devices that have been removed
        for (port=0; port < XIRM_PORTMAX; port++) {
            if (removals & (1 << port)) {
                if (devices[port].handle) {
                    ClosePort(port);
                }
            }
        }

        // open devices that have been inserted
        for (port=0; port < XIRM_PORTMAX; port++) {
            if (insertions & (1 << port)) {
                if (!OpenPort(port)) {
                    // If we failed to open the port, treat it as not plugged in
                    removals |= (1 << port);
                    insertions &= ~(1 << port);
                    ClosePort(port);
                }
            }
        }

        active_device_masks &= ~removals;
        active_device_masks |= insertions;

        pending_removals |= removals;
        pending_insertions &= ~removals;
        pending_insertions |= insertions;
    }

    // Now for currently inserted devices, update the state information
    for (port=0; port < XIRM_PORTMAX; port++) {
        struct DeviceData* device = &devices[port];
        if (device->handle) {
            DWORD err = XInputGetState(device->handle, &device->current_states);
            if (err != ERROR_SUCCESS) {
                XDBGWRN(MYMODNAME, "XInputGetState failed: %d", err);
                device->current_states = device->last_states;
            }
        }
    }
}

//
// Initialize the input module
//
#define MAGIC_KEYCOMBO (XINPUT_GAMEPAD_START | XINPUT_GAMEPAD_BACK)

VOID
InitInput()
{
    DWORD port;

    XInitDevices(0, NULL);
    Sleep(2000);

    // Get the initial device states
    PollInput();

    // Look for the special key combination to enter Setup mode
    for (port=0; port < XIRM_PORTMAX; port++) {
        if (devices[port].handle &&
            devices[port].current_states.Gamepad.wButtons == MAGIC_KEYCOMBO &&
            devices[port].current_states.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X]) {
            dosetup = TRUE;
            break;
        }
    }
}

//
// Transmit the first packet in the send queue
//
#define SetRexmitTimer() (rexmit_timer = ReadTimestamp() + XIRM_RETRANSMIT_TIMEOUT)
__inline VOID EnableRexmit() {
    if (rexmit_disabled) {
        rexmit_disabled = FALSE;
        rexmit_count = 1;
    }
}

VOID
SendqXmit(
    SendBuf* sendbuf
    )
{
    INT count;

    count = send(sock, (CHAR*) &sendbuf->data, sendbuf->datalen, 0);
    if (count != sendbuf->datalen) {
        XDBGWRN(MYMODNAME, "send failed: %d, %d", count, GetLastError());
    }

    if (sendbuf == GetSendqHead()) {
        SetRexmitTimer();
    }
    rexmit_count++;
    fastrexmit = 0;
}

//
// Do retransmission if needed
//
VOID
CheckRexmit()
{
    if (rexmit_disabled || IsSendqEmpty()) return;

    if ((INT) (rexmit_timer - ReadTimestamp()) < 0) {
        if (rexmit_count >= XIRM_RETRANSMIT_RETRY) {
            // If too many retries, we assume the client is down
            // and stop retransmitting until we receive a packet
            // from the client or there is an input state change.
            XDBGTRC(MYMODNAME, "client down?");
            rexmit_disabled = TRUE;
        } else {
            SendqXmit(GetSendqHead());
        }
    }
}

//
// Insert a new send buffer to the end of the send queue
// and then transmit it out to the client
//
VOID
InsertSendBuf(
    SendBuf* sendbuf
    )
{
    if (IsListEmpty(&sendq)) rexmit_count = 0;

    InsertTailList(&sendq, &sendbuf->links);
    SendqXmit(sendbuf);
}

//
// Allocate memory buffer for a new outgoing packet
//
SendBuf*
AllocSendBuf(
    INT size
    )
{
    SendBuf* sendbuf = (SendBuf*) malloc(offsetof(SendBuf, data) + size);

    if (sendbuf) {
        sendbuf->datalen = size;
        sendbuf->data.sendseq = serverseq_nxt++;
        sendbuf->data.ackseq = clientseq;
    } else {
        XDBGWRN(MYMODNAME, "out of memory");
    }
    return sendbuf;
}

//
// Send device changes to the client
//
VOID
SendDeviceChanges()
{
    SendBuf* sendbuf;
    XIrmPacketDeviceChanges* devchgpkt;
    XINPUT_CAPABILITIES* devcaps;
    INT pktlen;
    DWORD port;

    // Return immediately if:
    //  there has been no changes, or
    //  the send queue is full
    if ((pending_insertions | pending_removals) == 0 || IsSendqFull())
        return;

    pktlen = offsetof(XIrmPacketDeviceChanges, devicecaps) +
             XIrmCountDevices(active_device_masks) * sizeof(XINPUT_CAPABILITIES);
   
    sendbuf = AllocSendBuf(pktlen);
    if (!sendbuf) return;

    devchgpkt = (XIrmPacketDeviceChanges*) &sendbuf->data;
    devchgpkt->header.type = PKTTYPE_DEVICE_CHANGES;
    devchgpkt->active_devices = active_device_masks;
    devchgpkt->insertions = pending_insertions;
    devchgpkt->removals = pending_removals;

    // NOTE: We don't bother with keyboard support on silver box.
    devchgpkt->keyboard_device = 0;

    pending_insertions = pending_removals = 0;
    devcaps = devchgpkt->devicecaps;

    for (port=0; port < XIRM_PORTMAX; port++) {
        if (active_device_masks & (1 << port)) {
            *devcaps++ = devices[port].devicecaps;
        }
    }

    // Start retransmission again if input state has changed
    EnableRexmit();
    InsertSendBuf(sendbuf);
}

//
// Send input state changes to the client
//
VOID
SendStateChanges()
{
    DWORD port, masks = 0;
    INT pktlen = 0;
    SendBuf* sendbuf;
    XIrmPacketStateChanges* stchgpkt;
    XINPUT_STATE* states;
    
    if (IsSendqFull()) return;

    // Check if there has been any state changes
    for (port=0; port < XIRM_PORTMAX; port++) {
        if (devices[port].handle) {
            ASSERT(active_device_masks & (1 << port));
            if (memcmp(&devices[port].current_states,
                       &devices[port].last_states,
                       sizeof(XINPUT_STATE)) != 0) {
                masks |= (1 << port);
                pktlen++;
            }
        }
    }
    if (masks == 0) return;

    pktlen = offsetof(XIrmPacketStateChanges, states) + pktlen * sizeof(XINPUT_STATE);
    sendbuf = AllocSendBuf(pktlen);
    if (!sendbuf) return;

    stchgpkt = (XIrmPacketStateChanges*) &sendbuf->data;
    stchgpkt->header.type = PKTTYPE_STATE_CHANGES;
    stchgpkt->device_masks = masks;
    states = stchgpkt->states;
    for (port=0; port < XIRM_PORTMAX; port++) {
        if (masks & (1 << port)) {
            *states++ = devices[port].last_states = devices[port].current_states;
        }
    }

    // Start retransmission again if input state has changed
    EnableRexmit();
    InsertSendBuf(sendbuf);
}

//
// Initialize the connection with the client
//
VOID
InitClient(
    struct ConnectionRequest* connreq
    )
{
    SendBuf* sendbuf;
    DWORD port;
    INT err;

    strcpy(clientname, connreq->clientname);
    clientaddr = connreq->clientaddr;

    XDBGTRC(MYMODNAME,
        "connected to client: %s %u.%u.%u.%u:%d",
        clientname,
        clientaddr.sin_addr.S_un.S_un_b.s_b1, clientaddr.sin_addr.S_un.S_un_b.s_b2, clientaddr.sin_addr.S_un.S_un_b.s_b3, clientaddr.sin_addr.S_un.S_un_b.s_b4,
        ntohs(clientaddr.sin_port));

    err = connect(sock, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
    ASSERT(err == NO_ERROR);

    connecttime = ReadTimestamp();
    rexmit_disabled = FALSE;
    clientisn = connreq->clientisn;
    clientseq = clientisn+1;
    serverseq_una = serverseq_nxt = XIrmGetISN();

    while (!IsListEmpty(&sendq)) {
        LIST_ENTRY* p = RemoveHeadList(&sendq);
        free(p);
    }

    sendbuf = AllocSendBuf(sizeof(XIrmPacketHeader));
    ASSERT(sendbuf != NULL);
    sendbuf->data.type = PKTTYPE_SERVER_INIT;
    InsertSendBuf(sendbuf);

    // Send the initial device caps and states
    pending_insertions = active_device_masks;
    pending_removals = 0;
    for (port=0; port < XIRM_PORTMAX; port++) {
        memset(&devices[port].last_states, 0, sizeof(devices[port].last_states));
    }

    SendDeviceChanges();
    SendStateChanges();
}

//
// Process a set-state packet from the client
//
VOID
ProcessClientSetState(
    XIrmPacketSetState* pkt
    )
{
    static XINPUT_FEEDBACK feedback;
    DWORD err;

    if (pkt->port < XIRM_PORTMAX &&
        devices[pkt->port].handle &&
        feedback.Header.dwStatus != ERROR_IO_PENDING) {
        feedback = pkt->feedback;
        err = XInputSetState(devices[pkt->port].handle, &feedback);
        if (err != ERROR_SUCCESS &&
            err != ERROR_IO_PENDING &&
            err != ERROR_NOT_SUPPORTED) {
            XDBGWRN(MYMODNAME, "XInputSetState failed: %d", err);
        }
    }
}

//
// Check to see if we should serve an incoming connection request from a client
//
__inline BOOL IsAutonetAddr(ULONG addr) {
    // autonet address range is 169.254.x.x/16
    addr = ntohl(addr);
    return (addr & 0xffff0000) == 0xa9fe0000;
}

INT
MatchClient(
    CHAR* name,
    ULONG addr
    )
{
    XNADDR  XnAddr;
    static struct in_addr myaddr;

    // Try name match first
    if (clientname[0] && strcmp(clientname, name) == 0) return 1;

    // Then try explicit address match
    if (CLIENTADDR && CLIENTADDR == addr) return 2;

    // Now check to see if both client and server are using autonet addresses
    if (IsAutonetAddr(addr)) {
        if (myaddr.s_addr == 0) {
            XNetGetTitleXnAddr(&XnAddr);
            CopyMemory(&myaddr, &XnAddr.ina, sizeof(myaddr));
        }
        if (IsAutonetAddr(myaddr.s_addr)) return 3;
    }

    return 0;
}

//
// Process a client-init packet from the client
//
VOID
ProcessClientInit()
{
    struct ConnectionRequest connreq;

    if (GetPendingConnectReq(&connreq) &&
        MatchClient(connreq.clientname, connreq.clientaddr.sin_addr.s_addr)) {
        // Figure out how long has it been since the last connect
        INT time = ReadTimestamp() - connecttime;
        if (connreq.clientisn == clientisn && time >= 0 && time <= 3000) {
            // The packet is a duplicate - just drop it
        } else {
            // The client must have rebooted, reestablish connection
            XDBGTRC(MYMODNAME, "client must have rebooted");
            InitClient(&connreq);
        }
    }
}

//
// Process an acknowledgement packet from the client
//
VOID
ProcessClientAck(
    XIrmPacketHeader* pkt
    )
{
    DWORD ack;
    INT index, sent;

    ack = pkt->ackseq;
    index = (INT) (ack - serverseq_una);
    sent = GetSendqLen();

    if (index > 0 && index <= sent) {
        // The acknowledgement is valid
        ASSERT(!IsListEmpty(&sendq));
        serverseq_una = ack;

        do {
            SendBuf* sendbuf = GetSendqHead();
            if (sendbuf->data.sendseq == ack) {
                SetRexmitTimer();
                rexmit_count = 1;
                fastrexmit = 0;
                break;
            }
            RemoveHeadList(&sendq);
            free(sendbuf);
        } while (!IsListEmpty(&sendq));

    } else if (index == 0 && sent != 0) {
        // Fast retransmit
        if (++fastrexmit >= 2) {
            XDBGTRC(MYMODNAME, "fast rexmit");
            SendqXmit(GetSendqHead());
        }
    } else {
        XDBGTRC(MYMODNAME,
            "ack ignored: %u, %u, %u",
            ack, serverseq_una, serverseq_nxt);
    }
}

//
// Service network traffic from the client
//
VOID
ServiceClient()
{
    static BOOL pending = FALSE;
    static WSAOVERLAPPED overlapped;
    DWORD bytesrecv, flags;
    INT err;

    while (TRUE) {
        // Issue the overlapped recv request if necessary
        if (!pending) {
            WSABUF wsabuf;
            wsabuf.buf = recvbuf;
            wsabuf.len = sizeof(recvbuf);
            flags = 0;
            overlapped.hEvent = recvevt;
            err = WSARecv(sock, &wsabuf, 1, &bytesrecv, &flags, &overlapped, NULL);
            if (err == SOCKET_ERROR) {
                err = WSAGetLastError();
                if (err == WSA_IO_PENDING) {
                    pending = TRUE;
                } else {
                    XDBGWRN(MYMODNAME, "failed to issue recv request: %d", err);
                    return;
                }
            }
        }

        // If an overlapped recv request is pending,
        // check to see if it's completed yet.
        if (pending) {
            if (!WSAGetOverlappedResult(sock, &overlapped, &bytesrecv, FALSE, &flags)) {
                err = WSAGetLastError();
                if (err == WSA_IO_INCOMPLETE) return;

                //
                // This is very bad -
                //  cancel the pending recv request and try to issue a new one
                //
                XDBGWRN(MYMODNAME, "get overlapped result failed: %d", err);
                WSACancelOverlappedIO(sock);
                pending = FALSE;
                continue;
            }
            pending = FALSE;
        }

        // Client must be alive again - enable retransmission
        EnableRexmit();

        if (bytesrecv >= sizeof(XIrmPacketHeader)) {
            XIrmPacketHeader* pkt = (XIrmPacketHeader*) recvbuf;

            if (pkt->sendseq != clientseq) {
                XDBGTRC(MYMODNAME,
                    "client packet seq? %d %u, %u",
                    pkt->type, pkt->sendseq, clientseq);
            }

            switch (pkt->type) {
            case PKTTYPE_ACK:
                if (bytesrecv == sizeof(XIrmPacketHeader)) {
                    ProcessClientAck(pkt);
                }
                break;

            case PKTTYPE_SET_STATE:
                if (bytesrecv == sizeof(XIrmPacketSetState)) {
                    ProcessClientSetState((XIrmPacketSetState*) pkt);
                }
                break;

            default:
                XDBGTRC(MYMODNAME, "client packet type? %d", pkt->type);
                break;
            }
        } else {
            XDBGWRN(MYMODNAME, "client packet size? %d", bytesrecv);
        }
    }
}

//
// Wait for a client connection
//
VOID
WaitForClient()
{
    struct ConnectionRequest connreq;
    INT match = 0;
    
    XDBGTRC(MYMODNAME,
        "waiting for client connection: %u.%u.%u.%u %s",
        clientname,
        clientaddr.sin_addr.S_un.S_un_b.s_b1, clientaddr.sin_addr.S_un.S_un_b.s_b2, clientaddr.sin_addr.S_un.S_un_b.s_b3, clientaddr.sin_addr.S_un.S_un_b.s_b4);

    do {
        Sleep(SAMPLING_INTERVAL);
        if (HasPendingConnectReq() && GetPendingConnectReq(&connreq)) {
            match = MatchClient(connreq.clientname, connreq.clientaddr.sin_addr.s_addr);
            if (!match) {
                // the connection request wasn't valid
                XDBGTRC(MYMODNAME, "client-init doesn't match");
            }
        }
    } while (!match);

    if (match == 2 && connreq.clientname[0]) {
        // We have an address match and the client has a name.
        // Update the config file with the client name so
        // that we can do name match next time around.
        strcpy(clientname, connreq.clientname);
        WriteConfig();
    }

    InitClient(&connreq);
}

//
// Server loop
//
VOID
RunServer()
{
    DWORD timer;

    recvevt = WSACreateEvent();
    ASSERT(recvevt != NULL);
    ServiceClient();

    timer = ReadTimestamp() + SAMPLING_INTERVAL;
    while (TRUE) {
        INT timeout = timer - ReadTimestamp();

        if (timeout > 0) {
            DWORD wait = WaitForSingleObject(recvevt, timeout);
            if (wait == WAIT_OBJECT_0) {
                // Process packets from the client
                ServiceClient();
            }
        } else {
            if (HasPendingConnectReq()) {
                ProcessClientInit();
            }

            // Do retransmit if necessary
            CheckRexmit();

            // Check input state changes
            PollInput();
            SendDeviceChanges();
            SendStateChanges();

            timer = ReadTimestamp() + SAMPLING_INTERVAL;
        }
    }
}

//
// Client setup mode
//
VOID
DoSetup()
{
    struct ConnectionRequest connreq;

    XDBGTRC(MYMODNAME, "enter setup mode");
    memset(clientname, 0, sizeof(clientname));
    CLIENTADDR = 0;

    while (TRUE) {
        Sleep(SAMPLING_INTERVAL);
        if (HasPendingConnectReq()) {
            if (!GetPendingConnectReq(&connreq)) continue;

            // If both the client and server are using autonet address,
            // we'll automatically accept the client connection request.
            if (MatchClient(connreq.clientname, connreq.clientaddr.sin_addr.s_addr)) {
                autonetmode = TRUE;
                break;
            }

            // Display the client name / address in the UI and
            // ask the user for confirmation.
            // BUGBUG - not yet implemented
            XDBGWRN(MYMODNAME,
                "accept connection from %s %u.%u.%u.%u?",
                connreq.clientname,
                connreq.clientaddr.sin_addr.S_un.S_un_b.s_b1, connreq.clientaddr.sin_addr.S_un.S_un_b.s_b2, connreq.clientaddr.sin_addr.S_un.S_un_b.s_b3, connreq.clientaddr.sin_addr.S_un.S_un_b.s_b4);
        }
    }

    InitClient(&connreq);
}

//
// Initialize network stack
//
VOID
InitNet()
{
    INT err;
    WSADATA wsadata;
    struct sockaddr_in sockname;

    err = XNetStartup(NULL);
    ASSERT(err == NO_ERROR);

    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    ASSERT(err == NO_ERROR);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(sock != INVALID_SOCKET);

    memset(&sockname, 0, sizeof(sockname));
    sockname.sin_family = AF_INET;
    err = bind(sock, (struct sockaddr*) &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);

    listensock = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(listensock != INVALID_SOCKET);

    sockname.sin_port = htons(XIRM_SERVER_PORT);
    err = bind(listensock, (struct sockaddr*) &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);
}

//
// Server thread that listens for incoming client connection requests
//
DWORD WINAPI
ListenThreadProc(
    VOID* param
    )
{
    CHAR buf[XIRM_MAX_PACKET_SIZE];
    struct sockaddr sockname;
    struct sockaddr_in* sockin;
    XIrmPacketClientInit* initpkt;
    INT err;

    initpkt = (XIrmPacketClientInit*) buf;
    memset(&sockname, 0, sizeof(sockname));
    sockin = (struct sockaddr_in*) &sockname;

    while (TRUE) {
        INT fromlen = sizeof(sockname);
        INT count = recvfrom(listensock, buf, sizeof(buf), 0, &sockname, &fromlen);

        if (count != sizeof(*initpkt) || initpkt->header.type != PKTTYPE_CLIENT_INIT) {
            if (count >= sizeof(XIrmPacketHeader)) {
                XDBGTRC(MYMODNAME, "packet from %u.%u.%u.%u:%d discard: %d",
                    sockin->sin_addr.S_un.S_un_b.s_b1, sockin->sin_addr.S_un.S_un_b.s_b2, sockin->sin_addr.S_un.S_un_b.s_b3, sockin->sin_addr.S_un.S_un_b.s_b4,
                    ntohs(sockin->sin_port),
                    initpkt->header.type);
            } else {
                XDBGWRN(MYMODNAME, "recv error? %d, %d", count, GetLastError());
            }
            continue;
        }

        XDBGTRC(MYMODNAME,
            "client-init from %u.%u.%u.%u:%d - %x %u",
            sockin->sin_addr.S_un.S_un_b.s_b1, sockin->sin_addr.S_un.S_un_b.s_b2, sockin->sin_addr.S_un.S_un_b.s_b3, sockin->sin_addr.S_un.S_un_b.s_b4,
            ntohs(sockin->sin_port),
            initpkt->protocol_version,
            initpkt->header.sendseq);

        if (initpkt->protocol_version != XIRM_PROTOCOL_VERSION) {
            XDBGTRC(MYMODNAME, "client version mismatch: %x %x");
            continue;
        }

        ConnectReqLock();
        connection_request.clientisn = initpkt->header.sendseq;
        connection_request.clientaddr = *sockin;
        memcpy(connection_request.clientname, initpkt->client_name, XIRM_MACHINE_NAME_MAXLEN-1);
        ConnectReqUnlock();
    }
}


void __cdecl main()
{
    HANDLE thread;
    DWORD tid;

    InitializeListHead(&sendq);

    // Initialize network stack
    InitNet();

    // Initialize input
    InitInput();

    // Create server listening thread
    InitializeCriticalSection(&connection_request_lock);
    thread = CreateThread(NULL, 0, ListenThreadProc, NULL, 0, &tid);
    ASSERT(thread != NULL);
    CloseHandle(thread);

    // Check if the config file is present
    if (!dosetup) {
        if (!ReadConfig()) dosetup = TRUE;
    }

    if (dosetup) {
        DoSetup();
    } else {
        // Server loop
        WaitForClient();
    }

    RunServer();
    Sleep(INFINITE);
}

