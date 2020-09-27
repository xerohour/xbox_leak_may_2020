/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    exit.cpp

Abstract:

    This program contains routines to stress and test threads
    and threading related APIs

--*/

#include "threading.h"

VOID
WINAPI
ExitThreadTestThreadProc1(
    PTHREAD_PARAMETERS tpm
    )
{
    ExitThread( tpm->ExitCode );
}

DWORD
WINAPI
ExitThreadTestThreadProc2(
    PTHREAD_PARAMETERS tpm
    )
{
    return tpm->ExitCode;
}

DWORD
WINAPI
RunningThreadProc(
    PTHREAD_PARAMETERS tpm
    )
{
    volatile THREAD_PARAMETERS *p = tpm;

    while ( p->ExitCode ) {
        NOTHING;
    }

    return p->ExitCode;
}

VOID
ExitThreadVariation(
    HANDLE LogHandle,
    THREADPROC ThreadProc,
    LPSTR  Variation
    )
{
    HANDLE ThreadHandle;
    THREAD_PARAMETERS tpm;
    DWORD ExitCode;
    ULONG Seed = GetTickCount();

    BEGIN_VARIATION( Variation )
    {
        tpm.ExitCode = RtlRandom( &Seed );

        ThreadHandle = CreateThread(
                            0,
                            0,
                            (LPTHREAD_START_ROUTINE)ThreadProc,
                            (LPVOID)&tpm,
                            0,
                            NULL
                            );

        if ( !ThreadHandle ) {
            LOG_MSG_BLOCK( "CreateThread failed" );
        } else {
            WaitForSingleObject( ThreadHandle, INFINITE );

            if ( !GetExitCodeThread(ThreadHandle, &ExitCode) ) {
                LOG_MSG_BLOCK( "GetExitCodeThread failed" );
            } else if ( tpm.ExitCode == ExitCode ) {
                LOG_PASS();
            } else {
                LOG_MSG_FAIL( "mismatch exit code" );
            }

            CloseHandle( ThreadHandle );
        }
    }
    END_VARIATION();
}

VOID
GetExitCodeThreadVariation(
    HANDLE LogHandle,
    LPSTR  Variation,
    HANDLE Handle,
    PTHREAD_PARAMETERS tpm OPTIONAL,
    LPSTR  BlockMsg,
    BOOL   InvalidCase,
    BOOL   CleanupHandle
    )
{
    DWORD ExitCode;
    BOOL  Success;

    BEGIN_VARIATION( Variation )
    {
        if ( CleanupHandle && !Handle ) {
            LOG_MSG_BLOCK( BlockMsg );
        } else {
            Success = GetExitCodeThread( Handle, &ExitCode );

            if ( ARGUMENT_PRESENT(tpm) ) {
                LOG_PASS_FAIL( Success && tpm->ExitCode == ExitCode );
            } else {
                LOG_PASS_FAIL( InvalidCase ? !Success : Success );
            }
        }
    }
    END_VARIATION();

    if ( CleanupHandle && Handle && Handle != INVALID_HANDLE_VALUE ) {
        CloseHandle( Handle );
    }
}

VOID
ExitThreadTest(
    HANDLE LogHandle
    )
{
    HANDLE Handle;
    THREAD_PARAMETERS tpm;

    xSetFunctionName( LogHandle, "ExitThread" );

    ExitThreadVariation(
        LogHandle,
        (THREADPROC)ExitThreadTestThreadProc1,
        "exit code from ExitThread"
        );

    ExitThreadVariation(
        LogHandle,
        ExitThreadTestThreadProc2,
        "exit code from return value"
        );

    xSetFunctionName( LogHandle, "GetExitCodeThread" );

    GetExitCodeThreadVariation(
        LogHandle,
        "exit code from ExitThread",
        PrepareThread( (THREADPROC)ExitThreadTestThreadProc1, &tpm, 0 ),
        &tpm,
        "CreateThread failed",
        FALSE,
        TRUE
        );

    GetExitCodeThreadVariation(
        LogHandle,
        "exit code from return value",
        PrepareThread( ExitThreadTestThreadProc2, &tpm, 0 ),
        &tpm,
        "CreateThread failed",
        FALSE,
        TRUE
        );

    tpm.ExitCode = STILL_ACTIVE;
    GetExitCodeThreadVariation(
        LogHandle,
        "exit code from current thread",
        GetCurrentThread(),
        &tpm,
        "GetCurrentThread failed",
        FALSE,
        FALSE
        );

    GetExitCodeThreadVariation(
        LogHandle,
        "exit code from suspended thread",
        Handle = PrepareThread( (THREADPROC)ExitThreadTestThreadProc1, &tpm, CREATE_SUSPENDED, FALSE ),
        &tpm,
        "CreateThread failed",
        FALSE,
        FALSE
        );
    ResumeThread( Handle );
    CloseHandle( Handle );

    //
    // Invalid cases: calling GetExitCode thread by passing
    // various kinds of handle, event handle for example
    //
    GetExitCodeThreadVariation(
        LogHandle,
        "handle=0xdeadbeef",
        (HANDLE)0xdeadbeef,
        0,
        0,
        TRUE,
        FALSE
        );

    GetExitCodeThreadVariation(
        LogHandle,
        "handle=NULL",
        NULL,
        0,
        0,
        TRUE,
        FALSE
        );

    GetExitCodeThreadVariation(
        LogHandle,
        "handle=INVALID_HANDLE_VALUE",
        INVALID_HANDLE_VALUE,
        0,
        0,
        TRUE,
        FALSE
        );

    GetExitCodeThreadVariation(
        LogHandle,
        "handle=event handle",
        CreateEvent(0, 0, 0, 0),
        0,
        "CreateEvent failed",
        TRUE,
        TRUE
        );

    GetExitCodeThreadVariation(
        LogHandle,
        "handle=semaphore handle",
        CreateSemaphore(0, 0, 1, 0),
        0,
        "CreateSemaphore failed",
        TRUE,
        TRUE
        );

    GetExitCodeThreadVariation(
        LogHandle,
        "handle=mutex handle",
        CreateMutex(0, 0, 0),
        0,
        "CreateMutex failed",
        TRUE,
        TRUE
        );

    GetExitCodeThreadVariation(
        LogHandle,
        "handle=waitable timer handle",
        CreateWaitableTimer(0, 0, 0),
        0,
        "CreateWaitableTimer failed",
        TRUE,
        TRUE
        );
}
