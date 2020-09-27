// TimeSigMgr.cpp : implementation file
//

/*--------------
@doc TIMESIGSAMPLE
--------------*/

#include "stdafx.h"
#include "TimeSigIO.h"
#include "TimeSigStripMgr.h"
#include "TimeSigMgr.h"
#include "DLLJazzDataObject.h"
#include "PropPageMgr.h"
#include <dmusici.h>
#include <dmusicf.h>
#include <mmreg.h>
#include <RiffStrm.h>
#include <initguid.h>
#include <SegmentGuids.h>
#include "SegmentIO.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_MEASURE 0xFFFFFFFF

inline MUSIC_TIME ClocksPerBeat(DMUS_TIMESIGNATURE& TimeSig)
{ return DMUS_PPQ * 4 / TimeSig.bBeat; }

inline MUSIC_TIME ClocksPerMeasure(DMUS_TIMESIGNATURE& TimeSig)
{ return ClocksPerBeat(TimeSig) * TimeSig.bBeatsPerMeasure; }

inline DWORD ClocksToMeasure(DWORD dwTotalClocks, DMUS_TIMESIGNATURE& TimeSig)
{ return (dwTotalClocks / ClocksPerMeasure(TimeSig)); }

POSITION GetFirstValidTimeSig(POSITION pos, const CTypedPtrList<CPtrList, CTimeSigItem*> &lstTimeSigs )
{
	while( pos )
	{
		POSITION posToReturn = pos;
		if( !(lstTimeSigs.GetNext( pos )->m_dwBits & UD_FAKE) )
		{
			return posToReturn;
		}
	}
	return NULL;
}

void CTimeSigMgr::ClocksToMeasure(MUSIC_TIME mtTime, DWORD& rdwMeasure)
{
	//ASSERT( mtTime >= 0 );
	if( mtTime <= 0 )
	{
		rdwMeasure = 0;
		return;
	}

	DMUS_TIMESIGNATURE TimeSignature;

	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	pos = GetFirstValidTimeSig( pos, m_lstTimeSigs );
	if( !pos )
	{
		// Default to 4/4, since this is what the Timeline defaults to
		TimeSignature.bBeatsPerMeasure = 4;
		TimeSignature.bBeat = 4;
		rdwMeasure = ::ClocksToMeasure(mtTime, TimeSignature);
		return;
	}

	CTimeSigItem* pTimeSigItem = NULL;
	MUSIC_TIME mtSRTime = 0;
	DWORD dwCurrentMeasure = 0;

	// Initialize TimeSignature with the TimeSig of the first valid item
	pTimeSigItem = m_lstTimeSigs.GetAt( pos );
	TimeSignature = pTimeSigItem->m_TimeSignature;

	while( pos )
	{
		pTimeSigItem = m_lstTimeSigs.GetNext( pos );

		// This should be a real TimeSig
		ASSERT( !(pTimeSigItem->m_dwBits & UD_FAKE) );

		MUSIC_TIME mtMeasureClocks = ClocksPerMeasure(TimeSignature);
		long lNumMeasures = pTimeSigItem->m_dwMeasure - dwCurrentMeasure;
		if (mtSRTime + mtMeasureClocks * lNumMeasures > mtTime)
		{
			break;
		}

		mtSRTime += mtMeasureClocks * lNumMeasures;
		dwCurrentMeasure = pTimeSigItem->m_dwMeasure;
		TimeSignature = pTimeSigItem->m_TimeSignature;

		// Get the next valid TimeSig
		pos = GetFirstValidTimeSig( pos, m_lstTimeSigs );
	}

	mtTime -= mtSRTime;
	rdwMeasure = dwCurrentMeasure + ::ClocksToMeasure(mtTime, TimeSignature);
}

MUSIC_TIME CTimeSigMgr::MeasureToClocks(DWORD dwMeasure)
{ 
	DMUS_TIMESIGNATURE TimeSignature;

	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	pos = GetFirstValidTimeSig( pos, m_lstTimeSigs );
	if( !pos )
	{
		// Default to 4/4, since this is what the Timeline defaults to
		TimeSignature.bBeatsPerMeasure = 4;
		TimeSignature.bBeat = 4;
		return ClocksPerMeasure(TimeSignature) * (MUSIC_TIME) dwMeasure; 
	}

	CTimeSigItem* pTimeSigItem = NULL;
	MUSIC_TIME mtSRTime = 0;
	DWORD dwCurrentMeasure = 0;

	// Initialize TimeSignature with the TimeSig of the first valid item
	TimeSignature = m_lstTimeSigs.GetAt( pos )->m_TimeSignature;

	while( pos )
	{
		pTimeSigItem = m_lstTimeSigs.GetNext( pos );

		// This should be a real TimeSig
		ASSERT( !(pTimeSigItem->m_dwBits & UD_FAKE) );

		// JHD: This is >= (not >) because dwMeasure is 1-based, while
		// m_dwMeasure is 0-based
		if( pTimeSigItem->m_dwMeasure >= dwMeasure)
		{
			break;
		}

		mtSRTime += ClocksPerMeasure(TimeSignature) * 
			(MUSIC_TIME) (pTimeSigItem->m_dwMeasure - dwCurrentMeasure);
		dwCurrentMeasure = pTimeSigItem->m_dwMeasure;
		TimeSignature = pTimeSigItem->m_TimeSignature;

		// Get the next valid TimeSig
		pos = GetFirstValidTimeSig( pos, m_lstTimeSigs );
	}
	return 
		mtSRTime + 
		ClocksPerMeasure(TimeSignature) * (MUSIC_TIME) (dwMeasure - dwCurrentMeasure);
}

////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr constructor/destructor 

CTimeSigMgr::CTimeSigMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pDMProdFramework = NULL;
	m_pPropertyPage = NULL;
	m_pPropPageMgr = NULL;
	m_pCopyDataObject = NULL;
	m_pTimeSigStrip = NULL;
	m_pIDMTrack = NULL;
	m_pISegmentNode = NULL;
	m_fHaveStyleRefStrip = FALSE;

	// Initially we don't need to be saved
	m_fDirty = FALSE;

	// By default, belong to Group 1
	m_dwGroupBits = 1;

	// Initialize the track flags
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
	m_dwProducerOnlyFlags = 0;

	// Create a strip
	m_pTimeSigStrip = new CTimeSigStrip(this);
	ASSERT( m_pTimeSigStrip );
}

CTimeSigMgr::~CTimeSigMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Delete all the TimeSigs in m_lstTimeSigs
	EmptyTimeSigList();

	// Clean up our references
	if( m_pTimeSigStrip )
	{
		m_pTimeSigStrip->Release();
		m_pTimeSigStrip = NULL;
	}

	if( m_pDMProdFramework )
	{
		m_pDMProdFramework->Release();
		m_pDMProdFramework = NULL;
	}

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
// CTimeSigMgr ITimeSigMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::IsMeasureOpen

// Return: S_OK - Yes.  S_FALSE - No

HRESULT STDMETHODCALLTYPE CTimeSigMgr::IsMeasureOpen( DWORD dwMeasure )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get maximum number of measures
	long lMaxMeasure;
	VARIANT var;
	m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits,
									  0,
									   V_I4( &var ),
									  &lMaxMeasure,
									  NULL );
	// Ensure lMaxMeasure is at least 1
	lMaxMeasure = max( 1, lMaxMeasure );

	if( dwMeasure >= (unsigned)lMaxMeasure )
	{
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos );
		if( pTimeSigItem->m_dwMeasure > dwMeasure )
		{
			pos = NULL;
		}
		else if( (pTimeSigItem->m_dwMeasure == dwMeasure) && !(pTimeSigItem->m_dwBits & UD_FAKE) )
		{
			hr = S_FALSE;
			pos = NULL;
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr IDMUSProdStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::GetParam

HRESULT STDMETHODCALLTYPE CTimeSigMgr::GetParam(
		/* [in] */	REFGUID 	guidType,
		/* [in] */	MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME* pmtNext,
		/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pData != NULL );
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( !m_fHaveStyleRefStrip && (guidType == GUID_TimeSignature) )
	{
		// Initialize pmtNext
		if( pmtNext )
		{
			*pmtNext = 0;
		}

		// Search through TimeSig list for first valid time signature
		POSITION pos = m_lstTimeSigs.GetHeadPosition();
		pos = GetFirstValidTimeSig( pos, m_lstTimeSigs );

		if( !pos )
		{
			// No valid TimeSigs in this track.
			return DMUS_E_NOT_FOUND;
		}

		// Initialize Previous time signature
		CTimeSigItem* pPrevious = m_lstTimeSigs.GetAt( pos );

		MUSIC_TIME mtSRTime = 0;
		DWORD dwCurrentMeasure = 0;

		while( pos )
		{
			CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos );

			// This should be a real TimeSig
			ASSERT( !(pTimeSigItem->m_dwBits & UD_FAKE) );

			// Get the length of time between the previous time sig and this one
			long lSpan = ClocksPerMeasure(pPrevious->m_TimeSignature) * 
					(MUSIC_TIME) (pTimeSigItem->m_dwMeasure - dwCurrentMeasure);

			// If this time sig is beyond the requested time, break out of the loop
			if( mtSRTime + lSpan > mtTime )
			{
				// Set next
				if( pmtNext )
				{
					*pmtNext = mtSRTime + lSpan - mtTime;
				}
				break;
			}

			mtSRTime += lSpan;
			dwCurrentMeasure = pTimeSigItem->m_dwMeasure;
			pPrevious = pTimeSigItem;

			// Get the next valid Time Signature
			pos = GetFirstValidTimeSig( pos, m_lstTimeSigs );
		}

		// We should have found a valid TimeSig
		ASSERT( pPrevious );

		// Return the TimeSig we found
		DMUS_TIMESIGNATURE* pTS = (DMUS_TIMESIGNATURE*) pData;
		*pTS = pPrevious->m_TimeSignature;
		return S_OK;
	}

	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		IDMUSProdNode** ppIDocRootNode = (IDMUSProdNode **)pData;

		*ppIDocRootNode = m_pISegmentNode;
		if( m_pISegmentNode )
		{
			m_pISegmentNode->AddRef();
		}
		return S_OK;
	}

	if( ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		BSTR bstr;
		CString str;
		str.LoadString(m_pTimeSigStrip->m_nLastEdit);
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
// CTimeSigMgr::SetParam

HRESULT STDMETHODCALLTYPE CTimeSigMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(mtTime);

	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		IDMUSProdNode* pIDocRootNode = (IDMUSProdNode *)pData;

		m_pISegmentNode = pIDocRootNode;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::IsParamSupported

// support GUID_TimeSignature for maintaining the time signature
// support GUID_DocRootNode for maintaining pointer to DocRoot node
HRESULT STDMETHODCALLTYPE CTimeSigMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (!m_fHaveStyleRefStrip && ::IsEqualGUID( guidType, GUID_TimeSignature ))
	||  ::IsEqualGUID( guidType, GUID_DocRootNode )
	||  ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		return S_OK;
	}
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CTimeSigMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsEqualGUID( GUID_IDirectMusicStyle, rguidType) ||
		::IsEqualGUID( GUID_TimeSignature, rguidType) ||
		::IsEqualGUID( GUID_Segment_AllTracksAdded, rguidType) )
	{
		UpdateHaveStyleFlag();
		return S_OK;
	}

	// Track deleted
	if( ::IsEqualGUID( rguidType, GUID_Segment_DeletedTrack ) )
	{
		// Notify other StripMgrs that the time signature may have changed
		IDMUSProdTimeline *pTimeline = (IDMUSProdTimeline *)pData;
		if( pTimeline )
		{
			pTimeline->NotifyStripMgrs( GUID_TimeSignature, dwGroupBits, NULL );
		}
		return S_OK;
	}

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CTimeSigMgr::GetStripMgrProperty(
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

			pioTrackHeader->guidClassID = CLSID_DirectMusicTimeSigTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = NULL;
			pioTrackHeader->fccType = DMUS_FOURCC_TIMESIGTRACK_LIST;
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
// CTimeSigMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CTimeSigMgr::SetStripMgrProperty(
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
			m_pTimeline->RemoveFromNotifyList( this, GUID_TimeSignature, m_dwGroupBits );
			m_pTimeline->RemoveFromNotifyList( this, GUID_IDirectMusicStyle, m_dwGroupBits );
			m_pTimeline->RemoveFromNotifyList( this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
			if ( m_pTimeSigStrip )
			{
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)m_pTimeSigStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pTimeSigStrip );
			}
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
				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pTimeSigStrip, CLSID_DirectMusicTimeSigTrack, m_dwGroupBits, 0 );
				m_pTimeline->AddToNotifyList( this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( this, GUID_IDirectMusicStyle, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( this, GUID_TimeSignature, m_dwGroupBits );
				UpdateHaveStyleFlag();
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
// CTimeSigMgr IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::GetClassID

HRESULT CTimeSigMgr::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pClsId == NULL )
	{
		return E_POINTER;
	}

	// return our CLSID
	memcpy( pClsId, &CLSID_TimeSigMgr, sizeof( CLSID ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::IsDirty

HRESULT CTimeSigMgr::IsDirty()
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
// CTimeSigMgr::Load

HRESULT CTimeSigMgr::Load( IStream* pIStream )
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

	// Remove all existing time signatures
	EmptyTimeSigList();

	// Load the Track
	MMCKINFO ck;
	MMCKINFO ckList;
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_TIMESIGNATURE_TRACK: // TimeSig Chunk
				LoadTimeSigChunk( pIStream, ck.cksize, FALSE, 0 );
				hr = S_OK;
				break;

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_TIMESIGTRACK_LIST:
				        ckList.ckid = 0;
						ckList.fccType = 0;
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{				
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_TIMESIG_CHUNK :
									LoadTimeSigChunk( pIStream, ckList.cksize, FALSE, 0 );
									hr = S_OK;
									break;
							}    

							pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

	pIRiffStream->Release();

	SyncWithDirectMusic();

	return hr;
}


HRESULT CTimeSigMgr::LoadTimeSigChunk( IStream* pIStream,
										long lSize,
										BOOL fPaste,
										MUSIC_TIME mtPasteTime )
{
	BOOL fChanged = FALSE;

	// Validate pIStream
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	// Read in the size of the TimeSig structure
	DWORD cb, dwTimeSigSize;
	HRESULT hr = pIStream->Read( &dwTimeSigSize, sizeof( DWORD ), &cb );
	if( FAILED(hr) || cb != sizeof( DWORD ) )
	{
		if (SUCCEEDED(hr)) hr = E_FAIL;
		goto ON_END;
	}

	// Subtract off the four bytes we just read
	lSize -= sizeof(DWORD);

	// Compute how many bytes (if any) we'll need to skip
	ASSERT( dwTimeSigSize >= sizeof(DMUS_IO_TIMESIGNATURE_ITEM) );
	DWORD dwSkipBytes;
	dwSkipBytes = 0;
	if( dwTimeSigSize > sizeof(DMUS_IO_TIMESIGNATURE_ITEM) )
	{
		dwSkipBytes = dwTimeSigSize - sizeof(DMUS_IO_TIMESIGNATURE_ITEM);
	}

	// Ensure that the chunk size is an even number of TimeSig items
	ASSERT( (lSize % dwTimeSigSize) == 0 );

	if( fPaste )
	{
		DWORD dwCurrentFilePos = StreamTell( pIStream );
		long lTmpSize = lSize;

		MUSIC_TIME mtAdjustment = 0;;

		ASSERT( m_pTimeline != NULL );
		if( m_pTimeline )
		{
			// Get Timeline length
			VARIANT var;
			m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
			MUSIC_TIME mtMaxTimelineLength = V_I4( &var );

			// Determine new paste time to enforce boundaries
			while ( lTmpSize >= (signed)dwTimeSigSize )
			{
				// Read in a TimeSignature item
				DMUS_IO_TIMESIGNATURE_ITEM iTimeSigItem;
				DWORD cb;
				hr = pIStream->Read( &iTimeSigItem, sizeof( DMUS_IO_TIMESIGNATURE_ITEM ), &cb );
				if (FAILED(hr) || cb != sizeof( DMUS_IO_TIMESIGNATURE_ITEM ) ) 
				{
					if (SUCCEEDED(hr)) hr = E_FAIL;
					goto ON_END;
				}

				// If we need to, skip some bytes
				if( dwSkipBytes )
				{
					hr = StreamSeek( pIStream, dwSkipBytes, STREAM_SEEK_CUR );
					if( FAILED(hr) )
					{
						goto ON_END;
					}
				}

				// Subtract off the amount we read in
				lTmpSize -= dwTimeSigSize;

				// Now, figure out if we need to adjust the start time of the paste
				MUSIC_TIME mtTime = iTimeSigItem.lTime;
				mtTime += mtPasteTime;
				if( mtTime < 0 )
				{
					mtTime = 0 - mtTime;
					if( mtTime > mtAdjustment )
					{
						mtAdjustment = mtTime;
					}
				}
				else if( mtTime >= mtMaxTimelineLength )
				{
					mtTime = mtMaxTimelineLength - mtTime;
					if( mtTime < mtAdjustment )
					{
						mtAdjustment = mtTime - 1;
					}
				}
			}
		}

		// New paste time which will enforce strip boundaries
		mtPasteTime += mtAdjustment;

		// Restore our position back to the start of the TimeSigs
		StreamSeek( pIStream, dwCurrentFilePos, STREAM_SEEK_SET );
	}

	while ( lSize )
	{
		DMUS_IO_TIMESIGNATURE_ITEM iTimeSigItem;
		DWORD cb;
		hr = pIStream->Read( &iTimeSigItem, sizeof( DMUS_IO_TIMESIGNATURE_ITEM ), &cb );
		if (FAILED(hr) || cb != sizeof( DMUS_IO_TIMESIGNATURE_ITEM ) ) 
		{
			if (SUCCEEDED(hr)) hr = E_FAIL;
			goto ON_END;
		}

		// If we need to, skip some bytes
		if( dwSkipBytes )
		{
			hr = StreamSeek( pIStream, dwSkipBytes, STREAM_SEEK_CUR );
			if( FAILED(hr) )
			{
				goto ON_END;
			}
		}

		// Subtract off the amount we read in
		lSize -= dwTimeSigSize;

		CTimeSigItem* pItem = new CTimeSigItem( this );
		if( pItem == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto ON_END;
		}

		MUSIC_TIME mtTime = iTimeSigItem.lTime;
		if( fPaste )
		{
			mtTime += mtPasteTime;
		}

		// Copy TimeSignature to the pItem
		pItem->m_TimeSignature.bBeat = iTimeSigItem.bBeat;
		pItem->m_TimeSignature.bBeatsPerMeasure = iTimeSigItem.bBeatsPerMeasure;
		pItem->m_TimeSignature.wGridsPerBeat = iTimeSigItem.wGridsPerBeat;
		if( pItem->m_TimeSignature.wGridsPerBeat == 0 )
		{
			pItem->m_TimeSignature.wGridsPerBeat = 2;
		}
		if( pItem->m_TimeSignature.bBeat == 0 )
		{
			pItem->m_TimeSignature.bBeat = 2;
		}
		if( pItem->m_TimeSignature.bBeatsPerMeasure == 0 )
		{
			pItem->m_TimeSignature.bBeatsPerMeasure = 4;
		}

		// Set the measure this TimeSig belongs to
		if( !GetFirstValidTimeSig( m_lstTimeSigs.GetHeadPosition(), m_lstTimeSigs ) )
		{
			// If no valid TimeSigs, we need to use the TimeSig of this item to compute the measure #
			pItem->m_dwMeasure = mtTime / ClocksPerMeasure( pItem->m_TimeSignature );
		}
		else
		{
			// Have valid TimeSigs, can use ClocksToMeasure
			ClocksToMeasure( mtTime, (DWORD&)pItem->m_dwMeasure );
		}
		ASSERT( pItem->m_dwMeasure >= 0 );

		// Insert it into the list
		InsertByAscendingTime( pItem );
		fChanged = TRUE;

		// If pasting, select the item
		if( fPaste )
		{
			pItem->SetSelectFlag( TRUE );
		}
	}

ON_END:
	if( SUCCEEDED( hr )
	&&  fChanged == FALSE )
	{
		hr = S_FALSE;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::Save

HRESULT CTimeSigMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(fClearDirty);

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo	StreamInfo;
	FileType ftFileType = FT_DESIGN;	// Default to FT_DESIGN so clipboard
										// gets proper file ref chunk
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
	if( !(::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ))
	&&  !(::IsEqualGUID( guidDataFormat, GUID_CurrentVersion )) )
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
    MMCKINFO ck;

	// Create the track list chunk
	ckMain.fccType = DMUS_FOURCC_TIMESIGTRACK_LIST;
	if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// If the TimeSig list isn't empty, save it
	if ( !m_lstTimeSigs.IsEmpty() )
	{
		// Create a chunk to store the TimeSig data
		ck.ckid = DMUS_FOURCC_TIMESIG_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		DWORD cb, dwTimeSigSize = sizeof( DMUS_IO_TIMESIGNATURE_ITEM );
		hr = pIStream->Write( &dwTimeSigSize, sizeof( DWORD ), &cb );
		if( FAILED(hr) || cb != sizeof(DWORD) )
		{
			if(SUCCEEDED(hr)) hr = E_FAIL;
			goto ON_ERROR;
		}

		POSITION pos = m_lstTimeSigs.GetHeadPosition();
		while( pos )
		{
			CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos );
			if( !(pTimeSigItem->m_dwBits & UD_FAKE) )
			{
				DMUS_IO_TIMESIGNATURE_ITEM oTimeSigItem;

				oTimeSigItem.lTime = MeasureToClocks( pTimeSigItem->m_dwMeasure );
				oTimeSigItem.bBeatsPerMeasure = pTimeSigItem->m_TimeSignature.bBeatsPerMeasure;
				oTimeSigItem.bBeat = pTimeSigItem->m_TimeSignature.bBeat;
				oTimeSigItem.wGridsPerBeat = pTimeSigItem->m_TimeSignature.wGridsPerBeat;

				hr = pIStream->Write( &oTimeSigItem, sizeof( DMUS_IO_TIMESIGNATURE_ITEM ), &cb );
				if( FAILED(hr) || cb != sizeof(DMUS_IO_TIMESIGNATURE_ITEM) )
				{
					if(SUCCEEDED(hr)) hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}
		
		// Ascend out of the TimeSig chunk.
		pIRiffStream->Ascend( &ck, 0 );
	}
		
	// Ascend out of the track list chunk.
	pIRiffStream->Ascend( &ckMain, 0 );

ON_ERROR:
	pIRiffStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::SyncWithDirectMusic

HRESULT CTimeSigMgr::SyncWithDirectMusic( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pIDMTrack == NULL )
	{
		return E_FAIL;
	}

	IStream* pIMemStream;

	// Persist the strip into a stream
	HRESULT hr = m_pDMProdFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );
	if( SUCCEEDED ( hr ) )
	{
		hr = Save( pIMemStream, FALSE );
		if( SUCCEEDED ( hr ) )
		{
			IPersistStream* pIPersistStream;
			hr = m_pIDMTrack->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
			if( SUCCEEDED ( hr ) )
			{
				// Load into DirectMusic track
				StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
				hr = pIPersistStream->Load( pIMemStream );

				pIPersistStream->Release();
			}
		}

		pIMemStream->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::GetSizeMax

HRESULT CTimeSigMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	UNREFERENCED_PARAMETER(pcbSize);
	// Compute size of stream needed to persist ourself into.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::GetData

// This method is called by CTimeSigPropPageMgr to get data to send to the
// TimeSig property page.
// The CTimeSigStrip::GetData() method is called by CTimeSigStripPropPageMgr
// to get the strip's properties (Group Bits, etc.)
HRESULT STDMETHODCALLTYPE CTimeSigMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected TimeSig
	BOOL fMultipleSelect = FALSE;
	CTimeSigItem* pFirstTimeSigItem = NULL;

	CTimeSigItem* pTimeSigItem;
	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		pTimeSigItem = m_lstTimeSigs.GetNext( pos );
		if( pTimeSigItem->m_fSelected
		&&  !(pTimeSigItem->m_dwBits & UD_FAKE) )
		{
			pFirstTimeSigItem = pTimeSigItem;
			while( pos )
			{
				pTimeSigItem = m_lstTimeSigs.GetNext( pos );
				if( pTimeSigItem->m_fSelected
				&&  !(pTimeSigItem->m_dwBits & UD_FAKE) )
				{
					fMultipleSelect = TRUE;
					pos = NULL;
					break;
				}
			}
			break;
		}
	}

	*ppData = NULL;
	HRESULT hr = E_FAIL;

	// Multiple TimeSigs selected
	if( fMultipleSelect )
	{
		CPropTimeSig* pPropTimeSig = new CPropTimeSig;
		if( pPropTimeSig )
		{
			pPropTimeSig->m_dwMeasure = 0xFFFFFFFF;		// Signifies multiple TimeSigs selected
			*ppData = pPropTimeSig;
			hr = S_OK;
		}
	}

	// One TimeSig selected
	else if( pFirstTimeSigItem )
	{
		CPropTimeSig* pPropTimeSig = new CPropTimeSig( pFirstTimeSigItem );
		if( pPropTimeSig )
		{
			if( m_fHaveStyleRefStrip )
			{
				pPropTimeSig->m_dwBits |= UD_STYLEUPDATE;
			}
			*ppData = pPropTimeSig;
			hr = S_OK;
		}
	}

	// Nothing selected
	else
	{
		*ppData = NULL;
		hr = S_OK;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::SetData

// This method is called by CTimeSigPropPageMgr in response to user actions
// in the TimeSig Property page.  It changes the currenly selected TimeSig. 
HRESULT STDMETHODCALLTYPE CTimeSigMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected timesig
	CTimeSigItem* pTimeSigItem = FirstSelectedRealTimeSig();

	if( pTimeSigItem )
	{
		CPropTimeSig* pTimeSig = (CPropTimeSig*)pData;
		BOOL fChanged = FALSE;

		// Check if measure # changed
		if( pTimeSigItem->m_dwMeasure != pTimeSig->m_dwMeasure )
		{
			pTimeSigItem->m_dwMeasure = pTimeSig->m_dwMeasure;
			fChanged = TRUE;
			m_pTimeSigStrip->m_nLastEdit = IDS_UNDO_MOVE;

			// Re-insert the Time Sig into the list, since its measure info 
			// changed and it may now be out of order w.r.t. the other elements.
			if( RemoveItem( pTimeSigItem ) )
			{
				InsertByAscendingTime( pTimeSigItem );
			}
		}

		// Check if TimeSig changed
		if( (pTimeSigItem->m_TimeSignature.bBeat != pTimeSig->m_TimeSignature.bBeat) ||
			(pTimeSigItem->m_TimeSignature.bBeatsPerMeasure != pTimeSig->m_TimeSignature.bBeatsPerMeasure) ||
			(pTimeSigItem->m_TimeSignature.wGridsPerBeat != pTimeSig->m_TimeSignature.wGridsPerBeat) )
		{
			pTimeSigItem->m_TimeSignature = pTimeSig->m_TimeSignature;
			fChanged = TRUE;
			m_pTimeSigStrip->m_nLastEdit = IDS_CHANGE;
		}

		if( fChanged )
		{
			// Since it changed, make it into a 'real' Time signatures
			pTimeSigItem->m_dwBits &= ~UD_FAKE;

			// Redraw the timesig strip
			// BUGBUG: Should be smarter and only redraw the time sig that changed
			m_pTimeline->StripInvalidateRect( m_pTimeSigStrip, NULL, TRUE );

			// Let our hosting editor know about the changes
			OnDataChanged();

			SyncWithDirectMusic();

			// Notify the other strips of possible TimeSig change
			m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_dwGroupBits, NULL );
			return S_OK;
		}
	}

	// Nothing changed, or no items are selected
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CTimeSigMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		CTimeSigPropPageMgr* pPPM = new CTimeSigPropPageMgr(m_pDMProdFramework);
		if( pPPM == NULL )
		{
			return E_OUTOFMEMORY;
		}

		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		
		m_pPropPageMgr->SetObject( this );

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
// CTimeSigMgr::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CTimeSigMgr::OnRemoveFromPageManager( void)
{
	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::OnDataChanged

HRESULT STDMETHODCALLTYPE CTimeSigMgr::OnDataChanged( void)
{
	ASSERT( m_pTimeline );
	if ( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Let our hosting editor know about the change
	m_pTimeline->OnDataChanged( (ITimeSigMgr*)this );

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::DeleteSelectedTimeSigs

HRESULT CTimeSigMgr::DeleteSelectedTimeSigs()
{
	CTimeSigItem* pTimeSigItem;
	POSITION pos2, pos1 = m_lstTimeSigs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		pTimeSigItem = m_lstTimeSigs.GetNext( pos1 );
		if( pTimeSigItem->m_fSelected
		|| (pTimeSigItem->m_dwBits & UD_FAKE) )
		{
			m_lstTimeSigs.RemoveAt( pos2 );
			delete pTimeSigItem;
		}
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::SaveSelectedTimeSigs

HRESULT CTimeSigMgr::SaveSelectedTimeSigs(LPSTREAM pIStream, MUSIC_TIME mtOffset)
{
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// If the TimeSig list has anything in it, look for selected TimeSigs
	if ( !m_lstTimeSigs.IsEmpty() )
	{
		DWORD cb, dwTimeSigSize = sizeof( DMUS_IO_TIMESIGNATURE_ITEM );
		hr = pIStream->Write( &dwTimeSigSize, sizeof( DWORD ), &cb );
		if( FAILED(hr) || cb != sizeof(DWORD) )
		{
			if(SUCCEEDED(hr)) hr = E_FAIL;
			goto ON_ERROR;
		}

		POSITION pos = m_lstTimeSigs.GetHeadPosition();
		while( pos )
		{
			CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos );
			if(  pTimeSigItem->m_fSelected
			&& !(pTimeSigItem->m_dwBits & UD_FAKE) )
			{
				DMUS_IO_TIMESIGNATURE_ITEM oTimeSigItem;

				oTimeSigItem.lTime = MeasureToClocks( pTimeSigItem->m_dwMeasure ) - mtOffset;
				oTimeSigItem.bBeatsPerMeasure = pTimeSigItem->m_TimeSignature.bBeatsPerMeasure;
				oTimeSigItem.bBeat = pTimeSigItem->m_TimeSignature.bBeat;
				oTimeSigItem.wGridsPerBeat = pTimeSigItem->m_TimeSignature.wGridsPerBeat;

				hr = pIStream->Write( &oTimeSigItem, sizeof( DMUS_IO_TIMESIGNATURE_ITEM ), &cb );
				if( FAILED(hr) || cb != sizeof(DMUS_IO_TIMESIGNATURE_ITEM) )
				{
					if(SUCCEEDED(hr)) hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}
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
// CTimeSigMgr::CreateTimeSig

HRESULT CTimeSigMgr::CreateTimeSig( MUSIC_TIME mtTime, CTimeSigItem*& rpTimeSig )
{
	rpTimeSig = NULL;

	if( m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	rpTimeSig = new CTimeSigItem( this );
	if( rpTimeSig == NULL )
	{
		return E_OUTOFMEMORY;
	}

	long lMeasure = 0;

	HRESULT hr;
	hr = m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, NULL );
	ASSERT( SUCCEEDED ( hr ) );

	rpTimeSig->m_TimeSignature.bBeat = 4;
	rpTimeSig->m_TimeSignature.bBeatsPerMeasure = 4;
	rpTimeSig->m_TimeSignature.wGridsPerBeat = 4;

	rpTimeSig->m_dwMeasure = lMeasure;

	//rpTimeSig->SetSelectFlag( TRUE );
	InsertByAscendingTime( rpTimeSig );

	// Mark this time sig as 'fake'
	rpTimeSig->m_dwBits |= UD_FAKE;

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::MarkSelectedTimeSigs

// marks m_dwUndermined field CTimeSigItems in list
void CTimeSigMgr::MarkSelectedTimeSigs( DWORD dwFlags )
{
	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos );
		if ( pTimeSigItem->m_fSelected )
		{
			pTimeSigItem->m_dwBits |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::DeleteMarked

// deletes TimeSigs marked by given flag
void CTimeSigMgr::DeleteMarked( DWORD dwFlags )
{
	POSITION pos2, pos1 = m_lstTimeSigs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos1 );
		if( (pTimeSigItem->m_dwBits & dwFlags)
		||  (pTimeSigItem->m_dwBits & UD_FAKE) )
		{
			m_lstTimeSigs.RemoveAt( pos2 );
			delete pTimeSigItem;
		}
	}

	// Update the property page
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::UnMarkTimeSigs

// unmarks flag m_dwUndermined field CTimeSigItems in list
void CTimeSigMgr::UnMarkTimeSigs( DWORD dwFlags )
{
	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		m_lstTimeSigs.GetNext( pos )->m_dwBits &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::UnselectAllKeepBits

void CTimeSigMgr::UnselectAllKeepBits()
{
	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		m_lstTimeSigs.GetNext( pos )->m_fSelected = FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::UnselectAll

BOOL CTimeSigMgr::UnselectAll()
{
	BOOL fChange = FALSE; // Flag notifying the caller on whether they need to redraw

	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		CTimeSigItem *pItem = m_lstTimeSigs.GetNext( pos );
		if( pItem->m_fSelected || (pItem->m_dwBits & UD_DRAGSELECT) )
		{
			pItem->SetSelectFlag( FALSE );
			fChange = TRUE;
		}
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::SelectAll

void CTimeSigMgr::SelectAll()
{
	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos );
		// Only select "real" TimeSigs
		pTimeSigItem->SetSelectFlag( !(pTimeSigItem->m_dwBits & UD_FAKE) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::IsSelected

BOOL CTimeSigMgr::IsSelected()
{
	// If anything "real" is selected, return TRUE.
	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos );
		if( pTimeSigItem->m_fSelected
		&&  !(pTimeSigItem->m_dwBits & UD_FAKE) )
		{
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::FirstSelectedTimeSig

CTimeSigItem* CTimeSigMgr::FirstSelectedTimeSig()
{
	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos );
		if ( pTimeSigItem->m_fSelected )
		{
			return pTimeSigItem;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::FirstSelectedReaTimeSig

CTimeSigItem* CTimeSigMgr::FirstSelectedRealTimeSig()
{
	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos );

		if(  pTimeSigItem->m_fSelected
		&& !(pTimeSigItem->m_dwBits & UD_FAKE) )
		{
			return pTimeSigItem;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::SelectSegment

BOOL CTimeSigMgr::SelectSegment(long lBeginTime, long lEndTime)
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

	long lBeginMeas, lEndMeas;
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lBeginTime, &lBeginMeas, NULL );
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lEndTime, &lEndMeas, NULL );

	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos );
		pTimeSigItem->SetSelectFlag( FALSE );
		if( (DWORD)lBeginMeas < pTimeSigItem->m_dwMeasure && pTimeSigItem->m_dwMeasure < (DWORD)lEndMeas )
		{
			pTimeSigItem->SetSelectFlag( TRUE );
			result = TRUE;
		}
		else if( (DWORD)lBeginMeas == pTimeSigItem->m_dwMeasure )
		{
			if( (DWORD)lEndMeas == pTimeSigItem->m_dwMeasure )
			{
				pTimeSigItem->SetSelectFlag( TRUE );
				result = TRUE;
			}
			else
			{
				pTimeSigItem->SetSelectFlag( TRUE );
				result = TRUE;
			}
		}
		else if( (DWORD)lEndMeas == pTimeSigItem->m_dwMeasure )
		{
			pTimeSigItem->SetSelectFlag( TRUE );
			result = TRUE;
		}
	}
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::EmptyTimeSigList

void CTimeSigMgr::EmptyTimeSigList(void)
{
	if( !m_lstTimeSigs.IsEmpty() )
	{
		while ( !m_lstTimeSigs.IsEmpty() )
		{
			delete m_lstTimeSigs.RemoveHead();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::InsertByAscendingTime

void CTimeSigMgr::InsertByAscendingTime( CTimeSigItem *pTimeSig )
{
	ASSERT( pTimeSig );
	if ( pTimeSig == NULL )
	{
		return;
	}

	POSITION pos2, pos1 = m_lstTimeSigs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetNext( pos1 );
		if( pTimeSigItem->m_dwMeasure == pTimeSig->m_dwMeasure )
		{
			// Always replace item
			m_lstTimeSigs.InsertBefore( pos2, pTimeSig );
			m_lstTimeSigs.RemoveAt( pos2 );
			delete pTimeSigItem;
			return;
		}
		if( pTimeSigItem->m_dwMeasure > pTimeSig->m_dwMeasure )
		{
			// insert before pos2 (current position of pTimeSigItem)
			m_lstTimeSigs.InsertBefore( pos2, pTimeSig );
			return;
		}
	}
	// insert at end of list
	m_lstTimeSigs.AddTail( pTimeSig );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::RemoveItem

BOOL CTimeSigMgr::RemoveItem( CTimeSigItem* pItem )
{
	POSITION pos2;
	POSITION pos1 = m_lstTimeSigs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		if ( m_lstTimeSigs.GetNext( pos1 ) == pItem )
		{
			m_lstTimeSigs.RemoveAt( pos2 );
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::GetBoundariesOfSelectedTimeSigs

void CTimeSigMgr::GetBoundariesOfSelectedTimeSigs( long *plStart, long *plEnd )
{
	ASSERT( plStart );
	ASSERT( plEnd );

	HRESULT hr;
	long lClocks;
	long lEnd = -1;
	BOOL fSetStart = FALSE;

	POSITION pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		CTimeSigItem* pItem = m_lstTimeSigs.GetNext( pos );

		if( pItem->m_fSelected ) 
//		&&	!(pTimeSigItem->m_dwBits & UD_FAKE) )
		{
			hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits,
												   0,
												   pItem->m_dwMeasure,
												   0,
												   &lClocks );
			ASSERT( SUCCEEDED ( hr ) );
			if( SUCCEEDED ( hr ) )
			{
				if( lEnd < lClocks )
				{
					lEnd = lClocks;
				}
				if( !fSetStart )
				{
					fSetStart = TRUE;
					*plStart = lClocks;
				}
			}
		}
	}

	if( lEnd >= 0 )
	{
		if( lEnd <= *plStart )
		{
			lEnd = *plStart + 1;
		}
		*plEnd = lEnd;
	}
	else
	{
		*plStart = -1;
		*plEnd = -1;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::DeleteBetweenTimes

BOOL CTimeSigMgr::DeleteBetweenTimes( long lStart, long lEnd )
{
	HRESULT hr;
	long lClocks;

	BOOL fChanged = FALSE;

	// Iterate through the list
	CTimeSigItem* pItem;
	POSITION pos2, pos = m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		// Save the current position
		pos2 = pos;
		pItem = m_lstTimeSigs.GetNext( pos );

		hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits,
											   0,
											   pItem->m_dwMeasure,
											   0,
											   &lClocks );
		ASSERT( SUCCEEDED ( hr ) );
		if( SUCCEEDED ( hr ) )
		{
			// If the TimeSig occurs between lStart and lEnd, delete it
			if( (lClocks >= lStart) && (lClocks <= lEnd) ) 
			{
				m_lstTimeSigs.RemoveAt( pos2 );
				if( !(pItem->m_dwBits & UD_FAKE) )
				{
					fChanged = TRUE;
				}
				delete pItem;
			}
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr::UpdateHaveStyleFlag

void CTimeSigMgr::UpdateHaveStyleFlag( void )
{
	BOOL fOrigHaveStyleRefStrip = m_fHaveStyleRefStrip;

	IDMUSProdStripMgr *pIStripMgr = NULL;
	if( m_pTimeline &&
		SUCCEEDED( m_pTimeline->GetStripMgr( GUID_IDirectMusicStyle, m_dwGroupBits, 0, &pIStripMgr ) ) )
	{
		m_fHaveStyleRefStrip = TRUE;
		
		// Remove all unselected items
		POSITION pos2, pos = m_lstTimeSigs.GetHeadPosition();
		while ( pos )
		{
			pos2 = pos;
			if( !m_lstTimeSigs.GetNext( pos )->m_fSelected )
			{
				delete m_lstTimeSigs.GetAt( pos2 );
				m_lstTimeSigs.RemoveAt( pos2 );
			}
			else
			{
				m_lstTimeSigs.GetAt(pos2)->m_dwBits |= UD_STYLEUPDATE;
			}
		}

		// Add new TimeSigs from this Style track
		MUSIC_TIME mtCurrent = 0, mtNext = 1;
		pos = m_lstTimeSigs.GetHeadPosition();
		while( mtNext > 0 )
		{
			IDirectMusicStyle *pIDMStyle;
			// If first style isn't at time zero, we treat if as if it were at 0.
			if( SUCCEEDED( pIStripMgr->GetParam( GUID_IDirectMusicStyle, mtCurrent, &mtNext, &pIDMStyle ) ) )
			{
				// pIDMStyle will be NULL if mtCurrent is 0 and
				// there is NOT a style in Measure 1
				if( pIDMStyle )
				{
					long lMeasure;
					m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtCurrent, &lMeasure, NULL );

					CTimeSigItem *pItem = new CTimeSigItem(this);
					pItem->m_dwMeasure = lMeasure;

					DMUS_TIMESIGNATURE timeSig;
					pIDMStyle->GetTimeSignature( &timeSig );
					memcpy( &pItem->m_TimeSignature, &timeSig, sizeof(DMUS_TIMESIGNATURE) );

					// Insert the new item into the list
					while( pos )
					{
						CTimeSigItem* pTimeSigItem = m_lstTimeSigs.GetAt( pos );
						if( pTimeSigItem->m_dwMeasure == pItem->m_dwMeasure )
						{
							// replace item
							pItem->m_fSelected = pTimeSigItem->m_fSelected;
							m_lstTimeSigs.InsertBefore( pos, pItem );
							m_lstTimeSigs.RemoveAt( pos );
							delete pTimeSigItem;
							pos = m_lstTimeSigs.GetHeadPosition();
							break;
						}
						if( pTimeSigItem->m_dwMeasure > pItem->m_dwMeasure )
						{
							// insert before pos (current position of pTimeSigItem)
							m_lstTimeSigs.InsertBefore( pos, pItem );
							break;
						}
						// Get next item to check
						m_lstTimeSigs.GetNext( pos );
					}

					// insert at end of list
					if( pos == NULL )
					{
						m_lstTimeSigs.AddTail( pItem );
					}

					pIDMStyle->Release();
				}
				mtCurrent += mtNext;
			}
			else
			{
				mtNext = 0;
			}
		}

		// Mark all marked, valid TimeSig items as 'fake'
		pos = m_lstTimeSigs.GetHeadPosition();
		while ( pos )
		{
			pos2 = pos;
			if( m_lstTimeSigs.GetNext( pos )->m_dwBits & UD_STYLEUPDATE )
			{
				m_lstTimeSigs.GetAt( pos2 )->m_dwBits &= UD_STYLEUPDATE;
				m_lstTimeSigs.GetAt( pos2 )->m_dwBits |= UD_FAKE;
			}
		}

		pIStripMgr->Release();
	}
	else
	{
		m_fHaveStyleRefStrip = FALSE;
	}

	if( (fOrigHaveStyleRefStrip || m_fHaveStyleRefStrip) && m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
}
