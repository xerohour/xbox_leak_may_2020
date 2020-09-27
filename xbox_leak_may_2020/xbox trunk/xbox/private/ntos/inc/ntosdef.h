/*++ BUILD Version: 0003    // Increment this if a change has global effects

Copyright (c) 1989  Microsoft Corporation

Module Name:

    ntosdef.h

Abstract:

    Common type definitions for the NTOS component that are private to
    NTOS, but shared between NTOS sub-components.

Author:

    Steve Wood (stevewo) 08-May-1989

Revision History:

--*/

#ifndef _NTOSDEF_
#define _NTOSDEF_

//
// Declare the data sections used inside the kernel so that __declspec(allocate)
// will work.
//

#if (_MSC_VER >= 1300) && defined(_NTSYSTEM_)

#pragma data_seg(push)
#pragma data_seg("STICKY")
#pragma data_seg(".rdata")
#pragma data_seg("INIT_RW")
#pragma data_seg(pop)

#define DECLSPEC_STICKY     __declspec(allocate("STICKY"))
#define DECLSPEC_RDATA      __declspec(allocate(".rdata"))
#define DECLSPEC_INITDATA   __declspec(allocate("INIT_RW"))

#else

#define DECLSPEC_STICKY
#define DECLSPEC_RDATA
#define DECLSPEC_INITDATA

#endif

//
// Define interlocked sequenced list structure.
//
// begin_ntddk begin_wdm begin_nthal begin_ntifs begin_ntndis

typedef union _SLIST_HEADER {
    ULONGLONG Alignment;
    struct {
        SINGLE_LIST_ENTRY Next;
        USHORT Depth;
        USHORT Sequence;
    };
} SLIST_HEADER, *PSLIST_HEADER;

// end_ntddk end_wdm end_nthal end_ntifs end_ntndis

//
// Define the number of small pool lists.
//
// N.B. This value is used in pool.h and is used to allocate single entry
//      lookaside lists in the processor block of each processor.

#define POOL_SMALL_LISTS 8

// begin_ntddk begin_wdm begin_nthal begin_ntifs

//
// Define alignment macros to align structure sizes and pointers up and down.
//

#define ALIGN_DOWN(length, type) \
    ((ULONG)(length) & ~(sizeof(type) - 1))

#define ALIGN_UP(length, type) \
    (ALIGN_DOWN(((ULONG)(length) + sizeof(type) - 1), type))

#define ALIGN_DOWN_POINTER(address, type) \
    ((PVOID)((ULONG_PTR)(address) & ~((ULONG_PTR)sizeof(type) - 1)))

#define ALIGN_UP_POINTER(address, type) \
    (ALIGN_DOWN_POINTER(((ULONG_PTR)(address) + sizeof(type) - 1), type))

#define POOL_TAGGING 1

#ifndef DBG
#define DBG 0
#endif

#if DBG
#define IF_DEBUG if (TRUE)
#else
#define IF_DEBUG if (FALSE)
#endif

#if DEVL

// end_ntddk end_wdm end_nthal end_ntifs
//
// Global flag set by NtPartyByNumber(6) controls behaviour of
// NT.  See \nt\sdk\inc\ntexapi.h for flag definitions
//
// begin_ntddk begin_wdm begin_nthal begin_ntifs

extern ULONG NtGlobalFlag;

#define IF_NTOS_DEBUG( FlagName ) \
    if (NtGlobalFlag & (FLG_ ## FlagName))

#else
#define IF_NTOS_DEBUG( FlagName ) if (FALSE)
#endif

//
// Kernel definitions that need to be here for forward reference purposes
//

//
// APC function types
//

//
// Put in an empty definition for the KAPC so that the
// routines can reference it before it is declared.
//

struct _KAPC;

typedef
VOID
(*PKNORMAL_ROUTINE) (
    IN PVOID NormalContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

typedef
VOID
(*PKKERNEL_ROUTINE) (
    IN struct _KAPC *Apc,
    IN OUT PKNORMAL_ROUTINE *NormalRoutine,
    IN OUT PVOID *NormalContext,
    IN OUT PVOID *SystemArgument1,
    IN OUT PVOID *SystemArgument2
    );

typedef
VOID
(*PKRUNDOWN_ROUTINE) (
    IN struct _KAPC *Apc
    );

typedef
BOOLEAN
(*PKSYNCHRONIZE_ROUTINE) (
    IN PVOID SynchronizeContext
    );

typedef
BOOLEAN
(*PKTRANSFER_ROUTINE) (
    VOID
    );

//
//
// Asynchronous Procedure Call (APC) object
//

typedef struct _KAPC {
    CSHORT Type;
    KPROCESSOR_MODE ApcMode;
    BOOLEAN Inserted;
    struct _KTHREAD *Thread;
    LIST_ENTRY ApcListEntry;
    PKKERNEL_ROUTINE KernelRoutine;
    PKRUNDOWN_ROUTINE RundownRoutine;
    PKNORMAL_ROUTINE NormalRoutine;
    PVOID NormalContext;

    //
    // N.B. The following two members MUST be together.
    //

    PVOID SystemArgument1;
    PVOID SystemArgument2;
} KAPC, *PKAPC, *RESTRICTED_POINTER PRKAPC;

// begin_ntndis
//
// DPC routine
//

struct _KDPC;

typedef
VOID
(*PKDEFERRED_ROUTINE) (
    IN struct _KDPC *Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

//
// Deferred Procedure Call (DPC) object
//

typedef struct _KDPC {
    CSHORT Type;
    BOOLEAN Inserted;
    UCHAR Padding;
    LIST_ENTRY DpcListEntry;
    PKDEFERRED_ROUTINE DeferredRoutine;
    PVOID DeferredContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
} KDPC, *PKDPC, *RESTRICTED_POINTER PRKDPC;

// end_ntndis
//
// switch to DBG when appropriate
//

#if DBG
#define PAGED_CODE() \
    if (KeGetCurrentIrql() > APC_LEVEL) { \
    KdPrint(( "EX: Pageable code called at IRQL %d\n", KeGetCurrentIrql() )); \
        ASSERT(FALSE); \
        }
#else
#define PAGED_CODE()
#endif

// end_ntddk end_wdm end_nthal end_ntifs


// begin_ntddk begin_wdm begin_nthal begin_ntndis begin_ntifs
//
// Define function decoration depending on whether a driver, a file system,
// or a kernel component is being built.
//
// end_wdm

#if !defined(_NTSYSTEM_)

#define NTKERNELAPI DECLSPEC_IMPORT         // wdm

#else

#define NTKERNELAPI

#endif

//
// Define function decoration depending on whether the HAL or other kernel
// component is being build.
//

#define NTHALAPI NTKERNELAPI

//
// Define a statically allocated object string.
//

#define INITIALIZED_OBJECT_STRING(ObjectString, Value)                      \
    OCHAR ObjectString##Buffer[] = Value;                                   \
    OBJECT_STRING ObjectString = {                                          \
        sizeof(Value) - sizeof(OCHAR),                                      \
        sizeof(Value),                                                      \
        ObjectString##Buffer                                                \
    }

#define INITIALIZED_OBJECT_STRING_RDATA(ObjectString, Value)                \
    OCHAR DECLSPEC_RDATA ObjectString##Buffer[] = Value;                    \
    OBJECT_STRING DECLSPEC_RDATA ObjectString = {                           \
        sizeof(Value) - sizeof(OCHAR),                                      \
        sizeof(Value),                                                      \
        ObjectString##Buffer                                                \
    }

//
// Define a statically allocated list entry.
//

#define INITIALIZED_LIST_ENTRY(ListEntry)                       \
    LIST_ENTRY ListEntry = {                                    \
        &ListEntry, &ListEntry                                  \
    }

// end_ntddk end_nthal end_ntndis end_ntifs

#endif // _NTOSDEF_
