/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    kiinit.c

Abstract:

    This module implements architecture independent kernel initialization.

Author:

    David N. Cutler 11-May-1993

Environment:

    Kernel mode only.

Revision History:

--*/

#include "ki.h"

VOID
KiInitSystem (
    VOID
    )
/*++

Routine Description:

    This function initializes architecture independent kernel structures.

Arguments:

    None.

Return Value:

    None.

--*/
{

    ULONG Index;

    //
    // Initialize dispatcher ready queue listheads.
    //

    for (Index = 0; Index < MAXIMUM_PRIORITY; Index += 1) {
        InitializeListHead(&KiDispatcherReadyListHead[Index]);
    }

    //
    // Initialize the timer expiration DPC object.
    //

    KeInitializeDpc(&KiTimerExpireDpc,
                    (PKDEFERRED_ROUTINE)KiTimerExpiration, NIL);

    //
    // Initialize the timer table, the timer completion listhead, and the
    // timer completion DPC.
    //

    for (Index = 0; Index < TIMER_TABLE_SIZE; Index += 1) {
        InitializeListHead(&KiTimerTableListHead[Index]);
    }

    //
    // Initialize the wait listhead.
    //

    InitializeListHead(&KiWaitInListHead);
}
