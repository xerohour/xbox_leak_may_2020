//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_AppendParams(HANDLE hLog);

//==================================================================================
// XMatchTest_AppendParams
//----------------------------------------------------------------------------------
//
// Description: Contains test cases for appending parameters to search requests via
//   the Xbox Matchmaking APIs
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT XMatchTest_AppendParams(HANDLE hLog, XONLINETASK_HANDLE hLogon)
{
	PXMATCH_SEARCHRESULT pSearchResult = NULL;
	XONLINETASK_HANDLE hSearch = NULL;
	XNKID SessionID;
	ATTRIBUTE AttributeTest[4];
	HRESULT hr = E_FAIL;
    HANDLE hEvent = NULL;
    DWORD dwPollCounter = 0, dwResultCounter = 0;
	BOOL fCompleted = FALSE, fRet = FALSE, fDone = FALSE;

	memset(&SessionID, 0, sizeof(SessionID));

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
		goto Exit;

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Append integer parameter with NULL task handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchAppendInt(NULL, INT1);
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSearchAppendInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendInt failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendInt returned success but should have failed!");
	}

	END_TESTCASE();
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Append integer when no parameter slots are available");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSearchCreate(INTPARAM_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	hr = XOnlineMatchSearchAppendInt(hSearch, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchAppendInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendInt failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendInt returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Append integer when there isn't enough space available");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSearchCreate(INTPARAM_INDEX, 10, 1, 3, sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}
	
	hr = XOnlineMatchSearchAppendInt(hSearch, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchAppendInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendInt failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendInt returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);



	// -----------------------------------------------------------------
	START_TESTCASE("Append an integer attribute when there is exactly enough space");
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

	hr = XOnlineMatchSearchCreate(INTPARAM_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned success!");
	
	hr = XOnlineMatchSearchAppendInt(hSearch, INT1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Couldn't add integer to search request!");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if (FAILED(hr))
	{
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hSearch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Search completed successfully");

	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);



/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to append integer to a search that has already been sent");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(INTPARAM_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if (FAILED(hr))
	{
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
	}

	hr = XOnlineMatchSearchAppendInt(hSearch, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchAppendInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendInt failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendInt returned success but should have failed!");
	}
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Append string parameter with NULL task handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchAppendString(NULL, STRING1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchAppendString returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendString failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendString returned success but should have failed!");
	}

	END_TESTCASE();
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Append string when no parameter slots are available");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSearchCreate(STRINGPARAM_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	hr = XOnlineMatchSearchAppendString(hSearch, STRING1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchAppendString returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendString failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendString returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Append string when there isn't enough space available");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSearchCreate(STRINGPARAM_INDEX, 10, 1, GetBufferLenForString(STRING1) - 1, sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	hr = XOnlineMatchSearchAppendString(hSearch, STRING1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchAppendString returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendString failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendString returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);



	// -----------------------------------------------------------------
	START_TESTCASE("Append an string attribute when there is exactly enough space");
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

	hr = XOnlineMatchSearchCreate(STRINGPARAM_INDEX, 10, 1, GetBufferLenForString(STRING1), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned success!");
	
	hr = XOnlineMatchSearchAppendString(hSearch, STRING1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Couldn't add string to search request!");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if (FAILED(hr))
	{
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hSearch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Search completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);
	


	// -----------------------------------------------------------------
	START_TESTCASE("Append an empty string parameter");
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

	hr = XOnlineMatchSearchCreate(STRINGPARAM_INDEX, 10, 1, GetBufferLenForString(L""), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	hr = XOnlineMatchSearchAppendString(hSearch, L"");
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Couldn't add empty string to search request!");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if (FAILED(hr))
	{
		hSearch = NULL;
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

	PASS_TESTCASE("Search completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);



/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to append string to a search that has already been sent");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(STRINGPARAM_INDEX, 10, 1, GetBufferLenForString(STRING1), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}
	
	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if (FAILED(hr))
	{
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
	}

	hr = XOnlineMatchSearchAppendString(hSearch, STRING1);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendString failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendString returned success but should have failed!");
	}
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/	

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Append blob parameter with NULL task handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchAppendBlob(NULL, strlen(BLOB1), BLOB1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchAppendBlob returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendBlob failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendBlob returned success but should have failed!");
	}

	END_TESTCASE();
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Append blob when no parameter slots are available");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSearchCreate(BLOBPARAM_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
	}

	hr = XOnlineMatchSearchAppendBlob(hSearch, strlen(BLOB1), BLOB1);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendBlob failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendBlob returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Append blob when there isn't enough space available");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSearchCreate(BLOBPARAM_INDEX, 10, 1, GetBufferLenForBlob(BLOB1) - 1, sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
	}

	hr = XOnlineMatchSearchAppendBlob(hSearch, strlen(BLOB1), BLOB1);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendBlob failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendBlob returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hSearch);



	// -----------------------------------------------------------------
	START_TESTCASE("Append a blob attribute when there is exactly enough space");
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

	hr = XOnlineMatchSearchCreate(BLOBPARAM_INDEX, 10, 1, GetBufferLenForBlob(BLOB1), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	
	hr = XOnlineMatchSearchAppendBlob(hSearch, strlen(BLOB1), BLOB1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Couldn't add string to search request!");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if (FAILED(hr))
	{
		hSearch = NULL;
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

	PASS_TESTCASE("Search completed successfully");
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
	


	// -----------------------------------------------------------------
	START_TESTCASE("Append an empty blob");
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

	hr = XOnlineMatchSearchCreate(BLOBPARAM_INDEX, 10, 1, GetBufferLenForBlob(""), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}
	
	hr = XOnlineMatchSearchAppendBlob(hSearch, 0, "");
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Couldn't add string to search request!");
	}

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if (FAILED(hr))
	{
		hSearch = NULL;
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

	hSearch = NULL;
	PASS_TESTCASE("Search completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to append blob to a search that has already been sent");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(BLOBPARAM_INDEX, 10, 1, GetBufferLenForBlob(BLOB1), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned success!");
	
	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if (FAILED(hr))
	{
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSearchSend failed unexpectedly");
	}

	hr = XOnlineMatchSearchAppendBlob(hSearch, strlen(BLOB1), BLOB1);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchAppendBlob failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchAppendBlob returned success but should have failed!");
	}
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/
Exit:
	if(hEvent)
		CloseHandle(hEvent);

	if(!fCompleted && hSearch)
		XOnlineTaskClose(hSearch);

	return hr;
} // XMatchTest_AppendParams


} // namespace XMatchTestNamespace