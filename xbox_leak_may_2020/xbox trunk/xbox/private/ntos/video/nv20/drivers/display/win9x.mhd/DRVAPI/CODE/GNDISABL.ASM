;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gndisabl.asm
;
; Purpose:      This file holds the Disable display driver entry point.
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
include minivdd.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     GENERIC_Disable
;
; Purpose:      This function is the Disable display driver entry point.
;               It handles proper device disabling in a multi-monitor
;               environment.
;
; Arguments:    lpDstDev  - The device to disable.
;
; Returns:      eax     should always be set to -1
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_Disable, PASCAL, FRAME, FAR16
PARMD   lpDstDev
LOCALD  dwRet
LOCALD  dwVddDeviceHandle
OPENPROC
        PUSHR   ds,esi,edi
        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]

        ; Split into primary and secondary cases
        cmp     cs:[dwThisLogicalDeviceHandle_In_NVTEXT16],1
        je      AsIfPrimary

        ; Enable access to the secondary device
        mov     dwRet,0
        pushd   ENABLE_ALL
        call    GN_VddEnableDevice
        cmp     eax,ENABLE_ERROR
        je      done

AsIfPrimary:
        ; This is either the primary device or it is a secondary device
        ; with full access turned on. We can disable the device now.
        push    esi
        call    GN_DisableDevice
        mov     dwRet,eax

        ; If we are a secondary, we have a few things to clean up.
        cmp     cs:[dwThisLogicalDeviceHandle_In_NVTEXT16],1
        je      done

        ; Turn off all access to our device.
        pushd   ENABLE_NONE
        call    GN_VddEnableDevice

        ; We are done with the secondary device -- close it
        call    GN_VddCloseDevice
done:
        mov     eax,dwRet
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_DisableDevice
;
; Purpose:      This function does the disabling once a specific device has
;               been selected.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX
;
; Returns:      eax     should always be set to -1
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_DisableDevice, PASCAL, FRAME, NEAR
PARMD   pGNLogdevCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pGNLogdevCXOffset

        ; If we are not "attached to the desktop", then
        ; don't blank the screen.
        call    GN_VddIsDriverAttachedToDesktop
        or      ax,ax
        je      SkipBlankScreen

        ; Don't access to the FB if we're in powerdown
        cmp     ds:[esi].GNLOGDEVCX.dwInt2FScreenSwitchOut,0
        jne     SkipBlankScreen
        
        push    pGNLogdevCXOffset
        call    GN_BlankScreen

SkipBlankScreen:
        ; Let the DIBENG do its thing
        push    cs:[CSlp16LogDevice_In_NVTEXT16]
        call    DIB_Disable

        ; Send a message down to disable the device
        push    esi
        pushd   MSG_DISABLE
        push    esi
        push    cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        call    GN_LogdevMessage

        ; Free the primary surface memory.
        cmp     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        je      @F
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_FreeLogicalSurface
        
        ; Don't access to the FB if we're in powerdown
        cmp     ds:[esi].GNLOGDEVCX.dwInt2FScreenSwitchOut,0
        jne     SkipBlankScreen1
        
        ; Blank the screen. If we just rely on the earlier GN_BlankScreen,
        ; then when we free the logical surface, tiling may be disabled which
        ; can cause garbage on the screen. However, if we don't do the earlier
        ; GN_BlankScreen, we get a flash of garbage when the surface is
        ; untiled.
        push    pGNLogdevCXOffset
        call    GN_BlankScreen

SkipBlankScreen1:
        mov     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
@@:

        ; Enable IO trapping if we are not a secondary device.
        ; The GN_VddEnableIOTrapping checks the device ID.
        pushd   1
        call    GN_VddEnableIOTrapping
        POPR    esi

        ; Return Success
        mov     eax,-1
CLOSEPROC


;==============================================================================
;
; Function:     GN_BlankScreen
;
; Purpose:      This function blts black to the screen.
;
; Arguments:
;               ds                      Context Selector
;               pGNLogdevCXOffset       GNLOGDEVCX
;
; Returns:
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_BlankScreen, PASCAL, FRAME, NEAR
PARMD   pGNLogdevCXOffset
OPENPROC
        PUSHR   ds,es,esi,edi

        ; Clear the screen to black so there won't be wierd artifacts
        ; if we switch back to a lower resolution mode.
        mov     esi,pGNLogdevCXOffset

        ; Make sure we have the primary surface
        cmp     ds:[esi].GNLOGDEVCX.dwMemorySizeForPrimarySurface,0
        je      GN_BlankScreen_Exit

        ; Check DOS FS or power state.
        cmp     ds:[esi].GNLOGDEVCX.dwInt2FScreenSwitchOut,0
        jne     GN_BlankScreen_Exit

        ; We can't use DIBENG at this point because
        ; CSpGNLogicalDevice_In_NVTEXT16 not initialize yet.
        mov     ecx,ds:[esi].DIBENGINE.deDeltaScan
        movzx   ebx,word ptr ds:[esi].DIBENGINE.deHeight
        imul    ecx,ebx
        shr     ecx,2
        mov     es,ds:[esi].DIBENGINEHDR.deBitsSelector
        mov     edi,ds:[esi].DIBENGINEHDR.deBitsOffset
        sub     eax,eax
        rep     stos dword ptr es:[edi]

GN_BlankScreen_Exit:
        POPR    ds,es,esi,edi
CLOSEPROC

CLOSESEG _NVTEXT16

END

