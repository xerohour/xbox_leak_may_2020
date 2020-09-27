/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    logging.h

Abstract:

    This module contains routine to initialize logging engine

--*/

#ifndef __LOGGING_H__
#define __LOGGING_H__

#ifdef __cplusplus
extern "C" {
#endif

BOOL
WINAPI
HarnessInitializeLogging(
    VOID
    );

BOOL
WINAPI
HarnessShutdownLogging(
    VOID
    );

BOOL
HarnessLog(
    DWORD LogLevel,
    LPSTR Format,
    ...
    );

#ifdef __cplusplus
}
#endif

#endif // __LOGGING_H__
