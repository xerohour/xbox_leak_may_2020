;===================================================================
;
; Copyright (C) 1998, NVidia Coporation
;
; File:         gnmem.asm
;
; Purpose:      This file holds routines for the linear memory heap
;               manager. You can create and destroy heaps, allocate
;               memory from the heap and free memory back to heap.
;
; The basic strategy is to maintain three doubly linked lists for every
; heap. The elements in every list are always MEMINFO structures. There
; are a fixed number of total elements which places a limit on how many
; allocations that can be oustanding at any one time. This fixed number
; can be run-time decided, however, and shouldn't affect performance as
; long as it is made large enough to accomodate sufficient simultaneous
; allocations. The three lists are:
;
; The ALLOC list contains elements describing linear chunks of the heap
; which are currently allocated.
;
; The FREE list contains elements describing linear chunks of the heap
; which are NOT currently allocated.
;
; The MANAGEMENT list (or MAN list) contains elements which can be used
; as either an element in the ALLOC list or the FREE list.
;
; Whenever a new chunk of memory is allocated, a MEMINFO element is
; pulled out of the MAN list and placed into the ALLOC list after being
; filled in appropriately to describe the new allocation. Also, a MEMINFO
; element in the FREE list is modified to reflect that it no longer
; describes as large a chunk of memory as it used to. Alternatively, if
; the newly alloced chunk was less than but close to the size of the
; chunk in the FREE list, then the MEMINFO element from the FREE list
; is just placed in the ALLOC list and removed from the FREE list.
;
; When memory is freed a similar process takes place. The MEMINFO element
; in the ALLOC list that is being freed is checked to see if it can be
; merged into a larger contiguous block with other elements in the FREE
; list. If it can, then a block already in the FREE list is modified to
; reflect the larger free chunk and the MEMINFO element from the ALLOC
; list is removed and placed into the MAN list. If the belock being
; freed cannot be merged, then the MEMINFO element is removed from the
; ALLOC list and placed into the FREE list.
;
; There is one block in each list that is the head block. That is an
; anchor from which I can start walking the list.
;==============================================================================
.586
incDrawMode = 1
incDevice   = 1
incLogical  = 1
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc

PREPEND_BLOCK_TO_LIST   MACRO   listhead,block
        LOCAL   Eol

        mov     edi,listhead
        mov     eax,ds:[edi].MEMINFO.dwNext
        mov     ds:[edi].MEMINFO.dwNext,block
        mov     ds:[block].MEMINFO.dwPrev,listhead
        mov     ds:[block].MEMINFO.dwNext,eax
        cmp     eax,SENTINEL
        je      Eol
        mov     edi,eax
        mov     ds:[edi].MEMINFO.dwPrev,block
Eol:
ENDM

INSERT_BLOCK1_AFTER_BLOCK2      MACRO  blocka,blockb
        LOCAL   Eol

        mov     edi,ds:[blockb].MEMINFO.dwNext
        mov     ds:[blockb].MEMINFO.dwNext,blocka
        mov     ds:[blocka].MEMINFO.dwPrev,blockb
        mov     ds:[blocka].MEMINFO.dwNext,edi
        cmp     edi,SENTINEL
        je      Eol
        mov     ds:[edi].MEMINFO.dwPrev,blocka
Eol:
ENDM

EXTRACT_BLOCK_FROM_LIST MACRO   block
        LOCAL   Eol

        mov     edi,ds:[block].MEMINFO.dwPrev
        mov     eax,ds:[block].MEMINFO.dwNext
        mov     ds:[edi].MEMINFO.dwNext,eax
        cmp     eax,SENTINEL
        je      Eol
        mov     ds:[eax].MEMINFO.dwPrev,edi
Eol:
ENDM

; Every allocation will be rounded up to next MINALLOCUNIT multiple
MINALLOCUNIT    EQU     64

; This marks the beginning and ending of lists
SENTINEL        TEXTEQU <-1>

; This is the list element for all three lists. It holds doubly linked
; pointers and the base and length of an allocation.
MEMINFO STRUC
        dwBase          DD      0
        dwLength        DD      0
        dwPrev          DD      0
        dwNext          DD      0
MEMINFO ENDS

; I want to keep stats on the allocations and it makes the most sense
; to store these in a few fields of the MEMINFO structures that are
; unused since I have to allocate memory for these anyway. Here are
; the stats I will be keeping and where they are being placed.

; MANHEAD.dwBase        Number of MEMINFO elements in MAN list
; MANHEAD.dwLength      Number of MEMINFO elements (Total)
; ALLOCHEAD.dwBase      Number of Allocations currently outstanding
; ALLOCHEAD.dwLength    Number of Bytes in all Outstanding Allocations
; FREEHEAD.dwLength     Number of Free Bytes left in the Heap

NUMMEMINFOS     TEXTEQU <(MEMINFO PTR ds:[MANHEAD]).dwLength>
NUMMANS         TEXTEQU <word ptr (MEMINFO PTR ds:[MANHEAD]).dwBase>
NUMALLOCS       TEXTEQU <(MEMINFO PTR ds:[ALLOCHEAD]).dwBase>
ALLOCEDBYTES    TEXTEQU <(MEMINFO PTR ds:[ALLOCHEAD]).dwLength>
TOTALBYTES      TEXTEQU <(MEMINFO PTR ds:[FREEHEAD]).dwBase>
FREEBYTES       TEXTEQU <(MEMINFO PTR ds:[FREEHEAD]).dwLength>

; The first five blocks of a heap are fixed function. These equates
; make sure I am consistent about the usage of the blocks.
MANHEAD         EQU     ((SIZE MEMINFO) * 0)
FREEHEAD        EQU     ((SIZE MEMINFO) * 1)
ALLOCHEAD       EQU     ((SIZE MEMINFO) * 2)
FIRSTFREEBLOCK  EQU     ((SIZE MEMINFO) * 3)
FIRSTMANBLOCK   EQU     ((SIZE MEMINFO) * 4)



OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GN_HeapCreate
;
; Purpose:      This routine creates a linear memory heap. The base
;               minimum number of simultaneous
;               allocations (MSA) are determined by the caller. For
;               example, the caller might request that the MSA be 100H.
;               Then any attempt to allocate from the
;               heap when there are alredy 100H other allocations
;               might fail even if there is enough memory. I say might
;               because it depends upon how fragmented the heap is.
;               The less fragmented the more requests can be handled
;               at the same time. However, you never get less than
;               the number requested herein and you will never get
;               more than about 2 x the number you request.
;
;               NOTE: This routine does not actually allocate allocate
;               memory for a heap. It just sets up data structures to
;               manage a linear heap. The caller must allocate memory
;               if it wants something to actually match these number to.
;
; Arguments:
;               eax     The minimum number of simultaneous allocations
;                       allowed before allocs fail
;
; Returns:      eax     Heap token if successful
;                       NULL if unsuccessful
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_HeapCreate, PASCAL, FRAME, FAR32
LOCALD  dwTotalMemInfos
OPENPROC
        PUSHR   ds,esi,edi

        ; There is an alloc list, a free list and a manage list.
        ; All three lists must have a header block. Plus, the worst
        ; fragmented heap will have alternating FREE and ALLOC
        ; blocks with a FREE block at the beginning and the end.
        ; Also I have a few blocks that I keep statistics in.
        ; Hence, to guarantee ecx allocations, we need at
        ; least 2 * eax + 6 MEMINFO structs.
        lea     eax,[eax * 2 + 6]
        mov     dwTotalMemInfos,eax

        ; Now allocate enough bytes for eax MEMINFO structs
        mov     ecx,SIZE MEMINFO
        imul    eax,ecx
        call    GN_MemoryAlloc
        or	eax,eax
        je	DoneIt

        mov	ds,ax
        mov	ecx,dwTotalMemInfos
	mov	NUMMEMINFOS,ecx
DoneIt:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_HeapInit
;
; Purpose:      This routine inits a linear memory heap.
;
; Arguments:
;		eax	Heap Token returned from GN_HeapCreate
;		ebx     The desired base address
;               ecx     The number of bytes in the heap
;
; Returns:      Nothing
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_HeapInit, PASCAL, FRAME, FAR32
OPENPROC
        PUSHR   ds,esi,edi
        mov     ds,ax

        ; Save the base address and heap size. Round heap size down to
        ; the closest multiple of the MINALLOCUNIT.
        mov     esi,ebx
        mov     edi,ecx
        and     edi,NOT (MINALLOCUNIT - 1)
        mov     eax,SENTINEL

        ; First setup the FREE list
        mov     ebx,FREEHEAD
        mov     ds:[ebx].MEMINFO.dwPrev,eax
        mov     ds:[ebx].MEMINFO.dwNext,FIRSTFREEBLOCK
        mov     FREEBYTES,edi
        mov	TOTALBYTES,edi
        mov     ebx,FIRSTFREEBLOCK
        mov     ds:[ebx].MEMINFO.dwPrev,FREEHEAD
        mov     ds:[ebx].MEMINFO.dwNext,eax
        mov     ds:[ebx].MEMINFO.dwBase,esi
        mov     ds:[ebx].MEMINFO.dwLength,edi

        ; Next, setup the ALLOC list
        mov     ebx,ALLOCHEAD
        mov     ds:[ebx].MEMINFO.dwPrev,eax
        mov     ds:[ebx].MEMINFO.dwNext,eax
        sub     ecx,ecx
        mov     NUMALLOCS,ecx
        mov     ALLOCEDBYTES,ecx

        ; Lastly, setup the MAN list
        mov     ebx,MANHEAD
        mov     ds:[ebx].MEMINFO.dwPrev,eax
        mov     ds:[ebx].MEMINFO.dwNext,FIRSTMANBLOCK
        mov     ecx,NUMMEMINFOS
        mov     NUMMANS,cx

        ; All the remaining blocks belong in the MAN list -- doubly linked
        mov     ebx,FIRSTMANBLOCK
        mov     ds:[ebx].MEMINFO.dwPrev,MANHEAD
        mov     ds:[ebx].MEMINFO.dwNext,eax
        sub     ecx,5
        jle     Done
@@:     lea     esi,[ebx + SIZE MEMINFO]
        mov     ds:[ebx].MEMINFO.dwNext,esi
        mov     edx,ebx
        add     ebx,SIZE MEMINFO
        mov     ds:[ebx].MEMINFO.dwPrev,edx
        mov     ds:[ebx].MEMINFO.dwNext,eax
        dec     ecx
        jg      @B

Done:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_HeapDestroy
;
; Purpose:      This routine destroys a linear memory heap.
;
; Arguments:    eax     Heap Token returned by GN_HeapCreate
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_HeapDestroy, PASCAL, NOFRAME, FAR32
OPENPROC
        call    GN_MemoryFree
CLOSEPROC


;==============================================================================
;
; Function:     GN_HeapAlloc
;
; Purpose:      This routine allocates memory from a heap.
;
; Arguments:    eax     Heap Token returned by GN_HeapCreate
;               ecx     bytes needed from heap.
;                       NOTE: This number will be rounded up to
;                       the next multiple of MINALLOCUNIT.
;
; Returns:      eax     Offset of the allocation if successful
;                               In this case ecx = actual size allocated
;                       -1 if it could not allocate
;
; Preserve:     ds,esi,edi,es
;==============================================================================
DECPROC GN_HeapAlloc, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   ds,esi,edi

        ; Get the heap token (which is the selector of a memory block
        ; that has the heap info in it) into ds.
        mov     ds,ax

        ; Round the requested allocation amount up to next multiple
        ; of MINALLOCUNIT.
        add     ecx,MINALLOCUNIT - 1
        sub     eax,eax
        and     ecx,NOT (MINALLOCUNIT - 1)

        ; See if we have at least this many bytes free in the heap.
        ; If not, don't bother walking the FREE list.
        cmp     ecx,FREEBYTES
        ja      Done

        ; Is there a MEMINFO block that can hold this allocation or
        ; will we exceed the simultaneous allocation limit?
        mov     edi,MANHEAD
        cmp     ds:[edi].MEMINFO.dwNext,SENTINEL
        je      Done

        ; Walk the FREE list looking for a free block at least as big
        ; as the rounded up request size. If we reach the end of the
        ; free list without finding one, that is an error.
        mov     esi,FREEHEAD
@@:     mov     esi,ds:[esi].MEMINFO.dwNext
        cmp     esi,SENTINEL
        je      Done
        cmp     ecx,ds:[esi].MEMINFO.dwLength
        ja      @B

        ; We found a block in the FREE list that is big enough. If the
        ; block is exactly the requested size, then just remove the free
        ; block from the free list and put it in the alloced list.
        je      MoveFreeBlockToAllocList

        ; The free block is bigger than we need. Use the first portion of
        ; the free block as our allocated area and adjust the free block
        ; to describe only the free memory after the newly allocated area.

        ; Remember the base of the free block. It will become the
        ; base of our allocation.
        mov     eax,ds:[esi].MEMINFO.dwBase

        ; Update the block in the FREE list to show it is a smaller region
        add     ds:[esi].MEMINFO.dwBase,ecx     ; The base is further
        sub     ds:[esi].MEMINFO.dwLength,ecx   ; The length is less

        ; Get a new MEMINFO struct to use to describe the allocation.
        ; One exists or we would never have gotten this far.
        mov     esi,ds:[edi].MEMINFO.dwNext     ; edi = MANHEAD remember
        dec     NUMMANS                         ; We used a MAN list block

        ; Set the base and length of this new alloced block
        mov     ds:[esi].MEMINFO.dwBase,eax     ; Base of allocation
        mov     ds:[esi].MEMINFO.dwLength,ecx   ; And the is the length

MoveFreeBlockToAllocList:
        ; Pull the MEMINFO block pointed to by esi out of the list
        ; it is in. This may be the FREE list or the MAN list, but
        ; it doesn't matter. The same code is used for both.
        EXTRACT_BLOCK_FROM_LIST esi

        ; The MEMINFO block pointed to by esi describes an allocation
        ; so place it at the head of the ALLOC list.
        PREPEND_BLOCK_TO_LIST  ALLOCHEAD,esi

        ; Now return the base address of the length of the allocation
        mov     eax,ds:[esi].MEMINFO.dwBase
        mov     ecx,ds:[esi].MEMINFO.dwLength

        ; Keep track of these -- simple and takes little time.
        inc     NUMALLOCS
        add     ALLOCEDBYTES,ecx
        sub     FREEBYTES,ecx

Done:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_HeapFree
;
; Purpose:      This routine frees memory from a heap.
;
; Arguments:    eax     Heap Token returned by GN_HeapCreate
;               ecx     Allocation offset returned by GN_HeapAlloc
;
; Returns:      eax     non-0   Valid allocation was found and freed
;                               In this case ecx = number of bytes in
;                               the allocation.
;                       0       Could not find this allocation
;                               Nothing bad happens here, but as long
;                               as you match the allocs and frees this
;                               case shouldn't really ever happen.
;
; Preserve:     ds,esi,edi,es
;==============================================================================
DECPROC GN_HeapFree, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   ds,esi,edi

        ; Get the heap token (which is the selector of a memory block
        ; that has the heap info in it) into ds.
        mov     ds,ax
        sub     eax,eax

        ; Search the ALLOC list for the allocation offset
        mov     esi,ALLOCHEAD
@@:     mov     esi,ds:[esi].MEMINFO.dwNext
        cmp     esi,SENTINEL
        je      Fail
        cmp     ecx,ds:[esi].MEMINFO.dwBase
        jne     @B

        ; Found It! Remove the allocation from the ALLOC list
        EXTRACT_BLOCK_FROM_LIST esi

        ; save the amount we are freeing
        push    ds:[esi].MEMINFO.dwLength

        ; The FREE list is kept sorted so that we can merge newly
        ; freed blocks back in with existing free blocks to make
        ; the largest free blocks available. Keeping the list
        ; sorted makes this much easier.
        mov     ebx,FREEHEAD
        cmp     ds:[ebx].MEMINFO.dwNext,SENTINEL
        je      InsertEsiBlockAfterEbxBlock
@@:     mov     ebx,ds:[ebx].MEMINFO.dwNext
        cmp     ecx,ds:[ebx].MEMINFO.dwBase
        jc      TryMergeWithNextAndPrev
        cmp     ds:[ebx].MEMINFO.dwNext,SENTINEL
        jne     @B

TryMergeAtEnd:
        ; If we get here, then the esi block we are freeing might merge
        ; on the end of the last block currently in the FREE list, or it
        ; might be the new last block on the FREE list.
        mov     eax,ds:[ebx].MEMINFO.dwBase
        add     eax,ds:[ebx].MEMINFO.dwLength
        cmp     eax,ecx
        jne     InsertEsiBlockAfterEbxBlock

        ; The newly freed block pointed to by esi does merge with the
        ; current last free block pointed to by ebx.
        mov     eax,ds:[esi].MEMINFO.dwLength
        add     ds:[ebx].MEMINFO.dwLength,eax

        ; We no longer need the MEMINFO struct pointed to by esi which
        ; described the block just freed so give it back to the MAN list.
        ; Also, update the number of blocks in the MAN list.
        PREPEND_BLOCK_TO_LIST  MANHEAD,esi
        inc     NUMMANS
        jmp     Success

InsertEsiBlockAfterEdiBlock:
        mov     ebx,edi

InsertEsiBlockAfterEbxBlock:
        ; [esi] block belongs after [ebx] block
        INSERT_BLOCK1_AFTER_BLOCK2 esi,ebx
        jmp     Success

TryMergeWithNextAndPrev:
        ; We now know that the block pointed to by esi goes in the FREE
        ; list in between the block pointed to by ebx and the block
        ; before ebx in the FREE list. Let's see if the esi block merges
        ; with the ebx block, first.
        mov     edx,ds:[esi].MEMINFO.dwBase
        mov     eax,ds:[ebx].MEMINFO.dwBase
        add     edx,ds:[esi].MEMINFO.dwLength
        cmp     eax,edx
        jne     TryMergeWithPrev

        ; It does! Back up the base of the ebx block to include the
        ; esi block and increase the ebx block length.
        mov     edx,ds:[esi].MEMINFO.dwLength
        sub     ds:[ebx].MEMINFO.dwBase,edx
        add     ds:[ebx].MEMINFO.dwLength,edx

        ; Now we can get rid of the esi block since we don't need it.
        ; Indicate that we have given back a block to the MAN list.
        PREPEND_BLOCK_TO_LIST  MANHEAD,esi
        inc     NUMMANS

        ; Now see if we can merge this new free block with the previous
        ; block in the FREE list. Careful because we may already be at
        ; the start of the free list.
        mov     edi,ds:[ebx].MEMINFO.dwPrev
        cmp     edi,FREEHEAD
        je      Success
        mov     eax,ds:[edi].MEMINFO.dwBase
        add     eax,ds:[edi].MEMINFO.dwLength
        cmp     eax,ds:[ebx].MEMINFO.dwBase
        jne     Success

        ; It does merge! So combine these two blocks.
        mov     ecx,ds:[ebx].MEMINFO.dwLength
        add     ds:[edi].MEMINFO.dwLength,ecx
        jmp     RemoveEbxFromFreeListAndInsertItInManList

TryMergeWithPrev:
        ; The esi block did not merge with the block after it in the
        ; FREE list, so now let's see if it merges with the one before
        ; it in the FREE list. The one before it may be the FREEHEAD,
        ; so we need to check for that as well. DOn't forget that ecx
        ; holds the base addr of the allocation we are freeing.
        mov     edi,ds:[ebx].MEMINFO.dwPrev
        cmp     edi,FREEHEAD
        je      InsertEsiBlockAfterEdiBlock
        mov     eax,ds:[edi].MEMINFO.dwBase
        add     eax,ds:[edi].MEMINFO.dwLength
        cmp     eax,ecx
        jne     InsertEsiBlockAfterEdiBlock

        ; Yep! The esi block merges with the edi block -- so do it.
        mov     edx,ds:[esi].MEMINFO.dwLength
        add     ds:[edi].MEMINFO.dwLength,edx

        ; Now we can stick the esi block back into the MAN list
        ; and update the count of available MEMINFO blocks.
        PREPEND_BLOCK_TO_LIST  MANHEAD,esi
        inc     NUMMANS
        jmp     Success

RemoveEbxFromFreeListAndInsertItInManList:
        ; We no longer need the ebx block -- it was merged with another
        ; block. Remove it from the list it is currently in...
        EXTRACT_BLOCK_FROM_LIST ebx

        ; ... and stick the ebx block back in the MAN list.
        ; Update the count of available MEMINFO blocks.
        PREPEND_BLOCK_TO_LIST  MANHEAD,ebx
        inc     NUMMANS
        jmp     Success

Fail:   sub     eax,eax
        jmp     Done

Success:
        ; This is the number of bytes freed. We need to return this.
        pop     ecx

        ; Keep track of these -- simple and takes little time.
        dec     NUMALLOCS
        sub     ALLOCEDBYTES,ecx
        add     FREEBYTES,ecx
        mov     eax,1

Done:
        POPR    ds,esi,edi
CLOSEPROC

CLOSESEG _NVTEXT32

END

