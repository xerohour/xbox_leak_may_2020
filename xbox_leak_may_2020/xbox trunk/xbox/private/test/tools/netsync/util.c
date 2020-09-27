/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  util.c

Abstract:

  This module contains the utility functions for netsync.dll

Author:

  Steven Kehrli (steveke) 21-Jul-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



namespace NetsyncNamespace {

DWORD  dwNetsyncDebug = 0;  // dwNetsyncDebug specifies the level of debug messages enabled



VOID
DebugMessage(
    IN  DWORD  dwDebugLevel,
    IN  LPSTR  lpszFormatString,
    ...
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Displays a message in the debugger

Arguments:

  dwDebugLevel - The debug level of the message
  lpszFormatString - Pointer to a null-terminated format string for the message

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // varg_ptr is a pointer to the variable argument list
    va_list     varg_ptr;
    // SystemTime is the current system time
    SYSTEMTIME  SystemTime;
    // szDebugBuffer is the debug message string
    CHAR        szDebugBuffer[1024];
    // dwCaptionSize is the size of the caption, in bytes
    DWORD       dwCaptionSize;



    if (0 != (dwNetsyncDebug & dwDebugLevel)) {
        // Initialize the buffer
        ZeroMemory(szDebugBuffer, sizeof(szDebugBuffer));

        // Get the current time
        GetLocalTime(&SystemTime);

        dwCaptionSize = sprintf(szDebugBuffer, "NETSYNC - Time: %02d.%02d.%04d@%02d:%02d:%02d.%03d - Thread Id: 0x%08x\n  ", SystemTime.wMonth, SystemTime.wDay, SystemTime.wYear, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds, GetCurrentThreadId());

        va_start(varg_ptr, lpszFormatString);
        _vsnprintf(&szDebugBuffer[dwCaptionSize], sizeof(szDebugBuffer) - dwCaptionSize, lpszFormatString, varg_ptr);
        OutputDebugStringA(szDebugBuffer);
    }
}



BOOL
AddRefNet(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Increments the reference count for the net stack

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // WSAData is the details of the Winsock implementation
    WSADATA   WSAData;
    // XNetRefCount is the reference count of the net stack
    ULONG     XNetRefCount = 0;
    // dwErrorCode is the last error code
    DWORD     dwErrorCode = ERROR_SUCCESS;



#ifdef _XBOX
    // Initialize the net stack
    XNetRefCount = XNetAddRef();
    if (0 == XNetRefCount) {
        // Get the last error code
        dwErrorCode = GetLastError();

        goto FunctionFailed0;
    }
#endif

    // Initialize Winsock
    ZeroMemory(&WSAData, sizeof(WSAData));
    dwErrorCode = WSAStartup(MAKEWORD(2, 2), &WSAData);
    if (0 != dwErrorCode) {
        goto FunctionFailed1;
    }

    return TRUE;

FunctionFailed1:
#ifdef _XBOX
    // Terminate the net stack
    XNetRelease();

FunctionFailed0:
#endif
    return FALSE;
}



VOID
ReleaseNet(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Decrements the reference count for the net stack

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Terminate Winsock
    WSACleanup();

#ifdef _XBOX
    // Terminate the net stack
    XNetRelease();
#endif
}



#ifdef _XBOX

LPWSTR
GetIniSection(
    HANDLE   hMemObject,
    LPCWSTR  lpszSectionName
)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets the strings in the specified section of the .ini file

Arguments:

  hMemObject - handle to the memory object
  lpszSectionName - pointer to the ini section name

Return Value:

  LPWSTR:
    If the function succeeds, the return value is a pointer to the string.
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

-----------------------------------------------------------------------------*/
{
    // lpszString is a pointer to the string
    LPWSTR  lpszString = NULL;
    // dwBufferSize is the size of the buffer referenced by lpszString
    DWORD   dwBufferSize = 0;
    // dwResult is the result of the GetProfileString call
    DWORD   dwResult = 0;

    // Allocate the memory for the string
    dwBufferSize = 0x10;
    lpszString = (LPWSTR) xMemAlloc(hMemObject, dwBufferSize * sizeof(WCHAR));

    while (NULL != lpszString) {
        dwResult = GetProfileSectionW(lpszSectionName, lpszString, dwBufferSize);

        if (0 == dwResult) {
            goto FunctionFailed;
        }

        if (dwResult < (dwBufferSize - 2)) {
            break;
        }

        dwBufferSize += 0x10;
        lpszString = (LPWSTR) xMemReAlloc(hMemObject, lpszString, dwBufferSize * sizeof(WCHAR));
    }

    if (NULL == lpszString) {
        goto FunctionFailed;
    }

    return lpszString;

FunctionFailed:
    if (NULL != lpszString) {
        xMemFree(hMemObject, lpszString);
    }

    return NULL;
}



BOOL
FindString(
    LPWSTR  lpszString,
    LPWSTR  lpszSubString
)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Search a list of strings for a specified string

Arguments:

  lpszString - pointer to the list of strings
  lpszSubString - pointer to the specified string

Return Value:

  BOOL:
    If the specified string is found, the return value is non-zero.
    If the specified string is not found, the return value is zero.

-----------------------------------------------------------------------------*/
{
    // lpszCurString is a pointer to the current string
    LPWSTR  lpszCurString = lpszString;

    while ('\0' != *lpszCurString) {
        if (0 == _wcsicmp(lpszCurString, lpszSubString)) {
            break;
        }

        lpszCurString += (wcslen(lpszCurString) + 1);
    }

    return ('\0' != *lpszCurString);
}

#endif

} // namespace NetsyncNamespace
