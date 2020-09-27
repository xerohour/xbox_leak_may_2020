        title  "Interlocked Support"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    intrlfst.asm
;
; Abstract:
;
;    This module implements functions to support interlocked operations.
;    Interlocked operations can only operate on nonpaged data.
;
;    This module implements the fast call version of the interlocked
;    fuctions.
;
; Author:
;
;    Ken Reneris (kenr) 5-May-1994
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

        page ,132
        subttl  "ExInterlockedAddLargeStatistic"
;++
;
; VOID
; FASTCALL
; ExInterlockedAddLargeStatistic (
;    IN PLARGE_INTEGER Addend,
;    IN ULONG Increment
;    )
;
; Routine Description:
;
;    This function performs an interlocked add of an increment value to an
;    addend variable of type unsigned large integer.
;
; Arguments:
;
;    (ecx) Addend - Supplies a pointer to the variable whose value is
;                     adjusted by the increment value.
;
;    (edx) Increment - Supplies the increment value that is added to the
;                      addend variable.
;
; Return Value:
;
;    None.
;
;--

cPublicFastCall ExInterlockedAddLargeStatistic, 2
cPublicFpo 0,0

        add dword ptr [ecx], edx        ; add low part of large statistic
        adc dword ptr [ecx+4], 0        ; add carry to high part

        fstRET  ExInterlockedAddLargeStatistic ; return

fstENDP ExInterlockedAddLargeStatistic

        page , 132
        subttl  "Interlocked Insert Head List"
;++
;
; PLIST_ENTRY
; ExfInterlockedInsertHeadList (
;    IN PLIST_ENTRY ListHead,
;    IN PLIST_ENTRY ListEntry
;    )
;
; Routine Description:
;
;    This function inserts an entry at the head of a doubly linked list
;    so that access to the list is synchronized in a multiprocessor system.
;
;    N.B. The pages of data which this routine operates on MUST be
;         present.  No page fault is allowed in this routine.
;
; Arguments:
;
;   (ecx) = ListHead - Supplies a pointer to the head of the doubly linked
;                       list into which an entry is to be inserted.
;
;   (edx) = ListEntry - Supplies a pointer to the entry to be inserted at the
;                       head of the list.
;
; Return Value:
;
;    Pointer to entry that was at the head of the list or NULL if the list
;    was empty.
;
;--

cPublicFastCall ExfInterlockedInsertHeadList    , 2
cPublicFpo 0, 1

        pushfd
        cli

        mov     eax, LsFlink[ecx]       ; (eax)->next entry in the list
        mov     [edx]+LsFlink, eax      ; store next link in entry
        mov     [edx]+LsBlink, ecx      ; store previous link in entry
        mov     [ecx]+LsFlink, edx      ; store next link in head
        mov     [eax]+LsBlink, edx      ; store previous link in next

        popfd

        xor     eax, ecx                ; return null if list was empty
        jz      short Eiih15
        xor     eax, ecx
Eiih15: fstRET  ExfInterlockedInsertHeadList

fstENDP ExfInterlockedInsertHeadList

        page , 132
        subttl  "Interlocked Insert Tail List"
;++
;
; PLIST_ENTRY
; FASTCALL
; ExfInterlockedInsertTailList (
;    IN PLIST_ENTRY ListHead,
;    IN PLIST_ENTRY ListEntry
;    )
;
; Routine Description:
;
;    This function inserts an entry at the tail of a doubly linked list
;    so that access to the list is synchronized in a multiprocessor system.
;
;    N.B. The pages of data which this routine operates on MUST be
;         present.  No page fault is allowed in this routine.
;
; Arguments:
;
;   (ecx) =  ListHead - Supplies a pointer to the head of the doubly linked
;            list into which an entry is to be inserted.
;
;   (edx) =  ListEntry - Supplies a pointer to the entry to be inserted at the
;            tail of the list.
;
; Return Value:
;
;    Pointer to entry that was at the tail of the list or NULL if the list
;    was empty.
;
;--

cPublicFastCall ExfInterlockedInsertTailList, 2
cPublicFpo 0, 1

        pushfd
        cli

        mov     eax, LsBlink[ecx]       ; (eax)->prev entry in the list
        mov     [edx]+LsFlink, ecx      ; store next link in entry
        mov     [edx]+LsBlink, eax      ; store previous link in entry
        mov     [ecx]+LsBlink, edx      ; store next link in head
        mov     [eax]+LsFlink, edx      ; store previous link in next

        popfd

        xor     eax, ecx                ; return null if list was empty
        jz      short Eiit15
        xor     eax, ecx
Eiit15: fstRET  ExfInterlockedInsertTailList

fstENDP ExfInterlockedInsertTailList

        page , 132
        subttl  "Interlocked Remove Head List"
;++
;
; PLIST_ENTRY
; FASTCALL
; ExfInterlockedRemoveHeadList (
;    IN PLIST_ENTRY ListHead
;    )
;
; Routine Description:
;
;    This function removes an entry from the head of a doubly linked list
;    so that access to the list is synchronized in a multiprocessor system.
;    If there are no entries in the list, then a value of NULL is returned.
;    Otherwise, the address of the entry that is removed is returned as the
;    function value.
;
;    N.B. The pages of data which this routine operates on MUST be
;         present.  No page fault is allowed in this routine.
;
; Arguments:
;
;    (ecx) ListHead - Supplies a pointer to the head of the doubly linked
;          list from which an entry is to be removed.
;
; Return Value:
;
;    The address of the entry removed from the list, or NULL if the list is
;    empty.
;
;--

cPublicFastCall ExfInterlockedRemoveHeadList    , 1
cPublicFpo 0, 1

        pushfd
        cli

        mov     eax, [ecx]+LsFlink      ; (eax)-> next entry
        cmp     eax, ecx                ; Is list empty?
        je      short Eirh20            ; if e, list is empty, go Eirh20

        mov     edx, [eax]+LsFlink      ; (ecx)-> next entry(after deletion)
        mov     [ecx]+LsFlink, edx      ; store address of next in head
        mov     [edx]+LsBlink, ecx      ; store address of previous in next
if DBG
        mov     [eax]+LsFlink, 0baddd0ffh
        mov     [eax]+LsBlink, 0baddd0ffh
endif
        popfd                           ; restore flags including interrupts
        fstRET    ExfInterlockedRemoveHeadList

Eirh20: popfd
        xor     eax,eax                 ; (eax) = null for empty list
        fstRET  ExfInterlockedRemoveHeadList

fstENDP ExfInterlockedRemoveHeadList

        page , 132
        subttl  "Interlocked Pop Entry Sequenced List"
;++
;
; PSINGLE_LIST_ENTRY
; FASTCALL
; InterlockedPopEntrySList (
;    IN PSINGLE_LIST_ENTRY ListHead
;    )
;
; Routine Description:
;
;    This function removes an entry from the front of a sequenced singly
;    linked list so that access to the list is synchronized in an MP system.
;    If there are no entries in the list, then a value of NULL is returned.
;    Otherwise, the address of the entry that is removed is returned as the
;    function value.
;
;    N.B. The cmpxchg8b instruction is only supported on some processors.
;         If the host processor does not support this instruction, then
;         then following code is patched to contain a jump to the normal
;         pop entry code which has a compatible calling sequence and data
;         structure.
;
; Arguments:
;
;    (ecx) = ListHead - Supplies a pointer to the sequenced listhead from
;         which an entry is to be removed.
;
; Return Value:
;
;    The address of the entry removed from the list, or NULL if the list is
;    empty.
;
;--

cPublicFastCall InterlockedPopEntrySList, 1

cPublicFpo 0,2

;
; Save nonvolatile registers and read the listhead sequence number followed
; by the listhead next link.
;
; N.B. These two dwords MUST be read exactly in this order.
;

        push    ebx                     ; save nonvolatile registers
        push    ebp                     ;
        mov     ebp, ecx                ; save listhead address

;
; N.B. The following code is the continuation address should a fault
;      occur in the rare case described below.
;

        public  ExpInterlockedPopEntrySListResume
ExpInterlockedPopEntrySListResume:      ;

        mov     edx, [ebp] + 4          ; get current sequence number
        mov     eax, [ebp] + 0          ; get current next link

;
; If the list is empty, then there is nothing that can be removed.
;

Epop10: or      eax, eax                ; check if list is empty
        jz      short Epop20            ; if z set, list is empty
        mov     ecx, edx                ; copy sequence number and depth
        add     ecx, 0FFFFH             ; adjust sequence number and depth

;
; N.B. It is possible for the following instruction to fault in the rare
;      case where the first entry in the list is allocated on another
;      processor and freed between the time the free pointer is read above
;      and the following instruction. When this happens, the access fault
;      code continues execution above at the resumption address and the
;      entire operation is retried.
;

        public  ExpInterlockedPopEntrySListFault
ExpInterlockedPopEntrySListFault:       ;

        mov     ebx, [eax]              ; get address of successor entry

.586
        cmpxchg8b qword ptr [ebp]       ; compare and exchange
.386

        jnz     short Epop10            ; if z clear, exchange failed

;
; Restore nonvolatile registers and return result.
;

cPublicFpo 0,0

Epop20: pop     ebp                     ; restore nonvolatile registers
        pop     ebx                     ;

        fstRET    InterlockedPopEntrySList

fstENDP InterlockedPopEntrySList

        page , 132
        subttl  "Interlocked Push Entry Sequenced List"
;++
;
; PSINGLE_LIST_ENTRY
; InterlockedPushEntrySList (
;    IN PSINGLE_LIST_ENTRY ListHead,
;    IN PSINGLE_LIST_ENTRY ListEntry
;    )
;
; Routine Description:
;
;    This function inserts an entry at the head of a sequenced singly linked
;    list so that access to the list is synchronized in an MP system.
;
;    N.B. The cmpxchg8b instruction is only supported on some processors.
;         If the host processor does not support this instruction, then
;         then following code is patched to contain a jump to the normal
;         push entry code which has a compatible calling sequence and data
;         structure.
;
; Arguments:
;
;    (ecx) ListHead - Supplies a pointer to the sequenced listhead into which
;          an entry is to be inserted.
;
;    (edx) ListEntry - Supplies a pointer to the entry to be inserted at the
;          head of the list.
;
; Return Value:
;
;    Previous contents of ListHead.  NULL implies list went from empty
;       to not empty.
;
;--

cPublicFastCall InterlockedPushEntrySList, 2

cPublicFpo 0,2

;
; Save nonvolatile registers and read the listhead sequence number followed
; by the listhead next link.
;
; N.B. These two dwords MUST be read exactly in this order.
;

        push    ebx                     ; save nonvolatile registers
        push    ebp                     ;
        mov     ebp, ecx                ; save listhead address
        mov     ebx, edx                ; save list entry address
        mov     edx, [ebp] + 4          ; get current sequence number
        mov     eax, [ebp] + 0          ; get current next link
Epsh10: mov     [ebx], eax              ; set next link in new first entry
        mov     ecx, edx                ; copy sequence number
        add     ecx, 010001H            ; increment sequence number and depth

.586
        cmpxchg8b qword ptr[ebp]        ; compare and exchange
.386

        jnz     short Epsh10            ; if z clear, exchange failed

;
; Restore nonvolatile registers and return result.
;

cPublicFpo 0,0

        pop     ebp                     ; restore nonvolatile registers
        pop     ebx                     ;

        fstRET    InterlockedPushEntrySList

fstENDP InterlockedPushEntrySList

        page , 132
        subttl  "Interlocked Flush Sequenced List"
;++
;
; PSINGLE_LIST_ENTRY
; FASTCALL
; InterlockedFlushSList (
;    IN PSINGLE_LIST_ENTRY ListHead
;    )
;
; Routine Description:
;
;    This function removes the entire list from a sequenced singly
;    linked list so that access to the list is synchronized in an MP system.
;    If there are no entries in the list, then a value of NULL is returned.
;    Otherwise, the address of the entry at the top of the list is removed
;    and returned as the function value and the list header is set to point
;    to NULL.
;
;    N.B. The cmpxchg8b instruction is only supported on some processors.
;         If the host processor does not support this instruction, then
;         then following code is patched to contain a jump to the normal
;         pop entry code which has a compatible calling sequence and data
;         structure.
;
; Arguments:
;
;    (ecx) = ListHead - Supplies a pointer to the sequenced listhead from
;         which the list is to be flushed.
;
; Return Value:
;
;    The address of the entire current list, or NULL if the list is
;    empty.
;
;--

cPublicFastCall InterlockedFlushSList, 1

cPublicFpo 0,1

;
; Save nonvolatile registers and read the listhead sequence number followed
; by the listhead next link.
;
; N.B. These two dwords MUST be read exactly in this order.
;

        push    ebx                     ; save nonvolatile registers
        push    ebp                     ;
        mov     ebp, ecx                ; save listhead address
        mov     edx, [ebp] + 4          ; get current sequence number
        mov     eax, [ebp] + 0          ; get current next link

;
; N.B. The following code is the retry code should the compare
;      part of the compare exchange operation fail
;
; If the list is empty, then there is nothing that can be removed.
;

Efls10: or      eax, eax                ; check if list is empty
        jz      short Efls20            ; if z set, list is empty
        mov     ecx, 0	                ; clear sequence number and depth
        mov     ebx, 0                  ; clear successor entry pointer

.586
        cmpxchg8b qword ptr [ebp]       ; compare and exchange
.386

        jnz     short Efls10            ; if z clear, exchange failed

;
; Restore nonvolatile registers and return result.
;

cPublicFpo 0,0

Efls20: pop     ebp                     ; restore nonvolatile registers
        pop     ebx                     ;

        fstRET    InterlockedFlushSList

fstENDP InterlockedFlushSList

        page , 132
        subttl  "Interlocked i386 Exchange Ulong"
;++
;
;   ULONG
;   FASTCALL
;   InterlockedExchange (
;       IN PULONG Target,
;       IN ULONG Value
;       )
;
;   Routine Description:
;
;       This function atomically exchanges the Target and Value, returning
;       the prior contents of Target
;
;       See Exi386InterlockedExchangeUlong.  This function is the i386
;       architectural specific version of ExInterlockedDecrementLong.
;       No source directly calls this function, instead
;       ExInterlockedDecrementLong is called and when built on x86 these
;       calls are macroed to the i386 optimized version.
;
;   Arguments:
;
;       (ecx) = Source - Address of ULONG to exchange
;       (edx) = Value  - New value of ULONG
;
;   Return Value:
;
;       The prior value of Source
;--

cPublicFastCall InterlockedExchange, 2
cPublicFpo 0,0
.486
        mov     eax, [ecx]                  ; get comperand value
Ixchg:  cmpxchg [ecx], edx                  ; compare and swap
        jnz     Ixchg                       ; if nz, exchange failed
.386

        fstRET  InterlockedExchange
fstENDP InterlockedExchange

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
;    Addend (ecx) - Supplies a pointer to a variable whose value is to be
;       incremented by one.
;
; Return Value:
;
;   (eax) - The incremented value.
;
;--

cPublicFastCall InterlockedIncrement,1
cPublicFpo 0,0

        mov     eax, 1                  ; set increment value

.486
        xadd    [ecx], eax              ; interlocked increment
.386p
        inc     eax                     ; adjust return value

        fstRET InterlockedIncrement

fstENDP InterlockedIncrement

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
;    Addend (ecx) - Supplies a pointer to a variable whose value is to be
;       decremented by one.
;
; Return Value:
;
;   (eax) - The decremented value.
;
;--

cPublicFastCall InterlockedDecrement,1
cPublicFpo 0,0

        mov     eax, -1                 ; set decrment value

.486
        xadd    [ecx], eax              ; interlocked decrement
.386

        dec     eax                     ; adjust return value

        fstRET InterlockedDecrement

fstENDP InterlockedDecrement

        page , 132
        subttl  "Interlocked Compare Exchange"
;++
;
;   PVOID
;   FASTCALL
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
;    (ecx)  Destination - Supplies a pointer to destination value.
;
;    (edx) Exchange - Supplies the exchange value.
;
;    [esp + 4] Comperand - Supplies the comperand value.
;
; Return Value:
;
;    The initial destination value is returned as the function value.
;
;--

cPublicFastCall InterlockedCompareExchange, 3
cPublicFpo 0,0

        mov     eax, [esp + 4]          ; set comperand value
.486
        cmpxchg [ecx], edx              ; compare and exchange
.386

        fstRET  InterlockedCompareExchange

fstENDP InterlockedCompareExchange

        page , 132
        subttl  "Interlocked Compare Exchange 64-bits"
;++
;
; LONGLONG
; FASTCALL
; ExInterlockedCompareExchange64 (
;    IN PLONGLONG Destination,
;    IN PLONGLONG Exchange,
;    IN PLONGLONG Comperand
;    )
;
; Routine Description:
;
;    This function performs a compare and exchange of 64-bits.
;
;    N.B. The cmpxchg8b instruction is only supported on some processors.
;         If the host processor does not support this instruction, then
;         then following code is patched to contain a jump to the normal
;         compare exchange 64-bit code which has a compatible calling
;         sequence and data structure.
;
; Arguments:
;
;    (ecx) Destination - Supplies a pointer to the destination variable.
;
;    (edx) Exchange - Supplies a pointer to the exchange value.
;
;    (esp+4) Comperand - Supplies a pointer to the comperand value.
;
; Return Value:
;
;    The current destination value is returned as the function value.
;
;--

cPublicFastCall ExInterlockedCompareExchange64, 3

cPublicFpo 0,1

;
; Save nonvolatile registers and read the exchange and comperand values.
;

        push    ebx                     ; save nonvolatile registers
        push    ebp                     ;
        mov     ebp, ecx                ; set destination address
        mov     ebx, [edx]              ; get exchange value
        mov     ecx, [edx] + 4          ;
        mov     edx, [esp] + 12         ; get comperand address
        mov     eax, [edx]              ; get comperand value
        mov     edx, [edx] + 4          ;

.586
        cmpxchg8b qword ptr[ebp]        ; compare and exchange
.386

;
; Restore nonvolatile registers and return result in edx:eax.
;

cPublicFpo 0,0

        pop     ebp                     ; restore nonvolatile registers
        pop     ebx                     ;

        fstRET    ExInterlockedCompareExchange64

fstENDP ExInterlockedCompareExchange64

        page , 132
        subttl  "Interlocked Exchange Add"
;++
;
;   LONG
;   FASTCALL
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
;    (ecx)  Addend - Supplies a pointer to a variable whose value is to be
;                    adjusted by the increment value.
;
;    (edx) Increment - Supplies the increment value to be added to the
;                      addend variable.
;
; Return Value:
;
;    The initial value of the addend variable.
;
;--

cPublicFastCall InterlockedExchangeAdd, 2
cPublicFpo 0,0

.486
        xadd    [ecx], edx              ; exchange add
.386

        mov     eax, edx                ; set initial value

        fstRET  InterlockedExchangeAdd

fstENDP InterlockedExchangeAdd

_TEXT   ends
        end
