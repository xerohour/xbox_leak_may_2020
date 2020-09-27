        title   "Software Interrupts"

;++
;
; Copyright (c) 1992  Microsoft Corporation
;
; Module Name:
;
;    ixswint.asm
;
; Abstract:
;
;    This module implements the software interrupt handlers
;    for x86 machines
;
; Author:
;
;    John Vert (jvert) 2-Jan-1992
;
; Environment:
;
;    Kernel mode only.
;
; Revision History:
;
;--

.386p
        .xlist
include hal386.inc
include callconv.inc                    ; calling convention macros
include i386\ix8259.inc
include i386\kimacro.inc
        .list

        EXTRNP  _KiDeliverApc,0
        EXTRNP  _KiDispatchInterrupt,0
        EXTRNP  Kei386EoiHelper,0
        extrn   SWInterruptHandlerTable:dword
        extrn   SWInterruptLookUpTable:byte
        extrn   HalpIRR:dword
        extrn   _KiPCR:DWORD

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:FLAT, FS:NOTHING, GS:NOTHING

        page ,132
        subttl  "Request Software Interrupt"

;++
;
; VOID
; HalRequestSoftwareInterrupt (
;    IN KIRQL RequestIrql
;    )
;
; Routine Description:
;
;    This routine is used to request a software interrupt to the
;    system. Also, this routine checks to see if any software
;    interrupt should be generated.
;    The following condition will cause software interrupt to
;    be simulated:
;      any software interrupt which has higher priority than
;        current IRQL's is pending.
;
;    NOTE: This routine simulates software interrupt as long as
;          any pending SW interrupt level is higher than the current
;          IRQL, even when interrupts are disabled.
;
; Arguments:
;
;    (cl) = RequestIrql - Supplies the request IRQL value
;
; Return Value:
;
;    None.
;
;--

cPublicFastCall HalRequestSoftwareInterrupt ,1
cPublicFpo 0, 1

        mov     eax,1
        shl     eax, cl                 ; convert to mask
        pushfd                          ; save interrupt mode
        cli                             ; disable interrupt
        or      HalpIRR, eax            ; set the request bit
        mov     cl, PCRB[PcIrql]        ; get current IRQL

        mov     eax, HalpIRR            ; get SW interrupt request register
        and     eax, 3                  ; mask off pending HW interrupts

        xor     edx, edx
        mov     dl, SWInterruptLookUpTable[eax] ; get the highest pending
                                        ; software interrupt level
        cmp     dl, cl                  ; Is highest SW int level > irql?
        jbe     short KsiExit           ; No, jmp ksiexit
        call    SWInterruptHandlerTable[edx*4] ; yes, simulate interrupt
                                        ; to the appropriate handler
KsiExit:
        popfd                           ; restore original interrupt mode
        fstRET  HalRequestSoftwareInterrupt

fstENDP HalRequestSoftwareInterrupt

        page ,132
        subttl  "Request Software Interrupt"

;++
;
; VOID
; HalClearSoftwareInterrupt (
;    IN KIRQL RequestIrql
;    )
;
; Routine Description:
;
;   This routine is used to clear a possible pending software interrupt.
;   Support for this function is optional, and allows the kernel to
;   reduce the number of spurious software interrupts it receives/
;
; Arguments:
;
;    (cl) = RequestIrql - Supplies the request IRQL value
;
; Return Value:
;
;    None.
;
;--

cPublicFastCall HalClearSoftwareInterrupt ,1
cPublicFpo 0, 0

        mov     eax,1
        shl     eax, cl                 ; convert to mask

        not     eax
        and     HalpIRR, eax            ; clear pending irr bit

        fstRET  HalClearSoftwareInterrupt

fstENDP HalClearSoftwareInterrupt



        page ,132
        subttl  "Dispatch Interrupt"
;++
;
; VOID
; HalpDispatchInterrupt(
;       VOID
;       );
;
; Routine Description:
;
;    This routine is the interrupt handler for a software interrupt generated
;    at DISPATCH_LEVEL.  Its function is to save the machine state, raise
;    Irql to DISPATCH_LEVEL, dismiss the interrupt, and call the DPC
;    delivery routine.
;
; Arguments:
;
;    None
;    Interrupt is disabled
;
; Return Value:
;
;    None.
;
;--

        align dword
        public _HalpDispatchInterrupt
_HalpDispatchInterrupt proc

;
; Create IRET frame on stack
;
        pop     eax
        pushfd
        push    cs
        push    eax

;
; Save machine state on trap frame
;

        ENTER_INTERRUPT
.FPO ( FPO_LOCALS+1, 0, 0, 0, 0, FPO_TRAPFRAME )

        public  _HalpDispatchInterrupt2ndEntry
_HalpDispatchInterrupt2ndEntry:

; Save previous IRQL and set new priority level

        push    PCR[PcIrql]                       ; save previous IRQL
        mov     byte ptr PCR[PcIrql], DISPATCH_LEVEL; set new irql
        and     dword ptr HalpIRR, not (1 shl DISPATCH_LEVEL) ; clear the pending bit in IRR

;
; Now it is safe to enable interrupt to allow higher priority interrupt
; to come in.
;

        sti

;
; Go do Dispatch Interrupt processing
;
        stdCall   _KiDispatchInterrupt

;
; Do interrupt exit processing
;

        SOFT_INTERRUPT_EXIT                          ; will do an iret

_HalpDispatchInterrupt endp

        page ,132
        subttl  "APC Interrupt"
;++
;
; HalpApcInterrupt(
;       VOID
;       );
;
; Routine Description:
;
;    This routine is entered as the result of a software interrupt generated
;    at APC_LEVEL. Its function is to save the machine state, raise Irql to
;    APC_LEVEL, dismiss the interrupt, and call the APC delivery routine.
;
; Arguments:
;
;    None
;    Interrupt is Disabled
;
; Return Value:
;
;    None.
;
;--

        align dword
        public _HalpApcInterrupt
_HalpApcInterrupt proc

;
; Create IRET frame on stack
;
        pop     eax
        pushfd
        push    cs
        push    eax

;
; Save machine state in trap frame
;
        ENTER_INTERRUPT
.FPO ( FPO_LOCALS+1, 0, 0, 0, 0, FPO_TRAPFRAME )


        public     _HalpApcInterrupt2ndEntry
_HalpApcInterrupt2ndEntry:

;
; Save previous IRQL and set new priority level
;

        push    PCR[PcIrql]                 ; save previous Irql
        mov     PCRB[PcIrql], APC_LEVEL     ; set new Irql
        and     dword ptr HalpIRR, not (1 shl APC_LEVEL) ; dismiss pending APC
;
; Now it is safe to enable interrupt to allow higher priority interrupt
; to come in.
;

        sti

;
; call the APC delivery routine.
;

        stdCall   _KiDeliverApc

;
;
; Do interrupt exit processing
;

        SOFT_INTERRUPT_EXIT                  ; will do an iret

_HalpApcInterrupt       endp

_TEXT   ends

        end
