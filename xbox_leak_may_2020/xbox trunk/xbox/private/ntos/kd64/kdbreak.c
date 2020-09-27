/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    kdbreak.c

Abstract:

    This module implements machine dependent functions to add and delete
    breakpoints from the kernel debugger breakpoint table.

Author:

    David N. Cutler 2-Aug-1990

Revision History:

--*/

#include "kdp.h"

ULONG
KdpAddBreakpoint (
    IN PVOID Address
    )

/*++

Routine Description:

    This routine adds an entry to the breakpoint table and returns a handle
    to the breakpoint table entry.

Arguments:

    Address - Supplies the address where to set the breakpoint.

Return Value:

    A value of zero is returned if the specified address is already in the
    breakpoint table, there are no free entries in the breakpoint table, the
    specified address is not correctly aligned, or the specified address is
    not valid. Otherwise, the index of the assigned breakpoint table entry
    plus one is returned as the function value.

--*/

{

    KDP_BREAKPOINT_TYPE Content;
    ULONG Index;
    HARDWARE_PTE Opaque;
    PVOID AccessAddress;

    //DPRINT(("KD: Setting breakpoint at 0x%08x\n", Address));

    //
    // If the specified address is not properly aligned, then return zero.
    //

    if (((ULONG_PTR)Address & KDP_BREAKPOINT_ALIGN) != 0) {
        return 0;
    }


    //
    // Don't allow setting the same breakpoint twice.
    //

    for (Index = 0; Index < BREAKPOINT_TABLE_SIZE; Index += 1) {
        if ((KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_IN_USE) != 0 &&
            KdpBreakpointTable[Index].Address == Address) {

            if ((KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_NEEDS_REPLACE) != 0) {

                //
                // Breakpoint was set, the page was written out and was not
                // accessible when the breakpoint was cleared.  Now the breakpoint
                // is being set again.  Just clear the defer flag:
                //
                KdpBreakpointTable[Index].Flags &= ~KD_BREAKPOINT_NEEDS_REPLACE;
                return Index + 1;

            } else {

                DPRINT(("KD: Attempt to set breakpoint %08x twice!\n", Address));
                return 0;

            }
        }
    }

    //
    // Search the breakpoint table for a free entry.
    //

    for (Index = 0; Index < BREAKPOINT_TABLE_SIZE; Index += 1) {
        if (KdpBreakpointTable[Index].Flags == 0) {
            break;
        }
    }

    //
    // If a free entry was found, then write breakpoint and return the handle
    // value plus one. Otherwise, return zero.
    //

    if (Index == BREAKPOINT_TABLE_SIZE) {
        DPRINT(("KD: ran out of breakpoints!\n"));
        return 0;
    }


    //DPRINT(("KD: using Index %d\n", Index));

    //
    // Get the instruction to be replaced. If the instruction cannot be read,
    // then mark breakpoint as not accessible.
    //

    if (KdpMoveMemory(
            (PCHAR)&Content,
            (PCHAR)Address,
            sizeof(KDP_BREAKPOINT_TYPE) ) != sizeof(KDP_BREAKPOINT_TYPE)) {
        AccessAddress = NULL;
        //DPRINT(("KD: memory inaccessible\n"));
    } else {
        //DPRINT(("KD: memory readable...\n"));

        //
        // If the specified address is not write accessible, then return zero.
        // All references must be made through AccessAddress.
        //

        AccessAddress = MmDbgWriteCheck((PVOID)Address, &Opaque);
        if (AccessAddress == NULL) {
            DPRINT(("KD: memory not writable!\n"));
            return 0;
        }
    }

    if ( AccessAddress != NULL ) {
        KdpBreakpointTable[Index].Address = Address;
        KdpBreakpointTable[Index].Content = Content;
        KdpBreakpointTable[Index].Flags = KD_BREAKPOINT_IN_USE;
        if (KdpMoveMemory(
                (PCHAR)AccessAddress,
                (PCHAR)&KdpBreakpointInstruction,
                sizeof(KDP_BREAKPOINT_TYPE)
                ) != sizeof(KDP_BREAKPOINT_TYPE)) {

            DPRINT(("KD: KdpMoveMemory failed writing BP!\n"));
        }
        MmDbgReleaseAddress(AccessAddress, &Opaque);
    } else {
        KdpBreakpointTable[Index].Address = Address;
        KdpBreakpointTable[Index].Flags = KD_BREAKPOINT_IN_USE | KD_BREAKPOINT_NEEDS_WRITE;
        KdpOweBreakpoint = TRUE;
        //DPRINT(("KD: breakpoint write deferred\n"));
    }

    return Index + 1;

}

VOID
KdSetOwedBreakpoints(
    VOID
    )

/*++

Routine Description:

    This function is called after returning from memory management calls
    that may cause an inpage.  Its purpose is to store pending
    breakpoints in pages just made valid.

Arguments:

    None.

Return Value:

    None.

--*/

{

    KDP_BREAKPOINT_TYPE Content;
    BOOLEAN Enable;
    LONG Index;
    HARDWARE_PTE Opaque;
    PVOID AccessAddress;

    //
    // If we don't owe any breakpoints then return
    //

    if ( !KdpOweBreakpoint ) {
        return;
    }

    //
    // Freeze all other processors, disable interrupts, and save debug
    // port state.
    //

    Enable = KdEnterDebugger(NULL, NULL);
    KdpOweBreakpoint = FALSE;
    AccessAddress = NULL;

    //
    // Search the breakpoint table for breakpoints that need to be
    // written or replaced.
    //

    for (Index = 0; Index < BREAKPOINT_TABLE_SIZE; Index += 1) {
        if (KdpBreakpointTable[Index].Flags &
                (KD_BREAKPOINT_NEEDS_WRITE | KD_BREAKPOINT_NEEDS_REPLACE) ) {

            //
            // Breakpoint needs to be written
            //
            //DPRINT(("KD: Breakpoint %d at 0x%08x: trying to %s after page in.\n",
            //    Index,
            //    KdpBreakpointTable[Index].Address,
            //    (KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_NEEDS_WRITE) ?
            //        "set" : "clear"));

            //
            // Check to see if we have write access to the memory
            //

            AccessAddress = MmDbgWriteCheck((PVOID)KdpBreakpointTable[Index].Address, &Opaque);

            if (AccessAddress == NULL) {
                KdpOweBreakpoint = TRUE;
                //DPRINT(("KD: address not writeable.\n"));
                break;
            }

            //
            // Breakpoint is global, or its directory base matches
            //

            if (KdpMoveMemory(
                    (PCHAR)&Content,
                    (PCHAR)AccessAddress,
                    sizeof(KDP_BREAKPOINT_TYPE)
                    ) != sizeof(KDP_BREAKPOINT_TYPE)) {

                //
                // Memory is still inaccessible (is this possible after
                // the call above to MmDbgWriteCheck?)
                //

                DPRINT(("KD: read from 0x%08x failed\n", KdpBreakpointTable[Index].Address));

                KdpOweBreakpoint = TRUE;

            } else {
                if (KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_NEEDS_WRITE) {
                    KdpBreakpointTable[Index].Content = Content;
                    if (KdpMoveMemory(
                            (PCHAR)AccessAddress,
                            (PCHAR)&KdpBreakpointInstruction,
                            sizeof(KDP_BREAKPOINT_TYPE)
                            ) != sizeof(KDP_BREAKPOINT_TYPE)) {
                        KdpOweBreakpoint = TRUE;
                        DPRINT(("KD: write to 0x%08x failed\n", KdpBreakpointTable[Index].Address));
                    } else {
                        KdpBreakpointTable[Index].Flags = KD_BREAKPOINT_IN_USE;
                        DPRINT(("KD: write to 0x%08x ok\n", KdpBreakpointTable[Index].Address));
                    }
                } else {
                    if (KdpMoveMemory(
                            (PCHAR)AccessAddress,
                            (PCHAR)&KdpBreakpointTable[Index].Content,
                            sizeof(KDP_BREAKPOINT_TYPE)
                            ) != sizeof(KDP_BREAKPOINT_TYPE)) {
                        KdpOweBreakpoint = TRUE;
                        DPRINT(("KD: write to 0x%08x failed\n", KdpBreakpointTable[Index].Address));
                    } else {
                        //DPRINT(("KD: write to 0x%08x ok\n", KdpBreakpointTable[Index].Address));
                        if (KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_SUSPENDED) {
                            KdpBreakpointTable[Index].Flags = KD_BREAKPOINT_SUSPENDED | KD_BREAKPOINT_IN_USE;
                        } else {
                            KdpBreakpointTable[Index].Flags = 0;
                        }
                    }
                }
            }

            if (AccessAddress != NULL) {
                MmDbgReleaseAddress(
                        AccessAddress,
                        &Opaque
                        );
                AccessAddress = NULL;
            }
        }
    }

    if (AccessAddress != NULL) {
        MmDbgReleaseAddress(
                AccessAddress,
                &Opaque
                );
    }

    KdExitDebugger(Enable);
    return;
}

BOOLEAN
KdpLowWriteContent (
    IN ULONG Index
    )

/*++

Routine Description:

    This routine attempts to replace the code that a breakpoint is
    written over.  This routine, KdpAddBreakpoint,
    KdpLowRestoreBreakpoint and KdSetOwedBreakpoints are responsible
    for getting data written as requested.  Callers should not
    examine or use KdpOweBreakpoints, and they should not set the
    NEEDS_WRITE or NEEDS_REPLACE flags.

    Callers must still look at the return value from this function,
    however: if it returns FALSE, the breakpoint record must not be
    reused until KdSetOwedBreakpoints has finished with it.

Arguments:

    Index - Supplies the index of the breakpoint table entry
        which is to be deleted.

Return Value:

    Returns TRUE if the breakpoint was removed, FALSE if it was deferred.

--*/

{
    //
    // Do the contents need to be replaced at all?
    //

    if (KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_NEEDS_WRITE) {

        //
        // The breakpoint was never written out.  Clear the flag
        // and we are done.
        //

        KdpBreakpointTable[Index].Flags &= ~KD_BREAKPOINT_NEEDS_WRITE;
        //DPRINT(("KD: Breakpoint at 0x%08x never written; flag cleared.\n",
        //    KdpBreakpointTable[Index].Address));
        return TRUE;
    }

    if (KdpBreakpointTable[Index].Content == KdpBreakpointInstruction) {

        //
        // The instruction is a breakpoint anyway.
        //

        //DPRINT(("KD: Breakpoint at 0x%08x; instr is really BP; flag cleared.\n",
        //    KdpBreakpointTable[Index].Address));

        return TRUE;
    }

    //
    // Restore the instruction contents.
    //

    if (KdpMoveMemory( (PCHAR)KdpBreakpointTable[Index].Address,
                        (PCHAR)&KdpBreakpointTable[Index].Content,
                        sizeof(KDP_BREAKPOINT_TYPE) ) != sizeof(KDP_BREAKPOINT_TYPE)) {

        KdpOweBreakpoint = TRUE;
        KdpBreakpointTable[Index].Flags |= KD_BREAKPOINT_NEEDS_REPLACE;
        //DPRINT(("KD: Breakpoint at 0x%08x; unable to clear, flag set.\n",
            //KdpBreakpointTable[Index].Address));
        return FALSE;
    } else {
        //DPRINT(("KD: Breakpoint at 0x%08x cleared.\n",
            //KdpBreakpointTable[Index].Address));
        return TRUE;
    }
}

BOOLEAN
KdpDeleteBreakpoint (
    IN ULONG Handle
    )

/*++

Routine Description:

    This routine deletes an entry from the breakpoint table.

Arguments:

    Handle - Supplies the index plus one of the breakpoint table entry
        which is to be deleted.

Return Value:

    A value of FALSE is returned if the specified handle is not a valid
    value or the breakpoint cannot be deleted because the old instruction
    cannot be replaced. Otherwise, a value of TRUE is returned.

--*/

{
    ULONG Index = Handle - 1;

    //
    // If the specified handle is not valid, then return FALSE.
    //

    if ((Handle == 0) || (Handle > BREAKPOINT_TABLE_SIZE)) {
        DPRINT(("KD: Breakpoint %d invalid.\n", Index));
        return FALSE;
    }

    //
    // If the specified breakpoint table entry is not valid, then return FALSE.
    //

    if (KdpBreakpointTable[Index].Flags == 0) {
        //DPRINT(("KD: Breakpoint %d already clear.\n", Index));
        return FALSE;
    }

    //
    // If the breakpoint is already suspended, just delete it from the table.
    //

    if (KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_SUSPENDED) {
        //DPRINT(("KD: Deleting suspended breakpoint %d \n", Index));
        if ( !(KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_NEEDS_REPLACE) ) {
            //DPRINT(("KD: already clear.\n"));
            KdpBreakpointTable[Index].Flags = 0;
            return TRUE;
        }
    }

    //
    // Replace the instruction contents.
    //

    if (KdpLowWriteContent(Index)) {

        //
        // Delete breakpoint table entry
        //

        //DPRINT(("KD: Breakpoint %d deleted successfully.\n", Index));
        KdpBreakpointTable[Index].Flags = 0;
    }

    return TRUE;
}

BOOLEAN
KdpDeleteBreakpointRange (
    IN PVOID Lower,
    IN PVOID Upper
    )

/*++

Routine Description:

    This routine deletes all breakpoints falling in a given range
    from the breakpoint table.

Arguments:

    Lower - inclusive lower address of range from which to remove BPs.

    Upper - include upper address of range from which to remove BPs.

Return Value:

    TRUE if any breakpoints removed, FALSE otherwise.

--*/

{
    ULONG   Index;
    BOOLEAN ReturnStatus = FALSE;

    //
    // Examine each entry in the table in turn
    //

    for (Index = 0; Index < BREAKPOINT_TABLE_SIZE; Index++)
    {
        if ( (KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_IN_USE) &&
             ((KdpBreakpointTable[Index].Address >= Lower) &&
              (KdpBreakpointTable[Index].Address <= Upper)) )
        {

            //
            // Breakpoint is in use and falls in range, clear it.
            //

            if (KdpDeleteBreakpoint(Index+1))
            {
                ReturnStatus = TRUE;
            }
        }
    }

    return ReturnStatus;

}

VOID
KdpSuspendBreakpoint (
    ULONG Handle
    )
{
    ULONG Index = Handle - 1;

    if ( (KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_IN_USE) &&
        !(KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_SUSPENDED) ) {

        KdpBreakpointTable[Index].Flags |= KD_BREAKPOINT_SUSPENDED;
        KdpLowWriteContent(Index);
    }

    return;

} // KdpSuspendBreakpoint

VOID
KdpSuspendAllBreakpoints (
    VOID
    )
{
    ULONG Handle;

    BreakpointsSuspended = TRUE;

    for ( Handle = 1; Handle <= BREAKPOINT_TABLE_SIZE; Handle++ ) {
        KdpSuspendBreakpoint(Handle);
    }

    return;

} // KdpSuspendAllBreakpoints

BOOLEAN
KdpLowRestoreBreakpoint (
    IN ULONG Index
    )

/*++

Routine Description:

    This routine attempts to write a breakpoint instruction.
    The old contents must have already been stored in the
    breakpoint record.

Arguments:

    Index - Supplies the index of the breakpoint table entry
        which is to be written.

Return Value:

    Returns TRUE if the breakpoint was written, FALSE if it was
    not and has been marked for writing later.

--*/

{
    KDP_BREAKPOINT_TYPE Content;

    //
    // Does the breakpoint need to be written at all?
    //

    if (KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_NEEDS_REPLACE) {

        //
        // The breakpoint was never removed.  Clear the flag
        // and we are done.
        //

        KdpBreakpointTable[Index].Flags &= ~KD_BREAKPOINT_NEEDS_REPLACE;
        return TRUE;
    }

    if (KdpBreakpointTable[Index].Content == KdpBreakpointInstruction) {

        //
        // The instruction is a breakpoint anyway.
        //

        return TRUE;
    }

    //
    // Replace the instruction contents.
    //

    if (KdpBreakpointTable[Index].Content == KdpBreakpointInstruction) {

        //
        // The instruction is a breakpoint anyway.
        //

        return TRUE;
    }

    //
    // Replace the instruction contents.
    //

    if (KdpMoveMemory( (PCHAR)KdpBreakpointTable[Index].Address,
                       (PCHAR)&KdpBreakpointInstruction,
                       sizeof(KDP_BREAKPOINT_TYPE) ) != sizeof(KDP_BREAKPOINT_TYPE)) {

        KdpBreakpointTable[Index].Flags |= KD_BREAKPOINT_NEEDS_WRITE;
        KdpOweBreakpoint = TRUE;
        return FALSE;

    } else {

        KdpBreakpointTable[Index].Flags &= ~KD_BREAKPOINT_NEEDS_WRITE;
        return TRUE;
    }
}

VOID
KdpRestoreAllBreakpoints (
    VOID
    )
{
    ULONG Index;

    BreakpointsSuspended = FALSE;

    for ( Index = 0; Index < BREAKPOINT_TABLE_SIZE; Index++ ) {

        if ((KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_IN_USE) &&
            (KdpBreakpointTable[Index].Flags & KD_BREAKPOINT_SUSPENDED) ) {

            KdpBreakpointTable[Index].Flags &= ~KD_BREAKPOINT_SUSPENDED;
            KdpLowRestoreBreakpoint(Index);
        }
    }

    return;

} // KdpRestoreAllBreakpoints

VOID
KdDeleteAllBreakpoints(
    VOID
    )
{
    ULONG Handle;

    if (KdDebuggerEnabled == FALSE) {
        return;
    }

    BreakpointsSuspended = FALSE;

    for ( Handle = 1; Handle <= BREAKPOINT_TABLE_SIZE; Handle++ ) {
        KdpDeleteBreakpoint(Handle);
    }

    return;
} // KdDeleteAllBreakpoints
