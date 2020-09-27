/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    procobj.c

Abstract:

    This module implements the machine independent functions to manipulate
    the kernel process object. Functions are provided to initialize, attach,
    detach, exclude, include, and set the base priority of process objects.

Author:

    David N. Cutler (davec) 7-Mar-1989

Environment:

    Kernel mode only.

Revision History:

--*/

#include "ki.h"

VOID
KeInitializeProcess (
    IN PRKPROCESS Process,
    IN KPRIORITY BasePriority
    )

/*++

Routine Description:

    This function initializes a kernel process object. The base priority,
    affinity, and page frame numbers for the process page table directory
    and hyper space are stored in the process object.

    N.B. It is assumed that the process object is zeroed.

Arguments:

    Process - Supplies a pointer to a dispatcher object of type process.

    BasePriority - Supplies the base priority of the process.

Return Value:

    None.

--*/

{

    //
    // Initialize the base priority.
    //

    Process->BasePriority = (SCHAR)BasePriority;

    //
    // Initialize the stack count, profile listhead, ready queue list head,
    // accumulated runtime, process quantum, thread quantum, and thread list
    // head.
    //

    InitializeListHead(&Process->ReadyListHead);
    InitializeListHead(&Process->ThreadListHead);
    Process->ThreadQuantum = THREAD_QUANTUM;

    return;
}

KPRIORITY
KeSetPriorityProcess (
    IN PKPROCESS Process,
    IN KPRIORITY NewBase
    )

/*++

Routine Description:

    This function set the base priority of a process to a new value
    and adjusts the priority and base priority of all child threads
    as appropriate.

Arguments:

    Process - Supplies a pointer to a dispatcher object of type process.

    NewBase - Supplies the new base priority of the process.

Return Value:

    The previous base priority of the process.

--*/

{

    KPRIORITY Adjustment;
    PLIST_ENTRY NextEntry;
    KPRIORITY NewPriority;
    KIRQL OldIrql;
    KPRIORITY OldBase;
    PKTHREAD Thread;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Raise IRQL to dispatcher level and lock dispatcher database.
    //

    KiLockDispatcherDatabase(&OldIrql);

    //
    // Save the current process base priority, set the new process base
    // priority, compute the adjustment value, and adjust the priority
    // and base priority of all child threads as appropriate.
    //

    OldBase = Process->BasePriority;
    Process->BasePriority = (SCHAR)NewBase;
    Adjustment = NewBase - OldBase;
    NextEntry = Process->ThreadListHead.Flink;
    if (NewBase >= LOW_REALTIME_PRIORITY) {
        while (NextEntry != &Process->ThreadListHead) {
            Thread = CONTAINING_RECORD(NextEntry, KTHREAD, ThreadListEntry);

            //
            // Compute the new base priority of the thread.
            //

            NewPriority = Thread->BasePriority + Adjustment;

            //
            // If the new base priority is outside the realtime class,
            // then limit the change to the realtime class.
            //

            if (NewPriority < LOW_REALTIME_PRIORITY) {
                NewPriority = LOW_REALTIME_PRIORITY;

            } else if (NewPriority > HIGH_PRIORITY) {
                NewPriority = HIGH_PRIORITY;
            }

            //
            // Set the base priority and the current priority of the
            // thread to the appropriate value.
            //
            // N.B. If priority saturation occured the last time the thread
            //      base priority was set and the new process base priority
            //      is not crossing from variable to realtime, then it is not
            //      necessary to change the thread priority.
            //

            if ((Thread->Saturation == 0) || (OldBase < LOW_REALTIME_PRIORITY)) {
                if (Thread->Saturation > 0) {
                    NewPriority = HIGH_PRIORITY;

                } else if (Thread->Saturation < 0) {
                    NewPriority = LOW_REALTIME_PRIORITY;
                }

                Thread->BasePriority = (SCHAR)NewPriority;
                Thread->Quantum = Process->ThreadQuantum;
                Thread->DecrementCount = 0;
                Thread->PriorityDecrement = 0;
                KiSetPriorityThread(Thread, NewPriority);
            }

            NextEntry = NextEntry->Flink;
        }

    } else {
        while (NextEntry != &Process->ThreadListHead) {
            Thread = CONTAINING_RECORD(NextEntry, KTHREAD, ThreadListEntry);

            //
            // Compute the new base priority of the thread.
            //

            NewPriority = Thread->BasePriority + Adjustment;

            //
            // If the new base priority is outside the variable class,
            // then limit the change to the variable class.
            //

            if (NewPriority >= LOW_REALTIME_PRIORITY) {
                NewPriority = LOW_REALTIME_PRIORITY - 1;

            } else if (NewPriority <= LOW_PRIORITY) {
                NewPriority = 1;
            }

            //
            // Set the base priority and the current priority of the
            // thread to the computed value and reset the thread quantum.
            //
            // N.B. If priority saturation occured the last time the thread
            //      base priority was set and the new process base priority
            //      is not crossing from realtime to variable, then it is not
            //      necessary to change the thread priority.
            //

            if ((Thread->Saturation == 0) || (OldBase >= LOW_REALTIME_PRIORITY)) {
                if (Thread->Saturation > 0) {
                    NewPriority = LOW_REALTIME_PRIORITY - 1;

                } else if (Thread->Saturation < 0) {
                    NewPriority = 1;
                }

                Thread->BasePriority = (SCHAR)NewPriority;
                Thread->Quantum = Process->ThreadQuantum;
                Thread->DecrementCount = 0;
                Thread->PriorityDecrement = 0;
                KiSetPriorityThread(Thread, NewPriority);
            }

            NextEntry = NextEntry->Flink;
        }
    }

    //
    // Unlock dispatcher database and lower IRQL to its previous
    // value.
    //

    KiUnlockDispatcherDatabase(OldIrql);

    //
    // Return previous process base priority
    //

    return OldBase;
}
