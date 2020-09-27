;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnsetpal.asm
;
; Purpose:      This file holds the SetPalette, SetPaletteTranslate, and
;               the UpdateColors display driver entry points.
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
; Function:     GENERIC_SetPalette
;
; Purpose:      This function is the SetPalette display driver entry point.
;
; Arguments:    SetPalette Display Driver Stack Frame
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_SetPalette, PASCAL, FRAME, FAR16
PARMW   wStartIndex
PARMW   wNumEntries
PARMD   lpPalette
LOCALD  dwCRTCCount
LOCALD  pPalette
OPENPROC
        PUSHR   ds,esi,edi
		jmp		GN_SetPalette
GENERIC_SetPalette	ENDP

;==============================================================================
PUBLIC	GN_SetPaletteExit
GN_SetPaletteExit PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 08H
GN_SetPaletteExit ENDP

;==============================================================================
PUBLIC	GN_SetPalette
GN_SetPalette	PROC	NEAR

        ; Load up the GNLOGDEVCX
        mov     ds,cs:[wFlatDataSel]
        mov     esi,cs:[CSpGNLogicalDevice]

        ; See if screen is busy
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
		jne		GN_SetPalettePunt

        ; Get a flat ptr to the gamma ramp
        movzx   eax,word ptr lpPalette+2
        push    eax
        call    GN_GetSelectorBase
        movzx   edx,word ptr lpPalette+0
        add     eax,edx
        mov     pPalette,eax

        ; Copy the palette given into the GNLOGDEVCX palette.
        ; We need to maintain this for the DIBENGINE.
        ; We have to swap red and blue
        movzx   eax,wStartIndex
        movzx   ecx,wNumEntries
        mov     edi,pPalette
        lea     ebx,[esi][eax * 4].GNLOGDEVCX.dwPalette
@@:
        mov     eax,ds:[edi]
        add     edi,4
        bswap   eax
        ror     eax,8
        mov     ds:[ebx],eax
        add     ebx,4
        dec     ecx
        jne     @B

        ; Run through all the CRTCs attached to this logical device
        ; and program them all.
        mov     dwCRTCCount,0
        lea     edi,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        lea     ebx,[esi].GNLOGDEVCX.dwPalette
        mov     pPalette,ebx

Loopit3:
        ; Now call to set the palette on this DAC
        cmp     dword ptr ds:[edi],0
        je      @F
        push    dword ptr ds:[edi]
        push    pPalette
        movzx   eax,wStartIndex
        push    eax
        movzx   eax,wNumEntries
        push    eax
        call    GN_SetCrtcPalette
@@:
        add     edi,4
        inc     dwCRTCCount
        mov     eax,dwCRTCCount
        cmp     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      Loopit3
		jmp		GN_SetPaletteExit
GN_SetPalette	ENDP

PUBLIC  GN_SetPalettePunt
GN_SetPalettePunt	PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
        pop     eax
        push    cs:[CSlp16LogDevice]
        push    eax
        jmp     DIB_SetPaletteExt
GN_SetPalettePunt	ENDP


;==============================================================================
;
; Function:     GENERIC_SetPaletteTranslate
;
; Purpose:      This function is the SetPaletteTranslate display driver
;               entry point.
;
; Arguments:    SetPaletteTranslate Display Driver Stack Frame
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_SetPaletteTranslate, PASCAL, FRAME, FAR16
PARMD   lpTranslate
OPENPROC
        PUSHR   ds,esi,edi

        ; Call the DIBENG to do what it needs to do
        push    lpTranslate
        push    cs:[CSlp16LogDevice]
        call    DIB_SetPaletteTranslateExt

        ; If we are in a palettized mode then handle the xlat table
        mov     ds,cs:[wFlatDataSel]
        mov     esi,cs:[CSpGNLogicalDevice]
        test    ds:[esi].GNLOGDEVCX.Dibeng.deFlags,PALETTIZED
        jz      done

        ; Load up a few things and break into identity/non-identity cases
        sub     eax,eax
        lea     ebx,[esi].GNLOGDEVCX.bPaletteXlatTable
        lea     edi,[esi].GNLOGDEVCX.bPaletteInverseXlatTable
        mov     ecx,0FFH
        cmp     eax,lpTranslate
        je      isID

        ; There is an xlat table that is not the identity
        or      ds:[esi].GNLOGDEVCX.Dibeng.deFlags,PALETTE_XLAT
        sub     edx,edx
        les     dx,lpTranslate
@@:
        mov     al,es:[edx][ecx * 2]
        mov     ds:[ebx][ecx],al
        mov     ds:[edi][eax],cl
        dec     ecx
        jns     @B
        jmp     done

        ; The xlat table is NULL -- set the identity translate table
isID:
        mov     ds:[ebx][ecx],cl
        mov     ds:[edi][eax],cl
        dec     ecx
        jns     isID
        and     ds:[esi].GNLOGDEVCX.Dibeng.deFlags,NOT PALETTE_XLAT

done:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GENERIC_UpdateColors
;
; Purpose:      This function is the UpdateColors display driver entry point.
;
; Arguments:    UpdateColors Display Driver Stack Frame
;
; Returns:      None
;
;==============================================================================
DECPROC GENERIC_UpdateColors, PASCAL, FRAME, FAR16
PARMW   wStartX
PARMW   wStartY
PARMW   wExtX
PARMW   wExtY
PARMD   lpTranslate

LOCALD  lpBmp
LOCALD  lpBits
OPENPROC
        PUSHR   ds,esi,edi
	jmp	GN_UpdateColors
GENERIC_UpdateColors	ENDP

;==============================================================================
PUBLIC	GN_UpdateColorsExit
GN_UpdateColorsExit PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 0CH
GN_UpdateColorsExit ENDP

;==============================================================================
PUBLIC	GN_UpdateColors
GN_UpdateColors	PROC	NEAR
	sub	ebx,ebx
	mov	lpBmp,ebx
        ; Load up the DstDev for dispatching
        mov     ds,cs:[wFlatDataSel]
        mov     esi,cs:[CSpGNLogicalDevice]

        cmp	ds:[esi].GNLOGDEVCX.Dibeng.bRealBpp,8
	jne	GN_UpdateColorsPunt

        ; See if screen is busy
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
	jne	GN_UpdateColorsPunt

	;Ask memory size for shadow bitmap
	push	cs:[CSlp16LogDevice]
	push	word ptr 5					;OBJ_BITMAP
	push	cs:[CSlp16LogDevice]
	push	ebx
	push	ebx
	push	cs:[CSlp16LogDevice]
	call	DIB_RealizeObjectExt
	or	eax,eax
	jz	GN_UpdateColorsPunt

	;Allocate shadow memory
	call	GN_MemoryAlloc
	or	eax,eax
	jz	GN_UpdateColorsPunt
	shl	eax,10H
	mov	lpBmp,eax
	mov	edi,GN_UpdateColorsPunt

	;Create shadow bitmap in system memory
	sub	ebx,ebx
	push	cs:[CSlp16LogDevice]
	push	word ptr 5					;OBJ_BITMAP
	push	cs:[CSlp16LogDevice]
	push	lpBmp
	push	ebx
	push	cs:[CSlp16LogDevice]
	call	DIB_RealizeObjectExt
	or	eax,eax
	jz	FreeRes

	sub	ebx,ebx
	mov	es,word ptr lpBmp+2
	movzx	eax,word ptr es:[ebx].DIBENGINE.deBits+4
	shl	eax,10H
	mov	ax,word ptr es:[ebx].DIBENGINE.deBits
	mov	lpBits,eax

	;Copy screen --> shadow
        push    cs:[CSpGNLogicalDevice]
        push    lpBmp
        push    ebx
	movzx   eax,word ptr lpBits+2
        mov	ecx,8						;Bpp
        push    eax
	movzx   eax,word ptr lpBits
        push    eax
        push    ecx
        movzx	eax,es:[ebx].DIBENGINE.deWidthBytes
        push    eax
        movzx	eax,wStartX
        push    eax
        movzx	eax,wStartY
        push    eax

        push    ebx						;lpSrcDev
        movzx   eax,word ptr ds:[esi].GNLOGDEVCX.Dibeng.deBitsSelector
        push    eax
        push    dword ptr ds:[esi].GNLOGDEVCX.Dibeng.deBitsOffset
        push    ecx
        push    ds:[esi].GNLOGDEVCX.Dibeng.deDeltaScan
        movzx	eax,wStartX
        push    eax
        movzx	eax,wStartY
        push    eax
        movzx   eax,wExtX
        push    eax
        movzx   eax,wExtY
        push    eax
        mov     ecx,0CC0000H
        push    ecx
        push    ebx
        push    ebx
        movzx   ebx,cs:bTableBppToIndex[8]
	mov	edx,ebx
        imul    ebx,MAX_BPP_INDICES
        add     ebx,edx
        mov     ecx,cs:dwTableBppToXlatFunc[ebx * 4]
	push	ecx
        call    GN_BitBltSrcVramDstSystem
        or      eax,eax
        jbe	FreeRes

	;Copy shadow <-- screen
        mov     ds,cs:[wFlatDataSel]
        sub     ebx,ebx
        push    cs:[CSpGNLogicalDevice]
        movzx   eax,word ptr ds:[esi].GNLOGDEVCX.Dibeng.deBitsSelector
        mov	ecx,8						;Bpp
        push    eax
        push    dword ptr ds:[esi].GNLOGDEVCX.Dibeng.deBitsOffset
        push    ecx
        push    ds:[esi].GNLOGDEVCX.Dibeng.deDeltaScan
        movzx	eax,wStartX
        push    eax
        movzx	eax,wStartY
        push    eax
        movzx   eax,word ptr lpBits+2
        movzx	ecx,word ptr lpBits+0
        push    eax
        push    ecx
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deBitsPixel
        movzx	ecx,ds:[esi].GNLOGDEVCX.Dibeng.deWidthBytes
        push    eax
        push    ecx
        movzx	eax,wStartX
        push    eax
        movzx	eax,wStartY
        push    eax
        movzx   eax,wExtX
        push    eax
        movzx   eax,wExtY
        push    eax
        mov     ecx,0CC0000H
        push    ecx
        push    ebx						;lpBrush
        push    ebx						;lpDrawMode
        push    2						;size color table entry in bytes
        push    100H						;num color table entries
        push    lpTranslate					;lpColorTable
        push    ebx
        call    GN_BitBltSrcColorDstVram
        or      eax,eax
        jbe	FreeRes
	mov	edi,GN_UpdateColorsExit

FreeRes:
	;Delete shadow bitmap if exist
	mov	eax,lpBmp
	or	eax,eax
	je	JustFreeMem
        sub     ebx,ebx
	push	cs:[CSlp16LogDevice]
	push	word ptr -5					;-OBJ_PBITMAP
	push	eax
	push	ebx
	push	ebx
	push	cs:[CSlp16LogDevice]
	call	DIB_RealizeObjectExt

JustFreeMem:
	;Free shadow memory
	mov	eax,lpBmp
	shr	eax,10H
	call	GN_MemoryFree
	mov	eax,1
	jmp	edi
GN_UpdateColors	ENDP

PUBLIC  GN_UpdateColorsPunt
GN_UpdateColorsPunt PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
        pop     eax
        push    cs:[CSlp16LogDevice]
        push    eax
PLABEL	Punt_UpdateColors
        jmp     DIB_UpdateColorsExt
GN_UpdateColorsPunt	ENDP

CLOSESEG _NVTEXT32

END

