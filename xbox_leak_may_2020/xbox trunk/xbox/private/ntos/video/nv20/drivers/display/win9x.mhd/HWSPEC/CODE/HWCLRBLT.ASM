;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwclrblt.asm
;
; Purpose:      This file transfers color bitmaps or DIBs from system
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
; Function:     HW_BeginSrcColorDstVram
;
; Purpose:      This function is an easy way to implement color
;               bitmap transfers. This routine gets called to do setup,
;               and then the HW_BlockSrcColorDstVram gets called before
;               each chunk of lines is transferred, and finally the
;               function HW_EndColorSrcDstVram is called when the main
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
;                  The generic code will call HW_BlockSrcColorDstVram
;                  and have it wait for enough fifo slots or DMA buffer
;                  space. HW_BlockSrcColorDstVram will return a ptr to
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
;                  HW_BlockSrcColorDstVram again to both wait for more
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
;               dwExtX                  width of dst rectangle in pixels
;               dwExtY                  height of dst rectangle in scanlines
;               dwExtXAligned           See description above for definition
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
DECPROC HW_BeginSrcColorDstVram, PASCAL, FRAME, NEAR
PARMD   pHWLogdevCXOffset
PARMD   dwDstBitsOffset
PARMD   dwDstPitch
PARMD   dwDstBpp
PARMD   dwDstLeft
PARMD   dwDstTop
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwExtXAligned
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   dwRop3
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
        CHECKDMASPACE ds,esi,edi,30H
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
        mov     eax,CHNRUN(ImageFromCpuHdr.object,1)
        mov     ebx,OBJ_IMAGE_FROM_CPU
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ebx
        mov     eax,CHNRUN(ImageFromCpu.SetColorFormat,4)
        mov     ebx,ds:[edx].HWLOGDEVCX.dwImageFromCpuColorFormat
        mov     ds:[edi + 1CH],eax
        mov     ds:[edi + 20H],ebx
        mov     ebx,dwDstTop
        mov     ecx,dwExtY
        shl     ebx,10H
        shl     ecx,10H
        mov     edx,ecx
        mov     bx,word ptr dwDstLeft
        mov     cx,word ptr dwExtX
        mov     dx,word ptr dwExtXAligned
        mov     ds:[edi + 24H],ebx
        mov     ds:[edi + 28H],ecx
        mov     ds:[edi + 2CH],edx
        add     edi,30H

        ; I need to return a few things
        mov     ebx,dwExtXAligned
        sub     ebx,dwExtX
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
; Function:     HW_BlockSrcColorDstVram
;
; Purpose:      This function is an easy way to implement color
;               bitmap transfers. It is one of a sequence. For full
;               details, please see HW_BeginSrcColorDstVram eariler
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
;               sequence, you must fail at HW_BeginSrcColorDstVram time.
;               es:edi must point to the location for the generic code
;               to copy the next eax bytes of dst pixels translated from
;               src pixels.
;
; Preserve:     ds,esi,fs,ebx
;==============================================================================
DECPROC HW_BlockSrcColorDstVram, PASCAL, NOFRAME, NEAR
OPENPROC
        SETPUTPTR es,esi,edi
        shrd    ecx,eax,10H
        lea     edx,[eax + 4]
        mov     cx,CHNRUN(ImageFromCpu.Color,0)
        CHECKDMASPACE es,esi,edi,edx
        mov     es:[edi],ecx
        add     edi,4
CLOSEPROC


;==============================================================================
;
; Function:     HW_EndSrcColorDstVram
;
; Purpose:      This function is an easy way to implement color
;               bitmap transfers. It is one of a sequence. For full
;               details, please see HW_BeginSrcColorDstVram eariler
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
;                       HW_BlockSrcColorDstVram and edi has only
;                       changed by being incremented for each dst
;                       pixel transferred.
;
; Returns:      This routine is not failable. If you wish to fail the
;               sequence, you must fail at HW_BeginSrcColorDstVram time.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_EndSrcColorDstVram, PASCAL, NOFRAME, NEAR
OPENPROC
        SETPUTPTR es,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_BitBltSrcColorDstVram
;
; Purpose:      This function implements BitBlts where the rop involves
;               a src, the src is a color system memory bitmap and the
;               dst is in VRAM. There may be a pattern involved as well
;               but there may not be.
;
;               Unless the blt is from one pixel depth to another and
;               you have hardware that handles the pixel translation
;               itself, you may just wish to call the generic routine
;               GN_BitBltSrcColorDstVram which will handle 95% of the
;               work of this routine for you. If you use the
;               GN_BitBltSrcColorDstVram service, you must implement
;               the three functions: HW_BeginSrcColorDstVram,
;               HW_BlockSrcColorDstVram and HW_EndSrcColorDstVram.
;
; Arguments:
;               ds                      Context Selector
;               pHWLogdevCXOffset       HWLOGDEVCX
;               lpDstDev                Ptr to dst device (DIBENG header)
;               dwDstLeft               left edge of the dst rectangle
;               dwDstTop                top  edge of the dst rectangle
;               dwSrcBitsSel            selector to the src bits
;               dwSrcBitsOffset         offset to start of the source bits
;               dwSrcBpp                Bits Per Pixel in the src image
;                                       This can be 4, 8, 15, 16, 24, or 32
;               dwSrcPitch              address first pixel on line n+1 minus
;                                       address of first pixel on line n.
;                                       This number might be negative if the
;                                       DIB is upside down.
;               dwSrcLeft               left edge of the src rectangle
;               dwSrcLeft               top  edge of the src rectangle
;               dwExtX                  extent of src (and dst) rectangle.
;               dwExtY                  extent of the (and dst) rectangle.
;               dwRop3                  3-operand rop
;               lpBrush                 ptr to a DIB_Brush struct
;               lpDrawMode              This is only needed for case where
;                                       the rop involves a a bursh which
;                                       is a mono pattern. In this case,
;                                       DRAWMODE.TextColor gives the color
;                                       to which to translate src 0 bits,
;                                       DRAWMODE.bkColor gives the color
;                                       to which to translate src 1 bits.
;                                       Note that these are not the same
;                                       colors used translate 0 and 1 bits
;                                       from the src bitmap.
;               dwSizeColorTableEntry   bytes per entry in the color table
;               dwColorTableEntries     number of entries in the color table
;               lpColorTable            ptr to a color table. This ptr will
;                                       be NULL if no color table is
;                                       necessary.
;               pfnCopyXlatArg          If this argument is not NULL then
;                                       it is the correct inner loop to use.
;                                       If it is NULL, then the correct
;                                       inner loop needs to be computed
;                                       from the pixel depths of the src
;                                       and dst, and the color table.
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
; Returns:      eax     0       Can't handle the call -- please punt
;                       non-0   return code
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_BitBltSrcColorDstVram, PASCAL, FRAME, NEAR
PARMD   pHWLogdevCXOffset
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
LOCALD  pHWBoardCXOffset
LOCALD  dwColorIndexFormat
LOCALD  dwNotAlignMask
LOCALD  dwAlignedSrcLeft
LOCALD  dwAlignedDstLeft
LOCALD  dwAlignMask
LOCALD  dwAlignedExtXPixels
LOCALD  dwSrcPixelsToSrcBytesShiftCount
LOCALD  dwAlignedExtXInDwords
LOCALD  dwLUTEntriesShiftCount
LOCALD  dwSrcWrapWidth
LOCALD  dwBlockBytes
LOCALD  dwBlockHeader
LOCALD  dwWholeScansPerBlock
OPENPROC

        ; Filter out NV5 devices before the first beginaccess call
        ; because of hardware bug
        cmp     dwDstBpp,32
        jne     @F
        mov     eax,pHWLogdevCXOffset
        mov     eax,ds:[eax].HWLOGDEVCX.pHWBoardCX
        cmp     ds:[eax].HWBOARDCX.dwGetOffsetAtLastBeginAccess,-1
        jne     @F
        cmp     ds:[eax].HWBOARDCX.dwDeviceVersion, NV_DEVICE_VERSION_5
        je      Nope

@@:
        ; Filter out the 4->N and 8->N conversions to use special
        ; pixel translation hardware. Since we have to transfer
        ; the color table, however, only do so for 8bit if there
        ; are at least as many pixels to be translated as there
        ; are color table entries to be transferred.
        cmp     dwDstBpp,8
        jbe     Nope
        cmp     dwSrcBpp,4
        je      Yup
        cmp     dwSrcBpp,8
        jne     Nope
        mov     eax,dwExtX
        imul    eax,dwExtY
        cmp     eax,dwColorTableEntries
        ja      Yup
Nope:   STOPPROC
        jmp     GN_BitBltSrcColorDstVram

Yup:
        PUSHR   ds,esi,edi

        ; Compute a few things which depend upon some arguments
        mov     dwLUTEntriesShiftCount,0
        cmp     dwDstBpp,32
        je      @F
        mov     dwLUTEntriesShiftCount,1
@@:
        mov     eax,03H
        sub     ecx,ecx
        mov     dwColorIndexFormat,NV060_INDEX_FORMAT_LE_I8
        cmp     dwSrcBpp,8
        je      @F
        mov     eax,07H
        mov     ecx,1
        mov     dwColorIndexFormat,NV060_INDEX_FORMAT_SVGA65_I4
@@:     mov     dwNotAlignMask,eax
        not     eax
        mov     dwSrcPixelsToSrcBytesShiftCount,ecx
        mov     dwAlignMask,eax

        ; Align the src ptr on a dword boundary. Back up the dst
        ; as needed to match the alignment and extend the ExtX by
        ; the amount we backed up. Then pad the ExtX onthe right
        ; to make it a multiple of a dword.
        mov     eax,dwSrcLeft
        mov     ebx,dwDstLeft
        mov     ecx,eax
        mov     edx,dwExtX
        and     eax,dwNotAlignMask
        sub     ecx,eax
        sub     ebx,eax
        add     edx,eax
        mov     dwAlignedSrcLeft,ecx
        mov     dwAlignedDstLeft,ebx
        add     edx,dwNotAlignMask
        and     edx,dwAlignMask
        mov     dwAlignedExtXPixels,edx

        ; We will not do the transfer if we are forced to break up a
        ; src scan into more than one block (greater than 1792 dwords)
        ; as determined by the hardware method size.
        mov     ecx,dwSrcPixelsToSrcBytesShiftCount
        shr     edx,cl
        cmp     edx,1792 * 4
        ja      GN_BitBltSrcColorDstVram
        shr     edx,2
        mov     dwAlignedExtXInDwords,edx

        ; Load the DMA ptr
        mov     esi,pHWLogdevCXOffset
        push    ds:[esi].HWLOGDEVCX.dwPatternAlpha1
        push    ds:[esi].HWLOGDEVCX.dwPatternAlpha0
        mov     esi,ds:[esi].HWLOGDEVCX.pHWBoardCX
        mov     pHWBoardCXOffset,esi

        ; Wait for other contexts
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,esi,eax

        ; Load the push buffer ptr
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     eax,ds:[esi].HWBOARDCX.dwIndexedLUTOffset
        CHECKDSTBASE ds,esi,edi,eax,<400H>

        mov     edx,dwColorTableEntries
        lea     edx,[edx * 4 + 34H]
        CHECKDMASPACE ds,esi,edi,edx
        mov     dword ptr ds:[edi + 00H],CHNRUN(ContextPattern.SetMonochromeColor0,2)
        pop     dword ptr ds:[edi + 04H]
        pop     dword ptr ds:[edi + 08H]
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        mov     ecx,0CCH
        mov     ds:[edi + 0CH],eax
        mov     ds:[edi + 10H],ecx
        mov     eax,CHNRUN(ImageFromCpuHdr.object,1)
        mov     ecx,OBJ_IMAGE_FROM_CPU
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ecx
        mov     ecx,pHWLogdevCXOffset
        mov     eax,CHNRUN(ImageFromCpu.SetColorFormat,4)
        mov     ecx,ds:[ecx].HWLOGDEVCX.dwImageFromCpuColorFormat
        mov     ds:[edi + 1CH],eax
        sub     eax,eax
        mov     ds:[edi + 20H],ecx
        mov     ds:[edi + 24H],eax
        mov     eax,dwColorTableEntries
        mov     ecx,dwLUTEntriesShiftCount
        shl     eax,cl
        or      eax,10000H
        lea     ecx,[eax + 3]
        and     ecx,NOT 3
        mov     ds:[edi + 28H],eax
        mov     ds:[edi + 2CH],ecx
        sub     edx,34H
        shl     edx,10H
        mov     dx,CHNRUN(ImageFromCpu.Color,0)
        mov     ds:[edi + 30H],edx
        add     edi,34H

        mov     ecx,dwColorTableEntries
        mov     eax,dwSizeColorTableEntry
        sub     esi,esi
        les     si,lpColorTable
        push    OFFSET RetLUT
        cmp     dwDstBpp,16
        ja      CopyLUT32
        je      CopyLUT16
        jmp     CopyLUT15
RetLUT:
        mov     esi,pHWBoardCXOffset

        ; Set the dst base and pitch if need be
        mov     eax,dwDstBitsOffset
        CHECKDSTBASE ds,esi,edi,eax,<dwDstPitch>

        ; Prepare to transfer the pixels
        CHECKDMASPACE ds,esi,edi,34H
        mov     ecx,dwDstTop
        mov     edx,dwExtY
        shl     ecx,10H
        shl     edx,10H
        mov     cx,word ptr dwDstLeft
        mov     dx,word ptr dwExtX
        mov     eax,CHNRUN(ContextClipRect.SetPoint,2)
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        mov     ds:[edi + 08H],edx
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        movzx   ecx,byte ptr [dwRop3 + 2]
        mov     ds:[edi + 0CH],eax
        mov     ds:[edi + 10H],ecx
        mov     eax,CHNRUN(IndexedImageFromCpuHdr.object,1)
        mov     ecx,OBJ_INDEXED_IMAGE_FROM_CPU
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ecx

        ; We do not need to set the SetColorFormat method on the fly
        ; for this object BECAUSE for the hardware to work correctly,
        ; the 32bpp color format, NV060_SET_COLOR_FORMAT_LE_X8R8G8B8,
        ; MUST be set. The operation actually gets the color format
        ; from the CONTEXT_SURFACES_2D object embedded in it, so it
        ; seems like the SetColorFormat field should be irrelevant,
        ; but if it is not set to NV060_SET_COLOR_FORMAT_LE_X8R8G8B8
        ; the hardware draws weird color pixels. The 32Bpp format
        ; is set once when the object is created.

        mov     eax,CHNRUN(IndexedImageFromCpu.IndexFormat,5)
        mov     edx,dwColorIndexFormat
        mov     ecx,ds:[esi].HWBOARDCX.dwIndexedLUTOffset
        mov     ds:[edi + 1CH],eax
        mov     ds:[edi + 20H],edx
        mov     ds:[edi + 24H],ecx
        mov     ecx,dwExtY
        mov     eax,dwDstTop
        shl     ecx,10H
        shl     eax,10H
        mov     ax,word ptr dwAlignedDstLeft
        mov     cx,word ptr dwAlignedExtXPixels
        mov     ds:[edi + 28H],eax
        mov     ds:[edi + 2CH],ecx
        mov     ds:[edi + 30H],ecx
        add     edi,34H

        ; Load up the pointer and trasnfer the src image
        mov     eax,dwSrcTop
        mov     ebx,dwSrcPitch
        mov     esi,dwAlignedSrcLeft
        mov     ecx,dwSrcPixelsToSrcBytesShiftCount
        imul    eax,ebx
        shr     esi,cl
        mov     edx,dwAlignedExtXInDwords
        shl     edx,2
        sub     ebx,edx
        add     eax,dwSrcBitsOffset
        mov     dwSrcWrapWidth,ebx
        push    ds
        pop     es
        mov     ds,word ptr dwSrcBitsSel
        add     esi,eax

        ; Figure out how many whole scans we can do with one method header
        mov     eax,1792
        sub     edx,edx
        div     dwAlignedExtXInDwords
        mov     dwWholeScansPerBlock,eax
        imul    eax,dwAlignedExtXInDwords
        mov     ecx,eax
        lea     eax,[eax * 4 + 4]
        mov     dwBlockBytes,eax
        shl     ecx,12H
        mov     cx,CHNRUN(IndexedImageFromCpu.Indices,0)
        mov     dwBlockHeader,ecx

        ; Let's transfer as many complete src lines as we can in one chunk
LoopY:  mov     edx,dwExtY
        mov     eax,dwBlockBytes
        sub     edx,dwWholeScansPerBlock
        mov     ecx,dwBlockHeader
        mov     dwExtY,edx
        mov     edx,dwWholeScansPerBlock
        jge     @F

        ; There is only a partial block left. Adjust the parameters
        mov     edx,dwExtY
        mov     dwExtY,0
        add     edx,dwWholeScansPerBlock
        mov     eax,dwAlignedExtXInDwords
        shl     eax,2
        imul    eax,edx
        mov     ecx,eax
        add     eax,4
        shl     ecx,10H
        mov     cx,CHNRUN(IndexedImageFromCpu.Indices,0)
@@:
        ; Wait for space and transfer a whole block
        mov     ebx,pHWBoardCXOffset
        CHECKDMASPACE es,ebx,edi,eax
        mov     es:[edi],ecx
        add     edi,4
@@:
        mov     ecx,dwAlignedExtXInDwords
        rep     movs dword ptr es:[edi], ds:[esi]
        add     esi,dwSrcWrapWidth
        dec     edx
        jne     @B

        ; Let some data go
        SETPUTPTR es,ebx,edi
        cmp     ecx,dwExtY
        jl      LoopY

        ; Reset the ContextClip rectangle to the default
        CHECKDMASPACE es,ebx,edi,0CH
        mov     eax,CHNRUN(ContextClipRect.SetPoint,2)
        sub     edx,edx
        mov     ecx,07FFF7FFFH
        mov     es:[edi + 00H],eax
        mov     es:[edi + 04H],edx
        mov     es:[edi + 08H],ecx
        add     edi,0CH
        SETPUTPTR es,ebx,edi
Done:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     CopyLUT15
;
; Purpose:      This routine copies the LUT from a src color table with
;               dword entries into a dst area also with dword entries
;               also except the color are really 5-5-5 shifted left by
;               3 bits (the bottom 3 bits should always be 0.)
;
; Arguments:    es:esi          ptr to src LUT
;               ecx             number of LUT entries
;               ds:edi          dst LUT
;
; Returns:      ds:edi          must point immediately after last LUT
;                               entry copied
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  CopyLUT15
CopyLUT15       PROC    NEAR

        mov     eax,es:[esi]
        add     esi,4

        shld    edx,eax,10H
        sub     dl,04
        cmc
        sbb     dh,dh
        and     dh,dl
        and     dh,NOT 7

        mov     dl,ah
        sub     dl,4
        cmc
        sbb     ah,ah
        and     dl,ah
        and     dl,NOT 7

        sub     al,04
        cmc
        sbb     ah,ah
        and     al,ah
        and     al,NOT 7

        shl     eax,18H
        shld    edx,eax,8

        mov     ds:[edi],edx
        add     edi,4
        dec     ecx
        jne     CopyLUT15
        ret

CopyLUT15       ENDP


;==============================================================================
;
; Function:     CopyLUT16
;
; Purpose:      This routine copies the LUT from a src color table with
;               dword entries into a dst area also with dword entries
;               also except the color are really 5-6-5 shifted left by
;               3 bits for the red&blue and 2 bits for the green. (The
;               bottom bits should always be 0.)
;
; Arguments:    es:esi          ptr to src LUT
;               ecx             number of LUT entries
;               ds:edi          dst LUT
;
; Returns:      ds:edi          must point immediately after last LUT
;                               entry copied
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  CopyLUT16
CopyLUT16       PROC    NEAR

        mov     eax,es:[esi]
        add     esi,4

        shld    edx,eax,10H
        sub     dl,04
        cmc
        sbb     dh,dh
        and     dh,dl
        and     dh,NOT 7

        mov     dl,ah
        sub     dl,2
        cmc
        sbb     ah,ah
        and     dl,ah
        and     dl,NOT 3

        sub     al,04
        cmc
        sbb     ah,ah
        and     al,ah
        and     al,NOT 7

        shl     eax,18H
        shld    edx,eax,8

        mov     ds:[edi],edx
        add     edi,4
        dec     ecx
        jne     CopyLUT16
        ret

CopyLUT16       ENDP


;==============================================================================
;
; Function:     CopyLUT32
;
; Purpose:      This routine copies the LUT from a src color table with
;               dword entries into a dst area also with dword entries.
;
; Arguments:    es:esi          ptr to src LUT
;               ecx             number of LUT entries
;               ds:edi          dst LUT
;
; Returns:      ds:edi          must point immediately after last LUT
;                               entry copied
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  CopyLUT32
CopyLUT32       PROC    NEAR

        mov     eax,es:[esi + 0]
        add     esi,4
        mov     ds:[edi + 0],eax
        add     edi,4
        dec     ecx
        jne     CopyLUT32
        ret

CopyLUT32       ENDP


CLOSESEG _NVTEXT32

END

