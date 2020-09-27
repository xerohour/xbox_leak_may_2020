;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwboard2.asm
;
; Purpose:      This file contains all the hardware specific BOARD
;               routines for run-time. The board specific init
;               routines are in hwboard.asm.
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include sysdata.inc
include gngdi.inc
include hwgdi.inc
include files.inc
include nvcm.inc
include nvreg.inc
include hwmacs.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc
include nvos.inc
include modeext.inc
include ..\..\..\..\..\sdk\nvidia\inc\nvrmarch.inc
include ..\..\..\..\..\sdk\nvidia\inc\rddapi.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     HW_BoardMessage
;
; Purpose:      This routine gets called whenver there is an
;               message to send to all objects. There are
;               a few different messages.  See gngdi.inc
;               for details.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwMessage               which message
;               dwData1                 additional data that depends
;                                       upon the message type
;               dwData2                 additional data that depends
;                                       upon the message type
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_BoardMessage, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwMessage
PARMD   dwData1
PARMD   dwData2
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        mov     eax,dwMessage
        cmp     eax,MSG_PREMODESET
        je      PreMS
        cmp     eax,MSG_POSTMODESET
        je      PostMS
        cmp     eax,MSG_DIRECTX_ENUM
        je      DXEnum
        cmp     eax,MSG_DIRECTXON
        je      DXOn
        cmp     eax,MSG_DIRECTXOFF
        je      DXOff
        cmp     eax,MSG_DOSON
        je      DosOn
        cmp     eax,MSG_DOSOFF
        je      DosOff
        cmp     eax,MSG_POWERON
        je      PowerOn
        cmp     eax,MSG_POWEROFF
        je      PowerOff
        cmp     eax,MSG_DISABLE
        je      DrvDis
        jmp     DoneIt

PreMS:
        ; OpenGL mode switch count must be changed to an odd
        ; number before setting the mode.
        add     ds:[esi].HWBOARDCX.dwModeSwitchCount,2
        or      ds:[esi].HWBOARDCX.dwModeSwitchCount,1

        ; Wait until everyone goes idle
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,esi,eax

        ; JKTODO - Talk to the RM guys about getting rid of this.
        ; We can't possibly need it.  One thing that happens right
        ; now is that if the RM sees this call come before a heap
        ; allocation, then they treat the heap as an NV11 type heap
        ; where nothing goes away on a modeset. But I'm sure we can
        ; work out some better strategy than using this call.
        push    esi
        call    Enable_HiRes
        jmp     DoneIt

PostMS:
        ; Sync with other channels
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,esi,eax

        ; Reset the two offsets and pitches
        mov     eax,ds:[esi].HWBOARDCX.dwMinContextSurface2dPitch
        imul    eax,10001H
        mov     ds:[esi].HWBOARDCX.dwContextSurface2dDstSrcPitch,eax
        mov     eax,0FFFF0000h
        mov     ds:[esi].HWBOARDCX.dwContextSurface2dSrcOffset,eax
        mov     ds:[esi].HWBOARDCX.dwContextSurface2dDstOffset,eax
        push    esi
        push    eax
        mov     ebx,dwData1
        push    ds:[ebx].DIBENGINEHDR.deDeltaScan
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        call    SetDstBaseAndPitchToDsEdi
        SETPUTPTR ds,esi,edi

        ; OpenGL mode switch should be bumped to next even number
        add     ds:[esi].HWBOARDCX.dwModeSwitchCount,2
        and     ds:[esi].HWBOARDCX.dwModeSwitchCount,-2

        ; We are no longer initting after the first modeset
        mov     ds:[esi].HWBOARDCX.dwInitting,0
        jmp     DoneIt

DXEnum:
DXOn:
DXOff:
        jmp     DoneIt

DosOff:
PowerOn:
        push    esi
        call    Enable_HiRes

        add     ds:[esi].HWBOARDCX.dwModeSwitchCount,2
        and     ds:[esi].HWBOARDCX.dwModeSwitchCount,-2
        jmp     DoneIt

DosOn:
PowerOff:
        push    esi
        call    Enable_VGA

        ; OpenGL mode switch count must be changed to an odd
        ; number before setting the mode or while in DOS or POWERDOWN.
        add     ds:[esi].HWBOARDCX.dwModeSwitchCount,2
        or      ds:[esi].HWBOARDCX.dwModeSwitchCount,1
        jmp     DoneIt

DrvDis:
        ; Always enable VGA mode for the boards with single head.
        cmp     ds:[esi].HWBOARDCX.dwNumCRTCsOnBoard,1
        je      @F

        ; Check for the secondary device to prevent FIFO disable.
        ; dwData2 = dwThisLogicalDeviceHandle.
        cmp     dwData2,1
        je      @F
        test    dwData2, 03H
        jnz     DoneIt

@@:
        ; OpenGL mode switch count must be changed to an odd
        ; number before setting the mode or while in DOS.
        add     ds:[esi].HWBOARDCX.dwModeSwitchCount,2
        or      ds:[esi].HWBOARDCX.dwModeSwitchCount,1

        push    esi
        call    Enable_VGA
        jmp     DoneIt

DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     Enable_HiRes
;
; Purpose:      This routine calls the ResMan to disable VGA (enable hires)
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC Enable_HiRes, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
OPENPROC
        PUSHR   esi,edi

        ; Tell RM to disable VGA
        pushd   0
        mov     eax,pHWBoardCXOffset
        push    ds:[eax].HWBOARDCX.dwDeviceNum
        call    FFP16 PTR lpfnNvConfigVga

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     Enable_VGA
;
; Purpose:      This routine calls the ResMan to enable VGA (disable hires)
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC Enable_VGA, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
OPENPROC
        PUSHR   esi,edi

        ; Tell RM to enable VGA
        pushd   1
        mov     eax,pHWBoardCXOffset
        push    ds:[eax].HWBOARDCX.dwDeviceNum
        call    FFP16 PTR lpfnNvConfigVga

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetLargestOffscreenBlock
;
; Purpose:      This routine returns the size in bytes of the
;               largest contiguous offscreen block.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:
;               eax     length in bytes of offscreen block
;               ebx     offset in bytes of offscreen block
;
;               eax can be 0 if there is no offscreen memory
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetLargestOffscreenBlock, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
LOCALV  heapParams,NVIOCONTROL_PARAM
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        mov     eax,ds:[esi].HWBOARDCX.dwRootHandle
        mov     heapParams.NVIOCONTROL_hRoot,eax
        mov     heapParams.NVIOCONTROL_hObjectParent,NV_WIN_DEVICE
        mov     heapParams.NVIOCONTROL_owner,GDI2D_HEAP_OWNER
        mov     heapParams.NVIOCONTROL_function,NVIOCONTROL_HEAP_INFO

        pushd   NVIOCONTROL_ARCH_HEAP
        mov     ax,ss
        push    ax
        lea     ax,heapParams
        push    ax
        call    FFP16 PTR lpfnNvIoControl

        ; Return the correct things
        ; The size field is the size of the largest linearly
        ; contguous free block.
        mov     eax,heapParams.NVIOCONTROL_size
        mov     ebx,heapParams.NVIOCONTROL_offset
        POPR    esi
CLOSEPROC

;==============================================================================
;
; Function:     HW_GetLargestOffscreenSpace
;
; Purpose:      This routine returns the size in bytes of the
;               largest TWO contiguous offscreen spaces assuming
;		that the allocations at offset1 and offset2 are
;		free (note, the offset1 and offset2 allocations
;		are NOT freed, but the contiguous block sizes
;		returned are what they would be IF THE TWO
;		offets were freed.)
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               offset1                 framebuffer offset or -1
;               offset2                 offscreen heap offset or -1
;
; Returns:
;               eax     length of largest contiguous offscreen
;			block in bytes. (can be 0, if none available)
;               eax     length of second largest contiguous offscreen
;			block in bytes. (can be 0, if none available)
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetLargestOffscreenSpace, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   offset1
PARMD   offset2
LOCALV  heapParams,NVIOCONTROL_PARAM
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        mov     eax,ds:[esi].HWBOARDCX.dwRootHandle
        mov     heapParams.NVIOCONTROL_hRoot,eax
        mov     heapParams.NVIOCONTROL_hObjectParent,NV_WIN_DEVICE
        mov     heapParams.NVIOCONTROL_owner,GDI2D_HEAP_OWNER
        mov     heapParams.NVIOCONTROL_function,NVIOCONTROL_HEAP_INFO_FREE_BLOCKS
        mov     eax,offset1
        mov     heapParams.NVIOCONTROL_offset,eax
        mov     eax,offset2
        mov     heapParams.NVIOCONTROL_limit,eax

        pushd   NVIOCONTROL_ARCH_HEAP
        mov     ax,ss
        push    ax
        lea     ax,heapParams
        push    ax
        call    FFP16 PTR lpfnNvIoControl

        ; Return the correct things
        ; The size field is the size of the largest linearly
        ; contguous free block.
        mov     eax,heapParams.NVIOCONTROL_size
        sub	ebx,ebx
        POPR    esi
CLOSEPROC

;==============================================================================
;
; Function:     HW_AllocOffscreenBlock
;
; Purpose:      This routine allocates a block of offscreen
;               memory the size requested.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwSizeInBytesToAlloc
;
; Returns:
;               eax     offset into video memory of start of offscreen block
;               ebx     length in bytes of offscreen block
;
;               eax can be 0 if there isn't a contguous block
;               of the size requested.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_AllocOffscreenBlock, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwSizeInBytesToAlloc
OPENPROC
        push    pHWBoardCXOffset
        pushd   NVIOCONTROL_HEAP_ALLOC_SIZE
        pushd   GDI2D_HEAP_OWNER
        pushd   NVIOCONTROL_TYPE_IMAGE
        push    dwSizeInBytesToAlloc
        call    NV_OffscreenHeapAlloc
CLOSEPROC


;==============================================================================
;
; Function:     HW_FreeOffscreenBlock
;
; Purpose:      This routine frees a block of offscreen memory
;               that was allocated via HW_AllocOffscreenBlock.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwOffsetToBlockStart
;
; Returns:
;               None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_FreeOffscreenBlock, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwOffsetToBlockStart
OPENPROC
        ; Comment to avoid assembler error.
        push    pHWBoardCXOffset
        pushd   GDI2D_HEAP_OWNER
        push    dwOffsetToBlockStart
        call    NV_OffscreenHeapFree
CLOSEPROC



;==============================================================================
;
; Function:     HW_OffscreenHeapPurge
;
; Purpose:      This function purges the offscreen heap of all
;               allocations whose owner is GDI2D_HEAP_OWNER.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_OffscreenHeapPurge, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
OPENPROC
        push    pHWBoardCXOffset
        pushd   GDI2D_HEAP_OWNER
        call    NV_OffscreenHeapPurge
CLOSEPROC


DX_HEAP_ID      TEXTEQU <'NVDX'>
;==============================================================================
;
; Function:     HW_DxHeapPurge
;
; Purpose:      This function purges the offscreen heap of all
;               allocations whose owner is DX_HEAP_ID.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_DxHeapPurge, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
OPENPROC
        push    pHWBoardCXOffset
        pushd   DX_HEAP_ID
        call    NV_OffscreenHeapPurge
CLOSEPROC


;==============================================================================
;
; Function:     HW_HeapPurgeAllExceptPrimarySurface
;
; Purpose:      This routine purges everything except for a non-NVRM primary
;               surface.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_HeapPurgeAllExceptPrimarySurface, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
LOCALV  heapParams,NVIOCONTROL_PARAM
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        mov     eax,ds:[esi].HWBOARDCX.dwRootHandle
        mov     heapParams.NVIOCONTROL_hRoot,eax
        mov     heapParams.NVIOCONTROL_hObjectParent,NV_WIN_DEVICE
        mov     heapParams.NVIOCONTROL_owner,GDI2D_HEAP_OWNER
        mov     heapParams.NVIOCONTROL_function,NVOS11_HEAP_DESTROY

        pushd   NVIOCONTROL_ARCH_HEAP
        mov     ax,ss
        push    ax
        lea     ax,heapParams
        push    ax
        call    FFP16 PTR lpfnNvIoControl

        ; Now that everything is purged, we need to realloc 'NVBD'.
        call    NV_AllocRequiredMemory

        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_AllocPrimarySurface
;
; Purpose:      This routine allocates a block of offscreen
;               memory for a primary surface.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwSizeInBytesToAlloc
;
; Returns:
;               eax     offset into video memory of start of offscreen block
;                       (-1 if failure)
;               ebx     length in bytes of offscreen block
;
;
;               This routine really can't fail. Do whatever
;               you can to make it succeed.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_AllocPrimarySurface, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwPitch
PARMD   dwHeight
PARMD   dwCRTCIndex
LOCALV  heapParams,NVIOCONTROL_PARAM
OPENPROC
        push    pHWBoardCXOffset
        pushd   NVIOCONTROL_HEAP_ALLOC_TILED_PITCH_HEIGHT
        pushd   GDI2D_DESKTOP_OWNER
        mov     eax,dwCRTCIndex
        shl     eax,31
        or      eax,NVIOCONTROL_TYPE_PRIMARY
        push    eax
        mov     eax,dwHeight
        shl     eax,10H
        mov     ax,word ptr dwPitch
        push    eax
        call    NV_OffscreenHeapAlloc
CLOSEPROC


;==============================================================================
;
; Function:     HW_FreePrimarySurface
;
; Purpose:      This routine frees a block of offscreen
;               memory that is used as an offscreen surface.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwOffsetToBlockStart    vram offset to start of block to free
;
; Returns:
;               None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_FreePrimarySurface, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwOffsetToBlockStart
OPENPROC
        push    pHWBoardCXOffset
        pushd   GDI2D_DESKTOP_OWNER
        push    dwOffsetToBlockStart
        call    NV_OffscreenHeapFree
CLOSEPROC


;==============================================================================
;
; Function:     HW_PrimarySurfaceHeapPurge
;
; Purpose:      This routine purges all primary surface allocations
;               from video memory.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_PrimarySurfaceHeapPurge, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
LOCALV  heapParams,NVIOCONTROL_PARAM
OPENPROC
        push    pHWBoardCXOffset
        pushd   GDI2D_DESKTOP_OWNER
        call    NV_OffscreenHeapPurge
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetPciDeviceID
;
; Purpose:      This function gets the PCI Device ID for this board.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:
;               eax                     PCI Device ID
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetPciDeviceID, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset
        pushd   NV_CFG_PCI_ID
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        call    FFP16 PTR lpfnNvConfigGet
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetFrameBufferSelector
;
; Purpose:      This function gets the frame buffer selector.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:
;               eax                     frame buffer selector
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetFrameBufferSelector, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset
        mov     eax,ds:[esi].HWBOARDCX.dwVideoMemorySel
        POPR    esi
CLOSEPROC

;==============================================================================
;
; Function:     HW_GetModeDimensions
;
; Purpose:      This function gets the pitch and the amount of memory
;               used for the mode specified.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwXResolution           Xresolution of mode
;               dwYResolution           Yresolution of mode
;               dwBitsPerPixel          Bits Per Pixel of mode
;
; Returns:      eax             Pitch in bytes for the mode
;               ebx             Total amount of memory in bytes for the mode
;                               (including tiling)
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetModeDimensions, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwXResolution
PARMD   dwYResolution
PARMD   dwBitsPerPixel
LOCALV  Params,NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS
OPENPROC

        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        ; Get the parameters for NV_CFGEX_GET_SURFACE_PITCH from stack.
        mov     eax,dwXResolution
        mov     Params.dwWidth,eax
        mov     eax,dwYResolution
        mov     Params.Height,eax
        mov     eax,dwBitsPerPixel
        mov     Params.Depth,eax
        mov     ax,ss
        shl     eax,10H
        lea     ax,Params
        mov     ebx,SIZE NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS
        mov     ecx,NV_CFGEX_GET_SURFACE_DIMENSIONS
        call    NV_ConfigExCall

        ; Return pitch in DX:AX
        mov     eax,Params.Pitch
        mov     ebx,Params.dwSize

		cmp		ds:[esi].HWBOARDCX.dwStereoConnectionType, INTERLACED_STEREO
		jnz		@F
		shl		eax, 1
		shl		ebx, 1
@@:

        POPR    esi

CLOSEPROC


;==============================================================================
;
; Function:     HW_GetNumCRTCs
;
; Purpose:      This routine returns the number of CRTCs on the
;               board.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:      eax     Num total CRTCs on the board
;               ebx     number of available CRTCs
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetNumCRTCs, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset
        mov     ecx,ds:[esi].HWBOARDCX.dwNumCRTCsOnBoard
        mov     eax,1
        sub     ebx,ebx
Back1:
        test    eax,ds:[esi].HWBOARDCX.dwCRTCsAllocatedFlags
        jnz     @F
        inc     ebx
@@:     shl     eax,1
        dec     ecx
        jne     Back1

        mov     eax,ds:[esi].HWBOARDCX.dwNumCRTCsOnBoard
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     NV_ConfigExCall
;
; Purpose:      This function returns TRUE if the monitor is connected
;               and FALSE if it is not.
;
; Arguments:
;               ds:esi  HWBOARDCX
;               eax     16:16 ptr to particular EX structure
;               ebx     size of particular ex structure in bytes
;               ecx     index of EX function
;
; Returns:      eax     status
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_ConfigExCall, PASCAL, FRAME, FAR32
LOCALV  sExGetParams,NVOS_CONFIG_GET_EX_PARAMS
OPENPROC
        ; Get client
        mov     sExGetParams.paramSize,ebx
        mov     sExGetParams.index,ecx
        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     sExGetParams.hClient,ecx
        mov     ecx,ds:[esi].HWBOARDCX.dwhDevice
        mov     sExGetParams.hDevice,ecx

        ; Get flat ptr to EX structure
        push    eax
        shr     eax,10H
        push    eax
        call    GN_GetSelectorBase
        pop     ebx
        and     ebx,0FFFFh
        add     eax,ebx
        mov     sExGetParams.paramStructPtr,eax

        ; Call ConfigGetEx
        mov     ax,ss
        shl     eax,10H
        lea     ax,sExGetParams
        push    eax
        call    FFP16 PTR lpfnArch_Nv04ConfigGetEx

        mov     eax,sExGetParams.status
CLOSEPROC

;==============================================================================
;
; Function:     NV_ConfigExSetCall
;
; Purpose:      This function returns TRUE if the monitor is connected
;               and FALSE if it is not.
;
; Arguments:
;               ds:esi  HWBOARDCX
;               eax     16:16 ptr to particular EX structure
;               ebx     size of particular ex structure in bytes
;               ecx     index of EX function
;
; Returns:      eax     status
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_ConfigExSetCall, PASCAL, FRAME, FAR32
LOCALV  sExSetParams,NVOS_CONFIG_SET_EX_PARAMS
OPENPROC
        ; Get client
        mov     sExSetParams.paramSize,ebx
        mov     sExSetParams.index,ecx
        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     sExSetParams.hClient,ecx
        mov     ecx,ds:[esi].HWBOARDCX.dwhDevice
        mov     sExSetParams.hDevice,ecx

        ; Get flat ptr to EX structure
        push    eax
        shr     eax,10H
        push    eax
        call    GN_GetSelectorBase
        pop     ebx
        and     ebx,0FFFFh
        add     eax,ebx
        mov     sExSetParams.paramStructPtr,eax

        ; Call ConfigSetEx
        mov     ax,ss
        shl     eax,10H
        lea     ax,sExSetParams
        push    eax
        call    FFP16 PTR lpfnArch_Nv04ConfigSetEx

        mov     eax,sExSetParams.status
CLOSEPROC


;==============================================================================
;
; Function:     NV_OffscreenHeapAlloc
;
; Purpose:      This function allocates from the offscreen heap.
;
; Arguments:
;       ds                      Context Selector
;       pHWBoardCXOffset        HWBOARDCX ptr
;       dwFunction              what type of allocation
;                               like NVIOCONTROL_HEAP_ALLOC_SIZE or
;       dwOwnerID               ID you want for the owner
;       dwAllocType             heapParams.type value
;                               Like:  NVIOCONTROL_TYPE_CURSOR, etc.
;       dwSizeInBytes           if type is NVIOCONTROL_HEAP_ALLOC_SIZE,
;                               then this is num bytes for allocation
;                               if type is NVIOCONTROL_TYPE_PRIMARY
;                               then high word is height and low
;                               word is pitch in bytes
;
; Returns:      eax     video memory offset to allocation
;               ebx     size of allocation (may be bigger than requested)
;               ecx     number of free bytes left in the heap
;
;               If the allocation fails, then eax = -1.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_OffscreenHeapAlloc, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwFunction
PARMD   dwOwnerID
PARMD   dwAllocType
PARMD   dwSizeInBytes
LOCALV  heapParams,NVIOCONTROL_PARAM
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        mov     eax,dwFunction
        mov     heapParams.NVIOCONTROL_function,eax
        mov     eax,ds:[esi].HWBOARDCX.dwRootHandle
        mov     heapParams.NVIOCONTROL_hRoot,eax
        mov     eax,ds:[esi].HWBOARDCX.dwhDevice
        mov     heapParams.NVIOCONTROL_hObjectParent,eax
        mov     eax,dwOwnerID
        mov     heapParams.NVIOCONTROL_owner,eax
        mov     ebx,dwAllocType
        mov     heapParams.NVIOCONTROL_type,ebx
        mov     ecx,dwSizeInBytes
        mov     heapParams.NVIOCONTROL_size,ecx
        cmp     dwFunction,NVIOCONTROL_HEAP_ALLOC_SIZE
        je      @F
        movzx   eax,cx
        shr     ecx,10H
        mov     heapParams.NVIOCONTROL_pitch,eax
        mov     heapParams.NVIOCONTROL_height,ecx
@@:
        pushd   NVIOCONTROL_ARCH_HEAP
        mov     ax,ss
        push    ax
        lea     ax,heapParams
        push    ax
        call    FFP16 PTR lpfnNvIoControl

        mov     eax,-1
        cmp     heapParams.NVIOCONTROL_status,NVIOCONTROL_STATUS_SUCCESS
        jne     Done

        ; Return the correct things
        mov     eax,heapParams.NVIOCONTROL_offset
        mov     ebx,heapParams.NVIOCONTROL_limit
        inc     ebx
        mov     ecx,heapParams.NVIOCONTROL_free
Done:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     NV_OffscreenHeapFree
;
; Purpose:      This routine frees a block of offscreen memory
;               that was allocated via HW_AllocOffscreenBlock.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwOwnerID               ID you want for the owner
;               dwOffsetToBlockStart
;
; Returns:
;               None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_OffscreenHeapFree, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwOwnerID
PARMD   dwOffsetToBlockStart
LOCALV  heapParams,NVIOCONTROL_PARAM
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     heapParams.NVIOCONTROL_hRoot,ecx
        mov     eax,ds:[esi].HWBOARDCX.dwhDevice
        mov     heapParams.NVIOCONTROL_hObjectParent,eax
        mov     heapParams.NVIOCONTROL_function,NVIOCONTROL_HEAP_FREE
        mov     eax,dwOwnerID
        mov     heapParams.NVIOCONTROL_owner,eax
        mov     eax,dwOffsetToBlockStart
        mov     heapParams.NVIOCONTROL_offset,eax
        pushd   NVIOCONTROL_ARCH_HEAP
        mov     ax,ss
        push    ax
        lea     ax,heapParams
        push    ax
        call    FFP16 PTR lpfnNvIoControl

        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     NV_OffscreenHeapPurge
;
; Purpose:      This routine purges all primary surface allocations
;               from video memory.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwOwnerID               Owner ID of allocations to purge
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_OffscreenHeapPurge, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwOwnerID
LOCALV  heapParams,NVIOCONTROL_PARAM
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        pushd   NVIOCONTROL_ARCH_HEAP
        mov     cx,ss
        push    cx
        lea     cx,heapParams
        push    cx
        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     heapParams.NVIOCONTROL_hRoot,ecx
        mov     eax,ds:[esi].HWBOARDCX.dwhDevice
        mov     heapParams.NVIOCONTROL_hObjectParent,eax
        mov     eax,dwOwnerID
        mov     heapParams.NVIOCONTROL_owner,eax
        mov     heapParams.NVIOCONTROL_function,NVIOCONTROL_HEAP_PURGE
        call    FFP16 PTR lpfnNvIoControl

        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetAllAttachedDevices
;
; Purpose:      This routine returns a mask that describes all attached
;               devices.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:
;               eax             bits 7:0   indicate if CRTC #X is attached
;                               bits 15:8  indicate if TV   #X is attached
;                               bits 23:16 indicate if DFP  #X is attached
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetAllAttachedDevices, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
LOCALD  dwAllDevices
OPENPROC
        PUSHR   esi

        ; First get all the device ports
        mov     esi,pHWBoardCXOffset
        pushd   NV_CFG_GET_ALL_DEVICES
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        call    FFP16 PTR lpfnNvConfigGet
        shl     edx,10H
        mov     dx,ax
        mov     dwAllDevices,edx

        ; OK, it is. Find out if the device is connected
        mov     ax,ss
        shl     eax,10H
        lea     ax,dwAllDevices
        mov     ebx,4
        mov     ecx,NV_CFGEX_CHECK_CONNECTED_DEVICES
        call    NV_ConfigExCall

        mov     eax,dwAllDevices
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_IsDeviceAttached
;
; Purpose:      This routine determines if the specified device is
;               attached.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwDevData               low byte:       0 - CRT
;                                                       1 - TV
;                                                       2 - DFP
;                                       next byte:      0-7 for device number
;
; Returns:
;               eax                     TRUE, the device is attached
;                                       FALSE is is not
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_IsDeviceAttached, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwDevData
LOCALD  dwDeviceToCheck
OPENPROC
        PUSHR   esi

        ; First get all the device ports
        mov     esi,pHWBoardCXOffset
        pushd   NV_CFG_GET_ALL_DEVICES
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        call    FFP16 PTR lpfnNvConfigGet
        shl     edx,10H
        mov     dx,ax
        mov     dwAllDevices,edx

        ; Form the bitmask 1 << (devtype * 8 + dev num) and
        ; see if device port is even available
        mov     ecx,dwDevData
        shl     cl,3
        add     cl,ch
        mov     eax,1
        shl     eax,cl
        test    eax,edx
        jz      DeviceNotPresent

        ; OK, it is. Find out if the device is connected
        mov     dwDeviceToCheck,eax
        mov     ax,ss
        shl     eax,10H
        lea     ax,dwDeviceToCheck
        mov     ebx,4
        mov     ecx,NV_CFGEX_CHECK_CONNECTED_DEVICES
        call    NV_ConfigExCall

        ; a 0 is success!
        or      eax,eax
        je      @F

DeviceNotPresent:
        mov     dwDeviceToCheck,0

@@:
        ; The bit for the device we were checking is turned off
        ; if the device is not connected. Otherwise it is left on.
        mov     eax,dwDeviceToCheck

DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_CanDevicesBeMapped
;
; Purpose:      This routine checks to see whether all the devices
;               specified can be used simultaneously. It doesn't
;               matter what the head assignment actually is, just
;               indicate whether it is possible.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               lpDevData               dword array ptr
;                                       Each dword looks like
;                                       low byte:       0 - CRT
;                                                       1 - TV
;                                                       2 - DFP
;                                       next byte:      0-7 for device number
;                                       next two bytes ignored
;               array length
;
; Returns:
;               eax                     TRUE, then they can
;                                       In this case ebx is the mapping
;                                       FALSE, then they can't
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_CanDevicesBeMapped, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   lpDevData
PARMD   dwArrayLen
LOCALV  sConfig,NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS
OPENPROC
        PUSHR   esi,edi

        ; Form a bitmask of the desired device connections
        sub     eax,eax
        sub     edx,edx
        sub     ebx,ebx
        les     bx,lpDevData
@@:
        mov     ecx,es:[ebx][edx * 4]
        shl     cl,3
        add     cl,ch
        mov     esi,1
        shl     esi,cl
        or      eax,esi
        inc     edx
        cmp     edx,dwArrayLen
        jc      @B

        ; OK, it is. Find out if the device is connected
        mov     sConfig.DevicesConfig,eax
        sub     edx,edx
        mov     sConfig.DevicesAllocationMap,edx
        mov     sConfig.OldDevicesConfig,edx
        mov     sConfig.OldDevicesAllocationMap,edx
        mov     ax,ss
        shl     eax,10H
        lea     ax,sConfig
        mov     ebx,SIZE NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS
        mov     ecx,NV_CFGEX_GET_DEVICES_CONFIGURATION
        mov     esi,pHWBoardCXOffset
        call    NV_ConfigExCall
        mov     ebx,sConfig.DevicesAllocationMap

        ; a 0 is success!
        mov     ecx,eax
        sub     eax,eax
        or      ecx,ecx
        jne     DoneIt
        mov     eax,1
DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_MapDevices
;
; Purpose:      This routine shuffles around the heads for the
;               CRTCs to get a mapping of heads to devices that
;               uses all the devices in the list passed in. It
;               is the callers responsibility to check and make
;               sure that such a mapping is possible by calling
;               HW_CanDevicesBeMapped before calling this routine.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               lpDevData               dword array ptr
;                                       Each dword looks like
;                                       low byte:       0 - CRT
;                                                       1 - TV
;                                                       2 - DFP
;                                       next byte:      0-7 for device number
;                                       next two bytes ignored
;               lpHWCrtcCX              array of HWCRTCCXs
;               array length
;
; Returns:
;               eax                     A dword specifying which devices
;                                       have changed. If bit 0 is a 1,
;                                       then first device in the lpDevData
;                                       array was remapped. If bit 1, is a
;                                       1, then the 2nd device in lpDevData
;                                       array was remapped, etc...
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_MapDevices, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   lpDevData
PARMD   lpHWCrtcCX
PARMD   dwArrayLen
LOCALD  dwPass
LOCALD  dwArrayCount
LOCALD  lpHWCrtcCX_Inc
LOCALD  lpDevData_Inc
LOCALD  dwPitchInBytes
LOCALD  dwDevicesConfig
LOCALD  dwDevicesAllocationMap
LOCALD  dwDeviceBit
LOCALD  dwDevicesChanged
OPENPROC
        PUSHR   esi,edi
        mov     dwDevicesChanged,0

        push    pHWBoardCXOffset
        push    lpDevData
        push    dwArrayLen
        call    HW_CanDevicesBeMapped
        or      eax,eax
        je      DoneIt
        mov     dwDevicesAllocationMap,ebx

        ; Form a bitmask of the desired device connections
        sub     eax,eax
        sub     edx,edx
        sub     ebx,ebx
        les     bx,lpDevData
@@:
        mov     ecx,es:[ebx][edx * 4]
        shl     cl,3
        add     cl,ch
        mov     esi,1
        shl     esi,cl
        or      eax,esi
        inc     edx
        cmp     edx,dwArrayLen
        jc      @B
        mov     dwDevicesConfig,eax

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; OK, everything is fine. We have the mapping. Now
        ; we just need to change VidLutCursorDac objects
        ; in each HWCRTCCX to match the mapping.
        ;
        ; In an attempt to be spiffy, we make two passes through
        ; the array of all CRTCCXs. The first will free every
        ; VidLutCursorDac object corresponding to a device whose
        ; head is changing. Any CRTCCXs that by chance are on the
        ; same head this time as last time will not be freed and
        ; realloced. This eliminates flashing when there is no
        ; need for it to flash.

        ; Let the RM know we are beginning a CHANGE
        push    pHWBoardCXOffset
        push    dwDevicesConfig
        call    HW_BeginDisplayChange

        mov     dwPass,0

LoopOnAction:
        mov     eax,dwArrayLen
        mov     dwArrayCount,eax
        mov     eax,lpHWCrtcCX
        mov     lpHWCrtcCX_Inc,eax
        mov     eax,lpDevData
        mov     lpDevData_Inc,eax
        mov     dwDeviceBit,1

LoopOnCX:
        les     bx,lpDevData_Inc
        mov     ecx,es:[bx]
        shl     cl,3
        add     cl,ch
        mov     eax,1
        shl     eax,cl

        ; Count the number of 1 bits less than the bit set in eax.
        ; This will be the nibble number in dwDevicesAllocationMap
        ; that holds the head number to use for this HWCRTCCX
        mov     edx,1
        sub     ebx,ebx

LoopOnNibble:
        test    edx,eax
        jnz     GotNibbleNumber
        test    edx,dwDevicesConfig
        jz      @F
        inc     ebx
@@:     shl     edx,1
        jmp     LoopOnNibble

GotNibbleNumber:
        ; ebx holds the nibble number
        mov     ecx,ebx
        shl     ecx,2
        mov     edi,dwDevicesAllocationMap
        shr     edi,cl
        and     edi,0FH
        les     bx,lpHWCrtcCX_Inc
        push    dword ptr es:[bx]
        call    HW_GetPhysicalCrtcIndex
        cmp     edi,eax
        je      SameHead

        ; Which Pass is this?
        cmp     dwPass,0
        jne     ReAlloc

FreeIt:
        ; OK, if this is the first pass, then we want to free
        ; all head related info in the HWCRTCCX if it is changing.
        les     bx,lpHWCrtcCX_Inc
        push    dword ptr es:[bx]
        call    NV_FreeHeadRelatedInfo
        jmp     NextCX

ReAlloc:
        ; If it is the second pass, then realloc all head related
        ; info to get the new stuff.
        les     bx,lpHWCrtcCX_Inc
        push    dword ptr es:[bx]
        push    edi
        call    NV_AllocHeadRelatedInfo

        ; If we are reallocing, then the device must have changed head.
        mov     eax,dwDeviceBit
        or      dwDevicesChanged,eax
        jmp     NextCX

NextCX:
SameHead:
        shl     dwDeviceBit,1
        add     lpHWCrtcCX_Inc,4
        add     lpDevData_Inc,4
        dec     dwArrayCount
        jne     LoopOnCX

        inc     dwPass
        cmp     dwPass,2
        jc      LoopOnAction
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ; Let the RM know we are beginning a CHANGE
        push    pHWBoardCXOffset
        push    dwDevicesConfig
        call    HW_EndDisplayChange

DoneIt:
        mov     eax,dwDevicesChanged
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_BeginDisplayChange
;
; Purpose:      This routine issues the DISPLAY_CHANGE_START
;               message to the RM. It is an API because the
;               generic code can call this whenever they
;               are about to detach or attach devices.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwDeviceMask            correclty formatted
;                                       mask that syas which devices
;                                       will be enabled after the
;                                       change is completed.
;
; Returns:
;               Nothing
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_BeginDisplayChange, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwDeviceMask
LOCALV  Params,NV_CFGEX_DISPLAY_CHANGE_START_PARAMS
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

	; OK, this is a hack for now. We have multiple nested calls to
	; BeginDisplayChange all over the driver. We are working to
	; change this, but for now I keep a "lock count". The first
	; Begin call gets changed up to the RM. No other begin call
	; goes up until I receive the matching end call.
	inc	ds:[esi].HWBOARDCX.dwDisplayChangeCount
	cmp	ds:[esi].HWBOARDCX.dwDisplayChangeCount,1
	jne	SkipIt

        mov     eax,dwDeviceMask
        mov     Params.NewDevices,eax
        sub     eax,eax
        mov     Params.Properties,eax
        mov     ax,ss
        shl     eax,10H
        lea     ax,Params
        mov     ebx,SIZE NV_CFGEX_DISPLAY_CHANGE_START_PARAMS
        mov     ecx,NV_CFGEX_DISPLAY_CHANGE_START
        call    NV_ConfigExSetCall

SkipIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_EndDisplayChange
;
; Purpose:      This routine issues the DISPLAY_CHANGE_END
;               message to the RM. It is an API because the
;               generic code can call this whenever they
;               are done detaching or attaching devices.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:
;               Nothing
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_EndDisplayChange, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwDeviceMask
LOCALV  Params,NV_CFGEX_DISPLAY_CHANGE_END_PARAMS
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

	; OK, this is a hack for now. We have multiple nested calls to
	; BeginDisplayChange all over the driver. We are working to
	; change this, but for now I keep a "lock count". The first
	; Begin call gets changed up to the RM. No other begin call
	; goes up until I receive the matching end call.
	cmp	ds:[esi].HWBOARDCX.dwDisplayChangeCount,1
	jne	SkipIt

        mov     eax,dwDeviceMask
        mov     Params.NewDevices,eax
        sub     eax,eax
        mov     Params.Properties,eax
        mov     ax,ss
        shl     eax,10H
        lea     ax,Params
        mov     ebx,SIZE NV_CFGEX_DISPLAY_CHANGE_END_PARAMS
        mov     ecx,NV_CFGEX_DISPLAY_CHANGE_END
        call    NV_ConfigExSetCall

SkipIt:
	dec	ds:[esi].HWBOARDCX.dwDisplayChangeCount
        POPR    esi
CLOSEPROC

CLOSESEG _NVTEXT32

END

