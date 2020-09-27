;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation.
;
; File:         gnssb.asm
;
; Purpose:      This file holds the SaveScreenBitmap display driver
;               entry point.
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

; These aren't in any API header file so include them here
SAVE_AREA       EQU     0
RESTORE_AREA    EQU     1
DISCARD_AREA    EQU     2

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GN_ClearSaveScreenBitmapCache
;
; Purpose:      This function clears the SaveScreenBitmap cache.
;
; Arguments:
;               ds                      CXSel
;               pGNLogevCXOffset        GNLOGDEVCX ptr
;
; Returns:
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_ClearSaveScreenBitmapCache, PASCAL, FRAME, FAR32
PARMD   pGNLogevCXOffset
OPENPROC
        PUSHR   esi,edi
        mov     esi,pGNLogevCXOffset

	PUSHR	ds
	POPR	es
        lea     edi,ds:[esi].GNLOGDEVCX.SaveScreenBitmapCache
        mov     eax,-1
        mov     ebx,MAX_SSBS

        ; Initialize the template SSB headers
@@:
        mov     esi,pGNLogevCXOffset
        mov     ecx,(SIZE DIBENGINE + 3) / 4
        mov     edx,edi
        rep     movs dword ptr es:[edi], ds:[esi]
        and     es:[edx].DIBENGINE.deFlags,MINIDRIVER OR PALETTIZED OR FIVE6FIVE
        mov     es:[edi],eax
        lea     edi,[edx + SIZE DEVBMP]
        dec     ebx
        jne     @B
Done:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GENERIC_SaveScreenBitmap
;
; Purpose:      This function is the SaveScreenBitmap display driver
;               entry point.
;
; Arguments:    SaveScreenBitmap Display Driver Stack Frame
;
; Returns:      ax      0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_SaveScreenBitmap, PASCAL, FRAME, FAR16
PARMD   lpRect
PARMW   wCommand
OPENPROC
        PUSHR   ds,esi,edi
        mov     ds,cs:[wFlatDataSel]
        mov     ebx,cs:[CSpGNLogicalDevice]
        sub     eax,eax

        ; Make sure the screen is free
        cmp     eax,ds:[ebx].GNLOGDEVCX.dwHardwareBusy
        jne     GN_SaveScreenBitmapFail

        cmp     eax,lpRect
        je      GN_SaveScreenBitmapFail

        ; switch on the command
        cmp     wCommand,RESTORE_AREA
        jc      SSB_SaveArea
        jmp     SSB_RestoreOrDiscardArea

GENERIC_SaveScreenBitmap        ENDP


;==============================================================================
;
; Function:     SaveScreenBitmap ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of SaveScreenBitmap. You can either return a success code, a failure
;               code, punt to the DIBENG, etc.
;
; Arguments:    SaveScreenBitmap Display Driver Stack Frame
;
;==============================================================================
PUBLIC  GN_SaveScreenBitmapFail
GN_SaveScreenBitmapFail PROC    NEAR
        sub     eax,eax
        jmp     GN_SaveScreenBitmapExit
GN_SaveScreenBitmapFail ENDP

PUBLIC  GN_SaveScreenBitmapSuccess
GN_SaveScreenBitmapSuccess      PROC    NEAR
        mov     eax,1
GN_SaveScreenBitmapSuccess      ENDP

PUBLIC  GN_SaveScreenBitmapExit
GN_SaveScreenBitmapExit PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 06H
GN_SaveScreenBitmapExit ENDP



;==============================================================================
;
; Function:     SSB_SaveArea
;
; Purpose:      This function is jumped to when the SaveScreenBitmap
;               function flavor is to save the area specified.
;
;               We can try to allocate a bitmap both in offscreen or
;               in a page locked DMAable section of memory. Try the
;               offscreen area first -- if that doesn't work, go for
;               the DMAable memory area.
;               NOTE: Right now, only doing offscreen VRAM SSBs.
;
; Arguments:    SaveScreenBitmap Display Driver Stack Frame
;               ds              Context Selector
;               ebx             GNLOGDEVCX ptr
;
; Returns:      jump to one of the SSB ending routinee when you are done
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  SSB_SaveArea
SSB_SaveArea    PROC    NEAR

        ; Fail if cannot use any of the SSB area (offscreen, DMAable)
        mov     eax,ds:[ebx].GNLOGDEVCX.pGNBoardCX
        mov     eax,ds:[eax].GNBOARDCX.dwOffscreenFlags
        test    eax,NO_OFFS_BMPS
        jnz     GN_SaveScreenBitmapFail

        ; Get the rectangle to save
        sub     edi,edi
        lgs     di,lpRect
        mov     eax,dword ptr gs:[edi].RECT.left
        mov     edx,dword ptr gs:[edi].RECT.right

        ; Make sure the rectangle to save is valid
        mov     edi,edx
        mov     esi,edx
        sub     di,ax
        jle     GN_SaveScreenBitmapFail
        sub     esi,eax
        jle     GN_SaveScreenBitmapFail

        ; If any of the existing save screen areas match the new
        ; area to save, then consider the old area trashed and
        ; resave it.
        lea     edi,[ebx].GNLOGDEVCX.SaveScreenBitmapCache
        mov     esi,MAX_SSBS
Loopy1:
        cmp     eax,dword ptr ds:[edi].DEVBMP.wLeft
        jne     @F
        cmp     edx,dword ptr ds:[edi].DEVBMP.wRight
        je      DoBlt
@@:     add     edi,SIZE DEVBMP
        dec     esi
        jne     Loopy1

        ; Find a free SSB header to use.
        lea     edi,[ebx].GNLOGDEVCX.SaveScreenBitmapCache
        mov     ecx,-1
        mov     esi,MAX_SSBS
Loopy2:
        cmp     ecx,dword ptr ds:[edi].DEVBMP.wLeft
        je      @F
        add     edi,SIZE DEVBMP
        dec     esi
        jne     Loopy2
        jmp     GN_SaveScreenBitmapFail
@@:
        ; Complete one of the SSB headers to descibe the region being saved
        mov     dword ptr ds:[edi].DEVBMP.wLeft,eax
        mov     dword ptr ds:[edi].DEVBMP.wRight,edx
        sub     edx,eax
        mov     dword ptr ds:[edi].DIBENGINE.deWidth,edx
        mov     ax,word ptr ds:[ebx].GNLOGDEVCX.Dibeng.dePlanes
        mov     word ptr ds:[edi].DIBENGINE.dePlanes,ax
        movzx   eax,ah
        movzx	edx,dx
        shr     eax,3
        imul    eax,edx
        mov     ds:[edi].DIBENGINE.deWidthBytes,ax

        ; Can we get an offscreen bitmap?
        PUSHR	ds
        POPR	es
        push    ds:[ebx].GNLOGDEVCX.pGNBoardCX
        call    GN_OffscreenBitmapCreate
        or      eax,eax
        jne     DoBlt

        ; If doing DMAable -- try to allocate one in here

        ; If the offscreen allocation failed, mark the SSB slot as
        ; free again and then fail the call
        mov     dword ptr ds:[edi].DEVBMP.wLeft,-1
        jmp     GN_SaveScreenBitmapFail

DoBlt:
        mov     ebx,cs:[CSpGNLogicalDevice]
        push    ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        mov     eax,edi
        sub     eax,ebx
        add     eax,cs:[CSlp16LogDevice]
        push    eax                             ; lpDstDev
        push    cs:[CSlp16LogDevice]            ; lpSrcDev
        sub     eax,eax
        push    eax                             ; lpBrush
        push    eax                             ; lpDrawMode
        push    eax                             ; dstX:dstY
        mov     eax,dword ptr ds:[edi].DEVBMP.wLeft
        mov     ebx,dword ptr ds:[edi].DIBENGINE.deWidth
        ror     eax,10H
        ror     ebx,10H
        push    eax                             ; srcX:srcY
        push    ebx                             ; extX:extY
        mov     eax,0CC0000H
        push    eax                             ; Rop3
        call    HW_BitBltSrcVramDstVram
        or      eax,eax
        jg      GN_SaveScreenBitmapSuccess
        sub     eax,eax
        call    GN_SaveScreenBitmap
        jmp     GN_SaveScreenBitmapSuccess

SSB_SaveArea    ENDP


;==============================================================================
;
; Function:     SSB_RestoreOrDiscardArea
;
; Purpose:      This function is jumped to when the SaveScreenBitmap
;               function flavor is to restore or discard a saved area.
;
; Arguments:    SaveScreenBitmap Display Driver Stack Frame
;               ds              Context Selector
;               ebx             GNLOGDEVCX ptr
;
; Returns:      jump to one of the SSB ending routines when you are done
;
; Preserve:     Nothing
;==============================================================================
PUBLIC  SSB_RestoreOrDiscardArea
SSB_RestoreOrDiscardArea        PROC    NEAR

        ; Load the rectangle to restore or discard
        sub     esi,esi
        les     si,lpRect
        mov     eax,dword ptr es:[esi].RECT.left
        mov     edx,dword ptr es:[esi].RECT.right

        ; Search for it in the list of saved areas
        lea     edi,[ebx].GNLOGDEVCX.SaveScreenBitmapCache
        sub     edi,SIZE DEVBMP
        mov     esi,MAX_SSBS
        mov     ecx,-1
@@:
        add     edi,SIZE DEVBMP
        dec     esi
        jl      GN_SaveScreenBitmapFail
        cmp     ecx,dword ptr ds:[edi].DEVBMP.wLeft
        je      @B
        cmp     eax,dword ptr ds:[edi].DEVBMP.wLeft
        jne     @B
        cmp     edx,dword ptr ds:[edi].DEVBMP.wRight
        jne     @B

Match:
        ; Only do the copy back if the command was restore
        cmp     wCommand,RESTORE_AREA
        jne     @F

        ; If we are allowing DMAable, figure out which one it is
        ; The below only supports VRAM SSBs

        mov     ebx,cs:[CSpGNLogicalDevice]
        push    ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        push    cs:[CSlp16LogDevice]            ; lpDstDev
        mov     eax,edi
        sub     eax,ebx
        add     eax,cs:[CSlp16LogDevice]
        sub     ecx,ecx
        push    eax                             ; lpSrcDev
        push    ecx                             ; lpBrush
        push    ecx                             ; lpDrawMode
        mov     eax,dword ptr ds:[edi].DEVBMP.wLeft
        mov     ebx,dword ptr ds:[edi].DIBENGINE.deWidth
        ror     eax,10H
        ror     ebx,10H
        push    eax                             ; dstX:dstY
        push    ecx                             ; srcX:srcY
        push    ebx                             ; extX:extY
        mov     eax,0CC0000H
        push    eax                             ; rop3
        call    HW_BitBltSrcVramDstVram
        or      eax,eax
        jg      @F
        mov     eax,1
        call    GN_SaveScreenBitmap
@@:
        ; Inidicate the SSB cache slot is now free and release the
        ; memory used by the saved area.
        mov     eax,-1
        mov     dword ptr ds:[edi].DEVBMP.wLeft,eax
        mov     ebx,cs:[CSpGNLogicalDevice]
        push    ds:[ebx].GNLOGDEVCX.pGNBoardCX
	pushd	0
        PUSHR	ds
        POPR	es
        call    GN_OffscreenBitmapDelete
        jmp     GN_SaveScreenBitmapSuccess

SSB_RestoreOrDiscardArea        ENDP



;==============================================================================
;
; Function:     GN_SaveScreenBitmap
;
; Purpose:      This function will save or restore the screen area
;               in software. It is used only if the hardware specific
;               routine fails.
;
; Arguments:    ds      Context Selector
;               edi     DEVBMP ptr. This structure is a DIBENGINE struct
;                       with four extra fields DEVBMP.wLeft, DEVBMP.wRight,
;                       DEVBMP.wTop, DEVBMP.wBottom. These fields specify
;                       a rectangular area of the screen that the bitmap
;                       ds:esi is currently holding or is waiting to hold.
;               eax     0 then copy the rectangle from the primary screen
;                         given by the DEVBMP rectangle specifiers to the
;                         bitmap specified by the DIBENGINE portion of the
;                         DEVBMP structure. This is a SAVE operation.
;                       1 ten copy the bitmap specified by the DIBENGINE
;                         portion of the DEVBMP structure to the rectangle
;                         on the main screen given by the DEVBMP rectangle
;                         specifiers. This is a RESTORE operation.
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_SaveScreenBitmap, PASCAL, FRAME, NEAR
LOCALD  dwRestoreOperation
LOCALD  dwWidthInBytes
LOCALD  dwWholeDwords
LOCALD  dwExtraBytes
LOCALD  dwSrcWrapWidth
LOCALD  dwDstWrapWidth
LOCALD  dwHeight
OPENPROC
        PUSHR   ds,esi,edi

        ; Save this for later
        mov     dwRestoreOperation,eax

        ; Compute the Blt width in a few different formats
        movzx   eax,ds:[edi].DEVBMP.wRight
        movzx   ebx,ds:[edi].DEVBMP.wLeft
        movzx   ecx,ds:[edi].DIBENGINE.deBitsPixel
        sub     eax,ebx
        shr     ecx,3
        imul    eax,ecx
        mov     edx,eax
        mov     ecx,eax
        mov     dwWidthInBytes,eax
        and     eax,3
        mov     dwExtraBytes,eax
        shr     edx,2
        mov     dwWholeDwords,edx

        ; Compute the wrap widths
        mov     ebx,cs:[CSpGNLogicalDevice]
        mov     eax,ds:[ebx].GNLOGDEVCX.Dibeng.deDeltaScan
        mov     edx,ds:[edi].DIBENGINE.deDeltaScan
        sub     eax,ecx
        sub     edx,ecx
        mov     dwSrcWrapWidth,eax
        mov     dwDstWrapWidth,edx

        ; Compute the Blt height
        movzx   eax,ds:[edi].DEVBMP.wBottom
        movzx   ecx,ds:[edi].DEVBMP.wTop
        sub     eax,ecx
        mov     dwHeight,eax

        ; Compute the starting address on the primary display
        movzx   eax,ds:[edi].DIBENGINE.deBitsPixel
        movzx   edx,ds:[edi].DEVBMP.wTop
        movzx   ecx,ds:[edi].DEVBMP.wLeft
        shr     eax,3
        imul    edx,ds:[ebx].GNLOGDEVCX.Dibeng.deDeltaScan
        imul    eax,ecx
        add     eax,edx
        les     edi,fword ptr ds:[edi].DIBENGINE.deBits
        lds     esi,fword ptr ds:[ebx].DIBENGINE.deBits
        add     esi,eax

        ; If it is a save, then we are ready to go. If it is a restore
        ; then we need to switch the src and dst ptrs and wrap widths.
        cmp     dwRestoreOperation,0
        je      @F
        xchg    esi,edi
        mov     ax,es
        mov     dx,ds
        mov     ds,ax
        mov     es,dx
        mov     eax,dwSrcWrapWidth
        mov     edx,dwDstWrapWidth
        mov     dwSrcWrapWidth,edx
        mov     dwDstWrapWidth,eax
@@:
        mov     ecx,dwWholeDwords
        rep     movs dword ptr es:[edi], ds:[esi]
        mov     ecx,dwExtraBytes
        rep     movs byte ptr es:[edi], ds:[esi]
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
        dec     dwHeight
        jne     @B

        POPR    ds,esi,edi
CLOSEPROC

CLOSESEG _NVTEXT32

END

