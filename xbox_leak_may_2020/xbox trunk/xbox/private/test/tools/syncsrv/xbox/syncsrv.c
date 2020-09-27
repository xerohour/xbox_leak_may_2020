/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  syncsrv.c

Abstract:

  This module runs a Netsync server

Author:

  Steven Kehrli (steveke) 3-Oct-2000

------------------------------------------------------------------------------*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntos.h>

#include <xtl.h>

#include <netsync.h>



int WINAPI NetsyncMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR szCmdLine, int iCmdShow)
{
    // hDevices is an array of handles to the devices
    HANDLE        hDevices[4];
    // dwCurrentDevice is a counter to enumerate each device
    DWORD         dwCurrentDevice = 0;
    // dwInsertions is a bit mask of device insertions
    DWORD         dwInsertions;
    // dwRemovals is a bit mask of device removals
    DWORD         dwRemovals;
    // XInputState is the current device state
    XINPUT_STATE  XInputState;
    // bXPressed indicates if the X button has been pressed
    BOOL          bXPressed = FALSE;
    // dwLastError is the last error code
    DWORD         dwLastError = ERROR_SUCCESS;

    // Initialize the USB stack
    XInitDevices(0,NULL);

    // Initialize the array of device handles
    ZeroMemory(hDevices, sizeof(HANDLE) * XGetPortCount());

    DbgPrint("Starting the Netsync server...\n");

    if (FALSE == NetsyncCreateServer()) {
        dwLastError = GetLastError();
        DbgPrint("Start failed: 0x%08x\n", dwLastError);
        goto ExitMain;
    }

    DbgPrint("Started.\n\n");
    
    DbgPrint("Press 'X' to stop.\n\n");

    do {
        // Get for hotplug insertions or removals
        if (FALSE == XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals)) {
            dwInsertions = 0;
            dwRemovals = 0;
        }

        for (dwCurrentDevice = 0; dwCurrentDevice < XGetPortCount(); dwCurrentDevice++) {
            // Close the device if it has been removed
            if ((1 << dwCurrentDevice) & dwRemovals) {
                XInputClose(hDevices[dwCurrentDevice]);
                hDevices[dwCurrentDevice] = NULL;
            }

            // Open the device if it has been inserted
            if ((1 << dwCurrentDevice) & dwInsertions) {
                hDevices[dwCurrentDevice] = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + dwCurrentDevice, XDEVICE_NO_SLOT, NULL);
            }

            // Get the current device state
            if (NULL != hDevices[dwCurrentDevice]) {
                XInputGetState(hDevices[dwCurrentDevice], &XInputState);
                if (0 != XInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X]) {
                    bXPressed = TRUE;
                    break;
                }
            }
        }
    } while (FALSE == bXPressed);

    // Close any open devices
    for (dwCurrentDevice = 0; dwCurrentDevice < XGetPortCount(); dwCurrentDevice++) {
        if (NULL != hDevices[dwCurrentDevice]) {
            XInputClose(hDevices[dwCurrentDevice]);
        }
    }

    DbgPrint("Stopping the Netsync server...\n");

    NetsyncCloseServer();

    DbgPrint("Stopped.\n");

ExitMain:
    DbgPrint("Rebooting...\n");

    // Reboot
    HalReturnToFirmware(HalRebootRoutine);

    return dwLastError;
}
