/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       feedback.cpp
 *  Content:    feedback tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *   2/21/01    davidhef Created to test Xbox feedback
 *
 ****************************************************************************/

#include "feedback.h"
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
 *  feedback_BasicTest
 *
 *  Description:
 *      Basic Test for feedback
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT feedback_BasicTest( void )
{
    HRESULT             hr              = S_OK;
    LPCWSTR             pwszNickname = L"SuperSquirrel";
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

    static XONLINE_FEEDBACK_TYPE feedbackTypes[] = {
        	XONLINE_FEEDBACK_NEG_NICKNAME,
	        XONLINE_FEEDBACK_NEG_GAMEPLAY,
	        XONLINE_FEEDBACK_NEG_SCREAMING,
	        XONLINE_FEEDBACK_NEG_HARASSMENT,
	        XONLINE_FEEDBACK_NEG_LEWDNESS,
	        XONLINE_FEEDBACK_POS_ATTITUDE,
	        XONLINE_FEEDBACK_POS_SESSION
    };

    for (DWORD userIndex = 0; userIndex < 4 && SUCCEEDED(hr); ++userIndex)
    {
        const_AccountIterator accountIt = userMgr.Account_begin();
        for (; accountIt != userMgr.Account_end() && SUCCEEDED(hr); ++accountIt)
        {
            if (userMgr.UserIsSelected(accountIt->xuid))
            {
                // Not allowed to complain about a local, logged on user.
                continue;
            }

            if (CXLiveUserManager::IsGuestAccount(accountIt))
            {
                // Guests are not allowed to send feedback.
                continue;
            }

            for (   int type = 0;
                    type < sizeof(feedbackTypes) / sizeof(feedbackTypes[0]) && SUCCEEDED(hr);
                    ++type)
            {
                hr = XOnlineFeedbackSend(   userIndex,
                                            accountIt->xuid,
                                            feedbackTypes[type],
                                            pwszNickname,
                                            NULL,
                                            &phTask);

                if (SUCCEEDED(hr))
                {
                    if (phTask != NULL)
                    {
                        // Pump the task.
                        hr = XOnlineTaskContinue(phTask);
                    }
                }
            }
        }
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
    hr = (hr != E_NOTIMPL) ? E_FAIL : E_NOTIMPL;

    return hr;
}

/****************************************************************************
 *
 *  feedback_StartTest
 *
 *  Description:
 *      The Harness Entry into the feedback tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI feedback_StartTest( HANDLE LogHandle )
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
    // Test feedback in the manner it was meant to be called.
    //

    SETLOG( LogHandle, "davidhef", "Online", "feedback", "Basic" );
    EXECUTE( feedback_BasicTest() );
}

/****************************************************************************
 *
 *  feedback_EndTest
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

VOID WINAPI feedback_EndTest( VOID )
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
    feedback_StartTest( NULL );
    feedback_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( feedback )
#pragma data_seg()

BEGIN_EXPORT_TABLE( feedback )
    EXPORT_TABLE_ENTRY( "StartTest", feedback_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", feedback_EndTest )
END_EXPORT_TABLE( feedback )
