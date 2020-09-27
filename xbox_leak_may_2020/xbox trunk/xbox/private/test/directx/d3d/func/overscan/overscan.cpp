/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    overscan.cpp

Abstract:

    overscan tests

--*/

#include "overscan.h"
#include "OverScanTest.h"

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
overscan_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for overscan

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT             hr              = S_OK;

	COverScanTest	theTest;
	EXECUTE( theTest.Run() );

    //
    // Return the Hresult
    //

    return hr;
}

VOID 
WINAPI 
overscan_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the overscan tests

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
    // Test overscan in the mannor it was meant to be called
    //

    if ( SHOULDRUNTEST( "overscan", "Basic" ) )
    {
        SETLOG( LogHandle, "jeffsul", "Online", "overscan", "Basic" );
        EXECUTE( overscan_BasicTest() );
    }

}

VOID 
WINAPI 
overscan_EndTest( 
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
    overscan_StartTest( NULL );
    overscan_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( overscan )
#pragma data_seg()

BEGIN_EXPORT_TABLE( overscan )
    EXPORT_TABLE_ENTRY( "StartTest", overscan_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", overscan_EndTest )
END_EXPORT_TABLE( overscan )
