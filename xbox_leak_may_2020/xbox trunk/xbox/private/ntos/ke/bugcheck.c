/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    bugcheck.c

Abstract:

    This module implements bug check and system shutdown code.

Author:

    Mark Lucovsky (markl) 30-Aug-1990

Environment:

    Kernel mode only.

Revision History:

--*/

#include "ki.h"
#include "dm.h"

//
// Define bug count recursion counter and a context buffer.
//

#ifdef DEVKIT
ULONG KeBugCheckCount = 1;
#endif

ULONG_PTR KiBugCheckData[5];

VOID
KeBugCheck (
    IN ULONG BugCheckCode
    )

/*++

Routine Description:

    This function crashes the system in a controlled manner.

Arguments:

    BugCheckCode - Supplies the reason for the bug check.

Return Value:

    None.

--*/
{
    KeBugCheckEx(BugCheckCode,0,0,0,0);
}

VOID
KiBugCheckDebugBreak (
    IN ULONG    BreakStatus
    )
{
    do {
        try {

            //
            // Issue a breakpoint
            //

            DbgBreakPointWithStatus (BreakStatus);

        } except(EXCEPTION_EXECUTE_HANDLER) {

            //
            // Failure to issue breakpoint, halt the system
            //

            try {

                HalHaltSystem();

            } except(EXCEPTION_EXECUTE_HANDLER) {

                for (;;) {
                }
            }

            for (;;) {
            }
        }
    } while (BreakStatus != DBG_STATUS_BUGCHECK_FIRST);
}

VOID
KeBugCheckEx (
    IN ULONG BugCheckCode,
    IN ULONG_PTR BugCheckParameter1,
    IN ULONG_PTR BugCheckParameter2,
    IN ULONG_PTR BugCheckParameter3,
    IN ULONG_PTR BugCheckParameter4
    )

/*++

Routine Description:

    This function crashes the system in a controlled manner.

Arguments:

    BugCheckCode - Supplies the reason for the bug check.

    BugCheckParameter1-4 - Supplies additional bug check information

Return Value:

    None.

--*/

{
    KIRQL OldIrql;

#if defined(i386)
    KiSetHardwareTrigger();
#else
    KiHardwareTrigger = 1;
#endif

    KiBugCheckData[0] = BugCheckCode;
    KiBugCheckData[1] = BugCheckParameter1;
    KiBugCheckData[2] = BugCheckParameter2;
    KiBugCheckData[3] = BugCheckParameter3;
    KiBugCheckData[4] = BugCheckParameter4;

#ifdef DEVKIT
    //
    // Once we bugcheck, the debug monitor isn't going to have much luck with
    // anything, so we'll tell it to disable itself.
    //

    if (DmGetCurrentDmi() != NULL) {
        DmGetCurrentDmi()->Flags |= DMIFLAG_BUGCHECK;
    }
#endif

    //
    // If the user manually crashed the machine, skips the DbgPrints and
    // go to the crashdump.
    // Trying to do DbgPrint causes us to reeeter the debugger which causes
    // some problems.
    //
    // Otherwise, if the debugger is enabled, print out the information and
    // stop.
    //

    if ((BugCheckCode != MANUALLY_INITIATED_CRASH) &&
        (KdDebuggerEnabled)) {

        DbgPrint("\n*** Fatal System Error: 0x%08lx\n"
                 "                       (0x%p,0x%p,0x%p,0x%p)\n\n",
                 BugCheckCode,
                 BugCheckParameter1,
                 BugCheckParameter2,
                 BugCheckParameter3,
                 BugCheckParameter4);

        //
        // If the debugger is not actually connected, or the user manually
        // crashed the machine by typing .crash in the debugger, proceed to
        // "blue screen" the system.
        //
        // The call to DbgPrint above will have set the state of
        // KdDebuggerNotPresent if the debugger has become disconnected
        // since the system was booted.
        //

        if (KdDebuggerNotPresent == FALSE) {
            KiBugCheckDebugBreak (DBG_STATUS_BUGCHECK_FIRST);
        }
    }

#ifdef DEVKIT
    //
    // Freeze execution of the system by disabling interrupts and looping.
    //

    KiDisableInterrupts();
    KeRaiseIrql(HIGH_LEVEL, &OldIrql);

    if (InterlockedDecrement (&KeBugCheckCount) == 0) {
        if (KdDebuggerEnabled == FALSE) {
            KdInitSystem(FALSE);
        }
    }

    //
    // Attempt to enter the kernel debugger.
    //

    KiBugCheckDebugBreak (DBG_STATUS_BUGCHECK_SECOND);
#else
    //
    // Halt the system.  If a real debugger is not attached, then we cannot
    // safely use KiBugCheckDebugBreak.  That routine will generate a breakpoint
    // exception inside an exception handler.  RtlDispatchException will check
    // if the exception handler is within the bounds of the current thread's
    // stack.  If we entered this routine as a result of a fault from a stack
    // overflow exception, or a non-maskable interrupt, then we'll be executing
    // on a different stack.  RtlDispatchException will not be able to dispatch
    // the exception and will raise another bug check.
    //

    HalHaltSystem();
#endif
}

#ifdef DEVKIT

VOID
KeEnterKernelDebugger (
    VOID
    )

/*++

Routine Description:

    This function crashes the system in a controlled manner attempting
    to invoke the kernel debugger.

Arguments:

    None.

Return Value:

    None.

--*/

{

#if !defined(i386)
    KIRQL OldIrql;
#endif

    //
    // Freeze execution of the system by disabling interrupts and looping.
    //

    KiHardwareTrigger = 1;
    KiDisableInterrupts();
#if !defined(i386)
    KeRaiseIrql(HIGH_LEVEL, &OldIrql);
#endif
    if (InterlockedDecrement (&KeBugCheckCount) == 0) {
        if (KdDebuggerEnabled == FALSE) {
            KdInitSystem(FALSE);
        }
    }

    KiBugCheckDebugBreak (DBG_STATUS_FATAL);
}

#endif
