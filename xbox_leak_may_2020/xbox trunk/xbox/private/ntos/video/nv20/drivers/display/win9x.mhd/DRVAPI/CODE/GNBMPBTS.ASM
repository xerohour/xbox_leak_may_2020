;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnbmpbts.asm
;
; Purpose:      This file holds the BitmapBits display driver entry point.
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

DDB_SET                 EQU     1
DDB_GET                 EQU     2
DDB_COPY                EQU     4
DDB_SETWITHFILLER       EQU     8

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GENERIC_BitmapBits
;
; Purpose:      This function is the BitmapBits display driver entry point.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;
; Returns:      The return values from the functions vary.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_BitmapBits, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMD   dwFlags
PARMD   dwCount
PARMD   lpBits

LOCALD  dwBytesToCopyPerWholeScan
LOCALD  dwBytesToCopyPerPartialScan
LOCALD  dwPixelsToCopyPerWholeScan
LOCALD  dwWholeScansToCopy
LOCALD  dwTotalBytesToCopy
LOCALD  dwDstWrapWidth
LOCALD  dwSrcWrapWidth
LOCALD  dwSrcBpp
LOCALD  dwInVram
LOCALD  dwXferHeight
OPENPROC
        PUSHR   ds,esi,edi

        ; Check and block copies to the screen if it is busy
        mov     fs,cs:[wFlatDataSel]
        mov     ebx,cs:[CSpGNLogicalDevice]
        cmp     fs:[ebx].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_BitmapBitsPuntNoWait

        mov     eax,dwFlags
        mov     dwInVram,0
        cmp     eax,1
        je      BitmapBits_Set
        cmp     eax,4
        jc      BitmapBits_Get
        je      BitmapBits_Copy
        jmp     BitmapBits_SetWithFiller

GENERIC_BitmapBits      ENDP

;==============================================================================
;
; Function:     BitmapBits ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of BitmapBits. You can either return a success code,
;               a failure code, punt to the DIBENG, etc.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;
;==============================================================================
PUBLIC  GN_BitmapBitsPunt
GN_BitmapBitsPunt       PROC    NEAR
        sub     esi,esi
        lds     si,lpDstDev
        movzx   eax,ds:[esi].DIBENGINE.deFlags
        test    eax,VRAM
        jz      GN_BitmapBitsPuntNoWait

        ; Syncronize with the engine
        push    cs:[CSlp16LogDevice]
        push    eax
        push    eax
        pushw   FB_ACCESS
        call    GENERIC_BeginAccess

PLABEL  GN_BitmapBitsPuntNoWait
        POPR    ds,esi,edi
        STOPPROC
PLABEL  Punt_BitmapBits
        jmp     DIB_BitmapBits
GN_BitmapBitsPunt       ENDP

PUBLIC  GN_BitmapBitsSuccess
GN_BitmapBitsSuccess    PROC    NEAR
        mov     eax,dwTotalBytesToCopy
        shld    edx,eax,10H
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 10H
GN_BitmapBitsSuccess    ENDP


;==============================================================================
;
; Function:     BitmapBits_Set
;
; Purpose:      This function is jumped to when the BitmapBits routine
;               is called with dwFlags equal to DDB_SET. In this flavor
;               of the routine, the idea is to copy dwCount bytes from
;               the raw flat linear data buffer pointed to by lpBits
;               to the dst bitmap whose header is pointed to by lpDstDev.
;
;               If lpDstDev points to a VRAM bitmap, then use the hardware
;               to do the transfer.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;
;==============================================================================
PUBLIC  BitmapBits_Set
BitmapBits_Set  PROC    NEAR

        ; Load up a ptr to the dst bitmap header
        sub     esi,esi
        sub     edi,edi
        lds     si,lpDstDev

        ; Compute the smaller of bytes requested and size of bitmap.
        movzx   eax,ds:[esi].DIBENGINE.deHeight
        movzx   ebx,ds:[esi].DIBENGINE.deWidthBytes
        imul    eax,ebx
        cmp     eax,dwCount
        jc      @F
        mov     eax,dwCount
@@:     mov     dwTotalBytesToCopy,eax

        ; Punt dst that we can't handle (this almost never happens)
        movzx   eax,ds:[esi].DIBENGINE.deFlags
        test    eax,SELECTEDDIB
        jnz     GN_BitmapBitsPunt
        test    eax,VRAM
        jz      NotVrm
        mov     dwInVram,1

        ; We may be able to use the hardware, but only if the
        ; number of bytes we are asked to copy is an integral
        ; number of scanlines. If there is a partial scanline,
        ; then we could do it, but since we only process
        ; rectangles with the hardware, we would have to do it
        ; in two operations. The 2 rect case almost never
        ; happens, so we use software for the case.
        mov     eax,dwTotalBytesToCopy
        movzx   ecx,ds:[esi].DIBENGINE.deWidthBytes
        sub     edx,edx
        idiv    ecx
        mov     dwXferHeight,eax
        or      edx,edx
        jne     NotVrm

        ; USE THE HARDWARE HERE
        ; Let the vendor handle the whole thing
        sub     ebx,ebx
        push    cs:[CSpGNLogicalDevice]
        movzx   eax,word ptr ds:[esi].DIBENGINE.deBits+4
        movzx   ecx,ds:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    dword ptr ds:[esi].DIBENGINE.deBits
        push    ecx
        push    ds:[esi].DIBENGINE.deDeltaScan
        push    ebx                                             ;wDstX
        push    ebx                                             ;wDstY
        movzx   eax,word ptr lpBits+2
        movzx   ecx,word ptr lpBits+0
        push    eax
        push    ecx
        movzx   eax,ds:[esi].DIBENGINE.deBitsPixel
        movzx   ecx,ds:[esi].DIBENGINE.deWidthBytes
        push    eax
        push    ecx
        push    ebx                                             ;wSrcX
        push    ebx                                             ;wSrcY
        movzx   eax,word ptr ds:[esi].DIBENGINE.deWidth
        push    eax
        push    dwXferHeight
        mov     ecx,0CC0000H
        push    ecx
        push    ebx                                             ;lpBrush
        push    ebx                                             ;lpDrawMode
        push    ebx                                             ;size color table entry in bytes
        push    ebx                                             ;num color table entries
        push    ebx                                             ;lpColorTable
        push    ebx
        mov     ds,cs:[wFlatDataSel]
        call    GN_BitBltSrcColorDstVram
        or      eax,eax
        jg      GN_BitmapBitsSuccess
        je      NotVrm
        mov     dwTotalBytesToCopy,0
        jmp     GN_BitmapBitsSuccess                            ;Failure

NotVrm:
        ; Load up the ptrs
        movzx   ebx,ds:[esi].DIBENGINE.deWidthBytes
        mov     ecx,ds:[esi].DIBENGINE.deDeltaScan
        les     edi,ds:[esi].DIBENGINE.deBits
        lds     si,lpBits
        mov     dwBytesToCopyPerWholeScan,ebx
        sub     ecx,ebx
        je      BitmapBits_CopyAll

        ; The pitch and width of the bitmap are not the same so ...
        sub     eax,eax
        mov     dwDstWrapWidth,ecx
        mov     dwSrcWrapWidth,eax
        jmp     BitmapBits_CopyByScan

BitmapBits_Set  ENDP


;==============================================================================
;
; Function:     BitmapBits_SetWithFiller
;
; Purpose:      This function is jumped to when the BitmapBits routine
;               is called with dwFlags equal to DDB_SETWITHFILLER. In
;               this flavor of the routine, you copy dwCount bytes from
;               the src bitmap whose header is pointed to by lpBits to
;               the dst bitmap whose header is pointed to by lpDstDev.
;
;               This is like a BitmapBits_Set except that the raw data
;               buffer pointed to by lpBits is not flat. There are fill
;               bytes to prevent a break in the src data for a scanline.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;
;==============================================================================
PUBLIC  BitmapBits_SetWithFiller
BitmapBits_SetWithFiller        PROC    NEAR

        ; Load up a ptr to the src bitmap header
        sub     esi,esi
        sub     edi,edi
        lds     si,lpDstDev

        movzx   eax,ds:[esi].DIBENGINE.deFlags
        test    al,SELECTEDDIB
        jnz     GN_BitmapBitsPunt

        ; If the width in bytes of the dst is a power of 2 less than
        ; 64K, then we can treat the src as flat since there will be
        ; zero fillbytes.
        movzx   ebx,ds:[esi].DIBENGINE.deWidthBytes
        mov     eax,10000H
        sub     edx,edx
        div     ebx
        or      edx,edx
        je      BitmapBits_Set

        ; Also, we can treat the src as flat if we don't need to transfer
        ; more than 64K.
        cmp     ax,ds:[esi].DIBENGINE.deHeight
        ja      BitmapBits_Set

        ; Else, we are going to have to do it -- I vote we punt!
        ; This case almost never happens.
        jmp     GN_BitmapBitsPunt

BitmapBits_SetWithFiller        ENDP


;==============================================================================
;
; Function:     BitmapBits_Get
;
; Purpose:      This function is jumped to when the BitmapBits routine
;               is called with dwFlags equal to DDB_GET. In this flavor
;               of the routine, the idea is to copy dwCount bytes from
;               the src bitmap whose header is pointed to by lpDstDev to
;               the raw flat linear data buffer pointed to by lpBits.
;
;               If lpDstDev points to a VRAM bitmap, then use the hardware
;               DMA to do the transfer.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;
;==============================================================================
PUBLIC  BitmapBits_Get
BitmapBits_Get  PROC    NEAR

        ; Load up a ptr to the src bitmap header
        sub     esi,esi
        sub     edi,edi
        lds     si,lpDstDev

        ; Copy the smaller of bytes requested and size of bitmap.
        movzx   eax,ds:[esi].DIBENGINE.deHeight
        movzx   ebx,ds:[esi].DIBENGINE.deWidthBytes
        imul    eax,ebx
        cmp     eax,dwCount
        jc      @F
        mov     eax,dwCount
@@:     mov     dwTotalBytesToCopy,eax

        ; Punt dsts (actually sources in this case) that we can't handle
        mov     ax,ds:[esi].DIBENGINE.deFlags
        test    ax,SELECTEDDIB
        jnz     GN_BitmapBitsPunt
        test    eax,VRAM
        jz      NotVrm
        mov     dwInVram,1

        ; We may be able to use the hardware, but only if the
        ; number of bytes we are asked to copy is an integral
        ; number of scanlines. If there is a partial scanline,
        ; then we could do it, but since we only process
        ; rectangles with the hardware, we would have to do it
        ; in two operations. The 2 rect case almost never
        ; happens, so we use software for the case.
        mov     eax,dwTotalBytesToCopy
        movzx   ecx,ds:[esi].DIBENGINE.deWidthBytes
        sub     edx,edx
        idiv    ecx
        mov     dwXferHeight,eax
        or      edx,edx
        jne     NotVrm

        ; USE THE HARDWARE HERE
        ; Call the generic code to overlap DMAing and converting.
        sub     ebx,ebx
        push    cs:[CSpGNLogicalDevice]
        push    lpDstDev
        push    ebx
        movzx   eax,word ptr lpBits+2
        movzx   ecx,ds:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        movzx   eax,word ptr lpBits
        push    eax
        push    ecx
        movzx   eax,ds:[esi].DIBENGINE.deWidthBytes
        push    eax
        push    ebx                                             ;wDstX
        push    ebx                                             ;wDstY
        push    ebx                                             ;lpSrcDev
        movzx   eax,word ptr ds:[esi].DIBENGINE.deBits+4
        push    eax
        push    dword ptr ds:[esi].DIBENGINE.deBits
        push    ecx
        push    ds:[esi].DIBENGINE.deDeltaScan
        push    ebx                                             ;wSrcX
        push    ebx                                             ;wSrcY
        movzx   eax,ds:[esi].DIBENGINE.deWidth
        push    eax
        push    dwXferHeight
        mov     eax,0CC0000H
        push    eax
        push    ebx
        push    ebx
        movzx   ebx,cs:bTableBppToIndex[ecx]
        mov     edx,ebx
        cmp     ebx,MAX_BPP_INDICES
        jge     NotVrm
        imul    ebx,MAX_BPP_INDICES
        add     ebx,edx
        mov     ecx,cs:dwTableBppToXlatFunc[ebx * 4]
        push    ecx
        mov     ds,cs:[wFlatDataSel]
        call    GN_BitBltSrcVramDstSystem
        or      eax,eax
        jg      GN_BitmapBitsSuccess
        je      NotVrm
        mov     dwTotalBytesToCopy,0
        jmp     GN_BitmapBitsSuccess                            ;Failure

NotVrm:
        ; Load up the ptrs
        movzx   ebx,ds:[esi].DIBENGINE.deWidthBytes
        mov     ecx,ds:[esi].DIBENGINE.deDeltaScan
        les     di,lpBits
        lds     esi,ds:[esi].DIBENGINE.deBits
        mov     dwBytesToCopyPerWholeScan,ebx
        sub     ecx,ebx
        je      BitmapBits_CopyAll

        ; The pitch and width of the bitmap are not the same so ...
        sub     eax,eax
        mov     dwSrcWrapWidth,ecx
        mov     dwDstWrapWidth,eax
        jmp     BitmapBits_CopyByScan

BitmapBits_Get  ENDP


;==============================================================================
;
; Function:     BitmapBits_Copy
;
; Purpose:      This function is jumped to when the BitmapBits routine
;               is called with dwFlags equal to DDB_COPY. In this flavor
;               of the routine, the idea is to copy dwCount bytes from
;               the src bitmap whose header is pointed to by lpBits to
;               the dst bitmap whose header is pointed to by lpDstDev.
;
;               If one of lpBits or lpDstDev points to a header which
;               is in VRAM, then hardware acceleration can be used.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;
;==============================================================================
PUBLIC  BitmapBits_Copy
BitmapBits_Copy PROC    NEAR

        ; Load up the ptrs to both bitmap headers
        sub     esi,esi
        sub     edi,edi
        lds     si,lpBits
        les     di,lpDstDev
        movzx   eax,ds:[esi].DIBENGINE.deBitsPixel
        mov     dwSrcBpp,eax

        ; With two bitmaps, things are different. We will not copy
        ; more bytes per scan than the narrower of two bitmaps.
        movzx   eax,ds:[esi].DIBENGINE.deWidthBytes
        movzx   ecx,es:[edi].DIBENGINE.deWidthBytes
        movzx   edx,ds:[esi].DIBENGINE.deWidth
        cmp     eax,ecx
        jc      @F
        movzx   edx,es:[edi].DIBENGINE.deWidth
        mov     eax,ecx
@@:     mov     dwBytesToCopyPerWholeScan,eax
        mov     dwPixelsToCopyPerWholeScan,edx

        ; We will not copy more scans than the shorter of the two.
        movzx   ebx,ds:[esi].DIBENGINE.deHeight
        movzx   edx,es:[edi].DIBENGINE.deHeight
        cmp     ebx,edx
        jc      @F
        mov     ebx,edx
@@:     mov     dwWholeScansToCopy,ebx

        ; For this call, we ignore the dwCount parameter and just copy
        ; the src to the dst clipping to the smaller of the two bitmaps.
        imul    eax,ebx
        mov     dwTotalBytesToCopy,eax

        ; Figure out how many scans this number of bytes correpsonds to:
        sub     edx,edx
        div     dwBytesToCopyPerWholeScan
        mov     dwBytesToCopyPerPartialScan,edx
        cmp     eax,dwWholeScansToCopy
        jc      @F
        mov     dwBytesToCopyPerPartialScan,0
        mov     eax,dwWholeScansToCopy
@@:     mov     dwWholeScansToCopy,eax

        ; Punt those we don't handle (very rare that we punt)
        movzx   eax,ds:[esi].DIBENGINE.deFlags
        movzx   ebx,es:[edi].DIBENGINE.deFlags
        test    eax,SELECTEDDIB
        jnz     GN_BitmapBitsPunt
        test    ebx,SELECTEDDIB
        jnz     GN_BitmapBitsPunt

        ; Now break out into the four cases --
        ; sys->sys, sys->vram, vram->sys, vram->vram
        ; We can use the hardware for the last three cases
        test    eax,VRAM
        jz      @F
        test    ebx,VRAM
        jnz     BitmapBits_CopyVramToVram
        jmp     BitmapBits_CopyVramToSystem
@@:     test    ebx,VRAM
        jnz     BitmapBits_CopySystemToVram
        jmp     BitmapBits_CopySystemToSystem

BitmapBits_Copy ENDP


;==============================================================================
;
; Function:     BitmapBits_CopyVramToVram
;
; Purpose:      This function is jumped to when the BitmapBits routine
;               is called with dwFlags equal to DDB_COPY AND the src
;               and dst bitmaps are both known to be in vram.
;
;               Since, in this call, we are asked to do a certain number
;               of bytes and not a specified rectangle, it may be that
;               there are some whole number of scans to be copied and then
;               a portion of a scan. We must treat these as two separate
;               blts.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;               dwBytesToCopyPerWholeScan
;               dwPixelsToCopyPerWholeScan
;               dwWholeScansToCopy
;
;==============================================================================
PUBLIC  BitmapBits_CopyVramToVram
BitmapBits_CopyVramToVram       PROC    NEAR

        cmp     dwWholeScansToCopy,0
        je      @F
        mov     ds,cs:[wFlatDataSel]
        mov     esi,cs:[CSpGNLogicalDevice]
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    lpDstDev
        push    lpBits
        sub     eax,eax
        push    eax                             ; lpBrush
        push    eax                             ; lpDrawMode
        push    eax                             ; wDstX:wDstY
        push    eax                             ; wSrcX:wSrcY
        mov     eax,dwPixelsToCopyPerWholeScan
        shl     eax,10H
        or      eax,dwWholeScansToCopy
        push    eax                             ; wExtX:wExtY
        mov     ecx,0CC0000H
        push    ecx
        call    HW_BitBltSrcVramDstVram
        or      eax,eax
        jle     FailHW
@@:
        ; Do the extra portion of a scan
        cmp     dwBytesToCopyPerPartialScan,0
        je      GN_BitmapBitsSuccess
        mov     esi,cs:[CSpGNLogicalDevice]
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    lpDstDev
        push    lpBits
        sub     eax,eax
        push    eax                             ; lpBrush
        push    eax                             ; lpDrawMode
        mov     eax,dwWholeScansToCopy
        push    eax                             ; wDstX:wDstY
        push    eax                             ; wSrcX:wSrcY

        ; To how many pixels do the extra bytes correspond?
        mov     eax,dwBytesToCopyPerPartialScan
        cmp     dwSrcBpp,8
        je      @F
        shr     eax,1
        cmp     dwSrcBpp,32
        jc      @F
        shr     eax,1
@@:     shl     eax,10H
        mov     ax,1
        push    eax
        mov     ecx,0CC0000H
        push    ecx
        call    HW_BitBltSrcVramDstVram
        or      eax,eax
        jg      GN_BitmapBitsSuccess

FailHW:
        jmp     BitmapBits_CopySystemToSystem

BitmapBits_CopyVramToVram       ENDP


;==============================================================================
;
; Function:     BitmapBits_CopySystemToVram
;
; Purpose:      This function is jumped to when the BitmapBits routine
;               is called with dwFlags equal to DDB_COPY AND the src
;               bitmap is in system memory and the and dst bitmap is in
;               VRAM.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;               ds:esi  DIBENGINE ptr for src
;               es:edi  DIBENGINE ptr for dst
;               dwBytesToCopyPerWholeScan
;               dwPixelsToCopyPerWholeScan
;               dwWholeScansToCopy
;
;==============================================================================
PUBLIC  BitmapBits_CopySystemToVram
BitmapBits_CopySystemToVram     PROC    NEAR

        ; Let the vendor handle the whole thing
        sub     ebx,ebx
        push    cs:[CSpGNLogicalDevice]
        movzx   eax,word ptr es:[edi].DIBENGINE.deBits+4
        movzx   ecx,es:[edi].DIBENGINEHDR.bRealBpp
        push    eax
        push    dword ptr es:[edi].DIBENGINE.deBits
        push    ecx
        push    es:[edi].DIBENGINE.deDeltaScan
        push    ebx                                             ;wDstX
        push    ebx                                             ;wDstY
        movzx   eax,word ptr es:[edi].DIBENGINE.deBits+4
        push    eax
        push    dword ptr es:[edi].DIBENGINE.deBits
        movzx   eax,ds:[esi].DIBENGINE.deBitsPixel
        push    eax
        push    ds:[esi].DIBENGINE.deDeltaScan
        push    ebx                                             ;wSrcX
        push    ebx                                             ;wSrcY
        movzx   eax,word ptr ds:[esi].DIBENGINE.deWidth
        push    eax
        movzx   eax,word ptr ds:[esi].DIBENGINE.deHeight
        push    eax
        mov     ecx,0CC0000H
        push    ecx
        push    ebx                                             ;lpBrush
        push    ebx                                             ;lpDrawMode
        push    ebx                                             ;size color table entry in bytes
        push    ebx                                             ;num color table entries
        push    ebx                                             ;lpColorTable
        push    ebx
        mov     ds,cs:[wFlatDataSel]
        call    GN_BitBltSrcColorDstVram
        or      eax,eax
        jg      GN_BitmapBitsSuccess
        mov     dwInVram,1
        je      BitmapBits_CopySystemToSystem
        mov     dwTotalBytesToCopy,0
        jmp     GN_BitmapBitsSuccess                            ;Failure

BitmapBits_CopySystemToVram     ENDP


;==============================================================================
;
; Function:     BitmapBits_CopyVramToSystem
;
; Purpose:      This function is jumped to when the BitmapBits routine
;               is called with dwFlags equal to DDB_COPY AND the src
;               bitmap is in video memory and the and dst bitmap is in
;               system memory.
;
;               Since, in this call, we are asked to do a certain number
;               of bytes and not a specified rectangle, it may be that
;               there are some whole number of scans to be copied and then
;               a portion of a scan. We must treat these as two separate
;               blts.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;               dwBytesToCopyPerWholeScan
;               dwPixelsToCopyPerWholeScan
;               dwWholeScansToCopy
;
;==============================================================================
PUBLIC  BitmapBits_CopyVramToSystem
BitmapBits_CopyVramToSystem     PROC    NEAR

        ; Call the generic code to overlap DMAing and converting.
        sub     ebx,ebx
        push    cs:[CSpGNLogicalDevice]
        push    lpDstDev
        push    ebx
        movzx   eax,es:[edi].DIBENGINEHDR.deBitsSelector
        movzx   ecx,ds:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    es:[edi].DIBENGINEHDR.deBitsOffset
        push    ecx
        push    es:[edi].DIBENGINEHDR.deDeltaScan
        push    ebx                                             ;wDstX
        push    ebx                                             ;wDstY
        push    ebx                                             ;lpSrcDev
        movzx   eax,ds:[esi].DIBENGINEHDR.deBitsSelector
        movzx   ecx,ds:[esi].DIBENGINEHDR.bRealBpp
        push    eax
        push    ds:[esi].DIBENGINEHDR.deBitsOffset
        push    ecx
        push    ds:[esi].DIBENGINE.deDeltaScan
        push    ebx                                             ;wSrcX
        push    ebx                                             ;wSrcY
        movzx   eax,ds:[esi].DIBENGINE.deWidth
        push    eax
        movzx   eax,ds:[esi].DIBENGINE.deHeight
        push    eax
        mov     eax,0CC0000H
        push    eax
        push    ebx
        push    ebx
        movzx   ebx,cs:bTableBppToIndex[ecx]
        mov     edx,ebx
        imul    ebx,MAX_BPP_INDICES
        add     ebx,edx
        mov     ecx,cs:dwTableBppToXlatFunc[ebx * 4]
        push    ecx
        mov     ds,cs:[wFlatDataSel]
        call    GN_BitBltSrcVramDstSystem
        or      eax,eax
        jg      GN_BitmapBitsSuccess
        mov     dwInVram,1
        je      BitmapBits_CopySystemToSystem
        mov     dwTotalBytesToCopy,0
        jmp     GN_BitmapBitsSuccess                            ;Failure

BitmapBits_CopyVramToSystem     ENDP


;==============================================================================
;
; Function:     BitmapBits_CopySystemToSystem
;
; Purpose:      This function is jumped to when the BitmapBits routine
;               is called with dwFlags equal to DDB_COPY AND the src
;               and dst bitmaps are both in system memory.
;
;               Since, in this call, we are asked to do a certain number
;               of bytes and not a specified rectangle, it may be that
;               there are some whole number of scans to be copied and then
;               a portion of a scan. We must treat these as two separate
;               blts.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;               dwBytesToCopyPerWholeScan
;               dwPixelsToCopyPerWholeScan
;               dwWholeScansToCopy              can be 0
;               dwBytesToCopyPerPartialScan     can be 0
;
;==============================================================================
PUBLIC  BitmapBits_CopySystemToSystem
BitmapBits_CopySystemToSystem   PROC    NEAR

        ; Reload these ptrs and do it in software
        sub     esi,esi
        sub     edi,edi
        lds     si,lpBits
        les     di,lpDstDev

        ; We can treat the bitmaps as one big linear array of bytes if
        ; the width in bytes and pitches of the bitmaps match.
        movzx   eax,ds:[esi].DIBENGINE.deWidthBytes
        movzx   ebx,es:[edi].DIBENGINE.deWidthBytes
        mov     ecx,ds:[esi].DIBENGINE.deDeltaScan
        mov     edx,es:[edi].DIBENGINE.deDeltaScan
        add     eax,3
        add     ebx,3
        and     eax,NOT 3
        and     ebx,NOT 3
        cmp     eax,ebx
        jne     AsRect
        cmp     eax,ecx
        jne     AsRect
        cmp     ebx,edx
        jne     AsRect

        ; Linear it is, copy up all the bytes desired
        imul    ecx,dwWholeScansToCopy
        add     ecx,dwBytesToCopyPerPartialScan
        mov     dwTotalBytesToCopy,ecx
        lds     esi,ds:[esi].DIBENGINE.deBits
        les     edi,es:[edi].DIBENGINE.deBits
        jmp     BitmapBits_CopyAll

AsRect:
        ; Compute wrap widths, load ptrs and go copy it
        sub     ecx,dwBytesToCopyPerWholeScan
        sub     edx,dwBytesToCopyPerWholeScan
        mov     dwSrcWrapWidth,ecx
        mov     dwDstWrapWidth,edx
        lds     esi,ds:[esi].DIBENGINE.deBits
        les     edi,es:[edi].DIBENGINE.deBits
        jmp     BitmapBits_CopyByScan

BitmapBits_CopySystemToSystem   ENDP


;==============================================================================
;
; Function:     BitmapBits_CopyByScan
;
; Purpose:      This function is jumped to when we are ready to copy
;               data from one place to another. Unfortunately,it is
;               not just one long linear block, but rather rectangle
;               to rectangle. Wait for idle, if needed, and then copy.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;               ds:esi                  ptr to src bits
;               es:edi                  ptr to dst bits
;               dwTotalBytesToCopy
;               dwBytesToCopyPerWholeScan
;               dwSrcWrapWidth
;               dwDstWrapWidth
;
;==============================================================================
PUBLIC  BitmapBits_CopyByScan
BitmapBits_CopyByScan   PROC    NEAR

        cmp     dwInVram,0
        jz      @F

        ; Syncronize with the engine
        push    cs:[CSlp16LogDevice]
        push    eax
        push    eax
        pushw   FB_ACCESS
        call    GENERIC_BeginAccess
@@:
        ; Compute the dword and partial dword counts
        mov     eax,dwBytesToCopyPerWholeScan
        mov     edx,dwTotalBytesToCopy
        jmp     @F
LoopY:
        mov     ecx,eax
        shr     ecx,2
        rep     movs dword ptr es:[edi], ds:[esi]
        mov     ecx,eax
        and     ecx,3
        rep     movs byte ptr es:[edi], ds:[esi]
        add     esi,dwSrcWrapWidth
        add     edi,dwDstWrapWidth
@@:     sub     edx,eax
        jge     LoopY

        ; Check for a partial scan
        add     edx,eax
        jle     GN_BitmapBitsSuccess
        mov     ecx,edx
        shr     ecx,2
        rep     movs dword ptr es:[edi], ds:[esi]
        mov     ecx,edx
        and     ecx,3
        rep     movs byte ptr es:[edi], ds:[esi]

        cmp     dwInVram,0
        jz      GN_BitmapBitsSuccess
        push    cs:[CSlp16LogDevice]
        pushw   FB_ACCESS
        call    GENERIC_EndAccess
        jmp     GN_BitmapBitsSuccess

BitmapBits_CopyByScan   ENDP


;==============================================================================
;
; Function:     BitmapBits_CopyAll
;
; Purpose:      This function is jumped to when we are ready to copy
;               data from one place to another. It is just one long
;               linear block. We may need to wait for idle, though.
;
; Arguments:    BitmapBits Display Driver Stack Frame
;               dwTotalBytesToCopy
;
;==============================================================================
PUBLIC  BitmapBits_CopyAll
BitmapBits_CopyAll      PROC    NEAR

        cmp     dwInVram,0
        jz      @F

        ; Syncronize with the engine
        push    lpDstDev
        push    eax
        push    eax
        pushw   FB_ACCESS
        call    GENERIC_BeginAccess
@@:
        mov     ecx,dwTotalBytesToCopy
        shr     ecx,2
        rep     movs dword ptr es:[edi], ds:[esi]
        mov     ecx,dwTotalBytesToCopy
        and     ecx,3
        rep     movs byte ptr es:[edi], ds:[esi]

        cmp     dwInVram,0
        jz      GN_BitmapBitsSuccess
        push    lpDstDev
        pushw   FB_ACCESS
        call    GENERIC_EndAccess
        jmp     GN_BitmapBitsSuccess

BitmapBits_CopyAll      ENDP

CLOSESEG _NVTEXT32

END

