/***************************************************************************
 *
 *	Copyright (C) 1999-2000 Microsoft Corporation.	All Rights Reserved.
 *
 *	File:		 effects.cpp
 *
 *	Content:	 Implementation of the CEffectChain class and the CEffect
 *				 class hierarchy (CEffect, CDmoEffect and CSendEffect).
 *
 *	Description: These classes support audio effects and effect sends, a new
 *				 feature in DX8.  The CDirectSoundSecondaryBuffer object is
 *				 extended with a pointer to an associated CEffectChain,
 *				 which in turn manages a list of CEffect-derived objects.
 *
 *				 Almost everything here would fit more logically into the
 *				 existing CDirectSoundSecondaryBuffer class, but has been
 *				 segregated for ease of maintenance (and because dsbuf.cpp
 *				 is complex enough as it is).  So the CEffectChain object
 *				 should be understood as a sort of helper object belonging to
 *				 CDirectSoundSecondaryBuffer.  In particular, a CEffectChain
 *				 object's lifetime is contained by the lifetime of its owning
 *				 CDirectSoundSecondaryBuffer, so we can safely fiddle with
 *				 this buffer's innards at any time in CEffectChain code.
 *
 *	History:
 *
 * Date 	 By 	  Reason
 * ========  =======  ======================================================
 * 08/10/99  duganp   Created
 *
 ***************************************************************************/

#include "stdafx.h"
#include <mmsystem.h>
#include "dsoundp.h"
#include "effects.h"
#include "audiosink.h"
#include "cconduct.h"
#include <uuids.h>		// For MEDIATYPE_Audio, MEDIASUBTYPE_PCM and FORMAT_WaveFormatEx
#include "dsbufcfg.h"
#include "decibels.h"



/***************************************************************************
 *
 *	CEffectChain::CEffectChain
 *
 *	Description:
 *		Object constructor.
 *
 *	Arguments:
 *		CBuffer* [in]: Pointer to our associated buffer.
 *
 *	Returns:
 *		(void)
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffectChain::CEffectChain"

CEffectChain::CEffectChain(CBuffer* pBuffer)
{
	//DPF_ENTER();
	//DPF_CONSTRUCT(CEffectChain);

	// Set up initial values
	m_hrInit		= DSERR_UNINITIALIZED;
	m_pDsBuffer 	= pBuffer;

	// Keep a pointer to the audio format for convenience
	m_pFormat = pBuffer->Format();

	m_fHasSend = FALSE;

	m_cRef = 0;

	AddRef();
	//DPF(DPFLVL_INFO, "Created effect chain with PreFxBuffer=0x%p, PostFxBuffer=0x%p, BufSize=%lu",
	//	  m_pPreFxBuffer, m_pPostFxBuffer, m_dwBufSize);
	//DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *	CEffectChain::~CEffectChain
 *
 *	Description:
 *		Object destructor.
 *
 *	Arguments:
 *		(void)
 *
 *	Returns:
 *		(void)
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffectChain::~CEffectChain"

CEffectChain::~CEffectChain(void)
{
	//DPF_ENTER();
	//DPF_DESTRUCT(CEffectChain);

	// Ensure we don't re-enter the destructor
	m_cRef = LONG_MAX / 2;

	// m_fxList's destructor takes care of releasing our CEffect objects
	// Not anymore...
	while( !m_fxList.IsEmpty() )
	{
		m_fxList.RemoveHead()->Release();
	}

	//DPF_LEAVE_VOID();
}



STDMETHODIMP CEffectChain::QueryInterface(const IID &iid, void **ppv)
{
	if(iid == IID_IUnknown )
	{
		*ppv = static_cast<IUnknown*>(this);
	} 
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	
	return S_OK;
}

STDMETHODIMP_(ULONG) CEffectChain::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CEffectChain::Release()
{
	if(!InterlockedDecrement(&m_cRef))
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

/***************************************************************************
 *
 *	CEffectChain::Initialize
 *
 *	Description:
 *		Initializes the chain with the effects requested.
 *
 *	Arguments:
 *		DWORD [in]: Number of effects requested
 *		LPDSEFFECTDESC [in]: Pointer to effect description structures
 *		DWORD* [out]: Receives the effect creation status codes
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffectChain::Initialize"

HRESULT CEffectChain::Initialize(DWORD dwFxCount, LPDSEFFECTDESC pFxDesc, LPDWORD pdwResultCodes)
{
	HRESULT hr = DS_OK;
	//DPF_ENTER();
	HRESULT hrFirstFailure = DS_OK; // HR for the first FX creation failure

	ASSERT(dwFxCount > 0);
	if( !IS_VALID_READ_PTR(pFxDesc, sizeof( DSEFFECTDESC ) ) )
	{
		return DSERR_INVALIDPARAM;
	}

	DMO_MEDIA_TYPE dmt;
	ZeroMemory(&dmt, sizeof dmt);
	dmt.majortype				= MEDIATYPE_Audio;
	dmt.subtype 				= MEDIASUBTYPE_PCM;
	dmt.bFixedSizeSamples		= TRUE;
	dmt.bTemporalCompression	= FALSE;
	dmt.lSampleSize 			= m_pFormat->wBitsPerSample == 16 ? 2 : 1;
	dmt.formattype				= FORMAT_WaveFormatEx;
	dmt.cbFormat				= sizeof(WAVEFORMATEX);
	dmt.pbFormat				= PBYTE(m_pFormat);

	for (DWORD i=0; i<dwFxCount; ++i)
	{
		CEffect* pEffect = NULL;
		BOOL fIsSend = FALSE;

		if (pFxDesc[i].guidDSFXClass == GUID_DSFX_SEND /*|| pFxDesc[i].guidDSFXClass == GUID_DSFX_STANDARD_I3DL2SOURCE*/)
#ifdef ENABLE_SENDS
		{
			fIsSend = TRUE;
			pEffect = new CSendEffect(pFxDesc[i], m_pDsBuffer);
		}
#else
		hr = DSERR_INVALIDPARAM;
#endif // ENABLE_SENDS
		else
		{
			//fIsSend = FALSE;
			pEffect = new CDmoEffect(pFxDesc[i]);
		}

		if( pEffect == NULL )
		{
			hr = E_OUTOFMEMORY;
		}

		if (SUCCEEDED(hr))
			hr = pEffect->Initialize(&dmt);

		if (SUCCEEDED(hr))
		{
			m_fxList.AddTail(pEffect);
			pEffect->AddRef();
			m_fHasSend = m_fHasSend || fIsSend;
			if (pdwResultCodes)
				pdwResultCodes[i] = DSFXR_PRESENT;
		}
		else // We didn't get the effect for some reason.
		{
			if (pdwResultCodes)
				pdwResultCodes[i] = (hr == DSERR_SENDLOOP) ? DSFXR_SENDLOOP : DSFXR_UNKNOWN;
			if (SUCCEEDED(hrFirstFailure))
				hrFirstFailure = hr;
		}

		RELEASE(pEffect);  // It's managed by m_fxList now
	}

	hr = hrFirstFailure;

	m_hrInit = hr;
	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CEffectChain::Clone
 *
 *	Description:
 *		Creates a replica of this effect chain object (or should do!).
 *
 *	Arguments:
 *		FIXME - the arguments will change when we do a 'true' Clone.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffectChain::Clone"

HRESULT CEffectChain::Clone(CDirectSoundBufferConfig* pDSBConfigObj)
{
	HRESULT hr = DS_OK;
	//DPF_ENTER();

	//CHECK_WRITE_PTR(pDSBConfigObj);
	if( !IS_VALID_WRITE_PTR(pDSBConfigObj, sizeof( CDirectSoundBufferConfig ) ) )
	{
		return DSERR_INVALIDPARAM;
	}

	DMO_MEDIA_TYPE dmt;
	ZeroMemory(&dmt, sizeof dmt);
	dmt.majortype				= MEDIATYPE_Audio;
	dmt.subtype 				= MEDIASUBTYPE_PCM;
	dmt.bFixedSizeSamples		= TRUE;
	dmt.bTemporalCompression	= FALSE;
	dmt.lSampleSize 			= m_pFormat->wBitsPerSample == 16 ? 2 : 1;
	dmt.formattype				= FORMAT_WaveFormatEx;
	dmt.cbFormat				= sizeof(WAVEFORMATEX);
	dmt.pbFormat				= PBYTE(m_pFormat);

	CDirectSoundBufferConfig::CDXDMODesc *pDXDMOMap = pDSBConfigObj->m_pDXDMOMapList;
	for (; pDXDMOMap && SUCCEEDED(hr); pDXDMOMap = pDXDMOMap->pNext)
	{
		DSEFFECTDESC effectDesc;
		effectDesc.dwSize = sizeof effectDesc;
		effectDesc.dwFlags = pDXDMOMap->m_dwEffectFlags;
		effectDesc.guidDSFXClass = pDXDMOMap->m_guidDSFXClass;
		effectDesc.dwReserved2 = pDXDMOMap->m_dwReserved;
		effectDesc.dwReserved1 = NULL;

		CEffect* pEffect = NULL;

		// If this is a send effect, map the send buffer GUID to an actual buffer interface pointer
		if (pDXDMOMap->m_guidDSFXClass == GUID_DSFX_SEND
#ifdef ENABLE_I3DL2SOURCE
			|| pDXDMOMap->m_guidDSFXClass == GUID_DSFX_STANDARD_I3DL2SOURCE
#endif
			)
		{
			CBuffer* pSendBuffer = m_pDsBuffer->m_pParentSink->FindBufferFromGUID(pDXDMOMap->m_guidSendBuffer);
			if (pSendBuffer)
			{
#ifndef _WIN64	// FIXME: temporary fix for Win64 build error
				effectDesc.dwReserved1 = DWORD(pSendBuffer);
				if(IsValidEffectDesc(&effectDesc, m_pDsBuffer))
				{
					CSendEffect* pSendEffect;
					pSendEffect = new CSendEffect(effectDesc, m_pDsBuffer);
					pEffect = pSendEffect;
					if( pEffect == NULL )
					{
						hr = E_OUTOFMEMORY;
					}
					if (SUCCEEDED(hr))
						hr = pEffect->Initialize(&dmt);
					if (SUCCEEDED(hr))
					{
						DSFXSend SendParam;
						SendParam.lSendLevel = pDXDMOMap->m_lSendLevel;
						hr = pSendEffect->SetAllParameters(&SendParam);
					}
				}
				else
#endif // _WIN64 
				{
					hr = DSERR_INVALIDPARAM;
				}
			}
			else
			{
				hr = DSERR_BADSENDBUFFERGUID;
			}
		}
		else
		{
			pEffect = new CDmoEffect(effectDesc);
			if( pEffect == NULL )
			{
				hr = E_OUTOFMEMORY;
			}
			// FIXME: Do we need to validate pEffect as well?
			if (SUCCEEDED(hr))
				hr = pEffect->Clone(pDXDMOMap->m_pMediaObject, &dmt);
		}

		if (SUCCEEDED(hr))
		{
			m_fxList.AddTail(pEffect);
			pEffect->AddRef();
			if (pDXDMOMap->m_guidDSFXClass == GUID_DSFX_SEND
#ifdef ENABLE_I3DL2SOURCE
				|| pDXDMOMap->m_guidDSFXClass == GUID_DSFX_STANDARD_I3DL2SOURCE
#endif
				)
				m_fHasSend = TRUE;
		}

		RELEASE(pEffect);  // It's managed by m_fxList now
	}

	m_hrInit = hr;
	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CEffectChain::AcquireFxResources
 *
 *	Description:
 *		Allocates each effect to software (host processing) or hardware
 *		(processed by the audio device), according to its creation flags.
 *
 *	Arguments:
 *		(void)
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *				 Will return the partial success code DS_INCOMPLETE if any
 *				 effects that didn't obtain resources were marked OPTIONAL.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffectChain::AcquireFxResources"

HRESULT CEffectChain::AcquireFxResources(void)
{
	HRESULT hr = DS_OK;
	HRESULT hrTemp;
	//DPF_ENTER();

	// FIXME: Don't reacquire resources unnecessarily; only if (we have none / they're suboptimal?)

	// We loop through all the effects, even if some of them fail,
	// in order to return more complete information to the app

	for (POSITION pos = m_fxList.GetHeadPosition(); pos; m_fxList.GetNext(pos))
	{
		hrTemp = m_fxList.GetAt( pos )->AcquireFxResources();
		if (FAILED(hrTemp))
			hr = hrTemp;
		else if (hrTemp == DS_INCOMPLETE && SUCCEEDED(hr))
			hr = DS_INCOMPLETE;
	}

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CEffectChain::GetFxStatus
 *
 *	Description:
 *		Obtains the current effects' resource allocation status codes.
 *
 *	Arguments:
 &		DWORD* [out]: Receives the resource acquisition status codes
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffectChain::GetFxStatus"

HRESULT CEffectChain::GetFxStatus(LPDWORD pdwResultCodes)
{
	//DPF_ENTER();
	ASSERT(IS_VALID_WRITE_PTR(pdwResultCodes, GetFxCount() * sizeof(DWORD)));

	DWORD n = 0;
	for (POSITION pos = m_fxList.GetHeadPosition(); pos; m_fxList.GetNext(pos))
		pdwResultCodes[n++] = m_fxList.GetAt( pos )->m_fxStatus;

	//DPF_LEAVE_HRESULT(DS_OK);
	return DS_OK;
}


/***************************************************************************
 *
 *	CEffectChain::GetEffectInterface
 *
 *	Description:
 *		Searches the effect chain for an effect with a given COM CLSID and
 *		interface IID at a given index; returns a pointer to the interface.
 *
 *	Arguments:
 *		REFGUID [in]: CLSID required, or GUID_All_Objects for any CLSID.
 *		DWORD [in]: Index N of effect desired.	If the first argument was
 *					GUID_All_Objects, we will return the Nth effect in the
 *					chain; and if it was a specific CLSID, we return the
 *					Nth effect with that CLSID.
 *		REFGUID [in]: Interface to query for from the selected effect.
 *		VOID** [out]: Receives a pointer to the requested COM interface.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffectChain::GetEffectInterface"

HRESULT CEffectChain::GetEffectInterface(REFGUID guidObject, DWORD dwIndex, REFGUID iidInterface, LPVOID *ppObject)
{
	HRESULT hr = DSERR_OBJECTNOTFOUND;
	//DPF_ENTER();

	BOOL fAllObjects = (guidObject == GUID_All_Objects);

	DWORD count = 0;
	for (POSITION pos = m_fxList.GetHeadPosition(); pos; m_fxList.GetNext(pos))
		if (fAllObjects || m_fxList.GetAt(pos)->m_fxDescriptor.guidDSFXClass == guidObject)
			if (count++ == dwIndex)
				break;

	if (pos)
		hr = m_fxList.GetAt(pos)->GetInterface(iidInterface, ppObject);

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}



/***************************************************************************
 *
 *	CEffectChain::NotifyRelease
 *
 *	Description:
 *		Informs this effect chain of the release of a MIXIN buffer.  We in
 *		turn traverse our list of effects informing them, so that if one of
 *		them was a send to the MIXIN buffer it can react appropriately.
 *
 *	Arguments:
 *		CBuffer* [in]: Departing MIXIN buffer.
 *
 *	Returns:
 *		(void)
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffectChain::NotifyRelease"

void CEffectChain::NotifyRelease(CBuffer* pDsBuffer)
{
	//DPF_ENTER();

	// Call NotifyRelease() on each effect
	for (POSITION pos = m_fxList.GetHeadPosition(); pos; m_fxList.GetNext(pos))
		m_fxList.GetAt(pos)->NotifyRelease(pDsBuffer);

	//DPF_LEAVE_VOID();
}


void CEffectChain::Render( short *pnBuffer,DWORD dwLength,REFERENCE_TIME refTimeStart )
{
	HRESULT hr;

	// Call Process() on each effect
	for (POSITION pos = m_fxList.GetHeadPosition(); pos; m_fxList.GetNext(pos))
	{
		hr = m_fxList.GetAt(pos)->Process(dwLength, (BYTE *)pnBuffer, refTimeStart, 0/*dwSendOffset*/, m_pFormat );
		if (FAILED(hr))
		{
			//DPF(DPFLVL_WARNING, "DMO "DPF_GUID_STRING" failed with %s", DPF_GUID_VAL(pFxNode->m_data->m_fxDescriptor.guidDSFXClass), HRESULTtoSTRING(hr));
			break;
		}
	}
}


/***************************************************************************
 *
 *	CEffectChain::FxDiscontinuity
 *
 *	Description:
 *		Calls Discontinuity() on each effect of the effect chain.
 *
 *	Arguments:
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffectChain::FxDiscontinuity"

HRESULT CEffectChain::FxDiscontinuity(void)
{
	HRESULT hr = DS_OK;
	//DPF_ENTER();

	//DPF_TIMING(DPFLVL_INFO, "Discontinuity on effects chain at 0x%08X", this);

	//if (GetCurrentProcessId() != this->GetOwnerProcessId())
		//DPF(DPFLVL_MOREINFO, "Bailing out because we're being called from a different process");
	/*else*/ for (POSITION pos = m_fxList.GetHeadPosition(); pos && SUCCEEDED(hr); m_fxList.GetNext(pos))
		m_fxList.GetAt(pos)->Discontinuity();

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CEffect::CEffect
 *
 *	Description:
 *		Object constructor.
 *
 *	Arguments:
 *		DSEFFECTDESC& [in]: Effect description structure.
 *
 *	Returns:
 *		(void)
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffect::CEffect"

CEffect::CEffect(DSEFFECTDESC& fxDescriptor)
{
	//DPF_ENTER();
	//DPF_CONSTRUCT(CEffect);

	// Keep local copy of effect description structure
	m_fxDescriptor = fxDescriptor;

	// Initialize defaults
	m_fxStatus = DSFXR_UNALLOCATED;

	m_cRef = 0;

	AddRef();
	//DPF_LEAVE_VOID();
}


STDMETHODIMP CEffect::QueryInterface(const IID &iid, void **ppv)
{
	if(iid == IID_IUnknown )
	{
		*ppv = static_cast<IUnknown*>(this);
	} 
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	
	return S_OK;
}

STDMETHODIMP_(ULONG) CEffect::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CEffect::Release()
{
	if(!InterlockedDecrement(&m_cRef))
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

/***************************************************************************
 *
 *	CEffect::AcquireFxResources
 *
 *	Description:
 *		Acquires the hardware or software resources necessary to perform
 *		this effect.  Currently a bit of a no-op, but will come into its
 *		own when we do hardware acceleration of effects.
 *
 *	Arguments:
 *		(void)
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CEffect::AcquireFxResources"

HRESULT CEffect::AcquireFxResources(void)
{
	HRESULT hr = DS_OK;
	//DPF_ENTER();

	if (m_fxStatus == DSFXR_FAILED || m_fxStatus == DSFXR_UNKNOWN)
	{
		hr = DSERR_INVALIDCALL;
	}
	else if (m_fxStatus == DSFXR_UNALLOCATED)
	{
		if (m_fxDescriptor.dwFlags & DSFX_LOCHARDWARE)
		{
			hr = DSERR_INVALIDPARAM;
			m_fxStatus = DSFXR_FAILED;
		}
		else
		{
			m_fxStatus = DSFXR_LOCSOFTWARE;
		}
	}

	// FIXME: this code is due for resurrection in DX8.1
	//	  if (FAILED(hr) && (m_fxDescriptor.dwFlags & DSFX_OPTIONAL))
	//		  hr = DS_INCOMPLETE;

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CDmoEffect::CDmoEffect
 *
 *	Description:
 *		Object constructor.
 *
 *	Arguments:
 *		DSEFFECTDESC& [in]: Effect description structure.
 *
 *	Returns:
 *		(void)
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CDmoEffect::CDmoEffect"

CDmoEffect::CDmoEffect(DSEFFECTDESC& fxDescriptor)
	: CEffect(fxDescriptor)
{
	//DPF_ENTER();
	//DPF_CONSTRUCT(CDmoEffect);

	m_pMediaObject = NULL;
	m_pMediaObjectInPlace = NULL;

	// Check initial values
	ASSERT(m_pMediaObject == NULL);
	ASSERT(m_pMediaObjectInPlace == NULL);

	//DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *	CDmoEffect::~CDmoEffect
 *
 *	Description:
 *		Object destructor.
 *
 *	Arguments:
 *		(void)
 *
 *	Returns:
 *		(void)
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CDmoEffect::~CDmoEffect"

CDmoEffect::~CDmoEffect(void)
{
	//DPF_ENTER();
	//DPF_DESTRUCT(CDmoEffect);

	// During shutdown, if the buffer hasn't been freed, these calls can
	// cause an access violation because the DMO DLL has been unloaded.
	try
	{
		if (m_pMediaObject)
			m_pMediaObject->Release();
		if (m_pMediaObjectInPlace)
			m_pMediaObjectInPlace->Release();
	}
	catch (...) {}
	
	//DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *	CDmoEffect::Initialize
 *
 *	Description:
 *		Create the DirectX Media Object corresponding to this effect.
 *
 *	Arguments:
 *		DMO_MEDIA_TYPE* [in]: Wave format etc. information used to
 *							  initialize our contained DMO.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CDmoEffect::Initialize"

HRESULT CDmoEffect::Initialize(DMO_MEDIA_TYPE* pDmoMediaType)
{
	//DPF_ENTER();

	HRESULT hr = CoCreateInstance(m_fxDescriptor.guidDSFXClass, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (void**)&m_pMediaObject);

	if (SUCCEEDED(hr))
	{
		//CHECK_COM_INTERFACE(m_pMediaObject);
		hr = m_pMediaObject->QueryInterface(IID_IMediaObjectInPlace, (void**)&m_pMediaObjectInPlace);
		if (SUCCEEDED(hr))
		{
			//CHECK_COM_INTERFACE(m_pMediaObjectInPlace);
		}
		else
		{
			ASSERT(m_pMediaObjectInPlace == NULL);
			//DPF(DPFLVL_INFO, "Failed to obtain the IMediaObjectInPlace interface on effect "
			//	  DPF_GUID_STRING " (%s)", DPF_GUID_VAL(m_fxDescriptor.guidDSFXClass), HRESULTtoSTRING(hr));
		}

		// Throw away the previous return code - we can live without IMediaObjectInPlace
		hr = m_pMediaObject->SetInputType(0, pDmoMediaType, 0);
		if (SUCCEEDED(hr))
			hr = m_pMediaObject->SetOutputType(0, pDmoMediaType, 0);
	}

	if (FAILED(hr))
	{
		RELEASE(m_pMediaObject);
		RELEASE(m_pMediaObjectInPlace);
	}

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CDmoEffect::Clone
 *
 *	Description:
 *		Creates a replica of this DMO effect object (or should do!).
 *
 *	Arguments:
 *		FIXME - the arguments will change when we do a 'true' Clone.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CDmoEffect::Clone"

HRESULT CDmoEffect::Clone(IMediaObject *pMediaObject, DMO_MEDIA_TYPE* pDmoMediaType)
{
	//DPF_ENTER();

	IMediaObjectInPlace *pMediaObjectInPlace = NULL;

	HRESULT hr = pMediaObject->QueryInterface(IID_IMediaObjectInPlace, (void**)&pMediaObjectInPlace);
	if (SUCCEEDED(hr))
	{
		//CHECK_COM_INTERFACE(pMediaObjectInPlace);
		hr = pMediaObjectInPlace->Clone(&m_pMediaObjectInPlace);
		pMediaObjectInPlace->Release();

		if (SUCCEEDED(hr))
		{
			//CHECK_COM_INTERFACE(m_pMediaObjectInPlace);
			hr = m_pMediaObjectInPlace->QueryInterface(IID_IMediaObject, (void**)&m_pMediaObject);
		}
		if (SUCCEEDED(hr))
		{
			//CHECK_COM_INTERFACE(m_pMediaObject);
			hr = m_pMediaObject->SetInputType(0, pDmoMediaType, 0);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pMediaObject->SetOutputType(0, pDmoMediaType, 0);
		}
	}

	if (FAILED(hr))
	{
		RELEASE(m_pMediaObject);
		RELEASE(m_pMediaObjectInPlace);
	}

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CDmoEffect::Process
 *
 *	Description:
 *		Actually invoke effect processing on our contained DMO.
 *
 *	Arguments:
 *		DWORD [in]: Number of audio bytes to process.
 *		BYTE* [in, out]: Pointer to start of audio buffer to process.
 *		REFERENCE_TIME [in]: Timestamp of first sample to be processed
 *		DWORD [ignored]: Offset of a wrapped audio region.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CDmoEffect::Process"

HRESULT CDmoEffect::Process(DWORD dwBytes, BYTE *pAudio, REFERENCE_TIME rtTime, DWORD /*ignored*/, LPWAVEFORMATEX pFormat)
{
	HRESULT hr = DS_OK;
	//DPF_ENTER();

	// if (m_fxStatus == DSFXR_LOCSOFTWARE) ...
	// FIXME: We may need to handle hardware and software buffers differently here.

	if (m_pMediaObjectInPlace)	// If the DMO provides this interface, use it
	{
		static const int nPeriod = 3;

		// We divide the region to be processed into nPeriod-ms pieces so that the
		// DMO's parameter curve will have a nPeriod-ms update period (manbug 36228)

		DWORD dwStep = MsToBytes(nPeriod, pFormat);
		for (DWORD dwCur = 0; dwCur < dwBytes && SUCCEEDED(hr); dwCur += dwStep)
		{
			if (dwStep > dwBytes - dwCur)
				dwStep = dwBytes - dwCur;

			hr = m_pMediaObjectInPlace->Process(dwStep, pAudio + dwCur, rtTime, DMO_INPLACE_NORMAL);

			rtTime += MsToRefTime(nPeriod);
		}
	}
	else  // FIXME: Support for IMediaObject-only DMOs goes here
	{
		hr = DSERR_GENERIC;
	}

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CSendEffect::CSendEffect
 *
 *	Description:
 *		Object constructor.
 *
 *	Arguments:
 *		DSEFFECTDESC& [in]: Effect description structure.
 *		CBuffer* [in]: Pointer to our source buffer.
 *
 *	Returns:
 *		(void)
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CSendEffect::CSendEffect"

CSendEffect::CSendEffect(DSEFFECTDESC& fxDescriptor, CBuffer* pSrcBuffer)
	: CEffect(fxDescriptor)
{
	//DPF_ENTER();
	//DPF_CONSTRUCT(CSendEffect);

	// Double check we really are a send effect
#ifdef ENABLE_I3DL2SOURCE
	ASSERT(fxDescriptor.guidDSFXClass == GUID_DSFX_SEND || fxDescriptor.guidDSFXClass == GUID_DSFX_STANDARD_I3DL2SOURCE);
#else
	ASSERT(fxDescriptor.guidDSFXClass == GUID_DSFX_SEND);
#endif

#ifndef _WIN64	// FIXME: temporary fix for Win64 build error
	// Figure out our destination buffer

	CBuffer* pDestBuffer = (CBuffer*)fxDescriptor.dwReserved1;
	//CHECK_WRITE_PTR(pDestBuffer);
	if( !IS_VALID_WRITE_PTR(pDestBuffer, sizeof( CBuffer ) ) )
	{
		m_pSrcBuffer = NULL;
		m_pDestBuffer = NULL;
		return;// DSERR_INVALIDPARAM;
	}

	// Set up the initial send configuration
	m_impDSFXSend.m_pObject = this;
	m_pMixFunction = pSrcBuffer->Format()->wBitsPerSample == 16 ? Mix16bit : Mix8bit;
	m_mixMode = pSrcBuffer->Format()->nChannels == pDestBuffer->Format()->nChannels ? OneToOne : MonoToStereo;
	m_pSrcBuffer = pSrcBuffer;
	m_pDestBuffer = pDestBuffer;
	m_lSendLevel = DSBVOLUME_MAX;
	m_dwAmpFactor = 0xffff;
#endif // _WIN64 

#ifdef ENABLE_I3DL2SOURCE
	ASSERT(m_pI3DL2SrcDMO == NULL);
	ASSERT(m_pI3DL2SrcDMOInPlace == NULL);
#endif

	//DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *	CSendEffect::~CSendEffect
 *
 *	Description:
 *		Object destructor.
 *
 *	Arguments:
 *		(void)
 *
 *	Returns:
 *		(void)
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CSendEffect::~CSendEffect"

CSendEffect::~CSendEffect()
{
	//DPF_ENTER();
	//DPF_DESTRUCT(CSendEffect);

	// Unregister in our destination buffer's list of senders
	// (as long as the buffer hasn't been released already)

	// Only needed for synchronizing DSound buffers
	//if (m_pDestBuffer)
	//	m_pDestBuffer->UnregisterSender(m_pSrcBuffer);

#ifdef ENABLE_I3DL2SOURCE
	// During shutdown, if the buffer hasn't been freed, these calls can
	// cause an access violation because the DMO DLL has been unloaded.
	try
	{
		if (m_pI3DL2SrcDMO)
			m_pI3DL2SrcDMO->Release();
		if (m_pI3DL2SrcDMOInPlace)
			m_pI3DL2SrcDMOInPlace->Release();
	}
	catch (...) {}
#endif

	//DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *	CSendEffect::Initialize
 *
 *	Description:
 *		Initializes the send effect object.
 *
 *	Arguments:
 *		DMO_MEDIA_TYPE* [in]: Wave format etc. information used to initialize
 *							  our contained I3DL2 source DMO, if we have one.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CSendEffect::Initialize"

HRESULT CSendEffect::Initialize(DMO_MEDIA_TYPE* pDmoMediaType)
{
	//DPF_ENTER();

	// First we need to detect any send loops
	HRESULT hr = m_pSrcBuffer->FindSendLoop(m_pDestBuffer);

#ifdef ENABLE_I3DL2SOURCE
	if (SUCCEEDED(hr) && m_fxDescriptor.guidDSFXClass == GUID_DSFX_STANDARD_I3DL2SOURCE)
	{
		hr = CoCreateInstance(GUID_DSFX_STANDARD_I3DL2SOURCE, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (void**)&m_pI3DL2SrcDMO);
		if (SUCCEEDED(hr))
		{
			//CHECK_COM_INTERFACE(m_pI3DL2SrcDMO);
			hr = m_pI3DL2SrcDMO->QueryInterface(IID_IMediaObjectInPlace, (void**)&m_pI3DL2SrcDMOInPlace);
			//if (SUCCEEDED(hr))
			//	  CHECK_COM_INTERFACE(m_pI3DL2SrcDMOInPlace);
			//else
			//	  DPF(DPFLVL_WARNING, "Failed to obtain the IMediaObjectInPlace interface on the STANDARD_I3DL2SOURCE effect");

			// Throw away the return code - we can live without IMediaObjectInPlace

			// FIXME: maybe this will change when we restrict I3DL2 to mono buffers
			// and/or change the way the I3DL2 DMO returns two output streams.

			// If we have a mono buffer, special-case the I3DL2 DMO to use stereo
			BOOL fTweakedMediaType = FALSE;
			LPWAVEFORMATEX pFormat = LPWAVEFORMATEX(pDmoMediaType->pbFormat);
			if (pFormat->nChannels == 1)
			{
				fTweakedMediaType = TRUE;
				pFormat->nChannels = 2;
				pFormat->nBlockAlign *= 2;
				pFormat->nAvgBytesPerSec *= 2;
			}

			// Finally set up the (possibly tweaked) media type on the DMO
			hr = m_pI3DL2SrcDMO->SetInputType(0, pDmoMediaType, 0);
			if (SUCCEEDED(hr))
				hr = m_pI3DL2SrcDMO->SetOutputType(0, pDmoMediaType, 0);

			// Undo changes to the wave format if necessary
			if (fTweakedMediaType)
			{
				pFormat->nChannels = 1;
				pFormat->nBlockAlign /= 2;
				pFormat->nAvgBytesPerSec /= 2;
			}

			if (SUCCEEDED(hr))
			{
				// OK, we now need to hook-up the reverb source to its environment.
				// There is a special interface on the I3DL2SourceDMO just for this.

				LPDIRECTSOUNDFXI3DL2SOURCEENV pSrcEnv = NULL;
				LPDIRECTSOUNDFXI3DL2REVERB pEnvReverb = NULL;

				HRESULT hrTemp = m_pI3DL2SrcDMO->QueryInterface(IID_IDirectSoundFXI3DL2SourceEnv, (void**)&pSrcEnv);
				if (SUCCEEDED(hrTemp))
				{
					//CHECK_COM_INTERFACE(pSrcEnv);
					hrTemp = m_pDestBuffer->GetObjectInPath(GUID_DSFX_STANDARD_I3DL2REVERB, 0, IID_IDirectSoundFXI3DL2Reverb, (void**)&pEnvReverb);
				}

				if (SUCCEEDED(hrTemp))
				{
					//CHECK_COM_INTERFACE(pEnvReverb);
					hrTemp = pSrcEnv->SetEnvironmentReverb(pEnvReverb);
				}

				//if (SUCCEEDED(hrTemp))
				//	  DPF(DPFLVL_INFO, "Connected the I3DL2 source to its environment successfully");

				// We're done with these interfaces.  The lifetime of the two buffers is managed
				// by DirectSound.	It will handle releasing the destination buffer.  We do not
				// hold a reference to it, and neither does the I3DL2 Source DMO.
				RELEASE(pSrcEnv);
				RELEASE(pEnvReverb);
			}
		}

		if (FAILED(hr))
		{
			RELEASE(m_pI3DL2SrcDMO);
			RELEASE(m_pI3DL2SrcDMOInPlace);
		}
	}
#endif

	// Register in our destination buffer's list of senders
	// Only needed for synchronizing DSound buffers
	//if (SUCCEEDED(hr))
	//	m_pDestBuffer->RegisterSender(m_pSrcBuffer);
	
	// Save the effect creation status for future reference
	m_fxStatus = SUCCEEDED(hr)				? DSFXR_UNALLOCATED :
				 hr == REGDB_E_CLASSNOTREG	? DSFXR_UNKNOWN 	:
				 DSFXR_FAILED;

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CSendEffect::Clone
 *
 *	Description:
 *		Creates a replica of this send effect object (or should do!).
 *
 *	Arguments:
 *		FIXME - the arguments will change when we do a 'true' Clone.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CSendEffect::Clone"

HRESULT CSendEffect::Clone(IMediaObject*, DMO_MEDIA_TYPE*)
{
	HRESULT hr = DS_OK;
	//DPF_ENTER();

	// FIXME: todo - some code currently in CEffectChain::Clone() should move here.

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CSendEffect::CImpDirectSoundFXSend::QueryInterface
 *
 *	Description:
 *		Helper QueryInterface() method for our IDirectSoundFXSend interface.
 *
 *	Arguments:
 *		REFIID [in]: IID of interface desired.
 *		VOID** [out]: Receives pointer to COM interface.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CSendEffect::CImpDirectSoundFXSend::QueryInterface"

HRESULT CSendEffect::CImpDirectSoundFXSend::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	HRESULT hr = E_NOINTERFACE;
	//DPF_ENTER();

	// This should really be handled by our glorious COM interface manager, but... ;-)

	//CHECK_WRITE_PTR(pDSBConfigObj);
	if( !IS_VALID_WRITE_PTR(ppvObj, sizeof( LPVOID ) ) )
	{
		hr = E_INVALIDARG;
	}
#ifdef ENABLE_I3DL2SOURCE
	else if (m_pObject->m_pI3DL2SrcDMO)  // We are an I3DL2 Source - pass call to the DMO
	{
		//DPF(DPFLVL_INFO, "Forwarding QueryInterface() call to the I3DL2 Source DMO");
		hr = m_pObject->m_pI3DL2SrcDMO->QueryInterface(riid, ppvObj);
	}
#endif
	else if (riid == IID_IUnknown)
	{
		*ppvObj = (IUnknown*)this;
		m_pObject->AddRef();
		hr = S_OK;
	}
	else if (riid == IID_IDirectSoundFXSend)
	{
		*ppvObj = (IDirectSoundFXSend*)this;
		m_pObject->AddRef();
		hr = S_OK;
	}

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CSendEffect::SetAllParameters
 *
 *	Description:
 *		Sets all our parameters - i.e., our send level.
 *
 *	Arguments:
 *		DSFXSend* [in]: Pointer to send parameter structure.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CSendEffect::SetAllParameters"

HRESULT CSendEffect::SetAllParameters(LPCDSFXSend pcDsFxSend)
{
	HRESULT hr;
	//DPF_ENTER();

	if( !IS_VALID_READ_PTR(pcDsFxSend, sizeof( DSFXSend ) ) )
	{
		//RPF(DPFLVL_ERROR, "Invalid pcDsFxSend pointer");
		hr = DSERR_INVALIDPARAM;
	}
	else if (pcDsFxSend->lSendLevel < DSBVOLUME_MIN || pcDsFxSend->lSendLevel > DSBVOLUME_MAX)
	{
		//RPF(DPFLVL_ERROR, "Volume out of bounds");
		hr = DSERR_INVALIDPARAM;
	}
	else
	{
		m_lSendLevel = pcDsFxSend->lSendLevel;
		m_dwAmpFactor = DBToAmpFactor(m_lSendLevel);
		hr = DS_OK;
	}

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CSendEffect::GetAllParameters
 *
 *	Description:
 *		Gets all our parameters - i.e., our send level.
 *
 *	Arguments:
 *		DSFXSend* [out]: Receives send parameter structure.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CSendEffect::GetAllParameters"

HRESULT CSendEffect::GetAllParameters(LPDSFXSend pDsFxSend)
{
	HRESULT hr;
	//DPF_ENTER();

	if( !IS_VALID_WRITE_PTR(pDsFxSend, sizeof( DSFXSend ) ) )
	{
		//RPF(DPFLVL_ERROR, "Invalid pDsFxSend pointer");
		hr = DSERR_INVALIDPARAM;
	}
	else
	{
		pDsFxSend->lSendLevel = m_lSendLevel;
		hr = DS_OK;
	}

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	CSendEffect::NotifyRelease
 *
 *	Description:
 *		Informs this send effect of the release of a MIXIN buffer.	If it
 *		happens to be our destination buffer, we record that it's gone.
 *
 *	Arguments:
 *		CBuffer* [in]: Departing MIXIN buffer.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CSendEffect::NotifyRelease"

void CSendEffect::NotifyRelease(CBuffer* pDsBuffer)
{
	//DPF_ENTER();

	// Check if it was our destination buffer that was released
	if (pDsBuffer == m_pDestBuffer)
	{
		m_pDestBuffer = NULL;
		m_fxStatus = DSFXR_FAILED;
	}

	//DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *	CSendEffect::Process
 *
 *	Description:
 *		Handles mixing data from our source buffer into its destination,
 *		and invokes effect processing on the I3DL2 source DMO if necessary.
 *
 *	Arguments:
 *		DWORD [in]: Number of audio bytes to process.
 *		BYTE* [in, out]: Pointer to start of audio buffer to process.
 *		REFERENCE_TIME [in]: Timestamp of first sample to be processed
 *		DWORD [in]: Offset of a wrapped audio region.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CSendEffect::Process"

HRESULT CSendEffect::Process(DWORD dwBytes, BYTE *pAudio, REFERENCE_TIME /*rtTime*/, DWORD /*dwSendOffset*/, LPWAVEFORMATEX /*ignored*/)
{
	//DWORD dwDestSliceBegin, dwDestSliceEnd;
	HRESULT hr = DS_OK;
	//DPF_ENTER();

	// Pointer to the audio data we'll actually send; this pointer
	// may be modified below if this is an I3DL2 send
	BYTE* pSendAudio = pAudio;

	// Check whether our source buffer is active.  If it isn't,
	// we must be pre-rolling FX, so we don't perform the send.
	BOOL fPlaying = m_pSrcBuffer->IsPlaying();

	// If the source buffer is active, check the destination too.
	// (Note: if it has been released, m_pDestBuffer will be NULL)
	BOOL fSending = fPlaying && m_pDestBuffer && m_pDestBuffer->IsPlaying();

#ifdef ENABLE_I3DL2SOURCE
	// OPTIMIZE: replace the CopyMemorys below with BYTE, WORD or DWORD
	// assignments, since we only support nBlockSizes of 1, 2 or 4...
	// But hopefully this code can disappear altogether (see bug 40236).

	// First call the I3DL2 DMO if this is an I3DL2 Source effect
	if (m_pI3DL2SrcDMOInPlace)
	{
		// If we're processing a mono buffer, an ugly hack is required; copying
		// the data into the L and R channels of a temporary stereo buffer, so
		// we have room for the two output streams returned by the I3DL2 DMO.

		WORD nBlockSize = m_pSrcBuffer->Format()->nBlockAlign;

		if (m_pSrcBuffer->Format()->nChannels == 1)
		{
			hr = MEMALLOC_A_HR(pSendAudio, BYTE, 2*dwBytes);
			if (SUCCEEDED(hr))
			{
				for (DWORD i=0; i<dwBytes; i += nBlockSize)
				{
					CopyMemory(pSendAudio + 2*i,			  pAudio + i, nBlockSize); // L channel
					CopyMemory(pSendAudio + 2*i + nBlockSize, pAudio + i, nBlockSize); // R channel
				}
				hr = m_pI3DL2SrcDMOInPlace->Process(dwBytes, pSendAudio, rtTime, DMO_INPLACE_NORMAL);
			}

			if (SUCCEEDED(hr))
			{
				// Now we extract the two output streams from the data returned;
				// the direct path goes back to pAudio, and the reflected path
				// goes to the first half of pSendAudio.
				for (DWORD i=0; i<dwBytes; i += nBlockSize)
				{
					CopyMemory(pAudio + i, pSendAudio + 2*i, nBlockSize);
					if (fSending)  // Needn't preserve the reflected audio if we aren't sending it anywhere
						CopyMemory(pSendAudio + i, pSendAudio + 2*i + nBlockSize, nBlockSize);
				}
			}
		}
		else // Processing a stereo buffer
		{
			hr = m_pI3DL2SrcDMOInPlace->Process(dwBytes, pAudio, rtTime, DMO_INPLACE_NORMAL);

			if (SUCCEEDED(hr))
				hr = MEMALLOC_A_HR(pSendAudio, BYTE, dwBytes);

			if (SUCCEEDED(hr))
			{
				// Extract the output streams and stereoize them at the same time
				for (DWORD i=0; i<dwBytes; i += nBlockSize)
				{
					if (fSending)  // Needn't preserve the reflected audio if we aren't sending it anywhere
					{
						// Copy the R channel from pAudio into both channels of pSendAudio
						CopyMemory(pSendAudio + i,				  pAudio + i + nBlockSize/2, nBlockSize/2);
						CopyMemory(pSendAudio + i + nBlockSize/2, pAudio + i + nBlockSize/2, nBlockSize/2);
					}
					// Copy pAudio's L channel onto its R channel
					CopyMemory(pAudio + i + nBlockSize/2, pAudio + i, nBlockSize/2);
				}
			}
		}
	}
#endif

	// Now we handle the actual send
	if (SUCCEEDED(hr) && fSending)
	{
		m_pMixFunction(pSendAudio, (BYTE*)m_pDestBuffer->m_pExtraBuffer, dwBytes, m_dwAmpFactor, m_mixMode);
	}

#ifdef ENABLE_I3DL2SOURCE
	if (pSendAudio != pAudio)
		MEMFREE(pSendAudio);
#endif

	//DPF_LEAVE_HRESULT(hr);
	return hr;
}


/***************************************************************************
 *
 *	Mix8bit
 *
 *	Description:
 *		Primitive 8-bit mixing function.  Attenuates source audio by a
 *		given factor, adds it to the destination audio, and clips.
 *
 *	Arguments:
 *		VOID* [in]: Pointer to source audio buffer.
 *		VOID* [in, out]: Pointer to destination audio buffer.
 *		DWORD [in]: Number of bytes to mix.
 *		DWORD [in]: Amplification factor (in 1/65536 units).
 *		MIXMODE: Whether to double the channel data or not.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "Mix8bit"

static void Mix8bit(PVOID pSrc, PVOID pDest, DWORD dwBytes, DWORD dwAmpFactor, MIXMODE mixMode)
{
	unsigned char* pSampSrc = (unsigned char*)pSrc;
	unsigned char* pSampDest = (unsigned char*)pDest;
	//DPF_ENTER();

	while (dwBytes--)
	{
		INT sample = (INT(*pSampSrc++) - 0x80) * INT(dwAmpFactor) / 0xffff;
		INT mixedSample = sample + *pSampDest;
		if (mixedSample > 0xff) mixedSample = 0xff;
		else if (mixedSample < 0) mixedSample = 0;
		*pSampDest++ = unsigned char(mixedSample);
		if (mixMode == MonoToStereo)
		{
			INT mixedSample = sample + *pSampDest;
			if (mixedSample > 0xff) mixedSample = 0xff;
			else if (mixedSample < 0) mixedSample = 0;
			*pSampDest++ = unsigned char(mixedSample);
		}
	}

	//DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *	Mix16bit
 *
 *	Description:
 *		Primitive 16-bit mixing function.  Attenuates source audio by a
 *		given factor, adds it to the destination audio, and clips.
 *
 *	Arguments:
 *		VOID* [in]: Pointer to source audio buffer.
 *		VOID* [in, out]: Pointer to destination audio buffer.
 *		DWORD [in]: Number of bytes to mix.
 *		DWORD [in]: Amplification factor (in 1/65536 units).
 *		MIXMODE: Whether to double the channel data or not.
 *
 *	Returns:
 *		HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "Mix16bit"

static void Mix16bit(PVOID pSrc, PVOID pDest, DWORD dwBytes, DWORD dwAmpFactor, MIXMODE mixMode)
{
	DWORD dwSamples = dwBytes / 2;
	short* pSampSrc = (short*)pSrc;
	short* pSampDest = (short*)pDest;
	//DPF_ENTER();

	while (dwSamples--)
	{
		INT sample = INT(*pSampSrc++) * INT(dwAmpFactor) / 0xffff;
		INT mixedSample = sample + *pSampDest;
		if (mixedSample > 32767) mixedSample = 32767;
		else if (mixedSample < -32768) mixedSample = -32768;
		*pSampDest++ = short(mixedSample);
		if (mixMode == MonoToStereo)
		{
			INT mixedSample = sample + *pSampDest;
			if (mixedSample > 32767) mixedSample = 32767;
			else if (mixedSample < -32768) mixedSample = -32768;
			*pSampDest++ = short(mixedSample);
		}
	}

	//DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *	IsValidEffectDesc
 *
 *	Description:
 *		Determines if the given effect descriptor structure is valid for
 *		the given secondary buffer.
 *
 *	Arguments:
 *		DSEFFECTDESC* [in]: Effect descriptor to be validated.
 *		CBuffer* [in]: Host buffer for the effect.
 *
 *	Returns:
 *		BOOL: TRUE if the descriptor is valid.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "IsValidEffectDesc"

BOOL IsValidEffectDesc(LPCDSEFFECTDESC pEffectDesc, CBuffer* pDsBuffer)
{
	BOOL fValid = TRUE;
	//DPF_ENTER();

	if (pEffectDesc->dwSize != sizeof(DSEFFECTDESC))
	{
		//RPF(DPFLVL_ERROR, "Invalid DSEFFECTDESC structure size");
		fValid = FALSE;
	}
	else if (pEffectDesc->dwReserved2 != 0)
	{
		//RPF(DPFLVL_ERROR, "Reserved fields in the DSEFFECTDESC structure must be 0");
		fValid = FALSE;
	}

	//if (fValid)
	//{
	//	fValid = IsValidFxFlags(pEffectDesc->dwFlags);
	//}

	if (fValid)
	{
		BOOL fSendEffect = pEffectDesc->guidDSFXClass == GUID_DSFX_SEND
#ifdef ENABLE_I3DL2SOURCE
						   || pEffectDesc->guidDSFXClass == GUID_DSFX_STANDARD_I3DL2SOURCE
#endif
						   ;
		if (!fSendEffect && pEffectDesc->dwReserved1)
		{
			//RPF(DPFLVL_ERROR, "lpSendBuffer should only be specified with GUID_DSFX_SEND"
#ifdef ENABLE_I3DL2SOURCE
			//				  " or GUID_DSFX_STANDARD_I3DL2SOURCE"
#endif
			//);
			fValid = FALSE;
		}
		else if (fSendEffect)
		{
			//CImpDirectSoundBuffer<CBuffer>* pImpBuffer =
			//	(CImpDirectSoundBuffer<CBuffer>*) (pEffectDesc->dwReserved1);
			CBuffer *pBuffer = (CBuffer*)pEffectDesc->dwReserved1;
			LPWAVEFORMATEX pSrcWfx, pDstWfx;

			if (!pBuffer)
			{
				//RPF(DPFLVL_ERROR, "lpSendBuffer must be specified for GUID_DSFX_SEND"
#ifdef ENABLE_I3DL2SOURCE
				//				  " and GUID_DSFX_STANDARD_I3DL2SOURCE"
#endif
				//);
				fValid = FALSE;
			}
			//else if (!IS_VALID_IDIRECTSOUNDBUFFER(pImpBuffer))
			else if (!IS_VALID_WRITE_PTR(pBuffer, sizeof( CBuffer ) ) )
			{
				//RPF(DPFLVL_ERROR, "lpSendBuffer points to an invalid DirectSound buffer");
				fValid = FALSE;
			}
			else if (!(pBuffer->GetBufferType() & DSBCAPS_MIXIN))
			{
				//RPF(DPFLVL_ERROR, "lpSendBuffer must point to a DSBCAPS_MIXIN buffer");
				fValid = FALSE;
			}
			/*
			else if (pBuffer->GetDirectSound() != pDsBuffer->GetDirectSound())
			{
				//RPF(DPFLVL_ERROR, "Can't send to a buffer on a different DirectSound object");
				fValid = FALSE;
			}
			*/
			else if ((pSrcWfx = pDsBuffer->Format())->nSamplesPerSec !=
					 (pDstWfx = pBuffer->Format())->nSamplesPerSec)
			{
				//RPF(DPFLVL_ERROR, "The buffer sent to must have the same nSamplesPerSec as the sender");
				fValid = FALSE;
			}
			else if (pSrcWfx->wBitsPerSample != pDstWfx->wBitsPerSample)
			{
				//RPF(DPFLVL_ERROR, "The buffer sent to must have the same wBitsPerSample as the sender");
				fValid = FALSE;
			}
			else if ((pSrcWfx->nChannels > 2 || pDstWfx->nChannels > 2) && (pSrcWfx->nChannels != pDstWfx->nChannels))
			{
				//RPF(DPFLVL_ERROR, "If either the send buffer or the receive buffer has more than two channels, the number of channels must match");
				fValid = FALSE;
			}
			else if (pSrcWfx->nChannels == 2 && pDstWfx->nChannels == 1)
			{
				//RPF(DPFLVL_ERROR, "You can't send from a stereo buffer to a mono buffer");
				fValid = FALSE;
			}
			else if (pEffectDesc->dwFlags & (DSFX_LOCSOFTWARE | DSFX_LOCHARDWARE))
			{
				//RPF(DPFLVL_ERROR, "Location flags should not be specified for GUID_DSFX_SEND"
#ifdef ENABLE_I3DL2SOURCE
				//				  " or GUID_DSFX_STANDARD_I3DL2SOURCE"
#endif
				//);
				fValid = FALSE;
			}
		}
	}

	//DPF_LEAVE(fValid);
	return fValid;
}
