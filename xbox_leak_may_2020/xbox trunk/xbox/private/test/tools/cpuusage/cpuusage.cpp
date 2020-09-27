/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    couusage.cpp

Abstract:

    This program contains routines to calculate CPU utilization

Environment:

    Xbox

Revision History:

--*/


#include <precomp.h>


VOID
WINAPI
CpuUsageStartTest(
    HANDLE /* LogHandle */
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
    static LARGE_INTEGER PrevTotalTime = { 0, 0 };
    static LARGE_INTEGER PrevIdleTime = { 0, 0 };
    static LARGE_INTEGER PrevKernelTime = { 0, 0 };

    LARGE_INTEGER KernelTime;
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER TotalTime;

    LARGE_INTEGER DeltaTotalTime;
    LARGE_INTEGER DeltaIdleTime;
    LARGE_INTEGER DeltaKernelTime;

    BYTE CpuUsage;

    // Ignore Interrupt, DPC and User time
    QueryCpuTimesInformation( &KernelTime, &IdleTime, 0, 0, 0 );

    TotalTime.QuadPart = KernelTime.QuadPart + IdleTime.QuadPart;

    DeltaIdleTime.QuadPart = IdleTime.QuadPart - PrevIdleTime.QuadPart;
    DeltaKernelTime.QuadPart = KernelTime.QuadPart - PrevKernelTime.QuadPart;
    DeltaTotalTime.QuadPart = TotalTime.QuadPart - PrevTotalTime.QuadPart;

    PrevTotalTime.QuadPart = TotalTime.QuadPart;
    PrevIdleTime.QuadPart = IdleTime.QuadPart;
    PrevKernelTime.QuadPart = KernelTime.QuadPart;

    if ( DeltaTotalTime.QuadPart == 0 ) {
        CpuUsage = 0;
    } else {
        CpuUsage = (BYTE)( 100 - ( (DeltaIdleTime.QuadPart * 100)/DeltaTotalTime.QuadPart) );
    }

    KdPrint(( "\rCPUUSAGE: %u%%\r", CpuUsage ));
}


VOID
WINAPI
CpuUsageEndTest(
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
}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( cpuusage )
#pragma data_seg()

BEGIN_EXPORT_TABLE( cpuusage )
    EXPORT_TABLE_ENTRY( "StartTest", CpuUsageStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", CpuUsageEndTest )
END_EXPORT_TABLE( couusage )
