/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineStartup.cpp
 *  Content:    XOnlineStartup tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *   1/14/01    davidhef Created to test Xbox XOnlineStartup
 *
 ****************************************************************************/

#include "XOnlineStartup.h"
#include <XOnline.h>

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  XOnlineStartup_BasicTest
 *
 *  Description:
 *      Basic Test for XOnlineStartup
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineStartup_BASIC_TEST( void )
{
    HRESULT             hr              = S_OK;
    DWORD               dwResult = 0;
	XNADDR				xnaddr;
    const int spinCount = BVTMode() ? 1 : 10;
    
    for (int i = 0; i < spinCount && SUCCEEDED(hr); ++i)
    {
        hr = XOnlineStartup( NULL );
        if (SUCCEEDED(hr))
        {
            //
            // wait for DNS to succeed
            //
            do
            {
                dwResult = XNetGetTitleXnAddr( &xnaddr );  	
            } 
            while ( XNET_GET_XNADDR_PENDING == dwResult );
            
            hr = XOnlineCleanup();
        }
    }
    
    return hr;
}

HRESULT XOnlineStartup_NO_CLEANUP( void )
{
    HRESULT             hr              = S_OK;
    DWORD               dwResult = 0;
	XNADDR				xnaddr;
    const int spinCount = BVTMode() ? 1 : 10;
    
    for (int i = 0; i < spinCount && SUCCEEDED(hr); ++i)
    {
        hr = XOnlineStartup( NULL );
        if (SUCCEEDED(hr))
        {
            //
            // wait for DNS to succeed
            //
            do
            {
                dwResult = XNetGetTitleXnAddr( &xnaddr );  	
            } 
            while ( XNET_GET_XNADDR_PENDING == dwResult );
        }
    }
    
    if (SUCCEEDED(hr))
    {
        for (int i = 0; i < spinCount && SUCCEEDED(hr); ++i)
        {
            hr = XOnlineCleanup();
        }
    }

    return hr;
}

HRESULT XOnlineStartup_NO_STARTUP( void )
{
    HRESULT             hr              = S_OK;
    DWORD               dwResult = 0;
    XNADDR				xnaddr;
    const int spinCount = BVTMode() ? 1 : 10;

    for (int i = 0; i < spinCount && SUCCEEDED(hr); ++i)
    {
        //
        // wait for DNS to succeed
        //
        dwResult = XNetGetTitleXnAddr( &xnaddr );
        hr = (XNET_GET_XNADDR_NONE == dwResult) ? S_OK : E_FAIL;
        
        if (SUCCEEDED(hr))
        {
            hr = XOnlineCleanup();
            hr = (SUCCEEDED(hr)) ? E_FAIL : S_OK;
        }
    }
    
    return hr;
}

/****************************************************************************
 *
 *  XOnlineStartup_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlineStartup tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineStartup_StartTest( HANDLE LogHandle )
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
    // Test XOnlineStartup in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "davidhef", "Online", "XOnlineStartup", "BASIC_TEST" );
    EXECUTE( XOnlineStartup_BASIC_TEST() );

    if (!BVTMode())
    {
        SETLOG( LogHandle, "davidhef", "Online", "XOnlineStartup", "NO_CLEANUP" );
        EXECUTE( XOnlineStartup_NO_CLEANUP() );
        
        SETLOG( LogHandle, "davidhef", "Online", "XOnlineStartup", "NO_STARTUP" );
        EXECUTE( XOnlineStartup_NO_STARTUP() );
    }
}

/****************************************************************************
 *
 *  XOnlineStartup_EndTest
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

VOID WINAPI XOnlineStartup_EndTest( VOID )
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
    XOnlineStartup_StartTest( NULL );
    XOnlineStartup_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineStartup )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineStartup )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineStartup_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineStartup_EndTest )
END_EXPORT_TABLE( XOnlineStartup )
