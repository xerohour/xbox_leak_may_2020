//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_DeleteSession(HANDLE hLog);

//==================================================================================
// XMatchTest_DeleteSession
//----------------------------------------------------------------------------------
//
// Description: Contains test cases for deleting sessions
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT XMatchTest_DeleteSession(HANDLE hLog, XONLINETASK_HANDLE hLogon)
{
	XONLINETASK_HANDLE hDelete = NULL;
	XNKID SessionID;
	HRESULT hr = E_FAIL;
    HANDLE hEvent = NULL;
    DWORD dwPollCounter = 0;
	BOOL fCompleted = FALSE, fDone = FALSE;

	memset(&SessionID, 0, sizeof(SessionID));

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
		goto Exit;

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Delete session with NULL task handle");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to delete");
	}

	hr = XOnlineMatchSessionDelete(SessionID, hEvent, NULL);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionDelete returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionDelete failed as expected");
	}
	else
	{
		XOnlineTaskClose(hDelete);
		hDelete = NULL;
		FAIL_TESTCASE("XOnlineMatchSessionDelete returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hDelete);
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Delete session with session ID of 0");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionDelete(SessionID, hEvent, &hDelete);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionDelete returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionDelete failed as expected");
	}
	else
	{
		XOnlineTaskClose(hDelete);
		hDelete = NULL;
		FAIL_TESTCASE("XOnlineMatchSessionDelete returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hDelete);
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Delete session with a NULL work handle");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to delete");
	}

	hr = XOnlineMatchSessionDelete(SessionID, NULL, &hDelete);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionDelete returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionDelete failed unexpectedly");
	}
	
	hr = PollTaskUntilComplete(hDelete, NULL, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session delete failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session delete didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Delete completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hDelete);



	// -----------------------------------------------------------------
	START_TESTCASE("Delete session with a valid work handle");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to delete");
	}

	hr = XOnlineMatchSessionDelete(SessionID, hEvent, &hDelete);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionDelete returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionDelete failed unexpectedly");
	}
	
	hr = PollTaskUntilComplete(hDelete, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session delete failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session delete didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Delete completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hDelete);


	// -----------------------------------------------------------------
	START_TESTCASE("Try to delete a session that doesn't exist");
	// -----------------------------------------------------------------

	*((ULONGLONG *) &SessionID) = 0x8000000000000000;

	hr = XOnlineMatchSessionDelete(SessionID, hEvent, &hDelete);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionDelete returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionDelete failed unexpectedly");
	}
	
	hr = PollTaskUntilComplete(hDelete, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete  returned 0x%08x", hr);
	if(!FAILED(hr))
	{
		FAIL_TESTCASE("Delete returned success but should have failed");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session delete didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Delete completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hDelete);


	// -----------------------------------------------------------------
	START_TESTCASE("Delete session that doesn't have any pub/priv available");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(0, 0, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to delete");
	}

	hr = XOnlineMatchSessionDelete(SessionID, hEvent, &hDelete);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionDelete returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionDelete failed unexpectedly");
	}
	
	hr = PollTaskUntilComplete(hDelete, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session delete failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session delete didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Delete completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hDelete);



	// -----------------------------------------------------------------
	START_TESTCASE("Open and close a delete task without calling work pump");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionDelete(SessionID, hEvent, &hDelete);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionDelete returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionDelete failed unexpectedly");
	}

	XOnlineTaskClose(hDelete);
	hDelete = NULL;

	PASS_TESTCASE("XOnlineTaskClose completed successfully");

	END_TESTCASE();
	CLEANUP_TASK(hDelete);	



	// -----------------------------------------------------------------
	START_TESTCASE("Cancel a delete task that has already completed and then close it");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	if(!CreateSession(10, 10, NULL, 0, &SessionID))
	{
		FAIL_TESTCASE("Couldn't create session to delete");
	}

	hr = XOnlineMatchSessionDelete(SessionID, hEvent, &hDelete);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionDelete returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionDelete failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hDelete, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("Session delete failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session delete didn't complete within 30 seconds");
	}
	
	XOnlineTaskClose(hDelete);
	hDelete = NULL;
	
	PASS_TESTCASE("XOnlineTaskClose completed successfully");
	
	END_TESTCASE();
	CLEANUP_TASK(hDelete);

Exit:
	if(hEvent)
		CloseHandle(hEvent);

	if(!fCompleted && hDelete)
		XOnlineTaskClose(hDelete);

	return hr;
} // XMatchTest_DeleteSession


} // namespace XMatchTestNamespace