;++
;
;  Copyright (c) 2000-2001  Microsoft Corporation
;
;  Module Name:
;
;     startup.asm
;
;  Abstract:
;
;     This module implements the entry point for the 32-bit boot preloader code.
;
;  Environment:
;
;     32-bit protected mode.
;
;--

        .586p

        INCLUDE bldr.inc
        INCLUDE ks386.inc

        EXTERN  _PreloaderStartup2@4:NEAR
        EXTERN  _PreloaderFuncTable:DWORD
        EXTERN  _EncryptedPubKey:DWORD

_TEXT   SEGMENT PARA USE32 PUBLIC 'CODE'

        ASSUME  DS:_TEXT, ES:_TEXT, SS:_TEXT, FS:NOTHING, GS:NOTHING

;
; PreloaderStartup
;
; Entry point for the 32-bit boot preloader.  We don't do much work here; we
; inherit the cache policies that the romdec set up, and we set up just enough
; register and segment state for code to actually run.
;
; The entry point has to be preceded by a dword which indicates where the
; SHA function table lives.  This is so bldrbld can mark it somewhere where
; the bldr can find it.  The dword before that is the location of the public
; key.
;
;
        PUBLIC  _PreloaderStartup
        dd      _EncryptedPubKey
        dd      _PreloaderFuncTable
_PreloaderStartup PROC

        mov     eax, KGDT_R0_DATA
        mov     ds, eax
        mov     es, eax
        mov     ss, eax
        mov     esp, ROMDEC_STACK

        xor     eax, eax
        mov     fs, eax
        mov     gs, eax

;
; we should really set up the GDT here as well so we can unmap the SB ROM,
; but we'll put that off to the bldr for now
;

;
; Figure out where we're going to decrypt the boot loader to
;

        mov     esi, BLDR_RELOCATED_ORIGIN - ROMLDR_SIZE

;
; Call off to the loader code, which will return with the actual address
; to call off to, and then go to the final destination.  We'll load ebp with
; the decrypt address so it can find itself
;
        push    esi
        call    _PreloaderStartup2@4
        test    eax, eax
        jz      @F
        mov     ebp, esi
        jmp     eax

;
; If the load failed, we stop the clocks
; 
@@:     mov     edx, 0cf8h
        mov     eax, 08000036ch
        out     dx, eax
        add     edx, 4
        mov     eax, 01000000h
        out     dx, eax
        cli
        hlt

_PreloaderStartup ENDP

_TEXT   ENDS

        END
