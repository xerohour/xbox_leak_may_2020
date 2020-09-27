/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    WmvMemory.cpp

Abstract:

    WmvMemory tests

--*/

#include "WmvMemory.h"
#include "WmvMemoryTest.h"

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
WmvMemory_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for WmvMemory

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT             hr              = S_OK;

	CWmvMemTest theTest;
	EXECUTE( theTest.Run() );
    //
    // Return the Hresult
    //

    return hr;
}

VOID 
WINAPI 
WmvMemory_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the WmvMemory tests

Arguments:

    LogHandle - a handle to a logging object

Return Value:

    None

--*/
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

    //
    // Test WmvMemory in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "jeffsul", "MCPX", "WmvMemory", "Basic" );
    EXECUTE( WmvMemory_BasicTest() );

#ifdef NOLOGGING
	XLaunchNewImage( NULL, NULL );
#endif

}

VOID 
WINAPI 
WmvMemory_EndTest( 
    VOID 
    )
/*++

Routine Description:

    The exit function for the test harness

Arguments:

    None

Return Value:

    None

--*/
{
}

#ifdef NOLOGGING
void 
__cdecl 
main( 
    void 
    )
/*++

Routine Description:

    the exe entry point

Arguments:

    None

Return Value:

    None

--*/
{
    WmvMemory_StartTest( NULL );
    WmvMemory_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( WmvMemory )
#pragma data_seg()

BEGIN_EXPORT_TABLE( WmvMemory )
    EXPORT_TABLE_ENTRY( "StartTest", WmvMemory_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", WmvMemory_EndTest )
END_EXPORT_TABLE( WmvMemory )
