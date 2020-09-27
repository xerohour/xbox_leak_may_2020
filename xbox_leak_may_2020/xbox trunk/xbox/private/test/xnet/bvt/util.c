/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.c

Abstract:

  This module contains the utility functions for xnetbvt.dll

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetBVTNamespace;

namespace XNetBVTNamespace {

LPSTR
GetIniString(
    HANDLE  hMemObject,
    LPCSTR  lpszSectionName,
    LPCSTR  lpszKeyName
)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets the specified string of the .ini file

Arguments:

  hMemObject - handle to the memory object
  lpszSectionName - pointer to the ini section name
  lpszKeyName - pointer to the ini key name

Return Value:

  LPSTR:
    If the function succeeds, the return value is a pointer to the string.
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

-----------------------------------------------------------------------------*/
{
    // lpszString is a pointer to the string
    LPSTR  lpszString = NULL;
    // dwBufferSize is the size of the buffer referenced by lpszString
    DWORD  dwBufferSize = 0;
    // dwResult is the result of the GetProfileString call
    DWORD  dwResult = 0;

    // Allocate the memory for the string
    dwBufferSize = 0x10;
    lpszString = (LPSTR) xMemAlloc(hMemObject, dwBufferSize);

    while (NULL != lpszString)
    {
        dwResult = GetProfileStringA(lpszSectionName, lpszKeyName, NULL, lpszString, dwBufferSize);

        if (0 == dwResult) {
            goto FunctionFailed;
        }

        if (dwResult < (dwBufferSize - 2))
        {
            break;
        }

        dwBufferSize += 0x10;
        lpszString = (LPSTR) xMemReAlloc(hMemObject, lpszString, dwBufferSize);
    }

    if (NULL == lpszString)
    {
        goto FunctionFailed;
    }

    return lpszString;

FunctionFailed:
    if (NULL != lpszString) {
        xMemFree(hMemObject, lpszString);
    }

    return NULL;
}

} // namespace XNetBVTNamespace
