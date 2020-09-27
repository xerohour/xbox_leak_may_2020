/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    notify.cpp

Abstract:

    This program contains routines to stress and test threads
    and threading related APIs

Environment:

    Xbox

Revision History:

--*/


#include "threading.h"


LONG ThreadingCreateNotificationCounter;
LONG ThreadingDestroyNotificationCounter;


static
DWORD
WINAPI
NotificationTestThreadProc(
    PULONG
    )
{
    //
    // Does nothing, just returns immediately
    //
    return 0;
}


VOID
WINAPI
NotificationTestNotifyProc(
    BOOL fCreate
    )
{
    if ( fCreate ) {
        InterlockedIncrement( &ThreadingCreateNotificationCounter );
    } else {
        InterlockedIncrement( &ThreadingDestroyNotificationCounter );
    }
}


VOID
ThreadNotificationTest(
    HANDLE LogHandle
    )
{
    HANDLE ThreadHandle = NULL;
    XTHREAD_NOTIFICATION xtn;

    xSetFunctionName( LogHandle, "XRegisterThreadNotifyRoutine" );

    //
    // Reset counter so it will run correctly in stress mode
    //
    InterlockedExchange( &ThreadingCreateNotificationCounter, 0 );
    InterlockedExchange( &ThreadingDestroyNotificationCounter, 0 );

    xtn.pfnNotifyRoutine = (XTHREAD_NOTIFY_PROC)NotificationTestNotifyProc;
    XRegisterThreadNotifyRoutine( &xtn, TRUE );

    BEGIN_VARIATION( "create (registered)" )
    {
        ThreadHandle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)NotificationTestThreadProc, NULL, 0, 0 );

        if ( !ThreadHandle ) {
            LOG_MSG_BLOCK( "unable to create thread" );
        } else {
            WaitForSingleObject( ThreadHandle, INFINITE );
            LOG_PASS_FAIL( ThreadingCreateNotificationCounter );
        }
    }
    END_VARIATION();

    BEGIN_VARIATION( "destroy (registered)" )
    {
        if ( ThreadHandle ) {
            LOG_PASS_FAIL( ThreadingDestroyNotificationCounter );
        } else {
            LOG_MSG_BLOCK( "unable to create thread" );
        }
    }
    END_VARIATION();

    XRegisterThreadNotifyRoutine( &xtn, FALSE );

    if ( ThreadHandle ) {
        CloseHandle( ThreadHandle );
        ThreadHandle = NULL;
    }

    //
    // Reset counter back to zero
    //
    InterlockedExchange( &ThreadingCreateNotificationCounter, 0 );
    InterlockedExchange( &ThreadingDestroyNotificationCounter, 0 );

    BEGIN_VARIATION( "create (unregistered)" )
    {
        ThreadHandle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)NotificationTestThreadProc, NULL, 0, 0 );

        if ( !ThreadHandle ) {
            LOG_MSG_BLOCK( "unable to create thread" );
        } else {
            //
            // Counter should be zero because we already de-registered
            //
            WaitForSingleObject( ThreadHandle, INFINITE );
            LOG_PASS_FAIL( ThreadingCreateNotificationCounter == 0 );
        }
    }
    END_VARIATION();

    BEGIN_VARIATION( "destroy (unregistered)" )
    {
        if ( ThreadHandle ) {
            //
            // Counter should be zero because we already de-registered
            //
            LOG_PASS_FAIL( ThreadingDestroyNotificationCounter == 0);
        } else {
            LOG_MSG_BLOCK( "unable to create thread" );
        }
    }
    END_VARIATION();

    if ( ThreadHandle ) {
        CloseHandle( ThreadHandle );
    }
}
