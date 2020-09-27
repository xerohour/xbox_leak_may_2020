        title  "System Startup"
;++
;
; Copyright (c) 1989-2001  Microsoft Corporation
;
; Module Name:
;
;    newsysbg.asm
;
; Abstract:
;
;    This module implements the code necessary to initially startup the
;    NT system.
;
; Environment:
;
;    Kernel mode only.
;
;--
.386p
        .xlist
include ks386.inc
include i386\kimacro.inc
include callconv.inc
FPOFRAME macro a, b
.FPO ( a, b, 0, 0, 0, 0 )
endm
        .list

        option  segment:flat

        EXTRNP  _KdInitSystem,1
        EXTRNP  KfRaiseIrql,1,,FASTCALL
        EXTRNP  KfLowerIrql,1,,FASTCALL
        EXTRNP  _KiPreInitializeKernel
        EXTRNP  _KiInitializeKernel
        extrn   SwapContext:PROC
        EXTRNP  _KiSwapGDT
        EXTRNP  _KiSwapIDT
        extrn   _KiTrap08:PROC
        extrn   _KiTrap02:PROC
        EXTRNP  _KeGetCurrentIrql,0
        EXTRNP  _KeBugCheckEx, 5
        EXTRNP  _HalMicrocodeUpdateLoader,0
        EXTRNP  HalClearSoftwareInterrupt,1,,FASTCALL
        EXTRNP  _MmPrepareToQuickRebootSystem,0

        extrn   _KeHasQuickBooted:BYTE
        extrn   _KiIDT:BYTE
        extrn   _KiIDTLEN:BYTE          ; NOTE - really an ABS, linker problems

ifdef DEVKIT
        extrn   _KiDpcDispatchNotify:DWORD
endif

if DBG
        extrn   _KdDebuggerEnabled:BYTE
        EXTRNP  _DbgBreakPoint,0
        extrn   _DbgPrint:near
        extrn   _MsgDpcTrashedEsp:BYTE
        extrn   _MsgDpcFpuEnabled:BYTE
endif

;
; Constants for various variables
;

_DATA   SEGMENT PARA PUBLIC 'DATA'

COMM    _KiDoubleFaultStackLimit:byte:DOUBLE_FAULT_STACK_SIZE
COMM    _KiIdleThreadStackLimit:byte:KERNEL_STACK_SIZE

        align   16
        public  _KiIdleThread
_KiIdleThread  label   byte
        db      ExtendedThreadObjectLength dup(0) ; sizeof (ETHREAD)

        align   16
        dd      3 dup(0) ; padding to force PbNpxSaveArea to be 16 byte aligned
.errnz ((SIZEOF DWORD * 3 + PcPrcbData + PbNpxSaveArea) AND 15)
        public  _KiPCR
_KiPCR label dword
        db      ProcessorControlRegisterLength dup (0)

_DATA   ends

STICKY  SEGMENT PARA PUBLIC 'DATA'

;
; Allocate space for the task state segments.
;

INITIALIZE_TSS macro HandlerRoutine, HandlerStack
        dw      0                       ; TssBacklink
        dw      0                       ; TssReserved0
        dd      HandlerStack            ; TssEsp0
        dw      KGDT_R0_DATA            ; TssSs0
        dw      0                       ; TssReserved1
        dd      0                       ; TssEsp1
        dw      0                       ; TssSs1
        dw      0                       ; TssReserved2
        dd      0                       ; TssEsp2
        dw      0                       ; TssSs2
        dw      0                       ; TssReserved3
        dd      0                       ; TssCR3
        dd      HandlerRoutine          ; TssEip
        dd      0                       ; TssEFlags
        dd      0                       ; TssEax
        dd      0                       ; TssEcx
        dd      0                       ; TssEdx
        dd      0                       ; TssEbx
        dd      HandlerStack            ; TssEsp
        dd      0                       ; TssEbp
        dd      0                       ; TssEsi
        dd      0                       ; TssEdi
        dw      KGDT_R0_DATA            ; TssEs
        dw      0                       ; TssReserved4
        dw      KGDT_R0_CODE            ; TssCs
        dw      0                       ; TssReserved5
        dw      KGDT_R0_DATA            ; TssSs
        dw      0                       ; TssReserved6
        dw      KGDT_R0_DATA            ; TssDs
        dw      0                       ; TssReserved7
        dw      KGDT_R0_PCR             ; TssFs
        dw      0                       ; TssReserved8
        dw      0                       ; TssGs
        dw      0                       ; TssReserved9
        dw      0                       ; TssLDT
        dw      0                       ; TssReserved10
        dw      0                       ; TssFlags
        dw      TssIoMaps               ; TssIoMapBase
        endm

        align   8
        public  _KiNormalTSS
_KiNormalTSS label byte
INITIALIZE_TSS 0, 0

        align   8
        public  _KiDoubleFaultTSS
_KiDoubleFaultTSS label byte
INITIALIZE_TSS <OFFSET _KiTrap08>, <OFFSET _KiDoubleFaultStackLimit + DOUBLE_FAULT_STACK_SIZE>

        align   8
        public  _KiNMITSS
_KiNMITSS label byte
INITIALIZE_TSS <OFFSET _KiTrap02>, <OFFSET _KiDoubleFaultStackLimit + DOUBLE_FAULT_STACK_SIZE>

;
; Allocate space for the global descriptor table.
;

GDTEntry macro  base,limit,flags
        dw      limit
        dd      base
        dw      flags
        endm

        align   8
        public  _KiGDT, _KiGDTLEN, _KiGDTEnd
_KiGDT label byte

GDTEntry        0, 0, 0                                                 ; KGDT_NULL
GDTEntry        0, 0FFFFh, 0CB9Bh                                       ; KGDT_R0_CODE
GDTEntry        0, 0FFFFh, 0CF93h                                       ; KGDT_R0_DATA
GDTEntry        <OFFSET _KiNormalTSS>, TssIoMaps, 89h                   ; KGDT_TSS
GDTEntry        <OFFSET _KiPCR>, ProcessorControlRegisterLength, 0C093h ; KGDT_R0_PCR
GDTEntry        <OFFSET _KiDoubleFaultTSS>, TssIoMaps, 89h              ; KGDT_DF_TSS
GDTEntry        <OFFSET _KiNMITSS>, TssIoMaps, 89h                      ; KGDT_NMI_TSS

_KiGDTLEN       equ     $ - _KiGDT
_KiGDTEnd       equ     $

STICKY  ends

        page ,132
        subttl  "System Startup"
INIT    SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
;
; For processor 0, Routine Description:
;
;    This routine is called when the NT system begins execution.
;    Its function is to initialize system hardware state, call the
;    kernel initialization routine, and then fall into code that
;    represents the idle thread for all processors.
;
;    Entry state created by the boot loader:
;       A short-form IDT (0-1f) exists and is active.
;       A complete GDT is set up and loaded.
;       A complete TSS is set up and loaded.
;       Page map is set up with minimal start pages loaded
;           The lower 4Mb of virtual memory are directly mapped into
;           physical memory.
;
;           The system code (ntoskrnl.exe) is mapped into virtual memory
;           as described by its memory descriptor.
;       DS=ES=SS = flat
;       ESP->a usable boot stack
;       Interrupts OFF
;
; For processor > 0, Routine Description:
;
;   This routine is called when each additional processor begins execution.
;   The entry state for the processor is:
;       IDT, GDT, TSS, stack, selectors, PCR = all valid
;       Page directory is set to the current running directory
;
; Arguments:
;
;    None.
;
; Return Value:
;
;    None.
;
;--

cPublicProc _KiSystemStartup,0

;
; Initialize ebp, esp, and argument registers for initializing the kernel.
; Reserve space for idle thread stack NPX_SAVE_AREA, enough padding bytes to
; store the initial context from KiInitializeContextThread and the FWORD for
; lgdt/lidt.
;

        xor     ebp, ebp                ; (ebp) = 0.   No more stack frame
        mov     esp, offset _KiIdleThreadStackLimit + KERNEL_STACK_SIZE - NPX_FRAME_LENGTH - 64

;
; Rearrange the bytes in KiGDT to match the processor's layout.
;

        stdCall _KiSwapGDT

;
; Load the global descriptor table register.
;

        mov     WORD PTR [esp], _KiGDTLEN
        mov     DWORD PTR [esp+2], OFFSET _KiGDT
        lgdt    FWORD PTR [esp]

;
; Execute a 16:32 jump to reload the code selector.
;

        db      0EAh
        dd      OFFSET kis10
        dw      KGDT_R0_CODE

;
; Reload the data selectors.
;

kis10:  mov     eax, KGDT_R0_DATA
        mov     ds, eax
        mov     es, eax
        mov     ss, eax

;
; Load the various selector registers.
;

        push    KGDT_R0_PCR
        pop     fs

        mov     eax, KGDT_TSS
        ltr     ax

;
; Initialize TssCr3 of the task state segments to the boot page directory.
;

        mov     eax, cr3
        mov     dword ptr [_KiNormalTSS]+TssCr3, eax
        mov     dword ptr [_KiDoubleFaultTSS]+TssCr3, eax
        mov     dword ptr [_KiNMITSS]+TssCr3, eax

;
; Update the microcode for the processor.
;

        stdCall _HalMicrocodeUpdateLoader

;
; Since the entries of Kernel IDT have their Selector and Extended Offset
; fields set up in the wrong order, we need to swap them back to the order
; which i386 recognizes.
;

        stdCall _KiSwapIDT

;
; Load the interrupt descriptor table register.
;

        mov     ecx, offset FLAT:_KiIDTLEN
        mov     WORD PTR [esp], cx
        mov     DWORD PTR [esp+2], OFFSET _KiIDT
        lidt    FWORD PTR [esp]

;
; Preinitialize the system data structures.
;

        stdCall _KiPreInitializeKernel

;
; Enable the kernel debugger.
;

        stdCall _KdInitSystem, <1>

if  DEVL
;
; Give the debugger an opportunity to gain control.
;

        POLL_DEBUGGER
endif   ; DEVL
        nop                             ; leave a spot for int-3 patch

;
; Set initial IRQL = HIGH_LEVEL for init
;

        mov     ecx, HIGH_LEVEL
        fstCall KfRaiseIrql

;
; Initialize system data structures and HAL.
;

        stdCall _KiInitializeKernel

;
; Set "shadow" priority value for Idle thread.  This will keep the Mutex
; priority boost/drop code from dropping priority on the Idle thread, and
; thus avoids leaving a bit set in the ActiveMatrix for the Idle thread when
; there should never be any such bit.
;

        mov     byte ptr [_KiIdleThread]+ThPriority,LOW_REALTIME_PRIORITY

;
; Control is returned to the idle thread with IRQL at HIGH_LEVEL. Lower IRQL
; to DISPATCH_LEVEL and set wait IRQL of idle thread.
;

        sti
        mov     ecx, DISPATCH_LEVEL
        fstCall KfLowerIrql
        mov     byte ptr [_KiIdleThread]+ThWaitIrql, DISPATCH_LEVEL

;
; The following code represents the idle thread for a processor. The idle
; thread executes at IRQL DISPATCH_LEVEL and continually polls for work to
; do. Control may be given to this loop either as a result of a return from
; the system initialization routine or as the result of starting up another
; processor in a multiprocessor configuration.
;

        jmp     KiIdleLoop              ; enter idle loop

stdENDP _KiSystemStartup

INIT   ends

_TEXT   SEGMENT DWORD PUBLIC 'CODE'      ; Put IdleLoop in text section
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
;
; DECLSPEC_NORETURN
; VOID
; KeQuickRebootSystem(
;     VOID
;     )
;
; Routine Description:
;
;     This function quick reboots the system.  The .data section is refreshed
;     from the ROM and system initialization is started again.
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
cPublicProc _KeQuickRebootSystem,0

;
; Disable interrupts.
;

        cli

;
; Initialize ebp, esp, and argument registers for initializing the kernel.
; Reserve space for idle thread stack NPX_SAVE_AREA, enough padding bytes to
; store the initial context from KiInitializeContextThread and the FWORD for
; lgdt/lidt.
;

        xor     ebp, ebp                ; (ebp) = 0.   No more stack frame
        mov     esp, offset _KiIdleThreadStackLimit + KERNEL_STACK_SIZE - NPX_FRAME_LENGTH - 64

;
; Inform the memory manager that we're about to quick reboot the system.  This
; will cause us to switch back to large pages and to make the ROM visible so
; that we can copy out our .data section again.
;

        stdCall _MmPrepareToQuickRebootSystem

;
; Copy the initialized portion of the .data section from ROM.  Note that the
; stack that we're running on is part of this data image, so don't use the
; stack here or in the next block.
;

        cld
        mov     ecx, dword ptr ds:[MzXdataSectionHeader]+XdshSizeOfInitializedData
        shr     ecx, 2
        mov     esi, dword ptr ds:[MzXdataSectionHeader]+XdshPointerToRawData
        mov     edi, dword ptr ds:[MzXdataSectionHeader]+XdshVirtualAddress
        rep     movsd

;
; Zero out the uninitialized portion of the .data section.
;

        mov     ecx, dword ptr ds:[MzXdataSectionHeader]+XdshSizeOfUninitializedData
        xor     eax, eax
        shr     ecx, 2
        rep     stosd

;
; Indicate to the rest of the system that we're initializing due to a quick
; boot.
;

        mov     byte ptr [_KeHasQuickBooted], 1

;
; Since the entries of Kernel IDT have their Selector and Extended Offset
; fields set up in the wrong order, we need to swap them back to the order
; which i386 recognizes.
;

        stdCall _KiSwapIDT

;
; Load the interrupt descriptor table register.
;

        mov     ecx, offset FLAT:_KiIDTLEN
        mov     WORD PTR [esp], cx
        mov     DWORD PTR [esp+2], OFFSET _KiIDT
        lidt    FWORD PTR [esp]

;
; Preinitialize the system data structures.
;

        stdCall _KiPreInitializeKernel

;
; Enable the kernel debugger.
;

        stdCall _KdInitSystem, <1>

;
; Set initial IRQL = HIGH_LEVEL for init
;

        mov     ecx, HIGH_LEVEL
        fstCall KfRaiseIrql

;
; Initialize system data structures and HAL.
;

        stdCall _KiInitializeKernel

;
; Set "shadow" priority value for Idle thread.  This will keep the Mutex
; priority boost/drop code from dropping priority on the Idle thread, and
; thus avoids leaving a bit set in the ActiveMatrix for the Idle thread when
; there should never be any such bit.
;

        mov     byte ptr [_KiIdleThread]+ThPriority,LOW_REALTIME_PRIORITY

;
; Control is returned to the idle thread with IRQL at HIGH_LEVEL. Lower IRQL
; to DISPATCH_LEVEL and set wait IRQL of idle thread.
;

        sti
        mov     ecx, DISPATCH_LEVEL
        fstCall KfLowerIrql
        mov     byte ptr [_KiIdleThread]+ThWaitIrql, DISPATCH_LEVEL

;
; The following code represents the idle thread for a processor. The idle
; thread executes at IRQL DISPATCH_LEVEL and continually polls for work to
; do. Control may be given to this loop either as a result of a return from
; the system initialization routine or as the result of starting up another
; processor in a multiprocessor configuration.
;

        jmp     KiIdleLoop              ; enter idle loop

stdENDP _KeQuickRebootSystem

        page ,132
        subttl  "Idle Loop"
;++
;
; Routine Description:
;
;    This routine continuously executes the idle loop and never returns.
;
; Arguments:
;
;    None.
;
; Return value:
;
;    None - routine never returns.
;
;--

        public  KiIdleLoop
KiIdleLoop proc

        lea     ebx, _KiPCR             ; get address of PCR
        lea     ebp, [ebx].PcPrcbData.PbDpcListHead ; set DPC listhead address

kid10:

;
; N.B. The following code enables interrupts for a few cycles, then
;      disables them again for the subsequent DPC and next thread
;      checks.
;

        sti                             ; enable interrupts
        nop                             ;
        nop                             ;
        cli                             ; disable interrupts

;
; Process the deferred procedure call list for the current processor.
;

        cmp     ebp, [ebp].LsFlink      ; check if DPC list is empty
        je      short CheckNextThread   ; if eq, DPC list is empty
        mov     cl, DISPATCH_LEVEL      ; set interrupt level
        fstCall HalClearSoftwareInterrupt ; clear software interrupt
        call    KiRetireDpcList         ; process the current DPC list

;
; Check if a thread has been selected to run on the current processor.
;

CheckNextThread:                        ;
        cmp     dword ptr [ebx].PcPrcbData.PbNextThread, 0 ; thread selected?
        je      short kid10             ; if eq, no thread selected

;
; A thread has been selected for execution on this processor. Acquire
; the dispatcher database lock, get the thread address again (it may have
; changed), clear the address of the next thread in the processor block,
; and call swap context to start execution of the selected thread.
;
; N.B. If the dispatcher database lock cannot be obtained immediately,
;      then attempt to process another DPC rather than spinning on the
;      dispatcher database lock.
;

        sti                             ; enable interrupts
        mov     esi, [ebx].PcPrcbData.PbNextThread ; get next thread address
        mov     edi, [ebx].PcPrcbData.PbCurrentThread ; set current thread address
        mov     dword ptr [ebx].PcPrcbData.PbNextThread, 0 ; clear next thread address
        mov     [ebx].PcPrcbData.PbCurrentThread, esi ; set current thread address

        mov     cl, 1                   ; set APC interrupt bypass disable
        call    SwapContext             ;

        lea     ebp, [ebx].PcPrcbData.PbDpcListHead ; set DPC listhead address
        jmp     kid10

KiIdleLoop      endp

        page ,132
        subttl  "Retire DPC List Loop"
;++
;
; Routine Description:
;
;    This routine continuously executes a retire DPC loop and never returns.
;
; Arguments:
;
;    None.
;
; Return value:
;
;    None - routine never returns.
;
;--

cPublicProc _KeRetireDpcListLoop,0

        lea     ebx, _KiPCR             ; get address of PCR
        lea     ebp, [ebx].PcPrcbData.PbDpcListHead ; set DPC listhead address

;
; This routine is entered at DPC level from a HAL DPC routine.  Clear the flag
; that indicates that we're already executing a DPC so that the debug code in
; the clock interrupt won't fire because we're taking too long to execute.
;

        mov     [ebx].PcPrcbData.PbDpcRoutineActive, 0

krd10:

;
; N.B. The following code enables interrupts for a few cycles, then
;      disables them again for the subsequent DPC and next thread
;      checks.
;

        sti                             ; enable interrupts
        nop                             ;
        nop                             ;
        cli                             ; disable interrupts

;
; Process the deferred procedure call list for the current processor.
;

        cmp     ebp, [ebp].LsFlink      ; check if DPC list is empty
        je      short krd10             ; if eq, DPC list is empty
        mov     cl, DISPATCH_LEVEL      ; set interrupt level
        fstCall HalClearSoftwareInterrupt ; clear software interrupt
        call    KiRetireDpcList         ; process the current DPC list
        jmp     short krd10

stdENDP _KeRetireDpcListLoop

        page ,132
        subttl  "Retire Deferred Procedure Call List"
;++
;
; Routine Description:
;
;    This routine is called to retire the specified deferred procedure
;    call list. DPC routines are called using the idle thread (current)
;    stack.
;
;    N.B. Interrupts must be disabled and the DPC list lock held on entry
;         to this routine. Control is returned to the caller with the same
;         conditions true.
;
;    N.B. The registers ebx and ebp are preserved across the call.
;
; Arguments:
;
;    ebx - Address of the target processor PCR.
;    ebp - Address of the target DPC listhead.
;
; Return value:
;
;    None.
;
;--

        public  KiRetireDpcList
KiRetireDpcList proc

?FpoValue = 0

FPOFRAME ?FpoValue,0

;
; Disable the FPU to catch the use of floating point from a DPC without first
; calling KeSaveFloatingPointState and KeRestoreFloatingPointState.
;

if DBG
        mov     eax, PCR[PcPrcbData+PbCurrentThread]
        cmp     byte ptr [eax].ThNpxState, NPX_STATE_LOADED
        jne     rdl5
        mov     eax, cr0                ; retrieve CR0
        or      eax, CR0_MP+CR0_TS
        lmsw    ax                      ; load new CR0 (low 16-bits)
endif

rdl5:   mov     PCR[PcPrcbData.PbDpcRoutineActive], esp ; set DPC routine active

;
; Process the DPC List.
;

rdl10:                                  ;

        mov     edx, [ebp].LsFlink      ; get address of next entry
        mov     ecx, [edx].LsFlink      ; get address of next entry
        mov     [ebp].LsFlink, ecx      ; set address of next in header
        mov     [ecx].LsBlink, ebp      ; set address of previous in next
        sub     edx, DpDpcListEntry     ; compute address of DPC object
        mov     ecx, [edx].DpDeferredRoutine ; get DPC routine address

if DBG
        push    ecx                     ; remember DPC routine address
        push    edi                     ; save register
        mov     edi, esp                ; save current stack pointer
endif

ifdef DEVKIT
        push    esi                     ; save esi register
endif

FPOFRAME ?FpoValue,0

        push    [edx].DpSystemArgument2 ; second system argument
        push    [edx].DpSystemArgument1 ; first system argument
        push    [edx].DpDeferredContext ; get deferred context argument
        push    edx                     ; address of DPC object
        mov     byte ptr [edx]+DpInserted, 0 ; clear DPC inserted state
if DBG
        mov     PCR[PcPrcbData.PbDebugDpcTime], 0 ; Reset the time in DPC
endif

        sti                             ; enable interrupts

;
; If the profiler is active, call its notification routine.
;
ifdef DEVKIT
        cmp     _KiDpcDispatchNotify, 0
        jne     rdl70
        call    ecx                     ; call DPC routine
rdl20:  pop     esi                     ; restore esi

else
        call    ecx                     ; call DPC routine
endif

if DBG

;
; Verify that the DPC returned with the FPU disabled.  If the FPU is enabled,
; then the DPC probably forgot to call KeRestoreFloatingPointState.
;

        mov     eax, cr0                ; retrieve CR0
        test    al, CR0_TS              ; test if FPU is still disabled
        je      rdl57                   ; if e, FPU not disabled

;
; Verify that the DPC returned at DISPATCH_LEVEL.
;

rdl25:  movzx   eax, byte ptr PCR[PcIrql] ; get current IRQL
        cmp     al, DISPATCH_LEVEL      ; check if still at dispatch level
        jne     rdl55                   ; if ne, not at dispatch level
        cmp     esp, edi                ; check if stack pointer is correct
        jne     rdl60                   ; if ne, stack pointer is not correct
rdl30:  pop     edi                     ; restore register
        pop     ecx                     ; pop off saved DPC routine address

endif

FPOFRAME ?FpoValue,0

rdl35:  cli                             ; disable interrupts
        cmp     ebp, [ebp].LsFlink      ; check if DPC list is empty
        jne     rdl10                   ; if ne, DPC list not empty

;
; Clear DPC routine active and DPC requested flags.
;

rdl40:  mov     [ebx].PcPrcbData.PbDpcRoutineActive, 0
        mov     [ebx].PcPrcbData.PbDpcInterruptRequested, 0

;
; Check one last time that the DPC list is empty. This is required to
; close a race condition with the DPC queuing code where it appears that
; a DPC routine is active (and thus an interrupt is not requested), but
; this code has decided the DPC list is empty and is clearing the DPC
; active flag.
;

        cmp     ebp, [ebp].LsFlink      ; check if DPC list is empty
        jne     rdl5                    ; if ne, DPC list not empty

;
; Restore the original CR0 if the current thread still owns the FPU.  Note that
; if a DPC called KeSaveFloatingPointState or hit the debugger, then the FPU
; state was flushed, so we shouldn't enable the FPU.
;

if DBG
        mov     ecx, PCR[PcPrcbData+PbCurrentThread]
        cmp     byte ptr [ecx].ThNpxState, NPX_STATE_LOADED
        jne     rdl50

        mov     eax, cr0
        mov     edx, [ebx].PcStackBase
        and     eax, NOT (CR0_MP+CR0_EM+CR0_TS)
        or      eax, [edx].FpCr0NpxState
        lmsw    ax                      ; load new CR0 (low 16-bits)
endif

rdl50:  ret                             ; return

;
; Unlock DPC list and clear DPC active.
;

if DBG

rdl55:  stdCall _KeBugCheckEx, <IRQL_NOT_GREATER_OR_EQUAL, ebx, eax, 0, 0> ;

rdl57:  push    dword ptr [edi+4]
        push    offset FLAT:_MsgDpcFpuEnabled ; push message address
        call    _DbgPrint               ; print debug message
        add     esp, 8                  ; remove arguments from stack
        int     3                       ; break into debugger
        mov     esp, edi                ; reset stack pointer
        jmp     rdl25                   ;

rdl60:  push    dword ptr [edi+4]       ; push address of DPC function
        push    offset FLAT:_MsgDpcTrashedEsp ; push message address
        call    _DbgPrint               ; print debug message
        add     esp, 8                  ; remove arguments from stack
        int     3                       ; break into debugger
        mov     esp, edi                ; reset stack pointer
        jmp     rdl30                   ;

endif

;
; Notify the profiler that a DPC routine is called:
;
;   VOID (FASTCALL *KiDpcDispatchNotify)(VOID*, ULONG)
;

ifdef DEVKIT
rdl70:  mov     esi, ecx                ; save DPC routine address in esi
        xor     edx, edx                ; KiDpcDispatchNotify(dpcproc, 0)
        call    [_KiDpcDispatchNotify]

        call    esi                     ; call the DPC routine

        mov     ecx, esi                ; KiDpcDispatchNotify(dpcproc, 1)
        mov     edx, 1
        call    [_KiDpcDispatchNotify]
        jmp     rdl20
endif

KiRetireDpcList endp

_TEXT   ends

        end
