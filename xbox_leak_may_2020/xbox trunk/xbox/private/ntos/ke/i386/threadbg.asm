        title  "Thread Startup"

;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    threadbg.asm
;
; Abstract:
;
;    This module implements the code necessary to startup a thread in kernel
;    mode.
;
; Author:
;
;    Bryan Willman (bryanwi) 22-Feb-1990, derived from DaveC's code.
;
; Environment:
;
;    Kernel mode only, IRQL PASSIVE_LEVEL.
;
; Revision History:
;
;--

.386p
        .xlist
include ks386.inc
include i386\kimacro.inc
include callconv.inc
        .list

        EXTRNP  KfLowerIrql,1,,FASTCALL
        EXTRNP  _KeBugCheck,1
        EXTRNP  _PsTerminateSystemThread,1
        extrn   _KiPCR:DWORD

        page ,132
        subttl  "Thread Startup"

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
;
; Routine Description:
;
;    This routine is called at thread startup. Its function is to call the
;    initial thread procedure.
;
; Arguments:
;
;   (TOS)    = SystemRoutine - address of initial system routine.
;   (TOS+4)  = StartRoutine - Initial thread routine.
;   (TOS+8)  = StartContext - Context parm for initial thread routine.
;
; Return Value:
;
;    None.
;
;--

cPublicProc _KiThreadStartup    ,1

        xor     ebx,ebx             ; clear registers
        xor     esi,esi             ;
        xor     edi,edi             ;
        xor     ebp,ebp             ;
        mov     ecx, PASSIVE_LEVEL
        fstCall KfLowerIrql         ; KeLowerIrql(PASSIVE_LEVEL)

;
; Check if PsCreateSystemThreadEx failed to fully initialize the thread.  If so,
; it will mark the thread as already terminated and use PsTerminateSystemThread
; to cleanup the thread.
;

        mov     eax, PCR[PcPrcbData+PbCurrentThread]
        cmp     BYTE PTR [eax].ThHasTerminated, 0
        jne     kits20

;
; Invoke the system routine.
;

        pop     eax                 ; (eax)->SystemRoutine
        call    eax                 ; SystemRoutine(StartRoutine, StartContext)
IFNDEF STD_CALL
        add     esp,8               ; Clear off args
ENDIF

kits10: stdCall _KeBugCheck, <NO_USER_MODE_CONTEXT>

kits20: stdCall _PsTerminateSystemThread, <STATUS_NO_MEMORY>

stdENDP _KiThreadStartup

_TEXT   ends
        end
