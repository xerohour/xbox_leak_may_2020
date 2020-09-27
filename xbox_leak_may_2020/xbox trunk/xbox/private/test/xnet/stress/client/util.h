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



namespace XNetStressNamespace {

LPSTR
GetIniString(
    HANDLE  hMemObject,
    LPCSTR  lpszSectionName,
    LPCSTR  lpszKeyName
);

} // namespace XNetStressNamespace
