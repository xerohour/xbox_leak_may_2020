/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Enum_SP.cpp
 *  Content:    DNET service provider enumeration routines
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/28/99	mjn		Created
 *	01/05/00	mjn		Return DPNERR_NOINTERFACE if CoCreateInstance fails
 *	01/07/00	mjn		Moved Misc Functions to DNMisc.h
 *	01/11/00	mjn		Use CPackedBuffers instead of DN_ENUM_BUFFER_INFOs
 *	01/18/00	mjn		Converted EnumAdapters registry interface to use CRegistry
 *	01/24/00	mjn		Converted EnumSP registry interface to use CRegistry
 *	04/07/00	mjn		Fixed MemoryHeap corruption problem in DN_EnumSP
 *	04/08/00	mjn		Added DN_SPCrackEndPoint()
 *	05/01/00	mjn		Prevent unusable SPs from being enumerated.
 *	05/02/00	mjn		Better clean-up for DN_SPEnsureLoaded()
 *	05/03/00	mjn		Added DPNENUMSERVICEPROVIDERS_ALL flag
 *	05/23/00	mjn		Fixed cast from LPGUID to GUID*
 *	06/27/00	rmt		Added COM abstraction
 *	07/20/00	mjn		Return SP count from DN_EnumSP() even when buffer is too small
 *	07/29/00	mjn		Added fUseCachedCaps to DN_SPEnsureLoaded()
 *  08/05/00    RichGr  IA64: Use %p format specifier in DPFs for 32/64-bit pointers and handles.
 *	08/16/00	mjn		Removed DN_SPCrackEndPoint()
 *	08/20/00	mjn		Added DN_SPInstantiate(), DN_SPLoad()
 *				mjn		Removed fUseCachedCaps from DN_SPEnsureLoaded()
 *	09/25/00	mjn		Handle SP initialization failure in DN_EnumAdapters()
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dncorei.h"

#undef DPF_MODNAME
#define DPF_MODNAME "DN_EnumSP"

HRESULT DN_EnumSP(DIRECTNETOBJECT *const pdnObject,
				  const DWORD dwFlags,
				  const GUID *const lpguidApplication,
				  DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer,
				  DWORD *const pcbEnumData,
				  DWORD *const pcReturned)
{
	HRESULT	hResultCode = DPN_OK;
	DWORD	dwSizeRequired;
	DPN_SERVICE_PROVIDER_INFO	dnSpInfo;
	CPackedBuffer				packedBuffer;
	WCHAR wszSP[] = L"DirectPlay8 TCP/IP Service Provider";
	GUID guidSP = { 0xEBFE7BA0, 0x628D, 0x11D2, { 0xAE, 0x0F, 0x00, 0x60, 0x97, 0xB0, 0x14, 0x11 } };

	DPF(4,"Parameters: dwFlags [0x%lx], lpguidApplication [0x%p], pSPInfoBuffer [0x%p], pcbEnumData [0x%p], pcReturned [0x%p]",
		dwFlags,lpguidApplication,pSPInfoBuffer,pcbEnumData,pcReturned);

	
	DNASSERT(pcbEnumData != NULL);
	DNASSERT(pcReturned != NULL);

	dwSizeRequired = *pcbEnumData;
	packedBuffer.Initialize(static_cast<void*>(pSPInfoBuffer),dwSizeRequired);

	hResultCode = packedBuffer.AddToBack(wszSP,(wcslen(wszSP)+1)*sizeof(WCHAR));
	dnSpInfo.pwszName = static_cast<WCHAR*>(packedBuffer.GetTailAddress());

	memcpy(&dnSpInfo.guid,&guidSP,sizeof(GUID));

	dnSpInfo.dwFlags = 0;
	dnSpInfo.dwReserved = 0;
	dnSpInfo.pvReserved = NULL;
	hResultCode = packedBuffer.AddToFront(&dnSpInfo,sizeof(DPN_SERVICE_PROVIDER_INFO));

	dwSizeRequired = packedBuffer.GetSizeRequired();
	if (dwSizeRequired > *pcbEnumData)
	{
		DPF(5,"Buffer too small");
		*pcbEnumData = dwSizeRequired;
		*pcReturned = 1;
		hResultCode = DPNERR_BUFFERTOOSMALL;
		goto Exit;
	}
	else
	{
		*pcReturned = 1;
	}

	DPF(5,"*pcbEnumData [%ld], *pcReturned [%ld]",*pcbEnumData,*pcReturned);

Exit:
	DPF(4,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_SPReleaseAll"

void DN_SPReleaseAll(DIRECTNETOBJECT *const pdnObject)
{
	CBilink				*pBilink;
	CServiceProvider	*pSP;

	DPF(6,"Parameters: (none)");

	DNASSERT(pdnObject != NULL);

	DNEnterCriticalSection(&pdnObject->csServiceProviders);

	pBilink = pdnObject->m_bilinkServiceProviders.GetNext();
	while (pBilink != &pdnObject->m_bilinkServiceProviders)
	{
		pSP = CONTAINING_OBJECT(pBilink,CServiceProvider,m_bilinkServiceProviders);
		pBilink = pBilink->GetNext();
		pSP->m_bilinkServiceProviders.RemoveFromList();
		pSP->Release();
		pSP = NULL;
	}

	DNLeaveCriticalSection(&pdnObject->csServiceProviders);

	DPF(6,"Returning");
}



//	DN_SPFindEntry
//
//	Find a connected SP and AddRef it if it exists

#undef DPF_MODNAME
#define DPF_MODNAME "DN_SPFindEntry"

HRESULT DN_SPFindEntry(DIRECTNETOBJECT *const pdnObject,
					   const GUID *const pguid,
					   CServiceProvider **const ppSP)
{
	HRESULT				hResultCode;
	CBilink				*pBilink;
	CServiceProvider	*pSP;

	DPF(6,"Parameters: pguid [0x%p], ppSP [0x%p]",pguid,ppSP);

	DNEnterCriticalSection(&pdnObject->csServiceProviders);

	hResultCode = DPNERR_DOESNOTEXIST;
	pBilink = pdnObject->m_bilinkServiceProviders.GetNext();
	while (pBilink != &pdnObject->m_bilinkServiceProviders)
	{
		pSP = CONTAINING_OBJECT(pBilink,CServiceProvider,m_bilinkServiceProviders);
			pSP->AddRef();
			*ppSP = pSP;
			hResultCode = DPN_OK;
			break;
		pBilink = pBilink->GetNext();
	}

	DNLeaveCriticalSection(&pdnObject->csServiceProviders);

	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


//	DN_SPInstantiate
//
//	Instantiate an SP, regardless of whether it's loaded or not

#undef DPF_MODNAME
#define DPF_MODNAME "DN_SPInstantiate"

HRESULT DN_SPInstantiate(DIRECTNETOBJECT *const pdnObject,
						 const GUID *const pguid,
						 CServiceProvider **const ppSP)
{
	HRESULT		hResultCode;
	CServiceProvider	*pSP;

	DPF(6,"Parameters: pguid [0x%p], ppSP [0x%p]",pguid,ppSP);

	pSP = NULL;

	//
	//	Create and initialize SP
	//
	pSP = new CServiceProvider;
	if (pSP == NULL)
	{
		DPFERR("Could not create SP");
		hResultCode = DPNERR_OUTOFMEMORY;
		goto Failure;
	}
	if ((hResultCode = pSP->Initialize(pdnObject,pguid)) != DPN_OK)
	{
		DPFERR("Could not initialize SP");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	if (ppSP)
	{
		pSP->AddRef();
		*ppSP = pSP;
	}

	pSP->Release();
	pSP = NULL;

	hResultCode = DPN_OK;

Exit:
	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pSP)
	{
		pSP->Release();
		pSP = NULL;
	}
	goto Exit;
}


//	DN_SPLoad
//
//	Load an SP, and set caps

#undef DPF_MODNAME
#define DPF_MODNAME "DN_SPLoad"

HRESULT DN_SPLoad(DIRECTNETOBJECT *const pdnObject,
				  const GUID *const pguid,
				  CServiceProvider **const ppSP)
{
	HRESULT		hResultCode;
	DPN_SP_CAPS	*pCaps;
	CBilink		*pBilink;
	CServiceProvider	*pSP;

	DPF(6,"Parameters: pguid [0x%p], ppSP [0x%p]",pguid,ppSP);

	pSP = NULL;
	pCaps = NULL;

	//
	//	Instantiate SP
	//
	if ((hResultCode = DN_SPInstantiate(pdnObject,pguid,&pSP)) != DPN_OK)
	{
		DPFERR("Could not instantiate SP");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	//
	//	Try to load SP caps (if they exist)
	//
	if ((hResultCode = DNGetCachedSPCaps(pdnObject,pguid,&pCaps)) == DPN_OK)
	{
		if ((hResultCode = DNSetActualSPCaps(pdnObject,pSP,pCaps)) != DPN_OK)
		{
			DPFERR("Could not set SP caps");
			DisplayDNError(0,hResultCode);
			goto Failure;
		}
	}

	DNASSERT(pSP != NULL);

	//
	//	Keep this loaded on the DirectNet object.  We will also check for duplicates.
	//
	DNEnterCriticalSection(&pdnObject->csServiceProviders);

	pBilink = pdnObject->m_bilinkServiceProviders.GetNext();
	while (pBilink != &pdnObject->m_bilinkServiceProviders)
	{
			DNLeaveCriticalSection(&pdnObject->csServiceProviders);
			DPFERR("SP is already loaded!");
			hResultCode = DPNERR_ALREADYINITIALIZED;
			goto Failure;
		pBilink = pBilink->GetNext();
	}

	pSP->AddRef();
	pSP->m_bilinkServiceProviders.InsertBefore(&pdnObject->m_bilinkServiceProviders);

	DNLeaveCriticalSection(&pdnObject->csServiceProviders);

	if (ppSP)
	{
		pSP->AddRef();
		*ppSP = pSP;
	}

	pSP->Release();
	pSP = NULL;

	hResultCode = DPN_OK;

Exit:
	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pSP)
	{
		pSP->Release();
		pSP = NULL;
	}
	goto Exit;
}


//	DN_SPEnsureLoaded
//
//	Ensure that an SP is loaded.  If the SP is not loaded,
//	it will be instantiated, and connected to the protocol.
//	If it is loaded, its RefCount will be increased.

#undef DPF_MODNAME
#define DPF_MODNAME "DN_SPEnsureLoaded"

HRESULT DN_SPEnsureLoaded(DIRECTNETOBJECT *const pdnObject,
						  const GUID *const pguid,
						  CServiceProvider **const ppSP)
{
	HRESULT				hResultCode;
	CServiceProvider	*pSP;

	DPF(6,"Parameters: pguid [0x%p], ppSP [0x%p]",pguid,ppSP);

	pSP = NULL;

	//
	//	Try to find the SP
	//
	if ((hResultCode = DN_SPFindEntry(pdnObject,pguid,&pSP)) == DPNERR_DOESNOTEXIST)
	{
		//
		//	Instantiate SP and add to Protocol
		//
		if ((hResultCode = DN_SPLoad(pdnObject,pguid,&pSP)) != DPN_OK)
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

	if (ppSP != NULL)
	{
		pSP->AddRef();
		*ppSP = pSP;
	}

	pSP->Release();
	pSP = NULL;

Exit:
	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pSP)
	{
		pSP->Release();
		pSP = NULL;
	}
	goto Exit;
}

