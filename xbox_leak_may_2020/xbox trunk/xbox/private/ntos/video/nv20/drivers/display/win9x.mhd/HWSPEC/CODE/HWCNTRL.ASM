;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation
;
; File:         hwcntrl.asm
;
; Purpose:      This file implements hardware-specific escape support.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include sysdata.inc
include gngdi.inc
include hwgdi.inc
include hwmacs.inc
include nvcm.inc
include escape.inc
include nv32.inc
include hwlogdev.inc
include hwcrtc.inc
include hwboard.inc

CONTROL_FRAME_AND_LOCALS        MACRO
PARMD   pHWLogdevCXOffset
PARMD   lpDstDev
PARMW   wFunction
PARMD   lpIn
PARMD   lpOut
ENDM

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     HW_ControlQuery
;
; Purpose:      This function implements a hardware specific escape call
;               when it is already known that the call is a QUERYESCSUPPORT.
;               The generic code did not recognize the escape function in
;               the first word of the lpIn structure and so passed it on.
;
; Arguments:    Control Display Driver Stack Frame
;               ds      Context Selector
;
; Returns:      ecx     0   queried-escape was not recognized
;                       1   queried-escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds
;==============================================================================
DECPROC HW_ControlQuery, PASCAL, FRAME, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     ebx,ebx
        les     bx,lpIn
        mov     ecx,1
        movzx   edx,word ptr es:[ebx]

        cmp     edx,NV_ESC_INTEL_DIAG
        jne     @F
        mov     eax,1
        jmp     Done
@@:
        ; Unrecognized query escape
        sub     ecx,ecx
Done:
CLOSEPROC


;==============================================================================
;
; Function:     HW_ControlAction
;
; Purpose:      This function implements a hardware specific escape call.
;               The generic code did not recognize the escape number and
;               so passed it on.
;
; Arguments:    Control Display Driver Stack Frame
;
; Returns:      ecx     0   escape was not recognized
;                       1   escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds
;==============================================================================
DECPROC HW_ControlAction, PASCAL, FRAME, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        movzx   eax,wFunction

        push    OFFSET ReturnAddr
        cmp     eax,NV_ESC_DMAPUSHMODESWITCHCOUNT
        je      NV_GetFlatModeSwitchPtrCount
        cmp     eax,NV_ESC_GETACTIVECHANNELPTR
        je      NV_GetFlatActiveChannelPtr
IFDEF   NV_ESCAPE_ID
        cmp     eax,NV_ESCAPE_ID
        je      NV_GetEscapeID
ENDIF
        cmp     eax,NV_ESC_IS_NVIDIA_BOARD_AND_DRIVER
        je      NV_IsNvidiaBasedBoardAndDiver
        cmp     eax,NV_ESC_GET_HARDWARE_POINTERS
        je      NV_GetHardwarePointers
        cmp     eax,NV_ESC_GET_FLAT_DIRECTX_SHARED_DATA
        je      NV_GetFlatDirectXSharedData
        cmp     eax,NV_ESC_INC_MODE_SWITCH
        je      NV_IncModeSitchCounter
        cmp     eax,NV_ESC_INTEL_DIAG
        je      NV_IntelDiags
        add     sp,@WordSize
        sub     ecx,ecx
        jmp     Done

ReturnAddr:
Done:
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetFlatModeSwitchPtrCount
;
; Purpose:      This function returns a flat ptr to the mode switch
;               count variable that open gl uses to track modeswitch
;               state and changes.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;
; Returns:      ecx     0   escape was not recognized
;                       1   escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds
;==============================================================================
DECPROC NV_GetFlatModeSwitchPtrCount, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     esi,pHWLogdevCXOffset
        mov     edi,ds:[esi].HWLOGDEVCX.pHWBoardCX

        mov     ax,ds
        push    ax
        call    GetSelectorBase
        shl     edx,10H
        mov     dx,ax
        lea     eax,[edi].HWBOARDCX.dwModeSwitchCount
        add     eax,edx

        sub     edi,edi
        les     di,lpOut
        mov     es:[edi],eax
        mov     eax,1
        mov	ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetFlatActiveChannelPtr
;
; Purpose:      This function returns a flat ptr to the current
;               channelID variable that is used to sync between
;               channels.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;
; Returns:      ecx     0   escape was not recognized
;                       1   escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds
;==============================================================================
DECPROC NV_GetFlatActiveChannelPtr, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     esi,pHWLogdevCXOffset
        mov     edi,ds:[esi].HWLOGDEVCX.pHWBoardCX

        mov     ax,ds
        push    ax
        call    GetSelectorBase
        shl     edx,10H
        mov     dx,ax
        mov     eax,[edi].HWBOARDCX.pCurrentChannelID
        add     eax,edx

        sub     edi,edi
        les     di,lpOut
        mov     es:[edi],eax
        mov     eax,1
        mov	ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetEscapeID
;
; Purpose:      This function returns a constant indicating that
;               this is an NVIDIA driver (as opposed to an OEM
;               driver based upon nvidia source).
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;
; Returns:      ecx     0   escape was not recognized
;                       1   escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds
;==============================================================================
DECPROC NV_GetEscapeID, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     eax,eax
        cmp     eax,lpOut
        je      @F
        sub     edi,edi
        les     di,lpOut
        mov     dword ptr es:[edi],NV_ESCAPE_ID_RETURN
        mov     eax,1
@@:
        mov	ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_IsNvidiaBasedBoardAndDiver
;
; Purpose:      This function returns a constant indicating that
;               this is an NVIDIA based board and driver (it might
;               be nvidia's own board and driver or a customers
;               modified driver and board).
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;
; Returns:      ecx     0   escape was not recognized
;                       1   escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds
;==============================================================================
DECPROC NV_IsNvidiaBasedBoardAndDiver, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     esi,pHWLogdevCXOffset
	push	ds:[esi].HWLOGDEVCX.pHWCrtcCX
	call	HW_GetPhysicalCrtcIndex

        sub     edi,edi
        les     di,lpOut
        mov     dword ptr es:[edi],NV_ESC_IS_NVIDIA_BOARD_AND_DRIVER
        mov     ebx,ds:[esi].HWLOGDEVCX.pHWBoardCX
        mov     ecx,ds:[ebx].HWBOARDCX.dwDeviceNum
        mov     es:[edi + 4],ecx
        mov     es:[edi + 8],eax
        mov     eax,1
        mov	ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetHardwarePointers
;
; Purpose:      This function returns a ptr to the screen and
;               the linear base of the chip registers.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;
; Returns:      ecx     0   escape was not recognized
;                       1   escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds
;==============================================================================
DECPROC NV_GetHardwarePointers, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     esi,pHWLogdevCXOffset
        mov     esi,ds:[esi].HWLOGDEVCX.pHWBoardCX

        sub     edi,edi
        les     di,lpOut

        sub     eax,eax
        mov     es:[edi].HARDWAREPOINTERS.lp16Chip,eax
        mov     eax,ds:[esi].HWBOARDCX.dwVideoMemorySel
        shl     eax,10H
        mov     es:[edi].HARDWAREPOINTERS.lp16FrameBuffer,eax
        mov     eax,ds:[esi].HWBOARDCX.pVideoMemory
        mov     es:[edi].HARDWAREPOINTERS.lp32FrameBuffer,eax
        mov     eax,ds:[esi].HWBOARDCX.pChipRegs
        mov     es:[edi].HARDWAREPOINTERS.lp32Chip,eax
        mov     eax,1
        mov	ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_GetFlatDirectXSharedData
;
; Purpose:      This function returns a flat ptr to the
;               DirectX shared data structure.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;
; Returns:      ecx     0   escape was not recognized
;                       1   escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds
;==============================================================================
DECPROC NV_GetFlatDirectXSharedData, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     esi,pHWLogdevCXOffset

        push    word ptr ds:[esi].HWLOGDEVCX.lpDirectXSharedData+2
        call    GetSelectorBase
        shl     edx,10H
        mov     dx,ax
        xor     edi,edi
        les     di,lpOut
        mov     es:[edi],edx
        mov     eax,1
        mov	ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_IncModeSitchCounter
;
; Purpose:      This function increments the mode switch counter.
;               It is used for testing OGL stuff.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;
; Returns:      ecx     0   escape was not recognized
;                       1   escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds
;==============================================================================
DECPROC NV_IncModeSitchCounter, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     esi,pHWLogdevCXOffset
        mov     esi,ds:[esi].HWLOGDEVCX.pHWBoardCX
        inc     ds:[esi].HWBOARDCX.dwModeSwitchCount

        cmp     lpOut,0
        je      @F
        sub     edi,edi
        les     di,lpOut
        mov     eax,ds:[esi].HWBOARDCX.dwModeSwitchCount
        mov     es:[edi],eax
@@:
        mov     eax,1
        mov	ecx,1
CLOSEPROC


;==============================================================================
;
; Function:     NV_IntelDiags
;
; Purpose:      This function is a set of Intel diagnostics.
;               Intel requires this Escape and diagnostics in
;               order to purchase our boards.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;
; Returns:      ecx     0   escape was not recognized
;                       1   escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds
;==============================================================================
DECPROC NV_IntelDiags, PASCAL, ShareFrameNear16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        push    pHWLogdevCXOffset
        push    lpIn
        push    lpOut
        call    NV_IntelDiagnostics
CLOSEPROC

CLOSESEG _NVTEXT16

END

