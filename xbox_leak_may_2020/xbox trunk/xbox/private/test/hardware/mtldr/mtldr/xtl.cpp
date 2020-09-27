/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xtl.cpp

Abstract:

    This module contains various XTL wrapper used by Xnet

--*/

#include "mtldrp.h"

EXTERN_C ULONG XDebugOutLevel = 0;

EXTERN_C
VOID
WINAPI
vXDebugPrint(
        ULONG Level,
        PCHAR Module,
        PCHAR Format,
        va_list arglist
        )
{
    CHAR string[MAX_PATH];

    if (Level <= XDebugOutLevel) {
        _vsnprintf(string, sizeof(string), Format, arglist);
        DbgPrint( "[%s]: %s\n", Module, string );
    }
}

VOID
WINAPI
XRegisterThreadNotifyRoutine(
    PXTHREAD_NOTIFICATION pThreadNotification,
    BOOL fRegister
    )
/*++

Routine Description:

    Register or deregister a notification routine which will be called
    when a thread is created or deleted.

    This routine currently does nothing as it's a wrapper for Xnet

Arguments:

    pThreadNotification - Points to a XTHREAD_NOTIFICATION structure
        NOTE: The XTHREAD_NOTIFICATION structure must remain
        valid until the thread notification routine is deregistered.
        For example, you can use a global variable for this.
        But you should NOT use a local variable inside a function.

    fRegister - TRUE to register a new thread notification routine
        FALSE to deregister a previously registered notification routine

Return Value:

    NONE

--*/
{
    UNREFERENCED_PARAMETER( pThreadNotification );
    UNREFERENCED_PARAMETER( fRegister );
}

DWORD
WINAPI
XQueryValue(
    IN ULONG ulValueIndex,
    OUT PULONG pulType,
    OUT PVOID pValue,
    IN ULONG cbValueLength,
    IN OUT PULONG pcbResultLength
    )
/*++

Routine Description:

    This routine currently does nothing as it's a wrapper for Xnet

Arguments:

    Unused

Return Value:

    Always zero

--*/
{
    UNREFERENCED_PARAMETER( ulValueIndex );
    UNREFERENCED_PARAMETER( pulType );
    UNREFERENCED_PARAMETER( pValue );
    UNREFERENCED_PARAMETER( cbValueLength );
    UNREFERENCED_PARAMETER( pcbResultLength );

    return 0;
}
