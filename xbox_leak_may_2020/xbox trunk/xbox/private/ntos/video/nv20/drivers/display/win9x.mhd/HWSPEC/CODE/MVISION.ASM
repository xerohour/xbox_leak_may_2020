;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         mvision.asm
;
; Purpose:      This file holds the macrovision control code.
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include hwmacs.inc
include nvcm.inc
include nvos.inc
include nvmisc.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwcrtc.inc
include hwboard.inc


OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     HW_SetMacrovisionMode
;
; Purpose:      Sets the current Macrovision capable TV Out device to
;               the requested Macrovision copy protection mode.
;
; Arguments:    Mode		new macrovision copy protection mode to set
;               ds		Context Selector
;		pHWCrtcCXOffset	HWCRTCCX ptr
;               resX		PhysicalXRes
;               resY		PhysicalYRes
;
; Returns:      eax     0 then fail
;                       non-0 then success
;
; Preserve:     ds:esi,es,edi
;==============================================================================

DECPROC HW_SetMacrovisionMode, PASCAL, FRAME, NEAR
PARMD	Mode
PARMD	resX
PARMD	resY
PARMD	pHWCrtcCXOffset
LOCALV  Params, NV_CFGEX_SET_MACROVISION_MODE_PARAMS
OPENPROC

        PUSHR   ds,es,esi

	mov	eax,Mode
	mov     Params.mode,eax

        mov     esi,pHWCrtcCXOffset
        mov     eax,ds:[esi].HWCRTCCX.dwPhysicalCRTCInstance
        mov     Params.head,eax

        mov     eax,resX
        mov     Params.resx,eax
        mov     eax,resY
        mov     Params.resy,eax
        mov     ax,ss
        shl     eax,10H
        lea     ax,Params
        mov     ebx,SIZE NV_CFGEX_SET_MACROVISION_MODE_PARAMS
        mov     ecx,NV_CFGEX_SET_MACROVISION_MODE
        mov     esi,ds:[esi].HWCRTCCX.pHWBoardCX
        call    NV_ConfigExSetCall

	cmp	eax,NVOS_CGE_STATUS_SUCCESS
	mov	eax,1
	je	SMM_Ok
	sub	eax,eax

SMM_Ok:
        POPR    ds,es,esi
CLOSEPROC

CLOSESEG _NVTEXT16

END

