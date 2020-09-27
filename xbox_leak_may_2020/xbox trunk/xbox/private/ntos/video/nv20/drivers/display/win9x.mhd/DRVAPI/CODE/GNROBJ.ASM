;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation
;
; File:       gnrobj.asm
;
; Purpose:    This file holds the RealizeObject display driver entry point.
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
include	gnboard.inc
include gncrtc.inc
include gngdi.inc
include hwgdi.inc

DO_NOT_DITHER_16BPP_SOLID_BRUSHES       TEXTEQU <0>

OPENSEG  _DATA, USE16, DATA
EXTERNDEF                       szUserModuleName:BYTE
PUBLIC  szGDIModuleName
szGDIModuleName                 DB      'GDI',0

lfArray         LABEL   LogFont
LogFont         <-2048,0,0,0,400,0,0,0,0,0,0,0,18,'Times New Roman'>
LogFont         <-1000,0,0,0,400,0,0,0,0,2,0,1,0,'Arial'>
LogFont         <-228,108,0,0,700,0,0,0,0,0,0,2,0,'Arial'>
LogFont         <-169,0,0,0,400,0,0,0,0,4,0,0,32,'Tahoma'>
LogFont         <-96,0,0,0,400,0,0,0,0,0,0,1,34,'Arial'>
LogFont         <-88,44,0,0,700,0,0,0,0,0,0,2,0,'Arial'>
LogFont         <-86,0,0,0,400,0,0,0,0,0,0,0,18,'Times New Roman'>
LogFont         <-85,40,0,0,700,0,0,0,0,0,0,2,0,'Arial'>
LogFont         <-72,36,0,0,700,0,0,0,0,0,0,2,0,'Arial'>
LogFont         <-71,0,0,0,700,0,0,0,0,0,0,0,34,'Arial Narrow'>
LogFont         <-64,32,0,0,700,0,0,0,0,0,0,2,0,'Helvetica'>
LogFont         <-62,0,0,0,700,0,0,0,0,3,2,1,34,'Arial'>
LogFont         <-57,27,0,0,700,0,0,0,0,0,0,2,0,'Arial'>
LogFont         <-56,0,0,0,700,0,0,0,0,3,2,1,34,'Arial'>
LogFont         <-52,0,0,0,700,0,0,0,0,0,0,0,34,'Arial Narrow'>
LogFont         <-48,0,0,0,700,0,0,0,0,3,2,1,34,'Arial'>
LogFont         <-45,0,2700,2700,400,0,0,0,0,7,0,0,18,'Times New Roman'>
LogFont         <-40,0,3600,3600,400,0,0,0,0,7,0,0,18,'Times New Roman'>
LogFont         <-39,0,0,0,400,0,0,0,0,0,0,0,18,'Times New Roman'>
LogFont         <-37,0,3600,3600,700,0,0,0,0,7,0,0,34,'Arial'>
LogFont         <-35,0,0,0,400,0,0,0,0,0,0,0,34,'Arial'>
LogFont         <-33,0,0,0,400,0,0,0,0,0,0,0,18,'Times New Roman'>
LogFont         <-32,0,0,0,700,0,0,0,0,0,0,0,0,'Times New Roman'>
LogFont         <-31,0,0,0,400,0,0,0,0,3,2,1,34,'Arial'>
LogFont         <-30,0,0,0,400,0,0,0,0,0,0,0,34,'Arial'>
LogFont         <-29,0,0,0,400,0,0,0,0,0,0,0,18,'Times New Roman'>
LogFont         <-28,0,0,0,400,0,0,0,0,3,2,1,34,'Arial'>
LogFont         <-27,0,0,0,700,0,0,0,0,0,0,2,34,'Arial'>
LogFont         <-26,0,0,0,400,0,0,0,2,0,0,0,2,'Marlett'>
LogFont         <-25,0,0,0,400,0,0,0,0,0,0,0,18,'Times New Roman'>
LogFont         <-24,12,0,0,700,0,0,0,0,0,0,2,0,'Helvetica'>
LogFont         <-23,0,0,0,700,0,0,0,0,4,0,0,34,'Arial'>
LogFont         <-22,0,0,0,700,0,0,0,0,4,0,0,34,'Arial'>
LogFont         <-21,0,0,0,400,0,0,0,0,0,0,0,18,'Times New Roman'>
LogFont         <-20,0,0,0,400,0,0,0,0,0,0,2,34,'Arial'>
LogFont         <-19,0,0,0,700,0,0,0,1,4,0,2,0,'verdana'>
LogFont         <-18,0,0,0,700,0,0,0,0,0,0,2,0,'Arial'>
LogFont         <-17,0,0,0,700,0,0,0,0,0,0,2,34,'Arial'>
LogFont         <-16,0,0,0,400,0,0,0,0,0,0,0,0,'Arial'>
LogFont         <-15,9,0,0,700,0,0,0,0,0,2,2,2,'MS Sans Serif'>
LogFont         <-14,0,0,0,400,0,0,0,0,0,0,0,18,'Times New Roman'>
LogFont         <-13,0,0,0,700,1,0,0,0,4,0,2,0,'arial'>
LogFont         <-12,0,0,0,400,0,0,0,0,5,0,1,0,'Arial'>
LogFont         <-11,0,0,0,700,0,0,0,0,0,0,0,0,'MS Sans Serif'>
LogFont         <-10,0,0,0,400,0,0,0,0,0,0,0,0,'Times New Roman'>
LogFont         <-9,0,0,0,0,0,0,0,0,4,0,2,34,'Small Fonts'>
LogFont         <-8,0,0,0,400,0,0,0,0,0,0,0,0,'MS Sans Serif'>
LogFont         <-7,0,0,0,400,0,0,0,0,3,2,1,34,'Small Fonts'>
LogFont         <-6,0,0,0,400,0,0,0,0,0,0,0,34,'MS Sans Serif'>
LogFont         <-5,0,0,0,400,0,0,0,0,0,0,0,0,'MS Sans Serif'>
LogFont         <-4,0,0,0,700,0,0,0,0,0,0,1,18,'Times New Roman'>
LogFont         <-3,0,900,900,400,0,0,0,0,7,0,0,18,'Times New Roman'>
LogFont         <-2,0,0,0,700,0,0,0,0,0,0,0,32,'\0'>
LogFont         <-1,0,0,0,400,0,0,0,0,4,0,0,18,'Times New Roman'>
LogFont         <0,10,0,0,1000,0,0,0,0,0,0,0,0,'System'>
LogFont         <1,0,900,900,400,0,0,0,0,7,0,0,18,'Times New Roman'>
LogFont         <2,0,0,0,400,0,0,0,2,0,0,0,0,'MARLETT'>
LogFont         <3,1,0,0,700,0,0,0,0,4,2,2,34,'System'>
LogFont         <4,0,0,0,400,0,0,0,2,0,0,0,0,'MARLETT'>
LogFont         <5,0,3600,3600,400,0,0,0,0,7,0,0,18,'Times New Roman'>
LogFont         <6,0,0,0,400,0,0,0,0,0,0,0,0,'MS Sans Serif'>
LogFont         <7,0,0,0,400,0,0,0,2,0,0,0,0,'MARLETT'>
LogFont         <8,0,3600,3600,400,1,0,0,0,7,0,0,18,'Times New Roman'>
LogFont         <9,0,0,0,400,0,0,0,2,0,0,0,0,'MARLETT'>
LogFont         <10,0,0,0,400,0,0,0,2,0,0,0,0,'MARLETT'>
LogFont         <11,0,0,0,700,0,0,0,0,2,0,2,2,'AdobeSm'>
LogFont         <12,0,0,0,400,0,0,0,2,0,0,0,0,'MARLETT'>
LogFont         <13,0,0,0,400,0,0,0,2,0,0,0,0,'MARLETT'>
LogFont         <14,0,3600,3600,400,0,0,0,0,7,0,0,34,'Arial'>
LogFont         <15,0,0,0,400,0,0,0,2,0,0,0,0,'MARLETT'>
LogFont         <16,0,0,0,500,0,0,0,0,4,0,2,0,'Times New Roman'>
LogFont         <17,0,0,0,400,0,0,0,2,0,0,0,0,'MARLETT'>
LogFont         <18,0,0,0,400,0,0,0,2,0,0,0,0,'MARLETT'>
LogFont         <19,0,3600,3600,700,0,0,0,0,7,0,0,34,'Arial'>
LogFont         <21,0,3600,3600,400,1,0,0,0,7,0,0,18,'Times New Roman'>
LogFont         <22,0,3600,3600,700,0,0,0,0,7,0,0,18,'Times New Roman'>
LogFont         <24,0,3600,3600,400,0,0,0,0,7,0,0,34,'Arial'>
LogFont         <25,0,3600,3600,700,0,0,0,0,7,0,0,34,'Arial'>
LogFont         <26,0,3600,3600,400,0,0,0,0,7,0,0,34,'Arial'>
LogFont         <27,0,3600,3600,400,0,0,0,0,7,0,0,18,'Times New Roman'>
LogFont         <29,0,3600,3600,700,0,0,0,0,7,0,0,34,'Arial'>
LogFont         <46,0,3600,3600,700,0,0,0,0,7,0,0,34,'Arial'>
LogFont         <50,0,0,0,800,1,0,0,1,0,0,2,32,'Arial'>
LogFont         <-10000,0,0,0,0,0,0,0,0,0,0,0,0,'\0'>


szAllChars      LABEL   BYTE
                DW        0,  1,  2,  3,  4,  5,  6,  7,  8,  9
                DW       10, 11, 12, 13, 14, 15, 16, 17, 18, 19
                DW       20, 21, 22, 23, 24, 25, 26, 27, 28, 29
                DW       30, 31, 32, 33, 34, 35, 36, 37, 38, 39
                DW       40, 41, 42, 43, 44, 45, 46, 47, 48, 49
                DW       50, 51, 52, 53, 54, 55, 56, 57, 58, 59
                DW       60, 61, 62, 63, 64, 65, 66, 67, 68, 69
                DW       70, 71, 72, 73, 74, 75, 76, 77, 78, 79
                DW       80, 81, 82, 83, 84, 85, 86, 87, 88, 89
                DW       90, 91, 92, 93, 94, 95, 96, 97, 98, 99
                DW      100,101,102,103,104,105,106,107,108,109
                DW      110,111,112,113,114,115,116,117,118,119
                DW      120,121,122,123,124,125,126,127,128,129
                DW      130,131,132,133,134,135,136,137,138,139
                DW      140,141,142,143,144,145,146,147,148,149
                DW      150,151,152,153,154,155,156,157,158,159
                DW      160,161,162,163,164,165,166,167,168,169
                DW      170,171,172,173,174,175,176,177,178,179
                DW      180,181,182,183,184,185,186,187,188,189
                DW      190,191,192,193,194,195,196,197,198,199
                DW      200,201,202,203,204,205,206,207,208,209
                DW      210,211,212,213,214,215,216,217,218,219
                DW      220,221,222,223,224,225,226,227,228,229
                DW      230,231,232,233,234,235,236,237,238,239
                DW      240,241,242,243,244,245,246,247,248,249
                DW      250,251,252,253,254,255
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE
PUBLIC  lpfnUserGetDC, lpfnUserReleaseDC, lpfnGDICreateFontIndirect
PUBLIC  lpfnGDISelectObject, lpfnGDIDeleteObject, lpfnGDIExtTextOut
PUBLIC  dwFontInited
lpfnUserGetDC                   DD      0
lpfnUserReleaseDC               DD      0
lpfnGDICreateFontIndirect       DD      0
lpfnGDISelectObject             DD      0
lpfnGDIDeleteObject             DD      0
lpfnGDIExtTextOut               DD      0
dwFontInited                    DD      0


;==============================================================================
;
; Function:     GENERIC_RealizeObject
;
; Purpose:      This function is the display driver RealizeObject entry point.
;
; Arguments:    RealizeObject Display driver stack FRAME
;
; Returns:      For Object Creation
;               If lpOut is NULL
;                       return the size in bytes of the phys object whose
;                       type is given by wStyle
;               If lpOut is not NULL
;                       for solid brushes to a mono device --   ax = 8001H
;                       for solid brushes to a color device --  ax = 8002H
;                       for bitmaps                             ax = 1
;                       for everything else that is a succes -- ax = 8000H
;                       If unsuccessful                         ax = 0000H
;
;               For Object Deletion
;                       for a success                           ax = 1
;                       for a failure                           ax = 0
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_RealizeObject, PASCAL, NOFRAME, FAR16
OPENPROC
        ; Convert to the DIB_RealizeObjectExt stack FRAME
        pop     eax
        push    dword ptr cs:[CSlp16LogDevice]
        push    eax
        jmp	GN_RealizeObjectExt
CLOSEPROC

;==============================================================================
;
; Function:     GN_RealizeObjectExt
;
; Purpose:      This function is jumped to by the the display driver
;               GENERIC_RealizeObject entry point after the additional
;               parameter (the PDEVICE) is added to the parameter list.
;
; Arguments:    RealizeObjectExt Display Driver Stack Frame
;
; Returns:      If lpOut is NULL
;                       return the size in bytes of the phys object whose
;                       type is given by wStyle
;               If lpOut is not NULL
;                       for solid brushes to a mono device --   ax = 8001H
;                       for solid brushes to a color device --  ax = 8002H
;                       for everything else that is a succes -- ax = 8000H
;                       If unsuccessful                         ax = 0000H
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_RealizeObjectExt, PASCAL, FRAME, FAR16
PARMD   lpDevice
PARMW   wStyle
PARMD   lpIn
PARMD   lpOut
PARMD   lpXForm
PARMD   lpPDevice
OPENPROC
        PUSHR   ds,esi,edi

        ; Dispatch based upon style
        movsx   eax,wStyle
        cmp     eax,2
        je      GN_RealizeObjectExtBrush
        cmp     eax,5
        je      GN_RealizeObjectExtBitmap
        cmp     eax,-5
        je      GN_RealizeObjectExtDeleteBitmap
        cmp     eax,3
        je      GN_RealizeObjectExtFont

        ; Pin: DIBENG doesn't handle DeleteObject anyway
        test    eax, eax
        js      GN_RealizeObjectExtSuccess
        .errnz  $ - OFFSET GN_RealizeObjectExtPunt

GN_RealizeObjectExt     ENDP


;==============================================================================
;
; Function:     RealizeObjectExt ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of RealizeObjectExt. You can either return a success code,
;               a failure code, punt to the DIBENG, etc.
;
; Arguments:    RealizeObjectExt Display Driver Stack Frame
;
;==============================================================================
PUBLIC  GN_RealizeObjectExtPunt
GN_RealizeObjectExtPunt   PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
PLABEL	Punt_RealizeObjectExt
        jmp     DIB_RealizeObjectExt
GN_RealizeObjectExtPunt   ENDP

; NOTE: This routine is not a place to jump for everything. Some of the
; RealizeObjectExt flavors must return specific success codes -- not
; just a 1. See the main header in this file for more info.
PUBLIC  GN_RealizeObjectExtSuccess
GN_RealizeObjectExtSuccess   PROC         NEAR
        mov     eax,1
        .errnz  $ - OFFSET GN_RealizeObjectExtDone
GN_RealizeObjectExtSuccess   ENDP

PUBLIC  GN_RealizeObjectExtDone
GN_RealizeObjectExtDone   PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 16H
GN_RealizeObjectExtDone   ENDP


;==============================================================================
;
; Function:     GN_RealizeObjectExtBrush
;
; Purpose:      This function gets jumped to from RealizeObjectExt when
;               the object is known to be a brush. We need to do two
;               things in here.
;               1) If the brush is a color pattern and it is located in
;                  Video Memory, then we must wait for the graphics engine
;                  to go idle since the bitmap may not yet be drawn.
;               2) We normally let the DIBENG realize brushes. However,
;                  in 16bpp mode, the DIBENG will dither solid brushes
;                  that are not an exact color match. We prevent this by
;                  realizing solid brushes ourselves in 16bpp mode.
;
; Arguments:    RealizeObjectExt Display Driver Stack Frame
;
; Returns:      jump to one of the RealizeObjectExt ending routines
;               when you are done
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  GN_RealizeObjectExtBrush
GN_RealizeObjectExtBrush  PROC    NEAR

        ; We will be needing these
        sub     edi,edi
        les     di,lpDevice
        sub     esi,esi
        lds     si,lpIn

	; Check for ICM
	xor	eax,eax
	cmp     ds:[esi].LogBrush.lbhcmXform,eax
	jne     GN_RealizeObjectExtPunt

        ; Check for a solid brush
        cmp     ds:[esi].LogBrush.lbStyle,BS_SOLID
        je      SolidWorry

NonSolidWorry:
        ; If it is only a sizing call, then pass it on
        cmp     lpOut,0
        je      GN_RealizeObjectExtPunt

        xor     eax,eax
        cmp     cs:[dwFontInited], eax
        jle     @F
        call    GN_LoadAndInitFonts
@@:
        ; Check for color pattern brushes
        cmp     ds:[esi].LogBrush.lbStyle,BS_PATTERN
        jne     GN_RealizeObjectExtPunt

        ; Punt NULL bitmap ptrs
        sub     eax,eax
        cmp     eax,ds:[esi].LogBrush.lbColor
        je      GN_RealizeObjectExtPunt

        ; Punt any bitmaps which are definitely not in VRAM
        lds     si,ds:[esi].LogBrush.lbColor
        cmp     ax,ds:[esi].DIBENGINE.deType
        je      GN_RealizeObjectExtPunt
        test    ds:[esi].DIBENGINE.deFlags,VRAM OR OFFSCREEN
        jz      GN_RealizeObjectExtPunt

        ; Must wait for idle
        xor     edi,edi                 ; clear high word of EDI
        les     di,cs:[CSlp16LogDevice]
        push    cs:[CSlp16LogDevice]
        push    eax
        push    eax
        pushw   FB_ACCESS
        call    FFP16 PTR es:[edi].DIBENGINE.deBeginAccess
        jmp     GN_RealizeObjectExtPunt

SolidWorry:
IF  (DO_NOT_DITHER_16BPP_SOLID_BRUSHES NE 0)
        ; Check for 16bpp
        cmp     es:[edi].DIBENGINE.deBitsPixel,10H
        jne     GN_RealizeObjectExtPunt

        ; Handle the sizing call, also
        mov     ecx,lpOut
        mov     eax,52H
        or      ecx,ecx
        je      GN_RealizeObjectExtDone

        ; Realize a solid brush without dithering
        movzx   edx,es:[edi].DIBENGINE.deFlags
        xor     edi,edi                 ; clear high word of EDI
        les     di,lpOut
        movzx   eax,ds:[esi].LogBrush.lbStyle
        mov     ebx,ds:[esi].LogBrush.lbColor
        mov     es:[edi].DIB_Brush16.dp16BrushStyle,ax
        mov     dword ptr es:[edi].DIB_Brush16.dp16FgColor,ebx
        movzx   eax,ds:[esi].LogBrush.lbHatch
        mov     ecx,dword ptr ds:[esi].LogBrush.lbBkColor
        mov     word ptr es:[edi].DIB_Brush16.dp16Hatch,ax
        mov     dword ptr es:[edi].DIB_Brush16.dp16BgColor,ecx

        ; To be comatible with the DIBENG, we must place
        ; 1) the RGB color in the dp16BrushMono field
        ; 2) the lpXForm argument in the dp16BrushMono+4 field
        ; 3) Use 14H for the BPP for 5-6-5 and 10H for BPP for 5-5-5
        mov     eax,lpXForm
        mov     dword ptr es:[edi].DIB_Brush16.dp16BrushMono,ebx
        mov     dword ptr es:[edi + 4].DIB_Brush16.dp16BrushMono,eax
        mov     ecx,0403H
        mov     eax,01000h+COLORSOLID
        test    edx,FIVE6FIVE
        jz      @F
        mov     ecx,0202H
        mov     eax,01400h+COLORSOLID
@@:     mov     dword ptr es:[edi].DIB_Brush16.dp16BrushFlags,eax

        ; This code is functionally equivalent to the DIBENG
        ; Convert 24bpp RGB (8-8-8) to 16bpp (5-6-6) or 15bpp (5-5-5)
        sub     edx,edx
        mov     dl,bl
        sub     dl,04
        cmc
        sbb     al,al
        and     dl,al
        shl     dx,05

        mov     dl,bh
        sub     dl,ch
        cmc
        sbb     al,al
        and     dl,al
        shr     dx,cl

        shr     ebx,10H
        sub     bl,04
        cmc
        sbb     al,al
        and     bl,al

        shl     ebx,18H
        shld    edx,ebx,05

        mov     bx,dx
        shl     edx,16
        mov     dx,bx
        mov     dword ptr es:[edi].DIB_Brush16.dp16BrushBits,edx
        mov     eax,8002h
        jmp     GN_RealizeObjectExtDone
ELSE
        jmp     GN_RealizeObjectExtPunt
ENDIF

GN_RealizeObjectExtBrush  ENDP


;==============================================================================
;
; Function:     GN_RealizeObjectExtBitmap
;
; Purpose:      This function gets jumped to from RealizeObjectExt when
;               the object is known to be a bitmap. We need to either
;               size the bitmap or create a bitmap header.
;
; Arguments:    RealizeObjectExt Display Driver Stack Frame
;
; Returns:      If sizing call (lpOut is NULL)
;                       dx:ax   number of bytes for header + bits
;               else
;                       fill in bitmap header pointed to by lpOut
;                       ax = 1
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  GN_RealizeObjectExtBitmap
GN_RealizeObjectExtBitmap PROC    NEAR

        ; Load all these up.
        mov     ecx,lpOut
        sub     edi,edi
        les     di,lpIn

        ; Split into a sizing or a creation call
        or      ecx,ecx
        jne     create

        movzx   eax,es:[edi].DIBENGINE.deWidthBytes
        movzx   edx,es:[edi].DIBENGINE.deHeight
        add     eax,3
        and     eax,NOT 3
        imul    eax,edx
        add     eax,SIZE DEVBMP
        shld    edx,eax,10H
        jmp     GN_RealizeObjectExtDone

create:
        ; Build the generic portions of the DEVBMP header
        mov	ds,cs:[wFlatDataSel]
        mov	esi,cs:[CSpGNLogicalDevice]
        mov     eax,dword ptr es:[edi].DIBENGINE.deWidth
        mov     edx,dword ptr es:[edi].DIBENGINE.deWidthBytes
        les     di,lpOut
        mov     es:[edi].DIBENGINE.deType,5250H
        mov     dword ptr es:[edi].DIBENGINE.deWidth,eax
        mov     dword ptr es:[edi].DIBENGINE.deWidthBytes,edx
        mov     eax,cs:[CSlp16LogDevice]
        mov     cx,ds:[esi].DIBENGINE.deVersion
        mov     es:[edi].DIBENGINE.delpPDevice,eax
        mov     es:[edi].DIBENGINE.deVersion,cx
        mov     ax,ds:[esi].DIBENGINEHDR.deBitsSelector
        mov     ecx,ds:[esi].DIBENGINE.deBitmapInfo
        mov     es:[edi].DIBENGINEHDR.deBitsSelector,ax
        mov     es:[edi].DIBENGINE.deBitmapInfo,ecx
        mov     eax,ds:[esi].DIBENGINE.deBeginAccess
        mov     ecx,ds:[esi].DIBENGINE.deEndAccess
        mov     es:[edi].DIBENGINE.deBeginAccess,eax
        mov     es:[edi].DIBENGINE.deEndAccess,ecx

        ; Have to get the flags right.
        movzx   eax,es:[edi].DIBENGINE.deBitsPixel
        mov     es:[edi].DIBENGINEHDR.bRealBpp,al
        mov     ebx,MINIDRIVER OR PALETTIZED
        cmp     eax,10H
        jc      @F
        mov     ebx,MINIDRIVER
        ja      @F
        or      ebx,ds:[esi].GNLOGDEVCX.dw16BppFormat
        test    ebx,FIVE6FIVE
        jnz     @F
        mov     es:[edi].DIBENGINEHDR.bRealBpp,0FH
@@:     mov     es:[edi].DIBENGINEHDR.deFlags,bx

        ; Decide whether to put the bitmap in video memory.
        ; Block if hardware is busy, a DOS box is open, or the
        ; bitmap is to small to make it worthwhile.
        mov	eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        test    ds:[eax].GNBOARDCX.dwOffscreenFlags,NO_OFFS_BMPS
        jnz     regbmp
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
        jne     regbmp
        cmp     es:[edi].DIBENGINEHDR.deHeight,1
        je	regbmp
        cmp     es:[edi].DIBENGINEHDR.deWidth,10H
        jg	@F
        cmp     es:[edi].DIBENGINEHDR.deHeight,10H
        jle	regbmp
        cmp     es:[edi].DIBENGINEHDR.deBitsPixel,08H
        je      @F
        cmp     es:[edi].DIBENGINEHDR.deBitsPixel,10H
        je	@F
        cmp     es:[edi].DIBENGINEHDR.deBitsPixel,20H
        jne	regbmp
@@:
     	push	eax
        call    GN_OffscreenBitmapCreate
        or      eax,eax
        jne     GN_RealizeObjectExtSuccess

regbmp:
        and     es:[edi].DIBENGINE.deFlags,NOT (VRAM OR OFFSCREEN)
        movzx   eax,es:[edi].DIBENGINE.deWidthBytes
        add     eax,3
        and     al,NOT 3
        mov     es:[edi].DIBENGINE.deDeltaScan,eax
        mov     eax,SIZE DEVBMP
        mov     cx,word ptr lpOut+2
        mov     dword ptr es:[edi].DIBENGINE.deBits,eax
        mov      word ptr es:[edi].DIBENGINE.deBits+4,cx
        sub     eax,eax
        mov     es:[edi].DIBENGINE.deBeginAccess,eax
        mov     es:[edi].DIBENGINE.deEndAccess,eax
        jmp     GN_RealizeObjectExtSuccess

GN_RealizeObjectExtBitmap       ENDP


;==============================================================================
;
; Function:     GN_RealizeObjectExtDeleteBitmap
;
; Purpose:      This function gets jumped to from RealizeObjectExt when
;               the action is to delete a bitmap.
;
; Arguments:    RealizeObjectExt Display Driver Stack Frame
;
; Returns:      eax     1
;               edx     0
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  GN_RealizeObjectExtDeleteBitmap
GN_RealizeObjectExtDeleteBitmap PROC    NEAR

        ; If it is only a sizing call, then pass it on. This case shouldn't
        ; happen, but it appears that GDI sometimes passes this case to us.
        cmp     lpOut,0
        je      GN_RealizeObjectExtPunt

	; Load up the DIBENGINEHDR for the bitmap we are deleting
        sub     edi,edi
        les     di,lpOut

        movzx   eax,es:[edi].DIBENGINE.deFlags
        test    eax,VRAM
        jz      GN_RealizeObjectExtSuccess

	; Load up our context
	sub	eax,eax
        mov	ax,cs:[wFlatDataSel]

        ; Is this even a valid selector?
        lar     edx,eax
        jnz     GN_RealizeObjectExtSuccess

        ; The selector must be PRESENT, RING3, DATA
        and     dh,NOT 7
        cmp     dh,0F0H
        jne     GN_RealizeObjectExtSuccess

	mov	ds,ax
        mov	esi,cs:[CSpGNLogicalDevice]

	push	ds:[esi].GNLOGDEVCX.pGNBoardCX
	pushd	0
        call    GN_OffscreenBitmapDelete
        jmp     GN_RealizeObjectExtSuccess

GN_RealizeObjectExtDeleteBitmap ENDP


;==============================================================================
;
; Function:     GN_RealizeObjectExtFont
;
; Purpose:      This function gets jumped to from RealizeObjectExt when
;               the action is to realize a font.
;
; Arguments:    RealizeObjectExt Display Driver Stack Frame
;
; Returns:      Nothing
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  GN_RealizeObjectExtFont
GN_RealizeObjectExtFont         PROC    NEAR

        xor     eax,eax
        cmp     cs:[dwFontInited], eax
        jne     GN_RealizeObjectExtDone
        mov     ds,cs:[wNVTEXT32_Alias_In_NVTEXT32]
        inc     eax
        mov     ds:[dwFontInited], eax
        dec     eax
        jmp     GN_RealizeObjectExtDone

GN_RealizeObjectExtFont         ENDP


;==============================================================================
;
; Function:     GN_InitFunctionPtr
;
; Purpose:      This function returns the function ptr inside the 
;               windows module
;
; Arguments:    eax     ptr to the name of the windows module
;               esi     ORDINAL no of the function
;
; Returns:      eax     16:16 ptr to the requested function
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_InitFunctionPtr, PASCAL, NOFRAME, NEAR
OPENPROC
        push    eax
        call    GetModuleHandle
        and     eax, 0ffffh
        jz      Done
        push    ax
        push    esi
        call    GetProcAddress
        shl     edx, 10H
        mov     dx, ax
        mov     eax, edx
Done:
CLOSEPROC


USER_GETDC_ORDINAL              EQU     66
USER_RELEASEDC_ORDINAL          EQU     68
GDI_CREATEFONTINDIRECT_ORDINAL  EQU     57
GDI_SELECTOBJECT_ORDINAL        EQU     45
GDI_DELETEOBJECT_ORDINAL        EQU     69
GDI_EXTTEXTOUT_ORDINAL          EQU     351


;==============================================================================
;
; Function:     GN_LoadAndInitFonts
;
; Purpose:      This function realizes many common fonts at once
;
; Arguments:    none
;
; Returns:      eax
;
; Preserve:     ds,es,esi,edi,esp
;==============================================================================
DECPROC GN_LoadAndInitFonts, PASCAL, FRAME, NEAR
LOCALV  rcClip,RECT
LOCALW  hDC
OPENPROC
        PUSHR   ds,es,esi,edi,esp
        mov     ax,_DATA
        mov     es,ax
        mov     ds,cs:[wNVTEXT32_Alias_In_NVTEXT32]
        mov     eax,0ffffffffh
        mov     ds:[dwFontInited], eax
        mov     eax,_DATA
        shl     eax,10H
        mov     ax,OFFSET szUserModuleName
        push    eax
        mov     esi, USER_GETDC_ORDINAL
        call    GN_InitFunctionPtr
        test    eax,eax
        jz      InitDone
        mov     ds:[lpfnUserGetDC],eax
        pop     eax
        push    eax
        mov     esi, USER_RELEASEDC_ORDINAL
        call    GN_InitFunctionPtr
        test    eax,eax
        jz      InitDone
        mov     ds:[lpfnUserReleaseDC],eax
        pop     eax

        mov     ax,OFFSET szGDIModuleName
        push    eax
        mov     esi, GDI_CREATEFONTINDIRECT_ORDINAL
        call    GN_InitFunctionPtr
        test    eax,eax
        jz      InitDone
        mov     ds:[lpfnGDICreateFontIndirect],eax
        pop     eax
        push    eax
        mov     esi, GDI_SELECTOBJECT_ORDINAL
        call    GN_InitFunctionPtr
        test    eax,eax
        jz      InitDone
        mov     ds:[lpfnGDISelectObject],eax
        pop     eax
        push    eax
        mov     esi, GDI_DELETEOBJECT_ORDINAL
        call    GN_InitFunctionPtr
        test    eax,eax
        jz      InitDone
        mov     ds:[lpfnGDIDeleteObject],eax
        pop     eax
        push    eax
        mov     esi, GDI_EXTTEXTOUT_ORDINAL
        call    GN_InitFunctionPtr
        test    eax,eax
        jz      InitDone
        mov     ds:[lpfnGDIExtTextOut],eax
        xor     eax, eax
        push    ax
        call    FFP16 PTR ds:[lpfnUserGetDC]
        test    ax,ax
        jz      InitDone
        mov     hDC, ax
        xor     eax,eax
        mov     dword ptr rcClip.RECT.left, eax
        mov     dword ptr rcClip.RECT.right, eax
        mov     ax,_DATA
        mov     ds,ax
        mov     si,ax
        shl     esi,10H
        mov     si,OFFSET lfArray
        mov     edi, esi
        mov     di,OFFSET szAllChars

InitNextFont:
        cmp     [si].LogFont.lfHeight,-10000
        je      DoReleaseDC
        push    esi
        call    FFP16 PTR cs:[lpfnGDICreateFontIndirect]
        test    ax, ax
        jz      DoReleaseDC
        push    ax                                  ; for delete object later

        push    hDC
        push    ax
        call    FFP16 PTR cs:[lpfnGDISelectObject]
        push    hDC                                 ; for select oldfont later
        push    ax

        ; ExtTextOut (hDC, X, Y, fuOptions, lpClipRect, lpString, wCount, lpDX)
        push    hDC
        xor     ebx,ebx
        push    ebx
        mov     eax, 4
        push    ax                                  ; fuOptions=ETO_CLIPPED
        mov     ax,ss
        shl     eax,10H
        lea     ax, rcClip
        push    eax                                 ; lpClip
        push    edi                                 ; szAllChars
        mov     eax, 256
        push    ax
        push    ebx                                 ; lpDX=NULL
        call    FFP16 PTR cs:[lpfnGDIExtTextOut]

        call    FFP16 PTR cs:[lpfnGDISelectObject]  ; select oldfont
        call    FFP16 PTR cs:[lpfnGDIDeleteObject]  ; remove new hfont on stack
        add     si,SIZE LogFont
        jmp     InitNextFont


DoReleaseDC:
        xor     eax, eax
        push    ax
        push    hDC
        call    FFP16 PTR cs:[lpfnUserReleaseDC]

InitDone:
        pop     eax
Done:

        POPR    ds,es,esi,edi,esp
CLOSEPROC

CLOSESEG _NVTEXT32

END

