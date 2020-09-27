
;
;  Pointer size in bytes
;

SizeofPointer equ 00004H

;
;  Thread State Enumerated Type Values
;

Initialized equ 00000H
Ready equ 00001H
Running equ 00002H
Standby equ 00003H
Terminated equ 00004H
Waiting equ 00005H

;
;  Wait Reason and Wait Type Enumerated Type Values
;

WrExecutive equ 00000H
WrEventPair equ 0000EH
WaitAny equ 00001H
WaitAll equ 00000H

;
;  Apc State Structure Offset Definitions
;

AsApcListHead equ 00000H
AsProcess equ 00010H
AsKernelApcInProgress equ 00014H
AsKernelApcPending equ 00015H
AsUserApcPending equ 00016H
AsApcQueueable equ 00017H

;
;  Bug Check Code Definitions
;

APC_INDEX_MISMATCH equ 00001H
ATTEMPTED_FPU_USE_FROM_DPC equ 000F0H
ATTEMPTED_SWITCH_FROM_DPC equ 000B8H
DATA_BUS_ERROR equ 0002EH
DATA_COHERENCY_EXCEPTION equ 00055H
HAL1_INITIALIZATION_FAILED equ 00061H
INSTRUCTION_BUS_ERROR equ 0002FH
INSTRUCTION_COHERENCY_EXCEPTION equ 00056H
INTERRUPT_EXCEPTION_NOT_HANDLED equ 0003DH
INTERRUPT_UNWIND_ATTEMPTED equ 0003CH
INVALID_AFFINITY_SET equ 00003H
INVALID_DATA_ACCESS_TRAP equ 00004H
IRQL_GT_ZERO_AT_SYSTEM_SERVICE equ 0004AH
IRQL_NOT_LESS_OR_EQUAL equ 0000AH
KMODE_EXCEPTION_NOT_HANDLED equ 0001EH
NMI_HARDWARE_FAILURE equ 00080H
NO_USER_MODE_CONTEXT equ 0000EH
PAGE_FAULT_WITH_INTERRUPTS_OFF equ 00049H
PANIC_STACK_SWITCH equ 0002BH
SPIN_LOCK_INIT_FAILURE equ 00081H
SYSTEM_EXIT_OWNED_MUTEX equ 00039H
SYSTEM_SERVICE_EXCEPTION equ 0003BH
SYSTEM_UNWIND_PREVIOUS_USER equ 0003AH
TRAP_CAUSE_UNKNOWN equ 00012H
UNEXPECTED_KERNEL_MODE_TRAP equ 0007FH

;
;  Breakpoint type definitions
;

DBG_STATUS_CONTROL_C equ 00001H

;
;  Client Id Structure Offset Definitions
;

CidUniqueProcess equ 00000H
CidUniqueThread equ 00004H

;
;  Critical Section Structure Offset Definitions
;

CsLockCount equ 00010H
CsRecursionCount equ 00014H
CsOwningThread equ 00018H

;
;  File Object Structure Offset Definitions
;

FoLockCount equ 00024H
FoLock equ 00028H

;
;  Exception Record Offset, Flag, and Enumerated Type Definitions
;

EXCEPTION_NONCONTINUABLE equ 00001H
EXCEPTION_UNWINDING equ 00002H
EXCEPTION_EXIT_UNWIND equ 00004H
EXCEPTION_STACK_INVALID equ 00008H
EXCEPTION_NESTED_CALL equ 00010H
EXCEPTION_TARGET_UNWIND equ 00020H
EXCEPTION_COLLIDED_UNWIND equ 00040H
EXCEPTION_UNWIND equ 00066H
EXCEPTION_EXECUTE_HANDLER equ 00001H
EXCEPTION_CONTINUE_SEARCH equ 00000H
EXCEPTION_CONTINUE_EXECUTION equ 0FFFFFFFFH
EXCEPTION_CHAIN_END equ 0FFFFFFFFH

ExceptionContinueExecution equ 00000H
ExceptionContinueSearch equ 00001H
ExceptionNestedException equ 00002H
ExceptionCollidedUnwind equ 00003H

ErExceptionCode equ 00000H
ErExceptionFlags equ 00004H
ErExceptionRecord equ 00008H
ErExceptionAddress equ 0000CH
ErNumberParameters equ 00010H
ErExceptionInformation equ 00014H
ExceptionRecordLength equ 00050H

;
;  Interrupt Priority Request Level Definitions
;

PASSIVE_LEVEL equ 00000H
APC_LEVEL equ 00001H
DISPATCH_LEVEL equ 00002H
SCI_LEVEL equ 0001BH
SMBUS_LEVEL equ 0000FH
CLOCK_LEVEL equ 0001CH
IPI_LEVEL equ 0001DH
POWER_LEVEL equ 0001EH
PROFILE_LEVEL equ 0001AH
HIGH_LEVEL equ 0001FH
SYNCH_LEVEL equ 0001CH

;
;  Large Integer Structure Offset Definitions
;

LiLowPart equ 00000H
LiHighPart equ 00004H

;
;  List Entry Structure Offset Definitions
;

LsFlink equ 00000H
LsBlink equ 00004H

;
;  String Structure Offset Definitions
;

StrLength equ 00000H
StrMaximumLength equ 00002H
StrBuffer equ 00004H

;
;  System Time Structure Offset Definitions
;

StLowTime equ 00000H
StHigh1Time equ 00004H
StHigh2Time equ 00008H

;
;  Time Structure Offset Definitions
;

TmLowTime equ 00000H
TmHighTime equ 00004H

;
;  Status Code Definitions
;

STATUS_ACCESS_VIOLATION equ 0C0000005H
STATUS_ALERTED equ 00101H
STATUS_ARRAY_BOUNDS_EXCEEDED equ 0C000008CH
STATUS_BAD_COMPRESSION_BUFFER equ 0C0000242H
STATUS_BREAKPOINT equ 080000003H
STATUS_DATATYPE_MISALIGNMENT equ 080000002H
STATUS_DEVICE_PROTOCOL_ERROR equ 0C0000186H
STATUS_FLOAT_DENORMAL_OPERAND equ 0C000008DH
STATUS_FLOAT_DIVIDE_BY_ZERO equ 0C000008EH
STATUS_FLOAT_INEXACT_RESULT equ 0C000008FH
STATUS_FLOAT_INVALID_OPERATION equ 0C0000090H
STATUS_FLOAT_OVERFLOW equ 0C0000091H
STATUS_FLOAT_STACK_CHECK equ 0C0000092H
STATUS_FLOAT_UNDERFLOW equ 0C0000093H
STATUS_FLOAT_MULTIPLE_FAULTS equ 0C00002B4H
STATUS_FLOAT_MULTIPLE_TRAPS equ 0C00002B5H
STATUS_GUARD_PAGE_VIOLATION equ 080000001H
STATUS_ILLEGAL_FLOAT_CONTEXT equ 0C000014AH
STATUS_ILLEGAL_INSTRUCTION equ 0C000001DH
STATUS_INSTRUCTION_MISALIGNMENT equ 0C00000AAH
STATUS_INVALID_HANDLE equ 0C0000008H
STATUS_INVALID_LOCK_SEQUENCE equ 0C000001EH
STATUS_INVALID_OWNER equ 0C000005AH
STATUS_INVALID_PARAMETER equ 0C000000DH
STATUS_INVALID_PARAMETER_1 equ 0C00000EFH
STATUS_INVALID_SYSTEM_SERVICE equ 0C000001CH
STATUS_INTEGER_DIVIDE_BY_ZERO equ 0C0000094H
STATUS_INTEGER_OVERFLOW equ 0C0000095H
STATUS_IN_PAGE_ERROR equ 0C0000006H
STATUS_IO_DEVICE_ERROR equ 0C0000185H
STATUS_IO_TIMEOUT equ 0C00000B5H
STATUS_KERNEL_APC equ 00100H
STATUS_LONGJUMP equ 080000026H
STATUS_NO_CALLBACK_ACTIVE equ 0C0000258H
STATUS_NO_EVENT_PAIR equ 0C000014EH
STATUS_NO_MEMORY equ 0C0000017H
STATUS_PRIVILEGED_INSTRUCTION equ 0C0000096H
STATUS_SINGLE_STEP equ 080000004H
STATUS_STACK_OVERFLOW equ 0C00000FDH
STATUS_SUCCESS equ 00000H
STATUS_THREAD_IS_TERMINATING equ 0C000004BH
STATUS_TIMEOUT equ 00102H
STATUS_UNWIND equ 0C0000027H
STATUS_WAKE_SYSTEM_DEBUGGER equ 080000007H

;
;  APC Object Structure Offset Definitions
;

ApType equ 00000H
ApApcMode equ 00002H
ApInserted equ 00003H
ApThread equ 00004H
ApApcListEntry equ 00008H
ApKernelRoutine equ 00010H
ApRundownRoutine equ 00014H
ApNormalRoutine equ 00018H
ApNormalContext equ 0001CH
ApSystemArgument1 equ 00020H
ApSystemArgument2 equ 00024H
ApcObjectLength equ 00028H

;
;  DPC object Structure Offset Definitions
;

DpType equ 00000H
DpInserted equ 00002H
DpDpcListEntry equ 00004H
DpDeferredRoutine equ 0000CH
DpDeferredContext equ 00010H
DpSystemArgument1 equ 00014H
DpSystemArgument2 equ 00018H
DpcObjectLength equ 0001CH

;
;  Device Queue Object Structure Offset Definitions
;

DvType equ 00000H
DvSize equ 00002H
DvBusy equ 00003H
DvDeviceListHead equ 00004H
DeviceQueueObjectLength equ 0000CH

;
;  Device Queue Entry Structure Offset Definitions
;

DeDeviceListEntry equ 00000H
DeSortKey equ 00008H
DeInserted equ 0000CH
DeviceQueueEntryLength equ 00010H

;
;  Event Object Structure Offset Definitions
;

EvType equ 00000H
EvSize equ 00002H
EvSignalState equ 00004H
EvWaitListHead equ 00008H
EventObjectLength equ 00010H

;
;  Interrupt Object Structure Offset Definitions
;

InLevelSensitive equ 00000H
InLatched equ 00001H

InServiceRoutine equ 00000H
InServiceContext equ 00004H
InBusInterruptLevel equ 00008H
InIrql equ 0000CH
InConnected equ 00010H
InMode equ 00012H
InShareVector equ 00011H
InDispatchCode equ 00018H
InterruptObjectLength equ 00070H

NORMAL_DISPATCH_LENGTH equ 00058H
DISPATCH_LENGTH equ 00058H

;
;  Process Object Structure Offset Definitions
;

PrType equ 00000H
PrSize equ 00002H
PrSignalState equ 00004H
PrReadyListHead equ 00000H
PrThreadListHead equ 00008H
PrStackCount equ 00010H
PrBasePriority equ 00018H
PrThreadQuantum equ 00014H
ProcessObjectLength equ 00020H

;
;  Queue Object Structure Offset Definitions
;

QuType equ 00000H
QuSize equ 00002H
QuSignalState equ 00004H
QuEntryListHead equ 00010H
QuCurrentCount equ 00018H
QuMaximumCount equ 0001CH
QuThreadListHead equ 00020H
QueueObjectLength equ 00028H

;
;  Thread Object Structure Offset Definitions
;

EtUniqueThread equ 0012CH
EtEthreadLength equ 00140H

ThType equ 00000H
ThSize equ 00002H
ThSignalState equ 00004H
ThMutantListHead equ 00010H
ThKernelTime equ 00018H
ThStackBase equ 0001CH
ThStackLimit equ 00020H
ThKernelStack equ 00024H
ThTlsData equ 00028H
ThState equ 0002CH
ThAlerted equ 0002DH
ThAlertable equ 0002FH
ThNpxState equ 00030H
ThSaturation equ 00031H
ThPriority equ 00032H
ThApcState equ 00034H
ThContextSwitches equ 0004CH
ThWaitStatus equ 00050H
ThWaitIrql equ 00054H
ThWaitMode equ 00055H
ThWaitNext equ 00056H
ThWaitReason equ 00057H
ThWaitBlockList equ 00058H
ThWaitListEntry equ 0005CH
ThWaitTime equ 00064H
ThKernelApcDisable equ 00068H
ThQuantum equ 0006CH
ThBasePriority equ 00070H
ThDecrementCount equ 00071H
ThPriorityDecrement equ 00072H
ThDisableBoost equ 00073H
ThNpxIrql equ 00074H
ThSuspendCount equ 00075H
ThPreempted equ 00076H
THHasTerminated equ 00077H
ThQueue equ 00078H
ThQueueListEntry equ 0007CH
ThTimer equ 00088H
ThTimerWaitBlock equ 000B0H
ThSuspendApc equ 000C8H
ThSuspendSemaphore equ 000F0H
ThThreadListEntry equ 00104H
ThreadObjectLength equ 00110H
ExtendedThreadObjectLength equ 00140H

NPX_STATE_NOT_LOADED equ 0000AH
NPX_STATE_LOADED equ 00000H

;
;  Timer object Structure Offset Definitions
;

TiType equ 00000H
TiSize equ 00002H
TiInserted equ 00003H
TiSignalState equ 00004H
TiDueTime equ 00010H
TiTimerListEntry equ 00018H
TiDpc equ 00020H
TiPeriod equ 00024H
TimerObjectLength equ 00028H

TIMER_TABLE_SIZE equ 00020H

;
;  Wait Block Structure Offset Definitions
;

WbWaitListEntry equ 00000H
WbThread equ 00008H
WbObject equ 0000CH
WbNextWaitBlock equ 00010H
WbWaitKey equ 00014H
WbWaitType equ 00016H

;
;  Reader/Writer Lock Structure Offset Definitions
;

ErwLockCount equ 00000H
ErwWritersWaitingCount equ 00004H
ErwReadersWaitingCount equ 00008H
ErwReadersEntryCount equ 0000CH
ErwWriterEvent equ 00010H
ErwReaderSemaphore equ 00020H

;
;  Thread Environment Block Structure Offset Definitions
;

TeStackBase equ 00004H
TeStackLimit equ 00008H
TeFiberData equ 00010H
EVENT_INCREMENT equ 00001H

;
;  Apc Record Structure Offset Definitions
;

ArNormalRoutine equ 00000H
ArNormalContext equ 00004H
ArSystemArgument1 equ 00008H
ArSystemArgument2 equ 0000CH
ApcRecordLength equ 00010H


;
;  Processor Control Registers Structure Offset Definitions
;

PCR equ dword ptr [_KiPCR]
PCRB equ byte ptr [_KiPCR]
PcExceptionList equ 00000H
PcStackBase equ 00004H
PcStackLimit equ 00008H
PcSelfPcr equ 0001CH
PcPrcb equ 00020H
PcTeb equ 00018H
PcIrql equ 00024H
PcPrcbData equ 00028H
ProcessorControlRegisterLength equ 00284H

;
;  Tss Structure Offset Definitions
;

TssSs0 equ 00008H
TssEsp0 equ 00004H
TssEip equ 00020H
TssEFlags equ 00024H
TssEsp equ 00038H
TssCR3 equ 0001CH
TssCs equ 0004CH
TssSs equ 00050H
TssDs equ 00054H
TssEs equ 00048H
TssFs equ 00058H
TssGs equ 0005CH
TssIoMapBase equ 00066H
TssIoMaps equ 00068H
TssLength equ 020ACH

;
;  Gdt Descriptor Offset Definitions
;

KGDT_R0_CODE equ 00008H
KGDT_R0_DATA equ 00010H
KGDT_R0_PCR equ 00020H
KGDT_TSS equ 00018H
KGDT_DF_TSS equ 00028H
KGDT_NMI_TSS equ 00030H

;
;  GdtEntry Offset Definitions
;

KgdtBaseLow equ 00002H
KgdtBaseMid equ 00004H
KgdtBaseHi equ 00007H
KgdtFlags1 equ 00005H
KgdtLimitHi equ 00006H
KgdtLimitLow equ 00000H
KgdtEntryLength equ 00008H


;
;  IdtEntry Offset Definitions
;

KidtOffset equ 00000H
KidtSelector equ 00002H
KidtAccess equ 00004H
KidtExtendedOffset equ 00006H
KidtEntryLength equ 00008H


;
;  Processor Block Structure Offset Definitions
;

PbCurrentThread equ 00000H
PbNextThread equ 00004H
PbIdleThread equ 00008H
PbDebugHaltThread equ 00254H
PbDebugMonitorData equ 00250H
PbDebugDoubleFault equ 00258H
PbNpxThread equ 0000CH
PbInterruptCount equ 00010H
PbDpcTime equ 00014H
PbInterruptTime equ 00018H
PbDebugDpcTime equ 0001CH
PbContextSwitches equ 00020H
PbDpcInterruptRequested equ 00024H
PbDpcListHead equ 00028H
PbDpcRoutineActive equ 00030H
PbDpcStack equ 00034H
PbQuantumEnd equ 00038H
PbNpxSaveArea equ 0003CH
ProcessorBlockLength equ 00260H

;
;  Time Fields (TIME_FIELDS) Structure Offset Definitions
;

TfSecond equ 0000AH
TfMinute equ 00008H
TfHour equ 00006H
TfWeekday equ 0000EH
TfDay equ 00004H
TfMonth equ 00002H
TfYear equ 00000H
TfMilliseconds equ 0000CH


;
;  constants for system irql and IDT vector conversion
;

MAXIMUM_IDTVECTOR equ 000FFH
MAXIMUM_PRIMARY_VECTOR equ 000FFH
PRIMARY_VECTOR_BASE equ 00030H

;
;  Flags in the CR0 register
;

CR0_PG equ 080000000H
CR0_ET equ 00010H
CR0_TS equ 00008H
CR0_EM equ 00004H
CR0_MP equ 00002H
CR0_PE equ 00001H
CR0_CD equ 040000000H
CR0_NW equ 020000000H
CR0_AM equ 040000H
CR0_WP equ 010000H
CR0_NE equ 00020H

;
;  Flags in the CR4 register
;

CR4_VME equ 00001H
CR4_PVI equ 00002H
CR4_TSD equ 00004H
CR4_DE equ 00008H
CR4_PSE equ 00010H
CR4_PAE equ 00020H
CR4_MCE equ 00040H
CR4_PGE equ 00080H
CR4_FXSR equ 00200H
CR4_XMMEXCPT equ 00400H

;
;  Miscellaneous Definitions
;

MAXIMUM_PROCESSORS equ 00020H
INITIAL_STALL_COUNT equ 00064H
IRQL_NOT_GREATER_OR_EQUAL equ 00009H
IRQL_NOT_LESS_OR_EQUAL equ 0000AH
MUTEX_ALREADY_OWNED equ 000BFH
THREAD_NOT_MUTEX_OWNER equ 00011H
BASE_PRIORITY_THRESHOLD equ 00008H
LOW_REALTIME_PRIORITY equ 00010H
KERNEL_STACK_SIZE equ 03000H
DOUBLE_FAULT_STACK_SIZE equ 01000H
EFLAG_SELECT equ 0C000H
BREAKPOINT_BREAK equ 00000H
CLOCK_QUANTUM_DECREMENT equ 00003H
THREAD_QUANTUM equ 0003CH
WAIT_QUANTUM_DECREMENT equ 0000AH
ROUND_TRIP_DECREMENT_COUNT equ 00010H

;
;  Trap Frame Offset Definitions and Length
;

TsExceptionList equ 00024H
TsEdi equ 00028H
TsEsi equ 0002CH
TsEbp equ 00034H
TsEbx equ 00030H
TsEdx equ 00018H
TsEcx equ 0001CH
TsEax equ 00020H
TsErrCode equ 00038H
TsEip equ 0003CH
TsSegCs equ 00040H
TsEflags equ 00044H
TsHardwareEsp equ 00048H
TsHardwareSegSs equ 0004CH
TsTempSegCs equ 00010H
TsTempEsp equ 00014H
TsDbgEbp equ 00000H
TsDbgEip equ 00004H
TsDbgArgMark equ 00008H
TsDbgArgPointer equ 0000CH
KTRAP_FRAME_LENGTH equ 00050H
KTRAP_FRAME_ALIGN equ 00004H
FRAME_EDITED equ 0FFF8H
EFLAGS_ALIGN_CHECK equ 040000H
EFLAGS_V86_MASK equ 020000H
EFLAGS_INTERRUPT_MASK equ 00200H
EFLAGS_VIF equ 080000H
EFLAGS_VIP equ 0100000H
EFLAGS_USER_SANITIZE equ 03E0DD7H

;
;  Context Frame Offset and Flag Definitions
;

CONTEXT_FULL equ 010007H
CONTEXT_DEBUG_REGISTERS equ 010010H
CONTEXT_CONTROL equ 010001H
CONTEXT_FLOATING_POINT equ 010008H
CONTEXT_INTEGER equ 010002H
CONTEXT_SEGMENTS equ 010004H

CsContextFlags equ 00000H
CsFloatSave equ 00004H
CsEdi equ 00208H
CsEsi equ 0020CH
CsEbp equ 00220H
CsEbx equ 00210H
CsEdx equ 00214H
CsEcx equ 00218H
CsEax equ 0021CH
CsEip equ 00224H
CsSegCs equ 00228H
CsEflags equ 0022CH
CsEsp equ 00230H
CsSegSs equ 00234H
ContextFrameLength equ 00240H
DR6_LEGAL equ 0E00FH
DR7_LEGAL equ 0FFFF0155H
DR7_ACTIVE equ 00055H
ErrHandler equ 00004H
ErrNext equ 00000H

;
;  Floating save area field offset definitions
;

FxControlWord equ 00000H
FxStatusWord equ 00002H
FxErrorOffset equ 00008H
FxErrorSelector equ 0000CH
FxDataOffset equ 00010H
FxDataSelector equ 00014H
FxMXCsr equ 00018H
FpCr0NpxState equ 00200H

NPX_FRAME_LENGTH equ 00210H

;
;  Processor State Frame Offset Definitions
;

PsContextFrame equ 00000H
PsSpecialRegisters equ 00238H
SrCr0 equ 00000H
SrCr2 equ 00004H
SrCr3 equ 00008H
SrCr4 equ 0000CH
SrKernelDr0 equ 00010H
SrKernelDr1 equ 00014H
SrKernelDr2 equ 00018H
SrKernelDr3 equ 0001CH
SrKernelDr6 equ 00020H
SrKernelDr7 equ 00024H
SrGdtr equ 0002AH
SrIdtr equ 00032H
SrTr equ 00038H
SrLdtr equ 0003AH
PsNT5ContextFrame equ 00000H
PsNT5SpecialRegisters equ 002CCH
SrNT5Cr0 equ 00000H
SrNT5Cr2 equ 00004H
SrNT5Cr3 equ 00008H
SrNT5Cr4 equ 0000CH
SrNT5KernelDr0 equ 00010H
SrNT5KernelDr1 equ 00014H
SrNT5KernelDr2 equ 00018H
SrNT5KernelDr3 equ 0001CH
SrNT5KernelDr6 equ 00020H
SrNT5KernelDr7 equ 00024H
SrNT5Gdtr equ 0002AH
SrNT5Idtr equ 00032H
SrNT5Tr equ 00038H
SrNT5Ldtr equ 0003AH

PAGE_SIZE equ 01000H

CLOCK_ROLLOVER_COUNT equ 00465H
CLOCK_TIME_INCREMENT equ 02710H

PAGE_DIRECTORY_PHYSICAL_ADDRESS equ 0F000H

ROM_SHADOW_PHYSICAL_ADDRESS equ 07F00000H
ROM_SHADOW_SIZE equ 0100000H

XDISK_UNPARTITIONED_SECTORS equ 00400H

MzXdataSectionHeader equ 080010028H
XdshSizeOfUninitializedData equ 00000H
XdshSizeOfInitializedData equ 00004H
XdshPointerToRawData equ 00008H
XdshVirtualAddress equ 0000CH

MudbProcessor equ 0000CH
MudbUpdateData equ 00030H
MudbStructureLength equ 00800H
XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 equ 08000H
XPCICFG_LPCBRIDGE_IO_REGISTER_LENGTH_0 equ 00100H
XPCICFG_SMBUS_IO_REGISTER_BASE_1 equ 0C000H
XPCICFG_SMBUS_IO_REGISTER_LENGTH_1 equ 00010H
XPCICFG_GPU_MEMORY_REGISTER_BASE_0 equ 0FD000000H
XPCICFG_GPU_MEMORY_REGISTER_LENGTH_0 equ 01000000H
