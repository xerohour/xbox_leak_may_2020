/*
 *
 * critsect.c
 *
 * Critical sections
 *
 */

#include "precomp.h"

void InitializeCriticalSection(LPCRITICAL_SECTION pcs)
{
    NTSTATUS st;
    RtlZeroMemory(pcs, sizeof *pcs);
    pcs->LockCount = -1;
    st = NtCreateEvent(&pcs->LockSemaphore, EVENT_ALL_ACCESS, NULL,
        SynchronizationEvent, FALSE);
    ASSERT(NT_SUCCESS(st));
}

void EnterCriticalSection(LPCRITICAL_SECTION pcs)
{
    while(InterlockedIncrement(&pcs->LockCount))
        {
        NTSTATUS st;
        if(pcs->OwningThread == KeGetCurrentThread())
            break;
        InterlockedDecrement(&pcs->LockCount);
        st = NtWaitForSingleObjectEx(pcs->LockSemaphore, UserMode, FALSE, NULL);
        ASSERT(NT_SUCCESS(st));
        }
    pcs->OwningThread = KeGetCurrentThread();
    ++pcs->RecursionCount;
}

void LeaveCriticalSection(LPCRITICAL_SECTION pcs)
{
    if(pcs->RecursionCount && pcs->OwningThread == KeGetCurrentThread())
        {
        NTSTATUS st;
        if(!--pcs->RecursionCount)
            pcs->OwningThread = NULL;
        InterlockedDecrement(&pcs->LockCount);
        st = NtPulseEvent(pcs->LockSemaphore, NULL);
        ASSERT(NT_SUCCESS(st));
        }
}

void DeleteCriticalSection(LPCRITICAL_SECTION pcs)
{
    NtClose(pcs->LockSemaphore);
    RtlZeroMemory(pcs, sizeof(LPCRITICAL_SECTION));
}
