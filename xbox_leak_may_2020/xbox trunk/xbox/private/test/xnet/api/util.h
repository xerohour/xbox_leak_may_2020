/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.h

Abstract:

  This module contains the definitions for util.c

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#pragma once



namespace XNetAPINamespace {

LPSTR
GetIniString(
    HANDLE  hMemObject,
    LPCSTR  lpszSectionName,
    LPCSTR  lpszKeyName
);

LPSTR
GetIniSection(
    HANDLE  hMemObject,
    LPCSTR  lpszSectionName
);

BOOL
FindString(
    LPSTR  lpszString,
    LPSTR  lpszSubString
);

BOOL
ParseAndFindString(
    LPSTR  lpszString,
    LPSTR  lpszSubString
);

} // namespace XNetAPINamespace
