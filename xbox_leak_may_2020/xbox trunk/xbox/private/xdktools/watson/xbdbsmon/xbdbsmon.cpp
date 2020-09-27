/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbdbsmon.cpp

Abstract:

    This module implements routines which monitor debug message from Xbox.
    The debug messages are captured using XboxDbg notification mechanism.

--*/

#include "xbdbsmonp.h"

PDMN_SESSION DmSession = NULL;
BOOL fNotificationOpened = FALSE;
BOOL fConnectionOpened = FALSE;

VOID
TranslateError(
    HRESULT hr
    )
/*++

Routine Description:

    This routine translates HRESULT returned from XboxDbg APIs into string and
    output to standard error

Auguments:

    hr - HRESULT returned from various XboxDbg APIs

Return Value:

    None

--*/
{
    CHAR Buffer[512];

    DmTranslateErrorA( hr, Buffer, sizeof(Buffer) );
    fprintf( stderr, Buffer );
}

DWORD
WINAPI
NotificationProc(
    ULONG Notification,
    DWORD Parameter
    )
/*++

Routine Description:

    This is a callback routine registered by DmNotify.  It is called by XboxDbg
    for various types of notification event.

Arguments:

    Notification - Type of notification, DM_DEBUGSTR for example

    Parameter - Optional parameter based on notification type

Return Value:

    Always zero

--*/
{
    PDMN_DEBUGSTR p = (PDMN_DEBUGSTR)Parameter;

    //
    // Process only if notification type is DM_DEBUGSTR and there is
    // a valid string length
    //

    if ( Notification == DM_DEBUGSTR && p->Length ) {
        fprintf( stdout, "%.*s", p->Length, p->String );
    }

    return 0;
}

BOOL
WINAPI
ConsoleCtrlHandler(
    DWORD CtrlType
    )
/*++

Routine Description:

    A HandlerRoutine function is an application-defined function used with
    the SetConsoleCtrlHandler function.  A console process uses this function
    to handle control signals received by the process

Arguments:

    CtrlType - Indicates the type of control signal received by the handler.

Return Value:

    Return FALSE to terminate the process

--*/
{
    switch ( CtrlType ) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:

        fprintf( stdout, "Terminating...\n" );

        //
        // Close notification and connection if needed
        //

        if ( fNotificationOpened ) {
            DmCloseNotificationSession( DmSession );
        }

        if ( fConnectionOpened ) {
            DmUseSharedConnection( FALSE );
        }

        return FALSE;
    }

    return TRUE;
}

int __cdecl main( int cArg, char **rgszArg )
{
    char *szExe = *rgszArg;
    HRESULT hr;

    //
    // Hook Ctrl+C handle so that we can do a cleanup work
    //

    SetConsoleCtrlHandler( ConsoleCtrlHandler, TRUE );

    //
    // Open a shared debug connection with xbdm
    //

    hr = DmUseSharedConnection( TRUE );

    if ( FAILED(hr) ) {
        TranslateError( hr );
        return 1;
    } else {
        fConnectionOpened = TRUE;
    }

    //
    // Process command line arguments if any
    //

    ++rgszArg;

    while ( --cArg ) {

        if ( **rgszArg != '-' && **rgszArg != '/' ) {
usage:
            fprintf( stderr, "usage: %s [-x xboxname]\n", szExe );
            DmUseSharedConnection( FALSE );
            return 1;
        }

        char *szArg = *rgszArg++;

        while( *++szArg ) {

            switch( *szArg ) {
            case 'x':
            case 'X':
                if (!--cArg) {
                    goto usage;
                }

                hr = DmSetXboxName( *rgszArg++ );

                if( FAILED(hr) ) {
                    TranslateError( hr );
                    DmUseSharedConnection( FALSE );
                    return 1;
                }
                break;

            default:
                goto usage;
            }
        }
    }

    fprintf( stdout, "Initializing...\n" );
    hr = DmOpenNotificationSession( DM_PERSISTENT, &DmSession );

    if ( SUCCEEDED(hr) ) {

        fNotificationOpened = TRUE;

        hr = DmNotify( DmSession, DM_DEBUGSTR, NotificationProc );

        if ( SUCCEEDED(hr) ) {
            fprintf( stdout, "Start listening (Ctrl+C to exit)...\n" );
            Sleep( INFINITE );
        } else {
            TranslateError( hr );
        }

        DmCloseNotificationSession( DmSession );

    } else {
        TranslateError( hr );
    }

    DmUseSharedConnection( FALSE );

    return 0;
}

