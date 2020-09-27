#ifndef __UDPTEST_H__
#define __UDPTEST_H__

//==================================================================================
// Includes
//==================================================================================
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xnetref.h>
#include <xlog.h>
#include <xtestlib.h>
#include <netsync.h>
#include <xlog.h>
#include <xboxp.h>
#include <stdio.h>

#include "udpmsg.h"

//==================================================================================
// Defines
//==================================================================================
#define UDP_SERVER_DLL_W L"udpserv.dll"
#define UDP_SERVER_MAIN_FUNCTION_A "UdpServerCallbackFunction"
#define UDP_SERVER_START_FUNCTION_A "UdpServerStartFunction"
#define UDP_SERVER_STOP_FUNCTION_A "UdpServerStopFunction"

#define MAX_REQUEST_RETRIES 5
#define RETRY_TIMEOUT 3000
#define TIME_INFINITY 10000

//==================================================================================
// Typedefs
//==================================================================================

// UDP test function
typedef BOOL (*PUDP_TEST_FUNCTION)(WORD LowPort, WORD HighPort);

// States that sockets can be monitored for
typedef enum _SOCKET_STATE
{
	READABLE,
	WRITEABLE,
	EXCEPTION
} SOCKET_STATE;

//==================================================================================
// Prototypes 
//==================================================================================
// Harness functions
BOOL WINAPI UdpTestDllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext);
VOID WINAPI UdpTestStartTest(IN HANDLE hLog);
VOID WINAPI UdpTestEndTest();

// Test case functions
BOOL UDPSourceMaxTest (WORD LowPort, WORD HighPort);
BOOL UDPSourceMinTest (WORD LowPort, WORD HighPort);
BOOL UDPSourceZeroTest (WORD LowPort, WORD HighPort);
BOOL UDPDestMaxTest (WORD LowPort, WORD HighPort);
BOOL UDPDestMinTest (WORD LowPort, WORD HighPort);
BOOL UDPDestZeroTest (WORD LowPort, WORD HighPort);
BOOL UDPLenBelowMinTest (WORD LowPort, WORD HighPort);
BOOL UDPLenAtMinTest (WORD LowPort, WORD HighPort);
BOOL UDPLenAboveMinTest (WORD LowPort, WORD HighPort);
BOOL UDPLenAboveTotalTest (WORD LowPort, WORD HighPort);
BOOL UDPLenBelowTotalTest (WORD LowPort, WORD HighPort);
BOOL UDPLenMaxTest (WORD LowPort, WORD HighPort);
BOOL UDPCheckOnesTest (WORD LowPort, WORD HighPort);
BOOL UDPCheckZerosTest (WORD LowPort, WORD HighPort);

#endif // __UDPTEST_H__
