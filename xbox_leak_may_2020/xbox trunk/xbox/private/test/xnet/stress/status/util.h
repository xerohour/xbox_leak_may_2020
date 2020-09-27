/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.h

Abstract:

  This module contains the definitions for util.c

Author:

  Steven Kehrli (steveke) 8-Feb-2002

-----------------------------------------------------------------------------*/

#pragma once



BOOL
GetResourceString(
    UINT    uResource,
    LPWSTR  lpszString,
    DWORD   dwStringSize
    ...
);
