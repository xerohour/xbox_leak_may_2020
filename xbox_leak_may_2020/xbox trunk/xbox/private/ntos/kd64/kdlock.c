/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    kdlock.c

Abstract:

    This module contains code to synchronize the usage of the port
    used by the kernel debugger.

Author:

    Bryan M. Willman (bryanwi) 24-Sep-90

Revision History:

--*/

#include "kdp.h"
#include "dm.h"

BOOLEAN
KdPollBreakIn(
    VOID
    )

/*++

Routine Description:

    This procedure raises IRQL to high_level, seizes the Debug port
    spinlock, and checks to see if a breakin packet is pending.
    If a packet is present, return TRUE, else FALSE.

    A packet is present if:

    There is a valid character which matches BREAK_CHAR.

    N.B.    Interrupts must be OFF around this call

Return Value:

    TRUE if breakin sequence present, caller should execute int-3.
    FALSE if no breakin seen.

--*/

{
    BOOLEAN BreakIn;
    BOOLEAN Enable;
    UCHAR   Input;
    KIRQL   OldIrql;
    ULONG   Status;

    //
    // If the debugger is enabled, see if a breakin by the kernel
    // debugger is pending.
    //

    BreakIn = FALSE;
    if (KdDebuggerEnabled != FALSE) {
        Enable = KiDisableInterrupts();
#ifndef _X86_
        KeRaiseIrql(HIGH_LEVEL, &OldIrql);
#endif
        if (KdpControlCPending != FALSE) {
            KdpControlCPressed = TRUE;
            BreakIn = TRUE;
            KdpControlCPending = FALSE;

        } else {
            KdpPortLock();
            Status = KdPortPollByte(&Input);
            if ((Status == CP_GET_SUCCESS) &&
                (Input == BREAKIN_PACKET_BYTE)) {
                BreakIn = TRUE;
                KdpControlCPressed = TRUE;
            }
            KdpPortUnlock();
        }

#ifndef _X86_
        KeLowerIrql(OldIrql);
#endif
        KiRestoreInterrupts(Enable);
    }

    if(BreakIn)
        /* We can probably assume that the debugger is reconnected */
        KdDebuggerNotPresent = FALSE;

#ifdef DEVKIT
    /* If we're going to break in, give the debug monitor a chance to do its
     * own version of break in */
    if(BreakIn && KeGetCurrentPrcb())
        DmTell(DMTELL_ENTERDEBUGGER, NULL);
#endif

    return BreakIn;
}

BOOLEAN
KdpPollBreakInWithPortLock(
    VOID
    )

/*++

Routine Description:

    This procedure same as KdPollBreakIn, but assumes the caller
    already holds the port lock.  Returns TRUE if a breakin packet
    is pending.

    A packet is present if:

    There is a valid character which matches BREAK_CHAR.

    N.B.    Interrupts must be OFF around this call

Return Value:

    TRUE if breakin sequence present, caller should execute int-3.
    FALSE if no breakin seen.

--*/

{

    BOOLEAN BreakIn;
    BOOLEAN Enable;
    UCHAR Input;
    ULONG Status;

    //
    // If the debugger is enabled, see if a breakin by the kernel
    // debugger is pending.
    //

    BreakIn = FALSE;
    if (KdDebuggerEnabled != FALSE) {
        if (KdpControlCPending != FALSE) {
            BreakIn = TRUE;
            KdpControlCPending = FALSE;

        } else {
            Status = KdPortPollByte(&Input);
            if ((Status == CP_GET_SUCCESS) &&
                (Input == BREAKIN_PACKET_BYTE)) {
                BreakIn = TRUE;
            }
        }
    }

    return BreakIn;
}
