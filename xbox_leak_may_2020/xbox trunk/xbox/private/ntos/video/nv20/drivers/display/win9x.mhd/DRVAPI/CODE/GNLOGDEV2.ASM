;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnlogdev.asm
;
; Purpose:      This file holds runtime routines dealing with the
;               GNLOGDEVCX.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include minivdd.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc
include gngdi.inc
include hwgdi.inc
include escape.inc
include modeext.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     GN_LogdevMessage
;
; Purpose:      This routine gets called whenver there is an
;               message to send to all objects. There are
;               a few different messages.  See gngdi.inc
;               for details.
;
;               The dwData1 and dwData2 parameters may get
;               overridden by this routine.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;               dwMessage               which message
;               dwData1                 extra data
;               dwData2                 extra data
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_LogdevMessage, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
PARMD   dwMessage
PARMD   dwData1
PARMD   dwData2
LOCALD  pGNCrtcCXOffset
LOCALD  dwCRTCCount
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset

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
        je      LogdevMove
        jmp     ChainIt

PreMS:
        ; Flag that we are busy
        or      ds:[esi].GNLOGDEVCX.Dibeng.deFlags,BUSY
        mov     ds:[esi].GNLOGDEVCX.dwHardwareBusy,1

        ; Clear the SaveScrenBitmapCache
        push    pGNLogdevCXOffset
        call    GN_ClearSaveScreenBitmapCache
        jmp     ChainIt

PostMS:
        cmp     ds:[esi].GNLOGDEVCX.dwInitting,0
        je      @F

        ; Only if we are initting AND only if this is the first
        ; logical device on this board (i.e. we are booting windows.)
        cmp     ds:[esi].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard,0
        jne     @F
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX
        call    GN_DoOddballStuffAtBoot
@@:
        ; Flag that we are no longer busy
        and     ds:[esi].GNLOGDEVCX.Dibeng.deFlags,NOT BUSY
        mov     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0

        ; If this is a secondary driver, tell the ResMan we are in
        ; spanning mode.
        cmp     ds:[esi].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard,0
        je      SkipCallToRM

        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        pushd   1
        call    HW_TellResManSpanningState

SkipCallToRM:

        ; We are no longer initting after the first modeset
        mov     ds:[esi].GNLOGDEVCX.dwInitting,0
        jmp     ChainIt

DXEnum:
DXOn:
DXOff:
        jmp     ChainIt

DosOff:
PowerOn:
        ; Enable SW access to device
        and     ds:[esi].GNLOGDEVCX.Dibeng.deFlags,NOT BUSY

        ; Indicate a screen switch in has occurred.
        mov     ds:[esi].GNLOGDEVCX.dwInt2FScreenSwitchOut,0
        jmp     ChainIt

DosOn:
PowerOff:
        ; Disable SW access to device
        or      ds:[esi].GNLOGDEVCX.Dibeng.deFlags,BUSY

        ; Wait until all hardware contexts are idle
        push    esi
        mov     esi,ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_BeginAccess_Far32
        pop     esi

        ; Indicate a screen switch out has occurred.
        mov     ds:[esi].GNLOGDEVCX.dwInt2FScreenSwitchOut,1
        jmp     ChainIt

DrvDis:
        ; Disable HW & SW access to device
        or      ds:[esi].GNLOGDEVCX.Dibeng.deFlags,BUSY
        mov     ds:[esi].GNLOGDEVCX.dwHardwareBusy,1
        jmp     ChainIt

LogdevMove:
        ; Reset the ptr to the BitmapInfo structure
        ; alloc pointer to new location.
        mov     eax,[esi].GNLOGDEVCX.Dibeng.deBitmapInfo
        shr     eax,10h
        lea     ebx,[esi].GNLOGDEVCX.bmi
        push    ax
        push    ebx
        call    SetSelectorBase

        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    dwData2
        push    dwData1
        call    GN_ReattachLogicalDeviceToBoard
        jmp     ChainIt

ChainIt:
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    dwMessage
        push    dwData1
        push    dwData2
        call    GN_BoardMessage

        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    dwMessage
        push    dwData1
        push    dwData2
        call    HW_LogdevMessage

        ; Create the GNCRTCCXs
        lea     edi,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     dwCRTCCount,0
@@:
        ; Make sure this CRTC ptr is not NULL
        cmp     dword ptr ds:[edi],0
        je      SkipIt3

        push    dword ptr ds:[edi]
        push    dwMessage
        push    dwData1
        push    dwData2
        call    GN_CrtcMessage

SkipIt3:
        ; Next CRTCCX
        add     edi,4
        inc     dwCRTCCount
        mov     eax,dwCRTCCount
        cmp     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      @B

DoneIt:
        POPR    esi,edi
CLOSEPROC



;==============================================================================
;
; Function:     GN_OffscreenHeapDisappearing
;
; Purpose:      This routine gets called whenever the offscreen
;               heap is disappearing.
;
;               Right now, we simply flush our save screen bitmap cache.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_OffscreenHeapDisappearing, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
OPENPROC
        push    pGNLogdevCXOffset
        call    GN_ClearSaveScreenBitmapCache
CLOSEPROC


;==============================================================================
;
; Function:     GN_ResolveDeviceConnectionsOnLoad
;
; Purpose:      This routine gets called just before we attach the
;               first CRTC in a display driver. It gives us a chance
;               to modify what we think the display devices should be.
;
;               UseHwSelectedDevice registry key is interpreted as:
;               0 - use NVidia behavior for establishing device types
;                   when a driver loads (whatever means NVidia wants)
;               1 - Ask the RM for the device types
;               2 - Use a specific behavior described later
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_ResolveDeviceConnectionsOnLoad, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
LOCALD  LocalDevData
LOCALD  LocalDevData2
LOCALD  BootDevData
LOCALW  wMobile
LOCALV  tvd,NVTWINVIEWDATA
LOCALD  lpTVD
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset
        mov     ax,ss
        shl     eax,10H
        lea     ax,tvd
        mov     lpTVD,eax

        ; This routine returns TRUE if we want mobile device behavior
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetBootDeviceSelection,es,ax
        mov     wMobile,ax

        cmp     dwThisLogicalDeviceHandle_In_NVTEXT16,1
        jne     SecondaryDriver

PrimaryDriver:
        ; See if we want mobile device bahavior
        cmp     wMobile,0
        je      DoneIt

        ; Ask the HW for the boot device
        ; (the HW probably asks the BIOS or RM).
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        pushd   0                       ; get the primary device instance
        call    HW_GetBootDevice
        mov     dword ptr LocalDevData,eax

        ; We only get called for the primary once when it first
        ; loads. Hence, at this point, we may even want to override
        ; what the HW told us is the boot device with a forced boot
        ; device from the registry.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10h
        lea     ax,BootDevData
        push    eax
        CALLFAR16IND lpfnGetBootDevice,es,ax
        or      ax,ax
        je      @F
        mov     eax,BootDevData
        mov     LocalDevData,eax
@@:
        ; If for some reason, the HW didn't return any display
        ; devices, we'll just leave things as is.newu
        cmp     LocalDevData,-1
        je      DoneIt

        ; OK, we know the primary boot display. Write it out
        sub     ecx,ecx
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    ecx                             ; Logical device 0
        push    ecx                             ; Physical Device 0
        mov     bx,ss
        shl     ebx,10H
        lea     bx,LocalDevData
        push    ebx
        CALLFAR16IND lpfnSetDisplayDevice,es,ax
        
        ; If this device is a TV, set the TV Format in the ResMan.
        cmp     byte ptr LocalDevData,DEVTYPE_TV
        jne     SkipTVFormat
        mov     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    ds:[eax].GNCRTCCX.pHWCrtcCX
        push    LocalDevData
        call    HW_SetResManTvFormat
        
SkipTVFormat:
        ; Now, ask the HW if we booted with more than one display
        ; active and, if so, what was the second display device type.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        pushd   1                       ; get the next instance
        call    HW_GetBootDevice
        mov     dword ptr LocalDevData2,eax
        cmp     eax,-1
        je      PrimaryIsStandard

PrimaryIsCloned:
        ; Write the desktop state to clone
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        pushd   NVTWINVIEW_STATE_CLONE
        CALLFAR16IND lpfnSetDesktopState,es,ax

        ; Write out the clone display device type
        sub     ecx,ecx
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    ecx                             ; Logical device 0
        inc     ecx
        push    ecx                             ; Physical Device 1
        mov     bx,ss
        shl     ebx,10H
        lea     bx,LocalDevData2
        push    ebx
        CALLFAR16IND lpfnSetDisplayDevice,es,ax
        jmp     DoneIt

        ; If this device is a TV, set the TV Format in the ResMan.
        cmp     byte ptr LocalDevData,DEVTYPE_TV
        jne     PrimaryIsStandard
        mov     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    ds:[eax].GNCRTCCX.pHWCrtcCX
        push    LocalDevData2
        call    HW_SetResManTvFormat
        
PrimaryIsStandard:
        ; Write the desktop state to clone
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        pushd   NVTWINVIEW_STATE_NORMAL
        CALLFAR16IND lpfnSetDesktopState,es,ax
        jmp     DoneIt


SecondaryDriver:
        ; Write out desktop state to standard. If we have a secondary
        ; display driver loaded, we definitely aren't in clone.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        pushd   NVTWINVIEW_STATE_NORMAL
        CALLFAR16IND lpfnSetDesktopState,es,ax

        ; This routine returns TRUE if we want mobile device behavior
        cmp     wMobile,1
        jc      DoneIt
        cmp     wMobile,2
        je      SpecialBehavior2

SecondDriverAskRm:
        ; Now, ask the HW if we booted with more than one display
        ; active and, if so, what was the second display device type.
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        pushd   1                               ; get the next instance
        call    HW_GetBootDevice
        mov     dword ptr LocalDevData2,eax

        ; If we only booted on a single display device, then
        ; we just leave the registry as is and we will end up
        ; using whatever the registry said was the last display
        ; device used by the secondary driver.
        cmp     eax,-1
        je      DoneIt

        ; If we booted on two display devices, then use the second
        ; one as the secondary device.
        sub     ecx,ecx
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    ecx                             ; Logical device 1
        push    ecx                             ; Physical Device 0
        mov     bx,ss
        shl     ebx,10H
        lea     bx,LocalDevData2
        push    ebx
        CALLFAR16IND lpfnSetDisplayDevice,es,ax
        
        ; If this device is a TV, set the TV Format in the ResMan.
        cmp     byte ptr LocalDevData,DEVTYPE_TV
        jne     DoneIt
        mov     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    ds:[eax].GNCRTCCX.pHWCrtcCX
        push    LocalDevData2
        call    HW_SetResManTvFormat
        
        jmp     DoneIt
        
SpecialBehavior2:

        ; NOTE:
        ;       (N) means Normal (standard mode)
        ;       (C) means clone,                (M) Multi-mon
        ;       In clone mode
        ;       (1) means primary device        (2) means clone device
        ;       In multi-mon
        ;       (P) means primary device        (S) means secondary device
        ;
        ; At this point we are definitely going to multi-mon
        ; (if there are at least two display devices connected).
        ; It is just a question of which display devices we will
        ; be using for the primary and the secondary.
        ;
        ; If currently in this config,  then go to this config
        ; (N) LCD                       (M) LCD (P), CRT (S)
        ; (C) LCD (1), CRT (2)          (M) LCD (P), CRT (S)
        ; (N) CRT                       (M) LCD (P), CRT (S)
        ; (C) LCD (1), TV (2)           (M) LCD (P), TV  (S)
        ; (N) TV                        (M) LCD (P), TV  (S)

        ; Is the primary driver currently in clone mode?
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     edi,dword ptr ds:[ebx].GNBOARDCX.pGNLogdevCX

CheckCurrentPrimary:
        ; Is the current primary the DFP?
        push    dword ptr ds:[edi].GNLOGDEVCX.pGNCrtcCX
        call    GN_GetDevData
        mov     LocalDevData,eax

        ; Remember this ... and assume normal mode
        mov     ebx,ds:[edi].GNLOGDEVCX.pGNBoardCX
        mov     ds:[ebx].GNBOARDCX.dwOldPrimaryDevData,eax
        mov     ds:[ebx].GNBOARDCX.dwOldTwinviewState,NVTWINVIEW_STATE_NORMAL
        mov     ds:[ebx].GNBOARDCX.dwCheckCursorTicksSinceSecondDriverLoaded,0

        cmp     al,DEVTYPE_DFP
        jne     @F

        ; OK, the primary is the DFP. If there is a clone device,
        ; it will determine the device type to set on the second driver.
        ; If there isn't a clone device, the second driver gets a CRT.
        mov     LocalDevData,DEVTYPE_CRT
        mov     eax,dword ptr ds:[edi].GNLOGDEVCX.pGNCrtcCX+4
        or      eax,eax
        je      @F
        push    eax
        call    GN_GetDevData
        mov     LocalDevData,eax

        ; Remember the clone device type and that we were in clone mode
        mov     ebx,ds:[edi].GNLOGDEVCX.pGNBoardCX
        mov     ds:[ebx].GNBOARDCX.dwOldCloneDevData,eax
        mov     ds:[ebx].GNBOARDCX.dwOldTwinviewState,NVTWINVIEW_STATE_CLONE

@@:
        ; First, go to normal mode (if we are already, there, that's fine.)
        mov     tvd.dwFlag,NVTWINVIEW_FLAG_SET
        mov     tvd.dwAction,NVTWINVIEW_ACTION_SETGET_STATE
        mov     tvd.dwState,NVTWINVIEW_STATE_NORMAL

        push    ds:[edi].GNLOGDEVCX.Dibeng.delpPDevice
        pushw   NV_ESC_SETGET_TWINVIEW_DATA
        push    lpTVD
        push    lpTVD
        call    Control

        ; Make the DFP primary (just in case it isn't)
        mov     tvd.dwFlag,NVTWINVIEW_FLAG_SET
        mov     tvd.dwAction,NVTWINVIEW_ACTION_SETGET_DEVICE_TYPE
        mov     tvd.dwCrtc,0
        mov     ecx,DEVTYPE_DFP
        call    HW_MapGNDeviceTypeToHWDeviceType
        mov     tvd.NVTWINVIEWUNION.nvtwdevdata.dwDeviceType,ecx
        mov     tvd.NVTWINVIEWUNION.nvtwdevdata.dwTVFormat,0

        push    ds:[edi].GNLOGDEVCX.Dibeng.delpPDevice
        pushw   NV_ESC_SETGET_TWINVIEW_DATA
        push    lpTVD
        push    lpTVD
        call    Control

        ; Set the physical mode in the GNCRTCCX to the desktop mode
        ; so the physical mode set will be as close as possible to the
        ; desktop mode.
        sub     ebx,ebx
        mov     eax,ds:[edi].GNLOGDEVCX.pGNCrtcCX
        les     bx,ds:[eax].GNCRTCCX.lpCrtcModeInfo
        sub     eax,eax
        mov     ax,ds:[edi].DIBENGINEHDR.deWidth
        mov     es:[ebx].MODEINFO.dwXRes,eax
        mov     ax,ds:[edi].DIBENGINEHDR.deHeight
        mov     es:[ebx].MODEINFO.dwYRes,eax
        sub     ax,ax
        mov     al,ds:[edi].DIBENGINEHDR.deBitsPixel
        mov     es:[ebx].MODEINFO.dwBpp,eax
        
WriteOutType:
        ; If device type is TV, get the TV format from the ResMan.
        cmp     byte ptr LocalDevData,DEVTYPE_TV
        je      @F
        mov     eax,dword ptr ds:[edi].GNLOGDEVCX.pGNCrtcCX[4]
        push    ds:[eax].GNCRTCCX.pHWCrtcCX
        push    dword ptr LocalDevData
        call    HW_GetResManTvFormat
        shl     eax,10h
        and     dword ptr LocalDevData,0ff00ffffh
        or      dword ptr LocalDevData,eax
@@:
        ; Now we can set the display device type. Please note the ResMan
        ; already has the correct TV Format which we just got above.
        mov     ebx,dword ptr ds:[edi].GNLOGDEVCX.pGNCrtcCX
        mov     eax,1
        push    ds:[ebx].GNCRTCCX.lpBoardRegistryData
        push    eax
        dec     eax
        push    eax
        mov     ax,ss
        shl     eax,10h
        lea     ax,LocalDevData
        push    eax
        CALLFAR16IND lpfnSetDisplayDevice,es,ax

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_ResolveDeviceConnectionsOnUnload
;
; Purpose:      This routine gets called just before we attach the
;               first CRTC in a display driver. It gives us a chance
;               to modify what we think the display devices should be.
;
;               UseHwSelectedDevice registry key is interpreted as:
;               0 - use NVidia behavior for establishing device types
;                   when a driver loads (whatever means NVidia wants)
;               1 - Ask the RM for the device types
;               2 - Use a specific behavior described later
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_ResolveDeviceConnectionsOnUnload, PASCAL, FRAME, FAR16
PARMD   pGNLogdevCXOffset
LOCALV  LocalDevData,DEVDATA
LOCALW  wMobile
LOCALV  sModeInfo,MODEINFO
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogdevCXOffset

        ; This routine returns the type of mobile behavior we want
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetBootDeviceSelection,es,ax
        mov     wMobile,ax

        ; Nothing to do if we are unloading the primary driver!
        cmp     dwThisLogicalDeviceHandle_In_NVTEXT16,1
        je      DoneIt

SecondaryDriver:
        cmp     wMobile,2
        je      SpecialBehavior2

        ; Standard mobile behavior is NVidia default behavior
        ; which is to restore the last Clone or standard state
        ; they had before enabling multi-mon.

        ; So, we probably detached a CRTCCX when the logical device
        ; was destroyed. There may be another logical device on this
        ; board that would like to have the CRTCCX. Let's find out.
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        call    GN_FindLogdevWaitingToClone
        or      eax,eax
        je      DoneIt

        ; There was a logical device that wanted a clone! Try to
        ; add a clone to it.
        push    eax
        call    GN_LogdevAddAClone
        or      eax,eax
        je      DoneIt

        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     ecx,dword ptr ds:[eax].GNBOARDCX.pGNLogdevCX[0]
        push    ds:[ecx].GNLOGDEVCX.pHWLogdevCX
        push    ds:[ecx].GNLOGDEVCX.pHWLogdevCX
        call    HW_DriverTwinviewStateChanging

        ; We aren't going to receive a ReEnable call to reset all the
        ; CRTC modes. We must force a refresh here.
        push    esi
        call    GN_CrtcRemapArray
        push    esi
        call    GN_RestoreAllCrtcStates
        jmp     DoneIt

SpecialBehavior2:
        ; Special behavior #2 says that from any multi-mon config,
        ; we should always go back to Clone with LCD on the primary
        ; and CRT as the clone device.

        ; HOWEVER!!!! If we are unloaded very soon (within a second
        ; or two of when we are loaded), then this is probably just
        ; a "FAKE" unload. i.e. When enabling multi-mon, Windows
        ; loads and unloads the display driver a couple times. If
        ; we go back to LCD/CRT clone mode now, and then we instantly
        ; get reloaded, then we will go into LCD/CRT multimon. If
        ; this whole sequence started with LCD/TV clone, then we
        ; end up in the wrong state. Hence, on "FAKE" unloads, we
        ; restore the display device state that existed when we were
        ; loaded.
        mov  edi,ds:[esi].GNLOGDEVCX.pGNBoardCX
        cmp  ds:[edi].GNBOARDCX.dwCheckCursorTicksSinceSecondDriverLoaded,20H
        ja   DriverUnloadIsReal

DriverUnloadIsFake:
        ; Reset the old state.

        ; Reset the primary device type to be what it was
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    ds:[edi].GNBOARDCX.dwOldPrimaryDevData
        call    GN_SetDevData

        ; Were we in clone mode?????
        cmp     ds:[edi].GNBOARDCX.dwOldTwinviewState,NVTWINVIEW_STATE_NORMAL
        je      FinishUp

        ; Add a clone device.
        push    esi
        call    GN_LogdevAddAClone
        or      eax,eax
        je      FinishUp

        ; Make sure the clone device type is whatever it was previously
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX+4
        push    ds:[edi].GNBOARDCX.dwOldCloneDevData
        call    GN_SetDevData

DriverUnloadIsReal:
        ; First, make the primary device the DFP
        mov     LocalDevData.cType,DEVTYPE_DFP
        mov     LocalDevData.cNumber,0
        mov     LocalDevData.cFormat,0

        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    dword ptr LocalDevData
        call    GN_SetDevData

        ; Add a clone device.
        push    esi
        call    GN_LogdevAddAClone
        or      eax,eax
        je      FinishUp

        ; Make sure the clone is the CRT
        mov     LocalDevData.cType,DEVTYPE_CRT
        mov     LocalDevData.cNumber,0
        mov     LocalDevData.cFormat,0
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX+4
        push    dword ptr LocalDevData
        call    GN_SetDevData

        ; Now that we have the correct display device for the clone, we
        ; need to make sure we write the desktop mode to the MODEINFO
        ; structure so we will set the clone as close as possible to
        ; the desktop mode.
        
        ; Build up the desktop MODEINFO
        mov     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX+4
        sub     ebx,ebx
        lfs     bx,ds:[eax].GNCRTCCX.lpCrtcModeInfo
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
        mov     fs:[ebx].MODEINFO.dwXRes,eax
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
        mov     fs:[ebx].MODEINFO.dwYRes,eax
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deBitsPixel
        mov     fs:[ebx].MODEINFO.dwBpp,eax
        
        ; Get the refresh rate from the registry for the CRT as the clone.
        mov     ah,LocalDevData.cFormat
        mov     al,LocalDevData.cNumber
        shl     eax,8
        mov     al,LocalDevData.cType
        mov     sModeInfo.MIDevData,eax
        
        mov     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX+4
        push    ds:[eax].GNCRTCCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        push    eax
        CALLFAR16IND lpfnGetDeviceDisplayMode,es,ax
        sub     ebx,ebx
        lea     bx,sModeInfo
        mov     ebx,ss:[ebx].MODEINFO.dwRefreshRate
        or      ax,ax
        jne     UseRefreshRate

        ; No mode in the registry, use 60Hz as default.
        mov     ebx,60  

UseRefreshRate:
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX+4
        sub     eax,eax
        lfs     ax,ds:[ecx].GNCRTCCX.lpCrtcModeInfo
        mov     fs:[eax].MODEINFO.dwRefreshRate,ebx
        
FinishUp:
        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     ecx,dword ptr ds:[eax].GNBOARDCX.pGNLogdevCX[0]
        push    ds:[ecx].GNLOGDEVCX.pHWLogdevCX
        push    ds:[ecx].GNLOGDEVCX.pHWLogdevCX
        call    HW_DriverTwinviewStateChanging

        ; We aren't going to receive a ReEnable call to reset all the
        ; CRTC modes. We must force a refresh here.
        push    esi
        call    GN_CrtcRemapArray
        push    esi
        call    GN_RestoreAllCrtcStates

DoneIt:
        POPR    esi,edi
CLOSEPROC

CLOSESEG _NVTEXT16

END

