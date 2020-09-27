//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#pragma once
// those data structures are defined in Xbox.h, 
// cannot include the XBox.h due to various data types conflicts
/////////////////// XBOX definitions 
typedef struct _XINPUT_GAMEPAD
{
    WORD    wButtons;
    BYTE    bAnalogButtons[8];
    SHORT   sThumbLX;
    SHORT   sThumbLY;
    SHORT   sThumbRX;
    SHORT   sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

#define XINPUT_GAMEPAD_DPAD_UP          0x00000001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
#define XINPUT_GAMEPAD_START            0x00000010
#define XINPUT_GAMEPAD_BACK             0x00000020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080

#define XINPUT_GAMEPAD_A                0
#define XINPUT_GAMEPAD_B                1
#define XINPUT_GAMEPAD_X                2
#define XINPUT_GAMEPAD_Y                3
#define XINPUT_GAMEPAD_BLACK            4
#define XINPUT_GAMEPAD_WHITE            5
#define XINPUT_GAMEPAD_LEFT_TRIGGER     6
#define XINPUT_GAMEPAD_RIGHT_TRIGGER    7

#define XINPUT_GAMEPAD_MAX_CROSSTALK	30

typedef struct _XINPUT_RUMBLE
{
   WORD   wLeftMotorSpeed;
   WORD   wRightMotorSpeed;
} XINPUT_RUMBLE, *PXINPUT_RUMBLE;

typedef struct _XINPUT_STATE
{
    DWORD dwPacketNumber;
    union
    {
        XINPUT_GAMEPAD Gamepad;
    };
} XINPUT_STATE, *PXINPUT_STATE;

// HACK, HACK, HACK!! - complete wierdness - when XINPUT_FEEDBACK_HEADER_INTERNAL_SIZE = 58 
// as in the Xbox.h, the buffer is padded to 60, changed to be 56+2 is the expected size
#define XINPUT_FEEDBACK_HEADER_INTERNAL_SIZE 56
typedef struct _XINPUT_FEEDBACK_HEADER
{
    DWORD           dwStatus;
    HANDLE  hEvent;
    BYTE            Reserved[XINPUT_FEEDBACK_HEADER_INTERNAL_SIZE];
} XINPUT_FEEDBACK_HEADER, *PXINPUT_FEEDBACK_HEADER;

typedef struct _XINPUT_FEEDBACK
{
    XINPUT_FEEDBACK_HEADER Header;
    union
    {
      XINPUT_RUMBLE Rumble;
    };
} XINPUT_FEEDBACK, *PXINPUT_FEEDBACK;

typedef struct _XINPUT_CAPABILITIES
{
    BYTE    SubType;
    WORD    Reserved;
    union
    {
      XINPUT_GAMEPAD Gamepad;
    } In;
    union
    {
      XINPUT_RUMBLE Rumble;
    } Out;
} XINPUT_CAPABILITIES, *PXINPUT_CAPABILITIES;

////////////////////// End of XBox.h include

// here is XBGAMEPAD structure from XBINput.h
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

// This the AUTOMATION message definition
// Currently two messages are expected
// m_byMessageId = 1 - Gamepad to simulate user input
// m_byMessageId = 2 - Heartbit message , if no traffic inspected by the server during 1 minute, 
// the connection will be terminated by the server. 
// Client should send one of those two message at least once in a minute
struct AutoMessage
{
	char m_byMessageId;
	union Data
	{
		XBGAMEPAD xbGamepad;
	} m_Data;
};
