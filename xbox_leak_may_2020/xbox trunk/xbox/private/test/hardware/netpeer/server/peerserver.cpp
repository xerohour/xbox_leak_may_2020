//
//	peerserver.cpp
//		Server side of peer-to-peer ethernet bit error rate test.
//		Sends data to address received from a broadcast
//
#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>


const WORD  BROADCAST_PORT	= 10983;
const WORD  DATA_PORT		= 10984;
const INT	DATA_PACKETS	= 256;		// Number of packets to sent
const INT	DATA_BYTES		= 1024;		// Packet size


// Message IDs
enum {
	MSG_SEND_DATA,		// Server sends data
	MSG_QUIT			// Server quits
};


// Message payload
struct PEER_MESSAGE {
	BYTE Msg;
	XNADDR xnAddr;
};


VOID 
PeerServerStartTest(HANDLE LogHandle) {
	DWORD dwStatus;
	XNetStartupParams xnsp;
	INT Err, Bytes,i, j;
	WSADATA wsaData;
	XNADDR xnHostAddr;
	CHAR cAddr[20], cAddrLast[20];
	SOCKET sBroadcast, sData;
	SOCKADDR_IN saBroadcast, saData, saClient;
	BOOL bDone;
	PEER_MESSAGE ClientMsg;

	CHAR *Data, *cPtr;

	xSetOwnerAlias(LogHandle, "a-emebac");
	xSetComponent(LogHandle, "Hardware", "PeerServer");

	// Prepare send data
	xSetFunctionName(LogHandle, "GlobalAlloc");
	Data = (CHAR *)GlobalAlloc(GPTR, DATA_PACKETS*DATA_BYTES);
	if(Data == NULL) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "GlobalAlloc(%d bytes) FAILED Returned %d", DATA_PACKETS*DATA_BYTES, GetLastError());
		return;
	}
	cPtr = Data;
	for(i = 0; i < DATA_PACKETS; i++) {
		for(j = 0; j < DATA_BYTES; j++) {
			*cPtr = (CHAR)i;
			cPtr++;
		}
	}

	// Get link status
	xSetFunctionName(LogHandle, "XNetGetEthernetLinkStatus");
	dwStatus = XNetGetEthernetLinkStatus();
	if(dwStatus == 0) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "No Ethernet cable is connected");
	}
	if(dwStatus & XNET_ETHERNET_LINK_ACTIVE) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Ethernet cable is connected and active");
	}
	if(dwStatus & XNET_ETHERNET_LINK_100MBPS) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Ethernet link is set to 100 Mbps");
	}
	if(dwStatus & XNET_ETHERNET_LINK_10MBPS) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Ethernet link is set to 10 Mbps");
	}
	if(dwStatus & XNET_ETHERNET_LINK_FULL_DUPLEX) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Ethernet link is in full duplex mode");
	}
	if(dwStatus & XNET_ETHERNET_LINK_HALF_DUPLEX) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Ethernet link is in half duplex mode");
	}

	// Init XNet
	xSetFunctionName(LogHandle, "XNetStartup");
	memset(&xnsp, 0, sizeof(xnsp));
	xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
	xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
	Err = XNetStartup(&xnsp);
	if(Err != 0) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "XNetStartup() FAILED Returned %d", Err);
		return;
	}

	// Init Winsock
	Err = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(Err != 0) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "WSAStartup() FAILED Returned %d", Err);
		return;
	}

	// Get local address
	xSetFunctionName(LogHandle, "XNetGetTitleXnAddr");
	do {
		dwStatus = XNetGetTitleXnAddr(&xnHostAddr);
	} while(dwStatus == XNET_GET_XNADDR_PENDING);
	XNetInAddrToString(xnHostAddr.ina, cAddr, sizeof(cAddr));
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Server Address: %s", cAddr);

	// Broadcast socket
	xSetFunctionName(LogHandle, "Broadcast Socket");
	sBroadcast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sBroadcast == INVALID_SOCKET) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "socket(sBroadcast) FAILED WSALastError = %d", WSAGetLastError ());
		return;
	}

	// Bind to INADDR_ANY and broadcast port
	ZeroMemory(&saBroadcast, sizeof(SOCKADDR_IN));
	saBroadcast.sin_family = AF_INET;
	saBroadcast.sin_addr.s_addr = INADDR_ANY;
	saBroadcast.sin_port = htons(BROADCAST_PORT);
	Err = bind(sBroadcast, (const sockaddr *)&saBroadcast, sizeof(SOCKADDR_IN));
	if(Err == SOCKET_ERROR) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "bind(INADDR_ANY) FAILED WSALastError = %d", WSAGetLastError ());
		return;
	}

	// Data socket
	sData = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sData == INVALID_SOCKET) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "socket(sData) FAILED WSALastError = %d", WSAGetLastError ());
		return;
	}

	// Bind to data port
	ZeroMemory(&saData, sizeof(SOCKADDR_IN));
	saData.sin_family = AF_INET;
	saData.sin_addr.s_addr = INADDR_ANY;
	saData.sin_port = htons(DATA_PORT);
	Err = bind(sData, (const sockaddr *)&saData, sizeof(SOCKADDR_IN));
	if(Err == SOCKET_ERROR) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "bind(INADDR_ANY) FAILED WSALastError = %d", WSAGetLastError ());
		return;
	}

	// Process messages received from broadcast socket
	ZeroMemory(cAddrLast, 20);
	bDone = FALSE;
	while(!bDone) {
		// Wait to receive a peer message
		Bytes = recv(sBroadcast, (char *)&ClientMsg, sizeof(ClientMsg), 0);
		if(Bytes != sizeof(ClientMsg)) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "recv(BROADCAST_PORT) FAILED WSALastError = %d", WSAGetLastError ());
			return;
		}

		switch(ClientMsg.Msg) {
			case MSG_SEND_DATA:
				xSetFunctionName(LogHandle, "Send Data");
				XNetInAddrToString(ClientMsg.xnAddr.ina, cAddr, sizeof(cAddr));
				if(lstrcmpA(cAddr, cAddrLast) != 0) {
					xLog(gbConsoleOut, LogHandle, XLL_INFO, "Sending Data To: %s", cAddr);
					lstrcpyA(cAddrLast, cAddr);
				}

				ZeroMemory(&saClient, sizeof(SOCKADDR_IN));
				saClient.sin_family = AF_INET;
				CopyMemory(&saClient.sin_addr, &ClientMsg.xnAddr.ina, sizeof(in_addr));
				saClient.sin_port = htons(DATA_PORT);
				for(i = 0; i < DATA_PACKETS; i++) {
					Err = sendto(sData, (const char *)(Data + i*DATA_BYTES), DATA_BYTES, 0, 
						(const sockaddr *)&saClient, sizeof(SOCKADDR_IN));
					/*
					if(Err = SOCKET_ERROR) {
						xLog(gbConsoleOut, LogHandle, XLL_FAIL, "sendto(%d) FAILED WSALastError = %d", i, WSAGetLastError ());
						return;
					}
					*/
				}
				break;

			case MSG_QUIT:
				bDone = TRUE;
				break;
		}
	}


	// Shut down network
	xSetFunctionName(LogHandle, "Shut Down");
	closesocket(sData);
	closesocket(sBroadcast);
	WSACleanup();
	XNetCleanup();
}


VOID
PeerServerEndTest() {
}


#if !defined(HARNESSLIB)
void __cdecl main() {
	HANDLE LogHandle;

	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);

	// xlog
	LogHandle = xCreateLog_A("t:\\peerserver.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "PeerServer: couldn't create log (%s)", WinErrorSymbolicName(GetLastError()));
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "PeerServer: End - Waiting for reboot...");
		Sleep(INFINITE);
    }

	gbConsoleOut = TRUE;

	// Start Test
	PeerServerStartTest(LogHandle);

	// End Test
	PeerServerEndTest();

	// xlog
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "PeerServer: End - Waiting for reboot...");
	xCloseLog(LogHandle);

	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	Sleep(INFINITE);

	// Do the de-initialize that the harness does
	// dxconio
	xReleaseConsole();

}
#endif

//
// Export function pointers of StartTest and EndTest
//
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( peerserver )
#pragma data_seg()

BEGIN_EXPORT_TABLE( peerserver )
    EXPORT_TABLE_ENTRY( "StartTest", PeerServerStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", PeerServerEndTest )
END_EXPORT_TABLE( peerserver )


