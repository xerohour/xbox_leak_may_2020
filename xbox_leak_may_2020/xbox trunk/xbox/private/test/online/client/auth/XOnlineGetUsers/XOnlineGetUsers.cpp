/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlineGetUsers.cpp
 *  Content:    XOnlineGetUsers tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/28/01    danrose Created to test Xbox XOnlineGetUsers
 *
 ****************************************************************************/

#include "XOnlineGetUsers.h"
#include <XOnlineAuthUtil.h>
#include <CXLiveConnectionManager.h>
#include <CXLiveUserManager.h>

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


using XLive::CXLiveConnectionManager;
using XLive::CXLiveUserManager;
using XLive::CXLiveUserManager::MUAccountIterator;
using XLive::CXLiveUserManager::HDAccountIterator;
using XLive::CXLiveUserManager::AccountIterator;
using XLive::CXLiveUserManager::const_AccountIterator;


HRESULT XOnlineGetUsers_BASIC_TEST( void )
{
    HRESULT         hr = S_OK;
    XONLINE_USER    users[XONLINE_MAX_STORED_ONLINE_USERS];
    XONLINE_USER    aIniUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    DWORD           dwUsers = 0;
    DWORD           dwMUs;
    static DWORD dwServices[] = {
            XONLINE_MATCHMAKING_SERVICE,
            XONLINE_BILLING_OFFERING_SERVICE,
            XONLINE_STATISTICS_SERVICE
    };
    DWORD cServices = sizeof(dwServices) / sizeof(dwServices[0]);
    int spinCount = (BVTMode()) ? 1 : 1000;
    
    
    
    // Use this to logon with no users.
    ZeroMemory(aIniUsers, sizeof(aIniUsers));

    // Select the services we'll request at logon time.
    CHECKRUN(CXLiveConnectionManager::Instance().SelectServices(dwServices, cServices));

    for (int i = 0; i < spinCount && SUCCEEDED(hr); ++i)
    {
        ZeroMemory(users, sizeof(users));
        
        if (SUCCEEDED(hr))
        {
            hr = COnlineAccounts::Instance()->PopulateUserAccountsHD((PXONLINEP_USER) users, XONLINE_MAX_HD_ONLINE_USERS);
        }
        
        if (SUCCEEDED(hr))
        {
            hr = CXLiveUserManager::PopulateMUs(&dwMUs);
            hr = (SUCCEEDED(hr) && dwMUs > 0) ? S_OK : E_FAIL;
        }
        
        // Get the users.
        CHECKRUN(XOnlineGetUsers(&users[0], &dwUsers));
        CHECKRUN((XONLINE_MAX_HD_ONLINE_USERS + dwMUs == dwUsers && SUCCEEDED(hr)) ? hr : E_FAIL);
        
        CXLiveUserManager userMgr(&users[0]);
        CHECKRUN((userMgr.CountMUAccounts() + userMgr.CountHDAccounts() == dwUsers && SUCCEEDED(hr)) ? hr : E_FAIL);

        // Verify that there are no duplicates.
        CHECKRUN((userMgr.VerifyNoDuplicates()) ? S_OK : E_FAIL);
        
        AccountIterator it = userMgr.Account_begin();
        for (; it != userMgr.Account_end(); ++it)
        {
            // Clear account selections.
            CHECKRUN(userMgr.ClearAccountSelections());
            
            // Select an account to logon.
            CHECKRUN(userMgr.SelectAccount(it));
            
            // Logon with selected user, then logoff.  Services were selected above.
            if (SUCCEEDED(hr))
            {
                CHECKRUN(CXLiveConnectionManager::Instance().Logon(userMgr.SelectedAccounts()));
                CHECKRUN(CXLiveConnectionManager::Instance().Logoff());
            }
        }

        // Now we will verify that users are returned in order of most recently
        // logged on to least recently logged on.
        XONLINE_USER orderedUsers[XONLINE_MAX_STORED_ONLINE_USERS];
        ZeroMemory(orderedUsers, sizeof(orderedUsers));

        // Get the users. They should be in opposite order than is found in
        // the users array.
        CHECKRUN(XOnlineGetUsers(&orderedUsers[0], &dwUsers));

        // Verify that there are the correct number of accounts.
        CXLiveUserManager orderedMgr(&orderedUsers[0]);
        CHECKRUN((orderedMgr.CountMUAccounts() + orderedMgr.CountHDAccounts() == dwUsers && SUCCEEDED(hr)) ? hr : E_FAIL);
        
        // Check the order of accounts returned.
        it = userMgr.Account_begin();
        const_AccountIterator orderedIt = orderedMgr.Account_end();
        for (size_t j = 0; j < XONLINE_MAX_STORED_ONLINE_USERS && SUCCEEDED(hr); ++j)
        {
            hr = (*it++ == *--orderedIt) ? S_OK : E_FAIL;
        }
    }
    
    return hr;
}

HRESULT XOnlineGetUsers_NULL_USERS( void )
{
    HRESULT         hr = S_OK;
    DWORD           dwUsers = 0;
    XONLINE_USER    aIniUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    static DWORD dwServices[] = {
            XONLINE_MATCHMAKING_SERVICE,
            XONLINE_BILLING_OFFERING_SERVICE
    };
    DWORD cServices = sizeof(dwServices) / sizeof(dwServices[0]);
    XONLINETASK_HANDLE	taskHandle = NULL; 
    int spinCount = (BVTMode()) ? 1 : 10;

    HANDLE				hEvent = CreateEvent( NULL, TRUE, TRUE, NULL );
    if ( NULL == hEvent )
    {
        hr = E_FAIL;
    }
    
    // Use this to logon with no users.
    ZeroMemory(aIniUsers, sizeof(aIniUsers));

    // Logon with 0 users just to create a connection.
    CHECKRUN( XOnlineLogon((PXONLINE_USER) aIniUsers, dwServices, cServices, hEvent, &taskHandle) );
    if (SUCCEEDED(hr))
    {
        // Wait for logon to complete all initial work.
        hr = WaitForLogon(taskHandle, hEvent);
    }
    
    if (SUCCEEDED(hr))
    {
        hr = XOnlineGetUsers(NULL, &dwUsers);
        hr = (SUCCEEDED(hr)) ? E_FAIL : S_OK;
    }
    
    if (NULL != hEvent)
    {
        hr = (CloseHandle(hEvent)) ? hr : E_FAIL;
        hEvent = NULL;
    }

    if (taskHandle)
    {
        hr = XOnlineTaskClose( taskHandle );
        taskHandle = NULL;
    }

    return hr;
}

HRESULT XOnlineGetUsers_NULL_COUNT( void )
{
    HRESULT         hr = S_OK;
    XONLINE_USER    users[XONLINE_MAX_STORED_ONLINE_USERS];
    XONLINE_USER    aIniUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    static DWORD dwServices[] = {
            XONLINE_MATCHMAKING_SERVICE,
            XONLINE_BILLING_OFFERING_SERVICE
    };
    DWORD cServices = sizeof(dwServices) / sizeof(dwServices[0]);
    XONLINETASK_HANDLE	taskHandle = NULL; 
    int spinCount = (BVTMode()) ? 1 : 10;

    HANDLE				hEvent = CreateEvent( NULL, TRUE, TRUE, NULL );
    if ( NULL == hEvent )
    {
        hr = E_FAIL;
    }

    // Put 0 users in the array.
    ZeroMemory(aIniUsers, sizeof(aIniUsers));
    
    // Logon with 0 users just to create a connection.
    CHECKRUN( XOnlineLogon((PXONLINE_USER) aIniUsers, dwServices, cServices, hEvent, &taskHandle) );
    if (SUCCEEDED(hr))
    {
        // Wait for logon to complete all initial work.
        hr = WaitForLogon(taskHandle, hEvent);
    }
    
    if (SUCCEEDED(hr))
    {
        hr = XOnlineGetUsers(users, NULL);
        hr = (SUCCEEDED(hr)) ? E_FAIL : S_OK;
    }
    
    if (NULL != hEvent)
    {
        hr = (CloseHandle(hEvent)) ? hr : E_FAIL;
        hEvent = NULL;
    }

    if (taskHandle)
    {
        hr = XOnlineTaskClose( taskHandle );
        taskHandle = NULL;
    }

    return hr;
}


/****************************************************************************
 *
 *  XOnlineGetUsers_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlineGetUsers tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlineGetUsers_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;
    
    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //
    
#ifdef CODE_COVERAGE
    
#endif // CODE_COVERAGE
    
    g_ParamCheckReturnsError = TRUE;
    
    CHECKRUN( StartNetwork() );
    
    SETLOG( LogHandle, "davidhef", "Online", "XOnlineGetUsers", "BASIC_TEST" );
    EXECUTE( XOnlineGetUsers_BASIC_TEST() );
    
    if (!BVTMode())
    {
        SETLOG( LogHandle, "davidhef", "Online", "XOnlineGetUsers", "NULL_USERS" );
        EXECUTE( XOnlineGetUsers_NULL_USERS() );
        
        SETLOG( LogHandle, "davidhef", "Online", "XOnlineGetUsers", "NULL_COUNT" );
        EXECUTE( XOnlineGetUsers_NULL_COUNT() );
    }
    
    CHECKRUN( CloseNetwork() );
    
    g_ParamCheckReturnsError = FALSE;
}

/****************************************************************************
 *
 *  XOnlineGetUsers_EndTest
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

VOID WINAPI XOnlineGetUsers_EndTest( VOID )
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
    XOnlineGetUsers_StartTest( NULL );
    XOnlineGetUsers_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlineGetUsers )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlineGetUsers )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlineGetUsers_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlineGetUsers_EndTest )
END_EXPORT_TABLE( XOnlineGetUsers )
