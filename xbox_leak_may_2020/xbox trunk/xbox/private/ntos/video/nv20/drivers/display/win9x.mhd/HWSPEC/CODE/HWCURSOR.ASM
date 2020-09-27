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

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA


OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE
PUBLIC  dwAlphaCursorsEnabled
dwAlphaCursorsEnabled   DD      0

; These are shifted left to shift the alpha bit into the MSB
PUBLIC  wConvert4bppTo15bpp
wConvert4bppTo15bpp     LABEL   WORD
        DW      0000H SHL 1,    4000H SHL 1,    0200H SHL 1,    4200H SHL 1
        DW      0010H SHL 1,    4010H SHL 1,    0210H SHL 1,    6318H SHL 1
        DW      4210H SHL 1,    7C00H SHL 1,    03E0H SHL 1,    7FE0H SHL 1
        DW      001FH SHL 1,    7C1FH SHL 1,    03FFH SHL 1,    7FFFH SHL 1


;==============================================================================
;
; Function:     HW_CursorEnable
;
; Purpose:      This function will turn the cursor on or off as specified
;               by the enable argument.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX
;               dwEnable                0 then turn cursor off
;                                       non-0 then turn cursor on
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_CursorEnable_FAR32, PASCAL, FRAME, FAR32, LOGOFF
PARMD   pHWCrtcCXOffset
PARMD   dwEnable
OPENPROC
        push    pHWCrtcCXOffset
        push    dwEnable
        call    HW_CursorEnable
CLOSEPROC

DECPROC HW_CursorEnable, PASCAL, FRAME, NEAR, LOGOFF
PARMD   pHWCrtcCXOffset
PARMD   dwEnable
OPENPROC
        mov     ebx,pHWCrtcCXOffset
        or      ebx,ebx
        je      DoneIt
        mov     eax,ds:[ebx].HWCRTCCX.pHWBoardCX
        cmp     dwEnable,1
        sbb     ecx,ecx
        and     ecx,NV_CURSOR_DISABLE - NV_CURSOR_ENABLE
        add     ecx,NV_CURSOR_ENABLE
        or      ecx,NV_CURSOR_SET_ENABLE
        push    ecx
        push    ecx
        push    ecx
        push    ecx
        pushd   0
        push    ds:[eax].HWBOARDCX.dwDeviceNum
        push   ds:[ebx].HWCRTCCX.dwPhysicalCRTCInstance
        call    FFP16 PTR lpfnNvSysSetCursorImage
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:     HW_CursorSet
;
; Purpose:      This function places a cursor pattern in offscreen video
;               memory. It does not make this new pattern the active
;               cursor, nor should it change the state of the cursor
;               on/off. Remember that the cursor can be many different
;               color depths -- you need to convert it to the color
;               depth in which you store the cursor in video memory.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX
;               dwCursorCacheSlot       0 based cache slot
;               pCursorDesc             CURSORDESC ptr
;
; Returns:
;               eax                     Any piece of data the hardware
;                                       wants passed back in on other
;                                       calls ....
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_CursorSet, PASCAL, FRAME, NEAR, LOGOFF
PARMD   pHWCrtcCXOffset
PARMD   dwCursorCacheSlot
PARMD   pCursorDesc
LOCALD  dwCursorHWType
OPENPROC
        PUSHR   esi,edi
        mov     ebx,pHWCrtcCXOffset
        or      ebx,ebx
        je      DoneIt

        mov     ecx,ds:[ebx].HWCRTCCX.pHWBoardCX

IF  (USE_ALPHA_CURSOR NE 0)
        ; If this is at least an NV10, then we can try to use alpha
        cmp     ds:[ecx].HWBOARDCX.dwDeviceVersion,NV_DEVICE_VERSION_10
        jl      Is1555Cursor

        ; Revision BX MUST use the 32bit cursor beause of the DFP
        ; downscale dither using the 16bpp cursor cycles
        mov     eax,ds:[ecx].HWBOARDCX.dwDeviceRevision
        and     eax,0F0H
        cmp     eax,0B0H
        jne     Is1555Cursor

        ; Try to set an 8888 cursor.
        push    pHWCrtcCXOffset
        push    dwCursorCacheSlot
        push    pCursorDesc
        call    HW_CursorSet8888
        mov     dwCursorHWType,ebx
        
        or      eax,eax
        jne     SetRet

Is1555Cursor:
        mov     ebx,pHWCrtcCXOffset
        mov     ecx,ds:[ebx].HWCRTCCX.pHWBoardCX
ENDIF

        ; Get a ptr to the place to store the cursor pattern, and then
        ; call the correct routine to convert the incoming cursor
        ; pattern to the hardware format and store it in vram.
        mov     edi,dwCursorCacheSlot
        imul    edi,HW_CURSOR_SIZE
        add     edi,ds:[ecx].HWBOARDCX.pVideoMemory
        add     edi,ds:[ebx].HWCRTCCX.dwCursorCacheVramOffset
        mov     esi,pCursorDesc
        mov     dwCursorHWType,NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_ROP1R5G5B5

        push    OFFSET SetRet
        mov     al, ds:[esi].CURSORDESC.bBitsPerPixel
        cmp     al,01H
        je      SetCursor_1Bpp_1555
        cmp     al,10H
        je      SetCursor_16Bpp_1555
        cmp     al,20H
        je      SetCursor_32Bpp_1555
        cmp     al,04H
        je      SetCursor_4Bpp_1555
        cmp     al,18H
        je      SetCursor_24Bpp_1555
        cmp     al,0FH
        je      SetCursor_15Bpp_1555
        cmp     al,08H
        je      SetCursor_8Bpp_1555
        pop     eax

SetRet:
DoneIt:        
                mov     eax,dwCursorHWType
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_CursorSelect
;
; Purpose:      This function will select one of the cached cursors
;               to be the currently active cursor. It should not change
;               the state of the cursor on/off.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX
;               dwCursorCacheSlot       0 based cache slot
;               dwCursorHWType          returned from HW_CursorSet
;               pCursorDesc             CURSORDESC ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_CursorSelect, PASCAL, FRAME, NEAR, LOGOFF
PARMD   pHWCrtcCXOffset
PARMD   dwCursorCacheSlot
PARMD   dwCursorHWType
PARMD   pCursorDesc
OPENPROC
        PUSHR   esi
        mov     ebx,pHWCrtcCXOffset
        or      ebx,ebx
        je      DoneIt
        mov     ecx,ds:[ebx].HWCRTCCX.pHWBoardCX

        mov     eax,dwCursorCacheSlot
        imul    eax,HW_CURSOR_SIZE
        add     eax,ds:[ebx].HWCRTCCX.dwCursorCacheVramOffset
        mov     esi,ds:[ebx].HWCRTCCX.dwPhysicalCRTCInstance
        mov     ebx,pCursorDesc

        ; Call the RM to select the correct cursor
        pushd   NV_CURSOR_SET_PARAM
        push    eax                                     ; Cursor Offset
        movzx   edx,ds:[ebx].CURSORDESC.wWidth
        push    edx
        movzx   edx,ds:[ebx].CURSORDESC.wHeight
        push    edx
        push    dwCursorHWType
        push    ds:[ecx].HWBOARDCX.dwDeviceNum
        push   esi
        call    FFP16 PTR lpfnNvSysSetCursorImage
DoneIt:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_CursorMove
;
; Purpose:      This function should move the cursor to the location
;               specified. Do not change the on/off state of the cursor.
;
; Arguments:
;               ds                      Context Selector
;               pHWCrtcCXOffset         HWCRTCCX
;               dwYCoordHiXCoordLo      Ycoord:XCoord
;               dwYSpotHiXSpotLo        HotSpotY:HotSpotX (0-63:0-63)
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_CursorMove, PASCAL, FRAME, NEAR, LOGOFF
PARMD   pHWCrtcCXOffset
PARMD   dwYCoordHiXCoordLo
PARMD   dwYSpotHiXSpotLo
OPENPROC
        mov     ebx,pHWCrtcCXOffset
        or      ebx,ebx
        je      DoneIt
        mov     ecx,ds:[ebx].HWCRTCCX.pHWBoardCX

        mov     eax,dwYCoordHiXCoordLo
        sub     ax,word ptr dwYSpotHiXSpotLo
        ror     eax,10H
        sub     ax,word ptr dwYSpotHiXSpotLo + 2
        push    eax
        push    ds:[ecx].HWBOARDCX.dwDeviceNum
        push    ds:[ebx].HWCRTCCX.dwPhysicalCRTCInstance
        call    FFP16 PTR lpfnNvSysMoveCursor
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_1Bpp_1555
;
; Purpose:      This function places a 1Bpp cursor pattern in offscreen
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
DECPROC SetCursor_1Bpp_1555, PASCAL, FRAME, NEAR, LOGOFF
LOCALD  dwAndMaskSize
OPENPROC
        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        shr     ecx,3                          ; CX = bytes in the AND mask
        mov     [dwAndMaskSize],ecx
        shr     ecx,2                          ; CL = dwords of pixel data
        lea     esi,[esi].CURSORDESC.bCursorAndMask

Next_Dword:
        mov     ebx,[dwAndMaskSize]
        mov     eax,ds:[esi]                   ; EAX = 32 bits of AND mask
        mov     ebx,ds:[esi + ebx]             ; EBX = 32 bits of pixel data
        add     esi,4                          ; Src ptr++
        not     eax
        mov     ch,16                          ; One dword takes 16 passes
        bswap   eax
        bswap   ebx

Cont_Dword:
        add     ebx,ebx                        ; Carry = SHL pixel data
        sbb     dx,dx                          ; DX == 0000 or FFFF
        add     eax,eax                        ; Carry = SHL AND mask
        rcr     dx,1                           ; MSB of DX == ROP
        ror     edx,10H                        ; Save in the high word of EDX
        add     ebx,ebx                        ; Carry = SHL pixel data
        sbb     dx,dx                          ; DX == 0000 or FFFF
        add     eax,eax                        ; Carry = SHL AND mask
        rcr     dx,1                           ; MSB of DX == ROP
        ror     edx,10H                        ; Rstore the order of the 2 pixels
        mov     ds:[edi],edx                   ; Save the two pixels
        add     edi,4                          ; Increment the destination
        dec     ch
        jne     Cont_Dword
        dec     cl
        jne     Next_Dword
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_4Bpp_1555
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
; Preserve:     ds
;==============================================================================
DECPROC SetCursor_4Bpp_1555, PASCAL, NOFRAME, NEAR, LOGOFF
OPENPROC
        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     ebx,[esi + ecx]                ; EBX = ptr to pixel data
        shr     ecx,2                          ; CL = dwords of mask data

Next_Dword:
        ; 4bpp case
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; increment AND mask pointer
        not     edx
        mov     ch,(32/2)                      ; 16 pairs of 4bpp pixels per 32 bit AND
        bswap   edx

        ; This loop decodes one dword of AND mask,
        ; 32 pixel or 16 bytes of pixel data
Cont_Dword:
        movzx   eax,byte ptr ds:[ebx]
        shr     eax,4                          ; EAX = first 4bpp pixel in byte
        mov     ax,cs:wConvert4bppTo15bpp[eax * 2]
        add     edx,edx                        ; Put AND bit in CARRY
        rcr     ax,1                           ; Put AND bit in MSB of AX
        mov     ds:[edi],ax                    ; Save the 16bpp pixel
        movzx   eax,byte ptr ds:[ebx]
        inc     ebx                            ; Increment pixel pointer
        and     eax,0FH                        ; second 4bpp pixel in byte
        mov     ax,cs:wConvert4bppTo15bpp[eax * 2]
        add     edx,edx                        ; Put AND bit in CARRY
        rcr     ax,1                           ; Put AND bit in MSB of AX
        mov     ds:[edi+2],ax                  ; Save the 16bp pixel
        add     edi,4                          ; Increment the destination
        dec     ch
        jne     Cont_Dword
        dec     cl
        jne     Next_Dword
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_8Bpp_1555
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
DECPROC SetCursor_8Bpp_1555, PASCAL, FRAME, NEAR, LOGOFF
LOCALB  bColumn
LOCALB  bRow
OPENPROC
        movzx   eax,ds:[esi].CURSORDESC.wHeight
        imul    ax,ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     eax,3                          ; AX = bytes in the AND mask
        lea     ebx,[ebx].HWCRTCCX.dwPaletteTable
        lea     ecx,[esi + eax]                ; DS:ECX -> cursor pixel data
        shr     eax,2                          ; AL = dwords of mask data
        mov     [bRow],al

Next_Dword:
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; next dword of AND mask
        not     edx
        mov     [bColumn],32                   ; Prepare to handle one dword
        bswap   edx                            ; Roll the bits off the left

Cont_Dword:
        movzx   eax,byte ptr ds:[ecx]          ; E:A:X = 0000:00:XX one pixel
        inc     ecx                            ; Next src pixel
        mov     eax,ds:[ebx][eax * 4]          ; EAX   = XRGB for 8bpp pixel
        shr     eax,3                          ; Convert to 5 bits of blue
        ror     eax,5                          ; EAX = 5B:X:8R:8G
        shr     ax,3                           ; EAX = 5B:X:8R:5G
        ror     eax,5                          ; EAX = 5G:5B:X:8R
        shr     ax,3                           ; EAX = 5G:5B:X:5R
        rol     eax,10                         ; AX  = X:5R:5G:5B
        add     ax,ax                          ; AX  = 5R:5G:5B:0
        add     edx,edx                        ; Carry <= SHL AND mask
        rcr     ax,1                           ; AX  = A:5R:5G:5B
        mov     ds:[edi],ax                    ; Store the new pixel
        add     edi,2                          ; Next dst pixel
        dec     [bColumn]
        jne     Cont_Dword
        dec     [bRow]
        jne     Next_Dword
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_15Bpp_1555
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
DECPROC SetCursor_15Bpp_1555, PASCAL, NOFRAME, NEAR, LOGOFF
OPENPROC
        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx, ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     ebx,[esi + ecx]                ; DS:EBX -> cursor pixel data
        shr     ecx,2                          ; CL = dwords of mask data

Next_Dword:
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; next dword of AND mask
        not     edx
        mov     ch,(32 / 2)                    ; 2 pixels per iteration
        bswap   edx

Cont_Dword:
        mov     ax,ds:[ebx]                    ; AX = 1 pixel data
        add     ebx,2
        add     ax,ax
        add     edx,edx
        rcr     ax,1
        ror     eax,10H                        ; E:AX = A555:0000
        mov     ax,ds:[ebx]                    ; AX = 1 pixel data
        add     ebx,2
        add     ax,ax
        add     edx,edx
        rcr     ax,1
        ror     eax,10H                        ; E:AX = Pixel2:Pixel1
        mov     ds:[edi],eax                   ; Save both pixels
        add     edi,4                          ; Next dst pixel
        dec     ch
        jne     Cont_Dword
        dec     cl
        jne     Next_Dword
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_16Bpp_1555
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
DECPROC SetCursor_16Bpp_1555, PASCAL, NOFRAME, NEAR, LOGOFF
OPENPROC
        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx, ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     ebx,[esi + ecx]
        shr     ecx,2                          ; CL = dwords of mask data

Next_Dword:
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; next dword of AND mask
        not     edx
        mov     ch,32                          ; Handle 1 pixel per iteration
        bswap   edx

Cont_Dword:
        movzx   eax,word ptr ds:[ebx]          ; Get one pixel
        add     ebx,2                          ; Increment source pointer
        ror     eax,5
        shr     ax,1
        rol     eax,5
        add     ax,ax
        add     edx,edx
        rcr     ax,1
        mov     ds:[edi],ax                    ; Save pixel
        add     edi,2                          ; Next dst pixel
        dec     ch
        jne     Cont_Dword
        dec     cl
        jne     Next_Dword
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_24Bpp_1555
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
DECPROC SetCursor_24Bpp_1555, PASCAL, NOFRAME, NEAR, LOGOFF
OPENPROC
        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     ebx,[esi + ecx]
        shr     ecx,2                          ; CL = dwords of mask data

Next_Dword:
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; next dword of AND mask
        not     edx
        mov     ch,32                          ; 1 pixel per iteration
        bswap   edx

Cont_Dword:
        mov     al,ds:[ebx+2]
        shl     eax,10H
        mov     ax,ds:[ebx]                    ; EAX = 24 bpp pixel
        add     ebx,3                          ; Increment source pointer
        shr     eax,3
        ror     eax,5
        shr     ax,3
        ror     eax,5
        shr     ax,3
        rol     eax,11                         ; AX = 555X pixel
        add     edx,edx
        rcr     ax,1                           ; AX = R555
        mov     ds:[edi],ax                    ; Save pixel
        add     edi,2                          ; Next dst pixel
        dec     ch
        jg      Cont_Dword
        dec     cl
        jg      Next_Dword
CLOSEPROC


;==============================================================================
;
; Function:     SetCursor_32Bpp_1555
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
DECPROC SetCursor_32Bpp_1555, PASCAL, NOFRAME, NEAR, LOGOFF
OPENPROC
        movzx   ecx,ds:[esi].CURSORDESC.wHeight
        imul    cx,ds:[esi].CURSORDESC.wWidth
        lea     esi,[esi].CURSORDESC.bCursorAndMask
        shr     ecx,3                          ; CX = bytes in the AND mask
        lea     ebx,[esi + ecx]
        shr     ecx,2                          ; CL = dwords of mask data

Next_Dword:
        ; 32bpp case
        mov     edx,ds:[esi]                   ; EDX = 32 bits of AND mask
        add     esi,4                          ; next dword of AND mask
        not     edx
        mov     ch,32                          ; Handle 1 pixel per iteration
        bswap   edx

Cont_Dword:
        mov     eax,ds:[ebx]                   ; EAX = 32 bpp pixel
        add     ebx,4                          ; Increment source pointer
        shr     eax,3
        ror     eax,5
        shr     ax,3
        ror     eax,5
        shr     ax,3
        rol     eax,11                         ; AX = 555X
        add     edx,edx
        rcr     ax,1                           ; AX = R555
        mov     ds:[edi],ax                    ; Save pixel
        add     edi,2
        dec     ch
        jg      Cont_Dword
        dec     cl
        jg      Next_Dword
CLOSEPROC

CLOSESEG _NVTEXT32

END

