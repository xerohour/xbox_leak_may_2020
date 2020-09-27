/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineLogon.cpp
 *  Content:    XOnlineLogon tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/28/01    danrose Created to test Xbox XOnlineLogon
 *
 ****************************************************************************/

#include "XOnlineLogon.h"
#include <CXLiveConnectionManager.h>
#include <CXLiveUserManager.h>

#include <debug.h>

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 * The Global array of services
 *
 ****************************************************************************/

static DWORD SERVICES[][3] = {
	{ XONLINE_MATCHMAKING_SERVICE },       
	{ XONLINE_BILLING_OFFERING_SERVICE },        
	{ XONLINE_MATCHMAKING_SERVICE, XONLINE_BILLING_OFFERING_SERVICE }
};   

static const DWORD NUM_SERVICES[] = {
	1,
	1,
	2
};

/****************************************************************************
 *
 * The Global online param check flag
 *
 ****************************************************************************/

extern BOOL g_ParamCheckReturnsError;


using XLive::CXLiveConnectionManager;	
using XLive::CXLiveUserManager;
using XLive::CXLiveUserManager::MUAccountIterator;
using XLive::CXLiveUserManager::HDAccountIterator;
using XLive::CXLiveUserManager::AccountIterator;

/****************************************************************************
 *
 * Use XOnlineLogon the way we expect it to be used in most cases.
 *
 ****************************************************************************/
HRESULT XOnlineLogon_BASIC_TEST( void )
{
    HRESULT             hr              = S_OK;
    DWORD				dwUsers		= 0;
    DWORD               dwMUs = 0;
    XONLINE_USER		users[XONLINE_MAX_STORED_ONLINE_USERS];
    XONLINE_SERVICE_INFO serviceInfo;
    bool    bNetworkStarted = false;
    
    if (SUCCEEDED(hr))
    {
        hr = StartNetwork();
        bNetworkStarted = (S_OK == hr) ? true : false;
    }
    
    if (SUCCEEDED(hr))
    {
        // Cause PopulateUserAccountsHD to generate random user accounts.
        ZeroMemory(users, sizeof(users));
        
        // Delete any existing user accounts on the HD before populating.
        hr = COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) users, XONLINE_MAX_HD_ONLINE_USERS, true );
    }
    
    if (SUCCEEDED(hr))
    {
        // Create an account in every MU.
        hr = CXLiveUserManager::PopulateMUs(&dwMUs);
        hr = (SUCCEEDED(hr) && dwMUs > 0) ? S_OK : E_FAIL;
    }
        
    if (SUCCEEDED(hr))
    {
        hr = CXLiveConnectionManager::Instance().SelectServices(    XONLINE_MATCHMAKING_SERVICE,
                                                                    XONLINE_BILLING_OFFERING_SERVICE,
                                                                    XONLINE_STATISTICS_SERVICE);
    }
    
    // Get the users.
    CHECKRUN(XOnlineGetUsers(users, &dwUsers));
    CHECKRUN((XONLINE_MAX_HD_ONLINE_USERS + dwMUs == dwUsers && SUCCEEDED(hr)) ? hr : E_FAIL);
    
    // Verify that there are the correct number of users.
    CXLiveUserManager userMgr(&users[0]);
    CHECKRUN((userMgr.CountMUAccounts() + userMgr.CountHDAccounts() == dwUsers && SUCCEEDED(hr)) ? hr : E_FAIL);
    CHECKRUN((userMgr.CountAccounts() == dwUsers) ? hr : E_FAIL);
    
    // Verify that there are no duplicates.
    CHECKRUN((userMgr.VerifyNoDuplicates()) ? S_OK : E_FAIL);
    
    // Clear account selections.
    CHECKRUN(userMgr.ClearAccountSelections());
    
    // Select a MU account to logon.
    if (SUCCEEDED(hr))
    {
        MUAccountIterator muIt = userMgr.MUAccount_begin();
        hr = (muIt != userMgr.MUAccount_end()) ? S_OK : E_FAIL;
        CHECKRUN(userMgr.SelectAccount(muIt));

        // Create a guest account to logon.
        CHECKRUN(userMgr.CreateAndSelectGuestAccount(muIt, 1));
    }
    
    // Select some HD accounts to logon.
    if (SUCCEEDED(hr))
    {
        int count = 2;              // We already selected a MU account and a guest account.
        HDAccountIterator hdIt = userMgr.HDAccount_begin();
        for (; hdIt != userMgr.HDAccount_end() && count < XONLINE_MAX_LOGON_USERS && SUCCEEDED(hr); ++hdIt, ++count)
        {
            hr = userMgr.SelectAccount(hdIt);
        }
    }
    
    // Logon the selected accounts.
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection(userMgr.SelectedAccounts(), (DWORD) 5000));
    hr = (XONLINE_S_LOGON_CONNECTION_ESTABLISHED == hr) ? S_OK : E_FAIL;

    // Verify successful logon.
    PXONLINE_USER pLoggedOnUsers = NULL;
    if (SUCCEEDED(hr))
    {
        pLoggedOnUsers = XOnlineGetLogonUsers();
        hr = (pLoggedOnUsers == NULL) ? E_FAIL : S_OK;
    }

    if (SUCCEEDED(hr))
    {
        hr = userMgr.VerifyLogon(pLoggedOnUsers);
    }

    // Log off.
    CHECKRUN(CXLiveConnectionManager::Instance().Logoff());

    if (bNetworkStarted)
    {
        HRESULT closedHr = CloseNetwork();
        hr = (FAILED(closedHr)) ? closedHr : hr;
        bNetworkStarted = false;
    }
    
    return hr;
}

HRESULT XOnlineLogon_CORNER_CASES( void )
{
    HRESULT             hr              = S_OK;
    DWORD				dwUsers		= 0;
    DWORD               dwMUs = 0;
    XONLINE_USER		users[XONLINE_MAX_STORED_ONLINE_USERS];
    XONLINE_SERVICE_INFO serviceInfo;
    bool    bNetworkStarted = false;
    
    if (SUCCEEDED(hr))
    {
        hr = StartNetwork();
        bNetworkStarted = (S_OK == hr) ? true : false;
    }
    
    if (SUCCEEDED(hr))
    {
        // Cause PopulateUserAccountsHD to generate random user accounts.
        ZeroMemory(users, sizeof(users));
        
        // Delete any existing user accounts on the HD before populating.
        hr = COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) users, XONLINE_MAX_HD_ONLINE_USERS, true );
    }
    
    if (SUCCEEDED(hr))
    {
        // Create an account in every MU.
        hr = CXLiveUserManager::PopulateMUs(&dwMUs);
        hr = (SUCCEEDED(hr) && dwMUs > 0) ? S_OK : E_FAIL;
    }
        
    CXLiveUserManager userMgr(&users[0]);

    AccountIterator accountIt = userMgr.Account_begin();
    hr = (accountIt == userMgr.Account_end()) ? E_FAIL : S_OK;

    // Try to logon with a non-existent account.
    // Expected: It should fail.
    XONLINE_USER badUser[XONLINE_MAX_STORED_ONLINE_USERS];
    ZeroMemory(badUser, sizeof(badUser));
    badUser[0].xuid.qwUserID = (ULONGLONG) 987654321;
    (void) strcat(badUser[0].name, "BadUser");
    CHECKRUN(userMgr.ClearAccountSelections());
    CHECKRUN(CXLiveConnectionManager::Instance().SelectServices(    XONLINE_MATCHMAKING_SERVICE,
                                                                    XONLINE_STATISTICS_SERVICE,
                                                                    XONLINE_BILLING_OFFERING_SERVICE));
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection(badUser));
    hr = (SUCCEEDED(hr)) ? E_FAIL : hr;

    // Try to logon with the same service twice.
    // Expected: It should fail.
    CHECKRUN(CXLiveConnectionManager::Instance().SelectServices(    XONLINE_MATCHMAKING_SERVICE,
                                                                    XONLINE_BILLING_OFFERING_SERVICE,
                                                                    XONLINE_BILLING_OFFERING_SERVICE));
    CHECKRUN(userMgr.ClearAccountSelections());
    CHECKRUN(userMgr.SelectAccount(accountIt));
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection(userMgr.SelectedAccounts()));
    hr = (SUCCEEDED(hr)) ? E_FAIL : hr;

    // Try to logon with 3 guest accounts, followed
    // by their legitimate master.  This case should
    // SUCCEED.
    CHECKRUN(userMgr.ClearAccountSelections());
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 3));
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 1));
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 2));
    CHECKRUN(userMgr.SelectAccount(accountIt));
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection(userMgr.SelectedAccounts()));
    hr = (XONLINE_S_LOGON_CONNECTION_ESTABLISHED == hr) ? S_OK : E_FAIL;
    CHECKRUN(CXLiveConnectionManager::Instance().Logoff());

    // Try to logon with 3 guest accounts, followed
    // by their legitimate master and no services.  This case should
    // SUCCEED.
    CHECKRUN(userMgr.ClearAccountSelections());
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 3));
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 1));
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 2));
    CHECKRUN(userMgr.SelectAccount(accountIt));
    CHECKRUN(CXLiveConnectionManager::Instance().SelectNoServices());
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection(userMgr.SelectedAccounts()));
    hr = (XONLINE_S_LOGON_CONNECTION_ESTABLISHED == hr) ? S_OK : E_FAIL;
    CHECKRUN(CXLiveConnectionManager::Instance().Logoff());

    // Try to logon with a guest account that has no master account.
    // Expected: It should fail, since this is not allowed.
    CHECKRUN(userMgr.ClearAccountSelections());
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 1));
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection(userMgr.SelectedAccounts()));
    hr = (SUCCEEDED(hr)) ? E_FAIL : hr;

    // Try to logon with all guest accounts.
    // Expected: It should fail, since there must be at least
    // one real account.
    CHECKRUN(CXLiveConnectionManager::Instance().SelectServices(    XONLINE_MATCHMAKING_SERVICE,
                                                                    XONLINE_BILLING_OFFERING_SERVICE,
                                                                    XONLINE_STATISTICS_SERVICE));
    CHECKRUN(userMgr.ClearAccountSelections());
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 2));
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 1));
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 3));
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection(userMgr.SelectedAccounts()));
    hr = (SUCCEEDED(hr)) ? E_FAIL : hr;

    // Try to logon with 2 guest accounts with the same guest number.
    // Expected: It should fail.
    CHECKRUN(CXLiveConnectionManager::Instance().SelectServices(    XONLINE_MATCHMAKING_SERVICE,
                                                                    XONLINE_BILLING_OFFERING_SERVICE,
                                                                    XONLINE_STATISTICS_SERVICE));
    CHECKRUN(userMgr.ClearAccountSelections());
    CHECKRUN(userMgr.SelectAccount(accountIt));                     // Select the master account.
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 3));
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 2));
    CHECKRUN(userMgr.CreateAndSelectGuestAccount(accountIt, 3));
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection(userMgr.SelectedAccounts()));
    hr = (SUCCEEDED(hr)) ? E_FAIL : hr;

    if (bNetworkStarted)
    {
        HRESULT closedHr = CloseNetwork();
        hr = (FAILED(closedHr)) ? closedHr : hr;
        bNetworkStarted = false;
    }
    
    return hr;
}


/****************************************************************************
 *
 *  XOnlineLogon_X_USERS_Y_SERVICES_HAS_INTERNET
 *
 *	Description:
 *		Perform Online logon with all combination of valid users and valid services
 *
 *	Input: 
 *		PUsers - valid arrays of users
 *		PServices - valid arrays of services
 *		CServices - the corresponding count of services
 *		Hevent - a valid event
 *		PHandle - a valid pointer
 *
 *	System State:
 *			Connected to Internet: TRUE
 *
 *	Verify:
 *		Return value is S_OK
 *		Value in pHandle (*pHandle) is valid
 *		Values in pServices are valid
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineLogon_X_USERS_Y_SERVICES_HAS_INTERNET( void )
{
    HRESULT             hr              = S_OK;
	DWORD				dwNumUsers		= 0;
	DWORD				dwWorkFlags		= 0;

	XONLINE_USER		aUsers[XONLINE_MAX_STORED_ONLINE_USERS];
	XONLINETASK_HANDLE	taskHandle = NULL; 
    XONLINE_SERVICE_INFO serviceInfo;

	HANDLE				hEvent = CreateEvent( NULL, TRUE, TRUE, NULL );

	CHECKRUN( StartNetwork() );

	if ( NULL == hEvent )
	{
		hr = E_FAIL;
	}


	//
	// cycle through the different combination of users
	//

	for ( DWORD i = 0; i < 0x0000000F && SUCCEEDED( hr ); i++ )
	{
		ZeroMemory( aUsers, sizeof( aUsers ) );

		// populate the user list
        CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) aUsers, XONLINE_MAX_HD_ONLINE_USERS ) );
        dwNumUsers = XONLINE_MAX_STORED_ONLINE_USERS;

		//
		// Zero out the proper users
		//

		for ( DWORD dwMask = 0x00000008, dwShift = 0; 0 != dwMask && SUCCEEDED( hr ); dwMask >>= 1, dwShift++ )
		{
			if ( i & dwMask )
			{
				ZeroMemory( aUsers + dwShift, sizeof( XONLINE_USER ) );
			}
		}

		//
		// go through the different services
		//

		for ( DWORD j = 0; j < NUMELEMS( SERVICES ) && SUCCEEDED( hr ); j++ )
		{
			//
			// logon
			//

			CHECKRUN( XOnlineLogon( aUsers, static_cast<DWORD*> (&(SERVICES[0][j])), NUM_SERVICES[j], hEvent, &taskHandle ) );

			//
			// make sure the taskhandle came back
			//

			if ( SUCCEEDED( hr ) )
			{
				hr = NULL == taskHandle ? E_FAIL : S_OK;
			}

            if (SUCCEEDED(hr))
            {
                hr = WaitForLogon(taskHandle, hEvent);
            }

            hr = (XONLINE_S_LOGON_CONNECTION_ESTABLISHED == hr) ? S_OK : E_FAIL;

            //
			// go through each serice, and verify
			//

			for ( DWORD k = 0; k < NUM_SERVICES[j] && SUCCEEDED( hr ); k++ )
			{
				//
				// make sure the hr returned is S_OK
				//

				CHECKRUN( XOnlineGetServiceInfo(SERVICES[j][k], &serviceInfo) );

				//
				// make sure the handle is good
				//

				if ( SUCCEEDED( hr ) && serviceInfo.dwServiceID != SERVICES[j][k] )
				{
					hr = E_FAIL;

				}
			}
            
            if (SUCCEEDED(hr) && taskHandle != NULL)
            {
                hr = XOnlineTaskClose( taskHandle );
			    taskHandle = NULL;
            }
		}
	}


	CHECKRUN( CloseNetwork() );

    //
	// close the handle
	//

	if ( SUCCEEDED(hr) && hEvent )
	{
		CloseHandle( hEvent );
		hEvent = NULL;
	}

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlineLogon_VALID_USER_VALID_SERVICE_NO_INTERNET
 *
 *	Description:
 *		Perform Online logon when no internet is available
 *
 *	Input: 
 *		PUsers - valid array of users
 *		PServices - valid array of services
 *		CServices - the corresponding count of services
 *		Hevent - a valid event
 *		PHandle - a valid pointer
 *
 *	System State:
 *			Connected to Internet: FALSE
 *
 *	Verify:
 *		Return value is E_XXXXX
 *		Value in pHandle (*pHandle) is NULL
 *		Values in pServices are not valid
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineLogon_VALID_USER_VALID_SERVICE_NO_INTERNET( void )
{
    HRESULT             hr              = S_OK;

	DWORD				dwNumUsers		= 0;
	DWORD				dwWorkFlags		= 0;

	XONLINE_USER		aUsers[XONLINE_MAX_STORED_ONLINE_USERS];
	XONLINETASK_HANDLE	taskHandle = NULL; 

	HANDLE				hEvent = CreateEvent( NULL, TRUE, TRUE, NULL );

	if ( NULL == hEvent )
	{
		hr = E_FAIL;
	}


	ZeroMemory( aUsers, sizeof( aUsers ) );

	// populate the user list
    CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) aUsers, XONLINE_MAX_HD_ONLINE_USERS ) );
    dwNumUsers = XONLINE_MAX_STORED_ONLINE_USERS;

	//
	// make sure the correct number were loaded
	//

	if ( XONLINE_MAX_STORED_ONLINE_USERS != dwNumUsers )
	{
		hr = E_FAIL;
	}

	CHECKRUN( SUCCEED_ON_FAIL( XOnlineLogon( aUsers, SERVICES[0], NUM_SERVICES[0], hEvent, &taskHandle ), static_cast<HRESULT>(0xc000005e) ) );


	if ( hEvent )
	{
		CloseHandle( hEvent );
		hEvent = NULL;
	}

    return hr;
}

/****************************************************************************
 *
 *  XOnlineLogon_VALID_USERS_0_SERVICES
 *
 *	Description:
 *		Perform Online logon when no services are selected
 *
 *	Input: 
 *		PUsers - valid array of users
 *		PServices - array of 0 services
 *		CServices - 0
 *		Hevent - a valid event
 *		PHandle - a valid pointer
 *
 *	System State:
 *			Connected to Internet: TRUE
 *
 *	Verify:
 *		Return value is E_XXXXX
 *		Value in pHandle (*pHandle) is NULL
 *		Values in pServices are not valid
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineLogon_VALID_USERS_0_SERVICES( void )
{
    HRESULT             hr              = S_OK;

	DWORD				dwNumUsers		= 0;
	DWORD				dwWorkFlags		= 0;

	XONLINE_USER		aUsers[XONLINE_MAX_STORED_ONLINE_USERS];
	XONLINETASK_HANDLE	taskHandle = NULL; 

	HANDLE				hEvent = CreateEvent( NULL, TRUE, TRUE, NULL );

	if ( NULL == hEvent )
	{
		hr = E_FAIL;
	}

	ZeroMemory( aUsers, sizeof( aUsers ) );

	// populate the user list
    CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) aUsers, XONLINE_MAX_HD_ONLINE_USERS ) );
    dwNumUsers = XONLINE_MAX_STORED_ONLINE_USERS;


	//
	// make sure the correct number were loaded
	//

	if ( XONLINE_MAX_HD_ONLINE_USERS != dwNumUsers )
	{
		hr = E_FAIL;
	}

	//
	// logon with 0 services
	//

	CHECKRUN( XOnlineLogon( aUsers, NULL, 0, hEvent, &taskHandle ) );
    CHECKRUN( WaitForLogon( taskHandle, hEvent ) );

	//
	// close the handle
	//

	if ( hEvent )
	{
		CloseHandle( hEvent );
		hEvent = NULL;
	}

    return hr;
}

/****************************************************************************
 *
 *  XOnlineLogon_NULL_USER
 *
 *	Description:
 *		Make sure Online logon can handle a NULL user
 *
 *	Input: 
 *		PUsers - NULL
 *		PServices - valid array of services
 *		CServices - the corresponding count of services
 *		Hevent - a valid event
 *		PHandle - a valid pointer
 *
 *	System State:
 *		Connected to Internet: N/A
 *
 *	Verify:
 *		Return value is E_XXXXX
 *		Value in pHandle (*pHandle) is NULL
 *		Values in pServices are not valid
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineLogon_NULL_USER( void )
{
    HRESULT             hr              = S_OK;

	DWORD				dwNumUsers		= 0;
	DWORD				dwWorkFlags		= 0;

	XONLINETASK_HANDLE	taskHandle = NULL; 

	HANDLE				hEvent = CreateEvent( NULL, TRUE, TRUE, NULL );

	if ( NULL == hEvent )
	{
		hr = E_FAIL;
	}


	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// logon with 0 services
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlineLogon( NULL, SERVICES[0], 0, hEvent, &taskHandle ), E_INVALIDARG ) );

	//
	// close the handle
	//

	if ( hEvent )
	{
		CloseHandle( hEvent );
		hEvent = NULL;
	}

	//
	// clear the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

    return hr;
}

/****************************************************************************
 *
 *  XOnlineLogon_NULL_SERVICE
 *
 *	Description:
 *		Make sure Online logon can handle a NULL service
 *
 *	Input: 
 *		PUsers - a valid array of users
 *		PServices - NULL
 *		CServices - the corresponding count of services
 *		Hevent - a valid event
 *		PHandle - a valid pointer
 *
 *	System State:
 *			Connected to Internet: N/A
 *
 *	Verify:
 *		Return value is E_XXXXX
 *		Value in pHandle (*pHandle) is NULL
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineLogon_NULL_SERVICE( void )
{
    HRESULT             hr              = S_OK;

	DWORD				dwNumUsers		= 0;
	DWORD				dwWorkFlags		= 0;

	XONLINE_USER		aUsers[XONLINE_MAX_STORED_ONLINE_USERS];
	XONLINETASK_HANDLE	taskHandle = NULL; 

	HANDLE				hEvent = CreateEvent( NULL, TRUE, TRUE, NULL );

	if ( NULL == hEvent )
	{
		hr = E_FAIL;
	}

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	ZeroMemory( aUsers, sizeof( aUsers ) );

	// populate the user list
    CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) aUsers, XONLINE_MAX_HD_ONLINE_USERS ) );
    dwNumUsers = XONLINE_MAX_STORED_ONLINE_USERS;

	//
	// make sure the correct number were loaded
	//

	if ( XONLINE_MAX_HD_ONLINE_USERS != dwNumUsers )
	{
		hr = E_FAIL;
	}

	//
	// logon with NULL service pointer
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlineLogon( aUsers, NULL, NUM_SERVICES[0], hEvent, &taskHandle ), E_INVALIDARG ) );

	if ( hEvent )
	{
		CloseHandle( hEvent );
		hEvent = NULL;
	}

	//
	// clear the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

    return hr;
}

/****************************************************************************
 *
 *  XOnlineLogon_NULL_EVENT
 *
 *	Description:
 *		Make sure Online logon can handle a NULL handle
 *
 *	Input: 
 *		PUsers - valid array of users
 *		PServices - valid array of services
 *		CServices - the corresponding count of services
 *		Hevent - NULL
 *		PHandle - a valid pointer
 *
 *	System State:
 *			Connected to Internet: N/A
 *
 *	Verify:
 *		Return value is E_XXXXX
 *		Value in pHandle (*pHandle) is NULL
 *		Values in pServices are not valid
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineLogon_NULL_EVENT( void )
{
    HRESULT             hr              = S_OK;

	DWORD				dwNumUsers		= 0;
	DWORD				dwWorkFlags		= 0;

	XONLINE_USER		aUsers[XONLINE_MAX_STORED_ONLINE_USERS];
	XONLINETASK_HANDLE	taskHandle = NULL; 

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	ZeroMemory( aUsers, sizeof( aUsers ) );

	// populate the user list
    CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) aUsers, XONLINE_MAX_HD_ONLINE_USERS ) );
    dwNumUsers = XONLINE_MAX_HD_ONLINE_USERS;

	//
	// logon with NULL EVENT
	//

	CHECKRUN( XOnlineLogon( aUsers, SERVICES[0], NUM_SERVICES[0], NULL, &taskHandle ) );
    if (SUCCEEDED(hr))
    {
        do
        {
            hr = XOnlineTaskContinue(taskHandle);
        } while (XONLINETASK_S_RUNNING == hr);
    }

	//
	// clear the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

    return hr;
}

/****************************************************************************
 *
 *  XOnlineLogon_NULL_XONLINE_HANDLE
 *
 *
 *	Description:
 *		Make sure Online logon can handle a NULL handle pointer
 *
 *	Input: 
 *		PUsers - valid array of users
 *		PServices - valid array of services
 *		CServices - the corresponding count of services
 *		Hevent - a valid event
 *		PHandle - NULL
 *
 *	System State:
 *			Connected to Internet: N/A
 *
 *	Verify:
 *		Return value is E_XXXXX
 *			Values in pServices are not valid
 *
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlineLogon_NULL_XONLINE_HANDLE( void )
{
    HRESULT             hr              = S_OK;

	DWORD				dwNumUsers		= 0;
	DWORD				dwWorkFlags		= 0;

	XONLINE_USER		aUsers[XONLINE_MAX_STORED_ONLINE_USERS];

	HANDLE				hEvent = CreateEvent( NULL, TRUE, TRUE, NULL );

	if ( NULL == hEvent )
	{
		hr = E_FAIL;
	}

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

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
	// logon with NULL HANDLE
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlineLogon( aUsers, SERVICES[0], 0, hEvent, NULL ), E_INVALIDARG ) );

	//
	// close the handle
	//

	if ( hEvent )
	{
		CloseHandle( hEvent );
		hEvent = NULL;
	}

	//
	// clear the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

    return hr;
}
/****************************************************************************
 *
 *  XOnlineLogon_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlineLogon tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineLogon_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;
    
    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //
    
#ifdef CODE_COVERAGE
    
#endif // CODE_COVERAGE
    
    SETLOG( LogHandle, "davidhef", "Online", "XOnlineLogon", "BASIC_TEST" );
    EXECUTE( XOnlineLogon_BASIC_TEST() );
    
    if (!BVTMode())
    {
        // Corner test cases.
        SETLOG( LogHandle, "davidhef", "Online", "XOnlineLogon", "CORNER_CASES" );
        EXECUTE( XOnlineLogon_CORNER_CASES() );

        //
        // Test XOnlineLogon with all combinations of users and services
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineLogon", "X_USERS_Y_SERVICES_HAS_INTERNET" );
        EXECUTE( XOnlineLogon_X_USERS_Y_SERVICES_HAS_INTERNET() );
        
        //
        // Test XOnlineLogon with a valid user and valid service but no internet
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineLogon", "VALID_USER_VALID_SERVICE_NO_INTERNET" );
        EXECUTE( XOnlineLogon_VALID_USER_VALID_SERVICE_NO_INTERNET() );
        
        //
        // Test XOnlineLogon with 0 services
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineLogon", "VALID_USERS_0_SERVICES" );
        EXECUTE( XOnlineLogon_VALID_USERS_0_SERVICES() );
        
        //
        // Test XOnlineLogon with a null user
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineLogon", "NULL_USER" );
        EXECUTE( XOnlineLogon_NULL_USER() );
        
        //
        // Test XOnlineLogon with a null service
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineLogon", "NULL_SERVICE" );
        EXECUTE( XOnlineLogon_NULL_SERVICE() );
        
        //
        // Test XOnlineLogon with a null event
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineLogon", "NULL_EVENT" );
        EXECUTE( XOnlineLogon_NULL_EVENT() );
        
        //
        // Test XOnlineLogon with a null handle
        //
        
        SETLOG( LogHandle, "danrose", "Online", "XOnlineLogon", "NULL_XONLINE_HANDLE" );
        EXECUTE( XOnlineLogon_NULL_XONLINE_HANDLE() );
    }
}

/****************************************************************************
 *
 *  XOnlineLogon_EndTest
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

VOID WINAPI XOnlineLogon_EndTest( VOID )
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
    XOnlineLogon_StartTest( NULL );
    XOnlineLogon_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineLogon )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineLogon )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineLogon_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineLogon_EndTest )
END_EXPORT_TABLE( XOnlineLogon )
