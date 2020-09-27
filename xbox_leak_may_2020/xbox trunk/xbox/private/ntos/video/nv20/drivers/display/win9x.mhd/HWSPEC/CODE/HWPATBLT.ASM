;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwpatblt.asm
;
; Purpose:      This file performs BitBlts involving just
;               the dst or the dst and a pattern.
;
;==============================================================================
.586
incDrawMode = 1
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include hwmacs.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwcrtc.inc
include hwboard.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

PUBLIC  bConvertPatDstToSrcDstRop
bConvertPatDstToSrcDstRop       LABEL   BYTE
DB      000h, 011h, 022h, 033h
DB      000h, 011h, 022h, 033h
DB      000h, 011h, 022h, 033h
DB      000h, 011h, 022h, 033h
DB      044h, 055h, 066h, 077h
DB      044h, 055h, 066h, 077h
DB      044h, 055h, 066h, 077h
DB      044h, 055h, 066h, 077h
DB      088h, 099h, 0AAh, 0BBh
DB      088h, 099h, 0AAh, 0BBh
DB      088h, 099h, 0AAh, 0BBh
DB      088h, 099h, 0AAh, 0BBh
DB      0CCh, 0DDh, 0EEh, 0FFh
DB      0CCh, 0DDh, 0EEh, 0FFh
DB      0CCh, 0DDh, 0EEh, 0FFh
DB      0CCh, 0DDh, 0EEh, 0FFh


;==============================================================================
;
; Function:     HW_BitBltSolidPatDstRop
;
; Purpose:      This function implements Bitblts that involve only
;               a pattern and dst and the pattern is a solid color.
;               At this point, all that needs to be done is to program
;               the chip for the blt.
;
; Arguments:
;               ds              Context Selector
;               ebx             HWLOGDEVCX ptr
;               ecx             dwRop3
;               edx             physical color
;               gs:esi          lpDstDev
;
; Returns:      If you want to
;               report this     then do this
;               Success         jmp to GN_BitBltSuccess
;               Failure         jmp to GN_BitBltFail
;               Punt to DIBENG  jmp to GN_BitBltPunt
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_BitBltSolidPatDstRop, PASCAL, ShareFrameFar16, NEAR
BITBLT_FRAME_AND_LOCALS
OPENPROC
        ; Save the color format for later use.
        push    ds:[ebx].HWLOGDEVCX.dwRectAndTextColorFormat
        push    ds:[ebx].HWLOGDEVCX.dwPatternAlpha1
        push    ds:[ebx].HWLOGDEVCX.dwPatternAlpha0

        mov     ebx,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ebx,eax

        ; Load the push buffer ptr
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     eax,gs:[esi].DIBENGINEHDR.deBitsOffset
        CHECKDSTBASE ds,ebx,edi,eax,<gs:[esi].DIBENGINEHDR.deDeltaScan>

        ; Check for space and program the blt
        CHECKDMASPACE ds,ebx,edi,2CH
        mov     eax,CHNRUN(ContextPattern.SetMonochromeColor0,2)
        mov     ds:[edi + 00H],eax
        pop     dword ptr ds:[edi + 04H]
        pop     dword ptr ds:[edi + 08H]
        shr     ecx,2
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        movzx   ecx,cs:bConvertPatDstToSrcDstRop[ecx]
        mov     ds:[edi + 0CH],eax
        mov     ds:[edi + 10H],ecx
        mov     eax,CHNRUN(RectAndText.SetColorFormat,1)
        mov     ds:[edi + 14H],eax
        pop     dword ptr ds:[edi + 18H]
        mov     eax,CHNRUN(RectAndText.Color1A,3)
        mov     ds:[edi + 1CH],eax
        mov     ds:[edi + 20H],edx
        mov     eax,dword ptr wDstY
        mov     ecx,dword ptr wExtY
        mov     ds:[edi + 24H],eax
        mov     ds:[edi + 28H],ecx
        add     edi,2CH
        SETPUTPTR ds,ebx,edi
        jmp     GN_BitBltSuccess
CLOSEPROC


;==============================================================================
;
; Function:     HW_BitBltMonoPatDstRop
;
; Purpose:      This function implements Bitblts that involve only
;               a pattern and dst and the pattern is a mono pattern.
;               At this point, all that needs to be done is to program
;               the chip for the blt.
;
; Arguments:
;               ds              Context Selector
;               ebx             HWLOGDEVCX ptr
;               gs:esi          lpDstDev
;               es:edi          ptr to pattern to load
;               wDstX,wDstY     dst upper left corner
;               wExtX,wExtY     extent of blt rectangle
;               eax             Color For 0 bits
;               ecx             Color For 1 bits
;
; Returns:      If you want to
;               report this     then do this
;               Success         jmp to GN_BitBltSuccess
;               Failure         jmp to GN_BitBltFail
;               Punt to DIBENG  jmp to GN_BitBltPunt
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_BitBltMonoPatDstRop, PASCAL, ShareFrameFar16, NEAR
BITBLT_FRAME_AND_LOCALS
OPENPROC
        ; Save the color format for later use.
        push    ds:[ebx].HWLOGDEVCX.dwRectAndTextColorFormat
        or      ecx,ds:[ebx].HWLOGDEVCX.dwColorAlphaMask
        or      eax,ds:[ebx].HWLOGDEVCX.dwColorAlphaMask
        mov    	ds:[ebx].HWLOGDEVCX.dwPatternAlpha0,eax
        mov    	ds:[ebx].HWLOGDEVCX.dwPatternAlpha1,ecx
        push    ecx
        push    eax
        push    ds:[ebx].HWLOGDEVCX.dwPatternColorFormat

        mov	edx,edi
        mov     ebx,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ebx,eax

        ; Load the push buffer ptr
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     eax,gs:[esi].DIBENGINEHDR.deBitsOffset
        CHECKDSTBASE ds,ebx,edi,eax,<gs:[esi].DIBENGINEHDR.deDeltaScan>
        mov	esi,edx

        ; Check for space and program the blt
        CHECKDMASPACE ds,ebx,edi,40H
        mov     dword ptr ds:[edi + 00H],CHNRUN(ContextPattern.SetColorFormat,1)
        pop     dword ptr ds:[edi + 04H]
        mov     eax,CHNRUN(ContextPattern.SetPatternSelect,5)
        mov     ecx,NV044_SET_PATTERN_SELECT_MONOCHROME
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ecx
        pop     dword ptr ds:[edi + 10H]
        pop     dword ptr ds:[edi + 14H]
        mov     al,es:[esi + 08H]
        mov     cl,es:[esi + 18H]
        mov     ah,es:[esi + 0CH]
        mov     ch,es:[esi + 1CH]
        ror     eax,10H
        ror     ecx,10H
        mov     al,es:[esi + 00H]
        mov     cl,es:[esi + 10H]
        mov     ah,es:[esi + 04H]
        mov     ch,es:[esi + 14H]
        mov     ds:[edi + 18H],eax
        mov     ds:[edi + 1CH],ecx
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        movzx   ecx,byte ptr dwRop3+2
        mov     ds:[edi + 20H],eax
        mov     ds:[edi + 24H],ecx
        mov     eax,CHNRUN(RectAndText.SetColorFormat,1)
        mov     ds:[edi + 28H],eax
        pop     dword ptr ds:[edi + 2CH]
        mov     eax,CHNRUN(RectAndText.Color1A,3)
        mov     ecx,dword ptr wDstY
        mov     edx,dword ptr wExtY
        mov     ds:[edi + 30H],eax

        ; The Color1A field next is irrelevant but the NV
        ; architecture requires that it at least be written.
        mov     ds:[edi + 34H],eax
        mov     ds:[edi + 38H],ecx
        mov     ds:[edi + 3CH],edx
        add     edi,40H
        SETPUTPTR ds,ebx,edi
        jmp     GN_BitBltSuccess
CLOSEPROC


;==============================================================================
;
; Function:     HW_BitBltColorPatDstRop
;
; Purpose:      This function implements Bitblts that involve only
;               a pattern and dst and the pattern is a color pattern.
;               At this point, all that needs to be done is to program
;               the chip for the blt.
;
; Arguments:    ds:ebx          HWLOGDEV ptr
;               es:edi          lpBrush
;               gs:esi          lpDstDev
;               wDstX,wDstY     dst upper left corner
;               wExtX,wExtY     extent of blt rectangle
;
; Returns:      If you want to
;               report this     then do this
;               Success         jmp to GN_BitBltSuccess
;               Failure         jmp to GN_BitBltFail
;               Punt to DIBENG  jmp to GN_BitBltPunt
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_BitBltColorPatDstRop, PASCAL, ShareFrameFar16, NEAR
BITBLT_FRAME_AND_LOCALS
OPENPROC
        ; Save the Rect and Text color format for later use.
        push    ds:[ebx].HWLOGDEVCX.dwRectAndTextColorFormat

        ; Save these for later use.
        push    ds:[ebx].HWLOGDEVCX.dwPatternAlpha1
        push    ds:[ebx].HWLOGDEVCX.dwPatternAlpha0
        push    ds:[ebx].HWLOGDEVCX.dwPatternColorFormat
        mov     ecx,ds:[ebx].HWLOGDEVCX.dwColorPatternAndHeaderSize
        mov     edx,ds:[ebx].HWLOGDEVCX.dwColorPatternMethodOffset

        mov     ebx,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ebx,eax

        ; Load the push buffer ptr
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     eax,gs:[esi].DIBENGINEHDR.deBitsOffset
        CHECKDSTBASE ds,ebx,edi,eax,<gs:[esi].DIBENGINEHDR.deDeltaScan>

        ; Setup and copy the color pattern into the chip
        CHECKDMASPACE ds,ebx,edi,ecx
        mov	eax,CHNRUN(ContextPattern.SetColorFormat,6)
        mov	esi,NV044_SET_MONOCHROME_FORMAT_CGA6_M1
        mov     ds:[edi + 00H],eax
        pop     dword ptr ds:[edi + 04H]
        mov	ds:[edi + 08H],esi
        mov	eax,NV044_SET_MONOCHROME_SHAPE_8X_8Y
        mov	esi,NV044_SET_PATTERN_SELECT_COLOR
        mov	ds:[edi + 0CH],eax
        mov     ds:[edi + 10H],esi
        pop     dword ptr ds:[edi + 14H]
        pop     dword ptr ds:[edi + 18H]
        movzx   esi,word ptr lpBrush
        mov     ds:[edi + 1CH],edx
        sub     ecx,20H
        lea     esi,[esi].DIB_Brush8.dp8BrushBits
        add     edi,20H

@@:     mov     eax,es:[esi + 00H]
        mov     edx,es:[esi + 04H]
        add     esi,08H
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],edx
        add     edi,08H
        sub     ecx,08H
        jg      @B

        ; Now trigger the blt
        CHECKDMASPACE ds,ebx,edi,20H
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        movzx   ecx,byte ptr dwRop3+2
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        mov     dword ptr ds:[edi + 08H],CHNRUN(RectAndText.SetColorFormat,1)
        pop     dword ptr ds:[edi + 0CH]
        mov     eax,CHNRUN(RectAndText.Color1A,3)
        mov     ecx,dword ptr wDstY
        mov     edx,dword ptr wExtY
        mov     ds:[edi + 10H],eax
        ; The color field next is irrelevant but NV architecture requires it
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ecx
        mov     ds:[edi + 1CH],edx
        add     edi,20H
        SETPUTPTR ds,ebx,edi
        jmp     GN_BitBltSuccess
CLOSEPROC

CLOSESEG _NVTEXT32

END

