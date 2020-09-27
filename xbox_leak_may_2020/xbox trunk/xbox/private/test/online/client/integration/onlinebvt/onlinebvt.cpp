/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       onlinebvt.cpp
 *  Content:    onlinebvt tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  12/10/01    tristanj Created to test Xbox onlinebvt
 *
 ****************************************************************************/

#include "onlinebvt.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

HANDLE g_hLog;

/****************************************************************************
 *
 *  onlinebvt_BasicTest
 *
 *  Description:
 *      Basic Test for onlinebvt
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT onlinebvt_BasicTest( void )
{
    HRESULT                 hr              = S_OK;
	CLogonTask              LogonTask;
	CFriendEnumTask		    FriendEnumTask1;
	CMatchSessionCreateTask *pSessionCreateTask = NULL;
	CMatchSearchTask        *pSearchTask = NULL;
	COfferingEnumerateTask  OfferingEnumerateTask;
	CContentInstallTask     *pContentInstallTask = NULL;
	CContentVerifyTask      *pContentVerifyTask = NULL;
	CContentRemoveTask      *pContentRemoveTask = NULL;
	COnlineTimer			Timer;
	XNKID					SessionID;
	XONLINE_FRIEND           *pFriendList = NULL;
	PXONLINEOFFERING_INFO   *rgpEnumInfo;
	PXMATCH_SEARCHRESULT    *rgpSearchResults = NULL;
	DWORD					ServicesArray[] =
	{
		XONLINE_MATCHMAKING_SERVICE,
		XONLINE_BILLING_OFFERING_SERVICE
		// TODO - Add content-download here when it is figured out
	};
	DWORD                   dwServicesCount = sizeof(ServicesArray) / sizeof(DWORD);
	DWORD                   dwFriendCount = 0, dwResultsCount = 0, dwOfferingCount = 0, dwReturnedResults = 0;
	BOOL                    fPartialResults = FALSE;

	// Populate users accounts from users.ini
	hr = COnlineAccounts::Instance()->PopulateUserAccountsHD(NULL, XONLINE_MAX_LOGON_USERS);
	if(hr != S_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Populating user accounts failed.");
		goto Exit;
	}

	//------------------------
	// LOGON STAGE
	//------------------------

	LogonTask.RegisterLogHandle(g_hLog);

	// The logon task will automatically login with as many users as possible

	// Need to add the matchmaking and content services since they aren't included by default
	if(!LogonTask.SetServices(ServicesArray, dwServicesCount))
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Start logon async task
	if(!LogonTask.StartTask())
	{
		hr = E_FAIL;
		goto Exit;
	}

	Timer.SetAllowedTime(MAX_LOGON_TIME);
	Timer.Start();

	// Pump task until complete
	while(!LogonTask.IsConnectionEstablished())
	{
		if(Timer.HasTimeExpired())
			break;

		// Make sure to wait for the work event to test that it is always signalled when necessary
		if(!LogonTask.WaitForWork(Timer.GetTimeRemaining()))
			break;

		LogonTask.TaskContinue();
	}

	if(!LogonTask.IsConnectionEstablished())
	{
		LOGTASKINFO(g_hLog, "Logon timed-out");
		hr = E_FAIL;
		goto Exit;
	}

	LOGTASKINFO(g_hLog, "Auth test: PASSED");

	//------------------------
	// PRESENCE STAGE
	//------------------------

	FriendEnumTask1.RegisterLogHandle(g_hLog);

	// The buddy enum task will automatically use the player at index 0

	// Start buddy enum async task
	if(!FriendEnumTask1.StartTask())
	{
		hr = E_FAIL;
		goto Exit;
	}

	Timer.SetAllowedTime(MAX_FRIEND_ENUM_TIME);
	Timer.Start();

	// Pump task for MAX_FRIEND_ENUM_TIME seconds
	while(!FriendEnumTask1.IsFriendListUpToDate())
	{
		if(Timer.HasTimeExpired())
			break;

		FriendEnumTask1.TaskContinue();
	}

	if(!FriendEnumTask1.IsFriendListUpToDate())
	{
		LOGTASKINFO(g_hLog, "Friend enumeration task didn't report an up-to-date list");
		hr = E_FAIL;
		goto Exit;
	}

	dwFriendCount = FriendEnumTask1.GetLatestFriendList(&pFriendList);

	LOGTASKINFO(g_hLog, "Friend test: PASSED");

	//------------------------
	// MATCH STAGE
	//------------------------

	//
	// Session creation phase
	//

	pSessionCreateTask = new CMatchSessionCreateTask();
	if(!pSessionCreateTask)
	{
		LOGTASKINFO(g_hLog, "Couldn't allocate session creation task");
		hr = E_FAIL;
		goto Exit;
	}

	pSessionCreateTask->RegisterLogHandle(g_hLog);

	// Set 4 public slots, 0 private slots
	pSessionCreateTask->SetAvailableSlots(4, 2, 0, 0);

	// Start match session creation task
	if(!pSessionCreateTask->StartTask())
	{
		hr = E_FAIL;
		goto Exit;
	}

//	Timer.SetAllowedTime(MAX_SESSION_CREATE_TIME);
	Timer.SetAllowedTime(100000000);
	Timer.Start();

	// Pump task for MAX_SESSION_CREATE_TIME seconds
	while(!pSessionCreateTask->IsTaskComplete())
	{
		if(Timer.HasTimeExpired())
			break;

		LogonTask.TaskContinue();
		pSessionCreateTask->TaskContinue();
	}

	if(!pSessionCreateTask->IsTaskComplete())
	{
		LOGTASKINFO(g_hLog, "Session creation timed-out");
		hr = E_FAIL;
		goto Exit;
	}

	pSessionCreateTask->GetSessionInfo(&SessionID, NULL);

	if(*((ULONGLONG *) &SessionID) == 0)
	{
		LOGTASKINFO(g_hLog, "Session ID wasn't set");
		hr = E_FAIL;
		goto Exit;
	}

	//
	// Search phase
	//

	pSearchTask = new CMatchSearchTask();

	pSearchTask->SetSessionIDForSearch(&SessionID);

	// Start match search task
	if(!pSearchTask->StartTask())
	{
		hr = E_FAIL;
		goto Exit;
	}

//	Timer.SetAllowedTime(MAX_SEARCH_TIME);
	Timer.SetAllowedTime(100000000);
	Timer.Start();

	// Pump task for MAX_SESSION_CREATE_TIME seconds
	while(!pSearchTask->IsTaskComplete())
	{
		if(Timer.HasTimeExpired())
			break;

		LogonTask.TaskContinue();
		pSearchTask->TaskContinue();
	}

	if(!pSearchTask->IsTaskComplete())
	{
		LOGTASKINFO(g_hLog, "Search timed-out");
		hr = E_FAIL;
		goto Exit;
	}

	hr = pSearchTask->GetSearchResults(&rgpSearchResults, &dwReturnedResults);
	if(FAILED(hr))
	{
		LOGTASKINFO(g_hLog, "Failed getting search results");
		hr = E_FAIL;
		goto Exit;
	}

	if(dwReturnedResults == 0)
	{
		LOGTASKINFO(g_hLog, "Didn't find hosted session");
		hr = E_FAIL;
		goto Exit;
	}

	delete pSearchTask;
	pSearchTask = NULL;

	delete pSessionCreateTask;
	pSessionCreateTask = NULL;

	LOGTASKINFO(g_hLog, "Match test: PASSED");

	//------------------------
	// OFFERING ENUMERATION STAGE
	//------------------------

	OfferingEnumerateTask.RegisterLogHandle(g_hLog);

	// Start match session creation task
	if(!OfferingEnumerateTask.StartTask())
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Iterate through offering enumeration until there are no more results
	do
	{
		//	Timer.SetAllowedTime(MAX_ENUMERATION_TIME);
		Timer.SetAllowedTime(100000000);
		Timer.Start();
		
		// Pump task for MAX_ENUMERATION_TIME seconds
		do
		{
			if(Timer.HasTimeExpired())
				break;
			
			LogonTask.TaskContinue();
			OfferingEnumerateTask.TaskContinue();
		}while(!OfferingEnumerateTask.IsTaskComplete());
		
		if(!OfferingEnumerateTask.IsTaskComplete())
		{
			LOGTASKINFO(g_hLog, "Offering enumeration timed-out");
			hr = E_FAIL;
			goto Exit;
		}
		
		dwResultsCount = 0;
		fPartialResults = FALSE;
		hr = OfferingEnumerateTask.GetResults(&rgpEnumInfo, &dwResultsCount, &fPartialResults);
		xLog(g_hLog, XLL_INFO, "%u results returned by offering enumeration");

		if(!dwResultsCount)
		{
			LOGTASKINFO(g_hLog, "Offering enumeration didn't return any results");
			hr = E_FAIL;
			goto Exit;
		}


		do
		{
			pContentInstallTask = new CContentInstallTask;
			pContentVerifyTask = new CContentVerifyTask;
			pContentRemoveTask = new CContentRemoveTask;

			if(!pContentInstallTask || !pContentVerifyTask || !pContentRemoveTask)
			{
				LOGTASKINFO(g_hLog, "Couldn't allocate content tasks");
				hr = E_FAIL;
				goto Exit;
			}

			pContentInstallTask->RegisterLogHandle(g_hLog);
			pContentVerifyTask->RegisterLogHandle(g_hLog);
			pContentRemoveTask->RegisterLogHandle(g_hLog);

		    pContentInstallTask->SetOfferingID((rgpEnumInfo[dwOfferingCount])->OfferingId);
			pContentVerifyTask->SetOfferingID((rgpEnumInfo[dwOfferingCount])->OfferingId);
			pContentRemoveTask->SetOfferingID((rgpEnumInfo[dwOfferingCount])->OfferingId);

			++dwOfferingCount;

			//
			// Start installing the content
			//

			xLog(g_hLog, XLL_INFO, "Installing offering %u", dwOfferingCount);
			if(!pContentInstallTask->StartTask())
			{
				LOGTASKINFO(g_hLog, "Error starting content download");
				hr = E_FAIL;
				goto Exit;
			}

			//	Timer.SetAllowedTime(MAX_INSTALL_TIME);
			Timer.SetAllowedTime(100000000);
			Timer.Start();

			// Pump task for MAX_INSTALL_TIME seconds
			do
			{
				if(Timer.HasTimeExpired())
					break;
				
				LogonTask.TaskContinue();
				pContentInstallTask->TaskContinue();
			}while(!pContentInstallTask->IsTaskComplete());
			
			if(!pContentInstallTask->IsTaskComplete())
			{
				LOGTASKINFO(g_hLog, "Content install timed-out");
				hr = E_FAIL;
				goto Exit;
			}

			if(FAILED(pContentInstallTask->GetLastResult()))
			{
				LOGTASKINFO(g_hLog, "Content install failed");
				hr = E_FAIL;
				goto Exit;
			}

			delete pContentInstallTask;
			pContentInstallTask = NULL;

			xLog(g_hLog, XLL_INFO, "Installed offering %u successfully", dwOfferingCount);

			//
			// Start verifing the content
			//

			xLog(g_hLog, XLL_INFO, "Verifying offering %u", dwOfferingCount);
			if(!pContentVerifyTask->StartTask())
			{
				LOGTASKINFO(g_hLog, "Error starting content verification");
				hr = E_FAIL;
				goto Exit;
			}

			//	Timer.SetAllowedTime(MAX_VERIFICATION_TIME);
			Timer.SetAllowedTime(100000000);
			Timer.Start();

			// Pump task for MAX_VERIFICATION_TIME seconds
			do
			{
				if(Timer.HasTimeExpired())
					break;
				
				LogonTask.TaskContinue();
				pContentVerifyTask->TaskContinue();
			}while(!pContentVerifyTask->IsTaskComplete());
			
			if(!pContentVerifyTask->IsTaskComplete())
			{
				LOGTASKINFO(g_hLog, "Content verification timed-out");
				hr = E_FAIL;
				goto Exit;
			}

			if(FAILED(pContentVerifyTask->GetLastResult()))
			{
				LOGTASKINFO(g_hLog, "Content verification failed");
				hr = E_FAIL;
				goto Exit;
			}

			delete pContentVerifyTask;
			pContentVerifyTask = NULL;

			xLog(g_hLog, XLL_INFO, "Verified offering %u successfully", dwOfferingCount);

			//
			// Start remove the content
			//

			xLog(g_hLog, XLL_INFO, "Removing offering %u", dwOfferingCount);
			if(!pContentRemoveTask->StartTask())
			{
				LOGTASKINFO(g_hLog, "Error starting content removal");
				hr = E_FAIL;
				goto Exit;
			}

			//	Timer.SetAllowedTime(MAX_REMOVAL_TIME);
			Timer.SetAllowedTime(100000000);
			Timer.Start();

			// Pump task for MAX_REMOVAL_TIME seconds
			do
			{
				if(Timer.HasTimeExpired())
					break;
				
				LogonTask.TaskContinue();
				pContentRemoveTask->TaskContinue();
			}while(!pContentRemoveTask->IsTaskComplete());
			
			if(!pContentRemoveTask->IsTaskComplete())
			{
				LOGTASKINFO(g_hLog, "Content removal timed-out");
				hr = E_FAIL;
				goto Exit;
			}

			if(FAILED(pContentRemoveTask->GetLastResult()))
			{
				LOGTASKINFO(g_hLog, "Content removal failed");
				hr = E_FAIL;
				goto Exit;
			}

			delete pContentRemoveTask;
			pContentRemoveTask = NULL;

			xLog(g_hLog, XLL_INFO, "Removed offering %u successfully", dwOfferingCount);

		} while(--dwResultsCount);


	} while(!FAILED(hr) && fPartialResults);

	if(FAILED(hr))
	{
		LOGTASKINFOHR(g_hLog, "Offering enumeration failed", hr);
		hr = E_FAIL;
		goto Exit;
	}

	//------------------------
	// MAINTAIN LOGON
	//------------------------
	
	//	Stay online for another minute
	Timer.SetAllowedTime(60000);
	Timer.Start();

	while(LogonTask.IsConnectionEstablished())
	{
		if(Timer.HasTimeExpired())
			break;

		// Make sure to wait for the work event to test that it is always signalled when necessary
		if(LogonTask.WaitForWork(30000))
			LogonTask.TaskContinue();
	}

Exit:

	pContentInstallTask ? delete pContentInstallTask : 0;
	pContentVerifyTask ? delete pContentVerifyTask : 0;
	pContentRemoveTask ? delete pContentRemoveTask : 0;

	pSessionCreateTask ? delete pSessionCreateTask : 0;
	pSearchTask ? delete pSearchTask : 0;


    //
    // Return the Hresult
    //

	if(FAILED(hr))
		xLog(g_hLog, XLL_FAIL, "Test failed");
	else
		xLog(g_hLog, XLL_PASS, "Test passed");

    return hr;
}

/****************************************************************************
 *
 *  onlinebvt_StartTest
 *
 *  Description:
 *      The Harness Entry into the onlinebvt tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI onlinebvt_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;
    XDEVICE_PREALLOC_TYPE deviceTypes[] = {
               {XDEVICE_TYPE_GAMEPAD, 4},
               {XDEVICE_TYPE_MEMORY_UNIT, 8}
    };

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

	XInitDevices(sizeof(deviceTypes) / sizeof(XDEVICE_PREALLOC_TYPE), deviceTypes);

	XNetAddRef();

	hr = XOnlineStartup(NULL);

    //
    // Test onlinebvt in the mannor it was meant to be called
    //

    if ( SHOULDRUNTEST( "onlinebvt", "Basic" ) )
    {
        SETLOG( LogHandle, "tristanj", "Online", "onlinebvt", "Basic" );
        EXECUTE( onlinebvt_BasicTest() );
    }

	hr = XOnlineCleanup();

	XNetRelease();
}

/****************************************************************************
 *
 *  onlinebvt_EndTest
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

VOID WINAPI onlinebvt_EndTest( VOID )
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
    onlinebvt_StartTest( NULL );
    onlinebvt_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( onlinebvt )
#pragma data_seg()

BEGIN_EXPORT_TABLE( onlinebvt )
    EXPORT_TABLE_ENTRY( "StartTest", onlinebvt_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", onlinebvt_EndTest )
END_EXPORT_TABLE( onlinebvt )
