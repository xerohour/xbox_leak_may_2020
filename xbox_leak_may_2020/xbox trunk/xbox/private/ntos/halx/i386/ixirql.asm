        title  "Irql Processing"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    ixirql.asm
;
; Abstract:
;
;    This module implements the code necessary to raise and lower i386
;    Irql and dispatch software interrupts with the 8259 PIC.
;
; Author:
;
;    Shie-Lin Tzong (shielint) 8-Jan-1990
;
; Environment:
;
;    Kernel mode only.
;
; Revision History:
;
;    John Vert (jvert) 27-Nov-1991
;       Moved from kernel into HAL
;
;--

.386p
        .xlist
include hal386.inc
include callconv.inc                    ; calling convention macros
include i386\ix8259.inc
include i386\kimacro.inc
        .list


        EXTRNP  _KeBugCheck,1
        EXTRNP  _KiDispatchInterrupt,0
        extrn   _HalpApcInterrupt:near
        extrn   _HalpDispatchInterrupt:near
        extrn   _KiUnexpectedInterrupt:near
        extrn   _HalpApcInterrupt2ndEntry:NEAR
        extrn   _HalpDispatchInterrupt2ndEntry:NEAR
        extrn   HalpEisaELCR:dword
        extrn   HalpIRR:dword
        extrn   HalpIRRActive:dword
        extrn   HalpIDR:dword
        extrn   _KiPCR:DWORD

;
; Initialization control words equates for the PICs
;

ICW1_ICW4_NEEDED                equ     01H
ICW1_CASCADE                    equ     00H
ICW1_INTERVAL8                  equ     00H
ICW1_LEVEL_TRIG                 equ     08H
ICW1_EDGE_TRIG                  equ     00H
ICW1_ICW                        equ     10H

ICW4_8086_MODE                  equ     001H
ICW4_NORM_EOI                   equ     000H
ICW4_NON_BUF_MODE               equ     000H
ICW4_SPEC_FULLY_NESTED          equ     010H
ICW4_NOT_SPEC_FULLY_NESTED      equ     000H

OCW2_NON_SPECIFIC_EOI           equ     020H
OCW2_SPECIFIC_EOI               equ     060H
OCW2_SET_PRIORITY               equ     0c0H

PIC_SLAVE_IRQ                   equ     2
PIC1_BASE                       equ     30H
PIC2_BASE                       equ     38H

;
; Interrupt flag bit maks for EFLAGS
;

EFLAGS_IF                       equ     200H
EFLAGS_SHIFT                    equ     9

;
; Hardware irq active masks
;

IRQ_ACTIVE_MASK                 equ     0fffffff0h

_TEXT   SEGMENT DWORD PUBLIC 'CODE'

;
; PICsInitializationString - Master PIC initialization command string
;

PICsInitializationString   dw      PIC1_PORT0

;
; Master PIC initialization command
;

                           db      ICW1_ICW + ICW1_EDGE_TRIG + ICW1_INTERVAL8 +\
                                   ICW1_CASCADE + ICW1_ICW4_NEEDED
                           db      PIC1_BASE
                           db      1 SHL PIC_SLAVE_IRQ
                           db      ICW4_NOT_SPEC_FULLY_NESTED + \
                                   ICW4_NON_BUF_MODE + \
                                   ICW4_NORM_EOI + \
                                   ICW4_8086_MODE
;
; Slave PIC initialization command strings
;

                           dw      PIC2_PORT0
                           db      ICW1_ICW + ICW1_EDGE_TRIG + ICW1_INTERVAL8 +\
                                   ICW1_CASCADE + ICW1_ICW4_NEEDED
                           db      PIC2_BASE
                           db      PIC_SLAVE_IRQ
                           db      ICW4_NOT_SPEC_FULLY_NESTED + \
                                   ICW4_NON_BUF_MODE + \
                                   ICW4_NORM_EOI + \
                                   ICW4_8086_MODE
                           dw      0               ; end of string

            align   4
            public  KiI8259MaskTable
KiI8259MaskTable    label   dword
                dd      00000000000000000000000000000000B ; irql 0 (NORMAL)
                dd      00000000000000000000000000000000B ; irql 1 (APC)
                dd      00000000000000000000000000000000B ; irql 2 (DISPATCH)
                dd      00000000000000000000000000000000B ; irql 3
                dd      11111111110000000000000000000000B ; irql 4
                dd      11111111111000000000000000000000B ; irql 5
                dd      11111111111100000000000000000000B ; irql 6
                dd      11111111111110000000000000000000B ; irql 7
                dd      11111111111111000000000000000000B ; irql 8
                dd      11111111111111100000000000000000B ; irql 9
                dd      11111111111111110000000000000000B ; irql 10
                dd      11111111111111111000000000000000B ; irql 11
                dd      11111111111111111100000000000000B ; irql 12
                dd      11111111111111111110000000000000B ; irql 13
                dd      11111111111111111110000000000000B ; irql 14
                dd      11111111111111111110100000000000B ; irql 15
                dd      11111111111111111110110000000000B ; irql 16
                dd      11111111111111111110111000000000B ; irql 17
                dd      11111111111111111110111000000000B ; irql 18
                dd      11111111111111111110111010000000B ; irql 19
                dd      11111111111111111110111011000000B ; irql 20
                dd      11111111111111111110111011100000B ; irql 21
                dd      11111111111111111110111011110000B ; irql 22
                dd      11111111111111111110111011111000B ; irql 23
                dd      11111111111111111110111011111000B ; irql 24
                dd      11111111111111111110111011111010B ; irql 25
                dd      11111111111111111110111111111010B ; irql 26 (PROFILE)
                dd      11111111111111111111111111111010B ; irql 27 (SCI)
                dd      11111111111111111111111111111011B ; irql 28 (CLOCK)
                dd      11111111111111111111111111111011B ; irql 29 (IPI)
                dd      11111111111111111111111111111011B ; irql 30 (POWER)
                dd      11111111111111111111111111111011B ; irql 31 (HIGH)

;
; This table is used to mask all pending interrupts below a given Irql
; out of the IRR
;
        align 4

        public FindHigherIrqlMask
FindHigherIrqlMask label dword
                dd    11111111111111111111111111111110B ; irql 0 (NORMAL)
                dd    11111111111111111111111111111100B ; irql 1 (APC)
                dd    11111111111111111111111111111000B ; irql 2 (DISPATCH)
                dd    11111111111111111111111111110000B ; irql 3
                dd    00000011111111111111111111110000B ; irql 4
                dd    00000001111111111111111111110000B ; irql 5
                dd    00000000111111111111111111110000B ; irql 6
                dd    00000000011111111111111111110000B ; irql 7
                dd    00000000001111111111111111110000B ; irql 8
                dd    00000000000111111111111111110000B ; irql 9
                dd    00000000000011111111111111110000B ; irql 10
                dd    00000000000001111111111111110000B ; irql 11
                dd    00000000000000111111111111110000B ; irql 12
                dd    00000000000000011111111111110000B ; irql 13
                dd    00000000000000011111111111110000B ; irql 14
                dd    00000000000000010111111111110000B ; irql 15
                dd    00000000000000010011111111110000B ; irql 16
                dd    00000000000000010001111111110000B ; irql 17
                dd    00000000000000010001111111110000B ; irql 18
                dd    00000000000000010001011111110000B ; irql 19
                dd    00000000000000010001001111110000B ; irql 20
                dd    00000000000000010001000111110000B ; irql 21
                dd    00000000000000010001000011110000B ; irql 22
                dd    00000000000000010001000001110000B ; irql 23
                dd    00000000000000010001000000110000B ; irql 24
                dd    00000000000000010001000000010000B ; irql 25
                dd    00000000000000010000000000010000B ; irql 26 (PROFILE)
                dd    00000000000000000000000000010000B ; irql 27 (SCI)
                dd    00000000000000000000000000000000B ; irql 28 (CLOCK)
                dd    00000000000000000000000000000000B ; irql 29 (IPI)
                dd    00000000000000000000000000000000B ; irql 30 (POWER)
                dd    00000000000000000000000000000000B ; irql 31 (HIGH)

_TEXT   ends

_DATA   SEGMENT DWORD PUBLIC 'DATA'

        align   4
;
; The following tables define the addresses of software interrupt routers
;

;
; Use this table if there is NO machine state frame on stack already
;

        public  SWInterruptHandlerTable
SWInterruptHandlerTable label dword
        dd      offset FLAT:_KiUnexpectedInterrupt      ; irql 0
        dd      offset FLAT:_HalpApcInterrupt           ; irql 1
        dd      offset FLAT:_HalpDispatchInterrupt2     ; irql 2
        dd      offset FLAT:_KiUnexpectedInterrupt      ; irql 3
        dd      offset FLAT:HalpHardwareInterrupt00     ; 8259 irq#0
        dd      offset FLAT:HalpHardwareInterrupt01     ; 8259 irq#1
        dd      offset FLAT:HalpHardwareInterrupt02     ; 8259 irq#2
        dd      offset FLAT:HalpHardwareInterrupt03     ; 8259 irq#3
        dd      offset FLAT:HalpHardwareInterrupt04     ; 8259 irq#4
        dd      offset FLAT:HalpHardwareInterrupt05     ; 8259 irq#5
        dd      offset FLAT:HalpHardwareInterrupt06     ; 8259 irq#6
        dd      offset FLAT:HalpHardwareInterrupt07     ; 8259 irq#7
        dd      offset FLAT:HalpHardwareInterrupt08     ; 8259 irq#8
        dd      offset FLAT:HalpHardwareInterrupt09     ; 8259 irq#9
        dd      offset FLAT:HalpHardwareInterrupt10     ; 8259 irq#10
        dd      offset FLAT:HalpHardwareInterrupt11     ; 8259 irq#11
        dd      offset FLAT:HalpHardwareInterrupt12     ; 8259 irq#12
        dd      offset FLAT:HalpHardwareInterrupt13     ; 8259 irq#13
        dd      offset FLAT:HalpHardwareInterrupt14     ; 8259 irq#14
        dd      offset FLAT:HalpHardwareInterrupt15     ; 8259 irq#15

_DATA   ENDS

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:FLAT, FS:NOTHING, GS:NOTHING

;
; Use this table if there is already a machine state frame on stack
;

        public  SWInterruptHandlerTable2
SWInterruptHandlerTable2 label dword
        dd      offset FLAT:_KiUnexpectedInterrupt      ; irql 0
        dd      offset FLAT:_HalpApcInterrupt2ndEntry   ; irql 1
        dd      offset FLAT:_HalpDispatchInterrupt2ndEntry ; irql 2

;
; The following table picks up the highest pending software irq level
; from software irr
;

        public  SWInterruptLookUpTable
SWInterruptLookUpTable label byte
        db      0               ; SWIRR=0, so highest pending SW irql= 0
        db      0               ; SWIRR=1, so highest pending SW irql= 0
        db      1               ; SWIRR=2, so highest pending SW irql= 1
        db      1               ; SWIRR=3, so highest pending SW irql= 1
        db      2               ; SWIRR=4, so highest pending SW irql= 2
        db      2               ; SWIRR=5, so highest pending SW irql= 2
        db      2               ; SWIRR=6, so highest pending SW irql= 2
        db      2               ; SWIRR=7, so highest pending SW irql= 2

;++
;
; KIRQL
; KfRaiseIrql (
;    IN KIRQL NewIrql,
;    )
;
; Routine Description:
;
;    This routine is used to raise IRQL to the specified value.
;    Also, a mask will be used to mask off all the lower lever 8259
;    interrupts.
;
; Arguments:
;
;    (cl) = NewIrql - the new irql to be raised to
;
; Return Value:
;
;    OldIrql - the addr of a variable which old irql should be stored
;
;--

cPublicFastCall KfRaiseIrql,1
cPublicFpo 0, 0

        xor     eax, eax        ; Eliminate partial stall on return to caller
        mov     al, PCRB[PcIrql]         ; (al) = Old Irql
        mov     PCRB[PcIrql], cl         ; set new irql

if DBG
        cmp     al, cl                   ; old > new?
        ja      short Kri99              ; yes, go bugcheck

        fstRET  KfRaiseIrql

cPublicFpo 2, 2
Kri99:
        movzx   eax, al
        movzx   ecx, cl
        push    ecx                      ; put new irql where we can find it
        push    eax                      ; put old irql where we can find it
        mov     PCRB[PcIrql],0           ; avoid recursive error
        stdCall   _KeBugCheck, <IRQL_NOT_GREATER_OR_EQUAL>        ; never return
endif
        fstRET  KfRaiseIrql

fstENDP KfRaiseIrql

;++
;
; KIRQL
; KeRaiseIrqlToDpcLevel (
;    )
;
; Routine Description:
;
;    This routine is used to raise IRQL to DPC level.
;
; Arguments:
;
; Return Value:
;
;    OldIrql - the addr of a variable which old irql should be stored
;
;--

cPublicProc _KeRaiseIrqlToDpcLevel,0
cPublicFpo 0, 0

        xor     eax, eax                ; Eliminate partial stall
        mov     al, PCRB[PcIrql]        ; (al) = Old Irql
        mov     PCRB[PcIrql], DISPATCH_LEVEL    ; set new irql

if DBG
        cmp     al, DISPATCH_LEVEL      ; old > new?
        ja      short Krid99            ; yes, go bugcheck
endif

        stdRET  _KeRaiseIrqlToDpcLevel

if DBG
cPublicFpo 0,1
Krid99: movzx   eax, al
        push    eax                     ; put old irql where we can find it
        stdCall   _KeBugCheck, <IRQL_NOT_GREATER_OR_EQUAL>        ; never return
        stdRET  _KeRaiseIrqlToDpcLevel
endif

stdENDP _KeRaiseIrqlToDpcLevel


;++
;
; KIRQL
; KeRaiseIrqlToSynchLevel (
;    )
;
; Routine Description:
;
;    This routine is used to raise IRQL to SYNC level.
;
; Arguments:
;
; Return Value:
;
;    OldIrql - the addr of a variable which old irql should be stored
;
;--

cPublicProc _KeRaiseIrqlToSynchLevel,0
cPublicFpo 0, 0

        xor     eax, eax                ; Eliminate partial stall
        mov     al, PCRB[PcIrql]        ; (al) = Old Irql
        mov     PCRB[PcIrql], SYNCH_LEVEL       ; set new irql

if DBG
        cmp     al, SYNCH_LEVEL         ; old > new?
        ja      short Kris99            ; yes, go bugcheck
endif

        stdRET  _KeRaiseIrqlToSynchLevel

if DBG
cPublicFpo 0,1
Kris99: movzx   eax, al
        push    eax                     ; put old irql where we can find it
        stdCall   _KeBugCheck, <IRQL_NOT_GREATER_OR_EQUAL>        ; never return
        stdRET  _KeRaiseIrqlToSynchLevel
endif

stdENDP _KeRaiseIrqlToSynchLevel

;++
;
; VOID
; KfLowerIrql (
;    IN KIRQL NewIrql
;    )
;
; Routine Description:
;
;    This routine is used to lower IRQL to the specified value.
;    The IRQL and PIRQL will be updated accordingly.  Also, this
;    routine checks to see if any software interrupt should be
;    generated.  The following condition will cause software
;    interrupt to be simulated:
;      any software interrupt which has higher priority than
;        current IRQL's is pending.
;
;    NOTE: This routine simulates software interrupt as long as
;          any pending SW interrupt level is higher than the current
;          IRQL, even when interrupts are disabled.
;
; Arguments:
;
;    (cl) = NewIrql - the new irql to be set.
;
; Return Value:
;
;    None.
;
;--

cPublicFastCall KfLowerIrql,1
cPublicFpo 0, 0
        and     ecx, 0ffh

if DBG
        cmp     cl,PCRB[PcIrql]         ; Make sure we are not lowering to
        ja      KliBug                  ; ABOVE current level
endif
        pushfd
        cli
        mov     PCR[PcIrql], ecx
        mov     edx, HalpIRR
        and     edx, FindHigherIrqlMask[ecx*4]  ; (edx) is the bitmask of
                                                ; pending interrupts we need to
                                                ; dispatch now.
        jnz     short Kli10                     ; go dispatch pending interrupts

ifdef DEVKIT
        mov     eax, PCR[PcPrcbData+PbDebugHaltThread] ; find out if the
        test    eax,eax                         ; debugger wants to break into
        jnz     Kli20                           ; this thread
endif

;
; no interrupts pending, return quickly.
;

        popfd
        fstRET    KfLowerIrql

cPublicFpo 1, 1
align 4
Kli10:

;
; If there is a pending hardware interrupt, then the PICs have been
; masked to reflect the actual Irql.
;

        bsr     ecx, edx                        ; (ecx) = Pending irq level
        cmp     ecx, DISPATCH_LEVEL
        ja      short Kli40

        call    SWInterruptHandlerTable[ecx*4]  ; Dispatch the pending int.
        popfd

cPublicFpo 1, 0
        fstRET    KfLowerIrql

Kli40:
;
; Clear all the interrupt masks
;

        mov     eax, HalpIDR
        SET_8259_MASK

        mov     edx, 1
        shl     edx, cl
        xor     HalpIRR, edx                    ; clear bit in IRR
        call    SWInterruptHandlerTable[ecx*4]  ; Dispatch the pending int.
        popfd

cPublicFpo 1, 0
        fstRET    KfLowerIrql

if DBG
cPublicFpo 1, 2
KliBug:
        push    ecx                             ; new irql for debugging
        push    PCR[PcIrql]                     ; old irql for debugging
        mov     PCRB[PcIrql],HIGH_LEVEL         ; avoid recursive error
        stdCall   _KeBugCheck, <IRQL_NOT_LESS_OR_EQUAL>   ; never return
endif

ifdef DEVKIT
cPublicFpo 0, 0
Kli20:
        test    ecx,ecx                         ; only break if lowering to
        jne     Kli21                           ; PASSIVE_LEVEL
        call    eax                             ; find out whether to stop
        test    al, al
        je      short Kli21
        sti
        int     3h
Kli21:
        popfd
        fstRet    KfLowerIrql
endif

fstENDP KfLowerIrql

;++
;
; VOID
; FASTCALL
; HalEndSystemLevelInterrupt(
;    IN KIRQL NewIrql
;    )
;
; Routine Description:
;
;    This routine acknowledges the supplied level interrupt and then falls
;    into HalEndSystemInterrupt.
;
; Arguments:
;
;    eax - BusInterruptLevel
;
;    NewIrql - the new irql to be set.
;
;    Note that esp+4 is the beginning of interrupt/trap frame and upon
;    entering to this routine the interrupts are off.
;
; Return Value:
;
;    None.
;
;--

cPublicFastCall HalEndSystemLevelInterrupt  ,1

        cmp     eax, 8                      ; EOI to master or slave?
        jae     short Heil10

        or      al, PIC1_EOI_MASK           ; create specific eoi mask for master
        out     PIC1_PORT0, al              ; dismiss the interrupt
        jmp     @HalEndSystemInterrupt@4

align 4
Heil10:
        add     al, OCW2_SPECIFIC_EOI - 8   ; specific eoi to slave
        out     PIC2_PORT0, al

;
; fall into HalEndSystemInterrupt
;

;++
;
; VOID
; FASTCALL
; HalEndSystemInterrupt(
;    IN KIRQL NewIrql
;    )
;
; Routine Description:
;
;    This routine is used to lower IRQL to the specified value.
;    The IRQL and PIRQL will be updated accordingly.  Also, this
;    routine checks to see if any software interrupt should be
;    generated.  The following condition will cause software
;    interrupt to be simulated:
;      any software interrupt which has higher priority than
;        current IRQL's is pending.
;
;    NOTE: This routine simulates software interrupt as long as
;          any pending SW interrupt level is higher than the current
;          IRQL, even when interrupts are disabled.
;
; Arguments:
;
;    NewIrql - the new irql to be set.
;
;    Note that esp+4 is the beginning of interrupt/trap frame and upon
;    entering to this routine the interrupts are off.
;
; Return Value:
;
;    None.
;
;--

cPublicFastCall HalEndSystemInterrupt  ,1

        mov     edx, HalpIRR
        and     edx, FindHigherIrqlMask[ecx*4]  ; (edx) is the bitmask of
                                                ; pending interrupts we need to
                                                ; dispatch now.
        mov     PCR[PcIrql], ecx
        jnz     Hei10                           ; go dispatch pending interrupts

;
; no interrupts pending, fall into Kei386EoiHelper
;

;++
;
;   Kei386EoiHelper
;
;   Routine Description:
;
;       This code is transfered to at the end of an interrupt.  (via the
;       exit_interrupt macro).  It checks for user APC dispatching and
;       performs the exit_all for the interrupt.
;
;       NOTE: This code is jumped to, not called.
;
;   Arguments:
;
;       (esp) -> base of trap frame.
;       interrupts are disabled
;
;   Return Value:
;
;       None.
;
;--
cPublicProc Kei386EoiHelper, 0
;.FPO (0, 0, 0, 0, 0, FPO_TRAPFRAME)
        ASSERT_FS
        EXIT_ALL    ,,NoPreviousMode
stdENDP Kei386EoiHelper

align 4
Hei10:

;
; If there is any delayed hardware interrupt being serviced, we leave
; the interrupt masked and simply return.
;

        test    HalpIRRActive, IRQ_ACTIVE_MASK
        jnz     short Hei50

        bsr     ecx, edx                        ; (eax) = Pending irq level
        cmp     ecx, DISPATCH_LEVEL
        jle     short Hei40

;
; Clear all the interrupt masks
;

align 4
Hei15:
        mov     eax, HalpIDR
        SET_8259_MASK
;
; The pending interrupt is a hardware interrupt.  To prevent the delayed
; interrupts from overflowing stack, we check if the pending level is already
; active.  If yes, we simply return and let the higher level EndSystemInterrupt
; handle it.
;
; (ecx) = pending vector
;

        mov     edx, 1
        shl     edx, cl
        test    HalpIRRActive, edx              ; if the pending int is being
                                                ; processed, just return.
        jne     short Hei50
        or      HalpIRRActive, edx              ; Set Active bit
        xor     HalpIRR, edx                    ; clear bit in IRR
        call    SWInterruptHandlerTable[ecx*4]  ; Note, it destroys eax
        xor     HalpIRRActive, edx              ; Clear bit in ActiveIrql
        mov     eax, HalpIRR                    ; Reload IRR
        mov     ecx, PCR[PcIrql]
        and     eax, FindHigherIrqlMask[ecx*4]  ; Is any interrupt pending
        jz      short Hei50                     ; (Most time it will be zero.)
        bsr     ecx, eax                        ; (edx) = Pending irq level
        cmp     ecx, DISPATCH_LEVEL
        ja      short Hei15

Hei40:

;
; The pending interrupt is at Software Level.  We simply make current
; interrupt frame the new pending software interrupt's frame and
; jmp to the handler routine.
;

        jmp     SWInterruptHandlerTable2[ecx*4] ; Note, it destroys eax

Hei50:
        jmp     Kei386EoiHelper@0

fstENDP HalEndSystemInterrupt

fstENDP HalEndSystemLevelInterrupt

;++
;
; VOID
; HalpEndSoftwareInterrupt
;    IN KIRQL NewIrql
;    )
;
; Routine Description:
;
;    This routine is used to lower IRQL from software interrupt
;    leverl to the specified value.
;    The IRQL and PIRQL will be updated accordingly.  Also, this
;    routine checks to see if any software interrupt should be
;    generated.  The following condition will cause software
;    interrupt to be simulated:
;      any software interrupt which has higher priority than
;        current IRQL's is pending.
;
;    NOTE: This routine simulates software interrupt as long as
;          any pending SW interrupt level is higher than the current
;          IRQL, even when interrupts are disabled.
;
; Arguments:
;
;    NewIrql - the new irql to be set.
;
;    Note that esp+8 is the beginning of interrupt/trap frame and upon
;    entering to this routine the interrupts are off.
;
; Return Value:
;
;    None.
;
;--

HesNewIrql      equ     [esp + 4]

cPublicProc _HalpEndSoftwareInterrupt  ,1
cPublicFpo 1, 0

        movzx   ecx, byte ptr HesNewIrql        ; get new irql value
        mov     edx, HalpIRR
        and     edx, FindHigherIrqlMask[ecx*4]  ; (edx) is the bitmask of
                                                ; pending interrupts we need to
                                                ; dispatch now.
        mov     PCR[PcIrql], ecx
        jnz     short Hes10

        stdRET    _HalpEndSoftwareInterrupt

align 4
Hes10:
;
; Check if any delayed hardware interrupt is being serviced.  If yes, we
; simply return.
;

        test    HalpIRRActive, IRQ_ACTIVE_MASK
        jnz     short Hes90

;
; If there is a pending hardware interrupt, then the PICs have been
; masked to reflect the actual Irql.
;

        bsr     ecx, edx                        ; (ecx) = Pending irq level
        cmp     ecx, DISPATCH_LEVEL
        ja      short Hes20

;
; Pending interrupt is a soft interrupt. Recycle stack frame
;

        add     esp, 8
        jmp     SWInterruptHandlerTable2[ecx*4] ; Note, it destroys eax

Hes20:
;
; Clear all the interrupt masks
;

        mov     eax, HalpIDR
        SET_8259_MASK

;
; (ecx) = Pending level
;

        mov     edx, 1
        shl     edx, cl

        or      HalpIRRActive, edx              ; Set Active bit
        xor     HalpIRR, edx                    ; clear bit in IRR

        call    SWInterruptHandlerTable[ecx*4]  ; Dispatch the pending int.

        xor     HalpIRRActive, edx              ; Clear bit in ActiveIrql

        movzx   ecx, byte ptr HesNewIrql        ; get new irql value
        mov     edx, HalpIRR
        and     edx, FindHigherIrqlMask[ecx*4]  ; (edx) is the bitmask of
                                                ; pending interrupts we need to
                                                ; dispatch now.
        jnz     short Hes10

Hes90:  stdRET  _HalpEndSoftwareInterrupt

stdENDP _HalpEndSoftwareInterrupt

        page ,132
        subttl  "DispatchInterrupt 2"

;++
;
; VOID
; HalpDispatchInterrupt2(
;       VOID
;       );
;
; Routine Description:
;
;   The functional description is the same as HalpDispatchInterrupt.
;
;   This function differs from HalpDispatchInterrupt in how it has been
;   optimized.  This function is optimized for dispatching dispatch interrupts
;   for LowerIrql, ReleaseSpinLock, and RequestSoftwareInterrupt.
;
; Arguments:
;
;    None
;    Interrupt is disabled
;
; Return Value:
;
;    (edx) = 1 shl DISPATCH_LEVEL
;
; Warnings:
;
;   Not all SW int handles this hal uses save all the registers
;   callers to SWInterruptHandler for H/W interrupts assume that
;   ONLY EAX & ECX are destroyed.
;
;   Note: this function saves EBX since KiDispatchInterrupt uses
;   the value without preserving it.
;--

cPublicProc _HalpDispatchInterrupt2
cPublicFpo 0, 2

        xor     ecx, ecx
        and     dword ptr HalpIRR, not (1 shl DISPATCH_LEVEL) ; clear the pending bit in IRR

        mov     cl, PCRB[PcIrql]

        mov     PCRB[PcIrql], DISPATCH_LEVEL    ; set new irql
        push    ecx                             ; Save OldIrql

;
; Now it is safe to enable interrupt to allow higher priority interrupt
; to come in.
;
        sti

        push    ebx
        stdCall _KiDispatchInterrupt            ; Handle DispatchInterrupt
        pop     ebx
        pop     ecx                             ; (ecx) = OldIrql
        mov     edx, 1 shl DISPATCH_LEVEL

        cli

        mov     eax, HalpIRR
        mov     PCR[PcIrql], ecx                ; restore current irql

        and     eax, FindHigherIrqlMask[ecx*4]  ; (eax) is the bitmask of
                                                ; pending interrupts we need to
                                                ; dispatch now.

        jnz     short diq10                     ; go dispatch pending interrupts
        stdRET  _HalpDispatchInterrupt2

diq10:
;
; If there is a pending hardware interrupt, then the PICs have been
; masked to reflect the actual Irql.
;

        bsr     ecx, eax                        ; (ecx) = Pending irq level
        cmp     ecx, DISPATCH_LEVEL
        jbe     short diq20

;
; Clear all the interrupt masks
;

        mov     eax, HalpIDR
        SET_8259_MASK

        mov     edx, 1
        shl     edx, cl
        xor     HalpIRR, edx                    ; clear bit in IRR

diq20:
;
; (ecx) = Pending level
;

        jmp     SWInterruptHandlerTable[ecx*4]  ; Dispatch the pending int.
diq90:  stdRET  _HalpDispatchInterrupt2

stdENDP _HalpDispatchInterrupt2

        page ,132
        subttl  "Get current irql"

;++
;
; KIRQL
; KeGetCurrentIrql (VOID)
;
; Routine Description:
;
;    This routine returns to current IRQL.
;
; Arguments:
;
;    None.
;
; Return Value:
;
;    The current IRQL.
;
;--

cPublicProc _KeGetCurrentIrql   ,0
cPublicFpo 0, 0

        movzx   eax, PCRB[PcIrql]           ; Current irql is in the PCR
        stdRET    _KeGetCurrentIrql
stdENDP _KeGetCurrentIrql

        page ,132
        subttl  "Postponed Hardware Interrupt Dispatcher"
;++
;
; VOID
; HalpHardwareInterruptNN (
;       VOID
;       );
;
; Routine Description:
;
;    These routines branch through the IDT to simulate the appropriate
;    hardware interrupt.  They use the "INT nn" instruction to do this.
;
; Arguments:
;
;    None.
;
; Returns:
;
;    None.
;
; Environment:
;
;    IRET frame is on the stack
;
;--
cPublicProc _HalpHardwareInterruptTable, 0
cPublicFpo 0,0

        public HalpHardwareInterrupt00
HalpHardwareInterrupt00 label byte
        int     PRIMARY_VECTOR_BASE + 0
        ret

        public HalpHardwareInterrupt01
HalpHardwareInterrupt01 label byte
        int     PRIMARY_VECTOR_BASE + 1
        ret

        public HalpHardwareInterrupt02
HalpHardwareInterrupt02 label byte
        int     PRIMARY_VECTOR_BASE + 2
        ret

        public HalpHardwareInterrupt03
HalpHardwareInterrupt03 label byte
        int     PRIMARY_VECTOR_BASE + 3
        ret

        public HalpHardwareInterrupt04
HalpHardwareInterrupt04 label byte
        int     PRIMARY_VECTOR_BASE + 4
        ret

        public HalpHardwareInterrupt05
HalpHardwareInterrupt05 label byte
        int     PRIMARY_VECTOR_BASE + 5
        ret

        public HalpHardwareInterrupt06
HalpHardwareInterrupt06 label byte
        int     PRIMARY_VECTOR_BASE + 6
        ret

        public HalpHardwareInterrupt07
HalpHardwareInterrupt07 label byte
        int     PRIMARY_VECTOR_BASE + 7
        ret

        public HalpHardwareInterrupt08
HalpHardwareInterrupt08 label byte
        int     PRIMARY_VECTOR_BASE + 8
        ret

        public HalpHardwareInterrupt09
HalpHardwareInterrupt09 label byte
        int     PRIMARY_VECTOR_BASE + 9
        ret

        public HalpHardwareInterrupt10
HalpHardwareInterrupt10 label byte
        int     PRIMARY_VECTOR_BASE + 10
        ret

        public HalpHardwareInterrupt11
HalpHardwareInterrupt11 label byte
        int     PRIMARY_VECTOR_BASE + 11
        ret

        public HalpHardwareInterrupt12
HalpHardwareInterrupt12 label byte
        int     PRIMARY_VECTOR_BASE + 12
        ret

        public HalpHardwareInterrupt13
HalpHardwareInterrupt13 label byte
        int     PRIMARY_VECTOR_BASE + 13
        ret

        public HalpHardwareInterrupt14
HalpHardwareInterrupt14 label byte
        int     PRIMARY_VECTOR_BASE + 14
        ret

        public HalpHardwareInterrupt15
HalpHardwareInterrupt15 label byte
        int     PRIMARY_VECTOR_BASE + 15
        ret

stdENDP _HalpHardwareInterruptTable

        page ,132
        subttl  "Interrupt Controller Chip Initialization"
;++
;
; VOID
; HalpInitializePICs (
;    VOID
;    )
;
; Routine Description:
;
;    This routine sends the 8259 PIC initialization commands and
;    masks all the interrupts on 8259s.
;
; Arguments:
;
;    EnableInterrupts - If this is true, then this function will
;                       explicitly enable interrupts at the end,
;                       as it always did in the past.  If this
;                       is false, then it will preserve the interrupt
;                       flag.
;
; Return Value:
;
;    None.
;
;--

cPublicProc _HalpInitializePICs       ,0
cPublicFpo 0, 0

        push    esi                             ; save caller's esi
        pushfd
        cli                                     ; disable interrupt

        lea     esi, PICsInitializationString
        lodsw                                   ; (AX) = PIC port 0 address
Hip10:  movzx   edx, ax
        outsb                                   ; output ICW1
        IODelay
        inc     edx                             ; (DX) = PIC port 1 address
        outsb                                   ; output ICW2
        IODelay
        outsb                                   ; output ICW3
        IODelay
        outsb                                   ; output ICW4
        IODelay
        mov     al, 0FFH                        ; mask all 8259 irqs
        out     dx,al                           ; write mask to PIC
        lodsw
        cmp     ax, 0                           ; end of init string?
        jne     short Hip10                     ; go init next PIC

        or      [esp], EFLAGS_INTERRUPT_MASK    ; enable interrupts
        popfd
        pop     esi                             ; restore caller's esi
        stdRET    _HalpInitializePICs
stdENDP _HalpInitializePICs

_TEXT   ends

        end
