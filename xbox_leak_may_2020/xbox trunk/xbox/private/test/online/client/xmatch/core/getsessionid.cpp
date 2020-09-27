//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_GetSessionID(HANDLE hLog);

//==================================================================================
// XMatchTest_GetSessionID
//----------------------------------------------------------------------------------
//
// Description: Contains test cases for getting session ID's from completed creations
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT XMatchTest_GetSessionID(HANDLE hLog, XONLINETASK_HANDLE hLogon)
{
	XONLINETASK_HANDLE hMatch = NULL;
	XNKID SessionID, NewSessionID;
	HRESULT hr = E_FAIL;
	HANDLE hEvent = NULL;
    DWORD dwPollCounter = 0;
	BOOL fCompleted = FALSE, fDone = FALSE;

	memset(&SessionID, 0, sizeof(SessionID));
	memset(&NewSessionID, 0, sizeof(NewSessionID));

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
		goto Exit;

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get session ID with NULL task handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionGetID (NULL, &SessionID);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionGetID  failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but should have failed!");
	}

	END_TESTCASE();
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get session ID with NULL session ID pointer");
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

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSessionGetID (hMatch, NULL);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionGetID  failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get session ID for a session that hasn't yet been sent to server");
	// -----------------------------------------------------------------

	hMatch = NULL;
	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 0, 0, hEvent, &hMatch );

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionGetID (hMatch, &SessionID);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionGetID  failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Get session ID for a session that has been sent but hasn't completed");
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

	hr = XOnlineMatchSessionGetID (hMatch, &SessionID);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionGetID  failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Get session ID with a completed session creation handle");
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

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

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
	START_TESTCASE("Get session ID with a completed session modification handle");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to modify");
	}

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

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSessionGetID (hMatch, &NewSessionID);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  failed unexpectedly");
	}
	else if(memcmp(&NewSessionID, &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but session ID was unexpected");
	}

	PASS_TESTCASE("XOnlineMatchSessionGetID  returned expected session ID");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Get session ID with a reused session creation handle");
	// -----------------------------------------------------------------

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

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

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

	hr = XOnlineMatchSessionSend(hMatch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session search failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session search didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSessionGetID (hMatch, &NewSessionID);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  failed unexpectedly");
	}
	else if(memcmp(&NewSessionID, &SessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but session ID was unexpected");
	}

	PASS_TESTCASE("XOnlineMatchSessionGetID  returned expected session ID");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);

Exit:
	if(hEvent)
		CloseHandle(hEvent);

	if(!fCompleted && hMatch)
		XOnlineTaskClose(hMatch);

	return hr;
} // XMatchTest_GetSessionID


} // namespace XMatchTestNamespace