/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Client.cpp
 *  Content:    DNET client interface routines
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/21/99	mjn		Created
 *	01/06/99	mjn		Moved NameTable stuff to NameTable.h
 *	01/28/00	mjn		Implemented ReturnBuffer in API
 *	02/01/00	mjn		Implemented GetCaps and SetCaps in API
 *	02/15/00	mjn		Implement INFO flags in SetClientInfo
 *	02/18/00	mjn		Converted DNADDRESS to IDirectPlayAddress8
 *  03/17/00    rmt     Added new caps functions
 *	04/06/00	mjn		Added GetServerAddress to API
 *	04/16/00	mjn		DNSendMessage uses CAsyncOp
 *  04/17/00    rmt     Added more param validation
 *              rmt     Removed required for connection from Get/SetInfo / GetAddress
 *	04/19/00	mjn		Send API call accepts a range of DPN_BUFFER_DESCs and a count
 *	04/20/00	mjn		DN_Send() calls DN_SendTo() with DPNID=0
 *	04/24/00	mjn		Updated Group and Info operations to use CAsyncOp's
 *	04/28/00	mjn		Updated DN_GetHostSendQueueInfo() to use CAsyncOp's
 *	05/03/00	mjn		Use GetHostPlayerRef() rather than GetHostPlayer()
 *	05/31/00	mjn		Added operation specific SYNC flags
 *	06/23/00	mjn		Removed dwPriority from Send() API call
 *	06/27/00	mjn		Allow priorities to be specified to GetSendQueueInfo() API call
 *	06/27/00	mjn		Added DN_ClientConnect() (without pvPlayerContext)
 *				mjn		Allow mix-n-match of priorities in GetSendQueueInfo() API call
 *	07/09/00	mjn		Cleaned up DN_SetClientInfo()
 *  07/09/00	rmt		Bug #38323 - RegisterLobby needs a DPNHANDLE parameter.
 *  07/21/2000  RichGr  IA64: Use %p format specifier for 32/64-bit pointers.
 *	10/11/00	mjn		Take locks for CNameTableEntry::PackInfo()
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

ULONG WINAPI IDirectPlay8Client_AddRef(LPDIRECTPLAY8CLIENT pDirectPlay8Client)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->AddRef();
}
ULONG WINAPI IDirectPlay8Client_Release(LPDIRECTPLAY8CLIENT pDirectPlay8Client)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->Release();
}

HRESULT WINAPI IDirectPlay8Client_Initialize(LPDIRECTPLAY8CLIENT pDirectPlay8Client, LPVOID const lpvUserContext, const PFNDPNMESSAGEHANDLER lpfn, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_Initialize(NULL, lpvUserContext, lpfn, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_EnumServiceProviders(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const GUID *const pvoid,const GUID *const pguidApplication,DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer,DWORD *const pcbEnumData,DWORD *const pcReturned,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_EnumServiceProviders(NULL, pvoid,pguidApplication,pSPInfoBuffer,pcbEnumData,pcReturned,dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_EnumHosts(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DPN_APPLICATION_DESC *const pApplicationDesc,IDirectPlay8Address *const dnaddrHost,IDirectPlay8Address *const dnaddrDeviceInfo,PVOID const pUserEnumData,const DWORD dwUserEnumDataSize,const DWORD dwRetryCount,const DWORD dwRetryInterval,const DWORD dwTimeOut,PVOID const pvUserContext,DPNHANDLE *const pAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_EnumHosts(NULL, pApplicationDesc,dnaddrHost,dnaddrDeviceInfo,pUserEnumData,dwUserEnumDataSize,dwRetryCount,dwRetryInterval,dwTimeOut,pvUserContext,pAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_CancelAsyncOperation(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPNHANDLE hAsyncHandle, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_CancelAsyncOperation(NULL, hAsyncHandle, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_Connect(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPN_APPLICATION_DESC *const pdnAppDesc,IDirectPlay8Address *const pHostAddr,IDirectPlay8Address *const pDeviceInfo,const DPN_SECURITY_DESC *const pdnSecurity,const DPN_SECURITY_CREDENTIALS *const pdnCredentials,const void *const pvUserConnectData,const DWORD dwUserConnectDataSize,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_ClientConnect(NULL, pdnAppDesc,pHostAddr,pDeviceInfo,pdnSecurity,pdnCredentials,pvUserConnectData,dwUserConnectDataSize,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_Send(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPN_BUFFER_DESC *const prgBufferDesc,const DWORD cBufferDesc,const DWORD dwTimeOut,void *const pvAsyncContext,DPNHANDLE *const phAsyncHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_Send(NULL, prgBufferDesc,cBufferDesc,dwTimeOut,pvAsyncContext,phAsyncHandle,dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_GetSendQueueInfo(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DWORD *const pdwNumMsgs, DWORD *const pdwNumBytes, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_GetHostSendQueueInfo(NULL, pdwNumMsgs,pdwNumBytes,dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_GetApplicationDesc(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DPN_APPLICATION_DESC *const pAppDescBuffer,DWORD *const lpcbDataSize,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_GetApplicationDesc( NULL, pAppDescBuffer, lpcbDataSize, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_SetClientInfo(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPN_PLAYER_INFO *const pdpnPlayerInfo,PVOID const pvAsyncContext,DPNHANDLE *const phAsyncHandle, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_SetClientInfo( NULL, pdpnPlayerInfo, pvAsyncContext, phAsyncHandle, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_GetServerInfo(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DPN_PLAYER_INFO *const pdpnPlayerInfo,DWORD *const pdwSize,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_GetServerInfo( NULL, pdpnPlayerInfo, pdwSize, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_GetServerAddress(LPDIRECTPLAY8CLIENT pDirectPlay8Client, IDirectPlay8Address **const pAddress,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_GetServerAddress( NULL, pAddress, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_Close(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_Close( NULL, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_ReturnBuffer(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPNHANDLE hBufferHandle,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_ReturnBuffer( NULL, hBufferHandle, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_GetCaps(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DPN_CAPS *const pdnCaps,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_GetCaps( NULL, pdnCaps, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_SetCaps(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPN_CAPS *const pdnCaps,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_SetCaps( NULL, pdnCaps, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_SetSPCaps(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const GUID *const pvoid, const DPN_SP_CAPS *const pdpspCaps, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_SetSPCaps( NULL, pvoid, pdpspCaps, dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_GetSPCaps(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const GUID *const pvoid, DPN_SP_CAPS *const pdpspCaps,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_GetSPCaps(NULL, pvoid,pdpspCaps,dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_GetConnectionInfo(LPDIRECTPLAY8CLIENT pDirectPlay8Client, DPN_CONNECTION_INFO *const pdpConnectionInfo,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_GetServerConnectionInfo(NULL, pdpConnectionInfo,dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_RegisterLobby(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DPNHANDLE dpnHandle,PVOID const pIDP8LobbiedApplication,const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DN_RegisterLobby(NULL, dpnHandle,pIDP8LobbiedApplication,dwFlags);
}

HRESULT WINAPI IDirectPlay8Client_DoWork(LPDIRECTPLAY8CLIENT pDirectPlay8Client, const DWORD dwFlags)
{
    return DIRECTNETOBJECT::GetDirectPlay8Client(pDirectPlay8Client)->DoWork(dwFlags);
}


//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************

#undef DPF_MODNAME
#define DPF_MODNAME "DN_Send"

HRESULT DIRECTNETOBJECT::DN_Send( IDirectPlay8Client *pInterface,
					  const DPN_BUFFER_DESC *const prgBufferDesc,
					  const DWORD cBufferDesc,
					  const DWORD dwTimeOut,
					  const PVOID pvAsyncContext,
					  DPNHANDLE *const phAsyncHandle,
					  const DWORD dwFlags)
{
	return(	DN_SendTo(	pInterface,
						0,					// DN_SendTo should translate this call to the Host player
						prgBufferDesc,
						cBufferDesc,
						dwTimeOut,
						pvAsyncContext,
						phAsyncHandle,
						dwFlags ) );
}


//	DN_ClientConnect
//
//	Call DN_Connect, but with no PlayerContext

HRESULT DIRECTNETOBJECT::DN_ClientConnect(IDirectPlay8Client *pInterface,
							  const DPN_APPLICATION_DESC *const pdnAppDesc,
							  IDirectPlay8Address *const pHostAddr,
							  IDirectPlay8Address *const pDeviceInfo,
							  const DPN_SECURITY_DESC *const pdnSecurity,
							  const DPN_SECURITY_CREDENTIALS *const pdnCredentials,
							  const void *const pvUserConnectData,
							  const DWORD dwUserConnectDataSize,
							  void *const pvAsyncContext,
							  DPNHANDLE *const phAsyncHandle,
							  const DWORD dwFlags)
{
	return(	DN_Connect(	pInterface,
						pdnAppDesc,
						pHostAddr,
						pDeviceInfo,
						pdnSecurity,
						pdnCredentials,
						pvUserConnectData,
						dwUserConnectDataSize,
						NULL,
						pvAsyncContext,
						phAsyncHandle,
						dwFlags ) );
}


//	DN_SetClientInfo
//
//	Set the info for the client player and propagate to server

#undef DPF_MODNAME
#define DPF_MODNAME "DN_SetClientInfo"

HRESULT DIRECTNETOBJECT::DN_SetClientInfo(IDirectPlay8Client *pInterface,
							  const DPN_PLAYER_INFO *const pdpnPlayerInfo,
							  const PVOID pvAsyncContext,
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

	RIP_ON_NOT_TRUE( "SetClientInfo() Error validating params", SUCCEEDED( DN_ValidateSetClientInfo( pInterface , pdpnPlayerInfo, pvAsyncContext, phAsyncHandle, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "SetClientInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

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
	//	If we are connected, we will request the Host to update us.
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

		DPF(3,"Request host to update client info");

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
			DPFERR("Could not request host to update client info");
		}
		else
		{
				DPF(3,"Async Handle [0x%lx]",hAsyncOp);
				*phAsyncHandle = hAsyncOp;
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


//	DN_GetServerInfo
//
//	Retrieve server info from the local nametable.

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetServerInfo"

HRESULT DIRECTNETOBJECT::DN_GetServerInfo(IDirectPlay8Client *pInterface,
							  DPN_PLAYER_INFO *const pdpnPlayerInfo,
							  DWORD *const pdwSize,
							  const DWORD dwFlags)
{
	DIRECTNETOBJECT	*pdnObject = this;
	HRESULT			hResultCode;
	CPackedBuffer	packedBuffer;
	CNameTableEntry	*pHostPlayer;

	DPF(3,"Parameters: pInterface [0x%p], pdpnPlayerInfo [0x%p], pdwSize [%p], dwFlags [0x%lx]",
			pInterface,pdpnPlayerInfo,pdwSize,dwFlags);

	RIP_ON_NOT_TRUE( "GetServerInfo() Error validating params", SUCCEEDED( DN_ValidateGetServerInfo( pInterface , pdpnPlayerInfo, pdwSize, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "GetServerInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

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

	pHostPlayer = NULL;

	if ((hResultCode = pdnObject->NameTable.GetHostPlayerRef(&pHostPlayer)) != DPN_OK)
	{
		DPFERR("Could not find Host player");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	packedBuffer.Initialize(pdpnPlayerInfo,*pdwSize);

	pHostPlayer->Lock();
	hResultCode = pHostPlayer->PackInfo(&packedBuffer);
	pHostPlayer->Unlock();

	pHostPlayer->Release();
	pHostPlayer = NULL;

	if ((hResultCode == DPN_OK) || (hResultCode == DPNERR_BUFFERTOOSMALL))
	{
		*pdwSize = packedBuffer.GetSizeRequired();
	}

Exit:
	DPF_RETURN(hResultCode);

Failure:
	if (pHostPlayer)
	{
		pHostPlayer->Release();
		pHostPlayer = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetHostSendQueueInfo"

HRESULT DIRECTNETOBJECT::DN_GetHostSendQueueInfo(IDirectPlay8Client *pInterface,
									 DWORD *const pdwNumMsgs,
									 DWORD *const pdwNumBytes,
									 const DWORD dwFlags )
{
	DIRECTNETOBJECT		*pdnObject = this;
	DWORD				dwQueueFlags;
	DWORD				dwNumMsgs;
	DWORD				dwNumBytes;
	CNameTableEntry     *pNTEntry;
	CConnection			*pConnection;
	HRESULT				hResultCode;

	DPF(3,"Parameters : pInterface [0x%p], pdwNumMsgs [0x%p], pdwNumBytes [0x%p], dwFlags [0x%lx]",
		pInterface,pdwNumMsgs,pdwNumBytes,dwFlags);

	RIP_ON_NOT_TRUE( "GetHostSendQueueInfo() Error validating params", SUCCEEDED( DN_ValidateGetHostSendQueueInfo( pInterface , pdwNumMsgs, pdwNumBytes, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "GetHostSendQueueInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

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
   	
	pNTEntry = NULL;
	pConnection = NULL;

	//
    //	Validate specified player ID and get CConnection
	//
	if((hResultCode = pdnObject->NameTable.GetHostPlayerRef( &pNTEntry )))
	{
		DPF(0,"Could not find Host Player in NameTable");
		hResultCode = DPNERR_CONNECTIONLOST;
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
#define DPF_MODNAME "DN_GetServerAddress"

HRESULT DIRECTNETOBJECT::DN_GetServerAddress(IDirectPlay8Client *pInterface,
								 IDirectPlay8Address **const ppAddress,
								 const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	IDirectPlay8Address	*pAddress;
	HRESULT				hResultCode;
	CNameTableEntry		*pHostPlayer;

	DPF(3,"Parameters : pInterface [0x%p], ppAddress [0x%p], dwFlags [0x%lx]",
		pInterface,ppAddress,dwFlags);

	RIP_ON_NOT_TRUE( "GetServerAddress() Error validating params", SUCCEEDED( DN_ValidateGetServerAddress( pInterface,ppAddress,dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "GetServerAddress() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

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
  	
	pHostPlayer = NULL;

	if ((hResultCode = pdnObject->NameTable.GetHostPlayerRef( &pHostPlayer )) != DPN_OK)
	{
		DPFERR("Could not find Host player");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pAddress = pHostPlayer->GetAddress();
	DNASSERT(pAddress != NULL);
	hResultCode = pAddress->Duplicate(ppAddress);

	pHostPlayer->Release();
	pHostPlayer = NULL;

Exit:
	DPF_RETURN(hResultCode);

Failure:
	if (pHostPlayer)
	{
		pHostPlayer->Release();
		pHostPlayer = NULL;
	}
	goto Exit;
}
