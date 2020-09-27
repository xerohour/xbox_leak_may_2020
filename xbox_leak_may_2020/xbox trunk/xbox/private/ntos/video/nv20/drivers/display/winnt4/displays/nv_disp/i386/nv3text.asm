;******************************* Module Header *********************************
;
; Module Name: NV3TEXT.ASM
;
; Contains the x86 'Asm' versions of some inner-loop routines for the
; partially hardware accelerated text blt.
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

NV3_TRANSFER_TEXT_STRUC          struc

; Feel free to add any local variables here:

NV3TS_RightCase            dd      ?
NV3TS_pjSrcScan            dd      ?
NV3TS_SrcIntStep           dd      ?
NV3TS_DstStride            dd      ?
NV3TS_XCount               dd      ?
NV3TS_yDst                 dd      ?
NV3TS_pdev                 dd      ?

; Don't add any fields below here without modifying XXX_STRUC_SIZE!

NV3TS_ebp                  dd      ?
NV3TS_esi                  dd      ?
NV3TS_edi                  dd      ?
NV3TS_ebx                  dd      ?
NV3TS_RetAddr              dd      ?
NV3TS_NumDwordsPerGlyph    dd      ?
NV3TS_FreeCountPtr         dd      ?
NV3TS_GlyphDataPtr         dd      ?
NV3TS_NvChannelPtr         dd      ?

NV3_TRANSFER_TEXT_STRUC          ends

NV3_TRANSFER_TEXT_PARM_SIZE     equ     9 * 4

;*******************************************************************************
; Defines for NV Hardware
; Currently, channels are used as follows:
;   IMAGE_FROM_CPU          Channel 0
;   BLIT                    Channel 1
;   IMAGE_MONO_FROM_CPU     Channel 2
;   RECT_AND_TEXT           Channel 2
;   RECTANGLE               Channel 3
;   ROP_SOLID               Channel 4
;   IMAGE_PATTERN           Channel 5
;   CLIP                    Channel 6
;   IMAGE_SOLID             Channel 7
;*******************************************************************************

IMAGE_FROM_CPU_FREECOUNT           equ    ( (0*8192) + 16 )
BLIT_FREECOUNT                     equ    ( (1*8192) + 16 )
IMAGE_MONO_FROM_CPU_FREECOUNT      equ    ( (2*8192) + 16 )
RECT_AND_TEXT_FREECOUNT            equ    ( (2*8192) + 16 )
RECTANGLE_FREECOUNT                equ    ( (3*8192) + 16 )
ROP_SOLID_FREECOUNT                equ    ( (4*8192) + 16 )
IMAGE_PATTERN_FREECOUNT            equ    ( (5*8192) + 16 )
CLIP_FREECOUNT                     equ    ( (6*8192) + 16 )
IMAGE_SOLID_FREECOUNT              equ    ( (7*8192) + 16 )

RECT_AND_TEXT_MONODATA_0      equ    ( (2*8192) + 1000h )
RECT_AND_TEXT_MONODATA_1      equ    ( (2*8192) + 1004h )
RECT_AND_TEXT_MONODATA_2      equ    ( (2*8192) + 1008h )
RECT_AND_TEXT_MONODATA_3      equ    ( (2*8192) + 100ch )
RECT_AND_TEXT_MONODATA_4      equ    ( (2*8192) + 1010h )
RECT_AND_TEXT_MONODATA_5      equ    ( (2*8192) + 1014h )
RECT_AND_TEXT_MONODATA_6      equ    ( (2*8192) + 1018h )
RECT_AND_TEXT_MONODATA_7      equ    ( (2*8192) + 101ch )
RECT_AND_TEXT_MONODATA_8      equ    ( (2*8192) + 1020h )
RECT_AND_TEXT_MONODATA_9      equ    ( (2*8192) + 1024h )
RECT_AND_TEXT_MONODATA_10     equ    ( (2*8192) + 1028h )
RECT_AND_TEXT_MONODATA_11     equ    ( (2*8192) + 102ch )
RECT_AND_TEXT_MONODATA_12     equ    ( (2*8192) + 1030h )
RECT_AND_TEXT_MONODATA_13     equ    ( (2*8192) + 1034h )
RECT_AND_TEXT_MONODATA_14     equ    ( (2*8192) + 1038h )
RECT_AND_TEXT_MONODATA_15     equ    ( (2*8192) + 103ch )

        .code

;---------------------------Public-Routine------------------------------;
;
;-----------------------------------------------------------------------;

        public NV3_Transfer_Text_Data@16

NV3_Transfer_Text_Data@16 proc near

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

        sub     esp,(size NV3_TRANSFER_TEXT_STRUC) - NV3_TRANSFER_TEXT_PARM_SIZE  ; make room for local variables

        ;**********************************************************************
        ; Register Usage:
        ;   ebx = Number of dwords remaining
        ;   ecx = Current rep mov index
        ;   edx = Current Free Count
        ;   esi = Ptr to Glyph Image data
        ;   edi = Ptr to current user channel location
        ;   ebp = Ptr to NV User Channel
        ;**********************************************************************

        mov     ebp,[esp].NV3TS_NvChannelPtr        ; Get pointer to NV channel
        add     ebp,RECT_AND_TEXT_MONODATA_0        ; EDI ptr to data array in user channel
        mov     edi,ebp                             ; EDI pts to user channel location
        mov     ebx,[esp].NV3TS_NumDwordsPerGlyph   ; Number of DWORDS to output
        mov     esi,[esp].NV3TS_GlyphDataPtr        ; Ptr to GLYPH Image data
        mov     edx,[esp].NV3TS_FreeCountPtr        ; Ptr to current cached free count
        movzx   edx,word ptr [edx]                  ; Get current free count

        ;**********************************************************************
        ;
        ; Output 4 Dwords at a time
        ;
        ;**********************************************************************

NV3TTD_Check_For_4Dwords:

        cmp     ebx,4                               ; Q: Any more Dwords to output?
        jb      short NV3TTD_Check_For_1Dwords      ;  N: Less than 4 dwords remaining

NV3TTD_4Dwords_Chk_FreeCount:

        cmp     edx,4*4                             ; Q: Enough room to output 4 dwords?
        jae     short NV3TTD_4Dwords_UpdateFreeCount;  Y: Send the data

NV3TTD_4Dwords_WaitForEngine:

        movzx   edx,Word Ptr [ebp+RECT_AND_TEXT_FREECOUNT-RECT_AND_TEXT_MONODATA_0] ; Get freecount from hardware
        jmp     short NV3TTD_4Dwords_Chk_FreeCount  ; Check if FIFO is ready yet

NV3TTD_4Dwords_UpdateFreeCount:

        sub     edx,4*4                             ; Update cached freecount

NV3TTD_4Dwords_SendData:

        mov     eax,[esi]
        mov     [edi],eax
        mov     eax,[esi+4]
        mov     [edi],eax
        mov     eax,[esi+8]
        mov     [edi],eax
        mov     eax,[esi+12]
        mov     [edi],eax
        sub     ebx,4
        add     esi,16
        jmp     short NV3TTD_Check_For_4Dwords      ; Loop thr remaining data

        ;**********************************************************************
        ;
        ; Output 1 thru 3 Dwords at a time
        ;
        ;**********************************************************************

NV3TTD_Check_For_1Dwords:

        cmp     ebx,0                               ; Q: Any more Dwords to output?
        jbe     short NV3TTD_Done                   ;  N: All done, exit

NV3TTD_1Dwords_Chk_FreeCount:

        cmp     edx,1*4                             ; Q: Enough room to output 1 dword?
        jae     short NV3TTD_1Dwords_UpdateFreeCount;  Y: Send the data

NV3TTD_1Dwords_WaitForEngine:

        movzx   edx,Word Ptr [ebp+RECT_AND_TEXT_FREECOUNT-RECT_AND_TEXT_MONODATA_0] ; Get freecount from hardware
        jmp     short NV3TTD_1Dwords_Chk_FreeCount  ; Check if FIFO is ready yet

NV3TTD_1Dwords_UpdateFreeCount:

        sub     edx,1*4                             ; Update cached freecount

NV3TTD_1Dwords_SendData:

        mov     eax,[esi]                           ; Get source dword
        mov     [ebp],eax                           ; Output dword to monochrome array
        dec     ebx                                 ; Next dword
        jz      short NV3TTD_Done                   ; Check if we're done
        add     esi,1*4                             ; Next dword of glyph data
        jmp     short NV3TTD_1Dwords_Chk_FreeCount  ; Continue with remaining dwords

        ;**********************************************************************
        ; All done, restore stack and return to caller
        ;**********************************************************************

NV3TTD_Done:

        mov     eax,[esp].NV3TS_FreeCountPtr        ; Make sure to update
        mov     word ptr [eax],dx                   ;   cached free count

        add     esp,(size NV3_TRANSFER_TEXT_STRUC) - NV3_TRANSFER_TEXT_PARM_SIZE
        pop     ebp
        pop     esi
        pop     edi
        pop     ebx

        ret     16

NV3_Transfer_Text_Data@16 endp




end



