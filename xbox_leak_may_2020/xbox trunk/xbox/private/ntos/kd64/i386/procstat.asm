        title  "Processor State Save Restore"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    procstat.asm
;
; Abstract:
;
;    This module implements procedures for saving and restoring
;    processor control state, and processor run&control state.
;    These procedures support debugging of UP and MP systems.
;
; Author:
;
;    Shie-Lin Tzong (shielint) 30-Aug-1990
;
; Environment:
;
;    Kernel mode only.
;
; Revision History:
;
;--

.586p
        .xlist
include ks386.inc
include callconv.inc
        .list

        page ,132
KDCODE  SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page    ,132
        subttl  "Save Processor Control State"
;++
;
; KdpSaveProcessorControlState(
;       PX86_NT5_KPROCESSOR_STATE   ProcessorState
;       );
;
; Routine Description:
;
;    This routine saves the control subset of the processor state.
;    (Saves the same information as KdpSaveProcessorState EXCEPT that
;     data in TrapFrame/ExceptionFrame=Context record is NOT saved.)
;    Called by the debug subsystem, and KdpSaveProcessorState()
;
;   N.B.  This procedure will save Dr7, and then 0 it.  This prevents
;         recursive hardware trace breakpoints and allows debuggers
;         to work.
;
; Arguments:
;
; Return Value:
;
;    None.
;
;--

cPublicProc _KdpSaveProcessorControlState   ,1

        mov     edx, [esp+4]                    ; ProcessorState

;
; Save special registers for debugger
;
        xor     ecx,ecx

        mov     eax, cr0
        mov     [edx].PsNT5SpecialRegisters.SrNT5Cr0, eax
        mov     eax, cr2
        mov     [edx].PsNT5SpecialRegisters.SrNT5Cr2, eax
        mov     eax, cr3
        mov     [edx].PsNT5SpecialRegisters.SrNT5Cr3, eax
        mov     eax, cr4
        mov     [edx].PsNT5SpecialRegisters.SrNT5Cr4, eax

        mov     eax,dr0
        mov     [edx].PsNT5SpecialRegisters.SrNT5KernelDr0,eax
        mov     eax,dr1
        mov     [edx].PsNT5SpecialRegisters.SrNT5KernelDr1,eax
        mov     eax,dr2
        mov     [edx].PsNT5SpecialRegisters.SrNT5KernelDr2,eax
        mov     eax,dr3
        mov     [edx].PsNT5SpecialRegisters.SrNT5KernelDr3,eax
        mov     eax,dr6
        mov     [edx].PsNT5SpecialRegisters.SrNT5KernelDr6,eax

        mov     eax,dr7
        mov     dr7,ecx
        mov     [edx].PsNT5SpecialRegisters.SrNT5KernelDr7,eax

        sgdt    fword ptr [edx].PsNT5SpecialRegisters.SrNT5Gdtr
        sidt    fword ptr [edx].PsNT5SpecialRegisters.SrNT5Idtr

        str     word ptr [edx].PsNT5SpecialRegisters.SrNT5Tr
        sldt    word ptr [edx].PsNT5SpecialRegisters.SrNT5Ldtr

        stdRET    _KdpSaveProcessorControlState

stdENDP _KdpSaveProcessorControlState

        page    ,132
        subttl  "Restore Processor Control State"
;++
;
; KdpRestoreProcessorControlState(
;       PX86_NT5_KPROCESSOR_STATE   ProcessorState
;       );
;
; Routine Description:
;
;    This routine restores the control subset of the processor state.
;    (Restores the same information as KdpRestoreProcessorState EXCEPT that
;     data in TrapFrame/ExceptionFrame=Context record is NOT restored.)
;    Called by the debug subsystem, and KdpRestoreProcessorState()
;
; Arguments:
;
; Return Value:
;
;    None.
;
;--

cPublicProc _KdpRestoreProcessorControlState,1

        mov     edx, [esp+4]                    ; (edx)->ProcessorState

;
; Restore special registers for debugger
;

        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5Cr0
        mov     cr0, eax
        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5Cr2
        mov     cr2, eax
        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5Cr3
        mov     cr3, eax
        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5Cr4
        mov     cr4, eax

        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5KernelDr0
        mov     dr0, eax
        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5KernelDr1
        mov     dr1, eax
        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5KernelDr2
        mov     dr2, eax
        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5KernelDr3
        mov     dr3, eax
        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5KernelDr6
        mov     dr6, eax
        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5KernelDr7
        mov     dr7, eax

        lgdt    fword ptr [edx].PsNT5SpecialRegisters.SrNT5Gdtr
        lidt    fword ptr [edx].PsNT5SpecialRegisters.SrNT5Idtr

;
; Force the TSS descriptor into a non-busy state, so we don't fault
; when we load the TR.
;

        mov     eax, [edx].PsNT5SpecialRegisters.SrNT5Gdtr+2  ; (eax)->GDT base
        xor     ecx, ecx
        mov     cx,  word ptr [edx].PsNT5SpecialRegisters.SrNT5Tr
        add     eax, 5
        add     eax, ecx                                ; (eax)->TSS Desc. Byte
        and     byte ptr [eax],NOT 2
        ltr     word ptr [edx].PsNT5SpecialRegisters.SrNT5Tr

        lldt    word ptr [edx].PsNT5SpecialRegisters.SrNT5Ldtr

        stdRET    _KdpRestoreProcessorControlState

stdENDP _KdpRestoreProcessorControlState

KDCODE  ENDS
        END
