/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    kdp.h

Abstract:

    Private include file for the Kernel Debugger subcomponent
    of the NTOS project

Author:

    Mike O'Leary (mikeol) 29-June-1989

Revision History:

--*/

#include "ntos.h"
#include "ki.h"
#include "string.h"
#include "stdlib.h"

//
// Put all of the kernel debugger code in one section and the read-write data
// into another section.  Do this before including wdbgexts.h and ntdbg.h as
// these declare inline functions that shouldn't be placed in the normal code
// section.
//
// The kernel debugger data section needs to be merged into the main data
// section so that it's properly reinitialized across a quick reboot.
//

#pragma code_seg("KDCODE")
#pragma const_seg("KDDATA_RD")
#pragma data_seg("KDDATA_RW")
#pragma bss_seg("KDDATA_URW")
#pragma comment(linker, "/merge:KDDATA_RD=KDCODE")
#pragma comment(linker, "/merge:KDDATA_RW=.data")
#pragma comment(linker, "/merge:KDDATA_URW=.bss")

#pragma intrinsic(_inp,_inpw,_inpd,_outp,_outpw,_outpd)

#define NOEXTAPI
#include "wdbgexts.h"
#include "ntdbg.h"

#define KDP_BREAKPOINT_TYPE  UCHAR
#define KDP_BREAKPOINT_ALIGN 0
#define KDP_BREAKPOINT_VALUE 0xcc

//
// Define constants.
//

//
// Addresses above GLOBAL_BREAKPOINT_LIMIT are either in system space
// or part of dynlink, so we treat them as global.
//

#define GLOBAL_BREAKPOINT_LIMIT 1610612736L // 1.5gigabytes

//
// Define breakpoint table entry structure.
//

#define KD_BREAKPOINT_IN_USE        0x00000001
#define KD_BREAKPOINT_NEEDS_WRITE   0x00000002
#define KD_BREAKPOINT_SUSPENDED     0x00000004
#define KD_BREAKPOINT_NEEDS_REPLACE 0x00000008
// IA64 specific defines
#define KD_BREAKPOINT_STATE_MASK    0x0000000f

//
// status Constants for Packet waiting
//

#define KDP_PACKET_RECEIVED 0
#define KDP_PACKET_TIMEOUT 1
#define KDP_PACKET_RESEND 2


typedef struct _BREAKPOINT_ENTRY {
    UCHAR Flags;
    KDP_BREAKPOINT_TYPE Content;
    PVOID Address;
} BREAKPOINT_ENTRY, *PBREAKPOINT_ENTRY;


//
// Misc defines
//

#define MAXIMUM_RETRIES 20

#define DBGKD_MAX_SPECIAL_CALLS 10

typedef struct _TRACE_DATA_SYM {
    ULONG SymMin;
    ULONG SymMax;
} TRACE_DATA_SYM, *PTRACE_DATA_SYM;

//
//
//

typedef struct _X86_NT5_DBGKD_WAIT_STATE_CHANGE64 {
    ULONG NewState;
    USHORT ProcessorLevel;
    USHORT Processor;
    ULONG NumberProcessors;
    ULONG64 Thread;
    ULONG64 ProgramCounter;
    union {
        DBGKM_EXCEPTION64 Exception;
        DBGKD_LOAD_SYMBOLS64 LoadSymbols;
    } u;
    DBGKD_CONTROL_REPORT ControlReport;
    X86_NT5_CONTEXT Context;
} X86_NT5_DBGKD_WAIT_STATE_CHANGE64, *PX86_NT5_DBGKD_WAIT_STATE_CHANGE64;

//
// Kernel debugger port functions.
//

BOOLEAN
KdPortInitialize (
    PDEBUG_PARAMETERS DebugParameters,
    BOOLEAN Initialize
    );

ULONG
KdPortGetByte (
    OUT PUCHAR Input
    );

ULONG
KdPortPollByte (
    OUT PUCHAR Input
    );

VOID
KdPortPutByte (
    IN UCHAR Output
    );

//
// Define function prototypes.
//

BOOLEAN
KdpPrintString (
    IN PSTRING Output
    );

BOOLEAN
KdpPromptString (
    IN PSTRING Output,
    IN OUT PSTRING Input
    );

ULONG
KdpAddBreakpoint (
    IN PVOID Address
    );

BOOLEAN
KdpDeleteBreakpoint (
    IN ULONG Handle
    );

BOOLEAN
KdpDeleteBreakpointRange (
    IN PVOID Lower,
    IN PVOID Upper
    );

#if i386

NTSTATUS
KdGetTraceInformation (
    OUT PVOID TraceInformation,
    IN ULONG TraceInformationLength,
    OUT PULONG RequiredLength
    );

VOID
KdSetInternalBreakpoint (
    IN PDBGKD_MANIPULATE_STATE64 m
    );

#endif

NTSTATUS
KdQuerySpecialCalls (
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN ULONG Length,
    OUT PULONG RequiredLength
    );

VOID
KdSetSpecialCall (
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PX86_NT5_CONTEXT ContextRecord
    );

VOID
KdClearSpecialCalls (
    VOID
    );

ULONG
KdpMoveMemory (
    IN PCHAR Destination,
    IN PCHAR Source,
    IN ULONG Length
    );

VOID
KdpQuickMoveMemory (
    IN PCHAR Destination,
    IN PCHAR Source,
    IN ULONG Length
    );

ULONG
KdpReceivePacket (
    IN ULONG ExpectedPacketType,
    OUT PSTRING MessageHeader,
    OUT PSTRING MessageData,
    OUT PULONG DataLength
    );

VOID
KdpSetLoadState(
    IN PX86_NT5_DBGKD_WAIT_STATE_CHANGE64 WaitStateChange,
    IN PX86_NT5_CONTEXT ContextRecord
    );

VOID
KdpSetStateChange(
    IN PX86_NT5_DBGKD_WAIT_STATE_CHANGE64 WaitStateChange,
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PX86_NT5_CONTEXT ContextRecord,
    IN BOOLEAN SecondChance
    );

VOID
KdpGetStateChange(
    IN PDBGKD_MANIPULATE_STATE64 ManipulateState,
    IN PX86_NT5_CONTEXT ContextRecord
    );

VOID
KdpSendPacket (
    IN ULONG PacketType,
    IN PSTRING MessageHeader,
    IN PSTRING MessageData OPTIONAL
    );

BOOLEAN
KdpStub (
    IN PKTRAP_FRAME TrapFrame,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PCONTEXT ContextRecord,
    IN BOOLEAN SecondChance
    );

BOOLEAN
KdpTrap (
    IN PKTRAP_FRAME TrapFrame,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PEXCEPTION_RECORD ExceptionRecord64,
    IN PCONTEXT ContextRecord,
    IN BOOLEAN SecondChance
    );

BOOLEAN
KdpReportExceptionStateChange (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN OUT PX86_NT5_CONTEXT ContextRecord,
    IN BOOLEAN SecondChance
    );

BOOLEAN
KdpReportLoadSymbolsStateChange (
    IN PSTRING PathName,
    IN PKD_SYMBOLS_INFO SymbolInfo,
    IN BOOLEAN UnloadSymbols,
    IN OUT PX86_NT5_CONTEXT ContextRecord
    );

KCONTINUE_STATUS
KdpSendWaitContinue(
    IN ULONG PacketType,
    IN PSTRING MessageHeader,
    IN PSTRING MessageData OPTIONAL,
    IN OUT PX86_NT5_CONTEXT ContextRecord
    );

VOID
KdpReadVirtualMemory(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpWriteVirtualMemory(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpReadPhysicalMemory(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpWritePhysicalMemory(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpCheckLowMemory(
    IN PDBGKD_MANIPULATE_STATE64 m
    );

VOID
KdpGetContext(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData,
    IN PX86_NT5_CONTEXT Context
    );

VOID
KdpSetContext(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData,
    IN PX86_NT5_CONTEXT Context
    );

VOID
KdpWriteBreakpoint(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpRestoreBreakpoint(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpReadControlSpace(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpWriteControlSpace(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpReadIoSpace(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpReadMachineSpecificRegister(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpWriteIoSpace(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpWriteMachineSpecificRegister(
    IN PDBGKD_MANIPULATE_STATE64 m,
    IN PSTRING AdditionalData
    );

VOID
KdpSuspendBreakpoint (
    ULONG Handle
    );

VOID
KdpSuspendAllBreakpoints (
    VOID
    );

VOID
KdpRestoreAllBreakpoints (
    VOID
    );

VOID
KdpSaveProcessorControlState (
    IN PX86_NT5_KPROCESSOR_STATE ProcessorState
    );

VOID
KdpRestoreProcessorControlState (
    IN PX86_NT5_KPROCESSOR_STATE ProcessorState
    );

LARGE_INTEGER
KdpQueryPerformanceCounter (
    IN PKTRAP_FRAME TrapFrame
    );

//
// Define dummy prototype so the address of the standard breakpoint instruction
// can be captured.
//
// N.B. This function is NEVER called.
//

VOID
RtlpBreakWithStatusInstruction (
    VOID
    );

//
// Define external references.
//

#define KDP_MESSAGE_BUFFER_SIZE 4096

extern BREAKPOINT_ENTRY KdpBreakpointTable[BREAKPOINT_TABLE_SIZE];
extern BOOLEAN KdpControlCPending;
extern PKDEBUG_ROUTINE KiDebugRoutine;
extern KDP_BREAKPOINT_TYPE KdpBreakpointInstruction;
extern UCHAR KdpMessageBuffer[KDP_MESSAGE_BUFFER_SIZE];
extern UCHAR KdpPathBuffer[KDP_MESSAGE_BUFFER_SIZE];
extern ULONG KdpOweBreakpoint;
extern ULONG KdpNextPacketIdToSend;
extern ULONG KdpPacketIdExpected;

extern LARGE_INTEGER KdPerformanceCounterRate;
extern LARGE_INTEGER KdTimerStart;
extern LARGE_INTEGER KdTimerStop;
extern LARGE_INTEGER KdTimerDifference;

extern BOOLEAN BreakpointsSuspended;
extern LIST_ENTRY KdpDebuggerDataListHead;

typedef struct {
    ULONG64 Addr;                 // pc address of breakpoint
    ULONG Flags;                // Flags bits
    ULONG Calls;                // # of times traced routine called
    ULONG CallsLastCheck;       // # of calls at last periodic (1s) check
    ULONG MaxCallsPerPeriod;
    ULONG MinInstructions;      // largest number of instructions for 1 call
    ULONG MaxInstructions;      // smallest # of instructions for 1 call
    ULONG TotalInstructions;    // total instructions for all calls
    ULONG Handle;               // handle in (regular) bpt table
    PVOID Thread;               // Thread that's skipping this BP
    ULONG64 ReturnAddress;        // return address (if not COUNTONLY)
} DBGKD_INTERNAL_BREAKPOINT, *PDBGKD_INTERNAL_BREAKPOINT;


#define DBGKD_MAX_INTERNAL_BREAKPOINTS 20
extern DBGKD_INTERNAL_BREAKPOINT KdpInternalBPs[DBGKD_MAX_INTERNAL_BREAKPOINTS];

extern X86_NT5_KPROCESSOR_STATE KdProcessorState;

extern ULONG_PTR   KdpCurrentSymbolStart;
extern ULONG_PTR   KdpCurrentSymbolEnd;
extern LONG    KdpNextCallLevelChange;
extern ULONG_PTR   KdSpecialCalls[];
extern ULONG   KdNumberOfSpecialCalls;
extern ULONG_PTR   InitialSP;
extern ULONG   KdpNumInternalBreakpoints;
extern KTIMER  InternalBreakpointTimer;
extern KDPC    InternalBreakpointCheckDpc;
extern LARGE_INTEGER   KdpTimeEntered;

extern DBGKD_TRACE_DATA TraceDataBuffer[];
extern ULONG            TraceDataBufferPosition;
extern TRACE_DATA_SYM   TraceDataSyms[];
extern BOOLEAN          TraceDataBufferFilled;
extern UCHAR NextTraceDataSym;
extern UCHAR NumTraceDataSyms;
extern ULONG IntBPsSkipping;
extern BOOLEAN WatchStepOver;
extern PVOID WSOThread;
extern ULONG_PTR WSOEsp;
extern ULONG WatchStepOverHandle;
extern ULONG_PTR WatchStepOverBreakAddr;
extern BOOLEAN WatchStepOverSuspended;
extern ULONG InstructionsTraced;
extern BOOLEAN SymbolRecorded;
extern LONG CallLevelChange;
extern LONG_PTR oldpc;
extern BOOLEAN InstrCountInternal;
extern BOOLEAN BreakpointsSuspended;
extern BOOLEAN KdpControlCPending;
extern BOOLEAN KdpControlCPressed;
extern ULONG KdpRetryCount;
extern ULONG KdpNumberRetries;

extern KDP_BREAKPOINT_TYPE KdpBreakpointInstruction;
extern ULONG KdpOweBreakpoint;
extern ULONG KdpNextPacketIdToSend;
extern ULONG KdpPacketIdExpected;
extern UCHAR  KdPrintCircularBuffer[KDPRINTBUFFERSIZE];
extern PUCHAR KdPrintWritePointer;
extern ULONG  KdPrintRolloverCount;
extern DEBUG_PARAMETERS KdDebugParameters;
extern LIST_ENTRY KdpDebuggerDataListHead;
extern KDDEBUGGER_DATA64 KdDebuggerDataBlock;
extern BOOLEAN KdpDebuggerStructuresInitialized;
extern ULONG KdEnteredDebugger;
extern ULONG KdGlobalFlag;

extern LDR_DATA_TABLE_ENTRY KdpNtosDataTableEntry;

//
// !search support (page hit database)
//

//
// Hit database where search results are stored (kddata.c). 
// The debugger extensions know how to extract the information 
// from here.
//
// Note that the size of the hit database is large enough to
// accomodate any searches because the !search extension works
// in batches of pages < PAGE_SIZE and for every page we register only 
// one hit.
//

#define SEARCH_PAGE_HIT_DATABASE_SIZE PAGE_SIZE

extern PFN_NUMBER KdpSearchPageHits[SEARCH_PAGE_HIT_DATABASE_SIZE];
extern ULONG KdpSearchPageHitOffsets[SEARCH_PAGE_HIT_DATABASE_SIZE];

extern ULONG KdpSearchPageHitIndex;

//
// Set to true while a physical memory search is in progress.
// Reset at the end of the search. This is done in the debugger
// extension and it is a flag used by KdpCheckLowMemory to get
// onto a different code path.
//

extern LOGICAL KdpSearchInProgress;

//
// These variables store the current state of the search operation.
// They can be used to restore an interrupted search.
//

extern PFN_NUMBER KdpSearchStartPageFrame;
extern PFN_NUMBER KdpSearchEndPageFrame;

extern ULONG_PTR KdpSearchAddressRangeStart;
extern ULONG_PTR KdpSearchAddressRangeEnd;

//
// Checkpoint variable used to test if we have the right
// debugging symbols.
//

#define KDP_SEARCH_SYMBOL_CHECK 0xABCDDCBA

extern ULONG KdpSearchCheckPoint;

//
// Page search flags
//

#define KDP_SEARCH_ALL_OFFSETS_IN_PAGE 0x0001


//
// Define macros to lock and unlock the debug port.  The caller must have the
// proper IRQL before using these macros.
//

#define KdpPortLock()                   NOTHING
#define KdpPortUnlock()                 NOTHING


//
// Private procedure prototypes
//

VOID
KdpInitCom(
    VOID
    );

BOOLEAN
KdpPollBreakInWithPortLock(
    VOID
    );

USHORT
KdpReceivePacketLeader (
    IN ULONG PacketType,
    OUT PULONG PacketLeader
    );

#if DBG

#define DPRINT(s) KdpDprintf s

VOID
KdpDprintf(
    IN PCHAR f,
    ...
    );

#else

#define DPRINT(s)

#endif
