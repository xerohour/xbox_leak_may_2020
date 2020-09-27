/*==========================================================================;
 *
 *  Copyright (C) 1998-2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       DPlay8.h
 *  Content:    DirectPlay8 include file
 *
 ***************************************************************************/

#ifndef __DIRECTPLAY8_H__
#define __DIRECTPLAY8_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <dpaddr.h>

/****************************************************************************
 *
 * DirectPlay8 Callback Functions
 *
 ****************************************************************************/

//
// Callback Function Type Definition
//
typedef HRESULT (WINAPI *PFNDPNMESSAGEHANDLER)(PVOID,DWORD,PVOID);

/****************************************************************************
 *
 * DirectPlay8 Datatypes (Non-Structure / Non-Message)
 *
 ****************************************************************************/

//
// Player IDs.  Used to uniquely identify a player in a session
//
typedef DWORD   DPNID,      *PDPNID;

//
// Used as identifiers for operations
//
typedef DWORD   DPNHANDLE, *PDPNHANDLE;

/****************************************************************************
 *
 * DirectPlay8 Message Identifiers
 *
 ****************************************************************************/

#define DPN_MSGID_OFFSET                    0xFFFF0000
#define DPN_MSGID_ADD_PLAYER_TO_GROUP       ( DPN_MSGID_OFFSET | 0x0001 )
#define DPN_MSGID_APPLICATION_DESC          ( DPN_MSGID_OFFSET | 0x0002 )
#define DPN_MSGID_ASYNC_OP_COMPLETE         ( DPN_MSGID_OFFSET | 0x0003 )
#define DPN_MSGID_CLIENT_INFO               ( DPN_MSGID_OFFSET | 0x0004 )
#define DPN_MSGID_CONNECT_COMPLETE          ( DPN_MSGID_OFFSET | 0x0005 )
#define DPN_MSGID_CREATE_GROUP              ( DPN_MSGID_OFFSET | 0x0006 )
#define DPN_MSGID_CREATE_PLAYER             ( DPN_MSGID_OFFSET | 0x0007 )
#define DPN_MSGID_DESTROY_GROUP             ( DPN_MSGID_OFFSET | 0x0008 )
#define DPN_MSGID_DESTROY_PLAYER            ( DPN_MSGID_OFFSET | 0x0009 )
#define DPN_MSGID_ENUM_HOSTS_QUERY          ( DPN_MSGID_OFFSET | 0x000a )
#define DPN_MSGID_ENUM_HOSTS_RESPONSE       ( DPN_MSGID_OFFSET | 0x000b )
#define DPN_MSGID_GROUP_INFO                ( DPN_MSGID_OFFSET | 0x000c )
#define DPN_MSGID_HOST_MIGRATE              ( DPN_MSGID_OFFSET | 0x000d )
#define DPN_MSGID_INDICATE_CONNECT          ( DPN_MSGID_OFFSET | 0x000e )
#define DPN_MSGID_INDICATED_CONNECT_ABORTED ( DPN_MSGID_OFFSET | 0x000f )
#define DPN_MSGID_PEER_INFO                 ( DPN_MSGID_OFFSET | 0x0010 )
#define DPN_MSGID_RECEIVE                   ( DPN_MSGID_OFFSET | 0x0011 )
#define DPN_MSGID_REMOVE_PLAYER_FROM_GROUP  ( DPN_MSGID_OFFSET | 0x0012 )
#define DPN_MSGID_RETURN_BUFFER             ( DPN_MSGID_OFFSET | 0x0013 )
#define DPN_MSGID_SEND_COMPLETE             ( DPN_MSGID_OFFSET | 0x0014 )
#define DPN_MSGID_SERVER_INFO               ( DPN_MSGID_OFFSET | 0x0015 )
#define DPN_MSGID_TERMINATE_SESSION         ( DPN_MSGID_OFFSET | 0x0016 )

/****************************************************************************
 *
 * DirectPlay8 Constants
 *
 ****************************************************************************/

#define IID_IDirectPlay8Peer		0
#define IID_IDirectPlay8Client		1
#define IID_IDirectPlay8Server		2


// {EBFE7BA0-628D-11D2-AE0F-006097B01411}
DEFINE_GUID(CLSID_DP8SP_TCPIP, 0xebfe7ba0, 0x628d, 0x11d2, 0xae, 0xf, 0x0, 0x60, 0x97, 0xb0, 0x14, 0x11);

#define DPNID_ALL_PLAYERS_GROUP             0

//
// DESTROY_GROUP reasons
//
#define DPNDESTROYGROUPREASON_NORMAL                0x0001
#define DPNDESTROYGROUPREASON_AUTODESTRUCTED        0x0002
#define DPNDESTROYGROUPREASON_SESSIONTERMINATED     0x0003

//
// DESTROY_PLAYER reasons
//
#define DPNDESTROYPLAYERREASON_NORMAL               0x0001
#define DPNDESTROYPLAYERREASON_CONNECTIONLOST       0x0002
#define DPNDESTROYPLAYERREASON_SESSIONTERMINATED    0x0003
#define DPNDESTROYPLAYERREASON_HOSTDESTROYEDPLAYER  0x0004

/****************************************************************************
 *
 * DirectPlay8 Flags
 *
 ****************************************************************************/

//
// Cancel flags
//
#define DPNCANCEL_CONNECT                   0x0001
#define DPNCANCEL_ENUM                      0x0002
#define DPNCANCEL_SEND                      0x0004
#define DPNCANCEL_ALL_OPERATIONS            0x8000

//
// Enumerate clients and groups flags (For EnumPlayersAndGroups)
//
#define DPNENUM_PLAYERS                     0x0001
#define DPNENUM_GROUPS                      0x0010

//
// Enum hosts flags (For EnumHosts)
//
#define DPNENUMHOSTS_NOBROADCASTFALLBACK    0x0002

//
// Enum service provider flags (For EnumSP)
//
#define DPNENUMSERVICEPROVIDERS_ALL         0x0001

//
// Get send queue info flags (For GetSendQueueInfo)
//
#define DPNGETSENDQUEUEINFO_PRIORITY_NORMAL 0x0001
#define DPNGETSENDQUEUEINFO_PRIORITY_HIGH   0x0002
#define DPNGETSENDQUEUEINFO_PRIORITY_LOW    0x0004

//
// Group information flags (For Group Info)
//
#define DPNGROUP_AUTODESTRUCT               0x0001

//
// Host flags (For Host)
//

//
// Set info
//
#define DPNINFO_NAME                        0x0001
#define DPNINFO_DATA                        0x0002

//
// Register Lobby flags
//
#define DPNLOBBY_REGISTER                   0x0001
#define DPNLOBBY_UNREGISTER                 0x0002

//
// Player information flags (For Player Info / Player Messages)
//
#define DPNPLAYER_LOCAL                     0x0002
#define DPNPLAYER_HOST                      0x0004

//
// Send flags (For Send/SendTo)
//
#define DPNSEND_NOCOPY                      0x0001
#define DPNSEND_NOCOMPLETE                  0x0002
#define DPNSEND_COMPLETEONPROCESS           0x0004
#define DPNSEND_GUARANTEED                  0x0008
#define DPNSEND_NONSEQUENTIAL               0x0010
#define DPNSEND_NOLOOPBACK                  0x0020
#define DPNSEND_PRIORITY_LOW                0x0040
#define DPNSEND_PRIORITY_HIGH               0x0080

//
// Session Flags (for DPN_APPLICATION_DESC)
//
#define DPNSESSION_CLIENT_SERVER            0x0001
#define DPNSESSION_MIGRATE_HOST             0x0004
#define DPNSESSION_REQUIREPASSWORD          0x0080

//
// SP capabilities flags
//
#define DPNSPCAPS_SUPPORTSDPNSRV            0x0001
#define DPNSPCAPS_SUPPORTSBROADCAST         0x0002
#define DPNSPCAPS_SUPPORTSALLADAPTERS       0x0004

/****************************************************************************
 *
 * DirectPlay8 Structures (Non-Message)
 *
 ****************************************************************************/

//
// Application description
//
typedef struct  _DPN_APPLICATION_DESC
{
    DWORD   dwSize;                         // Size of this structure
    DWORD   dwFlags;                        // Flags (DPNSESSION_...)
    GUID    guidInstance;                   // Instance GUID
    GUID    guidApplication;                // Application GUID
    DWORD   dwMaxPlayers;                   // Maximum # of players allowed (0=no limit)
    DWORD   dwCurrentPlayers;               // Current # of players allowed
    WCHAR   *pwszSessionName;               // Name of the session
    WCHAR   *pwszPassword;                  // Password for the session
    PVOID   pvReservedData;
    DWORD   dwReservedDataSize;
    PVOID   pvApplicationReservedData;
    DWORD   dwApplicationReservedDataSize;
} DPN_APPLICATION_DESC, *PDPN_APPLICATION_DESC;

//
// Generic Buffer Description
//
typedef struct  _BUFFERDESC
{
    DWORD               dwBufferSize;
    BYTE * UNALIGNED    pBufferData;
} BUFFERDESC, DPN_BUFFER_DESC, *PDPN_BUFFER_DESC;

typedef BUFFERDESC  FAR * PBUFFERDESC;

//
// DirectPlay8 capabilities
//
typedef struct  _DPN_CAPS
{
    DWORD   dwSize;                         // Size of this structure
    DWORD   dwFlags;                        // Flags
    DWORD   dwConnectTimeout;               // ms before a connect request times out
    DWORD   dwConnectRetries;               // # of times to attempt the connection
    DWORD   dwTimeoutUntilKeepAlive;        // ms of inactivity before a keep alive is sent
} DPN_CAPS, *PDPN_CAPS;

// Connection Statistics information

typedef struct _DPN_CONNECTION_INFO
{
    DWORD   dwSize;
    DWORD   dwRoundTripLatencyMS;
    DWORD   dwThroughputBPS;
    DWORD   dwPeakThroughputBPS;

    DWORD   dwBytesSentGuaranteed;
    DWORD   dwPacketsSentGuaranteed;
    DWORD   dwBytesSentNonGuaranteed;
    DWORD   dwPacketsSentNonGuaranteed;

    DWORD   dwBytesRetried;     // Guaranteed only
    DWORD   dwPacketsRetried;   // Guaranteed only
    DWORD   dwBytesDropped;     // Non Guaranteed only
    DWORD   dwPacketsDropped;   // Non Guaranteed only

    DWORD   dwMessagesTransmittedHighPriority;
    DWORD   dwMessagesTimedOutHighPriority;
    DWORD   dwMessagesTransmittedNormalPriority;
    DWORD   dwMessagesTimedOutNormalPriority;
    DWORD   dwMessagesTransmittedLowPriority;
    DWORD   dwMessagesTimedOutLowPriority;

    DWORD   dwBytesReceivedGuaranteed;
    DWORD   dwPacketsReceivedGuaranteed;
    DWORD   dwBytesReceivedNonGuaranteed;
    DWORD   dwPacketsReceivedNonGuaranteed;
    DWORD   dwMessagesReceived;

} DPN_CONNECTION_INFO, *PDPN_CONNECTION_INFO;

//
// Group information strucutre
//
typedef struct  _DPN_GROUP_INFO
{
    DWORD   dwSize;             // size of this structure
    DWORD   dwInfoFlags;        // information contained
    PWSTR   pwszName;           // Unicode Name
    PVOID   pvData;             // data block
    DWORD   dwDataSize;         // size in BYTES of data block
    DWORD   dwGroupFlags;       // group flags (DPNGROUP_...)
} DPN_GROUP_INFO, *PDPN_GROUP_INFO;

//
// Player information structure
//
typedef struct  _DPN_PLAYER_INFO
{
    DWORD   dwSize;             // size of this structure
    DWORD   dwInfoFlags;        // information contained
    PWSTR   pwszName;           // Unicode Name
    PVOID   pvData;             // data block
    DWORD   dwDataSize;         // size in BYTES of data block
    DWORD   dwPlayerFlags;      // player flags (DPNPLAYER_...)
} DPN_PLAYER_INFO, *PDPN_PLAYER_INFO;

typedef struct _DPN_SECURITY_CREDENTIALS    DPN_SECURITY_CREDENTIALS, *PDPN_SECURITY_CREDENTIALS;
typedef struct _DPN_SECURITY_DESC           DPN_SECURITY_DESC, *PDPN_SECURITY_DESC;

//
// Service provider & adapter enumeration structure
//
typedef struct _DPN_SERVICE_PROVIDER_INFO
{
    DWORD       dwFlags;
    GUID        guid;       // SP Guid
    WCHAR       *pwszName;  // Friendly Name
    PVOID       pvReserved;
    DWORD       dwReserved;
} DPN_SERVICE_PROVIDER_INFO, *PDPN_SERVICE_PROVIDER_INFO;

//
// Service provider caps structure
//
typedef struct _DPN_SP_CAPS
{
    DWORD   dwSize;                         // Size of this structure
    DWORD   dwFlags;                        // Flags ((DPNSPCAPS_...)
    DWORD   dwNumThreads;                   // # of worker threads to use
    DWORD   dwDefaultEnumCount;             // default # of enum requests
    DWORD   dwDefaultEnumRetryInterval;     // default ms between enum requests
    DWORD   dwDefaultEnumTimeout;           // default enum timeout
    DWORD   dwMaxEnumPayloadSize;           // maximum size in bytes for enum payload data
    DWORD   dwBuffersPerThread;             // number of receive buffers per thread
    DWORD   dwSystemBufferSize;             // amount of buffering to do in addition to posted receive buffers
} DPN_SP_CAPS, *PDPN_SP_CAPS;



/****************************************************************************
 *
 * IDirectPlay8 message handler call back structures
 *
 ****************************************************************************/

//
// Add player to group strucutre for message handler
// (DPN_MSGID_ADD_PLAYER_TO_GROUP)
//
typedef struct  _DPNMSG_ADD_PLAYER_TO_GROUP
{
    DWORD   dwSize;             // Size of this structure
    DPNID   dpnidGroup;         // DPNID of group
    PVOID   pvGroupContext;     // Group context value
    DPNID   dpnidPlayer;        // DPNID of added player
    PVOID   pvPlayerContext;    // Player context value
} DPNMSG_ADD_PLAYER_TO_GROUP, *PDPNMSG_ADD_PLAYER_TO_GROUP;

//
// Async operation completion structure for message handler
// (DPN_MSGID_ASYNC_OP_COMPLETE)
//
typedef struct  _DPNMSG_ASYNC_OP_COMPLETE
{
    DWORD       dwSize;         // Size of this structure
    DPNHANDLE   hAsyncOp;       // DirectPlay8 async operation handle
    PVOID       pvUserContext;  // User context supplied
    HRESULT     hResultCode;    // HRESULT of operation
} DPNMSG_ASYNC_OP_COMPLETE, *PDPNMSG_ASYNC_OP_COMPLETE;

//
// Client info structure for message handler
// (DPN_MSGID_CLIENT_INFO)
//
typedef struct  _DPNMSG_CLIENT_INFO
{
    DWORD   dwSize;             // Size of this structure
    DPNID   dpnidClient;        // DPNID of client
    PVOID   pvPlayerContext;    // Player context value
} DPNMSG_CLIENT_INFO, *PDPNMSG_CLIENT_INFO;

//
// Connect complete structure for message handler
// (DPN_MSGID_CONNECT_COMPLETE)
//
typedef struct  _DPNMSG_CONNECT_COMPLETE
{
    DWORD       dwSize;                     // Size of this structure
    DPNHANDLE   hAsyncOp;                   // DirectPlay8 Async operation handle
    PVOID       pvUserContext;              // User context supplied at Connect
    HRESULT     hResultCode;                // HRESULT of connection attempt
    PVOID       pvApplicationReplyData;     // Connection reply data from Host/Server
    DWORD       dwApplicationReplyDataSize; // Size (in bytes) of pvApplicationReplyData
} DPNMSG_CONNECT_COMPLETE, *PDPNMSG_CONNECT_COMPLETE;

//
// Create group structure for message handler
// (DPN_MSGID_CREATE_GROUP)
//
typedef struct  _DPNMSG_CREATE_GROUP
{
    DWORD   dwSize;             // Size of this structure
    DPNID   dpnidGroup;         // DPNID of new group
    DPNID   dpnidOwner;         // Owner of newgroup
    PVOID   pvGroupContext;     // Group context value
} DPNMSG_CREATE_GROUP, *PDPNMSG_CREATE_GROUP;

//
// Create player structure for message handler
// (DPN_MSGID_CREATE_PLAYER)
//
typedef struct  _DPNMSG_CREATE_PLAYER
{
    DWORD   dwSize;             // Size of this structure
    DPNID   dpnidPlayer;        // DPNID of new player
    PVOID   pvPlayerContext;    // Player context value
} DPNMSG_CREATE_PLAYER, *PDPNMSG_CREATE_PLAYER;

//
// Destroy group structure for message handler
// (DPN_MSGID_DESTROY_GROUP)
//
typedef struct  _DPNMSG_DESTROY_GROUP
{
    DWORD   dwSize;             // Size of this structure
    DPNID   dpnidGroup;         // DPNID of destroyed group
    PVOID   pvGroupContext;     // Group context value
    DWORD   dwReason;           // Information only
} DPNMSG_DESTROY_GROUP, *PDPNMSG_DESTROY_GROUP;

//
// Destroy player structure for message handler
// (DPN_MSGID_DESTROY_PLAYER)
//
typedef struct  _DPNMSG_DESTROY_PLAYER
{
    DWORD   dwSize;             // Size of this structure
    DPNID   dpnidPlayer;        // DPNID of leaving player
    PVOID   pvPlayerContext;    // Player context value
    DWORD   dwReason;           // Information only
} DPNMSG_DESTROY_PLAYER, *PDPNMSG_DESTROY_PLAYER;

//
// Enumeration request received structure for message handler
// (DPN_MSGID_ENUM_HOSTS_QUERY)
//
typedef struct  _DPNMSG_ENUM_HOSTS_QUERY
{
    DWORD               dwSize;                 // Size of this structure.
    IDirectPlay8Address *pAddressSender;        // Address of client who sent the request
    IDirectPlay8Address *pAddressDevice;        // Address of device request was received on
    PVOID               pvReceivedData;         // Request data (set on client)
    DWORD               dwReceivedDataSize;     // Request data size (set on client)
    DWORD               dwMaxResponseDataSize;  // Max allowable size of enum response
    PVOID               pvResponseData;         // Optional query repsonse (user set)
    DWORD               dwResponseDataSize;     // Optional query response size (user set)
    PVOID               pvResponseContext;      // Optional query response context (user set)
} DPNMSG_ENUM_HOSTS_QUERY, *PDPNMSG_ENUM_HOSTS_QUERY;

//
// Enumeration response received structure for message handler
// (DPN_MSGID_ENUM_HOSTS_RESPONSE)
//
typedef struct  _DPNMSG_ENUM_HOSTS_RESPONSE
{
    DWORD                       dwSize;                     // Size of this structure
    IDirectPlay8Address         *pAddressSender;            // Address of host who responded
    IDirectPlay8Address         *pAddressDevice;            // Device response was received on
    const DPN_APPLICATION_DESC  *pApplicationDescription;   // Application description for the session
    PVOID                       pvResponseData;             // Optional response data (set on host)
    DWORD                       dwResponseDataSize;         // Optional response data size (set on host)
    PVOID                       pvUserContext;              // Context value supplied for enumeration
    DWORD                       dwRoundTripLatencyMS;       // Round trip latency in MS
} DPNMSG_ENUM_HOSTS_RESPONSE, *PDPNMSG_ENUM_HOSTS_RESPONSE;

//
// Group info structure for message handler
// (DPN_MSGID_GROUP_INFO)
//
typedef struct  _DPNMSG_GROUP_INFO
{
    DWORD   dwSize;                 // Size of this structure
    DPNID   dpnidGroup;             // DPNID of group
    PVOID   pvGroupContext;         // Group context value
} DPNMSG_GROUP_INFO, *PDPNMSG_GROUP_INFO;

//
// Migrate host structure for message handler
// (DPN_MSGID_HOST_MIGRATE)
//
typedef struct  _DPNMSG_HOST_MIGRATE
{
    DWORD   dwSize;                 // Size of this structure
    DPNID   dpnidNewHost;           // DPNID of new Host player
    PVOID   pvPlayerContext;        // Player context value
} DPNMSG_HOST_MIGRATE, *PDPNMSG_HOST_MIGRATE;

//
// Indicate connect structure for message handler
// (DPN_MSGID_INDICATE_CONNECT)
//
typedef struct  _DPNMSG_INDICATE_CONNECT
{
    DWORD       dwSize;                 // Size of this structure
    PVOID       pvUserConnectData;      // Connecting player data
    DWORD       dwUserConnectDataSize;  // Size (in bytes) of pvUserConnectData
    PVOID       pvReplyData;            // Connection reply data
    DWORD       dwReplyDataSize;        // Size (in bytes) of pvReplyData
    PVOID       pvReplyContext;         // Buffer context for pvReplyData
    PVOID       pvPlayerContext;        // Player context preset
    IDirectPlay8Address *pAddressPlayer;// Address of connecting player
    IDirectPlay8Address *pAddressDevice;// Address of device receiving connect attempt
} DPNMSG_INDICATE_CONNECT, *PDPNMSG_INDICATE_CONNECT;

//
// Indicated connect aborted structure for message handler
// (DPN_MSGID_INDICATED_CONNECT_ABORTED)
//
typedef struct  _DPNMSG_INDICATED_CONNECT_ABORTED
{
    DWORD       dwSize;             // Size of this structure
    PVOID       pvPlayerContext;    // Player context preset from DPNMSG_INDICATE_CONNECT
} DPNMSG_INDICATED_CONNECT_ABORTED, *PDPNMSG_INDICATED_CONNECT_ABORTED;

//
// Peer info structure for message handler
// (DPN_MSGID_PEER_INFO)
//
typedef struct  _DPNMSG_PEER_INFO
{
    DWORD   dwSize;                 // Size of this structure
    DPNID   dpnidPeer;              // DPNID of peer
    PVOID   pvPlayerContext;        // Player context value
} DPNMSG_PEER_INFO, *PDPNMSG_PEER_INFO;

//
// Receive structure for message handler
// (DPN_MSGID_RECEIVE)
//
typedef struct  _DPNMSG_RECEIVE
{
    DWORD       dwSize;             // Size of this structure
    DPNID       dpnidSender;        // DPNID of sending player
    PVOID       pvPlayerContext;    // Player context value of sending player
    PBYTE       pReceiveData;       // Received data
    DWORD       dwReceiveDataSize;  // Size (in bytes) of pReceiveData
    DPNHANDLE   hBufferHandle;      // Buffer handle for pReceiveData
} DPNMSG_RECEIVE, *PDPNMSG_RECEIVE;

//
// Remove player from group structure for message handler
// (DPN_MSGID_REMOVE_PLAYER_FROM_GROUP)
//
typedef struct  _DPNMSG_REMOVE_PLAYER_FROM_GROUP
{
    DWORD   dwSize;                 // Size of this structure
    DPNID   dpnidGroup;             // DPNID of group
    PVOID   pvGroupContext;         // Group context value
    DPNID   dpnidPlayer;            // DPNID of deleted player
    PVOID   pvPlayerContext;        // Player context value
} DPNMSG_REMOVE_PLAYER_FROM_GROUP, *PDPNMSG_REMOVE_PLAYER_FROM_GROUP;

//
// Returned buffer structure for message handler
// (DPN_MSGID_RETURN_BUFFER)
//
typedef struct  _DPNMSG_RETURN_BUFFER
{
    DWORD       dwSize;             // Size of this structure
    HRESULT     hResultCode;        // Return value of operation
    PVOID       pvBuffer;           // Buffer being returned
    PVOID       pvUserContext;      // Context associated with buffer
} DPNMSG_RETURN_BUFFER, *PDPNMSG_RETURN_BUFFER;

//
// Send complete structure for message handler
// (DPN_MSGID_SEND_COMPLETE)
//
typedef struct  _DPNMSG_SEND_COMPLETE
{
    DWORD       dwSize;             // Size of this structure
    DPNHANDLE   hAsyncOp;           // DirectPlay8 Async operation handle
    PVOID       pvUserContext;      // User context supplied at Send/SendTo
    HRESULT     hResultCode;        // HRESULT of send
    DWORD       dwSendTime;         // Send time in ms
} DPNMSG_SEND_COMPLETE, *PDPNMSG_SEND_COMPLETE;

//
// Server info structure for message handler
// (DPN_MSGID_SERVER_INFO)
//
typedef struct  _DPNMSG_SERVER_INFO
{
    DWORD   dwSize;                 // Size of this structure
    DPNID   dpnidServer;            // DPNID of server
    PVOID   pvPlayerContext;        // Player context value
} DPNMSG_SERVER_INFO, *PDPNMSG_SERVER_INFO;

//
// Terminated session structure for message handler
// (DPN_MSGID_TERMINATE_SESSION)
//
typedef struct  _DPNMSG_TERMINATE_SESSION
{
    DWORD       dwSize;             // Size of this structure
    HRESULT     hResultCode;        // Reason
    PVOID       pvTerminateData;    // Data passed from Host/Server
    DWORD       dwTerminateDataSize;// Size (in bytes) of pvTerminateData
} DPNMSG_TERMINATE_SESSION, *PDPNMSG_TERMINATE_SESSION;

typedef struct IDirectPlay8Peer     IDirectPlay8Peer;
typedef IDirectPlay8Peer    *PDIRECTPLAY8PEER, *LPDIRECTPLAY8PEER;

typedef struct IDirectPlay8Server   IDirectPlay8Server;
typedef IDirectPlay8Server  *PDIRECTPLAY8SERVER, *LPDIRECTPLAY8SERVER;

typedef struct IDirectPlay8Client   IDirectPlay8Client;
typedef IDirectPlay8Client  *PDIRECTPLAY8CLIENT, *LPDIRECTPLAY8CLIENT;




/****************************************************************************
 *
 * DirectPlay8 Functions
 *
 ****************************************************************************/

extern HRESULT WINAPI DPlayInitialize(DWORD dwMaxMemUsage);
extern HRESULT WINAPI DPlayCleanup(void);

extern HRESULT WINAPI DirectPlay8Create(DWORD dwIID, void **ppvInterface, void **pUnknown);

/****************************************************************************
 *
 * DirectPlay8 Application Interfaces
 *
 ****************************************************************************/


//
// COM definition for DirectPlay8 Client interface
//
// IUnknown methods
ULONG WINAPI IDirectPlay8Client_AddRef(LPDIRECTPLAY8CLIENT pDirectPlay8Client);
ULONG WINAPI IDirectPlay8Client_Release(LPDIRECTPLAY8CLIENT pDirectPlay8Client);

// IDirectPlay8Client methods
HRESULT WINAPI IDirectPlay8Client_Initialize(LPDIRECTPLAY8CLIENT pDirectPlay8Client, PVOID const pvUserContext, const PFNDPNMESSAGEHANDLER pfn, const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_EnumServiceProviders(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const GUID *const pguidServiceProvider, const GUID *const pguidApplication, DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer, PDWORD const pcbEnumData, PDWORD const pcReturned, const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_EnumHosts(LPDIRECTPLAY8CLIENT pDirectPlay8Client, PDPN_APPLICATION_DESC const pApplicationDesc,IDirectPlay8Address *const pAddrHost,IDirectPlay8Address *const pDeviceInfo,PVOID const pUserEnumData,const DWORD dwUserEnumDataSize,const DWORD dwEnumCount,const DWORD dwRetryInterval,const DWORD dwTimeOut,PVOID const pvUserContext,DPNHANDLE *const pAsyncHandle,const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_CancelAsyncOperation(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPNHANDLE hAsyncHandle, const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_Connect(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address *const pHostAddr,IDirectPlay8Address *const pDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,const void *const pvUserConnectData,const DWORD dwUserConnectDataSize,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_Send(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPN_BUFFER_DESC *const prgBufferDesc,const DWORD cBufferDesc,const DWORD dwTimeOut,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_GetSendQueueInfo(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DWORD *const pdwNumMsgs, DWORD *const pdwNumBytes, const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_GetApplicationDesc(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DPN_APPLICATION_DESC *const pAppDescBuffer, DWORD *const pcbDataSize, const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_SetClientInfo(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPN_PLAYER_INFO *const pdpnPlayerInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_GetServerInfo(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DPN_PLAYER_INFO *const pdpnPlayerInfo,DWORD *const pdwSize,const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_GetServerAddress(LPDIRECTPLAY8CLIENT pDirectPlay8Client, IDirectPlay8Address **const pAddress,const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_Close(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_ReturnBuffer(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPNHANDLE hBufferHandle,const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_GetCaps(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DPN_CAPS *const pdpCaps,const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_SetCaps(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPN_CAPS *const pdpCaps, const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_SetSPCaps(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const GUID *const pguidSP, const DPN_SP_CAPS *const pdpspCaps, const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_GetSPCaps(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const GUID *const pguidSP,DPN_SP_CAPS *const pdpspCaps,const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_GetConnectionInfo(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DPN_CONNECTION_INFO *const pdpConnectionInfo,const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_RegisterLobby(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPNHANDLE dpnHandle, PVOID const pIDP8LobbiedApplication,const DWORD dwFlags);
HRESULT WINAPI IDirectPlay8Client_DoWork(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DWORD dwFlags);

#ifdef __cplusplus

// IDirectPlay8Client wrapper class
struct IDirectPlay8Client
{
    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IDirectPlay8Client_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IDirectPlay8Client_Release(this);
    }

    __inline HRESULT STDMETHODCALLTYPE Initialize(PVOID const pvUserContext, const PFNDPNMESSAGEHANDLER pfn, const DWORD dwFlags)
    {
        return IDirectPlay8Client_Initialize(this, pvUserContext, pfn, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE EnumServiceProviders(const GUID *const pguidServiceProvider, const GUID *const pguidApplication, DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer, PDWORD const pcbEnumData, PDWORD const pcReturned, const DWORD dwFlags)
    {
        return IDirectPlay8Client_EnumServiceProviders(this, pguidServiceProvider, pguidApplication, pSPInfoBuffer, pcbEnumData, pcReturned, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE EnumHosts(PDPN_APPLICATION_DESC const pApplicationDesc,IDirectPlay8Address *const pAddrHost,IDirectPlay8Address *const pDeviceInfo,PVOID const pUserEnumData,const DWORD dwUserEnumDataSize,const DWORD dwEnumCount,const DWORD dwRetryInterval,const DWORD dwTimeOut,PVOID const pvUserContext,DPNHANDLE *const pAsyncHandle,const DWORD dwFlags)
    {
        return IDirectPlay8Client_EnumHosts(this, pApplicationDesc, pAddrHost, pDeviceInfo, pUserEnumData, dwUserEnumDataSize, dwEnumCount, dwRetryInterval, dwTimeOut, pvUserContext, pAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE CancelAsyncOperation(const DPNHANDLE hAsyncHandle, const DWORD dwFlags)
    {
        return IDirectPlay8Client_CancelAsyncOperation(this, hAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE Connect(const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address *const pHostAddr,IDirectPlay8Address *const pDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,const void *const pvUserConnectData,const DWORD dwUserConnectDataSize,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags)
    {
        return IDirectPlay8Client_Connect(this, pdnAppDesc, pHostAddr, pDeviceInfo, pdnSecurity, pdnCredentials, pvUserConnectData, dwUserConnectDataSize, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE Send(const DPN_BUFFER_DESC *const prgBufferDesc,const DWORD cBufferDesc,const DWORD dwTimeOut,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags)
    {
        return IDirectPlay8Client_Send(this, prgBufferDesc, cBufferDesc, dwTimeOut, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetSendQueueInfo(DWORD *const pdwNumMsgs, DWORD *const pdwNumBytes, const DWORD dwFlags)
    {
        return IDirectPlay8Client_GetSendQueueInfo(this, pdwNumMsgs, pdwNumBytes, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetApplicationDesc(DPN_APPLICATION_DESC *const pAppDescBuffer, DWORD *const pcbDataSize, const DWORD dwFlags)
    {
        return IDirectPlay8Client_GetApplicationDesc(this, pAppDescBuffer, pcbDataSize, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE SetClientInfo(const DPN_PLAYER_INFO *const pdpnPlayerInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
    {
        return IDirectPlay8Client_SetClientInfo(this, pdpnPlayerInfo, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetServerInfo(DPN_PLAYER_INFO *const pdpnPlayerInfo,DWORD *const pdwSize,const DWORD dwFlags)
    {
        return IDirectPlay8Client_GetServerInfo(this, pdpnPlayerInfo, pdwSize, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetServerAddress(IDirectPlay8Address **const pAddress,const DWORD dwFlags)
    {
        return IDirectPlay8Client_GetServerAddress(this, pAddress, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE Close(const DWORD dwFlags)
    {
        return IDirectPlay8Client_Close(this, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE ReturnBuffer(const DPNHANDLE hBufferHandle,const DWORD dwFlags)
    {
        return IDirectPlay8Client_ReturnBuffer(this, hBufferHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetCaps(DPN_CAPS *const pdpCaps,const DWORD dwFlags)
    {
        return IDirectPlay8Client_GetCaps(this, pdpCaps, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE SetCaps(const DPN_CAPS *const pdpCaps, const DWORD dwFlags)
    {
        return IDirectPlay8Client_SetCaps(this, pdpCaps, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE SetSPCaps(const GUID *const pguidSP, const DPN_SP_CAPS *const pdpspCaps, const DWORD dwFlags)
    {
        return IDirectPlay8Client_SetSPCaps(this, pguidSP, pdpspCaps, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetSPCaps(const GUID *const pguidSP,DPN_SP_CAPS *const pdpspCaps,const DWORD dwFlags)
    {
        return IDirectPlay8Client_GetSPCaps(this, pguidSP, pdpspCaps, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetConnectionInfo(DPN_CONNECTION_INFO *const pdpConnectionInfo,const DWORD dwFlags)
    {
        return IDirectPlay8Client_GetConnectionInfo(this, pdpConnectionInfo, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE RegisterLobby(const DPNHANDLE dpnHandle, PVOID const pIDP8LobbiedApplication,const DWORD dwFlags)
    {
        return IDirectPlay8Client_RegisterLobby(this, dpnHandle, pIDP8LobbiedApplication, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE DoWork(const DWORD dwFlags)
    {
        return IDirectPlay8Client_DoWork(this, dwFlags);
    }
};

#endif // __cplusplus



//
// COM definition for DirectPlay8 Server interface
//
// IUnknown methods
ULONG WINAPI IDirectPlay8Server_AddRef(LPDIRECTPLAY8SERVER pDirectPlay8Server);
ULONG WINAPI IDirectPlay8Server_Release(LPDIRECTPLAY8SERVER pDirectPlay8Server);

// IDirectPlay8Server methods
HRESULT WINAPI IDirectPlay8Server_Initialize(LPDIRECTPLAY8SERVER pDirectPlay8Server, PVOID const pvUserContext, const PFNDPNMESSAGEHANDLER pfn, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_EnumServiceProviders(LPDIRECTPLAY8SERVER pDirectPlay8Server, const GUID *const pguidServiceProvider,const GUID *const pguidApplication,DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer,PDWORD const pcbEnumData,PDWORD const pcReturned,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_CancelAsyncOperation(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNHANDLE hAsyncHandle,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetSendQueueInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,DWORD *const pdwNumMsgs, DWORD *const pdwNumBytes, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetApplicationDesc(LPDIRECTPLAY8SERVER pDirectPlay8Server, DPN_APPLICATION_DESC *const pAppDescBuffer, DWORD *const pcbDataSize, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_SetServerInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPN_PLAYER_INFO *const pdpnPlayerInfo,PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetClientInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,DPN_PLAYER_INFO *const pdpnPlayerInfo,DWORD *const pdwSize,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetClientAddress(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,IDirectPlay8Address **const pAddress,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetLocalHostAddresses(LPDIRECTPLAY8SERVER pDirectPlay8Server, IDirectPlay8Address **const prgpAddress,DWORD *const pcAddress,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_SetApplicationDesc(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPN_APPLICATION_DESC *const pad, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_Host(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address **const prgpDeviceInfo,const DWORD cDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,void *const pvPlayerContext,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_SendTo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,const DPN_BUFFER_DESC *const prgBufferDesc,const DWORD cBufferDesc,const DWORD dwTimeOut,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_CreateGroup(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPN_GROUP_INFO *const pdpnGroupInfo,void *const pvGroupContext,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_DestroyGroup(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID idGroup, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_AddPlayerToGroup(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID idGroup, const DPNID idClient, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_RemovePlayerFromGroup(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID idGroup, const DPNID idClient, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_SetGroupInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetGroupInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,DWORD *const pdwSize,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_EnumPlayersAndGroups(LPDIRECTPLAY8SERVER pDirectPlay8Server, DPNID *const prgdpnid, DWORD *const pcdpnid, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_EnumGroupMembers(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid, DPNID *const prgdpnid, DWORD *const pcdpnid, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_Close(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_DestroyClient(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnidClient, const void *const pvDestroyData, const DWORD dwDestroyDataSize, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_ReturnBuffer(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNHANDLE hBufferHandle,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetPlayerContext(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,PVOID *const ppvPlayerContext,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetGroupContext(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,PVOID *const ppvGroupContext,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetCaps(LPDIRECTPLAY8SERVER pDirectPlay8Server, DPN_CAPS *const pdpCaps,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_SetCaps(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPN_CAPS *const pdpCaps, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_SetSPCaps(LPDIRECTPLAY8SERVER pDirectPlay8Server, const GUID *const pguidSP, const DPN_SP_CAPS *const pdpspCaps, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetSPCaps(LPDIRECTPLAY8SERVER pDirectPlay8Server, const GUID *const pguidSP, DPN_SP_CAPS *const pdpspCaps,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_GetConnectionInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid, DPN_CONNECTION_INFO *const pdpConnectionInfo,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_RegisterLobby(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNHANDLE dpnHandle, PVOID const pIDP8LobbiedApplication,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Server_DoWork(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DWORD dwFlags);


#ifdef __cplusplus

// IDirectPlay8Server wrapper class
struct IDirectPlay8Server
{
    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IDirectPlay8Server_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IDirectPlay8Server_Release(this);
    }

    __inline HRESULT STDMETHODCALLTYPE Initialize(PVOID const pvUserContext, const PFNDPNMESSAGEHANDLER pfn, const DWORD dwFlags )
    {
        return IDirectPlay8Server_Initialize(this, pvUserContext, pfn, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE EnumServiceProviders(const GUID *const pguidServiceProvider,const GUID *const pguidApplication,DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer,PDWORD const pcbEnumData,PDWORD const pcReturned,const DWORD dwFlags )
    {
        return IDirectPlay8Server_EnumServiceProviders(this, pguidServiceProvider, pguidApplication, pSPInfoBuffer, pcbEnumData, pcReturned, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE CancelAsyncOperation(const DPNHANDLE hAsyncHandle,const DWORD dwFlags )
    {
        return IDirectPlay8Server_CancelAsyncOperation(this, hAsyncHandle, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetSendQueueInfo(const DPNID dpnid,DWORD *const pdwNumMsgs, DWORD *const pdwNumBytes, const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetSendQueueInfo(this, dpnid, pdwNumMsgs, pdwNumBytes, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetApplicationDesc(DPN_APPLICATION_DESC *const pAppDescBuffer, DWORD *const pcbDataSize, const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetApplicationDesc(this, pAppDescBuffer, pcbDataSize, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE SetServerInfo(const DPN_PLAYER_INFO *const pdpnPlayerInfo,PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags )
    {
        return IDirectPlay8Server_SetServerInfo(this, pdpnPlayerInfo, pvAsyncContext, phAsyncHandle, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetClientInfo(const DPNID dpnid,DPN_PLAYER_INFO *const pdpnPlayerInfo,DWORD *const pdwSize,const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetClientInfo(this, dpnid, pdpnPlayerInfo, pdwSize, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetClientAddress(const DPNID dpnid,IDirectPlay8Address **const pAddress,const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetClientAddress(this, dpnid, pAddress, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetLocalHostAddresses(IDirectPlay8Address **const prgpAddress,DWORD *const pcAddress,const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetLocalHostAddresses(this, prgpAddress, pcAddress, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE SetApplicationDesc(const DPN_APPLICATION_DESC *const pad, const DWORD dwFlags )
    {
        return IDirectPlay8Server_SetApplicationDesc(this, pad, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE Host(const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address **const prgpDeviceInfo,const DWORD cDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,void *const pvPlayerContext,const DWORD dwFlags )
    {
        return IDirectPlay8Server_Host(this, pdnAppDesc, prgpDeviceInfo, cDeviceInfo, pdnSecurity, pdnCredentials, pvPlayerContext, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE SendTo(const DPNID dpnid,const DPN_BUFFER_DESC *const prgBufferDesc,const DWORD cBufferDesc,const DWORD dwTimeOut,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags )
    {
        return IDirectPlay8Server_SendTo(this, dpnid, prgBufferDesc, cBufferDesc, dwTimeOut, pvAsyncContext, phAsyncHandle, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE CreateGroup(const DPN_GROUP_INFO *const pdpnGroupInfo,void *const pvGroupContext,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags )
    {
        return IDirectPlay8Server_CreateGroup(this, pdpnGroupInfo, pvGroupContext, pvAsyncContext, phAsyncHandle, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE DestroyGroup(const DPNID idGroup, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags )
    {
        return IDirectPlay8Server_DestroyGroup(this, idGroup, pvAsyncContext, phAsyncHandle, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE AddPlayerToGroup(const DPNID idGroup, const DPNID idClient, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags )
    {
        return IDirectPlay8Server_AddPlayerToGroup(this, idGroup, idClient, pvAsyncContext, phAsyncHandle, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE RemovePlayerFromGroup(const DPNID idGroup, const DPNID idClient, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags )
    {
        return IDirectPlay8Server_RemovePlayerFromGroup(this, idGroup, idClient, pvAsyncContext, phAsyncHandle, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE SetGroupInfo(const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags )
    {
        return IDirectPlay8Server_SetGroupInfo(this, dpnid, pdpnGroupInfo, pvAsyncContext, phAsyncHandle, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetGroupInfo(const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,DWORD *const pdwSize,const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetGroupInfo(this, dpnid, pdpnGroupInfo, pdwSize, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE EnumPlayersAndGroups(DPNID *const prgdpnid, DWORD *const pcdpnid, const DWORD dwFlags )
    {
        return IDirectPlay8Server_EnumPlayersAndGroups(this, prgdpnid, pcdpnid, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE EnumGroupMembers(const DPNID dpnid, DPNID *const prgdpnid, DWORD *const pcdpnid, const DWORD dwFlags )
    {
        return IDirectPlay8Server_EnumGroupMembers(this, dpnid, prgdpnid, pcdpnid, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE Close(const DWORD dwFlags )
    {
        return IDirectPlay8Server_Close(this, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE DestroyClient(const DPNID dpnidClient, const void *const pvDestroyData, const DWORD dwDestroyDataSize, const DWORD dwFlags )
    {
        return IDirectPlay8Server_DestroyClient(this, dpnidClient, pvDestroyData, dwDestroyDataSize, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE ReturnBuffer(const DPNHANDLE hBufferHandle,const DWORD dwFlags )
    {
        return IDirectPlay8Server_ReturnBuffer(this, hBufferHandle, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetPlayerContext(const DPNID dpnid,PVOID *const ppvPlayerContext,const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetPlayerContext(this, dpnid, ppvPlayerContext, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetGroupContext(const DPNID dpnid,PVOID *const ppvGroupContext,const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetGroupContext(this, dpnid, ppvGroupContext, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetCaps(DPN_CAPS *const pdpCaps,const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetCaps(this, pdpCaps, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE SetCaps(const DPN_CAPS *const pdpCaps, const DWORD dwFlags )
    {
        return IDirectPlay8Server_SetCaps(this, pdpCaps, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE SetSPCaps(const GUID *const pguidSP, const DPN_SP_CAPS *const pdpspCaps, const DWORD dwFlags )
    {
        return IDirectPlay8Server_SetSPCaps(this, pguidSP, pdpspCaps, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetSPCaps(const GUID *const pguidSP, DPN_SP_CAPS *const pdpspCaps,const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetSPCaps(this, pguidSP, pdpspCaps, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE GetConnectionInfo(const DPNID dpnid, DPN_CONNECTION_INFO *const pdpConnectionInfo,const DWORD dwFlags )
    {
        return IDirectPlay8Server_GetConnectionInfo(this, dpnid, pdpConnectionInfo, dwFlags );
    }

    __inline HRESULT STDMETHODCALLTYPE RegisterLobby(const DPNHANDLE dpnHandle, PVOID const pIDP8LobbiedApplication,const DWORD dwFlags )
    {
        return IDirectPlay8Server_RegisterLobby(this, dpnHandle, pIDP8LobbiedApplication, dwFlags );
    }


    __inline HRESULT STDMETHODCALLTYPE DoWork(const DWORD dwFlags)
    {
        return IDirectPlay8Server_DoWork(this, dwFlags);
    }
};

#endif // __cplusplus

//
// COM definition for DirectPlay8 Peer interface
//
// IUnknown methods
ULONG WINAPI IDirectPlay8Peer_AddRef(LPDIRECTPLAY8PEER pDirectPlay8Peer);
ULONG WINAPI IDirectPlay8Peer_Release(LPDIRECTPLAY8PEER pDirectPlay8Peer);

// IDirectPlay8Peer methods
HRESULT WINAPI IDirectPlay8Peer_Initialize(LPDIRECTPLAY8PEER pDirectPlay8Peer, PVOID const pvUserContext, const PFNDPNMESSAGEHANDLER pfn, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_EnumServiceProviders(LPDIRECTPLAY8PEER pDirectPlay8Peer, const GUID *const pguidServiceProvider, const GUID *const pguidApplication, DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer, DWORD *const pcbEnumData, DWORD *const pcReturned, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_CancelAsyncOperation(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNHANDLE hAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_Connect(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address *const pHostAddr,IDirectPlay8Address *const pDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,const void *const pvUserConnectData,const DWORD dwUserConnectDataSize,void *const pvPlayerContext,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_SendTo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,const DPN_BUFFER_DESC *const prgBufferDesc,const DWORD cBufferDesc,const DWORD dwTimeOut,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetSendQueueInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid, DWORD *const pdwNumMsgs, DWORD *const pdwNumBytes, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_Host(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address **const prgpDeviceInfo,const DWORD cDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,void *const pvPlayerContext,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetApplicationDesc(LPDIRECTPLAY8PEER pDirectPlay8Peer, DPN_APPLICATION_DESC *const pAppDescBuffer, DWORD *const pcbDataSize, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_SetApplicationDesc(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_APPLICATION_DESC *const pad, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_CreateGroup(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_GROUP_INFO *const pdpnGroupInfo,void *const pvGroupContext,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_DestroyGroup(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID idGroup, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_AddPlayerToGroup(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID idGroup, const DPNID idClient, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_RemovePlayerFromGroup(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID idGroup, const DPNID idClient, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_SetGroupInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetGroupInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,DWORD *const pdwSize,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_EnumPlayersAndGroups(LPDIRECTPLAY8PEER pDirectPlay8Peer, DPNID *const prgdpnid, DWORD *const pcdpnid, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_EnumGroupMembers(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid, DPNID *const prgdpnid, DWORD *const pcdpnid, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_SetPeerInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_PLAYER_INFO *const pdpnPlayerInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetPeerInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,DPN_PLAYER_INFO *const pdpnPlayerInfo,DWORD *const pdwSize,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetPeerAddress(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,IDirectPlay8Address **const pAddress,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetLocalHostAddresses(LPDIRECTPLAY8PEER pDirectPlay8Peer, IDirectPlay8Address **const prgpAddress,DWORD *const pcAddress,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_Close(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_EnumHosts(LPDIRECTPLAY8PEER pDirectPlay8Peer, PDPN_APPLICATION_DESC const pApplicationDesc,IDirectPlay8Address *const pAddrHost,IDirectPlay8Address *const pDeviceInfo,PVOID const pUserEnumData,const DWORD dwUserEnumDataSize,const DWORD dwEnumCount,const DWORD dwRetryInterval,const DWORD dwTimeOut,PVOID const pvUserContext,DPNHANDLE *const pAsyncHandle,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_DestroyPeer(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnidClient, const void *const pvDestroyData, const DWORD dwDestroyDataSize, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_ReturnBuffer(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNHANDLE hBufferHandle,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetPlayerContext(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,PVOID *const ppvPlayerContext,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetGroupContext(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,PVOID *const ppvGroupContext,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetCaps(LPDIRECTPLAY8PEER pDirectPlay8Peer, DPN_CAPS *const pdpCaps,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_SetCaps(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_CAPS *const pdpCaps, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_SetSPCaps(LPDIRECTPLAY8PEER pDirectPlay8Peer, const GUID *const pguidSP, const DPN_SP_CAPS *const pdpspCaps, const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetSPCaps(LPDIRECTPLAY8PEER pDirectPlay8Peer, const GUID *const pguidSP, DPN_SP_CAPS *const pdpspCaps,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_GetConnectionInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid, DPN_CONNECTION_INFO *const pdpConnectionInfo,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_RegisterLobby(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNHANDLE dpnHandle, PVOID const pIDP8LobbiedApplication,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_TerminateSession(LPDIRECTPLAY8PEER pDirectPlay8Peer, void *const pvTerminateData,const DWORD dwTerminateDataSize,const DWORD dwFlags );
HRESULT WINAPI IDirectPlay8Peer_DoWork(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DWORD dwFlags);

#ifdef __cplusplus

// IDirectPlay8Peer wrapper class
struct IDirectPlay8Peer
{
    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IDirectPlay8Peer_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IDirectPlay8Peer_Release(this);
    }

    __inline HRESULT STDMETHODCALLTYPE Initialize(PVOID const pvUserContext, const PFNDPNMESSAGEHANDLER pfn, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_Initialize(this, pvUserContext, pfn, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE EnumServiceProviders(const GUID *const pguidServiceProvider, const GUID *const pguidApplication, DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer, DWORD *const pcbEnumData, DWORD *const pcReturned, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_EnumServiceProviders(this, pguidServiceProvider, pguidApplication, pSPInfoBuffer, pcbEnumData, pcReturned, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE CancelAsyncOperation(const DPNHANDLE hAsyncHandle, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_CancelAsyncOperation(this, hAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE Connect(const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address *const pHostAddr,IDirectPlay8Address *const pDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,const void *const pvUserConnectData,const DWORD dwUserConnectDataSize,void *const pvPlayerContext,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_Connect(this, pdnAppDesc, pHostAddr, pDeviceInfo, pdnSecurity, pdnCredentials, pvUserConnectData, dwUserConnectDataSize, pvPlayerContext, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE SendTo(const DPNID dpnid,const DPN_BUFFER_DESC *const prgBufferDesc,const DWORD cBufferDesc,const DWORD dwTimeOut,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_SendTo(this, dpnid, prgBufferDesc, cBufferDesc, dwTimeOut, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetSendQueueInfo(const DPNID dpnid, DWORD *const pdwNumMsgs, DWORD *const pdwNumBytes, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetSendQueueInfo(this, dpnid, pdwNumMsgs, pdwNumBytes, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE Host(const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address **const prgpDeviceInfo,const DWORD cDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,void *const pvPlayerContext,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_Host(this, pdnAppDesc, prgpDeviceInfo, cDeviceInfo, pdnSecurity, pdnCredentials, pvPlayerContext, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetApplicationDesc(DPN_APPLICATION_DESC *const pAppDescBuffer, DWORD *const pcbDataSize, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetApplicationDesc(this, pAppDescBuffer, pcbDataSize, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE SetApplicationDesc(const DPN_APPLICATION_DESC *const pad, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_SetApplicationDesc(this, pad, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE CreateGroup(const DPN_GROUP_INFO *const pdpnGroupInfo,void *const pvGroupContext,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_CreateGroup(this, pdpnGroupInfo, pvGroupContext, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE DestroyGroup(const DPNID idGroup, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_DestroyGroup(this, idGroup, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE AddPlayerToGroup(const DPNID idGroup, const DPNID idClient, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_AddPlayerToGroup(this, idGroup, idClient, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE RemovePlayerFromGroup(const DPNID idGroup, const DPNID idClient, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_RemovePlayerFromGroup(this, idGroup, idClient, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE SetGroupInfo(const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_SetGroupInfo(this, dpnid, pdpnGroupInfo, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetGroupInfo(const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,DWORD *const pdwSize,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetGroupInfo(this, dpnid, pdpnGroupInfo, pdwSize, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE EnumPlayersAndGroups(DPNID *const prgdpnid, DWORD *const pcdpnid, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_EnumPlayersAndGroups(this, prgdpnid, pcdpnid, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE EnumGroupMembers(const DPNID dpnid, DPNID *const prgdpnid, DWORD *const pcdpnid, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_EnumGroupMembers(this, dpnid, prgdpnid, pcdpnid, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE SetPeerInfo(const DPN_PLAYER_INFO *const pdpnPlayerInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_SetPeerInfo(this, pdpnPlayerInfo, pvAsyncContext, phAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetPeerInfo(const DPNID dpnid,DPN_PLAYER_INFO *const pdpnPlayerInfo,DWORD *const pdwSize,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetPeerInfo(this, dpnid, pdpnPlayerInfo, pdwSize, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetPeerAddress(const DPNID dpnid,IDirectPlay8Address **const pAddress,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetPeerAddress(this, dpnid, pAddress, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetLocalHostAddresses(IDirectPlay8Address **const prgpAddress,DWORD *const pcAddress,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetLocalHostAddresses(this, prgpAddress, pcAddress, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE Close(const DWORD dwFlags)
    {
        return IDirectPlay8Peer_Close(this, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE EnumHosts(PDPN_APPLICATION_DESC const pApplicationDesc,IDirectPlay8Address *const pAddrHost,IDirectPlay8Address *const pDeviceInfo,PVOID const pUserEnumData,const DWORD dwUserEnumDataSize,const DWORD dwEnumCount,const DWORD dwRetryInterval,const DWORD dwTimeOut,PVOID const pvUserContext,DPNHANDLE *const pAsyncHandle,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_EnumHosts(this, pApplicationDesc, pAddrHost, pDeviceInfo, pUserEnumData, dwUserEnumDataSize, dwEnumCount, dwRetryInterval, dwTimeOut, pvUserContext, pAsyncHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE DestroyPeer(const DPNID dpnidClient, const void *const pvDestroyData, const DWORD dwDestroyDataSize, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_DestroyPeer(this, dpnidClient, pvDestroyData, dwDestroyDataSize, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE ReturnBuffer(const DPNHANDLE hBufferHandle,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_ReturnBuffer(this, hBufferHandle, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetPlayerContext(const DPNID dpnid,PVOID *const ppvPlayerContext,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetPlayerContext(this, dpnid, ppvPlayerContext, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetGroupContext(const DPNID dpnid,PVOID *const ppvGroupContext,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetGroupContext(this, dpnid, ppvGroupContext, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetCaps(DPN_CAPS *const pdpCaps,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetCaps(this, pdpCaps, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE SetCaps(const DPN_CAPS *const pdpCaps, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_SetCaps(this, pdpCaps, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE SetSPCaps(const GUID *const pguidSP, const DPN_SP_CAPS *const pdpspCaps, const DWORD dwFlags)
    {
        return IDirectPlay8Peer_SetSPCaps(this, pguidSP, pdpspCaps, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetSPCaps(const GUID *const pguidSP, DPN_SP_CAPS *const pdpspCaps,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetSPCaps(this, pguidSP, pdpspCaps, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE GetConnectionInfo(const DPNID dpnid, DPN_CONNECTION_INFO *const pdpConnectionInfo,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_GetConnectionInfo(this, dpnid, pdpConnectionInfo, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE RegisterLobby(const DPNHANDLE dpnHandle, PVOID const pIDP8LobbiedApplication,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_RegisterLobby(this, dpnHandle, pIDP8LobbiedApplication, dwFlags);
    }

    __inline HRESULT STDMETHODCALLTYPE TerminateSession(void *const pvTerminateData,const DWORD dwTerminateDataSize,const DWORD dwFlags)
    {
        return IDirectPlay8Peer_TerminateSession(this, pvTerminateData, dwTerminateDataSize, dwFlags);
    }


    __inline HRESULT STDMETHODCALLTYPE DoWork(const DWORD dwFlags)
    {
        return IDirectPlay8Peer_DoWork(this, dwFlags);
    }
};

#endif // __cplusplus






/****************************************************************************
 *
 * DIRECTPLAY8 ERRORS
 *
 * Errors are represented by negative values and cannot be combined.
 *
 ****************************************************************************/

#define _DPN_FACILITY_CODE  0x015
#define _DPNHRESULT_BASE        0x8000
#define MAKE_DPNHRESULT( code )         MAKE_HRESULT( 1, _DPN_FACILITY_CODE, ( code + _DPNHRESULT_BASE ) )

#define DPN_OK                          S_OK

#define DPNSUCCESS_EQUAL                MAKE_HRESULT( 0, _DPN_FACILITY_CODE, ( 0x5 + _DPNHRESULT_BASE ) )
#define DPNSUCCESS_NOTEQUAL             MAKE_HRESULT( 0, _DPN_FACILITY_CODE, (0x0A + _DPNHRESULT_BASE ) )
#define DPNSUCCESS_PENDING              MAKE_HRESULT( 0, _DPN_FACILITY_CODE, (0x0e + _DPNHRESULT_BASE ) )

#define DPNERR_ABORTED                  MAKE_DPNHRESULT(  0x30 )
#define DPNERR_ADDRESSING               MAKE_DPNHRESULT(  0x40 )
#define DPNERR_ALREADYCLOSING           MAKE_DPNHRESULT(  0x50 )
#define DPNERR_ALREADYCONNECTED         MAKE_DPNHRESULT(  0x60 )
#define DPNERR_ALREADYDISCONNECTING     MAKE_DPNHRESULT(  0x70 )
#define DPNERR_ALREADYINITIALIZED       MAKE_DPNHRESULT(  0x80 )
#define DPNERR_ALREADYREGISTERED        MAKE_DPNHRESULT(  0x90 )
#define DPNERR_BUFFERTOOSMALL           MAKE_DPNHRESULT( 0x100 )
#define DPNERR_CANNOTCANCEL             MAKE_DPNHRESULT( 0x110 )
#define DPNERR_CANTCREATEGROUP          MAKE_DPNHRESULT( 0x120 )
#define DPNERR_CANTCREATEPLAYER         MAKE_DPNHRESULT( 0x130 )
#define DPNERR_CANTLAUNCHAPPLICATION    MAKE_DPNHRESULT( 0x140 )
#define DPNERR_CONNECTING               MAKE_DPNHRESULT( 0x150 )
#define DPNERR_CONNECTIONLOST           MAKE_DPNHRESULT( 0x160 )
#define DPNERR_CONVERSION               MAKE_DPNHRESULT( 0x170 )
#define DPNERR_DATATOOLARGE             MAKE_DPNHRESULT( 0x175 )
#define DPNERR_DOESNOTEXIST             MAKE_DPNHRESULT( 0x180 )
#define DPNERR_DUPLICATECOMMAND         MAKE_DPNHRESULT( 0x190 )
#define DPNERR_ENDPOINTNOTRECEIVING     MAKE_DPNHRESULT( 0x200 )
#define DPNERR_ENUMQUERYTOOLARGE        MAKE_DPNHRESULT( 0x210 )
#define DPNERR_ENUMRESPONSETOOLARGE     MAKE_DPNHRESULT( 0x220 )
#define DPNERR_EXCEPTION                MAKE_DPNHRESULT( 0x230 )
#define DPNERR_GENERIC                  E_FAIL
#define DPNERR_GROUPNOTEMPTY            MAKE_DPNHRESULT( 0x240 )
#define DPNERR_HOSTING                  MAKE_DPNHRESULT( 0x250 )
#define DPNERR_HOSTREJECTEDCONNECTION   MAKE_DPNHRESULT( 0x260 )
#define DPNERR_HOSTTERMINATEDSESSION    MAKE_DPNHRESULT( 0x270 )
#define DPNERR_INCOMPLETEADDRESS        MAKE_DPNHRESULT( 0x280 )
#define DPNERR_INVALIDADDRESSFORMAT     MAKE_DPNHRESULT( 0x290 )
#define DPNERR_INVALIDAPPLICATION       MAKE_DPNHRESULT( 0x300 )
#define DPNERR_INVALIDCOMMAND           MAKE_DPNHRESULT( 0x310 )
#define DPNERR_INVALIDDEVICEADDRESS     MAKE_DPNHRESULT( 0x320 )
#define DPNERR_INVALIDENDPOINT          MAKE_DPNHRESULT( 0x330 )
#define DPNERR_INVALIDFLAGS             MAKE_DPNHRESULT( 0x340 )
#define DPNERR_INVALIDGROUP             MAKE_DPNHRESULT( 0x350 )
#define DPNERR_INVALIDHANDLE            MAKE_DPNHRESULT( 0x360 )
#define DPNERR_INVALIDHOSTADDRESS       MAKE_DPNHRESULT( 0x370 )
#define DPNERR_INVALIDINSTANCE          MAKE_DPNHRESULT( 0x380 )
#define DPNERR_INVALIDINTERFACE         MAKE_DPNHRESULT( 0x390 )
#define DPNERR_INVALIDOBJECT            MAKE_DPNHRESULT( 0x400 )
#define DPNERR_INVALIDPARAM             E_INVALIDARG
#define DPNERR_INVALIDPASSWORD          MAKE_DPNHRESULT( 0x410 )
#define DPNERR_INVALIDPLAYER            MAKE_DPNHRESULT( 0x420 )
#define DPNERR_INVALIDPOINTER           E_POINTER
#define DPNERR_INVALIDPRIORITY          MAKE_DPNHRESULT( 0x430 )
#define DPNERR_INVALIDSTRING            MAKE_DPNHRESULT( 0x440 )
#define DPNERR_INVALIDURL               MAKE_DPNHRESULT( 0x450 )
#define DPNERR_INVALIDVERSION           MAKE_DPNHRESULT( 0x460 )
#define DPNERR_NOCAPS                   MAKE_DPNHRESULT( 0x470 )
#define DPNERR_NOCONNECTION             MAKE_DPNHRESULT( 0x480 )
#define DPNERR_NOHOSTPLAYER             MAKE_DPNHRESULT( 0x490 )
#define DPNERR_NOINTERFACE              E_NOINTERFACE
#define DPNERR_NOMOREADDRESSCOMPONENTS  MAKE_DPNHRESULT( 0x500 )
#define DPNERR_NORESPONSE               MAKE_DPNHRESULT( 0x510 )
#define DPNERR_NOTALLOWED               MAKE_DPNHRESULT( 0x520 )
#define DPNERR_NOTHOST                  MAKE_DPNHRESULT( 0x530 )
#define DPNERR_NOTREADY                 MAKE_DPNHRESULT( 0x540 )
#define DPNERR_NOTREGISTERED            MAKE_DPNHRESULT( 0x550 )
#define DPNERR_OUTOFMEMORY              E_OUTOFMEMORY
#define DPNERR_PENDING                  DPNSUCCESS_PENDING
#define DPNERR_PLAYERALREADYINGROUP     MAKE_DPNHRESULT( 0x560 )
#define DPNERR_PLAYERLOST               MAKE_DPNHRESULT( 0x570 )
#define DPNERR_PLAYERNOTINGROUP         MAKE_DPNHRESULT( 0x580 )
#define DPNERR_PLAYERNOTREACHABLE       MAKE_DPNHRESULT( 0x590 )
#define DPNERR_SENDTOOLARGE             MAKE_DPNHRESULT( 0x600 )
#define DPNERR_SESSIONFULL              MAKE_DPNHRESULT( 0x610 )
#define DPNERR_TABLEFULL                MAKE_DPNHRESULT( 0x620 )
#define DPNERR_TIMEDOUT                 MAKE_DPNHRESULT( 0x630 )
#define DPNERR_UNINITIALIZED            MAKE_DPNHRESULT( 0x640 )
#define DPNERR_UNSUPPORTED              E_NOTIMPL
#define DPNERR_USERCANCEL               MAKE_DPNHRESULT( 0x650 )
#define DPNERR_NOAGGREGATION                CLASS_E_NOAGGREGATION

#ifdef __cplusplus
}
#endif

#endif
