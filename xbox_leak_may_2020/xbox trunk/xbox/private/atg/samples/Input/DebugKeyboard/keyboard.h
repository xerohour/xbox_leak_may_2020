//-----------------------------------------------------------------------------
// File: Keyboard.h
//
// Desc: Handles debug keyboard.
//
// Hist: 06.11.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef KEYBOARD_H
#define KEYBOARD_H

// Keyboard command button values
#define DELETE_KEY  8
#define TAB_KEY     9
#define RETURN_KEY  10
#define ESC_KEY     27


//-----------------------------------------------------------------------------
// Name: XBInput_InitDebugKeyboard()
// Desc: Initialise Debug Keyboard for use
//-----------------------------------------------------------------------------
HRESULT XBInput_InitDebugKeyboard();


//-----------------------------------------------------------------------------
// Name: XBInput_GetKeyboardInput()
// Desc: Processes input from a debug keyboard
//-----------------------------------------------------------------------------
CHAR XBInput_GetKeyboardInput();


#endif
