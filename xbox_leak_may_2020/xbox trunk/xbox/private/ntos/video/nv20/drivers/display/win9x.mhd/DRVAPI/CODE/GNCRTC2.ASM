;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gncrtc2.asm
;
; Purpose:      This file holds run-time routines for GNCRTCCX.
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
include modeext.inc
include minivdd.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE
PUBLIC  szMonitorDeviceName
szMonitorDeviceName     DB      "MONITOR",0

;============================================================================
;
; Function:     GN_CrtcMessage
;
; Purpose:      This routine gets called whenver there is an
;               message to send to all objects. There are
;               a few different messages.  See gngdi.inc
;               for details.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;               dwMessage               which message
;               dwData1                 additional data that depends
;                                       upon the message type
;               dwData2                 additional data that depends
;                                       upon the message type
; Returns:      None
;
; Preserve:     ds,esi,edi
;============================================================================
DECPROC GN_CrtcMessage, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   dwMessage
PARMD   dwData1
PARMD   dwData2
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNCrtcCXOffset

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
        je      MoveLogDev
        cmp     eax,MSG_DEVICESREMAPPED
        je      DevRemap
        jmp     ChainIt

PreMS:
        jmp     ChainIt

PostMS:
        ; Clear out the cursor cache.
        push    esi
        call    GN_ClearCursorCache

        ; We are no longer initting after the first modeset
        mov     ds:[esi].GNCRTCCX.dwInitting,0
        jmp     ChainIt

DXEnum:
DXOn:
DXOff:
        jmp     ChainIt

DosOff:
        push    dwData1
        push    esi
        call    GN_SetBytesPerPixelAndBytesPerLine

        push    esi
        call    GN_SetPanRectangle

        ; Now, reload the color map
        push    esi
        call    GN_ResetColorMap

PowerOn:
        ; Clear out the cursor cache.
        push    esi
        call    GN_ClearCursorCache
        jmp     ChainIt

DosOn:
DrvDis:
        ; Clear out the cursor cache.
        push    esi
        call    GN_ClearCursorCache

        ; Set a NULL cursor so the cursor will disappear
        pushd   0
        call    GENERIC_SetCursor

PowerOff:
        jmp     ChainIt

MoveLogDev:
        jmp     ChainIt

DevRemap:
        push    pGNCrtcCXOffset
        push    dwData1
        call    GN_CrtcDeviceHasBeenRemapped
        jmp     ChainIt

ChainIt:
        ; Call the hardware to inform it of the message
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        push    dwMessage
        push    dwData1
        push    dwData2
        call    HW_CrtcMessage

DoneIt2:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_RestoreCrtcState
;
; Purpose:      This routine gets called to put the GNCRTCCX
;               back into the HWCRTCCX.
;
; Arguments:
;               ds                      CX Sel
;               pGNCrtcCXOffset         GNCRTCCX ptr
;
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_RestoreCrtcState, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   pGNLogDevCXOffset
OPENPROC
        PUSHR   esi

        ; Reset the mode
        push    pGNCrtcCXOffset
        push    pGNLogDevCXOffset
        call    GN_SetCrtcMode

        ; Force the cursor cache to be re-created
        push    pGNCrtcCXOffset
        call    GN_ClearCursorCache

        ; Turn the cursor back on if it is not NULL
        push    pGNCrtcCXOffset
        pushd   1
        pushd   1
        call    GN_TurnCursorOffOrOn

        ; restore the
        push    pGNCrtcCXOffset
        call    GN_ResetColorMap

        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_CrtcDeviceHasBeenRemapped
;
; Purpose:      This routine gets called whenever we get the
;               DEVICESREMAPPED message. The CRTC must update
;               any internal state it needs to because its
;               display device has changed.
;
; Arguments:
;               ds                      CX Sel
;               pGNCrtcCXOffset         GNCRTCCX ptr
;
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_CrtcDeviceHasBeenRemapped, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   pGNLogDevCXOffset
LOCALD  pHWCrtcCXOffset
LOCALV  sDispInfo,DISPLAYINFO
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNCrtcCXOffset

        ; Check for video mirror enabled on this CRTC?
        mov     eax,ds:[esi].GNCRTCCX.pHWCrtcCX
        or      eax,eax
        jne     @F
        mov     eax,ds:[esi].GNCRTCCX.pHWCrtcCXWhenLocked
@@:
        mov     pHWCrtcCXOffset,eax

        ; We only do this for the primary device in each driver
        cmp     ds:[esi].GNCRTCCX.dwPhysicalDeviceInstance,0
        jne     @F

        ; This forces the monitors to be re-enumerated and
        ; it forces Windows to reget all the EDIDS which
        ; may be necessary after a device remapping. We pass
        ; up the physical head that the first Windows logical
        ; device is using.
        push    pHWCrtcCXOffset
        call    HW_GetPhysicalCrtcIndex
        mov     ebx,pGNLogDevCXOffset
        push    ds:[ebx].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard
        or      eax,80000000H
        push    eax
        call    GN_VddReenumerateDevNode

@@:
        push    pHWCrtcCXOffset
        call    HW_GetDacLimits
        lea     edi,[esi].GNCRTCCX.sNvmodeDispData
        mov     ds:[edi].DISPDATA.dwMaxDacSpeedInHertz8bpp,eax
        mov     ds:[edi].DISPDATA.dwMaxDacSpeedInHertz16bpp,ebx
        mov     ds:[edi].DISPDATA.dwMaxDacSpeedInHertz32bpp,ecx

        mov     ax,ss
        shl     eax,10H
        lea     ax,sDispInfo
        push    eax
        pushd   VDD_GET_DISPLAY_CONFIG
        call    GN_VddGetDisplayInfo

        ; Get the registry path to our device local stuff.
        ; Anything that is not the primary device does not
        ; get an .inf file.
        ; Also, because the results of Reenumerate are asyncronous
        ; and delayed by about five seconds, when switching display
        ; devices we have to just go without a .inf status the first
        ; time. A modeset that occurs but does not involve a display
        ; device switch will attempt to reget the .inf status.
        ; However, if we are initting, and the device is the primary,
        ; get the INF or we won't have it for modesets until the display
        ; device changes.
        mov     ds:[edi].DISPDATA.dwMonitorInfType,UNKNOWN_DEVICE
        cmp     ds:[esi].GNCRTCCX.dwPhysicalDeviceInstance,0
        jne     @F
        cmp     ds:[esi].GNCRTCCX.dwInitting,1
        je      GetDevPath
        cmp     ds:[esi].GNCRTCCX.dwDisplayDeviceChange,0
        jne     @F
        
GetDevPath:
        lea     eax,[esi].GNCRTCCX.szLocalMonitorBasePath
        push    sDispInfo.diMonitorDevNodeHandle
        mov     ax,ds
        push    eax
        lea     ecx,[esi].GNCRTCCX.szLocalMonitorBasePath
        push    ecx
        mov     ax,SEG szMonitorDeviceName
        push    eax
        mov     ax,OFFSET szMonitorDeviceName
        movzx   eax,ax
        push    eax
        call    GN_GetDeviceLocalBasePath
        mov     ds:[edi].DISPDATA.dwMonitorInfType,ecx
@@:
        ; In case it was set, clear out the display device change flag
        mov     ds:[esi].GNCRTCCX.dwDisplayDeviceChange,0

        ; Assume no Edid
        mov     ds:[esi].GNCRTCCX.dwEdidSize,0

        ; Reread the EDID if the device is not a TV
        push    esi
        call    GN_GetDevData
        cmp     al,DEVTYPE_TV
        je      @F

        ; Read the EDID for this device
        mov     ds:[esi].GNCRTCCX.dwEdidSize,MAX_EDID_BUFFER_SIZE
        push    pHWCrtcCXOffset
        push    eax
        lea     eax,[esi].GNCRTCCX.sEdidBuffer
        push    eax
        lea     eax,[esi].GNCRTCCX.dwEdidSize
        push    eax
        call    HW_ReadEdid
@@:
        ; Set the new Display Device type into the registry
        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        push    ds:[esi].GNCRTCCX.dwLogicalDeviceInstance
        push    ds:[esi].GNCRTCCX.dwPhysicalDeviceInstance
        sub     ebx,ebx
        lfs     bx,ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    fs:[ebx].DISPDATA.lpDevData
        CALLFAR16IND lpfnSetDisplayDevice,es,ax

        ; If this device is a TV, set the TV Format in the ResMan.
        sub     ebx,ebx
        lfs     bx,ds:[esi].GNCRTCCX.lpNvmodeDispData
        lfs     bx,fs:[ebx].DISPDATA.lpDevData
        mov     eax,fs:[ebx]
        cmp     al,DEVTYPE_TV
        jne     SkipTVFormat
        push    pHWCrtcCXOffset
        push    eax
        call    HW_SetResManTvFormat
        
SkipTVFormat:

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_CrtcRemapArray
;
; Purpose:      This routine gets all the display devices types
;               associated with all the CRTCCXs, asks the hardware
;               for a remapping.
;
;               The caller must have verified that the set of all
;               display device types in all the CRTCCXs are able
;               to be used simultaneously, presumably by having
;               called HW_CanDevicesBeMapped.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;               dwRestoreStates         Restore the CRTC state
;                                       for those devices which
;                                       changed under the remap
;                                       if this is non-0.
;                                       otherwise, don't.
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
DECPROC GN_CrtcRemapArray, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
LOCALS  LocalDevData,(MAX_ATTACHED_CRTCS*4)
LOCALD  lpLocalDevData
LOCALS  aGNCrtcCX,MAX_ATTACHED_CRTCS*4
LOCALS  aHWCrtcCX,MAX_ATTACHED_CRTCS*4
LOCALD  lpGNCrtcCX
LOCALD  lpHWCrtcCX
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset

        ; Just some setup
        mov     ax,ss
        shl     eax,10H
        lea     ax,LocalDevData
        mov     lpLocalDevData,eax
        lea     ax,aGNCrtcCX
        mov     lpGNCrtcCX,eax
        lea     ax,aHWCrtcCX
        mov     lpHWCrtcCX,eax

        ; Get all the attached devices
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    lpLocalDevData
        push    lpGNCrtcCX
        push    lpHWCrtcCX
        pushd   MAX_ATTACHED_CRTCS
        call    GN_GetAllDeviceData

        ; Ask the hardware to resolve the mappings so things
        ; will just work ;-).
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ecx].GNBOARDCX.pHWBoardCX
        push    lpLocalDevData
        push    lpHWCrtcCX
        push    eax
        call    HW_MapDevices
        push    eax

        ; Send a message around telling everyone that the
        ; hardware has remapped device assignments.
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        call    GN_SendDevicesRemapMsgToAllLogDevs

        pop     eax
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_RestoreAllCrtcStates
;
; Purpose:      This routine gets called to restore all the
;               CRTC states.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;               dwRestoreStates         Restore the CRTC state
;                                       for those devices which
;                                       changed under the remap
;                                       if this is non-0.
;                                       otherwise, don't.
;
; Returns:
;               None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_RestoreAllCrtcStates, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
LOCALS  LocalDevData,(MAX_ATTACHED_CRTCS*4)
LOCALD  lpLocalDevData
LOCALS  aGNCrtcCX,MAX_ATTACHED_CRTCS*4
LOCALS  aHWCrtcCX,MAX_ATTACHED_CRTCS*4
LOCALD  lpGNCrtcCX
LOCALD  lpHWCrtcCX
LOCALD  dwDeviceCount
LOCALD  dwDevDataMask
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset

        ; Just some setup
        mov     ax,ss
        shl     eax,10H
        lea     ax,LocalDevData
        mov     lpLocalDevData,eax
        lea     ax,aGNCrtcCX
        mov     lpGNCrtcCX,eax
        lea     ax,aHWCrtcCX
        mov     lpHWCrtcCX,eax

        ; Get all the attached devices
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    lpLocalDevData
        push    lpGNCrtcCX
        push    lpHWCrtcCX
        pushd   MAX_ATTACHED_CRTCS
        call    GN_GetAllDeviceData
        mov     dwDeviceCount,eax

        ; We need to go into a loop here. Restore all states
        sub     edi,edi

        ; Get the Device Data Mask for all display devices.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ebx
        pushd   0
        call    GN_GetDevDataMask
        or      eax,80000000H
        mov     dwDevDataMask,eax

        ; Signal the beginning of a display state change
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwDevDataMask
        call    HW_BeginDisplayChange

Loopit7:
        ; Is this CRTCCX valid?
        sub     ebx,ebx
        les     bx,lpGNCrtcCX
        push    dword ptr es:[ebx][edi * 4]
        push    esi
        call    GN_RestoreCrtcState
        inc     edi
        cmp     edi,dwDeviceCount
        jc      Loopit7

        ; Signal the end of a display state change
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwDevDataMask
        call    HW_EndDisplayChange

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_SetBytesPerPixelAndBytesPerLine
;
; Purpose:      This routine gets called to let the CRTCCX know
;               the current number of bytes per pixel and bytes
;               per line. It needs this to do panning.
;
; Arguments:
;               ds                      CX Sel
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;               pGNCrtcCXOffset         GNCRTCCX ptr
;
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_SetBytesPerPixelAndBytesPerLine, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
PARMD   pGNCrtcCXOffset
OPENPROC
        mov     ebx,pGNLogdevCXOffset
        mov     ecx,pGNCrtcCXOffset
        movzx   eax,ds:[ebx].DIBENGINEHDR.deBitsPixel
        inc     eax
        shr     eax,3
        mov     ds:[ecx].GNCRTCCX.dwBytesPerPixelInCurrentMode,eax
        mov     eax,ds:[ebx].DIBENGINEHDR.deDeltaScan
        mov     ds:[ecx].GNCRTCCX.dwBytesPerLineInCurrentMode,eax
CLOSEPROC


;==============================================================================
;
; Function:     GN_SetPanRectangle
;
; Purpose:      This routine gets called whenever the panning
;               rectangle needs to be reset. The fields in
;               the GNCRTCCX.rcPhysicalViewport have been
;               modified and need to get set back into the
;               hardware.
;
; Arguments:
;               ds                      CX Sel
;               pGNCrtcCXOffset         GNCRTCCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_SetPanRectangle, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
OPENPROC
        ; Set the Pan Coordinates
        mov     ebx,pGNCrtcCXOffset
        mov     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwTop
        imul    eax,ds:[ebx].GNCRTCCX.dwBytesPerLineInCurrentMode
        mov     ecx,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwLeft
        imul    ecx,ds:[ebx].GNCRTCCX.dwBytesPerPixelInCurrentMode
        add     eax,ecx

        ; Note that we must have a multiple of 4 for the hardware to
        ; pan to it, but this means the address may not match the
        ; rectangle coordinates exactly. We can fix that if it ever
        ; really becomes an issue. For right now, I'll ignore it.
        add     eax,3
        and     eax,NOT 3

        push    ds:[ebx].GNCRTCCX.pHWCrtcCX
        push    eax
        call    HW_SetPanOffset
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetDevData
;
; Purpose:      This routine gets called to get the DevData for the
;               CRTCCX.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;
; Returns:      GNCRTCCX.sCrtcModeInfo.MIDevData
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetDevData, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
OPENPROC
        mov     eax,pGNCrtcCXOffset
        lea     eax,[eax].GNCRTCCX.sCrtcModeInfo
        mov     eax,ds:[eax].MODEINFO.MIDevData
CLOSEPROC


;==============================================================================
;
; Function:     GN_SetDevData
;
; Purpose:      This routine gets called to set the DevData for the
;               CRTCCX.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;               NewDevData              DEVDATA
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_SetDevData, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   NewDevData
OPENPROC
        mov     ecx,pGNCrtcCXOffset
        mov     eax,NewDevData
        lea     ecx,[ecx].GNCRTCCX.sCrtcModeInfo
        mov     ds:[ecx].MODEINFO.MIDevData,eax
CLOSEPROC


;==============================================================================
;
; Function:     GN_AssignCrtcModeOnlyBackOffSpatial
;
; Purpose:      This routine assigns the CRTC mode to be the mode
;               passed in. If the CRTC cannot do the mode passed
;               in, then back off ONLY the XRes, and YRes, to
;               get the mode that it can do. Try to leave the
;               refresh rate alone.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;               lpNewModeInfo           DEVDATA
;
; Returns:      GNCRTCCX.CrtcDevData
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_AssignCrtcModeOnlyBackOffSpatial, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   lpNewModeInfo
LOCALV  sSettableCrtcModeInfo,MODEINFO
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNCrtcCXOffset

        ; Use the suggested mode passed in
        sub     edi,edi
        les     di,ds:[esi].GNCRTCCX.lpCrtcModeInfo
        push    es:[edi].MODEINFO.MIDevData
        sub     esi,esi
        lfs     si,lpNewModeInfo
        mov     ecx,SIZE MODEINFO
        rep     movs byte ptr es:[edi], fs:[esi]
        sub     edi,SIZE MODEINFO
        pop     es:[edi].MODEINFO.MIDevData

        mov     esi,pGNCrtcCXOffset

        ; We don't have a way to back off just the XRes,YRes at
        ; this point. This may back off the refresh rate. JKTODO
        push    ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    ds:[esi].GNCRTCCX.lpEdidBuffer
        push    ds:[esi].GNCRTCCX.dwEdidSize
        push    ds:[esi].GNCRTCCX.lpCrtcModeInfo
        push    ds:[esi].GNCRTCCX.lpCrtcModeOut
        mov     ax,ss
        shl     eax,10H
        lea     ax,sSettableCrtcModeInfo
        push    eax
        CALLFAR16IND lpfnFindModeEntry,es,ax

        ; Copy the SettableModeInfo into the CrtcModeInfo
        sub     esi,esi
        push    ss
        pop     fs
        lea     si,sSettableCrtcModeInfo
        mov     edi,pGNCrtcCXOffset
        les     di,ds:[edi].GNCRTCCX.lpCrtcModeInfo
        movzx   edi,di
        mov     ecx,SIZE MODEINFO
        rep     movs byte ptr es:[edi], fs:[esi]

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetCrtcMode
;
; Purpose:      This routine gets called to get the mode which should
;               be set on this CRTC.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;               lpSuggMode              mode to use if no mode found
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetCrtcMode, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   lpSuggMode
LOCALV  sSettableCrtcModeInfo,MODEINFO
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNCrtcCXOffset

        ; First, if there is a forced mode we MUST set that
        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        push    ds:[esi].GNCRTCCX.lpCrtcModeInfo
        CALLFAR16IND lpfnGetForcedDisplayMode,es,ax
        or      ax,ax
        jne     @F

        ; If not, just read the last mode that was set on this device
        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        push    ds:[esi].GNCRTCCX.lpCrtcModeInfo
        CALLFAR16IND lpfnGetDeviceDisplayMode,es,ax
        or      ax,ax
        jne     @F

        ; Use the suggested mode passed in
        sub     edi,edi
        les     di,ds:[esi].GNCRTCCX.lpCrtcModeInfo
        push    es:[edi].MODEINFO.MIDevData
        sub     esi,esi
        lfs     si,lpSuggMode
        mov     ecx,SIZE MODEINFO
        rep     movs byte ptr es:[edi], fs:[esi]
        sub     edi,SIZE MODEINFO
        pop     es:[edi].MODEINFO.MIDevData
@@:
        
        ; Validate the mode but first update the color depth to match
        ; the desktop. It also resets the lpCrtcModeInfo passed in to
        ; something it can handle if the mode provided was out of its
        ; range.
        mov     esi,pGNCrtcCXOffset
        sub     edi,edi
        les     di,lpSuggMode
        mov     eax,es:[edi].MODEINFO.dwBpp
        les     di,ds:[esi].GNCRTCCX.lpCrtcModeInfo
        mov     es:[edi].MODEINFO.dwBpp,eax
        
        ; Now we are ready to validate!
        push    ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    ds:[esi].GNCRTCCX.lpEdidBuffer
        push    ds:[esi].GNCRTCCX.dwEdidSize
        push    ds:[esi].GNCRTCCX.lpCrtcModeInfo
        push    ds:[esi].GNCRTCCX.lpCrtcModeOut
        mov     ax,ss
        shl     eax,10H
        lea     ax,sSettableCrtcModeInfo
        push    eax
        CALLFAR16IND lpfnFindModeEntry,es,ax

        ; Copy the SettableModeInfo into the CrtcModeInfo
        sub     esi,esi
        push    ss
        pop     fs
        lea     si,sSettableCrtcModeInfo
        mov     edi,pGNCrtcCXOffset
        les     di,ds:[edi].GNCRTCCX.lpCrtcModeInfo
        movzx   edi,di
        mov     ecx,SIZE MODEINFO
        rep     movs byte ptr es:[edi], fs:[esi]

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_RestrictOtherModeSpatial
;
; Purpose:      This routine gets called to munge down some other
;               mode to something smaller than the CRTC mode. If
;               the other mode is already smaller, then nothing
;               happens.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;               lpOtherModeInfo         MODEINFO structure describing mode
;                                       to restrict CRTC mode to
;
; Returns:
;               eax     1 then mode was reduced
;                       0 then mode did not need to be changed
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_RestrictOtherModeSpatial, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   lpOtherModeInfo
OPENPROC
        PUSHR   esi

        mov     eax,pGNCrtcCXOffset
        sub     ebx,ebx
        lfs     bx,ds:[eax].GNCRTCCX.lpCrtcModeInfo
        sub     edx,edx
        les     dx,lpOtherModeInfo
        sub     esi,esi

        mov     eax,fs:[ebx].MODEINFO.dwXRes
        mov     ecx,fs:[ebx].MODEINFO.dwYRes
        cmp     eax,es:[edx].MODEINFO.dwXRes
        jb      DropOther
        cmp     ecx,es:[edx].MODEINFO.dwYRes
        jae     IsOK

DropOther:
        mov     es:[edx].MODEINFO.dwXRes,eax
        mov     es:[edx].MODEINFO.dwYRes,ecx
        mov     esi,1
IsOK:
        mov     eax,esi
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_RestrictCrtcModeSpatial
;
; Purpose:      This routine gets called to munge down the CRTC
;               mode to something smaller than the other mode. If
;               the CRTC mode is already smaller, then nothing happens.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;               lpOtherModeInfo         MODEINFO structure describing mode
;                                       to restrict CRTC mode to
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_RestrictCrtcModeSpatial, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   lpOtherModeInfo
LOCALV  sSettableCrtcModeInfo,MODEINFO
OPENPROC
        PUSHR   esi,edi
        mov     eax,pGNCrtcCXOffset
        sub     ebx,ebx
        lfs     bx,lpOtherModeInfo
        sub     edx,edx
        les     dx,ds:[eax].GNCRTCCX.lpCrtcModeInfo

        mov     eax,fs:[ebx].MODEINFO.dwXRes
        mov     ecx,fs:[ebx].MODEINFO.dwYRes

        cmp     eax,es:[edx].MODEINFO.dwXRes
        ja      @F
        mov     es:[edx].MODEINFO.dwXRes,eax
@@:
        cmp     ecx,es:[edx].MODEINFO.dwYRes
        ja      @F
        mov     es:[edx].MODEINFO.dwYRes,ecx
        
@@:
        ; We now need to validate this mode as it may not be a valid mode since
        ; both the XRes and YRes may not have been changed to the new value,
        ; e.g., if the desktop is 1600x900 and the GNCRTCCX.lpCrtcModeInfo
        ; is set to 1400x1050, we'll end up with 1400x900.
        mov     esi,pGNCrtcCXOffset
        push    ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    ds:[esi].GNCRTCCX.lpEdidBuffer
        push    ds:[esi].GNCRTCCX.dwEdidSize
        push    ds:[esi].GNCRTCCX.lpCrtcModeInfo
        push    ds:[esi].GNCRTCCX.lpCrtcModeOut
        mov     ax,ss
        shl     eax,10H
        lea     ax,sSettableCrtcModeInfo
        push    eax
        CALLFAR16IND lpfnFindModeEntry,es,ax

        ; Copy the SettableModeInfo into the CrtcModeInfo
        sub     esi,esi
        push    ss
        pop     fs
        lea     si,sSettableCrtcModeInfo
        mov     edi,pGNCrtcCXOffset
        les     di,ds:[edi].GNCRTCCX.lpCrtcModeInfo
        movzx   edi,di
        mov     ecx,SIZE MODEINFO
        rep     movs byte ptr es:[edi], fs:[esi]

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_CrtcWriteModeToReg
;
; Purpose:      This routine can write out the phys CRTC mode to
;               the registry under certain conditions. It can either
;               always write it out, or only write it out if there
;               is no current phys mode in the registry for the
;               current CRTCCX device.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;               dwFlag                  if FALSE, always write mode to reg
;                                       if TRUE, only write mode to reg if
;                                       there is no mode in the reg for it
;                                       already
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_CrtcWriteModeToReg, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   dwFlag
LOCALV  sModeInfo,MODEINFO
LOCALD  lpThisModeInfo
OPENPROC
        PUSHR   esi,edi

        cmp     dwFlag,0
        je      @F

        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        mov     lpThisModeInfo,eax

        ; Copy the CRTC MODEINFO struct over to a temporary spot
        mov     edi,pGNCrtcCXOffset
        sub     esi,esi
        lfs     si,ds:[edi].GNCRTCCX.lpCrtcModeInfo
        sub     edi,edi
        les     di,lpThisModeInfo
        mov     ecx,SIZE MODEINFO
        rep     movs byte ptr es:[edi], fs:[esi]

        ; Is there already a mode for it?
        mov     eax,pGNCrtcCXOffset
        push    ds:[eax].GNCRTCCX.lpBoardRegistryData
        push    lpThisModeInfo
        CALLFAR16IND lpfnGetDeviceDisplayMode,es,ax
        or      ax,ax
        jne     DoneIt
@@:
        mov     eax,pGNCrtcCXOffset
        push    ds:[eax].GNCRTCCX.lpBoardRegistryData
        push    ds:[eax].GNCRTCCX.lpCrtcModeInfo
        CALLFAR16IND lpfnSetDeviceDisplayMode,es,ax
DoneIt:
        POPR    esi,edi
CLOSEPROC



;==============================================================================
;
; Function:     GN_SetCrtcMode
;
; Purpose:      This routine gets called to get the mode which should
;               be set on this CRTC. This routine cannot fail. Hence,
;               the caller must have validated that this mode can be
;               set on this CRTC. The mode is revalidated in here
;               however, and backed down if there is a problem.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;               pGNLogDevCXOffset       GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_SetCrtcMode, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   pGNLogDevCXOffset
LOCALD  lpSettableModeInfo
LOCALV  sSettableModeInfo,MODEINFO
LOCALD  dwTopEdge
LOCALD  dwLeftEdge
LOCALD  dwBottomEdge
LOCALD  dwRightEdge
OPENPROC
        PUSHR   esi,edi
        mov     edi,pGNCrtcCXOffset

        ; Need this a few times
        mov     ax,ss
        shl     eax,10H
        lea     ax,sSettableModeInfo
        mov     lpSettableModeInfo,eax

        ; Make sure the refresh rate list in the registry
        ; is the intersection of this specific hardware
        ; device and our board's capabilities.
        mov     ebx,ds:[edi].GNCRTCCX.lpCrtcModeInfo
        lea     ebx,[ebx].MODEINFO.MIDevData
        push    ebx
        push    ds:[edi].GNCRTCCX.lpNvmodeDispData
        push    ds:[edi].GNCRTCCX.lpEdidBuffer
        push    ds:[edi].GNCRTCCX.dwEdidSize
        call    FFP16 PTR lpfnResetRefreshRates

        ; Get the correct timings for this CRTC
        push    ds:[edi].GNCRTCCX.lpNvmodeDispData
        push    ds:[edi].GNCRTCCX.lpEdidBuffer
        push    ds:[edi].GNCRTCCX.dwEdidSize
        push    ds:[edi].GNCRTCCX.lpCrtcModeInfo
        push    ds:[edi].GNCRTCCX.lpCrtcModeOut
        push    lpSettableModeInfo
        CALLFAR16IND lpfnFindModeEntry,es,ax

        ; Copy the SettableModeInfo into the CrtcModeInfo
        sub     esi,esi
        lfs     si,lpSettableModeInfo
        les     di,ds:[edi].GNCRTCCX.lpCrtcModeInfo
        movzx   edi,di
        mov     ecx,SIZE MODEINFO
        rep     movs byte ptr es:[edi], fs:[esi]
        mov     edi,pGNCrtcCXOffset

        ; Remember the bytes per pixel and bytes per line of this mode
        push    pGNLogDevCXOffset
        push    edi
        call    GN_SetBytesPerPixelAndBytesPerLine

        ; Update the mode pitch to the desktop pitch
        mov     ecx,pGNLogDevCXOffset
        mov     eax,ds:[ecx].DIBENGINEHDR.deDeltaScan
        lfs     bx,ds:[edi].GNCRTCCX.lpCrtcModeOut
        mov     fs:[bx].MODEOUT.dwPitchInBytes,eax

        ; Fill in the Format field of the ModeOut structure whether
        ; or not this is the primary physical device on the primary
        ; logical device
        cmp     ds:[edi].GNCRTCCX.dwLogicalDeviceInstance,0
        jne     NotPrimary
        cmp     ds:[edi].GNCRTCCX.dwPhysicalDeviceInstance,0
        jne     NotPrimary
        sub     ebx,ebx
        lfs     bx,ds:[edi].GNCRTCCX.lpCrtcModeOut
        or      fs:[ebx].MODEOUT.dwFormat,10000000h

NotPrimary:
        ; Call the hardware to program the timings
        mov     ebx,pGNLogDevCXOffset
        push    ds:[edi].GNCRTCCX.pHWCrtcCX
        push    dword ptr ds:[ebx].DIBENGINEHDR.deBitsOffset
        push    ds:[edi].GNCRTCCX.lpCrtcModeOut
        call    HW_SetCrtcMode

        ; Get the registry key for whether or not a StopImage needs to
        ; be issued. If StopImage is issued, this will blank the display.
        mov     ebx,pGNLogDevCXOffset
        push    ds:[ebx].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetModesetStopImage,es,ax
        cmp     ax,1
        jne     SetViewPort
        
        push    ds:[edi].GNCRTCCX.pHWCrtcCX
        call    HW_SetStopImage
        jmp     Done
        
SetViewPort:
        ; Set the viewport coordinates
        push    ds:[edi].GNCRTCCX.lpBoardRegistryData
        lfs     bx,ds:[edi].GNCRTCCX.lpNvmodeDispData
        push    fs:[bx].DISPDATA.lpDevData
        mov     ax,ss
        shl     eax,10h
        lea     ax,dwLeftEdge
        push    eax
        lea     ax,dwTopEdge
        push    eax
        lea     ax,dwRightEdge
        push    eax
        lea     ax,dwBottomEdge
        push    eax
        CALLFAR16IND lpfnGetAutoPanMode,es,ax

        cmp     ax,0
        je      NoAutoPanRegKey

        ; Make sure the PhysicalViewport in the registry is smaller
        ; than the virtual mode.
        mov     ecx,pGNLogDevCXOffset
        movzx   edx,ds:[ecx].GNLOGDEVCX.Dibeng.deWidth
        cmp     edx,dwRightEdge
        jl      NoAutoPanRegKey
        movzx   edx,ds:[ecx].GNLOGDEVCX.Dibeng.deHeight
        cmp     edx,dwBottomEdge
        jl      NoAutoPanRegKey

        ; Make sure that the PhysicalViewport in the registry is the
        ; same dimensions as the mode that was set.
        sub     ebx,ebx
        lfs     bx,ds:[edi].GNCRTCCX.lpCrtcModeOut
        movzx   eax,fs:[ebx].MODEOUT.wXRes
        mov     edx,dwRightEdge
        sub     edx,dwLeftEdge
        cmp     eax,edx
        jne     NoAutoPanRegKey
        movzx   eax,fs:[ebx].MODEOUT.wYRes
        mov     edx,dwBottomEdge
        sub     edx,dwTopEdge
        cmp     eax,edx
        jne     NoAutoPanRegKey

SetScanLineStart:
        ; Use registry entry to set scanline start and freeze origin.
        mov     ds:[edi].GNCRTCCX.dwPanAndScanEnabled,0
        mov     eax,dwTopEdge
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwTop,eax
        mov     eax,dwLeftEdge
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwLeft,eax
        mov     eax,dwBottomEdge
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwBottom,eax
        mov     eax,dwRightEdge
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwRight,eax
        jmp     DoPanSet

NoAutoPanRegKey:
        ; No AutoPanMode registry entry start in upper left hand corner
        ; and enable PanAndScan.
        mov     ds:[edi].GNCRTCCX.dwPanAndScanEnabled,1
        lea     ebx,ds:[edi].GNCRTCCX.sCrtcModeInfo
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwLeft,0
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwTop,0
        mov     eax,ds:[ebx].MODEINFO.dwXRes
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwRight,eax
        mov     eax,ds:[ebx].MODEINFO.dwYRes
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwBottom,eax

DoPanSet:
        push    edi
        call    GN_SetPanRectangle

Done:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetDisplayDevice
;
; Purpose:      This routine gets called to get the desired display
;               device.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetDisplayDevice, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
LOCALD  LocalDevData
OPENPROC
        mov     ebx,pGNCrtcCXOffset

        ; Now let us get the display device
        push    ds:[ebx].GNCRTCCX.lpBoardRegistryData
        push    ds:[ebx].GNCRTCCX.dwLogicalDeviceInstance
        push    ds:[ebx].GNCRTCCX.dwPhysicalDeviceInstance
        mov     ax,ss
        shl     eax,10h
        lea     ax,LocalDevData
        push    eax
        CALLFAR16IND lpfnGetDisplayDevice,es,ax
        or      ax,ax
        jne     GetTVFormat

        ; We didn't find one so setup for CRT0
        mov     LocalDevData,DEVTYPE_CRT
        jmp     SetDevData
        
GetTVFormat:
        ; Get the TV format from the ResMan. IF this is not a TV, the Format
        ; will be returned as 0.
        mov     ebx,pGNCrtcCXOffset
        push    ds:[ebx].GNCRTCCX.pHWCrtcCX
        push    dword ptr LocalDevData
        call    HW_GetResManTvFormat
        shl     eax,10h
        and     dword ptr LocalDevData,0ff00ffffh
        or      dword ptr LocalDevData,eax

SetDevData:
        push    pGNCrtcCXOffset
        push    dword ptr LocalDevData
        call    GN_SetDevData
CLOSEPROC



;==============================================================================
;
; Function:     GN_SetCrtcGamma
;
; Purpose:      This function is called to set the gamma ramp.
;
; Arguments:
;               ds              Context Selector
;               pGNCrtcCXOffset GNCRTCCX ptr
;               pGammaRamp      ptr to gamma ramp
;                               256 words of red followed by
;                               256 words of greed followed by
;                               256 words of blue followed by
;                               Only the high byte of each
;                               word is relevant.
;
; Returns:      Always return 1.
;
;==============================================================================
DECPROC GN_SetCrtcGamma, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   pGammaRamp
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNCrtcCXOffset

        ; First, copy the gamma table passed in
        mov     edi,pGammaRamp
        lea     esi,[esi].GNCRTCCX.dwGammaTable
        mov     ecx,100H
@@:
        movzx   eax,byte ptr ds:[edi + 200H * 0 + 1]
        shl     eax,10H
        mov     ah,byte ptr ds:[edi + 200H * 1 + 1]
        mov     al,byte ptr ds:[edi + 200H * 2 + 1]
        add     edi,2
        mov     ds:[esi],eax
        add     esi,4
        dec     ecx
        jne     @B

        ; If we are in a palettized mode, then we are done!
        mov     esi,pGNCrtcCXOffset
        lfs     bx,ds:[esi].GNCRTCCX.lpCrtcModeInfo
        cmp     fs:[bx].MODEINFO.dwBpp,8
        je      Done

        lea     eax,[esi].GNCRTCCX.dwGammaTable
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        push    eax
        pushd   0
        pushd   100H
        call    HW_SetGamma
Done:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetCrtcGamma
;
; Purpose:      This function is called to get the gamma ramp.
;
; Arguments:
;               ds              Context Selector
;               pGNCrtcCXOffset GNCRTCCX ptr
;               pGammaRamp      ptr to gamma ramp
;                               256 words of red followed by
;                               256 words of greed followed by
;                               256 words of blue followed by
;                               Only the high byte of each
;                               word is relevant.
;
; Returns:      Always return 1.
;
;==============================================================================
DECPROC GN_GetCrtcGamma, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   pGammaRamp
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNCrtcCXOffset

        ; Copy the current gamma table to the passed in array ptr
        lea     esi,[esi].GNCRTCCX.dwGammaTable
        mov     edi,pGammaRamp
        mov     ecx,100H
        sub     eax,eax
        sub     ebx,ebx
        sub     edx,edx
@@:
        mov     dh,ds:[esi + 0]
        mov     bh,ds:[esi + 1]
        mov     ah,ds:[esi + 2]
        add     esi,4
        mov     ds:[edi + 200H * 0],ax
        mov     ds:[edi + 200H * 1],bx
        mov     ds:[edi + 200H * 2],dx
        add     edi,2
        dec     ecx
        jne     @B

        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_SetCrtcPalette
;
; Purpose:      This function is called to set the palette.
;
; Arguments:
;               ds              Context Selector
;               pGNCrtcCXOffset GNCRTCCX ptr
;               pPalette        ptr to palette
;                               256 dwords of XBGR
;
; Returns:      Always return 1.
;
;==============================================================================
DECPROC GN_SetCrtcPalette, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
PARMD   pPalette
PARMD   dwFirstIndex
PARMD   dwNumEntries
OPENPROC
        PUSHR   esi,edi
        mov     edi,pGNCrtcCXOffset

        ; First, copy the gamma table passed in
        mov     esi,pPalette
        lea     edi,[edi].GNCRTCCX.dwPaletteTable
        mov     eax,dwFirstIndex
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

        ; If we are not in a palettized mode, then we are done!
        mov     esi,pGNCrtcCXOffset
        lfs     bx,ds:[esi].GNCRTCCX.lpCrtcModeInfo
        cmp     fs:[bx].MODEINFO.dwBpp,8
        jne     Done

        lea     eax,[esi].GNCRTCCX.dwPaletteTable
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        push    eax
        push    dwFirstIndex
        push    dwNumEntries
        call    HW_SetPalette
Done:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_ResetColorMap
;
; Purpose:      This function tells the hardware to reload the color
;               map -- either the palette or the gamma table, whichever
;               one is currently being used.
;
; Arguments:
;               ds              Context Selector
;               pGNCrtcCXOffset GNCRTCCX ptr
;
; Returns:      Always return 1.
;
;==============================================================================
DECPROC GN_ResetColorMap, PASCAL, FRAME, FAR16
PARMD   pGNCrtcCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pGNCrtcCXOffset

        lfs     bx,ds:[esi].GNCRTCCX.lpCrtcModeInfo
        cmp     fs:[bx].MODEINFO.dwBpp,8
        jne     IsGamma

IsPal:
        lea     eax,[esi].GNCRTCCX.dwPaletteTable
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        push    eax
        pushd   0
        pushd   100H
        call    HW_SetPalette
        jmp     DoneIt

IsGamma:
        lea     eax,[esi].GNCRTCCX.dwGammaTable
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        push    eax
        pushd   0
        pushd   100H
        call    HW_SetGamma
DoneIt:
        mov     eax,1
        POPR    esi
CLOSEPROC

CLOSESEG  _NVTEXT16

END

