;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnvdd.asm
;
; Purpose:      This file holds the interface calls to the main VDD.
;
;==============================================================================
.586
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
include minivdd.inc
include ..\..\..\..\..\sdk\nvidia\inc\rddapi.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE
PUBLIC  dwThisLogicalDeviceHandle
dwThisLogicalDeviceHandle       DD      0
CLOSESEG _NVTEXT32

OPENSEG  _NVTEXT16, USE16, CODE
PUBLIC  lpfnVddEntryPoint
lpfnVddEntryPoint               DD      0

PUBLIC  dwThisLogicalDeviceHandle_In_NVTEXT16
dwThisLogicalDeviceHandle_In_NVTEXT16   DD      0

;==============================================================================
;
; Function:     GN_VddGetEntryPoint
;
; Purpose:      This function gets the entry point for the VDD Protected
;               Mode API services.
;
; Arguments:    None
;
; Returns:      eax     0       Failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddGetEntryPoint, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,edi
        mov     eax,1684h
        mov     ebx,0AH
        int     2fh
        mov     ax,es
        shl     eax,10H
        mov     ax,di
        mov     ds,cs:[wNVTEXT16_Alias_In_NVTEXT16]
        mov     ds:[lpfnVddEntryPoint],eax
        POPR    ds,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddGetDeviceHandle
;
; Purpose:      This function gets the device handle for this
;               instance of the driver. This function should
;               not rely on the CSlp16LogDevice being set.
;
; Arguments:    eax     lpDstDevType argument to GENERIC_Enable
;
; Returns:      eax     0     then there was an error getting the handle
;                       non-0 then ecx has the dwDeviceHandle
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddGetDeviceHandle, PASCAL, FRAME, FAR16
PARMD   lpDstDevType
OPENPROC
        PUSHR   ds,esi,edi

        ; If lpDstDevType is NULL, then we are the primary display.
        mov     ecx,1
        mov     eax,1
        cmp     lpDstDevType,0
        je      DoneIt

        ; Call the main VDD to have it open us as a secondary device
        mov     eax,lpDstDevType
        movzx   edi,ax
        shr     eax,10H
        mov     es,ax
        mov     eax,VDD_OPEN
        sub     edx,edx
        sub     ecx,ecx
        mov     si,es
        sub     ebx,ebx                 ; use EBX to point to PDEVICE
        call    FFP16 PTR lpfnVddEntryPoint

        ; 0 and -1 are not valid device handles
        ; Anything else we must accept as valid
        ; (Note: minivdd.inc does not specify any error codes.)
        mov     ecx,eax
        sub     eax,eax
        cmp     ecx,-1
        je      DoneIt

        mov     eax,1
        cmp     ecx,0
        jne     DoneIt
        mov     ecx,1

DoneIt:
        mov     ds,cs:[wNVTEXT16_Alias_In_NVTEXT16]
        mov     ds:[dwThisLogicalDeviceHandle_In_NVTEXT16],ecx
        mov     ds,cs:[wNVTEXT32_Alias_In_NVTEXT16]
        mov     ds:[dwThisLogicalDeviceHandle],ecx
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddEnableDevice
;
; Purpose:      This function will make a call to the VDD to enable
;               some type of access to the device.
;
; Arguments:
;               dwEnableType            Enable type like ENABLE_MEM, etc.
;                                       minivdd.inc has all the equates
;
; Returns:      eax     ENABLE_ERROR if there was a problem
;                       anything else if there was no problem
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddEnableDevice, PASCAL, FRAME, FAR16
PARMD   dwEnableType
OPENPROC
        PUSHR   ds,esi,edi
        mov     ebx,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        mov     edx,dwEnableType
        mov     eax,VDD_ENABLE
        sub     edi,edi
        mov     es,di
        mov     esi,edi
        sub     ecx,ecx
        call    FFP16 PTR lpfnVddEntryPoint
        cmp     eax,VDD_ENABLE
        jne     @F
        mov     eax,ENABLE_ERROR
@@:     POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddCloseDevice
;
; Purpose:      This function closes the device specified in
;               GNBOARDCX.dwDeviceHandle
;
; Arguments:
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddCloseDevice, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi
	mov	eax,VDD_CLOSE
        mov     ebx,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        sub     ecx,ecx
        sub     edx,edx
        mov     es,dx
        sub     edi,edi
        call    FFP16 PTR lpfnVddEntryPoint
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddGetDisplayInfo
;
; Purpose:      This function gets the DISPLAYINFO strcuture from the
;               main VDD.
;
; Arguments:
;               lpDisplayInfo           16:16 ptr to DUISPLAYINFO
;               dwDisplayCallIndex      either VDD_GET_DISPLAY_CONFIG
;                                       or VDD_GET_DISPLAY_CONFIG2
;
; Returns:      The DISPLAYINFO structure is filled in
;               There are no failure codes listed in minivdd.h
;               although the DISPLAYINFO.diInfoFlags has some error info.
;
; Preserve:     ds,esi,es,edi
;==============================================================================
DECPROC GN_VddGetDisplayInfo, PASCAL, FRAME, FAR16
PARMD   lpDisplayInfo
PARMD   dwDisplayCallIndex
OPENPROC
        PUSHR   ds,es,esi,edi
        sub     edi,edi
        les     di,lpDisplayInfo
        mov     eax,dwDisplayCallIndex
        mov     ebx,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        mov     ecx,SIZE DISPLAYINFO
        mov     es:[edi].DISPLAYINFO.diHdrSize,cx
        call    FFP16 PTR lpfnVddEntryPoint
        POPR    ds,es,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddDriverRegister
;
; Purpose:      This function calls the main VDD to register the
;               display driver. The driver can tell the VDD the function
;               that should be called when the driver needs to be put
;               back in a high resolution mode. Also, the driver can
;               specify an area of offscreen memory to be used for
;               VGA virtualization and whether or not planar modes
;               are supported in a Windowed DOS box.
;
;               The call to the VDD requires the following:
;               es:edi  ptr to ResetHiResMode function
;               ecx     Offscreen virtualization offset. The VDD may use
;                       from 32K to 64K beginning at this offset into
;                       video memory. If you don't want to allow
;                       VGA virtualization, pass in ecx equal to 0
;                       and edx below equal to -1.
;               edx     0 if planar mode virtualization is to be supported
;                         for Windowed DOS boxes.
;                       -1 if not
;                          If edx is set to -1, then ecx should be set
;                          to 0 -- ignore the minivdd.inc documentation.
;                          There was a bug in an early cut of the main VDD.
;
; Arguments:
;
; Returns:      eax     The offset of the end of the virtualization area
;                       if virtualization is being allowed (via edx,ecx).
;                       Hence, eax minus the value of ecx passed in will
;                       give the amount of memory the VDD is using for
;                       planar mode virtualization.
;                       If the caller requested no virtualization, then
;                       there is no return value.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddDriverRegister, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi
        mov     ebx,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        mov     ax,SEG GENERIC_ResetHiresMode
        mov     es,ax
        mov     edi,OFFSET GENERIC_ResetHiresMode
        sub     ecx,ecx
        mov     edx,-1
        mov     eax,VDD_DRIVER_REGISTER
        call    FFP16 PTR lpfnVddEntryPoint
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddDriverUnregister
;
; Purpose:      This function unregisters the display driver with the VDD.
;
; Arguments:
;
; Returns:      Nothing.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddDriverUnregister, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi
        mov     eax,VDD_DRIVER_UNREGISTER
        mov     ebx,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        call    FFP16 PTR lpfnVddEntryPoint
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddRegisterDisplayDriver
;
; Purpose:      This function was setup up by Windows to allow the
;               display driver to have a private communication with
;               its minivdd. The only required parameters for the
;               VDD_REGISTER_DISPLAY_DRIVER_INFO call are eax equal
;               to this service number and ebx equal to the
;               dwDeviceHandle. All other registers values are
;               private to the driver.
;
; Arguments:
;               ax                      desired value for ds
;               ebx                     desired value for esi
;
;               Whatever other arguments are desired in registers.
;               ax will be moved into ds and ebx will be moved into
;               esi. Then eax will be set to the value
;               VDD_REGISTER_DISPLAY_DRIVER_INFO and ebx will be set
;               the dwDeviceHandle and the call will be made to the
;               main VDD.
;
; Returns:      Whatever is returned from the
;               VDD_REGISTER_DISPLAY_DRIVER_INFO call.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddRegisterDisplayDriver, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi
        mov     ds,ax
        mov     esi,ebx
        mov     eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
        mov     ebx,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        call    FFP16 PTR lpfnVddEntryPoint
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddSaveDriverState
;
; Purpose:      This function tells the VDD to save the driver state.
;
; Arguments:
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddSaveDriverState, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi
        mov     eax,VDD_SAVE_DRIVER_STATE
        mov     ebx,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        call    FFP16 PTR lpfnVddEntryPoint
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddEnableIOTrapping
;
; Purpose:      This function can enable or disable IO trapping. The
;               routine is a NOP is we are not the primary display.
;
; Arguments:
;               dwEnableFlag            1 to enable trapping,
;                                       0 to disable trapping
;
; Returns:      None
;
; Preserve:     ds,esi
;==============================================================================
STOP_IO_TRAP    TEXTEQU         <4000H>
START_IO_TRAP   TEXTEQU         <4007H>
DECPROC GN_VddEnableIOTrapping, PASCAL, FRAME, FAR16
PARMD   dwEnableFlag
OPENPROC
        PUSHR   ds,esi,edi

        ; If we are the secondary display or if we are unsure what
        ; display we are -- do NOT enable trapping.
        cmp     cs:[dwThisLogicalDeviceHandle_In_NVTEXT16],1
        jne     done

        ; Otherwise, enable or disable based upon eax
        mov     eax,STOP_IO_TRAP
        cmp     dwEnableFlag,0
        je      @F
        mov     eax,START_IO_TRAP
@@:
        int     2FH
done:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddSetLockAndUnlockOffscreen
;
; Purpose:      Send the address of the Lock and Unlock Offscreen
;               Allocations up to the miniVDD. When a DOS box is created
;               the miniVDD will call Lock and whenever one is destroyed,
;               Unlock will be called. Since the display driver does not
;               get control in time to save the offscreen memory area
;               before a DOS box flips full screen, offscreen allocations
;               are inhibited while DOS boxes are active.
;
; Arguments:
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddSetLockAndUnlockOffscreen, PASCAL, FRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi
        mov     eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
        mov     ebx,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        mov     ecx,MVDD_SET_LOCK_AND_UNLOCK_OFFSCREEN_FUNCS
        mov     dx,SEG GN_OffscreenAllocationsLock
        movzx   edx,dx
        mov     esi,OFFSET GN_OffscreenAllocationsLock
        mov     edi,OFFSET GN_OffscreenAllocationsUnlock
        call    FFP16 PTR lpfnVddEntryPoint
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddSetPowerManagementCallbacks
;
; Purpose:      Send the address of the PowerOn and PowerOff
;               routines up to the miniVDD. When a power state will change
;               the miniVDD will call these routines respectively
;
; Arguments:
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddSetPowerManagementCallbacks, PASCAL, FRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi
        mov     eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
        mov     ebx,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        mov     ecx,MVDD_SET_POWER_MANAGEMENT_CALLBACKS
        mov     dx,SEG GN_PowerOn
        movzx   edx,dx
        mov     esi,OFFSET GN_PowerOn
        mov     edi,OFFSET GN_PowerOff
        call    FFP16 PTR lpfnVddEntryPoint
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddReenumerateDevNode
;
; Purpose:      Reenumerate the current DevNode.
;
; Arguments:
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddReenumerateDevNode, PASCAL, FRAME, FAR16
PARMD   dwLogicalDevIndex
PARMD   dwCrtcHead
OPENPROC
        PUSHR   ds,esi,edi
        mov     eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
        mov     ebx,cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        mov     ecx,MVDD_REENUMERATE_DEVNODE
        mov     edx,dwCrtcHead
        mov     esi,dwLogicalDevIndex
        call    FFP16 PTR lpfnVddEntryPoint
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_VddIsDriverAttachedToDesktop
;
; Purpose:      This routine determines if the display driver
;               is drivering a windows logical device that is
;               "attached to the desktop" -- i.e. is it enabled.
;               NOTE: If this is the primary driver, it is
;               always attached.
;
; Arguments:    ds      Context Selector
;               dwLogicalDeviceInstance
;
; Returns:      eax     0       not attached to the desktop
;                       non-0   is attached to the desktop
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_VddIsDriverAttachedToDesktop, PASCAL, FRAME, FAR16
LOCALS  szPciSpec,80H
OPENPROC
        mov     eax,1
        cmp     dwThisLogicalDeviceHandle_In_NVTEXT16,1
        je      DoneIt

        ; Copy the Pci Specifier to a local stack location
        mov     eax,dwThisLogicalDeviceHandle_In_NVTEXT16
        and     eax,NOT 03H
        mov     ebx,ds:[eax + 4]
        sub     edx,edx
        sub     ecx,ecx
        lea     cx,szPciSpec
@@:
        mov     al,ds:[ebx][edx]
        mov     ss:[ecx][edx],al
        inc     edx
        or      al,al
        jne     @B

        mov     ax,ss
        shl     eax,10H
        lea     ax,szPciSpec
        push    eax

        mov     eax,dwThisLogicalDeviceHandle_In_NVTEXT16
        and     eax,03H
        push    eax
        call    FFP16 PTR cs:[lpfnIsDriverAttachedToDesktop]
        movzx   eax,ax
DoneIt:
CLOSEPROC

CLOSESEG _NVTEXT16

END

