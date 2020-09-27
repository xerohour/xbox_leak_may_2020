        title   "Critical Section Support"
;++
;
;  Copyright (c) 1991  Microsoft Corporation
;
;  Module Name:
;
;     critsect.asm
;
;  Abstract:
;
;     This module implements functions to support user mode critical sections.
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
KERNELONLY  equ     1

include ks386.inc
include callconv.inc                    ; calling convention macros
        .list

_TEXT   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        extrn   _KiPCR:DWORD
        EXTRNP  _KeWaitForSingleObject,5
        EXTRNP  _KeSetEvent,3
        EXTRNP  HalRequestSoftwareInterrupt,1,,FASTCALL

CriticalSection equ     [esp + 4]

        page , 132
        subttl  "RtlEnterCriticalSectionAndRegion"

;++
;
; VOID
; RtlEnterCriticalSectionAndRegion(
;    IN PRTL_CRITICAL_SECTION CriticalSection
;    )
;
; Routine Description:
;
;    This function enters a critical section and disables kernel APC's.
;
; Arguments:
;
;    CriticalSection - supplies a pointer to a critical section.
;
; Return Value:
;
;    None.
;
;--

        align   16
cPublicProc _RtlEnterCriticalSectionAndRegion,1
cPublicFpo 1,0

        mov     ecx, PCR[PcPrcbData+PbCurrentThread]
        mov     edx, CriticalSection    ; get address of critical section

;
; Disable kernel APC's.
;

        dec     dword ptr ThKernelApcDisable[ecx]

;
; Attempt to acquire critical section.
;

        inc     dword ptr CsLockCount[edx] ; increment lock count
        jnz     short Ent20             ; if nz, already owned

;
; Set critical section owner and initialize recursion count.
;

        mov     CsOwningThread[edx], ecx ; set critical section owner
        mov     dword ptr CsRecursionCount[edx], 1 ; set recursion count

        stdRET  _RtlEnterCriticalSectionAndRegion

;
; For out of straight-line paths, we fall into RtlEnterCriticalSection
;

stdENDP _RtlEnterCriticalSectionAndRegion


        page , 132
        subttl  "RtlEnterCriticalSection"

;++
;
; VOID
; RtlEnterCriticalSection(
;    IN PRTL_CRITICAL_SECTION CriticalSection
;    )
;
; Routine Description:
;
;    This function enters a critical section.
;
; Arguments:
;
;    CriticalSection - supplies a pointer to a critical section.
;
; Return Value:
;
;    None.
;
;--

        align   16
cPublicProc _RtlEnterCriticalSection,1
cPublicFpo 1,0

        mov     ecx, PCR[PcPrcbData+PbCurrentThread]
        mov     edx, CriticalSection    ; get address of critical section

;
; Attempt to acquire critical section.
;

        inc     dword ptr CsLockCount[edx] ; increment lock count
        jnz     short Ent20             ; if nz, already owned

;
; Set critical section owner and initialize recursion count.
;

Ent10:
        mov     CsOwningThread[edx], ecx ; set critical section owner
        mov     dword ptr CsRecursionCount[edx], 1 ; set recursion count

        stdRET  _RtlEnterCriticalSection

;
; The critical section is already owned, but may be owned by the current thread.
;

        align   16
Ent20:  cmp     CsOwningThread[edx],ecx ; check if current thread is owner
        jne     short Ent30             ; if ne, current thread not owner
        inc     dword ptr CsRecursionCount[edx] ; increment recursion count

        stdRET  _RtlEnterCriticalSection

;
; The critcal section is owned by another thread and the current thread must
; wait for ownership.
;

Ent30:  stdCall _KeWaitForSingleObject,<edx,WrExecutive,0,0,0>
        mov     ecx, PCR[PcPrcbData+PbCurrentThread]
        mov     edx,CriticalSection     ; get address of critical section
        jmp     Ent10                   ; set owner and recursion count

stdENDP _RtlEnterCriticalSection


        page , 132
        subttl  "RtlLeaveCriticalSectionAndRegion"
;++
;
; VOID
; RtlLeaveCriticalSectionAndAndRegion(
;    IN PRTL_CRITICAL_SECTION CriticalSection
;    )
;
; Routine Description:
;
;    This function leaves a critical section and enables kernel APC's.
;
; Arguments:
;
;    CriticalSection - supplies a pointer to a critical section.
;
; Return Value:
;
;    None.
;
;--

        align   16
cPublicProc _RtlLeaveCriticalSectionAndRegion,1
cPublicFpo 1,0

        mov     edx,CriticalSection
        xor     eax,eax                     ; Assume STATUS_SUCCESS
        dec     dword ptr CsRecursionCount[edx]
        jnz     Lveea20                     ; skip if only leaving recursion

        mov     CsOwningThread[edx],eax     ; clear owning thread id

        dec     dword ptr CsLockCount[edx]  ; interlocked dec of
                                            ; ... CriticalSection->LockCount
        jl      Lveea10
        stdCall _KeSetEvent,<edx,EVENT_INCREMENT,0>

;
; Enable kernel APC's and check if we need to raise an APC interrupt.
;

Lveea10:
        mov     ecx, PCR[PcPrcbData+PbCurrentThread]
        mov     eax, dword ptr [ecx+ThKernelApcDisable]
                                            ; Grab the current APC disable count
        inc     eax
        test    eax, eax                    ; Kernel APC disable count == 0?
        mov     dword ptr [ecx+ThKernelApcDisable], eax
                                            ; Write the count back to the thread
        jnz     Lveea15
        lea     eax, [ecx+ThApcState+AsApcListHead]
        cmp     dword ptr [ecx+ThApcState+AsApcListHead], eax
                                            ; Check if APC list is empty
        je      Lveea15

        mov     byte ptr [ecx+ThApcState+AsKernelApcPending], 1
        mov     cl, APC_LEVEL               ; Service the pending kernel APC
        fstCall HalRequestSoftwareInterrupt

Lveea15:
        stdRET  _RtlLeaveCriticalSectionAndRegion

        align   16
Lveea20:
        dec     dword ptr CsLockCount[edx]  ; interlocked dec of
                                            ; ... CriticalSection->LockCount
        stdRET  _RtlLeaveCriticalSectionAndRegion

_RtlLeaveCriticalSectionAndRegion    endp


        page , 132
        subttl  "RtlLeaveCriticalSection"
;++
;
; VOID
; RtlLeaveCriticalSection(
;    IN PRTL_CRITICAL_SECTION CriticalSection
;    )
;
; Routine Description:
;
;    This function leaves a critical section.
;
; Arguments:
;
;    CriticalSection - supplies a pointer to a critical section.
;
; Return Value:
;
;    None.
;
;--

        align   16
cPublicProc _RtlLeaveCriticalSection,1
cPublicFpo 1,0

        mov     edx,CriticalSection
        xor     eax,eax                     ; Assume STATUS_SUCCESS
        dec     dword ptr CsRecursionCount[edx]
        jnz     leave_recurs                ; skip if only leaving recursion

        mov     CsOwningThread[edx],eax     ; clear owning thread id

        dec     dword ptr CsLockCount[edx]  ; interlocked dec of
                                            ; ... CriticalSection->LockCount
        jge     @F
        stdRET  _RtlLeaveCriticalSection
@@:
        stdCall _KeSetEvent,<edx,EVENT_INCREMENT,0>
        stdRET  _RtlLeaveCriticalSection

        align   16
leave_recurs:
        dec     dword ptr CsLockCount[edx]  ; interlocked dec of
                                            ; ... CriticalSection->LockCount
        stdRET  _RtlLeaveCriticalSection

_RtlLeaveCriticalSection    endp


        page    ,132
        subttl  "RtlTryEnterCriticalSection"
;++
;
; ULONG
; RtlTryEnterCriticalSection(
;    IN PRTL_CRITICAL_SECTION CriticalSection
;    )
;
; Routine Description:
;
;    This function attempts to enter a critical section without blocking.
;
; Arguments:
;
;    CriticalSection (a0) - Supplies a pointer to a critical section.
;
; Return Value:
;
;    If the critical section was successfully entered, then a value of TRUE
;    is returned as the function value. Otherwise, a value of FALSE is returned.
;
;--

CriticalSection equ     [esp + 4]

cPublicProc _RtlTryEnterCriticalSection,1
cPublicFpo 1,0

        mov     ecx, CriticalSection        ; interlocked inc of
        mov     eax, -1                     ; set value to compare against
        mov     edx, 0                      ; set value to set
        cmpxchg dword ptr CsLockCount[ecx], edx  ; Attempt to acquire critsect
        jnz     short tec10                 ; if nz, critsect already owned

        mov     eax, PCR[PcPrcbData+PbCurrentThread]
        mov     CsOwningThread[ecx], eax
        mov     dword ptr CsRecursionCount[ecx], 1

        mov     eax, 1                      ; set successful status

        stdRET  _RtlTryEnterCriticalSection

tec10:
;
; The critical section is already owned. If it is owned by another thread,
; return FALSE immediately. If it is owned by this thread, we must increment
; the lock count here.
;
        mov     eax, PCR[PcPrcbData+PbCurrentThread]
        cmp     CsOwningThread[ecx], eax
        jz      tec20                       ; if eq, this thread is already the owner
        xor     eax, eax                    ; set failure status
        stdRET  _RtlTryEnterCriticalSection

tec20:
;
; This thread is already the owner of the critical section. Perform an atomic
; increment of the LockCount and a normal increment of the RecursionCount and
; return success.
;
        inc     dword ptr CsLockCount[ecx]
        inc     dword ptr CsRecursionCount[ecx]
        mov     eax, 1
        stdRET  _RtlTryEnterCriticalSection

stdENDP _RtlTryEnterCriticalSection

_TEXT   ends
        end
