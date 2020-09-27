//==================================================================================
// Includes
//==================================================================================
#include "sendrecv.h"

//==================================================================================
// Globals
//==================================================================================
CRITICAL_SECTION g_csSerializeAccess;

//==================================================================================
// Functions
//==================================================================================

//==================================================================================
// MyLog
//----------------------------------------------------------------------------------
//
// Description: Output log function
//
// Arguments:
//	HINSTANCE	hInstance		Handle to module
//	DWORD		dwReason		Indicates the reason for calling the function
//	LPVOID		lpContext		reserved
// Returns:
//	TRUE on success
//==================================================================================
void WINAPI MyLog(HANDLE hLog, DWORD dwLogLevel, LPSTR szFormat, ...)
{
	va_list pArgs; 
	va_start(pArgs, szFormat);
	
#ifdef _XBOX
// Xbox version sends out put to the xLog subsystem
	if(hLog != INVALID_HANDLE_VALUE)
		xLog_va(hLog, dwLogLevel, szFormat, pArgs);
#else // XBOX
// NT version (console) sends output to the standard output
	_vtprintf(szFormat, pArgs);
	_tprintf(TEXT("\r\n"));
#endif // ! XBOX
	
	va_end(pArgs);
}

#ifdef _XBOX
// Xbox version is a harness DLL, so we need DllMain, StartTest and EndTest

//==================================================================================
// DllMain
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
BOOL WINAPI DllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
	// We'll initialize/delete the global critical section here
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		XnetInitialize(NULL, TRUE);
		InitializeCriticalSection(&g_csSerializeAccess);
		break;
	case DLL_PROCESS_DETACH:
		XnetCleanup();
		DeleteCriticalSection(&g_csSerializeAccess);
		break;
	default:
		break;
	}

    return TRUE;
}
#else
// NT version is an executable 

//==================================================================================
// _tmain
//----------------------------------------------------------------------------------
//
// Description: App entry point
//
// Arguments:
//	INT		argc		Int indicating num of cmd-line parameters available
//	TCHAR	*argvW[]	Array of command line available cmd-line parameters
// Returns:
//	An integer indicating exit status (ignored)
//==================================================================================
int _cdecl _tmain(INT argc, TCHAR *argvW[])
{

	InitializeCriticalSection(&g_csSerializeAccess);

//	ParseCommandLine();

	StartTest(NULL);

	EndTest();
	
	DeleteCriticalSection(&g_csSerializeAccess);

	return TRUE;
}
#endif // ! XBOX

//==================================================================================
// StartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI StartTest(IN HANDLE  hLog)
{
	TEST_PARAMS *pTestParams = NULL;
    WSADATA	WSAData;
	INT		nRet = 0;

	// Since we have to be thread safe, serialize entry for now
	EnterCriticalSection(&g_csSerializeAccess);

#ifdef _XBOX
    // Set the component and subcomponent
    xSetComponent(hLog, "Network(S)", "Winsock");
#endif

    // Init winsock
    if(nRet = WSAStartup(MAKEWORD(2, 2), &WSAData))
	{
		MyLog(hLog, XLL_FAIL, "Couldn't init winsock: %d", nRet);
		goto EXIT;
	}

//	MyLog(hLog, XLL_INFO, "Winsock layer initialized");

	// If we are running as a server, wait for incoming test requests and execute them
	if(IsServer())
	{
		DWORD dwSecondsToRun = 0, dwStartTick = 0;
		SOCKET BroadcastListener = INVALID_SOCKET;
		TEST_CASE NextTest = NO_TEST;
		HANDLE hClientHandlerThread = NULL;
		
		//BUGBUG - at some point, will want to automate getting the server wait time
		//		dwSecondsToRun = GetServerWaitTime();
		dwSecondsToRun = TIME_INFINITY;
		dwStartTick = GetTickCount();

		MyLog(hLog, XLL_INFO, "Listening for client programs...");

		// Create server socket here
		BroadcastListener = CreateBroadcastListenerSocket(hLog, FIND_SERVER_PORT);
		if(BroadcastListener == SOCKET_ERROR)
		{
			MyLog(hLog, XLL_WARN, "Couldn't create broadcast listening socket");
			goto EXIT;
		}

		// If time hasn't expired, continue running test
		while(!IsTimeExpired(dwSecondsToRun, dwStartTick))
		{
			NextTest = ProcessNextBroadcastPacket(hLog, BroadcastListener);

			pTestParams = (TEST_PARAMS *) LocalAlloc(LPTR, sizeof(TEST_PARAMS));
			if(!pTestParams)
			{
				MyLog(hLog, XLL_FAIL, "Not enough memory to allocate test params");
				goto EXIT;
			}

			// Server side of test always has these parameters in common
			pTestParams->dwSecondsToRun = TIME_INFINITY;
			pTestParams->fCleanUpAfterTest = TRUE;
			pTestParams->fInitiateConnection = FALSE;
			pTestParams->fNonBlocking = FALSE;
			pTestParams->fUseCriticalSection = FALSE;
			pTestParams->hLog = hLog;

			switch(NextTest)
			{
			case DOWNSTREAM_DATA:
				MyLog(hLog, XLL_INFO, "Client requested DOWNSTREAM_DATA test");
				// Server will send data to the client...
				pTestParams->fUseDelays = FALSE;
				if((hClientHandlerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) DoSendTest, (LPVOID) pTestParams, 0, NULL)) == INVALID_HANDLE_VALUE)
				{
					MyLog(hLog, XLL_WARN, "Couldn't create thread: %u", GetLastError());
					goto EXIT;
				}
				pTestParams = NULL;
				CloseHandle(hClientHandlerThread);
				break;
			case UPSTREAM_DATA:
				MyLog(hLog, XLL_INFO, "Client requested UPSTREAM_DATA test");
				// Server will receive data from the client...
				pTestParams->fUseDelays = FALSE;
				if((hClientHandlerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) DoRecvTest, (LPVOID) pTestParams, 0, NULL)) == INVALID_HANDLE_VALUE)
				{
					MyLog(hLog, XLL_WARN, "Couldn't create thread: %u", GetLastError());
					goto EXIT;
				}
				pTestParams = NULL;
				CloseHandle(hClientHandlerThread);
				break;
			case UPSTREAM_DATA_DELAYED_RECV:
				MyLog(hLog, XLL_INFO, "Client requested UPSTREAM_DATA_DELAYED_RECV test");
				// Server will receive data from the client...
				// but will occasionally delay to test recv window processing
				pTestParams->fUseDelays = TRUE;
				if((hClientHandlerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) DoRecvTest, (LPVOID) pTestParams, 0, NULL)) == INVALID_HANDLE_VALUE)
				{
					MyLog(hLog, XLL_WARN, "Couldn't create thread: %u", GetLastError());
					goto EXIT;
				}
				pTestParams = NULL;
				CloseHandle(hClientHandlerThread);
				break;
			case ECHO:
				MyLog(hLog, XLL_INFO, "Client requested ECHO test");
				// Server will echo back data to the client
				pTestParams->fUseDelays = FALSE;
				if((hClientHandlerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) DoSingleThreadEchoServerTest, (LPVOID) pTestParams, 0, NULL)) == INVALID_HANDLE_VALUE)
				{
					MyLog(hLog, XLL_WARN, "Couldn't create thread: %u", GetLastError());
					goto EXIT;
				}
				pTestParams = NULL;
				CloseHandle(hClientHandlerThread);
				break;
			case ECHO_DELAYED_RECV:
				MyLog(hLog, XLL_INFO, "Client requested ECHO_DELAYED_RECV test");
				// Server will echo back data to the client
				pTestParams->fUseDelays = TRUE;
				if((hClientHandlerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) DoSingleThreadEchoServerTest, (LPVOID) pTestParams, 0, NULL)) == INVALID_HANDLE_VALUE)
				{
					MyLog(hLog, XLL_WARN, "Couldn't create thread: %u", GetLastError());
					goto EXIT;
				}
				pTestParams = NULL;
				CloseHandle(hClientHandlerThread);
				break;
			default:
				// No test to perform, wait for the next packet
				LocalFree(pTestParams);
				pTestParams = NULL;
				continue;
			}
		}

		closesocket(BroadcastListener);
		BroadcastListener = INVALID_SOCKET;
	}
	// If we are running as a client, search for a server and issue test requests to it
	else
	{
		// Allocate a test params structure
		pTestParams = (TEST_PARAMS *) LocalAlloc(LPTR, sizeof(TEST_PARAMS));
		if(!pTestParams)
		{
			MyLog(hLog, XLL_FAIL, "Not enough memory to allocate test params");
			goto EXIT;
		}

		pTestParams->hLog = hLog;
		pTestParams->dwSecondsToRun = 60;
		pTestParams->fCleanUpAfterTest = FALSE;

		if(!FindServer(hLog, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "No server found.  Exiting test.");
			goto EXIT;
		}

		// BLOCKING TESTS

		pTestParams->fNonBlocking = FALSE;

		// Request the server to recv
		if(!RequestTest(hLog, UPSTREAM_DATA, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = FALSE;
		pTestParams->fUseDelays = FALSE;
		DoSendTest(pTestParams);
		Sleep(1000);

		// Request the server to recv with delays
		if(!RequestTest(hLog, UPSTREAM_DATA_DELAYED_RECV, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = FALSE;
		pTestParams->fUseDelays = FALSE;
		DoSendTest(pTestParams);
		Sleep(1000);

		// Request the server to send, client will recv normally
		if(!RequestTest(hLog, DOWNSTREAM_DATA, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = FALSE;
		pTestParams->fUseDelays = FALSE;
		DoRecvTest(pTestParams);
		Sleep(1000);

		// Request the server to send, client will recv with delays
		if(!RequestTest(hLog, DOWNSTREAM_DATA, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = FALSE;
		pTestParams->fUseDelays = TRUE;
		DoRecvTest(pTestParams);
		Sleep(1000);

/*
		// Request the server to echo, client will do multithreaded send/recv
		if(!RequestTest(hLog, ECHO, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = TRUE;
		pTestParams->fUseDelays = FALSE;
		DoMultiThreadEchoClientTest(pTestParams);

		// Request the server to echo with delays, client will do multithreaded send/recv
		if(!RequestTest(hLog, ECHO_DELAYED_RECV, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = TRUE;
		pTestParams->fUseDelays = FALSE;
		DoMultiThreadEchoClientTest(pTestParams);

		// NON-BLOCKING TESTS

		pTestParams->fNonBlocking = TRUE;

		// Request the server to recv
		if(!RequestTest(hLog, UPSTREAM_DATA, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = FALSE;
		pTestParams->fUseDelays = FALSE;
		DoSendTest(pTestParams);

		// Request the server to recv with delays
		if(!RequestTest(hLog, UPSTREAM_DATA_DELAYED_RECV, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = FALSE;
		pTestParams->fUseDelays = FALSE;
		DoSendTest(pTestParams);

		// Request the server to send, client will recv normally
		if(!RequestTest(hLog, DOWNSTREAM_DATA, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = FALSE;
		pTestParams->fUseDelays = FALSE;
		DoRecvTest(pTestParams);

		// Request the server to send, client will recv with delays
		if(!RequestTest(hLog, DOWNSTREAM_DATA, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = FALSE;
		pTestParams->fUseDelays = TRUE;
		DoRecvTest(pTestParams);

		// Request the server to echo, client will do multithreaded send/recv
		if(!RequestTest(hLog, ECHO, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = TRUE;
		pTestParams->fUseDelays = FALSE;
		DoMultiThreadEchoClientTest(pTestParams);

		// Request the server to echo with delays, client will do multithreaded send/recv
		if(!RequestTest(hLog, ECHO_DELAYED_RECV, pTestParams->szDestinationAddress))
		{
			MyLog(hLog, XLL_INFO, "Couldn't request test from server.");
			goto EXIT;
		}
		Sleep(1000);
		pTestParams->fInitiateConnection = TRUE;
		pTestParams->fUseCriticalSection = TRUE;
		pTestParams->fUseDelays = FALSE;
		DoMultiThreadEchoClientTest(pTestParams);
*/

		LocalFree(pTestParams);
		pTestParams = NULL;
	}
	

EXIT:

	if(pTestParams)
		LocalFree(pTestParams);

	// Clean up winsock
//	MyLog(hLog, XLL_INFO, "Cleaning up Winsock layer");
	WSACleanup();

	// Allow other threads to enter now
	LeaveCriticalSection(&g_csSerializeAccess);
}            

//==================================================================================
// EndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI EndTest()
{
	// We already cleaned up everything at the end of StartTest
}

//==================================================================================
// IsServer
//----------------------------------------------------------------------------------
//
// Description:  Helper function that determines if this instance should run as a test server
//
// Arguments:
//  none
// Returns:
//	TRUE if this instance should be the test sever, FALSE otherwise
//==================================================================================
BOOL IsServer()
{
//BUGBUG - for now, hardcode to PC as server, Xbox as client
#ifdef _XBOX
	return FALSE;
#else // XBOX
	return TRUE;
#endif // ! XBOX
}

//==================================================================================
// IsTimeExpired
//----------------------------------------------------------------------------------
//
// Description:  Helper function that determines if the test should continue running
//
// Arguments:
//	DWORD	dwSecondsToRun			Number of seconds to run test
//	DWORD	dwStartTick				Original tick count when test was started
// Returns:
//	TRUE if the time has been reached, FALSE otherwise
//==================================================================================
BOOL IsTimeExpired(DWORD dwSecondsToRun, DWORD dwStartTick)
{
	DWORD dwMillisecondsToRun = dwSecondsToRun * 1000, dwCurrTick = 0;

	// If this test is set to run infinitely, then always return FALSE
	if(dwSecondsToRun >= TIME_INFINITY)
		return FALSE;

	dwCurrTick = GetTickCount();

	if(dwCurrTick < dwStartTick)
		// If the clock has wrapped around...
		return (((MAXDWORD - dwStartTick) + dwCurrTick) >= dwMillisecondsToRun);
	else
		// If the clock hasn't wrapped around...
		return ((dwCurrTick - dwStartTick) >= dwMillisecondsToRun);

}

//==================================================================================
// RandomRange
//----------------------------------------------------------------------------------
//
// Description: Helper function that generates random number between dwMin and dwMax
//
// Arguments:
//	DWORD	dwMin				Minimum random number desired
//	DWORD	dwMax				Maximum random number desired
// Returns:
//	DWORD containing the value of the generated random number
//==================================================================================
DWORD RandomRange(DWORD dwMin, DWORD dwMax)
{
    return((rand() % (dwMax - dwMin + 1)) + dwMin);
} 

//==================================================================================
// BuildTestPacket
//----------------------------------------------------------------------------------
//
// Description: Generate a pseudo-random number
//
// Arguments:
//	INT		nLength			Size of the buffer to fill
//	BYTE	Buffer[]		Buffer to fill with data
// Returns:
//	DWORD containing the checksum of the generated packet
//==================================================================================
DWORD BuildTestPacket(DWORD dwLength, BYTE *pBuffer)
{
    DWORD n = 0, dwCheckSum = 0;
	static char cNextChar = 'a';
	
	// Fill the buffer with random data and calculate the checksum
    for (n = 0; n < dwLength; n++)
    {
		//		if(n == (dwLength - 1))
		//			pBuffer[n] = 0xAA;
		//		else
		//			pBuffer[n] = (BYTE) RandomRange(0, 255);
		
		if(n == (dwLength - 1))
			pBuffer[n] = 0xAA;
		else
			pBuffer[n] = (BYTE) cNextChar;
		
        dwCheckSum += pBuffer[n];
		
		if(cNextChar == 'z')
			cNextChar = 'a';
		else
			++cNextChar;
    }	

    
    return(dwCheckSum);
}

//==================================================================================
// CalculateCheckSum
//----------------------------------------------------------------------------------
//
// Description: Calculate a checksum over a buffer
//
// Arguments:
//	INT		nLength			Size of the buffer to compute checksum over
//	BYTE	Buffer[]		Buffer to compute checksum over
// Returns:
//	DWORD containing the checksum of the buffer
//==================================================================================
DWORD CalculateCheckSum(DWORD dwLength, BYTE *pBuffer)
{
	DWORD n = 0, dwCheckSum = 0;

	// Calculate the checksum
    for (n = 0; n < dwLength; n++)
        dwCheckSum += pBuffer[n];
    
    return(dwCheckSum);
}

//==================================================================================
// EstablishConnection
//----------------------------------------------------------------------------------
//
// Description: Establishes a connection to the specified address.
//
// Arguments:
//	HANDLE	hLog					Handle to the logging subsystem
//	LPCSTR	szDestinationAddress	Address to connect with
//	BOOL	fNonBlocking			Indicates whether to make socket non-blocking
// Returns:
//	Connected SOCKET that has been connected to the specified address.
//==================================================================================
SOCKET EstablishConnection(HANDLE hLog, LPCSTR szDestinationAddress, BOOL fNonBlocking)
{
	
    SOCKADDR_IN		DestinationSockAddr  = { AF_INET };
	SOCKET			Connection = INVALID_SOCKET;
	LINGER			LingerOption;
	BOOL			fConnectionComplete = FALSE;
	
	// Fill in the destination address structure
	DestinationSockAddr.sin_port        = htons(SEND_RECV_PORT);
	if((DestinationSockAddr.sin_addr.s_addr = inet_addr(szDestinationAddress)) == INADDR_NONE)
	{
		MyLog(hLog, XLL_FAIL, "Invalid destination address: %s", szDestinationAddress);
		goto EXIT;
	}
	
	// Create the socket
	if((Connection = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		MyLog(hLog, XLL_FAIL, "Couldn't create STREAM socket: %u", WSAGetLastError());
		goto EXIT;
	}

	LingerOption.l_linger = 0;
	LingerOption.l_onoff = TRUE;

	// Set socket for hard-close so the other-side will terminate on a close
	if(setsockopt(Connection, SOL_SOCKET, SO_LINGER, (const char *) &LingerOption, sizeof(LINGER)) == SOCKET_ERROR)
	{
		MyLog(hLog, XLL_FAIL, "Couldn't set socket for hard closes: %u", WSAGetLastError());
		goto EXIT;
	}

	// Set socket as non-blocking if necessary
	if(fNonBlocking)
	{
		DWORD	dwNonBlocking = TRUE;

		if(ioctlsocket(Connection, FIONBIO, &dwNonBlocking))
		{
			MyLog(hLog, XLL_INFO, "Couldn't set socket to be non-blocking");
			goto EXIT;
		}
	}

	// Connect the socket
	if (connect(Connection, (const struct sockaddr *) &DestinationSockAddr,  sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		// Wait for asynchronous completion if necessary
		if((WSAGetLastError() == WSAEWOULDBLOCK) && fNonBlocking)
		{
			while(!fConnectionComplete)
			{
				// Wait for socket to register as writeable (meaning connection has completed)
				if(IsSocketReady(Connection, WRITEABLE, 10))
				{
					fConnectionComplete = TRUE;
					MyLog(hLog, XLL_INFO, "Connection completed asynchronously");
					continue;
				}

				// If socket wasn't writeable, check to see if the connection failed
				if(IsSocketReady(Connection, EXCEPTION, 0))
				{
					MyLog(hLog, XLL_FAIL, "Connection attempt failed asynchronously");
					closesocket(Connection);
					goto EXIT;
				}
				
				MyLog(hLog, XLL_INFO, "Waiting for connection to complete");
				Sleep(1000);
			}
		}
		else
		{
			MyLog(hLog, XLL_FAIL, "Connection attempt failed: %u", WSAGetLastError());
			closesocket(Connection);
			Connection = INVALID_SOCKET;
			
			goto EXIT;
		}
	}
	else
		fConnectionComplete = TRUE;

EXIT:
//	fConnectionComplete ? MyLog(hLog, XLL_INFO, "Established connection") : 0;

	return Connection;
}

//==================================================================================
// WaitForIncomingConnection
//----------------------------------------------------------------------------------
//
// Description: Creates a listening socket and accepts a connection
//
// Arguments:
//	HANDLE	hLog					Handle to the logging subsystem
//	BOOL	fNonBlocking			Indicates whether to make socket non-blocking
// Returns:
//	SOCKET representing an accepted incoming connection
//==================================================================================
SOCKET WaitForIncomingConnection(HANDLE hLog, BOOL fNonBlocking)
{
	
    SOCKADDR_IN		LocalSockAddr = { AF_INET }, DestinationSockAddr = { AF_INET };
	SOCKET			Listener = INVALID_SOCKET, Connection = INVALID_SOCKET;
	LINGER			LingerOption;
	INT				nSockAddrSize = sizeof(SOCKADDR_IN);
	
    LocalSockAddr.sin_port        = htons(SEND_RECV_PORT);
    LocalSockAddr.sin_addr.s_addr = INADDR_ANY;
	
	// Create the socket
    if ((Listener = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		MyLog(hLog, XLL_FAIL, "Couldn't create STREAM socket: %u", WSAGetLastError());
		goto EXIT;
	}

	LingerOption.l_linger = 0;
	LingerOption.l_onoff = TRUE;

	// Set socket for hard-close so the other-side will terminate on a close
	if(setsockopt(Listener, SOL_SOCKET, SO_LINGER, (const char *) &LingerOption, sizeof(LINGER)) == SOCKET_ERROR)
	{
		MyLog(hLog, XLL_FAIL, "Couldn't set socket for hard closes: %u", WSAGetLastError());
		goto EXIT;
	}

	// Set socket as non-blocking if necessary
	if(fNonBlocking)
	{
		DWORD	dwNonBlocking = TRUE;

		if(ioctlsocket(Connection, FIONBIO, &dwNonBlocking))
		{
			MyLog(hLog, XLL_INFO, "Couldn't set socket to be non-blocking");
			goto EXIT;
		}
	}

	// Bind the socket to the local address
    if (bind(Listener, (const struct sockaddr *) &LocalSockAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
		MyLog(hLog, XLL_FAIL, "Couldn't bind listening socket: %u", WSAGetLastError());
		goto EXIT;
    }
	
	// Start listening for incoming connections
    if (listen(Listener, 10) == SOCKET_ERROR)
    {
		MyLog(hLog, XLL_FAIL, "Couldn't listen on socket: %u", WSAGetLastError());
		goto EXIT;
    }

//	MyLog(hLog, XLL_FAIL, "Listening for incoming connections...");

	// If this is a non-blocking socket, wait for readability, which means an incoming connection is pending
	if(fNonBlocking)
	{
		while(!IsSocketReady(Connection, READABLE, 10))
		{
			// If the socket registers an exception condition, something failed somewhere
			if(IsSocketReady(Connection, EXCEPTION, 0))
			{
				MyLog(hLog, XLL_FAIL, "An error occured while asynchronously waiting for inbound connections");
				goto EXIT;
			}
		}
	}

	// Accept the first incoming connection
	if ((Connection = accept(Listener, (struct sockaddr *) &DestinationSockAddr, &nSockAddrSize)) == INVALID_SOCKET)
	{
		if(fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
			MyLog(hLog, XLL_WARN, "This socket reported readability, but accept returned WSAEWOULDBLOCK");

		MyLog(hLog, XLL_FAIL, "Couldn't accept incoming connection: %u", WSAGetLastError());
		goto EXIT;
	}

//	MyLog(hLog, XLL_INFO, "Accepted incoming connection");

EXIT:

	// Clean up the listener socket, we don't need it anymore
	if(Listener != INVALID_SOCKET)
	{
		closesocket(Listener);
		Listener = INVALID_SOCKET;
	}

	return Connection;
}

//==================================================================================
// CreateBroadcastListenerSocket
//----------------------------------------------------------------------------------
//
// Description: Creates a socket to listen for broadcast packets
//
// Arguments:
//  HANDLE			hLog				Handle to the logging subsystem
//  WORD			wPort				Port to bind the socket to
// Returns:
//	Returns value indicating the next test case for the server to run
//==================================================================================
SOCKET CreateBroadcastListenerSocket(HANDLE hLog, WORD wPort)
{
    SOCKADDR_IN		LocalSockAddr = { AF_INET };
	SOCKET			BroadcastListener = INVALID_SOCKET;
	INT				nSockAddrSize = sizeof(SOCKADDR_IN);
	
    LocalSockAddr.sin_port        = htons(wPort);
    LocalSockAddr.sin_addr.s_addr = INADDR_ANY;
	
	// Create the socket
    if ((BroadcastListener = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		MyLog(hLog, XLL_FAIL, "Couldn't create DGRAM socket: %u", WSAGetLastError());
		goto EXIT;
	}

	// Bind the socket to the local address
    if (bind(BroadcastListener, (const struct sockaddr *) &LocalSockAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
		MyLog(hLog, XLL_FAIL, "Couldn't bind broadcast listening socket: %u", WSAGetLastError());
		goto EXIT;
    }

EXIT:

	return BroadcastListener;
}

//==================================================================================
// ProcessNextBroadcastPacket
//----------------------------------------------------------------------------------
//
// Description: Reads a datagram from the broadcast listener and responds appropriately
//
// Arguments:
//  HANDLE			hLog					Handle to the logging subsystem
//	SOCKET			BroadcastListener		Socket that server receives test requests on
// Returns:
//	Returns value indicating the next test case for the server to run
//==================================================================================
TEST_CASE ProcessNextBroadcastPacket(HANDLE hLog, SOCKET BroadcastListener)
{
	TESTREQUEST_PACKET *pTestRequest = NULL;
	GENERIC_PACKET *pPacket = NULL;
	TEST_CASE NextTest = NO_TEST;
	SOCKADDR_IN SocketAddress;
	DWORD	dwBytesAvailable = 0;
	BYTE	PacketBuffer[MAX_PACKET_SIZE];
	INT		n = 0, nAddressSize = sizeof(SOCKADDR_IN);
	static BOOL fArrayInitialized = FALSE;
	static INT nLastTestIndex = 0;
	static DWORD dwLastTests[10];

	// Initialize this static array only once...
	// This static array is a simple but imperfect attempt to prevent a client from losing a notification
	// that we started a test.  The client will ask again, but we'll recognize that the test ID is one of
	// the last 10 tests we launched... so we won't respawn the test, but we'll notify the client that the
	// already spawned test is waiting for it
	if(!fArrayInitialized)
	{
		nLastTestIndex = 0;

		for(n = 0; n < 10; ++n)
			dwLastTests[n] = 0;

		fArrayInitialized = TRUE;
	}

	pPacket = (GENERIC_PACKET *) PacketBuffer;

	// If we haven't received data within 10 seconds, exit...
	if(!IsSocketReady(BroadcastListener, READABLE, 10))
		goto EXIT;

	// See how large the next available datagram is...
	if(ioctlsocket(BroadcastListener, FIONREAD, &dwBytesAvailable))
	{
		MyLog(hLog, XLL_WARN, "Couldn't determine if bytes are available on socket");
		goto EXIT;
	}

	if(!dwBytesAvailable)
		goto EXIT;

	// If it's too large, truncate it
	if(dwBytesAvailable > MAX_PACKET_SIZE)
	{
		MyLog(hLog, XLL_WARN, "Received packet was too large: %u bytes", dwBytesAvailable);
		dwBytesAvailable = MAX_PACKET_SIZE;
	}

	// Read the datagram
	if (recvfrom(BroadcastListener, (char *) pPacket, dwBytesAvailable, 0, (struct sockaddr *) &SocketAddress, &nAddressSize) == SOCKET_ERROR)
	{	
		MyLog(hLog, XLL_FAIL, "Couldn't recv data: %u", WSAGetLastError());
		goto EXIT;				
	}

	// Determine what type of packet it is
	switch(pPacket->dwPacketType)
	{
		SERVREPLY_PACKET ServReply;
		TESTREPLY_PACKET TestReply;
	case FINDSERV:
//		MyLog(hLog, XLL_INFO, "Received FINDSERV packet from a client");
		// Received a packet from a client trying to find a server, reply back to indicate we're here
		ServReply.dwPacketType = SERVREPLY;
		if (sendto(BroadcastListener, (char *) &ServReply, 1 * sizeof(DWORD), 0, (struct sockaddr *) &SocketAddress, nAddressSize) == SOCKET_ERROR)
		{
			MyLog(hLog, XLL_FAIL, "Couldn't send data: %u", WSAGetLastError());
			goto EXIT;				
		}
		break;
	case TESTREQUEST:
//		MyLog(hLog, XLL_INFO, "Received TESTREQUEST packet from a client");
		pTestRequest = (TESTREQUEST_PACKET *) pPacket;

		// See if we already spawned this test
		for(n = 0; n < 10; ++n)
		{
			if(dwLastTests[n] == pTestRequest->dwTestID)
				break;
		}

		// We didn't already spawn this test, so mark it as the test to launch and record this test ID.
		if(n == 10)
		{
			nLastTestIndex = (nLastTestIndex + 1) % 10;
			dwLastTests[nLastTestIndex] = NextTest = pTestRequest->TestRequested;
		}

		// Initialize the reply packet to send
		TestReply.dwPacketType = TESTREPLY;
		TestReply.dwTestID = pTestRequest->dwTestID;
		TestReply.TestRequested = pTestRequest->TestRequested;

		// Received a packet from a client requesting a test, reply back to indicate we are running the server side
		if (sendto(BroadcastListener, (char *) &TestReply, 3 * sizeof(DWORD), 0, (struct sockaddr *) &SocketAddress, nAddressSize) == SOCKET_ERROR)
		{
			MyLog(hLog, XLL_FAIL, "Couldn't send data: %u", WSAGetLastError());
			goto EXIT;				
		}
		break;
	case SERVREPLY:
	case TESTREPLY:
	case SENDRECV:
		// Ignore these packets
		break;
	default:
		MyLog(hLog, XLL_INFO, "Received packet is of unknown type: %u", pPacket->dwPacketType);
		goto EXIT;
	}

EXIT:

	return NextTest;
}

//==================================================================================
// FindServer
//----------------------------------------------------------------------------------
//
// Description: Creates a listening socket and accepts a connection
//
// Arguments:
//	HANDLE		hLog						Handle to the logging subsystem
//	CHAR		*szDestinationAddress		String to be filled with IP address of server
// Returns:
//	Returns TRUE if the server was found, FALSE otherwise
//==================================================================================
BOOL FindServer(HANDLE hLog, CHAR *szDestinationAddress)
{
	FINDSERV_PACKET *pFindServPacket = NULL;
	GENERIC_PACKET *pGenericPacket = NULL;
	SOCKADDR_IN DestinationAddress;
	SOCKET	SearchSocket = INVALID_SOCKET;
	DWORD	dwBytesAvailable = 0;
	BYTE	PacketBuffer[MAX_PACKET_SIZE];
	BOOL	fEnableBroadcast = TRUE, fFoundServer = FALSE;
	INT		nAddressSize = 0;

#ifdef _XBOX
	SOCKADDR_IN LocalAddress;
#endif

	// Create the socket
    if((SearchSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		MyLog(hLog, XLL_FAIL, "Couldn't create DGRAM socket: %u", WSAGetLastError());
		goto EXIT;
	}

	// Set the socket to enable broadcast transmissions
	if(setsockopt(SearchSocket, SOL_SOCKET, SO_BROADCAST, (const char *) &fEnableBroadcast, sizeof(fEnableBroadcast)) == SOCKET_ERROR)
	{
		MyLog(hLog, XLL_FAIL, "Couldn't enable socket to send broadcast packets: %u", WSAGetLastError());
		goto EXIT;
	}

#ifdef _XBOX
// BUGBUG - this is a workaround for the fact that Xbox requires UDP sockets
//          to be explicitly bound before sending

	LocalAddress.sin_family = AF_INET;
    LocalAddress.sin_port = htons(FIND_SERVER_PORT);
    LocalAddress.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket to the local address
    if (bind(SearchSocket, (const struct sockaddr *) &LocalAddress, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
		MyLog(hLog, XLL_FAIL, "Couldn't bind search socket: %u", WSAGetLastError());
		goto EXIT;
    }

#endif

	// Continue sending search packets until we get a response from a server
	while(!fFoundServer)
	{
		pFindServPacket = (FINDSERV_PACKET *) PacketBuffer;
		pFindServPacket->dwPacketType = FINDSERV;
		
		DestinationAddress.sin_family = AF_INET;
		DestinationAddress.sin_port = htons(FIND_SERVER_PORT);
		DestinationAddress.sin_addr.s_addr = INADDR_BROADCAST;

		// Send the ping packet
		if(sendto(SearchSocket, (const char *) pFindServPacket, 1 * sizeof(DWORD), 0, (struct sockaddr *) &DestinationAddress, sizeof(DestinationAddress)) == SOCKET_ERROR)
		{
			MyLog(hLog, XLL_FAIL, "Couldn't send data: %u", WSAGetLastError());
			goto EXIT;
		}

//		MyLog(hLog, XLL_INFO, "Sent FINDSERV packet to server");

		// If read all incoming datagrams until we don't receive any for one second... then retransmit
		while(IsSocketReady(SearchSocket, READABLE, 1) && !fFoundServer)
		{
			
			// See how large the next available datagram is...
			if(ioctlsocket(SearchSocket, FIONREAD, &dwBytesAvailable))
			{
				MyLog(hLog, XLL_INFO, "Couldn't determine if bytes are available on socket");
				goto EXIT;
			}
			
			if(!dwBytesAvailable)
			{
				MyLog(hLog, XLL_WARN, "Possible error, select said socket was readable, but no data available");
				goto EXIT;
			}
			
			// If it's too large, truncate it
			if(dwBytesAvailable > MAX_PACKET_SIZE)
			{
				MyLog(hLog, XLL_INFO, "Received packet was too large: %u bytes", dwBytesAvailable);
				dwBytesAvailable = MAX_PACKET_SIZE;
			}
			
			pGenericPacket = (GENERIC_PACKET *) PacketBuffer;
			nAddressSize = sizeof(DestinationAddress);
			
			// Read the datagram
			if (recvfrom(SearchSocket, (char *) pGenericPacket, dwBytesAvailable, 0, (struct sockaddr *) &DestinationAddress, &nAddressSize) == SOCKET_ERROR)
			{	
				MyLog(hLog, XLL_FAIL, "Couldn't recv data: %u", WSAGetLastError());
				goto EXIT;				
			}
			
			// Determine what type of packet it is
			switch(pGenericPacket->dwPacketType)
			{
			case SERVREPLY:
//				MyLog(hLog, XLL_INFO, "Received SERVREPLY from server");
				if(!inet_ntoa(DestinationAddress.sin_addr))
				{
					MyLog(hLog, XLL_FAIL, "Couldn't convert server address to an IP string");
					continue;
				}
				strcpy(szDestinationAddress, inet_ntoa(DestinationAddress.sin_addr));
				MyLog(hLog, XLL_INFO, "Found server at %s", szDestinationAddress);
				fFoundServer = TRUE;
				break;
			case FINDSERV:
			case TESTREQUEST:
			case SENDRECV:
			case TESTREPLY:
				// Ignore these packets
				break;
			default:
				MyLog(hLog, XLL_INFO, "Received packet is of unknown type: %u", pGenericPacket->dwPacketType);
				goto EXIT;
			}

		}// while socket has incoming datagrams to read

	}// while the server hasn't been found

EXIT:

	if(SearchSocket != INVALID_SOCKET)
		closesocket(SearchSocket);

	return fFoundServer;
}

//==================================================================================
// RequestTest
//----------------------------------------------------------------------------------
//
// Description: Requests a test from the server
//
// Arguments:
//	HANDLE		hLog						Handle to the logging subsystem
//	TEST_CASE	TestCase					Test case being requested
//	CHAR		*szDestinationAddress		IP address of server
// Returns:
//	Returns TRUE if the activity has occured, FALSE otherwise
//==================================================================================
BOOL RequestTest(HANDLE hLog, TEST_CASE TestCase, CHAR *szDestinationAddress)
{
	TESTREQUEST_PACKET *pTestRequestPacket = NULL;
	TESTREPLY_PACKET *pTestReplyPacket = NULL;
	GENERIC_PACKET *pGenericPacket = NULL;
	SOCKADDR_IN DestinationAddress;
	SOCKET RequestSocket = INVALID_SOCKET;
	DWORD	dwBytesAvailable = 0, dwTestRequestID = 0;
	BOOL	fServerAckedRequest = FALSE;
	BYTE	PacketBuffer[MAX_PACKET_SIZE];

#ifdef _XBOX
	SOCKADDR_IN LocalAddress;
#endif

	BOOL	fReturn = FALSE, fEnableBroadcast = TRUE, fFoundServer = FALSE;
	INT		nAddressSize = 0;

	// Create the socket
    if((RequestSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		MyLog(hLog, XLL_FAIL, "Couldn't create DGRAM socket: %u", WSAGetLastError());
		goto EXIT;
	}

#ifdef _XBOX
// BUGBUG - this is a workaround for the fact that Xbox requires UDP sockets
//          to be explicitly bound before sending

	LocalAddress.sin_family = AF_INET;
    LocalAddress.sin_port = htons(FIND_SERVER_PORT);
    LocalAddress.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket to the local address
    if (bind(RequestSocket, (const struct sockaddr *) &LocalAddress, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
		MyLog(hLog, XLL_FAIL, "Couldn't bind search socket: %u", WSAGetLastError());
		goto EXIT;
    }

#endif

	dwTestRequestID = GetTickCount();

	// Continue sending test request packets until we get a responce from the server
	while(!fServerAckedRequest)
	{
		pTestRequestPacket = (TESTREQUEST_PACKET *) PacketBuffer;
		pTestRequestPacket->dwPacketType = TESTREQUEST;
		pTestRequestPacket->dwTestID = dwTestRequestID;
		pTestRequestPacket->TestRequested = TestCase;
		
		DestinationAddress.sin_family = AF_INET;
		DestinationAddress.sin_port = htons(FIND_SERVER_PORT);
		DestinationAddress.sin_addr.s_addr = inet_addr(szDestinationAddress);
		
		// Send the ping packet
		if(sendto(RequestSocket, (const char *) pTestRequestPacket, 3 * sizeof(DWORD), 0, (struct sockaddr *) &DestinationAddress, sizeof(DestinationAddress)) == SOCKET_ERROR)
		{
			MyLog(hLog, XLL_FAIL, "Couldn't send data: %u", WSAGetLastError());
			goto EXIT;
		}
		
//		MyLog(hLog, XLL_INFO, "Sent TESTREQUEST packet to server");
		
		// If read all incoming datagrams until we don't receive any for one second... then retransmit
		while(IsSocketReady(RequestSocket, READABLE, 1) && !fServerAckedRequest)
		{
			
			// See how large the next available datagram is...
			if(ioctlsocket(RequestSocket, FIONREAD, &dwBytesAvailable))
			{
				MyLog(hLog, XLL_FAIL, "Couldn't determine if bytes are available on socket");
				goto EXIT;
			}
			
			if(!dwBytesAvailable)
			{
				MyLog(hLog, XLL_WARN, "Possible bug: select returned that this socket is readable, but no data is waiting");
				continue;
			}
			
			// If it's too large, truncate it
			if(dwBytesAvailable > MAX_PACKET_SIZE)
			{
				MyLog(hLog, XLL_WARN, "Received packet was too large: %u bytes", dwBytesAvailable);
				dwBytesAvailable = MAX_PACKET_SIZE;
			}
			
			pGenericPacket = (GENERIC_PACKET *) PacketBuffer;
			nAddressSize = sizeof(DestinationAddress);
			
			// Read the datagram
			if (recvfrom(RequestSocket, (char *) pGenericPacket, dwBytesAvailable, 0, (struct sockaddr *) &DestinationAddress, &nAddressSize) == SOCKET_ERROR)
			{	
				MyLog(hLog, XLL_FAIL, "Couldn't recv data: %u", WSAGetLastError());
				goto EXIT;				
			}
			
			// Determine what type of packet it is
			switch(pGenericPacket->dwPacketType)
			{
			case TESTREPLY:
//				MyLog(hLog, XLL_INFO, "Received TESTREPLY from server");
				pTestReplyPacket = (TESTREPLY_PACKET *) PacketBuffer;
				
				if((pTestReplyPacket->dwTestID == dwTestRequestID) && (pTestReplyPacket->TestRequested == TestCase))
				{
//					MyLog(hLog, XLL_INFO, "Server acked test request");
					fServerAckedRequest = TRUE;
				}
				else
					MyLog(hLog, XLL_WARN, "Invalid test reply received: %u, %u",
					pTestReplyPacket->dwTestID, pTestReplyPacket->TestRequested);
				break;
			case FINDSERV:
			case TESTREQUEST:
			case SENDRECV:
			case SERVREPLY:
				// Ignore these packets
				break;
			default:
				MyLog(hLog, XLL_INFO, "Received packet is of unknown type: %u", pGenericPacket->dwPacketType);
				goto EXIT;
			}
			
		}// while socket has incoming datagrams to read
		
	}// while the server hasn't acked the test request
	
EXIT:
	if(RequestSocket != INVALID_SOCKET)
		closesocket(RequestSocket);

	return fServerAckedRequest;
}

//==================================================================================
// IsSocketReady
//----------------------------------------------------------------------------------
//
// Description: Creates a listening socket and accepts a connection
//
// Arguments:
//	SOCKET			Socket				Socket to monitor for a given state
//	SOCKET_STATE	SocketState			State to monitor for
// Returns:
//	Returns TRUE if the activity has occured, FALSE otherwise
//==================================================================================
BOOL IsSocketReady(SOCKET Socket, SOCKET_STATE SocketState, DWORD dwSecondsToWait)
{
	fd_set SocketCollection, *ReadCollection, *WriteCollection, *ExceptCollection;
	timeval TimeOut = {0,0}, *pTimeOut = NULL;

	// If we have a bad socket, just report FALSE
	if(Socket == INVALID_SOCKET)
		return FALSE;

	if(dwSecondsToWait != TIME_INFINITY)
	{
		TimeOut.tv_sec = dwSecondsToWait;
		pTimeOut = &TimeOut;
	}

	FD_ZERO(&SocketCollection);
	FD_SET(Socket, &SocketCollection);

	// Depending on the activity to monitor for, setup the parameters to select
	switch(SocketState)
	{
	case READABLE:
		ReadCollection = &SocketCollection;
		WriteCollection = NULL;
		ExceptCollection = NULL;
		break;
	case WRITEABLE:
		ReadCollection = NULL;
		WriteCollection = &SocketCollection;
		ExceptCollection = NULL;
		break;
	default:
		ReadCollection = NULL;
		WriteCollection = NULL;
		ExceptCollection = &SocketCollection;
		break;
	}

	if((select(1, ReadCollection, WriteCollection, ExceptCollection, pTimeOut)) != 1)
		return FALSE;
	else
		return TRUE;
}


//==================================================================================
// Tests
//==================================================================================

//==================================================================================
// DoSendTest
//----------------------------------------------------------------------------------
//
// Description:  Test which establishes a connection and begins a tight send loop.
//
// Arguments:
//	LPVOID			pParam		Pointer to structure containing test parameters
// Returns:
//	TRUE if successful, FALSE otherwise
//==================================================================================
DWORD WINAPI DoSendTest(LPVOID pParam)
{
	SENDRECV_PACKET	*pPacket;
	TEST_PARAMS *pTestParams = (TEST_PARAMS *) pParam;
	SOCKET	Connection;
	DWORD	dwStartTick, dwTotalBytesSent = 0, dwTotalPacketSize = 0, dwReturnValue = TRUE;
	BYTE	PacketBuffer[MAX_PACKET_SIZE + (4 * sizeof(DWORD))];
	INT		nBytesSent = 0;

	pPacket = (SENDRECV_PACKET *) PacketBuffer;
	pPacket->dwPacketNumber = 0;

	MyLog(pTestParams->hLog, XLL_INFO, "Running SendTest");

	// Establish the connection either by initiating one or waiting for an incoming one
	if(pTestParams->fInitiateConnection)
	{
		if(!pTestParams->szDestinationAddress)
		{
			MyLog(pTestParams->hLog, XLL_FAIL, "Cannot establish a connection without a destination address!");
			dwReturnValue = FALSE;
			goto EXIT;
		}

		Connection = EstablishConnection(pTestParams->hLog, pTestParams->szDestinationAddress, pTestParams->fNonBlocking);
	}
	else
		Connection = WaitForIncomingConnection(pTestParams->hLog, pTestParams->fNonBlocking);

	// Get the start time of the test
	dwStartTick = GetTickCount();

	// While the test hasn't reached it's end-time, send data
	while(!IsTimeExpired(pTestParams->dwSecondsToRun, dwStartTick))
	{
		// Create all the packet header fields and build the payload data
		pPacket->dwPacketType = SENDRECV;
		++(pPacket->dwPacketNumber);
		pPacket->dwPayloadSize = RandomRange(1, MAX_PACKET_SIZE);
		pPacket->dwPayloadChecksum = BuildTestPacket(pPacket->dwPayloadSize, (BYTE *) pPacket->bPayload);

		dwTotalPacketSize = pPacket->dwPayloadSize + (4 * sizeof(DWORD));

		// Loop until we send all the data from this packet
		for(dwTotalBytesSent = 0; dwTotalBytesSent < dwTotalPacketSize; dwTotalBytesSent += (DWORD) nBytesSent)
		{
			nBytesSent = 0;
			
			if ((nBytesSent = send(Connection, (char *) pPacket + dwTotalBytesSent, dwTotalPacketSize - dwTotalBytesSent, 0)) == SOCKET_ERROR)
			{
				// If we are non-blocking and we would have blocked... wait for writeability
				if(pTestParams->fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
				{
					while(!IsSocketReady(Connection, WRITEABLE, 10))
					{
						MyLog(pTestParams->hLog, XLL_INFO, "Waiting for socket to become writeable");
					}

					// We didn't send any bytes, the operation was just returning an error
					nBytesSent = 0;
					continue;
				}

				MyLog(pTestParams->hLog, XLL_FAIL, "Couldn't send data: %u", WSAGetLastError());
				dwReturnValue = FALSE;
				goto EXIT;				
			}
			
		}	
		
	} // Test has reached it's end-time

EXIT:

	// Clean up the test socket, we don't need it anymore
	if(Connection != INVALID_SOCKET)
	{
		closesocket(Connection);
		Connection = INVALID_SOCKET;
	}

	pTestParams->fCleanUpAfterTest ? LocalFree(pTestParams) : 0;

	if(dwReturnValue)
		MyLog(pTestParams->hLog, XLL_PASS, "SendTest passed");
	else
		MyLog(pTestParams->hLog, XLL_FAIL, "SendTest failed");

	return dwReturnValue;
}

//==================================================================================
// DoRecvTest
//----------------------------------------------------------------------------------
//
// Description: Test which establishes a connection and begins a tight recv loop with delays.
//
// Arguments:
//	LPVOID			pParams		Pointer to structure containing test parameters
// Returns:
//	TRUE if successful, FALSE otherwise
//==================================================================================
DWORD WINAPI DoRecvTest(LPVOID pParam)
{
	SENDRECV_PACKET	*pPacket;
	TEST_PARAMS *pTestParams = (TEST_PARAMS *) pParam;
	SOCKET	Connection;
	DWORD	dwStartTick, dwTotalBytesRead = 0, dwCheckSum = 0, dwNextDelay = 0, dwNextDelaySize = 0, dwReturnValue = TRUE;
	BYTE	PacketBuffer[MAX_PACKET_SIZE + (4 * sizeof(DWORD))];
	INT		nBytesRead = 0;

	pPacket = (SENDRECV_PACKET *) PacketBuffer;
	pPacket->dwPacketNumber = 0;

	MyLog(pTestParams->hLog, XLL_INFO, "Running RecvTest");

	// Establish the connection either by initiating one or waiting for an incoming one
	if(pTestParams->fInitiateConnection)
	{
		if(!pTestParams->szDestinationAddress)
		{
			MyLog(pTestParams->hLog, XLL_FAIL, "Cannot establish a connection without a destination address!");
			dwReturnValue = FALSE;
			goto EXIT;
		}

		Connection = EstablishConnection(pTestParams->hLog, pTestParams->szDestinationAddress, pTestParams->fNonBlocking);
	}
	else
		Connection = WaitForIncomingConnection(pTestParams->hLog, pTestParams->fNonBlocking);

	// Get the start time of the test
	dwStartTick = GetTickCount();

	// If we are using delays, calculate when the next delay should take place and how long it should last
	if(pTestParams->fUseDelays)
	{
		dwNextDelay = RandomRange(1, 50);
		dwNextDelaySize = RandomRange(1, 5000);
	}

	// While the test hasn't reached it's end-time, send data
	while(!IsTimeExpired(pTestParams->dwSecondsToRun, dwStartTick))
	{
		// If we are using delays, do delay processing
		if(pTestParams->fUseDelays)
		{
			if(!dwNextDelay)
			{
				Sleep(dwNextDelaySize);
				dwNextDelay = RandomRange(1, 100);
				dwNextDelaySize = RandomRange(1, 1000);
			}
			
			--dwNextDelay;
		}
		
		// Loop until we recv all the data from this packet's header
		for(dwTotalBytesRead = 0; dwTotalBytesRead < (4 * sizeof(DWORD)); dwTotalBytesRead += (DWORD) nBytesRead)
		{
			if ((nBytesRead = recv(Connection, (char *) pPacket + dwTotalBytesRead, (4 * sizeof(DWORD)) - dwTotalBytesRead, 0)) == SOCKET_ERROR)
			{
				// If we are non-blocking and we would have blocked... wait for readability
				if(pTestParams->fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
				{
					while(!IsSocketReady(Connection, READABLE, 10))
					{
						MyLog(pTestParams->hLog, XLL_INFO, "Waiting for socket to become readable");
					}

					// No bytes were read, the operation was just reporting that it would have blocked
					nBytesRead = 0;
					continue;
				}

				MyLog(pTestParams->hLog, XLL_FAIL, "Couldn't recv data: %u", WSAGetLastError());
				dwReturnValue = FALSE;
				goto EXIT;				
			}

		}

		// If this isn't a send-recv packet, then something is wrong
		if(pPacket->dwPacketType != SENDRECV)
			MyLog(pTestParams->hLog, XLL_WARN, "Received packet is of wrong type: %u", pPacket->dwPacketType);

		// If the packet is too big, truncate it
		if(pPacket->dwPayloadSize > MAX_PACKET_SIZE)
		{
			MyLog(pTestParams->hLog, XLL_WARN, "Received packet size is too large, truncating.");
			pPacket->dwPayloadSize = MAX_PACKET_SIZE;
		}

		// Loop until we recv all the data from this packet's payload
		for(dwTotalBytesRead = 0; dwTotalBytesRead < pPacket->dwPayloadSize; dwTotalBytesRead += (DWORD) nBytesRead)
		{
			nBytesRead = 0;
			
			if ((nBytesRead = recv(Connection, (char *) pPacket->bPayload + dwTotalBytesRead, pPacket->dwPayloadSize - dwTotalBytesRead, 0)) == SOCKET_ERROR)
			{
				// If we are non-blocking and we would have blocked... wait for readability
				if(pTestParams->fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
				{
					while(!IsSocketReady(Connection, READABLE, 10))
					{
						MyLog(pTestParams->hLog, XLL_INFO, "Waiting for socket to become readable");
					}
					// No bytes were read, the operation was just reporting that it would have blocked
					nBytesRead = 0;
					continue;
				}

				MyLog(pTestParams->hLog, XLL_FAIL, "Couldn't recv data: %u", WSAGetLastError());
				dwReturnValue = FALSE;
				goto EXIT;				
			}	
		}


		dwCheckSum = CalculateCheckSum(pPacket->dwPayloadSize, (BYTE *) pPacket->bPayload);

		if(dwCheckSum != pPacket->dwPayloadChecksum)
			MyLog(pTestParams->hLog, XLL_WARN, "Received packet %u checksum failed: %u",
			pPacket->dwPacketNumber, dwCheckSum);

	} // Test has reached it's end-time

EXIT:

	// Clean up the test socket, we don't need it anymore
	if(Connection != INVALID_SOCKET)
	{
		closesocket(Connection);
		Connection = INVALID_SOCKET;
	}

	pTestParams->fCleanUpAfterTest ? LocalFree(pTestParams) : 0;

	if(dwReturnValue)
		MyLog(pTestParams->hLog, XLL_PASS, "RecvTest passed");
	else
		MyLog(pTestParams->hLog, XLL_FAIL, "RecvTest failed");

	return dwReturnValue;

}

//==================================================================================
// SendRecvCountThread
//----------------------------------------------------------------------------------
//
// Description: Thread that periodically prints total number of send and recv calls
//
// Arguments:
//	LPVOID		*pParam				Pointer to the ECHO_CLIENT_PARAM structure for this thread
// Returns:
//	always 0
//==================================================================================
DWORD WINAPI ServerEchoCountThread(LPVOID *pParam)
{
	ECHO_SERVER_PARAM	*pEchoServerParam = (ECHO_SERVER_PARAM *) pParam;

	// While the test hasn't reached it's end-time, print send/recv totals
	while(pEchoServerParam->fTestActive)
	{
		Sleep(2000);
		MyLog(pEchoServerParam->hLog, XLL_INFO, "CountThread: %u echos, %s, %s",
			pEchoServerParam->dwTotalEchos,
			pEchoServerParam->fInSend ? "InSend" : "NotInSend",
			pEchoServerParam->fInRecv ? "InRecv" : "NotInRecv");
	}

	return 0;
}

//==================================================================================
// DoSingleThreadEchoServerTest
//----------------------------------------------------------------------------------
//
// Description: Test which establishes a connection and echos packets it receives in a single thread
//
// Arguments:
//	LPVOID			pParams		Pointer to structure containing test parameters
// Returns:
//	TRUE if successful, FALSE otherwise
//==================================================================================
DWORD WINAPI DoSingleThreadEchoServerTest(LPVOID pParam)
{
	ECHO_SERVER_PARAM EchoServerParam;
	TEST_PARAMS *pTestParams = (TEST_PARAMS *) pParam;
	SENDRECV_PACKET	*pPacket;
	HANDLE	hCountThread = INVALID_HANDLE_VALUE;
	SOCKET	Connection;
	DWORD	dwStartTick, dwTotalBytesRead = 0, dwCheckSum = 0, dwTotalBytesSent = 0, dwTotalPacketSize = 0, dwReturnValue = TRUE;
	BYTE	PacketBuffer[MAX_PACKET_SIZE + (4 * sizeof(DWORD))];
	INT		nBytesRead = 0, nBytesSent = 0;

	pPacket = (SENDRECV_PACKET *) PacketBuffer;
	pPacket->dwPacketNumber = 0;

	MyLog(pTestParams->hLog, XLL_INFO, "Running SingleThreadEchoServerTest");

	// Establish the connection either by initiating one or waiting for an incoming one
	if(pTestParams->fInitiateConnection)
	{
		if(!pTestParams->szDestinationAddress)
		{
			MyLog(pTestParams->hLog, XLL_FAIL, "Cannot establish a connection without a destination address!");
			dwReturnValue = FALSE;
			goto EXIT;
		}

		Connection = EstablishConnection(pTestParams->hLog, pTestParams->szDestinationAddress, FALSE);
	}
	else
		Connection = WaitForIncomingConnection(pTestParams->hLog, FALSE);

	// Start counting send's from zero
	EchoServerParam.dwTotalEchos = 0;
	EchoServerParam.fInSend = FALSE;
	EchoServerParam.fInRecv = FALSE;
	EchoServerParam.fTestActive = TRUE;
	EchoServerParam.hLog = pTestParams->hLog;

	// Spawn the count thread
//	if((hCountThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ServerEchoCountThread, (LPVOID) &EchoServerParam, 0, NULL)) == NULL)
//	{
//		MyLog(pTestParams->hLog, XLL_WARN, "Couldn't create thread: %u", GetLastError());
//		goto EXIT;
//	}

	// Get the start time of the test
	dwStartTick = GetTickCount();

	// While the test hasn't reached it's end-time, send data
	while(!IsTimeExpired(pTestParams->dwSecondsToRun, dwStartTick))
	{
		// Loop until we recv all the data from this packet's header
		for(dwTotalBytesRead = 0; dwTotalBytesRead < (4 * sizeof(DWORD)); dwTotalBytesRead += (DWORD) nBytesRead)
		{
			EchoServerParam.fInRecv = TRUE;
			if ((nBytesRead = recv(Connection, (char *) pPacket + dwTotalBytesRead, (4 * sizeof(DWORD)) - dwTotalBytesRead, 0)) == SOCKET_ERROR)
			{
				EchoServerParam.fInRecv = FALSE;

				MyLog(pTestParams->hLog, XLL_FAIL, "Couldn't recv data: %u", WSAGetLastError());
				dwReturnValue = FALSE;
				goto EXIT;				
			}
			EchoServerParam.fInRecv = FALSE;
		}

		// If this isn't a send-recv packet, then something is wrong
		if(pPacket->dwPacketType != SENDRECV)
			MyLog(pTestParams->hLog, XLL_WARN, "Received packet is of wrong type: %u", pPacket->dwPacketType);
		
		// If the packet is too big, truncate it
		if(pPacket->dwPayloadSize > MAX_PACKET_SIZE)
		{
			// The test is probably screwed at this point
			MyLog(pTestParams->hLog, XLL_WARN, "Received packet size is too large, truncating.");
			pPacket->dwPayloadSize = MAX_PACKET_SIZE;
		}

		// Loop until we recv all the data from this packet's payload
		for(dwTotalBytesRead = 0; dwTotalBytesRead < pPacket->dwPayloadSize; dwTotalBytesRead += (DWORD) nBytesRead)
		{
			nBytesRead = 0;
			
			EchoServerParam.fInRecv = TRUE;
			if ((nBytesRead = recv(Connection, (char *) pPacket->bPayload + dwTotalBytesRead, pPacket->dwPayloadSize - dwTotalBytesRead, 0)) == SOCKET_ERROR)
			{
				EchoServerParam.fInRecv = FALSE;

				MyLog(pTestParams->hLog, XLL_FAIL, "Couldn't recv data: %u", WSAGetLastError());
				dwReturnValue = FALSE;
				goto EXIT;				
			}	
			EchoServerParam.fInRecv = FALSE;
		}

		// Check to see if the packe was corrupted
		dwCheckSum = CalculateCheckSum(pPacket->dwPayloadSize, (BYTE *) pPacket->bPayload);
		if(dwCheckSum != pPacket->dwPayloadChecksum)
			MyLog(pTestParams->hLog, XLL_WARN, "Received packet %u checksum failed: %u",
			pPacket->dwPacketNumber, dwCheckSum);

		dwTotalPacketSize = pPacket->dwPayloadSize + (4 * sizeof(DWORD));

		// Loop until we send all the data from this packet
		for(dwTotalBytesSent = 0; dwTotalBytesSent < dwTotalPacketSize; dwTotalBytesSent += (DWORD) nBytesSent)
		{
			nBytesSent = 0;
			
			EchoServerParam.fInSend = TRUE;
			if ((nBytesSent = send(Connection, (char *) pPacket + dwTotalBytesSent, dwTotalPacketSize - dwTotalBytesSent, 0)) == SOCKET_ERROR)
			{
				EchoServerParam.fInSend = FALSE;

				MyLog(pTestParams->hLog, XLL_FAIL, "Couldn't send data: %u", WSAGetLastError());
				dwReturnValue = FALSE;
				goto EXIT;				
			}
			EchoServerParam.fInSend = FALSE;
			
		}	

		++EchoServerParam.dwTotalEchos;

	} // Test has reached it's end-time

	EchoServerParam.fTestActive = FALSE;

	WaitForSingleObject(hCountThread, INFINITE);

EXIT:

	if(hCountThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hCountThread);
		hCountThread = INVALID_HANDLE_VALUE;
	}

	// Clean up the test socket, we don't need it anymore
	if(Connection != INVALID_SOCKET)
	{
		closesocket(Connection);
		Connection = INVALID_SOCKET;
	}

	pTestParams->fCleanUpAfterTest ? LocalFree(pTestParams) : 0;

	if(dwReturnValue)
		MyLog(pTestParams->hLog, XLL_PASS, "SingleThreadEchoServerTest passed");
	else
		MyLog(pTestParams->hLog, XLL_FAIL, "SingleThreadEchoServerTest failed");

	return dwReturnValue;

}

//==================================================================================
// SendRecvCountThread
//----------------------------------------------------------------------------------
//
// Description: Thread that periodically prints total number of send and recv calls
//
// Arguments:
//	LPVOID		*pParam				Pointer to the ECHO_CLIENT_PARAM structure for this thread
// Returns:
//	always 0
//==================================================================================
DWORD WINAPI SendRecvCountThread(LPVOID *pParam)
{
	ECHO_CLIENT_PARAM	*pEchoClientParam = (ECHO_CLIENT_PARAM *) pParam;

	// While the test hasn't reached it's end-time, print send/recv totals
	while(pEchoClientParam->fTestActive)
	{
		Sleep(2000);
		MyLog(pEchoClientParam->pTestParams->hLog, XLL_INFO, "CountThread: %u Sends, %u Recvs, %s, %s",
			pEchoClientParam->dwTotalSends,
			pEchoClientParam->dwTotalRecvs,
			pEchoClientParam->fInSend ? "InSend" : "NotInSend",
			pEchoClientParam->fInRecv ? "InRecv" : "NotInRecv");
	}

	return 0;
}

//==================================================================================
// EchoClientRecvThread
//----------------------------------------------------------------------------------
//
// Description: Thread that receives echoed packets from the echo server
//
// Arguments:
//	LPVOID		*pParam				Pointer to the ECHO_CLIENT_PARAM structure for this thread
// Returns:
//	always 0
//==================================================================================
DWORD WINAPI EchoClientRecvThread(LPVOID *pParam)
{
	ECHO_CLIENT_PARAM	*pEchoClientParam = (ECHO_CLIENT_PARAM *) pParam;
	SENDRECV_PACKET	*pPacket;
	DWORD	dwTotalBytesRead = 0, dwCheckSum = 0, dwNextDelay = 0, dwNextDelaySize = 0;
	BYTE	PacketBuffer[MAX_PACKET_SIZE + (4 * sizeof(DWORD))];
	INT		nBytesRead = 0;

	pPacket = (SENDRECV_PACKET *) PacketBuffer;
	pPacket->dwPacketNumber = 0;

	MyLog(pEchoClientParam->pTestParams->hLog, XLL_INFO, "EchoClientRecvThread running");

	// If we are using delays, calculate when the next delay should take place and how long it should last
	if(pEchoClientParam->pTestParams->fUseDelays)
	{
		dwNextDelay = RandomRange(1, 100);
		dwNextDelaySize = RandomRange(1, 1000);
	}

	// Start counting recv's from zero
	pEchoClientParam->dwTotalRecvs = 0;

	// While the test hasn't reached it's end-time, recv data
	while(pEchoClientParam->fTestActive)
	{
		// If we are using delays, do delay processing
		if(pEchoClientParam->pTestParams->fUseDelays)
		{
			if(!dwNextDelay)
			{
				Sleep(dwNextDelaySize);
				dwNextDelay = RandomRange(1, 100);
				dwNextDelaySize = RandomRange(1, 1000);
			}
			
			--dwNextDelay;
		}
		
		// Loop until we recv all the data from this packet's header
		for(dwTotalBytesRead = 0; dwTotalBytesRead < (4 * sizeof(DWORD)); dwTotalBytesRead += (DWORD) nBytesRead)
		{
			// If we are using critical sections, lock the socket
			if(pEchoClientParam->pTestParams->fUseCriticalSection)
				EnterCriticalSection(&(pEchoClientParam->SocketCriticalSection));
			// Otherwise, wait until the socket is marked as readable so we don't block in recv for too long
			else
			{
				while(!IsSocketReady(pEchoClientParam->Connection, READABLE, 10))
				{
					MyLog(pEchoClientParam->pTestParams->hLog, XLL_INFO, "Waiting for socket to become readable");
				}
			}

			pEchoClientParam->fInRecv = TRUE;

			if ((nBytesRead = recv(pEchoClientParam->Connection, (char *) pPacket + dwTotalBytesRead, (4 * sizeof(DWORD)) - dwTotalBytesRead, 0)) == SOCKET_ERROR)
			{
				pEchoClientParam->fInRecv = FALSE;

				// Unlock the socket
				pEchoClientParam->pTestParams->fUseCriticalSection ? LeaveCriticalSection(&(pEchoClientParam->SocketCriticalSection)) : 0;

				// If we are non-blocking and we would have blocked... wait for readability
				if(pEchoClientParam->pTestParams->fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
				{
					while(!IsSocketReady(pEchoClientParam->Connection, READABLE, 10))
					{
						MyLog(pEchoClientParam->pTestParams->hLog, XLL_INFO, "Waiting for socket to become readable");
					}
					// No bytes were read, the operation was just reporting that it would have blocked
					nBytesRead = 0;
					continue;
				}

				if((WSAGetLastError() == WSAEINPROGRESS) && !pEchoClientParam->pTestParams->fUseCriticalSection)
				{
					++pEchoClientParam->dwInProgressOnRecvs;
					Sleep(500);
					continue;
				}

				MyLog(pEchoClientParam->pTestParams->hLog, XLL_FAIL, "Couldn't recv data: %u", WSAGetLastError());
				goto EXIT;				
			}

			pEchoClientParam->fInRecv = FALSE;
			++pEchoClientParam->dwTotalRecvs;

			// Unlock the socket
			pEchoClientParam->pTestParams->fUseCriticalSection ? LeaveCriticalSection(&(pEchoClientParam->SocketCriticalSection)) : 0;
		}

		// If this isn't a send-recv packet, then something is wrong
		if(pPacket->dwPacketType != SENDRECV)
			MyLog(pEchoClientParam->pTestParams->hLog, XLL_WARN, "Received packet is of wrong type: %u", pPacket->dwPacketType);

		// If the packet is too big, truncate it
		if(pPacket->dwPayloadSize > MAX_PACKET_SIZE)
		{
			MyLog(pEchoClientParam->pTestParams->hLog, XLL_WARN, "Received packet size is too large, truncating.");
			pPacket->dwPayloadSize = MAX_PACKET_SIZE;
		}

		// Loop until we recv all the data from this packet's payload
		for(dwTotalBytesRead = 0; dwTotalBytesRead < pPacket->dwPayloadSize; dwTotalBytesRead += (DWORD) nBytesRead)
		{
			nBytesRead = 0;
			
			// If we are using critical sections, lock the socket
			if(pEchoClientParam->pTestParams->fUseCriticalSection)
				EnterCriticalSection(&(pEchoClientParam->SocketCriticalSection));
			// Otherwise, wait until the socket is marked as readable so we don't block in recv for too long
			else
				while(!IsSocketReady(pEchoClientParam->Connection, READABLE, 10));
				{
					MyLog(pEchoClientParam->pTestParams->hLog, XLL_INFO, "Waiting for socket to become readable");
				}

			pEchoClientParam->fInRecv = TRUE;

			if ((nBytesRead = recv(pEchoClientParam->Connection, (char *) pPacket->bPayload + dwTotalBytesRead, pPacket->dwPayloadSize - dwTotalBytesRead, 0)) == SOCKET_ERROR)
			{
				pEchoClientParam->fInRecv = FALSE;

				// Unlock the socket
				pEchoClientParam->pTestParams->fUseCriticalSection ? LeaveCriticalSection(&(pEchoClientParam->SocketCriticalSection)) : 0;

				// If we are non-blocking and we would have blocked... wait for readability
				if(pEchoClientParam->pTestParams->fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
				{
					while(!IsSocketReady(pEchoClientParam->Connection, READABLE, 10))
					{
						MyLog(pEchoClientParam->pTestParams->hLog, XLL_INFO, "Waiting for socket to become readable");
					}
					// No bytes were read, the operation was just reporting that it would have blocked
					nBytesRead = 0;
					continue;
				}

				if((WSAGetLastError() == WSAEINPROGRESS) && !pEchoClientParam->pTestParams->fUseCriticalSection)
				{
					++pEchoClientParam->dwInProgressOnRecvs;
					Sleep(500);
					continue;
				}

				MyLog(pEchoClientParam->pTestParams->hLog, XLL_FAIL, "Couldn't recv data: %u", WSAGetLastError());
				goto EXIT;				
			}

			pEchoClientParam->fInRecv = FALSE;
			++pEchoClientParam->dwTotalRecvs;

			// Unlock the socket
			pEchoClientParam->pTestParams->fUseCriticalSection ? LeaveCriticalSection(&(pEchoClientParam->SocketCriticalSection)) : 0;
		}


		dwCheckSum = CalculateCheckSum(pPacket->dwPayloadSize, (BYTE *) pPacket->bPayload);

		if(dwCheckSum != pPacket->dwPayloadChecksum)
			MyLog(pEchoClientParam->pTestParams->hLog, XLL_WARN, "Received packet %u checksum failed: %u", 
			pPacket->dwPacketNumber, dwCheckSum);

	} // Test has reached it's end-time

EXIT:

	return 0;
}

//==================================================================================
// DoMultiThreadEchoClientTest
//----------------------------------------------------------------------------------
//
// Description:  Test which establishes a connection and begins a tight send loop.
//
// Arguments:
//	LPVOID			pParam		Pointer to structure containing test parameters
// Returns:
//	TRUE if successful, FALSE otherwise
//==================================================================================
DWORD WINAPI DoMultiThreadEchoClientTest(LPVOID pParam)
{
	ECHO_CLIENT_PARAM	EchoClientParam;
	TEST_PARAMS *pTestParams = (TEST_PARAMS *) pParam;
	SENDRECV_PACKET	*pPacket;
	HANDLE	hRecvThread = NULL, hCountThread = NULL;
	DWORD	dwStartTick, dwTotalBytesSent = 0, dwTotalPacketSize = 0, dwReturnValue = TRUE;
	BYTE	PacketBuffer[MAX_PACKET_SIZE + (4 * sizeof(DWORD))];
	INT		nBytesSent = 0;

	pPacket = (SENDRECV_PACKET *) PacketBuffer;
	pPacket->dwPacketNumber = 0;

	MyLog(pTestParams->hLog, XLL_INFO, "Running MultiThreadEchoClientTest");

	// Initialize the parameter for the recv thread
	pTestParams->fUseCriticalSection ? InitializeCriticalSection(&(EchoClientParam.SocketCriticalSection)) : 0;
	EchoClientParam.pTestParams = pTestParams;
	EchoClientParam.Connection = INVALID_SOCKET;

	// Establish the connection either by initiating one or waiting for an incoming one
	if(pTestParams->fInitiateConnection)
	{
		if(!pTestParams->szDestinationAddress)
		{
			MyLog(pTestParams->hLog, XLL_FAIL, "Cannot establish a connection without a destination address!");
			dwReturnValue = FALSE;
			goto EXIT;
		}

			EchoClientParam.Connection = EstablishConnection(pTestParams->hLog, pTestParams->szDestinationAddress, pTestParams->fNonBlocking);
	}
	else
		EchoClientParam.Connection = WaitForIncomingConnection(pTestParams->hLog, pTestParams->fNonBlocking);

	// Start counting send's from zero
	EchoClientParam.fTestActive = TRUE;
	EchoClientParam.dwInProgressOnRecvs = 0;
	EchoClientParam.dwInProgressOnSends = 0;
	EchoClientParam.dwTotalSends = 0;
	EchoClientParam.dwTotalRecvs = 0;
	EchoClientParam.fInSend = FALSE;
	EchoClientParam.fInRecv = FALSE;

	// Spawn the count thread
	if((hCountThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SendRecvCountThread, (LPVOID) &EchoClientParam, 0, NULL)) == INVALID_HANDLE_VALUE)
	{
		MyLog(pTestParams->hLog, XLL_WARN, "Couldn't create thread: %u", GetLastError());
		dwReturnValue = FALSE;
		goto EXIT;
	}

	// Spawn the recv thread
	if((hRecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) EchoClientRecvThread, (LPVOID) &EchoClientParam, 0, NULL)) == NULL)
	{
		MyLog(pTestParams->hLog, XLL_WARN, "Couldn't create thread: %u", GetLastError());
		dwReturnValue = FALSE;
		goto EXIT;
	}

	// Get the start time of the test
	dwStartTick = GetTickCount();

	// While the test hasn't reached it's end-time, send data
	while(!IsTimeExpired(pTestParams->dwSecondsToRun, dwStartTick))
	{
		// Create all the packet header fields and build the payload data
		pPacket->dwPacketType = SENDRECV;
		++(pPacket->dwPacketNumber);
		pPacket->dwPayloadSize = RandomRange(1, MAX_PACKET_SIZE);
		pPacket->dwPayloadChecksum = BuildTestPacket(pPacket->dwPayloadSize, (BYTE *) pPacket->bPayload);

		dwTotalPacketSize = pPacket->dwPayloadSize + (4 * sizeof(DWORD));

		// Loop until we send all the data from this packet
		for(dwTotalBytesSent = 0; dwTotalBytesSent < dwTotalPacketSize; dwTotalBytesSent += (DWORD) nBytesSent)
		{
			nBytesSent = 0;
			
			// If we are using critical sections, lock the socket
			if(pTestParams->fUseCriticalSection)
				EnterCriticalSection(&(EchoClientParam.SocketCriticalSection));
			// Otherwise, wait until the socket is marked as writable so we don't block in send for too long
			else
				while(!IsSocketReady(EchoClientParam.Connection, WRITEABLE, 10));
				{
					MyLog(pTestParams->hLog, XLL_INFO, "Waiting for socket to become writeable");
				}

			EchoClientParam.fInSend = TRUE;

			if ((nBytesSent = send(EchoClientParam.Connection, (char *) pPacket + dwTotalBytesSent, dwTotalPacketSize - dwTotalBytesSent, 0)) == SOCKET_ERROR)
			{
				EchoClientParam.fInSend = FALSE;
				
				// Unlock the socket
				pTestParams->fUseCriticalSection ? LeaveCriticalSection(&(EchoClientParam.SocketCriticalSection)) : 0;

				// If we are non-blocking and we would have blocked... wait for writeability
				if(pTestParams->fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
				{
					while(!IsSocketReady(EchoClientParam.Connection, WRITEABLE, 10))
					{
						MyLog(pTestParams->hLog, XLL_INFO, "Waiting for socket to become writeable");
					}
					// No bytes were sent, the operation was just reporting that it would have blocked
					nBytesSent = 0;
					continue;
				}

				// If another blocking operation was in progress, try again later
				if((WSAGetLastError() == WSAEINPROGRESS) && !pTestParams->fUseCriticalSection)
				{
					++EchoClientParam.dwInProgressOnSends;
					continue;
				}

				MyLog(pTestParams->hLog, XLL_FAIL, "Couldn't send data: %u", WSAGetLastError());
				dwReturnValue = FALSE;
				goto EXIT;				
			}

			EchoClientParam.fInSend = FALSE;
			++EchoClientParam.dwTotalSends;

			// Unlock the socket
			pTestParams->fUseCriticalSection ? LeaveCriticalSection(&(EchoClientParam.SocketCriticalSection)) : 0;
		}	
		
	} // Test has reached it's end-time

EXIT:

	// Signal the recv thread to end
	EchoClientParam.fTestActive = FALSE;

	// Wait for the threads to tend
	if(hRecvThread)
	{
		WaitForSingleObject(hRecvThread, INFINITE);
		MyLog(pTestParams->hLog, XLL_INFO, "RecvThread signalled completion");
		CloseHandle(hRecvThread);
		hRecvThread = INVALID_HANDLE_VALUE;
	}
	if(hCountThread)
	{
		WaitForSingleObject(hCountThread, INFINITE);
		MyLog(pTestParams->hLog, XLL_INFO, "CountThread signalled completion");
		CloseHandle(hCountThread);
		hCountThread = INVALID_HANDLE_VALUE;
	}

	// Clean up the critical section if we created it
	pTestParams->fUseCriticalSection ? DeleteCriticalSection(&(EchoClientParam.SocketCriticalSection)) : 0;

	// Clean up the test socket, we don't need it anymore
	if(EchoClientParam.Connection != INVALID_SOCKET)
	{
		closesocket(EchoClientParam.Connection);
		EchoClientParam.Connection = INVALID_SOCKET;
	}

	pTestParams->fCleanUpAfterTest ? LocalFree(pTestParams) : 0;

	if(dwReturnValue)
		MyLog(pTestParams->hLog, XLL_PASS, "MultiThreadEchoClientTest passed");
	else
		MyLog(pTestParams->hLog, XLL_FAIL, "MultiThreadEchoClientTest failed");

	return dwReturnValue;
}
