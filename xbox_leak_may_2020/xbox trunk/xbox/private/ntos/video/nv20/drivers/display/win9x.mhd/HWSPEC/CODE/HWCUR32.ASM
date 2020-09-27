;==============================================================================
;
; Copyright (C) 1997, Nvidia Coporation
;
; File:         hwcursor.asm
;
; Purpose:      This file implements the hardware specific code to
;               support the cursor.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include hwmacs.inc
include nvreg.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc

XOR_IS_OK       TEXTEQU <1>

IF  (USE_ALPHA_CURSOR NE 0)

OPENSEG  _DATA, USE16, DATA
PUBLIC  szNVDisplayKey
szNVDisplayKey  DB      NV4_REG_GLOBAL_BASE_PATH, '\'
                DB      NV4_REG_DISPLAY_DRIVER_SUBKEY, 0

PUBLIC  szCursorAlpha
szCursorAlpha   DB      NV4_REG_DRV_CURSOR_ALPHA, 0
CLOSESEG _DATA


OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

PUBLIC  dwEnableCursorShadow
dwEnableCursorShadow    DD      0

PUBLIC  dwConvert4bppTo32bpp
dwConvert4bppTo32bpp    LABEL   DWORD
        DD      00000000H, 00800000H, 00008000H, 00808000H
        DD      00000080H, 00800080H, 00008080H, 00C0C0C0H
        DD      00808080H, 00FF0000H, 0000FF00H, 00FFFF00H
        DD      000000FFH, 00FF00FFH, 0000FFFFH, 00FFFFFFH

PUBLIC  bConvert5bppTo8bpp
bConvert5bppTo8bpp      LABEL   BYTE
        DB    0,  10h,  18h,  20h,  28h,  30h,  38h,  40h
        DB  48h,  50h,  58h,  60h,  68h,  70h,  78h,  80h
        DB  88h,  90h,  98h, 0A0h, 0A8h, 0B0h, 0B8h, 0C0h
        DB 0C8h, 0D0h, 0D8h, 0E0h, 0E8h, 0F0h, 0F8h, 0FFh

PUBLIC  bConvert6bppTo8bpp
bConvert6bppTo8bpp      LABEL   BYTE
        DB    0,    8,  0Ch,  10h,  14h,  18h,  1Ch,  20h
        DB  24h,  28h,  2Ch,  30h,  34h,  38h,  3Ch,  40h
        DB  44h,  48h,  4Ch,  50h,  54h,  58h,  5Ch,  60h
        DB  64h,  68h,  6Ch,  70h,  74h,  78h,  7Ch,  80h
        DB  84h,  88h,  8Ch,  90h,  94h,  98h,  9Ch, 0A0h
        DB 0A4h, 0A8h, 0ACh, 0B0h, 0B4h, 0B8h, 0BCh, 0C0h
        DB 0C4h, 0C8h, 0CCh, 0D0h, 0D4h, 0D8h, 0DCh, 0E0h
        DB 0E4h, 0E8h, 0ECh, 0F0h, 0F4h, 0F8h, 0FCh, 0FFh


;==============================================================================
;
; Function:     HW_CursorSet8888
;
; Purpose:      This function places a cursor pattern in offscreen video
;               memory. It does not make this new pattern the active
;               cursor, nor should it change the state of the cursor
;               on/off. Remember that the cursor can be many different
;               color depths -- you need to convert it to the color
;               depth in which you store the cursor in video memory.
;
;               This routine in particular tries to set the cursor
;               using the 8888 format which has an alpha component.
;               The hardware is slightly broken with this cursor
;               on NV10 (which was the first noard to support 8888
;               format) and it is not always possible to do this
;               format. Basically, if the XOR mask is used to invert
;               a pixel, the cursor is not doable on NV10.
;
;               Hence, this routine can fail.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX
;               dwCursorCacheSlot       0 based cache slot
;               pCursorDesc             CURSORDESC ptr
;
; Returns:
;               eax                     0       Failure
;                                       non-0   Success
;               ebx     If eax indicates success, then this is a value
;                       that the hardware needs cached with the cursor
;                       information.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_CursorSet8888, PASCAL, FRAME, NEAR, LOGOFF
PARMD   pHWCrtcCXOffset
PARMD   dwCursorCacheSlot
PARMD   pCursorDesc
LOCALD  dwCursorHWType
OPENPROC
        PUSHR   esi,edi
        mov     ebx,pHWCrtcCXOffset
        mov     edx,ds:[ebx].HWCRTCCX.pHWBoardCX

        ; Get a ptr to the place to store the cursor pattern.
        mov     edi,dwCursorCacheSlot
        imul    edi,HW_CURSOR_SIZE
        add     edi,ds:[edx].HWBOARDCX.pVideoMemory
        add     edi,ds:[ebx].HWCRTCCX.dwCursorCacheVramOffset
        mov     esi,pCursorDesc
        mov     dwCursorHWType,NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR_PM_LE_A8R8G8B8

        ; Dispatch to the routine to copy/convert the cursor
        ; pattern to the hardware format and store it in vidmem.
        push    OFFSET Ret8888
        mov     al,ds:[esi].CURSORDESC.bBitsPerPixel
        cmp     al,01H
        je      SetCursor_1Bpp_8888
        cmp     al,10H
        je      SetCursor_16Bpp_8888
        cmp     al,20H
        je      SetCursor_32Bpp_8888
        cmp     al,04H
        je      SetCursor_4Bpp_8888
        cmp     al,18H
        je      SetCursor_24Bpp_8888
        cmp     al,0FH
        je      SetCursor_15Bpp_8888
        cmp     al,08H
        je      SetCursor_8Bpp_8888

        ; 21H bits/pixel is a DEBUG only thing. It allows for playing
        ; around with the alpha. The cursor depth is really 32bpp.
        ; We just say it is 33bpp as a flag.
        cmp     al,21H
        je      SetCursor_3XBpp_8888

        ; 22H bits/pixel is a DEBUG only thing. It allows for playing
        ; around with premultipliedalpha. The cursor depth is really
        ; 32bpp. We just say it is 34bpp as a flag.
        mov     dwCursorHWType,NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR_PM_LE_A8R8G8B8
        cmp     al,22H
        je      SetCursor_3XBpp_8888
        pop     eax
        sub     eax,eax
        jmp     SetDone

Ret8888:
        or      eax,eax
        sete    al
        movzx   eax,al
        mov     ebx,dwCursorHWType
SetDone:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_1Bpp_8888
;
; Purpose:      This function places a 1Bpp cursor pattern in
;               offscreen video memory converting it to the
;               hardware format on the fly.
;
; Arguments:
;               ds      Context Selector
;               ebx     HWCRTCCX ptr
;               esi     CURSORDESC ptr
;               edi     ptr to place to store cursor pattern
;
; Returns:
;               eax                     0       Failure
;                                       non-0   Success
;
; Preserve:     ds
;==============================================================================
DECPROC SetCursor_1Bpp_8888, PASCAL, FRAME, NEAR, LOGOFF
LOCALD  dwAlpha
LOCALD  dwAlpha1
LOCALD  dwAndMaskSize
OPENPROC
        mov     eax,ds:[ebx].HWCRTCCX.dwCursorAlpha
        shl     eax,16                         ; E:AX = alpha_shadow:alpha_cursor:0
        mov     [dwAlpha1],eax                 ; Save shadow alpha value
        shl     eax,8                          ; E:AX   = alpha_cursor:0
        mov     [dwAlpha],eax                  ; Save cursor alpha value

        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        shr     ecx,3                          ; CX = bytes in the AND mask
        mov     [dwAndMaskSize],ecx
        mov     ebx,ecx                        ; EBX = AndMaskSize
        shr     ecx,2                          ; CL = dwords of pixel data
        lea     esi,[esi].CURSORDESC.bCursorAndMask

IF      (XOR_IS_OK EQ 0)
        ; Check for any inverted pixels in the cursor.
        ; You can tell this because the AND mask and the XOR mask
        ; are both non-0 at the same pixel.
        ; If there are none, then we can use alpha.
        ; Otherwise, we must use ROP555 format (no alpha).
        mov     edx,esi
Check_XOR_Loop:
        mov     eax,ds:[edx]                   ; EAX = 32 bits of AND mask
        and     eax,ds:[edx + ebx]             ; EAX = 32 bits of pixel data AND mask
        jnz     SetCursorFail                  ; Ooops, there is an XOR, must use ROP555
        add     edx,4
        dec     ecx
        jnz     Check_XOR_Loop
ENDIF

        ; Set the cursor into video memory.
        mov     ecx,ebx                        ; ECX = bytes of pixel data
        shr     ecx,2                          ; CL = dwords of pixel data
Next_Dword:
        mov     ebx,[dwAndMaskSize]
        mov     eax,ds:[esi]                   ; EAX = 32 bits of AND mask
        mov     ebx,ds:[esi + ebx]             ; EBX = 32 bits of pixel data
        add     esi,4                          ; Src ptr++
        mov     ch,32                          ; 32 pixels per dword
        bswap   eax
        bswap   ebx

        ; On any bit which is not transparent (i.e. which is part of
        ; the actual cursor image), add in the alpha value.
Cont_Dword:
        add     ebx,ebx                        ; Carry = SHL pixel data
        sbb     edx,edx                        ; DX == 0000 or FFFF
        and     edx,00FFFFFFh                  ; Set alpha = 0
        add     eax,eax                        ; Carry = SHL AND mask
        jnc     @F
        or      edx,edx
        je      NoAlfa
        sub     edx,edx
@@:     add     edx,[dwAlpha]
NoAlfa: mov     ds:[edi],edx                   ; Save the two pixels
        add     edi,4                          ; Increment the destination
        dec     ch
        jne     Cont_Dword
        dec     cl
        jne     Next_Dword

        xor     eax, eax                        ; Return success
SetCursorFail:                                  ; Return failure (EAX != 0)
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_4Bpp_8888
;
; Purpose:      This function places a 4Bpp cursor pattern in offscreen
;               video memory converting it to the hardware format on the
;               fly.
;
; Arguments:
;               ds      Context Selector
;               ebx     HWCRTCCX ptr
;               esi     CURSORDESC ptr
;               edi     ptr to place to store cursor pattern
;
; Returns:
;               eax                     0       Failure
;                                       non-0   Success
;
; Preserve:     ds
;==============================================================================
DECPROC SetCursor_4Bpp_8888, PASCAL, FRAME, NEAR, LOGOFF
LOCALD  dwAlpha
OPENPROC
        mov     eax,ds:[ebx].HWCRTCCX.dwCursorAlpha
        shl     eax,24                         ; E:AX   = alpha_cursor:0
        mov     [dwAlpha],eax                  ; Save cursor alpha value

        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        lea     ebx,[esi + ecx]
        shr     ecx,2                          ; CL = dwords of pixel data

Next_Row:
        ; 4bpp case
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; DS:ESI next row of AND mask
        mov     ch,(32 / 2)                    ; CH is bytes to handle
        bswap   edx                            ; Roll the bits off the left

Cont_Row:
        movzx   eax,byte ptr ds:[ebx]          ; EAX = 000000:XX two pixels
        shr     eax,4                          ; EAX = 1 nibble of pixel data
        mov     eax,cs:dwConvert4bppTo32bpp[eax * 4]  ; EAX = converted pixel
        add     edx,edx                        ; Carry <= SHL AND mask
        jnc     @F
        or      eax,eax
        je      NoAlf0
        sub     eax,eax
@@:	add     eax,[dwAlpha]
NoAlf0: mov     ds:[edi],eax                   ; Save the pixel

        movzx   eax,byte ptr ds:[ebx]          ; E:A:X = 000000:XX two pixels of cursor data
        inc     ebx                            ; DS:EBX -> cursor pixel data++
        and     eax,0FH                        ; EAX   = 1 nibble of pixel data
        mov     eax,cs:dwConvert4bppTo32bpp[eax * 4]   ; EAX = converted pixel
        add     edx,edx                        ; Carry <= SHL AND mask
        jnc     @F
        or      eax,eax
        je      NoAlf1
        sub     eax,eax
@@:	add     eax,[dwAlpha]
NoAlf1: mov     ds:[edi+4],eax                 ; Save the pixel

        add     edi,8                          ; Increment the destination
        dec     ch                             ; Decrement the loops on this DWORD (EDX)
        jne     Cont_Row
        dec     cl                             ; Decrement the DWORD count
        jne     Next_Row

        xor     eax,eax                        ; Return success
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_8Bpp_8888
;
; Purpose:      This function places a 8Bpp cursor pattern in offscreen
;               video memory converting it to the hardware format on the
;               fly.
;
; Arguments:
;               ds      Context Selector
;               ebx     HWCRTCCX ptr
;               esi     CURSORDESC ptr
;               edi     ptr to place to store cursor pattern
;
; Preserve:     ds
;==============================================================================
DECPROC SetCursor_8Bpp_8888, PASCAL, FRAME, NEAR, LOGOFF
LOCALD  dwAlpha
LOCALB  bColumn
LOCALB  bRow
OPENPROC
        mov     eax,ds:[ebx].HWCRTCCX.dwCursorAlpha
        shl     eax,24                         ; E:AX   = alpha_cursor:0
        mov     [dwAlpha],eax                  ; Save cursor alpha value

        movzx   eax,ds:[esi].CURSORDESC.wHeight
        imul    ax,ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     eax,3                          ; AX = bytes in the AND mask
        lea     ecx,[ebx].HWCRTCCX.dwPaletteTable
        lea     ebx,[esi + eax]                ; DS:EBX -> cursor pixel data
        shr     eax,2                          ; AL = dwords of mask data
        mov     [bRow],al

Next_Dword:
        ; 8bpp case
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; DS:ESI next byte of AND mask
        mov     [bColumn],32                   ; Prepare to handle one dword
        bswap   edx                            ; Roll the bits off the left

Cont_Dword:
        movzx   eax,byte ptr ds:[ebx]          ; E:A:X = 0000:00:XX one pixel
        inc     ebx                            ; Next src pixel
        mov     eax,ds:[ecx][eax * 4]          ; EAX   = XRGB for 8bpp pixel
        and     eax,000FFFFFFh                 ; Clear alpha
        add     edx,edx                        ; Carry <= SHL AND mask
        jnc     @F
        or      eax,eax
        je      NoAlfa
        sub     eax,eax
@@:	add     eax,[dwAlpha]
NoAlfa: mov     ds:[edi],eax                   ; Save the pixel

        add     edi,4                          ; Next dst pixel
        dec     [bColumn]
        jne     Cont_Dword
        dec     [bRow]
        jne     Next_Dword

        xor     eax,eax                        ; Return success
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_15Bpp_8888
;
; Purpose:      This function places a 15Bpp cursor pattern in offscreen
;               video memory converting it to the hardware format on the
;               fly.
;
; Arguments:
;               ds      Context Selector
;               ebx     HWCRTCCX ptr
;               esi     CURSORDESC ptr
;               edi     ptr to place to store cursor pattern
;
; Preserve:     ds
;==============================================================================
DECPROC SetCursor_15Bpp_8888, PASCAL, FRAME, NEAR, LOGOFF
LOCALB  bColumn
LOCALB  bRow
LOCALD  dwAlpha
OPENPROC
        mov     eax,ds:[ebx].HWCRTCCX.dwCursorAlpha
        shl     eax,24                         ; E:AX   = alpha_cursor:0
        mov     [dwAlpha],eax                  ; Save cursor alpha value

        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     eax,[esi + ecx]                ; DS:EBX -> cursor pixel data
        shr     ecx,2                          ; CL = dwords of mask data
        mov     [bRow],cl
        sub     ebx,ebx

Next_Dword:
        ; 15bpp case
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; ESI next byte of AND mask
        mov     [bColumn],32                   ; Prepare to handle one dword
        bswap   edx                            ; Roll the bits off the left

Cont_Dword:
        movzx   ecx,word ptr ds:[eax]          ; CX = next pixel
        add     eax,2                          ; Next src pixel
        mov     bl,cl
        shl     ecx,3                          ; Save Red and Green
        and     bl,01Fh                        ; Get Blue 5bpp
        mov     cl,cs:bConvert5bppTo8bpp[ebx]  ; Convert to byte
        ror     ecx,8                          ; green to low, blue to high
        mov     bl,cl                          ; BL = some red & green
        shl     cx,3                           ; CH = red
        and     bl,01Fh                        ; Get Green 5bpp
        mov     cl,cs:bConvert5bppTo8bpp[ebx]  ; Convert to byte
        ror     ecx,8                          ; red to low, green to high
        mov     bl,cl
        and     bl,01Fh                        ; Get Red 5bpp
        mov     cl,cs:bConvert5bppTo8bpp[ebx]  ; Convert to byte
        ror     ecx,10H                        ; E:CX = R:G:B:0
        add     edx,edx                        ; Carry <= SHL AND mask
        jnc     @F
        or      ecx,ecx
        je      NoAlfa
        sub     ecx,ecx
@@:	add     ecx,[dwAlpha]
NoAlfa: mov     ds:[edi],ecx                   ; Save the pixel

        add     edi,4                          ; Next dst pixel
        dec     [bColumn]
        jne     Cont_Dword
        dec     [bRow]
        jne     Next_Dword

        xor     eax,eax                        ; Return success
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_16Bpp_8888
;
; Purpose:      This function places a 16Bpp cursor pattern in offscreen
;               video memory converting it to the hardware format on the
;               fly.
;
; Arguments:
;               ds      Context Selector
;               ebx     HWCRTCCX ptr
;               esi     CURSORDESC ptr
;               edi     ptr to place to store cursor pattern
;
; Preserve:     ds
;==============================================================================
DECPROC SetCursor_16Bpp_8888, PASCAL, FRAME, NEAR, LOGOFF
LOCALB  bColumn
LOCALB  bRow
LOCALD  dwAlpha
OPENPROC
        mov     eax,ds:[ebx].HWCRTCCX.dwCursorAlpha
        shl     eax,24                         ; E:AX   = alpha_cursor:0
        mov     [dwAlpha],eax                  ; Save cursor alpha value

        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     eax,[esi + ecx]                ; DS:EBX -> cursor pixel data
        shr     ecx,2                          ; CL = dwords of mask data
        mov     [bRow],cl
        sub     ebx,ebx

Next_Dword:
        ; 16bpp case
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; ESI next byte of AND mask
        mov     [bColumn],32                   ; 32 pixels per scanline
        bswap   edx                            ; Roll the bits off the left

Cont_Dword:
        movzx   ecx,word ptr ds:[eax]          ; CX = next pixel
        add     eax,2                          ; Next src pixel
        mov     bl,cl
        shl     ecx,3                          ; Save Red and Green
        and     bl,01Fh                        ; Get Blue 5bpp
        mov     cl,cs:bConvert5bppTo8bpp[ebx]  ; Convert to byte
        ror     ecx,8                          ; green to low, blue to high
        mov     bl,cl                          ; BL = some red & green
        shl     cx,2                           ; CH = red
        and     bl,03Fh                        ; Get Green 5bpp
        mov     cl,cs:bConvert6bppTo8bpp[ebx]  ; Convert to byte
        ror     ecx,8                          ; red to low, green to high
        mov     bl,cl
        and     bl,01Fh                        ; Get Red 5bpp
        mov     cl,cs:bConvert5bppTo8bpp[ebx]  ; Convert to byte
        ror     ecx,10H                        ; E:CX = R:G:B:0
        add     edx,edx                        ; Carry <= SHL AND mask
        jnc     @F
        or      ecx,ecx
        je      NoAlfa
        sub     ecx,ecx
@@:	add     ecx,[dwAlpha]
NoAlfa: mov     ds:[edi],ecx                   ; Save the pixel

        add     edi,4                          ; Next dst pixel
        dec     [bColumn]
        jne     Cont_Dword
        dec     [bRow]
        jne     Next_Dword

        xor     eax,eax                        ; Return success
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_24Bpp_8888
;
; Purpose:      This function places a 24Bpp cursor pattern in offscreen
;               video memory converting it to the hardware format on the
;               fly.
;
; Arguments:
;               ds      Context Selector
;               ebx     HWCRTCCX ptr
;               esi     CURSORDESC ptr
;               edi     ptr to place to store cursor pattern
;
; Preserve:     ds
;==============================================================================
DECPROC SetCursor_24Bpp_8888, PASCAL, FRAME, NEAR, LOGOFF
LOCALD  dwAlpha
OPENPROC
        mov     eax,ds:[ebx].HWCRTCCX.dwCursorAlpha
        shl     eax,24                         ; E:AX   = alpha_cursor:0
        mov     [dwAlpha],eax                  ; Save cursor alpha value

        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     ebx,[esi + ecx]                ; DS:EBX -> cursor pixel data
        shr     ecx,2                          ; CL = dwords of mask data

Next_Dword:
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; ESI next byte of AND mask
        mov     ch,32                          ; 32 pixels per scan
        bswap   edx                            ; Roll the bits off the left

Cont_Dword:
        xor     eax,eax                        ; EAX = 0:0:0:0
        mov     al,ds:[ebx + 2]                ; EAX = 0:0:0:R
        shl     eax,16                         ; EAX = 0:R:0:0
        mov     ax,ds:[ebx]                    ; EAX = 0:R:G:B
        add     ebx,3                          ; Next src pixel
        add     edx,edx                        ; Carry <= SHL AND mask
        jnc     @F
        or      eax,eax
        je      NoAlfa
        sub     eax,eax
@@:	add     eax,[dwAlpha]
NoAlfa: mov     ds:[edi],eax                   ; Save the pixel

        add     edi,4                          ; Next dst pixel
        dec     ch
        jne     Cont_Dword
        dec     cl
        jne     Next_Dword

        xor     eax,eax                        ; Return success
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_32Bpp_8888
;
; Purpose:      This function places a 32Bpp cursor pattern in offscreen
;               video memory converting it to the hardware format on the
;               fly.
;
; Arguments:
;               ds      Context Selector
;               ebx     HWCRTCCX ptr
;               esi     CURSORDESC ptr
;               edi     ptr to place to store cursor pattern
;
; Preserve:     ds
;==============================================================================
DECPROC SetCursor_32Bpp_8888, PASCAL, FRAME, NEAR, LOGOFF
LOCALD  dwAlpha
OPENPROC
        mov     eax,ds:[ebx].HWCRTCCX.dwCursorAlpha
        shl     eax,24                         ; E:AX   = alpha_cursor:0
        mov     [dwAlpha],eax                  ; Save cursor alpha value

        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     ebx,[esi + ecx]                ; DS:EBX -> cursor pixel data
        shr     ecx,2                          ; CL = dwords of mask data

Next_Dword:
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; ESI next byte of AND mask
        mov     ch,32                          ; 32 pixels per scan
        bswap   edx                            ; Roll the bits off the left

Cont_Dword:
        mov     eax,ds:[ebx]                   ; EAX = X:R:G:B
        add     ebx,4                          ; Next src pixel
        and     eax,00FFFFFFh                  ; EAX = 0:R:G:B
        add     edx,edx                        ; Carry <= SHL AND mask
        jnc     @F
        or      eax,eax
        je      NoAlfa
        sub     eax,eax
@@:	add     eax,[dwAlpha]
NoAlfa: mov     ds:[edi],eax                   ; Save the pixel

        add     edi,4                          ; Next dst pixel
        dec     ch
        jne     Cont_Dword
        dec     cl
        jne     Next_Dword

        xor     eax,eax                        ; Return success
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_3XBpp_8888
;
; Purpose:      This function places a 32Bpp ARGB cursor pattern in
;               offscreen video memory converting it to the hardware
;               format on the fly.  This is a nonstandard format that
;               is for testing only. The AND mask is ignored.
;
; Arguments:
;               ds      Context Selector
;               ebx     HWCRTCCX ptr
;               esi     CURSORDESC ptr
;               edi     ptr to place to store cursor pattern
;
; Preserve:     ds
;==============================================================================
DECPROC SetCursor_3XBpp_8888, PASCAL, FRAME, NEAR, LOGOFF
OPENPROC
        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     ebx,[esi + ecx]                ; DS:EBX -> cursor pixel data
        shl     ecx,3                          ; ECX = bytes of mask data

Next_Dword:
        mov     eax,ds:[ebx]                   ; EAX = X:R:G:B
        add     ebx,4                          ; Next src pixel
        mov     ds:[edi],eax                   ; Store the new pixel
        add     edi,4                          ; Next dst pixel
        dec     ecx
        jne     Next_Dword
        xor     eax,eax                        ; Return success
CLOSEPROC

CLOSESEG _NVTEXT32

ENDIF

END

