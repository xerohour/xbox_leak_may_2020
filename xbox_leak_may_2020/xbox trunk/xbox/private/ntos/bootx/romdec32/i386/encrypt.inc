;++
;  Copyright (c) 1996, 1997 Microsoft Corporation
;
;  Copyright (C) 1987 RSA Data Security, Inc. Created 1987.
;  This is an unpublished work protected as such under copyright law.
;  This work contains proprietary, confidential, and trade secret information
;  of RSA Data Security, Inc.  Use, disclosure, or reproduction without the
;  express written authorization of RSA Data Security, Inc., is prohibited.
;
;
;  Abstract:
;
;     This code is derived from Scott Field's optimized implementation of the
;     RC4 algorithm.  Following changes were made.
;        1.  rc4_key and rc4 functionality have been merged 
;        2.  Does not preserve any registers--does not use stack
;        3.  Does not take any parameters--everything is hardcoded
;        4.  Does not update the indicies in the key structure at end
;        5.  Separate input/output buffers
;
;  Environment:
;
;     32-bit Protected Mode
;
;--



;
; RC4 key expansion functionality (rc4_key) 
;

        mov     eax, 03020100H          ; initial fill value
        mov     ecx, 64                 ; loop iteration count

        mov     esi, ROMDEC_KEYSTRUCT

        mov     edx, esi

;
; for (a=0x03020100,i=0;i<RC4_TABLESIZE/sizeof(DWORD);i++, a+= 0x04040404)
;       ((DWORD*)p)[i] = a;

init_loop:

        mov     DWORD PTR [edx], eax
        add     edx, 4

        add     eax, 04040404H
        dec     ecx

        jne     init_loop

; i = j = k = 0;

        xor     ecx, ecx                ; k = 0
        xor     edi, edi

        mov     ebp, OFFSET EncKey
        mov     BYTE PTR [esi+256], cl  ; Key->i = 0;

        mov     BYTE PTR [esi+257], cl  ; Key->j = 0;
        xor     ebx, ebx


; while (i < RC4_TABLESIZE) {

permute_loop:

        ; ti = p[i];

        ; tk = K[k];
        ; j += tk;

        ; j += ti;

        ; j &= (RC4_TABLESIZE - 1);
        ; tj = p[j];
        ; k++;

        ; p[i] = tj;

        ; i++;
        ; p[j] = ti;

        xor     edx, edx                        ; avoid p6 partial stall
        xor     eax, eax                        ; avoid p6 partial stall

        mov     dl, BYTE PTR [esi+edi]
        mov     al, BYTE PTR [ecx+ebp]

        add     bl, al                          ; add ebx, eax
        inc     ecx

        add     bl, dl                          ; add ebx, edx
        inc     edi

        ; if (k == m)

;        and     ebx, 0FFh                      ; and not needed due to 8 bit add

        mov     al, BYTE PTR [esi+ebx]
        mov     BYTE PTR [esi+edi-1], al

        cmp     ecx, ROMDEC_KEYSIZE      
        mov     BYTE PTR [esi+ebx], dl

        jne     skip_zero_k

        xor     ecx, ecx

skip_zero_k:

        cmp     edi, 256
        jb      permute_loop



;
; RC4 encryption functionality (rc4) 
;

        xor     ecx, ecx
        xor     edx, edx
        xor     edi, edi
        xor     eax, eax

        mov     esi, ROMDEC_KEYSTRUCT   ; p = Key->S
        mov     ebp, ROMDEC_BUFFERSIZE  ; byte count
        
        mov     cl, BYTE PTR [esi+256]  ; i = Key->i

        mov     dl, BYTE PTR [esi+257]  ; j = Key->j

rc4_loop:

        inc     cl                      ; i ++ ; i &= (RC4_TABLESIZE-1)

        mov     al, BYTE PTR [ecx+esi]  ; t = p[i]

        add     dl, al                  ; j += t ; j &= (RC4_TABLESIZE-1)

        mov     bl, BYTE PTR [edx+esi]  ; pickup p[j]

        mov     BYTE PTR [ecx+esi], bl  ; p[i] = p[j]
        mov     BYTE PTR [edx+esi], al  ; p[j] = t (potential bank conflict)

;
; *outputbuffer++ = *inputbuffer++ ^ p[( p[i] + t) & (RC4_TABLESIZE-1) ];
;

        add     al, bl                                  ; (t += p[i]) & RC4_TABLESIZE-1

        mov     bl, BYTE PTR ROMDEC_INPUTBUFFER[edi]    ; get byte from input buffer

        mov     al, BYTE PTR [eax+esi]                  ; pickup p[t]

        xor     bl, al                  

        mov     BYTE PTR ROMDEC_OUTPUTBUFFER[edi], bl   ; write byte to output buffer

        inc     edi                                     ; bufferindex++

        dec     ebp                                     ; loop until no input left to process
        jnz     rc4_loop                

rc4_done:

