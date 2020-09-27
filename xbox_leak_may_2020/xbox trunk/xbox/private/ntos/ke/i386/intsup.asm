        TITLE  "Interrupt Object Support Routines"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    intsup.asm
;
; Abstract:
;
;    This module implements the code necessary to support interrupt objects.
;    It contains the interrupt dispatch code and the code template that gets
;    copied into an interrupt object.
;
; Author:
;
;    Shie-Lin Tzong (shielint) 20-Jan-1990
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
KERNELONLY  equ     1
include ks386.inc
include i386\kimacro.inc
include callconv.inc
        .list

        EXTRNP  KfRaiseIrql,1,,FASTCALL
        EXTRNP  KfLowerIrql,1,,FASTCALL
        EXTRNP  _KeBugCheck,1
        EXTRNP  _KiDeliverApc,0
        EXTRNP  HalBeginSystemInterrupt,2,,FASTCALL
        EXTRNP  HalEndSystemInterrupt,1,,FASTCALL
        EXTRNP  HalEndSystemLevelInterrupt,1,,FASTCALL
        extrn   HalpSpecialDismissTable:DWORD
        extrn   _KiPCR:DWORD
if DBG
        extrn   _DbgPrint:near
        extrn   _MsgISRTimeout:BYTE
        extrn   _MsgISROverflow:BYTE
        extrn   _KeTickCount:DWORD
        extrn   _KiISRTimeout:DWORD
        extrn   _KiISROverflow:DWORD
endif

_DATA   SEGMENT  DWORD PUBLIC 'DATA'

if DBG
COMM    KiInterruptCounts:dword:16*2
endif

_DATA   ends

        page ,132
        subttl  "Synchronize Execution"

_TEXT   SEGMENT PARA PUBLIC 'CODE'

;++
;
; BOOLEAN
; KeSynchronizeExecution (
;    IN PKINTERRUPT Interrupt,
;    IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,
;    IN PVOID SynchronizeContext
;    )
;
; Routine Description:
;
;    This function synchronizes the execution of the specified routine with the
;    execution of the service routine associated with the specified interrupt
;    object.
;
; Arguments:
;
;    Interrupt - Supplies a pointer to a control object of type interrupt.
;
;    SynchronizeRoutine - Supplies a pointer to a function whose execution
;       is to be synchronized with the execution of the service routine associated
;       with the specified interrupt object.
;
;    SynchronizeContext - Supplies a pointer to an arbitrary data structure
;       which is to be passed to the function specified by the SynchronizeRoutine
;       parameter.
;
; Return Value:
;
;    The value returned by the SynchronizeRoutine function is returned as the
;    function value.
;
;--
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING
cPublicProc _KeSynchronizeExecution ,3

; equates of Local variables

KsePreviousIrql equ [ebp - 4]                ; previous IRQL
KseStackSize = 4 * 1

; equates for arguments

KseInterrupt equ [ebp +8]
KseSynchronizeRoutine equ [ebp + 12]
KseSynchronizeContext equ [ebp + 16]

        push    ebp                     ; save ebp
        mov     ebp, esp                ; (ebp)-> base of local variable frame
        sub     esp, KseStackSize       ; allocate local variables space
        push    ebx                     ; save ebx

; Acquire the associated spin lock and raise IRQL to the interrupting source.

        mov     ebx, KseInterrupt       ; (ebx)->interrupt object

        mov     ecx, InIrql[ebx]        ; (ecx) = Synchronize Irql
        fstCall KfRaiseIrql
        mov     KsePreviousIrql, al

; Call specified routine passing the specified context parameter.
        mov     eax,KseSynchronizeContext
        push    eax
        call    KseSynchronizeRoutine
        mov     ebx, eax                ; save function returned value

; Unlock spin lock, lower IRQL to its previous level, and return the value
; returned by the specified routine.

        mov     ecx, KsePreviousIrql
        fstCall KfLowerIrql

        mov     eax, ebx                ; (eax) = returned value
        pop     ebx                     ; restore ebx
        leave                           ; will clear stack
        stdRET    _KeSynchronizeExecution

stdENDP _KeSynchronizeExecution

        page ,132
        subttl  "Interrupt Dispatch"
;++
;
; Routine Description:
;
;    This routine is entered as the result of an interrupt being generated
;    via a vector that is connected to an interrupt object. Its function is
;    to directly call the specified interrupt service routine.
;
;    This version is used for edge sensitive interrupts.
;
; Arguments:
;
;    edi - Supplies a pointer to the interrupt object.
;    esp - Supplies a pointer to the top of trap frame
;    ebp - Supplies a pointer to the top of trap frame
;
; Return Value:
;
;    None.
;
;--

align 16
cPublicProc _KiInterruptDispatch    ,0
.FPO (2, 0, 0, 0, 0, 1)

;
; update statistic
;
        inc     dword ptr PCR[PcPrcbData+PbInterruptCount]

;
; Save previous IRQL and set new priority level
;
; Instead of invoking HalBeginSystemInterrupt, we call indirect through the
; dismiss table ourselves.
;

        mov     ecx, [edi].InBusInterruptLevel
        mov     edx, [edi].InIrql
        push    0                       ; make room for OldIrql
        call    HalpSpecialDismissTable[ecx*4]

;
; Call the service routine.
;

if DBG
        mov     ebx, [edi].InBusInterruptLevel  ; this vector
        mov     eax, _KeTickCount               ; current time
        and     eax, NOT 31                     ; mask to closest 1/2 second
        shl     ebx, 3                          ; eax = eax * 8
        cmp     eax, [KiInterruptCounts+ebx]        ; in same 1/2 range?
        jne     kid_overflowreset

        mov     eax, _KiISROverflow
        inc     [KiInterruptCounts+ebx+4]
        cmp     [KiInterruptCounts+ebx+4], eax
        jnc     kid_interruptoverflow
kid_dbg2:
        mov     ebx, _KeTickCount
endif
        mov     eax, InServiceContext[edi] ; set parameter value
        push    eax
        push    edi                     ; pointer to interrupt object
        call    InServiceRoutine[edi]   ; call specified routine

if DBG
        add     ebx, _KiISRTimeout      ; adjust for ISR timeout
        cmp     _KeTickCount, ebx       ; Did ISR timeout?
        jnc     kid200
kid31:
endif

;
; Do interrupt exit processing
;

        INTERRUPT_EXIT                  ; will do an iret

;
; ISR took a long time to complete, abort to debugger
;

if DBG
kid200: push    InServiceRoutine[edi]   ; timed out
        push    offset FLAT:_MsgISRTimeout
        call    _DbgPrint
        add     esp,8
        int     3
        jmp     kid31                   ; continue

kid_interruptoverflow:
        push    [KiInterruptCounts+ebx+4]
        push    InServiceRoutine[edi]
        push    offset FLAT:_MsgISROverflow
        call    _DbgPrint
        add     esp,12
        int 3

        mov     eax, _KeTickCount               ; current time
        and     eax, NOT 31                     ; mask to closest 1/2 second

kid_overflowreset:
        mov     [KiInterruptCounts+ebx], eax        ; initialize time
        mov     [KiInterruptCounts+ebx+4], 0        ; reset count
        jmp     kid_dbg2
endif


stdENDP _KiInterruptDispatch

        page ,132
        subttl  "Level Interrupt Dispatch"
;++
;
; Routine Description:
;
;    This routine is entered as the result of an interrupt being generated
;    via a vector that is connected to an interrupt object. Its function is
;    to directly call the specified interrupt service routine.
;
;    This version is used for level sensitive interrupts.
;
; Arguments:
;
;    edi - Supplies a pointer to the interrupt object.
;    esp - Supplies a pointer to the top of trap frame
;    ebp - Supplies a pointer to the top of trap frame
;
; Return Value:
;
;    None.
;
;--

align 16
cPublicProc _KiLevelInterruptDispatch    ,0
.FPO (2, 0, 0, 0, 0, 1)

;
; update statistic
;
        inc     dword ptr PCR[PcPrcbData+PbInterruptCount]

;
; Save previous IRQL and set new priority level
;
; Instead of invoking HalBeginSystemInterrupt, we call indirect through the
; dismiss table ourselves.
;

        mov     ecx, [edi].InBusInterruptLevel
        mov     edx, [edi].InIrql
        push    0                       ; make room for OldIrql
        call    HalpSpecialDismissTable[ecx*4]

;
; Call the service routine.
;

if DBG
        mov     ebx, [edi].InBusInterruptLevel  ; this vector
        mov     eax, _KeTickCount               ; current time
        and     eax, NOT 31                     ; mask to closest 1/2 second
        shl     ebx, 3                          ; eax = eax * 8
        cmp     eax, [KiInterruptCounts+ebx]        ; in same 1/2 range?
        jne     klid_overflowreset

        mov     eax, _KiISROverflow
        inc     [KiInterruptCounts+ebx+4]
        cmp     [KiInterruptCounts+ebx+4], eax
        jnc     klid_interruptoverflow
klid_dbg2:
        mov     ebx, _KeTickCount
endif
        mov     eax, InServiceContext[edi] ; set parameter value
        push    eax
        push    edi                     ; pointer to interrupt object
        call    InServiceRoutine[edi]   ; call specified routine

if DBG
        add     ebx, _KiISRTimeout      ; adjust for ISR timeout
        cmp     _KeTickCount, ebx       ; Did ISR timeout?
        jnc     klid200
klid31:
endif

;
; Do interrupt exit processing
;

        mov     eax, [edi].InBusInterruptLevel
        LEVEL_INTERRUPT_EXIT

;
; ISR took a long time to complete, abort to debugger
;

if DBG
klid200: push    InServiceRoutine[edi]   ; timed out
        push    offset FLAT:_MsgISRTimeout
        call    _DbgPrint
        add     esp,8
        int     3
        jmp     klid31                  ; continue

klid_interruptoverflow:
        push    [KiInterruptCounts+ebx+4]
        push    InServiceRoutine[edi]
        push    offset FLAT:_MsgISROverflow
        call    _DbgPrint
        add     esp,12
        int 3

        mov     eax, _KeTickCount               ; current time
        and     eax, NOT 31                     ; mask to closest 1/2 second

klid_overflowreset:
        mov     [KiInterruptCounts+ebx], eax        ; initialize time
        mov     [KiInterruptCounts+ebx+4], 0        ; reset count
        jmp     klid_dbg2
endif

stdENDP _KiLevelInterruptDispatch

        page ,132
        subttl  "Interrupt Template"
;++
;
; Routine Description:
;
;    This routine is a template that is copied into each interrupt object. Its
;    function is to save machine state and pass the address of the respective
;    interrupt object and transfer control to the appropriate interrupt
;    dispatcher.
;
;    Control comes here through i386 interrupt gate and, upon entry, the
;    interrupt is disabled.
;
;    Note: If the length of this template changed, the corresponding constant
;          defined in Ki.h needs to be updated accordingly.
;
; Arguments:
;
;    None
;
; Return Value:
;
;    edi - addr of interrupt object
;    esp - top of trap frame
;    interrupts are disabled
;
;--

_KiShutUpAssembler      proc

        public  _KiInterruptTemplate
_KiInterruptTemplate    label   byte

; Save machine state on trap frame

        ENTER_INTERRUPT

;
; the following instruction gets the addr of associated interrupt object.
; the value ? will be replaced by REAL interrupt object address at
; interrupt object initialization time.
;       mov     edi, addr of interrupt object
;
; Template modifications made to support BBT, include replacing bogus
; insructions (created by db and dd) with real instructions.
; This stuff gets overwritten anyway.  BBT just needs to see real instructions.

        public  _KiInterruptTemplate2ndDispatch
_KiInterruptTemplate2ndDispatch equ     this dword
        mov      edi,0

        public  _KiInterruptTemplateObject
_KiInterruptTemplateObject      equ     this dword

; the following instruction transfers control to the appropriate dispatcher
; code.  The value ? will be replaced by real InterruptObj.DispatchAddr
; at interrupt initialization time.  The dispatcher routine will be any one
; of _KiInterruptDispatch or _KiChainDispatch.
;       jmp     [IntObj.DispatchAddr]

        db      0E9h, 0, 0, 0, 0

        public  _KiInterruptTemplateDispatch
_KiInterruptTemplateDispatch    equ     this dword

; end of _KiInterruptTemplate

; The template must fit in the KINTERRUPT.DispatchCode[DISPATCH_LENGTH] field.
.errnz (($ - _KiInterruptTemplate) GT DISPATCH_LENGTH)

_KiShutUpAssembler      endp

        page ,132
        subttl  "Unexpected Interrupt"
;++
;
; Routine Description:
;
;    This routine is entered as the result of an interrupt being generated
;    via a vector that is not connected to an interrupt object.
;
;    For any unconnected vector, its associated 8259 irq is masked out at
;    Initialization time.  So, this routine should NEVER be called.
;    If somehow, this routine gets control we simple raise a BugCheck and
;    stop the system.
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
        public _KiUnexpectedInterrupt
_KiUnexpectedInterrupt  proc
cPublicFpo 0,0

; stop the system
        stdCall   _KeBugCheck, <TRAP_CAUSE_UNKNOWN>
        nop

_KiUnexpectedInterrupt endp

_TEXT   ends
        end
