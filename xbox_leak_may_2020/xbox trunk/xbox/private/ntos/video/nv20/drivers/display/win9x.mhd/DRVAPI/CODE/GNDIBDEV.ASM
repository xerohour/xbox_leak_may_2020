;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gndibdev.asm
;
; Purpose:      This file holds the SetDIBitsToDevice display driver
;               entry point.
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
; Function:     GENERIC_SetDIBitsToDevice
;
; Purpose:      This function is the SetDIBitsToDevice driver entry point.
;
; Arguments:    SetDIBitsToDevice Display Driver Stack Frame
;
; Returns:      ax      number of scans copied if successful
;                       0 if not successful
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_SetDIBitsToDevice, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wDstX
PARMW   wDstY
PARMW   wInitScan
PARMW   wNumScans
PARMD   lpClipRect
PARMD   lpDrawMode
PARMD   lpDIBBits
PARMD   lpDIBInfo
PARMD   lpTranslate
LOCALD  dwBmpBitsPixel
LOCALD  dwBmpLeft
LOCALD  dwBmpTop
LOCALD  dwDibLeft
LOCALD  dwDibTop
LOCALD  dwDibBitsOffset
LOCALD  dwExtX
LOCALD  dwExtY
LOCALD  dwDibPitch
LOCALD  dwDibWidth
LOCALD  dwDibHeight
LOCALD  lpPhysColor
LOCALD  dwPhysColor
LOCALD  dwFlagSecondPhysColorPresent
OPENPROC
        PUSHR   ds,esi,edi
        sub     esi,esi
        sub     edi,edi

        ; Load up the DstDev and the DIB Header for dispatching
        mov     ds,cs:[wFlatDataSel]
        mov     esi,cs:[CSpGNLogicalDevice]
        lfs     di,lpDIBInfo

        ; See if screen is busy
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_DibDevPunt

        ; We only do RGB DIBs (No Compression)
        cmp     fs:[edi].BitmapInfoHeader.biCompression,BI_RGB
        jne     GN_DibDevPunt

IF DIB_PROCESS_SINGLESCAN
        ; Special case the 8bpp dibs that are 1 scanline high,
        ; where wInitScan = 0 and wNumScans = 1.
        cmp     fs:[edi].BitmapInfoHeader.biBitCount,8
        jne     @F
        cmp     fs:[edi].BitmapInfoHeader.biHeight,1
        jne     @F
        movzx   eax,wInitScan
        movzx   ecx,wNumScans
        or      eax,eax
        jne     @F
        cmp     ecx,1
        je      GN_SetDIBitsSingleScan
@@:
ENDIF

IF      (DIB_8BPP_ACCEL NE 0)
        ; If the dst is 8bpp and src is mono, then punt it.
        cmp     ds:[esi].DIBENGINE.deBitsPixel,8
        jne     @F
        cmp     fs:[edi].BitmapInfoHeader.biBitCount,1
        je      GN_DibDev8BppPunt
@@:
ELSE
        ; If the dst is 8bpp, then punt it. Already handled SingleScan case.
        cmp     ds:[esi].DIBENGINE.deBitsPixel,8
        je      GN_DibDev8BppPunt
ENDIF

        ; First, clip the DIB
        call    GN_SetDIBitsToDeviceClip
        or      eax,eax
        je      GN_DibDevExit
        mov     esi,cs:[CSpGNLogicalDevice]

        ; Break the call out into cases.
        cmp     fs:[edi].BitmapInfoHeader.biBitCount,1
        jne     GN_SetColorDibToDevice
        jmp     GN_SetMonoDibToDevice
ENDPROC


;==============================================================================
;
; Function:     SetDIBitsToDevice ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of SetDIBitsToDevice. You can either return a success code,
;               a failure code, punt to the DIBENG, etc.
;
; Arguments:    SetDIBitsToDevice Display Driver Stack Frame
;
;==============================================================================
PUBLIC  GN_DibDevPunt
GN_DibDevPunt   PROC    NEAR
        push    lpDstDev
        push    wDstX
        push    wDstY
        push    wInitScan
        push    wNumScans
        push    lpClipRect
        push    lpDrawMode
        push    lpDIBBits
        push    lpDIBInfo
        push    lpTranslate
PLABEL  Punt_DibToDevice
        call    DIB_DibToDevice
        .errnz  $ - OFFSET GN_DibDevExit

GN_DibDevPunt   ENDP

PUBLIC  GN_DibDevExit
GN_DibDevExit   PROC    NEAR
        POPR    ds,esi,edi
        mov     sp,bp
        mov     ecx,ss:[ebp + 4]
        pop     ebp
IF DIB_FAST_EXIT
        cmp     ax,-1
        je      @F
        cmp     cs:[bUseSetDIBitsFastExit],0
        je      @F
        cmp     ecx,cs:[lpfnGdiCode03_2119]
        jne     @F
        add     sp,4 + 20H
        jmp     GdiCode03_2119
@@:
ENDIF
        ; Clean and return normally
        RETFAR16 20H
GN_DibDevExit   ENDP


;==============================================================================
;
; Function:     GN_DibDev8BppPunt (SetDIBitsToDevice ending routine)
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
; Arguments:    SetDIBitsToDevice Display Driver Stack Frame
;
;==============================================================================
DECPROC GN_DibDev8BppPunt, PASCAL, NOFRAME, NEAR
OPENPROC
        test    wDstX,1                 ; Check for unaligned destination
        jz      GN_DibDevPunt
        xor     esi,esi
        cmp     word ptr [lpClipRect + 2],si
        jz      GN_DibDevPunt
        lfs     si,lpClipRect

        movzx   eax,fs:[esi].RECT.left
        inc     eax
        cmp     ax,fs:[esi].RECT.right  ; Check if clip is 1 pixel wide
        jnz     GN_DibDevPunt
        and     eax,3                   ; Check for (clipleft mod 4) == 3
        jnz     GN_DibDevPunt

        ; Clean and return a failure normally
        POPR    ds,esi,edi
        mov     sp,bp
        mov     eax,-1
        pop     ebp
        RETFAR16 20H
ENDPROC


;==============================================================================
;
; Function:     GN_SetMonoDibToDevice
;
; Purpose:      This function is jumped to from the SetDIBitsToDevice
;               display driver entry point. We have already clipped
;               the DIB/Bitmap, made the determination that we will
;               handle the call and locked the display. Here, the
;               parameters should be pushed on the stack to call the
;               HW specific routine to perform the mono transfer.
;
; Arguments:    SetDIBitsToDevice Display Driver Stack Frame
;               fs:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;               ds      Context Selector
;               esi     GNLOGDEVCX ptr
;
; Returns:      No returns -- this function is jumped to. When it is done it
;               should jump to one of the SetDIBitsToDevice ending routines.
;
;==============================================================================
DECPROC GN_SetMonoDibToDevice, PASCAL, NOFRAME, NEAR
OPENPROC
        ; We can handle this as a mono transfer
        push    cs:[CSpGNLogicalDevice]
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deBitsSelector
        push    eax
        push    ds:[esi].GNLOGDEVCX.Dibeng.deBitsOffset
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.bRealBpp
        push    eax
        push    ds:[esi].GNLOGDEVCX.Dibeng.deDeltaScan
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

        mov     ecx,0CC0000H
        mov     edx,07FFFH
        sub     ebx,ebx
        push    ecx                             ; Rop3
        push    ebx                             ; lpBrush
        push    lpDrawMode                      ; lpDrawmode
        push    edx                             ; scans left in start seg
        push    edx                             ; scans per seg
        push    ebx                             ; fill bytes

        call    GN_BitBltSrcMonoDstVram
        or      eax,eax
        je      GN_DibDevPunt
        movzx   eax,wNumScans
        jmp     GN_DibDevExit
ENDPROC


;==============================================================================
;
; Function:     GN_SetColorDibToDevice
;
; Purpose:      This function is jumped to from the SetDIBitsToDevice
;               display driver entry point. We have already clipped
;               the DIB/Bitmap, made the determination that we will
;               handle the call and locked the display. Here, the
;               parameters should be pushed on the stack to call the
;               HW specific routine to perform the color transfer.
;
; Arguments:    SetDIBitsToDevice Display Driver Stack Frame
;               fs:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;               ds      Context Selector
;               esi     GNLOGDEVCX ptr
;
; Returns:      No returns -- this function is jumped to. When it is done it
;               should jump to one of the SetDIBitsToDevice ending routines.
;
;==============================================================================
DECPROC GN_SetColorDibToDevice, PASCAL, NOFRAME, NEAR
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
        je      GN_DibDevPunt
        inc     ecx
        cmp     fs:[edi].BitmapV4Header.bV4GreenMask,7E0H
        jne     GN_DibDevPunt
@@:
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deBitsSelector
        push    eax
        push    ds:[esi].GNLOGDEVCX.Dibeng.deBitsOffset
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.bRealBpp
        push    eax
        push    ds:[esi].GNLOGDEVCX.Dibeng.deDeltaScan
        push    dwBmpLeft
        push    dwBmpTop
        movzx   edx,word ptr lpDIBBits+2
        push    edx
        push    dwDibBitsOffset
        push    ecx
        push    dwDibPitch
        push    dwDibLeft
        push    dwDibTop
        push    dwExtX
        push    dwExtY
        sub     eax,eax
        pushd   0CC0000H
        push    eax                             ; lpBrush
        push    lpDrawMode                      ; lpDrawMode
        pushd   4                               ; dwSizeColorTableEntry

        ; If the src is > 8bpp, then there is no color table.
        ; If there is a color table, compute the number of entries.
        sub     ebx,ebx
        cmp     cl,8
        jbe     IsColorTable
        cmp     ds:[esi].GNLOGDEVCX.Dibeng.bRealBpp,8
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

        call    HW_BitBltSrcColorDstVram
        or      eax,eax
        je      GN_DibDevPunt
        movzx   eax,wNumScans
        jmp     GN_DibDevExit
ENDPROC


;==============================================================================
;
; Function:     GN_SetDIBitsToDeviceClip
;
; Purpose:      This function clips the DIB and Bitmap for SetDIBitsToDevice.
;
; Arguments:    SetDIBitsToDevice Display Driver Stack Frame
;               fs:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;
; Returns:      eax     = 0, there is nothing to draw
;                       > 0, there is something to draw
;
; Preserve:     ds,edi,gs
;==============================================================================
DECPROC GN_SetDIBitsToDeviceClip, PASCAL, NOFRAME, NEAR
OPENPROC
        ; We need this for the clipping.
        xor     esi,esi                 ; clear high word of ESI
        les     si,lpClipRect

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
        mov     dwDibPitch,eax

        ; Bitmap Top = maximum(pClipRect->top, wDstY);
        movsx   eax,wDstY
        movsx   ecx,es:[esi].RECT.top
        mov     ebx,eax
        cmp     eax,ecx
        jge     @F
        mov     eax,ecx
@@:
        ; Bitmap Top = maximum (0, wDstY)
        ; Dib Top = maximum (0, wDstY) - wDstY
        or      eax,eax
        jge     @F
        sub     eax,eax
@@:     mov     dwBmpTop,eax
        sub     eax,ebx
        mov     dwDibTop,eax

        ; wClippedNumScans = mimimum (DibHeight - wInitScan, wNumScans)
        mov     edx,dwDibHeight
        movsx   eax,wInitScan
        movsx   ecx,wNumScans
        sub     edx,eax
        cmp     edx,ecx
        jle     @F
        mov     edx,ecx
@@:     or      edx,edx
        jle     AllClipped

        ; Bitmap Bottom = mimimum (lpClipRect->bottom, wDstY + wClippedNumScans)
        movzx   eax,es:[esi].RECT.bottom
        add     ebx,edx
        cmp     ebx,eax
        jl      @F
        mov     ebx,eax
@@:     sub     ebx,dwBmpTop
        jle     AllClipped
        mov     dwExtY,ebx

        ; Bitmap Left = maximum(pClipRect->left, wDstX);
        movsx   eax,wDstX
        movsx   ecx,es:[esi].RECT.left
        mov     ebx,eax
        cmp     eax,ecx
        jge     @F
        mov     eax,ecx
@@:
        ; Bitmap Left = maximum (0, wDstX)
        ; Dib Left = maximum (0, wDstX) - wDstX
        or      eax,eax
        jge     @F
        sub     eax,eax
@@:     mov     dwBmpLeft,eax
        sub     eax,ebx
        mov     dwDibLeft,eax

        ; Bitmap Right = minimum(lpClipRect->right, wDstX + DibWidth)
        ; SetDib X Ext = Bitmap Right - Bitmap Left
        mov     eax,dwDibWidth
        movsx   ecx,es:[esi].RECT.right
        add     eax,ebx
        cmp     eax,ecx
        jl      @F
        mov     eax,ecx
@@:     sub     eax,dwBmpLeft
        jle     AllClipped
        mov     dwExtX,eax

        ; Make a ptr to the DIB Bits based upon the orientation of the
        ; DIB (right side up or upside down).
        mov     ecx,dwDibPitch
        movzx   ebx,word ptr lpDIBBits
        or      ecx,ecx
        jg      @F
        mov     edx,dwDibHeight
        neg     ecx
        dec     edx
        imul    ecx,edx
        add     ebx,ecx
@@:     mov     dwDibBitsOffset,ebx

        mov     eax,1
        ret

AllClipped:
        sub     eax,eax
        ret
ENDPROC


IF DIB_PROCESS_SINGLESCAN
;==============================================================================
;
; Function:     GN_SetDIBitsSingleScan
;
; Purpose:      This routine is jumped to from GENERIC_SetDIBitsToDevice
;               when the generic code has determined that the call
;               matches this special case: the DIB is 8bpp, 1 scanline
;               high, wInitScan = 0, and wNumScans = 1.
;
; Arguments:    SetDIBitsToDevice Display Driver Stack Frame
;               fs:edi  lpDibInfo ptr (ptr to BitmapInfoHeader)
;               ds      Context Selector
;               esi     GNLOGDEVCX ptr
;
; Returns:      None
;               This routine jumps to other routines which eventually
;               jump to GN_DibDevExit with the proper return code for
;               the SetDIBitsToDevice call when they are done.
;
; Preserve:     ds,edi,gs
;==============================================================================
DECPROC GN_SetDIBitsSingleScan, PASCAL, NOFRAME, NEAR
OPENPROC
        movzx   eax,ds:[esi].DIBENGINE.deBitsPixel
        mov     dwBmpBitsPixel,eax

        sub     esi,esi
        les     si,lpClipRect
        sub     eax,eax

        ; clip top and bottom
        movsx   edx,wDstY
        movsx   ebx,es:[esi].RECT.top
        cmp     edx,ebx
        jl      GN_DibDevExit
        movsx   ebx,es:[esi].RECT.bottom
        sub     ebx,edx
        cmp     ebx,fs:[edi].BitmapInfoHeader.biHeight
        jg      GN_DibDevExit

        ; clip left
        movsx   eax,wDstX
        movsx   ecx,es:[esi].RECT.left
        sub     ebx,ebx
        cmp     eax,ecx
        jge     @F
        mov     ebx,ecx
        sub     ebx,eax
        mov     eax,ecx
@@:
        ; clip right
        movsx   ecx,es:[esi].RECT.right
        sub     ecx,eax
        cmp     ecx,fs:[edi].BitmapInfoHeader.biWidth
        jg      GN_DibDevExit

        ; At this point:
        ; eax = clipped DstX
        ; ebx = number src pixels clipped on left edge
        ; ecx = clipped ExtX
        ; edx = clipped DstY

        ; Load up the src ptr
        movzx   esi,word ptr lpDIBBits
        mov     es,word ptr lpDIBBits+2
        add     ebx,esi

        ; Point to the color table
        mov     esi,cs:[CSpGNLogicalDevice]
        add     edi,fs:[edi].BitmapInfoHeader.biSize

        ; Get the dst ptr in gs:edx
        imul    edx,ds:[esi].GNLOGDEVCX.Dibeng.deDeltaScan
        add     edx,ds:[esi].GNLOGDEVCX.Dibeng.deBitsOffset
        mov     gs,ds:[esi].GNLOGDEVCX.Dibeng.deBitsSelector

        ; Sync with the hardware
        mov     esi,ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_BeginAccess

        ; Dispatch to correct color depth routine
        push    OFFSET GN_DibDevExit
        cmp     dwBmpBitsPixel,10H
        ja      GN_DibSingleScan_Bpp32
        je      GN_DibSingleScan_Bpp16

        ; if not NULL, need to use lpTranslate
        cmp     lpTranslate,0
        je      GN_DibSingleScan_Bpp8
        sub     edi,edi
        lfs     di,lpTranslate
        jmp     GN_DibSingleScan_Bpp8
ENDPROC
ENDIF

CLOSESEG _NVTEXT32

END

