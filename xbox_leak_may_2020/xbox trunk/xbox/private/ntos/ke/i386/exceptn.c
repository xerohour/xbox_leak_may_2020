/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    exceptn.c

Abstract:

    This module implement the code necessary to dispatch expections to the
    proper mode and invoke the exception dispatcher.

--*/

#include "ki.h"

#define FN_BITS_PER_TAGWORD     16
#define FN_TAG_EMPTY            0x3
#define FN_TAG_MASK             0x3
#define FX_TAG_VALID            0x1
#define NUMBER_OF_FP_REGISTERS  8
#define BYTES_PER_FP_REGISTER   10
#define BYTES_PER_FX_REGISTER   16

ULONG
KiEspFromTrapFrame(
    IN PKTRAP_FRAME TrapFrame
    )

/*++

Routine Description:

    This routine fetches the correct esp from a trapframe, accounting
    for whether the frame is a user or kernel mode frame, and whether
    it has been edited.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame from which volatile context
        should be copied into the context record.

Return Value:

    Value of Esp.

--*/

{
    if ((TrapFrame->SegCs & FRAME_EDITED) == 0) {

        //  Kernel mode frame which has had esp edited,
        //  value of Esp is in TempEsp.

        return TrapFrame->TempEsp;

    } else {

        //  Kernel mode frame has has not had esp edited, compute esp.

        return (ULONG)&TrapFrame->HardwareEsp;
    }
}

VOID
KiEspToTrapFrame(
    IN PKTRAP_FRAME TrapFrame,
    IN ULONG Esp
    )

/*++

Routine Description:

    This routine sets the specified value Esp into the trap frame,
    accounting for whether the frame is a user or kernel mode frame,
    and whether it has been edited before.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame from which volatile context
        should be copied into the context record.

    Esp - New value for Esp.

Return Value:

    None.

--*/
{
    ULONG   OldEsp;

    OldEsp = KiEspFromTrapFrame(TrapFrame);

    //
    //  Kernel mode esp can't be lowered or iret emulation will fail
    //

    if (Esp < OldEsp)
        KeBugCheck(SET_OF_INVALID_CONTEXT);

    //
    //  Edit frame, setting edit marker as needed.
    //

    if ((TrapFrame->SegCs & FRAME_EDITED) == 0) {

        //  Kernel frame that has already been edited,
        //  store value in TempEsp.

        TrapFrame->TempEsp = Esp;

    } else {

        //  Kernel frame for which Esp is being edited first time.
        //  Save real SegCs, set marked in SegCs, save Esp value.

        if (OldEsp != Esp) {
            TrapFrame->TempSegCs = TrapFrame->SegCs;
            TrapFrame->SegCs = TrapFrame->SegCs & ~FRAME_EDITED;
            TrapFrame->TempEsp = Esp;
        }
    }
}

VOID
KeContextFromKframes (
    IN PKTRAP_FRAME TrapFrame,
    IN OUT PCONTEXT ContextFrame
    )

/*++

Routine Description:

    This routine moves the selected contents of the specified trap and exception frames
    frames into the specified context frame according to the specified context
    flags.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame from which volatile context
        should be copied into the context record.

    ContextFrame - Supplies a pointer to the context frame that receives the
        context copied from the trap and exception frames.

Return Value:

    None.

--*/

{
    PFX_SAVE_AREA NpxFrame;

    //
    // Set control information if specified.
    //

    if ((ContextFrame->ContextFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {

        //
        // Set registers ebp, eip, cs, eflag, esp and ss.
        //

        ContextFrame->Ebp = TrapFrame->Ebp;
        ContextFrame->Eip = TrapFrame->Eip;

        if (((TrapFrame->SegCs & FRAME_EDITED) == 0) &&
            ((TrapFrame->EFlags & EFLAGS_V86_MASK) == 0)) {
            ContextFrame->SegCs = TrapFrame->TempSegCs & SEGMENT_MASK;
        } else {
            ContextFrame->SegCs = TrapFrame->SegCs & SEGMENT_MASK;
        }
        ContextFrame->EFlags = TrapFrame->EFlags;
        ContextFrame->SegSs = KGDT_R0_DATA;
        ContextFrame->Esp = KiEspFromTrapFrame(TrapFrame);
    }

    //
    // Set integer register contents if specified.
    //

    if ((ContextFrame->ContextFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {

        //
        // Set integer registers edi, esi, ebx, edx, ecx, eax
        //

        ContextFrame->Edi = TrapFrame->Edi;
        ContextFrame->Esi = TrapFrame->Esi;
        ContextFrame->Ebx = TrapFrame->Ebx;
        ContextFrame->Ecx = TrapFrame->Ecx;
        ContextFrame->Edx = TrapFrame->Edx;
        ContextFrame->Eax = TrapFrame->Eax;
    }

    //
    // Set extended register contents if specified.
    //

    if (((ContextFrame->ContextFlags & CONTEXT_FLOATING_POINT) == CONTEXT_FLOATING_POINT) ||
        ((ContextFrame->ContextFlags & CONTEXT_EXTENDED_REGISTERS) == CONTEXT_EXTENDED_REGISTERS)) {

        KiFlushNPXState();

        NpxFrame = (PFX_SAVE_AREA)KeGetPcr()->NtTib.StackBase;

        RtlCopyMemory( &ContextFrame->FloatSave,
                       &NpxFrame->FloatSave,
                       sizeof(FLOATING_SAVE_AREA) );
    }
}

VOID
KeContextToKframes (
    IN OUT PKTRAP_FRAME TrapFrame,
    IN PCONTEXT ContextFrame,
    IN ULONG ContextFlags
    )

/*++

Routine Description:

    This routine moves the selected contents of the specified context frame into
    the specified trap and exception frames according to the specified context
    flags.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame that receives the volatile
        context from the context record.

    ContextFrame - Supplies a pointer to a context frame that contains the
        context that is to be copied into the trap and exception frames.

    ContextFlags - Supplies the set of flags that specify which parts of the
        context frame are to be copied into the trap and exception frames.

Return Value:

    None.

--*/

{
    PFX_SAVE_AREA NpxFrame;

    //
    // Set control information if specified.
    //

    if ((ContextFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {

        //
        // Set registers eflag, ebp, eip, cs, esp and ss.
        // Eflags is set first, so that the auxilliary routines
        // can check the v86 bit to determine as well as cs, to
        // determine if the frame is kernel or user mode. (v86 mode cs
        // can have any value)
        //

        TrapFrame->EFlags = SANITIZE_FLAGS(ContextFrame->EFlags);
        TrapFrame->Ebp = ContextFrame->Ebp;
        TrapFrame->Eip = ContextFrame->Eip;
        KiEspToTrapFrame(TrapFrame, ContextFrame->Esp);
    }

    //
    // Set integer registers contents if specified.
    //

    if ((ContextFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {

        //
        // Set integer registers edi, esi, ebx, edx, ecx, eax.
        //
        //  Can NOT call RtlMoveMemory here because the regs aren't
        //  contiguous in pusha frame, and we don't want to export
        //  bits of junk into context record.
        //

        TrapFrame->Edi = ContextFrame->Edi;
        TrapFrame->Esi = ContextFrame->Esi;
        TrapFrame->Ebx = ContextFrame->Ebx;
        TrapFrame->Ecx = ContextFrame->Ecx;
        TrapFrame->Edx = ContextFrame->Edx;
        TrapFrame->Eax = ContextFrame->Eax;
    }

    //
    // Set extended register contents if requested.
    //

    if (((ContextFrame->ContextFlags & CONTEXT_FLOATING_POINT) == CONTEXT_FLOATING_POINT) ||
        ((ContextFrame->ContextFlags & CONTEXT_EXTENDED_REGISTERS) == CONTEXT_EXTENDED_REGISTERS)) {

        KiFlushNPXState();

        NpxFrame = (PFX_SAVE_AREA)KeGetPcr()->NtTib.StackBase;

        RtlCopyMemory( &NpxFrame->FloatSave,
                       &ContextFrame->FloatSave,
                       sizeof(FLOATING_SAVE_AREA) );

        //
        // Make sure only valid floating state bits are moved to Cr0NpxState.
        //

        NpxFrame->FloatSave.Cr0NpxState &= ~(CR0_EM | CR0_MP | CR0_TS);

        //
        // Make sure all reserved bits are clear in MXCSR so we don't get a GP
        // fault when doing an FRSTOR on this state.
        //

        NpxFrame->FloatSave.MXCsr = SANITIZE_MXCSR(NpxFrame->FloatSave.MXCsr);
    }
}

VOID
KiDispatchException (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PKTRAP_FRAME TrapFrame,
    IN BOOLEAN FirstChance
    )

/*++

Routine Description:

    This function is called to dispatch an exception to the proper mode and
    to cause the exception dispatcher to be called. If the previous mode is
    kernel, then the exception dispatcher is called directly to process the
    exception. Otherwise the exception record, exception frame, and trap
    frame contents are copied to the user mode stack. The contents of the
    exception frame and trap are then modified such that when control is
    returned, execution will commense in user mode in a routine which will
    call the exception dispatcher.

Arguments:

    ExceptionRecord - Supplies a pointer to an exception record.

    ExceptionFrame - Supplies a pointer to an exception frame. For NT386,
        this should be NULL.

    TrapFrame - Supplies a pointer to a trap frame.

    FirstChance - Supplies a boolean value that specifies whether this is
        the first (TRUE) or second (FALSE) chance for the exception.

Return Value:

    None.

--*/

{
    CONTEXT ContextFrame;

    //
    // Move machine state from trap and exception frames to a context frame,
    // and increment the number of exceptions dispatched.
    //

    ContextFrame.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER |
        CONTEXT_FLOATING_POINT | CONTEXT_EXTENDED_REGISTERS;

    KeContextFromKframes(TrapFrame, &ContextFrame);

    //
    // if it is BREAK_POINT exception, we subtract 1 from EIP and report
    // the updated EIP to user.  This is because Cruiser requires EIP
    // points to the int 3 instruction (not the instruction following int 3).
    // In this case, BreakPoint exception is fatal. Otherwise we will step
    // on the int 3 over and over again, if user does not handle it
    //
    // if the BREAK_POINT occured in V86 mode, the debugger running in the
    // VDM will expect CS:EIP to point after the exception (the way the
    // processor left it.  this is also true for protected mode dos
    // app debuggers.  We will need a way to detect this.
    //
    //

    switch (ExceptionRecord->ExceptionCode) {
        case STATUS_BREAKPOINT:
            ContextFrame.Eip--;
            break;
    }

    //
    // Previous mode was kernel.
    //
    // If the kernel debugger is active, then give the kernel debugger the
    // first chance to handle the exception. If the kernel debugger handles
    // the exception, then continue execution. Else attempt to dispatch the
    // exception to a frame based handler. If a frame based handler handles
    // the exception, then continue execution.
    //
    // If a frame based handler does not handle the exception,
    // give the kernel debugger a second chance, if it's present.
    //
    // If the exception is still unhandled, call KeBugCheck().
    //

    if (FirstChance == TRUE) {

        if ((KiDebugRoutine != NULL) &&
           (((KiDebugRoutine) (TrapFrame,
                               ExceptionFrame,
                               ExceptionRecord,
                               &ContextFrame,
                               FALSE)) != FALSE)) {

            goto Handled1;
        }

        // Kernel debugger didn't handle exception.

        if (RtlDispatchException(ExceptionRecord, &ContextFrame) == TRUE) {
            goto Handled1;
        }
    }

    //
    // This is the second chance to handle the exception.
    //

    if ((KiDebugRoutine != NULL) &&
        (((KiDebugRoutine) (TrapFrame,
                            ExceptionFrame,
                            ExceptionRecord,
                            &ContextFrame,
                            TRUE)) != FALSE)) {

        goto Handled1;
    }

    KeBugCheckEx(
        KMODE_EXCEPTION_NOT_HANDLED,
        ExceptionRecord->ExceptionCode,
        (ULONG)ExceptionRecord->ExceptionAddress,
        ExceptionRecord->ExceptionInformation[0],
        ExceptionRecord->ExceptionInformation[1]
        );

    //
    // Move machine state from context frame to trap and exception frames and
    // then return to continue execution with the restored state.
    //

Handled1:
    KeContextToKframes(TrapFrame, &ContextFrame, ContextFrame.ContextFlags);
}
