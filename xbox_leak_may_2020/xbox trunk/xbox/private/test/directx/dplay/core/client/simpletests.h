#ifndef __SIMPLETESTS__
#define __SIMPLETESTS__

namespace DPlayCoreNamespace {

#define CONNECT_DATA			0
#define CONNECT_REPLY_DATA		1
#define SEND_DATA				2
#define PLAYER_DATA				3
#define APP_DATA				4
#define TERMINATE_DATA			5

#define MAX_MESSAGEDATA_DATA	32

#define SEND_COUNT				10

#define SEND_MESSAGE "Send message"
#define CONNECT_MESSAGE "Connect message"
#define CONNECT_REPLY_MESSAGE "Connect reply message"
#define DESTROY_MESSAGE "Destroy message"
#define TERMINATE_MESSAGE "Terminate message"
#define SERVER_DATA1 "First server data buffer"
#define SERVER_DATA2 "Second server data buffer"
#define CLIENT_DATA1 "First client data buffer"
#define CLIENT_DATA2 "Second client data buffer"
#define PEER_DATA1 "First peer data buffer"
#define PEER_DATA2 "Second peer data buffer"
#define HOST_DATA1 "First host data buffer"
#define HOST_DATA2 "Second host data buffer"

#define SERVER_PLAYER_NAME1 L"Server1"
#define SERVER_PLAYER_NAME2	L"Server2"
#define CLIENT_PLAYER_NAME1 L"Client1"
#define CLIENT_PLAYER_NAME2	L"Client2"
#define HOST_PLAYER_NAME1 L"Host1"
#define HOST_PLAYER_NAME2 L"Host2"
#define PEER_PLAYER_NAME1 L"Peer1"
#define PEER_PLAYER_NAME2 L"Peer2"

#define SERVER_SESSION_NAME1 L"ServerSession1"
#define SERVER_SESSION_NAME2 L"ServerSession2"
#define HOST_SESSION_NAME1 L"HostSession1"
#define HOST_SESSION_NAME2 L"HostSession2"

#define SERVER_PASSWORD1 L"ServerPassword1"
#define SERVER_PASSWORD2 L"ServerPassword2"
#define HOST_PASSWORD1 L"HostPassword1"
#define HOST_PASSWORD2 L"HostPassword2"

#define SERVER_APP_DATA1 "ServerAppData1"
#define SERVER_APP_DATA2 "ServerAppData2"
#define HOST_APP_DATA1 "HostAppData1"
#define HOST_APP_DATA2 "HostAppData2"

//==================================================================================
// Structure
//==================================================================================
typedef struct tagPLAYERCONTEXT
{
	BOOL					fLocalPlayer;
	DPNID					PlayerID;
	DWORD					TotalMessagesReceived;
	DWORD					TotalBytesReceived;
	DWORD					dwExpectedDestroyReason;
	struct tagPLAYERCONTEXT	*pPrev;
	struct tagPLAYERCONTEXT	*pNext;
} PLAYERCONTEXT, * PPLAYERCONTEXT;

typedef struct tagSIMPLEHOSTCONTEXT
{
	BOOL					fVerbose;
	CRITICAL_SECTION		Lock;
	PPLAYERCONTEXT			pPlayerList;
	HANDLE					hLog; // Handle to logging subsystem
	DPNID					LocalPlayerID;
	DWORD					dwCallbackTime;

	// Variables for handling incoming connections
	HANDLE					hIndicateConnect;
	BOOL					fCanAcceptConnect;
	BOOL					fAcceptedConnect;

	// Variables for handling player creation events
	HANDLE					hIndicateRemotePlayer;
	DWORD					dwExpectedRemotePlayers;
	DWORD					dwCurrentRemotePlayers;

	// Variables for handling incoming data
	HANDLE					hReceiveData;
	DWORD					dwExpectedReceives;
	DWORD					dwCurrentReceives;

	// Variables for handling random async operations
	HANDLE					hAsyncOpCompleted;
	BOOL					fAsyncOpCanComplete;
	BOOL					fAsyncOpCompleted;
	DPNHANDLE				dpnhAsyncOp;
	HRESULT					hrAsyncOpExpectedResult;

	// Variables for handling peer information changes
	HANDLE					hPeerInfoChanged;
	BOOL					fPeerInfoCanChange;
	BOOL					fPeerInfoChanged;

	// Variables for handling app desc changes
	HANDLE					hAppDescChanged;
	BOOL					fAppDescCanChange;
	BOOL					fAppDescChanged;

	DWORD					dwPlayersConnected;
} SIMPLEHOSTCONTEXT, * PSIMPLEHOSTCONTEXT;

typedef struct tagSIMPLEPEERCONTEXT
{
	// BUGBUG - make sure to add to other test's contexts later
	CHAR					szContextDescription[20];

	BOOL					fVerbose;
	CRITICAL_SECTION		Lock;
	PPLAYERCONTEXT			pPlayerList;
	HANDLE					hLog; // Handle to logging subsystem
	DPNID					LocalPlayerID;
	DWORD					dwCallbackTime;

	// Variables for handling incoming connections
	HANDLE					hConnectComplete;
	BOOL					fConnectCanComplete;
	BOOL					fConnectCompleted;
	DPNHANDLE				dpnhConnect;
	HRESULT					hrExpectedConnectResult;

	// Variables for handling player creation events
	HANDLE					hIndicateRemotePlayer;
	DWORD					dwExpectedRemotePlayers;
	DWORD					dwCurrentRemotePlayers;

	// Variables for handling incoming data
	HANDLE					hReceiveData;
	DWORD					dwExpectedReceives;
	DWORD					dwCurrentReceives;

	// Variables for handling random async operations
	HANDLE					hAsyncOpCompleted;
	BOOL					fAsyncOpCanComplete;
	BOOL					fAsyncOpCompleted;
	DPNHANDLE				dpnhAsyncOp;
	HRESULT					hrAsyncOpExpectedResult;

	// Variables for handling host information changes
	HANDLE					hHostInfoChanged;
	BOOL					fHostInfoCanChange;
	BOOL					fHostInfoChanged;

	// Variables for handling app desc changes
	HANDLE					hAppDescChanged;
	BOOL					fAppDescCanChange;
	BOOL					fAppDescChanged;

	// Variables for handling session termination
	HANDLE					hSessionTerminated;
	BOOL					fSessionCanTerminate;
	BOOL					fSessionTerminated;

	DWORD					dwPlayersConnected;
} SIMPLEPEERCONTEXT, * PSIMPLEPEERCONTEXT;

typedef struct tagSIMPLESERVERCONTEXT
{
	BOOL					fVerbose;
	CRITICAL_SECTION		Lock;
	PPLAYERCONTEXT			pPlayerList;
	HANDLE					hLog; // Handle to logging subsystem
	DPNID					LocalPlayerID;
	DWORD					dwCallbackTime;

	// Variables for handling incoming connections
	HANDLE					hIndicateConnect;
	BOOL					fCanAcceptConnect;
	BOOL					fAcceptedConnect;

	// Variables for handling player creation events
	HANDLE					hIndicateRemotePlayer;
	BOOL					fCanAcceptRemotePlayer;
	BOOL					fAcceptedRemotePlayer;

	// Variables for handling incoming data
	HANDLE					hReceiveData;
	BOOL					fCanReceive;
	BOOL					fReceived;

	// Variables for handling random async operations
	HANDLE					hAsyncOpCompleted;
	BOOL					fAsyncOpCanComplete;
	BOOL					fAsyncOpCompleted;
	DPNHANDLE				dpnhAsyncOp;
	HRESULT					hrAsyncOpExpectedResult;

	// Variables for handling client information changes
	HANDLE					hClientInfoChanged;
	BOOL					fClientInfoCanChange;
	BOOL					fClientInfoChanged;

	// Variables for handling app desc changes
	HANDLE					hAppDescChanged;
	BOOL					fAppDescCanChange;
	BOOL					fAppDescChanged;

	DWORD					dwPlayersConnected;
} SIMPLESERVERCONTEXT, * PSIMPLESERVERCONTEXT;

typedef struct tagSIMPLECLIENTCONTEXT
{
	BOOL					fVerbose;
	CRITICAL_SECTION		Lock;
	PPLAYERCONTEXT			pPlayerList;
	HANDLE					hLog; // Handle to logging subsystem
	DPNID					LocalPlayerID;
	DWORD					dwCallbackTime;

	// Variables for handling incoming connections
	HANDLE					hConnectComplete;
	BOOL					fConnectCanComplete;
	BOOL					fConnectCompleted;
	DPNHANDLE				dpnhConnect;
	HRESULT					hrExpectedConnectResult;

	// Variables for handling player creation events
	HANDLE					hIndicateRemotePlayer;
	BOOL					fCanAcceptRemotePlayer;
	BOOL					fAcceptedRemotePlayer;

	// Variables for handling incoming data
	HANDLE					hReceiveData;
	BOOL					fCanReceive;
	BOOL					fReceived;

	// Variables for handling random async operations
	HANDLE					hAsyncOpCompleted;
	BOOL					fAsyncOpCanComplete;
	BOOL					fAsyncOpCompleted;
	DPNHANDLE				dpnhAsyncOp;
	HRESULT					hrAsyncOpExpectedResult;

	// Variables for handling server information changes
	HANDLE					hServerInfoChanged;
	BOOL					fServerInfoCanChange;
	BOOL					fServerInfoChanged;

	// Variables for handling app desc changes
	HANDLE					hAppDescChanged;
	BOOL					fAppDescCanChange;
	BOOL					fAppDescChanged;

	// Variables for handling session termination
	HANDLE					hSessionTerminated;
	BOOL					fSessionCanTerminate;
	BOOL					fSessionTerminated;

	DWORD					dwPlayersConnected;
} SIMPLECLIENTCONTEXT, * PSIMPLECLIENTCONTEXT;

typedef struct tagMESSAGEDATA
{
	DWORD					dwMessageType;
	DWORD					dwChecksum;
	CHAR					szData[MAX_MESSAGEDATA_DATA];					
} MESSAGEDATA, * PMESSAGEDATA;

//==================================================================================
// Callbacks
//==================================================================================
HRESULT SimpleHostMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT SimplePeerMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT SimpleServerMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);
HRESULT SimpleClientMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg);

} // namespace DPlayCoreNamespace

#endif // __SIMPLETESTS__