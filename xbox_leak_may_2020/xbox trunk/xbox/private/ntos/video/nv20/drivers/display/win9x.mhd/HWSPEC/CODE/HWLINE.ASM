;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwline.asm
;
; Purpose:      This file implements the hardware specific Polyline function.
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

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

PUBLIC  bRop2ToSrc3
bRop2ToSrc3     LABEL   BYTE
DB      000H,   011H,   022H,   033H
DB      044H,   055H,   066H,   077H
DB      088H,   099H,   0AAH,   0BBH
DB      0CCH,   0DDH,   0EEH,   0FFH
DB      000H,   011H,   022H,   033H
DB      044H,   055H,   066H,   077H
DB      088H,   099H,   0AAH,   0BBH
DB      0CCH,   0DDH,   0EEH,   0FFH

PUBLIC  dwLineStyle
dwLineStyle     LABEL   DWORD
DD      000000000H      ; LS_SOLID
DD      00000FF00H      ; LS_DASHED
DD      00F0F0F0FH      ; LS_DOTTED
DD      00FF0FF00H      ; LS_DOTDASHED
DD      00F0F0F00H      ; LS_DASHDOTDOT
DD      000000000H      ; LS_SOLID
DD      000FF0000H      ; LS_DASHED
DD      0F0F0F0F0H      ; LS_DOTTED
DD      000FF0FF0H      ; LS_DOTDASHED
DD      000F0F0F0H      ; LS_DASHDOTDOT
DD      000000000H      ; LS_SOLID
DD      000FF0000H      ; LS_DASHED
DD      0F0F0F0F0H      ; LS_DOTTED
DD      00FF000FFH      ; LS_DOTDASHED
DD      0F0F0F000H      ; LS_DASHDOTDOT

PUBLIC  dwMultiPassSafe
dwMultiPassSafe LABEL   DWORD  ; Rop3  Reverse Polish
DD      1               ; Rop2 =  00   0
DD      0               ; Rop2 =  11   DSon
DD      1               ; Rop2 =  22   DSna
DD      1               ; Rop2 =  33   Sn
DD      0               ; Rop2 =  44   SDna
DD      0               ; Rop2 =  55   Dn
DD      0               ; Rop2 =  66   DSx
DD      0               ; Rop2 =  77   DSan
DD      1               ; Rop2 =  88   DSa
DD      0               ; Rop2 =  99   DSxn
DD      1               ; Rop2 =  AA   D
DD      1               ; Rop2 =  BB   DSno
DD      1               ; Rop2 =  CC   S
DD      0               ; Rop2 =  DD   SDno
DD      1               ; Rop2 =  EE   DSo
DD      1               ; Rop2 =  FF   1

;==============================================================================
;
; Function:     HW_Polyline
;
; Purpose:      This is the hardware specific Polyline routine.
;
; Arguments:
;               ds                      CXSel
;               pHWLogdevCXOffset       HWLOGDEV ptr
;               gs:esi                  lpDstDev ptr
;               lpPoints                list of points
;               dwCount                 Number of points in list lpPoints
;                                       Lines should be drawn between the
;                                       first and second pair of points,
;                                       the second and third pair, the
;                                       third and fourth, etc.  Hence,
;                                       you will draw dwCount - 1 lines.
;               dwPenColor              Line Color
;               dwRop2                  Rop 2 -- from 00H to 0FH inclusive
;               wClipBottom             These four values define the clip
;               wClipRight              rectangle. If wClipLeft equals -1,
;               wClipTop                there is no clip rectangle. The
;               wClipLeft               clip rect is validated by the
;                                       generic code.
;
; Returns:
;               eax     0       punt call to the DIBENG
;                       > 0     the call succeeded
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_Polyline, PASCAL, FRAME, NEAR
PARMD   pHWLogdevCXOffset
PARMD   lpPoints
PARMD   dwCount
PARMD   dwPenColor
PARMD   dwBkColor
PARMD   dwRop
PARMD   dwStyle
PARMW   wClipBottom
PARMW   wClipRight
PARMW   wClipTop
PARMW   wClipLeft
LOCALD  dwCountCopy
LOCALD  pHWBoardCXOffset
OPENPROC
        mov     ebx,pHWLogdevCXOffset
        push    ds:[ebx].HWLOGDEVCX.dwPatternAlpha1
        push    ds:[ebx].HWLOGDEVCX.dwPatternAlpha0

        mov     ebx,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        mov     pHWBoardCXOffset,ebx
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ebx,eax

        ; Load the push buffer ptr
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        mov     eax,gs:[esi].DIBENGINEHDR.deBitsOffset
        CHECKDSTBASE ds,ebx,edi,eax,<gs:[esi].DIBENGINEHDR.deDeltaScan>

        ; The global clip rect must be set if lpClipRect is not NULL
        cmp     wClipLeft,-1
        je      @F
        CHECKDMASPACE ds,ebx,edi,0CH
        mov     eax,CHNRUN(ContextClipRect.SetPoint,2)
        mov     ecx,dword ptr wClipLeft
        mov     edx,dword ptr wClipRight
        sub     edx,ecx
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        mov     ds:[edi + 08H],edx
        add     edi,0CH
@@:
        ; Set the rop and then the color
        CHECKDMASPACE ds,ebx,edi,34H
        mov     dword ptr ds:[edi + 00H],CHNRUN(ContextPattern.SetMonochromeColor0,2)
        pop     dword ptr ds:[edi + 04H]
        pop     dword ptr ds:[edi + 08H]

        mov     eax,dwRop
        mov     ebx,CHNRUN(ContextRop.SetRop5,1)
        movzx   ecx,cs:bRop2ToSrc3[eax]
        mov     ds:[edi + 0CH],ebx
        mov     ds:[edi + 10H],ecx

        ; Must set the line object in the sub-channel
        mov     eax,CHNRUN(RenderSolidLinHdr.object,1)
        mov     ecx,OBJ_SOLID_LINE
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ecx
        mov     ebx,pHWLogdevCXOffset
        mov     eax,CHNRUN(RenderSolidLin.SetColorFormat,2)
        mov     edx,ds:[ebx].HWLOGDEVCX.dwSolidLinColorFormat
        mov     ecx,dwPenColor
        mov     ds:[edi + 1CH],eax
        mov     ds:[edi + 20H],edx
        mov     ds:[edi + 24H],ecx

        ; Blast in the first pair of points
        sub     esi,esi
        les     si,lpPoints
        mov     eax,CHNRUN(RenderSolidLin.Lin.point0,2)
        mov     ecx,es:[esi]
        mov     edx,es:[esi + 4]
        add     esi,8
        mov     ds:[edi + 28H],eax
        mov     ds:[edi + 2CH],ecx
        mov     ds:[edi + 30H],edx
        add     edi,34H

        ; If we are done, then get out. Otherwise wait for space.
        ; We can draw 20H lines with one header and we have already
        ; used the first two points to draw the first line. Hence
        ; the number of dwords of space we need is:
        ; 21H * (dwCount >> 5) + (dwCount AND 1FH) +
        ;       1 (if (dwCount AND 1FH) != 0)
        mov     ecx,dwCount
        sub     ecx,2
        jle     Next
        mov     edx,ecx
        mov     ebx,ecx
        shr     edx,5
        and     ebx,1FH
        imul    eax,edx,(21H * 4)
        sub     ecx,ecx
        cmp     ebx,1
        sbb     ecx,-1
        add     ecx,ebx
        lea     eax,[eax][ecx * 4]
        mov     ecx,pHWBoardCXOffset
        CHECKDMASPACE ds,ecx,edi,eax

        ; There are edx chunks of 20H lines to do
        or      edx,edx
        je      Parts
Big1:   mov     ecx,08H
        mov     dword ptr ds:[edi],CHNRUN(RenderSolidLin.PolyLin,20H)
        add     edi,4
Small1: mov     eax,es:[esi]
        mov     ds:[edi],eax
        mov     eax,es:[esi + 04H]
        mov     ds:[edi + 04H],eax
        mov     eax,es:[esi + 08H]
        mov     ds:[edi + 08H],eax
        mov     eax,es:[esi + 0CH]
        mov     ds:[edi + 0CH],eax
        add     esi,10H
        add     edi,10H
        dec     ecx
        jne     Small1
        dec     edx
        jne     Big1

Parts:  ; There are ebx lines left to do
        or      ebx,ebx
        je      Next
        mov     ecx,ebx
        shl     ebx,10H + 2
        mov     bx,CHNRUN(RenderSolidLin.PolyLin,0)
        mov     ds:[edi],ebx
        add     edi,4
@@:     mov     eax,es:[esi]
        add     esi,4
        mov     ds:[edi],eax
        add     edi,04H
        dec     ecx
        jne     @B
Next:
        ; Reset the clip rect if needed
        cmp     wClipLeft,-1
        je      Done
        mov     ecx,pHWBoardCXOffset
        CHECKDMASPACE ds,ecx,edi,0CH
        mov     eax,CHNRUN(ContextClipRect.SetPoint,2)
        sub     ecx,ecx
        mov     edx,07FFF7FFFH
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        mov     ds:[edi + 08H],edx
        add     edi,0CH
Done:
        mov     ebx,pHWBoardCXOffset
        SETPUTPTR ds,ebx,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_PolyStyledLine
;
; Purpose:      This is the hardware specific PolyStyledLine routine.
;
; Arguments:
;               ds                      CXSel
;               pHWLogdevCXOffset       HWLOGDEV ptr
;               lpDstDev                lpDstDev ptr
;               lpPoints                list of points
;               dwCount                 Number of points in list lpPoints
;                                       Lines should be drawn between the
;                                       first and second pair of points,
;                                       the second and third pair, the
;                                       third and fourth, etc.  Hence,
;                                       you will draw dwCount - 1 lines.
;               dwPenColor              Line Color
;               dwRop2                  Rop 2 -- from 00H to 0FH inclusive
;               wClipBottom             These four values define the clip
;               wClipRight              rectangle. If wClipLeft equals -1,
;               wClipTop                there is no clip rectangle. The
;               wClipLeft               clip rect is validated by the
;                                       generic code.
;
; Returns:
;               eax     0       punt call to the DIBENG
;                       > 0     the call succeeded
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_PolyStyledLine, PASCAL, FRAME, NEAR
PARMD   pHWLogdevCXOffset
PARMD   lpPoints
PARMD   dwCount
PARMD   dwPenColor
PARMD   dwBkColor
PARMD   dwRop
PARMD   dwStyle
PARMW   wClipBottom
PARMW   wClipRight
PARMW   wClipTop
PARMW   wClipLeft
LOCALD  dwDestLeft
LOCALD  dwDestRight
LOCALD  dwStyleOffset
LOCALD  dwHWStyle
LOCALD  dwLocalStyle
LOCALD  dwRet
LOCALD  pHWBoardCXOffset
OPENPROC
        cmp     word ptr dwCount,2
        je      @f

        ; We can't handle some rops that change if we render more than once.
        ; We need to be able to back out and fail to DIBENG if there are
        ; non-slope0 lines bury inside a long chain of polylines
        mov     eax,dwRop
        cmp     cs:dwMultiPassSafe[eax*4],1
        jne     Fail
@@:
        mov     ecx,gs:[esi].DIBENGINEHDR.deBitsOffset
        mov     edx,gs:[esi].DIBENGINEHDR.deDeltaScan
        xor     esi,esi
        les     si,lpPoints
        mov     eax,es:[esi]
        mov     ebx,es:[esi + 4]
        add     esi,4
        call    SETUPPATTERNHWFORSTYLELINE
        test    eax,eax
        jz      Fail

DoStyleLine:
        dec     word ptr dwCount
        mov     dwRet,1

        mov     ebx,pHWLogdevCXOffset
        push    ds:[ebx].HWLOGDEVCX.dwPatternAlpha1
        push    ds:[ebx].HWLOGDEVCX.dwPatternAlpha0

        ; Sync to other contexts
        mov     ebx,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        mov     pHWBoardCXOffset,ebx
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ebx,eax

        ; Load the push buffer ptr
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Set the dst base and pitch if need be
        CHECKDSTBASE ds,ebx,edi,ecx,edx

        ; Set the pattern color even if it is not used
        CHECKDMASPACE ds,ebx,edi,0CH
        mov     eax,CHNRUN(ContextPattern.SetMonochromeColor0,2)
        mov     ds:[edi + 00H],eax
        pop     dword ptr ds:[edi + 04H]
        pop     dword ptr ds:[edi + 08H]
        add     edi,0CH

DoNextLine:
        ; Check for space and program the blt
        mov     edx,pHWBoardCXOffset
        CHECKDMASPACE ds,edx,edi,44H
        mov     eax,CHNRUN(ContextPattern.SetMonochromeShape,6)
        mov     ebx,dwHWStyle
        mov     ecx,NV044_SET_PATTERN_SELECT_MONOCHROME
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx
        mov     ds:[edi + 08H],ecx
        mov     eax,dwPenColor
        mov     ecx,dwBkColor
        mov     edx,pHWLogdevCXOffset
        or      eax,ds:[edx].HWLOGDEVCX.dwColorAlphaMask
        or      ecx,ds:[edx].HWLOGDEVCX.dwColorAlphaMask
        mov     ds:[edi + 0CH],eax
        mov     ds:[edi + 10H],ecx
        mov     eax,dwLocalStyle
        mov     eax,cs:dwLineStyle[eax*4]
        mov     ecx,dwStyleOffset
        ror     eax,cl
        bswap   eax
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],eax
        mov     eax,dwRop
        movzx   ecx,cs:bConvertRop2ToRop3[eax]
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        mov     ds:[edi + 1CH],eax
        mov     ds:[edi + 20H],ecx
        xor     ecx,ecx
        mov     edx,7FFF7FFFH
        cmp     wClipLeft,-1
        je      @F
        mov     ecx,dword ptr wClipLeft
        mov     edx,dword ptr wClipRight
@@:
        mov     eax,pHWLogdevCXOffset
        mov     dword ptr ds:[edi + 24H],CHNRUN(RectAndText.SetColorFormat,1)
        mov     eax,ds:[eax].HWLOGDEVCX.dwSolidLinColorFormat
        mov     ds:[edi + 28H],eax

        mov     eax,CHNRUN(RectAndText.ClipPoint0B,5)
        mov     ds:[edi + 2CH],eax
        mov     ds:[edi + 30H],ecx
        mov     ds:[edi + 34H],edx
        mov     ecx,dwDestLeft
        mov     edx,dwDestRight
        ; The color field next is irrelevant but NV architecture requires it
        mov     ds:[edi + 38H],eax
        mov     ds:[edi + 3CH],ecx
        mov     ds:[edi + 40H],edx
        add     edi,44H
        dec     word ptr dwCount
        jle     CleanUp
        mov     eax,es:[esi]
        mov     ebx,es:[esi + 4]
        add     esi,4
        push    edi
        call    SETUPPATTERNHWFORSTYLELINE
        pop     edi
        test    eax,eax
        jnz     DoNextLine
        mov     dwRet,eax

CleanUp:
        mov     edx,pHWBoardCXOffset
        CHECKDMASPACE ds,edx,edi,14H
        mov     eax,CHNRUN(ContextPattern.SetMonochromeShape,4)
        mov     ebx,NV044_SET_MONOCHROME_SHAPE_8X_8Y
        mov     ecx,NV044_SET_PATTERN_SELECT_MONOCHROME
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ebx
        mov     ds:[edi + 08H],ecx
        mov     eax,-1
        mov     ds:[edi + 0CH],eax
        mov     ds:[edi + 10H],eax
        add     edi,14H
        SETPUTPTR ds,edx,edi
        mov     eax,dwRet
        jmp     Done
Fail:
        xor     eax,eax
Done:
CLOSEPROC


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SETUPPATTERNHWFORSTYLELINE
;
; This macro breaks down the input line into HW rectagular format.  The
; style offset of each start line is also computed and stored.
;
; dwDestLeft    line describes in HW rect (left, top)
; dwDestRight   line describes in HW rect (right, bottom)
; dwStyleOffset Offset to the pattern for starting pixel
; dwHWStyle     Format of the Style (x major or y major)
;
; Arguments:
;       eax     Point0 (x,y)
;       ebx     Point1 (x,y)
;
; Returns:
;       eax     0   Cannot convert.  The line is not a slope 0 line
;       eax     1   Success
;
PUBLIC  SETUPPATTERNHWFORSTYLELINE
SETUPPATTERNHWFORSTYLELINE      PROC    NEAR

        xor     edi,edi
        cmp     ax,bx
        jne     NotVerticalLine
        cmp     eax,ebx
        jle     VerticalForwardLine
        mov     edi,eax
        mov     eax,ebx
        mov     ebx,edi
        add     eax,10000H
        add     ebx,10001H
        mov     dwDestLeft,eax
        mov     dwDestRight,ebx
        ror     eax,16
        dec     eax
        and     eax,31
        mov     edi,39
        sub     edi,eax
        mov     dwStyleOffset,edi
        mov     eax,5
        add     eax,dwStyle
        mov     dwLocalStyle,eax
        mov     eax,NV044_SET_MONOCHROME_SHAPE_1X_64Y
        mov     dwHWStyle,eax
        jmp     Success1

VerticalForwardLine:
        inc     ebx
        mov     dwDestLeft,eax
        mov     dwDestRight,ebx
        ror     eax,16
        and     eax,31
        mov     dwStyleOffset,eax
        mov     eax,NV044_SET_MONOCHROME_SHAPE_1X_64Y
        mov     dwHWStyle,eax
        mov     eax,dwStyle
        mov     dwLocalStyle,eax
        jmp     Success1

NotVerticalLine:
        ror     eax,16
        ror     ebx,16
        cmp     ax,bx
        jne     Fail1
        cmp     eax,ebx
        jle     HorizontalForwardLine

        ;!!! Punt Horizontal Backward Styled Line to dibengine for now. Temporal fix for 33580.
        jmp     Fail1

        mov     edi,eax
        mov     eax,ebx
        mov     ebx,edi
        ror     eax,16
        ror     ebx,16
        inc     eax
        add     ebx,10001H
        mov     dwDestLeft,eax
        mov     dwDestRight,ebx
        dec     eax
        and     eax,31
        mov     edi,41
        sub     edi,eax
        mov     dwStyleOffset,edi
        mov     eax,10
        add     eax,dwStyle
        mov     dwLocalStyle,eax
        mov     eax,NV044_SET_MONOCHROME_SHAPE_64X_1Y
        mov     dwHWStyle,eax
        jmp     Success1

HorizontalForwardLine:
        inc     ebx
        ror     eax,16
        ror     ebx,16
        mov     dwDestLeft,eax
        mov     dwDestRight,ebx
        and     eax,31
        mov     dwStyleOffset,eax
        mov     eax,NV044_SET_MONOCHROME_SHAPE_64X_1Y
        mov     dwHWStyle,eax
        mov     eax,dwStyle
        mov     dwLocalStyle,eax

Success1:
        mov     eax,1
        jmp     Done1
Fail1:
        xor     eax,eax
Done1:
        ret

SETUPPATTERNHWFORSTYLELINE      ENDP

CLOSESEG _NVTEXT32

END

