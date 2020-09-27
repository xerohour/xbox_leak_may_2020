        title   "Interlocked API Support"
;++
;
;  Copyright (c) 1991  Microsoft Corporation
;
;  Module Name:
;
;     intrlock.asm
;
;  Abstract:
;
;     This module implements functions to support user mode interlocked operations.
;
;  Author:
;
;     Bryan M. Willman (bryanwi) 2-Oct-91
;
;  Environment:
;
;     Any mode.
;
;  Revision History:
;
;--

.486p
        .xlist
include ks386.inc
include callconv.inc
        .list

_TEXT   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

Addend      equ     [esp + 4]

        page , 132
        subttl  "InterlockedIncrement"
;++
;
; LONG
; InterlockedIncrement(
;    IN PLONG Addend
;    )
;
; Routine Description:
;
;    This function performs an interlocked add of one to the addend variable.
;
;    No checking is done for overflow.
;
; Arguments:
;
;    Addend - Supplies a pointer to a variable whose value is to be
;       incremented by one.
;
; Return Value:
;
;   (eax) - the incremented value.
;
;--

cPublicProc __InterlockedIncrement,1
cPublicFpo 1,0
        mov     ecx,Addend              ; get pointer to addend variable
        mov     eax,1                   ; set increment value

        xadd    [ecx],eax               ; interlocked increment
        inc     eax                     ; adjust return value
        stdRET __InterlockedIncrement    ;

stdENDP __InterlockedIncrement

        page , 132
        subttl  "InterlockedDecrment"
;++
;
; LONG
; InterlockedDecrement(
;    IN PLONG Addend
;    )
;
; Routine Description:
;
;    This function performs an interlocked add of -1 to the addend variable.
;
;    No checking is done for overflow
;
; Arguments:
;
;    Addend - Supplies a pointer to a variable whose value is to be
;       decremented by one.
;
; Return Value:
;
;   (eax) - The decremented value.
;
;--

cPublicProc __InterlockedDecrement,1
cPublicFpo 1,0

        mov     ecx,Addend              ; get pointer to addend variable
        mov     eax,-1                  ; set decrement value

        xadd    [ecx],eax               ; interlocked decrement
        dec     eax                     ; adjust return value
        stdRET __InterlockedDecrement    ;

stdENDP __InterlockedDecrement

        page , 132
        subttl  "Interlocked Exchange"
;++
;
; LONG
; InterlockedExchange(
;    IN OUT LPLONG Target,
;    IN LONG Value
;    )
;
; Routine Description:
;
;    This function atomically exchanges the Target and Value, returning
;    the prior contents of Target
;
; Arguments:
;
;    Target - Address of LONG to exchange
;    Value  - New value of LONG
;
; Return Value:
;
;    (eax) - The prior value of target.
;--

cPublicProc __InterlockedExchange, 2
cPublicFpo 2,0

        mov     ecx, [esp+4]                ; (ecx) = Target
        mov     edx, [esp+8]                ; (edx) = Value
        mov     eax, [ecx]                  ; get comperand value
Ixchg:
        cmpxchg [ecx], edx                  ; compare and swap
        jnz     Ixchg                       ; if nz, exchange failed
        stdRET  __InterlockedExchange

stdENDP __InterlockedExchange

        page , 132
        subttl  "Interlocked Compare Exchange"
;++
;
;   PVOID
;   InterlockedCompareExchange (
;       IN OUT PVOID *Destination,
;       IN PVOID Exchange,
;       IN PVOID Comperand
;       )
;
;   Routine Description:
;
;    This function performs an interlocked compare of the destination
;    value with the comperand value. If the destination value is equal
;    to the comperand value, then the exchange value is stored in the
;    destination. Otherwise, no operation is performed.
;
; Arguments:
;
;    Destination - Supplies a pointer to destination value.
;
;    Exchange - Supplies the exchange value.
;
;    Comperand - Supplies the comperand value.
;
; Return Value:
;
;    (eax) - The initial destination value.
;
;--

cPublicProc __InterlockedCompareExchange, 3
cPublicFpo 3,0

        mov     ecx, [esp + 4]          ; get destination address
        mov     edx, [esp + 8]          ; get exchange value
        mov     eax, [esp + 12]         ; get comperand value

        cmpxchg [ecx], edx              ; compare and exchange
        stdRET  __InterlockedCompareExchange

stdENDP __InterlockedCompareExchange

        page , 132
        subttl  "Interlocked Exchange Add"
;++
;
;   LONG
;   InterlockedExchangeAdd (
;       IN OUT PLONG Addend,
;       IN LONG Increment
;       )
;
;   Routine Description:
;
;    This function performs an interlocked add of an increment value to an
;    addend variable of type unsinged long. The initial value of the addend
;    variable is returned as the function value.
;
;       It is NOT possible to mix ExInterlockedDecrementLong and
;       ExInterlockedIncrementong with ExInterlockedAddUlong.
;
;
; Arguments:
;
;    Addend - Supplies a pointer to a variable whose value is to be
;       adjusted by the increment value.
;
;    Increment - Supplies the increment value to be added to the
;       addend variable.
;
; Return Value:
;
;    (eax) - The initial value of the addend.
;
;--

cPublicProc __InterlockedExchangeAdd, 2
cPublicFpo 2,0

        mov     ecx, [esp + 4]          ; get addend address
        mov     eax, [esp + 8]          ; get increment value

        xadd    [ecx], eax              ; exchange add
        stdRET  __InterlockedExchangeAdd

stdENDP __InterlockedExchangeAdd

_TEXT   ends
        end
