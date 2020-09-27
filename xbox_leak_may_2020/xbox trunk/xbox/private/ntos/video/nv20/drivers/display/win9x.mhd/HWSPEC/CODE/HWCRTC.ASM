;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwcrtc.asm
;
; Purpose:      This file contains all the hardware specific CRTC
;               specific routines for init. The crtc specific run-
;               time routines are in hwcrtc2.asm.
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include files.inc
include hwmacs.inc
include nvcm.inc
include nvreg.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc
include modeext.inc
include ..\..\..\..\..\sdk\nvidia\inc\nvrmarch.inc
include ..\..\..\..\..\sdk\nvidia\inc\rddapi.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

IFDEF DEBUG
szCreate        DB      'DISPLAY:HW_CreateCrtcContext',0DH,0AH,0
szDestroy       DB      'DISPLAY:HW_DestroyCrtcContext',0DH,0AH,0
ENDIF

;==============================================================================
;
; Function:     HW_CreateCrtcContext
;
; Purpose:      This routine allocates memory for and initializes
;               a HWCRTCCX structure.
;
; Arguments:
;               ds                      The context selector.
;               pHWBoardCXOffset        HWBOARDCX ptr
;
; Returns:      eax     0       failure
;                       non-0   HWCRTCCX ptr
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_CreateCrtcContext, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
OPENPROC
        PUSHR   esi,edi

        sub     esi,esi
        pushd   SIZE HWCRTCCX
        call    HW_AllocFlatMemory
        or      eax,eax
        je      DoneIt
        mov     esi,eax

        ; We are initting
        mov     ds:[esi].HWCRTCCX.dwInitting,1

        ; Save the HWBoardCX owning this CRTC
        mov     edi,pHWBoardCXOffset
        mov     ds:[esi].HWCRTCCX.pHWBoardCX,edi

        ; For the rest of init, just assume head 0. It won't
        ; matter. This will all get updated later.
        mov     ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance,0

        ; We may not be head 0, but that doesn't really matter.
        ; We just need to get the max cacheable cursors, which
        ; we will assume is the same for both heads. The cursor
        ; cache offset will actually be regotten later so if
        ; the head is different it won't matter.
        push    pHWBoardCXOffset
        pushd   0
        call    NV_GetCursorCacheInfo
        mov     ds:[esi].HWCRTCCX.dwCursorCacheVramOffset,eax
        mov     ds:[esi].HWCRTCCX.dwMaxCachedCursors,ebx

        ; Determine the cursor alpha value... Init to -1 for now
        mov     ds:[esi].HWCRTCCX.dwCursorAlpha,-1

        ; Get the encoder information
        push    esi
        call    NV_GetEncoderInfo

        ; Now set it to -1 to show there is no instance
        mov     ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance,-1

IFDEF DEBUG
        mov     ax,cs
        shl     eax,10H
        lea     ax,szCreate
        push    eax
        call    GENERIC_OutputDebugString
ENDIF

        mov     eax,esi

DoneIt:
        ; If eax is 0, then we failed, BUT if esi is non-0, then
        ; some of the HWCRTCCX was filled in. We must handle
        ; this case and free any portion that got initted.
        or      eax,eax
        jne     ExitIt
        or      esi,esi
        je      ExitIt

        ; HW_DestroyBoardContext can clean up partial inits
        push    esi
        call    HW_DestroyCrtcContext
        sub     eax,eax

ExitIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_DestroyCrtcContext
;
; Purpose:      This routine allocates memory for and initializes
;               an array of HWCRTCCX structure.
;
; Arguments:
;               ds              The context selector.
;               pHWCrtcCXOffset offset of HWCRTCCX to destroy.
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_DestroyCrtcContext, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      DoneIt

IFDEF DEBUG
        mov     ax,cs
        shl     eax,10H
        lea     ax,szDestroy
        push    eax
        call    GENERIC_OutputDebugString
ENDIF

        push    esi
        call    NV_FreeHeadRelatedInfo

        ; Clear out the HWBoard ptr
        mov     ds:[esi].HWCRTCCX.pHWBoardCX,0

        push    esi
        call    HW_FreeFlatMemory

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_AllocHeadRelatedInfo
;
; Purpose:      This routine allocates all head related info. This
;               include the VidLutCursorDac and the physical CRTC.
;
; Arguments:
;               ds                      The context selector.
;               pHWCrtcCXOffset         HWCRTCCX ptr
;               dwPhysicalHead          physical head to use
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_AllocHeadRelatedInfo, PASCAL, FRAME, NEAR
PARMD   pHWCrtcCXOffset
PARMD   dwPhysicalHead
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      DoneIt

        mov     eax,dwPhysicalHead
        mov     ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance,eax
        mov     edi,ds:[esi].HWCRTCCX.pHWBoardCX

        ; Ask the board for the cursor cache offset and the
        ; maximum size of the cursor cache ...
        push    edi
        push    ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        call    NV_GetCursorCacheInfo
        mov     ds:[esi].HWCRTCCX.dwCursorCacheVramOffset,eax
        mov     ds:[esi].HWCRTCCX.dwMaxCachedCursors,ebx

        ; Allocate a VideoLutCursorDac object (this shouldn't fail)
        call    CreateVideoLutCursorDac
DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_FreeHeadRelatedInfo
;
; Purpose:      This routine frees all head related info. This
;               include the VidLutCursorDac and the physical CRTC.
;
; Arguments:
;               ds                      The context selector.
;               pHWCrtcCXOffset         HWCRTCCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_FreeHeadRelatedInfo, PASCAL, FRAME, NEAR
PARMD   pHWCrtcCXOffset
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      DoneIt

        mov     edi,ds:[esi].HWCRTCCX.pHWBoardCX
        call    DestroyVideoLutCursorDac

        mov     ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance,-1
DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_CrtcSetLogdev
;
; Purpose:      This routine tells the crtc what logical device
;               owns it.
;
; Arguments:
;               ds                      The context selector.
;               pHWCrtcCXOffset         HWCRTCCX ptr
;               pHWLogdevCXOffset       HWLOGDEVCX ptr
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_CrtcSetLogdev, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   pHWLogdevCXOffset
OPENPROC
        mov     ebx,pHWCrtcCXOffset
        or      ebx,ebx
        je      SkipSet
        mov     eax,pHWLogdevCXOffset
        mov     ds:[ebx].HWCRTCCX.pHWLogdevCX,eax
SkipSet:
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetEncoderInfo
;
; Purpose:      This routine gets the Encoder type on the card and
;               the I2C address at which it is located.
;
; Arguments:
;               ds                      The context selector.
;               pHWCrtcCXOffset         HWCRTCCX ptr
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_GetEncoderInfo, PASCAL, FRAME, NEAR
PARMD   pHWCrtcCXOffset
LOCALV  sVidTypeParams,NV_CFGEX_VIDEO_ENCODER_TYPE_PARAMS
LOCALV  sVidIDInfoParams,NV_CFGEX_VIDEO_ENCODER_ID_PARAMS
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      SkipGet
        mov     edi,ds:[esi].HWCRTCCX.pHWBoardCX

        ; Get the CRTC head number.
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        mov     sVidTypeParams.Head,eax
        mov     sVidIDInfoParams.Head,eax

        ; Get the encoder type
        mov     ax,ss
        shl     eax,10H
        lea     ax,sVidTypeParams
        mov     ebx,SIZE NV_CFGEX_VIDEO_ENCODER_TYPE_PARAMS
        mov     ecx,NV_CFGEX_VIDEO_ENCODER_TYPE
        push    esi
        mov     esi,edi
        call    NV_ConfigExCall
        pop     esi
        mov     eax,sVidTypeParams.EncoderdwType
        mov     ds:[esi].HWCRTCCX.dwTVOutDeviceID,eax

        mov     ax,ss
        shl     eax,10H
        lea     ax,sVidIDInfoParams
        mov     ebx,SIZE NV_CFGEX_VIDEO_ENCODER_ID_PARAMS
        mov     ecx,NV_CFGEX_VIDEO_ENCODER_ID
        push    esi
        mov     esi,edi
        call    NV_ConfigExCall
        pop     esi
        mov     eax,sVidIDInfoParams.EncoderID
        shr     eax,1
        mov     ds:[esi].HWCRTCCX.dwTVOutDeviceAddr,eax

SkipGet:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetMaxTVOutResolution
;
; Purpose:      Find out the maximum resolution for Encoder
;
; Arguments:
;               ds                      The context selector.
;               pHWCrtcCXOffset         HWCRTCCX ptr
;
; Returns:      eax     XRes, YRes
;               ebx     TVEncoder ID
;
;==============================================================================
DECPROC HW_GetMaxTVOutResolution, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
OPENPROC
        mov     ecx,pHWCrtcCXOffset

        ; Set 1024X768 for Brooktree 871
        mov     eax,04000300h
        mov     ebx,ds:[ecx].HWCRTCCX.dwTVOutDeviceID
        cmp     ebx,NV_ENCODER_BROOKTREE_871
        je      @F
        ; Default 800X600
        mov     eax,03200258h
@@:
CLOSEPROC



;==============================================================================
;
; Function:     CreateVideoLutCursorDac
;
; Purpose:      This function creates a VideoLutCursorDac object
;               for use in the WIN_CHANNEL.
;
; Arguments:
;               ds:esi  HWCRTCCX
;               ds:edi  HWBOARDCX
;
; Returns:
;               eax     0       failure
;                       non-0   Success
;
; Preserve:     ds,esi
;==============================================================================
DECPROC CreateVideoLutCursorDac, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Allocate a Context DMA for the VidLutCurDac notifier
        mov     ds:[esi].HWCRTCCX.bNotifierCreated,0
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     eax,OBJ_VIDEO_LUT_CURSOR_DAC_NOTIFIER
        push    edi
        push    eax
        lea     ebx,[esi].HWCRTCCX.sVideoLutCursorDacNotifier0
        push    ebx
        mov     ax,ds
        push    eax
        pushd   9 * (SIZE NvNotification)
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY
        call    NV_AllocContextDma
        or      eax,eax
        je      DoneIt
        mov     ds:[esi].HWCRTCCX.bNotifierCreated,1

        ; Allocate a Context DMA for the Palette
        mov     ds:[esi].HWCRTCCX.bPaletteContextDmaCreated,0
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     eax,OBJ_DMA_IN_SYSRAM_FOR_PALETTE
        push    edi
        push    eax
        lea     eax,[esi].HWCRTCCX.dwPaletteTable
        push    eax
        mov     ax,ds
        push    eax
        pushd   400H
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY
        call    NV_AllocContextDma
        or      eax,eax
        je      DoneIt
        mov     ds:[esi].HWCRTCCX.bPaletteContextDmaCreated,1

        ; Allocate a Context DMA for the Gamma
        mov     ds:[esi].HWCRTCCX.bGammaContextDmaCreated,0
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     eax,OBJ_DMA_IN_SYSRAM_FOR_GAMMA
        push    edi
        push    eax
        lea     eax,[esi].HWCRTCCX.dwGammaTable
        push    eax
        mov     ax,ds
        push    eax
        pushd   400H
        pushd   ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY
        call    NV_AllocContextDma
        or      eax,eax
        je      DoneIt
        mov     ds:[esi].HWCRTCCX.bGammaContextDmaCreated,1

        ; Create the object
        mov     ds:[esi].HWCRTCCX.bVidLutCursorDacObjectCreated,0
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     eax,OBJ_VIDEO_LUT_CURSOR_DAC
        push    esi
        push    edi
        pushd   NV15_VIDEO_LUT_CURSOR_DAC
        push    eax
        call    NV_RMAllocObject
        or      eax,eax
        je      @F
        jmp     ObjectCreated
@@:
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     eax,OBJ_VIDEO_LUT_CURSOR_DAC
        push    edi
        pushd   NV04_VIDEO_LUT_CURSOR_DAC
        push    eax
        call    NV_AllocWinChannelObject
        or      eax,eax
        je      DoneIt
        
ObjectCreated:
        mov     ds:[esi].HWCRTCCX.bVidLutCursorDacObjectCreated,1

        ; Set the object in a sub-channel
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,edi,eax

        mov     edx,edi
        mov     edi,ds:[edi].HWBOARDCX.pCurrentPushBufferOffset

        CHECKDMASPACE ds,edx,edi,18H
        mov     eax,CHNRUN(VideoLutCursorDacHdr.object,1)
        mov     ebx,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     ebx,OBJ_VIDEO_LUT_CURSOR_DAC
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(VideoLutCursorDac.SetContextDmaNotifies,1)
        mov     ebx,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     ebx,OBJ_VIDEO_LUT_CURSOR_DAC_NOTIFIER
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ebx

        ; Program the non-volatile data
        mov     eax,CHNRUN(VideoLutCursorDac.SetContextDmaImage,1)
        mov     ebx,OBJ_DMA_IN_VRAM
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ebx

        add     edi,18H
        SETPUTPTR ds,edx,edi
        mov     edi,edx
        mov     eax,1
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:     DestroyVideoLutCursorDac
;
; Purpose:      This function destroys the VideoLutCursorDac object.
;
; Arguments:
;               ds:esi  HWCRTCCX
;               ds:edi  HWBOARDCX
;
; Returns:      None
;
; Preserve:     ds,esi
;==============================================================================
DECPROC DestroyVideoLutCursorDac, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Destroy the VidLutCurDac object
        cmp     ds:[esi].HWCRTCCX.bVidLutCursorDacObjectCreated,0
        je      @F
        push    edi
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     eax,OBJ_VIDEO_LUT_CURSOR_DAC
        push    eax
        call    NV_FreeWinChannelObject
        mov     ds:[esi].HWCRTCCX.bVidLutCursorDacObjectCreated,0
@@:
        ; Destroy the Context DMA for the gamma
        cmp     ds:[esi].HWCRTCCX.bPaletteContextDmaCreated,0
        je      @F
        push    edi
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     eax,OBJ_DMA_IN_SYSRAM_FOR_PALETTE
        push    eax
        call    NV_FreeWinChannelObject
        mov     ds:[esi].HWCRTCCX.bPaletteContextDmaCreated,0
@@:
        ; Destroy the Context DMA for the gamma
        cmp     ds:[esi].HWCRTCCX.bGammaContextDmaCreated,0
        je      @F
        push    edi
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     eax,OBJ_DMA_IN_SYSRAM_FOR_GAMMA
        push    eax
        call    NV_FreeWinChannelObject
        mov     ds:[esi].HWCRTCCX.bGammaContextDmaCreated,0
@@:
        ; Destroy the Context DMA for the VidLutCurDac notifier.
        cmp     ds:[esi].HWCRTCCX.bNotifierCreated,0
        je      @F
        push    edi
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     eax,OBJ_VIDEO_LUT_CURSOR_DAC_NOTIFIER
        push    eax
        call    NV_FreeWinChannelObject
        mov     ds:[esi].HWCRTCCX.bNotifierCreated,0
@@:
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetDacLimits
;
; Purpose:      This routine gets the DAC limits in all pixel depths.
;
; Arguments:
;               ds              Context Selector
;               pHWCrtcCXOffset HWCRTCCX ptr
;
; Returns:
;               eax             Dac Speed limit in 8bpp in Hertz
;               ebx             Dac Speed limit in 16bpp in Hertz
;               ecx             Dac Speed limit in 32bpp in Hertz
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetDacLimits, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
LOCALD  dw8BppLimit
LOCALD  dw16BppLimit
LOCALD  dw32BppLimit
LOCALV  sDACInfoParams,NV_CFGEX_DAC_PCLK_LIMIT_PARAMS
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      SkipGet
        
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        mov     sDACInfoParams.Head,eax
        mov     esi,ds:[esi].HWCRTCCX.pHWBoardCX

        mov     ax,ss
        shl     eax,10H
        lea     ax,sDACInfoParams
        mov     ebx,SIZE NV_CFGEX_DAC_PCLK_LIMIT_PARAMS 
        mov     ecx,NV_CFGEX_DAC_PCLK_LIMIT_8BPP
        call    NV_ConfigExCall
        mov     eax,sDACInfoParams.pclkLimit
        mov     dw8BppLimit,eax

        mov     ax,ss
        shl     eax,10H
        lea     ax,sDACInfoParams
        mov     ebx,SIZE NV_CFGEX_DAC_PCLK_LIMIT_PARAMS 
        mov     ecx,NV_CFGEX_DAC_PCLK_LIMIT_16BPP
        call    NV_ConfigExCall
        mov     eax,sDACInfoParams.pclkLimit
        mov     dw16BppLimit,eax

        mov     ax,ss
        shl     eax,10H
        lea     ax,sDACInfoParams
        mov     ebx,SIZE NV_CFGEX_DAC_PCLK_LIMIT_PARAMS 
        mov     ecx,NV_CFGEX_DAC_PCLK_LIMIT_32BPP
        call    NV_ConfigExCall
        mov     eax,sDACInfoParams.pclkLimit
        mov     dw32BppLimit,eax

        mov     eax,dw8BppLimit
        mov     ebx,dw16BppLimit
        mov     ecx,dw32BppLimit
        
SkipGet:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_BitsPerColorGun
;
; Purpose:      This routine returns the number of bits per
;               color channel in the CRTC given.
;
; Arguments:
;               ds                      The context selector.
;               pHWCrtcCXOffset         offset of HWCRTCCX
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_BitsPerColorGun, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
OPENPROC
        mov     eax,8
CLOSEPROC


;==============================================================================
;
; Function:     HW_SizeCursorCache
;
; Purpose:      This routine tells the hardware the maximum
;               number of cursors the generic cursor cache
;               can support. The hardware returns the number
;               of cursors it wants to cache that is less than
;               or equal to the number provided.
;
; Arguments:
;               ds                      The context selector.
;               pHWCrtcCXOffset         offset of HWCRTCCX
;               dwMaxCursorCacheSize    maximum size of cursor cache
;
; Returns:
;               eax             Number of cursors the hardware wants
;                               to cache. Must be less than
;                               dwMaxCursorCacheSize
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_SizeCursorCache, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   dwMaxCursorCacheSize
OPENPROC
        mov     ebx,pHWCrtcCXOffset
        or      ebx,ebx
        je      @F
        mov     eax,dwMaxCursorCacheSize
        cmp     eax,ds:[ebx].HWCRTCCX.dwMaxCachedCursors
        jc      @F
        mov     eax,ds:[ebx].HWCRTCCX.dwMaxCachedCursors
@@:
CLOSEPROC


;==============================================================================
;
; Function:     HW_IsCRTConnected
;
; Purpose:      This function returns TRUE if the CRT is connected
;               and FALSE if it is not.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX ptr
;
; Returns:      eax     TRUE  if CRT is connected
;                       FALSE if it is not connected
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_IsCRTConnected, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
LOCALV  sCRTInfoParams,NV_CFGEX_GET_MONITOR_INFO_MULTI_PARAMS
OPENPROC
        PUSHR   esi
        mov     esi,pHWCrtcCXOffset
        sub     eax,eax
        or      esi,esi
        je      DoneIt

        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        mov     sCRTInfoParams.Head,eax
        mov     esi,ds:[esi].HWCRTCCX.pHWBoardCX
        .errnz  NV_CFGEX_GET_MONITOR_INFO_NOT_CONNECTED

        sub     eax,eax
        mov     sCRTInfoParams.MonitorConnected,eax
        mov     ax,ss
        shl     eax,10H
        lea     ax,sCRTInfoParams
        mov     ebx,SIZE NV_CFGEX_GET_MONITOR_INFO_MULTI_PARAMS
        mov     ecx,NV_CFGEX_GET_MONITOR_INFO_MULTI
        call    NV_ConfigExCall
        mov     eax,sCRTInfoParams.MonitorConnected
DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_IsDFPConnected
;
; Purpose:      This function returns TRUE if the DFP is connected
;               and FALSE if it is not.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX ptr
;
; Returns:      eax     TRUE  if DFP is connected
;                       FALSE if it is not connected
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_IsDFPConnected, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
LOCALV  sDFPInfoParams,NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS
OPENPROC
        PUSHR   esi
        mov     esi,pHWCrtcCXOffset
        sub     eax,eax
        or      esi,esi
        je      DoneIt

        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        mov     sDFPInfoParams.Head,eax
        mov     esi,ds:[esi].HWCRTCCX.pHWBoardCX

        .errnz  NV_CFGEX_GET_FLATPANEL_INFO_NOT_CONNECTED

        sub     eax,eax
        mov     sDFPInfoParams.FlatPanelConnected,eax
        mov     ax,ss
        shl     eax,10H
        lea     ax,sDFPInfoParams
        mov     ebx,SIZE NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS
        mov     ecx,NV_CFGEX_GET_FLAT_PANEL_INFO_MULTI
        call    NV_ConfigExCall
        mov     eax,sDFPInfoParams.FlatPanelConnected
DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_IsTVConnected
;
; Purpose:      This function returns TRUE if the TV is connected
;               and FALSE if it is not.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX ptr
;
; Returns:      eax     TRUE  if tv is connected
;                       FALSE if it is not connected
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_IsTVConnected, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
LOCALV  sTVInfoParams,NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS
OPENPROC
        PUSHR   esi
        mov     esi,pHWCrtcCXOffset
        sub     eax,eax
        or      esi,esi
        je      DoneIt

        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        mov     sTVInfoParams.Head,eax
        mov     esi,ds:[esi].HWCRTCCX.pHWBoardCX

        .errnz  TV_NOT_CONNECTED

        sub     eax,eax
        mov     sTVInfoParams.TVConnected,eax
        mov     ax,ss
        shl     eax,10H
        lea     ax,sTVInfoParams
        mov     ecx,NV_CFGEX_GET_TV_ENCODER_INFO_MULTI
        call    NV_ConfigExCall
        mov     eax,sTVInfoParams.TVConnected
DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetMaxDfpScaledXRes
;
; Purpose:      This function returns the maximum scanline length
;               in pixels that the hardware can scale to on a DFP.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX ptr
;
; Returns:      eax     max XRes scaling length in pixels
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetMaxDfpScaledXRes, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
LOCALV  sTVInfoParams,NV_CFGEX_TV_ENCODER_PARAMS
OPENPROC
        mov     eax,400H
CLOSEPROC


;==============================================================================
;
; Function:     HW_IsMacroVisionEncoderAttached
;
; Purpose:      This function returns whether or not we have a macro
;               vision capable TV out device attached.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX ptr
;
; Returns:      eax                     0       then no macrovision encoder
;                                       non-0   then yes
;
; Preserve:     ds,esi,edi,es
;==============================================================================
DECPROC HW_IsMacroVisionEncoderAttached, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
OPENPROC
        mov     ebx,pHWCrtcCXOffset
        sub     eax,eax
        or      ebx,ebx
        je      DoneIt
        mov     ebx,ds:[ebx].HWCRTCCX.dwTVOutDeviceID

        ; Check for devices that is not support macrovision
        cmp     ebx,NV_ENCODER_BROOKTREE_868
        je      DoneIt
        cmp     ebx,NV_ENCODER_CHRONTEL_7003
        je      DoneIt
        cmp     ebx,NV_ENCODER_CHRONTEL_7006
        je      DoneIt
        cmp     ebx,NV_ENCODER_CHRONTEL_7008
        je      DoneIt

        ; We assume all next generations should support macrovision.
        mov     eax,ebx
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:     HW_ReadEdid
;
; Purpose:      This function reads the EDID.
;
; Arguments:
;               ds              Context Selector
;               pHWCrtcCXOffset offset in Context selector to HWCRTCCX
;               dwDevData       type in low byte, num in next byte
;               pEdidBuffer     offset to EDID buffer
;               pEdidSize       offset to place to store edid size
;                               The variable this points to has been
;                               set to the maximum size it can handle.
;
; Returns:      eax     0       then there was a problem reading the EDID.
;                       non-0   then there was no problem
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_ReadEdid, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   dwDevData
PARMD   pEdidBuffer
PARMD   pEdidSize
LOCALV  sEdidParams,NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS
OPENPROC
        PUSHR   esi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      WasProb

        ; Form the bitmask 1 << (devtype * 8 + dev num) and
        ; see if device port is even available
        mov     ecx,dwDevData
        shl     cl,3
        add     cl,ch
        mov     eax,1
        shl     eax,cl
        mov     sEdidParams.displayMap,eax

        ; Get the base address of the CX selector
        mov     ax,ds
        push    eax
        call    GN_GetSelectorBase

        ; Build a flat offset to the EdidBuffer and EdidSize
        mov     edx,pEdidBuffer
        add     edx,eax
        mov     sEdidParams.edidBuffer,edx
        mov     edx,pEdidSize
        add     eax,edx
        mov     sEdidParams.bufferLength,eax

        ; Make the call to the RM to read the EDID
        mov     esi,ds:[esi].HWCRTCCX.pHWBoardCX
        mov     ax,ss
        shl     eax,10H
        lea     ax,sEdidParams
        mov     ebx,SIZE NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS
        mov     ecx,NV_CFGEX_GET_LOGICAL_DEV_EDID
        call    NV_ConfigExCall

        or      eax,eax
        mov     eax,1
        je      DoneIt

WasProb:
        sub     eax,eax
        mov     ecx,pEdidSize
        mov     ds:[ecx],eax

DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_ReadEdidForDevice
;
; Purpose:      This function reads the EDID to a buffer for the specific
;               display device requested (i.e., CRT0, DFP1, . . .)
;
; Arguments:
;               ds              Context Selector
;               pHWLogDevCXOffset offset in Context selector to HWLOGDEVCX
;               dwDisplayDev    display type device mask for device to read
;               lpEdidBuffer    ptr to EDID buffer
;               lpEdidSize      ptrt to place to store edid size
;                               The variable this points to has been
;                               set to the maximum size it can handle.
;
; Returns:      eax     0       then there was a problem reading the EDID.
;                       non-0   then there was no problem
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_ReadEdidForDevice, PASCAL, FRAME, FAR32
PARMD   pHWLogDevCxOffset
PARMD   dwDisplayDev
PARMD   lpEdidBuffer
PARMD   lpEdidSize
LOCALV  sEdidParams,NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS
OPENPROC
        PUSHR   esi
        mov     esi,pHWLogDevCxOffset
        or      esi,esi
        je      WasProb

        ; Get the base address of the selector for EdidBuffer. This will
        ; also be used for EdidSize.
        mov     eax,lpEdidBuffer
        shr     eax,10h
        push    eax
        call    GN_GetSelectorBase

        ; Build a flat offset to the EdidBuffer and EdidSize
        sub     edx,edx
        les     dx,lpEdidBuffer
        add     edx,eax
        mov     sEdidParams.edidBuffer,edx
        sub     edx,edx
        les     dx,lpEdidSize
        add     eax,edx
        mov     sEdidParams.bufferLength,eax

        ; Make the call to the RM to read the EDID. Set the displayMap to DFP0.
        mov     eax,dwDisplayDev
        mov     sEdidParams.displayMap,eax
        mov     esi,ds:[esi].HWLOGDEVCX.pHWBoardCX
        mov     ax,ss
        shl     eax,10H
        lea     ax,sEdidParams
        mov     ebx,SIZE NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS
        mov     ecx,NV_CFGEX_GET_LOGICAL_DEV_EDID
        call    NV_ConfigExCall

        or      eax,eax
        mov     eax,1
        je      DoneIt

WasProb:
        sub     eax,eax

DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetPhysicalCrtcIndex
;
; Purpose:      This function returns the physical CRTC index that this
;               HWCRTCCX is attached to.
;
; Arguments:
;               ds              Context Selector
;               pHWCrtcCXOffset offset in Context selector to HWCRTCCX
;
; Returns:      eax     0-based physical CRTC number
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetPhysicalCrtcIndex, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
OPENPROC
        mov     ebx,pHWCrtcCXOffset
        or      ebx,ebx
        je      @F
        sub     eax,eax
        or      ebx,ebx
        je      @F
        mov     eax,ds:[ebx].HWCRTCCX.dwPhysicalCRTCInstance
@@:
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetBootDevice
;
; Purpose:      This function returns the device the HW would like us
;               to use for this CRTC.
;
; Arguments:
;               ds              Context Selector
;               pHWCrtcCXOffset offset in Context selector to HWCRTCCX
;               dwDeviceInstance Primary device (0), secondary device (1), etc.
;
; Returns:      eax             bits 7:0  - Device Type to use
;                               bits 15:8 - Device Number to use
;                               bits 23:16 - format to use if type is TV
;                       -1 if no there are fewer than dwDeviceInstance+1
;                       bits set.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetBootDevice, PASCAL, FRAME, FAR32
PARMD   pHWBoardCXOffset
PARMD   dwDeviceInstance
LOCALD  dwFoundInstance
LOCALB  dbDeviceNumber
OPENPROC
        mov     ebx,pHWBoardCXOffset
        sub     eax,eax
        or      ebx,ebx
        je      NoDevice

        cmp     dwDeviceInstance,7
        ja      NoDevice

        mov     dbDeviceNumber,0
        mov     dwFoundInstance,0

        pushd   NV_CFG_GET_BOOT_DEVICES
        push    ds:[ebx].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigGet,es,ax

        ; Result from ConfigGet is in dx:ax
        ; Put CRTs in DL, TVs in CL, and DFPs in CH. DH = bit to test.
        mov     ch,dl
        mov     cl,ah
        mov     dl,al
        mov     dh,1
        sub     eax,eax

        ; The order of precedence is DFP0, CRT0, TV0, DFP1, CRT1, TV1, . . .
DeviceNumberLoop:

        mov     al,DEVTYPE_DFP
        test    ch,dh
        jz      CheckCRT
        mov     ebx,dwDeviceInstance
        cmp     dwFoundInstance,ebx
        je      FoundDevice
        inc     dwFoundInstance

CheckCRT:
        mov     al,DEVTYPE_CRT
        test    dl,dh
        jz      CheckTV
        mov     ebx,dwDeviceInstance
        cmp     dwFoundInstance,ebx
        je      FoundDevice
        inc     dwFoundInstance

CheckTV:
        mov     al,DEVTYPE_TV
        test    cl,dh
        jz      NextDeviceNumber
        mov     ebx,dwDeviceInstance
        cmp     dwFoundInstance,ebx
        je      FoundDevice
        inc     dwFoundInstance

NextDeviceNumber:
        cmp     dh,80h
        je      NoDevice
        shl     dh,1
        inc     dbDeviceNumber
        jmp     DeviceNumberLoop

FoundDevice:
        mov     ah,dbDeviceNumber
        cmp     al,DEVTYPE_TV
        jne     DoneIt

        ; With the TV, we need the format also
        push    eax
        mov     ebx,pHWBoardCXOffset
        pushd   NV_CFG_BIOS_DEFAULT_TV_TYPE
        push    ds:[ebx].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigGet,es,ax
        movzx   ecx,ax
        call    HW_MapHWTvFormatToGNTvFormat
        shl     ecx,10H
        pop     eax
        or      eax,ecx
        jmp     DoneIt

NoDevice:
        mov     eax,-1
        jmp     DoneIt

DoneIt:

CLOSEPROC


;==============================================================================
;
; Function:     HW_GetResManTvFormat
;
; Purpose:      This function returns the TV Format
;
; Arguments:
;               ds              Context Selector
;               pHWCrtcCXOffset offset in Context selector to HWCRTCCX
;               dwDevData       Device for which we want the TV Format
;
; Returns:      eax             TvFormat                
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetResManTvFormat, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   dwDevData
LOCALV  sTVParams,NV_CFGEX_GET_VIDEO_TV_STANDARD_PARAMS
OPENPROC

        PUSHR   esi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      WasProb

        ; Make sure this is a TV or don't call ResMan
        mov     cl,(DEVDATA PTR dwDevData).cType
        cmp     cl,DEVTYPE_TV
        jne     WasProb
        
        ; Form the bitmask 1 << (devtype * 8 + dev num) and
        ; see if device port is even available
        mov     ecx,dwDevData
        shl     cl,3
        add     cl,ch
        mov     eax,1
        shl     eax,cl
        mov     sTVParams.DevicesConfig,eax

        ; Make the call to the RM to read the EDID
        mov     esi,ds:[esi].HWCRTCCX.pHWBoardCX
        mov     ax,ss
        shl     eax,10H
        lea     ax,sTVParams
        mov     ebx,SIZE NV_CFGEX_GET_VIDEO_TV_STANDARD_PARAMS
        mov     ecx,NV_CFGEX_GET_VIDEO_TV_STANDARD
        call    NV_ConfigExCall

        or      eax,eax
        mov     eax,sTVParams.TVStandard
        je      DoneIt

WasProb:
        sub     eax,eax

DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_SetResManTvFormat
;
; Purpose:      This function sets the Tv Format in the ResMan
;
; Arguments:
;               ds              Context Selector
;               pHWCrtcCXOffset offset in Context selector to HWCRTCCX
;               dwDevData       Device and TV Format to set
;
; Returns:      
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_SetResManTvFormat, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   dwDevData
LOCALV  sTVParams,NV_CFGEX_GET_VIDEO_TV_STANDARD_PARAMS
OPENPROC

        PUSHR   esi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      WasProb

        ; Make sure this is a TV or don't call ResMan
        mov     cl,(DEVDATA PTR dwDevData).cType
        cmp     cl,DEVTYPE_TV
        jne     WasProb
        
        ; Form the bitmask 1 << (devtype * 8 + dev num) and
        ; see if device port is even available
        mov     ecx,dwDevData
        shl     cl,3
        add     cl,ch
        mov     eax,1
        shl     eax,cl
        mov     sTVParams.DevicesConfig,eax
        
        ; Store the TV Format we want to set in sTVParams.
        mov     ecx,dwDevData
        shr     ecx,10h
        and     ecx,0FFh        
        mov     sTVParams.TVStandard,ecx

        ; Make the call to the RM to read the EDID
        mov     esi,ds:[esi].HWCRTCCX.pHWBoardCX
        mov     ax,ss
        shl     eax,10H
        lea     ax,sTVParams
        mov     ebx,SIZE NV_CFGEX_GET_VIDEO_TV_STANDARD_PARAMS
        mov     ecx,NV_CFGEX_GET_VIDEO_TV_STANDARD
        call    NV_ConfigExCall

        or      eax,eax
        je      DoneIt

WasProb:
        sub     eax,eax

DoneIt:
        POPR    esi
CLOSEPROC


CLOSESEG _NVTEXT32

END

