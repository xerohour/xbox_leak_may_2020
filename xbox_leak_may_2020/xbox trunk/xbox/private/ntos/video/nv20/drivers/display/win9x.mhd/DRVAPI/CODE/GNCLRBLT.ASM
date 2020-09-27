;==============================================================================
;
; Copyright (C) 1998, NVidia Coporation
;
; File:         gnclrblt.asm
;
; Purpose:      This file holds the routines to do Bitblt's involving
;               a color source located in system memory. There may
;               also be a pattern involved but maybe not. The code is
;               generic in terms of not touching hardware, but it does
;               make assumptions about hardware capability and to some
;               extent programming order. These should all be fine for
;               Nvidia products.
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
include gnboard.inc
include gncrtc.inc
include gngdi.inc
include hwgdi.inc


OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE
PUBLIC  dwTableBppToXlatFunc
dwTableBppToXlatFunc    LABEL   DWORD
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0                               ; GN_Xfer1To8 - use mono transfer
DD      OFFSET  GN_Xfer4To8WordXlat
DD      OFFSET  GN_Xfer8To8NoXlat
DD      OFFSET  GN_Xfer15To8
DD      OFFSET  GN_Xfer16To8
DD      OFFSET  GN_Xfer24To8
DD      OFFSET  GN_Xfer32To8
DD      0;                              ; GN_Xfer1To15 - use mono transfer
DD      OFFSET  GN_Xfer4To15DwordXlat
DD      OFFSET  GN_Xfer8To15DwordXlat
DD      OFFSET  GN_Xfer15To15
DD      OFFSET  GN_Xfer16To15
DD      OFFSET  GN_Xfer24To15
DD      OFFSET  GN_Xfer32To15
DD      0                               ; GN_Xfer1To16 - use mono transfer
DD      OFFSET  GN_Xfer4To16DwordXlat
DD      OFFSET  GN_Xfer8To16DwordXlat
DD      OFFSET  GN_Xfer15To16
DD      OFFSET  GN_Xfer16To16
DD      OFFSET  GN_Xfer24To16
DD      OFFSET  GN_Xfer32To16
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0
DD      0                               ; GN_Xfer1To32 - use mono transfer
DD      OFFSET  GN_Xfer4To32
DD      OFFSET  GN_Xfer8To32
DD      OFFSET  GN_Xfer15To32
DD      OFFSET  GN_Xfer16To32
DD      OFFSET  GN_Xfer24To32
DD      OFFSET  GN_Xfer32To32

PUBLIC  bTableBppToIndex
bTableBppToIndex        LABEL   BYTE
DB      08H,    00H,    08H,    08H,    01H,    08H,    08H,    08H
DB      02H,    08H,    08H,    08H,    08H,    08H,    08H,    03H
DB      04H,    08H,    08H,    08H,    08H,    08H,    08H,    08H
DB      05H,    08H,    08H,    08H,    08H,    08H,    08H,    08H
DB      06H

;==============================================================================
;
; Function:     GN_BitBltSrcColorDstVram
;
; Purpose:      This function implements BitBlts where the rop involves
;               a src, the src is a color system memory bitmap and the
;               dst is in VRAM. There may be a pattern involved as well
;               but there may not be. The color depth of the src and
;               dst do not have to agree. Hence there is some sort of
;               pixel depth translation going on. The general strategy
;               implemented in this routine is:
;
;               1) Compute all the neccessary data
;
;               2) Call the HW_BeginSrcColorDstVram routine to set up
;                  the chip to expect to receive dwExtX * dwExtY pixels
;                  in the Destination Pixel Depth and place them
;                  in the rectangle whose upper left corner is at
;                  dwDstLeft,dwDstTop. HW_BeginSrcColorDstVram will
;                  also set the rop, base address, set up the pattern, etc.
;
;               3) GN_BitBltSrcColorDstVram calls the routine
;                  HW_BlockSrcColorDstVram to wait for enough space
;                  in the hardware buffer.
;
;               4) GN_BitBltSrcColorDstVram calls an inner loop routine
;                  which reads src pixels, translates them to dst space
;                  and stores them in the hardware. The inner loop routine
;                  can do partial scanlines or an integral number of scans.
;
;               5) Steps 3 and 4 are repeated until the entire bitmap has
;                  been transferred.
;
;               6) HW_EndSrcColorDstVram is called to allow the hardware
;                  specific code to clean up anything it needs to.
;
; Arguments:
;               ds              Context Selector
;               pNotUsed        Don't use this
;               dwDstBitsSel    Selector for the dst bits
;               dwDstBitsOffset Offset to the dst bits
;               dwDstBpp        bits per pixel of the dst bitmap
;               dwDstPitch      address of first pixel on line n+1 minus
;                               address of first pixel on line n.
;                               This number might be negative if the
;                               DIB is upside down.
;               dwDstLeft       left edge of the dst rectangle
;               dwDstTop        top  edge of the dst rectangle
;               dwSrcBitsSel    selector to the src bits
;               dwSrcBitsOffset offset to the start of the source bits
;               dwSrcBpp        Bits Per Pixel in the src image
;                               This can be 4, 8, 15, 16, 24, or 32
;               dwSrcPitch      address of first pixel on line n+1 minus
;                               address of first pixel on line n.
;                               This number might be negative if the
;                               DIB is upside down.
;               dwSrcLeft       left edge of the src rectangle
;               dwSrcTop        top  edge of the src rectangle
;               dwExtX          extent of the src (and dst) rectangle.
;               dwExtY          extent of the src (and dst) rectangle.
;               dwRop3          3-operand rop
;               lpBrush         ptr to a DIB_Brush struct
;               lpDrawMode      This is only needed for the case where
;                               the rop involves a brhs and the brush
;                               is a mono pattern. In this case,
;                               DRAWMODE.TextColor specifies the color
;                               to which to translate pattern 0 bits, and
;                               DRAWMODE.bkColor specifies the color to
;                               which to translate pattern 1 bits. Note
;                               that these are not the same colors used
;                               translate 0 and 1 bits from the src
;                               bitmap.
;               dwSizeColorTableEntry  bytes per entry in the color table
;               dwColorTableEntries  number of entries in the color table
;               lpColorTable    ptr to a color table. This ptr will be
;                               NULL if no color table is necessary.
;               pfnCopyXlatArg  If this argument is not NULL then it is
;                               the correct inner loop to use. If it is
;                               NULL, then the correct inner loop needs
;                               to be computed from the pixel depths of
;                               the src and dst, and the color table.
;
;       There will be a color table if:
;
;       1) The src is 4bpp. If the dst is 8bpp, then each entry will be
;          2 bytes. Only the low byte is used and is the remapped dst
;          color. If the dst is any other color depth besides 8bpp, then
;          each entry is 4 bytes -- a 32bpp logical color. These entries
;          will need to be mapped to the correct dst color depth.
;
;       2) the src is 8bpp and the dst is not 8bpp. Each entry is is
;          4 bytes -- a 32bpp logical color. These entries will need
;          to be mapped to the correct dst color depth.
;
;       There may be a color table if the src is 8bpp and the dst is
;       8bpp, but there also may not. If there is a color table then
;       each entry is 2 bytes in length. Only the low byte is used and
;       is the remapped dst color.
;
;       In all other cases, there is no color table. In some cases you
;       don't need one. i.e. if you are going from 15bpp->15bpp no
;       remapping is necessary -- just copy the source to the dst. On
;       the other hand if you are going from 24bpp->15bpp, there is no
;       color table either, but you need to convert each 24bit pixel to
;       a 15bit pixel. You MUST do these conversions in a manner identical
;       to the way the DIBENG does the conversions. If you aren't familiar
;       with the various algorithms the DIBENG uses for converting colors,
;       ask someone who is. They are not necessarily what you think. i.e.
;       15->24 does not just expand each 5bit color to an 8bit color by
;       tacking three 0 bits on the low end. There is a pseudo rounding
;       algorithm that is used.
;
; Returns:      eax     0       The routine will be punted to software
;                       else    the return code to GDI
;
;       Do not trash ds,esi,edi
;
;==============================================================================
DECPROC GN_BitBltSrcColorDstVram, PASCAL, NOFRAME, NEAR
;PARMD   pNotUsed
;PARMD   dwDstBitsSel
;PARMD   dwDstBitsOffset
;PARMD   dwDstBpp
;PARMD   dwDstPitch
;PARMD   dwDstLeft
;PARMD   dwDstTop
;PARMD   dwSrcBitsSel
;PARMD   dwSrcBitsOffset
;PARMD   dwSrcBpp
;PARMD   dwSrcPitch
;PARMD   dwSrcLeft
;PARMD   dwSrcTop
;PARMD   dwExtX
;PARMD   dwExtY
;PARMD   dwRop3
;PARMD   lpBrush
;PARMD   lpDrawMode
;PARMD   dwSizeColorTableEntry
;PARMD   dwColorTableEntries
;PARMD   lpColorTable
;PARMD   pfnCopyXlatArg
OPENPROC
        sub     ebx,ebx
        pop     eax
        push    ebx
        push    ebx
        push    ebx
        push    eax
        jmp	GN_StretchBltSrcColorDstVram
CLOSEPROC

;=============================================================
DECPROC GN_StretchBltSrcColorDstVram, PASCAL, FRAME, NEAR
PARMD   pNotUsed
PARMD   dwDstBitsSel
PARMD   dwDstBitsOffset
PARMD   dwDstBpp
PARMD   dwDstPitch
PARMD   dwDstLeft
PARMD   dwDstTop
PARMD   dwSrcBitsSel
PARMD   dwSrcBitsOffset
PARMD   dwSrcBpp
PARMD   dwSrcPitch
PARMD   dwSrcLeft
PARMD   dwSrcTop
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwRop3
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   dwSizeColorTableEntry
PARMD   dwColorTableEntries
PARMD   lpColorTable
PARMD   pfnCopyXlatArg
PARMD   dwExtDstX
PARMD   dwExtDstY
PARMD   lpClipRect

; These are needed by the generic code
LOCALD  pfnCopyXlat
LOCALD  dwDstBytesPerPixel
LOCALD  dwDstWrapWidth
LOCALD  dwAlignedExtXInPixels
LOCALD  dwAlignedDstExtXInBytes
LOCALD  dwSrcStart
LOCALD  dwSrcWrapWidth
LOCALD  dwMaxTransferSizeInBytes
LOCALD  dwScansPerBlock
LOCALD  dwBytesPerBlock
LOCALD  dwPixelsPerBlock
LOCALD  lpFinalColorTable
LOCALD  pHWBoardCXOffset
LOCALD  dwCXSel
LOCALD  dwScale
LOCALD	pfnHWBlockProc
LOCALS  FinalColorTable,400H
OPENPROC
        PUSHR   ds,esi,edi

        mov     dwCXSel,ds
	mov	eax,dwExtDstX
	or	eax,dwExtDstY
	mov	dwScale,eax

	mov	eax,HW_BlockSrcColorDstVram
	jz	@F
	mov	eax,HW_BlockStretchSrcColorDstVram
@@:
	mov	pfnHWBlockProc,eax
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Determine the inner loop copy/xlat routine

        ; If the caller supplied the correct copy routine, we are done
        mov     eax,pfnCopyXlatArg
        mov     pfnCopyXlat,eax
        or      eax,eax
        jne     DoneCT

        ; Figure out the correct copy/xlat routine from the pixel depth
        ; of the src and the dst.
        mov     ebx,dwDstBpp
        mov     edx,dwSrcBpp
        movzx   ebx,cs:bTableBppToIndex[ebx]
        movzx   edx,cs:bTableBppToIndex[edx]
        cmp     ebx,MAX_BPP_INDICES
        jge     DoneIt
        cmp     edx,MAX_BPP_INDICES
        jge     DoneIt
	imul	ebx,MAX_BPP_INDICES
        add     ebx,edx
        mov     ecx,cs:dwTableBppToXlatFunc[ebx * 4]
        mov     pfnCopyXlat,ecx

        ; The copy/xlat routine selected may not be quite correct. There
        ; are some special cases if a color table is involved.
        mov     eax,lpColorTable
        mov     lpFinalColorTable,eax
        or      eax,eax
        je      DoneCT

        ; If the copy/xlat routine is for 8->8, we need to know the size
        ; of a color table entry in order to get the correct routine.
        cmp     ecx,OFFSET GN_Xfer8To8NoXlat
        jne     @F
        mov     pfnCopyXlat,OFFSET GN_Xfer8To8ByteXlat
        cmp     dwSizeColorTableEntry,1
        je      DoneCT
        mov     pfnCopyXlat,OFFSET GN_Xfer8To8WordXlat
        jmp     DoneCT
@@:
        ; If the xlat is 4->15,16 or 8->15,16 and each color table entry
        ; is 32bits, then we may want to pre-convert the color table
        ; entries so we won't have to do a lookup from 4->32 (or 8->32)
        ; and then convert the 32 bit pixel down to 15 or 16. For a
        ; 4bpp source, we'll just say it is always worth pre-converting
        ; the table since there are at most 16 entries in the color table.
        ; For an 8bpp source, there may be 256 table entries and if we
        ; are only going to be processing a handful of pixels, converting
        ; the whole color table may be a waste. Check appropriately.
        cmp     dwSrcBpp,4
        je      S4
        cmp     dwSrcBpp,8
        jne     DoneCT
        mov     eax,dwExtX
        imul    eax,dwExtY
        cmp     eax,100H
        jc      DoneCT
        mov     ecx,403H
        mov     edx,OFFSET GN_Xfer8To15WordXlat
        cmp     dwDstBpp,15
        je      @F
        cmp     dwDstBpp,16
        jne     DoneCT
        mov     ecx,202H
        mov     edx,OFFSET GN_Xfer8To16WordXlat
        jmp     @F
S4:
        mov     ecx,403H
        mov     edx,OFFSET GN_Xfer4To15WordXlat
        cmp     dwDstBpp,15
        je      @F
        cmp     dwDstBpp,16
        jne     DoneCT
        mov     ecx,202H
        mov     edx,OFFSET GN_Xfer4To16WordXlat
@@:
        mov     pfnCopyXlat,edx
        sub     esi,esi
        lds     si,lpColorTable
        push    ss
        pop     es
        sub     edi,edi
        lea     di,FinalColorTable
        mov     edx,dwColorTableEntries
        mov     word ptr lpFinalColorTable+0,di
        mov     word ptr lpFinalColorTable+2,es
        call    GN_ConvertColorTableFrom32To16
DoneCT:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Compute the byte width of a dword aligned dst scan and also
        ; the number of dst pixels that woud fit in this aligned scan.
        mov     eax,dwExtX
        mov     edx,eax
        mov     ebx,dwDstBpp
        inc     ebx
        shr     ebx,3
        imul    eax,ebx
        mov     ecx,eax
        add     eax,3
        and     eax,NOT 3
        mov     dwAlignedDstExtXInBytes,eax
        sub     eax,ecx
        je      Skip
@@:     inc     edx
        sub     eax,ebx
        jne     @B
Skip:   mov     dwAlignedExtXInPixels,edx

        ; Compute src address and wrap width -- dlightly different for
        ; 4bpp vs. 8bpp and higher.
        mov     eax,dwSrcPitch
        mov     edx,dwSrcTop
        imul    edx,eax
        add     edx,dwSrcBitsOffset
        mov     ecx,dwSrcBpp
        mov     ebx,dwExtX
        cmp     ecx,4
        je      Is4
Is8orM:
        inc     ecx
        shr     ecx,3
        imul    ebx,ecx
        sub     eax,ebx
        imul    ecx,dwSrcLeft
        add     edx,ecx
        jmp     @F
Is4:
        add     eax,eax
        add     edx,edx
        sub     eax,ebx
        add     edx,dwSrcLeft
        jmp     @F
@@:
        mov     dwSrcWrapWidth,eax
        mov     dwSrcStart,edx
DoneW:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Call the hardware specific routine to setup for the transfer
        mov     ds,dwCXSel
        mov     ebx,cs:[CSpGNLogicalDevice]
        mov     ecx,ds:[ebx].GNLOGDEVCX.pGNBoardCX


        mov     eax,ds:[ecx].GNBOARDCX.pHWBoardCX
        mov     pHWBoardCXOffset,eax

        push    ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        push    dwDstBitsOffset
        push    dwDstPitch
        push    dwDstBpp
        push    dwDstLeft
        push    dwDstTop
	cmp	dwScale,0
	jne	StretchCase

        push    dwExtX
        push    dwExtY
        push    dwAlignedExtXInPixels
        push    lpBrush
        push    lpDrawMode
        push    dwRop3
        call    HW_BeginSrcColorDstVram
	jmp	@F

StretchCase:
        push    dwExtDstX
        push    dwExtDstY
        push    dwExtX
        push    dwExtY
        push    dwAlignedExtXInPixels
        push    lpBrush
        push    lpDrawMode
        push    dwRop3
	push	lpClipRect
        call    HW_BeginStretchSrcColorDstVram

@@:
        or      eax,eax
        je      DoneIt
        mov     dwDstWrapWidth,ebx
        mov     dwMaxTransferSizeInBytes,ecx

        ; At this point, the generic and hardware code are linked
        ; until the HW_EndSrcColorDstVram routine is called.
        ; The generic code has agreed not to touch es -- it is the
        ; hardware's selector for whatever purposes it may need.

        ; Now we can begin the inner loop. Ther are two different cases
        ; that can occur and we will handle them separately.
        ; 1) The number of bytes that are to be transferred for a single
        ;    scanline is larger than the most the hardware can accept
        ;    in one shot. Each scanline must be broken into multiple
        ;    pieces. This is most typically the FIFO based model.
        ; 2) At least one scanline, possibly many, can be sent as a
        ;    single transfer. This is the typical DMA scenario.
        push    OFFSET CRet
        cmp     ecx,dwAlignedDstExtXInBytes
        jge     GN_XferWholeColorScans
        jmp     GN_XferPartialColorScans
CRet:
        ; Let the hardware know we are done
        mov     esi,pHWBoardCXOffset
        call    HW_EndSrcColorDstVram
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        mov     eax,1

DoneIt:
        POPR    ds,esi,edi
CLOSEPROC



;==============================================================================
;
; Function:     GN_XferWholeColorScans
;
; Purpose:      This function is called from GN_BitBltSrcColorDstVram
;               when at least one whole scan will fit in the transfer
;               area of the chip. It handles calling the hardware for
;               space and then calling the inner loop to transfer data.
;
; Arguments:    Shares stack FRAME with GN_BitBltSrcColorDstVram
;               es      hardware specific code has set this
;                       Do not change it!
;               edi     Can be incremented appropriately as each
;                       pixel is put in the buffer and the wrapWidth
;                       can be added appropriately. These are the
;                       only changes allowed.
;
; Returns:      None
;
; Preserve:     None
;==============================================================================
PUBLIC  GN_XferWholeColorScans
GN_XferWholeColorScans  PROC    NEAR

        ; Figure out how many scanlines can be done in a single shot
        mov     eax,dwMaxTransferSizeInBytes
        sub     edx,edx
        div     dwAlignedDstExtXInBytes
        mov     dwScansPerBlock,eax
        imul    eax,dwAlignedDstExtXInBytes
        mov     dwBytesPerBlock,eax
        mov     esi,dwSrcStart
        mov     ds,word ptr dwSrcBitsSel
        sub     ebx,ebx
        lfs     bx,lpFinalColorTable

LoopY:
        ; Compute the number of scans in the next chunk
        mov     eax,dwBytesPerBlock
        mov     ecx,dwScansPerBlock
        sub     dwExtY,ecx
        jge     @F
        add     ecx,dwExtY
        mov     dwExtY,0
        mov     eax,dwAlignedDstExtXInBytes
        imul    eax,ecx
@@:
        ; Get ready to call inner loop
        push    dwExtX
        push    ecx
        push    dwSrcWrapWidth
        push    dwDstWrapWidth

        ; Have the hardware wait for enough space to hold one chunk's
        ; worth of bytes. The hardware returns in es:edi a ptr to where
        ; to place the dst pixels.
        push    esi
        mov     esi,pHWBoardCXOffset
        call    NEAR PTR pfnHWBlockProc
        pop     esi

        ; Call the inner loop to process a chunk of scans
        call    NEAR PTR pfnCopyXlat

        ; If there are more scans to do, go for it.
        cmp     dwExtY,0
        jg      LoopY
        ret

GN_XferWholeColorScans  ENDP


;==============================================================================
;
; Function:     GN_XferPartialColorScans
;
; Purpose:      This function is called from GN_BitBltSrcColorDstVram
;               when one whole scan will not fit in the transfer
;               area of the chip. It handles calling the hardware for
;               space and then calling the inner loop to transfer data.
;
; Arguments:    Shares stack FRAME with GN_BitBltSrcColorDstVram
;               es      hardware specific code has set this
;                       Do not change it!
;               edi     Can be incremented appropriately as each
;                       pixel is put in the buffer and the wrapWidth
;                       can be added appropriately. These are the
;                       only changes allowed.
;
; Returns:      None
;
; Preserve:     None
;==============================================================================
PUBLIC  GN_XferPartialColorScans
GN_XferPartialColorScans        PROC    NEAR

        ; We can't send a whole scanline in a single transfer so
        ; compute how many pixels we can send in one shot.
        mov     eax,dwMaxTransferSizeInBytes
        sub     edx,edx
        mov     ebx,dwDstBpp
        inc     ebx
        shr     ebx,3
        mov     dwDstBytesPerPixel,ebx
        div     ebx
        mov     dwPixelsPerBlock,eax
        imul    eax,ebx
        mov     dwBytesPerBlock,eax
        mov     esi,dwSrcStart
        mov     ds,word ptr dwSrcBitsSel
        sub     ebx,ebx
        lfs     bx,lpFinalColorTable

LoopY:
        push    dwExtX
LoopX:
        ; Compute the number of pixels in the next chunk
        mov     eax,dwBytesPerBlock
        mov     ecx,dwPixelsPerBlock
        sub     dwExtX,ecx
        jge     @F
        add     ecx,dwExtX
        mov     dwExtX,0
        mov     eax,dwDstBytesPerPixel
        imul    eax,ecx
        add     eax,3
        and     eax, NOT 3
@@:
        ; Get ready to call inner loop
        push    ecx
        pushd   0                               ; Only partial scanlines
        push    dwSrcWrapWidth
        push    dwDstWrapWidth

        ; Have the hardware wait for enough space to hold one chunk's
        ; worth of bytes. The hardware returns in es:edi a ptr to where
        ; to place the dst pixels.
        push    esi
        mov     esi,pHWBoardCXOffset
        call    NEAR PTR pfnHWBlockProc
        pop     esi

        ; Call the inner loop to process a chunk of scans
        call    NEAR PTR pfnCopyXlat

        ; Are there more pixels left on this scanline?
        cmp     dwExtX,0
        jne     LoopX
        pop     dwExtX

        ; Go to next scanline
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jg      LoopY
        ret

GN_XferPartialColorScans        ENDP


;==============================================================================
;
; Function:     GN_ConvertColorTableFrom32To16
;
; Purpose:      This function is called to convert a color table from
;               one with 32bpp color entries to one with 15bpp or 16bpp
;               color entries. The conversion code must be functionally
;               equivalent to the DIBENG.
;
; Arguments:
;               ds:esi  ptr to src color table with 32bpp entries
;               edx     number of entries in src color table
;               es:edi  ptr to dst color table with 15bpp entries
;               ecx     403H if should convert to 15bpp (5-5-5)
;                       202H if should convert to 16bpp (5-6-5)
;
; Returns:      The table has been converted.
;
; Preserve:     None
;==============================================================================
DECPROC GN_ConvertColorTableFrom32To16, PASCAL, NOFRAME, NEAR
OPENPROC
@@:
        mov     eax,ds:[esi]
        add     esi,4

        shld    ebx,eax,10H
        sub     bl,04
        cmc
        sbb     bh,bh
        and     bl,bh
        shl     bx,05

        mov     bl,ah
        sub     bl,ch
        cmc
        sbb     ah,ah
        and     bl,ah
        shr     bx,cl

        sub     al,04
        cmc
        sbb     ah,ah
        and     al,ah

        shl     eax,18H
        shld    ebx,eax,05

        mov     es:[edi],bx
        add     edi,2
        dec     edx
        jne     @B
CLOSEPROC

CLOSESEG _NVTEXT32

END

