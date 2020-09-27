/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    psinit.c

Abstract:

    Process Structure Initialization.

--*/

#include "psp.h"

//
// Process Structure Global Data
//

DECLSPEC_RDATA OBJECT_TYPE PsThreadObjectType = {
    ExAllocatePoolWithTag,
    ExFreePool,
    NULL,
    NULL,
    NULL,
    (PVOID)FIELD_OFFSET(KTHREAD, Header),
    'erhT'
};

INITIALIZED_LIST_ENTRY(PsReaperListHead);
KDPC PsReaperDpc;

//
// List head and mutex that links all processes that have been initialized
//

BOOLEAN
PsInitSystem (
    VOID
    )
/*++

Routine Description:

    This function fermorms process structure initialization.
    It is called during phase 0 and phase 1 initialization. Its
    function is to dispatch to the appropriate phase initialization
    routine.

Arguments:

    None.

Return Value:

    TRUE - Initialization succeeded.

    FALSE - Initialization failed.

--*/
{
    HANDLE ThreadHandle;

    //
    // Initialize Reaper Data Structures
    //

    KeInitializeDpc(&PsReaperDpc, PspReaper, NULL);

    //
    // Phase 1 System initialization
    //

    if ( !NT_SUCCESS(PsCreateSystemThread(
                    &ThreadHandle,
                    NULL,
                    Phase1Initialization,
                    NULL,
                    FALSE
                    )) ) {
        return FALSE;
    }

    NtClose( ThreadHandle );

    return TRUE;
}
