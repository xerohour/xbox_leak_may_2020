/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    dllmain.c

Abstract:

    DLL entry point for xtestlib

Author:

    Sakphong Chanbai (schanbai) 08-Mar-2000

Environment:

    XBox

Revision History:

--*/


#include "precomp.h"
#include "misc.h"
#include "leak.h"
#include "heaptrack.h"
#include "xlog.h"

//
// Flag set if running under Xbox development kit
//

BOOL XTestLibRunningInDevkit = FALSE;


BOOL
XTestLibIsRunningInDevkit(
    VOID
    )
{
    return XTestLibRunningInDevkit;
}


VOID
CDECL
XTestLibStartup(
    VOID
    )
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES ObjA;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;
    OBJECT_STRING DevkitDirectoryName;

    //
    // Copy TESTINI.INI from DVD to title partition if it doesn't exist
    //
    if ( !CopyFile(OTEXT("d:\\testini.ini"), OTEXT("t:\\testini.ini"), TRUE) ) {
        if ( GetLastError() != ERROR_FILE_NOT_FOUND ) {
            DbgPrint(
                "XTESTLIB: warning! unable to copy testini.ini from D: to T: (%s)\n",
                WinErrorSymbolicName(GetLastError())
                );
        }
    }

    NtGlobalFlag = GetProfileIntA(
                        "GeneralSettings",
                        "NtGlobalFlags",
                        NtGlobalFlag
                        );

    //
    // The default behavior of checked kernel has these 2 bits set
    //
#if DBG
    NtGlobalFlag |= FLG_ENABLE_CLOSE_EXCEPTIONS | FLG_ENABLE_KDEBUG_SYMBOL_LOAD;
#endif
    KdPrint(( "XTESTLIB: NtGlobalFlags = 0x%p\n", NtGlobalFlag ));

#if 0
    if ( !InitializeHeapHandleTracking() ) {
        KdPrint(( "XTESTLIB: unable to initialize heap handle tracking\n" ));
    }

    if ( !InitializeHeapAllocationHistory() ) {
        KdPrint(( "XTESTLIB: unable to initialize allocation tracking\n" ));
    }
#endif

    //
    // Check to see if we are running in DEVKIT
    // BUGBUG: is there any way better than this?
    //

    RtlInitObjectString(
        &DevkitDirectoryName,
        OTEXT("\\Device\\Harddisk0\\Partition1\\Devkit")
        );

    InitializeObjectAttributes(
        &ObjA,
        &DevkitDirectoryName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Status = NtQueryFullAttributesFile( &ObjA, &NetworkInfo );
    if ( NT_SUCCESS(Status) ) {
        if ( NetworkInfo.FileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            XTestLibRunningInDevkit = TRUE;
        }
    }
}


VOID
CDECL
XTestLibCleanup(
    VOID
    )
{
#if 0
    UninitializeHeapHandleTracking();
    UninitializeHeapAllocationHistory();
#endif
}


//
// Setup a startup pointer to be called by CRT
//
#pragma data_seg(".CRT$XIU")
PROC xtestlib_startup = (PROC)XTestLibStartup;
#pragma data_seg()


//
// Setup a cleanup pointer to be called by CRT
//
#pragma data_seg(".CRT$XTU")
PROC xtestlib_cleanup = (PROC)XTestLibCleanup;
#pragma data_seg()
