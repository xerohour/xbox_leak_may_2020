/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    WmvVerification.cpp

Abstract:

    WmvVerification tests

--*/

#include "WmvVerification.h"
#include "WmvVerificationTest.h"

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
WmvVerification_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for WmvVerification

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT             hr              = S_OK;

    //
    // Return the Hresult
    //
	CWmvVerificationTest theTest;
	EXECUTE( theTest.Run() );

	XLaunchNewImage( NULL, NULL );

    return hr;
}

VOID 
WINAPI 
WmvVerification_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the WmvVerification tests

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
    // Test WmvVerification in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "jeffsul", "MCPX", "WmvVerification", "Basic" );
    EXECUTE( WmvVerification_BasicTest() );

}

VOID 
WINAPI 
WmvVerification_EndTest( 
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
    WmvVerification_StartTest( NULL );
    WmvVerification_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( WmvVerification )
#pragma data_seg()

BEGIN_EXPORT_TABLE( WmvVerification )
    EXPORT_TABLE_ENTRY( "StartTest", WmvVerification_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", WmvVerification_EndTest )
END_EXPORT_TABLE( WmvVerification )
