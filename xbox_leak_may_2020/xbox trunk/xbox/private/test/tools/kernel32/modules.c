/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    modules.c

Abstract:

    This module contains APIs for KERNEL32. We want to use MSVCRT.DLL 
    but it couldn't be built from NT source tree. So we wrap all calls 
    required by MSVCRT.DLL here since in XBox we don't need KERNEL32.DLL

Author:

    Sakphong Chanbai (schanbai) 27-Mar-2000

Environment:

    Xbox

Revision History:

--*/


#include "precomp.h"


DWORD
WINAPI
GetModuleFileNameA(
    HMODULE hModule,
    LPSTR   lpFileName,
    DWORD   nSize
    )
{
    PCHAR ModuleName;
    SIZE_T StringLength;

    //
    // Try to get the module file name of main program
    //
    if ( hModule == NULL ) {
        ModuleName = XeModuleNameFromModuleIndex( 0 );
    } else {
        //
        // Check to see if the hInstance (or hModule) is really an image base
        // or PXIMAGE_MODULE_HEADER
        //
        PWORD p = (PWORD)hModule;
        if ( *p == IMAGE_DOS_SIGNATURE ) {
            ULONG ModuleIndex = XeModuleIndexFromAddress( (ULONG_PTR)hModule );
            ModuleName = XeModuleNameFromModuleIndex( ModuleIndex );
        } else {
            ModuleName = XeModuleNameFromModuleHeader( hModule );
        }
    }

    if ( !ModuleName ) {
        SetLastError( ERROR_MOD_NOT_FOUND );
        return 0;
    }

    StringLength = strlen(ModuleName);

    if (  StringLength >= nSize ) {
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
        return 0;
    }

    try {
        strcpy( lpFileName, ModuleName );
        SetLastError( ERROR_SUCCESS );
    } except( EXCEPTION_EXECUTE_HANDLER ) {
        SetLastError( RtlNtStatusToDosError(GetExceptionCode()) );
        StringLength = 0;
    }

    return StringLength;
}


DWORD
WINAPI
GetModuleFileNameW(
    HMODULE hModule,
    LPWSTR  lpFileName,
    DWORD   nSize
    )
{
    CHAR ModuleName[MAX_PATH];
    DWORD i, Length = GetModuleFileNameA( hModule, ModuleName, sizeof(ModuleName) );

    if ( Length ) {
        try {
            for ( i=0; i<=Length; i++ ) {
                lpFileName[i] = (WCHAR)ModuleName[i];
            }
        } except( EXCEPTION_EXECUTE_HANDLER ) {
            Length = 0;
            SetLastError( RtlNtStatusToDosError(GetExceptionCode()) );
        }
    }

    return Length;
}
