/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    XFApi.cpp

Abstract:

    XFApi tests

--*/

#include "XFApi.h"
#include "XFApiTest.h"

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
XFApi_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for XFApi

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT             hr              = S_OK;

	CXFApiTest			theTest;

	EXECUTE( theTest.Run() );
    //
    // Return the Hresult
    //

    return hr;
}

VOID 
WINAPI 
XFApi_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the XFApi tests

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
    // Test XFApi in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "jeffsul", "MCPX", "XFApi", "Basic" );
    EXECUTE( XFApi_BasicTest() );

}

VOID 
WINAPI 
XFApi_EndTest( 
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
#ifdef NOLOGGING
	XLaunchNewImage( NULL, NULL );
#endif
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
    XFApi_StartTest( NULL );
    XFApi_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XFApi )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XFApi )
    EXPORT_TABLE_ENTRY( "StartTest", XFApi_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XFApi_EndTest )
END_EXPORT_TABLE( XFApi )
