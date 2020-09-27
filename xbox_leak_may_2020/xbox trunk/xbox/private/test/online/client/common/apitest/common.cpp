#include "common.h"

//==================================================================================
// LogonForTestExceptionFilter
//----------------------------------------------------------------------------------
//
// Description: 
//
// Arguments:
//
// Returns: 
//==================================================================================

BOOL LogonForTest(HANDLE hTestLog, HANDLE hLogonEvent, PXONLINETASK_HANDLE phLogonTask, 
							DWORD* pServices, DWORD cServices, DWORD dwMaxWaitInterval)
{
	XONLINE_USER        *pUsersArray;
	BOOL				bRet = TRUE;
	DWORD				dwNumUsers = 0, dwStartTime = 0, dwCurrentTime = 0, dwElapsedTime = 0;
	HRESULT 			hr, hrContinue;
	char 				szMsg[128];
	WCHAR				wszMsg[128];

	__try
	{
		// Get local users (we assume that the hard-drive has been populated)
	    pUsersArray =  (XONLINE_USER *) LocalAlloc(LPTR,XONLINE_MAX_STORED_ONLINE_USERS * sizeof(XONLINE_USER));
	    if(pUsersArray == NULL)        
			RaiseException(SY_EXCEPTION_MEM_ALLOC_FAIL, 0,0, NULL);

	    hr = XOnlineGetUsers( pUsersArray, &dwNumUsers );
		if (FAILED(hr))
			RaiseException(SY_EXCEPTION_GET_USERS_FAIL, 0,0, NULL);
		
		if(dwNumUsers == 0)
			RaiseException(SY_EXCEPTION_NO_USERS, 0,0, NULL);

		// Initialize services
		xLog(hTestLog, XLL_INFO, "Logging into %u services with %u users", cServices, dwNumUsers);
		hr = XOnlineLogon(pUsersArray, pServices, cServices, hLogonEvent, phLogonTask);
	   	LocalFree(pUsersArray);
		if (FAILED(hr))
			RaiseException(SY_EXCEPTION_LOGON_FAIL, 0,0, NULL);

		// Pump until logon succeeds (PARTIAL_RESULTS flag set) or fails (DONT_CONTINUE flag set)
		xLog(hTestLog, XLL_INFO, "Waiting for logon to finish");
		INT	iRet = PumpTaskUntilTimeout(hTestLog, *phLogonTask, hLogonEvent, ALLOWEDPUMPINGINTERVAL, NULL, FALSE, &hr);

		switch(iRet)
		{
			case SY_SUCCESS :
				wsprintf(wszMsg, TEXT("XOnlineLogonTask returned %0x at last\n"),hr);
				OutputDebugString(wszMsg);
				break;
			case SY_FAIL :
				wsprintf(wszMsg, TEXT("XOnlineLogonTask returned %0x at last\n"),hr);
				OutputDebugString(wszMsg);
				RaiseException(SY_EXCEPTION_LOGON_PUMP_FAIL, 0,0, NULL);
				break;
			case SY_TIMEOUT :
				wsprintf(wszMsg, TEXT("XOnlineLogonTask timed out at last\n"));
				OutputDebugString(wszMsg);
				RaiseException(SY_EXCEPTION_LOGON_PUMP_TIMEOUT, 0,0, NULL);
			break;
		}

		hr = XOnlineLogonTaskGetResults(*phLogonTask);
		xLog(hTestLog, XLL_INFO, "Logon returned 0x%08x", hr);
		if (hr == XONLINE_S_LOGON_CONNECTION_ESTABLISHED)
		{		  
			xLog(hTestLog, XLL_INFO, "Logon completed successfully");
		}
		else
		{
			xLog(hTestLog, XLL_INFO, "Logon failed asynchronously");
			RaiseException(SY_EXCEPTION_LOGONTASK_RESULT_WRONG, 0,0, NULL);
		}
	}

	__except(LogonForTestExceptionFilter(GetExceptionCode()))
	{
	    bRet = FALSE;
	    
	    switch(GetExceptionCode())
	    {
	    	case SY_EXCEPTION_MEM_ALLOC_FAIL:
	    		sprintf(szMsg, "Memory Allocation Fail");
	    		break;

	    	case SY_EXCEPTION_GET_USERS_FAIL:
		   		LocalFree(pUsersArray);
	    		sprintf(szMsg, "Getting users from hard-drive failed with 0x%08x", hr);
			
	    		break;

	    	case SY_EXCEPTION_NO_USERS:
		   		LocalFree(pUsersArray);
//				OutputDebugString(TEXT("No users were found on the hard-drive, run the SetupOnline tool!"));
	    		sprintf(szMsg, "No users were found on the hard-drive, run the SetupOnline tool!");
	    		break;

	    	case SY_EXCEPTION_LOGON_FAIL:
	    		sprintf(szMsg, "Logon failed");
	    		break;
			case SY_EXCEPTION_LOGON_PUMP_FAIL :
	    		sprintf(szMsg, "Logon Pumping failed");
	    		break;
			case SY_EXCEPTION_LOGON_PUMP_TIMEOUT :
	    		sprintf(szMsg, "Logon Pumping timedout");
	    		break;
			case SY_EXCEPTION_LOGONTASK_RESULT_WRONG :
	    		sprintf(szMsg, "Logon Task result is unexpected");
	    		break;
	    		

	    }

	    xLog(hTestLog, XLL_INFO, szMsg);
	}

	return(bRet);
}			


//==================================================================================
// LogonForTestExceptionFilter
//----------------------------------------------------------------------------------
//
// Description: 
//
// Arguments:
//
// Returns: 
//==================================================================================

LONG LogonForTestExceptionFilter(DWORD dwExceptionCode)
{
	LONG lRet = EXCEPTION_CONTINUE_SEARCH;
	
	switch(dwExceptionCode)
	{
		case SY_EXCEPTION_GET_USERS_FAIL :
		case SY_EXCEPTION_NO_USERS :
		case SY_EXCEPTION_MEM_ALLOC_FAIL :
		case SY_EXCEPTION_LOGON_FAIL :	
		case SY_EXCEPTION_LOGON_PUMP_FAIL :
		case SY_EXCEPTION_LOGON_PUMP_TIMEOUT :
		case SY_EXCEPTION_LOGONTASK_RESULT_WRONG :
			lRet = EXCEPTION_EXECUTE_HANDLER;
			break;

		default:
			break;
	}

	return lRet;
}

//==================================================================================
// PumpTaskUntilTimeout	: Done
//----------------------------------------------------------------------------------
//
// Description: Calls the XOnlineTaskContinue function until either the given
//    task has been completed or the amount of available time has expired
//
// Arguments:
//	IN XONLINETASK_HANDLE	hTask		Handle to async task
//	IN HANDLE				hEvent		Event that is associated with this async task.
//											This can be set to NULL or INVALID_HANDLE_VALUE
//											if no event is associated with the task
//	IN DWORD				dwMaxWaitInterval		Maximum amount of time to wait for the task to
//											complete. This can be set to INFINITE to block
//											until completion
//	OUT DWORD				*pdwPollCounter		Optional counter passed in to keep track of the
//											number of calls made to the XOnlineTaskDoWork function
//  IN BOOL					bCloseOnComplete	Indicates whether the function should close a handle
//											after it indicates completion
//  OUT HRESULT				*hrReturn	
//
// Returns: SY_SUCCESS, SY_FAIL or SY_TIMEOUT
//==================================================================================
INT PumpTaskUntilTimeout(HANDLE hTestLog, XONLINETASK_HANDLE hTask, HANDLE hEvent, DWORD dwMaxWaitInterval, 
									DWORD *pdwPollCounter, BOOL bCloseOnComplete, HRESULT *hrReturn)
{
	INT			iResult = SY_TIMEOUT;
	DWORD 		dwStartTime;
	DWORD 		dwInternalPollCounter = 0;
	HRESULT 	hrContinue;

	dwStartTime = GetTickCount();

	do
	{
		if((hEvent != INVALID_HANDLE_VALUE) && (hEvent != NULL))
			WaitForSingleObject(hEvent, dwMaxWaitInterval);

		hrContinue = XOnlineTaskContinue(hTask);
		++dwInternalPollCounter;

		if (XONLINETASK_S_RUNNING == hrContinue) // same as if (XONLINETASK_STATUS_AVAILABLE(hrContinue))
		{
			if(dwMaxWaitInterval != INFINITE)
			{
				// See if we have gone beyond the maximum allowed time
				if(IsTimeExpired(dwStartTime, dwMaxWaitInterval))
				{
					DbgPrint("Async opration didn't complete within %u ms\n", dwMaxWaitInterval);
					xLog(hTestLog, XLL_INFO, "PumpTask Timed out");
					iResult = SY_TIMEOUT;
					break;
				}
				else
				{
					Sleep(SLEEPINTERVAL);
				}
			}
		}
		else if (SUCCEEDED(hrContinue))	// same as if (XONLINETASK_STATUS_SUCCESSFUL_COMPLETION(hrContinue))
		{
			iResult = SY_SUCCESS;
		    *hrReturn = hrContinue;
			xLog(hTestLog, XLL_INFO, "PumpTask Succedded");
		    break;
		}
		else
		{
			iResult = SY_FAIL;
		    *hrReturn = hrContinue;
			xLog(hTestLog, XLL_INFO, "PumpTask Failed");
		    break;
		}
	} while(TRUE);

	if(pdwPollCounter)
		*pdwPollCounter = dwInternalPollCounter;

	if(bCloseOnComplete)
		XOnlineTaskClose(hTask);


	return iResult;
}

//==================================================================================
// IsTimeExpired	: Done
//----------------------------------------------------------------------------------
//
// Description: Check if allowed time is expired
//
// Arguments:
//	DWORD	dwStartTime, DWORD dwAllowedInterval
// Returns:
//	TRUE if expired.
//==================================================================================

BOOL	IsTimeExpired(DWORD	dwStartTime, DWORD dwAllowedInterval)
{
	DWORD dwCurrentTime = GetTickCount(), dwDuration;

	if(dwCurrentTime < dwStartTime)
		dwDuration = (MAXDWORD - dwStartTime) + dwCurrentTime;
	else
		dwDuration = dwCurrentTime - dwStartTime;

	if(dwDuration > dwAllowedInterval)
		return TRUE;
	else
		return FALSE;
}
		


//==================================================================================
// PumpLogonTask	: Done
//----------------------------------------------------------------------------------
//
// Description: Pump Logon Task 
//
// Arguments:
//	HANDLE  hTestLog, XONLINETASK_HANDLE hLogonTask
// Returns:
//	FALSE if pumping fails and it will block whole test case after now.
//==================================================================================

BOOL PumpLogonTask(HANDLE  hTestLog, XONLINETASK_HANDLE hLogonTask, HRESULT* phrReturn)
{
	*phrReturn = XOnlineTaskContinue(hLogonTask); 
	
	if(FAILED(*phrReturn)) 
	{
	    xLog(hTestLog, XLL_FAIL, "Lost connection with CS, exiting test!");
	    return FALSE;
	}

	return TRUE;
}
