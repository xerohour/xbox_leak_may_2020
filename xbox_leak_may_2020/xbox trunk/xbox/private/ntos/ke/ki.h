/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    ki.h

Abstract:

    This module contains the private (internal) header file for the
    kernel.

Author:

    David N. Cutler (davec) 28-Feb-1989

Revision History:

--*/

#ifndef _KI_
#define _KI_
#include "ntos.h"
#include "stdio.h"
#include "stdlib.h"
#include "zwapi.h"

//
// Private (internal) constant definitions.
//
// Priority increment value definitions
//

#define ALERT_INCREMENT 2           // Alerted unwait priority increment
#define BALANCE_INCREMENT 10        // Balance set priority increment
#define RESUME_INCREMENT 0          // Resume thread priority increment
#define TIMER_EXPIRE_INCREMENT 0    // Timer expiration priority increment

//
// Define time critical priority class base.
//

#define TIME_CRITICAL_PRIORITY_BOUND 14

//
// Define NIL pointer value.
//

#define NIL (PVOID)NULL             // Null pointer to void

//
// Define macros which are used in the kernel only
//
// Clear member in set
//

#define ClearMember(Member, Set) \
    Set = Set & (~(1 << (Member)))

//
// Set member in set
//

#define SetMember(Member, Set) \
    Set = Set | (1 << (Member))

#define FindFirstSetLeftMember(Set, Member) {                          \
    ULONG _Bit;                                                        \
    ULONG _Mask;                                                       \
    ULONG _Offset = 16;                                                \
    if ((_Mask = Set >> 16) == 0) {                                    \
        _Offset = 0;                                                   \
        _Mask = Set;                                                   \
    }                                                                  \
    if (_Mask >> 8) {                                                  \
        _Offset += 8;                                                  \
    }                                                                  \
    if ((_Bit = Set >> _Offset) & 0xf0) {                              \
        _Bit >>= 4;                                                    \
        _Offset += 4;                                                  \
    }                                                                  \
    *(Member) = KiFindLeftNibbleBitTable[_Bit] + _Offset;              \
}

//
// Lock and unlock APC queue lock.
//

#define KiLockApcQueue(Thread, OldIrql) \
    *(OldIrql) = KeRaiseIrqlToSynchLevel()

#define KiLockApcQueueAtDpcLevel(Thread) \
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL)

#define KiUnlockApcQueue(Thread, OldIrql) KeLowerIrql((OldIrql))

#define KiUnlockApcQueueFromDpcLevel(Thread) \
    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL)

// VOID
// KiBoostPriorityThread (
//    IN PKTHREAD Thread,
//    IN KPRIORITY Increment
//    )
//
//*++
//
// Routine Description:
//
//    This function boosts the priority of the specified thread using
//    the same algorithm used when a thread gets a boost from a wait
//    operation.
//
// Arguments:
//
//    Thread  - Supplies a pointer to a dispatcher object of type thread.
//
//    Increment - Supplies the priority increment that is to be applied to
//        the thread's priority.
//
// Return Value:
//
//    None.
//
//--*

#define KiBoostPriorityThread(Thread, Increment) {              \
    KPRIORITY NewPriority;                                      \
    PKPROCESS Process;                                          \
                                                                \
    if ((Thread)->Priority < LOW_REALTIME_PRIORITY) {           \
        if ((Thread)->PriorityDecrement == 0) {                 \
            NewPriority = (Thread)->BasePriority + (Increment); \
            if (NewPriority > (Thread)->Priority) {             \
                if (NewPriority >= LOW_REALTIME_PRIORITY) {     \
                    NewPriority = LOW_REALTIME_PRIORITY - 1;    \
                }                                               \
                                                                \
                Process = (Thread)->ApcState.Process;           \
                (Thread)->Quantum = Process->ThreadQuantum;     \
                KiSetPriorityThread((Thread), NewPriority);     \
            }                                                   \
        }                                                       \
    }                                                           \
}

// VOID
// KiInsertWaitList (
//    IN KPROCESSOR_MODE WaitMode,
//    IN PKTHREAD Thread
//    )
//
//*++
//
// Routine Description:
//
//    This function inserts the specified thread in the appropriate
//    wait list.
//
// Arguments:
//
//    WaitMode - Supplies the processor mode of the wait operation.
//
//    Thread - Supplies a pointer to a dispatcher object of type
//        thread.
//
// Return Value:
//
//    None.
//
//--*

#define KiInsertWaitList(_WaitMode, _Thread) {                  \
    PLIST_ENTRY _ListHead;                                      \
    _ListHead = &KiWaitInListHead;                              \
    InsertTailList(_ListHead, &(_Thread)->WaitListEntry);       \
}

//
// Private (internal) structure definitions.
//
// APC Parameter structure.
//

typedef struct _KAPC_RECORD {
    PKNORMAL_ROUTINE NormalRoutine;
    PVOID NormalContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
} KAPC_RECORD, *PKAPC_RECORD;

//
// Executive initialization.
//

VOID
ExpInitializeExecutive (
    VOID
    );

//
// Private (internal) function definitions.
//

VOID
FASTCALL
KiActivateWaiterQueue (
    IN PRKQUEUE Queue
    );

VOID
KiApcInterrupt (
    VOID
    );

#if DBG

VOID
KiCheckTimerTable (
    IN ULARGE_INTEGER SystemTime
    );

#endif

ULONG
KiComputeTimerTableIndex (
    IN LARGE_INTEGER Interval,
    IN LARGE_INTEGER CurrentCount,
    IN PRKTIMER Timer
    );

PLARGE_INTEGER
FASTCALL
KiComputeWaitInterval (
    IN PLARGE_INTEGER OriginalTime,
    IN PLARGE_INTEGER DueTime,
    IN OUT PLARGE_INTEGER NewTime
    );

NTSTATUS
KiContinue (
    IN PCONTEXT ContextRecord,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PKTRAP_FRAME TrapFrame
    );

VOID
KiDeliverApc (
    VOID
    );

VOID
KiDeliverUserApc (
    VOID
    );

BOOLEAN
KiDisableInterrupts (
    VOID
    );

VOID
KiRestoreInterrupts (
    IN BOOLEAN Enable
    );

VOID
KiDispatchException (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PKTRAP_FRAME TrapFrame,
    IN BOOLEAN FirstChance
    );

VOID
KiDispatchInterrupt (
    VOID
    );

PKTHREAD
FASTCALL
KiFindReadyThread (
    KPRIORITY LowPriority
    );

VOID
KiInitializeContextThread (
    IN PKTHREAD Thread,
    IN SIZE_T ThreadLocalStorageSize,
    IN PKSYSTEM_ROUTINE SystemRoutine,
    IN PKSTART_ROUTINE StartRoutine OPTIONAL,
    IN PVOID StartContext OPTIONAL
    );

VOID
KiInitializeKernel (
    VOID
    );

VOID
KiInitSystem (
    VOID
    );

BOOLEAN
KiInitMachineDependent (
    VOID
    );

LONG
FASTCALL
KiInsertQueue (
    IN PRKQUEUE Queue,
    IN PLIST_ENTRY Entry,
    IN BOOLEAN Head
    );

BOOLEAN
FASTCALL
KiInsertQueueApc (
    IN PKAPC Apc,
    IN KPRIORITY Increment
    );

LOGICAL
FASTCALL
KiInsertTreeTimer (
    IN PRKTIMER Timer,
    IN LARGE_INTEGER Interval
    );

VOID
KiInterruptDispatch (
    VOID
    );

VOID
KiLevelInterruptDispatch (
    VOID
    );

#if defined(i386)

VOID
KiInitializePcr (
    IN PKPCR    Pcr,
    IN PKTHREAD Thread,
    IN PVOID DpcStack
    );

VOID
KiFlushNPXState (
    VOID
    );

VOID
KiSetHardwareTrigger (
    VOID
    );

#ifdef DBGMP
VOID
KiPollDebugger (
    VOID
    );
#endif

#endif

KIRQL
KiLockDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue
    );

PRKTHREAD
KiQuantumEnd (
    VOID
    );

NTSTATUS
KiRaiseException (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PCONTEXT ContextRecord,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PKTRAP_FRAME TrapFrame,
    IN BOOLEAN FirstChance
    );

VOID
FASTCALL
KiReadyThread (
    IN PRKTHREAD Thread
    );

LOGICAL
FASTCALL
KiReinsertTreeTimer (
    IN PRKTIMER Timer,
    IN ULARGE_INTEGER DueTime
    );

#if DBG

#define KiRemoveTreeTimer(Timer)               \
    (Timer)->Header.Inserted = FALSE;          \
    RemoveEntryList(&(Timer)->TimerListEntry); \
    (Timer)->TimerListEntry.Flink = NULL;      \
    (Timer)->TimerListEntry.Blink = NULL

#else

#define KiRemoveTreeTimer(Timer)               \
    (Timer)->Header.Inserted = FALSE;          \
    RemoveEntryList(&(Timer)->TimerListEntry)

#endif

#define KiRequestApcInterrupt() KiRequestSoftwareInterrupt(APC_LEVEL)

#define KiRequestDispatchInterrupt()

PRKTHREAD
FASTCALL
KiSelectNextThread (
    IN PRKTHREAD Thread
    );

VOID
KiSetSystemTime (
    IN PLARGE_INTEGER NewTime,
    OUT PLARGE_INTEGER OldTime
    );

VOID
KiSuspendNop (
    IN struct _KAPC *Apc,
    IN OUT PKNORMAL_ROUTINE *NormalRoutine,
    IN OUT PVOID *NormalContext,
    IN OUT PVOID *SystemArgument1,
    IN OUT PVOID *SystemArgument2
    );

VOID
KiSuspendThread (
    IN PVOID NormalContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

LONG_PTR
FASTCALL
KiSwapThread (
    VOID
    );

VOID
KiThreadStartup (
    IN PVOID StartContext
    );

VOID
KiTimerExpiration (
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

VOID
FASTCALL
KiTimerListExpire (
    IN PLIST_ENTRY ExpiredListHead,
    IN KIRQL OldIrql
    );

VOID
KiUnexpectedInterrupt (
    VOID
    );

VOID
KiUnlockDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN KIRQL OldIrql
    );

VOID
FASTCALL
KiUnwaitThread (
    IN PRKTHREAD Thread,
    IN LONG_PTR WaitStatus,
    IN KPRIORITY Increment
    );

VOID
FASTCALL
KiWaitSatisfyAll (
    IN PRKWAIT_BLOCK WaitBlock
    );

//
// VOID
// FASTCALL
// KiWaitSatisfyAny (
//    IN PKMUTANT Object,
//    IN PKTHREAD Thread
//    )
//
//
// Routine Description:
//
//    This function satisfies a wait for any type of object and performs
//    any side effects that are necessary.
//
// Arguments:
//
//    Object - Supplies a pointer to a dispatcher object.
//
//    Thread - Supplies a pointer to a dispatcher object of type thread.
//
// Return Value:
//
//    None.
//

#define KiWaitSatisfyAny(_Object_, _Thread_) {                               \
    if (((_Object_)->Header.Type & DISPATCHER_OBJECT_TYPE_MASK) == EventSynchronizationObject) { \
        (_Object_)->Header.SignalState = 0;                                  \
                                                                             \
    } else if ((_Object_)->Header.Type == SemaphoreObject) {                 \
        (_Object_)->Header.SignalState -= 1;                                 \
                                                                             \
    } else if ((_Object_)->Header.Type == MutantObject) {                    \
        (_Object_)->Header.SignalState -= 1;                                 \
        if ((_Object_)->Header.SignalState == 0) {                           \
            (_Object_)->OwnerThread = (_Thread_);                            \
            if ((_Object_)->Abandoned == TRUE) {                             \
                (_Object_)->Abandoned = FALSE;                               \
                (_Thread_)->WaitStatus = STATUS_ABANDONED;                   \
            }                                                                \
                                                                             \
            InsertHeadList((_Thread_)->MutantListHead.Blink,                 \
                           &(_Object_)->MutantListEntry);                    \
        }                                                                    \
    }                                                                        \
}

//
// VOID
// FASTCALL
// KiWaitSatisfyMutant (
//    IN PKMUTANT Object,
//    IN PKTHREAD Thread
//    )
//
//
// Routine Description:
//
//    This function satisfies a wait for a mutant object.
//
// Arguments:
//
//    Object - Supplies a pointer to a dispatcher object.
//
//    Thread - Supplies a pointer to a dispatcher object of type thread.
//
// Return Value:
//
//    None.
//

#define KiWaitSatisfyMutant(_Object_, _Thread_) {                            \
    (_Object_)->Header.SignalState -= 1;                                     \
    if ((_Object_)->Header.SignalState == 0) {                               \
        (_Object_)->OwnerThread = (_Thread_);                                \
        if ((_Object_)->Abandoned == TRUE) {                                 \
            (_Object_)->Abandoned = FALSE;                                   \
            (_Thread_)->WaitStatus = STATUS_ABANDONED;                       \
        }                                                                    \
                                                                             \
        InsertHeadList((_Thread_)->MutantListHead.Blink,                     \
                       &(_Object_)->MutantListEntry);                        \
    }                                                                        \
}

//
// VOID
// FASTCALL
// KiWaitSatisfyOther (
//    IN PKMUTANT Object
//    )
//
//
// Routine Description:
//
//    This function satisfies a wait for any type of object except a mutant
//    and performs any side effects that are necessary.
//
// Arguments:
//
//    Object - Supplies a pointer to a dispatcher object.
//
// Return Value:
//
//    None.
//

#define KiWaitSatisfyOther(_Object_) {                                       \
    if (((_Object_)->Header.Type & DISPATCHER_OBJECT_TYPE_MASK) == EventSynchronizationObject) { \
        (_Object_)->Header.SignalState = 0;                                  \
                                                                             \
    } else if ((_Object_)->Header.Type == SemaphoreObject) {                 \
        (_Object_)->Header.SignalState -= 1;                                 \
                                                                             \
    }                                                                        \
}

VOID
FASTCALL
KiWaitTest (
    IN PVOID Object,
    IN KPRIORITY Increment
    );

#endif // _KI_

//
// External references to private kernel data structures
//

extern PKDEBUG_ROUTINE KiDebugRoutine;
extern LIST_ENTRY KiDispatcherReadyListHead[MAXIMUM_PRIORITY];
extern ULONG_PTR KiHardwareTrigger;
extern KAFFINITY KiIdleSummary;
extern const UCHAR KiFindLeftNibbleBitTable[];
extern ULONG KiReadySummary;
extern const LARGE_INTEGER KiTimeIncrementReciprocal;
extern const CCHAR KiTimeIncrementShiftCount;
extern LIST_ENTRY KiTimerTableListHead[TIMER_TABLE_SIZE];
extern KDPC KiTimerExpireDpc;
extern LIST_ENTRY KiWaitInListHead;
extern KPROCESS KiIdleProcess;
extern KPROCESS KiSystemProcess;

#if defined(i386)

VOID
KeFlushCurrentTbAndInvalidateAllCaches (
    VOID
    );

VOID
KeInvalidateAllCaches (
    VOID
    );

#endif

#if DBG

extern ULONG KiMaximumSearchCount;

#endif
