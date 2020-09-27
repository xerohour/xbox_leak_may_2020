/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xnetref.h

Abstract:

  This module contains the definitions for xnetref.lib

Author:

  Steven Kehrli (steveke) 24-May-2001

------------------------------------------------------------------------------*/



#pragma once


#ifdef _XBOX



#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus



ULONG
WINAPI
XNetAddRefLocal(
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
);

#define XNetAddRef() XNetAddRefLocal(__FILE__, __LINE__)



ULONG
WINAPI
XNetReleaseLocal(
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
);

#define XNetRelease() XNetReleaseLocal(__FILE__, __LINE__)



#ifdef __cplusplus
}
#endif // __cplusplus



#endif // _XBOX
