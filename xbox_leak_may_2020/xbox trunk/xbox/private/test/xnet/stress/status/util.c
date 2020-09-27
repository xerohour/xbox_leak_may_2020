/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.c

Abstract:

  This module contains the utility functions for xnetstressstatus.exe

Author:

  Steven Kehrli (steveke) 8-Feb-2002

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
