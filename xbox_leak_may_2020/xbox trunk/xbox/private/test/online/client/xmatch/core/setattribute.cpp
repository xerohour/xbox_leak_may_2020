//==================================================================================
// Includes
//==================================================================================
#include "xmatchtest.h"

using namespace XMatchTestNamespace;

namespace XMatchTestNamespace {

//==================================================================================
// Prototypes
//==================================================================================
HRESULT XMatchTest_SetAttributes(HANDLE hLog);

//==================================================================================
// XMatchTest_SetAttributes
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
HRESULT XMatchTest_SetAttributes(HANDLE hLog, XONLINETASK_HANDLE hLogon)
{
	XONLINETASK_HANDLE hMatch;
	ULONGLONG qwUserID = 0;
	XNKID SessionID;
	ATTRIBUTE AttributeTest[4];
	HRESULT hr = E_FAIL;
    HANDLE hEvent = NULL;
    DWORD dwPollCounter = 0, dwTemp = 0;
	BOOL fCompleted = FALSE, fRet = FALSE, fDone = FALSE;

	memset(&SessionID, 0, sizeof(SessionID));

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
		goto Exit;

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Set integer with NULL task handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionSetInt(NULL, GLOBALINT_ID1, qwUserID, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetInt failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt returned success but should have failed!");
	}

	END_TESTCASE();
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Try to add integer attribute beyond max index");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForInt() + GetBufferLenForInt(), hEvent, &hMatch );

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALINT_ID1, qwUserID, INT1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALINT_ID2, qwUserID, INT2);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetInt failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);



	// -----------------------------------------------------------------
	START_TESTCASE("Try to add integer attribute when there isn't enough space available");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, GetBufferLenForInt() + GetBufferLenForInt() - 1, hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALINT_ID1, qwUserID, INT1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALINT_ID2, qwUserID, INT2);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetInt failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to add integer attribute with non-integer ID");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForInt(), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALSTR_ID1, qwUserID, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetInt failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to set an integer for a session that has already been sent");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForInt(), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALINT_ID1;
	AttributeTest[0].Value.qwValue = 10;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALINT_ID1, qwUserID, 10);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetInt failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to add user integer attribute with NULL user ID");
	// -----------------------------------------------------------------

	qwUserID = 0;
	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALUSERINT_ID1, qwUserID, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetInt failed.  User attributes with NULL user ID's are failing.");
	}
	else
	{
		PASS_TESTCASE("XOnlineMatchSessionSetInt succeeded.  User attributes with NULL user ID's are working.");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to add non-user integer attribute with non-NULL user ID");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForUserInt(), hEvent, &hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	qwUserID = USER1;
	
	hr = XOnlineMatchSessionSetInt(hMatch, GLOBALINT_ID1, qwUserID, INT1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetInt returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetInt failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetInt returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
	qwUserID = 0;
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Add public available when zero attribute were expected");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 0, 0, hEvent, &hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

// BUGBUG: Removed XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE since it's no longer supported
	AttributeTest[0].dwAttributeID = /*XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE*/ 0;
	AttributeTest[0].Value.qwValue = 10;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Add private availalbe when zero attribute were expected");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 0, 0, hEvent, &hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

// BUGBUG: Removed XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE since it's no longer supported
	AttributeTest[0].dwAttributeID = /*XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE*/ 0;
	AttributeTest[0].Value.qwValue = 10;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Add public available when additional attributes are expected");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForInt(), hEvent, &hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

// BUGBUG: Removed XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE since it's no longer supported
	AttributeTest[0].dwAttributeID = /*XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE*/ 0;
	AttributeTest[0].Value.qwValue = 10;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALINT_ID1;
	AttributeTest[1].Value.qwValue = INT1;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Add private available when additional attributes are expected");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForInt(), hEvent, &hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
// BUGBUG: Removed XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE since it's no longer supported
	AttributeTest[0].dwAttributeID = /*XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE*/ 0;
	AttributeTest[0].Value.qwValue = 10;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALINT_ID1;
	AttributeTest[1].Value.qwValue = INT1;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Update existing public available attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 0, 0, hEvent, &hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

// BUGBUG: Removed XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE since it's no longer supported
	AttributeTest[0].dwAttributeID = /*XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE*/ 0;
	AttributeTest[0].Value.qwValue = 1;
	AttributeTest[0].qwUserID = 0;
// BUGBUG: Removed XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE since it's no longer supported
	AttributeTest[1].dwAttributeID = /*XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE*/ 0;
	AttributeTest[1].Value.qwValue = 0xFFFFFFFF;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Update existing private available attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 0, 0, hEvent, &hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

// BUGBUG: Removed XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE since it's no longer supported
	AttributeTest[0].dwAttributeID = /*XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE*/ 0;
	AttributeTest[0].Value.qwValue = 1;
	AttributeTest[0].qwUserID = 0;
// BUGBUG: Removed XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE since it's no longer supported
	AttributeTest[1].dwAttributeID = /*XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE*/ 0;
	AttributeTest[1].Value.qwValue = 0xFFFFFFFF;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Try to add integer attribute when there is exactly enough space available");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForInt(), hEvent, &hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session and add the same integer attribute twice");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForInt(), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALINT_ID1;
	AttributeTest[1].Value.qwValue = INT2;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with global session integer attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForInt(), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with title-specific session integer attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForInt(), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = TITLEINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with global user integer attribute for non-existent player");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");
	
	AttributeTest[0].dwAttributeID = GLOBALUSERINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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

	PASS_TESTCASE("Creating session with non-exisitent player integer failed as expected");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a session with global user integer attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, GetBufferLenForUserInt() * 2, hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");
	
	AttributeTest[0].dwAttributeID = GLOBALUSERINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with title-specific user integer attribute for non-existent user");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserInt(), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = TITLEUSERINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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

	PASS_TESTCASE("Creating session with non-exisitent player integer failed as expected");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a session with title-specific user integer attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, 2 * GetBufferLenForUserInt(), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = TITLEUSERINT_ID1;
	AttributeTest[0].Value.qwValue = INT1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Set string with NULL task handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionSetString(NULL, GLOBALSTR_ID1, qwUserID, STRING1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetString returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetString failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetString returned success but should have failed!");
	}

	END_TESTCASE();
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Try to add string attribute beyond max index");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForString(STRING1) + GetBufferLenForString(STRING2), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
	hr = XOnlineMatchSessionSetString(hMatch, GLOBALSTR_ID1, qwUserID, STRING1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetString failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetString(hMatch, GLOBALSTR_ID2, qwUserID, STRING2);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetString returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetString failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetString returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
	

	
	// -----------------------------------------------------------------
	START_TESTCASE("Try to add string attribute when there isn't enough space available");
	// -----------------------------------------------------------------
	
	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, GetBufferLenForString(STRING1) + 1, hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
		
	hr = XOnlineMatchSessionSetString(hMatch, GLOBALSTR_ID1, qwUserID, STRING1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetString failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetString(hMatch, GLOBALSTR_ID2, qwUserID, STRING2);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetString returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetString failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetString returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
	

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to add string attribute with non-string ID");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}


	hr = XOnlineMatchSessionSetString(hMatch, GLOBALINT_ID1, qwUserID, STRING1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetString returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetString failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetString returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to set a string for a session that has already been sent");
	// -----------------------------------------------------------------

	hMatch = NULL;
	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) STRING1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetString(hMatch, GLOBALSTR_ID1, qwUserID, STRING2);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetString returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetString failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetString returned success but should have failed!");
	}
	
	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/
	
/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to add user string attribute with NULL user ID");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForUserString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
	hr = XOnlineMatchSessionSetString(hMatch, GLOBALUSERSTR_ID1, qwUserID, STRING1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetString returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetString failed.  User attributes with NULL user ID's are failing.");
	}
	else
	{
		PASS_TESTCASE("XOnlineMatchSessionSetString succeeded.  User attributes with NULL user ID's are working.");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to add non-user string attribute with non-NULL user ID");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	qwUserID = USER1;
	
	hr = XOnlineMatchSessionSetString(hMatch, GLOBALSTR_ID1, qwUserID, STRING1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetString returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetString failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetString returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
	qwUserID = 0;
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Try to add string attribute when there is exactly enough space available");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) STRING1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session and add an empty string");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForString(L""), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
	AttributeTest[0].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) L"";
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Resize max-length strings");
	// -----------------------------------------------------------------
	
	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, 2 * GetBufferLenForString(RESIZESTRING_MID), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) RESIZESTRING_MID;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALSTR_ID2;
	AttributeTest[1].Value.pvValue = (VOID *) RESIZESTRING_MID;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}
	
	AttributeTest[0].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) RESIZESTRING_MIN;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALSTR_ID2;
	AttributeTest[1].Value.pvValue = (VOID *) RESIZESTRING_MAX;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}
	
	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session and add the same string attribute twice");
	// -----------------------------------------------------------------
	
	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForString(STRING2), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
	AttributeTest[0].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) STRING1;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[1].Value.pvValue = (VOID *) STRING2;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with global session string attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
	AttributeTest[0].dwAttributeID = GLOBALSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) STRING1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with title-specific session string attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = TITLESTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) STRING1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with global user string attribute for a non-existent user");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALUSERSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) STRING1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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

	PASS_TESTCASE("Creating session with non-exisitent player string failed as expected");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a session with global user string attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, GetBufferLenForUserInt() + GetBufferLenForUserString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALUSERSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) STRING1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with title-specific user string attribute for a non-existent user");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = TITLEUSERSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) STRING1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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

	PASS_TESTCASE("Creating session with non-exisitent player string failed as expected");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a session with title-specific user string attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, GetBufferLenForUserInt() + GetBufferLenForUserString(STRING1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = TITLEUSERSTR_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) STRING1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Set blob with NULL task handle");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionSetBlob(NULL, GLOBALBLB_ID1, qwUserID, strlen(BLOB1), BLOB1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetBlob returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetBlob failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetBlob returned success but should have failed!");
	}

	END_TESTCASE();
*/

	// -----------------------------------------------------------------
	START_TESTCASE("Try to add blob attribute beyond max index");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetBlob(hMatch, GLOBALBLB_ID1, qwUserID, strlen(BLOB1), BLOB1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetBlob failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetBlob(hMatch, GLOBALBLB_ID2, qwUserID, strlen(BLOB2), BLOB2);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetBlob returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetBlob failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetBlob returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);

	

	// -----------------------------------------------------------------
	START_TESTCASE("Try to add blob attribute when there isn't enough space available");
	// -----------------------------------------------------------------
	
	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, GetBufferLenForBlob(BLOB1) + 1, hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
	hr = XOnlineMatchSessionSetBlob(hMatch, GLOBALBLB_ID1, qwUserID, strlen(BLOB1), BLOB1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetBlob failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetBlob(hMatch, GLOBALBLB_ID2, qwUserID, strlen(BLOB2), BLOB2);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetBlob returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetBlob failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetBlob returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
	

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to add blob attribute with non-blob ID");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetBlob(hMatch, GLOBALINT_ID1, qwUserID, strlen(BLOB1), BLOB1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetBlob returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetBlob failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetBlob returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/


/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to set a blob for a session that has already been sent");
	// -----------------------------------------------------------------

	hMatch = NULL;
	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSend failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetBlob(hMatch, GLOBALBLB_ID1, qwUserID, strlen(BLOB2), BLOB2);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetBlob returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetBlob failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetBlob returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/
	

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to add user blob attribute with NULL user ID");
	// -----------------------------------------------------------------
	
	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForUserBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetBlob(hMatch, GLOBALUSERBLB_ID1, qwUserID, strlen(BLOB1), BLOB1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetBlob returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetBlob failed.  User attributes with NULL user ID's are failing.");
	}
	else
	{
		PASS_TESTCASE("XOnlineMatchSessionSetBlob succeeded.  User attributes with NULL user ID's are working.");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
*/

/* Causes RIP
	// -----------------------------------------------------------------
	START_TESTCASE("Try to add non-user blob attribute with non-NULL user ID");
	// -----------------------------------------------------------------

	hr = XOnlineMatchSessionCreate(0, 1, GetBufferLenForBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	qwUserID = USER1;

	hr = XOnlineMatchSessionSetBlob(hMatch, GLOBALBLB_ID1, qwUserID, strlen(BLOB1), BLOB1);

	xLog(hLog, XLL_INFO, "XOnlineMatchSessionSetBlob returned 0x%08x", hr);
	if(FAILED(hr))
	{
		PASS_TESTCASE("XOnlineMatchSessionSetBlob failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetBlob returned success but should have failed!");
	}

	END_TESTCASE();
	CLEANUP_TASK(hMatch);
	qwUserID = 0;
*/


	// -----------------------------------------------------------------
	START_TESTCASE("Try to add blob attribute when there is exactly enough space available");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
	AttributeTest[0].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session and add an empty blob");
	// -----------------------------------------------------------------
	
	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForBlob(""), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionSetBlob(hMatch, GLOBALBLB_ID1, 0, 0, NULL);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionSetBlob failed unexpectedly");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Resize max-length blobs");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, 2 * GetBufferLenForBlob(RESIZEBLOB_MID), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) RESIZEBLOB_MID;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALBLB_ID2;
	AttributeTest[1].Value.pvValue = (VOID *) RESIZEBLOB_MID;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}
	
	AttributeTest[0].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) "";
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALBLB_ID2;
	AttributeTest[1].Value.pvValue = (VOID *) RESIZEBLOB_MAX;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}
	
	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session and add the same blob attribute twice");
	// -----------------------------------------------------------------
	
	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForBlob(BLOB2), hEvent, &hMatch);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[0].qwUserID = 0;
	AttributeTest[1].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[1].Value.pvValue = (VOID *) BLOB2;
	AttributeTest[1].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 2);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with global session blob attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}
	
	AttributeTest[0].dwAttributeID = GLOBALBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with title-specific session blob attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = TITLEBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[0].qwUserID = 0;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with global user blob attribute for a non-existent user");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALUSERBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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

	PASS_TESTCASE("Creating session with non-exisitent player blob failed as expected");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a session with global user blob attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, GetBufferLenForUserInt() + GetBufferLenForUserBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = GLOBALUSERBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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
	START_TESTCASE("Create a session with title-specific user blob attribute for non-existent user");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 1, GetBufferLenForUserBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = TITLEUSERBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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

	PASS_TESTCASE("Creating session with non-exisitent player blob failed as expected");

	END_TESTCASE();
	CLEANUP_TASK(hMatch);


	// -----------------------------------------------------------------
	START_TESTCASE("Create a session with title-specific user blob attribute");
	// -----------------------------------------------------------------

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, 2, GetBufferLenForUserInt() + GetBufferLenForUserBlob(BLOB1), hEvent, &hMatch );
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionCreate failed unexpectedly");
	}

	hr = XOnlineMatchSessionAddPlayer(hMatch, USER1);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineMatchSessionAddPlayer failed unexpectedly");
	}

	AttributeTest[0].dwAttributeID = TITLEUSERBLB_ID1;
	AttributeTest[0].Value.pvValue = (VOID *) BLOB1;
	AttributeTest[0].qwUserID = USER1;
	
	fRet = AddAttributes(hMatch, AttributeTest, 1);
	if(!fRet)
	{
		FAIL_TESTCASE("Couldn't add attributes to session!");
	}

	hr = XOnlineMatchSessionSend(hMatch);
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


Exit:
	if(hEvent)
		CloseHandle(hEvent);

	if(!fCompleted && hMatch)
		XOnlineTaskClose(hMatch);

	return hr;
} // XMatchTest_SetAttributes


} // namespace XMatchTestNamespace