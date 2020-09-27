/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    thread.c

Abstract:

    This module contains APIs for KERNEL32. We want to use MSVCRT.DLL 
    but it couldn't be built from NT source tree. So we wrap all calls 
    required by MSVCRT.DLL here since in XBox we don't need KERNEL32.DLL

Author:

    Sakphong Chanbai (schanbai) 27-Mar-2000

Environment:

    XBox

Revision History:

--*/


#include "precomp.h"


BOOL
WINAPI
TerminateThread(
    HANDLE hThread,
    DWORD dwExitCode
    )
{
    UNREFERENCED_PARAMETER( hThread );
    UNREFERENCED_PARAMETER( dwExitCode );
    return 0;
}
