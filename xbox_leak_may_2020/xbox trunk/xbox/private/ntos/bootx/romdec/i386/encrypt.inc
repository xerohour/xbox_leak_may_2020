;++
;
; Copyright (c) 1989-2000  Microsoft Corporation
;
; Portions Copyright (C) 1987 RSA Data Security, Inc. Created 1987.
;    This is an unpublished work protected as such under copyright law.
;    This work contains proprietary, confidential, and trade secret information
;    of RSA Data Security, Inc.  Use, disclosure, or reproduction without the
;    express written authorization of RSA Data Security, Inc., is prohibited.
;
; Abstract:
;
;    This module implements the RSA Data Security RC4 encryption algorithm.
;
;--


RC4_TABLESIZE     EQU     256             ; RC4 state table size



RC4Encrypt PROC NEAR

        LOCAL   statetable[RC4_TABLESIZE]:BYTE
        LOCAL   i:BYTE
        LOCAL   j:BYTE
        LOCAL   k:BYTE

;
; RC4 key expansion functionality (rc4_key)
;------------------------------------------

        lea     dx, statetable                  ; keep address of statetable in dx
        xor     ch, ch                          ; clear high byte of cx

;
; for (ii=0;ii<RC4_TABLESIZE;ii++)
;    keystruct.S[ii] = (unsigned char)ii;
; (bx => ii)
;
        xor     bx, bx
        mov     di, dx
.REPEAT
        mov     [di + bx], bl                   ; write out loop counter to statetable[ii]
        inc     bx
.UNTIL (bx > RC4_TABLESIZE)

;
; j = k = (unsigned char) 0;
; for (ii=0;ii<RC4_TABLESIZE;ii++) {
;    j = (unsigned char)((j + keystruct.S[ii] + enckey[k]) % RC4_TABLESIZE);
;
;    t = keystruct.S[ii];
;    keystruct.S[ii] = keystruct.S[j];
;    keystruct.S[j] = t;
;
;    k = (unsigned char) ((k+1) % RC4_KEYSIZE);
; }
; (bx => ii)
;
        xor     bx, bx                          ; clear loop counter, j, k
        xor     al, al                          ; keep j in al
        mov     k, bl

.REPEAT
        mov     di, dx                          ; calculate expression result in ax

        add     al, [di + bx]                   ; add statetable[ii]

        mov     cl, k
        mov     si, cx                          ; put value of k in si

        inc     cl                              ; calculate new value of k
        and     cl, 0Fh                         ; (k + 1) % RC4_KEYSIZE(=16)
        mov     k, cl

        add     al, cs:BYTE PTR EncKey[si]      ; add enckey[k] to expression result

        mov     di, dx                          ; swap bytes at ii and j in the keytable
        mov     cl, al                          ; word extend j to cx
        add     di, cx
        mov     cl, [di]                        ; statetable[j] => cl

        mov     si, dx                          ; swap ah with statetable[ii]
        mov     ah, [si + bx]
        mov     [si + bx], cl
        mov     [di], ah

        inc     bx                              ; advance loop counter
.UNTIL (bx >= RC4_TABLESIZE)


;
; RC4 encryption functionality (rc4)
;-----------------------------------

;
; for (k = 0; k < buffersize; k++) {
;    i = (unsigned char) ((i + 1) % RC4_TABLESIZE);
;    j = (unsigned char) ((j + keystruct.S[i]) % RC4_TABLESIZE);
;
;    t = keystruct.S[i];
;    keystruct.S[i] = keystruct.S[j];
;    keystruct.S[j] = t;
;
;    outbuffer[k] = inbuffer[k] ^ keystruct.S[(keystruct.S[i]+keystruct.S[j]) % RC4_TABLESIZE];
; }
; (bx => k)

        xor     bx, bx                          ; clear loop counter, i, j
        mov     i, bl
        mov     j, bl
.REPEAT

        inc     i                               ; calculate new value of i

        mov     di, dx                          ; add statetable[i]
        mov     cl, i
        add     di, cx
        mov     al, [di]
        add     j, al                           ; mod 256

        mov     di, dx                          ; swap bytes at i and j in the statetable
        mov     cl, j
        add     di, cx
        mov     al, [di]

        mov     si, dx
        mov     cl, i
        add     si, cx

        mov     ah, [si]                        ; swap al with statetable[i]
        mov     [si], al
        mov     [di], ah

        mov     cl, [di]                        ; add previously obtained statetable[i] and statetable[j] in ax
        mov     ax, cx
        add     al, [si]                        ; mod 256

        mov     di, dx                          ; get the statetable[modresult]
        add     di, ax

        mov     al, BYTE PTR [di]

IFDEF ENC_USE_FS_GS
        mov	    cl, fs:BYTE PTR [bx]
ELSE
        mov	    di, ENC_INPUT_OFFSET            ; get input buffer[k]
        mov	    cl, cs:BYTE PTR [bx + di]
ENDIF

        xor     cl, al                          ; xor the two values

IFDEF ENC_USE_FS_GS
        mov     gs:BYTE PTR [bx], cl
ELSE
        mov     di, ENC_OUTPUT_OFFSET           ; write the result to the output buffer
        mov     es:BYTE PTR [bx + di], cl
ENDIF

        inc     bx                              ; advance loop counter

.UNTIL (bx >= ENC_BUF_SIZE)

        ret

RC4Encrypt ENDP

