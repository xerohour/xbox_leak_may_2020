;******************************* Module Header *********************************
;
; Module Name: NV4BLIT.ASM
;
; Contains the x86 'Asm' versions of some inner-loop routines for the
; partially hardware accelerated Blt.
;
; Copyright (c) 1994-1995 Microsoft Corporation
;
;*******************************************************************************

;*******************************************************************************
;*                                                                             *
;*   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
;*                                                                             *
;*******************************************************************************

        .386

        .model  small,c

        assume cs:FLAT,ds:FLAT,es:FLAT,ss:FLAT
        assume fs:nothing,gs:nothing

        .xlist
        include stdcall.inc                     ;calling convention cmacros
        include i386\strucs.inc
        .list

        .data

;*******************************************************************************
; Dma Push version of structure
;*******************************************************************************

NV4_DMAPUSH_TRANSFER_BLITDATA_STRUC          struc

; Feel free to add any local variables here:

DwordsNeeded                dd      ?
NextScanInc                 dd      ?
RemainingDwordsPerDstScan   dd      ?
LocalHeight                 dd      ?

; Don't add any fields below here without modifying XXX_STRUC_SIZE!

NV4DTBDS_ebp         dd      ?
NV4DTBDS_esi         dd      ?
NV4DTBDS_edi         dd      ?
NV4DTBDS_ebx         dd      ?
NV4DTBDS_RetAddr     dd      ?
passedPpdev          dd      ?
BytesPerDstScan      dd      ?
Height               dd      ?
NextScanPtr          dd      ?
ScanInc              dd      ?

NV4_DMAPUSH_TRANSFER_BLITDATA_STRUC          ends

NV4_DMAPUSH_TRANSFER_BLITDATA_PARM_SIZE     equ     10 * 4

        .code


extrn       NV_DmaPush_Wrap@8:NEAR
extrn       NV_DmaPush_CheckWrapped@8:NEAR
extrn       NV4_DmaPushSend@4:NEAR


;---------------------------Public-Routine------------------------------;
;
; Routine to output a memory bitmap to the screen using the DMA push
; buffer.
;
;-----------------------------------------------------------------------;

align 4

        public NV4_DmaPush_Transfer_MemToScreen_Data@20


NV4_DmaPush_Transfer_MemToScreen_Data@20 proc near

        ;**********************************************************************
        ;
        ; use ebp as general register, use esp for parameter and local access
        ; save ebp,ebx,esi,edi
        ;
        ;**********************************************************************

        push    ebx
        push    edi
        push    esi
        push    ebp

        sub     esp, (size NV4_DMAPUSH_TRANSFER_BLITDATA_STRUC) - NV4_DMAPUSH_TRANSFER_BLITDATA_PARM_SIZE
        mov     ebp, esp                        ; points to parameters and local vars

        cld                                     ; Default to going forward
        xor     eax, eax
        mov     ecx, [ebp].Height               ; Number of scanlines to output
        mov     ebx, [ebp].passedPpdev          ; Ptr to pdev
        or      ecx, ecx                        ; If zero scanliness
        jz      TransferDone                    ; nothing to draw
        
        mov     [ebp].LocalHeight, ecx          ; Save height parameter
        mov     esi, [ebp].ScanInc
        mov     [ebx].nvDmaCachedFreeCount, eax ; init free count to zero

        mov     edx, [ebx].nvDmaCount
        mov     edi, [ebx].nvDmaFifo            ; pointer to DMA buffer
        mov     ecx, [ebp].BytesPerDstScan      ; number of BYTES in one scanline
        shl     edx, 2                          ; offset in DMA buffer
                                                ;  to point to next scanline
        shl     esi, 2                          ; byte increment to next scanline                                                    
        add     edi, edx                        ; offset to current location
                                                ;  in DMA buffer
        mov     edx, ecx                        ; BytesPerDstScan                                                
        
        cmp     edx, MAX_INDEXED_IMAGE_DWORDS*4 ; if more bytes than hardware can handle
        jl      NormalScanlineTransfer          ; then need to adjust RemainingDwordsPerDstScan
                                                ;  value
        sub     edx, MAX_INDEXED_IMAGE_DWORDS*4 ; adjust remaining byte count

NormalScanlineTransfer:
        sub     esi, ecx                        ; number of bytes to increment
        shr     edx, 2                          ; number of whole DWORDS in
                                                ;  remaining scanline
        shr     ecx, 2                          ; number of DWORDS in scanline
        
        mov     [ebp].RemainingDwordsPerDstScan, edx ; save dwords needed in main loop
        add     ecx, 4                          ; add max overhead for DMA push cmd data

        mov     [ebp].NextScanInc, esi          ; save increment to next scanline
        mov     [ebp].DwordsNeeded, ecx         ; save max number of DWORDS needed
                                                ;  in push buffer for each scanline

;*************************************************************************
;
;   Output source bitmap to DMA push buffer.
;
;   EBX - ppdev
;   EDI - offset to current location in DMA buffer
;
;*************************************************************************

TransferBitmapLoop:

        mov     ecx, [ebp].DwordsNeeded         ; number of DWORDS in one scanline
                                                ;  with max overhead
        sub     [ebx].nvDmaCachedFreeCount, ecx ; overhead per glyph
        jg      HaveRoomForScanline             ; have enough room in DMA buffer
                                                ;   for current scanline

    ;*************************************************************************
    ;
    ; Get current free count in DMA buffer. Updates ppdev->nvDmaCachedFreeCount.
    ; This is similar to the NV_DMAPUSH_CHECKFREE C macro.
    ;
    ; NOTE: updates ppdev->nvDmaCount to equal local nvDmaCount
    ;
    ; EBX - ppdev
    ; ECX - number of DWORDS needed in DMA buffer
    ; EDI - offset to current location in DMA buffer
    ;
    ;*************************************************************************

        ;******************************************************************
        ; Kickoff buffer when not enough room in cached buffer free count
        ;******************************************************************

        mov     edx, [ebx].nvDmaChannel
        sub     edi, [ebx].nvDmaFifo            ; offset within DMA buffer
        
        shr     edi, 2                          ; convert to dwords
        mov     [ebx].nvDmaCount, edi           ; update ppdev->nvDmaCount

        push    ecx        
        push    ebx
        push    ebx
        call    NV4_DmaPushSend@4
        pop     ebx
        pop     ecx

        cmp     [ebx].nvDmaWrapFlag, FALSE      ;ppdev->nvDmaWrapFlag
        jne     short CheckDmaWrapped

    ;*************************************************************************
    ;
    ; WRAP_FLAG = FALSE. Check if we need to wrap around.
    ;
    ; EBX - ppdev
    ; ECX - number of DWORDS needed in DMA buffer
    ;
    ;*************************************************************************

        mov     eax, [ebx].DmaPushBufTotalSize  ;
        shr     eax, 2                          ;
        sub     eax, DMAPUSH_PADDING            ;
        sub     eax, [ebx].nvDmaCount           ; less current nvDmaCount
        sub     eax, ecx                        ; less number of DWORDS needed
        jge     UpdateFreeCount                 ; have enough room in DMA buffer
        
    ;*************************************************************************
    ;
    ; Need to wrap.
    ;
    ; EBX - ppdev
    ; ECX - number of DWORDS needed in DMA buffer
    ;
    ;*************************************************************************

GoWrapDmaBuffer:

        push    ecx                             ; DWORDS needed - parameter
        push    ebx                             ; ppdev - parameter
        call    NV_DmaPush_Wrap@8               ; (ppdev, DWORDS_NEEDED)
        jmp     ZeroFreeCount                   ; need to check next time

    ;*************************************************************************
    ;
    ; WRAP_FLAG = TRUE. We are currently wrapped around (DMA count < Get Ptr)
    ;
    ; EBX - ppdev
    ; ECX - number of DWORDS needed in DMA buffer
    ;
    ;*************************************************************************

CheckDmaWrapped:

        push    ecx                             ; DWORDS needed - parameter
        push    ebx                             ; ppdev - parameter
        call    NV_DmaPush_CheckWrapped@8       ; (ppdev, DWORDS_NEEDED)

    ;*************************************************************************
    ;
    ; Set free count. This will be zero if we are wrapped. This way, we will
    ; force a check until buffer is not wrapped. 
    ;
    ;*************************************************************************

ZeroFreeCount:
        mov     ebx, [ebp].passedPpdev          ; reload ppdev
        xor     eax, eax                        ; set freecount to zero - will
                                                ;  force check next pass

    ;*************************************************************************
    ;
    ; WRAP_FLAG = FALSE. Check if we need to wrap around.
    ;
    ; EAX - number of free dwords in DMA push buffer
    ; EBX - ppdev
    ;
    ;*************************************************************************

UpdateFreeCount:
        mov     [ebx].nvDmaCachedFreeCount, eax
        mov     edx, [ebx].nvDmaCount
        mov     edi, [ebx].nvDmaFifo            ; pointer to DMA buffer
        shl     edx, 2                          ; offset in DMA buffer
        add     edi, edx                        ; offset to current location
                                                ;  in DMA buffer

    ;*************************************************************************
    ;
    ; Have room in DMA buffer, now output scanline.
    ;
    ; EBX - ppdev
    ; EDI - offset to current location in DMA buffer
    ;
    ;*************************************************************************

HaveRoomForScanline:

        mov     eax, [ebp].BytesPerDstScan
        mov     esi, [ebp].NextScanPtr
        
        cmp     eax, MAX_INDEXED_IMAGE_DWORDS*4 ; if more bytes than hardware can handle
        jge     TransferBigScanline             ; then output max dwords
                                                ; (jump away for performance since
                                                ;  this almost never happens and processors
                                                ;  want to fall through conditional jumps)

    ;*************************************************************************
    ;
    ; Handle remaining dwords.
    ;
    ; EAX - number of bytes in scanline to output
    ; EBX - ppdev
    ; EDI - offset to current location in DMA buffer
    ; ESI - offset to current location in source bitmap
    ;
    ;*************************************************************************
    
TransferRemainingDwords:

        mov     edx, [ebp].RemainingDwordsPerDstScan ; rounded up dwords in scanline
        mov     ecx, edx                        ; total dwords remaining
        test    edx, edx
        jz      TransferRemainingBytes          ; no more dwords to send, check
                                                ;  whether bytes to output
                                                
        shl     ecx, 18                         ; convert bytes to #Dwords for header
        or      ecx, IMAGE_FROM_CPU_SUBCHANNEL OR NV061_COLOR0
        mov     DWORD PTR [edi], ecx
        add     edi, 4

        mov     ecx, edx                        ; total dwords remaining
        rep     movsd
        
    ;*************************************************************************
    ;
    ;   Handle remaining bytes. Note: This is necessary because the end of the
    ;   bitmap is not always an even number of dwords from the start. Also,
    ;   the bitmap often begins at the highest scanline and progresses backwards.
    ;   For this reason, you can't just special case the last line.
    ;
    ;   EAX - number of bytes in scanline to output
    ;   EBX - ppdev
    ;   EDI - offset to current location in DMA buffer
    ;   ESI - offset to current location in source bitmap
    ;
    ;*************************************************************************

TransferRemainingBytes:
    
        and     eax, 3                          ; bytes leftover
        jz      NextScanline                    ; no bytes to draw, scanline finished

        mov     DWORD PTR [edi], IMAGE_FROM_CPU_SUBCHANNEL OR NV061_COLOR0 OR (1 SHL 18)
        add     edi, 4
        
        mov     ecx, eax                        ; remaining bytes
        rep     movsb
        add     edi, 4                          ; update DMA buffer to next DWORD
        sub     edi, eax

    ;******************************************************************
    ;
    ; All data from scanline is copied to DMA push buffer. Now update
    ; NextScanPtr to point to offset of next source scanline.
    ;
    ;   EBX - ppdev
    ;   EDI - offset to current location in DMA buffer
    ;   ESI - offset to current location in source bitmap
    ;
    ;******************************************************************

NextScanline:
        
        add     esi, [ebp].NextScanInc          ; points to next scanline
        mov     [ebp].NextScanPtr, esi          ; save update bitmap pointer
        dec     [ebp].LocalHeight               ; if more scanlines to draw
        jnz     TransferBitmapLoop              ; then draw next scanline
        
    ;******************************************************************
    ;
    ; All data from scanline is copied to DMA push buffer. 
    ; Kickoff buffer after last scanline
    ;
    ;   EBX - ppdev
    ;   EDI - offset to current location in DMA buffer
    ;
    ;******************************************************************

        mov     esi, [ebx].nvDmaChannel
        sub     edi, [ebx].nvDmaFifo            ; offset within DMA buffer
        
        shr     edi, 2                          ; convert to dwords
        mov     [ebx].nvDmaCount, edi           ; update ppdev->nvDmaCount

        push    ebx
        call    NV4_DmaPushSend@4

TransferDone:
        
        add     esp,(size NV4_DMAPUSH_TRANSFER_BLITDATA_STRUC) - NV4_DMAPUSH_TRANSFER_BLITDATA_PARM_SIZE
        pop     ebp
        pop     esi
        pop     edi
        pop     ebx

        ret     20

    ;*************************************************************************
    ;
    ;   Output the maximum amount of dwords allowed by hardware. Only need to
    ;   do this once, since no scanlines will be 2 times MAX_INDEXED_IMAGE_DWORDS
    ;
    ;   This code was moved from above loop for performance, since this rarely happens
    ;   and processors' branch prediction wants to fall through conditional jumps.
    ;
    ;   EAX - number of bytes in scanline
    ;   EBX - ppdev
    ;   EDI - offset to current location in DMA buffer
    ;   ESI - offset to current location in source bitmap
    ;
    ;*************************************************************************

TransferBigScanline:

        mov     DWORD PTR [edi], IMAGE_FROM_CPU_SUBCHANNEL OR NV061_COLOR0 OR (MAX_INDEXED_IMAGE_DWORDS SHL 18)
        add     edi, 4

        mov     ecx, MAX_INDEXED_IMAGE_DWORDS
        rep     movsd
        
        sub     eax, MAX_INDEXED_IMAGE_DWORDS*4 ; number of bytes left in scanline
        jmp     TransferRemainingDwords         ; output rest of dwords in scanline

NV4_DmaPush_Transfer_MemToScreen_Data@20 endp



end



