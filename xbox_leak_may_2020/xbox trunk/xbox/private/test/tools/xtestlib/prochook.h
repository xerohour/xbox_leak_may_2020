/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    prochook.h

Abstract:

    Routines to hook imported functions

Author:

    Sakphong Chanbai (schanbai) 05-Jul-2000

Environment:

    Xbox

Revision History:

--*/


#ifndef __HOOKPROC_H__
#define __HOOKPROC_H__

#ifdef __cplusplus
extern "C" {
#endif


PROC WINAPI HookImportedFunction(
    HMODULE hFromModule,        // Module to intercept calls from
    LPCSTR pszFunctionModule,   // Module to intercept calls to
    LPCSTR pszFunctionName,     // Function to intercept calls to
    PROC pfnNewProc             // New function (replace old function)
    );


PROC WINAPI HookImportedFunctionEx(
    HMODULE hFromModule,        // Module to intercept calls from
    LPCSTR pszFunctionModule,   // Module to intercept calls to
    PROC pfnProc,               // Function to intercept calls to
    PROC pfnNewProc             // New function (replace old function)
    );


PROC WINAPI HookPEImportedFunctionEx(
    ULONG_PTR BaseAddress,      // Module to intercept calls from
    LPCSTR pszFunctionModule,   // Module to intercept calls to
    PROC pfnProc,               // Function to intercept calls to
    PROC pfnNewProc             // New function (replace old function)
    );


#ifdef __cplusplus
}
#endif

#endif // __HOOKPROC_H__
