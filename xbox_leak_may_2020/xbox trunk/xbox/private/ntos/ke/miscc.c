/*++

Copyright (c) 1989-1992  Microsoft Corporation

Module Name:

    miscc.c

Abstract:

    This module implements machine independent miscellaneous kernel functions.

Author:

    David N. Cutler (davec) 13-May-1989

Environment:

    Kernel mode only.

Revision History:

--*/

#include "ki.h"

#undef KeEnterCriticalRegion
VOID
KeEnterCriticalRegion (
   VOID
   )

/*++

Routine Description:

   This function disables kernel APC's.

   N.B. The following code does not require any interlocks. There are
        two cases of interest: 1) On an MP system, the thread cannot
        be running on two processors as once, and 2) if the thread is
        is interrupted to deliver a kernel mode APC which also calls
        this routine, the values read and stored will stack and unstack
        properly.

Arguments:

   None.

Return Value:

   None.

--*/

{
    //
    // Simply directly disable kernel APCs.
    //

    KeGetCurrentThread()->KernelApcDisable -= 1;
}

#undef KeLeaveCriticalRegion
VOID
KeLeaveCriticalRegion (
    VOID
    )

/*++

Routine Description:

    This function enables kernel APC's and requests an APC interrupt if
    appropriate.

Arguments:

    None.

Return Value:

    None.

--*/

{
    //
    // Increment the kernel APC disable count. If the resultant count is
    // zero and the thread's kernel APC List is not empty, then request an
    // APC interrupt.
    //
    // For multiprocessor performance, the following code utilizes the fact
    // that queuing an APC is done by first queuing the APC, then checking
    // the AST disable count. The following code increments the disable
    // count first, checks to determine if it is zero, and then checks the
    // kernel AST queue.
    //
    // See also KiInsertQueueApc().
    //

    KiLeaveCriticalRegion();
}

ULONGLONG
KeQueryInterruptTime (
    VOID
    )

/*++

Routine Description:

    This function returns the current interrupt time by determining when the
    time is stable and then returning its value.

Arguments:

    CurrentTime - Supplies a pointer to a variable that will receive the
        current system time.

Return Value:

    None.

--*/

{
    LARGE_INTEGER CurrentTime;

    KiQueryInterruptTime(&CurrentTime);
    return CurrentTime.QuadPart;
}

VOID
KeQuerySystemTime (
    OUT PLARGE_INTEGER CurrentTime
    )

/*++

Routine Description:

    This function returns the current system time by determining when the
    time is stable and then returning its value.

Arguments:

    CurrentTime - Supplies a pointer to a variable that will receive the
        current system time.

Return Value:

    None.

--*/

{
    LARGE_INTEGER StackCurrentTime;

    KiQuerySystemTime(&StackCurrentTime);
    *CurrentTime = StackCurrentTime;
}

VOID
KeSetSystemTime (
    IN PLARGE_INTEGER NewTime,
    OUT PLARGE_INTEGER OldTime
    )

/*++

Routine Description:

    This function sets the system time to the specified value and updates
    timer queue entries to reflect the difference between the old system
    time and the new system time.

Arguments:

    NewTime - Supplies a pointer to a variable that specifies the new system
        time.

    OldTime - Supplies a pointer to a variable that will receive the previous
        system time.

Return Value:

    None.

--*/

{

    LIST_ENTRY AbsoluteListHead;
    LIST_ENTRY ExpiredListHead;
    ULONG Index;
    PLIST_ENTRY ListHead;
    PLIST_ENTRY NextEntry;
    KIRQL OldIrql1;
    KIRQL OldIrql2;
    LARGE_INTEGER TimeDelta;
    PKTIMER Timer;

    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    //
    // Set affinity to the processor that keeps the system time, raise IRQL
    // to dispatcher level and lock the dispatcher database, then raise IRQL
    // to HIGH_LEVEL to synchronize with the clock interrupt routine.
    //

    KiLockDispatcherDatabase(&OldIrql1);
    KeRaiseIrql(HIGH_LEVEL, &OldIrql2);

    //
    // Save the previous system time, set the new system time, and set
    // the realtime clock, if a time value is specified.
    //

    KiQuerySystemTime(OldTime);

    KeSystemTime.High2Time = NewTime->HighPart;
    KeSystemTime.LowPart   = NewTime->LowPart;
    KeSystemTime.High1Time = NewTime->HighPart;

    //
    // Compute the difference between the previous system time and the new
    // system time.
    //

    TimeDelta.QuadPart = NewTime->QuadPart - OldTime->QuadPart;

    //
    // Update the boot time to reflect the delta. This keeps time based
    // on boot time constant
    //

    KeBootTime.QuadPart = KeBootTime.QuadPart + TimeDelta.QuadPart;

    //
    // Track the overall bias applied to the boot time.
    //

    KeBootTimeBias = KeBootTimeBias + TimeDelta.QuadPart;

    //
    // Lower IRQL to dispatch level and if needed adjust the physical
    // system interrupt time.
    //

    KeLowerIrql(OldIrql2);

    //
    // Remove all absolute timers from the timer queue so their due time
    // can be recomputed.
    //

    InitializeListHead(&AbsoluteListHead);
    for (Index = 0; Index < TIMER_TABLE_SIZE; Index += 1) {
        ListHead = &KiTimerTableListHead[Index];
        NextEntry = ListHead->Flink;
        while (NextEntry != ListHead) {
            Timer = CONTAINING_RECORD(NextEntry, KTIMER, TimerListEntry);
            NextEntry = NextEntry->Flink;
            if (Timer->Header.Absolute != FALSE) {
                RemoveEntryList(&Timer->TimerListEntry);
                InsertTailList(&AbsoluteListHead, &Timer->TimerListEntry);
            }
        }
    }

    //
    // Recompute the due time and reinsert all absolute timers in the timer
    // tree. If a timer has already expired, then insert the timer in the
    // expired timer list.
    //

    InitializeListHead(&ExpiredListHead);
    while (AbsoluteListHead.Flink != &AbsoluteListHead) {
        Timer = CONTAINING_RECORD(AbsoluteListHead.Flink, KTIMER, TimerListEntry);
        KiRemoveTreeTimer(Timer);
        Timer->DueTime.QuadPart -= TimeDelta.QuadPart;
        if (KiReinsertTreeTimer(Timer, Timer->DueTime) == FALSE) {
            Timer->Header.Inserted = TRUE;
            InsertTailList(&ExpiredListHead, &Timer->TimerListEntry);
        }
    }

    //
    // If any of the attempts to reinsert a timer failed, then timers have
    // already expired and must be processed.
    //
    // N.B. The following function returns with the dispatcher database
    //      unlocked.
    //

    KiTimerListExpire(&ExpiredListHead, OldIrql1);
}
