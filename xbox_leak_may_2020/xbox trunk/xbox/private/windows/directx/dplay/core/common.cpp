/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Common.cpp
 *  Content:    DNET common interface routines
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/21/99	mjn		Created
 *  12/23/99	mjn		Hand all NameTable update sends from Host to worker thread
 *  12/23/99	mjn		Fixed use of Host and AllPlayers short cut pointers
 *	12/28/99	mjn		Moved Async Op stuff to Async.h
 *	12/29/99	mjn		Reformed DN_ASYNC_OP to use hParentOp instead of lpvUserContext
 *	12/29/99	mjn		Added Instance GUID generation in DN_Host
 *	01/05/00	mjn		Return DPNERR_NOINTERFACE if CoCreateInstance fails
 *	01/06/00	mjn		Moved NameTable stuff to NameTable.h
 *	01/07/00	mjn		Implemented DNSEND_NOCOPY flag in DN_SendTo
 *	01/07/00	mjn		Moved Misc Functions to DNMisc.h
 *	01/08/00	mjn		Implemented GetApplicationDesc
 *	01/09/00	mjn		Application Description stuff
 *	01/10/00	mjn		Implemented SetApplicationDesc
 *	01/11/00	mjn		Use CPackedBuffers instead of DN_ENUM_BUFFER_INFOs
 *	01/13/00	mjn		Removed DIRECTNETOBJECT from Pack/UnpackApplicationDesc
 *	01/14/00	mjn		Added pvUserContext to Host API call
 *	01/14/00	mjn		Removed pvUserContext from DN_NAMETABLE_ENTRY
 *	01/15/00	mjn		Replaced DN_COUNT_BUFFER with CRefCountBuffer
 *	01/16/00	mjn		Moved User callback stuff to User.h
 *	01/18/00	mjn		Implemented DNGROUP_AUTODESTRUCT
 *  01/18/00	rmt		Added calls into voice layer for Close
 *	01/19/00	mjn		Replaced DN_SYNC_EVENT with CSyncEvent
 *	01/20/00	mjn		Clean up NameTable operation list in DN_Close
 *	01/23/00	mjn		Added DN_DestroyPlayer and DNTerminateSession
 *	01/28/00	mjn		Added DN_ReturnBuffer
 *	02/01/00	mjn		Added DN_GetCaps, DN_SetCaps
 *	02/01/00	mjn		Implement Player/Group context values
 *	02/09/00	mjn		Implemented DNSEND_COMPLETEONPROCESS
 *	02/15/00	mjn		Implement INFO flags in SetInfo and return context in GetInfo
 *	02/17/00	mjn		Implemented GetPlayerContext and GetGroupContext
 *	02/17/00	mjn		Reordered parameters in EnumServiceProviders,EnumHosts,Connect,Host
 *	02/18/00	mjn		Converted DNADDRESS to IDirectPlayAddress8
 *  03/17/00    rmt     Moved caps funcs to caps.h/caps.cpp
 *	03/23/00	mjn		Set group context through CreateGroup
 *				mjn		Set player context through Host and Connect
 *				mjn		Implemented RegisterLobby()
 *	03/24/00	mjn		Release SP when EnumHost completes
 *  03/25/00    rmt     Added call into DPNSVR when host begins
 *  04/04/00	rmt		Added flag to disable calls to DPNSVR and flag to disable
 *						parameter validation
 *	04/05/00	mjn		Fixed DestroyClient API call
 *	04/06/00	mjn		Added DN_GetHostAddress()
 *	04/07/00	mjn		Prevent Initialize() API from being called twice
 *				mjn		Ensure Host addresses have SP included
 *				mjn		Fixed DN_GetHostAddress() to get address
 *	04/09/00	mjn		Convert DN_Host() and DN_Connect() to use CAsyncOp
 *	04/10/00	mjn		Fixed DN_Close() to use flags
 *	04/11/00	mjn		Use CAsyncOp for ENUMs
 *				mjn		Moved ProcessEnumQuery and ProcessEnumResponse to EnumHosts.cpp
 *				mjn		DNCancelEnum() uses CAsyncOp
 *	04/12/00	mjn		DNTerminateSession() cancels outstanding ENUMs
 *				mjn		DN_Close() cancels ENUMs instead of DNTerminateSession
 *				mjn		DN_Close() clears DN_OBJECT_FLAG_DISCONNECTING
 *  04/13/00	rmt     More parameter validation
 *	04/14/00	mjn		Default Host SP to Device SP if not specified
 *				mjn		Crack LISTENs in DN_Host for DPNSVR
 *	04/16/00	mjn		DNSendMessage uses CAsyncOp
 *	04/17/00	mjn		DNCancelSend() uses CAsyncOp
 *	04/17/00	mjn		Fixed DN_EnumHosts to use Handle parent and SYNC operation
 *				mjn		DN_Close tries to cancel SENDs and ENUMs
 *	04/18/00	mjn		CConnection tracks connection status better
 *				mjn		Deinitialize HandleTable, and release addresses in DN_Close
 *				mjn		Fixed DN_GetApplicationDesc to return correct size
 *	04/19/00	mjn		Changed DN_SendTo to accept a range of DPN_BUFFER_DESCs and a count
 *				mjn		Shut down LISTENs earlier in Close sequence
 *	04/20/00	mjn		Convert ReceiveBuffers to CAsyncOp and reclaim at Close
 *				mjn		DN_SendTo() may be invoked by IDirectPlay8Client::DN_Send()
 *	04/23/00	mjn		Added parameter to DNPerformChildSend()
 *				mjn		Reimplemented SEND_COMPLETEONPROCESS
 *	04/24/00	mjn		Updated Group and Info operations to use CAsyncOp's
 *	04/25/00	mjn		Added DNCancelConnect to DN_CancelAsyncOperation
 *	04/26/00	mjn		Fixed DN_GetSendQueueInfo to use CAsyncOp's
 *	04/26/00	mjn		Removed DN_ASYNC_OP and related functions
 *	04/27/00	mjn		Cause DN_GetPlayerContext()/DN_GetGroupContext() to fail if disconnecting
 *	04/28/00	mjn		Allow a NULL Device Address in DN_Connect() - steal SP from Host Address
 *				mjn		Save user connect data to be passed during CONNECT sequence
 *				mjn		Prevent infinite loops in group SENDs
 *  05/01/00    rmt     Bug #33403 - Require DPNSESSION_CLIENT_SERVER mode in client/server mode
 *	05/01/00	mjn		Prevent unusable SPs from being enumerated.
 *	05/03/00	mjn		Use GetHostPlayerRef() rather than GetHostPlayer()
 *	05/04/00	mjn		Clean up address list in DN_Host()
 *	05/05/00	mjn		Free pvConnectData in DN_Close()
 *				mjn		Fixed leak in DN_GetHostAddress()
 *	05/16/00	mjn		Force return code from ASYNC DN_SendTo() to return DPNERR_PENDING
 *				mjn		Better locking for User notifications
 *	05/30/00	mjn		Modified logic for group sends to target connected players only
 *				mjn		ASSERT if operations cannot be cancelled in DN_Close()
 *	05/31/00	mjn		Added operation specific SYNC flags
 *	05/31/00	mjn		Prevent ALL_PLAYERS group from being enum'd in DN_EnumClientsAndGroups()
 *				mjn		Skip invalid Host addresses
 *	06/05/00	mjn		Fixed DN_SendTo to handle some errors gracefully
 *	06/19/00	mjn		DN_Connect and DN_Host enumerate adapters if ALL_ADAPTERS specified
 *	06/20/00	mjn		Fixed DN_GetHostAddress() to extract host address from LISTENs
 *				mjn		DOH!  Forgot to change a line in DN_Host() so that LISTENs use enum'd adapter rather than ALL_ADAPTER
 *	06/22/00	mjn		Replace CConnection::MakeConnected() with SetStatus()
 *	06/23/00	mjn		Removed dwPriority from DN_SendTo()
 *	06/24/00	mjn		Added parent CONNECT AsyncOp to DN_Connect()
 *				mjn		Return DPNERR_UNINITIALIZED from DN_Close() if not initialized (called twice)
 *	06/25/00	mjn		Added DNUpdateLobbyStatus(), update status for CONNECTED,DISCONNECTED
 *				mjn		Set DirectNetObject as CONNECTED earlier in DN_Host()
 *	06/26/00	mjn		Replaced DPNADDCLIENTTOGROUP_SYNC DPNADDPLAYERTOGROUP_SYNC
 *				mjn		Replaced DPNREMOVECLIENTFROMGROUP_SYNC with DPNREMOVEPLAYERFROMGROUP_SYNC
 *				mjn		Fixed for() loop counter problem - nested counters used the same variable - DOH !
 *	06/27/00	mjn		Allow priorities to be specified to GetSendQueueInfo() API calls
 *				rmt		Added abstraction for COM_Co(Un)Initialize
 *				mjn		Removed ASSERT player not found in DN_GetPlayerContext()
 *				mjn		Added DPNSEND_NONSEQUENTIAL flag to Send/SendTo
 *				mjn		Allow mix-n-match of priorities in GetSendQueueInfo() API call
 *	07/02/00	mjn		Modified DN_SendTo() to use DNSendGroupMessage()
 *	07/06/00	mjn		Added missing completions for group sends
 *				mjn		Fixed locking problem in CNameTable::MakeLocalPlayer,MakeHostPlayer,MakeAllPlayersGroup
 *				mjn		Turned DPNSEND_NONSEQUENTIAL flag back on in DN_SendTo()
 *				mjn		Use SP handle instead of interface
 *	07/07/00	mjn		Cleanup pNewHost on DirectNetObject at close
 *	07/08/00	mjn		Fixed CAsyncOp to contain m_bilinkParent
 *  07/09/00	rmt		Bug #38323 - RegisterLobby needs a DPNHANDLE parameter.
 *	07/11/00	mjn		Added NOLOOPBACK capability to group sends
 *				mjn		Fixed DN_EnumHosts() to handle multiple adapters
 *	07/12/00	mjn		Copy user data on EnumHosts()
 *	07/17/00	mjn		Removed redundant SyncEvent->Reset() in DN_Initialize
 *				mjn		Clear DN_OBJECT_FLAG_HOST_CONNECTED in DN_Close()
 *				mjn		Check correct return value of DNSendGroupMessage() in DN_SendTo()
 *  07/19/00    aarono	Bug#39751 add CancelAsyncOperation flag support.
 *	07/20/00	mjn		Cleaned up DN_Connect()
 *  07/21/00    RichGr  IA64: Use %p format specifier for 32/64-bit pointers.
 *	07/21/00	mjn		Cleaned up DN_Close() to release connection object reference
 *	07/23/00	mjn		Moved assertion in DN_CanceAsyncOperation
 *	07/25/00	mjn		Fail DN_EnumHosts() if no valid device adapters exist
 *	07/26/00	mjn		Fix error codes returned from DN_Connect(),DN_GetSendQueueInfo(),DN_DestroyGroup()
 *						DN_AddClientToGroup(),DN_RemoveClientFromGroup(),DN_SetGroupInfo()
 *						DN_GetGroupInfo(),DN_EnumGroupMembers()
 *				mjn		Fix DN_GetApplicationDesc() to always return buffer size
 *	07/26/00	mjn		Fixed locking problem with CAsyncOp::MakeChild()
 *	07/28/00	mjn		Revised DN_GetSendQueueInfo() to use queue info on CConnection objects
 *				mjn		Cleaned up DN_GetPlayerContext() and DN_GetGroupContext()
 *	07/29/00	mjn		Better clean up of pending Connect()'s during Close()
 *				mjn		Check user data size in DN_EnumHosts()
 *				mjn		Added fUseCachedCaps to DN_SPEnsureLoaded()
 *	07/30/00	mjn		Replaced DN_NAMETABLE_PENDING_OP with CPendingDeletion
 *	07/31/00	mjn		Added hrReason to DNTerminateSession()
 *				mjn		Added dwDestroyReason to DNHostDisconnect()
 *				mjn		Cleaned up DN_TerminateSession()
 *  08/03/00	rmt		Bug #41244 - Wrong return codes -- part 2 
 *  08/05/00    RichGr  IA64: Use %p format specifier in DPFs for 32/64-bit pointers and handles.
 *	08/05/00	mjn		Added pParent to DNSendGroupMessage and DNSendMessage()
 *				mjn		Ensure cancelled operations don't proceed
 *				mjn		Prevent hosting players from calling DN_EnumHosts()
 *				mjn		Generate TERMINATE_SESSION notification for host player
 *	08/06/00	mjn		Added CWorkerJob
 *	08/09/00	mjn		Moved no-loop-back test in DN_SendTo()
 *  08/15/00	rmt		Bug #42506 - DPLAY8: LOBBY: Automatic connection settings not being sent
 *	08/15/00	mjn		Remapped DPNERR_INVALIDENDPOINT from DPNERR_INVALIDPLAYER to DPNERR_NOCONNECTION in DN_SendTo()
 *				mjn		Addef hProtocol to DNRegisterWithDPNSVR() and removed registration from DN_Host()
 *	08/16/00	mjn		Return DPNERR_INVALIDHOSTADDRESS from DN_EnumHosts() if host address is invalid
 *	08/20/00	mjn		Removed fUseCachedCaps from DN_SPEnsureLoaded()
 *	08/23/00	mjn		Flag DirectNetObject as registered with DPNSVR
 *	08/24/00	mjn		Replace DN_NAMETABLE_OP with CNameTableOp
 *	08/29/00	mjn		Remap DPNERR_INVALIDPLAYER to DPNERR_CONNECTIONLOST in DN_SendTo()
 *	09/01/00	masonb		Modified DN_Close to call CloseHandle on hWorkerThread
 *	09/04/00	mjn		Added CApplicationDesc
 *	09/05/00	mjn		Set NameTable DPNID mask when hosting
 *	09/06/00	mjn		Fixed register with DPNSVR problem
 *  09/13/00	rmt		Bug #44625 - DPVOICE: Multihomed machines are not always enumerable
 *						Moved registration for DPNSVR into ListenComplete
 *	09/17/00	mjn		Split CNameTable.m_bilinkEntries into m_bilinkPlayers and m_bilinkGroups
 *	10/11/00	mjn		Take locks for CNameTableEntry::PackInfo()
 *	10/12/00	mjn		Set async handle completion after succeeding in DN_EnumHosts()
 *	10/17/00	mjn		Fixed clean up for unreachable players
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dncorei.h"
#include <xboxverp.h>

extern "C"
{
#pragma comment(linker, "/include:_DPlayBuildNumber")

#pragma data_seg(".XBLD$V")

#ifdef ENABLE_DPLAY_VOICE
#if DBG
unsigned short DPlayBuildNumber[8] = { 'D' | ('P' << 8), 'L' | ('A' << 8), 'Y' | ('8' << 8), 'V' | ('D' << 8),
                                        VER_PRODUCTVERSION | 0x8000 };
#else  // DBG
unsigned short DPlayBuildNumber[8] = { 'D' | ('P' << 8), 'L' | ('A' << 8), 'Y' | ('8' << 8), 'V',
                                        VER_PRODUCTVERSION };
#endif // DBG
#else  // ENABLE_DPLAY_VOICE
#if DBG
unsigned short DPlayBuildNumber[8] = { 'D' | ('P' << 8), 'L' | ('A' << 8), 'Y' | ('8' << 8), 'D',
                                        VER_PRODUCTVERSION | 0x8000 };
#else  // DBG
unsigned short DPlayBuildNumber[8] = { 'D' | ('P' << 8), 'L' | ('A' << 8), 'Y' | ('8' << 8), 0,
                                        VER_PRODUCTVERSION };
#endif // DBG
#endif // ENABLE_DPLAY_VOICE

#pragma data_seg()
}


#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_CORE


//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

//**********************************************************************
// Variable definitions
//**********************************************************************

extern DN_PROTOCOL_INTERFACE_VTBL g_ProtocolVTBL;
extern void GlobalInitialize(void);
extern HRESULT GlobalCleanup(void);
extern CThreadPool *g_pThreadPool2;
extern DWORD g_dwGlobalObjectCount;
extern BOOL g_fGlobalObjectInit;
#ifdef DPLAY_DOWORK_STATEMN
extern STMN_ID g_stmnID;
#endif

// BUGBUG: [mgere] [xbox] temporary fix for Close() call blocking
DIRECTNETOBJECT *g_DPlayInterfaces[16] = { NULL };
DNCRITICAL_SECTION g_csDPlayInterfaces;


//**********************************************************************
// Function prototypes
//**********************************************************************

HRESULT DNGetHostAddressHelper(DIRECTNETOBJECT *pdnObject, 
							   IDirectPlay8Address **const prgpAddress,
							   DWORD *const pcAddress);


// Global DPlay Initialization
#undef DPF_MODNAME
#define DPF_MODNAME "DPlayInitialize"

HRESULT WINAPI DPlayInitialize(DWORD dwMaxMemUsage)
{
	g_dwMaxMem = dwMaxMemUsage;

	GlobalInitialize();

        if (!DNInitializeCriticalSection(&g_csDPlayInterfaces))
        {
            DPFERR( "Failed to initialize global critical section" );
            return E_FAIL;
        }

	return S_OK;
}

// Global DPlay Cleanup
#undef DPF_MODNAME
#define DPF_MODNAME "DPlayCleanup"

HRESULT WINAPI DPlayCleanup(void)
{
        DNDeleteCriticalSection(&g_csDPlayInterfaces);

	return GlobalCleanup();
}


#ifdef DPLAY_DOWORK_STATEMN

// State machine handler function
#undef DPF_MODNAME
#define DPF_MODNAME "StateMachineHandler"

HRESULT StateMachineHandler(PSTMN_ASYNC pstmnAsync, DWORD dwFlags)
{
	PDPLAY_STMN_ASYNC pdplaystmnAsync = (PDPLAY_STMN_ASYNC) pstmnAsync;

	pdplaystmnAsync->pDplayObject->DoWork(0);

	return S_OK;
}

#endif


/****************************************************************************
 *
 *  DirectPlay8Create
 *
 *  Description:
 *      Creates and initializes a DirectPlay8Peer object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_MODNAME
#define DPF_MODNAME "DirectPlay8Create"
HRESULT WINAPI
DirectPlay8Create
(
    DWORD dwIID,
    void **ppvInterface,
    void **pUnknown
)
{
    DIRECTNETOBJECT *          pDirectPlay8        = NULL;
    HRESULT                 hr                  = DPN_OK;

    RIP_ON_NOT_TRUE("DirectPlay8Create() pUnknown must be NULL", pUnknown == NULL );
    
    if (g_fGlobalObjectInit == FALSE)
    {
        return E_FAIL;
    }
    
    // Create the DIRECTNETOBJECT object
    if(SUCCEEDED(hr))
    {
        switch (dwIID)
        {
            case IID_IDirectPlay8Peer:
            {
                hr = (pDirectPlay8 = new DIRECTNETOBJECT(DN_OBJECT_FLAG_PEER)) ? S_OK : E_OUTOFMEMORY;
                break;
            }

            case IID_IDirectPlay8Client:
            {
                hr = (pDirectPlay8 = new DIRECTNETOBJECT(DN_OBJECT_FLAG_CLIENT)) ? S_OK : E_OUTOFMEMORY;
                break;
            }

            case IID_IDirectPlay8Server:
            {
                hr = (pDirectPlay8 = new DIRECTNETOBJECT(DN_OBJECT_FLAG_SERVER)) ? S_OK : E_OUTOFMEMORY;
                break;
            }

            default:
            {
                RIP("DirectPlay8Create() dwIID must be one of IID_IDirectPlay8Peer | IID_IDirectPlay8Client | IID_IDirectPlay8Server");
                return E_FAIL;
            }
        }

    }

    // Success
    if(SUCCEEDED(hr))
    {
        hr = pDirectPlay8->ObjectInit();
    }

    // Success
    if(SUCCEEDED(hr))
    {
        *ppvInterface = ADDREF(pDirectPlay8);
        g_dwGlobalObjectCount += 1;
    }

    DNEnterCriticalSection(&g_csDPlayInterfaces);
    for (DWORD i = 0; i < 16; i += 1)
    {
        if (g_DPlayInterfaces[i] == NULL)
        {
            g_DPlayInterfaces[i] = pDirectPlay8;
            break;
        }
    }
    DNLeaveCriticalSection(&g_csDPlayInterfaces);

    // Clean up
    RELEASE(pDirectPlay8);

    return hr;
}




//
//	Store the user-supplied message handler and context value for call backs
//

#undef DPF_MODNAME
#define DPF_MODNAME "DN_Initialize"

HRESULT DIRECTNETOBJECT::DN_Initialize(PVOID pInterface,
						   PVOID const pvUserContext,
						   const PFNDPNMESSAGEHANDLER pfn,
						   const DWORD dwFlags)
{
	HRESULT				hResultCode = DPN_OK;
	DIRECTNETOBJECT		*pdnObject = this;
#ifndef DPLAY_DOWORK
	DWORD				dwThreadID;
#endif
	CSyncEvent			*pSyncEvent;
	BOOL				fApplicationDesc;
	BOOL				fNameTable;
	BOOL				fHandleTable;
	BOOL				fProtocol;

	DPF(2,"Parameters: pInterface [0x%p], pvUserContext [0x%p], pfn [0x%p], dwFlags [0x%lx]",
			pInterface,pvUserContext,pfn,dwFlags);

	RIP_ON_NOT_TRUE("Initialize() You must specify a callback function", pfn != NULL );
    	RIP_ON_NOT_TRUE("Initialize() Invalid flags specified", dwFlags == 0 );

    	// Ensure not already initialized
	RIP_ON_NOT_TRUE("Initialize() Initialize has already been called", !(pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED) );

	fApplicationDesc = FALSE;
	fNameTable = FALSE;
	fHandleTable = FALSE;
	fProtocol = FALSE;
	pSyncEvent = NULL;

	//
	//	Lock DirectNetObject in case someone's trying something funny
	//
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);

	//
	//	Initialize ApplicationDescription
	//
	if ((hResultCode = pdnObject->ApplicationDesc.Initialize()) != DPN_OK)
	{
		DPFERR("Could not initialize ApplicationDesc");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	fApplicationDesc = TRUE;

	//
	//	Initialize NameTable
	//
	if ((hResultCode = pdnObject->NameTable.Initialize(pdnObject)) != DPN_OK)
	{
		DPFERR("Could not initialize NameTable");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	fNameTable = TRUE;

	//
	//	Initialize HandleTable
	//
	if ((hResultCode = pdnObject->HandleTable.Initialize()) != DPN_OK)
	{
		DPFERR("Could not initialize HandleTable");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	fHandleTable = TRUE;

	//
	//	Initialize protocol and create shut-down event
	//
	DNASSERT(pdnObject->lProtocolRefCount == 0);
	if ((hResultCode = DNPProtocolInitialize( pdnObject, pdnObject->pdnProtocolData, &g_ProtocolVTBL)) != DPN_OK)
	{
		DPFERR("DNPProtocolInitialize() failed");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pdnObject->lProtocolRefCount = 1;
	fProtocol = TRUE;

	DNASSERT( pdnObject->hProtocolShutdownEvent == NULL );
	if ((SyncEventNew(pdnObject,&pSyncEvent)) != DPN_OK)
	{
		DPFERR("Could not get protocol shutdown event");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pdnObject->hProtocolShutdownEvent = pSyncEvent;
	pSyncEvent = NULL;

#ifdef DPLAY_DOWORK
#ifdef DPLAY_DOWORK_STATEMN
	//
	// Xbox:  Add task to state machine handler
	//
	m_stmnAsync.stmnAsync.stmnid		= g_stmnID;
	m_stmnAsync.stmnAsync.wCurrentState	= 0;
	m_stmnAsync.stmnAsync.dwWorkFlags		= XON_ASYNCWORK_DPLAY;
	m_stmnAsync.stmnAsync.hr			= S_OK;
    m_stmnAsync.pDplayObject            = this;

	hResultCode = XONCreateAsyncTask( (PSTMN_ASYNC)&m_stmnAsync );
	if (FAILED(hResultCode))
	{
		// BUGBUG: [mgere] [xbox] Remove this once we get statemachine working
		__asm int 3;
		// E_OUTOFMEMORY can be returned...
	}
#endif
	DNASSERT( pdnObject->m_hWorkerCloseEvent == NULL );
	if ((SyncEventNew(pdnObject,&pSyncEvent)) != DPN_OK)
	{
		DPFERR("Could not get worker close event");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pdnObject->m_hWorkerCloseEvent = pSyncEvent;
	pSyncEvent = NULL;
#else
	//
	//	Spawn worker thread
	//
	DPF(3,"Spawning worker thread");
	if ((pdnObject->hWorkerThread = CreateThread(NULL,0,DNWorkerThreadProc,pdnObject,0,
			&dwThreadID)) == NULL)
	{
		DPFERR("DNPProtocolInitialize() failed");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	DPF(5,"Worker thread id [0x%lx]",dwThreadID);
#endif

	pdnObject->pfnDnUserMessageHandler = pfn;
	pdnObject->pvUserContext = pvUserContext;
	pdnObject->dwFlags |= DN_OBJECT_FLAG_INITIALIZED;

Exit:
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (fApplicationDesc)
	{
		pdnObject->ApplicationDesc.Deinitialize();
	}
	if (fNameTable)
	{
		pdnObject->NameTable.Deinitialize();
	}
	if (fHandleTable)
	{
		pdnObject->HandleTable.Deinitialize();
	}
	if (pSyncEvent)
	{
		pSyncEvent->ReturnSelfToPool();
		pSyncEvent = NULL;
	}
	if (fProtocol)
	{
		DNProtocolRelease(pdnObject);
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_Close"

HRESULT DIRECTNETOBJECT::DN_Close(PVOID pInterface,const DWORD dwFlags)
{
	HRESULT				hResultCode = DPN_OK;
	DIRECTNETOBJECT		*pdnObject = this;
	CBilink				*pBilink;
	CAsyncOp			*pAsyncOp;
	CConnection			*pConnection;
	CPendingDeletion	*pPending;
	BOOL				fWaitForEvent;

	DPF(2,"Parameters: pInterface [0x%p], dwFlags [0x%lx]",pInterface,dwFlags);

	RIP_ON_NOT_TRUE( "Close() Invalid flags", dwFlags == 0 );

	// Ensure already initialized
    	RIP_ON_NOT_TRUE( "Close() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

	pAsyncOp = NULL;
	pConnection = NULL;

    //
	//	Flag as closing.   Make sure this hasn't already been called.
	//
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	
	if (pdnObject->dwFlags & DN_OBJECT_FLAG_CLOSING)
	{
		DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
		DPFERR("Already closing" );
		hResultCode = DPNERR_ALREADYCLOSING;
		goto Failure;
	}
	
	pdnObject->dwFlags |= DN_OBJECT_FLAG_CLOSING;
	if (pdnObject->dwLockCount == 0)
	{
		fWaitForEvent = FALSE;
	}
	else
	{
		fWaitForEvent = TRUE;
	}

	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	// XBOX:  Spin through DoWork() to make sure everything is done before closing
	//
	while (DoWork(0) != S_FALSE);

#ifdef	DEBUG
	{
		CNameTableEntry	*pLocalPlayer;

		if (pdnObject->NameTable.GetLocalPlayerRef(&pLocalPlayer) == DPN_OK)
		{
			DPF(0,"Local player was [0x%lx]",pLocalPlayer->GetDPNID());
			pLocalPlayer->Release();
		}
	}
#endif

	//
	//	If there are operations underway, we will wait for them to complete and release the lock count
	//
	if (fWaitForEvent)
	{
		if (WaitForSingleObject(pdnObject->hLockEvent,INFINITE) != WAIT_OBJECT_0)
		{
			DPFERR("WaitForSingleObject() terminated strangely");
			DNASSERT(FALSE);
		}
	}

/*	REMOVE
	//
	//	Shut down listen(s)
	//
	DPF(3,"Checking LISTENs");
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	pAsyncOp = pdnObject->pListenParent;
	pdnObject->pListenParent = NULL;
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	if (pAsyncOp)
	{
		DPF(3,"Canceling LISTENs");
		hResultCode = DNCancelChildren(pdnObject,pAsyncOp);
		DPF(3,"Canceling LISTENs returned [0x%lx]",hResultCode);

		pAsyncOp->Release();
		pAsyncOp = NULL;
	}
*/
	//
	//	Cancel connect
	//
	DPF(3,"Checking CONNECT");
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	pAsyncOp = pdnObject->pConnectParent;
	pdnObject->pConnectParent = NULL;
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	if (pAsyncOp)
	{
		pAsyncOp->Lock();
		pConnection = pAsyncOp->GetConnection();
		pAsyncOp->SetConnection( NULL );
		pAsyncOp->Unlock();

		DPF(3,"Canceling CONNECT");
		hResultCode = DNCancelChildren(pdnObject,pAsyncOp);
		DPF(3,"Canceling CONNECT returned [0x%lx]",hResultCode);

		pAsyncOp->Release();
		pAsyncOp = NULL;

		if (pConnection)
		{
			pConnection->Disconnect();
			pConnection->Release();
			pConnection = NULL;
		}
	}

	//
	//	Remove outstanding ENUMs, SENDs, RECEIVE_BUFFERs
	//
	DPF(3,"Canceling outstanding operations");
	hResultCode = DNCancelActiveCommands(pdnObject,(  DN_CANCEL_FLAG_ENUM_QUERY
													| DN_CANCEL_FLAG_ENUM_RESPONSE
													| DN_CANCEL_FLAG_USER_SEND
													| DN_CANCEL_FLAG_INTERNAL_SEND
													| DN_CANCEL_FLAG_RECEIVE_BUFFER ) );
	DPF(3,"Canceling outstanding operations returned [0x%lx]",hResultCode);

	//
	//	Cancel any REQUESTs
	//
	DPF(3,"Canceling requests");
	hResultCode = DNCancelRequestCommands(pdnObject);
	DPF(3,"Canceling requests returned [0x%lx]",hResultCode);

	//
	//	Terminate session.  This will remove all players from the NameTable
	//
	DPF(3,"Terminate Session");
	if ((hResultCode = DNTerminateSession(pdnObject,DPN_OK)) != DPN_OK)
	{
		DPFERR("Could not terminate session");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
	}

	//
	//	Disconnect any indicated connections
	//
	DNEnterCriticalSection(&pdnObject->csConnectionList);
	while (pdnObject->m_bilinkIndicated.GetNext() != &pdnObject->m_bilinkIndicated)
	{
		pConnection = CONTAINING_OBJECT(pdnObject->m_bilinkIndicated.GetNext(),CConnection,m_bilinkIndicated);
		pConnection->m_bilinkIndicated.RemoveFromList();
		DNLeaveCriticalSection(&pdnObject->csConnectionList);

		DNASSERT(pConnection->GetDPNID() == 0);

		pConnection->Disconnect();
		pConnection->Release();
		pConnection = NULL;

		DNEnterCriticalSection(&pdnObject->csConnectionList);
	}
	DNLeaveCriticalSection(&pdnObject->csConnectionList);

	//
	//	Release SP's
	//
	DPF(3,"Releasing SPs");
	DN_SPReleaseAll(pdnObject);

	//
	//	Shut down protocol
	//
	DPF(3,"Shutting down Protocol");
	DNProtocolRelease(pdnObject);
	pdnObject->hProtocolShutdownEvent->WaitForEvent(INFINITE, this);
	if ((hResultCode = DNPProtocolShutdown(pdnObject->pdnProtocolData)) != DPN_OK)
	{
		DPFERR("Could not shut down Protocol Layer !");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
	}
	pdnObject->hProtocolShutdownEvent->ReturnSelfToPool();
	pdnObject->hProtocolShutdownEvent = NULL;


#ifdef DPLAY_DOWORK
#ifdef DPLAY_DOWORK_STATEMN
	hResultCode = XONCancelAsyncTask( (PSTMN_ASYNC)&m_stmnAsync );
	if (FAILED(hResultCode))
	{
		// BUGBUG: [mgere] [xbox] Remove this once we get statemachine working
		__asm int 3;
		// E_OUTOFMEMORY can be returned...
	}
#endif
	DNWTTerminate(pdnObject);
	pdnObject->m_hWorkerCloseEvent->WaitForEvent(INFINITE, this);
	pdnObject->m_hWorkerCloseEvent->ReturnSelfToPool();
	pdnObject->m_hWorkerCloseEvent = NULL;
#else
	//
	//	Shut down worker thread
	//
	if (pdnObject->hWorkerThread)
	{
		DPF(3,"Terminating Worker Thread");
		DNWTTerminate(pdnObject);
		WaitForSingleObject(pdnObject->hWorkerThread,INFINITE);
		CloseHandle(pdnObject->hWorkerThread);
		pdnObject->hWorkerThread = NULL;
	}
#endif

	//
	//	Deinitialize HandleTable
	//
	DPF(3,"Deinitializing HandleTable");
	pdnObject->HandleTable.Deinitialize();

	//
	//	Deinitialize NameTable
	//
	DPF(3,"Deinitializing NameTable");
	pdnObject->NameTable.Deinitialize();

	//
	//	Deinitialize ApplicationDescription
	//
	DPF(3,"Deinitializing ApplicationDesc");
	pdnObject->ApplicationDesc.Deinitialize();

	//
	//	Any pending NameTable operations
	//
	pBilink = pdnObject->m_bilinkPendingDeletions.GetNext();
	while (pBilink != &pdnObject->m_bilinkPendingDeletions)
	{
		pPending = CONTAINING_OBJECT(pBilink,CPendingDeletion,m_bilinkPendingDeletions);
		pBilink = pBilink->GetNext();
		pPending->m_bilinkPendingDeletions.RemoveFromList();
		pPending->ReturnSelfToPool();
		pPending = NULL;
	}

	//
	//	Misc Clean Up
	//
	if (pdnObject->pIDP8ADevice)
	{
		pdnObject->pIDP8ADevice->Release();
		pdnObject->pIDP8ADevice = NULL;
	}
	if (pdnObject->pvConnectData)
	{
		DNFree(pdnObject->pvConnectData);
		pdnObject->pvConnectData = NULL;
		pdnObject->dwConnectDataSize = 0;
	}

	if( pdnObject->pConnectAddress )
	{
		pdnObject->pConnectAddress->Release();
		pdnObject->pConnectAddress = NULL;
	}

	if( pdnObject->pTargetList )
	{
		delete [] pdnObject->pTargetList;
		pdnObject->pTargetList = NULL;
	}

	if( pdnObject->pExpandedTargetList )
	{
		delete [] pdnObject->pExpandedTargetList;
		pdnObject->pExpandedTargetList = NULL;
	}	

	//
	//	Reset DirectNet object flag
	//
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	pdnObject->dwFlags &= (~( DN_OBJECT_FLAG_INITIALIZED
							| DN_OBJECT_FLAG_CLOSING
							| DN_OBJECT_FLAG_DISCONNECTING
							| DN_OBJECT_FLAG_HOST_CONNECTED
							| DN_OBJECT_FLAG_LOCALHOST ));
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	goto Exit;
}


//
// XBox: DoWork function.  Spins through work like the threads used to.
//

#undef DPF_MODNAME
#define DPF_MODNAME "DIRECTNETOBJECT::DoWork"

HRESULT DIRECTNETOBJECT::DoWork( const DWORD dwFlags )
{
	HRESULT hr = S_FALSE;
#ifdef DPLAY_DOWORK
	DIRECTNETOBJECT    *pdnObject = this;
	DWORD dwNoMoreWork = 1;

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED) )
    	{
    	    DPF(1,"DoWork:  DPlay object is not initialized." );
    	    DPF_RETURN( DPNERR_UNINITIALIZED );
    	}    	

	DNEnterCriticalSection(&pdnObject->csDirectNetObject);

        PeriodicTimer();
	while (TimerWorkerThread() == 0);

	if (g_pThreadPool2 != NULL)
	{
		dwNoMoreWork = g_pThreadPool2->PrimaryWin9xThread();
	}

	if (dwNoMoreWork)
	{
	dwNoMoreWork = DNWorkerThreadProc();
	}

	if ( dwNoMoreWork )
	{
		hr = S_FALSE;
	}
	else
	{
		hr = S_OK;
	}

	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
#endif

	return hr;
}


//
// Enumerate SP's if no SPGUID supplied, or SP Adapters if an SPGUID is supplied
//

#undef DPF_MODNAME
#define DPF_MODNAME "DN_EnumServiceProviders"

HRESULT DIRECTNETOBJECT::DN_EnumServiceProviders( PVOID pInterface,
									  const GUID *const pguidServiceProvider,
									  const GUID *const pguidApplication,
									  DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer,
									  DWORD *const pcbEnumData,
									  DWORD *const pcReturned,
									  const DWORD dwFlags )
{
	HRESULT		        hResultCode;
	DIRECTNETOBJECT    *pdnObject = this;

	DPF(2,"Parameters: pInterface [0x%p], pguidServiceProvider [0x%p], pguidApplication [0x%p], pSPInfoBuffer [0x%p], pcbEnumData [0x%p], pcReturned [0x%p], dwFlags [0x%lx]",
		pInterface,pguidServiceProvider,pguidApplication,pSPInfoBuffer,pcbEnumData,pcReturned,dwFlags);

	RIP_ON_NOT_TRUE( "EnumServiceProviders() Error validating params", SUCCEEDED( DN_ValidateEnumServiceProviders( pInterface, pguidServiceProvider, pguidApplication,
                                                                       pSPInfoBuffer, pcbEnumData, pcReturned, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "EnumServiceProviders() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    if (pguidServiceProvider == NULL)	// Enumerate all service providers
	{
		hResultCode = DN_EnumSP(pdnObject,dwFlags,pguidApplication,pSPInfoBuffer,pcbEnumData,pcReturned);
	}
	else	// Service provider specified - enumerate adaptors
	{
		hResultCode = DPNERR_UNSUPPORTED;
	}

	DPF(3,"Set: *pcbEnumData [%ld], *pcReturned [%ld]",*pcbEnumData,*pcReturned);

	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


//
//	Cancel an outstanding Async Operation.  hAsyncHandle is the operation handle returned when
//	the operation was initiated.
//

#undef DPF_MODNAME
#define DPF_MODNAME "DN_CancelAsyncOperation"

HRESULT DIRECTNETOBJECT::DN_CancelAsyncOperation(PVOID pvInterface,
									 const DPNHANDLE hAsyncOp,
									 const DWORD dwFlags)
{
	HRESULT				hResultCode;
	CAsyncOp			*pHandleParent;
	CAsyncOp			*pAsyncOp;
	DIRECTNETOBJECT		*pdnObject = this;

	DPF(2,"Parameters: pvInterface [0x%p], hAsyncOp [0x%lx], dwFlags [0x%lx]",
			pvInterface,hAsyncOp,dwFlags);

	RIP_ON_NOT_TRUE( "CancelAsyncOperation() Error validating params", SUCCEEDED( DN_ValidateCancelAsyncOperation( pvInterface, hAsyncOp, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "CancelAsyncOperation() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

	//
	//	If hAsyncOp is specified, we will cancel it.  Otherwise, we will rely on the flags to
	//	determine which operations to cancel.
	//
	if( hAsyncOp )
	{
		//
		//	Cancel single operation
		//

		pHandleParent = NULL;
		pAsyncOp = NULL;

		if ((hResultCode = pdnObject->HandleTable.Find(hAsyncOp,&pHandleParent)) != DPN_OK)
		{
			DPFERR("Invalid USER Handle specified");
			hResultCode = DPNERR_INVALIDHANDLE;
			goto Failure;
		}
		if ( pHandleParent->GetOpType() != ASYNC_OP_USER_HANDLE )
		{
			DPFERR("Invalid USER Handle specified");
			hResultCode = DPNERR_INVALIDHANDLE;
			goto Failure;
		}

		hResultCode = DNCancelChildren(pdnObject,pHandleParent);

		pHandleParent->Release();
		pHandleParent = NULL;
	}
	else
	{
		DWORD	dwInternalFlags;
		HRESULT	hr;

		//
		//	Cancel many operations based on flags
		//

		//
		//	Re-map flags
		//
		dwInternalFlags = 0;
		if (dwFlags & DPNCANCEL_ALL_OPERATIONS)
		{
			dwInternalFlags = (	DN_CANCEL_FLAG_CONNECT | DN_CANCEL_FLAG_ENUM_QUERY | DN_CANCEL_FLAG_USER_SEND );
		}
		else if (dwFlags & DPNCANCEL_CONNECT)
		{
			dwInternalFlags = DN_CANCEL_FLAG_CONNECT;
		}
		else if (dwFlags & DPNCANCEL_ENUM)
		{
			dwInternalFlags = DN_CANCEL_FLAG_ENUM_QUERY;
		}
		else if (dwFlags & DPNCANCEL_SEND)
		{
			dwInternalFlags = DN_CANCEL_FLAG_USER_SEND;
		}
		else
		{
			DNASSERT(FALSE);	// Should never get here
		}
		DPF(3,"Re-mapped internal flags [0x%lx]",dwInternalFlags);

		//
		//	Pre-set error code
		hResultCode = DPN_OK;

		//
		//	To cancel a CONNECT, look at the DirectNetObject
		//
		if (dwInternalFlags & DN_CANCEL_FLAG_CONNECT)
		{
			DNEnterCriticalSection(&pdnObject->csDirectNetObject);
			if (pdnObject->pConnectParent)
			{
				if (pdnObject->pConnectParent->IsChild())
				{
					DNASSERT(pdnObject->pConnectParent->GetParent() != NULL);
					pdnObject->pConnectParent->GetParent()->AddRef();
					pAsyncOp = pdnObject->pConnectParent->GetParent();
				}
				else
				{
					pdnObject->pConnectParent->AddRef();
					pAsyncOp = pdnObject->pConnectParent;
				}
			}
			DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

			if (pAsyncOp)
			{
				DPF(3,"Canceling CONNECT");
				hr = DNCancelChildren(pdnObject,pAsyncOp);
				if (hr != DPN_OK)
				{
					hResultCode = DPNERR_CANNOTCANCEL;
					DPF(7,"Remapping: [0x%lx] returned by DNCancelChildren to: [0x%lx]",hr, hResultCode);
				}
				pAsyncOp->Release();
				pAsyncOp = NULL;
			}
		}

		//
		//	To cancel ENUMs and SENDs, cancel out of the active list
		//
		if (dwInternalFlags & (DN_CANCEL_FLAG_ENUM_QUERY | DN_CANCEL_FLAG_USER_SEND))
		{
			DPF(3,"Canceling ENUMs and SENDs");
			hr = DNCancelActiveCommands(pdnObject,dwInternalFlags);
			if (hr != DPN_OK)
			{
				hResultCode = DPNERR_CANNOTCANCEL;
				DPF(7,"Remapping: [0x%lx] returned by DNCancelActiveCommands to: [0x%lx]",hr, hResultCode);
			}
		}
	}

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pHandleParent)
	{
		pHandleParent->Release();
		pHandleParent = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_Connect"

HRESULT DIRECTNETOBJECT::DN_Connect( PVOID pInterface,
						 const DPN_APPLICATION_DESC *const pdnAppDesc,
						 IDirectPlay8Address *const pHostAddr,
						 IDirectPlay8Address *const pDeviceInfo,
						 const DPN_SECURITY_DESC *const pdnSecurity,
						 const DPN_SECURITY_CREDENTIALS *const pdnCredentials,
						 const void *const pvUserConnectData,
						 const DWORD dwUserConnectDataSize,
						 void *const pvPlayerContext,
						 void *const pvAsyncContext,
						 DPNHANDLE *const phAsyncHandle,
						 const DWORD dwFlags)
{
	CAsyncOp			*pHandleParent;
	CAsyncOp			*pConnectParent;
	CAsyncOp			*pAsyncOp;
	HRESULT				hResultCode;
	DIRECTNETOBJECT		*pdnObject = this;
	CSyncEvent			*pSyncEvent;
	HRESULT	volatile	hrOperation;
	IDirectPlay8Address	*pIHost;
	IDirectPlay8Address	*pIDevice;
	IDirectPlay8Address	*pIAdapter;
	DWORD				dwConnectFlags;
//	DWORD				dw;
	GUID				guidSP;
	GUID				guidAdapter;
	void				*pvConnectData;
	void				*pvAdapterBuffer;
//	DWORD				dwAdapterBufferSize;
//	DWORD				dwAdapterBufferCount;
//	DPN_SERVICE_PROVIDER_INFO	*pSPInfo;
	DPN_SP_CAPS			dnSPCaps;
	DN_CONNECT_OP_DATA	*pConnectOpData;
	CRefCountBuffer		*pReply;

	DPF(2,"Parameters: pInterface [0x%p], pdnAppDesc [0x%p], pHostAddr [0x%p], pDeviceInfo [0x%p], pdnSecurity [0x%p], pdnCredentials [0x%p], pvUserConnectData [0x%p], dwUserConnectDataSize [%ld], pvAsyncContext [0x%p], phAsyncHandle [0x%p], dwFlags [0x%lx]",
		pInterface,pdnAppDesc,pHostAddr,pDeviceInfo,pdnSecurity,pdnCredentials,pvUserConnectData,dwUserConnectDataSize,pvAsyncContext,phAsyncHandle,dwFlags);

	RIP_ON_NOT_TRUE( "Connect() Error validating connect params", SUCCEEDED( DN_ValidateConnect( pInterface, pdnAppDesc, pHostAddr, pDeviceInfo,
                                                          pdnSecurity, pdnCredentials, pvUserConnectData,
                                                          dwUserConnectDataSize,pvPlayerContext,
                                                          pvAsyncContext,phAsyncHandle,dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "Connect() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	// Check to ensure not already connected/connecting
    	DNEnterCriticalSection(&pdnObject->csDirectNetObject);

    	if (pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING)
    	{
    		DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
    		DPFERR( "Object is already connecting" );
    		DPF_RETURN(DPNERR_CONNECTING);
    	}
    	if (pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED)
    	{
    		DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
    		DPFERR( "Object is already connected" );
    		DPF_RETURN(DPNERR_ALREADYCONNECTED);
    	}
		if (pdnObject->dwFlags & (DN_OBJECT_FLAG_CLOSING | DN_OBJECT_FLAG_DISCONNECTING))
		{
    		DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
    		DPFERR( "Object is closing or disconnecting" );
    		DPF_RETURN(DPNERR_ALREADYCLOSING);
		}
    	pdnObject->dwFlags |= DN_OBJECT_FLAG_CONNECTING;
    	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
	
	// Preset these so that they are properly cleaned up
	pIHost = NULL;
	pIDevice = NULL;
	pIAdapter = NULL;
	pSyncEvent = NULL;
	pHandleParent = NULL;
	pConnectParent = NULL;
	pAsyncOp = NULL;
	pvConnectData = NULL;
	pvAdapterBuffer = NULL;
	hrOperation = DPNERR_GENERIC;
	pConnectOpData = NULL;
	pReply = NULL;

	if ((hResultCode = pHostAddr->Duplicate(&pIHost)) != DPN_OK)
	{
		DPFERR("Could not duplicate host address");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
		goto Failure;
	}

	//
	//	Duplicate specified Device Address, or create a blank one if NULL
	//
	if (pDeviceInfo != NULL)
	{
		if ((hResultCode = pDeviceInfo->Duplicate(&pIDevice)) != DPN_OK)
		{
			DPFERR("Could not duplicate device info");
			DisplayDNError(0,hResultCode);
			DNASSERT(FALSE);
			goto Failure;
		}
	}
	else
	{
		if ((hResultCode = DirectPlay8AddressCreate( IID_IDirectPlay8Address, (void **) &pIDevice, NULL)) != S_OK)
		{
			DPFERR("Could not create Device Address");
			DisplayDNError(0,hResultCode);
			DNASSERT(FALSE);
			goto Failure;
		}
	}

	//
	//	Get SP caps
	//
	if ((hResultCode = DNGetActualSPCaps(pdnObject,&guidSP,&dnSPCaps)) != DPN_OK)
	{
		DPFERR("Could not get SP caps");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	//
	//	Update DirectNet Application Description
	//
	pdnObject->ApplicationDesc.Lock();
	hResultCode = pdnObject->ApplicationDesc.Update(pdnAppDesc,DN_APPDESCINFO_FLAG_SESSIONNAME|DN_APPDESCINFO_FLAG_PASSWORD|
			DN_APPDESCINFO_FLAG_RESERVEDDATA|DN_APPDESCINFO_FLAG_APPRESERVEDDATA);
	pdnObject->ApplicationDesc.Unlock();

	// Connect flags to Protocol
	dwConnectFlags = 0;

	//
	//	Create parent async op, which will be released when the ENTIRE connection is finished
	//	including nametable transfer and installation on the local machine
	//
	if ((hResultCode = AsyncOpNew(pdnObject,&pConnectParent)) != DPN_OK)
	{
		DPFERR("Could not create AsyncOp");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
		goto Failure;
	}
	pConnectParent->SetOpType( ASYNC_OP_CONNECT );
	pConnectParent->MakeParent();
	pConnectParent->SetContext( pvPlayerContext );
	pConnectParent->SetResult( DPNERR_NOCONNECTION );
	pConnectParent->SetCompletion( DNCompleteConnectOperation );
	pConnectParent->SetReserved(1);

		DPF(5,"Async operation - create handle parent");
		if ((hResultCode = DNCreateUserHandle(pdnObject,&pHandleParent)) != DPN_OK)
		{
			DPFERR("Could not create handle parent");
			DisplayDNError(0,hResultCode);
			DNASSERT(FALSE);
			goto Failure;
		}
		pHandleParent->SetContext( pvAsyncContext );

		pHandleParent->Lock();
		if (pHandleParent->IsCancelled())
		{
			pHandleParent->Unlock();
			pConnectParent->SetResult( DPNERR_USERCANCEL );
			hResultCode = DPNERR_USERCANCEL;
			goto Failure;
		}
		pConnectParent->MakeChild( pHandleParent );
		pHandleParent->Unlock();

	//
	//	We will need a parent op for the CONNECTs to help with clean up when the initial CONNECT stage is done
	//
	if ((hResultCode = AsyncOpNew(pdnObject,&pAsyncOp)) != DPN_OK)
	{
		DPFERR("Could not create CONNECT parent");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
		goto Failure;
	}
	pAsyncOp->SetOpType( ASYNC_OP_CONNECT );
	pAsyncOp->MakeParent();
	pAsyncOp->SetResult( DPNERR_NOCONNECTION );
	pAsyncOp->SetCompletion( DNCompleteConnectToHost );
	pAsyncOp->SetOpFlags( dwConnectFlags );

	pConnectParent->Lock();
	if (pConnectParent->IsCancelled())
	{
		pConnectParent->Unlock();
		pAsyncOp->SetResult( DPNERR_USERCANCEL );
		hResultCode = DPNERR_USERCANCEL;
		goto Failure;
	}
	pAsyncOp->MakeChild( pConnectParent );
	pConnectParent->Unlock();

	//
	//	Save CONNECT data (if supplied)
	//
	if (pvUserConnectData && dwUserConnectDataSize)
	{
		if ((pvConnectData = DNMalloc(dwUserConnectDataSize)) == NULL)
		{
			DPFERR("Could not allocate CONNECT data buffer");
			DNASSERT(FALSE);
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		memcpy(pvConnectData,pvUserConnectData,dwUserConnectDataSize);
	}

	//
	//	Update DirectNet object with relevant data
	//
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	pConnectParent->AddRef();
	pdnObject->pConnectParent = pConnectParent;
	if (pvConnectData)
	{
		pdnObject->pvConnectData = pvConnectData;
		pdnObject->dwConnectDataSize = dwUserConnectDataSize;
	}
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

		if ((pConnectOpData = static_cast<DN_CONNECT_OP_DATA*>(MemoryBlockAlloc(pdnObject,sizeof(DN_CONNECT_OP_DATA)))) == NULL)
		{
			DPFERR("Could not allocate CONNECT op data block");
			hResultCode = DPNERR_OUTOFMEMORY;
			DNASSERT(FALSE);
			goto Failure;
		}
		pConnectOpData->dwNumAdapters = 0;
		pConnectOpData->dwCurrentAdapter = 0;

	pAsyncOp->SetOpData( pConnectOpData );

	pdnObject->pConnectAddress = pIHost;
	pdnObject->pConnectAddress->AddRef();

	//
	//	CONNECT !
	//
	hResultCode = DNPerformConnect(	pdnObject,
									NULL,
									pIDevice,
									pIHost,
									pAsyncOp->GetOpFlags(),
									pAsyncOp);
	if (hResultCode != DPN_OK)
	{
		DPFERR("Could not connect");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	pAsyncOp->Release();
	pAsyncOp = NULL;
	pConnectParent->Release();
	pConnectParent = NULL;

	pIHost->Release();
	pIHost = NULL;
	pIDevice->Release();
	pIDevice = NULL;

		pHandleParent->SetCompletion( DNCompleteUserConnect );
		if (phAsyncHandle)
		{
			*phAsyncHandle = pHandleParent->GetHandle();
		}
		pHandleParent->Release();
		pHandleParent = NULL;

		hResultCode = DPNERR_PENDING;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	DNASSERT(hResultCode != DPNERR_INVALIDENDPOINT);
	return(hResultCode);

Failure:

	if (pHandleParent)
	{
		pHandleParent->Release();
		pHandleParent = NULL;
	}
	if (pConnectParent)
	{
		if (pConnectParent->GetHandle())
		{
			pdnObject->HandleTable.Destroy(pConnectParent->GetHandle());
		}
		pConnectParent->Release();
		pConnectParent = NULL;
	}
	if (pAsyncOp)
	{
		pAsyncOp->Release();
		pAsyncOp = NULL;
	}
	if (pIHost)
	{
		pIHost->Release();
		pIHost = NULL;
	}
	if (pIDevice)
	{
		pIDevice->Release();
		pIDevice = NULL;
	}
	if (pIAdapter)
	{
		pIAdapter->Release();
		pIAdapter = NULL;
	}
	if (pSyncEvent)
	{
		pSyncEvent->ReturnSelfToPool();
		pSyncEvent = NULL;
	}
	if (pvAdapterBuffer)
	{
		DNFree(pvAdapterBuffer);
		pvAdapterBuffer = NULL;
	}

	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	if (pdnObject->pIDP8AEnum)
	{
		pdnObject->pIDP8AEnum->Release();
		pdnObject->pIDP8AEnum = NULL;
	}
	if (pdnObject->pIDP8ADevice)
	{
		pdnObject->pIDP8ADevice->Release();
		pdnObject->pIDP8ADevice = NULL;
	}
	if (pdnObject->pvConnectData)
	{
		DNFree(pdnObject->pvConnectData);
		pdnObject->pvConnectData = NULL;
		pdnObject->dwConnectDataSize = 0;
	}
	if( pdnObject->pConnectAddress )
	{
		pdnObject->pConnectAddress->Release();
		pdnObject->pConnectAddress = NULL;
	}
	pdnObject->dwFlags &= (~DN_OBJECT_FLAG_CONNECTING);
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	goto Exit;
}


//	DN_GetSendQueueInfo
//
//	Get info about the user send queue.
//	This will find the CConnection for a given player and extract the required queue infor from it.

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetSendQueueInfo"

HRESULT DIRECTNETOBJECT::DN_GetSendQueueInfo(PVOID pInterface,
								 const DPNID dpnid,
								 DWORD *const pdwNumMsgs,
								 DWORD *const pdwNumBytes,
								 const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	DWORD				dwQueueFlags;
	DWORD				dwNumMsgs;
	DWORD				dwNumBytes;
	CNameTableEntry     *pNTEntry;
	CConnection			*pConnection;
	HRESULT				hResultCode;

	DPF(2,"Parameters : pInterface [0x%p], dpnid [0x%lx], pdwNumMsgs [0x%p], pdwNumBytes [0x%p], dwFlags [0x%lx]",
		pInterface,dpnid,pdwNumMsgs,pdwNumBytes,dwFlags);

	RIP_ON_NOT_TRUE( "GetSendQueueInfo() Error validating params", SUCCEEDED( DN_ValidateGetSendQueueInfo( pInterface, pdwNumMsgs, pdwNumBytes, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "GetSendQueueInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    		DPFERR( "Object is already connecting" );
    		DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED ) )
    	{
    	    DPFERR("Object is not connected" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}    	

	pNTEntry = NULL;
	pConnection = NULL;

	//
    //	Validate specified player ID and get CConnection
	//
	if((hResultCode = pdnObject->NameTable.FindEntry( dpnid, &pNTEntry )))
	{
		DPF(0,"Could not find Player ID [0x%lx] in NameTable", dpnid );
		if ((hResultCode = pdnObject->NameTable.FindDeletedEntry(dpnid,&pNTEntry)) != DPN_OK)
		{
			DPFERR("Could not find entry in deleted list either");
			hResultCode = DPNERR_INVALIDPLAYER;
			goto Failure;
		}
		pNTEntry->Release();
		pNTEntry = NULL;
		hResultCode = DPNERR_CONNECTIONLOST;
		goto Failure;
	}
	if (pNTEntry->IsLocal() || pNTEntry->IsGroup())
	{
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
	}
	if ((hResultCode = pNTEntry->GetConnectionRef( &pConnection )) != DPN_OK)
	{
		hResultCode = DPNERR_CONNECTIONLOST;
		goto Failure;
	}
	pNTEntry->Release();
	pNTEntry = NULL;

	//
	//	Determine required queues
	//
	dwQueueFlags = dwFlags & (DPNGETSENDQUEUEINFO_PRIORITY_HIGH | DPNGETSENDQUEUEINFO_PRIORITY_NORMAL | DPNGETSENDQUEUEINFO_PRIORITY_LOW);
	if (dwQueueFlags == 0)
	{
		dwQueueFlags = (DPNGETSENDQUEUEINFO_PRIORITY_HIGH | DPNGETSENDQUEUEINFO_PRIORITY_NORMAL | DPNGETSENDQUEUEINFO_PRIORITY_LOW);
	}

	//
	//	Extract required info
	//
	dwNumMsgs = 0;
	dwNumBytes = 0;
	pConnection->Lock();
	if (dwQueueFlags & DPNGETSENDQUEUEINFO_PRIORITY_HIGH)
	{
		dwNumMsgs += pConnection->GetHighQueueNum();
		dwNumBytes += pConnection->GetHighQueueBytes();
	}
	if (dwQueueFlags & DPNGETSENDQUEUEINFO_PRIORITY_NORMAL)
	{
		dwNumMsgs += pConnection->GetNormalQueueNum();
		dwNumBytes += pConnection->GetNormalQueueBytes();
	}
	if (dwQueueFlags & DPNGETSENDQUEUEINFO_PRIORITY_LOW)
	{
		dwNumMsgs += pConnection->GetLowQueueNum();
		dwNumBytes += pConnection->GetLowQueueBytes();
	}
	pConnection->Unlock();
	pConnection->Release();
	pConnection = NULL;

	if (pdwNumMsgs)
	{
		*pdwNumMsgs = dwNumMsgs;
		DPF(3,"Setting: *pdwNumMsgs [%ld]",dwNumMsgs);
	}
	if (pdwNumBytes)
	{
		*pdwNumBytes = dwNumBytes;
		DPF(3,"Setting: *pdwNumBytes [%ld]",dwNumBytes);
	}

	hResultCode = DPN_OK;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	if (pConnection)
	{
		pConnection->Release();
		pConnection = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetApplicationDesc"

HRESULT DIRECTNETOBJECT::DN_GetApplicationDesc(PVOID pInterface,
								   DPN_APPLICATION_DESC *const pAppDescBuffer,
								   DWORD *const pcbDataSize,
								   const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	HRESULT				hResultCode;
	CPackedBuffer		packedBuffer;

	DPF(2,"Parameters: pInterface [0x%p], pAppDescBuffer [0x%p], pcbDataSize [0x%p], dwFlags [0x%lx]",
			pInterface,pAppDescBuffer,pcbDataSize,dwFlags);

	RIP_ON_NOT_TRUE( "GetApplicationDesc() Failed validation getappdesc", SUCCEEDED( DN_ValidateGetApplicationDesc( pInterface, pAppDescBuffer, pcbDataSize, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "GetApplicationDesc() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if (!(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("Object is not connected or hosting" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}    	

	//
	//	Initialize PackedBuffer
	//
	packedBuffer.Initialize(static_cast<void*>(pAppDescBuffer),*pcbDataSize);

	//
	//	Try to pack in the application description.
	//	If it won't fit, the required size will be in the PackedBuffer.
	//
	hResultCode = pdnObject->ApplicationDesc.Pack(&packedBuffer,DN_APPDESCINFO_FLAG_SESSIONNAME|DN_APPDESCINFO_FLAG_PASSWORD|
			DN_APPDESCINFO_FLAG_RESERVEDDATA|DN_APPDESCINFO_FLAG_APPRESERVEDDATA);

	//
	//	Ensure we know what's going on
	//
	if ((hResultCode != DPN_OK) && (hResultCode != DPNERR_BUFFERTOOSMALL))
	{
		DPFERR("Unknown error occurred packing application description");
		DisplayDNError(0,hResultCode);
		hResultCode = DPNERR_GENERIC;
		goto Failure;
	}

	//
	//	Size of buffer
	//
	*pcbDataSize = packedBuffer.GetSizeRequired();

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_SetApplicationDesc"

HRESULT DIRECTNETOBJECT::DN_SetApplicationDesc(PVOID pInterface,
								   const DPN_APPLICATION_DESC *const pdnApplicationDesc,
								   const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	HRESULT				hResultCode = DPN_OK;
	CRefCountBuffer		*pRefCountBuffer;
	CPackedBuffer		packedBuffer;
	CWorkerJob			*pWorkerJob;
	DWORD				dwAppDescInfoSize;
	DWORD				dwEnumFrameSize;
	DPN_BUFFER_DESC		*pdnBufferDesc;
	CNameTableEntry     *pNTEntry;

	DPF(2,"Parameters: pInterface [0x%p], pdnApplicationDesc [0x%p], dwFlags [0x%lx]",
			pInterface,pdnApplicationDesc,dwFlags);

	RIP_ON_NOT_TRUE( "SetApplicationDesc() Error validating setappdesc params", SUCCEEDED( DN_ValidateSetApplicationDesc( pInterface, pdnApplicationDesc, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "SetApplicationDesc() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR( "Object has not yet completed connecting / hosting" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if ( !DN_CHECK_LOCALHOST( pdnObject ) )
    	{
    	    DPFERR("Object is not connected or hosting" );
    	    DPF_RETURN(DPNERR_NOTHOST);
    	}

	pNTEntry = NULL;
	pRefCountBuffer = NULL;
	pWorkerJob = NULL;

	//
	//	This can only be called by the host
	//
	if ((hResultCode = pdnObject->NameTable.GetLocalPlayerRef( &pNTEntry )) != DPN_OK)
	{
		DPFERR("Could not get local player");
		hResultCode = DPNERR_NOTHOST;
		goto Failure;
	}
	if (!pNTEntry->IsHost())
	{
		DPFERR("Not Host player");
		hResultCode = DPNERR_NOTHOST;
		goto Failure;
	}
	pNTEntry->Release();
	pNTEntry = NULL;

	//
	//	Determine max allowable enum frame size by examining the SPCaps for each SP
	//
	if ((hResultCode = DNGetMaxEnumFrameSize(pdnObject,&dwEnumFrameSize)) != DPN_OK)
	{
		DPFERR("Could not determine enum frame size");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	DNASSERT( dwEnumFrameSize >= (sizeof(DN_ENUM_RESPONSE_PAYLOAD) + sizeof(DPN_APPLICATION_DESC_INFO)) );
	if (dwEnumFrameSize < (sizeof(DN_ENUM_RESPONSE_PAYLOAD) + sizeof(DPN_APPLICATION_DESC_INFO) + pdnApplicationDesc->dwApplicationReservedDataSize))
	{
		DPFERR("Not enough room for the application reserved data");
		hResultCode = DPNERR_DATATOOLARGE;
		goto Failure;
	}

	//
	//	Update Host player's application desc first
	//
	pdnObject->ApplicationDesc.Lock();
	if (pdnApplicationDesc->dwMaxPlayers > 0)
	{
		if (pdnApplicationDesc->dwMaxPlayers < pdnObject->ApplicationDesc.GetCurrentPlayers())
		{
			DPFERR("Cannot set max players to less than the current number of players");
			pdnObject->ApplicationDesc.Unlock();
			hResultCode = DPNERR_SESSIONFULL;
			goto Failure;
		}
	}
	hResultCode = pdnObject->ApplicationDesc.Update(pdnApplicationDesc,DN_APPDESCINFO_FLAG_SESSIONNAME|
			DN_APPDESCINFO_FLAG_PASSWORD|DN_APPDESCINFO_FLAG_RESERVEDDATA|DN_APPDESCINFO_FLAG_APPRESERVEDDATA);
	pdnObject->ApplicationDesc.Unlock();
	if (hResultCode != DPN_OK)
	{
		DPFERR("Could not update Application Desciption");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	//
	//	Inform host application
	//
	hResultCode = DNUserUpdateAppDesc(pdnObject);

	//
	//	Get Application Description Info size
	//
	packedBuffer.Initialize(NULL,0);
	hResultCode = pdnObject->ApplicationDesc.PackInfo(&packedBuffer,DN_APPDESCINFO_FLAG_SESSIONNAME|DN_APPDESCINFO_FLAG_PASSWORD|
			DN_APPDESCINFO_FLAG_RESERVEDDATA|DN_APPDESCINFO_FLAG_APPRESERVEDDATA);
	DNASSERT(hResultCode == DPNERR_BUFFERTOOSMALL);
	dwAppDescInfoSize = packedBuffer.GetSizeRequired();

	//
	//	Create packed buffer to send to other players
	//
	if ((hResultCode = RefCountBufferNew(pdnObject,dwAppDescInfoSize,&pRefCountBuffer)) != DPN_OK)
	{
		DPFERR("Could not create CountBuffer");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	packedBuffer.Initialize(pRefCountBuffer->GetBufferAddress(),pRefCountBuffer->GetBufferSize());
	hResultCode = pdnObject->ApplicationDesc.PackInfo(&packedBuffer,DN_APPDESCINFO_FLAG_SESSIONNAME|DN_APPDESCINFO_FLAG_PASSWORD|
			DN_APPDESCINFO_FLAG_RESERVEDDATA|DN_APPDESCINFO_FLAG_APPRESERVEDDATA);
	if (hResultCode != DPN_OK)
	{
		DPFERR("Could not pack Application Description into EnumBuffer");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	//
	//	Notify other players
	//
	DPF(5,"Adding UpdateApplicationDesc to Job Queue");
	if ((hResultCode = WorkerJobNew(pdnObject,&pWorkerJob)) == DPN_OK)
	{
		pWorkerJob->SetJobType( WORKER_JOB_SEND_NAMETABLE_OPERATION );
		pWorkerJob->SetSendNameTableOperationMsgId( DN_MSG_INTERNAL_UPDATE_APPLICATION_DESC );
		pWorkerJob->SetSendNameTableOperationVersion( 0 );
		pWorkerJob->SetSendNameTableOperationDPNIDExclude( 0 );
		pWorkerJob->SetRefCountBuffer( pRefCountBuffer );

		DNQueueWorkerJob(pdnObject,pWorkerJob);
		pWorkerJob = NULL;
	}
	else
	{
		DPFERR("Could not create worker job - ignore and continue");
		DisplayDNError(0,hResultCode);
	}

	pRefCountBuffer->Release();
	pRefCountBuffer = NULL;

	hResultCode = DPN_OK;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	if (pRefCountBuffer)
	{
		pRefCountBuffer->Release();
		pRefCountBuffer = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DNTerminateSession"

HRESULT DNTerminateSession(DIRECTNETOBJECT *const pdnObject,
						   const HRESULT hrReason)
{
	HRESULT		hResultCode;
	BOOL		fWasConnected;
	CAsyncOp	*pAsyncOp;

	DPF(4,"Parameters: hrReason [0x%lx]",hrReason);

	DNASSERT(pdnObject != NULL);
	DNASSERT( (hrReason == DPN_OK) || (hrReason == DPNERR_HOSTTERMINATEDSESSION) || (hrReason == DPNERR_CONNECTIONLOST));

	pAsyncOp = NULL;

	//
	//	Shut down listen(s)
	//
	DPF(3,"Checking LISTENs");
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	pAsyncOp = pdnObject->pListenParent;
	pdnObject->pListenParent = NULL;
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	if (pAsyncOp)
	{
		DPF(3,"Canceling LISTENs");
		hResultCode = DNCancelChildren(pdnObject,pAsyncOp);
		DPF(3,"Canceling LISTENs returned [0x%lx]",hResultCode);

		pAsyncOp->Release();
		pAsyncOp = NULL;
	}

	//
	//	Flag DirectNetObject as disconnecting.  This flag will be cleared when Close() finishes.
	//
	fWasConnected = FALSE;
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	if (pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED)
	{
		fWasConnected = TRUE;
		pdnObject->dwFlags &= (~DN_OBJECT_FLAG_CONNECTED);
	}
#pragma BUGBUG( minara,"How usefull is this DN_OBJECT_FLAG_DISCONNECTING flag ?" )
	pdnObject->dwFlags |= DN_OBJECT_FLAG_DISCONNECTING;
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

#ifdef ENABLE_DPLAY_VOICE
	//
	//	Notify Voice
	//
	Voice_Notify( pdnObject, DVEVENT_STOPSESSION, 0, 0 );
#endif

	//
	//	Remove host migration target
	//
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	if (pdnObject->pNewHost)
	{
		pdnObject->pNewHost->Release();
		pdnObject->pNewHost = NULL;
	}
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	//
	//	Delete all players from NameTable.  This will involve
	//	emptying the table and removing short-cut player pointers
	//
	DPF(5,"Removing players from NameTable");
	pdnObject->NameTable.EmptyTable(hrReason);

	//
	//	Clean up NameTable operation list
	//
	if (pdnObject->dwFlags & DN_OBJECT_FLAG_PEER)
	{
		DPF(5,"Cleaning up NameTable operation list");
		DNNTRemoveOperations(pdnObject,0,TRUE);
	}

	hResultCode = DPN_OK;

	DNASSERT(pAsyncOp == NULL);

	DPF(4,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_SendTo"

HRESULT DIRECTNETOBJECT::DN_SendTo( PVOID pv,
						const DPNID dpnid,
						const DPN_BUFFER_DESC *const prgBufferDesc,
						const DWORD cBufferDesc,
						const DWORD dwTimeOut,
						void *const pvAsyncContext,
						DPNHANDLE *const phAsyncHandle,
						const DWORD dwFlags)
{
	HRESULT				hResultCode;
	HRESULT				hrSend;
	CNameTableEntry		*pNTEntry;
	CNameTableEntry		*pLocalPlayer;
	DIRECTNETOBJECT		*pdnObject = this;
	DPNHANDLE			hParentOp;
	DWORD				dwSendFlags;
	CSyncEvent			*pSyncEvent;
	const DPN_BUFFER_DESC		*pActualBufferDesc;
	CRefCountBuffer		*pRefCountBuffer;
	DPNHANDLE			handle;
	CAsyncOp			*pAsyncOp;
	CAsyncOp			*pParent;
	CAsyncOp			*pHandleParent;
	CConnection			*pConnection;

	DPF(2,"Parameters: dpnid [0x%lx], prgBufferDesc [0x%p], dwTimeOut [%ld], pvAsyncContext [0x%p], phAsyncHandle [0x%p], dwFlags [0x%lx]",
			dpnid,prgBufferDesc,dwTimeOut,pvAsyncContext,phAsyncHandle,dwFlags);

	RIP_ON_NOT_TRUE( "SendTo() Error validating common send params", SUCCEEDED( hResultCode = DN_ValidateSendParams( pv , prgBufferDesc, cBufferDesc, dwTimeOut, pvAsyncContext, phAsyncHandle, dwFlags ) ) );

        // Check to ensure message handler registered
	RIP_ON_NOT_TRUE( "SendTo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR( "Object has not yet completed connecting / hosting" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if (!(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("Object is not connected or hosting" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}

	pRefCountBuffer = NULL;
	pSyncEvent = NULL;
	pNTEntry = NULL;
	pLocalPlayer = NULL;
	pAsyncOp = NULL;
	pParent = NULL;
	pHandleParent = NULL;
	pConnection = NULL;
	handle = 0;

	if (pdnObject->dwFlags & DN_OBJECT_FLAG_CLIENT)
	{
		if ((hResultCode = pdnObject->NameTable.GetHostPlayerRef( &pNTEntry )) != DPN_OK)
		{
			DPFERR("Could not find Host player");
			DisplayDNError(0,hResultCode);
			hResultCode = DPNERR_INVALIDPLAYER;
			goto Failure;
		}
	}
	else
	{
		if (dpnid == 0)
		{
			if ((hResultCode = pdnObject->NameTable.GetAllPlayersGroupRef( &pNTEntry )) != DPN_OK)
			{
				DPFERR("Unable to get all players group");
				DisplayDNError(0,hResultCode);
				hResultCode = DPNERR_INVALIDGROUP;
				goto Failure;
			}
		}
		else
		{
			if (dwFlags & DPNSEND_NOLOOPBACK)
			{
				if ((hResultCode = pdnObject->NameTable.GetLocalPlayerRef( &pLocalPlayer )) != DPN_OK)
				{
					DPFERR("Could not get local player reference");
					DisplayDNError(0,hResultCode);
					hResultCode = DPNERR_GENERIC;
					goto Failure;
				}
				if (dpnid == pLocalPlayer->GetDPNID())
				{
					hResultCode = DPNERR_INVALIDPARAM;
					goto Failure;
				}
				pLocalPlayer->Release();
				pLocalPlayer = NULL;
			}

			if ((hResultCode = pdnObject->NameTable.FindEntry(dpnid,&pNTEntry)) != DPN_OK)
			{
				DPFERR("Unable to find target player or group");
				DisplayDNError(0,hResultCode);
				//
				//	Try deleted list
				//
				if ((hResultCode = pdnObject->NameTable.FindDeletedEntry(dpnid,&pNTEntry)) != DPN_OK)
				{
					DPFERR("Could not find target in deleted list either");
					hResultCode = DPNERR_INVALIDPLAYER;
					goto Failure;
				}
				pNTEntry->Release();
				pNTEntry = NULL;

				//
				//	Target was found, but is not reachable
				//
				hResultCode = DPNERR_CONNECTIONLOST;
				goto Failure;
			}
		}
	}

	if (!(dwFlags & (DPNSEND_NOCOPY | DPNSEND_COMPLETEONPROCESS)))
	{
		if ((hResultCode = RefCountBufferNew(pdnObject,prgBufferDesc->dwBufferSize,&pRefCountBuffer)) != DPN_OK)
		{
			DPFERR("Could not allocate buffer");
			DisplayDNError(0,hResultCode);
			DNASSERT(FALSE);
			goto Failure;
		}
		pActualBufferDesc = pRefCountBuffer->BufferDescAddress();
		memcpy(pActualBufferDesc->pBufferData,prgBufferDesc->pBufferData,prgBufferDesc->dwBufferSize);
	}
	else
	{
		pRefCountBuffer = NULL;
		pActualBufferDesc = prgBufferDesc;
	}

	dwSendFlags = 0;
	if (dwFlags & DPNSEND_GUARANTEED)
	{
		dwSendFlags |= DN_SENDFLAGS_RELIABLE;
	}
	if (dwFlags & DPNSEND_NONSEQUENTIAL)
	{
		dwSendFlags |= DN_SENDFLAGS_NON_SEQUENTIAL;
	}
	if (dwFlags & DPNSEND_PRIORITY_HIGH)
	{
		dwSendFlags |= DN_SENDFLAGS_HIGH_PRIORITY;
	}
	if (dwFlags & DPNSEND_PRIORITY_LOW)
	{
		dwSendFlags |= DN_SENDFLAGS_LOW_PRIORITY;
	}
		//
		//	Create Handle for ASYNC operation
		//
		if ((hResultCode = DNCreateUserHandle(pdnObject,&pHandleParent)) != DPN_OK)
		{
			DPFERR("Could not create AsyncOp");
			DisplayDNError(0,hResultCode);
			DNASSERT(FALSE);
			goto Failure;
		}

		pHandleParent->SetContext( pvAsyncContext );
		pHandleParent->SetStartTime( timeGetTime() );
		handle = pHandleParent->GetHandle();

	if (pNTEntry->IsGroup())
	{
		BOOL	fRequest;
		BOOL	fNoLoopBack;

		//
		//	Perform group sends and get parent AsyncOp
		//
		if (dwFlags & DPNSEND_COMPLETEONPROCESS)
		{
			fRequest = TRUE;
		}
		else
		{
			fRequest = FALSE;
		}
		if (dwFlags & DPNSEND_NOLOOPBACK)
		{
			fNoLoopBack = TRUE;
		}
		else
		{
			fNoLoopBack = FALSE;
		}
		hResultCode = DNSendGroupMessage(	pdnObject,
											pNTEntry,
											DN_MSG_USER_SEND,
											pActualBufferDesc,
											pRefCountBuffer,
											dwTimeOut,
											dwSendFlags,
											fNoLoopBack,
											fRequest,
											pHandleParent,
											&pParent);

		if (hResultCode != DPN_OK)
		{
			DPFERR("SEND failed");
			DisplayDNError(0,hResultCode);
			goto Failure;
		}

			//
			//	Set async completion (if required).  We will only need this if the SEND succeeded.
			//
			if (!(dwFlags & DPNSEND_NOCOMPLETE))
			{
				pHandleParent->SetCompletion( DNCompleteSendHandle );
			}
			pHandleParent->Release();
			pHandleParent = NULL;

		pParent->Release();
		pParent = NULL;
	}
	else
	{
		if ((hResultCode = pNTEntry->GetConnectionRef( &pConnection )) != DPN_OK)
		{
			DPFERR("Could not get Connection reference");
			DisplayDNError(0,hResultCode);
			hResultCode = DPNERR_CONNECTIONLOST;	// re-map this
			goto Failure;
		}

		if (dwFlags & DPNSEND_COMPLETEONPROCESS)
		{
			hResultCode = DNPerformRequest(	pdnObject,
											DN_MSG_INTERNAL_REQ_PROCESS_COMPLETION,
											pActualBufferDesc,
											pConnection,
											pHandleParent,
											&pAsyncOp);
		}
		else
		{
			hResultCode = DNSendMessage(pdnObject,
										pConnection,
										DN_MSG_USER_SEND,
										dpnid,
										pActualBufferDesc,
										pRefCountBuffer,
										dwTimeOut,
										dwSendFlags,
										pHandleParent,
										&pAsyncOp);
		}

		pConnection->Release();
		pConnection = NULL;

		if (hResultCode != DPNERR_PENDING)
		{
			DPFERR("SEND failed");
			DisplayDNError(0,hResultCode);
			if (hResultCode == DPNERR_INVALIDENDPOINT)
			{
				hResultCode = DPNERR_CONNECTIONLOST;
			}
			goto Failure;
		}

			//
			//	Set async completion (if required).  We will only need this if the SEND succeeded.
			//
			if (!(dwFlags & DPNSEND_NOCOMPLETE))
			{
				pHandleParent->SetCompletion( DNCompleteSendHandle );
			}
			pHandleParent->Release();
			pHandleParent = NULL;

		pAsyncOp->Release();
		pAsyncOp = NULL;
	}

	pNTEntry->Release();
	pNTEntry = NULL;

	if (pRefCountBuffer)
	{
		pRefCountBuffer->Release();
		pRefCountBuffer = NULL;
	}

		if (phAsyncHandle != NULL)
		{
			*phAsyncHandle = handle;
		}
		hResultCode = DPNERR_PENDING;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	DNASSERT(hResultCode != DPNERR_INVALIDENDPOINT);
	return(hResultCode);

Failure:
	if (pHandleParent)
	{
		pHandleParent->Release();
		pHandleParent = NULL;
	}
	if (pAsyncOp)
	{
		pAsyncOp->Release();
		pAsyncOp = NULL;
	}
	if (pParent)
	{
		pParent->Release();
		pParent = NULL;
	}
	if (handle != 0)
	{
		pdnObject->HandleTable.Destroy( handle );
		handle = 0;
	}
	if (pSyncEvent)
	{
		pSyncEvent->ReturnSelfToPool();
		pSyncEvent = NULL;
	}
	if (pRefCountBuffer)
	{
		pRefCountBuffer->Release();
		pRefCountBuffer = NULL;
	}
	if (pLocalPlayer)
	{
		pLocalPlayer->Release();
		pLocalPlayer = NULL;
	}
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	if (pConnection)
	{
		pConnection->Release();
		pConnection = NULL;
	}
	goto Exit;
}


//	DN_Host

#undef DPF_MODNAME
#define DPF_MODNAME "DN_Host"

HRESULT DIRECTNETOBJECT::DN_Host( PVOID pInterface,
					  const DPN_APPLICATION_DESC *const pdnAppDesc,
					  IDirectPlay8Address **const prgpDeviceInfo,
					  const DWORD cDeviceInfo,
					  const DPN_SECURITY_DESC *const pdnSecurity,
					  const DPN_SECURITY_CREDENTIALS *const pdnCredentials,
					  void *const pvPlayerContext,
					  const DWORD dwFlags)
{
	CNameTableEntry		*pHostPlayer;
	CNameTableEntry		*pAllPlayersGroup;
	DWORD				dwCurrentAddress;
	DWORD				dwCurrentDevice;
	HRESULT				hResultCode;
	DIRECTNETOBJECT		*pdnObject = this;
	LPWSTR				lpwszPassword = NULL;
	PVOID				pvApplicationReservedData = NULL;
	IDirectPlay8Address	**prgIDevice;
	DWORD				dwListensRunning;
	CConnection			*pConnection;
	DWORD				dwVersion;
	GUID				guidSP;
	GUID				guidAdapter;
	CAsyncOp			*pListenParent;
	void				*pvAdapterBuffer;
	DWORD				dwEnumFrameSize;

	DPF(2,"Parameters: pInterface [0x%p], pdnAppDesc [0x%p], prgpDeviceInfo [0x%p], cDeviceInfo [%ld], pdnSecurity [0x%p], pdnCredentials [0x%p], dwFlags [0x%lx]",
		pInterface,pdnAppDesc,prgpDeviceInfo,cDeviceInfo,pdnSecurity,pdnCredentials,dwFlags);

	RIP_ON_NOT_TRUE( "Host() Error validating host params", SUCCEEDED( DN_ValidateHost( pInterface, pdnAppDesc, prgpDeviceInfo, cDeviceInfo,
                                                          pdnSecurity, pdnCredentials, pvPlayerContext,
                                                          dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "Host() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	// Check to ensure not already connected
    	if (pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED)
    	{
    	    if( DN_CHECK_LOCALHOST( pdnObject ) )
    	    {
    	        DPFERR("Object is already hosting" );
    	    	DPF_RETURN(DPNERR_HOSTING);	
    	    }
    	    else
    	    {
        	    DPFERR("Object is already connected" );
                DPF_RETURN(DPNERR_ALREADYCONNECTED);
    	    }
    	}

    	RIP_ON_NOT_TRUE( "Host() You cannot specify the clientserver flag in peer mode", !((pdnObject->dwFlags & DN_OBJECT_FLAG_PEER) &&
										     	   (pdnAppDesc->dwFlags & DPNSESSION_CLIENT_SERVER) ) );
    	RIP_ON_NOT_TRUE( "Host() You MUST specify the client/server flag for client/server mode", !((pdnObject->dwFlags & DN_OBJECT_FLAG_SERVER) &&
    	   											!(pdnAppDesc->dwFlags & DPNSESSION_CLIENT_SERVER) ) );

	prgIDevice = NULL;
	pListenParent = NULL;
	pConnection = NULL;
	pHostPlayer = NULL;
	pAllPlayersGroup = NULL;

	//
	//	Flag as CONNECTING to prevent other operations here
	//
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	if (pdnObject->dwFlags & (DN_OBJECT_FLAG_CONNECTING | DN_OBJECT_FLAG_CONNECTED))
	{
		DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
		hResultCode = DPNERR_ALREADYCONNECTED;
		goto Failure;
	}
	pdnObject->dwFlags |= DN_OBJECT_FLAG_CONNECTING;

	// Adding local host flag
	pdnObject->dwFlags |= DN_OBJECT_FLAG_LOCALHOST;
	
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
	
	//
	//	Copy application description to DirectNet object
	//
	pdnObject->ApplicationDesc.Lock();
	hResultCode = pdnObject->ApplicationDesc.Update(pdnAppDesc,DN_APPDESCINFO_FLAG_SESSIONNAME|DN_APPDESCINFO_FLAG_PASSWORD|
			DN_APPDESCINFO_FLAG_RESERVEDDATA|DN_APPDESCINFO_FLAG_APPRESERVEDDATA);
	pdnObject->ApplicationDesc.Unlock();
	if (hResultCode != DPN_OK)
	{
		DPFERR("Could not update application description");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	//
	// Create Instance GUID
	//
	if ((hResultCode = pdnObject->ApplicationDesc.CreateNewInstanceGuid()) != DPN_OK)
	{
		DPFERR("Could not create instance GUID - ignore and continue");
		DisplayDNError(0,hResultCode);
	}

	//
	// Start listens
	//

	// Duplicate address interfaces
	if ((prgIDevice = static_cast<IDirectPlay8Address**>(DNMalloc(cDeviceInfo*sizeof(IDirectPlay8Address*)))) == NULL)
	{
		DPFERR("Could not create duplicate address interface array");
		DNASSERT(FALSE);
		goto Failure;
	}
	for (dwCurrentDevice = 0 ; dwCurrentDevice < cDeviceInfo ; dwCurrentDevice++)
	{
		if ((hResultCode = (prgpDeviceInfo[dwCurrentDevice])->Duplicate(&prgIDevice[dwCurrentDevice])) != DPN_OK)
		{
			DPFERR("Could not duplicate Host address info - skipping it");
			prgIDevice[dwCurrentDevice] = NULL;
			continue;
		}
	}

	// Parent Async Op
	if ((hResultCode = AsyncOpNew(pdnObject,&pListenParent)) != DPN_OK)
	{
		DPFERR("Could not create AsyncOp");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
		goto Failure;
	}
	pListenParent->SetOpType( ASYNC_OP_LISTEN );
	pListenParent->MakeParent();
	pListenParent->SetCompletion( DNCompleteListen );

	// Children op's
	dwListensRunning = 0;
	for (dwCurrentDevice = 0 ; dwCurrentDevice < cDeviceInfo ; dwCurrentDevice++)
	{
		hResultCode = DNPerformSPListen(pdnObject,prgIDevice[dwCurrentDevice],pListenParent,NULL);
		if (hResultCode == DPN_OK)
		{
			dwListensRunning++;
		}
	}

	// Make sure at least 1 listen started
	if (dwListensRunning == 0)
	{
		DPFERR("Could not start any LISTENs");
		hResultCode = DPNERR_INVALIDDEVICEADDRESS;
		goto Failure;
	}

	// Store parent LISTEN on DirectNet object
	pListenParent->AddRef();
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	pdnObject->pListenParent = pListenParent;
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	pListenParent->Release();
	pListenParent = NULL;

	//
	//	Determine max allowable enum frame size by examining the SPCaps for each SP
	//
	if ((hResultCode = DNGetMaxEnumFrameSize(pdnObject,&dwEnumFrameSize)) != DPN_OK)
	{
		DPFERR("Could not determine enum frame size");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	DNASSERT( dwEnumFrameSize >= (sizeof(DN_ENUM_RESPONSE_PAYLOAD) + sizeof(DPN_APPLICATION_DESC_INFO)) );
	if (dwEnumFrameSize < (sizeof(DN_ENUM_RESPONSE_PAYLOAD) + sizeof(DPN_APPLICATION_DESC_INFO) + pdnAppDesc->dwApplicationReservedDataSize))
	{
		DPFERR("Not enough room for the application reserved data");
		hResultCode = DPNERR_DATATOOLARGE;
		goto Failure;
	}

	//
	//	Set NameTable DPNID mask
	//
	DPF(5,"DPNID Mask [0x%lx]",pdnObject->ApplicationDesc.GetDPNIDMask());
	pdnObject->NameTable.SetDPNIDMask( pdnObject->ApplicationDesc.GetDPNIDMask() );

	//
	// Create group "ALL PLAYERS"
	//
	if ((hResultCode = NameTableEntryNew(pdnObject,&pAllPlayersGroup)) != DPN_OK)
	{
		DPFERR("Could not create NameTableEntry");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
		goto Failure;
	}
	pAllPlayersGroup->MakeGroup();

	// This function takes the lock internally
	pAllPlayersGroup->UpdateEntryInfo(	DN_ALL_PLAYERS_GROUP_NAME,
										DN_ALL_PLAYERS_GROUP_NAME_SIZE,
										NULL,
										0,
										DPNINFO_NAME|DPNINFO_DATA,
										FALSE);

	pdnObject->NameTable.MakeAllPlayersGroup(pAllPlayersGroup);

	//
	// Create local player
	//
	if ((hResultCode = NameTableEntryNew(pdnObject,&pHostPlayer)) != DPN_OK)
	{
		DPFERR("Could not create NameTableEntry");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
		goto Failure;
	}

	// This function takes the lock internally
	pHostPlayer->UpdateEntryInfo(	pdnObject->NameTable.GetDefaultPlayer()->GetName(),
								pdnObject->NameTable.GetDefaultPlayer()->GetNameSize(),
								pdnObject->NameTable.GetDefaultPlayer()->GetData(),
								pdnObject->NameTable.GetDefaultPlayer()->GetDataSize(),
								DPNINFO_NAME|DPNINFO_DATA,
								FALSE);

	pHostPlayer->SetDNETVersion( DN_VERSION_CURRENT );

	if (pdnObject->dwFlags & DN_OBJECT_FLAG_PEER)
	{
		pHostPlayer->MakePeer();
	}
	else if (pdnObject->dwFlags & DN_OBJECT_FLAG_SERVER)
	{
		pHostPlayer->MakeServer();
	}
	else
	{
		DNASSERT(FALSE);
	}

	pHostPlayer->SetContext(pvPlayerContext);
	pHostPlayer->StartConnecting();

	if ((hResultCode = pdnObject->NameTable.AddEntry(pHostPlayer)) != DPN_OK)
	{
		DPFERR("Could not add NameTableEntry to NameTable");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
		goto Failure;
	}

	// Create Host's connection (NULL end point)
	if ((hResultCode = ConnectionNew(pdnObject,&pConnection)) != DPN_OK)
	{
		DPFERR("Could not create new connection");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
		goto Failure;
	}
	pConnection->SetStatus( CONNECTED );
	pConnection->MakeLocal();
	pConnection->SetEndPt(NULL);
	pConnection->SetDPNID(pHostPlayer->GetDPNID());

	pdnObject->NameTable.MakeLocalPlayer(pHostPlayer);
	pdnObject->NameTable.MakeHostPlayer(pHostPlayer);

	//
	//	Make ALL_PLAYERS group available
	//
	pAllPlayersGroup->Lock();
	pAllPlayersGroup->MakeAvailable();
	pAllPlayersGroup->Unlock();
	pAllPlayersGroup->Release();
	pAllPlayersGroup = NULL;

	//
	//	Update DirectNet object to be connected
	//
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	DNASSERT(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING);
	pdnObject->dwFlags &= ~DN_OBJECT_FLAG_CONNECTING;
	pdnObject->dwFlags |= DN_OBJECT_FLAG_CONNECTED;
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	//
	//	One player in game (Local/Host player)
	//
	pdnObject->ApplicationDesc.IncPlayerCount(TRUE);

	//
	//	Populate local player's connection
	//
	pConnection->SetDPNID(pHostPlayer->GetDPNID());
	pdnObject->NameTable.PopulateConnection(pConnection);
	pConnection->Release();
	pConnection = NULL;

	pHostPlayer->Release();
	pHostPlayer = NULL;

	hResultCode = DPN_OK;

Exit:
	//
	//	Clean up copies of device address
	//
	if (prgIDevice)
	{
		for (dwCurrentDevice = 0 ; dwCurrentDevice < cDeviceInfo ; dwCurrentDevice++)
		{
			if (prgIDevice[dwCurrentDevice])
			{
				(prgIDevice[dwCurrentDevice])->Release();
				prgIDevice[dwCurrentDevice] = NULL;
			}
		}
		DNFree(prgIDevice);
		prgIDevice = NULL;
	}

	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pListenParent)
	{
		pListenParent->Release();
		pListenParent = NULL;
	}
	if (pHostPlayer)
	{
		pHostPlayer->Release();
		pHostPlayer = NULL;
	}
	if (pAllPlayersGroup)
	{
		pAllPlayersGroup->Release();
		pAllPlayersGroup = NULL;
	}
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	pListenParent = pdnObject->pListenParent;
	pdnObject->pListenParent = NULL;
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
	if (pListenParent)
	{
		DNCancelChildren(pdnObject,pListenParent);
		pListenParent->Release();
		pListenParent = NULL;
	}

	pdnObject->NameTable.EmptyTable(DPNERR_HOSTTERMINATEDSESSION);

	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	pdnObject->dwFlags &= ~(DN_OBJECT_FLAG_CONNECTING|DN_OBJECT_FLAG_CONNECTED|DN_OBJECT_FLAG_LOCALHOST);
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_CreateGroup"

HRESULT DIRECTNETOBJECT::DN_CreateGroup(PVOID pInterface,
							const DPN_GROUP_INFO *const pdpnGroupInfo,
							void *const pvGroupContext,
							void *const pvAsyncContext,
							DPNHANDLE *const phAsyncHandle,
							const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	HRESULT				hResultCode;
	DPNHANDLE			hAsyncOp;
	PWSTR				pwszName;
	DWORD				dwNameSize;
	PVOID				pvData;
	DWORD				dwDataSize;
	CNameTableEntry		*pLocalPlayer;

	DPF(2,"Parameters: pInterface [0x%p], pdpnGroupInfo [0x%p], pvAsyncContext [0x%p], phAsyncHandle [0x%p], dwFlags [0x%lx]",
			pInterface,pdpnGroupInfo,pvAsyncContext,phAsyncHandle,dwFlags);

	RIP_ON_NOT_TRUE( "CreateGroup() Error validating create group params", SUCCEEDED( DN_ValidateCreateGroup( pInterface, pdpnGroupInfo, pvGroupContext,
    	                                                      pvAsyncContext,phAsyncHandle, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "CreateGroup() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to create a group" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}    	

	pLocalPlayer = NULL;

	if ((pdpnGroupInfo->dwInfoFlags & DPNINFO_NAME) && (pdpnGroupInfo->pwszName))
	{
		pwszName = pdpnGroupInfo->pwszName;
		dwNameSize = (wcslen(pwszName) + 1) * sizeof(WCHAR);
	}
	else
	{
		pwszName = NULL;
		dwNameSize = 0;
	}
	if ((pdpnGroupInfo->dwInfoFlags & DPNINFO_DATA) && (pdpnGroupInfo->pvData) && (pdpnGroupInfo->dwDataSize))
	{
		pvData = pdpnGroupInfo->pvData;
		dwDataSize = pdpnGroupInfo->dwDataSize;
	}
	else
	{
		pvData = NULL;
		dwDataSize = 0;
	}

	if ((hResultCode = pdnObject->NameTable.GetLocalPlayerRef( &pLocalPlayer )) != DPN_OK)
	{
		DPFERR("Could not get local player reference");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	if (pLocalPlayer->IsHost())
	{
		DPF(3,"Host is creating group");
		hResultCode = DNHostCreateGroup(pdnObject,
										pwszName,
										dwNameSize,
										pvData,
										dwDataSize,
										pdpnGroupInfo->dwInfoFlags,
										pdpnGroupInfo->dwGroupFlags,
										pvGroupContext,
										pvAsyncContext,
										pLocalPlayer->GetDPNID(),
										0,
										&hAsyncOp,
										dwFlags);
		if ((hResultCode != DPN_OK) && (hResultCode != DPNERR_PENDING))
		{
			DPFERR("Could not request host to create group");
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;

				//
				//	Release Async HANDLE since this operation has already completed (!)
				//
				pdnObject->HandleTable.Destroy( hAsyncOp );
				hAsyncOp = 0;
		}
	}
	else
	{
		DPF(3,"Request host to create group");

		hResultCode = DNRequestCreateGroup(	pdnObject,
											pwszName,
											dwNameSize,
											pvData,
											dwDataSize,
											pdpnGroupInfo->dwGroupFlags,
											pvGroupContext,
											pvAsyncContext,
											&hAsyncOp,
											dwFlags);
		if ((hResultCode != DPN_OK) && (hResultCode != DPNERR_PENDING))
		{
			DPFERR("Could not request host to create group");
			hResultCode = DPNERR_GENERIC;
#pragma BUGBUG( minara, "This operation should be queued to re-try after host migration" )
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;
		}
	}

	pLocalPlayer->Release();
	pLocalPlayer = NULL;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	DNASSERT(hResultCode != DPNERR_INVALIDENDPOINT);
	return(hResultCode);

Failure:
	if (pLocalPlayer)
	{
		pLocalPlayer->Release();
		pLocalPlayer = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_DestroyGroup"

HRESULT DIRECTNETOBJECT::DN_DestroyGroup(PVOID pInterface,
							 const DPNID dpnidGroup,
							 PVOID const pvAsyncContext,
							 DPNHANDLE *const phAsyncHandle,
							 const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	HRESULT				hResultCode;
	DPNHANDLE			hAsyncOp;
	CNameTableEntry		*pLocalPlayer;
	CNameTableEntry		*pNTEntry;

	DPF(2,"Parameters: pInterface [0x%p], dpnidGroup [0x%lx], pvAsyncContext [0x%p], phAsyncHandle [0x%p], dwFlags [0x%lx]",
			pInterface,dpnidGroup,pvAsyncContext,phAsyncHandle,dwFlags);

	RIP_ON_NOT_TRUE( "DestroyGroup() Error validating destroy group params", SUCCEEDED( DN_ValidateDestroyGroup( pInterface, dpnidGroup, pvAsyncContext,
    	                                                      phAsyncHandle, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "DestroyGroup() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to destroy a group" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	
    	
	pLocalPlayer = NULL;
	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnidGroup,&pNTEntry)) != DPN_OK)
	{
	    DPFERR( "Could not find specified group" );
		DisplayDNError(0,hResultCode);
		//
		//	Try deleted list
		//
		if ((hResultCode = pdnObject->NameTable.FindDeletedEntry(dpnidGroup,&pNTEntry)) != DPN_OK)
		{
			hResultCode = DPNERR_INVALIDGROUP;
			goto Failure;
		}
		pNTEntry->Release();
		pNTEntry = NULL;
		hResultCode = DPNERR_CONNECTIONLOST;
		goto Failure;

	}
	if (!pNTEntry->IsGroup() || pNTEntry->IsAllPlayersGroup())
	{
		hResultCode = DPNERR_INVALIDGROUP;
		goto Failure;
	}
	pNTEntry->Release();
	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.GetLocalPlayerRef( &pLocalPlayer )) != DPN_OK)
	{
		DPFERR("Could not get local player reference");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	if (pLocalPlayer->IsHost())
	{
		DPF(3,"Host is destroying group");
		hResultCode = DNHostDestroyGroup(	pdnObject,
											dpnidGroup,
											pvAsyncContext,
											pLocalPlayer->GetDPNID(),
											0,
											&hAsyncOp,
											dwFlags);
		if ((hResultCode != DPN_OK) && (hResultCode != DPNERR_PENDING))
		{
			DPFERR("Could not request host to destroy group");
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;

				//
				//	Release Async HANDLE since this operation has already completed (!)
				//
				pdnObject->HandleTable.Destroy( hAsyncOp );
				hAsyncOp = 0;
		}
	}
	else
	{
		DPF(3,"Request host to destroy group");

		hResultCode = DNRequestDestroyGroup(pdnObject,
											dpnidGroup,
											pvAsyncContext,
											&hAsyncOp,
											dwFlags);
		if (hResultCode != DPN_OK && hResultCode != DPNERR_PENDING)
		{
			DPFERR("Could not request host to destroy group");
			hResultCode = DPNERR_GENERIC;
#pragma BUGBUG( minara, "This operation should be queued to re-try after host migration" )
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;
		}
	}
	pLocalPlayer->Release();
	pLocalPlayer = NULL;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	DNASSERT(hResultCode != DPNERR_INVALIDENDPOINT);
	return(hResultCode);

Failure:
	if (pLocalPlayer)
	{
		pLocalPlayer->Release();
		pLocalPlayer = NULL;
	}
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_AddClientToGroup"

HRESULT DIRECTNETOBJECT::DN_AddClientToGroup(PVOID pInterface,
								 const DPNID dpnidGroup,
								 const DPNID dpnidClient,
								 PVOID const pvAsyncContext,
								 DPNHANDLE *const phAsyncHandle,
								 const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	HRESULT				hResultCode;
	DPNHANDLE			hAsyncOp;
	CNameTableEntry		*pLocalPlayer;
	CNameTableEntry		*pNTEntry;

	DPF(2,"Parameters: pInterface [0x%p], dpnidGroup [0x%lx], dpnidClient [0x%lx], pvAsyncContext [0x%p], phAsyncHandle [0x%p], dwFlags [0x%lx]",
			pInterface,dpnidGroup,dpnidClient,pvAsyncContext,phAsyncHandle,dwFlags);

	RIP_ON_NOT_TRUE( "AddClientToGroup() Error validating add client to group params", SUCCEEDED( DN_ValidateAddClientToGroup( pInterface, dpnidGroup, dpnidClient, pvAsyncContext,
    	                                                      phAsyncHandle, dwFlags ) ));

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "AddClientToGroup() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to add a player to a group" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}    	

	pLocalPlayer = NULL;
	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnidGroup,&pNTEntry)) != DPN_OK)
	{
        DPFERR( "Unable to find specified group" );
		hResultCode = DPNERR_INVALIDGROUP;
		goto Failure;
	}
	if (!pNTEntry->IsGroup() || pNTEntry->IsAllPlayersGroup())
	{
	    DPFERR( "Unable to specify client or all players group for group ID" );
		hResultCode = DPNERR_INVALIDGROUP;
		goto Failure;
	}
	pNTEntry->Release();
	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnidClient,&pNTEntry)) != DPN_OK)
	{
	    DPFERR( "Unable to find specified player" );
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
	}
	if (pNTEntry->IsGroup())
	{
	    DPFERR( "Specified client is a group ID" );
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
	}
	pNTEntry->Release();
	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.GetLocalPlayerRef( &pLocalPlayer )) != DPN_OK)
	{
		DPFERR("Could not get local player reference");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	if (pLocalPlayer->IsHost())
	{
		DPF(3,"Host is adding player to group");
		hResultCode = DNHostAddPlayerToGroup(	pdnObject,
												dpnidGroup,
												dpnidClient,
												pvAsyncContext,
												pLocalPlayer->GetDPNID(),
												0,
												&hAsyncOp,
												dwFlags);
		if ((hResultCode != DPN_OK) && (hResultCode != DPNERR_PENDING))
		{
			DPFERR("Could not request host to add player to group");
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;

				//
				//	Release Async HANDLE since this operation has already completed (!)
				//
				pdnObject->HandleTable.Destroy( hAsyncOp );
				hAsyncOp = 0;
		}
	}
	else
	{
		DPF(3,"Request host to add player to group");

		hResultCode = DNRequestAddPlayerToGroup(pdnObject,
												dpnidGroup,
												dpnidClient,
												pvAsyncContext,
												&hAsyncOp,
												dwFlags);
		if (hResultCode != DPN_OK && hResultCode != DPNERR_PENDING)
		{
			DPFERR("Could not request host to add player to group");
			hResultCode = DPNERR_GENERIC;
#pragma BUGBUG( minara, "This operation should be queued to re-try after host migration" )
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;
		}
	}
	pLocalPlayer->Release();
	pLocalPlayer = NULL;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	DNASSERT(hResultCode != DPNERR_INVALIDENDPOINT);
	return(hResultCode);

Failure:
	if (pLocalPlayer)
	{
		pLocalPlayer->Release();
		pLocalPlayer = NULL;
	}
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_RemoveClientFromGroup"

HRESULT DIRECTNETOBJECT::DN_RemoveClientFromGroup(PVOID pInterface,
									  const DPNID dpnidGroup,
									  const DPNID dpnidClient,
									  PVOID const pvAsyncContext,
									  DPNHANDLE *const phAsyncHandle,
									  const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	HRESULT				hResultCode;
	DPNHANDLE			hAsyncOp;
	CNameTableEntry		*pLocalPlayer;
	CNameTableEntry		*pNTEntry;

	DPF(2,"Parameters: pInterface [0x%p], dpnidGroup [0x%lx], dpnidClient [0x%lx], pvAsyncContext [0x%p], phAsyncHandle [0x%p], dwFlags [0x%lx]",
			pInterface,dpnidGroup,dpnidClient,pvAsyncContext,phAsyncHandle,dwFlags);

	RIP_ON_NOT_TRUE( "RemoveClientFromGroup() Error validating remove client from group params", SUCCEEDED( DN_ValidateRemoveClientFromGroup( pInterface, dpnidGroup, dpnidClient, pvAsyncContext, phAsyncHandle, dwFlags ) ));

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "RemoveClientFromGroup() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to remove a player from a group" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	

	pLocalPlayer = NULL;
	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnidGroup,&pNTEntry)) != DPN_OK)
	{
	    DPFERR( "Could not find specified group in nametable" );
		hResultCode = DPNERR_INVALIDGROUP;
		goto Failure;
	}
	if (!pNTEntry->IsGroup() || pNTEntry->IsAllPlayersGroup())
	{
	    DPFERR( "Specified ID is not a valid group!" );
		hResultCode = DPNERR_INVALIDGROUP;
		goto Failure;
	}
	pNTEntry->Release();
	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnidClient,&pNTEntry)) != DPN_OK)
	{
	    DPFERR( "Specified client ID is not a valid client!" );
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
	}
	if (pNTEntry->IsGroup())
	{
	    DPFERR( "Specified client ID is a group!" );	
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
	}
	pNTEntry->Release();
	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.GetLocalPlayerRef( &pLocalPlayer )) != DPN_OK)
	{
		DPFERR("Could not get local player reference");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	if (pLocalPlayer->IsHost())
	{
		DPF(3,"Host is deleting player from group");
		hResultCode = DNHostDeletePlayerFromGroup(	pdnObject,
													dpnidGroup,
													dpnidClient,
													pvAsyncContext,
													pLocalPlayer->GetDPNID(),
													0,
													&hAsyncOp,
													dwFlags);
		if ((hResultCode != DPN_OK) && (hResultCode != DPNERR_PENDING))
		{
			DPFERR("Could not request host to delete player from group");
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;

				//
				//	Release Async HANDLE since this operation has already completed (!)
				//
				pdnObject->HandleTable.Destroy( hAsyncOp );
				hAsyncOp = 0;
		}
	}
	else
	{
		DPF(3,"Request host to delete player from group");

		hResultCode = DNRequestDeletePlayerFromGroup(pdnObject,
													dpnidGroup,
													dpnidClient,
													pvAsyncContext,
													&hAsyncOp,
													dwFlags);
		if (hResultCode != DPN_OK && hResultCode != DPNERR_PENDING)
		{
			DPFERR("Could not request host to delete player from group");
			hResultCode = DPNERR_GENERIC;
#pragma BUGBUG( minara, "This operation should be queued to re-try after host migration" )
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;
		}
	}
	pLocalPlayer->Release();
	pLocalPlayer = NULL;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	DNASSERT(hResultCode != DPNERR_INVALIDENDPOINT);
	return(hResultCode);

Failure:
	if (pLocalPlayer)
	{
		pLocalPlayer->Release();
		pLocalPlayer = NULL;
	}
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	goto Exit;
}


//	DN_SetGroupInfo

#undef DPF_MODNAME
#define DPF_MODNAME "DN_SetGroupInfo"

HRESULT DIRECTNETOBJECT::DN_SetGroupInfo( PVOID pv,
							  const DPNID dpnid,
							  DPN_GROUP_INFO *const pdpnGroupInfo,
							  PVOID const pvAsyncContext,
							  DPNHANDLE *const phAsyncHandle,
							  const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	HRESULT				hResultCode;
	DPNHANDLE			hAsyncOp;
	PWSTR				pwszName;
	DWORD				dwNameSize;
	PVOID				pvData;
	DWORD				dwDataSize;
	CNameTableEntry		*pLocalPlayer;
	CNameTableEntry		*pNTEntry;

	DPF(2,"Parameters: pv [0x%p], dpnid [0x%lx], pvAsyncContext [0x%p], phAsyncHandle [0x%p], dwFlags [0x%lx]",
			pv,dpnid,pdpnGroupInfo,pvAsyncContext,phAsyncHandle,dwFlags);

	RIP_ON_NOT_TRUE( "SetGroupInfo() Error validating set group info params", SUCCEEDED( DN_ValidateSetGroupInfo( pv, dpnid, pdpnGroupInfo, pvAsyncContext, phAsyncHandle, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "SetGroupInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to set group info" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	    	
	
	pLocalPlayer = NULL;
	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnid,&pNTEntry)) != DPN_OK)
	{
	    DPFERR( "Specified ID is not a group" );
		if ((hResultCode = pdnObject->NameTable.FindDeletedEntry(dpnid,&pNTEntry)) != DPN_OK)
		{
			hResultCode = DPNERR_INVALIDGROUP;
			goto Failure;
		}
		pNTEntry->Release();
		pNTEntry = NULL;
		hResultCode = DPNERR_CONNECTIONLOST;
		goto Failure;
	}
	if (!pNTEntry->IsGroup() || pNTEntry->IsAllPlayersGroup())
	{
	    DPFERR( "Specified ID is not a valid group" );
		hResultCode = DPNERR_INVALIDGROUP;
		goto Failure;
	}
	pNTEntry->Release();
	pNTEntry = NULL;

	if ((pdpnGroupInfo->dwInfoFlags & DPNINFO_NAME) && (pdpnGroupInfo->pwszName))
	{
		pwszName = pdpnGroupInfo->pwszName;
		dwNameSize = (wcslen(pwszName) + 1) * sizeof(WCHAR);
	}
	else
	{
		pwszName = NULL;
		dwNameSize = 0;
	}
	if ((pdpnGroupInfo->dwInfoFlags & DPNINFO_DATA) && (pdpnGroupInfo->pvData) && (pdpnGroupInfo->dwDataSize))
	{
		pvData = pdpnGroupInfo->pvData;
		dwDataSize = pdpnGroupInfo->dwDataSize;
	}
	else
	{
		pvData = NULL;
		dwDataSize = 0;
	}

	if ((hResultCode = pdnObject->NameTable.GetLocalPlayerRef( &pLocalPlayer )) != DPN_OK)
	{
		DPFERR("Could not get local player reference");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	if (pLocalPlayer->IsHost())
	{
		DPF(3,"Host is updating group info");
		hResultCode = DNHostUpdateInfo(	pdnObject,
										dpnid,
										pwszName,
										dwNameSize,
										pvData,
										dwDataSize,
										pdpnGroupInfo->dwInfoFlags,
										pvAsyncContext,
										pLocalPlayer->GetDPNID(),
										0,
										&hAsyncOp,
										dwFlags );
		if ((hResultCode != DPN_OK) && (hResultCode != DPNERR_PENDING))
		{
			DPFERR("Could not request host to update info");
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;

				//
				//	Release Async HANDLE since this operation has already completed (!)
				//
				pdnObject->HandleTable.Destroy( hAsyncOp );
				hAsyncOp = 0;
		}
	}
	else
	{
		DPF(3,"Request host to update group info");

		hResultCode = DNRequestUpdateInfo(	pdnObject,
											dpnid,
											pwszName,
											dwNameSize,
											pvData,
											dwDataSize,
											pdpnGroupInfo->dwInfoFlags,
											pvAsyncContext,
											&hAsyncOp,
											dwFlags);
		if (hResultCode != DPN_OK && hResultCode != DPNERR_PENDING)
		{
			DPFERR("Could not request host to update info");
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;
		}
	}
	pLocalPlayer->Release();
	pLocalPlayer = NULL;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	DNASSERT(hResultCode != DPNERR_INVALIDENDPOINT);
	return(hResultCode);

Failure:
	if (pLocalPlayer)
	{
		pLocalPlayer->Release();
		pLocalPlayer = NULL;
	}
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	goto Exit;
}


//	DN_GetGroupInfo
//
//	Retrieve group name and/or data from the local nametable.
//
//	lpwszGroupName may be NULL to avoid retrieving group name
//	pdwGroupFlags may be NULL to avoid retrieving group flags
//	pvGroupData may not by NULL if *pdwDataSize is non zero

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetGroupInfo"

HRESULT DIRECTNETOBJECT::DN_GetGroupInfo(PVOID pv,
							 const DPNID dpnid,
							 DPN_GROUP_INFO *const pdpnGroupInfo,
							 DWORD *const pdwSize,
							 const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	CNameTableEntry		*pNTEntry;
	CPackedBuffer		packedBuffer;
	HRESULT				hResultCode;

	DPF(2,"Parameters: dpnid [0x%lx], pdpnGroupInfo [0x%p], dwFlags [0x%lx]",
			dpnid,pdpnGroupInfo,dwFlags);

	RIP_ON_NOT_TRUE( "GetGroupInfo() Error validating get group info params", SUCCEEDED( DN_ValidateGetGroupInfo( pv, dpnid, pdpnGroupInfo, pdwSize, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "GetGroupInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & (DN_OBJECT_FLAG_CONNECTED | DN_OBJECT_FLAG_DISCONNECTING)) )
    	{
    	    DPFERR("You must be connected / hosting to get group info" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	    	

	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnid,&pNTEntry)) != DPN_OK)
	{
	    DPFERR( "Specified group is not valid" );
		if ((hResultCode = pdnObject->NameTable.FindDeletedEntry(dpnid,&pNTEntry)) != DPN_OK)
		{
			hResultCode = DPNERR_INVALIDGROUP;
			goto Failure;
		}
	}
	packedBuffer.Initialize(pdpnGroupInfo,*pdwSize);

	pNTEntry->Lock();
	if (!pNTEntry->IsGroup() || pNTEntry->IsAllPlayersGroup())
	{
	    DPFERR( "Specified ID is not a group" );
		pNTEntry->Unlock();
		hResultCode = DPNERR_INVALIDGROUP;
		goto Failure;
	}

	hResultCode = pNTEntry->PackInfo(&packedBuffer);

	pNTEntry->Unlock();
	pNTEntry->Release();
	pNTEntry = NULL;

	if ((hResultCode == DPN_OK) || (hResultCode == DPNERR_BUFFERTOOSMALL))
	{
		*pdwSize = packedBuffer.GetSizeRequired();
	}

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_EnumClientsAndGroups"

HRESULT DIRECTNETOBJECT::DN_EnumClientsAndGroups(PVOID pInterface,
									 DPNID *const prgdpnid,
									 DWORD *const pcdpnid,
									 const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	CBilink				*pBilink;
	CNameTableEntry		*pNTEntry;
	DWORD				dwCount;
	DPNID				*pDPNID;
	BOOL				bEnum = TRUE;
	HRESULT             hResultCode;

	DPF(2,"Parameters: pInterface [0x%p], prgdpnid [0x%p], pcdpnid [0x%p], dwFlags [0x%lx]",
			pInterface,prgdpnid,pcdpnid,dwFlags);

	RIP_ON_NOT_TRUE( "EnumClientsAndGroups() Error validating enum clients and groups params", SUCCEEDED( DN_ValidateEnumClientsAndGroups( pInterface, prgdpnid, pcdpnid, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "EnumClientsAndGroups() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to create a group" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	
    	
	if (prgdpnid == NULL || *pcdpnid == 0)				// Don't enum if not asked to
	{
		bEnum = FALSE;
	}

	pdnObject->NameTable.Lock();

	dwCount = 0;
	pDPNID = prgdpnid;

	//
	//	Enum players
	//
	if (dwFlags & DPNENUM_PLAYERS)
	{
		pBilink = pdnObject->NameTable.m_bilinkPlayers.GetNext();
		while (pBilink != &pdnObject->NameTable.m_bilinkPlayers)
		{
			pNTEntry = CONTAINING_OBJECT(pBilink,CNameTableEntry,m_bilinkEntries);

			pNTEntry->Lock();
			if (pNTEntry->IsAvailable())
			{
				dwCount++;
				if (bEnum && (dwCount <= *pcdpnid))
				{
					*pDPNID++ = pNTEntry->GetDPNID();
				}
				else
				{
					bEnum = FALSE;
				}
			}
			pNTEntry->Unlock();
			pBilink = pBilink->GetNext();
		}
	}

	//
	//	Enum groups
	//
	if (dwFlags & DPNENUM_GROUPS)
	{
		pBilink = pdnObject->NameTable.m_bilinkGroups.GetNext();
		while (pBilink != &pdnObject->NameTable.m_bilinkGroups)
		{
			pNTEntry = CONTAINING_OBJECT(pBilink,CNameTableEntry,m_bilinkEntries);

			pNTEntry->Lock();
			if (pNTEntry->IsAvailable() && !pNTEntry->IsAllPlayersGroup())
			{
				dwCount++;
				if (bEnum && (dwCount <= *pcdpnid))
				{
					*pDPNID++ = pNTEntry->GetDPNID();
				}
				else
				{
					bEnum = FALSE;
				}
			}
			pNTEntry->Unlock();
			pBilink = pBilink->GetNext();
		}
	}

	pdnObject->NameTable.Unlock();

	//
	//	This will NOT include players/groups in the deleted list.
	//	i.e. removed from the NameTable but for whom DESTROY_PLAYER/GROUP notifications have yet to be posted
	//
	*pcdpnid = dwCount;
	if (!bEnum && dwCount)
	{
		hResultCode = DPNERR_BUFFERTOOSMALL;
	}
	else
	{
		hResultCode = DPN_OK;
	}

	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_EnumGroupMembers"

HRESULT DIRECTNETOBJECT::DN_EnumGroupMembers(PVOID pInterface,
								 const DPNID dpnid,
								 DPNID *const prgdpnid,
								 DWORD *const pcdpnid,
								 const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	CNameTableEntry		*pNTEntry;
	CGroupMember		*pGroupMember;
	CBilink				*pBilink;
	DWORD				dwCount;
	DPNID				*pDPNID;
	BOOL				bOutputBufferTooSmall = FALSE;
	HRESULT				hResultCode;

	DPF(2,"Parameters: pInterface [0x%p], dpnid [0x%lx], prgdpnid [0x%p], pcdpnid [0x%p], dwFlags [0x%lx]",
			pInterface,dpnid,prgdpnid,pcdpnid,dwFlags);

	RIP_ON_NOT_TRUE( "EnumGroupMembers() Error validating enum group params", SUCCEEDED( DN_ValidateEnumGroupMembers( pInterface, dpnid, prgdpnid, pcdpnid, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "EnumGroupMembers() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to create a group" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	

	pNTEntry = NULL;

	//
	// if the user didn't supply a buffer, assume that the
	// output buffer is too small
	//
	if ( ( prgdpnid == NULL ) || ( ( *pcdpnid ) == 0 ) )	
	{
		bOutputBufferTooSmall = TRUE;
	}

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnid,&pNTEntry)) != DPN_OK)
	{
		DPFERR("Could not find NameTableEntry");
		if ((hResultCode = pdnObject->NameTable.FindDeletedEntry(dpnid,&pNTEntry)) != DPN_OK)
		{
			hResultCode = DPNERR_INVALIDGROUP;
			goto Failure;
		}
		pNTEntry->Release();
		pNTEntry = NULL;
		hResultCode = DPNERR_CONNECTIONLOST;
		goto Failure;
	}

	if (!pNTEntry->IsGroup() || pNTEntry->IsAllPlayersGroup())
	{
		DPFERR("Not a group dpnid!");
		hResultCode = DPNERR_INVALIDGROUP;
		goto Failure;
	}

	pNTEntry->Lock();

	dwCount = 0;
	pDPNID = prgdpnid;

	pBilink = pNTEntry->m_bilinkMembership.GetNext();
	while (pBilink != &pNTEntry->m_bilinkMembership)
	{
		pGroupMember = CONTAINING_OBJECT(pBilink,CGroupMember,m_bilinkPlayers);
		dwCount++;
		if ( ( bOutputBufferTooSmall == FALSE ) && (dwCount <= *pcdpnid))
		{
			*pDPNID++ = pGroupMember->GetPlayer()->GetDPNID();
		}
		else
		{
			bOutputBufferTooSmall = TRUE;
		}
		pBilink = pBilink->GetNext();
	}

	pNTEntry->Unlock();
	pNTEntry->Release();
	pNTEntry = NULL;

	*pcdpnid = dwCount;

	//
	// if the user's output buffer appears to be incapable receiving
	// output, double-check to make sure that the output size requirement
	// isn't zero (which is really OK), before telling them that the
	// output buffer is too small
	//
	if ( ( bOutputBufferTooSmall ) && ( dwCount != 0 ) )
	{
		hResultCode = DPNERR_BUFFERTOOSMALL;
	}
	else
	{
		hResultCode = DPN_OK;
	}

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_EnumHosts"

HRESULT DIRECTNETOBJECT::DN_EnumHosts( PVOID pv,
						   DPN_APPLICATION_DESC *const pApplicationDesc,
                           IDirectPlay8Address *const pAddrHost,
						   IDirectPlay8Address *const pDeviceInfo,
						   PVOID const pUserEnumData,
						   const DWORD dwUserEnumDataSize,
						   const DWORD dwRetryCount,
						   const DWORD dwRetryInterval,
						   const DWORD dwTimeOut,
						   PVOID const pvAsyncContext,
						   DPNHANDLE *const pAsyncHandle,
						   const DWORD dwFlags )
{
	DIRECTNETOBJECT	*pdnObject = this;
	HRESULT		hResultCode;
	HRESULT		hrEnum;
	GUID		guidAdapter;
	GUID		guidHostSP;
	GUID		*pguid;
	CAsyncOp	*pParent;
	CAsyncOp	*pHandleParent;
	CSyncEvent	*pSyncEvent;
	CServiceProvider	*pSP;
	CRefCountBuffer		*pRCBuffer;
	DN_ENUM_QUERY	*pEnumQuery;
	IDirectPlay8Address	*pIHost;
	IDirectPlay8Address	*pIDevice;
	DPNHANDLE	handle;
	DWORD		dw;
	DPN_SP_CAPS	dnSPCaps;
	BOOL		fHosting;
#ifdef	DEBUG
	CHAR			DP8ABuffer[512];
	DWORD			DP8ASize;
#endif

	DPF(2,"Parameters: pApplicationDesc [0x%p], pAddrHost [0x%p], pDeviceInfo [0x%p], pUserEnumData [0x%p], dwUserEnumDataSize [%ld], dwRetryCount [%ld], dwRetryInterval [%ld], dwTimeOut [%ld], pvAsyncContext [0x%p], pAsyncHandle [0x%p], dwFlags [0x%lx]",
		pApplicationDesc,pAddrHost,pDeviceInfo,pUserEnumData,dwUserEnumDataSize,dwRetryCount,dwRetryInterval,dwTimeOut,pvAsyncContext,pAsyncHandle,dwFlags);

	RIP_ON_NOT_TRUE( "EnumHosts() Error validating enum hosts params", SUCCEEDED( DN_ValidateEnumHosts( pv, pApplicationDesc, pAddrHost,
    	                                                    pDeviceInfo, pUserEnumData, dwUserEnumDataSize,
    	                                                    dwRetryCount, dwRetryInterval, dwTimeOut,
															pvAsyncContext, pAsyncHandle, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "EnumHosts() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

	//
	// initialize
	//
	hResultCode = DPN_OK;
	pParent = NULL;
	pHandleParent = NULL;
	pSyncEvent = NULL;
	pRCBuffer = NULL;
	pSP = NULL;
	pIHost = NULL;
	pIDevice = NULL;
	handle = 0;

#ifdef	DEBUG
	if (pAddrHost)
	{
		DP8ASize = 512;
		pAddrHost->GetURLA(DP8ABuffer,&DP8ASize);
		DPF(4,"Host address [%s]",DP8ABuffer);
	}

	if (pDeviceInfo)
	{
		DP8ASize = 512;
		pDeviceInfo->GetURLA(DP8ABuffer,&DP8ASize);
		DPF(4,"Device address [%s]",DP8ABuffer);
	}
#endif

	//
	//	Cannot ENUM if Hosting - I have no idea why, but VanceO insisted on it
	//
	fHosting = FALSE;
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	if (pdnObject->dwFlags & (DN_OBJECT_FLAG_CONNECTED | DN_OBJECT_FLAG_CONNECTING))
	{
		CNameTableEntry		*pLocalPlayer;

		pLocalPlayer = NULL;

		DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
		if ((hResultCode = pdnObject->NameTable.GetLocalPlayerRef( &pLocalPlayer )) == DPN_OK)
		{
			if (pLocalPlayer->IsHost())
			{
				fHosting = TRUE;
			}
			pLocalPlayer->Release();
			pLocalPlayer = NULL;
		}
	}
	else
	{
		DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
	}

	if (fHosting)
	{
		hResultCode = DPNERR_HOSTING;
		goto Failure;
	}


	if ((hResultCode = DNGetActualSPCaps(pdnObject,NULL,&dnSPCaps)) != DPN_OK)
	{
		DPFERR("Could not get SP caps");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	//
	//	Ensure payload will fit
	//
	if (dwUserEnumDataSize > dnSPCaps.dwMaxEnumPayloadSize)
	{
		DPFERR("User enum data is too large");
		hResultCode = DPNERR_ENUMQUERYTOOLARGE;
		goto Failure;
	}

	//
	//	Duplicate addresses for local usage (so we can modify them if neccessary
	//
	if (pAddrHost)
	{
		// Use supplied Host address
		if ((hResultCode = pAddrHost->Duplicate(&pIHost)) != DPN_OK)
		{
			DPFERR("Could not duplicate Host address");
			DisplayDNError(0,hResultCode);
			hResultCode = DPNERR_INVALIDHOSTADDRESS;
			goto Failure;
		}
	}
	else
	{
		//
		//	Create new Host address and use Device SP guid
		//
		if ((hResultCode = DirectPlay8AddressCreate( IID_IDirectPlay8Address, (void **) &pIHost, NULL)) != DPN_OK)
		{
			DPFERR("Could not create Host address");
			DisplayDNError(0,hResultCode);
			DNASSERT(FALSE);
			goto Failure;
		}
	}

	if ((hResultCode = pDeviceInfo->Duplicate(&pIDevice)) != DPN_OK)
	{
		DPFERR("Could not duplicate Device address");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
		goto Failure;
	}

#ifdef	DEBUG
	DP8ASize = 512;
	pIHost->GetURLA(DP8ABuffer,&DP8ASize);
	DPF(4,"Host address [%s]",DP8ABuffer);

	DP8ASize = 512;
	pIDevice->GetURLA(DP8ABuffer,&DP8ASize);
	DPF(4,"Device address [%s]",DP8ABuffer);
#endif

	//
	//	Parent for ENUMs
	//
	if ((hResultCode = AsyncOpNew(pdnObject,&pParent)) != DPN_OK)
	{
		DPFERR("Could not create ENUM parent AsyncOp");
		DisplayDNError(0,hResultCode);
		DNASSERT(FALSE);
		goto Failure;
	}
	pParent->MakeParent();
	pParent->SetOpType( ASYNC_OP_ENUM_QUERY );
	pParent->SetContext( pvAsyncContext );
	pParent->SetCompletion( DNCompleteEnumQuery );

	//
	//	Synchronous ?
	//
		//
		//	Create Handle parent AsyncOp (if required)
		//
		if ((hResultCode = DNCreateUserHandle(pdnObject,&pHandleParent)) != DPN_OK)
		{
			DPFERR("Could not create AsyncOp");
			DisplayDNError(0,hResultCode);
			DNASSERT(FALSE);
			goto Failure;
		}
		pHandleParent->SetContext( pvAsyncContext );
		pHandleParent->Lock();
		if (pHandleParent->IsCancelled())
		{
			pHandleParent->Unlock();
			pParent->SetResult( DPNERR_USERCANCEL );
			hResultCode = DPNERR_USERCANCEL;
			goto Failure;
		}
		pParent->MakeChild( pHandleParent );
		handle = pHandleParent->GetHandle();
		pHandleParent->Unlock();

	//
	//	Ensure SP specified in Device address is loaded
	//
	if ((hResultCode = DN_SPEnsureLoaded(pdnObject,&GUID_NULL,&pSP)) != DPN_OK)
	{
		DPFERR("Could not ensure SP is loaded!");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pParent->SetSP( pSP );
	pSP->Release();
	pSP = NULL;

		void	*pvBlock;

		if ((pvBlock = MemoryBlockAlloc(pdnObject,sizeof(DN_ENUM_QUERY))) == NULL)
		{
			DPFERR("Could not allocate MemoryBlock");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		pEnumQuery = static_cast<DN_ENUM_QUERY*>(pvBlock);
		pEnumQuery->dwNumAdapters = 0;
		pEnumQuery->dwCurrentAdapter = 0;
		pParent->SetOpData( pvBlock );
		pvBlock = NULL;

	//
	//	Set up EnumQuery BufferDescriptions
	//
	//
	// When filling out the enum structure the SP requires an extra BUFFERDESC
	// to exist immediately before the one were passing with the user data.  The
	// SP will be using that extra buffer to prepend an optional header
	//

	pEnumQuery->BufferDesc[DN_ENUM_BUFFERDESC_QUERY_DN_PAYLOAD].pBufferData = reinterpret_cast<BYTE*>(&pEnumQuery->EnumQueryPayload);
	if (pApplicationDesc->guidApplication != GUID_NULL)
	{
		pEnumQuery->EnumQueryPayload.QueryType = DN_ENUM_QUERY_WITH_APPLICATION_GUID;
		memcpy(&pEnumQuery->EnumQueryPayload.guidApplication,&pApplicationDesc->guidApplication,sizeof(GUID));
		pEnumQuery->BufferDesc[DN_ENUM_BUFFERDESC_QUERY_DN_PAYLOAD].dwBufferSize = sizeof(DN_ENUM_QUERY_PAYLOAD);
	}
	else
	{
		pEnumQuery->EnumQueryPayload.QueryType = DN_ENUM_QUERY_WITHOUT_APPLICATION_GUID;
		pEnumQuery->BufferDesc[DN_ENUM_BUFFERDESC_QUERY_DN_PAYLOAD].dwBufferSize = sizeof(DN_ENUM_QUERY_PAYLOAD) - sizeof(GUID);
	}

	//
	//	Copy user data (if any)
	//
	if (pUserEnumData)
	{
		DNASSERT(dwUserEnumDataSize);
		if ((hResultCode = RefCountBufferNew(pdnObject,dwUserEnumDataSize,&pRCBuffer)) != DPN_OK)
		{
			DPFERR("Could not create RefCountBuffer");
			DisplayDNError(0,hResultCode);
			goto Failure;
		}
		memcpy(pRCBuffer->GetBufferAddress(),pUserEnumData,dwUserEnumDataSize);
		pEnumQuery->BufferDesc[DN_ENUM_BUFFERDESC_QUERY_USER_PAYLOAD].pBufferData = reinterpret_cast<BYTE*>(pRCBuffer->GetBufferAddress());
		pEnumQuery->BufferDesc[DN_ENUM_BUFFERDESC_QUERY_USER_PAYLOAD].dwBufferSize = dwUserEnumDataSize;
		pParent->SetRefCountBuffer( pRCBuffer );

		pRCBuffer->Release();
		pRCBuffer = NULL;
	}
	else
	{
		pEnumQuery->BufferDesc[DN_ENUM_BUFFERDESC_QUERY_USER_PAYLOAD].pBufferData = NULL;
		pEnumQuery->BufferDesc[DN_ENUM_BUFFERDESC_QUERY_USER_PAYLOAD].dwBufferSize = 0;
	}


	//
	//	Set up EnumQuery misc fields
	//
	pEnumQuery->dwRetryCount = dwRetryCount;
	pEnumQuery->dwRetryInterval = dwRetryInterval;
	pEnumQuery->dwTimeOut = dwTimeOut;
	pEnumQuery->dwFlags = dwFlags;

#pragma TODO( minara, "We do not need to send each of these parameters individually - they are already in a structure !" )
	hResultCode = DNPerformEnumQuery(	pdnObject,
										pIHost,
										pIDevice,
										pParent->GetSP()->GetHandle(),
										&pEnumQuery->BufferDesc[DN_ENUM_BUFFERDESC_QUERY_DN_PAYLOAD],
										DN_ENUM_BUFFERDESC_QUERY_COUNT,
										pEnumQuery->dwRetryCount,
										pEnumQuery->dwRetryInterval,
										pEnumQuery->dwTimeOut,
										pEnumQuery->dwFlags,
										pParent->GetContext(),
										pParent );
	if (hResultCode != DPN_OK)
	{
		DPFERR("Could not start ENUM");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
											
	pParent->Release();
	pParent = NULL;

	//
	//	Wait for SyncEvent or return Async Handle
	//
		//
		//	Blame vanceo if this EVER returns anything other than DPN_OK at this stage
		//
		pHandleParent->SetCompletion( DNCompleteAsyncHandle );
		pHandleParent->Release();
		pHandleParent = NULL;

		*pAsyncHandle = handle;
		hResultCode = DPNERR_PENDING;

	pIDevice->Release();
	pIDevice = NULL;

	pIHost->Release();
	pIHost = NULL;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	DNASSERT(hResultCode != DPNERR_INVALIDENDPOINT);
	return(hResultCode);

Failure:
	if (handle != 0)
	{
		pdnObject->HandleTable.Destroy( handle );
		handle = 0;
	}
	if (pParent)
	{
		pParent->Release();
		pParent = NULL;
	}
	if (pHandleParent)
	{
		pHandleParent->Release();
		pHandleParent = NULL;
	}
	if (pSyncEvent)
	{
		pSyncEvent->ReturnSelfToPool();
		pSyncEvent = NULL;
	}
	if (pRCBuffer)
	{
		pRCBuffer->Release();
		pRCBuffer = NULL;
	}
	if (pSP)
	{
		pSP->Release();
		pSP = NULL;
	}
	if (pIHost)
	{
		pIHost->Release();
		pIHost = NULL;
	}
	if (pIDevice)
	{
		pIDevice->Release();
		pIDevice = NULL;
	}
	goto Exit;
}


//**********************************************************************



//	DN_DestroyPlayer
//
//	Remove a player from this DirectNet session
//	This will send a termination message to the player.
//	Both the host and the player will terminate.

#undef DPF_MODNAME
#define DPF_MODNAME "DN_DestroyPlayer"

HRESULT DIRECTNETOBJECT::DN_DestroyPlayer(PVOID pInterface,
							  const DPNID dpnid,
							  const void *const pvDestroyData,
							  const DWORD dwDestroyDataSize,
							  const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	HRESULT				hResultCode;
	CRefCountBuffer		*pRefCountBuffer;
	CNameTableEntry		*pNTEntry;
	CConnection			*pConnection;
	DN_INTERNAL_MESSAGE_TERMINATE_SESSION	*pMsg;

	DPF(3,"Parameters: pInterface [0x%p], dpnid [0x%lx], pvDestroyData [0x%p], dwDestroyDataSize [%ld], dwFlags [0x%lx]",
			pInterface,dpnid,pvDestroyData,dwDestroyDataSize,dwFlags);

	RIP_ON_NOT_TRUE( "DestroyPlayer() Error validating destroy player params", SUCCEEDED( DN_ValidateDestroyPlayer( pInterface, dpnid, pvDestroyData, dwDestroyDataSize, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "DestroyPlayer() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to create a group" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	

	pNTEntry = NULL;
	pRefCountBuffer = NULL;
	pConnection = NULL;

	if (!DN_CHECK_LOCALHOST(pdnObject))
	{
	    DPFERR( "Object is not session host, cannot destroy players" );
		DPF_RETURN(DPNERR_NOTHOST);
	}

	// Ensure DNID specified is valid
	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnid,&pNTEntry)) != DPN_OK)
	{
		DPFERR("Could not find player entry");
		DisplayDNError(0,hResultCode);
		if ((hResultCode = pdnObject->NameTable.FindDeletedEntry(dpnid,&pNTEntry)) != DPN_OK)
		{
			hResultCode = DPNERR_INVALIDPLAYER;
			goto Failure;
		}
		pNTEntry->Release();
		pNTEntry = NULL;
		hResultCode = DPNERR_CONNECTIONLOST;
		goto Failure;
	}
	if (pNTEntry->IsLocal() )
	{
		DPFERR( "Cannot destroy local player" );
		hResultCode = DPNERR_INVALIDPLAYER;		
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	if (pNTEntry->IsGroup())
	{
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
	}
	if ((hResultCode = pNTEntry->GetConnectionRef( &pConnection )) != DPN_OK)
	{
		DPFERR("Could not get connection ref");
		DisplayDNError(0,hResultCode);
		hResultCode = DPNERR_CONNECTIONLOST;
		goto Failure;
	}
	pNTEntry->Release();
	pNTEntry = NULL;

	//
	//	Build terminate message
	//
	if ((hResultCode = RefCountBufferNew(pdnObject,
			sizeof(DN_INTERNAL_MESSAGE_TERMINATE_SESSION) + dwDestroyDataSize,&pRefCountBuffer)) != DPN_OK)
	{
		DPFERR("Could not allocate RefCountBuffer");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pMsg = reinterpret_cast<DN_INTERNAL_MESSAGE_TERMINATE_SESSION*>(pRefCountBuffer->GetBufferAddress());
	if (dwDestroyDataSize)
	{
		memcpy(pMsg+1,pvDestroyData,dwDestroyDataSize);
		pMsg->dwTerminateDataOffset = sizeof(DN_INTERNAL_MESSAGE_TERMINATE_SESSION);
	}
	else
	{
		pMsg->dwTerminateDataOffset = 0;
	}
	pMsg->dwTerminateDataSize = dwDestroyDataSize;

	//
	//	Send message to player to exit
	//
	hResultCode = DNSendMessage(pdnObject,
								pConnection,
								DN_MSG_INTERNAL_TERMINATE_SESSION,
								dpnid,
								pRefCountBuffer->BufferDescAddress(),
								pRefCountBuffer,
								0,
								DN_SENDFLAGS_RELIABLE,
								NULL,
								NULL);
	if (hResultCode != DPNERR_PENDING)
	{
		DPFERR("Could not send DESTROY_CLIENT message to player");
		DisplayDNError(0,hResultCode);
		if (hResultCode == DPNERR_INVALIDENDPOINT)
		{
			hResultCode = DPNERR_INVALIDPLAYER;
		}
		goto Failure;
	}

	pConnection->Release();
	pConnection = NULL;

	pRefCountBuffer->Release();
	pRefCountBuffer = NULL;

	//
	//	Remove from NameTable and inform other players of disconnect
	//
	hResultCode = DNHostDisconnect(pdnObject,dpnid,DPNDESTROYPLAYERREASON_HOSTDESTROYEDPLAYER);

	hResultCode = DPN_OK;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	DNASSERT(hResultCode != DPNERR_INVALIDENDPOINT);
	return(hResultCode);

Failure:
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	if (pConnection)
	{
		pConnection->Release();
		pConnection = NULL;
	}
	if (pRefCountBuffer)
	{
		pRefCountBuffer->Release();
		pRefCountBuffer = NULL;
	}
	goto Exit;
}



//	DN_ReturnBuffer
//
//	Return a receive buffer which is no longer in use

#undef DPF_MODNAME
#define DPF_MODNAME "DN_ReturnBuffer"

HRESULT DIRECTNETOBJECT::DN_ReturnBuffer(PVOID pv,
							 const DPNHANDLE hBufferHandle,
							 const DWORD dwFlags)
{
	DIRECTNETOBJECT	*pdnObject = this;
	HRESULT			hResultCode;
	CAsyncOp		*pAsyncOp;

	DPF(2,"Parameters: hBufferHandle [0x%lx], dwFlags [0x%lx]",hBufferHandle,dwFlags);

	RIP_ON_NOT_TRUE( "ReturnBuffer() Error validating return buffer params", SUCCEEDED( DN_ValidateReturnBuffer( pv, hBufferHandle, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "ReturnBuffer() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to create a group" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	

	pAsyncOp = NULL;

	//
	//	Find async op
	//
	if ((hResultCode = pdnObject->HandleTable.Find( hBufferHandle,&pAsyncOp )) != DPN_OK)
	{
		DPFERR("Could not find handle");
		DisplayDNError(0,hResultCode);
		hResultCode = DPNERR_INVALIDHANDLE;
		goto Failure;
	}

	//
	//	Ensure it's not already cancelled
	//
	pAsyncOp->Lock();
	if (pAsyncOp->IsCancelled() || pAsyncOp->IsComplete())
	{
		pAsyncOp->Unlock();
		hResultCode = DPNERR_INVALIDHANDLE;
		goto Failure;
	}
	pAsyncOp->SetComplete();
	pAsyncOp->Unlock();

	if ((hResultCode = pdnObject->HandleTable.Destroy( hBufferHandle )) == DPN_OK)
	{
		//
		//	Remove from active list
		//
		DNEnterCriticalSection(&pdnObject->csActiveList);
		pAsyncOp->m_bilinkActiveList.RemoveFromList();
		DNLeaveCriticalSection(&pdnObject->csActiveList);
	}

	pAsyncOp->Release();
	pAsyncOp = NULL;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pAsyncOp)
	{
		pAsyncOp->Release();
		pAsyncOp = NULL;
	}
	goto Exit;
}


//	DN_GetPlayerContext

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetPlayerContext"

HRESULT DIRECTNETOBJECT::DN_GetPlayerContext(PVOID pv,
								 const DPNID dpnid,
								 PVOID *const ppvPlayerContext,
								 const DWORD dwFlags)
{
	HRESULT				hResultCode;
	CNameTableEntry		*pNTEntry;
	DIRECTNETOBJECT		*pdnObject = this;

	DPF(2,"Parameters: pv [0x%p], dpnid [0x%lx], ppvPlayerContext [0x%p], dwFlags [0x%lx]",
			pv, dpnid,ppvPlayerContext,dwFlags);

	RIP_ON_NOT_TRUE( "GetPlayerContext() Error validating getplayercontext params", SUCCEEDED( DN_ValidateGetPlayerContext( pv, dpnid, ppvPlayerContext, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "GetPlayerContext() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & (DN_OBJECT_FLAG_CONNECTED | DN_OBJECT_FLAG_DISCONNECTING)) )
    	{
    	    DPFERR("You must be connected / hosting to get player context" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	

	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnid,&pNTEntry)) != DPN_OK)
	{
		DPFERR("Could not retrieve player entry");
		DisplayDNError(0,hResultCode);

		//
		//	Try deleted list
		//
		if ((hResultCode = pdnObject->NameTable.FindDeletedEntry(dpnid,&pNTEntry)) != DPN_OK)
		{
			DPFERR("Could not find player in deleted list either");
			DisplayDNError(0,hResultCode);
			hResultCode = DPNERR_INVALIDPLAYER;
			goto Failure;
		}
	}

	//
	//	Ensure player is available (ADD_PLAYER has been sent but not DELETE_PLAYER),
	//	and that this is not a group.
	//
	pNTEntry->Lock();
	if (!pNTEntry->IsCreated() || pNTEntry->IsGroup())
	{
		pNTEntry->Unlock();
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
	}

	*ppvPlayerContext = pNTEntry->GetContext();
	pNTEntry->Unlock();
	pNTEntry->Release();
	pNTEntry = NULL;

	hResultCode = DPN_OK;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	goto Exit;
}


//	DN_GetGroupContext

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetGroupContext"

HRESULT DIRECTNETOBJECT::DN_GetGroupContext(PVOID pv,
								const DPNID dpnid,
								PVOID *const ppvGroupContext,
								const DWORD dwFlags)
{
	HRESULT				hResultCode;
	CNameTableEntry		*pNTEntry;
	DIRECTNETOBJECT		*pdnObject = this;

	DPF(2,"Parameters: pv [0x%p], dpnid [0x%lx], ppvGroupContext [0x%p], dwFlags [0x%lx]",
			pv, dpnid,ppvGroupContext,dwFlags);

	RIP_ON_NOT_TRUE( "GetGroupContext() Error validating getgroupcontext params", SUCCEEDED( DN_ValidateGetGroupContext( pv, dpnid, ppvGroupContext,dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "GetGroupContext() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & (DN_OBJECT_FLAG_CONNECTED | DN_OBJECT_FLAG_DISCONNECTING)) )
    	{
    	    DPFERR("You must be connected / hosting to get group context" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	

	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnid,&pNTEntry)) != DPN_OK)
	{
		DPFERR("Could not retrieve group entry");
		DisplayDNError(0,hResultCode);

		//
		//	Try deleted list
		//
		if ((hResultCode = pdnObject->NameTable.FindDeletedEntry(dpnid,&pNTEntry)) != DPN_OK)
		{
			DPFERR("Could not find player in deleted list either");
			DisplayDNError(0,hResultCode);
			hResultCode = DPNERR_INVALIDGROUP;
			goto Failure;
		}
	}

	//
	//	Ensure that group is available (CREATE_GROUP has been sent, but not DESTROY_GROUP),
	//	and that this is a not a player.
	//
	pNTEntry->Lock();
	if (!pNTEntry->IsAvailable() || !pNTEntry->IsGroup())
	{
		pNTEntry->Unlock();
		hResultCode = DPNERR_INVALIDGROUP;
		goto Failure;
	}

	if( pNTEntry->IsAllPlayersGroup() )
	{
		pNTEntry->Unlock();
		DPFERR("Cannot getcontext for the all players group" );
		hResultCode = DPNERR_INVALIDGROUP;
		goto Failure;
	}

	*ppvGroupContext = pNTEntry->GetContext();
	pNTEntry->Unlock();
	pNTEntry->Release();
	pNTEntry = NULL;

	hResultCode = DPN_OK;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_RegisterLobby"

HRESULT DIRECTNETOBJECT::DN_RegisterLobby(PVOID pInterface,
							  const DPNHANDLE dpnhLobbyConnection, 
							  VOID *const pIDP8LobbiedApplication,
							  const DWORD dwFlags)
{
	return DPNERR_UNSUPPORTED;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_TerminateSession"

HRESULT DIRECTNETOBJECT::DN_TerminateSession(PVOID pInterface,
								 void *const pvTerminateData,
								 const DWORD dwTerminateDataSize,
								 const DWORD dwFlags)
{
	HRESULT				hResultCode;
	DIRECTNETOBJECT		*pdnObject = this;
	CRefCountBuffer		*pRefCountBuffer;
	CWorkerJob			*pWorkerJob;
	DN_INTERNAL_MESSAGE_TERMINATE_SESSION	*pMsg;

	DPF(2,"Parameters: pInterface [0x%p], pvTerminateData [0x%p], dwTerminateDataSize [%ld], dwFlags [0x%lx]",
			pInterface,pvTerminateData,dwTerminateDataSize,dwFlags);

	RIP_ON_NOT_TRUE( "TerminateSession() Error validating terminatesession params", SUCCEEDED( DN_ValidateTerminateSession( pInterface, pvTerminateData, dwTerminateDataSize, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "TerminateSession() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to create a group" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	

	pRefCountBuffer = NULL;
	pWorkerJob = NULL;

	if (!DN_CHECK_LOCALHOST(pdnObject))
	{
	    DPFERR( "Object is not session host, cannot destroy players" );
		hResultCode = DPNERR_NOTHOST;
		goto Failure;
	}	

	//
	//	Build terminate message
	//
	if ((hResultCode = RefCountBufferNew(pdnObject,
			sizeof(DN_INTERNAL_MESSAGE_TERMINATE_SESSION) + dwTerminateDataSize,&pRefCountBuffer)) != DPN_OK)
	{
		DPFERR("Could not allocate RefCountBuffer");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pMsg = reinterpret_cast<DN_INTERNAL_MESSAGE_TERMINATE_SESSION*>(pRefCountBuffer->GetBufferAddress());
	if (dwTerminateDataSize)
	{
		memcpy(pMsg+1,pvTerminateData,dwTerminateDataSize);
		pMsg->dwTerminateDataOffset = sizeof(DN_INTERNAL_MESSAGE_TERMINATE_SESSION);
	}
	else
	{
		pMsg->dwTerminateDataOffset = 0;
	}
	pMsg->dwTerminateDataSize = dwTerminateDataSize;

	//
	//	Worker job to send message to all players
	//
	if ((hResultCode = WorkerJobNew(pdnObject,&pWorkerJob)) != DPN_OK)
	{
		DPFERR("Could not allocate new WorkerJob");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pWorkerJob->SetJobType( WORKER_JOB_SEND_NAMETABLE_OPERATION );
	pWorkerJob->SetSendNameTableOperationMsgId( DN_MSG_INTERNAL_TERMINATE_SESSION );
	pWorkerJob->SetSendNameTableOperationVersion( 0 );
	pWorkerJob->SetSendNameTableOperationDPNIDExclude( 0 );
	pWorkerJob->SetRefCountBuffer( pRefCountBuffer );

	DNQueueWorkerJob(pdnObject,pWorkerJob);
	pWorkerJob = NULL;

	pRefCountBuffer->Release();
	pRefCountBuffer = NULL;

	//
	//	Terminate local session
	//
	hResultCode = DNUserTerminateSession(pdnObject,
										 DPNERR_HOSTTERMINATEDSESSION,
										 pvTerminateData,
										 dwTerminateDataSize);

	if ((hResultCode = WorkerJobNew(pdnObject,&pWorkerJob)) != DPN_OK)
	{
		DPFERR("Could not create WorkerJob");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pWorkerJob->SetJobType( WORKER_JOB_TERMINATE_SESSION );
	pWorkerJob->SetTerminateSessionReason( DPNERR_HOSTTERMINATEDSESSION );

	DNQueueWorkerJob(pdnObject,pWorkerJob);
	pWorkerJob = NULL;

	hResultCode = DPN_OK;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pRefCountBuffer)
	{
		pRefCountBuffer->Release();
		pRefCountBuffer = NULL;
	}
	goto Exit;
}

//
//	FUnction that performs work for DN_GetHostAddress and for Lobby informs
//
#undef DPF_MODNAME
#define DPF_MODNAME "DNGetHostAddressHelper"

HRESULT DNGetHostAddressHelper(DIRECTNETOBJECT *pdnObject, 
							   IDirectPlay8Address **const prgpAddress,
							   DWORD *const pcAddress)
{
	CAsyncOp		*pListenParent;
	CAsyncOp		*pListenSP;
	CAsyncOp		*pListen;
	CBilink			*pBilinkSP;
	CBilink			*pBilink;
	DWORD			dwListenCount;
	SPGETADDRESSINFODATA	spInfoData;
	CNameTableEntry			*pLocalPlayer;
	IDirectPlay8Address		**ppAddress;
	HRESULT hResultCode;

	pListenParent = NULL;
	pListenSP = NULL;
	pListen = NULL;	
	pLocalPlayer = NULL;
	
	if ((hResultCode = pdnObject->NameTable.GetLocalPlayerRef( &pLocalPlayer )) != DPN_OK)
	{
		DPFERR("Could not get local player reference");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	if (	!(pdnObject->dwFlags & DN_OBJECT_FLAG_LISTENING)
		||	!pLocalPlayer->IsHost()
		||	(pdnObject->pListenParent == NULL))

	{
		DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
		DPFERR("Not listening or Host player");
		hResultCode = DPNERR_NOTHOST;
		goto Failure;
	}
	pdnObject->pListenParent->AddRef();
	pListenParent = pdnObject->pListenParent;
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	pLocalPlayer->Release();
	pLocalPlayer = NULL;

	//
	//	Ensure that the address pointer buffer is large enough
	//
	dwListenCount = 0;
	pListenParent->Lock();	// Prevent changes while we run through
	pBilinkSP = pListenParent->m_bilinkParent.GetNext();
	while (pBilinkSP != &pListenParent->m_bilinkParent)
	{
		pListenSP = CONTAINING_OBJECT(pBilinkSP,CAsyncOp,m_bilinkChildren);

		pListenSP->Lock();

		pBilink = pListenSP->m_bilinkParent.GetNext();
		while (pBilink != &pListenSP->m_bilinkParent)
		{
			dwListenCount++;
			pBilink = pBilink->GetNext();
		}

		pListenSP->Unlock();
		pListenSP = NULL;

		pBilinkSP = pBilinkSP->GetNext();
	}

	if (dwListenCount > *pcAddress)
	{
		pListenParent->Unlock();
		*pcAddress = dwListenCount;
		hResultCode = DPNERR_BUFFERTOOSMALL;
		goto Failure;
	}

	//
	//	Get addresses of LISTENs
	//

	ppAddress = prgpAddress;
	dwListenCount = 0;
	pBilinkSP = pListenParent->m_bilinkParent.GetNext();
	while (pBilinkSP != &pListenParent->m_bilinkParent)
	{
		pListenSP = CONTAINING_OBJECT(pBilinkSP,CAsyncOp,m_bilinkChildren);

		pListenSP->Lock();

		pBilink = pListenSP->m_bilinkParent.GetNext();
		while (pBilink != &pListenSP->m_bilinkParent)
		{
			pListen = CONTAINING_OBJECT(pBilink,CAsyncOp,m_bilinkChildren);

			if (pListen->GetProtocolHandle() != NULL)
			{
				spInfoData.hEndpoint = pListen->GetProtocolHandle();
				spInfoData.Flags = SP_GET_ADDRESS_INFO_LISTEN_HOST_ADDRESSES;
				pListen = NULL;
				if ((hResultCode = DNPGetListenAddressInfo(spInfoData.hEndpoint,&spInfoData)) != DPN_OK)
				{
					DPFERR("Could not get LISTEN address - ignore and continue");
					DisplayDNError(0,hResultCode);
					DNASSERT(FALSE);
					pBilink = pBilink->GetNext();
					continue;
				}
				*ppAddress++ = spInfoData.pAddress;
				dwListenCount++;
			}

			pBilink = pBilink->GetNext();
		}

		pListenSP->Unlock();
		pListenSP = NULL;

		pBilinkSP = pBilinkSP->GetNext();
	}
	pListenParent->Unlock();

	*pcAddress = dwListenCount;

	hResultCode = DPN_OK;

	pListenParent->Release();
	pListenParent = NULL;

Exit:
	DPF_RETURN(hResultCode);

Failure:
	if (pListenParent)
	{
		pListenParent->Release();
		pListenParent = NULL;
	}
	if (pLocalPlayer)
	{
		pLocalPlayer->Release();
		pLocalPlayer = NULL;
	}
	goto Exit;
	
}

//
//	DN_GetHostAddress
//
//	We will determine the host addresses by examining the LISTENs which are running.
//	We do this because after Host migration, we may not be running the same LISTEN
//	we started with.

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetHostAddress"

HRESULT DIRECTNETOBJECT::DN_GetHostAddress(PVOID pInterface,
							   IDirectPlay8Address **const prgpAddress,
							   DWORD *const pcAddress,
							   const DWORD dwFlags)
{
	HRESULT			hResultCode;
	DIRECTNETOBJECT	*pdnObject = this;

	DPF(3,"Parameters: pInterface [0x%p], prgpAddress [0x%p], pcAddress [0x%p], dwFlags [0x%lx]",
			pInterface,prgpAddress,pcAddress,dwFlags);

	RIP_ON_NOT_TRUE( "GetHostAddress() Error validating gethostaddress params", SUCCEEDED( DN_ValidateGetHostAddress( pInterface, prgpAddress, pcAddress, dwFlags ) ) );

    	// Check to ensure message handler registered
    	RIP_ON_NOT_TRUE( "GetHostAddress() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) )
    	{
    	    DPFERR("You must be connected / hosting to create a group" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	

	// Actually do the work and get the addresses  
	hResultCode = DNGetHostAddressHelper( pdnObject, prgpAddress, pcAddress );

	DPF_RETURN(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DNGetMaxEnumFrameSize"

HRESULT DNGetMaxEnumFrameSize(DIRECTNETOBJECT *const pdnObject,
							  DWORD *const pdwEnumFrameSize)
{
	HRESULT				hResultCode;
	DWORD				dwMinMax;
	BOOL				fSet;
    SPGETCAPSDATA		spGetCapsData;
    IDP8ServiceProvider	*pIDP8SP;
	CBilink				*pBilink;
	CServiceProvider	*pSP;

	DPF(8,"Parameters: pdwEnumFrameSize [0x%p]",pdwEnumFrameSize);

	DNASSERT(pdnObject != NULL);
	DNASSERT(pdwEnumFrameSize != NULL);

	dwMinMax = 0;
	fSet = FALSE;

	DNEnterCriticalSection(&pdnObject->csServiceProviders);
	pBilink = pdnObject->m_bilinkServiceProviders.GetNext();
	while (pBilink != &pdnObject->m_bilinkServiceProviders)
	{
		pSP = CONTAINING_OBJECT(pBilink,CServiceProvider,m_bilinkServiceProviders);

		//
		//	Get SPCaps for this SP
		//
		if ((hResultCode = pSP->GetInterfaceRef( &pIDP8SP )) == DPN_OK)
		{
			memset( &spGetCapsData, 0x00, sizeof( SPGETCAPSDATA ) );
			spGetCapsData.dwSize = sizeof( SPGETCAPSDATA );
			spGetCapsData.hEndpoint = INVALID_HANDLE_VALUE;
			if ((hResultCode = pIDP8SP->lpVtbl->GetCaps( pIDP8SP, &spGetCapsData )) == DPN_OK)
			{
				if (!fSet || (spGetCapsData.dwEnumFrameSize < dwMinMax))
				{
					dwMinMax = spGetCapsData.dwEnumFrameSize;
					fSet = TRUE;
				}
			}

			pIDP8SP->lpVtbl->Release( pIDP8SP );
			pIDP8SP = NULL;
		}

		pBilink = pBilink->GetNext();
	}
	DNLeaveCriticalSection(&pdnObject->csServiceProviders);

	if (fSet)
	{
		DPF(9,"Setting *pdwEnumFrameSize [%ld]",dwMinMax);
		*pdwEnumFrameSize = dwMinMax;
		hResultCode = DPN_OK;
	}
	else
	{
		hResultCode = DPNERR_GENERIC;
	}

	DPF(8,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_DumpNameTable"

HRESULT DIRECTNETOBJECT::DN_DumpNameTable(PVOID pInterface,char *const Buffer)
{
	DIRECTNETOBJECT	*pdnObject = this;

	DNASSERT(pInterface != NULL);
	DNASSERT(Buffer != NULL);

	pdnObject->NameTable.DumpNameTable(Buffer);

#if 0
	//
	//	TEMP - for test purposes only
	//
/*	REMOVE start */
	if (pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED)
	{
		if (pdnObject->NameTable.GetLocalPlayer()->GetDPNID() == (DPNID)0x400004)
		{
			CNameTableEntry	*pNTEntry = NULL;

			if (pdnObject->NameTable.FindEntry(0x300003,&pNTEntry) == DPN_OK)
			{
				pNTEntry->GetConnection()->Disconnect();
				pNTEntry->Release();
				pNTEntry = NULL;
			}
		}
	}
/*	REMOVE end */
#endif
	return(DPN_OK);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DNAddRefLock"

HRESULT DNAddRefLock(DIRECTNETOBJECT *const pdnObject)
{
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	if ((pdnObject->dwFlags & (DN_OBJECT_FLAG_CLOSING | DN_OBJECT_FLAG_DISCONNECTING)) ||
			!(pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED))
	{
		DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
		return(DPNERR_ALREADYCLOSING);
	}
	pdnObject->dwLockCount++;
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	return(DPN_OK);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DNDecRefLock"

void DNDecRefLock(DIRECTNETOBJECT *const pdnObject)
{
	BOOL	fSetEvent;

	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	pdnObject->dwLockCount--;
	if ((pdnObject->dwLockCount == 0) && (pdnObject->dwFlags & DN_OBJECT_FLAG_CLOSING))
	{
		fSetEvent = TRUE;
	}
	else
	{
		fSetEvent = FALSE;
	}
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);

	if (fSetEvent)
	{
		SetEvent(pdnObject->hLockEvent);
	}
}
