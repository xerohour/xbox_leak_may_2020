;******************************* Module Header *********************************
;
; Module Name: NV1BLIT.ASM
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
        include stdcall.inc             ;calling convention cmacros
        include i386\strucs.inc
        .list

        .data

;
;  stack based params and local variables
;

NV1_TRANSFER_BLITDATA_STRUC          struc

; Feel free to add any local variables here:

NV1TBDS_RightCase            dd      ?
NV1TBDS_pjSrcScan            dd      ?
NV1TBDS_SrcIntStep           dd      ?
NV1TBDS_DstStride            dd      ?
NV1TBDS_XCount               dd      ?
NV1TBDS_yDst                 dd      ?
NV1TBDS_pdev                 dd      ?

; Don't add any fields below here without modifying XXX_STRUC_SIZE!

NV1TBDS_ebp                  dd      ?
NV1TBDS_esi                  dd      ?
NV1TBDS_edi                  dd      ?
NV1TBDS_ebx                  dd      ?
NV1TBDS_RetAddr              dd      ?
NV1TBDS_BytesPerDstScan      dd      ?
NV1TBDS_FreeCountPtr         dd      ?
NV1TBDS_Height               dd      ?
NV1TBDS_NextScanPtr          dd      ?
NV1TBDS_ScanInc              dd      ?
NV1TBDS_NvChannelPtr         dd      ?

NV1_TRANSFER_BLITDATA_STRUC          ends

NV1_TRANSFER_BLITDATA_PARM_SIZE     equ     11 * 4

;*******************************************************************************
; Defines for NV Hardware
; Currently, channels are used as follows:
;   IMAGE_FROM_CPU          Channel 0
;   BLIT                    Channel 1
;   IMAGE_MONO_FROM_CPU     Channel 2
;   RECTANGLE               Channel 3
;   ROP_SOLID               Channel 4
;   IMAGE_PATTERN           Channel 5
;   CLIP                    Channel 6
;   IMAGE_SOLID             Channel 7
;*******************************************************************************

IMAGE_FROM_CPU_FREECOUNT           equ    ( (0*8192) + 16 )
BLIT_FREECOUNT                     equ    ( (1*8192) + 16 )
IMAGE_MONO_FROM_CPU_FREECOUNT      equ    ( (2*8192) + 16 )
RECTANGLE_FREECOUNT                equ    ( (3*8192) + 16 )
ROP_SOLID_FREECOUNT                equ    ( (4*8192) + 16 )
IMAGE_PATTERN_FREECOUNT            equ    ( (5*8192) + 16 )
CLIP_FREECOUNT                     equ    ( (6*8192) + 16 )
IMAGE_SOLID_FREECOUNT              equ    ( (7*8192) + 16 )


IMAGE_FROM_CPU_DATA_0      equ    ( (0*8192) + 400h )
IMAGE_FROM_CPU_DATA_1      equ    ( (0*8192) + 404h )
IMAGE_FROM_CPU_DATA_2      equ    ( (0*8192) + 408h )
IMAGE_FROM_CPU_DATA_3      equ    ( (0*8192) + 40ch )
IMAGE_FROM_CPU_DATA_4      equ    ( (0*8192) + 410h )
IMAGE_FROM_CPU_DATA_5      equ    ( (0*8192) + 414h )
IMAGE_FROM_CPU_DATA_6      equ    ( (0*8192) + 418h )
IMAGE_FROM_CPU_DATA_7      equ    ( (0*8192) + 41ch )
IMAGE_FROM_CPU_DATA_8      equ    ( (0*8192) + 420h )
IMAGE_FROM_CPU_DATA_9      equ    ( (0*8192) + 424h )
IMAGE_FROM_CPU_DATA_10     equ    ( (0*8192) + 428h )
IMAGE_FROM_CPU_DATA_11     equ    ( (0*8192) + 42ch )
IMAGE_FROM_CPU_DATA_12     equ    ( (0*8192) + 430h )
IMAGE_FROM_CPU_DATA_13     equ    ( (0*8192) + 434h )
IMAGE_FROM_CPU_DATA_14     equ    ( (0*8192) + 438h )
IMAGE_FROM_CPU_DATA_15     equ    ( (0*8192) + 43ch )
IMAGE_FROM_CPU_DATA_16     equ    ( (0*8192) + 440h )
IMAGE_FROM_CPU_DATA_17     equ    ( (0*8192) + 444h )
IMAGE_FROM_CPU_DATA_18     equ    ( (0*8192) + 448h )
IMAGE_FROM_CPU_DATA_19     equ    ( (0*8192) + 44ch )
IMAGE_FROM_CPU_DATA_20     equ    ( (0*8192) + 450h )
IMAGE_FROM_CPU_DATA_21     equ    ( (0*8192) + 454h )
IMAGE_FROM_CPU_DATA_22     equ    ( (0*8192) + 458h )
IMAGE_FROM_CPU_DATA_23     equ    ( (0*8192) + 45ch )
IMAGE_FROM_CPU_DATA_24     equ    ( (0*8192) + 460h )
IMAGE_FROM_CPU_DATA_25     equ    ( (0*8192) + 464h )
IMAGE_FROM_CPU_DATA_26     equ    ( (0*8192) + 468h )
IMAGE_FROM_CPU_DATA_27     equ    ( (0*8192) + 46ch )
IMAGE_FROM_CPU_DATA_28     equ    ( (0*8192) + 470h )
IMAGE_FROM_CPU_DATA_29     equ    ( (0*8192) + 474h )
IMAGE_FROM_CPU_DATA_30     equ    ( (0*8192) + 478h )
IMAGE_FROM_CPU_DATA_31     equ    ( (0*8192) + 47ch )

        .code

;---------------------------Public-Routine------------------------------;
;
;-----------------------------------------------------------------------;

        public NV1_Transfer_MemToScreen_Data@24


NV1_Transfer_MemToScreen_Data@24 proc near

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

        sub     esp,(size NV1_TRANSFER_BLITDATA_STRUC) - NV1_TRANSFER_BLITDATA_PARM_SIZE  ; make room for local variables

        ;**********************************************************************
        ; Register Usage:
        ;   ebp = Ptr to NV User Channel
        ;   eax = Dwords remaining in current scanline
        ;   ebx = Current scanline (reverse index)
        ;   ecx = Used by Rep Mov string copy
        ;   edx = Current Free Count
        ;   esi = Ptr to Image data
        ;   edi = Ptr to data array in user channel
        ;**********************************************************************

        mov     ebp,[esp].NV1TBDS_NvChannelPtr      ; Get pointer to NV channel
        add     ebp,IMAGE_FROM_CPU_DATA_0           ; EDI ptr to data array in user channel
        mov     ebx,[esp].NV1TBDS_Height            ; Number of scanlines to output
        mov     edx,[esp].NV1TBDS_FreeCountPtr      ; Ptr to current cached free count
        movzx   edx,word ptr [edx]                  ; Get current free count
        cld                                         ; Default to going forward

        ;**********************************************************************
        ; Make sure there are scanlines to output
        ;**********************************************************************

        cmp     ebx,0                               ; Make sure we have data to output
        jle     NV1TBD_Done                         ; Otherwise just quit

        ;**********************************************************************
        ; Main loop
        ;**********************************************************************


NV1TBD_Scanline_Loop:

        mov     esi,[esp].NV1TBDS_NextScanPtr       ; Ptr to image data
        mov     eax,[esp].NV1TBDS_BytesPerDstScan   ; Number of DWORDS per scanline

        ;**********************************************************************
        ;
        ; Output 31 Dwords at a time
        ;
        ;**********************************************************************

NV1TBD_Check_For_31Dwords:

        cmp     eax,31*4                            ; Q: Any more Dwords to output?
        jb      short NV1TBD_Check_For_16Dwords     ;  N: Less than 31 dwords remaining

NV1TBD_31Dwords_Chk_FreeCount:

        cmp     edx,31*4                            ; Q: Enough room to output 31 dwords?
        jae     short NV1TBD_31Dwords_UpdateFreeCount;  Y: Send the data

NV1TBD_31Dwords_WaitForEngine:

        movzx   edx,Word Ptr [ebp+IMAGE_FROM_CPU_FREECOUNT-IMAGE_FROM_CPU_DATA_0] ; Get freecount from hardware
        jmp     short NV1TBD_31Dwords_Chk_FreeCount ; Check if FIFO is ready yet

NV1TBD_31Dwords_UpdateFreeCount:

        sub     edx,31*4                            ; Update cached freecount

NV1TBD_31Dwords_SendData:

        mov     ecx,31                              ; 16 dwords
        mov     edi,ebp                             ; ESI pts to source image data
        rep     movsd                               ; Copy 16 dwords
        sub     eax,31*4                            ; Decrement number of bytes we've done
        jmp     short NV1TBD_Check_For_31Dwords     ; Loop thru remaining data



        ;**********************************************************************
        ;
        ; Output 16 Dwords at a time
        ;
        ;**********************************************************************

NV1TBD_Check_For_16Dwords:

        cmp     eax,16*4                            ; Q: Any more Dwords to output?
        jb      short NV1TBD_Check_For_8Dwords      ;  N: Less than 16 dwords remaining

NV1TBD_16Dwords_Chk_FreeCount:

        cmp     edx,16*4                            ; Q: Enough room to output 16 dwords?
        jae     short NV1TBD_16Dwords_UpdateFreeCount;  Y: Send the data

NV1TBD_16Dwords_WaitForEngine:

        movzx   edx,Word Ptr [ebp+IMAGE_FROM_CPU_FREECOUNT-IMAGE_FROM_CPU_DATA_0] ; Get freecount from hardware
        jmp     short NV1TBD_16Dwords_Chk_FreeCount ; Check if FIFO is ready yet

NV1TBD_16Dwords_UpdateFreeCount:

        sub     edx,16*4                            ; Update cached freecount

NV1TBD_16Dwords_SendData:

        mov     ecx,16                              ; 16 dwords
        mov     edi,ebp                             ; ESI pts to source image data
        rep     movsd                               ; Copy 16 dwords
        sub     eax,16*4                            ; Decrement number of bytes we've done
        jmp     short NV1TBD_Check_For_16Dwords     ; Loop thru remaining data


        ;**********************************************************************
        ;
        ; Output 8 Dwords at a time
        ;
        ;**********************************************************************

NV1TBD_Check_For_8Dwords:

        cmp     eax,8*4                             ; Q: Any more Dwords to output?
        jb      short NV1TBD_Check_For_4Dwords      ;  N: Less than 8 dwords remaining

NV1TBD_8Dwords_Chk_FreeCount:

        cmp     edx,8*4                             ; Q: Enough room to output 8 dwords?
        jae     short NV1TBD_8Dwords_UpdateFreeCount;  Y: Send the data

NV1TBD_8Dwords_WaitForEngine:

        movzx   edx,Word Ptr [ebp+IMAGE_FROM_CPU_FREECOUNT-IMAGE_FROM_CPU_DATA_0] ; Get freecount from hardware
        jmp     short NV1TBD_8Dwords_Chk_FreeCount  ; Check if FIFO is ready yet

NV1TBD_8Dwords_UpdateFreeCount:

        sub     edx,8*4                             ; Update cached freecount

NV1TBD_8Dwords_SendData:

        mov     ecx,8                               ; 8 dwords
        mov     edi,ebp                             ; ESI pts to source image data
        rep     movsd                               ; Copy 8 dwords
        sub     eax,8*4                             ; Decrement number of bytes we've done
        jmp     short NV1TBD_Check_for_8Dwords      ; Loop thr remaining data


        ;**********************************************************************
        ;
        ; Output 4 Dwords at a time
        ;
        ;**********************************************************************

NV1TBD_Check_For_4Dwords:

        cmp     eax,4*4                             ; Q: Any more Dwords to output?
        jae     short NV1TBD_4Dwords_Chk_FreeCount  ;  Y: Output remaining dwords
        jmp     short NV1TBD_Check_For_1Dwords      ;  N: Less than 4 dwords remaining

NV1TBD_4Dwords_Chk_FreeCount:

        cmp     edx,4*4                             ; Q: Enough room to output 4 dwords?
        jae     short NV1TBD_4Dwords_UpdateFreeCount;  Y: Send the data

NV1TBD_4Dwords_WaitForEngine:

        movzx   edx,Word Ptr [ebp+IMAGE_FROM_CPU_FREECOUNT-IMAGE_FROM_CPU_DATA_0] ; Get freecount from hardware
        jmp     short NV1TBD_4Dwords_Chk_FreeCount  ; Check if FIFO is ready yet

NV1TBD_4Dwords_UpdateFreeCount:

        sub     edx,4*4                             ; Update cached freecount

NV1TBD_4Dwords_SendData:

        mov     ecx,4                               ; 4 dwords
        mov     edi,ebp                             ; ESI pts to source image data
        rep     movsd                               ; Copy 4 dwords
        sub     eax,4*4                             ; Decrement number of bytes we've done
        jmp     short NV1TBD_Check_for_4Dwords      ; Loop thr remaining data

        ;**********************************************************************
        ;
        ; Output 1 Dword at a time
        ;
        ;**********************************************************************


NV1TBD_Check_For_1Dwords:

        cmp     eax,4                               ; Q: Any more Dwords to output?
        jb      short NV1TBD_Check_For_Pad          ;  N: Check for pad bytes

NV1TBD_1Dwords_Chk_FreeCount:

        cmp     edx,4*3                             ; Q: Assume worst case 3 dwords
        jae     short NV1TBD_1Dwords_UpdateFreeCount;  Y: Send the data

NV1TBD_1Dwords_WaitForEngine:

        movzx   edx,Word Ptr [ebp+IMAGE_FROM_CPU_FREECOUNT-IMAGE_FROM_CPU_DATA_0] ; Get freecount from hardware
        jmp     short NV1TBD_1Dwords_Chk_FreeCount  ; Check if FIFO is ready yet

NV1TBD_1Dwords_UpdateFreeCount:
        mov     ecx,eax                             ; Get number of bytes
        shr     ecx,2                               ; div 4 = # dwords
        shl     ecx,2                               ; Convert to div4 num bytes
        sub     edx,ecx                             ; Update cached freecount, ASSUME
        sub     eax,ecx
        shr     ecx,2                               ; Back to num dwords

NV1TBD_1Dwords_SendData:

        mov     edi,ebp                             ; EDI ptr to data array in user channel
        rep     movsd                               ; Copy remaining dwords

NV1TBD_Check_For_Pad:
        or      eax,eax                             ; Any bytes left?
        jbe     NV1TBD_Advance_Scanline             ; N, go to next scan

NV1TBD_Pad_Bytes_Chk_FreeCount:

        cmp     edx,1*4                             ; Q: Assume worst case 1 dword
        jae     short NV1TBD_Pad_Bytes_UpdateFreeCount;  Y: Send the data

NV1TBD_Pad_Bytes_WaitForEngine:

        movzx   edx,Word Ptr [ebp+IMAGE_FROM_CPU_FREECOUNT-IMAGE_FROM_CPU_DATA_0] ; Get freecount from hardware
        jmp     short NV1TBD_Pad_Bytes_Chk_FreeCount  ; Check if FIFO is ready yet

NV1TBD_Pad_Bytes_UpdateFreeCount:

        sub     edx,1*4                             ; Update cached freecount, ASSUME
                                                    ; WORST CASE 1 dword sent

NV1TBD_Pad_Bytes_SendData:

        mov     ecx,eax                             ; Finish off remaining bytes

NV1TBD_Pad_Bytes_Get_Next:
        lodsb
        dec     ecx
        or      ecx,ecx                             ; Done yet?
        jz      NV1TBD_Send_Pad_Bytes
        xchg    ah,al                               ; Save last byte
        lodsb                                       ; Get next byte
        xchg    ah,al                               ; Restore byte order
        dec     ecx
        or      ecx,ecx                             ; Done yet?
        jz      NV1TBD_Send_Pad_Bytes
        mov     cl,byte ptr [esi]
        shl     ecx,16
        or      eax,ecx                             ; EAX = pad byte(s)

NV1TBD_Send_Pad_Bytes:
        mov     edi,ebp                             ; EDI ptr to data array in user channel
        stosd                                       ; Copy pad bytes

        ;**********************************************************************
        ; Advance to next scanline
        ;**********************************************************************

NV1TBD_Advance_Scanline:

        dec     ebx                                 ; Next scanline
        jz      short NV1TBD_Done                   ; Check if we're done
        mov     eax,[esp].NV1TBDS_ScanInc           ; Get number of bytes to advance src
        shl     eax,2                               ; Convert dwords to bytes
        add     [esp].NV1TBDS_NextScanPtr,eax       ; Next src scanline
        jmp     NV1TBD_Scanline_Loop                ; Loop thru entire image

        ;**********************************************************************
        ; All done, restore stack and return to caller
        ;**********************************************************************

NV1TBD_Done:

        mov     eax,[esp].NV1TBDS_FreeCountPtr      ; Make sure to update
        mov     word ptr [eax],dx                   ;   cached free count

        add     esp,(size NV1_TRANSFER_BLITDATA_STRUC) - NV1_TRANSFER_BLITDATA_PARM_SIZE
        pop     ebp
        pop     esi
        pop     edi
        pop     ebx

        ret     24

NV1_Transfer_MemToScreen_Data@24 endp



end



