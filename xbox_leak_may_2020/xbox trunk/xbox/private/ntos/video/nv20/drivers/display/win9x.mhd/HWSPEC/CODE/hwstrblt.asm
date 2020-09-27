;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwclrblt.asm
;
; Purpose:      This file stretches color bitmaps or DIBs from system
;               memory to video memory. The src color Bitmap can be
;               4, 8, 15, 16, 24, 32 bpp, and, the dst video memory
;               bitmap can be 8, 15, 16, or 32 bpp. There may be
;               a rop and a pattern as well.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc
include hwgdi.inc
include hwmacs.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     HW_BeginStretchSrcColorDstVram
;
; Purpose:      This function is an easy way to implement color
;               bitmap stretch. This routine gets called to do setup,
;               and then the HW_BlockStretchSrcColorDstVram gets called before
;               each chunk of lines is transferred, and finally the
;               function HW_EndStretchSrcColorDstVram is called when the main
;               routine is done. You need to do the following:
;
;               2) Set the Rop (which is the byte at dwRop3+2)
;
;               3) Set the starting destination coordinates to be:
;                  dwDstTop, dwDstLeft
;
;               4) Set the extents of the transfer which are:
;                  dwExtX, dwExtY
;
;                  Note that the number of pixels sent for each scan
;                  will not necessarily be dwExtX, but will be instead
;                  dwExtXAligned. dwExtXAligned is equal to dwExtX plus
;                  from 0 to 3 extra pixels required to make the byte
;                  length of a transferred scan a multiple of a dword.
;                  dwExtX is simply the dst rectangle width in pixels.
;                  If there are any extra pixels beyond dwExtX, they
;                  should be dropped by the hardware. That may mean
;                  you need to tell the hardware the blt width is
;                  really dwExtXAligned and set a hardware clip rect.
;
;               5) Program the base and pitch of the dst bitmap which are:
;                  dwDstBitsOffset, dwDstPitch
;
;               6) Anything else you need to do to prepare the
;                  hardware for a transfer of the src bitmap.
;
;               7) You must return three important values to the generic
;                  code. One is a pseudo wrap width.  The second is the
;                  maximum number of bytes you can handle in a single
;                  transfer (without waiting or writing new header info,
;                  or whatever the hardware requires.) The third is
;                  your hardware ptr (either MMIO or DMA buffer.)
;
;                  Here is how it works:
;
;                  When you return from this routine, you should have
;                  es set to your hardware selector.
;
;                  The generic code will call HW_BlockStretchSrcColorDstVram
;                  and have it wait for enough fifo slots or DMA buffer
;                  space. HW_BlockStretchSrcColorDstVram will return a ptr to
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
;                  HW_BlockStretchSrcColorDstVram again to both wait for more
;                  space and get a new dst ptr.
;
;                  So what exactly should these return values be?
;
;                  For a fifo based part like NV3/ZX, the pseudo wrap width
;                  should be -dwExtAligned * (dwDstBpp which is simply the
;                  negative of one of the locals on the predefined frame.
;                  The maximum single transfer size should be 128 bytes
;                  (or whatever the size of the fifo is in bytes.)
;
;                  For a DMA based part like NV4, the pseudo wrap width
;                  should be somewhere near 0 -- what does this mean?
;                  What we would like is for the next scanline's data to
;                  immediately follow the preceeding scanline's data.
;                  You might think that this implies the pseudo wrap
;                  width is 0. However, we will need to play with the
;                  dwExtX and dwExtXAligned to see if this is true.
;                  If dwExtX and dwExtXAligned are equal, the pseudo
;                  wrap width should be 0. If dwExtXAligned is greater
;                  (it can't be less since it is equal to dwExtX
;                  rounded up to the next number of pixels which gives
;                  a dword aligned scanline length), then the pseudo
;                  wrap width should be
;                  (dwExtXAligned - dwExtX) * dwDstBytesPerPixel
;
;                  The maximum single transfer size in bytes for DMAs is
;                  the size of the method to which the color pixels
;                  will be written. (Or the size of the DMA buffer if
;                  it is smaller, but for other driver policy reasons,
;                  the DMA buffer must be more than twice the size of
;                  the largest method so go with the method size.)
;
; Arguments:
;               ds                      Context Sel
;               pHWLogdevCXOffset       HWLOGDEVCX
;               dwDstBitsOffset         offset of dst bitmap in VRAM
;               dwDstPitch              pitch of dst bitmap in VRAM
;               dwDstBpp                bpp of dst bitmap (8,15,16,or 32)
;               dwDstLeft               left edge coord of dst rectangle
;               dwDstTop                top edge coord of dst rectangle
;               dwExtDstX               width of dst rectangle in pixels
;               dwExtDstY               height of dst rectangle in scanlines
;               dwExtSrcX               width of src rectangle in pixels
;               dwExtSrcY               height of dst rectangle in scanlines
;               dwExtSrcXAligned        aligned width of src rectangle in pixels
;               lpBrush                 ptr to DIB_Brush
;               lpDrawMode              ptr to DRAWMODE
;               dwRop3                  rop 3 in Microsoft rop format
;
; Returns:
;       eax     0       need to punt
;               else    Keep going -- everything's fine
;                       In this case, you must also return
;                       ebx  - The pseudo wrap width
;                       ecx  - The max single transfer size in bytes
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_BeginStretchSrcColorDstVram, PASCAL, FRAME, NEAR
PARMD   pHWLogdevCXOffset
PARMD   dwDstBitsOffset
PARMD   dwDstPitch
PARMD   dwDstBpp
PARMD   dwDstLeft
PARMD   dwDstTop
PARMD   dwExtDstX
PARMD   dwExtDstY
PARMD   dwExtSrcX
PARMD   dwExtSrcY
PARMD   dwExtSrcXAligned
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   dwRop3
PARMD   lpClipRect
OPENPROC
        mov     edx,pHWLogdevCXOffset
        mov     esi,ds:[edx].HWLOGDEVCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,esi,eax

        ; Load the push buffer ptr
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     eax,dwDstBitsOffset
        CHECKDSTBASE ds,esi,edi,eax,dwDstPitch

        ; Prepare for a src transfer
        CHECKDMASPACE ds,esi,edi,48H
        mov     eax,CHNRUN(ContextPattern.SetMonochromeColor0,2)
        mov     ebx,ds:[edx].HWLOGDEVCX.dwPatternAlpha0
        mov     ecx,ds:[edx].HWLOGDEVCX.dwPatternAlpha1
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx
        mov     ds:[edi + 08H],ecx
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        movzx   ebx,byte ptr dwRop3+2
        mov     ds:[edi + 0CH],eax
        mov     ds:[edi + 10H],ebx
        mov     eax,CHNRUN(StretchedImageFromCpuHdr.object,1)
        mov     ebx,OBJ_STRETCHED_IMAGE_FROM_CPU
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ebx

        mov     eax,CHNRUN(StretchedImageFromCpu.SetColorFormat,1)
        mov     ebx,ds:[edx].HWLOGDEVCX.dwImageFromCpuColorFormat
        mov     ds:[edi + 1CH],eax
        mov     ds:[edi + 20H],ebx
        mov     eax,CHNRUN(StretchedImageFromCpu.dwSizeIn,3)
        mov     ds:[edi + 24H],eax
        mov     ebx,dwExtSrcY
        mov     eax,dwExtSrcXAligned
	shl	ebx,10H
	mov	bx,ax
        mov     ds:[edi + 28H],ebx
	mov	eax,dwExtDstX
	mov	ecx,dwExtSrcX
	shl	eax,20
	sub	edx,edx
	idiv	ecx
        mov     ds:[edi + 2CH],eax
	mov	eax,dwExtDstY
	shl	eax,20
	mov	ecx,dwExtSrcY
	sub	edx,edx
	idiv	ecx
        mov     ds:[edi + 30H],eax
        mov     eax,CHNRUN(StretchedImageFromCpu.ClipPoint,2)
	sub	edx,edx
	mov	ebx,-1
	cmp	lpClipRect,edx
	je	@F
	lgs	dx,lpClipRect
        mov	ebx,dword ptr gs:[edx].RECT.right
        mov	edx,dword ptr gs:[edx].RECT.left
	sub	ebx,edx
@@:
        mov     ds:[edi + 34H],eax
        mov     ds:[edi + 38H],edx
        mov     ds:[edi + 3CH],ebx
        mov     eax,CHNRUN(StretchedImageFromCpu.Point12d4,1)
        mov     ds:[edi + 40H],eax
	mov	eax,dwDstTop
	shl	eax,20
	mov	ebx,dwDstLeft
	shl	ebx,4
	mov	ax,bx
	add	eax,80008H
        mov     ds:[edi + 44H],eax
        add     edi,48H

        ; I need to return a few things
        mov     ebx,dwExtSrcXAligned
        sub     ebx,dwExtSrcX
        mov     ecx,dwDstBpp
        inc     ecx
        shr     ecx,3
        imul    ebx,ecx         ; pseudo wrap width
        mov     ecx,1792 * 4    ; size of color transfer method
        mov     ax,ds
        mov     es,ax
        mov     eax,1
Done:
CLOSEPROC


;==============================================================================
;
; Function:     HW_BlockStretchSrcColorDstVram
;
; Purpose:      This function is an easy way to implement color
;               bitmap transfers. It is one of a sequence. For full
;               details, please see HW_BeginStretchSrcColorDstVram eariler
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
;               sequence, you must fail at HW_BeginStretchSrcColorDstVram time.
;               es:edi must point to the location for the generic code
;               to copy the next eax bytes of dst pixels translated from
;               src pixels.
;
; Preserve:     ds,esi,fs,ebx
;==============================================================================
DECPROC HW_BlockStretchSrcColorDstVram, PASCAL, NOFRAME, NEAR
OPENPROC
        SETPUTPTR es,esi,edi
        shrd    ecx,eax,10H
        lea     edx,[eax + 4]
        mov     cx,CHNRUN(StretchedImageFromCpu.Color,0)
        CHECKDMASPACE es,esi,edi,edx
        mov     es:[edi],ecx
        add     edi,4
CLOSEPROC


;==============================================================================
;
; Function:     HW_EndStretchSrcColorDstVram
;
; Purpose:      This function is an easy way to implement color
;               bitmap transfers. It is one of a sequence. For full
;               details, please see HW_BeginStretchSrcColorDstVram eariler
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
;                       HW_BlockStretchSrcColorDstVram and edi has only
;                       changed by being incremented for each dst
;                       pixel transferred.
;
; Returns:      This routine is not failable. If you wish to fail the
;               sequence, you must fail at HW_BeginStretchSrcColorDstVram time.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_EndStretchSrcColorDstVram, PASCAL, NOFRAME, NEAR
OPENPROC
        SETPUTPTR es,esi,edi
CLOSEPROC

CLOSESEG _NVTEXT32

END

