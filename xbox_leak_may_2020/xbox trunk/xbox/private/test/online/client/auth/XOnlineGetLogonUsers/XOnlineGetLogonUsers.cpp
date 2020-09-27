/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineGetLogonUsers.cpp
 *  Content:    XOnlineGetLogonUsers tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  01/04/02    davidhef Created to test Xbox XOnlineGetLogonUsers
 *
 ****************************************************************************/

#include "XOnlineGetLogonUsers.h"
#include <XOnlineAuthUtil.h>
#include <CXLiveConnectionManager.h>
#include <CXLiveUserManager.h>

using XLive::CXLiveConnectionManager;
using XLive::CXLiveUserManager;
using XLive::CXLiveUserManager::MUAccountIterator;
using XLive::CXLiveUserManager::HDAccountIterator;


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

static DWORD SERVICES[] = {
	    XONLINE_MATCHMAKING_SERVICE,
        XONLINE_BILLING_OFFERING_SERVICE
};   

/****************************************************************************
 *
 * The Global online param check flag
 *
 ****************************************************************************/

extern BOOL g_ParamCheckReturnsError;

/****************************************************************************
 *
 *  XOnlineGetLogonUsers_NOT_LOGGED_IN
 *
	Description:
        Verify that the API returns NULL when XOnlineLogon has not been called.

    System State:
	    XOnlineLogon has not been called

    Verify:
	    NULL is returned

*
*****************************************************************************/

HRESULT XOnlineGetLogonUsers_NOT_LOGGED_IN ( void )
{
    HRESULT             hr              = E_FAIL;
    PXONLINE_USER       pUsers          = (PXONLINE_USER) 1234;

    pUsers = XOnlineGetLogonUsers();
    hr = (NULL == pUsers) ? S_OK : E_FAIL;

    return hr;
}

/****************************************************************************
 *
 *  XOnlineGetLogonUsers_LOGGED_IN
 *
	Description:
        Verify that the API returns a non-NULL pointer when XOnlineLogon has completed.

    System State:
	    XOnlineLogon has been called and has completed successfully

    Verify:
	    NULL is not returned


*
*****************************************************************************/

HRESULT XOnlineGetLogonUsers_BASIC_TEST ( void )
{
    HRESULT             hr              = S_OK;
    XONLINE_USER		users[XONLINE_MAX_STORED_ONLINE_USERS];
    PXONLINE_USER pUsers = NULL;
    DWORD dwMUs = 0;
    DWORD dwUsers = 0;
    
    // zero out the users
    ZeroMemory( users, sizeof( users ) );
    
    // populate the user list
    CHECKRUN( COnlineAccounts::Instance()->PopulateUserAccountsHD( (PXONLINEP_USER) users, XONLINE_MAX_HD_ONLINE_USERS, true ) );
    
    if (SUCCEEDED(hr))
    {
        hr = CXLiveUserManager::PopulateMUs(&dwMUs);
        hr = (SUCCEEDED(hr) && dwMUs > 0) ? S_OK : E_FAIL;
    }

    // Get the number of existing accounts.
    CHECKRUN(XOnlineGetUsers(&users[0], &dwUsers));

    // Select services.
    CHECKRUN(CXLiveConnectionManager::Instance().SelectServices(&SERVICES[0], NUMELEMS(SERVICES)));
    
    // Instantiate helper class.
    CXLiveUserManager userMgr(&users[0]);

    // Verify the number of existing accounts.
    CHECKRUN((userMgr.CountMUAccounts() + userMgr.CountHDAccounts() == dwUsers && SUCCEEDED(hr)) ? hr : E_FAIL);
    
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
    }
    
    // Select some HD accounts to logon.
    if (SUCCEEDED(hr))
    {
        int count = 1;              // We already selected a MU account.
        HDAccountIterator hdIt = userMgr.HDAccount_begin();
        for (; hdIt != userMgr.HDAccount_end() && count < XONLINE_MAX_LOGON_USERS && SUCCEEDED(hr); ++hdIt, ++count)
        {
            hr = userMgr.SelectAccount(hdIt);
        }
    }
    
    // Logon with selected users.  Services were selected above.
    if (SUCCEEDED(hr))
    {
        CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection(userMgr.SelectedAccounts()));
    }

    // Get the logged on users.
    if (SUCCEEDED(hr))
    {
        pUsers = XOnlineGetLogonUsers();
        hr = ( NULL == pUsers ) ? E_FAIL : S_OK;
    }
    
    // Verify the logon.
    CHECKRUN(userMgr.VerifyLogon(pUsers));
    
    CHECKRUN(CXLiveConnectionManager::Instance().Logoff());
    
    return hr;
}

/****************************************************************************
 *
 *  XOnlineGetLogonUsers_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlineGetLogonUsers tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineGetLogonUsers_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

    CHECKRUN( StartNetwork() );
    
    SETLOG( LogHandle, "davidhef", "Online", "XOnlineGetLogonUsers", "BASIC_TEST" );
    EXECUTE( XOnlineGetLogonUsers_BASIC_TEST() );
    
    if (!BVTMode())
    {
        //
        // Test XOnlineGetLogonUsers when not logged in
        //
        
        SETLOG( LogHandle, "davidhef", "Online", "XOnlineGetLogonUsers", "NOT_LOGGED_IN" );
        EXECUTE( XOnlineGetLogonUsers_NOT_LOGGED_IN() );
    }
    
    CHECKRUN( CloseNetwork() );
}

/****************************************************************************
 *
 *  XOnlineGetLogonUsers_EndTest
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

VOID WINAPI XOnlineGetLogonUsers_EndTest( VOID )
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
    XOnlineGetLogonUsers_StartTest( NULL );
    XOnlineGetLogonUsers_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineGetLogonUsers )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineGetLogonUsers )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineGetLogonUsers_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineGetLogonUsers_EndTest )
END_EXPORT_TABLE( XOnlineGetLogonUsers )
