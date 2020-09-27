/*++ BUILD Version: 0009    // Increment this if a change has global effects

Copyright (c) 1989  Microsoft Corporation

Module Name:

    ps.h

Abstract:

    This module contains the process structure public data structures and
    procedure prototypes to be used within the NT system.

Author:

    Mark Lucovsky       16-Feb-1989

Revision History:

--*/

#ifndef _PS_
#define _PS_

//
// Specifies the base address of XBOXKRNL.EXE.
//

#define PsNtosImageBase ((PVOID)0x80010000)

//
// Thread Object
//
// Thread object body.  A pointer to this structure is returned when a handle
// to a thread object is referenced.  This structure contains a thread control
// block (TCB) which is the kernel's representation of a thread.
//
// If you remove a field from this structure, please also
// remove the reference to it from within the kernel debugger
// (nt\private\sdktools\ntsd\ntkext.c)
//

typedef struct _ETHREAD {
    KTHREAD Tcb;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER ExitTime;

    union {
        NTSTATUS ExitStatus;
        PVOID OfsChain;       // needed for the nt build of the C runtime
    };
    union {
        LIST_ENTRY ReaperListEntry;
        LIST_ENTRY ActiveTimerListHead;
    };
    HANDLE UniqueThread;
    PVOID StartAddress;

    //
    // Io
    //

    LIST_ENTRY IrpList;

#ifdef DEVKIT
    //
    // Dm
    //
    // keep this at the end so kd exts don't get confused
    //

    PVOID DebugData;
#endif
} ETHREAD, *PETHREAD;

//
// Global Variables
//

extern LIST_ENTRY PsReaperListHead;
extern KDPC PsReaperDpc;

BOOLEAN
PsInitSystem (
    VOID
    );

//
// Get Gurrent Prototypes
//

#define THREAD_TO_PROCESS(thread) ((thread)->Tcb.ApcState.Process)

#define PsGetCurrentProcess() (KeGetCurrentThread()->ApcState.Process)

#define PsGetCurrentThread() (CONTAINING_RECORD((KeGetCurrentThread()),ETHREAD,Tcb))

#define PsGetCurrentThreadId() (PsGetCurrentThread()->UniqueThread)

// begin_ntddk begin_wdm
//
// System Thread and Process Creation and Termination
//

NTKERNELAPI
NTSTATUS
PsCreateSystemThread(
    OUT PHANDLE ThreadHandle,
    OUT PHANDLE ThreadId OPTIONAL,
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext,
    IN BOOLEAN DebuggerThread
    );

NTKERNELAPI
NTSTATUS
PsCreateSystemThreadEx(
    OUT PHANDLE ThreadHandle,
    IN SIZE_T ThreadExtensionSize,
    IN SIZE_T KernelStackSize,
    IN SIZE_T TlsDataSize,
    OUT PHANDLE ThreadId OPTIONAL,
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext,
    IN BOOLEAN CreateSuspended,
    IN BOOLEAN DebuggerThread,
    IN PKSYSTEM_ROUTINE SystemRoutine OPTIONAL
    );

DECLSPEC_NORETURN
NTKERNELAPI
VOID
PsTerminateSystemThread(
    IN NTSTATUS ExitStatus
    );

// end_ntddk end_wdm

// begin_ntddk

typedef
VOID
(*PCREATE_THREAD_NOTIFY_ROUTINE)(
    IN PETHREAD Thread,
    IN HANDLE ThreadId,
    IN BOOLEAN Create
    );

NTSTATUS
PsSetCreateThreadNotifyRoutine(
    IN PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine
    );

// end_ntddk

//
// BOOLEAN
// PsIsThreadTerminating(
//   IN PETHREAD Thread
//   )
//
//  Returns TRUE if thread is in the process of terminating.
//

#define PsIsThreadTerminating(T)                                            \
    (T)->Tcb.HasTerminated

//
// Cid
//

#if !defined(_NTSYSTEM_)
#define PsLookupThreadByThreadId(ThreadId, Thread) \
    ObReferenceObjectByHandle(ThreadId, PsThreadObjectType, Thread);
#else
#define PsLookupThreadByThreadId(ThreadId, Thread) \
    ObReferenceObjectByHandle(ThreadId, &PsThreadObjectType, Thread);
#endif

//
// Process statistics.
//

typedef struct _PS_STATISTICS {
    ULONG Length;
    ULONG ThreadCount;
    ULONG HandleCount;
} PS_STATISTICS, *PPS_STATISTICS;

NTKERNELAPI
NTSTATUS
PsQueryStatistics(
    IN OUT PPS_STATISTICS ProcessStatistics
    );

#endif // _PS_
