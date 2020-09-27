#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <xtl.h>

//
// Deadzone for thumbsticks
//
#define XBINPUT_DEADZONE 0.24f

//-----------------------------------------------------------------------------
// Name: struct XBGAMEPAD
// Desc: structure for holding Gamepad data
//-----------------------------------------------------------------------------
struct XBGAMEPAD : public XINPUT_GAMEPAD
{
    // The following members are inherited from XINPUT_GAMEPAD:
    //    WORD    wButtons;
    //    BYTE    bAnalogButtons[8];
    //    SHORT   sThumbLX;
    //    SHORT   sThumbLY;
    //    SHORT   sThumbRX;
    //    SHORT   sThumbRY;

    // Thumb stick values converted to range [-1,+1]
    FLOAT      fX1;
    FLOAT      fY1;
    FLOAT      fX2;
    FLOAT      fY2;
    
    // State of buttons tracked since last poll
    WORD       wLastButtons;
    BOOL       bLastAnalogButtons[8];
    WORD       wPressedButtons;
    BOOL       bPressedAnalogButtons[8];

    // Rumble properties
    XINPUT_RUMBLE   Rumble;
    XINPUT_FEEDBACK Feedback;

    // Device properties
    XINPUT_CAPABILITIES caps;
    HANDLE     hDevice;

    // Flags for whether gamepad was just inserted or removed
    BOOL       bInserted;
    BOOL       bRemoved;
};

//
// Global instance of gamepad devices
//
extern XBGAMEPAD g_Gamepads[4];

HRESULT InitInput(void);
bool PollControllers(void);

#endif