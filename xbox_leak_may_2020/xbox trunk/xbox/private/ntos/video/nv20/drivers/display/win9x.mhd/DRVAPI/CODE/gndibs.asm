;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gndibs.asm
;
; Purpose:      This file implements some of the optimization routines
;               for specific portions of benchmarks or apps.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include	gnboard.inc
include gncrtc.inc
include gngdi.inc
include hwgdi.inc

IF 0
	; FOR SetDIBItsToDevice, these are the widths that have
	; alot of repeated pixels
        cmp     ecx,2EAH
        je      HW_SetDIBitsSingleScan_Sizeable
        cmp     ecx,1E2H
        je      HW_SetDIBitsSingleScan_Sizeable
        cmp     ecx,1C0H
        je      HW_SetDIBitsSingleScan_Sizeable
        cmp     ecx,208H
        je      HW_SetDIBitsSingleScan_Sizeable
        jmp     Not_Sizeable
ENDIF

IF 0
	; FOR StretchDIBits, these are the widths that have
	; alot of repeated pixels
        cmp     ecx,2E8H
        je      HW_SetDIBitsSingleScan_Sizeable
        cmp     ecx,1E2H
        je      HW_SetDIBitsSingleScan_Sizeable
        cmp     ecx,1C0H
        je      HW_SetDIBitsSingleScan_Sizeable
        jmp     Not_Sizeable
ENDIF


OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE
PUBLIC	dwEnableDibRunLengths
dwEnableDibRunLengths	DD	0

;==============================================================================
;
; Function:     GN_DibSingleScan_Bpp32
;
; Purpose:      This routine is jumped to from GENERIC_SetDIBitsSingleScan
;               when the generic code has determined that the call
;		matches this special case: the DIB is 8bpp, 1 scanline
;		high, wInitScan = 0, and wNumScans = 1 and dst is 32bpp.
;
; Arguments:
; 		es:ebx  ptr to first DIB src pixel    (SRC)
; 		gs:edx  framebuffer_selecotr:y * deltaScan + deBitsOffset
; 		fs:edi  ptr to color table
; 		ecx     ExtX
; 		eax     DstX
;
; Returns:      eax     always return 1
;
; Preserve:     ds,edi,gs
;==============================================================================
DECPROC GN_DibSingleScan_Bpp32, PASCAL, NOFRAME, NEAR
OPENPROC
	cmp	cs:[dwEnableDibRunLengths],0
	jne	RunLengths

        lea     esi,[edx][eax * 4]
@@:
        movzx   eax,byte ptr es:[ebx]
        inc     ebx
        mov     edx,fs:[edi][eax * 4]
        mov     gs:[esi],edx
        add     esi,4
        dec     ecx
        jne     @B
        mov     eax,1
	ret

RunLengths:
        lea     esi,[edx][eax * 4]

NextRun32:
        ; Load up the first src pixel
        movzx   eax,byte ptr es:[ebx]
        mov     edx,fs:[edi + eax * 4]
        imul    eax,1010101H
        cmp     ecx,4
        jl      Tl2

        ; Do the run cppropriately
@@:     cmp     eax,es:[ebx]
        jne     EndDwordRun32
        mov     gs:[esi + 00H],edx
        mov     gs:[esi + 04H],edx
        mov     gs:[esi + 08H],edx
        mov     gs:[esi + 0CH],edx
        add     esi,10H
        add     ebx,4
        sub     ecx,4
        cmp     ecx,4
        jge     @B
Tl2:
        or      ecx,ecx
        je      EndLine

EndDwordRun32:
@@:
        cmp     al,es:[ebx]
        jne     EndByteRun32
        mov     gs:[esi],edx
        add     esi,4
        inc     ebx
        dec     ecx
        jg      @B

EndByteRun32:
        or      ecx,ecx
        jne     NextRun32

EndLine:
	mov	eax,1
	ret
ENDPROC


;==============================================================================
;
; Function:     GN_DibSingleScan_Bpp16
;
; Purpose:      This routine is jumped to from GENERIC_SetDIBitsSingleScan
;               when the generic code has determined that the call
;		matches this special case: the DIB is 8bpp, 1 scanline
;		high, wInitScan = 0, and wNumScans = 1 and dst is 16bpp.
;
; Arguments:
; 		es:ebx  ptr to first DIB src pixel    (SRC)
; 		gs:edx  framebuffer_selecotr:y * deltaScan + deBitsOffset
; 		fs:edi  ptr to color table
; 		ecx     ExtX
; 		eax     DstX
; 		edx     y * deltascan + deBitsOffset
;
; Returns:      eax     always return 1
;
; Preserve:     ds,edi,gs
;==============================================================================
DECPROC GN_DibSingleScan_Bpp16, PASCAL, NOFRAME, NEAR
OPENPROC
	cmp	cs:[dwEnableDibRunLengths],0
	jne	RunLengths

        push    ebp
        lea     esi,[edx][eax * 2]
back:
        movzx   edx,byte ptr es:[ebx]
        inc     ebx
        movzx   eax,byte ptr fs:[edi + edx * 4 + 0]
        movzx   ebp,byte ptr fs:[edi + edx * 4 + 1]
        movzx   edx,byte ptr fs:[edi + edx * 4 + 2]

        sub     eax,4
        jnc     @F
        sub     eax,eax
@@:     sub     ebp,2
        jnc     @F
        sub     ebp,ebp
@@:     sub     edx,4
        jnc     @F
        sub     edx,edx
@@:     shr     ebp,2
        shr     edx,3
        shr     eax,3
        shl     ebp,5
        shl     edx,0BH
        or      eax,ebp
        or      eax,edx
        mov     gs:[esi],ax
        add     esi,2
        dec     ecx
        je      done1
        movzx   edx,byte ptr es:[ebx - 1]
@@:
        cmp     dl,es:[ebx]
        jne     back
        inc     ebx
        mov     gs:[esi],ax
        add     esi,2
        dec     ecx
        jne     @B
done1:
        pop     ebp
        mov     eax,1
        ret


RunLengths:
        lea     esi,[edx][eax * 2]

NextRun16:
        ; Load up the first src pixel
        movzx   edx,byte ptr es:[ebx]

        ; Remap the color into edx
        push    ebx
        movzx   eax,byte ptr fs:[edi + edx * 4 + 0]
        movzx   ebx,byte ptr fs:[edi + edx * 4 + 1]
        movzx   edx,byte ptr fs:[edi + edx * 4 + 2]
        sub     eax,4
        jnc     @F
        sub     eax,eax
@@:     sub     ebx,2
        jnc     @F
        sub     ebx,ebx
@@:     sub     edx,4
        jnc     @F
        sub     edx,edx
@@:     shr     ebx,2
        shr     edx,3
        shr     eax,3
        shl     ebx,5
        shl     edx,0BH
        or      eax,ebx
        or      eax,edx
        pop     ebx

        ; get the color into both words
        shrd    edx,eax,10H
        mov     dx,ax

        ; Now get the src pixel again into all 4 bytes of eax
        movzx   eax,byte ptr es:[ebx]
        imul    eax,1010101H

        ; Go into the dword loop if there are enough pixels left
        cmp     ecx,4
        jl      Tl1

        ; Do the run cppropriately
@@:     cmp     eax,es:[ebx]
        jne     EndDwordRun16
        mov     gs:[esi + 00H],edx
        mov     gs:[esi + 04H],edx
        add     esi,8
        add     ebx,4
        sub     ecx,4
        cmp     ecx,4
        jge     @B
Tl1:
        or      ecx,ecx
        je      EndLine

EndDwordRun16:
@@:
        cmp     al,es:[ebx]
        jne     EndByteRun16
        mov     gs:[esi],dx
        add     esi,2
        inc     ebx
        dec     ecx
        jg      @B

EndByteRun16:
        or      ecx,ecx
        jne     NextRun16

EndLine:
        mov     eax,1
        ret
ENDPROC


;==============================================================================
;
; Function:     GN_DibSingleScan_Bpp8
;
; Purpose:      This routine is jumped to from GENERIC_SetDIBitsSingleScan
;               when the generic code has determined that the call
;		matches this special case: the DIB is 8bpp, 1 scanline
;		high, wInitScan = 0, and wNumScans = 1 and dst is 8bpp.
;
; Arguments:
; 		es:ebx  ptr to first DIB src pixel    (SRC)
; 		gs:edx  framebuffer_selecotr:y * deltaScan + deBitsOffset
; 		fs:edi  ptr to color table
; 		ecx     ExtX
; 		eax     DstX
; 		edx     y * deltascan + deBitsOffset
;
; Returns:      eax     always return 1
;
; Preserve:     ds,edi,gs
;==============================================================================
DECPROC GN_DibSingleScan_Bpp8, PASCAL, NOFRAME, NEAR
OPENPROC
	cmp	cs:[dwEnableDibRunLengths],0
	jne	RunLengths

        lea     esi,[edx][eax]

Next_Pixel:
        movzx   edx,byte ptr es:[ebx]
        inc     ebx
        mov     al,fs:[edi][edx * 2]
        mov     gs:[esi],al
        inc     esi
        dec     ecx
        jne     Next_Pixel
        mov     eax,1
        ret

RunLengths:
        lea     esi,[edx][eax]

NextRun8:
        ; Load up the first src pixel
        movzx   edx,byte ptr es:[ebx]
        movzx   eax,byte ptr fs:[edi + edx * 2]
        imul	edx,1010101H
        imul    eax,1010101H
        cmp     ecx,4
        jl      Tl2

        ; Do the run cppropriately
@@:     cmp     edx,es:[ebx]
        jne     EndDwordRun8
        mov     gs:[esi + 00H],eax
        add     esi,4
        add     ebx,4
        sub     ecx,4
        cmp     ecx,4
        jge     @B
Tl2:
        or      ecx,ecx
        je      EndLine

EndDwordRun8:
@@:
        cmp     dl,es:[ebx]
        jne     EndByteRun8
        mov     gs:[esi],al
        add     esi,4
        inc     ebx
        dec     ecx
        jg      @B

EndByteRun8:
        or      ecx,ecx
        jne     NextRun8

EndLine:
	mov	eax,1
	ret
ENDPROC

CLOSESEG _NVTEXT32

END

