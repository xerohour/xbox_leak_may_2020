// TrackMgr.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "newparameterdialog.h"
#include "GroupBitsPPG.h"
#include "TabObject.h"
#include "SegmentDesigner.h"
#include "SegmentIO.h"
#include "PChannelName.h"
#include "ParamStrip.h"
#include <dmusicf.h>
#include <dmusici.h>
#include "TrackFlagsPPG.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Set information about this strip manager
const CLSID CTrackMgr::m_clsid = CLSID_DirectMusicParamControlTrack;
const DWORD CTrackMgr::m_ckid = NULL;
const DWORD CTrackMgr::m_fccType = DMUS_FOURCC_PARAMCONTROLTRACK_TRACK_LIST;

const DWORD g_dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
const DWORD g_dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr constructor/destructor 

CTrackMgr::CTrackMgr() : CBaseMgr()
{
	m_pParamStripForPropSheet = NULL;
	m_pIDMPerformance = NULL;

	CTrackObject* pTrackObject = new CTrackObject(this);
	ASSERT(pTrackObject);
	if(pTrackObject)
	{
		m_lstTrackObjects.AddTail(pTrackObject);
	}

	m_dwTrackExtrasFlags = PARAMTRACK_DEFAULT & TRACKCONFIG_VALID_MASK;
}

CTrackMgr::~CTrackMgr()
{
	// Release all the items in m_lstTrackObjects
	while( !m_lstTrackObjects.IsEmpty() )
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.RemoveHead();
		delete pTrackObject;
	}

	if( m_pIDMPerformance )
	{
		RELEASE( m_pIDMPerformance );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr ITrackMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::IsMeasureBeatOpen
// Returns S_OK if the specified measure and beat is empty.
// Returns S_FALSE if the specified measure and beat already has an item

HRESULT STDMETHODCALLTYPE CTrackMgr::IsMeasureBeatOpen( long lMeasure, long lBeat )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( 0 );
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CTrackMgr::OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = E_FAIL;

	// If the update isn't for our strip, exit
	if( !(dwGroupBits & m_dwGroupBits) )
	{
		return E_INVALIDARG;
	}

	// TimeSig change
	if( ::IsEqualGUID( rguidType, GUID_TimeSignature ) )
	{
		// Update the m_mtTime value of all items
		if( AllObjects_RecomputeTimes() )
		{
			OnDataChanged();
		}

		// Redraw all strips
		AllObjects_Invalidate();
		return S_OK;
	}

	// Tempo change
	if( ::IsEqualGUID( rguidType, GUID_TempoParam ) )
	{
		// Redraw all strips
		AllObjects_Invalidate();
		return S_OK;
	}

	if(::IsEqualGUID(rguidType, GUID_Conductor_OutputPortsChanged))
	{
		// Redraw all strips
		AllObjects_RemoveAllStripsFromTimeline();
		AllObjects_RefreshInfo();
		AllObjects_Invalidate();

		// This notification also comes in when we're shutting down
		if(m_pTimeline)
		{
			AllObjects_AddAllStripsToTimeline();
		}
		return S_OK;

	}

	// All Tracks Added
	if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		// Fix measure/beat of all track items
		AllObjects_RecomputeMeasureBeats();

		// Make sure host has latest version of data
		// May have changed during load if user prompted to choose Wave from File Open dialog
		if( m_fDirty )
		{
			OnDataChanged();
			m_fDirty = FALSE;
		}

		// Ask all the objects to update their info
		AllObjects_RefreshInfo();	

		// Redraw all strips
		AllObjects_Invalidate();
		return S_OK;
	}

	// Deleted Track
	if( ::IsEqualGUID( rguidType, GUID_Segment_DeletedTrack ) )
	{
		DeleteAllObjects();
		return S_OK;
	}

	// Framework message
	if( ::IsEqualGUID(rguidType, GUID_Segment_FrameworkMsg) )
	{
		AllObjects_OnUpdate( rguidType, dwGroupBits, pData );
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
		POSITION pos = m_lstTrackObjects.GetHeadPosition();
		while( pos )
		{
			CTrackObject* pTrackObject = m_lstTrackObjects.GetNext( pos );

			POSITION posTrack = pTrackObject->m_lstParamStrips.GetHeadPosition();
			while( posTrack )
			{
				CParamStrip* pParamStrip = pTrackObject->m_lstParamStrips.GetNext( posTrack );

				pParamStrip->m_StripUI.m_nSnapToMusicTime = wSnapVal;

				// If switching to none or grid, update the reftime snap-to setting also
				if( (wSnapVal == IDM_SNAP_NONE)
				||	(wSnapVal == IDM_SNAP_GRID) )
				{
					pParamStrip->m_StripUI.m_nSnapToRefTime = wSnapVal;
				}
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
		{
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
				AllObjects_RemoveAllStripsFromTimeline();

				// Remove ourself from the Timeline's notification list
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TempoParam, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetSnapTo, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Conductor_OutputPortsChanged, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Conductor_OutputPortsRemoved, m_dwOldGroupBits );

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
					AllObjects_AddAllStripsToTimeline();

					// Add ourself to the Timeline's notification list
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TempoParam, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetSnapTo, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Conductor_OutputPortsChanged, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Conductor_OutputPortsRemoved, m_dwGroupBits );
				}
			}
			return S_OK;
		}
		case SMP_DMUSIOTRACKEXTRASHEADER:
		{
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
	}

	// Let CBaseMgr handle
	return CBaseMgr::SetStripMgrProperty( stripMgrProperty, variant );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::Load

HRESULT CTrackMgr::Load(IStream* pIStream)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Verify that the stream pointer is non-null
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr = E_FAIL;

	// Try and allocate a RIFF stream
	if(FAILED(hr = AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		return hr;
	}

	// Delete all the items in m_lstTrackObjects
	while(!m_lstTrackObjects.IsEmpty())
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.RemoveHead();
		delete pTrackObject;
	}

	// Variables used when loading the Wave track
	MMCKINFO ckTrack;
	MMCKINFO ckList;

	// Interate through every chunk in the stream
	while(pIRiffStream->Descend(&ckTrack, NULL, 0) == 0)
	{
		switch(ckTrack.ckid)
		{
			case FOURCC_LIST:
			{
				switch(ckTrack.fccType)
				{
					case DMUS_FOURCC_PARAMCONTROLTRACK_TRACK_LIST:
					{
						while(pIRiffStream->Descend(&ckList, &ckTrack, 0) == 0)
						{
							switch(ckList.ckid)
							{
								case FOURCC_LIST:
								{
									switch(ckList.fccType)
									{
										case DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_LIST:
										{
											CTrackObject* pNewTrackObject = new CTrackObject(this);
											if(pNewTrackObject == NULL)
											{
												hr = E_OUTOFMEMORY;
												goto ON_ERROR;
											}
											hr = pNewTrackObject->LoadObject(pIRiffStream, &ckList);
											if( FAILED ( hr ) )
											{
												delete pNewTrackObject;
												goto ON_ERROR;
											}
											InsertInAlphabeticalOrder(&m_lstTrackObjects, pNewTrackObject );
											break;
										}
									}
									break;
								}
							}

							pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
					}
				}
				break;
			}
		}

		pIRiffStream->Ascend( &ckTrack, 0 );
	}

	// If we're here without errors and there are jo objects then this must be an empty track?
	if(m_lstTrackObjects.IsEmpty())
	{
		CTrackObject* pTrackObject = new CTrackObject(this);
		ASSERT(pTrackObject);
		if(pTrackObject)
		{
			m_lstTrackObjects.AddTail(pTrackObject);
		}
	}

	SyncWithDirectMusic();

ON_ERROR:
	pIRiffStream->Release();
	return hr;
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
	// stream that will be loaded into a DirectMusic parameter control track (GUID_DirectMusicObject)
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
 	ckTrack.fccType = DMUS_FOURCC_PARAMCONTROLTRACK_TRACK_LIST;
	if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the LIST chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}


	// Save all objects in the parameter control track
	if( !m_lstTrackObjects.IsEmpty() )
	{
		// Iterate through the object list
		POSITION pos = m_lstTrackObjects.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each item
			CTrackObject* pTrackObject = m_lstTrackObjects.GetNext( pos );

			// Save each object
			hr = pTrackObject->SaveObject( pIRiffStream );
			if(FAILED(hr))
			{
				goto ON_ERROR;
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

	if( m_pParamStripForPropSheet == NULL )
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
		// Object tab
		case 2:
		{
			ioObjectPPG *pioObjectPPG = static_cast<ioObjectPPG *>(*ppData);
			
			CTrackObject* pTrackObject = m_pParamStripForPropSheet->GetTrackObject();
			ASSERT(pTrackObject);
			if(pTrackObject)
			{
				pTrackObject->RefreshObjectProperties();
				pioObjectPPG->strStageName = pTrackObject->m_sStageName;
				pioObjectPPG->strObjectName = pTrackObject->m_sObjectName;
				pioObjectPPG->dwPChannel = pTrackObject->m_dwPChannel;
				pioObjectPPG->strPChannelText = pTrackObject->m_sPChannelText;
				pioObjectPPG->dwParamFlagsDM = m_pParamStripForPropSheet->GetDMParamFlags();
				pioObjectPPG->strStripName = m_pParamStripForPropSheet->GetName();
				GetPChannelName(pTrackObject->m_dwPChannel, pioObjectPPG->strPChannelName );
			}
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

	if( m_pParamStripForPropSheet == NULL )
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
			if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_dwTrackExtrasFlags )
			{
				m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
				bool fWasRefTimeTrack = IsRefTimeTrack();
				m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;
				bool fIsRefTimeTrack = IsRefTimeTrack();

				if( fWasRefTimeTrack != fIsRefTimeTrack )
				{
					AllObjects_SwitchTimeBase();
				}

				OnDataChanged();
				AllObjects_Invalidate();
			}
			else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_dwProducerOnlyFlags )
			{
				m_nLastEdit = IDS_UNDO_PRODUCERONLY;
				m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;
				OnDataChanged();
			}
			return S_OK;
			break;
		}

		// Object tab
		case 2:
		{
			ioObjectPPG *pioObjectPPG = static_cast<ioObjectPPG *>(pData);

			CTrackObject* pTrackObject = m_pParamStripForPropSheet->GetTrackObject();
			ASSERT(pTrackObject);
			if(pTrackObject == NULL)
			{
				return E_UNEXPECTED;
			}

			// m_pParamStripForPropSheet->m_dwPChannel
			if( pioObjectPPG->dwPChannel != pTrackObject->m_dwPChannel )
			{
				m_nLastEdit = IDS_UNDO_OBJECT_PCHANNEL;
				pTrackObject->m_dwPChannel = pioObjectPPG->dwPChannel;
				OnDataChanged();
				m_pTimeline->StripInvalidateRect( m_pParamStripForPropSheet, NULL, FALSE );
			}

			// m_pParamStripForPropSheet->m_dwParamFlagsDM
			else if( pioObjectPPG->dwParamFlagsDM != m_pParamStripForPropSheet->GetDMParamFlags())
			{
				m_nLastEdit = IDS_UNDO_OBJECT_FLAGS;
				m_pParamStripForPropSheet->SetDMParamFlags(pioObjectPPG->dwParamFlagsDM);
				OnDataChanged();
			}

			// m_pParamStripForPropSheet->m_strStripName
			else if( pioObjectPPG->strStripName != m_pParamStripForPropSheet->GetName())
			{
				m_nLastEdit = IDS_UNDO_OBJECT_NAME;
				m_pParamStripForPropSheet->SetName(pioObjectPPG->strStripName);
				OnDataChanged();
				m_pTimeline->StripInvalidateRect( m_pParamStripForPropSheet, NULL, FALSE );
			}

			// m_pParamStripForPropSheet's PChannel name
			else if( m_pDMProdFramework )
			{
				IDMUSProdProject* pIProject;
				if( SUCCEEDED ( m_pDMProdFramework->FindProject( m_pIDocRootNode, &pIProject ) ) ) 
				{
					IDMUSProdPChannelName* pIPChannelName;
					if( SUCCEEDED( pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&pIPChannelName ) ) )
					{
						WCHAR wstrText[MAX_PATH];
						if( SUCCEEDED ( pIPChannelName->GetPChannelName( pTrackObject->m_dwPChannel, wstrText ) ) )
						{
							CString strPChName = wstrText;
							if( strPChName.Compare( pioObjectPPG->strPChannelName ) != 0 )
							{
								MultiByteToWideChar( CP_ACP, 0, pioObjectPPG->strPChannelName, -1, wstrText, MAX_PATH );
								pIPChannelName->SetPChannelName( pTrackObject->m_dwPChannel, wstrText );

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

		if( m_pIDMPerformance == NULL )
		{
			ASSERT( 0 );
			return E_FAIL;
		}
	}

	// Hand DirectMusic new parameter control track
	return CBaseMgr::SyncWithDirectMusic();
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::RecomputeTimes

// Return true if anything changed
bool CTrackMgr::RecomputeTimes( void )
{
	return AllObjects_RecomputeTimes();
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SetDirtyFlag

void CTrackMgr::SetDirtyFlag( bool fDirty )
{
	m_fDirty = fDirty;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::InsertByAscendingPChannel

void CTrackMgr::InsertByAscendingPChannel( CTrackObject *pTrackObjectToInsert )
{
	// Ensure the pTrackObjectToInsert pointer is valid
	if ( pTrackObjectToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	CTrackObject *pTrackObject;
	POSITION posCurrent, posNext = m_lstTrackObjects.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pTrackObject = m_lstTrackObjects.GetNext( posNext );

		if( pTrackObject->m_dwPChannel > pTrackObjectToInsert->m_dwPChannel )
		{
			// insert before posCurrent (which is the position of pTrackObject)
			m_lstTrackObjects.InsertBefore( posCurrent, pTrackObjectToInsert );
			return;
		}
	}

	// pTrackObjectToInsert has higher PChannel than all items in the list
	// add it at the end of the list
	m_lstTrackObjects.AddTail( pTrackObjectToInsert );
	return;
}

void CTrackMgr::InsertInAlphabeticalOrder(CPtrList* plstObjects, CTrackObject* pTrackObject)
{
	ASSERT(plstObjects);
	if(plstObjects == NULL)
	{
		return;
	}

	ASSERT(pTrackObject);
	if(pTrackObject == NULL)
	{
		return;
	}
	
	POSITION position = plstObjects->GetHeadPosition();
	while(position)
	{
		POSITION insertPosition = position;
		CTrackObject* pObjectInList = (CTrackObject*) plstObjects->GetNext(position);
		ASSERT(pObjectInList);
		if(pObjectInList)
		{
			if(CompareObjectAlphabetical(pTrackObject, pObjectInList) <= 0)
			{
				plstObjects->InsertBefore(insertPosition, pTrackObject);
				return;
			}
		}
	}

	// This will go to the end of the list
	plstObjects->AddTail(pTrackObject);
}

CPtrList* CTrackMgr::SortListAlphabetically(CPtrList* plstObjects)
{
	ASSERT(plstObjects);
	if(plstObjects == NULL)
	{
		return NULL;
	}

	CPtrList* pSorted = new CPtrList();
	POSITION position = plstObjects->GetHeadPosition();
	while(position)
	{
		CTrackObject* pTrackObject = (CTrackObject*)plstObjects->GetNext(position);
		ASSERT(pTrackObject);
		if(pTrackObject)
		{
			InsertInAlphabeticalOrder(pSorted, pTrackObject);
		}
	}
	
	return pSorted;
}


int CTrackMgr::CompareObjectAlphabetical(CTrackObject* pTrackObject1, CTrackObject* pTrackObject2)
{
	ASSERT(pTrackObject1);
	if(pTrackObject1 == NULL)
	{
		return -1;
	}

	ASSERT(pTrackObject2);
	if(pTrackObject2 == NULL)
	{
		return -1;
	}

	CString sName1 = pTrackObject1->GetName();
	CString sName2 = pTrackObject2->GetName();

	return sName1.CompareNoCase(sName2);
}

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::ClocksToMeasureBeatTick

HRESULT CTrackMgr::ClocksToMeasureBeatTick( MUSIC_TIME mtTime,
											long* plMeasure, long* plBeat, long* plTick )
{
	ASSERT( plMeasure != NULL );
	ASSERT( plBeat != NULL );
	ASSERT( plTick != NULL );
	
	long lMeasure = -1;
	long lBeat = -1;
	long lTick = -1;

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
					lTick = mtTime - lClocks;

					// Try and preserve negative tick offsets
					if( lTick > 0 )
					{
						long lNewClocks;
						long lClocksPerBeat;

						hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat+1, &lNewClocks );
						if( SUCCEEDED ( hr ) )
						{
							lClocksPerBeat = lNewClocks - lClocks;

							if( lTick >= (lClocksPerBeat >> 1) )
							{
								// Get max clock
								VARIANT var;
								m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
								MUSIC_TIME mtMaxTimelineLength = V_I4( &var );

								if( lNewClocks < mtMaxTimelineLength )
								{
									lTick -= lClocksPerBeat;

									hr = m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lNewClocks, &lMeasure, &lBeat );
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
	*plTick	= lTick;

	ASSERT( SUCCEEDED ( hr ) );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::MeasureBeatTickToClocks

HRESULT CTrackMgr::MeasureBeatTickToClocks( long lMeasure, long lBeat, long lTick,
											MUSIC_TIME* pmtTime )
{
	ASSERT( pmtTime != NULL );

	MUSIC_TIME mtTime = -1;
	long lClocks;

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
			mtTime = lClocks + lTick;
		}
	}

	*pmtTime = mtTime;

	ASSERT( SUCCEEDED ( hr ) );
	return hr;
}


HRESULT CTrackMgr::AddNewObject()
{
	// Throw up a dialog that gets the stage, object and the parameter for the new strip
	CNewParameterDialog dlgParam(this);
	if(dlgParam.DoModal() == IDOK)
	{
		// Collect all the information from the dialog
		StripInfo newStripInfo;
		newStripInfo = dlgParam.m_StripInfo;

		CTrackObject* pTrackObject = NULL;
		POSITION pos = m_lstTrackObjects.GetHeadPosition();
		while(pos)
		{
			pTrackObject = (CTrackObject*)m_lstTrackObjects.GetNext(pos);
			if(pTrackObject->IsEqualObject(&newStripInfo) == TRUE)
			{
				break;
			}

			pTrackObject = NULL;
		}

		if(pTrackObject == NULL && m_lstTrackObjects.GetCount() == 1)
		{
			CTrackObject* pObject = (CTrackObject*)m_lstTrackObjects.GetHead();
			ASSERT(pObject);
			if(pObject && pObject->IsEmptyTrackObject() == TRUE)
			{
				pTrackObject = pObject;
			}

		}

		bool bNewObject = false;
		if(pTrackObject == NULL)
		{
			pTrackObject = new CTrackObject(this);
			ASSERT(pTrackObject);
			if(pTrackObject == NULL)
			{
				return E_OUTOFMEMORY;
			}
			
			bNewObject = true;
		}

		HRESULT hr = AddNewObject(pTrackObject, &newStripInfo);
		if(FAILED(hr))
		{
			if(bNewObject)
			{
				delete pTrackObject;
			}
			return hr;
		}


		if(bNewObject)
		{
			InsertInAlphabeticalOrder(&m_lstTrackObjects, pTrackObject);
		}
	
		return S_OK;

	}
	
	return E_FAIL;
}


HRESULT CTrackMgr::AddNewObject(CTrackObject* pTrackObject, StripInfo* pStripInfo)
{
	ASSERT(pTrackObject);
	if(pTrackObject == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pStripInfo);
	if(pStripInfo == NULL)
	{
		return E_POINTER;
	}

	HRESULT hr = pTrackObject->AddStrip(pStripInfo);
	return hr;

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
// CTrackMgr::PChannelExists

bool CTrackMgr::PChannelExists( DWORD dwPChannel )
{
	POSITION pos = m_lstTrackObjects.GetHeadPosition();
	while( pos )
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext( pos );

		if( pTrackObject->m_dwPChannel == dwPChannel )
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

	POSITION pos = m_lstTrackObjects.GetHeadPosition();
	while( pos )
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext( pos );

		dwPChannel = max( dwPChannel, pTrackObject->m_dwPChannel );
	}

	return dwPChannel;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllObjects_OnUpdate

void CTrackMgr::AllObjects_OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData )
{
	POSITION pos = m_lstTrackObjects.GetHeadPosition();
	while( pos )
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext( pos );

		pTrackObject->AllStrips_OnUpdate( rguidType, dwGroupBits, pData );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllObjects_Invalidate

void CTrackMgr::AllObjects_RefreshInfo()
{
	POSITION pos = m_lstTrackObjects.GetHeadPosition();
	while( pos )
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext( pos );
		ASSERT(pTrackObject);
		if(pTrackObject)
		{
			pTrackObject->RefreshObjectProperties();
		}
	}

	// Sort the track objects list by names
	CPtrList* plstSorted = SortListAlphabetically(&m_lstTrackObjects);
	if(plstSorted)
	{
		while(!m_lstTrackObjects.IsEmpty())
		{
			m_lstTrackObjects.RemoveHead();
		}

		while(!plstSorted->IsEmpty())
		{
			CTrackObject* pTrackObject = (CTrackObject*)plstSorted->RemoveHead();
			ASSERT(pTrackObject);
			if(pTrackObject)
			{
				m_lstTrackObjects.AddTail(pTrackObject);
			}
		}

		delete plstSorted;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllObjects_Invalidate

void CTrackMgr::AllObjects_Invalidate( void )
{
	POSITION pos = m_lstTrackObjects.GetHeadPosition();
	while( pos )
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext( pos );

		pTrackObject->AllStrips_Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllObjects_AddAllStripsToTimeline

void CTrackMgr::AllObjects_AddAllStripsToTimeline( void )
{

	POSITION positionObject = m_lstTrackObjects.GetHeadPosition();
	while(positionObject)
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext(positionObject);
		ASSERT(pTrackObject);
		if(pTrackObject)
		{
			pTrackObject->AllStrips_AddToTimeline();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllObjects_RecomputeMeasureBeats

void CTrackMgr::AllObjects_RecomputeMeasureBeats()
{
	POSITION pos = m_lstTrackObjects.GetHeadPosition();
	while( pos )
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext( pos );

		pTrackObject->AllStrips_RecomputeMeasureBeats();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllObjects_RecomputeTimes

bool CTrackMgr::AllObjects_RecomputeTimes()
{
	bool fChanged = FALSE;

	POSITION pos = m_lstTrackObjects.GetHeadPosition();
	while( pos )
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext( pos );

		fChanged |= pTrackObject->AllStrips_RecomputeTimes();
	}

	return fChanged;
}


void CTrackMgr::AllObjects_RemoveAllStripsFromTimeline()
{
	POSITION pos = m_lstTrackObjects.GetHeadPosition();
	while(pos)
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext(pos);
		ASSERT(pTrackObject);
		if(pTrackObject)
		{
			pTrackObject->AllStrips_RemoveFromTimeline();
		}
	}
}


void CTrackMgr::AllObjects_SwitchTimeBase()
{
	POSITION position = m_lstTrackObjects.GetHeadPosition();
	while(position)
	{
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext(position);
		ASSERT(pTrackObject);
		if(pTrackObject)
		{
			pTrackObject->AllStrips_SwitchTimeBase();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::DeleteAllObjects

void CTrackMgr::DeleteAllObjects()
{
	POSITION pos = m_lstTrackObjects.GetHeadPosition();
	while(!m_lstTrackObjects.IsEmpty())
	{
		CTrackObject* pTrackObject = (CTrackObject*)m_lstTrackObjects.RemoveHead();
		ASSERT(pTrackObject);
		if(pTrackObject)
		{
			DeleteAllStripsInObject(pTrackObject);
			delete pTrackObject;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::DeleteSelectedStripsFromObjects

void CTrackMgr::DeleteSelectedStripsFromObjects()
{
	POSITION position = m_lstTrackObjects.GetHeadPosition();
	while(position)
	{
		POSITION objectPos = position;
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext(position);
		ASSERT(pTrackObject);
		if(pTrackObject && pTrackObject->IsEmptyTrackObject() == FALSE)
		{
			pTrackObject->DeleteAllSelectedStrips();
		}
	}

	position = m_lstTrackObjects.GetHeadPosition();
	while(position)
	{
		POSITION removePosition = position;
		CTrackObject* pTrackObject = m_lstTrackObjects.GetNext(position);
		ASSERT(pTrackObject);
		if(pTrackObject && pTrackObject->GetNumberOfStrips() == 0)
		{
			m_lstTrackObjects.RemoveAt(removePosition);
			delete pTrackObject;
		}
	}

	if(m_lstTrackObjects.IsEmpty())
	{
		CTrackObject* pTrackObject = new CTrackObject(this);
		ASSERT(pTrackObject);
		if(pTrackObject)
		{
			m_lstTrackObjects.AddTail(pTrackObject);
			pTrackObject->AllStrips_AddToTimeline();
		}
	}

	// Save the undo state
	//SetLastEdit(IDS_UNDO_DELETE_PARAM);
	//TrackDataChanged();
	NotifyDataChange(IDS_UNDO_DELETE_PARAM);

	if(m_lstTrackObjects.IsEmpty())
	{
		IDMUSProdSegmentEdit8* pISegmentEdit8 = GetSegment();
		ASSERT(pISegmentEdit8);
		if(pISegmentEdit8 == NULL)
		{
			return;
		}

		pISegmentEdit8->RemoveStripMgr((IDMUSProdStripMgr *)this);
		RELEASE(pISegmentEdit8);
	}

}

HRESULT CTrackMgr::DeleteAllStripsInObject(CTrackObject* pTrackObject)
{
	ASSERT(pTrackObject);
	if(pTrackObject == NULL)
	{
		return E_POINTER;
	}

	if(FAILED(pTrackObject->DeleteAllStrips()))
	{
		return E_FAIL;
	}

	if(m_lstTrackObjects.IsEmpty())
	{
		IDMUSProdSegmentEdit8* pISegmentEdit8 = GetSegment();
		ASSERT(pISegmentEdit8);
		if(pISegmentEdit8 == NULL)
		{
			return E_UNEXPECTED;
		}

		pISegmentEdit8->RemoveStripMgr((IDMUSProdStripMgr *)this);
		RELEASE(pISegmentEdit8);
		return S_OK;		// Call to RemoveStripMgr should have deleted us!
	}
	else
	{
		// Update Undo queue
		m_nLastEdit = IDS_UNDO_DELETE_PART;
		OnDataChanged();
	}


	return S_OK;
}

IDMUSProdFramework* CTrackMgr::GetFramework()
{
	return m_pDMProdFramework;
}

IDMUSProdConductor* CTrackMgr::GetConductor()
{
	if( m_pDMProdFramework )
	{
		IDMUSProdComponent* pIComponent = NULL;
		if(SUCCEEDED(m_pDMProdFramework->FindComponent(CLSID_CConductor,  &pIComponent)))
		{
			IDMUSProdConductor* pIConductor = NULL;
			if(SUCCEEDED(pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pIConductor)))
			{
				RELEASE(pIComponent);
				return pIConductor;

			}

			RELEASE(pIComponent);
		}
	}

	return NULL;
}


IDMUSProdTimeline* CTrackMgr::GetTimeline()
{
	return m_pTimeline;
}

CParamStrip* CTrackMgr::GetParamStripForPropSheet()
{
	return m_pParamStripForPropSheet;
}

void CTrackMgr::RemoveFromPropSheet(CParamStrip* pParamStrip)
{
	ASSERT(pParamStrip);
	if(pParamStrip == NULL)
	{
		return;
	}

	if(pParamStrip == m_pParamStripForPropSheet)
	{
		m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);
		RELEASE(m_pPropPageMgr);
	}
}

CLSID CTrackMgr::GetCLSID()
{
	return m_clsid;
}

DWORD CTrackMgr::GetGroupBits()
{
	return m_dwGroupBits;
}

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::IsRefTimeTrack

bool CTrackMgr::IsRefTimeTrack()
{
	return (m_dwTrackExtrasFlags & DMUS_TRACKCONFIG_PLAY_CLOCKTIME) ? true : false;
}


IDMUSProdSegmentEdit8* CTrackMgr::GetSegment()
{
	ASSERT(m_pIDocRootNode != NULL);
	if(m_pIDocRootNode)
	{
		IDMUSProdSegmentEdit8* pISegmentEdit8 = NULL;
		if(SUCCEEDED(m_pIDocRootNode->QueryInterface(IID_IDMUSProdSegmentEdit8, (void **)&pISegmentEdit8)))
		{
			return pISegmentEdit8;		
		}
	}

	return NULL;
}



/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::RefTimeToMinSecGridMs

HRESULT	CTrackMgr::RefTimeToMinSecGridMs( CParamStrip* pParamStrip,
										  REFERENCE_TIME rtTime,
										  long* plMinute, long* plSecond, long* plGrid, long* plMillisecond )
{
	ASSERT( pParamStrip != NULL );
	ASSERT( plMinute != NULL );
	ASSERT( plSecond != NULL );
	ASSERT( plGrid != NULL );
	ASSERT( plMillisecond != NULL );

	REFERENCE_TIME rtRefClocksPerGrid = REFCLOCKS_PER_SECOND / pParamStrip->m_StripUI.m_nSnapToRefTime_GridsPerSecond;

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

HRESULT	CTrackMgr::MinSecGridMsToRefTime( CParamStrip* pParamStrip,
										  long lMinute, long lSecond, long lGrid, long lMillisecond,
										  REFERENCE_TIME* prtTime )
{
	ASSERT( pParamStrip != NULL );
	ASSERT( prtTime != NULL );

	REFERENCE_TIME rtRefClocksPerGrid = REFCLOCKS_PER_SECOND / pParamStrip->m_StripUI.m_nSnapToRefTime_GridsPerSecond;

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
// CTrackMgr::ClocksToUnknownTime

HRESULT CTrackMgr::ClocksToUnknownTime( MUSIC_TIME mtTime, REFERENCE_TIME* prtTime )
{
	if( m_pTimeline == NULL )
	{
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
// CTrackMgr::GetSegmentLength

REFERENCE_TIME CTrackMgr::GetSegmentLength( void )
{
	MUSIC_TIME mtSegmentLength = LONG_MAX;

	VARIANT varLength;
	if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
	{
		mtSegmentLength = V_I4(&varLength);
	}

	REFERENCE_TIME rtSegmentLength;
	ClocksToUnknownTime( mtSegmentLength, &rtSegmentLength );

	return rtSegmentLength;
}

void CTrackMgr::NotifyDataChange(UINT nLastEdit)
{
	m_nLastEdit = nLastEdit;
	OnDataChanged();
	SyncWithDirectMusic();
}


void CTrackMgr::SetParamStripForPropSheet(CParamStrip* pParamStrip, BOOL bSetToNULL)
{
	if(bSetToNULL)
	{
		if(m_pParamStripForPropSheet == pParamStrip)
		{
			m_pParamStripForPropSheet = NULL;
			return;
		}
	}
	else
	{
		m_pParamStripForPropSheet = pParamStrip;
	}


}

IDMUSProdNode* CTrackMgr::GetDocRootNode()
{
	return m_pIDocRootNode;
}

void CTrackMgr::TrackDataChanged()
{
	OnDataChanged();
}

IDMUSProdPropPageManager* CTrackMgr::GetPropPageMgr()
{
	return m_pPropPageMgr;
}

UINT CTrackMgr::GetLastEdit()
{
	return m_nLastEdit;
}

void CTrackMgr::SetLastEdit(UINT nLastEdit)
{
	m_nLastEdit = nLastEdit;
}