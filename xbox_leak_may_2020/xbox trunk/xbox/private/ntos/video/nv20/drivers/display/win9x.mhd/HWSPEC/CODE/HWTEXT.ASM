;==============================================================================
;
; Copyright (C) 1998,Nvidia Corporation
;
; File:         hwtext.asm
;
; Purpose:      This file implements the hardware specific text functions.
;
;==============================================================================
.586
incDrawMode = 1
incDevice   = 1
incLogical  = 1
incFont     = 1
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include hwmacs.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc

OPENSEG  _DATA,USE16,DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32,%DEF_CODE_SIZE,CODE

;==============================================================================
;
; Function:     HW_DrawOpaqueRects
;
; Purpose:      This function should draw any opaqueing rectangles
;               that exist. Do not do the text now -- that will be
;               done later.
;
;               NOTE: You may set es to anything you wish in this
;               routine and it will not get changed by the generic
;               for the duration of the ExtTextOut call.
;
; Arguments:    Display Driver ExtTextOut frame
;               es              CXSel
;               dwBgColor       the bgColor from the DRAWMODE struct
;                               opaque rects should be drawn in this color
;               wOpaqueClipRect opaque rectangles must be clipped to this
;                               rectangle. It is a RECT struct.
;
; Returns:      eax     0       then rest of ExtTextOut call will be punted
;                               It is not possible to support the text
;                               portion of the call unless you also support
;                               the opaqueing rects portion. However you
;                               can just support the opaqueing rects without
;                               handling the text part by failing TextBegin.
;                       non-0   then ExtTextOut continues
; Preserve:
;==============================================================================
DECPROC HW_DrawOpaqueRects,PASCAL,ShareFrameFar16,NEAR
EXTTEXTOUT_FRAME_AND_LOCALS
OPENPROC
        ; Load the board ptr
        mov     ebx,pHWBoardCXOffset_ForExtTextOut

        ; wait until everyone else goes idle, if they are idle now
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE es,ebx,eax

        ; load the push buffer ptr
        mov     edi,es:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Get ready to do opaqueing rects
        sub     esi,esi
        lds     si,lpOpaqueRects

        ; Set the dst base and pitch if need be
        mov     eax,dwDstBitsOffset
        CHECKDSTBASE es,ebx,edi,eax,dwDstBitsDeltaScan
	mov	edx,pHWLogDevCXOffset_ForExtTextOut

        ; Set the clip rect,color and rop
        CHECKDMASPACE es,ebx,edi,2CH
        mov    	eax,es:[edx].HWLOGDEVCX.dwPatternAlpha0
        mov	ecx,es:[edx].HWLOGDEVCX.dwPatternAlpha1
	mov	dword ptr es:[edi + 00H],CHNRUN(ContextPattern.SetMonochromeColor0,2)
	mov	es:[edi + 04H],eax
	mov	es:[edi + 08H],ecx
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        mov     ecx,0CCH
        mov     es:[edi + 0CH],eax
        mov     es:[edi + 10H],ecx
        mov     eax,CHNRUN(RectAndText.SetColorFormat,1)
        mov     ecx,pHWLogDevCXOffset_ForExtTextOut
        mov     ecx,es:[ecx].HWLOGDEVCX.dwRectAndTextColorFormat
        mov     es:[edi + 14H],eax
        mov     es:[edi + 18H],ecx
        mov     eax,CHNRUN(RectAndText.ClipPoint0B,3)
        mov     ecx,dword ptr wOpaqueClipRect.left
        mov     es:[edi + 1CH],eax
        mov     es:[edi + 20H],ecx
        mov     eax,dword ptr wOpaqueClipRect.right
        mov     ecx,dwBgColor
        mov     es:[edi + 24H],eax
        mov     es:[edi + 28H],ecx
        add     edi,2CH

        ; Draw all opaqeuing rects in the list
Loopy:  mov     edx,dword ptr ds:[esi].RECT.left
        mov     ecx,dword ptr ds:[esi].RECT.right
        add     esi,8
        cmp     cx,dx
        jle     Done
        cmp     ecx,edx
        jle     Done
        mov     eax,CHNRUN(RectAndText.ClippedRectangle.point0,2)
        CHECKDMASPACE es,ebx,edi,0CH
        mov     es:[edi + 00H],eax
        mov     es:[edi + 04H],edx
        mov     es:[edi + 08H],ecx
        add     edi,0CH
        jmp     Loopy
Done:
        SETPUTPTR es,ebx,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_TextBegin
;
; Purpose:      This function gets called by the generic code when
;               it is about to start processing glyphs. After you
;               receive this call,you will get some number of
;               HW_BltGlyph calls and finally a HW_EndText call.
;               The es register will not be changed by the generic
;               code between this routine and the call to HW_TextEnd.
;
; Arguments:    Display Driver ExtTextOut frame.
;               es              CXSel
;               dwFgColor       the fgColor from the DRAWMODE struct
;               dwBgColor       the bgColor from the DRAWMODE struct
;               wTextClipRect   opaque rectangles must be clipped to this
;                               rectangle. It is a RECT struct.
;
; Returns:      eax     0       then text portion will be punted
;                       non-0   then you must handle TextBlt and TextEnd
;
; Preserve:     ds,es
;==============================================================================
DECPROC HW_TextBegin,PASCAL,ShareFrameFar16,NEAR
EXTTEXTOUT_FRAME_AND_LOCALS
OPENPROC
        ; Load the board ptr
        mov     ebx,pHWBoardCXOffset_ForExtTextOut

        ; If there were opaqueing rects,some things are already setup
        cmp     lpOpaqueRects,0
        jne     PartialSetup

        ; We did not do opaque rectangles,so need complete setup.
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE es,ebx,eax

        ; load the push buffer ptr
        mov     edi,es:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     eax,dwDstBitsOffset
        CHECKDSTBASE es,ebx,edi,eax,dwDstBitsDeltaScan
	mov	edx,pHWLogDevCXOffset_ForExtTextOut

        ; Set the clip rect,color and rop
        CHECKDMASPACE es,ebx,edi,2CH
        mov    	eax,es:[edx].HWLOGDEVCX.dwPatternAlpha0
        mov	ecx,es:[edx].HWLOGDEVCX.dwPatternAlpha1
	mov	dword ptr es:[edi + 00H],CHNRUN(ContextPattern.SetMonochromeColor0,2)
	mov	es:[edi + 04H],eax
	mov	es:[edi + 08H],ecx
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        mov     edx,0CCH
        mov     es:[edi + 0CH],eax
        mov     es:[edi + 10H],edx
        mov     eax,CHNRUN(RectAndText.SetColorFormat,1)
        mov     edx,pHWLogDevCXOffset_ForExtTextOut
        mov     edx,es:[edx].HWLOGDEVCX.dwRectAndTextColorFormat
        mov     es:[edi + 14H],eax
        mov     es:[edi + 18H],edx
        mov     eax,CHNRUN(RectAndText.ClipPoint0F,3)
        mov     edx,dword ptr wTextClipRect.left
        mov     es:[edi + 1CH],eax
        mov     es:[edi + 20H],edx
        mov     eax,dword ptr wTextClipRect.right
        mov     edx,dwFgColor
        mov     es:[edi + 24H],eax
        mov     es:[edi + 28H],edx
        add     edi,2CH
        jmp     DoneIt

PartialSetup:
        ; Set the clip rect,color and rop
        CHECKDMASPACE es,ebx,NONE,10H
        mov     edi,es:[ebx].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(RectAndText.ClipPoint0F,3)
        mov     edx,dword ptr wTextClipRect.left
        mov     es:[edi + 00H],eax
        mov     es:[edi + 04H],edx
        mov     eax,dword ptr wTextClipRect.right
        mov     edx,dwFgColor
        mov     es:[edi + 08H],eax
        mov     es:[edi + 0CH],edx
        add     edi,10H
DoneIt:
        mov     es:[ebx].HWBOARDCX.pCurrentPushBufferOffset,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_TextBlt
;
; Purpose:      This function is called to blt an individual glyph.
;               The es register has not changed from the value you
;               assigned to it in HW_TextBegin.
;
; Arguments:    Display Driver ExtTextOut frame
;               es      CXSel
;               ds:esi  points to first byte of glyph data
;               ebx     DstYOrg:DstXOrg
;               edx     DstYExtent:DstXExtent
;               ecx     Total number of bits in the glyph
;
; Preserve:     ds,fs,gs
;==============================================================================
DECPROC HW_TextBlt,PASCAL,ShareFrameFar16,NEAR
EXTTEXTOUT_FRAME_AND_LOCALS
OPENPROC
        mov     edi,pHWBoardCXOffset_ForExtTextOut

        ; See if there are more bytes than we can send out in one shot
        add     ecx,31
        shr     ecx,3
        and     cl,NOT 3
        cmp     ecx,200H
        ja      BreakIntoChunks

        lea     eax,[ecx + 0CH]
        CHECKDMASPACE es,edi,NONE,eax
        mov     edi,es:[edi].HWBOARDCX.pCurrentPushBufferOffset
        sub     eax,4
        shr     ecx,2
        shl     eax,10H
        add     edi,0CH
        mov     ax,CHNRUN(RectAndText.dwSizeC,0)
        mov     es:[edi - 0CH],eax
        mov     eax,pHWBoardCXOffset_ForExtTextOut
        mov     es:[edi - 08H],edx
        mov     es:[edi - 04H],ebx
        rep     movs dword ptr es:[edi],ds:[esi]
        mov     es:[eax].HWBOARDCX.pCurrentPushBufferOffset,edi
        ret

BreakIntoChunks:
        ; Set the width,height and position of the character
        CHECKDMASPACE es,edi,NONE,0CH
        mov     edi,es:[edi].HWBOARDCX.pCurrentPushBufferOffset
        mov     eax,CHNRUN(RectAndText.dwSizeC,2)
        mov     es:[edi + 00H],eax
        mov     es:[edi + 04H],edx
        mov     es:[edi + 08H],ebx
        add     edi,0CH
        mov     edx,ecx
        mov     eax,CHNRUN(RectAndText.MonochromeColor1C,80H)
        mov     ebx,pHWBoardCXOffset_ForExtTextOut
Loopy:
        mov     ecx,200H
        sub     edx,ecx
        jae     @F
        add     edx,ecx
        mov     ecx,edx
        shrd    eax,edx,10H
        mov     ax,CHNRUN(RectAndText.MonochromeColor1C,0)
        xor     edx,edx
@@:
        add     ecx,4
        CHECKDMASPACE es,ebx,edi,ecx
        sub     ecx,4
        mov     es:[edi + 00H],eax
        add     edi,4
        shr     ecx,2
        rep     movs dword ptr es:[edi],ds:[esi]
        or      edx,edx
        jg      Loopy
        SETPUTPTR es,ebx,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_TextEnd
;
; Purpose:      This function is called by the generic code after
;               all the individual glyphs have been bltted. You
;               will receive this call only if you received a
;               HW_TextBegin call. Do anything you need to do in
;               here to clean up after doing text.
;
; Arguments:    Display Driver ExtTextOut frame
;               es      CXSel
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_TextEnd,PASCAL,ShareFrameFar16,NEAR
EXTTEXTOUT_FRAME_AND_LOCALS
OPENPROC
        mov     ebx,pHWBoardCXOffset_ForExtTextOut
        mov     edi,es:[ebx].HWBOARDCX.pCurrentPushBufferOffset
        SETPUTPTR es,ebx,edi
CLOSEPROC

CLOSESEG _NVTEXT32
END

