/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    WmvPerf.cpp

Abstract:

    WmvPerf tests

--*/

#include "WmvPerf.h"
#include "WmvPerfTest.h"

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
WmvPerf_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for WmvPerf

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT             hr              = S_OK;

	CWmvPerfTest	theTest;
	hr = theTest.Run();

	XLaunchNewImage( NULL, NULL );

    //
    // Return the Hresult
    //

    return hr;
}

VOID 
WINAPI 
WmvPerf_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the WmvPerf tests

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
    // Test WmvPerf in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "jeffsul", "MCPX", "WmvPerf", "Basic" );
    EXECUTE( WmvPerf_BasicTest() );

}

VOID 
WINAPI 
WmvPerf_EndTest( 
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
    WmvPerf_StartTest( NULL );
    WmvPerf_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( WmvPerf )
#pragma data_seg()

BEGIN_EXPORT_TABLE( WmvPerf )
    EXPORT_TABLE_ENTRY( "StartTest", WmvPerf_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", WmvPerf_EndTest )
END_EXPORT_TABLE( WmvPerf )
