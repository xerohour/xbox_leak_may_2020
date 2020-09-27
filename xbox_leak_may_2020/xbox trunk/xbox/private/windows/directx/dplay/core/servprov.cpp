/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ServProv.cpp
 *  Content:    Service Provider Objects
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  03/17/00	mjn		Created
 *  04/04/00    rmt     Added set of SP caps from cache (if cache exists).
 *	04/10/00	mjn		Farm out RemoveSP to worker thread
 *	05/02/00	mjn		Fixed RefCount issue
 *  06/09/00    rmt     Updates to split CLSID and allow whistler compat 
 *	07/06/00	mjn		Fixes to support SP handle to Protocol
 *  08/03/00	rmt		Bug #41244 - Wrong return codes -- part 2  
 *  08/05/00    RichGr  IA64: Use %p format specifier in DPFs for 32/64-bit pointers and handles.
 *	08/06/00	mjn		Added CWorkerJob
 *	08/20/00	mjn		Changed Initialize() to not add SP to DirectNet object bilink
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

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************

#undef DPF_MODNAME
#define DPF_MODNAME "CServiceProvider::Initialize"

HRESULT CServiceProvider::Initialize(DIRECTNETOBJECT *const pdnObject,const GUID *const pguid)
{
	HRESULT			hResultCode;
	IDP8ServiceProvider	*pISP;

	DNASSERT(pdnObject != NULL);
	DNASSERT(pguid != NULL);

	pISP = NULL;

	m_pdnObject = pdnObject;

	//
	//	Instantiate SP
	//
	if ((hResultCode = COM_CoCreateInstance(*pguid,
										NULL,
										GUID_NULL,
										reinterpret_cast<void**>(&pISP))) != DPN_OK)
	{
		DPFERR("Could not instantiate SP");
		DisplayDNError(0,hResultCode);
		hResultCode = DPNERR_DOESNOTEXIST;
		goto Exit;
	}

	//
	//	Add SP to Protocol Layer
	//
	DNProtocolAddRef(pdnObject);
	if ((hResultCode = DNPAddServiceProvider(m_pdnObject->pdnProtocolData,pISP,&m_hProtocolSPHandle)) != DPN_OK)
	{
		DPFERR("COULD NOT ADD SERVICE PROVIDER TO PROTOCOL !  (Is this SP supported or correctly installed locally ?)");
		DisplayDNError(0,hResultCode);
		DNProtocolRelease(pdnObject);
		goto Failure;
	}
	m_dwFlags |= DN_SERVICE_PROVIDER_FLAG_LOADED;

	pISP->lpVtbl->AddRef(pISP);
	m_pISP = pISP;
	pISP->lpVtbl->Release(pISP);
	pISP = NULL;

	m_guid = *pguid;
/*	REMOVE
	// Add to bilink
	AddRef();
	m_bilink.InsertBefore(&m_pdnObject->m_bilinkServiceProviders);
*/

	hResultCode = DPN_OK;

Exit:
	return(hResultCode);

Failure:
	if (pISP)
	{
		pISP->lpVtbl->Release(pISP);
		pISP = NULL;
	}
	goto Exit;
};


#undef DPF_MODNAME
#define DPF_MODNAME "CServiceProvider::Release"

void CServiceProvider::Release( void )
{
	HRESULT		hResultCode;
	LONG		lRefCount;

	lRefCount = InterlockedDecrement(&m_lRefCount);
	DPF(9,"[0x%p] new RefCount [%ld]",this,lRefCount);
	DNASSERT(lRefCount >= 0);
	if (lRefCount == 0)
	{
		if (m_dwFlags & DN_SERVICE_PROVIDER_FLAG_LOADED)
		{
			CWorkerJob	*pWorkerJob;

			pWorkerJob = NULL;

			if ((hResultCode = WorkerJobNew(m_pdnObject,&pWorkerJob)) == DPN_OK)
			{
				pWorkerJob->SetJobType( WORKER_JOB_REMOVE_SERVICE_PROVIDER );
				pWorkerJob->SetRemoveServiceProviderHandle( m_hProtocolSPHandle );

				DNQueueWorkerJob(m_pdnObject,pWorkerJob);
				pWorkerJob = NULL;
			}
			else
			{
				DPFERR("Could not remove SP");
				DisplayDNError(0,hResultCode);
				DNASSERT(FALSE);
			}
			if (m_pISP)
			{
				m_pISP->lpVtbl->Release(m_pISP);
				m_pISP = NULL;
			}
		}
		m_pdnObject = NULL;

		delete this;
	}
}


#undef DPF_MODNAME
#define DPF_MODNAME "CServiceProvider::GetInterfaceRef"

HRESULT CServiceProvider::GetInterfaceRef( IDP8ServiceProvider **ppIDP8SP )
{
	DNASSERT( ppIDP8SP != NULL );

	if (m_pISP == NULL)
	{
		return( DPNERR_GENERIC );
	}

	m_pISP->lpVtbl->AddRef( m_pISP );
	*ppIDP8SP = m_pISP;

	return( DPN_OK );
}

