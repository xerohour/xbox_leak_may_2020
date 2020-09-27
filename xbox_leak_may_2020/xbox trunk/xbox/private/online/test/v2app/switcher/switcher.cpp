/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Online Dash Bootstrap switcher app

Module Name:

    switcher.cpp

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

//
// Make an external reference to the switcher logic
//
VOID XoUpdateDashSwitcherLogic();

//======================== The main function
void __cdecl main()
{
	XoUpdateDashSwitcherLogic();

	// Break here
	_asm int 3;

	return;
}

