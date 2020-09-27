;==============================================================================
;
; Copyright (C) 1998, NVidia Coporation
;
; File:         gnmonblt.asm
;
; Purpose:      This file holds the routines to do Bitblt's involving
;               a mono source located in system memory. There may
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
include	gnboard.inc
include gncrtc.inc
include gngdi.inc
include hwgdi.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GN_BitBltSrcMonoDstVram
;
; Purpose:      This function implements BitBlts where the rop involves
;               a src, the src is a mono system memory bitmap and the
;               dst is in VRAM. There may be a pattern involved as well
;               but there may not be. The general strategy implemented
;               in this routine is
;
;               1) Compute all the neccessary data
;
;               2) Call the HW_BeginSrcMonoDstVram routine to set up
;                  the chip to expect to receive dwExtX * dwExtY pixels
;                  in the Destination Pixel Depth and place them
;                  in the rectangle whose upper left corner is at
;                  dwDstLeft,dwDstTop. HW_BeginSrcMonoDstVram will
;                  also set the rop, base address, set up the pattern, etc.
;
;               3) GN_BitBltSrcMonoDstVram calls the routine
;                  HW_BlockSrcMonoDstVram to wait for enough space
;                  in the hardware buffer.
;
;               4) GN_BitBltSrcMonoDstVram calls an inner loop routine
;                  which reads src pixels, translates them to dst space
;                  and stores them in the hardware. The inner loop routine
;                  can do partial scanlines or an integral number of scans.
;
;               5) Steps 3 and 4 are repeated until the entire bitmap has
;                  been transferred.
;
;               6) HW_EndSrcMonoDstVram is called to allow the hardware
;                  specific code to clean up anything it needs to.
;
; Arguments:
;		ds		Context Selector
;		pGNLogdevCXOffset  GNLOGDEVCX ptr
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
;               dwSrcPitch      address of first pixel on line n+1 minus
;                               address of first pixel on line n.
;                               This number might be negative if the
;                               DIB is upside down.
;               dwSrcLeft       left edge of the src rectangle
;               dwSrcTop        top  edge of the src rectangle
;               dwExtX          extent of the src (and dst) rectangle.
;               dwExtY          extent of the src (and dst) rectangle.
;               dwBgColor       expand 0 bits in the src to this color
;               dwFgColor       expand 1 bits in the src to this color
;               dwRop3          3-operand rop
;               lpBrush         ptr to a DIB_Brush struct
;               lpDrawMode      This is only needed for the case where
;                               the rop involves a brush and the brush
;                               is a mono pattern. In this case,
;                               DRAWMODE.TextColor specifies the color
;                               to which to translate pattern 0 bits, and
;                               DRAWMODE.bkColor specifies the color to
;                               which to translate pattern 1 bits. Note
;                               that these are not the same colors used
;                               translate 0 and 1 bits from the src
;                               bitmap.
;               dwScansLeftInSeg number of scanlines to process starting
;                               from the dwSrcTop coord passed in before
;                               the segment break and byte padding.
;               dwScansPerSeg   number of scans between segment breaks
;               dwFillBytes     number of bytes to add to src ptr to
;                               skip padding between bitmap segment breaks.
;
; Returns:      eax     0       The routine will be punted to software
;                       non-0   the return code to GDI
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_BitBltSrcMonoDstVram, PASCAL, FRAME, NEAR
PARMD   pGNLogdevCXOffset
PARMD   dwDstBitsSel
PARMD   dwDstBitsOffset
PARMD   dwDstBpp
PARMD   dwDstPitch
PARMD   dwDstLeft
PARMD   dwDstTop
PARMD   dwSrcBitsSel
PARMD   dwSrcBitsOffset
PARMD   dwSrcPitch
PARMD   dwSrcLeft
PARMD   dwSrcTop
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwBgColor
PARMD   dwFgColor
PARMD   dwRop3
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   dwScansLeftInSeg
PARMD   dwScansPerSeg
PARMD   dwFillBytes

LOCALD  dwExtraWord
LOCALD  dwPixelsIgnoredOnLeft
LOCALD  dwAlignedDstLeft
LOCALD  dwAlignedSrcLeft
LOCALD  dwAlignedExtXInPixels
LOCALD  dwAlignedSrcExtXInBytes
LOCALD  dwAlignedSrcExtXInDwords
LOCALD  dwMaxTransferSizeInBytes
LOCALD  dwSrcStart
LOCALD  dwDstWrapWidth
LOCALD  dwSrcWrapWidth
LOCALD  dwDwordsPerBlock
LOCALD  dwScansPerBlock
LOCALD  dwBytesPerBlock
LOCALD	pHWBoardCXOffset
OPENPROC
        PUSHR   ds,esi,edi

        ; We assume a hardware clip rect; hence, drop the starting
        ; src coord back to the start of a dword. The dst will need
        ; to be dropped back the same amount and the width will need
        ; to be increased by the same amount. Then to make things
        ; even simpler, increase the width from this new value until
        ; it is a multiple of 32, thus making the amount of src data
        ; to transfer per scan a multiple of a dword.
        mov     eax,dwSrcLeft
        mov     ebx,eax
        and     eax,NOT 1FH
        mov     dwAlignedSrcLeft,eax
        sub     ebx,eax
        mov     ecx,dwDstLeft
        mov     edx,dwExtX
        mov     dwPixelsIgnoredOnLeft,ebx
        sub     ecx,ebx
        lea     eax,[edx][ebx + 0FH]
        lea     edx,[edx][ebx + 1FH]
        and     eax,NOT 0FH
        and     edx,NOT 1FH
        mov     dwAlignedExtXInPixels,edx
        sub     eax,edx
        mov     dwExtraWord,eax
        mov     dwAlignedDstLeft,ecx
        shr     edx,3
        mov     dwAlignedSrcExtXInBytes,edx
        shr     edx,2
        mov     dwAlignedSrcExtXInDwords,edx

        ; Now we compute the src starting address and wrap width
        mov     eax,dwSrcTop
        sub     edx,edx
        div     dwScansPerSeg
        shl     eax,10H
        imul    edx,dwSrcPitch
        mov     ebx,dwAlignedSrcLeft
        add     eax,edx
        shr     ebx,3
        add     eax,dwSrcBitsOffset
        add     eax,ebx
        mov     dwSrcStart,eax
        mov     eax,dwSrcPitch
        sub     eax,dwAlignedSrcExtXInBytes
        mov     dwSrcWrapWidth,eax

        ; Call the hardware specific routine to setup for the transfer
        mov	ebx,pGNLogdevCXOffset
        mov    	ecx,ds:[ebx].GNLOGDEVCX.pGNBoardCX
	mov	eax,ds:[ecx].GNBOARDCX.pHWBoardCX
	mov	pHWBoardCXOffset,eax

        push    ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        push    dwDstBitsOffset
        push    dwDstPitch
        push    dwDstBpp
        push    dwDstLeft
        push    dwDstTop
        push    dwAlignedDstLeft
        push    dwExtX
        push    dwExtY
        push    dwAlignedSrcExtXInBytes
        push    dwAlignedExtXInPixels
        push    dwPixelsIgnoredOnLeft
        push    dwBgColor
        push    dwFgColor
        push    lpBrush
        push    lpDrawMode
        push    dwRop3
        call    HW_BeginSrcMonoDstVram
        or      eax,eax
        je      DoneIt
        mov     dwDstWrapWidth,ebx
        mov     dwMaxTransferSizeInBytes,ecx

        ; At this point, the generic and hardware code are linked
        ; until the HW_EndSrcMonoDstVram routine is called.
        ; The generic code has agreed not to touch es -- it is the
        ; hardware's selector for whatever purposes it may need.

        ; Now we can begin the inner loop. Ther are two major cases
        ; that can occur and we will handle them separately.
        ; 1) The number of bytes that are to be transferred for a single
        ;    scanline is larger than the most the hardware can accept
        ;    in one shot. Each scanline must be broken into multiple
        ;    pieces. This is most typically the FIFO based model.
        ; 2) At least one scanline, possibly many, can be sent as a
        ;    single transfer. This is the typical DMA scenario.
        ;
        ; For each of these cases, there are two subcases. Mono bitmaps
        ; are only guaranteed to have a word width and if we transfer a
        ; multiple of 4 bytes per scan, on the last scan we could walk
        ; off the end of the bitmap casuing a GPF unless we handle the
        ; scan properly. We don't have to worry about this unless the
        ; dwSrcPitch is NOT a multiple of 4 AND in extending the width
        ; of the transfer out to be dword aligned on the left edge, we
        ; have included a word in which all pixels are clipped.
        push    OFFSET CRet
        mov     eax,dwSrcPitch
        mov     ebx,dwExtraWord
        and     eax,3
        je      IsAlgn
        or      ebx,ebx
        je      IsAlgn
IsUnAl: cmp     ecx,dwAlignedSrcExtXInBytes
        jle     GN_XferPartialUnalignedMonoScans
        jmp     GN_XferWholeUnalignedMonoScans
IsAlgn: cmp     ecx,dwAlignedSrcExtXInBytes
        jle     GN_XferPartialAlignedMonoScans
Whole:  jmp     GN_XferWholeAlignedMonoScans
CRet:
        ; Let the hardware know we are done
        mov	esi,pHWBoardCXOffset
        call    HW_EndSrcMonoDstVram

        mov     eax,1
DoneIt:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_XferWholeAlignedMonoScans
;
; Purpose:      This function is called from GN_BitBltSrcMonoDstVram
;               when at least one whole scan will fit in the transfer
;               area of the chip AND the pitch of the src bitmap is a
;               multiple of a dword. It handles calling the hardware for
;               space and then calling the inner loop to transfer data.
;
; Arguments:    Shares stack FRAME with GN_BitBltSrcMonoDstVram
;               es      hardware specific code has set this
;                       Do not change it!
;               edi     Can be incremented appropriately as each scan
;                       is put in the buffer and the wrapWidth can be
;                       added appropriately. These are the only changes
;                       allowed.
;
; Returns:      None
;
; Preserve:     None
;==============================================================================
PUBLIC  GN_XferWholeAlignedMonoScans
GN_XferWholeAlignedMonoScans    PROC    NEAR

        ; Figure out how many scanlines can be done in a single shot
        mov     eax,dwMaxTransferSizeInBytes
        sub     edx,edx
        div     dwAlignedSrcExtXInBytes
        mov     dwScansPerBlock,eax
        imul    eax,dwAlignedSrcExtXInBytes
        mov     dwBytesPerBlock,eax
        mov     esi,dwSrcStart
        mov     ds,word ptr dwSrcBitsSel
LoopY:
        ; Compute the number of scans in the next chunk
        mov     eax,dwBytesPerBlock
        mov     ebx,dwScansPerBlock
        sub     dwExtY,ebx
        jge     @F
        add     ebx,dwExtY
        mov     dwExtY,0
        mov     eax,dwAlignedSrcExtXInBytes
        imul    eax,ebx
@@:
        ; Have the hardware wait for enough space to hold one chunk's
        ; worth of bytes. The hardware returns in es:edi a ptr to where
        ; to place the dst pixels.
        PUSHR	esi
        mov	esi,pHWBoardCXOffset
        call    HW_BlockSrcMonoDstVram
        POPR	esi
@@:
        ; Now we transfer the data
        mov     ecx,dwAlignedSrcExtXInDwords
        rep     movs dword ptr es:[edi], ds:[esi]
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwScansLeftInSeg
        je      Wrap
WrapD:
        dec     ebx
        jne     @B
        cmp     ebx,dwExtY
        jne     LoopY
        ret
Wrap:
        mov     eax,dwScansPerSeg
        mov     dwScansLeftInSeg,eax
        add     esi,dwFillBytes
        jmp     WrapD

GN_XferWholeAlignedMonoScans    ENDP


;==============================================================================
;
; Function:     GN_XferWholeUnalignedMonoScans
;
; Purpose:      This function is called from GN_BitBltSrcMonoDstVram
;               when at least one whole scan will fit in the transfer
;               area of the chip AND the pitch of the src bitmap is NOT a
;               multiple of a dword. It handles calling the hardware for
;               space and then calling the inner loop to transfer data.
;
; Arguments:    Shares stack FRAME with GN_BitBltSrcMonoDstVram
;               es      hardware specific code has set this
;                       Do not change it!
;               edi     Can be incremented appropriately as each scan
;                       is put in the buffer and the wrapWidth can be
;                       added appropriately. These are the only changes
;                       allowed.
;
; Returns:      None
;
; Preserve:     None
;==============================================================================
PUBLIC  GN_XferWholeUnalignedMonoScans
GN_XferWholeUnalignedMonoScans  PROC    NEAR

        ; Figure out how many scanlines can be done in a single shot
        mov     eax,dwMaxTransferSizeInBytes
        sub     edx,edx
        div     dwAlignedSrcExtXInBytes
        mov     dwScansPerBlock,eax
        imul    eax,dwAlignedSrcExtXInBytes
        mov     dwBytesPerBlock,eax
        mov     esi,dwSrcStart
        mov     ds,word ptr dwSrcBitsSel

        ; We must transfer one less dword and then one extra word
        ; so adjust some things to make up for this change.
        mov     eax,4
        dec     dwAlignedSrcExtXInDwords
        add     dwSrcWrapWidth,eax
        add     dwDstWrapWidth,eax
LoopY:
        ; Compute the number of scans in the next chunk
        mov     eax,dwBytesPerBlock
        mov     ebx,dwScansPerBlock
        sub     dwExtY,ebx
        jge     @F
        add     ebx,dwExtY
        mov     dwExtY,0
        mov     eax,dwAlignedSrcExtXInBytes
        imul    eax,ebx
@@:
        ; Have the hardware wait for enough space to hold one chunk's
        ; worth of bytes. The hardware returns in es:edi a ptr to where
        ; to place the dst pixels.
        PUSHR	esi
        mov	esi,pHWBoardCXOffset
        call    HW_BlockSrcMonoDstVram
        POPR	esi
@@:
        ; Now we transfer the data
        mov     ecx,dwAlignedSrcExtXInDwords
        rep     movs dword ptr es:[edi], ds:[esi]
        mov     ax,ds:[esi]
        mov     es:[edi],ax
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwScansLeftInSeg
        je      Wrap
WrapD:
        dec     ebx
        jne     @B
        cmp     ebx,dwExtY
        jne     LoopY
        ret
Wrap:
        mov     eax,dwScansPerSeg
        mov     dwScansLeftInSeg,eax
        add     esi,dwFillBytes
        jmp     WrapD

GN_XferWholeUnalignedMonoScans  ENDP


;==============================================================================
;
; Function:     GN_XferPartialAlignedMonoScans
;
; Purpose:      This function is called from GN_BitBltSrcMonoDstVram
;               when one whole scan will not fit in the transfer
;               area of the chip AND the pitch of the src bitmap is a
;               multiple of a dword. It handles calling the hardware for
;               space and then calling the inner loop to transfer data.
;
; Arguments:    Shares stack FRAME with GN_BitBltSrcMonoDstVram
;               es      hardware specific code has set this
;                       Do not change it!
;               edi     Can be incremented appropriately as each scan
;                       is put in the buffer and the wrapWidth can be
;                       added appropriately. These are the only changes
;                       allowed.
;
; Returns:      None
;
; Preserve:     None
;==============================================================================
PUBLIC  GN_XferPartialAlignedMonoScans
GN_XferPartialAlignedMonoScans  PROC    NEAR

        ; We can't send a whole scanline in a single transfer so compute
        ; how many dwords (chunks of 32 pixels) we can send in one shot.
        mov     eax,dwMaxTransferSizeInBytes
        mov     dwBytesPerBlock,eax
        shr     eax,2
        mov     dwDwordsPerBlock,eax
        mov     esi,dwSrcStart
        mov     ds,word ptr dwSrcBitsSel

LoopY:
        push    dwAlignedSrcExtXInDwords
LoopX:
        ; Compute the number of pixels in the next chunk
        mov     eax,dwBytesPerBlock
        mov     ebx,dwDwordsPerBlock
        sub     dwAlignedSrcExtXInDwords,ebx
        jge     @F
        add     ebx,dwAlignedSrcExtXInDwords
        mov     dwAlignedSrcExtXInDwords,0
        lea     eax,[ebx * 4]
@@:
        ; Have the hardware wait for enough space to hold one chunk's
        ; worth of bytes. The hardware returns in es:edi a ptr to where
        ; to place the dst pixels.
        PUSHR	esi
        mov	esi,pHWBoardCXOffset
        call    HW_BlockSrcMonoDstVram
        POPR	esi

        ; Now we transfer the data
        mov     ecx,ebx
        rep     movs dword ptr es:[edi], ds:[esi]

        ; Are there more pixels left on this scanline?
        cmp     ecx,dwAlignedSrcExtXInDwords
        jne     LoopX
        pop     dwAlignedSrcExtXInDwords

        ; Go to the next scanline
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwScansLeftInSeg
        je      Wrap
WrapD:
        dec     dwExtY
        jg      LoopY
        ret
Wrap:
        mov     eax,dwScansPerSeg
        mov     dwScansLeftInSeg,eax
        add     esi,dwFillBytes
        jmp     WrapD

GN_XferPartialAlignedMonoScans  ENDP


;==============================================================================
;
; Function:     GN_XferPartialUnalignedMonoScans
;
; Purpose:      This function is called from GN_BitBltSrcMonoDstVram
;               when one whole scan will not fit in the transfer
;               area of the chip AND the pitch of the src bitmap is NOT a
;               multiple of a dword. It handles calling the hardware for
;               space and then calling the inner loop to transfer data.
;
; Arguments:    Shares stack FRAME with GN_BitBltSrcMonoDstVram
;               es      hardware specific code has set this
;                       Do not change it!
;               edi     Can be incremented appropriately as each scan
;                       is put in the buffer and the wrapWidth can be
;                       added appropriately. These are the only changes
;                       allowed.
;
; Returns:      None
;
; Preserve:     None
;==============================================================================
PUBLIC  GN_XferPartialUnalignedMonoScans
GN_XferPartialUnalignedMonoScans        PROC    NEAR

        ; We can't send a whole scanline in a single transfer so compute
        ; how many dwords (chunks of 32 pixels) we can send in one shot.
        mov     eax,dwMaxTransferSizeInBytes
        mov     dwBytesPerBlock,eax
        shr     eax,2
        mov     dwDwordsPerBlock,eax
        mov     esi,dwSrcStart
        mov     ds,word ptr dwSrcBitsSel

        ; We must transfer one less dword and then one extra word
        ; so adjust some things to make up for this change.
        mov     eax,4
        add     dwSrcWrapWidth,eax
        add     dwDstWrapWidth,eax

LoopY:
        push    dwAlignedSrcExtXInDwords
LoopX:
        ; Compute the number of pixels in the next chunk
        mov     eax,dwBytesPerBlock
        mov     ebx,dwDwordsPerBlock
        sub     dwAlignedSrcExtXInDwords,ebx
        jge     @F
        add     ebx,dwAlignedSrcExtXInDwords
        mov     dwAlignedSrcExtXInDwords,0
        lea     eax,[ebx * 4]
        dec     ebx             ; allocate, but don't fill last dword
@@:
        ; Have the hardware wait for enough space to hold one chunk's
        ; worth of bytes. The hardware returns in es:edi a ptr to where
        ; to place the dst pixels.
        PUSHR	esi
        mov	esi,pHWBoardCXOffset
        call    HW_BlockSrcMonoDstVram
        POPR	esi

        ; Now we transfer the data
        mov     ecx,ebx
        rep     movs dword ptr es:[edi], ds:[esi]

        ; Are there more pixels left on this scanline?
        cmp     ecx,dwAlignedSrcExtXInDwords
        jne     LoopX
        mov     ax,ds:[esi]
        mov     es:[edi],ax
        pop     dwAlignedSrcExtXInDwords

        ; Go to the next scanline
        add     esi,dwSrcWrapWidth 
        add     edi,dwDstWrapWidth 
        dec     dwScansLeftInSeg
        je      Wrap
WrapD:
        dec     dwExtY
        jg      LoopY
        ret
Wrap:
        mov     eax,dwScansPerSeg
        mov     dwScansLeftInSeg,eax
        add     esi,dwFillBytes
        jmp     WrapD

GN_XferPartialUnalignedMonoScans        ENDP

CLOSESEG _NVTEXT32

END

