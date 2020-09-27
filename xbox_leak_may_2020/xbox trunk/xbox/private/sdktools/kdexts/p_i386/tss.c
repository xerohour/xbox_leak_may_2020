/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    tss.c

Abstract:

    WinDbg Extension Api

Author:

    Ramon J San Andres (ramonsa) 5-Nov-1993

    Peter L Johnston   (peterj) 28-Jan-1999
        Pulled TaskGate2TrapFrame inline and made it dump the rest of
        the TSS.

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#pragma hdrstop


ULONG_PTR
Selector2Address(
    USHORT      Processor,
    USHORT      TaskRegister
    )
{
    DESCRIPTOR_TABLE_ENTRY desc;

    //
    // Lookup task register
    //

    desc.Selector = TaskRegister;
    if (!NT_SUCCESS(LookupSelector(Processor, &desc))) {

        //
        // Can't do it.
        //

        return 0;
    }

    if (desc.Descriptor.HighWord.Bits.Type != 9  &&
        desc.Descriptor.HighWord.Bits.Type != 0xb) {

        //
        // not a 32bit task descriptor
        //

        return 0;
    }

    //
    // Read in Task State Segment
    //

    return ((ULONG)desc.Descriptor.BaseLow +
           ((ULONG)desc.Descriptor.HighWord.Bytes.BaseMid << 16) +
           ((ULONG)desc.Descriptor.HighWord.Bytes.BaseHi  << 24) );

}

DECLARE_API( tss )

/*++

Routine Description:



Arguments:

    args -

Return Value:

    None

--*/

{

#define MAX_RING 3

    ULONG       taskRegister;
    KTRAP_FRAME trapFrame;
    PUCHAR      buf;
    ULONG_PTR   hostAddress;
    BOOLEAN     extendedDump;
    ULONG       i;
    char ExprBuf[256] ;

    struct  {       // intel's TSS format
        ULONG   Previous;
        struct {
            ULONG   Esp;
            ULONG   Ss;
        } Ring[MAX_RING];
        ULONG   Cr3;
        ULONG   Eip;
        ULONG   EFlags;
        ULONG   Eax;
        ULONG   Ecx;
        ULONG   Edx;
        ULONG   Ebx;
        ULONG   Esp;
        ULONG   Ebp;
        ULONG   Esi;
        ULONG   Edi;
        ULONG   Es;
        ULONG   Cs;
        ULONG   Ss;
        ULONG   Ds;
        ULONG   Fs;
        ULONG   Gs;
        ULONG   Ldt;
        USHORT  T;
        USHORT  IoMapBase;
    } TSS;

    buf = (PUCHAR)&TSS;
    *buf = '\0';

    sscanf(args,"%s %s", ExprBuf, buf);

    if (ExprBuf[0]) {
        if (IsHexNumber(ExprBuf)) {
            sscanf(ExprBuf, "%lx", &taskRegister) ;
        } else {
            taskRegister = GetExpression(ExprBuf);
            if (taskRegister==0) {
                dprintf("An error occured trying to evaluate the expression\n") ;
                return;
            }
        }
    }

    //
    // If user specified a 2nd parameter, doesn't matter what it is,
    // dump the portions of the TSS not covered by the trap frame dump.
    //

    extendedDump = *buf != '\0';

    if (HIWORD(taskRegister) == 0) {

        hostAddress = Selector2Address((USHORT)dwProcessor, (USHORT)taskRegister);

        if (!hostAddress) {
            dprintf("unable to get Task State Segment address from selector %lX\n",
                    taskRegister);
            return;
        }

    } else {
        hostAddress = taskRegister;
    }

    if (!xReadMemory((PVOID)hostAddress,
                     &TSS,
                     sizeof(TSS))) {
        dprintf("unable to read Task State Segment from host address %p\n",
                hostAddress);
        return;
    }

    //
    // Display it.
    //

    if (extendedDump) {
        dprintf("\nTask State Segment at 0x%p\n\n", hostAddress);
        dprintf("Previous Task Link   = %4x\n", TSS.Previous);
        for (i = 0 ; i < MAX_RING ; i++) {
            dprintf("Esp%d = %8x  SS%d = %4x\n",
                    i, TSS.Ring[i].Esp,
                    i, TSS.Ring[i].Ss);
        }
        dprintf("CR3 (PDBR)           = %08x\n", TSS.Cr3);
        dprintf("I/O Map Base Address = %4x, Debug Trap (T) = %s\n",
                TSS.IoMapBase,
                TSS.T == 0 ? "False" : "True");
        dprintf("\nSaved General Purpose Registers\n\n");
    }

    //
    // Move fields from Task State Segment to TrapFrame and display
    // the registers in the familiar format.
    //

    trapFrame.Eip    = TSS.Eip;
    trapFrame.EFlags = TSS.EFlags;
    trapFrame.Eax    = TSS.Eax;
    trapFrame.Ecx    = TSS.Ecx;
    trapFrame.Edx    = TSS.Edx;
    trapFrame.Ebx    = TSS.Ebx;
    trapFrame.Ebp    = TSS.Ebp;
    trapFrame.Esi    = TSS.Esi;
    trapFrame.Edi    = TSS.Edi;
    trapFrame.SegCs  = TSS.Cs;
    trapFrame.HardwareEsp = TSS.Esp;
    trapFrame.HardwareSegSs = TSS.Ss;

    DisplayTrapFrame (&trapFrame, 0);
}
