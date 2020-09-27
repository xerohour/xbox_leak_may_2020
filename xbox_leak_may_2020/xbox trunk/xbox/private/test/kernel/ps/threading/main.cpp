/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    main.cpp

Abstract:

    This program contains routines to stress and test threads
    and threading related APIs

--*/

#include "threading.h"

VOID
WINAPI
ThreadingStartTest(
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
    xSetComponent( LogHandle, "Kernel", "Ps" );
    CreateThreadTest( LogHandle );
    ExitThreadTest( LogHandle );
    ThreadIDTest( LogHandle );
    ThreadPriorityTest( LogHandle );
    SuspendResumeTest( LogHandle );
    ThreadNotificationTest( LogHandle );
    ThreadLocalStorageTest( LogHandle );
    FiberTest( LogHandle );
}

VOID
WINAPI
ThreadingEndTest(
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
DECLARE_EXPORT_DIRECTORY( threading )
#pragma data_seg()

BEGIN_EXPORT_TABLE( threading )
    EXPORT_TABLE_ENTRY( "StartTest", ThreadingStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", ThreadingEndTest )
END_EXPORT_TABLE( threading )
