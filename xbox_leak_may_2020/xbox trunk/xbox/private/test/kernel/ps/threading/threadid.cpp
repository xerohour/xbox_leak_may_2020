/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    threadid.cpp

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


DWORD
WINAPI
ThreadIDTestThreadProc(
    PTHREAD_PARAMETERS tpm
    )
{
    tpm->ExpectedThreadID = GetCurrentThreadId();
    return 0;
}


VOID
ThreadIDVariation(
    HANDLE LogHandle,
    LPSTR  Variation
    )
{
    HANDLE ThreadHandle;
    THREAD_PARAMETERS tpm;

    BEGIN_VARIATION( Variation )
    {
        ThreadHandle = CreateThread(
                            0,
                            0,
                            (LPTHREAD_START_ROUTINE)ThreadIDTestThreadProc,
                            (LPVOID)&tpm,
                            0,
                            &tpm.ThreadID
                            );

        if ( !ThreadHandle ) {
            LOG_MSG_BLOCK( "CreateThread failed" );
        } else {
            WaitForSingleObject( ThreadHandle, INFINITE );

            if ( tpm.ExpectedThreadID == tpm.ThreadID ) {
                LOG_PASS();
            } else {
                LOG_MSG_FAIL( "mismatch thread id" );
            }

            CloseHandle( ThreadHandle );
        }
    }
    END_VARIATION();
}


VOID
ThreadIDTest(
    HANDLE LogHandle
    )
{
    xSetFunctionName( LogHandle, "GetCurrenThreadId" );
    ThreadIDVariation( LogHandle, "GetCurrentThreadId" );
}
