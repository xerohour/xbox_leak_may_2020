        TITLE   "Memory Management Support Routines"
;++
;
;  Copyright (c) 2000-2001  Microsoft Corporation
;
;  Module Name:
;
;     mmsupa.asm
;
;  Abstract:
;
;     This module implements the various routines for miscellaneous support
;     operations for memory management.
;
;  Environment:
;
;     Kernel mode only.
;
;--

        .586p
        .xlist
INCLUDE ks386.inc
INCLUDE callconv.inc
        .list

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:FLAT, FS:NOTHING, GS:NOTHING

;++
;
; VOID
; FASTCALL
; MiZeroAndFlushPtes(
;     IN PMMPTE StartingPte,
;     IN PFN_COUNT NumberOfPtes
;     )
;
; Routine Description:
;
;     This function zeroes a range of PTEs and flushes its corresponding TLB
;     entries.  The NumberOfPtes must be greater than zero.
;
; Arguments:
;
;     StartingPte - Supplies the first PTE to zero.
;
;     NumberOfPtes - Supplies the number of PTEs to zero.
;
; Return Value:
;
;     None.
;
;--
cPublicFastCall MiZeroAndFlushPtes, 2

IF DBG
        test    edx, edx
        jnz     @F
        int     3
@@:
ENDIF

        mov     eax, ecx
        shl     eax, 10             ; eax = MiGetVirtualAddressMappedByPte(ecx)

FlushNextPte:
        mov     [ecx], 0
        invlpg  [eax]
        add     ecx, 4
        add     eax, PAGE_SIZE
        dec     edx
        jnz     FlushNextPte

        fstRET  MiZeroAndFlushPtes

fstENDP MiZeroAndFlushPtes

;++
;
; VOID
; MiRelocateBusyPageHelper(
;     IN PVOID OldPhysicalAddress,
;     IN PVOID NewPhysicalAddress,
;     IN PMMPTE PointerPte OPTIONAL,
;     IN MMPTE NewPteContents OPTIONAL,
;     IN BOOLEAN FullTlbFlush
;     )
;
; Routine Description:
;
;     This function is called by MiRelocateBusyPage to handle the work of
;     copying a page's contents, updating the page tables, and flushing the
;     appropriate TLB entries.  This routine is implemented in assembly to
;     ensure that the stack is touched properly in the case of relocating the
;     current stack's pages.
;
; Arguments:
;
;     OldPhysicalAddress - Supplies a KSEG0 pointer to the old page's contents.
;
;     NewPhysicalAddress - Supplies a KSEG0 pointer to the new location for the
;         page.
;
;     PointerPte - Supplies an optional pointer to the PTE to update after doing
;         the page copy.
;
;     NewPteContents - Supplies the value to write to PointerPte.
;
;     FullTlbFlush - Specifies whether the entire TLB should be flushed after
;         updating PointerPte or only the virtual address mapped by the PTE.
;
; Return Value:
;
;     None.
;
;--
cPublicProc _MiRelocateBusyPageHelper, 5

        push    esi
        mov     esi, [esp+8]        ; esi = OldPhysicalAddress
        push    edi
        mov     edi, [esp+16]       ; edi = NewPhysicalAddress
        mov     edx, [esp+20]       ; edx = PointerPte
        mov     ecx, PAGE_SIZE / 4
        mov     eax, [esp+24]       ; eax = NewPteContents

;
; Disable interrupts during the rest of the operation.  The page we're moving
; could be modified by an interrupt service routine while we're copying it.
;

        cli

;
; Copy the old page to the new page.
;

        rep     movsd

;
; Grab the last value from the stack before we start changing the page tables.
;

        mov     ecx, [esp+28]       ; cl = FullTlbFlush

;
; Update the page table entry.
;

        mov     [edx], eax          ; MI_WRITE_PTE(edx, eax)

;
; Check if we should flush just a single TLB entry or the entire TLB.
;

        test    cl, cl
        jnz     FlushEntireTlb

FlushSingleTlb:
        shl     edx, 10             ; edx = MiGetVirtualAddressMappedByPte(edx)
        invlpg  [edx]
        jmp     ReenableInterrupts

FlushEntireTlb:
        mov     eax, cr3
        mov     cr3, eax

;
; The page tables now point at the new page and the TLB has been flushed.  It's
; safe to allow interrupts to fire again.
;

ReenableInterrupts:
        sti

        pop     edi
        pop     esi

        stdRET  _MiRelocateBusyPageHelper

stdENDP _MiRelocateBusyPageHelper

_TEXT   ENDS

        END
