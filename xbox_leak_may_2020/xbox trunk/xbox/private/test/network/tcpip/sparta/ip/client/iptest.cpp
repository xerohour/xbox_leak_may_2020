//==================================================================================
// Includes
//==================================================================================
#include "iptest.h"

//==================================================================================
// Globals
//==================================================================================

// Define harness stuff
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( iptest )
#pragma data_seg()

// Define export table for harness
BEGIN_EXPORT_TABLE( iptest )
    EXPORT_TABLE_ENTRY( "StartTest", IpTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", IpTestEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", IpTestDllMain )
END_EXPORT_TABLE( iptest )

// Test specific globals
static CRITICAL_SECTION g_csSerializeAccess;
static HANDLE g_hLog = INVALID_HANDLE_VALUE;
static PIP_TEST_FUNCTION	g_TestFunctions[IPMSG_MAXTEST] = 
{
	IPVersionSixTest,												// IPMSG_VERSION_SIX,
	IPVersionBadTest,												// IPMSG_VERSION_BAD,
	IPHeaderLenBelowMinTest,										// IPMSG_HEADLEN_BELOWMIN,
	IPHeaderLenMaxTest,												// IPMSG_HEADLEN_MAX,
	IPHeaderLenAboveTotalLenTest,									// IPMSG_HEADLEN_ABOVETOTALLEN,
	IPHeaderLenAboveAvailTest,										// IPMSG_HEADLEN_ABOVEAVAIL,
	IPTosNormalTest,												// IPMSG_TOS_NORMAL,
	IPTosNetControlTest,											// IPMSG_TOS_NETCONTROL,
	IPTosLowDelayTest,												// IPMSG_TOS_LOWDELAY,
	IPTosHighThroughputTest,										// IPMSG_TOS_HIGHTHROUGH,
	IPTosHighReliabilityTest,										// IPMSG_TOS_HIGHRELI,
	IPTosReservedTest,												// IPMSG_TOS_RESERVED,
	IPLengthBelowMinTest,											// IPMSG_LENGTH_BELOWMIN,
	IPLengthAtMinTest,												// IPMSG_LENGTH_ATMIN,
	IPLengthAboveMinTest,											// IPMSG_LENGTH_ABOVEMIN,
	IPLengthAboveTotalTest,											// IPMSG_LENGTH_ABOVETOTAL,
	IPLengthBelowTotalTest,											// IPMSG_LENGTH_BELOWTOTAL,
	IPLengthMaxTest,												// IPMSG_LENGTH_MAXIMUM,
	IPIDMinTest,													// IPMSG_ID_MIN,
	IPIDMaxTest,													// IPMSG_ID_MAX,
	IPFlagReservedSetTest,											// IPMSG_FLAG_RESERVEDSET,
	IPFlagDontFragAndMoreFragsTest,									// IPMSG_FLAG_DFANDMF,
	IPTtlMinTest,													// IPMSG_TTL_MIN,
	IPTtlMaxTest,													// IPMSG_TTL_MAX,
	IPProtocolUnsupportedTest,										// IPMSG_PRO_UNSUP,
	IPProtocolICMPTooSmallTest,										// IPMSG_PRO_ICMP_TOOSMALL,
	IPProtocolIGMPTooSmallTest,										// IPMSG_PRO_IGMP_TOOSMALL,
	IPProtocolTCPTooSmallTest,										// IPMSG_PRO_TCP_TOOSMALL,
	IPProtocolUDPTooSmallTest,										// IPMSG_PRO_UDP_TOOSMALL,
	IPProtocolUnsupportedTooSmallTest,								// IPMSG_PRO_UNSUP_TOOSMALL,
	IPChecksumBadTest,												// IPMSG_CHECKSUM_BAD,
	IPOptionsAboveAvailTest,										// IPMSG_OPT_ABOVEAVAIL,
	IPOptionsExactSizeTest,											// IPMSG_OPT_EXACTSIZE,
	IPOptionsEndOfOpsLastTest,										// IPMSG_OPT_ENDOFOPSLAST,
	IPOptionsEndOfOpsFirstTest,										// IPMSG_OPT_ENDOFOPSFIRST,
	IPFragMaxSimulReassemblyTest,									// IPMSG_FRAG_MAXSIMREASM,
	IPFragAboveMaxSimulReassemblyTest,								// IPMSG_FRAG_ABOVEMAXSIMREASM,
	IPFragFullReverseTest,											// IPMSG_FRAG_FULLREVERSE,
	IPFragMidReverseTest,											// IPMSG_FRAG_MIDREVERSE,
	IPFragLastInSecondTest,											// IPMSG_FRAG_LASTINSECOND,
	IPFragMixedSizeTest,											// IPMSG_FRAG_MIXEDSIZE,
	IPFragOneHundredTest,											// IPMSG_FRAG_HUNDRED,
	IPFragMultipleLastFragmentTest,									// IPMSG_FRAG_MULTILASTFRAG,
	IPFragOverlappingFragmentsTest,									// IPMSG_FRAG_OVERLAPPING,
	IPFragMaxDatagramSizeTest,										// IPMSG_FRAG_MAXDGRAMSIZE,
	IPFragMaxReassemblySizeTest,									// IPMSG_FRAG_MAXREASMSIZE,
	IPFragAboveMaxReassemblySizeTest,								// IPMSG_FRAG_ABOVEMAXREASMSIZE,
	IPFragOversizedFragmentsTest,									// IPMSG_FRAG_OVERSIZED,
	IPAttackTeardropTest,											// IPMSG_ATTACK_TEARDROP,
	IPAttackNewTeardropTest,										// IPMSG_ATTACK_NEWTEAR,
	IPAttackImpTeardropTest,										// IPMSG_ATTACK_IMPTEAR,
	IPAttackSynDropTest,											// IPMSG_ATTACK_SYNDROP,
	IPAttackBonkTest,												// IPMSG_ATTACK_BONK,
	IPAttackZeroLenOptTest,											// IPMSG_ATTACK_ZEROLENOPT,
	IPAttackNesteaTest												// IPMSG_ATTACK_NESTEA,
};

static LPSTR g_TestNames[IPMSG_MAXTEST] = 
{
	"IP version six",												// IPMSG_VERSION_SIX,
	"IP unknown version",											// IPMSG_VERSION_BAD,
	"IP header length below minimum",								// IPMSG_HEADLEN_BELOWMIN,
	"IP header length maximum",										// IPMSG_HEADLEN_MAX,
	"IP header length above total length",							// IPMSG_HEADLEN_ABOVETOTALLEN,
	"IP header length above total available",						// IPMSG_HEADLEN_ABOVEAVAIL,
	"IP normal type-of-service",									// IPMSG_TOS_NORMAL,
	"IP NetControl type-of-service",								// IPMSG_TOS_NETCONTROL,
	"IP low-delay type-of-service",									// IPMSG_TOS_LOWDELAY,
	"IP high-throughput type-of-service",							// IPMSG_TOS_HIGHTHROUGH,
	"IP high-reliability type-of-service",							// IPMSG_TOS_HIGHRELI,
	"IP reserved type-of-service",									// IPMSG_TOS_HIGHRELI,
	"IP length below minimum",										// IPMSG_LENGTH_BELOWMIN,
	"IP length at minimum",											// IPMSG_LENGTH_ATMIN,
	"IP length above minimum",										// IPMSG_LENGTH_ABOVEMIN,
	"IP length above total",										// IPMSG_LENGTH_ABOVETOTAL,
	"IP length below total",										// IPMSG_LENGTH_BELOWTOTAL,
	"IP length maximum", 											// IPMSG_LENGTH_MAXIMUM,
	"IP ID minimum",												// IPMSG_ID_MIN,
	"IP ID maximum",												// IPMSG_ID_MAX,
	"IP reserved flag set",											// IPMSG_FLAG_RESERVEDSET,
	"IP don't fragment and more fragments",							// IPMSG_FLAG_DFANDMF,
	"IP time-to-live minimum",										// IPMSG_TTL_MIN,
	"IP time-to-live maximum",										// IPMSG_TTL_MAX,
	"IP protocol unsupported",										// IPMSG_PRO_UNSUP,
	"IP ICMP packet too small",										// IPMSG_PRO_ICMP_TOOSMALL,
	"IP IGMP packet too small",										// IPMSG_PRO_IGMP_TOOSMALL,
	"IP TCP packet too small",										// IPMSG_PRO_TCP_TOOSMALL,
	"IP UDP packet too small",										// IPMSG_PRO_UDP_TOOSMALL,
	"IP unsupported packet too small",								// IPMSG_PRO_UNSUP_TOOSMALL,
	"IP checksum bad",												// IPMSG_CHECKSUM_BAD,
	"IP options size above avail",									// IPMSG_OPT_ABOVEAVAIL,
	"IP options size exactly avail",								// IPMSG_OPT_EXACTSIZE,
	"IP options end-of-options last",								// IPMSG_OPT_ENDOFOPSLAST,
	"IP options end-of-options first",								// IPMSG_OPT_ENDOFOPSFIRST,
	"IP max simultaneous reassembly",								// IPMSG_FRAG_MAXSIMREASM,
	"IP above max simultaneous reassembly",							// IPMSG_FRAG_ABOVEMAXSIMREASM,
	"IP fragments completely reversed",								// IPMSG_FRAG_FULLREVERSE,
	"IP fragments middle two reversed",								// IPMSG_FRAG_MIDREVERSE,
	"IP fragments last fragment second",							// IPMSG_FRAG_LASTINSECOND,
	"IP fragments mixed size",										// IPMSG_FRAG_MIXEDSIZE,
	"IP one-hundred-fragments",										// IPMSG_FRAG_HUNDRED,
	"IP multiple last fragments",									// IPMSG_FRAG_MULTILASTFRAG,
	"IP overlapping fragments",										// IPMSG_FRAG_OVERLAPPING,
	"IP fragments max datagram size",								// IPMSG_FRAG_MAXDGRAMSIZE,
	"IP max fragment reassembly size",								// IPMSG_FRAG_MAXREASMSIZE,
	"IP above max fragment reassembly size",						// IPMSG_FRAG_ABOVEMAXREASMSIZE,
	"IP frags larger buf than hdr claims",							// IPMSG_FRAG_OVERSIZED,
	"IP teardrop attack",											// IPMSG_ATTACK_TEARDROP,
	"IP new teardrop attack",										// IPMSG_ATTACK_NEWTEAR,
	"IP improved teardrop attack",									// IPMSG_ATTACK_IMPTEAR,
	"IP syndrop attack",											// IPMSG_ATTACK_SYNDROP,
	"IP bonk attack",												// IPMSG_ATTACK_BONK,
	"IP with zero length option attack",							// IPMSG_ATTACK_ZEROLENOPT,
	"IP nestea attack"												// IPMSG_ATTACK_NESTEA,
};

//==================================================================================
// IpTestDllMain
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
BOOL WINAPI IpTestDllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
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
// IpTestStartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI IpTestStartTest(IN HANDLE  hLog)
{
	NETSYNC_TYPE_CALLBACK CallbackSessionInfo;
	IP_TESTREQ TestRequest;
	IN_ADDR ServerAddr;
    WSADATA WSAData;
	HANDLE hClientObject = NULL;
	DWORD dwTestDone;
	WORD LowPort, HighPort;
	INT i = 0, j = 0, nRet = 0;
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

	// Prepare to find a server that can run the IP tests
	CallbackSessionInfo.byMinClientCount = 0;
	CallbackSessionInfo.byMaxClientCount = 0;
	CallbackSessionInfo.nMinPortCount = 1;
	CallbackSessionInfo.lpszDllName_W = IP_SERVER_DLL_W;
	CallbackSessionInfo.lpszMainFunction_A = IP_SERVER_MAIN_FUNCTION_A;
	CallbackSessionInfo.lpszStartFunction_A = IP_SERVER_START_FUNCTION_A;
	CallbackSessionInfo.lpszStopFunction_A = IP_SERVER_STOP_FUNCTION_A;

	// Create a client to communicate with that server
	hClientObject = NetsyncCreateClient(0, NETSYNC_SESSION_CALLBACK, &CallbackSessionInfo, &ServerAddr.S_un.S_addr, NULL, &LowPort, &HighPort);
	if (hClientObject == INVALID_HANDLE_VALUE)
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't create Netsync client");
		goto Exit;
	}

	DbgPrint("IPTEST: Starting tests...\n");

	// Run the tests
	for(i = 0; i < IPMSG_MAXTEST; ++i)
	{
		IN_ADDR FromAddr;
		DWORD dwReceiveStatus, dwSendResult;
		
		TestRequest.dwMessageId = IPMSG_TESTREQ;
		TestRequest.TestId = (IPMSG_TESTTYPES) i;
		
		xStartVariation(g_hLog, g_TestNames[i]);
		xLog(g_hLog, XLL_INFO, "Started test %u", i);
		
		// Send the test request to the Netsync server
		dwSendResult = NetsyncSendClientMessage(hClientObject, 0, sizeof(IP_TESTREQ), (char *) &TestRequest);
		if(dwSendResult != WAIT_OBJECT_0)
		{
			xLog(g_hLog, XLL_INFO, "Error sending message to TCP server");
			DbgPrint("IPTEST: Error during test, aborting...\n");
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
	
	DbgPrint("IPTEST: Tests complete\n");

Exit:

	// Tell the netsync server we're done
	dwTestDone = IPMSG_TESTDONE;
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
// IpTestEndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI IpTestEndTest()
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
// ReceiveIPPacket
//----------------------------------------------------------------------------------
//
// Description: Test that receives a IP packet with with a certain set of properties
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
BOOL ReceiveIPPacket(WORD LocalPort, WORD ExpectedRemotePort, DWORD ExpectedLength, BOOL fUseLength)
{
	SOCKET IPSocket = INVALID_SOCKET;
	SOCKADDR_IN LocalAddress, RemoteAddress;
	DWORD dwBytesAvailable = 0, dwBufferSize = 0, dwBytesReceived = 0;
	BOOL fPassed = TRUE;
	CHAR *pBuffer = NULL;
	INT nRemoteAddressSize = sizeof(SOCKADDR_IN);

	// Create the socket
	IPSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(IPSocket == INVALID_SOCKET)
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
	if(bind(IPSocket, (const struct sockaddr *) &LocalAddress, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		xLog(g_hLog, XLL_WARN, "Couldn't bind socket!");
		fPassed = FALSE;
		goto Exit;
	}

	// Wait for the socket to receive a packet (i.e. select will say it is readable)
	if(!IsSocketReady(IPSocket, READABLE, 10))
	{
		xLog(g_hLog, XLL_INFO, "Winsock reported no data available");
		fPassed = FALSE;
		goto Exit;
	}

	// A datagram is buffered, now receive it...
	if(ioctlsocket(IPSocket, FIONREAD, &dwBytesAvailable) == SOCKET_ERROR)
	{
		xLog(g_hLog, XLL_WARN, "Query for number of bytes available failed");
		fPassed = FALSE;
		goto Exit;
	}

	xLog(g_hLog, XLL_INFO, "%u bytes available on socket", dwBytesAvailable);

	dwBufferSize = dwBytesAvailable ? dwBytesAvailable : 1;
	pBuffer = (CHAR *) LocalAlloc(LPTR, dwBufferSize);

	// Receive the datagram
	if((dwBytesReceived = recvfrom(IPSocket, pBuffer, dwBufferSize, 0, (struct sockaddr *) &RemoteAddress, &nRemoteAddressSize)) == SOCKET_ERROR)
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
		xLog(g_hLog, XLL_WARN, "Reported length (%u) didn't match expected value (%u)", dwBytesReceived, ExpectedLength);
		fPassed = FALSE;
		goto Exit;
	}

	xLog(g_hLog, XLL_INFO, "Packet received successfully");

Exit:

	(IPSocket != INVALID_SOCKET) ? closesocket(IPSocket) : 0;
	pBuffer ? LocalFree(pBuffer) : 0;

	return fPassed;
}

//==================================================================================
// IPVersionSixTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with version 6
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPVersionSixTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPVersionBadTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with version other than 4 or 6
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPVersionBadTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPHeaderLenBelowMinTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a header indicating a size
// below the minimum
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPHeaderLenBelowMinTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPHeaderLenMaxTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with maximum header size
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPHeaderLenMaxTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPHeaderLenAboveTotalLenTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a header length greater
// than the total datagram length
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPHeaderLenAboveTotalLenTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPHeaderLenAboveAvailTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a header length greater
// than the available bytes in the datagram
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPHeaderLenAboveAvailTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPTosNormalTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a normal type-of-service
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosNormalTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPTosNetControlTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a net-control type-of-service
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosNetControlTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPTosLowDelayTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a low-delay type-of-service
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosLowDelayTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPTosHighThroughputTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a high-throughput type-of-service
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosHighThroughputTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPTosHighReliabilityTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a high-reliability type-of-service
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosHighReliabilityTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPTosReservedTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a reserved type-of-service
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTosReservedTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPLengthBelowMinTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a total length field
// below the minimum datagram size
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthBelowMinTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPLengthAtMinTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a total length field
// of exactly the minimum datagram size
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthAtMinTest(WORD LowPort, WORD HighPort)
{
	// We shouldn't be able to receive this, we're really just looking for an
	// off-by-one exception in the stack
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPLengthAboveMinTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a total length field
// above the minimum datagram size
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthAboveMinTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPLengthAboveTotalTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a total length field
// above the available bytes in the datagram
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthAboveTotalTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPLengthBelowTotalTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a total length field
// below the available bytes in the datagram
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthBelowTotalTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPLengthMaxTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a total length field
// is the maximum allowable in a single non-fragmented datagram over Ethernet
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPLengthMaxTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1472, TRUE);
}

//==================================================================================
// IPIDMinTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with the minimum possible ID
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPIDMinTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPIDMaxTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with the maximum possible ID
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPIDMaxTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPFlagReservedSetTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with the reserved flag set
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFlagReservedSetTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPFlagDontFragAndMoreFragsTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with both the don't-frag
// and more-frags flags set
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFlagDontFragAndMoreFragsTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPTtlMinTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with the minimum time-to-live
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTtlMinTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPTtlMaxTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with the maximum time-to-live
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPTtlMaxTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPProtocolUnsupportedTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can properly handle a packet from an unsupported protocol
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolUnsupportedTest(WORD LowPort, WORD HighPort)
{
	// We're really just testing to see if we crash when we get a packet like this
	// No point trying to actually receive it
	Sleep(5000);
	return TRUE;
}

//==================================================================================
// IPProtocolICMPTooSmallTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can properly handle an IP packet saying it is an ICMP
// packet but with no payload
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolICMPTooSmallTest(WORD LowPort, WORD HighPort)
{
	// We're really just testing to see if we crash when we get a packet like this
	// No point trying to actually receive it
	Sleep(5000);
	return TRUE;
}

//==================================================================================
// IPProtocolIGMPTooSmallTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can properly handle an IP packet saying it is an IGMP
// packet but with no payload
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolIGMPTooSmallTest(WORD LowPort, WORD HighPort)
{
	// We're really just testing to see if we crash when we get a packet like this
	// No point trying to actually receive it
	Sleep(5000);
	return TRUE;
}

//==================================================================================
// IPProtocolTCPTooSmallTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can properly handle an IP packet saying it is an TCP
// packet but with no payload
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolTCPTooSmallTest(WORD LowPort, WORD HighPort)
{
	// We're really just testing to see if we crash when we get a packet like this
	// No point trying to actually receive it
	Sleep(5000);
	return TRUE;
}

//==================================================================================
// IPProtocolUDPTooSmallTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can properly handle an IP packet saying it is an UDP
// packet but with no payload
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolUDPTooSmallTest(WORD LowPort, WORD HighPort)
{
	// We're really just testing to see if we crash when we get a packet like this
	// No point trying to actually receive it
	Sleep(5000);
	return TRUE;
}

//==================================================================================
// IPProtocolUnsupportedTooSmallTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can properly handle an IP packet saying it is a
// packet of an unsupported protocol but with no payload
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPProtocolUnsupportedTooSmallTest(WORD LowPort, WORD HighPort)
{
	// We're really just testing to see if we crash when we get a packet like this
	// No point trying to actually receive it
	Sleep(5000);
	return TRUE;
}

//==================================================================================
// IPChecksumBadTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive an IP packet with a bad checksum
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPChecksumBadTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPOptionsAboveAvailTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can properly handle an IP datagram where an option
// in the header is cutoff by the end of the packet
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPOptionsAboveAvailTest(WORD LowPort, WORD HighPort)
{
	// We're really just testing to see if we crash when we get a packet like this
	// No point trying to actually receive it
	Sleep(5000);
	return TRUE;
}

//==================================================================================
// IPOptionsExactSizeTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a datagram where the header contains a
// variable length option that ends exactly at the end of the header.
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPOptionsExactSizeTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPOptionsEndOfOpsLastTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a datagram where the header contains a
// variable length option and ends with an end-of-options option
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPOptionsEndOfOpsLastTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1, TRUE);
}

//==================================================================================
// IPOptionsEndOfOpsFirstTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a datagram where the header contains an
// end-of-options option followed by a variable-length option
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPOptionsEndOfOpsFirstTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPFragFullReverseTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a 4 fragmented datagram where the fragments
// arrive in full reverse order
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragFullReverseTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 56, TRUE);
}

//==================================================================================
// IPFragMidReverseTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a 4 fragmented datagram where the fragments
// arrive such that the middle two fragments are in reverse order
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMidReverseTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 56, TRUE);
}

//==================================================================================
// IPFragLastInSecondTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a 4 fragmented datagram where the fragments
// arrive such that the last fragment arrives second and the remaining fragments are in order
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragLastInSecondTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 56, TRUE);
}

//==================================================================================
// IPFragMixedSizeTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a 4 fragmented datagram where each of the
// fragments are of a different size
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMixedSizeTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 64, TRUE);
}

//==================================================================================
// IPFragOneHundredTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a 100 fragmented datagram
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragOneHundredTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 1592, TRUE);
}

//==================================================================================
// IPFragMultipleLastFragmentTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a datagram that has multiple fragments
// with different offsets that don't have the more-frags flag set
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMultipleLastFragmentTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 40, FALSE);
}

//==================================================================================
// IPFragOverlappingFragmentsTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a datagram that has where received
// fragments have overlapping payload data
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragOverlappingFragmentsTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 32, TRUE);
}

//==================================================================================
// IPFragMaxDatagramSizeTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a fragmented datagram with the maximum possible
// datagram size of 64K - 1
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMaxDatagramSizeTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 64 * 1024 - 1, FALSE);
}

//==================================================================================
// IPFragMaxReassemblySizeTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a fragmented datagram with the default
// maximum allowable reassembly size 2K
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMaxReassemblySizeTest(WORD LowPort, WORD HighPort)
{
	return ReceiveIPPacket(LowPort, LowPort, 2 * 1024 - UDPHEADER_SIZE, TRUE);
}

//==================================================================================
// IPFragAboveMaxReassemblySizeTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a fragmented datagram with a size of just
// above the default maximum allowable reassembly size (2K + 1)
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragAboveMaxReassemblySizeTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 2 * 1024 + 1, FALSE);
}

//==================================================================================
// IPFragMaxSimulReassemblyTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive and successfully reassemble the maximum
// number of simultaneous fragmented IP datagrams
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragMaxSimulReassemblyTest(WORD LowPort, WORD HighPort)
{
	SOCKET IPSockets[SIM_REASM_MAX];
	SOCKADDR_IN LocalAddress, RemoteAddress;
	DWORD dwBytesAvailable = 0, dwBufferSize = 0, dwBytesReceived = 0;
	BOOL fPassed = TRUE;
	CHAR *pBuffer = NULL;
	INT SocketIndex, SocketsReady, nRemoteAddressSize = sizeof(SOCKADDR_IN);
	fd_set SocketCollection;
	timeval TimeOut = {0, 0}; // set to wait 5 seconds

	FD_ZERO(&SocketCollection);

	// Create and bind all the sockets
	for(SocketIndex = 0; SocketIndex < SIM_REASM_MAX; ++SocketIndex)
	{
		IPSockets[SocketIndex] = INVALID_SOCKET;

		// Create the socket
		IPSockets[SocketIndex] = socket(AF_INET, SOCK_DGRAM, 0);
		if(IPSockets[SocketIndex] == INVALID_SOCKET)
		{
			xLog(g_hLog, XLL_WARN, "Couldn't create socket!");
			fPassed = FALSE;
			goto Exit;
		}
		
		// Setup the local binding info
		LocalAddress.sin_addr.s_addr = INADDR_ANY;
		LocalAddress.sin_family = AF_INET;
		LocalAddress.sin_port = htons((LowPort + SocketIndex + MAX_SIMUL_PORT_OFFSET) % 0xffff + 1);

		// Bind the socket
		if(bind(IPSockets[SocketIndex], (const struct sockaddr *) &LocalAddress, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			xLog(g_hLog, XLL_WARN, "Couldn't bind socket!");
			fPassed = FALSE;
			goto Exit;
		}

		// Add each socket to the fds structure
		FD_SET(IPSockets[SocketIndex], &SocketCollection);

		xLog(g_hLog, XLL_INFO, "Added socket %u to set for readability monitoring", SocketIndex);	
	}

	Sleep(5000);

	SocketsReady = 0;
	if((SocketsReady = select(0, &SocketCollection, NULL, NULL, &TimeOut)) != SIM_REASM_MAX)
	{
			xLog(g_hLog, XLL_WARN, "Only %u sockets were readable, expecting %u", SocketsReady, SIM_REASM_MAX);
			fPassed = FALSE;
			goto Exit;
	}

	// Check the available data and recv it from each of the sockets
	for(SocketIndex = 0; SocketIndex < SIM_REASM_MAX; ++SocketIndex)
	{
		// A datagram is buffered, now receive it...
		if(ioctlsocket(IPSockets[SocketIndex], FIONREAD, &dwBytesAvailable) == SOCKET_ERROR)
		{
			xLog(g_hLog, XLL_WARN, "Query for number of bytes available failed on socket %u", SocketIndex);
			fPassed = FALSE;
			goto Exit;
		}
		
		dwBufferSize = dwBytesAvailable ? dwBytesAvailable : 1;
		pBuffer = (CHAR *) LocalAlloc(LPTR, dwBufferSize);
		
		// Receive the datagram
		if((dwBytesReceived = recvfrom(IPSockets[SocketIndex], pBuffer, dwBufferSize, 0, (struct sockaddr *) &RemoteAddress, &nRemoteAddressSize)) == SOCKET_ERROR)
		{
			xLog(g_hLog, XLL_WARN, "Couldn't receive the datagram on socket %u", SocketIndex);
			fPassed = FALSE;
			goto Exit;
		}
		
		// Is the reported port correct?
		if(ntohs(RemoteAddress.sin_port) != LowPort)
		{
			xLog(g_hLog, XLL_WARN, "Reported destination port (%u) didn't match expected value (%u) on socket %u", ntohs(RemoteAddress.sin_port), LowPort, SocketIndex);
			fPassed = FALSE;
			goto Exit;
		}
		
		// Is the reported length correct?
		if(dwBytesReceived != 24)
		{
			xLog(g_hLog, XLL_WARN, "Reported length (%u) didn't match expected value (%u) on socket %u", dwBytesReceived, 24, SocketIndex);
			fPassed = FALSE;
			goto Exit;
		}
		
		xLog(g_hLog, XLL_INFO, "Packet on socket %u received successfully", SocketIndex);	

		// Free the buffer if necessary
		if(pBuffer)
		{
			LocalFree(pBuffer);
			pBuffer = NULL;
		}
	}

Exit:

	// We have to sleep long enough to guarantee that all fragments have timed out
	Sleep(60000);

		// Check the available data and recv it from each of the sockets
	for(SocketIndex = 0; SocketIndex < SIM_REASM_MAX; ++SocketIndex)
		(IPSockets[SocketIndex] != INVALID_SOCKET) ? closesocket(IPSockets[SocketIndex]) : 0;

	pBuffer ? LocalFree(pBuffer) : 0;

	return fPassed;
}

//==================================================================================
// IPFragAboveMaxSimulReassemblyTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive and successfully reassemble the first
// few reassembleable packets when we receive more than the maximum number of simultaneous
// fragmented IP datagrams
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragAboveMaxSimulReassemblyTest(WORD LowPort, WORD HighPort)
{
	SOCKET IPSockets[SIM_REASM_MAX + 1];
	SOCKADDR_IN LocalAddress, RemoteAddress;
	DWORD dwBytesAvailable = 0, dwBufferSize = 0, dwBytesReceived = 0;
	BOOL fPassed = TRUE;
	CHAR *pBuffer = NULL;
	INT SocketIndex, SocketsReady, nRemoteAddressSize = sizeof(SOCKADDR_IN);
	fd_set SocketCollection;
	timeval TimeOut = {0, 0}; // set to wait 5 seconds

	FD_ZERO(&SocketCollection);

	// Create and bind all the sockets
	for(SocketIndex = 0; SocketIndex < SIM_REASM_MAX + 1; ++SocketIndex)
	{
		IPSockets[SocketIndex] = INVALID_SOCKET;

		// Create the socket
		IPSockets[SocketIndex] = socket(AF_INET, SOCK_DGRAM, 0);
		if(IPSockets[SocketIndex] == INVALID_SOCKET)
		{
			xLog(g_hLog, XLL_WARN, "Couldn't create socket!");
			fPassed = FALSE;
			goto Exit;
		}
		
		// Setup the local binding info
		LocalAddress.sin_addr.s_addr = INADDR_ANY;
		LocalAddress.sin_family = AF_INET;
		LocalAddress.sin_port = htons((LowPort + SocketIndex + ABOVE_MAX_SIMUL_PORT_OFFSET) % 0xffff + 1);

		// Bind the socket
		if(bind(IPSockets[SocketIndex], (const struct sockaddr *) &LocalAddress, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			xLog(g_hLog, XLL_WARN, "Couldn't bind socket!");
			fPassed = FALSE;
			goto Exit;
		}

		// Add each socket to the fds structure
		FD_SET(IPSockets[SocketIndex], &SocketCollection);

		xLog(g_hLog, XLL_INFO, "Added socket %u to set for readability monitoring", SocketIndex);	
	}

	Sleep(5000);

	SocketsReady = 0;
	if((SocketsReady = select(0, &SocketCollection, NULL, NULL, &TimeOut)) != SIM_REASM_MAX)
	{
			xLog(g_hLog, XLL_WARN, "%u sockets were readable, expecting %u", SocketsReady, SIM_REASM_MAX);
			fPassed = FALSE;
			goto Exit;
	}

	// Check the available data and recv it from each of the sockets
	for(SocketIndex = 0; SocketIndex < SIM_REASM_MAX + 1; ++SocketIndex)
	{

		if(!FD_ISSET(IPSockets[SocketIndex], &SocketCollection))
		{
			xLog(g_hLog, XLL_INFO, "No data received on socket %u", SocketIndex);	
			continue;
		}

		xLog(g_hLog, XLL_INFO, "Querying socket %u for number of bytes available", SocketIndex);	

		// A datagram is buffered, now receive it...
		if(ioctlsocket(IPSockets[SocketIndex], FIONREAD, &dwBytesAvailable) == SOCKET_ERROR)
		{
			xLog(g_hLog, XLL_WARN, "Query for number of bytes available failed on socket %u", SocketIndex);
			fPassed = FALSE;
			goto Exit;
		}
		
		xLog(g_hLog, XLL_INFO, "Socket %u reported %u bytes available", SocketIndex, dwBytesAvailable);

		dwBufferSize = dwBytesAvailable ? dwBytesAvailable : 1;
		pBuffer = (CHAR *) LocalAlloc(LPTR, dwBufferSize);
		
		xLog(g_hLog, XLL_INFO, "Calling recvfrom on socket %u", SocketIndex);

		// Receive the datagram
		if((dwBytesReceived = recvfrom(IPSockets[SocketIndex], pBuffer, dwBufferSize, 0, (struct sockaddr *) &RemoteAddress, &nRemoteAddressSize)) == SOCKET_ERROR)
		{
			xLog(g_hLog, XLL_WARN, "Couldn't receive the datagram on socket %u", SocketIndex);
			fPassed = FALSE;
			goto Exit;
		}
		
		// Is the reported port correct?
		if(ntohs(RemoteAddress.sin_port) != LowPort)
		{
			xLog(g_hLog, XLL_WARN, "Reported destination port (%u) didn't match expected value (%u) on socket %u", ntohs(RemoteAddress.sin_port), LowPort, SocketIndex);
			fPassed = FALSE;
			goto Exit;
		}
		
		// Is the reported length correct?
		if(dwBytesReceived != 24)
		{
			xLog(g_hLog, XLL_WARN, "Reported length (%u) didn't match expected value (%u) on socket %u", dwBytesReceived, 24, SocketIndex);
			fPassed = FALSE;
			goto Exit;
		}
		
		xLog(g_hLog, XLL_INFO, "Packet on socket %u received successfully", SocketIndex);	

		// Free the buffer if necessary
		if(pBuffer)
		{
			LocalFree(pBuffer);
			pBuffer = NULL;
		}
	}

Exit:

	// We have to sleep long enough to guarantee that all fragments have timed out
	Sleep(60000);

		// Check the available data and recv it from each of the sockets
	for(SocketIndex = 0; SocketIndex < SIM_REASM_MAX + 1; ++SocketIndex)
		(IPSockets[SocketIndex] != INVALID_SOCKET) ? closesocket(IPSockets[SocketIndex]) : 0;

	pBuffer ? LocalFree(pBuffer) : 0;

	return fPassed;
}


//==================================================================================
// IPFragOversizedFragmentsTest
//----------------------------------------------------------------------------------
//
// Description: Test where server sends oversized fragment to try and empty the Xbox buffer
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPFragOversizedFragmentsTest(WORD LowPort, WORD HighPort)
{
	xLog(g_hLog, XLL_INFO, "Always report success.  Look at debugger output to see if we ran out of system memory.");
	Sleep(5000);
	return TRUE;
}

//==================================================================================
// IPAttackTeardropTest
//----------------------------------------------------------------------------------
//
// Description: Test where server sends the original teardrop attack
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackTeardropTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 256, FALSE);
}

//==================================================================================
// IPAttackNewTeardropTest
//----------------------------------------------------------------------------------
//
// Description: Test where server sends the original newtear attack
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackNewTeardropTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 256, FALSE);
}

//==================================================================================
// IPAttackImpTeardropTest
//----------------------------------------------------------------------------------
//
// Description: Test where server sends an improved version of teardrop attack
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackImpTeardropTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 256, FALSE);
}

//==================================================================================
// IPAttackSynDropTest
//----------------------------------------------------------------------------------
//
// Description: Test where server sends an improved version of teardrop attack
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackSynDropTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 256, FALSE);
}

//==================================================================================
// IPAttackBonkTest
//----------------------------------------------------------------------------------
//
// Description: Test where server sends an the original bonk attack
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackBonkTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 256, FALSE);
}


//==================================================================================
// IPAttackZeroLenOptTest
//----------------------------------------------------------------------------------
//
// Description: Test whether we can receive a datagram where the header contains a
// zero length option
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackZeroLenOptTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 1, FALSE);
}

//==================================================================================
// IPAttackNesteaTest
//----------------------------------------------------------------------------------
//
// Description: Test where server sends an the nestea attack
//
// Arguments:
//	WORD			LowPort				Lowest port to bind to
//	WORD			HighPort			Highest port to bind to
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL IPAttackNesteaTest(WORD LowPort, WORD HighPort)
{
	return !ReceiveIPPacket(LowPort, LowPort, 256, FALSE);
}
