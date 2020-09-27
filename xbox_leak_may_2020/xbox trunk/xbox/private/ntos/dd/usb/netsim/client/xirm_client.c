/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xirm_client.c

Abstract:

    Remote XInput - client code

--*/

#include "precomp.h"
#include "xinput_remote.h"

// Module name string for the debug spews
#define MYMODNAME "XIRM"

// Global critical section for synchronization
CRITICAL_SECTION XIrmCriticalSection;
#define XIrmUnlock() LeaveCriticalSection(&XIrmCriticalSection)
#define XIrmLockAndServiceNet() do { \
            EnterCriticalSection(&XIrmCriticalSection); \
            XIrmNetService(); \
        } while (0)

//
// Current state of input devices
//
DWORD XIrmCurrentDevices;
DWORD XIrmDeviceInsertions;
DWORD XIrmDeviceRemovals;

#define XIRM_OPENPORT 0x80000000
struct DeviceData {
    DWORD cookie;
    XINPUT_STATE states;
    XINPUT_CAPABILITIES devicecaps;
} XIrmDevices[XIRM_PORTMAX];

#define XIrmCookieFromHandle(hdev) (((DWORD) (hdev) >> XIRM_PORTSHIFT) | XIRM_OPENPORT)
#define XIrmPortFromHandle(hdev) ((DWORD) (hdev) & (XIRM_PORTMAX-1))
#define XIrmMakeHandle(port) ((HANDLE) ((XIrmDevices[port].cookie << XIRM_PORTSHIFT) | (port)))
#define XIrmIsDeviceOpen(port) ((XIrmDevices[port].cookie & XIRM_OPENPORT) != 0)
#define XIrmMarkDeviceAsOpened(port) (XIrmDevices[port].cookie |= XIRM_OPENPORT)
#define XIrmMarkDeviceAsClosed(port) (XIrmDevices[port].cookie &= ~XIRM_OPENPORT)

//
// Current state of the keyboard
//
#define XIRM_KEYBOARD_HANDLE 0x7fffffff
DWORD XIrmKeyboardDevice;
DWORD XIrmKeyboardPending;
HANDLE XIrmKeyboardHandle;

XINPUT_DEBUG_KEYSTROKE* XIrmKeyboardQueue;
DWORD XIrmKeyboardQueueLimit = 32;
DWORD XIrmKeyboardQueueCount;
DWORD XIrmKeyboardQueueHead;
DWORD XIrmKeyboardQueueTail;
DWORD XIrmKeyboardQueueFlags =
        XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN |
        XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT |
        XINPUT_DEBUG_KEYQUEUE_FLAG_KEYUP;

// Current sequence numbers for the client and the server
DWORD XIrmClientSeq;
DWORD XIrmServerSeq;

// Socket for connection with the server
SOCKET XIrmSock;
CHAR XIrmPacketBuf[XIRM_MAX_PACKET_SIZE];

//
// Send an acknowledgement packet to the server
//  NOTE: ACK packets don't affect the send sequence number.
//
BOOL
XIrmSendAck()
{
    XIrmPacketHeader pkt;
    INT err;

    pkt.type = PKTTYPE_ACK;
    pkt.sendseq = XIrmClientSeq;
    pkt.ackseq = XIrmServerSeq;

    err = send(XIrmSock, (CHAR*) &pkt, sizeof(pkt), 0);
    if (err != sizeof(pkt)) {
        XDBGWRN(MYMODNAME, "send ack failed: %d, %d", err, GetLastError());
        return FALSE;
    }

    return TRUE;
}

//
// Send a SET_STATE packet to the server
//
DWORD
XIrmSendSetState(
    DWORD port,
    XINPUT_FEEDBACK* feedback
    )
{
    XIrmPacketSetState pkt;
    INT count;

    pkt.header.type = PKTTYPE_SET_STATE;
    pkt.header.sendseq = XIrmClientSeq;
    pkt.header.ackseq = XIrmServerSeq;
    pkt.port = port;
    pkt.feedback = *feedback;

    count = send(XIrmSock, (CHAR*) &pkt, sizeof(pkt), 0);
    return (count == SOCKET_ERROR) ? WSAGetLastError() : ERROR_SUCCESS;
}

//
// Handle the packet that has validated to be in sequence
//
VOID
XIrmHandlePacket(
    XIrmPacketHeader* pkt,
    INT pktlen
    )
{
    INT devcount, port;
    DWORD masks;
    XIrmPacketDeviceChanges* devchgpkt;
    XIrmPacketStateChanges* stchgpkt;
    XINPUT_CAPABILITIES* devcaps;
    XINPUT_STATE* states;
    XINPUT_DEBUG_KEYSTROKE* key;

    // Increment the next expected server sequence number
    XIrmServerSeq++;

    switch (pkt->type) {
    case PKTTYPE_DEVICE_CHANGES:
        pktlen -= offsetof(XIrmPacketDeviceChanges, devicecaps);
        ASSERT(pktlen >= 0);
        devchgpkt = (XIrmPacketDeviceChanges*) pkt;
        devcount = XIrmCountDevices(devchgpkt->active_devices);
        ASSERT(pktlen == devcount * sizeof(XINPUT_CAPABILITIES));

        XIrmCurrentDevices = masks = devchgpkt->active_devices;
        XIrmDeviceInsertions = devchgpkt->insertions;
        XIrmDeviceRemovals = devchgpkt->removals;

        // NOTE: we don't report keyboard removal.
        if ((devchgpkt->keyboard_device & 1) && !XIrmKeyboardDevice)
            XIrmKeyboardDevice = XIrmKeyboardPending = 1;

        // Update the cached device capabilities information
        devcaps = devchgpkt->devicecaps;
        for (port=0; port < XIRM_PORTMAX; port++) {
            if (masks & (1 << port))
                XIrmDevices[port].devicecaps = *devcaps++;
        }

        // If there is any change for a port, change its cookie
        masks = XIrmDeviceInsertions | XIrmDeviceRemovals;
        for (port=0; port < XIRM_PORTMAX; port++) {
            if (masks & (1 << port))
                XIrmDevices[port].cookie += 1;

            if (XIrmDeviceRemovals & (1 << port)) {
                struct DeviceData* device = &XIrmDevices[port];
                memset(&device->states, 0, sizeof(device->states));
                memset(&device->devicecaps, 0, sizeof(device->devicecaps));
            }
        }
        break;

    case PKTTYPE_STATE_CHANGES:
        pktlen -= offsetof(XIrmPacketStateChanges, states);
        ASSERT(pktlen >= 0);
        stchgpkt = (XIrmPacketStateChanges*) pkt;
        devcount = XIrmCountDevices(stchgpkt->device_masks);
        ASSERT(pktlen == devcount * sizeof(XINPUT_STATE));

        states = stchgpkt->states;
        masks = stchgpkt->device_masks;
        for (port=0; port < XIRM_PORTMAX; port++) {
            if (masks & (1 << port))
                XIrmDevices[port].states = *states++;
        }
        break;

    case PKTTYPE_KEYSTROKE:
        if (!XIrmKeyboardHandle) break;

        ASSERT(pktlen == sizeof(XIrmPacketKeystroke));
        key = &((XIrmPacketKeystroke*) pkt)->keystroke;

        // Filter out unwanted keystrokes
        if (!(key->Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP) &&
            !(XIrmKeyboardQueueFlags & XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN) ||
            (key->Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP) &&
            !(XIrmKeyboardQueueFlags & XINPUT_DEBUG_KEYQUEUE_FLAG_KEYUP) ||
            (key->Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_REPEAT) &&
            !(XIrmKeyboardQueueFlags & XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT) ||
            (key->Ascii == 0) &&
            (XIrmKeyboardQueueFlags & XINPUT_DEBUG_KEYQUEUE_FLAG_ASCII_ONLY))
            break;

        // Queue up the keystroke
        if (XIrmKeyboardQueueCount < XIrmKeyboardQueueLimit) {
            XIrmKeyboardQueueCount++;
            XIrmKeyboardQueue[XIrmKeyboardQueueTail++] = *key;
            if (XIrmKeyboardQueueTail == XIrmKeyboardQueueLimit)
                XIrmKeyboardQueueTail;
        }
        break;

    default:
        XDBGWRN(MYMODNAME, "server packet type? %d", pkt->type);
        break;
    }
}

//
// Process a received packet from the server
//

// Cached list of packets that were received out-of-sequence
typedef struct _XIrmRecvBuf {
    struct _XIrmRecvBuf* next;
    INT datalen;
    XIrmPacketHeader data;
} XIrmRecvBuf;
XIrmRecvBuf* XIrmRecvq;

INT
XIrmProcessReceivedPacket(
    INT pktlen
    )
{
    XIrmPacketHeader* pkt = (XIrmPacketHeader*) XIrmPacketBuf;
    INT index, acks;

    if (pkt->ackseq != XIrmClientSeq) {
        XDBGTRC(MYMODNAME, "server packet ack? %u, %u", pkt->ackseq, XIrmClientSeq);
    }

    index = (INT) (pkt->sendseq - XIrmServerSeq);
    if (index == 0) {
        // Normal case - in sequence packet
        acks = 1;
        XIrmHandlePacket(pkt, pktlen);

        while (XIrmRecvq && XIrmRecvq->data.sendseq == XIrmServerSeq) {
            XIrmRecvBuf* p = XIrmRecvq;
            XIrmRecvq = p->next;
            acks++;

            XIrmHandlePacket(&p->data, p->datalen);
            free(p);
        }
    } else if (index > 0 && index < XIRM_SEND_WINDOW) {
        // Out-of-sequence but inside the send window
        XIrmRecvBuf** list;
        XDBGTRC(MYMODNAME, "server packet out-of-seq: %u, %u", pkt->sendseq, XIrmServerSeq);

        list = &XIrmRecvq;
        while (*list && (*list)->data.sendseq < pkt->sendseq)
            list = &(*list)->next;
        
        if (*list && (*list)->data.sendseq == pkt->sendseq) {
            // duplicate packet - just throw it away
            XDBGTRC(MYMODNAME, "server packet duplicate");
        } else {
            XIrmRecvBuf* p;
            p = (XIrmRecvBuf*) malloc(offsetof(XIrmRecvBuf, data) + pktlen);
            if (p) {
                *list = p;
                p->next = NULL;
                p->datalen = pktlen;
                memcpy(&p->data, pkt, pktlen);
            } else {
                XDBGWRN(MYMODNAME, "out of memory");
            }
        }

        // Send an ack immediately
        acks = XIRM_DELAY_ACKS;
    } else {
        XDBGTRC(MYMODNAME,
            "server packet unexpected: %u, %u",
            pkt->sendseq,
            XIrmServerSeq);

        acks = 0;
    }

    return acks;
}

//
// Service network traffic from the server
//
VOID
XIrmNetService()
{
    // NOTE: we assume the caller has acquired the global lock at this point.
    u_long bytesavail;
    INT err, count;
    INT delayacks = 0;

    // Check if we have any data to read
    while (TRUE) {
        err = ioctlsocket(XIrmSock, FIONREAD, &bytesavail);
        if (err == NO_ERROR) {
            if (!bytesavail) break;
            count = recv(XIrmSock, XIrmPacketBuf, sizeof(XIrmPacketBuf), 0);
            if (count >= sizeof(XIrmPacketHeader)) {
                delayacks += XIrmProcessReceivedPacket(count);
                if (delayacks >= XIRM_DELAY_ACKS && XIrmSendAck())
                    delayacks = 0;
            } else {
                XDBGTRC(MYMODNAME, "server packet size? %d, %d", count, GetLastError());
            }
        } else {
            XDBGWRN(MYMODNAME, "ioctlsocket failed: %d", GetLastError());
            break;
        }
    }

    // Send the last ACK if needed
    if (delayacks) {
        XIrmSendAck();
    }
}

//
// Send out client-init broadcasts and wait for a server connection
//
VOID
XIrmFindServer()
{
    INT err, opt;
    struct sockaddr_in sockname;
    XIrmPacketClientInit initpkt;
    DWORD namelen;
    HRESULT hr;
    struct timeval timeout;
    fd_set readfds;

    // BUGBUG - spew out a debug message even on free builds
    DbgPrint("XINPUT_REMOTE: Trying to connect to a server...\n");

    // Enable broadcast on the socket
    opt = 1;
    err = setsockopt(XIrmSock, SOL_SOCKET, SO_BROADCAST, (CHAR*) &opt, sizeof(opt));
    ASSERT(err == NO_ERROR);

    XIrmClientSeq = XIrmGetISN();

    // Assemble the client-init packet
    memset(&initpkt, 0, sizeof(initpkt));
    initpkt.header.type = PKTTYPE_CLIENT_INIT;
    initpkt.header.sendseq = XIrmClientSeq++;
    initpkt.header.ackseq = 0;
    initpkt.protocol_version = XIRM_PROTOCOL_VERSION;

    // Get xbox name
    namelen = sizeof(initpkt.client_name);
    hr = DmGetXboxName(initpkt.client_name, &namelen);
    if (FAILED(hr)) {
        memset(initpkt.client_name, 0, sizeof(initpkt.client_name));
    }

    memset(&sockname, 0, sizeof(sockname));
    sockname.sin_family = AF_INET;
    timeout.tv_sec = XIRM_CLIENT_INIT_INTERVAL / 1000;
    timeout.tv_usec = (XIRM_CLIENT_INIT_INTERVAL % 1000) * 1000;

    while (TRUE) {
        XDBGTRC(MYMODNAME, "broadcast client-init: %u", initpkt.header.sendseq);

        // Broadcast the client-init packet
        sockname.sin_port = htons(XIRM_SERVER_PORT);
        sockname.sin_addr.s_addr = INADDR_BROADCAST;
        err = sendto(XIrmSock,
                     (CHAR*) &initpkt,
                     sizeof(initpkt),
                     0,
                     (struct sockaddr*) &sockname,
                     sizeof(sockname));
        ASSERT(err != SOCKET_ERROR);

        // Wait for a server to respond
        FD_ZERO(&readfds);
        FD_SET(XIrmSock, &readfds);
        err = select(1, &readfds, NULL, NULL, &timeout);

        if (err == 1) {
            // Read the packet from the server
            XIrmPacketHeader* pkt = (XIrmPacketHeader*) XIrmPacketBuf;
            INT fromlen = sizeof(sockname);
            INT count = recvfrom(
                            XIrmSock,
                            XIrmPacketBuf,
                            sizeof(XIrmPacketBuf),
                            0, 
                            (struct sockaddr*) &sockname,
                            &fromlen);

            if (count >= sizeof(*pkt)) {
                if (pkt->type == PKTTYPE_SERVER_INIT && pkt->ackseq == XIrmClientSeq) {
                    // Great - we got a connection
                    // Remember the server sequence number and proceed
                    XIrmServerSeq = pkt->sendseq+1;
                    break;
                } else {
                    XDBGTRC(MYMODNAME, "server packet type/ack? %d %u", pkt->type, pkt->ackseq);
                }
            } else {
                XDBGTRC(MYMODNAME, "server packet size? %d, %d", count, GetLastError());
            }
        } else if (err != 0) {
            XDBGWRN(MYMODNAME, "select failed: %d, %d", err, GetLastError());
        }
    }

    // Disable broadcast on the socket
    opt = 0;
    err = setsockopt(XIrmSock, SOL_SOCKET, SO_BROADCAST, (CHAR*) &opt, sizeof(opt));
    ASSERT(err == NO_ERROR);

    // Now connect the socket to the server
    err = connect(XIrmSock, (struct sockaddr*) &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);
    XDBGTRC(MYMODNAME, "connected to server: %u.%u.%u.%u", sockname.sin_addr.S_un.S_un_b.s_b1, sockname.sin_addr.S_un.S_un_b.s_b2, sockname.sin_addr.S_un.S_un_b.s_b3, sockname.sin_addr.S_un.S_un_b.s_b4);

    // Acknowledge the server-init packet
    XIrmSendAck();

    // Wait for a while to receive the initial packets from the server
    FD_ZERO(&readfds);
    FD_SET(XIrmSock, &readfds);
    timeout.tv_sec = XIRM_RETRANSMIT_TIMEOUT / 1000;
    timeout.tv_usec = (XIRM_RETRANSMIT_TIMEOUT % 1000) * 1000;

    err = select(1, &readfds, NULL, NULL, &timeout);

    if (err == 1) {
        XIrmNetService();
    } else if (err != 0) {
        XDBGWRN(MYMODNAME, "select failed: %d, %d", err, GetLastError());
    }
}

//------------------------------------------------------------------
// Public APIs
//------------------------------------------------------------------

VOID WINAPI
XInitDevices(
    IN DWORD NumDeviceTypes,
    IN PXDEVICE_PREALLOC_TYPE DeviceTypes
    )
{
    INT err;
    WSADATA wsadata;
    struct sockaddr_in sockname;

    XDBGENT(MYMODNAME, "XInitDevices: %d %x", NumDeviceTypes, DeviceTypes);

    InitializeCriticalSection(&XIrmCriticalSection);

    // Initialize the network stack
    err = XNetStartup(NULL);
    ASSERT(err == NO_ERROR);

    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    ASSERT(err == NO_ERROR);

    XIrmSock = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(XIrmSock != INVALID_SOCKET);

    // Bind the socket to a randomly-chosen port
    memset(&sockname, 0, sizeof(sockname));
    sockname.sin_family = AF_INET;
    err = bind(XIrmSock, (struct sockaddr*) &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);

    // Send out client-init broadcasts and wait for a server connection
    XIrmFindServer();
}

DWORD WINAPI
XGetDevices(
    IN PXPP_DEVICE_TYPE DeviceType
    )
{
    DWORD devices;

    XDBGENT(MYMODNAME, "XGetDevices: %x", DeviceType);

    XIrmLockAndServiceNet();
    if (DeviceType == XDEVICE_TYPE_GAMEPAD) {
        devices = XIrmCurrentDevices;
        XIrmDeviceInsertions = XIrmDeviceRemovals = 0;
    } else if (DeviceType == XDEVICE_TYPE_DEBUG_KEYBOARD) {
        devices = XIrmKeyboardDevice;
        XIrmKeyboardPending = 0;
    } else {
        devices = 0;
    }
    XIrmUnlock();

    return devices;
}

BOOL WINAPI
XGetDeviceChanges(
    IN PXPP_DEVICE_TYPE DeviceType,
    OUT PDWORD pInsertions,
    OUT PDWORD pRemovals
    )
{
    BOOL changed;

    XDBGENT(MYMODNAME, "XGetDeviceChanges: %x %x %x", DeviceType, pInsertions, pRemovals);

    XIrmLockAndServiceNet();
    if (DeviceType == XDEVICE_TYPE_GAMEPAD) {
        *pInsertions = XIrmDeviceInsertions;
        *pRemovals = XIrmDeviceRemovals;
        changed = (XIrmDeviceInsertions | XIrmDeviceRemovals) != 0;
        XIrmDeviceInsertions = XIrmDeviceRemovals = 0;
    } else if (DeviceType == XDEVICE_TYPE_DEBUG_KEYBOARD) {
        // Note: we don't have keyboard insertion and removal.
        *pInsertions = XIrmKeyboardPending;
        *pRemovals = 0;
        changed = XIrmKeyboardPending != 0;
        XIrmKeyboardPending = 0;
    } else {
        changed = FALSE;
    }
    XIrmUnlock();

    return changed;
}

HANDLE WINAPI
XInputOpen(
    IN PXPP_DEVICE_TYPE XppDeviceType,
    IN DWORD dwPort,
    IN DWORD dwSlot,
    IN PXINPUT_POLLING_PARAMETERS pPollingParameters OPTIONAL
    )
{
    HANDLE hdev;
    DWORD err = ERROR_SUCCESS;

    XDBGENT(MYMODNAME, "XInputOpen: %x %d %d %x",
        XppDeviceType,
        dwPort,
        dwSlot,
        pPollingParameters);

    XIrmLockAndServiceNet();

    if (XppDeviceType == XDEVICE_TYPE_GAMEPAD) {
        ASSERT(dwPort >= XDEVICE_PORT0 &&
               dwPort <= XDEVICE_PORT3 &&
               dwSlot == XDEVICE_NO_SLOT);

        if (XIrmIsDeviceOpen(dwPort)) {
            // The specified port is already open
            err = ERROR_BUSY;
        } else if (XIrmCurrentDevices & (1 << dwPort)) {
            XIrmMarkDeviceAsOpened(dwPort);
            hdev = XIrmMakeHandle(dwPort);
            err = ERROR_SUCCESS;
        } else {
            // The specified port doesn't have anything plugged in
            err = ERROR_DEVICE_NOT_CONNECTED;
        }
    } else if (XppDeviceType == XDEVICE_TYPE_DEBUG_KEYBOARD) {
        // We support at most one keyboard
        ASSERT(dwPort == XDEVICE_PORT0 && dwSlot == XDEVICE_NO_SLOT);
        if (XIrmKeyboardHandle) {
            err = ERROR_BUSY;
        } else if (XIrmKeyboardDevice == 0) {
            err = ERROR_DEVICE_NOT_CONNECTED;
        } else if (!XIrmKeyboardQueue) {
            // XInputDebugInitKeyboardQueue must be called first
            err = ERROR_INVALID_STATE;
        } else {
            hdev = XIrmKeyboardHandle = (HANDLE) XIRM_KEYBOARD_HANDLE;
            err = ERROR_SUCCESS;
        }
    } else {
        err = ERROR_CALL_NOT_IMPLEMENTED;
    }

    XIrmUnlock();

    if (err == ERROR_SUCCESS) {
        return hdev;
    } else {
        SetLastError(err);
        return NULL;
    }
}

VOID WINAPI
XInputClose(
    IN HANDLE hDevice
    )
{
    DWORD port;

    XDBGENT(MYMODNAME, "XInputClose: %x", hDevice);

    if (hDevice == NULL) return;
    XIrmLockAndServiceNet();

    if (hDevice == XIrmKeyboardHandle) {
        // Close keyboard device
        XIrmKeyboardHandle = NULL;
        XIrmKeyboardQueueCount =
        XIrmKeyboardQueueHead =
        XIrmKeyboardQueueTail = 0;
    } else {
        port = XIrmPortFromHandle(hDevice);
        if (XIrmIsDeviceOpen(port)) {
            XIrmMarkDeviceAsClosed(port);
        } else {
            XDBGWRN(MYMODNAME, "port already closed: %d", port);
        }
    }

    XIrmUnlock();
}

DWORD WINAPI
XInputGetCapabilities(
    IN HANDLE hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
    )
{
    DWORD port;
    DWORD err = ERROR_SUCCESS;

    XDBGENT(MYMODNAME, "XInputGetCapabilities: %x %x", hDevice, pCapabilities);
    ASSERT(hDevice != NULL);
    
    XIrmLockAndServiceNet();

    port = XIrmPortFromHandle(hDevice);
    if (XIrmCookieFromHandle(hDevice) == XIrmDevices[port].cookie) {
        *pCapabilities = XIrmDevices[port].devicecaps;
    } else {
        // Device isn't opened or has been remved and reinserted
        err = ERROR_DEVICE_NOT_CONNECTED;
    }

    XIrmUnlock();
    return err;
}

DWORD WINAPI
XInputGetState(
    IN HANDLE hDevice,
    OUT PXINPUT_STATE pState
    )
{
    DWORD port;
    DWORD err = ERROR_SUCCESS;

    XDBGENT(MYMODNAME, "XInputGetState: %x %x", hDevice, pState);
    ASSERT(hDevice != NULL);
    
    XIrmLockAndServiceNet();

    port = XIrmPortFromHandle(hDevice);
    if (XIrmCookieFromHandle(hDevice) == XIrmDevices[port].cookie) {
        *pState = XIrmDevices[port].states;
    } else {
        // Device isn't opened or has been remved and reinserted
        err = ERROR_DEVICE_NOT_CONNECTED;
    }

    XIrmUnlock();
    return err;
}

DWORD WINAPI
XInputSetState(
    IN HANDLE hDevice,
    OUT PXINPUT_FEEDBACK pFeedback
    )
{
    DWORD port, err;

    XIrmLockAndServiceNet();

    port = XIrmPortFromHandle(hDevice);
    if (XIrmCookieFromHandle(hDevice) == XIrmDevices[port].cookie) {
        err = XIrmSendSetState(port, pFeedback);
    } else {
        // Device isn't opened or has been remved and reinserted
        err = ERROR_DEVICE_NOT_CONNECTED;
    }

    XIrmUnlock();
    return err;
}

DWORD WINAPI
XInputPoll(
    IN HANDLE hDevice
    )
{
    DWORD port;
    DWORD err = ERROR_SUCCESS;

    XDBGENT(MYMODNAME, "XInputPoll: %x", hDevice);
    ASSERT(hDevice != NULL);

    XIrmLockAndServiceNet();

    port = XIrmPortFromHandle(hDevice);
    if (XIrmCookieFromHandle(hDevice) != XIrmDevices[port].cookie) {
        // Device isn't opened or has been remved and reinserted
        err = ERROR_DEVICE_NOT_CONNECTED;
    }

    XIrmUnlock();
    return err;
}

DWORD WINAPI
XInputDebugInitKeyboardQueue(
    IN PXINPUT_DEBUG_KEYQUEUE_PARAMETERS pParameters OPTIONAL
    )
{
    DWORD err;
    XIrmLockAndServiceNet();

    if (XIrmKeyboardQueue) {
        // Keyboard already initialized
        err = ERROR_ALREADY_INITIALIZED;
    } else {
        if (pParameters) {
            XIrmKeyboardQueueFlags = pParameters->dwFlags;
            if (pParameters->dwQueueSize)
                XIrmKeyboardQueueLimit = pParameters->dwQueueSize;
        }

        XIrmKeyboardQueue = (XINPUT_DEBUG_KEYSTROKE*)
            malloc(XIrmKeyboardQueueLimit*sizeof(XINPUT_DEBUG_KEYSTROKE));
        err = XIrmKeyboardQueue ? ERROR_SUCCESS : ERROR_OUTOFMEMORY;
    }
    
    XIrmUnlock();
    return err;
}

DWORD WINAPI
XInputDebugGetKeystroke(
    OUT PXINPUT_DEBUG_KEYSTROKE pKeystroke
    )
{
    DWORD err;

    ASSERT(pKeystroke != NULL);
    XIrmLockAndServiceNet();

    if (!XIrmKeyboardHandle) {
        // Must have called XInputDebugInitKeyboardQueue and XInputOpen
        err = ERROR_NOT_READY;
    } else if (XIrmKeyboardQueueCount == 0) {
        err = ERROR_HANDLE_EOF;
    } else {
        XIrmKeyboardQueueCount--;
        *pKeystroke = XIrmKeyboardQueue[XIrmKeyboardQueueHead++];
        if (XIrmKeyboardQueueHead == XIrmKeyboardQueueLimit)
            XIrmKeyboardQueueHead = 0;

        err = ERROR_SUCCESS;
    }

    XIrmUnlock();
    return err;
}

DWORD WINAPI
XMountMUA(
    IN DWORD dwPort,
    IN DWORD dwSlot,
    OUT POCHAR pchDrive
    )
{
    XDBGWRN(MYMODNAME, "XMountMU not supported");
    return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD WINAPI
XUnmountMU(
    IN DWORD dwPort,
    IN DWORD dwSlot
    )
{
    XDBGWRN(MYMODNAME, "XUnmountMU not supported");
    return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD WINAPI
XMUPortFromDriveLetterA(
    OCHAR chDrive
    )
{
    XDBGWRN(MYMODNAME, "XMUPortFromDriveLetter not supported");
    return (DWORD) -1;
}

DWORD WINAPI
XMUSlotFromDriveLetterA(
    OCHAR chDrive
    )
{
    XDBGWRN(MYMODNAME, "XMUSlotFromDriveLetter not supported");
    return (DWORD) -1;
}

