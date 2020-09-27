/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    Resource.c

Abstract:

    This module implements the executive functions to acquire and release
    a shared resource.

--*/

#include "ntrtlp.h"

VOID
RtlInitializeCriticalSection(
    IN PRTL_CRITICAL_SECTION CriticalSection
    )

/*++

Routine Description:

    This routine initializes the input critial section variable

Arguments:

    CriticalSection - Supplies the resource variable being initialized

Return Value:

    TBD - Status of semaphore creation.

--*/

{
    ASSERT(sizeof(CriticalSection->Synchronization.RawEvent) == sizeof(KEVENT));

    KeInitializeEvent((PKEVENT)&CriticalSection->Synchronization.RawEvent,
        SynchronizationEvent, FALSE);

    CriticalSection->LockCount = -1;
    CriticalSection->RecursionCount = 0;
    CriticalSection->OwningThread = 0;
}
