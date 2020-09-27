;==============================================================================
;
; Copyright (C) 1998, NVidia Coporation
;
; File:         gnblt.asm
;
; Purpose:      This file holds the Biblt display driver entry point.
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

IFDEF DEBUG
PUBLIC  bVramToVram, bMonoToVram, bColorToVram, bToSystem
bVramToVram     DB      0
bMonoToVram     DB      0
bColorToVram    DB      0
bToSystem       DB      0
ENDIF

;==============================================================================
;
; Function:     GENERIC_BitBlt
;
; Purpose:      This function is the BitBlt display driver entry point.
;
; Arguments:    BitBlt Display Driver Stack Frame
;
; Returns:      ax      0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_BitBlt, PASCAL, FRAME, FAR16
BITBLT_FRAME_AND_LOCALS
OPENPROC
        PUSHR   ds,esi,edi

        ; Filter out old style destinations
        mov     gs,word ptr lpDstDev+2
        movzx   esi,word ptr lpDstDev
        movzx   edi,byte ptr [dwRop3 + 2]
        mov     ds,cs:[wFlatDataSel]
        mov     ebx,cs:[CSpGNLogicalDevice]

        cmp     gs:[esi].DIBENGINE.deType,0
        je      GN_BitBltSystemMemoryDst

        ; New style dst with DIBENG Header. Filter based upon deFlags.
        movzx   eax,gs:[esi].DIBENGINE.deFlags
        movzx   edx,ds:[ebx].DIBENGINE.deFlags
        movzx   ecx,cs:Rop3Table[edi]
        test    ah,VRAM / 256
        jz      GN_BitBltSystemMemoryDst
        test    dh,PALETTE_XLAT / 256
        jnz     GN_BitBltPunt

        ; is it by BOARD or by CRTC? Depends how DOS works.
        cmp     ds:[ebx].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_BitBltPunt

        ; Dispatch to routine based upon the rop
        cmp     cl,PAD
        je      GN_BitBltPatDstRop
        movsx   edx,byte ptr [dwRop3 + 2]
        cmp     cl,OND
        je      @f
        cmp     cl,PSD
        je      GN_BitBltPatUnknownSrcUnknownDstVram
        jmp     GN_BitBltSrcUnknownDstVram
@@:
        mov     ebx,ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        movzx   ecx,byte ptr [dwRop3 + 2]
        jmp     HW_BitBltSolidPatDstRop                                         

GENERIC_BitBlt  ENDP


;==============================================================================
;
; Function:     BitBlt ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of BitBlt. You can either return a success code, a failure
;               code, punt to the DIBENG, etc.
;
; Arguments:    BitBlt Display Driver Stack Frame
;
;==============================================================================
PUBLIC  GN_BitBltPunt
GN_BitBltPunt   PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
PLABEL  Punt_BitBlt
        jmp     DIB_BitBlt
GN_BitBltPunt   ENDP

PUBLIC  GN_BitBltFail
GN_BitBltFail   PROC    NEAR
        sub     eax,eax
        jmp     GN_BitBltExit
GN_BitBltFail   ENDP

PUBLIC  GN_BitBltSuccess
GN_BitBltSuccess        PROC    NEAR
        mov     eax,1
GN_BitBltSuccess        ENDP

PUBLIC  GN_BitBltExit
GN_BitBltExit   PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 20H
GN_BitBltExit   ENDP


;==============================================================================
;
; Function:     GN_BitBltPatDstRop
;
; Purpose:      This function does a bitblt when the rop involves a
;               pattern only or a pattern and a dst. This function
;               expects to be jumped to, not called.
;
; Arguments:    BitBlt Display Driver Stack Frame
;               ds              Context selector
;               gs:esi          ptr to lpDstDev
;               edi             rop
;
; Returns:      No returns -- this function is jumped to. When it is done
;               it should jump to one of the BitBlt ending routines.
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  GN_BitBltPatDstRop
GN_BitBltPatDstRop      PROC    NEAR

        mov     es,word ptr lpBrush+2
        movzx   edi,word ptr lpBrush+0
        mov     cl,byte ptr es:[edi].DIB_Brush8.dp8BrushStyle
        mov     al,byte ptr es:[edi].DIB_Brush8.dp8BrushFlags
        mov     edx,dword ptr es:[edi].DIB_Brush8.dp8BrushBits
        mov     ebx,ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        cmp     cl,BS_HOLLOW
        je      GN_BitBltSuccess
        jnc     @F
        test    al,COLORSOLID
        jz      HW_BitBltColorPatDstRop
        movzx   ecx,byte ptr [dwRop3 + 2]
        jmp     HW_BitBltSolidPatDstRop
@@:     cmp     cl,BS_PATTERN
        jc      IsHatch
        ja      GN_BitBltFail
        test    al,PATTERNMONO
        jz      HW_BitBltColorPatDstRop
IsMono:
        sub     edx,edx
        lfs     dx,lpDrawMode
        mov     eax,fs:[edx].DRAWMODE.TextColor
        mov     ecx,fs:[edx].DRAWMODE.bkColor
        lea     edi,[edi].DIB_Brush8.dp8BrushMono
        jmp     HW_BitBltMonoPatDstRop
IsHatch:
        mov     eax,es:[edi].DIB_Brush8.dp8BgColor
        mov     ecx,es:[edi].DIB_Brush8.dp8FgColor
        lea     edi,[edi].DIB_Brush8.dp8BrushMask
        jmp     HW_BitBltMonoPatDstRop

GN_BitBltPatDstRop      ENDP


;==============================================================================
;
; Function:     GN_BitBltPatUnknownSrcUnknownDstVram
;
; Purpose:      This function does a bitblt when the rop involves a src
;               and a pattern and the dst is in Vram. This function
;               expects to be jumped to, not called.
;
;               This routine will call the hardware to set up the
;               pattern and then continue on as if it were just a
;               src,dst blt.
;
; Arguments:    BitBlt Display Driver Stack Frame
;               gs:esi          lpDstDev
;               ds              Context selector
;               ebx             GNLOGDEVCX ptr
;               edi             rop
;
; Returns:      No returns -- this function is jumped to. When it is done
;               it should jump to one of the BitBlt ending routines.
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  GN_BitBltPatUnknownSrcUnknownDstVram
GN_BitBltPatUnknownSrcUnknownDstVram    PROC    NEAR

        mov     es,word ptr lpBrush+2
        movzx   esi,word ptr lpBrush+0
        push    lpDrawMode
        call    GN_PrepareBrush
        or      eax,eax
        jl      GN_BitBltFail
        je      GN_BitBltSuccess

        ; Reload the lpDstDev
        sub     esi,esi
        lgs     si,lpDstDev
        .errnz  $ - OFFSET GN_BitBltSrcUnknownDstVram

GN_BitBltPatUnknownSrcUnknownDstVram    ENDP



;==============================================================================
;
; Function:     GN_BitBltSrcUnknownDstVram
;
; Purpose:      This function does a bitblt when the rop involves a src
;               and the dst is in Vram. It may also involve a pattern
;               and the dst, but it doesn't have to. This function
;               expects to be jumped to, not called.
;
; Arguments:    BitBlt Display Driver Stack Frame
;               gs:esi          lpDstDev
;               ds              Context selector
;
; Returns:      No returns -- this function is jumped to. When it is done
;               it should jump to one of the BitBlt ending routines.
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  GN_BitBltSrcUnknownDstVram
GN_BitBltSrcUnknownDstVram      PROC    NEAR

        ; Load a ptr to the source device
        mov     es,word ptr lpSrcDev+2
        movzx   edi,word ptr lpSrcDev+0

        ; First priority is to clip the source
        call    GN_BitBltClipSrc
        or      eax,eax
        je      GN_BitBltSuccess

        ; Dispatch the blt to the correct place
        sub     eax,eax
        mov     dwScansPerSeg,7FFFH
        movzx   ecx,es:[edi].DIBENGINE.deFlags
        cmp     ax,es:[edi].DIBENGINE.deType
        je      OldStyleSrc
        test    ch,VRAM / 256
        jnz     GN_BitBltSrcVramDstVram
        cmp     es:[edi].DIBENGINE.deBitsPixel,1
        jne     GN_BitBltSrcColorDstVram_1
        mov     eax,dword ptr es:[edi].DIBENGINE.deBits
        mov     bx,word ptr es:[edi].DIBENGINE.deBits+4
        mov     ecx,es:[edi].DIBENGINE.deDeltaScan
        mov     edx,7FFFH
        jmp     GN_BitBltSrcMonoDstVram_1

OldStyleSrc:
        cmp     es:[edi].BITMAP.bmBitsPixel,1
        jne     GN_BitBltPunt
        mov     edx,07FFFH
        cmp     ax,es:[edi].BITMAP.bmSegmentIndex
        je      @F
        movzx   ecx,es:[edi].BITMAP.bmScanSegment
        mov     dwScansPerSeg,ecx
        movzx   eax,wSrcY
        sub     edx,edx
        div     cx
        sub     ecx,edx
        mov     edx,ecx
@@:
        movzx   ecx,es:[edi].BITMAP.bmWidthBytes
        movzx   eax,word ptr es:[edi].BITMAP.bmBits
        movzx   ebx,word ptr es:[edi].BITMAP.bmBits+2
        movzx   edi,es:[edi].BITMAP.bmFillBytes
        jmp     GN_BitBltSrcMonoDstVram_1

GN_BitBltSrcUnknownDstVram      ENDP


;==============================================================================
;
; Function:     GN_BitBltClipSrc
;
; Purpose:      This function clips the src adjusting the stack parameters
;               for BitBlt appropriately to represent the clipped coords
;               and clipped extents.
;
; Arguments:    BitBlt Display Driver Stack Frame
;               es:edi  lpSrcDev
;
; Returns:      eax     1 - there is something to draw
;                       0 - there is nothing to draw
;
; Preserve:     ds,esi,es,edi,fs
;==============================================================================
PUBLIC  GN_BitBltClipSrc
GN_BitBltClipSrc        PROC    NEAR

        movsx   eax,wSrcX
        movsx   ecx,wSrcY
        movzx   ebx,es:[edi].DIBENGINE.deWidth
        movzx   edx,es:[edi].DIBENGINE.deHeight
        or      eax,eax
        js      ClipLeft
cld1:   or      ecx,ecx
        js      ClipTop
clt:    add     ax,wExtX
        add     cx,wExtY
        sub     ax,bx
        jg      ClipRight
clr:    sub     cx,dx
        jg      ClipBottom
clb:    mov     eax,1
        ret

ClipLeft:
        add     wExtX,ax
        jle     ClipDone
        sub     wDstX,ax
        sub     eax,eax
        mov     wSrcX,ax
        jmp     cld1

ClipTop:
        add     wExtY,cx
        jle     ClipDone
        sub     wDstY,cx
        sub     ecx,ecx
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
        sub     eax,eax
        ret

GN_BitBltClipSrc        ENDP


;==============================================================================
;
; Function:     GN_BitBltSrcVramDstVram
;
; Purpose:      This function does a bitblt when the rop involves a source,
;               the source is in VRAM, and the dst is in VRAM. The rop may
;               also involve the dst and a pattern, but it may not. This
;               routine calls the hardware specific routine
;               HW_BitBltSrcVramDstVram to do the actual blt.
;               This function expects to be jumped to, not called.
;
; Arguments:    BitBlt Display Driver Stack Frame
;               es:edi          lpSrcDev
;               gs:esi          lpDstDev
;               ds              Context selector
;
; Returns:      No returns -- this function is jumped to. When it is done
;               it should jump to one of the BitBlt ending routines.
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  GN_BitBltSrcVramDstVram
GN_BitBltSrcVramDstVram PROC    NEAR

IFDEF DEBUG
        cmp     cs:[bVramToVram],0
        jne     GN_BitBltPunt
ENDIF
        mov     ebx,cs:[CSpGNLogicalDevice]
        push    ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        push    lpDstDev
        push    lpSrcDev
        push    lpBrush
        push    lpDrawMode
        push    dword ptr wDstY
        push    dword ptr wSrcY
        push    dword ptr wExtY
        push    dwRop3
        call    HW_BitBltSrcVramDstVram
        or      eax,eax
        jg      GN_BitBltSuccess
        jl      GN_BitBltPunt
        jmp     GN_BitBltFail

GN_BitBltSrcVramDstVram ENDP


;==============================================================================
;
; Function:     GN_BitBltSrcMonoDstVram_1
;
; Purpose:      This function does a bitblt when a source is involved,
;               the source is a mono bitmap, and the dst is in VRAM. The
;               rop may also involve the dst and a pattern, but it may
;               not. This routine calls the generic routine
;               GN_BitBltSrcMonoDstVram_1 to do the actual blt.
;               This function expects to be jumped to, not called.
;
; Arguments:    BitBlt Display Driver Stack Frame
;               es              selector of lpSrcDev
;               gs:esi          selector of lpDstDev
;               ds              Context selector
;               dwScansPerSeg   scans per segment of the mono bitmap
;               edx             starting from and including the wSrcY
;                               coord, this is the number of scanlines
;                               in the mono bitmap before there is a
;                               segment crossing (when you need to
;                               adjust the src pointer to account for
;                               the fillbytes.)
;               edi             padding bytes at the end of each segment
;               bx              src selector points to src bits
;               eax             src offset points to src bits
;               ecx             pitch of src bitmap in bytes
;
; Returns:      No returns -- this function is jumped to. When it is done
;               it should jump to one of the BitBlt ending routines.
;
;==============================================================================
PUBLIC  GN_BitBltSrcMonoDstVram_1
GN_BitBltSrcMonoDstVram_1 PROC  NEAR

IFDEF DEBUG
        cmp     cs:[bMonoToVram],0
        jne     GN_BitBltPunt
ENDIF
        mov     dwSrcBitsOffset,eax

        ; Let the hardware specific code handle the transfer.
        push    cs:[CSpGNLogicalDevice]
        movzx   eax,word ptr gs:[esi].DIBENGINE.deBits+4
        push    eax
        push    dword ptr gs:[esi].DIBENGINE.deBits
        movzx   eax,gs:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    gs:[esi].DIBENGINE.deDeltaScan
        movzx   eax,wDstX
        push    eax
        movzx   eax,wDstY
        push    eax
        push    ebx                             ; src sel
        push    dwSrcBitsOffset                 ; offset to src bits
        push    ecx                             ; src pitch in bytes
        xor     ebx,ebx                         ; clear high word of EBX
        les     bx,lpDrawMode
        movzx   eax,wSrcX
        push    eax
        movzx   eax,wSrcY
        push    eax
        movzx   eax,wExtX
        push    eax
        movzx   eax,wExtY
        push    eax
        push    es:[ebx].DRAWMODE.TextColor
        push    es:[ebx].DRAWMODE.bkColor
        push    dwRop3
        push    lpBrush
        push    lpDrawMode
        push    edx
        push    dwScansPerSeg
        push    edi                             ; fill bytes
        call    GN_BitBltSrcMonoDstVram
        or      eax,eax
        jg      GN_BitBltSuccess
        jl      GN_BitBltFail
        jmp     GN_BitBltPunt

GN_BitBltSrcMonoDstVram_1 ENDP


;==============================================================================
;
; Function:     GN_BitBltSrcColorDstVram_1
;
; Purpose:      This function does a bitblt when a source is involved,
;               the source is a color DIBENG bitmap, and the dst is in
;               VRAM. The rop may also involve the dst and a pattern,
;               but it may not. This routine calls the hardware specific
;               routine HW_BitBltSrcMonoDstVram to do the actual blt.
;               This function expects to be jumped to, not called.
;
; Arguments:    BitBlt Display Driver Stack Frame
;               gs:esi          lpDstDev
;               es:edi          lpSrcDev
;               ds              Context selector
;
; Returns:      No returns -- this function is jumped to. When it is done
;               it should jump to one of the BitBlt ending routines.
;
;==============================================================================
PUBLIC  GN_BitBltSrcColorDstVram_1
GN_BitBltSrcColorDstVram_1      PROC    NEAR

IFDEF DEBUG
        cmp     cs:[bColorToVram],0
        jne     GN_BitBltPunt
ENDIF
        ; Let the vendor handle the whole thing
        mov     ebx,cs:[CSpGNLogicalDevice]
        push    ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        movzx   eax,word ptr gs:[esi].DIBENGINE.deBits+4
        movzx   ecx,gs:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    dword ptr gs:[esi].DIBENGINE.deBits
        push    ecx
        push    gs:[esi].DIBENGINE.deDeltaScan
        movzx   eax,wDstX
        push    eax
        movzx   eax,wDstY
        push    eax
        movzx   eax, word ptr es:[edi].DIBENGINE.deBits+4
        mov     ecx,dword ptr es:[edi].DIBENGINE.deBits+0
        push    eax
        push    ecx
        movzx   eax,es:[edi].DIBENGINE.deBitsPixel
        mov     ecx,es:[edi].DIBENGINE.deDeltaScan
        push    eax
        push    ecx
        movzx   eax,wSrcX
        push    eax
        movzx   eax,wSrcY
        push    eax
        movzx   eax,wExtX
        push    eax
        movzx   eax,wExtY
        push    eax
        push    dwRop3
        push    lpBrush
        push    lpDrawMode
        sub     eax,eax
        push    eax                     ; size color table entry in bytes
        push    eax                     ; num color table entries
        push    eax                     ; lpColorTable
        push    eax
        call    HW_BitBltSrcColorDstVram
        or      eax,eax
        jg      GN_BitBltSuccess
        jl      GN_BitBltFail
        jmp     GN_BitBltPunt

GN_BitBltSrcColorDstVram_1      ENDP


;==============================================================================
;
; Function:     GN_BitBltSystemMemoryDst
;
; Purpose:      This routine gets jumped to when either the lpDstDev
;               points to an old BITMAP header or when lpDstDev points
;               to a new style DIBENGINE header but the deFlags field
;               does not have the VRAM bit set. Both these scenarios
;               describe a bitmap that is in system memory. We may
;               wish to handle a blt where the dst is in system memory
;               if there is also a source located in video memory since
;               we can use hardware DMA to transfer back from VRAM.
;               This function expects to be jumped to, not called.
;
; Arguments:    BitBlt Display Driver Stack Frame
;		ds:ebx		ptr GNLOGDEVCX which is also PDEVICE
;               gs:esi          lpDstDev
;               edi             rop
;
; Returns:      No returns -- this function is jumped to. When it is done
;               it should jump to one of the BitBlt ending routines.
;
;==============================================================================
PUBLIC  GN_BitBltSystemMemoryDst
GN_BitBltSystemMemoryDst        PROC    NEAR

IFDEF DEBUG
        cmp     cs:[bToSystem],0
        jne     GN_BitBltPunt
ENDIF

        ; Make sure hardware is not busy
        cmp     ds:[ebx].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_BitBltPunt

        ; Prepare some things that we need for testing
        movzx   ecx,cs:Rop3Table[edi]

        ; If the dst is a BITMAP hdr, then punt cases where dst is color.
        cmp     gs:[esi].DIBENGINE.deType,0
        movzx   eax,gs:[esi].BITMAP.bmBitsPixel
        jne     IsDib
        cmp     eax,1
        ja      GN_BitBltPunt
        jmp     @F
IsDib:
        ; Punt dsts that we cannot handle
        test    gs:[esi].DIBENGINE.deFlags,PALETTE_XLAT
        jnz     GN_BitBltPunt
@@:
        ; Punt rops not involving a src
        cmp     cl,SAD
        jc      GN_BitBltPunt

        ; Punt if source is in system memory too.
        xor     edi,edi                 ; clear high word of EDI
        les     di,lpSrcDev
        cmp     es:[edi].DIBENGINE.deType,0
        je      GN_BitBltPunt
        test    es:[edi].DIBENGINE.deFlags,VRAM
        jz      GN_BitBltPunt

        ; We must clip to the src rectangle
        call    GN_BitBltClipSrc
        or      eax,eax
        je      GN_BitBltSuccess

        ; Call the generic code to overlap DMAing and converting.
        push    cs:[CSpGNLogicalDevice]
        push    lpDstDev
        pushd   0

        ; Get the dst parameters -- depends upon bitmap header format
        cmp     gs:[esi].DIBENGINE.deType,0
        jne     NewT
        movzx   eax,word ptr gs:[esi].BITMAP.bmBits+2
        movzx   ecx,word ptr gs:[esi].BITMAP.bmBits+0
        movzx   edx,word ptr gs:[esi].BITMAP.bmWidthBytes
        push    eax
        push    ecx
        pushd   1
        push    edx
        jmp     @F

NewT:   movzx   eax,word ptr gs:[esi].DIBENGINE.deBits+4
        movzx   ecx,gs:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    dword ptr gs:[esi].DIBENGINE.deBits
        push    ecx
        push    gs:[esi].DIBENGINE.deDeltaScan
@@:
        movzx   eax,wDstX
        push    eax
        movzx   eax,wDstY
        push    eax
        push    lpSrcDev
        movzx   eax,word ptr es:[edi].DIBENGINE.deBits+4
        movzx   ecx,es:[edi].DIBENGINEHDR.bRealBpp
        push    eax
        push    dword ptr es:[edi].DIBENGINE.deBits
        push    ecx
        push    es:[edi].DIBENGINE.deDeltaScan
        movzx   eax,wSrcX
        push    eax
        movzx   eax,wSrcY
        push    eax
        movzx   eax,wExtX
        push    eax
        movzx   eax,wExtY
        push    eax
        push    dwRop3
        push    lpBrush
        push    lpDrawMode
        sub     eax,eax
        push    eax
        call    GN_BitBltSrcVramDstSystem
        or      eax,eax
        jg      GN_BitBltSuccess
        jl      GN_BitBltFail
        jmp     GN_BitBltPunt

GN_BitBltSystemMemoryDst        ENDP


;==============================================================================
;
; Function:     GN_PrepareBrush
;
; Purpose:      This function figures out the type of brush and
;               call the hardware specific routine to load the
;               brush into the hardware.
;
; Arguments:
;               ds              Context selector
;               ebx             GNLOGDEVCX ptr
;               es:esi          lpBrush
;
; Returns:
;               eax             -1      Failure
;               eax             0       Brush was hollow
;                               1       Success
;
; Preserve:     ds
;==============================================================================
DECPROC GN_PrepareBrush, PASCAL, FRAME, NEAR
PARMD   lpDrawMode
OPENPROC
        mov     cl,byte ptr es:[esi].DIB_Brush8.dp8BrushStyle
        mov     al,byte ptr es:[esi].DIB_Brush8.dp8BrushFlags
        mov     edx,dword ptr es:[esi].DIB_Brush8.dp8BrushBits
        mov     ebx,ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        cmp     cl,BS_HOLLOW
        je      IsHollow
        PUSH    OFFSET ReSetPat
        jnc     @F
        test    al,COLORSOLID
        jz      HW_LoadColorPattern
        jmp     HW_LoadSolidPattern
@@:     cmp     cl,BS_PATTERN
        jc      IsHatch
        ja      IsFail
        test    al,PATTERNMONO
        jz      HW_LoadColorPattern
IsMono:
        sub     edx,edx
        lfs     dx,lpDrawMode
        mov     ecx,fs:[edx].DRAWMODE.TextColor
        mov     edx,fs:[edx].DRAWMODE.bkColor
        lea     esi,[esi].DIB_Brush8.dp8BrushMono
        mov     eax,-1
        jmp     HW_LoadMonoPattern
IsHatch:
        mov     ecx,es:[esi].DIB_Brush8.dp8BgColor
        mov     edx,es:[esi].DIB_Brush8.dp8FgColor
        lea     esi,[esi].DIB_Brush8.dp8BrushMask
        mov     eax,-1
        jmp     HW_LoadMonoPattern

IsHollow:
        sub     eax,eax
        jmp     DoneIt

IsFail:
        mov     eax,-1
        jmp     DoneIt

ReSetPat:
        mov     eax,1
DoneIt:
CLOSEPROC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PUBLIC  Rop3Table
Rop3Table       LABEL   BYTE
DB      OND,    PSD,    PSD,    PSD,    PSD,    PAD,    PSD,    PSD
DB      PSD,    PSD,    PAD,    PSD,    PSD,    PSD,    PSD,    PAD

DB      PSD,    SAD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD
DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD

DB      PSD,    PSD,    SAD,    PSD,    PSD,    PSD,    PSD,    PSD
DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD

DB      PSD,    PSD,    PSD,    SAD,    PSD,    PSD,    PSD,    PSD
DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD

DB      PSD,    PSD,    PSD,    PSD,    SAD,    PSD,    PSD,    PSD
DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD

DB      PAD,    PSD,    PSD,    PSD,    PSD,    OND,    PSD,    PSD
DB      PSD,    PSD,    PAD,    PSD,    PSD,    PSD,    PSD,    PAD

DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    SAD,    PSD
DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD

DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    SAD
DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD

DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD
DB      SAD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD

DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD
DB      PSD,    SAD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD

DB      PAD,    PSD,    PSD,    PSD,    PSD,    PAD,    PSD,    PSD
DB      PSD,    PSD,    OND,    PSD,    PSD,    PSD,    PSD,    PAD

DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD
DB      PSD,    PSD,    PSD,    SAD,    PSD,    PSD,    PSD,    PSD

DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD
DB      PSD,    PSD,    PSD,    PSD,    SAD,    PSD,    PSD,    PSD

DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD
DB      PSD,    PSD,    PSD,    PSD,    PSD,    SAD,    PSD,    PSD

DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    PSD
DB      PSD,    PSD,    PSD,    PSD,    PSD,    PSD,    SAD,    PSD

DB      PAD,    PSD,    PSD,    PSD,    PSD,    PAD,    PSD,    PSD
DB      PSD,    PSD,    PAD,    PSD,    PSD,    PSD,    PSD,    OND
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CLOSESEG _NVTEXT32

END

