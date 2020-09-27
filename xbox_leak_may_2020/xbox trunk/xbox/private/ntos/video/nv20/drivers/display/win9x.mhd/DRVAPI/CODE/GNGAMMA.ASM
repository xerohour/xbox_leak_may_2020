;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gngamma.asm
;
; Purpose:      This file holds the GammaRamp display driver entry point.
;
;==============================================================================
.586
incDrawMode = 1
incDevice   = 1
incLogical  = 1
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GENERIC_GammaRamp
;
; Purpose:      This function is the GammaRamp display driver entry point.
;
; Arguments:    GammaRamp Display Driver Stack Frame
;
; Returns:      Always return 1.
;
;==============================================================================
DECPROC GENERIC_GammaRamp, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   fGetSet
PARMD   lpGammaRamp
LOCALD  dwCRTCCount
LOCALD  pGammaRamp
OPENPROC
        PUSHR   ds,esi,edi

        ; Load up the GNLOGDEVCX
        mov     ds,cs:[wFlatDataSel]
        mov     esi,cs:[CSpGNLogicalDevice]

        ; Get a flat ptr to the gamma ramp
        movzx   eax,word ptr lpGammaRamp+2
        push    eax
        call    GN_GetSelectorBase
        movzx   edx,word ptr lpGammaRamp+0
        add     eax,edx
        mov     pGammaRamp,eax

        ; Run through all the CRTCs attached to this logical device
        ; and program them all, or get the gamma ramp from the first.
        ; The first is sufficient on a Get since if there is more
        ; than 1 CRTC attached, then we are in CLONE mode.
        mov     dwCRTCCount,0
        lea     edi,ds:[esi].GNLOGDEVCX.pGNCrtcCX

Loopit3:
        ; Now call to set this mode
        cmp     dword ptr ds:[edi],0
        je      SkipIt3
        cmp     fGetSet,0
        jne     SetGamma
GetGamma:
        push    dword ptr ds:[edi]
        push    pGammaRamp
        call    GN_GetCrtcGamma
        jmp     DoneIt

SetGamma:
        push    dword ptr ds:[edi]
        push    pGammaRamp
        call    GN_SetCrtcGamma
SkipIt3:
        add     edi,4
        inc     dwCRTCCount
        mov     eax,dwCRTCCount
	cmp	eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      Loopit3

DoneIt:
        mov     eax,1
        POPR    ds,esi,edi
CLOSEPROC

CLOSESEG _NVTEXT32

END

