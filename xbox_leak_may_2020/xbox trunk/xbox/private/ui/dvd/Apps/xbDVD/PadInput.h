////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef __PADMAP_H__
#define __PADMAP_H__

#include <xbInput.h>


// all the different possibilities of gamepad input that we support
enum
{
	XB_NO_INPUT,
	XB_X_BUTTON,
	XB_Y_BUTTON,
	XB_A_BUTTON,
	XB_B_BUTTON,
	XB_BLACK_BUTTON,
	XB_WHITE_BUTTON,
	XB_LEFT_TRIGGER_BUTTON,
	XB_RIGHT_TRIGGER_BUTTON,
	XB_SHIFT_X_BUTTON,
	XB_SHIFT_Y_BUTTON,
	XB_SHIFT_A_BUTTON,
	XB_SHIFT_B_BUTTON,
	XB_SHIFT_BLACK_BUTTON,
	XB_SHIFT_WHITE_BUTTON,
	XB_SHIFT_LEFT_TRIGGER_BUTTON,
	XB_SHIFT_RIGHT_TRIGGER_BUTTON,
	XB_SHIFT_START,
	XB_SHIFT_BACK,
	XB_DPAD_UP,
	XB_DPAD_DOWN,
	XB_DPAD_LEFT,
	XB_DPAD_RIGHT,
	XB_START,
	XB_BACK,
	XB_LTHUMB,
	XB_RTHUMB,
	XB_SHIFT_DPAD_UP,
	XB_SHIFT_DPAD_DOWN,
	XB_SHIFT_DPAD_LEFT,
	XB_SHIFT_DPAD_RIGHT
};


// XBGAMEPAD.wPressedButtons is a newly pressed button. For held buttons we need to check
// XBGAMEPAD.wLastButtons.  THH




// macros to make it easy to see what input has occured
#define HITTEST_XB_DPAD_UP(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wPressedButtons & XINPUT_GAMEPAD_DPAD_UP) > 0 ? TRUE : FALSE )

#define HITTEST_XB_DPAD_DOWN(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) > 0 ? TRUE : FALSE )

#define HITTEST_XB_DPAD_LEFT(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT) > 0 ? TRUE : FALSE )

#define HITTEST_XB_DPAD_RIGHT(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT) > 0 ? TRUE : FALSE )

#define HITTEST_XB_START(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wPressedButtons & XINPUT_GAMEPAD_START ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_BACK(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wPressedButtons & XINPUT_GAMEPAD_BACK ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_LTHUMB(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_RTHUMB(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wPressedButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) > 0 ? TRUE : FALSE )

// test for held thumb (shift to us)
#define HOLDTEST_XB_LTHUMB(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wLastButtons & XINPUT_GAMEPAD_LEFT_THUMB ) > 0 ? TRUE : FALSE )

#define HOLDTEST_XB_RTHUMB(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wLastButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_A(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->bPressedAnalogButtons[XINPUT_GAMEPAD_A] ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_B(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->bPressedAnalogButtons[XINPUT_GAMEPAD_B] ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_X(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_Y(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->bPressedAnalogButtons[XINPUT_GAMEPAD_Y] ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_BLACK(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_WHITE(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_LEFT_TRIGGER(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_RIGHT_TRIGGER(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] ) > 0 ? TRUE : FALSE )

#define HITTEST_XB_SHIFT_X(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_X(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_Y(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_Y(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_A(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_A(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_B(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_B(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_BLACK(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_BLACK(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_WHITE(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_WHITE(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_LEFT_TRIGGER(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_LEFT_TRIGGER(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_RIGHT_TRIGGER(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_RIGHT_TRIGGER(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_START(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_START(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_BACK(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_BACK(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_DPAD_LEFT(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_DPAD_LEFT(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_DPAD_RIGHT(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_DPAD_RIGHT(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_DPAD_UP(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_DPAD_UP(pgamepad)) ? TRUE : FALSE)

#define HITTEST_XB_SHIFT_DPAD_DOWN(pgamepad)  \
	((HOLDTEST_XB_LTHUMB(pgamepad) && HITTEST_XB_DPAD_DOWN(pgamepad)) ? TRUE : FALSE)


// test for held dpad
#define HOLDTEST_XB_DPAD_LEFT(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wLastButtons & XINPUT_GAMEPAD_DPAD_LEFT ) > 0 ? TRUE : FALSE )

#define HOLDTEST_XB_DPAD_RIGHT(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wLastButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) > 0 ? TRUE : FALSE )

#define HOLDTEST_XB_DPAD_UP(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wLastButtons & XINPUT_GAMEPAD_DPAD_UP ) > 0 ? TRUE : FALSE )

#define HOLDTEST_XB_DPAD_DOWN(pgamepad)  \
	((((XBGAMEPAD*) pgamepad)->wLastButtons & XINPUT_GAMEPAD_DPAD_DOWN ) > 0 ? TRUE : FALSE )





inline DWORD GET_INPUT_ACTION(XBGAMEPAD* pgamepad)
{
	// check for shift first
	if (HITTEST_XB_SHIFT_X(pgamepad))
		return XB_SHIFT_X_BUTTON;
	if (HITTEST_XB_SHIFT_Y(pgamepad))
		return XB_SHIFT_Y_BUTTON;
	if (HITTEST_XB_SHIFT_A(pgamepad))
		return XB_SHIFT_A_BUTTON;
	if (HITTEST_XB_SHIFT_B(pgamepad))
		return XB_SHIFT_B_BUTTON;
	if (HITTEST_XB_SHIFT_BLACK(pgamepad))
		return XB_SHIFT_BLACK_BUTTON;
	if (HITTEST_XB_SHIFT_WHITE(pgamepad))
		return XB_SHIFT_WHITE_BUTTON;
	if (HITTEST_XB_SHIFT_LEFT_TRIGGER(pgamepad))
		return XB_SHIFT_LEFT_TRIGGER_BUTTON;
	if (HITTEST_XB_SHIFT_RIGHT_TRIGGER(pgamepad))
		return XB_SHIFT_RIGHT_TRIGGER_BUTTON;
	if (HITTEST_XB_SHIFT_START(pgamepad))
		return XB_SHIFT_START;
	if (HITTEST_XB_SHIFT_BACK(pgamepad))
		return XB_SHIFT_BACK;
	if (HITTEST_XB_SHIFT_DPAD_LEFT(pgamepad))
		return XB_SHIFT_DPAD_LEFT;
	if (HITTEST_XB_SHIFT_DPAD_RIGHT(pgamepad))
		return XB_SHIFT_DPAD_RIGHT;
	if (HITTEST_XB_SHIFT_DPAD_UP(pgamepad))
		return XB_SHIFT_DPAD_UP;
	if (HITTEST_XB_SHIFT_DPAD_DOWN(pgamepad))
		return XB_SHIFT_DPAD_DOWN;

	// no shift

//	// check for held pad buttons
//	if (HOLDTEST_XB_DPAD_LEFT(pgamepad))
//		return XB_DPAD_LEFT;
//	if (HOLDTEST_XB_DPAD_RIGHT(pgamepad))
//		return XB_DPAD_RIGHT;
//	if (HOLDTEST_XB_DPAD_UP(pgamepad))
//		return XB_DPAD_UP;
//	if (HOLDTEST_XB_DPAD_DOWN(pgamepad))
//		return XB_DPAD_DOWN;

	if (HITTEST_XB_X(pgamepad))
		return XB_X_BUTTON;
	if (HITTEST_XB_Y(pgamepad))
		return XB_Y_BUTTON;
	if (HITTEST_XB_A(pgamepad))
		return XB_A_BUTTON;
	if (HITTEST_XB_B(pgamepad))
		return XB_B_BUTTON;
	if (HITTEST_XB_BLACK(pgamepad))
		return XB_BLACK_BUTTON;
	if (HITTEST_XB_WHITE(pgamepad))
		return XB_WHITE_BUTTON;
	if (HITTEST_XB_LEFT_TRIGGER(pgamepad))
		return XB_LEFT_TRIGGER_BUTTON;
	if (HITTEST_XB_RIGHT_TRIGGER(pgamepad))
		return XB_RIGHT_TRIGGER_BUTTON;

	// check digital inputs
	if (HITTEST_XB_DPAD_UP(pgamepad))
		return XB_DPAD_UP;
	if (HITTEST_XB_DPAD_DOWN(pgamepad))
		return XB_DPAD_DOWN;
	if (HITTEST_XB_DPAD_LEFT(pgamepad))
		return XB_DPAD_LEFT;
	if (HITTEST_XB_DPAD_RIGHT(pgamepad))
		return XB_DPAD_RIGHT;
	if (HITTEST_XB_START(pgamepad))
		return XB_START;
	if (HITTEST_XB_BACK(pgamepad))
		return XB_BACK;
	if (HITTEST_XB_RTHUMB(pgamepad))
		return XB_RTHUMB;
	if (HITTEST_XB_LTHUMB(pgamepad))
		return XB_LTHUMB;

	return XB_NO_INPUT;
}

#endif	// #ifndef __PADMAP_H__
