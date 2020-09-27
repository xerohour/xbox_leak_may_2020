;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnoffbmp.asm
;
; Purpose:      This file holds routines for flushing, initting,
;               de-initting, allocating and freeing offscreen bitmaps.
;
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


; I want to keep stats on the allocations and it makes the most sense
; to store these in a few fields of the first OFFBMPINFO structure.
; Here are the stats I will be keeping and where they are being placed.
;
; dwBitmapLength - total number of offscreen bitmaps ever allocated
; dwBitmapVRAMOffset - bytes of offscreen memory currently allocated to
;                         offscreen bitmaps
; wHeaderSel     - number of offscreen bitmaps currently allocated
; wUnused        - Byte Offset in array of next OFFBMPINFO struct
;                         to use for the next offscreen bitmap,

NEXT_FREE_ENTRY                         MACRO   sreg,reg
        EXITM   <@CatStr(sreg, <:[>, reg, <].OFFBMPINFO.wUnused>)>
ENDM

NUM_ALLOCED_OFFSCREEN_BITMAPS           MACRO   sreg,reg
        EXITM   <@CatStr(sreg, <:[>, reg, <].OFFBMPINFO.wBitmapSel>)>
ENDM

BYTES_USED_FOR_OFFSCREEN_BITMAPS        MACRO   sreg,reg
        EXITM   <@CatStr(sreg, <:[>, reg, <].OFFBMPINFO.dwBitmapVRAMOffset>)>
ENDM

TOTAL_ALLOCED_OFFSCREEN_BITMAPS         MACRO   sreg,reg
        EXITM   <@CatStr(sreg, <:[>, reg, <].OFFBMPINFO.dwBitmapLength>)>
ENDM


OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE
;==============================================================================
;
; Function:     GN_ClearOffscreenBitmapCache
;
; Purpose:      This function clears the offscreen bitmap cache.
;
; Arguments:
;               ds              Selector for the board Context data
;               pBoardCXOffset  Offset to the board context data
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_ClearOffscreenBitmapCache, PASCAL, FRAME, FAR32
PARMD   pBoardCXOffset
OPENPROC
        PUSHR   es,esi,edi
        mov     esi,pBoardCXOffset
        mov     edi,ds:[esi].GNBOARDCX.pNextOffbmpinfo

LoopY:
        ; Are there more headers?
        lea     ebx,[esi].GNBOARDCX.OffscreenBitmapCache
        sub     edi,SIZE OFFBMPINFO
        cmp     edi,ebx
        jc      ClearStats

        ; Is the bitmap still valid?
        mov     eax,ds:[edi].OFFBMPINFO.dwBitmapSel
        call    GN_OffscreenBitmapValidCheck
        or      eax,eax
	jnz	FlushIt

	; Bitmap lost. Free offscreen memory.
	push	esi
	push	edi
	call	GN_OffscreenBitmapDelete
	jmp	LoopY

FlushIt:
        ; Yes it is, so flush it out to system memory and clear
        ; out the ptr to the bitmap header.
        mov     eax,ds:[edi].OFFBMPINFO.dwBitmapSel
	mov	es,ax
	push	esi
        mov     esi,ds:[edi].OFFBMPINFO.dwBitmapOffset
        call	GN_FlushOffscreenBitmap
	pop	esi
        mov     dword ptr ds:[edi],0
        jmp     LoopY

ClearStats:
        ; Reset the statistics
        sub     eax,eax
        mov     ds:[esi].GNBOARDCX.dwOffscreenBitmapsAllocatedSinceLastFlush,eax
        mov     ds:[esi].GNBOARDCX.dwOffscreenBitmapsCurrentlyAllocated,eax
        mov     ds:[esi].GNBOARDCX.dwBytesOfOffscreenMemoryCurrentlyAllocatedForBitmaps,eax
        lea     eax,[esi].GNBOARDCX.OffscreenBitmapCache
        mov     ds:[esi].GNBOARDCX.pNextOffbmpinfo,eax

        POPR    es,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_OffscreenBitmapCreate
;
; Purpose:      This routine will create an offscreen bitmap by allocating
;               memory and modifying an existing DIBENGINE header.
;
; Arguments:
;               ds                      CXSel
;               pGNBoardCXOffset
;               es:edi                  ptr to DEVBMP.
;                                       The fields below are valid:
;                                       deWidth
;                                       deHeight
;                                       deWidthBytes
;                                       dePlanes
;                                       deBitsPixel
;                                       delpPDevice
;
; Returns:
;               eax     0       If the routine did not create the bitmap
;                       non-0   If the routine did create the bitmap
;                               In this case, the routine must have
;                               filled in the fields below:
;                               ds:[esi].DIBENGINE.deDeltaScan - bitmap pitch
;                               ds:[esi].DIBENGINE.deBits - ptr to bits
;
; Preserve:     ds,esi,edi,es
;==============================================================================
DECPROC GN_OffscreenBitmapCreate, PASCAL, FRAME, NEAR
PARMD   pGNBoardCXOffset
LOCALD  dwBmpOffset
LOCALD  dwBmpSize
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNBoardCXOffset
        mov     dwBmpOffset,edi

        ; Can our list of offscreen bitmaps hold another one?
        sub     eax,eax
        lea     ecx,ds:[esi].GNBOARDCX.OffscreenBitmapCache
        add     ecx,(MAX_OFFSCREEN_BITMAPS + 1) * SIZE OFFBMPINFO
        cmp     ecx,ds:[esi].GNBOARDCX.pNextOffbmpinfo
        jbe     Done

        ; Is there an offscreen heap?
        test	ds:[esi].GNBOARDCX.dwOffscreenFlags,NO_OFFS_BMPS
        jnz     Done

        ; Call the hardware specific routine to make sure it is OK
        ; to place this bitmap in offscreen memory.
        mov     ebx,ds:[esi].GNBOARDCX.pHWBoardCX
        call    HW_CreateOffscreenBitmap
        or      eax,eax
        je      Done

	; Check for the same selector at first
        sub     ebx,ebx
        mov     bx,es
        or      ebx,3
        lea     edx,ds:[esi].GNBOARDCX.OffscreenBitmapCache
        mov     edi,ds:[esi].GNBOARDCX.pNextOffbmpinfo
@@:
        sub     edi,SIZE OFFBMPINFO
        cmp     edi,edx
        jb      AllocSpace
        cmp     ebx,ds:[edi].OFFBMPINFO.dwBitmapSel
        jne     @B
	mov	eax,ds:[edi].OFFBMPINFO.dwBitmapOffset
        cmp	eax,dwBmpOffset
        jne     @B

	; We've found the same selector in cache.
	; We have to free offscreen memory and update cache table.
        push    esi
        push    edi
	call	GN_OffscreenBitmapDelete

AllocSpace:
        ; Hardware specific code approved the bitmap. Now, see
        ; if we can allocate the offscreen memory for it.
        mov     edi,dwBmpOffset
        movzx   edx,es:[edi].DEVBMP.Dibeng.deHeight
        mov     ecx,es:[edi].DEVBMP.Dibeng.deDeltaScan
        imul    ecx,edx
        mov     dwBmpSize,ecx
        or      es:[edi].DEVBMP.Dibeng.deFlags,VRAM OR OFFSCREEN
        mov     eax,ds:[esi].GNBOARDCX.dwOffscreenHeap
        call    GN_HeapAlloc
        sub     ebx,ebx
        or      eax,eax
        jne     Success

        ; The bitmap could not fit into the free offscreen memory.
        ; Look for a bitmap that we can flush out to system memory
        ; that is bigger than the new one we want to put in vidmem.
        lea     edx,ds:[esi].GNBOARDCX.OffscreenBitmapCache
        mov     edi,ds:[esi].GNBOARDCX.pNextOffbmpinfo
@@:
        sub     edi,SIZE OFFBMPINFO
        cmp     edi,edx
        jb      Done
        cmp     ds:[edi].OFFBMPINFO.dwBitmapLength,ecx
        jl      @B

Found:
        ; Is the bitmap still valid?
        mov     eax,ds:[edi].OFFBMPINFO.dwBitmapSel
        call    GN_OffscreenBitmapValidCheck
        or      eax,eax
        jne	SelectorValid

	; Here we have to remove invalid element from the chain.
        push    esi
        push    edi
	call	GN_OffscreenBitmapDelete

	; Try to allocate again
	jmp	AllocSpace

SelectorValid:
        ; Yes it is, so flush it out to system memory and clear
        ; out the ptr to the bitmap header.
        mov     eax,ds:[edi].OFFBMPINFO.dwBitmapSel
	push	es
	mov	es,ax
	push	esi
        mov     esi,ds:[edi].OFFBMPINFO.dwBitmapOffset
        call    GN_FlushOffscreenBitmap
	pop	esi
	pop	es
        mov     ecx,dwBmpSize

RedoAlloc:
        xor     ebx,ebx
        mov     eax,ds:[esi].GNBOARDCX.dwOffscreenHeap
        call    GN_HeapAlloc
        sub     ebx,ebx
        or      eax,eax
        je      Done

Success:
        ; We got the memory for an offscreen bitmap. The selector
        ; for the bits ptr is the same as the screen. The offset
        ; is what we got back from the HeapAlloc.
        mov     ebx,dwBmpOffset
        mov     es:[ebx].DEVBMP.Dibeng.deBitsOffset,eax

        ; Keep track of the new offscreen bitmap in our header list.
        mov     edi,ds:[esi].GNBOARDCX.pNextOffbmpinfo
        mov     ds:[edi].OFFBMPINFO.dwBitmapLength,ecx
        mov     ds:[edi].OFFBMPINFO.dwBitmapVRAMOffset,eax
	sub	edx,edx
        mov     dx,es
        or      edx,3
        mov     ds:[edi].OFFBMPINFO.dwBitmapSel,edx
        mov     ds:[edi].OFFBMPINFO.dwBitmapOffset,ebx
        add     edi,SIZE OFFBMPINFO
        mov     ds:[esi].GNBOARDCX.pNextOffbmpinfo,edi

        ; Minimal time taken to keep some statistics as well.
        inc     ds:[esi].GNBOARDCX.dwOffscreenBitmapsAllocatedSinceLastFlush
        inc     ds:[esi].GNBOARDCX.dwOffscreenBitmapsCurrentlyAllocated
        add     ds:[esi].GNBOARDCX.dwBytesOfOffscreenMemoryCurrentlyAllocatedForBitmaps,ecx
        mov     eax,1
Done:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_OffscreenBitmapDelete
;
; Purpose:      This routine deletes an offscreen bitmap and frees
;               up memory associated with it.
;
; Arguments:
;               ds                      CXSel
;               pGNBoardCXOffset
;               es:edi                  ptr to DEVBMP.
;                                       es:[esi].DIBENGINE.deBitsOffset
;                                       has base address of the bitmap.
;               dwCacheOffset		offset OffscreenBitmapCache if selector invalid
;
; Returns:      None
;
; Preserve:     ds,esi,edi,es
;==============================================================================
DECPROC GN_OffscreenBitmapDelete, PASCAL, FRAME, NEAR
PARMD   pGNBoardCXOffset
PARMD   dwCacheOffset
LOCALD	dwBitsOffset
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNBoardCXOffset

	cmp	dwCacheOffset,0
	je	GetOffsetFromBmp
	mov	edi,dwCacheOffset
	mov	ecx,ds:[edi].OFFBMPINFO.dwBitmapVRAMOffset
	jmp	@F

GetOffsetFromBmp:
        mov     ecx,es:[edi].DEVBMP.Dibeng.deBitsOffset
@@:
	mov	dwBitsOffset,ecx

        ; Call the hardware specific routine to do their thing
        mov     ebx,ds:[esi].GNBOARDCX.pHWBoardCX
	mov	eax,ecx
        call    HW_DeleteOffscreenBitmap

        ; Make sure we have an offscreen heap
        test	ds:[esi].GNBOARDCX.dwOffscreenFlags,NO_OFFS_BMPS
        jnz     Done

        ; Attempt to free the offscreen memory used by the bitmap.
        mov     eax,ds:[esi].GNBOARDCX.dwOffscreenHeap
	mov	ecx,dwBitsOffset
        call    GN_HeapFree

        ; If the memory manager found the allocation, we can update
        ; our statistics on offscreen bitmap memory usage.
        or      eax,eax
        je      @F
        sub     ds:[esi].GNBOARDCX.dwBytesOfOffscreenMemoryCurrentlyAllocatedForBitmaps,ecx
@@:
	cmp	dwCacheOffset,0
	jnz	Found

        ; Search for the offscreen bitmap in the list.
	sub	ebx,ebx
	mov	ecx,edi
        mov     bx,es
        or      ebx,3
        lea     edx,ds:[esi].GNBOARDCX.OffscreenBitmapCache
        mov     edi,ds:[esi].GNBOARDCX.pNextOffbmpinfo
@@:
        sub     edi,SIZE OFFBMPINFO
        cmp     edi,edx
        jb      Done
        cmp     ebx,ds:[edi].OFFBMPINFO.dwBitmapSel
        jne     @B
	mov	eax,ds:[edi].OFFBMPINFO.dwBitmapOffset
        cmp	eax,ecx
        jne     @B

Found:
        ; The bitmap was in our list. Adjust statistics again.
        dec     ds:[esi].GNBOARDCX.dwOffscreenBitmapsCurrentlyAllocated

        ; Now, to remove the bitmap from our list, take the last entry
        ; in the list and move it into the spot that has this current
        ; bitmap info. Then, drop the list count by 1 entry.
        ; and drop the free entry pointer by one array entry.
        mov     ebx,ds:[esi].GNBOARDCX.pNextOffbmpinfo
        sub     ebx,SIZE OFFBMPINFO
        mov     ds:[esi].GNBOARDCX.pNextOffbmpinfo,ebx

	; We don't need to copy if we are the last element.
	cmp	edi,ebx
	je	Done

        .errnz  (SIZE OFFBMPINFO - 10H)
        mov     eax,ds:[ebx + 00H]
        mov     ecx,ds:[ebx + 04H]
        mov     edx,ds:[ebx + 08H]
        mov     ebx,ds:[ebx + 0CH]
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        mov     ds:[edi + 08H],edx
        mov     ds:[edi + 0CH],ebx

Done:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_OffscreenBitmapValidCheck
;
; Purpose:      This routine checks a bitmap header to make sure it is
;               still valid and that the memory doesn't appear to have
;               been allocated to someone else. Unfortunately, the
;               display driver does not always get informed of when
;               memory belonging to bitmaps is deleted, discarded, etc
;               so this routine is necessary.
;
; Arguments:    eax     selector for bitmap header (header at ax:0)
;
; Returns:      eax     0       invalid bitmap header
;                       non-0   valid bitmap header
;
; Preserve:     ds,esi,edi,es
;==============================================================================
DECPROC GN_OffscreenBitmapValidCheck, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   es

        ; Is this even a valid selector?
        lar     edx,eax
        jnz     Fail

        ; The selector must be PRESENT, RING3, DATA
        and     dh,NOT 7
        cmp     dh,0F0H
        jne     Fail

	; The selector base must exceed 7fffffffH
	push	eax
        push    ax
        call    GetSelectorBase
	test	edx,8000H
	pop	eax
	jz	Fail

        ; The selector limit must be large enough to hold
        ; at least a DEVBMP header.
        lsl     ebx,eax
        inc     ebx
        cmp     ebx,SIZE DEVBMP
        jc      Fail

        ; Now we can load the selector without a GPF.
        ; Make sure the first word in the memory block looks like
        ; the start of a DIBENGINE header.
        mov     es,ax
        sub     edx,edx
        cmp     es:[edx].DIBENGINE.deType,5250H
        jne     Fail

        ; Check the selector limit against the total size the bitmap
        ; would take up -- header and the bits.
        movzx   eax,es:[edx].DEVBMP.Dibeng.deWidthBytes
        movzx   edx,es:[edx].DEVBMP.Dibeng.deHeight
        add     eax,3
        and     eax,NOT 3
        imul    eax,edx
        add     eax,SIZE DEVBMP
        cmp     ebx,eax
        jae     @F

Fail:
        sub     eax,eax
@@:
        POPR    es
CLOSEPROC


;==============================================================================
;
; Function:     GN_FlushOffscreenBitmap
;
; Purpose:      This routine will copy an offscreen bitmap into
;               its system memory backup region and adjust the
;               bitmap header appropriately to point to the new
;               location of the bitmap data. The offscreen
;               memory used by the bitmap is freed.
;
; Arguments:    es:esi  ptr to DEVBMP.
;               The bitmap header MUST be valid. No
;               validity checking is performed in here.
;
; Returns:      None
;
; Preserve:     ds,es,gs,esi,edi
;==============================================================================
DECPROC GN_FlushOffscreenBitmap, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   ds,esi,edi,gs

        ; The deDeltaScan of the new system memory copy is always
        ; (deWidthBytes + 3) AND NOT 3 -- the DIBENG assumes this.
        movzx   eax,es:[esi].DEVBMP.Dibeng.deWidthBytes
        mov     edx,es:[esi].DEVBMP.Dibeng.deDeltaScan
        add     eax,3
        and     eax,NOT 3

        ; Compute the dst wrap width and load up src and dst ptrs
        sub     edx,eax
        shr     eax,2
        mov     edi,SIZE DEVBMP
        movzx   ebx,es:[esi].DEVBMP.Dibeng.deHeight
	;--->
	push	esi
        lds     esi,fword ptr es:[esi].DEVBMP.Dibeng.deBitsOffset

        ; Copy the bitmap from offscreen to system memory area
@@:     mov     ecx,eax
        rep     movs dword ptr es:[edi], ds:[esi]
        add     esi,edx
        dec     ebx
        jne     @B

	pop	edi
	;<---

        ; Now we need to delete the offscreen bitmap which will
        ; cause the offscreen memory allocated for it to be released.
        mov     ds,cs:[wFlatDataSel]
        mov     esi,cs:[CSpGNLogicalDevice]
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
	pushd	0
        call    GN_OffscreenBitmapDelete

        ; The DeltaScan and bits ptrs in the bitmap should now be
        ; updated to reflect the new location and bitmap pitch.
        movzx   eax,es:[edi].DEVBMP.Dibeng.deWidthBytes
        add     eax,3
        and     eax,NOT 3
        mov     es:[edi].DIBENGINE.deDeltaScan,eax
        mov     eax,SIZE DEVBMP
        mov     es:[edi].DIBENGINEHDR.deBitsOffset,eax
        mov     es:[edi].DIBENGINEHDR.deBitsSelector,es

        ; Since the bitmap is now in system memory, clear
        ; the offscreen flags and the access routines.
        sub     eax,eax
        and     es:[edi].DIBENGINEHDR.deFlags,NOT (VRAM OR OFFSCREEN)
        mov     es:[edi].DIBENGINEHDR.deBeginAccess,eax
        mov     es:[edi].DIBENGINEHDR.deEndAccess,eax
        POPR    ds,esi,edi,gs
CLOSEPROC

CLOSESEG _NVTEXT32

END

