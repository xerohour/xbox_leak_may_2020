/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    kernldat.c

Abstract:

    This module contains the declaration and allocation of kernel data
    structures.

Author:

    David N. Cutler (davec) 12-Mar-1989

Revision History:

--*/
#include "ki.h"

//
// The following data is read/write data that is grouped together for
// performance. The layout of this data is important and must not be
// changed.
//
// KiDispatcherReadyListHead - This is an array of type list entry. The
//      elements of the array are indexed by priority. Each element is a list
//      head for a set of threads that are in a ready state for the respective
//      priority. This array is used by the find next thread code to speed up
//      search for a ready thread when a thread becomes unrunnable. See also
//      KiReadySummary.
//

LIST_ENTRY KiDispatcherReadyListHead[MAXIMUM_PRIORITY];

//
// KiIdleSummary - This is the set of processors that are idle. It is used by
//      the ready thread code to speed up the search for a thread to preempt
//      when a thread becomes runnable.
//

KAFFINITY KiIdleSummary;

//
// KiReadySummary - This is the set of dispatcher ready queues that are not
//      empty. A member is set in this set for each priority that has one or
//      more entries in its respective dispatcher ready queues.
//

ULONG KiReadySummary;

//
// KiTimerTableListHead - This is a array of list heads that anchor the
//      individual timer lists.
//

LIST_ENTRY KiTimerTableListHead[TIMER_TABLE_SIZE];

//
// KeBootTime - This is the absolute time when the system was booted.
//

LARGE_INTEGER KeBootTime;

//
// KeBootTimeBias - The time for which KeBootTime has ever been biased
//

ULONGLONG KeBootTimeBias;

//
// KeTimeIncrement - This is the nominal number of 100ns units that are to
//      be added to the system time at each interval timer interupt. This
//      value is set by the HAL and is used to compute the dure time for
//      timer table entries.
//

const ULONG KeTimeIncrement = CLOCK_TIME_INCREMENT;

//
// KiMaximumSearchCount - this is the maximum number of timers entries that
//      have had to be examined to insert in the timer tree.
//

ULONG KiMaximumSearchCount;

//
// KiDebugRoutine - This is the address of the kernel debugger. Initially
//      this is filled with the address of a routine that just returns. If
//      the system debugger is present in the system, then it sets this
//      location to the address of the systemn debugger's routine.
//

PKDEBUG_ROUTINE KiDebugRoutine;

//
// KiFindLeftNibbleBitTable - This a table that is used to find the left most bit in
//      a 4-bit nibble.
//

const UCHAR KiFindLeftNibbleBitTable[] = {0, 0, 1, 1, 2, 2, 2, 2,
                                          3, 3, 3, 3, 3, 3, 3, 3};

//
// KiTimerExpireDpc - This is the Deferred Procedure Call (DPC) object that
//      is used to process the timer queue when a timer has expired.
//

KDPC KiTimerExpireDpc;

//
// KiTimeIncrementReciprocal - This is the reciprocal fraction of the time
//      increment value that is specified by the HAL when the system is
//      booted.
//
// The following is the number for a CLOCK_TIME_INCREMENT of 10000.
//

const LARGE_INTEGER KiTimeIncrementReciprocal = { 0xE219652C, 0xD1B71758 };

//
// KiTimeIncrementShiftCount - This is the shift count that corresponds to
//      the time increment reciprocal value.
//
// The following is the number for a CLOCK_TIME_INCREMENT of 10000.
//

const CCHAR KiTimeIncrementShiftCount = 13;

//
// KiWaitInListHead - This is a list of threads that are waiting with a
//      resident kernel stack.
//

LIST_ENTRY KiWaitInListHead;

//
// KiHardwareTrigger -
//

ULONG KiHardwareTrigger;

//
// KeTickCount - This is the number of clock ticks that have occurred since
//      the system was booted. This count is used to compute a millisecond
//      tick counter.
//

volatile ULONG KeTickCount;

//
// KeInterruptTime -
//

volatile KSYSTEM_TIME KeInterruptTime;

//
// KeSystemTime -
//

volatile KSYSTEM_TIME KeSystemTime;

//
// KeHasQuickBooted - This indicates that the system is initializing or has
//      initialized due to a quick boot.
//

BOOLEAN KeHasQuickBooted;

//
// KiIdleProcess - This is the process that holds the idle thread.
//

KPROCESS KiIdleProcess;

//
// KiSystemProcess - This is the process that holds all of the other threads.
//

KPROCESS KiSystemProcess;
