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
HRESULT XMatchTest_OpenSession(HANDLE hLog)
{
	PXMATCH_EXTENDED_HANDLE pMatchHandle = NULL;
	XONLINETASK_HANDLE hMatch = NULL;
	ULONGLONG qwSessionID = 0, qwNewSessionID = 0;
	ATTRIBUTE AttributeTest[4];
	HRESULT hr = E_FAIL;
    HANDLE hEvent = NULL;
    DWORD dwPollCounter = 0, dwWorkFlags = 0;
	BOOL fCompleted = FALSE, fRet = FALSE;

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
		goto Exit;

/* CAUSES RIP
	// -------------------------------------------
	// Non-zero attrib count, but zero attrib len
	// -------------------------------------------
	xStartVariation(hLog, "Non-zero attrib count, but zero attrib len");

	hr = XOnlineMatchSessionCreate(0, 10, 0, hEvent, &hMatch );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_PASS, "XOnlineMatchSessionCreate successfully returned 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate returned success but should have failed!");
		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);
*/

/* CAUSES RIP
	// -------------------------------------------
	// Non-zero attrib len, but zero attrib count
	// -------------------------------------------
	xStartVariation(hLog, "Non-zero attrib len, but zero attrib count");

	hr = XOnlineMatchSessionCreate(0, 0, 1000, hEvent, &hMatch );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_PASS, "XOnlineMatchSessionCreate successfully returned 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate returned success but should have failed!");
		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);
*/

/* CAUSES RIP
	// -------------------------------------------
	// Null phSession parameter
	// -------------------------------------------
	xStartVariation(hLog, "Null phSession parameter");

	hr = XOnlineMatchSessionCreate(0, 10, 1000, hEvent, NULL );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_PASS, "XOnlineMatchSessionCreate successfully returned 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate returned success but should have failed!");
		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);
*/

/* Removed until session creation bug is fixed
	// -------------------------------------------
	// Try to modify a session that doesn't exist
	// -------------------------------------------
	xStartVariation(hLog, "Try to modify a session that doesn't exist");

	hMatch = NULL;
	qwSessionID = 0;
	hr = XOnlineMatchSessionCreate(1, 0, 0, hEvent, &hMatch );
	if(!FAILED(hr))
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");
		
		hr = XOnlineMatchSessionSend(hMatch);
		if (!FAILED(hr))
		{
			hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, TRUE);
			if (FAILED(hr) && fCompleted)
			{
				pMatchHandle = (PXMATCH_EXTENDED_HANDLE) hMatch;
				qwSessionID = pMatchHandle->qwSessionID;

				if(!qwSessionID)
				{
					xLog(hLog, XLL_PASS, "Session creation failed with 0x%08x after %u polls", hr, dwPollCounter);
				}
				else
				{
					xLog(hLog, XLL_FAIL, "Session creation failed with 0x%08x after %u polls, but returned session ID %u", hr, dwPollCounter, qwSessionID);
				}
				
				hMatch = NULL;
			}
			else
			{
				if(fCompleted)
				{
					xLog(hLog, XLL_FAIL, "Session creation returned success, but should have failed");
					hMatch = NULL;
				}
				else
				{
					xLog(hLog, XLL_FAIL, "Session creation didn't complete after %u polls", dwPollCounter);
				}
			}
		}
		else
		{
			xLog(hLog, XLL_FAIL, "XOnlineMatchSessionSend failed with 0x%08x", hr);
			hMatch = NULL;
		}		
	}
	else
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}

	if(hMatch)
	{
		dwPollCounter = 0;
		XOnlineTaskCancel(hMatch, &dwWorkFlags);
		while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
			++dwPollCounter;
		}
		xLog(hLog, XLL_INFO, "Task was canceled after %u polls!", dwPollCounter);

		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);
*/

	// -------------------------------------------
	// Create a new session with no attributes
	// -------------------------------------------
	xStartVariation(hLog, "Create a new session with no attributes");

	hMatch = NULL;
	qwSessionID = 0;
	hr = XOnlineMatchSessionCreate(0, 0, 0, hEvent, &hMatch );
	if(!FAILED(hr))
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");
		
		hr = XOnlineMatchSessionSend(hMatch);
		if (!FAILED(hr))
		{
			hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, TRUE);
			if (!FAILED(hr) && fCompleted)
			{
				pMatchHandle = (PXMATCH_EXTENDED_HANDLE) hMatch;
				qwSessionID = pMatchHandle->qwSessionID;

				if(qwSessionID)
				{
					xLog(hLog, XLL_PASS, "Session creation succeeded after %u polls! (ID %u)", dwPollCounter, qwSessionID);
					hMatch = NULL;
				}
				else
				{
					xLog(hLog, XLL_FAIL, "Session creation succeeded after %u polls, but no session ID was returned.", dwPollCounter);
					hMatch = NULL;
				}
			}
			else
			{
				xLog(hLog, XLL_FAIL, "Session creation didn't complete successfully (0x%08x, completed %s)",
					hr, fCompleted ? "TRUE" : "FALSE");
				
				if(fCompleted)
					hMatch = NULL;
			}
		}
		else
		{
			xLog(hLog, XLL_FAIL, "XOnlineMatchSessionSend failed with 0x%08x", hr);
			hMatch = NULL;
		}		
	}
	else
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}

	if(hMatch)
	{
		dwPollCounter = 0;
		XOnlineTaskCancel(hMatch, &dwWorkFlags);
		while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
			++dwPollCounter;
		}
		xLog(hLog, XLL_INFO, "Task was canceled after %u polls!", dwPollCounter);

		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);


	// -------------------------------------------
	// Modify the session that was just created without adding attribute
	// -------------------------------------------
	xStartVariation(hLog, "Modify the session that was just created without adding attributes");

	hMatch = NULL;
	qwNewSessionID = 0;
	hr = XOnlineMatchSessionCreate(qwSessionID, 0, 0, hEvent, &hMatch );
	if(!FAILED(hr))
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");
		
		hr = XOnlineMatchSessionSend(hMatch);
		if (!FAILED(hr))
		{
			hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, TRUE);
			if (!FAILED(hr) && fCompleted)
			{
				pMatchHandle = (PXMATCH_EXTENDED_HANDLE) hMatch;
				qwNewSessionID = pMatchHandle->qwSessionID;

				if(qwNewSessionID == qwSessionID)
				{
					xLog(hLog, XLL_PASS, "Session modification succeeded after %u polls! (ID %u)", dwPollCounter, qwSessionID);
					hMatch = NULL;
				}
				else
				{
					xLog(hLog, XLL_FAIL, "Session modification succeeded after %u polls, but incorrect session ID was returned %u.",
						dwPollCounter, qwNewSessionID);
					hMatch = NULL;
				}
			}
			else
			{
				xLog(hLog, XLL_FAIL, "Session modification didn't complete successfully (0x%08x, completed %s)",
					hr, fCompleted ? "TRUE" : "FALSE");
				
				if(fCompleted)
					hMatch = NULL;
			}
		}
		else
		{
			xLog(hLog, XLL_FAIL, "XOnlineMatchSessionSend failed with 0x%08x", hr);
			hMatch = NULL;
		}		
	}
	else
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}

	if(hMatch)
	{
		dwPollCounter = 0;
		XOnlineTaskCancel(hMatch, &dwWorkFlags);
		while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
			++dwPollCounter;
		}
		xLog(hLog, XLL_INFO, "Task was canceled after %u polls!", dwPollCounter);

		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);

/* Bug 5485
	// -------------------------------------------
	// Use a NULL work event
	// -------------------------------------------
	xStartVariation(hLog, "Use a null work event");

	qwSessionID = 0;
	hr = XOnlineMatchSessionCreate(0, 0, 0, NULL, &hMatch );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");
		
		hr = XOnlineMatchSessionSend(hMatch);
		if (!FAILED(hr))
		{
			hr = PollTaskUntilComplete(hMatch, NULL, 30000, &dwPollCounter, &fCompleted, TRUE);
			if (!FAILED(hr) && fCompleted)
			{
				pMatchHandle = (PXMATCH_EXTENDED_HANDLE) hMatch;
				qwSessionID = pMatchHandle->qwSessionID;

				if(qwSessionID)
				{
					xLog(hLog, XLL_PASS, "Session creation succeeded after %u polls! (ID %u)", dwPollCounter, qwSessionID);
					hMatch = NULL;
				}
				else
				{
					xLog(hLog, XLL_FAIL, "Session creation succeeded after %u polls, but no session ID was returned.", dwPollCounter);
					hMatch = NULL;
				}
			}
			else
			{
				xLog(hLog, XLL_FAIL, "Session creation didn't complete successfully after %u polls (0x%08x, completed %s)",
					dwPollCounter, hr, fCompleted ? "TRUE" : "FALSE");
				
				if(fCompleted)
					hMatch = NULL;
			}
		}
		else
		{
			xLog(hLog, XLL_FAIL, "XOnlineMatchSessionSend failed with 0x%08x", hr);
			hMatch = NULL;
		}
	}

	if(hMatch)
	{
		dwPollCounter = 0;
		XOnlineTaskCancel(hMatch, &dwWorkFlags);
		while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
			++dwPollCounter;
		}
		xLog(hLog, XLL_INFO, "Task was canceled after %u polls!", dwPollCounter);

		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);
*/

/* No longer relevant now that session ID isn't passed back as an out parameter
	// -------------------------------------------
	// No session ID parameter
	// -------------------------------------------
	xStartVariation(hLog, "No session ID parameter");

	hr = XOnlineMatchSessionCreate(0, 0, 0, hEvent, NULL, &hMatch );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");
		
		hr = XOnlineMatchSessionSend(hMatch);
		if (!FAILED(hr))
		{
			hr = PollTaskUntilComplete(hMatch, NULL, 30000, &dwPollCounter, &fCompleted, TRUE);
			if (!FAILED(hr) && fCompleted)
			{
				xLog(hLog, XLL_PASS, "Session creation succeeded after %u polls!", dwPollCounter);
				hMatch = NULL;
			}
			else
			{
				xLog(hLog, XLL_FAIL, "Session creation didn't complete successfully after %u polls (0x%08x, completed %s)",
					dwPollCounter, hr, fCompleted ? "TRUE" : "FALSE");
				
				if(fCompleted)
					hMatch = NULL;
			}
		}
		else
		{
			xLog(hLog, XLL_FAIL, "XOnlineMatchSessionSend failed with 0x%08x", hr);
			hMatch = NULL;
		}
	}

	if(hMatch)
	{
		dwPollCounter = 0;
		XOnlineTaskCancel(hMatch, &dwWorkFlags);
		while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
			++dwPollCounter;
		}
		xLog(hLog, XLL_INFO, "Task was canceled after %u polls!", dwPollCounter);

		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);
*/

	// -------------------------------------------
	// Create a new session with one of each attribute type
	// -------------------------------------------
	xStartVariation(hLog, "Create a new session with one of each attribute type");

	qwSessionID = 0;
	hr = XOnlineMatchSessionCreate(0, 3, 1000, hEvent, &hMatch );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");

		AttributeTest[0].dwAttributeID = GLOBALINT_ID1;
		AttributeTest[0].Value.qwValue = INT1;
		AttributeTest[0].UserID.QuadPart = 0;
		AttributeTest[1].dwAttributeID = GLOBALSTR_ID1;
		AttributeTest[1].Value.pvValue = (VOID *) STRING1;
		AttributeTest[1].UserID.QuadPart = 0;
		AttributeTest[2].dwAttributeID = GLOBALBLB_ID1;
		AttributeTest[2].Value.pvValue = (VOID *) BLOB1;
		AttributeTest[2].UserID.QuadPart = 0;
		
		fRet = AddAttributes(hMatch, AttributeTest, 3);
		if(fRet)
		{
			hr = XOnlineMatchSessionSend(hMatch);
			if (!FAILED(hr))
			{
				hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, TRUE);
				if (!FAILED(hr) && fCompleted)
				{
					pMatchHandle = (PXMATCH_EXTENDED_HANDLE) hMatch;
					qwSessionID = pMatchHandle->qwSessionID;

					if(qwSessionID)
					{
						xLog(hLog, XLL_PASS, "Session creation succeeded after %u polls! (ID %u)", dwPollCounter, qwSessionID);
						hMatch = NULL;
					}
					else
					{
						xLog(hLog, XLL_FAIL, "Session creation succeeded after %u polls, but no session ID was returned.", dwPollCounter);
						hMatch = NULL;
					}
				}
				else
				{
					xLog(hLog, XLL_FAIL, "Session creation didn't complete successfully after %u polls (0x%08x, completed %s)",
						dwPollCounter, hr, fCompleted ? "TRUE" : "FALSE");
					
					if(fCompleted)
						hMatch = NULL;
				}
			}
			else
			{
				xLog(hLog, XLL_FAIL, "XOnlineMatchSessionSend failed with 0x%08x", hr);
				hMatch = NULL;
			}
		}
	}

	if(hMatch)
	{
		dwPollCounter = 0;
		XOnlineTaskCancel(hMatch, &dwWorkFlags);
		while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
			++dwPollCounter;
		}
		xLog(hLog, XLL_INFO, "Task was canceled after %u polls!", dwPollCounter);

		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);

	// -------------------------------------------
	// Modify the session that was just created adding attributes of each type
	// -------------------------------------------
	xStartVariation(hLog, "Modify the session that was just created adding attributes of each type");

	qwNewSessionID = 0;
	hr = XOnlineMatchSessionCreate(qwSessionID, 3, 1000, hEvent, &hMatch );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");

		AttributeTest[0].dwAttributeID = GLOBALINT_ID2;
		AttributeTest[0].Value.qwValue = INT1;
		AttributeTest[0].UserID.QuadPart = 0;
		AttributeTest[1].dwAttributeID = GLOBALSTR_ID2;
		AttributeTest[1].Value.pvValue = (VOID *) STRING1;
		AttributeTest[1].UserID.QuadPart = 0;
		AttributeTest[2].dwAttributeID = GLOBALBLB_ID2;
		AttributeTest[2].Value.pvValue = (VOID *) BLOB1;
		AttributeTest[2].UserID.QuadPart = 0;
		
		fRet = AddAttributes(hMatch, AttributeTest, 3);
		if(fRet)
		{
			hr = XOnlineMatchSessionSend(hMatch);
			if (!FAILED(hr))
			{
				hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, TRUE);
				if (!FAILED(hr) && fCompleted)
				{
					pMatchHandle = (PXMATCH_EXTENDED_HANDLE) hMatch;
					qwNewSessionID = pMatchHandle->qwSessionID;

					if(qwNewSessionID == qwSessionID)
					{
						xLog(hLog, XLL_PASS, "Session modification succeeded after %u polls! (ID %u)", dwPollCounter, qwSessionID);
						hMatch = NULL;
					}
					else
					{
						xLog(hLog, XLL_FAIL, "Session modification succeeded after %u polls, but incorrect session ID was returned.",
							dwPollCounter, qwNewSessionID);
						hMatch = NULL;
					}
				}
				else
				{
					xLog(hLog, XLL_FAIL, "Session modification didn't complete successfully after %u polls (0x%08x, completed %s)",
						dwPollCounter, hr, fCompleted ? "TRUE" : "FALSE");
					
					if(fCompleted)
						hMatch = NULL;
				}
			}
			else
			{
				xLog(hLog, XLL_FAIL, "XOnlineMatchSessionSend failed with 0x%08x", hr);
				hMatch = NULL;
			}
		}
	}

	if(hMatch)
	{
		dwPollCounter = 0;
		XOnlineTaskCancel(hMatch, &dwWorkFlags);
		while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
			++dwPollCounter;
		}
		xLog(hLog, XLL_INFO, "Task was canceled after %u polls!", dwPollCounter);

		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);

	// -------------------------------------------
	// Modify session and re-add existing attributes
	// -------------------------------------------
	xStartVariation(hLog, "Modify session and re-add existing attributes");

	qwNewSessionID = 0;
	hr = XOnlineMatchSessionCreate(qwSessionID, 3, 1000, hEvent, &hMatch );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");

		AttributeTest[0].dwAttributeID = GLOBALINT_ID2;
		AttributeTest[0].Value.qwValue = INT1;
		AttributeTest[0].UserID.QuadPart = 0;
		AttributeTest[1].dwAttributeID = GLOBALSTR_ID2;
		AttributeTest[1].Value.pvValue = (VOID *) STRING1;
		AttributeTest[1].UserID.QuadPart = 0;
		AttributeTest[2].dwAttributeID = GLOBALBLB_ID2;
		AttributeTest[2].Value.pvValue = (VOID *) BLOB1;
		AttributeTest[2].UserID.QuadPart = 0;
		
		fRet = AddAttributes(hMatch, AttributeTest, 3);
		if(fRet)
		{
			hr = XOnlineMatchSessionSend(hMatch);
			if (!FAILED(hr))
			{
				hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, TRUE);
				if (!FAILED(hr) && fCompleted)
				{
					pMatchHandle = (PXMATCH_EXTENDED_HANDLE) hMatch;
					qwNewSessionID = pMatchHandle->qwSessionID;

					if(qwNewSessionID == qwSessionID)
					{
						xLog(hLog, XLL_PASS, "Session modification succeeded after %u polls! (ID %u)", dwPollCounter, qwSessionID);
						hMatch = NULL;
					}
					else
					{
						xLog(hLog, XLL_FAIL, "Session modification succeeded after %u polls, but incorrect session ID was returned.",
							dwPollCounter, qwNewSessionID);
						hMatch = NULL;
					}
				}
				else
				{
					xLog(hLog, XLL_FAIL, "Session modification didn't complete successfully after %u polls (0x%08x, completed %s)",
						dwPollCounter, hr, fCompleted ? "TRUE" : "FALSE");
					
					if(fCompleted)
						hMatch = NULL;
				}
			}
			else
			{
				xLog(hLog, XLL_FAIL, "XOnlineMatchSessionSend failed with 0x%08x", hr);
				hMatch = NULL;
			}
		}
	}

	if(hMatch)
	{
		dwPollCounter = 0;
		XOnlineTaskCancel(hMatch, &dwWorkFlags);
		while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
			++dwPollCounter;
		}
		xLog(hLog, XLL_INFO, "Task was canceled after %u polls!", dwPollCounter);

		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}

	xEndVariation(hLog);

	// -------------------------------------------
	// Open and close an XMatch request without calling the work pump
	// -------------------------------------------
	xStartVariation(hLog, "Open and close an XMatch request without calling the work pump");

	qwSessionID = 0;
	hr = XOnlineMatchSessionCreate(0, 1, 100, hEvent, &hMatch );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");

		dwPollCounter = 0;
		XOnlineTaskCancel(hMatch, &dwWorkFlags);
		while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
			++dwPollCounter;
		}
		xLog(hLog, XLL_INFO, "Task was canceled after %u polls!", dwPollCounter);

		hr = XOnlineTaskCloseHandle(hMatch);
		if(FAILED(hr))
		{
			xLog(hLog, XLL_FAIL, "XOnlineTaskCloseHandle failed with 0x%08x", hr);
		}
		else
		{
			pMatchHandle = (PXMATCH_EXTENDED_HANDLE) hMatch;
			qwSessionID = pMatchHandle->qwSessionID;

			if(!qwSessionID)
			{
				xLog(hLog, XLL_PASS, "Task was cancelled and closed successfully.");
			}
			else
			{
				xLog(hLog, XLL_FAIL, "Task was cancelled and closed, but session ID was set to %u", qwSessionID);
			}
			hMatch = NULL;
		}
	}
	

	xEndVariation(hLog);

/* Removed until 6229 is fixed
	// -------------------------------------------
	// Call work pump immediately after creating a session request
	// -------------------------------------------
	xStartVariation(hLog, "Open a session request and immediately call work pump");

	hr = XOnlineMatchSessionCreate(0, 1, 100, hEvent, &hMatch );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");

		dwPollCounter = 1;
		hr = XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
		if(!FAILED(hr))
		{
			xLog(hLog, XLL_FAIL, "XOnlineTaskContinue should have failed!");
		}
		else
		{
			xLog(hLog, XLL_PASS, "XOnlineTaskContinue successfully returned 0x%08x", hr);
		}

		while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
		{
			XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
			++dwPollCounter;
		}
		xLog(hLog, XLL_INFO, "Task completed after %u polls!", dwPollCounter);
		XOnlineTaskCloseHandle(hMatch);
		hMatch = NULL;
	}
	
	xEndVariation(hLog);
*/

	// -------------------------------------------
	// Cancel a session request that has already completed and then close it
	// -------------------------------------------
	xStartVariation(hLog, "Cancel a session request that has already completed and then close it");

	qwSessionID = 0;
	hr = XOnlineMatchSessionCreate(0, 0, 0, hEvent, &hMatch );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "XOnlineMatchSessionCreate failed with 0x%08x", hr);
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineMatchSessionCreate returned success!");
		
		hr = XOnlineMatchSessionSend(hMatch);
		if (!FAILED(hr))
		{
			do
			{
				XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
				++dwPollCounter;
			}
			while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags));
		}
		else
		{
			xLog(hLog, XLL_FAIL, "XOnlineMatchSessionSend failed with 0x%08x", hr);
			hMatch = NULL;
		}
	}

	if(hMatch)
	{
		dwPollCounter = 0;
		hr = XOnlineTaskCancel(hMatch, &dwWorkFlags);
		if(hr == E_ABORT)
		{
			while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
			{
				XOnlineTaskContinue(hMatch, 0, &dwWorkFlags);
				++dwPollCounter;
			}
			xLog(hLog, XLL_INFO, "Task was canceled after %u polls!", dwPollCounter);
			
			hr = XOnlineTaskCloseHandle(hMatch);
			if(!FAILED(hr))
			{
				xLog(hLog, XLL_PASS, "Successfully canceled and closed a completed session handle");
			}
			else
			{
				xLog(hLog, XLL_FAIL, "XOnlineTaskCloseHandle failed with 0x%08x", hr);
			}
		}
		else
		{
			xLog(hLog, XLL_FAIL, "XOnlineTaskCancel didn't return E_ABORT (0x%08x)", hr);

			hr = XOnlineTaskCloseHandle(hMatch);
		}
		
		hMatch = NULL;
	}

	xEndVariation(hLog);

Exit:
	if(hEvent)
		CloseHandle(hEvent);

	if(!fCompleted && hMatch)
		XOnlineTaskCloseHandle(hMatch);

	return hr;
} // XMatchTest_OpenSession


} // namespace XMatchTestNamespace