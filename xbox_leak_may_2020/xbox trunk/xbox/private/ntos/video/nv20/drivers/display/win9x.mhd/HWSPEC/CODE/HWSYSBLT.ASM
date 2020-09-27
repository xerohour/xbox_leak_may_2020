;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwsysblt.asm
;
; Purpose:      This file has routines that assist the generic code
;               in doing blts involving a src in video memory and
;               a dst in system memory. There may be a pattern involved
;               with the rop as well, but there may not be.
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
include hwcrtc.inc
include hwboard.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     HW_BeginSrcVramDstSystem
;
; Purpose:      This function is an easier way to implement blts from
;               video memory back to system memory. The generic code
;               actually orchestrates the process. First it calls this
;               routine to do setup. The the generic code will make
;               some number of calls to HW_BlockSrcVramDstSystem and
;               HW_WaitSrcVramDstSystem which will DMA a chunk of the
;               src back into system memory and wait for its completion.
;               The generic code assumes you have at least two buffers
;               so that it can overlap the DMAing of one with the
;               processing of the other.
;               The generic code processes each chunk ropping it
;               into place in the dst bitmap. Finally when the entire
;               src has been transferred, the generic code calls the
;               routine HW_EndSrcVramDstSystem so the hardware specific
;               code can do cleanup. You will need to do the following
;               in this routine:
;
;               1) Anything special you need to do to prepare the
;                  hardware for multiple DMA transfers.
;
;               2) Compute and return the following three things needed by
;                  the generic code:
;                  dwTransferBufferPitchInBytes -- this is the byte width
;                       of a single scan of the DMAed bitmap. The generic
;                       code knows the dwExtX and the dwSrcBpp, but the
;                       hardware may wish to pad each line out to a
;                       multiple of a dword or a quadword or whatever.
;                       Since the generic code is doing the actual copy
;                       and ropping of the transferred data with the dst,
;                       it needs to know the transfer area pitch.
;                  dwMaxScanlinesPerTransfer -- just in case there is a
;                       limit on how many this can be. For example, the
;                       hardware may only be able to handle up to a
;                       certain number of lines or bytes in a transfer.
;                       Alternatively, the transfer area may have a
;                       limited size. Based upon the variable computed
;                       above, dwTransferBufferPitchInBytes, only some
;                       certain number of scans may fit in the buffer. e.g.
;                       if each scan is 1K long and the transfer area
;                       is 64K in size, then only 64 scans can be
;                       transferred at a time.
;
; Arguments:
;               ds                      Context Sel
;               pHWLogdevCXOffset       HWLOGDEVCX
;               dwSrcBitsSel    Selector for the src bitmap which is
;                               always the selector for the screen since
;                               the src bitmap is in offscreen memory.
;               dwSrcBitsOffset the offset in video memory of the base of
;                               the bitmap.
;               dwSrcBpp        the pixel depth of the src
;               dwSrcPitch      the pitch of the src bitmap
;               dwSrcLeft       Just in case the hardware cannot DMA back
;                               starting from arbitrary byte alignments,
;                               I have included this argument. You may
;                               need this when computing
;                               dwTransferBufferPitchInBytes
;               dwExtX          The X extent in pixels of the rectangle
;                               that we will be calling the routine
;                               HW_BlockSrcVramDstSystem to DMA back to
;                               system memory. This is also included
;                               here so you can do the appropriate
;                               computations for the return variables.
;
; Returns:
;       eax     0       need to punt
;               else    Keep going -- everything's fine
;                       In this case, you must also return
;                       ebx  -- dwTransferBufferPitchInBytes
;                       ecx  -- dwMaxScanlinesPerTransfer
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_BeginSrcVramDstSystem, PASCAL, FRAME, NEAR
PARMD   pHWLogdevCXOffset
PARMD   dwSrcBitsSel
PARMD   dwSrcBitsOffset
PARMD   dwSrcBpp
PARMD   dwSrcPitch
PARMD   dwSrcLeft
PARMD   dwExtX
OPENPROC
        mov     ebx,pHWLogdevCXOffset
        mov     ebx,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,ebx,eax

        ; Load the push buffer ptr
        mov     edi,ds:[ebx].HWBOARDCX.pCurrentPushBufferOffset

        ; Load up sub-channels 6 and 7 to use the MemoryToMemoryFormats
        CHECKDMASPACE ds,ebx,edi,10H
        mov     eax,CHNRUN(MemoryToMemoryFormat0Hdr,1)
        mov     ecx,OBJ_DMA_FROM_VRAM_TO_SYSRAM_0
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        mov     eax,CHNRUN(MemoryToMemoryFormat1Hdr,1)
        mov     ecx,OBJ_DMA_FROM_VRAM_TO_SYSRAM_1
        mov     ds:[edi + 08H],eax
        mov     ds:[edi + 0CH],ecx
        add     edi,10H
        SETPUTPTR ds,ebx,edi

        ; The transfer pitch in bytes is always a multiple of a dword
        mov     ebx,dwSrcBpp
        inc     ebx
        shr     ebx,3
        imul    ebx,dwExtX
        add     ebx,3
        and     ebx,NOT 3

        ; Since there must be at least 2 target buffers, limit the
        ; number of scanlines that can be transferred to the amount
        ; that will fit in half of the cacheblae buffer area.
        mov     eax,CACHEABLE_BUFFER_SIZE / 2
        sub     edx,edx
        div     ebx
        mov     ecx,eax
CLOSEPROC


;==============================================================================
;
; Function:     HW_BlockSrcVramDstSystem
;
; Purpose:      This function is an easy way to implement Blts where
;               the src is in vram and the dst is in system memory.
;               It is one of a sequence. For full details, please see
;               HW_BeginSrcVramDstSystem eariler in this file.
;
;               This routine must transfer a chunk of the src bitmap
;               back from vram into some system memory area. The base,
;               pitch, bitdepth, rectangle to transfer, and ID as to
;               which transfer buffer to use are all specified here.
;               NOTE: The pitch of a line in the transfer area must
;               be the same as the dwTransferBufferPitchInBytes
;               variable computed and returned to the generic code in
;               HW_BeginSrcVramDstSystem.
;
; Arguments:
;               ds              CXSel
;               ebx             0 or 1 -- which transfer area to use
;               pHWBoardCXOffset   HWBOARDCX
;               dwSrcBase       Offset in VRAM of start of bitmap
;               dwSrcBpp        bits per pixel of src (8, 15, 16, or 32)
;               dwSrcPitch      width of bytes of bitmap
;               dwSrcLeft       left edge coord of rectangle to transfer
;               dwSrcTop        top edge coord of rectangle to transfer
;               dwExtX          width in pixels of rectangle to transfer
;               dwExtY          height in scanlines of rectangle to transfer
;
; Returns:      This routine is not failable. If you wish to fail the
;               sequence, you must fail at HW_BeginSrcVramDstSystem time.
;
;               ecx     CTX offset of where rectangle was transferred
;
; Preserve:     ds,es,ebx
;==============================================================================
DECPROC HW_BlockSrcVramDstSystem, PASCAL, FRAME, NEAR
PARMD   pHWBoardCXOffset
PARMD   dwSrcBase
PARMD   dwSrcBpp
PARMD   dwSrcPitch
PARMD   dwSrcLeft
PARMD   dwSrcTop
PARMD   dwExtX
PARMD   dwExtY
OPENPROC
        ; Load the push buffer ptr
        mov     esi,pHWBoardCXOffset
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset

        CHECKDMASPACE ds,esi,edi,24H
        mov     eax,CHNRUN(MemoryToMemoryFormat0.dwOffsetIn,8)
        sub     edx,edx
        or      ebx,ebx
        je      @F
        mov     eax,CHNRUN(MemoryToMemoryFormat1.dwOffsetIn,8)
        mov     edx,CACHEABLE_BUFFER_SIZE / 2
@@:     mov     ds:[edi + 00H],eax
        mov     ecx,dwSrcTop
        imul    ecx,dwSrcPitch
        mov     eax,dwSrcBpp
        inc     eax
        shr     eax,3
        imul    eax,dwSrcLeft
        add     eax,ecx
        add     eax,dwSrcBase
        mov     ds:[edi + 04H],eax
        mov     ds:[edi + 08H],edx
        mov     eax,dwSrcPitch
        mov     ecx,dwSrcBpp
        inc     ecx
        shr     ecx,3
        imul    ecx,dwExtX
        lea     edx,[ecx + 3]
        and     edx,NOT 3
        mov     ds:[edi + 0CH],eax
        mov     ds:[edi + 10H],edx
        mov     eax,dwExtY
        mov     ds:[edi + 14H],ecx
        mov     ds:[edi + 18H],eax
        mov     eax,101H
        sub     ecx,ecx
        mov     ds:[edi + 1CH],eax
        mov     ds:[edi + 20H],ecx
        add     edi,24H
        lea     eax,ds:[esi].HWBOARDCX.sVramToSysramBufferNotifier0.status
        lea     ecx,[esi].HWBOARDCX.bCacheableDmaBuffer
        or      ebx,ebx
        je      @F
        lea     eax,ds:[esi].HWBOARDCX.sVramToSysramBufferNotifier1.status
        add     ecx,CACHEABLE_BUFFER_SIZE / 2
@@:     SETDMASTATUS <word ptr ds:[eax]>
        SETPUTPTR ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_WaitSrcVramDstSystem
;
; Purpose:      This function is an easy way to implement Blts where
;               the src is in vram and the dst is in system memory.
;               It is one of a sequence. For full details, please see
;               HW_BeginSrcVramDstSystem eariler in this file.
;
;               This routine must wait until the DMA transfer on the
;               specified transfer region is complete.
;
; Arguments:
;               ds              CXSel
;               edx             HWBOARDCX
;               ebx             0 or 1 -- which transfer area to wait for
;
; Returns:      This routine is not failable. If you wish to fail the
;               sequence, you must fail at HW_BeginSrcVramDstSystem time.
;
; Preserve:     ds,es,ebx
;==============================================================================
DECPROC HW_WaitSrcVramDstSystem, PASCAL, NOFRAME, NEAR
OPENPROC
        lea     eax,ds:[edx].HWBOARDCX.sVramToSysramBufferNotifier0.status
        or      ebx,ebx
        je      @F
        lea     eax,ds:[edx].HWBOARDCX.sVramToSysramBufferNotifier1.status
@@:     WAITDMASTATUS <word ptr ds:[eax]>
CLOSEPROC


;==============================================================================
;
; Function:     HW_EndSrcVramDstSystem
;
; Purpose:      This function is an easy way to implement Blts where
;               the src is in vram and the dst is in system memory.
;               It is one of a sequence. For full details, please see
;               HW_BeginSrcVramDstSystem eariler in this file.
;
; Arguments:
;               ds              CXSel
;               edx             HWBOARDCX
;
; Returns:      This routine is not failable. If you wish to fail the
;               sequence, you must fail at HW_BeginSrcVramDstSystem time.
;
; Preserve:     Nothing
;==============================================================================
DECPROC HW_EndSrcVramDstSystem, PASCAL, NOFRAME, NEAR
OPENPROC
CLOSEPROC

CLOSESEG _NVTEXT32

END

