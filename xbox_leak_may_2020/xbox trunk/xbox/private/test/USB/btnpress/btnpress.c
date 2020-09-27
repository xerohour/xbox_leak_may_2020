

#include "bppch.h"

//
// Imported from other modules.
//
VOID DrawMessage(LPCWSTR pcszText, int iProgPercent);
ULONG DebugPrint(PCHAR Format,...);


#define NO_PAD 0xFF

//
// Global variables related to the open gamepad.
//
HANDLE g_Pad = NULL;
UINT g_PadPort = NO_PAD;
PWSTR g_ButtonStrings[] = {
                    L"Button A pressed.",
                    L"Button B pressed.",
                    L"Button X pressed.",
                    L"Button Y pressed.",
                    L"Black button pressed.",
                    L"White button pressed.",
                    L"Left trigger pressed.",
                    L"Right trigger pressed."
                    };



void
pGetInitialGamepad (
    void
    )
{

    DWORD devices;
    int i;
    int c = XGetPortCount ();


    //
    // Get all connected gamepads.
    //
    devices = XGetDevices (XDEVICE_TYPE_GAMEPAD);

    for (i = 0; i < c; i++) {
        if ((1 << i) & devices) {
            //
            // Open the device.
            //
            DebugPrint("Gamepad found on port %u. Opening. \n", i);
            g_Pad = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + i, XDEVICE_NO_SLOT, NULL);
            if (g_Pad) {
                g_PadPort = i;
                DebugPrint ("Gamepad successfully opened. (g_PadPort/g_Pad) (%u/%08x)\n", g_PadPort, g_Pad);
                break;
            }
        }
    }
}

void
pGetGamepadChanges (
    void
    )
{

    DWORD insertions, removals;
    int i;
    int c = XGetPortCount();


    //
    // Get any insertions or removals to gamepads.
    //
    if (XGetDeviceChanges (XDEVICE_TYPE_GAMEPAD, &insertions, &removals)) {
        if (removals && g_PadPort != NO_PAD) {
            if ((1 << g_PadPort) & removals) {

                //
                // our pad removed.
                //
                XInputClose (g_Pad);
                DebugPrint ("Gamepad in use on port %u removed.\n", g_PadPort);

                g_PadPort = NO_PAD;
                g_Pad = NULL;

                //
                // See if there are any otherse connected.
                //
                pGetInitialGamepad ();

            }
        }

        if (insertions && g_PadPort == NO_PAD) {

            for (i = 0; i < c; i++) {
                if ((1 << i) & insertions) {
                    //
                    // Open the device.
                    //
                    DebugPrint ("Gamepad found on port %u. Opening. \n", i);
                    g_Pad = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + i, XDEVICE_NO_SLOT, NULL);
                    if (g_Pad) {
                        g_PadPort = i;
                        DebugPrint ("Gamepad successfully opened. (g_PadPort/g_Pad) (%u/%08x)\n", g_PadPort, g_Pad);
                        break;
                    }
                }
            }
        }
    }

}




void __cdecl main()
{

    XINPUT_STATE state;
    DWORD lastPacket;
    int i;

    //
    //  Initialize core peripheral port support
    //
    XInitDevices(0,NULL);

    //
    // Get any already connected gamepads.
    //
    pGetInitialGamepad ();

    //
    // Keep track of last packet -- this helps us determine if a new packet has been received or if it is the same
    // packet we asked for last time.
    //
    state.dwPacketNumber = 0;
    lastPacket = state.dwPacketNumber;

    while (1) {

        //
        // Look for
        //
        pGetGamepadChanges ();

        if (g_PadPort != NO_PAD && g_Pad) {

            if (ERROR_SUCCESS == XInputGetState (g_Pad, &state) && state.dwPacketNumber != lastPacket) {

                //
                // Update our last packet variable.
                //
                lastPacket = state.dwPacketNumber;

                for (i = 0; i < 8; i++) {

                    if (!state.Gamepad.bAnalogButtons[i]) {
                        continue;
                    }

                    DrawMessage (g_ButtonStrings[i], (state.Gamepad.bAnalogButtons[i] * 100) / 255);
                    break;

                }
                if (i == 8) {
                    DrawMessage (L"No button pressed.", 0);
                }


            }
        }

    }

}

