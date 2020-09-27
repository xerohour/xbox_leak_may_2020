;==============================================================================
;
; Copyright (C) 1998, NVidia Coporation
;
; File:         gnwrap.asm
;
; Purpose:      This file holds the entry points to drawing functions
;		that may require cursor exclusion / unexclusion. The
;		idea is to see if exclusion is necessary. If not jump
;		to the real code. If it is, then do the exclusion,
;		call the real code, do the unexclusion and return.
;
;==============================================================================
.586
incDrawMode = 1
incDevice   = 1
incLogical  = 1
incOutput = 1
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

IsItScreen	MACRO	bm,jump
	cmp	bm.DIBENGINE.deType,0
	je	jump
	mov	ax,bm.DIBENGINE.deFlags
	and	ax,VRAM + OFFSCREEN + PALETTE_XLAT
	cmp	ax,VRAM
	jne	jump
	cmp	bm.DIBENGINE.deBeginAccess,0
	je	jump
	ENDM

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     BitBlt_WithExclude
;
; Purpose:      This function is the BitBlt display driver entry point
;		when we are running a software cursor.
;
; Arguments:    BitBlt Display Driver Stack Frame
;
; Returns:      ax      0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC BitBlt_WithExclude, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wDstX
PARMW   wDstY
PARMD   lpSrcDev
PARMW   wSrcX
PARMW   wSrcY
PARMW   wExtX
PARMW   wExtY
PARMD   dwRop3
PARMD   lpBrush
PARMD   lpDrawMode
LOCALW	wExclude
LOCALW	wRet
OPENPROC
	PUSHR	ds,si
	mov	wExclude,0

if 0;!!
	; Check the rop and see what type of exclusion to check for
        movzx   bx,byte ptr [dwRop3 + 2]
	mov	ax,cs:[wNVTEXT32_Alias_In_NVTEXT16]
	mov	es,ax
        movzx  	ax,es:Rop3Table[bx]
        cmp	ax,PAD
        jae	SrcAndDstCheck
else
	cmp	lpSrcDev,0
	jne	SrcAndDstCheck
endif

JustDstCheck:
	les	bx,lpDstDev
	IsItScreen es:[bx],DoCall

JustDst:
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	mov	ax,wDstX
	mov	cx,wDstY
	push	ax
	push	cx
	add	ax,wExtX
	add	cx,wExtY
	push	ax
	push	cx
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR es:[bx].DIBENGINE.deBeginAccess
	jmp	DoCall_WithExclude

SrcAndDstCheck:
	; See if we have to exclude the src rectangle also
	lfs	si,lpSrcDev
	IsItScreen fs:[si],JustDstCheck

	; Clip the src rect
	call	BitBlt_ClipSrc
	or	ax,ax
	mov	ax,1
	je	DoneIt

	; We have to do the src rect.  How about the dst?
	les	bx,lpDstDev
	IsItScreen es:[bx],JustSrc

SrcAndDst:
	; We need to exclude based on both the src and dst rectangles
        push	cs:[CSlp16LogDevice_In_NVTEXT16]
	mov	ax,wDstX
	mov	si,wSrcX
	mov	cx,wDstY
	mov	dx,wSrcY

	; Order the src and dst X,Y coordinates
	cmp	ax,si
	jc	@F
	xchg	ax,si
@@:	cmp	cx,dx
	jc	@F
	xchg	cx,dx
@@:
	push	ax
	push	cx

	add	si,wExtX
	add	dx,wExtY
	push	si
	push	dx
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR es:[bx].DIBENGINE.deBeginAccess
	jmp	DoCall_WithExclude

JustSrc:
        push	cs:[CSlp16LogDevice_In_NVTEXT16]
	mov	ax,wSrcX
	mov	cx,wSrcY
	push	ax
	push	cx
	add	ax,wExtX
	add	cx,wExtY
	push	ax
	push	cx
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR fs:[si].DIBENGINE.deBeginAccess

DoCall_WithExclude:
	mov	wExclude,1

DoCall:
	push	lpDstDev
	push	wDstX
	push	wDstY
	push	lpSrcDev
	push	wSrcX
	push	wSrcY
	push	wExtX
	push	wExtY
	push	dwRop3
	push	lpBrush
	push	lpDrawMode

	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	call	DIB_BitBlt
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_BitBlt

	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoneIt

	; Do we need to unexclude?
	cmp	wExclude,0
	je	DoneIt

	; Yes so do it. Don't forget to save return value
	mov	wRet,ax
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deEndAccess
	mov	ax,wRet

DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     BitBlt_ClipSrc
;
; Purpose:      This function clips the src adjusting the stack parameters
;               for BitBlt appropriately to represent the clipped coords
;               and clipped extents.
;
; Arguments:    BitBlt Display Driver Stack Frame
;               fs:si  lpSrcDev
;
; Returns:      eax     1 - there is something to draw
;                       0 - there is nothing to draw
;
; Preserve:     ds,esi,es,edi,fs
;==============================================================================
PUBLIC  BitBlt_ClipSrc
BitBlt_ClipSrc	PROC    NEAR

        mov   	ax,wSrcX
        mov   	cx,wSrcY
        mov   	bx,fs:[si].DIBENGINE.deWidth
        mov   	dx,fs:[si].DIBENGINE.deHeight
        or      ax,ax
        js      ClipLeft
cld1:   or      cx,cx
        js      ClipTop
clt:    add     ax,wExtX
        add     cx,wExtY
        sub     ax,bx
        jg      ClipRight
clr:    sub     cx,dx
        jg      ClipBottom
clb:    mov     ax,1
        ret

ClipLeft:
        add     wExtX,ax
        jle     ClipDone
        sub     wDstX,ax
        sub     ax,ax
        mov     wSrcX,ax
        jmp     cld1

ClipTop:
        add     wExtY,cx
        jle     ClipDone
        sub     wDstY,cx
        sub     cx,cx
        mov     wSrcY,cx
        jmp     clt

ClipRight:
        sub     wExtX,ax
        jg      clr
        jmp     ClipDone

ClipBottom:
        sub     wExtY,cx
        jg      clb

ClipDone:
        sub     ax,ax
        ret

BitBlt_ClipSrc	ENDP


;==============================================================================
;
; Function:     BitmapBits_WithExclude
;
; Purpose:      This function is the BitmapBits display driver
;               entry point
;		when we are running a software cursor.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;
; Returns:      ax      0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;
;==============================================================================
DECPROC BitmapBits_WithExclude, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMD   dwFlags
PARMD   dwCount
PARMD   lpBits
OPENPROC
	PUSHR	ds,si

	push	lpDstDev
	push	dwFlags
	push	dwCount
	push	lpBits

	; We don't need exclude here because BitmapBits never use screen.
	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	call	DIB_BitmapBits
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_BitmapBits

	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT

DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     DibBlt_WithExclude
;
; Purpose:      This function is the DibBlt display driver
;               entry point
;		when we are running a software cursor.
;
; Arguments:    DibBlt Display Driver Stack Frame
;
; Returns:      ax      0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;
;==============================================================================
DECPROC DibBlt_WithExclude, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   fGet
PARMW   iStart
PARMW   cScans
PARMD   lpDIBBits
PARMD   lpDIBInfo
PARMD   lpDrawMode
PARMD   lpTranslate
OPENPROC
	PUSHR	ds,si

	push	lpDstDev
	push	fGet
	push	iStart
	push	cScans
	push	lpDIBBits
	push	lpDIBInfo
	push	lpDrawMode
	push	lpTranslate

	; We don't need exclude here because DibBlt never use screen.
	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	les	bx,lpDstDev
        ; The DIBENG DibBltExt takes an extra parameter which indicates
        ; whether we are running in a palettized mode right now.
        test    es:[bx].DIBENGINEHDR.deFlags,PALETTE_XLAT
        setne   al
        movzx   ax,al
	push	ax
	call	DIB_DibBltExt
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_DibBlt

	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT

DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     ExtTextOut_WithExclude
;
; Purpose:      This function is the ExtTextOut display driver entry point
;		when we are running a software cursor.
;
; Arguments:    ExtTextOut Display Driver Stack Frame
;
; Returns:      This function returns one of the following values:
;
;               If wCount is negative and the function is successful,
;               it returns the X extent in AX,and the Y extent in DX.
;               If wCount is negative and the function is unsuccessful,
;               it returns the 7FFFH in AX,and wDstY in DX.
;
;               If wCount is 0 or positive and the the function is
;               successful,it returns DX:AX set to 1.
;               If wCount is 0 or positive and the the function is
;               unsuccessful,it returns DX:AX set to 80000000H.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC ExtTextOut_WithExclude, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wDstX
PARMW   wDstY
PARMD   lpClipRect
PARMD   lpString
PARMW   wCount
PARMD   lpFontInfo
PARMD   lpDrawMode
PARMD   lpTextXForm
PARMD   lpKerning
PARMD   lpOpaqueRects
PARMW   wOptions
LOCALW	wExclude
LOCALW	wRet1
LOCALW	wRet2
OPENPROC
	PUSHR	ds,si
	mov	wExclude,0

	cmp	wCount,0
	jl	DoCall
	les	bx,lpDstDev
	IsItScreen es:[bx],DoCall

	; Here is a problem! I don't know what the exclusion rect is!
	; Oh well, just do the scan for the entire extent of the bitmap.
	lds	si,lpClipRect
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	push	ds:[si].RECT.left
	push	ds:[si].RECT.top
	push	ds:[si].RECT.right
	push	ds:[si].RECT.bottom
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR es:[bx].DIBENGINE.deBeginAccess

DoCall_WithExclude:
	mov	wExclude,1

DoCall:
	push	lpDstDev
	push	wDstX
	push	wDstY
	push	lpClipRect
	push	lpString
	push	wCount
	push	lpFontInfo
	push	lpDrawMode
	push	lpTextXForm
	push	lpKerning
	push	lpOpaqueRects
	push	wOptions

	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	call	DIB_ExtTextOut
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_ExtTextOut

	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoneIt

	; Do we need to unexclude?
	cmp	wExclude,0
	je	DoneIt

	; Yes so do it. Don't forget to save return value
	mov	wRet1,ax
	mov	wRet2,dx
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deEndAccess
	mov	ax,wRet1
	mov	dx,wRet2
DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     Output_WithExclude
;
; Purpose:      This function is the Output display driver
;               entry point
;		when we are running a software cursor.
;
; Arguments:    Output Display Driver Stack Frame
;
; Returns:      ax      0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC Output_WithExclude, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wStyle
PARMW   wCount
PARMD   lpPoints
PARMD   lpPen
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   lpClipRect
LOCALW	wExclude
LOCALW	wRet
OPENPROC
	PUSHR	ds,si
	mov	wExclude,0

	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]

	IsItScreen ds:[si],DoCall

	; We dont need to exclude if we are in scan sequence
        cmp	ds:[si].GNLOGDEVCX.bInScanSequence,0
	je	@F

	; We need to unexclude cursor after endscan
	cmp	wStyle,OS_ENDNSCAN
	je	DoCall_WithExclude
	jmp	DoCall

@@:

	; Exclude the save / restore rect
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	cmp	lpClipRect,0
	je	NoClipRect
	les	bx,lpClipRect

	mov	ax,es:[bx].RECT.left
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,es:[bx].RECT.top
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,es:[bx].RECT.right
	cmp	ax,ds:[si].DIBENGINE.deWidth
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deWidth
@@:	push	ax

	mov	ax,es:[bx].RECT.bottom
	cmp	ax,ds:[si].DIBENGINE.deHeight
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deHeight
@@:	push	ax
	jmp	@F

NoClipRect:
	sub	ax,ax
	push	ax
	push	ax
	mov	ax,ds:[si].DIBENGINE.deWidth
	push	ax
	mov	ax,ds:[si].DIBENGINE.deHeight
	push	ax

@@:
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deBeginAccess

DoCall_WithExclude:
	mov	wExclude,1

DoCall:
	push	lpDstDev
	push	wStyle
	push	wCount
	push	lpPoints
	push	lpPen
	push	lpBrush
	push	lpDrawMode
	push	lpClipRect

	; Busy bit already set if we are in scan sequence
        cmp	ds:[si].GNLOGDEVCX.bInScanSequence,0
	jne	DoRegularCall

	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	call	DIB_Output
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_Output

	; We need to keep busy bit until scan sequence end.
        cmp	ds:[si].GNLOGDEVCX.bInScanSequence,0
	jne	KeepBusy

	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoneIt

	; Do we need to unexclude?
	cmp	wExclude,0
	je	DoneIt

	; Yes so do it. Don't forget to save return value
	mov	wRet,ax
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deEndAccess
	mov	ax,wRet
KeepBusy:
DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     Pixel_WithExclude
;
; Purpose:      This function is the Pixel display driver entry point
;		when we are running a software cursor.
;
; Arguments:    Pixel Display driver stack FRAME
;
; Returns:      If lpDrawMode is NULL then
;                       pixel value at coords wDstX,wDstY if succeesful
;                       80000000H is failure
;               If lpDrawMode is NOT NULL then
;                       00000001H is succeesful
;                       80000000H is failure
;
;==============================================================================
DECPROC Pixel_WithExclude, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wDstX
PARMW   wDstY
PARMD   dwPhysColor
PARMD   lpDrawMode
LOCALW	wExclude
LOCALW	wRet1
LOCALW	wRet2
OPENPROC
	PUSHR	ds,si
	mov	wExclude,0

	les	bx,lpDstDev
	IsItScreen es:[bx],DoCall

	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	mov	ax,wDstX
	mov	cx,wDstY
	push	ax
	push	cx
	inc	ax
	inc	cx
	push	ax
	push	cx
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR es:[bx].DIBENGINE.deBeginAccess

DoCall_WithExclude:
	mov	wExclude,1

DoCall:
	push	lpDstDev
	push	wDstX
	push	wDstY
	push	dwPhysColor
	push	lpDrawMode

	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	call	DIB_Pixel
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_Pixel

	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoneIt

	; Do we need to unexclude?
	cmp	wExclude,0
	je	DoneIt

	; Yes so do it. Don't forget to save return value
	mov	wRet1,ax
	mov	wRet2,dx
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deEndAccess
	mov	ax,wRet1
	mov	dx,wRet2
DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     ScanLR_WithExclude
;
; Purpose:      This function is the ScanLR display driver entry point
;		when we are running a software cursor.
;
;==============================================================================
DECPROC	ScanLR_WithExclude, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wX
PARMW   wY
PARMD   dwPhysColor
PARMW   wStyle
LOCALW	wExclude
LOCALW	wRet1
LOCALW	wRet2
OPENPROC
	PUSHR	ds,si
	mov	wExclude,0

	les	bx,lpDstDev
	IsItScreen es:[bx],DoCall

	; Here is a problem! I don't know what the exclusion rect is!
	; Oh well, just do the scan for the entire extent of the bitmap.
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	mov	ax,wX
	mov	cx,wY
	push	ax
	push	cx
	mov	ax,es:[bx].DIBENGINE.deWidth
	inc	cx
	push	ax
	push	cx
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR es:[bx].DIBENGINE.deBeginAccess


DoCall_WithExclude:
	mov	wExclude,1

DoCall:
	push	lpDstDev
	push	wX
	push	wY
	push	dwPhysColor
	push	wStyle

	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	call	DIB_ScanLR
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_ScanLR

	; Turn off BUSY bit because we don't use h/w here
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoneIt

	; Do we need to unexclude?
	cmp	wExclude,0
	je	DoneIt

	; Yes so do it. Don't forget to save return value
	mov	wRet1,ax
	mov	wRet2,dx
	les	bx,lpDstDev
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR es:[bx].DIBENGINE.deEndAccess
	mov	ax,wRet1
	mov	dx,wRet2
DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     SetDIBitsToDevice_WithExclude
;
; Purpose:      This function is the SetDIBitsToDevice driver entry point
;		when we are running a software cursor.
;
; Arguments:    SetDIBitsToDevice Display Driver Stack Frame
;
; Returns:      ax      number of scans copied if successful
;                       0 if not successful
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC SetDIBitsToDevice_WithExclude, PASCAL, FRAME, FAR16
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
LOCALW	wExclude
LOCALW	wRet1
LOCALW	wRet2
OPENPROC
	PUSHR	ds,si
	mov	wExclude,0

	lds	si,lpDstDev
	IsItScreen ds:[si],DoCall

	; The lpClipRect may not be entirely accurate. To get it correct,
	; I really need to clip against the src rectangle. However, why
	; bother? Just make sure the clip coordinates are bounded by the
	; screen borders. Then it will be close enough -- that is we
	; might be excluding the cursor from a little larger an area than
	; necessary, but it won;t hurt.
	les	bx,lpClipRect
	push	cs:[CSlp16LogDevice_In_NVTEXT16]

	mov	ax,es:[bx].RECT.left
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,es:[bx].RECT.top
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,es:[bx].RECT.right
	cmp	ax,ds:[si].DIBENGINE.deWidth
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deWidth
@@:	push	ax

	mov	ax,es:[bx].RECT.bottom
	cmp	ax,ds:[si].DIBENGINE.deHeight
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deHeight
@@:	push	ax

	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deBeginAccess

DoCall_WithExclude:
	mov	wExclude,1

DoCall:
	push	lpDstDev
	push	wDstX
	push	wDstY
	push	wInitScan
	push	wNumScans
	push	lpClipRect
	push	lpDrawMode
	push	lpDIBBits
	push	lpDIBInfo
	push	lpTranslate

	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	call	DIB_DibToDevice
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_SetDIBitsToDevice

	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoneIt

	; Do we need to unexclude?
	cmp	wExclude,0
	je	DoneIt

	; Yes so do it. Don't forget to save return value
	mov	wRet1,ax
	mov	wRet2,dx
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deEndAccess
	mov	ax,wRet1
	mov	dx,wRet2
DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     StretchBlt_WithExclude
;
; Purpose:      This function is the StretchBlt display driver entry point.
;		when we are running a software cursor.
;
;==============================================================================
DECPROC StretchBlt_WithExclude, PASCAL, FRAME, FAR16
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
LOCALW	wExclude
LOCALW	wRet
OPENPROC
	PUSHR	ds,si
	mov	wExclude,0

if 0;!!
	; Check the rop and see what type of exclusion to check for
        movzx   bx,byte ptr [dwRop3 + 2]
	mov	ax,cs:[wNVTEXT32_Alias_In_NVTEXT16]
	mov	es,ax
        movzx  	ax,es:Rop3Table[bx]
        cmp	ax,PAD
        jae	SrcAndDstCheck
else
	cmp	lpSrcDev,0
	jne	SrcAndDstCheck
endif

JustDstCheck:
	lds	si,lpDstDev
	IsItScreen ds:[si],DoCall

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Just exclude the dst rect. Use cliprect for this but bound it
	; to the bitmap coords
	les	bx,lpClipRect
	push	cs:[CSlp16LogDevice_In_NVTEXT16]

	mov	ax,es:[bx].RECT.left
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,es:[bx].RECT.top
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,es:[bx].RECT.right
	cmp	ax,ds:[si].DIBENGINE.deWidth
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deWidth
@@:	push	ax

	mov	ax,es:[bx].RECT.bottom
	cmp	ax,ds:[si].DIBENGINE.deHeight
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deHeight
@@:	push	ax

	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deBeginAccess
	jmp	DoCall_WithExclude
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


SrcAndDstCheck:
	; See if we have to exclude the src rectangle also
	lfs	si,lpSrcDev
	IsItScreen fs:[si],JustDstCheck

	; Clip the src rect
	call	StretchBlt_ClipSrc
	or	ax,ax
	mov	ax,1
	je	DoneIt

	; We have to do the src rect.  How about the dst?
	les	bx,lpDstDev
	IsItScreen es:[bx],JustSrc

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SrcAndDst:
	; We need to exclude based on both the src and dst rectangles
	; I don't actually do the clip to generate the smallest possible
	; nounding rectangle. This might result in occassionally
	; excluding the cursor when we don't need to, but it won't hurt.
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	push	cs:[CSlp16LogDevice_In_NVTEXT16]

	; Order the src and dst X coordinates
	mov	ax,wDstX
	mov	bx,wSrcX
	cmp	ax,bx
	jl	@F
	mov	ax,bx
@@:	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	; Order the src and dst Y coordinates
	mov	ax,wDstY
	mov	bx,wSrcY
	cmp	ax,bx
	jl	@F
	mov	ax,bx
@@:	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	; Now find the larger of the src/dst right edges
	mov	ax,wDstX
	mov	bx,wSrcX
	add	ax,wDstExtX
	add	bx,wSrcExtX
	cmp	ax,bx
	jg	@F
	mov	ax,bx
@@:	cmp	ax,ds:[si].DIBENGINE.deWidth
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deWidth
@@:	push	ax

	; Now find the larger of the src/dst bottom edges
	mov	ax,wDstY
	mov	bx,wSrcY
	add	ax,wDstExtY
	add	bx,wSrcExtY
	cmp	ax,bx
	jg	@F
	mov	ax,bx
@@:	cmp	ax,ds:[si].DIBENGINE.deHeight
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deHeight
@@:	push	ax

	sub	bx,bx
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR es:[bx].DIBENGINE.deBeginAccess
	jmp	DoCall_WithExclude
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
JustSrc:
	; Just exclude the dst rect. Use cliprect for this but bound it
	; to the bitmap coords
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	les	bx,lpClipRect
	push	cs:[CSlp16LogDevice_In_NVTEXT16]

	mov	ax,wSrcX
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,wSrcY
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,wSrcX
	add	ax,wSrcExtX
	cmp	ax,ds:[si].DIBENGINE.deWidth
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deWidth
@@:	push	ax

	mov	ax,wSrcY
	add	ax,wSrcExtY
	cmp	ax,ds:[si].DIBENGINE.deHeight
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deHeight
@@:	push	ax

	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deBeginAccess
	jmp	DoCall_WithExclude
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


DoCall_WithExclude:
	mov	wExclude,1

DoCall:
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

	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	call	DIB_StretchBlt
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_StretchBlt

	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoneIt

	; Do we need to unexclude?
	cmp	wExclude,0
	je	DoneIt

	; Yes so do it. Don't forget to save return value
	mov	wRet,ax
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deEndAccess
	mov	ax,wRet

DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     StretchBlt_ClipSrc
;
; Purpose:      This function clips the src adjusting the stack parameters
;               for StrBlt appropriately to represent the clipped coords
;               and clipped extents.
;
; Arguments:    StretchBlt Display Driver Stack Frame
;               fs:si  lpSrcDev
;
; Returns:      eax     1 - there is something to draw
;                       0 - there is nothing to draw
;
; Preserve:     ds,esi,es,edi,fs
;==============================================================================
PUBLIC	StretchBlt_ClipSrc
StretchBlt_ClipSrc	PROC	NEAR

        mov   	ax,wSrcX
        mov   	cx,wSrcY
        mov   	bx,fs:[si].DIBENGINE.deWidth
        mov   	dx,fs:[si].DIBENGINE.deHeight
        or      ax,ax
        js      ClipLeft
cld1:   or      cx,cx
        js      ClipTop
clt:    add     ax,wExtX
        add     cx,wExtY
        sub     ax,bx
        jg      ClipRight
clr:    sub     cx,dx
        jg      ClipBottom
clb:    mov     ax,1
        ret

ClipLeft:
        add     wExtX,ax
        jle     ClipDone
        sub     wDstX,ax
        sub     ax,ax
        mov     wSrcX,ax
        jmp     cld1

ClipTop:
        add     wExtY,cx
        jle     ClipDone
        sub     wDstY,cx
        sub     cx,cx
        mov     wSrcY,cx
        jmp     clt

ClipRight:
        sub     wExtX,ax
        jg      clr
        jmp     ClipDone

ClipBottom:
        sub     wExtY,cx
        jg      clb

ClipDone:
        sub     ax,ax
        ret

StretchBlt_ClipSrc	ENDP


;==============================================================================
;
; Function:     StretchDIBits_WithExclude
;
; Purpose:      This function is the StretchDIBits display driver entry point
;		when we are running a software cursor.
;
; Arguments:    StretchDIBits Display Driver Stack Frame
;
; Returns:      ax      number of scans copied if successful
;                       0 if not successful
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC StretchDIBits_WithExclude, PASCAL, FRAME, FAR16
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
LOCALW	wExclude
LOCALW	wRet1
LOCALW	wRet2
OPENPROC
	PUSHR	ds,si
	mov	wExclude,0

	lds	si,lpDstDev
	IsItScreen ds:[si],DoCall

	; The lpClipRect may not be entirely accurate. To get it correct,
	; I really need to clip against the src rectangle. However, why
	; bother? Just make sure the clip coordinates are bounded by the
	; screen borders. Then it will be close enough -- that is we
	; might be excluding the cursor from a little larger an area than
	; necessary, but it won't hurt.
	les	bx,lpClipRect
	push	cs:[CSlp16LogDevice_In_NVTEXT16]

	mov	ax,es:[bx].RECT.left
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,es:[bx].RECT.top
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,es:[bx].RECT.right
	cmp	ax,ds:[si].DIBENGINE.deWidth
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deWidth
@@:	push	ax

	mov	ax,es:[bx].RECT.bottom
	cmp	ax,ds:[si].DIBENGINE.deHeight
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deHeight
@@:	push	ax

	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deBeginAccess

DoCall_WithExclude:
	mov	wExclude,1

DoCall:
	push	lpDstDev
	push	fGet
	push	wDstX
	push	wDstY
	push	wDstExtX
	push	wDstExtY
	push	wSrcX
	push	wSrcY
	push	wSrcExtX
	push	wSrcExtY
	push	lpDIBBits
	push	lpDIBInfo
	push	lpTranslate
	push	dwRop3
	push	lpBrush
	push	lpDrawMode
	push	lpClipRect

	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	call	DIB_StretchDIBits
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_StretchDIBits

	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoneIt

	; Do we need to unexclude?
	cmp	wExclude,0
	je	DoneIt

	; Yes so do it. Don't forget to save return value
	mov	wRet1,ax
	mov	wRet2,dx
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deEndAccess
	mov	ax,wRet1
	mov	dx,wRet2
DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     StrBlt_WithExclude
;
; Purpose:      This function is the StrBlt display driver
;               entry point
;		when we are running a software cursor.
;
; Arguments:    StrBlt Display Driver Stack Frame
;
; Returns:      ax      0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;
;==============================================================================
DECPROC StrBlt_WithExclude, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMW   wDstX
;PARMW   wDstY
;PARMD   lpClipRect
;PARMD   lpString
;PARMW   wCount
;PARMD   lpFontInfo
;PARMD   lpDrawMode
;PARMD   lpTextXForm
OPENPROC
        sub     ebx,ebx
        pop     eax
        push    ebx
        push    ebx
        push    bx
        push    eax
        jmp     ExtTextOut_WithExclude
CLOSEPROC


;==============================================================================
;
; Function:     UpdateColors_WithExclude
;
; Purpose:      This function is the UpdateColors display driver entry point
;		when we are running a software cursor.
;
; Arguments:    UpdateColors Display Driver Stack Frame
;
; Returns:      None
;
;==============================================================================
DECPROC UpdateColors_WithExclude, PASCAL, FRAME, FAR16
PARMW   wStartX
PARMW   wStartY
PARMW   wExtX
PARMW   wExtY
PARMD   lpTranslate
LOCALW	wExclude
LOCALW	wRet1
LOCALW	wRet2
OPENPROC
	PUSHR	ds,si
	mov	wExclude,0

	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]
	IsItScreen ds:[si],DoCall

	; Exclude the update rectangle
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	mov	ax,wStartX
	mov	bx,wStartX
	push	ax
	push	bx
	add	ax,wExtX
	add	bx,wExtX
	push	ax
	push	bx
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deBeginAccess

DoCall_WithExclude:
	mov	wExclude,1

DoCall:
	push	wStartX
	push	wStartY
	push	wExtX
	push	wExtY
	push	lpTranslate

	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoRegularCall

	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	call	DIB_UpdateColorsExt
	jmp	DoneIt

DoRegularCall:
	call	GENERIC_UpdateColors

	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoneIt

	; Do we need to unexclude?
	cmp	wExclude,0
	je	DoneIt

	; Yes so do it. Don't forget to save return value
	mov	wRet1,ax
	mov	wRet2,dx
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deEndAccess
	mov	ax,wRet1
	mov	dx,wRet2
DoneIt:
	POPR	ds,si
CLOSEPROC


;==============================================================================
;
; Function:     SaveScreenBitmap_WithExclude
;
; Purpose:      This function is the SaveScreenBitmap display driver
;               entry point
;		when we are running a software cursor.
;
; Arguments:    SaveScreenBitmap Display Driver Stack Frame
;
; Returns:      ax      0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;==============================================================================
; These aren't in any API header file so include them here
SAVE_AREA       EQU     0
RESTORE_AREA    EQU     1
DISCARD_AREA    EQU     2

DECPROC SaveScreenBitmap_WithExclude, PASCAL, FRAME, FAR16
PARMD   lpRect
PARMW   wCommand
LOCALW	wExclude
LOCALW	wRet
OPENPROC
	PUSHR	ds,si
	mov	wExclude,0
	lds	si,cs:[CSlp16LogDevice_In_NVTEXT16]

	; Discard does not require exclude
        cmp     wCommand,DISCARD_AREA
        je	DoCall

	IsItScreen ds:[si],DoCall

	; Exclude the save / restore rect
	les	bx,lpRect
	push	cs:[CSlp16LogDevice_In_NVTEXT16]

	mov	ax,es:[bx].RECT.left
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,es:[bx].RECT.top
	or	ax,ax
	jg	@F
	sub	ax,ax
@@:	push	ax

	mov	ax,es:[bx].RECT.right
	cmp	ax,ds:[si].DIBENGINE.deWidth
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deWidth
@@:	push	ax

	mov	ax,es:[bx].RECT.bottom
	cmp	ax,ds:[si].DIBENGINE.deHeight
	jb	@F
	mov	ax,ds:[si].DIBENGINE.deHeight
@@:	push	ax

	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deBeginAccess

	mov	wExclude,1

	; Prepare return value if busy bit set.
	xor	ax,ax

	; We must set the BUSY bit with a software cursor when we use h/w
	; because that is what the DIBENG uses to know when it can draw
	; and when it can't.
	bts	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jc	DoneIt

DoCall:
	push	lpRect
	push	wCommand
	call	GENERIC_SaveScreenBitmap

DoneCall:
	; Turn off BUSY bit
	btr	ds:[si].DIBENGINE.deFlags,BUSY_BIT
	jnc	DoneIt

	; Do we need to unexclude?
	cmp	wExclude,0
	je	DoneIt

	; Yes so do it. Don't forget to save return value
	mov	wRet,ax
	push	cs:[CSlp16LogDevice_In_NVTEXT16]
	pushw	FB_ACCESS + CURSOREXCLUDE
	call	FFP16 PTR ds:[si].DIBENGINE.deEndAccess
	mov	ax,wRet
DoneIt:
	POPR	ds,si
CLOSEPROC

;==============================================================================
;
; Function:     Wrap_BitBlt
;
; Purpose:      This function is junction to dibengine.
;               It clear busy bit before jmp because dibengine
;		return error if this bit set.
;
; Arguments:    none
;
; Returns:      none
;
;==============================================================================
DECPROC Wrap_BitBlt, PASCAL, NOFRAME, FAR16
OPENPROC
	les	bx,cs:[CSlp16LogDevice_In_NVTEXT16]
	and	es:[bx].DIBENGINE.deFlags,NOT BUSY
        jmp     DIB_BitBlt
CLOSEPROC

;==============================================================================
;
; Function:     Wrap_BitmapBits
;
; Purpose:      This function is junction to dibengine.
;               It clear busy bit before jmp because dibengine
;		return error if this bit set.
;
; Arguments:    none
;
; Returns:      none
;
;==============================================================================
DECPROC Wrap_BitmapBits, PASCAL, NOFRAME, FAR16
OPENPROC
	les	bx,cs:[CSlp16LogDevice_In_NVTEXT16]
	and	es:[bx].DIBENGINE.deFlags,NOT BUSY
        jmp     DIB_BitmapBits
CLOSEPROC

;==============================================================================
;
; Function:     Wrap_ExtTextOut
;
; Purpose:      This function is junction to dibengine.
;               It clear busy bit before jmp because dibengine
;		return error if this bit set.
;
; Arguments:    none
;
; Returns:      none
;
;==============================================================================
DECPROC Wrap_ExtTextOut, PASCAL, NOFRAME, FAR16
OPENPROC
	les	bx,cs:[CSlp16LogDevice_In_NVTEXT16]
	and	es:[bx].DIBENGINE.deFlags,NOT BUSY
        jmp     DIB_ExtTextOut
CLOSEPROC

;==============================================================================
;
; Function:     Wrap_DibBltExt
;
; Purpose:      This function is junction to dibengine.
;               It clear busy bit before jmp because dibengine
;		return error if this bit set.
;
; Arguments:    none
;
; Returns:      none
;
;==============================================================================
DECPROC Wrap_DibBltExt, PASCAL, NOFRAME, FAR16
OPENPROC
	les	bx,cs:[CSlp16LogDevice_In_NVTEXT16]
	and	es:[bx].DIBENGINE.deFlags,NOT BUSY
        jmp     DIB_DibBltExt
CLOSEPROC

;==============================================================================
;
; Function:     Wrap_DibToDevice
;
; Purpose:      This function is junction to dibengine.
;               It clear busy bit before jmp because dibengine
;		return error if this bit set.
;
; Arguments:    none
;
; Returns:      none
;
;==============================================================================
DECPROC Wrap_DibToDevice, PASCAL, NOFRAME, FAR16
OPENPROC
	les	bx,cs:[CSlp16LogDevice_In_NVTEXT16]
	and	es:[bx].DIBENGINE.deFlags,NOT BUSY
        jmp     DIB_DibToDevice
CLOSEPROC

;==============================================================================
;
; Function:     Wrap_Output
;
; Purpose:      This function is junction to dibengine.
;               It clear busy bit before jmp because dibengine
;		return error if this bit set.
;
; Arguments:    none
;
; Returns:      none
;
;==============================================================================
DECPROC Wrap_Output, PASCAL, NOFRAME, FAR16
OPENPROC
	les	bx,cs:[CSlp16LogDevice_In_NVTEXT16]
	and	es:[bx].DIBENGINE.deFlags,NOT BUSY
        jmp     DIB_Output
CLOSEPROC

;==============================================================================
;
; Function:     Wrap_Pixel
;
; Purpose:      This function is junction to dibengine.
;               It clear busy bit before jmp because dibengine
;		return error if this bit set.
;
; Arguments:    none
;
; Returns:      none
;
;==============================================================================
DECPROC Wrap_Pixel, PASCAL, NOFRAME, FAR16
OPENPROC
	les	bx,cs:[CSlp16LogDevice_In_NVTEXT16]
	and	es:[bx].DIBENGINE.deFlags,NOT BUSY
        jmp     DIB_Pixel
CLOSEPROC

;==============================================================================
;
; Function:     Wrap_StretchBlt
;
; Purpose:      This function is junction to dibengine.
;               It clear busy bit before jmp because dibengine
;		return error if this bit set.
;
; Arguments:    none
;
; Returns:      none
;
;==============================================================================
DECPROC Wrap_StretchBlt, PASCAL, NOFRAME, FAR16
OPENPROC
	les	bx,cs:[CSlp16LogDevice_In_NVTEXT16]
	and	es:[bx].DIBENGINE.deFlags,NOT BUSY
        jmp     DIB_StretchBlt
CLOSEPROC

;==============================================================================
;
; Function:     Wrap_StretchDIBits
;
; Purpose:      This function is junction to dibengine.
;               It clear busy bit before jmp because dibengine
;		return error if this bit set.
;
; Arguments:    none
;
; Returns:      none
;
;==============================================================================
DECPROC Wrap_StretchDIBits, PASCAL, NOFRAME, FAR16
OPENPROC
	les	bx,cs:[CSlp16LogDevice_In_NVTEXT16]
	and	es:[bx].DIBENGINE.deFlags,NOT BUSY
        jmp     DIB_StretchDIBits
CLOSEPROC

;==============================================================================
;
; Function:     Wrap_UpdateColorsExt
;
; Purpose:      This function is junction to dibengine.
;               It clear busy bit before jmp because dibengine
;		return error if this bit set.
;
; Arguments:    none
;
; Returns:      none
;
;==============================================================================
DECPROC Wrap_UpdateColorsExt, PASCAL, NOFRAME, FAR16
OPENPROC
	les	bx,cs:[CSlp16LogDevice_In_NVTEXT16]
	and	es:[bx].DIBENGINE.deFlags,NOT BUSY
        jmp     DIB_UpdateColorsExt
CLOSEPROC

;================================================================
DECPROC DIB_DibBlt, PASCAL, NOFRAME, FAR16
OPENPROC
        ; The DIBENG DibBltExt takes an extra parameter which indicates
        ; whether we are running in a palettized mode right now.
	mov	bx,sp
	lgs	bx,ss:[bx+30]
        test    gs:[bx].DIBENGINEHDR.deFlags,PALETTIZED
        setne   bl
        movzx   bx,bl
        pop     eax
        push    bx
        push    eax
        jmp     DIB_DibBltExt
CLOSEPROC

;================================================================
DECPROC DIB_RealizeObject, PASCAL, NOFRAME, FAR16
OPENPROC
        pop     eax
        push    cs:[CSlp16LogDevice_In_NVTEXT16]
        push    eax
        jmp     DIB_RealizeObjectExt
CLOSEPROC

;================================================================
DECPROC DIB_SetPalette, PASCAL, NOFRAME, FAR16
OPENPROC
        pop     eax
        push    cs:[CSlp16LogDevice_In_NVTEXT16]
        push    eax
        jmp     DIB_SetPaletteExt
CLOSEPROC

;================================================================
DECPROC DIB_UpdateColors, PASCAL, NOFRAME, FAR16
OPENPROC
        pop     eax
        push    cs:[CSlp16LogDevice_In_NVTEXT16]
        push    eax
        jmp     DIB_UpdateColorsExt
CLOSEPROC

;================================================================
DECPROC DIB_SaveScreenBitmap, PASCAL, NOFRAME, FAR16
OPENPROC
	sub	eax,eax
        RETFAR16 06H
CLOSEPROC

;================================================================
GN_PatchedProcTable:
	dd	[Patch_BitBlt]
	dd	[Patch_BitmapBits]
	dd	[Patch_DibBlt]
	dd	[Patch_ExtTextOut]
	dd	[Patch_Output]
	dd	[Patch_Pixel]
	dd	[Patch_RealizeObject]
	dd	[Patch_ScanLR]
	dd	[Patch_SetDIBitsToDevice]
	dd	[Patch_SetPalette]
	dd	[Patch_StretchBlt]
	dd	[Patch_StretchDIBits]
	dd	[Patch_StrBlt]
	dd	[Patch_UpdateColors]
	dd	[Patch_SaveScreenBitmap]
PatchedProcTableSize	equ	( $ - GN_PatchedProcTable ) / 4

GN_HWCursorProcTable:
	dd	[GENERIC_BitBlt]
	dd	[GENERIC_BitmapBits]
	dd	[GENERIC_DibBlt]
	dd	[GENERIC_ExtTextOut]
	dd	[GENERIC_Output]
	dd	[GENERIC_Pixel]
	dd	[GENERIC_RealizeObject]
	dd	[GENERIC_ScanLR]
	dd	[GENERIC_SetDIBitsToDevice]
	dd	[GENERIC_SetPalette]
	dd	[GENERIC_StretchBlt]
	dd	[GENERIC_StretchDIBits]
	dd	[GENERIC_StrBlt]
	dd	[GENERIC_UpdateColors]
	dd	[GENERIC_SaveScreenBitmap]
        .errnz  $ - GN_HWCursorProcTable - PatchedProcTableSize * 4

GN_SWCursorProcTable:
	dd	[BitBlt_WithExclude]
	dd	[BitmapBits_WithExclude]
	dd	[DibBlt_WithExclude]
	dd	[ExtTextOut_WithExclude]
	dd	[Output_WithExclude]
	dd	[Pixel_WithExclude]
	dd	[GENERIC_RealizeObject]
	dd	[DIB_ScanLR]
	dd	[SetDIBitsToDevice_WithExclude]
	dd	[GENERIC_SetPalette]
	dd	[StretchBlt_WithExclude]
	dd	[StretchDIBits_WithExclude]
	dd	[StrBlt_WithExclude]
	dd	[UpdateColors_WithExclude]
	dd	[SaveScreenBitmap_WithExclude]
        .errnz  $ - GN_SWCursorProcTable - PatchedProcTableSize * 4

GN_PuntProcTable:
	dd	[DIB_BitBlt]
	dd	[DIB_BitmapBits]
	dd	[DIB_DibBlt]
	dd	[DIB_ExtTextOut]
	dd	[DIB_Output]
	dd	[DIB_Pixel]
;;	dd	[GENERIC_RealizeObject]
	dd	[DIB_RealizeObject]; do not create offscreen objects
	dd	[DIB_ScanLR]
	dd	[DIB_DibToDevice]
	dd	[GENERIC_SetPalette]	;[DIB_SetPalette]; to exclude h/w access at all
	dd	[DIB_StretchBlt]
	dd	[DIB_StretchDIBits]
	dd	[DIB_StrBlt]
	dd	[DIB_UpdateColors]
	dd	[DIB_SaveScreenBitmap]
        .errnz  $ - GN_PuntProcTable - PatchedProcTableSize * 4

;================================================================
GN_PatchedPuntTable:
	dd	[Punt_BitmapBits]
	dd	[Punt_BitBlt]
	dd	[Punt_DibBlt]
	dd	[Punt_DibToDevice]
	dd	[Punt_ExtTextOut]
	dd	[Punt_Output]
	dd	[Punt_Polyline]
	dd	[Punt_Rectangle]
	dd	[Punt_BeginScan]
	dd	[Punt_Scan]
	dd	[Punt_EndScan]
	dd	[Punt_Pixel]
	dd	[Punt_StrBlt]
	dd	[Punt_StrDib]
	dd	[Punt_UpdateColors]
PatchedPuntTableSize	equ	( $ - GN_PatchedPuntTable ) / 4

GN_DibengTable:
	dd	[DIB_BitmapBits]
	dd	[DIB_BitBlt]
	dd	[DIB_DibBltExt]
	dd	[DIB_DibToDevice]
	dd	[DIB_ExtTextOut]
	dd	[DIB_Output]
	dd	[DIB_Output]
	dd	[DIB_Output]
	dd	[DIB_Output]
	dd	[DIB_Output]
	dd	[DIB_Output]
	dd	[DIB_Pixel]
	dd	[DIB_StretchBlt]
	dd	[DIB_StretchDIBits]
	dd	[DIB_UpdateColorsExt]
        .errnz  $ - GN_DibengTable - PatchedPuntTableSize * 4

GN_JuncToDibTable:
	dd	[Wrap_BitmapBits]
	dd	[Wrap_BitBlt]
	dd	[Wrap_DibBltExt]
	dd	[Wrap_DibToDevice]
	dd	[Wrap_ExtTextOut]
	dd	[Wrap_Output]
	dd	[Wrap_Output]
	dd	[Wrap_Output]
	dd	[Wrap_Output]
	dd	[Wrap_Output]
	dd	[Wrap_Output]
	dd	[Wrap_Pixel]
	dd	[Wrap_StretchBlt]
	dd	[Wrap_StretchDIBits]
	dd	[Wrap_UpdateColorsExt]
        .errnz  $ - GN_JuncToDibTable - PatchedPuntTableSize * 4

;==============================================================================
;
; Function:     GN_TurnExcludeOffOrOn
;
; Purpose:      This function do patch of drv entry points
;		to work with h/w - s/w cursors.
;
; Arguments:    dwCursorFlags
;
;==============================================================================
DECPROC GN_TurnExcludeOffOrOn, PASCAL, FRAME, FAR16
PARMD   dwCursorFlags
OPENPROC
	PUSHR	ds,es,si,di
	mov	ax,cs:[wNVTEXT16_Alias_In_NVTEXT16]
	mov	es,ax
	mov	bx,GN_PuntProcTable
	or	ax,ax

	;;;In order to punt everything to dibengine change jz --> jnz
	jz	@F
	mov	bx,GN_HWCursorProcTable
@@:
	test	dwCursorFlags,CURSOR_HAS_TRAILS
	jz	@F
	mov	bx,GN_SWCursorProcTable
@@:
	mov	si,offset GN_PatchedProcTable
	mov	cx,PatchedProcTableSize
ProcLoop:
	mov	di,word ptr cs:[si]
	inc	di
	mov	eax,dword ptr cs:[bx]
	mov	dword ptr es:[di],eax
	add	bx,size DWORD
	add	si,size DWORD
	dec	cx
	jnz	ProcLoop

	mov	ax,cs:[wNVTEXT32_Alias_In_NVTEXT16]
	mov	es,ax
	mov	bx,GN_DibengTable
	test	dwCursorFlags,CURSOR_HAS_TRAILS
	jz	@F
	mov	bx,GN_JuncToDibTable
@@:
	mov	si,offset GN_PatchedPuntTable
	mov	cx,PatchedPuntTableSize
PuntLoop:
	mov	di,word ptr cs:[si]
	add	di,2
	mov	eax,dword ptr cs:[bx]
	mov	dword ptr es:[di],eax
	add	bx,size DWORD
	add	si,size DWORD
	dec	cx
	jnz	PuntLoop
	POPR	ds,es,si,di
CLOSEPROC

CLOSESEG _NVTEXT16

END

