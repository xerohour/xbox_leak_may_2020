;******************************* Module Header *********************************
;
; Module Name: NV3CUR.ASM
;
; Contains the x86 'Asm' versions of some inner-loop routines for the hardware cursor
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

;**********************************************************************
; Tables are used to convert MONO mask. Ones convert to 7fffh and zeros to 0
; The AND mask is used to add transparency bit. Zero in AND mask converts
; to 8000h, 1 to zero.
;**********************************************************************


MonoTable   dd  0                       ; MONO 00 
            dd  7fff0000h               ; MONO 01 (bits need to be transposed)
            dd  00007fffh               ; MONO 10 (bits need to be transposed)
            dd  7fff7fffh               ; MONO 11
            
AndTable    dd  80008000h               ; MONO 00
            dd  00008000h               ; MONO 01 (bits need to be transposed)
            dd  80000000h               ; MONO 10 (bits need to be transposed)
            dd  0                       ; MONO 11

;
;  stack based params and local variables
;

NV3_LOAD_CURSOR_DATA_STRUC          struc

; Feel free to add any local variables here:

NV3LCD_Row_Count            dd      ?
NV3LCD_Byte_Count           dd      ?

; Don't add any fields below here without modifying XXX_STRUC_SIZE!

NV3LCD_ebp                  dd      ?
NV3LCD_esi                  dd      ?
NV3LCD_edi                  dd      ?
NV3LCD_ebx                  dd      ?
NV3LCD_RetAddr              dd      ?
NV3LCD_SrcPtr               dd      ?
NV3LCD_DestPtr              dd      ?

NV3_LOAD_CURSOR_DATA_STRUC          ends

NV3_LOAD_CURSOR_DATA_PARM_SIZE     equ     7 * 4

; --------

NV3_CURSOR_CHECKSUM_STRUC          struc

; Feel free to add any local variables here:

; Don't add any fields below here without modifying XXX_STRUC_SIZE!

NV3CHK_ebx                  dd      ?
NV3CHK_ebp                  dd      ?
NV3CHK_esi                  dd      ?
NV3CHK_RetAddr              dd      ?
NV3CHK_SrcPtr               dd      ?

NV3_CURSOR_CHECKSUM_STRUC          ends

NV3_CURSOR_CHECKSUM_PARM_SIZE     equ     5 * 4



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


IMAGE_MONO_FROM_CPU_MONODATA_0      equ    ( (2*8192) + 400h )
IMAGE_MONO_FROM_CPU_MONODATA_1      equ    ( (2*8192) + 404h )
IMAGE_MONO_FROM_CPU_MONODATA_2      equ    ( (2*8192) + 408h )
IMAGE_MONO_FROM_CPU_MONODATA_3      equ    ( (2*8192) + 40ch )
IMAGE_MONO_FROM_CPU_MONODATA_4      equ    ( (2*8192) + 410h )
IMAGE_MONO_FROM_CPU_MONODATA_5      equ    ( (2*8192) + 414h )
IMAGE_MONO_FROM_CPU_MONODATA_6      equ    ( (2*8192) + 418h )
IMAGE_MONO_FROM_CPU_MONODATA_7      equ    ( (2*8192) + 41ch )
IMAGE_MONO_FROM_CPU_MONODATA_8      equ    ( (2*8192) + 420h )
IMAGE_MONO_FROM_CPU_MONODATA_9      equ    ( (2*8192) + 424h )
IMAGE_MONO_FROM_CPU_MONODATA_10     equ    ( (2*8192) + 428h )
IMAGE_MONO_FROM_CPU_MONODATA_11     equ    ( (2*8192) + 42ch )
IMAGE_MONO_FROM_CPU_MONODATA_12     equ    ( (2*8192) + 430h )
IMAGE_MONO_FROM_CPU_MONODATA_13     equ    ( (2*8192) + 434h )
IMAGE_MONO_FROM_CPU_MONODATA_14     equ    ( (2*8192) + 438h )
IMAGE_MONO_FROM_CPU_MONODATA_15     equ    ( (2*8192) + 43ch )

        .code

;---------------------------Public-Routine------------------------------;
;
;-----------------------------------------------------------------------;

        public NV3LoadCursorData@8
        
NV3LoadCursorData@8 proc near

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

        sub     esp,(size NV3_LOAD_CURSOR_DATA_STRUC) - NV3_LOAD_CURSOR_DATA_PARM_SIZE  ; make room for local variables

        ;**********************************************************************
        ; Register Usage:
        ;   eax = Current 16bpp pel
        ;   edx = BitMask
        ;   ebx = Current dword from AND mask
        ;   ecx = Current dword from MONO mask
        ;   esi = Ptr to Source Image data
        ;   ebp = Ptr to Mono Image data        
        ;   edi = Ptr to Destination Data
        ;**********************************************************************
    
        mov     esi,[esp].NV3LCD_SrcPtr             ; Ptr to AND mask
        mov     ebp,esi                             ; Get ptr to MONO mask
        add     ebp,128                             ;  (128 bytes from beginning)
        mov     edi,[esp].NV3LCD_DestPtr            ; Ptr to current destination

        ;**********************************************************************
        ; 
        ; For Each row
        ;   For Each Dword
        ;      For Each Bit   
        ;        Store 16bpp pel corresponding to the bits in
        ;        the AND mask and MONO mask
        ;
        ;**********************************************************************

NV3LCD_Init_Variables:

        mov     [esp].NV3LCD_Row_Count,32           ; We'll be parsing 32 rows
        xor     edx,edx

NV3LCD_Get_Next_Dword:
        mov     [esp].NV3LCD_Byte_Count,4           ; 8 pels (4 inner loops) per byte
        mov     ebx,[esi]                           ; Get 1st dword of AND mask data
        mov     ecx,[ebp]                           ; Get 1st dword of MONO mask data
        
        ;**********************************************************************
        ; Load 16bpp pels - uses tables to convert ones to 7fffh and zeros to 0
        ; Also uses table to add transparency bit from AND mask. Zero in AND mask
        ; converts to 8000h, 1 to zero.
        ;**********************************************************************
                
NV3LCD_Get_Next_Byte:

        rol     cl,2                                ; first 2 pels
        mov     dl,cl
        and     dl,3
        rol     bl,2
        mov     eax,MonoTable[edx*4]
        mov     dl,bl
        and     dl,3
        rol     cl,2                                ; next 2 pels
        or      eax,AndTable[edx*4]
        
        mov     [edi],eax
        add     edi,4                               ; Store the 2 16bpp pels

        mov     dl,cl
        and     dl,3
        rol     bl,2
        mov     eax,MonoTable[edx*4]
        mov     dl,bl
        and     dl,3
        rol     cl,2                                ; next 2 pels
        or      eax,AndTable[edx*4]

        mov     [edi],eax
        add     edi,4                               ; Store the 2 16bpp pels

        mov     dl,cl
        and     dl,3
        rol     bl,2
        mov     eax,MonoTable[edx*4]
        mov     dl,bl
        and     dl,3
        rol     cl,2                                ; next 2 pels
        or      eax,AndTable[edx*4]

        mov     [edi],eax
        add     edi,4                               ; Store the 2 16bpp pels

        mov     dl,cl
        and     dl,3
        rol     bl,2
        mov     eax,MonoTable[edx*4]
        mov     dl,bl
        and     dl,3
        
        shr     ebx,8                               ; next byte of AND mask
        shr     ecx,8                               ; next byte of MONO mask
        or      eax,AndTable[edx*4]

        mov     [edi],eax
        add     edi,4                               ; Store the 2 16bpp pels

        dec     [esp].NV3LCD_Byte_Count             ; Continue with next 2 pels
        jnz     NV3LCD_Get_Next_Byte                ; Continue parsing this byte

        ;**********************************************************************
        ; We need to get the next dwords for the masks
        ;**********************************************************************

NV3LCD_Check_Next_Dword:

        add     esi,4                               ; Next AND mask dword
        add     ebp,4                               ; Next MONO mask dword

        ;**********************************************************************
        ; We've read 1 row (1 dword's worth of source data)   
        ;**********************************************************************

NV3LCD_Check_Row_Count:

        dec     [esp].NV3LCD_Row_Count              ; Check the Row count
        jnz     NV3LCD_Get_Next_Dword               ; Continue parsing the bits

        ;**********************************************************************
        ; All done, restore stack and return to caller
        ;**********************************************************************

NV3LCD_Done:

        add     esp,(size NV3_LOAD_CURSOR_DATA_STRUC) - NV3_LOAD_CURSOR_DATA_PARM_SIZE
        pop     ebp
        pop     esi
        pop     edi
        pop     ebx

        ret     8

NV3LoadCursorData@8 endp


;---------------------------Public-Routine------------------------------;
;
;-----------------------------------------------------------------------;

        public NV3Load16x16CursorData@8
        
NV3Load16x16CursorData@8 proc near

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

        sub     esp,(size NV3_LOAD_CURSOR_DATA_STRUC) - NV3_LOAD_CURSOR_DATA_PARM_SIZE  ; make room for local variables

        ;**********************************************************************
        ; Register Usage:
        ;   eax = Current 16bpp pel
        ;   edx = BitMask
        ;   ebx = Current dword from AND mask
        ;   ecx = Current dword from MONO mask
        ;   esi = Ptr to Source Image data
        ;   ebp = Ptr to Mono Image data        
        ;   edi = Ptr to Destination Data
        ;**********************************************************************
    
        mov     esi,[esp].NV3LCD_SrcPtr             ; Ptr to AND mask
        mov     ebp,esi                             ; Get ptr to MONO mask
        add     ebp,64                              ;  (64 bytes from beginning)
        mov     edi,[esp].NV3LCD_DestPtr            ; Ptr to current destination

        ;**********************************************************************
        ; 
        ; For Each row
        ;   For Each Dword
        ;      For Each Bit   
        ;        Store 16bpp pel corresponding to the bits in
        ;        the AND mask and MONO mask
        ;
        ;**********************************************************************

NV3_16x16_LCD_Init_Variables:

        mov     [esp].NV3LCD_Row_Count,16           ; We'll be parsing 16 rows
        xor     edx,edx

NV3_16x16_LCD_Get_Next_Dword:
        mov     [esp].NV3LCD_Byte_Count,2           ; 4 pels (4 inner loops) per byte
        mov     ebx,[esi]                           ; Get 1st word of AND mask data
        mov     ecx,[ebp]                           ; Get 1st word of MONO mask data
        
        ;**********************************************************************
        ; Load 16bpp pels - uses tables to convert ones to 7fffh and zeros to 0
        ; Also uses table to add transparency bit from AND mask. Zero in AND mask
        ; converts to 8000h, 1 to zero.
        ;**********************************************************************
                
NV3_16x16_LCD_Get_Next_Byte:

        rol     cl,2                                ; first 2 pels
        mov     dl,cl
        and     dl,3
        rol     bl,2
        mov     eax,MonoTable[edx*4]
        mov     dl,bl
        and     dl,3
        rol     cl,2                                ; next 2 pels
        or      eax,AndTable[edx*4]
        
        mov     [edi],eax
        add     edi,4                               ; Store the 2 16bpp pels

        mov     dl,cl
        and     dl,3
        rol     bl,2
        mov     eax,MonoTable[edx*4]
        mov     dl,bl
        and     dl,3
        rol     cl,2                                ; next 2 pels
        or      eax,AndTable[edx*4]

        mov     [edi],eax
        add     edi,4                               ; Store the 2 16bpp pels

        mov     dl,cl
        and     dl,3
        rol     bl,2
        mov     eax,MonoTable[edx*4]
        mov     dl,bl
        and     dl,3
        rol     cl,2                                ; next 2 pels
        or      eax,AndTable[edx*4]

        mov     [edi],eax
        add     edi,4                               ; Store the 2 16bpp pels

        mov     dl,cl
        and     dl,3
        rol     bl,2
        mov     eax,MonoTable[edx*4]
        mov     dl,bl
        and     dl,3
        
        shr     ebx,8                               ; next byte of AND mask
        shr     ecx,8                               ; next byte of MONO mask
        or      eax,AndTable[edx*4]

        mov     [edi],eax
        add     edi,4                               ; Store the 2 16bpp pels

        dec     [esp].NV3LCD_Byte_Count             ; Continue with next 2 pels
        jnz     NV3_16x16_LCD_Get_Next_Byte         ; Continue parsing this byte

        ;**********************************************************************
        ; Blank out remaining 'width' portion of 32x32 cursor  (clear 16 pels)
        ;**********************************************************************

        mov     [edi],00000000h                     ; 
        add     edi,4                               ; Store the 2 16bpp pels
        mov     [edi],00000000h                     ; 
        add     edi,4                               ; Store the 2 16bpp pels
        mov     [edi],00000000h                     ; 
        add     edi,4                               ; Store the 2 16bpp pels
        mov     [edi],00000000h                     ; 
        add     edi,4                               ; Store the 2 16bpp pels
        mov     [edi],00000000h                     ; 
        add     edi,4                               ; Store the 2 16bpp pels
        mov     [edi],00000000h                     ; 
        add     edi,4                               ; Store the 2 16bpp pels
        mov     [edi],00000000h                     ; 
        add     edi,4                               ; Store the 2 16bpp pels
        mov     [edi],00000000h                     ; 
        add     edi,4                               ; Store the 2 16bpp pels
        
        ;**********************************************************************
        ; We need to get the next words for the masks
        ;**********************************************************************

NV3_16x16_LCD_Check_Next_Dword:

        add     esi,4                               ; Next AND mask word
        add     ebp,4                               ; Next MONO mask word

        ;**********************************************************************
        ; We've read 1 row (1 dword's worth of source data)   
        ;**********************************************************************

NV3_16x16_LCD_Check_Row_Count:

        dec     [esp].NV3LCD_Row_Count              ; Check the Row count
        jnz     NV3_16x16_LCD_Get_Next_Dword        ; Continue parsing the bits

        ;**********************************************************************
        ; Blank out remaining 'height' portion of 32x32 cursor (clear out 16 lines * 32 pels = 512 pels = 256 dwords)
        ;**********************************************************************

        mov     [esp].NV3LCD_Row_Count,256           
NV3_16x16_Continue_Clearing:
        mov     [edi],00000000h                     ; 
        add     edi,4                               ; Store the 2 16bpp pels
        dec     [esp].NV3LCD_Row_Count              ; Check the Row count
        jnz     NV3_16x16_Continue_Clearing         ; Continue clearing out cursor


        ;**********************************************************************
        ; All done, restore stack and return to caller
        ;**********************************************************************

NV3_16x16_LCD_Done:

        add     esp,(size NV3_LOAD_CURSOR_DATA_STRUC) - NV3_LOAD_CURSOR_DATA_PARM_SIZE
        pop     ebp
        pop     esi
        pop     edi
        pop     ebx

        ret     8

NV3Load16x16CursorData@8 endp



;---------------------------Public-Routine------------------------------;
;
;-----------------------------------------------------------------------;

        public NV3CursorChecksum@4
        
NV3CursorChecksum@4 proc near

        ;**********************************************************************
        ;
        ; use ebp as general register, use esp for parameter and local access
        ; save ebp,ebx,esi,edi
        ;
        ;**********************************************************************

        push    esi
        push    ebp
        push    ebx
        
        sub     esp,(size NV3_CURSOR_CHECKSUM_STRUC) - NV3_CURSOR_CHECKSUM_PARM_SIZE  ; make room for local variables

        ;**********************************************************************
        ; Register Usage:
        ;   esi = Ptr to Source Image data
        ;   ecx = number of dwords
        ;   eax = top checksum
        ;   ebx = bottom checksum
        ;
        ; NOTE: Calculating a simplified checksum for the entire image
        ;       is NOT sufficient, because we get the same checksum value
        ;       if 2 images are horizontal mirror images of each other.
        ;       To avoid this case, we calculate a checksum for the top
        ;       and bottom half. If they are the same, then we need to
        ;       return a flag value that we can't distinguish this particular
        ;       image (i.e. we'll reload the cursor image completely)
        ;**********************************************************************
    
        mov     esi,[esp].NV3CHK_SrcPtr             ; Ptr to cursor masks
        xor     eax,eax                             ; Clear top checksum value
        xor     ebx,ebx                             ; Clear bottom checksum value

        ;**********************************************************************
        ; Calculate checksum for 1st half of AND mask
        ;**********************************************************************
        
        mov     ecx,8                               ; Top half of mask 
NV3CHK_CheckSumLoop1:

        mov     edx,[esi]                           ; Get current dword
        add     edx,ecx
        rol     edx,cl
        add     eax,edx
        mov     edx,[esi+4]                         ; Get current dword
        add     edx,ecx
        rol     edx,cl
        add     ebx,edx
        add     esi,8                               ; Next dword
        dec     ecx
        jnz     short NV3CHK_CheckSumLoop1          ; Parse dwords at a time

        ;**********************************************************************
        ; Calculate checksum for 2nd half of AND mask
        ;**********************************************************************
        
        mov     ecx,8                               ; Bottom half of mask 
NV3CHK_CheckSumLoop2:

        mov     edx,[esi]                           ; Get current dword
        add     edx,ecx
        rol     edx,cl
        add     ebx,edx
        mov     edx,[esi+4]                         ; Get current dword
        add     edx,ecx
        rol     edx,cl
        add     eax,edx
        add     esi,8                               ; Next dword
        dec     ecx
        jnz     short NV3CHK_CheckSumLoop2          ; Parse dwords at a time
        

        ;**********************************************************************
        ; Calculate checksum for 1st half of MONO mask
        ;**********************************************************************
        
        mov     ecx,8                               ; Top half of mask 
NV3CHK_CheckSumLoop3:

        mov     edx,[esi]                           ; Get current dword
        add     edx,ecx
        rol     edx,cl
        add     eax,edx
        mov     edx,[esi+4]                         ; Get current dword
        add     edx,ecx
        rol     edx,cl
        add     ebx,edx
        add     esi,8                               ; Next dword
        dec     ecx
        jnz     short NV3CHK_CheckSumLoop3          ; Parse dwords at a time
        

        ;**********************************************************************
        ; Calculate checksum for 2nd half of MONO mask
        ;**********************************************************************
        
        mov     ecx,8                               ; Bottom half of mask 
NV3CHK_CheckSumLoop4:

        mov     edx,[esi]                           ; Get current dword
        add     edx,ecx
        rol     edx,cl
        add     ebx,edx
        mov     edx,[esi+4]                         ; Get current dword
        add     edx,ecx
        rol     edx,cl
        add     eax,edx
        add     esi,8                               ; Next dword
        dec     ecx
        jnz     short NV3CHK_CheckSumLoop4          ; Parse dwords at a time
        
        ;**********************************************************************
        ; Check if we've got a mirror image.
        ; If so, then we return a flag value telling the calle to 
        ; reload the cursor image
        ;**********************************************************************

        cmp     eax,ebx                             ; Mirror image?
        jne     short NV3CHK_Done                   ; N: Just return
        mov     eax, 12345678h                      ; Return a flag value
                
        ;**********************************************************************
        ; All done, restore stack and return to caller
        ;**********************************************************************

NV3CHK_Done:

        add     esp,(size NV3_CURSOR_CHECKSUM_STRUC) - NV3_CURSOR_CHECKSUM_PARM_SIZE

        pop     ebx
        pop     ebp
        pop     esi

        ret     4

NV3CursorChecksum@4 endp

end

