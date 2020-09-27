/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    thredsup.c

Abstract:

    This module contains the support routines for the thread object. It
    contains functions to boost the priority of a thread, find a ready
    thread, select the next thread, ready a thread, set priority of a
    thread, and to suspend a thread.

Author:

    David N. Cutler (davec) 5-Mar-1989

Environment:

    All of the functions in this module execute in kernel mode except
    the function that raises a user mode alert condition.

Revision History:


--*/

#include "ki.h"

//
// Define context switch data collection macro.
//

//#define _COLLECT_SWITCH_DATA_ 1

#if defined(_COLLECT_SWITCH_DATA_)

#define KiIncrementSwitchCounter(Member) KeThreadSwitchCounters.Member += 1

#else

#define KiIncrementSwitchCounter(Member)

#endif

VOID
KiSuspendNop (
    IN PKAPC Apc,
    IN OUT PKNORMAL_ROUTINE *NormalRoutine,
    IN OUT PVOID *NormalContext,
    IN OUT PVOID *SystemArgument1,
    IN OUT PVOID *SystemArgument2
    )

/*++

Routine Description:

    This function is the kernel routine for the builtin suspend APC for a
    thread. It is executed in kernel mode as the result of queuing the
    builtin suspend APC and performs no operation. It is called just prior
    to calling the normal routine and simply returns.

Arguments:

    Apc - Supplies a pointer to a control object of type APC.

    NormalRoutine - not used

    NormalContext - not used

    SystemArgument1 - not used

    SystemArgument2 - not used

Return Value:

    None.

--*/

{

    //
    // No operation is performed by this routine.
    //

    return;
}

PKTHREAD
FASTCALL
KiFindReadyThread (
    IN KPRIORITY LowPriority
    )

/*++

Routine Description:

    This function searches the dispatcher ready queues from the specified
    high priority to the specified low priority in an attempt to find a thread
    that can execute on the specified processor.

Arguments:

    Processor - Supplies the number of the processor to find a thread for.

    LowPriority - Supplies the lowest priority dispatcher ready queue to
        examine.

Return Value:

    If a thread is located that can execute on the specified processor, then
    the address of the thread object is returned. Otherwise a null pointer is
    returned.

--*/

{

    ULONG HighPriority;
    PRLIST_ENTRY ListHead;
    PRLIST_ENTRY NextEntry;
    ULONG PrioritySet;
    PRKTHREAD Thread;

    //
    // Compute the set of priority levels that should be scanned in an attempt
    // to find a thread that can run on the specified processor.
    //

    PrioritySet = (~((1 << LowPriority) - 1)) & KiReadySummary;

    FindFirstSetLeftMember(PrioritySet, &HighPriority);
    ListHead = &KiDispatcherReadyListHead[HighPriority];
    PrioritySet <<= (31 - HighPriority);
    while (PrioritySet != 0) {

        //
        // If the next bit in the priority set is a one, then examine the
        // corresponding dispatcher ready queue.
        //

        if ((LONG)PrioritySet < 0) {
            NextEntry = ListHead->Flink;

            ASSERT(NextEntry != ListHead);

            Thread = CONTAINING_RECORD(NextEntry, KTHREAD, WaitListEntry);
            RemoveEntryList(&Thread->WaitListEntry);
            if (IsListEmpty(ListHead)) {
                ClearMember(HighPriority, KiReadySummary);
            }

            return (PKTHREAD)Thread;
        }

        HighPriority -= 1;
        ListHead -= 1;
        PrioritySet <<= 1;
    };

    //
    // No thread could be found, return a null pointer.
    //

    return (PKTHREAD)NULL;
}

VOID
FASTCALL
KiReadyThread (
    IN PRKTHREAD Thread
    )

/*++

Routine Description:

    This function readies a thread for execution and attempts to immediately
    dispatch the thread for execution by preempting another lower priority
    thread. If a thread can be preempted, then the specified thread enters
    the standby state and the target processor is requested to dispatch. If
    another thread cannot be preempted, then the specified thread is inserted
    either at the head or tail of the dispatcher ready selected by its priority
    acccording to whether it was preempted or not.

Arguments:

    Thread - Supplies a pointer to a dispatcher object of type thread.

Return Value:

    None.

--*/

{

    PRKPRCB Prcb;
    BOOLEAN Preempted;
    KPRIORITY ThreadPriority;
    PRKTHREAD Thread1;

    //
    // Save value of thread's preempted flag, set thread preempted FALSE,
    // capture the thread priority, and set clear the read wait time.
    //

    Preempted = Thread->Preempted;
    Thread->Preempted = FALSE;
    ThreadPriority = Thread->Priority;
    Thread->WaitTime = KiQueryLowTickCount();

    //
    // If there is an idle processor, then schedule the thread on an
    // idle processor giving preference to the processor the thread
    // last ran on. Otherwise, try to preempt either a thread in the
    // standby or running state.
    //

    Prcb = KeGetCurrentPrcb();
    if (KiIdleSummary != 0) {
        KiIdleSummary = 0;
        KiIncrementSwitchCounter(IdleLast);
        Prcb->NextThread = Thread;
        Thread->State = Standby;

        return;

    } else {

        if (Prcb->NextThread != NULL) {
            Thread1 = Prcb->NextThread;
            if (ThreadPriority > Thread1->Priority) {
                Thread1->Preempted = TRUE;
                Prcb->NextThread = Thread;
                Thread->State = Standby;
                KiReadyThread(Thread1);
                KiIncrementSwitchCounter(PreemptLast);
                return;
            }

        } else {
            Thread1 = Prcb->CurrentThread;
            if (ThreadPriority > Thread1->Priority) {
                Thread1->Preempted = TRUE;
                Prcb->NextThread = Thread;
                Thread->State = Standby;
                KiRequestDispatchInterrupt();
                KiIncrementSwitchCounter(PreemptLast);
                return;
            }
        }
    }

    //
    // No thread can be preempted. Insert the thread in the dispatcher
    // queue selected by its priority. If the thread was preempted and
    // runs at a realtime priority level, then insert the thread at the
    // front of the queue. Else insert the thread at the tail of the queue.
    //

    Thread->State = Ready;
    if (Preempted != FALSE) {
        InsertHeadList(&KiDispatcherReadyListHead[ThreadPriority],
                       &Thread->WaitListEntry);

    } else {
        InsertTailList(&KiDispatcherReadyListHead[ThreadPriority],
                       &Thread->WaitListEntry);
    }

    SetMember(ThreadPriority, KiReadySummary);
    return;
}

PRKTHREAD
FASTCALL
KiSelectNextThread (
    IN PRKTHREAD Thread
    )

/*++

Routine Description:

    This function selects the next thread to run on the processor that the
    specified thread is running on. If a thread cannot be found, then the
    idle thread is selected.

Arguments:

    Thread - Supplies a pointer to a dispatcher object of type thread.

Return Value:

    The address of the selected thread object.

--*/

{

    PRKPRCB Prcb;
    PRKTHREAD Thread1;

    //
    // Get the processor number and the address of the processor control block.
    //

    Prcb = KeGetCurrentPrcb();

    //
    // If a thread has already been selected to run on the specified processor,
    // then return that thread as the selected thread.
    //

    if ((Thread1 = Prcb->NextThread) != NULL) {
        Prcb->NextThread = (PKTHREAD)NULL;

    } else {

        //
        // Attempt to find a ready thread to run.
        //

        Thread1 = KiFindReadyThread(0);

        //
        // If a thread was not found, then select the idle thread and
        // set the processor member in the idle summary.
        //

        if (Thread1 == NULL) {
            KiIncrementSwitchCounter(SwitchToIdle);
            Thread1 = Prcb->IdleThread;

            KiIdleSummary = 1;
        }
    }

    //
    // Return address of selected thread object.
    //

    return Thread1;
}

VOID
FASTCALL
KiSetPriorityThread (
    IN PRKTHREAD Thread,
    IN KPRIORITY Priority
    )

/*++

Routine Description:

    This function set the priority of the specified thread to the specified
    value. If the thread is in the standby or running state, then the processor
    may be redispatched. If the thread is in the ready state, then some other
    thread may be preempted.

Arguments:

    Thread - Supplies a pointer to a dispatcher object of type thread.

    Priority - Supplies the new thread priority value.

Return Value:

    None.

--*/

{

    PRKPRCB Prcb;
    KPRIORITY ThreadPriority;
    PRKTHREAD Thread1;

    ASSERT(Priority <= HIGH_PRIORITY);

    //
    // Capture the current priority of the specified thread.
    //

    ThreadPriority = Thread->Priority;

    //
    // If the new priority is not equal to the old priority, then set the
    // new priority of the thread and redispatch a processor if necessary.
    //

    if (Priority != ThreadPriority) {
        Thread->Priority = (SCHAR)Priority;

        //
        // Case on the thread state.
        //

        switch (Thread->State) {

            //
            // Ready case - Remove the thread from its current dispatcher ready
            // queue. If the new priority is less than the old priority, then
            // insert the thread at the tail of the dispatcher ready queue
            // selected by the new priority. Else reready the thread for
            // execution.
            //

        case Ready:
            RemoveEntryList(&Thread->WaitListEntry);
            if (IsListEmpty(&KiDispatcherReadyListHead[ThreadPriority])) {
                ClearMember(ThreadPriority, KiReadySummary);
            }

            if (Priority < ThreadPriority) {
                InsertTailList(&KiDispatcherReadyListHead[Priority],
                               &Thread->WaitListEntry);
                SetMember(Priority, KiReadySummary);

            } else {
                KiReadyThread(Thread);
            }

            break;

            //
            // Standby case - If the thread's priority is being lowered, then
            // attempt to find another thread to execute. If a new thread is
            // found, then put the new thread in the standby state, and reready
            // the old thread.
            //

        case Standby:

            if (Priority < ThreadPriority) {

                Thread1 = KiFindReadyThread(Priority);

                if (Thread1 != NULL) {

                    Prcb = KeGetCurrentPrcb();

                    Thread1->State = Standby;
                    Prcb->NextThread = Thread1;
                    KiReadyThread(Thread);
                }
            }

            break;

            //
            // Running case - If there is not a thread in the standby state
            // on the thread's processor and the thread's priority is being
            // lowered, then attempt to find another thread to execute. If
            // a new thread is found, then put the new thread in the standby
            // state, and request a redispatch on the thread's processor.
            //

        case Running:

            Prcb = KeGetCurrentPrcb();

            if (Prcb->NextThread == NULL) {
                if (Priority < ThreadPriority) {

                    Thread1 = KiFindReadyThread(Priority);

                    if (Thread1 != NULL) {
                        Thread1->State = Standby;
                        Prcb->NextThread = Thread1;
                    }
                }
            }

            break;

            //
            // Initialized, Terminated, Waiting, Transition case - For
            // these states it is sufficient to just set the new thread
            // priority.
            //

        default:
            break;
        }
    }

    return;
}

VOID
KiSuspendThread (
    IN PVOID NormalContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )

/*++

Routine Description:

    This function is the kernel routine for the builtin suspend APC of a
    thread. It is executed in kernel mode as the result of queuing the builtin
    suspend APC and suspends thread execution by Waiting nonalertable on the
    thread's builtin suspend semaphore. When the thread is resumed, execution
    of thread is continued by simply returning.

Arguments:

    Apc - Supplies a pointer to a control object of type APC.

Return Value:

    None.

--*/

{

    PRKTHREAD Thread;

    //
    // Get the address of the current thread object and Wait nonalertable on
    // the thread's builtin suspend semaphore.
    //

    Thread = KeGetCurrentThread();

    KeWaitForSingleObject(&Thread->SuspendSemaphore,
                          Suspended,
                          KernelMode,
                          FALSE,
                          (PLARGE_INTEGER)NULL);

}
