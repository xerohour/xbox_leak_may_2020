/*++

Copyright (c) 1990-2002  Microsoft Corporation

Module Name:

    kddata.c

Abstract:

    This module contains global data for the portable kernel debgger.

--*/

#include "kdp.h"
#include "mi.h"
#include "pool.h"

//
// Miscellaneous data from all over the kernel
//

extern ULONG KiBugCheckData[];

extern POBJECT_DIRECTORY ObpRootDirectoryObject;
extern POBJECT_HANDLE_TABLE ObpObjectHandleTable;

extern SIZE_T PoolTrackTableSize;
extern PPOOL_TRACKER_TABLE PoolTrackTable;
extern POOL_DESCRIPTOR NonPagedPoolDescriptor;

extern PMMPFN MmPfnDatabase;

extern LIST_ENTRY FscLruList;

//
// Highest user address.
//

PVOID KdHighestUserAddress = MM_HIGHEST_USER_ADDRESS;

//
// Start of system address range.
//

PVOID KdSystemRangeStart = MM_SYSTEM_RANGE_START;

//
// User probe address.
//

ULONG_PTR KdUserProbeAddress = MM_USER_PROBE_ADDRESS;

//
// Lowest physical page number in the system.
//

PFN_NUMBER KdLowestPhysicalPage = MM_LOWEST_PHYSICAL_PAGE;

//
// Pointer to the default system process.
//

PKPROCESS KdActiveProcessHead = &KiSystemProcess;

//
// Storage for the processor state.
//

X86_NT5_KPROCESSOR_STATE KdProcessorState;

//
// This block of data needs to always be present because crashdumps
// need the information.  Otherwise, things like PAGE_SIZE are not available
// in crashdumps, and extensions like !pool fail.
//

KDDEBUGGER_DATA64 KdDebuggerDataBlock = {
    {0},                                    //  DBGKD_DEBUG_DATA_HEADER Header;
    (ULONG64)0,
    (ULONG64)RtlpBreakWithStatusInstruction,
    (ULONG64)0,
    (USHORT)0,                                      //  USHORT  ThCallbackStack;
    (USHORT)0,                                      //  USHORT  NextCallback;
    (USHORT)0,                                      //  USHORT  FramePointer;
    (USHORT)0,                                      //  USHORT  PaeEnabled;

    (ULONG64)NULL,
    (ULONG64)0,

    (ULONG64)&KdLoadedModuleList,
    (ULONG64)&KdActiveProcessHead,
    (ULONG64)NULL,

    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)NULL,

    (ULONG64)&KeTimeIncrement,
    (ULONG64)NULL,
    (ULONG64)KiBugCheckData,

    (ULONG64)NULL,

    (ULONG64)&ObpRootDirectoryObject,
    (ULONG64)NULL,

    (ULONG64)&KeSystemTime,
    (ULONG64)&KeInterruptTime,
    (ULONG64)&KiTimerTableListHead,

    (ULONG64)&MmPfnDatabase,
    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)NULL,

    (ULONG64)&KdLowestPhysicalPage,
    (ULONG64)&MmHighestPhysicalPage,
    (ULONG64)&MmNumberOfPhysicalPages,

    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)NULL,

    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)PAGE_SIZE,
    (ULONG64)NULL,

    (ULONG64)MmAllocatedPagesByUsage,
    (ULONG64)&ObpObjectHandleTable,
    (ULONG64)&MmVadRoot,
    (ULONG64)&FscLruList,
    (ULONG64)&FscNumberOfCachePages,
    (ULONG64)&FscElementArray,
    (ULONG64)NULL,

    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)NULL,
    (ULONG64)&MmAvailablePages,

#ifdef NO_POOL_TAG_TRACKING
    (ULONG64)NULL,
    (ULONG64)NULL,
#else
    (ULONG64)&PoolTrackTableSize,
    (ULONG64)&PoolTrackTable,
#endif
    (ULONG64)&NonPagedPoolDescriptor,

    (ULONG64)&KdHighestUserAddress,
    (ULONG64)&KdSystemRangeStart,
    (ULONG64)&KdUserProbeAddress,

    (ULONG64)KdPrintCircularBuffer,
    (ULONG64)KdPrintCircularBuffer+sizeof(KdPrintCircularBuffer),

    (ULONG64)&KdPrintWritePointer,
    (ULONG64)&KdPrintRolloverCount,

    (ULONG64)NULL,
};

UCHAR  KdPrintCircularBuffer[KDPRINTBUFFERSIZE];
PUCHAR KdPrintWritePointer = KdPrintCircularBuffer;
ULONG  KdPrintRolloverCount;

BREAKPOINT_ENTRY KdpBreakpointTable[BREAKPOINT_TABLE_SIZE];
UCHAR KdpMessageBuffer[KDP_MESSAGE_BUFFER_SIZE];
UCHAR KdpPathBuffer[KDP_MESSAGE_BUFFER_SIZE];
DBGKD_INTERNAL_BREAKPOINT KdpInternalBPs[DBGKD_MAX_INTERNAL_BREAKPOINTS];

LARGE_INTEGER  KdPerformanceCounterRate;
LARGE_INTEGER  KdTimerStart;
LARGE_INTEGER  KdTimerStop;
LARGE_INTEGER  KdTimerDifference;

ULONG_PTR KdpCurrentSymbolStart;
ULONG_PTR KdpCurrentSymbolEnd;
LONG      KdpNextCallLevelChange;   // used only over returns to the debugger.

ULONG_PTR KdSpecialCalls[DBGKD_MAX_SPECIAL_CALLS];
ULONG     KdNumberOfSpecialCalls;
ULONG_PTR InitialSP;
ULONG     KdpNumInternalBreakpoints;
KTIMER    InternalBreakpointTimer;
KDPC      InternalBreakpointCheckDpc;


DBGKD_TRACE_DATA TraceDataBuffer[TRACE_DATA_BUFFER_MAX_SIZE];
ULONG            TraceDataBufferPosition = 1; // Element # to write next
                                   // Recall elt 0 is a length

TRACE_DATA_SYM   TraceDataSyms[256];
BOOLEAN TraceDataBufferFilled;
UCHAR NextTraceDataSym;     // what's the next one to be replaced
UCHAR NumTraceDataSyms;     // how many are valid?

ULONG IntBPsSkipping;       // number of exceptions that are being skipped now

BOOLEAN   WatchStepOver;
PVOID     WSOThread;                // thread doing stepover
ULONG_PTR WSOEsp;                   // stack pointer of thread doing stepover (yes, we need it)
ULONG     WatchStepOverHandle;
ULONG_PTR WatchStepOverBreakAddr;   // where the WatchStepOver break is set
BOOLEAN   WatchStepOverSuspended;
ULONG     InstructionsTraced;
BOOLEAN   SymbolRecorded;
LONG      CallLevelChange;
LONG_PTR  oldpc;
BOOLEAN   InstrCountInternal;       // Processing a non-COUNTONLY?

BOOLEAN   BreakpointsSuspended;

//
// KdpRetryCount controls the number of retries before we give up and
//   assume kernel debugger is not present.
// KdpNumberRetries is the number of retries left.  Initially, it is set
//   to 5 such that booting NT without debugger won't be delayed to long.
//

ULONG KdpRetryCount = 5;
ULONG KdpNumberRetries = 5;
BOOLEAN KdpControlCPending;
BOOLEAN KdpControlCPressed;

KDP_BREAKPOINT_TYPE KdpBreakpointInstruction;
ULONG KdpNextPacketIdToSend;
ULONG KdpPacketIdExpected;

//
// KdDebugParameters contains the debug port address and baud rate
//     used to initialize kernel debugger port.
//
// (They both get initialized to zero to indicate using default settings.)
// If SYSTEM hive contains the parameters, i.e. port and baud rate, system
// init code will fill in these variable with the values stored in the hive.
//

DEBUG_PARAMETERS KdDebugParameters;

LIST_ENTRY      KdpDebuggerDataListHead;

//
// !search support variables (page hit database)
//

PFN_NUMBER KdpSearchPageHits [SEARCH_PAGE_HIT_DATABASE_SIZE];
ULONG KdpSearchPageHitOffsets [SEARCH_PAGE_HIT_DATABASE_SIZE];
ULONG KdpSearchPageHitIndex;

LOGICAL KdpSearchInProgress;

PFN_NUMBER KdpSearchStartPageFrame;
PFN_NUMBER KdpSearchEndPageFrame;

ULONG_PTR KdpSearchAddressRangeStart;
ULONG_PTR KdpSearchAddressRangeEnd;

ULONG KdpSearchCheckPoint = KDP_SEARCH_SYMBOL_CHECK;

BOOLEAN KdDebuggerNotPresent;
BOOLEAN KdDebuggerEnabled;
BOOLEAN KdpDebuggerStructuresInitialized;
ULONG KdpOweBreakpoint;
ULONG KdEnteredDebugger;
ULONG KdGlobalFlag;

//
// Static loader data table entry for XBOXKRNL.EXE.
//
LDR_DATA_TABLE_ENTRY KdpNtosDataTableEntry;

//
// List of modules that have been loaded.
//
INITIALIZED_LIST_ENTRY(KdLoadedModuleList);
