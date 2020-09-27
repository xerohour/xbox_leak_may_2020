/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    input.cpp

Author:

    Matt Bronder

Description:

    DirectInput initialization routines.

*******************************************************************************/

#include "d3dlocus.h"
#ifndef UNDER_XBOX
#include <dinput.h>
#else
#include <xbox.h>
#endif // UNDER_XBOX

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif // UNDER_XBOX

//******************************************************************************
// Local function prototypes
//******************************************************************************

#ifndef UNDER_XBOX
static LPDIRECTINPUTDEVICE8     CreateInputDevice(int nPort = -1);
#else
static HANDLE                   CreateInputDevice(int nPort = -1);
#endif // UNDER_XBOX

//******************************************************************************
// Globals
//******************************************************************************

#ifndef UNDER_XBOX
static HWND                     g_hWnd = NULL;
LPDIRECTINPUT8                  g_pdi;
LPDIRECTINPUTDEVICE8            g_pdid;
#else
HANDLE                          g_hDevice = NULL;
BOOL                            g_bInitDevices = TRUE;
#endif // UNDER_XBOX


int                             g_nEnumPort;
// Array of joystick button indices, arranged by usage.
BYTE                            g_rgbButtons[NUM_JOYSTICK_BUTTONS];
LONG                            g_lJoyMinX,
                                g_lJoyMaxX,
                                g_lJoyMinY,
                                g_lJoyMaxY;

//******************************************************************************
// Callback functions
//******************************************************************************

#ifndef UNDER_XBOX

//******************************************************************************
//
// Function:
//
//     DIDeviceEnumCallback
//
// Description:
//
//     Enumerate DirectInput devices for a joystick of the given port.  If
//     found, create a DirectInputDevice object using the guid of the device
//     instance, query for the IDirectInputDevice2 interface, and return a 
//     pointer to it through the application-defined context.
//
// Arguments:
//
//     LPCDIDEVICEINSTANCE pdidi    - Description of the device
//
//     LPVOID pvContext (Out)       - Pointer to an IDirectInputDevice2 interface
//
// Return Value:
//
//     DIENUM_CONTINUE to continue the enumeration or DIENUM_STOP to stop it.
//
//     If the enumeration succesfully found a joystick device corresponding to
//     the given port, pvRef will contain a pointer to an IDirectInputDevice2
//     interface on the device.
//
//******************************************************************************
static BOOL CALLBACK DIDeviceEnumCallback(LPCDIDEVICEINSTANCE pdidi, LPVOID pvContext) {

    LPDIRECTINPUTDEVICE8*   pdid8 = (LPDIRECTINPUTDEVICE8*)pvContext;
    HRESULT                 hr;

    if (!g_pdi) {
        return DIENUM_STOP;
    }

    // Since we don't really have the concept of a port, we'll
    // use the port number to decide which enumerated joystick to choose
    // (port 0 will yield the first joystick enumerated, port 1 will yield
    // the second, etc.)
    if (g_nEnumPort > 0) {
        g_nEnumPort--;
        return DIENUM_CONTINUE;
    }

    // Sanity check: Is this device a joystick?
    if (DIDFT_GETTYPE(pdidi->dwDevType) != DI8DEVTYPE_JOYSTICK)
    {
        DebugString(TEXT("Warning: Joystick enumeration callback enumerated ")
                            TEXT("a device not identified as a joystick"));
    }

    // Attempt to create the device
    hr = g_pdi->CreateDevice(pdidi->guidInstance, pdid8, NULL);
    ResultFailed(hr, TEXT("IDirectInput::CreateDevice"));

    return DIENUM_STOP;
}

//******************************************************************************
//
// Function:
//
//     DIObjectsEnumCallback
//
// Description:
//
//     Enumerate the objects (buttons and axes) on a DirectInput device.
//     Store the indices for the objects in a global array according to the
//     usages of the objects.
//
// Arguments:
//
//     LPCDIDEVICEOBJECTINSTANCE pddio  - Description of the object
//
//     LPVOID pvContext                 - Number of buttons enumerated
//
// Return Value:
//
//     DIENUM_CONTINUE to continue the enumeration or DIENUM_STOP to stop it.
//
//******************************************************************************
static BOOL CALLBACK DIObjectsEnumCallback(LPCDIDEVICEOBJECTINSTANCE pddio, 
                                            LPVOID pvContext) {

    UINT* puButtons = (UINT*)pvContext;

    // Store the index in the array, according to its usage.  We can
    // assume that the buttons on a device are enumerated in order,
    // by index.
#ifdef UNDER_CE
    g_rgbButtons[pddio->wUsage - USAGE_FIRST_BUTTON] = (*puButtons)++;
#else
    g_rgbButtons[*puButtons] = (*puButtons)++;
#endif // UNDER_CE

    return DIENUM_CONTINUE;
}

#endif // !UNDER_XBOX

//******************************************************************************
// Initialization functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     CreateInput
//
// Description:
//
//     Initialize all DirectInput objects necessary for obtaining joystick input.
//
// Arguments:
//
//     HWND hWnd                - Window handle to be associated with 
//                                DirectInput
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CreateInput(HWND hWnd) {

#ifndef UNDER_XBOX

    HRESULT hr;

    if (!hWnd) {
        return FALSE;
    }

    g_hWnd = hWnd;

#ifndef UNICODE
    hr = DirectInput8Create(GetTestInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8A, (LPVOID*)&g_pdi, NULL);
#else
    hr = DirectInput8Create(GetTestInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8W, (LPVOID*)&g_pdi, NULL);
#endif // !UNICODE
    if (ResultFailed(hr, TEXT("DirectInputCreate"))) {
        return FALSE;
    }

    // Create a joystick device using the first port with a joystick plugged in
    g_pdid = CreateInputDevice();

    return TRUE;

#else

    if (g_bInitDevices) {

        if (!(GetStartupContext() & TSTART_HARNESS)) {
            XInitDevices(0,NULL);
        }

        g_bInitDevices = FALSE;
    }

    g_hDevice = CreateInputDevice();

    return TRUE;

#endif // UNDER_XBOX
}

#ifndef UNDER_XBOX

//******************************************************************************
//
// Function:
//
//     CreateInputDevice
//
// Description:
//
//     Create a DirectInputDevice joystick object using the given port
//
// Arguments:
//
//     int nPort               - Joystick port to create the device on (a 
//                               negative port number will cause the first
//                               joystick enumerated to be chosen)
//
// Return Value:
//
//     A pointer to the created IDirectInputDevice2 object on success,
//     NULL on failure.
//
//******************************************************************************
static LPDIRECTINPUTDEVICE8 CreateInputDevice(int nPort) {

    LPDIRECTINPUTDEVICE8    pDevice = NULL;
    DIPROPRANGE             dipr;
    UINT                    uNumButtons = 0;
    HRESULT                 hr;

    if (!g_pdi) {
        return NULL;
    }

    g_nEnumPort = nPort;

    hr = g_pdi->EnumDevices(DI8DEVTYPE_JOYSTICK, DIDeviceEnumCallback, 
        (LPVOID)&pDevice, DIEDFL_ATTACHEDONLY);

    if (ResultFailed(hr, TEXT("IDirectInput::EnumDevices"))) {
        return NULL;
    }

    // Return false if the joystick is not currently attached, or no such
    // port exists
    if (!pDevice) {
        return NULL;
    }

    // Enumerate the buttons that are available with the joystick
    hr = pDevice->EnumObjects(DIObjectsEnumCallback, &uNumButtons, DIDFT_BUTTON);
    if (ResultFailed(hr, TEXT("IDirectInputDevice2::EnumObjects"))) {
        pDevice->Release();
        return NULL;
    }

    // Set the translation array
    g_rgbButtons[JOYBUTTON_X] = 0;
    g_rgbButtons[JOYBUTTON_Y] = 1;
    g_rgbButtons[JOYBUTTON_WHITE] = 2;
    g_rgbButtons[JOYBUTTON_A] = 3;
    g_rgbButtons[JOYBUTTON_B] = 4;
    g_rgbButtons[JOYBUTTON_BLACK] = 5;
    g_rgbButtons[JOYBUTTON_LTRIG] = 6; 
    g_rgbButtons[JOYBUTTON_RTRIG] = 7;
    g_rgbButtons[JOYBUTTON_LSTICK] = 8;
    g_rgbButtons[JOYBUTTON_RSTICK] = 9;
    g_rgbButtons[JOYBUTTON_UP] = 12;
    g_rgbButtons[JOYBUTTON_DOWN] = 13;
    g_rgbButtons[JOYBUTTON_LEFT] = 14;
    g_rgbButtons[JOYBUTTON_RIGHT] = 15;
    g_rgbButtons[JOYBUTTON_BACK] = 11;
    g_rgbButtons[JOYBUTTON_START] = 10;

    // Set the data format for the joystick
    hr = pDevice->SetDataFormat(&c_dfDIJoystick);
    if (ResultFailed(hr, TEXT("IDirectInputDevice::SetDataFormat"))) {
        pDevice->Release();
        return NULL;
    }

    // Set the cooperative level
    hr = pDevice->SetCooperativeLevel(g_hWnd, DISCL_EXCLUSIVE 
                                            | DISCL_FOREGROUND);
    if (ResultFailed(hr, TEXT("IDirectInputDevice::SetCooperativeLevel"))) {
        pDevice->Release();
        return NULL;
    }

    // Get the range of the joystick
    memset(&dipr, 0, sizeof(DIPROPRANGE));
    dipr.diph.dwSize = sizeof(DIPROPRANGE);
    dipr.diph.dwHeaderSize = sizeof(dipr.diph);
    dipr.diph.dwObj = DIJOFS_X;
    dipr.diph.dwHow = DIPH_BYOFFSET;

    hr = pDevice->GetProperty(DIPROP_RANGE, (LPDIPROPHEADER)&dipr);
    if (ResultFailed(hr, TEXT("IDirectInputDevice::GetProperty"))) {
        pDevice->Release();
        return NULL;
    }

    g_lJoyMinX = dipr.lMin;
    g_lJoyMaxX = dipr.lMax;

    dipr.diph.dwObj = DIJOFS_Y;

    hr = pDevice->GetProperty(DIPROP_RANGE, (LPDIPROPHEADER)&dipr);
    if (ResultFailed(hr, TEXT("IDirectInputDevice::GetProperty"))) {
        pDevice->Release();
        return NULL;
    }

    g_lJoyMinY = dipr.lMin;
    g_lJoyMaxY = dipr.lMax;

    // Acquire the joystick
    hr = pDevice->Acquire();
    if (ResultFailed(hr, TEXT("IDirectInputDevice::Acquire"))) {
        pDevice->Release();
        return NULL;
    }

    // Initiate a poll to prepare data for the first GetDeviceState call
    hr = pDevice->Poll();
    if (ResultFailed(hr, TEXT("IDirectInputDevice::Acquire"))) {
        pDevice->Release();
        return NULL;
    }

    // We're now connected and ready to go
    return pDevice;
}

#else 

//******************************************************************************
static HANDLE CreateInputDevice(int nPort) {

    XINPUT_POLLING_PARAMETERS   ipp;
    DWORD                       dwInputDevices;
    UINT                        uPorts, i;
    HANDLE                      hDevice = NULL;

    memset(&ipp, 0, sizeof(XINPUT_POLLING_PARAMETERS));
    ipp.fAutoPoll = 0;
    ipp.fInterruptOut = 0; // ##REVIEW: IS THIS THE BEST CHOICE??
    ipp.bInputInterval = 8;
    ipp.bOutputInterval = 8;

    dwInputDevices = XGetDevices(XDEVICE_TYPE_GAMEPAD);
    if (!dwInputDevices) {  // Save time and drop out now if no joysticks are plugged in
        return NULL;
    }

    if (nPort >= 0) {
        hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, nPort, 0, &ipp);
    }

    else {

        uPorts = XGetPortCount();

        for (i = 0; i < uPorts; i++) {
            if (dwInputDevices & (1 << i)) {
                hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, 0, &ipp);
                if (hDevice != NULL) {
                    break;
                }
            }
        }
    }

    if (hDevice != NULL) {
        // Initiate a poll to prepare data for the first GetDeviceState call
        XInputPoll(hDevice);
    }

    return hDevice;
}

#endif // UNDER_XBOX

//******************************************************************************
//
// Function:
//
//     ReleaseInput
//
// Description:
//
//     Release all input objects created by the InitDirectInput function.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseInput(void) {

#ifndef UNDER_XBOX

    if (g_pdid) {
        g_pdid->Release();
        g_pdid = NULL;
    }

    if (g_pdi) {
        g_pdi->Release();
        g_pdi = NULL;
    }

#else

    if (g_hDevice != NULL) {
        XInputClose(g_hDevice);
        g_hDevice = NULL;
    }

#endif // UNDER_XBOX
}

//******************************************************************************
//
// Function:
//
//     GetJoystickState
//
// Description:
//
//     Poll the joystick to obtain the state of its buttons and axes.
//
// Arguments:
//
//     LPDIJOYSTATE pdijs       - Pointer to a structure describing the state 
//                                of the joystick
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL GetJoystickState(PJOYSTATE pjs) {

#ifndef UNDER_XBOX

    DIJOYSTATE  dijs;
    static LONG lJoyMaxX = (g_lJoyMaxX - g_lJoyMinX) / 2;
    static LONG lJoyCenterX = g_lJoyMinX + lJoyMaxX;
    static LONG lJoyMaxY = (g_lJoyMaxY - g_lJoyMinY) / 2;
    static LONG lJoyCenterY = g_lJoyMinY + lJoyMaxY;
    UINT        i;
    HRESULT     hr;

    if (!pjs) {
        return FALSE;
    }

    if (!g_pdid) {

        memset(pjs, 0, sizeof(JOYSTATE));

        if (!g_pdi) {
            return FALSE;
        }

#ifdef UNDER_CE
        // Check to see if a joystick has been plugged in since we last checked
        g_pdid = CreateDevice();
        if (g_pdid) {
            g_pdid->GetDeviceState(sizeof(DIJOYSTATE), &dijs);
            g_pdid->Poll();
        }
        return FALSE;
#else
        return FALSE;
#endif // UNDER_CE
    }

    memset(&dijs, 0, sizeof(DIJOYSTATE));

    // Get the joystick data
    hr = g_pdid->GetDeviceState(sizeof(DIJOYSTATE), &dijs);
    if (FAILED(hr)) {
        if (hr == DIERR_UNPLUGGED) {
            // The joystick has been unplugged, delete the device since
            // we'll have to recreate it once the joystick is plugged
            // back in
            g_pdid->Release();
            g_pdid = NULL;
            return GetJoystickState(pjs);
        }
        ResultFailed(hr, TEXT("IDirectInputDevice::GetDeviceState"));
        return FALSE;
    }

    // Update the joystick for next time
    hr = g_pdid->Poll();
    if (hr == DIERR_UNPLUGGED) {
        g_pdid->Release();
        g_pdid = NULL;
        return GetJoystickState(pjs);
    }
    else if (hr == DIERR_INPUTLOST) {
        hr = g_pdid->Acquire();
        if (ResultFailed(hr, TEXT("IDirectInputDevice::Acquire"))) {
            return FALSE;
        }
    }
    else {
        if (ResultFailed(hr, TEXT("IDirectInputDevice::Poll"))) {
            return FALSE;
        }
    }

    // Translate the joystick information
    pjs->f1X = (float)(dijs.lX - lJoyCenterX) / (float)(lJoyMaxX);
    pjs->f1Y = (float)(dijs.lY - lJoyCenterY) / (float)(lJoyMaxY);
    pjs->f2Y = (float)dijs.lRz - (float)0x7F7C;
    if (pjs->f2Y < 0.0f) pjs->f2Y /= (float)0x7F7C;
    else pjs->f2Y /= (float)0x8083;
    for (i = 0; i < NUM_JOYSTICK_BUTTONS; i++) {
        pjs->buttons[i] = dijs.rgbButtons[g_rgbButtons[i]];
    }
    switch (dijs.rgdwPOV[0]) {
        case 0x0:
            pjs->buttons[JOYBUTTON_UP] = 0xFF;
            break;
        case 0x1194:
            pjs->buttons[JOYBUTTON_UP] = 0xFF;
            pjs->buttons[JOYBUTTON_RIGHT] = 0xFF;
            break;
        case 0x2328:
            pjs->buttons[JOYBUTTON_RIGHT] = 0xFF;
            break;
        case 0x34bc:
            pjs->buttons[JOYBUTTON_DOWN] = 0xFF;
            pjs->buttons[JOYBUTTON_RIGHT] = 0xFF;
            break;
        case 0x4650:
            pjs->buttons[JOYBUTTON_DOWN] = 0xFF;
            break;
        case 0x57e4:
            pjs->buttons[JOYBUTTON_LEFT] = 0xFF;
            pjs->buttons[JOYBUTTON_DOWN] = 0xFF;
            break;
        case 0x6978:
            pjs->buttons[JOYBUTTON_LEFT] = 0xFF;
            break;
        case 0x7B0C:
            pjs->buttons[JOYBUTTON_UP] = 0xFF;
            pjs->buttons[JOYBUTTON_LEFT] = 0xFF;
            break;
    }

//##HACK to work around a sidewinder bug where the lX and lY values of
// DIJOYSTATE change for an unmoving joystick after any of the buttons
// have been pressed
//#define FABS(x) (x < 0.0f ? -x : x)
if (FABS(pjs->f1X) < 0.01) pjs->f1X = 0.0f;
if (FABS(pjs->f1Y) < 0.01) pjs->f1Y = 0.0f;
if (FABS(pjs->f2X) < 0.01) pjs->f2X = 0.0f;
if (FABS(pjs->f2Y) < 0.01) pjs->f2Y = 0.0f;

    return TRUE;

#else
    
    XINPUT_STATE istate;
    DWORD        dwResult;

    memset(pjs, 0, sizeof(JOYSTATE));

    if (!g_hDevice) {
        g_hDevice = CreateInputDevice();
        return FALSE;
    }

    dwResult = XInputGetState(g_hDevice, &istate);
    if (dwResult != ERROR_SUCCESS) {
        if (dwResult == ERROR_DEVICE_NOT_CONNECTED) {
            XInputClose(g_hDevice);
            g_hDevice = CreateInputDevice();
        }
        return FALSE;
    }

    XInputPoll(g_hDevice);

    pjs->f1X =  (float)istate.Gamepad.sThumbLX / 32768.0f;
    pjs->f1Y = -(float)istate.Gamepad.sThumbLY / 32768.0f;
    pjs->f2X =  (float)istate.Gamepad.sThumbRX / 32768.0f;
    pjs->f2Y = -(float)istate.Gamepad.sThumbRY / 32768.0f;

    pjs->buttons[JOYBUTTON_X] = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X];
    pjs->buttons[JOYBUTTON_Y] = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y];
    pjs->buttons[JOYBUTTON_WHITE] = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE];
    pjs->buttons[JOYBUTTON_A] = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A];
    pjs->buttons[JOYBUTTON_B] = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B];
    pjs->buttons[JOYBUTTON_BLACK] = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK];
    pjs->buttons[JOYBUTTON_LTRIG] = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
    pjs->buttons[JOYBUTTON_RTRIG] = istate.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER];

    if (istate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) {
        pjs->buttons[JOYBUTTON_LSTICK] = 0xFF;
    }
    if (istate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
        pjs->buttons[JOYBUTTON_RSTICK] = 0xFF;
    }
    if (istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
        pjs->buttons[JOYBUTTON_UP] = 0xFF;
    }
    if (istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
        pjs->buttons[JOYBUTTON_DOWN] = 0xFF;
    }
    if (istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
        pjs->buttons[JOYBUTTON_LEFT] = 0xFF;
    }
    if (istate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
        pjs->buttons[JOYBUTTON_RIGHT] = 0xFF;
    }
    if (istate.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
        pjs->buttons[JOYBUTTON_BACK] = 0xFF;
    }
    if (istate.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
        pjs->buttons[JOYBUTTON_START] = 0xFF;
    }

    pjs->f1X *= 1.2f;
    pjs->f1Y *= 1.2f;
    pjs->f2X *= 1.2f;
    pjs->f2Y *= 1.2f;
    if (pjs->f1X > 0.0f) {
        pjs->f1X -= 0.2f; 
        if (pjs->f1X < 0.0f) {
            pjs->f1X = 0.0f; 
        }
    }
    else {
        pjs->f1X += 0.2f;
        if (pjs->f1X > 0.0f) {
            pjs->f1X = 0.0f; 
        }
    }
    if (pjs->f1Y > 0.0f) {
        pjs->f1Y -= 0.2f; 
        if (pjs->f1Y < 0.0f) {
            pjs->f1Y = 0.0f; 
        }
    }
    else {
        pjs->f1Y += 0.2f;
        if (pjs->f1Y > 0.0f) {
            pjs->f1Y = 0.0f; 
        }
    }
    if (pjs->f2X > 0.0f) {
        pjs->f2X -= 0.2f; 
        if (pjs->f2X < 0.0f) {
            pjs->f2X = 0.0f; 
        }
    }
    else {
        pjs->f2X += 0.2f;
        if (pjs->f2X > 0.0f) {
            pjs->f2X = 0.0f; 
        }
    }
    if (pjs->f2Y > 0.0f) {
        pjs->f2Y -= 0.2f; 
        if (pjs->f2Y < 0.0f) {
            pjs->f2Y = 0.0f; 
        }
    }
    else {
        pjs->f2Y += 0.2f;
        if (pjs->f2Y > 0.0f) {
            pjs->f2Y = 0.0f; 
        }
    }

    return TRUE;

#endif // UNDER_XBOX
}


