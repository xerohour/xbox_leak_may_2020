/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    apicln.c

Abstract:

    This module contains functions for cleaning up whatever resources were created
    by an API which was tested.

Author:

    John Miller (johnmil) 19-Mar-1992

Environment:

    XBox

Revision History:

    11-Apr-2000     schanbai

        Added more clean up routines

--*/


#ifdef __cplusplus
extern "C" {
#endif

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>

#ifdef __cplusplus
}
#endif

#include <apicln.h>
#include <stdarg.h>
#include <filever.h>


VOID
GlobalAllocAPICleanup(
    DWORD dwRet,
    DWORD *dwParms,
    HANDLE hConOut,
    HANDLE hLog
    )
{
    HANDLE hRet, h;
    UNREFERENCED_PARAMETER( dwParms );
    UNREFERENCED_PARAMETER( hConOut );

    if ( NULL == (h = (HANDLE)dwRet) ) {
        return;
    }

    hRet = GlobalFree( h );
    
    if ( hRet != NULL ) {
        ErrorPrint(
            hLog,
            "GlobalAllocAPICleanup",
            0,
            "Unable to free GlobalAlloc() results"
            );
    }
}


VOID
HeapCreateAPICleanup(
    DWORD dwRet,
    DWORD *dwParms,
    HANDLE hConOut,
    HANDLE hLog
    )
{
    HANDLE h = (HANDLE)dwRet;
    BOOL bDestroyed;
    
    UNREFERENCED_PARAMETER( dwParms );
    UNREFERENCED_PARAMETER( hConOut );

    if ( h != NULL ) {
        
        bDestroyed = HeapDestroy( h );
        if ( !bDestroyed ) {
            ErrorPrint(
                hLog,
                "HeapCreateAPICleanup",
                0,
                "Unable to free HeapCreate() results"
                );
        }

    }
}


VOID
LocalAllocAPICleanup(
    DWORD dwRet,
    DWORD *dwParms,
    HANDLE hConOut,
    HANDLE hLog
    )
{
    HANDLE hRet;

    UNREFERENCED_PARAMETER( dwParms );
    UNREFERENCED_PARAMETER( hConOut );
    
    if ( dwRet ) {
        hRet = LocalFree( (HANDLE)dwRet );
        if ( hRet != NULL ) {
            ErrorPrint(
                hLog,
                "LocalAllocAPICleanup",
                0,
                "Unable to free LocalAlloc() results"
                );
        }
    }
}


VOID
VirtualAllocAPICleanup(
    DWORD dwRet,
    DWORD *dwParms,
    HANDLE hConOut,
    HANDLE hLog
    )
{
    LPBYTE p = (LPBYTE)dwRet;

    UNREFERENCED_PARAMETER( dwParms );
    UNREFERENCED_PARAMETER( hConOut );
    
    if ( dwRet && !VirtualFree( p, 0, MEM_RELEASE ) ) {
        ErrorPrint(
            hLog,
            "VirtualAllocAPICleanup",
            0,
            "Unable to free VirtualAlloc() results"
            );
    }
}


VOID
CreateFileWAPICleanup(
    DWORD dwRet,
    DWORD *dwParms,
    HANDLE hConOut,
    HANDLE hLog
    )
{
    HANDLE hFile = (HANDLE)dwRet;

    UNREFERENCED_PARAMETER( hConOut );

    // Nothing to do if not opened
    if ( hFile == INVALID_HANDLE_VALUE ) {
        return;
    }

    // Close handle 
    if ( !CloseHandle(hFile) ) {
        ErrorPrint(
            hLog,
            "CreateFileWAPICleanup",
            0,
            "Error closing file"
            );
        return;
    }

    if ( SetFileAttributesW( (LPCWSTR)dwParms[0] , FILE_ATTRIBUTE_NORMAL ) &&
         !DeleteFileW((LPCWSTR)dwParms[0]) ) {
        ErrorPrint(
            hLog,
            "CreateFileWAPICleanup",
            0,
            "Unable to delete file"
            );
    }
}


VOID
CreateFileAAPICleanup(
    DWORD dwRet,
    DWORD *dwParms,
    HANDLE hConOut,
    HANDLE hLog
    )
{
    HANDLE hFile = (HANDLE)dwRet;

    UNREFERENCED_PARAMETER( hConOut );

    // Nothing to do if not opened
    if ( hFile == INVALID_HANDLE_VALUE ) {
        return;
    }

    // Close handle 
    if ( !CloseHandle(hFile) ) {
        ErrorPrint(
            hLog,
            "CreateFileAAPICleanup",
            0,
            "Error closing file"
            );
        return;
    }

    // Change attributes to normal, so can be deleted
    SetFileAttributesA( (LPCSTR)dwParms[0], FILE_ATTRIBUTE_NORMAL );

    if ( !DeleteFileA((LPCSTR)dwParms[0]) ) {
        ErrorPrint(
            hLog,
            "CreateFileAAPICleanup",
            0,
            "Unable to delete file"
            );
    }
}


VOID
GenericHandleCleanup(
    DWORD dwRet,
    DWORD *dwParms,
    HANDLE hConOut,
    HANDLE hLog
    )
{
    HANDLE h;

    UNREFERENCED_PARAMETER( hConOut );
    UNREFERENCED_PARAMETER( dwParms );
    UNREFERENCED_PARAMETER( hLog );

    h = (HANDLE)dwRet;

    if ( h != INVALID_HANDLE_VALUE ) {
        CloseHandle( h );
    }
}


VOID
CreateThreadAPICleanup(
    DWORD dwRet,
    DWORD *dwParms,
    HANDLE hConOut,
    HANDLE hLog
    )
{
    HANDLE ThreadHandle = (HANDLE)dwRet;

    UNREFERENCED_PARAMETER( hConOut );
    UNREFERENCED_PARAMETER( dwParms );
    UNREFERENCED_PARAMETER( hLog );

    if ( NULL != ThreadHandle ) {
        //BUGBUG no TerminateThread under Xbox
        //if ( !TerminateThread( ThreadHandle, 0xdeadbeef ) ) {
            ResumeThread( ThreadHandle );
            QueueUserAPC((PAPCFUNC)ExitThread, ThreadHandle, 0xdeadbeef);
        //}
        CloseHandle( ThreadHandle );
    }
}


INT
APrintf(
    char *Format,
    ...
    )
{
    char buff[512];
    va_list vaList;
    DWORD i;

    va_start( vaList, Format );
    i = vsprintf( buff, Format, vaList );
    OutputDebugStringA( buff );
    return i;
}


VOID
ErrorPrint(
    HANDLE hLog,
    char *Module,
    int CaseNo,
    char *Desc
    )
{
    UNREFERENCED_PARAMETER( CaseNo );

    APrintf( "APICLN: %s -- %s\n", Module, Desc);
    
    if ( hLog != INVALID_HANDLE_VALUE ) {
        xLog( hLog, XLL_WARN, "APICLN-%s -- %s", Module, Desc );
    }
}


BOOL
WINAPI
ApiclnDllEntry(
    HINSTANCE hInstance,
    DWORD fdwReason,
    LPVOID lpvReserved
    )
{
    UNREFERENCED_PARAMETER( hInstance );
    UNREFERENCED_PARAMETER( fdwReason );
    UNREFERENCED_PARAMETER( lpvReserved );
    return TRUE;
}
