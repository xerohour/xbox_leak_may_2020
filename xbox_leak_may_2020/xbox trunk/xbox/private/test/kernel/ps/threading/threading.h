/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    precomp.h

Abstract:

    This program contains routines to stress and test threads
    and threading related APIs

--*/

#include <ntos.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <windows.h>
#include <testutil.h>
#include <xlog.h>
#include <xtestlib.h>

typedef struct _THREAD_PARAMETERS {

    DWORD ExceptionCode;
    DWORD ExitCode;
    DWORD StackSize;
    DWORD ThreadID;
    DWORD ExpectedThreadID;
    HANDLE LogHandle;

} THREAD_PARAMETERS, *PTHREAD_PARAMETERS;

typedef DWORD (WINAPI *THREADPROC)(PTHREAD_PARAMETERS);

typedef struct _FIBER_PARAMETERS {

    PVOID MainFiber;
    LONG  Counter;
    ULONG ExpectedCounter;
    HANDLE LogHandle;

} FIBER_PARAMETERS, *PFIBER_PARAMETERS;

typedef VOID (WINAPI *FIBERPROC)(PFIBER_PARAMETERS);

VOID
CreateThreadTest(
    HANDLE LogHandle
    );

VOID
ExitThreadTest(
    HANDLE LogHandle
    );

VOID
ThreadIDTest(
    HANDLE LogHandle
    );

VOID
ThreadLocalStorageTest(
    HANDLE LogHandle
    );

VOID
ThreadPriorityTest(
    HANDLE LogHandle
    );

VOID
SuspendResumeTest(
    HANDLE LogHandle
    );

VOID
ThreadNotificationTest(
    HANDLE LogHandle
    );

VOID
FiberTest(
    HANDLE LogHandle
    );

__inline
HANDLE
PrepareThread(
    THREADPROC ThreadProc,
    PTHREAD_PARAMETERS tpm,
    DWORD CreationFlags,
    BOOL WaitUntilTerminate = TRUE
    )
{
    HANDLE Handle;
    ULONG Seed = GetTickCount();

    if ( tpm ) {
        tpm->ExitCode = CreationFlags == CREATE_SUSPENDED ? STILL_ACTIVE : RtlRandom( &Seed );
    }

    Handle = CreateThread(
                0,
                0,
                (LPTHREAD_START_ROUTINE)ThreadProc,
                (LPVOID)tpm,
                CreationFlags,
                0
                );

    if ( Handle && WaitUntilTerminate && CreationFlags != CREATE_SUSPENDED ) {
        WaitForSingleObject( Handle, INFINITE );
    }

    return Handle;
}
