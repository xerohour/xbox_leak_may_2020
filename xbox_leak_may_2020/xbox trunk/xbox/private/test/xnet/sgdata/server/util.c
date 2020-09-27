/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.c

Abstract:

  This module contains the utility functions for sgdata_nt.exe

Author:

  Steven Kehrli (steveke) 24-Jan-2002

-----------------------------------------------------------------------------*/

#include "precomp.h"



BOOL
FormatResourceString(
    UINT     uResource,
    LPWSTR   lpszString,
    DWORD    dwStringSize,
    va_list  varg_ptr
)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Formats a resource string using a variable argument list

Arguments:

  uResource - Specifies the resource id
  lpszString - Pointer to the resource string buffer
  dwStringSize - Specifies the resource string buffer size
  varg_ptr - Pointer to the variable argument list

Return Value:

  BOOL:
    If the function succeeds, the return value is non-zero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

-----------------------------------------------------------------------------*/
{
    // hModule is a handle to the module
    HMODULE  hModule = NULL;
    // szResourceBuffer is a buffer for the string resource
    WCHAR    szResourceBuffer[256];
    // dwResult is the result code
    DWORD    dwResult = 0;



    // Get the module handle
    hModule = GetModuleHandle(NULL);
    if (hModule == NULL)
    {
        return FALSE;
    }

    // Initialize the string resource buffer
    ZeroMemory(szResourceBuffer, sizeof(szResourceBuffer));

    // Load the string resource
    if (0 != LoadString(hModule, uResource, szResourceBuffer, sizeof(szResourceBuffer) / sizeof(WCHAR))) {
        // Copy the string resource
        dwResult = _vsnwprintf(lpszString, dwStringSize, szResourceBuffer, varg_ptr);
    }

    return ((0 != dwResult) && (dwStringSize != dwResult));
}



BOOL
GetResourceString(
    UINT    uResource,
    LPWSTR  lpszString,
    DWORD   dwStringSize
    ...
)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets a resource string by calling FormatResourceString()

Arguments:

  uResource - Resource id
  lpszString - Pointer to the resource string buffer
  dwStringSize - Specifies the resource string buffer size

Return Value:

  BOOL:
    If the function succeeds, the return value is non-zero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

-----------------------------------------------------------------------------*/
{
    // varg_ptr is a pointer to the variable argument list
    va_list  varg_ptr;



    // Get the variable argument list
    va_start(varg_ptr, dwStringSize);

    // Call FormatResourceString
    return FormatResourceString(uResource, lpszString, dwStringSize, varg_ptr);
}



VOID
LogActivityHeader(
    HANDLE      hFile
)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Writes activity header to the log file

Arguments:

  hFile - Handle to the log file

Return Value:

  None

-----------------------------------------------------------------------------*/
{
    // cUnicodeBOM is the Unicode BOM
    WCHAR   cUnicodeBOM = 0xFEFF;
    // lpszHeader is the header
    LPWSTR  lpszHeader = L"Date,Time,Remote,Activity,UDP Bytes,TCP Bytes\r\n";
    // cb is the number of bytes written to the log file
    DWORD   cb = 0;



    // Write the header to the file
    WriteFile(hFile, &cUnicodeBOM, sizeof(cUnicodeBOM), &cb, NULL);
    WriteFile(hFile, lpszHeader, wcslen(lpszHeader) * sizeof(WCHAR), &cb, NULL);
}



VOID
LogActivity(
    HANDLE      hFile,
    SYSTEMTIME  *ActivityTime,
    LPWSTR      RemoteName,
    DWORD       dwLogType,
    DWORD       dwBytes
)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Writes activity to the log file

Arguments:

  hFile - Handle to the log file
  ActivityTime - Pointer to the activity time
  RemoteName - Pointer to the remote name
  dwLogType - Specifies the log type
  dwBytes - Specifies the number of bytes

Return Value:

  None

-----------------------------------------------------------------------------*/
{
    // szDateString is the date string
    WCHAR   szDateString[128];
    // szTimeString is the time string
    WCHAR   szTimeString[128];
    // szDataString is the data string
    WCHAR   szDataString[128];
    // szLogString is the log string
    WCHAR   szLogString[1024];
    // lpszAction is the action string
    LPWSTR  lpszAction = NULL;
    // cb is the number of bytes written to the log file
    DWORD   cb = 0;

    // Get the date and time string
    GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, ActivityTime, NULL, szDateString, (sizeof(szDateString) / sizeof(WCHAR)) - 1);
    GetTimeFormat(LOCALE_USER_DEFAULT, 0, ActivityTime, NULL, szTimeString, (sizeof(szTimeString) / sizeof(WCHAR)) - 1);

    // Get the action string
    switch (dwLogType) {
        case LOG_CONNECTED:
            lpszAction = L"Connected";
            break;

        case LOG_DISCONNECTED:
            lpszAction = L"Disconnected";
            break;

        case LOG_TIMEOUT:
            lpszAction = L"Timeout";
            break;

        case LOG_UDP_DATA:
            lpszAction = L"UDP Data";
            break;

        case LOG_TCP_DATA:
            lpszAction = L"TCP Data";
            break;

        default:
            lpszAction = L"Unknown";
            break;
    }

    // Format the log string
    wsprintf(szLogString, L"%s,%s,%s,%s", szDateString, szTimeString, RemoteName, lpszAction);

    if ((LOG_UDP_DATA == dwLogType) || (LOG_TCP_DATA == dwLogType)) {
        wsprintf(szDataString, L",%u,%u", (LOG_UDP_DATA == dwLogType) ? dwBytes : 0, (LOG_TCP_DATA == dwLogType) ? dwBytes : 0);

        wcscat(szLogString, szDataString);
    }

    wcscat(szLogString, L"\r\n");

    // Write the log string to the file
    WriteFile(hFile, szLogString, wcslen(szLogString) * sizeof(WCHAR), &cb, NULL);
}
