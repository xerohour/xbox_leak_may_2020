/*++

Copyright (c) 2001  Microsoft Corporation. All rights reserved.

Module Name:

    input.cpp

Abstract:

    WMV playback sample application - input functions

--*/

#include "wmvplay.h"
#include "xboxp.h"

//
// IR remote key codes
//
#define IRKEY_POWER     0x0AD5
#define IRKEY_REVERSE   0x0AE2
#define IRKEY_PLAY      0x0AEA
#define IRKEY_FORWARD   0x0AE3
#define IRKEY_SKIPBACK  0x0ADD
#define IRKEY_STOP      0x0AE0
#define IRKEY_PAUSE     0x0AE6
#define IRKEY_SKIPFWD   0x0ADF
#define IRKEY_GUIDE     0x0AE5
#define IRKEY_INFO      0x0AC3
#define IRKEY_MENU      0x0AF7
#define IRKEY_BACK      0x0AD8
#define IRKEY_UP        0x0AA6
#define IRKEY_LEFT      0x0AA9
#define IRKEY_SELECT    0x0A0B
#define IRKEY_RIGHT     0x0AA8
#define IRKEY_DOWN      0x0AA7
#define IRKEY_0         0x0ACF
#define IRKEY_1         0x0ACE
#define IRKEY_2         0x0ACD
#define IRKEY_3         0x0ACC
#define IRKEY_4         0x0ACB
#define IRKEY_5         0x0ACA
#define IRKEY_6         0x0AC9
#define IRKEY_7         0x0AC8
#define IRKEY_8         0x0AC7
#define IRKEY_9         0x0AC6
#define IRKEY_NONE      0xFFFF

enum {
    BUTTON_ANALOG,
    BUTTON_A = BUTTON_ANALOG,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_BLACK,
    BUTTON_WHITE,
    BUTTON_LTRIGGER,
    BUTTON_RTRIGGER,

    BUTTON_DIGITAL,
    BUTTON_UP = BUTTON_DIGITAL,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_START,
    BUTTON_BACK,
    BUTTON_LTHUMB,
    BUTTON_RTHUMB,

    BUTTON_MAX
};

//
// Handle input
//
VOID ButtonPressed(INT button)
{
    switch (button) {
    case BUTTON_X:
        //
        // Toggle fullscreen mode
        //
        FullScreenMode = !FullScreenMode;
        VERBOSE("Full screen mode: %d\n", FullScreenMode);
        break;

    case BUTTON_B:
        //
        // Switch between D and A drive
        //
        TestDVDFiles = !TestDVDFiles;
        TestFileIndex = -1;

        // Fall through

    case BUTTON_START:
        //
        // Move on to the next test file
        //
        TestFileIndex++;
        AbortDecoding();
        break;

    case BUTTON_BACK:
        //
        // Move back to the previous test file
        //
        if (TestFileIndex > 0) {
            TestFileIndex--;
            AbortDecoding();
        }
        break;

    case BUTTON_A:
        //
        // Replay the current file
        //
        AbortDecoding();
        break;

    case BUTTON_Y:
        //
        // Toggle the continuous play mode
        //
        ContinuousMode = !ContinuousMode;
        VERBOSE("Continuous play mode: %d\n", ContinuousMode);
        break;

    case BUTTON_BLACK:
        //
        // Stop the test
        //
        DecodingFlag = DECODING_EXIT;
        break;
    }
}

VOID NumberPressed(INT n)
{
    TestFileIndex = n;
    AbortDecoding();
}

//
// Map IR remote keys to equivalent controller buttons
//
INT MapIRKey(WORD keycode)
{
    static const struct {
        WORD keycode;
        WORD button;
    } mappingTable[] = {
        { IRKEY_SELECT,     BUTTON_A },
        { IRKEY_PLAY,       BUTTON_A },
        { IRKEY_BACK,       BUTTON_B },
        { IRKEY_SKIPFWD,    BUTTON_START },
        { IRKEY_SKIPBACK,   BUTTON_BACK },
        { IRKEY_STOP,       BUTTON_BLACK },
        { IRKEY_UP,         BUTTON_UP },
        { IRKEY_DOWN,       BUTTON_DOWN },
        { IRKEY_LEFT,       BUTTON_LEFT },
        { IRKEY_RIGHT,      BUTTON_RIGHT },
        { IRKEY_GUIDE,      BUTTON_Y },
        { IRKEY_MENU,       BUTTON_X },

        { IRKEY_NONE,       BUTTON_MAX },
    };

    INT i = 0;
    while (mappingTable[i].keycode != IRKEY_NONE) {
        if (keycode == mappingTable[i].keycode)
            return mappingTable[i].button;
        i++;
    }

    return -1;
}

//
// Poll controllers for input
//
#define MAXDEVICES 4

HANDLE controllers[MAXDEVICES];
HANDLE irdevices[MAXDEVICES];

struct {
    DWORD lastPacketNumber;
    DWORD buttons;
} ctlrState[MAXDEVICES];

struct {
    DWORD lastPacketNumber;
    WORD lastKeyCode;
    WORD lastKeyTime;
} irState[MAXDEVICES];

VOID PollInput()
{
    DWORD insertions, removals;
    INT i, j;

    //
    // Check if any controller device has been added or removed
    //
    if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &insertions, &removals)) {
        for (i=0; i < MAXDEVICES; i++) {
            if ((removals & (1 << i)) && controllers[i]) {
                XInputClose(controllers[i]);
                controllers[i] = NULL;
            }
        }

        for (i=0; i < MAXDEVICES; i++) {
            if (insertions & (1 << i)) {
                controllers[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL);
                ctlrState[i].lastPacketNumber = -1;
                ctlrState[i].buttons = 0;
            }
        }
    }

    //
    // Check for IR remote devices
    //
    if (XGetDeviceChanges(XDEVICE_TYPE_IR_REMOTE, &insertions, &removals)) {
        for (i=0; i < MAXDEVICES; i++) {
            if ((removals & (1 << i)) && irdevices[i]) {
                XInputClose(irdevices[i]);
                irdevices[i] = NULL;
            }
        }

        for (i=0; i < MAXDEVICES; i++) {
            if (insertions & (1 << i)) {
                irdevices[i] = XInputOpen(XDEVICE_TYPE_IR_REMOTE, i, XDEVICE_NO_SLOT, NULL);
                irState[i].lastPacketNumber = -1;
                irState[i].lastKeyCode = IRKEY_NONE;
                irState[i].lastKeyTime = 0;
            }
        }

    }

    //
    // Poll controller inputs
    //
    for (i=0; i < MAXDEVICES; i++) {
        if (controllers[i] == NULL) continue;

        XINPUT_STATE states;
        DWORD err = XInputGetState(controllers[i], &states);
        if (err != ERROR_SUCCESS) continue;

        if (states.dwPacketNumber != ctlrState[i].lastPacketNumber) {
            DWORD buttons = 0;

            // Figure out which analog buttons are pressed
            for (j=0; j < 8; j++) {
                if (states.Gamepad.bAnalogButtons[j] >= 0x20)
                    buttons |= (1 << (BUTTON_ANALOG+j));
            }

            // Figure out which digital buttons are pressed
            for (j=0; j < 8; j++) {
                if (states.Gamepad.wButtons & (1 << j))
                    buttons |= (1 << (BUTTON_DIGITAL+j));
            }

            // NOTE: We're only interested in button-down event, i.e.
            // the previous state is up and the current state is down
            for (j=0; j < BUTTON_MAX; j++) {
                if ((ctlrState[i].buttons & (1 << j)) == 0 &&
                    (buttons & (1 << j)) != 0) {
                    ButtonPressed(j);
                }
            }
            ctlrState[i].buttons = buttons;
        }
        ctlrState[i].lastPacketNumber = states.dwPacketNumber;
    }

    //
    // Poll IR remote inputs
    //
    for (i=0; i < MAXDEVICES; i++) {
        if (irdevices[i] == NULL) continue;

        XINPUT_STATE_INTERNAL states;
        DWORD err = XInputGetState(irdevices[i], (XINPUT_STATE*) &states);
        if (err != ERROR_SUCCESS) continue;

        if (states.dwPacketNumber != irState[i].lastPacketNumber) {
            XINPUT_IR_REMOTE* irstate = &states.IrRemote;
            BOOL newkey = TRUE;

            if (irstate->wKeyCode != irState[i].lastKeyCode ||
                irstate->wTimeDelta >= 100) {
                //
                // If the new key is different from the old one,
                // or if the new key is not an auto-repeat
                //
                irState[i].lastKeyCode = irstate->wKeyCode;
                irState[i].lastKeyTime = 0;
            } else {
                //
                // If the new key is an auto-repeat,
                // we'll limit the repeat rate to a max of twice per second.
                //
                irState[i].lastKeyTime += irstate->wTimeDelta;
                if (irState[i].lastKeyTime >= 500)
                    irState[i].lastKeyTime %= 500;
                else
                    newkey = FALSE;
            }

            if (newkey) {
                if (irState[i].lastKeyCode >= IRKEY_9 && irState[i].lastKeyCode <= IRKEY_0) {
                    j = 9 - (irState[i].lastKeyCode - IRKEY_9);
                    NumberPressed(j);
                } else {
                    j = MapIRKey(irState[i].lastKeyCode);
                    if (j >= 0) {
                        ButtonPressed(j);
                    }
                }
            }
        }

        irState[i].lastPacketNumber = states.dwPacketNumber;
    }
}

