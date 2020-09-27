/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    time.c

Abstract:

    WinDbg Extension Api

Author:

    Ramon J San Andres (ramonsa) 8-Nov-1993

Environment:

    User Mode.

Revision History:

--*/


#include "precomp.h"
#pragma hdrstop


typedef struct _ETIMER {
    KTIMER KeTimer;
    KAPC TimerApc;
    KDPC TimerDpc;
    LIST_ENTRY ActiveTimerListEntry;
    KSPIN_LOCK Lock;
    LONG Period;
    BOOLEAN ApcAssociated;
    BOOLEAN WakeTimer;
    LIST_ENTRY WakeTimerListEntry;
} ETIMER, *PETIMER;

VOID
FileTimeToString(
    IN LARGE_INTEGER Time,
    IN BOOLEAN TimeZone,
    OUT PCHAR Buffer
    );

DECLARE_API( time )

/*++

Routine Description:



Arguments:

    args -

Return Value:

    None

--*/

{
    struct {
        ULONG   Low;
        ULONG   High;
    } rate;

    struct {
        ULONG   Low;
        ULONG   High;
    } diff;

    ULONG   rateaddr;
    ULONG   diffaddr;
    ULONG   result;
    ULONG   TicksPerNs;

    rateaddr = GetExpression( "xboxkrnl!KdPerformanceCounterRate" );
    if ( !rateaddr ||
         !ReadMemory( (DWORD)rateaddr, &rate, sizeof(rate), &result) ) {
        dprintf("%08lx: Unable to get value of KdPerformanceCounterRate\n",rateaddr);
        return;
    }

    diffaddr = GetExpression( "xboxkrnl!KdTimerDifference" );
    if ( !diffaddr ||
         !ReadMemory( (DWORD)diffaddr, &diff, sizeof(diff), &result) ) {
        dprintf("%08lx: Unable to get value of KdTimerDifference\n",diffaddr);
        return;
    }

    TicksPerNs = 1000000000L / rate.Low;

    if (diff.High == 0L) {
        dprintf("%ld ticks at %ld ticks/second (%ld ns)\n",
                diff.Low,
                rate.Low,
                diff.Low * TicksPerNs);
    } else {
        dprintf("%08lx:%08lx ticks at %ld ticks/second\n",
                diff.High, diff.Low, rate.Low);
    }
}

PLIST_ENTRY
DumpKTimer(
    IN PKTIMER pTimer,
    IN ULONGLONG InterruptTimeOffset,
    IN OPTIONAL PLIST_ENTRY Blink
    )
{
    KTIMER KTimer;
    ULONG           Displacement;
    KDPC Dpc;
    CHAR            Buff[256];
    ULONG           Result;
    PLIST_ENTRY     NextThread;
    LIST_ENTRY      ListEntry;
    PKTHREAD        Thread;
    KWAIT_BLOCK     WaitBlock;
    LARGE_INTEGER   SystemTime;

    if ( !ReadMemory( (DWORD)pTimer,
                      &KTimer,
                      sizeof(KTIMER),
                      &Result) ) {
        dprintf("Unable to get contents of Timer @ %lx\n", pTimer );
        return(NULL);
    }

    SystemTime.QuadPart = KTimer.DueTime.QuadPart + InterruptTimeOffset;
    if (SystemTime.QuadPart < 0) {
        strcpy(Buff, "         NEVER         ");
    } else {
        FileTimeToString(SystemTime, FALSE, Buff);
    }

    dprintf("%c %08lx %08lx [%s]  ", 
            (KTimer.Period != 0) ? 'P' : ' ',
            KTimer.DueTime.LowPart,
            KTimer.DueTime.HighPart,
            Buff);

    if (KTimer.Dpc != NULL) {
        if (!ReadMemory((DWORD)(KTimer.Dpc),
                        &Dpc,
                        sizeof(KDPC),
                        &Result)) {
            dprintf("Unable to get contents of DPC @ %lx\n", KTimer.Dpc);
            return(NULL);
        }
        GetSymbol(Dpc.DeferredRoutine,
                  Buff,
                  &Displacement);
        dprintf("%s",Buff);
        if (Displacement != 0) {
            dprintf("+%lx ", Displacement);
        } else {
            dprintf(" ");
        }
    }

    NextThread = KTimer.Header.WaitListHead.Flink;
    while (NextThread != &pTimer->Header.WaitListHead) {
        if (!ReadMemory((DWORD)NextThread,
                        &WaitBlock,
                        sizeof(WaitBlock),
                        &Result)) {
            dprintf("Unable to get contents of waitblock @ %lx\n",NextThread);
        } else {
            dprintf("thread %lx ",WaitBlock.Thread);
        }

        if (!ReadMemory((DWORD)NextThread,
                        &ListEntry,
                        sizeof(ListEntry),
                        &Result)) {
            dprintf("Unable to read next WaitListEntry @ %lx\n",NextThread);
            break;
        }
        NextThread = ListEntry.Flink;
    }
    dprintf("\n");

    if (Blink &&
        (KTimer.TimerListEntry.Blink != Blink)) {
        dprintf("   Timer at %lx has wrong Blink! (Blink %08lx, should be %08lx)\n",
                pTimer,
                KTimer.TimerListEntry.Blink,
                Blink);
    }

    if (KTimer.TimerListEntry.Flink == NULL) {
        dprintf("   Timer at %lx has been zeroed! (Flink %08lx, Blink %08lx)\n",
                pTimer,
                KTimer.TimerListEntry.Flink,
                KTimer.TimerListEntry.Blink);
    }

    return(KTimer.TimerListEntry.Flink);

}



DECLARE_API( timer )

/*++

Routine Description:

    Dumps all timers in the system.

Arguments:

    args -

Return Value:

    None

--*/

{
    ULONG           CurrentList;
    KTIMER          CurrentTimer;
    ULONG           Index;
    LARGE_INTEGER   InterruptTime;
    LARGE_INTEGER   SystemTime;
    ULONG           MaximumList;
    ULONG           MaximumSearchCount;
    ULONG           MaximumTimerCount;
    PLIST_ENTRY     NextEntry;
    PLIST_ENTRY     LastEntry;
    PLIST_ENTRY     p;
    PKTIMER         NextTimer;
    ULONG_PTR           KeTickCount;
    ULONG           KiMaximumSearchCount;
    ULONG           Result;
    ULONG           TickCount;
    PLIST_ENTRY     TimerTable;
    ULONG           TotalTimers;
    KSYSTEM_TIME    _SystemTime;
    KSYSTEM_TIME    _InterruptTime;
    LIST_ENTRY      ListEntry;
    ULONG_PTR     WakeTimerList;
    ETIMER          ETimer;
    ETIMER          *pETimer;
    CHAR            Buffer[256];
    ULONGLONG       InterruptTimeOffset;

    //
    // Get the system time and print the header banner.
    //
    if (!ReadMemory( GetNtDebuggerData( KeSystemTime ),
                     &_SystemTime,
                     sizeof(_SystemTime),
                     &Result) ) {
        dprintf("Unable to get system time\n");
        return;
    }
    if (!ReadMemory( GetNtDebuggerData( KeInterruptTime ),
                     &_InterruptTime,
                     sizeof(_InterruptTime),
                     &Result) ) {
        dprintf("Unable to get interrupt time\n");
        return;
    }

#ifdef TARGET_ALPHA
    InterruptTime.QuadPart = SharedData.InterruptTime;
    SystemTime.QuadPart = SharedData.SystemTime;
#else
    InterruptTime.HighPart = _InterruptTime.High1Time;
    InterruptTime.LowPart = _InterruptTime.LowPart;
    SystemTime.HighPart = _SystemTime.High1Time;
    SystemTime.LowPart = _SystemTime.LowPart;
#endif
    InterruptTimeOffset = SystemTime.QuadPart - InterruptTime.QuadPart;
    FileTimeToString(SystemTime, TRUE, Buffer);

    dprintf("Dump system timers\n\n");
    dprintf("Interrupt time: %08lx %08lx [%s]\n\n",
            InterruptTime.LowPart,
            InterruptTime.HighPart,
            Buffer);

    //
    // Get the address of the timer table list head array and scan each
    // list for timers.
    //

    dprintf("List Timer    Interrupt Low/High     Fire Time              DPC/thread\n");
    MaximumList = 0;

    TimerTable = (PLIST_ENTRY)GetNtDebuggerData( KiTimerTableListHead );
    if ( !TimerTable ) {
        dprintf("Unable to get value of KiTimerTableListHead\n");
        return;
    }

    TotalTimers = 0;
    for (Index = 0; Index < TIMER_TABLE_SIZE; Index += 1) {

        //
        // Read the forward link in the next timer table list head.
        //

        if ( !ReadMemory( (DWORD)TimerTable,
                          &NextEntry,
                          sizeof(PLIST_ENTRY),
                          &Result) ) {
            dprintf("Unable to get contents of next entry @ %lx\n", TimerTable );
            continue;
        }

        //
        // Scan the current timer list and display the timer values.
        //

        LastEntry = TimerTable;
        CurrentList = 0;
        while (NextEntry != TimerTable) {
            CurrentList += 1;
            NextTimer = CONTAINING_RECORD(NextEntry, KTIMER, TimerListEntry);
            TotalTimers += 1;

            if (CurrentList == 1) {
                dprintf("%3ld %08lx ", Index, NextTimer);
            } else {
                dprintf("    %08lx ", NextTimer);
            }

            p = LastEntry;
            LastEntry = NextEntry;
            NextEntry = DumpKTimer(NextTimer, InterruptTimeOffset, p);
            if (NextEntry==NULL) {
                break;
            }
        }

        TimerTable += 1;
        if (CurrentList > MaximumList) {
            MaximumList = CurrentList;
        }
    }

    dprintf("\n\nTotal Timers: %d, Maximum List: %d\n",
            TotalTimers,
            MaximumList);

    //
    // Get the current tick count and convert to the hand value.
    //

    KeTickCount = GetExpression( "xboxkrnl!KeTickCount" );
    if ( KeTickCount &&
         ReadMemory( (DWORD)KeTickCount,
                      &TickCount,
                      sizeof(ULONG),
                      &Result) ) {
        dprintf("Current Hand: %d", TickCount & (TIMER_TABLE_SIZE - 1));
    }

    //
    // Get the maximum search count if the target system is a checked
    // build and display the count.
    //

    KiMaximumSearchCount = GetExpression( "xboxkrnl!KiMaximumSearchCount" );
    if ( KiMaximumSearchCount &&
         ReadMemory( (DWORD)KiMaximumSearchCount,
                     &MaximumSearchCount,
                     sizeof(ULONG),
                     &Result) ) {
        dprintf(", Maximum Search: %d", MaximumSearchCount);
    }

    dprintf("\n\n");

    return;
}

VOID
FileTimeToString(
    IN LARGE_INTEGER Time,
    IN BOOLEAN TimeZone,
    OUT PCHAR Buffer
    )
{
    TIME_FIELDS TimeFields;
    TIME_ZONE_INFORMATION TimeZoneInfo;
    PWCHAR pszTz;
    ULONGLONG TzBias;
    DWORD Result;

    //
    // Get the local (to the debugger) timezone bias
    //
    Result = GetTimeZoneInformation(&TimeZoneInfo);
    if (Result == 0xffffffff) {
        pszTz = L"UTC";
    } else {
        //
        // Bias is in minutes, convert to 100ns units
        //
        TzBias = (ULONGLONG)TimeZoneInfo.Bias * 60 * 10000000;
        switch (Result) {
            case TIME_ZONE_ID_UNKNOWN:
                pszTz = L"unknown";
                break;
            case TIME_ZONE_ID_STANDARD:
                pszTz = TimeZoneInfo.StandardName;
                break;
            case TIME_ZONE_ID_DAYLIGHT:
                pszTz = TimeZoneInfo.DaylightName;
                break;
        }

        Time.QuadPart -= TzBias;
    }

    RtlTimeToTimeFields(&Time, &TimeFields);
    if (TimeZone) {
        sprintf(Buffer, "%2d/%2d/%d %02d:%02d:%02d.%03d (%ws)",
                TimeFields.Month,
                TimeFields.Day,
                TimeFields.Year,
                TimeFields.Hour,
                TimeFields.Minute,
                TimeFields.Second,
                TimeFields.Milliseconds,
                pszTz);
    } else {
        sprintf(Buffer, "%2d/%2d/%d %02d:%02d:%02d.%03d",
                TimeFields.Month,
                TimeFields.Day,
                TimeFields.Year,
                TimeFields.Hour,
                TimeFields.Minute,
                TimeFields.Second,
                TimeFields.Milliseconds);
    }

}


DECLARE_API( filetime )
    
/*++

Routine Description:

    Reformats a 64-bit NT time (FILETIME) as something a human
    being can understand

Arguments:

    args - 64-bit filetime to reformat

Return Value:

    None

--*/

{
    LARGE_INTEGER Time;
    CHAR Buffer[256];

    Time.QuadPart = 0;
    sscanf(args, "%I64X", &Time);

    if (Time.QuadPart == 0) {
        dprintf("!filetime <64-bit FILETIME>\n");
    } else {
        FileTimeToString(Time,TRUE, Buffer);
        dprintf("%s\n",Buffer);
    }
}

// BUGBUG: In order to avoid any references to ntdll (even those that will
// later be discarded, define ZwQuerySystemInformation below so the reference
// in ntos\rtl\time.c will be resolved.

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation (
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    OUT PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    ) {
        return((NTSTATUS)-1);
}

// BUGBUG: Similarly, implement RtlRaiseStatus for the largeint code on X86.  Make
// it call through Kernel32..

VOID
RtlRaiseStatus (
    IN NTSTATUS Status
    )
{
    RaiseException((DWORD) Status, EXCEPTION_NONCONTINUABLE, (DWORD) 0, (DWORD *) NULL);
}
