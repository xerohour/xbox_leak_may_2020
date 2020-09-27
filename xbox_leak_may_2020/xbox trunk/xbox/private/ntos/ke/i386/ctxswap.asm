        title  "Context Swap"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    ctxswap.asm
;
; Abstract:
;
;    This module implements the code necessary to field the dispatch
;    interrupt and to perform kernel initiated context switching.
;
; Author:
;
;    Shie-Lin Tzong (shielint) 14-Jan-1990
;
; Environment:
;
;    Kernel mode only.
;
; Revision History:
;
;   22-feb-90   bryanwi
;       write actual swap context procedure
;
;--

.486p
        .xlist
include ks386.inc
include i386\kimacro.inc
include callconv.inc
        .list

        EXTRNP  HalClearSoftwareInterrupt,1,,FASTCALL
        EXTRNP  HalRequestSoftwareInterrupt,1,,FASTCALL
        EXTRNP  KiReadyThread,1,,FASTCALL
        EXTRNP  KiWaitTest,2,,FASTCALL
        EXTRNP  KfLowerIrql,1,,FASTCALL
        EXTRNP  KfRaiseIrql,1,,FASTCALL
        EXTRNP  _KeGetCurrentIrql,0
        EXTRNP  _KeGetCurrentThread,0
        EXTRNP  _KiDeliverApc,0
        EXTRNP  _KiQuantumEnd,0
        EXTRNP  _KeBugCheckEx,5
        EXTRNP  _KeBugCheck,1

        extrn   _KiTrap13:PROC
        extrn   KiRetireDpcList:PROC
        extrn   _KeTickCount:DWORD

        extrn   _KiDispatcherReadyListHead:DWORD
        extrn   _KiIdleSummary:DWORD
        extrn   _KiReadySummary:DWORD
        extrn   _KiPCR:DWORD
        extrn   _KiIdleThread:DWORD

if DBG
        extrn   _KdDebuggerEnabled:BYTE
        EXTRNP  _DbgBreakPoint,0
        extrn   _DbgPrint:near
endif

ifdef DEVKIT
        extrn  _KiDbgCtxSwapNotify:DWORD
endif

_TEXT   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page ,132
        subttl  "Unlock Dispatcher Database"
;++
;
; VOID
; KiUnlockDispatcherDatabase (
;    IN KIRQL OldIrql
;    )
;
; Routine Description:
;
;    This routine is entered at IRQL DISPATCH_LEVEL with the dispatcher
;    database locked. Its function is to either unlock the dispatcher
;    database and return or initiate a context switch if another thread
;    has been selected for execution.
;
; Arguments:
;
;    (TOS)   Return address
;
;    (ecx)   OldIrql - Supplies the IRQL when the dispatcher database
;        lock was acquired.
;
; Return Value:
;
;    None.
;
;--

cPublicFastCall KiUnlockDispatcherDatabase, 1

;
; Check if a new thread is scheduled for execution.
;

        cmp     PCR[PcPrcbData+PbNextThread], 0 ; check if next thread
        jne     short Kiu20             ; if ne, new thread scheduled

;
; Release dispatcher database lock, lower IRQL to its previous level,
; and return.
;

Kiu00:                                  ;

;
; N.B. This exit jumps directly to the lower IRQL routine which has a
;      compatible fastcall interface.
;

        jmp     @KfLowerIrql@4          ; lower IRQL to previous level

;
; A new thread has been selected to run on the current processor, but
; the new IRQL is not below dispatch level. If the current processor is
; not executing a DPC, then request a dispatch interrupt on the current
; processor.
;

Kiu10:  cmp     dword ptr PCR[PcPrcbData.PbDpcRoutineActive],0  ; check if DPC routine active
        jne     short Kiu00             ; if ne, DPC routine is active

        push    ecx                     ; save new IRQL

        mov     cl, DISPATCH_LEVEL      ; request dispatch interrupt
        fstCall HalRequestSoftwareInterrupt ;
        pop     ecx                     ; restore new IRQL

;
; N.B. This exit jumps directly to the lower IRQL routine which has a
;      compatible fastcall interface.
;

        jmp     @KfLowerIrql@4          ; lower IRQL to previous level

;
; Check if the previous IRQL is less than dispatch level.
;

Kiu20:  cmp     cl, DISPATCH_LEVEL      ; check if IRQL below dispatch level
        jge     short Kiu10             ; if ge, not below dispatch level

;
; There is a new thread scheduled for execution and the previous IRQL is
; less than dispatch level. Context switch to the new thread immediately.
;
;
; N.B. The following registers MUST be saved such that ebp is saved last.
;      This is done so the debugger can find the saved ebp for a thread
;      that is not currently in the running state.
;

.fpo (0, 0, 0, 4, 1, 0)
        sub     esp, 4*4
        mov     [esp+12], ebx           ; save registers
        mov     [esp+8], esi            ;
        mov     [esp+4], edi            ;
        mov     [esp+0], ebp            ;
        lea     ebx, _KiPCR             ; get address of PCR
        mov     esi, [ebx].PcPrcbData.PbNextThread ; get next thread address
        mov     edi, [ebx].PcPrcbData.PbCurrentThread ; get current thread address
        mov     dword ptr [ebx].PcPrcbData.PbNextThread, 0 ; clear next thread address
        mov     [ebx].PcPrcbData.PbCurrentThread, esi ; set current thread address
        mov     [edi].ThWaitIrql, cl    ; save previous IRQL
        mov     ecx, edi                ; set address of current thread
        fstCall KiReadyThread           ; reready thread for execution
        mov     cl, [edi].ThWaitIrql    ; set APC interrupt bypass disable
        call    SwapContext             ; swap context
        or      al, al                  ; check if kernel APC pending
        mov     cl, [esi].ThWaitIrql    ; get original wait IRQL
        jnz     short Kiu50             ; if nz, kernel APC pending

Kiu30:  mov     ebp, [esp+0]            ; restore registers
        mov     edi, [esp+4]            ;
        mov     esi, [esp+8]            ;
        mov     ebx, [esp+12]           ;
        add     esp, 4*4

;
; N.B. This exit jumps directly to the lower IRQL routine which has a
;      compatible fastcall interface.
;

        jmp     @KfLowerIrql@4          ; lower IRQL to previous level

Kiu50:  mov     cl, APC_LEVEL           ; lower IRQL to APC level
        fstCall KfLowerIrql             ;
        xor     eax, eax                ; set previous mode to kernel
        stdCall _KiDeliverApc           ; deliver kernel mode APC
        xor     ecx, ecx                ; set original wait IRQL
        jmp     short Kiu30

fstENDP KiUnlockDispatcherDatabase

        page ,132
        subttl  "Swap Thread"
;++
;
; VOID
; KiSwapThread (
;    VOID
;    )
;
; Routine Description:
;
;    This routine is called to select the next thread to run on the
;    current processor and to perform a context switch to the thread.
;
; Arguments:
;
;    None.
;
; Return Value:
;
;    Wait completion status (eax).
;
;--

cPublicFastCall KiSwapThread, 0
.fpo (0, 0, 0, 4, 1, 0)

;
; N.B. The following registers MUST be saved such that ebp is saved last.
;      This is done so the debugger can find the saved ebp for a thread
;      that is not currently in the running state.
;

        sub     esp, 4*4
        mov     [esp+12], ebx           ; save registers
        mov     [esp+8], esi            ;
        mov     [esp+4], edi            ;
        mov     [esp+0], ebp            ;

        lea     ebx, _KiPCR             ; get address of PCR
        mov     edx, [ebx].PcPrcbData.PbNextThread ; get next thread address
        or      edx, edx                ; check if next thread selected
        jnz     Swt140                  ; if nz, next thread selected

;
; Find the highest nibble in the ready summary that contains a set bit
; and left justify so the nibble is in bits <31:28>
;

        mov     ecx, 16                 ; set base bit number
        mov     edi, _KiReadySummary    ; get ready summary
        mov     esi, edi                ; copy ready summary
        shr     esi, 16                 ; isolate bits <31:16> of summary
        jnz     short Swt10             ; if nz, bits <31:16> are nonzero
        xor     ecx, ecx                ; set base bit number
        mov     esi, edi                ; set bits <15:0> of summary
Swt10:  shr     esi, 8                  ; isolate bits <15:8> of low bits
        jz      short Swt20             ; if z, bits <15:8> are zero
        add     ecx, 8                  ; add offset to nonzero byte
Swt20:  mov     esi, edi                ; isolate highest nonzero byte
        shr     esi, cl                 ;
        add     ecx, 3                  ; adjust to high bit of nibble
        cmp     esi, 10h                ; check if high nibble nonzero
        jb      short Swt30             ; if b, then high nibble is zero
        add     ecx, 4                  ; compute ready queue priority
Swt30:  mov     esi, ecx                ; left justify ready summary nibble
        not     ecx                     ;
        shl     edi, cl                 ;
        or      edi, edi                ;

;
; If the next bit is set in the ready summary, then scan the corresponding
; dispatcher ready queue.
;

Swt40:  js      short Swt60             ; if s, queue contains an entry
Swt50:  sub     esi, 1                  ; decrement ready queue priority
        shl     edi, 1                  ; position next ready summary bit
        jnz     short Swt40             ; if nz, more queues to scan

;
; All ready queues were scanned without finding a runnable thread so
; default to the idle thread and set the appropriate bit in idle summary.
;

        mov     _KiIdleSummary, 1       ; set idle summary bit

        lea     edx, _KiIdleThread      ; set idle thread address
        jmp     Swt140                  ;

;
; If the thread can execute on the current processor, then remove it from
; the dispatcher ready queue.
;

        align   4
swt60:  lea     ebp, [esi*8] + _KiDispatcherReadyListHead ; get ready queue address
        mov     ecx, [ebp].LsFlink      ; get address of first queue entry
Swt70:  mov     edx, ecx                ; compute address of thread object
        sub     edx, ThWaitListEntry    ;

;
; Remove the selected thread from the ready queue.
;

        mov     eax, [ecx].LsFlink      ; get list entry forward link
        mov     ebp, [ecx].LsBlink      ; get list entry backward link
        mov     [ebp].LsFlink, eax      ; set forward link in previous entry
        mov     [eax].LsBlink, ebp      ; set backward link in next entry
        cmp     eax, ebp                ; check if list is empty
        jnz     short Swt140            ; if nz, list is not empty
        mov     ebp, 1                  ; clear ready summary bit
        mov     ecx, esi                ;
        shl     ebp, cl                 ;
        xor     _KiReadySummary, ebp    ;

;
; Swap context to the next thread.
;

Swt140: mov     esi, edx                ; set address of next thread
        mov     edi, [ebx].PcPrcbData.PbCurrentThread ; set current thread address
        mov     dword ptr [ebx].PcPrcbData.PbNextThread, 0 ; clear next thread address
        mov     [ebx].PcPrcbData.PbCurrentThread, esi ; set current thread address
        mov     cl, [edi].ThWaitIrql    ; set APC interrupt bypass disable
        call    SwapContext             ; swap context
        or      al, al                  ; check if kernel APC pending
        mov     edi, [esi].ThWaitStatus ; save wait completion status
        mov     cl, [esi].ThWaitIrql    ; get wait IRQL
        jnz     short Swt160            ; if nz, kernel APC pending

Swt150: fstCall KfLowerIrql             ; lower IRQL to previous value

        mov     eax, edi                ; set wait completion status
        mov     ebp, [esp+0]            ; restore registers
        mov     edi, [esp+4]            ;
        mov     esi, [esp+8]            ;
        mov     ebx, [esp+12]           ;
        add     esp, 4*4                ;
        fstRET  KiSwapThread            ;

Swt160: mov     cl, APC_LEVEL           ; lower IRQL to APC level
        fstCall KfLowerIrql             ;
        xor     eax, eax                ; set previous mode to kernel
        stdCall _KiDeliverApc           ; deliver kernel mode APC
        xor     ecx, ecx                ; set original wait IRQL
        jmp     short Swt150

fstENDP KiSwapThread

        page ,132
        subttl  "Dispatch Interrupt"
;++
;
; Routine Description:
;
;    This routine is entered as the result of a software interrupt generated
;    at DISPATCH_LEVEL. Its function is to process the Deferred Procedure Call
;    (DPC) list, and then perform a context switch if a new thread has been
;    selected for execution on the processor.
;
;    This routine is entered at IRQL DISPATCH_LEVEL with the dispatcher
;    database unlocked. When a return to the caller finally occurs, the
;    IRQL remains at DISPATCH_LEVEL, and the dispatcher database is still
;    unlocked.
;
; Arguments:
;
;    None
;
; Return Value:
;
;    None.
;
;--

        align 16
cPublicProc _KiDispatchInterrupt ,0
cPublicFpo 0, 0

        lea     ebx, _KiPCR             ; get address of PCR
kdi00:  lea     eax, [ebx].PcPrcbData.PbDpcListHead ; get DPC listhead address

;
; Disable interrupts and check if there is any work in the DPC list
; of the current processor.
;

kdi10:  cli                             ; disable interrupts
        cmp     eax, [eax].LsFlink      ; check if DPC List is empty
        je      short kdi40             ; if eq, list is empty
        push    ebp                     ; save register

;
; Exceptions occuring in DPCs are unrelated to any exception handlers
; in the interrupted thread.  Terminate the exception list.
;

        push    [ebx].PcExceptionList
        mov     [ebx].PcExceptionList, EXCEPTION_CHAIN_END

;
; Switch to the DPC stack for this processor.
;

        mov     edx, esp
        mov     esp, [ebx].PcPrcbData.PbDpcStack
        push    edx

.fpo (0, 0, 0, 1, 1, 0)

        mov     ebp, eax                ; set address of DPC listhead
        call    KiRetireDpcList         ; process the current DPC list

;
; Switch back to the current thread stack, restore the exception list
; and saved EBP.
;

        pop     esp
	pop     [ebx].PcExceptionList
        pop     ebp

.fpo (0, 0, 0, 0, 0, 0)

;
; Check to determine if quantum end is requested.
;
; N.B. If a new thread is selected as a result of processing the quantum
;      end request, then the new thread is returned with the dispatcher
;      database locked. Otherwise, NULL is returned with the dispatcher
;      database unlocked.
;

kdi40:  sti                             ; enable interrupts
        cmp     dword ptr [ebx].PcPrcbData.PbQuantumEnd, 0 ; quantum end requested
        jne     kdi90                   ; if neq, quantum end request

;
; Check to determine if a new thread has been selected for execution on this
; processor.
;

        cmp     dword ptr [ebx].PcPrcbData.PbNextThread, 0 ; check addr of next thread object
        je      short kdi70             ; if eq, then no new thread

;
; Disable interrupts and attempt to acquire the dispatcher database lock.
;

        mov     eax, [ebx].PcPrcbData.PbNextThread ; get next thread address

;
; N.B. The following registers MUST be saved such that ebp is saved last.
;      This is done so the debugger can find the saved ebp for a thread
;      that is not currently in the running state.
;

.fpo (0, 0, 0, 3, 1, 0)

kdi60:  sub     esp, 3*4
        mov     [esp+8], esi            ; save registers
        mov     [esp+4], edi            ;
        mov     [esp+0], ebp            ;
        mov     esi, eax                ; set next thread address
        mov     edi, [ebx].PcPrcbData.PbCurrentThread ; get current thread address
        mov     dword ptr [ebx].PcPrcbData.PbNextThread, 0 ; clear next thread address
        mov     [ebx].PcPrcbData.PbCurrentThread, esi ; set current thread address
        mov     ecx, edi                ; set address of current thread
        fstCall KiReadyThread           ; ready thread (ecx) for execution
        mov     cl, 1                   ; set APC interrupt bypass disable
        call    SwapContext             ; call context swap routine
        mov     ebp, [esp+0]            ; restore registers
        mov     edi, [esp+4]            ;
        mov     esi, [esp+8]            ;
        add     esp, 3*4
kdi70:  stdRET  _KiDispatchInterrupt    ; return

;
; Process quantum end event.
;
; N.B. If the quantum end code returns a NULL value, then no next thread
;      has been selected for execution. Otherwise, a next thread has been
;      selected and the dispatcher databased is locked.
;

kdi90:  mov     dword ptr [ebx].PcPrcbData.PbQuantumEnd, 0 ; clear quantum end indicator
        stdCall _KiQuantumEnd           ; process quantum end
        or      eax, eax                ; check if new thread selected
        jne     short kdi60             ; if ne, new thread selected
        stdRET  _KiDispatchInterrupt    ; return

stdENDP _KiDispatchInterrupt

        page ,132
        subttl  "Swap Context to Next Thread"
;++
;
; Routine Description:
;
;    This routine is called to swap context from one thread to the next.
;    It swaps context, flushes the data, instruction, and translation
;    buffer caches, restores nonvolatile integer registers, and returns
;    to its caller.
;
;    N.B. It is assumed that the caller (only callers are within this
;         module) saved the nonvolatile registers, ebx, esi, edi, and
;         ebp. This enables the caller to have more registers available.
;
; Arguments:
;
;    cl - APC interrupt bypass disable (zero enable, nonzero disable).
;    edi - Address of previous thread.
;    esi - Address of next thread.
;    ebx - Address of PCR.
;
; Return value:
;
;    al - Kernel APC pending.
;    ebx - Address of PCR.
;    esi - Address of current thread object.
;
;--

        align   16
        public  SwapContext

SwapContext     proc


;
;   NOTE:   The ES: override on the move to ThState is part of the
;           lazy-segment load system.  It assures that ES has a valid
;           selector in it, thus preventing us from propagating a bad
;           ES accross a context switch.
;
;           Note that if segments, other than the standard flat segments,
;           with limits above 2 gig exist, neither this nor the rest of
;           lazy segment loads are reliable.
;
; Note that ThState must be set before the dispatcher lock is released
; to prevent KiSetPriorityThread from seeing a stale value.
;

;
; Save the APC disable flag and set new thread state to running.
;

        or      cl, cl                  ; set zf in flags
        mov     byte ptr es:[esi]+ThState, Running ; set thread state to running
        pushfd
cPublicFpo 0, 1

;
; Save the APC disable flag and the exception listhead.
; (also, check for DPC running which is illegal right now).
;

        mov     ecx, [ebx]+PcExceptionList ; save exception list
        cmp     [ebx]+PcPrcbData+PbDpcRoutineActive, 0
        push    ecx
cPublicFpo 0, 2
        jne     sc91                    ; bugcheck if DPC active.

;
; Notify the profiling function of the context switch.
;

ifdef DEVKIT

        cmp     _KiDbgCtxSwapNotify, 0
        jne     sc92
sc03:

endif ; DEVKIT

;
; On a uniprocessor system the NPX state is swapped in a lazy manner.
; If a thread whose state is not in the coprocessor attempts to perform
; a coprocessor operation, the current NPX state is swapped out (if needed),
; and the new state is swapped in durning the fault.  (KiTrap07)
;
; On a multiprocessor system we still fault in the NPX state on demand, but
; we save the state when the thread switches out (assuming the NPX state
; was loaded).  This is because it could be difficult to obtain the thread's
; NPX in the trap handler if it was loaded into a different processor's
; coprocessor.
;
        mov     ebp, cr0                ; get current CR0
        mov     edx, ebp

;
; Switch stacks:
;
;   1.  Save old esp in old thread object.
;   2.  Copy stack base and stack limit into TSS AND PCR
;   3.  Load esp from new thread object
;
; Keep interrupts off so we don't confuse the trap handler into thinking
; we've overrun the kernel stack.
;

        cli                             ; disable interrupts
        mov     [edi]+ThKernelStack, esp ; save old kernel stack pointer
        mov     eax, [esi]+ThStackBase  ; get new initial stack pointer
        mov     ecx, [esi]+ThStackLimit ; get stack limit
        sub     eax, NPX_FRAME_LENGTH   ; space for NPX_FRAME & NPX CR0 flags
        mov     [ebx]+PcStackLimit, ecx ; set new stack limit
        mov     [ebx]+PcStackBase, eax  ; set new stack base

.errnz (NPX_STATE_NOT_LOADED - CR0_TS - CR0_MP)
.errnz (NPX_STATE_LOADED - 0)

; (eax) = Initial Stack
; (ebx) = Prcb
; (edi) = OldThread
; (esi) = NewThread
; (ebp) = Current CR0
; (edx) = Current CR0

        xor     ecx, ecx
        mov     cl, [esi]+ThNpxState            ; New NPX state is (or is not) loaded

        and     edx, NOT (CR0_MP+CR0_EM+CR0_TS) ; clear thread settable NPX bits
        or      ecx, edx                        ; or in new thread's cr0
        or      ecx, [eax]+FpCr0NpxState        ; merge new thread settable state
        cmp     ebp, ecx                ; check if old and new CR0 match
        jne     sc_reload_cr0           ; if ne, no change in CR0

        align   4
sc06:   mov     esp, [esi]+ThKernelStack ; set new stack pointer
        sti                             ; enable interrupts

;
; Update context switch counters.
;

        inc     dword ptr [esi]+ThContextSwitches ; thread count
        inc     dword ptr [ebx]+PcPrcbData+PbContextSwitches ; processor count
        pop     ecx                     ; restore exception list
        mov     [ebx].PcExceptionList, ecx ;

;
; If the new thread has a kernel mode APC pending, then request an APC
; interrupt.
;

        cmp     byte ptr [esi].ThApcState.AsKernelApcPending, 0 ; APC pending?
        jne     short sc80              ; if ne, kernel APC pending
        popfd                           ; restore flags
        xor     eax, eax                ; clear kernel APC pending
        ret                             ; return

;
; The new thread has an APC interrupt pending. If APC interrupt bypass is
; enable, then return kernel APC pending. Otherwise, request a software
; interrupt at APC_LEVEL and return no kernel APC pending.
;

sc80:   popfd                           ; restore flags
        jnz     short sc90              ; if nz, APC interupt bypass disabled
        mov     al, 1                   ; set kernel APC pending
        ret                             ;

sc90:   mov     cl, APC_LEVEL           ; request software interrupt level
        fstCall HalRequestSoftwareInterrupt ;
        xor     eax, eax                ; clear kernel APC pending
        ret                             ;

;
; Cr0 has changed (ie, floating point processor present), load the new value.
;

sc_reload_cr0:
if DBG

        test    byte ptr [esi]+ThNpxState, NOT (CR0_TS+CR0_MP)
        jnz     sc_error                ;
        test    dword ptr [eax]+FpCr0NpxState, NOT (CR0_PE+CR0_MP+CR0_EM+CR0_TS)
        jnz     sc_error3               ;

endif
        mov     cr0,ecx                 ; set new CR0 NPX state
        jmp     sc06

;
; Notify context swap callout routine.  This code is out of line to
; optimize the normal case (which is expected to be the case where
; there is no callout routine).
;

ifdef DEVKIT

sc92:   mov     edx, [esi].EtUniqueThread ; set new thread unique id
        mov     ecx, [edi].EtUniqueThread ; set old thread unique id
        call    [_KiDbgCtxSwapNotify]     ; notify callout routine
        jmp     sc03

endif ; DEVKIT

.fpo (2, 0, 0, 0, 0, 0)
sc91:   stdCall _KeBugCheck <ATTEMPTED_SWITCH_FROM_DPC>
        ret                             ; return

if DBG
sc_error5:  int 3
sc_error4:  int 3
sc_error3:  int 3
sc_error2:  int 3
sc_error:   int 3
endif

SwapContext     endp

        page , 132
        subttl "Flush EntireTranslation Buffer"
;++
;
; VOID
; KeFlushCurrentTb (
;     )
;
; Routine Description:
;
;     This function flushes the entire translation buffer (TB) on the current
;     processor and also flushes the data cache if an entry in the translation
;     buffer has become invalid.
;
; Arguments:
;
; Return Value:
;
;     None.
;
;--

cPublicProc _KeFlushCurrentTb ,0

        mov     eax, cr3                ; (eax) = directory table base
        mov     cr3, eax                ; flush TLB
        stdRET    _KeFlushCurrentTb

stdENDP _KeFlushCurrentTb

cPublicProc _KeFlushCurrentTbAndInvalidateAllCaches ,0

        mov     eax, cr3                ; (eax) = directory table base
        mov     cr3, eax                ; flush TLB

cPublicProc _KeInvalidateAllCaches ,0

        wbinvd
        stdRET    _KeFlushCurrentTbAndInvalidateAllCaches

stdENDP _KeInvalidateAllCaches

stdENDP _KeFlushCurrentTbAndInvalidateAllCaches

_TEXT   ends
        end
