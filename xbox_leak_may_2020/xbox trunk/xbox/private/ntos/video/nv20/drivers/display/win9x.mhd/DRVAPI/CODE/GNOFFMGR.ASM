;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation
;
; File:         gnoffmgr.asm
;
; Purpose:      This file holds the routines that manage offscreen
;               memory.
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

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     GN_ControlOffscreenAllocations
;
; Purpose:      This routine gets called to either allocate memory
;               for offscreen allocations or free it up.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;               dwFlag                  TRUE then enable, FALSE then disable
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_ControlOffscreenAllocations, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   dwFlag
LOCALD	dwLogdevCount
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNBoardCXOffset

        cmp     dwFlag,0
        jne     DoEnable

DoDisable:
        test   	ds:[esi].GNBOARDCX.dwOffscreenFlags,OFFS_HEAP_UNAVAILABLE
        jnz	DoneIt

        ; Flush all offscreen bitmaps
        push    pGNBoardCXOffset
        call    GN_ClearOffscreenBitmapCache

	; OK, this is funky. We have to run through all the logical
	; devices that are attached to this board and tell them that
	; the offscreen heap is disappearing.
	lea	edi,[esi].GNBOARDCX.pGNLogdevCX
	mov	dwLogdevCount,0
Loop1:
	mov	eax,ds:[edi]
	or	eax,eax
	je	@F
	push	eax
	call	GN_OffscreenHeapDisappearing
@@:
	add	edi,4
	inc	dwLogdevCount
	mov	ecx,dwLogdevCount
	cmp	ecx,MAX_ATTACHED_CRTCS
	jc	Loop1

        ; Mark the offscreen heap as unavailable
        or     	ds:[esi].GNBOARDCX.dwOffscreenFlags,OFFS_HEAP_UNAVAILABLE

        ; Call the hardware to have it free the offscreen block
        push    ds:[esi].GNBOARDCX.pHWBoardCX
        push    ds:[esi].GNBOARDCX.dwOffscreenBaseAddress
        call    HW_FreeOffscreenBlock
        mov     ds:[esi].GNBOARDCX.dwOffscreenBaseAddress,0
        mov     ds:[esi].GNBOARDCX.dwOffscreenLength,0
        jmp     DoneIt

DoEnable:
        ; If we are already enabled, then ignore the call
        test   	ds:[esi].GNBOARDCX.dwOffscreenFlags,OFFS_HEAP_UNAVAILABLE
        jz	DoneIt

        ; Call the hardware to get the largest offscreen block.
        push    ds:[esi].GNBOARDCX.pHWBoardCX
        call    HW_GetLargestOffscreenBlock

        ; Allocate it
        push    ds:[esi].GNBOARDCX.pHWBoardCX
        push    eax
        call    HW_AllocOffscreenBlock

        ; Set up an offscreen heap if there was any memory
        cmp	eax,-1
        je      DoneIt

        mov     ds:[esi].GNBOARDCX.dwOffscreenBaseAddress,eax
        mov     ds:[esi].GNBOARDCX.dwOffscreenLength,ebx

	mov	ecx,ebx
	mov	ebx,eax
        mov    	eax,ds:[esi].GNBOARDCX.dwOffscreenHeap
        call    GN_HeapInit

        and    	ds:[esi].GNBOARDCX.dwOffscreenFlags,NOT OFFS_HEAP_UNAVAILABLE

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_OffscreenAllocationsLock
;
; Purpose:      This function increments a lock count which begins at 0.
;               When the lock count goes from 0 to 1, all offscreen
;               allocations are flushed and offscreen usage by the display
;               driver is inhibited until the lock count goes to 0 via
;               matching calls to GN_OffscreenAllocationsUnlock.
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     All registers
;==============================================================================
DECPROC GN_OffscreenAllocationsLock, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   alld,ds,es,fs,gs
        mov     ds,wFlatDataSel_In_NVTEXT16
        mov     esi,CSpGNLogicalDevice_In_NVTEXT16
        mov     edi,ds:[esi].GNLOGDEVCX.pGNBoardCX
        cmp     ds:[edi].GNBOARDCX.dwOffscreenLockCount,0
        jne     @F

        ; Flush the offscreen list
        push    edi
        call    GN_ClearOffscreenBitmapCache
@@:
        or      ds:[edi].GNBOARDCX.dwOffscreenFlags,OFFS_ALLOCS_BLOCKED
        inc     ds:[edi].GNBOARDCX.dwOffscreenLockCount
        POPR    alld,ds,es,fs,gs
CLOSEPROC


;==============================================================================
;
; Function:     GN_OffscreenAllocationsUnlock
;
; Purpose:      See GN_OffscreenAllocationsLock for details.
;
; Arguments:    None
;
; Returns:      eax     Current Lock count
;
; Preserve:     All registers except eax
;==============================================================================
DECPROC GN_OffscreenAllocationsUnlock, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi
        mov     ds,wFlatDataSel_In_NVTEXT16
        mov     esi,CSpGNLogicalDevice_In_NVTEXT16
        mov     edi,ds:[esi].GNLOGDEVCX.pGNBoardCX
        dec     ds:[edi].GNBOARDCX.dwOffscreenLockCount
        jne     @F
        and     ds:[edi].GNBOARDCX.dwOffscreenFlags,NOT OFFS_ALLOCS_BLOCKED
@@:
        POPR    ds,esi,edi
CLOSEPROC

CLOSESEG _NVTEXT16

END

