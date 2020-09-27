// TrackMgr.cpp : implementation file
//

/*--------------
@doc SCRIPTSAMPLE
--------------*/

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "PropPageMgr_Item.h"
#include "SegmentDesigner.h"
#include <dmusicf.h>
#include <dmusicp.h>

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Set information about this strip manager
const CLSID CTrackMgr::m_clsid = CLSID_DirectMusicScriptTrack;
const DWORD CTrackMgr::m_ckid = NULL;
const DWORD CTrackMgr::m_fccType = DMUS_FOURCC_SCRIPTTRACK_LIST;

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr constructor/destructor 

CTrackMgr::CTrackMgr() : CBaseMgr()
{
	// Create a ScriptStrip
	m_pScriptStrip = new CScriptStrip(this);
	ASSERT( m_pScriptStrip );

	m_dwGroupBits = 0xFFFFFFFF;		// Script tracks belong to all track groups
	m_dwOldGroupBits = 0xFFFFFFFF;

	// Copy the pointer to the base strip manager
	m_pBaseStrip = m_pScriptStrip;

	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
}

CTrackMgr::~CTrackMgr()
{
	// Clean up our references
	if( m_pScriptStrip )
	{
		m_pScriptStrip->Release();
		m_pScriptStrip = NULL;
	}

	// Clear the base strip manager's pointer to the strip
	m_pBaseStrip = NULL;

	// Delete all the items in m_lstTrackItems
	EmptyList( m_lstTrackItems );
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

		// Iterate through all script items
		DWORD dwIndex = 0;
		POSITION pos = m_lstTrackItems.GetHeadPosition();
		while( pos )
		{
			CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

			if( pItem->m_FileRef.pIDocRootNode )
			{
				if( pDMUSProdReferencedNodes->apIDMUSProdNode
				&&	pDMUSProdReferencedNodes->dwArraySize > dwIndex )
				{
					pDMUSProdReferencedNodes->apIDMUSProdNode[dwIndex] = pItem->m_FileRef.pIDocRootNode;
					pDMUSProdReferencedNodes->apIDMUSProdNode[dwIndex]->AddRef();
				}

				// Increment the number of nodes we've found
				dwIndex++;
			}
		}

		HRESULT hr = pDMUSProdReferencedNodes->apIDMUSProdNode && (pDMUSProdReferencedNodes->dwArraySize < dwIndex) ? S_FALSE : S_OK;

		// Store the number of nodes we returned (or that we require)
		pDMUSProdReferencedNodes->dwArraySize = dwIndex;

		return hr;
	}

	return CBaseMgr::GetParam( guidType, mtTime, pmtNext, pData );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::IsParamSupported

HRESULT STDMETHODCALLTYPE CTrackMgr::IsParamSupported( REFGUID guidType )
{
    // We support getting the referenced nodes
	if( ::IsEqualGUID( guidType, GUID_Segment_ReferencedNodes ) )
	{
		return S_OK;
	}

	return CBaseMgr::IsParamSupported( guidType );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr IScriptMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::IsMeasureBeatOpen
// Returns S_OK if the specified measure and beat is empty.
// Returns S_FALSE if the specified measure and beat already has an item

HRESULT STDMETHODCALLTYPE CTrackMgr::IsMeasureBeatOpen( long lMeasure, long lBeat )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem;
		pItem = m_lstTrackItems.GetNext( pos );

		// Since the list is sorted, if this item is later than the specified time, we can exit
		if( pItem->m_lMeasure > lMeasure )
		{
			// The measure and beat are empty
			return S_OK;
		}
		// Check if the measure and beat values match
		else if( pItem->m_lMeasure == lMeasure &&
				 pItem->m_lBeat == lBeat )
		{
			// They match - return that the measure and beat are already occupied
			return S_FALSE;
		}
	}

	// The measure and beat are empty
	return S_OK;
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

	// All Tracks Added
	if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		// Fix measure/beat of all Script routines
		RecomputeMeasureBeats();

		// Make sure host has latest version of data
		// May have changed during load if user prompted to choose Script from File Open dialog
		if( m_fDirty )
		{
			OnDataChanged();
			m_fDirty = FALSE;
		}

		m_pTimeline->StripInvalidateRect( m_pScriptStrip, NULL, TRUE );
		return S_OK;
	}

	// Tempo change
	if( ::IsEqualGUID( rguidType, GUID_TempoParam ) )
	{
		m_pTimeline->StripInvalidateRect( m_pScriptStrip, NULL, TRUE );
		return S_OK;
	}

	// Framework message
	if( ::IsEqualGUID(rguidType, GUID_Segment_FrameworkMsg) )
	{
		DMUSProdFrameworkMsg* pFrameworkMsg =  (DMUSProdFrameworkMsg *)pData;
		ASSERT( pFrameworkMsg != NULL );

		IDMUSProdNode* pINode = NULL;

		if( pFrameworkMsg->punkIDMUSProdNode )
		{
			if( FAILED ( pFrameworkMsg->punkIDMUSProdNode->QueryInterface( IID_IDMUSProdNode, (void**)&pINode ) ) )
			{
				pINode = NULL;
			}
		}

		if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileDeleted)  
		||  ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileClosed) )
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find the items that changed and update them
				POSITION pos = m_lstTrackItems.GetHeadPosition();
				while( pos )
				{
					CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

					if( pItem
					&&  pItem->m_FileRef.pIDocRootNode == pINode )
					{
						// This Script was removed from the Project Tree
						pItem->SetFileReference( NULL );

						// Set undo text resource id
						if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileDeleted) )
						{
							m_nLastEdit = IDS_UNDO_DELETE_SCRIPT;
							hr = S_OK;
						}
						else
						{
							hr = S_FALSE;
						}
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileReplaced) ) 
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find the items that changed and update them
				POSITION pos = m_lstTrackItems.GetHeadPosition();
				while( pos )
				{
					CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

					if( pItem
					&&  pItem->m_FileRef.pIDocRootNode == pINode )
					{
						// This Script was replaced in the Project Tree, set to new Script pointer
						pItem->SetFileReference( (IDMUSProdNode *)pFrameworkMsg->pData );

						// This Script may have different routine names!!!
						// AMC TODO ????

						m_nLastEdit = IDS_UNDO_REPLACE_SCRIPT;
						hr = S_OK;
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, DOCROOT_GuidChange) 
			 ||  ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileNameChange) ) 
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find the items that changed and update them
				POSITION pos = m_lstTrackItems.GetHeadPosition();
				while( pos )
				{
					CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

					if( pItem
					&&  pItem->m_FileRef.pIDocRootNode == pINode )
					{
						// Set undo text resource id
						m_nLastEdit = IDS_UNDO_CHANGE_SCRIPT_LINK;
						hr = S_OK;
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, SCRIPT_NameChange) ) 
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find the items that changed and update them
				POSITION pos = m_lstTrackItems.GetHeadPosition();
				while( pos )
				{
					CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

					if( pItem
					&&  pItem->m_FileRef.pIDocRootNode == pINode )
					{
						// This Script was renamed so update Script list info fields
						hr = pItem->SetListInfo( m_pDMProdFramework );

						// Set undo text resource id
						m_nLastEdit = IDS_UNDO_CHANGE_SCRIPT_NAME;
						hr = S_OK;
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileLoadFinished) )
		{
			hr = E_FAIL;

			GUID guidFile = *(GUID *)pFrameworkMsg->pData;

			POSITION pos = m_lstTrackItems.GetHeadPosition();
			while( pos )
			{
				CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

				if( ::IsEqualGUID( pItem->m_FileRef.li.guidFile, guidFile ) )
				{ 
					ASSERT( pItem->m_FileRef.pIDocRootNode == NULL );
					ASSERT( pItem->m_wFlagsUI & RF_PENDING_LOAD );
					pItem->SetFileReference( pINode );
					pItem->m_wFlagsUI &= ~RF_PENDING_LOAD;
					hr = S_FALSE;
				}
			}
		}

		if( pINode )
		{
			pINode->Release();
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		// Let our hosting editor know about the changes
		if( hr == S_OK )
		{
			OnDataChanged();
		}

		if( m_pTimeline )
		{
			m_pTimeline->StripInvalidateRect( m_pScriptStrip, NULL, TRUE );
		}

		// Update the property page
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}

		SyncWithDirectMusic();

		return hr;
	}

	// We don't handle whichever notification was passed to us
	return CBaseMgr::OnUpdate( rguidType, dwGroupBits, pData );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CTrackMgr::GetStripMgrProperty(
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
			return S_OK;
	}

	// Let CBaseMgr handle
	return CBaseMgr::GetStripMgrProperty( stripMgrProperty, pVariant );
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
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TempoParam, m_dwOldGroupBits );
				m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwOldGroupBits );

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
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TempoParam, m_dwGroupBits );
					m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwGroupBits );
				}
			}
			return S_OK;

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

				// Ignore everything in the structure, since we don't care about it
				// Our track group is always 1-32
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

	// Remove all existing items
	EmptyList( m_lstTrackItems );

	// Make sure legacy tracks belong to all track groups
	m_dwGroupBits = 0xFFFFFFFF;
	m_dwOldGroupBits = 0xFFFFFFFF;
	
	// Initialize last script name field
	m_strLastScriptName.Empty();

	// Variables used when loading the Script track
	MMCKINFO ckTrack;
	MMCKINFO ckRoutineList;
	MMCKINFO ckItem;
	CTrackItem* pNewItem;

	// Interate through every chunk in the stream
	while( pIRiffStream->Descend( &ckTrack, NULL, 0 ) == 0 )
	{
		switch( ckTrack.ckid )
		{
			case FOURCC_LIST:
				switch( ckTrack.fccType )
				{
					case DMUS_FOURCC_SCRIPTTRACK_LIST:
						while( pIRiffStream->Descend( &ckRoutineList, &ckTrack, 0 ) == 0 )
						{
							switch( ckRoutineList.ckid )
							{
								case FOURCC_LIST:
									switch( ckRoutineList.fccType )
									{
										case DMUS_FOURCC_SCRIPTTRACKEVENTS_LIST:
											while( pIRiffStream->Descend( &ckItem, &ckRoutineList, 0 ) == 0 )
											{
												switch( ckItem.ckid )
												{
													case FOURCC_LIST:
														switch( ckItem.fccType )
														{
															case DMUS_FOURCC_SCRIPTTRACKEVENT_LIST:
																hr = LoadTrackItem( pIRiffStream, &ckItem, &pNewItem );
																if( FAILED ( hr ) )
																{
																	goto ON_ERROR;
																}
																InsertByAscendingTime( pNewItem, FALSE );
																break;
														}
														break;
												}

												pIRiffStream->Ascend( &ckItem, 0 );
											}
											break;
									}
									break;
							}

							pIRiffStream->Ascend( &ckRoutineList, 0 );
						}
						break;
				}
				break;
		}

		pIRiffStream->Ascend( &ckTrack, 0 );
	}

	SyncWithDirectMusic();

ON_ERROR:
	m_strLastScriptName.Empty();
	pIRiffStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SaveDMRef

HRESULT CTrackMgr::SaveDMRef( IDMUSProdRIFFStream* pIRiffStream,
							   IDMUSProdNode* pIDocRootNode, WhichLoader whichLoader )
{
	IDMUSProdLoaderRefChunk* pIRefChunkLoader;

	ASSERT( m_pDMProdFramework != NULL );
	if( m_pDMProdFramework == NULL )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( SUCCEEDED( m_pDMProdFramework->QueryInterface( IID_IDMUSProdLoaderRefChunk, (void**)&pIRefChunkLoader ) ) )
	{
		if( pIRefChunkLoader )
		{
			switch( whichLoader )
			{
				case WL_DIRECTMUSIC:
				case WL_PRODUCER:
					hr = pIRefChunkLoader->SaveRefChunkForLoader( pIStream,
																  pIDocRootNode,
																  CLSID_DirectMusicScript,
																  NULL,
																  whichLoader );
					break;
			}

			pIRefChunkLoader->Release();
		}
	}

	pIStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SaveProducerRef

HRESULT CTrackMgr::SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode )
{
	IDMUSProdFileRefChunk* pIFileRefChunk;

	ASSERT( m_pDMProdFramework != NULL );
	if( m_pDMProdFramework == NULL )
	{
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if ( SUCCEEDED ( m_pDMProdFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**)&pIFileRefChunk ) ) )
	{
		pIFileRefChunk->SaveRefChunk( pIStream, pIDocRootNode );
		pIFileRefChunk->Release();
	}

	pIStream->Release();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SaveRefPendingLoad

HRESULT CTrackMgr::SaveRefPendingLoad( IDMUSProdRIFFStream* pIRiffStream, CTrackItem* pItem )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
	MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwLength;

	// Should only call this method when asked to save
	// while waiting for FRAMEWORK_FileLoadFinished notification
	ASSERT( pItem->m_FileRef.pIDocRootNode == NULL );
	ASSERT( pItem->m_wFlagsUI & RF_PENDING_LOAD );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pIPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pIPersistInfo ) ) )
	{
		pIPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pIPersistInfo );
	}

	if( StreamInfo.ftFileType == FT_RUNTIME )
	{
		// This method cannot save FT_RUNTIME format because it cannot embed files
		// OK to return without doing anything 
		hr = S_FALSE;
		goto ON_ERROR;
	}

	// Initialize the DMUS_OBJECTDESC structure
	DMUS_OBJECTDESC dmusObjectDesc;
	memset( &dmusObjectDesc, 0, sizeof(DMUS_OBJECTDESC) );
	dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);
	
	// Prepare the DMUS_OBJECTDESC structure
	{
		dmusObjectDesc.dwValidData = ( DMUS_OBJ_CLASS | DMUS_OBJ_NAME );
		dmusObjectDesc.guidClass = CLSID_DirectMusicScript;
		MultiByteToWideChar( CP_ACP, 0, pItem->m_FileRef.li.strName, -1, dmusObjectDesc.wszName, DMUS_MAX_NAME );
	}

	// Write DMUS_FOURCC_REF_LIST
	{
		// Write REF LIST header
		ckMain.fccType = DMUS_FOURCC_REF_LIST;
		if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write REF chunk
		{
			DMUS_IO_REFERENCE dmusReference;

			ck.ckid = DMUS_FOURCC_REF_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Prepare DMUS_IO_REFERENCE structure
			memset( &dmusReference, 0, sizeof(DMUS_IO_REFERENCE) );
			memcpy( &dmusReference.guidClassID, &dmusObjectDesc.guidClass, sizeof(GUID) );
			dmusReference.dwValidData = dmusObjectDesc.dwValidData;

			// Write REF chunk data 
			hr = pIStream->Write( &dmusReference, sizeof(DMUS_IO_REFERENCE), &dwBytesWritten);
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DMUS_IO_REFERENCE) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		// Write Name chunk
		{
			ck.ckid = DMUS_FOURCC_NAME_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			dwLength = wcslen( dmusObjectDesc.wszName ) + 1; 
			dwLength *= sizeof( wchar_t );
			hr = pIStream->Write( &dmusObjectDesc.wszName, dwLength, &dwBytesWritten);
			if( FAILED( hr )
			||  dwBytesWritten != dwLength )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::LoadTrackItem

HRESULT CTrackMgr::LoadTrackItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent, CTrackItem** ppItem )
{
	MMCKINFO		ck;
	DWORD			dwByteCount;
	DWORD			dwSize;
	DWORD			dwCurrentFilePos;
	IDMUSProdNode*	pIDocRootNode = NULL;
	CString			strScriptName;
	HRESULT			hr;
    HRESULT         hrReference = S_OK;

	if( ppItem == NULL )
	{
		return E_POINTER;
	}
	*ppItem = NULL;

	CTrackItem* pNewItem = new CTrackItem( this );
	if( pNewItem == NULL )
	{
		return E_OUTOFMEMORY;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	dwCurrentFilePos = StreamTell( pIStream );

	// Load the routine item
	while( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_SCRIPTTRACKEVENTHEADER_CHUNK:
			{
				DMUS_IO_SCRIPTTRACK_EVENTHEADER iEventHeader;

				// Read in the routine item's header structure
				dwSize = min( sizeof( DMUS_IO_SCRIPTTRACK_EVENTHEADER ), ck.cksize );
				hr = pIStream->Read( &iEventHeader, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				pNewItem->m_dwFlagsDM = iEventHeader.dwFlags;
				if( IsRefTimeTrack() )
				{
					pNewItem->m_rtTimePhysical = iEventHeader.lTimePhysical * REFCLOCKS_PER_MILLISECOND;
					pNewItem->m_rtTimeLogical = iEventHeader.lTimeLogical * REFCLOCKS_PER_MILLISECOND;
				}
				else
				{
					pNewItem->m_rtTimePhysical = iEventHeader.lTimePhysical;
					pNewItem->m_rtTimeLogical = iEventHeader.lTimeLogical;
				}
				pNewItem->SetTimePhysical( pNewItem->m_rtTimePhysical, STP_LOGICAL_RECALC_MEASURE_BEAT );
				break;
			}

			case FOURCC_DMUSPROD_FILEREF:
			{
				IDMUSProdFileRefChunk* pIFileRef;

				hr = m_pDMProdFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**) &pIFileRef );
				if( FAILED ( hr ) )
				{
					goto ON_ERROR;
				}
				StreamSeek( pIStream, dwCurrentFilePos, 0 );
				hrReference = pIFileRef->LoadRefChunk( pIStream, &pIDocRootNode );
				if( hrReference == E_PENDING )
				{
					ASSERT( m_pIDocRootNode != NULL );
					if( m_pIDocRootNode )
					{
						IDMUSProdNotifySink* pINotifySink;
						if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
						{
							// Store File's GUID so we can resolve reference in our handler 
							// for the FRAMEWORK_FileLoadFinished notification
							StreamSeek( pIStream, dwCurrentFilePos, 0 );
							if( SUCCEEDED ( pIFileRef->ResolveWhenLoadFinished( pIStream, pINotifySink, &pNewItem->m_FileRef.li.guidFile ) ) )
							{
								pNewItem->m_wFlagsUI |= RF_PENDING_LOAD;
							}
							RELEASE( pINotifySink );
						}
					}
				}
				pIFileRef->Release();
				break;
			}

			case FOURCC_LIST:
				if( ck.fccType == DMUS_FOURCC_REF_LIST )
				{
					MMCKINFO ckName;

					ckName.ckid = DMUS_FOURCC_NAME_CHUNK;
					if( pIRiffStream->Descend( &ckName, NULL, MMIO_FINDCHUNK ) == 0 )
					{
						// Store Script name
						ReadMBSfromWCS( pIStream, ckName.cksize, &strScriptName );
					}
				}
				break;

			case DMUS_FOURCC_SCRIPTTRACKEVENTNAME_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &pNewItem->m_strRoutine );
				break;
		}

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ck, 0 );
		dwCurrentFilePos = StreamTell( pIStream );
	}

	if( pIDocRootNode == NULL
	&&  hrReference != E_PENDING )
	{
		// Do we have a Script name?
		if( !strScriptName.IsEmpty() )
		{
			// Framework could not resolve Script file reference
			// so we will ask user to help
			hrReference = FindScript( pNewItem, strScriptName, pIStream, &pIDocRootNode );
			if( FAILED ( hrReference ) )
			{
				pIDocRootNode = NULL;
			}
		}
	}

	if( pIDocRootNode )
	{
		hr = pNewItem->SetFileReference( pIDocRootNode );
		pIDocRootNode->Release();

		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

ON_ERROR:
	RELEASE( pIStream );

	if( SUCCEEDED ( hr ) )
	{
		*ppItem = pNewItem;
	}
	else
	{
		delete pNewItem;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SaveTrackItem

HRESULT CTrackMgr::SaveTrackItem( IDMUSProdRIFFStream* pIRiffStream, CTrackItem* pItem )
{
	MMCKINFO ckItem;
	MMCKINFO ck;
	HRESULT hr;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

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

	// Create the DMUS_FOURCC_SCRIPTTRACKEVENT_LIST list chunk
	ckItem.fccType = DMUS_FOURCC_SCRIPTTRACKEVENT_LIST;
	if( pIRiffStream->CreateChunk( &ckItem, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_SCRIPTTRACKEVENTHEADER_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_SCRIPTTRACKEVENTHEADER_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_SCRIPTTRACK_EVENTHEADER oEventHeader;
		ZeroMemory( &oEventHeader, sizeof(DMUS_IO_SCRIPTTRACK_EVENTHEADER) );

		// Fill in the members of the DMUS_IO_SCRIPTTRACK_EVENTHEADER structure
		oEventHeader.dwFlags = pItem->m_dwFlagsDM;		
		if( IsRefTimeTrack() )
		{
			oEventHeader.lTimePhysical = (long)(pItem->m_rtTimePhysical / REFCLOCKS_PER_MILLISECOND);
			oEventHeader.lTimeLogical = (long)(pItem->m_rtTimeLogical / REFCLOCKS_PER_MILLISECOND);
		}
		else
		{
			oEventHeader.lTimePhysical = (long)pItem->m_rtTimePhysical;
			oEventHeader.lTimeLogical = (long)pItem->m_rtTimeLogical;
		}

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oEventHeader, sizeof(DMUS_IO_SCRIPTTRACK_EVENTHEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_SCRIPTTRACK_EVENTHEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_SCRIPTTRACKEVENTHEADER_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Create the Script file reference chunk(s)
	{
		if( pItem->m_FileRef.pIDocRootNode )
		{
			if( ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
			{
				SaveDMRef( pIRiffStream, pItem->m_FileRef.pIDocRootNode, WL_PRODUCER );
			}
			else if( ::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) )
			{
				SaveDMRef( pIRiffStream, pItem->m_FileRef.pIDocRootNode, WL_DIRECTMUSIC );
				if( ftFileType == FT_DESIGN )
				{
					SaveProducerRef( pIRiffStream, pItem->m_FileRef.pIDocRootNode );
				}
			}
		}
		else
		{
			if( pItem->m_wFlagsUI & RF_PENDING_LOAD )
			{
				// Will happen while waiting for FRAMEWORK_FileLoadFinished notification
				hr = SaveRefPendingLoad( pIRiffStream, pItem );
				if( FAILED ( hr ) )
				{
					goto ON_ERROR;
				}
			}
		}
	}

	// Create the DMUS_FOURCC_SCRIPTTRACKEVENTNAME_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_SCRIPTTRACKEVENTNAME_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write out the Routine text as a Wide Character String
		hr = SaveMBStoWCS( pIStream, &pItem->m_strRoutine );
		if( FAILED( hr ) )
		{
			// Handle I/O errors by return an error code
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_SCRIPTTRACKEVENTNAME_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Ascend out of the DMUS_FOURCC_SCRIPTTRACKEVENT_LIST list chunk
	if( pIRiffStream->Ascend(&ckItem, 0) != 0 )
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
	// stream that will be loaded into a DirectMusic Script track (GUID_DirectMusicObject)
	if( !::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) &&
		!::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
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

	// If the Script list isn't empty, save it
	if ( !m_lstTrackItems.IsEmpty() )
	{
		// Create a LIST chunk to store the Script track data
		MMCKINFO ckTrack;
		ckTrack.fccType = DMUS_FOURCC_SCRIPTTRACK_LIST;
		if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Create a LIST chunk to store the list of items 
		MMCKINFO ckRoutineList;
		ckRoutineList.fccType = DMUS_FOURCC_SCRIPTTRACKEVENTS_LIST;
		if( pIRiffStream->CreateChunk( &ckRoutineList, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Iterate through the Routine list
		POSITION pos = m_lstTrackItems.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each item
			CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

			// Save each item
			hr = SaveTrackItem( pIRiffStream, pItem );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
		
		// Ascend out of the Routine LIST chunk.
		pIRiffStream->Ascend( &ckRoutineList, 0 );

		// Ascend out of the Script track LIST chunk.
		pIRiffStream->Ascend( &ckTrack, 0 );
	}

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

// This method is called by CScriptPropPageMgr to get data to send to the
// Script property page.
// The CScriptStrip::GetData() method is called by CPropPageMgrTrack
// to get the strip's properties (currently just Group Bits)
HRESULT STDMETHODCALLTYPE CTrackMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate the ppData pointer
	if ( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	// Flag set to TRUE if more than one item is selected
	BOOL fMultipleSelect = FALSE;

	// Initialize the pointer to the first selected item to NULL
	CTrackItem* pFirstSelectedTrackItem = NULL;

	// Start iterating through the item list
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem;
		pItem = m_lstTrackItems.GetNext( pos );

		// Check if the item is selected
		if ( pItem->m_fSelected )
		{
			// Routine is selected - save a pointer to it in pFirstSelectedTrackItem
			pFirstSelectedTrackItem = pItem;

			// Now, continue through the list to see if any other items are selected
			while( pos )
			{
				// Get a pointer to each item
				pItem = m_lstTrackItems.GetNext( pos );

				// Check if the item is selected
				if ( pItem->m_fSelected )
				{
					// More than one item is selected - set fMultipleSelect to TRUE
					// and break out of this loop
					fMultipleSelect = TRUE;
					break;
				}
			}

			// Found a selected item - break out of the main while loop
			break;
		}
	}

	// If at least one item is selected
	if( pFirstSelectedTrackItem )
	{
		// Copy the first selected item to a CTrackItem class
		m_SelectedTrackItem.Copy( pFirstSelectedTrackItem );

		// If more than one item was selected, set UD_MULTIPLESELECT
		if( fMultipleSelect )
		{
			m_SelectedTrackItem.m_dwBitsUI |= UD_MULTIPLESELECT;
		}
		else
		{
			// Only one item selected, clear UD_MULTIPLESELECT
			m_SelectedTrackItem.m_dwBitsUI &= ~UD_MULTIPLESELECT;
		}

		// Set the passed in pointer to point to the CTrackItem class
		*ppData = &m_SelectedTrackItem;

		// Return a success code
		return S_OK;
	}

	// Nothing selected, so clear the passed in pointer
	*ppData = NULL;

	// Return a success code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SetData

// This method is called by CScriptPropPageMgr in response to user actions
// in the Script Property page.  It changes the currenly selected Script. 
HRESULT STDMETHODCALLTYPE CTrackMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate the pData pointer
	if ( pData == NULL )
	{
		return E_POINTER;
	}

	// Get a pointer to the first selected item.
	CTrackItem* pItem = FirstSelectedItem();

	// If a selected item was found
	if( pItem )
	{
		// Convert the passed-in pointer to a CTrackItem*
		CTrackItem* pNewItem = static_cast<CTrackItem*>(pData);

		// m_rtTimePhysical
		if( pNewItem->m_rtTimePhysical != pItem->m_rtTimePhysical )
		{
			// Update the item's time
			m_nLastEdit = IDS_UNDO_MOVE;
			pItem->SetTimePhysical( pNewItem->m_rtTimePhysical, STP_LOGICAL_ADJUST );

			// Remove the TrackItem from the list
			CTrackItem* pItemTmp;
			POSITION pos2, pos1 = m_lstTrackItems.GetHeadPosition();
			while( pos1 )
			{
				pos2 = pos1;
				pItemTmp = m_lstTrackItems.GetNext( pos1 );
				if ( pItemTmp == pItem )
				{
					m_lstTrackItems.RemoveAt( pos2 );
					break;
				}
			}

			// Re-add the item at its new position - this will overwrite any existing
			// item at this position
			InsertByAscendingTime( pItem, FALSE );

			// Clear all selections
			m_pScriptStrip->m_pSelectedRegions->Clear();

			// Select just the changed item
			m_pScriptStrip->CListSelectedRegion_AddRegion( pItem );

		}

		// m_rtTimeLogical
		else if( pNewItem->m_lLogicalMeasure != pItem->m_lLogicalMeasure
			 ||  pNewItem->m_lLogicalBeat != pItem->m_lLogicalBeat )
		{
			ASSERT( IsRefTimeTrack() == false );
			// Following code assumes we are using MUSIC_TIME
			MUSIC_TIME mtNewTimeLogical;
			if( SUCCEEDED ( ForceBoundaries( pNewItem->m_lLogicalMeasure, pNewItem->m_lLogicalBeat, 0, &mtNewTimeLogical ) ) )
			{
				if( mtNewTimeLogical != pItem->m_rtTimeLogical )
				{
					m_nLastEdit = IDS_UNDO_LOGICAL_TIME;
					pItem->SetTimeLogical( mtNewTimeLogical );
				}
			}
		}

		// m_FileRef.pIDocRootNode
		else if( pNewItem->m_FileRef.pIDocRootNode != pItem->m_FileRef.pIDocRootNode )
		{
			// Update the script reference
			pItem->SetFileReference( pNewItem->m_FileRef.pIDocRootNode );

			// We just changed the item
			m_nLastEdit = IDS_UNDO_CHANGE_SCRIPT;
		}

		// m_strRoutine
		else if( pNewItem->m_strRoutine != pItem->m_strRoutine )
		{
			// Update the item's text
			pItem->m_strRoutine = pNewItem->m_strRoutine;

			// We just changed the item
			m_nLastEdit = IDS_UNDO_CHANGE_ROUTINE;
		}

		// m_dwFlagsDM
		else if( pNewItem->m_dwFlagsDM != pItem->m_dwFlagsDM )
		{
			// Update the item's flags
			pItem->m_dwFlagsDM = pNewItem->m_dwFlagsDM;

			// We just changed the item
			m_nLastEdit = IDS_UNDO_CHANGE_TIMING;
		}

		// Nothing changed
		else
		{
			// Return a success code saying that nothing happened
			return S_FALSE;
		}

		// Redraw the Script strip
		m_pTimeline->StripInvalidateRect( m_pScriptStrip, NULL, TRUE );

		// Let our hosting editor know about the changes
		OnDataChanged();

		// Refresh the property page with new values
		ASSERT( m_pPropPageMgr != NULL );	// OnDataChanged() should not set m_pPropPageMgr to NULL!
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}

		// Sync track with DirectMusic
		SyncWithDirectMusic();

		return S_OK;
	}
	// No items selected - nothing to change
	else
	{
		// Return a success code saying that nothing happened
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CTrackMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		// Create a new Script property page manager
		CPropPageMgrItem* pPPM = new CPropPageMgrItem( m_pDMProdFramework, this );

		// Verify that we're not out of memory
		if( pPPM == NULL )
		{
			return E_OUTOFMEMORY;
		}

		// Get the IDMUSProdPropPageManager interface from the property page
		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		
		// Release the reference created by the contructor, leaving the one created by QueryInterface.
		// If QueryInterface failed, this will delete m_pPropPageMgr.
		m_pPropPageMgr->Release();

		// If we failed, return a failure code
		if( FAILED(hr) )
		{
			return hr;
		}
	}

	// Set the displayed property page to our property page
	m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);

	// Return a success code
	return hr;
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

	// Hand DirectMusic new Script track
	HRESULT hr = CBaseMgr::SyncWithDirectMusic();
	if( SUCCEEDED ( hr ) )
	{
		if( SUCCEEDED ( m_pIDMTrack->IsParamSupported(GUID_EnableScriptTrackError) ) )
		{
			BOOL fEnableError = TRUE;
			m_pIDMTrack->SetParam( GUID_EnableScriptTrackError,  0, &fEnableError );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::DeleteSelectedItems

void CTrackMgr::DeleteSelectedItems()
{
	// Start iterating through the list of items
	POSITION pos1 = m_lstTrackItems.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos1 );

		// Check if the current item is selected
		if ( pItem->m_fSelected )
		{
			// This item is selected, remove it from the list
			m_lstTrackItems.RemoveAt( pos2 );

			// Now, delete this item
			delete pItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SaveSelectedItems

HRESULT CTrackMgr::SaveSelectedItems( IStream* pIStream, REFERENCE_TIME rtOffset, CTrackItem* pItemAtDragPoint )
{
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	MUSIC_TIME mtPasteOverwriteRange = -1;
	MMCKINFO ck;
	DWORD	 dwBytesWritten;

	// Verify pIStream is valid
	if( pIStream == NULL )
	{
		ASSERT( 0 );
		return E_POINTER;
	}

	// Create a list to store the items to save in
	CTypedPtrList<CPtrList, CTrackItem*> lstItemsToSave;

	// Iterate throught the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext(pos);

		// Check if the item is selected
		if( pItem->m_fSelected )
		{
			// Add the item to the list of items to save
			CTrackItem* pNewItem = new CTrackItem( this, *pItem );
			if( pNewItem )
			{
				lstItemsToSave.AddTail( pNewItem );
			}
		}
	}

	// Get item for DMUS_FOURCC_COPYPASTE_UI_CHUNK
	CTrackItem* pCopyPasteItem = pItemAtDragPoint;
	if( pCopyPasteItem == NULL )
	{
		if( lstItemsToSave.IsEmpty() == FALSE )
		{
			pCopyPasteItem = lstItemsToSave.GetHead();
		}
	}

	// Try and allocate a RIFF stream
	HRESULT hr = AllocRIFFStream( pIStream, &pIRiffStream );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	// Create DMUS_FOURCC_COPYPASTE_UI_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_COPYPASTE_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// If range selected via timeline, use it
		// TODO!!!   Put back
//		if( m_bGutterSelected && (m_lGutterBeginSelect != m_lGutterEndSelect) )
		{
			long lStartTime;
			long lEndTime;
			m_pScriptStrip->m_pSelectedRegions->GetSpan( lStartTime, lEndTime );

			// Compute the start offset
			MUSIC_TIME mtStartOffset;
			long lMeasure;
			long lBeat;
			m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lStartTime, &lMeasure, &lBeat );
			MeasureBeatTickToClocks( lMeasure, lBeat, 0, &mtStartOffset );

			// Compute the end offset
			MUSIC_TIME mtEndOffset;
			m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lEndTime, &lMeasure, &lBeat );
			MeasureBeatTickToClocks( lMeasure, lBeat, 0, &mtEndOffset );

			// Compute the paste overwrite range
			mtPasteOverwriteRange = mtEndOffset - mtStartOffset;
		}

		// Clear out the structure (clears out the padding bytes as well).
		ioCopyPasteUI oCopyPasteUI;
		ZeroMemory( &oCopyPasteUI, sizeof(ioCopyPasteUI) );

		// Fill in the members of the ioCopyPasteUI structure
		if( m_pScriptStrip->m_bGutterSelected
		&& (m_pScriptStrip->m_lGutterBeginSelect != m_pScriptStrip->m_lGutterEndSelect) )
		{
			// Use start of timeline selection
			long lStartTime, lEndTime;
			m_pScriptStrip->m_pSelectedRegions->GetSpan( lStartTime, lEndTime );

			REFERENCE_TIME rtStartTime;
			ClocksToUnknownTime( lStartTime, &rtStartTime );

			long lMinute, lMeasure;
			UnknownTimeToRefTime( rtStartTime, &oCopyPasteUI.m_rtStartTime );
			RefTimeToMinSecMs( oCopyPasteUI.m_rtStartTime,
							   &lMinute, &oCopyPasteUI.m_lRefTimeSec, &oCopyPasteUI.m_lRefTimeMs );

			UnknownTimeToClocks( rtStartTime, &oCopyPasteUI.m_mtStartTime );
			ClocksToMeasureBeatTick( oCopyPasteUI.m_mtStartTime,
									 &lMeasure, &oCopyPasteUI.m_lMusicTimeBeat, &oCopyPasteUI.m_lMusicTimeTick );
		}
		else if( pCopyPasteItem )
		{
			// Use time of first selected item
			long lMinute, lMeasure;
			UnknownTimeToRefTime( pCopyPasteItem->m_rtTimePhysical, &oCopyPasteUI.m_rtStartTime );
			RefTimeToMinSecMs( oCopyPasteUI.m_rtStartTime,
							   &lMinute, &oCopyPasteUI.m_lRefTimeSec, &oCopyPasteUI.m_lRefTimeMs );

			UnknownTimeToClocks( pCopyPasteItem->m_rtTimePhysical, &oCopyPasteUI.m_mtStartTime );
			ClocksToMeasureBeatTick( oCopyPasteUI.m_mtStartTime,
									 &lMeasure, &oCopyPasteUI.m_lMusicTimeBeat, &oCopyPasteUI.m_lMusicTimeTick );
		}
		m_pTimeline->ClocksToRefTime( mtPasteOverwriteRange, &oCopyPasteUI.m_rtPasteOverwriteRange );
		oCopyPasteUI.m_mtPasteOverwriteRange = mtPasteOverwriteRange;
		oCopyPasteUI.m_fRefTimeTrack = IsRefTimeTrack();

		// Write the structure out to the stream
		hr = pIStream->Write( &oCopyPasteUI, sizeof(ioCopyPasteUI), &dwBytesWritten );
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(ioCopyPasteUI) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_COPYPASTE_UI_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	//check that anything is selected
	if( lstItemsToSave.IsEmpty() )
	{
		hr = S_FALSE;
		goto ON_ERROR;
	}

	// For each item in lstItemsToSave, subtract rtOffset from m_rtTimePhysical
	REFERENCE_TIME rtNormalizeOffset;
	if( m_pScriptStrip->m_bGutterSelected
	&& (m_pScriptStrip->m_lGutterBeginSelect != m_pScriptStrip->m_lGutterEndSelect) )
	{
		// Use start of timeline selection
		long lStartTime, lEndTime;
		m_pScriptStrip->m_pSelectedRegions->GetSpan( lStartTime, lEndTime );

		ClocksToUnknownTime( lStartTime, &rtNormalizeOffset );
	}
	else if( pCopyPasteItem )
	{
		// Use time of first selected item
		rtNormalizeOffset = pCopyPasteItem->m_rtTimePhysical;
	}
	else
	{
		ASSERT( 0 );
		rtNormalizeOffset = 0;
	}
	NormalizeList( this, lstItemsToSave, rtNormalizeOffset );

	// Save the list of items into pIStream
	hr = SaveList( lstItemsToSave, this, pIStream );

	// Empty the temporary list of items
	EmptyList( lstItemsToSave );

ON_ERROR:
	RELEASE( pIRiffStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::MarkSelectedItems

// ORs dwFlags with the m_dwBits of each selected item
void CTrackMgr::MarkSelectedItems( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// Check if the item is selected
		if ( pItem->m_fSelected )
		{
			// It's selected - update m_dwBits
			pItem->m_dwBitsUI |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::DeleteMarked

// deletes items marked by given flag
void CTrackMgr::DeleteMarked( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos1 = m_lstTrackItems.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos1 );

		// Check if any of the specified dwFlags are set in this item
		if ( pItem->m_dwBitsUI & dwFlags )
		{
			// At least one of the flags in dwFlags is set, remove the item
			m_lstTrackItems.RemoveAt( pos2 );

			// Now, delete it
			delete pItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::UnMarkItems

// unmarks flag m_dwUndermined field CTrackItems in list
void CTrackMgr::UnMarkItems( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Clear the selected flags in each item
		m_lstTrackItems.GetNext( pos )->m_dwBitsUI &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::UnselectAll

void CTrackMgr::UnselectAll()
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Clear the selection flag for each item
		m_lstTrackItems.GetNext( pos )->m_fSelected = FALSE;
	}

	// Clear the list of selected regions
	m_pScriptStrip->m_pSelectedRegions->Clear();
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SelectAll

void CTrackMgr::SelectAll()
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Set the selection flag for each item
		m_lstTrackItems.GetNext( pos )->m_fSelected = TRUE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::FirstSelectedItem

CTrackItem* CTrackMgr::FirstSelectedItem()
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to this lyic
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// Check if the item is selected
		if ( pItem->m_fSelected )
		{
			// Routine is selected, return a pointer to it
			return pItem;
		}
	}

	// No items are selected, return NULL
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::InsertByAscendingTime

void CTrackMgr::InsertByAscendingTime( CTrackItem *pItemToInsert, BOOL fPaste )
{
	// Ensure the pItemToInsert pointer is valid
	if ( pItemToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	CTrackItem* pItem;
	POSITION posCurrent, posNext = m_lstTrackItems.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pItem = m_lstTrackItems.GetNext( posNext );

		if( fPaste )
		{
			if( pItem->m_rtTimePhysical == pItemToInsert->m_rtTimePhysical )
			{
				// Replace item
				m_lstTrackItems.InsertBefore( posCurrent, pItemToInsert );
				m_lstTrackItems.RemoveAt( posCurrent );
				delete pItem;
				return;
			}
		}

		if( pItem->m_rtTimePhysical > pItemToInsert->m_rtTimePhysical )
		{
			// insert before posCurrent (which is the position of pItem)
			m_lstTrackItems.InsertBefore( posCurrent, pItemToInsert );
			return;
		}
	}

	// pItemToInsert is later than all items in the list, add it at the end of the list
	m_lstTrackItems.AddTail( pItemToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::RemoveItem

bool CTrackMgr::RemoveItem( CTrackItem* pItem )
{
	// Find the given item
	POSITION posToRemove = m_lstTrackItems.Find( pItem, NULL );

	// If item wasn't found, return false
	if( posToRemove == NULL )
	{
		return false;
	}

	// Remove the item from the list - the caller must delete it
	m_lstTrackItems.RemoveAt( posToRemove );

	// Return true since we found the item
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::RecomputeMeasureBeats

void CTrackMgr::RecomputeMeasureBeats()
{
	CTrackItem* pItem;

	// Recompute measure/beat (or minutes/seconds) of all items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		pItem->SetTimePhysical( pItem->m_rtTimePhysical, STP_LOGICAL_ADJUST );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetNextGreatestUniqueTime

MUSIC_TIME CTrackMgr::GetNextGreatestUniqueTime( long lMeasure, long lBeat, long lTick )
{
	DMUS_TIMESIGNATURE dmTimeSig;
	MUSIC_TIME mtTime;

	// This method assumes we are a MUSIC_TIME track
	ASSERT( IsRefTimeTrack() == false );

	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		if( pItem->m_lMeasure > lMeasure )
		{
			break;
		}

		MeasureBeatTickToClocks( pItem->m_lMeasure, 0, 0, &mtTime );
		m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, mtTime, NULL, &dmTimeSig );
		if( pItem->m_lBeat > dmTimeSig.bBeatsPerMeasure )
		{
			break;
		}

		if( pItem->m_lMeasure == lMeasure
		&&  pItem->m_lBeat == lBeat )
		{
			lTick = pItem->m_lTick + 1;

			// Take care of measure/beat rollover
			MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );  
			ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick );  
		}
	}

	MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );  
	return mtTime;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::RecomputeTimes

bool CTrackMgr::RecomputeTimes()
{
	// This method assumes we are a MUSIC_TIME track
	if( IsRefTimeTrack() )
	{
		// Nothing to do
		return false;
	}

	MUSIC_TIME mtTime;
	long lMeasure;
	long lBeat;
	long lTick;

	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Save our current position
		POSITION pos2 = pos;

		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// Make sure measure and beat are valid
		if( pItem->m_lMeasure >= 0
		&&  pItem->m_lBeat >= 0 )
		{
			// Using the item's current measure, beat, tick settings, determine which measure, beat and tick
			// the item will end up on
			MeasureBeatTickToClocks( pItem->m_lMeasure, pItem->m_lBeat, pItem->m_lTick, &mtTime );  
			ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick ); 

			// Check if either the measure or beat were changed
			if( pItem->m_rtTimePhysical != mtTime
			||	pItem->m_lMeasure != lMeasure
			||	pItem->m_lBeat != lBeat
			||	pItem->m_lTick != lTick )
			{
				// Remove the itme from the list
				m_lstTrackItems.RemoveAt( pos2 );

				// The measure of the Routine changed
				if( pItem->m_lMeasure != lMeasure )
				{
					// This would happen when moving from 7/4 to 4/4, for example
					// items on beat 7 would end up on next measure's beat 3
					while( pItem->m_lMeasure != lMeasure )
					{
						// Keep moving back a beat until the measure does not change
						MeasureBeatTickToClocks( lMeasure, --lBeat, lTick, &mtTime );  
						ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick ); 
					}
				}

				mtTime = GetNextGreatestUniqueTime( lMeasure, lBeat, lTick ); 
				pItem->SetTimePhysical( mtTime, STP_LOGICAL_ADJUST );
				fChanged = TRUE;

				InsertByAscendingTime( pItem, FALSE );
			}
		}
	}

	// Return whether or not anything changed
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::DeleteBetweenMeasureBeats

bool CTrackMgr::DeleteBetweenMeasureBeats(long lmStart, long lbStart, long lmEnd, long lbEnd )
{
	// Initially, nothing changed
	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while(pos)
	{
		// Save current position
		POSITION posTemp = pos;

		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext(pos);

		// Check if the item's measure value is before the start measure
		if( pItem->m_lMeasure < lmStart )
		{
			// Too early - keep looking
			continue;
		}

		// Check if the item's measure value is after the end measure
		if( pItem->m_lMeasure > lmEnd )
		{
			// Too late - done looking (since the list is sorted)
			break;
		}

		// If the item is in the start measure, check if it is before the start beat
		if( ( pItem->m_lMeasure == lmStart )
		&&	( pItem->m_lBeat < lbStart ) )
		{
			// Too early - keep looking
			continue;
		}

		// If the item is in the end measure, check if it is after the end beat
		if( ( pItem->m_lMeasure == lmEnd)
		&&	( pItem->m_lBeat >= lbEnd ) )
		{
			// Too late - done looking (since the list is sorted)
			break;
		}

		// Within the given range, delete the item
		m_lstTrackItems.RemoveAt(posTemp);
		delete pItem;
		fChanged = true;
	}

	// Return whether or not anything changed
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::CallSelectedRoutines

HRESULT CTrackMgr::CallSelectedRoutines( void  )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	CString strNone;
	strNone.LoadString( IDS_NONE_TEXT );

	HRESULT hr = S_OK;

	// Start iterating through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// Check if the current item is selected
		if( pItem->m_fSelected )
		{
			if( pItem->m_FileRef.pIDocRootNode
			&&  pItem->m_strRoutine.IsEmpty() == FALSE
			&&  pItem->m_strRoutine.Compare( strNone) != 0 )
			{
				IDirectMusicScript* pIDMScript;
				if( SUCCEEDED ( pItem->m_FileRef.pIDocRootNode->GetObject( CLSID_DirectMusicScript, IID_IDirectMusicScript, (void**)&pIDMScript ) ) )
				{
					WCHAR awchRoutineName[MAX_PATH];

					// Convert the routine name to wide characters
					MultiByteToWideChar( CP_ACP, 0, pItem->m_strRoutine, -1, awchRoutineName, MAX_PATH );

					// Call the routine
					DMUS_SCRIPT_ERRORINFO ErrorInfo;
					memset( &ErrorInfo, 0, sizeof(DMUS_SCRIPT_ERRORINFO) );
					ErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);

					hr = pIDMScript->CallRoutine( awchRoutineName, &ErrorInfo );
					pIDMScript->Release();

					if( FAILED ( hr ) )
					{
						if( m_pDMProdFramework )
						{
							IDMUSProdComponent* pIComponent;
							if( SUCCEEDED ( m_pDMProdFramework->FindComponent( CLSID_ScriptComponent,  &pIComponent ) ) )
							{
								IDMUSProdDebugScript* pIDebugScript;
								if( SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdDebugScript, (void**)&pIDebugScript ) ) )
								{
									pIDebugScript->DisplayScriptError( &ErrorInfo );

									pIDebugScript->Release();
								}
							
								pIComponent->Release();
							}
						}
						break;
					}
				}
			}
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::CycleItems

HRESULT CTrackMgr::CycleItems( long lXPos  )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	long lMeasure;
	long lBeat;
	HRESULT hr = SnapPositionToUnknownIncrements( lXPos, &lMeasure, &lBeat );

	if( SUCCEEDED ( hr ) ) 
	{
		CTrackItem* pFirstItem = NULL;
		CTrackItem* pSecondItem = NULL;
		CTrackItem* pItem;

		hr = E_FAIL;

		POSITION pos = m_lstTrackItems.GetHeadPosition();
		while( pos )
		{
			pItem = m_lstTrackItems.GetNext( pos );

			if( pItem->m_lMeasure == lMeasure
			&&  pItem->m_lBeat == lBeat )
			{
				if( pFirstItem == NULL )
				{
					pFirstItem = pItem;
				}
				else if( pSecondItem == NULL )
				{
					pSecondItem = pItem;
				}

				if( pItem->m_wFlagsUI & RF_TOP_ITEM )
				{
					if( pos )
					{
						// Cycle to next Routine if on same measure/beat (or minute/second)
						CTrackItem* pNextItem = m_lstTrackItems.GetNext( pos );

						if( pNextItem->m_lMeasure == lMeasure
						&&  pNextItem->m_lBeat == lBeat )
						{
							UnselectAll();
							pItem->m_wFlagsUI &= ~RF_TOP_ITEM;
							pNextItem->m_fSelected = TRUE;
							pNextItem->m_wFlagsUI |= RF_TOP_ITEM;
							hr = S_OK;
							break;
						}
					}

					// Cycle to first Routine on same measure/beat
					UnselectAll();
					pItem->m_wFlagsUI &= ~RF_TOP_ITEM;
					pFirstItem->m_fSelected = TRUE;
					pFirstItem->m_wFlagsUI |= RF_TOP_ITEM;
					hr = S_OK;
					break;
				}
			}

			if( pItem->m_lMeasure > lMeasure
			||  pos == NULL )
			{
				UnselectAll();
				if( pSecondItem )
				{
					pSecondItem->m_fSelected = TRUE;
					pSecondItem->m_wFlagsUI |= RF_TOP_ITEM;
					hr = S_OK;
				}
				else if( pFirstItem )
				{
					pFirstItem->m_fSelected = TRUE;
					pFirstItem->m_wFlagsUI |= RF_TOP_ITEM;
					hr = S_OK;
				}
				break;
			}
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		// Update the selection regions to include just this selected item
		m_pScriptStrip->SelectRegionsFromSelectedItems();

		// Redraw the Script strip
		m_pTimeline->StripInvalidateRect( m_pScriptStrip, NULL, TRUE );

		// Update the property page
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
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


/////////////////////////////////////////////////////////////////////////////
// NormalizeList

void NormalizeList( CTrackMgr* pTrackMgr, 
				    CTypedPtrList<CPtrList, CTrackItem*>& list, REFERENCE_TIME rtOffset  )
{
	long lMeasure, lBeat, lTick;

	DWORD dwGroupBits = pTrackMgr->GetGroupBits();

	// Iterate through the list of items
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		CTrackItem* pItem = list.GetNext(pos);

		// This method only called for drag/drop and cut/copy/paste
		// so it is safe to mess with the values that are stored in time fields

		// Snap physical time to number of beats
		long lPhysicalBeats;
		MUSIC_TIME mtTimePhysical;
		pTrackMgr->UnknownTimeToClocks( pItem->m_rtTimePhysical, &mtTimePhysical );
		pTrackMgr->ClocksToMeasureBeatTick( mtTimePhysical, &lMeasure, &lBeat, &lTick );
		MeasureBeatToBeats( pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

		// Convert logical time to number of beats
		if( pTrackMgr->IsRefTimeTrack() )
		{
			// Use m_rtTimeLogical to store beat difference between physical time and logical time
			ASSERT( pItem->m_rtTimePhysical == pItem->m_rtTimeLogical );
			pItem->m_rtTimeLogical = 0;
		}
		else
		{
			long lLogicalBeats;
			pTrackMgr->ClocksToMeasureBeatTick( (MUSIC_TIME)pItem->m_rtTimeLogical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lLogicalBeats );

			// Use m_rtTimeLogical to store beat difference between physical time and logical time
			pItem->m_rtTimeLogical = lLogicalBeats - lPhysicalBeats;
		}

		// Use m_rtTimePhysical to store an offset
		pItem->m_rtTimePhysical -= rtOffset;
	}
}


/////////////////////////////////////////////////////////////////////////////
// SaveList

HRESULT SaveList( CTypedPtrList<CPtrList, CTrackItem*>& list, CTrackMgr* pTrackMgr, IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that the stream pointer is non-null
	if( pTrackMgr == NULL
	||  pIStream == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
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
	// stream that will be loaded into a DirectMusic Script track (GUID_DirectMusicObject)
	if( !::IsEqualGUID( guidDataFormat, GUID_CurrentVersion )
	&&	!::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
	{
		ASSERT( 0 );
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

	// If the Script list isn't empty, save it
	if( !list.IsEmpty() )
	{
		// Create a LIST chunk to store the Script track data
		MMCKINFO ckTrack;
		ckTrack.fccType = DMUS_FOURCC_SCRIPTTRACK_LIST;
		if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Create a LIST chunk to store the list of items 
		MMCKINFO ckRoutineList;
		ckRoutineList.fccType = DMUS_FOURCC_SCRIPTTRACKEVENTS_LIST;
		if( pIRiffStream->CreateChunk( &ckRoutineList, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Iterate through the Routine list
		POSITION pos = list.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each item
			CTrackItem* pItem = list.GetNext( pos );

			// Save each item
			hr = SaveListItem( pIRiffStream, pTrackMgr, pItem );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
		
		// Ascend out of the Routine LIST chunk.
		pIRiffStream->Ascend( &ckRoutineList, 0 );

		// Ascend out of the Script track LIST chunk.
		pIRiffStream->Ascend( &ckTrack, 0 );
	}

ON_ERROR:
	// Release our pointer to the IDMUSProdRIFFStream
	pIRiffStream->Release();

	// Return the success/failure code
    return hr;
}

HRESULT LoadList( CTypedPtrList<CPtrList, CTrackItem*>& list, IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr, IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Must have pTrackMgr pointer!
	ASSERT( pTrackMgr != NULL );
	if( pTrackMgr == NULL )
	{
		return E_FAIL;
	}
	
	// Verify that the stream pointer is non-null
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	// Try and allocate a RIFF stream
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = AllocRIFFStream( pIStream, &pIRiffStream );
	if( FAILED ( hr ) )
	{
		return hr;
	}

	// Initialize last script name field
	pTrackMgr->m_strLastScriptName.Empty();

	// Variables used when loading the Script track
	MMCKINFO ckTrack;
	MMCKINFO ckRoutineList;
	MMCKINFO ckItem;
	CTrackItem* pNewItem;

	// Descend into the Routine LIST chunk
	ckTrack.fccType = DMUS_FOURCC_SCRIPTTRACK_LIST;
	if( pIRiffStream->Descend(&ckTrack, NULL, MMIO_FINDLIST) == 0)
	{
		// Now, descend into each chunk in this LIST chunk
		while( pIRiffStream->Descend( &ckRoutineList, &ckTrack, 0 ) == 0 )
		{
			switch( ckRoutineList.ckid )
			{
				case FOURCC_LIST:
					switch( ckRoutineList.fccType )
					{
						case DMUS_FOURCC_SCRIPTTRACKEVENTS_LIST:
							while( pIRiffStream->Descend( &ckItem, &ckRoutineList, 0 ) == 0 )
							{
								switch( ckItem.ckid )
								{
									case FOURCC_LIST:
										switch( ckItem.fccType )
										{
											case DMUS_FOURCC_SCRIPTTRACKEVENT_LIST:
												hr = LoadListItem( pIRiffStream, &ckItem, pIFramework, pTrackMgr, &pNewItem );
												if( FAILED ( hr ) )
												{
													goto ON_ERROR;
												}
												list.AddTail( pNewItem );
												break;
										}
										break;
								}

								pIRiffStream->Ascend( &ckItem, 0 );
							}
							break;
					}
					break;
			}

			pIRiffStream->Ascend( &ckRoutineList, 0 );
		}

		pIRiffStream->Ascend( &ckTrack, 0 );
	}

ON_ERROR:
	pTrackMgr->m_strLastScriptName.Empty();
	pIRiffStream->Release();
	return hr;
}

void EmptyList( CTypedPtrList<CPtrList, CTrackItem*>& list )
{
	// Remove and delete all the CTrackItems from the given list
	while( !list.IsEmpty() )
	{
		delete list.RemoveHead();
	}
}

HRESULT GetBoundariesOfItems( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, long &lStartTime, long &lEndTime, CTypedPtrList<CPtrList, CTrackItem*>& list)
{
	// Validate the given pTimeline pointer
	if( pTimeline == NULL )
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	// Initialize the start and end times to -1
	lStartTime = -1;
	lEndTime = -1;

	// No items in list, return S_FALSE since there's nothing to do
	if( list.IsEmpty() )
	{
		return S_FALSE;
	}

	// Initialize our return value to S_OK
	HRESULT hr = S_OK;
	MUSIC_TIME mtTime;

	// Iterate through the list of items
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		CTrackItem* pItem = list.GetNext(pos);

		// Try and convert the measure and beat of the item to a value in number of clocks
		hr = pTimeline->MeasureBeatToClocks( dwGroupBits, 0, pItem->m_lMeasure, pItem->m_lBeat, &mtTime );

		// If the conversion failed, break out of the while loop
		if(FAILED(hr))
		{
			break;
		}

		// If the start time is not yet set, or the item's time is earlier
		// than any other item, update lStartTime.
		if( ( lStartTime == -1 )
		||	( mtTime < lStartTime ) )
		{
			lStartTime = mtTime;
		}

		// If the end time is not yet set, or the item's time is later
		// than any other item, update lEnd.
		if( ( lEndTime == -1 )
		||	( mtTime > lEndTime ) )
		{
			lEndTime = mtTime;
		}
	}

	// If the conversions (MeasureBeatToClocks) succeeded
	if( SUCCEEDED(hr) )
	{
		// The start and end times should be set to valid values
		ASSERT( lStartTime != -1 );
		ASSERT( lEndTime != -1 );

		// Get the measure and beat of the last item
		long lMeasure, lBeat;
		CMusicTimeConverter cmt = lEndTime;
		hr = cmt.GetMeasureBeat( lMeasure, lBeat, pTimeline, dwGroupBits );

		// If the conversion failed, return the failure code
		if( FAILED( hr ) )
		{
			return hr;
		}

		// Increment the beat value
		lBeat++;

		// Convert from measure and beat back to clocks
		hr = cmt.SetTime( lMeasure, lBeat, pTimeline, dwGroupBits );

		// If the conversion failed, return the failure code
		if( FAILED( hr ) )
		{
			return hr;
		}

		// Set the end time to one tick before the next beat
		lEndTime = cmt - 1;
	}

	// Return the success or failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// LoadListItem

HRESULT LoadListItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent,
					  IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr, CTrackItem** ppItem )
{
	MMCKINFO		ck;
	DWORD			dwByteCount;
	DWORD			dwSize;
	DWORD			dwCurrentFilePos;
	IDMUSProdNode*	pIDocRootNode = NULL;
	CString			strScriptName;
	HRESULT			hr;

	if( pIFramework == NULL 
	||  pTrackMgr == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	if( ppItem == NULL )
	{
		return E_POINTER;
	}
	*ppItem = NULL;

	CTrackItem* pNewItem = new CTrackItem( pTrackMgr );
	if( pNewItem == NULL )
	{
		return E_OUTOFMEMORY;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	dwCurrentFilePos = StreamTell( pIStream );

	// Load the routine item
	while( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_SCRIPTTRACKEVENTHEADER_CHUNK:
			{
				DMUS_IO_SCRIPTTRACK_EVENTHEADER iEventHeader;

				// Read in the routine item's header structure
				dwSize = min( sizeof( DMUS_IO_SCRIPTTRACK_EVENTHEADER ), ck.cksize );
				hr = pIStream->Read( &iEventHeader, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				pNewItem->m_dwFlagsDM = iEventHeader.dwFlags;
				pNewItem->m_rtTimePhysical = iEventHeader.lTimePhysical;	// lTimePhysical stores offset
				pNewItem->m_rtTimeLogical = iEventHeader.lTimeLogical;		// lTimeLogical stores beat difference
																			// between physical and logical times
				break;
			}

			case FOURCC_DMUSPROD_FILEREF:
			{
				IDMUSProdFileRefChunk* pIFileRef;

				hr = pIFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**) &pIFileRef );
				if( FAILED ( hr ) )
				{
					goto ON_ERROR;
				}
				StreamSeek( pIStream, dwCurrentFilePos, 0 );
				pIFileRef->LoadRefChunk( pIStream, &pIDocRootNode );
				pIFileRef->Release();
				break;
			}

			case FOURCC_LIST:
				if( ck.fccType == DMUS_FOURCC_REF_LIST )
				{
					MMCKINFO ckName;

					ckName.ckid = DMUS_FOURCC_NAME_CHUNK;
					if( pIRiffStream->Descend( &ckName, NULL, MMIO_FINDCHUNK ) == 0 )
					{
						// Store Script name
						ReadMBSfromWCS( pIStream, ckName.cksize, &strScriptName );
					}
				}
				break;

			case DMUS_FOURCC_SCRIPTTRACKEVENTNAME_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &pNewItem->m_strRoutine );
				break;
		}

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ck, 0 );
		dwCurrentFilePos = StreamTell( pIStream );
	}

	if( pIDocRootNode == NULL )
	{
		// Do we have a Script name?
		if( !strScriptName.IsEmpty() )
		{
			// Framework could not resolve Script file reference
			// so we will ask user to help
			hr = pTrackMgr->FindScript( pNewItem, strScriptName, pIStream, &pIDocRootNode );
			if( FAILED ( hr ) )
			{
				// Make sure E_PENDING becomes E_FAIL
				hr = E_FAIL;
				pIDocRootNode = NULL;
			}
		}
	}

	if( pIDocRootNode )
	{
		hr = pNewItem->SetFileReference( pIDocRootNode );
		pIDocRootNode->Release();

		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

ON_ERROR:
	RELEASE( pIStream );

	if( SUCCEEDED ( hr ) )
	{
		*ppItem = pNewItem;
	}
	else
	{
		delete pNewItem;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// SaveListItem

HRESULT SaveListItem( IDMUSProdRIFFStream* pIRiffStream, CTrackMgr* pTrackMgr, CTrackItem* pItem )
{
	MMCKINFO ckItem;
	MMCKINFO ck;
	HRESULT hr;

	if( pTrackMgr == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

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

	// Create the DMUS_FOURCC_SCRIPTTRACKEVENT_LIST list chunk
	ckItem.fccType = DMUS_FOURCC_SCRIPTTRACKEVENT_LIST;
	if( pIRiffStream->CreateChunk( &ckItem, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_SCRIPTTRACKEVENTHEADER_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_SCRIPTTRACKEVENTHEADER_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_SCRIPTTRACK_EVENTHEADER oEventHeader;
		ZeroMemory( &oEventHeader, sizeof(DMUS_IO_SCRIPTTRACK_EVENTHEADER) );

		// Fill in the members of the DMUS_IO_SCRIPTTRACK_EVENTHEADER structure
		oEventHeader.dwFlags = pItem->m_dwFlagsDM;		
		oEventHeader.lTimeLogical = (long)pItem->m_rtTimeLogical;		// m_rtTimeLogical stores beat difference
																// between physical and logical times
		if( pTrackMgr->IsRefTimeTrack() )
		{
			oEventHeader.lTimePhysical = (long)(pItem->m_rtTimePhysical / REFCLOCKS_PER_MILLISECOND);
		}
		else
		{
			oEventHeader.lTimePhysical = (long)pItem->m_rtTimePhysical;
		}

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oEventHeader, sizeof(DMUS_IO_SCRIPTTRACK_EVENTHEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_SCRIPTTRACK_EVENTHEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_SCRIPTTRACKEVENTHEADER_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Create the Script file reference chunk(s)
	{
		if( pItem->m_FileRef.pIDocRootNode )
		{
			if( ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
			{
				pTrackMgr->SaveDMRef( pIRiffStream, pItem->m_FileRef.pIDocRootNode, WL_PRODUCER );
			}
			else if( ::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) )
			{
				pTrackMgr->SaveDMRef( pIRiffStream, pItem->m_FileRef.pIDocRootNode, WL_DIRECTMUSIC );
				if( ftFileType == FT_DESIGN )
				{
					pTrackMgr->SaveProducerRef( pIRiffStream, pItem->m_FileRef.pIDocRootNode );
				}
			}
		}
	}

	// Create the DMUS_FOURCC_SCRIPTTRACKEVENTNAME_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_SCRIPTTRACKEVENTNAME_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write out the Routine text as a Wide Character String
		hr = SaveMBStoWCS( pIStream, &pItem->m_strRoutine );
		if( FAILED( hr ) )
		{
			// Handle I/O errors by return an error code
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_SCRIPTTRACKEVENTNAME_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Ascend out of the DMUS_FOURCC_SCRIPTTRACKEVENT_LIST list chunk
	if( pIRiffStream->Ascend(&ckItem, 0) != 0 )
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
// CTrackMgr::RefTimeToMinSecMs

HRESULT	CTrackMgr::RefTimeToMinSecMs( REFERENCE_TIME rtTime,
									  long* plMinute, long* plSecond, long* plMillisecond )
{
	ASSERT( plMinute != NULL );
	ASSERT( plSecond != NULL );
	ASSERT( plMillisecond != NULL );

	*plMinute = (long)(rtTime / REFCLOCKS_PER_MINUTE);
	rtTime = rtTime % REFCLOCKS_PER_MINUTE;

	*plSecond = (long)(rtTime / REFCLOCKS_PER_SECOND);
	rtTime = rtTime % REFCLOCKS_PER_SECOND;

	*plMillisecond = (long)(rtTime / REFCLOCKS_PER_MILLISECOND);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::MinSecMsToRefTime

HRESULT	CTrackMgr::MinSecMsToRefTime( long lMinute, long lSecond, long lMillisecond,
									  REFERENCE_TIME* prtTime )
{
	ASSERT( prtTime != NULL );

	*prtTime =  lMinute * REFCLOCKS_PER_MINUTE;
	*prtTime += lSecond * REFCLOCKS_PER_SECOND;
	*prtTime += lMillisecond * REFCLOCKS_PER_MILLISECOND;

	return S_OK;
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


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::UnknownTimeToMeasureBeatTick

HRESULT	CTrackMgr::UnknownTimeToMeasureBeatTick( REFERENCE_TIME rtTime, long* plMeasure, long* plBeat, long* plTick )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}
	
	MUSIC_TIME mtTime;
	UnknownTimeToClocks( rtTime, &mtTime );

	return ClocksToMeasureBeatTick( mtTime, plMeasure, plBeat, plTick );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::MeasureBeatTickToUnknownTime

HRESULT	CTrackMgr::MeasureBeatTickToUnknownTime( long lMeasure, long lBeat, long lTick, REFERENCE_TIME* prtTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}
	
	MUSIC_TIME mtTime;
	HRESULT hr = MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );

	ClocksToUnknownTime( mtTime, prtTime );

	return hr; 
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SnapUnknownTime

HRESULT	CTrackMgr::SnapUnknownTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	if( IsRefTimeTrack() )
	{
		long lMinute, lSecond, lMs;
		RefTimeToMinSecMs( rtTime, &lMinute, &lSecond, &lMs );
		MinSecMsToRefTime( lMinute, lSecond, 0, prtTime );
	}
	else
	{
		long lMeasure, lBeat;
		MUSIC_TIME mtTime;
		m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, (MUSIC_TIME)rtTime, &lMeasure, &lBeat );
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &mtTime );
		*prtTime = mtTime;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SnapUnknownTimeToDisplayPosition

HRESULT	CTrackMgr::SnapUnknownTimeToDisplayPosition( REFERENCE_TIME rtTime, long* plPosition )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	if( IsRefTimeTrack() )
	{
		long lMinute, lSecond, lMs;
		RefTimeToMinSecMs( rtTime, &lMinute, &lSecond, &lMs );
		MinSecMsToRefTime( lMinute, lSecond, 0, &rtTime );
	}
	else
	{
		MUSIC_TIME mtTime;
		long lMeasure, lBeat, lTick;
		ClocksToMeasureBeatTick( (MUSIC_TIME)rtTime, &lMeasure, &lBeat, &lTick );
		MeasureBeatTickToClocks( lMeasure, lBeat, 0, &mtTime );
		m_pTimeline->ClocksToRefTime( mtTime, &rtTime );
	}

	m_pTimeline->RefTimeToPosition( rtTime, plPosition );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SnapUnknownTimeToDisplayClocks

HRESULT	CTrackMgr::SnapUnknownTimeToDisplayClocks( REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	if( IsRefTimeTrack() )
	{
		long lMinute, lSecond, lMs;
		RefTimeToMinSecMs( rtTime, &lMinute, &lSecond, &lMs );
		MinSecMsToRefTime( lMinute, lSecond, 0, &rtTime );
		m_pTimeline->RefTimeToClocks( rtTime, pmtTime );
	}
	else
	{
		long lMeasure, lBeat, lTick;
		ClocksToMeasureBeatTick( (MUSIC_TIME)rtTime, &lMeasure, &lBeat, &lTick );
		MeasureBeatTickToClocks( lMeasure, lBeat, 0, pmtTime );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SnapUnknownTimeToUnknownIncrements

HRESULT	CTrackMgr::SnapUnknownTimeToUnknownIncrements( REFERENCE_TIME rtTime, long* plMeasure, long* plBeat )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	if( IsRefTimeTrack() )
	{
		long lMs;
		RefTimeToMinSecMs( rtTime, plMeasure, plBeat, &lMs );
	}
	else
	{
		m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, (MUSIC_TIME)rtTime, plMeasure, plBeat );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SnapPositionToUnknownTime

HRESULT CTrackMgr::SnapPositionToUnknownTime( long lPosition, REFERENCE_TIME* prtTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}
	
	if( IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;
		long lMinute, lSecond, lMs;

		m_pTimeline->PositionToRefTime( lPosition, &rtTime );
		RefTimeToMinSecMs( rtTime, &lMinute, &lSecond, &lMs );
		MinSecMsToRefTime( lMinute, lSecond, 0, prtTime );
	}
	else
	{
		MUSIC_TIME mtTime;
		long lMeasure, lBeat;
		m_pTimeline->PositionToClocks( lPosition, &mtTime );
		m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &mtTime );
		*prtTime = mtTime;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SnapPositionToUnknownIncrements

HRESULT	CTrackMgr::SnapPositionToUnknownIncrements( long lPosition, long* plMeasure, long* plBeat )
{
	*plMeasure = -1;
	*plBeat= -1;

	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	if( IsRefTimeTrack() )
	{
		long lMs;
		REFERENCE_TIME rtTime;

		m_pTimeline->PositionToRefTime( lPosition, &rtTime );
		RefTimeToMinSecMs( rtTime, plMeasure, plBeat, &lMs );
	}
	else
	{
		long lTick;
		MUSIC_TIME mtTime;

		m_pTimeline->PositionToClocks( lPosition, &mtTime );
		ClocksToMeasureBeatTick( mtTime, plMeasure, plBeat, &lTick );
		if( lTick < 0 )
		{
			MeasureBeatTickToClocks( *plMeasure, (*plBeat - 1), 0, &mtTime );
			ClocksToMeasureBeatTick( mtTime, plMeasure, plBeat, &lTick );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::SnapPositionToStartEndClocks

HRESULT CTrackMgr::SnapPositionToStartEndClocks( long lXPos, MUSIC_TIME* pmtStart, MUSIC_TIME* pmtEnd )
{
	ASSERT( m_pTimeline != NULL );

	long lMeasure, lBeat;
	SnapPositionToUnknownIncrements( lXPos, &lMeasure, &lBeat ); 

	// Determine start and end times
	if( IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;

		// Determine start time
		MinSecMsToRefTime( lMeasure, lBeat, 0, &rtTime );
		m_pTimeline->RefTimeToClocks( rtTime, pmtStart );

		// Determine end time
		MinSecMsToRefTime( lMeasure, (lBeat + 1), 0, &rtTime );
		m_pTimeline->RefTimeToClocks( rtTime, pmtEnd );
	}
	else
	{
		// Determine start time
		MeasureBeatTickToClocks( lMeasure, lBeat, 0, pmtStart );

		// Determine end time
		MeasureBeatTickToClocks( lMeasure, (lBeat + 1), 0, pmtEnd );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::ClocksToUnknownTime

HRESULT CTrackMgr::ClocksToUnknownTime( MUSIC_TIME mtTime, REFERENCE_TIME* prtTime )
{
	if( m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
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
		return E_FAIL;
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
		return E_FAIL;
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
		return E_FAIL;
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
// CTrackMgr::FindScript

HRESULT CTrackMgr::FindScript( CTrackItem* pItem, CString strScriptName, IStream* pIStream, IDMUSProdNode** ppIDocRootNode )
{
	IDMUSProdNode*		pIDocRootNode = NULL;
	IDMUSProdNode*		pITargetDirectoryNode = NULL;
	IDMUSProdDocType*	pIDocType = NULL;
	HRESULT				hr;

	ASSERT( m_pDMProdFramework != NULL );
	ASSERT( pIStream != NULL );

	// Get DocType for DLS Collections
	hr = m_pDMProdFramework->FindDocTypeByNodeId( GUID_ScriptNode, &pIDocType );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Get the target directory
	DMUSProdStreamInfo	StreamInfo;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pITargetDirectoryNode = StreamInfo.pITargetDirectoryNode;
		pPersistInfo->Release();
	}

	// If a target directory is not associated with the stream
	// use the strip's DocRoot node
	if( pITargetDirectoryNode == NULL )
	{
		if( m_pIDocRootNode )
		{
			IDMUSProdNode* pIDocNode;
			if( SUCCEEDED ( m_pIDocRootNode->GetDocRootNode( &pIDocNode ) ) )
			{
				pITargetDirectoryNode = pIDocNode;
				RELEASE( pIDocNode );
			}
		}
	}

	hr = E_FAIL;

	// See if there is a Script named 'strScriptName' in this Project
	if( !strScriptName.IsEmpty() )
	{
		BSTR bstrScriptName = strScriptName.AllocSysString();
		hr = m_pDMProdFramework->GetBestGuessDocRootNode( pIDocType, bstrScriptName, pITargetDirectoryNode, &pIDocRootNode );
		if( FAILED ( hr ) )
		{
			pIDocRootNode = NULL;

			if( hr == E_PENDING )
			{
				// File is in process of being loaded
				// Store file GUID so we can resolve reference in our handler 
				// for the FRAMEWORK_FileLoadFinished notification
				ASSERT( m_pIDocRootNode != NULL );
				if( m_pIDocRootNode )
				{
					IDMUSProdNotifySink* pINotifySink;
					if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
					{
						IDMUSProdFramework8* pIFramework8;
						if( SUCCEEDED ( m_pDMProdFramework->QueryInterface( IID_IDMUSProdFramework8, (void**)&pIFramework8 ) ) )
						{
							HRESULT hrResolve;

							bstrScriptName = strScriptName.AllocSysString();
							hrResolve = pIFramework8->ResolveBestGuessWhenLoadFinished( pIDocType,
																						bstrScriptName,
																						pITargetDirectoryNode,
																						pINotifySink,
																						&pItem->m_FileRef.li.guidFile );
							if( SUCCEEDED ( hrResolve ) )
							{
								pItem->m_wFlagsUI |= RF_PENDING_LOAD;
								pItem->m_FileRef.li.strName = strScriptName;
							}

							RELEASE( pIFramework8 );
						}

						RELEASE( pINotifySink );
					}
				}
				goto ON_ERROR;
			}
		}
	}

	if( pIDocRootNode == NULL )
	{
		// Cannot find the Script
		// If user cancelled previous search for this Script, no need to ask again
		if( strScriptName.CompareNoCase( m_strLastScriptName ) == 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR; 
		}
		m_strLastScriptName = strScriptName;

		// Determine File Open dialog prompt
		CString	strOpenDlgTitle;
		if( strScriptName.IsEmpty() )
		{
			strOpenDlgTitle.LoadString( IDS_FILE_OPEN_ANY_SCRIPT );
		}
		else
		{
			AfxFormatString1( strOpenDlgTitle, IDS_FILE_OPEN_SCRIPT, strScriptName );
		}
		BSTR bstrOpenDlgTitle = strOpenDlgTitle.AllocSysString();

		// Display File open dialog
		hr = m_pDMProdFramework->OpenFile( pIDocType, bstrOpenDlgTitle, pITargetDirectoryNode, &pIDocRootNode );
		if( hr != S_OK )
		{
			// Did not open a file, or opened file other than Script file
			// so we do not want this DocRoot
			if( pIDocRootNode )
			{
				pIDocRootNode->Release();
				pIDocRootNode = NULL;
			}
		}
	}

	if( pIDocRootNode )
	{
		// Set dirty flag so that GUID_Segment_AllTracksAdded notification will call OnUpdate()
		m_fDirty = true;
	}

ON_ERROR:
	if( pIDocType )
	{
		pIDocType->Release();
	}

	*ppIDocRootNode = pIDocRootNode;
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::ForceBoundaries

HRESULT CTrackMgr::ForceBoundaries( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmtTime )
{
	// Get length of segment
	MUSIC_TIME mtMaxTimelineLength;
	VARIANT var;
	m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	mtMaxTimelineLength = V_I4( &var );

	// Get maximum number of measures, beats
	long lMaxMeasure;
	long lMaxBeat;
	long lMaxTick;
	long lClocks;
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtMaxTimelineLength - 1, &lMaxMeasure, &lMaxBeat );
	lMaxMeasure = max( 0, lMaxMeasure );
	lMaxBeat = max( 0, lMaxBeat );
	MeasureBeatTickToClocks( lMaxMeasure, lMaxBeat, 0, &lClocks );
	lMaxTick = mtMaxTimelineLength - lClocks; 

	// Force boundaries 
	MUSIC_TIME mtTime;
	MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );
	ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick );


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

	// Enforce tick boundary (must catch incomplete last beats - happens when importing MIDI files)
	if( lMeasure == lMaxMeasure 
	&&  lBeat == lMaxBeat 
	&&  lTick > lMaxTick )
	{
		lTick = lMaxTick;
	}
	if( lTick < -MAX_TICK )
	{
		lTick = -MAX_TICK;
	}

	// Make sure mtTime matches adjusted measure, tick
	MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );
	*pmtTime = mtTime;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::GetGroupBits

DWORD CTrackMgr::GetGroupBits( void )
{
	return m_dwGroupBits;
}
