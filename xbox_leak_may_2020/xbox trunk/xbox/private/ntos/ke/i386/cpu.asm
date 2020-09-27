        title  "Processor type and stepping detection"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    cpu.asm
;
; Abstract:
;
;    This module implements the assembley code necessary to determine
;    cpu type and stepping information.
;
; Author:
;
;    Shie-Lin Tzong (shielint) 28-Oct-1991.
;        Some of the code is extracted from Cruiser (mainly,
;        the code to determine 386 stepping.)
;
; Environment:
;
;    80x86
;
; Revision History:
;
;--

        .xlist
include ks386.inc
include callconv.inc
        .list

.586p

_TEXT   SEGMENT DWORD PUBLIC 'CODE'      ; Put IdleLoop in text section
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
;
; ULONGLONG
; FASTCALL
; RDMSR (
;   IN ULONG MsrRegister
;   );
;
; Routine Description:
;
; Arguments:
;
; Return Value:
;
;--
cPublicFastCall RDMSR, 1
    rdmsr
    fstRET  RDMSR
fstENDP RDMSR


;++
;
; VOID
; WRMSR (
;   IN ULONG MsrRegister
;   IN LONGLONG MsrValue
;   );
;
; Routine Description:
;
; Arguments:
;
; Return Value:
;
;--
cPublicProc _WRMSR, 3
    mov     ecx, [esp+4]
    mov     eax, [esp+8]
    mov     edx, [esp+12]
    wrmsr
    stdRET  _WRMSR
stdENDP _WRMSR

_TEXT   ENDS
        END
