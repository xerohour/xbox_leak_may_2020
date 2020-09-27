/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dncore.h
 *  Content:    DIRECT NET CORE HEADER FILE
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/21/99	mjn		Created
 *	10/08/99	jtk		Moved COM interfaces into separate files
 *	11/09/99	mjn		Moved Worker Thread constants/structures to separate file
 *  12/23/99	mjn		Hand all NameTable update sends from Host to worker thread
 *  12/23/99	mjn		Added host migration structures and functions
 *	12/28/99	mjn		Added DNCompleteOutstandingOperations
 *	12/28/99	mjn		Added NameTable version to Host migration message
 *	12/28/99	mjn		Moved Async Op stuff to Async.h
 *	01/03/00	mjn		Added DNPrepareToDeletePlayer
 *	01/04/00	mjn		Added code to allow outstanding ops to complete at host migration
 *	01/06/00	mjn		Moved NameTable stuff to NameTable.h
 *	01/07/00	mjn		Moved Misc Functions to DNMisc.h
 *	01/08/00	mjn		Added DN_INTERNAL_MESSAGE_CONNECT_FAILED
 *	01/08/00	mjn		Removed unused connection info
 *	01/09/00	mjn		Added Application Description routines
 *						Changed SEND/ACK NAMETABLE to SEND/ACK CONNECT INFO
 *	01/10/00	mjn		Added DNSendUpdateAppDescMessage and DN_UserUpdateAppDesc
 *	01/11/00	mjn		Use CPackedBuffers instead of DN_ENUM_BUFFER_INFOs
 *						Moved Application Description stuff to AppDesc.h
 *						Moved Connect/Disconnect stuff to Connect.h
 *	01/13/00	mjn		Added CFixedPools for CRefCountBuffers
 *	01/14/00	mjn		Removed pvUserContext from DN_NAMETABLE_ENTRY_INFO
 *	01/14/00	mjn		Moved Message stuff to Message.h
 *	01/15/00	mjn		Replaced DN_COUNT_BUFFER with CRefCountBuffer
 *	01/16/00	mjn		Modified User message handler definition
 *						Moved User call back stuff to User.h
 *	01/17/00	mjn		Added DN_MSG_INTERNAL_VOICE_SEND and DN_MSG_INTERNAL_BUFFER_IN_USE
 *	01/18/00	mjn		Moved NameTable info structures to NameTable.h
 *	01/19/00	mjn		Added structures for NameTable Operation List
 *	01/20/00	mjn		Moved internal messages to Message.h
 *	01/21/00	mjn		Removed DNAcknowledgeHostRequest
 *	01/23/00	mjn		Added DN_MSG_INTERNAL_HOST_DESTROY_PLAYER
 *	01/24/00	mjn		Implemented NameTable operation list clean up
 *	01/25/00	mjn		Changed Host Migration to multi-step affair
 *	01/26/00	mjn		Implemented NameTable re-sync at host migration
 *	01/27/00	mjn		Reordered DN_MSG_INTERNAL's
 *	01/31/00	mjn		Added Internal FPM's for RefCountBuffers
 *	02/09/00	mjn		Implemented DNSEND_COMPLETEONPROCESS
 *	03/23/00	mjn		Implemented RegisterLobby()
 *	04/04/00	mjn		Added DN_MSG_INTERNAL_TERMINATE_SESSION
 *	04/09/00	mjn		Added support for CAsyncOp
 *	04/11/00	mjn		Moved DN_INTERNAL_MESSAGE_HEADER from Async.h
 *	04/11/00	mjn		Added DIRECTNETOBJECT bilink for CAsyncOps
 *	04/23/00	mjn		Replaced DN_MSG_INTERNAL_SEND_PROCESSED with DN_MSG_INTERNAL_REQ_PROCESS_COMPLETION
 *				mjn		Replaced DN_MSG_INTERNAL_SEND_PROCESSED_COMPLETE with DN_MSG_INTERNAL_PROCESS_COMPLETION
 *	04/26/00	mjn		Removed DN_ASYNC_OP and related functions
 *	04/28/00	mjn		Code clean up - removed comments and unused consts/structs/funcs
 *	05/23/00	mjn		Added DN_MSG_INTERNAL_INSTRUCTED_CONNECT_FAILED
 *	07/05/00	mjn		Removed references to DN_MSG_INTERNAL_ENUM_WITH_APPLICATION_GUID,DN_MSG_INTERNAL_ENUM,DN_MSG_INTERNAL_ENUM_RESPONSE
 *	07/07/00	mjn		Added pNewHost as host migration target to DirectNetObject
 *  07/09/00	rmt		Bug #38323 - RegisterLobby needs a DPNHANDLE parameter.
 *	07/12/00	mjn		Moved internal messages back to Message.h
 *	07/17/00	mjn		Add signature to DirectNetObject
 *	07/28/00	mjn		Added m_bilinkConnections to DirectNetObject
 *	07/30/00	mjn		Added CPendingDeletion
 *	07/31/00	mjn		Added CQueuedMsg
 *	08/05/00	mjn		Added m_bilinkActiveList and csActiveList
 *	08/06/00	mjn		Added CWorkerJob
 *	08/09/00	mjn		Added csConnectionList and m_bilinkIndicated
 *	08/11/00	mjn		Added DN_OBJECT_FLAG_HOST_MIGRATING_2 flag (!)
 *	08/23/00	mjn		Added DN_OBJECT_FLAG_DPNSVR_REGISTERED
 *	08/23/00	mjn		Added CNameTableOp
 *	09/04/00	mjn		Added CApplicationDesc
  *  09/13/00	rmt		Bug #44625 - DPVOICE: Multihomed machines are not always enumerable (Added DN_OBJECT_FLAG_LOCALHOST flag).
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#ifndef	__DNCORE_H__
#define	__DNCORE_H__

//**********************************************************************
// Constant definitions
//**********************************************************************

#define	DN_VERSION_MAJOR					0x0000
#define	DN_VERSION_MINOR					0x0001
#define	DN_VERSION_CURRENT					((DN_VERSION_MAJOR << 16) | DN_VERSION_MINOR)

#define	DN_OBJECT_FLAG_INITIALIZED			0x80000000
#define	DN_OBJECT_FLAG_LOBBY_AWARE			0x40000000
#define DN_OBJECT_FLAG_LOCALHOST			0x10000000
#define	DN_OBJECT_FLAG_LISTENING			0x04000000
#define	DN_OBJECT_FLAG_ENUMERATING			0x02000000
#define	DN_OBJECT_FLAG_HOST_CONNECTED		0x01000000
#define	DN_OBJECT_FLAG_CONNECTING			0x00800000
#define	DN_OBJECT_FLAG_CONNECTED			0x00400000
#define	DN_OBJECT_FLAG_DISCONNECTING		0x00200000
#define	DN_OBJECT_FLAG_CLOSING				0x00100000
#define DN_OBJECT_FLAG_HOST_MIGRATING_2		0x00020000
#define DN_OBJECT_FLAG_HOST_MIGRATING		0x00010000
#define	DN_OBJECT_FLAG_PEER					0x00000004
#define	DN_OBJECT_FLAG_CLIENT				0x00000002
#define	DN_OBJECT_FLAG_SERVER				0x00000001

#define	DN_ALL_PLAYERS_GROUP_NAME			L"ALL PLAYERS"
#define	DN_ALL_PLAYERS_GROUP_NAME_SIZE		24

#undef DPF_SUBCOMP
#define DPF_SUBCOMP	DN_SUBCOMP_CORE // Used by Debug Logging to determine sub-component

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

typedef struct IDP8ServiceProvider	IDP8ServiceProvider;				// DPSP8.h
typedef struct IDirectPlayVoiceNotify			*PDIRECTPLAYVOICENOTIFY;

template < class CRefCountBuffer > class CLockedContextClassFixedPool;
template < class CSyncEvent > class CLockedContextClassFixedPool;
template < class CConnection > class CLockedContextClassFixedPool;
template < class CGroupConnection > class CLockedContextClassFixedPool;
template < class CGroupMember > class CLockedContextClassFixedPool;
template < class CNameTableEntry > class CLockedContextClassFixedPool;
template < class CNameTableOp > class CLockedContextClassFixedPool;
template < class CAsyncOp > class CLockedContextClassFixedPool;
template < class CPendingDeletion > class CLockedContextClassFixedPool;
template < class CQueuedMsg > class CLockedContextClassFixedPool;
template < class CWorkerJob > class CLockedContextClassFixedPool;
template < class CMemoryBlockTiny > class CLockedContextClassFixedPool;
template < class CMemoryBlockSmall > class CLockedContextClassFixedPool;
template < class CMemoryBlockMedium > class CLockedContextClassFixedPool;
template < class CMemoryBlockLarge > class CLockedContextClassFixedPool;
template < class CMemoryBlockHuge > class CLockedContextClassFixedPool;

typedef struct protocoldata	*PProtocolData;

#ifdef DPLAY_DOWORK_STATEMN
typedef struct _DPLAY_STMN_ASYNC
{
	STMN_ASYNC stmnAsync;
	DIRECTNETOBJECT *pDplayObject;
} DPLAY_STMN_ASYNC, *PDPLAY_STMN_ASYNC;
#endif

//
// Voice Additions
//
// this is the number of clients of IDirectXVoice in this process
// this is actually a ridiculously large number of client slots.
//
#ifdef ENABLE_DPLAY_VOICE
#define MAX_VOICE_CLIENTS	32	
#endif

class DIRECTNETOBJECT
    : public IDirectPlay8Peer, public IDirectPlay8Client, public IDirectPlay8Server
#ifdef ENABLE_DPLAY_VOICE
    , IDirectPlayVoiceTransport
#endif
{
	STDNEWDELETE

protected:
	ULONG m_ulRefCount;	//object reference count

public:
	DIRECTNETOBJECT(DWORD dwDirectPlayType);
	~DIRECTNETOBJECT(void);
	HRESULT ObjectInit(void);

	// IUnknown interface
	ULONG AddRef(void);
	ULONG Release(void);

	// Client interface
	HRESULT DN_ClientConnect(IDirectPlay8Client *pInterface, const DPN_APPLICATION_DESC *const pdnAppDesc, IDirectPlay8Address *const pHostAddr, IDirectPlay8Address *const pDeviceInfo, const DPN_SECURITY_DESC *const pdnSecurity, const DPN_SECURITY_CREDENTIALS *const pdnCredentials, const void *const pvUserConnectData, const DWORD dwUserConnectDataSize, void *const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_Send( IDirectPlay8Client *pInterface, const DPN_BUFFER_DESC *const prgBufferDesc, const DWORD cBufferDesc, const DWORD dwTimeOut, void *const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_SetClientInfo(IDirectPlay8Client *pInterface, const DPN_PLAYER_INFO *const pdpnPlayerInfo, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_GetServerInfo(IDirectPlay8Client *pInterface, DPN_PLAYER_INFO *const pdpnPlayerInfo, DWORD *const pdwSize, const DWORD dwFlags);
	HRESULT DN_GetHostSendQueueInfo(IDirectPlay8Client *pInterface, DWORD *const lpdwNumMsgs, DWORD *const lpdwNumBytes, const DWORD dwFlags );
	HRESULT DN_GetServerAddress(IDirectPlay8Client *pInterface, IDirectPlay8Address **const ppAddress, const DWORD dwFlags);

	// Peer interface
	HRESULT DN_SetPeerInfo( IDirectPlay8Peer *pInterface, const DPN_PLAYER_INFO *const pdpnPlayerInfo, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_GetPeerInfo(IDirectPlay8Peer *pInterface, const DPNID dpnid, DPN_PLAYER_INFO *const pdpnPlayerInfo, DWORD *const pdwSize, const DWORD dwFlags);
	HRESULT DN_GetPeerAddress(IDirectPlay8Peer *pInterface, const DPNID dpnid, IDirectPlay8Address **const ppAddress, const DWORD dwFlags);

	// Server interface
	HRESULT DN_SetServerInfo(IDirectPlay8Server *pInterface, const DPN_PLAYER_INFO *const pdpnPlayerInfo, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_GetClientInfo(IDirectPlay8Server *pInterface, const DPNID dpnid, DPN_PLAYER_INFO *const pdpnPlayerInfo, DWORD *const pdwSize, const DWORD dwFlags);
	HRESULT DN_GetClientAddress(IDirectPlay8Server *pInterface, const DPNID dpnid, IDirectPlay8Address **const ppAddress, const DWORD dwFlags);

	// Common interface methods
	HRESULT DN_Initialize(PVOID pInterface, PVOID const pvUserContext, const PFNDPNMESSAGEHANDLER pfn, const DWORD dwFlags);
	HRESULT DN_Close(PVOID pInterface, const DWORD dwFlags);
	HRESULT DN_EnumServiceProviders( PVOID pInterface, const GUID *const pguidServiceProvider, const GUID *const pguidApplication, DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer, DWORD *const pcbEnumData, DWORD *const pcReturned, const DWORD dwFlags );
	HRESULT DN_CancelAsyncOperation(PVOID pvInterface, const DPNHANDLE hAsyncOp, const DWORD dwFlags);
	HRESULT DN_Connect( PVOID pInterface, const DPN_APPLICATION_DESC *const pdnAppDesc, IDirectPlay8Address *const pHostAddr, IDirectPlay8Address *const pDeviceInfo, const DPN_SECURITY_DESC *const pdnSecurity, const DPN_SECURITY_CREDENTIALS *const pdnCredentials, const void *const pvUserConnectData, const DWORD dwUserConnectDataSize, void *const pvPlayerContext, void *const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_GetSendQueueInfo(PVOID pInterface, const DPNID dpnid, DWORD *const pdwNumMsgs, DWORD *const pdwNumBytes, const DWORD dwFlags);
	HRESULT DN_GetApplicationDesc(PVOID pInterface, DPN_APPLICATION_DESC *const pAppDescBuffer, DWORD *const pcbDataSize, const DWORD dwFlags);
	HRESULT DN_SetApplicationDesc(PVOID pInterface, const DPN_APPLICATION_DESC *const pdnApplicationDesc, const DWORD dwFlags);
	HRESULT DN_SendTo( PVOID pv, const DPNID dpnid, const DPN_BUFFER_DESC *const prgBufferDesc, const DWORD cBufferDesc, const DWORD dwTimeOut, void *const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_Host( PVOID pInterface, const DPN_APPLICATION_DESC *const pdnAppDesc, IDirectPlay8Address **const prgpDeviceInfo, const DWORD cDeviceInfo, const DPN_SECURITY_DESC *const pdnSecurity, const DPN_SECURITY_CREDENTIALS *const pdnCredentials, void *const pvPlayerContext, const DWORD dwFlags);
	HRESULT DN_CreateGroup(PVOID pInterface, const DPN_GROUP_INFO *const pdpnGroupInfo, void *const pvGroupContext, void *const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_DestroyGroup(PVOID pInterface, const DPNID dpnidGroup, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_AddClientToGroup(PVOID pInterface, const DPNID dpnidGroup, const DPNID dpnidClient, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_RemoveClientFromGroup(PVOID pInterface, const DPNID dpnidGroup, const DPNID dpnidClient, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_SetGroupInfo( PVOID pv, const DPNID dpnid, DPN_GROUP_INFO *const pdpnGroupInfo, PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags);
	HRESULT DN_GetGroupInfo(PVOID pv, const DPNID dpnid, DPN_GROUP_INFO *const pdpnGroupInfo, DWORD *const pdwSize, const DWORD dwFlags);
	HRESULT DN_EnumClientsAndGroups(LPVOID lpv, DPNID *const lprgdpnid, DWORD *const lpcdpnid, const DWORD dwFlags);
	HRESULT DN_EnumGroupMembers(LPVOID lpv,DPNID dpnid, DPNID *const lprgdpnid, DWORD *const lpcdpnid, const DWORD dwFlags);
	HRESULT DN_EnumHosts( PVOID pv, DPN_APPLICATION_DESC *const pApplicationDesc, IDirectPlay8Address *const pAddrHost, IDirectPlay8Address *const pDeviceInfo, PVOID const pUserEnumData, const DWORD dwUserEnumDataSize, const DWORD dwRetryCount, const DWORD dwRetryInterval, const DWORD dwTimeOut, PVOID const pvUserContext, DPNHANDLE *const pAsyncHandle, const DWORD dwFlags );
	HRESULT DN_DestroyPlayer(PVOID pv, const DPNID dnid, const void *const pvDestroyData, const DWORD dwDestroyDataSize, const DWORD dwFlags);
	HRESULT DN_ReturnBuffer(PVOID pv, const DPNHANDLE hBufferHandle, const DWORD dwFlags);
	HRESULT DN_GetPlayerContext(PVOID pv, const DPNID dpnid, PVOID *const ppvPlayerContext, const DWORD dwFlags);
	HRESULT DN_GetGroupContext(PVOID pv, const DPNID dpnid, PVOID *const ppvGroupContext, const DWORD dwFlags);
	HRESULT DN_RegisterLobby(PVOID pInterface, const DPNHANDLE dpnhLobbyConnection, VOID *const pIDP8LobbiedApplication, const DWORD dwFlags);
	HRESULT DN_TerminateSession(PVOID pInterface, void *const pvTerminateData, const DWORD dwTerminateDataSize, const DWORD dwFlags);
	HRESULT DN_GetHostAddress(PVOID pInterface, IDirectPlay8Address **const prgpAddress, DWORD *const pcAddress, const DWORD dwFlags);
	HRESULT DN_DumpNameTable(PVOID pInterface,char *const Buffer);
	HRESULT DoWork( const DWORD dwFlags );

	// Caps
	HRESULT DN_SetCaps(PVOID pv, const DPN_CAPS *const pdnCaps, const DWORD dwFlags);
	HRESULT DN_GetCaps(PVOID pv, DPN_CAPS *const pdnCaps, const DWORD dwFlags);
	HRESULT DN_GetSPCaps(PVOID pv, const GUID * const pguidSP, DPN_SP_CAPS *const pdnSPCaps, const DWORD dwFlags);
	HRESULT DN_SetSPCaps(PVOID pv, const GUID * const pguidSP, const DPN_SP_CAPS *const pdnSPCaps, const DWORD dwFlags);
	HRESULT DN_GetConnectionInfo(PVOID pv, const DPNID dpnid, DPN_CONNECTION_INFO *const pdpConnectionInfo, const DWORD dwFlags);
	HRESULT DN_GetServerConnectionInfo(PVOID pv, DPN_CONNECTION_INFO *const pdpConnectionInfo, const DWORD dwFlags);
	HRESULT DN_GetConnectionInfoHelper(PVOID pv, const DPNID dpnid, DPN_CONNECTION_INFO *const pdpConnectionInfo, BOOL fServerPlayer, const DWORD dwFlags);

	// Voice methods
#ifdef ENABLE_DPLAY_VOICE
	HRESULT VoiceAdvise( IUnknown *pUnknown, DWORD dwObjectType );
	HRESULT VoiceUnAdvise( DWORD dwObjectType );
	HRESULT VoiceIsGroupMember( DVID dvidGroup, DVID dvidPlayer );
	HRESULT VoiceSendSpeech( DVID dvidFrom, DVID dvidTo, PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvUserContext, DWORD dwFlags );
	HRESULT VoiceSendSpeechAsync( DVID dvidFrom, DVID dvidTo, PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvUserContext, DWORD dwFlags );
	HRESULT VoiceSendSpeechSync( DVID dvidFrom, DVID dvidTo, PDVTRANSPORT_BUFFERDESC pBufferDesc, void *pvUserContext, DWORD dwFlags );
	HRESULT VoiceGetSessionInfo( PDVTRANSPORTINFO pdvTransportInfo );
	HRESULT VoiceIsValidEntity( DVID dvidID, PBOOL pfIsValid );
	HRESULT VoiceSendSpeechEx( DVID dvidFrom, DWORD dwNumTargets, PDVID pdvidTargets, PDVTRANSPORT_BUFFERDESC, PVOID pvUserContext, DWORD dwFlags );
	HRESULT VoiceIsValidGroup( DVID dvidID, PBOOL pfIsValid );
	HRESULT VoiceIsValidPlayer( DVID dvidID, PBOOL pfIsValid );
#endif

	// Typecast
	static DIRECTNETOBJECT *STDMETHODCALLTYPE GetDirectPlay8Peer(IDirectPlay8Peer *pBuffer);
	static DIRECTNETOBJECT *STDMETHODCALLTYPE GetDirectPlay8Client(IDirectPlay8Client *pBuffer);
	static DIRECTNETOBJECT *STDMETHODCALLTYPE GetDirectPlay8Server(IDirectPlay8Server *pBuffer);
#ifdef ENABLE_DPLAY_VOICE
	static DIRECTNETOBJECT *STDMETHODCALLTYPE GetDirectPlayVoiceTransport(IDirectPlayVoiceTransport *pBuffer);
#endif

#ifdef DPLAY_DOWORK
	DWORD DNWorkerThreadProc(void);
#ifdef DPLAY_DOWORK_STATEMN
	DPLAY_STMN_ASYNC m_stmnAsync;
#endif
#endif

	BYTE					Sig[4];					// Signature
	DWORD					dwFlags;
	DNCRITICAL_SECTION		csDirectNetObject;		// access control critical section
	PVOID					pvUserContext;
	PFNDPNMESSAGEHANDLER	pfnDnUserMessageHandler;
	DWORD					dwLockCount;			// Count to prevent closing
	HANDLE					hLockEvent;				// Set when dwLockCount=0

	CNameTableEntry			*pNewHost;				// Host migration target

	CApplicationDesc		ApplicationDesc;

	CNameTable				NameTable;

	CHandleTable			HandleTable;

	DNCRITICAL_SECTION		csAsyncOperations;		// CS to protect outstanding async CBilink
	CBilink					m_bilinkAsyncOps;		// Outstanding CAsyncOps	(use csAsyncOperations)
	CBilink					m_bilinkConnections;	// Outstanding CConnections

	DNCRITICAL_SECTION		csActiveList;			// CS to protect active list of AsyncOps
	CBilink					m_bilinkActiveList;		// Active CAsyncOps (w/ protocol handles)

	DNCRITICAL_SECTION		csConnectionList;		// CS to guard the connection list
	CBilink					m_bilinkIndicated;		// Indicated connections

	DNCRITICAL_SECTION		csNameTableOpList;		// CS to protect NameTable operation list
	CBilink					m_bilinkPendingDeletions;// CBilink of NameTable pending deletions

	CAsyncOp				*pListenParent;			// LISTEN async op

	CAsyncOp				*pConnectParent;		// CONNECT async op
	PVOID					pvConnectData;			// Connect data
	DWORD					dwConnectDataSize;

	PProtocolData			pdnProtocolData;
	LONG					lProtocolRefCount;		// Protocol usage
	CSyncEvent				*hProtocolShutdownEvent;// No outstanding protocol operations

	CSyncEvent				*m_hWorkerCloseEvent;// No outstanding worker operations

	DNCRITICAL_SECTION		csServiceProviders;
	CBilink					m_bilinkServiceProviders;
    CBilink                  blSPCapsList;           // Cached SP settings structures -- used to cache SPCAPS settings

	IDirectPlay8Address		*pIDP8ADevice;			// SP Local Device
	IDirectPlay8Address		*pIDP8AEnum;			// Address to listen on for enums after host migration


	DNCRITICAL_SECTION		csWorkerQueue;			// CS to protect worker thread job queue
	CBilink					m_bilinkWorkerJobs;
#ifndef DPLAY_DOWORK
	HANDLE					hWorkerEvent;			// Handle of event to trigger worker thread
	HANDLE					hWorkerThread;			// Worker thread handle
#endif

	CLockedContextClassFixedPool< CRefCountBuffer >		*m_pFPOOLRefCountBuffer;

	CLockedContextClassFixedPool< CSyncEvent >			*m_pFPOOLSyncEvent;

	CLockedContextClassFixedPool< CConnection >			*m_pFPOOLConnection;

	CLockedContextClassFixedPool< CGroupConnection>		*m_pFPOOLGroupConnection;

	CLockedContextClassFixedPool< CGroupMember >		*m_pFPOOLGroupMember;

	CLockedContextClassFixedPool< CNameTableEntry >		*m_pFPOOLNameTableEntry;

	CLockedContextClassFixedPool< CNameTableOp >		*m_pFPOOLNameTableOp;

	CLockedContextClassFixedPool< CAsyncOp >			*m_pFPOOLAsyncOp;

	CLockedContextClassFixedPool< CPendingDeletion >	*m_pFPOOLPendingDeletion;

	CLockedContextClassFixedPool< CQueuedMsg >			*m_pFPOOLQueuedMsg;

	CLockedContextClassFixedPool< CWorkerJob >			*m_pFPOOLWorkerJob;

	CLockedContextClassFixedPool< CMemoryBlockTiny>		*m_pFPOOLMemoryBlockTiny;
	CLockedContextClassFixedPool< CMemoryBlockSmall>	*m_pFPOOLMemoryBlockSmall;
	CLockedContextClassFixedPool< CMemoryBlockMedium>	*m_pFPOOLMemoryBlockMedium;
	CLockedContextClassFixedPool< CMemoryBlockLarge>	*m_pFPOOLMemoryBlockLarge;
	CLockedContextClassFixedPool< CMemoryBlockHuge>		*m_pFPOOLMemoryBlockHuge;

#ifdef ENABLE_DPLAY_VOICE
	// Voice Additions
	PDIRECTPLAYVOICENOTIFY	lpDxVoiceNotifyServer;
	PDIRECTPLAYVOICENOTIFY  lpDxVoiceNotifyClient;
	DNCRITICAL_SECTION		csVoice;
#endif

	// Send Target Cache for voice targets on DV_SendSpeechEx
	DWORD					nTargets;			     // number of used entries in the target list
	DWORD					nTargetListLen;          // max number of target list entries list can hold
	PDPNID					pTargetList;	  	   // ptr to target list array
	DWORD					nExpandedTargets;        // simplified list of targets, removes dup's
	DWORD					nExpandedTargetListLen;  // max number of target list entries list can hold
	PDPNID					pExpandedTargetList;	// ptr to array of simplified list of targets

	// Lobby additions
	IDirectPlay8Address		*pConnectAddress;		// Connect Address (cached) for clients
	
	
#ifdef	DEBUG
	DNCRITICAL_SECTION		csDebugCount;			// DEBUG - CS to protect debug counts
	DWORD					dwWorkerJobCount;		// DEBUG - Count of worker jobs
	BOOL					bDebugUse;				// DEBUG - BOOLEAN
#endif
};

__inline DIRECTNETOBJECT *DIRECTNETOBJECT::GetDirectPlay8Peer(IDirectPlay8Peer *pBuffer)
{
    return (DIRECTNETOBJECT *)pBuffer;
}

__inline DIRECTNETOBJECT *DIRECTNETOBJECT::GetDirectPlay8Client(IDirectPlay8Client *pBuffer)
{
    return (DIRECTNETOBJECT *)((void*)pBuffer);
}

__inline DIRECTNETOBJECT *DIRECTNETOBJECT::GetDirectPlay8Server(IDirectPlay8Server *pBuffer)
{
    return (DIRECTNETOBJECT *)((void*)pBuffer);
}

#ifdef ENABLE_DPLAY_VOICE
__inline DIRECTNETOBJECT *DIRECTNETOBJECT::GetDirectPlayVoiceTransport(IDirectPlayVoiceTransport *pBuffer)
{
    return (DIRECTNETOBJECT *)((PVOID)pBuffer);
}
#endif


//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

//	DirectNet Core Message Handler
HRESULT	DN_CoreMessageHandler(PVOID const pv,
							  const DWORD dwMsgId,
							  const HANDLE hEndPt,
							  PBYTE const pData,
							  const DWORD dwDataSize,
							  PVOID const pvUserContext,
							  const HANDLE hProtocol,
							  const HRESULT hr);

//	Protocol Ref Counts
void DNProtocolAddRef(DIRECTNETOBJECT *const pdnObject);
void DNProtocolRelease(DIRECTNETOBJECT *const pdnObject);

// DirectNet - Host Migration routines
HRESULT	DNFindNewHost(DIRECTNETOBJECT *const pdnObject,
					  DPNID *const pdpnidNewHost);

HRESULT	DNPerformHostMigration1(DIRECTNETOBJECT *const pdnObject,
								const DPNID dpnidOldHost);

HRESULT	DNPerformHostMigration2(DIRECTNETOBJECT *const pdnObject);
HRESULT	DNPerformHostMigration3(DIRECTNETOBJECT *const pdnObject,void *const pMsg);
HRESULT	DNProcessHostMigration1(DIRECTNETOBJECT *const pdnObject,void *const pvMsg);
HRESULT	DNProcessHostMigration2(DIRECTNETOBJECT *const pdnObject,void *const pMsg);
HRESULT	DNProcessHostMigration3(DIRECTNETOBJECT *const pdnObject);
HRESULT DNCompleteOutstandingOperations(DIRECTNETOBJECT *const pdnObject);
HRESULT DNCheckReceivedAllVersions(DIRECTNETOBJECT *const pdnObject);
HRESULT DNCleanUpNameTable(DIRECTNETOBJECT *const pdnObject);

HRESULT	DNSendHostMigrateCompleteMessage(DIRECTNETOBJECT *const pdnObject);

/*********************/

HRESULT DNPIIndicateListenTerminated(void *const pvUserContext,
									 void *const pvEndPtContext,
									 const HRESULT hr);

HRESULT DNPIIndicateEnumQuery(void *const pvUserContext,
							  void *const pvEndPtContext,
							  const HANDLE hCommand,
							  void *const pvEnumQueryData,
							  const DWORD dwEnumQueryDataSize);

HRESULT DNPIIndicateEnumResponse(void *const pvUserContext,
								 const HANDLE hCommand,
								 void *const pvCommandContext,
								 void *const pvEnumResponseData,
								 const DWORD dwEnumResponseDataSize);

HRESULT DNPIIndicateConnect(void *const pvUserContext,
							void *const pvListenContext,
							const HANDLE hEndPt,
							void **const ppvEndPtContext);

HRESULT DNPIIndicateDisconnect(void *const pvUserContext,
							   void *const pvEndPtContext);

HRESULT DNPIIndicateConnectionTerminated(void *const pvUserContext,
										 void *const pvEndPtContext,
										 const HRESULT hr);

HRESULT DNPIIndicateReceive(void *const pvUserContext,
							void *const pvEndPtContext,
							void *const pvData,
							const DWORD dwDataSize,
							const HANDLE hBuffer,
							const DWORD dwFlags);

HRESULT DNPICompleteListen(void *const pvUserContext,
						   void **const ppvCommandContext,
						   const HRESULT hr,
						   const HANDLE hEndPt);

HRESULT DNPICompleteListenTerminate(void *const pvUserContext,
									void *const pvCommandContext,
									const HRESULT hr);

HRESULT DNPICompleteEnumQuery(void *const pvUserContext,
							  void *const pvCommandContext,
							  const HRESULT hr);

HRESULT DNPICompleteEnumResponse(void *const pvUserContext,
								 void *const pvCommandContext,
								 const HRESULT hr);

HRESULT DNPICompleteConnect(void *const pvUserContext,
							void *const pvCommandContext,
							const HRESULT hr,
							const HANDLE hEndPt,
							void **const ppvEndPtContext);

HRESULT DNPICompleteDisconnect(void *const pvUserContext,
							   void *const pvCommandContext,
							   const HRESULT hr);

HRESULT DNPICompleteSend(void *const pvUserContext,
						 void *const pvCommandContext,
						 const HRESULT hr);

HRESULT DNPIAddressInfoConnect(void *const pvUserContext,
							   void *const pvCommandContext,
							   const HRESULT hr,
							   IDirectPlay8Address *const pHostAddress,
							   IDirectPlay8Address *const pDeviceAddress );

HRESULT DNPIAddressInfoEnum(void *const pvUserContext,
							void *const pvCommandContext,
							const HRESULT hr,
							IDirectPlay8Address *const pHostAddress,
							IDirectPlay8Address *const pDeviceAddress );

HRESULT DNPIAddressInfoListen(void *const pvUserContext,
							  void *const pvCommandContext,
							  const HRESULT hr,
							  IDirectPlay8Address *const pDeviceAddress );


/*********************/

//**********************************************************************
// Class prototypes
//**********************************************************************


#endif	// __DNCORE_H__
