//
//	netdata
//
//	Description:	
//		Send data to and received echoed data from xbnettest and check for errors.
//
//	recvfrom(): sync with xbnettest
//	sendto(): send datagram to xbnettest
//	recvfrom(): receive echoed datagram
//	compare send and receive data


#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>

#define BUFSIZE 1400

VOID
NetDataStartTest(HANDLE	LogHandle) {
	WSADATA			wsaData;
	SOCKET			sock;
	sockaddr_in		saLocalAddr, saFromAddr, saXbNetTestAddr;
	u_short			sXboxPort;
	int				iRet, iSize, iNumMsg, i;
	int				iBytesRecv, iBytesSent, iMiscompare;
	char			cSendBuf[BUFSIZE], cRecvBuf[BUFSIZE];
	char			cAddress[15];
	BOOL			bDone;

    xSetOwnerAlias(LogHandle, "a-emebac");
	xSetComponent(LogHandle, "Hardware", "NetData");
    xSetFunctionName(LogHandle, "NetData");

	iBytesRecv = 0;
	iBytesSent = 0;
	iMiscompare = 0;
	bDone = FALSE;

	// random data for send buffer
	for(i = 0; i < BUFSIZE; i++) {
		cSendBuf[i] = (char)rand()%256;
	}

	// get number of messages from testini.ini
	iNumMsg = GetProfileInt(TEXT("netdata"), TEXT("NumMsg"), 1);

	// Initialize Winsock layer
	iRet = WSAStartup(0x202, &wsaData);
	if(iRet != 0) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"NetData: WSAStartup FAILED: Return Value = %d", iRet);
		return;
	}

	// Create UDP socket
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock == INVALID_SOCKET) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"NetData: socket() FAILED: WSALastError = %d", WSAGetLastError());
		return;
	}

	// Bind socket to local address
	sXboxPort = IPPORT_RESERVED + 1;
	saLocalAddr.sin_family = AF_INET;
	saLocalAddr.sin_addr.s_addr = INADDR_ANY;
	saLocalAddr.sin_port = htons(sXboxPort);

	iRet = bind(sock, (sockaddr *)&saLocalAddr, sizeof(saLocalAddr));
	if(iRet == SOCKET_ERROR) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"NetData: bind() FAILED: WSALastError = %d", WSAGetLastError());
		return;
	}

	// Sync with xbnettest
	iSize = sizeof(saXbNetTestAddr);
	iRet = recvfrom(sock, cSendBuf, BUFSIZE, 0,(sockaddr *)&saXbNetTestAddr, &iSize);
	if((iRet == SOCKET_ERROR)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "NetData: Sync recvfrom() FAILED: WSALastError = %d", WSAGetLastError());
		return;
	}

	// Output xbnettest info
	XNetInAddrToString(saXbNetTestAddr.sin_addr, cAddress, sizeof(cAddress));
	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"NetData: Connected to: %s Exchanging %d %d byte messages", cAddress, iNumMsg, BUFSIZE);

	i = 0;
	do {
		// Increment iteration counter
		i++;

		// Send test data 
		iRet = sendto(sock, cSendBuf, BUFSIZE, 0,(sockaddr *)&saXbNetTestAddr, sizeof(saXbNetTestAddr));
		if((iRet == SOCKET_ERROR)) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"NetData: Rep %d: sendto() FAILED: WSALastError = %d", i, WSAGetLastError());
			break;
		}

		if(iRet != BUFSIZE) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"NetData: Rep %d: sendto() FAILED: sent %d bytes of %d", i, iRet, BUFSIZE);
		}

		iBytesSent += iRet;

		// Receive echoed test data
		iSize = sizeof(saFromAddr);
		iRet = recvfrom(sock, cRecvBuf, BUFSIZE, 0,(sockaddr *)&saFromAddr, &iSize);
		if((iRet == SOCKET_ERROR)) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"NetData: Rep %d: recvfrom() FAILED: WSALastError = %d", i, WSAGetLastError());
			break;
		}

		iBytesRecv += iRet;

		// compare send and receive
		for(int j = 0; j < BUFSIZE; j++) {
			if(cSendBuf[j] != cRecvBuf[j]) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
					"NetData: Rep %d: FAILED Data miscompare: cSendBuf[%d] = %c  cRecvBuf[%d] = %c", i, j, cSendBuf[j], j, cRecvBuf[j]);
				iMiscompare++;
			}
		}

		// Check for done
		if(iNumMsg != -1) {
			if(i >= iNumMsg) {
				bDone = TRUE;
			}
		}
	} while (!bDone);


	xLog(gbConsoleOut, LogHandle, XLL_INFO, 
		"NetData: Finished %d messages: Sent %d Bytes Received %d Bytes %d Bytes did not match", 
		i, iBytesSent, iBytesRecv, iMiscompare);

	// Send stop to xbnettest
	iRet = sendto(sock, cSendBuf, 0, 0,(sockaddr *)&saXbNetTestAddr, sizeof(saXbNetTestAddr));
	if((iRet == SOCKET_ERROR)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"NetData: sendto() FAILED: WSALastError = %d", WSAGetLastError());
	}

	if(iRet != 0) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"NetData: sendto() FAILED: sent %d bytes of %d", iRet, BUFSIZE);
	}

	// Clean up WSA and Xnet
	iRet = closesocket(sock);
	if(iRet != 0) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "closesocket FAILED");
	}

	iRet = WSACleanup(); 
	if(iRet != 0) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "WSACleanup FAILED");
	}

	iRet = XNetCleanup();
	if(iRet != NO_ERROR) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "XnetCleanup FAILED");
	}
}

VOID
NetDataEndTest() {
}

#if !defined(HARNESSLIB)
__cdecl main() {
	XNetStartupParams xnspParams;
	int iRet;
	HANDLE	LogHandle = NULL;

	// Get test configuration from ini file
	if (GetProfileInt(TEXT("GeneralSettings"), TEXT("StressTestMode"), 0) ||
		GetProfileInt(TEXT("GeneralSettings"), TEXT("ParallelTestMode"), 0)) {
		gbConsoleOut = FALSE;
	} else {
		// Start dxconio if in SerialTestMode
		xCreateConsole(NULL);
		xSetBackgroundImage(NULL);
		gbConsoleOut = TRUE;
	}

	// Do the initialization that the harness does
	// xlog
	LogHandle = xCreateLog_W(L"t:\\netdata.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("NetData: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		return 0;
    }

    xSetOwnerAlias(LogHandle, "a-emebac");
	xSetComponent(LogHandle, "Hardware", "NetData");
    xSetFunctionName(LogHandle, "NetData");

	// Initialize Xbox network stack
	memset(&xnspParams, 0, sizeof(xnspParams));
	xnspParams.cfgSizeOfStruct = sizeof(xnspParams);
	xnspParams.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
	iRet = XNetStartup(&xnspParams);
	if(iRet != NO_ERROR) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "XNetStartUp FAILED");
	} else {
		// Start Test
		NetDataStartTest(LogHandle);

		// End Test
		NetDataEndTest();
	}

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("NetData: End - Waiting for reboot...\n");
	Sleep(INFINITE);

	// Do the de-initialize that the harness does
	// dxconio
	xReleaseConsole();

	return 0;
}
#endif


//
// Export function pointers of StartTest and EndTest
//
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( netdata )
#pragma data_seg()

BEGIN_EXPORT_TABLE( netdata )
    EXPORT_TABLE_ENTRY( "StartTest", NetDataStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", NetDataEndTest )
END_EXPORT_TABLE( netdata )

