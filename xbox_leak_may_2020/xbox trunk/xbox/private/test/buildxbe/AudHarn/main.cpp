/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    main.cpp

Abstract:

    Entry point and initialization routines for test harness

Environment:

    Xbox

Revision History:

--*/

#include <xtl.h>

HANDLE g_hLog;

extern "C"
DWORD
WINAPI
HarnessEntryPoint(
    VOID
    );

int __cdecl main()
{
    //
    // Call main routine of the tets harness library in harnesslib.lib
    //

    return HarnessEntryPoint();
}

//
// Tell the linker to include the following symbols so that XIU and XTU
// sections inside CRT will get built and startup/cleanup routines in
// xtestlib will get executed
//

#pragma comment( linker, "/include:_xtestlib_startup" )
#pragma comment( linker, "/include:_xtestlib_cleanup" )
