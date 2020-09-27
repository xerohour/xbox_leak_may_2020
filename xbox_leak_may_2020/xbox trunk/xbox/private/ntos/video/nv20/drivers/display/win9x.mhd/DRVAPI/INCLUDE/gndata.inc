;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Copyright (C) 1998-1999, Nvidia Corporation
;
; File:         gndata.inc
;
; Purpose:      This file holds some data/structures/externs that
;               the generic code needs.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; DibBlt, SetDIBitsToDevice, StretchDIBits 8 bpp h/w acceleration define.
; Set to 1 will turn on.
DIB_FAST_EXIT           TEXTEQU <1>
DIB_8BPP_ACCEL          TEXTEQU <1>
DIB_PROCESS_SINGLESCAN  TEXTEQU <1>
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
OPENSEG  _DATA, USE16, DATA
EXTERNDEF       szUserModuleName:BYTE
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

; gnaccess.asm
EXTERNDEF       GENERIC_BeginAccess:FAR16
EXTERNDEF       GENERIC_EndAccess:FAR16

; gnblt.asm
NOROP   TEXTEQU <0>
OND     TEXTEQU <1>
PAD     TEXTEQU <2>
SAD     TEXTEQU <3>
PSD     TEXTEQU <4>
EXTERNDEF       GENERIC_BitBlt:FAR16
EXTERNDEF       Punt_BitBlt:NEAR
EXTERNDEF       GN_BitBltPatDstRop:NEAR
EXTERNDEF       GN_BitBltPatUnknownSrcUnknownDstVram:NEAR
EXTERNDEF       GN_BitBltSrcUnknownDstVram:NEAR
EXTERNDEF       GN_BitBltClipSrc:NEAR
EXTERNDEF       GN_BitBltSrcVramDstVram:NEAR
EXTERNDEF       GN_BitBltSrcMonoDstVram_1:NEAR
EXTERNDEF       GN_BitBltSrcColorDstVram_1:NEAR
EXTERNDEF       GN_BitBltSystemMemoryDst:NEAR
EXTERNDEF       GN_PrepareBrush:NEAR
EXTERNDEF       Rop3Table:BYTE

; gnbmpbts.asm
EXTERNDEF       GENERIC_BitmapBits:FAR16
EXTERNDEF       Punt_BitmapBits:NEAR
EXTERNDEF       GN_BitmapBitsSuccess:NEAR
EXTERNDEF       BitmapBits_Set:NEAR
EXTERNDEF       BitmapBits_SetWithFiller:NEAR
EXTERNDEF       BitmapBits_Get:NEAR
EXTERNDEF       BitmapBits_Copy:NEAR
EXTERNDEF       BitmapBits_CopyVramToVram:NEAR
EXTERNDEF       BitmapBits_CopySystemToVram:NEAR
EXTERNDEF       BitmapBits_CopyVramToSystem:NEAR
EXTERNDEF       BitmapBits_CopySystemToSystem:NEAR
EXTERNDEF       BitmapBits_CopyByScan:NEAR
EXTERNDEF       BitmapBits_CopyAll:NEAR

; gnclrinf.asm
EXTERNDEF       GENERIC_ColorInfo:FAR16

; gnclrblt.asm
EXTERNDEF       bTableBppToIndex:BYTE
EXTERNDEF       dwTableBppToXlatFunc:DWORD
; This needs to reflect how many entries per color depth are in the
; above table. Right now, we have routines for 1,4,8,15,16,24,32
; which is 7 different color depths. So MAX_BPP_INDICES should be 7.
MAX_BPP_INDICES TEXTEQU <7>
EXTERNDEF       GN_XferWholeColorScans:NEAR
EXTERNDEF       GN_XferPartialColorScans:NEAR
EXTERNDEF       GN_ConvertColorTableFrom32To16:NEAR

; gncp8.asm
EXTERNDEF       GN_Xfer1To8:NEAR
EXTERNDEF       GN_Xfer4To8WordXlat:NEAR
EXTERNDEF       GN_Xfer8To8NoXlat:NEAR
EXTERNDEF       GN_Xfer8To8ByteXlat:NEAR
EXTERNDEF       GN_Xfer8To8WordXlat:NEAR
EXTERNDEF       GN_Xfer15To8:NEAR
EXTERNDEF       GN_Xfer16To8:NEAR
EXTERNDEF       GN_Xfer24To8:NEAR
EXTERNDEF       GN_Xfer32To8:NEAR
EXTERNDEF       FindLMSPaletteIndex:NEAR

; gncp15.asm
EXTERNDEF       GN_Xfer4To15DwordXlat:NEAR
EXTERNDEF       GN_Xfer4To15WordXlat:NEAR
EXTERNDEF       GN_Xfer8To15WordXlat:NEAR
EXTERNDEF       GN_Xfer8To15DwordXlat:NEAR
EXTERNDEF       GN_Xfer15To15:NEAR
EXTERNDEF       GN_Xfer16To15:NEAR
EXTERNDEF       GN_Xfer24To15:NEAR
EXTERNDEF       GN_Xfer32To15:NEAR

; gncp16.asm
EXTERNDEF       GN_Xfer4To16DwordXlat:NEAR
EXTERNDEF       GN_Xfer4To16WordXlat:NEAR
EXTERNDEF       GN_Xfer8To16WordXlat:NEAR
EXTERNDEF       GN_Xfer8To16DwordXlat:NEAR
EXTERNDEF       GN_Xfer15To16:NEAR
EXTERNDEF       GN_Xfer16To16:NEAR
EXTERNDEF       GN_Xfer24To16:NEAR
EXTERNDEF       GN_Xfer32To16:NEAR

; gncp32.asm
EXTERNDEF       GN_Xfer4To32:NEAR
EXTERNDEF       GN_Xfer8To32:NEAR
EXTERNDEF       GN_Xfer15To32:NEAR
EXTERNDEF       GN_Xfer16To32:NEAR
EXTERNDEF       GN_Xfer24To32:NEAR
EXTERNDEF       GN_Xfer32To32:NEAR

; gncursor.asm
EXTERNDEF       GN_CreateCursorCache:FAR32
EXTERNDEF       GN_DestroyCursorCache:FAR32
EXTERNDEF       GN_ClearCursorCache:FAR32
EXTERNDEF       GN_ClearAllCursorCaches:FAR32
EXTERNDEF       GN_Convert8BppCursorTo32BppCursor:NEAR
EXTERNDEF       GENERIC_SetCursor:FAR16
EXTERNDEF       GN_CursorAction:NEAR
EXTERNDEF       GENERIC_MoveCursor:FAR16
EXTERNDEF       GENERIC_CheckCursor:FAR16
EXTERNDEF       GN_TurnCursorOffOrOn:FAR32
EXTERNDEF       GN_ResetSelectedCursor:FAR32

; gndibblt.asm
EXTERNDEF       GENERIC_DibBlt:FAR16
EXTERNDEF       Punt_DibBlt:NEAR
EXTERNDEF       GN_DibBltSuccess:NEAR
EXTERNDEF       GN_DibBltSetMono:NEAR
EXTERNDEF       GN_DibBltSetColor:NEAR
EXTERNDEF       GN_DibBltGet:NEAR
EXTERNDEF       GN_DibBltClip:NEAR

; gndibdev.asm
EXTERNDEF       GENERIC_SetDIBitsToDevice:FAR16
EXTERNDEF       Punt_DibToDevice:NEAR
EXTERNDEF       GN_DibDevExit:NEAR
EXTERNDEF       GN_SetMonoDibToDevice:NEAR
EXTERNDEF       GN_SetColorDibToDevice:NEAR
EXTERNDEF       GN_SetDIBitsToDeviceClip:NEAR
EXTERNDEF       GN_SetDIBitsSingleScan:NEAR

; gndibs.asm
EXTERNDEF       GN_DibSingleScan_Bpp8:NEAR
EXTERNDEF       GN_DibSingleScan_Bpp16:NEAR
EXTERNDEF       GN_DibSingleScan_Bpp32:NEAR

; gndibxit.asm
EXTERNDEF       GN_InitDibFastExit:FAR16
EXTERNDEF       bUseSetDIBitsFastExit:BYTE
EXTERNDEF       bUseStretchDIBitsFastExit:BYTE
EXTERNDEF       lpfnGdiCode03_2119:DWORD
EXTERNDEF       lpfnGdiCode03_20B7:DWORD
EXTERNDEF       GdiCode03_2119:FAR16
EXTERNDEF       GdiCode03_217C:FAR16

; gngamma.asm
EXTERNDEF       GENERIC_GammaRamp:FAR16

; gninit.asm
EXTERNDEF       wFlatDataSel:WORD
EXTERNDEF       CSlp16LogDevice:DWORD
EXTERNDEF       CSpGNLogicalDevice:DWORD

; gnline
;EXTERNDEF      GENERIC_Polyline:FAR16
EXTERNDEF       GN_PoylineNonSolid:NEAR
EXTERNDEF       Punt_Polyline:NEAR
EXTERNDEF       GN_PolylineSuccess:NEAR

; gnmem.asm
EXTERNDEF       GN_HeapCreate:FAR32
EXTERNDEF       GN_HeapInit:FAR32
EXTERNDEF       GN_HeapDestroy:FAR32
EXTERNDEF       GN_HeapAlloc:NEAR
EXTERNDEF       GN_HeapFree:NEAR

; gnmonblt.asm
EXTERNDEF       GN_BitBltSrcMonoDstVram:NEAR
EXTERNDEF       GN_XferWholeAlignedMonoScans:NEAR
EXTERNDEF       GN_XferWholeUnalignedMonoScans:NEAR
EXTERNDEF       GN_XferPartialAlignedMonoScans:NEAR
EXTERNDEF       GN_XferPartialUnalignedMonoScans:NEAR

; gnoffbmp.asm
EXTERNDEF       GN_ClearOffscreenBitmapCache:FAR32
EXTERNDEF       GN_OffscreenBitmapCreate:NEAR
EXTERNDEF       GN_OffscreenBitmapDelete:NEAR
EXTERNDEF       GN_OffscreenBitmapValidCheck:NEAR
EXTERNDEF       GN_FlushOffscreenBitmap:NEAR

; gnoutput.asm
EXTERNDEF       GENERIC_Output:FAR16
EXTERNDEF       Punt_Output:NEAR
;EXTERNDEF      GENERIC_Polygon:FAR16
;EXTERNDEF      GENERIC_Polyline:FAR16
;EXTERNDEF      GENERIC_PolyScanOrScanLine:FAR16

; gnpalet.asm
EXTERNDEF       GENERIC_SetPalette:FAR16
EXTERNDEF       GENERIC_SetPaletteTranslate:FAR16
EXTERNDEF       GENERIC_UpdateColors:FAR16
EXTERNDEF       GN_UpdateColorsExit:NEAR
EXTERNDEF       GN_UpdateColors:NEAR
EXTERNDEF       Punt_UpdateColors:NEAR

; gnpixel.asm
EXTERNDEF       GENERIC_Pixel:FAR16
EXTERNDEF       Punt_Pixel:NEAR
EXTERNDEF       GN_PixelDone:NEAR
EXTERNDEF       GN_SetPixel:NEAR
EXTERNDEF       GN_GetPixel:NEAR

; gnrect.asm
;EXTERNDEF      GENERIC_Rectangle:FAR16
EXTERNDEF       Punt_Rectangle:NEAR

; gnrobj.asm
EXTERNDEF       GENERIC_RealizeObject:FAR16
EXTERNDEF       GN_RealizeObjectExt:FAR16
EXTERNDEF       Punt_RealizeObjectExt:NEAR
EXTERNDEF       GN_RealizeObjectExtSuccess:NEAR
EXTERNDEF       GN_RealizeObjectExtDone:NEAR
EXTERNDEF       GN_RealizeObjectExtBrush:NEAR
EXTERNDEF       GN_RealizeObjectExtBitmap:NEAR
EXTERNDEF       GN_RealizeObjectExtDeleteBitmap:NEAR
EXTERNDEF       GN_RealizeObjectExtFont:NEAR
EXTERNDEF       GN_InitFunctionPtr:NEAR
EXTERNDEF       GN_LoadAndInitFonts:NEAR

; gnscan.asm
;EXTERNDEF      GENERIC_BeginScan:FAR16
;EXTERNDEF      GENERIC_EndScan:FAR16
;EXTERNDEF      GENERIC_PolyScanOrScanLine:FAR16
EXTERNDEF       Punt_BeginScan:NEAR
EXTERNDEF       Punt_Scan:NEAR
EXTERNDEF       Punt_EndScan:NEAR

; gnsdib.asm
EXTERNDEF       GENERIC_StretchDIBits:FAR16
EXTERNDEF       Punt_StrDib:NEAR
EXTERNDEF       GN_StrDibExit:NEAR
EXTERNDEF       GN_StretchMonoDib:NEAR
EXTERNDEF       GN_StretchColorDib:NEAR
EXTERNDEF       GN_StretchDIBitsClip:NEAR
EXTERNDEF       GN_StretchDIBitsSingleScan:NEAR

; gnssb.asm
EXTERNDEF       GN_ClearSaveScreenBitmapCache:FAR32
EXTERNDEF       SaveScreenBitmap:FAR16
EXTERNDEF       GENERIC_SaveScreenBitmap:FAR16
EXTERNDEF       GN_SaveScreenBitmapFail:NEAR
EXTERNDEF       GN_SaveScreenBitmapSuccess:NEAR
EXTERNDEF       GN_SaveScreenBitmapExit:NEAR
EXTERNDEF       SSB_SaveArea:NEAR
EXTERNDEF       SSB_RestoreOrDiscardArea:NEAR
EXTERNDEF       GN_SaveScreenBitmap:NEAR

; gnstrblt.asm
EXTERNDEF       GENERIC_StretchBlt:FAR16
EXTERNDEF       Punt_StrBlt:NEAR

; gnstub.asm
EXTERNDEF       GENERIC_GetCharWidth:FAR16
EXTERNDEF       GENERIC_DeviceBitmap:FAR16
EXTERNDEF       GENERIC_EnumDFonts:FAR16
EXTERNDEF       GENERIC_EnumObj:FAR16
EXTERNDEF       GENERIC_FastBorder:FAR16
EXTERNDEF       GENERIC_GetPalette:FAR16
EXTERNDEF       GENERIC_GetPaletteTranslate:FAR16
EXTERNDEF       GENERIC_Inquire:FAR16
EXTERNDEF       GENERIC_ScanLR:FAR16
EXTERNDEF       GENERIC_SetAttribute:FAR16
EXTERNDEF       GENERIC_StrBlt:FAR16

; gnsysblt.asm
EXTERNDEF       GN_BitBltSrcVramDstSystem:NEAR
EXTERNDEF       OurRopSetup:NEAR
EXTERNDEF       DibengRopSetup:NEAR

; gntext.asm
EXTERNDEF       GENERIC_ExtTextOut:FAR16
EXTERNDEF       Punt_ExtTextOut:NEAR
EXTERNDEF       GN_ExtTextOutRet:NEAR
EXTERNDEF       GN_ExtTextOutSuccess:NEAR
EXTERNDEF       GN_ExtTextOutExit:NEAR
EXTERNDEF       GlyphBltByteIndexWordTableIsKerning:NEAR
EXTERNDEF       GlyphBltWordIndexWordTableIsKerning:NEAR
EXTERNDEF       GlyphBltByteIndexWordTableNoKerning:NEAR
EXTERNDEF       GlyphBltWordIndexWordTableNoKerning:NEAR
EXTERNDEF       GlyphBltByteIndexDWordTableIsKerning:NEAR
EXTERNDEF       GlyphBltWordIndexDWordTableIsKerning:NEAR
EXTERNDEF       GlyphBltByteIndexDWordTableNoKerning:NEAR
EXTERNDEF       GlyphBltWordIndexDWordTableNoKerning:NEAR

; gnvdd.asm
EXTERNDEF       dwThisLogicalDeviceHandle:DWORD
CLOSESEG _NVTEXT32
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
OPENSEG  _NVTEXT16, USE16, CODE
; gnboard.asm
EXTERNDEF       GN_FindBoardContext:FAR16
EXTERNDEF       GN_CreateBoardContext:FAR16
EXTERNDEF       GN_DestroyBoardContext:FAR16
EXTERNDEF       GN_AttachLogicalDeviceToBoard:FAR16
EXTERNDEF       GN_ReattachLogicalDeviceToBoard:FAR16
EXTERNDEF       GN_DetachLogicalDeviceFromBoard:FAR16
EXTERNDEF       GN_GetPciDeviceID:FAR16
EXTERNDEF       GN_GetFrameBufferSelector:FAR16
EXTERNDEF       GN_GetNumCRTCs:FAR16
EXTERNDEF       GN_GetBoardRegistryPath:FAR16

; gnboard2.asm
EXTERNDEF       GN_BoardMessage:FAR16
EXTERNDEF       GN_FindFirstCloneDevice:FAR16
EXTERNDEF       GN_FindUnusedDevice:FAR16
EXTERNDEF       GN_SendDevicesRemapMsgToAllLogDevs:FAR16
EXTERNDEF       GN_FindLogdevWaitingToClone:FAR16
EXTERNDEF       GN_IsDevDataUniqueOnBoard:FAR16
EXTERNDEF       GN_GetAllDeviceData:FAR16
EXTERNDEF       GN_GetDevDataMask:FAR16

; gncntrl.asm
EXTERNDEF       GENERIC_Control:FAR16
EXTERNDEF       GN_ControlQuery:NEAR
EXTERNDEF       GN_ControlDDraw:NEAR
EXTERNDEF       GN_ControlDDrawNewCallbackFns:NEAR
EXTERNDEF       GN_ControlDDrawGet32BitDriverName:NEAR
EXTERNDEF       GN_ControlDDrawCreateDriverObject:NEAR
EXTERNDEF       GN_ControlDDrawDestroyDriverObject:NEAR
EXTERNDEF       GN_ControlDDrawDDVersionInfo:NEAR
EXTERNDEF       GN_MouseTrails:NEAR
EXTERNDEF       GN_ControlReturnPDevice:NEAR
EXTERNDEF       GN_ControlAllocSelectorAndMap:NEAR
EXTERNDEF       GN_ControlUnmapAndFreeSelector:NEAR
EXTERNDEF       GN_ControlSetCursor:NEAR
EXTERNDEF       GN_DvdMacrovisionCommand:NEAR
EXTERNDEF       GN_OpenGLGetInfo:NEAR
EXTERNDEF       GN_OpenGLCommand:NEAR
EXTERNDEF       GN_VideoParameters:NEAR
EXTERNDEF       GN_VpCommandGet:NEAR
EXTERNDEF       GN_VpCommandSet:NEAR
EXTERNDEF       GN_GetLocalRegistryPath:NEAR
EXTERNDEF       GN_GetRegistryModeList:NEAR

; gncrtc.asm
EXTERNDEF       GN_CreateCrtcContext:FAR16
EXTERNDEF       GN_DestroyCrtcContext:FAR16
EXTERNDEF       GN_SetLogicalCrtcInstance:FAR16
EXTERNDEF       GN_CrtcSetMonitorDevNode:FAR16
EXTERNDEF       GN_GetLargestEdidMode:FAR16
EXTERNDEF       GN_GetRefreshRateForDesktop:FAR16
EXTERNDEF       GN_DoOddballStuffAtBoot:FAR16
EXTERNDEF       GN_CheckAndMaybeReadEdid:FAR16
EXTERNDEF       GN_Far16GetModeSize:FAR16

; gncrtc2.asm
EXTERNDEF       GN_CrtcMessage:FAR16
EXTERNDEF       GN_RestoreCrtcState:FAR16
EXTERNDEF       GN_CrtcRemapArray:FAR16
EXTERNDEF       GN_RestoreAllCrtcStates:FAR16
EXTERNDEF       GN_CrtcDeviceHasBeenRemapped:FAR16
EXTERNDEF       GN_SetBytesPerPixelAndBytesPerLine:FAR16
EXTERNDEF       GN_SetPanRectangle:FAR16
EXTERNDEF       GN_GetDevData:FAR16
EXTERNDEF       GN_SetDevData:FAR16
EXTERNDEF       GN_AssignCrtcModeOnlyBackOffSpatial:FAR16
EXTERNDEF       GN_CrtcWriteModeToReg:FAR16
EXTERNDEF       GN_GetCrtcMode:FAR16
EXTERNDEF       GN_RestrictOtherModeSpatial:FAR16
EXTERNDEF       GN_RestrictCrtcModeSpatial:FAR16
EXTERNDEF       GN_SetCrtcMode:FAR16
EXTERNDEF       GN_GetDisplayDevice:FAR16
EXTERNDEF       GN_SetCrtcGamma:FAR16
EXTERNDEF       GN_GetCrtcGamma:FAR16
EXTERNDEF       GN_SetCrtcPalette:FAR16
EXTERNDEF       GN_ResetColorMap:FAR16

; gndisabl.asm
EXTERNDEF       GENERIC_Disable:FAR16
EXTERNDEF       GN_DisableDevice:NEAR
EXTERNDEF       GN_BlankScreen:NEAR

; gnenable.asm
EXTERNDEF       GENERIC_ReEnable:FAR16
EXTERNDEF       GENERIC_Enable:FAR16
EXTERNDEF       GN_EnableGdiInfo:NEAR
EXTERNDEF       GN_EnableDevice:NEAR
EXTERNDEF       GN_GetLogicalAndPhysicalModes:FAR16
EXTERNDEF       GN_SetLogicalAndPhysicalModes:FAR16
EXTERNDEF       GN_NoPrimarySurfaceRecovery:NEAR
EXTERNDEF       GN_DoFullStateChange:FAR16

; gngetid.asm
EXTERNDEF       GENERIC_GetDriverResourceID:FAR16

; gninit.asm
EXTERNDEF       wFlatDataSel_In_NVTEXT16:WORD
EXTERNDEF       CSlp16LogDevice_In_NVTEXT16:DWORD
EXTERNDEF       CSpGNLogicalDevice_In_NVTEXT16:DWORD
EXTERNDEF       lpfnFindModeEntry:DWORD
EXTERNDEF       lpfnFindVirtualModeEntry:DWORD
EXTERNDEF       lpfnGetModeList:DWORD
EXTERNDEF       lpfnGetDesktopModeList:DWORD
EXTERNDEF       lpfnSetVirtualDesktop:DWORD
EXTERNDEF       lpfnGetVirtualDesktop:DWORD
EXTERNDEF       lpfnSetDesktopState:DWORD
EXTERNDEF       lpfnGetDesktopState:DWORD
EXTERNDEF       lpfnSetAutoPanMode:DWORD
EXTERNDEF       lpfnGetAutoPanMode:DWORD
EXTERNDEF       lpfnSetTimingOverride:DWORD
EXTERNDEF       lpfnGetTimingOverride:DWORD
EXTERNDEF       lpfnGetBootDevice:DWORD
EXTERNDEF       lpfnGetLastDisplayFormat:DWORD
EXTERNDEF       lpfnGetDisplayDevice:DWORD
EXTERNDEF       lpfnSetDisplayDevice:DWORD
EXTERNDEF       lpfnGetRefreshRateForDesktop:DWORD
EXTERNDEF       lpfnGetForcedDesktopMode:DWORD
EXTERNDEF       lpfnGetLargestEdidMode:DWORD
EXTERNDEF       lpfnGetDeviceDesktopMode:DWORD
EXTERNDEF       lpfnSetDeviceDesktopMode:DWORD
EXTERNDEF       lpfnGetForcedDisplayMode:DWORD
EXTERNDEF       lpfnGetDeviceDisplayMode:DWORD
EXTERNDEF       lpfnSetDeviceDisplayMode:DWORD
EXTERNDEF       lpfnGetEdidRegistryKey:DWORD
EXTERNDEF       lpfnDeleteLocalRegEntry:DWORD
EXTERNDEF       lpfnResetRefreshRates:DWORD
EXTERNDEF       lpfnGetSafeTiming:DWORD
EXTERNDEF       lpfnSetResolutionAndColorDepth:DWORD
EXTERNDEF       lpfnGetControlVisionFlag:DWORD
EXTERNDEF       lpfnGetBootDeviceSelection:DWORD
EXTERNDEF       lpfnIsDriverAttachedToDesktop:DWORD
EXTERNDEF       lpfnGetAllowAllModesFlag:DWORD
EXTERNDEF       lpfnResetDisabledDeviceResolutions:DWORD
EXTERNDEF       lpfnGetAllowMightyModesFlag:DWORD
EXTERNDEF       lpfnGetCheckSXGAPanelFlag:DWORD
EXTERNDEF       lpfnGetClonePanningFlag:DWORD
EXTERNDEF       lpfnSetClonePanningFlag:DWORD
EXTERNDEF       lpfnFreeMasterLists:DWORD
EXTERNDEF       lpfnGetCloneDesktopSelection:DWORD
EXTERNDEF       lpfnGetRestrictLargestMode:DWORD
EXTERNDEF       lpfnGet1400x1050Override:DWORD
EXTERNDEF       lpfnGetMaxDfpMode:DWORD
EXTERNDEF       lpfnGetModesetStopImage:DWORD


EXTERNDEF       DllEntryPoint:FAR16
EXTERNDEF       GN_DllEntryPoint:FAR16
EXTERNDEF       GENERIC_WEP:FAR16
EXTERNDEF       WEP:FAR16
EXTERNDEF       GN_CreateConstants:FAR16
EXTERNDEF       GN_DestroyConstants:FAR16

; gnint2f.asm
EXTERNDEF       lpUserFullscreenDOSBoxFlag:DWORD

EXTERNDEF       GN_HookInt2F:FAR16
EXTERNDEF       GN_UnhookInt2F:FAR16
EXTERNDEF       Int2FHandler:FAR16
EXTERNDEF       Int2FSwitchingToWindows:FAR16
EXTERNDEF       Int2FSwitchingToDos:FAR16
EXTERNDEF       GENERIC_ResetHiresMode:FAR16
EXTERNDEF       GN_Int2FSwitchingToWindows:FAR16

; gnlogdev.asm
EXTERNDEF       GN_CreateLogdevContext:FAR16
EXTERNDEF       GN_DestroyLogdevContext:FAR16
EXTERNDEF       GN_SetGdiInfoPtr:FAR16
EXTERNDEF       GN_LogdevAttachCrtc:FAR16
EXTERNDEF       GN_LogdevDetachCrtc:FAR16
EXTERNDEF       GN_LogdevAddAClone:FAR16

; gnlogdev2.asm
EXTERNDEF       GN_LogdevMessage:FAR16
EXTERNDEF       GN_OffscreenHeapDisappearing:FAR16
EXTERNDEF       GN_ResolveDeviceConnectionsOnLoad:FAR16
EXTERNDEF       GN_ResolveDeviceConnectionsOnUnload:FAR16

; gnoffmgr.asm
EXTERNDEF       GN_ControlOffscreenAllocations:FAR16
EXTERNDEF       GN_OffscreenAllocationsLock:FAR16
EXTERNDEF       GN_OffscreenAllocationsUnlock:FAR16

; gnpanel.asm
EXTERNDEF       ControlPanelInterface:NEAR
EXTERNDEF       NV_SetGet_State:NEAR
EXTERNDEF       NV_SetGet_Device_Type:NEAR
EXTERNDEF       NV_SetDisplayDeviceType:NEAR
EXTERNDEF       NV_GetDisplayDeviceType:NEAR
EXTERNDEF       NV_SetGet_Autopan:NEAR
EXTERNDEF       NV_SetGet_VirtualMode:NEAR
EXTERNDEF       NV_SetGet_PhysicalMode:NEAR
EXTERNDEF       NV_SetGet_PhysicalModeImmediate:NEAR
EXTERNDEF       NV_ValidateVirtualMode:NEAR
EXTERNDEF       NV_ValidatePhysicalMode:NEAR
EXTERNDEF       NV_SetGet_Freeze_Origin:NEAR
EXTERNDEF       NV_SetGet_PanOffset:NEAR
EXTERNDEF       NV_SetGet_Primary_Crtc:NEAR
EXTERNDEF       GN_MapHeadToGNLogdev:FAR16

; gnpower.asm
EXTERNDEF       GN_PowerOn:FAR16
EXTERNDEF       GN_PowerOff:FAR16
EXTERNDEF       GN_VddSetPowerManagementCallbacks:FAR16

; gnrepnt
EXTERNDEF       GN_InitUserRepaint:FAR16
EXTERNDEF       GENERIC_UserRepaintDisable:FAR16
EXTERNDEF       GN_UserRepaint:FAR16
EXTERNDEF       GN_IssueUserRepaint:NEAR
EXTERNDEF       GN_CheckUserRepaint:FAR16

; gnthk16.asm
EXTERNDEF       wLog:WORD
EXTERNDEF       Patch_BitBlt:FAR16
EXTERNDEF       SetCursor:FAR16
EXTERNDEF       MoveCursor:FAR16
EXTERNDEF       SetPalette:FAR16
EXTERNDEF       Patch_BitmapBits:FAR16
EXTERNDEF       Control:FAR16
EXTERNDEF       Patch_ExtTextOut:FAR16
EXTERNDEF       Patch_DibBlt:FAR16
EXTERNDEF       FastBorder:FAR16
EXTERNDEF       Patch_Output:FAR16
EXTERNDEF       Patch_Pixel:FAR16
EXTERNDEF       RealizeObject:FAR16
EXTERNDEF       ReEnable:FAR16
EXTERNDEF       Patch_RealizeObject:FAR16
EXTERNDEF       Patch_ScanLR:FAR16
EXTERNDEF       Patch_SetDIBitsToDevice:FAR16
EXTERNDEF       Patch_SetPalette:FAR16
EXTERNDEF       SetPaletteTranslate:FAR16
EXTERNDEF       Patch_StrBlt:FAR16
EXTERNDEF       Patch_StretchBlt:FAR16
EXTERNDEF       Patch_StretchDIBits:FAR16
EXTERNDEF       Patch_SaveScreenBitmap:FAR16
EXTERNDEF       Patch_UpdateColors:FAR16
EXTERNDEF       GENERIC_OutputDebugString:FAR16

; gnvalmod
EXTERNDEF       ValidateMode:FAR16
EXTERNDEF       GENERIC_ValidateMode:FAR16
EXTERNDEF       GN_ValidatePhysicalMode:FAR16
EXTERNDEF       GN_GetFreeBlockSizeForMode:FAR16
EXTERNDEF       GN_CheckForMultipleOf8Mode:FAR16

; gnvdd.asm
EXTERNDEF       lpfnVddEntryPoint:DWORD
EXTERNDEF       dwThisLogicalDeviceHandle_In_NVTEXT16:DWORD
EXTERNDEF       GN_VddGetEntryPoint:FAR16
EXTERNDEF       GN_VddGetDeviceHandle:FAR16
EXTERNDEF       GN_VddEnableDevice:FAR16
EXTERNDEF       GN_VddCloseDevice:FAR16
EXTERNDEF       GN_VddGetDisplayInfo:FAR16
EXTERNDEF       GN_VddDriverRegister:FAR16
EXTERNDEF       GN_VddDriverUnregister:FAR16
EXTERNDEF       GN_VddRegisterDisplayDriver:FAR16
EXTERNDEF       GN_VddSaveDriverState:FAR16
EXTERNDEF       GN_VddEnableIOTrapping:FAR16
EXTERNDEF       GN_VddSetLockAndUnlockOffscreen:FAR16
EXTERNDEF       GN_VddReenumerateDevNode:FAR16
EXTERNDEF       GN_VddDriverRegister:FAR16
EXTERNDEF       GN_VddSaveDriverState:FAR16
EXTERNDEF       GN_VddEnableIOTrapping:FAR16
EXTERNDEF       GN_VddGetDisplayInfo:FAR16
EXTERNDEF       GN_VddIsDriverAttachedToDesktop:FAR16

;  gnwrap.asm
EXTERNDEF       BitBlt_WithExclude:FAR16
EXTERNDEF       Pixel_WithExclude:FAR16
EXTERNDEF       ScanLR_WithExclude:FAR16
EXTERNDEF       ExtTextOut_WithExclude:FAR16
EXTERNDEF       SetDIBitsToDevice_WithExclude:FAR16
EXTERNDEF       UpdateColors_WithExclude:FAR16
EXTERNDEF       StretchBlt_WithExclude:FAR16
EXTERNDEF       StretchDIBits_WithExclude:FAR16
EXTERNDEF       SaveScreenBitmap_WithExclude:FAR16
EXTERNDEF       Output_WithExclude:FAR16
EXTERNDEF       GN_TurnExcludeOffOrOn:FAR16
CLOSESEG _NVTEXT16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; DIBENG exports
EXTERNDEF       DIB_BeginAccess:FAR16
EXTERNDEF       DIB_EndAccess:FAR16
EXTERNDEF       DIB_BitBlt:FAR16
EXTERNDEF       DIB_BitmapBits:FAR16
EXTERNDEF       DIB_ColorInfo:FAR16
EXTERNDEF       DIB_Control:FAR16
EXTERNDEF       DIB_DibBltExt:FAR16
EXTERNDEF       DIB_Disable:FAR16
EXTERNDEF       DIB_Enable:FAR16
EXTERNDEF       DIB_EnumObjExt:FAR16
EXTERNDEF       DIB_ExtTextOut:FAR16
EXTERNDEF       DIB_GetCharWidth:FAR16
EXTERNDEF       DIB_GetPaletteExt:FAR16
EXTERNDEF       DIB_GetPaletteTranslateExt:FAR16
EXTERNDEF       DIB_Inquire:FAR16
EXTERNDEF       DIB_Output:FAR16
EXTERNDEF       DIB_Pixel:FAR16
EXTERNDEF       DIB_RealizeObjectExt:FAR16
EXTERNDEF       DIB_ScanLR:FAR16
EXTERNDEF       DIB_DibToDevice:FAR16
EXTERNDEF       DIB_SetPaletteExt:FAR16
EXTERNDEF       DIB_SetPaletteTranslateExt:FAR16
EXTERNDEF       DIB_StrBlt:FAR16
EXTERNDEF       DIB_StretchBlt:FAR16
EXTERNDEF       DIB_StretchDIBits:FAR16
EXTERNDEF       DIB_UpdateColorsExt:FAR16
EXTERNDEF       DIB_SetCursorExt:FAR16
EXTERNDEF       DIB_MoveCursorExt:FAR16
EXTERNDEF       DIB_CheckCursorExt:FAR16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

