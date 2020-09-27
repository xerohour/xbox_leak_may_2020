//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include "xauthbvt.h"


//@@@ drm: XOnlineSetupTempAccounts has been removed
#define XOnlineSetupTempAccounts( a, b, c ) (E_FAIL)

using namespace XAuthBVTNamespace;

namespace XAuthBVTNamespace {

//==================================================================================
// Globals
//==================================================================================
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
	DWORD dwInternalPollCounter = 0;

	if(pfCompleted)
		*pfCompleted = FALSE;

	dwLastTime = GetTickCount();

	while(TRUE)
	{

		if((hWorkToDoEvent != INVALID_HANDLE_VALUE) && (hWorkToDoEvent != NULL))
			WaitForSingleObject(hWorkToDoEvent, dwWaitTimeLeft);

        hr = XOnlineTaskContinue(hAsyncHandle);

		if(FAILED(hr))
			return hr;

		// Update the poll-counter
		++dwInternalPollCounter;

        if (hr == XONLINETASK_S_SUCCESS)
        {
			if(fCloseOnComplete)
				XOnlineTaskClose(hAsyncHandle);

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
// VerifyServiceLogons
//----------------------------------------------------------------------------------
//
// Description: Scans through a service array looking for failed logons
//
// Arguments:
//	HANDLE			hLog			Handle to logging subsystem
//	DWORD	        *pServices		Pointer to an array of services
//	DWORD			dwNumServices	Indicates number of services in pServices array
//
// Returns: FALSE if a service in the array failed logon, TRUE other wise
//==================================================================================
BOOL VerifyServiceLogons(HANDLE hLog, DWORD *pServices, DWORD dwNumServices)
{
	BOOL fServiceFailed = FALSE;
	XONLINE_SERVICE_INFO serviceInfo;
	HRESULT hr;
	
	for(DWORD dwCurrentService = 0; dwCurrentService < dwNumServices; ++dwCurrentService)
	{
		if(FAILED(hr = XOnlineGetServiceInfo(pServices[dwCurrentService],&serviceInfo)))
		{
			fServiceFailed = TRUE;
			xLog(hLog, XLL_INFO, "Logon to S%d failed with 0x%08x",
				serviceInfo.dwServiceID, hr);
		}
	}

	return !fServiceFailed;
}

//==================================================================================
// XAuthBVTDllMain
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
BOOL WINAPI XAuthBVTDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
	WSADATA WSAData;
	XNADDR xnaddr;
	DWORD dwElapsedTime = 0, dwStartTime = 0, dwCurrentTime = 0, dwResult = 0;

	// We'll initialize/delete the global critical section here
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		XNetAddRef();
		
		dwElapsedTime = 0;
		dwStartTime = GetTickCount();
		
		// Wait until stack initialization completes
		do
		{
			dwCurrentTime = GetTickCount();
			
			// Calculate the elapsed time based on whether the time has wrapped around
			if(dwCurrentTime < dwStartTime)
				dwElapsedTime =  MAXDWORD - dwStartTime + dwCurrentTime;
			else
				dwElapsedTime = dwCurrentTime - dwStartTime;
			
			if(dwElapsedTime > 10000)
			{
				OutputDebugString(TEXT("Net stack failed to initialize"));
				return FALSE;
			}
			
			dwResult = XNetGetTitleXnAddr (&xnaddr);		
			
		} while (XNET_GET_XNADDR_PENDING == dwResult);

		WSAStartup(MAKEWORD(2, 2), &WSAData);
		g_hHeap = HeapCreate(0,0,0);
		break;
	case DLL_PROCESS_DETACH:
		WSACleanup();
		XNetRelease();
		HeapDestroy(g_hHeap);
		g_hHeap = NULL;
		break;
	default:
		break;
	}

    return TRUE;
}


//==================================================================================
// XAuthBVTStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI XAuthBVTStartTest(IN HANDLE  hLog)
{
	XONLINETASK_HANDLE hLogon = NULL;
	XPININPUTHANDLE hPinHandle = NULL;
	DWORD ServicesArray[] =
	{
		XONLINE_BILLING_OFFERING_SERVICE,
		XONLINE_MATCHMAKING_SERVICE
	};
	XONLINE_USER UsersArray[XONLINE_MAX_STORED_ONLINE_USERS];
	XINPUT_STATE inputState;
	HRESULT hr = S_OK;
	HANDLE hEvent = NULL;
	DWORD numUsers = 0, cServices = 0;
	BOOL fCompleted = FALSE;
	BYTE pinByte = 0;

	memset(&inputState, 0, sizeof(inputState));
	memset(UsersArray, 0, sizeof(UsersArray));

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
	{
		xLog(hLog, XLL_FAIL, "Couldn't create event for authentication BVT");
		goto Exit;
	}

	xSetOwnerAlias(hLog, "tristanj");
    xSetComponent(hLog, "Online", "Authentication - Client (O)");


#if 0 // XOnlineSetupTempAccounts and XOnlineHasMachineAccount are gone
	xSetFunctionName(hLog, "XOnlineSetupTempAccounts");
	START_TESTCASE("Create temporary user and machine accounts");

	XOnlineSetupTempAccounts( TRUE, XONLINE_LOCAL_HARD_DRIVE, XONLINE_MAX_STORED_ONLINE_USERS );
	PASS_TESTCASE("XOnlineSetupTempAccounts was called" );

	END_TESTCASE();
	xSetFunctionName(hLog, "");



	xSetFunctionName(hLog, "XOnlineHasMachineAccount");
	START_TESTCASE("Look for machine account");

	if(XOnlineHasMachineAccount() == FALSE)
	{
		FAIL_TESTCASE("XOnlineHasMachineAccount didn't find machine account");
	}
	else
	{
		PASS_TESTCASE("XOnlineHasMachineAccount found machine account" );
	}

	END_TESTCASE();
	xSetFunctionName(hLog, "");
#endif


	xSetFunctionName(hLog, "XOnlineGetUsers");
	START_TESTCASE("Look for user accounts");

	hr = XOnlineGetUsers( UsersArray, &numUsers );

	xLog(hLog, XLL_INFO, "XOnlineGetUsers returned 0x%08x", hr);
	if( FAILED(hr) )
	{
		FAIL_TESTCASE("XOnlineGetUsers failed unexpectedly");
	}
	else if(numUsers == 0)
	{
		FAIL_TESTCASE("XOnlineGetUsers returned success but no users were returned");
	}
	else
	{
		xLog(hLog, XLL_INFO, "XOnlineGetUsers found %u users", numUsers);
		PASS_TESTCASE("XOnlineGetUsers succeeded");
	}

	END_TESTCASE();
	xSetFunctionName(hLog, "");



	xSetFunctionName(hLog, "XOnlineLogon");
	START_TESTCASE("Logon to XOnline");

	cServices = sizeof(ServicesArray) / sizeof(DWORD);

	xLog(hLog, XLL_INFO, "Logging %u users into %u services", numUsers, cServices);
	hr = XOnlineLogon(UsersArray, ServicesArray, cServices, hEvent, &hLogon);

	xLog(hLog, XLL_INFO, "XOnlineLogon returned 0x%08x", hr);
	if (FAILED(hr))
	{
		FAIL_TESTCASE("XOnlineLogon failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "Waiting for logon to complete");
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
	
	if(!VerifyServiceLogons(hLog, ServicesArray, cServices))
	{
		FAIL_TESTCASE("Failed logon to some services");
	}

	END_TESTCASE();
	xSetFunctionName(hLog, "");



	xSetFunctionName(hLog, "XOnlinePINDecodeInput");
	START_TESTCASE("Decode PIN input");

	inputState.dwPacketNumber = 10;
	inputState.Gamepad.bAnalogButtons[0] = 0;

	hPinHandle = XOnlinePINStartInput( &inputState );
	
	xLog(hLog, XLL_INFO, "XOnlinePINStartInput returned 0x%08x", hPinHandle);
	if(!hPinHandle)
	{
		FAIL_TESTCASE("Failed to start PIN input");
	}

	inputState.dwPacketNumber = 20;
	inputState.Gamepad.bAnalogButtons[0] = 200;
	hr = XOnlinePINDecodeInput( hPinHandle, &inputState, &pinByte );
	
	xLog(hLog, XLL_INFO, "XOnlinePINDecodeInput returned 0x%08x", hr);
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlinePINDecodeInput failed unexpectedly");
	}

	xLog(hLog, XLL_INFO, "Decoded byte 0x%02x", pinByte);

	hr = XOnlinePINEndInput(hPinHandle);
	hPinHandle = NULL;
	if(FAILED(hr))
	{
		FAIL_TESTCASE("XOnlinePINEndInput failed unexpectedly");
	}

	PASS_TESTCASE("PIN successfully decoded");

	END_TESTCASE();
	xSetFunctionName(hLog, "");


Exit:

	if(hPinHandle)
		XOnlinePINEndInput(hPinHandle);

	if(hLogon)
		XOnlineTaskClose(hLogon);

	if(hEvent)
		CloseHandle(hEvent);

	return;
}

//==================================================================================
// XAuthBVTEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI XAuthBVTEndTest()
{

}

} // namespace XAuthTestNamespace

// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XAuthBVT )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( XAuthBVT )
    EXPORT_TABLE_ENTRY( "StartTest", XAuthBVTStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XAuthBVTEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", XAuthBVTDllMain )
END_EXPORT_TABLE( XAuthBVT )

