;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwvrmblt.asm
;
; Purpose:      This file performs BitBlts involving a source when the
;               source and dst are both in video memory. The blt may
;               also involve a pattern but it does not have to.
;
;==============================================================================
.586
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

;==============================================================================
;
; Function:     HW_BitBltSrcVramDstVram
;
; Purpose:      This function implements Bitblts that involve a source,
;               and both the source and dst are in video memory. There
;               may also be a pattern involved inthe rop, but there may
;               not be.
;
; Arguments:
;               ds                      CXSel
;               pHWLogdevCXOffset       GWLOGDEVCX ptr
;               lpDstDev                ptr to dst device (DIBENGINE format)
;               lpSrcDev                ptr to src device (DIBENGINE format)
;               lpBrush                 ptr to DIB_Brush
;               lpDrawMode              ptr to DRAWMODE
;               wDstX                   dst left edge coord
;               wDstY                   dst top  edge coord
;               wSrcX                   src left edge coord
;               wSrcY                   src top  edge coord
;               wExtX                   width of blt rectangle
;               wExtY                   height of blt rectangle
;               dwRop3                  rop in standard Microsoft format
;               dwRopFlags              one of: NOROP, OND, PAD, SAD, PSD
;                                       which are defined in gngdi.inc
;
; Returns:
;               If you want to
;               report this     then do this
;               Success         eax should be set to a number > 0
;               Failure         eax should be set equal to 0
;               Punt to DIBENG  eax should be set to a number < 0
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_BitBltSrcVramDstVram, PASCAL, FRAME, NEAR
PARMD   pHWLogdevCXOffset
PARMD   lpDstDev
PARMD   lpSrcDev
PARMD   lpBrush
PARMD   lpDrawMode
PARMW   wDstX
PARMW   wDstY
PARMW   wSrcX
PARMW   wSrcY
PARMW   wExtX
PARMW   wExtY
PARMD   dwRop3
OPENPROC
        PUSHR   esi,edi

        ; Load the board ptr
        mov     esi,pHWLogdevCXOffset
        push    ds:[esi].HWLOGDEVCX.dwPatternAlpha1
        push    ds:[esi].HWLOGDEVCX.dwPatternAlpha0

        mov     esi,ds:[esi].HWLOGDEVCX.pHWBoardCX

        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,esi,eax

        ; Load the push buffer ptr
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        sub     ecx,ecx

        ; Set the src base and pitch if need be
        sub     edx,edx
        les     dx,lpSrcDev
        mov     eax,es:[edx].DIBENGINEHDR.deBitsOffset
        cmp     eax,ds:[esi].HWBOARDCX.dwContextSurface2dSrcOffset
        je      Skip
        mov     ds:[esi].HWBOARDCX.dwContextSurface2dSrcOffset,eax
        mov     eax,es:[edx].DIBENGINEHDR.deDeltaScan
        mov     word ptr ds:[esi].HWBOARDCX.dwContextSurface2dDstSrcPitch,ax
        mov     ecx,1
Skip:
        ; Set the dst base and pitch if need be
        sub     edx,edx
        les     dx,lpDstDev
        mov     eax,es:[edx].DIBENGINEHDR.deBitsOffset
        cmp     eax,ds:[esi].HWBOARDCX.dwContextSurface2dDstOffset
        je      Skip1
        mov     ds:[esi].HWBOARDCX.dwContextSurface2dDstOffset,eax
        mov     eax,es:[edx].DIBENGINEHDR.deDeltaScan
        mov     word ptr ds:[esi].HWBOARDCX.dwContextSurface2dDstSrcPitch+2,ax
        mov     ecx,1
Skip1:
        or      ecx,ecx
        je      @F
        movzx   eax,word ptr ds:[esi].HWBOARDCX.dwContextSurface2dDstSrcPitch+2
        push    esi
        push    ds:[esi].HWBOARDCX.dwContextSurface2dDstOffset
        push    eax
        call    SetDstBaseAndPitchToDsEdi
@@:
        ; Now fire off the blt
        CHECKDMASPACE ds,esi,edi,24H
        mov     dword ptr ds:[edi + 00H],CHNRUN(ContextPattern.SetMonochromeColor0,2)
        pop     dword ptr ds:[edi + 04H]
        pop     dword ptr ds:[edi + 08H]
        mov     eax,CHNRUN(ContextRop.SetRop5,1)
        movzx   ebx,byte ptr [dwRop3 + 2]
        mov     ds:[edi + 0CH],eax
        mov     ds:[edi + 10H],ebx
        mov     eax,CHNRUN(ImageBlit.ControlPointIn,3)
        mov     ebx,dword ptr wSrcY
        ror     ebx,10H
        mov     ds:[edi + 14H],eax
        mov     ds:[edi + 18H],ebx
        mov     eax,dword ptr wDstY
        mov     ebx,dword ptr wExtY
        ror     eax,10H
        ror     ebx,10H
        mov     ds:[edi + 1CH],eax
        mov     ds:[edi + 20H],ebx
        add     edi,24H
        SETPUTPTR ds,esi,edi

        mov     eax,1
        POPR    esi,edi
CLOSEPROC

CLOSESEG _NVTEXT32

END

