/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    harnessapi.h

Abstract:

    This module contains definition of APIs exported from test harness

--*/


#ifndef __HARNESSAPI_H__
#define __HARNESSAPI_H__

#ifdef __cplusplus
extern "C" {
#endif

BOOL
WINAPI
RunTestA(
    SOCKET  socket,
    LPCSTR  TestName
    );

BOOL
WINAPI
RunTestW(
    SOCKET  socket,
    LPCWSTR TestName
    );

BOOL
WINAPI
Shutdown(
    SOCKET socket,
    ULONG  Reserved
    );

#if defined(UNICODE) || defined(_UNICODE)
#define RunTest RunTestW
#else
#define RunTest RunTestA
#endif

#ifdef __cplusplus
}
#endif

#endif // __HARNESSAPI_H__
