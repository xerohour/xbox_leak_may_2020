//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_AddRemovePlayers(HANDLE hLog);

//==================================================================================
// XMatchTest_AddRemovePlayers
//----------------------------------------------------------------------------------
//
// Description: Contains test cases for adding and removing players from sessions
//
// Arguments:
//	HANDLE hLog		Handle to logging subsystem
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT XMatchTest_AddRemovePlayers(HANDLE hLog, XONLINETASK_HANDLE hLogon)
{
	PXMATCH_SEARCHRESULT pSearchResult = NULL;
	XONLINETASK_HANDLE hMatch = NULL, hSearch = NULL;
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
	START_TESTCASE("Add player with a NULL task handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionAddPlayer(NULL, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionAddPlayer failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer returned success but should have failed!");
	}

	END_TESTCASE();
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Remove player with a NULL task handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionRemovePlayer(NULL, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionRemovePlayer failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer returned success but should have failed!");
	}

	END_TESTCASE();
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Add player with an invalid ID");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, 0);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionAddPlayer failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);	
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Remove player with an invalid ID");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionRemovePlayer(hMatch, 0);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionRemovePlayer failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);	
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Add player when there isn't enough room in the request");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt() - 1, hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionAddPlayer failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Remove player when there isn't enough room in the request");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt() - 1, hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionRemovePlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionRemovePlayer failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Add player to a task that has already been sent");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
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

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionAddPlayer failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Remove player from a task that has already been sent");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
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

	hr = XOnlineMatchSessionRemovePlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionRemovePlayer failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer returned success but should have failed!");
	}

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/

	// -----------------------------------------------------------------
	START_TESTCASE("Verify that add player is cleared after successful send completion");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
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

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALUSERINT_ID1, USER1, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt failed unexpectedly");
	}

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Verify that remove player is cleared after successful send completion");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));

	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionRemovePlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer failed unexpectedly");
	}

	hr = XOnlineMatchSessionSend(hMatch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete  returned 0x%08x", hr);
	if(hr != XMATCH_E_INVALID_PLAYER)
	{
		FAIL_TESTCASE("Session creation returned success but should have failed");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
	}

/* API behavior is such that if a request fails, the attributes inside it aren't removed... makes sense

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALUSERINT_ID1, USER1, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt failed unexpectedly");
	}
*/

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


	// -----------------------------------------------------------------
	START_TESTCASE("Verify that add player is cleared after second successful send completion");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
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

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
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

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALUSERINT_ID1, USER1, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt failed unexpectedly");
	}

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Verify that remove player is cleared after second successful send completion");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
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

	hr = XOnlineMatchSessionRemovePlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer failed unexpectedly");
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

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALUSERINT_ID1, USER1, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt failed unexpectedly");
	}

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Call add twice for same player");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
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

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Call remove twice for same player");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionRemovePlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer failed unexpectedly");
	}

	hr = XOnlineMatchSessionRemovePlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer failed unexpectedly");
	}

	hr = XOnlineMatchSessionSend(hMatch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete  returned 0x%08x", hr);
	if(!FAILED(hr))
	{
		FAIL_TESTCASE("Session creation returned success but should have failed");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


	// -----------------------------------------------------------------
	START_TESTCASE("Call add when remove is already present for player");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionRemovePlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
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

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Call remove when add is already present for player");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
	}

	hr = XOnlineMatchSessionRemovePlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer failed unexpectedly");
	}

	hr = XOnlineMatchSessionSend(hMatch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete  returned 0x%08x", hr);
	if(!FAILED(hr))
	{
		FAIL_TESTCASE("Session creation returned success but should have failed");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Call to add a player via XOnlineMatchSessionSetInt");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt() * 1, hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetInt(hMatch, XMATCHUSERINT_ID1, USER1, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(!FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt allowed us to add a player");
	}

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Call remove when add and a user attribute are already present for player");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, GetBufferLenForUserInt() * 2, hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionAddPlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALUSERINT_ID1, USER1, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt failed unexpectedly");
	}

	hr = XOnlineMatchSessionRemovePlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer failed unexpectedly");
	}

	hr = XOnlineMatchSessionSend(hMatch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete  returned 0x%08x", hr);
	if(!FAILED(hr))
	{
		FAIL_TESTCASE("Session creation returned success but should have failed");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


	// -----------------------------------------------------------------
	START_TESTCASE("Add user attribute for a player that is already being removed");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, GetBufferLenForUserInt() * 2, hEvent, &hMatch );
	
	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionRemovePlayer(hMatch, USER1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionRemovePlayer returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionRemovePlayer failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALUSERINT_ID1, USER1, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt failed unexpectedly");
	}

	hr = XOnlineMatchSessionSend(hMatch);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSend returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, FALSE);

	xLog(hLog, XLL_INFO, "PollTaskUntilComplete  returned 0x%08x", hr);
	if(!FAILED(hr))
	{
		FAIL_TESTCASE("Session creation returned success but should have failed");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("Session creation didn't complete within 30 seconds");
	}

	PASS_TESTCASE("Test passed");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);

Exit:
	if(hEvent)
		CloseHandle(hEvent);

	if(!fCompleted && hMatch)
		XOnlineTaskClose(hMatch);

	return hr;
} // XMatchTest_AddRemovePlayers


} // namespace XMatchTestNamespace