#ifndef __DPSTRESS_H__
#define __DPSTRESS_H__

//==================================================================================
// Includes
//==================================================================================
#include <xtl.h>
#include <stdio.h>
#include <xlog.h>
#include <xtestlib.h>
#include <netsync.h>
#include <xlog.h>

#include <dplay8.h>
#include <dpaddr.h>

#include "dpmsg.h"
#include "stressutils.h"

namespace DPlayStressNamespace {

//==================================================================================
// Stress function prototypes
//==================================================================================
BOOL PeerStress(HANDLE hLog, HANDLE hNetsync, DP_HOSTINFO *pMsg, IN_ADDR *pLocalAddr, BOOL fStartedAsHost);

// Private heap functions
LPVOID MemAlloc(DWORD dwSize);
BOOL   MemFree(LPVOID pBuffer);

//==================================================================================
// Defines
//==================================================================================
// BUGBUG - change to real names
#define CORPNET_NETSYNC_NAME "xboxnetsync1"
//#define CORPNET_NETSYNC_NAME "tristanj0"
#define INTERNET_NETSYNC_NAME "xonline.org"
#define DPSTRESS_PORT 2302

#define DP_PEER_STRESS_DLL_W L"dpserv.dll"
#define DP_PEER_STRESS_MAIN_FUNCTION_A "DpStressCallbackFunction"
#define DP_PEER_STRESS_START_FUNCTION_A "DpStressStartFunction"
#define DP_PEER_STRESS_STOP_FUNCTION_A "DpStressStopFunction"

#define UNKNOWN_HOST_STRING		"Unknown"

#define MAX_GROUPS					(3)

#define HOST_KEEPALIVE_INTERVAL		(5000)
#define MAX_PENDING_SEND_BYTES		(10 * 1024)
#define MAX_ENUM_TIME				(30000)
#define MAX_CONN_TIME				(60000)

#define SEND_DATA_MIN_SIZE			(8)
#define SEND_DATA_MAX_SIZE			(256)
#define ENUM_DATA_MIN_SIZE			(8)
#define ENUM_DATA_MAX_SIZE			(1000)
#define	ENUMREPLY_DATA_MIN_SIZE		(8)
#define CONN_DATA_MIN_SIZE			(8)
#define CONN_DATA_MAX_SIZE			(1000)
#define CONNREPLY_DATA_MIN_SIZE		(8)
#define CONNREPLY_DATA_MAX_SIZE		(1000)
#define GROUP_DATA_MIN_SIZE			(8)
#define GROUP_DATA_MAX_SIZE			(1000)
#define APP_DESC_MIN_SIZE			(8)
#define APP_DESC_MAX_SIZE			(10)

//==================================================================================
// Netsync Macros
//==================================================================================
// These macros are used in DirectPlay stress tests that use Netsync for determining
// server and host addresses...

// Cleans up all variables... deallocating buffers and closing handles...
// Used at the end of each loop and whenever the loop is being exited
#define CLEAN_NETSYNC_VARIABLES			(hClient && (hClient != INVALID_HANDLE_VALUE)) ? NetsyncCloseClient(hClient) : 0;\
										hClient = NULL;\
										(hEnum && (hEnum != INVALID_HANDLE_VALUE)) ? NetsyncCloseFindServer(hEnum) : 0;\
										hEnum = NULL;\
										pMsg ? NetsyncFreeMessage((char *) pMsg) : 0;\
										pMsg = NULL;\
										wLowPort = wHighPort = 0;\
										dwMsgSize = dwRecvStatus = dwAddrType = 0;\
										dwStartTime = dwEndTime = dwTotalTime = 0;

// The start of the loop code.  Allocates all necessary variables for Netsync communication
// and starts a while loop
#define BEGIN_STRESS_LOOP(log)			{\
											PDP_HOSTINFO pMsg = NULL;\
											IN_ADDR HostAddr, FromAddr, LocalAddr;\
											HANDLE hEnum = NULL, hClient = NULL;\
											DWORD dwMsgSize = 0, dwRecvStatus = 0, dwAddrType = 0;\
											DWORD dwStartTime = 0, dwEndTime = 0, dwTotalTime = 0;\
											WORD wLowPort = 0, wHighPort = 0;\
											/*xLog(log, XLL_INFO, "Starting stress loop...");*/\
											\
											while(TRUE)\
											{
						
// The end of the while loop code.  Cleans up all the variables and prints a message if the
// loop has been ended				
#define END_STRESS_LOOP(log)					CLEAN_NETSYNC_VARIABLES;\
											}\
											\
											xLog(log, XLL_INFO, "Leaving stress loop...");\
										}

// Cleans Netsync variables and restarts the loop.  Used when a problem has been encountered
// that will require the client to resync with the Netsync server
#define WAIT10_AND_LOOP(log)			/*xLog(log, XLL_WARN, "Wait 10 seconds and try again...");*/\
										CLEAN_NETSYNC_VARIABLES;\
										Sleep(10000);\
										continue

// Used to exit the loop.  Not currently used anywhere so that stress tests will contiue trying
// to connect to the Netsync server even if it is temporarily unavailable
#define EXIT_LOOP(log)					xLog(log, XLL_WARN, "An unrecoverable error occurred!");\
										CLEAN_NETSYNC_VARIABLES;\
										break

// Gets the local IP address and prins a message indicating what type it is
#define GET_LOCALINFO(log)				dwAddrType = XnetGetIpAddress(&LocalAddr);\
										switch(dwAddrType & (XNET_ADDR_DHCP | XNET_ADDR_AUTOIP | XNET_ADDR_STATIC))\
										{\
										case XNET_ADDR_DHCP:\
											/*xLog(g_hLog, XLL_INFO, "Local address %s was obtained via DHCP", inet_ntoa(LocalAddr));*/\
											break;\
										case XNET_ADDR_AUTOIP:\
											/*xLog(g_hLog, XLL_INFO, "Local address %s was obtained via AutoIP", inet_ntoa(LocalAddr));*/\
											break;\
										case XNET_ADDR_STATIC:\
											/*xLog(g_hLog, XLL_INFO, "Local address %s was statically assigned", inet_ntoa(LocalAddr));*/\
											break;\
										default:\
											xLog(g_hLog, XLL_WARN, "Couldn't get an acceptable IP address: 0x%08x", dwAddrType);\
											LocalAddr.S_un.S_addr = 0;\
											WAIT10_AND_LOOP(hLog);\
										}
// Communicates with Netsync server and retries a host info message indicating where the Netsync server
// thinks the current host for this session is
#define GET_HOSTINFO(log, naddr, pinfo)	hEnum = NetsyncQueryServer(naddr.S_un.S_addr, SESSION_CALLBACK, pinfo);\
										if(hEnum == INVALID_HANDLE_VALUE)\
										{\
											xLog(log, XLL_WARN, "Netsync server %s can't run this test now", inet_ntoa(naddr));\
											WAIT10_AND_LOOP(log);\
										}\
										\
										hClient = NetsyncCreateClient(hEnum, naddr.S_un.S_addr, &wLowPort, &wHighPort);\
										if(hClient == INVALID_HANDLE_VALUE)\
										{\
											xLog(log, XLL_WARN, "Couldn't start session on Netsync server %s", inet_ntoa(naddr));\
											WAIT10_AND_LOOP(log);\
										}\
										\
										NetsyncCloseFindServer(hEnum);\
										hEnum = NULL;\
										\
										dwStartTime = dwEndTime = dwTotalTime = dwMsgSize = 0;\
										pMsg = NULL;\
										\
										dwStartTime = GetTickCount();\
										\
										do\
										{\
											dwRecvStatus = NetsyncReceiveClientMessage(hClient, 30000 - dwTotalTime, (DWORD *) &FromAddr, &dwMsgSize, (char **) &pMsg);\
											if((dwRecvStatus == WAIT_OBJECT_0) && (pMsg->dwMessageId == DPMSG_HOSTINFO))\
												break;\
											\
											dwEndTime = GetTickCount();\
											dwTotalTime = (dwEndTime < dwStartTime) ? ((DWORD) 0xffffffff) - dwStartTime + dwEndTime : (dwEndTime - dwStartTime);\
											pMsg ? NetsyncFreeMessage((char *) pMsg) : 0;\
											pMsg = NULL;\
										} while(dwTotalTime < 30000);\
										\
										if((dwRecvStatus != WAIT_OBJECT_0) || (pMsg->dwMessageId != DPMSG_HOSTINFO))\
										{\
											xLog(log, XLL_WARN, "Netsync didn't reply with valid host info within 30 seconds, try again later...");\
											WAIT10_AND_LOOP(log);\
										}\
										\
										if(!pMsg->dwHostAddr)\
										{\
											xLog(log, XLL_WARN, "Netsync indicated this session's host is migrating, try again later...");\
											WAIT10_AND_LOOP(log);\
										}\
										\
										HostAddr.S_un.S_addr = pMsg->dwHostAddr;\
										xLog(log, XLL_INFO, "DirectPlay host address is %s", inet_ntoa(HostAddr))

// Examines address variables in the loop and determins if this client has been selected as the
// host for this session
#define IS_HOST()						(LocalAddr.S_un.S_addr && HostAddr.S_un.S_addr && (LocalAddr.S_un.S_addr == HostAddr.S_un.S_addr))

// Executes the peer stress test... passing in all necessary variables from Netsync
#define RUN_PEERSTRESS(log)				PeerStress(log, hClient, pMsg, &LocalAddr, IS_HOST())

//==================================================================================
// General Macros
//==================================================================================
// These macros are for general tasks such as determining handle validity and safely
// creating and closing various objects

#define IS_VALID_HANDLE(x)      		(x && (INVALID_HANDLE_VALUE != x))
#define SAFE_CLOSEHANDLE(h)				{\
											if (IS_VALID_HANDLE(h))\
											{\
												CloseHandle(h);\
												h = NULL;\
											}\
										}												
#define SAFE_CREATEEVENT(handle, lpEventAttributes, fManualReset, fInitialState, pszName)\
										{\
											if (handle != NULL)\
											{\
												xLog(hLog, XLL_WARN, "Handle %x is not NULL at line %i, DEBUGBREAK()-ing.", handle, __LINE__);\
												CloseHandle(handle);\
											}\
											handle = NULL;\
											handle = CreateEvent(lpEventAttributes, fManualReset, fInitialState, pszName);\
											if (handle == NULL)\
											{\
												DWORD dwError = GetLastError();\
												xLog(hLog, XLL_WARN, "Couldn't create event!: 0x%08x(%u)", dwError, dwError);\
											}\
										}

//==================================================================================
// Typedefs
//==================================================================================
// Player context that is allocated for all players in a session...
// allows callbacks to have instant access to information about the player
// that a given network event pertains to
typedef struct tagPLAYERCONTEXT
{
	DPNID					PlayerID;					// DirectPlay player ID this context refers to
	DWORD					TotalMessagesReceived;		// Counts total messages received from player
	DWORD					TotalBytesReceived;			// Counts total bytes received from player
	DWORD					dwPlayerInfoChangeCounter;	// Counts number of times player has changed info
	struct tagPLAYERCONTEXT	*pPrev;						// Pointer to the next player in the list
	struct tagPLAYERCONTEXT	*pNext;						// Pointer to the previous player in the list
} PLAYERCONTEXT, * PPLAYERCONTEXT;

// DirectPlay callback context for peer stress tests
typedef struct tagPEERSTRESSCONTEXT
{
	CHAR					szContextDescription[20];	// A short description of who this context belongs to
	PDIRECTPLAY8ADDRESS		pExpectedHostAddress;		// Expected host address from enumeration
	BOOL					fVerbose;					// Controls amount of logging from callback
	BOOL					fIsHost;					// Indicates if this context refers to the host
	DWORD					dwCurrentGroups;			// Tracks number of local groups created
	DPNID					LocalGroupIDs[MAX_GROUPS];	// Player IDs of the locally created groups
	DWORD					dwPlayersAdded[MAX_GROUPS];	// Number of players added to each group
	DWORD					dwGroupCreationCounter;		// Counts number of local groups that have been created
	CRITICAL_SECTION		Lock;						// Critical section for callback
	PPLAYERCONTEXT			pPlayerList;				// Maintains a list of known players
	HANDLE					hLog;						// Handle to logging subsystem
	DPNID					LocalPlayerID;				// Stores the id of the local player
	DWORD					dwCallbackTime;				// Tracks the number of milliseconds spent in the callback
	DWORD					dwPlayersConnected;			// Tracks the number of players currently connected
	DWORD					dwAppDescChangeCounter;		// Counts app desc changes
	HANDLE					hConnectComplete;			// Event for indicating incoming connections
	HANDLE					hSessionTerminated;			// Event for indicating session termination
	HANDLE					hHostMigrated;				// Event for indicating host migration
	HANDLE					hCriticalError;				// Event for indicating critical errors
	DWORD					TotalMessagesSent;			// Counts total message sent by this player
	DWORD					TotalBytesSent;				// Counts total bytes sent by this player
	DWORD					TotalGroupMessagesSent;		// Counts total message sent by this player
	DWORD					TotalGroupBytesSent;		// Counts total bytes sent by this player
} PEERSTRESSCONTEXT, * PPEERSTRESSCONTEXT;

// DirectPlay callback context for client stress tests
typedef struct tagCLIENTSTRESSCONTEXT
{
	CHAR					szContextDescription[20];	// A short description of who this context belongs to
	BOOL					fVerbose;					// Controls amount of logging from callback
	CRITICAL_SECTION		Lock;						// Critical section for callback
	HANDLE					hLog;						// Handle to logging subsystem
	DWORD					dwCallbackTime;				// Tracks the number of milliseconds spent in the callback
	HANDLE					hConnectComplete;			// Variable for handling incoming connections
	DWORD					dwServerInfoChangeCounter;	// Variable for handling host information changes
	DWORD					dwAppDescChangeCounter;		// Counts app desc changes
	HANDLE					hSessionTerminated;			// Variables for handling session termination
	HANDLE					hCriticalError;				// Event for indicating critical errors
} CLIENTSTRESSCONTEXT, * PCLIENTSTRESSCONTEXT;

// DirectPlay callback context for server stress tests
typedef struct tagSERVERSTRESSCONTEXT
{
	CHAR					szContextDescription[20];	// A short description of who this context belongs to
	BOOL					fVerbose;					// Controls amount of logging from callback
	CRITICAL_SECTION		Lock;						// Critical section for callback
	PPLAYERCONTEXT			pPlayerList;				// Maintains a list of known players
	HANDLE					hLog;						// Handle to logging subsystem
	DPNID					LocalPlayerID;				// Stores the id of the local player
	DWORD					dwCallbackTime;				// Tracks the number of milliseconds spent in the callback
	DWORD					dwPlayersConnected;			// Tracks the number of players currently connected
	DWORD					dwAppDescChangeCounter;		// Counts app desc changes
	HANDLE					hConnectComplete;			// Event for indicating incoming connections
	HANDLE					hSessionTerminated;			// Event for indicating session termination
	HANDLE					hCriticalError;				// Event for indicating critical errors
} SERVERSTRESSCONTEXT, * PSERVERSTRESSCONTEXT;

// Header applied to all data buffers that are sent between machines during
// DirectPlay tests
typedef struct tagDATAHEADER
{
	DWORD					dwDataLength;
	DWORD					dwChecksum;
} DATAHEADER, *PDATAHEADER;

//==================================================================================
// Globals
//==================================================================================
// {6C84E0F9-5CF8-43f2-A3C1-C69D8BCB2CB9}
DEFINE_GUID(GUID_PEER_STRESS, 
0x6c84e0f9, 0x5cf8, 0x43f2, 0xa3, 0xc1, 0xc6, 0x9d, 0x8b, 0xcb, 0x2c, 0xb9);

} // namespace DPlayStressNamespace

#endif