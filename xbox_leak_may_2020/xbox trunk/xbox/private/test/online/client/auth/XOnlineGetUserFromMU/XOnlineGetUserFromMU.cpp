/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineGetUserFromMU.cpp
 *  Content:    XOnlineGetUserFromMU tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *   1/ 8/01    davidhef Created to test Xbox XOnlineGetUserFromMU
 *
 ****************************************************************************/

#include "XOnlineGetUserFromMU.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  XOnlineGetUserFromMU_BasicTest
 *
 *  Description:
 *      Basic Test for XOnlineGetUserFromMU
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineGetUserFromMU_BasicTest( void )
{
    HRESULT             hr              = S_OK;

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineGetUserFromMU_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlineGetUserFromMU tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineGetUserFromMU_StartTest( HANDLE LogHandle )
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
    // Test XOnlineGetUserFromMU in the mannor it was meant to be called
    //

    if ( SHOULDRUNTEST( "XOnlineGetUserFromMU", "Basic" ) )
    {
        SETLOG( LogHandle, "davidhef", "Online", "XOnlineGetUserFromMU", "Basic" );
        EXECUTE( XOnlineGetUserFromMU_BasicTest() );
    }

}

/****************************************************************************
 *
 *  XOnlineGetUserFromMU_EndTest
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

VOID WINAPI XOnlineGetUserFromMU_EndTest( VOID )
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
    XOnlineGetUserFromMU_StartTest( NULL );
    XOnlineGetUserFromMU_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineGetUserFromMU )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineGetUserFromMU )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineGetUserFromMU_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineGetUserFromMU_EndTest )
END_EXPORT_TABLE( XOnlineGetUserFromMU )
