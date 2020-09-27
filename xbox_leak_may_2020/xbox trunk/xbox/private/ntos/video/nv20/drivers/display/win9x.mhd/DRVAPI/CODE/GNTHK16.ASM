;==============================================================================
;
; Copyright (C) 1998, NVidia Coporation
;
; File:         gnblt.asm
;
; Purpose:      This file holds the 16 bit entry point for compatibility
;               with some old apps that patch display driver entry points
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

ReserveForPatch	macro name
	nop
	nop
	nop
	nop
	nop
public	Patch_&name&
Patch_&name&	proc	FAR16
Patch_&name&	endp
        jmp     GENERIC_&name&
	endm

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

PUBLIC  wLog, wBreak16
wLog                    DW      1
wBreak16                DW      0

;==============================================================================
;
; Function:     Bitblt
;
; Purpose:      This function is the 16 Bit BitBlt display driver
;               entry point. It jumps to the real 32 Bit BitBlt to
;               do all the work.
;
; Arguments:    BitBlt Display Driver Stack Frame
;
;==============================================================================
DECPROC BitBlt, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	BitBlt
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor
;
; Purpose:      This function is the 16 Bit SetCursor display driver
;               entry point. It jumps to the real 32 Bit SetCursor to
;               do all the work.
;
; Arguments:    SetCursor Display Driver Stack Frame
;
;==============================================================================
DECPROC SetCursor, PASCAL, NOFRAME, FAR16
OPENPROC
        jmp     GENERIC_SetCursor
CLOSEPROC


;==============================================================================
;
; Function:     MoveCursor
;
; Purpose:      This function is the 16 Bit MoveCursor display driver
;               entry point. It jumps to the real 32 Bit MoveCursor to
;               do all the work.
;
; Arguments:    MoveCursor Display Driver Stack Frame
;
;==============================================================================
DECPROC MoveCursor, PASCAL, NOFRAME, FAR16
OPENPROC
        jmp     GENERIC_MoveCursor
CLOSEPROC


;==============================================================================
;
; Function:     SetPalette
;
; Purpose:      This function is the 16 Bit SetPalette display driver
;               entry point. It jumps to the real 32 Bit SetPalette to
;               do all the work.
;
; Arguments:    SetPalette Display Driver Stack Frame
;
;==============================================================================
DECPROC SetPalette, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	SetPalette
CLOSEPROC

;==============================================================================
;
; Function:     BitmapBits
;
; Purpose:      This function is the 16 Bit BitmapBits display driver
;               entry point. It jumps to the real 32 Bit BitmapBits to
;               do all the work.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;
;==============================================================================
DECPROC BitmapBits, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	BitmapBits
CLOSEPROC

;==============================================================================
;
; Function:     Control
;
; Purpose:      This function is the 16 Bit Control display driver
;               entry point. It jumps to the real 32 Bit Control to
;               do all the work.
;
; Arguments:    Control Display Driver Stack Frame
;
;==============================================================================
DECPROC Control, PASCAL, NOFRAME, FAR16
OPENPROC
        jmp     GENERIC_Control
CLOSEPROC

;==============================================================================
;
; Function:     ExtTextOut
;
; Purpose:      This function is the 16 Bit ExtTextOut display driver
;               entry point. It jumps to the real 32 Bit ExtTextOut to
;               do all the work.
;
; Arguments:    ExtTextOut Display Driver Stack Frame
;
;==============================================================================
DECPROC ExtTextOut, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	ExtTextOut
CLOSEPROC

;==============================================================================
;
; Function:     DibBlt
;
; Purpose:      This function is the 16 Bit DibBlt display driver
;               entry point. It jumps to the real 32 Bit DibBlt to
;               do all the work.
;
; Arguments:    DibBlt Display Driver Stack Frame
;
;==============================================================================
DECPROC DibBlt, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	DibBlt
CLOSEPROC

;==============================================================================
;
; Function:     FastBorder
;
; Purpose:      This function is the 16 Bit FastBorder display driver
;               entry point.
;
; Arguments:    FastBorder Display Driver Stack Frame
;
;==============================================================================
DECPROC FastBorder, PASCAL, NOFRAME, FAR16
OPENPROC
        sub     eax,eax
        retf	1CH
CLOSEPROC

;==============================================================================
;
; Function:     Output
;
; Purpose:      This function is the 16 Bit Output display driver
;               entry point. It jumps to the real 32 Bit Output to
;               do all the work.
;
; Arguments:    Output Display Driver Stack Frame
;
;==============================================================================
DECPROC Output, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	Output
CLOSEPROC

;==============================================================================
;
; Function:     Pixel
;
; Purpose:      This function is the 16 Bit Pixel display driver
;               entry point. It jumps to the real 32 Bit Pixel to
;               do all the work.
;
; Arguments:    Pixel Display Driver Stack Frame
;
;==============================================================================
DECPROC Pixel, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	Pixel
CLOSEPROC

;==============================================================================
;
; Function:     RealizeObject
;
; Purpose:      This function is the 16 Bit RealizeObject display driver
;               entry point. It jumps to the real 32 Bit RealizeObject to
;               do all the work.
;
; Arguments:    RealizeObject Display Driver Stack Frame
;
;==============================================================================
DECPROC RealizeObject, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	RealizeObject
CLOSEPROC

;==============================================================================
;
; Function:     ReEnable
;
; Purpose:      This function is the 16 Bit ReEnable display driver
;               entry point. It jumps to the real 32 Bit ReEnable to
;               do all the work.
;
; Arguments:    ReEnable Display Driver Stack Frame
;
;==============================================================================
DECPROC ReEnable, PASCAL, NOFRAME, FAR16
OPENPROC
        jmp     GENERIC_ReEnable
CLOSEPROC

;==============================================================================
;
; Function:     ScanLR
;
; Purpose:      This function is the 16 Bit ScanLR display driver
;               entry point. It jumps to the real 32 Bit ScanLR to
;               do all the work.
;
; Arguments:    ScanLR Display Driver Stack Frame
;
;==============================================================================
DECPROC ScanLR, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	ScanLR
CLOSEPROC

;==============================================================================
;
; Function:     SetDIBitsToDevice
;
; Purpose:      This function is the 16 Bit SetDIBitsToDevice display driver
;               entry point. It jumps to the real 32 Bit SetDIBitsToDevice to
;               do all the work.
;
; Arguments:    SetDIBitsToDevice Display Driver Stack Frame
;
;==============================================================================
DECPROC SetDIBitsToDevice, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	SetDIBitsToDevice
CLOSEPROC

;==============================================================================
;
; Function:     SetPaletteTranslate
;
; Purpose:      This function is the 16 Bit SetPaletteTranslate display driver
;               entry point. It jumps to the real 32 Bit SetPaletteTranslate to
;               do all the work.
;
; Arguments:    SetPaletteTranslate Display Driver Stack Frame
;
;==============================================================================
DECPROC SetPaletteTranslate, PASCAL, NOFRAME, FAR16
OPENPROC
        jmp     GENERIC_SetPaletteTranslate
CLOSEPROC

;==============================================================================
;
; Function:     StrBlt
;
; Purpose:      This function is the 16 Bit StrBlt display driver
;               entry point. It jumps to the real 32 Bit StrBlt to
;               do all the work.
;
; Arguments:    StrBlt Display Driver Stack Frame
;
;==============================================================================
DECPROC StrBlt, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	StrBlt
CLOSEPROC

;==============================================================================
;
; Function:     StretchBlt
;
; Purpose:      This function is the 16 Bit StretchBlt display driver
;               entry point. It jumps to the real 32 Bit StretchBlt to
;               do all the work.
;
; Arguments:    StretchBlt Display Driver Stack Frame
;
;==============================================================================
DECPROC StretchBlt, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	StretchBlt
CLOSEPROC

;==============================================================================
;
; Function:     StretchDIBits
;
; Purpose:      This function is the 16 Bit StretchDIBits display driver
;               entry point. It jumps to the real 32 Bit StretchDIBits to
;               do all the work.
;
; Arguments:    StretchDIBits Display Driver Stack Frame
;
;==============================================================================
DECPROC StretchDIBits, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	StretchDIBits
CLOSEPROC


;==============================================================================
;
; Function:     SaveScreenBitmap
;
; Purpose:      This function is the 16 Bit SaveScreenBitmap display driver
;               entry point. It jumps to the real 32 Bit SaveScreenBitmap to
;               do all the work.
;
; Arguments:    SaveScreenBitmap Display Driver Stack Frame
;
;==============================================================================
DECPROC SaveScreenBitmap, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	SaveScreenBitmap
CLOSEPROC

;==============================================================================
;
; Function:     UpdateColors
;
; Purpose:      This function is the 16 Bit UpdateColors display driver
;               entry point. It jumps to the real 32 Bit UpdateColors to
;               do all the work.
;
; Arguments:    UpdateColors Display Driver Stack Frame
;
;==============================================================================
DECPROC UpdateColors, PASCAL, NOFRAME, FAR16
OPENPROC
	ReserveForPatch	UpdateColors
CLOSEPROC

IFDEF DEBUG
;==============================================================================
;
; Function:     GENERIC_OutputDebugString
;
; Purpose:      This function checks a global logging variable and then
;               calls OutputDebugString if logging is enabled.
;
;               DO NOT USE the DECPROC macro to define this routine.
;               The DECPROC macro logs when in debug mode. The logging
;               calls this routine. We will nest until the stack
;               overflows.
;
; Arguments:    16:16 ptr to string to print
;
; Returns:      None
;
; Preserve:     All registers
;==============================================================================
PUBLIC  GENERIC_OutputDebugString
GENERIC_OutputDebugString       PROC    FAR16

        cmp     cs:[wLog],0
        je      DoneIt

        push    bp
        mov     bp,sp
        PUSHR   alld,ds,es,fs,gs
        push    dword ptr ss:[bp + 06H]
        call    OutputDebugString
        POPR    alld,ds,es,fs,gs
        pop     bp
DoneIt:
        retf    4

GENERIC_OutputDebugString       ENDP
ENDIF

CLOSESEG _NVTEXT16

END

