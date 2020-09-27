/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    main.cpp

Abstract:

    Entry point and initialization routines for CRT test harness

--*/

#include "crttestp.h"

void __cdecl main()
{
    KdPrint(( "CRTTEST: starting...\n" ));
    PlumHallStartTest();
    SlConfrmStartTest();
    Sleep( INFINITE );
}

