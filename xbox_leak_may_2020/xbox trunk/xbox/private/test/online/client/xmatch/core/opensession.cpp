//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_OpenSession(HANDLE hLog);

//==================================================================================
// XMatchTest_OpenSession
//----------------------------------------------------------------------------------
//
// Description: Contains test cases for creating sessions via Xbox Matchmaking APIs
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT XMatchTest_OpenSession(HANDLE hLog, XONLINETASK_HANDLE hLogon)
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

/* CAUSES RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Non-zero attrib count, but zero attrib len");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionCreate(0, 10, 0, hEvent, &hMatch );

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionCreate failed as expected");
	}
	else
	{
		XOnlineTaskClose(hMatch);
		FAIL_TESTCASE("XOnlineMatchSessionCreate returned success but should have failed!");
	}

	END_TESTCASE();
	hMatch = NULL;
*/

/* CAUSES RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Non-zero attrib len, but zero attrib count");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionCreate(0, 0, 1000, hEvent, &hMatch );

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionCreate failed as expected");
	}
	else
	{
		XOnlineTaskClose(hMatch);
		FAIL_TESTCASE("XOnlineMatchSessionCreate returned success but should have failed!");
	}

	END_TESTCASE();
	hMatch = NULL;
*/

/* CAUSES RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Null phSession parameter");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionCreate(0, 10, 1000, hEvent, NULL );

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionCreate failed as expected");
	}
	else
	{
		XOnlineTaskClose(hMatch);
		FAIL_TESTCASE("XOnlineMatchSessionCreate returned success but should have failed!");
	}

	END_TESTCASE();
	hMatch = NULL;
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Try to modify a session that doesn't exist");
	// -----------------------------------------------------------------

	hMatch = NULL;
	*((ULONGLONG *) &SessionID) = 1;
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
	
	xLog(hLog, XLL_INFO, "PollTaskUntilComplete  returned 0x%08x", hr);
	if(hr != XMATCH_E_INVALID_SESSION_ID)
	{
		FAIL_TESTCASE("Session creation didn't return expected error code");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
	}
	
	FAIL_TESTCASE("Received expected error code from server.");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a new session with no attributes");
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
		FAIL_TESTCASE("Session creation failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
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
	START_TESTCASE("Modify the session that was just created without adding attributes");
	// -----------------------------------------------------------------

	memset(&NewSessionID, 0, sizeof(NewSessionID));
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
		FAIL_TESTCASE("Session creation failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSessionGetID (hMatch, &NewSessionID);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  failed unexpectedly");
	}
	else if(memcmp(&SessionID, &NewSessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but returned a different session ID");
	}

	PASS_TESTCASE("XOnlineMatchSessionGetID  returned expected session ID");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Use a null work event");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 0, 0, NULL, &hMatch );
	
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
		FAIL_TESTCASE("Session creation failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
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
	START_TESTCASE("Create a new session with one of each attribute type");
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
	AttributeTest[2].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[2].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[2].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 3);
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

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session creation failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
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
	START_TESTCASE("Modify the session that was just created adding attributes of each type");
	// -----------------------------------------------------------------

	memset(&NewSessionID, 0, sizeof(NewSessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 3, GetBufferLenForInt() + GetBufferLenForString(STRING1) + GetBufferLenForBlob(BLOB1), hEvent, &hMatch );

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALINT_ID2;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALSTR_ID2;
	AttributeTest[1].Value.pvValue = (VOID *) STRING1;
	AttributeTest[1].qwUserID = 0;
	AttributeTest[2].dwAttributeID = GLOBALBLB_ID2;
	AttributeTest[2].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[2].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 3);
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

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session creation failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
	}

	hr = XOnlineMatchSessionGetID (hMatch, &NewSessionID);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  failed unexpectedly");
	}
	else if(memcmp(&SessionID, &NewSessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but returned a different session ID");
	}

	PASS_TESTCASE("XOnlineMatchSessionGetID  returned expected session ID");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Modify session and re-add existing attributes");
	// -----------------------------------------------------------------

	memset(&NewSessionID, 0, sizeof(NewSessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 3, GetBufferLenForInt() + GetBufferLenForString(STRING1) + GetBufferLenForBlob(BLOB1), hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
	AttributeTest[0].dwAttributeID = GLOBALINT_ID2;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALSTR_ID2;
	AttributeTest[1].Value.pvValue = (VOID *) STRING1;
	AttributeTest[1].qwUserID = 0;
	AttributeTest[2].dwAttributeID = GLOBALBLB_ID2;
	AttributeTest[2].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[2].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 3);
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

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session creation failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
	}
	
	hr = XOnlineMatchSessionGetID (hMatch, &NewSessionID);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionGetID  returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  failed unexpectedly");
	}
	else if(memcmp(&SessionID, &NewSessionID, sizeof(XNKID)))
	{
		FAIL_TESTCASE("XOnlineMatchSessionGetID  returned success but returned a different session ID");
	}

	PASS_TESTCASE("XOnlineMatchSessionGetID  returned expected session ID");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Open and close an XMatch request without calling the work pump");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 0, 0, hEvent, &hMatch );

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	XOnlineTaskClose(hMatch);
	hMatch = NULL;

	PASS_TESTCASE("XOnlineTaskClose completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);	



	// -----------------------------------------------------------------
	START_TESTCASE("Open a session request and immediately call work pump");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 0, 0, hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	dwPollCounter = 1;
	fRet = XOnlineTaskDoWork(hMatch, 0);
	
	if(fRet)
	{
		PASS_TESTCASE("XOnlineTaskDoWork returned TRUE");
	}
	else
	{
		FAIL_TESTCASE("XOnlineTaskDoWork returned FALSE, but should have returned TRUE");
	}
	
	END_TESTCASE();
	CLEANUP_TASK(hMatch);	

	// -----------------------------------------------------------------
	START_TESTCASE("Cancel a session request that has already been sent and then close it");
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

	XOnlineTaskClose(hMatch);
	hMatch = NULL;

	PASS_TESTCASE("XOnlineTaskClose completed successfully");
	
	END_TESTCASE();
	CLEANUP_TASK(hMatch);

Exit:
	if(hEvent)
		CloseHandle(hEvent);

	if(!fCompleted && hMatch)
		XOnlineTaskClose(hMatch);

	return hr;
} // XMatchTest_OpenSession


} // namespace XMatchTestNamespace