/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.c

Abstract:

  This module contains the utility functions for xnetapi.dll

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

namespace XNetAPINamespace {

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



LPSTR
GetIniSection(
    HANDLE  hMemObject,
    LPCSTR  lpszSectionName
)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets the strings in the specified section of the .ini file

Arguments:

  hMemObject - handle to the memory object
  lpszSectionName - pointer to the ini section name

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
        dwResult = GetProfileSectionA(lpszSectionName, lpszString, dwBufferSize);

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



BOOL
FindString(
    LPSTR  lpszString,
    LPSTR  lpszSubString
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
    LPSTR  lpszCurString = lpszString;

    while ('\0' != *lpszCurString) {
        if (0 == _stricmp(lpszCurString, lpszSubString)) {
            break;
        }

        lpszCurString += (strlen(lpszCurString) + 1);
    }

    return ('\0' != *lpszCurString);
}



BOOL
ParseAndFindString(
    LPSTR  lpszString,
    LPSTR  lpszSubString
)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Parse the specified string for the first space and compares the result against the string

Arguments:

  lpszString - pointer to the string
  lpszSubString - pointer to the specified string

Return Value:

  BOOL:
    If the specified string is found, the return value is non-zero.
    If the specified string is not found, the return value is zero.

-----------------------------------------------------------------------------*/
{
    // lpszSpace is a pointer to the space
    LPSTR  lpszSpace = NULL;
    // lpszCurString is a pointer to the current string
    LPSTR  lpszCurString = lpszString;

    lpszSpace = strchr(lpszSubString, ' ');

    while ('\0' != *lpszCurString) {
        if ((strlen(lpszCurString) == (lpszSpace - lpszSubString)) && (0 == _strnicmp(lpszCurString, lpszSubString, lpszSpace - lpszSubString))) {
            break;
        }

        lpszCurString += (strlen(lpszCurString) + 1);
    }

    return ('\0' != *lpszCurString);
}

} // namespace XNetAPINamespace
