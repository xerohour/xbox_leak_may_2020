        TITLE   "Large Integer Arithmetic"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    largeint.s
;
; Abstract:
;
;    This module implements routines for performing extended integer
;    arithmtic.
;
; Author:
;
;    David N. Cutler (davec) 24-Aug-1989
;
; Environment:
;
;    Any mode.
;
; Revision History:
;
;--

.386p
        .xlist
include ks386.inc
include callconv.inc            ; calling convention macros
        .list

IFNDEF BLDR_KERNEL_RUNTIME
        EXTRNP  _RtlRaiseStatus, 1
ENDIF


_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page
        subttl  "Extended Large Integer Divide"

;++
;
; LARGE_INTEGER
; RtlExtendedLargeIntegerDivide (
;     IN LARGE_INTEGER Dividend,
;     IN ULONG Divisor,
;     OUT PULONG Remainder OPTIONAL
;     )
;
; Routine Description:
;
;     This routine divides an unsigned 64 bit dividend by a 32 bit divisor
;     and returns a 64-bit quotient, and optionally the 32-bit remainder.
;
;
; Arguments:
;
;     Dividend - Supplies the 64 bit dividend for the divide operation.
;
;     Divisor - Supplies the 32 bit divisor for the divide operation.
;
;     Remainder - Supplies an optional pointer to a variable which receives
;         the remainder
;
; Return Value:
;
;     The 64-bit quotient is returned as the function value.
;
;--

cPublicProc _RtlExtendedLargeIntegerDivide, 4
cPublicFpo 4,3

        push    esi
        push    edi
        push    ebx

        mov     eax, [esp+16]       ; (eax) = Dividend.LowPart
        mov     edx, [esp+20]       ; (edx) = Dividend.HighPart

lid00:  mov     ebx, [esp+24]       ; (ebx) = Divisor
        or      ebx, ebx
        jz      short lid_zero      ; Attempted a divide by zero

        push    ebp

        mov     ecx, 64             ; Loop count
        xor     esi, esi            ; Clear partial remainder

; (edx:eax) = Dividend
; (ebx) = Divisor
; (ecx) = Loop count
; (esi) = partial remainder

align 4
lid10:  shl     eax, 1              ; (LowPart << 1)  | 0
        rcl     edx, 1              ; (HighPart << 1) | CF
        rcl     esi, 1              ; (Partial << 1)  | CF

        sbb     edi, edi            ; clone CF into edi (0 or -1)

        cmp     esi, ebx            ; check if partial remainder less then divisor
        cmc
        sbb     ebp, ebp            ; clone CF intp ebp
        or      edi, ebp            ; merge with remainder of high bit

        sub     eax, edi            ; merge quotient bit
        and     edi, ebx            ; Select divisor or 0
        sub     esi, edi

        dec     ecx                 ; dec interration count
        jnz     short lid10         ; go around again

        pop     ebp
        pop     ebx
        pop     edi

        mov     ecx, [esp+20]       ; (ecx) = Remainder
        or      ecx, ecx
        jnz     short lid20

        pop     esi
        stdRET  _RtlExtendedLargeIntegerDivide

align 4
lid20:
        mov     [ecx], esi          ; store remainder
        pop     esi
        stdRET  _RtlExtendedLargeIntegerDivide

lid_zero:
IFNDEF BLDR_KERNEL_RUNTIME
        stdCall _RtlRaiseStatus, <STATUS_INTEGER_DIVIDE_BY_ZERO>
ENDIF
        pop     ebx
        pop     edi
        pop     esi
        stdRET  _RtlExtendedLargeIntegerDivide

stdENDP     _RtlExtendedLargeIntegerDivide

        page
        subttl  "Extended Magic Divide"
;++
;
; LARGE_INTEGER
; RtlExtendedMagicDivide (
;    IN LARGE_INTEGER Dividend,
;    IN LARGE_INTEGER MagicDivisor,
;    IN CCHAR ShiftCount
;    )
;
; Routine Description:
;
;    This function divides a signed large integer by an unsigned large integer
;    and returns the signed large integer result. The division is performed
;    using reciprocal multiplication of a signed large integer value by an
;    unsigned large integer fraction which represents the most significant
;    64-bits of the reciprocal divisor rounded up in its least significant bit
;    and normalized with respect to bit 63. A shift count is also provided
;    which is used to truncate the fractional bits from the result value.
;
; Arguments:
;
;   (ebp+8) = Dividend
;   (ebp+16) = MagicDivisor value is a 64-bit multiplicative reciprocal
;   (ebp+24) = ShiftCount - Right shift adjustment value.
;
; Return Value:
;
;    The large integer result is stored  in (edx:eax)
;
;--

RemdDiv     equ [ebp+8]             ; Dividend
RemdRec     equ [ebp+16]            ; Reciprocal (magic divisor)
RemdShift   equ [ebp+24]
RemdTmp1    equ [ebp-4]
RemdTmp2    equ [ebp-8]
RemdTmp3    equ [ebp-12]

cPublicProc _RtlExtendedMagicDivide ,5

        push    ebp
        mov     ebp,esp
        sub     esp,12
        push    esi

        mov     esi, RemdDiv+4
        test    esi,80000000h
        jz      remd10                  ; no sign, no need to negate

        neg     dword ptr RemdDiv+4
        neg     dword ptr RemdDiv
        sbb     dword ptr RemdDiv+4,0   ; negate

remd10: mov     eax,RemdRec
        mul     dword ptr RemdDiv       ; (edx:eax) = Div.lo * Rec.lo
        mov     RemdTmp1,edx

        mov     eax,RemdRec
        mul     dword ptr RemdDiv+4     ; (edx:eax) = Div.hi * Rec.lo
        mov     RemdTmp2,eax
        mov     RemdTmp3,edx

        mov     eax,RemdRec+4
        mul     dword ptr RemdDiv       ; (edx:eax) = Div.lo * Rec.hi

;
;   Col 0 doesn't matter
;   Col 1 = Hi(Div.lo * Rec.lo) + Low(Div.Hi * Rec.lo) + Low(Div.lo * Rec.hi)
;         = RemdTmp1 + RemdTmp2 + eax
;         -> Only want carry from Col 1
;

        xor     ecx,ecx                 ; (ecx) = 0
        add     eax,RemdTmp1
        adc     ecx, 0                  ; save carry in ecx
        add     eax,RemdTmp2
        adc     ecx, 0                  ; Save Carry, all we want from Col2

        mov     RemdTmp1,edx

        mov     eax,RemdRec+4
        mul     dword ptr RemdDiv+4     ; (edx:eax) = Div.Hi * Rec.Hi

;
;   TOS = carry flag from Col 1
;
;   Col 2 = Col1 CF +
;           Hi(Div.Hi * Rec.Lo) + Hi(Div.Lo * Rec.Hi) + Low(Div.Hi * Rec.Hi)
;         = CF + RemdTmp3 + RemdTmp1 + eax
;
;   Col 3 = Col2 CF + Hi(Div.Hi * Rec.Hi)
;         = CF + edx
;

        add     eax,RemdTmp1
        adc     edx, 0                  ; add carry to edx
        add     eax,RemdTmp3            ; (eax) = col 2
        adc     edx, 0                  ; add carry to edx
        add     eax, ecx
        adc     edx, 0                  ; (edx) = col 3

;
;   (edx:eax) = the high 64 bits of the multiply, shift it right by
;               shift count to discard bits to right of virtual decimal pt.
;
;   RemdShift could be as large as 63 and still not 0 the result, 386
;   will only shift 31 bits at a time, so must do the sift multiple
;   times to get correct effect.
;

        mov     cl,RemdShift
remd20: cmp     cl,31
        jbe     remd30
        sub     cl,31
        shrd    eax,edx,31
        shr     edx,31
        jmp     remd20

remd30: shrd    eax,edx,cl
        shr     edx,cl

;
;   Negate the result if need be
;

        test    esi,80000000h
        jz      remd40                  ; no sign, go return without negate

        neg     edx
        neg     eax
        sbb     edx,0

;
;   Store the result
;

remd40:
        ; results in (edx:eax)

        pop     esi
        mov     esp,ebp
        pop     ebp
        stdRET    _RtlExtendedMagicDivide

stdENDP _RtlExtendedMagicDivide


        page
        subttl  "Extended Integer Multiply"
;++
;
; LARGE_INTEGER
; RtlExtendedIntegerMultiply (
;    IN LARGE_INTEGER Multiplicand,
;    IN ULONG Multiplier
;    )
;
; Routine Description:
;
;    This function multiplies a signed large integer by a signed integer and
;    returns the signed large integer result.
;
; Arguments:
;
;   (ebp+8,12)=multiplican (MCAN)
;   (ebp+16)=multiplier (MPER)
;
; Return Value:
;
;    The large integer result is stored in (edx:eax)
;
;--

ReimMCAN    equ <dword ptr [ebp+8]>
ReimMPER    equ <dword ptr [ebp+16]>

cPublicProc _RtlExtendedIntegerMultiply ,3

        push    ebp
        mov     ebp,esp
        push    esi

        mov     esi,ReimMPER
        xor     esi,ReimMCAN+4              ; (esi) = result sign

        test    ReimMCAN+4,80000000h
        jz      short reim10                ; MCAN pos, go look at MPER

        neg     dword ptr ReimMCAN+4
        neg     dword ptr ReimMCAN
        sbb     dword ptr ReimMCAN+4,0      ; negate multiplican

reim10: test    ReimMPER,80000000h
        jz      short reim20                ; MPER pos, go do multiply

        neg     dword ptr ReimMPER          ; negate multiplier

reim20: mov     eax,ReimMPER
        mul     dword ptr ReimMCAN          ; (edx:eax) = MPER * MCAN.low
        push    edx
        mov     ecx, eax
        mov     eax,ReimMPER
        mul     dword ptr ReimMCAN+4        ; (edx:eax) = MPER * MCAN.high
        add     eax,[esp]                   ; (eax) = hi part of MPER*MCAN.low
                                            ;   plus low part of MPER*MCAN.hi

        test    esi,80000000h
        jz      short reim30                ; result sign is OK, go return

        neg     eax
        neg     ecx
        sbb     eax,0                       ; negate result

reim30: add     esp,4                       ; clean eax off stack
        pop     esi                         ; restore nonvolatile reg
        mov     edx,eax                     ; (edx:ecx) = result
        mov     eax,ecx                     ; (edx:eax) = result

        pop     ebp
        stdRET    _RtlExtendedIntegerMultiply

stdENDP _RtlExtendedIntegerMultiply

_TEXT   ends
        end
