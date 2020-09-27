/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    ixnmi.c

Abstract:

    Provides standard x86 NMI handler

Author:

    kenr

Revision History:

--*/

#include "halp.h"

VOID
HalHandleNMI(
    VOID
    )
/*++

Routine Description:

    Called DURING an NMI.  The system will BugCheck when an NMI occurs.
    This function can return the proper bugcheck code, bugcheck itself,
    or return success which will cause the system to iret from the nmi.

    This function is called during an NMI - no system services are available.
    In addition, you don't want to touch any spinlock which is normally
    used since we may have been interrupted while owning it, etc, etc...

Warnings:

    Do NOT:
      Make any system calls
      Attempt to acquire any spinlock used by any code outside the NMI handler
      Change the interrupt state.  Do not execute any IRET inside this code

    Passing data to non-NMI code must be done using manual interlocked
    functions.  (xchg instructions).

Arguments:

    None.

Return Value:

    None.

--*/
{
#ifdef DEVKIT
    if ((KdDebuggerNotPresent == FALSE) && (KdDebuggerEnabled != FALSE)) {
        KeEnterKernelDebugger();
    }
#endif

    HalHaltSystem();
}
