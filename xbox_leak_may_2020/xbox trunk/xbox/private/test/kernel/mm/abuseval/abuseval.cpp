/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    abuseval.c

Abstract:

    This module contains routines for page attributes testing.  Page with
    PAGE_READONLY, PAGE_READWRITE, PAGE_GUARD, PAGE_NOACCESS will be tested.

--*/

#include <ntos.h>
#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <xtestlib.h>
#include <xlog.h>

VOID
PageAttributeTestWorker(
    IN HANDLE LogHandle,
    IN PCSTR PageAttributeString,
    IN DWORD PageAttribute,
    IN PVOID lpBuffer,
    IN BOOL  fAllowWrite,
    IN BOOL  fAllowRead
    )
{
    CHAR Buffer[256];
    BOOL b;
    DWORD dwOldProtect;

    sprintf( Buffer, "Write to %s", PageAttributeString );
    xStartVariation( LogHandle, Buffer );

    __try {

        b = VirtualProtect( lpBuffer, PAGE_SIZE, PageAttribute, &dwOldProtect );

        if ( !b ) {
            xLog( LogHandle, XLL_BLOCK,
                  "Failed to change page attributes to %s (%s)",
                  PageAttributeString, WinErrorSymbolicName(GetLastError()) );
            __leave;
        }

        *(volatile CHAR *)lpBuffer = 0;

        xLog( LogHandle, fAllowWrite ? XLL_PASS : XLL_FAIL,
            "No exception raised" );

        sprintf( Buffer, "Write to %s", PageAttributeString );
        xStartVariation( LogHandle, Buffer );

        __try {
            *(volatile CHAR *)lpBuffer;
            xLog( LogHandle, fAllowRead ? XLL_PASS : XLL_FAIL,
                "No exception raised" );
        } __except( EXCEPTION_EXECUTE_HANDLER ) {
            xLog( LogHandle, fAllowRead ? XLL_FAIL : XLL_PASS, "Exception raised" );
        }

        xEndVariation( LogHandle );

    } __except( EXCEPTION_EXECUTE_HANDLER ) {
        xLog( LogHandle, fAllowWrite ? XLL_FAIL : XLL_PASS, "Exception raised" );
    }

    xEndVariation( LogHandle );
}

VOID
PageAttributesTest(
    HANDLE LogHandle,
    LPVOID lpBuffer
    )
/*++

Routine Description:

    This routine will check that the region pointed to by lpBuffer is of
    PAGE_GUARD

Arguments:

    LogHandle - Handle for xLog

    lpBuffer - buffer pointing to the virtualalloc'd region

    dwBufferSize - size of the buffer

Return:

    None

--*/
{
    BOOL b;
    DWORD dwOldProtect;

    //
    // PAGE_GUARD
    //
    // We read the same page twice, first time we expect an exception
    // second time we don't
    //

    xStartVariation( LogHandle, "Read from PAGE_GUARD (1st read)" );

    __try {

        b = VirtualProtect( lpBuffer, PAGE_SIZE, PAGE_READWRITE | PAGE_GUARD,
                            &dwOldProtect );
        if ( !b ) {
            xLog( LogHandle, XLL_BLOCK,
                  "Failed to change page attributes to "
                  "PAGE_READWRITE | PAGE_GUARD (%s)",
                  WinErrorSymbolicName(GetLastError()) );
            __leave;
        }

        *(volatile CHAR *)lpBuffer;
        xLog( LogHandle, XLL_FAIL, "No exception raised" );

        xStartVariation( LogHandle, "Read from PAGE_GUARD (2nd read)" );

        __try {
            *(volatile CHAR *)lpBuffer;
            xLog( LogHandle, XLL_PASS, "No exception raised" );
        } __except( EXCEPTION_EXECUTE_HANDLER ) {
            xLog( LogHandle, XLL_FAIL, "Exception raised unexpectedly" );
        }

        xEndVariation( LogHandle );

    } __except( EXCEPTION_EXECUTE_HANDLER ) {

        xLog( LogHandle, XLL_PASS, "Exception raised" );
    }

    xEndVariation( LogHandle );

    //
    // Test various page attributes
    //

    PageAttributeTestWorker( LogHandle, "PAGE_READONLY", PAGE_READONLY,
                             lpBuffer, FALSE, TRUE );

    PageAttributeTestWorker( LogHandle, "PAGE_READWRITE", PAGE_READWRITE,
                             lpBuffer, TRUE, TRUE );

    PageAttributeTestWorker( LogHandle, "PAGE_NOACCESS", PAGE_NOACCESS,
                             lpBuffer, FALSE, FALSE );
}


VOID
WINAPI
PageAttributesStartTest(
    HANDLE LogHandle
    )

/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/

{
    LPVOID lpBuffer;

    xSetComponent( LogHandle, "Kernel", "Mm" );
    xSetFunctionName( LogHandle, "VirtualAlloc" );

    lpBuffer = VirtualAlloc( NULL, PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE );

    if ( !lpBuffer ) {
        xStartVariation( LogHandle, "Allocation" );
        xLog( LogHandle, XLL_BLOCK, "VirtualAlloc failed (%s)",
              WinErrorSymbolicName(GetLastError()) );
        xEndVariation( LogHandle );
        return;
    }

    PageAttributesTest( LogHandle, lpBuffer );
    VirtualFree( lpBuffer, 0, MEM_RELEASE );
}

VOID
WINAPI
PageAttributesEndTest(
    VOID
    )

/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/

{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( abuseval )
#pragma data_seg()

BEGIN_EXPORT_TABLE( abuseval )
    EXPORT_TABLE_ENTRY( "StartTest", PageAttributesStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", PageAttributesEndTest )
END_EXPORT_TABLE( abuseval )
