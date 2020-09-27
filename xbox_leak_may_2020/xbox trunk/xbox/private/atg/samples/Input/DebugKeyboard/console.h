//-----------------------------------------------------------------------------
// File: console.h
//
// Desc: text console functions
//
// Hist: 31.05.2001 - New
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef CONSOLE_H
#define CONSOLE_H




//-----------------------------------------------------------------------------
// Name: InitConsole()
// Desc: Initialises console
//-----------------------------------------------------------------------------
VOID InitConsole( CXBFont *i_textFont );




//-----------------------------------------------------------------------------
// Name: ShutdownConsole()
// Desc: Tidies up console
//-----------------------------------------------------------------------------
VOID ShutdownConsole( VOID );




//-----------------------------------------------------------------------------
// Name: OpenConsole()
// Desc: Sets the console to be active and initialises the current text line
//-----------------------------------------------------------------------------
VOID OpenConsole( VOID );




//-----------------------------------------------------------------------------
// Name: CloseConsole()
// Desc: Sets the console to be inactive
//-----------------------------------------------------------------------------
VOID CloseConsole( VOID );




//-----------------------------------------------------------------------------
// Name: ToggleConsole
// Desc: Switches between open and close
//-----------------------------------------------------------------------------
VOID ToggleConsole( VOID );




//-----------------------------------------------------------------------------
// Name: ClearCurrentTextLine()
// Desc: Clear the text buffer and reset the current cursor position within the
//			string
//-----------------------------------------------------------------------------
VOID ClearCurrentTextLine( VOID );




//-----------------------------------------------------------------------------
// Name: ClearConsole()
// Desc: Clear console buffer
//-----------------------------------------------------------------------------
VOID ClearConsole( VOID );




//-----------------------------------------------------------------------------
// Name: IsConsoleActive()
// Desc: Returns whether the console is currently active or not
//-----------------------------------------------------------------------------
BOOL IsConsoleActive( VOID );




//-----------------------------------------------------------------------------
// Name: DrawConsole()
// Desc: Draws the console on top of the screen
//-----------------------------------------------------------------------------
VOID DrawConsole( VOID );




//-----------------------------------------------------------------------------
// Name: ProcessConsole()
// Desc: Get keypress and perform any necessary commands
//-----------------------------------------------------------------------------
VOID ProcessConsole( VOID );




#endif

