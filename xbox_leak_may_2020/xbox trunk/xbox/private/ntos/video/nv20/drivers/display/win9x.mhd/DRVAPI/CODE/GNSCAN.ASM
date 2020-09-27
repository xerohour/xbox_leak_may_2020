;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnscan.asm
;
; Purpose:      This file holds the Output display driver implementation
;               when the style is either OS_SCANLINE, OS_POLYSCANLINE,
;               OS_BEGINNSCAN, or OS_ENDNSCAN.
;               The Output entry point jumps to here without creating a
;               FRAME when it is discovered that the style is one of
;               these four.
;
;==============================================================================
.586
incOutput = 1
incLogical = 1
incPolyScanline = 1
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


EXTERNDEF       GENERIC_BeginScan:FAR16
EXTERNDEF       GENERIC_EndScan:FAR16

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GENERIC_BeginScan
;
; Purpose:      The Output entry point jumps to here without creating a
;               FRAME when it is discovered that the Output style is
;               OS_BEGINNSCAN.
;
; Arguments:    Output Display Driver Stack Frame
;
; Returns:      ax      1       success
;                       0       failure
;                       -1      have GDI simulate
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_BeginScan, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wStyle
PARMW   wCount
PARMD   lpPoints
PARMD   lpPen
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   lpClipRect
LOCALD  dwRop2
LOCALD  pHWLogDevCXOffset
OPENPROC
        PUSHR   ds,esi,edi

        ; Get a ptr to the logical device context
        mov     ds,cs:[wFlatDataSel]
        mov     edi,cs:[CSpGNLogicalDevice]

        ; Get a ptr to the dst bitmap
        mov     gs,word ptr lpDstDev+2
        movzx   esi,word ptr lpDstDev+0

        ; Keep track of the fact that we are in a BeginScan
        mov     ds:[edi].GNLOGDEVCX.bInScanSequence,1

        ; Load dst flags and prep some registers
        sub     ebx,ebx
        movzx   eax,gs:[esi].DIBENGINE.deFlags

        ; Filter out old style destinations
        cmp     bx,gs:[esi].DIBENGINE.deType
        je      GN_BeginScanPunt

        ; Filter out based upon deFlags
        test    ah,VRAM / 256
        jz      GN_BeginScanPunt
        test    ah,PALETTE_XLAT / 256
        jnz     GN_BeginScanPunt

        ; Watch for busy hardware -- some other API owns the screen
        cmp     ebx,ds:[edi].GNLOGDEVCX.dwHardwareBusy
        jne     GN_BeginScanPunt

        ; If both the pen and brush are null, then we are done!
        ; The only way the two ptrs can be equal is if they are
        ; both NULL!
        mov     eax,lpBrush
        cmp     eax,lpPen
        je      BeginScanHollow

        ; Load up the drawmode ptr
        sub     ecx,ecx
        lfs     cx,lpDrawMode

	; Save off the hwlogdev for later
        mov     edx,ds:[edi].GNLOGDEVCX.pHWLogdevCX
	mov     pHWLogDevCXOffset,edx

        ; Get the rop
        movzx   edx,fs:[ecx].DRAWMODE.Rop2
        mov     dwRop2,edx

        ; Is there a brush?
        or	eax,eax
        je      UsePen

UseBrush:
        ; If the brush is hollow, then even if the rop only involves
        ; the dst (like BLACKNESS, WHITENESS, or INVERTDST), we blow
        ; off doing the output calls whatever they are. This sounds
        ; wierd but it is WHQL-correct behavior.
        sub     esi,esi
        les     si,lpBrush
        cmp     es:[esi].DIB_Brush8.dp8BrushStyle,BS_HOLLOW
        je      BeginScanHollow

        ; It is not a hollow brush. If the rop only involves the
        ; dst (no pattern involvement), then skip the pattern load.
        dec     edx
        je      RetSetPat
        cmp     edx,05H
        je      RetSetPat
        cmp     edx,0AH
        je      RetSetPat
        cmp     edx,0FH
        je      RetSetPat

        ; Load up the pattern
        mov     cl,byte ptr es:[esi].DIB_Brush8.dp8BrushStyle
        mov     al,byte ptr es:[esi].DIB_Brush8.dp8BrushFlags
        mov     edx,dword ptr es:[esi].DIB_Brush8.dp8BrushBits
        mov     ebx,pHWLogDevCXOffset
        PUSH    OFFSET RetSetPat
        cmp     cl,BS_HOLLOW
        jnc     @F
        test    al,COLORSOLID
        jz      HW_LoadColorPattern
        jmp     HW_LoadSolidPattern
@@:     cmp     cl,BS_PATTERN
        jc      IsHatch
        ja      GN_BitBltFail
        test    al,PATTERNMONO
        jz      HW_LoadColorPattern
IsMono:
        sub     edx,edx
        lfs     dx,lpDrawMode
	lea	esi,[esi].DIB_Brush16.dp16BrushMono
        mov     ecx,fs:[edx].DRAWMODE.TextColor
        mov     edx,fs:[edx].DRAWMODE.bkColor
        mov     eax,-1
        jmp     HW_LoadMonoPattern
IsHatch:
        sub     edx,edx
        lfs     dx,lpDrawMode
        cmp     fs:[edx].DRAWMODE.bkMode,TRANSPARENT
        sete    al
        cmp     al,1
        sbb     eax,eax
	mov     ecx,es:[esi].DIB_Brush8.dp8BgColor
        mov     edx,es:[esi].DIB_Brush8.dp8FgColor
	lea	esi,[esi].DIB_Brush16.dp16BrushMask
        jmp     HW_LoadMonoPattern

UsePen:
	sub	ebx,ebx
        les     bx,lpPen
        cmp     es:[ebx].DIB_Pen.dpPenStyle,LS_SOLID
        jne     BeginScanHollow
        mov     edx,es:[ebx].DIB_Pen.dpPenColor
        mov     ebx,pHWLogDevCXOffset
        call    HW_LoadSolidPattern
        jmp     RetSetPat

BeginScanHollow:
        mov     ds:[edi].GNLOGDEVCX.bHollowBrushForScan,1
        jmp     BeginScanSuccess

GN_BeginScanPunt:
        mov     ds:[edi].GNLOGDEVCX.bPuntingScan,1
        POPR    ds,esi,edi
        STOPPROC
PLABEL	Punt_BeginScan
        jmp     DIB_Output

RetSetPat:
        ; We are not punting Scanlines AND they are not hollow
        mov     edi,cs:[CSpGNLogicalDevice]
	movzx	esi,word ptr lpDstDev
        mov     ds:[edi].GNLOGDEVCX.bPuntingScan,0
        mov     ds:[edi].GNLOGDEVCX.bHollowBrushForScan,0

        ; Call the hardware begin rect routine
        sub	ebx,ebx
        mov	ecx,7FFF7FFFH
        mov     edi,ds:[edi].GNLOGDEVCX.pHWLogdevCX
        cmp     lpClipRect,0
        je      @F
        lfs     cx,lpClipRect
        mov     ebx,dword ptr fs:[ecx].RECT.left
        mov     ecx,dword ptr fs:[ecx].RECT.right
@@:
	push	pHWLogDevCXOffset
	push	gs:[esi].DIBENGINEHDR.deBitsOffset
	push	gs:[esi].DIBENGINEHDR.deDeltaScan
	push	dwRop2
	push	ecx
	push	ebx
        call    HW_BeginRect

BeginScanSuccess:
        mov     eax,1

BeginScanDone:
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 1CH
CLOSEPROC


;==============================================================================
;
; Function:     GENERIC_PolyScanOrScanLine
;
; Purpose:      The Output entry point jumps to here without creating a
;               FRAME when it is discovered that the Output style is
;               OS_POLYSCANLIE or OS_SCANLINE. They are very similar
;               except for the inner loop so we process them in the
;               same routine.
;
; Arguments:    Output Display Driver Stack Frame
;
; Returns:      ax      1       success
;                       0       failure
;                       -1      have GDI simulate
;
; Preserve:     ds,esi,edi
; Purpose:      This function is the gdi Polyscan/Scnaline entry point.
;
; Arguments:
;
; Returns:
;
;==============================================================================
DECPROC GENERIC_PolyScanOrScanLine, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wStyle
PARMW   wCount
PARMD   lpPoints
PARMD   lpPen
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   lpClipRect
LOCALD  dwXPairs
LOCALD  dwYPairs
LOCALD  dwLocalBeginScan
OPENPROC
        PUSHR   ds,esi,edi

        ; Get a ptr to the logical device context
        mov     ds,cs:[wFlatDataSel]
        mov     edi,cs:[CSpGNLogicalDevice]

        ; If we are not in a BEGINSCAN sequence, then do BeginScan now
        cmp     ds:[edi].GNLOGDEVCX.bInScanSequence,0
        je      DoBeginScan
        mov     dwLocalBeginScan,0

        ; We did receive a BeginScan call earlier, but are we punting
        ; the whole scan sequence to the DIBENG?
        cmp     ds:[edi].GNLOGDEVCX.bPuntingScan,0
        jne     GN_ScanPunt
DoneBeginScan:

        ; We are supposed to do it, but nothing to do if brush was hollow
        cmp     ds:[edi].GNLOGDEVCX.bHollowBrushForScan,0
        jne     ScanDone

        ; Watch for busy hardware -- some other API owns the screen
        cmp     ds:[edi].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_ScanPunt

        ; Now go into the appropriate loop calling the hardware
        xor     esi,esi                 ; clear high word of ESI
        mov     edi,ds:[edi].GNLOGDEVCX.pHWLogdevCX
        cmp     wStyle,OS_SCANLINES
        je      DoScanline

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DoPolyScan:
        les     si,lpPoints
        movzx   ecx,wCount
        mov     dwYPairs,ecx
LoopY:
        movzx   ecx,es:[esi].SCAN.scnPntCnt
        movsx   eax,es:[esi].SCAN.scnPntTop
        movsx   ebx,es:[esi].SCAN.scnPntBottom
        shr     ecx,1
        je      NextY
        cmp     ebx,eax
        jle     NextY
        lea     esi,es:[esi].SCAN.scnPntX
        shl     eax,10H
        shl     ebx,10H
        mov     dwXPairs,ecx

        ; high word of eax = top of block
        ; high word of ebx = bottom of block
        ; es:esi = ptr to begin of coord pairs for this scan group
LoopX:
        mov     ax,es:[esi + 0]
        mov     bx,es:[esi + 2]
        add     esi,4
        cmp     ax,bx
        jge     @F
        cmp     eax,ebx
        jge     @F
        call    HW_BlockRect
@@:
        dec     dwXPairs
        jg      LoopX
        add     esi,2                    ; skip unused field
NextY:
        dec     dwYPairs
        jg      LoopY
        jmp     ScanDone
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DoScanline:
        les     si,lpPoints
        mov     eax,es:[esi]
        add     esi,4
        movzx   ecx,wCount
        lea     ebx,[eax + 10000H]
        dec     ecx
        mov     dwXPairs,ecx
LoopX1: mov     ax,es:[esi + 0]
        mov     bx,es:[esi + 2]
        add     esi,4
        cmp     ax,bx
        jge     @F
        cmp     eax,ebx
        jge     @F
        call    HW_BlockRect
@@:
        dec     dwXPairs
        jne     LoopX1
        jmp     ScanDone
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ScanDone:
        ; If we are not in a Begin/Scan/End sequence and had to do the
        ; begin locally, then we need to do the end locally as well.
        cmp     dwLocalBeginScan,0
        jne     DoEndScan

DoneEndScan:
        mov     eax,1
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 1CH

GN_ScanPunt:
        POPR    ds,esi,edi
        STOPPROC
PLABEL	Punt_Scan
        jmp     DIB_Output

DoBeginScan:
        mov     dwLocalBeginScan,1
        push    lpDstDev
        pushw   OS_BEGINNSCAN
        push    wCount
        push    lpPoints
        push    lpPen
        push    lpBrush
        push    lpDrawMode
        push    lpClipRect
        CALLFAR16 GENERIC_BeginScan
        cmp     ds:[edi].GNLOGDEVCX.bPuntingScan,0
        je      DoneBeginScan

        ; We are punting to the DIBENG -- let it handle everything
        push    lpDstDev
        push    wStyle
        push    wCount
        push    lpPoints
        push    lpPen
        push    lpBrush
        push    lpDrawMode
        push    lpClipRect
        call    DIB_Output

DoEndScan:
        push    lpDstDev
        pushw   OS_ENDNSCAN
        push    wCount
        push    lpPoints
        push    lpPen
        push    lpBrush
        push    lpDrawMode
        push    lpClipRect
        CALLFAR16  GENERIC_EndScan
        jmp     DoneEndScan

GENERIC_PolyScanOrScanLine      ENDP


;==============================================================================
;
; Function:     GENERIC_EndScan
;
; Purpose:      The Output entry point jumps to here without creating a
;               FRAME when it is discovered that the Output style is
;               OS_ENDNSCAN.
;
; Arguments:    Output Display Driver Stack Frame
;
; Returns:      ax      1       success
;                       0       failure
;                       -1      have GDI simulate
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_EndScan, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wStyle
PARMW   wCount
PARMD   lpPoints
PARMD   lpPen
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   lpClipRect
OPENPROC
        PUSHR   ds,esi,edi

        ; Get a ptr to the logical device context
        mov     ds,cs:[wFlatDataSel]
        mov     edi,cs:[CSpGNLogicalDevice]
        sub     eax,eax

        ; If we are not in a BEGINSCAN sequence, get out
        cmp     al,ds:[edi].GNLOGDEVCX.bInScanSequence
        je      EndScanSuccess
        mov     ds:[edi].GNLOGDEVCX.bInScanSequence,al

        ; If we have a hollow brush, get out
        cmp     al,ds:[edi].GNLOGDEVCX.bHollowBrushForScan
        mov     ds:[edi].GNLOGDEVCX.bHollowBrushForScan,al
        jne     EndScanSuccess

        ; If we have routed the whole scan sequence to the DIBENG,
        ; then don't stop now.
        cmp     al,ds:[edi].GNLOGDEVCX.bPuntingScan
        jne     GN_EndScanPunt

        ; Watch for busy hardware -- some other API owns the screen
        cmp     eax,ds:[edi].GNLOGDEVCX.dwHardwareBusy
        jne     GN_EndScanPunt

        ; call the hardware specific routine
        mov     edi,ds:[edi].GNLOGDEVCX.pHWLogdevCX
        call    HW_EndRect

EndScanSuccess:
        POPR    ds,esi,edi
        mov     eax,1
        STOPPROC
        RETFAR16 1CH

GN_EndScanPunt:
        POPR    ds,esi,edi
        STOPPROC
PLABEL	Punt_EndScan
        jmp     DIB_Output

GENERIC_EndScan     ENDP

CLOSESEG _NVTEXT32

END

