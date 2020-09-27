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
;    32-bit Protected Mode
;
;--

; ==========================================================================

        .586p

        .xlist
        INCLUDE bldr.inc
        INCLUDE ks386.inc
        .list
        
        INCLUDE chipset.inc
        

_TEXT   SEGMENT PARA USE32 PUBLIC 'CODE'
        ASSUME  DS:_TEXT, ES:_TEXT, SS:NOTHING, FS:NOTHING, GS:NOTHING


        PUBLIC  _Startup32

_Startup32 PROC

;
; Setup segment registers
;       
        xor     eax, eax
        mov     al, KGDT_R0_DATA
        
        mov     ds, eax
        mov     es, eax
        mov     ss, eax


;
; Initialize chipset and RAM by parsing the init table
;
        INCLUDE command.inc
        INCLUDE initcode.inc

;
; Setup MTRRs and enable caching.  EBX was setup by the COMMAND_QUIT
; command in the init table and is supposed to contain the value
; used for default MTRR register
;
        xor     ecx, ecx
        mov     ch, 02h
        xor     eax, eax
        xor     edx, edx
@@:
        wrmsr
        inc     ecx
        cmp     cl, 0Fh
        jbe     @B

;
; Enable MTRR, disable fix-range MTRRs and set default memory type to UC.
;

        mov     cl, 0FFh
        mov     eax, ebx
        wrmsr

;
; Enable the processor cache by clearing cache disable and not-write-through
; flags in CR0.
;
        mov     eax, cr0
        and     eax, NOT (CR0_CD OR CR0_NW)
        mov     cr0, eax

IFNDEF MCP_XMODE2
IFNDEF MCP_XMODE3P
;
; XMODE3: Decrypt the boot loader.  Destination ROMDEC_OUTPUTBUFFER
;
        INCLUDE encrypt.inc

;
; Check to see if the encryption was successful.  RC4 algorithm does not provide
; success information.  Therefore, we check for a known signature at the end of
; decrypted boot loader
;

        mov     eax, ds:DWORD PTR [ROMDEC_OUTPUTBUFFER + ROMLDR_SIZE - ROMLDR_BOOTPARAMSIZE]
        cmp     eax, ROMLDR_SIGNATURE
        jne     Shutdown

; Jump to the boot loader startup.  The entry point of the boot loader is
; stored as the first DWORD at the decrypted code
;
; NOTE: The content of eax register must be the entry point of the boot
;       loader to indicate that we are not running in XDK box so that
;       boot loader will initialize MTRRs to enable RAM/ROM caching
; stored just prior to the signature in the boot param
;

        mov     eax, ds:DWORD PTR [ROMDEC_OUTPUTBUFFER]
        jmp     eax

ELSE ; XM3P

;
; XMODE3P: Hash the boot loader and verify that its hash is what we expect to find
;
        INCLUDE boothash.inc

; returns with ZF indicating whether we matched the hash
        jnz Shutdown

;
; Jump to the boot loader startup.  The entry point of the boot loader is
; stored just prior to the signature in the boot param
;

        jmp     _Startup32 - ROMPRELDR_SIZE

ENDIF ; XM3P
ELSE ; XM2

;
; XMODE2: Jump to the boot loader startup.  The entry point of the boot loader is
; stored just prior to the signature in the boot param
;

        jmp     _Startup32 - ROMPRELDR_SIZE

ENDIF ; XM2

;
; Sequence of instructions to turn off SB ROM and halt.  The following code
; does not do RMW because the system is shutting down
;
Shutdown:
IFDEF MCP_XMODE2

        hlt
        
ELSE

        mov     eax, 80000880h
        mov     dx, 0CF8h
        out     dx, eax

ENDIF
;
; Now jump to the top of the address space.  The code there will complete the shutdown sequence
;
        db      0EAh
        dd      0FFFFFFFAh
        dw      KGDT_R0_CODE

_Startup32 ENDP

IFNDEF MCP_XMODE2
IFNDEF MCP_XMODE3P
;
; XMODE3: Encryption key placeholder
;
        INCLUDE ENCKEY.INC
ENDIF
ENDIF


_TEXT   ENDS

; ==========================================================================

        END



