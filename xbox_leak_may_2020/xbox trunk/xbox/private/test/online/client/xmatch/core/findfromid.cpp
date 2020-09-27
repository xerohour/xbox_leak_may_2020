//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_FindSessionFromID(HANDLE hLog);

//==================================================================================
// XMatchTest_FindSessionFromID
//----------------------------------------------------------------------------------
//
// Description: Contains test cases for creating searches for a specific session ID
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT XMatchTest_FindSessionFromID(HANDLE hLog, XONLINETASK_HANDLE hLogon)
{
	PXMATCH_SEARCHRESULT *ppSearchResults = NULL;
	XONLINETASK_HANDLE hSearch = NULL;
	XNADDR LocalXnAddr;
	XNKID SessionID;
	HRESULT hr = E_FAIL;
    HANDLE hEvent = NULL;
    DWORD dwPollCounter = 0, dwAddrStatus = 0, dwNumResults = 0;
	BOOL fCompleted = FALSE, fDone = FALSE;

	memset(&SessionID, 0, sizeof(SessionID));

	dwAddrStatus = XNetGetTitleXnAddr(&LocalXnAddr);
	if(dwAddrStatus == XNET_GET_XNADDR_PENDING)
		xLog(hLog, XLL_FAIL, "Local address still pending, some tests will fail.");
	else if(dwAddrStatus & XNET_GET_XNADDR_NONE)
		xLog(hLog, XLL_FAIL, "Local address not available, some tests will fail.");

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
		goto Exit;

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Find session from ID with NULL task handle");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSessionFindFromID(SessionID, hEvent, NULL);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionFindFromID returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionFindFromID failed as expected");
	}
	else
	{
		XOnlineTaskClose(hSearch);
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSessionFindFromID returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Find session with session ID of 0");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionFindFromID(SessionID, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionFindFromID returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionFindFromID failed as expected");
	}
	else
	{
		XOnlineTaskClose(hSearch);
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSessionFindFromID returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Find session from ID with a NULL work handle");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSessionFindFromID(SessionID, NULL, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionFindFromID returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionFindFromID failed unexpectedly");
	}
	
	hr = PollTaskUntilComplete(hSearch, NULL, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSearchGetResults(hSearch, &ppSearchResults, &dwNumResults);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchGetResults returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetResults failed unexpectedly");
	}
	
	if(dwNumResults != 1)
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetResults succeeded but didn't return expected number of results");
	}

	xLog(hLog, XLL_INFO, "Successfully retrieved first search result!");
	xLog(hLog, XLL_INFO, "ResultLen: %u; NumAttr: %u", 
		(ppSearchResults[0])->dwResultLength, (ppSearchResults[0])->dwNumAttributes);

	if(memcmp(&((ppSearchResults[0])->SessionID), &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("Search result returned unexpected session ID!");
	}

	if(memcmp(&((ppSearchResults[0])->HostAddress), &LocalXnAddr, sizeof(XNADDR)))
	{
		FAIL_TESTCASE("Returned host address differs from local address");
	}

	PASS_TESTCASE("Search completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);



	// -----------------------------------------------------------------
	START_TESTCASE("Find session from ID with a valid work handle");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSessionFindFromID(SessionID, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionFindFromID returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionFindFromID failed unexpectedly");
	}
	
	hr = PollTaskUntilComplete(hSearch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSearchGetResults(hSearch, &ppSearchResults, &dwNumResults);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchGetResults returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetResults failed unexpectedly");
	}
	
	if(dwNumResults != 1)
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetResults succeeded but didn't return expected number of results");
	}

	xLog(hLog, XLL_INFO, "Successfully retrieved first search result!");
	xLog(hLog, XLL_INFO, "ResultLen: %u; NumAttr: %u", 
		(ppSearchResults[0])->dwResultLength, (ppSearchResults[0])->dwNumAttributes);

	if(memcmp(&((ppSearchResults[0])->SessionID), &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("Search result returned unexpected session ID!");
	}

	if(memcmp(&((ppSearchResults[0])->HostAddress), &LocalXnAddr, sizeof(XNADDR)))
	{
		FAIL_TESTCASE("Returned host address differs from local address");
	}

	PASS_TESTCASE("Search completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);



	// -----------------------------------------------------------------
	START_TESTCASE("Try to find a session that doesn't exist");
	// -----------------------------------------------------------------

	*((ULONGLONG *) &SessionID) = 0x8000000000000000;
	hr = XOnlineMatchSessionFindFromID(SessionID, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionFindFromID returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionFindFromID failed unexpectedly");
	}
	
	hr = PollTaskUntilComplete(hSearch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSearchGetResults(hSearch, &ppSearchResults, &dwNumResults);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchGetResults returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetResults failed unexpectedly");
	}
	
	if(dwNumResults)
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetResults succeeded but didn't return expected number of results");
	}

	xLog(hLog, XLL_INFO, "Successfully indicated that session doesn't exist!");

	PASS_TESTCASE("Search completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);



	// -----------------------------------------------------------------
	START_TESTCASE("Find session from ID that doesn't have any pub/priv available");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(0, 0, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSessionFindFromID(SessionID, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionFindFromID returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionFindFromID failed unexpectedly");
	}
	
	hr = PollTaskUntilComplete(hSearch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSearchGetResults(hSearch, &ppSearchResults, &dwNumResults);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchGetResults returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetResults failed unexpectedly");
	}
	
	if(dwNumResults != 1)
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetResults succeeded but didn't return expected number of results");
	}

	xLog(hLog, XLL_INFO, "Successfully retrieved first search result!");
	xLog(hLog, XLL_INFO, "ResultLen: %u; NumAttr: %u", 
		(ppSearchResults[0])->dwResultLength, (ppSearchResults[0])->dwNumAttributes);

	if(memcmp(&((ppSearchResults[0])->SessionID), &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("Search result returned unexpected session ID!");
	}

	if(memcmp(&((ppSearchResults[0])->HostAddress), &LocalXnAddr, sizeof(XNADDR)))
	{
		FAIL_TESTCASE("Returned host address differs from local address");
	}

	PASS_TESTCASE("Search completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);



	// -----------------------------------------------------------------
	START_TESTCASE("Open and close a find-session task without calling work pump");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionFindFromID(SessionID, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionFindFromID returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionFindFromID failed unexpectedly");
	}

	XOnlineTaskClose(hSearch);
	hSearch = NULL;

	PASS_TESTCASE("XOnlineTaskClose completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);	



	// -----------------------------------------------------------------
	START_TESTCASE("Cancel a find-session task that has already completed and then close it");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSessionFindFromID(SessionID, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionFindFromID returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionFindFromID failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hSearch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}
	
	XOnlineTaskClose(hSearch);
	hSearch = NULL;
	
	PASS_TESTCASE("XOnlineTaskClose completed successfully");
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);

Exit:
	if(hEvent)
		CloseHandle(hEvent);

	if(!fCompleted && hSearch)
		XOnlineTaskClose(hSearch);

	return hr;
} // XMatchTest_FindSessionFromID


} // namespace XMatchTestNamespace