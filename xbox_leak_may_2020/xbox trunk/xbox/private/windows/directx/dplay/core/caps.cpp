/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Caps.cpp
 *  Content:    Dplay8 caps routines
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  03/17/00	rmt		Created
 *  03/23/00	rmt		Removed unused local variables
 *  03/25/00    rmt		Updated to make calls into SP's function
 *              rmt		Updated SP calls to Initialize SP (and create if required)
 *  03/31/00    rmt		Hooked up the GetCaps/SetCaps calls to call the protocol
 *  04/17/00    rmt		Strong param validation
 *	04/19/00	mjn		Removed AddRef() for NameTableEntry in DN_GetConnectionInfoHelper()
 *	05/03/00	mjn		Use GetHostPlayerRef() rather than GetHostPlayer()
 *	06/05/00	mjn		Fixed DN_GetConnectionInfoHelper() to use GetConnectionRef
 *  06/09/00    rmt     Updates to split CLSID and allow whistler compat
 *	07/06/00	mjn		Use GetInterfaceRef for SP interface
 *				mjn		Fixed up DN_SetActualSPCaps() and DN_GetActualSPCaps()
 *	07/29/00	mjn		Fixed SetSPCaps() recursion problem
 *	07/31/00	mjn		Renamed dwDefaultEnumRetryCount to dwDefaultEnumCount in DPN_SP_CAPS
 *  08/03/2000	rmt		Bug #41244 - Wrong return codes -- part 2
 *  08/05/00    RichGr  IA64: Use %p format specifier in DPFs for 32/64-bit pointers and handles.
 *	08/05/00	mjn		Added dwFlags to DN_GetConnectionInfoHelper()
 *	08/20/00	mjn		DNSetActualSPCaps() uses CServiceProvider object instead of GUID
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dncorei.h"


//	DN_SetCaps
//
//	Set caps
#undef DPF_MODNAME
#define DPF_MODNAME "DN_SetCaps"

HRESULT DIRECTNETOBJECT::DN_SetCaps(PVOID pv,
						const DPN_CAPS *const pdnCaps,
						const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
    HRESULT             hResultCode;

	DPF(3,"Parameters: pdnCaps [0x%p] dwFlags [0x%lx]", pdnCaps, dwFlags );

	RIP_ON_NOT_TRUE( "SetCaps() Error validating params", SUCCEEDED( DN_ValidateSetCaps( pv, pdnCaps, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "SetCaps() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    hResultCode = DNPSetProtocolCaps( pdnObject->pdnProtocolData, pdnCaps );

	DPF_RETURN(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetCaps"
HRESULT DIRECTNETOBJECT::DN_GetCaps(PVOID pv,
						DPN_CAPS *const pdnCaps,
						const DWORD dwFlags)
{
	DPF(2,"Parameters: pdnCaps [0x%p], dwFlags [0x%lx]", pdnCaps,dwFlags);

	DIRECTNETOBJECT		*pdnObject = this;
	HRESULT hResultCode;

	RIP_ON_NOT_TRUE( "GetCaps() Error validating params", SUCCEEDED( DN_ValidateGetCaps( pv, pdnCaps, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "GetCaps() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    hResultCode = DNPGetProtocolCaps( pdnObject->pdnProtocolData, pdnCaps );

	DPF_RETURN(hResultCode);
}

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetSPCaps"
HRESULT DIRECTNETOBJECT::DN_GetSPCaps(PVOID pv,
						  const GUID * const pguidSP,
						  DPN_SP_CAPS *const pdnSPCaps,
						  const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
	HRESULT             hResultCode;

	DPF(2,"Parameters: pdnSPCaps [0x%p], dwFlags [0x%lx]", pdnSPCaps, dwFlags );

	RIP_ON_NOT_TRUE( "GetSPCaps() Error validating params", SUCCEEDED( DN_ValidateGetSPCaps( pv, pguidSP, pdnSPCaps, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "GetSPCaps() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

    if ((hResultCode = DNGetActualSPCaps( pdnObject, pguidSP, pdnSPCaps )) != DPN_OK)
    {
		DPFERR("Could not get actual SP caps");
		DisplayDNError(0,hResultCode);
    }

/*	REMOVE
    hResultCode = DN_SetCachedSPCaps( pdnObject, pguidSP, pdnSPCaps );

    if( FAILED( hResultCode ) )
    {
        DPF( 0, "Could not set cached SP caps hr=[0x%08x]", hResultCode );
        DNLeaveCriticalSection( &pdnObject->csDirectNetObject );
        DPF_RETURN(hResultCode);
    }
*/

	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}

#undef DPF_MODNAME
#define DPF_MODNAME "DN_SetSPCaps"
HRESULT DIRECTNETOBJECT::DN_SetSPCaps(PVOID pv,const GUID * const pguidSP, const DPN_SP_CAPS *const pdnSPCaps, const DWORD dwFlags)
{
	DPF(2,"Parameters: pdnSPCaps [0x%p]", pdnSPCaps );

	DIRECTNETOBJECT		*pdnObject = this;
	CServiceProvider	*pSP;
    HRESULT             hResultCode;

	RIP_ON_NOT_TRUE( "SetSPCaps() Error validating params", SUCCEEDED( DN_ValidateSetSPCaps( pv, pguidSP, pdnSPCaps, dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "SetSPCaps() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

	pSP = NULL;

	//
	//	If the SP is loaded, we will take a reference, otherwise, we will instantiate the SP for the
	//	life of this call, and release it when we're done.
	//
	if ((hResultCode = DN_SPFindEntry(pdnObject,pguidSP,&pSP)) == DPNERR_DOESNOTEXIST)
	{
		//
		//	Instantiate SP and add to Protocol
		//
		if ((hResultCode = DN_SPInstantiate(pdnObject,&GUID_NULL,&pSP)) != DPN_OK)
		{
			DPFERR("Could not load SP");
			DisplayDNError(0,hResultCode);
			goto Failure;
		}
	}
	else
	{
		if (hResultCode != DPN_OK)
		{
			DPFERR("Could not find SP");
			DisplayDNError(0,hResultCode);
			goto Failure;
		}
	}

	DNASSERT(pSP != NULL);

	//
	//	Set the SP's actual caps
	//
    if ((hResultCode = DNSetActualSPCaps(pdnObject,pSP,pdnSPCaps)) != DPN_OK)
    {
		DPFERR("Could not set actual SP caps");
		DisplayDNError(0,hResultCode);
		goto Failure;
    }

	//
	//	Set the SP's cached caps
	//
    if ((hResultCode = DNSetCachedSPCaps( pdnObject, NULL, pdnSPCaps )) != DPN_OK)
    {
		DPFERR("Could not set cached SP caps");
		DisplayDNError(0,hResultCode);
		goto Failure;
    }

	pSP->Release();
	pSP = NULL;

	hResultCode = DPN_OK;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
    return(hResultCode);

Failure:
	if (pSP)
	{
		pSP->Release();
		pSP = NULL;
	}
	goto Exit;
}

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetConnectionInfoHelper"
HRESULT DIRECTNETOBJECT::DN_GetConnectionInfoHelper(PVOID pv,
										const DPNID dpnid,
										DPN_CONNECTION_INFO *const pdpConnectionInfo,
										BOOL fServerPlayer,
										const DWORD dwFlags)
{
	DIRECTNETOBJECT		*pdnObject = this;
    HRESULT             hResultCode;
    CNameTableEntry     *pPlayerEntry;
    CConnection         *pConnection;
    HANDLE              hEndPoint;

	pPlayerEntry = NULL;
	pConnection = NULL;

	RIP_ON_NOT_TRUE( "GetConnectionInfo() Error validating params", SUCCEEDED( DN_ValidateGetConnectionInfoHelper( pv, dpnid, pdpConnectionInfo, fServerPlayer,dwFlags ) ) );
    	RIP_ON_NOT_TRUE( "GetConnectionInfo() Object is not initialized", pdnObject->dwFlags & DN_OBJECT_FLAG_INITIALIZED );

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


	// Temporary debugging purposes only, shouldn't be checked in!
#if	0
	DNMemoryTrackingValidateMemory();
#endif


    if( fServerPlayer )
    {
		hResultCode = pdnObject->NameTable.GetHostPlayerRef( &pPlayerEntry );
		if ( FAILED( hResultCode ) )
		{
            DPF( 0, "No host player present" );
            DPF_RETURN(DPNERR_INVALIDPLAYER);
		}
    }
    else
    {
        hResultCode = pdnObject->NameTable.FindEntry( dpnid, &pPlayerEntry );

        if( FAILED( hResultCode ) )
        {
            DPF( 0, "Could not find specified player" );
            DPF_RETURN(DPNERR_INVALIDPLAYER);
        }
    }

    if( pPlayerEntry == NULL )
    {
        DNASSERT(FALSE);
        DPF( 0, "Internal error" );
        DPF_RETURN(DPNERR_GENERIC);
    }

    if( pPlayerEntry->IsGroup() )
    {
        DPF( 0, "Cannot retrieve connection info on groups" );
		hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
    }

	if ((hResultCode = pPlayerEntry->GetConnectionRef( &pConnection )) != DPN_OK)
	{
		DPFERR("Could not get connection reference");
		hResultCode = DPNERR_CONNECTIONLOST;
		goto Failure;
	}

    hResultCode = pConnection->GetEndPt(&hEndPoint, FALSE );

    if( FAILED( hResultCode ) )
    {
        DPF( 0, "Unable to get endpoint hr=[0x%08x]", hResultCode );
		hResultCode = DPNERR_CONNECTIONLOST;
		goto Failure;
    }

    hResultCode = DNPGetEPCaps( hEndPoint, pdpConnectionInfo );

    if( FAILED( hResultCode ) )
    {
        DPF( 0, "Error getting connection info hr=[0x%08x]", hResultCode );
        hResultCode = DPNERR_INVALIDPLAYER;
		goto Failure;
    }

    pConnection->Release();
    pPlayerEntry->Release();

	hResultCode = DPN_OK;

Exit:
    DPF_RETURN(hResultCode);

Failure:
	if (pPlayerEntry)
	{
		pPlayerEntry->Release();
		pPlayerEntry = NULL;
	}
	if (pConnection)
	{
		pConnection->Release();
		pConnection = NULL;
	}
	goto Exit;
}

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetConnectionInfo"
HRESULT DIRECTNETOBJECT::DN_GetConnectionInfo(PVOID pv,
								  const DPNID dpnid,
								  DPN_CONNECTION_INFO *const pdpConnectionInfo,
								  const DWORD dwFlags)
{
	DPF(3,"Parameters: dpnid [0x%lx] pdpConnectionInfo [0x%p], dwFlags [0x%lx]", dpnid, pdpConnectionInfo,dwFlags );

    return DN_GetConnectionInfoHelper( pv, dpnid, pdpConnectionInfo, FALSE, dwFlags );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DN_GetConnectionInfo"
HRESULT DIRECTNETOBJECT::DN_GetServerConnectionInfo(PVOID pv,
										DPN_CONNECTION_INFO *const pdpConnectionInfo,
										const DWORD dwFlags)
{
	DPF(2,"Parameters: pdpConnectionInfo [0x%p], dwFlags [0x%lx]", pdpConnectionInfo, dwFlags);

    return DN_GetConnectionInfoHelper( pv, 0, pdpConnectionInfo, TRUE, dwFlags );
}

#undef DPF_MODNAME
#define DPF_MODNAME "DN_InitSPCapsList"
HRESULT DN_InitSPCapsList( DIRECTNETOBJECT *pdnObject )
{
    pdnObject->blSPCapsList.Initialize();

    return DPN_OK;
}

// This function destroys the list of SP_CAPS_ENTRY structures
#undef DPF_MODNAME
#define DPF_MODNAME "DN_FreeSPCapsList"
HRESULT DN_FreeSPCapsList( DIRECTNETOBJECT *pdnObject )
{
    CBilink *pblSearch;
    PDN_SP_CAPS_ENTRY pdpSPCaps;

    DNEnterCriticalSection( &pdnObject->csDirectNetObject );

    pblSearch = pdnObject->blSPCapsList.GetNext();

    while( pblSearch != &pdnObject->blSPCapsList )
    {
        pdpSPCaps = CONTAINING_RECORD( pblSearch, DN_SP_CAPS_ENTRY, blList );

        pblSearch->RemoveFromList();

        delete pdpSPCaps;

        pblSearch = pdnObject->blSPCapsList.GetNext();
    }

    DNLeaveCriticalSection( &pdnObject->csDirectNetObject );

    return DPN_OK;
}

// This function is a helper and assumes that you already have the global object lock
#undef DPF_MODNAME
#define DPF_MODNAME "DNGetCachedSPCaps"
HRESULT DNGetCachedSPCaps( DIRECTNETOBJECT *pdnObject, const GUID * const pguidSP, PDPN_SP_CAPS *ppCaps )
{
    CBilink *pblSearch;
    PDN_SP_CAPS_ENTRY pdpSPCaps;

    pblSearch = pdnObject->blSPCapsList.GetNext();

    if( pblSearch != &pdnObject->blSPCapsList )
    {
        pdpSPCaps = CONTAINING_RECORD( pblSearch, DN_SP_CAPS_ENTRY, blList );

            *ppCaps = &pdpSPCaps->dpnCaps;
            return DPN_OK;
    }

    return DPNERR_GENERIC;
}

// This function is a helper and assumes that you already have the global object lock
#undef DPF_MODNAME
#define DPF_MODNAME "DNSetCachedSPCaps"
HRESULT DNSetCachedSPCaps( DIRECTNETOBJECT *pdnObject, const GUID * const pguidSP, const DPN_SP_CAPS * const pCaps )
{
    PDN_SP_CAPS_ENTRY pdpSPCaps;
    PDPN_SP_CAPS dpnCaps;
    HRESULT hr;

    hr = DNGetCachedSPCaps( pdnObject, pguidSP, &dpnCaps );

    if( FAILED( hr ) )
    {
        DPF( 5, "Creating new cache entry for SP" );

        pdpSPCaps = new DN_SP_CAPS_ENTRY;

        if( pdpSPCaps == NULL )
        {
            DPF( 0, "Error allocating memory for SP caps" );
            return DPNERR_OUTOFMEMORY;
        }

        pdpSPCaps->blList.Initialize();

        if (pguidSP != NULL)
            pdpSPCaps->guidSP = *pguidSP;
        pdpSPCaps->dpnCaps.dwFlags = pCaps->dwFlags;
        pdpSPCaps->dpnCaps.dwSize = sizeof( DPN_SP_CAPS );
        pdpSPCaps->dpnCaps.dwNumThreads = pCaps->dwNumThreads;
		pdpSPCaps->dpnCaps.dwBuffersPerThread = pCaps->dwBuffersPerThread;
		pdpSPCaps->dpnCaps.dwSystemBufferSize = pCaps->dwSystemBufferSize;

        pdpSPCaps->blList.InsertAfter( &pdnObject->blSPCapsList );
    }
    else
    {
        DPF( 5, "Setting info on current cached SP entry" );

        dpnCaps->dwNumThreads = pCaps->dwNumThreads;
		dpnCaps->dwBuffersPerThread = pCaps->dwBuffersPerThread;
		dpnCaps->dwSystemBufferSize = pCaps->dwSystemBufferSize;
        dpnCaps->dwFlags = pCaps->dwFlags;
    }

    return DPN_OK;
}

HRESULT DNCAPS_QueryInterface( IDP8SPCallback *pSP, LPVOID * ppvObj )
{
    *ppvObj = pSP;
    return DPN_OK;
}

ULONG DNCAPS_AddRef( IDP8SPCallback *pSP )
{
    return 1;
}

ULONG DNCAPS_Release( IDP8SPCallback *pSP )
{
    return 1;
}

HRESULT DNCAPS_IndicateEvent( IDP8SPCallback *pSP, SP_EVENT_TYPE spetEvent,LPVOID pvData )
{
    return DPN_OK;
}

HRESULT DNCAPS_CommandComplete( IDP8SPCallback *pSP,HANDLE hCommand,HRESULT hrResult,LPVOID pvData )
{
    return DPN_OK;
}


LPVOID dncapsspInterface[] =
{
    (LPVOID)DNCAPS_QueryInterface,
    (LPVOID)DNCAPS_AddRef,
    (LPVOID)DNCAPS_Release,
	(LPVOID)DNCAPS_IndicateEvent,
	(LPVOID)DNCAPS_CommandComplete
};


#undef DPF_MODNAME
#define DPF_MODNAME "DNSetActualSPCaps"

HRESULT DNSetActualSPCaps(DIRECTNETOBJECT *pdnObject,
						  CServiceProvider *const pSP,
						  const DPN_SP_CAPS * const pCaps)
{
    HRESULT				hResultCode;
    SPSETCAPSDATA		spSetCapsData;
    IDP8ServiceProvider	*pIDP8SP;

	DPF(6,"Parameters: pSP [0x%p], pCaps [0x%p]",pSP,pCaps);

	DNASSERT(pdnObject != NULL);
	DNASSERT(pSP != NULL);
	DNASSERT(pCaps != NULL);

	pIDP8SP = NULL;

	//
	//	Get SP interface
	//
	if ((hResultCode = pSP->GetInterfaceRef( &pIDP8SP )) != DPN_OK)
	{
		DPFERR("Could not get SP interface reference");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	//
	//	Set the SP caps
	//
    memset( &spSetCapsData, 0x00, sizeof( SPSETCAPSDATA ) );
    spSetCapsData.dwSize = sizeof( SPSETCAPSDATA );
    spSetCapsData.dwIOThreadCount = pCaps->dwNumThreads;
    spSetCapsData.dwBuffersPerThread = pCaps->dwBuffersPerThread;
	spSetCapsData.dwSystemBufferSize = pCaps->dwSystemBufferSize;

	if ((hResultCode = pIDP8SP->lpVtbl->SetCaps( pIDP8SP, &spSetCapsData )) != DPN_OK)
	{
		DPFERR("Could not set SP caps");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pIDP8SP->lpVtbl->Release( pIDP8SP );
	pIDP8SP = NULL;

Exit:
	DPF(6,"Returning: [0x%lx]",hResultCode);
    return(hResultCode);

Failure:
	if (pIDP8SP)
	{
        pIDP8SP->lpVtbl->Release(pIDP8SP);
		pIDP8SP = NULL;
	}
	goto Exit;
}


// SP should be loaded
//

#undef DPF_MODNAME
#define DPF_MODNAME "DNGetActualSPCaps"

HRESULT DNGetActualSPCaps(DIRECTNETOBJECT *pdnObject,
						  const GUID * const pguidSP,
						  PDPN_SP_CAPS pCaps)
{
    HRESULT				hResultCode;
    SPGETCAPSDATA		spGetCapsData;
    IDP8ServiceProvider	*pIDP8SP;
    CServiceProvider	*pSP;

	DPF(6,"Parameters: pguidSP [0x%p], pCaps [0x%p]",pguidSP,pCaps);

	DNASSERT(pdnObject != NULL);

	pIDP8SP = NULL;
	pSP = NULL;

	//
	//	Find SP interface - either we have already loaded it, in which case we will take a reference,
	//	or it has not been loaded yet, in which case we will instantiate it.  Either way, we will
	//	release it once we have the actual SP COM interface.
	//
	if ((hResultCode = DN_SPFindEntry(pdnObject,NULL,&pSP)) == DPNERR_DOESNOTEXIST)
	{
		//
		//	Instantiate SP and add to Protocol
		//
		if ((hResultCode = DN_SPInstantiate(pdnObject,&GUID_NULL,&pSP)) != DPN_OK)
		{
			DPFERR("Could not load SP");
			DisplayDNError(0,hResultCode);
			goto Failure;
		}
	}
	else
	{
		if (hResultCode != DPN_OK)
		{
			DPFERR("Could not find SP");
			DisplayDNError(0,hResultCode);
			goto Failure;
		}
	}

	DNASSERT(pSP != NULL);

	if ((hResultCode = pSP->GetInterfaceRef( &pIDP8SP )) != DPN_OK)
	{
		DPFERR("Could not get SP interface reference");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	//
	//	Get the SP caps
	//
    memset( &spGetCapsData, 0x00, sizeof( SPGETCAPSDATA ) );
    spGetCapsData.dwSize = sizeof( SPGETCAPSDATA );
	spGetCapsData.hEndpoint = INVALID_HANDLE_VALUE;
    if ((hResultCode = pIDP8SP->lpVtbl->GetCaps( pIDP8SP, &spGetCapsData )) != DPN_OK)
	{
		DPFERR("Could not get SP caps");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	pIDP8SP->lpVtbl->Release( pIDP8SP );
	pIDP8SP = NULL;
	pSP->Release();
	pSP = NULL;

	//
    //	Map from SP structure to our own
	//
	pCaps->dwFlags = spGetCapsData.dwFlags;
    pCaps->dwNumThreads = spGetCapsData.dwIOThreadCount;
	pCaps->dwDefaultEnumCount = spGetCapsData.dwDefaultEnumRetryCount;
	pCaps->dwDefaultEnumRetryInterval = spGetCapsData.dwDefaultEnumRetryInterval;
	pCaps->dwDefaultEnumTimeout = spGetCapsData.dwDefaultEnumTimeout;
	pCaps->dwMaxEnumPayloadSize = spGetCapsData.dwEnumFrameSize - sizeof( DN_ENUM_QUERY_PAYLOAD );
	pCaps->dwBuffersPerThread = spGetCapsData.dwBuffersPerThread;
	pCaps->dwSystemBufferSize = spGetCapsData.dwSystemBufferSize;

Exit:
	DPF(6,"Returning: [0x%lx]",hResultCode);
    return(hResultCode);

Failure:
	if (pIDP8SP)
	{
        pIDP8SP->lpVtbl->Release(pIDP8SP);
		pIDP8SP = NULL;
	}
	if (pSP)
	{
		pSP->Release();
		pSP = NULL;
	}
	goto Exit;
}
