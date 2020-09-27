/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    DeviceDump.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    05-30-2000  Created

Notes:

*/

#include "stdafx.h"
#include "commontest.h"
#include <initguid.h>
#include <dinput.h>
#include <input.h>

#define MAX_DEVICES 64

BOOL CALLBACK DeviceInsert_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
BOOL CALLBACK DeviceRemove_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

class USBDevice
    {
    public:
        DWORD id;
        IDirectInput8 *pDI;
        IDirectInputDevice8 *pDID;
        LPCDIDEVICEINSTANCE devInstance;
    public:
        USBDevice();
    };

USBDevice::USBDevice()
    {
    id = 0;
    pDI = NULL;
    pDID = NULL;
    devInstance = NULL;
    }

struct EnumCallbackState
    {
    DWORD threadID;
    DWORD numFound;
    IDirectInput8 *pDI;
    USBDevice *devices;
    };


BOOL HasInput(DIXBOXCONTROLLERSTATE *data1, DIXBOXCONTROLLERSTATE *data2)
    {
    if(memcmp(data1, data2, sizeof(DIXBOXCONTROLLERSTATE)) == 0) return FALSE;

    return TRUE;
    }


void DumpControllerState(DWORD threadID, unsigned ID, DIXBOXCONTROLLERSTATE *controllerState)
    {
    DebugPrint("DITEST(%d): Input from %08X-0000-0000-0000-000000000000:\n"
        "   Reserved  = 0x%0.4X\n"
        "   dButtons  = 0x%0.4X\n"
        "   aButtons  = (%02X %02X %02X %02X %02X %02X %02X %02X)\n"
        "   DPAD      = 0x%0.4X\n"
        "   Thumb1    = (0x%0.4X, 0x%0.4X)\n"
        "   Thumb2    = (0x%0.4X, 0x%0.4X)\n"
        "   Tilt      = (0x%0.4X, 0x%0.4X)\n"
        ,
        threadID,
        ID,
        controllerState->Reserved,
        controllerState->bmButtons,
        controllerState->rgbAnalogButtons[0],
        controllerState->rgbAnalogButtons[1],
        controllerState->rgbAnalogButtons[2],
        controllerState->rgbAnalogButtons[3],
        controllerState->rgbAnalogButtons[4],
        controllerState->rgbAnalogButtons[5],
        controllerState->rgbAnalogButtons[6],
        controllerState->rgbAnalogButtons[7],
        controllerState->wDPAD,
        controllerState->wThumb1X,
        controllerState->wThumb1Y,
        controllerState->wThumb2X,
        controllerState->wThumb2Y,
        controllerState->wTiltX,
        controllerState->wTiltY
        );
    }

/*

Routine Description:

    

Arguments:

    

Return Value:

    

*/
DWORD DeviceDump_Test(HANDLE hLog, DWORD ThreadID)
    {
    HRESULT result;
    IDirectInput8 *pDI = NULL;

    USBDevice devices[MAX_DEVICES];
    EnumCallbackState state = { 0, 0, 0, devices };
	DIXBOXCONTROLLERSTATE controllerState[MAX_DEVICES][2];
    unsigned cs = 1;
    unsigned i;
    unsigned exitState = 0;
    DWORD time = GetTickCount();

    //__asm int 3;

    DebugPrint("DEVDUMP(%d): Creating DirectInput\n", ThreadID);
    result = DirectInput8Create(NULL, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDI, NULL);
    if(result != DI_OK)
        {
        DebugPrint("DEVDUMP(%d): Creating DirectInput Failed (ec: %08X)\n", ThreadID, result);
        return -1;
        }
    state.pDI = pDI;

    // Enumerate Devices
    DebugPrint("DEVDUMP(%d): Enumerating devices\n\n\n", ThreadID);
    result = pDI->EnumDevices(0, DeviceInsert_Callback, (void*)&state, DIEDFL_ENUMANDNOTIFYINSERT);
    result = pDI->EnumDevices(0, DeviceRemove_Callback, (void*)&state, DIEDFL_NOTIFYREMOVE);

    DebugPrint("DITEST(%d): Device Watch...\n", ThreadID);
    DebugPrint("*************************************\n");
    DebugPrint("* Press A B A A on Device 0 to exit *\n");
    DebugPrint("*************************************\n");

    // Loop
    while(exitState < 8)
        {
        Sleep(8);
        InputPumpCallbacks();
        cs = !cs;

        for(i=0; i<MAX_DEVICES; i++)
            {
            if(devices[i].pDID)
                {
                devices[i].pDID->GetDeviceState(sizeof(DIXBOXCONTROLLERSTATE), &controllerState[i][cs]);

                // check to see if the exit combo (abaa) has been pressed
                if(i == 0 && HasInput(&controllerState[i][cs], &controllerState[i][!cs]))
                    {
                    if(exitState == 0 && controllerState[i][cs].rgbAnalogButtons[0] > 0) ++exitState;
                    else if(exitState == 1 && controllerState[i][cs].rgbAnalogButtons[0] < 10) ++exitState;
                    else if(exitState == 2 && controllerState[i][cs].rgbAnalogButtons[1] > 0) ++exitState;
                    else if(exitState == 3 && controllerState[i][cs].rgbAnalogButtons[1] < 10) ++exitState;
                    else if(exitState == 4 && controllerState[i][cs].rgbAnalogButtons[0] > 0) ++exitState;
                    else if(exitState == 5 && controllerState[i][cs].rgbAnalogButtons[0] < 10) ++exitState;
                    else if(exitState == 6 && controllerState[i][cs].rgbAnalogButtons[0] > 0) ++exitState;
                    else if(exitState == 7 && controllerState[i][cs].rgbAnalogButtons[0] < 10) ++exitState;
                    else exitState = 0;
                    }

                // Dump out the device state if it has changed
                if(HasInput(&controllerState[i][cs], &controllerState[i][!cs]))
                    {
                    DumpControllerState(ThreadID, i, &controllerState[i][cs]);
                    }
                }
            }
        }

    // End Enumeration
    DebugPrint("\n\nDEVDUMP(%d): Halting Enumeration\n", ThreadID);
    result = pDI->EnumDevices(0, DeviceInsert_Callback, (void*)&state, DIEDFL_HALTNOTIFYINSERT);
    result = pDI->EnumDevices(0, DeviceRemove_Callback, (void*)&state, DIEDFL_HALTNOTIFYREMOVE);

    // Clean up opened devices
    DebugPrint("DEVDUMP(%d): Cleaning up opened devices\n", ThreadID);
    for(i=0; i<MAX_DEVICES; i++)
        {
        if(devices[i].pDID)
            {
            devices[i].pDID->Unacquire();
            devices[i].pDID->Release();
            }
        }

    // Close DirectInput
    pDI->Release();

    return 0;
    }



BOOL CALLBACK DeviceInsert_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
    {
    if(!pvRef)
        {
        DebugPrint("DITEST(-): Found Device: but state is NULL\n");
        return DIENUM_CONTINUE;
        }

    DIDEVCAPS diDevCaps;
    DIPROPDWORD dipdw;
    diDevCaps.dwSize = sizeof(DIDEVCAPS);
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = true;

    EnumCallbackState *state = (EnumCallbackState*)pvRef;

    if(lpddi->guidInstance.Data1 >= MAX_DEVICES)
        {
        DebugPrint("DITEST(%d): Found Device but not saving it because id is too high\n", state->threadID);
        return DIENUM_CONTINUE;
        }
    ++state->numFound;

    state->pDI->CreateDevice(lpddi->guidInstance, (IDirectInputDevice8**)&(state->devices[lpddi->guidInstance.Data1].pDID), NULL);
    if(!state->devices[lpddi->guidInstance.Data1].pDID)
        {
        DebugPrint("DITEST(-): Found Device: but unable to Create it\n");
        return DIENUM_CONTINUE;
        }
    state->devices[lpddi->guidInstance.Data1].pDI = state->pDI;
    state->devices[lpddi->guidInstance.Data1].pDID->SetProperty(DIPROP_AUTOPOLL, &dipdw.diph);
    state->devices[lpddi->guidInstance.Data1].pDID->Acquire();
    state->devices[lpddi->guidInstance.Data1].devInstance = lpddi;

    DebugPrint("DITEST(%d): Found Device:\n"
        "   Device Type:   %X\n"
        "   Instance GUID: %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"
        "   Product GUID:  %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"
        ,
        state->threadID, lpddi->dwDevType, 
        lpddi->guidInstance.Data1, lpddi->guidInstance.Data2, lpddi->guidInstance.Data3, lpddi->guidInstance.Data4[0], lpddi->guidInstance.Data4[1], lpddi->guidInstance.Data4[2], lpddi->guidInstance.Data4[3], lpddi->guidInstance.Data4[4], lpddi->guidInstance.Data4[5], lpddi->guidInstance.Data4[6], lpddi->guidInstance.Data4[7],
        lpddi->guidProduct.Data1, lpddi->guidProduct.Data2, lpddi->guidProduct.Data3, lpddi->guidProduct.Data4[0], lpddi->guidProduct.Data4[1], lpddi->guidProduct.Data4[2], lpddi->guidProduct.Data4[3], lpddi->guidProduct.Data4[4], lpddi->guidProduct.Data4[5], lpddi->guidProduct.Data4[6], lpddi->guidProduct.Data4[7]
        );

    return DIENUM_CONTINUE;
    }

BOOL CALLBACK DeviceRemove_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
    {
    if(!pvRef)
        {
        DebugPrint("DITEST(-): Removing Device: but state is NULL\n");
        return DIENUM_CONTINUE;
        }

    EnumCallbackState *state = (EnumCallbackState*)pvRef;

    if(lpddi->guidInstance.Data1 >= MAX_DEVICES)
        {
        DebugPrint("DITEST(%d): Found Device but not saving it because id is too high\n", state->threadID);
        return DIENUM_CONTINUE;
        }
    --state->numFound;

    if(state->devices[lpddi->guidInstance.Data1].pDID == NULL)
        {
        DebugPrint("DITEST(-): Removing Device: but pDID is NULL\n");
        return DIENUM_CONTINUE;
        }

    state->devices[lpddi->guidInstance.Data1].pDID->Unacquire();
    state->devices[lpddi->guidInstance.Data1].pDID->Release();
    state->devices[lpddi->guidInstance.Data1].pDI = NULL;
    state->devices[lpddi->guidInstance.Data1].pDID = NULL;
    state->devices[lpddi->guidInstance.Data1].devInstance = NULL;

    DebugPrint("DITEST(%d): Removing Device:\n"
        "   Device Type:   %X\n"
        "   Instance GUID: %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"
        "   Product GUID:  %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"
        ,
        state->threadID, lpddi->dwDevType, 
        lpddi->guidInstance.Data1, lpddi->guidInstance.Data2, lpddi->guidInstance.Data3, lpddi->guidInstance.Data4[0], lpddi->guidInstance.Data4[1], lpddi->guidInstance.Data4[2], lpddi->guidInstance.Data4[3], lpddi->guidInstance.Data4[4], lpddi->guidInstance.Data4[5], lpddi->guidInstance.Data4[6], lpddi->guidInstance.Data4[7],
        lpddi->guidProduct.Data1, lpddi->guidProduct.Data2, lpddi->guidProduct.Data3, lpddi->guidProduct.Data4[0], lpddi->guidProduct.Data4[1], lpddi->guidProduct.Data4[2], lpddi->guidProduct.Data4[3], lpddi->guidProduct.Data4[4], lpddi->guidProduct.Data4[5], lpddi->guidProduct.Data4[6], lpddi->guidProduct.Data4[7]
        );

    return DIENUM_CONTINUE;
    }

