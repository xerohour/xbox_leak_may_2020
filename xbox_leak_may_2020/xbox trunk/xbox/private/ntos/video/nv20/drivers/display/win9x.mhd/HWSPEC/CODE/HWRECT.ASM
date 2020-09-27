;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwrect.asm
;
; Purpose:      This file contains a special sequence of three hardware
;               routines which the generic code calls frequently. They
;               are used for ropping a patterned rectangle into a dst
;               bitmap.
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

PUBLIC  bConvertRop2ToRop3
bConvertRop2ToRop3      LABEL   BYTE
DB      000H,   005H,   00AH,   00FH
DB      050H,   055H,   05AH,   05FH
DB      0A0H,   0A5H,   0AAH,   0AFH
DB      0F0H,   0F5H,   0FAH,   0FFH

;==============================================================================
;
; Function:     HW_DrawRectAndBorder
;
; Purpose:      This function will be called by the generic code to have
;               the hardware draw a rectangle and/or its border int a
;               random bitmap. There is a rop and maybe a clip rect.
;               If the rectangle should be filled, the generic code will
;               already have called one of the pattern load routines to
;               put the appropriate fill pattern into the hardware
;               before calling this routine. Hence, program the dst
;               base, pitch, clip rect, rop, and issue the blt. Once
;               that is done, the border is always drawn in the solid
;               color, dwSolidBorderColor with the same rop at the
;               border coordinates given.
;
; Arguments:
;               ds                      CXSel
;               pHWLogDevCXOffset       HWLOGDEVCX ptr
;               dwDstBase               offset of dst bitmap in video memory
;               dwDstPitch              pitch of dst bitmap in video memory
;               dwRop2                  two operand Rop (1 based)
;               wClipBottom             clip rectangle
;               wClipRight              if there is no clip rect then
;               wClipTop                wClipLeft = wClipTop = 0   and
;               wClipLeft               wclipRight = wClipBottom = 7FFF
;               dwDrawBorder            0       then no border,
;                                       non-0   then there is a border
;               dwSolidBorderColor      border color
;               wBorderBottom           border rectangle.
;               wBorderRight
;               wBorderTop
;               wBorderLeft
;               dwDrawInterior          non-0 then draw inside, 0 then don't
;               wRectBottom             Rectangle coordinates
;               wRectRight
;               wRectTop
;               wRectLeft
;
;               Using the border rect, draw the following four edges
;               as the border
;               inclusive       to      exclusive
;               (left,top)      to      (right,top+1)
;               (right-1,top+1) to      (right,bottom)
;               (left,top+1)    to      (left+1,bottom-1)
;               (left,bottom-1) to      (right-1,bottom)
;
; Returns:      eax             0       Punt to Dibeng
;                               non-0   Success
;
; Preserve:     ds
;==============================================================================
DECPROC HW_DrawRectAndBorder, PASCAL, FRAME, NEAR
PARMD   pHWLogDevCXOffset
PARMD   dwDstBase
PARMD   dwDstPitch
PARMD   dwRop2
PARMW   wClipBottom
PARMW   wClipRight
PARMW   wClipTop
PARMW   wClipLeft
PARMD   dwDrawBorder
PARMD   dwSolidBorderColor
PARMW   wBorderBottom
PARMW   wBorderRight
PARMW   wBorderTop
PARMW   wBorderLeft
PARMD   dwDrawInterior
PARMW   wRectBottom
PARMW   wRectRight
PARMW   wRectTop
PARMW   wRectLeft
OPENPROC
        ; Load up the logical device CX and the board CX
        mov     esi,pHWLogDevCXOffset
        mov     ebx,ds:[esi].HWLOGDEVCX.pHWBoardCX

        ; Restore the pattern alpha to -1 in stucture. It may have been 
	; changed because this output call used	a hatch brush with a 
	; transparent background.
        mov      ds:[esi].HWLOGDEVCX.dwPatternAlpha0,-1

        ; sync with other channels
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ebx,eax

        ; Load the push buffer ptr
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     eax,dwDstBase
        CHECKDSTBASE ds,ebx,edi,eax,<dwDstPitch>

        ; Set the Rop, ColorFormat and Local Clip Rect
        CHECKDMASPACE ds,ebx,edi,10H
        mov     eax,dwRop2
        mov     ecx,ds:[esi].HWLOGDEVCX.dwRectAndTextColorFormat
        movzx   eax,cs:bConvertRop2ToRop3[eax - 1]
        mov     dword ptr ds:[edi + 00H],CHNRUN(ContextRop.SetRop5,1)
        mov     dword ptr ds:[edi + 04H],eax
        mov     dword ptr ds:[edi + 08H],CHNRUN(RectAndText.SetColorFormat,1)
        mov     dword ptr ds:[edi + 0CH],ecx
        add     edi,10H

        cmp     dwDrawInterior,0
        je      DoneInterior

        CHECKDMASPACE ds,ebx,edi,18H
        mov     eax,dword ptr wClipLeft
        mov     ecx,dword ptr wClipRight
        mov     dword ptr ds:[edi + 00H],CHNRUN(RectAndText.ClipPoint0B,5)
        mov     ds:[edi + 04H],eax
        mov     ds:[edi + 08H],ecx

        ; The Color1B value is irrelevant but must be written
        mov     ds:[edi + 0CH],eax

        mov     eax,dword ptr wRectLeft
        mov     ecx,dword ptr wRectRight
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ecx
        add     edi,18H

DoneInterior:
        cmp     dwDrawBorder,0
        je      DoneIt

        CHECKDMASPACE ds,ebx,edi,40H

        ; The format of the rop we have set requires that we use the
        ; image pattern sub-channel rather than the solid color field
        ; in the rect and text object for the solid color border.
        mov     edx,dwSolidBorderColor
        mov     eax,CHNRUN(ContextPattern.SetPatternSelect,3)
        mov     ecx,NV044_SET_PATTERN_SELECT_MONOCHROME
        or      edx,ds:[esi].HWLOGDEVCX.dwColorAlphaMask
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        mov     ds:[edi + 08H],edx
        mov     ds:[edi + 0CH],edx

        ; Now we need to reset the clip rect in RectAndText since we
        ; switched the sub-channel and the volatile data was lost.
        mov     eax,CHNRUN(RectAndText.ClipPoint0B,11)
        mov     ecx,dword ptr wClipLeft
        mov     edx,dword ptr wClipRight
        mov     ds:[edi + 10H],eax
        mov     ds:[edi + 14H],ecx
        mov     ds:[edi + 18H],edx
        mov     ds:[edi + 1CH],eax  ; Solid color irrelevant but required

        ; Program up the coordinates for the 4 borders edges
        movzx   eax,wBorderLeft
        movzx   ebx,wBorderTop
        movzx   ecx,wBorderRight
        movzx   edx,wBorderBottom

        ; (left,top)      to      (right,top+1)
        mov     ds:[edi + 20H],ax               ; left
        mov     ds:[edi + 22H],bx               ; top
        inc     ebx
        mov     ds:[edi + 24H],cx               ; right
        mov     ds:[edi + 26H],bx               ; top + 1

        ; (right-1,top+1) to      (right,bottom)
        dec     ecx
        mov     ds:[edi + 28H],cx               ; right - 1
        mov     ds:[edi + 2AH],bx               ; top + 1
        inc     ecx
        mov     ds:[edi + 2CH],cx               ; right
        mov     ds:[edi + 2EH],dx               ; bottom

        ; (left,top+1)    to      (left+1,bottom-1)
        mov     ds:[edi + 30H],ax               ; left
        mov     ds:[edi + 32H],bx               ; top + 1
        inc     eax
        dec     edx
        mov     ds:[edi + 34H],ax               ; left + 1
        mov     ds:[edi + 36H],dx               ; bottom - 1

        ; (left,bottom-1) to      (right-1,bottom)
        dec     eax
        mov     ds:[edi + 38H],ax               ; left
        mov     ds:[edi + 3AH],dx               ; bottom - 1
        dec     ecx
        inc     edx
        mov     ds:[edi + 3CH],cx               ; right - 1
        mov     ds:[edi + 3EH],dx               ; bottom

        mov     ebx,ds:[esi].HWLOGDEVCX.pHWBoardCX
        add     edi,40H

DoneIt:
        SETPUTPTR ds,ebx,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_BeginRect
;
; Purpose:      Before calling this function, the generic code will have
;               called a LoadPattern function to load a solid, mono, or
;               color pattern into the pattern registers. After that,
;               this function will be called by the generic code to have
;               the hardware prepare for receiving a list of rectangles.
;               This routine should set the rop, the clip rect, and set
;               the dst base and pitch in preparation for a random number
;               (including 0) of HW_BlockRect calls. When the generic code
;               is done drawing all the rectangles via HW_BlockRect,
;               it will call the HW_EndRect routine to allow the hardware
;               specific code to clean up after itself.
;
; Arguments:
;               ds                      CXSel
;               pHWLogDevCXOffset       HWLOGDEVCX ptr
;               dwDstBase               offset of dst bitmap in video memory
;               dwDstPitch              pitch of dst bitmap in video memory
;               dwRop2                  two operand Rop (1 based)
;               wClipBottom             clip rectangle
;               wClipRight              if there is no clip rect then
;               wClipTop                wClipLeft = wClipTop = 0   and
;               wClipLeft               wclipRight = wClipBottom = 7FFF
;
; Returns:
;               eax             0       Punt to Dibeng
;                               non-0   Success
;
; Preserve:     ds
;==============================================================================
DECPROC HW_BeginRect, PASCAL, FRAME, NEAR
PARMD   pHWLogDevCXOffset
PARMD   dwDstBase
PARMD   dwDstPitch
PARMD   dwRop2
PARMW   wClipBottom
PARMW   wClipRight
PARMW   wClipTop
PARMW   wClipLeft
OPENPROC
        ; Load up the logical device CX and the board CX
        mov     esi,pHWLogDevCXOffset
        mov     ebx,ds:[esi].HWLOGDEVCX.pHWBoardCX

        ; sync with other channels
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ebx,eax

        ; Load the push buffer ptr
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     eax,dwDstBase
        CHECKDSTBASE ds,ebx,edi,eax,<dwDstPitch>

        ; Set the Rop, ColorFormat and Local Clip Rect
        CHECKDMASPACE ds,ebx,edi,20H
        mov     eax,dwRop2
        mov     ecx,ds:[esi].HWLOGDEVCX.dwRectAndTextColorFormat
        movzx   eax,cs:bConvertRop2ToRop3[eax - 1]
        mov     dword ptr ds:[edi + 00H],CHNRUN(ContextRop.SetRop5,1)
        mov     dword ptr ds:[edi + 04H],eax
        mov     dword ptr ds:[edi + 08H],CHNRUN(RectAndText.SetColorFormat,1)
        mov     dword ptr ds:[edi + 0CH],ecx
        mov     eax,dword ptr wClipLeft
        mov     ecx,dword ptr wClipRight
        mov     dword ptr ds:[edi + 10H],CHNRUN(RectAndText.ClipPoint0B,3)
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ecx

        ; The Color1B field value is irrelevant but it must be written
        mov     ds:[edi + 1CH],eax
        add     edi,20H

        mov     ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_BlockRect
;
; Purpose:      This function can be called a random number of times
;               after HW_BeginRect is called and before HW_EndRect is
;               called. See Purpose under HW_BeginRect for more details.
;
; Arguments:
;               ds      CXSel
;               edi     HWLOGDEVCX ptr
;               eax     top:left of rectangle to draw
;               ebx     bottom:right of rectangle to draw
;
; Returns:      None
;
; Preserve:     ds,es,esi,edi,eax,ebx
;==============================================================================
DECPROC HW_BlockRect, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   edi

        ; Load the push buffer ptr
        mov     ecx,ds:[edi].HWLOGDEVCX.pHWBoardCX
        mov     edi,ds:[ecx].HWBOARDCX.pCurrentPushBufferOffset

        mov     edx,CHNRUN(RectAndText.ClippedRectangle.point0,2)
        CHECKDMASPACE ds,ecx,edi,0CH
        mov     ds:[edi + 00H],edx
        mov     ds:[edi + 04H],eax
        mov     ds:[edi + 08H],ebx
        add     edi,0CH
        mov     ds:[ecx].HWBOARDCX.pCurrentPushBufferOffset,edi

        POPR    edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_EndRect
;
; Purpose:      This function will get called after some random number
;               of HW_BlockRect calls. See Purpose
;               under HW_BeginRect for more details.
;
;               You can do whatever you want to do in here. Probably
;               reset the hardware clip rect if one was needed or
;               bump the DMA ptr or whatever.
;
; Arguments:
;               ds      CXSel
;               edi     HWLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_EndRect, PASCAL, NOFRAME, NEAR
OPENPROC

        ; Restore the pattern alpha to -1. It may have been changed
        ; via a hwldpat.asm call prior to the HW_BeginRect call. It
        ; also may have been changed because this output call used
        ; a hatch brush with a transparent background.
        mov     ecx,-1
        mov     ds:[edi].HWLOGDEVCX.dwPatternAlpha0,ecx

        ; Load the push buffer ptr
        mov     ebx,ds:[edi].HWLOGDEVCX.pHWBoardCX
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(ContextPattern.SetMonochromeColor0,1)
        CHECKDMASPACE ds,ebx,edi,08H
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        add     edi,08H

        ; Don't need to reset clip rect here because I use the local
        ; clip rect for this sequence which will get cleared by any
        ; sub-channel change. Hence, just bang the put ptr.
        SETPUTPTR ds,ebx,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_SolidColorRectangle
;
; Purpose:      This function draws a solid color rectangle gievn
;               the coordinates and rop as below. No clipping
;               is done.
;
; Arguments:
;               ds              Context Selector
;               eax             left:top
;               ebx             width:height
;               ecx             dwRop2
;               edx             physical color
;               edi             HWLOGDEVCX ptr
;               gs:esi          DIBENGINEHDR of bitmap in which to draw
;
; Returns:      None
;
; Preserve:     ds
;==============================================================================
DECPROC HW_SolidColorRectangle, PASCAL, NOFRAME, NEAR
OPENPROC
        ; Save the color
        or      edx,ds:[edi].HWLOGDEVCX.dwColorAlphaMask

        ; Save the color format for later use.
        push    ds:[edi].HWLOGDEVCX.dwRectAndTextColorFormat

        ; remap the rop

        movzx   ecx,cs:bConvertRop2ToRop3[ecx - 1]
        push    ecx

        ; Save the pattern alpha
	push	edx
	push	edx
	mov	ds:[edi].HWLOGDEVCX.dwPatternAlpha0,edx
	mov	ds:[edi].HWLOGDEVCX.dwPatternAlpha1,edx

        mov     ecx,ds:[edi].HWLOGDEVCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ecx,edx

        ; Load the push buffer ptr
        mov     edi,ds:[ecx].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     edx,gs:[esi].DIBENGINEHDR.deBitsOffset
        CHECKDSTBASE ds,ecx,edi,edx,<gs:[esi].DIBENGINEHDR.deDeltaScan>

        ; Check for space and program the blt
        CHECKDMASPACE ds,ecx,edi,30H
        mov     dword ptr ds:[edi + 00H],CHNRUN(ContextPattern.SetPatternSelect,3)
        mov     dword ptr ds:[edi + 04H],NV044_SET_PATTERN_SELECT_MONOCHROME
        pop     dword ptr ds:[edi + 08H]
        pop     dword ptr ds:[edi + 0CH]
        mov     dword ptr ds:[edi + 10H],CHNRUN(ContextRop.SetRop5,1)
        pop     dword ptr ds:[edi + 14H]
        mov     dword ptr ds:[edi + 18H],CHNRUN(RectAndText.SetColorFormat,1)
        pop     dword ptr ds:[edi + 1CH]
        mov     dword ptr ds:[edi + 20H],CHNRUN(RectAndText.Color1A,3)
        mov     dword ptr ds:[edi + 24H],0
        mov     ds:[edi + 28H],eax
        mov     ds:[edi + 2CH],ebx
        add     edi,30H
        SETPUTPTR ds,ecx,edi
CLOSEPROC

CLOSESEG _NVTEXT32

END

