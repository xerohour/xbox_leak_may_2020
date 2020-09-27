/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    XFFunc.cpp

Abstract:

    XFFunc tests

--*/

#include "XFFunc.h"
#include "XFFuncTest.h"

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
XFFunc_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for XFFunc

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
	CXFFuncTest theTest;
	EXECUTE( theTest.Run() );

    return hr;
}

VOID 
WINAPI 
XFFunc_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the XFFunc tests

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
    // Test XFFunc in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "jeffsul", "MCPX", "XFFunc", "Basic" );
    EXECUTE( XFFunc_BasicTest() );

}

VOID 
WINAPI 
XFFunc_EndTest( 
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
#endif NOLOGGING
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
    XFFunc_StartTest( NULL );
    XFFunc_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XFFunc )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XFFunc )
    EXPORT_TABLE_ENTRY( "StartTest", XFFunc_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XFFunc_EndTest )
END_EXPORT_TABLE( XFFunc )
