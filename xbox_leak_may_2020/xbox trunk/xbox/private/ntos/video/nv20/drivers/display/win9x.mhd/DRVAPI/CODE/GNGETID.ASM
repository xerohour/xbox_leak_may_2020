;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gngetrid.asm
;
; Purpose:      This file holds the GetDriverResourceID display driver
;               entry point.
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

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     GENERIC_GetDriverResourceID
;
; Purpose:      This function is the GetDriverResourceID display driver
;               entry point.
;
; Arguments:    GetDriverResourceID Display Driver Stack Frame
;
; Returns:      The remapped resource id.
;
;==============================================================================
DECPROC GENERIC_GetDriverResourceID, PASCAL, FRAME, FAR16
PARMW   wResID
PARMD   lpResType
OPENPROC
        PUSHR   ds
        sub     edx,edx
        movzx   eax,wResID
        cmp     eax,3
        jne     @F
        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     ebx,cs:[CSpGNLogicalDevice_In_NVTEXT16]
        lds     bx,ds:[ebx].GNLOGDEVCX.lpGdiInfo
        cmp     ds:[ebx].GDIINFO.dpLogPixelsX,96
        je      @F
        mov     eax,2003
@@:     POPR    ds
CLOSEPROC

CLOSESEG _NVTEXT16

END

