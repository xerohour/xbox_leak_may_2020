;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gncp15.asm
;
; Purpose:      This file has inner loop routines in it to copy 4, 8,
;               15, 16, 24, or 32bpp bitmaps to a 15bpp dst bitmap.
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

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GN_Xfer4To15WordXlat
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 4bpp src bitmap to a 15bpp dst bitmap. The color
;               translate table is indexed with a 4bpp value (a number
;               from 0 through 15 inclusive) scaled up by a factor of 2,
;               and a word 15bpp value is pulled out of the table and
;               used as the dst pixel.
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
DECPROC GN_Xfer4To15WordXlat, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
LoopY:
        ; Remember the dwExtX
        mov     edx,dwExtX

        ; Get a byte address
        shr     esi,1
        jnc     ByteAligned

        movzx   eax,byte ptr ds:[esi]
        inc     esi
        and     eax,0FH
        mov     ax,fs:[ebx][eax * 2]
        mov     es:[edi],ax
        add     edi,2
	sub	ecx,ecx
        dec     edx
        je      NextScan

ByteAligned:
        cmp     edx,2
        jl      CheckLastPixel

ChunksOf2:
        movzx   eax,byte ptr ds:[esi]
        inc     esi
        mov     ecx,eax
        and     eax,0F0H
	shr	eax,4
        and     ecx,00FH
        mov     ax,fs:[ebx][eax * 2]
        mov     cx,fs:[ebx][ecx * 2]
        mov     es:[edi+0],ax
        mov     es:[edi+2],cx
        add     edi,4
        sub     edx,2
        cmp     edx,2
        jge     ChunksOf2

CheckLastPixel:
	sub	ecx,ecx
        dec     edx
        jl	NextScan
        movzx   eax,byte ptr ds:[esi]
        and     eax,0F0H
	shr	eax,4
        mov     ax,fs:[ebx][eax * 2]
        mov     es:[edi],ax
        add     edi,2
	inc	ecx

NextScan:
	shl	esi,1
	add	esi,ecx
        cmp     dwExtY,0
        je      @F
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     LoopY
@@:
CLOSEPROC


;==============================================================================
;
; Function:     GN_Xfer4To15DwordXlat
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 4bpp src bitmap to a 15bpp dst bitmap. The color
;               translate table is indexed with a 4bpp value (a number
;               from 0 through 15 inclusive) scaled up by a factor of 4,
;               and a dword 32bpp RGB value is pulled out of the table.
;               Then the 32bpp dword value is down converted to 15bpp
;               on the fly ad the resulting value written to the dst.
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
DECPROC GN_Xfer4To15DwordXlat, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
LoopY:
        ; Remember the dwExtX
        mov     edx,dwExtX

        ; Get a byte address
        shr     esi,1
        jnc     ByteAligned

        mov     al,ds:[esi]
        inc     esi
        and     eax,0FH
        mov     eax,fs:[ebx][eax * 4]
        shr     eax,3
        ror     eax,5
        shr     ax,3
        ror     eax,5
        shr     ax,3
        rol     eax,10
        mov     es:[edi],ax
        add     edi,2
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
        and     ecx,00FH
	shr	eax,4
        mov     eax,fs:[ebx][eax * 4]
        mov     ecx,fs:[ebx][ecx * 4]
        shr     eax,3
        shr     ecx,3
        ror     eax,5
        ror     ecx,5
        shr     ax,3
        shr     cx,3
        ror     eax,5
        ror     ecx,5
        shr     ax,3
        shr     cx,3
        rol     eax,10
        rol     ecx,10
        mov     es:[edi+0],ax
        mov     es:[edi+2],cx
        add     edi,4
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
        shr     eax,3
        ror     eax,5
        shr     ax,3
        ror     eax,5
        shr     ax,3
        rol     eax,10
        mov     es:[edi],ax
        add     edi,2
	inc	ecx

NextScan:
	shl	esi,1
	add	esi,ecx
        cmp     dwExtY,0
        je      @F
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     LoopY
@@:
CLOSEPROC


;==============================================================================
;
; Function:     GN_Xfer8To15WordXlat
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 8bpp src bitmap to a 15bpp dst bitmap. The color
;               translate table is indexed with a 8bpp value (a number
;               from 0 through 255 inclusive) scaled up by a factor of 2,
;               and a word value is pulled out and written to the dst.
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
DECPROC GN_Xfer8To15WordXlat, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
LOCALD  dwTempExtX
OPENPROC
LoopTop:
        mov     eax,dwExtX
        mov     dwTempExtX,eax
        cmp     eax,4
        jl      CheckLastPixels

ChunksOf4:
        mov     eax,ds:[esi]
        add     esi,4
        movzx   ecx,ah
        mov     edx,dword ptr fs:[ebx][ecx * 2 - 2]
        movzx   ecx,al
        shr     eax,10H
        mov     dx,fs:[ebx][ecx * 2]
        mov     es:[edi],edx
        add     edi,4
        movzx   ecx,ah
        mov     edx,dword ptr fs:[ebx][ecx * 2 - 2]
        movzx   ecx,al
        mov     dx,fs:[ebx][ecx * 2]
        mov     es:[edi],edx
        add     edi,4
        sub     dwTempExtX,4
        cmp     dwTempExtX,4
        jge     ChunksOf4

CheckLastPixels:
        cmp     dwTempExtX,0
        je      NextScan

OnePixelAtATime:
        movzx   eax,byte ptr ds:[esi]
        inc     esi
        mov     cx,fs:[ebx][eax * 2]
        mov     es:[edi],cx
        add     edi,2
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
; Function:     GN_Xfer8To15DwordXlat
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 8bpp src bitmap to a 15bpp dst bitmap. The color
;               translate table is indexed with a 8bpp value (a number
;               from 0 through 255 inclusive) scaled up by a factor of 4,
;               and a dword 32bpp RGB value is pulled out of the table.
;               Then the 32bpp dword value is down converted to 15bpp
;               on the fly ad the resulting value written to the dst.
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
DECPROC GN_Xfer8To15DwordXlat, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
LOCALD  dwTempExtX
OPENPROC
LoopTop:
        mov     eax,dwExtX
        mov     dwTempExtX,eax
        cmp     eax,2
        jl      CheckLastPixels

ChunksOf2:
        movzx   eax,word ptr ds:[esi]
        add     esi,4
        movzx   ecx,al
        movzx   edx,ah
        mov     ecx,dword ptr fs:[ebx][ecx * 4]
        mov     edx,dword ptr fs:[ebx][edx * 4]
        shr     ecx,3
        shr     edx,3
        ror     ecx,5
        ror     edx,5
        shr     cx,3
        shr     dx,3
        ror     ecx,5
        ror     edx,5
        shr     cx,3
        shr     dx,3
        rol     ecx,10
        rol     edx,10
        mov     es:[edi+0],cx
        mov     es:[edi+2],dx
        add     edi,4
        sub     dwTempExtX,2
        cmp     dwTempExtX,2
        jge     ChunksOf2

CheckLastPixels:
        cmp     dwTempExtX,0
        je      NextScan

OnePixelAtATime:
        movzx   eax,byte ptr ds:[esi]
        inc     esi
        mov     ecx,fs:[ebx][eax * 4]
        shr     ecx,3
        ror     ecx,5
        shr     cx,3
        ror     ecx,5
        shr     cx,3
        rol     ecx,10
        mov     es:[edi],cx
        add     edi,2
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
; Function:     GN_Xfer15To15
;
; Purpose:      This function can be used as the inner loop for copies
;               from a 15bpp bitmap to a 15bpp bitmap. No Color translate
;               table is needed. Each pixel must be converted on the fly.
;
; Arguments:    ds:esi  ptr to first src pixel -- this is a byte address
;               es:edi  ptr to first dst pixel -- this is a byte address
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
; Preserve:     Nothing
;==============================================================================
DECPROC GN_Xfer15To15, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
        ; Let's compute the optimal inner loop. Basically, there
        ; is either 0 or 1 word to get dword aligned (a left edge),
        ; then 0 or more dwords (middle), and finally there may be
        ; an extra word on the end (right edge).
        mov     edx,dwExtX
        add     edx,edx
        mov     eax,esi
        and     eax,2
        sub     edx,eax
        mov     ecx,edx
        shr     eax,1
        cmp     edx,3
        adc     eax,eax
        and     ecx,2
        cmp     ecx,1
        adc     eax,eax
        xor     eax,3
        shr     edx,2
        jmp     NEAR PTR cs:JmpTable[eax * 4]

JmpTable:
DD      OFFSET L0_M0_R0,OFFSET L0_M0_R1,OFFSET L0_MX_R0,OFFSET L0_MX_R1
DD      OFFSET L1_M0_R0,OFFSET L1_M0_R1,OFFSET L1_MX_R0,OFFSET L1_MX_R1

L1_MX_R1:
        movs    word ptr es:[edi], ds:[esi]
        mov     ecx,edx
        rep     movs dword ptr es:[edi], ds:[esi]
        movs    word ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      Done
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     L1_MX_R1
        jmp     Done

L1_MX_R0:
        movs    word ptr es:[edi], ds:[esi]
        mov     ecx,edx
        rep     movs dword ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      Done
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     L1_MX_R0
        jmp     Done

L1_M0_R1:
        movs    dword ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      Done
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     L1_M0_R1
        jmp     Done

L0_MX_R1:
        mov     ecx,edx
        rep     movs dword ptr es:[edi], ds:[esi]
        movs    word ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      Done
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     L0_MX_R1
        jmp     Done

L0_MX_R0:
        mov     ecx,edx
        rep     movs dword ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      Done
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     L0_MX_R0
        jmp     Done

L1_M0_R0:
L0_M0_R1:
        movs    word ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      Done
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     L0_M0_R1

L0_M0_R0:
Done:
CLOSEPROC


;==============================================================================
;
; Function:     GN_Xfer16To15
;
; Purpose:      This function can be used as the inner loop for copies
;               from a 16bpp bitmap to a 15bpp bitmap. No Color translate
;               table is needed. Each pixel must be converted on the fly.
;
; Arguments:    ds:esi  ptr to first src pixel -- this is a byte address
;               es:edi  ptr to first dst pixel -- this is a byte address
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
; Preserve:     Nothing
;==============================================================================
DECPROC GN_Xfer16To15, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
LoopTop:
        mov     edx,dwExtX
        cmp     edx,2
        jl      CheckLastPixel

ChunksOf2:
        mov     ax,ds:[esi+0]
        mov     cx,ds:[esi+2]
        add     esi,4
        ror     eax,5
        ror     ecx,5
        shr     ax,1
        shr     cx,1
        rol     eax,5
        rol     ecx,5
        mov     es:[edi+0],ax
        mov     es:[edi+2],cx
        add     edi,4
        sub     edx,2
        cmp     edx,2
        jge     ChunksOf2

CheckLastPixel:
        or      edx,edx
        je      NextScan

OnePixelAtATime:
        mov     ax,ds:[esi]
        add     esi,2
        ror     eax,5
        shr     ax,1
        rol     eax,5
        mov     es:[edi],ax
        add     edi,2

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
; Function:     GN_Xfer24To15
;
; Purpose:      This function can be used as the inner loop for copies
;               from a 24bpp bitmap to a 15bpp bitmap. No Color translate
;               table is needed. Each pixel must be converted on the fly.
;
; Arguments:    ds:esi  ptr to first src pixel -- this is a byte address
;               es:edi  ptr to first dst pixel -- this is a byte address
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
; Preserve:     Nothing
;==============================================================================
DECPROC GN_Xfer24To15, PASCAL, FRAME, NEAR
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
        shr     eax,3
        shr     ecx,3
        ror     eax,5
        ror     ecx,5
        shr     ax,3
        shr     cx,3
        ror     eax,5
        ror     ecx,5
        shr     ax,3
        shr     cx,3
        rol     eax,10
        rol     ecx,10
        mov     es:[edi+0],ax
        mov     es:[edi+2],cx
        add     edi,4

        mov     eax,ds:[esi+0]
        mov     cl,ds:[esi+5]
        shl     ecx,10H
        mov     cx,ds:[esi+3]
        add     esi,6
        shr     eax,3
        shr     ecx,3
        ror     eax,5
        ror     ecx,5
        shr     ax,3
        shr     cx,3
        ror     eax,5
        ror     ecx,5
        shr     ax,3
        shr     cx,3
        rol     eax,10
        rol     ecx,10
        mov     es:[edi+0],ax
        mov     es:[edi+2],cx
        add     edi,4
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
        shr     eax,3
        ror     eax,5
        shr     ax,3
        ror     eax,5
        shr     ax,3
        rol     eax,10
        mov     es:[edi],ax
        add     edi,2
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
; Function:     GN_Xfer32To15
;
; Purpose:      This function can be used as the inner loop for copies
;               from a 32bpp bitmap to a 15bpp bitmap. No Color translate
;               table is needed. Each pixel must be converted on the fly.
;
; Arguments:    ds:esi  ptr to first src pixel -- this is a byte address
;               es:edi  ptr to first dst pixel -- this is a byte address
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
; Preserve:     Nothing
;==============================================================================
DECPROC GN_Xfer32To15, PASCAL, FRAME, NEAR
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
        mov     ecx,ds:[esi+4]
        add     esi,8
        shr     eax,3
        shr     ecx,3
        ror     eax,5
        ror     ecx,5
        shr     ax,3
        shr     cx,3
        ror     eax,5
        ror     ecx,5
        shr     ax,3
        shr     cx,3
        rol     eax,10
        rol     ecx,10
        mov     es:[edi+0],ax
        mov     es:[edi+2],cx
        add     edi,4

        mov     eax,ds:[esi+0]
        mov     ecx,ds:[esi+4]
        add     esi,8
        shr     eax,3
        shr     ecx,3
        ror     eax,5
        ror     ecx,5
        shr     ax,3
        shr     cx,3
        ror     eax,5
        ror     ecx,5
        shr     ax,3
        shr     cx,3
        rol     eax,10
        rol     ecx,10
        mov     es:[edi+0],ax
        mov     es:[edi+2],cx
        add     edi,4
        sub     edx,4
        cmp     edx,4
        jge     ChunksOf4

CheckLastPixels:
        or      edx,edx
        je      NextScan

OnePixelAtATime:
        mov     eax,ds:[esi]
        add     esi,4
        shr     eax,3
        ror     eax,5
        shr     ax,3
        ror     eax,5
        shr     ax,3
        rol     eax,10
        mov     es:[edi],ax
        add     edi,2
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

CLOSESEG _NVTEXT32

END

