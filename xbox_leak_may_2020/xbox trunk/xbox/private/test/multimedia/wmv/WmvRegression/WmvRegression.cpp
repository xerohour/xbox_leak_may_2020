/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    WmvRegression.cpp

Abstract:

    WmvRegression tests

--*/

#include "WmvRegression.h"
#include "WmvRegressionTest.h"

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
WmvRegression_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for WmvRegression

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT             hr              = S_OK;

	CWmvRegTest		theTest;
	EXECUTE( theTest.Run() );

#ifdef NOLOGGING
	XLaunchNewImage( NULL, NULL );
#endif

    //
    // Return the Hresult
    //

    return hr;
}

VOID 
WINAPI 
WmvRegression_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the WmvRegression tests

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
    // Test WmvRegression in the mannor it was meant to be called
    //

    if ( SHOULDRUNTEST( "WmvRegression", "Basic" ) )
    {
        SETLOG( LogHandle, "jeffsul", "Online", "WmvRegression", "Basic" );
        EXECUTE( WmvRegression_BasicTest() );
    }

}

VOID 
WINAPI 
WmvRegression_EndTest( 
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
    WmvRegression_StartTest( NULL );
    WmvRegression_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( WmvRegression )
#pragma data_seg()

BEGIN_EXPORT_TABLE( WmvRegression )
    EXPORT_TABLE_ENTRY( "StartTest", WmvRegression_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", WmvRegression_EndTest )
END_EXPORT_TABLE( WmvRegression )
