;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnenable.asm
;
; Purpose:      This file holds the Enable and ReEnable display driver
;               entry points.
;
;==============================================================================
.586
incDrawMode = 1
incDevice   = 1
incLogical  = 1
include macros.dat
include gdidefs.inc
include dibeng.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc
include gngdi.inc
include hwgdi.inc
include escape.inc
include minivdd.inc
include modeext.inc

; Keep the caps we are supporting here for ease of modification
CAPS_C1         TEXTEQU <C1_DIBENGINE + C1_REINIT_ABLE + C1_COLORCURSOR + C1_GLYPH_INDEX + C1_GAMMA_RAMP>
CAPS_CLIP       TEXTEQU <CP_RECTANGLE>
CAPS_CURVE      TEXTEQU <0>
CAPS_LINE       TEXTEQU <LC_POLYLINE + LC_STYLED + LC_POLYSCANLINE>
CAPS_POLYGONAL  TEXTEQU <PC_SCANLINE + PC_ALTPOLYGON + PC_RECTANGLE + PC_WINDPOLYGON + PC_INTERIORS>
CAPS_RASTER     TEXTEQU <RC_BITBLT + RC_BITMAP64 + RC_GDI20_OUTPUT + RC_DI_BITMAP + RC_DIBTODEV + RC_BIGFONT + RC_STRETCHBLT + RC_STRETCHDIB + RC_OP_DX_OUTPUT + RC_SAVEBITMAP + RC_DEVBITS>
CAPS_TEXT       TEXTEQU <TC_CP_STROKE + TC_RA_ABLE>

; For different resolutions, we need to give GDI different Window/Viewport
; scaling combinations. There is an array of data in the following format
; located in this file -- each element in the array is for a different
; screen resolution.
METRICDATA      STRUCT
        wMetLowX        DW      0
        wMetLowY        DW      0
        wEngLowX        DW      0
        wEngLowY        DW      0
        wEngHighX       DW      0
        wEngHighY       DW      0
        wTwipX          DW      0
        wTwipY          DW      0
METRICDATA      ENDS


OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA


OPENSEG  _NVTEXT16, USE16, CODE

ALIGN 2
PUBLIC  sMetricData
sMetricData  LABEL   METRICDATA
;       nXres,  nYres,  nEngLoX nEngLoY,nEngHiX,nEngHiY,nTwipX, nTwipY
DW      320,    200,    127,    127,    64,     64,     64,     64
DW      640,    480,    254,    254,    127,    127,    127,    127
DW      800,    600,    318,    318,    159,    159,    159,    159
DW      1024,   768,    407,    407,    203,    203,    203,    203
DW      1280,   1024,   509,    509,    254,    254,    254,    254
DW      1152,   864,    457,    457,    230,    230,    230,    230
DW      1600,   1200,   635,    635,    320,    320,    320,    320
DW      2048,   1536,   635,    635,    320,    320,    320,    320
DW      0

IFDEF DEBUG
szReEnable      DB      'DISPLAY:GENERIC_ReEnable',0DH,0AH,0
szEnable        DB      'DISPLAY:GENERIC_Enable',0DH,0AH,0
ENDIF

;==============================================================================
;
; Function:     GENERIC_ReEnable
;
; Purpose:      This function is the ReEnable display driver entry point.
;               Mostly it just sequences the two Enable calls to do the
;               ReEnable plus a few extra things unique to ReEnable.
;
; Arguments:    ReEnable Display Driver Stack Frame
;
; Returns:      ax      0       then failure
;                       non-0   then success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_ReEnable, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMD   lpGdiInfoA
LOCALS  dwTempPalette,400H
OPENPROC
        PUSHR   ds,esi,edi

IFDEF DEBUG
        mov     ax,cs
        shl     eax,10H
        lea     ax,szReEnable
        push    eax
        call    GENERIC_OutputDebugString
ENDIF

        ; Load up the GNLOGDEVCX
        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]

        cmp     ds:[esi].GNLOGDEVCX.bSetModeBypass,0
        je      @F
        mov     ds:[esi].GNLOGDEVCX.bSetModeBypass,0
        jmp     Bypass_ReEnable

@@:
        ; Save the ptr to the GDIINFO table in case it has changed
        mov     eax,lpGdiInfoA
        mov     ds:[esi].GNLOGDEVCX.lpGdiInfo,eax

        ; Save off the palette in case we need to restore it later
        mov     ax,ss
        mov     es,ax
        lea     edi,dwTempPalette
        lea     esi,[esi].GNLOGDEVCX.dwPalette
        mov     ecx,100H
        rep     movs dword ptr es:[edi], ds:[esi]

        ; The DIBENG is clobbering the spotsize which should only be
        ; touched by GDI so we save and restore it around the call.
        lds     di,lpGdiInfoA
        push    ds:[edi].GDIINFO.dpSpotSizeX
        push    ds:[edi].GDIINFO.dpSpotSizeY

        ; Do the GDIINFO flavor of the enable call.
        sub     eax,eax
        mov     ecx,1
        push    lpGdiInfoA
        push    cx
        push    eax
        push    eax
        inc     eax
        push    eax
        call    GENERIC_Enable

        ; Here is where the Restore happens.
        pop     ds:[edi].GDIINFO.dpSpotSizeY
        pop     ds:[edi].GDIINFO.dpSpotSizeX

        ; Do the Enable Device flavor of the enable call.
        sub     eax,eax
        push    lpDstDev
        push    ax
        push    eax
        push    eax
        inc     eax
        push    eax
        call    GENERIC_Enable

        ; Restore and set the palette if we are palettized.
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]
        mov     es,cs:[wFlatDataSel_In_NVTEXT16]
        test    es:[esi].GNLOGDEVCX.Dibeng.deFlags,PALETTIZED
        jz      @F

        ; Restore the palette we saved off
        ; This palette is not the one that actually gets set
        ; into the hardware. That palette in in the CRTC objects
        ; and their palette does not get trashed. Hence, there is
        ; no need to resend the palette to the CRTC objects.
        lea     edi,[esi].GNLOGDEVCX.dwPalette
        mov     ax,ss
        mov     ds,ax
        lea     esi,dwTempPalette
        mov     ecx,100H
        rep     movs dword ptr es:[edi], ds:[esi]
@@:

Bypass_ReEnable:
        mov     eax,1
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GENERIC_Enable
;
; Purpose:      This function is the Enable display driver entry point.
;               It deals with multi-monitor stuff and then hands off to
;               the real enable routine which implements the enable
;               for the selected device.
;
; Arguments:    Enable Display Driver Stack Frame
;
; Returns:      ax      0       then failure
;                       non-0   then success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_Enable, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wAction
PARMD   lpDstDevType
PARMD   lpOutputFile
PARMD   lpData
OPENPROC
        PUSHR   ds,esi,edi

IFDEF DEBUG
        mov     ax,cs
        shl     eax,10H
        lea     ax,szEnable
        push    eax
        call    GENERIC_OutputDebugString
ENDIF

        ; This is one of the two functions in the driver that can be
        ; called first. If we haven't initted yet, we must do it now.
        cmp     cs:[wNVTEXT32_Alias_In_NVTEXT16],0
        jne     @F
        push    lpDstDevType
        push    lpDstDev                ; GDIINFO PTR the first time.
        call    GN_DllEntryPoint
        or      eax,eax
        je      DoneIt
@@:
        ; Load up the GNLOGDEV
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]
        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]

        ; Which case is it? Primary or secondary
        mov     eax,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        cmp     eax,1
        je      AsIfPrimary

        ; We are a secondary card. Hence, we must first enable access
        ; to the board. If we are truly multi-monitor compliant, we
        ; can just enable Memory access and leave it on all the time.
        ; If we use I/O (in or out) instructions anywhere, this won't
        ; work because I will not Enable I/O access to the board.
        pushd   ENABLE_MEM
        call    GN_VddEnableDevice
        cmp     eax,ENABLE_ERROR
        je      Exit_Fail

AsIfPrimary:
        ; At this point, we are either primary, or we are secondary and
        ; Memory accesses to our board have been enabled. Hence, we can
        ; now dispatch to the correct subfunction.
        ; Split based on action.
        push    esi
        push    lpDstDev
        push    wAction
        push    lpDstDevType
        push    lpOutputFile
        push    lpData

        push    OFFSET DoneIt
        movzx   eax,wAction
        test    al,1
        jz      GN_EnableDevice
        jmp     GN_EnableGdiInfo

Exit_Fail:
        sub     eax,eax
DoneIt:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_EnableGdiInfo
;
; Purpose:      This function is jumped to from the Enable display driver
;               entry point once it is known that the flavor of the call
;               is to get the GDIINFO table.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX offset
;
; Returns:      ax      0       then failure
;                       non-0   then success
;
; Preserve:     esi,edi
;==============================================================================
DECPROC GN_EnableGdiInfo, PASCAL, FRAME, NEAR
PARMD   pGNLogdevCXOffset
PARMD   lpThisGdiInfo
PARMW   wAction
PARMD   lpDstDevType
PARMD   lpOutputFile
PARMD   lpData
OPENPROC
        PUSHR   esi,edi

        ; Let the DIBENG do its own thing
        push    lpThisGdiInfo
        push    wAction
        push    lpDstDevType
        push    lpOutputFile
        push    lpData
        call    DIB_Enable

        PUSHR   ds

        sub     esi,esi
        lds     si,lpThisGdiInfo
        mov     ds:[esi].GDIINFO.dpVersion,400H
        mov     ds:[esi].GDIINFO.dpTechnology,DT_RASDISPLAY
        mov     ds:[esi].GDIINFO.dpHorzSize,208
        mov     ds:[esi].GDIINFO.dpVertSize,156
;        mov     ds:[esi].GDIINFO.dpHorzRes
;        mov     ds:[esi].GDIINFO.dpVertRes
;        mov     ds:[esi].GDIINFO.dpBitsPixel
        mov     ds:[esi].GDIINFO.dpPlanes,1
        mov     ds:[esi].GDIINFO.dpNumBrushes,-1
        mov     ds:[esi].GDIINFO.dpNumPens,20*5
        mov     ds:[esi].GDIINFO.dpCapsFE,0
        mov     ds:[esi].GDIINFO.dpNumFonts,0
        mov     ds:[esi].GDIINFO.dpNumColors,-1
        mov     ds:[esi].GDIINFO.dpDEVICEsize,SIZE GNLOGDEVCX
        mov     ds:[esi].GDIINFO.dpCurves,CAPS_CURVE
        mov     ds:[esi].GDIINFO.dpLines,CAPS_LINE
        mov     ds:[esi].GDIINFO.dpPolygonals,CAPS_POLYGONAL
        mov     ds:[esi].GDIINFO.dpText,CAPS_TEXT
        mov     ds:[esi].GDIINFO.dpClip,CAPS_CLIP
        mov     ds:[esi].GDIINFO.dpRaster,CAPS_RASTER
        mov     ds:[esi].GDIINFO.dpAspectX,10
        mov     ds:[esi].GDIINFO.dpAspectY,10
        mov     ds:[esi].GDIINFO.dpAspectXY,14
        mov     ds:[esi].GDIINFO.dpStyleLen,28

        ; Based upon the screen resolution, we need to find
        ; the best set of Window/Viewport scalings.
        mov     ebx,OFFSET sMetricData
@@:     mov     ax,cs:[ebx].METRICDATA.wMetLowX
        cmp     ax,ds:[esi].GDIINFO.dpHorzRes
        je      Match
        add     ebx,SIZE METRICDATA
        cmp     cs:[ebx].METRICDATA.wMetLowX,0
        jne     @B
        mov     ebx,OFFSET sMetricData
Match:
        mov     ds:[esi+0].GDIINFO.dpMLoWin,2080
        mov     ds:[esi+2].GDIINFO.dpMLoWin,1560
        mov     ax,cs:[ebx].METRICDATA.wMetLowY
        mov     dx,cs:[ebx].METRICDATA.wMetLowX
        neg     ax
        mov     ds:[esi+0].GDIINFO.dpMLoVpt,dx
        mov     ds:[esi+2].GDIINFO.dpMLoVpt,ax

        mov     ds:[esi+0].GDIINFO.dpMHiWin,20800
        mov     ds:[esi+2].GDIINFO.dpMHiWin,15600
        mov     ds:[esi+0].GDIINFO.dpMHiVpt,dx
        mov     ds:[esi+2].GDIINFO.dpMHiVpt,ax

        mov     ds:[esi+0].GDIINFO.dpELoWin,325
        mov     ds:[esi+2].GDIINFO.dpELoWin,325
        mov     ax,cs:[ebx].METRICDATA.wEngLowY
        neg     ax
        mov     ds:[esi+2].GDIINFO.dpELoVpt,ax
        mov     ax,cs:[ebx].METRICDATA.wEngLowX
        mov     ds:[esi+0].GDIINFO.dpELoVpt,ax

        mov     ds:[esi+0].GDIINFO.dpEHiWin,1625
        mov     ds:[esi+2].GDIINFO.dpEHiWin,1625
        mov     ax,cs:[ebx].METRICDATA.wEngHighY
        neg     ax
        mov     ds:[esi+2].GDIINFO.dpEHiVpt,ax
        mov     ax,cs:[ebx].METRICDATA.wEngHighX
        mov     ds:[esi+0].GDIINFO.dpEHiVpt,ax

        mov     ax,cs:[ebx].METRICDATA.wTwipY
        neg     ax
        mov     ds:[esi+2].GDIINFO.dpTwpVpt,ax
        mov     ax,cs:[ebx].METRICDATA.wTwipX
        mov     ds:[esi+0].GDIINFO.dpTwpVpt,ax
        mov     ds:[esi+0].GDIINFO.dpTwpWin,2340
        mov     ds:[esi+2].GDIINFO.dpTwpWin,2340

        ; Always set to 96 right now. GDI overrides this as needed.
        mov     ax,96
        mov     ds:[esi].GDIINFO.dpLogPixelsX,ax
        mov     ds:[esi].GDIINFO.dpLogPixelsY,ax

        ; The display driver doesn't use File Name Pair matching
        mov     ds:[esi].GDIINFO.dpDCManage,DC_IgnoreDFNP

        ; The CAPS_C1 are at the top of the file.
        mov     ds:[esi].GDIINFO.dpCaps1,CAPS_C1

        mov     es,cs:[wFlatDataSel_In_NVTEXT16]
        mov     edi,pGNLogdevCXOffset
        mov     eax,es:[edi].GNLOGDEVCX.dwFontFormat
        or      ds:[esi].GDIINFO.dpCaps1,ax

        ; The dpSpotSize are set by GDI, not the driver -- do not touch!
        ; mov    ds:[esi].GDIINFO.dpSpotSizeX
        ; mov    ds:[esi].GDIINFO.dpSpotSizeY

        ; The color resolution. We will always be 8 bits per gun.
        mov     ds:[esi].GDIINFO.dpColorRes,24

        mov     ecx,pGNLogdevCXOffset
        mov     eax,lpThisGdiInfo
        mov     es:[ecx].GNLOGDEVCX.lpGdiInfo,eax

        ; Set the desktop mode first.
        PUSHR   es
        POPR    ds
        push    ecx
        call    GN_GetLogicalAndPhysicalModes

        ; Reload stuff after the call
        sub     esi,esi
        lds     si,lpThisGdiInfo
        mov     es,cs:[wFlatDataSel_In_NVTEXT16]

        ; If we are not PALETTE_MANAGED, then clear out palette fields
        cmp     ds:[esi].GDIINFO.dpBitsPixel,8
        je      IsPal
        sub     eax,eax
        mov     ds:[esi].GDIINFO.dpNumPalReg,ax
        mov     ds:[esi].GDIINFO.dpPalReserved,ax
        mov     ds:[esi].GDIINFO.dpColorRes,ax
        jmp     @F
IsPal:  mov     ds:[esi].GDIINFO.dpNumPalReg,256
        or      ds:[esi].GDIINFO.dpRaster,RC_PALETTE
        mov     ds:[esi].GDIINFO.dpPalReserved,20
        mov     ds:[esi].GDIINFO.dpNumColors,20
        mov     ebx,es:[edi].GNLOGDEVCX.pGNCrtcCX
        mov     eax,es:[ebx].GNCRTCCX.dwBitsPerColorChannel
        imul    eax,3
        mov     ds:[esi].GDIINFO.dpColorRes,ax
@@:
        POPR    ds

        ; Give the correct return value
        mov     eax,SIZE GDIINFO

        POPR    esi,edi
CLOSEPROC

;==============================================================================
;
; Function:     GN_EnableDevice
;
; Purpose:      This function is jumped to from the Enable display driver
;               entry point once it is known that the flavor of the call
;               is to Enable the Physical Device.  (Actually this flavor
;               of call breaks out into two itself -- Enable the device
;               or just return the device context.)
;
; Arguments:    Enable Display Driver Stack Frame
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX offset
;
; Returns:      ax      0       then failure
;                       non-0   then success
;
; Preserve:     ss,ebp,esp
;==============================================================================
DECPROC GN_EnableDevice, PASCAL, FRAME, NEAR
PARMD   pGNLogdevCXOffset
PARMD   lpDstDevice
PARMW   wAction
PARMD   lpDstDevType
PARMD   lpOutputFile
PARMD   lpData
LOCALD  dwCRTCCount
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset

        call    GN_CheckInterlacedStereoConnection
        mov     ds:[esi].GNLOGDEVCX.bInterlacedStereo, al
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ecx].GNBOARDCX.pHWBoardCX
        push    eax
        call    NV_SetInterlacedStereoMode

        ; The high bit of the Action argument tells whether or not to
        ; enable the hardware or simply return the device context.
        movzx   eax,wAction
        test    ah,80H
        jz      EnableDev

GetDeviceContext:
        mov     esi,pGNLogdevCXOffset
        sub     edi,edi
        les     di,lpDstDevice
        mov     ecx,SIZE GNLOGDEVCX
        rep     movs byte ptr es:[edi], ds:[esi]
        jmp     DoneIt

EnableDev:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Everything in the driver is based upon the GNLOGDEVCX as the
        ; sole piece of context data. Before the very first time this
        ; function is called, however, we technically do not yet have
        ; a GNLOGDEVCCX. A temporary one was initalized at driver load
        ; time and has been used up to this point. We copy over the
        ; template to the one passed in here and reassign the ptr to
        ; the real GNLOGDEVCX.
        mov     eax,lpDstDevice
        shr     eax,10H
        push    eax
        call    GN_GetSelectorBase
        cmp     eax,pGNLogdevCXOffset
        je      DoneFirst

        ; Copy the GNLOGDEVCX over to the new ptr
        mov     esi,pGNLogdevCXOffset
        sub     edi,edi
        les     di,lpDstDevice
        mov     ecx,SIZE GNLOGDEVCX
        rep     movs byte ptr es:[edi], ds:[esi]

        ; Free the old ptr
        mov     esi,eax
        push    pGNLogdevCXOffset
        call    HW_FreeFlatMemory

        ; Load these up
        mov     es,cs:[wNVTEXT16_Alias_In_NVTEXT16]
        mov     fs,cs:[wNVTEXT32_Alias_In_NVTEXT16]

        ; Set the new ptr
        mov     es:[CSpGNLogicalDevice_In_NVTEXT16],esi
        mov     fs:[CSpGNLogicalDevice],esi

        ; Also fill these in
        mov     eax,lpDstDevice
        mov     es:[CSlp16LogDevice_In_NVTEXT16],eax
        mov     fs:[CSlp16LogDevice],eax
        mov     ds:[esi].GNLOGDEVCX.Dibeng.delpPDevice,eax

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Let everyone know that Logical device has moved.
        push    esi
        pushd   MSG_LOGDEVMOVED
        push    esi                             ; push new prt
        push    pGNLogdevCXOffset               ; push old ptr
        call    GN_LogdevMessage
        mov     pGNLogdevCXOffset,esi
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DoneFirst:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Ugh! If we are not "attached to the desktop", then
        ; don't set the mode! We might not have any CRTCCXs
        ; if we are not attached to the desktop.
        call    GN_VddIsDriverAttachedToDesktop
        or      ax,ax
        je      DoneIt
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ; Do not get any modes from the registry, etc. All the GDIINFO
        ; stuff and mode fetching should have been snapshotted at
        ; GetGdiInfo time. In this call, just set whatever is in the
        ; CRTCCXs. Some weird apps and WHQL stuff just call Enable and
        ; not ReEnable after going out to a DOS box. They expect that
        ; to just reset the mode that existed before the DOS box.

        push    esi
        call    GN_SetLogicalAndPhysicalModes

        ; Make sure we turn off busy so we can paint to the screen.
        and     ds:[esi].GNLOGDEVCX.Dibeng.deFlags,NOT BUSY
        mov     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0

        ; Register the driver with the VDD
        call    GN_VddDriverRegister

        ; Tell the VDD to save the driver state
        call    GN_VddSaveDriverState

        ; Disable IO trapping if we are not a secondary device.
        ; The GN_VddEnableIOTrapping checks the device ID.
        sub     eax,eax
        push    eax
        call    GN_VddEnableIOTrapping

        ; We need to force all logical devices to redraw themselves
        ; since all the desktops may have been repositioned in
        ; video memory.
        push    esi
        call    GN_UserRepaint

        ; We let the driver load if it is disabled, but we don't
        ; give it a GNCRTCCX. Protect against NULL ptr.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        or      ebx,ebx
        je      DoneIt

IF DIB_FAST_EXIT
        ; Init the Fast Exit Stiff for DIBS
        push    ds:[ebx].GNCRTCCX.lpNvmodeDispData
        call    GN_InitDibFastExit
ENDIF

DoneIt:
        ; Return Success
        mov     eax,-1
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetLogicalAndPhysicalModes
;
; Purpose:      This function gets the logical (GDIINFO or desktop) mode
;               as well as the physical modes for all the CRTCs attached
;               to the logical device.
;
; Arguments:
;               ds                      CXSel
;               pGNLogdevCXOffset       GNLOGDEVCX offset
;               lpThisGdiInfo
;
; Returns:
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetLogicalAndPhysicalModes, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
LOCALW  wForcedModePresent
LOCALV  sDispInfo,DISPLAYINFO
LOCALD  lpDispInfo
LOCALV  sDesktopModeInfo,MODEINFO
LOCALD  lpDesktopModeInfo
LOCALD  dwSettableModeFreeBlockSize
LOCALD  dwMemoryNeededFor640x480x8
LOCALS  LocalDevData,(MAX_ATTACHED_CRTCS*4)
LOCALD  lpLocalDevData
LOCALS  aHWCrtcCX,(MAX_ATTACHED_CRTCS*4)
LOCALD  lpHWCrtcCX
LOCALS  aGNCrtcCX,(MAX_ATTACHED_CRTCS*4)
LOCALD  lpGNCrtcCX
LOCALD  dwOriginalXRes
LOCALD  dwOriginalYRes
LOCALD  dwCloneRefreshRate
LOCALW  wClone
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Build some things we will need
        mov     ax,ss
        shl     eax,10H
        lea     ax,sDispInfo
        mov     lpDispInfo,eax
        lea     ax,sDesktopModeInfo
        mov     lpDesktopModeInfo,eax
        lea     ax,LocalDevData
        mov     lpLocalDevData,eax
        lea     ax,aHWCrtcCX
        mov     lpHWCrtcCX,eax
        lea     ax,aGNCrtcCX
        mov     lpGNCrtcCX,eax

        ; We will be needing this
        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[eax].GNBOARDCX.pHWBoardCX
        pushd   640
        pushd   480
        pushd   8
        call    HW_GetModeDimensions
        mov     dwMemoryNeededFor640x480x8,ebx
        
        ; Get the registry key for clone behavior
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetCloneDesktopSelection,es,ax
        mov     wClone,ax

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Get the Desktop mode that the system wants to set
        ; This is the LOGICAL DEVICE mode.
        push    lpDispInfo
        pushd   VDD_GET_DISPLAY_CONFIG
        call    GN_VddGetDisplayInfo

        ; Microsoft uses a 0 return for the refresh rate to indicate
        ; that we should make the CONFIG2 call for display res info.
        cmp     sDispInfo.diRefreshRateMax,0
        jne     @F
        push    lpDispInfo
        pushd   VDD_GET_DISPLAY_CONFIG2
        call    GN_VddGetDisplayInfo
@@:
        ; Save the xres,yres that Windows wants us to set
        sub     ecx,ecx
        lfs     cx,lpDispInfo
        mov     eax,dword ptr fs:[ecx].DISPLAYINFO.diXRes
        mov     dword ptr ds:[esi].GNLOGDEVCX.wWindowsXRes,eax

        ; Make sure that the mode Windows is asking us for
        ; hasn't been wierdly modified by Windows. If it has,
        ; then correct it to our real mode.
        mov     ebx,lpDispInfo
        lea     eax,[ebx].DISPLAYINFO.diXRes
        push    eax
        lea     eax,[ebx].DISPLAYINFO.diYRes
        push    eax
        call    GN_CheckForMultipleOf8Mode

        ; Peg the pixel depth to something we can handle
        sub     ecx,ecx
        lfs     cx,lpDispInfo
        mov     al,fs:[ecx].DISPLAYINFO.diBpp
        cmp     al,08H
        je      @F
        cmp     al,10H
        je      @F
        cmp     al,20H
        je      @F
        mov     al,08H
@@:     mov     fs:[ecx].DISPLAYINFO.diBpp,al

        ; Get this into a MODEINFO format which we need later
        sub     ebx,ebx
        les     bx,lpDesktopModeInfo
        movzx   eax,fs:[ecx].DISPLAYINFO.diXRes
        mov     es:[ebx].MODEINFO.dwXRes,eax
        mov     dwOriginalXRes,eax
        movzx   eax,fs:[ecx].DISPLAYINFO.diYRes
        mov     es:[ebx].MODEINFO.dwYRes,eax
        mov     dwOriginalYRes,eax
        movzx   eax,fs:[ecx].DISPLAYINFO.diBpp
        mov     es:[ebx].MODEINFO.dwBpp,eax

        ; Ugh! If we are not "attached to the desktop", then
        ; don't set the mode! We might not have any CRTCCXs
        ; if we are not attached to the desktop.
        call    GN_VddIsDriverAttachedToDesktop
        or      ax,ax
        je      JustRes
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Is this a first boot (are we initting still?)
        cmp     ds:[esi].GNLOGDEVCX.dwInitting,0
        je      NotInit0

        ; OK, this is a pain. There is one circumstance under
        ; which we do not want to set the desktop mode that
        ; Windows is asking us to. If the user changed the color
        ; depth, but said, reboot to make it happen, then we might
        ; not set what Windows asks. Why? Because, suppose the user
        ; changed the color depth on the CRT and asked for a reboot.
        ; Then the user unplugs the CRT and goes with a DFP and
        ; boots up.  The color depth switch does not apply to the
        ; DFP, but rather the "Shutdown device" which was the CRT.
        ; No biggie, this scenario will only happen on boot. All
        ; we need to do is write out the mode Windows is asking us
        ; to boot in to the Shutdown device. If the shutdown device
        ; is the current device, that is OK.

        ; OK, what device did we shut down on last time?
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    ds:[esi].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard
        pushd   0
        push    lpLocalDevData
        CALLFAR16IND lpfnGetDisplayDevice,es,ax

        ; If there was no physical device 0 on this logical device,
        ; then don't worry about it.
        or      ax,ax
        je      NotInit0
        
        ; Get the TV Format for this device from the ResMan. Please note
        ; if this device is not a TV, this function will return 0.
        mov     eax,dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    ds:[eax].GNCRTCCX.pHWCrtcCX
        push    dword ptr LocalDevData
        call    HW_GetResManTvFormat
        shl     eax,10h
        and     dword ptr LocalDevData,0ff00ffffh
        or      dword ptr LocalDevData,eax

        ; Write out the desired desktop mode for that device
        mov     eax,dword ptr LocalDevData
        mov     sDesktopModeInfo.MIDevData,eax
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    lpDesktopModeInfo
        mov     eax,0
        cmp     wClone,1
        jne     @F
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,2
        jl      @F
        mov     eax,1
@@:     push    eax     
        CALLFAR16IND lpfnSetDeviceDesktopMode,es,ax

        ; Now what device are we booting on this time?
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX
        call    GN_GetDevData
        cmp     sDesktopModeInfo.MIDevData.cType,al
        je      MergeInit
        mov     sDesktopModeInfo.MIDevData,eax

        ; Get Last Desktop mode set on this new device
        push    sDesktopModeInfo.dwBpp
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    lpDesktopModeInfo
        mov     eax,0
        cmp     wClone,1
        jne     @F
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,2
        jl      @F
        mov     eax,1
@@:     push    eax     
        CALLFAR16IND lpfnGetDeviceDesktopMode,es,ax
        pop     sDesktopModeInfo.dwBpp
        jmp     MergeInit

NotInit0:
        ; Since we are not initting, we can just use the DevData
        ; for PhysicalDevice0
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX
        call    GN_GetDevData
        mov     sDesktopModeInfo.MIDevData,eax
MergeInit:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Now we may modify the desktop mode the system requested.

        ; Get any forced desktop mode
        push    sDesktopModeInfo.dwBpp
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    lpDesktopModeInfo
        CALLFAR16IND lpfnGetForcedDesktopMode,es,ax
        mov     wForcedModePresent,ax
        mov     eax,sDesktopModeInfo.dwBpp
        pop     sDesktopModeInfo.dwBpp

        ; If Windows is booting, then we can change the color depth.
        ; Otherwise, we cannot.
        cmp     dwThisLogicalDeviceHandle_In_NVTEXT16,1
        jne     @F
        cmp     ds:[esi].GNLOGDEVCX.dwInitting,0
        je      @F
        mov     sDesktopModeInfo.dwBpp,eax
@@:

        ; It is possible that the forced desktop mode has XRes = 0
        ; This is a signal to use the largest EDID mode.
        cmp     sDesktopModeInfo.dwXRes,0
        jne     GetRefreshRateForDesktop

        ; Ask the CRTC for its largest EDID mode.
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    lpDesktopModeInfo
        call    GN_GetLargestEdidMode
        or      ax,ax
        jne     GotDesktopAndRefreshRate

        ; Restore the original desktop size for non DDC display
        mov     eax,dwOriginalXRes
        mov     sDesktopModeInfo.dwXRes,eax
        mov     eax,dwOriginalYRes
        mov     sDesktopModeInfo.dwYRes,eax

GetRefreshRateForDesktop:
        ; We don't have a refresh rate yet. Get one.
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    lpDesktopModeInfo
        call    GN_GetRefreshRateForDesktop
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

GotDesktopAndRefreshRate:
        ; Get the largest block size that we could put this mode in
        ; If this is smaller than a 640x480x8 mode, scream loudly!
        push    esi
        call    GN_GetFreeBlockSizeForMode
        cmp     eax,dwMemoryNeededFor640x480x8
        jae     DoneRecoverFromNoPrimary

RecoverFromNoPrimary:
        push    esi
        call    GN_NoPrimarySurfaceRecovery

DoneRecoverFromNoPrimary:
        mov     dwSettableModeFreeBlockSize,eax

        test    dwSettableModeFreeBlockSize,80000000H
        jnz     EnoughSpace
        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[eax].GNBOARDCX.pHWBoardCX
        pushd   sDesktopModeInfo.dwXRes
        pushd   sDesktopModeInfo.dwYRes
        pushd   sDesktopModeInfo.dwBpp
        call    HW_GetModeDimensions
        cmp     ebx,dwSettableModeFreeBlockSize
        jg      RecoverFromNoPrimary

EnoughSpace:
        and     dwSettableModeFreeBlockSize,NOT 80000000H

        ; Restrict the desktop mode based upon the memory requirements
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    ds:[ebx].GNCRTCCX.lpNvmodeDispData
        push    lpDesktopModeInfo
        push    lpDesktopModeInfo
        push    dwSettableModeFreeBlockSize
        CALLFAR16IND lpfnFindVirtualModeEntry,es,ax

        ; Write any forced desktop mode out to the registry.
        ; Technically Windows will do this, but if we have modified
        ; the desktop mode in any way from what Windows requested,
        ; and if we reboot right away, then Windows will not have
        ; updated its registry section to what we wanted the desktop
        ; to be. Hence, the reboot will not come up in the desktop
        ; that we 'modified' to this time.
        cmp     wForcedModePresent,0
        je      @F
        push    sDesktopModeInfo.dwBpp
        push    sDesktopModeInfo.dwXRes
        push    sDesktopModeInfo.dwYRes
        CALLFAR16IND lpfnSetResolutionAndColorDepth,es,ax
@@:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; There are two possibilities here. We may get called by
        ; 1) Windows Control Panel
        ; 2) NVidiaControlPanel or hotkey handler

        ; If we are called by the Windows Control Panel, then skip
        ; this section and go on to the next section -- normal
        ; processing.
        cmp     ds:[esi].GNLOGDEVCX.dwDisplayConfigPending,0
        je      SkipNVSet

        ; OK, if we get here, it means that we received a
        ; NVTWINVIEW_ACTION_SETGET_DISPLAY_CONFIG escape
        ; call with a delayed state change. The NV Control
        ; Panel or HotKey handler is making this modeset
        ; call to snap the new state into the hardware.

        ; First, clear out the pending display config flag
        mov     ds:[esi].GNLOGDEVCX.dwDisplayConfigPending,0
        lea     edi,[esi].GNLOGDEVCX.sDispConfig

        ; Issue the Begin
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        mov     eax,ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwDeviceMask
        or      eax,80000000H
        push    eax
        call    HW_BeginDisplayChange
        mov     ds:[esi].GNLOGDEVCX.dwBeginDisplayChangeIssued,1

        push    esi
        push    lpDesktopModeInfo
        call    GN_DoFullStateChange
        
        ; Set the refresh rate for the desktop to the refresh rate we
        ; got for the primary CRTCCX from GN_DoFullStateChange.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        sub     edx,edx
        lfs     dx,ds:[ebx].GNCRTCCX.lpCrtcModeInfo
        mov     eax,fs:[edx].MODEINFO.dwRefreshRate
        mov     sDesktopModeInfo.dwRefreshRate,eax

SkipNVSet:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; OK, what we need to do now is to have the hardware take
        ; all the devices and map each device to some head. We
        ; know this can be done or we wouldn't even be here.
        push    esi
        call    GN_CrtcRemapArray
        mov     ds:[esi].GNLOGDEVCX.dwDeviceHasBeenRemapped,eax
        
        ; Now that everything's been remapped, make sure the local copy
        ; of sDesktopModeInfo has the MIDevData for the primary GNCrtcCX.
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX
        call    GN_GetDevData
        mov     sDesktopModeInfo.MIDevData,eax
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ; We may need to override the desktop resolution here if
        ; we are responding to an Enable call after the NV Control Panel
        ; or the hotkey handler issued ChangeDisplaySettings. If this
        ; Enable call is in response to the Windows Control Panel then
        ; use the desktop resolution already provided.
        cmp     ds:[esi].GNLOGDEVCX.dwNvDisplayChangePending,1
        jne     SkipDesktopOverride
        
        mov     ds:[esi].GNLOGDEVCX.dwNvDisplayChangePending,0

        ; We only want to override the desktop resolution if we are writing
        ; separate registry keys for the desktop mode in clone and standard
        ; mode.
        cmp     wClone,1
        jne     SkipDesktopOverride

        ; In order to know which registry key to read, LastCloneDesktopMode or
        ; LastDesktopMode, we need to know if we are in clone mode or not.
        ; Unfortunately, if this Enable is as a result of the hotkey handler,
        ; GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice is not yet set to
        ; the correct value. However, if called by the hotkey handler,
        ; GNLOGDEVCX.dwDisplayConfigPending = 1 and
        ; NVTWINVIEW_DISPLAY_CONFIG.dwActiveDevices is valid.
        cmp     ds:[esi].GNLOGDEVCX.dwDisplayConfigPending,1
        jne     NvControlPanelSet
        
        lea     edi,[esi].GNLOGDEVCX.sDispConfig
        mov     edx,0
        cmp     ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwActiveDevices,1
        je      GetDesktopFromRegistry
        mov     edx,1
        jmp     GetDesktopFromRegistry

NvControlPanelSet:
        mov     edx,0
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,1
        je      GetDesktopFromRegistry
        mov     edx,1

GetDesktopFromRegistry:
        ; Get Last Desktop mode set on this new device
        push    sDesktopModeInfo.dwBpp
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    lpDesktopModeInfo
        push    edx
        CALLFAR16IND lpfnGetDeviceDesktopMode,es,ax
        pop     sDesktopModeInfo.dwBpp

        ; Write any forced desktop mode out to the registry.
        push    sDesktopModeInfo.dwBpp
        push    sDesktopModeInfo.dwXRes
        push    sDesktopModeInfo.dwYRes
        CALLFAR16IND lpfnSetResolutionAndColorDepth,es,ax

SkipDesktopOverride:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Now for the physical modes.
        ; At this point, I am going to assume that a Windows
        ; Control Panel modeset and an NVidia Control Panel
        ; modeset are essentially the same thing.

        ; Run through all devices.
        sub     edi,edi

GetLoop0:
        ; The first PhysicalDevice attached to this logical device
        ; will set, as it's physical mode, the mode and refresh rate of
        ; the desktop. If it is unable to, then back off the phys mode.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        sub     edx,edx
        lfs     dx,ds:[ebx].GNCRTCCX.lpCrtcModeInfo
        mov     eax,fs:[edx].MODEINFO.dwRefreshRate
        mov     dwCloneRefreshRate,eax
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    lpDesktopModeInfo
        call    GN_AssignCrtcModeOnlyBackOffSpatial
        or      edi,edi
        je      SkipRestoreRefreshRate
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        sub     edx,edx
        lfs     dx,ds:[ebx].GNCRTCCX.lpCrtcModeInfo
        mov     eax,dwCloneRefreshRate
        cmp     eax,0
        je      SkipRestoreRefreshRate
        mov     fs:[edx].MODEINFO.dwRefreshRate,eax
        
SkipRestoreRefreshRate:
        jmp     PickupGetLoop

GetLoop1:
        ; For any clone monitors attached to this logical
        ; device, read the CRTC mode from the registry.
        cmp     dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4],0
        je      PickupGetLoop

        ; Read the panning flag for appropriate clone device.
        ; If this flag is 0 than set the desktop resolution,
        ; Otherwise set mode from the registry
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    ds:[ebx].GNCRTCCX.lpBoardRegistryData
        push    ds:[ebx].GNCRTCCX.lpCrtcModeInfo
        CALLFAR16IND lpfnGetClonePanningFlag,es,ax
        or      ax,ax
        jz      GetLoop0

        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    lpDesktopModeInfo
        call    GN_GetCrtcMode

PickupGetLoop:
        inc     edi
        cmp     edi,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jb      GetLoop1
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Now start restricting things ...

        ; If we aren't allowing all modes, then make sure the
        ; desktop mode does not exceed the mode of the primary
        ; CRTC. We always allow virtualization on secondaries.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetAllowMightyModesFlag,es,ax
        or      ax,ax
        jne     @F
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    lpDesktopModeInfo
        call    GN_RestrictOtherModeSpatial
@@:
        sub     edi,edi

RestrictLoop:
        ; We can never set a physical mode larger than the desktop mode
        cmp     dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4],0
        je      @F
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    lpDesktopModeInfo
        call    GN_RestrictCrtcModeSpatial
@@:
        inc     edi
        cmp     edi,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      RestrictLoop
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; We've got all the info we need. We know the desktop and
        ; the physical modes on each attached CRTC.

        ; For the first physical device attached, write out the
        ; last desktop mode for that device.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    lpDesktopModeInfo
        mov     eax,0
        cmp     wClone,1
        jne     @F
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,2
        jl      @F
        mov     eax,1
@@:     push    eax     
        CALLFAR16IND lpfnSetDeviceDesktopMode,es,ax

        ; Also, for the first physical device ONLY, write out its
        ; physical mode. Only the NVidia Control Panel can change
        ; the physical mode of a Clone device. We may have had to
        ; back off what the CRTC modes in the registry for the
        ; Clone devices, but we still don't change them here. This
        ; solves the problem of D3D changing to a fullscreen 6x4
        ; mode, and when it restores the original desktop mode
        ; all cloned device physical modes were left at 6x4. No!
        ; OK, We can write out secondary devices modes as well,
        ; but ONLY if there is not currently any mode for the
        ; secnodary device in the registry.
        sub     edi,edi
@@:
        cmp     dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4],0
        je      ELoop
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    edi
        call    GN_CrtcWriteModeToReg
ELoop:
        inc     edi
        cmp     edi,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      @B
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

JustRes:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Finally! Update the GDIInfo table with our desktop
        sub     edi,edi
        les     di,ds:[esi].GNLOGDEVCX.lpGdiInfo
        mov     eax,sDesktopModeInfo.dwXRes
        mov     ebx,sDesktopModeInfo.dwYRes
        mov     ecx,sDesktopModeInfo.dwBpp
        mov     es:[edi].GDIINFO.dpHorzRes,ax
        mov     es:[edi].GDIINFO.dpVertRes,bx
        mov     es:[edi].GDIINFO.dpBitsPixel,cx
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_SetLogicalAndPhysicalModes
;
; Purpose:      This function sets the desktop mode and and the physical
;               modes in each attached CRTC.
;
; Arguments:
;               ds                      CXSel
;               pGNLogdevCXOffset       GNLOGDEVCX offset
;               lpThisGdiInfo
;
; Returns:
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_SetLogicalAndPhysicalModes, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
LOCALD  dwJustOnePrimaryCanMove
LOCALD  pOtherGNLogdevCXOffset
LOCALD  dwMemoryNeededFor640x480x8
LOCALD  dwMemoryNeededForPrimarySurface
LOCALD  dwMemoryNeededForOtherPrimarySurface
LOCALD  pFirstNonNullCRTCForLogdev2
LOCALD  dwClearAndPanSecondSurface
LOCALD  dwDevDataMask
LOCALD  dwSetModeOnPrimary
LOCALD  dwSetModeOnSecondary
LOCALD  dwForceBothDevices
OPENPROC
        PUSHR   esi,edi

        mov     dwForceBothDevices,0

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; We will be needing this
        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[eax].GNBOARDCX.pHWBoardCX
        pushd   640
        pushd   480
        pushd   8
        call    HW_GetModeDimensions
        mov     dwMemoryNeededFor640x480x8,ebx
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TryItAgain:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Figure out whether one, or more than one (which means 2
        ; right now) primary surfaces will be moving.
        push    esi
        push    dwForceBothDevices
        call    GN_GetOtherLogdev
        mov     dwJustOnePrimaryCanMove,ecx
        or      ecx,ecx
        jne     ClearJustOnePrimary

        mov     dwMemoryNeededForOtherPrimarySurface,ebx
        mov     pOtherGNLogdevCXOffset,edi
        mov     eax,ds:[edi].GNLOGDEVCX.pGNCrtcCX
        mov     pFirstNonNullCRTCForLogdev2,eax

ClearBothPrimaries:
        cmp     ds:[edi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        je      ClearJustOnePrimary
        push    edi
        call    GN_BlankScreen

ClearJustOnePrimary:
        cmp     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        je      @F
        push    esi
        call    GN_BlankScreen
@@:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Isolate ourselves so that the modeswitch is atomic

        cmp     ds:[esi].GNLOGDEVCX.dwInitting,0
        je      @F

        ; We are initting. Are we the second display driver
        cmp     ds:[esi].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard,0
        je      @F

        ; We are the second display driver and we are initting.
        ; We must call the hardware side to allow them to do some
        ; stuff.
        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     ecx,dword ptr ds:[eax].GNBOARDCX.pGNLogdevCX[0]
        mov     edx,dword ptr ds:[eax].GNBOARDCX.pGNLogdevCX[4]
        push    ds:[ecx].GNLOGDEVCX.pHWLogdevCX
        push    ds:[edx].GNLOGDEVCX.pHWLogdevCX
        call    HW_DriverTwinviewStateChanging
@@:

        ; The first thing we do is send the PRE_MODESET message.
        ; All other components should stop all interactions with
        ; the board and video memory from this point until they
        ; see the POST_MODESET message. This way all our code
        ; and calculations herein appear atomic. For the most
        ; part just being inside the Win16 mutex does this.
        push    esi
        pushd   MSG_PREMODESET
        push    esi
        push    ds:[esi].GNLOGDEVCX.Dibeng.delpPDevice
        call    GN_LogdevMessage
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; NOTE: The general fitting algorithm is too difficult to do in
        ; its full generality. Hence, we just implement the two specific
        ; cases that we know are the only ones that can happen right now.
        cmp     dwJustOnePrimaryCanMove,0
        jne     FillInPDevice

FreeBothPrimaries:
        ; Do we even have the secondary?
        mov     eax,ds:[edi].GNLOGDEVCX.dwMemorySizeForPrimarySurface
        or      eax,eax
        je      FillInPDevice

        ; Can we use the same surface?
        or      eax,dwForceBothDevices
        cmp     eax,dwMemoryNeededForOtherPrimarySurface
        je      FillInPDevice

        push    ds:[edi].GNLOGDEVCX.pHWLogdevCX
        call    HW_FreeLogicalSurface
        mov     ds:[edi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FillInPDevice:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Fill in the resolution to the GDIINFO table
        sub     edi,edi
        sub     eax,eax
        sub     ebx,ebx
        les     di,ds:[esi].GNLOGDEVCX.lpGdiInfo
        mov     ax,es:[edi].GDIINFO.dpHorzRes
        mov     bx,es:[edi].GDIINFO.dpVertRes
        mov     cx,es:[edi].GDIINFO.dpBitsPixel
        mov     ds:[esi].DIBENGINEHDR.deWidth,ax
        mov     ds:[esi].DIBENGINEHDR.deHeight,bx
        mov     ds:[esi].DIBENGINEHDR.deBitsPixel,cl

        ; Flesh out the Dibeng structure
        and     ds:[esi].DIBENGINEHDR.deFlags,NOT FIVE6FIVE
        mov     edx,ds:[esi].GNLOGDEVCX.dw16BppFormat
        cmp     cl,16
        jne     @f
        or      ds:[esi].DIBENGINEHDR.deFlags,dx
        cmp     edx,FIVE6FIVE
        je      @F
        mov     cl,15
@@:
        mov     ds:[esi].DIBENGINEHDR.bRealBpp,cl
        les     di,ds:[esi].DIBENGINEHDR.deBitmapInfo
        mov     es:[edi].BitmapInfoHeader.biWidth,eax
        mov     es:[edi].BitmapInfoHeader.biHeight,ebx
        mov     es:[edi].BitmapInfoHeader.biBitCount,cx

        mov     edx,ecx
        imul    ecx,eax
        shr     ecx,3
        inc     ecx
        and     cl,NOT 1
        mov     ds:[esi].DIBENGINEHDR.deWidthBytes,cx

        ; Set the palettized flag if needed
        and     ds:[esi].DIBENGINEHDR.deFlags,NOT PALETTIZED
        cmp     ds:[esi].DIBENGINEHDR.deBitsPixel,8
        jne     @F
        or      ds:[esi].DIBENGINEHDR.deFlags,PALETTIZED
@@:
        ; Get the size of the primary we are going to set.
        ; We NEVER get less than 640x480x8 amount of memory
        ; or we might not be able to set the mode back to a
        ; reasonable desktop (if we are going to 320x200, say)
        ; since we might not be able to get the memory later.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        movzx   eax,ds:[esi].DIBENGINEHDR.deWidth
        push    eax
        movzx   eax,ds:[esi].DIBENGINEHDR.deHeight
        push    eax
        movzx   eax,ds:[esi].DIBENGINEHDR.deBitsPixel
        push    eax
        call    HW_GetModeDimensions
        mov     ds:[esi].DIBENGINEHDR.deDeltaScan,eax
        mov     ecx,ebx
        cmp     ecx,dwMemoryNeededFor640x480x8
        ja      @F
        mov     ecx,dwMemoryNeededFor640x480x8
@@:     mov     dwMemoryNeededForPrimarySurface,ecx
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ; Do we even have the primary?
        mov     eax,ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface
        or      eax,eax
        je      @F

        ; Can we use the same surface?
        or      eax,dwForceBothDevices
        cmp     eax,ebx
        je      @F

        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_FreeLogicalSurface
        mov     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
@@:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        mov     dwClearAndPanSecondSurface,0
        cmp     dwJustOnePrimaryCanMove,0
        jne     AllocJustOnePrimary

AllocBothPrimary:
        ; We need to re-allocate these in order from larger to smaller
        ; or we might not get both. As long as we allocate in the
        ; correct order, code earlier in this routine should have
        ; guaranteed that we will get the surfaces.
        mov     dwClearAndPanSecondSurface,1
        mov     edi,pOtherGNLogdevCXOffset
        mov     eax,dwMemoryNeededForOtherPrimarySurface
        cmp     eax,dwMemoryNeededForPrimarySurface
        ja      SecondSurfaceAlloc

FirstSurfaceAlloc:
        mov     dwSetModeOnPrimary,1;!!!
        cmp     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        jne     HavePrimary

        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    esi
        call    HW_AllocLogicalSurface
        cmp     eax,-1
        jne     FirstAllocOk

        push    esi
        call    GN_NoPrimarySurfaceRecovery
        test    eax,80000000H
        jz      FirstSurfaceAlloc

CheckForceBothDevicesFlag:
        cmp     dwForceBothDevices,0
        mov     dwForceBothDevices,1
        je      TryItAgain

        ;VMEM deadlock. Never should be here.
IFDEF DEBUG
        int     3
ENDIF
@@:
        mov     eax,0DEADBEEFH
        jmp     @B

FirstAllocOk:
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deBitsOffset,eax
        mov     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,ebx
        mov     dwSetModeOnPrimary,ebx

HavePrimary:
        mov     eax,dwMemoryNeededForOtherPrimarySurface
        cmp     eax,dwMemoryNeededForPrimarySurface
        ja      FinishUp

SecondSurfaceAlloc:
        ; Now alloc the other guy
        mov     dwSetModeOnSecondary,1;!!!
        cmp     ds:[edi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        jne     HaveSecondary

        push    ds:[edi].GNLOGDEVCX.pHWLogdevCX
        push    edi
        call    HW_AllocLogicalSurface
        cmp     eax,-1
        jne     @F
        push    esi
        call    GN_NoPrimarySurfaceRecovery
        test    eax,80000000H
        jz      SecondSurfaceAlloc
        jmp     CheckForceBothDevicesFlag

@@:
        mov     ds:[edi].GNLOGDEVCX.Dibeng.deBitsOffset,eax
        mov     ds:[edi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,ebx
        mov     dwSetModeOnSecondary,ebx

HaveSecondary:
        mov     eax,dwMemoryNeededForOtherPrimarySurface
        cmp     eax,dwMemoryNeededForPrimarySurface
        ja      FirstSurfaceAlloc
        jmp     FinishUp
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

AllocJustOnePrimary:
        mov     dwSetModeOnPrimary,1;!!!
        mov     dwSetModeOnSecondary,0
        cmp     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        jne     FinishUp

        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    esi
        call    HW_AllocLogicalSurface
        cmp     eax,-1
        jne     @F
        push    esi
        call    GN_NoPrimarySurfaceRecovery
        test    eax,80000000H
        jz      AllocJustOnePrimary
        jmp     CheckForceBothDevicesFlag

@@:
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deBitsOffset,eax
        mov     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,ebx
        mov     dwSetModeOnPrimary,ebx
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FinishUp:
        ; Tell all other componenets that the modeset is complete.
        push    esi
        pushd   MSG_POSTMODESET
        push    esi
        push    ds:[esi].GNLOGDEVCX.Dibeng.delpPDevice
        call    GN_LogdevMessage

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ; Get the Device Data Mask for all display devices.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ebx
        pushd   0
        call    GN_GetDevDataMask
        or      eax,80000000H
        mov     dwDevDataMask,eax

        ; Did we already issue a BeginDisplayChange in
        ; GetLogicalAndPhysicalModes?
        cmp     ds:[esi].GNLOGDEVCX.dwBeginDisplayChangeIssued,0
        jne     @F
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwDevDataMask
        call    HW_BeginDisplayChange
@@:
        cmp     dwSetModeOnSecondary,0
        je      DoneSecondary

        ; Don't forget to clear the OTHER surface if needed
        cmp     dwClearAndPanSecondSurface,0
        je      @F
        push    edi
        call    GN_BlankScreen
@@:

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; We need to call and reset the mode on the OTHER guy now.
        ; The problem is that the other guys CRTC timings may have
        ; gotten messed up when we set the mode here because we may
        ; have had to do a head reassignment in order to get all the
        ; heads to display to their appropriate output devices. If
        ; we did, then the other logical device's HW CRTC state is
        ; uninitted.
        cmp     dwClearAndPanSecondSurface,0
        je      DoneSecondary

        push    dword ptr ds:[edi].GNLOGDEVCX.pGNCrtcCX
        push    edi
        call    GN_RestoreCrtcState

DoneSecondary:
        ; Now we can actually set the modes in the CRTCCXs
        ; which are attached to THIS logical device.
        ; Let's make the screen look pretty by clearing it to black.
        push    esi
        call    GN_BlankScreen

        sub     edi,edi
        cmp     dwSetModeOnPrimary,0
        je      NextCrtc7

Loopit7:
        ; Is this CRTCCX valid?
        cmp     dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4],0
        je      NextCrtc7

        ; Set the mode on this CRTC.
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    esi
        call    GN_RestoreCrtcState

NextCrtc7:
        inc     edi
        cmp     edi,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      Loopit7

        ; Signal the end of a display state change
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwDevDataMask
        call    HW_EndDisplayChange

        ; We just issued the matching end.
        mov     ds:[esi].GNLOGDEVCX.dwBeginDisplayChangeIssued,0

        ; See if we need to modify some results back to the
        ; Windows requested mode (which is one of our modes
        ; ANDed with 8 -- this only happens in multimon).
        mov     ax,ds:[esi].GNLOGDEVCX.wWindowsXRes
        mov     cx,ds:[esi].GNLOGDEVCX.wWindowsYRes
        mov     bx,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
        cmp     ax,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
        jne     Skip10
        and     bx,NOT 7
        and     cx,NOT 7
        cmp     bx,cx
        jne     Skip10

        mov     cx,ds:[esi].GNLOGDEVCX.wWindowsYRes
        mov     dword ptr ds:[esi].GNLOGDEVCX.wWindowsXRes,0
        mov     ds:[esi].GNLOGDEVCX.Dibeng.deHeight,cx
        sub     edi,edi
        les     di,ds:[esi].GNLOGDEVCX.lpGdiInfo
        mov     es:[edi].GDIINFO.dpVertRes,cx
Skip10:

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetOtherLogdev
;
; Purpose:      This routine is called to get whether or not there
;               is more than one logical device on this board, and,
;               if there is, to get information on the other logical
;               device.
;
; Arguments:
;               ds                      CXSel
;               pGNLogdevCXOffset       GNLOGDEVCX offset
;
; Returns:      ecx     0 if parm pGNLogdevCXOffset is the only device on
;                               the board
;                       non-0 if there is another logdev on this board
;               edi     if ecx is non-0, then edi is the other pGNLogdevCX
;               ebx     if ecx is non-0, then ebx is the memory needed
;                               for the primary surface of the other
;                               pGNLogdevCX
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetOtherLogdev, PASCAL, FRAME, NEAR
PARMD   pGNLogdevCXOffset
PARMD   dwForceBothDevices
OPENPROC
        PUSHR   esi
        mov     esi,pGNLogdevCXOffset

        mov     ecx,1
        sub     edi,edi
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX

        ; Is there another primary surface on this board?
        cmp     ds:[ebx].GNBOARDCX.dwLogicalDeviceInstancesOnThisBoard,1
        je      GotPrimaryData

        ; OK, there is more than 1 primary (that means 2 right now),
        ; but can the other primary be moved? First figure out
        ; which is the OTHER logical device
        mov     edi,ds:[ebx].GNBOARDCX.pGNLogdevCX[0]
        cmp     edi,esi
        jne     @F
        mov     edi,ds:[ebx].GNBOARDCX.pGNLogdevCX[4]
@@:
        ; HACK -- if the OTHER logical device is disabled,
        ; then just say there is only 1 logical device. Do NOT
        ; call IsDriverAttachedToDesktop!! We are in the wrong
        ; display driver for this to get called. Just see if
        ; there is a primary CRTC attached to the other logdev
        cmp     dword ptr ds:[edi].GNLOGDEVCX.pGNCrtcCX,0
        je      GotPrimaryData

        ; The OTHER logical device is enabled. If dwForceBothDevices is true
        ; then it can be moved. Otherwise it shouldn't.
        mov     ebx,ds:[edi].GNLOGDEVCX.dwMemorySizeForPrimarySurface
        cmp     dwForceBothDevices,0
        jne     ForceBothDevices

        ; Check for remapped devices
        mov     eax,ds:[esi].GNLOGDEVCX.dwDeviceHasBeenRemapped
        or      eax,eax
        jz      GotPrimaryData

        ; Determine how many devices has been remapped
@@:
        test    eax,1
        jnz     @F
        shr     eax,1
        jmp     @B

@@:
        ; Don't set mode on the primary if the only one device has been remapped
        shr     eax,1
        je      GotPrimaryData

ForceBothDevices:
        ; Set mode on both devices
        sub     ecx,ecx

GotPrimaryData:
        POPR    esi
CLOSEPROC

;==============================================================================
;
; Function:     GN_NoPrimarySurfaceRecovery
;
; Purpose:      This routine is only called in dire emergencies when
;               we are unable to get the memory for even a 640x480x8
;               primary surface. The display driver should be able to
;               guarantee that this will never happen. As long as its
;               code is correct and the heap doesn't get corrupted,
;               etc. then everything is fine. However, we need to be
;               able to recover somehow if we can't alloc the screen.
;
;               In here, we do a purge of all of video memory and ask
;               the hareware code to reset its offscreen heap manager.
;               This will probably result in some temporary drawing
;               problems, because some offscreen surfaces that are
;               allocated to various clients will be written over as
;               the memory freed behind their backs gets reallocated
;               to other clients. However, these drawing issues will
;               probably be short lived and this strategy does allow
;               us to recover fully.
;
;       JKTODO -- purge all offscreen video memory through special
;       RM call which does not yet exist. OGL wants us to increment the
;       mode counter so that they will reallocate appropriately.
;       We would also need to flush, etc...
;
; Arguments:
;               ds                      CXSel
;               pGNLogdevCXOffset       GNLOGDEVCX offset
;
; Returns:      eax                     New size of free offscreen memory
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_NoPrimarySurfaceRecovery, PASCAL, FRAME, NEAR
PARMD   pGNLogdevCXOffset
LOCALD  dwOldOffscreenFreeSpace
OPENPROC
        PUSHR   esi
        mov     esi,pGNLogdevCXOffset

        ; Send a broadcast message around that video memory
        ; is about to be reset. Since the heap is probably
        ; already trashed, object cleanup may not be that
        ; effective, but it iw worth a shot.

        ; Get the largest block size before purge
        ; If this is the same as before purge, scream loudly!
        push    esi
        call    GN_GetFreeBlockSizeForMode
        mov     dwOldOffscreenFreeSpace,eax

        ; Purge DX video memory
        ; Specifically call the HWBOARD routine so that it
        ; can do stuff that is global and crucial to the
        ; board.
        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[eax].GNBOARDCX.pHWBoardCX
        call    HW_DxHeapPurge

        ; Send a broadcast message around that video memory
        ; has been reset. Everyone can realloc stuff.

        ; Get the largest block size again
        ; If this is the same as before purge, scream loudly!
        push    esi
        call    GN_GetFreeBlockSizeForMode
        cmp     eax,dwOldOffscreenFreeSpace
        jg      DoneIt

        ; There is not enough memory to set the requested mode, and purging
        ; DX video memory didn't help. Now, purge everything except the
        ; primary surface which was purged earlier. After we purge everything,
        ; we will need to purge the cursor cache.
        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[eax].GNBOARDCX.pHWBoardCX
        call    HW_HeapPurgeAllExceptPrimarySurface
        
        ; Clear all cursor cahces.
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        call    GN_ClearAllCursorCaches
        
        ; Get the largest block size again
        ; If this is the same as before purge, scream loudly!
        push    esi
        call    GN_GetFreeBlockSizeForMode
        cmp     eax,dwOldOffscreenFreeSpace
        jg      DoneIt

        ; At this point we might not realloc the secondary surface.
        ; Return E31 to force it.
        or      eax,80000000H

DoneIt:
        POPR    esi
CLOSEPROC



;==============================================================================
;
; Function:     GN_DoFullStateChange
;
; Purpose:      This routine is called when the information in the
;               NVTWINVIEW_DISPAY_CONFIG buffer in the GNLOGDEVCX
;               is to be processed. This may happen at escape time
;               or it may be deferred until the next modeset.
;
; Arguments:
;               ds                      CXSel
;               pGNLogdevCXOffset       GNLOGDEVCX offset
;
; Returns:      eax                     New size of free offscreen memory
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_DoFullStateChange, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
PARMD   lpPassedInModeInfo
LOCALD  dwMaxMonitors
LOCALD  dwMonitorNum
LOCALD  dwLocalDeviceMask
LOCALD  dwDeviceCount
LOCALV  sModeInfo,MODEINFO
LOCALD  lpModeInfo
LOCALV  sDesktopModeInfo,MODEINFO
LOCALD  lpDesktopModeInfo
LOCALS  LocalDevData,MAX_ATTACHED_CRTCS*4
LOCALD  lpLocalDevData
OPENPROC
        PUSHR   esi,edi

        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        mov     lpModeInfo,eax
        lea     ax,sDesktopModeInfo
        mov     lpDesktopModeInfo,eax
        lea     ax,LocalDevData
        mov     lpLocalDevData,eax

        mov     esi,pGNLogdevCXOffset
        lea     edi,[esi].GNLOGDEVCX.sDispConfig

        ; If we are going to standard mode then destroy any CLONE CRTCCX
        ; if one exists
        cmp     ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwActiveDevices,1
        jne     @F
        mov     dwMaxMonitors,1
        push    esi
        pushd   1
        call    GN_LogdevDetachCrtc
        jmp     DoneDevice
@@:
        ; If we are going to clone mode and we are currently in
        ; standard mode then add a clone
        cmp     ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwActiveDevices,3
        jne     DoneDevice
        cmp     ds:[esi].GNLOGDEVCX.pGNCrtcCX[4],0
        jne     DoneDevice
        mov     dwMaxMonitors,2
        push    esi
        call    GN_LogdevAddAClone

DoneDevice:

        ; Loop on all the modes.
        mov     dwMonitorNum,0

        ; We have to do the primary device first. But then we will want
        ; to skip whiever one it was later, so remove it from the mask.
        mov     eax,ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwDeviceMask
        mov     dwLocalDeviceMask,eax
        mov     eax,ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwPrimaryDeviceMask
        xor     dwLocalDeviceMask,eax
        mov     edx,eax

        sub     ecx,ecx
        mov     dwDeviceCount,ecx
@@:     shr     edx,1
        je      Merge1
        inc     ecx
        mov     dwDeviceCount,ecx
        jmp     @B


LoopNow1:
        ; Does this guy have a mode to set?
        mov     ecx,dwDeviceCount
        mov     eax,1
        shl     eax,cl

        test    eax,dwLocalDeviceMask
        jz      SkipInitNoDev

Merge1:
        PUSHR   eax

        mov     ebx,ecx
        shr     cl,3
        and     ebx,07H
        mov     ch,bl
        mov     dword ptr sModeInfo.MIDevData,ecx

        ; Get the TV Format from the ResMan.
        mov     ebx,dwMonitorNum
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4]
        push    ds:[ebx].GNCRTCCX.pHWCrtcCX
        push    ecx
        call    HW_GetResManTvFormat
        
        mov     sModeInfo.MIDevData.cFormat,al

        ; Now change the device type in the CRTCCX
        mov     ecx,dwMonitorNum
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX[ecx * 4]
        push    dword ptr sModeInfo.MIDevData
        call    GN_SetDevData

        POPR    eax
        test    eax,ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwPhysicalModeFlags
        jz      SkipInit

        mov     ecx,dwDeviceCount
        imul    ecx,SIZE PHYSICAL_MODE
        lea     eax,[edi].NVTWINVIEW_DISPLAY_CONFIG.PhysModes
        add     eax,ecx
        push    eax
        
        ; Figure out the EDID thing
        mov     ebx,dwMonitorNum
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4]
        mov     ds:[ebx].GNCRTCCX.dwEdidSize,0
        cmp     sModeInfo.MIDevData.cType,DEVTYPE_TV
        je      DoneEdidRead

        ; Read the EDID for this device
        mov     ds:[ebx].GNCRTCCX.dwEdidSize,MAX_EDID_BUFFER_SIZE
        push    ds:[ebx].GNCRTCCX.pHWCrtcCX
        push    dword ptr sModeInfo.MIDevData
        mov     ax,ds
        push    eax
        call    GN_GetSelectorBase
        lea     ecx,ds:[ebx].GNCRTCCX.sEdidBuffer
        add     ecx,eax
        push    ecx
        lea     ecx,ds:[ebx].GNCRTCCX.dwEdidSize
        add     ecx,eax
        push    ecx
        call    HW_ReadEdid

DoneEdidRead:
        ; Assume that this is a normal mode
        pop     eax
        mov     ebx,ds:[eax].MODEINFO.dwXRes
        mov     ecx,ds:[eax].MODEINFO.dwYRes
        mov     edx,ds:[eax].MODEINFO.dwRefreshRate
        or      ebx,ebx
        jne     UseTwinViewMode

        ; Special max device mode when dwXRes, dwYRes, and dwRefreshRate = 0.
        ; First, check to see if this is a Toshiba mobile system. If so, use
        ; the refresh rate out of the registry unless the monitor is
        ; non-DDC (no EDID) in which case use 60Hz. If this is not a
        ; Toshiba mobile system, use the largest dwXRes, dwYRes, and
        ; dwRefreshRate allowable for the current desktop.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetBootDeviceSelection,es,ax
        mov     edx,1000
        cmp     ax,2
        jne     UseRefreshRate
        
        ; Check to see if there is an EDID, and if not, use 60Hz.
        mov     ebx,dwMonitorNum
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4]
        cmp     ds:[ebx].GNCRTCCX.dwEdidSize,0
        mov     edx,60
        je      UseRefreshRate

        ; Get the refresh rate from the registry.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    lpModeInfo
        CALLFAR16IND lpfnGetDeviceDisplayMode,es,ax
        sub     ebx,ebx
        lea     bx,sModeInfo
        mov     edx,ss:[ebx].MODEINFO.dwRefreshRate
        or      ax,ax
        jne     UseRefreshRate

        ; No mode in the registry, use 60Hz as default.
        mov     edx,60

UseRefreshRate:
        mov     ebx,7FFFH
        mov     ecx,ebx
        
UseTwinViewMode:
        sub     eax,eax
        lfs     ax,lpPassedInModeInfo
        cmp     ebx,fs:[eax].MODEINFO.dwXRes
        jl      @F
        mov     ebx,fs:[eax].MODEINFO.dwXRes
@@:     
        mov     sModeInfo.dwXRes,ebx
        
        cmp     ecx,fs:[eax].MODEINFO.dwYRes
        jl      @F
        mov     ecx,fs:[eax].MODEINFO.dwYRes
@@:     
        mov     sModeInfo.dwYRes,ecx
        mov     sModeInfo.dwRefreshRate,edx
        mov     eax,fs:[eax].MODEINFO.dwBpp
        mov     sModeInfo.dwBpp,eax

        ; NOTE: At this point, the DISPDATA.dwMaxDacSpeedInHertzXXbpp
        ; structure members are stale, but since we don't have a head
        ; mapping yet, we can not get the real values for the DAC.
        ; Set this to a large number so we won't restrict now according
        ; to the DAC speed. The DAC speed will be filled in after we
        ; have a head mapping, and the mode will be revalidated using
        ; the correct DAC speed. This only needs to be done for the
        ; primary as GN_LogdevAddAClone already does this for the secondary.
        mov     edx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        lea     edx,[edx].GNCRTCCX.sNvmodeDispData
        mov     ds:[edx].DISPDATA.dwMaxDacSpeedInHertz8bpp,40000000H
        mov     ds:[edx].DISPDATA.dwMaxDacSpeedInHertz16bpp,40000000H
        mov     ds:[edx].DISPDATA.dwMaxDacSpeedInHertz32bpp,40000000H

        mov     ecx,dwMonitorNum
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX[ecx * 4]
        push    lpModeInfo
        call    GN_AssignCrtcModeOnlyBackOffSpatial

        ; We should now have a mode that is settable on this device.
        ; Let's write this out to the registry
        mov     ecx,dwMonitorNum
        sub     eax,eax
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[ecx * 4]
        push    eax
        call    GN_CrtcWriteModeToReg

SkipInit:
        ; Go to the next monitor
        inc     dwMonitorNum
        mov     ecx,dwMonitorNum
        cmp     ecx,dwMaxMonitors
        jnc     DoneInit

        cmp     ecx,1
        jne     SkipInitNoDev
        ; We just did MonitorNum = 0 which is the primary.
        ; Start over with the beginning of the deviceMask.
        ; The primary won't be repeated -- we reomved it from
        ; the deviceMask at the beginning.
        mov     dwDeviceCount,-1

SkipInitNoDev:
        inc     dwDeviceCount
        cmp     dwDeviceCount,18H
        jc      LoopNow1

DoneInit:
        POPR    esi,edi
CLOSEPROC

CLOSESEG _NVTEXT16

END

