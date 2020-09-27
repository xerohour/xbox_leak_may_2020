/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    dllmain.c

Abstract:

    DLL entry point for DLL version of Badman. This module will read all
    parameter settings from .ini file and pass to main in original badman.
    By doing this, we can minimize changes to original badman and minimize
    any regression bugs that might cause by our modification

Author:

    Sakphong Chanbai (schanbai) 07-Mar-2000

Environment:

    XBox

Revision History:

--*/


#include "precomp.h"
#include <stdio.h>
#include "xtestlib.h"
#include "profilep.h"


typedef struct _BADMAN_STARTER_PARAMS {
    HANDLE LogHandle;
    INT    Argc;
    LPSTR* Argv;
} BADMAN_STARTER_PARAMS, *PBADMAN_STARTER_PARAMS;


HANDLE HeapHandle;


VOID
_cdecl
badman_main(
    int Argc,
    char * Argv[],
    HANDLE LogHandle
    );

BOOL
WINAPI
DllMain(
    HINSTANCE hInstance,
    DWORD fdwReason,
    LPVOID lpvReserved
    )
{
    UNREFERENCED_PARAMETER( lpvReserved );

    if ( DLL_PROCESS_ATTACH == fdwReason ) {
        DisableThreadLibraryCalls( hInstance );
        HeapHandle = HeapCreate( 0, 0, 0 );
    } else if ( DLL_PROCESS_DETACH == fdwReason ) {
        HeapDestroy( HeapHandle );
    }

    return HeapHandle != NULL;
}


DWORD
WINAPI
BadmanThreadProc(
    PBADMAN_STARTER_PARAMS p
    )
{
    badman_main( p->Argc, p->Argv, p->LogHandle );
    return 0;
}


VOID
WINAPI
StartTest(
    IN HANDLE LogHandle
    )
{
    HANDLE ThreadHandle;
    BADMAN_STARTER_PARAMS bsp;
    char CmdLine[256];
    char KeyName[32];
    int i;


    ASSERT( LogHandle != INVALID_HANDLE_VALUE );
    bsp.LogHandle = LogHandle;

    for ( i=0; i<50; i++ ) {
        sprintf( KeyName, "CommandLine%u", i);
        GetProfileStringA( "badman", KeyName, "", CmdLine, sizeof(CmdLine) );

        if ( !CmdLine[0] ) {
            break;
        }

        bsp.Argv = CommandLineToArgvA( CmdLine, &bsp.Argc );

        //
        // We have to run badman in separate thread because badman uses a lot of
        // 'exit' C run-time library and we want to change 'exit' to ExitThread.
        //
        // Do not call ExitThread unless you create thread for it. This is
        // important for the harness
        //

        ThreadHandle = CreateThread(
                            NULL,
                            0,
                            (LPTHREAD_START_ROUTINE)BadmanThreadProc,
                            (LPVOID)&bsp,
                            0,
                            NULL
                            );

            if ( !ThreadHandle ) {
                DbgPrint( "BADMAN: Unable to create thread (%d)\n", GetLastError() );
            }


        if ( ThreadHandle ) {
            WaitForSingleObject( ThreadHandle, INFINITE );
            CloseHandle( ThreadHandle );
        }

        if ( bsp.Argv ) {
            GlobalFree( bsp.Argv );
        }
    }
}


VOID
WINAPI
EndTest(
    VOID
    )
{
    KdPrint(( "BADMAN: EndTest is called\n" ));
}
