#define DEBUG_KEYBOARD

#include <xtl.h>
#include <xdbg.h>

#ifdef D3DTEST
//
// One-time D3D initialization
//
#define CHECKHR(expr) do { hr = (expr); ASSERT(SUCCEEDED(hr)); } while (0)
IDirect3DDevice8* d3ddev;
VOID D3DInitialize()
{
    D3DPRESENT_PARAMETERS d3dpp;
    HRESULT hr;

    IDirect3D8* d3d = Direct3DCreate8(D3D_SDK_VERSION);
    ASSERT(d3d != NULL);

    memset(&d3dpp, 0, sizeof(d3dpp));
    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 1;
    d3dpp.Windowed                        = FALSE;
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                   = NULL;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    CHECKHR(IDirect3D8_CreateDevice(d3d,
                D3DADAPTER_DEFAULT,
                D3DDEVTYPE_HAL,
                NULL,
                D3DCREATE_HARDWARE_VERTEXPROCESSING,
                &d3dpp,
                &d3ddev));

    IDirect3D8_Release(d3d);

    CHECKHR(IDirect3DDevice8_Clear(d3ddev,
                0,
                NULL,
                D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                0x00000080,
                1.0,
                0));

    CHECKHR(IDirect3DDevice8_Present(d3ddev, NULL, NULL, NULL, NULL));
    IDirect3DDevice8_BlockUntilIdle(d3ddev);
}
#endif // D3DTEST


ULONG __cdecl DbgPrint(CHAR*, ...);

#define MAXPORTS 4
struct DeviceData {
    HANDLE handle;
    XINPUT_STATE states;
    XINPUT_CAPABILITIES devicecaps;
} devices[MAXPORTS];
DWORD active_device_masks;
HANDLE keyboard;

CHAR* GetDeviceType(UINT type)
{
    static CHAR* typestrs[3] = {
        "Gamepad",
        "Joystick",
        "Wheel"
    };

    return (type < 3) ? typestrs[type] : "<Unknown>";
}

VOID DumpThumbField(CHAR* name, SHORT old, SHORT new)
{
    if (old != new) {
        DbgPrint("  %10s: %d\n", name, new);
    }
}

VOID DumpGamepadChanges(XINPUT_GAMEPAD* oldst, XINPUT_GAMEPAD* newst)
{
    static CHAR* dbuttons[8] = {
        "UP",
        "DOWN",
        "LEFT",
        "RIGHT",
        "START",
        "BACK",
        "L-THUMB",
        "R-THUMB"
    };
    static CHAR* abuttons[8] = {
        "A",
        "B",
        "X",
        "Y",
        "BLACK",
        "WHITE",
        "L-TRIGGER",
        "R-TRIGGER"
    };

    INT i;
    DWORD dw0, dw1;

    // digital buttons
    dw0 = oldst->wButtons;
    dw1 = newst->wButtons;
    for (i=0; i < 8; i++) {
        DWORD m0 = dw0 & (1 << i);
        DWORD m1 = dw1 & (1 << i);
        if (m0 != m1) {
            DbgPrint("  %10s %s\n", dbuttons[i], m0 ? "-" : "+");
        }
    }

    // analog buttons
    for (i=0; i < 8; i++) {
        if (oldst->bAnalogButtons[i] != newst->bAnalogButtons[i]) {
            DbgPrint("  %10s %d\n", abuttons[i], newst->bAnalogButtons[i]);
        }
    }

    #define DUMPTHUMBFIELD(name) DumpThumbField(#name, oldst->s##name, newst->s##name)
    DUMPTHUMBFIELD(ThumbLX);
    DUMPTHUMBFIELD(ThumbLY);
    DUMPTHUMBFIELD(ThumbRX);
    DUMPTHUMBFIELD(ThumbRY);
}

VOID ClosePort(DWORD port)
{
    struct DeviceData* device = &devices[port];
    if (device->handle) {
        DbgPrint("Closing port %d...\n", port);
        XInputClose(device->handle);
        memset(device, 0, sizeof(*device));
    }
}

BOOL OpenPort(DWORD port)
{
    struct DeviceData* device = &devices[port];
    DWORD err;

    DbgPrint("Opening port %d...\n", port);
    if (device->handle) {
        DbgPrint("*** Port already open\n");
    }

    device->handle = XInputOpen(
                        XDEVICE_TYPE_GAMEPAD,
                        port,
                        XDEVICE_NO_SLOT,
                        NULL);

    if (device->handle == NULL) {
        DbgPrint("*** XInputOpen failed: %d\n", GetLastError());
        return FALSE;
    }

    err = XInputGetCapabilities(device->handle, &device->devicecaps);
    if (err != ERROR_SUCCESS) {
        DbgPrint("*** XInputGetCapabilities failed: %d\n", err);
        return FALSE;
    }

    DbgPrint("  device type: %s\n", GetDeviceType(device->devicecaps.SubType));
    return TRUE;
}

VOID PollInput()
{
    DWORD insertions, removals;
    DWORD port;
    static XINPUT_FEEDBACK feedback;
    XINPUT_DEBUG_KEYSTROKE key;

    // Check if there has been any device changes
    if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &insertions, &removals)) {
        DbgPrint("Device changes: in = %x out = %x\n", insertions, removals);

        // close devices that have been removed
        for (port=0; port < MAXPORTS; port++) {
            if (removals & (1 << port)) {
                if (devices[port].handle) {
                    ClosePort(port);
                }
            }
        }

        // open devices that have been inserted
        for (port=0; port < MAXPORTS; port++) {
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
        DbgPrint("  active devices: %x\n", active_device_masks);
    }

    // Now for currently inserted devices, update the state information
    for (port=0; port < MAXPORTS; port++) {
        struct DeviceData* device = &devices[port];
        if (device->handle) {
            XINPUT_STATE states;
            DWORD err = XInputGetState(device->handle, &states);
            if (err == ERROR_SUCCESS) {
                if (memcmp(&device->states, &states, sizeof(states)) != 0) {
                    DbgPrint("Input state change: port %d\n", port);
                    DumpGamepadChanges(&device->states.Gamepad, &states.Gamepad);

                    if (states.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] !=
                        device->states.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X]) {
                        feedback.Rumble.wLeftMotorSpeed =
                            (WORD) states.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] << 8;
                        DbgPrint("Rumble-L: %d\n", feedback.Rumble.wLeftMotorSpeed);
                        err = XInputSetState(device->handle, &feedback);
                        if (err != ERROR_SUCCESS) {
                            DbgPrint("*** XInputSetState failed: %d\n", err);
                        }
                    }

                    device->states = states;
                }
            } else {
                DbgPrint("*** XInputGetState failed: %d\n", err);
            }
        }
    }

    // Poll keystrokes
    #define DumpKeyFlags(f) \
            if (key.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_##f) { DbgPrint(" %s", #f); }
    while (XInputDebugGetKeystroke(&key) == ERROR_SUCCESS) {
        DbgPrint("Keystroke: ");
        DbgPrint(isprint(key.Ascii) ? "%c " : "%02x", key.Ascii);
        DbgPrint(" %02x", key.VirtualKey);
        DumpKeyFlags(KEYUP);
        DumpKeyFlags(REPEAT);
        DumpKeyFlags(CTRL);
        DumpKeyFlags(SHIFT);
        DumpKeyFlags(ALT);
        DumpKeyFlags(CAPSLOCK);
        DumpKeyFlags(NUMLOCK);
        DumpKeyFlags(SCROLLLOCK);
        DbgPrint("\n");
    }
}

VOID InitKeyboardDevice()
{
    DWORD err, devices;

    err = XInputDebugInitKeyboardQueue(NULL);
    if (err == ERROR_SUCCESS) {
        devices = XGetDevices(XDEVICE_TYPE_DEBUG_KEYBOARD);
        if (devices & 1) {
            keyboard = XInputOpen(
                            XDEVICE_TYPE_DEBUG_KEYBOARD,
                            XDEVICE_PORT0,
                            XDEVICE_NO_SLOT,
                            NULL);
        }
    }

    if (keyboard) DbgPrint("Keyboard initialized.\n");
}

void __cdecl main()
{
    DbgPrint("Starting XInput Remote tests...\n");
    XInitDevices(0, NULL);
    InitKeyboardDevice();

    #ifdef D3DTEST
    D3DInitialize();
    #endif

    while (TRUE) {
        PollInput();
        Sleep(20);
    }
}

