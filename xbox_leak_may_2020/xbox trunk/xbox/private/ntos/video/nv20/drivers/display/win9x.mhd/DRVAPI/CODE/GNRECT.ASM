;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnrect.asm
;
; Purpose:      This file holds the Output(OS_RECTANGLE) display driver
;               implementation. The Output display driver entry point
;               jumps to here without creating a FRAME when it is
;               discovered that this is and OS_RECTANGLE style.
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
; Function:     GENERIC_Rectangle
;
; Purpose:      This function is the Output(OS_RECTANGLE) display driver
;               implementation. The Output display driver entry point
;               jumps to here without creating a FRAME when it is
;               discovered that this is the OS_RECTANGLE style.
;
; Arguments:    Output Display Driver Stack Frame
;
; Returns:      ax      0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_Rectangle, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wStyle
PARMW   wCount
PARMD   lpPoints
PARMD   lpPen
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   lpClipRect
LOCALD	dwDrawInterior
LOCALD  dwPenColor
LOCALD  dwRop2
LOCALD  pHWLogDevCXOffset
LOCALD	dwBottomRight
LOCALD	dwTopLeft
OPENPROC
        PUSHR   ds,esi,edi

        ; Get a ptr to the dst bitmap
        mov     gs,word ptr lpDstDev+2
        movzx   esi,word ptr lpDstDev+0

        ; Load the Context Selector
        mov     ds,cs:[wFlatDataSel]
        mov     edi,cs:[CSpGNLogicalDevice]

        ; Load dst flags and prep some registers
        sub     ebx,ebx
        movzx   eax,gs:[esi].DIBENGINE.deFlags

        ; Filter out old style destinations
        cmp     bx,gs:[esi].DIBENGINE.deType
        je      GN_RectanglePunt

        ; Filter out based upon deFlags
        test    ah,VRAM / 256
        jz      GN_RectanglePunt
        test    ah,PALETTE_XLAT / 256
        jnz     GN_RectanglePunt

        ; Watch for busy hardware -- some other API owns the screen
        cmp     ds:[edi].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_RectanglePunt

        ; If both the pen and brush are null, then we are done!
        ; The only way the two ptrs can be equal is if they are
        ; both NULL!
        mov     eax,lpBrush
        cmp     eax,lpPen
        je      RectangleSuccess

	; Get ready to use points
	sub	esi,esi
        les     si,lpPoints
        mov    	eax,dword ptr es:[esi].RECT.left
        mov    	ebx,dword ptr es:[esi].RECT.right

	; If either the left or top has negative coordinates, then
	; we punt. Handling the interior drawing is not too bad even
	; with signed coordinates, but the border drawing is ugly.
	; Negative coordinates happen VERY rarely.
	test	eax,80008000H
	jnz	GN_RectanglePunt

	; Sometimes we get left > right or top > bottom
        cmp     ax,bx
        jge	GN_RectanglePunt
        cmp     eax,ebx
        jge	GN_RectanglePunt

	mov	dwTopLeft,eax
	mov	dwBottomRight,ebx

        ; Load up the drawmode ptr
        sub     ecx,ecx
        lfs     cx,lpDrawMode

	; Save off the hwlogdev for later
        mov     edx,ds:[edi].GNLOGDEVCX.pHWLogdevCX
	mov     pHWLogDevCXOffset,edx

        ; Get the rop
        movzx   edx,fs:[ecx].DRAWMODE.Rop2
        sub     ecx,ecx
        mov     dwRop2,edx

        ; Get the PenColor if the pen is not null
        cmp     ecx,lpPen
        je      RestP
        les     cx,lpPen
        cmp     es:[ecx].DIB_Pen.dpPenStyle,LS_SOLID
        je     	@F
        mov	lpPen,0
@@:     mov     ecx,es:[ecx].DIB_Pen.dpPenColor
RestP:  mov     dwPenColor,ecx

	; Is there a brush?
	mov	dwDrawInterior,1
	cmp	lpBrush,0
	je	UsePen

UseBrush:
        ; If the brush is hollow, then even if the rop only involves
        ; the dst (like BLACKNESS, WHITENESS, or INVERTDST), we blow
        ; off filling the interior of the rectangle. This sounds
        ; wierd but it is WHQL-correct behavior.
        sub	esi,esi
	les	si,lpBrush
	cmp	es:[esi].DIB_Brush8.dp8BrushStyle,BS_HOLLOW
	je	NoInterior

	; It is not a hollow brush. If the rop only involves the
	; dst (no pattern involvement), then skip the pattern load.
	dec	edx
        je	RetSetPat
        cmp     edx,05H
        je	RetSetPat
        cmp     edx,0AH
        je	RetSetPat
        cmp     edx,0FH
        je	RetSetPat

	; Load up the pattern
        mov     cl,byte ptr es:[esi].DIB_Brush8.dp8BrushStyle
        mov     al,byte ptr es:[esi].DIB_Brush8.dp8BrushFlags
        mov     edx,dword ptr es:[esi].DIB_Brush8.dp8BrushBits
        mov	ebx,pHWLogDevCXOffset
        PUSH	OFFSET RetSetPat
        cmp     cl,BS_HOLLOW
        jnc     @F
        test    al,COLORSOLID
        jz      HW_LoadColorPattern
        jmp     HW_LoadSolidPattern
@@:     cmp     cl,BS_PATTERN
        jc      IsHatch
        ja      GN_BitBltFail
        test    al,PATTERNMONO
        jz	HW_LoadColorPattern
IsMono:
	sub	edx,edx
	lfs	dx,lpDrawMode
	lea	esi,[esi].DIB_Brush16.dp16BrushMono
	mov	ecx,fs:[edx].DRAWMODE.TextColor
	mov	edx,fs:[edx].DRAWMODE.bkColor
	mov	eax,-1
	jmp	HW_LoadMonoPattern
IsHatch:
	sub	edx,edx
	lfs	dx,lpDrawMode
	cmp	fs:[edx].DRAWMODE.bkMode,TRANSPARENT
	sete	al
	cmp	al,1
	sbb	eax,eax
        mov     ecx,es:[esi].DIB_Brush8.dp8BgColor
        mov     edx,es:[esi].DIB_Brush8.dp8FgColor
	lea	esi,[esi].DIB_Brush16.dp16BrushMask
	jmp	HW_LoadMonoPattern

NoInterior:
	mov	dwDrawInterior,0
	jmp	RetSetPat

UsePen:
        mov	ebx,pHWLogDevCXOffset
	mov	edx,dwPenColor
	call	HW_LoadSolidPattern

RetSetPat:
	; The pattern to draw the interior of the rectangle with
	; has now been loaded. This could have been either the
	; brush or the pen. Now call the HW_DrawRectAndBorder routine.

	sub	esi,esi
	lgs	si,lpDstDev
	push	pHWLogDevCXOffset
	push	gs:[esi].DIBENGINEHDR.deBitsOffset
	push	gs:[esi].DIBENGINEHDR.deDeltaScan
	push	dwRop2

        ; pass in the clip rect as an argument
        sub	ebx,ebx
        mov	ecx,7FFF7FFFH
        cmp     ebx,lpClipRect
        je      @F
        sub	ecx,ecx
        lfs     cx,lpClipRect
        mov     ebx,dword ptr fs:[ecx].RECT.left
        mov     ecx,dword ptr fs:[ecx].RECT.right
@@:     push	ecx
	push	ebx

	; If lpPen is NULL, the border won't be drawn
	; in which case dwPenColor is irrelevant
	push	lpPen
	push	dwPenColor

        mov	ebx,dwBottomRight
        mov	eax,dwTopLeft
	push	ebx
	push	eax

	; Always bring in the bottom and right edge of the interior
	; of the rectangle by 1 pixel. If there is a border, then
	; bring in the left and top by 1 pixel as well.

        cmp     lpPen,0
        je      @F
        add     eax,10001H
@@:     sub     ebx,10001H

	push	dwDrawInterior
	push	ebx
	push	eax
	call	HW_DrawRectAndBorder

RectangleSuccess:
        mov     eax,1
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 1CH

GN_RectanglePunt:
        POPR    ds,esi,edi
        STOPPROC
PLABEL	Punt_Rectangle
        jmp     DIB_Output
CLOSEPROC

CLOSESEG _NVTEXT32

END

