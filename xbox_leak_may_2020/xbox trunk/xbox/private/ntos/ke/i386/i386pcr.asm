        title  "I386 PCR"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    i386pcr.asm
;
; Abstract:
;
;    This module implements routines for accessing and initing the pcr.
;
; Author:
;
;    Bryan Willman (bryanwi) 20 Mar 90
;
; Environment:
;
;    Kernel mode, early init of first processor.
;
; Revision History:
;
;--

.386p
        .xlist
include ks386.inc
include callconv.inc                    ; calling convention macros
        .list

        extrn   _KiPCR:DWORD

;
;   NOTE - This definition of PCR gives us 2 instructions to get to some
;       variables that need to be addressable in one instruction.  Any
;       such variable (such as current thread) must be accessed via its
;       own access procedure (see below), NOT by KeGetPcr()->PbCurrentThread.
;       (This is only an issue on MP machines.)
;

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

cPublicProc _KeGetPcr ,0

        lea     eax,_KiPCR
        stdRET    _KeGetPcr

stdENDP _KeGetPcr


;++
;
; PKPRCB
; KeGetCurrentPrcb()
;
; Return Value:
;
;   Pointer to current PRCB.
;
;--
cPublicProc _KeGetCurrentPrcb   ,0

        mov     eax,PCR[PcPrcb]
        stdRET    _KeGetCurrentPrcb

stdENDP _KeGetCurrentPrcb


;++
;
; PKTHREAD
; KeGetCurrentThread()
;
; Return Value:
;
;   Pointer to current Thread object.
;
;--
cPublicProc _KeGetCurrentThread   ,0

        mov     eax,PCR[PcPrcbData+PbCurrentThread]
        stdRET    _KeGetCurrentThread

stdENDP _KeGetCurrentThread


;++
;
; BOOLEAN
; KeIsExecutingDpc(
;       VOID
;       );
;
; Return Value:
;
;   Value of flag which indicates whether we're executing in DPC context
;
;--

cPublicProc _KeIsExecutingDpc   ,0

        mov     eax,PCR[PcPrcbData.PbDpcRoutineActive]
        stdRET    _KeIsExecutingDpc

stdENDP _KeIsExecutingDpc

_TEXT   ENDS
        end
