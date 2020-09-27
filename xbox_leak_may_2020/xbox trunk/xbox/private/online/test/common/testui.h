/*++

Copyright (c) Microsoft Corporation

Author:
	Keith Lau (keithlau)

Description:
	Definitions for the test UI functions

Module Name:

    testui.h

--*/

#ifndef __TESTUI_H__
#define __TESTUI_H__

#define XFONT_TRUETYPE 1 
#include <xfont.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// API to initialize D3D8 graphics
//
HRESULT InitializeGraphics();

//
// API to initialize Font support
//
HRESULT InitializeFont(
			D3DCOLOR	TextColor,
			D3DCOLOR	BackgroundColor,
			int			iBkMode
			);

//
// API to start a fresh rendering scene
//
VOID BeginNewScene();

//
// API to print a string to the screen as if the screen is in text mode
// Sorry, no scrolling
//
int Xputs(WCHAR *wszString);

//
// API to render and display a scene
//
VOID PresentScene();

#ifdef __cplusplus
}
#endif

#endif  //__TESTUI_H__

