;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnpower.asm
;
; Purpose:      This file holds the routines dealing with power management support.
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

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

IFDEF DEBUG
szPowerOn       DB      'DISPLAY:GN_PowerOn',0DH,0AH,0
szPowerOff      DB      'DISPLAY:GN_PowerOff',0DH,0AH,0
ENDIF

;==============================================================================
;
; Function:     GN_PowerOn
;
; Purpose:      This function gets called from minivdd when
;               we are going to resume from powerdown.
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     All registers must be preserved
;==============================================================================
DECPROC GN_PowerOn, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi

IFDEF DEBUG
        mov     ax,cs
        shl     eax,10H
        lea     ax,szPowerOn
        push    eax
        call    GENERIC_OutputDebugString
ENDIF

        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]

        ; Send a message down the chain
        push    esi
        pushd   MSG_POWERON
        push    esi                             ; CX ptr to DIBENGINEHDR
        push	CSlp16LogDevice_In_NVTEXT16	; 16:16 ptr to DIBENGINEHDR
        call    GN_LogdevMessage

        mov     ds:[esi].GNLOGDEVCX.bSetModeBypass,0

        ; Do the Enable Device.
        sub     eax,eax
        push    CSlp16LogDevice_In_NVTEXT16
        push    ax
        push    eax
        push    eax
        inc     eax
        push    eax
        call    GENERIC_Enable

        mov     ds:[esi].GNLOGDEVCX.bSetModeBypass,1

        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_PowerOff
;
; Purpose:      This function gets called from minivdd when
;               we are going to resume from powerdown.
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     All registers must be preserved
;==============================================================================
DECPROC GN_PowerOff, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi

IFDEF DEBUG
        mov     ax,cs
        shl     eax,10H
        lea     ax,szPowerOff
        push    eax
        call    GENERIC_OutputDebugString
ENDIF

        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]

        ;Check for power call after INT2F
        inc     ds:[esi].GNLOGDEVCX.dwInt2FScreenSwitchOut
        cmp     ds:[esi].GNLOGDEVCX.dwInt2FScreenSwitchOut,2
        jge     @F

        ; Send a message down the chain
        push    esi
        pushd   MSG_POWEROFF
        push    esi                             ; CX ptr to DIBENGINEHDR
        push	CSlp16LogDevice_In_NVTEXT16	; 16:16 ptr to DIBENGINEHDR
        call    GN_LogdevMessage

        push    esi
        call    GN_BlankScreen

@@:
        POPR    ds,esi,edi
CLOSEPROC

CLOSESEG _NVTEXT16

END

