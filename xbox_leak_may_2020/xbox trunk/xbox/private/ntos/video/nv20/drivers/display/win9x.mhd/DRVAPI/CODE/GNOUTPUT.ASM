;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnoutput.asm
;
; Purpose:      This file holds the Output display driver entry point.
;
;==============================================================================
.586
incOutput = 1
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

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

; These all have to be marked as NEAR or else the conditional jumps to them
; below will cause an assembler error. They are really FAR16 functions.
EXTERNDEF       GENERIC_Rectangle:NEAR
EXTERNDEF       GENERIC_Polyline:NEAR
EXTERNDEF       GENERIC_PolyScanOrScanLine:NEAR
EXTERNDEF       GENERIC_BeginScan:NEAR
EXTERNDEF       GENERIC_EndScan:NEAR

;==============================================================================
;
; Function:     GENERIC_Output
;
; Purpose:      This function is the Output display driver entry point.
;
; Arguments:    Output Display Driver Stack Frame
;
;
; Returns:      ax      1       success
;                       0       failure
;                       -1      have GDI simulate
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_Output, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMW   wStyle
;PARMW   wCount
;PARMD   lpPoints
;PARMD   lpPen
;PARMD   lpBrush
;PARMD   lpDrawMode
;PARMD   lpClipRect
OPENPROC
        movzx   ebx,sp
        movzx   eax,word ptr ss:[ebx + 1AH]
        cmp     eax,OS_RECTANGLE
        je      GENERIC_Rectangle
        cmp     eax,OS_POLYLINE
        je      GENERIC_Polyline
        cmp     eax,OS_POLYSCANLINE
        je      GENERIC_PolyScanOrScanLine
        cmp     eax,OS_BEGINNSCAN
        je      GENERIC_BeginScan
        cmp     eax,OS_ENDNSCAN
        je      GENERIC_EndScan
        cmp     eax,OS_SCANLINES
        je      GENERIC_PolyScanOrScanLine
        cmp     eax,OS_ALTPOLYGON
        je      O_Ret
        cmp     eax,OS_WINDPOLYGON
        jne	Punt_Output     
O_Ret:	mov     eax,-1
        RETFAR16 1CH
PLABEL	Punt_Output
        jmp     DIB_Output
CLOSEPROC

CLOSESEG _NVTEXT32

END

