;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation
;
; File:         gnpixel.asm
;
; Purpose:      This file holds the Pixel display driver entry point.
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

;==============================================================================
;
; Function:     GENERIC_Pixel
;
; Purpose:      This function is the Pixel display driver entry point.
;
; Arguments:    Pixel Display driver stack FRAME
;
; Returns:      If lpDrawMode is NULL then
;                       pixel value at coords wDstX,wDstY if succeesful
;                       80000000H is failure
;               If lpDrawMode is NOT NULL then
;                       00000001H is succeesful
;                       80000000H is failure
;
;==============================================================================
DECPROC GENERIC_Pixel, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wDstX
PARMW   wDstY
PARMD   dwPhysColor
PARMD   lpDrawMode
LOCALV  sPixel,DIB_Brush8
OPENPROC
        PUSHR   ds,esi,edi
        sub     esi,esi
        lgs     si,lpDstDev

        ; Punt old style bitmaps, Selected Dibs, and Mono Bitmaps
        movzx   eax,gs:[esi].DIBENGINE.deType
        movzx   ecx,gs:[esi].DIBENGINE.deFlags
        movzx   edx,gs:[esi].DIBENGINE.deBitsPixel
        mov     ebx,lpDrawMode
        or      eax,eax
        je      GN_PixelPunt
        test    ecx,SELECTEDDIB
        jnz     GN_PixelPunt
        cmp     edx,1
        je      GN_PixelPunt

        ; Switch on Get or Set
        or      ebx,ebx
        je      GN_GetPixel
        jmp     GN_SetPixel

GENERIC_Pixel   ENDP


;==============================================================================
;
; Functions:    Pixel ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of Pixel. You can either return a success code, a failure
;               code, punt to the DIBENG, etc.
;
; Arguments:    Pixel Display Driver Stack Frame
;
;==============================================================================
PUBLIC  GN_PixelPunt
GN_PixelPunt       PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
PLABEL	Punt_Pixel
        jmp     DIB_Pixel
GN_PixelPunt       ENDP

PUBLIC  GN_PixelDone
GN_PixelDone      PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 10H
GN_PixelDone      ENDP


;==============================================================================
;
; Function:     GN_SetPixel
;
; Purpose:      This function jumped to by the Pixel display driver entry
;               point when it is known the operation is to set a pixel.
;
; Arguments:    Display driver Pixel stack FRAME
;               gs:esi  lpDstDev
;               ecx     lpDstDev->deFlags
;
; Returns:      00000001H is succeesful
;               80000000H is failure
;
;==============================================================================
PUBLIC  GN_SetPixel
GN_SetPixel     PROC    NEAR

        ; We won't do a set into a non-VRAM bitmap since we don't want to
        ; have to handle all the different rops
        test    ch,VRAM / 256
        jz      GN_PixelPunt

        ; Check the screen for busy
        mov     ds,cs:[wFlatDataSel]
        mov     edi,cs:[CSpGNLogicalDevice]
        cmp     ds:[edi].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_PixelPunt

        ; Use the hardware specific rect functions to do this
        sub     edx,edx
        lfs     dx,lpDrawMode
        mov     edi,ds:[edi].GNLOGDEVCX.pHWLogdevCX
        movzx   ecx,fs:[edx].DRAWMODE.Rop2
        mov     eax,dword ptr wDstY
        mov	ebx,10001H
        mov     edx,dwPhysColor
        call    HW_SolidColorRectangle
        jmp     GN_PixelDone

GN_SetPixel     ENDP


;==============================================================================
;
; Function:     GN_GetPixel
;
; Purpose:      This function jumped to by the Pixel display driver entry
;               point when it is known the operation is to get a pixel.
;
; Arguments:    Display driver Pixel stack FRAME
;               gs:esi  lpDstDev
;               ecx     lpDstDev->deFlags
;
; Returns:      00000001H is succeesful
;               80000000H is failure
;
;==============================================================================
PUBLIC  GN_GetPixel
GN_GetPixel     PROC    NEAR
        ; If the pixel is in video memory, we must wait for idle
        test    ch,VRAM / 256
        jz      @F

        ; Check the screen for busy
        mov     ds,cs:[wFlatDataSel]
        mov     ebx,cs:[CSpGNLogicalDevice]
        cmp     ds:[ebx].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_PixelPunt
        push    cs:[CSlp16LogDevice]
        push    ebx
        push    ebx
        pushw   FB_ACCESS
        call    FFP16 PTR gs:[esi].DIBENGINE.deBeginAccess
@@:
        ; Compute the address of the pixel to get
        movzx   edx,wDstX
        movzx   ecx,gs:[esi].DIBENGINE.deBitsPixel
        movzx   eax,wDstY
        imul    edx,ecx
        imul    eax,gs:[esi].DIBENGINE.deDeltaScan
        shr     edx,3
        les     edi,fword ptr gs:[esi].DIBENGINE.deBits
        add     edx,eax
        add     edi,edx

        ; Fetch the pixel based upon the size
        mov     edx,0FF00H
        movzx   eax,byte ptr es:[edi]
        cmp     ecx,16
        jc      @F
        movzx   eax,word ptr es:[edi]
        jbe     @F
        mov     dx,word ptr es:[edi+2]
@@:
        ; If the pixel was in vram, call the end access function
        test    gs:[esi].DIBENGINE.deFlags,VRAM OR OFFSCREEN
        jz      GN_PixelDone
        push    cs:[CSlp16LogDevice]
        pushw   FB_ACCESS
        call    FFP16 PTR gs:[esi].DIBENGINE.deEndAccess
        jmp     GN_PixelDone

GN_GetPixel     ENDP

CLOSESEG _NVTEXT32

END

