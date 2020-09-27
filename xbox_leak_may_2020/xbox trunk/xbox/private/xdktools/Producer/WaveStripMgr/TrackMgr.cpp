// TrackMgr.cpp : implementation file
//

/*----------------
@doc WAVESAMPLE
----------------*/

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include "TabWaveTrack.h"
#include "TabWavePart.h"
#include "DLSDesigner.h"
#include "SegmentDesigner.h"
#include "SegmentIO.h"
#include "NewPartDlg.h"
#include <conductor.h>
#include <dmusicf.h>
#include <dmusici.h>

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Set information about this strip manager
const CLSID CTrackMgr::m_clsid = CLSID_DirectMusicWaveTrack;
const DWORD CTrackMgr::m_ckid = NULL;
const DWORD CTrackMgr::m_fccType = DMUS_FOURCC_WAVETRACK_LIST;

const DWORD g_dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
const DWORD g_dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr constructor/destructor 

CTrackMgr::CTrackMgr() : CBaseMgr()
{
	// DirectMusic wave track data
	m_lVolume = 0;
	m_dwTrackFlagsDM = DMUS_WAVETRACKF_PERSIST_CONTROL;
	m_pWaveStripForPropSheet = NULL;
	m_pIDMPerformance = NULL;
	m_pIDMSegmentState = NULL;
	m_hStatusBar = NULL;
	m_fInAllTracksAdded = false;
	m_fWasRuntimeTrack = false;

	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;

	CWaveStrip* pWaveStrip = new CWaveStrip( this );
	ASSERT( pWaveStrip );
	if( pWaveStrip )
	{
		m_lstWaveStrips.AddTail( pWaveStrip );
	}
}

CTrackMgr::~CTrackMgr()
{
	// Remove pane from status bar
	if( m_hStatusBar )
	{
		m_pDMProdFramework->RestoreStatusBar( m_hStatusBar );
		m_hStatusBar = NULL;
	}

	// Release all the items in m_lstWaveStrips
	while( !m_lstWaveStrips.IsEmpty() )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.RemoveHead();
		RELEASE( pWaveStrip );
	}

	UnloadQueuedWaves();
	ASSERT( m_lstQueuedWaves.IsEmpty() );

	RELEASE( m_pIDMPerformance );
	RELEASE( m_pIDMSegmentState );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr ITrackMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::IsMeasureBeatOpen
// Returns S_OK if the specified measure and beat is empty.
// Returns S_FALSE if the specified measure and beat already has an item

HRESULT STDMETHODCALLTYPE CTrackMgr::IsMeasureBeatOpen( long lMeasure, long lBeat )
{
	UNREFERENCED_PARAMETER(lMeasure);
	UNREFERENCED_PARAMETER(lBeat);
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( 0 );
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr IDMUSProdStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetParam

HRESULT STDMETHODCALLTYPE CTrackMgr::GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(mtTime);
	UNREFERENCED_PARAMETER(pmtNext);

	ASSERT( pData != NULL );
	if( pData == NULL )
	{
		return E_POINTER;
	}

	// Get the list of wave nodes we reference
	if( ::IsEqualGUID( guidType, GUID_Segment_ReferencedNodes ) )
	{
		DMUSProdReferencedNodes *pDMUSProdReferencedNodes = (DMUSProdReferencedNodes *)pData;

        // Iterate through the wave strips
		DWORD dwIndex = 0;
	    POSITION pos = m_lstWaveStrips.GetHeadPosition();
	    while( pos )
	    {
		    CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );
            pWaveStrip->GetReferencedNodes( &dwIndex, pDMUSProdReferencedNodes );
	    }

		HRESULT hr = pDMUSProdReferencedNodes->apIDMUSProdNode && (pDMUSProdReferencedNodes->dwArraySize < dwIndex) ? S_FALSE : S_OK;

		// Store the number of nodes we returned (or that we require)
		pDMUSProdReferencedNodes->dwArraySize = dwIndex;

		return hr;
	}

	return CBaseMgr::GetParam( guidType, mtTime, pmtNext, pData );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SetParam

HRESULT STDMETHODCALLTYPE CTrackMgr::SetParam( REFGUID guidType, MUSIC_TIME mtTime, void* pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(mtTime);

	if( pData == NULL )
	{
		ASSERT( 0 );
		return E_POINTER;
	}

	// Get wave from wave track
	if( ::IsEqualGUID( guidType, GUID_WaveParam ) )
	{
		DMUSPROD_WAVE_PARAM* pWaveParam = (DMUSPROD_WAVE_PARAM *)pData;
		if( pWaveParam->pIWaveNode == NULL )
		{
			ASSERT( 0 );
			return E_INVALIDARG;
		}

		CWaveStrip* pWaveStrip = GetWaveStrip( pWaveParam->dwPChannel, pWaveParam->dwIndex );
		if( pWaveStrip )
		{
			CTrackItem* pItem = new CTrackItem( this, pWaveStrip );
			if( pItem )
			{
				pItem->m_rtTimeLogical = mtTime;
				pItem->SetTimePhysical( pWaveParam->rtTimePhysical, STP_LOGICAL_NO_ACTION );
				pItem->SetFileReference( pWaveParam->pIWaveNode );
				pItem->m_pLayer = pWaveStrip->CreateLayerForIndex( 0 );
				pWaveStrip->InsertItem( pItem );
				return S_OK;
			}

			delete pWaveStrip;
		}

		return E_FAIL;
	}

	// Get wave from wave track
	if( ::IsEqualGUID( guidType, GUID_WaveParam2 ) )
	{
		DMUSPROD_WAVE_PARAM2* pWaveParam = (DMUSPROD_WAVE_PARAM2 *)pData;
		if( pWaveParam->pIWaveNode == NULL )
		{
			ASSERT( 0 );
			return E_INVALIDARG;
		}

		CWaveStrip* pWaveStrip = GetWaveStrip( pWaveParam->dwPChannel, pWaveParam->dwIndex );
		if( pWaveStrip )
		{
			CTrackItem* pItem = new CTrackItem( this, pWaveStrip );
			if( pItem )
			{
				pItem->m_rtTimeLogical = mtTime;
				pItem->SetTimePhysical( pWaveParam->rtTimePhysical, STP_LOGICAL_NO_ACTION );
				pItem->SetFileReference( pWaveParam->pIWaveNode );
				pItem->m_pLayer = pWaveStrip->CreateLayerForIndex( 0 );

				pWaveStrip->InsertItem( pItem );

				// Must set the variation after insertion, as InsertItem() will override them
				pItem->m_dwVariations = pWaveParam->dwVariation;

				// Only enable the variations with waves in them
				pWaveStrip->m_dwVariationsMask = 0;
				POSITION pos = pWaveStrip->m_lstTrackItems.GetHeadPosition();
				while( pos )
				{
					pWaveStrip->m_dwVariationsMask |= pWaveStrip->m_lstTrackItems.GetNext(pos)->m_dwVariations;
				}

				// If no variations enabled, enable them all
				if( 0 == pWaveStrip->m_dwVariationsMask )
				{
					pWaveStrip->m_dwVariationsMask = 0xFFFFFFFF;
				}

				pWaveStrip->m_StripUI.m_dwVariationBtns = pWaveStrip->m_dwVariationsMask;
				return S_OK;
			}

			delete pWaveStrip;
		}

		return E_FAIL;
	}

	return CBaseMgr::SetParam( guidType, mtTime, pData );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::IsParamSupported

HRESULT STDMETHODCALLTYPE CTrackMgr::IsParamSupported( REFGUID guidType )
{
	// We support getting/setting waves in a wave track
    // We support getting the referenced nodes
	if( ::IsEqualGUID( guidType, GUID_WaveParam )
	||	::IsEqualGUID( guidType, GUID_WaveParam2 )
	||  ::IsEqualGUID( guidType, GUID_Segment_ReferencedNodes ) )
	{
		return S_OK;
	}

	return CBaseMgr::IsParamSupported( guidType );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CTrackMgr::OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the update isn't for our strip, exit
	if( !(dwGroupBits & m_dwGroupBits) )
	{
		return E_INVALIDARG;
	}

	// GUID_TimeSignature
	if( ::IsEqualGUID( rguidType, GUID_TimeSignature ) )
	{
		// Redraw all strips
		AllStrips_Invalidate();
		return S_OK;
	}

	// GUID_TempoParam
	if( ::IsEqualGUID( rguidType, GUID_TempoParam ) )
	{
		// Ask DLS Designer for info about all items
		AllStrips_GetWaveInfo();

		// Redraw all strips
		AllStrips_Invalidate();
		return S_OK;
	}

	// GUID_Segment_AllTracksAdded
	if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		// Make sure host has latest version of data
		// May have changed during load if user prompted to choose Wave from File Open dialog
		if( m_fDirty )
		{
			OnDataChanged();
			m_fDirty = FALSE;
		}

		// Ask DLS Designer for info about all items
		m_fInAllTracksAdded = true;
		AllStrips_GetWaveInfo();
		m_fInAllTracksAdded = false;
		m_fWasRuntimeTrack = false;	// Turn off both flags!

		// Redraw all strips
		AllStrips_Invalidate();
		return S_OK;
	}

	// GUID_Segment_Start
	if( ::IsEqualGUID( rguidType, GUID_Segment_Start ) )
	{
		if( pData == NULL )
		{
			return E_POINTER;
		}
		
		RELEASE( m_pIDMSegmentState );

		IDirectMusicSegmentState* pIDMSegmentState;
		if( SUCCEEDED( ((IUnknown *)pData)->QueryInterface( IID_IDirectMusicSegmentState, (void **) &pIDMSegmentState ) ) )
		{
			m_pIDMSegmentState = pIDMSegmentState;

			// Update the audition variations
			SetAuditionVariations( false );

			// Enable all the variation timers
			POSITION position = m_lstWaveStrips.GetHeadPosition();
			while (position != NULL)
			{
				CWaveStrip *pCWaveStrip = m_lstWaveStrips.GetNext(position);

				// Enable the variation timer.  This is a no-op if there is no
				// Timeline window
				// Also, update the currently playing variation.
				pCWaveStrip->EnableVariationTimer();
				pCWaveStrip->UpdatePlayingVariation();
			}
		}
		return S_OK;
	}

	// GUID_Segment_Stop
	if( ::IsEqualGUID( rguidType, GUID_Segment_Stop ) )
	{
		RELEASE( m_pIDMSegmentState );

		// Disable all the variation timers
		POSITION position = m_lstWaveStrips.GetHeadPosition();
		while (position != NULL)
		{
			CWaveStrip *pCWaveStrip = m_lstWaveStrips.GetNext(position);

			// Disable the variation timer and update the currently playing variation.
			pCWaveStrip->KillVariationTimer();
			pCWaveStrip->UpdatePlayingVariation();
		}

		return S_OK;
	}

	// GUID_Segment_WindowActive
	if( ::IsEqualGUID( rguidType, GUID_Segment_WindowActive ) )
	{
		if( pData == NULL )
		{
			return E_POINTER;
		}

		// Set audition variations
		BOOL fActivate = *((BOOL *)pData);
		if( fActivate )
		{
			SetAuditionVariations( false );
		}
		else
		{
			SetAuditionVariations( true );
		}
	}

	// GUID_Segment_DeletedTrack
	if( ::IsEqualGUID( rguidType, GUID_Segment_DeletedTrack ) )
	{
		AllStrips_DeleteAllParts();
		return S_OK;
	}

	// GUID_ConductorUnloadWaves
	if(::IsEqualGUID(rguidType, GUID_ConductorUnloadWaves))
	{
		POSITION pos = m_lstWaveStrips.GetHeadPosition();
		while( pos )
		{
			CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

			pWaveStrip->OnUpdate( rguidType, dwGroupBits, pData );
		}

		// Unload waves
		UnloadQueuedWaves();
		return S_OK;
	}

	// GUID_ConductorDownloadWaves
	if(::IsEqualGUID(rguidType, GUID_ConductorDownloadWaves))
	{
		POSITION pos = m_lstWaveStrips.GetHeadPosition();
		while( pos )
		{
			CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

			pWaveStrip->OnUpdate( rguidType, dwGroupBits, pData );
		}

		// Download waves
		DownloadQueuedWaves();
		return S_OK;
	}

	// GUID_ConductorFlushWaves
	if(::IsEqualGUID(rguidType, GUID_ConductorFlushWaves))
	{
		// Flush all waves
		FlushAllWaves();
		return S_OK;
	}

	// GUID_Segment_FrameworkMsg
	if( ::IsEqualGUID(rguidType, GUID_Segment_FrameworkMsg) )
	{
		DMUSProdFrameworkMsg* pFrameworkMsg =  (DMUSProdFrameworkMsg *)pData;

		if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, WAVENODE_DataChange) )
		{
			if( m_pIDMSegmentState 
			&&  m_pIDMPerformance )
			{
				// Stop segments that are playing so that unload/download will not fail.
				// Hopefully this is only a temporary work around to a DMusic issue!
				m_pIDMPerformance->Stop( NULL, m_pIDMSegmentState, 0, 0 );
				Sleep( 300 );	// Yikes!  This is necessary to make sure unload/download does not fail.
			}
		}

		// Pass notification to all strips
		POSITION pos = m_lstWaveStrips.GetHeadPosition();
		while( pos )
		{
			CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

			pWaveStrip->OnUpdate( rguidType, dwGroupBits, pData );
		}
		return S_OK;
	}

	if( ::IsEqualGUID( rguidType, GUID_TimelineSetSnapTo )
	&&	(pData != NULL) )
	{
		// Convert from the Timeline SNAP_TO setting to our own SNAP_TO settings
		WORD wSnapVal;
		DMUSPROD_TIMELINE_SNAP_TO *ptlSnapTo = (DMUSPROD_TIMELINE_SNAP_TO *)pData;

		switch( *ptlSnapTo )
		{
		case DMUSPROD_TIMELINE_SNAP_NONE:
			wSnapVal = IDM_SNAP_NONE;
			break;
		case DMUSPROD_TIMELINE_SNAP_GRID:
			wSnapVal = IDM_SNAP_GRID;
			break;
		case DMUSPROD_TIMELINE_SNAP_BEAT:
			wSnapVal = IDM_SNAP_BEAT;
			break;
		case DMUSPROD_TIMELINE_SNAP_BAR:
			wSnapVal = IDM_SNAP_BAR;
			break;
		default:
			ASSERT(FALSE);
			wSnapVal = IDM_SNAP_GRID;
			break;
		}

		// Update all the strips with the new snap-to setting
		POSITION pos = m_lstWaveStrips.GetHeadPosition();
		while( pos )
		{
			CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

			pWaveStrip->m_StripUI.m_nSnapToMusicTime = wSnapVal;

			// If switching to none or grid, update the reftime snap-to setting also
			if( (wSnapVal == IDM_SNAP_NONE)
			||	(wSnapVal == IDM_SNAP_GRID) )
			{
				pWaveStrip->m_StripUI.m_nSnapToRefTime = wSnapVal;
			}
		}
	}

	// We don't handle whichever notification was passed to us
	return CBaseMgr::OnUpdate( rguidType, dwGroupBits, pData );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CTrackMgr::SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant )
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
				RELEASE( m_pPropPageMgr );

				// Remove our strips from the Timeline
				AllStrips_RemoveFromTimeline();

				// Remove ourself from the Timeline's notification list
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TempoParam, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Start, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Stop, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_WindowActive, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetSnapTo, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_ConductorDownloadWaves, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_ConductorUnloadWaves, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_ConductorFlushWaves, m_dwOldGroupBits );

				// Release our reference on the Timeline
				RELEASE( m_pTimeline );
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
					// Add our strips to the timeline
					AllStrips_AddToTimeline();

					// Add ourself to the Timeline's notification list
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TempoParam, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Start, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Stop, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_WindowActive, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetSnapTo, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_ConductorDownloadWaves, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_ConductorUnloadWaves, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_ConductorFlushWaves, m_dwGroupBits );
				}
			}
			else
			{
				// Window is being closed
				SetAuditionVariations( true );
			}
			return S_OK;

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
			return S_OK;
	}

	// Let CBaseMgr handle
	return CBaseMgr::SetStripMgrProperty( stripMgrProperty, variant );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::Load

HRESULT CTrackMgr::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	DWORD dwSize;
	DWORD dwByteCount;

	// Verify that the stream pointer is non-null
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Try and allocate a RIFF stream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Release all the items in m_lstWaveStrips
	while( !m_lstWaveStrips.IsEmpty() )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.RemoveHead();
		
		long cRef = pWaveStrip->Release();
		ASSERT( cRef == 0 );
	}
	
	// Initialize fields
	m_strLastWaveName.Empty();
	m_fWasRuntimeTrack = true;

	// Variables used when loading the Wave track
	MMCKINFO ckTrack;
	MMCKINFO ckList;

	// Interate through every chunk in the stream
	while( pIRiffStream->Descend( &ckTrack, NULL, 0 ) == 0 )
	{
		switch( ckTrack.ckid )
		{
			case FOURCC_LIST:
				switch( ckTrack.fccType )
				{
					case DMUS_FOURCC_WAVETRACK_LIST:
						while( pIRiffStream->Descend( &ckList, &ckTrack, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_WAVETRACK_CHUNK:
								{
									DMUS_IO_WAVE_TRACK_HEADER iTrackHeader;

									// Read in the item's header structure
									dwSize = min( sizeof( DMUS_IO_WAVE_TRACK_HEADER ), ckList.cksize );
									hr = pIStream->Read( &iTrackHeader, dwSize, &dwByteCount );

									// Handle any I/O error by returning a failure code
									if( FAILED( hr )
									||  dwByteCount != dwSize )
									{
										hr = E_FAIL;
										goto ON_ERROR;
									}

									m_lVolume = iTrackHeader.lVolume;
									m_dwTrackFlagsDM = iTrackHeader.dwFlags;
									break;
								}

								case FOURCC_LIST:
									switch( ckList.fccType )
									{
										case DMUS_FOURCC_WAVEPART_LIST:
										{
											CWaveStrip* pNewStrip = new CWaveStrip( this );
											if( pNewStrip == NULL )
											{
												hr = E_OUTOFMEMORY;
												goto ON_ERROR;
											}
											hr = pNewStrip->LoadStrip( pIRiffStream, &ckList );
											if( FAILED ( hr ) )
											{
												delete pNewStrip;
												goto ON_ERROR;
											}
											InsertByAscendingPChannel( pNewStrip );
											break;
										}
									}
									break;
							}

							pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
		}

		pIRiffStream->Ascend( &ckTrack, 0 );
	}

	SyncWithDirectMusic();

ON_ERROR:
	m_strLastWaveName.Empty();
	pIRiffStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SaveTrackHeader

HRESULT CTrackMgr::SaveTrackHeader( IDMUSProdRIFFStream* pIRiffStream )
{
	MMCKINFO ck;
	HRESULT hr;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Create the DMUS_FOURCC_WAVETRACK_CHUNK chunk
	ck.ckid = DMUS_FOURCC_WAVETRACK_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Clear out the structure (clears out the padding bytes as well).
	DMUS_IO_WAVE_TRACK_HEADER oTrackHeader;
	ZeroMemory( &oTrackHeader, sizeof(DMUS_IO_WAVE_TRACK_HEADER) );

	// Fill in the members of the DMUS_IO_WAVE_TRACK_HEADER structure
	oTrackHeader.lVolume = m_lVolume;		
	oTrackHeader.dwFlags = m_dwTrackFlagsDM;		

	// Write the structure out to the stream
	DWORD dwBytesWritten;
	hr = pIStream->Write( &oTrackHeader, sizeof(DMUS_IO_WAVE_TRACK_HEADER), &dwBytesWritten );
	if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_WAVE_TRACK_HEADER) )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Ascend out of the DMUS_FOURCC_WAVETRACK_CHUNK chunk
	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIStream );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::Save

HRESULT CTrackMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that the stream pointer is non-null
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	// Structures for determining the stream type
	DMUSProdStreamInfo	StreamInfo;
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

	// We only support saving to a DirectMusic stream (GUID_CurrentVersion) or a DirectMusic
	// stream that will be loaded into a DirectMusic Wave track (GUID_DirectMusicObject)
	if( !::IsEqualGUID( guidDataFormat, GUID_CurrentVersion )
	&&	!::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
	{
		return E_INVALIDARG;
	}

	// Now, finally save ourself
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Allocate an IDMUSProdRIFFStream from the IStream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Create a LIST chunk to store the track data
	MMCKINFO ckTrack;
 	ckTrack.fccType = DMUS_FOURCC_WAVETRACK_LIST;
	if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the LIST chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save the track data
	{
		// Save the track header
		hr = SaveTrackHeader( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}

		// Save the wave strips
		AllStrips_ResetIndexes();
		if( !m_lstWaveStrips.IsEmpty() )
		{

			// Iterate through the item list
			POSITION pos = m_lstWaveStrips.GetHeadPosition();
			while( pos )
			{
				// Get a pointer to each item
				CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

				// Save each strip
				hr = pWaveStrip->SaveStrip( pIRiffStream );
				if( FAILED ( hr ) )
				{
					goto ON_ERROR;
				}
			}
		}
	}

	pIRiffStream->Ascend( &ckTrack, 0 );

	// If we're supposed to clear our dirty flag, do so now (since the save succeeded)
	if( fClearDirty )
	{
		m_fDirty = false;
	}

ON_ERROR:
	// Release our pointer to the IDMUSProdRIFFStream
	pIRiffStream->Release();

	// Return the success/failure code
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetData

HRESULT STDMETHODCALLTYPE CTrackMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameter
	if( (ppData == NULL)
	|| (*ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	if( m_pWaveStripForPropSheet == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	DWORD *pdwIndex = static_cast<DWORD *>(*ppData);
	switch( *pdwIndex )
	{
		// Group Bits tab
		case 0:
		{
			ioGroupBitsPPG *pioGroupBitsPPG = static_cast<ioGroupBitsPPG *>(*ppData);

			pioGroupBitsPPG->dwGroupBits = m_dwGroupBits;
			break;
		}

		// Track flags tab
		case 1:
		{
			PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
			pPPGTrackFlagsParams->dwTrackExtrasFlags = m_dwTrackExtrasFlags;
			pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
			pPPGTrackFlagsParams->dwProducerOnlyFlags = m_dwProducerOnlyFlags;
			pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
			break;
		}

		// Wave Track tab
		case 2:
		{
			ioWaveTrackPPG *pioWaveTrackPPG = static_cast<ioWaveTrackPPG *>(*ppData);

			pioWaveTrackPPG->lVolume = m_lVolume;		
			pioWaveTrackPPG->dwTrackFlagsDM = m_dwTrackFlagsDM;
			break;
		}

		// Wave Part tab
		case 3:
		{
			ioWavePartPPG *pioWavePartPPG = static_cast<ioWavePartPPG *>(*ppData);

			pioWavePartPPG->dwPChannel = m_pWaveStripForPropSheet->m_dwPChannel;
			pioWavePartPPG->lVolume = m_pWaveStripForPropSheet->m_lVolume;		
			pioWavePartPPG->dwLockToPart = m_pWaveStripForPropSheet->m_dwLockToPart;	
			pioWavePartPPG->dwPartFlagsDM = m_pWaveStripForPropSheet->m_dwPartFlagsDM;
			pioWavePartPPG->strStripName = m_pWaveStripForPropSheet->m_strStripName;
			GetPChannelName( m_pWaveStripForPropSheet->m_dwPChannel, pioWavePartPPG->strPChannelName );
			break;
		}

		default:
			ASSERT(FALSE);
			break;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SetData

HRESULT STDMETHODCALLTYPE CTrackMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameter
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( m_pWaveStripForPropSheet == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}
	
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
		// Group bits
		case 0:
		{
			ioGroupBitsPPG *pioGroupBitsPPG = static_cast<ioGroupBitsPPG *>(pData);

			// m_dwGroupBits
			if( pioGroupBitsPPG->dwGroupBits != m_dwGroupBits )
			{
				m_nLastEdit = IDS_UNDO_TRACK_GROUP;
				m_dwGroupBits = pioGroupBitsPPG->dwGroupBits;
				OnUpdate( GUID_TimeSignature, m_dwGroupBits, NULL );
				OnDataChanged();
				m_dwOldGroupBits = pioGroupBitsPPG->dwGroupBits;
			}

			break;
		}

		// Track flags tab
		case 1:
		{
			PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

			// m_dwTrackExtrasFlags
			if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_dwTrackExtrasFlags )
			{
				m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
				bool fWasRefTimeTrack = IsRefTimeTrack();
				m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;
				bool fIsRefTimeTrack = IsRefTimeTrack();

				if( fWasRefTimeTrack != fIsRefTimeTrack )
				{
					AllStrips_SwitchTimeBase();
				}

				OnDataChanged();
				AllStrips_Invalidate();
			}

			// m_dwProducerOnlyFlags
			else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_dwProducerOnlyFlags )
			{
				m_nLastEdit = IDS_UNDO_PRODUCERONLY;
				m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;
				OnDataChanged();
			}

			break;
		}

		// Wave Track tab
		case 2:
		{
			ioWaveTrackPPG *pioWaveTrackPPG = static_cast<ioWaveTrackPPG *>(pData);

			// m_lVolume
			if( pioWaveTrackPPG->lVolume != m_lVolume )
			{
				m_nLastEdit = IDS_UNDO_TRACK_VOLUME;
				m_lVolume = pioWaveTrackPPG->lVolume;
				OnDataChanged();
			}

			// m_dwTrackFlagsDM
			else if( pioWaveTrackPPG->dwTrackFlagsDM != m_dwTrackFlagsDM )
			{
				m_nLastEdit = IDS_UNDO_TRACK_FLAGS;
				m_dwTrackFlagsDM = pioWaveTrackPPG->dwTrackFlagsDM;
				OnDataChanged();
			}

			break;
		}

		// Wave Part tab
		case 3:
		{
			ioWavePartPPG *pioWavePartPPG = static_cast<ioWavePartPPG *>(pData);

			// m_pWaveStripForPropSheet->m_dwPChannel
			if( pioWavePartPPG->dwPChannel != m_pWaveStripForPropSheet->m_dwPChannel )
			{
				CWaitCursor wait;

				m_nLastEdit = IDS_UNDO_PART_PCHANNEL;
				m_pWaveStripForPropSheet->m_dwPChannel = pioWavePartPPG->dwPChannel;
				RepositionPart( m_pWaveStripForPropSheet );
				m_pWaveStripForPropSheet->RefreshAllWaves();
				OnDataChanged();
				m_pTimeline->StripInvalidateRect( m_pWaveStripForPropSheet, NULL, FALSE );
			}

			// m_pWaveStripForPropSheet->m_lVolume
			else if( pioWavePartPPG->lVolume != m_pWaveStripForPropSheet->m_lVolume )
			{
				m_nLastEdit = IDS_UNDO_PART_VOLUME;
				m_pWaveStripForPropSheet->m_lVolume = pioWavePartPPG->lVolume;
				OnDataChanged();
			}

			// m_pWaveStripForPropSheet->m_dwLockToPart
			else if( pioWavePartPPG->dwLockToPart != m_pWaveStripForPropSheet->m_dwLockToPart )
			{
				m_nLastEdit = IDS_UNDO_PART_LOCK_TO_PART;
				m_pWaveStripForPropSheet->m_dwLockToPart = pioWavePartPPG->dwLockToPart;
				OnDataChanged();
				// Refresh the property page to update values in Lock ID combo box
				if( m_pPropPageMgr )
				{
					m_pPropPageMgr->RefreshData();
				}
			}

			// m_pWaveStripForPropSheet->m_dwPartFlagsDM
			else if( pioWavePartPPG->dwPartFlagsDM != m_pWaveStripForPropSheet->m_dwPartFlagsDM )
			{
				m_nLastEdit = IDS_UNDO_PART_FLAGS;
				m_pWaveStripForPropSheet->m_dwPartFlagsDM = pioWavePartPPG->dwPartFlagsDM;
				OnDataChanged();
			}

			// m_pWaveStripForPropSheet->m_strStripName
			else if( pioWavePartPPG->strStripName != m_pWaveStripForPropSheet->m_strStripName )
			{
				m_nLastEdit = IDS_UNDO_PART_NAME;
				m_pWaveStripForPropSheet->m_strStripName = pioWavePartPPG->strStripName;
				OnDataChanged();
				m_pTimeline->StripInvalidateRect( m_pWaveStripForPropSheet, NULL, FALSE );
			}

			// m_pWaveStripForPropSheet's PChannel name
			else if( m_pDMProdFramework )
			{
				IDMUSProdProject* pIProject;
				if( SUCCEEDED ( m_pDMProdFramework->FindProject( m_pIDocRootNode, &pIProject ) ) ) 
				{
					IDMUSProdPChannelName* pIPChannelName;
					if( SUCCEEDED( pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&pIPChannelName ) ) )
					{
						WCHAR wstrText[MAX_PATH];
						if( SUCCEEDED ( pIPChannelName->GetPChannelName( m_pWaveStripForPropSheet->m_dwPChannel, wstrText ) ) )
						{
							CString strPChName = wstrText;
							if( strPChName.Compare( pioWavePartPPG->strPChannelName ) != 0 )
							{
								MultiByteToWideChar( CP_ACP, 0, pioWavePartPPG->strPChannelName, -1, wstrText, MAX_PATH );
								pIPChannelName->SetPChannelName( m_pWaveStripForPropSheet->m_dwPChannel, wstrText );

								// This doesn't affect anything in the segment - just the PChannel names
								// in the Producer project file.
							}
						}
					
						RELEASE( pIPChannelName );
					}

					RELEASE( pIProject );
				}
			}

			break;
		}

		default:
			ASSERT(FALSE);
			break;
	}

	// Sync track with DirectMusic
	SyncWithDirectMusic();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CTrackMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate pointers
	if( m_pTimeline == NULL
	||  m_pDMProdFramework == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the property sheet
	IDMUSProdPropSheet* pIPropSheet = NULL;
	m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
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

	// If our property page is already displayed, refresh and exit
	if( pIPropSheet->IsEqualPageManagerObject( (IDMUSProdPropPageObject*)this ) == S_OK )
	{
		ASSERT( m_pPropPageMgr != NULL );
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
		pIPropSheet->Release();
		return S_OK;
	}

	// release our reference to the property sheet
	pIPropSheet->Release();

	// Check if our property page manager exists yet
	if( m_pPropPageMgr == NULL )
	{
		// Nope, need to create it
		CGroupBitsPropPageMgr* pPPM = new CGroupBitsPropPageMgr( m_pDMProdFramework, this );

		// Check if the creation succeeded.
		if( pPPM == NULL )
		{
			return E_OUTOFMEMORY;
		}

		// Set m_pPropPageMgr with a reference to the IDMUSProdPropPageManager interface
		HRESULT hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );

		// Release our original reference on the property page manager (added when it
		// was created).
		m_pPropPageMgr->Release();

		// If the QueryInterface failed, return.
		if( FAILED(hr) )
		{
			// Nothing to release, since no reference was added by the call to QueryInterface.
			return hr;
		}
	}

	// Store the last active tab
	short nActiveTab = CGroupBitsPropPageMgr::sm_nActiveTab;

	// Set the property page to refer to the group bits property page.
	m_pTimeline->SetPropertyPage( m_pPropPageMgr, (IDMUSProdPropPageObject*)this );

	// Set the active tab
	if( SUCCEEDED ( m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		CWnd* pWndHadFocus = CWnd::GetFocus();

		pIPropSheet->SetActivePage( nActiveTab ); 
		RELEASE( pIPropSheet );

		if( pWndHadFocus )
		{
			pWndHadFocus->SetFocus();
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::OnRemoveFromPageManager

HRESULT CTrackMgr::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Clear our property page manager
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->SetObject( NULL );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SyncWithDirectMusic

HRESULT CTrackMgr::SyncWithDirectMusic( void )
{
	// If there is no DirectMusic Track, return S_FALSE
	if( m_pIDMTrack == NULL )
	{
		return S_FALSE;
	}

	// Unload waves
	UnloadQueuedWaves();

	// Hand DirectMusic new wave track
	HRESULT hr = CBaseMgr::SyncWithDirectMusic();
	if( SUCCEEDED ( hr ) )
	{
		// Set audition variations
		SetAuditionVariations( false );
	}

	// Download waves
	DownloadQueuedWaves();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::RecomputeTimes

// Return true if anything changed
bool CTrackMgr::RecomputeTimes( void )
{
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SetDirtyFlag

void CTrackMgr::SetDirtyFlag( bool fDirty )
{
	m_fDirty = fDirty;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::InsertByAscendingPChannel

void CTrackMgr::InsertByAscendingPChannel( CWaveStrip *pWaveStripToInsert )
{
	// Ensure the pWaveStripToInsert pointer is valid
	if ( pWaveStripToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	CWaveStrip *pWaveStrip;
	POSITION posCurrent, posNext = m_lstWaveStrips.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pWaveStrip = m_lstWaveStrips.GetNext( posNext );

		if( pWaveStrip->m_dwPChannel > pWaveStripToInsert->m_dwPChannel )
		{
			// insert before posCurrent (which is the position of pWaveStrip)
			m_lstWaveStrips.InsertBefore( posCurrent, pWaveStripToInsert );
			return;
		}
	}

	// pWaveStripToInsert has higher PChannel than all items in the list
	// add it at the end of the list
	m_lstWaveStrips.AddTail( pWaveStripToInsert );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::RefTimeToMinSecGridMs

HRESULT	CTrackMgr::RefTimeToMinSecGridMs( CWaveStrip* pWaveStrip,
										  REFERENCE_TIME rtTime,
										  long* plMinute, long* plSecond, long* plGrid, long* plMillisecond )
{
	ASSERT( pWaveStrip != NULL );
	ASSERT( plMinute != NULL );
	ASSERT( plSecond != NULL );
	ASSERT( plGrid != NULL );
	ASSERT( plMillisecond != NULL );

	REFERENCE_TIME rtRefClocksPerGrid = REFCLOCKS_PER_SECOND / pWaveStrip->m_StripUI.m_nSnapToRefTime_GridsPerSecond;

	*plMinute = (long)(rtTime / REFCLOCKS_PER_MINUTE);
	rtTime = rtTime % REFCLOCKS_PER_MINUTE;

	*plSecond = (long)(rtTime / REFCLOCKS_PER_SECOND);
	rtTime = rtTime % REFCLOCKS_PER_SECOND;

	*plGrid = (long)(rtTime / rtRefClocksPerGrid);
	rtTime = rtTime % rtRefClocksPerGrid;

	*plMillisecond = (long)(rtTime / REFCLOCKS_PER_MILLISECOND);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::MinSecGridMsToRefTime

HRESULT	CTrackMgr::MinSecGridMsToRefTime( CWaveStrip* pWaveStrip,
										  long lMinute, long lSecond, long lGrid, long lMillisecond,
										  REFERENCE_TIME* prtTime )
{
	ASSERT( pWaveStrip != NULL );
	ASSERT( prtTime != NULL );

	REFERENCE_TIME rtRefClocksPerGrid = REFCLOCKS_PER_SECOND / pWaveStrip->m_StripUI.m_nSnapToRefTime_GridsPerSecond;

	*prtTime =  lMinute * REFCLOCKS_PER_MINUTE;
	*prtTime += lSecond * REFCLOCKS_PER_SECOND;
	*prtTime += lGrid * rtRefClocksPerGrid;
	*prtTime += lMillisecond * REFCLOCKS_PER_MILLISECOND;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::ClocksToMeasureBeatGridTick

HRESULT	CTrackMgr::ClocksToMeasureBeatGridTick( MUSIC_TIME mtTime, long* plMeasure, long* plBeat, long* plGrid, long* plTick )
{
	ASSERT( plMeasure != NULL );
	ASSERT( plBeat != NULL );
	ASSERT( plGrid != NULL );
	ASSERT( plTick != NULL );
	
	long lMeasure = -1;
	long lBeat = -1;
	long lGrid = -1;
	long lTick = -1;
	long lGridsAndTicks;

	HRESULT hr = S_OK;

	if( m_pTimeline == NULL )
	{
		hr = S_FALSE;
	}
	else
	{
		if( mtTime < 0 )
		{
			lMeasure = 0;
			lBeat = 0;
			lGrid = 0;
			lTick = mtTime;
		}
		else
		{
			hr = m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
			if( SUCCEEDED ( hr ) )
			{
				long lClocks;

				hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &lClocks );
				if( SUCCEEDED ( hr ) )
				{
					lGridsAndTicks = mtTime - lClocks;

					// Get the TimeSig for this measure/beat
					DMUS_TIMESIGNATURE ts;
					if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, lClocks, NULL, &ts ) ) )
					{
						// Compute the grid and tick
						long lClocksPerGrid = ((DMUS_PPQ << 2) / ts.bBeat) / ts.wGridsPerBeat;
						lGrid = lGridsAndTicks / lClocksPerGrid;
						lTick = lGridsAndTicks % lClocksPerGrid;

						// Try and preserve negative tick offsets
						if( lTick > 0 )
						{
							long lNewClocks = lClocks + lClocksPerGrid;

							if( lTick >= (lClocksPerGrid >> 1) )
							{
								// Get max clock
								VARIANT var;
								m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
								MUSIC_TIME mtMaxTimelineLength = V_I4( &var );

								if( lNewClocks < mtMaxTimelineLength )
								{
									lGrid++;
									lTick -= lClocksPerGrid;
									if( lGrid >= ts.wGridsPerBeat )
									{
										lGrid = 0;
										mtTime += lClocksPerGrid;
										hr = m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
									}
								}
							}
						}
					}
				}
			}
		}
	}

	*plMeasure = lMeasure;
	*plBeat = lBeat;
	*plGrid = lGrid;
	*plTick	= lTick;

	ASSERT( SUCCEEDED ( hr ) );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::UnknownTimeToMeasureBeatGridTick

HRESULT	CTrackMgr::UnknownTimeToMeasureBeatGridTick( REFERENCE_TIME rtTime, long* plMeasure, long* plBeat, long* plGrid, long* plTick )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return S_FALSE;
	}
	
	MUSIC_TIME mtTime;
	UnknownTimeToClocks( rtTime, &mtTime );

	return ClocksToMeasureBeatGridTick( mtTime, plMeasure, plBeat, plGrid, plTick );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::MeasureBeatGridTickToUnknownTime

HRESULT	CTrackMgr::MeasureBeatGridTickToUnknownTime( long lMeasure, long lBeat, long lGrid, long lTick, REFERENCE_TIME* prtTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return S_FALSE;
	}
	
	MUSIC_TIME mtTime;
	HRESULT hr = MeasureBeatGridTickToClocks( lMeasure, lBeat, lGrid, lTick, &mtTime );

	ClocksToUnknownTime( mtTime, prtTime );

	return hr; 
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SnapUnknownTimeToBeat

HRESULT	CTrackMgr::SnapUnknownTimeToBeat( REFERENCE_TIME rtTime, REFERENCE_TIME* prtBeatTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return S_FALSE;
	}

	MUSIC_TIME mtTime;
	UnknownTimeToClocks( rtTime, &mtTime );

	long lMeasure, lBeat, lGrid, lTick;
	ClocksToMeasureBeatGridTick( mtTime, &lMeasure, &lBeat, &lGrid, &lTick );
	MeasureBeatGridTickToClocks( lMeasure, lBeat, 0, 0, &mtTime );

	ClocksToUnknownTime( mtTime, prtBeatTime );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::ClocksToUnknownTime

HRESULT CTrackMgr::ClocksToUnknownTime( MUSIC_TIME mtTime, REFERENCE_TIME* prtTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return S_FALSE;
	}
	
	HRESULT hr = S_OK;

	if( IsRefTimeTrack() )
	{
		hr = m_pTimeline->ClocksToRefTime( mtTime, prtTime );
	}
	else
	{
		*prtTime = mtTime;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::UnknownTimeToClocks

HRESULT CTrackMgr::UnknownTimeToClocks( REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return S_FALSE;
	}

	HRESULT hr = S_OK;

	if( IsRefTimeTrack() )
	{
		hr = m_pTimeline->RefTimeToClocks( rtTime, pmtTime );
	}
	else
	{
		*pmtTime = (MUSIC_TIME)rtTime;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::RefTimeToUnknownTime

HRESULT CTrackMgr::RefTimeToUnknownTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return S_FALSE;
	}

	HRESULT hr = S_OK;

	if( IsRefTimeTrack() )
	{
		*prtTime = rtTime;
	}
	else
	{
		MUSIC_TIME mtTime;
		hr = m_pTimeline->RefTimeToClocks( rtTime, &mtTime );
		*prtTime = mtTime;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::UnknownTimeToRefTime

HRESULT CTrackMgr::UnknownTimeToRefTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return S_FALSE;
	}

	HRESULT hr = S_OK;

	if( IsRefTimeTrack() )
	{
		*prtTime = rtTime;
	}
	else
	{
		hr = m_pTimeline->ClocksToRefTime( (MUSIC_TIME)rtTime, prtTime );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::MeasureBeatGridTickToClocks

HRESULT	CTrackMgr::MeasureBeatGridTickToClocks( long lMeasure, long lBeat, long lGrid, long lTick, MUSIC_TIME* pmtTime )
{
	ASSERT( pmtTime != NULL );

	MUSIC_TIME mtTime = -1;
	long lClocks;
	long lGridClocks;

	HRESULT hr = S_OK;

	if( m_pTimeline == NULL )
	{
		hr = S_FALSE;
	}
	else
	{
		hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &lClocks );
		if( SUCCEEDED ( hr ) )
		{
			DMUS_TIMESIGNATURE ts;
			if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, lClocks, NULL, &ts ) ) )
			{
				long lClocksPerGrid = ((DMUS_PPQ << 2) / ts.bBeat) / ts.wGridsPerBeat;
				lGridClocks = lGrid * lClocksPerGrid;

				mtTime = lClocks + lGridClocks + lTick;
			}
		}
	}

	*pmtTime = mtTime;

	ASSERT( SUCCEEDED ( hr ) );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::OnNewPart

HRESULT CTrackMgr::OnNewPart( void )
{
	// Have user select a PChannel
	CNewPartDlg newPartDlg;

	newPartDlg.SetTrack( 0 );
	newPartDlg.SetTrackMgr( this );

	// The dialog handles adding parts itself
	newPartDlg.DoModal();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AddPart

HRESULT CTrackMgr::AddPart( DWORD dwPChannel )
{
	// Get position of first wave strip in the timeline
	int nPosition = -1;
	if( m_lstWaveStrips.IsEmpty() == FALSE )
	{
		CWaveStrip* pFirstWaveStrip = m_lstWaveStrips.GetHead();

		VARIANT var;
		if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)pFirstWaveStrip, STP_POSITION, &var) ) )
		{
			nPosition = V_I4(&var);
		}
	}
		
	// Create the new Wave strip
	CWaveStrip* pNewWaveStrip = new CWaveStrip( this );
	if( pNewWaveStrip == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Set the new strip's PChannel
	pNewWaveStrip->m_dwPChannel = dwPChannel;

	// Add the new strip to the list of wave strips
	InsertByAscendingPChannel( pNewWaveStrip );

	// Increment dwPosition
	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		if( pWaveStrip == pNewWaveStrip )
		{
			break;
		}

		nPosition++;
	}

	// Add the new strip to the timeline
	AddStripToTimeline( pNewWaveStrip, nPosition );

	// Activate strip
	VARIANT var;
	var.vt = VT_UNKNOWN;
	if( SUCCEEDED( pNewWaveStrip->QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&var)) ) ) )
	{
		m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );
	}

	// Update Undo queue
	m_nLastEdit = IDS_UNDO_ADD_PART;
	OnDataChanged();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::DeletePart

HRESULT CTrackMgr::DeletePart( CWaveStrip* pWaveStrip )
{
	if( pWaveStrip == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	HRESULT hr = RemoveStripFromTimeline( pWaveStrip );

	// Remove from list of Wave strips
	POSITION pos = m_lstWaveStrips.Find( pWaveStrip );
	if( pos )
	{
		m_lstWaveStrips.RemoveAt( pos );

		pWaveStrip->CleanUp();
		RELEASE( pWaveStrip );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::RepositionPart

HRESULT CTrackMgr::RepositionPart( CWaveStrip* pTheWaveStrip )
{
	// Get position of first wave strip in the timeline
	int nPosition = -1;
	if( m_lstWaveStrips.IsEmpty() == FALSE )
	{
		CWaveStrip* pFirstWaveStrip = m_lstWaveStrips.GetHead();

		VARIANT var;
		if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)pFirstWaveStrip, STP_POSITION, &var) ) )
		{
			nPosition = V_I4(&var);
		}
	}

	// Reposition strip in m_lstWaveStrips
	POSITION pos = m_lstWaveStrips.Find( pTheWaveStrip );
	if( pos )
	{
		m_lstWaveStrips.RemoveAt( pos );
		InsertByAscendingPChannel( pTheWaveStrip );
	}

	// Determine new position of strip for timeline
	pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		if( pWaveStrip == pTheWaveStrip )
		{
			break;
		}

		nPosition++;
	}

	// Reposition strip in timeline
	if( SUCCEEDED ( m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pTheWaveStrip ) ) )
	{
		AddStripToTimeline( pTheWaveStrip, nPosition );

		// Activate strip
		VARIANT var;
		var.vt = VT_UNKNOWN;
		if( SUCCEEDED( pTheWaveStrip->QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&var)) ) ) )
		{
			m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetSegmentLength

REFERENCE_TIME CTrackMgr::GetSegmentLength( void )
{
	MUSIC_TIME mtSegmentLength = LONG_MAX;
	REFERENCE_TIME rtSegmentLength = _I64_MAX;

	if( m_pTimeline )
	{
		VARIANT varLength;
		if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
		{
			mtSegmentLength = V_I4(&varLength);
		}

		ClocksToUnknownTime( mtSegmentLength, &rtSegmentLength );
	}

	return rtSegmentLength;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetPChannelName

HRESULT CTrackMgr::GetPChannelName( DWORD dwPChannel, CString& strPChannelName )
{
	HRESULT hr = E_FAIL;

	strPChannelName.Empty();

	if( m_pDMProdFramework )
	{
		IDMUSProdProject* pIProject;
		if( SUCCEEDED ( m_pDMProdFramework->FindProject( m_pIDocRootNode, &pIProject ) ) ) 
		{
			IDMUSProdPChannelName* pIPChannelName;
			if( SUCCEEDED( pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&pIPChannelName ) ) )
			{
				WCHAR wstrText[MAX_PATH];
				if( SUCCEEDED ( pIPChannelName->GetPChannelName( dwPChannel, wstrText ) ) )
				{
					strPChannelName = wstrText;
					hr = S_OK;
				}
			
				RELEASE( pIPChannelName );
			}

			RELEASE( pIProject );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetObject

HRESULT CTrackMgr::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	// CLSID_DirectMusicWaveTrack
	if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicWaveTrack ) )
	{
		if( m_pIDMTrack )
		{
			return m_pIDMTrack->QueryInterface( riid, ppvObject );
		}
	}

	// CLSID_DirectMusicPerformance
	else if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicPerformance ) )
	{
		if( m_pIDMPerformance == NULL )
		{
			// Get IDirectMusicPerformance interface pointers 
			if( m_pDMProdFramework )
			{
				IDMUSProdComponent* pIComponent;
				if( SUCCEEDED ( m_pDMProdFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ) )
				{
					IDMUSProdConductor* pIConductor;
					if( SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pIConductor ) ) )
					{
						if( FAILED ( pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIDMPerformance ) ) )
						{
							m_pIDMPerformance = NULL;
						}

						RELEASE( pIConductor );
					}

					RELEASE( pIComponent );
				}
			}
		}

		if( m_pIDMPerformance )
		{
			return m_pIDMPerformance->QueryInterface( riid, ppvObject );
		}
	}

	// CLSID_DirectMusicAudioPathConfig
	else if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicAudioPathConfig ) )
	{
		if( m_pIDMPerformance == NULL )
		{
			// Get IDirectMusicPerformance interface pointers 
			if( m_pDMProdFramework )
			{
				IDMUSProdComponent* pIComponent;
				if( SUCCEEDED ( m_pDMProdFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ) )
				{
					IDMUSProdConductor* pIConductor;
					if( SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pIConductor ) ) )
					{
						if( FAILED ( pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIDMPerformance ) ) )
						{
							m_pIDMPerformance = NULL;
						}

						RELEASE( pIConductor );
					}

					RELEASE( pIComponent );
				}
			}
		}

		ASSERT( 0 );
		// Still have to implement code to get the AudioPath from the Performance
	}

    *ppvObject = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::IsValidLockID

bool CTrackMgr::IsValidLockID( DWORD dwLockID )
{
	ASSERT( dwLockID > 0 );

	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		if( pWaveStrip->m_dwLockToPart == dwLockID )
		{
			return true;
		}
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::StripExists

bool CTrackMgr::StripExists( DWORD dwPChannel )
{
	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		if( pWaveStrip->m_dwPChannel == dwPChannel )
		{
			return true;
		}
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetGreatestUsedPChannel

DWORD CTrackMgr::GetGreatestUsedPChannel( void )
{
	DWORD dwPChannel = 0;

	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		dwPChannel = max( dwPChannel, pWaveStrip->m_dwPChannel );
	}

	return dwPChannel;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetWaveStrip

CWaveStrip* CTrackMgr::GetWaveStrip( DWORD dwPChannel, DWORD dwIndex )
{
	CWaveStrip* pTheWaveStrip = NULL;

	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		if( pWaveStrip->m_dwPChannel > dwPChannel )
		{
			break;
		}

		if( pWaveStrip->m_dwPChannel == dwPChannel )
		{
			if( dwIndex == 0 )
			{
				pTheWaveStrip = pWaveStrip;
				break;
			}
			dwIndex--;
		}
	}

	return pTheWaveStrip;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllStrips_Invalidate

void CTrackMgr::AllStrips_Invalidate( void )
{
	if( m_pTimeline == NULL )
	{
		return;
	}

	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		// Refresh strip properties
		pWaveStrip->RefreshPropertyPage();

		// Redraw strip
		m_pTimeline->StripInvalidateRect( pWaveStrip, NULL, TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllStrips_ResetIndexes

void CTrackMgr::AllStrips_ResetIndexes( void )
{
	DWORD dwLastPChannel = 0;
	DWORD dwLastIndex = 0;

	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		if( pWaveStrip->m_dwPChannel != dwLastPChannel )
		{
			dwLastPChannel = pWaveStrip->m_dwPChannel;
			dwLastIndex = 0;
		}
		
		pWaveStrip->m_dwIndex = dwLastIndex++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AddStripToTimeline

HRESULT CTrackMgr::AddStripToTimeline( CWaveStrip* pWaveStrip, int nPosition )
{
	if( pWaveStrip == NULL
	||	m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	HRESULT hr;

	// Place strip in timeline
	if( nPosition <= 0 )
	{
		hr = m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)pWaveStrip, m_clsid, m_dwGroupBits, 0 );
	}
	else
	{
		hr = m_pTimeline->InsertStripAtPos( (IDMUSProdStrip *)pWaveStrip, nPosition );
	}

	// Set strip state information
	pWaveStrip->WavePartUIToTimeline();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllStrips_AddToTimeline

void CTrackMgr::AllStrips_AddToTimeline( void )
{
	int nPosition = -1;

	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		AddStripToTimeline( pWaveStrip, nPosition );

		if( nPosition == -1 )
		{
			VARIANT var;
			if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)pWaveStrip, STP_POSITION, &var) ) )
			{
				nPosition = V_I4(&var);
				ASSERT( nPosition > 0 );
			}
		}

		nPosition++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::RemoveStripFromTimeline

HRESULT CTrackMgr::RemoveStripFromTimeline( CWaveStrip* pWaveStrip )
{
	if( pWaveStrip == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	if( m_pTimeline )
	{
		// Make sure we are storing the latest UI state information
		pWaveStrip->TimelineToWavePartUI();

		// Make sure our strip's property page isn't displayed
		if( pWaveStrip == m_pWaveStripForPropSheet )
		{
			m_pTimeline->RemovePropertyPageObject( (IDMUSProdPropPageObject*)this );
			RELEASE( m_pPropPageMgr );
		}
		m_pTimeline->RemovePropertyPageObject( (IDMUSProdPropPageObject*)pWaveStrip );
		RELEASE( pWaveStrip->m_pPropPageMgr );

		m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pWaveStrip );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllStrips_RemoveFromTimeline

void CTrackMgr::AllStrips_RemoveFromTimeline( void )
{
	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		RemoveStripFromTimeline( pWaveStrip );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllStrips_DeleteAllParts

void CTrackMgr::AllStrips_DeleteAllParts()
{
	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		DeletePart( pWaveStrip );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllStrips_DeleteSelectedParts

void CTrackMgr::AllStrips_DeleteSelectedParts()
{
	if( AfxMessageBox(IDS_WARNING_DELETE_PART, MB_OKCANCEL) != IDOK )
	{
		return;
	}

	IDMUSProdStrip* pIActiveStrip;
	VARIANT var;

	BOOL fStripWasDeleted = FALSE;

	// Delete all gutter selected strips
	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( pWaveStrip, STP_GUTTER_SELECTED, &var ) ) )
		{
			if( V_BOOL(&var) == TRUE )
			{
				DeletePart( pWaveStrip );
				fStripWasDeleted = TRUE;
			}
		}
	}
	
	// If none of the wave strips were gutter selected, delete the active strip
	if( fStripWasDeleted == FALSE )
	{
		if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
		{
			if( V_UNKNOWN(&var) != NULL)
			{
				if( SUCCEEDED ( V_UNKNOWN(&var)->QueryInterface( IID_IDMUSProdStrip, (void **) &pIActiveStrip ) ) )
				{
					pos = m_lstWaveStrips.GetHeadPosition();
					while( pos )
					{
						CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

						if( pWaveStrip == pIActiveStrip )
						{
							DeletePart( pWaveStrip );
						}
					}

					RELEASE( pIActiveStrip );
				}

				V_UNKNOWN(&var)->Release();
			}
		}
	}

	if( m_lstWaveStrips.IsEmpty() )
	{
		ASSERT( m_pIDocRootNode != NULL );
		if( m_pIDocRootNode )
		{
			IDMUSProdSegmentEdit8* pISegmentEdit8;

			if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdSegmentEdit8, (void **)&pISegmentEdit8 ) ) )
			{
				pISegmentEdit8->RemoveStripMgr( (IDMUSProdStripMgr *)this );
				RELEASE( pISegmentEdit8 );
				return;		// Call to RemoveStripMgr should have deleted us!
			}
		}
	}
	else
	{
		// Update Undo queue
		m_nLastEdit = IDS_UNDO_DELETE_PART;
		OnDataChanged();

		// Sync with DirectMusic
		SyncWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllStrips_GetWaveInfo

void CTrackMgr::AllStrips_GetWaveInfo( void )
{
	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		pWaveStrip->GetWaveInfo();
	}

	// Sync with DirectMusic to pick up latest wave durations
	SyncWithDirectMusic();
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllStrips_SwitchTimeBase

void CTrackMgr::AllStrips_SwitchTimeBase( void )
{
	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		pWaveStrip->SwitchTimeBase();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllStrips_SetNbrGridsPerSecond

BOOL CTrackMgr::AllStrips_SetNbrGridsPerSecond( short nNbrGridsPerSecond )
{
	BOOL fChanged = FALSE;

	POSITION pos = m_lstWaveStrips.GetHeadPosition();
	while( pos )
	{
		CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

		if( pWaveStrip->m_StripUI.m_nSnapToRefTime_GridsPerSecond != nNbrGridsPerSecond )
		{
			pWaveStrip->m_StripUI.m_nSnapToRefTime_GridsPerSecond = nNbrGridsPerSecond;
			fChanged = TRUE;
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetActiveStripInThisTrack

CWaveStrip* CTrackMgr::GetActiveStripInThisTrack( void )
{
	CWaveStrip* pActiveWaveStrip = NULL;

	if( m_pTimeline )
	{
		VARIANT var;
		if( SUCCEEDED ( m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
		{
			IUnknown* pIUnknown = V_UNKNOWN(&var);
			if( pIUnknown )
			{
				IDMUSProdStrip* pIActiveStrip;
				if( SUCCEEDED ( pIUnknown->QueryInterface( IID_IDMUSProdStrip, (void**)&pIActiveStrip ) ) )
				{
					POSITION pos = m_lstWaveStrips.GetHeadPosition();
					while( pos )
					{
						CWaveStrip* pWaveStrip = m_lstWaveStrips.GetNext( pos );

						if( pIActiveStrip == (IDMUSProdStrip *)pWaveStrip )
						{
							pActiveWaveStrip = pWaveStrip;
							break;
						}
					}

					RELEASE( pIActiveStrip );
				}
				
				RELEASE( pIUnknown );
			}
		}
	}

	return pActiveWaveStrip;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetLockBitmap

CBitmap* CTrackMgr::GetLockBitmap( BOOL fLocked )
{
	CWaveStrip* pWaveStrip;

	if( m_lstWaveStrips.IsEmpty() == FALSE )
	{
		pWaveStrip = m_lstWaveStrips.GetHead();

		switch( fLocked )
		{
			case FALSE:
				if( pWaveStrip->sm_bmpUnlocked.GetSafeHandle() )
				{
					return &m_pWaveStripForPropSheet->sm_bmpUnlocked;
				}
				break;

			case TRUE:
				if( pWaveStrip->sm_bmpLocked.GetSafeHandle() )
				{
					return &m_pWaveStripForPropSheet->sm_bmpLocked;
				}
				break;

			default:
				ASSERT( 0 );
				break;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SetAuditionVariations

HRESULT CTrackMgr::SetAuditionVariations( bool fForceClear )
{
	if( m_pIDMTrack == NULL )
	{
		return S_FALSE;
	}

	HRESULT hr = E_FAIL;

	IPrivateWaveTrack* pIPrivateWaveTrack;
	IPrivateWaveTrack9* pIPrivateWaveTrack9;
	if( SUCCEEDED ( m_pIDMTrack->QueryInterface( IID_IPrivateWaveTrack9, (void **)&pIPrivateWaveTrack9 ) ) )
	{
		CWaveStrip* pWaveStrip = GetActiveStripInThisTrack();
		if( pWaveStrip
		&&  fForceClear == false )
		{
			DWORD dwVariations = pWaveStrip->m_StripUI.m_dwVariationBtns;

			// If no variations are selected, then fall back to the variation mask
			hr = pIPrivateWaveTrack9->SetVariationMask( m_pIDMSegmentState, dwVariations ? dwVariations : pWaveStrip->m_dwVariationsMask, pWaveStrip->m_dwPChannel, pWaveStrip->m_dwIndex );
		}
		else
		{
			// Clear the variation mask by setting this on an invalid PChannel
			hr = pIPrivateWaveTrack9->SetVariationMask( m_pIDMSegmentState, 0xFFFFFFFF, DMUS_PCHANNEL_BROADCAST_GROUPS - 1000, 0 );
		}

		RELEASE( pIPrivateWaveTrack9 );
	}
	else if( SUCCEEDED ( m_pIDMTrack->QueryInterface( IID_IPrivateWaveTrack, (void **)&pIPrivateWaveTrack ) ) )
	{
		CWaveStrip* pWaveStrip = GetActiveStripInThisTrack();
		if( pWaveStrip
		&&  fForceClear == false )
		{
			DWORD dwVariations = pWaveStrip->m_StripUI.m_dwVariationBtns;
			hr = pIPrivateWaveTrack->SetVariation( m_pIDMSegmentState, dwVariations, pWaveStrip->m_dwPChannel, pWaveStrip->m_dwIndex );
		}
		else
		{
			hr = pIPrivateWaveTrack->ClearVariations( m_pIDMSegmentState );
		}

		RELEASE( pIPrivateWaveTrack );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::IsRefTimeTrack

bool CTrackMgr::IsRefTimeTrack( void )
{
	return (m_dwTrackExtrasFlags & DMUS_TRACKCONFIG_PLAY_CLOCKTIME) ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetQueuedWave
	
QueuedWave* CTrackMgr::GetQueuedWave( IDirectSoundWave* pIDSWave, GUID guidVersion )
{
	QueuedWave* pTheQueuedWave = NULL;

	POSITION pos = m_lstQueuedWaves.GetHeadPosition();
	while( pos )
	{
		QueuedWave* pQueuedWave = m_lstQueuedWaves.GetNext( pos );

		if( pQueuedWave->pIDSWave == pIDSWave )
		{
			pTheQueuedWave = pQueuedWave;
		}
	}

	if( pTheQueuedWave == NULL )
	{
		pTheQueuedWave = new QueuedWave;
		if( pTheQueuedWave )
		{
			pTheQueuedWave->pIDSWave = pIDSWave;
			pTheQueuedWave->pIDSWave->AddRef();
			pTheQueuedWave->guidVersion = guidVersion;
	
			m_lstQueuedWaves.AddHead( pTheQueuedWave );
		}
	}

	return pTheQueuedWave;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::QueueWaveForDownload

HRESULT CTrackMgr::QueueWaveForDownload( CTrackItem* pItem )
{
	if( pItem->m_FileRef.pIDocRootNode == NULL ) 
	{
		// Nothing to do
		return S_OK;
	}

	// Get the corresponding DirectSound Wave object
	IDirectSoundWave* pIDSWave;
	if( FAILED ( pItem->m_FileRef.pIDocRootNode->GetObject( CLSID_DirectSoundWave, IID_IDirectSoundWave, (void **)&pIDSWave ) ) )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	QueuedWave* pQueuedWave = GetQueuedWave( pIDSWave, pItem->m_WaveInfo.guidVersion );

	RELEASE( pIDSWave );

	if( pQueuedWave )
	{
		pQueuedWave->lDownloadCount++;
		return S_OK;
	}

	return E_OUTOFMEMORY;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::QueueWaveForUnload
	
HRESULT CTrackMgr::QueueWaveForUnload( CTrackItem* pItem )
{
	if( pItem->m_FileRef.pIDocRootNode == NULL ) 
	{
		// Nothing to do
		return S_OK;
	}

	// Get the corresponding DirectSound Wave object
	IDirectSoundWave* pIDSWave;
	if( FAILED ( pItem->m_FileRef.pIDocRootNode->GetObject( CLSID_DirectSoundWave, IID_IDirectSoundWave, (void **)&pIDSWave ) ) )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	QueuedWave* pQueuedWave = GetQueuedWave( pIDSWave, pItem->m_WaveInfo.guidVersion );

	RELEASE( pIDSWave );

	if( pQueuedWave )
	{
		pQueuedWave->lDownloadCount--;
		return S_OK;
	}

	return E_OUTOFMEMORY;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::UnloadQueuedWaves

void CTrackMgr::UnloadQueuedWaves( void )
{
	CWaitCursor wait;

	IUnknown* pIUnknown = NULL;

	// Get pIUnknown for UnloadWave method
	IDirectMusicPerformance8* pIDMPerformance8;
	if( SUCCEEDED ( GetObject( CLSID_DirectMusicPerformance, IID_IDirectMusicPerformance8, (void **)&pIDMPerformance8 ) ) )
	{
		IDirectMusicAudioPath* pIDMAudioPath;
		if( SUCCEEDED ( pIDMPerformance8->GetDefaultAudioPath( &pIDMAudioPath ) ) )
		{
			if( FAILED ( pIDMAudioPath->QueryInterface( IID_IUnknown, (void **)&pIUnknown ) ) )
			{
				pIUnknown = NULL;
			}

			RELEASE( pIDMAudioPath );
		}
		else
		{
			if( FAILED ( pIDMPerformance8->QueryInterface( IID_IUnknown, (void **)&pIUnknown ) ) )
			{
				pIUnknown = NULL;
			}
		}

		RELEASE( pIDMPerformance8 );
	}
	ASSERT( pIUnknown != NULL );

	if( pIUnknown )
	{
		// Get the IPrivateWaveTrack interface
		IPrivateWaveTrack* pIPrivateWaveTrack;
		if( SUCCEEDED ( GetObject( CLSID_DirectMusicWaveTrack, IID_IPrivateWaveTrack, (void **)&pIPrivateWaveTrack ) ) )
		{
			POSITION pos = m_lstQueuedWaves.GetHeadPosition();
			POSITION posCur;
			while( pos )
			{
				posCur = pos;
				QueuedWave* pQueuedWave = m_lstQueuedWaves.GetNext( pos );

				if( pQueuedWave->lDownloadCount > 0 )
				{
					// Nothing to do, request is for download
					continue;
				}

				while( pQueuedWave->lDownloadCount < 0 )
				{
					// Unload the Wave
					HRESULT hr = pIPrivateWaveTrack->UnloadWave( pQueuedWave->pIDSWave, pIUnknown );
					ASSERT( SUCCEEDED ( hr ) );
					pQueuedWave->lDownloadCount++;
				}

				m_lstQueuedWaves.RemoveAt( posCur );
				delete pQueuedWave;
			}

			RELEASE( pIPrivateWaveTrack );
		}

		RELEASE( pIUnknown );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::DownloadQueuedWaves

void CTrackMgr::DownloadQueuedWaves( void )
{
	CWaitCursor wait;

	IUnknown* pIUnknown = NULL;

	// Get pIUnknown for DownloadWave method
	IDirectMusicPerformance8* pIDMPerformance8;
	if( SUCCEEDED ( GetObject( CLSID_DirectMusicPerformance, IID_IDirectMusicPerformance8, (void **)&pIDMPerformance8 ) ) )
	{
		IDirectMusicAudioPath* pIDMAudioPath;
		if( SUCCEEDED ( pIDMPerformance8->GetDefaultAudioPath( &pIDMAudioPath ) ) )
		{
			if( FAILED ( pIDMAudioPath->QueryInterface( IID_IUnknown, (void **)&pIUnknown ) ) )
			{
				pIUnknown = NULL;
			}

			RELEASE( pIDMAudioPath );
		}
		else
		{
			if( FAILED ( pIDMPerformance8->QueryInterface( IID_IUnknown, (void **)&pIUnknown ) ) )
			{
				pIUnknown = NULL;
			}
		}

		RELEASE( pIDMPerformance8 );
	}
	ASSERT( pIUnknown != NULL );

	if( pIUnknown )
	{
		// Get the IPrivateWaveTrack interface
		IPrivateWaveTrack* pIPrivateWaveTrack;
		if( SUCCEEDED ( GetObject( CLSID_DirectMusicWaveTrack, IID_IPrivateWaveTrack, (void **)&pIPrivateWaveTrack ) ) )
		{
			POSITION pos = m_lstQueuedWaves.GetHeadPosition();
			POSITION posCur;
			while( pos )
			{
				posCur = pos;
				QueuedWave* pQueuedWave = m_lstQueuedWaves.GetNext( pos );

				if( pQueuedWave->lDownloadCount < 0 )
				{
					// Nothing to do, request is for unload
					continue;
				}

				while( pQueuedWave->lDownloadCount > 0 )
				{
					// Download the Wave
					HRESULT hr = pIPrivateWaveTrack->DownloadWave( pQueuedWave->pIDSWave, pIUnknown, pQueuedWave->guidVersion );
					ASSERT( SUCCEEDED ( hr ) );
					pQueuedWave->lDownloadCount--;
				}

				m_lstQueuedWaves.RemoveAt( posCur );
				delete pQueuedWave;
			}

			RELEASE( pIPrivateWaveTrack );
		}

		RELEASE( pIUnknown );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::FlushAllWaves

void CTrackMgr::FlushAllWaves( void )
{
	CWaitCursor wait;

	// Clear all items from m_lstQueuedWaves
	while( !m_lstQueuedWaves.IsEmpty() )
	{
		QueuedWave* pQueuedWave = m_lstQueuedWaves.RemoveHead();
		delete pQueuedWave;
	}

	// Get the IPrivateWaveTrack interface
	IPrivateWaveTrack* pIPrivateWaveTrack;
	if( SUCCEEDED ( GetObject( CLSID_DirectMusicWaveTrack, IID_IPrivateWaveTrack, (void **)&pIPrivateWaveTrack ) ) )
	{
		// Flush Waves
		HRESULT hr = pIPrivateWaveTrack->FlushAllWaves();
		ASSERT( SUCCEEDED ( hr ) );

		RELEASE( pIPrivateWaveTrack );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::ForceBoundaries

HRESULT CTrackMgr::ForceBoundaries( long lMeasure, long lBeat, MUSIC_TIME* pmtTime )
{
	// Get length of segment
	MUSIC_TIME mtMaxTimelineLength;
	VARIANT var;
	m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	mtMaxTimelineLength = V_I4( &var );

	// Get maximum number of measures, beats
	long lMaxMeasure;
	long lMaxBeat;
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtMaxTimelineLength - 1, &lMaxMeasure, &lMaxBeat );
	lMaxMeasure = max( 0, lMaxMeasure );
	lMaxBeat = max( 0, lMaxBeat );

	// Force boundaries 
	MUSIC_TIME mtTime;
	m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &mtTime );
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );

	// Enforce measure boundary
	if( lMeasure > lMaxMeasure )
	{
		lMeasure = lMaxMeasure;
		lBeat = lMaxBeat;
	}

	// Enforce beat boundary (must catch incomplete last measures - happens when importing MIDI files)
	if( lMeasure == lMaxMeasure 
	&&  lBeat > lMaxBeat )
	{
		lBeat = lMaxBeat;
	}

	// Make sure mtTime matches adjusted measure, tick
	m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &mtTime );
	*pmtTime = mtTime;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetGroupBits

DWORD CTrackMgr::GetGroupBits( void )
{
	return m_dwGroupBits;
}

// Helper function for RealTime display

void RefTimeToString( REFERENCE_TIME rtTime, int nResourceID, int nNegativeResourceID, CString &cstrTime )
{
	bool fNegative = false;
	if( rtTime < 0 )
	{
		fNegative = true;
		rtTime = -rtTime;
	}

	int iMillisecond, iSecond, iMinute;//, iHour;
	// Convert to milliseconds
	iMillisecond = int(rtTime / 10000);
	iSecond = iMillisecond / 1000;
	iMillisecond %= 1000;
	iMinute = iSecond / 60;
	iSecond %= 60;
	//iHour = iMinute / 60;
	//iMinute %= 60;

	CString strFormat;
	if( strFormat.LoadString( fNegative ? nNegativeResourceID : nResourceID ) )
	{
		cstrTime.Format(strFormat, iMinute, iSecond, iMillisecond);
	}
	else
	{
		cstrTime.Format(fNegative ? "-%02d:%02d.%03d" : "%02d:%02d.%03d", iMinute, iSecond, iMillisecond);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::UpdateStatusBarDisplay

void CTrackMgr::UpdateStatusBarDisplay( void )
{
	if( !m_pDMProdFramework )
	{
		return;
	}

	bool fShowStatusBar = false;
	CWaveStrip *pActiveWaveStrip = GetActiveStripInThisTrack();
	if( pActiveWaveStrip )
	{
		CTrackItem *pItem = pActiveWaveStrip->GetFirstSelectedItem();
		if( pItem )
		{
			fShowStatusBar = true;

			if( !m_hStatusBar
			&&	SUCCEEDED( m_pDMProdFramework->SetNbrStatusBarPanes( 1, SBLS_CONTROL, &m_hStatusBar ) ) )
			{
				m_pDMProdFramework->SetStatusBarPaneInfo( m_hStatusBar, 0, SBS_SUNKEN, 14 );
			}

			if( m_hStatusBar )
			{
				REFERENCE_TIME rtWave;
				if( SUCCEEDED( UnknownTimeToRefTime( pItem->m_rtTimePhysical, &rtWave ) ) )
				{
					CString strText;
					RefTimeToString( rtWave, IDS_WAVE_STATUS_TEXT, IDS_WAVE_NEG_STATUS_TEXT, strText );
					m_pDMProdFramework->SetStatusBarPaneText( m_hStatusBar, 0, strText.AllocSysString(), TRUE );
				}
			}
		}
	}


	if( !fShowStatusBar
	&&	m_hStatusBar )
	{
		m_pDMProdFramework->RestoreStatusBar( m_hStatusBar );
		m_hStatusBar = NULL;
	}
}
