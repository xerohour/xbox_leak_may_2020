//==================================================================================
// Includes
//==================================================================================
#include "udptest.h"

//==================================================================================
// Globals
//==================================================================================
// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( udptest )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( udptest )
    EXPORT_TABLE_ENTRY( "StartTest", UdpTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", UdpTestEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", UdpTestDllMain )
END_EXPORT_TABLE( udptest )

// Test specific globals
static CRITICAL_SECTION g_csSerializeAccess;
static HANDLE g_hLog = INVALID_HANDLE_VALUE;
static PUDP_TEST_FUNCTION g_TestFunctions[UDPMSG_MAXTEST] = 
{
	UDPSourceMaxTest,												// UDPMSG_SOURCEPORT_MAX
	UDPSourceMinTest,												// UDPMSG_SOURCEPORT_MIN
	UDPSourceZeroTest,												// UDPMSG_SOURCEPORT_ZERO
	UDPDestMaxTest,													// UDPMSG_DESTPORT_MAX
	UDPDestMinTest,													// UDPMSG_DESTPORT_MIN
	UDPDestZeroTest,												// UDPMSG_DESTPORT_ZERO
	UDPLenBelowMinTest,												// UDPMSG_LENGTH_BELOWMIN
	UDPLenAtMinTest,												// UDPMSG_LENGTH_ATMIN
	UDPLenAboveMinTest,												// UDPMSG_LENGTH_ABOVEMIN
	UDPLenAboveTotalTest,											// UDPMSG_LENGTH_ABOVETOTAL
	UDPLenBelowTotalTest,											// UDPMSG_LENGTH_BELOWTOTAL
	UDPLenMaxTest,													// UDPMSG_LENGTH_MAXIMUM
	UDPCheckZerosTest												// UDPMSG_CHECKSUM_ZEROSOK
};

static LPSTR g_TestNames[UDPMSG_MAXTEST] = 
{
	"Packet from maximum port",							// UDPMSG_SOURCEPORT_MAX
	"Packet from minimum port",							// UDPMSG_SOURCEPORT_MIN
	"Packet from port zero",							// UDPMSG_SOURCEPORT_ZERO
	"Packet on maximum port",							// UDPMSG_DESTPORT_MAX
	"Packet on minimum port",							// UDPMSG_DESTPORT_MIN
	"Packet on port zero",								// UDPMSG_DESTPORT_ZERO
	"Packet with length below minimum",					// UDPMSG_LENGTH_BELOWMIN
	"Packet with minimum length",						// UDPMSG_LENGTH_ATMIN
	"Packet with length above minimum",					// UDPMSG_LENGTH_ABOVEMIN
	"Packet with oversized length",						// UDPMSG_LENGTH_ABOVETOTAL
	"Packet with undersized length",					// UDPMSG_LENGTH_BELOWTOTAL
	"Packet with maximum length",						// UDPMSG_LENGTH_MAXIMUM
	"Packet with checksum of all zeros"					// UDPMSG_CHECKSUM_ZEROSOK
};


//==================================================================================
// UdpTestDllMain
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
BOOL WINAPI UdpTestDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
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
// UdpTestStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI UdpTestStartTest(IN HANDLE  hLog)
{
	UDP_TESTREQ TestRequest;
	INT iConn;
	HANDLE hThread;
	DWORD dwThreadId, dwTestDone;
    WSADATA WSAData;
	INT nRet = 0;
	NETSYNC_TYPE_CALLBACK CallbackSessionInfo;
	HANDLE hClientObject = NULL;
	IN_ADDR ServerAddr;
	WORD LowPort, HighPort;
	INT i;
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

	// Prepare to find a server that can run the UDP tests
	CallbackSessionInfo.byMinClientCount = 0;
	CallbackSessionInfo.byMaxClientCount = 0;
	CallbackSessionInfo.nMinPortCount = 1;
	CallbackSessionInfo.lpszDllName_W = UDP_SERVER_DLL_W;
	CallbackSessionInfo.lpszMainFunction_A = UDP_SERVER_MAIN_FUNCTION_A;
	CallbackSessionInfo.lpszStartFunction_A = UDP_SERVER_START_FUNCTION_A;
	CallbackSessionInfo.lpszStopFunction_A = UDP_SERVER_STOP_FUNCTION_A;

	// Create a client to communicate with that server
	hClientObject = NetsyncCreateClient(0, NETSYNC_SESSION_CALLBACK, &CallbackSessionInfo, &ServerAddr.S_un.S_addr, NULL, &LowPort, &HighPort);
	if (hClientObject == INVALID_HANDLE_VALUE)
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't create Netsync client");
		goto Exit;
	}

	DbgPrint("UDPTEST: Starting tests...\n");

	// Run the tests
	for(i = 0; i < UDPMSG_MAXTEST; i++)
	{
		DWORD dwSendResult;

		TestRequest.dwMessageId = UDPMSG_TESTREQ;
		TestRequest.TestId = (UDPMSG_TESTTYPES) i;

		xStartVariation(g_hLog, g_TestNames[i]);

		// Send the test request to the Netsync server
		dwSendResult = NetsyncSendClientMessage(hClientObject, 0, sizeof(UDP_TESTREQ), (char *) &TestRequest);
		if(dwSendResult != WAIT_OBJECT_0)
		{
			xLog(g_hLog, XLL_INFO, "Error sending message to UDP server");
			DbgPrint("UDPTEST: Error during test, aborting...\n");
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

	DbgPrint("UDPTEST: Tests complete\n");

Exit:

	// Tell the netsync server we're done
	dwTestDone = UDPMSG_TESTDONE;
	NetsyncSendClientMessage(hClientObject, 0, sizeof(DWORD), (char *) &dwTestDone);

	hClientObject ? NetsyncCloseClient(hClientObject) : 0;

	// Clean up winsock
	xLog(g_hLog, XLL_INFO, "Cleaning up Winsock layer");
	WSACleanup();

	g_hLog = INVALID_HANDLE_VALUE;

	// Allow other threads to enter now
	LeaveCriticalSection(&g_csSerializeAccess);

}            

//==================================================================================
// UdpTestEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI UdpTestEndTest()
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
// ReceiveUDPPacket
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet with with a certain set of properties
//
// Arguments:
//	WORD			LocalPort			Port to receive the packet on
//	WORD			ExpectedRemotePort	Remote port that should be reported for packet
//	DWORD			ExpectedLength		Length that should be reported for packet
//	BOOL			fUseLength			Whether to care if the length matches or not
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL ReceiveUDPPacket(WORD LocalPort, WORD ExpectedRemotePort, DWORD ExpectedLength, BOOL fUseLength)
{
	SOCKET UDPSocket = INVALID_SOCKET;
	SOCKADDR_IN LocalAddress, RemoteAddress;
	DWORD dwBytesAvailable = 0, dwBufferSize = 0, dwBytesReceived = 0;
	BOOL fPassed = TRUE;
	CHAR *pBuffer = NULL;
	INT nRemoteAddressSize = sizeof(SOCKADDR_IN);

	// Create the socket
	UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(UDPSocket == INVALID_SOCKET)
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
	if(bind(UDPSocket, (const struct sockaddr *) &LocalAddress, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		xLog(g_hLog, XLL_WARN, "Couldn't bind socket!");
		fPassed = FALSE;
		goto Exit;
	}

	// Wait for the socket to receive a packet (i.e. select will say it is readable)
	if(!IsSocketReady(UDPSocket, READABLE, 10))
	{
		xLog(g_hLog, XLL_INFO, "Winsock reported no data available");
		fPassed = FALSE;
		goto Exit;
	}

	// A datagram is buffered, now receive it...
	if(ioctlsocket(UDPSocket, FIONREAD, &dwBytesAvailable) == SOCKET_ERROR)
	{
		xLog(g_hLog, XLL_WARN, "Query for number of bytes available failed");
		fPassed = FALSE;
		goto Exit;
	}

	dwBufferSize = dwBytesAvailable ? dwBytesAvailable : 1;
	pBuffer = (CHAR *) LocalAlloc(LPTR, dwBufferSize);

	// Receive the datagram
	if((dwBytesReceived = recvfrom(UDPSocket, pBuffer, dwBufferSize, 0, (struct sockaddr *) &RemoteAddress, &nRemoteAddressSize)) == SOCKET_ERROR)
	{
		xLog(g_hLog, XLL_INFO, "Couldn't receive the datagram");
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

	// Is the reported length correct?
	if(fUseLength && (dwBytesReceived != ExpectedLength))
	{
		xLog(g_hLog, XLL_INFO, "Reported length (%u) didn't match expected value (%u)", dwBytesReceived, ExpectedLength);
		fPassed = FALSE;
		goto Exit;
	}

	xLog(g_hLog, XLL_INFO, "Packet received successfully");

Exit:

	(UDPSocket != INVALID_SOCKET) ? closesocket(UDPSocket) : 0;
	pBuffer ? LocalFree(pBuffer) : 0;

	return fPassed;
}

//==================================================================================
// UDPSourceMaxTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet from the maximum port number
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPSourceMaxTest(WORD LowPort, WORD HighPort)
{
	return ReceiveUDPPacket(LowPort, (WORD) 0xffff, 0, FALSE);
}

//==================================================================================
// UDPSourceMinTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet from the minimum port number
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPSourceMinTest(WORD LowPort, WORD HighPort)
{
	return ReceiveUDPPacket(LowPort, 1, 0, FALSE);
}

//==================================================================================
// UDPSourceZeroTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet from port number zero
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPSourceZeroTest(WORD LowPort, WORD HighPort)
{
	return ReceiveUDPPacket(LowPort, 0, 0, FALSE);
}

//==================================================================================
// UDPDestMaxTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet on the maximum port number
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPDestMaxTest(WORD LowPort, WORD HighPort)
{
	return ReceiveUDPPacket((WORD) 0xfffe, LowPort, 0, FALSE);
}

//==================================================================================
// UDPDestMinTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet on the minimum port number
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPDestMinTest(WORD LowPort, WORD HighPort)
{
	return ReceiveUDPPacket(1, LowPort, 0, FALSE);
}

//==================================================================================
// UDPDestZeroTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet on port number zero
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPDestZeroTest(WORD LowPort, WORD HighPort)
{
	// There's no way to bind to port zero.
	// We're really just seeing if the stack blows up
	Sleep(5000);
	return TRUE;
}

//==================================================================================
// UDPLenBelowMinTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet with a length below the minimum
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenBelowMinTest(WORD LowPort, WORD HighPort)
{
	// The packet we're receiving shouldn't work...
	// We're really just seeing if the stack blows up
	return !ReceiveUDPPacket(LowPort, LowPort, 0, FALSE);
}

//==================================================================================
// UDPLenAtMinTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet with a length of exactly the minimum
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenAtMinTest(WORD LowPort, WORD HighPort)
{
	return ReceiveUDPPacket(LowPort, LowPort, 0, TRUE);
}

//==================================================================================
// UDPLenAboveMinTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet with a length above the minimum
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenAboveMinTest(WORD LowPort, WORD HighPort)
{
	return ReceiveUDPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// UDPLenAboveTotalTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet with a length above total bytes in packet
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenAboveTotalTest(WORD LowPort, WORD HighPort)
{
	// The packet we're receiving shouldn't work...
	// We're really just seeing if the stack blows up
	return !ReceiveUDPPacket(LowPort, LowPort, 0, FALSE);
}

//==================================================================================
// UDPLenBelowTotalTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet with a length below the total bytes in packet
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenBelowTotalTest(WORD LowPort, WORD HighPort)
{
	// This packet should actually work, the extra byte should be thrown away and not received
	return ReceiveUDPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// UDPLenMaxTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet with maximum length
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPLenMaxTest(WORD LowPort, WORD HighPort)
{
	// This packet will probably be discarded because it's too long.
	// But at least check to make sure it doesn't break the stack.
	return ReceiveUDPPacket(LowPort, LowPort, 1472, TRUE);
}

//==================================================================================
// UDPCheckOnesTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet with a valid checksum of all ones
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPCheckOnesTest(WORD LowPort, WORD HighPort)
{
	return ReceiveUDPPacket(LowPort, LowPort, 0, FALSE);
}

//==================================================================================
// UDPCheckZerosTest
//----------------------------------------------------------------------------------
//
// Description: Test that receives a UDP packet with a checksum of all zeros (indicating not computed)
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL UDPCheckZerosTest(WORD LowPort, WORD HighPort)
{
	// Supposedly we should keep the packet if there is no checksum
	return ReceiveUDPPacket(LowPort, LowPort, 0, FALSE);
}
