//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include <common.h>
#include "matchcoretest.h"
#include "CTestCase_CreateSession.h"
#include "CParameter_CreateSession.h"
#include "TestCaseAndResult_CreateSession.h"

using namespace MatchCoreTestNamespace;

namespace MatchCoreTestNamespace {

//==================================================================================
// Globals
//==================================================================================
// Test specific globals

static CRITICAL_SECTION g_csSerializeAccess;
HANDLE g_hTestLog = INVALID_HANDLE_VALUE;
XONLINETASK_HANDLE g_hLogonTask = NULL;
HANDLE g_hHeap = NULL;

static PMATCH_CORE_TEST_FUNCTION g_LocalTestFunctions[MATCHMSG_LOCAL_MAXTEST] = 
{
	MatchCoreTest_CreateSession							//MATCHMSG_CREATESESSION
};

static LPSTR g_LocalTestNames[MATCHMSG_LOCAL_MAXTEST] = 
{
	"Open session"									//MATCHMSG_CREATESESSION
};

//==================================================================================
// MatchCoreTestDllMain
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
BOOL WINAPI MatchCoreTestDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
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
		InitializeCriticalSection(&g_csSerializeAccess);
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

//==================================================================================
// MatchCoreTestStart
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		g_hTestLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI MatchCoreTestStart(IN HANDLE  hTestLog)
{
	XONLINETASK_HANDLE hLogonTask = NULL;
	HRESULT hr = S_OK;
	HANDLE hPingObject = NULL, hEnumObject = NULL, hClientObject = NULL;
	DWORD dwInitialized = 0, dwServices = 0;
	INT nRet = 0, i;
	BOOL	bRet = TRUE;

	g_hTestLog = hTestLog;
	
	DWORD ServicesArray[NUM_SERVICES] =
	{
		XONLINE_MATCHMAKING_SERVICE
	};

	// Since we have to be thread safe, serialize entry for now
	EnterCriticalSection(&g_csSerializeAccess);

    // Set XLog info
	xSetOwnerAlias(g_hTestLog, "styoo");
    xSetComponent(g_hTestLog, "Online(S)", "Matching - Client (S-o)");
	xSetFunctionName(g_hTestLog, "<none>");

	dwServices = (sizeof(ServicesArray)/sizeof(DWORD));

	bRet = LogonForTest(hTestLog, NULL, &g_hLogonTask, ServicesArray, dwServices, ALLOWEDLOGONINTERVAL);
    if (bRet == FALSE)
	{
		xLog(g_hTestLog, XLL_FAIL, "Failed to Logon for Test");
		goto Exit;
	}

	// Run tests that don't require another machine
	for(i = MATCHMSG_LOCAL_BASE; i < MATCHMSG_LOCAL_MAXTEST; i++)
	{
		xSetFunctionName(g_hTestLog, g_LocalTestNames[i]);

		g_LocalTestFunctions[i]();

		xSetFunctionName(g_hTestLog, "");
	}
Exit:

	g_hTestLog = INVALID_HANDLE_VALUE;

	CLEANUP_TASK(g_hLogonTask);

	// Allow other threads to enter now
	LeaveCriticalSection(&g_csSerializeAccess);
}            

//==================================================================================
// MatchCoreTest_CreateSession
//----------------------------------------------------------------------------------
//
// Description:  
//
// Arguments:
 // Returns:
 //==================================================================================

void MatchCoreTest_CreateSession()
{
	int		ii; 

	
//	int		nTotalCase = sizeof g_TCAE_CreateSession / sizeof(TestCaseAndExpected_CreateSession);

	for(ii=0; ii<g_nCase_CreateSession ; ii++)
	{
		CTestCase_CreateSession* pTestCase = new(CTestCase_CreateSession);

		pTestCase->SetnCase(ii);
		pTestCase->BringPameterAndExpected();
		pTestCase->EvaluateParameters(m_tc, m_pPara);
		if(pTestCase->m_nUpdate != SY_NOUPDATE)
			pTestCase->EvaluateParameters(m_tcUpdate, m_pParaUpdate);
		
		xStartVariation(g_hTestLog, "");	// Add Calling find Test Name 
		if( pTestCase->RunTest() == SY_EXCEPTION_STOP_WHOLE_TEST )
			break;
		xEndVariation(g_hTestLog);

		delete(pTestCase);
	}

}



//==================================================================================
// MatchCoreTestEnd
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI MatchCoreTestEnd()
{
	// We already cleaned up everything at the end of StartTest
}
#if 0
//==================================================================================
// MemAlloc
//----------------------------------------------------------------------------------
//
// MemAlloc: Private heap allocation function
//
// Arguments:
//	DWORD		dwSize		Size of buffer to allocate from the private heap
//
// Returns: Valid pointer to newly allocated memory, NULL otherwise
//==================================================================================
LPVOID MemAlloc(DWORD dwSize)
{
	if(!g_hHeap)
	{
//		DbgPrint("DirectPlay test's private heap hasn't been created!");
		return NULL;
	}

	return HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY, dwSize);
}

//==================================================================================
// MemFree
//----------------------------------------------------------------------------------
//
// Description: Private heap deallocation function
//
// Arguments:
//	LPVOID		pBuffer		Pointer to buffer to be released
//
// Returns: TRUE if the buffer was deallocated, FALSE otherwise
//==================================================================================
BOOL MemFree(LPVOID pBuffer)
{
	if(!g_hHeap)
	{
//		DbgPrint("DirectPlay test's private heap hasn't been created!");
		return FALSE;
	}

	return HeapFree(g_hHeap, 0, pBuffer);
}

DWORD GetBufferLenForInt()
{
	return sizeof(DWORD) + sizeof(ULONGLONG);
}

DWORD GetBufferLenForString(LPWSTR szString)
{
	DWORD dwSize = sizeof(DWORD) + sizeof(WORD);
	dwSize += (wcslen(szString) + 1) * sizeof(WCHAR);
	return dwSize;
}

DWORD GetBufferLenForBlob(LPSTR szBlob)
{
	DWORD dwSize = sizeof(DWORD) + sizeof(WORD);
	dwSize += strlen(szBlob) * sizeof(CHAR);
	return dwSize;
}

DWORD GetBufferLenForUserInt()
{
	return (sizeof(ULONGLONG) + GetBufferLenForInt());
}

DWORD GetBufferLenForUserString(LPWSTR szString)
{
	return (sizeof(ULONGLONG) + GetBufferLenForString(szString));
}

DWORD GetBufferLenForUserBlob(LPSTR szBlob)
{
	return (sizeof(ULONGLONG) + GetBufferLenForBlob(szBlob));
}

//==================================================================================
// ReadSearchResults
//----------------------------------------------------------------------------------
//
// Description: Adds attributes from a list to the async task indicated
//
// Arguments:
//	HANDLE				hTestLog				Handle to logging subsystem
//	XONLINETASK_HANDLE	hSearch				Handle to search task
//	DWORD				dwResultsExpected	Number of results expected
//	BOOL				fMoreResultsOK		Indicates whether an exact number of results is expected, or just a minimum
//
// Returns: TRUE if the search results were returned and formatted correctly, FALSE otherwise
//==================================================================================
BOOL ReadSearchResults(HANDLE hTestLog, XONLINETASK_HANDLE hSearch, DWORD dwResultsExpected, BOOL fMoreResultsOK)
{
	PXMATCH_SEARCHRESULT *ppSearchResults = NULL;
	HRESULT hr = S_OK;
	DWORD dwNumResults = 0;
	BOOL fPassed = TRUE;

	hr = XOnlineMatchSearchGetResults(hSearch, &ppSearchResults, &dwNumResults);
	if(FAILED(hr))
	{
		xLog(hTestLog, XLL_FAIL, "Getting search results failed with 0x%08x", hr);
		return FALSE;
	}

	if(fMoreResultsOK)
	{
		if(dwNumResults < dwResultsExpected)
		{
			xLog(hTestLog, XLL_FAIL, "Received %u results but at least %u were expected",
				dwNumResults, dwResultsExpected);
			return FALSE;
		}
	}
	else
	{
		if(dwNumResults != dwResultsExpected)
		{
			xLog(hTestLog, XLL_FAIL, "Received %u results but exactly %u were expected",
				dwNumResults, dwResultsExpected);
			return FALSE;
		}
	}

	xLog(hTestLog, XLL_PASS, "Recieved %u results as expected", dwNumResults, dwResultsExpected);
	return TRUE;
}

//==================================================================================
// AddAttributes
//----------------------------------------------------------------------------------
//
// Description: Adds attributes from a list to the async task indicated
//
// Arguments:
//	XONLINETASK_HANDLE	hAsyncHandle		Handle to async task
//	PATTRIBUTE			pAttributeArray		Pointer to an array of attributes
//	DWORD				dwAttributeCount	Indicates number of attributes in array
//
// Returns: TRUE if the attributes were added successfully, FALSE otherwise
//==================================================================================
BOOL AddAttributes(XONLINETASK_HANDLE hAsyncHandle, PATTRIBUTE pAttributeArray, DWORD dwAttributeCount)
{
	HRESULT hr = S_OK;
	DWORD dwAttributeIndex = 0;
	BOOL fRet = TRUE;

	if(!pAttributeArray)
	{
		fRet = FALSE;
		goto Exit;
	}

	for (dwAttributeIndex = 0; dwAttributeIndex < dwAttributeCount; ++dwAttributeIndex)
	{
		switch (pAttributeArray[dwAttributeIndex].dwAttributeID & X_ATTRIBUTE_DATATYPE_MASK)
		{
			case X_ATTRIBUTE_DATATYPE_INTEGER:
			{
				hr = XOnlineMatchSessionSetInt(hAsyncHandle, pAttributeArray[dwAttributeIndex].dwAttributeID,
					pAttributeArray[dwAttributeIndex].qwUserID, pAttributeArray[dwAttributeIndex].Value.qwValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding integer to session 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_STRING:
			{
				hr = XOnlineMatchSessionSetString(hAsyncHandle, pAttributeArray[dwAttributeIndex].dwAttributeID,
					pAttributeArray[dwAttributeIndex].qwUserID, (LPWSTR) pAttributeArray[dwAttributeIndex].Value.pvValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding string to session 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_BLOB:
			{
				hr = XOnlineMatchSessionSetBlob(hAsyncHandle, pAttributeArray[dwAttributeIndex].dwAttributeID,
					pAttributeArray[dwAttributeIndex].qwUserID, strlen((LPSTR)pAttributeArray[dwAttributeIndex].Value.pvValue),
					pAttributeArray[dwAttributeIndex].Value.pvValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding string to session 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}
		}
	}

Exit:

	return fRet;
}

//==================================================================================
// AddParameters
//----------------------------------------------------------------------------------
//
// Description: Adds parameters from a list to the async task indicated
//
// Arguments:
//	XONLINETASK_HANDLE	hAsyncHandle		Handle to async task
//	PPARAMETER			pParameterArray		Pointer to an array of parameters
//	DWORD				dwParameterCount	Indicates number of parameters in array
//
// Returns: TRUE if the attributes were added successfully, FALSE otherwise
//==================================================================================
BOOL AddParameters(XONLINETASK_HANDLE hAsyncHandle, PPARAMETER pParameterArray, DWORD dwParameterCount)
{
	HRESULT hr = S_OK;
	DWORD dwParameterIndex = 0;
	BOOL fRet = TRUE;

	if(!pParameterArray)
	{
		fRet = FALSE;
		goto Exit;
	}

	for (dwParameterIndex = 0; dwParameterIndex < dwParameterCount; ++dwParameterIndex)
	{
		switch (pParameterArray[dwParameterIndex].dwParameterType & X_ATTRIBUTE_DATATYPE_MASK)
		{
			case X_ATTRIBUTE_DATATYPE_INTEGER:
			{
				hr = XOnlineMatchSearchAppendInt(hAsyncHandle, pParameterArray[dwParameterIndex].Value.qwValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding integer to search 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_STRING:
			{
				hr = XOnlineMatchSearchAppendString(hAsyncHandle, (LPWSTR) pParameterArray[dwParameterIndex].Value.pvValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding string to search 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_BLOB:
			{
				hr = XOnlineMatchSearchAppendBlob(hAsyncHandle, strlen((LPSTR)pParameterArray[dwParameterIndex].Value.pvValue),
					pParameterArray[dwParameterIndex].Value.pvValue );
				if (FAILED(hr))
				{
					DbgPrint("Error adding string to search 0x%08x\n", hr);
					fRet = FALSE;
					goto Exit;
				}
				break;
			}
		}
	}

Exit:

	return fRet;
}


//==================================================================================
// CreateSession
//----------------------------------------------------------------------------------
//
// Description: Creates a session with the specified attributes and returns the session ID 
//
// Arguments:
//	DWORD		dwPublicAvailable	Number of public slots available
//	DWORD		dwPrivateAvailable	Number of private slots available
//	PATTRIBUTE	pAttributeArray		Array of attributes to add to this session
//	DWORD		dwAttributeCount	Number of attributes in the array
//	DWORD		*pdwSessionID		Variable that passes back the creates session's ID
//
// Returns: TRUE if the session was created successfully, FALSE otherwise
//==================================================================================
BOOL CreateSession(ULONGLONG qwPublicAvailable, ULONGLONG qwPrivateAvailable, PATTRIBUTE pAttributeArray, DWORD dwAttributeCount, XNKID *pSessionID)
{
	XONLINETASK_HANDLE hMatch = NULL;
	ULONGLONG qwUserID = 0;
	HRESULT hr = S_OK;
    HANDLE hEvent = NULL;
	DWORD dwAttributeSize = 0, dwAttributeIndex = 0, dwPollCounter = 0;
	XNKID SessionID;
	BOOL fSuccess = FALSE, fCompleted = TRUE;

	if(!pSessionID || (!pAttributeArray && dwAttributeCount)) 
	{
		goto Exit;
	}

	memset(pSessionID, 0, sizeof(XNKID));
	qwUserID = 0;

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!hEvent)
	{
		goto Exit;
	}

	for(dwAttributeIndex = 0;dwAttributeIndex < dwAttributeCount; ++dwAttributeIndex)
	{
		switch(pAttributeArray[dwAttributeIndex].dwAttributeID & X_ATTRIBUTE_DATATYPE_MASK)
		{
		case X_ATTRIBUTE_DATATYPE_INTEGER:
			dwAttributeSize += sizeof(DWORD) + sizeof(ULONGLONG);
			break;
		case X_ATTRIBUTE_DATATYPE_STRING:
			dwAttributeSize += sizeof(DWORD) + sizeof(WORD) + (wcslen((WCHAR * ) (pAttributeArray[dwAttributeIndex].Value.pvValue)) + 1) * sizeof(WCHAR);
			break;
		case X_ATTRIBUTE_DATATYPE_BLOB:
			dwAttributeSize += sizeof(DWORD) + sizeof(WORD) + strlen((CHAR * ) (pAttributeArray[dwAttributeIndex].Value.pvValue));
			break;
		default:
			goto Exit;
		}

		// Add space for the user ID
// BUGBUG: Removed X_ATTRIBUTE_TYPE_MASK since it's no longer supported
//		if(!(pAttributeArray[dwAttributeIndex].dwAttributeID & X_ATTRIBUTE_TYPE_MASK))
//		{
			dwAttributeSize += sizeof(ULONGLONG);
//		}
	}

	memset(&SessionID, 0, sizeof(SessionID));
	hr = XOnlineMatchSessionCreate(SessionID, dwAttributeCount, dwAttributeSize, hEvent, &hMatch );
	if(FAILED(hr))
	{
		fSuccess = FALSE;
		goto Exit;
	}

	if(dwAttributeCount)
	{
		if(!AddAttributes(hMatch, pAttributeArray, dwAttributeCount))
		{
			fSuccess = FALSE;
			goto Exit;
		}
	}

// BUGBUG: Removed XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE since it's no longer supported
	hr = XOnlineMatchSessionSetInt(hMatch, /*XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE*/ 0, qwUserID, qwPublicAvailable);
	if(FAILED(hr))
	{		
		fSuccess = FALSE;
		goto Exit;
	}

// BUGBUG: Removed XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE since it's no longer supported
	hr = XOnlineMatchSessionSetInt(hMatch, /*XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE*/ 0, qwUserID, qwPrivateAvailable);
	if(FAILED(hr))
	{		
		fSuccess = FALSE;
		goto Exit;
	}

	hr = XOnlineMatchSessionSend(hMatch);
	if (FAILED(hr))
	{
		fSuccess = FALSE;
		goto Exit;
	}

	hr = PollTaskUntilComplete(hMatch, hEvent, 30000, &dwPollCounter, &fCompleted, TRUE);
	if((hr == S_OK) && fCompleted)
	{
		XOnlineMatchSessionGetID (hMatch, pSessionID);
		
		if(*((ULONGLONG *) pSessionID))
		{
			fSuccess = TRUE;
		}
		
		hMatch = NULL;
	}
	else
	{
		if(fCompleted)
			hMatch = NULL;
		fSuccess = FALSE;
		goto Exit;
	}

Exit:

	if(hMatch)
	{
		dwPollCounter = 0;
		XOnlineTaskClose(hMatch);
		hMatch = NULL;
	}

	if(hEvent)
		CloseHandle(hEvent);

	return fSuccess;
}
#endif



} // namespace MatchCoreTestNamespace

// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( matchcoretest )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( matchcoretest )
    EXPORT_TABLE_ENTRY( "StartTest", MatchCoreTestStart )
    EXPORT_TABLE_ENTRY( "EndTest", MatchCoreTestEnd )
    EXPORT_TABLE_ENTRY( "DllMain", MatchCoreTestDllMain )
END_EXPORT_TABLE( matchcoretest )
