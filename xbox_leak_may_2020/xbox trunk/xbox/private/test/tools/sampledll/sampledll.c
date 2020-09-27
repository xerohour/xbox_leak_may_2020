/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    SampleDLL.c

Abstract:

    Sample test DLL for XBox test harness. This sample demostrates how to write
    test DLL for XBox test harness. The DLL exports two of the mandatory APIs,
    StartTest and EndTest. TESTINI.INI must be modified correctly so that the
    harness will load and call StartTest/EndTest for this sample DLL.

    Please see "x-box test harness.doc" for more information.

Author:

    Sakphong Chanbai (schanbai) 21-Mar-2000

Environment:

    XBox

Revision History:

--*/


#include <stdio.h>
#include <xlog.h>
#include <xtestlib.h>


//
// Thread ID in multiple-thread configuration (not a Win32 thread ID)
//
// You can specify how many threads you want to run your test by
// setting the key NumberOfThreads=n under your DLL section. The 
// default is only one thread.
// 

LONG SampleDllThreadID = 0;


//
// Heap handle from HeapCreate
//
HANDLE SampleDllHeapHandle;


BOOL
WINAPI
SampleDllMain(
    HINSTANCE   hInstance,
    DWORD       fdwReason,
    LPVOID      lpReserved
    )
{
    UNREFERENCED_PARAMETER( hInstance );
    UNREFERENCED_PARAMETER( lpReserved );

    if ( fdwReason == DLL_PROCESS_ATTACH ) {

        SampleDllHeapHandle = HeapCreate( 0, 0, 0 );

        if ( !SampleDllHeapHandle ) {
            OutputDebugString( TEXT("SAMPLEDLL: Unable to create heap\n") );
        }

    } else if ( fdwReason == DLL_PROCESS_DETACH ) {

        if ( SampleDllHeapHandle ) {
            HeapDestroy( SampleDllHeapHandle );
        }
    }

    return TRUE;
}


VOID
RunTest1(
    IN HANDLE LogHandle
    )
{
    HANDLE FileHandle;
    CHAR FileName[MAX_PATH];
    xSetComponent( LogHandle, "Harness", "SampleDll" );
    xSetFunctionName( LogHandle, "Test1" );
    xStartVariation( LogHandle, "CreateFile" );

    //
    // First test: Create new file if it doesn't exist, overwrite it 
    // if it is there. This test must be success
    //
    lstrcpyA( FileName, "Z:\\TestFile.XBox" );
    FileHandle = CreateFile(
                    FileName,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,   // No SECURITY_ATTRIBUTES
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL    // No template
                    );

    xLog(
        LogHandle,
        FileHandle == INVALID_HANDLE_VALUE ? XLL_FAIL : XLL_PASS,
        "CreateFile with CREATE_ALWAYS, no template (LastError=%d)",
        GetLastError()
        );

    if ( FileHandle != INVALID_HANDLE_VALUE ) {
        CloseHandle( FileHandle );
    }


    //
    // Second test: Always create new file. This test must be failed
    // because the file is already there
    //
    FileHandle = CreateFile(
                    FileName,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,   // No SECURITY_ATTRIBUTES
                    CREATE_NEW,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL    // No template
                    );

    xLog(
        LogHandle,
        FileHandle == INVALID_HANDLE_VALUE ? XLL_PASS: XLL_FAIL,
        "CreateFile with CREATE_NEW, no template (LastError=%d)",
        GetLastError()
        );

    if ( FileHandle != INVALID_HANDLE_VALUE ) {
        CloseHandle( FileHandle );
    }

    //
    // Third test: Create new file with hTemplateFile != NULL
    // This test must be failed with ERROR_INVALID_PARAMETER since
    // we don't support hTemplateFile
    //
    FileHandle = CreateFile(
                    FileName,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,   // No SECURITY_ATTRIBUTES
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    INVALID_HANDLE_VALUE
                    );

    xLog(
        LogHandle,
        FileHandle == INVALID_HANDLE_VALUE ? XLL_PASS: XLL_FAIL,
        "CreateFile with hTemplateFile, no template (LastError=%d)",
        GetLastError()
        );

    if ( FileHandle != INVALID_HANDLE_VALUE ) {
        CloseHandle( FileHandle );
    }

    OutputDebugString( TEXT("SAMPLEDLL: Test 1 is terminating\n") );
    xEndVariation( LogHandle );
}


VOID
RunTest2(
    IN HANDLE LogHandle
    )
{
    LPVOID pv;

    xSetComponent( LogHandle, "Harness", "SampleDll" );
    xSetFunctionName( LogHandle, "Test2" );
    xStartVariation( LogHandle, "Test2" );
    OutputDebugString( TEXT("SAMPLEDLL: Test 2 is running\n") );

    pv = HeapAlloc( SampleDllHeapHandle, HEAP_ZERO_MEMORY, 4096 );

    if ( !pv ) {
        OutputDebugString( TEXT("SAMPLEDLL: Unable to allocate heap\n") );
        xLog( LogHandle, XLL_FAIL, "HeapAlloc failed" );
    } else {
        xLog( LogHandle, XLL_PASS, "HeapAlloc passed" );
        HeapFree( SampleDllHeapHandle, 0, pv );
    }

    OutputDebugString( TEXT("SAMPLEDLL: Test 2 is terminating\n") );
    xEndVariation( LogHandle );
}


VOID
RunTest3(
    IN HANDLE LogHandle,
    ULONG ID
    )
{
    char Buffer[80];

    sprintf( Buffer, "Test%d", ID );
    xSetComponent( LogHandle, "Harness", "SampleDll" );
    xSetFunctionName( LogHandle, "Test3" );
    xStartVariation( LogHandle, Buffer );
    sprintf( Buffer, "SAMPLEDLL: Test %d is running\n", ID );
    OutputDebugStringA( Buffer );
    Sleep(250);
    xEndVariation( LogHandle );
}


VOID
WINAPI
SampleStartTest(
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
    LONG ID;

    OutputDebugString( TEXT("SAMPLEDLL: StartTest is called\n") );

    //
    // One way to run multiple threads without creating threads is to
    // set NumberOfThreads=n under the section [sample.dll]. Everytime
    // the harness calls StartTest, SampleDllThreadID will get increment by one
    // after InterlockedIncrement call below. If no NumberOfThreads
    // specified, only RunTest1 will be run
    // 

    ID = InterlockedIncrement( &SampleDllThreadID );

    switch ( SampleDllThreadID ) {

    case 1:
        RunTest1( LogHandle );
        break;

    case 2:
        RunTest2( LogHandle );
        break;

    default:
        RunTest3( LogHandle, ID );
        break;
    }
}


VOID
WINAPI
SampleEndTest(
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
    SampleDllThreadID = 0;
    OutputDebugString( TEXT("SAMPLEDLL: EndTest is called\n") );
}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( sampledll )
#pragma data_seg()

BEGIN_EXPORT_TABLE( sampledll )
    EXPORT_TABLE_ENTRY( "StartTest", SampleStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", SampleEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", SampleDllMain )
END_EXPORT_TABLE( sampledll )
