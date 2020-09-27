//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include "xmatchbvt.h"

#define	TestingTime	300000
#define BUG11022READY 0

using namespace XMatchBVTNamespace;

namespace XMatchBVTNamespace {

//==================================================================================
// Globals
//==================================================================================
HANDLE g_hHeap = NULL;

//==================================================================================
// Helper functions
//==================================================================================

DWORD GetBufferLenForInt()
{
	return sizeof(DWORD) + sizeof(ULONGLONG);
}

DWORD GetBufferLenForString(LPWSTR szString)
{
	DWORD dwSize = sizeof(DWORD) + sizeof(WORD);
	dwSize += (wcslen(szString) + 1) * sizeof(WCHAR);
	return dwSize;
}

DWORD GetBufferLenForBlob(LPSTR szBlob)
{
	DWORD dwSize = sizeof(DWORD) + sizeof(WORD);
	dwSize += strlen(szBlob) * sizeof(CHAR);
	return dwSize;
}

DWORD GetBufferLenForUserInt()
{
	return (sizeof(ULONGLONG) + GetBufferLenForInt());
}

DWORD GetBufferLenForUserString(LPWSTR szString)
{
	return (sizeof(ULONGLONG) + GetBufferLenForString(szString));
}

DWORD GetBufferLenForUserBlob(LPSTR szBlob)
{
	return (sizeof(ULONGLONG) + GetBufferLenForBlob(szBlob));
}

//==================================================================================
// XOnlineLogonWrapper
//----------------------------------------------------------------------------------
//
// Description: XOnlineLogon wrapper code stolen from dev tests
//
// Arguments:
//	HANDLE				hLog			Handle to logging subsystem
//	DWORD*          	pServices		Pointer to services for which credentials are desired
//	DWORD				cServices		Number of services
//	DWORD				dwMaxWaitTime	Maximum number of seconds to allow for logon to complete
//
// Returns: Passes back async completion result
//==================================================================================
HRESULT XOnlineLogonWrapper(HANDLE hLog, HANDLE	hEvent, PXONLINETASK_HANDLE phLogon, 
									DWORD* pServices, DWORD cServices, DWORD dwMaxWaitTime)
{
	XONLINE_USER        *pUsersArray;
	HRESULT				hr = E_FAIL;
	DWORD				dwNumUsers = 0, dwStartTime = 0, dwCurrentTime = 0, dwElapsedTime = 0;
	HRESULT 			hrDoWork2;
	TCHAR 				szMsg[128];

	// Get local users (we assume that the hard-drive has been populated)
    pUsersArray =  (XONLINE_USER *) LocalAlloc(LPTR,XONLINE_MAX_STORED_ONLINE_USERS * sizeof(XONLINE_USER));

    if(pUsersArray)        
	    hr = XOnlineGetUsers( pUsersArray, &dwNumUsers );
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

	if(!dwNumUsers)
	{
		OutputDebugString(TEXT("No users were found on the hard-drive, run the SetupOnline tool!"));
		xLog(hLog, XLL_INFO, "No users were found on the hard-drive, run the SetupOnline tool!");
		hr = E_FAIL;
		goto Exit;
	}

	// Initialize services
	xLog(hLog, XLL_INFO, "Logging into %u services with %u users", cServices, dwNumUsers);
	hr = XOnlineLogon(pUsersArray, pServices, cServices, hEvent, phLogon);

       LocalFree(pUsersArray);

	if (FAILED(hr))
        goto Exit;

	dwElapsedTime = 0;
	dwStartTime = GetTickCount();

	// Pump until logon succeeds (PARTIAL_RESULTS flag set) or fails (DONT_CONTINUE flag set)
	xLog(hLog, XLL_INFO, "Waiting for logon to complete");
	do
	{
		if( WaitForSingleObject( hEvent, dwMaxWaitTime) != WAIT_OBJECT_0 )
		{
			xLog(hLog, XLL_INFO, "Waiting for logon event failed!");
			hr = E_FAIL;
			goto Exit;
		}

		dwCurrentTime = GetTickCount();

		hrDoWork2 = XOnlineTaskContinue(*phLogon);

	} while( XONLINETASK_S_RUNNING == hrDoWork2);		//same as while ( !XONLINETASK_STATUS_AVAILABLE(hrDoWork2) );while( XONLINETASK_S_RUNNING == hrDoWorks)

	wsprintf(szMsg, TEXT("XMatchBVTNamespace__XOnlineLogonWrapper: XOnlineTaskContinue returned %0x at last\n"),hrDoWork2);
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
// PollTaskUntilComplete
//----------------------------------------------------------------------------------
//
// Description: Calls the XOnlineTaskComplete function until either the given
//    task has been completed or the amount of available time has expired
//
// Arguments:
//	XONLINETASK_HANDLE	hAsyncHandle		Handle to async task
//	HANDLE				hWorkToDoEvent		Event that is associated with this async task.
//											This can be set to NULL or INVALID_HANDLE_VALUE
//											if no event is associated with the task
//	DWORD				dwMaxWaitTime		Maximum amount of time to wait for the task to
//											complete. This can be set to INFINITE to block
//											until completion
//	DWORD				*pdwPollCounter		Optional counter passed in to keep track of the
//											number of calls made to the XOnlineTaskDoWork function
//	BOOL				*pfCompleted		Optional boolean passed in to keep track if whether
//											the task completed or not
//  BOOL				fCloseOnComplete	Indicates whether the function should close a handle
//											after it indicates completion
//
// Returns: The last result code from XOnlineTaskDoWork is returned.  If a valid pointer
//   is provided for pfCompleted, it is updated to reflect whether the task has completed or not
//==================================================================================
HRESULT PollTaskUntilComplete(XONLINETASK_HANDLE hAsyncHandle, HANDLE hWorkToDoEvent, DWORD dwMaxWaitTime, 
									DWORD *pdwPollCounter, BOOL *pfCompleted, BOOL fCloseOnComplete)
{
	HRESULT hr = S_OK;
	DWORD dwLastTime = 0, dwCurrentTime = 0, dwCurrentDuration = 0, dwWaitTimeLeft = dwMaxWaitTime;
	DWORD dwInternalPollCounter = 0;
	HRESULT 			hrDoWork2;

	if(pfCompleted)
		*pfCompleted = FALSE;

	dwLastTime = GetTickCount();

	do
	{

		if((hWorkToDoEvent != INVALID_HANDLE_VALUE) && (hWorkToDoEvent != NULL))
			WaitForSingleObject(hWorkToDoEvent, dwWaitTimeLeft);

		hrDoWork2 = XOnlineTaskContinue(hAsyncHandle);

		// Update the poll-counter
		++dwInternalPollCounter;

		if (XONLINETASK_S_RUNNING != hrDoWork2) // same as if (XONLINETASK_STATUS_AVAILABLE(hrDoWork2))
		{
			if (XONLINETASK_S_SUCCESS == hrDoWork2)	// same as if (XONLINETASK_STATUS_SUCCESSFUL_COMPLETION(hrDoWork2))
			{
				if(pfCompleted)
					*pfCompleted = TRUE;
			}
			else if(FAILED(hrDoWork2))
			{
			    	hr = hrDoWork2;
			}

			if(fCloseOnComplete)
				XOnlineTaskClose(hAsyncHandle);

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
				DbgPrint("Async opration didn't complete within %u ms\n", dwMaxWaitTime);
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
// XMatchBVTDllMain
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
BOOL WINAPI XMatchBVTDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
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


//==================================================================================
// XMatchBVTStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI XMatchBVTStartTest(IN HANDLE  hLog)
{
	PXMATCH_SEARCHRESULT *ppSearchResult = NULL;
	XONLINETASK_HANDLE hLogon = NULL, hSessionTask = NULL, hUpdateTask = NULL, hSearchTask = NULL, hDeleteTask = NULL;
	DWORD ServicesArray[] =
	{
		XONLINE_MATCHMAKING_SERVICE
	};
//	XONLINE_USER UsersArray[XONLINE_MAX_STORED_ONLINE_USERS];
	ULONGLONG qwAttributeValue = 0;
	XNKID SessionID, newSessionID;
    XNKEY KeyExchangeKey;
	HRESULT hr = S_OK;
	HANDLE hLogonEvent, hSessionEvent = NULL, hUpdateEvent, hSearchEvent = NULL, hDeleteEvent = NULL;
	DWORD dwServices = 0, dwNumUsers = 0, dwNumResults = 0, dwAttributeType = 0, dwAttributeLen = 0, dwLogonFlags = 0;
	BOOL fDone = FALSE;
	HRESULT hrDoWork2;
	XONLINE_ATTRIBUTE	xOnAttr[6];

	hr = XOnlineStartup(NULL);
	Verify(hr == S_OK);	

#if BUG11022READY
	__try{
		XONLINETASK_HANDLE hTaskGarbage;
		XOnlineTaskClose(hTaskGarbage);
	}
	__except(EXCEPTION_EXECUTE_HANDLER){
		DWORD dwEx = GetExceptionCode();
	}
#endif

	xSetOwnerAlias(hLog, "tristanj");
	xSetComponent(hLog, "Online", "Matching - Client (O)");

	hLogonEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hSessionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hUpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hSearchEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hDeleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hLogonEvent || !hSessionEvent || !hUpdateEvent || !hSearchEvent || !hDeleteEvent)
	{
		xLog(hLog, XLL_FAIL, "Couldn't create events for matchmaking BVT");
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

	// Handle logging on to necessary services
	dwServices = (sizeof(ServicesArray)/sizeof(DWORD));
//	hr = XOnlineLogonWrapper(hLog, &hLogon, NULL, 0, 30000);
	hr = XOnlineLogonWrapper(hLog, hLogonEvent, &hLogon, ServicesArray, dwServices, TestingTime);
	if(hr != XONLINE_S_LOGON_CONNECTION_ESTABLISHED)
	{
		xLog(hLog, XLL_FAIL, "Failed logon.");
		goto Exit;
	}

//==================================================================================
	xSetFunctionName(hLog, "XOnlineMatchSessionCreate");
	xStartVariation(hLog, "Advertise a session on the matchmaking server");
//==================================================================================
	while(TRUE)
	{
		memset(&SessionID, 0, sizeof(SessionID));

		xOnAttr[0].dwAttributeID = GLOBALINT_ID1;
		xOnAttr[0].info.integer.qwValue = INT1;
		xOnAttr[1].dwAttributeID = GLOBALSTR_ID1;
		xOnAttr[1].info.string.pwszValue = STRING1;
		xOnAttr[2].dwAttributeID = GLOBALBLB_ID1;
		xOnAttr[2].info.blob.pvValue = BLOB1;
		xOnAttr[2].info.blob.dwLength = strlen(BLOB1);
#if 0
		xOnAttr[3].dwAttributeID = GLOBALINT_ID2;
		xOnAttr[3].info.integer.qwValue = INT2;
		xOnAttr[4].dwAttributeID = GLOBALSTR_ID2;
		xOnAttr[4].info.string.pwszValue = STRING2;
		xOnAttr[5].dwAttributeID = GLOBALBLB_ID2;
		xOnAttr[5].info.blob.pvValue = BLOB2;
		xOnAttr[5].info.blob.dwLength = strlen(BLOB2);
#endif
		
		hr = XOnlineMatchSessionCreate(0,10,0,10,3, xOnAttr, hSessionEvent, &hSessionTask );

		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
		if(FAILED(hr))
		{
			FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
		}

		hr = PollTaskUntilComplete(hSessionTask, hSessionEvent, TestingTime, NULL, &fDone, FALSE);
		if(FAILED(hr))
		{
			xLog(hLog, XLL_INFO, "Pumping XOnlineMatchSessionCreate returned 0x%08x", hr);
			FAIL_TESTCASE("Session creation failed asynchronously");
		}
		else if (!fDone)
		{
			FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
		}

		PASS_TESTCASE("Session successfully advertised");

		break; 
	}; 

	PUMP_CS_EVENT()
	xEndVariation(hLog);
	
	xSetFunctionName(hLog, "");

//==================================================================================
	xSetFunctionName(hLog, "XOnlineMatchSessionGetInfo");
	xStartVariation(hLog, "Call XOnlineMatchSessionGetInfo");
//==================================================================================
	BOOL	bValidSession = FALSE;

	while(TRUE)
	{
		hr = XOnlineMatchSessionGetInfo(hSessionTask, &SessionID, &KeyExchangeKey);

		xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetInfo  returned 0x%08x", hr);
		if(FAILED(hr))
		{
			FAIL_TESTCASE("XOnlineMatchSessionGetInfo  failed unexpectedly");
		}
		else if(*((ULONGLONG *) &SessionID) == 0)
		{
			FAIL_TESTCASE("XOnlineMatchSessionGetInfo  returned success but didn't return session ID");
		}

		PASS_TESTCASE("XOnlineMatchSessionGetInfo return correctly");
		bValidSession = TRUE;
		break; 
	}; 

	PUMP_CS_EVENT()
	xEndVariation(hLog);
	
	CLEANUP_TASK(hSessionTask);
	xSetFunctionName(hLog, "");

	if(!bValidSession)
	{
		xLog(hLog, XLL_INFO, "Couldn't get session id. Should finish whole test.");
		goto Exit;
	}

//==================================================================================
	xSetFunctionName(hLog, "XOnlineMatchSessionUpdate");
	xStartVariation(hLog, "Advertise a session on the matchmaking server");
//==================================================================================
	while(TRUE)
	{
		xOnAttr[0].dwAttributeID = GLOBALINT_ID1;
		xOnAttr[0].info.integer.qwValue = INT1;
		xOnAttr[1].dwAttributeID = GLOBALSTR_ID1;
		xOnAttr[1].info.string.pwszValue = STRING1;
		xOnAttr[2].dwAttributeID = GLOBALBLB_ID1;
		xOnAttr[2].info.blob.pvValue = BLOB1;
		xOnAttr[2].info.blob.dwLength = strlen(BLOB1);
#if 0
		xOnAttr[3].dwAttributeID = GLOBALINT_ID2;
		xOnAttr[3].info.integer.qwValue = INT2;
		xOnAttr[4].dwAttributeID = GLOBALSTR_ID2;
		xOnAttr[4].info.string.pwszValue = STRING2;
		xOnAttr[5].dwAttributeID = GLOBALBLB_ID2;
		xOnAttr[5].info.blob.pvValue = BLOB2;
		xOnAttr[5].info.blob.dwLength = strlen(BLOB2);
#endif
		
		hr = XOnlineMatchSessionUpdate(SessionID, 5,5,5,5,3, xOnAttr, hUpdateEvent, &hUpdateTask );

		xLog(hLog, XLL_INFO, "XOnlineMatchSessionUpdate returned 0x%08x", hr);
		if(FAILED(hr))
		{
			FAIL_TESTCASE("XOnlineMatchSessionUpdate failed unexpectedly");
		}

		hr = PollTaskUntilComplete(hUpdateTask, hUpdateEvent, TestingTime, NULL, &fDone, FALSE);
		if(FAILED(hr))
		{
			xLog(hLog, XLL_INFO, "Pumping XOnlineMatchSessionUpdate returned 0x%08x", hr);
			FAIL_TESTCASE("Session creation failed asynchronously");
		}
		else if (!fDone)
		{
			FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
		}

		hr = XOnlineMatchSessionGetInfo(hUpdateTask, &newSessionID, &KeyExchangeKey);

		xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
		if(FAILED(hr))
		{
			FAIL_TESTCASE("XOnlineMatchSessionGetID  failed unexpectedly");
		}
		else if(*((ULONGLONG *) &SessionID) != *((ULONGLONG *) &newSessionID))
		{
			FAIL_TESTCASE("XOnlineMatchSessionGetID  returned different session ID");
		}

		PASS_TESTCASE("Session successfully advertised");

		break; 
	}; 

	PUMP_CS_EVENT()
	xEndVariation(hLog);
	
	CLEANUP_TASK(hUpdateTask);
	xSetFunctionName(hLog, "");
//==================================================================================
	xSetFunctionName(hLog, "XOnlineMatchSearch");
	xStartVariation(hLog, "Search for a session on the matchmaking server");
//==================================================================================

	while(TRUE)
	{
		xOnAttr[0].dwAttributeID = X_ATTRIBUTE_DATATYPE_INTEGER;
		xOnAttr[0].info.integer.qwValue = *((ULONGLONG *) &SessionID);
//		hr = XOnlineMatchSearch(0, 10, 0, NULL, 10*(sizeof(XMATCH_SEARCHRESULT) + 500), hSearchEvent, &hSearchTask);
		hr = XOnlineMatchSearch(GETSESSINT_INDEX, 1, 1, xOnAttr, sizeof(XMATCH_SEARCHRESULT) + 500, hSearchEvent, &hSearchTask);

		xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
		if(FAILED(hr))
		{
			FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
		}

		hr = PollTaskUntilComplete(hSearchTask, hSearchEvent, TestingTime, NULL, &fDone, FALSE);
		if(FAILED(hr))
		{
			xLog(hLog, XLL_INFO, "Pumping XOnlineMatchSearchCreate returned 0x%08x", hr);
			FAIL_TESTCASE("Session search failed asynchronously");
		}
		else if (!fDone)
		{
			FAIL_TESTCASE("Session search didn't complete within 30 seconds");
		}

		PASS_TESTCASE("Session search successful");

		break; 
	}; 

	PUMP_CS_EVENT()
	xEndVariation(hLog);
	
	xSetFunctionName(hLog, "");

//==================================================================================
	xSetFunctionName(hLog, "XOnlineMatchSearchGetResults");
	xStartVariation(hLog, "Call XOnlineMatchSearchGetResults");
//==================================================================================

	while(TRUE)
	{
		hr = XOnlineMatchSearchGetResults(hSearchTask, &ppSearchResult, &dwNumResults);

		xLog(hLog, XLL_INFO, "XOnlineMatchSearchGetResults returned 0x%08x", hr);
		if(FAILED(hr))
		{
			FAIL_TESTCASE("XOnlineMatchSearchGetResults failed unexpectedly");
		}
		else if(!dwNumResults)
		{
			FAIL_TESTCASE("XOnlineMatchSearchGetResults succeeded but didn't return a result");
		}

		if(ppSearchResult == NULL)
		{
			FAIL_TESTCASE("Returned search result is NULL");
		}

		if(memcmp(&((ppSearchResult[0])->SessionID), &SessionID, sizeof(XNKID)))
		{
			FAIL_TESTCASE("Returned search result had incorrect session ID");
		}

		PASS_TESTCASE("Call XOnlineMatchSearchGetResults successful");

		break; 
	}; 

	PUMP_CS_EVENT()
	xEndVariation(hLog);
	

	xSetFunctionName(hLog, "");

//==================================================================================
	xSetFunctionName(hLog, "XOnlineMatchSearchParse");
	xStartVariation(hLog, "Call XOnlineMatchSearchParse");
//==================================================================================

#pragma pack(push, 1)
	typedef struct
	{
		ULONGLONG	syInt;
	} SYSearchStruct;
#pragma pack(pop)

	while(TRUE)
	{
		XONLINE_ATTRIBUTE_SPEC	xonAttribSpec[3];
		SYSearchStruct	sySeachParse;
		
		xonAttribSpec[0].type = X_ATTRIBUTE_DATATYPE_INTEGER;
		xonAttribSpec[0].length = 8;

		for( DWORD ii=0; ii<dwNumResults; ii++)
		{
			hr = XOnlineMatchSearchParse(ppSearchResult[ii], 1, xonAttribSpec, (PVOID)&sySeachParse);

			xLog(hLog, XLL_INFO, "XOnlineMatchSearchParse returned 0x%08x", hr);
			if(FAILED(hr))
			{
				FAIL_TESTCASE("XOnlineMatchSearchParse failed unexpectedly");
			}
		}
		
		PASS_TESTCASE("Call XOnlineMatchSearchParse successful");

//
// Here need to check if the result is correct.
//

		break; 
	}; 

	PUMP_CS_EVENT()
	xEndVariation(hLog);
	
	CLEANUP_TASK(hSearchTask);
	xSetFunctionName(hLog, "");

//==================================================================================
	xSetFunctionName(hLog, "XOnlineMatchSessionFindFromID");
	xStartVariation(hLog, "Search for a session using session ID on the matchmaking server");
//==================================================================================
	XONLINETASK_HANDLE hFindFromIDTask;
	HANDLE hFindFromIDEvent;

	hFindFromIDEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	while(TRUE)
	{
		hr = XOnlineMatchSessionFindFromID(SessionID, hFindFromIDEvent, &hFindFromIDTask);

		xLog(hLog, XLL_INFO, "XOnlineMatchSessionFindFromID returned 0x%08x", hr);
		if(FAILED(hr))
		{
			FAIL_TESTCASE("XOnlineMatchSessionFindFromID failed unexpectedly");
		}

		hr = PollTaskUntilComplete(hFindFromIDTask, hFindFromIDEvent, TestingTime, NULL, &fDone, FALSE);
		if(FAILED(hr))
		{
			xLog(hLog, XLL_INFO, "Pumping XOnlineMatchSessionFindFromID returned 0x%08x", hr);
			FAIL_TESTCASE("Session search using Session ID failed asynchronously");
		}
		else if (!fDone)
		{
			FAIL_TESTCASE("Session search using Session ID didn't complete within 30 seconds");
		}

		PASS_TESTCASE("Session search using Session ID successful");

		break; 
	}; 

	PUMP_CS_EVENT()
	xEndVariation(hLog);
	
	xSetFunctionName(hLog, "");

//==================================================================================
	xSetFunctionName(hLog, "XOnlineMatchSearchGetResults");
	xStartVariation(hLog, "Call XOnlineMatchSearchGetResults");
//==================================================================================

	while(TRUE)
	{
		hr = XOnlineMatchSearchGetResults(hFindFromIDTask, &ppSearchResult, &dwNumResults);

		xLog(hLog, XLL_INFO, "XOnlineMatchSearchGetResults returned 0x%08x", hr);
		if(FAILED(hr))
		{
			FAIL_TESTCASE("XOnlineMatchSearchGetResults failed unexpectedly");
		}
		else if(!dwNumResults)
		{
			FAIL_TESTCASE("XOnlineMatchSearchGetResults succeeded but didn't return a result");
		}

		if(ppSearchResult == NULL)
		{
			FAIL_TESTCASE("Returned search result is NULL");
		}

		if(memcmp(&((ppSearchResult[0])->SessionID), &SessionID, sizeof(XNKID)))
		{
			FAIL_TESTCASE("Returned search result had incorrect session ID");
		}

		PASS_TESTCASE("Call XOnlineMatchSearchGetResults successful");

		break; 
	}; 

	PUMP_CS_EVENT()
	xEndVariation(hLog);
	
	CLEANUP_TASK(hFindFromIDTask);
	xSetFunctionName(hLog, "");


//==================================================================================
	xSetFunctionName(hLog, "XOnlineMatchSessionDelete");
	xStartVariation(hLog, "Delete a session that was created earlier");
//==================================================================================

	while(TRUE)
	{
		hr = XOnlineMatchSessionDelete(SessionID, hDeleteEvent, &hDeleteTask);

		xLog(hLog, XLL_INFO, "XOnlineMatchSessionDelete returned 0x%08x", hr);
		if(FAILED(hr))
		{
			FAIL_TESTCASE("XOnlineMatchSessionDelete failed unexpectedly");
		}

		hr = PollTaskUntilComplete(hDeleteTask, hDeleteEvent, TestingTime, NULL, &fDone, FALSE);
		if(FAILED(hr))
		{
			xLog(hLog, XLL_INFO, "Pumping XOnlineMatchSessionDelete returned 0x%08x", hr);
			FAIL_TESTCASE("Session delete failed asynchronously");
		}
		else if (!fDone)
		{
			FAIL_TESTCASE("Session delete didn't complete within 30 seconds");
		}

		PASS_TESTCASE("Session delete successful");

		break; 
	}; 

	PUMP_CS_EVENT()
	xEndVariation(hLog);
	
	CLEANUP_TASK(hDeleteTask);
	xSetFunctionName(hLog, "");


Exit:

	if(hSessionTask)
		CLEANUP_TASK(hSessionTask);

	if(hSessionEvent)
		CloseHandle(hSessionEvent);

	if(hUpdateTask)
		CLEANUP_TASK(hUpdateTask);

	if(hUpdateEvent)
		CloseHandle(hUpdateEvent);

	if(hSearchTask)
		CLEANUP_TASK(hSearchTask);

	if(hSearchEvent)
		CloseHandle(hSearchEvent);

	if(hSearchTask)
		CLEANUP_TASK(hFindFromIDTask);

	if(hSearchEvent)
		CloseHandle(hFindFromIDEvent);

	if(hDeleteTask)
		CLEANUP_TASK(hDeleteTask);

	if(hDeleteEvent)
		CloseHandle(hDeleteEvent);

	CLEANUP_TASK(hLogon);

	if(hLogonEvent)
		CloseHandle(hLogonEvent);
     
	hr = XOnlineCleanup();
	Verify(hr == S_OK);	

	return;
}

//==================================================================================
// XMatchBVTEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI XMatchBVTEndTest()
{

}

} // namespace XAuthTestNamespace

// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XMatchBVT )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( XMatchBVT )
    EXPORT_TABLE_ENTRY( "StartTest", XMatchBVTStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XMatchBVTEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", XMatchBVTDllMain )
END_EXPORT_TABLE( XMatchBVT )

