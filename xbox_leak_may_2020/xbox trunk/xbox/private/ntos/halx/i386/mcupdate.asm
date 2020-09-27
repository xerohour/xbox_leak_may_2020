        TITLE   "Microcode Update Loader Routines"
;++
;
;  Copyright (c) 2001  Microsoft Corporation
;
;  Module Name:
;
;     mcupdate.asm
;
;  Abstract:
;
;     This module implements the routines to update the microcode for an Intel
;     processor.
;
;  Environment:
;
;     Kernel mode only.
;
;--

        .586p
        .xlist
INCLUDE ks386.inc
INCLUDE callconv.inc
        .list

INIT    SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
;
; VOID
; HalMicrocodeUpdateLoader(
;     VOID
;     )
;
; Routine Description:
;
;     This function updates the microcode for an Intel processor.
;
; Arguments:
;
;     None.
;
; Return Value:
;
;     None.
;
;--
cPublicProc _HalMicrocodeUpdateLoader, 0

;
; Determine what family and model of processor we're executing on.
;

        mov     eax, 1
        cpuid

        mov     ecx, eax

        cmp     ecx, 0686h
        lea     eax, [HalpMicrocode686]+MudbUpdateData
        je      LoadMicrocodeUpdate

        cmp     ecx, 068Ah
        lea     eax, [HalpMicrocode68A]+MudbUpdateData
        je      LoadMicrocodeUpdate

        stdRET  _HalMicrocodeUpdateLoader

;
; Load the microcode update into the processor.  EAX contains the linear address
; of the start of the update data, EDX is zero, and ECX contains the microcode
; update register.
;

LoadMicrocodeUpdate:
        mov     ecx, 079h               ; microcode update register
        xor     edx, edx
        wrmsr                           ; do the update

        stdRET  _HalMicrocodeUpdateLoader

stdENDP _HalMicrocodeUpdateLoader

;
; Include the latest microcode updates for the processors that Xbox can run on.
;

        ALIGN   4
        PUBLIC  HalpMicrocode686
HalpMicrocode686 LABEL BYTE
        INCLUDE ..\bootx\mcupdate\mu168608.txt
.errnz ($ - HalpMicrocode686) NE MudbStructureLength

        ALIGN   4
        PUBLIC  HalpMicrocode68A
HalpMicrocode68A LABEL BYTE
        INCLUDE ..\bootx\mcupdate\mu168A01.txt
.errnz ($ - HalpMicrocode68A) NE MudbStructureLength

INIT    ends

        END
