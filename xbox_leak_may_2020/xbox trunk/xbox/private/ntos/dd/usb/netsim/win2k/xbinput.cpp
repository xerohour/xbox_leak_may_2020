#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <xboxdbg.h>

extern "C" {
#include "xboxstub.h"
#include "xinput_remote.h"
}

#define DIRECTINPUT_VERSION 0x0700
#include <initguid.h>
#include <dinput.h>

CHAR* programname;
HINSTANCE appinstance;
HWND hwndmain;
BOOL hasfocus;
SOCKET sock, listensock;
HANDLE recvevt;
CHAR recvbuf[XIRM_MAX_PACKET_SIZE];
CHAR clientname[XIRM_MACHINE_NAME_MAXLEN];
CHAR xboxname[XIRM_MACHINE_NAME_MAXLEN];
ULONG clientaddr;
INT connected;
DWORD connecttime;
DWORD clientisn, clientseq;
DWORD serverseq_una, serverseq_nxt;
INT fastrexmit, rexmit_timer, rexmit_count;
BOOL rexmit_disabled;
IDirectInput7* dinput7;

//
// Pending connection request
//
struct ConnectionRequest {
    BOOL active;
    DWORD clientisn;
    struct sockaddr_in clientaddr;
    CHAR clientname[XIRM_MACHINE_NAME_MAXLEN];
} connection_request;
CRITICAL_SECTION connection_request_lock;

#define ConnectReqLock()    EnterCriticalSection(&connection_request_lock)
#define ConnectReqUnlock()  LeaveCriticalSection(&connection_request_lock)
#define HasPendingConnectReq() (connection_request.active)

//
// Send queue
//
#define GetSendqHead() ((SendBuf*) sendq.Flink)
#define GetSendqLen() ((INT) (serverseq_nxt - serverseq_una))
#define IsSendqFull() (GetSendqLen() >= XIRM_SEND_WINDOW)
#define IsSendqEmpty() (serverseq_una == serverseq_nxt)

LIST_ENTRY sendq;
typedef struct _SendBuf {
    LIST_ENTRY links;
    INT datalen;
    XIrmPacketHeader data;
} SendBuf;

//
// Device state information
//
DWORD device_count;
XINPUT_CAPABILITIES device_caps;
struct DeviceData {
    GUID guid_instance;
    IDirectInputDevice7* indev7;
    XINPUT_GAMEPAD current_states;
    XINPUT_GAMEPAD last_states;
} devices[XIRM_PORTMAX];

//
// InterAct HammerHead FX product GUID and capabilities
//
const GUID HammerHeadGuid = {
    0x262A05FD,
    0,
    0,
    { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 }
};

//
// Main server loop sampling interval (10 msecs = 100Hz)
//
#define SAMPLING_INTERVAL 10


//
// Error handling functions
//
inline VOID errormsg(CHAR* msg) {
    MessageBox(hwndmain, msg, programname, MB_OK|MB_ICONERROR);
}

VOID error(CHAR* format, ...)
{
    CHAR buf[1024];
    va_list arglist;

    va_start(arglist, format);
    vsprintf(buf, format, arglist);
    va_end(arglist);

    errormsg(buf);
    exit(-1);
}

#define ASSERT(cond) do { if (!(cond)) assert(__LINE__, #cond); } while(0)
VOID assert(INT line, CHAR* msg)
{
    error("Assertion failed: line %d\n"
          "  %s\n"
          "  error code = %d",
          line, msg, GetLastError());
}

VOID usage()
{
    CHAR buf[256];

    sprintf(buf, "usage: %s [-x xboxname]", programname);
    errormsg(buf);
    exit(1);
}

//
// Initialize the input module
//
BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    if (memcmp(&lpddi->guidProduct, &HammerHeadGuid, sizeof(HammerHeadGuid)) == 0 &&
        device_count < XIRM_PORTMAX) {
        devices[device_count++].guid_instance = lpddi->guidInstance;
    }
    return DIENUM_CONTINUE;
}

VOID InitInput()
{
    //
    // Initialize DirectInput
    //
    HRESULT hr;
    hr= DirectInputCreateEx(
            appinstance,
            DIRECTINPUT_VERSION,
            IID_IDirectInput7,
            (VOID**) &dinput7,
            NULL);

    ASSERT(SUCCEEDED(hr)); 

    //
    // Enumerate attached HammerHead controllers
    //
    hr = dinput7->EnumDevices(
            DIDEVTYPE_JOYSTICK ,
            EnumDevicesCallback,
            0,
            DIEDFL_ATTACHEDONLY);

    if (device_count == 0) {
        error("Found no HammerHead controller.");
    }

    //
    // Acquire all enumerated controllers for exclusive access
    //
    for (DWORD i=0; i < device_count; i++) {
        IDirectInputDevice7* indev7;
        hr = dinput7->CreateDeviceEx(
                devices[i].guid_instance,
                IID_IDirectInputDevice7,
                (VOID**) &indev7,
                NULL);

        if (FAILED(hr)) break;

        hr = indev7->SetDataFormat(&c_dfDIJoystick);
        if (FAILED(hr)) break;
        
        DIPROPDWORD prop;
        prop.diph.dwSize = sizeof(prop);
        prop.diph.dwHeaderSize = sizeof(prop.diph);
        prop.diph.dwObj = 0;
        prop.diph.dwHow = DIPH_DEVICE;
        prop.dwData = DIPROPAXISMODE_ABS;
        hr = indev7->SetProperty(DIPROP_AXISMODE, &prop.diph);
        if (FAILED(hr)) break;

        hr = indev7->SetCooperativeLevel(hwndmain, DISCL_EXCLUSIVE|DISCL_BACKGROUND);
        if (FAILED(hr)) break;

        hr = indev7->Acquire();
        if (FAILED(hr)) break;
        devices[i].indev7 = indev7;
    }

    if (i != device_count) {
        error("Failed to acquire controller %d: %x", i, hr);
    }

    XINPUT_GAMEPAD* gamepad = &device_caps.In.Gamepad;
    device_caps.SubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;
    gamepad->wButtons = 0xff;
    memset(gamepad->bAnalogButtons, 0x80, 8);
    gamepad->sThumbLX =
    gamepad->sThumbLY =
    gamepad->sThumbRX =
    gamepad->sThumbRY = (SHORT) 0xff00;
}

//
// Transmit the first packet in the send queue
//
#define SetRexmitTimer() (rexmit_timer = GetTickCount() + XIRM_RETRANSMIT_TIMEOUT)
inline VOID EnableRexmit() {
    if (rexmit_disabled) {
        rexmit_disabled = FALSE;
        rexmit_count = 1;
    }
}

VOID SendqXmit(SendBuf* sendbuf)
{
    send(sock, (CHAR*) &sendbuf->data, sendbuf->datalen, 0);
    if (sendbuf == GetSendqHead()) {
        SetRexmitTimer();
    }
    rexmit_count++;
    fastrexmit = 0;
}

//
// Do retransmission if needed
//
VOID CheckRexmit()
{
    if (rexmit_disabled || IsSendqEmpty()) return;

    if ((INT) (rexmit_timer - GetTickCount()) < 0) {
        if (rexmit_count >= XIRM_RETRANSMIT_RETRY) {
            // If too many retries, we assume the client is down
            // and stop retransmitting until we receive a packet
            // from the client or there is an input state change.
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
VOID InsertSendBuf(SendBuf* sendbuf)
{
    if (IsListEmpty(&sendq)) rexmit_count = 0;

    InsertTailList(&sendq, &sendbuf->links);
    SendqXmit(sendbuf);
}

//
// Allocate memory buffer for a new outgoing packet
//
SendBuf* AllocSendBuf(INT size)
{
    SendBuf* sendbuf = (SendBuf*) malloc(offsetof(SendBuf, data) + size);

    if (sendbuf) {
        sendbuf->datalen = size;
        sendbuf->data.sendseq = serverseq_nxt++;
        sendbuf->data.ackseq = clientseq;
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
    INT pktlen;

    pktlen = offsetof(XIrmPacketDeviceChanges, devicecaps) +
             device_count * sizeof(XINPUT_CAPABILITIES);
    sendbuf = AllocSendBuf(pktlen);
    if (!sendbuf) return;

    devchgpkt = (XIrmPacketDeviceChanges*) &sendbuf->data;
    devchgpkt->header.type = PKTTYPE_DEVICE_CHANGES;

    devchgpkt->keyboard_device = 1;
    devchgpkt->active_devices = 
    devchgpkt->insertions = (1 << device_count) - 1;
    devchgpkt->removals = 0;

    for (DWORD i=0; i < device_count; i++)
        devchgpkt->devicecaps[i] = device_caps;

    // Start retransmission again if input state has changed
    EnableRexmit();
    InsertSendBuf(sendbuf);
}

//
// Send input state changes to the client
//
VOID SendStateChanges()
{
    static DWORD pktnumber;
    SendBuf* sendbuf;
    XIrmPacketStateChanges* stchgpkt;
    
    // Send queue is full, do nothing during this interval
    if (IsSendqFull()) return;

    // Check to see if anything has changed
    DWORD masks = 0;
    INT pktlen = 0;
    for (DWORD i=0; i < device_count; i++) {
        if (memcmp(&devices[i].current_states,
                   &devices[i].last_states,
                   sizeof(XINPUT_GAMEPAD)) != 0) {
            masks |= (1 << i);
            pktlen++;
        }
    }
    if (masks == 0) return;

    pktlen = offsetof(XIrmPacketStateChanges, states) + pktlen*sizeof(XINPUT_STATE);
    sendbuf = AllocSendBuf(pktlen);
    if (!sendbuf) return;

    stchgpkt = (XIrmPacketStateChanges*) &sendbuf->data;
    stchgpkt->header.type = PKTTYPE_STATE_CHANGES;
    stchgpkt->device_masks = masks;

    XINPUT_STATE* states = stchgpkt->states;
    for (i=0; i < XIRM_PORTMAX; i++) {
        if (masks & (1 << i)) {
            states->Gamepad = devices[i].last_states = devices[i].current_states;
            states->dwPacketNumber = pktnumber++;
        }
    }

    // Start retransmission again if input state has changed
    EnableRexmit();
    InsertSendBuf(sendbuf);
}

//
// Poll the current state of the controllers
//
#define BUTTON_MAP1(x, w) \
        if (joystate.rgbButtons[w-1] & 0x80) buttons |= x

#define BUTTON_MAP2(x, w) \
        xstate->bAnalogButtons[x] = (joystate.rgbButtons[w-1] & 0x80)

#define BUTTON_MAP3(d) \
        buttons |= XINPUT_GAMEPAD_DPAD_##d

#define BUTTON_MAP4(d1, d2) \
        buttons |= (XINPUT_GAMEPAD_DPAD_##d1 | XINPUT_GAMEPAD_DPAD_##d2)

#define BUTTON_MAP5(x, w) \
        xstate->x = MapThumbValue(joystate.w)

inline SHORT MapThumbValue(LONG val) {
    // convert from range 0-ffff to 0-ff
    val = (val >> 8) & 0xff;

    // 0 - 8000
    // 7f and 80 - 0
    // ff - 7fff
    return (SHORT) ((val == 0x7f || val == 0x80) ? 0 : ((val | (val << 8)) - 0x8000));
}

VOID PollInput()
{
    IDirectInputDevice7* indev7;
    HRESULT hr;
    DIJOYSTATE joystate;

    if (!connected) return;

    for (DWORD i=0; i < device_count; i++) {
        indev7 = devices[i].indev7;
        
        memset(&joystate, 0, sizeof(joystate));
        indev7->Poll();
        hr = indev7->GetDeviceState(sizeof(joystate), &joystate);
        if (FAILED(hr)) continue;

        // Map the buttons
        XINPUT_GAMEPAD* xstate = &devices[i].current_states;
        DWORD buttons = 0;

        BUTTON_MAP1(XINPUT_GAMEPAD_LEFT_THUMB, 9);
        BUTTON_MAP1(XINPUT_GAMEPAD_RIGHT_THUMB, 10);
        BUTTON_MAP1(XINPUT_GAMEPAD_START, 11);
        BUTTON_MAP1(XINPUT_GAMEPAD_BACK, 12);

        BUTTON_MAP2(XINPUT_GAMEPAD_X, 1);
        BUTTON_MAP2(XINPUT_GAMEPAD_Y, 2);
        BUTTON_MAP2(XINPUT_GAMEPAD_WHITE, 3);
        BUTTON_MAP2(XINPUT_GAMEPAD_A, 4);
        BUTTON_MAP2(XINPUT_GAMEPAD_B, 5);
        BUTTON_MAP2(XINPUT_GAMEPAD_BLACK, 6);
        BUTTON_MAP2(XINPUT_GAMEPAD_B, 5);
        BUTTON_MAP2(XINPUT_GAMEPAD_LEFT_TRIGGER, 7);
        BUTTON_MAP2(XINPUT_GAMEPAD_RIGHT_TRIGGER, 8);
        
        DWORD pov = (joystate.rgdwPOV[0] & 0xffff);
        if (pov != 0xffff) {
            switch ((pov / 4500) % 8) {
            case 0:
                BUTTON_MAP3(UP); break;
            case 1:
                BUTTON_MAP4(UP,RIGHT); break;
            case 2:
                BUTTON_MAP3(RIGHT); break;
            case 3:
                BUTTON_MAP4(RIGHT,DOWN); break;
            case 4:
                BUTTON_MAP3(DOWN); break;
            case 5:
                BUTTON_MAP4(DOWN,LEFT); break;
            case 6:
                BUTTON_MAP3(LEFT); break;
            case 7:
                BUTTON_MAP4(LEFT,UP); break;
            }
        }
        xstate->wButtons = (WORD) buttons;

        joystate.lY ^= 0xffff;
        joystate.lRz ^= 0xffff;
        
        BUTTON_MAP5(sThumbLX, lX);
        BUTTON_MAP5(sThumbLY, lY);
        BUTTON_MAP5(sThumbRX, lZ);
        BUTTON_MAP5(sThumbRY, lRz);
    }

    SendStateChanges();
}

//
// Process an acknowledgement packet from the client
//
VOID ProcessClientAck(XIrmPacketHeader* pkt)
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
            SendqXmit(GetSendqHead());
        }
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

    XIrmPacketHeader* pkt;
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
                    // recv failed, try again
                    continue;
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
                CancelIo((HANDLE) sock);
                pending = FALSE;
                continue;
            }
            pending = FALSE;
        }

        // Client must be alive again - enable retransmission
        EnableRexmit();

        // The only packet type we care about is ACK
        pkt = (XIrmPacketHeader*) recvbuf;
        if (bytesrecv == sizeof(XIrmPacketHeader) &&
            pkt->type == PKTTYPE_ACK) {
            ProcessClientAck(pkt);
        }
    }
}

//
// Initialize the connection with the client
//
VOID InitClient(struct ConnectionRequest* connreq)
{
    SendBuf* sendbuf;
    INT err;

    connected++;
    strcpy(clientname, connreq->clientname);
    clientaddr = connreq->clientaddr.sin_addr.s_addr;
    err = connect(sock, (struct sockaddr*) &connreq->clientaddr, sizeof(connreq->clientaddr));
    ASSERT(err == NO_ERROR);

    // Force a window update
    InvalidateRect(hwndmain, NULL, TRUE);

    connecttime = GetTickCount();
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
    for (DWORD i=0; i < device_count; i++) {
        memset(&devices[i].last_states, 0, sizeof(XINPUT_GAMEPAD));
    }
    SendDeviceChanges();
    
    PollInput();
}

//
// Process a client-init packet from the client
//
VOID ProcessClientInit()
{
    struct ConnectionRequest connreq;

    // Retrieve information about the pending connection request
    ConnectReqLock();
    connreq = connection_request;
    connection_request.active = FALSE;
    ConnectReqUnlock();

    if (clientname[0] && strcmp(clientname, connreq.clientname) == 0 ||
        clientaddr == connreq.clientaddr.sin_addr.s_addr) {
        if (connected) {
            // We were connected to the client already.
            // Check to see if this client-init packet is a duplicate
            // or if the client has been rebooted.
            INT time = GetTickCount() - connecttime;
            if (connreq.clientisn == clientisn && time >= 0 && time <= 3000) return;
        }
        InitClient(&connreq);

        // For first time connection, start the overlapped receive request
        if (connected == 1) {
            ServiceClient();
        }
    }
}

//
// Handle keyboard events
//
VOID HandleKeyEvents(INT vkey, INT flags, BOOL keydown)
{
    SendBuf* sendbuf;
    XIrmPacketKeystroke* kbdpkt;
    BYTE xflags = 0;

    // Send queue is full, drop the keystroke
    if (!connected || IsSendqFull()) return;

    sendbuf = AllocSendBuf(sizeof(XIrmPacketKeystroke));
    if (!sendbuf) return;

    kbdpkt = (XIrmPacketKeystroke*) &sendbuf->data;
    kbdpkt->header.type = PKTTYPE_KEYSTROKE;

    kbdpkt->keystroke.VirtualKey = (BYTE) vkey;
    kbdpkt->keystroke.Ascii = (CHAR) MapVirtualKey(vkey, 2);
    
    if (!keydown) xflags |= XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP;
    if (keydown && (flags & (1 << 30))) xflags |= XINPUT_DEBUG_KEYSTROKE_FLAG_REPEAT;
    if (GetKeyState(VK_SHIFT) & 0x8000) xflags |= XINPUT_DEBUG_KEYSTROKE_FLAG_SHIFT;
    if (GetKeyState(VK_CONTROL) & 0x8000) xflags |= XINPUT_DEBUG_KEYSTROKE_FLAG_CTRL;
    if (GetKeyState(VK_MENU) & 0x8000) xflags |= XINPUT_DEBUG_KEYSTROKE_FLAG_ALT;
    if (GetKeyState(VK_CAPITAL) & 1) xflags |= XINPUT_DEBUG_KEYSTROKE_FLAG_CAPSLOCK;
    if (GetKeyState(VK_NUMLOCK) & 1) xflags |= XINPUT_DEBUG_KEYSTROKE_FLAG_NUMLOCK;
    if (GetKeyState(VK_SCROLL) & 1) xflags |= XINPUT_DEBUG_KEYSTROKE_FLAG_SCROLLLOCK;
    kbdpkt->keystroke.Flags = xflags;

    // Start retransmission again if input state has changed
    EnableRexmit();
    InsertSendBuf(sendbuf);
}

//
// Handle window repaint event
//
VOID DoPaint(HWND hwnd)
{
    static HFONT font;
    HDC hdc;
    PAINTSTRUCT ps;
    CHAR* p;
    INT x, y;
    CHAR buf[64];

    hdc = BeginPaint(hwnd, &ps);

    if (font == NULL) {
        LOGFONT lf;
        INT mmy = GetDeviceCaps(hdc, VERTSIZE);
        INT pixy = GetDeviceCaps(hdc, VERTRES);
        double pt = pixy * 25.4 / (mmy * 72);

        memset(&lf, 0, sizeof(lf));
        lf.lfHeight = -(INT) (10 * pt);
        lf.lfWeight = FW_BOLD;
        strcpy(lf.lfFaceName, "Arial");
        font = CreateFontIndirect(&lf);
    }

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 255, 0));
    SelectObject(hdc, font);

    x = 5;
    y = 0;
    if (connected) {
        sprintf(buf, "Connected to client (%d):", connected);
        p = buf;
    } else {
        p = "Waiting for client connection...";
    }
    TextOut(hdc, x, y, p, strlen(p));

    if (clientname[0]) {
        p = clientname;
        y += 20;
        TextOut(hdc, x, y, p, strlen(p));
    }

    if (clientaddr) {
        p = inet_ntoa(*((struct in_addr*) &clientaddr));
        y += 20;
        TextOut(hdc, x, y, p, strlen(p));
    }

    EndPaint(hwnd, &ps);
}

//
// Window callback procedure
//
LRESULT CALLBACK MyWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case WM_PAINT:
        DoPaint(hwnd);
        break;

    case WM_SETFOCUS:
        hasfocus = TRUE;
        break;

    case WM_KILLFOCUS:
        hasfocus = FALSE;
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
        HandleKeyEvents(wparam, lparam, msg==WM_KEYDOWN);
        break;

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        HandleKeyEvents(wparam, lparam, msg==WM_SYSKEYDOWN);
        // fall through to the default winproc

    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}

//
// Create the main application window
//
VOID CreateMainWindow()
{
    HBITMAP bitmap;
    BITMAP bmpinfo;
    HBRUSH brush;
    LOGBRUSH logbrush;
    INT ret;

    WNDCLASS wndclass = {
        0,
        MyWindowProc,
        0,
        0,
        appinstance,
        LoadIcon(appinstance, MAKEINTRESOURCE(128)),
        LoadCursor(NULL, IDC_ARROW),
        NULL,
        NULL,
        "XBINPUT"
    };

    // Load the background bitmap
    bitmap = LoadBitmap(appinstance, MAKEINTRESOURCE(128));
    ASSERT(bitmap != NULL);
    ret = GetObject(bitmap, sizeof(bmpinfo), &bmpinfo);
    ASSERT(ret != 0);

    // Create the pattern brush
    logbrush.lbStyle = BS_PATTERN;
    logbrush.lbColor = 0;
    logbrush.lbHatch = (LONG) bitmap;
    brush = CreateBrushIndirect(&logbrush);
    ASSERT(brush != NULL);

    wndclass.hbrBackground = brush;
    RegisterClass(&wndclass);

    hwndmain = CreateWindowEx(
                    WS_EX_APPWINDOW|WS_EX_OVERLAPPEDWINDOW,
                    "XBINPUT",
                    programname,
                    WS_POPUPWINDOW|WS_CAPTION|WS_MINIMIZEBOX,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    bmpinfo.bmWidth +
                        2*GetSystemMetrics(SM_CXFIXEDFRAME),
                    bmpinfo.bmHeight +
                        2*GetSystemMetrics(SM_CYFIXEDFRAME) +
                        GetSystemMetrics(SM_CYCAPTION),
                    NULL,
                    NULL,
                    appinstance,
                    NULL);

    ASSERT(hwndmain != NULL);
    ShowWindow(hwndmain, SW_SHOW);
}

//
// Server loop
//
VOID RunServer()
{
    MSG msg;
    DWORD wait;

    while (TRUE) {
        // NOTE: We can't call MsgWaitForMultipleObjects when we're inactive
        //  because it doesn't return long after the event is signaled.
        if (hasfocus) {
            wait = MsgWaitForMultipleObjects(1, &recvevt, FALSE, SAMPLING_INTERVAL, QS_ALLEVENTS);
        } else {
            wait = WaitForSingleObject(recvevt, SAMPLING_INTERVAL);
        }

        // Process client packets
        if (wait == WAIT_OBJECT_0) {
            ServiceClient();
        }

        // Check if there is any pending connection request
        if (HasPendingConnectReq()) {
            ProcessClientInit();
        }
        
        // Process window messages
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return;
            DispatchMessage(&msg);
        }

        // Check input state changes
        PollInput();

        // Do retransmit if necessary
        CheckRexmit();
    }
}

//
// Initialize network stack
//
VOID InitNet()
{
    INT err;
    WSADATA wsadata;
    struct sockaddr_in sockname;

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
DWORD WINAPI ListenThreadProc(VOID* param)
{
    CHAR buf[XIRM_MAX_PACKET_SIZE];
    struct sockaddr sockname;
    struct sockaddr_in* sockin;
    XIrmPacketClientInit* initpkt;

    initpkt = (XIrmPacketClientInit*) buf;
    memset(&sockname, 0, sizeof(sockname));
    sockin = (struct sockaddr_in*) &sockname;

    while (TRUE) {
        INT fromlen = sizeof(sockname);
        INT count = recvfrom(listensock, buf, sizeof(buf), 0, &sockname, &fromlen);

        if (count != sizeof(*initpkt) ||
            initpkt->header.type != PKTTYPE_CLIENT_INIT ||
            initpkt->protocol_version != XIRM_PROTOCOL_VERSION) {
            continue;
        }

        ConnectReqLock();
        connection_request.clientisn = initpkt->header.sendseq;
        connection_request.clientaddr = *sockin;
        memcpy(connection_request.clientname, initpkt->client_name, XIRM_MACHINE_NAME_MAXLEN-1);
        connection_request.active = TRUE;
        ConnectReqUnlock();
    }
}

INT __cdecl main(INT argc, CHAR** argv)
{
    HANDLE thread;
    DWORD tid;
    HRESULT hr;

    appinstance = GetModuleHandle(NULL);
    programname = *argv++;
    argc--;

    hr = CoInitialize(NULL);
    ASSERT(SUCCEEDED(hr));

    InitializeCriticalSection(&connection_request_lock);
    InitializeListHead(&sendq);
    recvevt = WSACreateEvent();
    ASSERT(recvevt != NULL);

    // Parse command line arguments
    while (argc) {
        CHAR* opt = *argv++;
        argc--;

        if (strcmp(opt, "-x") == 0 && argc > 0 &&
            strlen(*argv) < XIRM_MACHINE_NAME_MAXLEN && !xboxname[0]) {
            strcpy(xboxname, *argv);
            argv++;
            argc--;
        } else {
            usage();
        }
    }

    // Initialize network stack
    InitNet();

    if (xboxname[0]) {
        hr = DmSetXboxName(xboxname);
    } else {
        DWORD size = sizeof(xboxname);
        hr = DmGetXboxName(xboxname, &size);
    }

    if (FAILED(hr)) {
        error("Failed to determine the client xbox name");
    }

    clientaddr = inet_addr(xboxname);
    if (clientaddr == INADDR_NONE) clientaddr = 0;
    if (!clientaddr) {
        strcpy(clientname, xboxname);
    }

    // Create the main application window
    CreateMainWindow();

    // Initialize input
    InitInput();

    // Create server listening thread
    thread = CreateThread(NULL, 0, ListenThreadProc, NULL, 0, &tid);
    ASSERT(thread != NULL);
    CloseHandle(thread);

    RunServer();

    CoUninitialize();
    return 0;
}

