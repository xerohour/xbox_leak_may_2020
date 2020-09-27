;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation
;
; File:         gnsdib.asm
;
; Purpose:      This file holds the StretchDIBits display driver entry point.
;
;==============================================================================
.586
incDevice = 1
incDrawMode = 1
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
; Function:     GENERIC_StretchDIBits
;
; Purpose:      This function is the StretchDIBits display driver entry point.
;
; Arguments:    StretchDIBits Display Driver Stack Frame
;
; Returns:      ax      number of scans copied if successful
;                       0 if not successful
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_StretchDIBits, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   fGet
PARMW   wDstX
PARMW   wDstY
PARMW   wDstExtX
PARMW   wDstExtY
PARMW   wSrcX
PARMW   wSrcY
PARMW   wSrcExtX
PARMW   wSrcExtY
PARMD   lpDIBBits
PARMD   lpDIBInfo
PARMD   lpTranslate
PARMD   dwRop3
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   lpClipRect
LOCALD  dwBmpLeft
LOCALD  dwBmpTop
LOCALD  dwBmpRight
LOCALD  dwBmpBottom
LOCALD  dwDibLeft
LOCALD  dwDibTop
LOCALD  dwDibRight
LOCALD  dwDibBottom
LOCALD  dwDibBitsOffset
LOCALD  dwExtX
LOCALD  dwExtY
LOCALD  dwDibPitch
LOCALD  dwDibWidth
LOCALD  dwDibHeight
LOCALD  dwDstBitsPixel
LOCALD  lpPhysColor
LOCALD  dwPhysColor
LOCALD  dwScaleX
LOCALD  dwScaleY
LOCALD  dwDstExtX
LOCALD  dwDstExtY
LOCALD  dwStretchFlag
OPENPROC
        PUSHR   ds,esi,edi

        ; Load up the DstDev and the DIB Header for dispatching
        sub     eax,eax
        cmp     ax,fGet
        jne     GN_StrDibPunt

        ; Load up the ptrs
        sub     edi,edi
        sub     esi,esi
        lfs     di,lpDIBInfo
        les     si,lpDstDev

        ; We only do RGB DIBs (No Compression)
        cmp     fs:[edi].BitmapInfoHeader.biCompression,BI_RGB
        je      @F
        
        cmp     fs:[edi].BitmapInfoHeader.biCompression,BI_BITFIELDS
        jne     GN_StrDibPunt

@@:
        ; If the bitmap is not in VRAM, then punt it.
        cmp     ax,es:[esi].DIBENGINE.deType
        je      GN_StrDibPunt
        test    es:[esi].DIBENGINE.deFlags,VRAM
        jz      GN_StrDibPunt

        ; Make sure the device is not busy.
        mov     ds,cs:[wFlatDataSel]
        mov     ebx,cs:[CSpGNLogicalDevice]
        cmp     ds:[ebx].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_StrDibPunt

        movsx   eax,wSrcExtX

        ; Punt the negative width (Left-Right mirror Arabic Windows)
        or      eax,eax
        js      GN_StrDibPunt

        movsx   ebx,wDstExtX
        movsx   ecx,wSrcExtY
        movsx   edx,wDstExtY

        cmp     eax,ebx
        mov     eax,1
        jne     @F
        cmp     ecx,edx
        jne     @F
        sub     eax,eax

@@:
        mov     dwStretchFlag,eax

        ; Punt the negative heights
        or      ecx,edx
        js      GN_StrDibPunt

        ; Punt stretched case for the mono dib
        cmp     fs:[edi].BitmapInfoHeader.biBitCount,ax
        je      GN_StrDibPunt

IF DIB_PROCESS_SINGLESCAN
        ; Special case the 8bpp DIBS that are 1 scanline high,
        ; where wSrcExtY = wDstExtY = 1
        cmp     dwStretchFlag,0
        jne     @F
        cmp     fs:[edi].BitmapInfoHeader.biBitCount,8
        jne     @F
        cmp     fs:[edi].BitmapInfoHeader.biHeight,1
        jne     @F
        cmp     ecx,1
        jne     @F

        cmp     byte ptr dwRop3+2,0CCH
        jne     @F
        call    GN_StretchDIBitsSingleScan
        or      eax,eax
        jne     GN_StrDibExit
@@:
ENDIF

IF      (DIB_8BPP_ACCEL NE 0)
        ; If the dst is 8bpp and Rop not srccopy or src is mono, then punt it.
        cmp     es:[esi].DIBENGINE.deBitsPixel,8
        jne     @F
        cmp     byte ptr dwRop3+2,0CCH
        jne     GN_StrDib8BppPunt
        cmp     fs:[edi].BitmapInfoHeader.biBitCount,1
        je      GN_StrDib8BppPunt
@@:
ELSE
        ; If the dst is 8bpp, but not SingleScan case, then punt it.
        cmp     es:[esi].DIBENGINE.deBitsPixel,8
        je      GN_StrDib8BppPunt
ENDIF

        ; First, clip the DIB
        call    GN_StretchDIBitsClip
        or      eax,eax
        je      GN_StrDibExit

        ; If the rop involves a pattern, prepare the pattern now
        movzx   eax,byte ptr [dwRop3 + 2]
        movzx   ecx,byte ptr cs:Rop3Table[eax]
        cmp     ecx,PAD
        je      PrepBrush
        cmp     ecx,PSD
        je      PrepBrush

BackIn:
        ; Break the call out into cases.
        movzx   esi,word ptr lpDstDev+0
        cmp     fs:[edi].BitmapInfoHeader.biBitCount,1
        jne     GN_StretchColorDib
        jmp     GN_StretchMonoDib

PrepBrush:
        mov     es,word ptr lpBrush+2
        movzx   esi,word ptr lpBrush+0
        push    lpDrawMode
        mov     ebx,cs:[CSpGNLogicalDevice]
        call    GN_PrepareBrush
        or      eax,eax
        je      GN_StrDibExit
        mov     eax,0
        jl      GN_StrDibExit

        ; reload some stuff that got trashed
        ; Load up the ptrs
        sub     edi,edi
        sub     esi,esi
        lfs     di,lpDIBInfo
        les     si,lpDstDev
        jmp     BackIn

GENERIC_StretchDIBits   ENDP


;==============================================================================
;
; Function:     StretchDIBits ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of StretchDIBits. You can either return a success code,
;               a failure code, punt to the DIBENG, etc.
;
; Arguments:    StretchDIBits Display Driver Stack Frame
;
;==============================================================================
PUBLIC  GN_StrDibPunt
GN_StrDibPunt     PROC    NEAR

        push    lpDstDev
        push    fGet
        push    wDstX
        push    wDstY
        push    wDstExtX
        push    wDstExtY
        push    wSrcX
        push    wSrcY
        push    wSrcExtX
        push    wSrcExtY
        push    lpDIBBits
        push    lpDIBInfo
        push    lpTranslate
        push    dwRop3
        push    lpBrush
        push    lpDrawMode
        push    lpClipRect
PLABEL  Punt_StrDib
        call    DIB_StretchDIBits
        .errnz  $ - OFFSET GN_StrDibExit

GN_StrDibPunt     ENDP

PUBLIC  GN_StrDibExit
GN_StrDibExit   PROC    NEAR

        POPR    ds,esi,edi
        mov     sp,bp
        mov     ecx,ss:[ebp + 4]
        pop     ebp
IF DIB_FAST_EXIT
        cmp     ax,-1
        je      @F
        cmp     cs:[bUseStretchDIBitsFastExit],0
        je      @F
        cmp     ecx,cs:[lpfnGdiCode03_20B7]
        jne     @F
        mov     ss:[bp - 02H],ax
        test    byte ptr ss:[di + 02H],40H
        jz      @F
        add     sp,4 + 32H
        jmp     GdiCode03_217C
@@:
ENDIF
        ; Clean and return normally
        RETFAR16 32H
GN_StrDibExit   ENDP


;==============================================================================
;
; Function:     GN_StrDib8BppPunt (StretchDIBits ending routine)
;
; Purpose:      This function is a work-around for a bug in the
;               DIBEngine DIB compiler.  At 8bpp, if the destination
;               bitmap is not word aligned (i.e. (wDestX & 1) == 1)
;               AND the clip rect is one (1) pixel wide AND the clip
;               rect is misaligned by 3 (i.e. (wClipLeft & 3) == 3),
;               AND the stretch is 1:1, then the DIB compiler will
;               fail to compile the code to do the pixel copy
;               (tested in the 16bpp source case) but will compile
;               the epilogue code incrementing the source and dest
;               pointers furiously.
;               The result of this is somewhat unsatisfying.
;
; Arguments:    StretchDIBits Display Driver Stack Frame
;
;==============================================================================
DECPROC GN_StrDib8BppPunt, PASCAL, NOFRAME, NEAR
OPENPROC
        test    wDstX,1                 ; Check for unaligned destination
        jz      GN_StrDibPunt
        mov     ax,wDstExtX             ; Check for 1:1 stretch
        mov     bx,wDstExtY
        cmp     ax,wSrcExtX
        jne     GN_StrDibPunt
        cmp     bx,wSrcExtY
        jne     GN_StrDibPunt
        xor     esi,esi
        cmp     word ptr [lpClipRect + 2],si   ; Check for clip rect
        je      GN_StrDibPunt
        lfs     si,lpClipRect

        movzx   eax,fs:[esi].RECT.left
        inc     eax
        cmp     ax,fs:[esi].RECT.right  ; Check if 1 pixel wide
        jne     GN_StrDibPunt
        and     eax,3                   ; Check for (clipleft mod 4) == 3
        jnz     GN_StrDibPunt

        ; Clean and return a failure normally
        POPR    ds,esi,edi
        mov     sp,bp
        mov     eax,-1
        pop     ebp
        RETFAR16 32H
ENDPROC


;==============================================================================
;
; Function:     GN_StretchMonoDib
;
; Purpose:      This function is jumped to from the StretchDIBits
;               display driver entry point. We have already clipped
;               the DIB/Bitmap, made the determination that we will
;               handle the call and locked the display. Here, the
;               parameters should be pushed on the stack to call the
;               HW specific routine to perform the mono transfer.
;
; Arguments:    StretchDIBits Display Driver Stack Frame
;               ds      Context Selector
;               fs:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;               es:esi  lpDstDev ptr  (ptr to DIBENGINE HDR)
;
; Returns:      No returns -- this function is jumped to. When it is done it
;               should jump to one of the StretchDIBits ending routines.
;
;==============================================================================
DECPROC GN_StretchMonoDib, PASCAL, NOFRAME, NEAR
OPENPROC
        ; We can handle this as a mono transfer
        push    cs:[CSpGNLogicalDevice]
        movzx   eax,word ptr es:[esi].DIBENGINE.deBits+4
        push    eax
        push    dword ptr es:[esi].DIBENGINE.deBits
        movzx   eax,es:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    es:[esi].DIBENGINE.deDeltaScan
        push    dwBmpLeft
        push    dwBmpTop
        movzx   eax,word ptr lpDIBBits+2
        push    eax
        push    dwDibBitsOffset
        push    dwDibPitch
        push    dwDibLeft
        push    dwDibTop
        push    dwExtX
        push    dwExtY

        ; The bg and fg colors come from the color table at end of DIB
        lea     esi,dwPhysColor
        ror     esi,10H
        mov     si,ss
        ror     esi,10H
        mov     lpPhysColor,esi

        mov     esi,dword ptr fs:[edi].BitmapInfo.bmiColors[0]
        cmp     fs:[edi].BitmapInfoHeader.biClrUsed,1
        je      @F
        mov     edi,dword ptr fs:[edi].BitmapInfo.bmiColors[4]
        push    lpDstDev
        bswap   edi
        ror     edi,8
        push    edi
        push    lpPhysColor
        call    GENERIC_ColorInfo
        mov     edi,dwPhysColor
@@:
        push    lpDstDev
        bswap   esi
        ror     esi,8
        push    esi
        push    lpPhysColor
        call    GENERIC_ColorInfo

        ; push two translated colors
        push    dwPhysColor
        push    edi

        mov     edx,07FFFH
        sub     ebx,ebx
        push    dwRop3                          ; Rop3
        push    lpBrush                         ; lpBrush
        push    lpDrawMode                      ; lpDrawmode
        push    edx                             ; scans left in start seg
        push    edx                             ; scans per seg
        push    ebx                             ; fill bytes

        call    GN_BitBltSrcMonoDstVram
        or      eax,eax
        je      GN_StrDibPunt
        mov     eax,dwDstExtY
        jmp     GN_StrDibExit
ENDPROC


;==============================================================================
;
; Function:     GN_StretchColorDib
;
; Purpose:      This function is jumped to from the StretchDIBits
;               display driver entry point. We have already clipped
;               the DIB/Bitmap, made the determination that we will
;               handle the call and locked the display. Here, the
;               parameters should be pushed on the stack to call the
;               HW specific routine to perform the mono transfer.
;
; Arguments:    StretchDIBits Display Driver Stack Frame
;               ds      Context Selector
;               fs:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;               es:esi  lpDstDev ptr  (ptr to DIBENGINE HDR)
;
; Returns:      No returns -- this function is jumped to. When it is done it
;               should jump to one of the StretchDIBits ending routines.
;
;==============================================================================
DECPROC GN_StretchColorDib, PASCAL, NOFRAME, NEAR
OPENPROC
        ; The bitcount is tricky. If biCompression is BI_RGB, then 16bpp
        ; is really 15bpp. If biCompression is BI_BITFIELDS, then 16bpp
        ; might be either 16bpp or 15bpp -- have to check the masks.
        movzx   ecx,fs:[edi].BitmapInfoHeader.biBitCount
        cmp     ecx,10H
        jne     @F
        mov     ecx,0FH
        cmp     fs:[edi].BitmapInfoHeader.biCompression,BI_BITFIELDS
        jne     @F
        cmp     fs:[edi].BitmapV4Header.bV4GreenMask,3E0H
        je      GN_StrDibPunt
        inc     ecx
        cmp     fs:[edi].BitmapV4Header.bV4GreenMask,7E0H
        jne     GN_StrDibPunt
@@:
        mov     ebx,cs:[CSpGNLogicalDevice]
        push    ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        movzx   eax,word ptr es:[esi].DIBENGINE.deBits+4
        push    eax
        push    dword ptr es:[esi].DIBENGINE.deBits
        movzx   eax,es:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    es:[esi].DIBENGINE.deDeltaScan
        push    dwBmpLeft
        push    dwBmpTop
        movzx   eax,word ptr lpDIBBits+2
        push    eax
        push    dwDibBitsOffset
        push    ecx
        push    dwDibPitch
        push    dwDibLeft
        push    dwDibTop
        push    dwExtX
        push    dwExtY

        push    dwRop3
        push    lpBrush                         ; lpBrush
        push    lpDrawMode                      ; lpDrawMode
        pushd   4                               ; dwSizeColorTableEntry

        ; If the src is > 8bpp, then there is no color table.
        ; If there is a color table, compute the number of entries.
        sub     eax,eax
        sub     ebx,ebx
        cmp     cl,8
        jbe     IsColorTable
        cmp     es:[esi].DIBENGINEHDR.bRealBpp,8
        jne     @F
        mov     ebx,lpTranslate
        jmp     @F

IsColorTable:
        mov     ebx,lpDIBInfo
        add     bx,word ptr fs:[edi].BitmapInfoHeader.biSize
        mov     edx,fs:[edi].BitmapInfoHeader.biClrUsed
        mov     eax,1
        shl     eax,cl
        or      edx,edx
        je      @F
        cmp     eax,edx
        jc      @F
        mov     eax,edx
@@:
        push    eax                             ; number color table entries
        push    ebx                             ; lpColorTable

        ; pfnCopyXlat - NULL means that HW_BitBltSrcColorDstVram
        ; must determine the correct copy / xlat routine for itself.
        sub     eax,eax
        push    eax

        ; Final check for the stretch case
        cmp     dwStretchFlag,0
        jne     StretchCase
        call    HW_BitBltSrcColorDstVram
        jmp     RetFromClrBlt

StretchCase:
        push    dwDstExtX
        push    dwDstExtY
        push    lpClipRect
        call    GN_StretchBltSrcColorDstVram

RetFromClrBlt:
        or      eax,eax
        je      GN_StrDibPunt
        mov     eax,dwDstExtY
        jmp     GN_StrDibExit

GN_StretchColorDib      ENDP


;==============================================================================
;
; Function:     GN_StretchDIBitsClip
;
; Purpose:      This function clips the DIB and Bitmap for StretchDIBits.
;
; Arguments:    StretchDIBits Display Driver Stack Frame
;               fs:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;
; Returns:      eax     = 0, there is nothing to draw
;                       > 0, there is something to draw
;
; Preserve:     fs,edi,es
;==============================================================================
PUBLIC  GN_StretchDIBitsClip
GN_StretchDIBitsClip    PROC    NEAR

        ; Get the width and height of the DIB and compute the signed pitch.
        mov     ecx,fs:[edi].BitmapInfoHeader.biHeight
        mov     edx,fs:[edi].BitmapInfoHeader.biWidth
        movzx   eax,fs:[edi].BitmapInfoHeader.biBitCount
        imul    eax,edx
        add     eax,1FH
        shr     eax,3
        and     eax,NOT 3
        neg     ecx
        jns     @F
        neg     ecx
        neg     eax
@@:     mov     dwDibWidth,edx
        mov     dwDibHeight,ecx

        movsx   edx,wSrcY
        movsx   ecx,wSrcExtY
        movsx   esi,wDstY
        movsx   ebx,wDstExtY

        or      ecx,ecx
        jns     @F
        add     edx,ecx
        neg     eax
        neg     ecx
@@:
        or      ebx,ebx
        jns     @F
        add     esi,ebx
        neg     eax
        neg     ebx
@@:
        mov     dwDibPitch,eax
        mov     dwExtY,ecx
        mov     dwDstExtY,ebx
        mov     dwDibTop,edx
        mov     dwBmpTop,esi

        ; Compute the unclipped DIB bounding rectangle
        mov     esi,dwDibHeight
        movsx   eax,wSrcX
        mov     edx,dwExtY
        mov     ebx,dwDibTop
        movsx   ecx,wSrcExtX
        add     ecx,eax
        sub     esi,ebx
        mov     dwDibRight,ecx
        mov     dwDibBottom,esi
        mov     dwDibLeft,eax
        sub     esi,edx
        mov     dwDibTop,esi

        ; Compute the unclipped Bitmap bounding rectangle
        movsx   eax,wDstX
        mov     ebx,dwBmpTop
        mov     dwBmpLeft,eax
        movsx   ecx,wDstExtX
        mov     edx,dwDstExtY
        add     eax,ecx
        add     ebx,edx
        mov     dwBmpRight,eax
        mov     dwBmpBottom,ebx

        ; Calculate the scaling factor
        sub     eax,eax
        shrd    eax,ecx,12
        mov     ecx,edx
        movsx   ebx,wSrcExtX
        sub     edx,edx
        idiv    ebx
        mov     dwScaleX,eax
        sub     eax,eax
        shrd    eax,ecx,12
        mov     ebx,dwExtY
        sub     edx,edx
        idiv    ebx
        mov     dwScaleY,eax

        ; Do the source clipping on the left and top of the DIB and bitmap
        mov     eax,dwDibLeft
        mov     ebx,dwDibTop
        or      eax,eax
        jns     @F
        sub     dwDibLeft,eax
        neg     eax
        imul    eax,dwScaleX
        shr     eax,20
        add     dwBmpLeft,eax
@@:     or      ebx,ebx
        jns     @F
        sub     dwDibTop,ebx
        neg     ebx
        imul    ebx,dwScaleY
        shr     ebx,20
        add     dwBmpTop,ebx
@@:
        ; Do source clipping on the right and bottom of the DIB and bitmap
        mov     eax,dwDibBottom
        sub     eax,dwDibHeight
        jle     @F
        sub     dwDibBottom,eax
        imul    eax,dwScaleY
        shr     eax,20
        sub     dwBmpBottom,eax
@@:     mov     eax,dwDibRight
        sub     eax,dwDibWidth
        jle     @F
        sub     dwDibRight,eax
        imul    eax,dwScaleX
        shr     eax,20
        sub     dwBmpRight,eax

@@:
        ; Dst clipping uses the clip rectangle
        sub     esi,esi
        lgs     si,lpClipRect

        ; Clip the dst left and top edges
        mov     eax,dwBmpLeft
        movsx   edx,gs:[esi].RECT.left
        sub     eax,edx
        jge     @F
        neg     eax
        shl     eax,20
        sub     edx,edx
        idiv    dwScaleX
        add     dwDibLeft,eax
        imul    eax,dwScaleX
        shr     eax,20
        add     dwBmpLeft,eax
@@:
        mov     eax,dwBmpTop
        movsx   edx,gs:[esi].RECT.top
        sub     eax,edx
        jge     @F
        neg     eax
        shl     eax,20
        sub     edx,edx
        idiv    dwScaleY
        add     dwDibTop,eax
        imul    eax,dwScaleY
        shr     eax,20
        add     dwBmpTop,eax
@@:

        ; Clip the dst right and bottom edges
        mov     eax,dwBmpRight
        movsx   edx,gs:[esi].RECT.right
        sub     eax,edx
        jle     @F
        shl     eax,20
        sub     edx,edx
        idiv    dwScaleX
        sub     dwDibRight,eax
        imul    eax,dwScaleX
        shr     eax,20
        sub     dwBmpRight,eax
@@:
        mov     eax,dwBmpBottom
        movsx   edx,gs:[esi].RECT.bottom
        sub     eax,edx
        jle     @F
        shl     eax,20
        sub     edx,edx
        idiv    dwScaleY
        sub     dwDibBottom,eax
        imul    eax,dwScaleY
        shr     eax,20
        sub     dwBmpBottom,eax
@@:

DoneClip:
        ; It's all clipped -- now compute the extents
        mov     eax,dwDibRight
        mov     ebx,dwDibBottom
        sub     eax,dwDibLeft
        jle     NothingVisible
        sub     ebx,dwDibTop
        jle     NothingVisible
        mov     dwExtX,eax
        mov     dwExtY,ebx

        mov     eax,dwBmpRight
        mov     ebx,dwBmpBottom
        sub     eax,dwBmpLeft
        jle     NothingVisible
        sub     ebx,dwBmpTop
        jle     NothingVisible
        mov     dwDstExtX,eax
        mov     dwDstExtY,ebx

        ; The bits ptr is normal if the DIB is right side up
        mov     ecx,dwDibPitch
        movzx   ebx,word ptr lpDIBBits
        or      ecx,ecx
        jg      @F
        mov     edx,dwDibHeight
        neg     ecx
        dec     edx
        imul    ecx,edx
        add     ebx,ecx
@@:
        mov     dwDibBitsOffset,ebx
        mov     eax,1
        ret

NothingVisible:
        sub     eax,eax
        ret

GN_StretchDIBitsClip    ENDP


IF DIB_PROCESS_SINGLESCAN
;==============================================================================
;
; Function:     GN_StretchDIBitsSingleScan
;
; Purpose:      This routine is jumped to from GENERIC_StretchDIBits.
;               when the generic code has determined that the call
;               matches this special case: the DIB is 8bpp, 1 scanline
;               high, wSrcExtY = wDstExtY = 1, the rop is 0CCH.
;
; Arguments:    StretchDIBits Display Driver Stack Frame
;               fs:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;               es:esi  lpDstDev
;
; Returns:      eax     > 0     Success!
;                       0       this routine wants the normal code
;                               to handle the call.
;
; Preserve:     If you return success, you do not need to preserve
;               anything. If you return that you want the normal
;               code to handle the call, then preserve es,esi,fs,edi.
;==============================================================================
DECPROC GN_StretchDIBitsSingleScan, PASCAL, NOFRAME, NEAR
OPENPROC
        movzx   eax,es:[esi].DIBENGINE.deBitsPixel
        mov     dwDstBitsPixel,eax

        sub     ebx,ebx
        lgs     bx,lpClipRect

        ; Clip the YCoord
        movzx   edx,wDstExtY
        movzx   esi,wSrcY
        movzx   eax,wDstY
        mov     ecx,eax
        sub     ax,gs:[ebx].RECT.top
        jge     @F
        movsx   eax,ax
        add     edx,eax
        sub     ecx,eax
@@:
        mov     eax,ecx
        add     eax,edx
        sub     ax,gs:[ebx].RECT.bottom
        jle     @F
        sub     edx,eax
        add     esi,eax
@@:
        ; The ClippedSrcY must be 0 or we can't special case
        or      esi,esi
        jne     Fallback

        ; The ClippedExtY must be 1 or we can't special case
        cmp     edx,1
        jne     Fallback

        ; We can special case it! NOTE: ecx = clipped starting dst y
        mov     dwBmpTop,ecx

        ; We need to compute the clipped DstX, clipped SrcX,
        ; and clipped ExtX
        movzx   ecx,wDstExtX
        movzx   esi,wSrcX
        movsx   eax,wDstX
        or      eax,eax
        jge     @F
        sub     esi,eax
        add     ecx,eax
        sub     eax,eax
@@:
        mov     edx,eax
        sub     ax,gs:[ebx].RECT.left
        jge     @F
        movsx   eax,ax
        add     ecx,eax
        sub     edx,eax
        sub     esi,eax
@@:
        lea     eax,[edx][ecx]
        sub     ax,gs:[ebx].RECT.right
        jle     @F
        sub     ecx,eax
@@:
        ; At this point:
        ; edx = clipped DstX
        ; ecx = clipped ExtX
        ; esi = clipped SrcX

        ; Get edx = DstY * DeltaScan + deBitsOffset
        mov     eax,edx
        movzx   ebx,word ptr lpDstDev
        mov     edx,dwBmpTop
        imul    edx,es:[ebx].DIBENGINE.deDeltaScan
        add     edx,es:[ebx].DIBENGINEHDR.deBitsOffset
        mov     gs,es:[ebx].DIBENGINEHDR.deBitsSelector

        ; Load up ptr to src
        sub     ebx,ebx
        les     bx,lpDIBBits
        add     ebx,esi
        mov     esi,cs:[CSpGNLogicalDevice]

        ; Point to color table
        add     edi,fs:[edi].BitmapInfoHeader.biSize

        ; Sync with the hardware
        mov     esi,ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_BeginAccess

        ; Dispatch to correct color depth routine
        cmp     dwDstBitsPixel,10H
        ja      GN_DibSingleScan_Bpp32
        je      GN_DibSingleScan_Bpp16

        ; if not NULL, need to use lpTranslate
        cmp     lpTranslate,0
        je      GN_DibSingleScan_Bpp8
        sub     edi,edi
        lfs     di,lpTranslate
        jmp     GN_DibSingleScan_Bpp8

Fallback:
        movzx   esi,word ptr lpDstDev
        sub     eax,eax
CLOSEPROC
ENDIF

CLOSESEG _NVTEXT32

END

