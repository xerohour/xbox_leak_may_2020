/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    priority.cpp

Abstract:

    This program contains routines to stress and test threads
    and threading related APIs

Author:

    Sakphong Chanbai (schanbai) 10-May-2000

Environment:

    Xbox

Revision History:

--*/


#include "threading.h"


static
DWORD
WINAPI
ThreadProc(
    PTHREAD_PARAMETERS tpm
    )
{
    return (DWORD)tpm;
}


VOID
GetThreadPriorityVariation(
    HANDLE LogHandle,
    LPSTR  Variation,
    HANDLE ThreadHandle,
    LPSTR  BlockMsg,
    BOOL   InvalidCase,
    BOOL   CleanupHandle
    )
{
    BOOL Success;

    BEGIN_VARIATION( Variation )
    {
        if ( !InvalidCase && !ThreadHandle ) {
            LOG_MSG_BLOCK( BlockMsg );
        } else {
            Success = ( THREAD_PRIORITY_ERROR_RETURN != GetThreadPriority(ThreadHandle) );
            LOG_PASS_FAIL( InvalidCase ? !Success : Success );
        }
    }
    END_VARIATION();

    if ( CleanupHandle ) {
        CloseHandle( ThreadHandle );
    }
}


VOID
GetThreadPriorityTest(
    HANDLE LogHandle
    )
{
    HANDLE Handle;

    xSetFunctionName( LogHandle, "GetThreadPriority" );

    GetThreadPriorityVariation(
        LogHandle,
        "normal thread",
        PrepareThread(ThreadProc, 0, 0),
        "CreateThread failed",
        FALSE,
        TRUE
        );

    GetThreadPriorityVariation(
        LogHandle,
        "current thread",
        GetCurrentThread(),
        "GetCurrentThread failed",
        FALSE,
        FALSE
        );

    GetThreadPriorityVariation(
        LogHandle,
        "suspended thread",
        Handle = PrepareThread( ThreadProc, 0, CREATE_SUSPENDED ),
        "CreateThread failed",
        FALSE,
        FALSE
        );
    ResumeThread( Handle );
    CloseHandle( Handle );

    GetThreadPriorityVariation(
        LogHandle,
        "handle=0xdeadbeef",
        (HANDLE)0xdeadbeef,
        0,
        TRUE,
        FALSE
        );

    GetThreadPriorityVariation(
        LogHandle,
        "handle=NULL",
        NULL,
        0,
        TRUE,
        FALSE
        );

    GetThreadPriorityVariation(
        LogHandle,
        "handle=INVALID_HANDLE_VALUE",
        INVALID_HANDLE_VALUE,
        0,
        TRUE,
        FALSE
        );

    GetThreadPriorityVariation(
        LogHandle,
        "event handle",
        CreateEvent(0, 0, 0, 0),
        "CreateEvent failed",
        TRUE,
        TRUE
        );

    GetThreadPriorityVariation(
        LogHandle,
        "semaphore handle",
        CreateSemaphore(0, 0, 1, 0),
        "CreateSemaphore failed",
        TRUE,
        TRUE
        );

    GetThreadPriorityVariation(
        LogHandle,
        "mutex handle",
        CreateMutex(0, 0, 0),
        "CreateMutex failed",
        TRUE,
        TRUE
        );

    GetThreadPriorityVariation(
        LogHandle,
        "waitable timer handle",
        CreateWaitableTimer(0, 0, 0),
        "CreateWaitableTimer failed",
        TRUE,
        TRUE
        );
}


VOID
SetThreadPriorityVariation(
    HANDLE LogHandle,
    LPSTR  Variation,
    HANDLE ThreadHandle,
    LPSTR  BlockMsg,
    BOOL   InvalidCase,
    BOOL   CleanupHandle
    )
{
    BOOL Success;
    static int ThreadPriorities[] = {
        THREAD_PRIORITY_ABOVE_NORMAL,
        THREAD_PRIORITY_BELOW_NORMAL,
        THREAD_PRIORITY_HIGHEST,
        THREAD_PRIORITY_IDLE,
        THREAD_PRIORITY_LOWEST,
        THREAD_PRIORITY_NORMAL,
        THREAD_PRIORITY_TIME_CRITICAL
    };
    const int ArraySize = sizeof(ThreadPriorities)/sizeof(ThreadPriorities[0]);
    int Priority, OriginalPriority = THREAD_PRIORITY_ERROR_RETURN;
    ULONG Seed = GetTickCount();

    BEGIN_VARIATION( Variation )
    {
        if ( !InvalidCase && !ThreadHandle ) {
            LOG_MSG_BLOCK( BlockMsg );
        } else {
            OriginalPriority = GetThreadPriority( ThreadHandle );
            Priority = ThreadPriorities[ RtlRandom( &Seed ) % ArraySize ];
            Success = SetThreadPriority( ThreadHandle, Priority );

            if ( InvalidCase ) {
                LOG_PASS_FAIL( !Success );
            } else if ( GetThreadPriority( ThreadHandle ) != Priority ) {
                LOG_MSG_FAIL( "mismatch thread priority" );
            } else {
                LOG_PASS();
            }
        }
    }
    END_VARIATION();

    //
    // Restore previous thread's priority
    //

    if ( !InvalidCase && ThreadHandle != NULL ) {
        if ( OriginalPriority == THREAD_PRIORITY_ERROR_RETURN ) {
            OriginalPriority = THREAD_PRIORITY_NORMAL;
        }
        SetThreadPriority( ThreadHandle, OriginalPriority );
    }

    if ( CleanupHandle ) {
        CloseHandle( ThreadHandle );
    }
}


VOID
SetThreadPriorityTest(
    HANDLE LogHandle
    )
{
    HANDLE Handle;

    xSetFunctionName( LogHandle, "SetThreadPriority" );

    SetThreadPriorityVariation(
        LogHandle,
        "normal thread",
        PrepareThread(ThreadProc, 0, 0),
        "CreateThread failed",
        FALSE,
        TRUE
        );

    SetThreadPriorityVariation(
        LogHandle,
        "current thread",
        GetCurrentThread(),
        "GetCurrentThread failed",
        FALSE,
        FALSE
        );

    SetThreadPriorityVariation(
        LogHandle,
        "suspended thread",
        Handle = PrepareThread( ThreadProc, 0, CREATE_SUSPENDED ),
        "CreateThread failed",
        FALSE,
        FALSE
        );
    ResumeThread( Handle );
    CloseHandle( Handle );

    SetThreadPriorityVariation(
        LogHandle,
        "handle=0xdeadbeef",
        (HANDLE)0xdeadbeef,
        0,
        TRUE,
        FALSE
        );

    SetThreadPriorityVariation(
        LogHandle,
        "handle=NULL",
        NULL,
        0,
        TRUE,
        FALSE
        );

    SetThreadPriorityVariation(
        LogHandle,
        "handle=INVALID_HANDLE_VALUE",
        INVALID_HANDLE_VALUE,
        0,
        TRUE,
        FALSE
        );

    SetThreadPriorityVariation(
        LogHandle,
        "event handle",
        CreateEvent(0, 0, 0, 0),
        "CreateEvent failed",
        TRUE,
        TRUE
        );

    SetThreadPriorityVariation(
        LogHandle,
        "semaphore handle",
        CreateSemaphore(0, 0, 1, 0),
        "CreateSemaphore failed",
        TRUE,
        TRUE
        );

    SetThreadPriorityVariation(
        LogHandle,
        "mutex handle",
        CreateMutex(0, 0, 0),
        "CreateMutex failed",
        TRUE,
        TRUE
        );

    SetThreadPriorityVariation(
        LogHandle,
        "waitable timer handle",
        CreateWaitableTimer(0, 0, 0),
        "CreateWaitableTimer failed",
        TRUE,
        TRUE
        );
}


VOID
ThreadPriorityTest(
    HANDLE LogHandle
    )
{
    GetThreadPriorityTest( LogHandle );
    SetThreadPriorityTest( LogHandle );
}
