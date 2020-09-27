/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       onlinestress.cpp
 *  Content:    onlinestress tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  12/06/01    tristanj Created to test Xbox onlinestress
 *
 ****************************************************************************/

#include "onlinestress.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

HANDLE g_hLog = NULL;
CLogonTask *g_pLogonTask = NULL;
BOOL g_fExitTest = FALSE;

/****************************************************************************
 *
 *  TRACE
 *
 ****************************************************************************/
#if defined(NOLOGGING)
void WINAPI TRACE(HANDLE hLog, LPCSTR szFormat, ...) {
	WCHAR szBuffer[1024] = L"", szWideFormat[1024] = L"";
	va_list pArgs; 
	va_start(pArgs, szFormat);

	mbstowcs(szWideFormat, szFormat, strlen(szFormat));
	szWideFormat[strlen(szFormat)] = 0;
	
	wvsprintf(szBuffer, szWideFormat, pArgs);
	wcscat(szBuffer, L"\n");
	
	va_end(pArgs);

	OutputDebugString(szBuffer);
}
#endif

/****************************************************************************
 *
 *  SendPacket
 *
 *  Description:
 *      Sends data to the socket until the request completes, or the specified
 *      timer has expired
 *
 *  Arguments:
 *      SOCKET                 Socket              Socket from which to send the packet
 *      CHAR                   *pBuff              Buffer to send the packet from
 *      INT                    nSizeToSend         Ammount of data to send
 *      COnlineTimer           *pTimer             If this object expires, then the send will fail
 *                                                 If NULL, return immediately if send fails
 *
 *  Returns:  
 *      S_OK on success
 *      S_FALSE if the socket timed out
 *      E_FAIL on system failure
 *
 ****************************************************************************/
HRESULT SendPacket(SOCKET Socket, CHAR *pBuff, INT nSizeToSend, COnlineTimer *pTimer)
{
	HRESULT hr = S_OK;
	INT nSentSize = 0, nRet = 0;

	if(pTimer)
		pTimer->Start();

	do
	{
		nRet = send(Socket, pBuff + nSentSize, nSizeToSend - nSentSize, 0);
		// If a socket error other than WOULDBLOCK occurs, then bail
		if(nRet == SOCKET_ERROR)
		{
			// Keep trying until timeout for a WOULDBLOCK error
			if(WSAGetLastError() == WSAEWOULDBLOCK)
			{
				Sleep(0);
				hr = S_FALSE;
				continue;
			}

			LOGTASKINFO(g_hLog, "Unexpected socket error received from client");
			return E_FAIL;
		}
		
		nSentSize += nRet;

		// If we read the client size, then continue to the next stage
		if(nSentSize == nSizeToSend)
		{
			hr = S_OK;
			break;
		}
	}
	while(pTimer && (!pTimer->HasTimeExpired()));

	if(pTimer && (pTimer->HasTimeExpired()))
		hr = S_FALSE;

	return hr;
}

/****************************************************************************
 *
 *  ReadPacket
 *
 *  Description:
 *      Reads data from the socket until either the amount of data requested
 *      has been received, or the specified timer has expired
 *
 *  Arguments:
 *      SOCKET                 Socket              Socket from which to read the packet
 *      CHAR                   *pBuff              Buffer in which to put the packet
 *      INT                    nSizeToRead         Ammount of data to read
 *      COnlineTimer           *pTimer             If this object expires, then the read will fail
 *                                                 If NULL, return immediately if nothing to read
 *
 *  Returns:  
 *      S_OK all data was 
 *      S_FALSE if the socket timed out
 *      E_FAIL if the there was a system error
 *
 ****************************************************************************/
HRESULT ReadPacket(SOCKET Socket, CHAR *pBuff, INT nSizeToRead, COnlineTimer *pTimer)
{
	HRESULT hr = S_OK;
	INT nReceivedSize = 0, nRet = 0;

	if(pTimer)
		pTimer->Start();

	do
	{
		nRet = recv(Socket, pBuff + nReceivedSize, nSizeToRead - nReceivedSize, 0);
		// If a socket error other than WOULDBLOCK occurs, then bail
		if(nRet == SOCKET_ERROR)
		{
			// Keep trying until timeout for a WOULDBLOCK error
			if(WSAGetLastError() == WSAEWOULDBLOCK)
			{
				Sleep(0);
				hr = S_FALSE;
				continue;
			}

			LOGTASKINFO(g_hLog, "Unexpected socket error received from client");
			return E_FAIL;
		}
		// If the socket was closed on the other end, then bail
		else if(nRet == 0)
		{
			LOGTASKINFO(g_hLog, "Client socket closed unexpectedly");
			return E_FAIL;
		}
		
		nReceivedSize += nRet;

		// If we read the client size, then continue to the next stage
		if(nReceivedSize == nSizeToRead)
		{
			hr = S_OK;
			break;
		}
	}
	while(pTimer && (!pTimer->HasTimeExpired()));

	if(pTimer && (pTimer->HasTimeExpired()))
		hr = S_FALSE;

	return hr;
}

/****************************************************************************
 *
 *  ReadNewClientInfo
 *
 *  Description:
 *      Reads the client/user information from the client that just connected
 *      and parses the data into the newly created CClientConnection object
 *
 *  Arguments:
 *      Socket                 Socket              Socket connecting to host
 *      CClientConnection      *pNewClient         Client object that will hold the received client info
 *
 *  Returns:  
 *      S_OK if the client info was successfully parsed
 *      S_FALSE if the socket timed out
 *      E_FAIL if the data received was invalid or there was a system error
 *
 ****************************************************************************/
HRESULT ReadNewClientInfo(SOCKET Socket, CClientConnection *pNewClient)
{
	COnlineTimer Timer;
	HRESULT hr = S_OK;
	DWORD dwClientSize = 0;
	CHAR *pBuff = (CHAR *) &dwClientSize;

	// Allow 5 seconds for client to send its information
	Timer.SetAllowedTime(MAX_NEW_CLIENT_INFO_TIME);

	// Read the first DWORD of the incoming packet which contains the client info size
	hr = ReadPacket(Socket, pBuff, sizeof(DWORD), &Timer);

	if(hr != S_OK)
		return hr;

	if((dwClientSize > XONSTRESS_MAX_CLIENT_SIZE) || (dwClientSize < XONSTRESS_MIN_CLIENT_SIZE))
	{
		LOGTASKINFO(g_hLog, "Client indicated size greater than maximum or less than minimum");
		return E_FAIL;
	}

	// Allocate a buffer for the rest of the packet
	pBuff = new CHAR[dwClientSize];
	if(!pBuff)
	{
		LOGTASKINFO(g_hLog, "Not enough memory to allocate client buffer");
		return E_FAIL;
	}

	*((DWORD *) pBuff) = dwClientSize;

	// Read the rest of the packet
	hr = ReadPacket(Socket, pBuff + sizeof(DWORD), dwClientSize, &Timer);

	if(hr != S_OK)
		return hr;

	// Parse the packet information into the client object
	hr = pNewClient->UnpackFromBuffer(pBuff, &dwClientSize);

	return hr;
}

/****************************************************************************
 *
 *  CreateListenSocket
 *
 *  Description:
 *      Create a host socket and begin listening for incoming connections
 *
 *  Arguments:
 *      CListenSocket         *pListenSocket      The socket that will be created to handle incoming
 *                                                connection requests
 *      XNKID                 pSessionID          XNKID to register with the network stack
 *      XNKEY                 pKeyExchangeKey     XNKEY to register with the network stack
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/
HRESULT CreateListenSocket(CListenSocket **ppListenSocket, XNKID *pSessionID, XNKEY *pKeyExchangeKey)
{
	HRESULT hr = S_OK;

	if(!ppListenSocket || !pSessionID || !pKeyExchangeKey)
		return E_FAIL;

	*ppListenSocket = new CListenSocket();

	hr = (*ppListenSocket)->StartListening(XONSTRESS_HOST_PORT);
	if(hr != S_OK)
		goto Exit;

Exit:

	if(hr != S_OK)
	{
		delete *ppListenSocket;
		*ppListenSocket = NULL;
	}

	return hr;
}

/****************************************************************************
 *
 *  SearchForStressSession
 *
 *  Description:
 *      Searches for active stress sessions with available slots
 *
 *  Arguments:
 *      CMatchSearchTask      *pMatchSearchTask   Pointer to a match search task used to issue the search
 *
 *  Returns:  
 *      S_OK if a stress session with extra space was found
 *      S_FALSE if a stress session wasn't found
 *      E_FAIL if there was some kind of system failure
 *
 ****************************************************************************/
HRESULT SearchForStressSession(CMatchSearchTask *pMatchSearchTask)
{
	COnlineTimer SearchTimer;
	HRESULT hr = S_OK;
	DWORD dwResultsCount = 0;

	pMatchSearchTask->SetSearchAttributes(NULL, 0);
	pMatchSearchTask->SetResultsSize(XONLINESTRESS_MAX_RESULTS, XONLINESTRESS_MAX_RESULTS_LEN);
	pMatchSearchTask->SetSearchProcIndex(XONLINESTRESS_SEARCH_PROC);

	// Start the search for an available stress session
	if(!pMatchSearchTask->StartTask())
	{
		LOGTASKINFOHR(g_hLog, "Error starting matchmaking search", hr);
		hr = E_FAIL;
		goto Exit;
	}

	SearchTimer.SetAllowedTime(XONLINESTRESS_MAX_MATCH_SEARCH_TIME);
	SearchTimer.Start();

	// Pump the search task until completion
	do
	{
		if(!pMatchSearchTask->WaitForWork(SearchTimer.GetTimeRemaining()))
			continue;

		hr = pMatchSearchTask->TaskContinue();
		if(XONLINETASK_STATUS_AVAILABLE(hr))
			break;

	} while(!SearchTimer.HasTimeExpired());

	if(SearchTimer.HasTimeExpired())
	{
		LOGTASKINFO(g_hLog, "Timed out searching for host session");
		hr = S_FALSE;
		goto Exit;
	}

	hr = pMatchSearchTask->GetSearchResults(NULL, &dwResultsCount);
	if(hr != S_OK)
	{
		TRACE(g_hLog, "Error getting search results: 0x%08x", hr);
		goto Exit;
	}

	if(dwResultsCount == 0)
		hr = S_FALSE;
	else
		hr = S_OK;

Exit:

	return hr;
}

/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      The stress test can be compiled in three ways:
 *         1) As a standalone Xbox XBE not running under the harness (i.e. _XBOX and NOLOGGING are defined)
 *         2) As an Xbox harness lib (i.e. _XBOX is defined and NOLOGGING is NOT defined)
 *         3) As a standalone PC app using CXOnline (i.e. _XBOX and NOLOGGING are NOT defined)
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#if defined(_XBOX) && defined(NOLOGGING)
int __cdecl main(int argc, char * argv[])
#else
#if !defined(_XBOX)
#define NOLOGGING
class CStressXOnline : public CXOnline
{
public:
    CStressXOnline(char * pszXbox = NULL) : CXOnline(pszXbox) {}
    int Main(int argc, char * argv[]);
	// BUGBUG - need to add entries for every function 
};

int __cdecl main(int argc, char * argv[])
{
    int result;
    char buffer[128];

    sprintf(buffer, "%s@Nic/0", getenv("COMPUTERNAME") );
    CStressXOnline * pTestXOnline = new CStressXOnline(buffer);
    result = pTestXOnline->Main(argc, argv);
    delete pTestXOnline;
    return(result);
}

int CStressXOnline::Main(int argc, char * argv[])
#endif
#endif

#ifdef NOLOGGING
{
    onlinestress_StartTest( NULL );
    onlinestress_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 *  onlinestress_BasicTest
 *
 *  Description:
 *      Basic Test for onlinestress
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT onlinestress_BasicTest( void )
{
	CMatchSearchTask        *pMatchSearchTask = NULL;
	CLogonTask              *pLogonTask = NULL;
	COnlineTimer			Timer;
    HRESULT                 hr              = S_OK;
	DWORD					ServicesArray[] =
	{
		XONLINE_USER_ACCOUNT_SERVICE,
		XONLINE_MATCHMAKING_SERVICE
	};
	DWORD                   dwServicesCount = sizeof(ServicesArray) / sizeof(DWORD), dwRandTime = 0, dwOnlineStatus = 0;
	XNADDR                  HostAddress;
	XNKID                   SessionID;
	XNKEY                   KeyExchangeKey;

	XNetAddRef();
		
	hr = XOnlineStartup(NULL);

	pLogonTask = new CLogonTask;
	pMatchSearchTask = new CMatchSearchTask;
	if(!pLogonTask || !pMatchSearchTask)
	{
		hr = E_FAIL;
		goto Exit;
	}

	memset(&HostAddress, 0, sizeof(HostAddress));
	memset(&SessionID, 0, sizeof(SessionID));
	memset(&KeyExchangeKey, 0, sizeof(KeyExchangeKey));
	
	// Set the global logon task
	g_pLogonTask = pLogonTask;
	
	// Populate users accounts from user account creation service
	hr = COnlineAccounts::Instance()->PopulateUserAccountsHD(NULL, 4);
	if(hr != S_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Populating user accounts failed.");
		goto Exit;
	}
	
	pLogonTask->RegisterLogHandle(g_hLog);
	
	// The logon task will automatically login with as many users as possible
	
	// Add any requested services
	if(!pLogonTask->SetServices(ServicesArray, dwServicesCount))
	{
		hr = E_FAIL;
		goto Exit;
	}
	
	// Start logon async task
	if(!pLogonTask->StartTask())
	{
		hr = E_FAIL;
		goto Exit;
	}
	
	Timer.SetAllowedTime(MAX_LOGON_TIME);
	Timer.Start();
	
	// Pump task until complete
	while(!pLogonTask->IsConnectionEstablished())
	{
		if(Timer.HasTimeExpired())
			break;
		
		// Make sure to wait for the work event to test that it is always signalled when necessary
		if(!pLogonTask->WaitForWork(Timer.GetTimeRemaining()))
		{
			LOGTASKINFO(g_hLog, "Logon task wasn't signalled");
			break;
		}
		
		pLogonTask->TaskContinue();
	}
	
	if(!pLogonTask->IsConnectionEstablished())
	{
		LOGTASKINFO(g_hLog, "Logon timed-out");
		hr = E_FAIL;
		goto Exit;
	}
	
	// See if we can find a stress session to join
	hr = SearchForStressSession(pMatchSearchTask);

	// If a stress session was found with space left, then connect as a client
	if(hr == S_OK)
	{
		hr = client_Main(pMatchSearchTask);
	}
	// If a stress session wasn't found, then create one
	else if(hr == S_FALSE)
	{
		dwOnlineStatus = XNetGetTitleXnAddr(&HostAddress);
		if(!(dwOnlineStatus | XNET_GET_XNADDR_ONLINE))
		{
			LOGTASKINFO(g_hLog, "Error retrieving title XNADDR");
			hr = E_FAIL;
			goto Exit;
		}

		TRACE(g_hLog, "Hosting stress session on %02x-%02x-%02x-%02x-%02x-%02x",
			HostAddress.abEnet[0],
			HostAddress.abEnet[1],
			HostAddress.abEnet[2],
			HostAddress.abEnet[3],
			HostAddress.abEnet[4],
			HostAddress.abEnet[5]);

		hr = host_Main();
	}
	// If some other error occurred, then exit
	else
	{
		LOGTASKINFOHR(g_hLog, "Error searching for stress session", hr);
		hr = E_FAIL;
		goto Exit;
	}


Exit:

	if(pLogonTask)
		delete pLogonTask;

	if(pMatchSearchTask)
		delete pMatchSearchTask;

	if(FAILED(hr))
		TRACE(NULL, "ONLINESTRESS FAILED: 0x%08x", hr);
	else
		TRACE(NULL, "ONLINESTRESS SUCCEEDED");

	hr = XOnlineCleanup();
	
	XNetRelease();

    return hr;
}

/****************************************************************************
 *
 *  onlinestress_StartTest
 *
 *  Description:
 *      The Harness Entry into the onlinestress tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI onlinestress_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;
    XDEVICE_PREALLOC_TYPE deviceTypes[] = {
               {XDEVICE_TYPE_GAMEPAD, 4},
               {XDEVICE_TYPE_MEMORY_UNIT, 8}
    };

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

	g_hLog = LogHandle;

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

#ifdef NOLOGGING
	XInitDevices(sizeof(deviceTypes) / sizeof(XDEVICE_PREALLOC_TYPE), deviceTypes);
#endif
		
	
#ifdef NOLOGGING
	while(TRUE)
	{
#endif

	//
	// Test onlinestress in the mannor it was meant to be called
	//
	
	if ( SHOULDRUNTEST( "onlinestress", "Basic" ) )
	{
		SETLOG( LogHandle, "tristanj", "Online", "onlinestress", "Basic" );
		EXECUTE( onlinestress_BasicTest() );
	}
	
#ifdef NOLOGGING
	}
#endif

	g_hLog = NULL;
		
}

/****************************************************************************
 *
 *  onlinestress_EndTest
 *
 *  Description:
 *      The Exit function for the test harness
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI onlinestress_EndTest( VOID )
{
	g_fExitTest = TRUE;
}

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( onlinestress )
#pragma data_seg()

BEGIN_EXPORT_TABLE( onlinestress )
    EXPORT_TABLE_ENTRY( "StartTest", onlinestress_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", onlinestress_EndTest )
END_EXPORT_TABLE( onlinestress )
