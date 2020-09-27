/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    main.cpp

Abstract:

    This module contains routines to measure memory performace under NT

Environment:

    User mode

Revision History:

--*/


#include <windows.h>
#include <stdio.h>


VOID
WINAPI
MemPerfStartTest(
    HANDLE
    );


int
_cdecl
main( void )
{
    MemPerfStartTest( NULL );
    return 0;
}
