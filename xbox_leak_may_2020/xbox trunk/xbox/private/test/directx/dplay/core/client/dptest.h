#ifndef __DPTEST_H__
#define __DPTEST_H__

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

#include "guids.h"
#include "dpmsg.h"
#include "wrappeer.h"
#include "wrapserver.h"
#include "wrapclient.h"

namespace DPlayCoreNamespace {

//==================================================================================
// Defines
//==================================================================================
#define DP_SERVER_DLL_W L"dpserv.dll"
#define DP_SERVER_MAIN_FUNCTION_A "DpServerCallbackFunction"
#define DP_SERVER_START_FUNCTION_A "DpServerStartFunction"
#define DP_SERVER_STOP_FUNCTION_A "DpServerStopFunction"

#define SIMPLE_SERVER_EVENT "SimpleServer"
#define SIMPLE_HOST_EVENT "SimpleHost"
#define CS_ENUM_EVENT "CSEnum"
#define PEER_ENUM_EVENT "PeerEnum"
#define CSREJECT_ENUM_EVENT "CSReject"
#define PEERREJECT_ENUM_EVENT "PeerReject"
#define CSAPP_ENUM_EVENT "CSApp"
#define PEERAPP_ENUM_EVENT "PeerApp"
#define CSUSER_ENUM_EVENT "CSUser"
#define PEERUSER_ENUM_EVENT "PeerUser"
#define ENUM_VALIDATE_EVENT "EnumValidate"
#define SEND_BEGIN_EVENT "SendBegin"
#define SEND_END_EVENT "SendEnd"
#define GROUP_BEGIN_EVENT "GroupBegin"
#define PEER_GROUP_EVENT "PeerGroup"
#define PEER_SENDS_EVENT "PeerSends"
#define CS_GROUP_EVENT "CSGroup"
#define CS_SENDS_EVENT "CSSends"

#define MAX_REQUEST_RETRIES 5
#define RECEIVE_TIMEOUT 30000
#define	MAX_SYNC_DATASIZE 512

//#pragma TODO(tristan, "Do ANSI versions need to be present in Xbox header?")
#ifndef DPNA_SEPARATOR_KEYVALUE_A
#define DPNA_SEPARATOR_KEYVALUE_A			'='
#endif

#ifndef DPNA_SEPARATOR_USERDATA_A
#define DPNA_SEPARATOR_USERDATA_A			'#'
#endif

#ifndef DPNA_SEPARATOR_COMPONENT_A
#define DPNA_SEPARATOR_COMPONENT_A			';'
#endif

#ifndef DPNA_ESCAPECHAR_A
#define DPNA_ESCAPECHAR_A					'%'
#endif

#ifndef DPNA_HEADER_A
#define DPNA_HEADER_A						"x-directplay:/"
#endif

#ifndef DPNA_KEY_APPLICATION_INSTANCE_A
#define DPNA_KEY_APPLICATION_INSTANCE_A		"applicationinstance"
#endif

#ifndef DPNA_KEY_BAUD_A
#define DPNA_KEY_BAUD_A						"baud"
#endif

#ifndef DPNA_KEY_DEVICE_A
#define DPNA_KEY_DEVICE_A					"device"
#endif

#ifndef DPNA_KEY_FLOWCONTROL_A
#define DPNA_KEY_FLOWCONTROL_A				"flowcontrol"
#endif

#ifndef DPNA_KEY_HOSTNAME_A
#define DPNA_KEY_HOSTNAME_A					"hostname"
#endif

#ifndef DPNA_KEY_PARITY_A
#define DPNA_KEY_PARITY_A					"parity"
#endif

#ifndef DPNA_KEY_PHONENUMBER_A
#define DPNA_KEY_PHONENUMBER_A				"phonenumber"
#endif

#ifndef DPNA_KEY_PORT_A
#define DPNA_KEY_PORT_A						"port"
#endif

#ifndef DPNA_KEY_PROGRAM_A
#define DPNA_KEY_PROGRAM_A					"program"
#endif

#ifndef DPNA_KEY_PROVIDER_A
#define DPNA_KEY_PROVIDER_A					"provider"
#endif

#ifndef DPNA_KEY_STOPBITS_A
#define DPNA_KEY_STOPBITS_A					"stopbits"
#endif

#ifndef DPNA_VALUE_TCPIPPROVIDER_A
#define DPNA_VALUE_TCPIPPROVIDER_A			"IP"
#endif

//==================================================================================
// Typedefs
//==================================================================================

// DP test case IDs
typedef enum _DPMSG_LOCAL_TESTTYPES
{
	DPMSG_LOCAL_BASE = 0,
	DPMSG_ADDR_BUILDW = 0,
	DPMSG_ADDR_BUILDA,
	DPMSG_ADDR_DUPLICATE,
	DPMSG_ADDR_SETEQUAL,
	DPMSG_ADDR_ISEQUAL,
	DPMSG_ADDR_CLEAR,
	DPMSG_ADDR_GETURLW,
	DPMSG_ADDR_GETURLA,
	DPMSG_ADDR_GETSP,
	DPMSG_ADDR_GETUSERDATA,
	DPMSG_ADDR_SETSP,
	DPMSG_ADDR_SETUSERDATA,
	DPMSG_ADDR_GETNUMCOMPONENTS,
	DPMSG_ADDR_GETBYNAME,
	DPMSG_ADDR_GETBYINDEX,
	DPMSG_ADDR_ADD,
	DPMSG_ADDR_GETDEVICE,
	DPMSG_ADDR_SETDEVICE,
		
	DPMSG_ADDRIP_BUILDSOCKADDR,
	DPMSG_ADDRIP_GETSOCKADDR,
	DPMSG_ADDRIP_BUILDADDRESS,
	DPMSG_ADDRIP_GETADDRESS,
	DPMSG_ADDRIP_BUILDLOCALADDRESS,
	DPMSG_ADDRIP_GETLOCALADDRESS,
		
	DPMSG_CLIENT_GETSENDQINFO,
	DPMSG_CLIENT_SETCLIENTINFO,
	DPMSG_CLIENT_GETSERVERINFO,
	DPMSG_CLIENT_GETSERVERADDRESS,
	DPMSG_CLIENT_GETCONNINFO,
		
	DPMSG_PEER_CREATEQI,
	DPMSG_PEER_INIT,
	DPMSG_PEER_ENUMSPS,
	DPMSG_PEER_CANCEL,
	DPMSG_PEER_CONNECT,
	DPMSG_PEER_SENDTO,
	DPMSG_PEER_GETSENDQINFO,
	DPMSG_PEER_HOST,
	DPMSG_PEER_GETAPPDESC,
	DPMSG_PEER_SETAPPDESC,
	DPMSG_PEER_CREATEGROUP,
	DPMSG_PEER_DESTROYGROUP,
	DPMSG_PEER_ADDTOGROUP,
	DPMSG_PEER_REMOVEFROMGROUP,
	DPMSG_PEER_SETGROUPINFO,
	DPMSG_PEER_GETGROUPINFO,
	DPMSG_PEER_ENUMPANDG,
	DPMSG_PEER_ENUMGROUPMEMBERS,
	DPMSG_PEER_SETPEERINFO,
	DPMSG_PEER_GETPEERINFO,
	DPMSG_PEER_GETPEERADDRESS,
	DPMSG_PEER_GETLHOSTADDRESSES,
	DPMSG_PEER_CLOSE,
	DPMSG_PEER_ENUMHOSTS,
	DPMSG_PEER_DESTROYPEER,
	DPMSG_PEER_RETURN,
	DPMSG_PEER_GETPLAYERCONTEXT,
	DPMSG_PEER_GETGROUPCONTEXT,
	DPMSG_PEER_GETCAPS,
	DPMSG_PEER_SETCAPS,
	DPMSG_PEER_SETSPCAPS,
	DPMSG_PEER_GETSPCAPS,
	DPMSG_PEER_GETCONNINFO,
	DPMSG_PEER_TERMINATE,
	
	DPMSG_SERVER_CREATEQI,
	DPMSG_SERVER_INIT,
	DPMSG_SERVER_ENUMSPS,
	DPMSG_SERVER_CANCEL,
	DPMSG_SERVER_GETSENDQINFO,
	DPMSG_SERVER_GETAPPDESC,
	DPMSG_SERVER_SETSERVERINFO,
	DPMSG_SERVER_GETCLIENTINFO,
	DPMSG_SERVER_GETCLIENTADDRESS,
	
	DPMSG_LOCAL_MAXTEST
} DPMSG_LOCAL_TESTTYPES, *PDPMSG_LOCAL_TESTTYPES;

// DP test function
typedef HRESULT (*PDP_LOCAL_TEST_FUNCTION)(HANDLE hLog);

typedef struct _BASESESSIONCREATECONTEXT
{
	BOOL		fPlayersHaveLeft;
	HANDLE		hbsccAllPlayersCreated;
	DWORD		dwPlayersExpected;
	DWORD		dwPlayersCreated;
} BASESESSIONCREATECONTEXT, *PBASESESSIONCREATECONTEXT;

typedef struct _DP_ENUMTESTPARAM {
	HANDLE hNetsyncObject;
	CHAR szServerIPAddr[16];
	DWORD dwAppDescDataSize;
	DWORD dwUserEnumDataSize;
	DWORD dwEnumCount;
	DWORD dwRetryInterval;
	DWORD dwTimeOut;
	BOOL fClientServer;
	BOOL fHostTest;
	BOOL fBroadcast;
	BOOL fReject;
} DP_ENUMTESTPARAM, *PDP_ENUMTESTPARAM;

typedef struct _DP_SENDTESTPARAM {
	DWORD dwHostAddr; // DWORD value of host's IP address
	DWORD dwFlags;  // Flags to provide to send... possibly DPNSEND_SYNC
	DWORD dwTimeout;  // Milliseconds to wait before timing out the send operation
	DWORD dwCancelInterval; // How many sends to do before trying a cancel.  Can be 0 for no cancels
	DWORD dwNumMsgs; // Number of sends to perform
	BOOL fHostTest; // Is this the host/server?
	DWORD dwNumMachines; // Total number of players we're expecting
	DWORD dwMsgSize; // Size of message to send
	DWORD dwPercentEatCPU; // If implemented, will set the amount of background CPU utilization
	HANDLE hNetsyncObject; // Handle to netsync
	DWORD dwInterval; // milliseconds to sleep between sends
	BOOL fSendToAllPlayersGroup; // Broadcast to all vs. send to individuals
	DWORD dwCancelFlags; // Flags to provide to cancel... like _SENDS
} DP_SENDTESTPARAM, *PDP_SENDTESTPARAM;


typedef struct _DP_GROUPTESTPARAM {
	DWORD dwHostAddr; // DWORD value of host's IP address
	BOOL fHostTest; // Is this the host/server?
	DWORD dwNumMachines; // Total number of players we're expecting
	HANDLE hNetsyncObject; // Handle to netsync
} DP_GROUPTESTPARAM, *PDP_GROUPTESTPARAM;

// This is cumbersome, but its needed in order to get an idea of how much time
// is being spent inside the DoWork code.  It's also needed so that the polling function
// can know which objects it needs to call DoWork on to complete a particular event
typedef struct _DP_DOWORKLIST {
	BOOL					fTrackDoWorkTimes;			// Boolean indicates if DoWork times should be tracked
														// If true, then CallbackTimes members should be present
														// for all objects
														// If false, then CallbackTimes members will be ignored

	DWORD					dwNumPeers;					// Number of peer objects present
	PDIRECTPLAY8PEER		*apDP8Peers;				// An array of pointers to peer objects
	DWORD					**apdwPeerCallbackTimes;	// An array of pointers to DWORDS that track the
														// amount of time spent in callbacks per peer

	DWORD					dwNumClients;				// Number of client objects present
	PDIRECTPLAY8CLIENT		*apDP8Clients;				// An array of pointers to client objects
	DWORD					**apdwClientCallbackTimes;	// An array of pointers to DWORDS that track the
														// amount of time spent in callbacks per client

	DWORD					dwNumServers;				// Number of client objects present
	PDIRECTPLAY8SERVER		*apDP8Servers;				// An array of pointers to server objects
	DWORD					**apdwServerCallbackTimes;	// An array of pointers to DWORDS that track the
														// amount of time spent in callbacks per server
} DP_DOWORKLIST, *PDP_DOWORKLIST;

//==================================================================================
// Prototypes 
//==================================================================================
// Harness functions
BOOL WINAPI DpTestDllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext);
VOID WINAPI DpTestStartTest(IN HANDLE hLog);
VOID WINAPI DpTestEndTest();

// Helper functions
LPVOID MemAlloc(DWORD dwSize);
BOOL MemFree(LPVOID pBuffer);
void DPTEST_TRACE(HANDLE hLog, LPSTR szFormat, INT nNumArgs ...);
void DPTEST_FAIL(HANDLE hLog, LPSTR szFormat, INT nNumArgs ...);
BOOL SyncWithOtherTesters(HANDLE, HANDLE, LPCSTR, PBYTE, DWORD, PBYTE, DWORD *);
BOOL CreateHostAndWaitForPlayers(HANDLE, PWRAPDP8PEER *, PBASESESSIONCREATECONTEXT, DPN_APPLICATION_DESC *, PFNDPNMESSAGEHANDLER, PDPNID *, DWORD, DWORD);
BOOL ConnectNonHostAndWaitForPlayers(HANDLE, PWRAPDP8PEER *, PBASESESSIONCREATECONTEXT, DWORD, DPN_APPLICATION_DESC *, PFNDPNMESSAGEHANDLER, PDPNID *, DWORD, DWORD);
BOOL CreateServerAndWaitForPlayers(HANDLE, PWRAPDP8SERVER *, PBASESESSIONCREATECONTEXT, DPN_APPLICATION_DESC *, PFNDPNMESSAGEHANDLER, PDPNID *, DWORD, DWORD);
BOOL ConnectClient(HANDLE, PWRAPDP8CLIENT *, PBASESESSIONCREATECONTEXT, DWORD, DPN_APPLICATION_DESC *, PFNDPNMESSAGEHANDLER, DWORD);
BOOL PollUntilEventSignalled(HANDLE hLog, PDP_DOWORKLIST pDoWorkList, DWORD dwMaxTime, HANDLE hEvent);

// DirectPlay8Address tests
HRESULT ParmVExec_BBuildW(HANDLE hLog);
HRESULT ParmVExec_BBuildA(HANDLE hLog);
HRESULT ParmVExec_BDuplicate(HANDLE hLog);
HRESULT ParmVExec_BSetEqual(HANDLE hLog);
HRESULT ParmVExec_BIsEqual(HANDLE hLog);
HRESULT ParmVExec_BClear(HANDLE hLog);
HRESULT ParmVExec_BGetURLW(HANDLE hLog);
HRESULT ParmVExec_BGetURLA(HANDLE hLog);
HRESULT ParmVExec_BGetSP(HANDLE hLog);
HRESULT ParmVExec_BGetUserData(HANDLE hLog);
HRESULT ParmVExec_BSetSP(HANDLE hLog);
HRESULT ParmVExec_BSetUserData(HANDLE hLog);
HRESULT ParmVExec_BGetNumComponents(HANDLE hLog);
HRESULT ParmVExec_BGetByName(HANDLE hLog);
HRESULT ParmVExec_BGetByIndex(HANDLE hLog);
HRESULT ParmVExec_BAdd(HANDLE hLog);
HRESULT ParmVExec_BGetDevice(HANDLE hLog);
HRESULT ParmVExec_BSetDevice(HANDLE hLog);

// DirectPlay8AddressIP tests
HRESULT ParmVExec_IPBuildSockAddr(HANDLE hLog);
HRESULT ParmVExec_IPGetSockAddr(HANDLE hLog);
HRESULT ParmVExec_IPBuildAddress(HANDLE hLog);
HRESULT ParmVExec_IPGetAddress(HANDLE hLog);
HRESULT ParmVExec_IPBuildLocalAddress(HANDLE hLog);
HRESULT ParmVExec_IPGetLocalAddress(HANDLE hLog);

// DirectPlay8Client tests
HRESULT ParmVClientExec_GetSendQInfo(HANDLE hLog);
HRESULT ParmVClientExec_SetClientInfo(HANDLE hLog);
HRESULT ParmVClientExec_GetServerInfo(HANDLE hLog);
HRESULT ParmVClientExec_GetServerAddress(HANDLE hLog);
HRESULT ParmVClientExec_GetConnInfo(HANDLE hLog);

// DirectPlay8Peer tests
HRESULT ParmVPeerExec_CreateQI(HANDLE hLog);
HRESULT ParmVPeerExec_Init(HANDLE hLog);
HRESULT ParmVPeerExec_EnumSPs(HANDLE hLog);
HRESULT ParmVPeerExec_Cancel(HANDLE hLog);
HRESULT ParmVPeerExec_Connect(HANDLE hLog);
HRESULT ParmVPeerExec_SendTo(HANDLE hLog);
HRESULT ParmVPeerExec_GetSendQInfo(HANDLE hLog);
HRESULT ParmVPeerExec_Host(HANDLE hLog);
HRESULT ParmVPeerExec_GetAppDesc(HANDLE hLog);
HRESULT ParmVPeerExec_SetAppDesc(HANDLE hLog);
HRESULT ParmVPeerExec_CreateGroup(HANDLE hLog);
HRESULT ParmVPeerExec_DestroyGroup(HANDLE hLog);
HRESULT ParmVPeerExec_AddToGroup(HANDLE hLog);
HRESULT ParmVPeerExec_RemoveFromGroup(HANDLE hLog);
HRESULT ParmVPeerExec_SetGroupInfo(HANDLE hLog);
HRESULT ParmVPeerExec_GetGroupInfo(HANDLE hLog);
HRESULT ParmVPeerExec_EnumPAndG(HANDLE hLog);
HRESULT ParmVPeerExec_EnumGroupMembers(HANDLE hLog);
HRESULT ParmVPeerExec_SetPeerInfo(HANDLE hLog);
HRESULT ParmVPeerExec_GetPeerInfo(HANDLE hLog);
HRESULT ParmVPeerExec_GetPeerAddress(HANDLE hLog);
HRESULT ParmVPeerExec_GetLHostAddresses(HANDLE hLog);
HRESULT ParmVPeerExec_Close(HANDLE hLog);
HRESULT ParmVPeerExec_EnumHosts(HANDLE hLog);
HRESULT ParmVPeerExec_DestroyPeer(HANDLE hLog);
HRESULT ParmVPeerExec_Return(HANDLE hLog);
HRESULT ParmVPeerExec_GetPlayerContext(HANDLE hLog);
HRESULT ParmVPeerExec_GetGroupContext(HANDLE hLog);
HRESULT ParmVPeerExec_GetCaps(HANDLE hLog);
HRESULT ParmVPeerExec_SetCaps(HANDLE hLog);
HRESULT ParmVPeerExec_SetSPCaps(HANDLE hLog);
HRESULT ParmVPeerExec_GetSPCaps(HANDLE hLog);
HRESULT ParmVPeerExec_GetConnInfo(HANDLE hLog);
HRESULT ParmVPeerExec_Terminate(HANDLE hLog);

// DirectPlay8Server tests
HRESULT ParmVServerExec_CreateQI(HANDLE hLog);
HRESULT ParmVServerExec_Init(HANDLE hLog);
HRESULT ParmVServerExec_EnumSPs(HANDLE hLog);
HRESULT ParmVServerExec_Cancel(HANDLE hLog);
HRESULT ParmVServerExec_GetSendQInfo(HANDLE hLog);
HRESULT ParmVServerExec_GetAppDesc(HANDLE hLog);
HRESULT ParmVServerExec_SetServerInfo(HANDLE hLog);
HRESULT ParmVServerExec_GetClientInfo(HANDLE hLog);
HRESULT ParmVServerExec_GetClientAddress(HANDLE hLog);


HRESULT DirectPlayExec_SimpleConnect(HANDLE hLog, PDP_HOSTINFO pHostInfo, WORD LowPort);
HRESULT DirectPlayExec_SimpleHost(HANDLE hLog, PDP_HOSTINFO pHostInfo, WORD LowPort);
HRESULT DirectPlayExec_SimpleClient(HANDLE hLog, PDP_HOSTINFO pHostInfo, WORD LowPort);
HRESULT DirectPlayExec_SimpleServer(HANDLE hLog, PDP_HOSTINFO pHostInfo, WORD LowPort);

HRESULT GroupsExec_SimplePeer(HANDLE hLog, PDP_GROUPTESTPARAM pGroupTestParam);
HRESULT GroupsExec_SimpleCS(HANDLE hLog, PDP_GROUPTESTPARAM pGroupTestParam);
HRESULT SendsExec_SimplePeer(HANDLE hLog, PDP_SENDTESTPARAM pSendTestParam);
HRESULT SendsExec_SimpleCS(HANDLE hLog, PDP_SENDTESTPARAM pSendTestParam);
//HRESULT SendsExec_PendingPeer(HANDLE hLog, PDP_SENDTESTPARAM pSendTestParam);
//HRESULT SendsExec_PendingCS(HANDLE hLog, PDP_SENDTESTPARAM pSendTestParam);

//==================================================================================
// Helper classes 
//==================================================================================
class CTNSystemResult
{
	private:
		HRESULT		m_hr;


	public:
		CTNSystemResult(void):
		  m_hr(S_OK)
		{
		};

		CTNSystemResult(HRESULT hr):
		  m_hr(hr)
		{
		};

		~CTNSystemResult(void)
		{
		};


		inline void operator =(HRESULT hr)
		{
			this->m_hr = hr;
		};

		inline BOOL operator ==(HRESULT hr)
		{
			if (this->m_hr == hr)
				return (TRUE);
			return (FALSE);
		};

		inline BOOL operator !=(HRESULT hr)
		{
			if (this->m_hr != hr)
				return (TRUE);
			return (FALSE);
		};

		inline operator HRESULT(void)
		{
			return (this->m_hr);
		};
};

class CTNTestResult
{
	private:
		HRESULT		m_hr;


	public:
		CTNTestResult(void):
		  m_hr(S_OK)
		{
		};

		CTNTestResult(HRESULT hr):
		  m_hr(hr)
		{
		};

		~CTNTestResult(void)
		{
		};


		inline void operator =(HRESULT hr)
		{
			this->m_hr = hr;
		};

		inline BOOL operator ==(HRESULT hr)
		{
			if (this->m_hr == hr)
				return (TRUE);
			return (FALSE);
		};

		inline BOOL operator !=(HRESULT hr)
		{
			if (this->m_hr != hr)
				return (TRUE);
			return (FALSE);
		};

		inline operator HRESULT(void)
		{
			return (this->m_hr);
		};
};

} // namespace DPlayCoreNamespace

#endif // __DPTEST_H__
