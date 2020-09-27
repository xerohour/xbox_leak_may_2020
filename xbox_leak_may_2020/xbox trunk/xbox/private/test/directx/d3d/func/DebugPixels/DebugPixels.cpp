/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    DebugPixels.cpp

Abstract:

    DebugPixels tests

--*/

#include "DebugPixels.h"
#include "DebugPixelsTest.h"

//
// The Global Logging Handle
//

extern HANDLE g_hLog;

HRESULT
DebugPixels_BasicTest(
    void
    )
/*++

Routine Description:

    Basic Test for DebugPixels

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT             hr              = S_OK;

	CDbgPixelsTest	theTest;
	EXECUTE( theTest.Run() );

#ifdef NOLOGGING
	XLaunchNewImage( NULL, NULL );
#endif NOLOGGING

    //
    // Return the Hresult
    //

    return hr;
}

VOID 
WINAPI 
DebugPixels_StartTest( 
    HANDLE              LogHandle 
    )
/*++

Routine Description:

    The harness entry into the DebugPixels tests

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
    // Test DebugPixels in the mannor it was meant to be called
    //

    if ( SHOULDRUNTEST( "DebugPixels", "Basic" ) )
    {
        SETLOG( LogHandle, "jeffsul", "Online", "DebugPixels", "Basic" );
        EXECUTE( DebugPixels_BasicTest() );
    }

}

VOID 
WINAPI 
DebugPixels_EndTest( 
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
    DebugPixels_StartTest( NULL );
    DebugPixels_EndTest();
}
#endif // NOLOGGING

//
// Export Function Pointers for StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( DebugPixels )
#pragma data_seg()

BEGIN_EXPORT_TABLE( DebugPixels )
    EXPORT_TABLE_ENTRY( "StartTest", DebugPixels_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DebugPixels_EndTest )
END_EXPORT_TABLE( DebugPixels )
