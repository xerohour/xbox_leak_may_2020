//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_SearchResults(HANDLE hLog);

//==================================================================================
// XMatchTest_SearchResults
//----------------------------------------------------------------------------------
//
// Description: Contains test cases for sending searches via Xbox Matchmaking APIs
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT XMatchTest_SearchResults(HANDLE hLog, XONLINETASK_HANDLE hLogon)
{
	PXMATCH_SEARCHRESULT pSearchResult = NULL;
	XONLINETASK_HANDLE hSearch = NULL;
	XNKID SessionID, NewSessionID;
	ATTRIBUTE AttributeTest[4];
	PARAMETER ParameterTest[4];
	HRESULT hr = E_FAIL;
    HANDLE hEvent = NULL;
    DWORD dwPollCounter = 0;
	BOOL fCompleted = FALSE, fRet = FALSE, fDone = FALSE;

	memset(&SessionID, 0, sizeof(SessionID));
	memset(&NewSessionID, 0, sizeof(NewSessionID));

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
		goto Exit;

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get first result with NULL search handle");
	// -----------------------------------------------------------------

	hr = XMatchGetFirstSearchResult(NULL, &pSearchResult);

	xLog(hLog, XLL_INFO, "XMatchGetFirstSearchResult returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XMatchGetFirstSearchResult failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XMatchGetFirstSearchResult returned success but should have failed!");
	}

	END_TESTCASE();
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get next result with NULL search handle");
	// -----------------------------------------------------------------

	hr = XMatchGetNextSearchResult(NULL, &pSearchResult);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XMatchGetFirstSearchResult failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XMatchGetFirstSearchResult returned success but should have failed!");
	}

	END_TESTCASE();
*/
/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get first result with NULL search result pointer");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, AttributeTest, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSearchCreate(GETSESSSP_INDEX, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[0].Value.qwValue = *((ULONGLONG *) &SessionID);
	
	fRet = AddParameters(hSearch, ParameterTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add parameters to search!");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
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

	hr = XMatchGetFirstSearchResult(hSearch, NULL);

	xLog(hLog, XLL_INFO, "XMatchGetFirstSearchResult returned 0x%08x", hr);
	if (FAILED(hr))
	{
		PASS_TESTCASE("XMatchGetFirstSearchResult failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XMatchGetFirstSearchResult returned success but should have failed!");
	}
		
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get next result with NULL search result pointer");
	// -----------------------------------------------------------------
	
	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, AttributeTest, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSearchCreate(GETSESSSP_INDEX, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}
	
	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[0].Value.qwValue = *((ULONGLONG *) &SessionID);
	
	fRet = AddParameters(hSearch, ParameterTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add parameters to search!");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
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

	hr = XMatchGetFirstSearchResult(hSearch, &pSearchResult);

	xLog(hLog, XLL_INFO, "XMatchGetFirstSearchResult returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XMatchGetFirstSearchResult failed unexpectedly");
	}

	hr = XMatchGetNextSearchResult(hSearch, NULL);

	xLog(hLog, XLL_INFO, "XMatchGetNextSearchResult returned 0x%08x", hr);
	if (FAILED(hr))
	{
		PASS_TESTCASE("XMatchGetNextSearchResult failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XMatchGetNextSearchResult returned success but should have failed!");
	}
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get first result before sending search");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(NOPARAM_INDEX, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}


	pSearchResult = NULL;
	hr = XMatchGetFirstSearchResult(hSearch, &pSearchResult);

	xLog(hLog, XLL_INFO, "XMatchGetFirstSearchResult returned 0x%08x", hr);
	if (FAILED(hr))
	{
		PASS_TESTCASE("XMatchGetFirstSearchResult failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XMatchGetFirstSearchResult returned success but should have failed!");
	}
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get next result before sending search");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(NOPARAM_INDEX, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}
	
	pSearchResult = NULL;
	hr = XMatchGetNextSearchResult(hSearch, &pSearchResult);

	xLog(hLog, XLL_INFO, "XMatchGetNextSearchResult returned 0x%08x", hr);
	if (FAILED(hr))
	{
		PASS_TESTCASE("XMatchGetNextSearchResult failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XMatchGetNextSearchResult returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get first result before search completes");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSearchCreate(NOPARAM_INDEX, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	hr = XOnlineMatchSearchSend(hSearch);
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
	}

	pSearchResult = NULL;
	hr = XMatchGetFirstSearchResult(hSearch, &pSearchResult);

	xLog(hLog, XLL_INFO, "XMatchGetFirstSearchResult returned 0x%08x", hr);
	if (FAILED(hr))
	{
		PASS_TESTCASE("XMatchGetFirstSearchResult failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XMatchGetFirstSearchResult returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get next result before search completes");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSearchCreate(NOPARAM_INDEX, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
	}

	pSearchResult = NULL;
	hr = XMatchGetNextSearchResult(hSearch, &pSearchResult);

	xLog(hLog, XLL_INFO, "XMatchGetNextSearchResult returned 0x%08x", hr);
	if (FAILED(hr))
	{
		PASS_TESTCASE("XMatchGetNextSearchResult failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XMatchGetNextSearchResult returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get next result without getting first result");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, AttributeTest, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSearchCreate(GETSESSSP_INDEX, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[0].Value.qwValue = *((ULONGLONG *) &SessionID);
	
	fRet = AddParameters(hSearch, ParameterTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add parameters to search!");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
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

	pSearchResult = NULL;
	hr = XMatchGetNextSearchResult(hSearch, &pSearchResult);

	xLog(hLog, XLL_INFO, "XMatchGetNextSearchResult returned 0x%08x", hr);
	if (FAILED(hr))
	{
		PASS_TESTCASE("XMatchGetNextSearchResult failed as expected");
	}
	else if(hr == S_OK)
	{
		PASS_TESTCASE("XMatchGetNextSearchResult even though first result hadn't been retrieved, acceptable");
	}
	else
	{
		FAIL_TESTCASE("XMatchGetNextSearchResult returned success but should have failed!");
	}
		
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/

	// -----------------------------------------------------------------
	START_TESTCASE("Loop through search results twice");
	// -----------------------------------------------------------------
	
	AttributeTest[0].dwAttributeID = GLOBALINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[1].Value.pvValue = (VOID *) STRING1;
	AttributeTest[1].qwUserID = 0;
	AttributeTest[2].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[2].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[2].qwUserID = 0;
	
	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, AttributeTest, 3, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, AttributeTest, 3, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSearchCreate(GETTITLEALL_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 2000, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
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

	if(ReadSearchResults(hLog, hSearch, 2, TRUE))
		ReadSearchResults(hLog, hSearch, 2, TRUE);
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);

Exit:
	if(hEvent)
		CloseHandle(hEvent);

	if(!fCompleted && hSearch)
		XOnlineTaskClose(hSearch);

	return hr;
} // XMatchTest_SearchResults


} // namespace XMatchTestNamespace