     title  "Trap Processing"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    trap.asm
;
; Abstract:
;
;    This module implements the code necessary to field and process i386
;    trap conditions.
;
; Author:
;
;    Shie-Lin Tzong (shielint) 4-Feb-1990
;
; Environment:
;
;    Kernel mode only.
;
; Revision History:
;
;--
.386p
        .xlist
KERNELONLY  equ     1
include ks386.inc
include callconv.inc                    ; calling convention macros
include i386\kimacro.inc
        .list


        page ,132
        extrn   ExpInterlockedPopEntrySListFault:DWORD
        extrn   ExpInterlockedPopEntrySListResume:DWORD
        extrn   _KiHardwareTrigger:dword
        extrn   _KiBugCheckData:dword
        EXTRNP  _KeGetCurrentIrql,0

        EXTRNP  _HalHandleNMI,0
        EXTRNP  HalBeginSystemInterrupt,2,,FASTCALL
        EXTRNP  HalEndSystemInterrupt,1,,FASTCALL
        EXTRNP  _KiDispatchException,4
        EXTRNP  _MmAccessFault,3
        EXTRNP  _KeBugCheck,1
        EXTRNP  _KeBugCheckEx,5
        EXTRNP  _KeTestAlertThread,1
        EXTRNP  _KiContinue,3
        EXTRNP  _KiRaiseException,5
        extrn   _DbgPrint:proc
        EXTRNP  _Ki386CheckDivideByZeroTrap,1
        extrn   SwapContext:near
        extrn   _KiGDT:BYTE
        extrn   _KiPCR:DWORD
        extrn   _KiDoubleFaultTSS:DWORD
ifdef DEVKIT
        EXTRNP  _KdSetOwedBreakpoints
        extrn   _KdpOweBreakpoint:dword
endif

;
; Equates for exceptions which cause system fatal error
;

EXCEPTION_DIVIDED_BY_ZERO       EQU     0
EXCEPTION_DEBUG                 EQU     1
EXCEPTION_NMI                   EQU     2
EXCEPTION_INT3                  EQU     3
EXCEPTION_BOUND_CHECK           EQU     5
EXCEPTION_INVALID_OPCODE        EQU     6
EXCEPTION_NPX_NOT_AVAILABLE     EQU     7
EXCEPTION_DOUBLE_FAULT          EQU     8
EXCEPTION_NPX_OVERRUN           EQU     9
EXCEPTION_INVALID_TSS           EQU     0AH
EXCEPTION_SEGMENT_NOT_PRESENT   EQU     0BH
EXCEPTION_STACK_FAULT           EQU     0CH
EXCEPTION_GP_FAULT              EQU     0DH
EXCEPTION_RESERVED_TRAP         EQU     0FH
EXCEPTION_NPX_ERROR             EQU     010H
EXCEPTION_ALIGNMENT_CHECK       EQU     011H

;
; Exception flags
;

EXCEPT_UNKNOWN_ACCESS           EQU     0H
EXCEPT_LIMIT_ACCESS             EQU     10H

;
; page fault read/write mask
;

ERR_0E_STORE                    EQU     2

;
; EFLAGS single step bit
;

EFLAGS_TF_BIT                   EQU     100h
EFLAGS_OF_BIT                   EQU     4000H

;
;   Force assume into place
;

_TEXT   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:NOTHING, ES:NOTHING, SS:NOTHING, FS:NOTHING, GS:NOTHING
_TEXT   ENDS

_DATA   SEGMENT PARA PUBLIC 'DATA'

;
; Definitions for gate descriptors
;

GATE_TYPE_386INT        EQU     0E00H
GATE_TYPE_386TRAP       EQU     0F00H
GATE_TYPE_TASK          EQU     0500H
D_GATE                  EQU     0
D_PRESENT               EQU     8000H
D_DPL_3                 EQU     6000H
D_DPL_0                 EQU     0

;
; Definitions for present 386 trap and interrupt gate attributes
;

D_TRAP032               EQU     D_PRESENT+D_DPL_0+D_GATE+GATE_TYPE_386TRAP
D_TRAP332               EQU     D_PRESENT+D_DPL_3+D_GATE+GATE_TYPE_386TRAP
D_INT032                EQU     D_PRESENT+D_DPL_0+D_GATE+GATE_TYPE_386INT
D_INT332                EQU     D_PRESENT+D_DPL_3+D_GATE+GATE_TYPE_386INT
D_TASK                  EQU     D_PRESENT+D_DPL_0+D_GATE+GATE_TYPE_TASK

;
;       This is the protected mode interrupt descriptor table.
;

if DBG
BadInterruptMessage db 0ah,7,7,'!!! Unexpected Interrupt %02lx !!!',0ah,00
endif

;++
;
;   DEFINE_SINGLE_EMPTY_VECTOR - helper for DEFINE_EMPTY_VECTORS
;
;--

DEFINE_SINGLE_EMPTY_VECTOR macro    number
IDTEntry    _KiUnexpectedInterrupt&number, D_INT032
_TEXT   SEGMENT
        public  _KiUnexpectedInterrupt&number
_KiUnexpectedInterrupt&number proc
        push    dword ptr (&number)
        jmp     _KiUnexpectedInterruptTail
_KiUnexpectedInterrupt&number endp
_TEXT   ENDS

        endm

FPOFRAME macro a, b
.FPO ( a, b, 0, 0, 0, FPO_TRAPFRAME )
endm

FXSAVE_ESI  macro
    db  0FH, 0AEH, 06
endm

FXSAVE_ECX  macro
    db  0FH, 0AEH, 01
endm

FXRSTOR_ECX macro
    db  0FH, 0AEH, 09
endm

;++
;
;   DEFINE_EMPTY_VECTORS emits an IDTEntry macro (and thus and IDT entry)
;   into the data segment.  It then emits an unexpected interrupt target
;   with push of a constant into the code segment.  Labels in the code
;   segment are defined to bracket the unexpected interrupt targets so
;   that KeConnectInterrupt can correctly test for them.
;
;   Empty vectors will be defined from 30 to 3f, which is the hardware
;   vector set.
;
;--

NUMBER_OF_IDT_VECTOR    EQU     03fH

DEFINE_EMPTY_VECTORS macro

;
;   Set up
;

        empty_vector = 00H

_TEXT   SEGMENT
IFDEF STD_CALL
        public  _KiStartUnexpectedRange@0
_KiStartUnexpectedRange@0   equ     $
ELSE
        public  _KiStartUnexpectedRange
_KiStartUnexpectedRange     equ     $
ENDIF
_TEXT   ENDS

        rept (NUMBER_OF_IDT_VECTOR - (($ - _KiIDT)/8)) + 1

        DEFINE_SINGLE_EMPTY_VECTOR  %empty_vector
        empty_vector = empty_vector + 1

        endm    ;; rept

_TEXT   SEGMENT
IFDEF STD_CALL
        public  _KiEndUnexpectedRange@0
_KiEndUnexpectedRange@0     equ     $
ELSE
        public  _KiEndUnexpectedRange
_KiEndUnexpectedRange       equ     $
ENDIF
_TEXT   ENDS

        endm    ;; DEFINE_EMPTY_VECTORS macro

IDTEntry macro  name,access
        dd      offset FLAT:name
        dw      access
        dw      KGDT_R0_CODE
        endm

IDTEntryEx macro  name,access,sel
        dd      offset FLAT:name
        dw      access
        dw      sel
        endm

ALIGN 8
                public  _KiIDT, _KiIDTLEN, _KiIDTEnd
_KiIDT          label byte

IDTEntry        _KiTrap00, D_INT032             ; 0: Divide Error
IDTEntry        _KiTrap01, D_INT032             ; 1: DEBUG TRAP
IDTEntryEx      _KiTrap02, D_TASK, KGDT_NMI_TSS ; 2: NMI/NPX Error
IDTEntry        _KiTrap03, D_INT332             ; 3: Breakpoint
IDTEntry        _KiTrap04, D_INT332             ; 4: INTO
IDTEntry        _KiTrap05, D_INT032             ; 5: BOUND/Print Screen
IDTEntry        _KiTrap06, D_INT032             ; 6: Invalid Opcode
IDTEntry        _KiTrap07, D_INT032             ; 7: NPX Not Available
IDTEntryEx      _KiTrap08, D_TASK, KGDT_DF_TSS  ; 8: Double Exception
IDTEntry        _KiTrap09, D_INT032             ; 9: NPX Segment Overrun
IDTEntry        _KiTrap0A, D_INT032             ; A: Invalid TSS
IDTEntry        _KiTrap0B, D_INT032             ; B: Segment Not Present
IDTEntry        _KiTrap0C, D_INT032             ; C: Stack Fault
IDTEntry        _KiTrap0D, D_INT032             ; D: General Protection
IDTEntry        _KiTrap0E, D_INT032             ; E: Page Fault
IDTEntry        _KiTrap0F, D_INT032             ; F: Intel Reserved

IDTEntry        _KiTrap10, D_INT032             ;10: 486 coprocessor error
IDTEntry        _KiTrap11, D_INT032             ;11: 486 alignment
IDTEntry        _KiTrap0F, D_INT032             ;12: Machine-Check
IDTEntry        _KiTrap13, D_INT032             ;13: XMMI unmasked numeric exception
IDTEntry        _KiTrap0F, D_INT032             ;14: Intel Reserved
IDTEntry        _KiTrap0F, D_INT032             ;15: Intel Reserved
IDTEntry        _KiTrap0F, D_INT032             ;16: Intel Reserved
IDTEntry        _KiTrap0F, D_INT032             ;17: Intel Reserved

IDTEntry        _KiTrap0F, D_INT032             ;18: Intel Reserved
IDTEntry        _KiTrap0F, D_INT032             ;19: Intel Reserved
IDTEntry        _KiTrap0F, D_INT032             ;1A: Intel Reserved
IDTEntry        _KiTrap0F, D_INT032             ;1B: Intel Reserved
IDTEntry        _KiTrap0F, D_INT032             ;1C: Intel Reserved
IDTEntry        _KiTrap0F, D_INT032             ;1D: Intel Reserved
IDTEntry        _KiTrap0F, D_INT032             ;1E: Intel Reserved
IDTEntry        _KiTrap0F, D_INT032             ;1F: Reserved for APIC

        rept 28H - (($ - _KiIDT)/8)
IDTEntry        0, 0                            ;invalid IDT entry
        endm
IDTEntry        _KiContinueService, D_INT332        ;28: NtContinue service
IDTEntry        _KiRaiseExceptionService, D_INT332  ;29: NtRaiseException service
IDTEntry        0, 0                                ;2A: OBSOLETE: KiGetTickCount service
IDTEntry        0, 0                                ;2B: OBSOLETE: KiCallbackReturn
IDTEntry        0, 0                                ;2C: OBSOLETE: KiSetLowWaitHighThread service
IDTEntry        _KiDebugService,  D_INT332          ;2D: debugger calls
IDTEntry        0, 0                                ;2E: OBSOLETE: system service calls
IDTEntry        _KiTrap0F, D_INT032                 ;2F: Reserved for APIC

;
;   Generate per-vector unexpected interrupt entries for 30 - 3f
;
        DEFINE_EMPTY_VECTORS

_KiIDTLEN       equ     $ - _KiIDT
_KiIDTEnd       equ     $

                public  _KiUnexpectedEntrySize
_KiUnexpectedEntrySize          dd  _KiUnexpectedInterrupt1 - _KiUnexpectedInterrupt0

;
; definition for  floating status word error mask
;

FSW_INVALID_OPERATION   EQU     1
FSW_DENORMAL            EQU     2
FSW_ZERO_DIVIDE         EQU     4
FSW_OVERFLOW            EQU     8
FSW_UNDERFLOW           EQU     16
FSW_PRECISION           EQU     32
FSW_STACK_FAULT         EQU     64
FSW_CONDITION_CODE_0    EQU     100H
FSW_CONDITION_CODE_1    EQU     200H
FSW_CONDITION_CODE_2    EQU     400H
FSW_CONDITION_CODE_3    EQU     4000H
_DATA   ENDS

_TEXT   SEGMENT
        ASSUME  DS:NOTHING, ES:NOTHING, SS:FLAT, FS:NOTHING, GS:NOTHING

        page    ,132
        subttl "NtContinue service"
;++
;
; Routine Description:
;
;    This routine gains control when trap occurs via vector 28H.
;
;    After the trap frame is constructed, control is transferred to
;    NtContinue.  The call will not return.
;
; Arguments:
;
;    eax - ContextRecord parameter to NtContinue.
;    ecx - TestAlert parameter to NtContinue.
;
; Return Value:
;
;    Does not return.
;
;--

align 16
        PUBLIC  _KiContinueService
_KiContinueService proc

        ENTER_SYSCALL
        stdCall _NtContinue, <eax, ecx>

_KiContinueService endp

        page    ,132
        subttl "NtRaiseException service"
;++
;
; Routine Description:
;
;    This routine gains control when trap occurs via vector 29H.
;
;    After the trap frame is constructed, control is transferred to
;    NtRaiseException.  The call will not return.
;
; Arguments:
;
;    eax - ExceptionRecord parameter to NtRaiseException.
;    ecx - ContextRecord parameter to NtRaiseException.
;    edx - FirstChance parameter to NtRaiseException.
;
; Return Value:
;
;    Does not return.
;
;--

align 16
        PUBLIC  _KiRaiseExceptionService
_KiRaiseExceptionService proc

        ENTER_SYSCALL
        stdcall _NtRaiseException, <eax, ecx, edx>

_KiRaiseExceptionService endp

;
;   System service's private version of KiExceptionExit
;   (Also used by KiDebugService)
;
        public  _KiServiceExit
_KiServiceExit:

        cli                                         ; disable interrupts

;
; Exit from SystemService
;

        EXIT_ALL    NoRestoreSegs, NoRestoreVolatile

;++
;
;   _KiServiceExit2 - same as _KiServiceExit BUT the full trap_frame
;       context is restored
;
;--
        public  _KiServiceExit2
_KiServiceExit2:

        cli                             ; disable interrupts

;
; Exit from SystemService
;
        EXIT_ALL                            ; RestoreAll

        page ,132
        subttl  "Common Trap Exit"
;++
;
;   KiExceptionExit
;
;   Routine Description:
;
;       This code is transfered to at the end of the processing for
;       an exception.  Its function is to restore machine state, and
;       continue thread execution.  If control is returning to user mode
;       and there is a user APC pending, then control is transfered to
;       the user APC delivery routine.
;
;       N.B. It is assumed that this code executes at IRQL zero or APC_LEVEL.
;          Therefore page faults and access violations can be taken.
;
;       NOTE: This code is jumped to, not called.
;
;   Arguments:
;
;       (ebp) -> base of trap frame.
;
;   Return Value:
;
;       None.
;
;--
align 4
        public  _KiExceptionExit
_KiExceptionExit proc
.FPO (0, 0, 0, 0, 0, FPO_TRAPFRAME)

        cli                             ; disable interrupts

;
; Exit from Exception
;

        EXIT_ALL    ,,NoPreviousMode

_KiExceptionExit endp


;++
;
;   KiUnexpectedInterruptTail
;
;   Routine Description:
;       This function is jumped to by an IDT entry who has no interrupt
;       handler.
;
;   Arguments:
;
;       (esp) -> base of trap frame.
;       (ebp) -> base of trap frame.
;
;--

        public  _KiUnexpectedInterruptTail
_KiUnexpectedInterruptTail  proc
        ENTER_INTERRUPT PassDwordParm

        inc     dword ptr PCR[PcPrcbData+PbInterruptCount]

        mov     ecx, [ebp]+TsErrCode    ; get IRQ passed through error code
        mov     edx, HIGH_LEVEL
        push    0                       ; make space for OldIrql

        fstCall HalBeginSystemInterrupt

if DBG
        push    dword ptr [ebp]+TsErrCode
        push    offset FLAT:BadInterruptMessage
        call    _DbgPrint               ; display unexpected interrupt message
        add     esp, 8
endif
;
; end this interrupt
;
        INTERRUPT_EXIT

_KiUnexpectedInterruptTail  endp


        page , 132
        subttl "trap processing"

;++
;
; Routine Description:
;
;    _KiTrapxx - protected mode trap entry points
;
;    These entry points are for internally generated exceptions,
;    such as a general protection fault.  They do not handle
;    external hardware interrupts, or user software interrupts.
;
; Arguments:
;
;    On entry the stack looks like:
;
;       [ss]
;       [esp]
;       eflags
;       cs
;       eip
;    ss:sp-> [error]
;
;    The cpu saves the previous SS:ESP, eflags, and CS:EIP on
;    the new stack if there was a privilige transition. If no
;    priviledge level transition occurred, then there is no
;    saved SS:ESP.
;
;    Some exceptions save an error code, others do not.
;
; Return Value:
;
;       None.
;
;--


        page , 132
        subttl "dispatch exception"

;++
;
; CommonDispatchException
;
; Routine Description:
;
;    This routine allocates exception record on stack, sets up exception
;    record using specified parameters and finally sets up arguments
;    and calls _KiDispatchException.
;
;    NOTE:
;
;    The purpose of this routine is to save code space.  Use this routine
;    only if:
;    1. ExceptionRecord is NULL
;    2. ExceptionFlags is 0
;    3. Number of parameters is less or equal than 3.
;
;    Otherwise, you should use DISPATCH_EXCEPTION macro to set up your special
;    exception record.
;
; Arguments:
;
;    (eax) = ExcepCode - Exception code to put into exception record
;    (ebx) = ExceptAddress - Addr of instruction which the hardware exception occurs
;    (ecx) = NumParms - Number of additional parameters
;    (edx) = Parameter1
;    (esi) = Parameter2
;    (edi) = Parameter3
;
; Return Value:
;
;    None.
;
;--
CommonDispatchException0Args:
        xor     ecx, ecx                ; zero arguments
        call    CommonDispatchException

CommonDispatchException1Arg0d:
        xor     edx, edx                ; zero edx
CommonDispatchException1Arg:
        mov     ecx, 1                  ; one argument
        call    CommonDispatchException ; there is no return

CommonDispatchException2Args0d:
        xor     edx, edx                ; zero edx
CommonDispatchException2Args:
        mov     ecx, 2                  ; two arguments
        call    CommonDispatchException ; there is no return

      public CommonDispatchException
align dword
CommonDispatchException proc
cPublicFpo 0, ExceptionRecordLength / 4
;
;       Set up exception record for raising exception
;

        sub     esp, ExceptionRecordLength
                                        ; allocate exception record
        mov     dword ptr [esp]+ErExceptionCode, eax
                                        ; set up exception code
        xor     eax, eax
        mov     dword ptr [esp]+ErExceptionFlags, eax
                                        ; set exception flags
        mov     dword ptr [esp]+ErExceptionRecord, eax
                                        ; set associated exception record
        mov     dword ptr [esp]+ErExceptionAddress, ebx
        mov     dword ptr [esp]+ErNumberParameters, ecx
                                        ; set number of parameters
        cmp     ecx, 0
        je      short de00

        lea     ebx, [esp + ErExceptionInformation]
        mov     [ebx], edx
        mov     [ebx+4], esi
        mov     [ebx+8], edi
de00:
;
; set up arguments and call _KiDispatchException
;

        mov     ecx, esp                ; (ecx)->exception record

; 1 - first chance TRUE
; ebp - trap frame addr
; 0 - Null exception frame
; ecx - exception record addr

        stdCall _KiDispatchException,<ecx, 0, ebp, 1>

        mov     esp, ebp                ; (esp) -> trap frame
        jmp     _KiExceptionExit

CommonDispatchException endp

        page ,132
        subttl "Divide error processing"
;++
;
; Routine Description:
;
;    Handle divide error fault.
;
;    The divide error fault occurs if a DIV or IDIV instructions is
;    executed with a divisor of 0, or if the quotient is too big to
;    fit in the result operand.
;
;    An INTEGER DIVIDED BY ZERO exception will be raised for the fault.
;    If the fault occurs in kernel mode, the system will be terminated.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the faulting instruction.
;    No error code is provided with the divide error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING
align dword
        public  _KiTrap00
_KiTrap00       proc

        push    0                       ; push dummy error code
        ENTER_TRAP

;
; Set up exception record for raising Integer_Divided_by_zero exception
; and call _KiDispatchException
;

if DBG
        test    [ebp]+TsEFlags, EFLAGS_INTERRUPT_MASK   ; faulted with
        jnz     short @f                                ; interrupts disabled?

        xor     eax, eax
        mov     esi, [ebp]+TsEip        ; [esi] = faulting instruction
        stdCall _KeBugCheckEx,<IRQL_NOT_LESS_OR_EQUAL,eax,-1,eax,esi>
@@:
endif

        sti

;
; Flat mode
;
; The intel processor raises a divide by zero expcetion on DIV instruction
; which overflows. To be compatible we other processors we want to
; return overflows as such and not as divide by zero's.  The operand
; on the div instruction is tested to see if it's zero or not.
;
        stdCall _Ki386CheckDivideByZeroTrap,<ebp>
        mov     ebx, [ebp]+TsEip        ; (ebx)-> faulting instruction
        jmp     CommonDispatchException0Args ; Won't return

_KiTrap00       endp

        page ,132
        subttl "Debug Exception"
;++
;
; Routine Description:
;
;    Handle debug exception.
;
;    The processor triggers this exception for any of the following
;    conditions:
;
;    1. Instruction breakpoint fault.
;    2. Data address breakpoint trap.
;    3. General detect fault.
;    4. Single-step trap.
;    5. Task-switch breadkpoint trap.
;
;
; Arguments:
;
;    At entry, the values of saved CS and EIP depend on whether the
;    exception is a fault or a trap.
;    No error code is provided with the divide error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING
align dword
        public  _KiTrap01
_KiTrap01       proc

; Set up machine state frame for displaying

        push    0                       ; push dummy error code
        ENTER_TRAP

;
; Set up exception record for raising single step exception
; and call _KiDispatchException
;

        and     dword ptr [ebp]+TsEflags, not EFLAGS_TF_BIT
        mov     ebx, [ebp]+TsEip                ; (ebx)-> faulting instruction
        mov     eax, STATUS_SINGLE_STEP
        jmp     CommonDispatchException0Args    ; Never return

_KiTrap01       endp

        page ,132
        subttl "Nonmaskable Interrupt"
;++
;
; Routine Description:
;
;    Handle Nonmaskable interrupt.
;
;    An NMI is typically used to signal serious system conditions
;    such as bus time-out, memory parity error, and so on.
;
;    Upon detection of the NMI, the system will be terminated, ie a
;    bugcheck will be raised, no matter what previous mode is.
;
; Arguments:
;
;    No error code is provided with the error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING
align dword
        public  _KiTrap02
_KiTrap02       proc
.FPO (1, 0, 0, 0, 0, 2)
        cli

;
; Clear Nested Task bit in EFLAGS
;
        pushfd
        and     [esp], not 04000h
        popfd

;
; Clear the busy bit in the TSS selector
;
        mov     byte ptr [_KiGDT+KGDT_NMI_TSS+5],089h

;
; Let the HAL have a crack at it before we crash
;
        stdCall _HalHandleNMI

        stdCall _KeBugCheckEx,<UNEXPECTED_KERNEL_MODE_TRAP,2,0,0,0>

_KiTrap02       endp

        page ,132
        subttl "DebugService Breakpoint"
;++
;
; Routine Description:
;
;    Handle INT 2d DebugService
;
;    The trap is caused by an INT 2d.  This is used instead of a
;    BREAKPOINT exception so that parameters can be passed for the
;    requested debug service.  A BREAKPOINT instruction is assumed
;    to be right after the INT 2d - this allows this code to share code
;    with the breakpoint handler.
;
; Arguments:
;     eax - ServiceClass - which call is to be performed
;     ecx - Arg1 - generic first argument
;     edx - Arg2 - generic second argument
;
;--

        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiDebugService
_KiDebugService  proc
        push    0                           ; push dummy error code
        ENTER_TRAP
;       sti                                 ; *NEVER sti here*

        inc     dword ptr [ebp]+TsEip
        mov     eax, [ebp]+TsEax            ; ServiceClass
        mov     ecx, [ebp]+TsEcx            ; Arg1      (already loaded)
        mov     edx, [ebp]+TsEdx            ; Arg2      (already loaded)
        jmp     KiTrap03DebugService

_KiDebugService  endp

        page ,132
        subttl "Single Byte INT3 Breakpoin"
;++
;
; Routine Description:
;
;    Handle INT 3 breakpoint.
;
;    The trap is caused by a single byte INT 3 instruction.  A
;    BREAKPOINT exception with additional parameter indicating
;    READ access is raised for this trap if previous mode is user.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the instruction immediately
;    following the INT 3 instruction.
;    No error code is provided with the error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap03
_KiTrap03       proc
        push    0                       ; push dummy error code
        ENTER_TRAP

   lock inc     ds:_KiHardwareTrigger   ; trip hardware analyzer

        mov     eax, BREAKPOINT_BREAK

KiTrap03DebugService:

;
; Set up exception record and arguments for raising breakpoint exception
;

        mov     esi, ecx                ; ExceptionInfo 2
        mov     edi, edx                ; ExceptionInfo 3
        mov     edx, eax                ; ExceptionInfo 1

        mov     ebx, [ebp]+TsEip
        dec     ebx                     ; (ebx)-> int3 instruction
        mov     ecx, 3
        mov     eax, STATUS_BREAKPOINT
        call    CommonDispatchException ; Never return

_KiTrap03       endp

        page ,132
        subttl "Integer Overflow"
;++
;
; Routine Description:
;
;    Handle INTO overflow.
;
;    The trap occurs when the processor encounters an INTO instruction
;    and the OF flag is set.
;
;    An INTEGER_OVERFLOW exception will be raised for this fault.
;
;    N.B. i386 will not generate fault if only OF flag is set.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the instruction immediately
;    following the INTO instruction.
;    No error code is provided with the error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap04
_KiTrap04       proc

        push    0                       ; push dummy error code
        ENTER_TRAP

;
; set exception record and arguments and call _KiDispatchException
;
        sti
        mov     ebx, [ebp]+TsEip        ; (ebx)-> instr. after INTO
        dec     ebx                     ; (ebx)-> INTO
        mov     eax, STATUS_INTEGER_OVERFLOW
        jmp     CommonDispatchException0Args ; Never return

_KiTrap04       endp

        page ,132
        subttl "Bound Check fault"
;++
;
; Routine Description:
;
;    Handle bound check fault.
;
;    The bound check fault occurs if a BOUND instruction finds that
;    the tested value is outside the specified range.
;
;    For bound check fault, an ARRAY BOUND EXCEEDED exception will be
;    raised.
;    For kernel mode exception, it causes system to be terminated.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the faulting BOUND
;    instruction.
;    No error code is provided with the error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING
align dword
        public  _KiTrap05
_KiTrap05       proc

        push    0                       ; push dummy error code
        ENTER_TRAP

;
; set exception record and arguments and call _KiDispatchException
;
        sti
        mov     ebx, [ebp]+TsEip        ; (ebx)->BOUND instruction
        mov     eax, STATUS_ARRAY_BOUNDS_EXCEEDED
        jmp     CommonDispatchException0Args ; Won't return

_KiTrap05       endp

        page ,132
        subttl "Invalid OP code"
;++
;
; Routine Description:
;
;    Handle invalid op code fault.
;
;    The invalid opcode fault occurs if CS:EIP point to a bit pattern which
;    is not recognized as an instruction by the 386.  This may happen if:
;
;    1. the opcode is not a valid 80386 instruction
;    2. a register operand is specified for an instruction which requires
;       a memory operand
;    3. the LOCK prefix is used on an instruction that cannot be locked
;
;    If fault occurs in USER mode:
;       an Illegal_Instruction exception will be raised
;    if fault occurs in KERNEL mode:
;       system will be terminated.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the first byte of the invalid
;    instruction.
;    No error code is provided with the error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:FLAT, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap06
_KiTrap06       proc

        push    0                       ; Push dummy error code
        ENTER_TRAP

;
; set exception record and arguments and call _KiDispatchException
;
        sti
        mov     ebx, [ebp]+TsEip        ; (ebx)-> invalid instruction
        mov     eax, STATUS_ILLEGAL_INSTRUCTION
        jmp     CommonDispatchException0Args ; Won't return

_KiTrap06       endp

        page ,132
        subttl "Coprocessor Not Avalaible"
;++
;
; Routine Description:
;
;   Handle Coprocessor not avaliable exception.
;
;   If we are REALLY emulating the 80387, the trap 07 vector is edited
;   to point directly at the emulator's entry point.  So this code is
;   only hit when an 80387 DOES exist.
;
;   The current threads coprocessor state is loaded into the
;   coprocessor.  If the coprocessor has a different threads state
;   in it (UP only) it is first saved away.  The thread is then continued.
;   Note: the threads state may contian the TS bit - In this case the
;   code loops back to the top of the Trap07 handler.  (which is where
;   we would end up if we let the thread return to user code anyway).
;
;   If the threads NPX context is in the coprocessor and we hit a Trap07
;   there is an NPX error which needs to be processed.  If the trap was
;   from usermode the error is dispatched.  If the trap was from kernelmode
;   the error is remembered, but we clear CR0 so the kernel code can
;   continue.  We can do this because the kernel mode code will restore
;   CR0 (and set TS) to signal a delayed error for this thread.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the first byte of the faulting
;    instruction.
;    No error code is provided with the error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap07
_KiTrap07       proc

        push    0                       ; push dummy error code
        ENTER_TRAP
Kt0700:
        mov     eax, PCR[PcPrcbData+PbCurrentThread]
        mov     ecx, PCR[PcStackBase] ; (ecx) -> top of kernel stack
        cli                             ; don't context switch

;
; Bugcheck if attempting to use the FPU from a DPC without first calling
; KeSaveFloatingPointState.
;

if DBG
        cmp     PCR[PcPrcbData.PbDpcRoutineActive],0
        jne     Kt07110
endif

Kt0701: cmp     byte ptr [eax].ThNpxState, NPX_STATE_LOADED
        je      Kt0715

;
; Trap occured and this threads NPX state is not loaded.  Load it now
; and resume the application.  If someone elses state is in the coprocessor
; (uniprocessor implementation only) then save it first.
;

        mov     ebx, cr0
        and     ebx, NOT (CR0_MP+CR0_TS+CR0_EM)
        mov     cr0, ebx                ; allow frstor (& fnsave) to work

Kt0702:
        mov     edx, PCR[PcPrcbData+PbNpxThread] ; Owner of NPX state
        or      edx, edx                ; NULL?
        jz      Kt0704                  ; Yes - skip save

;
; Due to an hardware errata we need to know that the coprocessor
; doesn't generate an error condition once interrupts are disabled and
; trying to perform an fnsave which could wait for the error condition
; to be handled.
;
; The fix for this errata is that we "know" that the coprocessor is
; being used by a different thread then the one which may have caused
; the error condition.  The round trip time to swap to a new thread
; is longer then ANY floating point instruction.  We therefore know
; that any possible coprocessor error has already occured and been
; handled.
;
        mov     esi,[edx].ThStackBase
        sub     esi, NPX_FRAME_LENGTH   ; Space for NPX_FRAME

        FXSAVE_ESI
        mov     byte ptr [edx].ThNpxState, NPX_STATE_NOT_LOADED
Kt0704:

;
; Load current threads coprocessor state into the coprocessor
;
; (eax) - CurrentThread
; (ecx) - CurrentThreads NPX save area
; (ebx) - CR0
; (ebp) - trap frame
; Interrupts disabled
;

        FXRSTOR_ECX                     ; reload NPX context

Kt0704c:
        mov     byte ptr [eax].ThNpxState, NPX_STATE_LOADED
        mov     PCR[PcPrcbData+PbNpxThread], eax  ; owner of coprocessors state

        sti                             ; Allow interrupts & context switches
        nop                             ; sti needs one cycle

        cmp     dword ptr [ecx].FpCr0NpxState, 0
        jz      _KiExceptionExit        ; nothing to set, skip CR0 reload

;
; Note: we have to get the CR0 value again to insure that we have the
;       correct state for TS.  We may have context switched since
;       the last move from CR0, and our npx state may have been moved off
;       of the npx.
;
        cli
if DBG
        test    dword ptr [ecx].FpCr0NpxState, NOT (CR0_MP+CR0_EM+CR0_TS)
        jnz short Kt07dbg1
endif
        mov     ebx,CR0
        or      ebx, [ecx].FpCr0NpxState
        mov     cr0, ebx                ; restore threads CR0 NPX state
        sti
        test    ebx, CR0_TS             ; Setting TS?  (delayed error)
        jz      _KiExceptionExit        ; No - continue

        jmp     Kt0700                  ; Dispatch delayed exception
if DBG
Kt07dbg1:    int 3
Kt07dbg2:    int 3
Kt07dbg3:    int 3
        sti
        jmp short $-2
endif

;
; A Trap07 or Trap10 has occured from a ring 0 ESCAPE instruction.  This
; may occur when trying to load the coprocessors state.  These
; code paths rely on Cr0NpxState to signal a delayed error (not CR0) - we
; set CR0_TS in Cr0NpxState to get a delayed error, and make sure CR0 CR0_TS
; is not set so the R0 ESC instruction(s) can complete.
;
; (ecx) - CurrentThreads NPX save area
; (ebp) - trap frame
; Interrupts disabled
;

Kt0710:
        mov     eax, PCR[PcPrcbData+PbCurrentThread]
        mov     ecx, PCR[PcStackBase] ; (ecx) -> top of kernel stack

Kt0715:
if DBG
        mov     eax, cr0                    ; Did we fault because some bit in CR0
        test    eax, (CR0_TS+CR0_MP+CR0_EM)
        jnz     short Kt07dbg3
endif

        or      dword ptr [ecx].FpCr0NpxState, CR0_TS   ; signal a delayed error

Kt0716: mov     eax, PCR[PcPrcbData+PbCurrentThread]
        mov     ecx, PCR[PcStackBase] ; (ecx) -> top of kernel stack

Kt0720:
;
; Some type of coprocessor exception has occured for the current thread.
;
; (eax) - CurrentThread
; (ecx) - CurrentThreads NPX save area
; (ebp) - TrapFrame
; Interrupts disabled
;
        mov     ebx, cr0
        and     ebx, NOT (CR0_MP+CR0_EM+CR0_TS)
        mov     cr0, ebx                ; Clear MP+TS+EM to do fnsave & fwait

;
; Save the faulting state so we can inspect the cause of the floating
; point fault
;

        FXSAVE_ECX

if DBG
        test    dword ptr [ecx].FpCr0NpxState, NOT (CR0_MP+CR0_EM+CR0_TS)
        jnz     Kt07dbg2
endif
        or      ebx, NPX_STATE_NOT_LOADED
        or      ebx,[ecx]+FpCr0NpxState ; restore this threads CR0 NPX state
        mov     cr0, ebx                ; set TS so next ESC access causes trap

;
; Clear TS bit in Cr0NpxFlags in case it was set to trigger this trap.
;
        and     dword ptr [ecx].FpCr0NpxState, NOT CR0_TS

;
; The state is no longer in the coprocessor.  Clear ThNpxState and
; re-enable interrupts to allow context switching.
;
        mov     byte ptr [eax].ThNpxState, NPX_STATE_NOT_LOADED
        mov     dword ptr PCR[PcPrcbData+PbNpxThread], 0  ; No state in coprocessor
        sti

;
; According to the floating error priority, we test what is the cause of
; the NPX error and raise an appropriate exception.
;

        mov     ebx, [ecx] + FxErrorOffset
        movzx   eax, word ptr [ecx] + FxControlWord
        movzx   edx, word ptr [ecx] + FxStatusWord
        mov     esi, [ecx] + FxDataOffset ; (esi) = operand addr

        and     eax, FSW_INVALID_OPERATION + FSW_DENORMAL + FSW_ZERO_DIVIDE + FSW_OVERFLOW + FSW_UNDERFLOW + FSW_PRECISION
        not     eax                        ; ax = mask of enabled exceptions
        and     eax, edx
        test    eax, FSW_INVALID_OPERATION ; Is it an invalid op exception?
        jz      short Kt0740               ; if z, no, go Kt0740
        test    eax, FSW_STACK_FAULT       ; Is it caused by stack fault?
        jnz     short Kt0730               ; if nz, yes, go Kt0730

; Raise Floating reserved operand exception
;

        mov     eax, STATUS_FLOAT_INVALID_OPERATION
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt0730:
;
; Raise Access Violation exception for stack overflow/underflow
;

        mov     eax, STATUS_FLOAT_STACK_CHECK
        jmp     CommonDispatchException2Args0d ; Won't return

Kt0740:

; Check for floating zero divide exception

        test    eax, FSW_ZERO_DIVIDE    ; Is it a zero divide error?
        jz      short Kt0750            ; if z, no, go Kt0750

; Raise Floating divided by zero exception

        mov     eax, STATUS_FLOAT_DIVIDE_BY_ZERO
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt0750:

; Check for denormal error

        test    eax, FSW_DENORMAL       ; Is it a denormal error?
        jz      short Kt0760            ; if z, no, go Kt0760

; Raise floating reserved operand exception

        mov     eax, STATUS_FLOAT_INVALID_OPERATION
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt0760:

; Check for floating overflow error

        test    eax, FSW_OVERFLOW       ; Is it an overflow error?
        jz      short Kt0770            ; if z, no, go Kt0770

; Raise floating overflow exception

        mov     eax, STATUS_FLOAT_OVERFLOW
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt0770:

; Check for floating underflow error

        test    eax, FSW_UNDERFLOW      ; Is it a underflow error?
        jz      short Kt0780            ; if z, no, go Kt0780

; Raise floating underflow exception

        mov     eax, STATUS_FLOAT_UNDERFLOW
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt0780:

; Check for precision (IEEE inexact) error

        test    eax, FSW_PRECISION      ; Is it a precision error
        jz      short Kt07100           ; if z, no, go Kt07100

        mov     eax, STATUS_FLOAT_INEXACT_RESULT
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt07100:
        sti                             ; stop the system
        stdCall _KeBugCheck, <TRAP_CAUSE_UNKNOWN>

if DBG
Kt07110:
        cmp     byte ptr [eax].ThNpxIrql, DISPATCH_LEVEL
        je      Kt0701
        stdCall _KeBugCheck, <ATTEMPTED_FPU_USE_FROM_DPC>
endif

_KiTrap07       endp


        page ,132
        subttl "Double Fault"
;++
;
; Routine Description:
;
;    Handle double exception fault.
;
;    Normally, when the processor detects an exception while trying to
;    invoke the handler for a prior exception, the two exception can be
;    handled serially.  If, however, the processor cannot handle them
;    serially, it signals the double-fault exception instead.
;
;    If double exception is detected, no matter previous mode is USER
;    or kernel, a bugcheck will be raised and the system will be terminated.
;
; Arguments:
;
;    error code, which is always zero, is pushed on stack.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING
align dword
        public  _KiTrap08
_KiTrap08       proc
.FPO (0, 0, 0, 0, 0, 2)

        cli

ifdef DEVKIT
;
; If the debugger can handle a double fault now, then we do so
;
        lea     edi, PCR[PcPrcbData+PbDebugDoubleFault]
        mov     eax, [edi]
        test    eax, eax
        je      Kt0810

;
; we're only going to take one double fault at a time
;
        xor     edx, edx
        mov     [edi], edx

;
; find the old TSS
;
        mov     dx, word ptr [_KiDoubleFaultTSS]
        lea     ecx, _KiGDT
        mov     edi, [edx+ecx+2]
        push    edi
        mov     dl, [edx+ecx+7]
        mov     [esp+3], dl

;
; ask the debugger whether we can handle things
;
        call    eax
        test    al, al
        je      Kt0810

;
; we're good to go
;
        iretd
        jmp     _KiTrap08   ; in case we dbl-fault again
endif ; DEVKIT

Kt0810:
;
; Clear the busy bit in the TSS selector
;
        mov     byte ptr [_KiGDT+KGDT_DF_TSS+5],089h

;
; Clear Nested Task bit in EFLAGS
;
        pushfd
        and     [esp], not 04000h
        popfd

;
; The original machine context is in original task's TSS
;
@@:     stdCall _KeBugCheckEx,<UNEXPECTED_KERNEL_MODE_TRAP,8,0,0,0>
        jmp     short @b        ; do not remove - for debugger

_KiTrap08       endp

        page ,132
        subttl "Coprocessor Segment Overrun"
;++
;
; Routine Description:
;
;    Handle Coprocessor Segment Overrun exception.
;
;    This exception only occurs on the 80286 (it's a trap 0d on the 80386),
;    so choke if we get here.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the aborted instruction.
;    No error code is provided with the error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap09
_KiTrap09       proc

        push    0                       ; push dummy error code
        ENTER_TRAP

        sti
        mov     eax, EXCEPTION_NPX_OVERRUN ; (eax) = exception type
        jmp     _KiSystemFatalException ; go terminate the system

_KiTrap09       endp

        page ,132
        subttl "Invalid TSS exception"
;++
;
; Routine Description:
;
;    Handle Invalid TSS fault.
;
;    This exception occurs if a segment exception other than the
;    not-present exception is detected when loading a selector
;    from the TSS.
;
;    If the exception is caused as a result of the kernel, device
;    drivers, or user incorrectly setting the NT bit in the flags
;    while the back-link selector in the TSS is invalid and the
;    IRET instruction being executed, in this case, this routine
;    will clear the NT bit in the trap frame and restart the iret
;    instruction.  For other causes of the fault, the user process
;    will be terminated if previous mode is user and the system
;    will stop if the exception occurs in kernel mode.  No exception
;    is raised.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the faulting instruction or
;    the first instruction of the task if the fault occurs as part of
;    a task switch.
;    Error code containing the segment causing the exception is provided.
;
;    NT386 does not use TSS for context switching.  So, the invalid tss
;    fault should NEVER occur.  If it does, something is wrong with
;    the kernel.  We simply shutdown the system.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap0A
_KiTrap0A       proc

        ENTER_TRAP

        sti
        mov     eax, EXCEPTION_INVALID_TSS ; (eax) = trap type
        jmp     _KiSystemFatalException ; go terminate the system

_KiTrap0A       endp

        page ,132
        subttl "Segment Not Present"
;++
;
; Routine Description:
;
;    Handle Segment Not Present fault.
;
;    This exception occurs when the processor finds the P bit 0
;    when accessing an otherwise valid descriptor that is not to
;    be loaded in SS register.
;
;    The only place the fault can occur (in kernel mode) is Trap/Exception
;    exit code.  Otherwise, this exception causes system to be terminated.
;    NT386 uses flat mode, the segment not present fault in Kernel mode
;    indicates system malfunction.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the faulting instruction or
;    the first instruction of the task if the fault occurs as part of
;    a task switch.
;    Error code containing the segment causing the exception is provided.
;
; Return value:
;
;    None
;
;--
        page ,132
        subttl "Stack segment fault"
;++
;
; Routine Description:
;
;    Handle Stack Segment fault.
;
;    This exception occurs when the processor detects certain problem
;    with the segment addressed by the SS segment register:
;
;    1. A limit violation in the segment addressed by the SS (error
;       code = 0)
;    2. A limit vioalation in the inner stack during an interlevel
;       call or interrupt (error code = selector for the inner stack)
;    3. If the descriptor to be loaded into SS has its present bit 0
;       (error code = selector for the not-present segment)
;
;    The exception should never occurs in kernel mode except when we
;    perform the iret back to user mode.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the faulting instruction or
;    the first instruction of the task if the fault occurs as part of
;    a task switch.
;    Error code (whose value depends on detected condition) is provided.
;
; Return value:
;
;    None
;
;--
        page ,132
        subttl "General Protection Fault"
;++
;
; Routine Description:
;
;    Handle General protection fault.
;
;    First, check to see if the fault occured in kernel mode with
;    incorrect selector values.  If so, this is a lazy segment load.
;    Correct the selector values and restart the instruction.  Otherwise,
;    parse out various kinds of faults and report as exceptions.
;
;    All protection violations that do not cause another exception
;    cause a general exception.  If the exception indicates a violation
;    of the protection model by an application program executing a
;    previleged instruction or I/O reference, a PRIVILEGED INSTRUCTION
;    exception will be raised.  All other causes of general protection
;    fault cause a ACCESS VIOLATION exception to be raised.
;
;    If previous mode = Kernel;
;        the system will be terminated  (assuming not lazy segment load)
;    Else previous mode = USER
;        the process will be terminated if the exception was not caused
;        by privileged instruction.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the faulting instruction or
;    the first instruction of the task if the fault occurs as part of
;    a task switch.
;    Error code (whose value depends on detected condition) is provided.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap0B
_KiTrap0B       proc
_KiTrap0C       label byte
_KiTrap0D       label byte

; Set up machine state frame for displaying

        ENTER_TRAP

        sti
        mov     ebx, [ebp]+TsEip        ; (ebx)->faulting instruction
        mov     esi, [ebp]+TsErrCode
        mov     eax, STATUS_ACCESS_VIOLATION
        jmp     CommonDispatchException2Args0d ; Won't return

_KiTrap0B       endp

        page ,132
        subttl "Page fault processing"
;++
;
; Routine Description:
;
;    Handle page fault.
;
;    The page fault occurs if paging is enabled and any one of the
;    conditions is true:
;
;    1. page not present
;    2. the faulting procedure does not have sufficient privilege to
;       access the indicated page.
;
;    For case 1, the referenced page will be loaded to memory and
;    execution continues.
;    For case 2, registered exception handler will be invoked with
;    appropriate error code (in most cases STATUS_ACCESS_VIOLATION)
;
;    N.B. It is assumed that no page fault is allowed during task
;    switches.
;
;    N.B. INTERRUPTS MUST REMAIN OFF UNTIL AFTER CR2 IS CAPTURED.
;
; Arguments:
;
;    Error code left on stack.
;    CR2 contains faulting address.
;    Interrupts are turned off at entry by use of an interrupt gate.
;
; Return value:
;
;    None
;
;--

        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING
align dword
        public  _KiTrap0E
_KiTrap0E       proc

        ENTER_TRAP

        mov     edi,cr2
        sti

        test    [ebp]+TsEFlags, EFLAGS_INTERRUPT_MASK   ; faulted with
        jz      Kt0e12b                 ; interrupts disabled?
Kt0e01:

;
; call _MmAccessFault to page in the not present page.  If the cause
; of the fault is 2, _MmAccessFault will return approriate error code
;

        sub     esp, 12
        mov     [esp+8], ebp            ; pass in the trap frame base
        mov     [esp+4], edi
        mov     eax, [ebp]+TsErrCode    ; (eax)= error code
        and     eax, ERR_0E_STORE       ; (eax)= 0 if fault caused by read
                                        ;      = 2 if fault caused by write
        shr     eax, 1                  ; (eax) = 0 if read fault, 1 if write fault
        mov     [esp+0], eax            ; arg3: load/store indicator

        call    _MmAccessFault@12

        or      eax, eax                ; sucessful?
        jge     Kt0e10                  ; yes, go exit

;
; Check to determine if the fault occured in the interlocked pop entry slist
; code. There is a case where a fault may occur in this code when the right
; set of circumstances occurs. The fault can be ignored by simply skipping
; the faulting instruction.
;

Kt0e05: mov     ecx, offset FLAT:ExpInterlockedPopEntrySListFault ; get pop code address
        cmp     [ebp].TsEip, ecx        ; check if fault at pop code address
        je      Kt0e10a                 ; if eq, skip faulting instruction

        mov     ecx, [ebp]+TsErrCode    ; (ecx) = error code
        and     ecx, ERR_0E_STORE       ; (ecx) = 0 if fault caused by read
                                        ;         2 if fault caused by write
        shr     ecx,1                   ; (ecx) = load/store indicator

;
; Set up exception record and arguments and call _KiDispatchException
;

        mov     esi, [ebp]+TsEip        ; (esi)-> faulting instruction

        cmp     eax, STATUS_ACCESS_VIOLATION ; dispatch access violation or
        je      short Kt0e9b                 ; or in_page_error?

        cmp     eax, STATUS_GUARD_PAGE_VIOLATION
        je      short Kt0e9b

        cmp     eax, STATUS_STACK_OVERFLOW
        je      short Kt0e9b

;
; test to see if davec's reserved status code bit is set. If so, then bugchecka
;

        cmp     eax, STATUS_IN_PAGE_ERROR or 10000000h
        je      Kt0e12                  ; bugchecka

;
; (ecx) = ExceptionInfo 1
; (edi) = ExceptionInfo 2
; (eax) = ExceptionInfo 3
; (esi) -> Exception Addr
;

        mov     edx, ecx
        mov     ebx, esi
        mov     esi, edi
        mov     ecx, 3
        mov     edi, eax
        mov     eax, STATUS_IN_PAGE_ERROR
        call    CommonDispatchException ; Won't return

Kt0e9b:
        mov     ebx, esi
        mov     edx, ecx
        mov     esi, edi
        jmp     CommonDispatchException2Args ; Won't return

.FPO ( 0, 0, 0, 0, 0, FPO_TRAPFRAME )

;
; The fault occured in the interlocked pop slist function and the faulting
; instruction should be skipped.
;

Kt0e10a:mov     ecx, offset FLAT:ExpInterlockedPopEntrySListResume ; get resume address
        mov     [ebp].TsEip, ecx        ; set continuation address

Kt0e10:

ifdef DEVKIT
        mov     esp,ebp                 ; (esp) -> trap frame
        test    _KdpOweBreakpoint, 1    ; do we have any owed breakpoints?
        jz      _KiExceptionExit        ; No, all done

        stdCall _KdSetOwedBreakpoints   ; notify the debugger
endif

Kt0e11: mov     esp,ebp                 ; (esp) -> trap frame
        jmp     _KiExceptionExit        ; join common code

Kt0e12:
        stdCall _KeGetCurrentIrql       ; (eax) = OldIrql
Kt0e12a:
   lock inc     ds:_KiHardwareTrigger   ; trip hardware analyzer

;
; bugcheck a, addr, irql, load/store, pc
;
        mov     ecx, [ebp]+TsErrCode    ; (ecx)= error code
        and     ecx, ERR_0E_STORE       ; (ecx)= 0 if fault caused by read
        shr     ecx, 1                  ; (ecx) = 0 if read fault, 1 if write fault

        mov     esi, [ebp]+TsEip        ; [esi] = faulting instruction

        stdCall _KeBugCheckEx,<IRQL_NOT_LESS_OR_EQUAL,edi,eax,ecx,esi>

Kt0e12b:
        cmp     _KiBugCheckData, 0      ; If crashed, handle trap in
        jnz     Kt0e01                  ; normal manner


        mov     eax, 0ffh               ; OldIrql = -1
        jmp     short Kt0e12a

_KiTrap0E       endp

        page ,132
        subttl "Trap0F -- Intel Reserved"
;++
;
; Routine Description:
;
;    The trap 0F should never occur.  If, however, the exception occurs in
;    USER mode, the current process will be terminated.  If the exception
;    occurs in KERNEL mode, a bugcheck will be raised.  NO registered
;    handler, if any, will be inviked to handle the exception.
;
; Arguments:
;
;    None
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap0F
_KiTrap0F       proc

        push    0                       ; push dummy error code
        ENTER_TRAP
        sti

        mov     eax, EXCEPTION_RESERVED_TRAP ; (eax) = trap type
        jmp     _KiSystemFatalException ; go terminate the system

_KiTrap0F       endp


        page ,132
        subttl "Coprocessor Error"

;++
;
; Routine Description:
;
;    Handle Coprocessor Error.
;
;    This exception is used on 486 or above only.  For i386, it uses
;    IRQ 13 instead.
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the aborted instruction.
;    No error code is provided with the error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap10
_KiTrap10       proc

        push    0                       ; push dummy error code
        ENTER_TRAP

        mov     eax, PCR[PcPrcbData+PbNpxThread]  ; Correct context for fault?
        cmp     eax, PCR[PcPrcbData+PbCurrentThread]
        je      Kt0710                  ; Yes - go try to dispatch it

;
; We are in the wrong NPX context and can not dispatch the exception right now.
; Set up the target thread for a delay exception.
;
; Note: we don't think this is a possible case, but just to be safe...
;
        mov     eax, [eax].ThStackBase
        sub     eax, NPX_FRAME_LENGTH   ; Space for NPX_FRAME
        or      dword ptr [eax].FpCr0NpxState, CR0_TS   ; Set for delayed error

        jmp     _KiExceptionExit

_KiTrap10       endp

        page ,132
        subttl "Alignment fault"
;++
;
; Routine Description:
;
;    Handle alignment faults.
;
;    This exception occurs when an unaligned data access is made by a thread
;    with alignment checking turned on.
;
;    This exception will only occur on 486 machines.  The 386 will not do
;    any alignment checking.  Only threads which have the appropriate bit
;    set in EFLAGS will generate alignment faults.
;
;    The exception will never occur in kernel mode.  (hardware limitation)
;
; Arguments:
;
;    At entry, the saved CS:EIP point to the faulting instruction.
;    Error code is provided.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap11
_KiTrap11       proc

        ENTER_TRAP
        sti

;
; We should never be here, since the 486 will not generate alignment faults
; in kernel mode.
;
        mov     eax, EXCEPTION_ALIGNMENT_CHECK      ; (eax) = trap type
        jmp     _KiSystemFatalException

_KiTrap11       endp

;++
;
; Routine Description:
;
;    Handle XMMI Exception.
;;
; Arguments:
;
;    At entry, the saved CS:EIP point to the aborted instruction.
;    No error code is provided with the error.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:NOTHING, SS:NOTHING, ES:NOTHING

align dword
        public  _KiTrap13
_KiTrap13       proc

        push    0                       ; push dummy error code
        ENTER_TRAP

        mov     eax, PCR[PcPrcbData+PbNpxThread]  ; Correct context for fault?
        cmp     eax, PCR[PcPrcbData+PbCurrentThread]
        je      Kt13_10                 ; Yes - go try to dispatch it

;
;       Katmai New Instruction exceptions are precise and occur immediately.
;       if we are in the wrong NPX context, bugcheck the system.
;
        ; stop the system
        stdCall _KeBugCheckEx,<TRAP_CAUSE_UNKNOWN,13,eax,0,0>

Kt13_10:
        mov     ecx, PCR[PcStackBase] ; (ecx) -> top of kernel stack

;
;       TrapFrame is built by ENTER_TRAP.
;       XMMI are accessible from all IA execution modes:
;       Protected Mode, Real address mode, Virtual 8086 mode
;
Kt13_15:
;
; We are about to dispatch a XMMI floating point exception to user mode.
;
; (ebp) - Trap frame
; (ecx) - CurrentThreads NPX save area (PCR[PcStackBase])
; (eax) - CurrentThread

; Dispatch
Kt13_20:
;
; Some type of coprocessor exception has occured for the current thread.
;
; Interrupts disabled
;
        mov     ebx, cr0
        and     ebx, NOT (CR0_MP+CR0_EM+CR0_TS)
        mov     cr0, ebx                ; Clear MP+TS+EM to do fxsave

;
; Save the faulting state so we can inspect the cause of the floating
; point fault
;
        FXSAVE_ECX

if DBG
        test    dword ptr [ecx].FpCr0NpxState, NOT (CR0_MP+CR0_EM+CR0_TS)
        jnz     Kt13_dbg2
endif

        or      ebx, NPX_STATE_NOT_LOADED ; CR0_TS | CR0_MP
        or      ebx,[ecx]+FpCr0NpxState ; restore this threads CR0 NPX state
        mov     cr0, ebx                ; set TS so next ESC access causes trap

;
; Clear TS bit in Cr0NpxFlags in case it was set to trigger this trap.
;
        and     dword ptr [ecx].FpCr0NpxState, NOT CR0_TS

;
; The state is no longer in the coprocessor.  Clear ThNpxState and
; re-enable interrupts to allow context switching.
;
        mov     byte ptr [eax].ThNpxState, NPX_STATE_NOT_LOADED
        mov     dword ptr PCR[PcPrcbData+PbNpxThread], 0  ; No state in coprocessor
        sti

; (eax) = ExcepCode - Exception code to put into exception record
; (ebx) = ExceptAddress - Addr of instruction which the hardware exception occurs
; (ecx) = NumParms - Number of additional parameters
; (edx) = Parameter1
; (esi) = Parameter2
; (edi) = Parameter3
        mov     ebx, [ebp].TsEip          ; Eip is from trap frame, not from FxErrorOffset
        movzx   eax, word ptr [ecx] + FxMXCsr
        mov     edx, eax
        shr     edx, 7                    ; get the mask
        not     edx
        mov     esi, 0                    ; (esi) = operand addr, addr is computed from
                                          ; trap frame, not from FxDataOffset
;
;       Exception will be handled in user's handler if there is one declared.
;
        and     eax, FSW_INVALID_OPERATION + FSW_DENORMAL + FSW_ZERO_DIVIDE + FSW_OVERFLOW + FSW_UNDERFLOW + FSW_PRECISION
        and     eax, edx
        test    eax, FSW_INVALID_OPERATION ; Is it an invalid op exception?
        jz      short Kt13_40              ; if z, no, go Kt13_40
;
; Invalid Operation Exception - Invalid arithmetic operand
; Raise exception
;
        mov     eax, STATUS_FLOAT_MULTIPLE_TRAPS
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt13_40:
; Check for floating zero divide exception
;
        test    eax, FSW_ZERO_DIVIDE    ; Is it a zero divide error?
        jz      short Kt13_50           ; if z, no, go Kt13_50
;
; Division-By-Zero Exception
; Raise exception
;
        mov     eax, STATUS_FLOAT_MULTIPLE_TRAPS
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt13_50:
; Check for denormal error
;
        test    eax, FSW_DENORMAL       ; Is it a denormal error?
        jz      short Kt13_60           ; if z, no, go Kt13_60
;
; Denormal Operand Excpetion
; Raise exception
;
        mov     eax, STATUS_FLOAT_MULTIPLE_TRAPS
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt13_60:
; Check for floating overflow error
;
        test    eax, FSW_OVERFLOW       ; Is it an overflow error?
        jz      short Kt13_70           ; if z, no, go Kt13_70
;
; Numeric Overflow Exception
; Raise exception
;
        mov     eax, STATUS_FLOAT_MULTIPLE_FAULTS
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt13_70:
; Check for floating underflow error
;
        test    eax, FSW_UNDERFLOW      ; Is it a underflow error?
        jz      short Kt13_80           ; if z, no, go Kt13_80
;
; Numeric Underflow Exception
; Raise exception
;
        mov     eax, STATUS_FLOAT_MULTIPLE_FAULTS
        jmp     CommonDispatchException1Arg0d ; Won't return

Kt13_80:
; Check for precision (IEEE inexact) error
;
        test    eax, FSW_PRECISION      ; Is it a precision error
        jz      short Kt13_100          ; if z, no, go Kt13_100
;
; Inexact-Result (Precision) Exception
; Raise exception
;
        mov     eax, STATUS_FLOAT_MULTIPLE_FAULTS
        jmp     CommonDispatchException1Arg0d ; Won't return

; Huh?
Kt13_100:
; If status word does not indicate error, then something is wrong...
; (Note: that we have done a sti, before the status is examined)
        sti
; stop the system
        stdCall _KeBugCheckEx,<TRAP_CAUSE_UNKNOWN,13,eax,0,1>

if DBG
Kt13_dbg1:    int 3
Kt13_dbg2:    int 3
Kt13_dbg3:    int 3
        sti
        jmp short $-2
endif

_KiTrap13       endp

;++
;
; VOID
; KiFlushNPXState (
;     VOID
;     )
;
; Routine Description:
;
;   When a threads NPX context is requested (most likely by a debugger)
;   this function is called to flush the threads NPX context out of the
;   compressor if required.
;
; Arguments:
;
;    None.
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:FLAT, SS:NOTHING, ES:NOTHING
align dword

cPublicProc _KiFlushNPXState    ,0
cPublicFpo 1, 1

        pushfd
        cli                             ; don't context switch

        mov     edx, PCR[PcPrcbData+PbCurrentThread]

        cmp     byte ptr [edx].ThNpxState, NPX_STATE_LOADED
        jne     short fnpx70

fnpx20:
    ;
    ; Current thread has NPX state in the coprocessor, flush it
    ;
        mov     eax, cr0
        test    eax, CR0_MP+CR0_TS+CR0_EM
        jz      short fnpx30
        and     eax, NOT (CR0_MP+CR0_TS+CR0_EM)
        mov     cr0, eax                ; allow frstor (& fnsave) to work
fnpx30:
        mov     ecx, PCR[PcStackBase]   ; (ecx) -> top of kernel stack
        FXSAVE_ECX

        mov     byte ptr [edx].ThNpxState, NPX_STATE_NOT_LOADED
        mov     PCR[PcPrcbData+PbNpxThread], 0      ; clear npx owner

        or      eax, NPX_STATE_NOT_LOADED           ; or in new threads cr0
        or      eax, [ecx].FpCr0NpxState            ; merge new thread setable state
        mov     cr0, eax

fnpx70:
        popfd                           ; enable interrupts
        stdRET    _KiFlushNPXState

stdENDP _KiFlushNPXState

;++
;
; VOID
; KiSetHardwareTrigger (
;     VOID
;     )
;
; Routine Description:
;
;   This function sets KiHardwareTrigger such that an analyzer can sniff
;   for this access.   It needs to occur with a lock cycle such that
;   the processor won't speculatively read this value.   Interlocked
;   functions can't be used as in a UP build they do not use a
;   lock prefix.
;
; Arguments:
;
;    None
;
; Return value:
;
;    None
;
;--
        ASSUME  DS:FLAT, SS:NOTHING, ES:NOTHING
cPublicProc _KiSetHardwareTrigger,0
   lock inc     ds:_KiHardwareTrigger   ; trip hardware analyzer
        stdRet  _KiSetHardwareTrigger
stdENDP _KiSetHardwareTrigger


        page ,132
        subttl "Processing System Fatal Exceptions"
;++
;
; Routine Description:
;
;    This routine processes the system fatal exceptions.
;    The machine state and trap type will be displayed and
;    System will be stopped.
;
; Arguments:
;
;    (eax) = Trap type
;    (ebp) -> machine state frame
;
; Return value:
;
;    system stopped.
;
;--
        assume  ds:nothing, es:nothing, ss:nothing, fs:nothing, gs:nothing

align dword
        public  _KiSystemFatalException
_KiSystemFatalException proc
.FPO (0, 0, 0, 0, 0, FPO_TRAPFRAME)

        stdCall _KeBugCheckEx,<UNEXPECTED_KERNEL_MODE_TRAP, eax, 0, 0, 0>
        ret

_KiSystemFatalException endp

        page
        subttl  "Continue Execution System Service"
;++
;
; NTSTATUS
; NtContinue (
;    IN PCONTEXT ContextRecord,
;    IN BOOLEAN TestAlert
;    )
;
; Routine Description:
;
;    This routine is called as a system service to continue execution after
;    an exception has occurred. Its function is to transfer information from
;    the specified context record into the trap frame that was built when the
;    system service was executed, and then exit the system as if an exception
;    had occurred.
;
;   WARNING - Do not call this routine directly, always call it as
;             ZwContinue!!!  This is required because it needs the
;             trapframe built by KiSystemService.
;
; Arguments:
;
;    KTrapFrame (ebp+0: after setup) -> base of KTrapFrame
;
;    ContextRecord (ebp+8: after setup) = Supplies a pointer to a context rec.
;
;    TestAlert (esp+12: after setup) = Supplies a boolean value that specifies
;       whether alert should be tested for the previous processor mode.
;
; Return Value:
;
;    Normally there is no return from this routine. However, if the specified
;    context record is misaligned or is not accessible, then the appropriate
;    status code is returned.
;
;--

NcTrapFrame             equ     [ebp + 0]
NcContextRecord         equ     [ebp + 8]
NcTestAlert             equ     [ebp + 12]

align dword
cPublicProc _NtContinue     ,2

        push    ebp

;
; Call KiContinue to load ContextRecord into TrapFrame.  On x86 TrapFrame
; is an atomic entity, so we don't need to allocate any other space here.
;
; KiContinue(NcContextRecord, 0, NcTrapFrame)
;

        mov     ebp,esp
        mov     eax, NcTrapFrame
        mov     ecx, NcContextRecord
        stdCall  _KiContinue, <ecx, 0, eax>
        or      eax,eax                 ; return value 0?
        jnz     short Nc20              ; KiContinue failed, go report error

;
; Check to determine if alert should be tested for the previous processor mode.
;

        cmp     byte ptr NcTestAlert,0  ; Check test alert flag
        je      short Nc10              ; if z, don't test alert, go Nc10
        stdCall _KeTestAlertThread, <0>  test alert for current thread

Nc10:   pop     ebp                     ; (ebp) -> TrapFrame
        mov     esp,ebp                 ; (esp) = (ebp) -> trapframe
        jmp     _KiServiceExit2         ; common exit

Nc20:   pop     ebp                     ; (ebp) -> TrapFrame
        mov     esp,ebp                 ; (esp) = (ebp) -> trapframe
        jmp     _KiServiceExit          ; common exit

stdENDP _NtContinue

align dword
cPublicProc _ZwContinue     ,2

;
; Must go through an interrupt trap in order to build the trap frame that
; NtContinue requires.
;
        mov     eax, [esp+4]            ; (eax) -> ContextRecord
        mov     ecx, [esp+8]            ; (ecx) -> TestAlert
        int     28h
        stdRet  _ZwContinue

stdENDP _ZwContinue

        page
        subttl  "Raise Exception System Service"
;++
;
; NTSTATUS
; NtRaiseException (
;    IN PEXCEPTION_RECORD ExceptionRecord,
;    IN PCONTEXT ContextRecord,
;    IN BOOLEAN FirstChance
;    )
;
; Routine Description:
;
;    This routine is called as a system service to raise an exception. Its
;    function is to transfer information from the specified context record
;    into the trap frame that was built when the system service was executed.
;    The exception may be raised as a first or second chance exception.
;
;   WARNING - Do not call this routine directly, always call it as
;             ZwRaiseException!!!  This is required because it needs the
;             trapframe built by KiSystemService.
;
;   NOTE - KiSystemService will terminate the ExceptionList, which is
;          not what we want for this case, so we will fish it out of
;          the trap frame and restore it.
;
; Arguments:
;
;    TrapFrame (ebp+0: before setup) -> System trap frame for this call
;
;    ExceptionRecord (ebp+8: after setup) -> An exception record.
;
;    ContextRecord (ebp+12: after setup) -> Points to a context record.
;
;    FirstChance (epb+16: after setup) -> Supplies a boolean value that
;       specifies whether the exception is to be raised as a first (TRUE)
;       or second chance (FALSE) exception.
;
; Return Value:
;
;    None.
;--
align dword
cPublicProc _NtRaiseException ,3

        push    ebp

;
;   Put back the ExceptionList so the exception can be properly
;   dispatched.
;

        mov     ebp,esp                 ; [ebp+0] -> TrapFrame
        mov     ebx, [ebp+0]            ; (ebx)->TrapFrame
        mov     edx, [ebp+16]           ; (edx) = First chance indicator
        mov     eax, [ebx]+TsExceptionList ; Old exception list
        mov     ecx, [ebp+12]           ; (ecx)->ContextRecord
        mov     PCR[PcExceptionList],eax
        mov     eax, [ebp+8]            ; (eax)->ExceptionRecord

;
;   KiRaiseException(ExceptionRecord, ContextRecord, ExceptionFrame,
;           TrapFrame, FirstChance)
;

        stdCall   _KiRaiseException,<eax, ecx, 0, ebx, edx>

        pop     ebp
        mov     esp,ebp                 ; (esp) = (ebp) -> trap frame

;
;   If the exception was handled, then the trap frame has been edited to
;   reflect new state, and we'll simply exit the system service to get
;   the effect of a continue.
;
;   If the exception was not handled, we'll return to our caller, who
;   will raise a new exception.
;
        jmp     _KiServiceExit2

stdENDP _NtRaiseException

align dword
cPublicProc _ZwRaiseException     ,3

;
; Must go through an interrupt trap in order to build the trap frame that
; NtRaiseException requires.
;
        mov     eax, [esp+4]            ; (eax) -> ExceptionRecord
        mov     ecx, [esp+8]            ; (ecx) -> ContextRecord
        mov     edx, [esp+12]           ; (edx) -> FirstChance
        int     29h
        stdRet  _ZwRaiseException

stdENDP _ZwRaiseException

_TEXT   ends
        end
