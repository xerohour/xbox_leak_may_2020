/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Server.cpp
 *  Content:    DNET server interface routines
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/21/99	mjn		Created
 *  12/23/99	mjn		Hand all NameTable update sends from Host to worker thread
 *	12/28/99	mjn		Moved Async Op stuff to Async.h
 *	01/06/00	mjn		Moved NameTable stuff to NameTable.h
 *	01/14/00	mjn		Added pvUserContext to Host API call
 *	01/16/00	mjn		Moved User callback stuff to User.h
 *	01/22/00	mjn		Implemented DestroyClient in API
 *	01/28/00	mjn		Implemented ReturnBuffer in API
 *	02/01/00	mjn		Implemented GetCaps and SetCaps in API
 *	02/01/00	mjn		Implement Player/Group context values
 *	02/15/00	mjn		Use INFO flags in SetServerInfo and return context in GetClientInfo
 *	02/17/00	mjn		Implemented GetPlayerContext and GetGroupContext
 *  03/17/00    rmt     Added new caps functions
 *	04/04/00	mjn		Added TerminateSession to API
 *	04/05/00	mjn		Modified DestroyClient
 *	04/06/00	mjn		Added GetClientAddress to API
 *				mjn		Added GetHostAddress to API
 *  04/18/00    rmt     Added additional paramtere validation
 *	04/19/00	mjn		SendTo API call accepts a range of DPN_BUFFER_DESCs and a count
 *	04/24/00	mjn		Updated Group and Info operations to use CAsyncOp's
 *	05/31/00	mjn		Added operation specific SYNC flags
 *	06/23/00	mjn		Removed dwPriority from SendTo() API call
 *	07/09/00	mjn		Cleaned up DN_SetServerInfo()
 *  07/09/00	rmt		Bug #38323 - RegisterLobby needs a DPNHANDLE parameter.
 *  08/05/00    RichGr  IA64: Use %p format specifier in DPFs for 32/64-bit pointers and handles.
 *	10/11/00	mjn		Take locks for CNameTableEntry::PackInfo()
 *				mjn		Check deleted list in DN_GetClientInfo()
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

ULONG WINAPI IDirectPlay8Server_AddRef(LPDIRECTPLAY8SERVER pDirectPlay8Server)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->AddRef();
}
ULONG WINAPI IDirectPlay8Server_Release(LPDIRECTPLAY8SERVER pDirectPlay8Server)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->Release();
}

HRESULT WINAPI IDirectPlay8Server_Initialize(LPDIRECTPLAY8SERVER pDirectPlay8Server, LPVOID const lpvUserContext, const PFNDPNMESSAGEHANDLER lpfn, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_Initialize(NULL, lpvUserContext,lpfn,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_EnumServiceProviders(LPDIRECTPLAY8SERVER pDirectPlay8Server, const GUID *const pvoid,const GUID *const pguidApplication,DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer,DWORD *const pcbEnumData,DWORD *const pcReturned,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_EnumServiceProviders(NULL, pvoid,pguidApplication,pSPInfoBuffer,pcbEnumData,pcReturned,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_CancelAsyncOperation(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNHANDLE hAsyncHandle, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_CancelAsyncOperation(NULL, hAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetSendQueueInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,DWORD *const lpdwNumMsgs, DWORD *const lpdwNumBytes, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetSendQueueInfo(NULL, dpnid,lpdwNumMsgs,lpdwNumBytes,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetApplicationDesc(LPDIRECTPLAY8SERVER pDirectPlay8Server, DPN_APPLICATION_DESC *const pAppDescBuffer,DWORD *const lpcbDataSize,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetApplicationDesc(NULL, pAppDescBuffer,lpcbDataSize,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_SetServerInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPN_PLAYER_INFO *const pdpnPlayerInfo,PVOID const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags )
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_SetServerInfo(NULL, pdpnPlayerInfo,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetClientInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,DPN_PLAYER_INFO *const pdpnPlayerInfo,DWORD *const pdwSize,const DWORD dwFlags )
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetClientInfo(NULL, dpnid,pdpnPlayerInfo,pdwSize,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetClientAddress(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,IDirectPlay8Address **const pAddress,const DWORD dwFlags )
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetClientAddress(NULL, dpnid,pAddress,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetLocalHostAddresses(LPDIRECTPLAY8SERVER pDirectPlay8Server, IDirectPlay8Address **const prgpAddress, DWORD *const pcAddress,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetHostAddress(NULL, prgpAddress,pcAddress,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_SetApplicationDesc(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPN_APPLICATION_DESC *const lpad, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_SetApplicationDesc(NULL, lpad,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_Host(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address **const prgpDeviceInfo,const DWORD cDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,void *const pvPlayerContext,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_Host(NULL, pdnAppDesc,prgpDeviceInfo,cDeviceInfo,pdnSecurity,pdnCredentials,pvPlayerContext,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_SendTo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dnid, const DPN_BUFFER_DESC *const prgBufferDesc,const DWORD cBufferDesc,const DWORD dwTimeOut, void *const pvAsyncContext, DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_SendTo(NULL, dnid,prgBufferDesc,cBufferDesc,dwTimeOut,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_CreateGroup(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPN_GROUP_INFO *const pdpnGroupInfo,void *const pvGroupContext,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_CreateGroup(NULL, pdpnGroupInfo,pvGroupContext,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_DestroyGroup(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID idGroup,PVOID const pvUserContext,DPNHANDLE *const lpAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_DestroyGroup(NULL, idGroup,pvUserContext,lpAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_AddPlayerToGroup(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID idGroup, const DPNID idClient,PVOID const pvUserContext,DPNHANDLE *const lpAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_AddClientToGroup(NULL, idGroup,idClient,pvUserContext,lpAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_RemovePlayerFromGroup(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID idGroup, const DPNID idClient,PVOID const pvUserContext,DPNHANDLE *const lpAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_RemoveClientFromGroup(NULL, idGroup,idClient,pvUserContext,lpAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_SetGroupInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_SetGroupInfo(NULL, dpnid,pdpnGroupInfo,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetGroupInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,DPN_GROUP_INFO *const pdpnGroupInfo,DWORD *const pdwSize,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetGroupInfo(NULL, dpnid,pdpnGroupInfo,pdwSize,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_EnumPlayersAndGroups(LPDIRECTPLAY8SERVER pDirectPlay8Server, DPNID *const lprgdnid, DWORD *const lpcdnid, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_EnumClientsAndGroups(NULL, lprgdnid,lpcdnid,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_EnumGroupMembers(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dnid, DPNID *const lprgdnid, DWORD *const lpcdnid, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_EnumGroupMembers(NULL, dnid,lprgdnid,lpcdnid,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_Close(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_Close(NULL, dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_DestroyClient(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dnid,const void *const pvDestroyData,const DWORD dwDestroyDataSize,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_DestroyPlayer(NULL, dnid,pvDestroyData,dwDestroyDataSize,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_ReturnBuffer(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNHANDLE hBufferHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_ReturnBuffer(NULL, hBufferHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetPlayerContext(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,PVOID *const ppvPlayerContext,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetPlayerContext(NULL, dpnid,ppvPlayerContext,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetGroupContext(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid,PVOID *const ppvGroupContext,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetGroupContext(NULL, dpnid,ppvGroupContext,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetCaps(LPDIRECTPLAY8SERVER pDirectPlay8Server, DPN_CAPS *const pdnCaps,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetCaps(NULL, pdnCaps,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_SetCaps(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPN_CAPS *const pdnCaps,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_SetCaps(NULL, pdnCaps,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_SetSPCaps(LPDIRECTPLAY8SERVER pDirectPlay8Server, const GUID *const pvoid, const DPN_SP_CAPS *const pdpspCaps, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_SetSPCaps(NULL, pvoid,pdpspCaps,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetSPCaps(LPDIRECTPLAY8SERVER pDirectPlay8Server, const GUID *const pvoid, DPN_SP_CAPS *const pdpspCaps,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetSPCaps(NULL, pvoid,pdpspCaps,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_GetConnectionInfo(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNID dpnid, DPN_CONNECTION_INFO *const pdpConnectionInfo,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_GetConnectionInfo(NULL, dpnid,pdpConnectionInfo,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_RegisterLobby(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DPNHANDLE dpnHandle,PVOID const pIDP8LobbiedApplication,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_RegisterLobby(NULL, dpnHandle,pIDP8LobbiedApplication,dwFlags);
}

HRESULT WINAPI IDirectPlay8Server_DumpNameTable(LPDIRECTPLAY8SERVER pDirectPlay8Server, char *const Buffer)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DN_DumpNameTable(NULL, Buffer);
}

HRESULT WINAPI IDirectPlay8Server_DoWork(LPDIRECTPLAY8SERVER pDirectPlay8Server, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Server(pDirectPlay8Server)->DoWork(dwFlags);
}



//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************

//	DN_SetServerInfo
//
//	Set the info for the server and propagate to client players

#undef DPF_MODNAME
#define DPF_MODNAME "DN_SetServerInfo"

HRESULT DIRECTNETOBJECT::DN_SetServerInfo(IDirectPlay8Server *pInterface,
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

	DPF(2,"Parameters: pInterface [0x%p], pdpnPlayerInfo [0x%p], pvAsyncContext [0x%p], phAsyncHandle [0x%p], dwFlags [0x%lx]",
			pInterface,pdpnPlayerInfo,pvAsyncContext,phAsyncHandle,dwFlags);

	RIP_ON_NOT_TRUE( "SetServerInfo() Error validating params", SUCCEEDED( DN_ValidateSetServerInfo( pInterface , pdpnPlayerInfo, pvAsyncContext, phAsyncHandle, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "SetServerInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

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
	//	If we are connected, we will update our entry.
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

		DPF(3,"Host is updating server info");

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


//	DN_GetClientInfo
//
//	Retrieve client info from the local nametable.

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetClientInfo"

HRESULT DIRECTNETOBJECT::DN_GetClientInfo(IDirectPlay8Server *pInterface,
							  const DPNID dpnid,
							  DPN_PLAYER_INFO *const pdpnPlayerInfo,
							  DWORD *const pdwSize,
							  const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	CNameTableEntry		*pNTEntry;
	CPackedBuffer		packedBuffer;
	HRESULT				hResultCode;

	DPF(2,"Parameters: dpnid [0x%lx], pdpnPlayerInfo [0x%p], dwFlags [0x%lx]",
			dpnid,pdpnPlayerInfo,dwFlags);

	RIP_ON_NOT_TRUE( "GetClientInfo() Error validating params", SUCCEEDED( DN_ValidateGetClientInfo( pInterface , dpnid, pdpnPlayerInfo, pdwSize, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "GetClientInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

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
	if (pNTEntry->IsGroup() || pNTEntry->IsHost())
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
#define DPF_MODNAME "DN_GetClientAddress"

HRESULT DIRECTNETOBJECT::DN_GetClientAddress(IDirectPlay8Server *pInterface,
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

	RIP_ON_NOT_TRUE( "GetClientAddress() Error validating params", SUCCEEDED( DN_ValidateGetClientAddress( pInterface , dpnid, ppAddress, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "GetClientAddress() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

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
	    DPFERR( "Specified player is not valid" );
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
	}

	hResultCode = pNTEntry->GetAddress()->Duplicate(ppAddress);

	pNTEntry->Release();
	pNTEntry = NULL;

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
