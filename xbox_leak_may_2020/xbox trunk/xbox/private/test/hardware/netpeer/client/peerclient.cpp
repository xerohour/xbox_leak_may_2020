//
//	peerclient.cpp
//		Client side of peer-to-peer ethernet bit error rate test.
//		Broadcast address and 'send data' message to server
//		Receive data from server
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

// Globals
HANDLE ghGamePads[4];
// BOOL gbPause = FALSE;
BOOL gbQuit = FALSE;


VOID
InitGamePads() {
	DWORD i;
	XDEVICE_PREALLOC_TYPE xdpt;

	// Initialize gamepads
	xdpt.DeviceType = XDEVICE_TYPE_GAMEPAD;
	xdpt.dwPreallocCount = 4;

	XInitDevices(1, &xdpt);

	// Get a mask of all currently available devices
    DWORD dwDeviceMask = XGetDevices(XDEVICE_TYPE_GAMEPAD);

    // Open the devices
	for(i = 0; i < 4; i++){
		// Initialize game pad array
		ghGamePads[i] = NULL;

        if( dwDeviceMask & (1<<i) ) {
            // Get a handle to the device
            ghGamePads[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL );
			// xDebugStringA("Added Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
        }
	}
}


VOID
ReadGamePads() {
	DWORD dwInsertions, dwRemovals;
	DWORD b, i, Buttons = 0;
	XINPUT_STATE State;
	BOOL AnalogButtons[8];

	// Get gamepad insertions and removals
	XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals);
	for(i = 0; i < 4; i++) {
		// Handle removed devices.
		if(dwRemovals & (1<<i) ) {
			XInputClose( ghGamePads[i]);
			// xDebugStringA("HD SIT: Removed Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
			ghGamePads[i] = NULL;
		}

		// Handle inserted devices
		if(dwInsertions & (1<<i)) {
			ghGamePads[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL);
			// xDebugStringA("HD SIT: Added Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
		}
	}

	// Init button variables
	Buttons = 0;
	for(b = 0; b < 8; b++) {
		AnalogButtons[b] = FALSE;
	}

	// Get button presses
	for(i = 0; i < 4; i++) {
		if(ghGamePads[i] != NULL) {
			XInputGetState(ghGamePads[i], &State);

			// Digital buttons
			Buttons |= State.Gamepad.wButtons;

			// Analog buttons (threshold = 25/256)
			for(b = 0; b < 8; b++) {
				AnalogButtons[b] |= (State.Gamepad.bAnalogButtons[b] > 25);
			}
		}
	}

	// Check button presses
	// Back button pauses app
	/*
	if((Buttons & XINPUT_GAMEPAD_BACK) > 0) {
		if(!gbPause) {
			xDebugStringA("HD SIT: Application Paused\n");
			gbPause = TRUE;
		}
	}

	// Start button unpauses app
	if((Buttons & XINPUT_GAMEPAD_START) > 0 ) {
		if(gbPause) {
			xDebugStringA("HD SIT: Application NOT Paused\n");
			gbPause = FALSE;
		}
	}
	*/

	// Both triggers plus Black button quits app (from Samples)
	if(AnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] & AnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] & AnalogButtons[XINPUT_GAMEPAD_BLACK]) {
		if(!gbQuit) {
			// xDebugStringA("HD SIT: Application Quiting...\n");
			gbQuit = TRUE;
		}
	}
}


VOID
CloseGamePads() {
	DWORD i;

	for(i = 0; i < 4; i++ ) {
		if(ghGamePads[i] != NULL) {
			XInputClose(ghGamePads[i]);
			// xDebugStringA("HD SIT: Removed Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
			ghGamePads[i] = NULL;
		}
	}
}


VOID 
PeerClientStartTest(HANDLE LogHandle) {
	DWORD dwStatus, dwLastTickCount, dwLogTime;
	XNetStartupParams xnsp;
	INT Err, Bytes,i, j, p, Errors;
	WSADATA wsaData;
	CHAR cAddr[20];
	SOCKET sBroadcast, sData;
	SOCKADDR_IN saBroadcast, saData;
	BOOL bBroadcast;
	PEER_MESSAGE ClientMsg;
	LARGE_INTEGER StartTime, EndTime,  Frequency;
	LARGE_INTEGER TotalTime, TotalBytes, TotalReps;
	DOUBLE Rate;

	UCHAR *Data, *cPtr;

	xSetOwnerAlias(LogHandle, "a-emebac");
	xSetComponent(LogHandle, "Hardware", "PeerClient");
	QueryPerformanceFrequency(&Frequency);

	// Allocate receive data array
	xSetFunctionName(LogHandle, "GlobalAlloc");
	Data = (UCHAR *)GlobalAlloc(GPTR, DATA_PACKETS*DATA_BYTES);
	if(Data == NULL) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "GlobalAlloc(%d bytes) FAILED Returned %d", DATA_PACKETS*DATA_BYTES, GetLastError());
		return;
	}

	// Initialize gamepads
	InitGamePads();
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Press both triggers and the 'Black' button to quit the application");

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
		dwStatus = XNetGetTitleXnAddr(&ClientMsg.xnAddr);
	} while(dwStatus == XNET_GET_XNADDR_PENDING);
	XNetInAddrToString(ClientMsg.xnAddr.ina, cAddr, sizeof(cAddr));
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Client Address: %s", cAddr);

	// Broadcast socket
	xSetFunctionName(LogHandle, "Broadcast Socket");
	sBroadcast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sBroadcast == INVALID_SOCKET) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "socket(sBroadcast) FAILED WSALastError = %d", WSAGetLastError ());
		return;
	}

	// Set broadcast mode
	bBroadcast = TRUE;
	Err = setsockopt(sBroadcast, SOL_SOCKET, SO_BROADCAST, (const char *)&bBroadcast, sizeof(bBroadcast));
	if(Err == SOCKET_ERROR) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "setsockopt(SO_BROADCAST) FAILED WSALastError = %d", WSAGetLastError ());
		return;
	}

	// Broadcast address
	ZeroMemory(&saBroadcast, sizeof(SOCKADDR_IN));
	saBroadcast.sin_family = AF_INET;
	saBroadcast.sin_addr.s_addr = INADDR_BROADCAST;
	saBroadcast.sin_port = htons(BROADCAST_PORT);

	// Data socket
	xSetFunctionName(LogHandle, "Data Socket");
	sData = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sData == INVALID_SOCKET) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "socket(sData) FAILED WSALastError = %d", WSAGetLastError ());
		return;
	}

	// Bind data address
	ZeroMemory(&saData, sizeof(SOCKADDR_IN));
	saData.sin_family = AF_INET;
	saData.sin_addr.s_addr = INADDR_ANY;
	saData.sin_port = htons(DATA_PORT);
	Err = bind(sData, (const sockaddr *)&saData, sizeof(SOCKADDR_IN));
	if(Err == SOCKET_ERROR) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "bind(INADDR_ANY) FAILED WSALastError = %d", WSAGetLastError ());
		return;
	}

	// Get LogTime from testini.ini
	xSetFunctionName(LogHandle, "Receive Data");
	dwLogTime = (DWORD)GetProfileIntA("peerclient", "LogMinutes", 1);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Logging every %d minutes", dwLogTime);
	dwLogTime *= 60*1000;	// ms
	TotalTime.QuadPart = 0;
	TotalBytes.QuadPart = 0;
	TotalReps.QuadPart = 0;
	dwLastTickCount = GetTickCount();
	Errors = 0;

	while(!gbQuit) {
		// Send 'send data' message to server
		ClientMsg.Msg = MSG_SEND_DATA;
		Err = sendto(sBroadcast, (const char *)&ClientMsg, sizeof(ClientMsg), 0, 
						(const sockaddr *)&saBroadcast, sizeof(SOCKADDR_IN));

		/*
		if(Err = SOCKET_ERROR) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "sendto(BROADCAST_PORT) FAILED WSALastError = %d", WSAGetLastError ());
			return;
		}
		*/
		
		// Receive test data
		xSetFunctionName(LogHandle, "Receive Data");
		QueryPerformanceCounter(&StartTime);
		for(p = 0; p < DATA_PACKETS; p++) {
			Bytes = recv(sData, (char *)(Data + p*DATA_BYTES), DATA_BYTES, 0);
			if(Bytes != DATA_BYTES) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "recv(%d) Rep %I64d FAILED WSALastError = %d", 
					p, TotalReps.QuadPart, WSAGetLastError());
				return;
			}
			TotalBytes.QuadPart += Bytes;
		}
		QueryPerformanceCounter(&EndTime);
		TotalTime.QuadPart += (EndTime.QuadPart - StartTime.QuadPart);

		// Check/clear data
		cPtr = Data;
		for(i = 0; i < DATA_PACKETS; i++) {
			for(j = 0; j < DATA_BYTES; j++) {
				if(*cPtr != i) {
					Errors++;
				}
				*cPtr = 0;
				cPtr++;
			}
		}

		if(GetTickCount() - dwLastTickCount > dwLogTime) {
			// Reset last tick count
			dwLastTickCount = GetTickCount();

			// Log data rate and errors
			Rate = (DOUBLE)TotalBytes.QuadPart;
			Rate *= 8.0;	// bit
			Rate /= (DOUBLE)TotalTime.QuadPart;
			Rate *= (DOUBLE)Frequency.QuadPart;
			Rate /= 1024.0;	// K bits
			Rate /= 1024.0;	// M bits
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "Received Data Rate: %f Mbps Total Reps = %I64d Total Errors = %d ",
				Rate, TotalReps.QuadPart, Errors);

		}
		// Count reps
		TotalReps.QuadPart++;

		// Check for button presses
		ReadGamePads();
	}

	// Log data rate and errors
	Rate = (DOUBLE)TotalBytes.QuadPart;
	Rate *= 8.0;	// bit
	Rate /= (DOUBLE)TotalTime.QuadPart;
	Rate *= (DOUBLE)Frequency.QuadPart;
	Rate /= 1024.0;	// K bits
	Rate /= 1024.0;	// M bits
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Received Data Rate: %f Mbps Total Reps = %I64d Total Errors = %d ",
		Rate, TotalReps.QuadPart, Errors);

	// Send 'quit' message to server
	xSetFunctionName(LogHandle, "Quit");
	ClientMsg.Msg = MSG_QUIT;
	Err = sendto(sBroadcast, (const char *)&ClientMsg, sizeof(ClientMsg), 0, 
					(const sockaddr *)&saBroadcast, sizeof(SOCKADDR_IN));
	/*
	if(Err = SOCKET_ERROR) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "sendto(BROADCAST_PORT) FAILED WSALastError = %d", WSAGetLastError ());
		return;
	}
	*/

	// Check data
	// Shut down network
	xSetFunctionName(LogHandle, "Shut Down");
	closesocket(sData);
	closesocket(sBroadcast);
	WSACleanup();
	XNetCleanup();
	CloseGamePads();
	GlobalFree(Data);
}


VOID
PeerClientEndTest() {
}


#if !defined(HARNESSLIB)
void __cdecl main() {
	HANDLE LogHandle;

	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);

	// xlog
	LogHandle = xCreateLog_A("t:\\peerclient.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "PeerClient: couldn't create log (%s)", WinErrorSymbolicName(GetLastError()));
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "PeerClient: End - Waiting for reboot...");
		Sleep(INFINITE);
    }

	gbConsoleOut = TRUE;

	// Start Test
	PeerClientStartTest(LogHandle);

	// End Test
	PeerClientEndTest();

	// xlog
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "PeerClient: End - Waiting for reboot...");
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
DECLARE_EXPORT_DIRECTORY( peerclient )
#pragma data_seg()

BEGIN_EXPORT_TABLE( peerclient )
    EXPORT_TABLE_ENTRY( "StartTest", PeerClientStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", PeerClientEndTest )
END_EXPORT_TABLE( peerclient )


