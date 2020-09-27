;++
;
;  Copyright (c) 2000-2001  Microsoft Corporation
;
;  Module Name:
;
;     bldr.inc
;
;  Abstract:
;
;     This module contains shared constants for the various boot loaders.
;
;--

;
; Define the starting physical address where the ROM decrypts the boot loader
;

BLDR_BOOT_ORIGIN                    EQU     00090000h

;
; Define the starting physical address where the boot loader relocates itself
;

BLDR_RELOCATED_ORIGIN               EQU     00400000h

;
; Initialize any page table entries with valid, write, accessed, and dirty.
;

BLDR_VALID_KERNEL_PTE_BITS          EQU     063h

;
; Initialize any page table entries with valid, write, accessed, large page, and dirty.
;

BLDR_VALID_KERNEL_LARGE_PTE_BITS    EQU     0E3h

;
; Initialize any page table entries with valid, write, accessed, large page, 
; cache disabled, write through, and dirty.
;

BLDR_VALID_KERNEL_LARGE_PTE_UC_BITS  EQU    0FBh


;
; Origin for the processor reset vector.
;

BOOT_CODE_ORIGIN                    EQU     0FE00h

;
; Size of the rom loader (must match BLDR_BLOCK_SIZE in bldr.h).
;

ROMLDR_SIZE                         EQU     6000h

;
; Size of the rom preloader (must match PRELDR_BLOCK_SIZE in bldr.h).
;

ROMPRELDR_SIZE                      EQU     2A00h

;
; Signature in the rom loader (must match BLDR_SIGNATURE in bldr.h)
;

ROMLDR_SIGNATURE                    EQU     7854794Ah

;
; Size of the XBOOT_PARAM structure defined in bldr.h
;

ROMLDR_BOOTPARAMSIZE                EQU     28

;
; ROMDEC hash address and nonce
;

ROMDEC_HASH                         EQU     88
ROMDEC_N                            EQU     100

;
; ROMDEC key size in bytes
;

ROMDEC_KEYSIZE                      EQU     16

;
; linear address for ROMDEC stack
;

ROMDEC_STACK                        EQU     8F000h

;
; linear address for RC4 key structure (258 bytes)
;

ROMDEC_KEYSTRUCT                    EQU     8F000h

;
; linear address for the start of the ROMDEC code block (top 512 bytes of
; the address space)
;

ROMDEC_STARTADDRESS                 EQU     0FFFFFE00h

;
; ROMDEC buffer size to decrypt
;

ROMDEC_BUFFERSIZE                   EQU     ROMLDR_SIZE

;
; liner address for input buffer used in encryption
;

ROMDEC_INPUTBUFFER                  EQU     (ROMDEC_STARTADDRESS - ROMLDR_SIZE)

;
; liner address for output buffer used in encryption
;

ROMDEC_OUTPUTBUFFER                 EQU     BLDR_BOOT_ORIGIN

;
; linear address for the init table
;

ROMDEC_INIT_TABLE                   EQU     0FF000080h

