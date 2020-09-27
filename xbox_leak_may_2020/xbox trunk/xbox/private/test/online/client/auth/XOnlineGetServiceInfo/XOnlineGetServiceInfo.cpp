/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineGetServiceInfo.cpp
 *  Content:    XOnlineGetServiceInfo tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/28/01    danrose Created to test Xbox XOnlineGetServiceInfo
 *
 ****************************************************************************/

#include "XOnlineGetServiceInfo.h"
#include "CXLiveConnectionManager.h"

using XLive::CXLiveConnectionManager;

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 * The Global online param check flag
 *
 ****************************************************************************/

extern BOOL g_ParamCheckReturnsError;

/****************************************************************************
 *
 * The Global array of services
 *
 ****************************************************************************/

static DWORD SERVICES[] = {
	XONLINE_MATCHMAKING_SERVICE,       
	XONLINE_BILLING_OFFERING_SERVICE     
};   

typedef struct
{
	DWORD	*pdwServices;
	DWORD	cServices;
	HRESULT	expectedHR[3];
} SERVICE_COMBO;

static SERVICE_COMBO SERVICE_COMBOS[] = {
	{ SERVICES + 0, 1, { S_OK } },
	{ SERVICES + 1, 1, { S_OK } },
	{ SERVICES + 0, 2, { S_OK, S_OK } }	// Test case asking for all valid services
};

/****************************************************************************
 *
 *  XOnlineGetServiceInfo_X_SERVICE_HANDLE_HAS_INTERNET 
 *
 *	Description:
 *		Get service info on each available service
 *
 *	Input: 
 *		ServiceHandle - each of the valid service handle values
 *
 *	System State:
 *		Connected to Internet: TRUE
 *
 *	Verify:
 *		Return value is not NULL
 *		Return value has valid data for each service
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineGetServiceInfo_X_SERVICE_HANDLE_HAS_INTERNET( void )
{
    HRESULT             hr              = S_OK;

	DWORD				dwNumUsers		= 0;
	DWORD				dwWorkFlags		= 0;

	XONLINE_USER		aUsers[XONLINE_MAX_STORED_ONLINE_USERS];
	XONLINE_SERVICE_INFO serviceInfo;

	//
	// start the network
	//

	CHECKRUN( StartNetwork() );

	//
	// zero out the users
	//

	ZeroMemory( aUsers, sizeof( aUsers ) );

	// populate the user list
    CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) aUsers, XONLINE_MAX_HD_ONLINE_USERS ) );
    dwNumUsers = XONLINE_MAX_HD_ONLINE_USERS;

	//
	// go through the different combinations of services
	//

	for ( DWORD j = 0; j < NUMELEMS( SERVICE_COMBOS ) && SUCCEEDED( hr ); j++ )
	{
		//
		// logon
		//

        if (SUCCEEDED(hr))
        {
            hr = CXLiveConnectionManager::Instance().SelectServices(SERVICE_COMBOS[j].pdwServices, SERVICE_COMBOS[j].cServices);
        }

        if (SUCCEEDED(hr))
        {
            hr = CXLiveConnectionManager::Instance().Logon(aUsers);
        }

        if (SUCCEEDED(hr))
        {
            hr = CXLiveConnectionManager::Instance().WaitForConnection();
            hr = (XONLINE_S_LOGON_CONNECTION_ESTABLISHED == hr) ? S_OK : E_FAIL;
        }

		//
		// go through each serice, and verify
		//

		for ( DWORD k = 0; k < SERVICE_COMBOS[j].cServices && SUCCEEDED( hr ); k++ )
		{
			//
			// make sure the hr returned is as expected
			//

			hr = ( hr == SERVICE_COMBOS[j].expectedHR[k] ) ? S_OK : E_FAIL;

	        //
	        // zero out the serviceInfo
	        //

	        ZeroMemory( (PVOID) &serviceInfo, sizeof( serviceInfo ) );

			//
			// get the service info
			//

            if (SUCCEEDED(hr))
            {
                DWORD dwServiceID = *(SERVICE_COMBOS[ j ].pdwServices + k);
                hr = XOnlineGetServiceInfo(dwServiceID, &serviceInfo);
            }

			//
			// verify the service info
			//

			DbgPrint( "%u %u %u %u\n", serviceInfo.dwServiceID, serviceInfo.serviceIP, serviceInfo.wServicePort, serviceInfo.wReserved );
		}

		//
		// Logoff
		//
        if (SUCCEEDED(hr))
        {
            hr = CXLiveConnectionManager::Instance().Logoff();
        }
	}

	//
	// close the network
	//

	CloseNetwork();

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineGetServiceInfo_INVALID_SERVICE_HANDLE
 *
 *	Description:
 *		Make sure Get Service Info can handle invalid handle
 *
 *	Input: 
 *		ServiceHandle - XONLINE_INVALID_SERVICE
 *
 *	System State:
 *		Connected to Internet: N/A
 *
 *	Verify:
 *		Return value is NULL
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineGetServiceInfo_INVALID_SERVICE_HANDLE( void )
{
    HRESULT             hr              = S_OK;
    XONLINE_SERVICE_INFO serviceInfo;

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// start the network
	//

	CHECKRUN( StartNetwork() );

	//
	// call Get service info with an invalid service
	//

	CHECKRUN( XOnlineGetServiceInfo( (DWORD)999, &serviceInfo ) );
    hr = (SUCCEEDED(hr)) ? E_FAIL : S_OK;

	//
	// clear the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

	//
	// close the network
	//

	CloseNetwork();

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineGetServiceInfo_NULL_SERVICE_HANDLE
 *
 *	Description:
 *		Make sure Get Service Info can handle NULL handle
 *
 *	Input: 
 *		ServiceHandle - NULL
 *
 *	System State:
 *		Connected to Internet: N/A
 *
 *	Verify:
 *		Return value is NULL
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineGetServiceInfo_NULL_SERVICE_HANDLE( void )
{
    HRESULT             hr              = S_OK;

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// start the network
	//

	CHECKRUN( StartNetwork() );

	//
	// call Get service info with an invalid service
	//

	CHECKRUN( XOnlineGetServiceInfo( (DWORD)999, NULL ) );
    hr = (SUCCEEDED(hr)) ? E_FAIL : S_OK;

    //
	// clear the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

	//
	// close the network
	//

	CloseNetwork();

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineGetServiceInfo_StartTest
 *
 *  Description:
 *      The entry function for the test harness
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineGetServiceInfo_StartTest( HANDLE LogHandle )
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
    // Test XOnlineGetServiceInfo with all the service handles and internet
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlineGetServiceInfo", "X_SERVICE_HANDLE_HAS_INTERNET" );
    EXECUTE( XOnlineGetServiceInfo_X_SERVICE_HANDLE_HAS_INTERNET() );

    if (!BVTMode())
    {
        //
        // Test XOnlineGetServiceInfo with an invalid service handle
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineGetServiceInfo", "INVALID_SERVICE_HANDLE" );
        EXECUTE( XOnlineGetServiceInfo_INVALID_SERVICE_HANDLE() );
        
        //
        // Test XOnlineGetServiceInfo with a NULL service handle
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineGetServiceInfo", "NULL_SERVICE_HANDLE" );
        EXECUTE( XOnlineGetServiceInfo_NULL_SERVICE_HANDLE() );
    }
}

/****************************************************************************
 *
 *  XOnlineGetServiceInfo_EndTest
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

VOID WINAPI XOnlineGetServiceInfo_EndTest( VOID )
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
    XOnlineGetServiceInfo_StartTest( NULL );
    XOnlineGetServiceInfo_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineGetServiceInfo )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineGetServiceInfo )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineGetServiceInfo_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineGetServiceInfo_EndTest )
END_EXPORT_TABLE( XOnlineGetServiceInfo )
