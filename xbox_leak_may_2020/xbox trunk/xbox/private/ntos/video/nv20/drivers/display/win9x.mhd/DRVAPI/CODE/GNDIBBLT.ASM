;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gndibblt.asm
;
; Purpose:      This file holds the DibBlt display driver entry point.
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
; Function:     GENERIC_DibBlt
;
; Purpose:      This function is the DibBlt display driver entry point.
;
; Arguments:    DibBlt Display Driver Stack Frame
;
; Returns:      ax      number of scans copied if successful
;                       0 if not successful
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_DibBlt, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   fGet
PARMW   iStart
PARMW   cScans
PARMD   lpDIBBits
PARMD   lpDIBInfo
PARMD   lpDrawMode
PARMD   lpTranslate

LOCALD  dwBmpLeft
LOCALD  dwBmpTop
LOCALD  dwDibBitsOffset
LOCALD  dwExtX
LOCALD  dwExtY
LOCALD  dwDibPitch
LOCALD  dwPhysColor
OPENPROC
        PUSHR   ds,esi,edi

        ; Load up the DstDev and the DIB Header for dispatching
        sub     eax,eax
        mov     gs,word ptr lpDstDev+2
        movzx   esi,word ptr lpDstDev+0
        mov     ds,word ptr lpDIBInfo+2
        movzx   edi,word ptr lpDIBInfo+0

        ; not handling negative cScans case
        cmp     ax,cScans
        jge     GN_DibBltPunt

        ; The case where lpDIBBits is NULL is a sizing request -- punt it
        cmp     eax,lpDIBBits
        je      GN_DibBltPuntNoWait

IF      (DIB_8BPP_ACCEL NE 0)
        ; If the dst is 8bpp and src is mono, then punt it.
        cmp     gs:[esi].DIBENGINE.deBitsPixel,8
        jne     @F
        cmp     ds:[edi].BitmapInfoHeader.biBitCount,1
        je      GN_DibBltPunt
        ; It we're reading from 8bpp screen, then punt it. 
        cmp     fGet,0
        jne     GN_DibBltPunt
@@:
ELSE
        ; If the dst is 8bpp, then punt it.
        cmp     gs:[esi].DIBENGINE.deBitsPixel,8
        je      GN_DibBltPunt
ENDIF

        ; We only do RGB DIBs (No Compression)
        cmp     ds:[edi].BitmapInfoHeader.biCompression,BI_RGB
        jne     GN_DibBltPunt

        ; If the bitmap is not in VRAM, then punt it.
        cmp     ax,gs:[esi].DIBENGINE.deType
        je      GN_DibBltPuntNoWait

        test    gs:[esi].DIBENGINE.deFlags,VRAM
        jz      GN_DibBltPuntNoWait

        ; Clip the call now since we are going to handle it
        call    GN_DibBltClip
        or      eax,eax
        je      GN_DibBltSuccess

        ; Syncronize with other drawing components
        mov     fs,cs:[wFlatDataSel]
        mov     ebx,cs:[CSpGNLogicalDevice]
        cmp     fs:[ebx].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_DibBltPunt

        ; Is this a DIB->Bitmap or Bitmap->DIB conversion
        cmp     fGet,0
        jne     GN_DibBltGet
        cmp     ds:[edi].BitmapInfoHeader.biBitCount,1
        jne     GN_DibBltSetColor
        jmp     GN_DibBltSetMono

GENERIC_DibBlt  ENDP


;==============================================================================
;
; Functions:    DibBlt ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of DibBlt. You can either return a success code,
;               a failure code, punt to the DIBENG, etc.
;
; Arguments:    DibBlt Display Driver Stack Frame
;
;==============================================================================
PUBLIC  GN_DibBltPunt
GN_DibBltPunt     PROC    NEAR
        ; If the bitmap is in VRAM, then we must wait for idle
        ; because the DIBENG will not.
        mov     gs,word ptr lpDstDev+2
        movzx   esi,word ptr lpDstDev+0
        cmp     gs:[esi].DIBENGINE.deType,0
        je      GN_DibBltPuntNoWait
        test    gs:[esi].DIBENGINE.deFlags,VRAM OR OFFSCREEN
        jz      GN_DibBltPuntNoWait
        push    cs:[CSlp16LogDevice]
        push    eax
        push    eax
        pushw   FB_ACCESS
        call    GENERIC_BeginAccess
        .errnz  $ - OFFSET GN_DibBltPuntNoWait
GN_DibBltPunt   ENDP

PUBLIC  GN_DibBltPuntNoWait
GN_DibBltPuntNoWait PROC    NEAR
        ; The DIBENG DibBltExt takes an extra parameter which indicates
        ; whether we are running in a palettized mode right now.
        test    gs:[esi].DIBENGINEHDR.deFlags,PALETTIZED
        setne   al
        movzx   eax,al
        POPR    ds,esi,edi
        STOPPROC
        pop     ecx
        push    ax
        push    ecx
PLABEL  Punt_DibBlt
        jmp     DIB_DibBltExt
GN_DibBltPuntNoWait ENDP


PUBLIC  GN_DibBltSuccess
GN_DibBltSuccess  PROC    NEAR
        movzx   eax,cScans
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 1AH
GN_DibBltSuccess  ENDP


;==============================================================================
;
; Function:     GN_DibBltSetMono
;
; Purpose:      This function is jumped to from the DibBlt
;               display driver entry point. We have already clipped
;               the DIB/Bitmap, made the determination that we will
;               handle the call and locked the display. Here, the
;               parameters should be pushed on the stack to call the
;               HW specific routine to perform the mono transfer.
;               The operation is from a DIB to a Bitmap and the DIB is mono.
;
; Arguments:    DibBlt Display Driver Stack Frame
;               ds:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;               gs:esi  lpDstDev ptr  (ptr to DIBENGINE HDR)
;
; Returns:      No returns -- this function is jumped to. When it is done
;               it should jump to one of the DibBlt ending routines.
;
;==============================================================================
PUBLIC  GN_DibBltSetMono
GN_DibBltSetMono        PROC    NEAR

        ; We can handle this as a mono transfer
        push    cs:[CSpGNLogicalDevice]
        movzx   eax,word ptr gs:[esi].DIBENGINE.deBits+4
        push    eax
        push    dword ptr gs:[esi].DIBENGINE.deBits
        movzx   eax,gs:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    gs:[esi].DIBENGINE.deDeltaScan
        push    dwBmpLeft
        push    dwBmpTop
        movzx   eax,word ptr lpDIBBits+2
        sub     ecx,ecx
        push    eax
        push    dwDibBitsOffset
        push    dwDibPitch
        push    ecx
        push    ecx
        push    dwExtX
        push    dwExtY

        lea     esi,dwPhysColor
        ror     esi,10H
        mov     si,ss
        ror     esi,10H
        push    lpDstDev
        mov     eax,dword ptr ds:[edi].BitmapInfo.bmiColors[0]
        bswap   eax
        ror     eax,8
        push    eax
        push    esi
        call    GENERIC_ColorInfo
        push    dwPhysColor

        cmp     ds:[edi].BitmapInfoHeader.biClrUsed,1
        je      @F
        push    lpDstDev
        mov     eax,dword ptr ds:[edi].BitmapInfo.bmiColors[4]
        bswap   eax
        ror     eax,8
        push    eax
        push    esi
        call    GENERIC_ColorInfo
@@:     push    dwPhysColor

        sub     eax,eax
        mov     edx,07FFFH
        pushd   0CC0000H                        ; Rop3
        push    eax                             ; lpBrush
        push    lpDrawMode                      ; lpDrawmode
        push    edx                             ; scans left in start seg
        push    edx                             ; scans per seg
        push    eax                             ; fill bytes

        ; Load Device Context Selector
        mov     ds,cs:[wFlatDataSel]

        call    GN_BitBltSrcMonoDstVram
        or      eax,eax
        je      GN_DibBltPunt
        jmp     GN_DibBltSuccess

GN_DibBltSetMono        ENDP


;==============================================================================
;
; Function:     GN_DibBltSetColor
;
; Purpose:      This function is jumped to from the DibBlt
;               display driver entry point. We have already clipped
;               the DIB/Bitmap, made the determination that we will
;               handle the call and locked the display. Here, the
;               parameters should be pushed on the stack to call the
;               HW specific routine to perform the color transfer.
;               The operation is from a DIB to a Bitmap and the DIB is color.
;               (this could be a 4, 8, 15, 16, 24, or 32bpp DIB.)
;
; Arguments:    DibBlt Display Driver Stack Frame
;               ds:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;               gs:esi  lpDstDev ptr  (ptr to DIBENGINE HDR)
;
; Returns:      No returns -- this function is jumped to. When it is done
;               it should jump to one of the DibBlt ending routines.
;
;==============================================================================
PUBLIC  GN_DibBltSetColor
GN_DibBltSetColor       PROC    NEAR

        ; The bitcount is tricky. If biCompression is BI_RGB, then 16bpp
        ; is really 15bpp. If biCompression is BI_BITFIELDS, then 16bpp
        ; might be either 16bpp or 15bpp -- have to check the masks.
        movzx   ecx,ds:[edi].BitmapInfoHeader.biBitCount
        cmp     ecx,10H
        jne     @F
        mov     ecx,0FH
        cmp     ds:[edi].BitmapInfoHeader.biCompression,BI_BITFIELDS
        jne     @F
        cmp     ds:[edi].BitmapV4Header.bV4GreenMask,3E0H
        je      GN_DibBltPunt
        inc     ecx
        cmp     ds:[edi].BitmapV4Header.bV4GreenMask,7E0H
        jne     GN_DibBltPunt
@@:
        ; For this conversion, we use color transfer
        mov     es,cs:[wFlatDataSel]
        mov     ebx,cs:[CSpGNLogicalDevice]
        push    es:[ebx].GNLOGDEVCX.pHWLogdevCX
        movzx   eax,word ptr gs:[esi].DIBENGINE.deBits+4
        push    eax
        push    dword ptr gs:[esi].DIBENGINE.deBits
        movzx   eax,gs:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    gs:[esi].DIBENGINE.deDeltaScan
        push    dwBmpLeft
        push    dwBmpTop
        movzx   edx,word ptr lpDIBBits+2
        sub     eax,eax
        push    edx
        push    dwDibBitsOffset
        push    ecx
        push    dwDibPitch
        push    eax                             ; dwSrcLeft
        push    eax                             ; dwSrcTop
        push    dwExtX
        push    dwExtY

        pushd   0CC0000H
        push    eax                             ; lpBrush
        push    lpDrawMode                      ; lpDrawMode
        pushd   4                               ; dwSizeColorTableEntry

        ; If the src is > 8bpp, then there is no color table.
        ; If there is a color table, compute the number of entries.
        sub     ebx,ebx
        cmp     cl,8
        jbe     IsColorTable
        cmp     gs:[esi].DIBENGINEHDR.bRealBpp,8
        jne     @F
        mov     ebx,lpTranslate
        jmp     @F

IsColorTable:
        mov     bx,ds
        shl     ebx,10H
        lea     bx,[di].BitmapInfo.bmiColors
        mov     edx,ds:[edi].BitmapInfoHeader.biClrUsed
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

        ; Load Device Context Selector
        mov     ds,cs:[wFlatDataSel]

        call    HW_BitBltSrcColorDstVram
        or      eax,eax
        je      GN_DibBltPunt
        jmp     GN_DibBltSuccess

GN_DibBltSetColor       ENDP


;==============================================================================
;
; Function:     GN_DibBltGet
;
; Purpose:      This function is jumped to from the DibBlt
;               display driver entry point. We have already clipped
;               the DIB/Bitmap, made the determination that we will
;               handle the call and locked the display. Here, the
;               parameters should be pushed on the stack to call the
;               HW specific routine to perform the DMA back transfer
;               interleaved with the copy into the correct system
;               memory bitmap.
;               The operation is from a Bitmap to a DIB.
;
; Arguments:    DibBlt Display Driver Stack Frame
;               ds:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;               gs:esi  lpDstDev ptr  (ptr to DIBENGINE HDR)
;
; Returns:      No returns -- this function is jumped to. When it is done
;               it should jump to one of the DibBlt ending routines.
;
;==============================================================================
PUBLIC  GN_DibBltGet
GN_DibBltGet    PROC    NEAR

        ; The bitcount is tricky. If biCompression is BI_RGB, then 16bpp
        ; is really 15bpp. If biCompression is BI_BITFIELDS, then 16bpp
        ; might be either 16bpp or 15bpp -- have to check the masks.
        movzx   eax,ds:[edi].BitmapInfoHeader.biBitCount
        cmp     eax,10H
        jne     @F
        mov     eax,0FH
        cmp     ds:[edi].BitmapInfoHeader.biCompression,BI_BITFIELDS
        jne     @F
        cmp     ds:[edi].BitmapV4Header.bV4GreenMask,3E0H
        je      GN_DibBltPunt
        inc     eax
        cmp     ds:[edi].BitmapV4Header.bV4GreenMask,7E0H
        jne     GN_DibBltPunt
@@:
        ; Use the GetVramBitmap to interleave DMA and conversion
        push    cs:[CSpGNLogicalDevice]
        sub     edx,edx
        push    edx
        push    lpDIBInfo
        movzx   ecx,word ptr lpDIBBits+2
        push    ecx
        push    dwDibBitsOffset
        push    eax
        push    dwDibPitch
        push    edx             ; dwDibLeft
        push    edx             ; dwDibTop

        push    lpDstDev        ; It's called dstdev but it is really the src
        movzx   eax,word ptr gs:[esi].DIBENGINE.deBits+4
        push    eax
        push    gs:[esi].DIBENGINEHDR.deBitsOffset
        movzx   eax,gs:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    gs:[esi].DIBENGINE.deDeltaScan
        push    dwBmpLeft
        push    dwBmpTop
        push    dwExtX
        push    dwExtY
        pushd   0CC0000H
        push    edx             ; lpBrush
        push    lpDrawMode      ; lpDrawMode
        push    edx             ; pfnCopyXlat :Let called func figure it out

        ; Load Device Context Selector
        mov     ds,cs:[wFlatDataSel]

        call    GN_BitBltSrcVramDstSystem
        or      eax,eax
        je      GN_DibBltPunt
        jmp     GN_DibBltSuccess

GN_DibBltGet    ENDP


;==============================================================================
;
; Function:     GN_DibBltClip
;
; Purpose:      This function clips the DIB and Bitmap for DibBlt.
;
; Arguments:    DibBlt Display Driver Stack Frame
;               ds:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;               gs:esi  lpDstDev
;
; Returns:      eax     = 0, there is nothing to draw
;                       > 0, there is something to draw
;
; Preserve:     ds,edi,gs
;==============================================================================
PUBLIC  GN_DibBltClip
GN_DibBltClip   PROC    NEAR

        ; If there is nothing to do, then report all clipped
        movsx   ecx,cScans
        or      ecx,ecx
        je      AllClipped

        ; Ext X = minimum (Bitmap Width, DIB width)
        movsx   eax,gs:[esi].DIBENGINE.deWidth
        mov     edx,ds:[edi].BitmapInfoHeader.biWidth
        cmp     eax,edx
        jl      @F
        mov     eax,edx
@@:     mov     dwExtX,eax

        ; Compute the unsigned DIB pitch. We will make it signed to
        ; indicate DIB orientation later when we adjust the bits ptr.
        movzx   eax,ds:[edi].BitmapInfoHeader.biBitCount
        mov     ecx,ds:[edi].BitmapInfoHeader.biWidth
        imul    eax,ecx
        add     eax,31
        and     eax,NOT 31
        shr     eax,3
        mov     dwDibPitch,eax
        movzx   edx,word ptr lpDIBBits
        mov     dwDibBitsOffset,edx

        ; Based upon the sign of the DIB height, adjust for DIB orientation.
        mov     edx,ds:[edi].BitmapInfoHeader.biHeight
        or      edx,edx
        js      @F
        movzx   ecx,cScans
        dec     ecx
        imul    ecx,eax
        add     dwDibBitsOffset,ecx
        neg     eax
        mov     dwDibPitch,eax
        neg     edx
@@:     neg     edx

        ; The only thing left is Bitmap top and bottom coords
        movsx   ebx,iStart
        movsx   ecx,cScans
        sub     edx,ebx
        sub     edx,ecx
        mov     dwBmpTop,edx
        sub     edx,edx
        mov     dwExtY,ecx
        mov     dwBmpLeft,edx
        mov     eax,1
        ret

AllClipped:
        sub     eax,eax
        ret

GN_DibBltClip   ENDP

CLOSESEG _NVTEXT32

END

