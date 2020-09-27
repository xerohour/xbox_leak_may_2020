     title  "Trap 03 Processing"
;++
;
; Copyright (c) 2000  Microsoft Corporation
;
; Module Name:
;
;    trap03.asm
;
; Abstract:
;
;    This module implements the code necessary to process i386
;    break point trap.
;
;--

.386p
        .xlist
KERNELONLY  equ     1
include ks386.inc
include callconv.inc                    ; calling convention macros
        .list


        extrn   _HarnessTrap03TlsIndex:DWORD
        EXTRNP  _SafeTlsGetValue,1
        EXTRNP  _HarnessDispatchBreakpointException,4


SET_DEBUG_DATA  macro

ife FPO

;
; This macro is used by ENTER_SYSTEM_CALL, ENTER_TRAP and ENTER_INTERRUPT
; and is used at the end of above macros.  It is safe to destroy ebx, edi.
;

        mov     ebx,[ebp]+TsEbp
        mov     edi,[ebp]+TsEip
        mov     [ebp]+TsDbgArgPointer,edx
        mov     [ebp]+TsDbgArgMark,0BADB0D00h
        mov     [ebp]+TsDbgEbp,ebx
        mov     [ebp]+TsDbgEip,edi
endif

endm


;++
;
;   ENTER_TRAP
;
;   Macro Description:
;
;       Build the frame and set registers needed by a trap or exception.
;
;       Save:
;           Non-volatile regs,
;           FS,
;           ExceptionList,
;           PreviousMode,
;           Volatile regs
;           Seg Regs from V86 mode
;           DS, ES, GS
;
;       Don't Save:
;           Floating point state
;
;       Set:
;           FS,
;           Direction,
;           DS, ES
;
;       Don't Set:
;           PreviousMode,
;           ExceptionList
;
;   Arguments:
;       None.
;
;   Exit-conditions:
;       Interrupts match input state (this routine doesn't change IEF)
;       (esp)->base of trap frame
;       (ebp)->base of trap frame
;       Preserves entry eax
;
;--

ENTER_TRAP macro

;
;   Fill in parts of frame we care about
;

        mov     word ptr [esp+2], 0 ; Clear upper word of ErrorCode

        push    ebp                 ; Save the non-volatile registers
        push    ebx
        push    esi
        push    edi

        mov     ebx, fs:[0]         ; Save ExceptionList
        push    ebx
        push    eax                 ; Save the volatile registers
        push    ecx
        push    edx

;
;   Skip allocate reset of trap frame.
;

        sub     esp,TsEdx

        mov     ebp,esp

        cld
        SET_DEBUG_DATA

        endm

;++
;
;   EXIT_ALL    NoRestoreSegs, NoRestoreVolatiles, NoPreviousMode
;
;   Macro Description:
;
;       Load a syscall frame back into the machine.
;
;       Restore:
;           Volatile regs, IF NoRestoreVolatiles blank
;           NoPreviousMode,
;           ExceptionList,
;           FS,
;           Non-volatile regs
;
;       If the frame is a kernel mode frame, AND esp has been edited,
;       then TsSegCs will have a special value.  Test for that value
;       and execute special code for that case.
;
;       N.B. This macro generates an IRET!  (i.e. It exits!)
;
;   Arguments:
;
;       NoRestoreSegs - non-blank if DS, ES, GS are NOT to be restored
;
;       NoRestoreVolatiles - non-blank if Volatile regs are NOT to be restored
;
;       NoPreviousMode - if nb pop ThPreviousMode
;
;   Entry-conditions:
;
;       (esp)->base of trap frame
;       (ebp)->Base of trap frame
;
;   Exit-conditions:
;
;       Does not exit, returns.
;       Preserves eax, ecx, edx, IFF NoRestoreVolatiles is set
;
;--

?adjesp = 0
?RestoreAll = 1

EXIT_ALL macro  NoRestoreSegs, NoRestoreVolatiles, NoPreviousMode
local   a, b, f, x
local   Db_NotATrapFrame, Db_A, Db_NotValidEntry

;
; Sanity check some values and setup globals for macro
;

?adjesp = TsEdx
?RestoreAll = 1

ifnb <NoRestoreSegs>
    ?RestoreAll = 0
endif

ifnb <NoRestoreVolatiles>
    if ?RestoreAll eq 1
        %out "EXIT_ALL NoRestoreVolatiles requires NoRestoreSegs"
        .err
    endif
    ?adjesp = ?adjesp + 12
endif

ifb <NoPreviousMode>
ifndef KERNELONLY
        %out    EXIT_ALL can not restore previousmode outside kernel
        .err
endif
endif

; All callers are responsible for getting here with interrupts disabled.

if DBG
        pushfd
        pop     edx

        test    edx, EFLAGS_INTERRUPT_MASK
        jnz     Db_NotValidEntry

        cmp     esp, ebp                    ; make sure esp = ebp
        jne     Db_NotValidEntry

; Make sure BADB0D00 sig is present.  If not this isn't a trap frame!
Db_A:   sub     [esp]+TsDbgArgMark,0BADB0D00h
        jne     Db_NotATrapFrame
endif

        mov     edx, [esp]+TsExceptionList
if DBG
        or      edx, edx
        jnz     short @f
    int 3
@@:
endif
        mov     dword ptr fs:[0], edx       ; Restore ExceptionList

        test    word ptr [esp]+TsSegCs,FRAME_EDITED
        jz      b                           ; Edited frame pop out.

ifb <NoRestoreVolatiles>
ifb <NoRestoreSegs>                         ; must restore eax before any
        mov     eax, [esp].TsEax            ; selectors! (see trap0e handler)
endif
endif

ifb <NoRestoreVolatiles>
        mov     edx, [ebp]+TsEdx            ; Restore volitales
        mov     ecx, [ebp]+TsEcx
ifb <NoRestoreSegs>
else
        mov     eax, [ebp]+TsEax
endif
endif   ; NoRestoreVolatiles

        lea     esp, [ebp]+TsEdi            ; Skip PreMode, ExceptList and fs

        pop     edi                         ; restore non-volatiles
        pop     esi
        pop     ebx
        pop     ebp

;
; Esp MUST point to the Error Code on the stack.  Because we use it to
; store the entering esp.
;

        add     esp, 4              ; remove error code from trap frame
        iretd                       ; return

if DBG
Db_NotATrapFrame:
        add     [esp]+TsDbgArgMark,0BADB0D00h   ; put back the orig value
Db_NotValidEntry:
        int 3
        jmp     Db_A
endif

;
;   TsSegCs contains the special value that means the frame was edited
;   in a way that affected esp, AND it's a kernel mode frame.
;   (Special value is null selector except for RPL.)
;
;   Put back the real CS.
;   push eflags, eip onto target stack
;   restore
;   switch to target stack
;   iret
;

b:      mov     ebx,[esp]+TsTempSegCs
        mov     [esp]+TsSegCs,ebx

;
;   There is no instruction that will load esp with an arbitrary value
;   (i.e. one out of a frame) and do a return, if no privledge transition
;   is occuring.  Therefore, if we are returning to kernel mode, and
;   esp has been edited, we must "emulate" a kind of iretd.
;
;   We do this by logically pushing the eip,cs,eflags onto the new
;   logical stack, loading that stack, and doing an iretd.  This
;   requires that the new logical stack is at least 1 dword higher
;   than the unedited esp would have been.  (i.e.  It is not legal
;   to edit esp to have a new value < the old value.)
;
;   KeContextToKframes enforces this rule.
;

;
;   Compute new logical stack address
;

        mov     ebx,[esp]+TsTempEsp
        sub     ebx,12
        mov     [esp]+TsErrCode,ebx

;
;   Copy eip,cs,eflags to new stack.  note we do this high to low
;

        mov     esi,[esp]+TsEflags
        mov     [ebx+8],esi
        mov     esi,[esp]+TsSegCs
        mov     [ebx+4],esi
        mov     esi,[esp]+TsEip
        mov     [ebx],esi

;
;   Do a standard restore sequence.
;
;   Observe that RestoreVolatiles is honored.  Editing a volatile
;   register has no effect when returning from a system call.
;
ifb     <NoRestoreVolatiles>
        mov     eax,[esp].TsEax
endif
;        add     esp,TsSegGs
;
;ifb     <NoRestoreSegs>
;        pop     gs
;        pop     es
;        pop     ds
;else
;        add     esp,12
;endif

ifb     <NoRestoreVolatiles>
        mov     edx, [esp]+TsEdx
        mov     ecx, [esp]+TsEcx
endif

;ifnb <NoPreviousMode>
;        add     esp, 4              ; Skip previous mode
;else
;        pop     ebx                 ; Restore PreviousMode
;        mov     esi,fs:[PcPrcbData+PbCurrentThread]
;        mov     ss:[esi]+ThPreviousMode,bl
;endif
;
;       pop     ebx
;
;       mov     fs:[PcExceptionList], ebx ;Restore ExceptionList
;       pop     fs

        add     esp, TsEdi
        pop     edi                 ; restore non-volatiles
        pop     esi
        pop     ebx
        pop     ebp

;
;   (esp)->TsErrCode, where we saved the new esp
;

        mov     esp,[esp]           ; Do move not push to avoid increment
        iretd

        endm


_DATA   SEGMENT PARA PUBLIC 'DATA'

PrevTrap03Handler   dd  0

_DATA   ENDS


_TEXT$00   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:NOTHING, ES:NOTHING, SS:FLAT, FS:NOTHING, GS:NOTHING
align dword


;++
;
; Routine Description:
;
;    Handle INT 3 breakpoint.
;
;    The trap is caused by a single byte INT 3 instruction.
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

align dword

        public  HarnessTrap03Handler

HarnessTrap03Handler    proc

        pushfd
        pushad
        mov     eax, _HarnessTrap03TlsIndex
        stdCall _SafeTlsGetValue, <eax>
        test    eax, eax
        jnz     @F
        popad
        popfd
        jmp     dword ptr PrevTrap03Handler
@@:
        popad
        popfd

;
; Build the trap frame and set registers needed by a trap
;
                                        ; control registers are at TOS already
        push    0                       ; dummy error code
        ENTER_TRAP

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

        sub     esp, ExceptionRecordLength
        mov     dword ptr [esp]+ErExceptionCode, eax
        xor     eax, eax
        mov     dword ptr [esp]+ErExceptionFlags, eax
        mov     dword ptr [esp]+ErExceptionRecord, eax
        mov     dword ptr [esp]+ErExceptionAddress, ebx
        mov     dword ptr [esp]+ErNumberParameters, ecx

        lea     ebx, [esp + ErExceptionInformation]
        mov     [ebx], edx
        mov     [ebx+4], esi
        mov     [ebx+8], edi

;
; Set up arguments and call _HarnessDispatchBreakpointException
;

        mov     ecx, esp
        stdCall _HarnessDispatchBreakpointException, <ecx, 0, ebp, 1>

        mov     esp, ebp
        cli
        EXIT_ALL    ,,NoPreviousMode

HarnessTrap03Handler    endp


;++
;
; VOID
; HarnessInitializeTrap03Handler(
;     VOID
;     )
;
; Routine Description:
;
;    This routine installs trap03 handler
;
; Arguments:
;
;    None
;
; Return Value:
;
;    None
;
;--

align dword

cPublicProc _HarnessInitializeTrap03Handler ,0

        push    ebp
        mov     ebp, esp
        sub     esp, 8

        sidt    fword ptr [ebp-8]       ; get IDT address
        mov     ecx, dword ptr [ebp-6]  ; ecx now point to IDT
        mov     eax, 3                  ; trap03
        shl     eax, 3                  ; 8 bytes per IDT entry
        add     ecx, eax                ; ecx now point to IDT entry of trap03

        mov     ax, word ptr [ecx+6]    ; save original trap03 handler
        shl     eax, 16
        mov     ax, word ptr [ecx]
        mov     PrevTrap03Handler, eax

        mov     eax, offset FLAT:HarnessTrap03Handler

        mov     word ptr [ecx], ax      ; install new handler
        shr     eax, 16
        mov     word ptr [ecx+6], ax

        mov     esp, ebp
        pop     ebp
        stdRET  _HarnessInitializeTrap03Handler

stdENDP _HarnessInitializeTrap03Handler


;++
;
; VOID
; GetStackLimits(
;    OUT PULONG LowLimit,
;    OUT PULONG HighLimit
;    )
;
; Routine Description:
;
;    This function returns the current stack limits based on the current
;    processor mode.
;
;    On the 386 we always store the stack limits in the PCR, and address
;    both PCR and TEB the same way, so this code is mode independent.
;
; Arguments:
;
;    LowLimit (esp+4) - Supplies a pointer to a variable that is to receive
;       the low limit of the stack.
;
;    HighLimit (esp+8) - Supplies a pointer to a variable that is to receive
;       the high limit of the stack.
;
; Return Value:
;
;    None.
;
;--

align dword

cPublicProc _GetStackLimits ,2

        mov     eax,fs:PcStackLimit
        mov     ecx,[esp+4]
        mov     [ecx],eax               ; Save low limit

        mov     eax,fs:PcStackBase
        mov     ecx,[esp+8]
        mov     [ecx],eax               ; Save high limit

        stdRET    _GetStackLimits

stdENDP _GetStackLimits


;++
;
;   PVOID
;   GetRegistrationHead()
;
;   Routine Description:
;
;       This function returns the address of the first Exception
;       registration record for the current context.
;
;   Arguments:
;
;       None.
;
;   Return Value:
;
;       The address of the first registration record.
;
;--

align dword

cPublicProc _GetRegistrationHead    ,0

        mov     eax,fs:PcExceptionList
        stdRET  _GetRegistrationHead

stdENDP _GetRegistrationHead


;++
;
; EXCEPTION_DISPOSITION
; RtlpExecuteHandlerForException (
;    IN PEXCEPTION_RECORD ExceptionRecord,
;    IN PVOID EstablisherFrame,
;    IN OUT PCONTEXT ContextRecord,
;    IN OUT PVOID DispatcherContext,
;    IN PEXCEPTION_ROUTINE ExceptionRoutine
;    )
;
; Routine Description:
;
;    This function allocates a call frame, stores the handler address and
;    establisher frame pointer in the frame, establishes an exception
;    handler, and then calls the specified exception handler as an exception
;    handler. If a nested exception occurs, then the exception handler of
;    of this function is called and the handler address and establisher
;    frame pointer are returned to the exception dispatcher via the dispatcher
;    context parameter. If control is returned to this routine, then the
;    frame is deallocated and the disposition status is returned to the
;    exception dispatcher.
;
; Arguments:
;
;    ExceptionRecord (ebp+8) - Supplies a pointer to an exception record.
;
;    EstablisherFrame (ebp+12) - Supplies the frame pointer of the establisher
;       of the exception handler that is to be called.
;
;    ContextRecord (ebp+16) - Supplies a pointer to a context record.
;
;    DispatcherContext (ebp+20) - Supplies a pointer to the dispatcher context
;       record.
;
;    ExceptionRoutine (ebp+24) - supplies a pointer to the exception handler
;       that is to be called.
;
; Return Value:
;
;    The disposition value returned by the specified exception handler is
;    returned as the function value.
;
;--

align dword

cPublicProc _RtlpExecuteHandlerForException,5

        mov     edx,offset FLAT:ExceptionHandler    ; Set who to register
        jmp     ExecuteHandler                      ; jump to common code

stdENDP _RtlpExecuteHandlerForException


;
;   ExecuteHandler is the common tail for RtlpExecuteHandlerForException
;   and RtlpExecuteHandlerForUnwind.
;
;   (edx) = handler (Exception or Unwind) address
;


ExceptionRecord     equ [ebp+8]
EstablisherFrame    equ [ebp+12]
ContextRecord       equ [ebp+16]
DispatcherContext   equ [ebp+20]
ExceptionRoutine    equ [ebp+24]


cPublicProc   ExecuteHandler,5

        push    ebp
        mov     ebp,esp

        push    EstablisherFrame        ; Save context of exception handler
                                        ; that we're about to call.

    .errnz   ErrHandler-4
        push    edx                     ; Set Handler address

    .errnz   ErrNext-0
        push    fs:PcExceptionList                      ; Set next pointer


        mov     fs:PcExceptionList,esp                  ; Link us on

; Call the specified exception handler.

        push    DispatcherContext
        push    ContextRecord
        push    EstablisherFrame
        push    ExceptionRecord

        mov     ecx,ExceptionRoutine
        call    ecx
        mov     esp,fs:PcExceptionList

; Don't clean stack here, code in front of ret will blow it off anyway

; Disposition is in eax, so all we do is deregister handler and return

    .errnz  ErrNext-0
        pop     fs:PcExceptionList

        mov     esp,ebp
        pop     ebp
        stdRET  ExecuteHandler

stdENDP ExecuteHandler


;++
;
; EXCEPTION_DISPOSITION
; ExceptionHandler (
;    IN PEXCEPTION_RECORD ExceptionRecord,
;    IN PVOID EstablisherFrame,
;    IN OUT PCONTEXT ContextRecord,
;    IN OUT PVOID DispatcherContext
;    )
;
; Routine Description:
;
;    This function is called when a nested exception occurs. Its function
;    is to retrieve the establisher frame pointer and handler address from
;    its establisher's call frame, store this information in the dispatcher
;    context record, and return a disposition value of nested exception.
;
; Arguments:
;
;    ExceptionRecord (exp+4) - Supplies a pointer to an exception record.
;
;    EstablisherFrame (esp+8) - Supplies the frame pointer of the establisher
;       of this exception handler.
;
;    ContextRecord (esp+12) - Supplies a pointer to a context record.
;
;    DispatcherContext (esp+16) - Supplies a pointer to the dispatcher context
;       record.
;
; Return Value:
;
;    A disposition value ExceptionNestedException is returned if an unwind
;    is not in progress. Otherwise a value of ExceptionContinueSearch is
;    returned.
;
;--

stdPROC   ExceptionHandler,4

        mov     ecx,dword ptr [esp+4]           ; (ecx) -> ExceptionRecord
        test    dword ptr [ecx.ErExceptionFlags], (EXCEPTION_UNWINDING OR EXCEPTION_EXIT_UNWIND)
        mov     eax,ExceptionContinueSearch     ; Assume unwind
        jnz     eh10                            ; unwind, go return

;
; Unwind is not in progress - return nested exception disposition.
;

        mov     ecx,[esp+8]             ; (ecx) -> EstablisherFrame
        mov     edx,[esp+16]            ; (edx) -> DispatcherContext
        mov     eax,[ecx+8]             ; (eax) -> EstablisherFrame for the
                                        ;          handler active when we
                                        ;          nested.
        mov     [edx],eax               ; Set DispatcherContext field.
        mov     eax,ExceptionNestedException

eh10:   stdRET    ExceptionHandler

stdENDP ExceptionHandler


_TEXT$00   ends

        end
