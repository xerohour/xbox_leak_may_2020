//==================================================================================
// Includes
//==================================================================================
#include "tcptest.h"

//==================================================================================
// Globals
//==================================================================================
// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( tcptest )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( tcptest )
    EXPORT_TABLE_ENTRY( "StartTest", TcpTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", TcpTestEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", TcpTestDllMain )
END_EXPORT_TABLE( tcptest )

// Test specific globals
static CRITICAL_SECTION g_csSerializeAccess;
static HANDLE g_hLog = INVALID_HANDLE_VALUE;
static PTCP_TEST_FUNCTION g_TestFunctions[TCPMSG_MAXTEST] = 
{
	TCPSeqNumMaxTest,													// TCPMSG_SEQNUM_MAX,
	TCPSeqNumMinTest,													// TCPMSG_SEQNUM_MIN,
	TCPDataOffBelowMinTest,												// TCPMSG_DATAOFF_BELOWMIN,
	TCPDataOffAboveAvailTest											// TCPMSG_DATAOFF_ABOVEAVAIL,
#if 0
	TCPReservedSetTest,													// TCPMSG_RESERVED_SET,
	TCPWindowMinTest,													// TCPMSG_WINDOW_MIN,
	TCPWindowMaxTest,													// TCPMSG_WINDOW_MAX,
	TCPChecksumBadTest,													// TCPMSG_CHECKSUM_BAD,
	TCPUrgentAfterEndTest,												// TCPMSG_URGENT_AFTEREND,
	TCPUrgentAtEndTest,													// TCPMSG_URGENT_ATEND,
	TCPUrgentBeforeEndTest,												// TCPMSG_URGENT_BEFOREEND,
	TCPUrgentMaxTest,													// TCPMSG_URGENT_MAX,
	TCPUrgentNoFlagTest,												// TCPMSG_URGENT_NOFLAG,
	TCPOptCutoffTest,													// TCPMSG_OPT_CUTOFF,
	TCPOptAtEndTest,													// TCPMSG_OPT_ATEND,
	TCPOptBeforeEndTest,												// TCPMSG_OPT_BEFOREEND,
	TCPOptNoOptsTest,													// TCPMSG_OPT_NOOPTIONS,
	TCPOptMaxOptsTest,													// TCPMSG_OPT_MAXOPTIONS,
	TCPOptMaxOptLenTest,												// TCPMSG_OPT_MAXOPTLEN,
	TCPOptEndOfOptsTest,												// TCPMSG_OPT_ENDOPTFIRST,
	TCPOptMssZeroTest,													// TCPMSG_OPT_MSSZERO,
	TCPOptMssOneTest,													// TCPMSG_OPT_MSSONE,
	TCPOptMssMaxTest,													// TCPMSG_OPT_MSSMAX,
	TCPConnSynAndNoAckTest,												// TCPMSG_CONN_SYNANDNOACK,
	TCPConnExtraSynTest,												// TCPMSG_CONN_EXTRASYN,
	TCPConnAckWrongSynTest,												// TCPMSG_CONN_ACKWRONGSYN,
	TCPConnSynAckWrongSynTest,											// TCPMSG_CONN_SYNACKWRONGSYN,
	TCPConnAckNoConnTest,												// TCPMSG_CONN_ACKNOCONN,
	TCPConnSynAckListenerTest,											// TCPMSG_CONN_SYNACKLISTENER,
	TCPShutFinAndNoAckTest,												// TCPMSG_SHUT_FINANDNOACK
#endif
};


static LPSTR g_TestNames[TCPMSG_MAXTEST] = 
{
	"TCP conn request with max seq num",								// TCPMSG_SEQNUM_MAX,
	"TCP conn request with min seq num",								// TCPMSG_SEQNUM_MIN,
	"TCP conn request with data off too small",							// TCPMSG_DATAOFF_BELOWMIN,
	"TCP conn request with data off too large"							// TCPMSG_DATAOFF_ABOVEAVAIL,
};


//==================================================================================
// TcpTestDllMain
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
BOOL WINAPI TcpTestDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
	// We'll initialize/delete the global critical section here
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		XNetAddRef();
		InitializeCriticalSection(&g_csSerializeAccess);
		break;
	case DLL_PROCESS_DETACH:
		XNetRelease();
		DeleteCriticalSection(&g_csSerializeAccess);
		break;
	default:
		break;
	}

    return TRUE;
}

//==================================================================================
// TcpTestStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI TcpTestStartTest(IN HANDLE  hLog)
{
	NETSYNC_TYPE_CALLBACK CallbackSessionInfo;
	TCP_TESTREQ TestRequest;
	IN_ADDR ServerAddr;
    WSADATA WSAData;
	HANDLE hClientObject = NULL;
	DWORD dwTestDone;
	WORD LowPort, HighPort;
	INT i, nRet = 0;
	BYTE   MacAddr[6];
	DWORD  dwType = 0, dwSize = 0, dwErrorCode = 0;

	// Since we have to be thread safe, serialize entry for now
	EnterCriticalSection(&g_csSerializeAccess);

	g_hLog = hLog;

    // Set XLog info
	xSetOwnerAlias(g_hLog, "tristanj");
    xSetComponent(g_hLog, "Network(S)", "TCP/IP (S-n)");
	xSetFunctionName(g_hLog, "recvfrom");

    // Init winsock
    if(nRet = WSAStartup(MAKEWORD(2, 2), &WSAData))
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't init winsock: %d", nRet);
		goto Exit;
	}

	xLog(g_hLog, XLL_INFO, "Winsock layer initialized");

	// Get the mac address
	dwErrorCode = XQueryValue(XC_FACTORY_ETHERNET_ADDR, &dwType, MacAddr, sizeof(MacAddr), &dwSize);
	if (ERROR_SUCCESS != dwErrorCode) {
		xLog(g_hLog, XLL_FAIL, "Couldn't get ethernet address: 0x%08x", dwErrorCode);
		goto Exit;
	}

	sprintf(TestRequest.MacAddr, "_%02x%02x%02x%02x%02x%02x",
		MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
	xLog(g_hLog, XLL_INFO, "Local ethernet address: %s", TestRequest.MacAddr);

	// Prepare to find a server that can run the TCP tests
	CallbackSessionInfo.byMinClientCount = 0;
	CallbackSessionInfo.byMaxClientCount = 0;
	CallbackSessionInfo.nMinPortCount = 1;
	CallbackSessionInfo.lpszDllName_W = TCP_SERVER_DLL_W;
	CallbackSessionInfo.lpszMainFunction_A = TCP_SERVER_MAIN_FUNCTION_A;
	CallbackSessionInfo.lpszStartFunction_A = TCP_SERVER_START_FUNCTION_A;
	CallbackSessionInfo.lpszStopFunction_A = TCP_SERVER_STOP_FUNCTION_A;

	// Create a client to communicate with that server
	hClientObject = NetsyncCreateClient(0, NETSYNC_SESSION_CALLBACK, &CallbackSessionInfo, &ServerAddr.S_un.S_addr, NULL, &LowPort, &HighPort);
	if (hClientObject == INVALID_HANDLE_VALUE)
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't create Netsync client");
		goto Exit;
	}

	DbgPrint("TCPTEST: Starting tests...\n");

	// Run the tests
	for(i = 0; i < TCPMSG_MAXTEST; ++i)
	{
		IN_ADDR FromAddr;
		DWORD dwReceiveStatus, dwSendResult;

		TestRequest.dwMessageId = TCPMSG_TESTREQ;
		TestRequest.TestId = (TCPMSG_TESTTYPES) i;

		xStartVariation(g_hLog, g_TestNames[i]);
		xLog(g_hLog, XLL_INFO, "Started test %u", i);

		// Send the test request to the Netsync server
		dwSendResult = NetsyncSendClientMessage(hClientObject, 0, sizeof(TCP_TESTREQ), (char *) &TestRequest);
		if(dwSendResult != WAIT_OBJECT_0)
		{
			xLog(g_hLog, XLL_INFO, "Error sending message to TCP server");
			DbgPrint("TCPTEST: Error during test, aborting...\n");
			goto Exit;
		}

		// If we get here, then the Netsync server successfully acked the test.
		// Now run the client side of the test

		if(g_TestFunctions[i](LowPort, HighPort))
			xLog(g_hLog, XLL_PASS, "Test passed!");
		else
			xLog(g_hLog, XLL_FAIL, "Test failed!");

		xEndVariation(g_hLog);

		Sleep(5000);
	}

	DbgPrint("TCPTEST: Tests complete\n");

Exit:

	// Tell the netsync server we're done
	dwTestDone = TCPMSG_TESTDONE;
	NetsyncSendClientMessage(hClientObject, 0, 4, (char *) &dwTestDone);
	
	hClientObject ? NetsyncCloseClient(hClientObject) : 0;

	// Clean up winsock
	xLog(g_hLog, XLL_INFO, "Cleaning up Winsock layer");
	WSACleanup();

	g_hLog = INVALID_HANDLE_VALUE;

	// Allow other threads to enter now
	LeaveCriticalSection(&g_csSerializeAccess);

}            

//==================================================================================
// TcpTestEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI TcpTestEndTest()
{
	// We already cleaned up everything at the end of StartTest
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
static BOOL IsSocketReady(SOCKET Socket, SOCKET_STATE SocketState, DWORD dwSecondsToWait)
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
// AcceptIncomingConnection
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet with with a certain set of properties
//
// Arguments:
//	WORD			LocalPort			Port to receive the packet on
//	WORD			ExpectedRemotePort	Remote port that should be reported for packet
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL AcceptIncomingConnection(WORD LocalPort, WORD ExpectedRemotePort)
{
	SOCKET ListenerSocket = INVALID_SOCKET, AcceptedConnection = INVALID_SOCKET;
	SOCKADDR_IN LocalAddress, RemoteAddress;
	LINGER LingerOption;
	DWORD dwBytesAvailable = 0, dwBufferSize = 0, dwBytesReceived = 0;
	BOOL fPassed = TRUE;
	INT nRemoteAddressSize = sizeof(SOCKADDR_IN);

	// Create the socket
	ListenerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(ListenerSocket == INVALID_SOCKET)
	{
		xLog(g_hLog, XLL_WARN, "Couldn't create socket!");
		fPassed = FALSE;
		goto Exit;
	}

	// Setup the local binding info
	LocalAddress.sin_addr.s_addr = INADDR_ANY;
	LocalAddress.sin_family = AF_INET;
	LocalAddress.sin_port = htons(LocalPort);

	// Bind the socket
	if(bind(ListenerSocket, (const struct sockaddr *) &LocalAddress, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		xLog(g_hLog, XLL_WARN, "Couldn't bind socket!");
		fPassed = FALSE;
		goto Exit;
	}

	LingerOption.l_linger = 0;
	LingerOption.l_onoff = TRUE;

	// Set socket for hard-close so the other-side will terminate on a close
	if(setsockopt(ListenerSocket, SOL_SOCKET, SO_LINGER, (const char *) &LingerOption, sizeof(LINGER)) == SOCKET_ERROR)
	{
		xLog(g_hLog, XLL_WARN, "Couldn't set socket for hard closes: %u", WSAGetLastError());
		fPassed = FALSE;
		goto Exit;
	}

	// Listen to the socket
	if(listen(ListenerSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		xLog(g_hLog, XLL_WARN, "Couldn't listen to socket!");
		fPassed = FALSE;
		goto Exit;
	}

	xLog(g_hLog, XLL_INFO, "Listening to socket bound to port 0x%04x", LocalPort);

	// Wait for the socket to receive an incoming connection (i.e. select will say it is readable)
	if(!IsSocketReady(ListenerSocket, READABLE, 10))
	{
		xLog(g_hLog, XLL_INFO, "Winsock reported no pending connections");
		fPassed = FALSE;
		goto Exit;
	}

	if((AcceptedConnection = accept(ListenerSocket, (struct sockaddr *) &RemoteAddress, &nRemoteAddressSize)) == INVALID_SOCKET)
	{
		xLog(g_hLog, XLL_INFO, "Couldn't accept incoming connection");
		fPassed = FALSE;
		goto Exit;
	}

	// Is the reported port correct?
	if(ntohs(RemoteAddress.sin_port) != ExpectedRemotePort)
	{
		xLog(g_hLog, XLL_INFO, "Reported destination port (%u) didn't match expected value (%u)", ntohs(RemoteAddress.sin_port), ExpectedRemotePort);
		fPassed = FALSE;
		goto Exit;
	}

	xLog(g_hLog, XLL_INFO, "Connection accepted successfully");

Exit:

	(AcceptedConnection != INVALID_SOCKET) ? closesocket(AcceptedConnection) : 0;
	(ListenerSocket != INVALID_SOCKET) ? closesocket(ListenerSocket) : 0;

	return fPassed;
}

//==================================================================================
// TCPSeqNumMaxTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can accept an incoming connection with max seq num
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPSeqNumMaxTest(WORD LowPort, WORD HighPort)
{
	return AcceptIncomingConnection(LowPort, LowPort);
}

//==================================================================================
// TCPSeqNumMinTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can accept an incoming connection with min seq num
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPSeqNumMinTest(WORD LowPort, WORD HighPort)
{
	return AcceptIncomingConnection(LowPort, LowPort);
}

//==================================================================================
// TCPDataOffBelowMinTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can accept an incoming connection with data offset
// below minimum valid value
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPDataOffBelowMinTest(WORD LowPort, WORD HighPort)
{
	return !AcceptIncomingConnection(LowPort, LowPort);
}

//==================================================================================
// TCPDataOffAboveAvailTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can accept an incoming connection with data offset
// larger than the number of bytes in the header
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPDataOffAboveAvailTest(WORD LowPort, WORD HighPort)
{
	return !AcceptIncomingConnection(LowPort, LowPort);
}
