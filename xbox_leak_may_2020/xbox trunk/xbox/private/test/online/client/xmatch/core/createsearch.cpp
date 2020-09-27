//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_CreateSearch(HANDLE hLog);

//==================================================================================
// XMatchTest_CreateSearch
//----------------------------------------------------------------------------------
//
// Description: Contains test cases for creating searches via Xbox Matchmaking APIs
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT XMatchTest_CreateSearch(HANDLE hLog, XONLINETASK_HANDLE hLogon)
{
	PXMATCH_SEARCHRESULT *ppSearchResults = NULL;
	XONLINETASK_HANDLE hSearch = NULL;
	XNADDR LocalXnAddr;
// TEMP Need to add test cases for key exchange key comparisons
//	XNKEY LocalXnKey;
	XNKID SessionID;
	BYTE BigBlob[800], bAttributeBuffer[100];
	ATTRIBUTE AttributeTest[4];
	PARAMETER ParameterTest[4];
	HRESULT hr = E_FAIL;
    HANDLE hEvent = NULL;
    DWORD dwPollCounter = 0, dwResultCounter = 0, dwAddrStatus = 0, dwNumResults = 0, dwAttributeType = 0;
	DWORD dwAttributeLen = 0;
	BOOL fCompleted = FALSE, fRet = FALSE, fDone = FALSE;

	memset(BigBlob, 'a', 799);
	BigBlob[799] = 0;

	memset(&SessionID, 0, sizeof(XNKID));
// TEMP Need to add test cases for key exchange key comparisons
//	memset(&LocalXnKey, 0, sizeof(XNKEY));

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
	START_TESTCASE("Create a search with a NULL task pointer");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(INVALIDSP_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 100, hEvent, NULL);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchCreate failed as expected");
	}
	else
	{
		XOnlineTaskClose(hSearch);
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSearchCreate returned success but should have failed!");
	}

	END_TESTCASE();
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Create search with non-zero parameters but zero parameters length");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(INVALIDSP_INDEX, 10, 1, 0, sizeof(XMATCH_SEARCHRESULT) + 100, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchCreate failed as expected");
	}
	else
	{
		XOnlineTaskClose(hSearch);
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSearchCreate returned success but should have failed!");
	}

	END_TESTCASE();
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Create search with zero parameters but non-zero parameters length");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(INVALIDSP_INDEX, 10, 0, 100, sizeof(XMATCH_SEARCHRESULT) + 100, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchCreate failed as expected");
	}
	else
	{
		XOnlineTaskClose(hSearch);
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSearchCreate returned success but should have failed!");
	}

	END_TESTCASE();
*/

	// -----------------------------------------------------------------
	START_TESTCASE("Create search with zero results allowed");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(NOPARAM_INDEX, 0, 1, 100, sizeof(XMATCH_SEARCHRESULT) + 100, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchCreate failed as expected");
	}
	else
	{
		XOnlineTaskClose(hSearch);
		hSearch = NULL;
		FAIL_TESTCASE("XOnlineMatchSearchCreate returned success but should have failed!");
	}

	END_TESTCASE();


	// -----------------------------------------------------------------
	START_TESTCASE("Create a search with a NULL work handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(NOPARAM_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 100, NULL, &hSearch);

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

	hr = PollTaskUntilComplete(hSearch, NULL, 30000, &dwPollCounter, &fCompleted, FALSE);
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
	START_TESTCASE("Create a search with an invalid stored procedure ID and no parameters");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(INVALIDSP_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 100, hEvent, &hSearch);

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

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete returned 0x%08x", hr);
	if(!FAILED(hr))
	{
		FAIL_TESTCASE("Search succeeded but should have failed!");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Search failed as expected");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a search with an invalid stored procedure ID and each kind of parameter");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSearchCreate(INVALIDSP_INDEX, 10, 3, 1000, sizeof(XMATCH_SEARCHRESULT) + 100, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}
	
	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[0].Value.qwValue = INT1;
	ParameterTest[1].dwParameterType = X_ATTRIBUTE_DATATYPE_STRING;
	ParameterTest[1].Value.pvValue = (VOID *) STRING1;
	ParameterTest[2].dwParameterType = X_ATTRIBUTE_DATATYPE_BLOB;
	ParameterTest[2].Value.pvValue = (VOID *) BLOB1;
	
	fRet = AddParameters(hSearch, ParameterTest, 3);
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

	hr = PollTaskUntilComplete(hSearch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete  returned 0x%08x", hr);
	if(!FAILED(hr))
	{
		FAIL_TESTCASE("Search succeeded but should have failed!");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Search failed as expected");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a search with a stored procedure that doesn't have exec permissions");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(NOEXECPERM_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 100, hEvent, &hSearch);

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

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete  returned 0x%08x", hr);
	if(!FAILED(hr))
	{
		FAIL_TESTCASE("Search succeeded but should have failed!");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Search failed as expected");
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a search where the results length is too small for a single result");
	// -----------------------------------------------------------------
	
	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSearchCreate(GETSESSSP_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) - 1, hEvent, &hSearch);

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

	hr = PollTaskUntilComplete(hSearch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(!FAILED(hr))
	{
		PASS_TESTCASE("Search succeeded even though results length specified was too small, acceptible.");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Search failed as expected");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);

/* X_SSINDEX_GET_SESSION isn't publically exposed
	// -----------------------------------------------------------------
	START_TESTCASE("Create a search using X_SSINDEX_GET_SESSION index");
	// -----------------------------------------------------------------
	
	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSearchCreate(X_SSINDEX_GET_SESSION, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);
	
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

	if(memcmp(&((ppSearchResults[0])->SessionID), &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("Search result returned unexpected session ID!");
	}

	if(memcmp(&((ppSearchResults[0])->HostAddress), &LocalXnAddr, sizeof(XNADDR)))
	{
		FAIL_TESTCASE("Returned host address differs from local address");
	}
	
	PASS_TESTCASE("XOnlineMatchSearchGetResults returned expected result buffer");
	xLog(hLog, XLL_INFO, "ResultLen: %u; NumAttr: %u", 
		(ppSearchResults[0])->dwResultLength, (ppSearchResults[0])->dwNumAttributes);
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Create a search where the results contain all types of attributes");
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

	hr = XOnlineMatchSearchCreate(GETSESSALL_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

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

	if(memcmp(&((ppSearchResults[0])->SessionID), &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("Search result returned unexpected session ID!");
	}

	if(memcmp(&((ppSearchResults[0])->HostAddress), &LocalXnAddr, sizeof(XNADDR)))
	{
		FAIL_TESTCASE("Returned host address differs from local address");
	}

	xLog(hLog, XLL_INFO, "Getting integer attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 0, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchGetAttribute returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute failed unexpectedly");
	}

	if(dwAttributeType != X_ATTRIBUTE_DATATYPE_INTEGER)
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute returned unexpected attribute");
	}

	if(dwAttributeLen != sizeof(ULONGLONG))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute returned unexpected attribute length");
	}

	if(*((ULONGLONG *) bAttributeBuffer) != (ULONGLONG) INT1)
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute returned unexpected attribute value");
	}

	xLog(hLog, XLL_INFO, "Getting string attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 1, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchGetAttribute returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute failed unexpectedly");
	}

	if(dwAttributeType != X_ATTRIBUTE_DATATYPE_STRING)
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute returned unexpected attribute");
	}

	xLog(hLog, XLL_INFO, "String attribute size reported as %u", dwAttributeLen);
	if(dwAttributeLen != (wcslen(STRING1) * 2 + 1))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute returned unexpected attribute length (possibly bug 8676)");
	}

	if(wcscmp(STRING1, (WCHAR *) bAttributeBuffer))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute returned unexpected attribute value");
	}

	xLog(hLog, XLL_INFO, "Getting binary attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 2, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchGetAttribute returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute failed unexpectedly");
	}

	if(dwAttributeType != X_ATTRIBUTE_DATATYPE_BLOB)
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute returned unexpected attribute");
	}

	xLog(hLog, XLL_INFO, "Blob attribute size reported as %u", dwAttributeLen);
	if(dwAttributeLen != strlen(BLOB1))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute returned unexpected attribute length");
	}

	if(memcmp(BLOB1, bAttributeBuffer, dwAttributeLen))
	{
		FAIL_TESTCASE("XOnlineMatchSearchGetAttribute returned unexpected attribute value");
	}


	PASS_TESTCASE("XOnlineMatchSearchGetResults returned expected result buffer");
	xLog(hLog, XLL_INFO, "ResultLen: %u; NumAttr: %u", 
		(ppSearchResults[0])->dwResultLength, (ppSearchResults[0])->dwNumAttributes);

	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
	


	// -----------------------------------------------------------------
	START_TESTCASE("Call XMatchGetNextSearchResult when only one result returned");
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

	hr = XOnlineMatchSearchCreate(GETSESSALL_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);
	
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

	if(memcmp(&((ppSearchResults[0])->SessionID), &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("Search result returned unexpected session ID!");
	}

	if(memcmp(&((ppSearchResults[0])->HostAddress), &LocalXnAddr, sizeof(XNADDR)))
	{
		FAIL_TESTCASE("Returned host address differs from local address");
	}

	xLog(hLog, XLL_INFO, "Successfully retrieved first search result!");
	xLog(hLog, XLL_INFO, "ResultLen: %u; NumAttr: %u", 
		(ppSearchResults[0])->dwResultLength, (ppSearchResults[0])->dwNumAttributes);
		
	ReadSearchResults(hLog, hSearch, 1, FALSE);
		
	END_TESTCASE();
	CLEANUP_TASK(hSearch);



	// -----------------------------------------------------------------
	START_TESTCASE("Create a search where multiple results are returned");
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

	ReadSearchResults(hLog, hSearch, 2, TRUE);
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a search where result buffer is smaller than a returned single result");
	// -----------------------------------------------------------------
	
	AttributeTest[0].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BigBlob;
	AttributeTest[0].qwUserID = 0;
	
	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, AttributeTest, 1, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSearchCreate(GETSESSBLB_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT), hEvent, &hSearch);
	
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

	hr = PollTaskUntilComplete(hSearch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

// TEMP August release won't indicate when results were truncated
//	ReadSearchResults(hLog, hSearch, 1, TRUE);
	ReadSearchResults(hLog, hSearch, 0, TRUE);
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
	

	// -----------------------------------------------------------------
	START_TESTCASE("Create a search where result buffer is smaller than multiple results");
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

	hr = XOnlineMatchSearchCreate(GETTITLEALL_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT) - 10, hEvent, &hSearch);

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

	ReadSearchResults(hLog, hSearch, 1, TRUE);
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
	


	// -----------------------------------------------------------------
	START_TESTCASE("Create a search where multiple results won't fit into buffer");
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

	hr = XOnlineMatchSearchCreate(GETTITLEALL_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT) + 100, hEvent, &hSearch);

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

// TEMP August release won't indicate when results were truncated
//	ReadSearchResults(hLog, hSearch, 2, TRUE);
	ReadSearchResults(hLog, hSearch, 1, TRUE);
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);

	

	// -----------------------------------------------------------------
	START_TESTCASE("Call work pump immediately after creating a search");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSearchCreate(GETSESSALL_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT), hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	fRet = XOnlineTaskDoWork(hSearch,0);

	if(fRet)
	{
		PASS_TESTCASE("XOnlineTaskDoWork returned TRUE");
	}
	else
	{
		FAIL_TESTCASE("XOnlineTaskDoWork returned FALSE, but should have returned TRUE");
	}
	
	END_TESTCASE();
	CLEANUP_TASK(hSearch);	



	// -----------------------------------------------------------------
	START_TESTCASE("Open and close an XMatch search without calling the work pump");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchCreate(GETSESSALL_INDEX, 10, 0, 0, sizeof(XMATCH_SEARCHRESULT), hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	XOnlineTaskClose(hSearch);
	hSearch = NULL;

	PASS_TESTCASE("XOnlineTaskClose completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);	



	// -----------------------------------------------------------------
	START_TESTCASE("Cancel a search request that has already completed and then close it");
	// -----------------------------------------------------------------
	// This case also tests the case where the search returns no results

	hr = XOnlineMatchSearchCreate(GETSESSSP_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT), hEvent, &hSearch);
	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}


	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[0].Value.qwValue = 1;
	
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
} // XMatchTest_CreateSearch


} // namespace XMatchTestNamespace