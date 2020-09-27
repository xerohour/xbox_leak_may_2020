;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwcrtc2.asm
;
; Purpose:      This file contains all the hardware specific CRTC
;               routines for run-time. The CRTC specific init
;               routines are in hwcrtc.asm.
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
PUBLIC  TableMonitorTypeRMToModeset
TableMonitorTypeRMToModeset     LABEL   BYTE
DB      DEVTYPE_CRT, DEVTYPE_CRT, DEVTYPE_TV, DEVTYPE_TV, DEVTYPE_DFP

PUBLIC  TableMonitorTypeModesetToRM
TableMonitorTypeModesetToRM     LABEL   BYTE
DB      MONITOR_TYPE_VGA, MONITOR_TYPE_NTSC, MONITOR_TYPE_FLAT_PANEL

PUBLIC  TableTvFormatRMToModeset
TableTvFormatRMToModeset        LABEL   BYTE
DB      TVTYPE_NTSCM, TVTYPE_NTSCJ, TVTYPE_PALM, TVTYPE_PALA, TVTYPE_PALN
DB      TVTYPE_PALNC

PUBLIC  TableTvFormatModesetToRM
TableTvFormatModesetToRM        LABEL   BYTE
DB      NTSC_M, NTSC_J, PAL_M, PAL_A, PAL_N, PAL_NC

IFDEF DEBUG
szSetMode       DB      'DISPLAY:HW_SetCrtcMode',0DH,0AH,0
ENDIF

;==============================================================================
;
; Function:     HW_CrtcMessage
;
; Purpose:      This routine gets called whenver there is an
;               message to send to all objects. There are
;               a few different messages.  See gngdi.inc
;               for details.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX ptr
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
DECPROC HW_CrtcMessage, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   dwMessage
PARMD   dwData1
PARMD   dwData2
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      DoneIt

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
        cmp     eax,MSG_DISABLE
        je      DrvDis
        jmp     DoneIt

PreMS:
        jmp     DoneIt

PostMS:
        ; We are no longer initting after the first modeset
        mov     ds:[esi].HWCRTCCX.dwInitting,0
        jmp     DoneIt

DXEnum:
DXOn:
DXOff:
        jmp     DoneIt

DosOff:
        jmp     DoneIt

DosOn:
        jmp     DoneIt

DrvDis:

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_SetCrtcMode
;
; Purpose:      This routine gets called to stuff the CRTCs
;               timings into the push buffer and physically
;               set the mode.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX ptr
;               lpModeOut               16:16 ptr to MODEOUT
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_SetCrtcMode, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   dwImageOffset
PARMD   lpModeOut
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      SkipSet

IFDEF DEBUG
        mov     ax,cs
        shl     eax,10H
        lea     ax,szSetMode
        push    eax
        call    GENERIC_OutputDebugString
ENDIF

        sub     ebx,ebx
        les     bx,lpModeOut

        mov     edx,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        mov     ecx,ds:[esi].HWCRTCCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ecx,eax

        mov     edi,ds:[ecx].HWBOARDCX.pCurrentPushBufferOffset

        SETDMASTATUS <ds:[esi].HWCRTCCX.sVideoLutCursorDacNotifier1.status>

        CHECKDMASPACE ds,ecx,edi,38H

        ; Set the object in a sub-channel.
        mov     eax,CHNRUN(VideoLutCursorDacHdr.object,1)
        mov     ds:[edi + 00H],eax
        add     edx,OBJ_VIDEO_LUT_CURSOR_DAC
        mov     ds:[edi + 04H],edx

        ; get X-res and Y-res
        mov     eax,CHNRUN(VideoLutCursorDac.SetDac.imagedwSize,8)
        mov     ds:[edi + 08H],eax
        mov     eax,dword ptr es:[ebx].MODEOUT.wXRes
        mov     ds:[edi + 0CH],eax

        ; horz blank start and width
        mov     eax,dword ptr es:[ebx].MODEOUT.wHBlankStart
        mov     ds:[edi + 10H],eax

        ; horz sync start and width
        mov     eax,dword ptr es:[ebx].MODEOUT.wHSyncStart
        mov     ds:[edi + 14H],eax

        ; vert blank start and width
        mov     eax,dword ptr es:[ebx].MODEOUT.wVBlankStart
        mov     ds:[edi + 18H],eax

        ; vert sync start and width
        mov     eax,dword ptr es:[ebx].MODEOUT.wVSyncStart
        mov     ds:[edi + 1CH],eax

        ; set total size
        mov     eax,dword ptr es:[ebx].MODEOUT.wHTotal
        mov     ds:[edi + 20H],eax

        ; set pixel clock
        mov     eax,dword ptr es:[ebx].MODEOUT.dwPixelClock
        mov     ds:[edi + 24H],eax

        ; set format
        mov     eax,dword ptr es:[ebx].MODEOUT.dwFormat
        mov     ds:[edi + 28H],eax

        ; set image offset and pitch
        mov     eax,CHNRUN(VideoLutCursorDac.SetImage.dwdwOffset,2)
        mov     edx,dwImageOffset
        mov     ds:[edi + 2CH],eax
        mov     ds:[edi + 30H],edx
        mov     edx,dword ptr es:[ebx].MODEOUT.dwPitchInBytes
        mov     ds:[edi + 34H],edx
        mov     ds:[esi].HWCRTCCX.dwPanPitch,edx

        add     edi,38H
        SETPUTPTR ds,ecx,edi

        ; Now wait until the notify is complete
        WAITDMASTATUS <ds:[esi].HWCRTCCX.sVideoLutCursorDacNotifier1.status>

SkipSet:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_SetStopImage
;
; Purpose:      This routine gets called to send the StopImage method
;               to the VideoLutCursorDac class. This will blank the screen.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_SetStopImage, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      SkipSet

        mov     edx,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        mov     ecx,ds:[esi].HWCRTCCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ecx,eax

        mov     edi,ds:[ecx].HWBOARDCX.pCurrentPushBufferOffset

        SETDMASTATUS <ds:[esi].HWCRTCCX.sVideoLutCursorDacNotifier0.status>

        CHECKDMASPACE ds,ecx,edi,18H

        ; Set the object in a sub-channel.
        mov     eax,CHNRUN(VideoLutCursorDacHdr.object,1)
        mov     ds:[edi + 00H],eax
        add     edx,OBJ_VIDEO_LUT_CURSOR_DAC
        mov     ds:[edi + 04H],edx

        ; Request a notifier
        mov     eax,CHNRUN(VideoLutCursorDac.Notify,1)
        mov     ds:[edi + 08H],eax
        mov     eax,NV07C_NOTIFY_WRITE_ONLY
        mov     ds:[edi + 0CH],eax
        
        ; Set StopImage.
        mov     eax,CHNRUN(VideoLutCursorDac.StopImage,1)
        mov     ds:[edi + 10H],eax
        mov     eax,NV07C_STOP_IMAGE_AS_SOON_AS_POSSIBLE
        mov     ds:[edi + 14H],eax

        add     edi,18H
        SETPUTPTR ds,ecx,edi

        ; Now wait until the notify is complete
        WAITDMASTATUS <ds:[esi].HWCRTCCX.sVideoLutCursorDacNotifier0.status>

SkipSet:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_SetPanOffset
;
; Purpose:      This routine sets the offset in video memory of the
;               first byte scanned out by this crtc for each frame.
;
; Arguments:
;               ds                      The context selector.
;               pHWCrtcCXOffset         offset of HWCRTCCX
;               dwwPanOffset            offset into the screen surface
;                                       (not into video memory), of the
;                                       new panning position.
;
; Returns:
;               None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_SetPanOffset, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   dwNewPanOffset
OPENPROC
        PUSHR   esi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      SkipPan

        mov     eax,OBJ_VIDEO_LUT_CURSOR_DAC
        mov     edx,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     eax,edx
        mov     ecx,ds:[esi].HWCRTCCX.pHWBoardCX

        push    dwNewPanOffset
        push    ds:[esi].HWCRTCCX.dwPanPitch
        push    ds:[ecx].HWBOARDCX.dwGdiDriverChannelID
        push    eax
        push    ds:[ecx].HWBOARDCX.dwDeviceNum
        push    edx
        call    FFP16 PTR lpfnNvSysUpdateImageOffsetFormat

SkipPan:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_SetGamma
;
; Purpose:      This is the hardware specific SetGamma routine.
;               Place the gamma passed in into the hardware.
;
; Arguments:
;               ds                      CXSel
;               pHWCrtcCXOffset         HWCRTCCX ptr
;               pGammaRamp              ptr to 256 dwords of XBGR
;               dwFirstEntry            0-255 index of first entry to set
;               dwNumEntries            number of contiguous palette
;                                       indices to set
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_SetGamma, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   pGammaRamp
PARMD   dwFirstEntry
PARMD   dwNumEntries
OPENPROC
        PUSHR   esi,edi
        mov     edi,pHWCrtcCXOffset
        or      edi,edi
        je      DoneIt

        ; First, copy the gamma table passed in
        mov     esi,pGammaRamp
        lea     edi,[edi].HWCRTCCX.dwGammaTable
        mov     eax,dwFirstEntry
        lea     esi,[esi][eax * 4]
        lea     edi,[edi][eax * 4]
        mov     ecx,dwNumEntries
@@:
        mov     eax,ds:[esi]
        add     esi,4
        mov     ds:[edi],eax
        add     edi,4
        dec     ecx
        jne     @B

        push    pHWCrtcCXOffset
        pushd   OBJ_DMA_IN_SYSRAM_FOR_GAMMA
        push    dwFirstEntry
        push    dwNumEntries
        call    NV_SetColorMap

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_SetPalette
;
; Purpose:      This is the hardware specific SetGamma routine.
;               Place the gamma passed in into the hardware.
;
; Arguments:
;               ds                      CXSel
;               pHWCrtcCXOffset         HWCRTCCX ptr
;               pPalette                ptr to 256 dwords of XBGR
;               dwFirstEntry            0-255 index of first entry to set
;               dwNumEntries            number of contiguous palette
;                                       indices to set
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_SetPalette, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   pPalette
PARMD   dwFirstEntry
PARMD   dwNumEntries
OPENPROC
        PUSHR   esi,edi
        mov     edi,pHWCrtcCXOffset
        or      edi,edi
        je      DoneIt

        ; First, copy the gamma table passed in
        mov     esi,pPalette
        lea     edi,[edi].HWCRTCCX.dwPaletteTable
        mov     eax,dwFirstEntry
        lea     esi,[esi][eax * 4]
        lea     edi,[edi][eax * 4]
        mov     ecx,dwNumEntries
@@:
        mov     eax,ds:[esi]
        add     esi,4
        mov     ds:[edi],eax
        add     edi,4
        dec     ecx
        jne     @B

        push    pHWCrtcCXOffset
        pushd   OBJ_DMA_IN_SYSRAM_FOR_PALETTE
        push    dwFirstEntry
        push    dwNumEntries
        call    NV_SetColorMap

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_SetColorMap
;
; Purpose:      This routine sets a color map into the hardware.
;               It is called by both the set palette and set
;               gamma fcuntions.
;
; Arguments:
;               ds                      CXSel
;               pHWCrtcCXOffset         HWCRTCCX ptr
;               object base to set      Should be either:
;                                       OBJ_DMA_IN_SYSRAM_FOR_PALETTE
;                                       OBJ_DMA_IN_SYSRAM_FOR_GAMMA
;               pColorMap               Ptr to a color map
;               dwFirstEntry            0-255 index of first entry to set
;               dwNumEntries            number of contiguous palette
;                                       indices to set
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_SetColorMap, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   dwBaseObjectID
PARMD   dwFirstEntry
PARMD   dwNumEntries
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWCrtcCXOffset
        or      esi,esi
        je      SkipSet

        mov     ebx,ds:[esi].HWCRTCCX.pHWBoardCX

        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ebx,eax

        ; Load the push buffer ptr
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Tell the hardware to load this color map
        CHECKDMASPACE ds,ebx,edi,1CH

        ; Set VideoLutCursorDac in a sub-channel
        mov     eax,CHNRUN(VideoLutCursorDacHdr.object,1)
        mov     edx,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     edx,OBJ_VIDEO_LUT_CURSOR_DAC
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],edx

        ; Set the correct color map object
        mov     eax,CHNRUN(VideoLutCursorDac.SetContextDmaLut,1)
        mov     edx,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        add     edx,dwBaseObjectID
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],edx

        ; Program the buffer offset for the color map.
        ; We have created a ContextDma just for the
        ; dwColorMap so the offset will be 0.
        mov     eax,CHNRUN(VideoLutCursorDac.SetLut,2)
        sub     edx,edx
        mov     ecx,NV049_SET_LUT_FORMAT_NOTIFY_WRITE_ONLY
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],edx
        mov     ds:[edi + 18H],ecx
        add     edi,1CH

        SETPUTPTR ds,ebx,edi
        sub     edi,ds:[ebx].HWBOARDCX.pPushBufferContextStartOffset
@@:
        cmp     ds:[ebx].HWBOARDCX.dwDmaGetPtr,edi
        jne     @B

SkipSet:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_IsBandwidthOK
;
; Purpose:      This function checks to see whether the mode(s)
;               specified violate bandwidth requirements on the
;               display device.
;
; Arguments:
;               dwTotalWidth0           paramters for physical head 0
;               dwVisibleWidth0
;               dwTotalHeight0
;               dwVisibleHeight0
;               dwPixelDepth0
;               dwPixelClock0
;               dwRefreshRate0
;
;               dwTotalWidth1           parameters for physical head 1
;               dwVisibleWidth1
;               dwTotalHeight1
;               dwVisibleHeight1
;               dwPixelDepth1
;               dwPixelClock1
;               dwRefreshRate1
;
;               If the dwTotaldwWidth field is 0, then that head
;               is not being used.
;
; Returns:      eax     non-0   then bandwidth is OK
;                       0       then bandwidth is violated
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_IsBandwidthOK, PASCAL, FRAME, FAR32
PARMD   pHWCrtcCXOffset
PARMD   dwTotaldwWidth0
PARMD   dwVisibledwWidth0
PARMD   dwTotalHeight0
PARMD   dwVisibleHeight0
PARMD   dwPixelDepth0
PARMD   dwPixelClock0
PARMD   dwRefreshRate0
PARMD   dwTotaldwWidth1
PARMD   dwVisibledwWidth1
PARMD   dwTotalHeight1
PARMD   dwVisibleHeight1
PARMD   dwPixelDepth1
PARMD   dwPixelClock1
PARMD   dwRefreshRate1
LOCALV  Params,NV_CFGEX_VALIDATE_BANDWIDTH_PARAMS
OPENPROC
        PUSHR   esi
        mov     esi,pHWCrtcCXOffset
        mov     esi,ds:[esi].HWCRTCCX.pHWBoardCX

        mov     Params.VidScalerHead,NV_CFGEX_VALIDATE_BANDWIDTH_SCALER_WORST_CASE

        mov     eax,dwTotaldwWidth0
        mov     Params.HeadActive0,eax
        or      eax,eax
        je      Skip0
        mov     Params.HeadActive0,-1
        mov     Params.TotaldwWidth0,eax
        mov     eax,dwVisibledwWidth0
        mov     Params.VisibledwWidth0,eax
        mov     eax,dwTotalHeight0
        mov     Params.TotalHeight0,eax
        mov     eax,dwVisibleHeight0
        mov     Params.VisibleHeight0,eax
        mov     eax,dwPixelDepth0
        mov     Params.PixelDepth0,eax
        mov     eax,dwPixelClock0
        mov     Params.PixelClock0,eax

        ; Isn't being used now
;       mov     Params.RefreshRate0,eax

Skip0:
        mov     eax,dwTotaldwWidth1
        mov     Params.HeadActive1,eax
        or      eax,eax
        je      Skip1
        mov     Params.HeadActive1,-1
        mov     eax,dwTotaldwWidth1
        mov     Params.TotaldwWidth1,eax
        mov     eax,dwVisibledwWidth1
        mov     Params.VisibledwWidth1,eax
        mov     eax,dwTotalHeight1
        mov     Params.TotalHeight1,eax
        mov     eax,dwVisibleHeight1
        mov     Params.VisibleHeight1,eax
        mov     eax,dwPixelDepth1
        mov     Params.PixelDepth1,eax
        mov     eax,dwPixelClock1
        mov     Params.PixelClock1,eax

        ; Isn't being used now
;       mov     Params.RefreshRate1,0

Skip1:
;       mov     Params.status;

        mov     ax,ss
        shl     eax,10H
        lea     ax,Params
        mov     ebx,SIZE NV_CFGEX_VALIDATE_BANDWIDTH_PARAMS
        mov     ecx,NV_CFGEX_VALIDATE_BANDWIDTH
        call    NV_ConfigExCall

        ; Return success/fail in eax
        ; Status = 0 is Success, so flip the sense
        or      eax,eax
        mov     eax,1
        je      @F
        sub     eax,eax
@@:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_MapHWDeviceTypeToGNDeviceType
;
; Purpose:      This function maps the device type constants
;               MONITOR_XXXX defined in nvcm.h to the device type
;               constants defined in modeext.h.
;
; Arguments:    ecx     RM device type -- MONITOR_XXX
;
; Returns:      ecx     Modeset device constant -- DEVTYPE_XXXX
;
; Preserve:     everything except ecx
;==============================================================================
DECPROC HW_MapHWDeviceTypeToGNDeviceType, PASCAL, FRAME, FAR32
OPENPROC
        cmp     ecx,MONITOR_TYPE_FLAT_PANEL
        jbe     @F
        mov     ecx,MONITOR_TYPE_VGA
@@:     movzx   ecx,TableMonitorTypeRMToModeset[ecx]
CLOSEPROC


;==============================================================================
;
; Function:     HW_MapGNDeviceTypeToHWDeviceType
;
; Purpose:      This function maps the device type constants
;               DEVTYPE_XXXX defined in modeext.h to the device type
;               constants MONITOR_XXXX defined in nvcm.h.
;
; Arguments:    ecx     Modeset device type -- DEVTYPE_XXX
;
; Returns:      ecx     RM device constant -- MONITOR_XXXX
;
; Preserve:     everythin except ecx
;==============================================================================
DECPROC HW_MapGNDeviceTypeToHWDeviceType, PASCAL, FRAME, FAR32
OPENPROC
        cmp     ecx,DEVTYPE_DFP
        jbe     @F
        mov     ecx,DEVTYPE_CRT
@@:     movzx   ecx,TableMonitorTypeModesetToRM[ecx]
CLOSEPROC


;==============================================================================
;
; Function:     HW_MapHWTvFormatToGNTvFormat
;
; Purpose:      This function maps the Tv Format constants
;               NTSC_X and PAL_X defined in nvcm.h to the
;               Tv Format constants defined in modeext.h
;
; Arguments:    ecx     RM Tv Format -- NTSC_X and PAL_X
;
; Returns:      ecx     Modeset TV Format -- TVTYPE_XXX
;
; Preserve:     everything except ecx
;==============================================================================
DECPROC HW_MapHWTvFormatToGNTvFormat, PASCAL, FRAME, FAR32
OPENPROC
        cmp     ecx,KNOWN_TV_FORMATS
        jbe     @F
        mov     ecx,NTSC_M
@@:     movzx   ecx,TableTvFormatRMToModeset[ecx]
CLOSEPROC


;==============================================================================
;
; Function:     HW_MapGNTvFormatToHWTvFormat
;
; Purpose:      This function maps the Tv Format constants
;               TVTYPE_XXXX defined in modeext.h to the TV Format
;               constants NTSC_X and PAL_X defined in nvcm.h.
;
; Arguments:    ecx     Modeset Tv Format -- TVTYPE_XXX
;
; Returns:      ecx     RM Tv Format -- NTSC_X and PAL_X
;
; Preserve:     everything except ecx
;==============================================================================
DECPROC HW_MapGNTvFormatToHWTvFormat, PASCAL, FRAME, FAR32
OPENPROC
        cmp     ecx,TVTYPE_PALNC
        jbe     @F
        mov     ecx,TVTYPE_NTSCM
@@:     movzx   ecx,TableTvFormatModesetToRM[ecx]
CLOSEPROC

CLOSESEG _NVTEXT32

END

