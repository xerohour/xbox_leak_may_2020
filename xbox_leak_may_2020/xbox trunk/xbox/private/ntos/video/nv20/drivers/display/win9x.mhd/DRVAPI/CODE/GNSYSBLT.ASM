;==============================================================================
;
; Copyright (C) 1998, NVidia Coporation
;
; File:         gnsysblt.asm
;
; Purpose:      This file holds the routines to do Bitblt's involving
;               a source located in video memory and a dst located in
;               system memory. There may also be a pattern involved but
;               maybe not. The code is generic in terms of not touching
;               hardware, but it does make assumptions about hardware
;               capability and to some extent programming order. These
;               should all be fine for Nvidia products.
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

;==============================================================================
;
; Function:     GN_BitBltSrcVramDstSystem
;
; Purpose:      This function implements BitBlts where the rop involves
;               a src, the src is a color video memory bitmap and the
;               dst is in system memory (color or mono). There may be
;               a pattern involved as well but there may not be.
;
; Arguments:
;               ds              Context Selector
;               pGNLogdevCXOffset  GNLOGDEVCX ptr
;               lpDstDev        Ptr to the dst device (DIBENG header)
;                               Can be NULL if the actual dst is really
;                               a DIB in which case the lpDstDib holds
;                               the real dst. Either lpDstDib or lpDstDev
;                               must be valid and the other must be NULL.
;               lpDstDib        Ptr to the dst Device (BitmapInfoHeader)
;                               Can be NULL if the actual dst is really
;                               a bitmap in which case lpDstDev holds
;                               the real dst. Either lpDstDib or lpDstDev
;                               must be valid and the other must be NULL.
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
;               dwSrcLeft       top  edge of the src rectangle
;               dwExtX          extent of the src (and dst) rectangle.
;               dwExtY          extent of the src (and dst) rectangle.
;               dwRop3          3-operand rop
;               lpBrush         ptr to a DIB_Brush struct
;               lpDrawMode      This is only needed for the case where
;                               the rop involves a brhs and the brush
;                               is a mono pattern. In this case,
;                               DRAWMODE.TextColor specifies the color
;                               to which to translate src 0 bits, and
;                               DRAWMODE.bkColor specifies the color
;                               to which to translate src 1 bits. Note
;                               that these are not the same colors used
;                               translate 0 and 1 bits from the src
;                               bitmap.
;               pfnCopyXlat     If this argument is not NULL then it is
;                               the correct inner loop to use. If it is
;                               NULL, then the DIBENG should be used.
;
; Returns:      eax     0       The routine will be punted to software
;                       else    the return code to GDI
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_BitBltSrcVramDstSystem, PASCAL, FRAME, NEAR
PARMD   pGNLogdevCXOffset
PARMD   lpDstDev
PARMD   lpDstDib
PARMD   dwDstBitsSel
PARMD   dwDstBitsOffset
PARMD   dwDstBpp
PARMD   dwDstPitch
PARMD   dwDstLeft
PARMD   dwDstTop
PARMD   lpSrcDev
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
PARMD   pfnCopyXlat

LOCALD  pHWLogdevCXOffset
LOCALD  pHWBoardCXOffset
LOCALD  dwScreenPalettized
LOCALD  dwSrcBufferPitch
LOCALD  dwMaxScansPerTransfer
LOCALD  dwDstBltOffset
LOCALD  dwScansRemaining
LOCALD  dwSrcTopOffset
LOCALD  dwSrcWrapWidth
LOCALD  dwDstWrapWidth
LOCALD  dwUseDibeng
LOCALD  lpDMASrcDev
LOCALD  lpDMADstDev
LOCALS  dwBlockTop,8
LOCALS  dwBlockHeight,8
LOCALS  dwBlockTransferInitiated,8
LOCALS  dwBlockOffset,8
LOCALV  sSrcPDevice,DIBENGINEHDR
LOCALV  sDstPDevice,DIBENGINEHDR
OPENPROC
        PUSHR   ds,esi,edi
        mov     esi,pGNLogdevCXOffset
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     eax,ds:[esi].GNLOGDEVCX.pHWLogdevCX
        mov     pHWLogdevCXOffset,eax
        mov     eax,ds:[ebx].GNBOARDCX.pHWBoardCX
        mov     pHWBoardCXOffset,eax
        test    ds:[esi].GNLOGDEVCX.Dibeng.deFlags,PALETTIZED
        setne   al
        movzx   eax,al
        mov     dwScreenPalettized,eax

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Tell the hardware specific code the src bitmap with which
        ; we will be working and have it return some important data.
        push    pHWLogdevCXOffset
        push    dwSrcBitsSel
        push    dwSrcBitsOffset
        push    dwSrcBpp
        push    dwSrcPitch
        push    dwSrcLeft
        push    dwExtX
        call    HW_BeginSrcVramDstSystem
        or      eax,eax
        je      ExitIt
        mov     dwSrcBufferPitch,ebx
        mov     dwMaxScansPerTransfer,ecx

        ; At this point, the generic and hardware code are linked
        ; until the HW_EndSrcVramDstSystem routine is called.
        ; The generic code has agreed not to touch fs -- it is the
        ; hardware's selector for whatever purposes it may need.
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; For each chunk of the bitmap we DMA back, we need to rop it
        ; in with the destination. If the caller passed in a non-NULL
        ; pfnCopyXlat, then it is the routine to use to do the ropping.
        ; Otherwise, we will use the DIBENG to do the ropping.
        push    OFFSET SetupDone
        cmp     pfnCopyXlat,0
        jne     OurRopSetup
        jmp     DibengRopSetup
SetupDone:

        sub     ebx,ebx
        mov     dword ptr dwBlockTransferInitiated[ebx * 4 + 0],0
        mov     dword ptr dwBlockTransferInitiated[ebx * 4 + 4],0
        mov     eax,dwExtY
        mov     dwScansRemaining,eax
        mov     dwSrcTopOffset,ebx

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Enter the following code with ebx = to 0 or 1. ebx is the
        ; ID of which of two ping pong buffers in the DMA transfer
        ; area we are currently using. It doesn't matter which one
        ; you start with -- this inner loop will toggle it for you.
LoopY:
        ; Compute the smaller of:
        ; 1) As many scans as the hardware can
        ; 2) The number of scans remaining to process
        ; Save off the coords of the block we will be DMAing back
        mov     eax,dwMaxScansPerTransfer
        cmp     eax,dwScansRemaining
        jl      @F
        mov     eax,dwScansRemaining
@@:     sub     dwScansRemaining,eax
        or      eax,eax
        je      DoneXfer
        mov     ecx,dwSrcTopOffset
        mov     dword ptr dwBlockHeight[ebx * 4],eax
        mov     dword ptr dwBlockTop[ebx * 4],ecx
        mov     dword ptr dwBlockTransferInitiated[ebx * 4],1
        lea     edx,[eax + ecx]
        mov     dwSrcTopOffset,edx

        ; DMA back what we have computed as the Block rectangle
        push    pHWBoardCXOffset
        push    dwSrcBitsOffset
        push    dwSrcBpp
        push    dwSrcPitch
        push    dwSrcLeft
        add     ecx,dwSrcTop
        push    ecx
        push    dwExtX
        push    eax
        call    HW_BlockSrcVramDstSystem
        mov     dword ptr dwBlockOffset[ebx * 4],ecx

DoneXfer:
        ; Flip to the other block
        xor     ebx,1

        ; Call the hardware specific code to wait for the other block
        ; (the one we did not just issue the transfer for immediately
        ; above) to complete. The first time through, there will be
        ; no "other" block that has yet been transfered, so skip the
        ; waiting and the copy.
        cmp     dword ptr dwBlockTransferInitiated[ebx * 4],0
        je      DoneCopy
        mov     dword ptr dwBlockTransferInitiated[ebx * 4],0
        mov     edx,pHWBoardCXOffset
        call    HW_WaitSrcVramDstSystem

        ; Now call the appropriate routine to handle the Blt for the Block
        cmp     dwUseDibeng,0
        jne     UseDibeng

        ; Using our own internal routines -- faster but only handle
        ; SrcCopy from any color depth to any other.
        push    ebx
        mov     esi,dword ptr dwBlockOffset[ebx * 4]
        mov     edi,dwDstBltOffset
        push    dwExtX
        push    dword ptr dwBlockHeight[ebx * 4]
        push    dwSrcWrapWidth
        push    dwDstWrapWidth
        call    NEAR PTR pfnCopyXlat
        mov     dwDstBltOffset,edi
        pop     ebx

DoneCopy:
        ; We know we are done when there are no active transfers
        mov     eax,dword ptr dwBlockTransferInitiated[0 * 4]
        or      eax,dword ptr dwBlockTransferInitiated[1 * 4]
        jne     LoopY
        jmp     DoneLoop

UseDibeng:
        call    RopBackWithDibeng
        jmp     DoneCopy

DoneLoop:
        ; Let the hardware know we are done
        mov     edx,pHWBoardCXOffset
        call    HW_EndSrcVramDstSystem
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DoneIt:
        mov     eax,1
ExitIt:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     OurRopSetup
;
; Purpose:      This function is called when we will be using some of
;               our own private routines to do the ropping of data that
;               has been DMAed back from the screen. It sets up some
;               variables that are needed for our inner loop.
;
; Arguments:    stack frame for GN_BitBltSrcVramDstSystem
;               ds      Context Selector
;
; Returns:      None
;
; Preserve:     ds
;==============================================================================
PUBLIC  OurRopSetup
OurRopSetup     PROC    NEAR

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Setup variables that we know our pfnCopyXlat routine needs.
        ; Compute the dst addresses. For dst that are multiples of a
        ; byte/pixel, the addresses will be byte addresses. For 4bpp,
        ; addresses will be nibble addresses. For 1bpp, bit addresses.
OurSetup:
        mov     dwUseDibeng,0
        mov     eax,dwDstPitch
        mov     edx,dwDstTop
        imul    edx,eax
        add     edx,dwDstBitsOffset
        mov     es,word ptr dwDstBitsSel

        mov     ecx,dwDstBpp
        mov     ebx,dwExtX
        cmp     ecx,4
        je      Is4
        jc      Is1
Is8orM:
        inc     ecx
        and     ecx,NOT 1
        shr     ecx,3
        imul    ebx,ecx
        sub     eax,ebx
        imul    ecx,dwDstLeft
        add     edx,ecx
        jmp     @F
Is4:
        add     eax,eax
        add     edx,edx
        sub     eax,ebx
        add     edx,dwDstLeft
        jmp     @F
Is1:
        shl     eax,3
        shl     edx,3
        sub     eax,ebx
        add     edx,dwDstLeft
@@:
        mov     dwDstWrapWidth,eax
        mov     dwDstBltOffset,edx

        ; Compute the src wrap width
        mov     eax,dwSrcBufferPitch
        mov     ecx,dwSrcBpp
        inc     ecx
        and     ecx,NOT 1
        shr     ecx,3
        imul    ecx,dwExtX
        sub     eax,ecx
        mov     dwSrcWrapWidth,eax
        ret

OurRopSetup     ENDP


;==============================================================================
;
; Function:     DibengRopSetup
;
; Purpose:      This function is called when the Dibeng will be used
;               to do the ropping of data that has been DMAed back from
;               the screen. It sets up some variables that are needed
;               by either DIB_BitBlt or DIB_DibBlt, one of which will
;               be called by the BitBltSrcVramDstSystem interleaved
;               with calls to the DMA routine. The Dibeng routines
;               need a DIBENGINEHDR structure created that describes
;               the DMA transfer area since that will be the source
;               as far as the DIBENG is concerned.
;
; Arguments:    stack frame for GN_BitBltSrcVramDstSystem
;               ds      Context Selector
;
; Returns:      None
;
; Preserve:     ds
;==============================================================================
PUBLIC  DibengRopSetup
DibengRopSetup  PROC    NEAR

        ; We need a DEVICE descriptor for the src which, as far
        ; as the DIBENG is concerned will be the DMA transfer region.
        mov     dwUseDibeng,1
        sub     esi,esi
        les     si,lpSrcDev
        mov     sSrcPDevice.deType,TYPE_DIBENG
        mov     eax,dwExtX
        mov     ebx,dwExtY
        mov     ecx,dwSrcBpp
        inc     ecx
        shr     ecx,3
        imul    ecx,eax
        mov     sSrcPDevice.deWidth,ax
        mov     sSrcPDevice.deHeight,bx
        mov     eax,dword ptr es:[esi].DIBENGINE.deWidthBytes
        mov     ax,cx
        mov     dword ptr sSrcPDevice.deWidthBytes,eax
        mov     eax,dwSrcBufferPitch
        mov     sSrcPDevice.deDeltaScan,eax
        mov     eax,cs:[CSlp16LogDevice]
        mov     sSrcPDevice.delpPDevice,eax
;       mov     dword ptr sSrcPDevice.deBits,           ; Filled in later
        mov     word ptr sSrcPDevice.deBitsSelector,ds
        mov     eax,dword ptr es:[esi].DIBENGINE.deFlags
        and     eax,NOT (VRAM OR OFFSCREEN OR BUSY)
        test    eax,PALETTE_XLAT
        jz      @F
        or      eax,VRAM
@@:
        mov     dword ptr sSrcPDevice.deFlags,eax
        mov     eax,es:[esi].DIBENGINE.deBitmapInfo
        mov     sSrcPDevice.deBitmapInfo,eax
        sub     eax,eax
        ; The bRealBpp field is not needed for this temporary device
        mov     dword ptr sSrcPDevice.bRealBpp,eax
        mov     sSrcPDevice.deBeginAccess,eax
        mov     sSrcPDevice.deEndAccess,eax
        mov     ax,ss
        lea     esi,sSrcPDevice
        shl     eax,10H
        mov     ax,si
        mov     lpDMASrcDev,eax

        mov     eax,lpDstDev
        or      eax,eax
        jne     @F
        mov     eax,lpDstDib
@@:     mov     lpDMADstDev,eax
        ret

DibengRopSetup  ENDP


;==============================================================================
;
; Function:     RopBackWithDibeng
;
; Purpose:      This function is called when the Dibeng should be called
;               to take the data transferred back from the screen, which
;               is now in the DMA transfer region in system memory, and
;               use it as the src in the blt which needs to happen.
;               Based upon whether the caller gave us an lpDstDev or an
;               lpDstDib argument to GN_BitBltSrcVramDstSystem we will
;               call DIB_BitBlt or DIB_DibBlt respectively.
;
; Arguments:    stack frame for GN_BitBltSrcVramDstSystem
;
; Returns:      None
;
; Preserve:     ds,ebx
;==============================================================================
PUBLIC  RopBackWithDibeng
RopBackWithDibeng       PROC    NEAR

        mov     eax,dword ptr dwBlockOffset[ebx * 4]
        mov     dword ptr sSrcPDevice.deBitsOffset,eax

        ; We are using the DIBENG, so we will call either BitBlt or
        ; DibBlt. The src is definitely a bitmap (it is a bitmap in
        ; VRAM, right? -- that's why we are in this routine). The dst
        ; can either be a DIB or a bitmap. Call BitBlt if it is a
        ; bitmap and DibBlt if it is a DIB.
        cmp     lpDstDev,0
        je      UseDibBlt

        PUSHR   ebx
        push    lpDMADstDev
        push    word ptr dwDstLeft
        mov     eax,dwDstTop
        add     eax,dword ptr dwBlockTop[ebx * 4]
        push    ax
        push    lpDMASrcDev
        sub     eax,eax
        push    eax
        mov     eax,dwExtX
        mov     ecx,dword ptr dwBlockHeight[ebx * 4]
        push    ax
        push    cx
        push    dwRop3
        push    lpBrush
        push    lpDrawMode
        call    DIB_BitBlt
        POPR    ebx
        ret

UseDibBlt:
IF 1
        PUSHR   ds,ebx

        ; Conceptually, the important things are:
        ; iStart = BlockTop
        ; DIBBitsPtr = abs(dwDstPitch * dwBlockTop) + dwDstBitsOffset
        ; But we have to adjust the DIBBitsPtr since we can only pass
        ; 16:16 ptrs to the DIBENG. See comments later about tiling.

        ; We need to make some adjustments. Conceptually
        ; DIBBitsPtr = dwDstBitsOffset + (BlockTop + BlockHeight) * DibPitch
        ; DibHeightInHdr temporarily becomes BlockHeight
        ;
        ; The only problem is that DIBBitsPtr might be >= 64K which
        ; means we will need to adjust the selector appropriately ...
        sub     esi,esi
        lds     si,lpDMADstDev

        ; First save the current DIB Height and reset it to the band height
        push    ds:[esi].BitmapInfoHeader.biHeight
        cmp     ds:[esi].BitmapInfoHeader.biHeight,0
        jg      positive_height

        ; Negative DIB Height
        mov     ecx,dword ptr dwBlockHeight[ebx * 4]
        neg     ecx
        mov     ds:[esi].BitmapInfoHeader.biHeight,ecx
        neg     ecx

        ; Compute the DIBBitsPtr
        sub     edx,edx
        mov     eax,dword ptr dwBlockTop[ebx * 4]
        imul    eax,dwDstPitch
        add     eax,dwDstBitsOffset
        jmp     neg_continue

positive_height:
        mov     ecx,dword ptr dwBlockHeight[ebx * 4]
        mov     ds:[esi].BitmapInfoHeader.biHeight,ecx

        ; Compute the DIBBitsPtr
        sub     edx,edx
        mov     eax,dword ptr dwBlockTop[ebx * 4]
        add     eax,ecx
        dec     eax
        imul    eax,dwDstPitch
        add     eax,dwDstBitsOffset

        ; The offset is >= 64K. Windows will have tiled the selectors
        ; so we can just add 8 to the selector for every multiple we
        ; are over 64K.
neg_continue:
        test    eax,0FFFF0000H
        jz      @F
        shld    edx,eax,10H + 3
        and     edx,NOT 7
@@:     add     edx,dwDstBitsSel

        push    lpDMASrcDev
        pushd   10000H
        push    cx
        push    dx
        push    ax
        push    lpDMADstDev
        push    lpDrawMode
        xor     eax,eax
        push    eax
        push    word ptr dwScreenPalettized
        call    DIB_DibBltExt

        ; Restore the real dib height
        pop     ds:[esi].BitmapInfoHeader.biHeight
        POPR    ds,ebx
        ret
ENDIF

RopBackWithDibeng       ENDP

CLOSESEG _NVTEXT32

END

