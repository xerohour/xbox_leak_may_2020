//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include "xbudbvt.h"

#define	BUG10049REGRESS	1
        
using namespace XBudBVTNamespace;

namespace XBudBVTNamespace {

//==================================================================================
// Globals
//==================================================================================
XONLINE_USER g_LoggedOnUsers[XONLINE_MAX_LOGON_USERS];
DWORD g_dwNumLoggedOnUsers = 0;
HANDLE g_hHeap = NULL;

//==================================================================================
// Internal Functions
//==================================================================================
SY_RESULT PumpFriendTask(HANDLE  hLog, XONLINETASK_HANDLE hLogonTask, XONLINETASK_HANDLE hGameOpTask, 
							XONLINETASK_HANDLE hEnumGamesTask[], XONLINETASK_HANDLE hLockoutTask, UINT count,
							HRESULT* phrReturn);
SY_RESULT PumpLogonTask(HANDLE  hLog, XONLINETASK_HANDLE hLogonTask, HRESULT* phrReturn);
BOOL	IsValidResultForFriendList(HANDLE  hLog, DWORD dwNumOfExpectedFriend, DWORD dwNumOfReturnedFriend, 
									XONLINE_FRIEND *pFriendListExpected, XONLINE_FRIEND *pFriendListReturned);
BOOL	IsTimeFinished(DWORD	dwStart, DWORD dwAllowed);
HRESULT SY_CreateUserAccounts(DWORD dwNumAccounts );
BOOL IsLockoutUser(DWORD dwNumLockoutUsers, PXONLINE_LOCKOUTUSER  pLockoutlist, XUID xuid);


//==================================================================================
// PollTaskUntilComplete
//----------------------------------------------------------------------------------
//
// Description: Calls the XOnlineTaskComplete function until either the given
//    task has been completed or the amount of available time has expired
//
// Arguments:
//	XONLINETASK_HANDLE	hTask		Handle to async task
//	HANDLE				hEvent		Event that is associated with this async task.
//											This can be set to NULL or INVALID_HANDLE_VALUE
//											if no event is associated with the task
//	DWORD				dwMaxWaitTime		Maximum amount of time to wait for the task to
//											complete. This can be set to INFINITE to block
//											until completion
//	DWORD				*pdwPollCounter		Optional counter passed in to keep track of the
//											number of calls made to the XOnlineTaskContinue function
//	BOOL				*pfCompleted		Optional boolean passed in to keep track if whether
//											the task completed or not
//  BOOL				fCloseOnComplete	Indicates whether the function should close a handle
//											after it indicates completion
//
// Returns: The last result code from XOnlineTaskContinue is returned.  If a valid pointer
//   is provided for pfCompleted, it is updated to reflect whether the task has completed or not
//==================================================================================
HRESULT PollTaskUntilComplete(XONLINETASK_HANDLE hTask, HANDLE hEvent, DWORD dwMaxWaitTime, DWORD *pdwPollCounter, BOOL *pfCompleted, BOOL fCloseOnComplete)
{
	HRESULT hr = S_OK;
	DWORD dwLastTime = 0, dwCurrentTime = 0, dwCurrentDuration = 0, dwWaitTimeLeft = dwMaxWaitTime;
	DWORD dwInternalPollCounter = 0;
	HRESULT hrContinue;

	if(pfCompleted)
		*pfCompleted = FALSE;

	dwLastTime = GetTickCount();

	do
	{

		if((hEvent != INVALID_HANDLE_VALUE) && (hEvent != NULL))
			WaitForSingleObject(hEvent, dwWaitTimeLeft);

		hrContinue = XOnlineTaskContinue(hTask);

		// Update the poll-counter
		++dwInternalPollCounter;

		if (XONLINETASK_S_RUNNING != hrContinue)	//if hrDoWorks is not XONLINETASK_S_RUNNING(0x0)
		{	
		 	if (XONLINETASK_S_SUCCESS == hrContinue)	//if hrDoWorks is XONLINETASK_S_SUCCESS(0x1)
			{
				if(pfCompleted)
				{
					*pfCompleted = TRUE;
				}
			}
		  	else if (FAILED(hTask))
		    	{
		        	hr = hrContinue;
		    	}

			if(fCloseOnComplete)
			{
				XOnlineTaskClose(hTask);
			}	
		    	
		    break;
		}		

		if(dwMaxWaitTime != INFINITE)
		{
			// See if we have gone beyond the maximum allowed time
			dwCurrentTime = GetTickCount();
			if(dwCurrentTime < dwLastTime)
			{
				dwCurrentDuration = ((MAXDWORD - dwLastTime) + dwCurrentTime);
			}
			else
			{
				dwCurrentDuration = (dwCurrentTime - dwLastTime);
			}
			
			if(dwCurrentDuration >= dwWaitTimeLeft)
			{
				DbgPrint("Async operation didn't complete within %u ms\n", dwMaxWaitTime);
				hr = E_FAIL;
				goto Exit;
			}
			
			dwWaitTimeLeft -= dwCurrentDuration;
			dwLastTime = dwCurrentTime;
		}
	} while(TRUE);

Exit:
	if(pdwPollCounter)
		*pdwPollCounter = dwInternalPollCounter;

	return hr;
}

//==================================================================================
// XOnlineLogonWrapper
//----------------------------------------------------------------------------------
//
// Description: XOnlineLogon wrapper code stolen from dev tests
//
// Arguments:
//	HANDLE				hLog				Handle to logging subsystem
//	HANDLE				hEvent				Event for Xbox Online task
//	PXONLINETASK_HANDLE	phLogon				Pointer that receives a handle to the Xbox Online service
//	BYTE				bLogonControllers	Byte whose low order 4 bits store which controllers are logging in
//	DWORD*          	pServices			Pointer to services for which credentials are desired
//	DWORD				cServices			Number of services
//	DWORD				dwMaxWaitTime		Maximum number of seconds to allow for logon to complete
//
// Returns: Passes back async completion result
//==================================================================================
HRESULT XOnlineLogonWrapper(HANDLE hLog, HANDLE hLogonEvent, PXONLINETASK_HANDLE phLogon, BYTE bLogonControllers, DWORD* pServices, DWORD cServices, DWORD dwMaxWaitTime)
{
	XONLINE_USER		*pLocalUsersArray;
	DWORD				dwNumLocalUsers = 0;
	HRESULT				hr = S_OK;
	DWORD				dwStartTime = 0, dwCurrentTime = 0, dwElapsedTime = 0, dwResultsCounter = 0;
	BYTE				bCurrentController = 0;
	HRESULT 			hrContinue;
	TCHAR 				szMsg[128];
	
	// Get local users (we assume that the hard-drive has been populated)
	pLocalUsersArray =  (XONLINE_USER *) LocalAlloc(LPTR,XONLINE_MAX_STORED_ONLINE_USERS * sizeof(XONLINE_USER));

    if(pLocalUsersArray)        
	    hr = XOnlineGetUsers( pLocalUsersArray, &dwNumLocalUsers );
    else
    {
        xLog(hLog, XLL_INFO, "Memory Allocation Fail");
        hr = E_FAIL;
        goto Exit;
    }

	if (FAILED(hr))
	{
		xLog(hLog, XLL_INFO, "Getting users from hard-drive failed with 0x%08x", hr);
		hr = E_FAIL;
		goto Exit;
	}

	if(dwNumLocalUsers < XONLINE_MAX_LOGON_USERS)
	{
		OutputDebugString(TEXT("Not enough users stored on hard-drive to run friend tests!"));
		xLog(hLog, XLL_INFO, "%u users necessary to run friend tests, but only %u found on hard-drive",
			XONLINE_MAX_LOGON_USERS, dwNumLocalUsers);
		hr = E_FAIL;
		goto Exit;
	}

	g_dwNumLoggedOnUsers = 0;
	memset(g_LoggedOnUsers, 0, sizeof(g_LoggedOnUsers));

	// Log users in for the requested controllers
	for(bCurrentController = 0; bCurrentController < XONLINE_MAX_LOGON_USERS; ++bCurrentController)
	{
		if(bLogonControllers & (0x000000001 << bCurrentController))
		{
			memcpy(g_LoggedOnUsers + bCurrentController, pLocalUsersArray + bCurrentController, sizeof(XONLINE_USER));
			++g_dwNumLoggedOnUsers;
		}
	}

    LocalFree(pLocalUsersArray);
    
	// Initialize services
	xLog(hLog, XLL_INFO, "Logging into %u services with %u users", cServices, g_dwNumLoggedOnUsers);
	hr = XOnlineLogon(g_LoggedOnUsers, pServices, cServices, hLogonEvent, phLogon);
	if (FAILED(hr))
        goto Exit;

	dwElapsedTime = 0;
	dwStartTime = GetTickCount();

	xLog(hLog, XLL_INFO, "Waiting for logon to complete");

	// Pump until logon succeeds (PARTIAL_RESULTS flag set) or fails (DONT_CONTINUE flag set)
	do
	{
		if( WaitForSingleObject( hLogonEvent, dwMaxWaitTime) != WAIT_OBJECT_0 )
		{
			xLog(hLog, XLL_INFO, "Waiting for logon event failed!");
			hr = E_FAIL;
			goto Exit;
		}

		hrContinue = XOnlineTaskContinue(*phLogon);
	} while ( XONLINETASK_S_RUNNING == hrContinue );		//while hrDoWorks is XONLINETASK_S_RUNNING(0x0)	

	wsprintf(szMsg, TEXT("XBudBVTNamespace__XOnlineLogonWrapper: XOnlineTaskContinue returned %0x at last\n"),hrContinue);
	OutputDebugString(szMsg);

	hr = XOnlineLogonTaskGetResults(*phLogon);
	xLog(hLog, XLL_INFO, "Logon returned 0x%08x", hr);

	if (hr != XONLINE_S_LOGON_CONNECTION_ESTABLISHED)
	{
		xLog(hLog, XLL_INFO, "Logon failed asynchronously");
		hr = E_FAIL;
		goto Exit;
	}

	xLog(hLog, XLL_INFO, "Logon completed successfully");
	
Exit:

	return(hr);
}			

//==================================================================================
// XBudBVTDllMain
//----------------------------------------------------------------------------------
//
// Description: DLL entry
//
// Arguments:
//	HINSTANCE	hInstance		Handle to module
//	DWORD		dwReason		Indicates the reason for calling the function
//	LPVOID		lpContext		reserved
// Returns:
//	TRUE on success
//==================================================================================
BOOL WINAPI XBudBVTDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
	WSADATA WSAData;
	XNADDR xnaddr;
	DWORD dwElapsedTime = 0, dwStartTime = 0, dwCurrentTime = 0, dwResult = 0;

	// We'll initialize/delete the global critical section here
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		XNetAddRef();
		
		dwElapsedTime = 0;
		dwStartTime = GetTickCount();
		
		// Wait until stack initialization completes
		do
		{
			dwCurrentTime = GetTickCount();
			
			// Calculate the elapsed time based on whether the time has wrapped around
			if(dwCurrentTime < dwStartTime)
				dwElapsedTime =  MAXDWORD - dwStartTime + dwCurrentTime;
			else
				dwElapsedTime = dwCurrentTime - dwStartTime;
			
			if(dwElapsedTime > 10000)
			{
				OutputDebugString(TEXT("Net stack failed to initialize"));
				return FALSE;
			}
			
			dwResult = XNetGetTitleXnAddr (&xnaddr);		
			
		} while (XNET_GET_XNADDR_PENDING == dwResult);

		WSAStartup(MAKEWORD(2, 2), &WSAData);
		g_hHeap = HeapCreate(0,0,0);
		break;
	case DLL_PROCESS_DETACH:
		WSACleanup();
		XNetRelease();
		HeapDestroy(g_hHeap);
		g_hHeap = NULL;
		break;
	default:
		break;
	}

    return TRUE;
}

BOOL RemoveAllFriends(HANDLE hLog, DWORD dwUserIndex, XONLINE_FRIEND *pFriendList)
{
	HRESULT hr = S_OK;
	DWORD dwCurrentFriend = 0, dwNumFriends = 0;

	for(dwCurrentFriend = 0; dwCurrentFriend < MAX_FRIENDS; ++dwCurrentFriend)
	{
		if(pFriendList[dwCurrentFriend].xuid.qwUserID != 0)
			++dwNumFriends;
	}

	for(dwCurrentFriend = 0; dwCurrentFriend < dwNumFriends; ++dwCurrentFriend)
	{

		xLog(hLog, XLL_INFO, "Removing friend %u from user %u", dwCurrentFriend + 1, dwUserIndex + 1);

		hr = XOnlineFriendsRemove(dwUserIndex, pFriendList + dwCurrentFriend);
		if(hr != S_OK)
		{
			xLog(hLog, XLL_FAIL, "XOnlineFriendsRemove failed unexpectedly");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL RemoveAllLockoutlist(HANDLE hLog, DWORD dwUserIndex, DWORD dwNumLockoutUser, 
							PXONLINE_LOCKOUTUSER pLockoutUser)
{
	HRESULT hr = S_OK;
	DWORD dwCurrentFriend = 0, dwNumFriends = 0;

	for( DWORD ii = 0; ii < dwNumLockoutUser; ++ii)
	{

		xLog(hLog, XLL_INFO, "Removing lockout user %u from user %u", ii + 1, dwUserIndex + 1);

		hr = XOnlineLockoutlistRemove(dwUserIndex, pLockoutUser[ii].xuid);
		if(hr != S_OK)
		{
			xLog(hLog, XLL_INFO, "XOnlineLockoutlistRemove returned %0x", hr);
			xLog(hLog, XLL_FAIL, "XOnlineLockoutlistRemove failed unexpectedly");
			return FALSE;
		}
	}

	return TRUE;
}


DWORD DisplayFriendList(HANDLE hLog, DWORD dwNumFriends, XONLINE_FRIEND *pFriendList)
{
	DWORD dwCurrentFriend = 0;

	xLog(hLog, XLL_INFO, "List has %u friends", dwNumFriends);

	for(dwCurrentFriend = 0; dwCurrentFriend < dwNumFriends; ++dwCurrentFriend)
	{
		if(pFriendList[dwCurrentFriend].xuid.qwUserID != 0)
		{
			xLog(hLog, XLL_INFO, "%s 0x%016I64x (S 0x%08x;T 0x%08x;TI %u)",
				pFriendList[dwCurrentFriend].username,
				pFriendList[dwCurrentFriend].xuid.qwUserID,
				pFriendList[dwCurrentFriend].friendState,
				pFriendList[dwCurrentFriend].titleID,
				pFriendList[dwCurrentFriend].StateDataSize);
			xLog(hLog, XLL_INFO, "    Session: 0x%016I64x", pFriendList[dwCurrentFriend].sessionID);
		}
	}

	return dwNumFriends;
}

//==================================================================================
// XBudBVTStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================

VOID WINAPI XBudBVTStartTest(IN HANDLE  hLog)
{
//	XONLINE_FRIENDS_RESULTS_ERROR friendError;
	XONLINETASK_HANDLE hLogonTask = NULL, hEnumGamesTask[XONLINE_MAX_LOGON_USERS] = {NULL,}, hGameOpTask = NULL, hLockoutTask = NULL;
	XONLINE_FRIEND *pCurrentFriendList = NULL, *pFriendListExpected;
	LPBYTE pFriendBuffers = NULL;
//	XONLINE_SERVICE ServicesArray[] =
//	{
//		{ XONLINE_MATCHMAKING_SERVICE, S_OK, NULL }
//	};
//	DWORD dwServices = 0;
	XNKID SessionID;
	DWORD dwUserIndex = 0, dwEventType = 0, dwCounter1 = 0, dwCounter2 = 0, dwNumFriends = 0, dwNumOfExpectedFriend, dwNumLockoutUsers;
	HRESULT hr = S_OK;
	HANDLE hLogonEvent = NULL, hFriendOpEvent = NULL;
	BYTE byStateData1[MAX_STATEDATA_SIZE], byStateData2[MAX_STATEDATA_SIZE];
	HRESULT 	hrContinue, hReturn;
	BOOL bExpectedResult = FALSE;
	DWORD dwStartTickCount;
	PXONLINE_LOCKOUTUSER pLockoutlist;
	SY_RESULT		nResult;

	hr = XOnlineStartup(NULL);
	Verify(hr == S_OK);
    
	pFriendBuffers = (LPBYTE)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * MAX_FRIENDS * XONLINE_MAX_LOGON_USERS);
	if(!pFriendBuffers)
	{
		xLog(hLog, XLL_FAIL, "Couldn't allocate friend buffers");
		goto Exit;
	}

	memset(byStateData1, 0, MAX_STATEDATA_SIZE);
	memset(byStateData2, 0, MAX_STATEDATA_SIZE);
	memset(pFriendBuffers, 0, sizeof(XONLINE_FRIEND) * MAX_FRIENDS * XONLINE_MAX_LOGON_USERS);
	memset(&SessionID, 0, sizeof(SessionID));
	memset(hEnumGamesTask, 0, sizeof(hEnumGamesTask));
//	memset(&friendError, 0, sizeof(friendError));

	xSetOwnerAlias(hLog, "styoo");
    xSetComponent(hLog, "Online", "Friends - Client (O)");

	hLogonEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hFriendOpEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hLogonEvent || !hFriendOpEvent)
	{
		xLog(hLog, XLL_FAIL, "Couldn't create events for friends BVT");
		goto Exit;
	}

#if SY_NEED_POPULATE_USERS	
	hr = COnlineAccounts::Instance()->PopulateUserAccountsHD(NULL, XONLINE_MAX_LOGON_USERS);
	if(hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "Populating user accounts failed.");
		goto Exit;
	}
#endif

#if SY_NEED_CREATE_USERS	
	hr = SY_CreateUserAccounts(XONLINE_MAX_LOGON_USERS);
	if(hr != S_OK)
	{
		xLog(hLog, XLL_FAIL, "Creating user accounts failed.");
		goto Exit;
	}
#endif
	



// Handle logging on to necessary services
//	dwServices = (sizeof(ServicesArray)/sizeof(XONLINE_SERVICE));

	hr = XOnlineLogonWrapper(hLog, hLogonEvent, &hLogonTask, 0x0F, NULL, 0, 300000);
//	hr = XOnlineLogonWrapper(hLog, hLogonEvent, &hLogonTask, 0x0F, ServicesArray, dwServices, 30000);
	if(hr != XONLINE_S_LOGON_CONNECTION_ESTABLISHED)
	{
		xLog(hLog, XLL_FAIL, "Failed logon.");
		goto Exit;
	}

	for(dwUserIndex = 0; dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		xLog(hLog, XLL_INFO, "Logged on user %u is 0x%16I64x", dwUserIndex + 1, g_LoggedOnUsers[dwUserIndex].xuid.qwUserID);
	}

#if	BUG10049REGRESS	
//===================================================================	
	xSetFunctionName(hLog, "XOnlineNotificationSetState");
	START_TESTCASE("Set status of all friends as online/cloaked");
//===================================================================
	for(dwUserIndex = 0; dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		xLog(hLog, XLL_INFO, "Setting state for user %u", dwUserIndex + 1);

		if(XOnlineNotificationSetState(dwUserIndex, XONLINE_FRIENDSTATE_FLAG_ONLINE | XONLINE_FRIENDSTATE_FLAG_CLOAKED, 
										SessionID, 0, NULL))
		{
			bExpectedResult = TRUE;
		}
		else
		{
			bExpectedResult = FALSE;
			FAIL_TESTCASE("XOnlineNotificationSetState returned FALSE")
			break;
		}
	}

	if( bExpectedResult )
	{
		PASS_TESTCASE("Set online state for all users")
	}
	else
	{
		FAIL_TESTCASE("XOnlineNotificationSetState failed unexpectedly")
	}
		
	
	END_TESTCASE
	xSetFunctionName(hLog, "");
#endif

//===================================================================	
	xSetFunctionName(hLog, "XOnlineNotificationSetState");
	START_TESTCASE("Set status of all friends as online");
//===================================================================
	for(dwUserIndex = 0; dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		xLog(hLog, XLL_INFO, "Setting state for user %u", dwUserIndex + 1);

		if(XOnlineNotificationSetState(dwUserIndex, XONLINE_FRIENDSTATE_FLAG_ONLINE, SessionID, 0, NULL))
		{
			bExpectedResult = TRUE;
		}
		else
		{
			bExpectedResult = FALSE;
			FAIL_TESTCASE("XOnlineNotificationSetState returned FALSE")
			break;
		}
	}

	if( bExpectedResult )
	{
		PASS_TESTCASE("Set online state for all users")
	}
	else
	{
		FAIL_TESTCASE("XOnlineNotificationSetState failed unexpectedly")
	}
		
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================	
	xSetFunctionName(hLog, "XOnlineNotificationSetUserData");
	START_TESTCASE("Set user data of all friends as online");
//===================================================================

	BYTE	byUserData[] = "abcxyz";
	XONLINETASK_HANDLE hTaskUserData = NULL;
	BOOL	fDone;
	
	for(dwUserIndex = 0; dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		xLog(hLog, XLL_INFO, "Setting user data for user %u", dwUserIndex + 1);

		hr = XOnlineNotificationSetUserData(dwUserIndex, strlen((char*)byUserData), (PBYTE)byUserData, NULL, &hTaskUserData);
		
		if(SUCCEEDED(hr))
		{
			bExpectedResult = TRUE;
		}
		else
		{
			bExpectedResult = FALSE;
			xLog(hLog, XLL_INFO, "XOnlineNotificationSetUserData returned 0x%08x", hr);
			FAIL_TESTCASE("XOnlineNotificationSetUserData failed unexpectedly")
			break;
		}

		hr = PollTaskUntilComplete(hTaskUserData, NULL, ALLOWEDPUMPINGTIME, NULL, &fDone, FALSE);
		if(FAILED(hr))
		{
			bExpectedResult = FALSE;
			xLog(hLog, XLL_INFO, "Pumping XOnlineNotificationSetUserData returned 0x%08x", hr);
			FAIL_TESTCASE("Pumping XOnlineNotificationSetUserData failed asynchronously");
			break;
		}
		else if (!fDone)
		{
			bExpectedResult = FALSE;
			FAIL_TESTCASE("Pumping XOnlineNotificationSetUserData didn't complete within 30 seconds");
			break;
		}
		
	}

	if( bExpectedResult )
	{
		PASS_TESTCASE("Set online state for all users")
	}
	else
	{
		FAIL_TESTCASE("Setting online state for all users failed");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");


//===================================================================
	xSetFunctionName(hLog, "XOnlineNotificationIsPending");
	START_TESTCASE("Check the notification status for each user")
//===================================================================

	for(dwUserIndex = 0; dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		xLog(hLog, XLL_INFO, "Checking notifications for user %u", dwUserIndex + 1);

		if(XOnlineNotificationIsPending(dwUserIndex, XONLINE_NOTIFICATION_TYPE_ALL))
			xLog(hLog, XLL_INFO, "Events pending!");
		else
			xLog(hLog, XLL_INFO, "No events pending");

		PUMP_CS_AND_EXIT_IF_FAILED
	}

	PASS_TESTCASE("Checked notifications for all users")

	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineNotificationEnumerate");
	START_TESTCASE("Check if XOnlineNotificationEnumerate returns E_NOTIMPL ")
//===================================================================

	XONLINE_NOTIFICATION_MSG	NotifiMsg;
	XONLINETASK_HANDLE			hTaskNoficationEnum;

	if(E_NOTIMPL == XOnlineNotificationEnumerate(0, &NotifiMsg, sizeof(XONLINE_NOTIFICATION_MSG), 
													XONLINE_NOTIFICATION_TYPE_ALL, NULL, &hTaskNoficationEnum))
		PASS_TESTCASE("XOnlineNotificationEnumerate returned E_NOTIMPL")
	else
		FAIL_TESTCASE("XOnlineNotificationEnumerate didn't return E_NOTIMPL")
	PUMP_CS_AND_EXIT_IF_FAILED

	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsEnumerate");
	START_TESTCASE("Enumerate friends for each user");
//===================================================================

	bExpectedResult = TRUE;
	for(dwUserIndex = 0; bExpectedResult && dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		xLog(hLog, XLL_INFO, "Enumerating friends for user %u", dwUserIndex + 1);

		hr = XOnlineFriendsEnumerate(dwUserIndex, NULL, hEnumGamesTask + dwUserIndex);
		if(hr == S_OK)
		{
			bExpectedResult = TRUE;
		}
		else
		{
			FAIL_TESTCASE("XOnlineFriendsEnumerate failed unexpectedly")
			bExpectedResult = FALSE;
		}

		PUMP_CS_AND_EXIT_IF_FAILED
	}

	if(bExpectedResult)
	{
		nResult = PumpFriendTask(hLog, hLogonTask, 0, hEnumGamesTask, 0, 1, &hReturn);
		if(nResult == SY_STOPWHOLETEST)	// Pump LogonTask failed
		{
			xEndVariation(hLog);
			goto Exit;
		}
		else if(nResult == SY_STOPTESTCASE)
			bExpectedResult = FALSE;
	}

	if(bExpectedResult)
	{
		PASS_TESTCASE("Started enumerating friend lists of all users");
	}
	else
	{
		FAIL_TESTCASE("Starting enumerating friend lists of all users failed");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsGetLatest");
	START_TESTCASE("Get friend list for each user #1");
//===================================================================

	for(dwUserIndex = 0; dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		xLog(hLog, XLL_INFO, "Getting friend list for user %u", dwUserIndex + 1);

		pCurrentFriendList = (PXONLINE_FRIEND) (pFriendBuffers + (dwUserIndex * sizeof(XONLINE_FRIEND) * MAX_FRIENDS));

		memset(pCurrentFriendList, 0, sizeof(XONLINE_FRIEND) * MAX_FRIENDS);

		dwNumFriends = XOnlineFriendsGetLatest(dwUserIndex, MAX_FRIENDS, pCurrentFriendList);

		DisplayFriendList(hLog, dwNumFriends, pCurrentFriendList);

		PUMP_CS_AND_EXIT_IF_FAILED
	}

	PASS_TESTCASE("Retrieved friend list for all users")

	END_TESTCASE
	xSetFunctionName(hLog, "");



//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsStartup");
	START_TESTCASE("Initializing friend operation handle");
//===================================================================

	hr = XOnlineFriendsStartup(hFriendOpEvent, &hGameOpTask);
	if(hr == S_OK)
	{
		bExpectedResult = TRUE;
	}
	else
	{
		bExpectedResult = FALSE;
	}

	if(bExpectedResult == TRUE)
	{
		PASS_TESTCASE("Initialized friend operation handle")
	}
	else
	{
		FAIL_TESTCASE("XOnlineFriendsStartup failed unexpectedly")
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineLockoutlistStartup");
	START_TESTCASE("Initializing lockoutlist operation handle");
//===================================================================

	hr = XOnlineLockoutlistStartup(NULL, &hLockoutTask);
	if(hr == S_OK)
	{
		bExpectedResult = TRUE;
	}
	else
	{
		bExpectedResult = FALSE;
	}

	if(bExpectedResult == TRUE)
	{
		PASS_TESTCASE("Initialized lockoutlist operation handle")
	}
	else
	{
		FAIL_TESTCASE("XOnlineLockoutlistStartup failed unexpectedly")
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

#if 1
//===================================================================
	xSetFunctionName(hLog, "XOnlineLockoutlistAdd");
	START_TESTCASE("Add user2 in user1's Lockout list");
//===================================================================

	hr = XOnlineLockoutlistAdd(0,g_LoggedOnUsers[1].xuid, g_LoggedOnUsers[1].name);

	if(hr == S_OK)
	{
		PASS_TESTCASE("XOnlineLockoutlistAdd succeeded")
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineLockoutlistAdd returned %0x", hr);
		FAIL_TESTCASE("XOnlineLockoutlistAdd failed");
		bExpectedResult = FALSE;
	}
	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineLockoutlistGet");
	START_TESTCASE("Check if user2 is in user1's Lockout list");
//===================================================================
	bExpectedResult = FALSE;
	dwNumLockoutUsers = 0;
	dwStartTickCount = GetTickCount();
	
	pLockoutlist = (PXONLINE_LOCKOUTUSER) LocalAlloc(0, MAX_LOCKOUTUSERS * sizeof(XONLINE_LOCKOUTUSER));

	memset(pLockoutlist, 0, MAX_LOCKOUTUSERS * sizeof(XONLINE_LOCKOUTUSER));

	while(!bExpectedResult && !IsTimeFinished(dwStartTickCount, ALLOWEDPUMPINGTIME))
	{
		bExpectedResult = TRUE;

		nResult = PumpFriendTask(hLog, hLogonTask, hGameOpTask, hEnumGamesTask, hLockoutTask, 0, &hReturn);
		if(nResult == SY_STOPWHOLETEST)	// Pump LogonTask failed
		{
			xEndVariation(hLog);
			goto Exit;
		}
		else if(nResult == SY_STOPTESTCASE)
		{
			bExpectedResult = FALSE;
			break;
		}
		
		if(bExpectedResult)
		{
			dwNumLockoutUsers = XOnlineLockoutlistGet( 0, MAX_LOCKOUTUSERS, pLockoutlist );

			bExpectedResult = IsLockoutUser(dwNumLockoutUsers, pLockoutlist, g_LoggedOnUsers[1].xuid);

		}

	}
	
	if(bExpectedResult)
	{
		PASS_TESTCASE("Found lockout user after adding")
	}
	else
	{
		FAIL_TESTCASE("Couldn't find lockout user after adding");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineLockoutlistRemove");
	START_TESTCASE("Remove all lockout list for user 1");
//===================================================================

	bExpectedResult = TRUE;

	if( RemoveAllLockoutlist(hLog, 0, dwNumLockoutUsers, pLockoutlist) )
	{
		bExpectedResult = TRUE;
	}
	else
	{
		bExpectedResult = FALSE;
	}			

	LocalFree(pLockoutlist);
	
	if(bExpectedResult)
	{
		PASS_TESTCASE("Removed all lockout list for user 1")
	}
	else
	{
		FAIL_TESTCASE("Couldn't remove lockout list");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineLockoutlistGet");
	START_TESTCASE("Checking if all users are removed from user1's Lockout list");
//===================================================================
	bExpectedResult = FALSE;
	dwStartTickCount = GetTickCount();
	
	pLockoutlist = (PXONLINE_LOCKOUTUSER) LocalAlloc(0, MAX_LOCKOUTUSERS * sizeof(XONLINE_LOCKOUTUSER));

	memset(pLockoutlist, 0, MAX_LOCKOUTUSERS * sizeof(XONLINE_LOCKOUTUSER));

	while(!bExpectedResult && !IsTimeFinished(dwStartTickCount, ALLOWEDPUMPINGTIME))
	{
		bExpectedResult = TRUE;

		nResult = PumpFriendTask(hLog, hLogonTask, hGameOpTask, hEnumGamesTask, hLockoutTask, 0, &hReturn);
		if(nResult == SY_STOPWHOLETEST)	// Pump LogonTask failed
		{
			xEndVariation(hLog);
			goto Exit;
		}
		else if(nResult == SY_STOPTESTCASE)
		{
			bExpectedResult = FALSE;
			break;
		}
		
		if(bExpectedResult)
		{
			dwNumLockoutUsers = XOnlineLockoutlistGet( 0, MAX_LOCKOUTUSERS, pLockoutlist );

			if(dwNumLockoutUsers == 0)
				bExpectedResult = TRUE;
			else
				bExpectedResult = FALSE;

		}

	}

	LocalFree(pLockoutlist);
	
	if(bExpectedResult)
	{
		PASS_TESTCASE("Confirmed all lockout users are gone for user1")
	}
	else
	{
		FAIL_TESTCASE("Fail to confirm all lockout users are gone for user1");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

// Need to add codes to remove all lockoutlists	
#else
//===================================================================
	xSetFunctionName(hLog, "XOnlineLockoutlistGet");
	START_TESTCASE("Remove all Lockout list for user 2, 3 and 4");
//===================================================================
	bExpectedResult = TRUE;

	for(dwUserIndex = 1; bExpectedResult && dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		bExpectedResult = GetAndRemoveAllLockoutlist(hLog, dwUserIndex);
	}

	if(bExpectedResult)
	{
		PASS_TESTCASE("Confirmed all lockout users are gone for user 2, 3 and 4")
	}
	else
	{
		FAIL_TESTCASE("Fail to confirm all lockout users are gone for user 2, 3 and 4");
	}


	BOOL GetAndRemoveAllLockoutlist(HANDLE hLog, DWORD dwUserIndex)
	{
		PXONLINE_LOCKOUTUSER	pLockoutlist;
		
		pLockoutlist = (PXONLINE_LOCKOUTUSER) LocalAlloc(0, MAX_LOCKOUTUSERS * sizeof(XONLINE_LOCKOUTUSER));

		memset(pLockoutlist, 0, MAX_LOCKOUTUSERS * sizeof(XONLINE_LOCKOUTUSER));

		dwNumLockoutUsers = XOnlineLockoutlistGet( 0, MAX_LOCKOUTUSERS, pLockoutlist );

		dwStartTickCount = GetTickCount();

		if( RemoveAllLockoutlist(hLog, dwUserIndex, dwNumLockoutUsers, pLockoutlist) )
		{
			bExpectedResult = TRUE;
		}
		else
		{
			xLog(hLog, XLL_INFO, "Fail in RemoveAllLockoutlist");
			return FALSE;
		}	

		while(dwNumLockoutUsers > 0 && !IsTimeFinished(dwStartTickCount, ALLOWEDPUMPINGTIME))
		{
			nResult = PumpFriendTask(hLog, hLogonTask, hGameOpTask, hEnumGamesTask, hLockoutTask, 0, &hReturn);
		if(nResult == SY_STOPWHOLETEST)	// Pump LogonTask failed
		{
			xEndVariation(hLog);
			goto Exit;
		}
		else if(nResult == SY_STOPTESTCASE)
			bExpectedResult = FALSE;
			dwNumLockoutUsers = XOnlineLockoutlistGet( 0, MAX_LOCKOUTUSERS, pLockoutlist );
		}

		LocalFree(pLockoutlist);

		if(dwNumLockoutUsers == 0)
			return TRUE;
		else
			return FALSE;
	}

	END_TESTCASE
	xSetFunctionName(hLog, "");

#endif

//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsRemove");
	START_TESTCASE("Remove all existing friends from each user");
//===================================================================

	bExpectedResult = TRUE;
	for(dwUserIndex = 0; bExpectedResult && dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		pCurrentFriendList = (PXONLINE_FRIEND) (pFriendBuffers + (dwUserIndex * sizeof(XONLINE_FRIEND) * MAX_FRIENDS));


		if(RemoveAllFriends(hLog, dwUserIndex, pCurrentFriendList))
		{

			bExpectedResult = TRUE;
		}
		else
		{

			FAIL_TESTCASE("Couldn't remove friends");
			bExpectedResult = FALSE;
		}			
	}

	if(bExpectedResult)
	{
		PASS_TESTCASE("Removed initial friend lists")
	}
	else
	{
		FAIL_TESTCASE("Removing initial friend lists failed");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");


//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsGetLatest");
	START_TESTCASE("Get friend list for each user #2")
//===================================================================

	bExpectedResult = FALSE;
	dwStartTickCount = GetTickCount();
	
	while(!bExpectedResult && !IsTimeFinished(dwStartTickCount, ALLOWEDPUMPINGTIME))
	{
		bExpectedResult = TRUE;

		nResult = PumpFriendTask(hLog, hLogonTask, hGameOpTask, hEnumGamesTask, hLockoutTask, 2, &hReturn);
		if(nResult == SY_STOPWHOLETEST)	// Pump LogonTask failed
		{
			xEndVariation(hLog);
			goto Exit;
		}
		else if(nResult == SY_STOPTESTCASE)
		{
			bExpectedResult = FALSE;		
			break;
		}
		for(dwUserIndex = 0; bExpectedResult && dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
		{
			xLog(hLog, XLL_INFO, "Getting friend list for user %u", dwUserIndex + 1);

			pCurrentFriendList = (PXONLINE_FRIEND) (pFriendBuffers + (dwUserIndex * sizeof(XONLINE_FRIEND) * MAX_FRIENDS));

			memset(pCurrentFriendList, 0, sizeof(XONLINE_FRIEND) * MAX_FRIENDS);

			dwNumFriends = XOnlineFriendsGetLatest(dwUserIndex, MAX_FRIENDS, pCurrentFriendList);

			if(dwNumFriends == 0)
			{
				bExpectedResult = TRUE;
			}
			else
			{
				bExpectedResult = FALSE;
			}

			PUMP_CS_AND_EXIT_IF_FAILED
		}
	}

	if(bExpectedResult)
	{
		PASS_TESTCASE("Retrieved friend list for all users")
	}
	else
	{
		FAIL_TESTCASE("Unexpected friends are in the list")
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");



	// Have user 1 invite users 2, 3 and 4
//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsRequest");
	START_TESTCASE("Inviting friends")
//===================================================================

	for(dwUserIndex = 1; dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		xLog(hLog, XLL_INFO, "User 1 is inviting user %u", dwUserIndex + 1);

		hr = XOnlineFriendsRequest(0, g_LoggedOnUsers[dwUserIndex].xuid);
		if(hr != S_OK){
			FAIL_TESTCASE("XOnlineFriendsRequest failed unexpectedly");
			bExpectedResult = FALSE;
			break;
		}

		PUMP_CS_AND_EXIT_IF_FAILED
	}

	if(bExpectedResult)
	{
		PASS_TESTCASE("Invited friends successfully")
	}
	else
	{
		FAIL_TESTCASE("Inviting friends failed")
	}

	END_TESTCASE
	xSetFunctionName(hLog, "");

	// Get friend list for each user
//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsGetLatest");
	START_TESTCASE("Get friend list for each user #3");
//===================================================================

	bExpectedResult = FALSE;
	dwStartTickCount = GetTickCount();

	while(!bExpectedResult && !IsTimeFinished(dwStartTickCount, ALLOWEDPUMPINGTIME))
	{
		bExpectedResult = TRUE;

		nResult = PumpFriendTask(hLog, hLogonTask, hGameOpTask, hEnumGamesTask, hLockoutTask, 3, &hReturn);
		if(nResult == SY_STOPWHOLETEST)	// Pump LogonTask failed
		{
			xEndVariation(hLog);
			goto Exit;
		}
		else if(nResult == SY_STOPTESTCASE)
		{
			bExpectedResult = FALSE;
			break;
		}
		
		for(dwUserIndex = 0; bExpectedResult && (dwUserIndex < XONLINE_MAX_LOGON_USERS); ++dwUserIndex)
		{


			xLog(hLog, XLL_INFO, "Getting friend list for user %u", dwUserIndex + 1);

			pCurrentFriendList = (PXONLINE_FRIEND) (pFriendBuffers + (dwUserIndex * sizeof(XONLINE_FRIEND) * MAX_FRIENDS));

			memset(pCurrentFriendList, 0, sizeof(XONLINE_FRIEND) * MAX_FRIENDS);

			dwNumFriends = XOnlineFriendsGetLatest(dwUserIndex, MAX_FRIENDS, pCurrentFriendList);

			DisplayFriendList(hLog, dwNumFriends, pCurrentFriendList);

			PUMP_CS_AND_EXIT_IF_FAILED

			// Check the specific friend list and verify that all entries are correct
			switch(dwUserIndex)
			{
			case 0:
				dwNumOfExpectedFriend = 3;
				
				pFriendListExpected = (XONLINE_FRIEND *)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );
				memset(pFriendListExpected , 0, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );

				pFriendListExpected[0].xuid.qwUserID = g_LoggedOnUsers[1].xuid.qwUserID;
				strcpy(pFriendListExpected[0].username, g_LoggedOnUsers[1].name);
				pFriendListExpected[0].friendState = XONLINE_FRIENDSTATE_FLAG_PENDING;
				pFriendListExpected[0].sessionID.ab[0] = 0;
				pFriendListExpected[0].titleID = 0;
				pFriendListExpected[0].StateDataSize = 0;

				pFriendListExpected[1].xuid.qwUserID = g_LoggedOnUsers[2].xuid.qwUserID;
				strcpy(pFriendListExpected[1].username, g_LoggedOnUsers[2].name);
				pFriendListExpected[1].friendState = XONLINE_FRIENDSTATE_FLAG_PENDING;
				pFriendListExpected[1].sessionID.ab[0] = 0;
				pFriendListExpected[1].titleID = 0;
				pFriendListExpected[1].StateDataSize = 0;
				
				pFriendListExpected[2].xuid.qwUserID = g_LoggedOnUsers[3].xuid.qwUserID;
				strcpy(pFriendListExpected[2].username, g_LoggedOnUsers[3].name);
				pFriendListExpected[2].friendState = XONLINE_FRIENDSTATE_FLAG_PENDING;
				pFriendListExpected[2].sessionID.ab[0] = 0;
				pFriendListExpected[2].titleID = 0;
				pFriendListExpected[2].StateDataSize = 0;

				bExpectedResult = IsValidResultForFriendList(hLog, dwNumOfExpectedFriend, dwNumFriends, pFriendListExpected, pCurrentFriendList);

				LocalFree(pFriendListExpected);
				break;

			case 1:
			case 2:
			case 3:
				dwNumOfExpectedFriend = 1;
				
				pFriendListExpected = (XONLINE_FRIEND *)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );
				memset(pFriendListExpected , 0, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );

				pFriendListExpected[0].xuid.qwUserID = g_LoggedOnUsers[0].xuid.qwUserID;
				strcpy(pFriendListExpected[0].username, g_LoggedOnUsers[0].name);
				pFriendListExpected[0].friendState = XONLINE_FRIENDSTATE_FLAG_REQUEST;
				pFriendListExpected[0].sessionID.ab[0] = 0;
				pFriendListExpected[0].titleID = 0;
				pFriendListExpected[0].StateDataSize = 0;

				bExpectedResult = IsValidResultForFriendList(hLog, dwNumOfExpectedFriend, dwNumFriends, pFriendListExpected, pCurrentFriendList);

				LocalFree(pFriendListExpected);
				break;

			default:
				FAIL_TESTCASE("Unexpected user index")
				bExpectedResult = FALSE;
				break;
			}
		}
	}
	
	// Verify presence of friends in user 0's list
	if(bExpectedResult)
	{
		for(dwCounter1 = 0; dwCounter1 < (XONLINE_MAX_LOGON_USERS - 1); ++dwCounter1)
		{
			pCurrentFriendList = (PXONLINE_FRIEND) pFriendBuffers;

			if(!pCurrentFriendList[dwCounter1].xuid.qwUserID)
			{
				xLog(hLog, XLL_INFO, "Friend %u missing from user 1's list.", dwCounter1 + 1);
				FAIL_TESTCASE("Expected friend missing")
				break;
			}
		}
	}
	else
	{
		FAIL_TESTCASE("Unxxpected friend list")
	}

	if(bExpectedResult)
	{
		PASS_TESTCASE("Retrieved friend lists for all users");
	}
	else
	{
		FAIL_TESTCASE("Retrieving friend lists for all users failed");
	}
	
	END_TESTCASE;
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsAnswerRequest");
	START_TESTCASE("Have user 2 accept user 1's invitation");
//===================================================================

	bExpectedResult = TRUE;
	pCurrentFriendList = (PXONLINE_FRIEND) (pFriendBuffers + (1 * sizeof(XONLINE_FRIEND) * MAX_FRIENDS));
	
	if(pCurrentFriendList[0].xuid.qwUserID != g_LoggedOnUsers[0].xuid.qwUserID)
	{
		xLog(hLog, XLL_INFO, "Friend 1 missing from user 2's list.");
		FAIL_TESTCASE("Expected friend missing");
		bExpectedResult = FALSE;
	}

	if(bExpectedResult)
	{
		hr = XOnlineFriendsAnswerRequest(1, &pCurrentFriendList[0], XONLINE_REQUEST_YES);
		if(hr != S_OK)
		{
			FAIL_TESTCASE("XOnlineFriendsAnswerRequest failed unexpectedly");
			bExpectedResult = FALSE;
		}
	}
	
	if(bExpectedResult)
	{
		PASS_TESTCASE("XOnlineFriendsAnswerRequest returned success");
	}
	else
	{
		FAIL_TESTCASE("XOnlineFriendsAnswerRequest returned fail");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsAnswerRequest");
	START_TESTCASE("Have user 3 reject user 1's invitation");
//===================================================================

	bExpectedResult = TRUE;
	pCurrentFriendList = (PXONLINE_FRIEND) (pFriendBuffers + (2 * sizeof(XONLINE_FRIEND) * MAX_FRIENDS));
	
	if(pCurrentFriendList[0].xuid.qwUserID != g_LoggedOnUsers[0].xuid.qwUserID)
	{
		xLog(hLog, XLL_INFO, "Friend 1 missing from user 3's list.");
		FAIL_TESTCASE("Expected friend missing");
		bExpectedResult = FALSE;
	}

	if(bExpectedResult)
	{
		hr = XOnlineFriendsAnswerRequest(2, &pCurrentFriendList[0], XONLINE_REQUEST_NO);
		if(hr != S_OK)
		{
			FAIL_TESTCASE("XOnlineFriendsAnswerRequest failed unexpectedly");
			bExpectedResult = FALSE;
		}
	}
	
	if(bExpectedResult)
	{
		PASS_TESTCASE("XOnlineFriendsAnswerRequest returned success");
	}
	else
	{
		FAIL_TESTCASE("XOnlineFriendsAnswerRequest returned fail");
	}
		
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

#if SY_BLOCKING_IMPLEMENTED
//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsAnswerRequest");
	START_TESTCASE("Have user 4 block user 1's invitation");
//===================================================================

	pCurrentFriendList = (PXONLINE_FRIEND) (pFriendBuffers + (3 * sizeof(XONLINE_FRIEND) * MAX_FRIENDS));
	
	if(pCurrentFriendList[0].xuid.qwUserID != g_LoggedOnUsers[0].xuid.qwUserID)
	{
		xLog(hLog, XLL_INFO, "Friend 1 missing from user 4's list.");
		FAIL_TESTCASE("Expected friend missing");
	}

	hr = XOnlineFriendsAnswerRequest(3, &pCurrentFriendList[0], XONLINE_REQUEST_BLOCK);

	if(hr == S_OK)
	{
		PASS_TESTCASE("XOnlineFriendsAnswerRequest returned success");
	}
	{
		FAIL_TESTCASE("XOnlineFriendsAnswerRequest failed unexpectedly");
	}

	END_TESTCASE
	xSetFunctionName(hLog, "");
#endif

//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsGetLatest");
	START_TESTCASE("Get friend list for each user #4");
//===================================================================

	bExpectedResult = FALSE;
	dwStartTickCount = GetTickCount();

	while(!bExpectedResult && !IsTimeFinished(dwStartTickCount, ALLOWEDPUMPINGTIME))
	{
		bExpectedResult = TRUE;

		nResult = PumpFriendTask(hLog, hLogonTask, hGameOpTask, hEnumGamesTask, hLockoutTask, 4, &hReturn);
		if(nResult == SY_STOPWHOLETEST)	// Pump LogonTask failed
		{
			xEndVariation(hLog);
			goto Exit;
		}
		else if(nResult == SY_STOPTESTCASE)
		{
			bExpectedResult = FALSE;		
			break;
		}
		for(dwUserIndex = 0; bExpectedResult && (dwUserIndex < XONLINE_MAX_LOGON_USERS); ++dwUserIndex)
		{
			xLog(hLog, XLL_INFO, "Getting friend list for user %u", dwUserIndex + 1);

			pCurrentFriendList = (PXONLINE_FRIEND) (pFriendBuffers + (dwUserIndex * sizeof(XONLINE_FRIEND) * MAX_FRIENDS));

			memset(pCurrentFriendList, 0, sizeof(XONLINE_FRIEND) * MAX_FRIENDS);

			dwNumFriends = XOnlineFriendsGetLatest(dwUserIndex, MAX_FRIENDS, pCurrentFriendList);

			DisplayFriendList(hLog, dwNumFriends, pCurrentFriendList);

			PUMP_CS_AND_EXIT_IF_FAILED

			// Check the specific friend list and verify that all entries are correct
			switch(dwUserIndex)
			{
			case 0:
#if SY_BLOCKING_IMPLEMENTED				
				dwNumOfExpectedFriend = 1;
				
				pFriendListExpected = (XONLINE_FRIEND *)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );
				memset(pFriendListExpected , 0, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );

				pFriendListExpected[0].xuid.qwUserID = g_LoggedOnUsers[1].xuid.qwUserID;
				strcpy(pFriendListExpected[0].username, g_LoggedOnUsers[1].name);
				pFriendListExpected[0].friendState = XONLINE_FRIENDSTATE_FLAG_ONLINE;
				pFriendListExpected[0].sessionID.ab[0] = 0;
				pFriendListExpected[0].titleID = HARNESS_TITLE_ID;
				pFriendListExpected[0].StateDataSize = 0;
#else
				dwNumOfExpectedFriend = 2;
				
				pFriendListExpected = (XONLINE_FRIEND *)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );
				memset(pFriendListExpected , 0, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );

				pFriendListExpected[0].xuid.qwUserID = g_LoggedOnUsers[1].xuid.qwUserID;
				strcpy(pFriendListExpected[0].username, g_LoggedOnUsers[1].name);
				pFriendListExpected[0].friendState = XONLINE_FRIENDSTATE_FLAG_ONLINE;
				pFriendListExpected[0].sessionID.ab[0] = 0;
				pFriendListExpected[0].titleID = HARNESS_TITLE_ID;
				pFriendListExpected[0].StateDataSize = 0;
				
				pFriendListExpected[1].xuid.qwUserID = g_LoggedOnUsers[3].xuid.qwUserID;
				strcpy(pFriendListExpected[1].username, g_LoggedOnUsers[3].name);
				pFriendListExpected[1].friendState = XONLINE_FRIENDSTATE_FLAG_PENDING;
				pFriendListExpected[1].sessionID.ab[0] = 0;
				pFriendListExpected[1].titleID = 0;
				pFriendListExpected[1].StateDataSize = 0;
#endif				

				bExpectedResult = IsValidResultForFriendList(hLog, dwNumOfExpectedFriend, dwNumFriends, pFriendListExpected, pCurrentFriendList);

				LocalFree(pFriendListExpected);
				break;				

			case 1:
				dwNumOfExpectedFriend = 1;
				
				pFriendListExpected = (XONLINE_FRIEND *)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );
				memset(pFriendListExpected , 0, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );

				pFriendListExpected[0].xuid.qwUserID = g_LoggedOnUsers[0].xuid.qwUserID;
				strcpy(pFriendListExpected[0].username, g_LoggedOnUsers[0].name);
				pFriendListExpected[0].friendState = XONLINE_FRIENDSTATE_FLAG_ONLINE;
				pFriendListExpected[0].sessionID.ab[0] = 0;
				pFriendListExpected[0].titleID = HARNESS_TITLE_ID;
				pFriendListExpected[0].StateDataSize = 0;

				bExpectedResult = IsValidResultForFriendList(hLog, dwNumOfExpectedFriend, dwNumFriends, pFriendListExpected, pCurrentFriendList);

				LocalFree(pFriendListExpected);
				break;
			case 2:
				if(dwNumFriends != 0){
					FAIL_TESTCASE("Unexpected number of friends in list");			
					bExpectedResult = FALSE;
				}
				break;
			case 3:
#if SY_BLOCKING_IMPLEMENTED				
				if(dwNumFriends != 0){
					FAIL_TESTCASE("Unexpected number of friends in list");			
					bExpectedResult = FALSE;
				}
#else
				dwNumOfExpectedFriend = 1;
				
				pFriendListExpected = (XONLINE_FRIEND *)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );
				memset(pFriendListExpected , 0, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );

				pFriendListExpected[0].xuid.qwUserID = g_LoggedOnUsers[0].xuid.qwUserID;
				strcpy(pFriendListExpected[0].username, g_LoggedOnUsers[0].name);
				pFriendListExpected[0].friendState = XONLINE_FRIENDSTATE_FLAG_REQUEST;
				pFriendListExpected[0].sessionID.ab[0] = 0;
				pFriendListExpected[0].titleID = 0;
				pFriendListExpected[0].StateDataSize = 0;

				bExpectedResult = IsValidResultForFriendList(hLog, dwNumOfExpectedFriend, dwNumFriends, pFriendListExpected, pCurrentFriendList);

				LocalFree(pFriendListExpected);
#endif				
				break;
			default:
				FAIL_TESTCASE("Unexpected user index");
				bExpectedResult = FALSE;
				break;
			}

		}
	}

	// TODO - check for list updates

	if(bExpectedResult)
	{
		PASS_TESTCASE("Retrieved friend lists for all users");
	}
	else
	{
		FAIL_TESTCASE("Retrieving friend lists for all users failed");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineNotificationSetState");
	START_TESTCASE("Set status for user 1");
//===================================================================

	bExpectedResult = TRUE;
	*((ULONGLONG *) &SessionID) = 0xAAAAAAAAAAAAAAAA;
	memset(byStateData1, 'A', MAX_STATEDATA_SIZE);

	if(!XOnlineNotificationSetState(0,
		XONLINE_FRIENDSTATE_FLAG_ONLINE | 
		XONLINE_FRIENDSTATE_FLAG_PLAYING | 
		XONLINE_FRIENDSTATE_FLAG_VOICE | 
		XONLINE_FRIENDSTATE_FLAG_JOINABLE, 
		SessionID, MAX_STATEDATA_SIZE, byStateData1))
	{
		FAIL_TESTCASE("XOnlineNotificationSetState failed unexpectedly");
		bExpectedResult = FALSE;
	}

	if(bExpectedResult)
	{
		PASS_TESTCASE("Set user 1's state");
	}
	else
	{
		FAIL_TESTCASE("Setting user 1's state failed");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineNotificationSetState");
	START_TESTCASE("Set status for user 2");
//===================================================================

	bExpectedResult = TRUE;
	*((ULONGLONG *) &SessionID) = 0xBBBBBBBBBBBBBBBB;
	memset(byStateData2, 'B', MAX_STATEDATA_SIZE);

	if(!XOnlineNotificationSetState(1,
		XONLINE_FRIENDSTATE_FLAG_ONLINE | 
		XONLINE_FRIENDSTATE_FLAG_PLAYING | 
		XONLINE_FRIENDSTATE_FLAG_VOICE | 
		XONLINE_FRIENDSTATE_FLAG_JOINABLE, 
		SessionID, 1, byStateData2))
	{
		FAIL_TESTCASE("XOnlineNotificationSetState failed unexpectedly");
		bExpectedResult = FALSE;
	}

	if(bExpectedResult)
	{
		PASS_TESTCASE("Set user 2's state");
	}
	else
	{
		FAIL_TESTCASE("Setting user 2's state failed");
	}
	END_TESTCASE
	xSetFunctionName(hLog, "");


//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsGetLatest");
	START_TESTCASE("Get friend lists for each user #5");
//===================================================================

	bExpectedResult = FALSE;
	dwStartTickCount = GetTickCount();

	while(!bExpectedResult && !IsTimeFinished(dwStartTickCount, ALLOWEDPUMPINGTIME))
	{
		bExpectedResult = TRUE;

		nResult = PumpFriendTask(hLog, hLogonTask, hGameOpTask, hEnumGamesTask, hLockoutTask, 5, &hReturn);
		if(nResult == SY_STOPWHOLETEST)	// Pump LogonTask failed
		{
			xEndVariation(hLog);
			goto Exit;
		}
		else if(nResult == SY_STOPTESTCASE)
		{
			bExpectedResult = FALSE;
			break;
		}
		
		for(dwUserIndex = 0; bExpectedResult && dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
		{
			xLog(hLog, XLL_INFO, "Getting friend list for user %u", dwUserIndex + 1);

			pCurrentFriendList = (PXONLINE_FRIEND) (pFriendBuffers + (dwUserIndex * sizeof(XONLINE_FRIEND) * MAX_FRIENDS));

			memset(pCurrentFriendList, 0, sizeof(XONLINE_FRIEND) * MAX_FRIENDS);

			dwNumFriends = XOnlineFriendsGetLatest(dwUserIndex, MAX_FRIENDS, pCurrentFriendList);

			DisplayFriendList(hLog, dwNumFriends, pCurrentFriendList);

			PUMP_CS_AND_EXIT_IF_FAILED  // Pump the connection server in the background

			// Check the specific friend list and verify that all entries are correct
			switch(dwUserIndex)
			{
			case 0:
#if SY_BLOCKING_IMPLEMENTED				
				dwNumOfExpectedFriend = 1;
				
				pFriendListExpected = (XONLINE_FRIEND *)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );
				memset(pFriendListExpected , 0, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );

				pFriendListExpected[0].xuid.qwUserID = g_LoggedOnUsers[1].xuid.qwUserID;
				strcpy(pFriendListExpected[0].username, g_LoggedOnUsers[1].name);
				pFriendListExpected[0].friendState = XONLINE_FRIENDSTATE_FLAG_ONLINE | 
							XONLINE_FRIENDSTATE_FLAG_PLAYING | 
							XONLINE_FRIENDSTATE_FLAG_VOICE | 
							XONLINE_FRIENDSTATE_FLAG_JOINABLE;
				*((ULONGLONG *) &(pFriendListExpected[0].sessionID)) = 0xBBBBBBBBBBBBBBBB;
				pFriendListExpected[0].titleID = HARNESS_TITLE_ID;
				pFriendListExpected[0].StateDataSize = 1;
				memcpy(pFriendListExpected[0].StateData,byStateData2,pFriendListExpected[0].StateDataSize);
#else					
				dwNumOfExpectedFriend = 2;
				
				pFriendListExpected = (XONLINE_FRIEND *)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );
				memset(pFriendListExpected , 0, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );

				pFriendListExpected[0].xuid.qwUserID = g_LoggedOnUsers[1].xuid.qwUserID;
				strcpy(pFriendListExpected[0].username, g_LoggedOnUsers[1].name);
				pFriendListExpected[0].friendState = XONLINE_FRIENDSTATE_FLAG_ONLINE | 
							XONLINE_FRIENDSTATE_FLAG_PLAYING | 
							XONLINE_FRIENDSTATE_FLAG_VOICE | 
							XONLINE_FRIENDSTATE_FLAG_JOINABLE;
				*((ULONGLONG *) &(pFriendListExpected[0].sessionID)) = 0xBBBBBBBBBBBBBBBB;
				pFriendListExpected[0].titleID = HARNESS_TITLE_ID;
				pFriendListExpected[0].StateDataSize = 1;
				memcpy(pFriendListExpected[0].StateData,byStateData2,pFriendListExpected[0].StateDataSize);

				pFriendListExpected[1].xuid.qwUserID = g_LoggedOnUsers[3].xuid.qwUserID;
				strcpy(pFriendListExpected[1].username, g_LoggedOnUsers[3].name);
				pFriendListExpected[1].friendState = XONLINE_FRIENDSTATE_FLAG_PENDING;
				pFriendListExpected[1].sessionID.ab[0] = 0;
				pFriendListExpected[1].titleID = 0;
				pFriendListExpected[1].StateDataSize = 0;
#endif				
				
				bExpectedResult = IsValidResultForFriendList(hLog, dwNumOfExpectedFriend, dwNumFriends, pFriendListExpected, pCurrentFriendList);

				LocalFree(pFriendListExpected);
				break;

			case 1:
				dwNumOfExpectedFriend = 1;
				
				pFriendListExpected = (XONLINE_FRIEND *)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );
				memset(pFriendListExpected , 0, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );

				pFriendListExpected[0].xuid.qwUserID = g_LoggedOnUsers[0].xuid.qwUserID;
				strcpy(pFriendListExpected[0].username, g_LoggedOnUsers[0].name);
				pFriendListExpected[0].friendState = XONLINE_FRIENDSTATE_FLAG_ONLINE | 
					XONLINE_FRIENDSTATE_FLAG_PLAYING | 
					XONLINE_FRIENDSTATE_FLAG_VOICE | 
					XONLINE_FRIENDSTATE_FLAG_JOINABLE;
				*((ULONGLONG *) &(pFriendListExpected[0].sessionID)) = 0xAAAAAAAAAAAAAAAA;
				pFriendListExpected[0].titleID = HARNESS_TITLE_ID;
				pFriendListExpected[0].StateDataSize = MAX_STATEDATA_SIZE;
				memcpy(pFriendListExpected[0].StateData,byStateData1,MAX_STATEDATA_SIZE);
					
				bExpectedResult = IsValidResultForFriendList(hLog, dwNumOfExpectedFriend, dwNumFriends, pFriendListExpected, pCurrentFriendList);

				LocalFree(pFriendListExpected);
				break;

			case 2:
				if(dwNumFriends != 0)
				{
					FAIL_TESTCASE("Unexpected number of friends in list")
					bExpectedResult = FALSE;
				}
				break;
			case 3:
#if SY_BLOCKING_IMPLEMENTED				
				if(dwNumFriends != 0)
				{
					FAIL_TESTCASE("Unexpected number of friends in list")
					bExpectedResult = FALSE;
				}
#else
				dwNumOfExpectedFriend = 1;
				
				pFriendListExpected = (XONLINE_FRIEND *)LocalAlloc(LPTR, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );
				memset(pFriendListExpected , 0, sizeof(XONLINE_FRIEND) * dwNumOfExpectedFriend );

				pFriendListExpected[0].xuid.qwUserID = g_LoggedOnUsers[0].xuid.qwUserID;
				strcpy(pFriendListExpected[0].username, g_LoggedOnUsers[0].name);
				pFriendListExpected[0].friendState = XONLINE_FRIENDSTATE_FLAG_REQUEST;
				*((ULONGLONG *) &(pFriendListExpected[0].sessionID)) = 0x0;
				pFriendListExpected[0].titleID = 0;
				pFriendListExpected[0].StateDataSize = 0;
				
				bExpectedResult = IsValidResultForFriendList(hLog, dwNumOfExpectedFriend, dwNumFriends, pFriendListExpected, pCurrentFriendList);

				LocalFree(pFriendListExpected);
#endif				
				break;

			default:
				FAIL_TESTCASE("Unexpected user index");
				bExpectedResult = FALSE;
				break;
			}
		}
	}

	// TODO - check for list updates

	if(bExpectedResult)
	{
		PASS_TESTCASE("Retrieved friend list for all users");
	}
	else
	{
		FAIL_TESTCASE("Retrieving friend list for all users failed");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");


//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsRemove");
	START_TESTCASE("Have user 1 remove users 2, 3 and 4");
//===================================================================

	pCurrentFriendList = (PXONLINE_FRIEND) pFriendBuffers;

	bExpectedResult = TRUE;
//	for(dwCounter1 = 0; bExpectedResult && dwCounter1 < (XONLINE_MAX_LOGON_USERS - 1); ++dwCounter1)
	for(dwCounter1 = 0; bExpectedResult && dwCounter1 < 2; ++dwCounter1)
	{
		xLog(hLog, XLL_INFO, "User 1 is removing friend %u", dwCounter1 + 1);

		if(!pCurrentFriendList[dwCounter1].xuid.qwUserID)
		{
			xLog(hLog, XLL_INFO, "Expected friend %u doesn't exist", dwCounter1 + 1);
			continue;
		}
		
		hr = XOnlineFriendsRemove(0, pCurrentFriendList + dwCounter1);
		if(hr != S_OK)
		{
			FAIL_TESTCASE("XOnlineFriendsRemove failed unexpectedly");
			bExpectedResult = FALSE;
		}

		PUMP_CS_AND_EXIT_IF_FAILED		
	}

	if(bExpectedResult)
	{
		PASS_TESTCASE("Retrieved friend lists for all users");
	}
	else
	{
		FAIL_TESTCASE("Retrieving friend lists for all users");
	}
	
	END_TESTCASE
	xSetFunctionName(hLog, "");

//===================================================================
	xSetFunctionName(hLog, "XOnlineFriendsGetLatest");
	START_TESTCASE("Get friend lists for each user #6");
//===================================================================

	bExpectedResult = FALSE;
	dwStartTickCount = GetTickCount();

	while(!bExpectedResult && !IsTimeFinished(dwStartTickCount, ALLOWEDPUMPINGTIME))
	{
		bExpectedResult = TRUE;

		nResult = PumpFriendTask(hLog, hLogonTask, hGameOpTask, hEnumGamesTask, hLockoutTask, 6, &hReturn);
		if(nResult == SY_STOPWHOLETEST)	// Pump LogonTask failed
		{
			xEndVariation(hLog);
			goto Exit;
		}
		else if(nResult == SY_STOPTESTCASE)
		{
			bExpectedResult = FALSE;
			break;
		}
		for(dwUserIndex = 0; bExpectedResult && dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
		{
			xLog(hLog, XLL_INFO, "Getting friend list for user %u", dwUserIndex + 1);

			pCurrentFriendList = (PXONLINE_FRIEND) (pFriendBuffers + (dwUserIndex * sizeof(XONLINE_FRIEND) * MAX_FRIENDS));

			memset(pCurrentFriendList, 0, sizeof(XONLINE_FRIEND) * MAX_FRIENDS);

			dwNumFriends = XOnlineFriendsGetLatest(dwUserIndex, MAX_FRIENDS, pCurrentFriendList);

			DisplayFriendList(hLog, dwNumFriends, pCurrentFriendList);

			PUMP_CS_AND_EXIT_IF_FAILED

			// Check the specific friend list and verify that all entries are correct
			switch(dwUserIndex)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				if(dwNumFriends != 0)
				{
					bExpectedResult = FALSE;
				}
					
				break;
			default:
				FAIL_TESTCASE("Unexpected user index");
				bExpectedResult = FALSE;
				break;
			}

		}
	}

	// TODO - check for list updates

	if(bExpectedResult)
	{
		PASS_TESTCASE("Retrieved friend list for all users");
	}
	else
	{
		FAIL_TESTCASE("Unexpected number of friends in list");
	}
		
	
	END_TESTCASE
	xSetFunctionName(hLog, "");


Exit:

	for(dwUserIndex = 0; dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
	{
		xLog(hLog, XLL_INFO, "Closing friend enum handle for user %u", dwUserIndex);
		CLEANUP_TASK(hEnumGamesTask[dwUserIndex]);
	}

	CLEANUP_TASK(hGameOpTask);
	CLEANUP_TASK(hLockoutTask);
	CLEANUP_TASK(hLogonTask);

	if(hLogonEvent)
		CloseHandle(hLogonEvent);

	if(hFriendOpEvent)
		CloseHandle(hFriendOpEvent);

    hr = XOnlineCleanup();
    Verify(hr == S_OK);	

    return;
}

//==================================================================================
// IsTimeFinished
//----------------------------------------------------------------------------------
//
// Description: 
// Check if allowed time is expired
//
// Arguments:
//	DWORD	dwStart, DWORD dwAllowed
// Returns:
//	TRUE if expired.
//==================================================================================

BOOL	IsTimeFinished(DWORD	dwStart, DWORD dwAllowed)
{
	DWORD dwCurrent = GetTickCount(), dwDuration;

	if(dwCurrent < dwStart)
		dwDuration = (MAXDWORD - dwStart) + dwCurrent;
	else
		dwDuration = dwCurrent - dwStart;

	if(dwDuration > dwAllowed)
		return TRUE;
	else
		return FALSE;
}
		

//==================================================================================
// PumpFriendTask
//----------------------------------------------------------------------------------
//
// Description: Pump Friend Task periodically
//
// Arguments:
//	XONLINETASK_HANDLE hLogonTask, XONLINETASK_HANDLE hGameOpTask, XONLINETASK_HANDLE hEnumGamesTask[], UINT count
// Returns:
//	S_OK if test is finished(regardless of test result). E_FAIL if it got any whole test blocking problem.
//==================================================================================

SY_RESULT PumpFriendTask(HANDLE  hLog, XONLINETASK_HANDLE hLogonTask, XONLINETASK_HANDLE hGameOpTask, 
							XONLINETASK_HANDLE hEnumGamesTask[], XONLINETASK_HANDLE hLockoutTask, UINT count,
							HRESULT* phrReturn)
{
    HRESULT hReturn = S_OK, hrContinue;         // if this is E_FAIL, give up all test cases
    BOOL    bTestResult=TRUE; 	// if this is FALSE, give up only this case case
    DWORD 	dwCurrentTime;
    TCHAR 	szMsg[128];
    
    // Pump friend enumeration handles for each user
    for(UINT ii = 0; bTestResult && ii < FRIEND_HANDLE_PUMP_COUNT; ++ii)
    {	
    	if(hGameOpTask)
    	{
	        HRESULT hrContinue = XOnlineTaskContinue(hGameOpTask);
	        	
	        if(XONLINETASK_S_RUNNING != hrContinue)
	        {
	            xLog(hLog, XLL_FAIL, "Pumping hGameOpTask returned %0x and failed unexpectedly", hrContinue); 
	            
	            bTestResult = FALSE;
	            *phrReturn = hrContinue;
	            return SY_STOPTESTCASE;
	        }
    	}
        
        for(DWORD dwUserIndex = 0; bTestResult && dwUserIndex < XONLINE_MAX_LOGON_USERS; ++dwUserIndex)
        {
            hrContinue = XOnlineTaskContinue(hEnumGamesTask[dwUserIndex]);
            if(XONLINETASK_S_RUNNING != hrContinue && XONLINE_S_NOTIFICATION_UPTODATE != hrContinue)
            {
                xLog(hLog, XLL_FAIL, "Pumping hEnumGamesTask returned %0x and failed unexpectedly for user %d",hrContinue, dwUserIndex);
                bTestResult = FALSE;
	            *phrReturn = hrContinue;
	            return SY_STOPTESTCASE;
            }
        }

    	if(hLockoutTask)
    	{
	        HRESULT hrContinue = XOnlineTaskContinue(hLockoutTask);
	        	
	        if(XONLINETASK_S_RUNNING != hrContinue)
	        {
	            xLog(hLog, XLL_FAIL, "Pumping hLockoutTask returned %0x and failed unexpectedly", hrContinue); 
	            bTestResult = FALSE;
	            *phrReturn = hrContinue;
	            return SY_STOPTESTCASE;
	        }
    	}
        
	    if(SY_STOPWHOLETEST == PumpLogonTask(hLog, hLogonTask, phrReturn))
	    {
            return SY_STOPWHOLETEST;
		}
			
        Sleep(SLEEPTIME);
    }

    if(SY_STOPWHOLETEST == PumpLogonTask(hLog, hLogonTask, phrReturn))
    {
        *phrReturn = hReturn;
        return SY_STOPWHOLETEST;
	}
    
    return SY_SUCCESS;	
}

//==================================================================================
// PumpLogonTask
//----------------------------------------------------------------------------------
//
// Description: Pump Logon Task periodically
//
// Arguments:
//	HANDLE  hLog, XONLINETASK_HANDLE hLogonTask
// Returns:
//	E_FAIL if pumping fails and it will block whole test case after now.
//==================================================================================

SY_RESULT PumpLogonTask(HANDLE  hLog, XONLINETASK_HANDLE hLogonTask, HRESULT* phrReturn)
{
	*phrReturn = XOnlineTaskContinue(hLogonTask); 

	if(FAILED(*phrReturn)) 
	{
	    xLog(hLog, XLL_FAIL, "Pumping Logon Task returned %0x and Lost connection with CS, exiting test!",*phrReturn);
        return SY_STOPWHOLETEST;
	}

	return SY_SUCCESS;
}

//==================================================================================
// IsLockoutUser
//----------------------------------------------------------------------------------
//
// Description: Check if specified user is locked out
//
// Arguments:
//	DWORD dwNumLockoutUsers, PXONLINE_LOCKOUTUSER  pLockoutlist, XUID xuid
// Returns:
//	TURE if the specified user is locked out.
//==================================================================================

BOOL IsLockoutUser(DWORD dwNumLockoutUsers, PXONLINE_LOCKOUTUSER  pLockoutlist, XUID xuid)
{
	for(DWORD ii=0; ii<dwNumLockoutUsers; ii++)
	{
		if(pLockoutlist[ii].xuid.qwUserID == xuid.qwUserID)
			return TRUE;
	}

	return FALSE;
	
}
//==================================================================================
// IsValidResultForFriendList
//----------------------------------------------------------------------------------
//
// Description: Check if it is valid result for Friend List
//
// Arguments:
//	HANDLE  hLog, DWORD dwNumOfExpectedFriend, DWORD dwNumOfReturnedFriend, 
//	XONLINE_FRIEND *pFriendListExpected, XONLINE_FRIEND *pFriendListReturned
// Returns:
//	TURE if valid result
//==================================================================================

BOOL	IsValidResultForFriendList(HANDLE  hLog, DWORD dwNumOfExpectedFriend, DWORD dwNumOfReturnedFriend, 
										XONLINE_FRIEND *pFriendListExpected, XONLINE_FRIEND *pFriendListReturned)
{
	BOOL	bExpectedResult;
	
	if(dwNumOfExpectedFriend != dwNumOfReturnedFriend){
		return FALSE;
	}

	for(UINT ii = 0; ii < dwNumOfReturnedFriend; ++ii)
	{
		for(UINT jj = 0; jj < dwNumOfExpectedFriend; ++jj)
		{
			if(pFriendListReturned[ii].xuid.qwUserID == pFriendListExpected[jj].xuid.qwUserID)
			{
				if(strncmp(pFriendListReturned[ii].username, pFriendListExpected[jj].username, strlen(pFriendListExpected[jj].username)))
					return FALSE;
				else
					break;
			}
		}

		if(jj == dwNumOfExpectedFriend)
		{
			bExpectedResult = FALSE;
			return FALSE;
		}
			
		if(pFriendListReturned[ii].friendState != pFriendListExpected[jj].friendState)
		{
			bExpectedResult = FALSE;
			return FALSE;
		}

		if(*((ULONGLONG *) &(pFriendListReturned[ii].sessionID)) != *((ULONGLONG *) &(pFriendListExpected[jj].sessionID)) ||
			pFriendListReturned[ii].titleID != pFriendListExpected[jj].titleID ||
			pFriendListReturned[ii].StateDataSize != pFriendListExpected[jj].StateDataSize )
		{
			bExpectedResult = FALSE;
			return FALSE;
		}

		if( pFriendListExpected[ii].StateDataSize != 0 )
			if( memcmp(pFriendListReturned[ii].StateData, pFriendListExpected[jj].StateData, pFriendListExpected[jj].StateDataSize))
			{
				bExpectedResult = FALSE;
				return FALSE;
			}
				
	}

	return TRUE;
}


#if SY_NEED_CREATE_USERS

//==================================================================================
// SY_CreateUserAccounts
//----------------------------------------------------------------------------------
//
// Description: 
//
// Arguments:
//	DWORD dwNumAccounts				the number of accounts to create
//											
//
// Returns: 
//==================================================================================

HRESULT 
SY_CreateUserAccounts(DWORD dwNumAccounts )
{
	HRESULT		hr = S_OK, hrTask;
	HANDLE hEvent = NULL;
	XONLINETASK_HANDLE hTask = NULL;

	XONLINEP_USER pUsers[XONLINE_MAX_LOGON_USERS];

	__try
	{
		hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

		ASSERT(hEvent);
		if ( NULL == hEvent )
		{
			hr = E_OUTOFMEMORY;
			RaiseException(0xe0000001, 0,0, NULL);
		}
		
//		pUsers = (PXONLINEP_USER) LocalAlloc(LPTR, sizeof(PXONLINEP_USER) * dwNumAccounts);

		strcpy(pUsers[0].name,"Steve650122");
		strcpy(pUsers[1].name,"Kay660125");
		strcpy(pUsers[2].name,"Chanyoung940327");
		strcpy(pUsers[3].name,"Chris990607");

		for( DWORD ii=0; ii<dwNumAccounts; ii++ )
		{
			strcpy(pUsers[ii].kingdom,"Earth");

			hr = _XOnlineAccountTempCreate( (PXONLINE_USER) &pUsers[ii], hEvent, &hTask );

			ASSERT( SUCCEEDED( hr ) );
			if(FAILED(hr))
			{
				hr = E_FAIL;
				RaiseException(0xe0000001, 0,0, NULL);
			}


			do
			{   
				//
				// wait for data to be ready for processing
				//

				DWORD dwWait = WaitForSingleObject( hEvent, INFINITE );

				ASSERT(WAIT_FAILED != dwWait);
				if ( WAIT_FAILED == dwWait )
				{
					hr = E_OUTOFMEMORY;
					RaiseException(0xe0000001, 0,0, NULL);
				}
    
				//
				// continue pumping the handle
				//

				hrTask = XOnlineTaskContinue( hTask);

			} while ( hrTask == XONLINETASK_S_RUNNING );

			ASSERT( SUCCEEDED( hrTask ) );
			if(FAILED(hrTask))
			{
				hr = E_FAIL;
				RaiseException(0xe0000001, 0,0, NULL);
			}
			
			XOnlineTaskClose( hTask );

		}

	}
	__except((GetExceptionCode() == 0xe0000001)?
				EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH )
	{
	}

	return hr;
}
#endif

#if 0
//==================================================================================
// PollTaskUntilGetResult
//----------------------------------------------------------------------------------
//
// Description: Calls the XOnlineTaskComplete function until either the given
//    task has been completed or the amount of available time has expired
//
// Arguments:
//	XONLINETASK_HANDLE	hTask				Handle to async task
//	HANDLE				hEvent		Event that is associated with this async task.
//											This can be set to NULL or INVALID_HANDLE_VALUE
//											if no event is associated with the task
//	DWORD				dwMaxWaitTime		Maximum amount of time to wait for the task to
//											complete. This can be set to INFINITE to block
//											until completion
//	DWORD				*pdwPollCounter		Optional counter passed in to keep track of the
//											number of calls made to the XOnlineTaskContinue function
//	BOOL				*pfCompleted		Optional boolean passed in to keep track if whether
//											the task completed or not
//  BOOL				fCloseOnComplete	Indicates whether the function should close a handle
//											after it indicates completion
//
// Returns: The last result code from XOnlineTaskContinue is returned.  If a valid pointer
//   is provided for pfCompleted, it is updated to reflect whether the task has completed or not
//==================================================================================
HRESULT PollTaskUntilGetResult(XONLINETASK_HANDLE hTask, HANDLE hEvent, DWORD dwMaxWaitTime, DWORD *pdwPollCounter, BOOL *pfCompleted, BOOL fCloseOnComplete)
{
	HRESULT hr = S_OK;
	DWORD dwLastTime = 0, dwCurrentTime = 0, dwCurrentDuration = 0, dwWaitTimeLeft = dwMaxWaitTime;
	DWORD dwInternalPollCounter = 0;
	HRESULT hrContinue;

	if(pfCompleted)
		*pfCompleted = FALSE;

	dwLastTime = GetTickCount();

	do
	{
		if((hEvent != INVALID_HANDLE_VALUE) && (hEvent != NULL))
			WaitForSingleObject(hEvent, dwWaitTimeLeft);

		hrContinue = XOnlineTaskContinue(hTask);

		// Update the poll-counter
		++dwInternalPollCounter;

		if (XONLINETASK_S_RUNNING != hrContinue)	//if hrDoWorks is not XONLINETASK_S_RUNNING(0x0)
		{	
		 	if (XONLINETASK_S_SUCCESS == hrContinue)	//if hrDoWorks is XONLINETASK_S_SUCCESS(0x1)
			{
				if(pfCompleted)
				{
					*pfCompleted = TRUE;
				}
			}
		  	else if (FAILED(hTask))
		    	{
		        	hr = hrContinue;
		    	}

			if(fCloseOnComplete)
			{
				XOnlineTaskClose(hTask);
			}	
		    	
		    break;
		}		

		if(dwMaxWaitTime != INFINITE)
		{
			// See if we have gone beyond the maximum allowed time
			dwCurrentTime = GetTickCount();
			if(dwCurrentTime < dwLastTime)
			{
				dwCurrentDuration = ((MAXDWORD - dwLastTime) + dwCurrentTime);
			}
			else
			{
				dwCurrentDuration = (dwCurrentTime - dwLastTime);
			}
			
			if(dwCurrentDuration >= dwWaitTimeLeft)
			{
				DbgPrint("Async operation didn't complete within %u ms\n", dwMaxWaitTime);
				hr = E_FAIL;
				goto Exit;
			}
			
			dwWaitTimeLeft -= dwCurrentDuration;
			dwLastTime = dwCurrentTime;
		}
	} while(TRUE);

Exit:
	if(pdwPollCounter)
		*pdwPollCounter = dwInternalPollCounter;

	return hr;
}

#endif
//==================================================================================
// XBudBVTEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI XBudBVTEndTest()
{

}

} // namespace XBudTestNamespace

// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XBudBVT )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( XBudBVT )
    EXPORT_TABLE_ENTRY( "StartTest", XBudBVTStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XBudBVTEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", XBudBVTDllMain )
END_EXPORT_TABLE( XBudBVT )

