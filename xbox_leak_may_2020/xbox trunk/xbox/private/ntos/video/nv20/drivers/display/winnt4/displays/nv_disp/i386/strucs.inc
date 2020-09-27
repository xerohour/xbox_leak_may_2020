;*************************** Include File Header *******************************
;
; Module Name: strucs.inc
;
; Most of these definitions are pulled straight from winddi.h.  We have
; to copy these here because we don't have any good 'h2inc' C-include-to-
; Asm-include program for NT.
;
; Copyright (c) 1993-1995 Microsoft Corporation
;
;*******************************************************************************

;*******************************************************************************
;*                                                                             *
;*   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
;*                                                                             *
;*******************************************************************************

; !!! CAUTION CAUTION !!! See Note !!!
DMAPUSH_BUFFER_SIZE EQU 200000h      ; NOTE: If you change this value here, change
DMAPUSH_PADDING     EQU 32           ;       it also in DRIVER.H
; !!! CAUTION CAUTION !!!

FALSE                       EQU 0
TRUE                        EQU 1
MAX_CX_CY_MEMORY_VALUE      EQU 7fffh   ; Hardware limitation

BMF_1BPP            equ     1
BMF_4BPP            equ     2
BMF_8BPP            equ     3
BMF_16BPP           equ     4
BMF_24BPP           equ     5
BMF_32BPP           equ     6

;-----------------------------------------------------------------------;
; POINTL is used for points with a range of +/- 2G.
;-----------------------------------------------------------------------;

POINTL              STRUC
ptl_x               DD      0
ptl_y               DD      0
POINTL              ENDS

;-----------------------------------------------------------------------;
; SIZEL is used for extents with a range of +/- 2G
;-----------------------------------------------------------------------;

SIZEL               STRUC
sizl_cx             DD      0
sizl_cy             DD      0
SIZEL               ENDS

;-----------------------------------------------------------------------;
; Definition of a rectangle
;-----------------------------------------------------------------------;

RECTL               struc           ; /* rcl */
xLeft               dd      ?
yTop                dd      ?
xRight              dd      ?
yBottom             dd      ?
RECTL               ends

;-----------------------------------------------------------------------;
; Constants for accessing frame buffer via pfnBankSelectMode
;-----------------------------------------------------------------------;

BANK_OFF            equ     0
BANK_ON             equ     1
BANK_ON_NO_WAIT     equ     2

;-----------------------------------------------------------------------;
; Definition of our realized brush structures.
;
; NOTE: Changes to these structure must be reflected in driver.h!
;-----------------------------------------------------------------------;

RBRUSH              struc           ; /* rb, prb */
rb_fl                   dd  ?
rb_bTransparent         dd  ?
rb_ulForeColor          dd  ?
rb_ulBackColor          dd  ?
rb_ptlBrushOrg          db  (size POINTL) dup(?)
rb_apbe                 dd  ?

; I haven't declared rb_aulPattern because we don't know the size of the
; rb_apbe array here.

RBRUSH              ends

BRUSHENTRY          struc           ; /* be, pbe */
be_prbVerify            dd  ?
be_x                    dd  ?
be_y                    dd  ?
BRUSHENTRY          ends

NV04ControlDma  struc
    reserved        dd  10h dup(?)
    ChannelPut      dd  ?
    ChannelGet      dd  ?
NV04ControlDma  ends

SURFOBJ struc
    dhsurf          dd  ?
    hsurf           dd  ?
    dhpdev          dd  ?
    hdev            dd  ?
    sizlBitmapx     dd  ?
    sizlBitmapy     dd  ?
    cjBits          dd  ?
    pvBits          dd  ?
    pvScan0         dd  ?
    lDelta          dd  ?
    iSOUniq         dd  ?
    iBitmapFormat   dd  ?
    iType           dw  ?
    fjBitmap        dw  ?
SURFOBJ ends

PDEV_ASM    struc
    NVSignature         dd      ?   ; Unique NV signature
    xOffset             dd      ?   ; Pixel offset from (0, 0) to current
    yOffset             dd      ?   ;  DFB located in off-screen memory
    pjMmBase            dd      ?   ; We'll use it as a ptr to our NV Channel
    GrStatusReg         dd      ?   ; We'll use it as a ptr to our Gr Status Reg
    FbConfig0Reg        dd      ?   ; We'll use it as a ptr to our Fb Config0 Reg
    FbStartAddr         dd      ?   ; We'll use it as a ptr to our Fb Start Addr
    DACRegs             dd      ?   ; We'll use it as a ptr to our Cursor registers
    PRMCIORegs          dd      ?   ; NV3 PRMCIO Regs
    PRMVIORegs          dd      ?   ; NV3 PRMVIO Regs
    PRAMDACRegs         dd      ?   ; NV3 PRAMDAC Regs
    PRAMINRegs          dd      ?   ; NV3 PRAMIN Regs
    PGRAPHRegs          dd      ?   ; NV3 PGRAPH Regs
    PFIFORegs           dd      ?   ; NV3 PFIFO Regs
    FbBufferToggle      dd      ?   ; 0 or 1 (denotes frame buffer)
    pjFrameBufbase      dd      ?   ; pjFrameBuf base pointing to the base of frame buffer
    pjScreen            dd      ?   ; Points to base screen address
    lDelta              dd      ?   ; Distance from one scan to the next.
    cjPelSize           dd      ?   ; 1 if 8bpp, 2 if 16bpp, 3 if 24bpp, 4 if 32bpp
    iBitmapFormat       dd      ?   ; BMF_8BPP or BMF_16BPP or BMF_32BPP (color depth
    iBoard              dd      ?   ; Logical multi-board identifier (default zero)
    fontCacheOffset     dd      ?   ; Offset into font cache offscreen area
    fontCacheEnabled    dd      ?   ; Offset into font cache offscreen area
    flCaps              dd      ?   ; Capabilities flags
    flStatus            dd      ?   ; Status flags
    bEnabled            dd      ?   ; In graphics mode (not full-screen)

    hDriver             dd      ?   ; Handle to \Device\Screen
    hdevEng             dd      ?   ; Engine's handle to PDEV

    cxScreen            dd      ?   ; Visible screen width
    cyScreen            dd      ?   ; Visible screen height
    cxMemory            dd      ?   ; Width of Video RAM
    cyMemory            dd      ?   ; Height of Video RAM
    cBitsPerPel         dd      ?   ; Bits per pel (8, 15, 16, 24 or 32)
    cbFrameBuf          dd      ?   ; Size of frame buffer in bytes
    AlphaEnableValue    dd      ?   ; Global alpha enable value

    DmaPushEnabled2D    dd      ?
    DmaPushBufTotalSize dd      ?
    DmaPushBufCanUseSize dd     ?
    DmaPushBufMinSize   dd      ?
    nvDmaPushHandle     dd      ?
    nvDmaFifo           dd      ?
    nvDmaCount          dd      ?
    nvDmaChannel        dd      ?
    nvDmaCachedGet      dd      ?
    nvDmaCachedPut      dd      ?
    nvDmaPreviousPut    dd      ?
    nvDmaWrapFlag       dd      ?
    nvDmaCachedFreeCount dd     ?
    nvDmaFlushFixDwordPtr dd    ?
    nvDmaFlushFixDummyValue dd    ?
    
PDEV_ASM    ends

;-----------------------------------------------------------------------;
; Definitions for text output
;-----------------------------------------------------------------------;

RECT_AND_TEXT_SUBCHANNEL    EQU 2*8192
NV04A_CLIP_POINT0_C         EQU 07ECh
NV04A_CLIP_POINT1_C         EQU 07F0h
NV04A_MONOCHROME_COLOR1_C   EQU 0800h
NV04A_SIZE_IN_E             EQU 0BF4h
NV04A_SIZE_OUT_E            EQU 0BF8h
NV04A_POINT_E               EQU 0BFCh
NV04A_FONTF                 EQU 0FF0h
NV04A_CHARACTER_COLOR1_F    EQU 1000h
MAXMONODWORDS               EQU 1 SHL MAXMONOSHIFT  ; 128
MAXMONOSHIFT                EQU 7

;-----------------------------------------------------------------------;
; Definitions for image from CPU output
;-----------------------------------------------------------------------;

IMAGE_FROM_CPU_SUBCHANNEL   EQU 0*8192
MAX_INDEXED_IMAGE_DWORDS    EQU 1792
NV061_COLOR0                EQU 400h

GLYPHPOS    struc           ; /* gp */
gp_hg       dd      ?       ; hglyph (unused by vga)
gp_pgdf     dd      ?       ; glyphdef
gp_x        dd      ?       ; glyph x position
gp_y        dd      ?       ; glyph y position
GLYPHPOS    ends

GLYPHDEF struc              ; /* gdf */
gdf_pgb     dd      ?       ; points to GLYPHBITS
GLYPHDEF ends

GLYPHBITS struc             ; /* gb */
gb_x        dd      ?       ; glyph x offset
gb_y        dd      ?       ; glyph y offset
gb_cx       dd      ?       ; glyph width in pels
gb_cy       dd      ?       ; glyph height in pels
gb_aj       db      ?       ; glyph bit mask
GLYPHBITS ends

STROBJ  struc
    cGlyphs          dd  ?       ; # of glyphs to render
    flAccel          dd  ?       ; accel flags
    ulCharInc        dd  ?       ; non-zero only if fixed pitch font, equal to advanced width.
    rclBGxleft       dd  ?       ; bk ground  rect of the string in device coords
    rclBGytop        dd  ?       ; bk ground  rect of the string in device coords
    rclBGxright      dd  ?       ; bk ground  rect of the string in device coords
    rclBGybottom     dd  ?       ; bk ground  rect of the string in device coords
    pgp              dd  ?       ; If non-NULL then has all glyphs.
    pwszOrg          dd  ?       ; pointer to original unicode string.
STROBJ  ends

FONTINFO  struc
    cjThis           dd  ?       ; size of FONTINFO structure in bytes
    flCaps           dd  ?       ; capabilities flag
    cGlyphsSupported dd  ?       ; number of glyphs in the font
    cjMaxGlyph1      dd  ?       ; size oflargest glyph in 1 bit/pixel
    cjMaxGlyph4      dd  ?       ; size oflargest glyph in 4 bit/pixel
    cjMaxGlyph8      dd  ?       ; size oflargest glyph in 8 bit/pixel
    cjMaxGlyph32     dd  ?       ; size oflargest glyph in 32 bit/pixel
FONTINFO  ends

FONTOBJ   struc
    iUniq            dd  ?       ;
    iface            dd  ?       ;
    cxMax            dd  ?       ;
    flFontType       dd  ?       ;
    iTTUniq          dd  ?       ;
    iFile            dd  ?       ;
    sizeLogResPpix   dd  ?       ;
    sizeLogResPpiy   dd  ?       ;
    ulStyleSize      dd  ?       ;
    pvConsumer       dd  ?       ;
    pvProducer       dd  ?       ;
FONTOBJ   ends

;-----------------------------------------------------------------------;
; Macro to do a simple RET, with no stack stuff, in a proc.
;-----------------------------------------------------------------------;

PLAIN_RET macro
        db      0c3h
        endm
