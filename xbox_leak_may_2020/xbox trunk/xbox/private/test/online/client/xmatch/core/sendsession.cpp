//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_SendSession(HANDLE hLog);

//==================================================================================
// XMatchTest_SendSession
//----------------------------------------------------------------------------------
//
// Description: Contains test cases for sending sessions via Xbox Matchmaking APIs
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT XMatchTest_SendSession(HANDLE hLog, XONLINETASK_HANDLE hLogon)
{
	XONLINETASK_HANDLE hMatch = NULL;
	XNKID SessionID, NewSessionID;
	ATTRIBUTE AttributeTest[4];
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
	START_TESTCASE("Send with NULL session handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionSend(NULL);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSend failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend returned success but should have failed!");
	}

	END_TESTCASE();
*/

	// -----------------------------------------------------------------
	START_TESTCASE("Send and poll without waiting for event handle");
	// -----------------------------------------------------------------

	hMatch = NULL;
	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 0, 0, hEvent, &hMatch );

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSend(hMatch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hMatch, NULL, 30000, &dwPollCounter, &fCompleted, FALSE);

	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSessionGetID (hMatch, &SessionID);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  failed unexpectedly");
	}
	else if(*((ULONGLONG *) &SessionID) == 0)
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but didn't return session ID");
	}

	PASS_TESTCASE("XOnlineMatchSessionGetID  returned valid session ID");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);

	// -----------------------------------------------------------------
	START_TESTCASE("Send without adding expected attributes");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 3, GetBufferLenForInt() + GetBufferLenForString(STRING1) + GetBufferLenForBlob(BLOB1), hEvent, &hMatch );

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
	AttributeTest[0].dwAttributeID = GLOBALINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[1].Value.pvValue = (VOID *) STRING1;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);

	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}
	
	hr = XOnlineMatchSessionSend(hMatch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hMatch, NULL, 30000, &dwPollCounter, &fCompleted, FALSE);

	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSessionGetID (hMatch, &SessionID);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  failed unexpectedly");
	}
	else if(*((ULONGLONG *) &SessionID) == 0)
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but didn't return session ID");
	}

	PASS_TESTCASE("XOnlineMatchSessionGetID  returned valid session ID");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Send a task handle that has already been sent");
	// -----------------------------------------------------------------

	hMatch = NULL;
	hr = XOnlineMatchSessionCreate(0, 0, 0, hEvent, &hMatch );

	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSend(hMatch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = XOnlineMatchSessionSend(hMatch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSend failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend returned success but should have failed!");
	}
	
	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/

Exit:
	if(hEvent)
		CloseHandle(hEvent);
	
	if(!fCompleted && hMatch)
		XOnlineTaskClose(hMatch);
	
	return hr;
} // XMatchTest_SendSession


} // namespace XMatchTestNamespace