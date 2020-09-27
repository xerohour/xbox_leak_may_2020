/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    threadobj.c

Abstract:

    This module implements the machine independent functions to manipulate
    the kernel thread object. Functions are provided to initialize, ready,
    alert, test alert, boost priority, enable APC queuing, disable APC
    queuing, confine, set affinity, set priority, suspend, resume, alert
    resume, terminate, read thread state, freeze, unfreeze, query data
    alignment handling mode, force resume, and enter and leave critical
    regions for thread objects.

Author:

    David N. Cutler (davec) 4-Mar-1989

Environment:

    Kernel mode only.

Revision History:

--*/

#include "ki.h"

//
// The following assert macro is used to check that an input thread object is
// really a kthread and not something else, like deallocated pool.
//

#define ASSERT_THREAD(E) {                    \
    ASSERT((E)->Header.Type == ThreadObject); \
}

VOID
KeInitializeThread (
    IN PKTHREAD Thread,
    IN PVOID KernelStack,
    IN SIZE_T KernelStackSize,
    IN SIZE_T TlsDataSize,
    IN PKSYSTEM_ROUTINE SystemRoutine,
    IN PKSTART_ROUTINE StartRoutine OPTIONAL,
    IN PVOID StartContext OPTIONAL,
    IN PKPROCESS Process
    )

/*++

Routine Description:

    This function initializes a thread object. The priority, affinity,
    and initial quantum are taken from the parent process object. The
    thread object is inserted at the end of the thread list for the
    parent process.

    N.B. It is assumed that the thread object is zeroed.

Arguments:

    Thread - Supplies a pointer to a dispatcher object of type thread.

    KernelStack - Supplies a pointer to the base of a kernel stack on which
        the context frame for the thread is to be constructed.

    KernelStackSize - Supplies the number of bytes allocated for the kernel
        stack.

    TlsDataSize - Supplies the number of bytes reserved from the kernel stack
        for thread local storage.

    SystemRoutine - Supplies a pointer to the system function that is to be
        called when the thread is first scheduled for execution.

    StartRoutine - Supplies an optional pointer to a function that is to be
        called after the system has finished initializing the thread. This
        parameter is specified if the thread is a system thread and will
        execute totally in kernel mode.

    StartContext - Supplies an optional pointer to an arbitrary data structure
        which will be passed to the StartRoutine as a parameter. This
        parameter is specified if the thread is a system thread and will
        execute totally in kernel mode.

    Process - Supplies a pointer to a control object of type process.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;
    PKTIMER Timer;
    PKWAIT_BLOCK WaitBlock;

    //
    // Initialize the standard dispatcher object header and set the initial
    // state of the thread object.
    //

    Thread->Header.Type = ThreadObject;
    Thread->Header.Size = sizeof(KTHREAD) / sizeof(LONG);
    InitializeListHead(&Thread->Header.WaitListHead);

    //
    // Initialize the owned mutant listhead.
    //

    InitializeListHead(&Thread->MutantListHead);

    //
    // Initialize the alerted, preempted, debugactive, autoalignment,
    // kernel stack resident, enable kernel stack swap, and process
    // ready queue boolean values.
    //
    // N.B. Only nonzero values are initialized.
    //

    //
    // Initialize the APC state pointers, the current APC state, the saved
    // APC state, and enable APC queuing.
    //

    InitializeListHead(&Thread->ApcState.ApcListHead[KernelMode]);
    InitializeListHead(&Thread->ApcState.ApcListHead[UserMode]);
    Thread->ApcState.Process = Process;
    Thread->ApcState.ApcQueueable = TRUE;

    //
    // Initialize the kernel mode suspend APC and the suspend semaphore object.
    // and the builtin wait timeout timer object.
    //

    KeInitializeApc(&Thread->SuspendApc,
                    Thread,
                    (PKKERNEL_ROUTINE)KiSuspendNop,
                    (PKRUNDOWN_ROUTINE)NULL,
                    KiSuspendThread,
                    KernelMode,
                    NULL);

    KeInitializeSemaphore(&Thread->SuspendSemaphore, 0L, 2L);

    //
    // Initialize the builtin timer trimer wait wait block.
    //
    // N.B. This is the only time the wait block is initialized since this
    //      information is constant.
    //

    Timer = &Thread->Timer;
    KeInitializeTimer(Timer);
    WaitBlock = &Thread->TimerWaitBlock;
    WaitBlock->Object = Timer;
    WaitBlock->WaitKey = (CSHORT)STATUS_TIMEOUT;
    WaitBlock->WaitType = WaitAny;
    WaitBlock->Thread = Thread;
    WaitBlock->WaitListEntry.Flink = &Timer->Header.WaitListHead;
    WaitBlock->WaitListEntry.Blink = &Timer->Header.WaitListHead;

    //
    // Set the initial kernel stack and the initial thread context.
    //

    Thread->StackBase = KernelStack;
    Thread->StackLimit = (PVOID)((ULONG_PTR)KernelStack - KernelStackSize);
    KiInitializeContextThread(Thread,
                              TlsDataSize,
                              SystemRoutine,
                              StartRoutine,
                              StartContext);

    //
    // Set the base thread priority, the thread priority, the thread affinity,
    // the thread quantum, and the scheduling state.
    //

    Thread->BasePriority = Process->BasePriority;
    Thread->Priority = Thread->BasePriority;
    Thread->Quantum = Process->ThreadQuantum;
    Thread->State = Initialized;
    Thread->DisableBoost = Process->DisableBoost;

    //
    // Lock the dispatcher database, insert the thread in the process
    // thread list, increment the kernel stack count, and unlock the
    // dispatcher database.
    //
    // N.B. The distinguished value MAXSHORT is used to signify that no
    //      threads have been created for a process.
    //

    KiLockDispatcherDatabase(&OldIrql);
    InsertTailList(&Process->ThreadListHead, &Thread->ThreadListEntry);
    Process->StackCount += 1;

    //
    // Initialize the ideal processor number for the thread.
    //
    //  N.B. This must be done under the dispatcher lock to prevent byte
    //      granularity problems on Alpha.
    //

    KiUnlockDispatcherDatabase(OldIrql);
    return;
}

BOOLEAN
KeAlertThread (
    IN PKTHREAD Thread,
    IN KPROCESSOR_MODE AlertMode
    )

/*++

Routine Description:

    This function attempts to alert a thread and cause its execution to
    be continued if it is currently in an alertable Wait state. Otherwise
    it just sets the alerted variable for the specified processor mode.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

    AlertMode - Supplies the processor mode for which the thread is
        to be alerted.

Return Value:

    The previous state of the alerted variable for the specified processor
    mode.

--*/

{

    BOOLEAN Alerted;
    KIRQL OldIrql;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level, lock dispatcher database, and lock
    // APC queue.
    //

    KiLockDispatcherDatabase(&OldIrql);
    KiLockApcQueueAtDpcLevel(Thread);

    //
    // Capture the current state of the alerted variable for the specified
    // processor mode.
    //

    Alerted = Thread->Alerted[AlertMode];

    //
    // If the alerted state for the specified processor mode is Not-Alerted,
    // then attempt to alert the thread.
    //

    if (Alerted == FALSE) {

        //
        // If the thread is currently in a Wait state, the Wait is alertable,
        // and the specified processor mode is less than or equal to the Wait
        // mode, then the thread is unwaited with a status of "alerted".
        //

        if ((Thread->State == Waiting) && (Thread->Alertable == TRUE) &&
            (AlertMode <= Thread->WaitMode)) {
            KiUnwaitThread(Thread, STATUS_ALERTED, ALERT_INCREMENT);

        } else {
            Thread->Alerted[AlertMode] = TRUE;
        }
    }

    //
    // Unlock APC queue, unlock dispatcher database, lower IRQL to its
    // previous value, and return the previous alerted state for the
    // specified mode.
    //

    KiUnlockApcQueueFromDpcLevel(Thread);
    KiUnlockDispatcherDatabase(OldIrql);
    return Alerted;
}

ULONG
KeAlertResumeThread (
    IN PKTHREAD Thread
    )

/*++

Routine Description:

    This function attempts to alert a thread in kernel mode and cause its
    execution to be continued if it is currently in an alertable Wait state.
    In addition, a resume operation is performed on the specified thread.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

Return Value:

    The previous suspend count.

--*/

{

    ULONG OldCount;
    KIRQL OldIrql;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level, lock dispatcher database, and lock
    // APC queue.
    //

    KiLockDispatcherDatabase(&OldIrql);
    KiLockApcQueueAtDpcLevel(Thread);

    //
    // If the kernel mode alerted state is FALSE, then attempt to alert
    // the thread for kernel mode.
    //

    if (Thread->Alerted[KernelMode] == FALSE) {

        //
        // If the thread is currently in a Wait state and the Wait is alertable,
        // then the thread is unwaited with a status of "alerted". Else set the
        // kernel mode alerted variable.
        //

        if ((Thread->State == Waiting) && (Thread->Alertable == TRUE)) {
            KiUnwaitThread(Thread, STATUS_ALERTED, ALERT_INCREMENT);

        } else {
            Thread->Alerted[KernelMode] = TRUE;
        }
    }

    //
    // Capture the current suspend count.
    //

    OldCount = Thread->SuspendCount;

    //
    // If the thread is currently suspended, then decrement its suspend count.
    //

    if (OldCount != 0) {
        Thread->SuspendCount -= 1;

        //
        // If the resultant suspend count is zero, then resume the thread by
        // releasing its suspend semaphore.
        //

        if (Thread->SuspendCount == 0) {
            Thread->SuspendSemaphore.Header.SignalState += 1;
            KiWaitTest(&Thread->SuspendSemaphore, RESUME_INCREMENT);
        }
    }

    //
    // Unlock APC queue, unlock dispatcher database, lower IRQL to its
    // previous value, and return the previous suspend count.
    //

    KiUnlockApcQueueFromDpcLevel(Thread);
    KiUnlockDispatcherDatabase(OldIrql);
    return OldCount;
}

VOID
KeBoostPriorityThread (
    IN PKTHREAD Thread,
    IN KPRIORITY Increment
    )

/*++

Routine Description:

    This function boosts the priority of the specified thread using the
    same algorithm used when a thread gets a boost from a wait operation.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

    Increment - Supplies the priority increment that is to be applied to
        the thread's priority.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // If the thread does not run at a realtime priority level, then boost
    // the thread priority.
    //

    if (Thread->Priority < LOW_REALTIME_PRIORITY) {
        KiBoostPriorityThread(Thread, Increment);
    }

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);
    return;
}

BOOLEAN
KeDisableApcQueuingThread (
    IN PKTHREAD Thread
    )

/*++

Routine Description:

    This function disables the queuing of APC's to the specified thread.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

Return Value:

    The previous value of the APC queuing state variable.

--*/

{

    BOOLEAN ApcQueueable;
    KIRQL OldIrql;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Capture the current state of the APC queueable state variable and
    // set its state to FALSE.
    //

    ApcQueueable = Thread->ApcState.ApcQueueable;
    Thread->ApcState.ApcQueueable = FALSE;

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return the previous APC queueable state.
    //

    return ApcQueueable;
}

BOOLEAN
KeEnableApcQueuingThread (
    IN PKTHREAD Thread
    )

/*++

Routine Description:

    This function enables the queuing of APC's to the specified thread.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

Return Value:

    The previous value of the APC queuing state variable.

--*/

{

    BOOLEAN ApcQueueable;
    KIRQL OldIrql;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Capture the current state of the APC queueable state variable and
    // set its state to TRUE.
    //

    ApcQueueable = Thread->ApcState.ApcQueueable;
    Thread->ApcState.ApcQueueable = TRUE;

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return previous APC queueable state.
    //

    return ApcQueueable;
}

ULONG
KeForceResumeThread (
    IN PKTHREAD Thread
    )

/*++

Routine Description:

    This function forces resumption of thread execution if the thread is
    suspended. If the specified thread is not suspended, then no operation
    is performed.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

Return Value:

    The sum of the previous suspend count and the freeze count.

--*/

{

    ULONG OldCount;
    KIRQL OldIrql;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Capture the current suspend count.
    //

    OldCount = Thread->SuspendCount;

    //
    // If the thread is currently suspended, then force resumption of
    // thread execution.
    //

    if (OldCount != 0) {
        Thread->SuspendCount = 0;
        Thread->SuspendSemaphore.Header.SignalState += 1;
        KiWaitTest(&Thread->SuspendSemaphore, RESUME_INCREMENT);
    }

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return the previous suspend count.
    //

    return OldCount;
}

LONG
KeQueryBasePriorityThread (
    IN PKTHREAD Thread
    )

/*++

Routine Description:

    This function returns the base priority increment of the specified
    thread.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

Return Value:

    The base priority increment of the specified thread.

--*/

{

    LONG Increment;
    KIRQL OldIrql;
    PKPROCESS Process;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // If priority saturation occured the last time the thread base priority
    // was set, then return the saturation increment value. Otherwise, compute
    // the increment value as the difference between the thread base priority
    // and the process base priority.
    //

    Process = Thread->ApcState.Process;
    Increment = Thread->BasePriority - Process->BasePriority;
    if (Thread->Saturation != 0) {
        Increment = ((HIGH_PRIORITY + 1) / 2) * Thread->Saturation;
    }

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return the previous thread base priority increment.
    //

    return Increment;
}

VOID
KeReadyThread (
    IN PKTHREAD Thread
    )

/*++

Routine Description:

    This function readies a thread for execution. If the thread's process
    is currently not in the balance set, then the thread is inserted in the
    thread's process' ready queue. Else if the thread is higher priority than
    another thread that is currently running on a processor then the thread
    is selected for execution on that processor. Else the thread is inserted
    in the dispatcher ready queue selected by its priority.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Ready the specified thread for execution.
    //

    KiReadyThread(Thread);

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);
    return;
}

ULONG
KeResumeThread (
    IN PKTHREAD Thread
    )

/*++

Routine Description:

    This function resumes the execution of a suspended thread. If the
    specified thread is not suspended, then no operation is performed.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

Return Value:

    The previous suspend count.

--*/

{

    ULONG OldCount;
    KIRQL OldIrql;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Capture the current suspend count.
    //

    OldCount = Thread->SuspendCount;

    //
    // If the thread is currently suspended, then decrement its suspend count.
    //

    if (OldCount != 0) {
        Thread->SuspendCount -= 1;

        //
        // If the resultant suspend count is zero, then resume the thread by
        // releasing its suspend semaphore.
        //

        if (Thread->SuspendCount == 0) {
            Thread->SuspendSemaphore.Header.SignalState += 1;
            KiWaitTest(&Thread->SuspendSemaphore, RESUME_INCREMENT);
        }
    }

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return the previous suspend count.
    //

    return OldCount;
}

VOID
KeRundownThread (
    VOID
    )

/*++

Routine Description:

    This function is called by the executive to rundown thread structures
    which must be guarded by the dispatcher database lock and which must
    be processed before actually terminating the thread. An example of such
    a structure is the mutant ownership list that is anchored in the kernel
    thread object.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PKMUTANT Mutant;
    PLIST_ENTRY NextEntry;
    KIRQL OldIrql;
    PKTHREAD Thread;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    Thread = KeGetCurrentThread();
    KiLockDispatcherDatabase(&OldIrql);

    //
    // Scan the list of owned mutant objects and release the mutant objects
    // with an abandoned status. If the mutant is a kernel mutex, then bug
    // check.
    //

    NextEntry = Thread->MutantListHead.Flink;
    while (NextEntry != &Thread->MutantListHead) {
        Mutant = CONTAINING_RECORD(NextEntry, KMUTANT, MutantListEntry);

        RemoveEntryList(&Mutant->MutantListEntry);
        Mutant->Header.SignalState = 1;
        Mutant->Abandoned = TRUE;
        Mutant->OwnerThread = (PKTHREAD)NULL;
        if (IsListEmpty(&Mutant->Header.WaitListHead) != TRUE) {
            KiWaitTest(Mutant, MUTANT_INCREMENT);
        }

        NextEntry = Thread->MutantListHead.Flink;
    }

    //
    // Release dispatcher database lock and lower IRQL to its previous value.
    //

    KiUnlockDispatcherDatabase(OldIrql);
    return;
}

LONG
KeSetBasePriorityThread (
    IN PKTHREAD Thread,
    IN LONG Increment
    )

/*++

Routine Description:

    This function sets the base priority of the specified thread to a
    new value.  The new base priority for the thread is the process base
    priority plus the increment.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

    Increment - Supplies the base priority increment of the subject thread.

        N.B. If the absolute value of the increment is such that saturation
             of the base priority is forced, then subsequent changes to the
             parent process base priority will not change the base priority
             of the thread.

Return Value:

    The previous base priority increment of the specified thread.

--*/

{

    KPRIORITY NewBase;
    KPRIORITY NewPriority;
    KPRIORITY OldBase;
    LONG OldIncrement;
    KIRQL OldIrql;
    PKPROCESS Process;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Capture the base priority of the specified thread and determine
    // whether saturation if being forced.
    //

    Process = Thread->ApcState.Process;
    OldBase = Thread->BasePriority;
    OldIncrement = OldBase - Process->BasePriority;
    if (Thread->Saturation != 0) {
        OldIncrement = ((HIGH_PRIORITY + 1) / 2) * Thread->Saturation;
    }

    Thread->Saturation = FALSE;
    if (abs(Increment) >= (HIGH_PRIORITY + 1) / 2) {
        Thread->Saturation = (Increment > 0) ? 1 : -1;
    }

    //
    // Set the base priority of the specified thread. If the thread's process
    // is in the realtime class, then limit the change to the realtime class.
    // Otherwise, limit the change to the variable class.
    //

    NewBase = Process->BasePriority + Increment;
    if (Process->BasePriority >= LOW_REALTIME_PRIORITY) {
        if (NewBase < LOW_REALTIME_PRIORITY) {
            NewBase = LOW_REALTIME_PRIORITY;

        } else if (NewBase > HIGH_PRIORITY) {
            NewBase = HIGH_PRIORITY;
        }

        //
        // Set the new priority of the thread to the new base priority.
        //

        NewPriority = NewBase;

    } else {
        if (NewBase >= LOW_REALTIME_PRIORITY) {
            NewBase = LOW_REALTIME_PRIORITY - 1;

        } else if (NewBase <= LOW_PRIORITY) {
            NewBase = 1;
        }

        //
        // Compute the new thread priority. If the new priority is outside
        // the variable class, then set the new priority to the highest
        // variable priority.
        //

        if (Thread->Saturation != 0) {
            NewPriority = NewBase;

        } else {
            NewPriority = Thread->Priority +
                            (NewBase - OldBase) - Thread->PriorityDecrement;

            if (NewPriority >= LOW_REALTIME_PRIORITY) {
                NewPriority = LOW_REALTIME_PRIORITY - 1;
            }
        }
    }

    //
    // Set the new base priority and clear the priority decrement. If the
    // new priority is not equal to the old priority, then set the new thread
    // priority.
    //

    Thread->BasePriority = (SCHAR)NewBase;
    Thread->DecrementCount = 0;
    Thread->PriorityDecrement = 0;
    if (NewPriority != Thread->Priority) {
        Thread->Quantum = Process->ThreadQuantum;
        KiSetPriorityThread(Thread, NewPriority);
    }

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return the previous thread base priority.
    //

    return OldIncrement;
}

LOGICAL
KeSetDisableBoostThread (
    IN PKTHREAD Thread,
    IN LOGICAL Disable
    )

/*++

Routine Description:

    This function disables priority boosts for the specified thread.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

    Disable - Supplies a logical value that determines whether priority
        boosts for the thread are disabled or enabled.

Return Value:

    The previous value of the disable boost state variable.

--*/

{

    LOGICAL DisableBoost;
    KIRQL OldIrql;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Capture the current state of the disable boost variable and set its
    // state to TRUE.
    //

    DisableBoost = Thread->DisableBoost;
    Thread->DisableBoost = (BOOLEAN)Disable;

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return the previous disable boost state.
    //

    return DisableBoost;
}

KPRIORITY
KeSetPriorityThread (
    IN PKTHREAD Thread,
    IN KPRIORITY Priority
    )

/*++

Routine Description:

    This function sets the priority of the specified thread to a new value.
    If the new thread priority is lower than the old thread priority, then
    resecheduling may take place if the thread is currently running on, or
    about to run on, a processor.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

    Priority - Supplies the new priority of the subject thread.

Return Value:

    The previous priority of the specified thread.

--*/

{

    KIRQL OldIrql;
    KPRIORITY OldPriority;
    PKPROCESS Process;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);
    ASSERT(((Priority != 0) || (Thread->BasePriority == 0)) &&
           (Priority <= HIGH_PRIORITY));

    ASSERT(KeIsExecutingDpc() == FALSE);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Capture the current thread priority, set the thread priority to the
    // the new value, and replenish the thread quantum. It is assumed that
    // the priority would not be set unless the thread had already lost it
    // initial quantum.
    //

    OldPriority = Thread->Priority;
    Process = Thread->ApcState.Process;
    Thread->Quantum = Process->ThreadQuantum;
    Thread->DecrementCount = 0;
    Thread->PriorityDecrement = 0;
    KiSetPriorityThread(Thread, Priority);

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return the previous thread priority.
    //

    return OldPriority;
}

ULONG
KeSuspendThread (
    IN PKTHREAD Thread
    )

/*++

Routine Description:

    This function suspends the execution of a thread. If the suspend count
    overflows the maximum suspend count, then a condition is raised.

Arguments:

    Thread  - Supplies a pointer to a dispatcher object of type thread.

Return Value:

    The previous suspend count.

--*/

{

    ULONG OldCount;
    KIRQL OldIrql;

    ASSERT_THREAD(Thread);
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Capture the current suspend count.
    //

    OldCount = Thread->SuspendCount;

    //
    // If the suspend count is at its maximum value, then unlock dispatcher
    // database, lower IRQL to its previous value, and raise an error
    // condition.
    //

    if (OldCount == MAXIMUM_SUSPEND_COUNT) {

        //
        // Unlock the dispatcher database and raise an exception.
        //

        KiUnlockDispatcherDatabase(OldIrql);
        ExRaiseStatus(STATUS_SUSPEND_COUNT_EXCEEDED);
    }

    //
    // Don't suspend if APC queuing is disabled. Thread is being deleted in this case.
    //

    if (Thread->ApcState.ApcQueueable == TRUE) {

        //
        // Increment the suspend count. If the thread was not previously suspended,
        // then queue the thread's suspend APC.
        //

        Thread->SuspendCount += 1;
        if (OldCount == 0) {
            if (KiInsertQueueApc(&Thread->SuspendApc, RESUME_INCREMENT) == FALSE) {
                Thread->SuspendSemaphore.Header.SignalState -= 1;
            }
        }
    }

    //
    // Unlock dispatcher database and lower IRQL to its previous value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return the previous suspend count.
    //

    return OldCount;
}

VOID
KeTerminateThread (
    VOID
    )

/*++

Routine Description:

    This function terminates the execution of the current thread, sets the
    signal state of the thread to Signaled, and attempts to satisfy as many
    Waits as possible. The scheduling state of the thread is set to terminated,
    and a new thread is selected to run on the current processor. There is no
    return from this function.

Arguments:

    None.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;
    PKPROCESS Process;
    PRKQUEUE Queue;
    PRKTHREAD Thread;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    Thread = KeGetCurrentThread();
    KiLockDispatcherDatabase(&OldIrql);

    //
    // If the current thread is processing a queue entry, then remove
    // the thrread from the queue object thread list and attempt to
    // activate another thread that is blocked on the queue object.
    //

    Queue = Thread->Queue;
    if (Queue != NULL) {
        RemoveEntryList(&Thread->QueueListEntry);
        KiActivateWaiterQueue(Queue);
    }

    //
    // Set the state of the current thread object to Signaled, and attempt
    // to satisfy as many Waits as possible.
    //

    Thread->Header.SignalState = TRUE;
    if (IsListEmpty(&Thread->Header.WaitListHead) != TRUE) {
        KiWaitTest((PVOID)Thread, 0);
    }

    //
    // Remove thread from its parent process' thread list.
    //

    RemoveEntryList(&Thread->ThreadListEntry);

    //
    // Set thread scheduling state to terminated, decrement the process'
    // stack count, select a new thread to run on the current processor,
    // and swap context to the new thread.
    //

    Thread->State = Terminated;
    Process = Thread->ApcState.Process;
    Process->StackCount -= 1;

    //
    // Rundown any architectural specific structures.
    //

    KiRundownThread(Thread);

    //
    // Insert the reaper DPC into the DPC queue to delete the stack and release
    // the reference on the thread object.
    //

    InsertTailList(&PsReaperListHead, &((PETHREAD)Thread)->ReaperListEntry);
    KeInsertQueueDpc(&PsReaperDpc, NULL, NULL);

    //
    // Get off the processor for the last time.
    //

    KiSwapThread();
    return;
}

BOOLEAN
KeTestAlertThread (
    IN KPROCESSOR_MODE AlertMode
    )

/*++

Routine Description:

    This function tests to determine if the alerted variable for the
    specified processor mode has a value of TRUE or whether a user mode
    APC should be delivered to the current thread.

Arguments:

    AlertMode - Supplies the processor mode which is to be tested
        for an alerted condition.

Return Value:

    The previous state of the alerted variable for the specified processor
    mode.

--*/

{

    BOOLEAN Alerted;
    KIRQL OldIrql;
    PKTHREAD Thread;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level, lock dispatcher database, and lock
    // APC queue.
    //

    Thread = KeGetCurrentThread();
    KiLockDispatcherDatabase(&OldIrql);
    KiLockApcQueueAtDpcLevel(Thread);

    //
    // If the current thread is alerted for the specified processor mode,
    // then clear the alerted state. Else if the specified processor mode
    // is user and the current thread's user mode APC queue contains an entry,
    // then set user APC pending.
    //

    Alerted = Thread->Alerted[AlertMode];
    if (Alerted == TRUE) {
        Thread->Alerted[AlertMode] = FALSE;

    } else if ((AlertMode == UserMode) &&
              (IsListEmpty(&Thread->ApcState.ApcListHead[UserMode]) != TRUE)) {
        Thread->ApcState.UserApcPending = TRUE;
    }

    //
    // Unlock APC queue, unlock dispatcher database, lower IRQL to its
    // previous value, and return the previous alerted state for the
    // specified mode.
    //

    KiUnlockApcQueueFromDpcLevel(Thread);
    KiUnlockDispatcherDatabase(OldIrql);
    return Alerted;
}
