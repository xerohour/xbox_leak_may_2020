/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    runner.c

Abstract:

    This module contains routine that run each test module

--*/

#include "precomp.h"
#include "loader.h"
#include "logging.h"
#include "harnessapi.h"

//
// Counter for running ModuleRunnerThread in serial, stress and parallel test mode
//

ULONG NumberOfRunningModules;

//
// Event to signal main thread that all module runners are terminated
//

HANDLE DoneAllModulesEventHandle;

//
// Sutdown notification event when we want to shutdown the stress/test
//

HANDLE ShutdownEventHandle;

//
// Pointer to the head of test modules list
//

LIST_ENTRY HarnessTestListHead;

BOOL StressTestMode = FALSE;
BOOL RunningInSerialMode = FALSE;

#if DBG
extern ULONG XDebugOutLevel;
extern DWORD g_dwDirectSoundDebugLevel;
extern DWORD g_dwDirectSoundDebugBreakLevel;
#endif

DWORD
WINAPI
TestRunnerThreadProc(
    IN CONST PHARNESS_TEST_LIST_ENTRY p
    )
/*++

Routine Description:

    This is the wrapper routine that will run the StartTest. After StartTest
    returns, if we are the last thread, the end test event will be signaled
    to notify the module runner

Arguments:

    p - Pointer to HARNESS_TEST_LIST_ENTRY structure

Return Value:

    NT status code

--*/
{
    ULONG ExceptionCode = STATUS_SUCCESS;
    HANDLE EventHandles[2];
    DWORD WaitStatus;
    BOOL b;

    if ( DebugVerbose ) {
        DbgPrint( "HARNESS: test runner of %s is starting\n",
            p->FullTestPath );
    }

    //
    // Waits for the start test signal from module runner
    //

    ASSERT( p->StartTestEventHandle != NULL );
    ASSERT( ShutdownEventHandle != NULL );

    EventHandles[0] = ShutdownEventHandle;
    EventHandles[1] = p->StartTestEventHandle;
    WaitStatus = WaitForMultipleObjects( 2, EventHandles, FALSE, INFINITE );

    //
    // The harness wants to shutdown, just set the end event and return
    //

    if ( WaitStatus == WAIT_OBJECT_0 ) {
        goto EndTestRunner;
    }

    if ( WaitStatus != WAIT_OBJECT_0+1 ) {
        HarnessLog(
            XLL_WARN,
            "WaitForMultipleObjects returned 0x%x in test runner of %s (%s)",
            WaitStatus,
            p->FullTestPath,
            WinErrorSymbolicName(GetLastError())
            );
    }

    ASSERT( WaitStatus == WAIT_OBJECT_0+1 );

    try {

        ASSERT( p->LogHandle != INVALID_HANDLE_VALUE );
        ASSERT( p->LogHandle != NULL );
        p->pStartupProc( StressTestMode ? INVALID_HANDLE_VALUE : p->LogHandle );

    } except( HarnessUnhandledExceptionFilter(GetExceptionInformation()) ) {

        ExceptionCode = GetExceptionCode();
        HarnessLog(
            XLL_EXCEPTION,
            "Exception %s is not handled while calling StartTest of %s\n",
            NtStatusSymbolicName(ExceptionCode),
            p->FullTestPath
            );
    }

    if ( DebugVerbose ) {
        DbgPrint( "HARNESS: test runner of %s is done\n", p->FullTestPath
); }

EndTestRunner:

    //
    // Set the end test event if we are the last thread
    //

    ASSERT( p->NumberOfRunningThreads != 0);

    if ( InterlockedDecrement((PULONG)&p->NumberOfRunningThreads) == 0 ) {
        ASSERT( p->EndTestEventHandle != NULL );
        b = SetEvent( p->EndTestEventHandle );
        if ( !b ) {
            HarnessLog(
                XLL_WARN,
                "SetEvent returned FALSE in test runner of %s (%s)",
                p->FullTestPath,
                WinErrorSymbolicName(GetLastError())
                );
        }
        ASSERT( b );
    }

    if ( DebugVerbose ) {
        DbgPrint( "HARNESS: returning from test runner of %s\n",
            p->FullTestPath );
    }

    return ExceptionCode;
}

DWORD
WINAPI
ModuleRunnerThreadProc(
    IN PHARNESS_TEST_LIST_ENTRY p
    )
/*++

Routine Description:

    This is a wrapper routine that will run the entire test module

Arguments:

    p - Pointer to HARNESS_TEST_LIST_ENTRY structure

Return Value:

    NT status code

--*/
{
    ULONG Iterations, Threads;
    ULONG ExceptionCode = STATUS_SUCCESS;
    HANDLE Handles[2], ThreadHandle;
    DWORD WaitStatus;
    BOOL b, DllMainReturnCode = TRUE;
    BOOL NeedToCallEndTest;

    Handles[0] = ShutdownEventHandle;
    Handles[1] = p->EndTestEventHandle;

    do {

        while ( !HarnessLoadTestModule(p, INVALID_SOCKET, TRUE) ) {

            //
            // Sometimes there is not enough memory to re-load the test, if it
            // happens, wait for a while and try again unless we have to shutdown
            //

            if ( WAIT_OBJECT_0 == WaitForSingleObject( ShutdownEventHandle, 1000 ) ) {
                goto EndModuleRunner;
            }
        }

        //
        // Call DllMain of test module if necessary
        //

        if ( p->pDllMain ) {
            ASSERT( p->ModuleHandle );
            DllMainReturnCode = p->pDllMain( p->ModuleHandle, DLL_PROCESS_ATTACH, NULL );
        }

        if ( DllMainReturnCode ) {
            ASSERT( p->Iterations != 0 );
            NeedToCallEndTest = FALSE;

            for ( Iterations=0; Iterations<p->Iterations; Iterations++ ) {

                ASSERT( p->StartTestEventHandle != NULL );
                ASSERT( p->EndTestEventHandle != NULL );
                b = ResetEvent( p->StartTestEventHandle );
                b &= ResetEvent( p->EndTestEventHandle );

                if ( !b ) {
                    HarnessLog(
                        XLL_WARN,
                        "ResetEvent returned FALSE in module runner of %s (%s)",
                        p->FullTestPath,
                        WinErrorSymbolicName(GetLastError())
                        );
                }

                ASSERT( p->NumberOfThreads != 0 );
                ASSERT( p->NumberOfRunningThreads == 0 );

                for ( Threads=0; Threads<p->NumberOfThreads; Threads++ ) {

                    ThreadHandle = CreateThread(
                                        NULL,               // No security attributes
                                        p->ThreadStackSize,
                                        (LPTHREAD_START_ROUTINE)TestRunnerThreadProc,
                                        (LPVOID)p,
                                        0,
                                        NULL
                                        );

                    if ( !ThreadHandle ) {
                        HarnessLog(
                            XLL_WARN,
                            "Unable to create runner thread #%d for %s (%s)",
                            Threads,
                            p->FullTestPath,
                            WinErrorSymbolicName(GetLastError())
                            );
                    } else {
                        InterlockedIncrement( (PLONG)&p->NumberOfRunningThreads );
                        CloseHandle( ThreadHandle );
                    }
                }

                if ( p->NumberOfRunningThreads ) {

                    NeedToCallEndTest = TRUE;

                    //
                    // Signal threads to start to run their test and wait
                    //
                    b = SetEvent( p->StartTestEventHandle );
                    if ( !b ) {
                        HarnessLog(
                            XLL_WARN,
                            "ResetEvent returned FALSE in module runner of %s (%s)",
                            p->FullTestPath,
                            WinErrorSymbolicName(GetLastError())
                            );
                    }

                    WaitStatus = WaitForMultipleObjects( 2, Handles, FALSE, INFINITE );

                    //
                    // If the harness want to shutdown, we must stop now
                    //

                    if ( WaitStatus == WAIT_OBJECT_0 ) {
                        if ( DebugVerbose ) {
                            DbgPrint( "HARNESS: Shutdown event received, terminating\n" );
                        }
                        goto EndModuleRunner;
                    }

                    if ( WaitStatus != WAIT_OBJECT_0+1 ) {
                        HarnessLog(
                            XLL_WARN,
                            "WaitForMultipleObjects returned 0x%x in ModuleRunnerThreadProc (%s)",
                            WaitStatus,
                            WinErrorSymbolicName(GetLastError())
                            );
                    }

                    ASSERT( WaitStatus == WAIT_OBJECT_0+1 );

                    if ( DebugVerbose ) {
                        DbgPrint( "HARNESS: All test runner of %s has terminated\n",
                                  p->FullTestPath );
                    }
                }
            } // Iteration loop

            if ( NeedToCallEndTest ) {

                //
                // The StartTest has finished, call EndTest now
                //
                //

                if ( DebugVerbose ) {
                    DbgPrint( "HARNESS: calling EndTest of %s\n", p->FullTestPath );
                }

                try {
                    p->pCleanupProc();
                } except( HarnessUnhandledExceptionFilter(GetExceptionInformation()) ) {
                    ExceptionCode = GetExceptionCode();
                    HarnessLog(
                        XLL_EXCEPTION,
                        "Exception %s is not handled while calling EndTest of %s\n",
                        NtStatusSymbolicName( ExceptionCode ),
                        p->FullTestPath
                        );
                }
            }

            if ( StressTestMode ) {
                if ( DebugVerbose ) {
                    DbgPrint(
                        "HARNESS: sleep %d msec before running %s again\n",
                        p->SleepBetweenRun,
                        p->FullTestPath );
                }

                if ( WAIT_OBJECT_0 == WaitForSingleObject(
                                        ShutdownEventHandle,
                                        p->SleepBetweenRun ) ) {
                    break;
                }
            }

            if ( p->pDllMain ) {
                ASSERT( p->ModuleHandle );
                p->pDllMain( p->ModuleHandle, DLL_PROCESS_DETACH, NULL );
            }
        } else {
            HarnessLog(
                XLL_WARN,
                "DllMain of %s returned FALSE",
                p->FullTestPath
                );
        }

        HarnessUnloadTestModule( p, StressTestMode == FALSE );

    } while ( StressTestMode );

EndModuleRunner:

    ASSERT( p->NumberOfRunningThreads == 0 );
    ASSERT( NumberOfRunningModules != 0 );

    if ( InterlockedDecrement((PLONG)&NumberOfRunningModules) == 0 ) {
        ASSERT( DoneAllModulesEventHandle != NULL );
        b = SetEvent( DoneAllModulesEventHandle );
        if ( !b ) {
            HarnessLog(
                XLL_WARN,
                "ResetEvent returned FALSE at the end of module runner of %s (%s)",
                p->FullTestPath,
                WinErrorSymbolicName(GetLastError())
                );
        }
        ASSERT( b );
    }

    if ( DebugVerbose ) {
        DbgPrint( "HARNESS: module runner of %s end\n", p->FullTestPath );
    }

    return ExceptionCode;
}

BOOL
RunModule(
    PHARNESS_TEST_LIST_ENTRY p,
    BOOL Wait
    )
{
    DWORD WaitStatus;
    HANDLE Handles[2];

    Handles[1] = CreateThread(
                    NULL,
                    16 * 1024,
                    (LPTHREAD_START_ROUTINE)ModuleRunnerThreadProc,
                    (LPVOID)p,
                    0,
                    NULL
                    );
    if ( !Handles[1] ) {
        HarnessLog(
            XLL_WARN,
            "Unable to create module runner thread for %s (%s)\n",
            p->FullTestPath,
            WinErrorSymbolicName(GetLastError())
            );
    } else {
        InterlockedIncrement( (PLONG)&NumberOfRunningModules );
        if ( Wait ) {
            ASSERT( ShutdownEventHandle != NULL );
            Handles[0] = ShutdownEventHandle;
            WaitStatus = WaitForMultipleObjects( 2, Handles, FALSE, INFINITE );
            if ( WaitStatus != WAIT_OBJECT_0 && WaitStatus != WAIT_OBJECT_0+1 ) {
                HarnessLog(
                    XLL_WARN,
                    "WaitForMultipleObjects returned 0x%x in RunModule (%s)",
                    WaitStatus,
                    WinErrorSymbolicName(GetLastError())
                    );
            }
            ASSERT( WaitStatus == WAIT_OBJECT_0 || WaitStatus == WAIT_OBJECT_0+1 );
        }
        CloseHandle( Handles[1] );
    }

    return Handles[1] != NULL;
}

VOID
RunSerialTest(
    VOID
    )
/*++

Routine Description:

    This routine will run the test module in modules list serially

Arguments:

    None

Return Value:

    None

--*/
{
    PLIST_ENTRY NextEntry;
    PHARNESS_TEST_LIST_ENTRY p;
    HANDLE Handles[2];
    BOOL Running = TRUE;

    ASSERT( RunningInSerialMode == FALSE );
    RunningInSerialMode = TRUE;

    ASSERT( ShutdownEventHandle != NULL );
    Handles[0] = ShutdownEventHandle;
    NextEntry = HarnessTestListHead.Flink;

    while ( Running && NextEntry != &HarnessTestListHead ) {
        p = CONTAINING_RECORD( NextEntry, HARNESS_TEST_LIST_ENTRY, List );
        ASSERT( NumberOfRunningModules == 0 );
        RunModule( p, TRUE );
        NextEntry = NextEntry->Flink;
    }

    if ( DebugVerbose ) {
        DbgPrint( "HARNESS: Serial test end\n\n" );
    }

    RunningInSerialMode = FALSE;
}

VOID
RunParallelTest(
    VOID
    )
/*++

Routine Description:

    This routine will run the test module in modules list in parallel mode

Arguments:

    None

Return Value:

    None

--*/
{
    PLIST_ENTRY NextEntry;
    PHARNESS_TEST_LIST_ENTRY p;
    HANDLE Handles[2];
    ULONG ModulesCount;

    ModulesCount = 0;

    NextEntry = HarnessTestListHead.Flink;
    while ( NextEntry != &HarnessTestListHead ) {
        ModulesCount++;
        NextEntry = NextEntry->Flink;
    }

    if ( DebugVerbose ) {
        DbgPrint( "HARNESS: parallel test found %d module(s)\n", ModulesCount );
    }

    if ( ModulesCount == 0 ) {
        return;
    }

    ASSERT( DoneAllModulesEventHandle != NULL );
    ResetEvent( DoneAllModulesEventHandle );
    Handles[0] = ShutdownEventHandle;
    Handles[1] = DoneAllModulesEventHandle;

    NextEntry = HarnessTestListHead.Flink;

    while ( NextEntry != &HarnessTestListHead ) {
        p = CONTAINING_RECORD( NextEntry, HARNESS_TEST_LIST_ENTRY, List );
        RunModule( p, FALSE );
        NextEntry = NextEntry->Flink;
    }

    if ( StressTestMode ) {

        WaitForSingleObject( ShutdownEventHandle, INFINITE );

    } else {

        DWORD WaitStatus = WaitForMultipleObjects( 2, Handles, FALSE, INFINITE );

        //
        // If the harness want to shutdown, we must stop now
        //

        if ( WaitStatus == WAIT_OBJECT_0 ) {
            goto EndParallelTest;
        }

        if ( WaitStatus != WAIT_OBJECT_0+1 ) {
            HarnessLog(
                XLL_WARN,
                "WaitForMultipleObjects returned 0x%x in RunParallelTest (%s)",
                WaitStatus,
                WinErrorSymbolicName(GetLastError())
                );
        }
        ASSERT( WaitStatus == WAIT_OBJECT_0+1 );
    }

EndParallelTest:

    if ( DebugVerbose ) {
        DbgPrint( "HARNESS: Parallel test end\n" );
    }
}

BOOL
WINAPI
RunTestA(
    SOCKET Socket,
    LPCSTR TestName
    )
{
    BOOL b;
    HARNESS_TEST_LIST_ENTRY ModuleInfo;

    //
    // Currently only work in serial mode
    //
    if ( !RunningInSerialMode ) {
        return FALSE;
    }

    if ( !HarnessInitializeModule(TestName, Socket, &ModuleInfo) ) {
        return FALSE;
    }

    b = RunModule( &ModuleInfo, TRUE );
    HarnessCleanupModule( &ModuleInfo );
    return b;
}

BOOL
WINAPI
RunTestW(
    SOCKET  Socket,
    LPCWSTR TestName
    )
{
    CHAR ModuleName[MAX_PATH];
    wsprintfA( ModuleName, "%ls", TestName );
    return RunTestA( Socket, ModuleName );
}

BOOL
HarnessLoadTestModules(
    VOID
    )
/*++

Routine Description:

    This routine will read TESTINI.INI and enumerate for loadable test
    modules and load them

Arguments:

    None

Return Value:

    TRUE if at least one test module is loaded, otherwise return FALSE

--*/
{
    PSTR psz, Buffer = NULL;
    ULONG AllocationSize = 0;
    ULONG LengthReturned;
    ULONG ModulesCount = 0;
    PLIST_ENTRY NextEntry;
    PHARNESS_TEST_LIST_ENTRY p;

    ShutdownEventHandle = CreateEvent( NULL, TRUE, FALSE, NULL );
    DoneAllModulesEventHandle = CreateEvent( NULL, TRUE, FALSE, NULL );

    if ( !ShutdownEventHandle || !DoneAllModulesEventHandle ) {
        DbgPrint( "HARNESS: Unable to create event (%s)\n", WinErrorSymbolicName(GetLastError()) );
        return FALSE;
    }

    //
    // We don't know how big of the test list, so we try to allocate the buffer
    // starting from 64K characters and try to get the whole section. If the
    // buffer is not big enough we will try to increase the buffer every 50K
    // until we can get all the section in the buffer
    //

    do {

        AllocationSize += ( 64 * 1024 );
        if ( Buffer ) {
            ExFreePool( Buffer );
        }

        //
        // If verifier is turned on with DRIVER_VERIFIER_INJECT_ALLOCATION_FAILURES
        // sometimes ExAllocatePool will return NULL
        //

        do {
            Buffer = ExAllocatePoolWithTag( AllocationSize * sizeof(CHAR), HARNESS_POOLTAG );
        } while ( !Buffer );

        LengthReturned = GetProfileSectionA( "TestList", Buffer, AllocationSize );

    } while ( LengthReturned == AllocationSize - 2 );


    //
    // Enumerate the test list that we retrieved from [TestList] section
    // and create loaded modules list
    //

    InitializeListHead( &HarnessTestListHead );

    psz = Buffer;
    if ( DebugVerbose ) {
        DbgPrint(( "HARNESS: Enumerating [TestList] section from TESTINI.INI...\n" ));
    }

    while ( *psz ) {

        p = (PHARNESS_TEST_LIST_ENTRY)\
            ExAllocatePoolWithTag( sizeof(HARNESS_TEST_LIST_ENTRY), HARNESS_POOLTAG );

        if ( p ) {
            RtlZeroMemory( p, sizeof(HARNESS_TEST_LIST_ENTRY) );
            if ( HarnessInitializeModule(psz, INVALID_SOCKET, p) ) {
                InsertTailList( &HarnessTestListHead, &p->List );
            } else {
                ExFreePool( p );
            }
        } else {
            DbgPrint(( "HARNESS: Unable to allocate buffer for HARNESS_TEST_LIST_ENTRY\n" ));
        }

        psz += strlen( psz );
        psz++;
    }

    ExFreePool( Buffer );

    NextEntry = HarnessTestListHead.Flink;
    while ( NextEntry != &HarnessTestListHead ) {

        ModulesCount++;

        p = CONTAINING_RECORD( NextEntry, HARNESS_TEST_LIST_ENTRY, List );

        if ( DebugVerbose ) {
            DbgPrint( "  FullTestPath = \"%s\"\n", p->FullTestPath );
            DbgPrint( "  FullLogPath = \"%s\"\n", p->FullLogPath );
            DbgPrint( "  LogLevel = %d\n", p->LogLevel );
            DbgPrint( "  LogOptions = %d\n", p->LogOptions );
            DbgPrint( "  Iterations = %d\n", p->Iterations );
            DbgPrint( "  NumberOfThreads = %d\n", p->NumberOfThreads );
            DbgPrint( "  ThreadStackSize = %d\n", p->ThreadStackSize );
            DbgPrint( "  SleepBetweenRun = %d\n", p->SleepBetweenRun );
            DbgPrint( "  Shutdown aware = %s\n", p->pShutdownProc ? "TRUE" : "FALSE" );
            DbgPrint( "\n" );
        }

        NextEntry = NextEntry->Flink;
    }

    if ( !ModulesCount ) {
        DbgPrint(( "HARNESS: WARNING! TestList is empty, there is no test to run\n" ));
    }

    StressTestMode = GetProfileIntA( GeneralSettingsSectionName, "StressTestMode", FALSE );

#if DBG
	g_dwDirectSoundDebugLevel  = GetProfileIntA( "GeneralSettings", "DirectSoundDebugLevel", 1 );
	g_dwDirectSoundDebugBreakLevel  = GetProfileIntA( "GeneralSettings", "DirectSoundDebugBreakLevel", 1 );
#endif // DBG

    if ( StressTestMode ) {

#if DBG
        //
        // Read XDebugOutLevel for stress
        //

        XDebugOutLevel = GetProfileIntA( "GeneralSettings", "XDebugOutLevel",
                                         XDBG_WARNING );

#endif

        RunParallelTest();
    } else {
        if ( GetProfileIntA(GeneralSettingsSectionName, "SerialTestMode", TRUE) ) {
            RunSerialTest();
        }
        if ( GetProfileIntA(GeneralSettingsSectionName, "ParallelTestMode", FALSE) ) {
            RunParallelTest();
        }
    }

    NextEntry = HarnessTestListHead.Flink;

    while ( NextEntry != &HarnessTestListHead ) {
        p = CONTAINING_RECORD( NextEntry, HARNESS_TEST_LIST_ENTRY, List );
        NextEntry = NextEntry->Flink;
        HarnessCleanupModule( p );
        ExFreePool( p );
    }

    CloseHandle( ShutdownEventHandle );
    CloseHandle( DoneAllModulesEventHandle );
    return ModulesCount;
}

BOOL
WINAPI
Shutdown(
    SOCKET socket,
    ULONG  Reserved
    )
{
    return FALSE;
}
