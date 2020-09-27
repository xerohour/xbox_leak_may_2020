;==============================================================================
;
; Copyright (C) 1998,Nvidia Corporation
;
; File:         gntext.asm
;
; Purpose:      This file holds the ExtTextOut display driver entry point.
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
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc
include gngdi.inc
include hwgdi.inc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GLYPHLOOP MACRO IndexSize,TableSize,IsKerning
;
; This macro generates all the inner loops for the text code. There are
; three arguments each of which can be one of two options giving a total
; of 8 inner loops. The options for each argument are as follows:
; IndexSize     ByteGlyph or WordGlyph
; TableSize     WordTable or DwordTable
; IsKerning     Kerning   or NoKerning
;
; The following registers/variables are expected to be setup before
; invoking the macro. The registers on the far right are just notes -
; they describe what registers will be used to hold the variable
; values when they are being used in the macro.
;
; ds                    NewFontSeg selector
; dwGlyphCount          number of glyphs to do
; word ptr lpString+2   selector to list of glyphs      gs
; dwStringOffset        offset to list of glyphs        ecx
; word ptr lpKerning+2  selector to Kerning Vector      fs
; dwkerningOffset       offset to Kerning vector        esi

GLYPHLOOP       MACRO   IndexSize,TableSize,IsKerning
        LOCAL   Loopy

        ;; Load up a few ptrs outside the main loop
        mov     gs,word ptr lpString+2

        IFIDNI  <IsKerning>,<Kerning>
                mov     fs,word ptr lpKerning+2
        ENDIF
Loopy:
        mov     ecx,dwStringOffset
        mov     edi,(NewFontSeg PTR ds:[0]).nfGlyphOffset

        IFDIFI  <IsKerning>,<Kerning>
                ;; If there is no Kerning,we need to know the character width
                mov     esi,(NewFontSeg PTR ds:[0]).nfAWTable
        ENDIF

        ;; Load a byte or word character glyph based upon the glyph size
        IFIDNI  <IndexSize>,<ByteIndex>
                movzx   edx,byte ptr gs:[ecx]
                inc     ecx
        ELSE
                movzx   edx,word ptr gs:[ecx]
                add     ecx,2
        ENDIF

        ;; Handle SMALLBITGLYHP and LARGEBITGLYPH separately.
        IFIDNI  <TableSize>,<WordTable>
                ;; If there is no Kerning,save the glyph width for later
                IFDIFI  <IsKerning>,<Kerning>
                        movzx   eax,word ptr ds:[esi][edx * 2]
                        movzx   esi,word ptr ds:[edi][edx * 2]
                        mov     dwStringOffset,ecx
                        mov     dwGlyphWidth,eax
                ELSE
                        movzx   esi,word ptr ds:[edi][edx * 2]
                        mov     dwStringOffset,ecx
                ENDIF

                mov     ebx,dwDstYDstX
                movsx   eax,ds:[esi].SMALLBITGLYPH.sbgOrgX
                movsx   ecx,ds:[esi].SMALLBITGLYPH.sbgOrgY
                movsx   edx,ds:[esi].SMALLBITGLYPH.sbgHeight
                add     bx,ax
                shl     ecx,10H
                shl     edx,10H

	        ;; Handle the right edge clipping
		cmp     bx,word ptr dwClipRight
	        jge     Doneit

                sub     ebx,ecx
                or      dl,ds:[esi].SMALLBITGLYPH.sbgWidth
                je      next1
                movzx   ecx,ds:[esi].SMALLBITGLYPH.sbgPixels
                add     esi,SIZE SMALLBITGLYPH
        ELSE
                IFDIFI  <IsKerning>,<Kerning>
                        movzx   eax,word ptr ds:[esi][edx * 2]
                        mov     esi,dword ptr ds:[edi][edx * 4]
                        mov     dwStringOffset,ecx
                        mov     dwGlyphWidth,eax
                ELSE
                        mov     esi,dword ptr ds:[edi][edx * 4]
                        mov     dwStringOffset,ecx
                ENDIF

                mov     ebx,dwDstYDstX
                mov     edx,dword ptr ds:[esi].LARGEBITGLYPH.lbgWidth
                add     bx,ds:[esi].LARGEBITGLYPH.lbgOrgX
                mov     ecx,ds:[esi].LARGEBITGLYPH.lbgPixels

	        ;; Handle the right edge clipping
		cmp     bx,word ptr dwClipRight
	        jge     Doneit

                ror     ebx,10H
                sub     bx,ds:[esi].LARGEBITGLYPH.lbgOrgY
                or      dx,dx
                je      next1
                ror     ebx,10H
                add     esi,SIZE LARGEBITGLYPH
        ENDIF

        ;; Call the hardware specific routine to blt the glyph
        call    HW_TextBlt

next1:
        IFIDNI  <IsKerning>,<Kerning>
                mov     edi,dwKerningOffset
                movsx   eax,word ptr fs:[edi]
                add     edi,2
                add     eax,dwDstYDstX
                mov     dwKerningOffset,edi
                mov     word ptr dwDstYDstX,ax
        ELSE
                mov     eax,dwGlyphWidth
                add     eax,dwDstYDstX
                mov     word ptr dwDstYDstX,ax
        ENDIF

        ;; Do all the glyphs in the string
        dec     dwGlyphCount
        jg      Loopy
Doneit:
ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


OPENSEG  _DATA,USE16,DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32,%DEF_CODE_SIZE,CODE

PUBLIC  GlyphBltTable
GlyphBltTable   LABEL   NEAR
DD      OFFSET  GlyphBltByteIndexWordTableIsKerning
DD      OFFSET  GlyphBltWordIndexWordTableIsKerning
DD      OFFSET  GlyphBltByteIndexWordTableNoKerning
DD      OFFSET  GlyphBltWordIndexWordTableNoKerning
DD      OFFSET  GlyphBltByteIndexDWordTableIsKerning
DD      OFFSET  GlyphBltWordIndexDWordTableIsKerning
DD      OFFSET  GlyphBltByteIndexDWordTableNoKerning
DD      OFFSET  GlyphBltWordIndexDWordTableNoKerning

;==============================================================================
;
; Function:     GENERIC_ExtTextOut
;
; Purpose:      This function is the ExtTextOut display driver entry point.
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
DECPROC GENERIC_ExtTextOut,PASCAL,FRAME,FAR16
EXTTEXTOUT_FRAME_AND_LOCALS
OPENPROC
        PUSHR   ds,esi,edi

        ; Grab some arguments for initial filtering
        movsx   edx,wCount
        movzx   eax,wOptions
        mov     ds,word ptr lpDstDev+2
        movzx   esi,word ptr lpDstDev+0

        ; If this is a sizing call,then punt it.
        or      edx,edx
        jl      GN_ExtTextOutPunt

        ; If the text is anti-aliased,then punt it.
        test    ah,ETO_LEVEL_MODE / 256
        jnz     GN_ExtTextOutPunt

        ; If the font is not packed properly,then punt it.
        test    ah,ETO_BIT_PACKED / 256
        jz      GN_ExtTextOutPunt

        ; Grab some parameters from the dst.
        movzx   ecx,ds:[esi].DIBENGINE.deType
        movzx   eax,ds:[esi].DIBENGINE.deFlags

        ; If the font is not in VRAM,then punt it.
        or      ecx,ecx
        je      GN_ExtTextOutPunt
        test    ah,VRAM / 256
        jz      GN_ExtTextOutPunt

        mov     es,cs:[wFlatDataSel]
        mov     ebx,cs:[CSpGNLogicalDevice]

        ; If the text operation involves palette translation then punt it.
        test    ah,PALETTE_XLAT / 256
        jnz     GN_ExtTextOutPunt

        mov     eax,es:[ebx].GNLOGDEVCX.pHWLogdevCX
        mov     ecx,es:[ebx].GNLOGDEVCX.pGNBoardCX
        cmp     es:[ebx].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_ExtTextOutPunt
        mov     edx,es:[ecx].GNBOARDCX.pHWBoardCX
        mov     pHWLogDevCXOffset_ForExtTextOut,eax
        mov     pHWBoardCXOffset_ForExtTextOut,edx

        ; Save off a few things from the dst device we will need.
        mov     eax,dword ptr ds:[esi].DIBENGINE.deBits
        mov     ecx,ds:[esi].DIBENGINE.deDeltaScan
        mov     dwDstBitsOffset,eax
        mov     dwDstBitsDeltaScan,ecx

        ; We will need the colors from the DRAWMODE structure
        sub     edi,edi
        lds     di,lpDrawMode
        mov     eax,ds:[edi].DRAWMODE.TextColor
        mov     edx,ds:[edi].DRAWMODE.bkColor
        mov     dwFgColor,eax
        mov     dwBgColor,edx

        ; Save the clip rect now for later use.
        lds     di,lpClipRect
        mov     eax,dword ptr ds:[edi].RECT.left
        mov     edx,dword ptr ds:[edi].RECT.right
        mov     dword ptr wOpaqueClipRect.left,eax
        mov     dword ptr wOpaqueClipRect.right,edx
        mov     dword ptr wTextClipRect.left,eax
        mov     dword ptr wTextClipRect.right,edx

        ; Draw opaque rectangles if there are some
        cmp     lpOpaqueRects,0
        je      @F
        call    HW_DrawOpaqueRects
        or      eax,eax
        je      GN_ExtTextOutPunt
@@:
        ; Is there any text to do?
        movsx   eax,wCount
        or      eax,eax
        jle     GN_ExtTextOutSuccess
        mov     dwGlyphCount,eax

IF  (INTERSECT_TEXTXFORM_WITH_CLIP NE 0)
        cmp     lpTextXForm,0
        je      NoX
        mov     eax,dword ptr wTextClipRect.left
        mov     edx,dword ptr wTextClipRect.right
        sub     edi,edi
        lds     di,lpTextXForm
        cmp     ax,ds:[edi].RECT.left
        jg      @F
        mov     ax,ds:[edi].RECT.left
@@:     cmp     dx,ds:[edi].RECT.right
        jl      @F
        mov     dx,ds:[edi].RECT.right
@@:     cmp     dx,ax
        jle     GN_ExtTextOutSuccess
        ror     eax,10H
        ror     edx,10H
        cmp     ax,ds:[edi].RECT.top
        jg      @F
        mov     ax,ds:[edi].RECT.top
@@:     cmp     dx,ds:[edi].RECT.bottom
        jl      @F
        mov     dx,ds:[edi].RECT.bottom
@@:     cmp     dx,ax
        jle     GN_ExtTextOutSuccess
        ror     eax,10H
        ror     edx,10H
        mov     dword ptr wTextClipRect.left,eax
        mov     dword ptr wTextClipRect.right,edx
NoX:
ENDIF
        ; Setup up variables for the glyph loop
        mov     ds,word ptr lpFontInfo+2
        movzx   ecx,wTextClipRect.right
        mov     edx,dword ptr wDstY
        ror     edx,10H
        mov     dwClipRight,ecx
        mov     dwDstYDstX,edx
        movzx   eax,word ptr lpString
        movzx   ecx,word ptr lpKerning
        mov     dwStringOffset,eax
        mov     dwKerningOffset,ecx

        ; Call the hardware specific routine to prepare for glyphs
        call    HW_TextBegin
        or      eax,eax
        je      GN_ExtTextOutPunt1

HostTextBlt:
        ; Dispatch to the correct handler
        sub     ebx,ebx
        push    OFFSET GN_ExtTextOutRet
        mov     cl,byte ptr ds:[ebx].NewFontSeg.nfFormat
        mov     ebx,lpKerning
        mov     al,byte ptr wOptions
        test    cl,NF_LARGE
        jnz     IsLG
IsSG:   or      ebx,ebx
        jne     IsSGK
IsSGNK: test    al,ETO_GLYPH_INDEX
        jz      GlyphBltByteIndexWordTableNoKerning
        jmp     GlyphBltWordIndexWordTableNoKerning
IsSGK:  test    al,ETO_GLYPH_INDEX
        jz      GlyphBltByteIndexWordTableIsKerning
        jmp     GlyphBltWordIndexWordTableIsKerning
IsLG:   or      ebx,ebx
        jne     IsLGK
IsLGNK: test    al,ETO_GLYPH_INDEX
        jz      GlyphBltByteIndexDWordTableNoKerning
        jmp     GlyphBltWordIndexDWordTableNoKerning
IsLGK:  test    al,ETO_GLYPH_INDEX
        jz      GlyphBltByteIndexDWordTableIsKerning
        jmp     GlyphBltWordIndexDWordTableIsKerning

GENERIC_ExtTextOut  ENDP


;==============================================================================
;
; Functions:    ExtTextOut ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of ExtTextOut. You can either return a success code,a
;               failure code,punt to the DIBENG,etc.
;
; Arguments:    ExtTextOut Display Driver Stack Frame
;               fs      selector of PDEVICE
;
;==============================================================================
PUBLIC  GN_ExtTextOutPunt1
GN_ExtTextOutPunt1      PROC    NEAR
        ; Jump to here when the opaqueing rects have been drawn but you
        ; wish to punt the text.
        mov     lpOpaqueRects,0
GN_ExtTextOutPunt1      ENDP

PUBLIC  GN_ExtTextOutPunt
GN_ExtTextOutPunt     PROC    NEAR
        ; Jump to here when you want to punt the whole call.
        POPR    ds,esi,edi
        STOPPROC
PLABEL	Punt_ExtTextOut
        jmp     DIB_ExtTextOut
GN_ExtTextOutPunt     ENDP

PUBLIC  GN_ExtTextOutRet
GN_ExtTextOutRet  PROC    NEAR
        call    HW_TextEnd
GN_ExtTextOutRet  ENDP

PUBLIC  GN_ExtTextOutSuccess
GN_ExtTextOutSuccess   PROC    NEAR
        mov     eax,1
GN_ExtTextOutSuccess   ENDP

PUBLIC  GN_ExtTextOutExit
GN_ExtTextOutExit     PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 28H
GN_ExtTextOutExit     ENDP


;==============================================================================
;
; Function:     GlyphBltByteIndexWordTableIsKerning
;
; Purpose:      This glyphblt routine handles the case where the:
;               1) Glyph Index is a byte quantity
;               2) The Glyph Table is a word array
;               3) There is a Kerning vector
;
; Arguments:    Same as for the GLYPHLOOP macro
;
;==============================================================================
PUBLIC  GlyphBltByteIndexWordTableIsKerning
GlyphBltByteIndexWordTableIsKerning     PROC    NEAR

        GLYPHLOOP  ByteIndex,WordTable,Kerning
        ret

GlyphBltByteIndexWordTableIsKerning     ENDP



;==============================================================================
;
; Function:     GlyphBltWordIndexWordTableIsKerning
;
; Purpose:      This glyphblt routine handles the case where the:
;               1) Glyph Index is a byte quantity
;               2) The Glyph Table is a word array
;               3) There is a Kerning vector
;
; Arguments:    Same as for the GLYPHLOOP macro
;
;==============================================================================
PUBLIC  GlyphBltWordIndexWordTableIsKerning
GlyphBltWordIndexWordTableIsKerning     PROC    NEAR

        GLYPHLOOP  WordIndex,WordTable,Kerning
        ret

GlyphBltWordIndexWordTableIsKerning     ENDP



;==============================================================================
;
; Function:     GlyphBltByteIndexWordTableNoKerning
;
; Purpose:      This glyphblt routine handles the case where the:
;               1) Glyph Index is a byte quantity
;               2) The Glyph Table is a word array
;               3) There is a Kerning vector
;
; Arguments:    Same as for the GLYPHLOOP macro
;
;==============================================================================
PUBLIC  GlyphBltByteIndexWordTableNoKerning
GlyphBltByteIndexWordTableNoKerning PROC    NEAR

        GLYPHLOOP  ByteIndex,WordTable,NoKerning
        ret

GlyphBltByteIndexWordTableNoKerning ENDP



;==============================================================================
;
; Function:     GlyphBltWordIndexWordTableNoKerning
;
; Purpose:      This glyphblt routine handles the case where the:
;               1) Glyph Index is a byte quantity
;               2) The Glyph Table is a word array
;               3) There is a Kerning vector
;
; Arguments:    Same as for the GLYPHLOOP macro
;
;==============================================================================
PUBLIC  GlyphBltWordIndexWordTableNoKerning
GlyphBltWordIndexWordTableNoKerning PROC    NEAR

        GLYPHLOOP  WordIndex,WordTable,NoKerning
        ret

GlyphBltWordIndexWordTableNoKerning ENDP



;==============================================================================
;
; Function:     GlyphBltByteIndexDWordTableIsKerning
;
; Purpose:      This glyphblt routine handles the case where the:
;               1) Glyph Index is a byte quantity
;               2) The Glyph Table is a word array
;               3) There is a Kerning vector
;
; Arguments:    Same as for the GLYPHLOOP macro
;
;==============================================================================
PUBLIC  GlyphBltByteIndexDWordTableIsKerning
GlyphBltByteIndexDWordTableIsKerning    PROC    NEAR

        GLYPHLOOP  ByteIndex,DWordTable,Kerning
        ret

GlyphBltByteIndexDWordTableIsKerning    ENDP



;==============================================================================
;
; Function:     GlyphBltWordIndexDWordTableIsKerning
;
; Purpose:      This glyphblt routine handles the case where the:
;               1) Glyph Index is a byte quantity
;               2) The Glyph Table is a word array
;               3) There is a Kerning vector
;
; Arguments:    Same as for the GLYPHLOOP macro
;
;==============================================================================
PUBLIC  GlyphBltWordIndexDWordTableIsKerning
GlyphBltWordIndexDWordTableIsKerning PROC    NEAR

        GLYPHLOOP  WordIndex,DWordTable,Kerning
        ret

GlyphBltWordIndexDWordTableIsKerning ENDP



;==============================================================================
;
; Function:     GlyphBltByteIndexDWordTableNoKerning
;
; Purpose:      This glyphblt routine handles the case where the:
;               1) Glyph Index is a byte quantity
;               2) The Glyph Table is a word array
;               3) There is a Kerning vector
;
; Arguments:    Same as for the GLYPHLOOP macro
;
;==============================================================================
PUBLIC  GlyphBltByteIndexDWordTableNoKerning
GlyphBltByteIndexDWordTableNoKerning PROC    NEAR

        GLYPHLOOP  ByteIndex,DWordTable,NoKerning
        ret

GlyphBltByteIndexDWordTableNoKerning ENDP



;==============================================================================
;
; Function:     GlyphBltWordIndexDWordTableNoKerning
;
; Purpose:      This glyphblt routine handles the case where the:
;               1) Glyph Index is a byte quantity
;               2) The Glyph Table is a word array
;               3) There is a Kerning vector
;
; Arguments:    Same as for the GLYPHLOOP macro
;
;==============================================================================
PUBLIC  GlyphBltWordIndexDWordTableNoKerning
GlyphBltWordIndexDWordTableNoKerning PROC    NEAR

        GLYPHLOOP  WordIndex,DWordTable,NoKerning
        ret

GlyphBltWordIndexDWordTableNoKerning ENDP

CLOSESEG _NVTEXT32

END

