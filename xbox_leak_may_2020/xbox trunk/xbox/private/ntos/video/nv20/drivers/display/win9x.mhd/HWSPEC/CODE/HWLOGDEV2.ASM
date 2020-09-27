;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwlogdev2.asm
;
; Purpose:      This file contains all the hardware specific
;               logical DEVICE specific run-time routines.
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc
include gngdi.inc
include hwgdi.inc
include files.inc
include hwmacs.inc
include nvcm.inc
include nvreg.inc
include dxshare.inc
include escape.inc
include ..\..\..\..\..\sdk\nvidia\inc\nvrmarch.inc
include ..\..\..\..\..\sdk\nvidia\inc\rddapi.inc

OPENSEG  _DATA, USE16, DATA
TempStack       DB 400H DUP(0)
TempStackTop    DB 10H  DUP(0)
dwOldSS         DD      0
dwOldEsp        DD      0
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE
PUBLIC  szDDrawBinaryName, szDDrawEntryPointName, szOGLRegKeyName
szDDrawBinaryName       DB      NV4_DDRAW_BINARY,0
szDDrawEntryPointName   DB      "DriverInit",0
szOGLRegKeyName         DB      "RIVATNT",0

;==============================================================================
;
; Function:     HW_LogdevMessage
;
; Purpose:      This routine gets called whenver there is an
;               message to send to all objects. There are
;               a few different messages.  See gngdi.inc
;               for details.
;
; Arguments:
;               ds                      Context Selector
;               pHWLogdevCXOffset               HWCRTCCX ptr
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
DECPROC HW_LogdevMessage, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD   dwMessage
PARMD   dwData1
PARMD   dwData2
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWLogdevCXOffset

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
        cmp     eax,MSG_LOGDEVMOVED
        je      logdevMoved
        cmp     eax,MSG_DEVICESREMAPPED
        je      DevRemap
        cmp     eax,MSG_DISPLAY_DRIVER_UNLOADING
        je      DispUnload
        jmp     DoneIt

PreMS:
        cmp     ds:[esi].HWLOGDEVCX.dwDirectXActive,0
        je      DoneIt

        ; Tell DDRAW we are going to switch the mode.
        les     di,ds:[esi].HWLOGDEVCX.lpDirectXSharedData
        movzx   edi,di
        push    esi
        push    es:[edi].DISPDRVDIRECTXCOMMON.dwFlatCodeSel
        push    es:[edi].DISPDRVDIRECTXCOMMON.pfnInterfaceFunc
        pushd   GDIDX_PRE_MODESET_NOTIFY
        pushd   0
        call    DirectCallToFlatland
        jmp     DoneIt

PostMS:
        push    pHWLogdevCXOffset
        push    dwData1
        call    NV_PostModeset

        ; We are no longer initting after the first modeset
        mov     ds:[esi].HWLOGDEVCX.dwInitting,0

        jmp     DoneIt

DXEnum:
        ; Init the shared Data structure and inform DirectX
        ; of our 16 bit capabilities
        push    pHWLogdevCXOffset
        pushd   0
        CALLFAR16 NV_CreateDrvObj
        jmp     DoneIt

DXOn:
        ; DirectX is coming up
        mov     ds:[esi].HWLOGDEVCX.dwDirectXActive,1
        jmp     DoneIt

DXOff:
        mov     ds:[esi].HWLOGDEVCX.dwDirectXActive,0
        jmp     DoneIt

DosOff:
PowerOn:
        les     di,ds:[esi].HWLOGDEVCX.lpDirectXSharedData
        movzx   edi,di
        push    esi
        push    es:[edi].DISPDRVDIRECTXCOMMON.dwFlatCodeSel
        push    es:[edi].DISPDRVDIRECTXCOMMON.pfnInterfaceFunc
        pushd   GDIDX_EXIT_FULL_SCREEN_DOS
        pushd   0
        call    DirectCallToFlatland
        jmp     DoneIt

DosOn:
PowerOff:
        les     di,ds:[esi].HWLOGDEVCX.lpDirectXSharedData
        movzx   edi,di
        push    esi
        push    es:[edi].DISPDRVDIRECTXCOMMON.dwFlatCodeSel
        push    es:[edi].DISPDRVDIRECTXCOMMON.pfnInterfaceFunc
        pushd   GDIDX_ENTER_FULL_SCREEN_DOS
        pushd   0
        call    DirectCallToFlatland
        jmp     DoneIt

DrvDis:
        jmp     DoneIt

logdevMoved:
        push    esi
        push    dwData1
        call    NV_PostModeset
        jmp     DoneIt

DevRemap:
        push    esi
        call    NV_DevicesRemapped
        jmp     DoneIt

DispUnload:
        les     di,ds:[esi].HWLOGDEVCX.lpDirectXSharedData
        movzx   edi,di
        push    esi
        push    es:[edi].DISPDRVDIRECTXCOMMON.dwFlatCodeSel
        push    es:[edi].DISPDRVDIRECTXCOMMON.pfnInterfaceFunc
        pushd   GDIDX_DRIVER_UNLOADING
        pushd   0
        call    DirectCallToFlatland
        jmp     DoneIt

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:	HW_DriverTwinviewStateChanging
;
; Purpose:	This is called when the twinview state is changing
;		We may be going in or out of clone mode or in or out
;		of multimon mode.
;		Right now we are only calling this when going in or
;		out of muti-mon to inform the DX driver so they can
;		change resource allocation.
;
; Arguments:
;               pHWLogdevCXOffsetPrimary
;               pHWLogdevCXOffsetSecondary
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_DriverTwinviewStateChanging, PASCAL, FRAME, FAR16
PARMD	pHWLogdevCXOffsetPrimary
PARMD	pHWLogdevCXOffsetSecondary
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWLogdevCXOffsetPrimary

        les     di,ds:[esi].HWLOGDEVCX.lpDirectXSharedData
        movzx   edi,di
        push    esi
        push    es:[edi].DISPDRVDIRECTXCOMMON.dwFlatCodeSel
        push    es:[edi].DISPDRVDIRECTXCOMMON.pfnInterfaceFunc
        pushd   GDIDX_EARLY_DX_CONNECT
        pushd   0
        call    DirectCallToFlatland

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_DevicesRemapped
;
; Purpose:      This routine gets called to update info when the devices
;               get remapped.
;
; Arguments:
;               pHWLogdevCXOffset
;               pDibeng
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_DevicesRemapped, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
LOCALD  dwNumCRTCs
LOCALD  dwCountCRTCs
OPENPROC
        PUSHR   esi
        mov     esi,pHWLogdevCXOffset
        mov     ebx,ds:[esi].HWLOGDEVCX.pDirectXSharedData

        ; We need to get the correct head number for this DX share
        ; structure. We can peek at our HWCRTCCXs. If we have more
        ; than 1 HWCRTCCX attached, then we must be in clone mode in
        ; which case, we can set head 0. If we only have 1 HWCRTCCX
        ; attached, then use the PhysicalCRTCInstance from it.
        lea     eax,ds:[esi].HWLOGDEVCX.pHWCrtcCX
        mov     dwCountCRTCs,MAX_ATTACHED_CRTCS
        mov     dwNumCRTCs,0

        ; Keep head number for the video mirror device.
        mov     ecx,ds:[ebx].DISPDRVDIRECTXCOMMON.dwHeadNumber
BackIt:
        mov     edx,ds:[eax]
        or      edx,edx
        je      @F
        mov     ecx,ds:[edx].HWCRTCCX.dwPhysicalCRTCInstance
        inc     dwNumCRTCs
@@:     add     eax,4
        dec     dwCountCRTCs
        jne     BackIt

        ; More than 1 CRTC implies Clone mode which means Physical
        ; Instance 0.
        cmp     dwNumCRTCs,1
        jbe     @F
        sub     ecx,ecx
@@:     mov     ds:[ebx].DISPDRVDIRECTXCOMMON.dwHeadNumber,ecx

        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     NV_PostModeset
;
; Purpose:      This routine gets called right after a modeset from the
;               HW_LogdevMessage routine.
;
; Arguments:
;               pHWLogdevCXOffset
;               pDibeng
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_PostModeset, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD   pDibeng
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWLogdevCXOffset
        mov     edi,pDibeng

        ; Let the RM know the desktop mode.
        mov     ebx,ds:[esi].HWLOGDEVCX.pHWBoardCX
        pushd   NV_CFG_SCREEN_WIDTH
        movzx   eax,ds:[edi].DIBENGINEHDR.deWidth
        push    eax
        push    ds:[ebx].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigSet,es,ax

        mov     ebx,ds:[esi].HWLOGDEVCX.pHWBoardCX
        pushd   NV_CFG_SCREEN_HEIGHT
        movzx   eax,ds:[edi].DIBENGINEHDR.deHeight
        push    eax
        push    ds:[ebx].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigSet,es,ax

        mov     ebx,ds:[esi].HWLOGDEVCX.pHWBoardCX
        pushd   NV_CFG_PIXEL_DEPTH
        movzx   eax,ds:[edi].DIBENGINEHDR.deBitsPixel
        push    eax
        push    ds:[ebx].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigSet,es,ax

        ; Fill in a few fields for the shared data structure
        ; that are changeable or that we coiuldn't know at
        ; HW_CreateLogdevConetext time.
        movzx   ecx,ds:[edi].DIBENGINEHDR.deBitsSelector
        push    ecx
        call    GN_GetSelectorBase
        mov     ebx,ds:[esi].HWLOGDEVCX.pDirectXSharedData
        mov     ds:[ebx].DISPDRVDIRECTXCOMMON.dwScreenLinearBase,eax
        mov     ds:[ebx].DISPDRVDIRECTXCOMMON.pFlatDibeng,edi

        ; Let the DD driver object know about the modeset
        cmp     ds:[esi].HWLOGDEVCX.dwDirectXActive,0
        je      @F
        push    pHWLogdevCXOffset
        pushd   1
        CALLFAR16 NV_CreateDrvObj
@@:
        ; Start this off at -1 after a mode switch. It is used for
        ; temporary storage of the last value written into the
        ; ContextPattern.SetMonochromeColor0 method. -1 is an OK
        ; value to have because it means the alpha is fully on.
        mov     ds:[esi].HWLOGDEVCX.dwPatternAlpha0,-1
        mov     ds:[esi].HWLOGDEVCX.dwPatternAlpha1,-1

        ; Figure out all the color depth specific object values
        mov     ax,SEG wNVTEXT32_Alias_In_NVTEXT16
        mov     es,ax
        mov     es,es:[wNVTEXT32_Alias_In_NVTEXT16]
        mov     al,ds:[edi].DIBENGINEHDR.bRealBpp
        cmp     al,20H
        je      Is32
        cmp     al,0FH
        je      Is15
        ja      Is16

Is8:
        mov     es:[dwContextSurface2DColorFormat],NV042_SET_COLOR_FORMAT_LE_Y8
        mov     ds:[esi].HWLOGDEVCX.dwRectAndTextColorFormat,NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwIndexedImageColorFormat,NV060_SET_COLOR_FORMAT_LE_X8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwImageFromCpuColorFormat,NV061_SET_COLOR_FORMAT_LE_X8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwSolidLinColorFormat,NV05C_SET_COLOR_FORMAT_LE_X8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwColorAlphaMask,0FF00H
        mov     ds:[esi].HWLOGDEVCX.dwPatternColorFormat,NV044_SET_COLOR_FORMAT_LE_A8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwColorPatternAndHeaderSize,40H + 20H
        mov     ds:[esi].HWLOGDEVCX.dwColorPatternMethodOffset,CHNRUN(ContextPattern.SetPatternY8,10H)
        jmp     DoneCD

Is15:
        mov     es:[dwContextSurface2DColorFormat],NV042_SET_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5
        mov     ds:[esi].HWLOGDEVCX.dwRectAndTextColorFormat,NV04A_SET_COLOR_FORMAT_LE_X17R5G5B5
        mov     ds:[esi].HWLOGDEVCX.dwIndexedImageColorFormat,NV060_SET_COLOR_FORMAT_LE_X1R5G5B5
        mov     ds:[esi].HWLOGDEVCX.dwImageFromCpuColorFormat,NV061_SET_COLOR_FORMAT_LE_X1R5G5B5
        mov     ds:[esi].HWLOGDEVCX.dwSolidLinColorFormat,NV05C_SET_COLOR_FORMAT_LE_X17R5G5B5
        mov     ds:[esi].HWLOGDEVCX.dwColorAlphaMask,8000H
        mov     ds:[esi].HWLOGDEVCX.dwPatternColorFormat,NV044_SET_COLOR_FORMAT_LE_X16A1R5G5B5
        mov     ds:[esi].HWLOGDEVCX.dwColorPatternAndHeaderSize,80H + 20H
        mov     ds:[esi].HWLOGDEVCX.dwColorPatternMethodOffset,CHNRUN(ContextPattern.SetPatternX1R5G5B5,20H)
        jmp     DoneCD

Is16:
        mov     es:[dwContextSurface2DColorFormat],NV042_SET_COLOR_FORMAT_LE_R5G6B5
        mov     ds:[esi].HWLOGDEVCX.dwRectAndTextColorFormat,NV04A_SET_COLOR_FORMAT_LE_X16R5G6B5
        mov     ds:[esi].HWLOGDEVCX.dwIndexedImageColorFormat,NV060_SET_COLOR_FORMAT_LE_R5G6B5
        mov     ds:[esi].HWLOGDEVCX.dwImageFromCpuColorFormat,NV061_SET_COLOR_FORMAT_LE_R5G6B5
        mov     ds:[esi].HWLOGDEVCX.dwSolidLinColorFormat,NV05C_SET_COLOR_FORMAT_LE_X16R5G6B5
        mov     ds:[esi].HWLOGDEVCX.dwColorAlphaMask,0FFFF0000H
        mov     ds:[esi].HWLOGDEVCX.dwPatternColorFormat,NV044_SET_COLOR_FORMAT_LE_A16R5G6B5
        mov     ds:[esi].HWLOGDEVCX.dwColorPatternAndHeaderSize,80H + 20H
        mov     ds:[esi].HWLOGDEVCX.dwColorPatternMethodOffset,CHNRUN(ContextPattern.SetPatternR5G6B5,20H)
        jmp     DoneCD

Is32:
        mov     es:[dwContextSurface2DColorFormat],NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwRectAndTextColorFormat,NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwIndexedImageColorFormat,NV060_SET_COLOR_FORMAT_LE_X8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwImageFromCpuColorFormat,NV061_SET_COLOR_FORMAT_LE_X8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwSolidLinColorFormat,NV05C_SET_COLOR_FORMAT_LE_X8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwColorAlphaMask,0FF000000H
        mov     ds:[esi].HWLOGDEVCX.dwPatternColorFormat,NV044_SET_COLOR_FORMAT_LE_A8R8G8B8
        mov     ds:[esi].HWLOGDEVCX.dwColorPatternAndHeaderSize,100H + 20H
        mov     ds:[esi].HWLOGDEVCX.dwColorPatternMethodOffset,CHNRUN(ContextPattern.SetPatternX8R8G8B8,40H)
        jmp     DoneCD

DoneCD:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_AllocLogicalSurface
;
; Purpose:      This routine gets called to free the logical display
;               surface.
;
; Arguments:
;               ds                              Context Selector
;               pHWLogdevCXOffset               HWLOGDEVCX ptr
;               lpDibeng                        Describes logical surface
; Returns:
;               eax     offset into video memory of start of offscreen block
;                       (-1 if failure)
;               ebx     length in bytes of offscreen block
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_AllocLogicalSurface, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD   lpDibeng
OPENPROC
        mov     ebx,pHWLogdevCXOffset
        or      ebx,ebx
        je      DoneIt

        mov     ecx,lpDibeng
        push    ds:[ebx].HWLOGDEVCX.pHWBoardCX
        push    ds:[ecx].DIBENGINE.deDeltaScan
        movzx   eax,ds:[ecx].DIBENGINE.deHeight
        push    eax
        push    ds:[ebx].HWLOGDEVCX.pHWCrtcCX
        call    HW_GetPhysicalCrtcIndex
        push    eax
        call    HW_AllocPrimarySurface
        cmp     eax,-1
        je	DoneIt
	push	ebx
        mov     ebx,pHWLogdevCXOffset
        mov     ds:[ebx].HWLOGDEVCX.dwLogicalSurfaceOffset,eax
	pop	ebx
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:     HW_FreeLogicalSurface
;
; Purpose:      This routine gets called to free the logical display
;               surface.
;
; Arguments:
;               pHWLogdevCXOffset
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_FreeLogicalSurface, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
OPENPROC
        mov     ebx,pHWLogdevCXOffset
        or      ebx,ebx
        je      @F
        push    ds:[ebx].HWLOGDEVCX.pHWBoardCX
        push    ds:[ebx].HWLOGDEVCX.dwLogicalSurfaceOffset
        call    HW_FreePrimarySurface
@@:
CLOSEPROC


;==============================================================================
;
; Function:     NV_CreateDrvObj
;
; Purpose:      This is the hardware specific CreateDriverObject
;               routine. You must fill in the "16bit portion" of the
;               HALInfo structure. It is not well documented what
;               fields exactly must be filled in here vs. which you
;               can put off and fill in when you get to the DirectX
;               driver. You will just have to look at the sample
;               Microsoft code to figure it out.
;
;               After the 16bit portion if filled out, you must call
;               the lpSetInfo routine with the apprpriate parameters
;               like this:
;               (*lpSetInfo) ((DDHALINFO FAR *)lpDDHalInfo, (WORD)dwReason);
;
;               The generic code also calls this routine whenever the
;               DDCREATEDRIVEROBJECT escape is received. This means that
;               the DirectX driver is coming up for use.
;
;               This routine is also called by the hardware specific
;               code itself when a modeset has occurred.
;
;               In both cases, before getting here, the generic code
;               has flushed all offscreen bitmaps. When NV_CreateDrvObj
;               returns, the generic code will re-init the offscreen
;               memory manager based upon the variables returned.
;
;               This gives the hardware specific code the chance to change
;               the amount of offscreen memory used for offscreen bitmaps
;               when DirectX starts up. By setting dwOffscreenLength
;               to 0, the HW specific code can eliminate offscreen
;               bitmap allocation while DirectX is active.
;
; Arguments:
;               ds                      CXSel
;               pHWLogdevCXOffset       HWLOGDEVCX
;               dwReason        non-0   then the call to this routine is
;                                       because the mode has changed and
;                                       the DrvObj needs to be updated to
;                                       reflect the new mode.
;                               0       then the call to this routine is
;                                       because the DDCREATEDRIVEROBJECT
;                                       escape was received by the generic
;                                       code.
;
; Returns:      eax     0       then driver object not created. This
;                               probably means that we have not connected
;                               with the 32bit DirectX driver yet.
;                       hModule the HINSTANCE handle of the DirectX driver
;                               which was passed into its DllMain procedure.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_CreateDrvObj, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD   dwReason
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWLogdevCXOffset
        mov     edi,ds:[esi].HWLOGDEVCX.pDirectXSharedData

        ; If we don't have any DirectX context data, then we probably
        ; didn't connect with the DirectX driver, or we did, but there
        ; was an init failure.
        cmp     ds:[edi].DISPDRVDIRECTXCOMMON.pDirectXData,0
        je      Done

        ; If we don't have the ptr to the pfnInterfaceFunc routine which
        ; is now located in the DirectX driver, then we can't call it.
        cmp     ds:[edi].DISPDRVDIRECTXCOMMON.pfnInterfaceFunc,0
        je      Done

        ; I don't know if this will ever happen, but I was worried
        ; because of some new code paths that have been added to both
        ; display and DX driver, so it won't hurt to check this here.
        cmp     ds:[esi].HWLOGDEVCX.sDDHalDDrawFns.lpSetInfo,0
        je      Done

        ; If we get here, then the DirectX driver must be loaded and
        ; DriverInit must have been called, because that is where the
        ; pDirectXData and pfnInterfaceFunc fields get filled in.

        ; Now do a Direct call into the DirectX driver so it can init
        ; the "16bit portion" of the DDHALINFO structure.
        mov     ebx,ds:[esi].HWLOGDEVCX.pDirectXSharedData
        push    esi
        push    ds:[edi].DISPDRVDIRECTXCOMMON.dwFlatCodeSel
        push    ds:[edi].DISPDRVDIRECTXCOMMON.pfnInterfaceFunc
        pushd   GDIDX_PRE_SETINFOINIT
        pushd   0
        call    DirectCallToFlatland

        ; One last conversion -- the ddraw16.dll SetInfo routine needs
        ; a 16:16 ptr to the DDHALInfo structure and ours is currently
        ; a flat32 pointer.
        mov     ebx,ds:[esi].HWLOGDEVCX.pDirectXSharedData
        mov     eax,1
        mov     ebx,ds:[edi].DISPDRVDIRECTXCOMMON.pHalInfo
        mov     ecx,ds:[edi].DISPDRVDIRECTXCOMMON.dwHALInfoSize
        call    GN_AllocSelectorAndMapIt

        ; Save the selector so we can restore it later.
        push    eax

        ; Now call the ddraw16.dll SetInfo routine
        shl     eax,10H
        push    eax
        push    word ptr dwReason
        call    FFP16 PTR ds:[esi].HWLOGDEVCX.sDDHalDDrawFns.lpSetInfo

        ; Free the selector used to map the DDHALInfo structure
        pop     eax
        call    GN_UnmapAndFreeSelector

SkipIt:
        ; Return the hInstance for the DirectX driver
        mov     eax,ds:[edi].DISPDRVDIRECTXCOMMON.hDirectXInstance

Done:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     DirectCallToFlatland
;
; Purpose:      This procedure calls the far routine passed in. The
;               far routine must be in code that wants a USE32 stack
;               and assumes a flat space (ds = es = ss).
;               We map the current stack offset to a 32bit stack and
;               set ds, es, and ss to the flat data selector.
;
; Arguments:
;               ds                      CXSel
;               pHWLogdevCXOffset       HWLOGDEVCX
;               dwFarCallCS             cs:eip of call dst
;               dwFarCallOffset
;               dwInterfaceID           GDIDX_XXXX in dxshare.h
;               dwExtraData             parameter to pass up
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DirectCallToFlatland, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD   dwFarCallCS
PARMD   dwFarCallOffset
PARMD   dwInterfaceID
PARMD   dwExtraData
LOCALD  pfnSharedData
LOCALD  dwFlatDataSelector
OPENPROC
        PUSHR   ds,esi,edi
        mov     esi,pHWLogdevCXOffset

        mov     edi,ds:[esi].HWLOGDEVCX.pDirectXSharedData
        mov     pfnSharedData,edi
        mov     ecx,ds:[edi].DISPDRVDIRECTXCOMMON.dwFlatDataSel
        mov     dwFlatDataSelector,ecx

        ; If we don't have any DirectX context data, then we probably
        ; didn't connect with the DirectX driver, or we did, but there
        ; was an init failure.
        cmp     ds:[edi].DISPDRVDIRECTXCOMMON.pDirectXData,0
        je      Done

        ; If we don't have the ptr to the pfnInterfaceFunc routine which
        ; is now located in the DirectX driver, then we can't call it.
        cmp     ds:[edi].DISPDRVDIRECTXCOMMON.pfnInterfaceFunc,0
        je      Done

        ; Push the return address on the stack (this is a FAR32)
        sub     eax,eax
        mov     ax,SEG ComeBack
        push    eax
        mov     eax,OFFSET ComeBack
        push    eax

        ; Push the call address on the stack
        push    dwFarCallCS
        push    dwFarCallOffset

        ; Get the linear base address of the current ss and add to sp
        mov     ax,ss
        push    eax
        call    GN_GetSelectorBase
        movzx   edx,sp
        add     edx,eax

        ; Save the old ss and sp in registers that won't get trashed
        ; It's a bit tricky because, the "saved sp" should actually
        ; be the value of sp before pushing the return and calling
        ; addresses above. Why? Because when the addresses are popped
        ; off (via return calls), these returns are made with the
        ; other stack pointer!
        mov     di,ss
        mov     si,sp
        add     si,10H

        ; Load up the flat data segments
        mov     eax,dwFlatDataSelector
        mov     ds,ax
        mov     es,ax

        ; We need to pass these in
        mov     eax,pfnSharedData
        mov     ebx,dwInterfaceID
        mov     ecx,dwExtraData

        ; Load up the new ss and sp. The processor inhibits interrupts
        ; for an additional instruction after loading ss to give you a
        ; chance to load sp as well and prevent non-coherency.
        push    ds
        pop     ss
        mov     esp,edx
        RETFAR32     ; this far return will "call" the pfnInterfaceFunc func

ComeBack:
        ; We still are on the 32bit flat stack. Change back to our stack
        mov     ss,di
        mov     sp,si
Done:

        ; Restore the registers
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetDirectXRuntimeVersion
;
; Purpose:      This routine returns the DD runtime version.
;
; Arguments:
;               pHWLogdevCXOffset       HWLOGDEVCX
;               dwOSDirectXVersion      OS version of direct X
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetDirectXRuntimeVersion, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD   dwOSDirectXVersion
OPENPROC
        ; If we get asked what version DirectX the driver was built for,
        ; then we will also inform the DirectX driver what version of DX
        ; the OS is running.
        mov     edx,pHWLogdevCXOffset
        les     bx,ds:[edx].HWLOGDEVCX.lpDirectXSharedData
        movzx   ebx,bx
        push    edx
        push    es:[ebx].DISPDRVDIRECTXCOMMON.dwFlatCodeSel
        push    es:[ebx].DISPDRVDIRECTXCOMMON.pfnInterfaceFunc
        pushd   GDIDX_DIRECTX_OS_VERSION
        push    dwOSDirectXVersion
        call    DirectCallToFlatland

        mov     eax,DD_RUNTIME_VERSION
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetDirectXDllNameAndEntryPoint
;
; Purpose:      This routine returns the names of the DirectX DLL
;               and the entry point name.
;
; Arguments:
;               ds                      Context selector
;               pHWLogdevCXOffset       offset to HWLOGDEVCX
;               dwDllNameSel            Selector of where to put dll name
;               dwDllNameOffset         Offset of where to put dll name
;               dwEntryPointNameSel     Selector of where to put entry
;                                       point name
;               dwEntryPointNameOffset  Offset of where to put entry
;                                       point name
;
; Returns:      eax     Ptr to DirectX Context Data (must be 16:16 ptr)
;
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetDirectXDllNameAndEntryPoint, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD   dwDllNameSel
PARMD   dwDllNameOffset
PARMD   dwEntryPointNameSel
PARMD   dwEntryPointNameOffset
OPENPROC
        PUSHR   ds,esi,edi

        ; Copy the DLL Name
        mov     es,dwDllNameSel
        mov     edi,dwDllNameOffset
        mov     ax,SEG szDDrawBinaryName
        mov     fs,ax
        lea     esi,szDDrawBinaryName
@@:
        mov     ax,fs:[esi]
        inc     esi
        mov     es:[edi],al
        inc     edi
        or      al,al
        jne     @B

        ; Copy the DLL Entry Point Name
        mov     es,dwEntryPointNameSel
        mov     edi,dwEntryPointNameOffset
        mov     ax,SEG szDDrawEntryPointName
        mov     fs,ax
        lea     esi,szDDrawEntryPointName
@@:
        mov     ax,fs:[esi]
        inc     esi
        mov     es:[edi],al
        inc     edi
        or      al,al
        jne     @B

        ; Return a 16:16 ptr to the DirectX shared data
        mov     esi,pHWLogdevCXOffset
        mov     eax,ds:[esi].HWLOGDEVCX.pDirectXSharedData

        POPR    ds,esi,edi
CLOSEPROC



;==============================================================================
;
; Function:     HW_SetNewDirxCallbacks
;
; Purpose:      This routine tells the ardware what the addresses
;               of the DirectX callback functions are.
;
; Arguments:
;               ds                      Context selector
;               pHWLogdevCXOffset       offset to HWLOGDEVCX
;               lpDDHalDDrawFuncs       16:16 ptr to DDraw callbacks
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_SetNewDirxCallbacks, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD   lpDDHalDDrawFuncs
OPENPROC
        PUSHR   ds,esi,edi
        PUSHR   ds
        POPR    es
        mov     edi,pHWLogdevCXOffset
        lea     edi,[edi].HWLOGDEVCX.sDDHalDDrawFns

        sub     esi,esi
        lds     si,lpDDHalDDrawFuncs

        pushf
        cld
        mov     ecx,(SIZE MYDDHALDDRAWFNS) / 4
        rep     movs dword ptr es:[edi], ds:[esi]
        popf

        POPR    ds,esi,edi
CLOSEPROC



;==============================================================================
;
; Function:     HW_GetDirectXInstance
;
; Purpose:      This routine returns the instance handle of the
;               DirectX dll.
;
; Arguments:
;               ds                      Context selector
;               pHWLogdevCXOffset       offset to HWLOGDEVCX
;
; Returns:
;               eax             DirectX Driver instance handle.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetDirectXInstance, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pHWLogdevCXOffset
        mov     eax,ds:[esi].HWLOGDEVCX.pDirectXSharedData
        mov     eax,ds:[eax].DISPDRVDIRECTXCOMMON.hDirectXInstance
        POPR    esi
CLOSEPROC



;==============================================================================
;
; Function:     HW_GetOpenGLRegistryName
;
; Purpose:      This routine returns the names of the OpenGL
;               Registry subkey that should be used by the system.
;
; Arguments:
;               ds                      Context selector
;               pHWLogdevCXOffset       offset to HWLOGDEVCX
;               dwDllNameSel            Selector of where to put dll name
;               dwDllNameOffset         Offset of where to put dll name
;
; Returns:      eax                     The OGL ICD version
;               ebx                     The OGL driver version
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetOpenGLRegistryName, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD   dwDllNameSel
PARMD   dwDllNameOffset
OPENPROC
        PUSHR   ds,esi,edi

        ; Copy the DLL Name
        mov     es,dwDllNameSel
        mov     edi,dwDllNameOffset
        mov     ax,SEG szOGLRegKeyName
        mov     fs,ax
        lea     esi,szOGLRegKeyName
@@:
        mov     ax,fs:[esi]
        inc     esi
        mov     es:[edi],al
        inc     edi
        or      al,al
        jne     @B

        mov     eax,OPENGL_ICD_VER
        mov     ebx,OPENGL_DRIVER_VER
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_DirectXService
;
; Purpose:      This routine can be called from the DirectX driver
;               when it is executing within the context of a
;               DirectCallToFlatland. There are various services
;               that are supported.
;
;               This routine is a bit of a pain. The caller is
;               running on a flat 32bit stack. Any system functions
;               we call can only handle 16bit stacks, so we have
;               to switch the stack over before doing anything.
;
; Arguments:    None
;
; Returns:
;
; Preserve:     Everything
;==============================================================================
DECPROC NV_DirectXServiceNoFrame, PASCAL, NOFRAME, FAR32
;PARMD  pHWLogdevCX
;PARMD  dwFunction
;PARMD  pIn
;PARMD  pOut
OPENPROC
        ; Save some registers -- 30H bytes worth
        ; on the 32bit stack.
        PUSHR   alld,ds,es,fs,gs

        ; Load up the parameters
        lea     edi,[esp + 20H + 4 * @WordSize + 08H]
        mov     ebp,ss:[edi + 0CH]                      ; pHWLogdevCX
        mov     eax,ss:[edi + 08H]                      ; dwFunction
        mov     ebx,ss:[edi + 04H]                      ; pIn
        mov     ecx,ss:[edi + 00H]                      ; pOut
        mov     edi,esp
        sub     esi,esi
        mov     si,ss

        ; Load up a temporary stack we have in our data segment
        mov     dx,_DATA
        mov     ss,dx
        mov     sp,OFFSET TempStackTop

        ; Save the 32bit stack registers
        mov     ss:[dwOldSS],esi
        mov     ss:[dwOldEsp],edi

        ; Now push the arguments on the new stack
        push    ebp
        push    eax
        push    ebx
        push    ecx
        call    NV_DirectXService

        ; restore the 32bit stack
        mov     eax,ss:[dwOldSS]
        mov     ecx,ss:[dwOldEsp]
        mov     ss,ax
        mov     esp,ecx

        ; pop all the registers
        POPR    alld,ds,es,fs,gs

        RETFAR32 10H
CLOSEPROC

DECPROC NV_DirectXService, PASCAL, FRAME, NEAR
PARMD   pHWLogdevCX
PARMD   dwFunction
PARMD   pIn
PARMD   pOut
LOCALD  dwFlatSelector
OPENPROC
        ; pIn and pOut are both flat ptrs. We need the flat selector
        ; loaded in order to access them.
        sub     esi,esi
        mov     ds,wFlatDataSel_In_NVTEXT16
        mov     edi,pHWLogdevCX
        mov     edi,ds:[edi].HWLOGDEVCX.pDirectXSharedData
        mov     eax,ds:[edi].DISPDRVDIRECTXCOMMON.dwFlatDataSel
        mov     dwFlatSelector,eax

        mov     eax,dwFunction
        cmp     eax,NV_DRVSERVICE_ALLOC_MEMORY
        je      AllocMem
        cmp     eax,NV_DRVSERVICE_FREE_MEMORY
        je      FreeMem
        cmp     eax,NV_DRVSERVICE_BEGINACCESS
        je      DoBegAcc
        cmp     eax,NV_DRVSERVICE_ENDACCESS
        je      DoEndAcc
        sub     eax,eax
        jmp     Done

AllocMem:
        mov     es,dwFlatSelector
        mov     edx,pIn
        mov     eax,es:[edx].DRVSERVICE_ALLOC_MEMORY.dwNumBytes
        mov     ebx,es:[edx].DRVSERVICE_ALLOC_MEMORY.dwAllocFlags
        mov     ecx,es:[edx].DRVSERVICE_ALLOC_MEMORY.dwObjectID
        mov     esi,pHWLogdevCX
        mov     esi,ds:[esi].HWLOGDEVCX.pHWBoardCX
        call    NV_AllocMemory
        mov     es,dwFlatSelector
        mov     edx,pOut
        mov     es:[edx],eax
        jmp     Done

FreeMem:
        mov     esi,pHWLogdevCX
        mov     esi,ds:[esi].HWLOGDEVCX.pHWBoardCX
        mov     es,dwFlatSelector
        mov     edx,pIn
        mov     ecx,es:[edx].DRVSERVICE_FREE_MEMORY.dwObjectID
        call    NV_FreeMemory
        mov     es,dwFlatSelector
        mov     edx,pOut
        mov     es:[edx],eax
        jmp     Done

DoBegAcc:
        sub     eax,eax
        les     bx,pIn
        mov     di,es:[bx].DIBENGINE.deFlags
        and     di,BUSY
        and     es:[bx].DIBENGINE.deFlags,NOT BUSY
        push    pIn
        push    eax
        push    eax
        pushw   FB_ACCESS OR CURSOREXCLUDE
        call    FFP16 PTR es:[bx].DIBENGINE.deBeginAccess
        les     bx,pIn
        or      es:[bx].DIBENGINE.deFlags,di
        jmp     Done

DoEndAcc:
        les     bx,pIn
        mov     di,es:[bx].DIBENGINE.deFlags
        and     di,BUSY
        and     es:[bx].DIBENGINE.deFlags,NOT BUSY
        push    pIn
        pushw   FB_ACCESS OR CURSOREXCLUDE
        call    FFP16 PTR es:[bx].DIBENGINE.deEndAccess
        les     bx,pIn
        or      es:[bx].DIBENGINE.deFlags,di
        jmp     Done

Done:
CLOSEPROC


;==============================================================================
;
; Function:     HW_TellResManSpanningState
;
; Purpose:      This routine informs the Resource Manager that we are
;               in spanning mode.
;
; Arguments:
;               ds                      Context selector
;               pHWLogdevCXOffset       offset to HWLOGDEVCX
;
; Returns:      
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_TellResManSpanningState, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD   dwSpanningState
OPENPROC
        PUSHR   ds,esi,edi
        
        mov     esi,pHWLogdevCXOffset
        mov     edi,ds:[esi].HWLOGDEVCX.pHWBoardCX

        pushd   NV_CFG_DISPLAY_CHANGE_CONFIG    
        push    dwSpanningState
        push    ds:[edi].HWBOARDCX.dwDeviceNum
        CALLFAR16IND lpfnNvConfigSet,es,ax

        POPR    ds,esi,edi
CLOSEPROC


CLOSESEG _NVTEXT16

END

