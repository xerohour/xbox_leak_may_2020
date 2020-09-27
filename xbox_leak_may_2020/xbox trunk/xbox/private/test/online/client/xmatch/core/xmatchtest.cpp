//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//
// BUGBUG: Dummy stubs to let it compile
//

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionCreate(
    IN XNKID SessionID,
    IN DWORD dwNumAttributes,
    IN DWORD dwAttributesLen,
    IN HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE* phSession
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSetInt(
    IN XONLINETASK_HANDLE hSession,
    IN DWORD dwAttributeID,
    IN ULONGLONG qwUserPUID,
    IN ULONGLONG qwAttributeValue
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSetString(
    IN XONLINETASK_HANDLE hSession,
    IN DWORD dwAttributeID,
    IN ULONGLONG qwUserPUID,
    IN LPCWSTR pwszAttributeValue
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSetBlob(
    IN XONLINETASK_HANDLE hSession,
    IN DWORD dwAttributeID,
    IN ULONGLONG qwUserPUID,
    IN DWORD dwAttributeLength,
    IN PVOID pvAttributeValue
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionAddPlayer(
    IN XONLINETASK_HANDLE hSession,
    IN ULONGLONG qwUserPUID
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionRemovePlayer(
    IN XONLINETASK_HANDLE hSession,
    IN ULONGLONG qwUserPUID
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSend(
    IN XONLINETASK_HANDLE hSession
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionGetID (
    IN XONLINETASK_HANDLE hSession,
    OUT XNKID* pSessionID
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionDelete(
    IN XNKID SessionID,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phDelete
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionFindFromID(
    IN XNKID SessionID,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phSearch
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchCreate(
    IN DWORD dwProcedureIndex,
    IN DWORD dwNumResults,
    IN DWORD dwNumParameters,
    IN DWORD dwParametersLen,
    IN DWORD dwResultsLen,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phSearch
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchAppendInt(
    IN XONLINETASK_HANDLE hSearch,
    IN ULONGLONG qwParameterValue
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchAppendString(
    IN XONLINETASK_HANDLE hSearch,
    IN LPCWSTR pwszParameterValue
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchAppendBlob(
    IN XONLINETASK_HANDLE hSearch,
    IN DWORD dwParameterLen,
    IN LPVOID pvParameterValue
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchSend(
    IN XONLINETASK_HANDLE hSearch
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT 
WINAPI
XOnlineMatchSearchGetResults(
    IN XONLINETASK_HANDLE hSearch,
    OUT PXMATCH_SEARCHRESULT **prgpSearchResults,
    OUT DWORD *pdwReturnedResults
    )
{
    return S_OK;
}
 
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchGetAttribute(
    IN XONLINETASK_HANDLE hSearch,
    IN DWORD dwSearchResultIndex,
    IN DWORD dwAttributeIndex,
    OUT DWORD *pdwAttributeID,
    OUT VOID *pAttributeValue,
    OUT DWORD *pcbAttributeValue
    )
{
    return S_OK;
}

//==================================================================================
// Globals
//==================================================================================
// Test specific globals
static CRITICAL_SECTION g_csSerializeAccess;
static HANDLE g_hLog = INVALID_HANDLE_VALUE;
HANDLE g_hHeap = NULL;

static PXMATCH_LOCAL_TEST_FUNCTION g_LocalTestFunctions[XMATCHMSG_LOCAL_MAXTEST] = 
{
	XMatchTest_OpenSession,							//XMATCHMSG_OPENSESSION
	XMatchTest_SetAttributes,						//XMATCHMSG_SETATTRIBUTES
	XMatchTest_SendSession,							//XMATCHMSG_SENDSESSION
	XMatchTest_CreateSearch,						//XMATCHMSG_CREATESEARCH
	XMatchTest_AppendParams,						//XMATCHMSG_APPENDPARAMS
	XMatchTest_SendSearch,							//XMATCHMSG_SENDSEARCH
	XMatchTest_SearchResults,						//XMATCHMSG_SEARCHRESULTS
	XMatchTest_AddRemovePlayers,					//XMATCHMSG_ADDREMOVEPLAYERS
	XMatchTest_DeleteSession,						//XMATCHMSG_DELETESESSION
	XMatchTest_FindSessionFromID,					//XMATCHMSG_FINDFROMID
	XMatchTest_GetSessionID							//XMATCHMSG_GETSESSIONID
};

static LPSTR g_LocalTestNames[XMATCHMSG_LOCAL_MAXTEST] = 
{
	"Open session",									//XMATCHMSG_OPENSESSION
	"Set attributes",								//XMATCHMSG_SETATTRIBUTES
	"Send session",									//XMATCHMSG_SENDSESSION
	"Create search",								//XMATCHMSG_CREATESEARCH
	"Append params",								//XMATCHMSG_APPENDPARAMS
	"Send search",									//XMATCHMSG_SENDSEARCH
	"Search results",								//XMATCHMSG_SEARCHRESULTS
	"Add/Remove players",							//XMATCHMSG_ADDREMOVEPLAYERS
	"Delete session",								//XMATCHMSG_DELETESESSION
	"Find session from ID",							//XMATCHMSG_FINDFROMID
	"Get session ID"								//XMATCHMSG_GETSESSIONID
};

//==================================================================================
// MemAlloc
//----------------------------------------------------------------------------------
//
// MemAlloc: Private heap allocation function
//
// Arguments:
//	DWORD		dwSize		Size of buffer to allocate from the private heap
//
// Returns: Valid pointer to newly allocated memory, NULL otherwise
//==================================================================================
LPVOID MemAlloc(DWORD dwSize)
{
	if(!g_hHeap)
	{
//		DbgPrint("DirectPlay test's private heap hasn't been created!");
		return NULL;
	}

	return HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY, dwSize);
}

//==================================================================================
// MemFree
//----------------------------------------------------------------------------------
//
// Description: Private heap deallocation function
//
// Arguments:
//	LPVOID		pBuffer		Pointer to buffer to be released
//
// Returns: TRUE if the buffer was deallocated, FALSE otherwise
//==================================================================================
BOOL MemFree(LPVOID pBuffer)
{
	if(!g_hHeap)
	{
//		DbgPrint("DirectPlay test's private heap hasn't been created!");
		return FALSE;
	}

	return HeapFree(g_hHeap, 0, pBuffer);
}

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
HRESULT XOnlineLogonWrapper(HANDLE hLog, PXONLINETASK_HANDLE phLogon, DWORD* pServices, DWORD cServices, DWORD dwMaxWaitTime)
{
	XONLINE_USER		UsersArray[XONLINE_MAX_STORED_ONLINE_USERS];
	HRESULT				hr = S_OK;
	HANDLE				hEvent = NULL;
	DWORD				dwNumUsers = 0, dwStartTime = 0, dwCurrentTime = 0, dwElapsedTime = 0;
	BOOL				fDone = FALSE;
	
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
		goto Exit;

	// Get local users (we assume that the hard-drive has been populated)
	hr = XOnlineGetUsers( UsersArray, &dwNumUsers );
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
	hr = XOnlineLogon(UsersArray, pServices, cServices, hEvent, phLogon);
	if (FAILED(hr))
        goto Exit;

	dwElapsedTime = 0;
	dwStartTime = GetTickCount();

	// Pump until logon succeeds (PARTIAL_RESULTS flag set) or fails (DONT_CONTINUE flag set)
	xLog(hLog, XLL_INFO, "Waiting for logon to complete");
	do
	{

		dwCurrentTime = GetTickCount();

		// Calculate the elapsed time based on whether the time has wrapped around
		if(dwCurrentTime < dwStartTime)
			dwElapsedTime =  MAXDWORD - dwStartTime + dwCurrentTime;
		else
			dwElapsedTime = dwCurrentTime - dwStartTime;

		if(dwElapsedTime > dwMaxWaitTime)
		{
			xLog(hLog, XLL_INFO, "Logon didn't complete in allowed time");
			hr = E_FAIL;
			goto Exit;
		}

		if( WaitForSingleObject( hEvent, dwMaxWaitTime - dwElapsedTime ) == WAIT_OBJECT_0 )
		{
			fDone = XOnlineTaskDoWork(*phLogon, 0);
		}
		else
		{
			xLog(hLog, XLL_INFO, "Waiting for logon event failed!");
			hr = E_FAIL;
			break;
		}

	} while(!fDone);


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

	if(hEvent)
		CloseHandle(hEvent);

	return(hr);
}			

//==================================================================================
// ReadSearchResults
//----------------------------------------------------------------------------------
//
// Description: Adds attributes from a list to the async task indicated
//
// Arguments:
//	HANDLE				hLog				Handle to logging subsystem
//	XONLINETASK_HANDLE	hSearch				Handle to search task
//	DWORD				dwResultsExpected	Number of results expected
//	BOOL				fMoreResultsOK		Indicates whether an exact number of results is expected, or just a minimum
//
// Returns: TRUE if the search results were returned and formatted correctly, FALSE otherwise
//==================================================================================
BOOL ReadSearchResults(HANDLE hLog, XONLINETASK_HANDLE hSearch, DWORD dwResultsExpected, BOOL fMoreResultsOK)
{
	PXMATCH_SEARCHRESULT *ppSearchResults = NULL;
	HRESULT hr = S_OK;
	DWORD dwNumResults = 0;
	BOOL fPassed = TRUE;

	hr = XOnlineMatchSearchGetResults(hSearch, &ppSearchResults, &dwNumResults);
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "Getting search results failed with 0x%08x", hr);
		return FALSE;
	}

	if(fMoreResultsOK)
	{
		if(dwNumResults < dwResultsExpected)
		{
			xLog(hLog, XLL_FAIL, "Received %u results but at least %u were expected",
				dwNumResults, dwResultsExpected);
			return FALSE;
		}
	}
	else
	{
		if(dwNumResults != dwResultsExpected)
		{
			xLog(hLog, XLL_FAIL, "Received %u results but exactly %u were expected",
				dwNumResults, dwResultsExpected);
			return FALSE;
		}
	}

	xLog(hLog, XLL_PASS, "Recieved %u results as expected", dwNumResults, dwResultsExpected);
	return TRUE;
}

//==================================================================================
// AddAttributes
//----------------------------------------------------------------------------------
//
// Description: Adds attributes from a list to the async task indicated
//
// Arguments:
//	XONLINETASK_HANDLE	hAsyncHandle		Handle to async task
//	PATTRIBUTE			pAttributeArray		Pointer to an array of attributes
//	DWORD				dwAttributeCount	Indicates number of attributes in array
//
// Returns: TRUE if the attributes were added successfully, FALSE otherwise
//==================================================================================
BOOL AddAttributes(XONLINETASK_HANDLE hAsyncHandle, PATTRIBUTE pAttributeArray, DWORD dwAttributeCount)
{
	HRESULT hr = S_OK;
	DWORD dwAttributeIndex = 0;
	BOOL fRet = TRUE;

	if(!pAttributeArray)
	{
		fRet = FALSE;
		goto Exit;
	}

	for (dwAttributeIndex = 0; dwAttributeIndex < dwAttributeCount; ++dwAttributeIndex)
	{
		switch (pAttributeArray[dwAttributeIndex].dwAttributeID & X_ATTRIBUTE_DATATYPE_MASK)
		{
			case X_ATTRIBUTE_DATATYPE_INTEGER:
			{
				hr = XOnlineMatchSessionSetInt(hAsyncHandle, pAttributeArray[dwAttributeIndex].dwAttributeID,
					pAttributeArray[dwAttributeIndex].qwUserID, pAttributeArray[dwAttributeIndex].Value.qwValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding integer to session 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_STRING:
			{
				hr = XOnlineMatchSessionSetString(hAsyncHandle, pAttributeArray[dwAttributeIndex].dwAttributeID,
					pAttributeArray[dwAttributeIndex].qwUserID, (LPWSTR) pAttributeArray[dwAttributeIndex].Value.pvValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding string to session 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_BLOB:
			{
				hr = XOnlineMatchSessionSetBlob(hAsyncHandle, pAttributeArray[dwAttributeIndex].dwAttributeID,
					pAttributeArray[dwAttributeIndex].qwUserID, strlen((LPSTR)pAttributeArray[dwAttributeIndex].Value.pvValue),
					pAttributeArray[dwAttributeIndex].Value.pvValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding string to session 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}
		}
	}

Exit:

	return fRet;
}

//==================================================================================
// AddParameters
//----------------------------------------------------------------------------------
//
// Description: Adds parameters from a list to the async task indicated
//
// Arguments:
//	XONLINETASK_HANDLE	hAsyncHandle		Handle to async task
//	PPARAMETER			pParameterArray		Pointer to an array of parameters
//	DWORD				dwParameterCount	Indicates number of parameters in array
//
// Returns: TRUE if the attributes were added successfully, FALSE otherwise
//==================================================================================
BOOL AddParameters(XONLINETASK_HANDLE hAsyncHandle, PPARAMETER pParameterArray, DWORD dwParameterCount)
{
	HRESULT hr = S_OK;
	DWORD dwParameterIndex = 0;
	BOOL fRet = TRUE;

	if(!pParameterArray)
	{
		fRet = FALSE;
		goto Exit;
	}

	for (dwParameterIndex = 0; dwParameterIndex < dwParameterCount; ++dwParameterIndex)
	{
		switch (pParameterArray[dwParameterIndex].dwParameterType & X_ATTRIBUTE_DATATYPE_MASK)
		{
			case X_ATTRIBUTE_DATATYPE_INTEGER:
			{
				hr = XOnlineMatchSearchAppendInt(hAsyncHandle, pParameterArray[dwParameterIndex].Value.qwValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding integer to search 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_STRING:
			{
				hr = XOnlineMatchSearchAppendString(hAsyncHandle, (LPWSTR) pParameterArray[dwParameterIndex].Value.pvValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding string to search 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_BLOB:
			{
				hr = XOnlineMatchSearchAppendBlob(hAsyncHandle, strlen((LPSTR)pParameterArray[dwParameterIndex].Value.pvValue),
					pParameterArray[dwParameterIndex].Value.pvValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding string to search 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}
		}
	}

Exit:

	return fRet;
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
HRESULT PollTaskUntilComplete(XONLINETASK_HANDLE hAsyncHandle, HANDLE hWorkToDoEvent, DWORD dwMaxWaitTime, DWORD *pdwPollCounter, BOOL *pfCompleted, BOOL fCloseOnComplete)
{
	HRESULT hr = S_OK;
	DWORD dwLastTime = 0, dwCurrentTime = 0, dwCurrentDuration = 0, dwWaitTimeLeft = dwMaxWaitTime;
	DWORD dwInternalPollCounter = 0;
	BOOL fDone = FALSE;

	if(pfCompleted)
		*pfCompleted = FALSE;

	dwLastTime = GetTickCount();

	do
	{

		if((hWorkToDoEvent != INVALID_HANDLE_VALUE) && (hWorkToDoEvent != NULL))
			WaitForSingleObject(hWorkToDoEvent, dwWaitTimeLeft);

        fDone = XOnlineTaskDoWork(hAsyncHandle, 0);

		// Update the poll-counter
		++dwInternalPollCounter;

        if(fDone)
        {
			if(fCloseOnComplete)
				XOnlineTaskClose(hAsyncHandle);

			if(pfCompleted)
				*pfCompleted = TRUE;

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
// CreateSession
//----------------------------------------------------------------------------------
//
// Description: Creates a session with the specified attributes and returns the session ID 
//
// Arguments:
//	DWORD		dwPublicAvailable	Number of public slots available
//	DWORD		dwPrivateAvailable	Number of private slots available
//	PATTRIBUTE	pAttributeArray		Array of attributes to add to this session
//	DWORD		dwAttributeCount	Number of attributes in the array
//	DWORD		*pdwSessionID		Variable that passes back the creates session's ID
//
// Returns: TRUE if the session was created successfully, FALSE otherwise
//==================================================================================
BOOL CreateSession(ULONGLONG qwPublicAvailable, ULONGLONG qwPrivateAvailable, PATTRIBUTE pAttributeArray, DWORD dwAttributeCount, XNKID *pSessionID)
{
	XONLINETASK_HANDLE hMatch = NULL;
	ULONGLONG qwUserID = 0;
	HRESULT hr = S_OK;
    HANDLE hEvent = NULL;
	DWORD dwAttributeSize = 0, dwAttributeIndex = 0, dwPollCounter = 0;
	XNKID SessionID;
	BOOL fSuccess = FALSE, fCompleted = TRUE;

	if(!pSessionID || (!pAttributeArray && dwAttributeCount)) 
	{
		goto Exit;
	}

	memset(pSessionID, 0, sizeof(XNKID));
	qwUserID = 0;

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
	{
		goto Exit;
	}

	for(dwAttributeIndex = 0;dwAttributeIndex < dwAttributeCount; ++dwAttributeIndex)
	{
		switch(pAttributeArray[dwAttributeIndex].dwAttributeID & X_ATTRIBUTE_DATATYPE_MASK)
		{
		case X_ATTRIBUTE_DATATYPE_INTEGER:
			dwAttributeSize += sizeof(DWORD) + sizeof(ULONGLONG);
			break;
		case X_ATTRIBUTE_DATATYPE_STRING:
			dwAttributeSize += sizeof(DWORD) + sizeof(WORD) + (wcslen((WCHAR * ) (pAttributeArray[dwAttributeIndex].Value.pvValue)) + 1) * sizeof(WCHAR);
			break;
		case X_ATTRIBUTE_DATATYPE_BLOB:
			dwAttributeSize += sizeof(DWORD) + sizeof(WORD) + strlen((CHAR * ) (pAttributeArray[dwAttributeIndex].Value.pvValue));
			break;
		default:
			goto Exit;
		}

		// Add space for the user ID
// BUGBUG: Removed X_ATTRIBUTE_TYPE_MASK since it's no longer supported
//		if(!(pAttributeArray[dwAttributeIndex].dwAttributeID & X_ATTRIBUTE_TYPE_MASK))
//		{
			dwAttributeSize += sizeof(ULONGLONG);
//		}
	}

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, dwAttributeCount, dwAttributeSize, hEvent, &hMatch );
	if(FAILED(hr))
	{
		fSuccess = FALSE;
		goto Exit;
	}

	if(dwAttributeCount)
	{
		if(!AddAttributes(hMatch, pAttributeArray, dwAttributeCount))
		{
			fSuccess = FALSE;
			goto Exit;
		}
	}

// BUGBUG: Removed XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE since it's no longer supported
	hr = XOnlineMatchSessionSetInt(hMatch, /*XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE*/ 0, qwUserID, qwPublicAvailable);
	if(FAILED(hr))
	{		
		fSuccess = FALSE;
		goto Exit;
	}

// BUGBUG: Removed XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE since it's no longer supported
	hr = XOnlineMatchSessionSetInt(hMatch, /*XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE*/ 0, qwUserID, qwPrivateAvailable);
	if(FAILED(hr))
	{		
		fSuccess = FALSE;
		goto Exit;
	}

	hr = XOnlineMatchSessionSend(hMatch);
	if (FAILED(hr))
	{
		fSuccess = FALSE;
		goto Exit;
	}

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, TRUE);
	if((hr == S_OK) && fCompleted)
	{
		XOnlineMatchSessionGetID (hMatch, pSessionID);
		
		if(*((ULONGLONG *) pSessionID))
		{
			fSuccess = TRUE;
		}
		
		hMatch = NULL;
	}
	else
	{
		if(fCompleted)
			hMatch = NULL;
		fSuccess = FALSE;
		goto Exit;
	}

Exit:

	if(hMatch)
	{
		dwPollCounter = 0;
		XOnlineTaskClose(hMatch);
		hMatch = NULL;
	}

	if(hEvent)
		CloseHandle(hEvent);

	return fSuccess;
}


//==================================================================================
// XMatchTestDllMain
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
BOOL WINAPI XMatchTestDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
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
		InitializeCriticalSection(&g_csSerializeAccess);
		break;
	case DLL_PROCESS_DETACH:
		WSACleanup();
		XNetRelease();
		DeleteCriticalSection(&g_csSerializeAccess);
		HeapDestroy(g_hHeap);
		g_hHeap = NULL;
		break;
	default:
		break;
	}

    return TRUE;
}


//==================================================================================
// XMatchTestStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI XMatchTestStartTest(IN HANDLE  hLog)
{
	XONLINETASK_HANDLE hLogon = NULL;
	HRESULT hr = S_OK;
	HANDLE hPingObject = NULL, hEnumObject = NULL, hClientObject = NULL;
	DWORD dwInitialized = 0, dwServices = 0;
	INT nRet = 0, i;

	DWORD ServicesArray[NUM_SERVICES] =
	{
		XONLINE_MATCHMAKING_SERVICE
	};

	// Since we have to be thread safe, serialize entry for now
	EnterCriticalSection(&g_csSerializeAccess);

	g_hLog = hLog;

    // Set XLog info
	xSetOwnerAlias(g_hLog, "tristanj");
    xSetComponent(g_hLog, "Online(S)", "Matching - Client (S-o)");
	xSetFunctionName(g_hLog, "<none>");

	dwServices = (sizeof(ServicesArray)/sizeof(DWORD));

	hr = XOnlineLogonWrapper(hLog, &hLogon, ServicesArray, dwServices, 30000);
    if (FAILED(hr))
	{
		xLog(g_hLog, XLL_FAIL, "Failed to initialize services");
		goto Exit;
	}

	// Run tests that don't require another machine
	for(i = XMATCHMSG_LOCAL_BASE; i < XMATCHMSG_LOCAL_MAXTEST; i++)
//	for(i = XMATCHMSG_SENDSEARCH; i <= XMATCHMSG_SENDSEARCH; i++)
	{
		xSetFunctionName(g_hLog, g_LocalTestNames[i]);

		g_LocalTestFunctions[i](hLog, hLogon);

		xSetFunctionName(g_hLog, "");
	}

Exit:

	g_hLog = INVALID_HANDLE_VALUE;

	CLEANUP_TASK(hLogon);

	// Allow other threads to enter now
	LeaveCriticalSection(&g_csSerializeAccess);
}            

//==================================================================================
// XMatchTestEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI XMatchTestEndTest()
{
	// We already cleaned up everything at the end of StartTest
}

} // namespace XMatchTestNamespace

// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( xmatchtest )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( xmatchtest )
    EXPORT_TABLE_ENTRY( "StartTest", XMatchTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XMatchTestEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", XMatchTestDllMain )
END_EXPORT_TABLE( xmatchtest )
