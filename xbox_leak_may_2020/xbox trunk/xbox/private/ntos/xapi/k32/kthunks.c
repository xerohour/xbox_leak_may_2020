#include "basedll.h"
#include <stdio.h>


VOID
WINAPI
DebugBreak()
{
    DbgBreakPoint();
}

VOID
WINAPI
GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime)
{
    KeQuerySystemTime((PLARGE_INTEGER) lpSystemTimeAsFileTime);
}

int
WINAPIV
wsprintfW(LPWSTR lpOut, LPCWSTR lpFmt, ...)
{
    va_list arglist;
    int ret;

    va_start(arglist, lpFmt);
    ret = wvsprintfW(lpOut, lpFmt, arglist);

    va_end(arglist);
    return ret;
}

int
WINAPIV
wsprintfA(LPSTR lpOut, LPCSTR lpFmt, ...)
{
    va_list arglist;
    int ret;

    va_start(arglist, lpFmt);
    ret = wvsprintfA(lpOut, lpFmt, arglist);

    va_end(arglist);
    return ret;
}

//
// We need a wrapper for wvsprintf() (as opposed to forwarding it to NTOSKRNL)
// because the calling convention is not the same as vwsprintf()
//

int
WINAPI
wvsprintfW(
    OUT LPWSTR lpOut,
    IN LPCWSTR lpFmt,
    IN va_list arglist)
{
    return vswprintf(lpOut, lpFmt, arglist);
}

int
WINAPI
wvsprintfA(
    OUT LPSTR lpOut,
    IN LPCSTR lpFmt,
    IN va_list arglist)
{
    return vsprintf(lpOut, lpFmt, arglist);
}

ULONG
WINAPIV
DebugPrint(PCHAR Format, ...)
{
    va_list arglist;
    CHAR string[MAX_PATH];
    ULONG ret;

    va_start(arglist, Format);
    ret = _vsnprintf(string, sizeof(string), Format, arglist);
    OutputDebugStringA(string);

    va_end(arglist);
    return ret;
}
