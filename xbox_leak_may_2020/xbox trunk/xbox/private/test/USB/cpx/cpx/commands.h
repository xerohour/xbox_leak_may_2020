/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    commands.h

Abstract:

    function declarations for commands.cpp

Author:

    Jason Gould (a-jasgou)   May 2000

Revision History:

--*/
#ifndef __commands_h_
#define __commands_h_

//#include "cpx.h"

VOID QuitFunction(DWORD context, BOOL repeat, BYTE slot);
VOID ModeFunction(DWORD context, BOOL repeat, BYTE slot);
VOID SwitchController(DWORD context, BOOL repeat, BYTE slot);

#endif
