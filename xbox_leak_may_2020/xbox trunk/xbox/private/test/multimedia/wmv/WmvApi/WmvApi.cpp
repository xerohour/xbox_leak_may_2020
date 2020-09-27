/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    WmvApi.cpp

Abstract:

    WmvApi tests

--*/

#include "WmvApi.h"
#include "WmvApiTest.h"

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
WmvApi_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for WmvApi

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT             hr              = S_OK;

	CWmvApiTest			ApiTest;
	EXECUTE( ApiTest.Run() );

    //
    // Return the Hresult
    //
	XLaunchNewImage( NULL, NULL );

    return hr;
}

VOID 
WINAPI 
WmvApi_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the WmvApi tests

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
    // Test WmvApi in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "jeffsul", "MCPX", "WmvApi", "Basic" );
    EXECUTE( WmvApi_BasicTest() );

}

VOID 
WINAPI 
WmvApi_EndTest( 
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
    WmvApi_StartTest( NULL );
    WmvApi_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( WmvApi )
#pragma data_seg()

BEGIN_EXPORT_TABLE( WmvApi )
    EXPORT_TABLE_ENTRY( "StartTest", WmvApi_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", WmvApi_EndTest )
END_EXPORT_TABLE( WmvApi )
