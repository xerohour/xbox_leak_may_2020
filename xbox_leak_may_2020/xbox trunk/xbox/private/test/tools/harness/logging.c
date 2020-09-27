/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    logging.c

Abstract:

    This module contains routine to initialize logging engine

--*/

#include "precomp.h"
#include "logging.h"

HANDLE HarnessLogHandle;
extern WEB_SERVER_INFO_A WebServerInfo;

BOOL
WINAPI
HarnessInitializeLogging(
    VOID
    )
{
    HarnessLogHandle = xCreateLog_A(
                            "t:\\harness.txt",
                            &WebServerInfo,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG
                            );
    if ( HarnessLogHandle == INVALID_HANDLE_VALUE ) {
        DbgPrint( "HARNESS: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()) );
    } else {
        xSetComponent( HarnessLogHandle, "Harness", "Harness" );
        xSetFunctionName( HarnessLogHandle, "This is not a test result" );
        xSetOwnerAlias( HarnessLogHandle, "schanbai" );
        xStartVariation( HarnessLogHandle, "Internal Logging" );
    }

    return HarnessLogHandle != INVALID_HANDLE_VALUE;
}

BOOL
WINAPI
HarnessShutdownLogging(
    VOID
    )
{
    ASSERT( HarnessLogHandle != INVALID_HANDLE_VALUE );
    xEndVariation( HarnessLogHandle );
    xCloseLog( HarnessLogHandle );
    return TRUE;
}

BOOL
HarnessLog(
    DWORD LogLevel,
    LPSTR Format,
    ...
    )
{
    BOOL b;
    va_list arg_ptr;

    ASSERT( HarnessLogHandle != INVALID_HANDLE_VALUE );

    va_start( arg_ptr, Format );
    b = xLog_va( HarnessLogHandle, LogLevel, Format, arg_ptr );
    va_end( arg_ptr );
    return b;
}
