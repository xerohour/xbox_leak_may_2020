/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    psp.h

Abstract:

    Private Interfaces for process structure.

--*/

#ifndef _PSP_
#define _PSP_

#include <ntos.h>
#include <ki.h>

#define PSP_MAX_CREATE_THREAD_NOTIFY 8

ULONG PspCreateThreadNotifyRoutineCount;
PCREATE_THREAD_NOTIFY_ROUTINE PspCreateThreadNotifyRoutine[ PSP_MAX_CREATE_THREAD_NOTIFY ];

VOID
PspReaper(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

#endif // _PSP_
