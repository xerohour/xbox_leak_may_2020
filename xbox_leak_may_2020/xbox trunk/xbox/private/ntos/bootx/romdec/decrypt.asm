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
; Module Name:
;
;    decrypt.asm
;
; Abstract:
;
;    The module decrypts the ROM loader
;
;    This module implements the RSA Data Security RC4 encryption algorithm.
;
; Author:
;
;    Yasser Asmi (yasmi)  7-Jun-2000
;
; Environment:
;
;    Real Address Mode
;
;--

; ==========================================================================

.586p

BOOT_CODE_ORIGIN  EQU    0FE00h          ; start of the code

ROMLDR_SIZE       EQU    6000h           ; size of the rom loader (must match BLDR_BLOCK_SIZE)

ENC_BUF_SIZE      EQU    ROMLDR_SIZE

ENC_INPUT_OFFSET  EQU    (BOOT_CODE_ORIGIN - ROMLDR_SIZE)

ENC_OUTPUT_OFFSET EQU    0

; ==========================================================================


; temp EQUs will be replaced by include files
BLDR_CODE         EQU    90000h
CR0_CD            EQU    040000000H
CR0_NW            EQU    020000000H


;
; For now, the boot loader will be loaded at 9000:0000.
;
BLDRTEXT SEGMENT USE16 AT (BLDR_CODE SHR 4)

        ORG     0h
Bldr16Startup   LABEL BYTE

BLDRTEXT ENDS



STACKSEG SEGMENT USE16 AT 8000h

        ORG     0FFFEh
StackStartup    LABEL BYTE

STACKSEG ENDS







_TEXT   SEGMENT  PARA USE16 PUBLIC 'CODE' 
        ASSUME  CS:_TEXT, DS:NOTHING, SS:NOTHING, ES:NOTHING


        PUBLIC Start

        ORG BOOT_CODE_ORIGIN

Start:
        cld

; 
; enable processor cache
;
        mov     eax, cr0
        and     eax, NOT (CR0_NW OR CR0_CD)
        mov     cr0, eax  


; INTEL 440BX

; initialize DRAM using SDROM Control Register  
;

        ; setup PCI cycle to 76h
        mov     eax, 080000074h
        mov     dx, 0CFEh

        ; NOP Command Enable
        mov     al, 20h 
        out     dx, al

        mov     cx, -1
@@:     loop    @B

         ; All Banks Precharge Enable
        mov     al, 40h  
        out     dx, al

    	mov	    ebx, ds:[0]

        mov     cx, 1000h
@@:     loop    @B
        	

        ; CBR Enable Command
        mov     al, 80h   
        out     dx, al

        mov     cx, 8
CBRLoop:
    	mov	    ebx, ds:[0]
        loop    CBRLoop


        ; Mode Register Set Enable Command 
        mov     al, 60h   
        out     dx, al

    	;mov     di, 01F0h    ;MKF_CL2_ADDRESS_FOR_MAA;CL2 address for Rows 3:0
	    ;mov     di, 0FEA8h   ; MKF_CL2_ADDRESS_FOR_MAB; No, get correct address
    	;mov     di, 01D0h    ; CL3

        mov     di, 0DE28h
	    mov     ebx, ds:[di]


        ; Normal SDRAM Operation
        mov     al, 0h   
        out     dx, al

        mov     ebx, ds:[0] 

; 
; initialize chipset registers
;
        xor     si, si
        xor     bx, bx

RAMInitLoop:
        mov     bl, cs:BYTE PTR RAMInitData[si]
        cmp     bl, 0
        je      RAMInitDone
            
        mov     eax, 080000000h
        mov     al, bl
        mov     dx, 0CF8h
        inc     si
        out     dx, eax
        
        mov     dx, 0CFCh
        mov     ax, bx
        mov     eax, cs:DWORD PTR RAMInitData[si]
        out     dx, eax
        add     si, 4
        jmp     RAMInitLoop
                
RAMInitDone:


;
; 
        mov     ax, SEG STACKSEG          
        mov     ss, ax                           
        mov     sp, 0FFFEh

        mov     ax, SEG STACKSEG
        mov     ds, ax

        mov     ax, SEG BLDRTEXT
        mov     es, ax

        call    RC4Encrypt
        ;call    JustCopy


        jmp     FAR PTR Bldr16Startup
        
      
; ==========================================================================

EncKey           DB      'abcdefgh'       ; the encryption key 


;
; ram initialization data pairs (register offset byte, value dword)
;
RAMInitData:

        DB  050h
        DD  0FF00800Ch
        DB  054h
        DD  009000000h
        DB  058h
        DD  000111003h
        DB  05Ch
        DD  011110000h
        DB  060h
        DD  004040404h
        DB  064h
        DD  004040404h
        DB  068h
        DD  000000000h
        DB  06Ch
        DD  000000000h
        DB  074h
        DD  000000000h
        DB  078h
        DD  0381C0100h

        ; end of RAM init data
        DB  00h
    
; ==========================================================================



JustCopy PROC NEAR 

        xor     bx, bx                          ; clear loop counter
.REPEAT
        mov	    di, WORD PTR ENC_INPUT_OFFSET   ; get input buffer[k]
        mov	    cl, BYTE PTR cs:[bx + di]  

        mov     di, WORD PTR ENC_OUTPUT_OFFSET  ; write the result to the output buffer   
        mov     BYTE PTR es:[bx + di], cl

        inc     bx                              ; advance loop counter

.UNTIL (bx >= ENC_BUF_SIZE)

        ret     0

JustCopy ENDP




;++
;
; RC4Encrypt - This function carries performs decryption using the RC4 algorithm
;
; Arguments - 
;
; Returns
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
;



;!  for (ii=0;ii<RC4_TABLESIZE;ii++)
;!    keystruct.S[ii] = (unsigned char)ii;
;!
;! bx => ii

        xor     bx, bx
        lea     di, statetable               
.REPEAT
        mov     [di + bx], bl                ; write out loop counter to statetable[ii]
        inc     bx 
.UNTIL (bx > RC4_TABLESIZE)


;!  j = k = (unsigned char) 0;
;!  for (ii=0;ii<RC4_TABLESIZE;ii++)
;!  {
;!    j = (unsigned char)((j + keystruct.S[ii] + enckey[k]) % RC4_TABLESIZE);
;!
;!    t = keystruct.S[ii];
;!    keystruct.S[ii] = keystruct.S[j];
;!    keystruct.S[j] = t;
;!
;!    k = (unsigned char) ((k+1) % RC4_KEYSIZE);
;!  }
;! 
;! bx => ii


        xor     bx, bx                          ; clear loop counter, j, k
        mov     j, bl
        mov     k, bl

.REPEAT
        xor     ch, ch
        
        lea     di, statetable                  ; calculate expression result in ax 
        xor     ah, ah        
        mov     al, j                           ;   add j
        mov     cl, [di + bx]                   ;   add statetable[ii]
        add     ax, cx

        xor     si, si                          ;   put value of k in cx and add it to si
        mov     cl, k
        add     si, cx 
        
        inc     cx                              ;   calculate new value of k
        and     cx, 7                           ;      (k + 1) % RC4_KEYSIZE(=8)
        mov     k, cl

        mov	    cl, cs:BYTE PTR EncKey[si]        ;   add enckey[k] to expression result
        add     ax, cx

        mov     cx, RC4_TABLESIZE               ; divide ax by tablesize and put remainer byte in j
        cwd 
        idiv    cx
        mov     j, dl

        lea     di, statetable                  ; swap bytes at ii and j in the keytable
        xor     ch, ch       
        mov     cl, j 
        add     di, cx 
        mov     al, [di]                        ;   statetable[j] => al
        
        lea     si, statetable                  ;   swap al with statetable[ii] 
        xchg    al, BYTE PTR [si + bx]
        mov     [di], al                        ;   write al back


        inc     bx                              ; advance loop counter
.UNTIL (bx >= RC4_TABLESIZE)



; RC4 encryption functionality (rc4) 
;


;!  for (k = 0; k < buffersize; k++)
;!  {
;!    i = (unsigned char) ((i + 1) % RC4_TABLESIZE);
;!    j = (unsigned char) ((j + keystruct.S[i]) % RC4_TABLESIZE);
;!
;!    t = keystruct.S[i];
;!    keystruct.S[i] = keystruct.S[j];
;!    keystruct.S[j] = t;
;!
;!    outbuffer[k] = inbuffer[k] ^ keystruct.S[(keystruct.S[i]+keystruct.S[j]) % RC4_TABLESIZE];
;!   }
;!
;! bx => k


        xor     bx, bx                          ; clear loop counter, i, j
        mov     i, bl
        mov     j, bl
.REPEAT

        xor     ah, ah                          ; calculate new value of i
        mov     al, i                           ;   i + 1
        inc     ax
        
        mov     cx, RC4_TABLESIZE               ;   divide by table size and put remained byte in i
        cwd 
        idiv    cx
        mov     i, dl

        xor     ah, ah                          ; calculate new value of j in ax
        mov     al, j

        lea     di, statetable                  ;   add statetable[i]                  
        xor     ch, ch         
        mov     cl, i
        add     di, cx
        add     ax, [di]

        mov     cx, RC4_TABLESIZE               ;   divide by table size and put remained byte in j
        cwd 
        idiv    cx
        mov     j, dl

        lea     di, statetable                  ; swap bytes at i and j in the statetable
        xor     ch, ch                          
        mov     cl, j 
        add     di, cx 
        mov     al, [di]                        ;   statetable[j] => al
        
        lea     si, statetable
        mov     cl, i
        add     si, cx  
        
        xchg    al, BYTE PTR [si]               ;   swap al with statetable[i] 
        mov     [di], al                        ;   write al back


        xor     ax, ax                          ; calculate expression result in ax

        xor     ch, ch                          ;   add previously obtained statetable[i] and statetable[i]
        mov     cl, [di]  
        add     ax, cx
        mov     cl, [si]  
        add     ax, cx

        mov     cx, RC4_TABLESIZE               ;   divide by table size
        cwd
        idiv    cx


        lea     di, statetable                  ; get the statetable[modresult]
        add     di, dx
        mov     al, BYTE PTR [di]      


        mov	    di, WORD PTR ENC_INPUT_OFFSET   ; get input buffer[k]
        mov	    cl, cs:BYTE PTR [bx + di]      
        xor     cl, al                          ; xor the two values

        mov     di, WORD PTR ENC_OUTPUT_OFFSET  ; write the result to the output buffer   
        mov     es:BYTE PTR [bx + di], cl

        inc     bx                              ; advance loop counter

.UNTIL (bx >= ENC_BUF_SIZE)


        ret     0

RC4Encrypt ENDP




; ==========================================================================
;
; Pad until processor startup location.  The code will be loaded at 512 bytes
; below the uppermost memory location.  The processor starts execution at 16
; bytes below the uppermost memory location.  If the above code takes less than
; 512-16 bytes, then we want to put padding of appropriate number of bytes
; so that we can put a jmp to our start of code at the right place

PadLabel:

        PadSize = (200h - 10h - (PadLabel - Start))
        
        ;db      PadSize     DUP(90H)                ; 90h=NOP
        db      11h     DUP(90H)                ; 90h=NOP

ProcessorInit:

        jmp     Start                    ; jump to the start of code
        
        REPT 13                                 ; pad with nop to complete 512 bytes
            nop
        ENDM

; ==========================================================================


_TEXT   ENDS



        END      Start



