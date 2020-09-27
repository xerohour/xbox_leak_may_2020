/*++

Copyright (c) 1990-1999  Microsoft Corporation

Module Name:

    debug.c

Abstract:

    This module implements Win32 Debug APIs

--*/

#include "basedll.h"
#include <stdio.h>
#pragma hdrstop

VOID
APIENTRY
OutputDebugStringW(
    LPCWSTR lpOutputString
    )

/*++

Routine Description:

    UNICODE thunk to OutputDebugStringA

--*/

{
    UNICODE_STRING UnicodeString;
    ANSI_STRING AnsiString;
    NTSTATUS Status;

    RtlInitUnicodeString(&UnicodeString,lpOutputString);
    Status = RtlUnicodeStringToAnsiString(&AnsiString,&UnicodeString,TRUE);
    if ( !NT_SUCCESS(Status) ) {
        AnsiString.Buffer = "";
        }
    OutputDebugStringA(AnsiString.Buffer);
    if ( NT_SUCCESS(Status) ) {
        RtlFreeAnsiString(&AnsiString);
        }
}

VOID
APIENTRY
OutputDebugStringA(
    IN LPCSTR lpOutputString
    )

/*++

Routine Description:

    This function allows an application to send a string to its debugger
    for display.  If the application is not being debugged, but the
    system debugger is active, the system debugger displays the string.
    Otherwise, this function has no effect.

Arguments:

    lpOutputString - Supplies the address of the debug string to be sent
        to the debugger.

Return Value:

    None.

--*/

{
    ANSI_STRING ast, *past;

    ast.Buffer = (LPSTR)lpOutputString;
    ast.Length = (USHORT)strlen(ast.Buffer);
    ast.MaximumLength = ast.Length+1;
    _asm {
        lea ecx, ast
        mov eax, BREAKPOINT_PRINT
        int 2dh
        int 3
    }
}

#if DBG

ULONG XDebugOutLevel = XDBG_WARNING;
ULONG XDebugBrkLevel = XDBG_ERROR;

PSTR XDebugLevels[] =
{
    "!!!",
    "ERR",
    "WRN",
    "TRC",
    "ENT",
    "EXT",
};

VOID
XDBGAPI
vXDebugPrint(ULONG Level, PCHAR Module, PCHAR Format, va_list arglist)
{
    CHAR string[MAX_PATH];

    if (Level <= XDebugOutLevel)
    {
        _vsnprintf(string, sizeof(string), Format, arglist);
        DbgPrint("%s[%s]: %s\n", (Level <= XDBG_EXIT) ? XDebugLevels[Level] : "???", Module, string);
        if (Level <= XDebugBrkLevel)
        {
            _asm { int 3 };
        }
    }
}

VOID
XDBGAPIV
XDebugPrint(ULONG Level, PCHAR Module, PCHAR Format, ...)
{
    va_list arglist;
    va_start(arglist, Format);
    vXDebugPrint(Level, Module, Format, arglist);
    va_end(arglist);
}

VOID
XDBGAPIV
XDebugError(PCHAR Module, PCHAR Format, ...)
{
    va_list arglist;
    va_start(arglist, Format);
    vXDebugPrint(XDBG_ERROR, Module, Format, arglist);
    va_end(arglist);
}

VOID
XDBGAPIV
XDebugWarning(PCHAR Module, PCHAR Format, ...)
{
    va_list arglist;
    va_start(arglist, Format);
    vXDebugPrint(XDBG_WARNING, Module, Format, arglist);
    va_end(arglist);
}

VOID
XDBGAPIV
XDebugTrace(PCHAR Module, PCHAR Format, ...)
{
    va_list arglist;
    va_start(arglist, Format);
    vXDebugPrint(XDBG_TRACE, Module, Format, arglist);
    va_end(arglist);
}

VOID
XDBGAPIV
XDebugEntry(PCHAR Module, PCHAR Format, ...)
{
    va_list arglist;
    va_start(arglist, Format);
    vXDebugPrint(XDBG_ENTRY, Module, Format, arglist);
    va_end(arglist);
}

VOID
XDBGAPIV
XDebugExit(PCHAR Module, PCHAR Format, ...)
{
    va_list arglist;
    va_start(arglist, Format);
    vXDebugPrint(XDBG_EXIT, Module, Format, arglist);
    va_end(arglist);
}

#endif // DBG
