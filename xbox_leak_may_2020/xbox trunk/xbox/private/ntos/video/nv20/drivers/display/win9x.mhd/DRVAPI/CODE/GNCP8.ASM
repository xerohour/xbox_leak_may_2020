;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gncp8.asm
;
; Purpose:      This file has functions to copy X-bpp bitmaps to
;               16bpp bitmaps.
;
;==============================================================================
.586
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
; Function:     GN_Xfer4To8WordXlat
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 4bpp src bitmap to a 8bpp dst bitmap. The color
;               translate table is indexed with a 4bpp value (a number
;               from 0 through 15 inclusive) scaled up by a factor of 2,
;               and a word 16bpp value is pulled out of the table. The
;               low byte of this word should be used as the dst pixel.
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
DECPROC GN_Xfer4To8WordXlat, PASCAL, FRAME, NEAR
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
        mov     cl,fs:[ebx][eax * 2]
        mov     es:[edi],cl
        inc     edi
        sub     ecx,ecx
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
        shr     eax,4
        and     ecx,00FH
        mov     al,fs:[ebx][eax * 2]
        mov     cl,fs:[ebx][ecx * 2]
        mov     es:[edi+0],al
        mov     es:[edi+1],cl
        add     edi,2
        sub     edx,2
        cmp     edx,2
        jge     ChunksOf2

CheckLastPixel:
        sub     ecx,ecx
        dec     edx
        jl      NextScan
        mov     al,ds:[esi]
        and     eax,0F0H
        shr     eax,4
        mov     al,fs:[ebx][eax * 2]
        mov     es:[edi],al
        inc     edi
        inc     ecx

NextScan:
        shl     esi,1
        add     esi,ecx
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
; Function:     GN_Xfer8To8NoXlat
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 8bpp src bitmap to a 8bpp dst bitmap. There is no
;               color translate table needed. Just copy the src to the
;               dst.
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
;               ds:esi  should point to the next src nibble to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_Xfer8To8NoXlat, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
        ; Are there just a few bytes to do?
        mov     eax,esi
        neg     eax
        and     eax,3
        mov     edx,dwExtX
        sub     edx,eax
        jg      @F
        mov     eax,dwExtX
        sub     ebx,ebx
        jmp     L0_M0_RX
@@:
        ; We can
        sub     ecx,ecx
        mov     ebx,edx
        shr     edx,2
        and     ebx,3
        or      eax,eax
        setne   cl
        cmp     edx,1
        adc     ecx,ecx
        cmp     ebx,1
        adc     ecx,ecx
        xor     ecx,3
        jmp     NEAR PTR cs:CopyJmpTable[ecx * 4]


CopyJmpTable    LABEL   DWORD
DD      OFFSET L0_M0_R0,OFFSET L0_M0_RX,OFFSET L0_MX_R0,OFFSET L0_MX_RX
DD      OFFSET LX_M0_R0,OFFSET LX_M0_RX,OFFSET LX_MX_R0,OFFSET LX_MX_RX

LX_MX_RX:
        mov     ecx,eax
        rep     movs byte ptr es:[edi], ds:[esi]
        mov     ecx,edx
        rep     movs dword ptr es:[edi], ds:[esi]
        mov     ecx,ebx
        rep     movs byte ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      Done
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     LX_MX_RX
        jmp     Done

LX_MX_R0:
        mov     ecx,eax
        rep     movs byte ptr es:[edi], ds:[esi]
        mov     ecx,edx
        rep     movs dword ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      Done
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     LX_MX_R0
        jmp     Done

L0_MX_RX:
        mov     ecx,edx
        rep     movs dword ptr es:[edi], ds:[esi]
        mov     ecx,ebx
        rep     movs byte ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      Done
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     L0_MX_RX
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

LX_M0_RX:
LX_M0_R0:
L0_M0_RX:
        add     eax,ebx
@@:     mov     ecx,eax
        rep     movs byte ptr es:[edi], ds:[esi]
        cmp     dwExtY,0
        je      Done
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     @B
        jmp     Done

L0_M0_R0:
Done:
CLOSEPROC


;==============================================================================
;
; Function:     GN_Xfer8To8ByteXlat
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 8bpp src bitmap to a 8bpp dst bitmap. The color
;               translate table is indexed with a 8bpp value (a number
;               from 0 through 255 inclusive) scaled up by a factor of 1,
;               and a byte value is pulled out and written to the dst.
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
;               ds:esi  should point to the next src nibble to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     fs,ebx
;==============================================================================
DECPROC GN_Xfer8To8ByteXlat, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
LoopTop:
        mov     edx,dwExtX
        sub     ecx,ecx
        cmp     edx,4
        jl      CheckLastPixels

ChunksOf4:
        mov     eax,ds:[esi]
        add     esi,4
        mov     cl,al
        mov     al,fs:[ebx + ecx]
        mov     cl,ah
        mov     ah,fs:[ebx + ecx]
        rol     eax,10H
        mov     cl,al
        mov     al,fs:[ebx + ecx]
        mov     cl,ah
        mov     ah,fs:[ebx + ecx]
        rol     eax,10H
        mov     es:[edi],eax
        sub     edx,4
        add     edi,4
        cmp     edx,4
        jge     ChunksOf4

CheckLastPixels:
        or      edx,edx
        je      NextScan

OnePixelAtATime:
        mov     cl,ds:[esi]
        inc     esi
        mov     al,fs:[ebx + ecx]
        mov     es:[edi],al
        inc     edi
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
; Function:     GN_Xfer8To8WordXlat
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 8bpp src bitmap to a 8bpp dst bitmap. The color
;               translate table is indexed with a 8bpp value (a number
;               from 0 through 255 inclusive) scaled up by a factor of 2,
;               and a word value is pulled out. The low byte of this word
;               is written to the dst.
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
;               ds:esi  should point to the next src nibble to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     fs,ebx
;==============================================================================
DECPROC GN_Xfer8To8WordXlat, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
LOCALD  dwBytesRest
OPENPROC
LoopTop:
        mov     edx,dwExtX
        sub     ecx,ecx
        cmp     edx,4
        jl      CheckLastPixels

ChunksOf4:
        mov     eax,ds:[esi]
        add     esi,4
        mov     cl,al
        mov     al,fs:[ebx + ecx * 2]
        mov     cl,ah
        mov     ah,fs:[ebx + ecx * 2]
        rol     eax,10H
        mov     cl,al
        mov     al,fs:[ebx + ecx * 2]
        mov     cl,ah
        mov     ah,fs:[ebx + ecx * 2]
        rol     eax,10H
        mov     es:[edi],eax
        sub     edx,4
        add     edi,4
        cmp     edx,4
        jge     ChunksOf4

CheckLastPixels:
        or      edx,edx
        je      NextScan

OnePixelAtATime:
        mov     cl,ds:[esi]
        inc     esi
        mov     al,fs:[ebx + ecx * 2]
        mov     es:[edi],al
        inc     edi
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
; Function:     GN_Xfer15To8
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 15bpp src bitmap to a 8bpp dst bitmap.
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
;               ds:esi  should point to the next src nibble to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_Xfer15To8, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
LoopY:
        mov     ecx,dwExtX
LoopX:
        mov     dx,ds:[esi]
        and     edx,7FFFH
        add     esi,2
        mov     al,fs:[ebx + edx]
        mov     es:[edi],al
        inc     edi
        dec     ecx
        jne     LoopX

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
; Function:     GN_Xfer16To8
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 16bpp src bitmap to a 8bpp dst bitmap.
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
;               ds:esi  should point to the next src nibble to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_Xfer16To8, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
LoopY:
        mov     ecx,dwExtX
        sub     eax,eax
LoopX:
        mov     ax,ds:[esi]
        ror     eax,5
        shr     ax,1
        rol     eax,5
        add     esi,2
        mov     al,fs:[ebx + eax]
        mov     es:[edi],al
        inc     edi
        dec     ecx
        jne     LoopX

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
; Function:     GN_Xfer24To8
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 24bpp src bitmap to a 8bpp dst bitmap.
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
;               ds:esi  should point to the next src nibble to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_Xfer24To8, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
LoopY:
        mov     ecx,dwExtX
LoopX:
        mov     al,ds:[esi]
        shr     al,3
        shrd    edx,eax,5
        mov     al,ds:[esi + 1]
        shr     al,3
        shrd    edx,eax,5
        xor     ah,ah
        mov     al,ds:[esi + 2]
        shr     al,3
        shrd    edx,eax,16H
        add     esi,3
        mov     al,fs:[ebx + edx]
        mov     es:[edi],al
        inc     edi
        dec     ecx
        jne     LoopX

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
; Function:     GN_Xfer32To8
;
; Purpose:      This function can be used as the inner loop for copies
;               of a 32bpp src bitmap to a 8bpp dst bitmap.
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
;               ds:esi  should point to the next src nibble to be processed
;               es:edi  should point to the next dst pixel to be processed
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_Xfer32To8, PASCAL, FRAME, NEAR
PARMD   dwExtX
PARMD   dwExtY
PARMD   dwSrcWrapWidth
PARMD   dwDstWrapWidth
OPENPROC
LoopY:
        mov     ecx,dwExtX
LoopX:
        mov     al,ds:[esi]
        shr     al,3
        shrd    edx,eax,5
        mov     al,ds:[esi + 1]
        shr     al,3
        shrd    edx,eax,5
        xor     ah,ah
        mov     al,ds:[esi + 2]
        shr     al,3
        shrd    edx,eax,16H
        add     esi,4
        mov     al,fs:[ebx + edx]
        mov     es:[edi],al
        inc     edi
        dec     ecx
        jne     LoopX

        cmp     dwExtY,0
        je      @F
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwExtY
        jne     LoopY
@@:
CLOSEPROC

CLOSESEG _NVTEXT32

END

