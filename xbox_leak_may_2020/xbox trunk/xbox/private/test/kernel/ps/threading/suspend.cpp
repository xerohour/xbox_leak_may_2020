/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    suspend.cpp

Abstract:

    This program contains routines to stress and test threads
    and threading related APIs

--*/

#include "threading.h"

VOID
SuspendResumeThreadVariation(
    HANDLE LogHandle,
    LPSTR  Variation,
    HANDLE ThreadHandle,
    LPSTR  BlockMsg,
    BOOL   InvalidCase,
    BOOL   CleanupHandle
    )
{
    DWORD Success;

    BEGIN_VARIATION( Variation )
    {
        if ( CleanupHandle && !ThreadHandle ) {
            LOG_MSG_BLOCK( BlockMsg );
        } else {
            Success = SuspendThread( ThreadHandle );

            if ( InvalidCase ) {
                LOG_PASS_FAIL( Success == ~0UL );
            } else {
                if ( Success == ~0UL ) {
                    LOG_MSG_FAIL( "SuspendThread failed" );
                } else {
                    Success = ResumeThread( ThreadHandle );
                    if ( Success == 0UL ) {
                        LOG_MSG_FAIL( "ResumeThread failed" );
                    } else {
                        LOG_PASS();
                    }
                }
            }
        }
    }
    END_VARIATION();

    if ( CleanupHandle && ThreadHandle && ThreadHandle != INVALID_HANDLE_VALUE ) {
        CloseHandle( ThreadHandle );
    }
}

VOID
SuspendResumeTest(
    HANDLE LogHandle
    )
{
    xSetFunctionName( LogHandle, "SuspendThread/ResumeThread" );

    SuspendResumeThreadVariation(
        LogHandle,
        "handle=0xdeadbeef",
        (HANDLE)0xdeadbeef,
        0,
        TRUE,
        FALSE
        );

    SuspendResumeThreadVariation(
        LogHandle,
        "handle=NULL",
        NULL,
        0,
        TRUE,
        FALSE
        );

    SuspendResumeThreadVariation(
        LogHandle,
        "handle=INVALID_HANDLE_VALUE",
        INVALID_HANDLE_VALUE,
        0,
        TRUE,
        FALSE
        );

    SuspendResumeThreadVariation(
        LogHandle,
        "handle=event handle",
        CreateEvent(0, 0, 0, 0),
        "CreateEvent failed",
        TRUE,
        TRUE
        );

    SuspendResumeThreadVariation(
        LogHandle,
        "handle=semaphore handle",
        CreateSemaphore(0, 0, 1, 0),
        "CreateSemaphore failed",
        TRUE,
        TRUE
        );

    SuspendResumeThreadVariation(
        LogHandle,
        "handle=mutex handle",
        CreateMutex(0, 0, 0),
        "CreateMutex failed",
        TRUE,
        TRUE
        );

    SuspendResumeThreadVariation(
        LogHandle,
        "handle=waitable timer handle",
        CreateWaitableTimer(0, 0, 0),
        "CreateWaitableTimer failed",
        TRUE,
        TRUE
        );
}
