/*++ BUILD Version: 0028    // Increment this if a change has global effects

Copyright (c) 1989  Microsoft Corporation

Module Name:

    ke.h

Abstract:

    This module contains the public (external) header file for the kernel.

Author:

    David N. Cutler (davec) 27-Feb-1989

Revision History:

--*/

#ifndef _KE_
#define _KE_

//
// Define the default quantum decrement values.
//

#define CLOCK_QUANTUM_DECREMENT 3
#define WAIT_QUANTUM_DECREMENT 10

//
// Define the default thread quantum value.
//

#define THREAD_QUANTUM 60

//
// Define the round trip decrement count.
//

#define ROUND_TRIP_DECREMENT_COUNT 16

//
// Public (external) constant definitions.
//

#define BASE_PRIORITY_THRESHOLD NORMAL_BASE_PRIORITY // fast path base threshold

//
// Define timer table size.
//

#define TIMER_TABLE_SIZE 32

//
// Enumerated kernel types
//
// Kernel object types.
//
//  N.B. There are really two types of event objects; NotificationEvent and
//       SynchronizationEvent. The type value for a notification event is 0,
//       and that for a synchronization event 1.
//
//  N.B. There are two types of new timer objects; NotificationTimer and
//       SynchronizationTimer. The type value for a notification timer is
//       8, and that for a synchronization timer is 9. These values are
//       very carefully chosen so that the dispatcher object type AND'ed
//       with 0x7 yields 0 or 1 for event objects and the timer objects.
//

#define DISPATCHER_OBJECT_TYPE_MASK 0x7

typedef enum _KOBJECTS {
    EventNotificationObject = 0,
    EventSynchronizationObject = 1,
    MutantObject = 2,
    ProcessObject = 3,
    QueueObject = 4,
    SemaphoreObject = 5,
    ThreadObject = 6,
    Spare1Object = 7,
    TimerNotificationObject = 8,
    TimerSynchronizationObject = 9,
    Spare2Object = 10,
    Spare3Object = 11,
    Spare4Object = 12,
    Spare5Object = 13,
    Spare6Object = 14,
    Spare7Object = 15,
    Spare8Object = 16,
    Spare9Object = 17,
    ApcObject,
    DpcObject,
    DeviceQueueObject,
    EventPairObject,
    InterruptObject,
    ProfileObject
    } KOBJECTS;

// begin_ntddk begin_wdm begin_ntminiport

//
// Interrupt modes.
//

typedef enum _KINTERRUPT_MODE {
    LevelSensitive,
    Latched
    } KINTERRUPT_MODE;

// end_ntddk end_wdm end_ntminiport

//
// Thread scheduling states.
//

typedef enum _KTHREAD_STATE {
    Initialized,
    Ready,
    Running,
    Standby,
    Terminated,
    Waiting,
    Transition
    } KTHREAD_STATE;

// begin_ntddk begin_wdm
//
// Wait reasons
//

typedef enum _KWAIT_REASON {
    Executive,
    FreePage,
    PageIn,
    PoolAllocation,
    DelayExecution,
    Suspended,
    UserRequest,
    WrExecutive,
    WrFreePage,
    WrPageIn,
    WrPoolAllocation,
    WrDelayExecution,
    WrSuspended,
    WrUserRequest,
    WrEventPair,
    WrQueue,
    WrLpcReceive,
    WrLpcReply,
    WrVirtualMemory,
    WrPageOut,
    WrRendezvous,
    WrFsCacheIn,
    WrFsCacheOut,
    Spare4,
    Spare5,
    Spare6,
    WrKernel,
    MaximumWaitReason
    } KWAIT_REASON;

// end_ntddk end_wdm

//
// Miscellaneous type definitions
//
// APC state
//

typedef struct _KAPC_STATE {
    LIST_ENTRY ApcListHead[MaximumMode];
    struct _KPROCESS *Process;
    BOOLEAN KernelApcInProgress;
    BOOLEAN KernelApcPending;
    BOOLEAN UserApcPending;
    BOOLEAN ApcQueueable;
} KAPC_STATE, *PKAPC_STATE, *RESTRICTED_POINTER PRKAPC_STATE;

// begin_ntddk begin_wdm
//
// Common dispatcher object header
//
// N.B. The size field contains the number of dwords in the structure.
//

typedef struct _DISPATCHER_HEADER {
    UCHAR Type;
    UCHAR Absolute;
    UCHAR Size;
    UCHAR Inserted;
    LONG SignalState;
    LIST_ENTRY WaitListHead;
} DISPATCHER_HEADER;

// end_ntddk end_wdm

//
// Wait block
//
// begin_ntddk begin_wdm

typedef struct _KWAIT_BLOCK {
    LIST_ENTRY WaitListEntry;
    struct _KTHREAD *RESTRICTED_POINTER Thread;
    PVOID Object;
    struct _KWAIT_BLOCK *RESTRICTED_POINTER NextWaitBlock;
    USHORT WaitKey;
    USHORT WaitType;
} KWAIT_BLOCK, *PKWAIT_BLOCK, *RESTRICTED_POINTER PRKWAIT_BLOCK;

// end_ntddk end_wdm

//
// Procedure type definitions
//
// Debug routine
//

typedef
BOOLEAN
(*PKDEBUG_ROUTINE) (
    IN PKTRAP_FRAME TrapFrame,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PCONTEXT ContextRecord,
    IN BOOLEAN SecondChance
    );

typedef enum {
    ContinueError = FALSE,
    ContinueSuccess = TRUE,
    ContinueProcessorReselected,
    ContinueNextProcessor
} KCONTINUE_STATUS;

// begin_ntddk begin_wdm
//
// Thread start function
//

typedef
VOID
(*PKSTART_ROUTINE) (
    IN PVOID StartContext
    );

// end_wdm

//
// Thread system function
//

typedef
VOID
(*PKSYSTEM_ROUTINE) (
    IN PKSTART_ROUTINE StartRoutine OPTIONAL,
    IN PVOID StartContext OPTIONAL
    );

// begin_wdm
//
// Kernel object structure definitions
//

//
// Device Queue object and entry
//

typedef struct _KDEVICE_QUEUE {
    CSHORT Type;
    UCHAR Size;
    BOOLEAN Busy;
    LIST_ENTRY DeviceListHead;
} KDEVICE_QUEUE, *PKDEVICE_QUEUE, *RESTRICTED_POINTER PRKDEVICE_QUEUE;

typedef struct _KDEVICE_QUEUE_ENTRY {
    LIST_ENTRY DeviceListEntry;
    ULONG SortKey;
    BOOLEAN Inserted;
} KDEVICE_QUEUE_ENTRY, *PKDEVICE_QUEUE_ENTRY, *RESTRICTED_POINTER PRKDEVICE_QUEUE_ENTRY;

//
// Event object
//

typedef struct _KEVENT {
    DISPATCHER_HEADER Header;
} KEVENT, *PKEVENT, *RESTRICTED_POINTER PRKEVENT;

// end_ntddk end_wdm   
// begin_ntddk begin_wdm 
//
// Define the interrupt service function type and the empty struct
// type.
//
// end_ntddk end_wdm 

struct _KINTERRUPT;

// begin_ntddk begin_wdm

typedef
BOOLEAN
(*PKSERVICE_ROUTINE) (
    IN struct _KINTERRUPT *Interrupt,
    IN PVOID ServiceContext
    );

// end_ntddk end_wdm

//
// Interrupt object
//

typedef struct _KINTERRUPT {
    PKSERVICE_ROUTINE ServiceRoutine;
    PVOID ServiceContext;
    ULONG BusInterruptLevel;
    ULONG Irql;                         // zero extended
    BOOLEAN Connected;
    BOOLEAN ShareVector;
    UCHAR Mode;
    ULONG ServiceCount;
    ULONG DispatchCode[DISPATCH_LENGTH];
} KINTERRUPT;

typedef struct _KINTERRUPT *PKINTERRUPT, *RESTRICTED_POINTER PRKINTERRUPT; // ntndis

// begin_ntddk begin_wdm
//
// Mutant object
//

typedef struct _KMUTANT {
    DISPATCHER_HEADER Header;
    LIST_ENTRY MutantListEntry;
    struct _KTHREAD *RESTRICTED_POINTER OwnerThread;
    BOOLEAN Abandoned;
} KMUTANT, *PKMUTANT, *RESTRICTED_POINTER PRKMUTANT;

// end_ntddk end_wdm
//
// Queue object
//

typedef struct _KQUEUE {
    DISPATCHER_HEADER Header;
    LIST_ENTRY EntryListHead;
    ULONG CurrentCount;
    ULONG MaximumCount;
    LIST_ENTRY ThreadListHead;
} KQUEUE, *PKQUEUE, *RESTRICTED_POINTER PRKQUEUE;

// begin_ntddk begin_wdm
//
//
// Semaphore object
//

typedef struct _KSEMAPHORE {
    DISPATCHER_HEADER Header;
    LONG Limit;
} KSEMAPHORE, *PKSEMAPHORE, *RESTRICTED_POINTER PRKSEMAPHORE;

//
//
// Timer object
//

typedef struct _KTIMER {
    DISPATCHER_HEADER Header;
    ULARGE_INTEGER DueTime;
    LIST_ENTRY TimerListEntry;
    struct _KDPC *Dpc;
    LONG Period;
} KTIMER, *PKTIMER, *RESTRICTED_POINTER PRKTIMER;

// end_ntddk end_wdm

//
// Thread object
//

typedef struct _KTHREAD {

    //
    // The dispatcher header and mutant listhead are fairly infrequently
    // referenced, but pad the thread to a 32-byte boundary (assumption
    // that pool allocation is in units of 32-bytes).
    //

    DISPATCHER_HEADER Header;
    LIST_ENTRY MutantListHead;

    //
    // The following entries are referenced during clock interrupts.
    //

    ULONG KernelTime;

    //
    // The following fields are referenced during trap, interrupts, or
    // context switches.
    //

    PVOID StackBase;
    PVOID StackLimit;
    PVOID KernelStack;
    PVOID TlsData;
    UCHAR State;
    BOOLEAN Alerted[MaximumMode];
    BOOLEAN Alertable;
    UCHAR NpxState;
    CHAR Saturation;
    SCHAR Priority;
    UCHAR Padding;
    KAPC_STATE ApcState;
    ULONG ContextSwitches;

    //
    // The following fields are referenced during wait operations.
    //

    LONG_PTR WaitStatus;
    KIRQL WaitIrql;
    KPROCESSOR_MODE WaitMode;
    BOOLEAN WaitNext;
    UCHAR WaitReason;
    PRKWAIT_BLOCK WaitBlockList;
    LIST_ENTRY WaitListEntry;
    ULONG WaitTime;
    ULONG KernelApcDisable;
    LONG Quantum;
    SCHAR BasePriority;
    UCHAR DecrementCount;
    SCHAR PriorityDecrement;
    BOOLEAN DisableBoost;
    UCHAR NpxIrql;
    CCHAR SuspendCount;
    BOOLEAN Preempted;
    BOOLEAN HasTerminated;

    //
    // The following fields are referenced during queue operations.
    //

    PRKQUEUE Queue;
    LIST_ENTRY QueueListEntry;

    //
    // The following fields are referenced when the thread is blocking for a
    // timed interval.
    //

    KTIMER Timer;
    KWAIT_BLOCK TimerWaitBlock;

    //
    // The following fields are referenced when the thread is initialized
    // and very infrequently thereafter.
    //

    KAPC SuspendApc;
    KSEMAPHORE SuspendSemaphore;
    LIST_ENTRY ThreadListEntry;

} KTHREAD, *PKTHREAD, *RESTRICTED_POINTER PRKTHREAD;

//
// Process object structure definition
//

typedef struct _KPROCESS {
    LIST_ENTRY ReadyListHead;
    LIST_ENTRY ThreadListHead;
    ULONG StackCount;
    LONG ThreadQuantum;
    SCHAR BasePriority;
    BOOLEAN DisableBoost;
    BOOLEAN DisableQuantum;
} KPROCESS, *PKPROCESS, *RESTRICTED_POINTER PRKPROCESS;

//
// Kernel control object functions
//
// APC object
//

NTKERNELAPI
VOID
KeInitializeApc (
    IN PRKAPC Apc,
    IN PRKTHREAD Thread,
    IN PKKERNEL_ROUTINE KernelRoutine,
    IN PKRUNDOWN_ROUTINE RundownRoutine OPTIONAL,
    IN PKNORMAL_ROUTINE NormalRoutine OPTIONAL,
    IN KPROCESSOR_MODE ProcessorMode OPTIONAL,
    IN PVOID NormalContext OPTIONAL
    );

PLIST_ENTRY
KeFlushQueueApc (
    IN PKTHREAD Thread,
    IN KPROCESSOR_MODE ProcessorMode
    );

NTKERNELAPI
BOOLEAN
KeInsertQueueApc (
    IN PRKAPC Apc,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2,
    IN KPRIORITY Increment
    );

BOOLEAN
KeRemoveQueueApc (
    IN PKAPC Apc
    );

// begin_ntddk begin_wdm  
//
// DPC object
//

NTKERNELAPI
VOID
KeInitializeDpc (
    IN PRKDPC Dpc,
    IN PKDEFERRED_ROUTINE DeferredRoutine,
    IN PVOID DeferredContext
    );

#define INITIALIZED_KDPC(_Dpc, _DeferredRoutine, _DeferredContext)  \
    KDPC _Dpc = {                                                   \
        DpcObject,                                                  \
        FALSE,                                                      \
        0,                                                          \
        NULL,                                                       \
        NULL,                                                       \
        _DeferredRoutine,                                           \
        _DeferredContext                                            \
    }

NTKERNELAPI
BOOLEAN
KeInsertQueueDpc (
    IN PRKDPC Dpc,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

NTKERNELAPI
BOOLEAN
KeRemoveQueueDpc (
    IN PRKDPC Dpc
    );

DECLSPEC_NORETURN
VOID
KeRetireDpcListLoop(
    VOID
    );

//
// Device queue object
//

NTKERNELAPI
VOID
KeInitializeDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue
    );

NTKERNELAPI
BOOLEAN
KeInsertDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
    );

NTKERNELAPI
BOOLEAN
KeInsertByKeyDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry,
    IN ULONG SortKey
    );

NTKERNELAPI
PKDEVICE_QUEUE_ENTRY
KeRemoveDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue
    );

NTKERNELAPI
PKDEVICE_QUEUE_ENTRY
KeRemoveByKeyDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN ULONG SortKey
    );

NTKERNELAPI
BOOLEAN
KeRemoveEntryDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
    );

// end_ntddk end_wdm

//
// Interrupt object
//

NTKERNELAPI                                         // nthal
VOID                                                // nthal
KeInitializeInterrupt (                             // nthal
    IN PKINTERRUPT Interrupt,                       // nthal
    IN PKSERVICE_ROUTINE ServiceRoutine,            // nthal
    IN PVOID ServiceContext,                        // nthal
    IN ULONG Vector,                                // nthal
    IN KIRQL Irql,                                  // nthal
    IN KINTERRUPT_MODE InterruptMode,               // nthal
    IN BOOLEAN ShareVector                          // nthal
    );                                              // nthal
                                                    // nthal
NTKERNELAPI                                         // nthal
BOOLEAN                                             // nthal
KeConnectInterrupt (                                // nthal
    IN PKINTERRUPT Interrupt                        // nthal
    );                                              // nthal
                                                    // nthal
NTKERNELAPI
BOOLEAN
KeDisconnectInterrupt (
    IN PKINTERRUPT Interrupt
    );

NTKERNELAPI                                         // ntddk wdm nthal
BOOLEAN                                             // ntddk wdm nthal
KeSynchronizeExecution (                            // ntddk wdm nthal
    IN PKINTERRUPT Interrupt,                       // ntddk wdm nthal
    IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,    // ntddk wdm nthal
    IN PVOID SynchronizeContext                     // ntddk wdm nthal
    );                                              // ntddk wdm nthal
                                                    // ntddk wdm nthal
// begin_ntddk begin_wdm
//
// Kernel dispatcher object functions
//
// Event Object
//

// end_wdm end_ntddk

#if defined(_NTDRIVER_) || defined(_NTDDK_) || defined(_NTIFS_) || defined(_NTHAL_)

// begin_wdm begin_ntddk

NTKERNELAPI
VOID
KeInitializeEvent (
    IN PRKEVENT Event,
    IN EVENT_TYPE Type,
    IN BOOLEAN State
    );

// end_wdm end_ntddk

#else

#define KeInitializeEvent(_Event, _Type, _State)            \
    (_Event)->Header.Type = (UCHAR)_Type;                   \
    (_Event)->Header.Size =  sizeof(KEVENT) / sizeof(LONG); \
    (_Event)->Header.SignalState = _State;                  \
    InitializeListHead(&(_Event)->Header.WaitListHead)

#endif

#define INITIALIZED_KEVENT(_Event, _Type, _State)           \
    KEVENT _Event = {                                       \
        _Type,                                              \
        FALSE,                                              \
        sizeof(KEVENT) / sizeof(LONG),                      \
        FALSE,                                              \
        _State,                                             \
        &_Event.Header.WaitListHead,                        \
        &_Event.Header.WaitListHead                         \
    }

// begin_ntddk

#define KeClearEvent(Event) ((Event)->Header.SignalState = 0)

NTKERNELAPI
LONG
KePulseEvent (
    IN PRKEVENT Event,
    IN KPRIORITY Increment,
    IN BOOLEAN Wait
    );

#define KeReadStateEvent(Event) (Event)->Header.SignalState

// begin_wdm

NTKERNELAPI
LONG
KeResetEvent (
    IN PRKEVENT Event
    );

NTKERNELAPI
LONG
KeSetEvent (
    IN PRKEVENT Event,
    IN KPRIORITY Increment,
    IN BOOLEAN Wait
    );

// end_ntddk end_wdm

VOID
KeSetEventBoostPriority (
    IN PRKEVENT Event,
    IN PRKTHREAD *Thread OPTIONAL
    );

//
// Mutant object
//

NTKERNELAPI
VOID
KeInitializeMutant (
    IN PRKMUTANT Mutant,
    IN BOOLEAN InitialOwner
    );

#define KeReadStateMutant(Mutant) (Mutant)->Header.SignalState

NTKERNELAPI
LONG
KeReleaseMutant (
    IN PRKMUTANT Mutant,
    IN KPRIORITY Increment,
    IN BOOLEAN Abandoned,
    IN BOOLEAN Wait
    );

//
//
// Queue Object.
//

NTKERNELAPI
VOID
KeInitializeQueue (
    IN PRKQUEUE Queue,
    IN ULONG Count OPTIONAL
    );

#define KeReadStateQueue(Queue) (Queue)->Header.SignalState

NTKERNELAPI
LONG
KeInsertQueue (
    IN PRKQUEUE Queue,
    IN PLIST_ENTRY Entry
    );

NTKERNELAPI
LONG
KeInsertHeadQueue (
    IN PRKQUEUE Queue,
    IN PLIST_ENTRY Entry
    );

NTKERNELAPI
PLIST_ENTRY
KeRemoveQueue (
    IN PRKQUEUE Queue,
    IN KPROCESSOR_MODE WaitMode,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

PLIST_ENTRY
KeRundownQueue (
    IN PRKQUEUE Queue
    );

// begin_ntddk begin_wdm
//
// Semaphore object
//

NTKERNELAPI
VOID
KeInitializeSemaphore (
    IN PRKSEMAPHORE Semaphore,
    IN LONG Count,
    IN LONG Limit
    );

#define KeReadStateSemaphore(Semaphore) (Semaphore)->Header.SignalState

NTKERNELAPI
LONG
KeReleaseSemaphore (
    IN PRKSEMAPHORE Semaphore,
    IN KPRIORITY Increment,
    IN LONG Adjustment,
    IN BOOLEAN Wait
    );

// end_ntddk end_wdm  

//
// Process object
//

VOID
KeInitializeProcess (
    IN PRKPROCESS Process,
    IN KPRIORITY Priority
    );

NTKERNELAPI
KPRIORITY
KeSetPriorityProcess (
    IN PKPROCESS Process,
    IN KPRIORITY BasePriority
    );

LOGICAL
KeSetDisableQuantumProcess (
    IN PKPROCESS Process,
    IN LOGICAL Disable
    );

//
// Thread object
//

VOID
KeInitializeThread (
    IN PKTHREAD Thread,
    IN PVOID KernelStack,
    IN SIZE_T KernelStackSize,
    IN SIZE_T TlsDataSize,
    IN PKSYSTEM_ROUTINE SystemRoutine,
    IN PKSTART_ROUTINE StartRoutine OPTIONAL,
    IN PVOID StartContext OPTIONAL,
    IN PKPROCESS Process
    );

NTKERNELAPI
BOOLEAN
KeAlertThread (
    IN PKTHREAD Thread,
    IN KPROCESSOR_MODE ProcessorMode
    );

NTKERNELAPI
ULONG
KeAlertResumeThread (
    IN PKTHREAD Thread
    );

NTKERNELAPI
VOID
KeBoostCurrentThread (
    VOID
    );

VOID
KeBoostPriorityThread (
    IN PKTHREAD Thread,
    IN KPRIORITY Increment
    );

NTKERNELAPI                                         // ntddk wdm nthal ntifs
NTSTATUS                                            // ntddk wdm nthal ntifs
KeDelayExecutionThread (                            // ntddk wdm nthal ntifs
    IN KPROCESSOR_MODE WaitMode,                    // ntddk wdm nthal ntifs
    IN BOOLEAN Alertable,                           // ntddk wdm nthal ntifs
    IN PLARGE_INTEGER Interval                      // ntddk wdm nthal ntifs
    );                                              // ntddk wdm nthal ntifs
                                                    // ntddk wdm nthal ntifs
BOOLEAN
KeDisableApcQueuingThread (
    IN PKTHREAD Thread
    );

BOOLEAN
KeEnableApcQueuingThread (
    IN PKTHREAD Thread
    );

NTKERNELAPI
LOGICAL
KeSetDisableBoostThread (
    IN PKTHREAD Thread,
    IN LOGICAL Disable
    );

ULONG
KeForceResumeThread (
    IN PKTHREAD Thread
    );

NTKERNELAPI
LONG
KeQueryBasePriorityThread (
    IN PKTHREAD Thread
    );

#define KeQueryPriorityThread(Thread) Thread->Priority

#define KeReadStateThread(Thread) ((BOOLEAN)(Thread)->Header.SignalState)

VOID
KeReadyThread (
    IN PKTHREAD Thread
    );

ULONG
KeResumeThread (
    IN PKTHREAD Thread
    );

VOID
KeRundownThread (
    VOID
    );

NTKERNELAPI                                         // ntddk nthal ntifs
LONG                                                // ntddk nthal ntifs
KeSetBasePriorityThread (                           // ntddk nthal ntifs
    IN PKTHREAD Thread,                             // ntddk nthal ntifs
    IN LONG Increment                               // ntddk nthal ntifs
    );                                              // ntddk nthal ntifs

NTKERNELAPI                                         // ntddk wdm nthal ntifs
KPRIORITY                                           // ntddk wdm nthal ntifs
KeSetPriorityThread (                               // ntddk wdm nthal ntifs
    IN PKTHREAD Thread,                             // ntddk wdm nthal ntifs
    IN KPRIORITY Priority                           // ntddk wdm nthal ntifs
    );                                              // ntddk wdm nthal ntifs
                                                    // ntddk wdm nthal ntifs
ULONG
KeSuspendThread (
    IN PKTHREAD Thread
    );

VOID
KeTerminateThread (
    VOID
    );

NTKERNELAPI
BOOLEAN
KeTestAlertThread (
    IN KPROCESSOR_MODE ProcessorMode
    );

//
// Define leave critical region macro used for inline and function code
// generation.
//
// Warning: assembly versions of this code are included directly in
// ntgdi assembly routines mutexs.s for MIPS and locka.asm for i386.
// Any changes made to KeEnterCriticalRegion/KeEnterCriticalRegion
// must be reflected in these routines.
//

#define KiLeaveCriticalRegion() {                                           \
    PKTHREAD Thread;                                                        \
    Thread = KeGetCurrentThread();                                          \
    if (((*((volatile ULONG *)&Thread->KernelApcDisable) += 1) == 0) &&     \
        (((volatile LIST_ENTRY *)&Thread->ApcState.ApcListHead[KernelMode])->Flink != \
         &Thread->ApcState.ApcListHead[KernelMode])) {                      \
        Thread->ApcState.KernelApcPending = TRUE;                           \
        KiRequestSoftwareInterrupt(APC_LEVEL);                              \
    }                                                                       \
}

// begin_ntddk

#if (defined(_NTDRIVER_) || defined(_NTDDK_) || defined(_NTIFS_) || defined(_NTHAL_)) && !defined(_NTSYSTEM_DRIVER_)

// begin_wdm

NTKERNELAPI
VOID
KeEnterCriticalRegion (
    VOID
    );

NTKERNELAPI
VOID
KeLeaveCriticalRegion (
    VOID
    );

// end_wdm

#else

//++
//
// VOID
// KeEnterCriticalRegion (
//    VOID
//    )
//
//
// Routine Description:
//
//    This function disables kernel APC's.
//
//    N.B. The following code does not require any interlocks. There are
//         two cases of interest: 1) On an MP system, the thread cannot
//         be running on two processors as once, and 2) if the thread is
//         is interrupted to deliver a kernel mode APC which also calls
//         this routine, the values read and stored will stack and unstack
//         properly.
//
// Arguments:
//
//    None.
//
// Return Value:
//
//    None.
//--

#define KeEnterCriticalRegion() KeGetCurrentThread()->KernelApcDisable -= 1;

//++
//
// VOID
// KeLeaveCriticalRegion (
//    VOID
//    )
//
//
// Routine Description:
//
//    This function enables kernel APC's.
//
//    N.B. The following code does not require any interlocks. There are
//         two cases of interest: 1) On an MP system, the thread cannot
//         be running on two processors as once, and 2) if the thread is
//         is interrupted to deliver a kernel mode APC which also calls
//         this routine, the values read and stored will stack and unstack
//         properly.
//
// Arguments:
//
//    None.
//
// Return Value:
//
//    None.
//--

#define KeLeaveCriticalRegion() KiLeaveCriticalRegion()

#endif

// begin_wdm

//
// Timer object
//

#define KeInitializeTimer(Timer) KeInitializeTimerEx(Timer, NotificationTimer)

NTKERNELAPI
VOID
KeInitializeTimerEx (
    IN PKTIMER Timer,
    IN TIMER_TYPE Type
    );

#define INITIALIZED_KTIMER(_Timer, _Type)                   \
    KTIMER _Timer = {                                       \
        TimerNotificationObject + _Type,                    \
        FALSE,                                              \
        sizeof(KTIMER) / sizeof(LONG),                      \
        FALSE,                                              \
        FALSE,                                              \
        &_Timer.Header.WaitListHead,                        \
        &_Timer.Header.WaitListHead                         \
    }

NTKERNELAPI
BOOLEAN
KeCancelTimer (
    IN PKTIMER
    );

#define KeClearTimer(Timer) ((Timer)->Header.SignalState = 0)

#define KeReadStateTimer(Timer) ((BOOLEAN)(Timer)->Header.SignalState)

NTKERNELAPI
BOOLEAN
KeSetTimer (
    IN PKTIMER Timer,
    IN LARGE_INTEGER DueTime,
    IN PKDPC Dpc OPTIONAL
    );

NTKERNELAPI
BOOLEAN
KeSetTimerEx (
    IN PKTIMER Timer,
    IN LARGE_INTEGER DueTime,
    IN LONG Period OPTIONAL,
    IN PKDPC Dpc OPTIONAL
    );

// end_ntddk end_wdm

PVOID
KeCheckForTimer(
    IN PVOID p,
    IN ULONG Size
    );

ULONGLONG
KeQueryTimerDueTime (
    IN PKTIMER Timer
    );

//
// Wait functions
//

// begin_ntddk begin_wdm

NTKERNELAPI
NTSTATUS
KeWaitForMultipleObjects (
    IN ULONG Count,
    IN PVOID Object[],
    IN WAIT_TYPE WaitType,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL,
    IN PKWAIT_BLOCK WaitBlockArray
    );

NTKERNELAPI
NTSTATUS
KeWaitForSingleObject (
    IN PVOID Object,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

// end_ntddk end_wdm

//
// On Uni-processor systems there is no real Dispatcher Database Lock
// so raising to SYNCH won't help get the lock released any sooner.
// On X86, these functions are implemented in the HAL and don't use
// the KiSynchLevel variable, on other platforms, KiSynchLevel can
// be set appropriately.
//

#define KiLockDispatcherDatabase(OldIrql) \
    *(OldIrql) = KeRaiseIrqlToDpcLevel()

NTKERNELAPI
VOID
FASTCALL
KiUnlockDispatcherDatabase (
    IN KIRQL OldIrql
    );

#define KiLockDispatcherDatabaseAtDpcLevel() \
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL)

#define KiUnlockDispatcherDatabaseFromDpcLevel() \
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL)

VOID
FASTCALL
KiSetPriorityThread (
    IN PRKTHREAD Thread,
    IN KPRIORITY Priority
    );

// begin_ntddk begin_wdm
//
// spin lock functions
//

#define KeInitializeSpinLock(a)             *(a) = 0

#define KeAcquireSpinLockAtDpcLevel(a)      ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL)
#define KeReleaseSpinLockFromDpcLevel(a)

#define KeAcquireSpinLock(a,b)              *(b) = KeRaiseIrqlToDpcLevel()
#define KeReleaseSpinLock(a,b)              KeLowerIrql(b)

#define KeTryToAcquireSpinLock(a,b)         (KeAcquireSpinLock(a,b),TRUE)

// end_wdm end_ntddk

//
// Raise and lower IRQL functions.
//

// begin_wdm begin_ntddk

#if defined(_X86_)

NTHALAPI
VOID
FASTCALL
KfLowerIrql (
    IN KIRQL NewIrql
    );

NTHALAPI
KIRQL
FASTCALL
KfRaiseIrql (
    IN KIRQL NewIrql
    );

NTHALAPI
KIRQL
KeRaiseIrqlToDpcLevel(
    VOID
    );

NTHALAPI
KIRQL
KeRaiseIrqlToSynchLevel(
    VOID
    );

#define KeLowerIrql(a)      KfLowerIrql(a)
#define KeRaiseIrql(a,b)    *(b) = KfRaiseIrql(a)

#endif

//
// Miscellaneous kernel functions
//

// end_wdm

NTKERNELAPI
DECLSPEC_NORETURN
VOID
NTAPI
KeBugCheck (
    IN ULONG BugCheckCode
    );

// begin_wdm

NTKERNELAPI
DECLSPEC_NORETURN
VOID
KeBugCheckEx(
    IN ULONG BugCheckCode,
    IN ULONG_PTR BugCheckParameter1,
    IN ULONG_PTR BugCheckParameter2,
    IN ULONG_PTR BugCheckParameter3,
    IN ULONG_PTR BugCheckParameter4
    );

// end_wdm

NTKERNELAPI
VOID
KeEnterKernelDebugger (
    VOID
    );

// end_ntddk  

VOID
KeContextFromKframes (
    IN PKTRAP_FRAME TrapFrame,
    IN OUT PCONTEXT ContextFrame
    );

VOID
KeContextToKframes (
    IN OUT PKTRAP_FRAME TrapFrame,
    IN PCONTEXT ContextFrame,
    IN ULONG ContextFlags
    );

NTKERNELAPI                                         // nthal
VOID                                                // nthal
KeFlushCurrentTb (                                  // nthal
    VOID                                            // nthal
    );                                              // nthal
                                                    // nthal
VOID
KeSetSystemTime (
    IN PLARGE_INTEGER NewTime,
    OUT PLARGE_INTEGER OldTime
    );

// begin_ntddk begin_wdm

NTKERNELAPI
ULONGLONG
KeQueryInterruptTime (
    VOID
    );

NTKERNELAPI
VOID
KeQuerySystemTime (
    OUT PLARGE_INTEGER CurrentTime
    );

#if !defined(_NTSYSTEM_)

#define KeQueryTickCount() *KeTickCount

#else

#define KeQueryTickCount() KeTickCount

#endif

#define KiQueryLowTickCount KeQueryTickCount

// end_wdm end_ntddk

//
// Define the firmware routine types
//

typedef enum _FIRMWARE_REENTRY {
    HalHaltRoutine,
    HalRebootRoutine,
    HalQuickRebootRoutine,
    HalKdRebootRoutine,
    HalFatalErrorRebootRoutine,
    HalMaximumRoutine
} FIRMWARE_REENTRY, *PFIRMWARE_REENTRY;

//
// Quick reboot routine.
//

VOID
KeQuickRebootSystem(
    VOID
    );

//
// begin_ntddk
//
// Context swap notify routine.
//

typedef
VOID
(FASTCALL *PSWAP_CONTEXT_NOTIFY_ROUTINE)(
    IN HANDLE OldThreadId,
    IN HANDLE NewThreadId
    );

//
// Thread select notify routine.
//

typedef
LOGICAL
(FASTCALL *PTHREAD_SELECT_NOTIFY_ROUTINE)(
    IN HANDLE ThreadId
    );

//
// Time update notify routine.
//

typedef
VOID
(FASTCALL *PTIME_UPDATE_NOTIFY_ROUTINE)(
    IN HANDLE ThreadId,
    IN KPROCESSOR_MODE Mode
    );

// end_ntddk

//
// External references to public kernel data structures
//

extern BOOLEAN KeHasQuickBooted;
extern LARGE_INTEGER KeBootTime;
extern ULONGLONG KeBootTimeBias;

#if !defined(_NTSYSTEM_)
extern const ULONG *KeTimeIncrement;
extern volatile PULONG KeTickCount;
extern volatile PKSYSTEM_TIME KeInterruptTime;
extern volatile PKSYSTEM_TIME KeSystemTime;
#else
extern const ULONG KeTimeIncrement;
extern volatile ULONG KeTickCount;
extern volatile KSYSTEM_TIME KeInterruptTime;
extern volatile KSYSTEM_TIME KeSystemTime;
#endif

#endif // _KE_
