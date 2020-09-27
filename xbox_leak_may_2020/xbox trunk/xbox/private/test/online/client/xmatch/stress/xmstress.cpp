//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include "xmstress.h"

using namespace XMatchStressNamespace;

namespace XMatchStressNamespace {

//==================================================================================
// Globals
//==================================================================================
// Test specific globals
static HANDLE g_hLog = INVALID_HANDLE_VALUE;
HANDLE g_hHeap = NULL;

// Returns the elapsed time given the start and current times
inline DWORD GetElapsedTime(DWORD dwStartTime, DWORD dwCurrentTime)
{
	dwCurrentTime = GetTickCount();
	if(dwStartTime > dwCurrentTime)
		return (MAXDWORD - dwStartTime + dwCurrentTime);
	else
		return (dwCurrentTime - dwStartTime);
}

// Generates a random positive integer between 0 and dwMaxValue
inline DWORD GenerateRandomNumber(DWORD dwMaxValue)
{
	return ((DWORD) rand() % dwMaxValue);
}

template< class T > void ShuffleRemainingArrayItems( T *pStartLocation, DWORD dwItemsToShuffle)
{
   T *pNext = pStartLocation + 1, *pCurrent = pStartLocation;

   while(dwItemsToShuffle--)
   {
	   *pCurrent = *pNext;
	   pCurrent++;
	   pNext++;
   }

   *pCurrent = NULL;
}


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

//==================================================================================
// XOnlineLogonWrapper
//----------------------------------------------------------------------------------
//
// Description: XOnlineLogon wrapper code stolen from dev tests
//
// Arguments:
//	HANDLE				hLog		Handle to logging subsystem
//	PXONLINE_SERVICE	pServices	Pointer to services for which credentials are desired
//	DWORD				cServices	Number of services
//
// Returns: Passes back async completion result
//==================================================================================
HRESULT XOnlineLogonWrapper(HANDLE hLog, PXONLINE_SERVICE pServices, DWORD cServices)
{
	XONLINETASK_HANDLE	hLogon;
	XONLINE_USER		UsersArray[XONLINE_MAX_STORED_ONLINE_USERS];
	HRESULT				hr = S_OK;
	HANDLE				hEvent = NULL;
	DWORD				dwWorkFlags = 0, dwNumUsers = 0;
	
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
		goto Exit;

	XOnlineSetupTempAccounts( TRUE, XONLINE_LOCAL_HARD_DRIVE, XONLINE_MAX_STORED_ONLINE_USERS );

	// Get local users
	hr = XOnlineGetUsers( XONLINE_LOCAL_HARD_DRIVE, UsersArray, &dwNumUsers );
	if (FAILED(hr))
        goto Exit;

	// Initialize services
	xLog(hLog, XLL_INFO, "Logging into %u services with %u users", cServices, dwNumUsers);
	hr = XOnlineLogon(UsersArray, pServices, cServices, hEvent, &hLogon);
	if (FAILED(hr))
        goto Exit;

	// Pump until logon returns
	xLog(hLog, XLL_INFO, "Waiting for logon to complete");
	do
	{
		if( WaitForSingleObject( hEvent, 30000 ) == WAIT_OBJECT_0 )
		{
			hr = XOnlineTaskContinue(hLogon, 0, &dwWorkFlags);
		}
		else
		{
			xLog(hLog, XLL_INFO, "Waiting for logon event failed!");
			hr = E_FAIL;
			break;
		}

	} while (!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags));

	if(!FAILED(hr))
		xLog(hLog, XLL_INFO, "Logon completed successfully");
	else
		xLog(hLog, XLL_INFO, "Logon failed with 0x%08x", hr);

Exit:

	if(hEvent)
		CloseHandle(hEvent);

	return(hr);
}			

//==================================================================================
// XMatchStressDllMain
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
BOOL WINAPI XMatchStressDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
	WSADATA WSAData;

	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		XNetAddRef();
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
// StartFollowUpXmatchRequest
//----------------------------------------------------------------------------------
//
// Description: Creates a follow up request to a previous session creation.  The
//              follow up request can be either a modfication, a deletion or a search
//
// Arguments:
//	HANDLE					hLog						Handle to logging subsystem
//  XMATCH_REQUEST_CATEGORY	nPreviousRequestCategory	Previous request category which the
//														follow up will be based on
//	XNKID					SessionID					Session ID of the previous request
//
// Returns:
//	A pointer to the created Xmatch request or NULL if one couldn't be created
//==================================================================================
CXOnlineAsyncRequest *StartFollowUpXmatchRequest(HANDLE hLog, XMATCH_REQUEST_CATEGORY nPreviousRequestCategory, XNKID SessionID, DWORD dwPreviousAsyncID)
{
	CXOnlineAsyncRequest *pRequest = NULL;
	CXMatchSessionRequest *pFollowUpSession = NULL;
	CXMatchSearchRequest *pFollowUpSearch = NULL;
	DWORD dwNumAttributes = 0, dwAttributesLen = 0;
	ATTRIBUTE Attribute;
	DWORD dwSearchProcedure = 0, dwNumParameters = 0, dwParametersLen = 0;
	PARAMETER Parameter;

	// The DWORD category variables are used to receive random values and for branching
	// code execution to set up the proper type of test
	DWORD dwRequestCategory = 0, dwRequestSubCategory = 0;

	// The XMATCH_REQUEST_CATEGORY category variable is used to set the specific category type
	// in the CXOnlineAsyncRequest class member
	XMATCH_REQUEST_CATEGORY nFinalRequestCategory = INVALID_REQUEST_CATEGORY;

	dwRequestCategory = GenerateRandomNumber(2);  // Either 0 or 1

	switch(dwRequestCategory)
	{
	// --------------------------------------------
	// Create a session half of the time
	// --------------------------------------------
	case 0:
		dwRequestSubCategory = GenerateRandomNumber(4);
		switch(dwRequestSubCategory)
		{
		// --------------------------------------------
		// Add an integer to an existing session
		case 0:
			dwNumAttributes = 1;
			// Length       = AttributeID   + Size of int att
			dwAttributesLen = GetBufferLenForInt();
			Attribute.dwAttributeID = GLOBALINT_ID1;
			Attribute.UserID = 0;
			Attribute.Value.qwValue = INT1;
			nFinalRequestCategory = CREATE_INTEGER_REQ;
//			xLog(hLog, XLL_INFO, "Adding integer attribute to an existing session");
			break;
		// --------------------------------------------
		// Add a string to an existing session
		case 1:
			dwNumAttributes = 1;
			// Length       = AttributeID   + Len prefix   + Str len         + NULL terminator
			dwAttributesLen = GetBufferLenForString(STRING1);
			Attribute.dwAttributeID = GLOBALSTR_ID1;
			Attribute.UserID = 0;
			Attribute.Value.pvValue = STRING1;
			nFinalRequestCategory = CREATE_STRING_REQ;
//			xLog(hLog, XLL_INFO, "Adding string attribute to an existing session");
			break;
		// --------------------------------------------
		// Add a blob to an existing session
		case 2:
			dwNumAttributes = 1;
			// Length       = AttributeID   + Len prefix    + Blb len
			dwAttributesLen = GetBufferLenForBlob(BLOB1);
			Attribute.dwAttributeID = GLOBALBLB_ID1;
			Attribute.UserID = 0;
			Attribute.Value.pvValue = BLOB1;
			nFinalRequestCategory = CREATE_BLOB_REQ;
//			xLog(hLog, XLL_INFO, "Adding blob attribute to an existing session");
			break;
		// --------------------------------------------
		// Don't add any new attributes to an existing session
		default:
			dwNumAttributes = 0;
			dwAttributesLen = 0;
			nFinalRequestCategory = CREATE_NO_ATTRIB_REQ;
//			xLog(hLog, XLL_INFO, "Updating existing session without adding attributes");
			break;
		}

		// Create the session request with the appropriate attribute buffers
		pFollowUpSession = new CXMatchSessionRequest(SessionID, dwNumAttributes, dwAttributesLen);
		if(!pFollowUpSession)
		{
			xLog(hLog, XLL_WARN, "Not enough memory to allocate new session request");
			return NULL;
		}

		pFollowUpSession->SetAsyncID(GetTickCount());

//		xLog(hLog, XLL_INFO, "Creating follow-up session 0x%08x from 0x%08x", dwPreviousAsyncID, pFollowUpSession->GetAsyncID());

		// Add the attribute if necessary
		if(dwNumAttributes)
		{
			if(!pFollowUpSession->AddAttributes(&Attribute, dwNumAttributes))
			{
				xLog(hLog, XLL_WARN, "Couldn't add attributes to session request");
				delete pFollowUpSession;
				return NULL;
			}
		}

		pFollowUpSession->SetRequestCategory(nFinalRequestCategory);
		if(FAILED(pFollowUpSession->StartRequest()))
		{
			xLog(hLog, XLL_WARN, "Couldn't start request");
			delete pFollowUpSession;
			return NULL;
		}
		pRequest = pFollowUpSession;
		break;

	// --------------------------------------------
	// Create a search the other half of the time
	// --------------------------------------------
	default:
		switch(nPreviousRequestCategory)
		{
		// --------------------------------------------
		// If the previous request was an integer session
		// Then create a search that will return the integer
		case CREATE_INTEGER_REQ:
			nFinalRequestCategory = SEARCH_RETURN_INT_REQ;
			dwSearchProcedure = GETSESSINT_INDEX;
//			xLog(hLog, XLL_INFO, "Searching for session with integer attribute");
			break;
		// --------------------------------------------
		// If the previous request was a string session
		// Then create a search that will return the string
		case CREATE_STRING_REQ:
			nFinalRequestCategory = SEARCH_RETURN_STR_REQ;
			dwSearchProcedure = GETSESSSTR_INDEX;
//			xLog(hLog, XLL_INFO, "Searching for session with string attribute");
			break;
		// --------------------------------------------
		// If the previous request was a blob session
		// Then create a search that will return the blob
		case CREATE_BLOB_REQ:
			nFinalRequestCategory = SEARCH_RETURN_BLB_REQ;
			dwSearchProcedure = GETSESSBLB_INDEX;
//			xLog(hLog, XLL_INFO, "Searching for session with blob attribute");
			break;
		// --------------------------------------------
		// If the previous request was a session without attributes
		// Then create a search that won't return any attributes
		default:
			nFinalRequestCategory = SEARCH_RETURN_NO_PARAM_REQ;
			dwSearchProcedure = GETSESSSP_INDEX;
//			xLog(hLog, XLL_INFO, "Searching for session without any attributes");
			break;
		}

		dwNumParameters = 1;
		// Length       = Param type    + Size of int param
		dwParametersLen = GetBufferLenForInt();
		Parameter.dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
		Parameter.Value.qwValue = *((ULONGLONG *) &SessionID);

		// Create the search request with the appropriate parameter and result buffers
		pFollowUpSearch = new CXMatchSearchRequest(dwSearchProcedure, dwNumParameters, dwParametersLen, DEFAULT_RESULTS_LENGTH);
		if(!pFollowUpSearch)
		{
			xLog(hLog, XLL_WARN, "Not enough memory to allocate new search request");
			return NULL;
		}
		
		pFollowUpSearch->SetAsyncID(GetTickCount());

//		xLog(hLog, XLL_INFO, "Creating follow-up search 0x%08x from 0x%08x", dwPreviousAsyncID, pFollowUpSearch->GetAsyncID());

		// Add the parameter if necessary
		if(dwNumParameters)
		{
			if(!pFollowUpSearch->AddParameters(&Parameter, dwNumParameters))
			{
				xLog(hLog, XLL_WARN, "Couldn't add parameters to search request");
				delete pFollowUpSearch;
				return NULL;
			}
		}
		
		pFollowUpSearch->SetRequestCategory(nFinalRequestCategory);
		if(FAILED(pFollowUpSearch->StartRequest()))
		{
			xLog(hLog, XLL_WARN, "Couldn't start request");
			delete pFollowUpSearch;
			return NULL;
		}
		pRequest = pFollowUpSearch;
		break;
		
		break;
	}
	
	return pRequest;
}

//==================================================================================
// StartNewXmatchRequest
//----------------------------------------------------------------------------------
//
// Description: Creates a new Xmatch request and returns a pointer to it
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	A pointer to the created Xmatch request or NULL if one couldn't be created
//==================================================================================
CXOnlineAsyncRequest *StartNewXmatchRequest(HANDLE hLog)
{
	CXOnlineAsyncRequest *pRequest = NULL;
	CXMatchSessionRequest *pNewSession = NULL;
	CXMatchSearchRequest *pNewSearch = NULL;
	DWORD dwNumAttributes = 0, dwAttributesLen = 0;
	ATTRIBUTE Attribute;
	DWORD dwSearchProcedure = 0, dwNumParameters = 0, dwParametersLen = 0;
	PARAMETER Parameter;
	XNKID SessionID;
	HRESULT hr = S_OK;

	// The DWORD category variables are used to receive random values and for branching
	// code execution to set up the proper type of test
	DWORD dwRequestCategory = 0, dwRequestSubCategory = 0;

	// The XMATCH_REQUEST_CATEGORY category variable is used to set the specific category type
	// in the CXOnlineAsyncRequest class member
	XMATCH_REQUEST_CATEGORY nFinalRequestCategory = INVALID_REQUEST_CATEGORY;

	dwRequestCategory = GenerateRandomNumber(2);  // Either 0 or 1

	switch(dwRequestCategory)
	{
	// --------------------------------------------
	// Create a session half of the time
	// --------------------------------------------
	case 0:
		dwRequestSubCategory = GenerateRandomNumber(4);
		switch(dwRequestSubCategory)
		{
		// --------------------------------------------
		// Create a session with an integer attribute
		case 0:
			dwNumAttributes = 1;
			// Length       = AttributeID   + Size of int att
			dwAttributesLen = GetBufferLenForInt();
			Attribute.dwAttributeID = GLOBALINT_ID1;
			Attribute.UserID = 0;
			Attribute.Value.qwValue = INT1;
			nFinalRequestCategory = CREATE_INTEGER_REQ;
//			xLog(hLog, XLL_INFO, "Started session creation with an int attribute");
			break;
		// --------------------------------------------
		// Create a session with a string attribute
		case 1:
			dwNumAttributes = 1;
			// Length       = AttributeID   + Len prefix   + Str len         + NULL terminator
			dwAttributesLen = GetBufferLenForString(STRING1);
			Attribute.dwAttributeID = GLOBALSTR_ID1;
			Attribute.UserID = 0;
			Attribute.Value.pvValue = STRING1;
			nFinalRequestCategory = CREATE_STRING_REQ;
//			xLog(hLog, XLL_INFO, "Started session creation with a string attribute");
			break;
		// --------------------------------------------
		// Create a session with a blob attribute
		case 2:
			dwNumAttributes = 1;
			// Length       = AttributeID   + Len prefix    + Blb len
			dwAttributesLen = GetBufferLenForBlob(BLOB1);
			Attribute.dwAttributeID = GLOBALBLB_ID1;
			Attribute.UserID = 0;
			Attribute.Value.pvValue = BLOB1;
			nFinalRequestCategory = CREATE_BLOB_REQ;
//			xLog(hLog, XLL_INFO, "Started session creation with a blob attribute");
			break;
		// --------------------------------------------
		// Create a session without any attributes
		default:
			dwNumAttributes = 0;
			dwAttributesLen = 0;
			nFinalRequestCategory = CREATE_NO_ATTRIB_REQ;
//			xLog(hLog, XLL_INFO, "Started session creation without attributes");
			break;
		}

		memset(&SessionID, 0, sizeof(XNKID));

		// Create the session request with the appropriate attribute buffers
		pNewSession = new CXMatchSessionRequest(SessionID, dwNumAttributes, dwAttributesLen);
		if(!pNewSession)
		{
			xLog(hLog, XLL_WARN, "Not enough memory to allocate new session request");
			return NULL;
		}

		pNewSession->SetAsyncID(GetTickCount());

//		xLog(hLog, XLL_INFO, "Creating session 0x%08x", pNewSession->GetAsyncID());

		// Add the attribute if necessary
		if(dwNumAttributes)
		{
			if(!pNewSession->AddAttributes(&Attribute, dwNumAttributes))
			{
				xLog(hLog, XLL_WARN, "Couldn't add attributes to session request");
				delete pNewSession;
				return NULL;
			}
		}

		pNewSession->SetRequestCategory(nFinalRequestCategory);
		if(FAILED(pNewSession->StartRequest()))
		{
			xLog(hLog, XLL_WARN, "Couldn't start request");
			delete pNewSession;
			return NULL;
		}
		pRequest = pNewSession;
		break;

	// --------------------------------------------
	// Create a search the other half of the time
	// --------------------------------------------
	default:
		dwRequestSubCategory = GenerateRandomNumber(4);
		switch(dwRequestSubCategory)
		{
		// --------------------------------------------
		// Create a search with an integer paramter
		case 0:
			dwSearchProcedure = INTPARAM_INDEX;
			dwNumParameters = 1;
			// Length       = Param type    + Size of int param
			dwParametersLen = GetBufferLenForInt();
			Parameter.dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
			Parameter.Value.qwValue = INT1;
			nFinalRequestCategory = SEARCH_INTEGER_REQ;
//			xLog(hLog, XLL_INFO, "Started session search with an integer parameter");
			break;
		// --------------------------------------------
		// Create a search with a string paramter
		case 1:
			dwSearchProcedure = STRINGPARAM_INDEX;
			dwNumParameters = 1;
			// Length       = Param type    + Len prefix   + Str len         + NULL terminator
			dwParametersLen = GetBufferLenForString(STRING1);
			Parameter.dwParameterType = X_ATTRIBUTE_DATATYPE_STRING;
			Parameter.Value.pvValue = STRING1;
			nFinalRequestCategory = SEARCH_STRING_REQ;
//			xLog(hLog, XLL_INFO, "Started session search with a string parameter");
			break;
		// --------------------------------------------
		// Create a search with a blob paramter
		case 2:
			dwSearchProcedure = STRINGPARAM_INDEX;
			dwNumParameters = 1;
			// Length       = Param type    + Len prefix    + Blb len
			dwParametersLen = GetBufferLenForBlob(BLOB1);
			Parameter.dwParameterType = X_ATTRIBUTE_DATATYPE_BLOB;
			Parameter.Value.pvValue = BLOB1;
			nFinalRequestCategory = SEARCH_BLOB_REQ;
//			xLog(hLog, XLL_INFO, "Started session search with a blob parameter");
			break;
		// --------------------------------------------
		// Create a search without any paramters
		default:
			dwSearchProcedure = NOPARAM_INDEX;
			dwNumParameters = 0;
			dwParametersLen = 0;
			nFinalRequestCategory = SEARCH_NO_PARAM_REQ;
//			xLog(hLog, XLL_INFO, "Started session search without any parameters");
			break;
		}

		// Create the search request with the appropriate parameter and result buffers
		pNewSearch = new CXMatchSearchRequest(dwSearchProcedure, dwNumParameters, dwParametersLen, DEFAULT_RESULTS_LENGTH);
		if(!pNewSearch)
		{
			xLog(hLog, XLL_WARN, "Not enough memory to allocate new search request");
			return NULL;
		}
		
		pNewSearch->SetAsyncID(GetTickCount());

//		xLog(hLog, XLL_INFO, "Creating search 0x%08x", pNewSearch->GetAsyncID());

		// Add the parameter if necessary
		if(dwNumParameters)
		{
			if(!pNewSearch->AddParameters(&Parameter, dwNumParameters))
			{
				xLog(hLog, XLL_WARN, "Couldn't add parameters to search request");
				delete pNewSearch;
				return NULL;
			}
		}
		
		pNewSearch->SetRequestCategory(nFinalRequestCategory);
		if(FAILED(pNewSearch->StartRequest()))
		{
			xLog(hLog, XLL_WARN, "Couldn't start request");
			delete pNewSearch;
			return NULL;
		}
		pRequest = pNewSearch;		
		break;
	}
	
	return pRequest;
}

//==================================================================================
// XMatchStressMain
//----------------------------------------------------------------------------------
//
// Description: Function that implements main stress functionality
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID XMatchStressMain(IN HANDLE hLog, DWORD dwTimeToRun)
{
	CXOnlineAsyncRequest *AsyncRequestArray[MAX_SIMULTANEOUS_REQUESTS];
	HRESULT hr = NULL;
	HANDLE AsyncWorkEventArray[MAX_SIMULTANEOUS_REQUESTS];
	DWORD dwRequestsOutstanding = 0, dwWaitResult = 0, dwStartTime = 0, dwElapsedTime = 0, dwRequestBeingProcessed = 0;
	
	// Set up all the initial requests
	while(dwRequestsOutstanding < MAX_SIMULTANEOUS_REQUESTS)
	{
		if(!(AsyncRequestArray[dwRequestsOutstanding] = StartNewXmatchRequest(hLog)))
		{
			goto CloseOutstandingRequests;
		}
		
		AsyncWorkEventArray[dwRequestsOutstanding] = AsyncRequestArray[dwRequestsOutstanding]->GetWorkEvent();
		++dwRequestsOutstanding;
	}
	
	// Start keeping track of elapsed time
	dwStartTime = GetTickCount();
	dwElapsedTime = 0;
	
	// Wait for work events to signal
	dwWaitResult = WaitForMultipleObjects(dwRequestsOutstanding, AsyncWorkEventArray, FALSE, dwTimeToRun);

	while((dwWaitResult >= WAIT_OBJECT_0) &&
		(dwWaitResult <= WAIT_OBJECT_0 + dwRequestsOutstanding) &&
		(dwElapsedTime < dwTimeToRun))
	{
		dwRequestBeingProcessed = dwWaitResult - WAIT_OBJECT_0;
		
		// If this request has completed, then shift down the rest of the requests in the array
		// and create a new request
		hr = AsyncRequestArray[dwRequestBeingProcessed]->DoWork(0);

		if(FAILED(hr))
		{
			// If pending, then the item isn't complete yet
			if(hr != E_PENDING)
			{
				xLog(hLog, XLL_INFO, "DoWork failed with 0x%08x", hr);
			}
		}
		else
		{
			XMATCH_REQUEST_CATEGORY nRequestCategory = INVALID_REQUEST_CATEGORY;
			XNKID SessionID;
			DWORD dwRemainingRequests = MAX_SIMULTANEOUS_REQUESTS - dwRequestBeingProcessed - 1, dwFollowUp = 0;
			DWORD dwPreviousAsyncID = 0;

			memset(&SessionID, 0, sizeof(XNKID));

			// In order to do follow up requsts, we need to get the session ID if it exists
			// for this type of request
			nRequestCategory = (XMATCH_REQUEST_CATEGORY) AsyncRequestArray[dwRequestBeingProcessed]->GetRequestCategory();
			switch(nRequestCategory)
			{
			case CREATE_NO_ATTRIB_REQ:
			case CREATE_INTEGER_REQ:
			case CREATE_STRING_REQ:
			case CREATE_BLOB_REQ:
				((CXMatchSessionRequest *) AsyncRequestArray[dwRequestBeingProcessed])->GetSessionID(&SessionID);
				break;
			default:
				break;
			}
			
			dwPreviousAsyncID = AsyncRequestArray[dwRequestBeingProcessed]->GetAsyncID();

			// Close the old request
			hr = AsyncRequestArray[dwRequestBeingProcessed]->Close();
			if(FAILED(hr))
			{
				xLog(hLog, XLL_WARN, "Close failed with 0x%08x", hr);
				// BUGBUG - handle a failure in a Close call
			}

			delete AsyncRequestArray[dwRequestBeingProcessed];
			
			--dwRequestsOutstanding;
			
			// Shuffle the remaining requests in the list down to occupy the vacant space
			ShuffleRemainingArrayItems(AsyncRequestArray + dwRequestBeingProcessed, dwRemainingRequests);
			ShuffleRemainingArrayItems(AsyncWorkEventArray + dwRequestBeingProcessed, dwRemainingRequests);
			
			// Create a new request which can either be a follow up request to the previous one
			// or can be a completely new request
			
			// Determine if new request or a follow up
			dwFollowUp = GenerateRandomNumber(2);  // Either 0 or 1
			
			// If we decided not to do a follow up request or we don't have a session ID to
			// make a follow up request with, then start a new request
			if(!dwFollowUp || *((ULONGLONG *) &SessionID))
			{
				if(!(AsyncRequestArray[MAX_SIMULTANEOUS_REQUESTS - 1] = StartNewXmatchRequest(hLog)))
				{
					goto CloseOutstandingRequests;
				}
			}
			// Otherwise, we must have decided to make a follow up request and we must have a vaild
			// session ID to make a follow up request with
			else
			{
				if(!(AsyncRequestArray[MAX_SIMULTANEOUS_REQUESTS - 1] =
					StartFollowUpXmatchRequest(hLog, nRequestCategory, SessionID, dwPreviousAsyncID)))
				{
					goto CloseOutstandingRequests;
				}
			}
			
			AsyncWorkEventArray[MAX_SIMULTANEOUS_REQUESTS - 1] = AsyncRequestArray[dwRequestsOutstanding]->GetWorkEvent();
			++dwRequestsOutstanding;
			
			// BUGBUG - Randomly cancel events			
		}

		// Determine the ammount of time that has elapsed
		dwElapsedTime = GetElapsedTime(dwStartTime, GetTickCount());
		if(dwElapsedTime > dwTimeToRun)
			break;
		
		// Wait for work events to signal
		dwWaitResult = WaitForMultipleObjects(dwRequestsOutstanding, AsyncWorkEventArray, FALSE, dwTimeToRun - dwElapsedTime);
	}

CloseOutstandingRequests:

	while(dwRequestsOutstanding > 0)
	{
		--dwRequestsOutstanding;

		// Cancel the task
		hr = AsyncRequestArray[dwRequestsOutstanding]->Cancel();
		if(FAILED(hr))
		{
			xLog(hLog, XLL_WARN, "Couldn't cancel outstanding request");
			AsyncRequestArray[dwRequestsOutstanding]->Close();
			continue;
		}

		// Poll until the task is complete
		while(AsyncRequestArray[dwRequestsOutstanding]->GetCurrentState() != STATE_COMPLETE)
		{
			hr = AsyncRequestArray[dwRequestsOutstanding]->DoWork(0);
			if(FAILED(hr))
			{
				// BUGBUG - Handle errors from DoWork, E_PENDING is acceptable
			}
		}

		// Close the task
		hr = AsyncRequestArray[dwRequestsOutstanding]->Close();
		if(FAILED(hr))
		{
			xLog(hLog, XLL_WARN, "Couldn't close outstanding request");
			continue;
		}

		delete AsyncRequestArray[dwRequestsOutstanding];

		AsyncRequestArray[dwRequestsOutstanding] = NULL;
		AsyncWorkEventArray[dwRequestsOutstanding] = NULL;
	}

}



//==================================================================================
// XMatchStressStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI XMatchStressStartTest(IN HANDLE  hLog)
{
	HRESULT hr = S_OK;
	HANDLE hPingObject = NULL, hEnumObject = NULL, hClientObject = NULL;
	INT nRet = 0, i;

	XONLINE_SERVICE ServicesArray[NUM_SERVICES] =
	{
		{ XONLINE_MATCHMAKING_SERVICE, S_OK, NULL }
	};

	g_hLog = hLog;

    // Set XLog info
	xSetOwnerAlias(g_hLog, "tristanj");
    xSetComponent(g_hLog, "Online(S)", "Matching - Client (S-o)");
	xSetFunctionName(g_hLog, "<none>");

	hr = XOnlineLogonWrapper(hLog, ServicesArray, NUM_SERVICES);
    if (FAILED(hr))
	{
		xLog(g_hLog, XLL_FAIL, "Failed to initialize services");
		return;
	}

	XMatchStressMain(hLog, 1800000000);

	g_hLog = INVALID_HANDLE_VALUE;
}            

//==================================================================================
// XMatchStressEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI XMatchStressEndTest()
{
	// We already cleaned up everything at the end of StartTest
}

} // namespace XMatchStressNamespace

// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( xmstress )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( xmstress )
    EXPORT_TABLE_ENTRY( "StartTest", XMatchStressStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XMatchStressEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", XMatchStressDllMain )
END_EXPORT_TABLE( xmstress )
