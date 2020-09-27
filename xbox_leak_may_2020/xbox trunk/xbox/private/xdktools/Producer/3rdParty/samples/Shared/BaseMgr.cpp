/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// BaseMgr.cpp : implementation file
//

#include "stdafx.h"
#include "BaseMgr.h"
#include <dmusici.h>
#include <dmusicf.h>
#include "SegmentGuids.h"
#include "SegmentIO.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr constructor/destructor 

CBaseMgr::CBaseMgr()
{
	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pDMProdFramework = NULL;
	m_pIDocRootNode = NULL;
	m_pPropPageMgr = NULL;
	m_pCopyDataObject = NULL;
	m_pIDMTrack = NULL;
	m_pBaseStrip = NULL;

	// By default, belong to Group 1 only
	m_dwGroupBits = 1;
	m_dwOldGroupBits = 1;

	// Initially we don't need to be saved
	m_fDirty = FALSE;

	// Initially, no edits have happened
	m_nLastEdit = 0;

	// Initalize track flags
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT;
	m_dwProducerOnlyFlags = 0;

	// TODO: Derived class must create a strip and assign it to m_pBaseStrip
	// OR: Derived class must override all methods referencing m_pBaseStrip
}

CBaseMgr::~CBaseMgr()
{
	// Clean up our references
	if( m_pDMProdFramework )
	{
		m_pDMProdFramework->Release();
		m_pDMProdFramework = NULL;
	}

	ASSERT( m_pTimeline == NULL );
	ASSERT( m_pBaseStrip == NULL );

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
// CBaseMgr IDMUSProdStripMgr implementation


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::GetParam

HRESULT STDMETHODCALLTYPE CBaseMgr::GetParam(
		/* [in] */	REFGUID 	guidType,
		/* [in] */	MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME* pmtNext,
		/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(mtTime);
	UNREFERENCED_PARAMETER(pmtNext);

	ASSERT( pData != NULL );
	if( pData == NULL )
	{
		return E_POINTER;
	}

	// Check if the segment is asking for a string to display in the Undo and Redo edit menu items
	if( ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		// Try and load our current undo string
		CComBSTR comBSTR;
		if( comBSTR.LoadString( m_nLastEdit ) )
		{
			// Succeeded - return the BSTR
			*(BSTR*)pData = comBSTR.Detach();
			return S_OK;
		}
		return E_FAIL;
	}

	// Get DocRoot node of StripMgr's file (i.e. the Segment)
	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		IDMUSProdNode** ppIDocRootNode = (IDMUSProdNode **)pData;

		*ppIDocRootNode = m_pIDocRootNode;
		if( m_pIDocRootNode )
		{
			m_pIDocRootNode->AddRef();
		}
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::SetParam

HRESULT STDMETHODCALLTYPE CBaseMgr::SetParam(
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

		m_pIDocRootNode = pIDocRootNode;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::IsParamSupported

HRESULT STDMETHODCALLTYPE CBaseMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// We support returning text to display in the Undo and Redo edit menu items.
	// We support GUID_DocRootNode for maintaining pointer to our DocRoot node.
	if( ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) 
	||  ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		return S_OK;
	}
	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CBaseMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pData);

	// If the update isn't for our strip, exit
	if( !(dwGroupBits & m_dwGroupBits) )
	{
		return E_INVALIDARG;
	}

	// TimeSig change
	if( ::IsEqualGUID( rguidType, GUID_TimeSignature ) )
	{
		// Update the m_mtTime value of all items
		if( RecomputeTimes() )
		{
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->RefreshData();
			}
			OnDataChanged();
		}

		// Redraw our strip
		if( m_pBaseStrip )
		{
			m_pTimeline->StripInvalidateRect( m_pBaseStrip, NULL, TRUE );
		}
		return S_OK;
	}

	// All Tracks Added
	if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		// Update the m_mtTime value of all items
		RecomputeTimes();

		// Redraw our strip
		if( m_pBaseStrip )
		{
			m_pTimeline->StripInvalidateRect( m_pBaseStrip, NULL, TRUE );
		}
		return S_OK;
	}

	// We don't handle whichever notification was passed to us
	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CBaseMgr::GetStripMgrProperty(
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
		// Return a copy of our timeline pointer
		pVariant->vt = VT_UNKNOWN;
		if( m_pTimeline )
		{
			return m_pTimeline->QueryInterface( IID_IUnknown, (void**)&V_UNKNOWN( pVariant ) );
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;

	case SMP_IDIRECTMUSICTRACK:
		// Return a copy of our DirectMusic track pointer
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
		// Return a copy of our framework pointer
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
		// Check that the caller passed in a reference
		if( pVariant->vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			// The reference passed by the caller is a pointer to a DMUS_TRACK_HEADER structure.
			DMUS_IO_TRACK_HEADER *pioTrackHeader = static_cast<DMUS_IO_TRACK_HEADER *>(V_BYREF( pVariant ));
			if( pioTrackHeader == NULL )
			{
				return E_POINTER;
			}

			// Fill in the structure with our data
			pioTrackHeader->guidClassID = m_clsid;
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
		V_I4(pVariant) = g_dwTrackExtrasMask;
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
// CBaseMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CBaseMgr::SetStripMgrProperty(
		/* [in] */ STRIPMGRPROPERTY stripMgrProperty,
		/* [in] */ VARIANT		variant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	switch( stripMgrProperty )
	{
	case SMP_ITIMELINECTL:
		// Check that the caller passed in an IUnknown pointer
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}

		// If we were previously attached to a timeline
		if( m_pTimeline )
		{
			// Make sure our property page isn't displayed
			m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);

			// Release our reference on our property page manager
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->Release();
				m_pPropPageMgr = NULL;
			}

			// Remove our strip from the Timeline
			if ( m_pBaseStrip )
			{
				// Make sure our strip's property page isn't displayed
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)m_pBaseStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pBaseStrip );
			}

			// Remove ourself from the Timeline's notification list
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwOldGroupBits );

			// Release our reference on the Timeline
			m_pTimeline->Release();
			m_pTimeline = NULL;
		}

		// If a non-NULL pointer was passed in
		if( V_UNKNOWN( &variant ) )
		{
			// Query the pointer for a Timeline interface
			if( FAILED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pTimeline )))
			{
				return E_FAIL;
			}
			else
			{
				// Add the strip to the timeline
				ASSERT( m_pBaseStrip );
				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pBaseStrip, m_clsid, m_dwGroupBits, 0 );

				// Add ourself to the Timeline's notification list
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
			}
		}
		break;

	case SMP_IDIRECTMUSICTRACK:
		// Check that the caller passed in an IUnknown pointer
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}

		// If we have an existing DirectMusic track pointer, release it
		if( m_pIDMTrack )
		{
			m_pIDMTrack->Release();
			m_pIDMTrack = NULL;
		}

		// If we were passed a non-NULL pointer, query it for an IDirectMusicTrack interface
		if( V_UNKNOWN( &variant ) )
		{
			V_UNKNOWN( &variant )->QueryInterface( IID_IDirectMusicTrack, (void**)&m_pIDMTrack );
		}
		break;

	case SMP_IDMUSPRODFRAMEWORK:
		// Check that the caller passed in an IUnknown pointer
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}

		// If we have an existing Framework pointer, release it
		if( m_pDMProdFramework )
		{
			m_pDMProdFramework->Release();
			m_pDMProdFramework = NULL;
		}

		// If we were passed a non-NULL pointer, query it for an IDMUSProdFramework interface
		if( V_UNKNOWN( &variant ) )
		{
			return V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pDMProdFramework);
		}
		break;

	case SMP_DMUSIOTRACKHEADER:
		// Check that the caller passed in a reference
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			// The reference should be a pointer to a DMUS_IO_TRACK_HEADER structure
			DMUS_IO_TRACK_HEADER *pioTrackHeader = static_cast<DMUS_IO_TRACK_HEADER *>(V_BYREF( &variant ));
			if( pioTrackHeader == NULL )
			{
				return E_POINTER;
			}

			// Update our group bits settings
			// If your strip does not support changing its group bits, you must override
			// this method so that m_dwGroupBits and m_dwOldGroupBits do not get updated.
			m_dwGroupBits = pioTrackHeader->dwGroup;
			m_dwOldGroupBits = pioTrackHeader->dwGroup;

			// Ignore everything else in the structure, since we don't care about it
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
		// We don't support whichever property was given
		return E_INVALIDARG;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::GetClassID

HRESULT CBaseMgr::GetClassID( CLSID* pClsId )
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate pClsId
	if ( pClsId == NULL )
	{
		return E_POINTER;
	}

	// return our CLSID
	memcpy( pClsId, &m_clsid, sizeof( CLSID ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::IsDirty

HRESULT CBaseMgr::IsDirty()
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( m_fDirty )
	{
		return S_OK; // Dirty
	}
	else
	{
		return S_FALSE; // Clean
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::GetSizeMax

HRESULT CBaseMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pcbSize);

	// Compute size of stream needed to persist ourself into.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CBaseMgr::OnRemoveFromPageManager( void)
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::OnDataChanged

void CBaseMgr::OnDataChanged( void )
{
	// Verify that we have a valid pointer to the Timeline
	if ( m_pTimeline == NULL )
	{
		return;
	}

	// Let our hosting editor know about the change
	IUnknown *punkStripMgr;
	if( SUCCEEDED( QueryInterface( IID_IUnknown, (void **)&punkStripMgr ) ) )
	{
		m_pTimeline->OnDataChanged( punkStripMgr );
		punkStripMgr->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBaseMgr::SyncWithDirectMusic

HRESULT CBaseMgr::SyncWithDirectMusic( void )
{
	// If there is no DirectMusic Track, return S_FALSE
	if( m_pIDMTrack == NULL )
	{
		return S_FALSE;
	}

	// Create a memory stream to use
	IStream* pIMemStream = NULL;
	HRESULT hr = m_pDMProdFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );
	if( SUCCEEDED ( hr ) )
	{
		// Ensure the stream is at the beginning
		const LARGE_INTEGER liStart = {0,0};
		pIMemStream->Seek( liStart, STREAM_SEEK_SET, NULL );

		// Save our self
		hr = Save(pIMemStream, FALSE);
		if ( SUCCEEDED( hr ) )
		{
			// Query the DirectMusic Track for its IPersistStream interface
			IPersistStream* pIPersistStream = NULL;
			hr = m_pIDMTrack->QueryInterface(IID_IPersistStream, (void**)&pIPersistStream);
			if ( SUCCEEDED( hr ) )
			{
				// Seek back to the beginning of the stream
				pIMemStream->Seek( liStart, STREAM_SEEK_SET, NULL );

				// Load the DirectMusic Track with the new data
				hr = pIPersistStream->Load(pIMemStream);

				// If we have a Segment DocRoot Node pointer, initialize the DirectMusic Track
				if( m_pIDocRootNode )
				{
					// Get a pointer to the DirectMusic segment the track is in
					IDirectMusicSegment *pSegment;
					if( SUCCEEDED( m_pIDocRootNode->GetObject(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pSegment ) ) )
					{
						// Now, initialize the track.
						m_pIDMTrack->Init( pSegment );
						pSegment->Release();
					}
				}

				// Release the track's IPersistStream interface reference
				pIPersistStream->Release();
			}
		}

		// Release the reference to the memory stream
		pIMemStream->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// MeasureBeatToBeats

HRESULT MeasureBeatToBeats( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, DWORD dwIndex, long lMeasure, long lBeat, long &lNumBeats )
{
	// Validate parameters
	ASSERT( pITimeline );
	ASSERT( dwGroupBits );
	ASSERT( lMeasure >= 0 );
	ASSERT( lBeat >= 0 );

	if( NULL == pITimeline )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	// Initialize variables
	HRESULT hr;
	DMUS_TIMESIGNATURE TimeSig;
	MUSIC_TIME mtTSCur = 0, mtTSNext = 1;

	// Initialize value to return
	lNumBeats = 0;

	// Loop until lMeasure is zero
	do
	{
		// Get the time signature at mtTSCur
		hr = pITimeline->GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, mtTSCur, &mtTSNext, &TimeSig );
		if ( FAILED( hr ) )
		{
			return E_UNEXPECTED;
		}

		// Check if this time signature is valid forever
		if( mtTSNext == 0 )
		{
			// Just compute the number of beats to add and break out of the loop
			lNumBeats += lMeasure * TimeSig.bBeatsPerMeasure;
			break;
		}
		else
		{
			// Compute the number of clocks in a measure
			long lMeasureClocks = TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );

			// Compute the number of measures until the next time signature
			long lTmpMeasures = mtTSNext / lMeasureClocks;

			// If we won't reach the next time signature
			if( lMeasure <= lTmpMeasures )
			{
				// Just compute the number of beats to add and break out of the loop
				lNumBeats += lMeasure * TimeSig.bBeatsPerMeasure;
				break;
			}
			else
			{
				// Compute when to look for the next time signature
				mtTSCur += lMeasureClocks * lTmpMeasures;

				// Compute the number of beats to add
				lNumBeats += lTmpMeasures * TimeSig.bBeatsPerMeasure;

				// Update the number of measures we have left to traverse
				lMeasure -= lTmpMeasures;
			}
		}
	}
	while( lMeasure > 0 );

	// We've compute all the beats from the lMeasure paramter - now just add lBeat
	lNumBeats += lBeat;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// BeatsToMeasureBeat

HRESULT BeatsToMeasureBeat( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, DWORD dwIndex, long lNumBeats, long &lMeasure, long &lBeat )
{
	// Validate parameters
	ASSERT( pITimeline );
	ASSERT( dwGroupBits );
	ASSERT( lNumBeats >= 0 );

	if( NULL == pITimeline )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	// Initialize variables
	DMUS_TIMESIGNATURE TimeSig;
	MUSIC_TIME mtTSCur = 0, mtTSNext;
	lBeat = 0;
	lMeasure = 0;

	do
	{
		// Try and get the current time signature
		if ( FAILED( pITimeline->GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, mtTSCur, &mtTSNext, &TimeSig ) ) )
		{
			return E_UNEXPECTED;
		}

		// If there is no next time signature, do the math to find how many more measures to add
		if( mtTSNext == 0 )
		{
			lMeasure += lNumBeats / TimeSig.bBeatsPerMeasure;
			lNumBeats %= TimeSig.bBeatsPerMeasure;
			break;
		}
		// Otherwise it's more complicated
		else
		{
			// Compute the number of clocks in a beat
			long lBeatClocks = NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );

			// If the next time signature is after the time we're looking for
			if( lNumBeats < mtTSNext / lBeatClocks )
			{
				// Add the number of complete measures between here and there
				lMeasure += lNumBeats / TimeSig.bBeatsPerMeasure;

				// lNumBeats now stores an offset from the beginning of the measure
				lNumBeats %= TimeSig.bBeatsPerMeasure;
				break;
			}
			// The next time signature is before the time we're looking for
			else
			{
				// Compute how many complete measures there are between now and the next Time signature
				long lMeasureDiff= mtTSNext / (TimeSig.bBeatsPerMeasure * lBeatClocks);

				// Add them to lMeasure
				lMeasure += lMeasureDiff;

				// Subtract off the number of beats between mtTSCur and mtTSNext
				lNumBeats -= lMeasureDiff * TimeSig.bBeatsPerMeasure;

				// Change lMeasureDiff from measures to clocks
				lMeasureDiff *= TimeSig.bBeatsPerMeasure * lBeatClocks;

				// Add the clocks of the measures between mtTSCur and mtTSNext to mtTSCur
				mtTSCur += lMeasureDiff;
			}
		}
	}
	// While the beats left is greater than 0
	while ( lNumBeats > 0 );

	// Any leftover beats are assigned to lBeat
	lBeat = lNumBeats;

	return S_OK;
}
