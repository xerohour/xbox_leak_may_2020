/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xmem.h

Abstract:

  This module contains the definitions for xmem.dll

Author:

  Steven Kehrli (steveke) 1-Nov-2001

------------------------------------------------------------------------------*/

#pragma once



#ifdef __cplusplus
extern "C" {
#endif



HANDLE
WINAPI
xMemCreateLocal(
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
);

LPVOID
WINAPI
xMemAllocLocal(
    IN  HANDLE  hMemObject,
    IN  SIZE_T  dwBytes,
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
);

LPVOID
WINAPI
xMemReAllocLocal(
    IN  HANDLE  hMemObject,
    IN  LPVOID  lpMem,
    IN  SIZE_T  dwBytes,
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
);

BOOL
WINAPI
xMemFree(
    IN  HANDLE  hMemObject,
    IN  LPVOID  lpMem
);

BOOL
WINAPI
xMemClose(
    IN  HANDLE  hMemObject
);

#define xMemCreate() xMemCreateLocal(__FILE__, __LINE__)
#define xMemAlloc(hMemObject, dwBytes) xMemAllocLocal(hMemObject, dwBytes, __FILE__, __LINE__)
#define xMemReAlloc(hMemObject, lpMem, dwBytes) xMemReAllocLocal(hMemObject, lpMem, dwBytes, __FILE__, __LINE__)

#ifdef __cplusplus
}
#endif
