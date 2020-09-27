;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gncp32.asm
;
; Purpose:      This file has inner loop routines in it to copy 4, 8,
;               15, 16, 24, or 32bpp bitmaps to a 32bpp dst bitmap.
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

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GN_Xfer4To32
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 4bpp src bitmap to a 32bpp dst bitmap. The color
;               translate table is indexed with a 4bpp value (a number
;               from 0 through 15 inclusive) scaled up by a factor of 4,
;               and a dword 32bpp RGB value is pulled out of the table.
;               and used as the dst pixel.
;
; Arguments:    ds:esi  ptr to first src nibble
;                       NOTE: This is a nibble address!!!
;                       You must divide esi by 2 to get the byte address
;                       If esi is even then use the high nibble in the byte
;                       If esi is odd then use the low nibble in the byte
;               es:edi  ptr to first dst pixel -- normal byte address
;               fs:ebx  ptr to color translate table
;               dwExtX  In Pixels
;               dwExtY  In Scanlines -- if this number is 0, then
;                       you are only processing a portion of the scanline
;                       so do not add in the WrapWidths when you are done
;                       processing dwExtX pixels. You just return.
;               dwSrcWrapWidth  This is the number of nibbles (not bytes)
;                       to be added into esi after dwExtX src pixels
;                       (which are nibbles) have been processed to bring
;                       esi to point to the first nibble to be processed
;                       on the following scanline.
;               dwDstWrapWidth  This is the number of bytes to add to edi
;                       after dwExtX dst pixels have been processed to
;                       bring edi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;
; Returns:      None
;               ds:esi  should point to the next src nibble to be processed
;                       NOTE: Once again this should be a nibble address
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     fs,ebx
;==============================================================================
DECPROC GN_Xfer4To32, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
LoopTop:
        ; Remember the dwExtX
        mov     edx,dwExtX

        ; Get a byte address
        shr     esi,1
        jnc     ByteAligned

        mov     al,ds:[esi]
        inc     esi
        and     eax,0FH
        mov     eax,fs:[ebx][eax * 4]
        mov     es:[edi],eax
        add     edi,4
	sub	ecx,ecx
        dec     edx
        je      NextScan

ByteAligned:
        cmp     edx,2
        jl      CheckLastPixel

ChunksOf2:
        mov     al,ds:[esi]
        inc     esi
        mov     ecx,eax
        and     eax,0F0H
	shr	eax,4
        and     ecx,00FH
        mov     eax,fs:[ebx][eax * 4]
        mov     ecx,fs:[ebx][ecx * 4]
        mov     es:[edi+0],eax
        mov     es:[edi+4],ecx
        add     edi,8
        sub     edx,2
        cmp     edx,2
        jge     ChunksOf2

CheckLastPixel:
	sub	ecx,ecx
        dec     edx
        jl	NextScan
        mov     al,ds:[esi]
        and     eax,0F0H
	shr	eax,4
        mov     eax,fs:[ebx][eax * 4]
        mov     es:[edi],eax
        add     edi,4
	inc	ecx

NextScan:
	shl	esi,1
	add	esi,ecx
        cmp     dwExtY,0
        je      @F
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     LoopTop
@@:
CLOSEPROC


;==============================================================================
;
; Function:     GN_Xfer8To32
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 8bpp src bitmap to a 32bpp dst bitmap. The color
;               translate table is indexed with a 8bpp value (a number
;               from 0 through 255 inclusive) scaled up by a factor of 4,
;               and a dword value is pulled out and written to the dst.
;
; Arguments:    ds:esi  ptr to first src pixel -- this is a byte address
;               es:edi  ptr to first dst pixel -- this is a byte address
;               fs:ebx  ptr to color translate table
;               dwExtX  In Pixels
;               dwExtY  In Scanlines -- if this number is 0, then
;                       you are only processing a portion of the scanline
;                       so do not add in the WrapWidths when you are done
;                       processing dwExtX pixels. You just return.
;               dwSrcWrapWidth  This is the number of bytes to add to esi
;                       after dwExtX src pixels have been processed to
;                       bring esi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;               dwDstWrapWidth  This is the number of bytes to add to edi
;                       after dwExtX dst pixels have been processed to
;                       bring edi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;
; Returns:      None
;               ds:esi  should point to the next src pixel to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     fs,ebx
;==============================================================================
DECPROC GN_Xfer8To32, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
LOCALD  dwTempExtX
OPENPROC
LoopTop:
        mov     edx,dwExtX
        mov     dwTempExtX,edx
        cmp     edx,4
        jl      CheckLastPixels

ChunksOf4:
        mov     eax,ds:[esi]
        add     esi,4
        movzx   ecx,al
        movzx   edx,ah
        shr     eax,10H
        mov     ecx,dword ptr fs:[ebx][ecx * 4]
        mov     edx,dword ptr fs:[ebx][edx * 4]
        mov     es:[edi+0],ecx
        mov     es:[edi+4],edx
        movzx   ecx,al
        movzx   edx,ah
        add     edi,8
        mov     ecx,dword ptr fs:[ebx][ecx * 4]
        mov     edx,dword ptr fs:[ebx][edx * 4]
        mov     es:[edi+0],ecx
        mov     es:[edi+4],edx
        add     edi,8
        sub     dwTempExtX,4
        cmp     dwTempExtX,4
        jge     ChunksOf4

CheckLastPixels:
        cmp     dwTempExtX,0
        je      NextScan

OnePixelAtATime:
        movzx   eax,byte ptr ds:[esi]
        inc     esi
        mov     eax,fs:[ebx][eax * 4]
        mov     es:[edi],eax
        add     edi,4
        dec     dwTempExtX
        jne     OnePixelAtATime

NextScan:
        cmp     dwExtY,0
        je      @F
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     LoopTop
@@:
CLOSEPROC


;==============================================================================
;
; Function:     GN_Xfer15To32
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 15bpp src bitmap to a 32bpp dst bitmap. There is no
;               color translate table. Each pixel is converted on the fly.
;
; Arguments:    ds:esi  ptr to first src pixel -- this is a byte address
;               es:edi  ptr to first dst pixel -- this is a byte address
;               fs:ebx  ptr to color translate table
;               dwExtX  In Pixels
;               dwExtY  In Scanlines -- if this number is 0, then
;                       you are only processing a portion of the scanline
;                       so do not add in the WrapWidths when you are done
;                       processing dwExtX pixels. You just return.
;               dwSrcWrapWidth  This is the number of bytes to add to esi
;                       after dwExtX src pixels have been processed to
;                       bring esi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;               dwDstWrapWidth  This is the number of bytes to add to edi
;                       after dwExtX dst pixels have been processed to
;                       bring edi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;
; Returns:      None
;               ds:esi  should point to the next src pixel to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     fs,ebx
;==============================================================================
DECPROC GN_Xfer15To32, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
LOCALD	dwTempExtX
OPENPROC
LoopTop:
        mov     edx,dwExtX
        mov     dwTempExtX,edx
        cmp     edx,4
        jl      CheckLastPixels

ChunksOf4:
        ; First chunk of 2
        movzx   eax,word ptr ds:[esi+0]
        movzx   ebx,word ptr ds:[esi+2]
        add     esi,4

        shrd    ecx,eax,1
        shrd    edx,ebx,1
        sar     ecx,2
        sar     edx,2
        shrd    ecx,eax,5
        shrd    edx,ebx,5
        shr     eax,5
        shr     ebx,5
        shrd    ecx,eax,1
        shrd    edx,ebx,1
        sar     ecx,2
        sar     edx,2
        shrd    ecx,eax,5
        shrd    edx,ebx,5
        shr     eax,5
        shr     ebx,5
        shrd    ecx,eax,1
        shrd    edx,ebx,1
        sar     ecx,2
        sar     edx,2
        shrd    ecx,eax,5 + 8
        shrd    edx,ebx,5 + 8

        mov     es:[edi+0],ecx
        mov     es:[edi+4],edx
        add     edi,8

        ; Do a second chunk of two
        movzx   eax,word ptr ds:[esi+0]
        movzx   ebx,word ptr ds:[esi+2]
        add     esi,4

        shrd    ecx,eax,1
	shrd    edx,ebx,1
        sar     ecx,2
        sar     edx,2
        shrd    ecx,eax,5
        shrd    edx,ebx,5
        shr     eax,5
        shr     ebx,5
        shrd    ecx,eax,1
        shrd    edx,ebx,1
        sar     ecx,2
        sar     edx,2
        shrd    ecx,eax,5
        shrd    edx,ebx,5
        shr     eax,5
        shr     ebx,5
        shrd    ecx,eax,1
        shrd    edx,ebx,1
        sar     ecx,2
        sar     edx,2
        shrd    ecx,eax,5 + 8
        shrd    edx,ebx,5 + 8

        mov     es:[edi+0],ecx
        mov     es:[edi+4],edx
        add     edi,8
        sub     dwTempExtX,4
        cmp     dwTempExtX,4
        jge     ChunksOf4

CheckLastPixels:
        cmp     dwTempExtX,0
        je      NextScan

OnePixelAtATime:
        movzx   eax,word ptr ds:[esi]
        add     esi,2
        shrd    ecx,eax,1
        sar     ecx,2
        shrd    ecx,eax,5
        shr     eax,5
        shrd    ecx,eax,1
        sar     ecx,2
        shrd    ecx,eax,5
        shr     eax,5
        shrd    ecx,eax,1
        sar     ecx,2
        shrd    ecx,eax,5 + 8
        mov     es:[edi],ecx
        add     edi,4
        dec     dwTempExtX
        jne     OnePixelAtATime

NextScan:
        cmp     dwExtY,0
        je      @F
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     LoopTop
@@:
CLOSEPROC


;==============================================================================
;
; Function:     GN_Xfer16To32
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 16bpp src bitmap to a 32bpp dst bitmap. There is no
;               color translate table. Each pixel is converted on the fly.
;
; Arguments:    ds:esi  ptr to first src pixel -- this is a byte address
;               es:edi  ptr to first dst pixel -- this is a byte address
;               fs:ebx  ptr to color translate table
;               dwExtX  In Pixels
;               dwExtY  In Scanlines -- if this number is 0, then
;                       you are only processing a portion of the scanline
;                       so do not add in the WrapWidths when you are done
;                       processing dwExtX pixels. You just return.
;               dwSrcWrapWidth  This is the number of bytes to add to esi
;                       after dwExtX src pixels have been processed to
;                       bring esi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;               dwDstWrapWidth  This is the number of bytes to add to edi
;                       after dwExtX dst pixels have been processed to
;                       bring edi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;
; Returns:      None
;               ds:esi  should point to the next src pixel to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     fs,ebx
;==============================================================================
DECPROC GN_Xfer16To32, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
LOCALD  dwTempExtX
OPENPROC
LoopTop:
        mov     edx,dwExtX
        mov     dwTempExtX,edx
        cmp     edx,4
        jl      CheckLastPixels

ChunksOf4:
        ; First chunk of 2
        movzx   eax,word ptr ds:[esi+0]
        movzx   ebx,word ptr ds:[esi+2]
        add     esi,4

        shrd    ecx,eax,1
                                        shrd    edx,ebx,1
        sar     ecx,2
                                        sar     edx,2
        shrd    ecx,eax,5
                                        shrd    edx,ebx,5
        shr     eax,5
                                        shr     ebx,5
        shrd    ecx,eax,1
                                        shrd    edx,ebx,1
        sar     ecx,1
                                        sar     edx,1
        shrd    ecx,eax,6
                                        shrd    edx,ebx,6
        shr     eax,6
                                        shr     ebx,6
        shrd    ecx,eax,1
                                        shrd    edx,ebx,1
        sar     ecx,2
                                        sar     edx,2
        shrd    ecx,eax,5 + 8
                                        shrd    edx,ebx,5 + 8

        mov     es:[edi+0],ecx
        mov     es:[edi+4],edx
        add     edi,8

        ; Do a seond chunk of two
        movzx   eax,word ptr ds:[esi+0]
        movzx   ebx,word ptr ds:[esi+2]
        add     esi,4

        shrd    ecx,eax,1
                                        shrd    edx,ebx,1
        sar     ecx,2
                                        sar     edx,2
        shrd    ecx,eax,5
                                        shrd    edx,ebx,5
        shr     eax,5
                                        shr     ebx,5
        shrd    ecx,eax,1
                                        shrd    edx,ebx,1
        sar     ecx,1
                                        sar     edx,1
        shrd    ecx,eax,6
                                        shrd    edx,ebx,6
        shr     eax,6
                                        shr     ebx,6
        shrd    ecx,eax,1
                                        shrd    edx,ebx,1
        sar     ecx,2
                                        sar     edx,2
        shrd    ecx,eax,5 + 8
                                        shrd    edx,ebx,5 + 8

        mov     es:[edi+0],ecx
        mov     es:[edi+4],edx
        add     edi,8
        sub     dwTempExtX,4
        cmp     dwTempExtX,4
        jge     ChunksOf4

CheckLastPixels:
        cmp     dwTempExtX,0
        je      NextScan

OnePixelAtATime:
        movzx   eax,word ptr ds:[esi]
        add     esi,2
        shrd    ecx,eax,1
        sar     ecx,2
        shrd    ecx,eax,5
        shr     eax,5
        shrd    ecx,eax,1
        sar     ecx,1
        shrd    ecx,eax,6
        shr     eax,6
        shrd    ecx,eax,1
        sar     ecx,2
        shrd    ecx,eax,5 + 8
        mov     es:[edi],ecx
        add     edi,4
        dec     dwTempExtX
        jne     OnePixelAtATime

NextScan:
        cmp     dwExtY,0
        je      @F
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     LoopTop
@@:
CLOSEPROC


;==============================================================================
;
; Function:     GN_Xfer24To32
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 24bpp src bitmap to a 32bpp dst bitmap. There is no
;               color translate table. Each pixel is converted on the fly.
;
; Arguments:    ds:esi  ptr to first src pixel -- this is a byte address
;               es:edi  ptr to first dst pixel -- this is a byte address
;               fs:ebx  ptr to color translate table
;               dwExtX  In Pixels
;               dwExtY  In Scanlines -- if this number is 0, then
;                       you are only processing a portion of the scanline
;                       so do not add in the WrapWidths when you are done
;                       processing dwExtX pixels. You just return.
;               dwSrcWrapWidth  This is the number of bytes to add to esi
;                       after dwExtX src pixels have been processed to
;                       bring esi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;               dwDstWrapWidth  This is the number of bytes to add to edi
;                       after dwExtX dst pixels have been processed to
;                       bring edi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;
; Returns:      None
;               ds:esi  should point to the next src pixel to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     fs,ebx
;==============================================================================
DECPROC GN_Xfer24To32, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
LoopTop:
        mov     edx,dwExtX
        cmp     edx,4
        jl      CheckLastPixels

ChunksOf4:
        mov     eax,ds:[esi+0]
        mov     ecx,ds:[esi+3]
        add     esi,6
        mov     es:[edi+0],eax
        mov     es:[edi+4],ecx
        add     edi,8
        mov     eax,ds:[esi+0]
        mov     ecx,ds:[esi+2]
        add     esi,6
        shr     ecx,8
        mov     es:[edi+0],eax
        mov     es:[edi+4],ecx
        add     edi,8
        sub     edx,4
        cmp     edx,4
        jge     ChunksOf4

CheckLastPixels:
        or      edx,edx
        je      NextScan

OnePixelAtATime:
        mov     al,ds:[esi+2]
        shl     eax,10H
        mov     ax,ds:[esi+0]
        add     esi,3
        mov     es:[edi],eax
        add     edi,4
        dec     edx
        jne     OnePixelAtATime

NextScan:
        cmp     dwExtY,0
        je      @F
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     LoopTop
@@:
CLOSEPROC


;==============================================================================
;
; Function:     GN_Xfer32To32
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 32bpp src bitmap to a 32bpp dst bitmap. There is no
;               color translate table. In fast, there is no conversion
;               period. Just copy the src to the dst.
;
; Arguments:    ds:esi  ptr to first src pixel -- this is a byte address
;               es:edi  ptr to first dst pixel -- this is a byte address
;               fs:ebx  ptr to color translate table
;               dwExtX  In Pixels
;               dwExtY  In Scanlines -- if this number is 0, then
;                       you are only processing a portion of the scanline
;                       so do not add in the WrapWidths when you are done
;                       processing dwExtX pixels. You just return.
;               dwSrcWrapWidth  This is the number of bytes to add to esi
;                       after dwExtX src pixels have been processed to
;                       bring esi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;               dwDstWrapWidth  This is the number of bytes to add to edi
;                       after dwExtX dst pixels have been processed to
;                       bring edi to point to the first pixel to be
;                       processed on the following scanline. This is all
;                       in easy byte addresses.
;
; Returns:      None
;               ds:esi  should point to the next src pixel to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     fs,ebx
;==============================================================================
DECPROC GN_Xfer32To32, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
@@:
        mov     ecx,dwExtX
        rep     movs dword ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      @F
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     @B
@@:
CLOSEPROC

CLOSESEG _NVTEXT32

END

