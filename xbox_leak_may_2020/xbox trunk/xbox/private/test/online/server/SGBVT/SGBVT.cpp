/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       SGBVT.cpp
 *  Content:    SGBVT tests
 *  History:
 *   Date		By		Reason
 *   ====	==		======
 *  2/18/02    johnblac	Ripped from Tristan's onlinestress tool
 *
 ****************************************************************************/

#include "SGBVT.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

HANDLE g_hLog = NULL;
CLogonTask *g_pLogonTask = NULL;
BOOL g_fExitTest = FALSE;

/****************************************************************************
 *
 *  TRACE
 *
 ****************************************************************************/
void WINAPI TRACE(LPCWSTR szFormat, ...) {
	WCHAR szBuffer[1024] = L"";
	va_list pArgs; 
	va_start(pArgs, szFormat);
	
	wvsprintf(szBuffer, szFormat, pArgs);
	
	va_end(pArgs);

	OutputDebugString(szBuffer);
}

/****************************************************************************
 *
 *  SendPacket
 *
 *  Description:
 *      Sends data to the socket until the request completes, or the specified
 *      timer has expired
 *
 *  Arguments:
 *      SOCKET                 Socket              Socket from which to send the packet
 *      CHAR                   *pBuff              Buffer to send the packet from
 *      INT                    nSizeToSend         Ammount of data to send
 *      COnlineTimer           *pTimer             If this object expires, then the send will fail
 *                                                 If NULL, return immediately if send fails
 *
 *  Returns:  
 *      S_OK on success
 *      S_FALSE if the socket timed out
 *      E_FAIL on system failure
 *
 ****************************************************************************/
HRESULT SendPacket(SOCKET Socket, CHAR *pBuff, INT nSizeToSend, COnlineTimer *pTimer, sockaddr_in* pDestAddr)
{
	HRESULT hr = S_OK;
	INT nSentSize = 0, nRet = 0;

	if(pTimer)
		pTimer->Start();

	do
	{
		nRet = sendto(Socket, pBuff, nSizeToSend, 0, (sockaddr*)pDestAddr, sizeof(sockaddr_in));
		// If a socket error other than WOULDBLOCK occurs, then bail
		if(nRet == SOCKET_ERROR)
		{
			// Keep trying until timeout for a WOULDBLOCK error
			if(WSAGetLastError() == WSAEWOULDBLOCK)
			{
				Sleep(0);
				hr = S_FALSE;
				continue;
			}

			LOGTASKINFO(g_hLog, "Unexpected socket error received from client");
			return E_FAIL;
		}
		
		// If we read the client size, then continue to the next stage
		if(nRet == nSizeToSend)
		{
			hr = S_OK;
			break;
		}
	}
	while(pTimer && (!pTimer->HasTimeExpired()));

	return hr;
}

/****************************************************************************
 *
 *  ReadPacket
 *
 *  Description:
 *      Reads data from the socket until either the amount of data requested
 *      has been received, or the specified timer has expired
 *
 *  Arguments:
 *      SOCKET                 Socket              Socket from which to read the packet
 *      CHAR                   *pBuff              Buffer in which to put the packet
 *      INT                    nSizeToRead         Ammount of data to read
 *      COnlineTimer           *pTimer             If this object expires, then the read will fail
 *                                                 If NULL, return immediately if nothing to read
 *
 *  Returns:  
 *      S_OK all data was 
 *      S_FALSE if the socket timed out
 *      E_FAIL if the there was a system error
 *
 ****************************************************************************/
HRESULT ReadPacket(SOCKET Socket, CHAR *pBuff, INT nSizeOfBuffer, COnlineTimer *pTimer, sockaddr_in* pSrcAddr, int* piBuffSize)
{
	HRESULT hr = S_OK;
	int iSizeOfSock = sizeof(sockaddr_in);

	if(pTimer)
		pTimer->Start();

	do
	{
		*piBuffSize = recvfrom(Socket, pBuff, nSizeOfBuffer, 0, (sockaddr*)pSrcAddr, &iSizeOfSock);
		// If a socket error other than WOULDBLOCK occurs, then bail
		if(*piBuffSize == SOCKET_ERROR)
		{
			// Keep trying until timeout for a WOULDBLOCK error
			if(WSAGetLastError() == WSAEWOULDBLOCK)
			{
				Sleep(0);
				hr = S_FALSE;
				continue;
			}

			LOGTASKINFO(g_hLog, "Unexpected socket error received from client");
			return E_FAIL;
		}
		// If the socket was closed on the other end, then bail
		else if(*piBuffSize == 0)
		{
			LOGTASKINFO(g_hLog, "Client socket closed unexpectedly");
			return E_FAIL;
		}
		else if(*piBuffSize > nSizeOfBuffer)
		{
			// We overflowed, all is lost
			hr = E_OVERFLOW;
			break;
		}
		else
		{
			// We got a valid packet, we're done
			break;
		}
	}
	while(pTimer && (!pTimer->HasTimeExpired()));

	return hr;
}


/****************************************************************************
 *
 *  CreateListenSocket
 *
 *  Description:
 *      Create a host socket and begin listening for incoming connections
 *
 *  Arguments:
 *      CListenSocket         *pListenSocket      The socket that will be created to handle incoming
 *                                                connection requests
 *      XNKID                 pSessionID          XNKID to register with the network stack
 *      XNKEY                 pKeyExchangeKey     XNKEY to register with the network stack
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT CreateListenSocket(CListenSocket **ppListenSocket, XNKID *pSessionID, XNKEY *pKeyExchangeKey)
{
	HRESULT hr = S_OK;

	if(!ppListenSocket || !pSessionID || !pKeyExchangeKey)
		return E_FAIL;

	*ppListenSocket = new CListenSocket();

	hr = (*ppListenSocket)->StartListening(SGBVT_HOST_PORT);
	if(hr != S_OK)
		goto Exit;

Exit:

	if(hr != S_OK)
	{
		delete *ppListenSocket;
		*ppListenSocket = NULL;
	}

	return hr;
}

/****************************************************************************
 *
 *  SearchForStressSession
 *
 *  Description:
 *      Searches for active stress sessions with available slots
 *
 *  Arguments:
 *      XNADDR                *pHostXnAddr       XNADDR of the host session that was found
 *      XNKID                 *pSessionID         XNKID of the host session that was found
 *      XNKEY                 *pKeyExchangeKey    XNKEY of the host session that was found
 *
 *  Returns:  
 *      S_OK if a stress session with extra space was found
 *      S_FALSE if a stress session wasn't found
 *      E_FAIL if there was some kind of system failure
 *
 ****************************************************************************/
HRESULT SearchForStressSession(XNADDR *pHostXnAddr, XNKID *pSessionID, XNKEY *pKeyExchangeKey)
{
	XONLINETASK_HANDLE hSearchTask = NULL;
	PXMATCH_SEARCHRESULT *ppSearchResults = NULL;
	COnlineTimer SearchTimer;
	HRESULT hr = S_OK;
	HANDLE hWorkEvent = NULL;
	DWORD dwResultsCount = 0, dwCurrentResult = 0;

	if(!pHostXnAddr || !pSessionID || !pKeyExchangeKey)
	{
		LOGTASKINFO(g_hLog, "Invalid parameters");
		hr = E_FAIL;
		goto Exit;
	}

	// Create an event to be associated with the match search task
	hWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hWorkEvent)
	{
		LOGTASKINFO(g_hLog, "Couldn't create event for match search");
		hr = E_FAIL;
		goto Exit;
	}

	// Start the search for an available stress session
	hr = XOnlineMatchSearch(SGBVT_SEARCH_PROC, SGBVT_MAX_RESULTS, 0, NULL, SGBVT_MAX_RESULTS_LEN, hWorkEvent, &hSearchTask);
	if(FAILED(hr))
	{
		LOGTASKINFOHR(g_hLog, "Error starting matchmaking search", hr);
		hr = E_FAIL;
		goto Exit;
	}


	SearchTimer.SetAllowedTime(SGBVT_MAX_MATCH_SEARCH_TIME);
	SearchTimer.Start();

	// Pump the search task until completion
	do
	{
		if(WaitForSingleObject(hWorkEvent, SearchTimer.GetTimeRemaining()) != WAIT_OBJECT_0)
			continue;

		hr = XOnlineTaskContinue(hSearchTask);
		if(XONLINETASK_STATUS_AVAILABLE(hr))
			break;

	} while(!SearchTimer.HasTimeExpired());

	if(SearchTimer.HasTimeExpired())
	{
		LOGTASKINFO(g_hLog, "Timed out searching for host session");
		hr = S_FALSE;
		goto Exit;
	}

	// Get the results of the search for host sessions
	hr = XOnlineMatchSearchGetResults(hSearchTask, &ppSearchResults, &dwResultsCount);
	if(FAILED(hr))
	{
		LOGTASKINFOHR(g_hLog, "Error getting matchmaking search results", hr);
		hr = E_FAIL;
		goto Exit;
	}

	// If no results were found, then exit and indicate this
	if(!dwResultsCount)
	{
		LOGTASKINFO(g_hLog, "No host sessions found");
		hr = S_FALSE;
		goto Exit;
	}

	// Parse through the results and find the first one that has available slots
	for(dwCurrentResult = 0; dwCurrentResult < dwResultsCount; ++dwCurrentResult)
	{
		if((ppSearchResults[dwCurrentResult])->dwPublicAvailable > (ppSearchResults[dwCurrentResult])->dwPublicCurrent)
		{
			LOGTASKINFO(g_hLog, "Found a host session with available slots");
			break;
		}
	}

	// If we looped through the entire search results list and didn't find a session with available slots
	// then indicate this
	if(dwCurrentResult == dwResultsCount)
	{
		LOGTASKINFO(g_hLog, "Host sessions found, but none had available slots");
		hr = S_FALSE;
		goto Exit;
	}

	
	// Return all of the info needed to connect to this session
	memcpy(pHostXnAddr, &((ppSearchResults[dwCurrentResult])->HostAddress), sizeof(XNADDR));
	memcpy(pSessionID, &((ppSearchResults[dwCurrentResult])->SessionID), sizeof(XNKID));
	memcpy(pKeyExchangeKey, &((ppSearchResults[dwCurrentResult])->KeyExchangeKey), sizeof(XNKEY));

Exit:

	if(hSearchTask)
	{
		XOnlineTaskClose(hSearchTask);
		hSearchTask = NULL;
	}

	return hr;
}


/****************************************************************************
 *
 *  SearchForStressSession
 *
 *  Description:
 *      Searches for active stress sessions with available slots
 *
 *  Arguments:
 *      XNKID                 *pSessionID         XNKID of the host session to delete
 *
 *  Returns:  
 *      S_OK if a stress session specified was successfully deleted
 *      S_FALSE if we timed out before the session delete could complete
 *      E_FAIL if there was some kind of system failure
 *
 ****************************************************************************/
HRESULT DeleteStressSession(XNKID *pSessionID)
{
	XONLINETASK_HANDLE hDeleteTask = NULL;
	COnlineTimer DeleteTimer;
	HRESULT hr = S_OK;
	HANDLE hWorkEvent = NULL;

	if(!pSessionID)
	{
		LOGTASKINFO(g_hLog, "Invalid parameters");
		hr = E_FAIL;
		goto Exit;
	}

	// Create an event to be associated with the session delete task
	hWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hWorkEvent)
	{
		LOGTASKINFO(g_hLog, "Couldn't create event for session delete");
		hr = E_FAIL;
		goto Exit;
	}

	// Start the delete task for the host session
	hr = XOnlineMatchSessionDelete(*pSessionID, hWorkEvent, &hDeleteTask);
	if(FAILED(hr))
	{
		LOGTASKINFOHR(g_hLog, "Error starting session delete", hr);
		hr = E_FAIL;
		goto Exit;
	}


	DeleteTimer.SetAllowedTime(SGBVT_MAX_SESSION_DELETE_TIME);
	DeleteTimer.Start();

	// Pump the delete task until completion
	do
	{
		if(WaitForSingleObject(hDeleteTask, DeleteTimer.GetTimeRemaining()) != WAIT_OBJECT_0)
			continue;

		hr = XOnlineTaskContinue(hDeleteTask);
		if(XONLINETASK_STATUS_AVAILABLE(hr))
			break;

	} while(!DeleteTimer.HasTimeExpired());

	if(DeleteTimer.HasTimeExpired())
	{
		LOGTASKINFO(g_hLog, "Timed out trying to delete session");
		hr = S_FALSE;
		goto Exit;
	}

Exit:

	if(hDeleteTask)
	{
		XOnlineTaskClose(hDeleteTask);
		hDeleteTask = NULL;
	}

	return hr;
}

/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      The stress test can be compiled in three ways:
 *         1) As a standalone Xbox XBE not running under the harness (i.e. _XBOX and NOLOGGING are defined)
 *         2) As an Xbox harness lib (i.e. _XBOX is defined and NOLOGGING is NOT defined)
 *         3) As a standalone PC app using CXOnline (i.e. _XBOX and NOLOGGING are NOT defined)
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#if defined(_XBOX) && defined(NOLOGGING)
int __cdecl main(int argc, char * argv[])
#else
#if !defined(_XBOX)
#define NOLOGGING
class CStressXOnline : public CXOnline
{
public:
    CStressXOnline(char * pszXbox = NULL) : CXOnline(pszXbox) {}
    int Main(int argc, char * argv[]);
	// BUGBUG - need to add entries for every function 
};

int __cdecl main(int argc, char * argv[])
{
    int result;
    char buffer[128];

    sprintf(buffer, "%s@Nic/0", getenv("COMPUTERNAME") );
    CStressXOnline * pTestXOnline = new CStressXOnline(buffer);
    result = pTestXOnline->Main(argc, argv);
    delete pTestXOnline;
    return(result);
}

int CStressXOnline::Main(int argc, char * argv[])
#endif
#endif

#ifdef NOLOGGING
{
    SGBVT_StartTest( NULL );
    SGBVT_EndTest();
}
#endif // NOLOGGING


/****************************************************************************
 *
 *  SGBVT_Logon
 *
 *  Description:
 *	Calls XOnlineLogon and pumps.  This code is reused freqnetly so it has been seperated
 *	into a sub function for ease of use.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT SGBVT_Logon(CLogonTask* pLogonTask, COnlineTimer* pTimer)
{
	// Start logon async task
	if(!pLogonTask->StartTask())
	{
		return pLogonTask->GetLastResult();
	}

	pTimer->SetAllowedTime(MAX_LOGON_TIME);
	pTimer->Start();

	// Pump task until complete
	while(!pLogonTask->IsConnectionEstablished())
	{
		if(pTimer->HasTimeExpired())
			break;

		// Make sure to wait for the work event to test that it is always signalled when necessary
		if(!pLogonTask->WaitForWork(pTimer->GetTimeRemaining()))
			break;

		pLogonTask->TaskContinue();
	}

	return S_OK;
}


/****************************************************************************
 *
 *  sgbvt_BasicTest
 *
 *  Description:
 *      Basic Test for sgbvt
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT SGBVT( void )
{
	HRESULT	hr = S_OK;
	CLogonTask LogonTask;
	CFriendEnumTask			FriendEnumTask;
	COfferingEnumerateTask	OfferingEnumerateTask;
	CContentInstallTask		*pContentInstallTask = NULL;
	CContentVerifyTask		*pContentVerifyTask = NULL;
	CContentRemoveTask		*pContentRemoveTask = NULL;
	COnlineTimer				Timer;
	XONLINE_FRIEND			*pFriendList = NULL;
	PXONLINEOFFERING_INFO		*rgpEnumInfo;
	XONLINE_USER UsersArray[XONLINE_MAX_STORED_ONLINE_USERS];
	DWORD					ServicesArray[] =
	{
		XONLINE_MATCHMAKING_SERVICE,
		XONLINE_BILLING_OFFERING_SERVICE
		// TODO - Add content-download here when it is figured out
	};
	DWORD dwServicesCount = sizeof(ServicesArray) / sizeof(DWORD);
	DWORD dwFriendCount = 0, dwResultsCount = 0, dwOfferingCount = 0, dwNumUsers = 0;
	BOOL fPartialResults = FALSE;

	//------------------------
	// LOGON STAGE - 4 users
	//------------------------

	LogonTask.RegisterLogHandle(g_hLog);

	// The logon task will automatically login with as many users as possible

	// Need to add the matchmaking and content services since they aren't included by default
	if(!LogonTask.SetServices(ServicesArray, dwServicesCount))
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Fill the users array with all of the users on the HD
	// We created 8 so I can be confident that I got the first 4 back (XONLINE_MAX_LOGON_USERS)
	XOnlineGetUsers(UsersArray, &dwNumUsers);
//	Assert(dwNumUsers == XONLINE_MAX_LOGON_USERS);

	// Set the member users array to match the one we've munged
	if(!LogonTask.SetUsers(UsersArray))
	{
		hr = E_FAIL;
		goto Exit;
	}

	hr = SGBVT_Logon(&LogonTask, &Timer);
	if(FAILED(hr))
	{
		goto Exit;
	}

	if(!LogonTask.IsConnectionEstablished())
	{
		LOGTASKINFO(g_hLog, "Logon 4 users timed-out");
		hr = E_FAIL;
		goto Exit;
	}

	LOGTASKINFO(g_hLog, "Auth test 4 users: PASSED");

	//------------------------
	// LOGON STAGE - Double Logon
	//------------------------

	// Logon with 4 users
	// Start logon async task
	if(!LogonTask.StartTask())
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Logon again - should work
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
		LOGTASKINFO(g_hLog, "Double logon timed-out");
		hr = E_FAIL;
		goto Exit;
	}

	LOGTASKINFO(g_hLog, "Auth test double Logon: PASSED");

	
	//------------------------
	// LOGON STAGE - 0 users
	//------------------------

	// Empty the Users Array
	memset(UsersArray, 0, 4*sizeof(XONLINE_USER));

	// Set the member users array to match the one we've munged
	if(!LogonTask.SetUsers(UsersArray))
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Logon with no users
	hr = SGBVT_Logon(&LogonTask, &Timer);
	if(FAILED(hr))
	{
		goto Exit;
	}

	if(!LogonTask.IsConnectionEstablished())
	{
		LOGTASKINFO(g_hLog, "Logon 0 users timed-out");
		hr = E_FAIL;
		goto Exit;
	}

	LOGTASKINFO(g_hLog, "Auth test 0 Users: PASSED");

	//------------------------
	// LOGON STAGE - 1 user
	//------------------------

	// Fill the users array with all of the users on the HD
	// We created 8 so I can be confident that I got the first 4 back (XONLINE_MAX_LOGON_USERS)
	XOnlineGetUsers(UsersArray, &dwNumUsers);
//	Assert(dwNumUsers == XONLINE_MAX_LOGON_USERS);

	// Empty the last three Users in the Array
	memset(&(UsersArray[1]), 0, 3*sizeof(XONLINE_USER));	// Users 2-4 deleted

	// Set the member users array to match the one we've munged
	// User 1 logs in
	if(!LogonTask.SetUsers(UsersArray))
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Logon with no users
	hr = SGBVT_Logon(&LogonTask, &Timer);
	if(FAILED(hr))
	{
		goto Exit;
	}

	if(!LogonTask.IsConnectionEstablished())
	{
		LOGTASKINFO(g_hLog, "Logon 1 user timed-out");
		hr = E_FAIL;
		goto Exit;
	}

	LOGTASKINFO(g_hLog, "Auth test 1 User: PASSED");

	//------------------------
	// LOGON STAGE - 2 users
	//------------------------

	// Fill the users array with all of the users on the HD
	// We created 8 so I can be confident that I got the first 4 back (XONLINE_MAX_LOGON_USERS)
	XOnlineGetUsers(UsersArray, &dwNumUsers);
//	Assert(dwNumUsers == XONLINE_MAX_LOGON_USERS);

	// Empty the last two Users in the Array
	memset(&(UsersArray[2]), 0, 2*sizeof(XONLINE_USER));	// Users 2-4 deleted

	// Set the member users array to match the one we've munged
	// User 1 logs in
	if(!LogonTask.SetUsers(UsersArray))
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Logon with no users
	hr = SGBVT_Logon(&LogonTask, &Timer);
	if(FAILED(hr))
	{
		goto Exit;
	}

	if(!LogonTask.IsConnectionEstablished())
	{
		LOGTASKINFO(g_hLog, "Logon 2 users timed-out");
		hr = E_FAIL;
		goto Exit;
	}

	LOGTASKINFO(g_hLog, "Auth test 2 users: PASSED");

	//------------------------
	// LOGON STAGE - 2 different users
	//------------------------

	// Fill the users array with all of the users on the HD
	// We created 8 so I can be confident that I got the first 4 back (XONLINE_MAX_LOGON_USERS)
	XOnlineGetUsers(UsersArray, &dwNumUsers);
//	Assert(dwNumUsers == XONLINE_MAX_LOGON_USERS);

	// Empty the first two users in the array
	memset(&(UsersArray[0]), 0, 2*sizeof(XONLINE_USER));	// First two users deleted

	// Set the member users array to match the one we've munged
	// Users 2 and 4 log in	
	if(!LogonTask.SetUsers(UsersArray))
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Logon with no users
	hr = SGBVT_Logon(&LogonTask, &Timer);
	if(FAILED(hr))
	{
		goto Exit;
	}

	if(!LogonTask.IsConnectionEstablished())
	{
		LOGTASKINFO(g_hLog, "Logon 2 different users timed-out");
		hr = E_FAIL;
		goto Exit;
	}

	LOGTASKINFO(g_hLog, "Auth test 2 different users: PASSED");	

	//------------------------
	// LOGON STAGE - Timeout connection
	//------------------------

	// Fill the users array with all of the users on the HD
	// We created 8 so I can be confident that I got the first 4 back (XONLINE_MAX_LOGON_USERS)
	XOnlineGetUsers(UsersArray, &dwNumUsers);
//	Assert(dwNumUsers == XONLINE_MAX_LOGON_USERS);

	// Set the member users array to match the one we've munged
	// All 4 users log in
	if(!LogonTask.SetUsers(UsersArray))
	{
		hr = E_FAIL;
		goto Exit;
	}

	// Logon with 4 users
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
		LOGTASKINFO(g_hLog, "Timeout test timed-out early");
		hr = E_FAIL;
		goto Exit;
	}

	// Wait 125 seconds, the time it takes for the SG to time out a client
	Sleep(XBOX_TIMEOUT);
	if(!LogonTask.IsConnectionEstablished())
	{
		LOGTASKINFO(g_hLog, "XNet stack didn't maintain pulses");
		hr = E_FAIL;
		goto Exit;
	}

	LOGTASKINFO(g_hLog, "Auth test timeout: PASSED");

	//------------------------
	// PRESENCE STAGE
	//------------------------

	FriendEnumTask.RegisterLogHandle(g_hLog);

	// The buddy enum task will automatically use the player at index 0

	// Start buddy enum async task
	if(!FriendEnumTask.StartTask())
	{
		hr = E_FAIL;
		goto Exit;
	}

	Timer.SetAllowedTime(MAX_GAMER_ENUM_TIME);
	Timer.Start();

	// Pump task for MAX_GAMER_ENUM_TIME seconds
	while(!FriendEnumTask.IsFriendListUpToDate())
	{
		if(Timer.HasTimeExpired())
		{
			LOGTASKINFO(g_hLog, "Friend enumeration task timed out");
			break;
		}

		FriendEnumTask.TaskContinue();
	}

	if(!FriendEnumTask.IsFriendListUpToDate())
	{
		LOGTASKINFO(g_hLog, "Friend enumeration task didn't report an up-to-date list");
		hr = E_FAIL;
		goto Exit;
	}

	dwFriendCount = FriendEnumTask.GetLatestFriendList(&pFriendList);

	LOGTASKINFO(g_hLog, "Friend test: PASSED");

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
 *  SGBVT_BasicTest
 *
 *  Description:
 *      Basic Test for SGBVT
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT SGBVT_BasicTest( void )
{

	CLogonTask*		pLogonTask;
	COnlineTimer		Timer;
	HRESULT			hr              = S_OK;
	DWORD					ServicesArray[] =
	{
		XONLINE_USER_ACCOUNT_SERVICE,
		XONLINE_MATCHMAKING_SERVICE
	};

	pLogonTask = new CLogonTask;
	if(!pLogonTask)
	{
		return E_OUTOFMEMORY;
	}
	
	DWORD                   dwServicesCount = sizeof(ServicesArray) / sizeof(DWORD), dwRandTime = 0, dwOnlineStatus = 0;
	XNADDR HostXnAddr;
	XNKID SessionID;
	XNKEY KeyExchangeKey;

	memset(&HostXnAddr, 0, sizeof(HostXnAddr));
	memset(&SessionID, 0, sizeof(SessionID));
	memset(&KeyExchangeKey, 0, sizeof(KeyExchangeKey));
	
	// Set the global logon task
	g_pLogonTask = pLogonTask;
	
	// Populate users accounts from user account creation service
	hr = COnlineAccounts::Instance()->PopulateUserAccountsHD(NULL, 8);
	if(hr != S_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Populating user accounts failed.");
		goto Exit;
	}
	
	pLogonTask->RegisterLogHandle(g_hLog);
	
	// The logon task will automatically login with as many users as possible
	
	// Add any requested services
	if(!pLogonTask->SetServices(ServicesArray, dwServicesCount))
	{
		hr = E_FAIL;
		goto Exit;
	}
	
	// Start logon async task
	if(!pLogonTask->StartTask())
	{
		hr = E_FAIL;
		goto Exit;
	}
	
	Timer.SetAllowedTime(MAX_LOGON_TIME);
	Timer.Start();
	
	// Pump task until complete
	while(!pLogonTask->IsConnectionEstablished())
	{
		if(Timer.HasTimeExpired())
			break;
		
		// Make sure to wait for the work event to test that it is always signalled when necessary
		if(!pLogonTask->WaitForWork(Timer.GetTimeRemaining()))
		{
			LOGTASKINFO(g_hLog, "Logon task wasn't signalled");
			break;
		}
		
		pLogonTask->TaskContinue();
	}
	
	if(!pLogonTask->IsConnectionEstablished())
	{
		LOGTASKINFO(g_hLog, "Logon timed-out");
		hr = E_FAIL;
		goto Exit;
	}
	
	// See if we can find a stress session to join
	hr = SearchForStressSession(&HostXnAddr, &SessionID, &KeyExchangeKey);

	// If a stress session was found with space left, then connect as a client
	if(hr == S_OK)
	{
		xLog(g_hLog, XLL_INFO, "Connecting to Match making session on %02x-%02x-%02x-%02x-%02x-%02x",
			HostXnAddr.abEnet[0],
			HostXnAddr.abEnet[1],
			HostXnAddr.abEnet[2],
			HostXnAddr.abEnet[3],
			HostXnAddr.abEnet[4],
			HostXnAddr.abEnet[5]);

		hr = client_Main(&HostXnAddr, &SessionID, &KeyExchangeKey);
	}
	// If a stress session wasn't found, then create one
	else if(hr == S_FALSE)
	{
		dwOnlineStatus = XNetGetTitleXnAddr(&HostXnAddr);
		if(!(dwOnlineStatus | XNET_GET_XNADDR_ONLINE))
		{
			LOGTASKINFO(g_hLog, "Error retrieving title XNADDR");
			hr = E_FAIL;
			goto Exit;
		}

		xLog(g_hLog, XLL_INFO, "Hosting stress session on %02x-%02x-%02x-%02x-%02x-%02x",
			HostXnAddr.abEnet[0],
			HostXnAddr.abEnet[1],
			HostXnAddr.abEnet[2],
			HostXnAddr.abEnet[3],
			HostXnAddr.abEnet[4],
			HostXnAddr.abEnet[5]);

		hr = host_Main();
	}
	// If some other error occurred, then exit
	else
	{
		LOGTASKINFOHR(g_hLog, "Error searching for stress session", hr);
		hr = E_FAIL;
		goto Exit;
	}

	if(pLogonTask)
	{
		delete pLogonTask;
		pLogonTask = NULL;
	}

	// Now that we're done with tests that require another server, run basic tests
	hr = SGBVT();

Exit:

	if(pLogonTask)
	{
		delete pLogonTask;
		pLogonTask = NULL;
	}

	if(FAILED(hr))
		TRACE(L"SGBVT FAILED: 0x%08x\n", hr);
	else
		TRACE(L"SGBVT SUCCEEDED\n");

    return hr;
}

/****************************************************************************
 *
 *  SGBVT_StartTest
 *
 *  Description:
 *      The Harness Entry into the SGBVT tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI SGBVT_StartTest( HANDLE LogHandle )
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

	g_hLog = LogHandle;

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

	XInitDevices(sizeof(deviceTypes) / sizeof(XDEVICE_PREALLOC_TYPE), deviceTypes);

	XNetAddRef();

	//	Sleep(1000);
		
	hr = XOnlineStartup(NULL);
		
	//
	// Test SGBVT in the mannor it was meant to be called
	//
		
	if ( SHOULDRUNTEST( "SGBVT", "Basic" ) )
	{
		SETLOG( LogHandle, "johnblac", "Online", "SGBVT", "Basic" );
		EXECUTE( SGBVT_BasicTest() );
	}
		
	g_hLog = NULL;
		
	hr = XOnlineCleanup();
		
	XNetRelease();
	
}

/****************************************************************************
 *
 *  SGBVT_EndTest
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

VOID WINAPI SGBVT_EndTest( VOID )
{
	g_fExitTest = TRUE;
}

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( SGBVT )
#pragma data_seg()

BEGIN_EXPORT_TABLE( SGBVT )
    EXPORT_TABLE_ENTRY( "StartTest", SGBVT_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", SGBVT_EndTest )
END_EXPORT_TABLE( SGBVT )
