;==============================================================================
;
; Copyright (C) 1998, NVidia Coporation
;
; File:         objects.asm
;
; Purpose:      This file creates and destroys objects that are global
;               to the BOARD.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include hwmacs.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc
include ..\..\..\..\..\sdk\nvidia\inc\nvrmarch.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     NV_CreateDmaContexts
;
; Purpose:      This function creates all the DMA contexts we will need
;               for the WIN CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_CreateDmaContexts, PASCAL, NOFRAME, FAR32
OPENPROC
        ; We only create these once -
        ; they are never destroyed untl the driver unloads.
        call    CreatePushBufferNotifier
        call    CreateVramToSysramNotifier0
        call    CreateVramToSysramNotifier1
        call    CreateSysramToVideoNotifier
        call    CreateDmaFromSysram
        call    CreateDmaToSysram
        call    CreateDmaFromVram
        call    CreateDmaInVram
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_CreateObjects
;
; Purpose:      This function creates all the objects we will need
;               for the WIN CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_CreateObjects, PASCAL, NOFRAME, FAR32
OPENPROC
        ; I don't want to have to wait for space anywhere in the
        ; object creation routines. Just wait for 4K bytes. That
        ; will be enough to hold the whole creation process.
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        CHECKDMASPACE ds,esi,edi,1000H

        ; Create all the objects we will need.
        call    CreateContextSurfaces2D
        call    CreateVramToSysramCopy0
        call    CreateVramToSysramCopy1
        call    CreateContextClipRect
        call    CreateContextPattern
        call    CreateContextRop
        call    CreateImageBlit
        call    CreateRectAndText
        call    CreateImageFromCpu
        call    CreateSolidLine
        call    CreateIndexedImageFromCpu
	call	CreateStretchedImageFromCpu

        ; I want to track the byte usage in the DMA buffer accurately,
        ; but I didn't wait for the exact number of dwords needed for
        ; the object creations above. Let's stick a JMP_COMMAND in the
        ; DMA buffer to get back to the beginning and then wait until
        ; the DMA pusher goes idle. Then we can reset all our variables
        ; to indicate the whole buffer is free and start out fresh.
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset

        lea     eax,[esi].HWBOARDCX.bDmaPushBuffer
        mov     ecx,eax
        sub     ecx,ds:[esi].HWBOARDCX.pPushBufferContextStartOffset
        or      ecx,JMP_COMMAND
        mov     ds:[edi],ecx
        xor     ecx,JMP_COMMAND
        SETPUTPTR ds,esi,eax
@@:
        cmp     ds:[esi].HWBOARDCX.dwDmaGetPtr,ecx
        jne     @B

        ; Now set the number of bytes free
        mov     ds:[esi].HWBOARDCX.dwBytesFree,DMA_PUSH_BUFFER_SIZE - WRITE_COMBINE_PAD_SIZE - 10h
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     CreatePushBufferNotifier
;
; Purpose:      This function creates the main push buffer notifier
;               for the WIN CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreatePushBufferNotifier, PASCAL, NOFRAME, NEAR
OPENPROC
        push    esi
        pushd   OBJ_DMA_PUSH_BUFFER_NOTIFIER
        lea     ebx,[esi].HWBOARDCX.sPushBufferNotifier
        push    ebx
        push    ds
        pushd   SIZE NvNotification
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY
        call    NV_AllocContextDma
CLOSEPROC


;==============================================================================
;
; Function:     CreateVramToSysramNotifier0
;
; Purpose:      This function creates a context DMA which is to
;               be used as the notifier for a video memory to
;               system memory MemoryToMemoryFormat copy in
;               the WIN CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateVramToSysramNotifier0, PASCAL, NOFRAME, NEAR
OPENPROC
        push    esi
        pushd   OBJ_VRAM_TO_SYSRAM_NOTIFIER0
        lea     ebx,[esi].HWBOARDCX.sVramToSysramMethodNotifier0
        push    ebx
        push    ds
        pushd   2 * (SIZE NvNotification)
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY
        call    NV_AllocContextDma
CLOSEPROC


;==============================================================================
;
; Function:     CreateVramToSysramNotifier1
;
; Purpose:      This function creates a context DMA which is to
;               be used as the notifier for a video memory to
;               system memory MemoryToMemoryFormat copy in
;               the WIN CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateVramToSysramNotifier1, PASCAL, NOFRAME, NEAR
OPENPROC
        push    esi
        pushd   OBJ_VRAM_TO_SYSRAM_NOTIFIER1
        lea     ebx,[esi].HWBOARDCX.sVramToSysramMethodNotifier1
        push    ebx
        push    ds
        pushd   2 * (SIZE NvNotification)
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY
        call    NV_AllocContextDma
CLOSEPROC


;==============================================================================
;
; Function:     CreateSysramToVideoNotifier
;
; Purpose:      This function creates a context DMA to be used as a
;               notifier for a system memory to video (Color Look Up
;               Table stuff) NV_VIDEO_FROM_MEMORY object in the
;               WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateSysramToVideoNotifier, PASCAL, NOFRAME, NEAR
OPENPROC
        push    esi
        pushd   OBJ_SYSRAM_TO_VIDEO_NOTIFIER
        lea     ebx,[esi].HWBOARDCX.sSysramToVideoBufferNotifier
        push    ebx
        push    ds
        pushd   5 * (SIZE NvNotification)
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY
        call    NV_AllocContextDma
CLOSEPROC


;==============================================================================
;
; Function:     CreateDmaToSysram
;
; Purpose:      This function creates a context DMA for system memory
;               to be used as the destination in a MemoryToMemoryFormat
;               copy in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateDmaToSysram, PASCAL, NOFRAME, NEAR
OPENPROC
        push    esi
        pushd   OBJ_DMA_TO_SYSRAM
        lea     ebx,[esi].HWBOARDCX.bCacheableDmaBuffer
        push    ebx
        push    ds
        pushd   (CACHEABLE_BUFFER_SIZE + 0FFFH) AND 0FFFFF000H
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY
        call    NV_AllocContextDma
CLOSEPROC


;==============================================================================
;
; Function:     CreateDmaFromSysram
;
; Purpose:      This function creates a context DMA for system memory
;               to be used as the source in a MemoryToMemoryFormat
;               copy in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateDmaFromSysram, PASCAL, NOFRAME, NEAR
OPENPROC
        push    esi
        pushd   OBJ_DMA_FROM_SYSRAM
        lea     ebx,[esi].HWBOARDCX.bCacheableDmaBuffer
        push    ebx
        push    ds
        pushd   (CACHEABLE_BUFFER_SIZE + 0FFFH) AND 0FFFFF000H
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY
        call    NV_AllocContextDma
CLOSEPROC


;==============================================================================
;
; Function:     CreateDmaFromVram
;
; Purpose:      This function creates a context DMA for video memory
;               to be used as the source in objects in the WIN_CHANNEL.
;
; Purpose:      This function creates a context DMA from the push buffer.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateDmaFromVram, PASCAL, NOFRAME, NEAR
OPENPROC
        push    esi
        pushd   OBJ_DMA_FROM_VRAM
        pushd   0
        mov     ecx,ds:[esi].HWBOARDCX.dwVideoMemorySel
        push    ecx
        push    ds:[esi].HWBOARDCX.dwVideoMemorySize
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY
        call    NV_AllocContextDma
CLOSEPROC


;==============================================================================
;
; Function:     CreateDmaInVram
;
; Purpose:      This function creates a context DMA for video memory
;               to be used as a field for objects in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateDmaInVram, PASCAL, NOFRAME, NEAR
OPENPROC
        push    esi
        pushd   OBJ_DMA_IN_VRAM
        pushd   0
        mov     ecx,ds:[esi].HWBOARDCX.dwVideoMemorySel
        push    ecx
        push    ds:[esi].HWBOARDCX.dwVideoMemorySize
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR
        call    NV_AllocContextDma
CLOSEPROC


;==============================================================================
;
; Function:     CreateContextSurfaces2D
;
; Purpose:      This function creates a ContextSurfaces2D object
;               to be used in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateContextSurfaces2D, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        mov     eax,NV4_CONTEXT_SURFACES_2D
        cmp     ds:[esi].HWBOARDCX.dwDeviceVersion,NV_DEVICE_VERSION_10
        jl      @F
        mov     eax,NV10_CONTEXT_SURFACES_2D
@@:
        push    esi
        push    eax
        pushd   OBJ_CONTEXT_SURFACES_2D
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(ContextSurfaces2dHdr.object,1)
        mov     ebx,OBJ_CONTEXT_SURFACES_2D
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        mov     eax,CHNRUN(ContextSurfaces2d.SetColorFormat,1)
        mov     ds:[edi + 08H],eax
        mov     dword ptr ds:[edi + 0CH],NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8

        ; Program the non-volatile data
        mov     eax,CHNRUN(ContextSurfaces2d.SetContextDmaImageSource,2)
        mov     ecx,OBJ_DMA_IN_VRAM
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ecx
        mov     ds:[edi + 18H],ecx
        add     edi,1CH
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateVramToSysramCopy0
;
; Purpose:      This function creates a MemoryToMemoryFormat object
;               for copying from video memory to system memory. This
;               is one of the two that we will need.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateVramToSysramCopy0, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        push    esi
        pushd   NV03_MEMORY_TO_MEMORY_FORMAT
        pushd   OBJ_DMA_FROM_VRAM_TO_SYSRAM_0
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(MemoryToMemoryFormat0Hdr.object,1)
        mov     ebx,OBJ_DMA_FROM_VRAM_TO_SYSRAM_0
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(MemoryToMemoryFormat0.SetContextDmaNotifies,3)
        mov     ecx,OBJ_VRAM_TO_SYSRAM_NOTIFIER0
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ecx
        mov     eax,OBJ_DMA_FROM_VRAM
        mov     ecx,OBJ_DMA_TO_SYSRAM
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ecx
        add     edi,18H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateVramToSysramCopy1
;
; Purpose:      This function creates a MemoryToMemoryFormat object
;               for copying from video memory to system memory. This
;               is one of the two that we will need.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateVramToSysramCopy1, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        push    esi
        pushd   NV03_MEMORY_TO_MEMORY_FORMAT
        pushd   OBJ_DMA_FROM_VRAM_TO_SYSRAM_1
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(MemoryToMemoryFormat1Hdr.object,1)
        mov     ebx,OBJ_DMA_FROM_VRAM_TO_SYSRAM_1
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(MemoryToMemoryFormat1.SetContextDmaNotifies,3)
        mov     ecx,OBJ_VRAM_TO_SYSRAM_NOTIFIER1
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ecx
        mov     eax,OBJ_DMA_FROM_VRAM
        mov     ecx,OBJ_DMA_TO_SYSRAM
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ecx
        add     edi,18H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateContextClipRect
;
; Purpose:      This function creates a ContextClipRectangle object
;               for use as a global clip rectangle in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateContextClipRect, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        push    esi
        pushd   NV01_CONTEXT_CLIP_RECTANGLE
        pushd   OBJ_CONTEXT_CLIP_RECT
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(ContextClipRectHdr.object,1)
        mov     ebx,OBJ_CONTEXT_CLIP_RECT
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(ContextClipRect.SetContextDmaNotifies,1)
        mov     ebx,OBJ_DMA_PUSH_BUFFER_NOTIFIER
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx

        ; Default it to the largest possible clip area
        mov     eax,CHNRUN(ContextClipRect.SetPoint,2)
        sub     ebx,ebx
        mov     ecx,7FFF7FFFH
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ebx
        mov     ds:[edi + 18H],ecx
        add     edi,1CH
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateContextPattern
;
; Purpose:      This function creates an ContextPattern object
;               for use as a global pattern in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateContextPattern, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        push    esi
        pushd   NV04_CONTEXT_PATTERN
        pushd   OBJ_CONTEXT_PATTERN
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(ContextPatternHdr.object,1)
        mov     ebx,OBJ_CONTEXT_PATTERN
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        ; Doesn't matter what we set the color format and the
        mov     eax,CHNRUN(ContextPattern.SetColorFormat,6)
        mov     ds:[edi + 08H],eax
        mov     dword ptr ds:[edi + 0CH],NV044_SET_COLOR_FORMAT_LE_A8R8G8B8
        mov     ebx,NV044_SET_MONOCHROME_FORMAT_CGA6_M1
        mov     ds:[edi + 10H],ebx
        mov     eax,NV044_SET_MONOCHROME_SHAPE_8X_8Y
        mov     ebx,NV044_SET_PATTERN_SELECT_MONOCHROME
        mov     ecx,-1
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ebx
        mov     ds:[edi + 1CH],ecx
        mov     ds:[edi + 20H],ecx
        
        add     edi,24H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateContextRop
;
; Purpose:      This function creates a ContextRop object
;               for use as a global rop in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateContextRop, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        push    esi
        pushd   NV03_CONTEXT_ROP
        pushd   OBJ_CONTEXT_ROP
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(ContextRopHdr.object,1)
        mov     ebx,OBJ_CONTEXT_ROP
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx
        add     edi,08H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateImageBlit
;
; Purpose:      This function creates an ImageBlit object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateImageBlit, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        push    esi
        pushd   NV04_IMAGE_BLIT
        pushd   OBJ_IMAGE_BLIT
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(ImageBlitHdr.object,1)
        mov     ebx,OBJ_IMAGE_BLIT
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(ImageBlit.SetContextClipRectangle,3)
        mov     ebx,OBJ_CONTEXT_CLIP_RECT
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx
        mov     eax,OBJ_CONTEXT_PATTERN
        mov     ebx,OBJ_CONTEXT_ROP
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ebx
        mov     eax,CHNRUN(ImageBlit.SetContextSurfaces,1)
        mov     ebx,OBJ_CONTEXT_SURFACES_2D
        mov     ds:[edi + 18H],eax
        mov     ds:[edi + 1CH],ebx
        mov     eax,CHNRUN(ImageBlit.SetOperation,1)
        mov     ebx,NV05F_SET_OPERATION_ROP_AND
        mov     ds:[edi + 20H],eax
        mov     ds:[edi + 24H],ebx
        add     edi,28H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateRectAndText
;
; Purpose:      This function creates a GdiRectangleText object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateRectAndText, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        push    esi
        pushd   NV04_GDI_RECTANGLE_TEXT
        pushd   OBJ_RECT_AND_TEXT
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(RectAndTextHdr.object,1)
        mov     ebx,OBJ_RECT_AND_TEXT
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(RectAndText.SetContextDmaFonts,3)
        mov     ebx,OBJ_DMA_IN_VRAM
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx
        mov     eax,OBJ_CONTEXT_PATTERN
        mov     ebx,OBJ_CONTEXT_ROP
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ebx
        mov     eax,CHNRUN(RectAndText.SetContextSurface,1)
        mov     ebx,OBJ_CONTEXT_SURFACES_2D
        mov     ds:[edi + 18H],eax
        mov     ds:[edi + 1CH],ebx
        mov     eax,CHNRUN(RectAndText.SetOperation,2)
        mov     ebx,NV04A_SET_OPERATION_ROP_AND
        mov     ds:[edi + 20H],eax
        mov     ds:[edi + 24H],ebx
        mov     dword ptr ds:[edi + 28H],NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8
        add     edi,2CH
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateImageFromCpu
;
; Purpose:      This function creates an ImageFromCpu object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateImageFromCpu, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        push    esi
        pushd   NV04_IMAGE_FROM_CPU
        pushd   OBJ_IMAGE_FROM_CPU
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(ImageFromCpuHdr.object,1)
        mov     ebx,OBJ_IMAGE_FROM_CPU
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(ImageFromCpu.SetContextClipRectangle,3)
        mov     ebx,OBJ_CONTEXT_CLIP_RECT
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx
        mov     eax,OBJ_CONTEXT_PATTERN
        mov     ebx,OBJ_CONTEXT_ROP
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ebx
        mov     eax,CHNRUN(ImageFromCpu.SetContextSurface,1)
        mov     ebx,OBJ_CONTEXT_SURFACES_2D
        mov     ds:[edi + 18H],eax
        mov     ds:[edi + 1CH],ebx
        mov     eax,CHNRUN(ImageFromCpu.SetOperation,2)
        mov     ebx,NV061_SET_OPERATION_ROP_AND
        mov     ds:[edi + 20H],eax
        mov     ds:[edi + 24H],ebx
        mov     dword ptr ds:[edi + 28H],NV061_SET_COLOR_FORMAT_LE_X8R8G8B8
        add     edi,2CH
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateSolidLine
;
; Purpose:      This function creates a RenderSolidLin object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateSolidLine, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        push    esi
        pushd   NV04_RENDER_SOLID_LIN
        pushd   OBJ_SOLID_LINE
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(RenderSolidLinHdr.object,1)
        mov     ebx,OBJ_SOLID_LINE
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(RenderSolidLin.SetContextClipRectangle,3)
        mov     ebx,OBJ_CONTEXT_CLIP_RECT
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx
        mov     eax,OBJ_CONTEXT_PATTERN
        mov     ebx,OBJ_CONTEXT_ROP
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ebx
        mov     eax,CHNRUN(RenderSolidLin.SetContextSurface,1)
        mov     ebx,OBJ_CONTEXT_SURFACES_2D
        mov     ds:[edi + 18H],eax
        mov     ds:[edi + 1CH],ebx
        mov     eax,CHNRUN(RenderSolidLin.SetOperation,2)
        mov     ebx,NV05C_SET_OPERATION_ROP_AND
        mov     ds:[edi + 20H],eax
        mov     ds:[edi + 24H],ebx
        mov     dword ptr ds:[edi + 28H],NV05C_SET_COLOR_FORMAT_LE_X8R8G8B8
        add     edi,2CH
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateIndexedImageFromCpu
;
; Purpose:      This function creates an IndexedImageFromCpu object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateIndexedImageFromCpu, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        mov     eax,NV04_INDEXED_IMAGE_FROM_CPU
        cmp     ds:[esi].HWBOARDCX.dwDeviceVersion, NV_DEVICE_VERSION_5
        jl      @F
        mov     eax,NV05_INDEXED_IMAGE_FROM_CPU
@@:
        push    esi
        pushd   eax
        pushd   OBJ_INDEXED_IMAGE_FROM_CPU
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(IndexedImageFromCpuHdr.object,1)
        mov     ebx,OBJ_INDEXED_IMAGE_FROM_CPU
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(IndexedImageFromCpu.SetContextDmaLut,1)
        mov     ebx,OBJ_DMA_FROM_VRAM
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx
        mov     eax,CHNRUN(IndexedImageFromCpu.SetContextClipRectangle,3)
        mov     ebx,OBJ_CONTEXT_CLIP_RECT
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ebx
        mov     eax,OBJ_CONTEXT_PATTERN
        mov     ebx,OBJ_CONTEXT_ROP
        mov     ds:[edi + 18H],eax
        mov     ds:[edi + 1CH],ebx
        mov     eax,CHNRUN(IndexedImageFromCpu.SetContextSurface,1)
        mov     ebx,OBJ_CONTEXT_SURFACES_2D
        mov     ds:[edi + 20H],eax
        mov     ds:[edi + 24H],ebx
        mov     eax,CHNRUN(IndexedImageFromCpu.SetColorConversion,3)
        mov     ebx,NV060_SET_COLOR_CONVERSION_TRUNCATE
        mov     ecx,NV060_SET_OPERATION_ROP_AND
        mov     edx,NV060_SET_COLOR_FORMAT_LE_X8R8G8B8
        mov     ds:[edi + 28H],eax
        mov     ds:[edi + 2CH],ebx
        mov     ds:[edi + 30H],ecx
        mov     ds:[edi + 34H],edx
        add     edi,38H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CreateStretchedImageFromCpu
;
; Purpose:      This function creates an StretchedImageFromCpu object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC CreateStretchedImageFromCpu, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Create the object
        push    esi
        pushd   NV04_STRETCHED_IMAGE_FROM_CPU
        pushd   OBJ_STRETCHED_IMAGE_FROM_CPU
        call    NV_AllocWinChannelObject

        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(StretchedImageFromCpuHdr.object,1)
        mov     ebx,OBJ_STRETCHED_IMAGE_FROM_CPU
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(StretchedImageFromCpu.SetContextPattern,2)
        mov     ebx,OBJ_CONTEXT_PATTERN
        mov     ecx,OBJ_CONTEXT_ROP
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx
        mov     ds:[edi + 10H],ecx
        mov     eax,CHNRUN(StretchedImageFromCpu.SetContextSurface,1)
        mov     ebx,OBJ_CONTEXT_SURFACES_2D
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ebx
        mov     eax,CHNRUN(StretchedImageFromCpu.SetOperation,2)
        mov     ebx,NV061_SET_OPERATION_ROP_AND
        mov     ds:[edi + 1CH],eax
        mov     ds:[edi + 20H],ebx
        mov     dword ptr ds:[edi + 24H],NV061_SET_COLOR_FORMAT_LE_X8R8G8B8
        add     edi,28H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocContextDma
;
; Purpose:      This function allocates an NV01_CONTEXT_DMA in the memory
;               range defined by the arguments and gives that instance the
;               object ID provided.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwObjectID      Object ID
;               pBaseAddr       Base
;               dwSelector      selector
;               dwLimit         limit
;               dwFlags         Flags (e.g.
;                               ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY)
;                               The possible flags are defined in the
;                               header file nvrmarch.inc
;
; Returns:      eax     0       failure
;                       non-0   Sucess
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocContextDma, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwObjectID
PARMD   pBaseAddr
PARMD   dwSelector
PARMD   dwLimit
PARMD   dwFlags
LOCALV  ContextDmaParam,ALLOC_CTX_DMA_PARAM
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        PUSHSREG ss
        lea     bx,ContextDmaParam
        push    bx
        mov     eax,ds:[esi].HWBOARDCX.dwRootHandle
        mov     ContextDmaParam.ALLOC_CTX_DMA_hObjectParent,eax
        mov     eax,dwObjectID
        mov     ContextDmaParam.ALLOC_CTX_DMA_hObjectNew,eax
        mov     ContextDmaParam.ALLOC_CTX_DMA_hClass,NV01_CONTEXT_DMA
        mov     eax,dwFlags
        mov     ContextDmaParam.ALLOC_CTX_DMA_flags,eax
        mov     eax,pBaseAddr
        mov     ContextDmaParam.ALLOC_CTX_DMA_pBase,eax
        movzx   eax,word ptr dwSelector
        mov     ContextDmaParam.ALLOC_CTX_DMA_pBase+4,eax
        mov     eax,dwLimit
        dec     eax
        mov     ContextDmaParam.ALLOC_CTX_DMA_limit,eax
        mov     ContextDmaParam.ALLOC_CTX_DMA_limit+4,0
        mov     eax,ALLOC_CTX_DMA_STATUS_SUCCESS
        mov     ContextDmaParam.ALLOC_CTX_DMA_status,eax
        call    FFP16 PTR lpfnArch_Nv01AllocContextDma

        cmp     ContextDmaParam.ALLOC_CTX_DMA_status,ALLOC_CTX_DMA_STATUS_SUCCESS
        sete    al
        movzx   eax,al
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     NV_FreeContextDma
;
; Purpose:      This function frees a Context Dma.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwObjectID              object ID to allocate
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_FreeContextDma, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwObjectID
LOCALV  FreeParam,NV01FREE_PARAM
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        mov	bx,ss
        shl	ebx,10H
        lea     bx,FreeParam
        push    ebx
        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     FreeParam.NV01FREE_hClient,ecx
        mov     FreeParam.NV01FREE_hObjectParent,ecx
        mov     eax,dwObjectID
        mov     FreeParam.NV01FREE_hObjectOld,eax
        mov     FreeParam.NV01FREE_status,NV01FREE_STATUS_SUCCESS
        call    FFP16 PTR lpfnArch_Nv01Free
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocWinChannelObject
;
; Purpose:      This function allocates any type of object whose parent
;               should be NV_WIN_CHANNEL.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwObjectClass           object class to allocate
;               dwObjectID              object ID to allocate
;
; Returns:      eax     0       failure
;                       non-0   Sucess
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocWinChannelObject, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwObjectClass
PARMD   dwObjectID
LOCALV  AllocObjectParam,ALLOC_OBJECT_PARAM
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        PUSHSREG ss
        lea     cx,AllocObjectParam
        push    cx
        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     AllocObjectParam.ALLOC_OBJECT_hClient,ecx
        mov     AllocObjectParam.ALLOC_OBJECT_hObjectParent,NV_WIN_CHANNEL
        mov     eax,dwObjectID
        mov     AllocObjectParam.ALLOC_OBJECT_hObjectNew,eax
        mov     eax,dwObjectClass
        mov     AllocObjectParam.ALLOC_OBJECT_hClass,eax
        mov     AllocObjectParam.ALLOC_OBJECT_status,ALLOC_OBJ_STATUS_SUCCESS
        call    FFP16 PTR lpfnArch_Nv01AllocObject

        cmp     AllocObjectParam.ALLOC_OBJECT_status,ALLOC_OBJ_STATUS_SUCCESS
        sete    al
        movzx   eax,al
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     NV_RMAllocObject
;
; Purpose:      This function allocates any type of object whose parent
;               should be NV_WIN_CHANNEL.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwObjectClass           object class to allocate
;               dwObjectID              object ID to allocate
;
; Returns:      eax     0       failure
;                       non-0   Sucess
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_RMAllocObject, PASCAL, FRAME, FAR32
PARMD   pHWCRTCCXOffset
PARMD   pHWBoardCXOffset
PARMD   dwObjectClass
PARMD   dwObjectID
LOCALV  AllocParam,ALLOC_PARAM
LOCALV  AllocParms,NV07C_ALLOCATION_PARAMETERS
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWBoardCXOffset
        mov     edi,pHWCRTCCXOffset

;NvRmAlloc(ULONG hClient, ULONG hParent, ULONG hObject, ULONG hClass,
;          PVOID pAllocParms); //  NvRmApi.h

        sub     eax,eax
        mov     ax,ss
        push    eax
        call    GN_GetSelectorBase
        lea     ecx,AllocParms
        add     ecx,eax
        mov     AllocParam.ALLOC_pAllocParms,ecx

        mov     edx,ds:[edi].HWCRTCCX.dwPhysicalCRTCInstance
        mov     AllocParms.logicalHeadId,edx

        PUSHSREG ss
        lea     cx,AllocParam
        push    cx
        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     AllocParam.ALLOC_hRoot,ecx
        mov     AllocParam.ALLOC_hObjectParent,NV_WIN_CHANNEL
        mov     eax,dwObjectID
        mov     AllocParam.ALLOC_hObjectNew,eax
        mov     eax,dwObjectClass
        mov     AllocParam.ALLOC_hClass,eax
        mov     AllocParam.ALLOC_status,NVOS21_STATUS_SUCCESS
        call    FFP16 PTR lpfnArch_Nv04Alloc

        cmp     AllocParam.ALLOC_status,NVOS21_STATUS_SUCCESS
        sete    al
        movzx   eax,al
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_FreeWinChannelObject
;
; Purpose:      This function frees an object whose parent is
;               NV_WIN_CHANNEL.
;
; Arguments:
;               ds                      Context Selector
;               pHWBoardCXOffset        HWBOARDCX ptr
;               dwObjectID              object ID to allocate
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_FreeWinChannelObject, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwObjectID
LOCALV  FreeParam,NV01FREE_PARAM
OPENPROC
        PUSHR   esi
        mov     esi,pHWBoardCXOffset

        PUSHSREG ss
        lea     bx,FreeParam
        push    bx
        mov     ecx,ds:[esi].HWBOARDCX.dwRootHandle
        mov     FreeParam.NV01FREE_hClient,ecx
        mov     FreeParam.NV01FREE_hObjectParent,NV_WIN_CHANNEL
        mov     eax,dwObjectID
        mov     FreeParam.NV01FREE_hObjectOld,eax
        mov     FreeParam.NV01FREE_status,NV01FREE_STATUS_SUCCESS
        call    FFP16 PTR lpfnArch_Nv01Free
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     NV_DestroyDmaContexts
;
; Purpose:      This function creates all the DMA contexts we will need
;               for the WIN CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_DestroyDmaContexts, PASCAL, NOFRAME, FAR32
OPENPROC
	push	esi
        pushd	OBJ_DMA_PUSH_BUFFER_NOTIFIER
        call    NV_FreeContextDma

	push	esi
	pushd	OBJ_VRAM_TO_SYSRAM_NOTIFIER0
	call	NV_FreeContextDma

	push	esi
	pushd	OBJ_VRAM_TO_SYSRAM_NOTIFIER1
	call	NV_FreeContextDma

	push	esi
	pushd	OBJ_SYSRAM_TO_VIDEO_NOTIFIER
	call	NV_FreeContextDma

	push	esi
	pushd	OBJ_DMA_TO_SYSRAM
	call	NV_FreeContextDma

	push	esi
	pushd	OBJ_DMA_FROM_SYSRAM
	call	NV_FreeContextDma

	push	esi
	pushd	OBJ_DMA_FROM_VRAM
	call	NV_FreeContextDma

	push	esi
	pushd	OBJ_DMA_IN_VRAM
	call	NV_FreeContextDma
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_DestroyObjects
;
; Purpose:      This function destroys all the objects we will need
;               for the WIN CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_DestroyObjects, PASCAL, NOFRAME, FAR32
OPENPROC

IF 0
        ; Can't do this right now. By the time we get here,
        ; Windows has thrown us into DOS and the push buffer
        ; is disabled at that time. Oh well!

        ; I don't want to have to wait for space anywhere in the
        ; object destruction routines. Just wait for 4K bytes. That
        ; will be enough to hold the whole destruction process.
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        CHECKDMASPACE ds,esi,edi,1000H

        call    DestroyContextSurfaces2D
        call    DestroyVramToSysramCopy0
        call    DestroyVramToSysramCopy1
        call    DestroyContextClipRect
        call    DestroyContextPattern
        call    DestroyContextRop
        call    DestroyImageBlit
        call    DestroyRectAndText
        call    DestroyImageFromCpu
        call    DestroySolidLine
        call    DestroyIndexedImageFromCpu

        ; I want to track the byte usage in the DMA buffer accurately,
        ; but I didn't wait for the exact number of dwords needed for
        ; the object destruction routines above. Sync up here.
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        lea     eax,[esi].HWBOARDCX.bDmaPushBuffer
        mov     ecx,eax
        sub     ecx,ds:[esi].HWBOARDCX.pPushBufferContextStartOffset
        or      ecx,JMP_COMMAND
        mov     ds:[edi],ecx
        xor     ecx,JMP_COMMAND
        SETPUTPTR ds,esi,eax
@@:
        cmp     ds:[esi].HWBOARDCX.dwDmaGetPtr,ecx
        jne     @B
ENDIF

	; Now we can actually free all the objects
	push	esi
        pushd	OBJ_CONTEXT_SURFACES_2D
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_DMA_FROM_VRAM_TO_SYSRAM_0
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_DMA_FROM_VRAM_TO_SYSRAM_1
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_CONTEXT_CLIP_RECT
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_CONTEXT_PATTERN
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_CONTEXT_ROP
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_IMAGE_BLIT
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_RECT_AND_TEXT
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_IMAGE_FROM_CPU
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_SOLID_LINE
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_INDEXED_IMAGE_FROM_CPU
        call    NV_FreeWinChannelObject

	push	esi
        pushd	OBJ_STRETCHED_IMAGE_FROM_CPU
        call    NV_FreeWinChannelObject
CLOSEPROC

IF 0
;==============================================================================
;
; Function:     DestroyContextSurfaces2D
;
; Purpose:      This function Destroys a ContextSurfaces2D object
;               to be used in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroyContextSurfaces2D, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(ContextSurfaces2dHdr.object,1)
        mov     ebx,OBJ_CONTEXT_SURFACES_2D
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Null out the context DMAs
        mov     eax,CHNRUN(ContextSurfaces2d.SetContextDmaImageSource,2)
        sub	ecx,ecx
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ecx
        mov     ds:[edi + 10H],ecx
        add     edi,14H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     DestroyVramToSysramCopy0
;
; Purpose:      This function destroys a MemoryToMemoryFormat object
;               for copying from video memory to system memory.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroyVramToSysramCopy0, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(MemoryToMemoryFormat0Hdr.object,1)
        mov     ebx,OBJ_DMA_FROM_VRAM_TO_SYSRAM_0
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Clear out the context DMA
        mov     eax,CHNRUN(MemoryToMemoryFormat0.SetContextDmaNotifies,3)
        sub	ecx,ecx
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ecx
        mov     ds:[edi + 10H],ecx
        mov     ds:[edi + 14H],ecx
        add     edi,18H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     DestroyVramToSysramCopy1
;
; Purpose:      This function destroys a MemoryToMemoryFormat object
;               for copying from video memory to system memory.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroyVramToSysramCopy1, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(MemoryToMemoryFormat1Hdr.object,1)
        mov     ebx,OBJ_DMA_FROM_VRAM_TO_SYSRAM_1
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Clear out the context DMA
        mov     eax,CHNRUN(MemoryToMemoryFormat1.SetContextDmaNotifies,3)
        sub	ecx,ecx
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ecx
        mov     ds:[edi + 10H],ecx
        mov     ds:[edi + 14H],ecx
        add     edi,18H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     DestroyContextClipRect
;
; Purpose:      This function destroys a ContextClipRectangle object
;               for use as a global clip rectangle in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroyContextClipRect, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(ContextClipRectHdr.object,1)
        mov     ebx,OBJ_CONTEXT_CLIP_RECT
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Clear out the context DMA
        mov     eax,CHNRUN(ContextClipRect.SetContextDmaNotifies,1)
        sub	ebx,ebx
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx
        add     edi,10H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     DestroyContextPattern
;
; Purpose:      This function destroys an ContextPattern object
;               for use as a global pattern in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroyContextPattern, PASCAL, NOFRAME, NEAR
OPENPROC
	; Don't need to do anything special
CLOSEPROC


;==============================================================================
;
; Function:     DestroyContextRop
;
; Purpose:      This function destroys a ContextRop object
;               for use as a global rop in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroyContextRop, PASCAL, NOFRAME, NEAR
OPENPROC
CLOSEPROC


;==============================================================================
;
; Function:     DestroyImageBlit
;
; Purpose:      This function destroys an ImageBlit object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroyImageBlit, PASCAL, NOFRAME, NEAR
OPENPROC
CLOSEPROC


;==============================================================================
;
; Function:     DestroyRectAndText
;
; Purpose:      This function destroys a GdiRectangleText object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroyRectAndText, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(RectAndTextHdr.object,1)
        mov     ebx,OBJ_RECT_AND_TEXT
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(RectAndText.SetContextDmaFonts,1)
        sub	ebx,ebx
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx
        add     edi,10H
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     DestroyImageFromCpu
;
; Purpose:      This function destroys an ImageFromCpu object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroyImageFromCpu, PASCAL, NOFRAME, NEAR
OPENPROC
CLOSEPROC


;==============================================================================
;
; Function:     DestroySolidLine
;
; Purpose:      This function destroys a RenderSolidLin object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroySolidLine, PASCAL, NOFRAME, NEAR
OPENPROC
CLOSEPROC


;==============================================================================
;
; Function:     DestroyIndexedImageFromCpu
;
; Purpose:      This function destroys an IndexedImageFromCpu object
;               for use in the WIN_CHANNEL.
;
; Arguments:    ds:esi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DestroyIndexedImageFromCpu, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Set the object in a sub-channel
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(IndexedImageFromCpuHdr.object,1)
        mov     ebx,OBJ_INDEXED_IMAGE_FROM_CPU
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(IndexedImageFromCpu.SetContextDmaLut,1)
        sub	ebx,ebx
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx
        add     edi,10H
        SETPUTPTR ds,esi,edi
CLOSEPROC
ENDIF

CLOSESEG _NVTEXT32

END

