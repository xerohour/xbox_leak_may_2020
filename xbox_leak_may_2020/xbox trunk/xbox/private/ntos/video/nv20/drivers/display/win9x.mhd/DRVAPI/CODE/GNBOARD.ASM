;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnboard.asm
;
; Purpose:      This file holds init routines for the GNBOARDCX.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include minivdd.inc
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
PUBLIC  szDisplayDeviceName
szDisplayDeviceName     DB      "DISPLAY",0

;==============================================================================
;
; Function:     GN_FindBoardContext
;
; Purpose:      This routine searches to see if a GNBOARDCX exists
;               for the board specified.
;
;		If this routine finds a GNBOARDCX already active on
;		this board, then it establishes the wFlatDataSelector
;		in the code segments. If it does not find one, then
;		the flat data selectors are left untouched.
;
; Arguments:
;               dwBoardDevnode
;
; Returns:      eax             GNBOARDCX ptr
;                               0 if there are no instances yet
;
;		If there are instances, then ds has the context selector
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_FindBoardContext, PASCAL, FRAME, FAR16
PARMD   dwBoardDevnode
OPENPROC
	; We need a flat data selector temporarily
        mov     eax,1
        sub     ebx,ebx
        mov     ecx,-1
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
	mov	ds,ax

	; Now get the global mapping ptr
        call    HW_GetLogdevToBoardPtr

        ; Search the mapping array looking for this board context
        mov     ebx,eax
        mov     ecx,dwBoardDevnode
        sub     eax,eax
Back1:
        cmp     ds:[ebx].LOGDEVTOBOARD.dwBoardDevnodeHandle,0
        je      FreeFlat
        cmp     ecx,ds:[ebx].LOGDEVTOBOARD.dwBoardDevnodeHandle
        jne     @F
        mov     eax,ds:[ebx].LOGDEVTOBOARD.dwGNBoardCXOffset
        jmp     FreeFlat
@@:
        add     ebx,SIZE LOGDEVTOBOARD
        jmp     Back1

FreeFlat:
	mov	dx,ds
	sub	cx,cx
	or	eax,eax
	je	@F
        mov	ecx,ds:[eax].GNBOARDCX.dwFlatDataSelector
        mov     es,wNVTEXT32_Alias_In_NVTEXT16
        mov     es:[wFlatDataSel],cx
        mov     es,wNVTEXT16_Alias_In_NVTEXT16
        mov     es:[wFlatDataSel_In_NVTEXT16],cx
@@:
        mov	ds,cx
	push	eax
	mov	eax,edx
        call    GN_UnmapAndFreeSelector
        pop	eax
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:     GN_CreateBoardContext
;
; Purpose:      This routine initializes a GNBOARDCX structure.
;
;		This routine stablishes the wFlatDataSelector
;		in the code segments.
;
; Arguments:
;
; Returns:      eax     0       failure
;                       else    Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_CreateBoardContext, PASCAL, FRAME, FAR16
PARMD   dwThisLogDeviceHandle
LOCALV  sDispInfo,DISPLAYINFO
OPENPROC
        PUSHR   esi
        sub     esi,esi

        ; Allocate memory for the GNBOARDCX
        pushd   SIZE GNBOARDCX
        call    HW_AllocFlatMemory
        or      eax,eax
        je      DoneIt
        mov     esi,eax

	; We need a flat data selector that everyone can use
        mov     eax,1
        sub     ebx,ebx
        mov     ecx,-1
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt

        ; Put the flat selector in both code segments
        mov     ds,cs:[wNVTEXT32_Alias_In_NVTEXT16]
        mov     ds:[wFlatDataSel],ax
        mov     ds,cs:[wNVTEXT16_Alias_In_NVTEXT16]
        mov     ds:[wFlatDataSel_In_NVTEXT16],ax
        mov	ds,ax
        mov	ds:[esi].GNBOARDCX.dwFlatDataSelector,eax

        ; we are initting. Cleared at end of first modeset.
        mov     ds:[esi].GNBOARDCX.dwInitting,1

        ; Init this to 0
        mov     ds:[esi].GNBOARDCX.dwLogicalDeviceInstancesOnThisBoard,0

        ; DirectX is not active right now
        mov     ds:[esi].GNBOARDCX.dwDirectXActive,0

        ; Crete a HW board context
        push    dwThisLogDeviceHandle
        call    HW_CreateBoardContext
        or      eax,eax
        je      DoneIt
        mov     ds:[esi].GNBOARDCX.pHWBoardCX,eax

        ; Ask RM about free offscreen space on this board
        push    eax
        call    HW_GetLargestOffscreenBlock
        mov     ds:[esi].GNBOARDCX.dwMemorySizeForValidateMode,eax

	; Create a heap
        mov     eax,MAX_OFFSCREEN_BITMAPS
        call    GN_HeapCreate
        mov     ds:[esi].GNBOARDCX.dwOffscreenHeap,eax
        mov     ds:[esi].GNBOARDCX.dwOffscreenFlags,OFFS_HEAP_UNAVAILABLE
        or	eax,eax
        je	DoneIt
        and     ds:[esi].GNBOARDCX.dwOffscreenFlags,NOT OFFS_HEAP_UNAVAILABLE

        push    esi
        call    GN_GetNumCRTCs
        mov     ds:[esi].GNBOARDCX.dwNumCrtcsOnBoard,eax

        ; We want the board path into the registry.
        mov     ax,ss
        shl     eax,10H
        lea     ax,sDispInfo
        push    eax
        pushd   VDD_GET_DISPLAY_CONFIG
        call    GN_VddGetDisplayInfo
        mov     ecx,sDispInfo.diDevNodeHandle
        mov     ds:[esi].GNBOARDCX.dwDeviceNodeHandle,ecx

        ; Build a 16:16 ptr to the display path. We need to be able
        ; to pass these into the modeset dll which is 16bit C code.
        mov     ax,ds
        push    eax
        call    GN_GetSelectorBase
        lea     ebx,[esi].GNBOARDCX.szLocalDisplayBasePath
        add     ebx,eax
        mov     eax,1
        mov     ecx,100H
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
        shl     eax,10H
        mov     ds:[esi].GNBOARDCX.lpLocalDisplayBasePath,eax

        ; Fill in the actual path to our board registry stuff
        push    ds:[esi].GNBOARDCX.dwDeviceNodeHandle
        mov     ax,ds
        push    eax
        lea     ecx,[esi].GNBOARDCX.szLocalDisplayBasePath
        push    ecx
        sub     eax,eax
        mov     ax,SEG szDisplayDeviceName
        push    eax
        mov     ax,OFFSET szDisplayDeviceName
        movzx   eax,ax
        push    eax
        call    GN_GetDeviceLocalBasePath

        ; Init offset to bitmap info
        lea     eax,ds:[esi].GNBOARDCX.OffscreenBitmapCache
        mov     ds:[esi].GNBOARDCX.pNextOffbmpinfo,eax

        ; Clear/Reset the offscreen bitmap manager.
        push    esi
        call    GN_ClearOffscreenBitmapCache

        call    HW_GetLogdevToBoardPtr

        ; Search the mapping array looking for an empty slot
        ; and save off the GNBOARDCX there.
        mov     ebx,eax
        mov     ecx,ds:[esi].GNBOARDCX.dwDeviceNodeHandle
Back1:
        cmp     ds:[ebx].LOGDEVTOBOARD.dwBoardDevnodeHandle,0
        je      FoundIt
        add     ebx,SIZE LOGDEVTOBOARD
        jmp     Back1
FoundIt:
        mov     ds:[ebx].LOGDEVTOBOARD.dwBoardDevnodeHandle,ecx
        mov     ds:[ebx].LOGDEVTOBOARD.dwGNBoardCXOffset,esi

        mov     eax,esi

DoneIt:
        ; If eax is 0, then we failed, BUT if esi is non-0, then
        ; some of the GNBOARDCX was filled in. We must handle
        ; this case and free any portion that got initted.
        or      eax,eax
        jne     ExitIt
        or      esi,esi
        je      ExitIt

        ; GN_DestroyBoardContext can clean up partial inits
        push    esi
        call    GN_DestroyBoardContext
        sub     eax,eax

ExitIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_DestroyBoardContext
;
; Purpose:      This routine destroys a GNBOARDCX structure.
;               You do not need to free its memory. Just free any
;               resources inside the structure.
;
; Arguments:
;               ds                      Selector for the board Context data
;               pGNBoardCXOffset        Offset to the board context data
;
; Returns:      eax     0       failure
;                       else    Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_DestroyBoardContext, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pGNBoardCXOffset
        or      esi,esi
        je      DoneIt

        ; If the DeviceNodeHandle is 0, then we didn't get far enough
        ; in the init to add an entry to the LOGDEVTOBOARD structure.
        cmp     ds:[esi].GNBOARDCX.dwDeviceNodeHandle,0
        je      NextThing

        ; Search the mapping array looking for this GNBOARDCX.
        call    HW_GetLogdevToBoardPtr
        mov     ebx,eax
        mov     ecx,ds:[esi].GNBOARDCX.dwDeviceNodeHandle
Back1:
        cmp     ds:[ebx].LOGDEVTOBOARD.dwBoardDevnodeHandle,0
        je      NextThing
        cmp     ecx,ds:[ebx].LOGDEVTOBOARD.dwBoardDevnodeHandle
        je      FoundIt
        add     ebx,SIZE LOGDEVTOBOARD
        jmp     Back1

FoundIt:
        ; Find the end of the array
        mov     edx,ebx
Back2:
        cmp     ds:[ebx].LOGDEVTOBOARD.dwBoardDevnodeHandle,0
        je      FoundEnd
        add     ebx,SIZE LOGDEVTOBOARD
        jmp     Back2

FoundEnd:
        ; Copy the last entry in the array down into the entry we
        ; are removing. Then zero out the final entry
        sub     ebx,SIZE LOGDEVTOBOARD
        mov     eax,ds:[ebx].LOGDEVTOBOARD.dwGNBoardCXOffset
        mov     ds:[edx].LOGDEVTOBOARD.dwGNBoardCXOffset,eax
        mov     eax,ds:[ebx].LOGDEVTOBOARD.dwBoardDevnodeHandle
        mov     ds:[edx].LOGDEVTOBOARD.dwBoardDevnodeHandle,eax
        sub     eax,eax
        mov     ds:[ebx].LOGDEVTOBOARD.dwGNBoardCXOffset,eax
        mov     ds:[ebx].LOGDEVTOBOARD.dwBoardDevnodeHandle,eax

NextThing:
        ; Free up the 16:16 base path ptr if need be
        cmp     ds:[esi].GNBOARDCX.lpLocalDisplayBasePath,0
        je      @f
        mov     eax,ds:[esi].GNBOARDCX.lpLocalDisplayBasePath
        shr     eax,10H
        call    GN_UnmapAndFreeSelector
        mov     ds:[esi].GNBOARDCX.lpLocalDisplayBasePath,0
@@:
        mov     eax,ds:[esi].GNBOARDCX.dwOffscreenHeap
        or	eax,eax
        je	@F
        call    GN_HeapDestroy
        mov     ds:[esi].GNBOARDCX.dwOffscreenHeap,0
@@:
        cmp     ds:[esi].GNBOARDCX.pHWBoardCX,0
        je      @F
        push    ds:[esi].GNBOARDCX.pHWBoardCX
        call    HW_DestroyBoardContext
        mov     ds:[esi].GNBOARDCX.pHWBoardCX,0
@@:

	; The following may have been allocated in the
	; SHARED MEMORY escapes. We need to free the
	; memory blocks if so.
	sub	ecx,ecx
LoopSM:
	mov	eax,ds:[esi].GNBOARDCX.sSharedMemBlocks[ecx * 4].dwData
	or	eax,eax
	je	@F
	mov	ds:[esi].GNBOARDCX.sSharedMemBlocks[ecx * 4].dwData,0
	PUSHR	ecx
	shr	eax,10H
	call	GN_MemoryFree
	POPR	ecx
@@:	inc	ecx
	cmp	ecx,MAX_SHARED_MEM_BLOCKS
	jc	LoopSM

	; Free the flat data selector
	mov	eax,ds:[esi].GNBOARDCX.dwFlatDataSelector
	or	eax,eax
	je	@F

	; To prevent the system chash we have to change DS
	; because it points to the selector we are going free.
	sub	ebx,ebx
	mov	ds,bx
	call	GN_UnmapAndFreeSelector

@@:
        push    esi
        call    HW_FreeFlatMemory

DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_AttachLogicalDeviceToBoard
;
; Purpose:      This routine attaches a logical device to a given
;               board by placing the logical device into the array
;               of GNLOGDEVCXs in the GNBOARDCX structure.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;
; Returns:      eax                     0 failure
;                                       non-0 success
;                                       In this case, ecx is the
;                                       0 based instance number
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_AttachLogicalDeviceToBoard, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   pGNLogdevCXOffset
OPENPROC
        mov     ebx,pGNBoardCXOffset
        sub     edx,edx
        sub     eax,eax
@@:
        cmp     edx,ds:[ebx].GNBOARDCX.pGNLogdevCX[eax * 4]
        je      Found
        inc     eax
        cmp     eax,ds:[ebx].GNBOARDCX.dwNumCrtcsOnBoard
        jc      @B

        ; UhOh ... What is wrong here? This shouldn't happen.
        sub     eax,eax
        jmp     DoneIt
Found:
        mov     ecx,pGNLogdevCXOffset
        mov     ds:[ebx].GNBOARDCX.pGNLogdevCX[eax * 4],ecx
        inc     ds:[ebx].GNBOARDCX.dwLogicalDeviceInstancesOnThisBoard
        mov     ecx,eax
        mov     eax,1
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:     GN_ReattachLogicalDeviceToBoard
;
; Purpose:      This routine searches for the logical device / board
;               combination and then updates the GNBOARDCXs knowledge
;               of what GNLOGDEVCXs are attached to it.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;               pGNLogdevCXOffsetOld    GNLOGDEVCX ptr
;               pGNLogdevCXOffsetNew    GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_ReattachLogicalDeviceToBoard, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   pGNLogdevCXOffsetOld
PARMD   pGNLogdevCXOffsetNew
OPENPROC
        mov     ebx,pGNBoardCXOffset
        mov     edx,pGNLogdevCXOffsetOld
        sub     eax,eax
@@:
        cmp     edx,ds:[ebx].GNBOARDCX.pGNLogdevCX[eax * 4]
        je      Found
        inc     eax
        cmp     eax,ds:[ebx].GNBOARDCX.dwNumCrtcsOnBoard
        jc      @B

        ; UhOh ... What is wrong here? This shouldn't happen.
        ; Cross your fingers and return slot 1 ....
        mov     eax,1
Found:
        mov     ecx,pGNLogdevCXOffsetNew
        mov     ds:[ebx].GNBOARDCX.pGNLogdevCX[eax * 4],ecx
CLOSEPROC


;==============================================================================
;
; Function:     GN_DetachLogicalDeviceFromBoard
;
; Purpose:      This routine detaches a logical device from a
;               given board.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;
; Returns:      eax     Number of logical devices left attached to
;                       the board after this one detaches
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_DetachLogicalDeviceFromBoard, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   pGNLogdevCXOffset
OPENPROC
        mov     ebx,pGNBoardCXOffset
        mov     edx,pGNLogdevCXOffset
        sub     eax,eax
@@:
        cmp     edx,ds:[ebx].GNBOARDCX.pGNLogdevCX[eax * 4]
        je      Found
        inc     eax
        cmp     eax,ds:[ebx].GNBOARDCX.dwNumCrtcsOnBoard
        jc      @B
        jmp     DoneIt

Found:
        mov     ds:[ebx].GNBOARDCX.pGNLogdevCX[eax * 4],0

        ; One less logical device on this board
        dec     ds:[ebx].GNBOARDCX.dwLogicalDeviceInstancesOnThisBoard
DoneIt:
        mov     eax,ds:[ebx].GNBOARDCX.dwLogicalDeviceInstancesOnThisBoard
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetPciDeviceID
;
; Purpose:      This routine gets the PCI Device ID of the board.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;
; Returns:      eax     Frame buffer selector
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetPciDeviceID, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
OPENPROC
        mov     ebx,pGNBoardCXOffset
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        call    HW_GetPciDeviceID
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetFrameBufferSelector
;
; Purpose:      This routine gets the frame buffer selector.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;
; Returns:      eax     Frame buffer selector
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetFrameBufferSelector, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
OPENPROC
        mov     ebx,pGNBoardCXOffset
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        call    HW_GetFrameBufferSelector
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetNumCRTCs
;
; Purpose:      This routine gets the number of physical CRTCs
;               on the board.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;
; Returns:      eax     Frame buffer selector
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetNumCRTCs, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
OPENPROC
        mov     ebx,pGNBoardCXOffset
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        call    HW_GetNumCRTCs
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetBoardRegistryPath
;
; Purpose:      This routine gets the registry path to the board
;               info.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;
; Returns:      eax     ptr to a REGDATA structure describing registry path
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetBoardRegistryPath, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
OPENPROC
        mov     ebx,pGNBoardCXOffset
        mov     eax,ds:[ebx].GNBOARDCX.lpLocalDisplayBasePath
CLOSEPROC


CLOSESEG _NVTEXT16

END

