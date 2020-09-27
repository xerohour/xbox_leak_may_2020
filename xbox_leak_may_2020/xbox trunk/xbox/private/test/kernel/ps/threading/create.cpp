/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    create.cpp

Abstract:

    This program contains routines to stress and test threads
    and threading related APIs

--*/

#include "threading.h"

#define MAX_THREADS 10000

DWORD
WINAPI
StackSizeTestThreadProc(
    PTHREAD_PARAMETERS tpm
    )
{
    PVOID pv;

    if ( tpm ) {
        tpm->ExceptionCode = 0;
        __try {
            // Try to allocate space in stack
            pv = _alloca( tpm->StackSize );
        } __except( EXCEPTION_EXECUTE_HANDLER ) {
            tpm->ExceptionCode = GetExceptionCode();
        }
    }

    return 0;
}

DWORD
WINAPI
DoNothingThreadProc(
    DWORD ExitCode
    )
{
    return ExitCode;
}

DWORD
WINAPI
PollThreadProc(
    volatile LONG *ExitFlag
    )
{
    while ( *ExitFlag == 0 ) {
        Sleep( 1000 );
    }

    return InterlockedDecrement( (LPLONG)ExitFlag );
}

VOID
ThreadStackSizeTest(
    HANDLE LogHandle,
    ULONG Size
    )
{
    HANDLE ThreadHandle;
    CHAR Variation[64];
    THREAD_PARAMETERS tpm;

    if ( Size <= 1024*1024 ) {
        sprintf( Variation, "%dK stack size", Size/1024 );
    } else {
        sprintf( Variation, "%dM stack size", Size/(1024*1024) );
    }

    tpm.StackSize = Size;

    BEGIN_VARIATION( Variation )
    {
        ThreadHandle = CreateThread(
                            0,
                            4096 + Size,
                            (LPTHREAD_START_ROUTINE)StackSizeTestThreadProc,
                            (LPVOID)&tpm,
                            0,
                            NULL
                            );

        if ( !ThreadHandle ) {
            LOG_FAIL();
        } else {
            WaitForSingleObject( ThreadHandle, INFINITE );

            if ( tpm.ExceptionCode ) {
                xLog(
                    LogHandle,
                    XLL_EXCEPTION,
                    "BugID 7 - Exception=%s (0x%X)",
                    NtStatusSymbolicName( tpm.ExceptionCode ),
                    tpm.ExceptionCode
                    );
            } else {
                LOG_PASS();
            }

            CloseHandle( ThreadHandle );
        }
    }
    END_VARIATION();
}

VOID
SecurityAttributesTest(
    HANDLE LogHandle,
    LPSTR  Variation,
    LPSECURITY_ATTRIBUTES lpSA
    )
{
    HANDLE ThreadHandle;

    BEGIN_VARIATION( Variation )
    {
        ThreadHandle = CreateThread(
                            lpSA,
                            0,
                            (LPTHREAD_START_ROUTINE)StackSizeTestThreadProc,
                            0,
                            0,
                            NULL
                            );

        if ( !ThreadHandle ) {
            LOG_FAIL();
        } else {
            WaitForSingleObject( ThreadHandle, INFINITE );
            LOG_PASS();
            CloseHandle( ThreadHandle );
        }
    }
    END_VARIATION();
}

VOID
CreateThreadTest(
    HANDLE LogHandle
    )
{
    int i;
    HANDLE ThreadHandle;
    DWORD ExitCode, RealExitCode;
    volatile LONG ExitFlag;
    CHAR Buffer[256];
    ULONG Seed = GetTickCount();

    xSetFunctionName( LogHandle, "CreateThread" );

    ThreadStackSizeTest( LogHandle, 0 );
    ThreadStackSizeTest( LogHandle, 16*1024 );
    ThreadStackSizeTest( LogHandle, 128*1024 );
    ThreadStackSizeTest( LogHandle, 1*1024*1024 );
    ThreadStackSizeTest( LogHandle, 2*1024*1024 );
    ThreadStackSizeTest( LogHandle, 5*1024*1024 );
    ThreadStackSizeTest( LogHandle, 16*1024*1024 );

    SecurityAttributesTest(
        LogHandle,
        "lpsa = 0xdeadbeef",
        (LPSECURITY_ATTRIBUTES)0xdeadbeef
        );

    SecurityAttributesTest(
        LogHandle,
        "lpsa = NULL",
        NULL
        );

    BEGIN_VARIATION( "Create 10000 thread serially" )
    {
        for ( i=0; i<MAX_THREADS; i++ ) {
            ExitCode = RtlRandom( &Seed );
            ThreadHandle = CreateThread(
                                0,
                                16384 + (RtlRandom( &Seed) % 4096),
                                (LPTHREAD_START_ROUTINE)DoNothingThreadProc,
                                (LPVOID)ExitCode,
                                0,
                                NULL
                                );

            if ( !ThreadHandle ) {
                LOG_FAIL();
                break;
            } else {
                WaitForSingleObject( ThreadHandle, INFINITE );
                if ( !GetExitCodeThread( ThreadHandle, &RealExitCode ) ) {
                    CloseHandle( ThreadHandle );
                    LOG_MSG_BLOCK( "GetExitCodeThread failed" );
                    break;
                } else if ( ExitCode != RealExitCode ) {
                    CloseHandle( ThreadHandle );
                    LOG_FAIL();
                    break;
                } else {
                    CloseHandle( ThreadHandle );
                    if ( i == MAX_THREADS-1 ) {
                        LOG_PASS();
                    }
                }
            }
        }
    }
    END_VARIATION();

    if ( !XTestLibIsStressTesting() ) {

        int ThreadPriority = GetThreadPriority( GetCurrentThread() );

        if ( ThreadPriority != THREAD_PRIORITY_ERROR_RETURN ) {
            SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_HIGHEST );
        }

        BEGIN_VARIATION( "Create as many threads as possible" )
        {
            i = 0;
            ExitFlag = 0;

            do {
                ThreadHandle = CreateThread(
                                    0,
                                    16384 + (RtlRandom( &Seed ) % 16384),
                                    (LPTHREAD_START_ROUTINE)PollThreadProc,
                                    (LPVOID)&ExitFlag,
                                    0,
                                    NULL
                                    );

                if ( ThreadHandle ) {
                    i++;
                    SetThreadPriority( ThreadHandle, THREAD_PRIORITY_TIME_CRITICAL );
                    CloseHandle( ThreadHandle );
                }

            } while ( ThreadHandle );

            InterlockedExchange( (LPLONG)&ExitFlag, i );
            NtYieldExecution();

            while ( ExitFlag ) {
                Sleep( 1000 );
            }

            sprintf( Buffer, "%d threads created", i );
            LOG_MSG_PASS( Buffer );
        }
        END_VARIATION();

        if ( ThreadPriority != THREAD_PRIORITY_ERROR_RETURN ) {
            SetThreadPriority( GetCurrentThread(), ThreadPriority );
        }
    }
}
