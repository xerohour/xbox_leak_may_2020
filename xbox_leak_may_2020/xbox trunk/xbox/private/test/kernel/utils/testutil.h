/*++

Copyright (c) 1997-2000  Microsoft Corporation

Module Name:

    testutil.h

Abstract:

    This module contains common testing routine and xLog wrapper
    under NT environment

Author:

    Sakphong Chanbai (schanbai) 12-May-2000

Environment:

    Xbox

Revision History:

--*/


#ifndef _A8FD5CE7_2528_4795_90B4_4845082EDC3F_
#define _A8FD5CE7_2528_4795_90B4_4845082EDC3F_


#include <xlog.h>
#include <stdarg.h>


#ifdef _UNDER_NT

__inline
BOOL
xStartVariation(
    HANDLE LogHandle,
    PSTR   lpszVariationName
    )
{
    UNREFERENCED_PARAMETER( LogHandle );
    printf( "Variation: %s\n", lpszVariationName );
    return TRUE;
}

__inline
BOOL
xLog(
    IN HANDLE LogHandle,
    IN DWORD  dwLevel,
    IN LPSTR  lpFormat,
    ...
    )
{
    va_list argptr;

    printf( "Log: " );
    va_start( argptr, lpFormat );
    vprintf( lpFormat, argptr );
    va_end( argptr );
    printf( "\n" );

    UNREFERENCED_PARAMETER( LogHandle );
    UNREFERENCED_PARAMETER( dwLevel );

    return TRUE;
}

__inline
DWORD
xEndVariation(
    IN HANDLE LogHandle
    )
{
    UNREFERENCED_PARAMETER( LogHandle );
    return TRUE;
}

__inline
BOOL
xSetComponent(
    IN HANDLE LogHandle,
    IN LPSTR  lpCompName,
    IN LPSTR  lpSubCompName
    )
{
    UNREFERENCED_PARAMETER( LogHandle );
    printf( "\n*** %s, %s ***\n", lpCompName, lpSubCompName );
    return TRUE;
}

__inline
BOOL
xSetFunctionName(
    IN HANDLE LogHandle,
    IN LPSTR  lpFuncName
    )
{
    UNREFERENCED_PARAMETER( LogHandle );
    printf( "=== %s ===\n", lpFuncName );
    return TRUE;
}

__inline
LPSTR
WinErrorSymbolicName(
    DWORD /*ErrorCode*/
    )
{
    return "unknown";
}


_inline
LPSTR
NtStatusSymbolicName(
    NTSTATUS /*Status*/
    )
{
    return "unknown";
}


#define DbgPrint printf

#endif // _UNDER_NT


#define BEGIN_VARIATION( v ) \
        { \
            xStartVariation( LogHandle, v ); \
            SetLastError( 0 ); \
            __try

#define END_VARIATION() \
            __except ( EXCEPTION_EXECUTE_HANDLER ) { \
                xLog( \
                    LogHandle, \
                    GetExceptionCode() == DBG_RIPEXCEPTION ? XLL_PASS : XLL_EXCEPTION, \
                    "Exception=%s (0x%X)", \
                    NtStatusSymbolicName(GetExceptionCode()), GetExceptionCode() \
                    ); \
            } \
            xEndVariation( LogHandle ); \
        }

#define LOG_FAIL()          xLog( LogHandle, XLL_FAIL, "last error = %s (0x%X)", \
                                WinErrorSymbolicName(GetLastError()), GetLastError() )

#define LOG_MSG_FAIL( s )   xLog( LogHandle, XLL_FAIL, "%s;last error = %s (0x%X)", \
                                s, WinErrorSymbolicName(GetLastError()), GetLastError() )

#define LOG_BLOCK()         xLog( LogHandle, XLL_BLOCK, "last error = %s (0x%X)", \
                                WinErrorSymbolicName(GetLastError()), GetLastError() )

#define LOG_MSG_BLOCK( s )  xLog( LogHandle, XLL_BLOCK, "%s;last error = %s (0x%X)", \
                                s, WinErrorSymbolicName(GetLastError()), GetLastError() )

#define LOG_PASS()          xLog( LogHandle, XLL_PASS, "last error = %s (0x%X)", \
                                WinErrorSymbolicName(GetLastError()), GetLastError() )

#define LOG_MSG_PASS( s )   xLog( LogHandle, XLL_PASS, "%s;last error = %s (0x%X)", \
                                s, WinErrorSymbolicName(GetLastError()), GetLastError() )

#define LOG_PASS_FAIL( b )  xLog( LogHandle, (b) ? XLL_PASS : XLL_FAIL, "last error = %s (0x%X)", \
                                WinErrorSymbolicName(GetLastError()), GetLastError() )

#endif // _A8FD5CE7_2528_4795_90B4_4845082EDC3F_
