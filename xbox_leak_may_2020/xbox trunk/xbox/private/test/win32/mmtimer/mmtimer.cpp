/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    mmtimer.cpp

Abstract:

    This program contains routines to test the following multimedia timer APIs

        timeGetSystemTime
        timeGetTime
        timeKillEvent
        timeSetEvent

Environment:

    Xbox

Revision History:

--*/


#include <precomp.h>


#define MMTIMER_TEST_DELAY1     250
#define MMTIMER_TEST_DELAY2     350
#define MAX_TIMER_TO_BE_TESTED  0x10000


VOID
MmTimerGetTimeTest(
    HANDLE LogHandle
    )
{
    DWORD time;
    MMTIME mmt;
    MMRESULT mmr;

    xSetFunctionName( LogHandle, "timeGetTime" );
    
    BEGIN_VARIATION( "basic" )
    {
        time = timeGetTime();
        LOG_PASS_FAIL( time != 0 );
    }
    END_VARIATION();

    xSetFunctionName( LogHandle, "timeGetSystemTime" );

    BEGIN_VARIATION( "basic" )
    {
        mmr = timeGetSystemTime( &mmt, sizeof(mmt) );
        LOG_PASS_FAIL( mmr == TIMERR_NOERROR );

        if ( mmr == TIMERR_NOERROR ) {
            xLog(
                LogHandle,
                XLL_INFO,
                "wType=%lu,u.ms=%lu",
                mmt.wType,
                mmt.u.ms
                );
        }
    }
    END_VARIATION();
}


VOID
CALLBACK
MmTimerCallbackProc(
    UINT uId,
    UINT uMsg,
    PLONG Counter,
    DWORD dw1,
    DWORD dw2
    )
{
    UNREFERENCED_PARAMETER( uId );
    UNREFERENCED_PARAMETER( uMsg );
    UNREFERENCED_PARAMETER( dw1 );
    UNREFERENCED_PARAMETER( dw2 );
    if ( Counter ) {
        InterlockedIncrement( Counter );
    }
}


VOID
MmTimerEventTest(
    HANDLE LogHandle
    )
{
    MMRESULT mmr;
    LONG Counter;
    HANDLE EventHandle;
    DWORD Wait;

    xSetFunctionName( LogHandle, "timeSetEvent" );

    BEGIN_VARIATION( "one-shot callback" )
    {
        Counter = 0;
        mmr = timeSetEvent(
                MMTIMER_TEST_DELAY1,
                0,
                (LPTIMECALLBACK)MmTimerCallbackProc,
                (DWORD)&Counter,
                TIME_ONESHOT | TIME_CALLBACK_FUNCTION
                );
        if ( mmr == NULL ) {
            LOG_MSG_FAIL( "timeSetEvent failed" );
        } else {
            Sleep( MMTIMER_TEST_DELAY1 * 5 );
            LOG_PASS_FAIL( Counter == 1 );
        }
    }
    END_VARIATION();

    BEGIN_VARIATION( "periodic callback" )
    {
        Counter = 0;
        mmr = timeSetEvent(
                MMTIMER_TEST_DELAY1,
                0,
                (LPTIMECALLBACK)MmTimerCallbackProc,
                (DWORD)&Counter,
                TIME_PERIODIC | TIME_CALLBACK_FUNCTION
                );
        if ( mmr == NULL ) {
            LOG_MSG_FAIL( "timeSetEvent failed" );
        } else {
            Sleep( MMTIMER_TEST_DELAY1 * 5 );
            LOG_PASS_FAIL( Counter >= 1 );
            timeKillEvent( mmr );
        }
    }
    END_VARIATION();

    EventHandle = CreateEvent( 0, TRUE, FALSE, 0 );
    if ( !EventHandle ) {
        LOG_MSG_BLOCK( "Couldn't create event" );
        return;
    }

    BEGIN_VARIATION( "one-shot event" )
    {
        mmr = timeSetEvent(
                MMTIMER_TEST_DELAY1,
                0,
                (LPTIMECALLBACK)EventHandle,
                (DWORD)&Counter,
                TIME_ONESHOT | TIME_CALLBACK_EVENT_SET
                );
        if ( mmr == NULL ) {
            LOG_MSG_FAIL( "timeSetEvent failed" );
        } else {
            Wait = WaitForSingleObject( EventHandle, MMTIMER_TEST_DELAY1 * 2 );
            switch ( Wait ) {
            case WAIT_OBJECT_0:
                LOG_PASS();
                break;
            default:
                LOG_FAIL();
            }
        }
    }
    END_VARIATION();

    BEGIN_VARIATION( "periodic event" )
    {
        Counter = 0;
        mmr = timeSetEvent(
                MMTIMER_TEST_DELAY1,
                0,
                (LPTIMECALLBACK)EventHandle,
                (DWORD)&Counter,
                TIME_PERIODIC | TIME_CALLBACK_EVENT_SET
                );
        if ( mmr == NULL ) {
            LOG_MSG_FAIL( "timeSetEvent failed" );
        } else {
            for (;;) {
                Wait = WaitForSingleObject( EventHandle, MMTIMER_TEST_DELAY1 * 2 );
                if ( Wait == WAIT_OBJECT_0 ) {
                    if ( InterlockedIncrement(&Counter) > 5 ) {
                        break;
                    }
                } else {
                    break;
                }
            }
            timeKillEvent( mmr );
            LOG_PASS_FAIL( Counter > 1 );
        }
    }
    END_VARIATION();

    CloseHandle( EventHandle );

    BEGIN_VARIATION( "stress" )
    {
        MMRESULT * TimerIDs = (MMRESULT*)HeapAlloc(
                                            GetProcessHeap(),
                                            HEAP_ZERO_MEMORY,
                                            sizeof(MMRESULT) * MAX_TIMER_TO_BE_TESTED
                                            );
        if ( !TimerIDs ) {
            LOG_MSG_BLOCK( "cound't allocate memory" );
        } else {
            __try {
                for ( Counter=0; Counter<MAX_TIMER_TO_BE_TESTED; Counter++ ) {
                    mmr = timeSetEvent(
                            MMTIMER_TEST_DELAY2,
                            0,
                            (LPTIMECALLBACK)MmTimerCallbackProc,
                            (DWORD)NULL,
                            TIME_PERIODIC | TIME_CALLBACK_FUNCTION
                            );
                    TimerIDs[Counter] = mmr;
                }
                for ( Counter=0; Counter<MAX_TIMER_TO_BE_TESTED; Counter++ ) {
                    if ( TimerIDs[Counter] ) {
                        timeKillEvent( TimerIDs[Counter] );
                    }
                }
                LOG_PASS();
            } __finally {
                HeapFree( GetProcessHeap(), 0, TimerIDs );
            }
        }
    }
    END_VARIATION();

    xSetFunctionName( LogHandle, "timeKillEvent" );

    BEGIN_VARIATION( "Valid ID" )
    {
        mmr = timeSetEvent(
                MMTIMER_TEST_DELAY1,
                0,
                (LPTIMECALLBACK)MmTimerCallbackProc,
                (DWORD)NULL,
                TIME_ONESHOT | TIME_CALLBACK_FUNCTION
                );
        if ( mmr == NULL) {
            LOG_MSG_BLOCK( "timeSetEvent failed" );
        } else {
            LOG_PASS_FAIL( timeKillEvent(mmr) == TIMERR_NOERROR );
        }
    }
    END_VARIATION();

    BEGIN_VARIATION( "Invalid ID" )
    {
        LOG_PASS_FAIL( timeKillEvent(~0UL) != TIMERR_NOERROR );
    }
    END_VARIATION();
}


VOID
WINAPI
MmTimerStartTest(
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
    KdPrint(( "MMTIMER: StartTest\n" ));
    xSetComponent( LogHandle, "Win32", "mmtimer" );

    MmTimerGetTimeTest( LogHandle );
    MmTimerEventTest( LogHandle );
}


VOID
WINAPI
MmTimerEndTest(
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
    KdPrint(( "MMTIMER: EndTest\n" ));
}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( mmtimer )
#pragma data_seg()

BEGIN_EXPORT_TABLE( mmtimer )
    EXPORT_TABLE_ENTRY( "StartTest", MmTimerStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MmTimerEndTest )
END_EXPORT_TABLE( mmtimer )

