/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Peer.cpp
 *  Content:    DNET peer interface routines
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/21/99	mjn		Created
 *  12/23/99	mjn		Hand all NameTable update sends from Host to worker thread
 *	12/28/99	mjn		Disconnect handling happens when disconnect finishes instead of starts
 *	12/28/99	mjn		Moved Async Op stuff to Async.h
 *	01/04/00	mjn		Added code to allow outstanding ops to complete at host migration
 *	01/06/00	mjn		Moved NameTable stuff to NameTable.h
 *	01/11/00	mjn		Moved connect/disconnect stuff to Connect.h
 *	01/14/00	mjn		Added pvUserContext to Host API call
 *	01/16/00	mjn		Moved User callback stuff to User.h
 *	01/22/00	mjn		Implemented DestroyClient in API
 *	01/28/00	mjn		Implemented ReturnBuffer in API
 *	02/01/00	mjn		Implemented GetCaps and SetCaps in API
 *	02/01/00	mjn		Implement Player/Group context values
 *	02/15/00	mjn		Implement INFO flags in SetInfo and return context in GetInfo
 *	02/17/00	mjn		Implemented GetPlayerContext and GetGroupContext
 *  03/17/00    rmt     Added new caps functions
 *	04/04/00	mjn		Added TerminateSession to API
 *	04/05/00	mjn		Modified DestroyClient
 *	04/06/00	mjn		Added GetPeerAddress to API
 *				mjn		Added GetHostAddress to API
 *  04/17/00    rmt     Added more parameter validation
 *              rmt     Removed required for connection from Get/SetInfo / GetAddress
 *	04/19/00	mjn		SendTo API call accepts a range of DPN_BUFFER_DESCs and a count
 *	04/24/00	mjn		Updated Group and Info operations to use CAsyncOp's
 *	05/31/00	mjn		Added operation specific SYNC flags
 *	06/23/00	mjn		Removed dwPriority from SendTo() API call
 *	07/09/00	mjn		Cleaned up DN_SetPeerInfo()
 *  07/09/00	rmt		Bug #38323 - RegisterLobby needs a DPNHANDLE parameter.
 *  07/21/00    RichGr  IA64: Use %p format specifier for 32/64-bit pointers.
 *  08/03/00	rmt		Bug #41244 - Wrong return codes -- part 2
 *	09/13/00	mjn		Fixed return value from DN_GetPeerAddress() if peer not found
 *	10/11/00	mjn		Take locks for CNameTableEntry::PackInfo()
 *				mjn		Check deleted list in DN_GetPeerInfo()
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dncorei.h"

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


ULONG WINAPI IDirectPlay8Peer_AddRef(LPDIRECTPLAY8PEER pDirectPlay8Peer)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->AddRef();
}
ULONG WINAPI IDirectPlay8Peer_Release(LPDIRECTPLAY8PEER pDirectPlay8Peer)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->Release();
}

HRESULT WINAPI IDirectPlay8Peer_Initialize(LPDIRECTPLAY8PEER pDirectPlay8Peer, LPVOID const lpvUserContext, const PFNDPNMESSAGEHANDLER lpfn, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_Initialize(NULL, lpvUserContext,lpfn,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_EnumServiceProviders(LPDIRECTPLAY8PEER pDirectPlay8Peer, const GUID *const pvoid,const GUID *const pguidApplication,DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer,DWORD *const pcbEnumData,DWORD *const pcReturned,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_EnumServiceProviders(NULL, pvoid,pguidApplication,pSPInfoBuffer,pcbEnumData,pcReturned,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_CancelAsyncOperation(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNHANDLE hAsyncHandle, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_CancelAsyncOperation(NULL, hAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_Connect(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address *const pHostAddr,IDirectPlay8Address *const pDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,const void *const pvUserConnectData,const DWORD dwUserConnectDataSize,void *const pvPlayerContext,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_Connect(NULL, pdnAppDesc,pHostAddr,pDeviceInfo,pdnSecurity,pdnCredentials,pvUserConnectData,dwUserConnectDataSize,pvPlayerContext,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_SendTo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dnid, const DPN_BUFFER_DESC *const prgBufferDesc,const DWORD cBufferDesc,const DWORD dwTimeOut, void *const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_SendTo(NULL, dnid,prgBufferDesc,cBufferDesc,dwTimeOut,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetSendQueueInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,DWORD *const lpdwNumMsgs, DWORD *const lpdwNumBytes, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetSendQueueInfo(NULL, dpnid,lpdwNumMsgs,lpdwNumBytes,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_Host(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address **const prgpDeviceInfo,const DWORD cDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,void *const pvPlayerContext,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_Host(NULL, pdnAppDesc,prgpDeviceInfo,cDeviceInfo,pdnSecurity,pdnCredentials,pvPlayerContext,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetApplicationDesc(LPDIRECTPLAY8PEER pDirectPlay8Peer, DPN_APPLICATION_DESC *const pAppDescBuffer,DWORD *const lpcbDataSize,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetApplicationDesc(NULL, pAppDescBuffer,lpcbDataSize,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_SetApplicationDesc(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_APPLICATION_DESC *const lpad, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_SetApplicationDesc(NULL, lpad,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_CreateGroup(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_GROUP_INFO *const pdpnGroupInfo,void *const pvGroupContext,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_CreateGroup(NULL, pdpnGroupInfo,pvGroupContext,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_DestroyGroup(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID idGroup ,PVOID const pvUserContext,DPNHANDLE *const lpAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_DestroyGroup(NULL, idGroup,pvUserContext,lpAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_AddPlayerToGroup(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID idGroup, const DPNID idClient ,PVOID const pvUserContext,DPNHANDLE *const lpAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_AddClientToGroup(NULL, idGroup,idClient,pvUserContext,lpAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_RemovePlayerFromGroup(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID idGroup, const DPNID idClient,PVOID const pvUserContext,DPNHANDLE *const lpAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_RemoveClientFromGroup(NULL, idGroup,idClient,pvUserContext,lpAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_SetGroupInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_SetGroupInfo(NULL, dpnid,pdpnGroupInfo,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetGroupInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,DWORD *const pdwSize,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetGroupInfo(NULL, dpnid,pdpnGroupInfo,pdwSize,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_EnumPlayersAndGroups(LPDIRECTPLAY8PEER pDirectPlay8Peer, DPNID *const lprgdnid, DWORD *const lpcdnid, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_EnumClientsAndGroups(NULL, lprgdnid,lpcdnid,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_EnumGroupMembers(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dnid, DPNID *const lprgdnid, DWORD *const lpcdnid, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_EnumGroupMembers(NULL, dnid,lprgdnid,lpcdnid,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_SetPeerInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_PLAYER_INFO *const pdpnPlayerInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_SetPeerInfo(NULL, pdpnPlayerInfo,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetPeerInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,DPN_PLAYER_INFO *const pdpnPlayerInfo,DWORD *const pdwSize,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetPeerInfo(NULL, dpnid,pdpnPlayerInfo,pdwSize,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetPeerAddress(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,IDirectPlay8Address **const pAddress,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetPeerAddress(NULL, dpnid,pAddress,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetLocalHostAddresses(LPDIRECTPLAY8PEER pDirectPlay8Peer, IDirectPlay8Address **const prgpAddress, DWORD *const pcAddress,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetHostAddress(NULL, prgpAddress,pcAddress,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_Close(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_Close(NULL, dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_EnumHosts(LPDIRECTPLAY8PEER pDirectPlay8Peer, PDPN_APPLICATION_DESC const pApplicationDesc,IDirectPlay8Address *const pAddrHost,IDirectPlay8Address *const pDeviceInfo,PVOID const pUserEnumData,const DWORD dwUserEnumDataSize,const DWORD dwRetryCount,const DWORD dwRetryInterval,const DWORD dwTimeOut,PVOID const pvUserContext,DPNHANDLE *const pAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_EnumHosts(NULL, pApplicationDesc,pAddrHost,pDeviceInfo,pUserEnumData,dwUserEnumDataSize,dwRetryCount,dwRetryInterval,dwTimeOut,pvUserContext,pAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_DestroyPeer(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dnid,const void *const pvDestroyData,const DWORD dwDestroyDataSize,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_DestroyPlayer(NULL, dnid,pvDestroyData,dwDestroyDataSize,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_ReturnBuffer(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNHANDLE hBufferHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_ReturnBuffer(NULL, hBufferHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetPlayerContext(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,PVOID *const ppvPlayerContext,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetPlayerContext(NULL, dpnid,ppvPlayerContext,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetGroupContext(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid,PVOID *const ppvGroupContext,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetGroupContext(NULL, dpnid,ppvGroupContext,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetCaps(LPDIRECTPLAY8PEER pDirectPlay8Peer, DPN_CAPS *const pdnCaps,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetCaps(NULL, pdnCaps,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_SetCaps(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPN_CAPS *const pdnCaps,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_SetCaps(NULL, pdnCaps,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_SetSPCaps(LPDIRECTPLAY8PEER pDirectPlay8Peer, const GUID *const pvoid, const DPN_SP_CAPS *const pdpspCaps, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_SetSPCaps(NULL, pvoid,pdpspCaps,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetSPCaps(LPDIRECTPLAY8PEER pDirectPlay8Peer, const GUID *const pvoid, DPN_SP_CAPS *const pdpspCaps,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetSPCaps(NULL, pvoid,pdpspCaps,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_GetConnectionInfo(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNID dpnid, DPN_CONNECTION_INFO *const pdpConnectionInfo,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_GetConnectionInfo(NULL, dpnid,pdpConnectionInfo,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_RegisterLobby(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DPNHANDLE dpnHandle,PVOID const pIDP8LobbiedApplication,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_RegisterLobby(NULL, dpnHandle,pIDP8LobbiedApplication,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_TerminateSession(LPDIRECTPLAY8PEER pDirectPlay8Peer, void *const pvTerminateData,const DWORD dwTerminateDataSize,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_TerminateSession(NULL, pvTerminateData,dwTerminateDataSize,dwFlags);
}

HRESULT WINAPI IDirectPlay8Peer_DumpNameTable(LPDIRECTPLAY8PEER pDirectPlay8Peer, char *const Buffer)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DN_DumpNameTable(NULL, Buffer);
}

HRESULT WINAPI IDirectPlay8Peer_DoWork(LPDIRECTPLAY8PEER pDirectPlay8Peer, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Peer(pDirectPlay8Peer)->DoWork(dwFlags);
}



//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************

//	DN_SetPeerInfo
//
//	Set the info for the local player (peer) and propagate to other players

#undef DPF_MODNAME
#define DPF_MODNAME "DN_SetPeerInfo"

HRESULT DIRECTNETOBJECT::DN_SetPeerInfo( IDirectPlay8Peer *pInterface,
							  const DPN_PLAYER_INFO *const pdpnPlayerInfo,
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
	BOOL				fConnected;

	DPF(2,"Parameters: pInterface [0x%p], pdpnPlayerInfo [0x%p], pvAsyncContext [0x%p], phAsyncHandle [%p], dwFlags [0x%lx]",
			pInterface,pdpnPlayerInfo,pvAsyncContext,phAsyncHandle,dwFlags);
    	
	RIP_ON_NOT_TRUE( "SetPeerInfo() Error validating params", SUCCEEDED( DN_ValidateSetPeerInfo( pInterface , pdpnPlayerInfo, pvAsyncContext, phAsyncHandle, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "SetPeerInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

	pLocalPlayer = NULL;

	if ((pdpnPlayerInfo->dwInfoFlags & DPNINFO_NAME) && (pdpnPlayerInfo->pwszName))
	{
		pwszName = pdpnPlayerInfo->pwszName;
		dwNameSize = (wcslen(pwszName) + 1) * sizeof(WCHAR);
	}
	else
	{
		pwszName = NULL;
		dwNameSize = 0;
	}
	if ((pdpnPlayerInfo->dwInfoFlags & DPNINFO_DATA) && (pdpnPlayerInfo->pvData) && (pdpnPlayerInfo->dwDataSize))
	{
		pvData = pdpnPlayerInfo->pvData;
		dwDataSize = pdpnPlayerInfo->dwDataSize;
	}
	else
	{
		pvData = NULL;
		dwDataSize = 0;
	}

	//
	//	If we are connected, we will update our entry if we are the Host, or request the Host to update us.
	//	Otherwise, we will just update the DefaultPlayer.
	//
	DNEnterCriticalSection(&pdnObject->csDirectNetObject);
	if (pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED)
	{
		fConnected = TRUE;
	}
	else
	{
		fConnected = FALSE;
	}
	DNLeaveCriticalSection(&pdnObject->csDirectNetObject);
	if (fConnected)
	{
		if ((hResultCode = pdnObject->NameTable.GetLocalPlayerRef( &pLocalPlayer )) != DPN_OK)
		{
			DPFERR( "Could not get local player reference" );
			DisplayDNError(0,hResultCode);
			goto Failure;
		}

		if (pLocalPlayer->IsHost())
		{
			DPF(3,"Host is updating peer info");

			hResultCode = DNHostUpdateInfo(	pdnObject,
											pLocalPlayer->GetDPNID(),
											pwszName,
											dwNameSize,
											pvData,
											dwDataSize,
											pdpnPlayerInfo->dwInfoFlags,
											pvAsyncContext,
											pLocalPlayer->GetDPNID(),
											0,
											&hAsyncOp,
											dwFlags );
			if ((hResultCode != DPN_OK) && (hResultCode != DPNERR_PENDING))
			{
				DPFERR("Could not request host to update group");
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
												pLocalPlayer->GetDPNID(),
												pwszName,
												dwNameSize,
												pvData,
												dwDataSize,
												pdpnPlayerInfo->dwInfoFlags,
												pvAsyncContext,
												&hAsyncOp,
												dwFlags);
			if (hResultCode != DPN_OK && hResultCode != DPNERR_PENDING)
			{
				DPFERR("Could not request host to update group info");
			}
			else
			{
					DPF(3,"Async Handle [0x%lx]",hAsyncOp);
					*phAsyncHandle = hAsyncOp;
			}
		}

		pLocalPlayer->Release();
		pLocalPlayer = NULL;
	}
	else
	{
		DNASSERT(pdnObject->NameTable.GetDefaultPlayer() != NULL);

		// This function takes the lock internally
		pdnObject->NameTable.GetDefaultPlayer()->UpdateEntryInfo(pwszName,dwNameSize,pvData,dwDataSize,pdpnPlayerInfo->dwInfoFlags, FALSE);

		hResultCode = DPN_OK;
	}

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pLocalPlayer)
	{
		pLocalPlayer->Release();
		pLocalPlayer = NULL;
	}
	goto Exit;
}


//	DN_GetPeerInfo
//
//	Retrieve peer info from the local nametable.

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetPeerInfo"

HRESULT DIRECTNETOBJECT::DN_GetPeerInfo(IDirectPlay8Peer *pInterface,
							const DPNID dpnid,
							DPN_PLAYER_INFO *const pdpnPlayerInfo,
							DWORD *const pdwSize,
							const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	CNameTableEntry		*pNTEntry;
	HRESULT				hResultCode;
	CPackedBuffer		packedBuffer;

	DPF(2,"Parameters: dpnid [0x%lx], pdpnPlayerInfo [0x%p], dwFlags [0x%lx]",
			dpnid,pdpnPlayerInfo,dwFlags);

	RIP_ON_NOT_TRUE( "GetPeerInfo() Error validating params", SUCCEEDED( DN_ValidateGetPeerInfo( pInterface , dpnid, pdpnPlayerInfo, pdwSize, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "GetPeerInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) &&
    	   !(pdnObject->dwFlags & DN_OBJECT_FLAG_DISCONNECTING) )
    	{
    	    DPFERR("You must be connected / disconnecting to use this function" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	    	

	pNTEntry = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnid,&pNTEntry)) != DPN_OK)
	{
		DPFERR("Could not retrieve name table entry");
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
	packedBuffer.Initialize(pdpnPlayerInfo,*pdwSize);

	pNTEntry->Lock();
	if (pNTEntry->IsGroup())
	{
	    DPFERR( "Specified ID is invalid" );
		pNTEntry->Unlock();
		hResultCode = DPNERR_INVALIDPLAYER;
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
#define DPF_MODNAME "DN_GetPeerAddress"

HRESULT DIRECTNETOBJECT::DN_GetPeerAddress(IDirectPlay8Peer *pInterface,
							   const DPNID dpnid,
							   IDirectPlay8Address **const ppAddress,
							   const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	CNameTableEntry		*pNTEntry;
	IDirectPlay8Address	*pAddress;
	HRESULT				hResultCode;

	DPF(2,"Parameters : pInterface [0x%p], dpnid [0x%lx], ppAddress [0x%p], dwFlags [0x%lx]",
		pInterface,dpnid,ppAddress,dwFlags);

	RIP_ON_NOT_TRUE( "GetPeerAddress() Error validating params", SUCCEEDED( DN_ValidateGetPeerAddress( pInterface , dpnid, ppAddress, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "GetPeerAddress() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    	if( pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTING )
    	{
    	    DPFERR("Object is connecting / starting to host" );
    	    DPF_RETURN(DPNERR_CONNECTING);
    	}

    	if( !(pdnObject->dwFlags & DN_OBJECT_FLAG_CONNECTED) &&
    	   !(pdnObject->dwFlags & DN_OBJECT_FLAG_DISCONNECTING) )
    	{
    	    DPFERR("You must be connected / disconnecting to use this function" );
    	    DPF_RETURN(DPNERR_NOCONNECTION);
    	}	    	

	pNTEntry = NULL;
	pAddress = NULL;

	if ((hResultCode = pdnObject->NameTable.FindEntry(dpnid,&pNTEntry)) != DPN_OK)
	{
		DPFERR("Could not find NameTableEntry");
		DisplayDNError(0,hResultCode);
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
	}

	if ((pNTEntry->GetAddress() == NULL) || (pNTEntry->IsGroup()) || !pNTEntry->IsAvailable())
	{
	    DPFERR( "Invalid ID specified.  Not a player" );
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
	}

	hResultCode = pNTEntry->GetAddress()->Duplicate(ppAddress);

	pNTEntry->Release();
	pNTEntry = NULL;

Exit:
	DPF_RETURN(hResultCode);

Failure:
	if (pNTEntry)
	{
		pNTEntry->Release();
		pNTEntry = NULL;
	}

	goto Exit;
}
