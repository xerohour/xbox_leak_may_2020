/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    misc.c

Abstract:

    This module implements machine dependent miscellaneous kernel functions.

Author:

    Ken Reneris     7-5-95

Environment:

    Kernel mode only.

Revision History:

--*/

#include "ki.h"

//
// Internal format of the floating_save structure which is passed
//
typedef struct _CONTROL_WORD {
    USHORT      ControlWord;
    ULONG       MXCsr;
} CONTROL_WORD, *PCONTROL_WORD;

typedef struct {
    UCHAR       Flags;
    KIRQL       Irql;
    KIRQL       PreviousNpxIrql;
    UCHAR       Spare[2];

    union {
        CONTROL_WORD    Fcw;
        PFX_SAVE_AREA   Context;
    } u;
    ULONG       Cr0NpxState;

    PKTHREAD    Thread;         // debug

} FLOAT_SAVE, *PFLOAT_SAVE;


#define FLOAT_SAVE_COMPLETE_CONTEXT     0x01
#define FLOAT_SAVE_FREE_CONTEXT_HEAP    0x02
#define FLOAT_SAVE_VALID                0x04
#define FLOAT_SAVE_RESERVED             0xF8

#pragma warning(disable:4035)               // re-enable below

// notes:
// Kix86FxSave(NpxFame) - performs an FxSave to the address specificied
//                   - no other action occurs
__inline
KIRQL
Kix86FxSave(
    PULONG NpxFrame
    )
{
    _asm {
        mov eax, NpxFrame
        ;fxsave [eax]
        _emit  0fh
        _emit  0aeh
        _emit   0
    }
}

//
// Load Katmai New Instruction Technology Control/Status
//
__inline
KIRQL
Kix86LdMXCsr(
    PULONG MXCsr
    )
{
    _asm {
        mov eax, MXCsr
        ;LDMXCSR [eax]
        _emit  0fh
        _emit  0aeh
        _emit  10h
    }
}

//
// Store Katmai New Instruction Technology Control/Status
//
__inline
KIRQL
Kix86StMXCsr(
    PULONG MXCsr
    )
{
    _asm {
        mov eax, MXCsr
        ;STMXCSR [eax]
        _emit  0fh
        _emit  0aeh
        _emit  18h
    }
}
#pragma warning(default:4035)


NTSTATUS
KeSaveFloatingPointState (
    OUT PKFLOATING_SAVE     PublicFloatSave
    )
/*++

Routine Description:

    This routine saves the thread's current non-volatile NPX state,
    and sets a new initial floating point state for the caller.

Arguments:

    FloatSave - receives the current non-volatile npx state for the thread

Return Value:

--*/
{
    PKTHREAD Thread;
    PFX_SAVE_AREA NpxFrame;
    KIRQL                   Irql;
    USHORT                  ControlWord;
    ULONG                   MXCsr;
    PKPRCB                  Prcb;
    PFLOAT_SAVE             FloatSave;

    //
    // Get the current irql and thread
    //

    FloatSave = (PFLOAT_SAVE) PublicFloatSave;

    Irql = KeGetCurrentIrql();
    Thread = KeGetCurrentThread();

    ASSERT (Thread->NpxIrql <= Irql);

    FloatSave->Flags           = 0;
    FloatSave->Irql            = Irql;
    FloatSave->PreviousNpxIrql = Thread->NpxIrql;
    FloatSave->Thread          = Thread;

    //
    // If the irql has changed we need to save the complete floating
    // state context as the prior level has been interrupted.
    //

    if (Thread->NpxIrql != Irql) {

        //
        // If this is apc level we don't have anyplace to hold this
        // context, allocate some heap.
        //

        if (Irql == APC_LEVEL) {
            FloatSave->u.Context = ExAllocatePoolWithTag (
                                        sizeof (FX_SAVE_AREA),
                                        ' XPN'
                                        );

            if (!FloatSave->u.Context) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            FloatSave->Flags |= FLOAT_SAVE_FREE_CONTEXT_HEAP;

        } else {

            ASSERT (Irql == DISPATCH_LEVEL);
            FloatSave->u.Context = &KeGetCurrentPrcb()->NpxSaveArea;

        }

        FloatSave->Flags |= FLOAT_SAVE_COMPLETE_CONTEXT;
    }

    //
    // Stop context switching and allow access to the local fp unit
    //

    _asm {
        cli
        mov     eax, cr0
        mov     ecx, eax
        and     eax, not (CR0_MP|CR0_EM|CR0_TS)
        cmp     eax, ecx
        je      short sav10

        mov     cr0, eax
sav10:
    }

    Prcb = KeGetCurrentPrcb();

    //
    // Get ownership of npx register set for this context
    //

    if (Prcb->NpxThread != Thread) {

        //
        // If the other context is loaded in the npx registers, flush
        // it to that threads save area
        //
        if (Prcb->NpxThread) {

            NpxFrame = (PFX_SAVE_AREA)(((ULONG)(Prcb->NpxThread->StackBase) -
                        sizeof(FX_SAVE_AREA)));

            Kix86FxSave((PULONG)NpxFrame);

            Prcb->NpxThread->NpxState = NPX_STATE_NOT_LOADED;
        }

        Prcb->NpxThread = Thread;
    }

    NpxFrame = (PFX_SAVE_AREA)(((ULONG)(Thread->StackBase) -
                sizeof(FX_SAVE_AREA)));


    //
    // Save the previous state as required
    //

    if (FloatSave->Flags & FLOAT_SAVE_COMPLETE_CONTEXT) {

        //
        // Need to save the entire context
        //

        if (Thread->NpxState == NPX_STATE_LOADED) {

            Kix86FxSave ((PULONG)(FloatSave->u.Context));

            FloatSave->u.Context->FloatSave.Cr0NpxState = NpxFrame->FloatSave.Cr0NpxState;

        } else {

            RtlCopyMemory (FloatSave->u.Context, NpxFrame, sizeof(FX_SAVE_AREA));
        }

    } else {

        //
        // Save only the non-volatile state
        //

        if (Thread->NpxState == NPX_STATE_LOADED) {

            _asm {
                mov     eax, FloatSave
                fnstcw  [eax] FLOAT_SAVE.u.Fcw.ControlWord
            }

            Kix86StMXCsr(&FloatSave->u.Fcw.MXCsr);

        } else {
            //
            // Save the control word from the npx frame.
            //

            FloatSave->u.Fcw.ControlWord = (USHORT) NpxFrame->FloatSave.ControlWord;
            FloatSave->u.Fcw.MXCsr = NpxFrame->FloatSave.MXCsr;
        }

        //
        // Save Cr0NpxState, but clear CR0_TS as there's not non-volatile
        // pending fp exceptions
        //

        FloatSave->Cr0NpxState = NpxFrame->FloatSave.Cr0NpxState & ~CR0_TS;
    }

    //
    // The previous state is saved.  Set an initial default
    // FP state for the caller
    //

    NpxFrame->FloatSave.Cr0NpxState = 0;
    Thread->NpxState = NPX_STATE_LOADED;
    Thread->NpxIrql  = Irql;
    ControlWord = 0x27f;    // 64bit mode
    MXCsr = 0x1f80;

    _asm {
        fninit
        fldcw       ControlWord
    }

    Kix86LdMXCsr(&MXCsr);

    _asm {
        sti
    }

    FloatSave->Flags |= FLOAT_SAVE_VALID;
    return STATUS_SUCCESS;
}


NTSTATUS
KeRestoreFloatingPointState (
    IN PKFLOATING_SAVE      PublicFloatSave
    )
/*++

Routine Description:

    This routine retores the thread's current non-volatile NPX state,
    to the passed in state.

Arguments:

    FloatSave - the non-volatile npx state for the thread to restore

Return Value:

--*/
{
    PKTHREAD Thread;
    PFX_SAVE_AREA NpxFrame;
    ULONG                   Cr0State;
    PFLOAT_SAVE             FloatSave;

    FloatSave = (PFLOAT_SAVE) PublicFloatSave;
    Thread = FloatSave->Thread;
    
    NpxFrame = (PFX_SAVE_AREA)(((ULONG)(Thread->StackBase) -
                sizeof(FX_SAVE_AREA)));


    //
    // Verify float save looks like it's from the right context
    //

    if ((FloatSave->Flags & (FLOAT_SAVE_VALID | FLOAT_SAVE_RESERVED)) != FLOAT_SAVE_VALID) {

        //
        // Invalid floating point save area.
        //

        KeBugCheckEx(INVALID_FLOATING_POINT_STATE,
                     0,
                     FloatSave->Flags,
                     0,
                     0);
    }

    if (FloatSave->Irql != KeGetCurrentIrql()) {

        //
        // Invalid IRQL.   IRQL now must be the same as when the
        // context was saved.  (Why?   Because we save it in different
        // places depending on the IRQL at that time).
        //

        KeBugCheckEx(INVALID_FLOATING_POINT_STATE,
                     1,
                     FloatSave->Irql,
                     KeGetCurrentIrql(),
                     0);
    }

    if (Thread != KeGetCurrentThread()) {

        //
        // Invalid Thread.   The thread this floating point context
        // belongs to is not the current thread (or the saved thread
        // field is trash).
        //

        KeBugCheckEx(INVALID_FLOATING_POINT_STATE,
                     2,
                     (ULONG_PTR)Thread,
                     (ULONG_PTR)KeGetCurrentThread(),
                     0);
    }


    //
    // Synchronize with context switches and the npx trap handlers
    //

    _asm {
        cli
    }

    //
    // Restore the required state
    //

    if (FloatSave->Flags & FLOAT_SAVE_COMPLETE_CONTEXT) {

        //
        // Restore the entire fp state to the threads save area
        //

        if (Thread->NpxState == NPX_STATE_LOADED) {

            //
            // This state in the fp unit is no longer needed, just disregard it
            //

            Thread->NpxState = NPX_STATE_NOT_LOADED;
            KeGetCurrentPrcb()->NpxThread = NULL;
        }

        //
        // Copy restored state to npx frame
        //

        RtlCopyMemory (NpxFrame, FloatSave->u.Context, sizeof(FX_SAVE_AREA));

    } else {

        //
        // Restore the non-volatile state
        //

        if (Thread->NpxState == NPX_STATE_LOADED) {

            //
            // Init fp state and restore control word
            //

            _asm {
                fninit
                mov     eax, FloatSave
                fldcw   [eax] FLOAT_SAVE.u.Fcw.ControlWord
            }

            Kix86LdMXCsr(&FloatSave->u.Fcw.MXCsr);

        } else {

            //
            // Fp state not loaded.  Restore control word in npx frame
            //

            NpxFrame->FloatSave.ControlWord = FloatSave->u.Fcw.ControlWord;
            NpxFrame->FloatSave.StatusWord = 0;
            NpxFrame->FloatSave.TagWord = 0;
            NpxFrame->FloatSave.MXCsr = FloatSave->u.Fcw.MXCsr;
        }

        NpxFrame->FloatSave.Cr0NpxState = FloatSave->Cr0NpxState;
    }

    //
    // Restore NpxIrql and Cr0
    //

    Thread->NpxIrql = FloatSave->PreviousNpxIrql;
    Cr0State = Thread->NpxState | NpxFrame->FloatSave.Cr0NpxState;

    _asm {
        mov     eax, cr0
        mov     ecx, eax
        and     eax, not (CR0_MP|CR0_EM|CR0_TS)
        or      eax, Cr0State
        cmp     eax, ecx
        je      short res10
        mov     cr0, eax
res10:
        sti
    }

    //
    // Done
    //

    if (FloatSave->Flags & FLOAT_SAVE_FREE_CONTEXT_HEAP) {
        ExFreePool (FloatSave->u.Context);
    }

    FloatSave->Flags = 0;
    return STATUS_SUCCESS;
}

