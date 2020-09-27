;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation
;
; File:         gnpanel.asm
;
; Purpose:      This file holds the Control Panel interface as
;               implemented through the display driver escapes.
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
include valmode.inc
include escape.inc
include tvout.inc
include modeext.inc

IF      ((MAX_DISP_CONFIG * 4) LT (SIZE NVTWINVIEW_DISPLAY_CONFIG))
        echo MAX_DISP_CONFIG is less than NVTWINVIEW_DISPLAY_CONFIG
        echo Make MAX_DISP_CONFIG in gnlogdev.h bigger
        .err
ENDIF

CONTROL_FRAME_AND_LOCALS        MACRO
PARMD   pGNLogdevCXOffset
PARMD   lpDstDev
PARMW   wFunction
PARMD   lpIn
PARMD   lpOut
LOCALD  dwRequestedDeviceType
LOCALD  dwRequestedTVFormat
LOCALD  dwSizeEdidBuffer
LOCALS  sLocalEdidBuffer,512
LOCALD  dwCRTCCount
LOCALS  LocalDevData,MAX_ATTACHED_CRTCS*4
LOCALD  lpLocalDevData
LOCALS  aHWCrtcCX,MAX_ATTACHED_CRTCS*4
LOCALD  lpHWCrtcCX
LOCALS  aGNCrtcCX,MAX_ATTACHED_CRTCS*4
LOCALD  lpGNCrtcCX
LOCALD  dwDeviceCount
LOCALD  dwDevicesChanged
LOCALD  OldCrtcDevData
LOCALD  dwDevDataMask
LOCALD  dwMaxMonitors
LOCALD  dwMonitorNum
LOCALD  dwLocalDeviceMask
LOCALD  pGNLogdevCXUsingTheHead
LOCALD  pGNCrtcCXUsingTheHead
LOCALD  pHWCrtcCXUsingTheHead
LOCALD  dwLogCrtcIndex
LOCALD  lpDesktopModeInfo
LOCALD  lpModeInfo
LOCALV  sDesktopModeInfo,MODEINFO
LOCALV  sModeInfo,MODEINFO
LOCALV  sModeOut,MODEOUT
LOCALV  sSettableModeInfo,MODEINFO
LOCALS  aGammaTable,400H
LOCALW  wClone
ENDM

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     ControlPanelInterface
;
; Purpose:      This function is the Control Panel interface to the
;               display driver. We will only get here if a Control
;               panel escape is issued to the display driver.
;
; Arguments:    Control Display Driver Stack Frame
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC ControlPanelInterface, PASCAL, FRAME, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        PUSHR   ds,esi,edi

        sub     ecx,ecx
        movzx   eax,wFunction
        cmp     eax,NV_ESC_SETGET_TWINVIEW_DATA
        jne     Done

        push    OFFSET ReturnAddr
        mov     esi,pGNLogdevCXOffset
        sub     edi,edi
        les     di,lpOut
        mov     eax,es:[edi].NVTWINVIEWDATA.dwAction

        cmp     eax,NVTWINVIEW_ACTION_SETGET_STATE
        je      NV_SetGet_State
        cmp     eax,NVTWINVIEW_ACTION_SETGET_DEVICE_TYPE
        je      NV_SetGet_Device_Type
        cmp     eax,NVTWINVIEW_ACTION_SETGET_VIRTUALDESKTOP
        je      NV_SetGet_VirtualDesktop
        cmp     eax,NVTWINVIEW_ACTION_SETGET_FREEZE_ORIGIN
        je      NV_SetGet_Freeze_Origin
        cmp     eax,NVTWINVIEW_ACTION_SETGET_VIRTUAL_MODE
        je      NV_SetGet_VirtualMode
        cmp     eax,NVTWINVIEW_ACTION_SETGET_PHYSICAL_MODE
        je      NV_SetGet_PhysicalMode
        cmp     eax,NVTWINVIEW_ACTION_VALIDATE_VIRTUAL_MODE
        je      NV_ValidateVirtualMode
        cmp     eax,NVTWINVIEW_ACTION_VALIDATE_PHYSICAL_MODE
        je      NV_ValidatePhysicalMode
        cmp     eax,NVTWINVIEW_ACTION_SETGET_TIMING_OVERRIDE
        je      NV_SetGet_Timing_Override
        cmp     eax,NVTWINVIEW_ACTION_SETGET_CLONE_GAMMA
        je      NV_SetGetCloneGamma
        cmp     eax,NVTWINVIEW_ACTION_ROTATE_CLONE_DEVICES
        je      GN_RotateCloneDevices
        cmp     eax,NVTWINVIEW_ACTION_LOCK_HWCRTC
        je      GN_LockHWCrtcOnHead
        cmp     eax,NVTWINVIEW_ACTION_UNLOCK_HWCRTC
        je      GN_UnlockHWCrtcOnHead
        cmp     eax,NVTWINVIEW_ACTION_SETGET_PHYSICAL_MODE_IMMEDIATE
        je      GN_SpecialSetPhysicalMode
        cmp     eax,NVTWINVIEW_ACTION_MAP_LOG_TO_PHYS_HEAD
        je      GN_MapLogToPhysHead
        cmp     eax,NVTWINVIEW_ACTION_SETGET_DISPLAY_CONFIG
        je      GN_SetGetDisplayConfig
        cmp     eax,NVTWINVIEW_ACTION_GET_LARGEST_EDID_MODE
        je      GN_GetBiggestEdidMode

        add     sp,@WordSize
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,0
        sub     ecx,ecx
        jmp     Done

ReturnAddr:
Done:
        mov     eax,1
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     NV_SetGet_State
;
; Purpose:      This function gets or sets the desktop state.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;               es:edi          NVTWINVIEWDATA
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:
;==============================================================================
DECPROC NV_SetGet_State, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     ax,ss
        shl     eax,10H
        lea     ax,LocalDevData
        mov     lpLocalDevData,eax
        lea     ax,aHWCrtcCX
        mov     lpHWCrtcCX,eax

        sub     edi,edi
        les     di,lpOut

        ; Let's get the current state first!
        mov     eax,NVTWINVIEW_STATE_CLONE
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,1
        ja      @F
        mov     eax,NVTWINVIEW_STATE_MULTIMON
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        cmp     ds:[ebx].GNBOARDCX.dwLogicalDeviceInstancesOnThisBoard,1
        ja      @F
        mov     eax,NVTWINVIEW_STATE_NORMAL
@@:
        mov     edx,es:[edi].NVTWINVIEWDATA.dwFlag
        cmp     edx,NVTWINVIEW_FLAG_GET
        jne     SetState

GetState:
        ; We got it and we're done!
        mov     es:[edi].NVTWINVIEWDATA.dwState,eax
        mov     eax,1
        jmp     Done

SetState:
        cmp     eax,NVTWINVIEW_STATE_NORMAL
        je      CurrentNormal
        cmp     eax,NVTWINVIEW_STATE_CLONE
        je      CurrentClone
        mov     eax,NVTWINVIEW_STATE_CHANGE_FAILED
        jmp     Done

CurrentNormal:
        cmp     es:[di].NVTWINVIEWDATA.dwState,NVTWINVIEW_STATE_NORMAL
        je      NoChange

        ; OK, normal->clone means we attach a crtc
        ; The caller will issue a ChangeDisplaySettings after we return
        push    esi
        call    GN_LogdevAddAClone
        mov     eax,NVTWINVIEW_STATE_CLONE
        mov     ds:[esi].GNCRTCCX.dwDisplayDeviceChange,1
        jmp     WriteItOut

CurrentClone:
        cmp     es:[di].NVTWINVIEWDATA.dwState,NVTWINVIEW_STATE_CLONE
        je      NoChange

        ; Clear panning flag to prevent the next clone limitation
        mov     edi,1
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]

        push    ds:[ebx].GNCRTCCX.lpBoardRegistryData
        push    ds:[ebx].GNCRTCCX.lpCrtcModeInfo
        pushd   0
        CALLFAR16IND lpfnSetClonePanningFlag,es,ax

        ; Save last mode for this device
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    ds:[ebx].GNCRTCCX.lpBoardRegistryData
        push    ds:[ebx].GNCRTCCX.lpCrtcModeInfo
        CALLFAR16IND lpfnSetDeviceDisplayMode,es,ax

        ; Clone->Normal means we detach a CRTCCX.
        ; The caller will issue a ChangeDisplaySettings after we return
        push    esi
        push    edi
        call    GN_LogdevDetachCrtc
        mov     eax,NVTWINVIEW_STATE_NORMAL

WriteItOut:
        ; The change succeeded -- write out the new state
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    eax
        CALLFAR16IND lpfnSetDesktopState,es,ax
        
        mov     ds:[esi].GNLOGDEVCX.dwNvDisplayChangePending,1

NoChange:
        mov     eax,NVTWINVIEW_STATE_CHANGE_DONE

Done:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_SetGet_Device_Type
;
; Purpose:      This function is the Control Panel interface to the
;               display driver. We will only get here if a Control
;               panel escape is issued to the display driver.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;               es:edi          NVTWINVIEWDATA
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:
;==============================================================================
DECPROC NV_SetGet_Device_Type, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     eax,eax

        ; Determine the correct CRTC to address
        mov     edx,es:[edi].NVTWINVIEWDATA.dwCrtc
        cmp     edx,ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        jae     WriteRegistry

        ; Get the CRTC context ptr
        lea     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     esi,ds:[ecx][edx * 4]
        or      esi,esi
        je      DoneIt

        mov     edx,es:[edi].NVTWINVIEWDATA.dwFlag
        cmp     edx,NVTWINVIEW_FLAG_GET
        jne     NV_SetDisplayDeviceType
        jmp     NV_GetDisplayDeviceType

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;IDEBUG;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; HACK!!!
; JKTODO - Jim, Sorry about this, but it was the only solution I could
;          come up with given the current time constraints. We can talk
;          about this further.
WriteRegistry:
        ; If we are in Normal Mode, and a request is coming in to write
        ; logical head 1 then write the registry with the new display
        ; device to LogicalDevice0 PhysicalDevice1.
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,1
        ja      DoneIt
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        cmp     ds:[ebx].GNBOARDCX.dwLogicalDeviceInstancesOnThisBoard,1
        ja      DoneIt
        
        ; We are in normal mode. Check if we are being asked to write
        ; logical device 1.
        cmp     es:[edi].NVTWINVIEWDATA.dwCrtc,1
        jne     DoneIt

        ; Get the type of device we are pointing to
        ; and convert it to the DEVTYPE_XXX type list.
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceType
        call    HW_MapHWDeviceTypeToGNDeviceType
        mov     (DEVDATA PTR LocalDevData).cType,cl

        ; Map the RM TvFormat to the NVmode Tvformat
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwTVFormat
        call    HW_MapHWTvFormatToGNTvFormat
        mov     (DEVDATA PTR LocalDevData).cFormat,cl

        ; Assume device number 0 for now
        mov     (DEVDATA PTR LocalDevData).cNumber,0

        ; Write this out to the registry.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        mov     eax,0
        push    eax
        inc     eax
        push    eax
        mov     ax,ss
        shl     eax,10h
        lea     ax,LocalDevData
        push    eax
        CALLFAR16IND lpfnSetDisplayDevice,es,ax

        mov     eax,1                   ; return success
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   

DoneIt:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_SetDisplayDeviceType
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is NV_ESC_SET_DISPLAY_DEVICE_TYPE.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNCRTCCX
;               es:edi          NVTWINVIEWDATA
;
; Returns:      ecx             1
;               eax             1
;
; Preserve:
;==============================================================================
DECPROC NV_SetDisplayDeviceType, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     ax,ss
        shl     eax,10H
        lea     ax,LocalDevData
        mov     lpLocalDevData,eax
        lea     ax,aHWCrtcCX
        mov     lpHWCrtcCX,eax
        lea     ax,aGNCrtcCX
        mov     lpGNCrtcCX,eax

        ; Get the type of device we are pointing to
        ; and convert it to the DEVTYPE_XXX type list.
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceType
        call    HW_MapHWDeviceTypeToGNDeviceType
        mov     (DEVDATA PTR LocalDevData).cType,cl
        cmp     cl,DEVTYPE_TV
        mov     cl,0
        jne     @F

        ; Map the RM TvFormat to the NVmode Tvformat
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwTVFormat
        call    HW_MapHWTvFormatToGNTvFormat
@@:
        mov     (DEVDATA PTR LocalDevData).cFormat,cl

        ; Get the device number.
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceMask
        sub     ecx,ecx

Find_Device_Number:     
        test    eax,1
        jz      @F
        mov     ch,cl
        and     ch,07H
        jmp     Got_Device_number
        
@@:     inc     cl
        shr     eax,1
        cmp     cl,18H
        jc      Find_Device_Number
        
        mov     ch,0
        
Got_Device_number:
        mov     (DEVDATA PTR LocalDevData).cNumber,ch

        ; Save off the old device data
        push    esi
        call    GN_GetDevData
        mov     OldCrtcDevData,eax

        ; Temporarily set the new device
        push    esi
        push    dword ptr LocalDevData
        call    GN_SetDevData

        mov     edi,pGNLogdevCXOffset

        ; Make sure that the device desired by this CRTC is not
        ; in use by any other active CRTCCX on the board.
        push    ds:[edi].GNLOGDEVCX.pGNBoardCX
        push    esi
        call    GN_IsDevDataUniqueOnBoard
        or      eax,eax
        je      DeviceInUse

        ; OK, sure it's unique, but is the device connectable to some
        ; head? (any head - it doesn't matter right now which one.)
        mov     ecx,ds:[edi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ecx].GNBOARDCX.pHWBoardCX
        push    dword ptr LocalDevData
        call    HW_IsDeviceAttached
        or      eax,eax
        je      DeviceNotAttached

        ; OK, we have a unique device (from the others in use)
        ; that is connected to the system. Now can we find an
        ; arrangment whereby this device can be used?
        push    ds:[edi].GNLOGDEVCX.pGNBoardCX
        push    lpLocalDevData
        push    lpGNCrtcCX
        push    lpHWCrtcCX
        pushd   MAX_ATTACHED_CRTCS
        call    GN_GetAllDeviceData
        mov     dwDeviceCount,eax

        ; Ask the hardware if it is possible to have all the
        ; devices specified enabled simultaneously by assigning
        ; each to a different head.
        mov     ecx,ds:[edi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ecx].GNBOARDCX.pHWBoardCX
        push    lpLocalDevData
        push    dwDeviceCount
        call    HW_CanDevicesBeMapped
        or      eax,eax
        je      AssignmentNotPossible

        ; Don't bother and try to read the edid on a tv
        mov     ds:[esi].GNCRTCCX.dwEdidSize,0
        cmp     (DEVDATA PTR LocalDevData).cType,DEVTYPE_TV
        je      DoneEdidRead

        ; Read the EDID for this device if it has an EDID
        mov     ds:[esi].GNCRTCCX.dwEdidSize,MAX_EDID_BUFFER_SIZE
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        push    dword ptr LocalDevData
        mov     ax,ds
        push    eax
        call    GN_GetSelectorBase
        lea     ecx,ds:[esi].GNCRTCCX.sEdidBuffer
        add     ecx,eax
        push    ecx
        lea     ecx,ds:[esi].GNCRTCCX.dwEdidSize
        add     ecx,eax
        push    ecx
        call    HW_ReadEdid

        ; Set a flag in this CRTCCX that this is a display device
        ; change
        mov     ds:[esi].GNCRTCCX.dwDisplayDeviceChange,1

DoneEdidRead:
        ; The caller will issue a ChangeDisplaySettings after we return
        mov     eax,1
        jmp     DoneIt

AssignmentNotPossible:
DeviceNotAttached:
DeviceInUse:
        ; Restore the old dev data
        push    esi
        push    dword ptr OldCrtcDevData
        call    GN_SetDevData
        sub     eax,eax

DoneIt:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetDisplayDeviceType
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is NV_ESC_GET_DISPLAY_DEVICE_TYPE.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNCRTCCX
;               es:edi          NVTWINVIEWDATA
;
; Returns:      eax             1
;               ecx             1
;
; Preserve:
;==============================================================================
DECPROC NV_GetDisplayDeviceType, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        lea     ebx,[esi].GNCRTCCX.sCrtcModeInfo
        movzx   ecx,(DEVDATA PTR ds:[ebx].MODEINFO.MIDevData).cType
        call    HW_MapGNDeviceTypeToHWDeviceType
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceType,ecx
        lea     ebx,[esi].GNCRTCCX.sCrtcModeInfo
        movzx   ecx,(DEVDATA PTR ds:[ebx].MODEINFO.MIDevData).cFormat
        call    HW_MapGNTvFormatToHWTvFormat
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwTVFormat,ecx

        ; Create the bitmask for the display type and number.
        lea     ebx,[esi].GNCRTCCX.sCrtcModeInfo
        mov     cl,(DEVDATA PTR ds:[ebx].MODEINFO.MIDevData).cType
        shl     cl,3
        mov     eax,1
        shl     eax,cl
        mov     cl,(DEVDATA PTR ds:[ebx].MODEINFO.MIDevData).cNumber
        shl     eax,cl
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceMask,eax

        mov     eax,1
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_SetGet_VirtualDesktop
;
; Purpose:      This function is for setting or getting the state of the
;               virtual desktop. Panning is only possible if the virtual
;               desktop is enabled.
;
; Arguments:    Control Display Driver Stack Frame
;               ds      Context selector
;               esi     GNLOGDEVCX ptr
;               es:edi  NVTWINVIEWDATA
;
; Returns:      ecx     1
;               eax     0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_SetGet_VirtualDesktop, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     edx,es:[edi].NVTWINVIEWDATA.dwFlag
        cmp     edx,NVTWINVIEW_FLAG_GET
        jne     SetAutoPan

        mov     eax,ds:[esi].GNLOGDEVCX.dwVirtualDesktopEnabled
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwvirtualdesktopdata.dwVirtualDesktop,eax
        jmp     Done

SetAutoPan:
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwvirtualdesktopdata.dwVirtualDesktop
        mov     ds:[esi].GNLOGDEVCX.dwVirtualDesktopEnabled,eax

        ; Write this out to the registry.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        push    ds:[esi].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard
        push    eax
        CALLFAR16IND lpfnSetVirtualDesktop,es,ax
Done:
        sub     edi,edi
        les     di,lpOut
        mov     eax,1
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_SetGet_VirtualMode
;
; Purpose:      This function writes/reads the virtual mode for the
;               desktop to/from the registry.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               es:edi          NVTWINVIEWDATA
;               esi             GNLOGDEVCX ptr
;
; Returns:      ecx     1
;               eax     0 failure
;                       1 success
;
; Preserve:
;==============================================================================
DECPROC NV_SetGet_VirtualMode, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; I need to do this regarless of get or set
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceType
        call    HW_MapHWDeviceTypeToGNDeviceType
        mov     (DEVDATA PTR sModeInfo.MIDevData).cType,cl
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwTVFormat
        call    HW_MapHWTvFormatToGNTvFormat
        mov     (DEVDATA PTR sModeInfo.MIDevData).cFormat,cl

        ; Get the device number.
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceMask
        sub     ecx,ecx

Find_Device_Number:     
        test    eax,1
        jz      @F
        mov     ch,cl
        and     ch,07H
        jmp     Got_Device_number
        
@@:     inc     cl
        shr     eax,1
        cmp     cl,18H
        jc      Find_Device_Number
        
        mov     ch,0
        
Got_Device_number:
        mov     (DEVDATA PTR sModeInfo.MIDevData).cNumber,ch

        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        CALLFAR16IND lpfnGetCloneDesktopSelection,es,ax
        mov     wClone,ax

        sub     edi,edi
        les     di,lpOut

        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     edx,es:[edi].NVTWINVIEWDATA.dwFlag
        cmp     edx,NVTWINVIEW_FLAG_GET
        je      GetVirt

SetVirt:
        ; Get the Twinview data into a MODEINFO structure
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes
        mov     sModeInfo.dwXRes,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes
        mov     sModeInfo.dwYRes,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp
        mov     sModeInfo.dwBpp,eax

        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        push    eax
        cmp     wClone,1
        mov     eax,0
        jne     @F
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,2
        jl      @F
        mov     eax,1
@@:     push    eax     
        CALLFAR16IND lpfnSetDeviceDesktopMode,es,ax
        jmp     DoneIt

GetVirt:
        ; What device is he asking about?
        mov     eax,dword ptr sModeInfo.MIDevData
        lea     ecx,[ebx].GNCRTCCX.sCrtcModeInfo
        xor     eax,ds:[ecx].MODEINFO.MIDevData
        and     eax,0FFFFFFFFH
        jne     NotCurrent

UseCurrent:
        ; He is asking about the current device
        sub     edi,edi
        les     di,lpOut
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes,eax
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes,eax
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deBitsPixel
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp,eax
        jmp     DoneIt

NotCurrent:
        ; Read the registry for the virtual desktops of devices that
        ; are not currently active
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        push    eax
        cmp     wClone,1
        mov     eax,0
        jne     @F
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,2
        jl      @F
        mov     eax,1
@@:     push    eax     
        CALLFAR16IND lpfnGetDeviceDesktopMode,es,ax
        or      ax,ax
        je      UseCurrent

        ; Copy the data back from our MODEINFO to the Twinview struct
        sub     edi,edi
        les     di,lpOut
        mov     eax,sModeInfo.dwXRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes,eax
        mov     eax,sModeInfo.dwYRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes,eax
        mov     eax,sModeInfo.dwBpp
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp,eax

DoneIt:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_SetGet_PhysicalMode
;
; Purpose:      This function writes/reads the physical mode for a
;               specific CRTC to/from the registry.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               es:edi          NVTWINVIEWDATA
;               esi             GNLOGDEVCX ptr
;
; Returns:      ecx     1
;               eax     0 failure
;                       1 success
;
; Preserve:
;==============================================================================
DECPROC NV_SetGet_PhysicalMode, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Determine the correct CRTC to address
        sub     eax,eax
        mov     edx,es:[edi].NVTWINVIEWDATA.dwCrtc
        cmp     edx,ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        jae     DoneIt

        ; Get the CRTC context ptr
        lea     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     esi,ds:[ebx][edx * 4]
        or      esi,esi
        je      DoneIt

        ; I need to do this regardles of set or get
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceType
        call    HW_MapHWDeviceTypeToGNDeviceType
        mov     (DEVDATA PTR sModeInfo.MIDevData).cType,cl
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwTVFormat
        call    HW_MapHWTvFormatToGNTvFormat
        mov     (DEVDATA PTR sModeInfo.MIDevData).cFormat,cl

        ; Get the device number.
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceMask
        sub     ecx,ecx

Find_Device_Number:     
        test    eax,1
        jz      @F
        mov     ch,cl
        and     ch,07H
        jmp     Got_Device_number
        
@@:     inc     cl
        shr     eax,1
        cmp     cl,18H
        jc      Find_Device_Number
        
        mov     ch,0
        
Got_Device_number:
        mov     (DEVDATA PTR sModeInfo.MIDevData).cNumber,ch

        mov     edx,es:[edi].NVTWINVIEWDATA.dwFlag
        cmp     edx,NVTWINVIEW_FLAG_GET
        je      GetPhys

SetPhys:
        ; Get the Twinview data into a MODEINFO structure
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes
        mov     sModeInfo.dwXRes,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes
        mov     sModeInfo.dwYRes,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp
        mov     sModeInfo.dwBpp,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh
        mov     sModeInfo.dwRefreshRate,eax

        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        push    eax
        CALLFAR16IND lpfnSetDeviceDisplayMode,es,ax

        cmp     ds:[esi].GNCRTCCX.dwPhysicalDeviceInstance,0
        je      DoneIt

        ; Set panning flag for the clone device
        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        push    eax
        pushd   1
        CALLFAR16IND lpfnSetClonePanningFlag,es,ax
        jmp     DoneIt

GetPhys:
        ; What device is he asking about?
        movzx   eax,(DEVDATA PTR sModeInfo.MIDevData).cType
        lea     ecx,[esi].GNCRTCCX.sCrtcModeInfo
        xor     eax,ds:[ecx].MODEINFO.MIDevData
        and     eax,0FFFFFFH
        jne     NotCurrent

UseCurrent:
        ; He is asking about the current device
        sub     edi,edi
        les     di,lpOut
        sub     ebx,ebx
        lfs     bx,ds:[esi].GNCRTCCX.lpCrtcModeInfo
        mov     eax,fs:[ebx].MODEINFO.dwXRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes,eax
        mov     eax,fs:[ebx].MODEINFO.dwYRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes,eax
        mov     eax,fs:[ebx].MODEINFO.dwBpp
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp,eax
        mov     eax,fs:[ebx].MODEINFO.dwRefreshRate
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh,eax
        jmp     DoneIt

UseDefault:
        sub     edi,edi
        les     di,lpOut
        mov     eax,640                 ;XRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes,eax
        mov     eax,480                 ;YRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes,eax
        mov     eax,10H                 ;bpp
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp,eax
        mov     eax,60                  ;RefreshRate
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh,eax
        jmp     DoneIt

NotCurrent:
        ; Read the registry for the virtual desktops of devices that
        ; are not currently active
        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        push    eax
        CALLFAR16IND lpfnGetDeviceDisplayMode,es,ax
        or      ax,ax
        je      UseDefault

        ; Copy the data back from our MODEINFO to the Twinview struct
        sub     edi,edi
        les     di,lpOut
        mov     eax,sModeInfo.dwXRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes,eax
        mov     eax,sModeInfo.dwYRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes,eax
        mov     eax,sModeInfo.dwBpp
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp,eax
        mov     eax,sModeInfo.dwRefreshRate
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh,eax

DoneIt:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_ValidateVirtualMode
;
; Purpose:      This function validates a virtual mode for the desktop.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               es:edi          NVTWINVIEWDATA
;               esi             GNLOGDEVCX ptr
;
; Returns:      ecx     1
;               eax     0 failure
;                       1 success
;
; Preserve:
;==============================================================================
DECPROC NV_ValidateVirtualMode, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Load up our context
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes
        push    es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes
        push    es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp
        call    HW_GetModeDimensions

        ; Get the largest block size that we could put this mode in
        push    ebx
        push    esi
        call    GN_GetFreeBlockSizeForMode
        pop     ebx

        ; Is it enough?
        cmp     eax,ebx
        jl      Invalid

Valid:
        mov     eax,1
        jmp     Done

Invalid:
        sub     eax,eax

Done:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_ValidatePhysicalMode
;
; Purpose:      This function validates the physical mode for the
;               desktop.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               es:edi          NVTWINVIEWDATA
;               esi             GNLOGDEVCX ptr
;
; Returns:      ecx     1
;               eax     0 failure
;                       1 success
;
; Preserve:
;==============================================================================
DECPROC NV_ValidatePhysicalMode, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Determine the correct CRTC to address
        sub     eax,eax
        mov     edx,es:[edi].NVTWINVIEWDATA.dwCrtc
        cmp     edx,ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        jae     Done

        ; Get the CRTC context ptr
        lea     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     esi,ds:[ebx][edx * 4]
        or      esi,esi
        je      Done

        ; Get the type of device we are validating the mode on
        ; and convert it to the DEVTYPE_XXX type list.
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceType
        call    HW_MapHWDeviceTypeToGNDeviceType
        mov     (DEVDATA PTR LocalDevData).cType,cl
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwTVFormat
        call    HW_MapHWTvFormatToGNTvFormat
        mov     (DEVDATA PTR LocalDevData).cFormat,cl

        ; Get the device number.
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceMask
        sub     ecx,ecx

Find_Device_Number:     
        test    eax,1
        jz      @F
        mov     ch,cl
        and     ch,07H
        jmp     Got_Device_number
        
@@:     inc     cl
        shr     eax,1
        cmp     cl,18H
        jc      Find_Device_Number
        
        mov     ch,0
        
Got_Device_number:
        mov     (DEVDATA PTR sModeInfo.MIDevData).cNumber,ch
        mov     (DEVDATA PTR LocalDevData).cNumber,ch

        ; Don't bother and try to read the edid on a tv
        mov     dwSizeEdidBuffer,0
        cmp     (DEVDATA PTR LocalDevData).cType,DEVTYPE_TV
        je      DoneEdidRead

        ; Read the EDID for this device if it has an EDID
        mov     dwSizeEdidBuffer,MAX_EDID_BUFFER_SIZE
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        push    dword ptr LocalDevData
        mov     ax,ss
        push    eax
        call    GN_GetSelectorBase
        lea     ecx,sLocalEdidBuffer
        add     ecx,eax
        push    ecx
        lea     ecx,dwSizeEdidBuffer
        add     ecx,eax
        push    ecx
        call    HW_ReadEdid

DoneEdidRead:
        ; Get everything into a MODEINFO structure
        sub     edi,edi
        les     di,lpOut
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes
        mov     sModeInfo.dwXRes,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes
        mov     sModeInfo.dwYRes,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp
        mov     sModeInfo.dwBpp,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh
        mov     sModeInfo.dwRefreshRate,eax
        mov     eax,dword ptr LocalDevData
        mov     sModeInfo.MIDevData,eax

        ; Validate the mode.
        push    esi
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        push    eax
        lfs     bx,ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    fs:[bx].DISPDATA.dwMonitorInfType
        mov     ax,ss
        shl     eax,10H
        lea     ax,sLocalEdidBuffer
        push    eax
        push    dwSizeEdidBuffer
        call    GN_ValidatePhysicalMode
        mov     edx,eax

        ; Otherwise, we need an exact match
        mov     eax,1
        cmp     edx,MODE_EXACT_MATCH
        je      Done

        ; Fail, but copy back the best match to Twinview
        sub     edi,edi
        les     di,lpOut
        mov     eax,sModeInfo.dwXRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes,eax
        mov     eax,sModeInfo.dwYRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes,eax
        mov     eax,sModeInfo.dwBpp
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp,eax
        mov     eax,sModeInfo.dwRefreshRate
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh,eax
        sub     eax,eax

Done:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_SetGet_Timing_Override
;
; Purpose:      This function gets or sets the timing override.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;               es:edi          NVTWINVIEWDATA
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:
;==============================================================================
DECPROC NV_SetGet_Timing_Override, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Determine the correct CRTC to address
        sub     eax,eax
        mov     edx,es:[edi].NVTWINVIEWDATA.dwCrtc
        ; See we are in clone or extended mode
        cmp     ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice,1
        jg      @F
        sub     edx,edx
@@:
        cmp     edx,ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        jae     Done

        ; Get the CRTC context ptr
        lea     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     esi,ds:[ebx][edx * 4]
        or      esi,esi
        je      Done

        mov     edx,es:[edi].NVTWINVIEWDATA.dwFlag
        cmp     edx,NVTWINVIEW_FLAG_GET
        jne     SetTimingOverride

GetTimingOverride:
        ; Read the registry for the virtual desktops of devices that
        ; are not currently active
        lfs     bx,ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        push    fs:[bx].DISPDATA.lpDevData
        mov     ax,es
        shl     eax,10h
        lea     ax,es:[di].NVTWINVIEWDATA.dwState
        push    eax
        CALLFAR16IND lpfnGetTimingOverride,es,ax
        movzx   eax,ax
        or      ax,ax
        jmp     Done

SetTimingOverride:
        ; Write this out to the registry.
        lfs     bx,ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        push    fs:[bx].DISPDATA.lpDevData
        push    es:[di].NVTWINVIEWDATA.dwState
        CALLFAR16IND lpfnSetTimingOverride,es,ax
        mov     eax,1
Done:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_SetGet_Freeze_Origin
;
; Purpose:      This function is the Control Panel interface to the
;               display driver. We will only get here if a Control
;               panel escape is issued to the display driver.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               es:edi          NVTWINVIEWDATA
;               esi             GNLOGDEVCX ptr
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_SetGet_Freeze_Origin, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Determine the correct CRTC to address
        sub     eax,eax
        mov     edx,es:[edi].NVTWINVIEWDATA.dwCrtc
        cmp     edx,ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        jae     Done

        ; Get the CRTC context ptr
        lea     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     esi,ds:[ebx][edx * 4]
        or      esi,esi
        je      Done

        mov     edx,es:[edi].NVTWINVIEWDATA.dwFlag
        cmp     edx,NVTWINVIEW_FLAG_GET
        jne     SetFreeze

        ; dwPanAndScanEnabled has the opposite sense of dwFreezeOrigin.
        mov     eax,ds:[esi].GNCRTCCX.dwPanAndScanEnabled
        cmp     eax,0
        mov     eax,1
        je      WriteData
        mov     eax,0
WriteData:
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwfreezedata.dwFreezeOrigin,eax
        jmp     Done

SetFreeze:
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwfreezedata.dwFreezeOrigin
        cmp     eax,0
        je      Unfreeze

        mov     ds:[esi].GNCRTCCX.dwPanAndScanEnabled,0

        ; Write this out to the registry.
        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        lfs     bx,ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    fs:[bx].DISPDATA.lpDevData
        push    ds:[esi].GNCRTCCX.rcPhysicalViewport.dwTop
        push    ds:[esi].GNCRTCCX.rcPhysicalViewport.dwLeft
        push    ds:[esi].GNCRTCCX.rcPhysicalViewport.dwBottom
        push    ds:[esi].GNCRTCCX.rcPhysicalViewport.dwRight
        push    eax
        CALLFAR16IND lpfnSetAutoPanMode,es,ax

        jmp     Done

Unfreeze:
        mov     ds:[esi].GNCRTCCX.dwPanAndScanEnabled,1

        ; Write this out to the registry.
        push    ds:[esi].GNCRTCCX.lpBoardRegistryData
        lfs     bx,ds:[esi].GNCRTCCX.lpNvmodeDispData
        push    fs:[bx].DISPDATA.lpDevData
        push    ds:[esi].GNCRTCCX.rcPhysicalViewport.dwTop
        push    ds:[esi].GNCRTCCX.rcPhysicalViewport.dwLeft
        push    ds:[esi].GNCRTCCX.rcPhysicalViewport.dwBottom
        push    ds:[esi].GNCRTCCX.rcPhysicalViewport.dwRight
        push    eax
        CALLFAR16IND lpfnSetAutoPanMode,es,ax

Done:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1

CLOSEPROC


;==============================================================================
;
; Function:     NV_SetGetCloneGamma
;
; Purpose:      This function is the Control Panel interface to the
;               display driver. We will only get here if a Control
;               panel escape is issued to the display driver.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               es:edi          NVTWINVIEWDATA
;               esi             GNLOGDEVCX ptr
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_SetGetCloneGamma, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Determine the correct CRTC to address
        sub     eax,eax
        mov     edx,es:[edi].NVTWINVIEWDATA.dwCrtc
        cmp     edx,ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        jae     EndIt

        ; Get the CRTC context ptr
        lea     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     esi,ds:[ebx][edx * 4]
        or      esi,esi
        je      EndIt

        mov     edx,es:[edi].NVTWINVIEWDATA.dwFlag
        cmp     edx,NVTWINVIEW_FLAG_GET
        jne     SetGamma

GetGamma:
        lea     esi,[esi].GNCRTCCX.dwGammaTable
        lea     edi,[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.adwGammaTable
        mov     ecx,100H
        rep     movs dword ptr es:[edi], ds:[esi]
        mov     eax,1
        jmp     EndIt

SetGamma:
        PUSHR   ds
        mov     ax,ds
        mov     cx,es
        mov     es,ax
        mov     ds,cx
        mov     ebx,esi
        lea     esi,[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.adwGammaTable
        lea     edi,[ebx].GNCRTCCX.dwGammaTable
        mov     ecx,100H
        rep     movs dword ptr es:[edi], ds:[esi]
        POPR    ds

        ; Now put it into the hardware
        push    ebx
        call    GN_ResetColorMap
        mov     eax,1

EndIt:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_RotateCloneDevices
;
; Purpose:      This function is the Control Panel interface to the
;               display driver. We will only get here if a Control
;               panel escape is issued to the display driver.
;
;               This escape will take the display device the primary
;               is using and use that device for the first clone. It
;               will take the display device used by the first clone
;               and make the primary use it. For a dual head board
;               this functions as a "Swap display devices". For more
;               than a two head board, it is a "rotate display devices."
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               es:edi          NVTWINVIEWDATA
;               esi             GNLOGDEVCX ptr
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_RotateCloneDevices, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     edi,edi
        mov     dwDeviceCount,edi

        ; Get the primary CRTC dislay device
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        call    GN_GetDevData
        mov     OldCrtcDevData,eax
        jmp     NextCrtc

TopL:
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        or      ecx,ecx
        je      NextCrtc

        ; Aha! Found another one. Get it's DevData
        push    ecx
        call    GN_GetDevData
        push    eax

        ; Set the prior CRTCCX's DevData into this one
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    OldCrtcDevData
        call    GN_SetDevData

        ; Now save this guys old one as the next one to rotate in
        pop     OldCrtcDevData

        ; We actually rotated one
        inc     dwDeviceCount

NextCrtc:
        inc     edi
        cmp     edi,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jb      TopL

        ; Did we rotate any?
        cmp     dwDeviceCount,0
        mov     eax,0
        je      DoneIt

        ; Yes! That means that the display device used by the last
        ; CRTCCX we found should go into the primary.
        sub     edi,edi
        push    ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        push    OldCrtcDevData
        call    GN_SetDevData

        ; Get the Device Data Mask for all display devices.
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        pushd   0
        call    GN_GetDevDataMask
        or      eax,80000000H
        mov     dwLocalDeviceMask,eax

        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwLocalDeviceMask
        call    HW_BeginDisplayChange

        ; Go into each GNCRTCCX and set the physical resolution
        ; equal to the desktop resolution. The mode will be
        ; backed off if it cannot be set directly.
        sub     edi,edi
Loop8:
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[edi * 4]
        or      ecx,ecx
        je      NextCrtc8

        lea     ebx,ds:[ecx].GNCRTCCX.sCrtcModeInfo
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
        mov     ds:[ebx].MODEINFO.dwXRes,eax
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
        mov     ds:[ebx].MODEINFO.dwYRes,eax

NextCrtc8:
        inc     edi
        cmp     edi,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jb      Loop8

        ; remap everything
        push    esi
        call    GN_CrtcRemapArray
        push    esi
        call    GN_RestoreAllCrtcStates

        ; Signal the end of a display state change
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwLocalDeviceMask
        call    HW_EndDisplayChange

        mov     ds:[esi].GNCRTCCX.dwDisplayDeviceChange,1
        mov     eax,1

DoneIt:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_LockHWCrtcOnHead
;
; Purpose:      This function locks the HWCRTCCX on the head specified
;               for use by the caller. The display driver will not change
;               the hardware state of the CRTC controller on the head
;               specified for any reason. It is the caller's
;               responsibility to unlock when he is done.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               es:edi          NVTWINVIEWDATA
;               esi             GNLOGDEVCX ptr
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_LockHWCrtcOnHead, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Load up the ptr to the Data
        sub     edi,edi
        les     di,lpOut
        mov     pHWCrtcCXUsingTheHead,0

        ; Find the GNLOGDEVCX that is using this head number
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    es:[edi].NVTWINVIEWDATA.dwCrtc
        call    GN_MapHeadToGNLogdev
        or      eax,eax
        je      DoneIt
        mov     pGNLogdevCXUsingTheHead,ebx
        mov     pGNCrtcCXUsingTheHead,eax
        mov     dwLogCrtcIndex,ecx

        ; We need to reset the panning rectangle to be justified
        ; in the upper left corner.
        sub     ecx,ecx
        mov     ds:[eax].GNCRTCCX.rcPhysicalViewport.dwLeft,ecx
        mov     ds:[eax].GNCRTCCX.rcPhysicalViewport.dwTop,ecx
        lea     ebx,[eax].GNCRTCCX.sCrtcModeInfo
        mov     ecx,ds:[ebx].MODEINFO.dwXRes
        mov     ds:[eax].GNCRTCCX.rcPhysicalViewport.dwRight,ecx
        mov     ecx,ds:[ebx].MODEINFO.dwYRes
        mov     ds:[eax].GNCRTCCX.rcPhysicalViewport.dwBottom,ecx
        push    eax
        call    GN_SetPanRectangle

        ; Save the HWCRTCCX using the head to return to the caller
        mov     eax,pGNCrtcCXUsingTheHead
        mov     ebx,ds:[eax].GNCRTCCX.pHWCrtcCX
        mov     pHWCrtcCXUsingTheHead,ebx

        ; Play some games here! NULL out the HWCRTCCX in the GNCRTCCX
        ; so that the display driver will not mess with that head.
        mov     ds:[eax].GNCRTCCX.pHWCrtcCXWhenLocked,ebx
        mov     ds:[eax].GNCRTCCX.pHWCrtcCX,0

        ; Tell the hwlogdev that the HWCRTCCX is NULL
        mov     eax,pGNLogdevCXUsingTheHead
        push    ds:[eax].GNLOGDEVCX.pHWLogdevCX
        push    dwLogCrtcIndex
        push    pHWCrtcCXUsingTheHead
        call    HW_LogdevDetachCrtc

        ; Reset the gamma ramp to the identity
        lea     ebx,aGammaTable
        sub     ecx,ecx
@@:     mov     ss:[ebx],ecx
        add     ebx,4
        add     ecx,010101H
        or      cl,cl
        jne     @B

        ; Get a flat ptr to the identity gamma table
        mov     ax,ss
        push    eax
        call    GN_GetSelectorBase
        lea     edx,aGammaTable
        add     eax,edx

        ; Tell the hardware to set it
        push    pHWCrtcCXUsingTheHead
        push    eax
        pushd   0
        pushd   100H
        call    HW_SetGamma

        ; Turn the cursor off on the CRTC in question
        push    pHWCrtcCXUsingTheHead
        pushd   0
        call    HW_CursorEnable_FAR32
        mov     eax,1

DoneIt:
        sub     edi,edi
        les     di,lpOut
        mov     eax,pGNLogdevCXUsingTheHead
        mov     es:[edi].NVTWINVIEWDATA.nvtwLockUnlockHwcrtc.pGNLogdevCX,eax
        mov     eax,dwLogCrtcIndex
        mov     es:[edi].NVTWINVIEWDATA.nvtwLockUnlockHwcrtc.dwLogicalCrtcIndex,eax
        mov     eax,pHWCrtcCXUsingTheHead
        mov     es:[edi].NVTWINVIEWDATA.nvtwLockUnlockHwcrtc.pHWCrtcCX,eax
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_UnlockHWCrtcOnHead
;
; Purpose:      This function unlocks the HWCRTCCX on the head specified
;               for use by the caller. The display driver can once again
;               change the hardware state of the CRTC controller on the
;               head specified for any reason.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:     ds
;==============================================================================
DECPROC GN_UnlockHWCrtcOnHead, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Load up the ptr to the Data
        sub     edi,edi
        les     di,lpOut
        mov     ebx,es:[edi].NVTWINVIEWDATA.nvtwLockUnlockHwcrtc.pGNLogdevCX
        mov     pGNLogdevCXUsingTheHead,ebx
        mov     ecx,es:[edi].NVTWINVIEWDATA.nvtwLockUnlockHwcrtc.dwLogicalCrtcIndex
        mov     dwLogCrtcIndex,ecx
        mov     eax,es:[edi].NVTWINVIEWDATA.nvtwLockUnlockHwcrtc.pHWCrtcCX
        mov     pHWCrtcCXUsingTheHead,eax

        ; Check for NULL ptrs.
        mov     eax,1
        cmp     pHWCrtcCXUsingTheHead,0
        je      DoneIt

        ; Is this a legal logical crtc index for this device?
        sub     eax,eax
        cmp     ecx,ds:[ebx].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        jae     DoneIt

        ; Let's just make sure we don't already have a HWCRTCCX there
        sub     eax,eax
        mov     edx,ds:[ebx].GNLOGDEVCX.pGNCrtcCX[ecx * 4]
        cmp     ds:[edx].GNCRTCCX.pHWCrtcCX,0
        jne     DoneIt
        mov     pGNCrtcCXUsingTheHead,edx

        ; Now Attach the HWCRTCCX to the GNCRTCCX
        mov     eax,pHWCrtcCXUsingTheHead
        mov     ds:[edx].GNCRTCCX.pHWCrtcCX,eax
        mov     ds:[edx].GNCRTCCX.pHWCrtcCXWhenLocked,0

        ; Now Attach the HWCRTCCX to the HWLOGDEVCX
        mov     esi,pGNLogdevCXUsingTheHead
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    dwLogCrtcIndex
        push    pHWCrtcCXUsingTheHead
        call    HW_LogdevAttachCrtc

        ; Get the Device Data Mask for all display devices.
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        pushd   0
        call    GN_GetDevDataMask
        or      eax,80000000H
        mov     dwLocalDeviceMask,eax

        ; Signal the beginning of a display state change
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwLocalDeviceMask
        call    HW_BeginDisplayChange

        ; Restore the software state into the hardware
        push    pGNCrtcCXUsingTheHead
        push    pGNLogdevCXUsingTheHead
        call    GN_RestoreCrtcState

        ; Reset the last active cursor
        push    pGNCrtcCXUsingTheHead
        call    GN_ResetSelectedCursor

        ; Signal the end of a display state change
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwLocalDeviceMask
        call    HW_EndDisplayChange

        mov     eax,1
DoneIt:
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_SpecialSetPhysicalMode
;
; Purpose:      This function can be called as part of the DVD Zoom
;               escapes. Once the caller has locked a HWCRTCCX on a
;               given head, they can call this routine to set the
;               physical mode on the given head.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:     ds
;==============================================================================
DECPROC GN_SpecialSetPhysicalMode, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Load up the ptr to the Data
        sub     edi,edi
        les     di,lpOut

        mov     ebx,es:[edi].NVTWINVIEWDATA.nvtwPhysicalModeImmediate.pGNLogdevCX
        mov     pGNLogdevCXUsingTheHead,ebx
        mov     ecx,es:[edi].NVTWINVIEWDATA.nvtwPhysicalModeImmediate.dwLogicalCrtcIndex
        mov     dwLogCrtcIndex,ecx
        mov     eax,es:[edi].NVTWINVIEWDATA.nvtwPhysicalModeImmediate.pHWCrtcCX
        mov     pHWCrtcCXUsingTheHead,eax
        or      eax,eax
        je      DoneIt

        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes
        mov     sModeInfo.dwXRes,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes
        mov     sModeInfo.dwYRes,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp
        mov     sModeInfo.dwBpp,eax
        mov     eax,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh
        mov     sModeInfo.dwRefreshRate,eax
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceType
        call    HW_MapHWDeviceTypeToGNDeviceType
        mov     (DEVDATA PTR sModeInfo.MIDevData).cType,cl
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwTVFormat
        call    HW_MapHWTvFormatToGNTvFormat
        mov     (DEVDATA PTR sModeInfo.MIDevData).cFormat,cl

        ; This gets the timing parameters for the mode requested
        ; or a smaller mode if the requested mode was not settable.
        mov     ebx,pGNLogdevCXUsingTheHead
        mov     ecx,dwLogCrtcIndex
        mov     edx,ds:[ebx].GNLOGDEVCX.pGNCrtcCX[ecx * 4]
        mov     pGNCrtcCXUsingTheHead,edx

        ; Get timings
        push    ds:[edx].GNCRTCCX.lpNvmodeDispData
        push    ds:[edx].GNCRTCCX.lpEdidBuffer
        push    ds:[edx].GNCRTCCX.dwEdidSize
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        push    eax
        lea     ax,sModeOut
        push    eax
        lea     ax,sSettableModeInfo
        push    eax
        CALLFAR16IND lpfnFindModeEntry,es,ax

        ; Get the devices in use
        mov     ebx,pGNLogdevCXUsingTheHead
        push    ds:[ebx].GNLOGDEVCX.pGNBoardCX
        pushd   0
        call    GN_GetDevDataMask
        or      eax,80000000H
        mov     dwDevDataMask,eax

        ; Signal the beginning of a display state change
        mov     ebx,pGNLogdevCXUsingTheHead
        mov     ebx,ds:[ebx].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwDevDataMask
        call    HW_BeginDisplayChange

        ; Call the hardware to program the timings
        mov     eax,pGNCrtcCXUsingTheHead
        mov     ebx,pGNLogdevCXUsingTheHead
        push    pHWCrtcCXUsingTheHead
        push    ds:[ebx].DIBENGINEHDR.deBitsOffset
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeOut
        push    eax
        call    HW_SetCrtcMode

        mov     ebx,pGNLogdevCXUsingTheHead
        mov     ebx,ds:[ebx].GNLOGDEVCX.pGNBoardCX
        push    ds:[ebx].GNBOARDCX.pHWBoardCX
        push    dwDevDataMask
        call    HW_EndDisplayChange

        ; Return the mode actually set
        sub     edi,edi
        les     di,lpOut
        mov     eax,sSettableModeInfo.dwXRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes,eax
        mov     eax,sSettableModeInfo.dwYRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes,eax
        mov     eax,sSettableModeInfo.dwBpp
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp,eax
        mov     eax,sSettableModeInfo.dwRefreshRate
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh,eax
        movzx   ecx,(DEVDATA PTR sSettableModeInfo.MIDevData).cType
        call    HW_MapGNDeviceTypeToHWDeviceType
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwDeviceType,ecx
        movzx   ecx,(DEVDATA PTR sSettableModeInfo.MIDevData).cFormat
        call    HW_MapGNTvFormatToHWTvFormat
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwTVFormat,ecx
        mov     eax,1

DoneIt:
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,eax
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_MapLogToPhysHead
;
; Purpose:      This routine takes a 0 based head logical head number
;               and returns a physical head number.
;
; Arguments:
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      ecx             1
;                               Also the logical head number in
;                               lpOut->NVTINWVIEWDATA.dwCrtc was
;                               replaced with the physical head
;                               number that corresponds to it.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_MapLogToPhysHead, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     edi,edi
        les     di,lpOut

        ; Assume failure
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,0

        ; Get the logical CRTC number -- make sure it is within range
        ; for this display driver.
        mov     ebx,es:[edi].NVTWINVIEWDATA.dwCrtc
        cmp     ebx,ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        jae     MLTPH_End

        ; Is that logical CRTC NULL?
        mov     edx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4]
        or      edx,edx
        je      MLTPH_End

        ; Nope it is fine.  Get the physical head number
        mov     eax,ds:[edx].GNCRTCCX.pHWCrtcCX
        or      eax,eax
        jne     @F

        ; This CRTC has already been locked
        mov     eax,ds:[edx].GNCRTCCX.pHWCrtcCXWhenLocked
        or      eax,eax
        je      MLTPH_End

@@:
        push    eax
        call    HW_GetPhysicalCrtcIndex
        mov     es:[edi].NVTWINVIEWDATA.dwCrtc,eax
        mov     es:[edi].NVTWINVIEWDATA.dwSuccess,1

MLTPH_End:
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_SetGetDisplayConfig
;
; Purpose:      This routine either sts or gets the display configuration
;               which includes, the desktop state (normal or clone), the
;               display devices being used, and the physical modes on
;               each display device.
;
; Arguments:
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      ecx             1
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_SetGetDisplayConfig, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; We might need these
        mov     ax,ss
        shl     eax,10H
        lea     ax,sModeInfo
        mov     lpModeInfo,eax
        lea     ax,sDesktopModeInfo
        mov     lpDesktopModeInfo,eax
        lea     ax,LocalDevData
        mov     lpLocalDevData,eax

        sub     edi,edi
        les     di,lpOut

        ; Determine Set or Get
        mov     edx,es:[edi].NVTWINVIEWDATA.dwFlag
        cmp     edx,NVTWINVIEW_FLAG_GET
        jne     SetDisplayConfig

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GetDisplayConfig:

        ; Find out info on the display devices
        mov     dwDeviceCount,0
        mov     dwDevDataMask,1
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwDisplayConfig.dwActiveDevices,0
Loop1:
        mov     ecx,dwDeviceCount
        mov     edx,dwDevDataMask
        cmp     ds:[esi].GNLOGDEVCX.pGNCrtcCX[ecx * 4],0
        je      Next1

        ; CRTCCX is present -- OR in a present bit for this
        or      es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwDisplayConfig.dwActiveDevices,edx

        ; Get the device type
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[ecx * 4]
        call    GN_GetDevData
        mov     OldCrtcDevData,eax

        ; Convert the DevData type to a mask
        shl     al,3
        add     al,ah
        mov     cl,al
        mov     ebx,1
        shl     ebx,cl
        movzx   eax,cl
        or      es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwDisplayConfig.dwDeviceMask,ebx

        ; If this is the primary device, set its mask
        cmp     dwDeviceCount,0
        jne     @F
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwDisplayConfig.dwPrimaryDeviceMask,ebx
@@:
        ; Compute the correct offset to store the PhysMode at
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwDisplayConfig.dwPhysicalModeFlags
        imul    eax,SIZE PHYSICAL_MODE
        lea     edi,[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwDisplayConfig.PhysModes
        add     edi,eax

        ; Is bit mask ebx set in the dwPhysicalModeFlags field?
        test    ebx,ecx
        jz      GetMaxMode

GetCurrentMode:
        mov     ecx,dwDeviceCount
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ecx * 4]
        lea     ebx,[ebx].GNCRTCCX.sCrtcModeInfo
        mov     eax,ds:[ebx].MODEINFO.dwXRes
        mov     es:[edi].PHYSICAL_MODE.dwXRes,eax
        mov     eax,ds:[ebx].MODEINFO.dwYRes
        mov     es:[edi].PHYSICAL_MODE.dwYRes,eax
        mov     eax,ds:[ebx].MODEINFO.dwRefreshRate
        mov     es:[edi].PHYSICAL_MODE.dwRefreshRate,eax
        jmp     Next1

GetMaxMode:
        ; Pass in a ridiculously large mode to FindModeEntry and it
        ; will bump it down to something the device can set.
        mov     sModeInfo.dwXRes,7FFFH
        mov     sModeInfo.dwYRes,7FFFH
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deBitsPixel
        mov     sModeInfo.dwBpp,eax
        mov     sModeInfo.dwRefreshRate,1000
        mov     eax,OldCrtcDevData
        mov     dword ptr sModeInfo.MIDevData,eax

        mov     ecx,dwDeviceCount
        mov     edx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ecx * 4]
        push    edx
        push    lpModeInfo
        lfs     bx,ds:[edx].GNCRTCCX.lpNvmodeDispData
        push    fs:[bx].DISPDATA.dwMonitorInfType
        push    ds:[edx].GNCRTCCX.lpEdidBuffer
        push    ds:[edx].GNCRTCCX.dwEdidSize
        call    GN_ValidatePhysicalMode

        ; We don't care about the return value. The point is,
        ; whatever is in sModeInfo is the largest settable mode.
        ; Copy it into the PHYSICAL_MODE structure.
        mov     es,word ptr lpOut+2
        mov     eax,sModeInfo.dwXRes
        mov     es:[edi].PHYSICAL_MODE.dwXRes,eax
        mov     eax,sModeInfo.dwYRes
        mov     es:[edi].PHYSICAL_MODE.dwYRes,eax
        mov     eax,sModeInfo.dwRefreshRate
        mov     es:[edi].PHYSICAL_MODE.dwRefreshRate,eax

Next1:
        inc     dwDeviceCount
        shl     dwDevDataMask,1
        mov     ecx,dwDeviceCount
        cmp     ecx,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      Loop1
        jmp     Success

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SetDisplayConfig:
        ; Let's shotgun save the entire _NVTWINVIEW_DISPLAY_CONFIG
        ; structure into our GNLOGDEVCX in case we need it.
        PUSHR   ds,esi
        mov     ebx,edi
        push    ds
        push    es
        pop     ds
        pop     es
        lea     edi,[esi].GNLOGDEVCX.sDispConfig
        lea     esi,[ebx].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwDisplayConfig
        mov     ecx,SIZE NVTWINVIEW_DISPLAY_CONFIG
        cld
        rep     movs byte ptr es:[edi], ds:[esi]
        POPR    ds,esi
        lea     edi,[esi].GNLOGDEVCX.sDispConfig

        cmp     ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwSetModesNow,0
        jne     FailIt

        ; Now we can reference everything from ds:[edi]
        ; This code assumes only two heads and assumes that
        ; it will never be called when we are in multi-mon mode.
        mov     eax,ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwActiveDevices
        shr     eax,1
        inc     eax
        mov     dwDeviceCount,eax
        or      eax,eax
        je      FailIt
        cmp     eax,2
        ja      FailIt

        ; Convert the dwDeviceMask to an array of DevDatas and ask if
        mov     edx,ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwDeviceMask
        lfs     bx,lpLocalDevData

Loop3:
        sub     ecx,ecx
Loop4:
        test    edx,1
        jz      @F
        mov     dword ptr fs:[bx],0
        mov     ch,cl
        shr     ch,3
        mov     byte ptr fs:[bx].DEVDATA.cType,ch
        mov     ch,cl
        and     ch,07H
        mov     byte ptr fs:[bx].DEVDATA.cNumber,ch
        add     bx,4
        dec     eax
        je      Out1
@@:     inc     cl
        shr     edx,1
        cmp     cl,18H
        jc      Loop4
Out1:

        ; Ask the hardware if it is possible to have all the
        ; devices specified enabled simultaneously by assigning
        ; each to a different head.
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    ds:[ecx].GNBOARDCX.pHWBoardCX
        push    lpLocalDevData
        push    dwDeviceCount
        call    HW_CanDevicesBeMapped
        or      eax,eax
        je      FailIt

        ; dwPrimaryDeviceMask should be one of dwDeviceMask bits
        mov     eax,ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwDeviceMask
        mov     ebx,ds:[edi].NVTWINVIEW_DISPLAY_CONFIG.dwPrimaryDeviceMask
        and     eax,ebx
        je      FailIt

DeferChange:
        mov     ds:[esi].GNLOGDEVCX.dwDisplayConfigPending,1
        mov     ds:[esi].GNLOGDEVCX.dwNvDisplayChangePending,1
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     ds:[ebx].GNCRTCCX.dwDisplayDeviceChange,1
        
        jmp     Success
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

FailIt:
        les     di,lpOut
        mov     es:[di].NVTWINVIEWDATA.dwSuccess,0
        jmp     Done
Success:
        les     di,lpOut
        mov     es:[di].NVTWINVIEWDATA.dwSuccess,1
Done:
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetBiggestEdidMode
;
; Purpose:      This routine gets the largest mode in the EDID for the
;               display device passed in.
;
; Arguments:
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      ecx             1
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetBiggestEdidMode, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC


        ; Need to change dwDeviceMask into DevData
        mov     ecx,es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwDisplayConfig.dwDeviceMask

        ; First look at CRTS
        mov     eax,DEVTYPE_CRT + 0FF00H
@@:     inc     ah
        test    cl,1
        jz      Next1
        jmp     GotDevData
Next1:  shr     cl,1
        cmp     ah,8
        jc      @B

        ; Nope. Try DFPS
        shr     ecx,8
        mov     eax,DEVTYPE_DFP + 0FF00H
@@:     inc     ah
        test    ch,1
        jz      Next2
        jmp     GotDevData
Next2:  shr     ch,1
        cmp     ah,8
        jc      @B

        ; Last hope is TVS
        mov     eax,DEVTYPE_TV + 0FF00H
@@:     inc     ah
        test    cl,1
        jz      Next3
        jmp     GotDevData
Next3:  shr     cl,1
        cmp     ah,8
        jc      @B
        
        ; No valid device attached. Fail
        jmp     FailIt
        
GotDevData:
        mov     (DEVDATA PTR LocalDevData).cType,al
        mov     (DEVDATA PTR LocalDevData).cNumber,ah

        ; TV has no EDID. Return largest TV mode.
        cmp     (DEVDATA PTR LocalDevData).cType,DEVTYPE_TV
        jne     GetEdid

        ; The specific GNCRTCCX is not important as dwMaxTVOutResolution is
        ; the same regardless of the GNCRTCCX.
        mov     edx,ds:[esi].GNLOGDEVCX.pGNCrtcCX

        ; The largest TV mode is in the DispData structure.
        sub     ebx,ebx
        lfs     bx,ds:[edx].GNCRTCCX.lpNvmodeDispData
        mov     eax,fs:[ebx].DISPDATA.dwMaxTVOutResolution
        lea     ecx,ds:[edx].GNCRTCCX.sCrtcModeInfo
        mov     ecx,ds:[ecx].MODEINFO.dwBpp

        mov     ebx,eax
        shr     ebx,16  
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes,ebx
        and     eax,0FFFFh
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes,eax
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp,ecx
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh,60

        jmp     Success

GetEdid:
        ; Read the EDID for this device if it has an EDID
        mov     dwSizeEdidBuffer,MAX_EDID_BUFFER_SIZE
        mov     edx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        push    ds:[edx].GNCRTCCX.pHWCrtcCX
        push    dword ptr LocalDevData
        mov     ax,ss
        push    eax
        call    GN_GetSelectorBase
        lea     ecx,sLocalEdidBuffer
        add     ecx,eax
        push    ecx
        lea     ecx,dwSizeEdidBuffer
        add     ecx,eax
        push    ecx
        call    HW_ReadEdid
        
        cmp     dwSizeEdidBuffer,0
        jne     DoneEdidRead
        
        ; No EDID for this display device. Return something safe like
        ; 800x600@60Hz.
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes,800
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes,600
        mov     edx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        lea     edx,ds:[edx].GNCRTCCX.sCrtcModeInfo
        mov     edx,ds:[edx].MODEINFO.dwBpp
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp,edx
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh,60
        
        jmp     Success

DoneEdidRead:
        ; Now get the largest mode for this EDID.
        mov     ax,ss
        shl     eax,10h
        lea     ax,sLocalEdidBuffer
        push    eax
        push    dwSizeEdidBuffer
        lea     ax,sModeInfo
        push    eax
        CALLFAR16IND lpfnGetLargestEdidMode,es,ax

        sub     edi,edi
        les     di,lpOut
        sub     ebx,ebx
        lea     bx,sModeInfo
        mov     eax,ss:[ebx].MODEINFO.dwXRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwXRes,eax
        mov     eax,ss:[ebx].MODEINFO.dwYRes
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwYRes,eax
        mov     edx,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        lea     edx,ds:[edx].GNCRTCCX.sCrtcModeInfo
        mov     edx,ds:[edx].MODEINFO.dwBpp
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwBpp,edx
        mov     eax,ss:[ebx].MODEINFO.dwRefreshRate
        mov     es:[edi].NVTWINVIEWDATA.NVTWINVIEWUNION.nvtwdevdata.dwRefresh,eax

        jmp     Success

FailIt:
        les     di,lpOut
        mov     es:[di].NVTWINVIEWDATA.dwSuccess,0
        jmp     Done
Success:
        les     di,lpOut
        mov     es:[di].NVTWINVIEWDATA.dwSuccess,1
Done:
        mov     ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_MapHeadToGNLogdev
;
; Purpose:      This routine takes a 0 based head nmber and returns
;               the GNLOGDEVCX ptr that the head number is attached to.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;               dwHead                  0 based head number
;
; Returns:      eax                     GNCRTCCX using the head
;               ebx                     GNLOGDEVCX owning the GNCRTCCX
;                                       which is using the head
;               ecx                     dwLogicalCrtcIndex of GNCRTCCX
;                                       in GNLOGDEVCX
;               eax,ebx, are NULL if not found
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_MapHeadToGNLogdev, PASCAL, FRAME, FAR16
PARMD   pGNBoardCXOffset
PARMD   dwHead
LOCALD  dwCRTCCount
LOCALD  dwLogdevCount
OPENPROC
        PUSHR   esi
        mov     ebx,pGNBoardCXOffset
        mov     dwLogdevCount,0
        sub     eax,eax

LoopLogdev:
        mov     edx,ds:[ebx].GNBOARDCX.pGNLogdevCX[eax * 4]
        or      edx,edx
        je      NextLogdev

        ; Is this head on this GNLOGDEVCX?
        lea     ecx,[edx].GNLOGDEVCX.pGNCrtcCX
        mov     dwCRTCCount,0

LoopCrtc:
        mov     eax,ds:[ecx]
        or      eax,eax
        je      NextCrtc

        PUSHR   eax,ebx,ecx,edx
        push    ds:[eax].GNCRTCCX.pHWCrtcCX
        call    HW_GetPhysicalCrtcIndex
        mov     esi,eax
        POPR    eax,ebx,ecx,edx
        cmp     esi,dwHead
        je      Found

NextCrtc:
        inc     dwCRTCCount
        add     ecx,4
        mov     eax,dwCRTCCount
        cmp     eax,ds:[edx].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        jc      LoopCrtc

NextLogdev:
        inc     dwLogdevCount
        mov     eax,dwLogdevCount
        cmp     eax,ds:[ebx].GNBOARDCX.dwLogicalDeviceInstancesOnThisBoard
        jc      LoopLogdev

NotFound:
        sub     eax,eax
        sub     ebx,ebx
        jmp     DoneIt

Found:
        mov     ebx,edx
        mov     ecx,dwCRTCCount
DoneIt:
        POPR    esi
CLOSEPROC

CLOSESEG _NVTEXT16

END

