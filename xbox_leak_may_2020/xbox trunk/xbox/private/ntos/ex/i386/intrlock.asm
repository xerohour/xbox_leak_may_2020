        title  "Interlocked Support"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    intrlock.asm
;
; Abstract:
;
;    This module implements functions to support interlocked operations.
;    Interlocked operations can only operate on nonpaged data.
;
; Author:
;
;    Shie-Lin Tzong (shielint) 12-Feb-1990
;
; Environment:
;
;    Any mode.
;
; Revision History:
;
;   bryanwi 1-aug-90    Clean up and fix stuff.
;   bryanwi 3-aug-90    Add ExInterlockedIncrementLlong,...
;
;--
.386p
        .xlist
include ks386.inc
include callconv.inc                    ; calling convention macros
        .list


_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
;
;   General Notes on Interlocked Procedures:
;
;       These procedures assume that neither their code, nor any of
;       the data they touch, will cause a page fault.
;
;       They use spinlocks to achieve MP atomicity, iff it's an MP machine.
;       (The spinlock macros generate zilch if NT_UP = 1, and
;        we if out some aux code here as well.)
;
;       They turn off interrupts so that they can be used for synchronization
;       between ISRs and driver code.  Flags are preserved so they can
;       be called in special code (Like IPC interrupt handlers) that
;       may have interrupts off.
;
;--


;;      align  512

        page ,132
        subttl  "ExInterlockedAddLargeInteger"
;++
;
; LARGE_INTEGER
; ExInterlockedAddLargeInteger (
;    IN PLARGE_INTEGER Addend,
;    IN LARGE_INTEGER Increment,
;    IN PKSPIN_LOCK Lock
;    )
;
; Routine Description:
;
;    This function performs an interlocked add of an increment value to an
;    addend variable of type unsigned large integer. The initial value of
;    the addend variable is returned as the function value.
;
; Arguments:
;
;    (TOS+4) = Addend - a pointer to the addend value
;    (TOS+8) = Increment - the increment value
;    (TOS+16) = Lock - a pointer to a pointer to a spin lock
;
; Return Value:
;
;    The initial value of the addend variable is stored in eax:edx
;
;--

EiulAddend      equ     [ebp + 8]
EiulIncrement   equ     [ebp + 12]
EiulLock        equ     [ebp + 20]
EiulRetval      equ     [ebp - 8]

cPublicProc _ExInterlockedAddLargeInteger, 4

        push    ebp
        mov     ebp,esp
        sub     esp, 8

eiul10: pushfd
        cli                             ; disable interrupts

        mov     eax,EiulAddend          ; (eax)-> addend variable
        mov     ecx,[eax]               ; (ecx)= low part of addend value
        mov     edx,[eax]+4             ; (edx)= high part of addend value
        mov     EiulRetVal,ecx               ; set low part of return value
        mov     EiulRetVal+4,edx             ; set high part of return value
        add     ecx,EiulIncrement       ; add low parts of large integer
        adc     edx,EiulIncrement+4     ; add high parts of large integer and carry
        mov     eax,EiulAddend          ; RELOAD (eax)-> addend variable
        mov     [eax],ecx               ; store low part of result
        mov     [eax]+4,edx             ; store high part of result

        popfd                           ; restore flags including interrupts
        mov     eax, EiulRetval         ; calling convention
        mov     edx, EiulRetval+4       ; calling convention
        mov     esp, ebp
        pop     ebp
        stdRET    _ExInterlockedAddLargeInteger

stdENDP _ExInterlockedAddLargeInteger

        page ,132
        subttl  "ExInterlocked Exchange Add Large Integer"
;++
;
; LARGE_INTEGER
; ExInterlockedExchangeAddLargeInteger (
;    IN PLARGE_INTEGER Addend,
;    IN LARGE_INTEGER Increment,
;    IN PKSPIN_LOCK Lock
;    )
;
; Routine Description:
;
;    This function performs an interlocked add of an increment value to an
;    addend variable of type unsigned large integer. The initial value of
;    the addend variable is returned as the function value.
;
;    N.B. The cmpxchg8b instruction is only supported on some processors.
;         If the host processor does not support this instruction, then
;         then following code is patched to contain a jump to the normal
;         add large integer code which has a compatible calling sequence
;         and data structure.
;
; Arguments:
;
;    (TOS + 4) = Addend - Supplies a pointer to the addend variable.
;
;    (TOS + 8) = Increment - Supplies the increment value.
;
;    (TOS + 16) = Lock - Supplies a pointer a spin lock.
;
;    N.B. This routine does not use the spin lock.
;
; Return Value:
;
;    The initial value of the addend variable is stored in eax:edx.
;
;--

XaAddend      equ     [esp + 12]
XaIncrement   equ     [esp + 16]
XaLock        equ     [esp + 24]

cPublicProc _ExInterlockedExchangeAddLargeInteger, 4

cPublicFpo 0,2

;
; Save nonvolatile registers and get the addend value.
;

        push    ebx                     ; save nonvolatile registers
        push    ebp                     ;
        mov     ebp, XaAddend           ; get the addend variable address
        mov     eax,[ebp] + 0           ; get low part of addend value
        mov     edx,[ebp] + 4           ; get high part of addend value

;
; Add the increment value to the addend value.
;

Xa10:   mov     ebx, eax                ; copy low part of addend value
        mov     ecx, edx                ; copy high part of addend value
        add     ebx, XaIncrement        ; add low part of increment value
        adc     ecx, XaIncrement + 4    ; add high part of increment value

;
; Exchange the updated addend value with the previous addend value.
;

.586
        cmpxchg8b qword ptr [ebp]       ; compare and exchange
.386

        jnz     short Xa10              ; if z clear, exchange failed

;
; Restore nonvolatile registers and return result.
;

cPublicFpo 0,0

        pop     ebp                     ;
        pop     ebx                     ;

        stdRET    _ExInterlockedExchangeAddLargeInteger

stdENDP _ExInterlockedExchangeAddLargeInteger

_TEXT   ends
        end
