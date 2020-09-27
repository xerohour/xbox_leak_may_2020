;++
;
; Copyright (c) 1989-2000  Microsoft Corporation
;
; Module Name:
;
;    romdec.asm
;
; Abstract:
;
;    The module decrypts the ROM loader
;
; Environment:
;
;    Real Address Mode
;
;--

; ==========================================================================

        .586p

        INCLUDE bldr.inc


ENC_BUF_SIZE      EQU    ROMLDR_SIZE

ENC_INPUT_OFFSET  EQU    (BOOT_CODE_ORIGIN - ROMLDR_SIZE)

ENC_OUTPUT_OFFSET EQU    0

CR0_CD            EQU    040000000H

CR0_NW            EQU    020000000H

; ==========================================================================
;
; segment where bootloader code will be decrypted
;
BLDRTEXT SEGMENT USE16 AT (BLDR_CODE SHR 4)

        ORG     0h
Bldr16Startup   LABEL BYTE

BLDRTEXT ENDS


; ==========================================================================
;
; stack segment
;
STACKSEG SEGMENT USE16 AT 8000h

        ORG     0FFFEh
StackStartup    LABEL BYTE

STACKSEG ENDS



; ==========================================================================
;
; main segment
;
_TEXT   SEGMENT  PARA USE16 PUBLIC 'CODE' 
        ASSUME  CS:_TEXT, DS:NOTHING, SS:NOTHING, ES:NOTHING


        PUBLIC  Start

        ORG     BOOT_CODE_ORIGIN

Start:
;
; reset vector transfers the control here
;

        cld
; 
; enable processor cache
;
        mov     eax, cr0
        and     eax, NOT (CR0_NW OR CR0_CD)
        mov     cr0, eax  

;
; initialize chipset and DRAM
;
        INCLUDE INITCODE.INC


;
; setup segments
; 
        mov     ax, SEG STACKSEG          
        mov     ss, ax                           
        mov     sp, 0FFFEh

        mov     ax, SEG STACKSEG
        mov     ds, ax

        mov     ax, SEG BLDRTEXT
        mov     es, ax

;
; decrypt the boot loader
;
        call    RC4Encrypt


;
; check to see if the encryption was successful.  RC4 algorithm does not provide
; success information.  Therefore, we check for a known signature in the
; decrypted boot loader
;

        mov     eax, es:DWORD PTR [ROMLDR_SIZE - ROMLDR_BOOTPARAMSIZE]   
        cmp     eax, ROMLDR_SIGNATURE
        jz      JumpToBootLoader

Fail:
;
; ROM is not valid.  Shutdown by copying the shutdown sequence to RAM and executing it.  We
; cannot execute in SB ROM because after we turn of SB ROM, the CPU will continue execution
; in the main ROM.  
;
        
        mov     si, OFFSET TurnOffSbRom
        xor     di, di
        mov     cx, 22
ShutdownCopyLoop:
        mov     al, cs:BYTE PTR [si]                           
        mov     es:BYTE PTR [di], al
        inc     si
        inc     di
        loop    ShutdownCopyLoop

JumpToBootLoader:
;
; jump to the boot loader startup or in case of failure the shutdown sequence
;
        mov     si, OFFSET CmdLine
        jmp     FAR PTR Bldr16Startup
        
      
; ==========================================================================

;
; command line options to boot loader 
;
CmdLine     DB      0

;
; sequence of instructions to turn off SB ROM and halt
;
TurnOffSBRom:
          
;        mov     eax, 80000880h
;        mov     dx, 0CF8h
;        out     dx, eax
;        mov     dx, 0CFCh
;        mov     al, 2
;        out     dx, al
        hlt

;
; encryption key
;
        INCLUDE ENCKEY.INC

;
; chipset initialization related data
;
        INCLUDE INITDATA.INC


;
; encryption function
;
        INCLUDE ENCRYPT.INC

; ==========================================================================


;
; pad until processor startup location.  The code will be loaded at 512 bytes
; below the uppermost memory location.  The processor starts execution at 16
; bytes below the uppermost memory location.  If the above code takes less than
; 512-16 bytes, then we want to put padding of appropriate number of bytes
; so that we can put a jmp to our start of code at the right place

PadLabel:

;
; NOTE: following constant sets the number of bytes to be used for padding.  If
; the above code changes then the pad size must be adjusted.  We cannot use 
; this directly because of one-pass assembly.  However, if the number is not
; correct the following .ERRNZ condition will fail with A2045.  If you see the
; error during compilation, look at the .lst file and change PadSize value to
; the value reported for RequiredPadSize
;
        PadSize = 44h
        RequiredPadSize = (200h - 10h - (PadLabel - Start))

.ERRNZ (200h - 10h - (PadLabel - Start)) - PadSize


        db      PadSize     DUP(90H)                ; 90h=NOP

;
; this is where the processor will start executing code
;
ProcessorInit:
        jmp     Start                    ; jump to the start of code


;
; pad with nop to complete 512 bytes
;
        REPT 13
            nop
        ENDM

; ==========================================================================

_TEXT   ENDS

        END      Start



