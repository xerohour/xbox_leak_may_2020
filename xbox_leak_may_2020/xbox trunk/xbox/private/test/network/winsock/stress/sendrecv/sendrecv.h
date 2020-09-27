#ifndef __SENDRECV_H__
#define __SENDRECV_H__

//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _XBOX
#include <xtl.h>
#include "xlog.h"
#else
#include <winsock.h>
#include <stdarg.h>
#include <tchar.h>
#endif


//==================================================================================
// Defines
//==================================================================================
#define	SEND_RECV_PORT		65533
#define	FIND_SERVER_PORT	65532
#define TIME_INFINITY		10000
#define	MAX_PACKET_SIZE		4096

#ifndef _XBOX
#define XLL_INFO			0
#define	XLL_WARN			0
#define XLL_FAIL			0
#define XLL_PASS			0
#endif

//==================================================================================
// Typedefs
//==================================================================================

// States that sockets can be monitored for
typedef enum _SOCKET_STATE
{
	READABLE,
	WRITEABLE,
	EXCEPTION
} SOCKET_STATE;

// Lists all the possible tests cases that are supported
typedef enum _TEST_CASE
{
	NO_TEST,
	DOWNSTREAM_DATA,
	UPSTREAM_DATA,
	UPSTREAM_DATA_DELAYED_RECV,
	ECHO,
	ECHO_DELAYED_RECV
} TEST_CASE;

// Lists all the packet types available
typedef enum _PACKET_TYPE
{
	FINDSERV,
	SERVREPLY,
	TESTREQUEST,
	TESTREPLY,
	SENDRECV,
	ENDTEST
} PACKET_TYPE;

// This structure is used to pass test params into the test case functions
typedef struct _TEST_PARAMS
{
	HANDLE hLog;
	BOOL fInitiateConnection;
	BOOL fUseDelays;
	BOOL fNonBlocking;
	BOOL fUseCriticalSection;
	BOOL fCleanUpAfterTest;
	DWORD dwSecondsToRun;
	CHAR szDestinationAddress[16];
} TEST_PARAMS;

// This structure defines a generic packet for initial casting
typedef struct _GENERIC_PACKET
{
	DWORD	dwPacketType;
} GENERIC_PACKET;

// This structure defines the simple packet used to detect sendrecv servers
typedef struct _FINDSERV_PACKET
{
	DWORD	dwPacketType;
} FINDSERV_PACKET;

// This structure defines the simple packet used to reply to client detection
typedef struct _SERVREPLY_PACKET
{
	DWORD	dwPacketType;
} SERVREPLY_PACKET;

// This structure defines the simple packet used to detect sendrecv servers
typedef struct _TESTREQEST_PACKET
{
	DWORD		dwPacketType;
	DWORD		dwTestID;
	TEST_CASE	TestRequested;
} TESTREQUEST_PACKET;

// This structure defines the simple packet used to detect sendrecv servers
typedef struct _TESTREPLY_PACKET
{
	DWORD		dwPacketType;
	DWORD		dwTestID;
	TEST_CASE	TestRequested;
} TESTREPLY_PACKET;

// This structure defines the header for the packets this test exchanges
typedef struct _SENDRECV_PACKET
{
	DWORD	dwPacketType;
	DWORD	dwPacketNumber;
	DWORD	dwPayloadSize;
	DWORD	dwPayloadChecksum;
	BYTE	bPayload[1];
} SENDRECV_PACKET;

// This structure defines the simple packet used to reply to client detection
typedef struct _ENDTEST_PACKET
{
	DWORD	dwPacketType;
} ENDTEST_PACKET;

// This structure is used to log the number of echos being sent
typedef struct _ECHO_SERVER_PARAM
{
	BOOL				fTestActive;
	BOOL				fInSend;
	BOOL				fInRecv;
	DWORD				dwTotalEchos;
	HANDLE				hLog;
} ECHO_SERVER_PARAM;

// This structure is used to share access to a socket via a critical section
typedef struct _ECHO_CLIENT_PARAM
{
	TEST_PARAMS			*pTestParams;
	BOOL				fTestActive;
	BOOL				fInSend;
	BOOL				fInRecv;
	DWORD				dwInProgressOnSends;
	DWORD				dwInProgressOnRecvs;
	DWORD				dwTotalSends;
	DWORD				dwTotalRecvs;
	SOCKET				Connection;
	CRITICAL_SECTION	SocketCriticalSection;
} ECHO_CLIENT_PARAM;


//==================================================================================
// Prototypes
//==================================================================================
SOCKET EstablishConnection(HANDLE hLog, LPCSTR szDestinationAddress, BOOL fNonBlocking);
SOCKET WaitForIncomingConnection(HANDLE hLog, BOOL fNonBlocking);
SOCKET CreateBroadcastListenerSocket(HANDLE hLog, WORD wPort);
TEST_CASE ProcessNextBroadcastPacket(HANDLE hLog, SOCKET BroadcastListener);
DWORD WINAPI EchoClientRecvThread(LPVOID *pParm);
BOOL FindServer(HANDLE hLog, CHAR *szDestinationAddress);
BOOL RequestTest(HANDLE hLog, TEST_CASE TestCase, CHAR *szDestinationAddress);

BOOL IsSocketReady(SOCKET Socket, SOCKET_STATE SocketState, DWORD dwSecondsToWait);
BOOL IsTimeExpired(DWORD dwSecondsToRun, DWORD dwStartTick);
BOOL IsServer();

DWORD WINAPI DoSendTest(LPVOID pParam);
DWORD WINAPI DoRecvTest(LPVOID pParam);
DWORD WINAPI DoSingleThreadEchoServerTest(LPVOID pParam);
DWORD WINAPI DoMultiThreadEchoClientTest(LPVOID pParam);

#ifndef _XBOX
VOID WINAPI StartTest(IN HANDLE  hLog);
VOID WINAPI EndTest();
#endif

#ifdef __cplusplus
extern "C" {
#endif
WINBASEAPI
UINT
WINAPI
GetProfileIntA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpKeyName,
    IN INT nDefault
    );
WINBASEAPI
DWORD
WINAPI
GetProfileStringA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpKeyName,
    IN LPCSTR lpDefault,
    OUT LPSTR lpReturnedString,
    IN DWORD nSize
    );
#ifdef __cplusplus
}
#endif

#endif // __SLDGMAIN_H__
