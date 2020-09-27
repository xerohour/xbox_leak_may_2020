/*++ BUILD Version: 0007    // Increment this if a change has global effects

Copyright (c) 1989  Microsoft Corporation

Module Name:

    ex.h

Abstract:

    Public executive data structures and procedure prototypes.

Author:

    Mark Lucovsky (markl) 23-Feb-1989

Revision History:

--*/

#ifndef _EX_
#define _EX_

//
// Empty struct def so we can forward reference ETHREAD
//

struct _ETHREAD;

//
// System Initialization procedure for EX subcomponent of NTOS (in exinit.c)
//

VOID
InitializePool(
    VOID
    );

// begin_ntddk begin_nthal begin_ntifs begin_wdm

NTKERNELAPI
PVOID
ExAllocatePool(
    IN SIZE_T NumberOfBytes
    );

NTKERNELAPI
PVOID
NTAPI
ExAllocatePoolWithTag(
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    );

#ifndef POOL_TAGGING
#define ExAllocatePoolWithTag(a,b) ExAllocatePool(a)
#endif //POOL_TAGGING

NTKERNELAPI
VOID
NTAPI
ExFreePool(
    IN PVOID P
    );

// end_ntddk end_wdm end_nthal end_ntifs

NTKERNELAPI                                     // ntifs
ULONG                                           // ntifs
ExQueryPoolBlockSize (                          // ntifs
    IN PVOID PoolBlock                          // ntifs
    );                                          // ntifs

//
// Interlocked support routine definitions.
//
// begin_ntddk begin_wdm begin_nthal begin_ntifs begin_ntndis
//

NTKERNELAPI
VOID
FASTCALL
ExInterlockedAddLargeStatistic (
    IN PLARGE_INTEGER Addend,
    IN ULONG Increment
    );

// end_ntndis

NTKERNELAPI
LARGE_INTEGER
ExInterlockedAddLargeInteger (
    IN PLARGE_INTEGER Addend,
    IN LARGE_INTEGER Increment
    );

// begin_wdm begin_ntddk begin_nthal begin_ntifs

NTKERNELAPI
LONGLONG
FASTCALL
ExInterlockedCompareExchange64 (
    IN PLONGLONG Destination,
    IN PLONGLONG Exchange,
    IN PLONGLONG Comperand
    );

NTKERNELAPI
PLIST_ENTRY
FASTCALL
ExInterlockedInsertHeadList (
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry
    );

NTKERNELAPI
PLIST_ENTRY
FASTCALL
ExInterlockedInsertTailList (
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry
    );

NTKERNELAPI
PLIST_ENTRY
FASTCALL
ExInterlockedRemoveHeadList (
    IN PLIST_ENTRY ListHead
    );

// begin_ntndis

//
// Define interlocked sequenced listhead functions.
//
// A sequenced interlocked list is a singly linked list with a header that
// contains the current depth and a sequence number. Each time an entry is
// inserted or removed from the list the depth is updated and the sequence
// number is incremented. This enables MIPS, Alpha, and Pentium and later
// machines to insert and remove from the list without the use of spinlocks.
// The PowerPc, however, must use a spinlock to synchronize access to the
// list.
//
// N.B. A spinlock must be specified with SLIST operations. However, it may
//      not actually be used.
//

/*++

VOID
ExInitializeSListHead (
    IN PSLIST_HEADER SListHead
    )

Routine Description:

    This function initializes a sequenced singly linked listhead.

Arguments:

    SListHead - Supplies a pointer to a sequenced singly linked listhead.

Return Value:

    None.

--*/

#define ExInitializeSListHead(_listhead_) (_listhead_)->Alignment = 0

/*++

USHORT
ExQueryDepthSList (
    IN PSLIST_HEADERT SListHead
    )

Routine Description:

    This function queries the current number of entries contained in a
    sequenced single linked list.

Arguments:

    SListHead - Supplies a pointer to the sequenced listhead which is
        be queried.

Return Value:

    The current number of entries in the sequenced singly linked list is
    returned as the function value.

--*/

#define ExQueryDepthSList(_listhead_) (USHORT)(_listhead_)->Depth

#if !defined(_WINBASE_)

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
InterlockedPopEntrySList (
    IN PSLIST_HEADER ListHead
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
InterlockedPushEntrySList (
    IN PSLIST_HEADER ListHead,
    IN PSINGLE_LIST_ENTRY ListEntry
    );

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
InterlockedFlushSList (
    IN PSLIST_HEADER ListHead
    );

#endif

// end_ntndis
// end_ntddk end_nthal end_ntifs end_wdm

//
// Timer Rundown
//

NTKERNELAPI
VOID
ExTimerRundown (
    VOID
    );

//
// Reader/writer lock.
//

typedef struct _ERWLOCK {
    LONG LockCount;
    ULONG WritersWaitingCount;
    ULONG ReadersWaitingCount;
    ULONG ReadersEntryCount;
    KEVENT WriterEvent;
    KSEMAPHORE ReaderSemaphore;
} ERWLOCK, *PERWLOCK;

NTKERNELAPI
VOID
ExInitializeReadWriteLock(
    IN PERWLOCK ReadWriteLock
    );

NTKERNELAPI
VOID
ExAcquireReadWriteLockExclusive(
    IN PERWLOCK ReadWriteLock
    );

NTKERNELAPI
VOID
ExAcquireReadWriteLockShared(
    IN PERWLOCK ReadWriteLock
    );

NTKERNELAPI
VOID
ExReleaseReadWriteLock(
    IN PERWLOCK ReadWriteLock
    );

#if DBG

#define ExDbgIsReadWriteLockLocked(ReadWriteLock) \
    ((ReadWriteLock)->LockCount >= 0)

#define ExDbgIsReadWriteLockExclusive(ReadWriteLock) \
    (ExDbgIsReadWriteLockLocked(ReadWriteLock) && ((ReadWriteLock)->ReadersEntryCount == 0))

#define ExDbgIsReadWriteLockShared(ReadWriteLock) \
    (ExDbgIsReadWriteLockLocked(ReadWriteLock) && ((ReadWriteLock)->ReadersEntryCount > 0))

#endif

//
// Raise exception from kernel mode.
//

NTKERNELAPI
VOID
NTAPI
ExRaiseException (
    PEXCEPTION_RECORD ExceptionRecord
    );

// begin_ntddk begin_wdm begin_ntifs
//
// Raise status from kernel mode.
//

NTKERNELAPI
VOID
NTAPI
ExRaiseStatus (
    IN NTSTATUS Status
    );

// end_wdm end_ntddk end_ntifs

// begin_ntddk

//
// UUID Generation
//

typedef GUID UUID;

// end_ntddk

#endif /* _EX_ */
