;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Copyright (C) 1998-1999, Nvidia Corporation
;
; File:         hwgdi.inc
;
; Purpose:      This file holds the interface to the harware specific
;               service routines.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; I put these equates inside things that I think are wrong or not necessary
; but they break verdict so I made the changes.
MAKE_VERDICT_WORK       TEXTEQU <1>

IF      (MAKE_VERDICT_WORK NE 0)
INTERSECT_TEXTXFORM_WITH_CLIP   TEXTEQU <1>
CHIP_SORTS_RECT_AND_TEXT_COORDS TEXTEQU <1>
ELSE

; The DIBENG intersects the rectangle specified by the lpTextXForm
; argument to ExtTextOut with the rectangle specified by lpClipRect
; to generate the real clip rect. I think that this is wrong.
; lpClipRect gives the clip rectangle period.
INTERSECT_TEXTXFORM_WITH_CLIP   TEXTEQU <0>
ENDIF

; This enables font cache.  It is off by default for now because font cache
; is actually slower on a very fast machine like Camino 700 Mhz.  On a slower
; machine like PIII-500 it is slightly faster than the traditional text.
RENDER_TEXT_WITH_FONT_CACHE     TEXTEQU <0>

; Enables support for A8R8G8B8 color cursors with alpha
USE_ALPHA_CURSOR        TEXTEQU <1>

; Enables cursor shadows.  Down and right are the number of pixels the
; shadow is displaced.
CURSOR_SHADOW           TEXTEQU <0>
CURSOR_SHADOW_DOWN      TEXTEQU <3>
CURSOR_SHADOW_RIGHT     TEXTEQU <3>
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
OPENSEG  _NVTEXT16, USE16, CODE
; hwaccess.asm
EXTERNDEF       HW_GetGraphicsTimeStamp:NEAR

; hwcntrl.asm
EXTERNDEF       HW_ControlQuery:NEAR
EXTERNDEF       HW_ControlAction:NEAR

; hwinit.asm
EXTERNDEF       HW_CreateConstants:FAR16
EXTERNDEF       HW_DestroyConstants:FAR16
EXTERNDEF       HW_AllocFlatMemory:FAR16
EXTERNDEF       HW_FreeFlatMemory:FAR16
EXTERNDEF       HW_GetLogdevToBoardPtr:FAR16

; hwlogdev.asm
EXTERNDEF       HW_CreateLogdevContext:FAR16
EXTERNDEF       HW_DestroyLogdevContext:FAR16
EXTERNDEF       HW_LogdevAttachCrtc:FAR16
EXTERNDEF       HW_LogdevDetachCrtc:FAR16
EXTERNDEF       HW_Get16BppPixelFormat:FAR16
EXTERNDEF       HW_GetTextFormat:FAR16

; hwlogdev2.asm
EXTERNDEF       HW_LogdevMessage:FAR16
EXTERNDEF       HW_DriverTwinviewStateChanging:FAR16
EXTERNDEF       HW_AllocLogicalSurface:FAR16
EXTERNDEF       HW_FreeLogicalSurface:FAR16
EXTERNDEF       HW_GetDirectXRuntimeVersion:FAR16
EXTERNDEF       HW_GetDirectXDllNameAndEntryPoint:FAR16
EXTERNDEF       HW_SetNewDirxCallbacks:FAR16
EXTERNDEF       HW_GetDirectXInstance:FAR16
EXTERNDEF       HW_GetOpenGLRegistryName:FAR16
EXTERNDEF       HW_TellResManSpanningState:FAR16

; mvision
EXTERNDEF       HW_SetMacrovisionMode:NEAR
CLOSESEG _NVTEXT16


OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE
; hwaccess.asm
EXTERNDEF       HW_BeginAccess:NEAR
EXTERNDEF       HW_BeginAccess_Far32:FAR32
EXTERNDEF       HW_EndAccess:NEAR
EXTERNDEF       HW_CreateOffscreenBitmap:NEAR
EXTERNDEF       HW_DeleteOffscreenBitmap:NEAR

; hwboard.asm
EXTERNDEF       HW_CreateBoardContext:FAR32
EXTERNDEF       HW_DestroyBoardContext:FAR32
EXTERNDEF       HW_SetDesktopState:FAR32
EXTERNDEF       NV_SetInterlacedStereoMode:FAR32
DESKTOP_STATE_NORMAL    TEXTEQU <0>
DESKTOP_STATE_MULTIMON  TEXTEQU <1>
DESKTOP_STATE_CLONE     TEXTEQU <2>

; hwboard2.asm
EXTERNDEF       HW_BoardMessage:FAR32
EXTERNDEF       HW_GetLargestOffscreenBlock:FAR32
EXTERNDEF       HW_GetLargestOffscreenSpace:FAR32
EXTERNDEF       HW_AllocOffscreenBlock:FAR32
EXTERNDEF       HW_FreeOffscreenBlock:FAR32
EXTERNDEF       HW_OffscreenHeapPurge:FAR32
EXTERNDEF       HW_DxHeapPurge:FAR32
EXTERNDEF       HW_HeapPurgeAllExceptPrimarySurface:FAR32
EXTERNDEF       HW_AllocPrimarySurface:FAR32
EXTERNDEF       HW_FreePrimarySurface:FAR32
EXTERNDEF       HW_PrimarySurfaceHeapPurge:FAR32
EXTERNDEF       HW_GetPciDeviceID:FAR32
EXTERNDEF       HW_GetFrameBufferSelector:FAR32
EXTERNDEF       HW_GetModeDimensions:FAR32
EXTERNDEF       HW_GetNumCRTCs:FAR32
EXTERNDEF       HW_GetAllAttachedDevices:FAR32
EXTERNDEF       HW_IsDeviceAttached:FAR32
EXTERNDEF       HW_CanDevicesBeMapped:FAR32
EXTERNDEF       HW_MapDevices:FAR32
EXTERNDEF       HW_BeginDisplayChange:FAR32
EXTERNDEF       HW_EndDisplayChange:FAR32
EXTERNDEF       Enable_VGA:FAR32

; hwclrblt.asm
EXTERNDEF       HW_BitBltSrcColorDstVram:NEAR
EXTERNDEF       HW_BeginSrcColorDstVram:NEAR
EXTERNDEF       HW_BlockSrcColorDstVram:NEAR
EXTERNDEF       HW_EndSrcColorDstVram:NEAR

; hwcrtc.asm
EXTERNDEF       HW_CreateCrtcContext:FAR32
EXTERNDEF       HW_DestroyCrtcContext:FAR32
EXTERNDEF       HW_GetDacLimits:FAR32
EXTERNDEF       HW_BitsPerColorGun:FAR32
EXTERNDEF       HW_SizeCursorCache:FAR32
EXTERNDEF       HW_IsCRTConnected:FAR32
EXTERNDEF       HW_IsDFPConnected:FAR32
EXTERNDEF       HW_IsTVConnected:FAR32
EXTERNDEF       HW_GetMaxDfpScaledXRes:FAR32
EXTERNDEF       HW_IsMacroVisionEncoderAttached:FAR32
EXTERNDEF       HW_ReadEdid:FAR32
EXTERNDEF       HW_ReadEdidForDevice:FAR32
EXTERNDEF       HW_GetPhysicalCrtcIndex:FAR32
EXTERNDEF       HW_GetBootDevice:FAR32
EXTERNDEF       HW_GetResManTvFormat:FAR32
EXTERNDEF       HW_SetResManTvFormat:FAR32
EXTERNDEF       HW_GetMaxTVOutResolution:FAR32

; hwcrtc2.asm
EXTERNDEF       HW_CrtcMessage:FAR32
EXTERNDEF       HW_SetCrtcMode:FAR32
EXTERNDEF       HW_SetStopImage:FAR32
EXTERNDEF       HW_SetPanOffset:FAR32
EXTERNDEF       HW_SetGamma:FAR32
EXTERNDEF       HW_SetPalette:FAR32
EXTERNDEF       HW_MapHWDeviceTypeToGNDeviceType:FAR32
EXTERNDEF       HW_MapGNDeviceTypeToHWDeviceType:FAR32
EXTERNDEF       HW_MapHWTvFormatToGNTvFormat:FAR32
EXTERNDEF       HW_MapGNTvFormatToHWTvFormat:FAR32
EXTERNDEF       HW_IsBandwidthOK:FAR32

; hwcursor.asm
EXTERNDEF       HW_CursorEnable_FAR32:FAR32
EXTERNDEF       HW_CursorEnable:NEAR
EXTERNDEF       HW_CursorSet:NEAR
EXTERNDEF       HW_CursorSelect:NEAR
EXTERNDEF       HW_CursorMove:NEAR

; hwldpat.asm
EXTERNDEF       HW_LoadSolidPattern:NEAR
EXTERNDEF       HW_LoadMonoPattern:NEAR
EXTERNDEF       HW_LoadColorPattern:NEAR

; hwline.asm
EXTERNDEF       HW_Polyline:NEAR
EXTERNDEF       HW_PolyStyledLine:NEAR
EXTERNDEF       SETUPPATTERNHWFORSTYLELINE:NEAR

; hwmonblt.asm
EXTERNDEF       HW_BeginSrcMonoDstVram:NEAR
EXTERNDEF       HW_BlockSrcMonoDstVram:NEAR
EXTERNDEF       HW_EndSrcMonoDstVram:NEAR

; hwpatblt.asm
EXTERNDEF       HW_BitBltSolidPatDstRop:NEAR
EXTERNDEF       HW_BitBltColorPatDstRop:NEAR
EXTERNDEF       HW_BitBltMonoPatDstRop:NEAR

; hwrect.asm
EXTERNDEF       HW_DrawRectAndBorder:NEAR
EXTERNDEF       HW_BeginRect:NEAR
EXTERNDEF       HW_BlockRect:NEAR
EXTERNDEF       HW_BorderRect:NEAR
EXTERNDEF       HW_EndRect:NEAR
EXTERNDEF       HW_SolidColorRectangle:NEAR

; hwstrblt.asm
EXTERNDEF       HW_BeginStretchSrcColorDstVram:NEAR
EXTERNDEF       HW_BlockStretchSrcColorDstVram:NEAR
EXTERNDEF       HW_EndStretchSrcColorDstVram:NEAR

; hwsysblt.asm
EXTERNDEF       HW_BeginSrcVramDstSystem:NEAR
EXTERNDEF       HW_BlockSrcVramDstSystem:NEAR
EXTERNDEF       HW_WaitSrcVramDstSystem:NEAR
EXTERNDEF       HW_EndSrcVramDstSystem:NEAR

; hwtext.asm
EXTERNDEF       HW_DrawOpaqueRects:NEAR
EXTERNDEF       HW_TextBegin:NEAR
EXTERNDEF       HW_TextBlt:NEAR
EXTERNDEF       HW_TextEnd:NEAR

; hwvrmblt.asm
EXTERNDEF       HW_BitBltSrcVramDstVram:NEAR
CLOSESEG _NVTEXT32
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; BITBLT

; This will be the frame for BitBlt that is shared between the generic
; code and SOME of the hardware specific bitblt routines. You may add
; locals as you need them.
BITBLT_FRAME_AND_LOCALS MACRO
PARMD   lpDstDev
PARMW   wDstX
PARMW   wDstY
PARMD   lpSrcDev
PARMW   wSrcX
PARMW   wSrcY
PARMW   wExtX
PARMW   wExtY
PARMD   dwRop3
PARMD   lpBrush
PARMD   lpDrawMode

; Generic code needs these
LOCALD  dwSrcBitsOffset
LOCALD  dwScansPerSeg

; Add hardware specific ones here.
ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; EXTTEXTOUT

; This will be the frame for ExtTextOut that is shared between the generic
; code and the hardware specific code. You may add locals as you need them.
EXTTEXTOUT_FRAME_AND_LOCALS     MACRO
PARMD   lpDstDev
PARMW   wDstX
PARMW   wDstY
PARMD   lpClipRect
PARMD   lpString
PARMW   wCount
PARMD   lpFontInfo
PARMD   lpDrawMode
PARMD   lpTextXForm
PARMD   lpKerning
PARMD   lpOpaqueRects
PARMW   wOptions

; Generic code needs these
LOCALD  dwDstBitsOffset
LOCALD  dwDstBitsDeltaScan
LOCALD  dwFgColor
LOCALD  dwBgColor
LOCALD  dwStringOffset
LOCALD  dwGlyphWidth
LOCALD  dwKerningOffset
LOCALD  dwDstYDstX
LOCALD  dwClipRight
LOCALD  dwGlyphCount
LOCALD  pHWLogDevCXOffset_ForExtTextOut
LOCALD  pHWBoardCXOffset_ForExtTextOut
LOCALV  wOpaqueClipRect,RECT
LOCALV  wTextClipRect,RECT
ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This macro allows you to call through function ptrs in other segments.
; I was using this alot in both the HWSPEC and GENERIC code so I put a
; macro in here to make it simple.
CALLFAR16IND    MACRO   lpfnFunc,sreg,reg
        mov     reg,SEG lpfnFunc
        mov     sreg,reg
        call    FFP16 PTR sreg:[lpfnFunc]
ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

