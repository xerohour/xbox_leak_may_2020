/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    memperf.cpp

Abstract:

    This module contains routines to measure memory performace under Xbox

Environment:

    Xbox

Revision History:

--*/


extern "C" {
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
}

#if XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif

#include <xlog.h>
#include <xtestlib.h>
#include <stdio.h>
#include <testutil.h>
#include <tsc.h>


#define NUMBER_OF_LOOP  1500
#define MEGABYTE   (1024*1024)


VOID
MemoryBandwidthTest(
    HANDLE LogHandle,
    LPSTR  FunctionName,
    DWORD  AdditionalProtecionFlags,
    BOOL   BlockInterrupt,
    DWORD  numBytes
    )
{
    ULONG i;

    xSetFunctionName( LogHandle, FunctionName );

    BEGIN_VARIATION( "Memory Bandwidth" )
    {
        PBYTE Destination = NULL;
        PBYTE Source = NULL;
        INT64 BestTime = 0x7FFFFFFFFFFFFFFFI64;

        #ifndef _UNDER_NT
        if ( XTestLibIsStressTesting() ) {
            LOG_MSG_BLOCK( "This test only runs in serial mode" );
            goto EndVariation;
        }
        #else
        UNREFERENCED_PARAMETER( BlockInterrupt );
        #endif

        Source = (PBYTE)VirtualAlloc(
                                0,
                                numBytes,
                                MEM_COMMIT,
                                PAGE_READWRITE | AdditionalProtecionFlags
                                );
        if ( !Source ) {
            LOG_MSG_BLOCK( "not enough memory" );
            goto EndVariation;
        }

        Destination = (PBYTE)VirtualAlloc(
                                0,
                                numBytes,
                                MEM_COMMIT,
                                PAGE_READWRITE | AdditionalProtecionFlags
                                );
        if ( !Destination ) {
            LOG_MSG_BLOCK( "not enough memory" );
            VirtualFree( Source, 0, MEM_RELEASE );
            goto EndVariation;
        }

        if ( Destination ) {

            CHAR Buffer[256];
            int ThreadPriority = GetThreadPriority( GetCurrentThread() );
            if ( ThreadPriority != THREAD_PRIORITY_ERROR_RETURN ) {
                SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );
            }

            for ( i=0; i<NUMBER_OF_LOOP; i++ ) {
#ifndef _UNDER_NT
                if ( BlockInterrupt ) {
                    __asm cli;
                }
#endif
                INT64 StartTime = TscBegin();
                memcpy( Destination, Source, numBytes );
                INT64 TotalTime = TscEnd( StartTime );

#ifndef _UNDER_NT
                if ( BlockInterrupt ) {
                    __asm sti
                }
#endif
                if ( TotalTime < BestTime ) {
                    BestTime = TotalTime;
                }
            }

            if ( ThreadPriority != THREAD_PRIORITY_ERROR_RETURN ) {
                SetThreadPriority( GetCurrentThread(), ThreadPriority );
            }

            sprintf(
                Buffer,
                "MEMPERF: %f MB/sec",
                (float)(numBytes/TscTicksToFUSeconds(BestTime))
                );

            KdPrint(( "%s\n", Buffer ));
            LOG_MSG_PASS( Buffer );

            VirtualFree( Source, 0, MEM_RELEASE );
            VirtualFree( Destination, 0, MEM_RELEASE );
        }

EndVariation:
        NOTHING;
    }
    END_VARIATION();
}


VOID
WINAPI
MemPerfStartTest(
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
    KdPrint(( "MEMPERF: StartTest\n" ));
    xSetComponent( LogHandle, "Mm", "Perf" );

    TscInit( 0 );
    MemoryBandwidthTest( LogHandle, "Cache (1 meg)", 0, FALSE, MEGABYTE );
    MemoryBandwidthTest( LogHandle, "No Cache (1 meg)", PAGE_NOCACHE, FALSE, MEGABYTE );
    MemoryBandwidthTest( LogHandle, "No Cache (4 meg)", PAGE_NOCACHE, FALSE, 4*MEGABYTE );
#ifndef _UNDER_NT
    MemoryBandwidthTest( LogHandle, "Cache (1 meg) (block interrupt)", 0, TRUE, MEGABYTE );
    MemoryBandwidthTest( LogHandle, "No Cache (1 meg) (block interrupt)", PAGE_NOCACHE, TRUE, MEGABYTE );
#endif
}


VOID
WINAPI
MemPerfEndTest(
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
    KdPrint(( "MEMPERF: EndTest\n" ));
}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( memperf )
#pragma data_seg()

BEGIN_EXPORT_TABLE( memperf )
    EXPORT_TABLE_ENTRY( "StartTest", MemPerfStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MemPerfEndTest )
END_EXPORT_TABLE( memperf )
