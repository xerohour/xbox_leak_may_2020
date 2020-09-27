;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnstub.asm
;
; Purpose:      This file holds all the display driver entry points
;               that we just end up punting to the DIBENG or returning
;               true or false -- all the stubs basically.
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
; Function:     GENERIC_GetCharWidth
;
; Purpose:      This function is the GetCharWidth display driver entry point.
;
;==============================================================================
DECPROC GENERIC_GetCharWidth, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMD   lpBuffer
;PARMW   wFirstChar
;PARMW   wLastChar
;PARMD   lpFontInfo
;PARMD   lpDrawMode
;PARMD   lpTextXForm
OPENPROC
        jmp     DIB_GetCharWidth
CLOSEPROC

IF 0
;==============================================================================
;
; Function:     GENERIC_ColorInfo
;
; Purpose:      This function is the ColorInfo display driver entry point.
;
;==============================================================================
DECPROC GENERIC_ColorInfo, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMD   dwColorIn
;PARMD   lpPhysColor
OPENPROC
        jmp     DIB_ColorInfo
CLOSEPROC
ENDIF

;==============================================================================
;
; Function:     GENERIC_DeviceBitmap
;
; Purpose:      This function is the DeviceBitmap display driver entry point.
;
; Returns:      Always return 0.
;
;==============================================================================
DECPROC GENERIC_DeviceBitmap, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMW   wCommand
;PARMD   lpBitmap
;PARMD   lpBits
OPENPROC
        sub     eax,eax
        RETFAR16  0EH
CLOSEPROC

;==============================================================================
;
; Function:     GENERIC_EnumDFonts
;
; Purpose:      This function is the EnumDFonts display driver entry point.
;
; Returns:      Always return 1.
;
;==============================================================================
DECPROC GENERIC_EnumDFonts, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMD   lpFaceName
;PARMD   lpfnCallback
;PARMD   lpClinetData
OPENPROC
        mov     eax,1
        RETFAR16  10H
CLOSEPROC

;==============================================================================
;
; Function:     GENERIC_EnumObj
;
; Purpose:      This function is the EnumObj display driver entry point.
;
;==============================================================================
DECPROC GENERIC_EnumObj, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMD   lpFaceName
;PARMD   lpfnCallback
;PARMD   lpClinetData
OPENPROC
        pop     eax
        push    cs:[CSlp16LogDevice_In_NVTEXT16]
        push    eax
        jmp     DIB_EnumObjExt
CLOSEPROC

;==============================================================================
;
; Function:     GENERIC_FastBorder
;
; Purpose:      This function is the FastBorder display driver entry point.
;
; Returns:      Return 0 so GDI will simulate via BitBlt.
;
;==============================================================================
DECPROC GENERIC_FastBorder, PASCAL, NOFRAME, FAR16
;PARMD   lpRect
;PARMW   wHorzBorderThick
;PARMW   wVertBorderThick
;PARMD   dwRop
;PARMD   lpDstDev
;PARMD   lpBrush
;PARMD   lpDrawMode
;PARMD   lpClipRect
OPENPROC
        sub     eax,eax
        RETFAR16  1CH
CLOSEPROC

;==============================================================================
;
; Function:     GENERIC_GetPalette
;
; Purpose:      This function is the GetPalette display driver entry point.
;
;==============================================================================
DECPROC GENERIC_GetPalette, PASCAL, NOFRAME, FAR16
;PARMW   wStartIndex
;PARMW   wNumEntries
;PARMD   lpPalette
OPENPROC
        pop     eax
        push    cs:[CSlp16LogDevice_In_NVTEXT16]
        push    eax
        jmp     DIB_GetPaletteExt
CLOSEPROC

;==============================================================================
;
; Function:     GENERIC_GetPaletteTranslate
;
; Purpose:      This function is the GetPaletteTranslate display driver entry
;               point.
;
;==============================================================================
DECPROC GENERIC_GetPaletteTranslate, PASCAL, NOFRAME, FAR16
;PARMD   lpTranslate
OPENPROC
        pop     eax
        push    cs:[CSlp16LogDevice_In_NVTEXT16]
        push    eax
        jmp     DIB_GetPaletteTranslateExt
CLOSEPROC

;==============================================================================
;
; Function:     GENERIC_Inquire
;
; Purpose:      This function is the Inquire display driver entry point.
;
;==============================================================================
DECPROC GENERIC_Inquire, PASCAL, NOFRAME, FAR16
;PARMD   lpCursorInfo
OPENPROC
        jmp     DIB_Inquire
CLOSEPROC

;==============================================================================
;
; Function:     GENERIC_ScanLR
;
; Purpose:      This function is the ScanLR display driver entry point.
;
;==============================================================================
DECPROC GENERIC_ScanLR, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMW   wX
;PARMW   wY
;PARMD   dwPhysColor
;PARMW   wStyle
OPENPROC
        jmp     DIB_ScanLR
CLOSEPROC

;==============================================================================
;
; Function:     GENERIC_SelectBitmap
;
; Purpose:      This function is the SelectBitmap display driver entry point.
;
; Returns:      Always return 1.
;
;==============================================================================
DECPROC GENERIC_SelectBitmap, PASCAL, NOFRAME, FAR16
;PARMD   lpDevice
;PARMD   lpPrevBitmap
;PARMD   lpBitmap
;PARMD   dwFlags
OPENPROC
        mov     eax,1
        RETFAR16  10H
CLOSEPROC

;==============================================================================
;
; Function:     GENERIC_SetAttribute
;
; Purpose:      This function is the SetAttribute display driver entry point.
;
; Returns:      Always return 0.
;
;==============================================================================
DECPROC GENERIC_SetAttribute, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMW   wStateNum
;PARMW   wIndex
;PARMD   dwAttribute
OPENPROC
        sub     eax,eax
        RETFAR16  0CH
CLOSEPROC

;==============================================================================
;
; Function:     GENERIC_Strblt
;
; Purpose:      This function is the Strblt display driver entry point.
;
; Arguments:
;
; Returns:      This function pushes some additional parameters and
;               jumps to the ExtTextOut function. See ExtTextOut for
;               appropiate return values.
;
;==============================================================================
DECPROC GENERIC_Strblt, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMW   wDstX
;PARMW   wDstY
;PARMD   lpClipRect
;PARMD   lpString
;PARMW   wCount
;PARMD   lpFontInfo
;PARMD   lpDrawMode
;PARMD   lpTextXForm
OPENPROC
        sub     ebx,ebx
        pop     eax
        push    ebx
        push    ebx
        push    bx
        push    eax
        jmp     GENERIC_ExtTextOut
CLOSEPROC

CLOSESEG _NVTEXT16

END

