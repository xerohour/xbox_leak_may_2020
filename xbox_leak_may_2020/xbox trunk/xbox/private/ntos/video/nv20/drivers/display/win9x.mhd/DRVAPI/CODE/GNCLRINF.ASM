;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnclrinf.asm
;
; Purpose:      This file holds the ColorInfo display driver entry point.
;
;==============================================================================
.586
incDrawMode = 1
incDevice   = 1
incLogical  = 1
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc

COMPARE_VS_DIBENG       MACRO
        LOCAL   skip1, skip2
IFDEF DEBUG
        movzx   ecx,sp
        push    eax
        push    edx
        push    lpDstDev
        push    dwColorIn
        push    lpPhysColor
        call    DIB_ColorInfo
        mov     ebx,eax
        mov     ecx,edx
        pop     edx
        pop     eax
        cmp     ax,bx
        je      skip1
        int     3
skip1:
        cmp     cx,dx
        je      skip2
        int     3
skip2:
ENDIF
ENDM


OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

PUBLIC  Table5To8
Table5To8       LABEL   BYTE
DB      000H, 010H, 018H, 020H, 028H, 030H, 038H, 040H
DB      048H, 050H, 058H, 060H, 068H, 070H, 078H, 080H
DB      088H, 090H, 098H, 0A0H, 0A8H, 0B0H, 0B8H, 0C0H
DB      0C8H, 0D0H, 0D8H, 0E0H, 0E8H, 0F0H, 0F8H, 0FFH

PUBLIC  Table6To8
Table6To8       LABEL   BYTE
DB      000H, 008H, 00CH, 010H, 014H, 018H, 01CH, 020H
DB      024H, 028H, 02CH, 030H, 034H, 038H, 03CH, 040H
DB      044H, 048H, 04CH, 050H, 054H, 058H, 05CH, 060H
DB      064H, 068H, 06CH, 070H, 074H, 078H, 07CH, 080H
DB      084H, 088H, 08CH, 090H, 094H, 098H, 09CH, 0A0H
DB      0A4H, 0A8H, 0ACH, 0B0H, 0B4H, 0B8H, 0BCH, 0C0H
DB      0C4H, 0C8H, 0CCH, 0D0H, 0D4H, 0D8H, 0DCH, 0E0H
DB      0E4H, 0E8H, 0ECH, 0F0H, 0F4H, 0F8H, 0FCH, 0FFH

;==============================================================================
;
; Function:     GENERIC_ColorInfo
;
; Purpose:      This function is the ColorInfo display driver entry point.
;
;==============================================================================
lpDstDev        TEXTEQU <dword ptr ss:[ecx + 0CH]>
dwColorIn       TEXTEQU <dword ptr ss:[ecx + 08H]>
lpPhysColor     TEXTEQU <dword ptr ss:[ecx + 04H]>

DECPROC GENERIC_ColorInfo, PASCAL, NOFRAME, FAR16
;PARMD   lpDstDev
;PARMD   dwColorIn
;PARMD   lpPhysColor
OPENPROC
        movzx   ecx,sp
        sub     ebx,ebx
        lfs     bx,lpDstDev

        ; Only handle 16bpp and 32bpp color info calls
        mov     al,fs:[ebx].DIBENGINE.deBitsPixel
        movzx   edx,fs:[ebx].DIBENGINE.deFlags
        cmp     al,32
        je      Bpp32
        cmp     al,16
        jne     Punt

Bpp16:
        ; Only deal with 5-6-5
        test    edx,FIVE6FIVE
        jz      Punt

        ; Break into LogToPhys and PhysToLog cases
        mov     eax,dwColorIn
        cmp     word ptr lpPhysColor+2,0
        je      Phys16ToLog

LogToPhys16:
        test    eax,0FF000000H
        jnz     LogPaletteIndexToPhys

LogRGBToPhys16:
        lfs     bx,lpPhysColor

        ; Special case check for black
        and     eax,0FFFFFFH
        jne     @F
        or      eax,40000000H
        mov     fs:[ebx],eax
        mov     byte ptr fs:[ebx + 3],40H
        sub     edx,edx
        COMPARE_VS_DIBENG
        RETFAR16  0CH
@@:
        ; Special case check for white
        cmp     eax,0FFFFFFH
        jne     @F
        or      eax,40000000H
        mov     fs:[ebx],eax
        mov     byte ptr fs:[ebx + 3],40H
        mov     edx,0FFH
        COMPARE_VS_DIBENG
        RETFAR16  0CH
@@:
        ; Not special -- convert 32 to 16 the hard way
        movzx   edx,al
        shr     eax,8
        xchg    ah,dl
        sub     al,02H
        jnc     @F
        sub     al,al
@@:     sub     ah,4
        jnc     @F
        sub     ah,ah
@@:     sub     dl,4
        jnc     @F
        sub     dl,dl
@@:     shr     ah,3
        shl     edx,18H
        shr     ax,2
        shld    eax,edx,5
        movzx   eax,ax
        mov     fs:[ebx],eax

Phys16ToLog:
        ; Convert 16 to 32 via tables
        mov     ebx,eax
        and     eax,1FH
        shr     ebx,5
        mov     dl,cs:Table5To8[eax]
        mov     ecx,ebx
        and     ebx,3FH
        shr     ecx,6
        and     ecx,1FH
        mov     ah,cs:Table6To8[ebx]
        mov     al,cs:Table5To8[ecx]
        sub     dh,dh
        COMPARE_VS_DIBENG
        RETFAR16  0CH

Bpp32:
        movzx   eax,word ptr dwColorIn
        movzx   edx,word ptr dwColorIn+2
        cmp     word ptr lpPhysColor+2,0
        je      Phys32ToLog

LogToPhys32:
        ; Watch for palette indices
        or	dh,dh
        jne     LogPaletteIndexToPhys

        ; Easy -- just switch red and blue
        lfs     bx,lpPhysColor
        mov     byte ptr fs:[ebx+0],dl
        mov     byte ptr fs:[ebx+1],ah
        mov     byte ptr fs:[ebx+2],al
        mov     byte ptr fs:[ebx+3],00
        sub     dh,dh
        COMPARE_VS_DIBENG
        RETFAR16  0CH

Phys32ToLog:
        ; Easy -- just switch red and blue
        xchg    al,dl
        sub     dh,dh
        COMPARE_VS_DIBENG
        RETFAR16  0CH

LogPaletteIndexToPhys:
        or      eax,0FFFF0000H
        lfs     bx,lpPhysColor
        mov     fs:[ebx],eax
        shld    edx,eax,10H
        COMPARE_VS_DIBENG
        RETFAR16  0CH

Punt:
        jmp     DIB_ColorInfo
CLOSEPROC

CLOSESEG _NVTEXT32

END

