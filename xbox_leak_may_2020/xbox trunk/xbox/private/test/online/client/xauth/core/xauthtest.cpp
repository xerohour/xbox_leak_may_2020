//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include "xauthtest.h"

using namespace XAuthTestNamespace;

namespace XAuthTestNamespace {

//==================================================================================
// Globals
//==================================================================================
static CRITICAL_SECTION g_csSerializeAccess;
HANDLE g_hHeap = NULL;

//==================================================================================
// PollTaskUntilComplete
//----------------------------------------------------------------------------------
//
// Description: Calls the XOnlineTaskComplete function until either the given
//    task has been completed or the amount of available time has expired
//
// Arguments:
//	XONLINETASK_HANDLE	hAsyncHandle		Handle to async task
//	HANDLE				hWorkToDoEvent		Event that is associated with this async task.
//											This can be set to NULL or INVALID_HANDLE_VALUE
//											if no event is associated with the task
//	DWORD				dwMaxWaitTime		Maximum amount of time to wait for the task to
//											complete. This can be set to INFINITE to block
//											until completion
//	DWORD				*pdwPollCounter		Optional counter passed in to keep track of the
//											number of calls made to the XOnlineTaskDoWork2 function
//	BOOL				*pfCompleted		Optional boolean passed in to keep track if whether
//											the task completed or not
//  BOOL				fCloseOnComplete	Indicates whether the function should close a handle
//											after it indicates completion
//
// Returns: The last result code from XOnlineTaskDoWork2 is returned.  If a valid pointer
//   is provided for pfCompleted, it is updated to reflect whether the task has completed or not
//==================================================================================
HRESULT PollTaskUntilComplete(XONLINETASK_HANDLE hAsyncHandle, HANDLE hWorkToDoEvent, DWORD dwMaxWaitTime, DWORD *pdwPollCounter, BOOL *pfCompleted, BOOL fCloseOnComplete)
{
	HRESULT hr = S_OK;
	DWORD dwLastTime = 0, dwCurrentTime = 0, dwCurrentDuration = 0, dwWaitTimeLeft = dwMaxWaitTime;
	DWORD dwWorkFlags = 0, dwInternalPollCounter = 0;

	if(pfCompleted)
		*pfCompleted = FALSE;

	dwLastTime = GetTickCount();

	while(TRUE)
	{

		if((hWorkToDoEvent != INVALID_HANDLE_VALUE) && (hWorkToDoEvent != NULL))
			WaitForSingleObject(hWorkToDoEvent, dwWaitTimeLeft);

//        hr = XOnlineTaskContinue(hAsyncHandle, 0, &dwWorkFlags);
        hr = XOnlineTaskDoWork2(hAsyncHandle, 0);

		if(FAILED(hr))
			return hr;

		// Update the poll-counter
		++dwInternalPollCounter;

        if (XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
        {
			if(fCloseOnComplete)
				XOnlineTaskClose(hAsyncHandle);
//				XOnlineTaskCloseHandle(hAsyncHandle);

			if(pfCompleted)
				*pfCompleted = TRUE;

            break;
        }
		
		if(dwMaxWaitTime != INFINITE)
		{
			// See if we have gone beyond the maximum allowed time
			dwCurrentTime = GetTickCount();
			if(dwCurrentTime < dwLastTime)
			{
				dwCurrentDuration = ((MAXDWORD - dwLastTime) + dwCurrentTime);
			}
			else
			{
				dwCurrentDuration = (dwCurrentTime - dwLastTime);
			}
			
			if(dwCurrentDuration >= dwWaitTimeLeft)
			{
				hr = E_FAIL;
				goto Exit;
			}
			
			dwWaitTimeLeft -= dwCurrentDuration;
			dwLastTime = dwCurrentTime;
		}
	}

Exit:
	if(pdwPollCounter)
		*pdwPollCounter = dwInternalPollCounter;

	return hr;
}


//==================================================================================
// XAuthTestDllMain
//----------------------------------------------------------------------------------
//
// Description: DLL entry
//
// Arguments:
//	HINSTANCE	hInstance		Handle to module
//	DWORD		dwReason		Indicates the reason for calling the function
//	LPVOID		lpContext		reserved
// Returns:
//	TRUE on success
//==================================================================================
BOOL WINAPI XAuthTestDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
	WSADATA WSAData;

	// We'll initialize/delete the global critical section here
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		XNetAddRef();
		WSAStartup(MAKEWORD(2, 2), &WSAData);
		InitializeCriticalSection(&g_csSerializeAccess);
		g_hHeap = HeapCreate(0,0,0);
		break;
	case DLL_PROCESS_DETACH:
		WSACleanup();
		XNetRelease();
		DeleteCriticalSection(&g_csSerializeAccess);
		HeapDestroy(g_hHeap);
		g_hHeap = NULL;
		break;
	default:
		break;
	}

    return TRUE;
}

//------------ tests start below ---------------------------------------------------

VOID WINAPI xauthtest_XOnlineHasMachineAccount(HANDLE hLog)
{
	START_TESTCASE("Look for machine account when it isn't present");

	//make sure no accounts are present
    XOnlineSetupTempAccounts( FALSE, XONLINE_LOCAL_HARD_DRIVE, 0 );

	if( XOnlineHasMachineAccount() == FALSE )
	{
		PASS_TESTCASE("XOnlineHasMachineAccount didn't find machine account" );
	}
	else
	{
		FAIL_TESTCASE("XOnlineHasMachineAccount found machine account" );
	}

	END_TESTCASE();

	START_TESTCASE("Look for machine account when it is present");

	//make sure accounts are present
	XOnlineSetupTempAccounts( TRUE, XONLINE_LOCAL_HARD_DRIVE, XONLINE_MAX_STORED_ONLINE_USERS );

	if( XOnlineHasMachineAccount() == FALSE )
	{
		FAIL_TESTCASE("XOnlineHasMachineAccount didn't find machine account" );
	}
	else
	{
		PASS_TESTCASE("XOnlineHasMachineAccount found machine account" );
	}

	END_TESTCASE();

	return;
}

VOID WINAPI xauthtest_XOnlineGetUsers(HANDLE hLog)
{
	HRESULT hr;
	XONLINE_USER UsersArray[XONLINE_MAX_STORED_ONLINE_USERS];
	DWORD dwNumUsers;

	//make sure accounts are not present
	XOnlineSetupTempAccounts( FALSE, XONLINE_LOCAL_HARD_DRIVE, 0 );

	//failure case #1; call with an unmounted MMU
	//  at this point there is no way to get accounts onto the MMU
	


	START_TESTCASE("Get user accounts when none are present");

	//failure case #2; no online users
	hr = XOnlineGetUsers( XONLINE_LOCAL_HARD_DRIVE, UsersArray, &dwNumUsers );
	
	xLog(hLog, XLL_INFO, "XOnlineGetUsers returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlineGetUsers failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineGetUsers returned success, but should have failed");
	}

	END_TESTCASE();



	//for remaining cases accounts must exist
	XOnlineSetupTempAccounts( TRUE, XONLINE_LOCAL_HARD_DRIVE, 8 );


/* Causes RIP
	START_TESTCASE("Get user accounts with NULL pUsers");

	//failure case #3; call with NULL pUsers;
	hr = XOnlineGetUsers( XONLINE_LOCAL_HARD_DRIVE, NULL, &dwNumUsers );

	xLog(hLog, XLL_INFO, "XOnlineGetUsers returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlineGetUsers failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineGetUsers returned success, but should have failed");
	}

	END_TESTCASE();
*/

/* TEMP Occasionally cause break
	START_TESTCASE("Get user accounts with invalid chDrive");

	//failure case #4; call with invalid chDrive
	hr = XOnlineGetUsers( 'F', UsersArray, &dwNumUsers );

	xLog(hLog, XLL_INFO, "XOnlineGetUsers returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlineGetUsers failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineGetUsers returned success, but should have failed");
	}

	END_TESTCASE();
*/

/* Causes RIP
	START_TESTCASE("Get user accounts with NULL pcUsers");

	//failure case #5; call with NULL pcUsers
	hr = XOnlineGetUsers( XONLINE_LOCAL_HARD_DRIVE, UsersArray, NULL );

	xLog(hLog, XLL_INFO, "XOnlineGetUsers returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlineGetUsers failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineGetUsers returned success, but should have failed");
	}

	END_TESTCASE();
*/


	//success case #1; call with mounted MMU and user accounts
	//  at this point there is no way to get accounts onto the MMU




	START_TESTCASE("Get user accounts off of hard-drive");

	//success case #2; call with local hd and user accounts
	hr = XOnlineGetUsers( XONLINE_LOCAL_HARD_DRIVE, UsersArray, &dwNumUsers );

	xLog(hLog, XLL_INFO, "XOnlineGetUsers returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		FAIL_TESTCASE("XOnlineGetUsers failed unexpectedly");
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineGetUsers found %u users", dwNumUsers);
		PASS_TESTCASE("XOnlineGetUsers succeeded");
	}

	END_TESTCASE();

	return;
}

VOID WINAPI xauthtest_XOnlinePINStartInput(HANDLE hLog)
{
	XINPUT_STATE inputState;
	XPININPUTHANDLE hPinInput;
	

/* Causes RIP
	START_TESTCASE("Start PIN input with NULL input state");

	//failure case #1; call with NULL pInputState
	hPinInput = XOnlinePINStartInput( NULL );

	xLog(hLog, XLL_INFO, "XOnlinePINStartInput returned 0x%08x", hPinInput);
	if( hPinInput == NULL )
	{
		PASS_TESTCASE("XOnlinePINStartInput failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlinePINStartInput returned success but should have failed");
	}

	END_TESTCASE();
*/

	START_TESTCASE("Start PIN input with valid input state");

	//success case #1; call with a valid pInputState
	//
	//set XINPUT_STATE values
	inputState.dwPacketNumber = 1;
	inputState.Gamepad.wButtons = 0;
	inputState.Gamepad.bAnalogButtons[0] = 0;
	inputState.Gamepad.bAnalogButtons[1] = 0;
	inputState.Gamepad.bAnalogButtons[2] = 0;
	inputState.Gamepad.bAnalogButtons[3] = 0;
	inputState.Gamepad.bAnalogButtons[4] = 0;
	inputState.Gamepad.bAnalogButtons[5] = 0;
	inputState.Gamepad.bAnalogButtons[6] = 0;
	inputState.Gamepad.bAnalogButtons[7] = 0;
	inputState.Gamepad.sThumbLX = 0;
	inputState.Gamepad.sThumbLY = 0;
	inputState.Gamepad.sThumbRX = 0;
	inputState.Gamepad.sThumbRY = 0;

	hPinInput = XOnlinePINStartInput( &inputState );

	xLog(hLog, XLL_INFO, "XOnlinePINStartInput returned 0x%08x", hPinInput);
	if( hPinInput != NULL )
	{
		PASS_TESTCASE("XOnlinePINStartInput succeeded");
	}
	else
	{
		FAIL_TESTCASE("XOnlinePINStartInput failed unexpectedly");
	}

	END_TESTCASE();

	return;
}


VOID WINAPI xauthtest_XOnlinePINDecodeInput(HANDLE hLog)
{
	HRESULT hr;
	XINPUT_STATE inputState;
	BYTE pinByte;
	XPININPUTHANDLE hPinInput;

	//set initial XINPUT_STATE
	inputState.dwPacketNumber = 10;
	inputState.Gamepad.wButtons = 0;
	inputState.Gamepad.bAnalogButtons[0] = 0;
	inputState.Gamepad.bAnalogButtons[1] = 0;
	inputState.Gamepad.bAnalogButtons[2] = 0;
	inputState.Gamepad.bAnalogButtons[3] = 0;
	inputState.Gamepad.bAnalogButtons[4] = 0;
	inputState.Gamepad.bAnalogButtons[5] = 0;
	inputState.Gamepad.bAnalogButtons[6] = 0;
	inputState.Gamepad.bAnalogButtons[7] = 0;
	inputState.Gamepad.sThumbLX = 0;
	inputState.Gamepad.sThumbLY = 0;
	inputState.Gamepad.sThumbRX = 0;
	inputState.Gamepad.sThumbRY = 0;

	//get pin handle needed for further decoding
	hPinInput = XOnlinePINStartInput( &inputState );
	
	xLog(hLog, XLL_INFO, "XOnlinePINStartInput returned 0x%08x", hPinInput);
	if(!hPinInput)
	{
		xLog(hLog, XLL_FAIL, "Couldn't start PIN input for decoding tests");
		return;
	}

/* Causes RIP
	START_TESTCASE("Decode PIN input with NULL input handle");

	//failure case #1; call with NULL handle
	hr = XOnlinePINDecodeInput( NULL, &inputState, &pinByte );
	
	xLog(hLog, XLL_INFO, "XOnlinePINDecodeInput returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlinePINDecodeInput failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlinePINDecodeInput returned success but should have failed");
	}
	
	END_TESTCASE();
*/


/* Causes RIP
	START_TESTCASE("Decode PIN input with NULL PIN byte pointer");

	//failure case #2; call with NULL pPINByte
	//
	//advance dwPacketNumber
	inputState.dwPacketNumber = 15;
	hr = XOnlinePINDecodeInput( hPinInput, &inputState, NULL );
	
	xLog(hLog, XLL_INFO, "XOnlinePINDecodeInput returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlinePINDecodeInput failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlinePINDecodeInput returned success but should have failed");
	}
	
	END_TESTCASE();
*/

/* Causes RIP
	START_TESTCASE("Decode PIN input with NULL PIN input state");

	//failure case #3; call with NULL pInputState
	hr = XOnlinePINDecodeInput( hPinInput, NULL, &pinByte );
	
	xLog(hLog, XLL_INFO, "XOnlinePINDecodeInput returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlinePINDecodeInput failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlinePINDecodeInput returned success but should have failed");
	}
	
	END_TESTCASE();
*/

	//failure case #4, no longer applies as the game code checks PIN values
	//failure case #5, TBC
	


	START_TESTCASE("Decode PIN input with changed input state");

	//success case #1; call with valid arguments
	//change the input state for decoding to work
	inputState.dwPacketNumber = 20;
	inputState.Gamepad.bAnalogButtons[5] = 0;
	hr = XOnlinePINDecodeInput( hPinInput, &inputState, &pinByte );
	
	xLog(hLog, XLL_INFO, "XOnlinePINDecodeInput returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		FAIL_TESTCASE("XOnlinePINDecodeInput failed unexpectedly");
	}
	else
	{
		PASS_TESTCASE("XOnlinePINDecodeInput succeeded");
	}
	
	END_TESTCASE();



	//close to hPinInput to conserve resources
	hr = XOnlinePINEndInput( hPinInput );
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "Couldn't end PIN input after decoding tests");
		return;
	}

	return;
}

VOID WINAPI xauthtest_XOnlinePINEndInput(HANDLE hLog)
{
	HRESULT	hr;
	XINPUT_STATE inputState;
	BYTE pinByte;
	XPININPUTHANDLE hPinInput;

	//set initial XINPUT_STATE
	inputState.dwPacketNumber = 10;
	inputState.Gamepad.wButtons = 0;
	inputState.Gamepad.bAnalogButtons[0] = 0;
	inputState.Gamepad.bAnalogButtons[1] = 0;
	inputState.Gamepad.bAnalogButtons[2] = 0;
	inputState.Gamepad.bAnalogButtons[3] = 0;
	inputState.Gamepad.bAnalogButtons[4] = 0;
	inputState.Gamepad.bAnalogButtons[5] = 0;
	inputState.Gamepad.bAnalogButtons[6] = 0;
	inputState.Gamepad.bAnalogButtons[7] = 0;
	inputState.Gamepad.sThumbLX = 0;
	inputState.Gamepad.sThumbLY = 0;
	inputState.Gamepad.sThumbRX = 0;
	inputState.Gamepad.sThumbRY = 0;

	//get pin handle needed for testing closing
	hPinInput = XOnlinePINStartInput( &inputState );

	xLog(hLog, XLL_INFO, "XOnlinePINStartInput returned 0x%08x", hPinInput);
	if(!hPinInput)
	{
		xLog(hLog, XLL_FAIL, "Couldn't start PIN input for decoding tests");
		return;
	}


/* Causes RIP
	START_TESTCASE("End PIN input with NULL input handle");

	//failure case #1; call with NULL handle
	hr = XOnlinePINEndInput( NULL );

	xLog(hLog, XLL_INFO, "XOnlinePINEndInput returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlinePINEndInput failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlinePINEndInput returned success but should have failed");
	}

	END_TESTCASE();
*/


	START_TESTCASE("End PIN input with valid input handle");

	//success case #1; call with a valid handle
	hr = XOnlinePINEndInput( hPinInput );

	xLog(hLog, XLL_INFO, "XOnlinePINEndInput returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		FAIL_TESTCASE("XOnlinePINEndInput failed unexpectedly");
	}
	else
	{
		PASS_TESTCASE("XOnlinePINEndInput succeeded");
	}

	END_TESTCASE();

	return;
}

VOID WINAPI xauthtest_XOnlineLogon(HANDLE hLog)
{
	HRESULT hr;
	XONLINE_SERVICE ServicesArray[] =
	{
		{ XONLINE_CATALOG_REFERRAL_SERVICE, S_OK, NULL },
		{ XONLINE_DOWNLOAD_SERVICE, S_OK, NULL },
		{ XONLINE_UPLOAD_SERVICE, S_OK, NULL },
		{ XONLINE_MATCHMAKING_SERVICE, S_OK, NULL }
	};
	XONLINE_USER UsersArray[XONLINE_MAX_STORED_ONLINE_USERS];
	DWORD dwNumUsers;
	XONLINETASK_HANDLE hLogon = NULL;
	DWORD cServices = sizeof(ServicesArray) / sizeof(XONLINE_SERVICE);
	HANDLE hEvent = NULL;
	BOOL fCompleted = TRUE;

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
	{
		xLog(hLog, XLL_FAIL, "Couldn't create event for logon tests");
		goto Exit;
	}


	//make sure there are temporary accounts
	XOnlineSetupTempAccounts( TRUE, XONLINE_LOCAL_HARD_DRIVE, 8 );
	//obtain these accounts
	hr = XOnlineGetUsers( XONLINE_LOCAL_HARD_DRIVE, UsersArray, &dwNumUsers );

	xLog(hLog, XLL_INFO, "XOnlineGetUsers returned 0x%08x", hr);
	if(FAILED(hr))
	{
		xLog(hLog, XLL_FAIL, "Couldn't get local users for logon tests");
		goto Exit;
	}



	START_TESTCASE("Logon with NULL users pointer");

	//failure case #1; call with NULL pUsers pointer
	hr = XOnlineLogon( NULL, ServicesArray, cServices, NULL, &hLogon );

	xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlineLogon failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineLogon returned success but should have failed");
	}

	END_TESTCASE();


/* TEMP still causes crash
	START_TESTCASE("Logon with NULL services pointer");

	//failure case #2; call with NULL pServices
	hr = XOnlineLogon( UsersArray, NULL, 1, NULL, &hLogon );

	xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlineLogon failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineLogon returned success but should have failed");
	}

	END_TESTCASE();
*/

/* Causes RIP
	START_TESTCASE("Logon with 0 services count");

	//failure case #3; call with 0 count of services
	hr = XOnlineLogon( UsersArray, ServicesArray, 0, NULL, &hLogon );

	xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlineLogon failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineLogon returned success but should have failed");
	}

	END_TESTCASE();
*/


	//failure case #4; call when there is an update
	//at this time I cannot force a required update

	//failure case #5; not loger applies, Logon is not thread safe

	//failure cases #6, #7, #8, #9 will be implemented later



	START_TESTCASE("Logon with duplicate users");

	//failure case #10; duplicate accounts
	memcpy( UsersArray + 1, UsersArray, sizeof(XONLINE_USER) );
	hr = XOnlineLogon( UsersArray, ServicesArray, cServices, NULL, &hLogon );

	xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		PASS_TESTCASE("XOnlineLogon failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineLogon returned success but should have failed");
	}

	END_TESTCASE();



	START_TESTCASE("Logon with one local users");

	//success case #1; call with valid arguments
	// recreate accounts array
	XOnlineSetupTempAccounts( TRUE, XONLINE_LOCAL_HARD_DRIVE, 1 );
	XOnlineGetUsers( XONLINE_LOCAL_HARD_DRIVE, UsersArray, &dwNumUsers );
	hr = XOnlineLogon( UsersArray, ServicesArray, cServices, hEvent, &hLogon );

	xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		FAIL_TESTCASE("XOnlineLogon failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hLogon, hEvent, 30000, NULL, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
		FAIL_TESTCASE("XOnlineLogon failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("XOnlineLogon didn't complete after 30 seconds");
	}

	PASS_TESTCASE("XOnlineLogon succeeded");

	END_TESTCASE();
	CLEANUP_TASK(hLogon);


	START_TESTCASE("Logon with all local users");

	//success case #2; call with valid argument and maximum num of users
	XOnlineSetupTempAccounts( TRUE, XONLINE_LOCAL_HARD_DRIVE, XONLINE_MAX_STORED_ONLINE_USERS );
	XOnlineGetUsers( XONLINE_LOCAL_HARD_DRIVE, UsersArray, &dwNumUsers );
	hr = XOnlineLogon( UsersArray, ServicesArray, cServices, hEvent, &hLogon );

	xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		FAIL_TESTCASE("XOnlineLogon failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hLogon, hEvent, 30000, NULL, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
		FAIL_TESTCASE("XOnlineLogon failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("XOnlineLogon didn't complete after 30 seconds");
	}

	PASS_TESTCASE("XOnlineLogon succeeded");

	END_TESTCASE();
	CLEANUP_TASK(hLogon);

Exit:

	if(hEvent)
		CloseHandle(hEvent);

	return;
}

VOID WINAPI xauthtest_XOnlineGetServiceInfo(HANDLE hLog)
{
	HRESULT hr = S_OK;
	PXONLINE_SERVICE_INFO pServiceInfo = NULL;
	XONLINE_SERVICE ServicesArray[] =
	{
		{ XONLINE_CATALOG_REFERRAL_SERVICE, S_OK, NULL },
		{ XONLINE_DOWNLOAD_SERVICE, S_OK, NULL },
		{ XONLINE_UPLOAD_SERVICE, S_OK, NULL },
		{ XONLINE_MATCHMAKING_SERVICE, S_OK, NULL }
	};
	XONLINE_USER UsersArray[XONLINE_MAX_STORED_ONLINE_USERS];
	DWORD dwNumUsers;
	XONLINETASK_HANDLE hLogon = NULL;
	DWORD cServices = sizeof(ServicesArray) / sizeof(XONLINE_SERVICE);
	HANDLE hEvent = NULL;
	BOOL fCompleted = TRUE;

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
	{
		xLog(hLog, XLL_FAIL, "Couldn't create event for logon tests");
		goto Exit;
	}

	START_TESTCASE("Get service info with NULL service handle");

	pServiceInfo = XOnlineGetServiceInfo(NULL);

	xLog(hLog, XLL_INFO, "XOnlineGetServiceInfo returned 0x%08x", hr);
	if(!pServiceInfo)
	{
		PASS_TESTCASE("XOnlineGetServiceInfo failed as expected");
	}
	else
	{
		FAIL_TESTCASE("XOnlineGetServiceInfo returned success but should have failed");
	}

	END_TESTCASE();



	START_TESTCASE("Get service info for all services");

	//success case #1; call with all services
	XOnlineSetupTempAccounts( TRUE, XONLINE_MAX_STORED_ONLINE_USERS );
	XOnlineGetUsers( XONLINE_LOCAL_HARD_DRIVE, UsersArray, &dwNumUsers );
	hr = XOnlineLogon( UsersArray, ServicesArray, cServices, hEvent, &hLogon );

	xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		FAIL_TESTCASE("XOnlineLogon failed unexpectedly");
	}

	hr = PollTaskUntilComplete(hLogon, hEvent, 30000, NULL, &fCompleted, FALSE);
	if(FAILED(hr))
	{
		xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
		FAIL_TESTCASE("XOnlineLogon failed unexpectedly");
	}
	else if (!fCompleted)
	{
		FAIL_TESTCASE("XOnlineLogon didn't complete after 30 seconds");
	}

	// Get catalog service info
	pServiceInfo = XOnlineGetServiceInfo(ServicesArray[0].handle);

	xLog(hLog, XLL_INFO, "XOnlineGetServiceInfo returned 0x%08x", hr);
	if(!pServiceInfo)
	{
		FAIL_TESTCASE("XOnlineGetServiceInfo failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "Service 1 (Name %s, Address 0x%08x, Port 0x%04x",
		pServiceInfo->serviceName, pServiceInfo->dwServiceIP, pServiceInfo->wServicePort);

	// Get catalog service info
	pServiceInfo = XOnlineGetServiceInfo(ServicesArray[1].handle);

	xLog(hLog, XLL_INFO, "XOnlineGetServiceInfo returned 0x%08x", hr);
	if(!pServiceInfo)
	{
		FAIL_TESTCASE("XOnlineGetServiceInfo failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "Service 2 (Name %s, Address 0x%08x, Port 0x%04x",
		pServiceInfo->serviceName, pServiceInfo->dwServiceIP, pServiceInfo->wServicePort);

	// Get catalog service info
	pServiceInfo = XOnlineGetServiceInfo(ServicesArray[2].handle);

	xLog(hLog, XLL_INFO, "XOnlineGetServiceInfo returned 0x%08x", hr);
	if(!pServiceInfo)
	{
		FAIL_TESTCASE("XOnlineGetServiceInfo failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "Service 3 (Name %s, Address 0x%08x, Port 0x%04x",
		pServiceInfo->serviceName, pServiceInfo->dwServiceIP, pServiceInfo->wServicePort);

	// Get catalog service info
	pServiceInfo = XOnlineGetServiceInfo(ServicesArray[3].handle);

	xLog(hLog, XLL_INFO, "XOnlineGetServiceInfo returned 0x%08x", hr);
	if(!pServiceInfo)
	{
		FAIL_TESTCASE("XOnlineGetServiceInfo failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "Service 4 (Name %s, Address 0x%08x, Port 0x%04x",
		pServiceInfo->serviceName, pServiceInfo->dwServiceIP, pServiceInfo->wServicePort);

	PASS_TESTCASE("Retrieved info for all services");

	END_TESTCASE();
	CLEANUP_TASK(hLogon);

	START_TESTCASE("Get service info for all services");




	END_TESTCASE();

Exit:

	if(hEvent)
		CloseHandle(hEvent);

	return;
}

//==================================================================================
// XAuthTestStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI XAuthTestStartTest(IN HANDLE  hLog)
{
	xSetOwnerAlias(hLog, "tristanj");
    xSetComponent(hLog, "Online", "Authentication - Client (O)");

	xSetFunctionName(hLog, "XOnlineHasMachineAccount");
	xauthtest_XOnlineHasMachineAccount(hLog);
	xSetFunctionName(hLog, "");

	xSetFunctionName(hLog, "XOnlineGetUsers");
	xauthtest_XOnlineGetUsers(hLog);
	xSetFunctionName(hLog, "");

	xSetFunctionName(hLog, "XOnlinePINStartInput");
	xauthtest_XOnlinePINStartInput(hLog);
	xSetFunctionName(hLog, "");

	xSetFunctionName(hLog, "XOnlinePINDecodeInput");
	xauthtest_XOnlinePINDecodeInput(hLog);
	xSetFunctionName(hLog, "");

	xSetFunctionName(hLog, "XOnlinePINEndInput");
	xauthtest_XOnlinePINEndInput(hLog);
	xSetFunctionName(hLog, "");

	xSetFunctionName(hLog, "XOnlineLogon");
	xauthtest_XOnlineLogon(hLog);
	xSetFunctionName(hLog, "");

	xSetFunctionName(hLog, "XOnlineGetServiceInfo");
	xauthtest_XOnlineGetServiceInfo(hLog);
	xSetFunctionName(hLog, "");

	return;
}

//==================================================================================
// XAuthTestEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI XAuthTestEndTest()
{

}

} // namespace XAuthTestNamespace

// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XAuthTest )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( XAuthTest )
    EXPORT_TABLE_ENTRY( "StartTest", XAuthTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XAuthTestEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", XAuthTestDllMain )
END_EXPORT_TABLE( XAuthTest )

