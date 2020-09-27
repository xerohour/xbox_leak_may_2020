#ifndef __TCPTEST_H__
#define __TCPTEST_H__

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

#include "tcpmsg.h"

//==================================================================================
// Defines
//==================================================================================
#define TCP_SERVER_DLL_W L"tcpserv.dll"
#define TCP_SERVER_MAIN_FUNCTION_A "TcpServerCallbackFunction"
#define TCP_SERVER_START_FUNCTION_A "TcpServerStartFunction"
#define TCP_SERVER_STOP_FUNCTION_A "TcpServerStopFunction"

#define MAX_REQUEST_RETRIES 5
#define RETRY_TIMEOUT 3000
#define SIM_REASM_MAX 4
#define TIME_INFINITY 10000

//==================================================================================
// Typedefs
//==================================================================================

// TCP test function
typedef BOOL (*PTCP_TEST_FUNCTION)(WORD LowPort, WORD HighPort);

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
BOOL WINAPI TcpTestDllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext);
VOID WINAPI TcpTestStartTest(IN HANDLE hLog);
VOID WINAPI TcpTestEndTest();

// Test case functions
BOOL TCPSeqNumMaxTest(WORD LowPort, WORD HighPort);
BOOL TCPSeqNumMinTest(WORD LowPort, WORD HighPort);
BOOL TCPDataOffBelowMinTest(WORD LowPort, WORD HighPort);
BOOL TCPDataOffAboveAvailTest(WORD LowPort, WORD HighPort);
#if 0
BOOL TCPReservedSetTest(WORD LowPort, WORD HighPort);
BOOL TCPWindowMinTest(WORD LowPort, WORD HighPort);
BOOL TCPWindowMaxTest(WORD LowPort, WORD HighPort);
BOOL TCPChecksumBadTest(WORD LowPort, WORD HighPort);
BOOL TCPUrgentAfterEndTest(WORD LowPort, WORD HighPort);
BOOL TCPUrgentAtEndTest(WORD LowPort, WORD HighPort);
BOOL TCPUrgentBeforeEndTest(WORD LowPort, WORD HighPort);
BOOL TCPUrgentMaxTest(WORD LowPort, WORD HighPort);
BOOL TCPUrgentNoFlagTest(WORD LowPort, WORD HighPort);
BOOL TCPOptCutoffTest(WORD LowPort, WORD HighPort);
BOOL TCPOptAtEndTest(WORD LowPort, WORD HighPort);
BOOL TCPOptBeforeEndTest(WORD LowPort, WORD HighPort);
BOOL TCPOptNoOptsTest(WORD LowPort, WORD HighPort);
BOOL TCPOptMaxOptsTest(WORD LowPort, WORD HighPort);
BOOL TCPOptMaxOptLenTest(WORD LowPort, WORD HighPort);
BOOL TCPOptEndOfOptsTest(WORD LowPort, WORD HighPort);
BOOL TCPOptMssZeroTest(WORD LowPort, WORD HighPort);
BOOL TCPOptMssOneTest(WORD LowPort, WORD HighPort);
BOOL TCPOptMssMaxTest(WORD LowPort, WORD HighPort);
BOOL TCPConnSynAndNoAckTest(WORD LowPort, WORD HighPort);
BOOL TCPConnExtraSynTest(WORD LowPort, WORD HighPort);
BOOL TCPConnAckWrongSynTest(WORD LowPort, WORD HighPort);
BOOL TCPConnSynAckWrongSynTest(WORD LowPort, WORD HighPort);
BOOL TCPConnAckNoConnTest(WORD LowPort, WORD HighPort);
BOOL TCPConnSynAckListenerTest(WORD LowPort, WORD HighPort);
BOOL TCPShutFinAndNoAckTest(WORD LowPort, WORD HighPort);
#endif

#endif // __TCPTEST_H__
