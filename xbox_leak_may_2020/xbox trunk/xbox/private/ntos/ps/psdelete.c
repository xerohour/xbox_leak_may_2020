/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    psdelete.c

Abstract:

    This module implements process and thread object termination and
    deletion.

--*/

#include "psp.h"

VOID
PspReaper(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )

/*++

Routine Description:

    This routine implements the thread reaper. The reaper is responsible
    for processing terminated threads. This includes:

        - deallocating their kernel stacks

        - releasing their process' CreateDelete lock

        - dereferencing their process

        - dereferencing themselves

Arguments:

    Dpc - Supplies a pointer to the DPC that has fired.

    Context - Unused.

    SystemArgument1 - Supplies a pointer to the thread object to reap.

    SystemArgument2 - Unused.

Return Value:

    None.

--*/

{

    PLIST_ENTRY NextEntry;
    PETHREAD Thread;

    //
    // Lock the dispatcher data and continually remove entries from the
    // reaper list until no more entries exist.
    //
    // N.B. The dispatcher database lock is used to synchronize access to
    //      the reaper list. This is done to avoid a race condition with
    //      the thread termination code.
    //

    KiLockDispatcherDatabaseAtDpcLevel();
    NextEntry = PsReaperListHead.Flink;
    while (NextEntry != &PsReaperListHead) {

        //
        // Remove the next thread from the reaper list, get the address of
        // the executive thread object, and then release the dispatcher
        // database lock.
        //

        RemoveEntryList(NextEntry);
        Thread = CONTAINING_RECORD(NextEntry, ETHREAD, ReaperListEntry);

        KiUnlockDispatcherDatabaseFromDpcLevel();

        //
        // Delete the kernel stack and dereference the thread.
        //

        MmDeleteKernelStack(Thread->Tcb.StackBase, Thread->Tcb.StackLimit);
        Thread->Tcb.StackBase = NULL;
        ObDereferenceObject(Thread);

        //
        // Lock the dispatcher database and get the address of the next
        // entry in the list.
        //

        KiLockDispatcherDatabaseAtDpcLevel();
        NextEntry = PsReaperListHead.Flink;
    }

    //
    // Unlock the dispatcher database.
    //

    KiUnlockDispatcherDatabaseFromDpcLevel();
    return;
}

VOID
PsTerminateSystemThread(
    IN NTSTATUS ExitStatus
    )

/*++

Routine Description:

    This function causes the current thread, which must be a system
    thread, to terminate.

Arguments:

    ExitStatus - Supplies the exit status associated with the thread.

Return Value:

    None.

--*/

{
    PETHREAD Thread;
    PKAPC Apc;
    PLIST_ENTRY FirstEntry;
    PLIST_ENTRY NextEntry;

    PAGED_CODE();

    Thread = PsGetCurrentThread();
    Thread->Tcb.HasTerminated = TRUE;

    KeLowerIrql(PASSIVE_LEVEL);

    //
    // Notify registered callout routines of thread deletion.
    //

    if (Thread->UniqueThread != NULL) {

        if (PspCreateThreadNotifyRoutineCount != 0) {
            ULONG i;

            for (i=0; i<PSP_MAX_CREATE_THREAD_NOTIFY; i++) {
                if (PspCreateThreadNotifyRoutine[i] != NULL) {
                    (*PspCreateThreadNotifyRoutine[i])(Thread,
                                                       Thread->UniqueThread,
                                                       FALSE);
                }
            }
        }
    }

    //
    // Finish terminating the thread at a realtime priority.
    //

    if (Thread->Tcb.Priority < LOW_REALTIME_PRIORITY) {
        KeSetPriorityThread (&Thread->Tcb, LOW_REALTIME_PRIORITY);
    }

    //
    // Rundown The Lists:
    //
    //      - Cancel Io By Thread
    //      - Cancel Timers
    //      - Cancel Registry Notify Requests pending against this thread
    //      - Perform kernel thread rundown
    //

    IoCancelThreadIo(Thread);
    ExTimerRundown();
    KeRundownThread();

    //
    // Set the exit status and exit time for the thread.
    //

    Thread->ExitStatus = ExitStatus;
    KeQuerySystemTime(&Thread->ExitTime);

    //
    // Delete the thread id handle.
    //

    if (Thread->UniqueThread != NULL) {
        NtClose(Thread->UniqueThread);
        Thread->UniqueThread = NULL;
    }

    //
    // Prevent additional APCs from being queued to this thread.
    //

    KeEnterCriticalRegion();
    KeDisableApcQueuingThread(&Thread->Tcb);

    //
    // Guard against another thread suspending us while we were in the process
    // of disabling APCs.
    //

    KeForceResumeThread (&Thread->Tcb);
    KeLeaveCriticalRegion();

    //
    // Flush user-mode APC queue.
    //

    FirstEntry = KeFlushQueueApc(&Thread->Tcb,UserMode);

    if ( FirstEntry ) {

        NextEntry = FirstEntry;
        do {
            Apc = CONTAINING_RECORD(NextEntry, KAPC, ApcListEntry);
            NextEntry = NextEntry->Flink;

            //
            // If the APC has a rundown routine then call it. Otherwise
            // deallocate the APC
            //

            if ( Apc->RundownRoutine ) {
                (Apc->RundownRoutine)(Apc);
            } else {
                ExFreePool(Apc);
            }

        } while (NextEntry != FirstEntry);
    }

    //
    // At this point, we shouldn't have kernel APCs disabled at all.  If kernel
    // APCs are disabled, then some component forgot to exit a critical region.
    //

    ASSERT(Thread->Tcb.KernelApcDisable == 0);

    //
    // Flush kernel-mode APC queue.
    //
    // There should never be any kernel mode APCs found this far
    // into thread termination. Since we go to PASSIVE_LEVEL upon
    // entering exit.
    //

    FirstEntry = KeFlushQueueApc(&Thread->Tcb,KernelMode);

    if ( FirstEntry ) {
        KeBugCheckEx(
            KERNEL_APC_PENDING_DURING_EXIT,
            (ULONG_PTR)FirstEntry,
            (ULONG_PTR)Thread->Tcb.KernelApcDisable,
            (ULONG_PTR)KeGetCurrentIrql(),
            0
            );
    }

    //
    // Terminate the thread.
    //
    // N.B. There is no return from this call.
    //
    // N.B. The kernel inserts the current thread in the reaper list.
    //

    KeTerminateThread();
}
