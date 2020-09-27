/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    input.h

Author:

    Matt Bronder

Description:

    DirectInput initialization routines.

*******************************************************************************/

#ifndef __INPUT_H__
#define __INPUT_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define NUM_JOYSTICK_BUTTONS     17

#define JOYBUTTON_X              0  // Analog
#define JOYBUTTON_Y              1  // Analog
#define JOYBUTTON_WHITE          2  // Analog
#define JOYBUTTON_A              3  // Analog
#define JOYBUTTON_B              4  // Analog
#define JOYBUTTON_BLACK          5  // Analog
#define JOYBUTTON_LTRIG          6  // Analog
#define JOYBUTTON_RTRIG          7  // Analog
#define JOYBUTTON_LSTICK         9  // Digital
#define JOYBUTTON_RSTICK         10 // Digital
#define JOYBUTTON_UP             11 // Digital
#define JOYBUTTON_DOWN           12 // Digital
#define JOYBUTTON_LEFT           13 // Digital
#define JOYBUTTON_RIGHT          14 // Digital
#define JOYBUTTON_BACK           15 // Digital
#define JOYBUTTON_START          16 // Digital

//******************************************************************************
// Macros
//******************************************************************************

#define BUTTON_PRESSED(joystate, button) \
                                (joystate.buttons[button] & 0x80)

#define BUTTON_CHANGED(joycurrent, joylast, button) \
                                ((joycurrent.buttons[button] & 0x80) != (joylast.buttons[button] & 0x80))

//******************************************************************************
// Data types
//******************************************************************************

typedef struct _JOYSTATE {
    float                       f1X; // Analog joystick 1 (left joystick)
    float                       f1Y;
    float                       f2X; // Analog joystick 2 (right joystick)
    float                       f2Y;
    BYTE                        buttons[NUM_JOYSTICK_BUTTONS];
} JOYSTATE, *PJOYSTATE;

//******************************************************************************
// Function prototypes
//******************************************************************************

BOOL                            CreateInput(HWND hWnd);
void                            ReleaseInput(void);
BOOL                            GetJoystickState(PJOYSTATE pjs);

#endif //__INPUT_H__
