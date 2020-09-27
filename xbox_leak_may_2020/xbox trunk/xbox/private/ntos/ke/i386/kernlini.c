/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    kernlini.c

Abstract:

    This module contains the code to initialize the kernel data structures
    and to initialize the idle thread, its process, and the processor control
    block.

--*/

#include "ki.h"

VOID
KiSwapGDT (
    VOID
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT,KiSwapGDT)
#endif

#if DBG
const UCHAR MsgDpcTrashedEsp[] = "\n*** DPC routine %p trashed ESP\n";
const UCHAR MsgDpcFpuEnabled[] = "\n*** DPC routine %p returned with the FPU enabled\n";
const UCHAR MsgDpcTimeout[]    = "\n*** DPC routine > 1 sec --- This is not a break in KeUpdateSystemTime\n";
const UCHAR MsgISRTimeout[]    = "\n*** ISR at %lx took over .5 second\n";
const UCHAR MsgISROverflow[]   = "\n*** ISR at %lx - %d interrupts per .5 second\n";

ULONG   KiDPCTimeout       = 110;
ULONG   KiISRTimeout       = 55;
ULONG   KiISROverflow      = 5500;
#endif

extern  KPCR KiPCR;
extern  KTHREAD KiIdleThread;
extern  UCHAR KiIdleThreadStackLimit[];
extern  KGDTENTRY KiGDTEnd[];
extern  KIDTENTRY KiIDTEnd[];

VOID
KiPreInitializeKernel(
    VOID
    )
/*++

Routine Description:

    This function gains control before the kernel debugger has been initialized
    in order to partially initialize static kernel data structures.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PKPCR Pcr;

    Pcr = &KiPCR;

    //
    // Initialize the PCR.  The stack base and limit are initialized in case we
    // end up attempting to flush the NPX state before the KiInitializeKernel
    // has completed initialization.
    //

    Pcr->SelfPcr = Pcr;
    Pcr->Prcb = &Pcr->PrcbData;

    Pcr->NtTib.ExceptionList = EXCEPTION_CHAIN_END;
    Pcr->NtTib.StackLimit = KiIdleThreadStackLimit;
    Pcr->NtTib.StackBase = KiIdleThreadStackLimit + KERNEL_STACK_SIZE - sizeof(FX_SAVE_AREA);

    Pcr->PrcbData.CurrentThread = &KiIdleThread;

    //
    // Mark the idle thread as not having a NPX state as well to further avoid
    // attempting to flush the NPX state.
    //

    KiIdleThread.NpxState = NPX_STATE_NOT_LOADED;
    KiIdleThread.StackLimit = KiIdleThreadStackLimit;
    KiIdleThread.StackBase = KiIdleThreadStackLimit + KERNEL_STACK_SIZE;
}

VOID
KiInitializeKernel(
    VOID
    )
/*++

Routine Description:

    This function gains control after the system has been bootstrapped and
    before the system has been initialized. Its function is to initialize
    the kernel data structures, initialize the idle thread and process objects,
    initialize the processor control block, call the executive initialization
    routine, and then return to the system startup routine. This routine is
    also called to initialize the processor specific structures when a new
    processor is brought on line.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PKPCR Pcr;
    PKPRCB Prcb;
    PFX_SAVE_AREA NpxFrame;
    KIRQL OldIrql;
    PVOID DpcStack;

    Pcr = KeGetPcr();
    Prcb = KeGetCurrentPrcb();

    //
    // Assert that NpxSaveArea is 16 byte aligned; KeSaveFloatingPointState will
    // use the fxsave instruction with this buffer and the Intel architecture
    // requires that the pointer be 16 byte aligned.
    //

    ASSERT((((ULONG_PTR)&Prcb->NpxSaveArea) % 16) == 0);

    //
    // Initialize DPC listhead and lock.
    //

    InitializeListHead(&Prcb->DpcListHead);
    Prcb->DpcRoutineActive = 0;

    //
    // Lower IRQL to APC level.
    //

    KeLowerIrql(APC_LEVEL);

    //
    // Platform architecture independent initialization.
    //

    KiInitSystem();

    //
    // Initialize idle process object.
    //

    KeInitializeProcess(&KiIdleProcess, (KPRIORITY)0);

    KiIdleProcess.ThreadQuantum = MAXCHAR;

    //
    // Initialize system process object.
    //

    KeInitializeProcess(&KiSystemProcess, NORMAL_BASE_PRIORITY);

    KiSystemProcess.ThreadQuantum = THREAD_QUANTUM;

    //
    // Initialize idle thread object and then set:
    //
    //      1. the initial kernel stack to the specified idle stack.
    //      2. the next processor number to the specified processor.
    //      3. the thread priority to the highest possible value.
    //      4. the state of the thread to running.
    //      5. the thread affinity to the specified processor.
    //      6. the specified processor member in the process active processors
    //          set.
    //

    KeInitializeThread(&KiIdleThread, KiIdleThreadStackLimit + KERNEL_STACK_SIZE,
                       KERNEL_STACK_SIZE, 0, (PKSYSTEM_ROUTINE)NULL,
                       (PKSTART_ROUTINE)NULL, (PVOID)NULL, &KiIdleProcess);
    KiIdleThread.Priority = HIGH_PRIORITY;
    KiIdleThread.State = Running;
    KiIdleThread.WaitIrql = DISPATCH_LEVEL;

    //
    // Initialize the idle thread's NPX state to the same state used for normal
    // threads.
    //

    NpxFrame = (PFX_SAVE_AREA)(((ULONG)(KiIdleThread.StackBase) -
                sizeof(FX_SAVE_AREA)));

    RtlZeroMemory((PVOID)NpxFrame, sizeof(FX_SAVE_AREA));

    NpxFrame->FloatSave.ControlWord = 0x27f;     //like fpinit but 64bit mode
    NpxFrame->FloatSave.MXCsr       = 0x1f80;    // mask all the exceptions

    //
    // Initialize the processor block. (Note that some fields have been
    // initialized at KiPreInitializeKernel).
    //

    Prcb->CurrentThread = &KiIdleThread;
    Prcb->NextThread = (PKTHREAD)NULL;
    Prcb->IdleThread = &KiIdleThread;
    Pcr->NtTib.StackBase = (PCHAR)KiIdleThread.StackBase - sizeof(FX_SAVE_AREA);

    //
    // call the executive initialization routine.
    //

    try {
        ExpInitializeExecutive();
    } except (EXCEPTION_EXECUTE_HANDLER) {
        KeBugCheck (PHASE0_EXCEPTION);
    }

    //
    // Processor 0's DPC stack was temporarily allocated on
    // the Double Fault Stack, switch to a proper kernel
    // stack now.
    //

    DpcStack = MmCreateKernelStack(KERNEL_STACK_SIZE, FALSE);

    if (DpcStack == NULL) {
        KeBugCheckEx(NO_PAGES_AVAILABLE, 1, 0, 0, 0);
    }
    Prcb->DpcStack = DpcStack;

    //
    // Set the priority of the specified idle thread to zero, set appropriate
    // member in KiIdleSummary and return to the system start up routine.
    //

    KeRaiseIrql(DISPATCH_LEVEL, &OldIrql);
    KeSetPriorityThread(&KiIdleThread, (KPRIORITY)0);

    //
    // if a thread has not been selected to run on the current processors,
    // check to see if there are any ready threads; otherwise add this
    // processors to the IdleSummary
    //

    if (Prcb->NextThread == (PKTHREAD)NULL) {
        SetMember(0, KiIdleSummary);
    }

    KeRaiseIrql(HIGH_LEVEL, &OldIrql);
}

VOID
KiSwapGDT(
    VOID
    )
/*++

Routine Description:

    This function is called to edit the GDT.  It swaps words of the address
    and access fields around into the format the part actually needs.
    This allows for easy static init of the GDT.

    Note that this procedure edits the current GDT.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PKGDTENTRY Entry;
    UCHAR Temp;

    //
    // Rearrange the entries of GDT to match i386 structure
    //

    for (Entry = KiGDT; Entry < KiGDTEnd; Entry++) {
        Temp = Entry->HighWord.Bytes.Flags1;
        Entry->HighWord.Bytes.Flags1 = Entry->HighWord.Bytes.Flags2;
        Entry->HighWord.Bytes.Flags2 = Entry->HighWord.Bytes.BaseHi;
        Entry->HighWord.Bytes.BaseHi = Temp;
    }
}

VOID
KiSwapIDT(
    VOID
    )
/*++

Routine Description:

    This function is called to edit the IDT.  It swaps words of the address
    and access fields around into the format the part actually needs.
    This allows for easy static init of the IDT.

    Note that this procedure edits the current IDT.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PKIDTENTRY Entry;
    USHORT Temp;

    //
    // Rearrange the entries of IDT to match i386 interrupt gate structure
    //

    for (Entry = KiIDT; Entry < KiIDTEnd; Entry++) {
        Temp = Entry->Selector;
        Entry->Selector = Entry->ExtendedOffset;
        Entry->ExtendedOffset = Temp;
    }
}
