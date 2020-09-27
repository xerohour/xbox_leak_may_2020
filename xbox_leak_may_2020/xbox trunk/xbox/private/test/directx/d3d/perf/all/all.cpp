/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       all.cpp
 *  Content:    all tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  04/20/01    jgould Created to test Xbox all
 *
 ****************************************************************************/

#include "all.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  all_BasicTest
 *
 *  Description:
 *      Basic Test for all
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT all_BasicTest( void )
{
    HRESULT             hr              = S_OK;

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  all_StartTest
 *
 *  Description:
 *      The Harness Entry into the all tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI all_StartTest( HANDLE LogHandle )
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
    // Test all in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "jgould", "Core (s-d3d)", "all", "Basic" );
    EXECUTE( all_BasicTest() );

}

/****************************************************************************
 *
 *  all_EndTest
 *
 *  Description:
 *      The Exit function for the test harness
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI all_EndTest( VOID )
{
}

/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      the exe entry point
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef NOLOGGING
void __cdecl main( void )
{
    all_StartTest( NULL );
    all_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( all )
#pragma data_seg()

BEGIN_EXPORT_TABLE( all )
    EXPORT_TABLE_ENTRY( "StartTest", all_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", all_EndTest )
END_EXPORT_TABLE( all )
