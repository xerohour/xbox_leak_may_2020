;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation
;
; File:         gnstrblt.asm
;
; Purpose:      This file holds the StretchBlt display driver entry point.
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
; Function:     GENERIC_StretchBlt
;
; Purpose:      This function is the StretchBlt display driver entry point.
;
; Arguments:    StretchDIBits Display Driver Stack Frame
;
; Returns:      ax      1  if successful
;                       0  if not successful
;                       -1 GDI should carry out the operation.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_StretchBlt, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wDstX
PARMW   wDstY
PARMW   wDstExtX
PARMW   wDstExtY
PARMD   lpSrcDev
PARMW   wSrcX
PARMW   wSrcY
PARMW   wSrcExtX
PARMW   wSrcExtY
PARMD   dwRop3
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   lpClipRect

LOCALD  dwBmpLeft
LOCALD  dwBmpTop
LOCALD  dwBmpRight
LOCALD  dwBmpBottom
LOCALD  dwSrcLeft
LOCALD  dwSrcTop
LOCALD  dwSrcRight
LOCALD  dwSrcBottom
LOCALD  dwExtX
LOCALD  dwExtY
LOCALD  dwSrcWidth
LOCALD  dwSrcHeight
LOCALD  dwScaleX
LOCALD  dwScaleY
LOCALD	dwDstExtX
LOCALD	dwDstExtY
OPENPROC
        PUSHR   ds,esi,edi

        sub     eax,eax

        ; Load up the ptrs
        sub     edi,edi
        sub     esi,esi
        lfs     di,lpSrcDev
        les     si,lpDstDev

        ; If the dst bitmap is not in VRAM, then punt it.
        cmp     ax,es:[esi].DIBENGINE.deType
        je      GN_StrBltPunt
        test    es:[esi].DIBENGINE.deFlags,VRAM
        jz      GN_StrBltPunt

        ; If the src bitmap is in VRAM, then punt it.
        cmp     ax,fs:[edi].DIBENGINE.deType
        je      GN_StrBltPunt
        test    fs:[edi].DIBENGINE.deFlags,VRAM
        jnz	GN_StrBltPunt

        movsx   eax,wSrcExtX
        movsx   ebx,wDstExtX
        movsx   ecx,wSrcExtY
        movsx   edx,wDstExtY

	; Punt the negative widths (Left-Right mirror Arabic Windows)
	or	eax,ebx
        js	GN_StrBltPunt

	; Punt the negative heights (Top-Bottom mirror)
	or	ecx,edx
        js	GN_StrBltPunt

	; Punt Stretch for mono source
        cmp     fs:[edi].DIBENGINEHDR.bRealBpp,1
	je	GN_StrBltPunt

        ; Make sure the device is not busy.
        mov     ds,cs:[wFlatDataSel]
        mov     ebx,cs:[CSpGNLogicalDevice]
        cmp     ds:[ebx].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_StrBltPunt

        ; First, do clipping
        call    GN_StretchBltClip
        or      eax,eax
	mov	eax,1
        je      GN_StrBltExit

        ; If the rop involves a pattern, prepare the pattern now
        movzx   eax,byte ptr [dwRop3 + 2]
        movzx   ecx,byte ptr cs:Rop3Table[eax]
        cmp     ecx,PAD
        je      PrepBrush
        cmp     ecx,PSD
        je      PrepBrush
        movzx   esi,word ptr lpDstDev+0
        jmp     GN_StretchColorBlt

PrepBrush:
        mov     es,word ptr lpBrush+2
        movzx   esi,word ptr lpBrush+0
        push    lpDrawMode
        mov     ebx,cs:[CSpGNLogicalDevice]
        call    GN_PrepareBrush
        or      eax,eax
        je      GN_StrBltExit
        mov     eax,0
        jl      GN_StrBltExit

        ; reload some stuff that got trashed
        ; Load up the ptrs
        sub     edi,edi
        sub     esi,esi
        lfs     di,lpSrcDev
        les     si,lpDstDev
        jmp     GN_StretchColorBlt

GENERIC_StretchBlt	ENDP

;==============================================================================
;
; Function:     StretchBlt ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of StretchBlt. You can either return a success code,
;               a failure code, punt to the DIBENG, etc.
;
; Arguments:    StretchBlt Display Driver Stack Frame
;
;==============================================================================
PUBLIC  GN_StrBltPunt
GN_StrBltPunt     PROC    NEAR
	push	lpDstDev
	push	wDstX
	push	wDstY
	push	wDstExtX
	push	wDstExtY
	push	lpSrcDev
	push	wSrcX
	push	wSrcY
	push	wSrcExtX
	push	wSrcExtY
	push	dwRop3
	push	lpBrush
	push	lpDrawMode
	push	lpClipRect
PLABEL	Punt_StrBlt
        call    DIB_StretchBlt
        .errnz  $ - OFFSET GN_StrBltExit

GN_StrBltPunt     ENDP

PUBLIC  GN_StrBltExit
GN_StrBltExit   PROC    NEAR

        POPR    ds,esi,edi
        mov     sp,bp
        pop     ebp

        ; Clean and return normally
        RETFAR16 28H
GN_StrBltExit   ENDP


;==============================================================================
;
; Function:     GN_StretchColorBlt
;
; Purpose:      This function is jumped to from the StretchBlt
;               display driver entry point. We have already clipped
;               the DIB/Bitmap, made the determination that we will
;               handle the call and locked the display. Here, the
;               parameters should be pushed on the stack to call the
;               HW specific routine to perform the mono transfer.
;
; Arguments:    StretchBlt Display Driver Stack Frame
;               ds      Context Selector
;               fs:edi  lpSrcDev ptr (ptr to DIBENGINE HDR)
;               es:esi  lpDstDev ptr  (ptr to DIBENGINE HDR)
;
; Returns:      No returns -- this function is jumped to. When it is done it
;               should jump to one of the StretchBlt ending routines.
;
;==============================================================================
DECPROC GN_StretchColorBlt, PASCAL, NOFRAME, NEAR
OPENPROC
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
        movzx   eax,word ptr fs:[edi].DIBENGINE.deBits+4
        push    eax
        push    dword ptr fs:[edi].DIBENGINE.deBits
        movzx   eax,fs:[edi].DIBENGINEHDR.bRealBpp
        push    eax
        push    fs:[edi].DIBENGINE.deDeltaScan
        push    dwSrcLeft
        push    dwSrcTop
        push    dwExtX
        push    dwExtY

        push    dwRop3
        push    lpBrush                         ; lpBrush
        push    lpDrawMode                      ; lpDrawMode

        sub     ebx,ebx
        push	ebx                             ; dwSizeColorTableEntry
        push    ebx                             ; number color table entries
        push    ebx                             ; lpColorTable

        ; pfnCopyXlat - NULL means that HW_BitBltSrcColorDstVram
        ; must determine the correct copy / xlat routine for itself.
        push    ebx
	push	dwDstExtX
	push	dwDstExtY
	push	lpClipRect
        call    GN_StretchBltSrcColorDstVram
        or      eax,eax
        je      GN_StrBltPunt
        mov     eax,1
        jmp     GN_StrBltExit

GN_StretchColorBlt      ENDP


;==============================================================================
;
; Function:     GN_StretchBltClip
;
; Purpose:      This function clips the src and dst Bitmaps for StretchBlt.
;
; Arguments:    StretchBlt Display Driver Stack Frame
;               fs:edi  lpSrcDev ptr (ptr to DIBENGINE HDR)
;
; Returns:      eax     = 0, there is nothing to draw
;                       > 0, there is something to draw
;
; Preserve:     fs,edi,es
;==============================================================================
PUBLIC  GN_StretchBltClip
GN_StretchBltClip    PROC    NEAR

        ; Get the width and height of the DIB and compute the signed pitch.
        movsx	edx,fs:[edi].DIBENGINE.deWidth
        movsx	ecx,fs:[edi].DIBENGINE.deHeight
	mov     dwSrcWidth,edx
        mov     dwSrcHeight,ecx

        movsx   edx,wSrcY
        movsx   ecx,wSrcExtY
        movsx   esi,wDstY
	movsx	ebx,wDstExtY

	mov	dwExtY,ecx
	mov	dwDstExtY,ebx
        mov     dwSrcTop,edx
        mov     dwBmpTop,esi

        ; Compute the unclipped src bounding rectangle
        mov     esi,dwSrcHeight
        movsx   eax,wSrcX
        mov	edx,dwExtY
        mov	ebx,dwSrcTop
        movsx   ecx,wSrcExtX
        add     ecx,eax
        sub     esi,ebx
        mov     dwSrcRight,ecx
        mov     dwSrcBottom,esi
        mov     dwSrcLeft,eax
        sub     esi,edx
        mov     dwSrcTop,esi

        ; Compute the unclipped Bitmap bounding rectangle
        movsx	eax,wDstX
        mov     ebx,dwBmpTop
        mov     dwBmpLeft,eax
	movsx	ecx,wDstExtX
	mov	edx,dwDstExtY
        add     eax,ecx
        add     ebx,edx
        mov     dwBmpRight,eax
        mov     dwBmpBottom,ebx

	; Calculate the scaling factor
	sub	eax,eax
	shrd	eax,ecx,12
	mov	ecx,edx
        movsx   ebx,wSrcExtX
	sub	edx,edx
	idiv	ebx
	mov	dwScaleX,eax
	sub	eax,eax
	shrd	eax,ecx,12
        mov	ebx,dwExtY
	sub	edx,edx
	idiv	ebx
	mov	dwScaleY,eax

        ; Do the source clipping on the left and top of the src and dst bitmaps
        mov	eax,dwSrcLeft
        mov	ebx,dwSrcTop
        or      eax,eax
        jns     @F
        sub     dwSrcLeft,eax
	neg	eax
	imul	eax,dwScaleX
	shr	eax,20
        add	dwBmpLeft,eax
@@:     or      ebx,ebx
        jns     @F
        sub     dwSrcTop,ebx
	neg	ebx
	imul	ebx,dwScaleY
	shr	ebx,20
        add	dwBmpTop,ebx
@@:
        ; Do source clipping on the right and bottom of the src and dst bitmaps
        mov     eax,dwSrcBottom
        sub     eax,dwSrcHeight
        jle	@F
        sub     dwSrcBottom,eax
	imul	eax,dwScaleY
	shr	eax,20
        sub     dwBmpBottom,eax
@@:	mov	eax,dwSrcRight
        sub     eax,dwSrcWidth
        jle	@F
        sub     dwSrcRight,eax
	imul	eax,dwScaleX
	shr	eax,20
        sub     dwBmpRight,eax

@@:
        ; Dst clipping uses the clip rectangle
        sub     esi,esi
        lgs     si,lpClipRect

        ; Clip the dst left and top edges
        mov     eax,dwBmpLeft
        movsx   edx,gs:[esi].RECT.left
        sub     eax,edx
        jge	@F
	neg	eax
	shl	eax,20
	sub	edx,edx
	idiv	dwScaleX
        add     dwSrcLeft,eax
	imul	eax,dwScaleX
	shr	eax,20
        add     dwBmpLeft,eax
@@:
	mov     eax,dwBmpTop
        movsx   edx,gs:[esi].RECT.top
        sub     eax,edx
        jge	@F
	neg	eax
	shl	eax,20
	sub	edx,edx
	idiv	dwScaleY
        add     dwSrcTop,eax
	imul	eax,dwScaleY
	shr	eax,20
        add     dwBmpTop,eax
@@:

        ; Clip the dst right and bottom edges
        mov     eax,dwBmpRight
        movsx   edx,gs:[esi].RECT.right
        sub     eax,edx
        jle	@F
	shl	eax,20
	sub	edx,edx
	idiv	dwScaleX
        sub     dwSrcRight,eax
	imul	eax,dwScaleX
	shr	eax,20
        sub     dwBmpRight,eax
@@:
	mov     eax,dwBmpBottom
        movsx   edx,gs:[esi].RECT.bottom
        sub     eax,edx
        jle	@F
	shl	eax,20
	sub	edx,edx
	idiv	dwScaleY
        sub     dwSrcBottom,eax
	imul	eax,dwScaleY
	shr	eax,20
        sub     dwBmpBottom,eax
@@:

DoneClip:
        ; It's all clipped -- now compute the extents
        mov     eax,dwSrcRight
        mov     ebx,dwSrcBottom
        sub     eax,dwSrcLeft
        jle     NothingVisible
        sub     ebx,dwSrcTop
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
        mov     eax,1
        ret

NothingVisible:
        sub     eax,eax
        ret

GN_StretchBltClip    ENDP

CLOSESEG _NVTEXT32

END

