/*++

Copyright (c) 1997-2000  Microsoft Corporation

Module Name:

    main.cpp

Abstract:

    This program contains routines to stress object manager

Author:

    Silviu Calinoiu (silviuc) 18-Mar-1997

Environment:

    Xbox

Revision History:

    04-May-2000     schanbai

        Ported to Xbox

--*/


#include "pch.h"
#include "utils.h"
#include "stress.h"
#include "xlog.h"
#include "xtestlib.h"


HANDLE ObStressHeapHandle;


BOOL
AllocateMemory(
    VOID
    )
{
    ULONG i;

    for ( i=0; i<THREADS_NUMBER; i++ ) {
        aHandles[i] = (HANDLE *)\
            HeapAlloc( ObStressHeapHandle, HEAP_ZERO_MEMORY, sizeof(HANDLE) * MAX_OBJECTS_PER_THREAD );

        if ( !aHandles[i] ) {
            return FALSE;
        }
    }

    return TRUE;
}


VOID
FreeMemory(
    VOID
    )
{
    ULONG i;

    for ( i=0; i<THREADS_NUMBER; i++ ) {
        if ( aHandles[i] ) {
            HeapFree( ObStressHeapHandle, HEAP_ZERO_MEMORY, aHandles[i] );
        }
    }
}


BOOL
WINAPI
ObStressDllMain(
    HINSTANCE   hInstance,
    DWORD       fdwReason,
    LPVOID      lpReserved
    )
{
    UNREFERENCED_PARAMETER( lpReserved );

    if ( fdwReason == DLL_PROCESS_ATTACH ) {
        ObStressHeapHandle = HeapCreate( 0, 0, 0 );
    } else if ( fdwReason == DLL_PROCESS_DETACH ) {
        ASSERT( ObStressHeapHandle );
        HeapDestroy( ObStressHeapHandle );
    }

    return ObStressHeapHandle != NULL;
}


VOID
WINAPI
ObStressStartTest(
    HANDLE LogHandle
    )

/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/

{
    HANDLE ahThreads[ THREADS_NUMBER ];
    UINT uThreadIndex;
    UINT uStressThreadsNo;
    UINT uCleanupThreadsNo;
    UINT uCrtHandle;
    DWORD dwThreadId;
    DWORD dwWaitResult;
    DWORD dwSleepTime;
    BOOL StressMode;
    ULONG LastTickCount = 0;
    ULONG Seed = GetTickCount();

    if ( AllocateMemory() ) {

        xSetComponent( LogHandle, "Kernel", "Ob" );
        xSetFunctionName( LogHandle, "stress" );
        xStartVariation( LogHandle, "stress" );

        KdPrint(( "OBSTRESS: StartTest\n" ));

        ASSERT( THREADS_NUMBER <= MAXIMUM_WAIT_OBJECTS );

        StressMode = XTestLibIsStressTesting();
        KdPrint(( "OBSTRESS: running in stress mode = %d\n", StressMode ));

        LastTickCount = GetTickCount();

        //
        // loop forever if in stress mode
        //

        for (;;) {
            PS_STATISTICS ProcessStatistics;
            ULONG HandleCount;

            ProcessStatistics.Length = sizeof(PS_STATISTICS);
            PsQueryStatistics(&ProcessStatistics);
            HandleCount = ProcessStatistics.HandleCount;

            DbgMessage( "\nStarting new execution cycle, handle count = %ld\n", HandleCount );

            //
            // launch the stress threads
            //

            for ( uThreadIndex = 0; uThreadIndex < THREADS_NUMBER; uThreadIndex++ ) {

                //
                // no handles yet
                //

                auHandlesNo[ uThreadIndex ] = 0;

                //
                // create a new stress thread
                //

                ahThreads[ uThreadIndex ] = CreateThread(
                                                NULL,
                                                0,
                                                StressHandleTableThread,
                                                UlongToPtr( uThreadIndex ),
                                                0,
                                                &dwThreadId
                                                );

                if ( ahThreads[ uThreadIndex ] == NULL ) {
                    DbgMessage( "Cannot create thread %u, error %u\n", uThreadIndex );
                    break;
                }
            }
        
            uStressThreadsNo = uThreadIndex;

            if ( uStressThreadsNo == 0 ) {
                DbgMessage( "Could not create any thread.\n" );
            } else {
                DbgMessage( "Number of stress threads launched %u\n", uStressThreadsNo );

                //
                // wait for the stress threads to end
                //

                dwWaitResult = WaitForMultipleObjects(
                                    uStressThreadsNo,
                                    ahThreads,
                                    TRUE,
                                    INFINITE
                                    );

                ASSERT( ( dwWaitResult >= WAIT_OBJECT_0 ) && 
                        ( dwWaitResult <= ( WAIT_OBJECT_0 + uStressThreadsNo ) ) );

                //
                // close the thread handles
                //

                for ( uThreadIndex = 0; uThreadIndex < uStressThreadsNo; uThreadIndex++ ) {
                    CloseHandle( ahThreads[ uThreadIndex ] );
                }

                //
                // launch the clean-up threads
                //

                DbgMessage( "Starting clean-up threads\n" );

                for ( uThreadIndex = 0; uThreadIndex < uStressThreadsNo; uThreadIndex++ ) {
                    ahThreads[ uThreadIndex ] = CreateThread( 
                                                    NULL,
                                                    0,
                                                    CleanHandleTableThread,
                                                    UlongToPtr( uThreadIndex ),
                                                    0,
                                                    &dwThreadId
                                                    );

                    if ( ahThreads[ uThreadIndex ] == NULL ) {
                        DbgMessage( "Cannot create clean-up thread %u, error %u\n", uThreadIndex );
                        break;
                    }
                }

                uCleanupThreadsNo = uThreadIndex;
                DbgMessage( "Number of cleanup threads launched %u\n", uCleanupThreadsNo );

                //
                // wait for the clean-up threads to end
                //

                if ( uCleanupThreadsNo > 0 ) {
                    dwWaitResult = WaitForMultipleObjects(
                                        uCleanupThreadsNo,
                                        ahThreads,
                                        TRUE,
                                        INFINITE
                                        );


                    ASSERT( ( dwWaitResult >= WAIT_OBJECT_0 ) && 
                            ( dwWaitResult <= ( WAIT_OBJECT_0 + uCleanupThreadsNo ) ) );

                    //
                    // close the thread handles
                    //

                    for ( uThreadIndex = 0; uThreadIndex < uCleanupThreadsNo; uThreadIndex++ ) {
                        CloseHandle( ahThreads[ uThreadIndex ] );
                    }
                }

                //
                // if we couldn't create enough cleanup threads then clean-up the rest of the handles here
                //

                for ( uThreadIndex = uCleanupThreadsNo; uThreadIndex < uStressThreadsNo; uThreadIndex++ ) {
                    for( uCrtHandle = 0; uCrtHandle < auHandlesNo[ uThreadIndex ]; uCrtHandle++ ) {
                        CloseHandle( aHandles[ uThreadIndex ][ uCrtHandle ] );
                    }
                }
            }

            //
            // sleep for some time
            //

            dwSleepTime = abs(RtlRandom(&Seed)) % 5000;

            DbgMessage( "Main thread - sleeping...\n" );

            Sleep( dwSleepTime );

            ProcessStatistics.Length = sizeof(PS_STATISTICS);
            PsQueryStatistics(&ProcessStatistics);

            if ( HandleCount != ProcessStatistics.HandleCount ) {
                DbgMessage(
                    "Warning: handle leak, previous handle count is %ld, now is %ld\n",
                    HandleCount,
                    ProcessStatistics.HandleCount
                    );
            }

            if ( !StressMode && (GetTickCount()-LastTickCount) > 60000 ) {
                break;
            }
        }

        xEndVariation( LogHandle );
    }

    FreeMemory();        
}


VOID
WINAPI
ObStressEndTest(
    VOID
    )

/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/

{
    KdPrint(( "OBSTRESS: EndTest\n" ));
}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( obstress )
#pragma data_seg()

BEGIN_EXPORT_TABLE( obstress )
    EXPORT_TABLE_ENTRY( "StartTest", ObStressStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", ObStressEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", ObStressDllMain )
END_EXPORT_TABLE( obstress )
