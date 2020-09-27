;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwmonblt.asm
;
; Purpose:      This file trasnfers mono system memory bitmaps or DIBS
;               to a dst video memory bitmap. The dst bitmap may be 8,
;               15, 16, or 32bpp. There may be a rop and pattern as well.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include hwmacs.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     HW_BeginSrcMonoDstVram
;
; Purpose:      This function is an easy way to implement mono
;               bitmap transfers. This routine gets called to do setup,
;               and then the HW_BlockSrcMonoDstVram gets called before
;               each chunk of lines is transferred, and finally the
;               function HW_EndSrcMonoDstVram is called when the main
;               routine is done. You need to do the following:
;
;               2) Set the Rop (which is the byte at dwRop3+2)
;
;               3) Set the hardware clipping rectangle to the coords
;                  (dwDstLeft, dwDstTop, dwDstLeft+dwExtX, dwDstTop+dwExtY)
;
;               4) Set the starting destination coordinates to be:
;                  dwAlignedDstLeft, dwDstTop
;
;               5) Set the extents of the transfer. The number of pixels
;                  sent in per line will always be a multiple of 32 and
;                  is given by the parameter dwAlignedSrcExtXInPixels.
;                  Of course, the number of pixels to be written to the
;                  dst rectangle is given by the paramter dwExtX. The
;                  number of scanlines sent in is the same as the number
;                  to be displayed and is given by dwExtY.
;
;               6) Program in the colors to which to expand the bits
;                  in the mono src. These colors should NOT be used
;                  to expand mono bits in a mono pattern if a pattern
;                  is involved. The DRAWMODE TextColor and bkColor
;                  are to be used for that. The generic code would
;                  already have called HW_LoadXXXPattrn to load the
;                  pattrn properly if one were involved.
;
;               7) Program the base and pitch of the dst bitmap which are:
;                  dwDstBitsOffset, dwDstPitch
;
;               8) Anything else you need to do to prepare the
;                  hardware for a transfer of the src bitmap.
;
;               9) You must return three important values to the generic
;                  code. One is a pseudo wrap width.  The second is the
;                  maximum number of bytes you can handle in a single
;                  transfer (without waiting or writing new header info,
;                  or whatever the hardware requires.) The third is
;                  your hardware ptr (either MMIO or DMA buffer.)
;
;                  Here is how it works:
;
;                  When you return from this routine, you should have
;                  es:edi pointing to either the next location in the
;                  DMA buffer if you are DMA based, or pointing to the
;                  MMIO area if you are fifo based.
;
;                  The generic code will call HW_BlockSrcMonoDstVram
;                  and have it wait for enough fifo slots or DMA buffer
;                  space. HW_BlockSrcMonoDstVram will return a ptr to
;                  the generic code telling it where to begin writing
;                  pixels. The generic code starts xlatting and copying
;                  pixels incrementing this ptr as it goes. When the
;                  generic code reaches the end of a scanline, it will
;                  add in the pseudo wrap width you returned to the current
;                  value of the ptr and begin transferring pixels on
;                  the next scanline at this new address. If the
;                  generic code reaches the maximum number of pixels
;                  that can be handled in a single transfer (which was
;                  the second parameter you returned) before transferring
;                  the whole bitmap, it will call the block routine
;                  HW_BlockSrcMonoDstVram again to both wait for more
;                  space and get a new dst ptr.
;
;                  So what exactly should these return values be?
;
;                  For a fifo based part like NV3/ZX, the pseudo wrap width
;                  should be -dwAlignedSrcExtXInBytes -- this is the
;                  negative of one of the locals on the predefined frame.
;                  The maximum single transfer size should be 128 bytes
;                  (or whatever the size of the fifo is in bytes.)
;
;                  For a DMA based part like NV4, the pseudo wrap width
;                  should be 0. This will mean that the data for
;                  the next scanline will fall 0 bytes after the end
;                  of the data for the previous scanline -- exactly
;                  what is needed for DMA buffer sceanrios. The
;                  maximum single transfer size in bytes for DMAs is
;                  the size of the method to which the mono pixels
;                  will be written. (Or the size of the DMA buffer if
;                  it is smaller, but for other driver policy reasons,
;                  the DMA buffer must be more than twice the size of
;                  the largest method so go with the method size.)
;
; Arguments:
;               ds                      CXSel
;               pHWLogdevCXOffset       HWLOGDEVCX ptr
;               dwDstBitsOffset         base address of dst bitmap in VRAM
;               dwDstPitch              pitch of dst bitmap in VRAM
;               dwDstBpp                bpp of dst bitmap (be 8,15,16,or 32)
;               dwDstLeft               left edge coord of dst clip rectangle
;               dwDstTop                top edge coord of dst rectangle
;               dwAlignedDstLeft        left edge of unclipped dst rectangle
;               dwExtX                  width of dst rectangle in pixels
;               dwExtY                  height of dst rectangle in scanlines
;               dwAlignedSrcExtXInBytes number of bytes transferred per
;                                       line - will always be a multiple of 4
;               dwAlignedSrcExtXInPixels number of pixels sent per src line
;                                       -- will always be a multiple of 32 so
;                                       the lines have a multiple of 4 bytes
;               dwPixelsIgnoredOnLeft   number of pixels (bits) of the
;                                       src to be ignored. i.e. ignore this
;                                       many pixels, then then next dwExtX
;                                       pixels should be expanded and placed
;                                       in the dst beginning at the dwDstLeft
;                                       coord. There will be some pixels on
;                                       the right to be ignored also. The
;                                       number of these is:
;                                       dwAlignedSrcExtXInPixels - dwExtX -
;                                       dwPixelsIgnoredOnLeft.
;               dwBgColor               expand 0 bits in src to this color
;               dwFgColor               expand 1 bits in src to this color
;               lpBrush                 ptr to DIB_Brush
;               lpDrawMode              ptr to DRAWMODE
;               dwRop3                  rop 3 in Microsoft rop format
;               dwRopFlags              one of NOROP, OND, PAD, SAD, PSD
;                                       See file gngdi.inc for definitions
;                                       and description above for purpose.
;
; Returns:
;       eax     0       need to punt
;               else    Keep going -- everything's fine
;                       In this case, you must also return
;                       ebx  - The pseudo wrap width
;                       ecx  - The max single transfer size in bytes
;                       edi  - push buffer offset
;                       es   - whatever you want (probably CXSel)
;                       See the discussion under purpose for more
;                       info about these return values.
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_BeginSrcMonoDstVram, PASCAL, FRAME, NEAR
PARMD   pHWLogdevCXOffset
PARMD   dwDstBitsOffset
PARMD   dwDstPitch
PARMD   dwDstBpp
PARMD   dwDstLeft
PARMD   dwDstTop
PARMD   dwAlignedDstLeft
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwAlignedSrcExtXInBytes
PARMD   dwAlignedSrcExtXInPixels
PARMD   dwPixelsIgnoredOnLeft
PARMD   dwBgColor
PARMD   dwFgColor
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   dwRop3
OPENPROC
        PUSHR   esi

        mov     edx,pHWLogdevCXOffset
        mov     esi,ds:[edx].HWLOGDEVCX.pHWBoardCX

        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,esi,eax

        ; Load the push buffer ptr
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     eax,dwDstBitsOffset
        CHECKDSTBASE ds,esi,edi,eax,dwDstPitch

        ; Prepare for a src transfer
        CHECKDMASPACE ds,esi,edi,3CH
        mov	eax,CHNRUN(ContextPattern.SetMonochromeColor0,2)
        mov	ebx,ds:[edx].HWLOGDEVCX.dwPatternAlpha0
        mov	ecx,ds:[edx].HWLOGDEVCX.dwPatternAlpha1
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx
        mov     ds:[edi + 08H],ecx
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        movzx   ebx,byte ptr dwRop3+2
        mov     ds:[edi + 0CH],eax
        mov     ds:[edi + 10H],ebx
        mov     eax,CHNRUN(RectAndText.SetColorFormat,1)
        mov     ebx,ds:[edx].HWLOGDEVCX.dwRectAndTextColorFormat
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ebx
        mov     eax,CHNRUN(RectAndText.ClipPoint0E,7)
        mov     ecx,dwExtY
        mov     ebx,dwDstTop
        shl     ecx,10H
        shl     ebx,10H
        mov     bx,word ptr dwDstLeft
        mov     cx,word ptr dwExtX
        add     ecx,ebx
        mov     ds:[edi + 1CH],eax
        mov     ds:[edi + 20H],ebx
        mov     ds:[edi + 24H],ecx
        mov     eax,dwBgColor
        mov     ebx,dwFgColor
        or      eax,ds:[edx].HWLOGDEVCX.dwColorAlphaMask
        or      ebx,ds:[edx].HWLOGDEVCX.dwColorAlphaMask
        mov     ds:[edi + 28H],eax
        mov     ds:[edi + 2CH],ebx
        mov     eax,dwExtY
        mov     ebx,dwDstTop
        shl     eax,10H
        shl     ebx,10H
        mov     ecx,eax
        mov     bx,word ptr dwAlignedDstLeft
        mov     ax,word ptr dwExtX
        mov     cx,word ptr dwAlignedSrcExtXInPixels
        add     ax,word ptr dwPixelsIgnoredOnLeft
        mov     ds:[edi + 30H],ecx
        mov     ds:[edi + 34H],eax
        mov     ds:[edi + 38H],ebx
        add     edi,3CH
        mov     ds:[esi].HWBOARDCX.pCurrentPushBufferOffset,edi

        ; I need to return a few things
        sub     ebx,ebx         ; pseudo wrap width is 0
        mov     ecx,80H * 4     ; size of mono transfer method
        mov     ax,ds
        mov     es,ax
Done:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_BlockSrcMonoDstVram
;
; Purpose:      This function is an easy way to implement mono
;               bitmap transfers. It is one of a sequence. For full
;               details, please see HW_BeginSrcMonoDstVram eariler
;               in this file.
;
;               Here, wait for eax bytes of space in the DMA buffer or
;               in the fifo. You may adjust es:edi to whatever you wish.
;               Hence, if you need to wrap the DMA buffer, you may do
;               do and then reset es:edi to the start of the buffer.
;               The generic code never touches es and only adjusts edi
;               by incrementing it appropriately for every dst pixel
;               that is transferred.
;
; Arguments:
;               es      Context Seletor
;               eax     bytes to wait for
;               esi     pHWBoardCX
;               edi     push buffer ptr
;
; Returns:      This routine is not failable. If you wish to fail the
;               sequence, you must fail at HW_BeginSrcMonoDstVram time.
;               es:edi must point to the location for the generic code
;               to copy the next eax bytes of dst pixels translated from
;               src pixels.
;
; Preserve:     ds
;==============================================================================
DECPROC HW_BlockSrcMonoDstVram, PASCAL, NOFRAME, NEAR
OPENPROC
        SETPUTPTR es,esi,edi
        shrd    ecx,eax,10H
        lea     edx,[eax + 4]
        mov     cx,CHNRUN(RectAndText.MonochromeColor01E,0)
        CHECKDMASPACE es,esi,edi,edx
        mov     es:[edi],ecx
        add     edi,4
CLOSEPROC


;==============================================================================
;
; Function:     HW_EndSrcMonoDstVram
;
; Purpose:      This function is an easy way to implement color
;               bitmap transfers. It is one of a sequence. For full
;               details, please see HW_BeginSrcMonoDstVram eariler
;               in this file.
;
;               You may do anything you wish to here. There is nothing
;               specifically required. Some possibiliites are:
;               1) Bump the DMA ptr if you are DMA based
;               2) Reset the hardware clip rect if that was needed.
;
; Arguments:
;               es      Context Selector
;               esi     pHWBoardCX
;               edi     es has not changed since the last call to
;                       HW_BlockSrcMonoDstVram and edi has only
;                       changed by being incremented for each dst
;                       pixel transferred.
;
; Returns:      This routine is not failable. If you wish to fail the
;               sequence, you must fail at HW_BeginSrcMonoDstVram time.
;
; Preserve:     ds
;==============================================================================
DECPROC HW_EndSrcMonoDstVram, PASCAL, NOFRAME, NEAR
OPENPROC
        SETPUTPTR es,esi,edi
CLOSEPROC

CLOSESEG _NVTEXT32

END

