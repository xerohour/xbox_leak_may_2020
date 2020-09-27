//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_SendSearch(HANDLE hLog);

//==================================================================================
// XMatchTest_SendSearch
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
HRESULT XMatchTest_SendSearch(HANDLE hLog, XONLINETASK_HANDLE hLogon)
{
	PXMATCH_SEARCHRESULT *ppSearchResults = NULL;
	XONLINETASK_HANDLE hSearch = NULL;
	XNADDR LocalXnAddr;
	XNKID SessionID, NewSessionID;
	ATTRIBUTE AttributeTest[4];
	PARAMETER ParameterTest[4];
	HRESULT hr = E_FAIL;
    HANDLE hEvent = NULL;
    DWORD dwPollCounter = 0, *pdwOffset = NULL, *pdwAttributeType = NULL, dwAddrStatus = 0;
	DWORD dwAttributeType = 0, dwAttributeLen = 0, dwNumResults = 0;
	BOOL fCompleted = FALSE, fRet = FALSE, fDone = FALSE;
	BYTE bAttributeBuffer[100];

	memset(&SessionID, 0, sizeof(SessionID));
	memset(&NewSessionID, 0, sizeof(NewSessionID));

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
	START_TESTCASE("Send with NULL session handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSearchSend(NULL);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchSend failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchSend returned success but should have failed!");
	}

	END_TESTCASE();
*/

	// -----------------------------------------------------------------
	START_TESTCASE("Send search and poll without waiting for event handle");
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

	hr = XOnlineMatchSearchCreate(GETSESSSP_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

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

	ReadSearchResults(hLog, hSearch, 1, FALSE);
		
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
	
/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Send a search task that has already been sent");
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

	hr = XOnlineMatchSearchCreate(GETSESSSP_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}
	
	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[0].Value.qwValue = *((ULONGLONG *) SessionID);
	
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

	hr = XOnlineMatchSearchSend(hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSearchSend failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSearchSend returned success but should have failed!");
	}
		
	END_TESTCASE();
	CLEANUP_TASK(hSearch);
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Send search where results will contain 1 integer attribute");
	// -----------------------------------------------------------------

	AttributeTest[0].dwAttributeID = GLOBALINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = 0;

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, AttributeTest, 1, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSearchCreate(GETSESSINT_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

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

	PASS_TESTCASE("Search results contained expected attributes");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);

	// -----------------------------------------------------------------
	START_TESTCASE("Send search where results will contain 1 string attribute");
	// -----------------------------------------------------------------
	
	AttributeTest[0].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) STRING1;
	AttributeTest[0].qwUserID = 0;
	
	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, AttributeTest, 1, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSearchCreate(GETSESSSTR_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

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

	xLog(hLog, XLL_INFO, "Getting string attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 0, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	PASS_TESTCASE("Search results contained expected attributes");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);

	

	// -----------------------------------------------------------------
	START_TESTCASE("Send search where results will contain 1 blob attribute");
	// -----------------------------------------------------------------

	AttributeTest[0].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[0].qwUserID = 0;

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, AttributeTest, 1, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to search for");
	}

	hr = XOnlineMatchSearchCreate(GETSESSBLB_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

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

	xLog(hLog, XLL_INFO, "Getting binary attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 0, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	PASS_TESTCASE("Search results contained expected attributes");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);
	


	// -----------------------------------------------------------------
	START_TESTCASE("Send search without adding expected parameters");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSearchCreate(NOPARAM_INDEX, 10, 1, GetBufferLenForInt(), sizeof(XMATCH_SEARCHRESULT) + 200, hEvent, &hSearch);

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
	START_TESTCASE("Int/String/Blob params and Int/String/Blob results");
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

	hr = XOnlineMatchSearchCreate(INTSTRBLB_INDEX, 10, 3, GetBufferLenForInt() + GetBufferLenForString(STRING1) + GetBufferLenForBlob(BLOB1), sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[0].Value.qwValue = *((ULONGLONG *) &SessionID);
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

	PASS_TESTCASE("Search results contained expected attributes");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);


	// -----------------------------------------------------------------
	START_TESTCASE("Int/Blob/String params and Int/Blob/String results");
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

	hr = XOnlineMatchSearchCreate(INTBLBSTR_INDEX, 10, 3, GetBufferLenForInt() + GetBufferLenForString(STRING1) + GetBufferLenForBlob(BLOB1), sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[0].Value.qwValue = *((ULONGLONG *) &SessionID);
	ParameterTest[1].dwParameterType = X_ATTRIBUTE_DATATYPE_BLOB;
	ParameterTest[1].Value.pvValue = (VOID *) BLOB1;
	ParameterTest[2].dwParameterType = X_ATTRIBUTE_DATATYPE_STRING;
	ParameterTest[2].Value.pvValue = (VOID *) STRING1;
	
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

	xLog(hLog, XLL_INFO, "Getting binary attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 1, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	xLog(hLog, XLL_INFO, "Getting string attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 2, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	PASS_TESTCASE("Search results contained expected attributes");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);


	// -----------------------------------------------------------------
	START_TESTCASE("String/Int/Blob params and String/Int/Blob results");
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

	hr = XOnlineMatchSearchCreate(STRINTBLB_INDEX, 10, 3, GetBufferLenForInt() + GetBufferLenForString(STRING1) + GetBufferLenForBlob(BLOB1), sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_STRING;
	ParameterTest[0].Value.pvValue = (VOID *) STRING1;
	ParameterTest[1].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[1].Value.qwValue = *((ULONGLONG *) &SessionID);
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

	if(memcmp(&((ppSearchResults[0])->SessionID), &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("Search result returned unexpected session ID!");
	}

	if(memcmp(&((ppSearchResults[0])->HostAddress), &LocalXnAddr, sizeof(XNADDR)))
	{
		FAIL_TESTCASE("Returned host address differs from local address");
	}

	xLog(hLog, XLL_INFO, "Getting string attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 0, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	xLog(hLog, XLL_INFO, "Getting integer attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 1, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	PASS_TESTCASE("Search results contained expected attributes");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);


	// -----------------------------------------------------------------
	START_TESTCASE("Blob/Int/String params and Blob/Int/String results");
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

	hr = XOnlineMatchSearchCreate(BLBINTSTR_INDEX, 10, 3, GetBufferLenForInt() + GetBufferLenForString(STRING1) + GetBufferLenForBlob(BLOB1), sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_BLOB;
	ParameterTest[0].Value.pvValue = (VOID *) BLOB1;
	ParameterTest[1].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[1].Value.qwValue = *((ULONGLONG *) &SessionID);
	ParameterTest[2].dwParameterType = X_ATTRIBUTE_DATATYPE_STRING;
	ParameterTest[2].Value.pvValue = (VOID *) STRING1;
	
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

	if(memcmp(&((ppSearchResults[0])->SessionID), &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("Search result returned unexpected session ID!");
	}

	if(memcmp(&((ppSearchResults[0])->HostAddress), &LocalXnAddr, sizeof(XNADDR)))
	{
		FAIL_TESTCASE("Returned host address differs from local address");
	}

	xLog(hLog, XLL_INFO, "Getting binary attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 0, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	xLog(hLog, XLL_INFO, "Getting integer attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 1, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 2, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	PASS_TESTCASE("Search results contained expected attributes");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);


	// -----------------------------------------------------------------
	START_TESTCASE("String/Blob/Int params and String/Blob/Int results");
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

	hr = XOnlineMatchSearchCreate(STRBLBINT_INDEX, 10, 3, GetBufferLenForInt() + GetBufferLenForString(STRING1) + GetBufferLenForBlob(BLOB1), sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_STRING;
	ParameterTest[0].Value.pvValue = (VOID *) STRING1;
	ParameterTest[1].dwParameterType = X_ATTRIBUTE_DATATYPE_BLOB;
	ParameterTest[1].Value.pvValue = (VOID *) BLOB1;
	ParameterTest[2].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[2].Value.qwValue = *((ULONGLONG *) &SessionID);
	
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

	if(memcmp(&((ppSearchResults[0])->SessionID), &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("Search result returned unexpected session ID!");
	}

	if(memcmp(&((ppSearchResults[0])->HostAddress), &LocalXnAddr, sizeof(XNADDR)))
	{
		FAIL_TESTCASE("Returned host address differs from local address");
	}

	xLog(hLog, XLL_INFO, "Getting string attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 0, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 1, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	xLog(hLog, XLL_INFO, "Getting integer attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 2, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	PASS_TESTCASE("Search results contained expected attributes");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);

	// -----------------------------------------------------------------
	START_TESTCASE("Blob/String/Int params and Blob/String/Int results");
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

	hr = XOnlineMatchSearchCreate(BLBSTRINT_INDEX, 10, 3, GetBufferLenForInt() + GetBufferLenForString(STRING1) + GetBufferLenForBlob(BLOB1), sizeof(XMATCH_SEARCHRESULT) + 500, hEvent, &hSearch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSearchCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSearchCreate failed unexpectedly");
	}

	ParameterTest[0].dwParameterType = X_ATTRIBUTE_DATATYPE_BLOB;
	ParameterTest[0].Value.pvValue = (VOID *) BLOB1;
	ParameterTest[1].dwParameterType = X_ATTRIBUTE_DATATYPE_STRING;
	ParameterTest[1].Value.pvValue = (VOID *) STRING1;
	ParameterTest[2].dwParameterType = X_ATTRIBUTE_DATATYPE_INTEGER;
	ParameterTest[2].Value.qwValue = *((ULONGLONG *) &SessionID);
	
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

	if(memcmp(&((ppSearchResults[0])->SessionID), &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("Search result returned unexpected session ID!");
	}

	if(memcmp(&((ppSearchResults[0])->HostAddress), &LocalXnAddr, sizeof(XNADDR)))
	{
		FAIL_TESTCASE("Returned host address differs from local address");
	}

	xLog(hLog, XLL_INFO, "Getting binary attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 0, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	xLog(hLog, XLL_INFO, "Getting integer attribute");
	dwAttributeLen = sizeof(bAttributeBuffer);
	hr = XOnlineMatchSearchGetAttribute(hSearch, 0, 2, &dwAttributeType, (LPVOID) bAttributeBuffer, &dwAttributeLen);

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

	PASS_TESTCASE("Search results contained expected attributes");

	END_TESTCASE();
	CLEANUP_TASK(hSearch);


Exit:
	if(hEvent)
		CloseHandle(hEvent);
	
	if(!fCompleted && hSearch)
		XOnlineTaskClose(hSearch);
	
	return hr;
} // XMatchTest_SendSearch


} // namespace XMatchTestNamespace