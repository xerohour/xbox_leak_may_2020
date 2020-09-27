/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    wmvfunc.cpp

Abstract:

    wmvfunc tests

--*/

#include "wmvfunc.h"
#include "WMVTest.h"

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
wmvfunc_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for wmvfunc

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT             hr              = S_OK;

	CWMVTest	WMVTest;
	EXECUTE( WMVTest.Run() );

	XLaunchNewImage( NULL, NULL );

    //
    // Return the Hresult
    //

    return hr;
}

VOID 
WINAPI 
wmvfunc_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the wmvfunc tests

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
    // Test wmvfunc in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "jeffsul", "MCPX", "wmvfunc", "Basic" );
    EXECUTE( wmvfunc_BasicTest() );

}

VOID 
WINAPI 
wmvfunc_EndTest( 
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
    wmvfunc_StartTest( NULL );
    wmvfunc_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( wmvfunc )
#pragma data_seg()

BEGIN_EXPORT_TABLE( wmvfunc )
    EXPORT_TABLE_ENTRY( "StartTest", wmvfunc_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", wmvfunc_EndTest )
END_EXPORT_TABLE( wmvfunc )
