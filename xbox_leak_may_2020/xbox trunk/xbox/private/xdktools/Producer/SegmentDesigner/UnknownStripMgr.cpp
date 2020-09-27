// UnknownStripMgr.cpp : implementation file
//

#include "stdafx.h"
#include "UnknownStripMgr.h"
#include "DLLJazzDataObject.h"
#include "SegmentDesigner.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include <RiffStrm.h>
#include <mmreg.h>
#include <dmusicf.h>
#include "SegmentIO.h"
#include <SegmentGuids.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr constructor/destructor 

CUnknownStripMgr::CUnknownStripMgr()
{
	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pDMProdFramework = NULL;
	m_pISegmentNode = NULL;
	m_pPropertyPage = NULL;
	m_pPropPageMgr = NULL;
	m_pCopyDataObject = NULL;
	m_fPropPageActive = FALSE;
	m_dwRef = 0;
	m_pIDMTrack = NULL;
	m_ckid = NULL;
	m_fccType = NULL;
	m_pIStreamCopy = NULL;
	m_uliStreamSize.QuadPart = 0;
	AddRef();

	// Initially we don't need to be saved
	m_fDirty = FALSE;

	// By default, belong to Group 1 only
	m_dwGroupBits = 1;

	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT;
	m_dwProducerOnlyFlags = 0;

	// Create a strip
	m_pUnknownStrip = new CUnknownStrip(this);
	ASSERT( m_pUnknownStrip );
}

CUnknownStripMgr::~CUnknownStripMgr()
{
	// Clean up our references
	if( m_pUnknownStrip )
	{
		m_pUnknownStrip->Release();
		m_pUnknownStrip = NULL;
	}
	if( m_pDMProdFramework )
	{
		m_pDMProdFramework->Release();
		m_pDMProdFramework = NULL;
	}

	// No Release!
	/*
	if( m_pDMProdSegmentNode )
	{
		m_pDMProdSegmentNode->Release();
		m_pDMProdSegmentNode = NULL;
	}
	*/

	ASSERT( m_pTimeline == NULL );
	ASSERT( m_pPropertyPage == NULL );

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->Release();
	}
	if( m_pIDMTrack )
	{
		m_pIDMTrack->Release();
	}
	if( m_pIStreamCopy )
	{
		m_pIStreamCopy->Release();
	}

	// If we have an object on the clipboard, make sure it doesn't have any references
	// back to us.
	if( m_pCopyDataObject )
	{
		if( S_OK == OleIsCurrentClipboard( m_pCopyDataObject ))
		{
			OleFlushClipboard();
		}
		m_pCopyDataObject->Release();
		m_pCopyDataObject = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr IUnknown implementation

HRESULT CUnknownStripMgr::QueryInterface( REFIID riid, void** ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdStripMgr)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdStripMgr *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IPersist) )
    {
        AddRef();
        *ppvObj = (IPersist *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IPersistStream) )
    {
        AddRef();
        *ppvObj = (IPersistStream *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageObject *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CUnknownStripMgr::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CUnknownStripMgr::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

    if( m_dwRef == 0 )
    {
		TRACE( "SEGMENT: CUnknownStripMgr destroyed!\n" );
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr IDMUSProdStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::GetParam

HRESULT STDMETHODCALLTYPE CUnknownStripMgr::GetParam(
		/* [in] */	REFGUID 	guidType,
		/* [in] */	MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME* pmtNext,
		/* [out] */ void*		pData)
{
	UNREFERENCED_PARAMETER(guidType);
	UNREFERENCED_PARAMETER(mtTime);
	UNREFERENCED_PARAMETER(pmtNext);
	UNREFERENCED_PARAMETER(pData);
	return E_NOTIMPL;
	// In an implementation you would want to check guidType against the data type
	// GUIDs that you support.  If it is a known type, then return the value.  Otherwise
	// return E_INVALIDARG.
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::SetParam

HRESULT STDMETHODCALLTYPE CUnknownStripMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	UNREFERENCED_PARAMETER(mtTime);
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		m_pISegmentNode = reinterpret_cast<IDMUSProdNode*>(pData);
		// No addref!
		//m_pISegmentNode->AddRef();
		return S_OK;
	}

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::IsParamSupported

HRESULT STDMETHODCALLTYPE CUnknownStripMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CUnknownStripMgr::OnUpdate(
		/* [out] */ REFGUID rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(rguidType);
	UNREFERENCED_PARAMETER(dwGroupBits);
	UNREFERENCED_PARAMETER(pData);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CUnknownStripMgr::GetStripMgrProperty(
		/* [in] */ STRIPMGRPROPERTY stripMgrProperty,
		/* [out] */ VARIANT*	pVariant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pVariant );
	if( !pVariant )
	{
		return E_POINTER;
	}

	switch( stripMgrProperty )
	{
	case SMP_ITIMELINECTL:
		pVariant->vt = VT_UNKNOWN;
		if( m_pTimeline )
		{
			V_UNKNOWN( pVariant ) = m_pTimeline;
			V_UNKNOWN( pVariant )->AddRef();
			return S_OK;
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;

	case SMP_IDIRECTMUSICTRACK:
		pVariant->vt = VT_UNKNOWN;
		if( m_pIDMTrack )
		{
			return m_pIDMTrack->QueryInterface( IID_IUnknown, (void**)&V_UNKNOWN( pVariant ) );
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;

	case SMP_IDMUSPRODFRAMEWORK:
		pVariant->vt = VT_UNKNOWN;
		if( m_pDMProdFramework )
		{
			return m_pDMProdFramework->QueryInterface( IID_IUnknown, (void**)&V_UNKNOWN( pVariant ) );
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;

	case SMP_DMUSIOTRACKHEADER:
		if( pVariant->vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_HEADER *pioTrackHeader = static_cast<DMUS_IO_TRACK_HEADER *>(V_BYREF( pVariant ));
			if( pioTrackHeader == NULL )
			{
				return E_POINTER;
			}

			pioTrackHeader->guidClassID = GUID_AllZeros;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = m_ckid;
			pioTrackHeader->fccType = m_fccType;
		}
		break;

	case SMP_DMUSIOTRACKEXTRASHEADER:
		if( pVariant->vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_EXTRAS_HEADER *pioTrackExtrasHeader = static_cast<DMUS_IO_TRACK_EXTRAS_HEADER *>(V_BYREF( pVariant ));
			if( pioTrackExtrasHeader == NULL )
			{
				return E_POINTER;
			}

			pioTrackExtrasHeader->dwFlags = m_dwTrackExtrasFlags;
		}
		break;

	case SMP_DMUSIOTRACKEXTRASHEADER_MASK:
		pVariant->vt = VT_I4;
		V_I4(pVariant) = ALLEXTRAS_FLAGS;
		break;

	case SMP_PRODUCERONLY_FLAGS:
		if( pVariant->vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			IOProducerOnlyChunk *pioProducerOnlyChunk = static_cast<IOProducerOnlyChunk *>(V_BYREF( pVariant ));
			if( pioProducerOnlyChunk == NULL )
			{
				return E_POINTER;
			}

			pioProducerOnlyChunk->dwProducerOnlyFlags = m_dwProducerOnlyFlags;
		}
		break;

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CUnknownStripMgr::SetStripMgrProperty(
		/* [in] */ STRIPMGRPROPERTY stripMgrProperty,
		/* [in] */ VARIANT		variant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	switch( stripMgrProperty )
	{
	case SMP_ITIMELINECTL:
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}
		// If we were previously attached to a timeline, remove our PropPageObject and strip from it
		if( m_pTimeline )
		{
			m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);
			ASSERT( m_pUnknownStrip );
			m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pUnknownStrip );
			m_pTimeline->Release();
			m_pTimeline = NULL;
		}

		if( V_UNKNOWN( &variant ) )
		{
			if( FAILED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pTimeline )))
			{
				return E_FAIL;
			}
			else
			{
				// Only support handling one strip at a time
				ASSERT( m_pUnknownStrip );

				// Add our strip to the timeline
				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pUnknownStrip, GUID_AllZeros, m_dwGroupBits, 0 );
			}
		}
		break;

	case SMP_IDIRECTMUSICTRACK:
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}
		if( m_pIDMTrack )
		{
			m_pIDMTrack->Release();
		}
		if( V_UNKNOWN( &variant ) )
		{
			if( FAILED( V_UNKNOWN( &variant )->QueryInterface( IID_IDirectMusicTrack, (void**)&m_pIDMTrack ) ) )
			{
				return E_INVALIDARG;
			}

			if( m_pIStreamCopy )
			{
				LARGE_INTEGER	liTemp;
				liTemp.QuadPart = 0;
				if( FAILED( m_pIStreamCopy->Seek(liTemp, STREAM_SEEK_SET, NULL) ) ) //seek to beginning
				{
					return S_FALSE;
				}

				IPersistStream* pIPersistStream;
				if( FAILED( m_pIDMTrack->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
				{
					return S_FALSE;
				}

				pIPersistStream->Load( m_pIStreamCopy );

				if( m_pISegmentNode )
				{
					IDirectMusicSegment *pSegment;
					if( SUCCEEDED( m_pISegmentNode->GetObject(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pSegment ) ) )
					{
						m_pIDMTrack->Init( pSegment );
						pSegment->Release();
					}
				}

				pIPersistStream->Release();
			}
		}
		else
		{
			m_pIDMTrack = NULL;
		}
		break;

	case SMP_IDMUSPRODFRAMEWORK:
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}
		if( m_pDMProdFramework )
		{
			m_pDMProdFramework->Release();
			m_pDMProdFramework = NULL;
		}
		if( V_UNKNOWN( &variant ) )
		{
			return V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pDMProdFramework);
		}
		break;

	case SMP_DMUSIOTRACKHEADER:
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_HEADER *pioTrackHeader = static_cast<DMUS_IO_TRACK_HEADER *>(V_BYREF( &variant ));
			if( pioTrackHeader == NULL )
			{
				return E_POINTER;
			}
			m_dwGroupBits = pioTrackHeader->dwGroup;
			m_ckid = pioTrackHeader->ckid;
			m_fccType = pioTrackHeader->fccType;
			if( m_pUnknownStrip )
			{
				m_pUnknownStrip->UpdateName();
			}
		}
		break;

	case SMP_DMUSIOTRACKEXTRASHEADER:
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_EXTRAS_HEADER *pioTrackExtrasHeader = static_cast<DMUS_IO_TRACK_EXTRAS_HEADER *>(V_BYREF( &variant ));
			if( pioTrackExtrasHeader == NULL )
			{
				return E_POINTER;
			}

			m_dwTrackExtrasFlags = pioTrackExtrasHeader->dwFlags;
		}
		break;

	case SMP_PRODUCERONLY_FLAGS:
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			IOProducerOnlyChunk *pioProducerOnlyChunk = static_cast<IOProducerOnlyChunk *>(V_BYREF( &variant ));
			if( pioProducerOnlyChunk == NULL )
			{
				return E_POINTER;
			}

			m_dwProducerOnlyFlags = pioProducerOnlyChunk->dwProducerOnlyFlags;
		}
		break;

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::GetClassID

HRESULT CUnknownStripMgr::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pClsId == NULL )
	{
		return E_POINTER;
	}

	// return our CLSID
	memcpy( pClsId, &CLSID_UnknownStripMgr, sizeof( CLSID ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::IsDirty

HRESULT CUnknownStripMgr::IsDirty()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( m_fDirty )
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::Load

HRESULT CUnknownStripMgr::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	IStream* pIStreamCopy = NULL;
	IPersistStream* pIPersistStream = NULL;

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pDMProdFramework )
	{
		if( FAILED( m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStreamCopy) ) )
		{
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		if( FAILED( CreateStreamOnHGlobal( NULL, TRUE, &pIStreamCopy ) ) )
		{
			return E_OUTOFMEMORY;
		}
	}

	HRESULT hr;
	STATSTG StatStg;
	if( FAILED( pIStream->Stat( &StatStg, STATFLAG_NONAME ) ) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	m_uliStreamSize = StatStg.cbSize;
	if( FAILED( pIStream->CopyTo( pIStreamCopy, StatStg.cbSize, NULL, NULL ) ) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	if( m_pIDMTrack == NULL )
	{
		hr = S_OK;
		goto ON_ERROR;
	}

	LARGE_INTEGER	liTemp;
	liTemp.QuadPart = 0;
	pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL); //seek to beginning

	hr = m_pIDMTrack->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
	if( FAILED( hr ) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	hr = pIPersistStream->Load( pIStream );
	if( FAILED( hr ) )
	{
		// Fix 44757
		hr = S_FALSE;
		//hr = E_FAIL;
		//goto ON_ERROR;
	}

	if( m_pISegmentNode )
	{
		IDirectMusicSegment *pSegment;
		if( SUCCEEDED( m_pISegmentNode->GetObject(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pSegment ) ) )
		{
			m_pIDMTrack->Init( pSegment );
			pSegment->Release();
		}
	}

ON_ERROR:
	if( SUCCEEDED( hr ) && (pIStreamCopy != NULL) )
	{
		m_pIStreamCopy = pIStreamCopy;
		m_pIStreamCopy->AddRef();
	}
	if( pIStreamCopy )
	{
		pIStreamCopy->Release();
	}
	if( pIPersistStream )
	{
		pIPersistStream->Release();
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::Save

HRESULT CUnknownStripMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pIStreamCopy )
	{
		LARGE_INTEGER	liTemp;
		liTemp.QuadPart = 0;
		if( FAILED( m_pIStreamCopy->Seek(liTemp, STREAM_SEEK_SET, NULL) ) ) //seek to beginning
		{
			return E_FAIL;
		}
		if( fClearDirty )
		{
			m_fDirty = FALSE;
		}
		return m_pIStreamCopy->CopyTo( pIStream, m_uliStreamSize, NULL, NULL );
	}
	if( fClearDirty )
	{
		m_fDirty = FALSE;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::GetSizeMax

HRESULT CUnknownStripMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	UNREFERENCED_PARAMETER(pcbSize);
	// Compute size of stream needed to persist ourself into.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::GetData

// This method is called by CGroupBitsPropPageMgr to get data to send to the
// Group bits property page.
HRESULT STDMETHODCALLTYPE CUnknownStripMgr::GetData( /* [retval][out] */ void **ppData)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (ppData == NULL) || (*ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	// Check which property page is requesting the data
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(*ppData);
	switch( *pdwIndex )
	{
	case GROUPBITSPPG_INDEX:
	{
		// Copy our groupbits to the location pointed to by ppData
		PPGTrackParams *pPPGTrackParams = static_cast<PPGTrackParams *>(*ppData);
		pPPGTrackParams->dwGroupBits = m_dwGroupBits;
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		// Copy our track setting to the location pointed to by ppData
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = ALLEXTRAS_FLAGS;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
		break;
	}
	default:
		ASSERT(FALSE);
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::SetData

// This method is called by CUnknownStripPropPageMgr in response to user actions
// in the Group bits Property page.
HRESULT STDMETHODCALLTYPE CUnknownStripMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pData == NULL )
	{
		return E_INVALIDARG;
	}

	// Check which property page is setting the data
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
	case GROUPBITSPPG_INDEX:
	{
		PPGTrackParams *pPPGTrackParams = reinterpret_cast<PPGTrackParams *>(pData);

		// Update our group bits setting, if necessary
		if( pPPGTrackParams->dwGroupBits != m_dwGroupBits )
		{
			m_dwGroupBits = pPPGTrackParams->dwGroupBits;
			m_fDirty = TRUE;

			if( m_pUnknownStrip )
			{
				m_pUnknownStrip->UpdateName();
			}

			// Notify our editor that we've changed
			//m_nLastEdit = IDS_UNDO_TRACK_GROUP;
			m_pTimeline->OnDataChanged( (IDMUSProdStripMgr*) this );
		}
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

		// Update our track extras flags, if necessary
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_dwTrackExtrasFlags )
		{
			m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;

			// Notify our editor that we've changed
			//m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pTimeline->OnDataChanged( (IDMUSProdStripMgr*) this );
		}
		// Update our Producer-specific flags, if necessary
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_dwProducerOnlyFlags )
		{
			m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;

			// Notify our editor that we've changed
			//m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pTimeline->OnDataChanged( (IDMUSProdStripMgr*) this );
		}
		break;
	}
	default:
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CUnknownStripMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
	pIFramework = (IDMUSProdFramework*) V_UNKNOWN(&var);
	if (pIFramework == NULL)
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the property sheet
	IDMUSProdPropSheet* pIPropSheet = NULL;
	pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
	pIFramework->Release();
	if( pIPropSheet == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	//  If the property sheet is hidden, exit
	if( pIPropSheet->IsShowing() != S_OK )
	{
		pIPropSheet->Release();
		return S_OK;
	}

	// If our property page is already displayed, exit
	if(m_fPropPageActive)
	{
		ASSERT( m_pPropPageMgr != NULL );
		pIPropSheet->Release();
		return S_OK;
	}

	// Get a reference to our property page manager
	HRESULT hr = S_OK;
	if( m_pPropPageMgr == NULL )
	{
		CGroupBitsPropPageMgr* pPPM = new CGroupBitsPropPageMgr;
		if( NULL == pPPM )
		{
			hr = E_OUTOFMEMORY;
			goto EXIT;
		}
		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		m_pPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
		if( FAILED(hr) )
		{
			goto EXIT;
		}
	}

	// Set the property page to refer to the Piano Roll property page.
	short nActiveTab;
	nActiveTab = CGroupBitsPropPageMgr::sm_nActiveTab;
	m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	// release our reference to the property sheet
	pIPropSheet->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CUnknownStripMgr::OnRemoveFromPageManager( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->SetObject(NULL);
	}
	m_fPropPageActive = FALSE;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr::OnDataChanged

HRESULT STDMETHODCALLTYPE CUnknownStripMgr::OnDataChanged( void)
{
	ASSERT( m_pTimeline );
	if ( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Let our hosting editor know about the change
	m_pTimeline->OnDataChanged( (IDMUSProdStripMgr*)this );

	return S_OK;
}
