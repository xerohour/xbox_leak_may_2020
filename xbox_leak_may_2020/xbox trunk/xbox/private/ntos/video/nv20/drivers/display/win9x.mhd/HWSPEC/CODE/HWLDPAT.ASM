;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwldpat.asm
;
; Purpose:      This file contains routines to load the different kinds
;               of patterns into the hardware when the patterns are
;               going to be used for 3operand blts or cases where there
;               might be xparency.
;
;==============================================================================
.586
incLogical = 1
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

;==============================================================================
;
; Function:     HW_LoadSolidPattern
;
; Purpose:      This function is called by the generic code to load
;               up a solid pattern that will be used in a 3operand blt
;               or in a situation where xparency may be required.
;
; Arguments:
;               ds                      CXSel
;               ebx                     HWLOGDEVCX ptr
;               edx                     Solid Color
;
; Returns:      None
;
; Preserve:     ds,es,esi
;==============================================================================
DECPROC HW_LoadSolidPattern, PASCAL, NOFRAME, NEAR
OPENPROC
	mov	ecx,ds:[ebx].HWLOGDEVCX.dwPatternColorFormat
        or      edx,ds:[ebx].HWLOGDEVCX.dwColorAlphaMask
        mov	ds:[ebx].HWLOGDEVCX.dwPatternAlpha0,edx
        mov	ds:[ebx].HWLOGDEVCX.dwPatternAlpha1,edx

        mov     ebx,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ebx,eax

        ; Load the push buffer ptr
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        CHECKDMASPACE ds,ebx,edi,18H
        mov     eax,CHNRUN(ContextPattern.SetColorFormat,1)
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        mov     eax,CHNRUN(ContextPattern.SetPatternSelect,3)
        mov     ecx,NV044_SET_PATTERN_SELECT_MONOCHROME
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ecx
        mov     ds:[edi + 10H],edx
        mov     ds:[edi + 14H],edx
        add     edi,18H

        mov     ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_LoadMonoPattern
;
; Purpose:      This function is called by the generic code to load
;               up a mono pattern that will be used in a 3operand blt
;               or in a situation where xparency may be required.
;
; Arguments:
;               ds                      CXSel
;               ebx                     HWLOGDEVCX ptr
;               es:esi                  ptr to pattern to use
;               ecx                     Color for 0 bits
;               edx                     Color for 1 bits
;               eax                     0 if 0 src bits should be skipped
;                                               (transparent mode)
;                                       -1 if 0 src bits should be expanded
;                                               and written (opaque mode)
; Returns:      None
;
; Preserve:     ds
;==============================================================================
DECPROC HW_LoadMonoPattern, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Hatch is similar to mono, except we have to worry about
        ; the possibility of the bg being xparent.
        or      edx,ds:[ebx].HWLOGDEVCX.dwColorAlphaMask
        or      ecx,ds:[ebx].HWLOGDEVCX.dwColorAlphaMask
        and     ecx,eax
        mov	ds:[ebx].HWLOGDEVCX.dwPatternAlpha0,ecx
        mov	ds:[ebx].HWLOGDEVCX.dwPatternAlpha1,edx
        push    edx
        push    ecx

        mov     eax,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,eax,ecx

        ; Load the push buffer ptr
        mov     edi,ds:[eax].HWBOARDCX.pCurrentPushBufferOffset

        ; Get the pattern
        mov     cl,es:[esi + 08H]
        mov     dl,es:[esi + 18H]
        mov     ch,es:[esi + 0CH]
        mov     dh,es:[esi + 1CH]
        shl     ecx,10H
        shl     edx,10H
        mov     cl,es:[esi + 00H]
        mov     dl,es:[esi + 10H]
        mov     ch,es:[esi + 04H]
        mov     dh,es:[esi + 14H]

        ; Put code in here to set the mono or hatch pattern.
        ; ecx = 0 pattern bits map to this color
        ; edx = 1 pattern bits map to this color

        CHECKDMASPACE ds,eax,edi,20H
        mov	esi,ds:[ebx].HWLOGDEVCX.dwPatternColorFormat
        mov     dword ptr ds:[edi + 00H],CHNRUN(ContextPattern.SetColorFormat,1)
        mov     ds:[edi + 04H],esi
        mov     dword ptr ds:[edi + 08H],CHNRUN(ContextPattern.SetPatternSelect,5)
        mov     dword ptr ds:[edi + 0CH],NV044_SET_PATTERN_SELECT_MONOCHROME
        pop     dword ptr ds:[edi + 10H]
        pop     dword ptr ds:[edi + 14H]
        mov     ds:[edi + 18H],ecx
        mov     ds:[edi + 1CH],edx
        add     edi,20H

        mov     ds:[eax].HWBOARDCX.pCurrentPushBufferOffset,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_LoadColorPattern
;
; Purpose:      This function is called by the generic code to load
;               up a color pattern that will be used in a 3operand blt
;               or in a situation where xparency may be required.
;
; Arguments:
;               ds                      CXSel
;               ebx                     HWLOGDEVCX ptr
;               es:esi                  DIB_Brush ptr
;
; Returns:      None
;
; Preserve:     ds
;==============================================================================
DECPROC HW_LoadColorPattern, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Load the HWBOARDCX
        mov     eax,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,eax,ecx

        mov     ecx,ds:[ebx].HWLOGDEVCX.dwColorPatternAndHeaderSize

        ; Load the push buffer ptr
        mov     edi,ds:[eax].HWBOARDCX.pCurrentPushBufferOffset
        lea     esi,[esi].DIB_Brush8.dp8BrushBits

        ; Put code in here to set the color brush
        ; eax   bits per pixel of a pixel in the pattern
        ; es:esi ptr to 64 consecutive pixels describing the pattern

        ; Setup and copy the color pattern into the chip
        CHECKDMASPACE ds,eax,edi,ecx
        mov     edx,ds:[ebx].HWLOGDEVCX.dwPatternColorFormat
        mov     dword ptr ds:[edi + 00H],CHNRUN(ContextPattern.SetColorFormat,6)
        mov     ds:[edi + 04H],edx
        mov	dword ptr ds:[edi + 08H],NV044_SET_MONOCHROME_FORMAT_CGA6_M1
        mov	dword ptr ds:[edi + 0CH],NV044_SET_MONOCHROME_SHAPE_8X_8Y
        mov     dword ptr ds:[edi + 10H],NV044_SET_PATTERN_SELECT_COLOR
        mov	edx,ds:[ebx].HWLOGDEVCX.dwPatternAlpha0
        mov     dword ptr ds:[edi + 14H],edx
        mov	edx,ds:[ebx].HWLOGDEVCX.dwPatternAlpha1
        mov     dword ptr ds:[edi + 18H],edx
        mov     edx,ds:[ebx].HWLOGDEVCX.dwColorPatternMethodOffset
        mov     ds:[edi + 1CH],edx
        sub     ecx,20H
        add     edi,20H
@@:
	mov     ebx,es:[esi + 00H]
        mov     edx,es:[esi + 04H]
        add     esi,08H
        mov     ds:[edi + 00H],ebx
        mov     ds:[edi + 04H],edx
        add     edi,08H
        sub     ecx,08H
        jg      @B

        mov     ds:[eax].HWBOARDCX.pCurrentPushBufferOffset,edi
        mov     eax,1
CLOSEPROC

CLOSESEG _NVTEXT32

END

