/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       stats.cpp
 *  Content:    stats tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *   2/21/01    davidhef Created to test Xbox stats
 *
 ****************************************************************************/

#include "stats.h"
#include <xonlinep.h>
#include <XOnline.h>
#include <XOnlineAuthUtil.h>
#include <CXLiveConnectionManager.h>
#include <CXLiveUserManager.h>

using XLive::CXLiveUserManager;
using XLive::CXLiveConnectionManager;
using XLive::CXLiveUserManager::const_AccountIterator;

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  stats_BasicTest
 *
 *  Description:
 *      Basic Test for stats
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT stats_BasicTest( void )
{
    HRESULT             hr              = S_OK;
    XONLINETASK_HANDLE  phTask = NULL;
    bool    bNetworkStarted = false;
    bool    bLoggedOn = false;
    
    if (SUCCEEDED(hr))
    {
        hr = StartNetwork();
        bNetworkStarted = (S_OK == hr) ? true : false;
    }
    
    CXLiveUserManager userMgr;
    CHECKRUN(userMgr.GenerateAndSelectRandomAccounts());

    // Logon the selected accounts.
    CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection(userMgr.SelectedAccounts(), (DWORD) 5000));
    hr = (XONLINE_S_LOGON_CONNECTION_ESTABLISHED == hr) ? S_OK : E_FAIL;
    bLoggedOn = (SUCCEEDED(hr)) ? true : false;

    XONLINE_SET_STAT setStats[XONLINE_MAX_LOGON_USERS];
    DWORD dwLeaderBoardID = 37;
    WORD statID = 13;           // Our lucky number ;-)

    // Fill in a XONLINE_STAT.
    XONLINE_STAT stats[1];
    stats[0].wID = XONLINE_STAT_RATING;
    stats[0].type = XONLINE_STAT_LONGLONG;
    static LONGLONG scores[XONLINE_MAX_LOGON_USERS] = {100293, 59567, 200456, 345};

    // Fill in and set a XONLINE_SET_STAT for each user.
    for (DWORD userIndex = 0; userIndex < XONLINE_MAX_LOGON_USERS && SUCCEEDED(hr); ++userIndex)
    {
        XONLINE_SET_STAT *pStat = setStats + userIndex;
        XONLINE_USER *pUser = userMgr.SelectedAccounts() + userIndex;

        pStat->xuidUser = pUser->xuid;
        pStat->dwLeaderBoardID = dwLeaderBoardID;
        pStat->dwNumStats = sizeof(stats) / sizeof(stats[0]);
        pStat->pStats = stats;
        stats[0].qwValue = scores[userIndex];

        hr = XOnlineStatSet(1, pStat, NULL, &phTask);
    }

    if (bLoggedOn)
    {
        // Log off.
        HRESULT loggedHr = CXLiveConnectionManager::Instance().Logoff();
        bLoggedOn = false;
    }

    if (bNetworkStarted)
    {
        HRESULT closedHr = CloseNetwork();
        hr = (FAILED(closedHr)) ? closedHr : hr;
        bNetworkStarted = false;
    }
    
    // BUGBUG: Remove this statement when the service is implemented.
    hr = E_NOTIMPL;

    return hr;
}

/****************************************************************************
 *
 *  stats_StartTest
 *
 *  Description:
 *      The Harness Entry into the stats tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI stats_StartTest( HANDLE LogHandle )
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
    // Test stats in the manner it was meant to be called
    //

    SETLOG( LogHandle, "davidhef", "Online", "stats", "Basic" );
    EXECUTE( stats_BasicTest() );
}

/****************************************************************************
 *
 *  stats_EndTest
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

VOID WINAPI stats_EndTest( VOID )
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
    stats_StartTest( NULL );
    stats_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( stats )
#pragma data_seg()

BEGIN_EXPORT_TABLE( stats )
    EXPORT_TABLE_ENTRY( "StartTest", stats_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", stats_EndTest )
END_EXPORT_TABLE( stats )
