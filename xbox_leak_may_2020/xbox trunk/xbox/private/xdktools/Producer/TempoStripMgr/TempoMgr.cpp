// TempoMgr.cpp : implementation file
//

/*--------------
@doc TEMPOSAMPLE
--------------*/

#include "stdafx.h"
#include "TempoIO.h"
#include "TempoMgr.h"
#include "PropPageMgr.h"
#include <dmusicf.h>
#include <RiffStrm.h>
#include "SegmentGuids.h"
#include "SegmentIO.h"
#include <StyleDesigner.h>
#include <Conductor.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr constructor/destructor 

CTempoMgr::CTempoMgr()
{
	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pDMProdFramework = NULL;
	m_pPropPageMgr = NULL;
	m_pCopyDataObject = NULL;
	m_pTempoStrip = NULL;
	m_pIDMTrack = NULL;
	m_pDMPerformance = NULL;
	m_posLastGetParam = NULL;

	// Initially the segment isn't playing
	m_pSegmentState = NULL;
	m_mtLoopEnd = 0;
	m_mtLoopStart = 0;
	m_mtStartTime = 0;
	m_mtStartPoint = 0;
	m_dwNumLoops = 0;

	// Initially we don't need to be saved
	m_fDirty = FALSE;

	// By default, belong to all groups
	m_dwGroupBits = 0xFFFFFFFF;
	m_dwIndex = 0;

	// Initialize the track extras flags
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
	m_dwProducerOnlyFlags = 0;

	// Initially, have a tempo of 120 at time 0
	CTempoItem *pTempo = new CTempoItem();
	pTempo->m_dblTempo = 120.0;
	pTempo->m_mtTime = 0;
	pTempo->m_strText.Format("%.2f", 120.0);
	m_lstTempos.AddHead( pTempo );

	// Create a TempoStrip
	m_pTempoStrip = new CTempoStrip(this);
	ASSERT( m_pTempoStrip );
}

CTempoMgr::~CTempoMgr()
{
	// Clean up our references
	if( m_pDMProdFramework )
	{
		m_pDMProdFramework->Release();
		m_pDMProdFramework = NULL;
	}

	ASSERT( m_pTimeline == NULL );

	if( m_pDMPerformance )
	{
		m_pDMPerformance->Release();
		m_pDMPerformance = NULL;
	}
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->Release();
		m_pPropPageMgr = NULL;
	}
	if( m_pIDMTrack )
	{
		m_pIDMTrack->Release();
		m_pIDMTrack = NULL;
	}
	if( m_pTempoStrip )
	{
		m_pTempoStrip->Release();
		m_pTempoStrip = NULL;
	}
	if( m_pSegmentState )
	{
		m_pSegmentState->Release();
		m_pSegmentState = NULL;
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

	// Delete all the tempos in m_lstTempos
	EmptyTempoList();
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr ITempoMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::IsMeasureBeatOpen
/*
HRESULT STDMETHODCALLTYPE CTempoMgr::IsMeasureBeatOpen( long lMeasure, long lBeat )
{
	HRESULT hr = S_FALSE;
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		if( pTempoItem->m_lMeasure > lMeasure )
		{
			pos = NULL;
		}
		else if( pTempoItem->m_lMeasure == lMeasure &&
				 pTempoItem->m_lBeat == lBeat )
		{
			hr = S_OK;
			pos = NULL;
		}
	}
	return hr;
}
*/


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr IDMUSProdStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::GetParam

HRESULT STDMETHODCALLTYPE CTempoMgr::GetParam(
		/* [in] */	REFGUID 	rguidType,
		/* [in] */	MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME* pmtNext,
		/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsEqualGUID( rguidType, GUID_TempoParam ) )
	{
		if( pData == NULL )
		{
			return E_POINTER;
		}

		CTempoItem *pTempoItemNext, *pTempoItemCur;
		pTempoItemCur = GetTempoForTime( mtTime, &pTempoItemNext );

		DMUS_TEMPO_PARAM *pDMTempoItem = (DMUS_TEMPO_PARAM *)pData;
		if( pmtNext )
		{
			if( pTempoItemNext )
			{
				*pmtNext = pTempoItemNext->m_mtTime - mtTime;
			}
			else
			{
				*pmtNext = 0;
			}
		}

		if( pTempoItemCur )
		{
			pDMTempoItem->dblTempo = pTempoItemCur->m_dblTempo;
			pDMTempoItem->mtTime = pTempoItemCur->m_mtTime;
			return S_OK;
		}
		else
		{
			pDMTempoItem->dblTempo = 120.0;
			pDMTempoItem->mtTime = 0;
			return S_FALSE;
		}
	}
	else if( ::IsEqualGUID( rguidType, GUID_Segment_Undo_BSTR ) )
	{
		BSTR bstr;
		CString str;
		str.LoadString(m_pTempoStrip->m_nLastEdit);
		try
		{
			bstr = str.AllocSysString();
		}
		catch(CMemoryException*)
		{
			return E_OUTOFMEMORY;
		}
		*(BSTR*)pData = bstr;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::SetParam

HRESULT STDMETHODCALLTYPE CTempoMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	UNREFERENCED_PARAMETER( guidType );
	UNREFERENCED_PARAMETER( mtTime );
	UNREFERENCED_PARAMETER( pData );
	return E_NOTIMPL;
	// In an implementation you would want to check guidType against the data type GUIDs
	// that you support.  If it is a known type, copy the value to local storage.  Otherwise
	// return E_INVALIDARG.
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::IsParamSupported

HRESULT STDMETHODCALLTYPE CTempoMgr::IsParamSupported(
		/* [in] */ REFGUID		rguidType)
{
	if( ::IsEqualGUID( rguidType, GUID_TempoParam )
	||  ::IsEqualGUID( rguidType, GUID_Segment_Undo_BSTR ))
	{
		return S_OK;
	}
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CTempoMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	UNREFERENCED_PARAMETER( dwGroupBits );
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// TimeSig change
	if( ::IsEqualGUID( rguidType, GUID_TimeSignature ) )
	{
		// Update the m_mtTime value of all tempo events
		if( RecomputeTempoTimes() )
		{
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->RefreshData();
			}
			OnDataChanged();
		}

		// Redraw our strip
		m_pTimeline->StripInvalidateRect( m_pTempoStrip, NULL, TRUE );
		return S_OK;
	}

	// All Tracks Added
	if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		// Update the Measure/beat value of all tempo events
		RecomputeTempoMeasureBeats();

		// Redraw our strip
		m_pTimeline->StripInvalidateRect( m_pTempoStrip, NULL, TRUE );
		//OnDataChanged();
		return S_OK;
	}

	// New Style just added to Style strip
	if( ::IsEqualGUID( rguidType, GUID_Segment_NewStyleSelected ) )
	{
		MUSIC_TIME* pmtTime = (MUSIC_TIME *)pData;
		IDMUSProdNode* pIStyleNode;
		IDMUSProdStyleInfo* pIStyleInfo;
		double dblTempo;

		// Use dwGroupBits handed to this method
		// dwGroupBits contains GroupBits of the Style track that sent the notification
		if( SUCCEEDED ( m_pTimeline->GetParam( GUID_StyleNode, dwGroupBits, 0, 
											   *pmtTime, NULL, (void *)&pIStyleNode ) ) )
		{
			if( SUCCEEDED ( pIStyleNode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pIStyleInfo ) ) )
			{
				if( SUCCEEDED ( pIStyleInfo->GetTempo( &dblTempo ) ) )
				{
					m_pTempoStrip->AddTempo( dblTempo, *pmtTime );
				}
				RELEASE( pIStyleInfo );
			}
			RELEASE( pIStyleNode );
		}
		OnDataChanged();

		// Redraw our strip
		m_pTimeline->StripInvalidateRect( m_pTempoStrip, NULL, TRUE );
		return S_OK;
	}

	if( ::IsEqualGUID( rguidType, GUID_Segment_CreateTrack ) )
	{
		// Notify all other StripMgrs that something changed
		// Tempos are global, so don't restrict to just our group(s)
		m_pTimeline->NotifyStripMgrs( GUID_TempoParam, 0xFFFFFFFF, NULL );

		// Notify the Segment designer that we need to be saved, since we have a default tempo
		ASSERT( m_pTempoStrip );
		if( m_pTempoStrip )
		{
			m_pTempoStrip->m_nLastEdit = IDS_INSERT;
			OnDataChanged();
			return S_OK;
		}
	}

	if( ::IsEqualGUID( rguidType, GUID_Segment_DeletedTrack ) )
	{
		if( !pData )
		{
			return E_POINTER;
		}

		IDMUSProdTimeline *pTimeline = (IDMUSProdTimeline *) pData;

		// Notify all other StripMgrs that something changed
		// Tempos are global, so don't restrict to just our group(s)
		pTimeline->NotifyStripMgrs( GUID_TempoParam, 0xFFFFFFFF, NULL );
	}

	if( ::IsEqualGUID( rguidType, GUID_Segment_Set_Tempo ) )
	{
		if( !pData )
		{
			return E_POINTER;
		}

		DMUS_TEMPO_PARAM *pTempo = (DMUS_TEMPO_PARAM *)pData;
		OnTransportChangeTempo( pTempo->dblTempo );

		return S_OK;
	}

	if( ::IsEqualGUID( rguidType, GUID_Segment_Start ) )
	{
		m_pSegmentState = (IDirectMusicSegmentState *)pData;
		if( m_pSegmentState )
		{
			m_pSegmentState->AddRef();

			// If mtLoopEnd is non zero, set m_mtLoopEnd to mtLoopEnd, otherwise use the segment length
			long m_mtLoopEnd;
			IDirectMusicSegment *pSegment;
			if( SUCCEEDED( m_pSegmentState->GetSegment( &pSegment ) ) )
			{
				pSegment->GetLoopPoints( &m_mtLoopStart, &m_mtLoopEnd );
				pSegment->GetRepeats( &m_dwNumLoops );
				if( m_dwNumLoops && (m_mtLoopEnd == 0) )
				{
					MUSIC_TIME mtLength;
					pSegment->GetLength( &mtLength );
					m_mtLoopEnd = mtLength;
				}
				pSegment->Release();
			}
			m_pSegmentState->GetStartPoint( &m_mtStartPoint );
			m_pSegmentState->GetStartTime( &m_mtStartTime );
			//TRACE("Segment Start: %d %d %d %d %d\n", m_mtLoopStart, m_mtLoopEnd, m_dwNumLoops, m_mtStartPoint, m_mtStartTime);
		}
		return S_OK;
	}

	if( ::IsEqualGUID( rguidType, GUID_Segment_Stop ) )
	{
		if( m_pSegmentState )
		{
			m_pSegmentState->Release();
			m_pSegmentState = NULL;
		}
		m_mtLoopEnd = 0;
		m_mtLoopStart = 0;
		m_mtStartTime = 0;
		m_mtStartPoint = 0;
		m_dwNumLoops = 0;
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CTempoMgr::GetStripMgrProperty(
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

			pioTrackHeader->guidClassID = CLSID_DirectMusicTempoTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = DMUS_FOURCC_TEMPO_TRACK;
			pioTrackHeader->fccType = NULL;
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
		V_I4(pVariant) = TRACKCONFIG_VALID_MASK;
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
// CTempoMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CTempoMgr::SetStripMgrProperty(
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
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->Release();
				m_pPropPageMgr = NULL;
			}
			if ( m_pTempoStrip )
			{
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)m_pTempoStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pTempoStrip );
			}
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Stop, m_dwGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Start, m_dwGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Set_Tempo, m_dwGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_NewStyleSelected, m_dwGroupBits );
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
				// Add the strip to the timeline
				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pTempoStrip, CLSID_DirectMusicTempoTrack, m_dwGroupBits, 0 );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_NewStyleSelected, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Set_Tempo, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Start, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Stop, m_dwGroupBits );
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
			V_UNKNOWN( &variant )->QueryInterface( IID_IDirectMusicTrack, (void**)&m_pIDMTrack );
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

		if( m_pDMPerformance )
		{
			m_pDMPerformance->Release();
			m_pDMPerformance = NULL;
		}

		if( V_UNKNOWN( &variant )
		&&	SUCCEEDED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pDMProdFramework) ) )
		{
			IDMUSProdComponent* pIComponent = NULL;
			if( SUCCEEDED ( m_pDMProdFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
			{
				IDMUSProdConductor *pIConductor;
				if( SUCCEEDED( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pIConductor ) ) )
				{
					IUnknown* punk;
					if( SUCCEEDED( pIConductor->GetPerformanceEngine( &punk ) ) )
					{
						punk->QueryInterface( IID_IDirectMusicPerformance, (void **)&m_pDMPerformance );
						punk->Release();
					}
					pIConductor->Release();
				}
				pIComponent->Release();
			}
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
			// Can't change the group bits
			//m_dwGroupBits = pioTrackHeader->dwGroup;
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

			m_dwTrackExtrasFlags = TRACKCONFIG_VALID_MASK & (pioTrackExtrasHeader->dwFlags);
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
// CTempoMgr IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::GetClassID

HRESULT CTempoMgr::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pClsId == NULL )
	{
		return E_POINTER;
	}

	// return our CLSID
	memcpy( pClsId, &CLSID_TempoMgr, sizeof( CLSID ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::IsDirty

HRESULT CTempoMgr::IsDirty()
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
// CTempoMgr::Load

HRESULT CTempoMgr::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Check for Direct Music format
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Remove all existing tempos
	EmptyTempoList();

	MMCKINFO	ck;
	DWORD		dwByteCount;
	DWORD		dwTempoSize;
	DMUS_IO_TEMPO_ITEM iTempo;
	long		lChunkSize;

	// Load the Track
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
		case DMUS_FOURCC_TEMPO_TRACK:
			hr = pIStream->Read( &dwTempoSize, sizeof(DWORD), &dwByteCount );
			if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			dwTempoSize = min( dwTempoSize, sizeof(DMUS_IO_TEMPO_ITEM) );

			lChunkSize = ck.cksize - sizeof(DWORD);

			while(lChunkSize > 0)
			{
				ZeroMemory(	&iTempo, sizeof(DMUS_IO_TEMPO_ITEM) );
				hr = pIStream->Read( &iTempo, dwTempoSize, &dwByteCount );
				if( FAILED( hr ) || dwByteCount != dwTempoSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				lChunkSize -= dwTempoSize;

				CTempoItem* pItem = new CTempoItem;
				if ( pItem == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}

				pItem->m_mtTime = iTempo.lTime;
				pItem->m_dblTempo = iTempo.dblTempo;
				pItem->m_strText.Format("%.2f", pItem->m_dblTempo);

				if( m_pTimeline )
				{
					SetPropTempoBarBeat( (CPropTempo*) pItem );
				}
				InsertByAscendingTime( pItem, FALSE );
			}
			break;
		}
		pIRiffStream->Ascend( &ck, 0 );
	}

	SyncWithDirectMusic();

ON_ERROR:
	pIRiffStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::Save

HRESULT CTempoMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo	StreamInfo;//Added ECW 4/24/98
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

	// We only support saving to a design-time stream (GUID_CurrentVersion) or a DirectMusic
	// stream that will be loaded into a DirectMusicTempoTrack (GUID_DirectMusicObject)
	if( !::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) &&
		!::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
	{
		return E_INVALIDARG;
	}

	// Now, finally save ourself
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Alloc an IDMUSProdRIFFStream from the IStream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

    MMCKINFO ckMain;

	// Always write a tempo data chunk, even if it will be empty

	// Create the chunk to store the Tempo data
	ckMain.ckid = DMUS_FOURCC_TEMPO_TRACK;
	if( pIRiffStream->CreateChunk( &ckMain, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write the structure size
	DWORD dwBytesWritten;
	DWORD dwTempoSize;
	dwTempoSize = sizeof(DMUS_IO_TEMPO_ITEM);
	hr = pIStream->Write( &dwTempoSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	POSITION pos;
	pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		if( pTempoItem->m_dblTempo != 0.0 )
		{
			DMUS_IO_TEMPO_ITEM oTempo;
			// Clear out the structure (clears out the padding bytes as well).
			ZeroMemory( &oTempo, sizeof(DMUS_IO_TEMPO_ITEM) );
			oTempo.lTime = pTempoItem->m_mtTime;
			oTempo.dblTempo = pTempoItem->m_dblTempo;

			hr = pIStream->Write( &oTempo, sizeof(DMUS_IO_TEMPO_ITEM), &dwBytesWritten );
			if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_TEMPO_ITEM) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}
		}
	}

	// Ascend out of the Tempo data chunk.
	pIRiffStream->Ascend( &ckMain, 0 );

	if( fClearDirty )
	{
		m_fDirty = FALSE;
	}

ON_ERROR:
	pIRiffStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::GetSizeMax

HRESULT CTempoMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	UNREFERENCED_PARAMETER( pcbSize );
	// Compute size of stream needed to persist ourself into.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::GetData

// This method is called by CTempoPropPageMgr to get data to send to the
// Tempo property page.
// The CTempoStrip::GetData() method is called by CTempoStripPropPageMgr
// to get the strip's properties (Group Bits, etc.)
HRESULT STDMETHODCALLTYPE CTempoMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected tempo.
	BOOL fMultipleSelect = FALSE;
	CTempoItem* pFirstTempoItem = NULL;
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		if ( pTempoItem->m_fSelected && (pTempoItem->m_dblTempo != 0.0) )
		{
			pFirstTempoItem = pTempoItem;
			while( pos && !fMultipleSelect )
			{
				pTempoItem = m_lstTempos.GetNext( pos );
				if ( pTempoItem->m_fSelected && (pTempoItem->m_dblTempo != 0.0) )
				{
					fMultipleSelect = TRUE;
					break;
				}
			}
			break;
		}
	}

	if( pFirstTempoItem )
	{
		m_SelectedPropTempo.Copy( (CPropTempo*)pFirstTempoItem );
		m_SelectedPropTempo.m_wFlags |= fMultipleSelect ? BF_MULTIPLE : 0;
		*ppData = &m_SelectedPropTempo;
		return S_OK;
	}

	// Nothing selected, multiple items selected, or no 'real' tempos selected
	*ppData = NULL;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::SetData

// This method is called by CTempoPropPageMgr in response to user actions
// in the Tempo Property page.  It changes the currenly selected Tempo. 
HRESULT STDMETHODCALLTYPE CTempoMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pData == NULL )
	{
		return E_INVALIDARG;
	}

	// Get a pointer to the currently selected tempo.
	CTempoItem* pTempoItem = FirstSelectedTempo();

	if ( pTempoItem )
	{
		CPropTempo* pTempo = (CPropTempo*)pData;

		if( (pTempo->m_lMeasure != pTempoItem->m_lMeasure) ||
			(pTempo->m_lBeat != pTempoItem->m_lBeat) ||
			(pTempo->m_lOffset != pTempoItem->m_lOffset) )
		{
			MUSIC_TIME mtOrigTime = pTempoItem->m_mtTime;
			pTempoItem->m_lMeasure = pTempo->m_lMeasure;
			pTempoItem->m_lBeat = pTempo->m_lBeat;
			pTempoItem->m_lOffset = pTempo->m_lOffset;
			SetPropTempoTime( pTempoItem );
			SetPropTempoBarBeat( pTempoItem );

			// Get maximum number of measures
			long lMaxMeasure;
			VARIANT var;
			m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
			MUSIC_TIME mtMaxTimelineLength = V_I4( &var );
			m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtMaxTimelineLength, &lMaxMeasure, NULL );
			lMaxMeasure--;
			// Ensure lMaxMeasure is at least 0
			lMaxMeasure = max( 0, lMaxMeasure );

			// Enforce measure boundary
			if( pTempoItem->m_lMeasure > lMaxMeasure )
			{
				pTempoItem->m_mtTime = mtMaxTimelineLength - 1;
				SetPropTempoBarBeat( pTempoItem );
			}

			if( pTempoItem->m_mtTime != mtOrigTime )
			{
				m_pTempoStrip->m_nLastEdit = IDS_MOVE;

				// Reposition item in tempo list
				POSITION pos = m_lstTempos.Find( pTempoItem );
				ASSERT( pos != NULL );
				if( pos )
				{
					m_lstTempos.RemoveAt( pos );
					if( pos == m_posLastGetParam )
					{
						m_posLastGetParam = NULL;
					}
					InsertByAscendingTime( pTempoItem, FALSE );
				}
			}

			// Refresh property page
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->RefreshData();
			}
		}
		else if( pTempo->m_dblTempo != pTempoItem->m_dblTempo )
		{
			pTempoItem->m_strText.Format("%.2f", pTempo->m_dblTempo);
			pTempoItem->m_dblTempo = pTempo->m_dblTempo;
			m_pTempoStrip->m_nLastEdit = IDS_UNDO_CHANGE;

			if( m_pSegmentState )
			{
				if( pTempoItem == GetTempoForTime(GetCurrentOffset(), NULL) )
				{
					// Change tempo now!!!
					DMUS_TEMPO_PMSG* pTempo;

					if( SUCCEEDED( m_pDMPerformance->AllocPMsg( sizeof(DMUS_TEMPO_PMSG),
						(DMUS_PMSG**)&pTempo ) ) )
					{
						// Queue tempo event
						ZeroMemory( pTempo, sizeof(DMUS_TEMPO_PMSG) );
						pTempo->dblTempo = pTempoItem->m_dblTempo;
						pTempo->dwFlags = DMUS_PMSGF_REFTIME;
						pTempo->dwType = DMUS_PMSGT_TEMPO;
						m_pDMPerformance->SendPMsg( (DMUS_PMSG*)pTempo );
					}
				}
			}
		}
		else
		{
			return S_OK;
		}
			
		// Redraw the tempo strip
		// TODO: Should be smarter and only redraw the tempo that changed
		m_pTimeline->StripInvalidateRect( m_pTempoStrip, NULL, TRUE );

		// Let our hosting editor and the music engine know about the changes
		OnDataChanged();
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CTempoMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		CTempoPropPageMgr* pPPM = new CTempoPropPageMgr;
		if( pPPM == NULL )
		{
			return E_OUTOFMEMORY;
		}

		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		
		// Remove the reference created by the contrustor, leaving the one created by QueryInterface.
		// If QueryInterface failed, this will delete m_pPropPageMgr.
		m_pPropPageMgr->Release();

		if( FAILED(hr) )
		{
			return hr;
		}
	}

	// Set the displayed property page to our property page
	m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CTempoMgr::OnRemoveFromPageManager( void)
{
	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::OnDataChanged

HRESULT STDMETHODCALLTYPE CTempoMgr::OnDataChanged( void)
{
	SyncWithDirectMusic();

	ASSERT( m_pTimeline );
	if ( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Let our hosting editor know about the change
	m_pTimeline->OnDataChanged( (IDMUSProdStripMgr*)this );

	// Notify all other StripMgrs that something changed
	// Tempos are global, so don't restrict to just our group(s)
	m_pTimeline->NotifyStripMgrs( GUID_TempoParam, 0xFFFFFFFF, NULL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::DeleteSelectedTempos

HRESULT CTempoMgr::DeleteSelectedTempos()
{
	POSITION pos2, pos1 = m_lstTempos.GetHeadPosition();
	while( pos1 )
	{
		// Need to save current position, since GetNext will set pos1 to point
		// to the next tempo.
		pos2 = pos1;
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos1 );
		if ( pTempoItem->m_fSelected )
		{
			// Remove the tempo from the current position.  pos1 will
			// still be valid, and it will still point to the next tempo.
			m_lstTempos.RemoveAt( pos2 );
			if( pos2 == m_posLastGetParam )
			{
				m_posLastGetParam = NULL;
			}
			delete pTempoItem;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::SaveSelectedTempos

HRESULT CTempoMgr::SaveSelectedTempos(LPSTREAM pIStream, long lOffset)
{
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// If the Tempo list has anything in it, look for selected Tempos
	if ( !m_lstTempos.IsEmpty() )
	{
		POSITION pos;

		// Create the chunk to wrap our data in
		MMCKINFO ckMain;
		ckMain.ckid = DMUS_FOURCC_TEMPO_TRACK;
		if( pIRiffStream->CreateChunk( &ckMain, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write the structure size
		DWORD dwBytesWritten;
		DWORD dwTempoSize;
		dwTempoSize = sizeof(PROD_IO_TEMPO_ITEM);
		hr = pIStream->Write( &dwTempoSize, sizeof(DWORD), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Now, write out each selected item
		pos = m_lstTempos.GetHeadPosition();
		while( pos )
		{
			CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
			if( pTempoItem->m_fSelected && (pTempoItem->m_dblTempo != 0.0) )
			{
				PROD_IO_TEMPO_ITEM oTempo;
				oTempo.lTime = pTempoItem->m_mtTime - lOffset;
				oTempo.dblTempo = pTempoItem->m_dblTempo;
				oTempo.lOffset = pTempoItem->m_lOffset;

				DWORD dwBytesWritten;
				hr = pIStream->Write( &oTempo, sizeof(PROD_IO_TEMPO_ITEM), &dwBytesWritten );
				if( FAILED( hr ) || dwBytesWritten != sizeof(PROD_IO_TEMPO_ITEM) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}

		// Ascend out of the chunk our data is in
		pIRiffStream->Ascend( &ckMain, 0 );
	}
	else
	{
		hr = S_FALSE; // Nothing in the list
	}

ON_ERROR:
	pIRiffStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::MarkSelectedTempos

void CTempoMgr::MarkSelectedTempos( DWORD dwFlags )
{
	// For all selected items, OR dwFlags with the item's m_dwBits member.
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		if ( pTempoItem->m_fSelected )
		{
			pTempoItem->m_dwBits |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::DeleteMarked

void CTempoMgr::DeleteMarked( DWORD dwFlags )
{
	// For all items, if the item's m_dwBits member has any of the same bits set as
	// dwFlags (m_dwBits & dwFlags is non-zero), delete it.
	POSITION pos2, pos1 = m_lstTempos.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos1 );
		if ( pTempoItem->m_dwBits & dwFlags )
		{
			m_lstTempos.RemoveAt( pos2 );
			if( pos2 == m_posLastGetParam )
			{
				m_posLastGetParam = NULL;
			}
			delete pTempoItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::UnMarkTempos

void CTempoMgr::UnMarkTempos( DWORD dwFlags )
{
	// For all items, clear the bits in the item's m_dwBits member that are also set in dwFlags.
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		m_lstTempos.GetNext( pos )->m_dwBits &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::UnselectAll

bool CTempoMgr::UnselectAll()
{
	bool fChanged = false;

	// Unselect all items in the list
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		if( pTempoItem->m_fSelected )
		{
			pTempoItem->m_fSelected = FALSE;
			fChanged = true;
		}
	}

	if( m_pTempoStrip )
	{
		m_pTempoStrip->m_pCurrentlySelectedTempoItem = NULL;
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::SelectAll

void CTempoMgr::SelectAll()
{
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		if( (pTempoItem->m_dblTempo != 0.0) )
		{
			pTempoItem->m_fSelected = TRUE;
		}
		// Unselect all 'fake' items
		else
		{
			pTempoItem->m_fSelected = FALSE;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::IsSelected

BOOL CTempoMgr::IsSelected()
{
	// If anything is selected, return TRUE.
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		if ( (pTempoItem->m_dblTempo != 0.0) && pTempoItem->m_fSelected )
		{
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::FirstSelectedTempo

CTempoItem* CTempoMgr::FirstSelectedTempo()
{
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		if ( pTempoItem->m_fSelected )
		{
			return pTempoItem;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::SelectSegment

BOOL CTempoMgr::SelectSegment(long lBeginTime, long lEndTime)
{
	ASSERT(m_pTimeline);
	if( !m_pTimeline )
	{
		return FALSE;
	}

	BOOL result = FALSE;
	if( lBeginTime == -1)
	{
		// till we implement m_lShiftFromMeasure (see CommandMgr::SelectSegment),
		// just set it to beginning
		lBeginTime = 0;
	}

	long lBeginMeas, lBeginBeat, lEndMeas, lEndBeat;
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lBeginTime, &lBeginMeas, &lBeginBeat );
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lEndTime, &lEndMeas, &lEndBeat );

	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		pTempoItem->m_fSelected = FALSE;
		if( lBeginMeas < pTempoItem->m_lMeasure && pTempoItem->m_lMeasure < lEndMeas )
		{
			pTempoItem->m_fSelected = TRUE;
			result = TRUE;
		}
		else if( lBeginMeas == pTempoItem->m_lMeasure )
		{
			if( lEndMeas == pTempoItem->m_lMeasure )
			{
				if( lBeginBeat <= pTempoItem->m_lBeat && pTempoItem->m_lBeat <= lEndBeat )
				{
					pTempoItem->m_fSelected = TRUE;
					result = TRUE;
				}
			}
			else
			{
				if( lBeginBeat <= pTempoItem->m_lBeat )
				{
					pTempoItem->m_fSelected = TRUE;
					result = TRUE;
				}
			}
		}
		else if( lEndMeas == pTempoItem->m_lMeasure )
		{
			if( pTempoItem->m_lBeat <= lEndBeat )
			{
				pTempoItem->m_fSelected = TRUE;
				result = TRUE;
			}
		}
	}
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::EmptyTempoList

void CTempoMgr::EmptyTempoList(void)
{
	// Remove all items from the list
	if( !m_lstTempos.IsEmpty() )
	{
		m_posLastGetParam = NULL;
		CTempoItem *pTempoItem;
		while ( !m_lstTempos.IsEmpty() )
		{
			pTempoItem = m_lstTempos.RemoveHead();
			delete pTempoItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::InsertByAscendingTime

void CTempoMgr::InsertByAscendingTime( CTempoItem *pTempo, BOOL fPaste )
{
	// Insert pTempo into the list in ascending order, determined by the values of
	// the item's m_mtTime member.
	ASSERT( pTempo );
	if ( pTempo == NULL )
	{
		return;
	}

	POSITION posCurrent, posNext = m_lstTempos.GetHeadPosition();
	while( posNext )
	{
		posCurrent = posNext;
		CTempoItem* pTempoItem = m_lstTempos.GetNext( posNext );
		if ( fPaste )
		{
			if ( pTempoItem->m_mtTime == pTempo->m_mtTime )
			{
				// replace item
				m_lstTempos.InsertBefore( posCurrent, pTempo );
				m_lstTempos.RemoveAt( posCurrent );
				if( posCurrent == m_posLastGetParam )
				{
					m_posLastGetParam = NULL;
				}
				delete pTempoItem;
				return;
			}
		}
		if ( pTempoItem->m_mtTime > pTempo->m_mtTime )
		{
			// insert before posCurrent (which is current position of pTempoItem)
			m_lstTempos.InsertBefore( posCurrent, pTempo );
			return;
		}
	}
	// insert at end of list
	m_lstTempos.AddTail( pTempo );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::RemoveItem

BOOL CTempoMgr::RemoveItem( CTempoItem* pItem )
{
	// Removes the specified item from the list
	POSITION pos2;
	POSITION pos1 = m_lstTempos.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		if ( m_lstTempos.GetNext( pos1 ) == pItem )
		{
			m_lstTempos.RemoveAt( pos2 );
			if( pos2 == m_posLastGetParam )
			{
				m_posLastGetParam = NULL;
			}
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::SyncWithDirectMusic

HRESULT CTempoMgr::SyncWithDirectMusic(  )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// 1. persist the strip as a tempo track to a stream
	IStream* pIMemStream = NULL;
	IPersistStream* pIPersistStream = NULL;
	hr = m_pDMProdFramework->AllocMemoryStream( FT_RUNTIME, GUID_CurrentVersion, &pIMemStream );
	if( SUCCEEDED ( hr ) )
	{
		StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
		hr = Save(pIMemStream, FALSE);
		if ( SUCCEEDED( hr ) )
		{
			// 2. load the stream into m_pIDMTrack
			hr = m_pIDMTrack->QueryInterface(IID_IPersistStream, (void**)&pIPersistStream);
			if ( SUCCEEDED( hr ) )
			{
				StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
				hr = pIPersistStream->Load(pIMemStream);
				pIPersistStream->Release();
			}
		}
		pIMemStream->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::SetPropTempoBarBeat

void CTempoMgr::SetPropTempoBarBeat( CPropTempo *pTempo )
{
	// Fill in the item's m_lMeasure, m_lBeat, and m_lOffset members, based on
	// the timesignature(s) for the group(s) we belong to.
	ASSERT( pTempo && m_pTimeline );
	if( !pTempo || !m_pTimeline )
	{
		return;
	}

	// If our time value is negative, set the offset to our time value and everything else
	// to zero.
	if( pTempo->m_mtTime < 0 )
	{
		pTempo->m_lMeasure = 0;
		pTempo->m_lBeat = 0;
		pTempo->m_lOffset = pTempo->m_mtTime;
		return;
	}

	// Our time value is positive (or 0)

	// Let the Timeline decide which Measure and Beat we're in, and get the clock position
	// of that measure and beat.
	long lMeasure, lBeat, lClocks;
	if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, m_dwIndex, pTempo->m_mtTime, &lMeasure, &lBeat ) ) && 
		SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, lMeasure, lBeat, &lClocks )))
	{
		pTempo->m_lMeasure = lMeasure;
		pTempo->m_lBeat = lBeat;
		pTempo->m_lOffset = pTempo->m_mtTime - lClocks;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::SetPropTempoTime

BOOL CTempoMgr::SetPropTempoTime( CPropTempo *pTempo )
{
	BOOL fChanged = FALSE;

	// Fill in the item's m_mtTime member, based on the timesignature(s) for the group(s)
	// we belong to, and the item's m_lMeasure, m_lBeat, and m_lOffset members
	ASSERT( pTempo && m_pTimeline );
	if( !pTempo || !m_pTimeline )
	{
		return FALSE;
	}

	// Convert m_lMeasure and m_lBeat to a clock value
	long lClocks;
	if( SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, pTempo->m_lMeasure, pTempo->m_lBeat, &lClocks )))
	{
		lClocks += pTempo->m_lOffset;
		if( lClocks < 0 )
		{
			lClocks = 0;
		}
		if( lClocks != pTempo->m_mtTime )
		{
			pTempo->m_mtTime = lClocks;
			fChanged = TRUE;
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::RecomputeTempoMeasureBeats

void CTempoMgr::RecomputeTempoMeasureBeats()
{
	// For all items, call SetPropTempoBarBeat to update their m_lMeasure, m_lBeat,
	// and m_lOffset members
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		SetPropTempoBarBeat( static_cast<CPropTempo*>(m_lstTempos.GetNext( pos )) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::GetNextGreatestUniqueTime

MUSIC_TIME CTempoMgr::GetNextGreatestUniqueTime( MUSIC_TIME mtOrigTime )
{
	DMUS_TIMESIGNATURE dmTimeSig;
	MUSIC_TIME mtTime;
	long lMeasure = 0;
	long lBeat = 0;
	long lTick = 0;

	// Calc measure, beat, tick associated with mtOrigTime
	if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, m_dwIndex, mtOrigTime, &lMeasure, &lBeat ) )
	&&	SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, lMeasure, lBeat, &lTick ) ) )
	{
		lTick = mtOrigTime - lTick;
	}

	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );

		if( pTempoItem->m_lMeasure > lMeasure )
		{
			break;
		}

		// Get the time signature for this measure
		if( SUCCEEDED ( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, pTempoItem->m_lMeasure, 0, &mtTime ) ) )
		{
			m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, mtTime, NULL, &dmTimeSig );
			if( pTempoItem->m_lBeat > dmTimeSig.bBeatsPerMeasure )
			{
				break;
			}

			if( pTempoItem->m_lMeasure == lMeasure
			&&  pTempoItem->m_lBeat == lBeat )
			{
				lTick = pTempoItem->m_lOffset + 1;

				// Take care of measure/beat rollover
				if( SUCCEEDED ( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, lMeasure, lBeat, &mtTime ) ) )
				{
					mtTime += lTick;

					if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, m_dwIndex, mtTime, &lMeasure, &lBeat ) )
					&&	SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, lMeasure, lBeat, &lTick ) ) )
					{
						lTick = mtTime - lTick;
					}
				}
			}
		}
	}

	mtTime = mtOrigTime;
	if( SUCCEEDED ( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, lMeasure, lBeat, &mtTime ) ) )
	{
		mtTime += lTick;
	}

	return mtTime;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::RecomputeTempoTimes

BOOL CTempoMgr::RecomputeTempoTimes()
{
	MUSIC_TIME mtTime;
	long lMeasure = 0;
	long lBeat = 0;
	long lTick = 0;
	POSITION pos2;

	if( m_pTimeline == NULL )
	{
		return FALSE;
	}

	BOOL fChanged = FALSE;

	// For all items, call SetPropTempoTime to update their m_mtTime member
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );

		if( pTempoItem->m_dblTempo == 0.0 )
		{
			m_lstTempos.RemoveAt( pos2 );
			if( pos2 == m_posLastGetParam )
			{
				m_posLastGetParam = NULL;
			}
			delete pTempoItem;
		}
		else if( pTempoItem->m_mtTime > 0 )
		{
			if( SUCCEEDED ( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, pTempoItem->m_lMeasure, pTempoItem->m_lBeat, &mtTime ) ) )
			{
				mtTime += pTempoItem->m_lOffset;

				if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, m_dwIndex, mtTime, &lMeasure, &lBeat ) )
				&&	SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, lMeasure, lBeat, &lTick ) ) )
				{
					lTick = mtTime - lTick;
				}
			}

			if( pTempoItem->m_mtTime != mtTime
			||	pTempoItem->m_lMeasure != lMeasure
			||	pTempoItem->m_lBeat != lBeat
			||	pTempoItem->m_lOffset != lTick )
			{
				m_lstTempos.RemoveAt( pos2 );
				if( pos2 == m_posLastGetParam )
				{
					m_posLastGetParam = NULL;
				}

				if( pTempoItem->m_lMeasure != lMeasure )
				{
					// This would happen when moving from 7/4 to 4/4, for example
					// Tempos on beat 7 would end up on next measure's beat 3

					MUSIC_TIME mtTempTime = mtTime;
					long lTempMeasure = lMeasure;
					long lTempBeat = lBeat;
					long lTempTick = lTick;

					while( pTempoItem->m_lMeasure != lTempMeasure )
					{
						BOOL fSuccess = FALSE;

						// Keep moving back a beat until the measure does not change
						if( SUCCEEDED ( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, lTempMeasure, --lTempBeat, &mtTempTime ) ) )
						{
							mtTempTime += lTempTick;
							if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, m_dwIndex, mtTempTime, &lTempMeasure, &lTempBeat ) )
							&&	SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, lTempMeasure, lTempBeat, &lTempTick ) ) )
							{
								lTempTick = mtTempTime - lTempTick;

								fSuccess = TRUE;
								mtTime = mtTempTime;
								lMeasure = lTempMeasure;
								lBeat = lTempBeat;
								lTick = lTempTick;
							}
						}
						
						if( fSuccess == FALSE)
						{
							break;
						}
					}
				}

				mtTime = GetNextGreatestUniqueTime( mtTime ); 
				if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, m_dwIndex, mtTime, &lMeasure, &lBeat ) )
				&&	SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, lMeasure, lBeat, &lTick ) ) )
				{
					lTick = mtTime - lTick;

					pTempoItem->m_mtTime = mtTime;
					pTempoItem->m_lMeasure = lMeasure;
					pTempoItem->m_lBeat = (BYTE)lBeat;
					pTempoItem->m_lOffset = lTick;
					fChanged = TRUE;
				}

				InsertByAscendingTime( pTempoItem, FALSE );
			}
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::GetBoundariesOfSelectedTempos

void CTempoMgr::GetBoundariesOfSelectedTempos( long *plStart, long *plEnd )
{
	ASSERT( plStart );
	ASSERT( plEnd );

	long lEnd = -1;
	BOOL fSetStart = FALSE;

	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		if ( pTempoItem->m_fSelected )
		{
			if( lEnd < pTempoItem->m_mtTime )
			{
				lEnd = pTempoItem->m_mtTime;
			}
			if( !fSetStart )
			{
				fSetStart = TRUE;
				*plStart = pTempoItem->m_mtTime;
			}
		}
	}

	if( lEnd >= 0 )
	{
		*plEnd = lEnd;
	}
	else
	{
		*plStart = -1;
		*plEnd = -1;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::DeleteBetweenTimes

BOOL CTempoMgr::DeleteBetweenTimes( long lStart, long lEnd )
{
	BOOL fChanged = FALSE;

	// Iterate through the list
	POSITION pos2, pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		// Save the current position
		pos2 = pos;
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );

		// If the tempo occurs between lStart and lEnd, delete it
		if( (pTempoItem->m_mtTime >= lStart) && (pTempoItem->m_mtTime <= lEnd) ) 
		{
			m_lstTempos.RemoveAt( pos2 );
			if( pos2 == m_posLastGetParam )
			{
				m_posLastGetParam = NULL;
			}
			delete pTempoItem;
			fChanged = TRUE;
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::ClocksToMeasureBeat

HRESULT CTempoMgr::ClocksToMeasureBeat( long lTime, long *plMeasure, long *plBeat )
{
	if( lTime < 0 )
	{
		if( plMeasure )
		{
			*plMeasure = 0;
		}
		if( plBeat )
		{
			*plBeat = 0;
		}
		return S_OK;
	}
	else if( m_pTimeline )
	{
		return m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits,
												 m_dwIndex,
												 lTime,
												 plMeasure,
												 plBeat );
	}
	else
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}
}



/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::CanCycle

BOOL CTempoMgr::CanCycle( long lXPos  )
{
	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	long lMeasure;
	long lBeat;
	HRESULT hr = m_pTimeline->PositionToMeasureBeat( m_dwGroupBits, 0, lXPos, &lMeasure, &lBeat );

	if( SUCCEEDED ( hr ) ) 
	{
		int nCount = 0;
		long lTempoMeasure;
		long lTempoBeat;

		POSITION pos = m_lstTempos.GetHeadPosition();
		while( pos )
		{
			CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );

			ClocksToMeasureBeat( pTempoItem->m_mtTime,
								 &lTempoMeasure,
								 &lTempoBeat );
			if( lTempoMeasure == lMeasure
			&&  lTempoBeat == lBeat
			&&  pTempoItem->m_strText.IsEmpty() == FALSE )
			{
				if( ++nCount > 1 )
				{
					return TRUE;
				}
			}

			if( lTempoMeasure > lMeasure )
			{
				break;
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::CycleTempos

HRESULT CTempoMgr::CycleTempos( long lXPos  )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	long lMeasure;
	long lBeat;
	HRESULT hr = m_pTimeline->PositionToMeasureBeat( m_dwGroupBits, 0, lXPos, &lMeasure, &lBeat );

	if( SUCCEEDED ( hr ) ) 
	{
		CTempoItem* pFirstTempoItem = NULL;
		CTempoItem* pSecondTempoItem = NULL;
		long lTempoMeasure;
		long lTempoBeat;
		long lNextTempoMeasure;
		long lNextTempoBeat;

		hr = E_FAIL;

		POSITION pos = m_lstTempos.GetHeadPosition();
		while( pos )
		{
			CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );

			ClocksToMeasureBeat( pTempoItem->m_mtTime,
								 &lTempoMeasure,
								 &lTempoBeat );
			if( lTempoMeasure == lMeasure
			&&  lTempoBeat == lBeat
			&&  pTempoItem->m_strText.IsEmpty() == FALSE )
			{
				if( pFirstTempoItem == NULL )
				{
					pFirstTempoItem = pTempoItem;
				}
				else if( pSecondTempoItem == NULL )
				{
					pSecondTempoItem = pTempoItem;
				}

				if( pTempoItem->m_wFlags & BF_TOPTEMPO )
				{
					if( pos )
					{
						// Cycle to next Tempo if on same measure/beat
						CTempoItem* pNextTempoItem = m_lstTempos.GetNext( pos );

						ClocksToMeasureBeat( pNextTempoItem->m_mtTime,
											 &lNextTempoMeasure,
											 &lNextTempoBeat );
						if( lNextTempoMeasure == lMeasure
						&&  lNextTempoBeat == lBeat
						&&  pNextTempoItem->m_strText.IsEmpty() == FALSE )
						{
							UnselectAll();
							pTempoItem->m_wFlags &= ~BF_TOPTEMPO;
							pNextTempoItem->m_fSelected = TRUE;
							pNextTempoItem->m_wFlags |= BF_TOPTEMPO;
							hr = S_OK;
							break;
						}
					}

					// Cycle to first Tempo on same measure/beat
					UnselectAll();
					pTempoItem->m_wFlags &= ~BF_TOPTEMPO;
					pFirstTempoItem->m_fSelected = TRUE;
					pFirstTempoItem->m_wFlags |= BF_TOPTEMPO;
					hr = S_OK;
					break;
				}
			}

			if( lTempoMeasure > lMeasure
			||  pos == NULL )
			{
				UnselectAll();
				if( pSecondTempoItem )
				{
					pSecondTempoItem->m_fSelected = TRUE;
					pSecondTempoItem->m_wFlags |= BF_TOPTEMPO;
					hr = S_OK;
				}
				else if( pFirstTempoItem )
				{
					pFirstTempoItem->m_fSelected = TRUE;
					pFirstTempoItem->m_wFlags |= BF_TOPTEMPO;
					hr = S_OK;
				}
				break;
			}
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		// Redraw the Tempo strip
		m_pTimeline->StripInvalidateRect( m_pTempoStrip, NULL, TRUE );

		// Update the property page
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::CreateTempo

HRESULT	CTempoMgr::CreateTempo( MUSIC_TIME mtTime, CTempoItem *&rpTempo )
{
	rpTempo = NULL;

	if ( !m_pTimeline )
	{
		return E_FAIL;
	}

	rpTempo = new CTempoItem;

	if(rpTempo == NULL)
	{
		return E_OUTOFMEMORY;
	}

	long lMeasure = 0;
	long lBeat = 0;
	HRESULT hr;
	hr = m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
	ASSERT(SUCCEEDED(hr));
	rpTempo->m_dblTempo = 0.0;
	rpTempo->m_strText.Empty();
	rpTempo->m_lMeasure = lMeasure;
	ASSERT( lBeat >= 0 && lBeat < 256 );
	if( lBeat < 0 )
	{
		lBeat = 0;
	}
	else if( lBeat > 255 )
	{
		lBeat = 255;
	}
	rpTempo->m_lBeat = lBeat;
	rpTempo->m_lOffset = 0;
	hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &rpTempo->m_mtTime );
	InsertByAscendingTime( rpTempo, FALSE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::FindTempo

CTempoItem* CTempoMgr::FindTempo(long lMeasure, long lBeat)
{
	POSITION pos = m_lstTempos.GetHeadPosition();
	while( pos )
	{
		CTempoItem* pTempoItem = m_lstTempos.GetNext( pos );
		if ( pTempoItem->m_lMeasure == lMeasure && pTempoItem->m_lBeat == lBeat)
		{
			return pTempoItem;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::OnTransportChangeTempo

void CTempoMgr::OnTransportChangeTempo( double dblTempo )
{
	// Get the tempo at the current cursor position;
	long lTimeCursor;
	if( m_pTimeline && m_pTempoStrip &&
		SUCCEEDED(m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTimeCursor) ) )
	{
		CTempoItem *pTempoKeep = NULL;
		POSITION pos = m_lstTempos.GetHeadPosition();
		while( pos )
		{
			CTempoItem *pTempoItem = m_lstTempos.GetNext( pos );
			if( lTimeCursor < pTempoItem->m_mtTime )
			{
				break;
			}

			if( pTempoItem->m_dblTempo != 0.0 )
			{
				pTempoKeep = pTempoItem;
			}
		}
		if( pTempoKeep && (pTempoKeep->m_dblTempo != dblTempo) )
		{
			if( m_pSegmentState )
			{
				if( pTempoKeep == GetTempoForTime(GetCurrentOffset(), NULL) )
				{
					// Change tempo now!!!
					DMUS_TEMPO_PMSG* pTempo;

					if( SUCCEEDED( m_pDMPerformance->AllocPMsg( sizeof(DMUS_TEMPO_PMSG),
						(DMUS_PMSG**)&pTempo ) ) )
					{
						// Queue tempo event
						ZeroMemory( pTempo, sizeof(DMUS_TEMPO_PMSG) );
						pTempo->dblTempo = pTempoKeep->m_dblTempo;
						pTempo->dwFlags = DMUS_PMSGF_REFTIME;
						pTempo->dwType = DMUS_PMSGT_TEMPO;
						m_pDMPerformance->SendPMsg( (DMUS_PMSG*)pTempo );
					}
				}
			}

			pTempoKeep->m_strText.Format("%.2f", dblTempo);
			pTempoKeep->m_dblTempo = dblTempo;
			m_pTempoStrip->m_nLastEdit = IDS_UNDO_CHANGE;

			// Refresh property page
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->RefreshData();
			}
			
			// Redraw the tempo strip
			// TODO: Should be smarter and only redraw the tempo that changed
			m_pTimeline->StripInvalidateRect( m_pTempoStrip, NULL, TRUE );

			// Un-Freeze undo queue
			BOOL fOrigFreezeState = FALSE;
			VARIANT var;
			if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
			{
				fOrigFreezeState = V_BOOL(&var);
			}

			if( fOrigFreezeState )
			{
				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
			}

			// Let our hosting editor and the music engine know about the changes
			OnDataChanged();

			// Re-disable undo queue, if neccessary
			if( fOrigFreezeState )
			{
				var.vt = VT_BOOL;
				V_BOOL(&var) = TRUE;
				m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::GetCurrentOffset

MUSIC_TIME CTempoMgr::GetCurrentOffset( void )
{
	ASSERT(m_pDMPerformance);

	MUSIC_TIME mtNow;
	if ( m_pSegmentState && m_pDMPerformance && SUCCEEDED( m_pDMPerformance->GetTime( NULL, &mtNow ) ) )
	{
		// Convert mtNow from absolute time to an offset from when the segment started playing
		mtNow -= m_mtStartTime - m_mtStartPoint;

		if( (m_dwNumLoops != 0) && (m_mtStartPoint < m_mtLoopEnd) )
		{
			if( mtNow > (m_mtLoopStart + (m_mtLoopEnd - m_mtLoopStart) * (signed)m_dwNumLoops) )
			{
				mtNow -= (m_mtLoopEnd - m_mtLoopStart) * m_dwNumLoops;
			}
			else if( mtNow > m_mtLoopStart )
			{
				mtNow = m_mtLoopStart + (mtNow - m_mtLoopStart) % (m_mtLoopEnd - m_mtLoopStart);
			}
		}

		return mtNow;
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoMgr::GetTempoForTime

CTempoItem* CTempoMgr::GetTempoForTime( MUSIC_TIME mtTime, CTempoItem** ppTempoItemNext )
{
	// Initialize data to return
	CTempoItem *pTempoToReturn = NULL;
	if( ppTempoItemNext )
	{
		*ppTempoItemNext = NULL;
	}

	// Initialize pos to NULL
	POSITION pos = NULL;

	// If we have a cached position
	if( m_posLastGetParam )
	{
		// Get the cached tempo
		CTempoItem *pItem = m_lstTempos.GetAt(m_posLastGetParam);

		// If the time is more than halfway to the cached tempo
		if( mtTime > pItem->m_mtTime / 2 )
		{
			// Start searching from m_posLastGetParam
			pos = m_posLastGetParam;

			// If mtTime is earlier than the cached tempo, search backwards
			if( mtTime < pItem->m_mtTime )
			{
				while( pos )
				{
					POSITION posNow = pos;
					// Continue until we find a tempo earlier than mtTime
					if( m_lstTempos.GetPrev( pos )->m_mtTime < mtTime )
					{
						pos = posNow;
						break;
					}
				}

				// If pos is NULL, set pos to the head position (happens below)
			}

			// Otherwise, fall through and search forwards (from m_posLastGetParam)
		}
	}

	if( pos == NULL )
	{
		pos = m_lstTempos.GetHeadPosition();
	}

	while( pos )
	{
		const POSITION posCurrent = pos;
		CTempoItem *pTempoItem = m_lstTempos.GetNext( pos );
		if( mtTime < pTempoItem->m_mtTime )
		{
			// Set next	
			if( ppTempoItemNext )
			{
				if( pTempoItem->m_dblTempo != 0.0 )
				{
					*ppTempoItemNext = pTempoItem;
				}
				else
				{
					while( pTempoItem->m_dblTempo == 0.0 )
					{
						if( pos == NULL )
						{
							break;
						}
						pTempoItem = m_lstTempos.GetNext( pos );
						if( pTempoItem->m_dblTempo != 0.0 )
						{
							*ppTempoItemNext = pTempoItem;
						}
					}
				}
			}
			break;
		}

		if( pTempoItem->m_dblTempo != 0.0 )
		{
			pTempoToReturn = pTempoItem;
			m_posLastGetParam = posCurrent;
		}
	}

	return pTempoToReturn;
}
