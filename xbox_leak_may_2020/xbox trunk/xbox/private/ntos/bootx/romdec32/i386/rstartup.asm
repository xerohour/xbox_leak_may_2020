;++
;
; Copyright (c) 1989-2000  Microsoft Corporation
;
; Module Name:
;
;    rstartup.asm
;
; Abstract:
;
;    The module implements code to put the processor in protected mode.  This code executes
;    at processor reset vector
;
; Environment:
;
;    16-bit Real Mode
;
;--

; ==========================================================================

        .586p

        .xlist
        INCLUDE bldr.inc
        INCLUDE ks386.inc
        .list
        INCLUDE chipset.inc


_TEXT   SEGMENT  PARA USE16 PUBLIC 'CODE' 
        ASSUME  CS:_TEXT, DS:_TEXT, SS:_TEXT, ES:NOTHING


        PUBLIC  Startup16

;
; NOTE: Following ORG is hardcoded.  Changing any code below requires changing the ORG value
; so that the processor init always lines up at FFF0. To calculate this value, subtract the value of
; CodeSoFar label in the .lst file from FFF0 
;

IFDEF MCP_REV_B01
        ORG     0FFBCh  
ELSE    
        ORG     0FFB8h
ENDIF   


Startup16:

;
; Load the processor's global descriptor table by executing a 32-bit lgdt
;

        db      066h
        lgdt    cs:[RomDecGDTFWORD]
        
IFNDEF MCP_REV_B01
;
; Load the processor's interrupt descriptor table by executing a 32-bit lidt
; We point it to the same structure as the gdt to force the idt limit to be
; very small thus limiting the processor's capability to execute interrupt
; and fault handlers
;

        db      066h
        lidt    cs:[RomDecGDTFWORD]

ENDIF

;
; Enable the processor's protected mode support 
;

        mov     eax, cr0
        or      al, CR0_PE
        mov     cr0, eax

;
; Execute a 16:32 jump to the 32-bit part of romdec which is 512 bytes from 
; top of the address space
;

        db      066h, 0EAh
        dd      0FFFFFE00h
        dw      KGDT_R0_CODE
   

;
; NOTE: The linear address of this table is hardcoded in RomDecGDTFWORD below.  Changing the
; the size of this table requires updating the address
;
        ALIGN   4
RomDecGDT LABEL DWORD
        dd      0                       ; KGDT_NULL
        dd      0
        dd      00000FFFFh              ; KGDT_R0_CODE
        dd      000CF9B00h
        dd      00000FFFFh              ; KGDT_R0_DATA
        dd      000CF9300h
RomDecGDTEnd LABEL DWORD


;
; Processor will start executing code here which must be at FFFF_FFF0
;

ProcessorInit:
       
        CodeSoFar = (ProcessorInit - Startup16)

;
; Jump to the start of 16-bit code
;
        jmp     Startup16                    

;
; FWORD to initialize the processor's descriptor tables.
;

        ALIGN   4
RomDecGDTFWORD LABEL FWORD
        dw      OFFSET RomDecGDTEnd - OFFSET RomDecGDT    

        dd      0FFFFFFD8h

;
; The 32-bit part of the RomDec jumps to this code located FFFF FFFA.  This code finishes the PCI
; cycle that was started by the 32-bit code to turn of the Southbridge ROM.  Turning off SB ROM causes
; the next instruction being executed fetched from main ROM.  This code causes the next instruction 
; to fall outside of the the address space causing an unhandled exception to occur so
; we don't execute any code from main ROM
;
; The processor will be in Protected Mode while executing this code.  This code must always ORG to FFFA.
;
ShutdownEnd:
        add     dl, 04h
        mov     al, 2
        out     dx, al

_TEXT ENDS


; ==========================================================================

        END Startup16



