// PersRefMgr.cpp : implementation file
//

/*--------------
@doc PERS_REFSAMPLE
--------------*/

#include "stdafx.h"
#include "PersRefIO.h"
#include "ChordMapRefStripMgr.h"
#include "PersRefMgr.h"
#include "DLLJazzDataObject.h"
#include "PropPageMgr.h"
#include <dmusici.h>
#include <dmusicf.h>
#include <RiffStrm.h>
#include <mmreg.h>
#include <ChordMapDesigner.h>
#include <SegmentDesigner.h>
#include <initguid.h>
#include "SegmentGuids.h"
#include "SegmentIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// GetFirstValidPersRef 

POSITION GetFirstValidPersRef( POSITION pos, const CTypedPtrList<CPtrList, CPersRefItem*> &lstPersRefs )
{
	CPersRefItem* pItem;
	POSITION posToReturn;
	while( pos )
	{
		posToReturn = pos;
		pItem = lstPersRefs.GetNext( pos );
		if( pItem->m_pIPersDocRootNode )
		{
			return posToReturn;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr constructor/destructor 

CPersRefMgr::CPersRefMgr()
{
	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pDMProdFramework = NULL;
	m_pPropertyPage = NULL;
	m_pPropPageMgr = NULL;
	m_pCopyDataObject = NULL;
	m_pPersRefStrip = NULL;
	m_pIDMTrack = NULL;
	m_pISegmentNode = NULL;

	// Initially we don't need to be saved
	m_fDirty = FALSE;

	// By default, belong to Group 1
	m_dwGroupBits = 1;

	// Initialize the track flags
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
	m_dwProducerOnlyFlags = 0;

	// Create a strip and add it to the timeline
	m_pPersRefStrip = new CPersRefStrip(this);
	ASSERT( m_pPersRefStrip );
}

CPersRefMgr::~CPersRefMgr()
{
	// Delete all the personality references in m_lstPersRefs
	EmptyPersRefList();

	// Clean up our references
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

	if( m_pPersRefStrip )
	{
		m_pPersRefStrip->Release();
		m_pPersRefStrip = NULL;
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
// CPersRefMgr IPersRefMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::IsMeasureBeatOpen

HRESULT STDMETHODCALLTYPE CPersRefMgr::IsMeasureBeatOpen( DWORD dwMeasure, BYTE bBeat )
{
	HRESULT hr = S_FALSE;
	POSITION pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );
		if( pPersRefItem->m_dwMeasure > dwMeasure )
		{
			pos = NULL;
		}
		else if( pPersRefItem->m_dwMeasure == dwMeasure )
		{
			hr = S_OK;
			pos = NULL;
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr IDMUSProdStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::GetParam

HRESULT STDMETHODCALLTYPE CPersRefMgr::GetParam(
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

	if( ::IsEqualGUID(guidType, GUID_PersonalityNode) )
	{
		*(IDMUSProdNode**) pData = NULL;

		// Initialize pmtNext
		if( pmtNext )
		{
			*pmtNext = 0;
		}

		// Search through PersRef list for first valid ChordMap
		POSITION pos = m_lstPersRefs.GetHeadPosition();
		pos = GetFirstValidPersRef( pos, m_lstPersRefs );

		if( !pos )
		{
			// No valid PersRefs in this track.
			return DMUS_E_NOT_FOUND;
		}

		// Initialize Previous PersRef Item
		CPersRefItem* pPrevious = m_lstPersRefs.GetAt( pos );

		// Search through PersRef list for valid PersRefs
		MUSIC_TIME mtSRTime = 0;
		while( pos )
		{
			CPersRefItem* pItem = m_lstPersRefs.GetNext( pos );
			ASSERT( pItem->m_pIPersDocRootNode );

			m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, pItem->m_dwMeasure, 0, &mtSRTime );
			if( mtTime < mtSRTime )
			{
				// Set next	
				if( pmtNext )
				{
					m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, pItem->m_dwMeasure, 0, pmtNext );
					*pmtNext -= mtTime;
				}
				break;
			}

			pPrevious = pItem;

			// Get the next valid PersRef
			pos = GetFirstValidPersRef( pos, m_lstPersRefs );
		}

		// We should have found a valid ChordMap
		ASSERT( pPrevious );
		if( ::IsEqualGUID( guidType, GUID_PersonalityNode) )
		{
			IDMUSProdNode *pIPersNode = pPrevious->m_pIPersDocRootNode;
			pIPersNode->AddRef();

			*(IDMUSProdNode**) pData = pIPersNode;
		}
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
		str.LoadString(m_pPersRefStrip->m_nLastEdit);
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

	if( ::IsEqualGUID( guidType, GUID_Segment_ReferencedNodes ) )
	{
		DMUSProdReferencedNodes *pDMUSProdReferencedNodes = (DMUSProdReferencedNodes *)pData;

		// Search through PersRef list for first valid time signature
		POSITION pos = m_lstPersRefs.GetHeadPosition();
		pos = GetFirstValidPersRef( pos, m_lstPersRefs );

		if( !pos )
		{
			// No valid PersRefs in this track.
			pDMUSProdReferencedNodes->dwArraySize = 0;
			return S_OK;
		}

		DWORD dwIndex = 0;

		// Search through PersRef list for valid PersRefs
		while( pos )
		{
			CPersRefItem* pItem = m_lstPersRefs.GetNext( pos );
			ASSERT( pItem->m_pIPersDocRootNode );

			if( pDMUSProdReferencedNodes->apIDMUSProdNode
			&&	pDMUSProdReferencedNodes->dwArraySize > dwIndex )
			{
				pDMUSProdReferencedNodes->apIDMUSProdNode[dwIndex] = pItem->m_pIPersDocRootNode;
				pDMUSProdReferencedNodes->apIDMUSProdNode[dwIndex]->AddRef();
			}

			// Get the next valid PersRef
			pos = GetFirstValidPersRef( pos, m_lstPersRefs );
			dwIndex++;
		}

		HRESULT hr = pDMUSProdReferencedNodes->apIDMUSProdNode && (pDMUSProdReferencedNodes->dwArraySize < dwIndex) ? S_FALSE : S_OK;

		// Store the number of nodes we returned (or that we require)
		pDMUSProdReferencedNodes->dwArraySize = dwIndex;

		return hr;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::SetParam

HRESULT STDMETHODCALLTYPE CPersRefMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pData != NULL );
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
// CPersRefMgr::IsParamSupported

// support GUID_DocRootNode for maintaining pointer to DocRoot node
HRESULT STDMETHODCALLTYPE CPersRefMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) 
	||  ::IsEqualGUID( guidType, GUID_PersonalityNode ) 
	||  ::IsEqualGUID( guidType, GUID_Segment_ReferencedNodes )
	||  ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		return S_OK;
	}
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CPersRefMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPersRefStrip != NULL );

	if( (m_pPersRefStrip == NULL) 
	|| !(dwGroupBits & m_dwGroupBits) )
	{
		return E_FAIL;
	}

	HRESULT hr = E_INVALIDARG;

	// All Tracks Added
	if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		// Make sure host has latest version of data
		// May have changed during load if user prompted to choose ChordMap from File Open dialog
		if( m_fDirty )
		{
			OnDataChanged();
			m_fDirty = FALSE;
		}
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
				// Find which ChordMap changed
				POSITION pos = m_lstPersRefs.GetHeadPosition();
				while( pos )
				{
					CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );

					if( pPersRefItem->m_pIPersDocRootNode == pINode )
					{
						// This ChordMap was removed from the Project Tree
						SetPersReference( NULL, pPersRefItem );

						// Set undo text resource id
						if( ::IsEqualGUID(rguidType, FRAMEWORK_FileDeleted) )
						{
							m_pPersRefStrip->m_nLastEdit = IDS_DELETE;
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
				// Find which ChordMap changed
				POSITION pos = m_lstPersRefs.GetHeadPosition();
				while( pos )
				{
					CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );

					if( pPersRefItem->m_pIPersDocRootNode == pINode )
					{
						// This ChordMap was replaced in the Project Tree, set to new ChordMap pointer
						SetPersReference( (IDMUSProdNode *)pFrameworkMsg->pData, pPersRefItem );

						m_pPersRefStrip->m_nLastEdit = IDS_REPLACE;
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
				// Find which ChordMap changed
				POSITION pos = m_lstPersRefs.GetHeadPosition();
				while( pos )
				{
					CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );

					if( pPersRefItem->m_pIPersDocRootNode == pINode )
					{
						// Set undo text resource id
						m_pPersRefStrip->m_nLastEdit = IDS_CHANGE_LINK;
						hr = S_OK;
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, PERSONALITY_NameChange) ) 
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find which ChordMap changed
				POSITION pos = m_lstPersRefs.GetHeadPosition();
				while( pos )
				{
					CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );

					if( pPersRefItem->m_pIPersDocRootNode == pINode )
					{
						// This ChordMap was renamed
						DMUSProdListInfo ListInfo;
						ZeroMemory( &ListInfo, sizeof(ListInfo) );
						ListInfo.wSize = sizeof(ListInfo);

						if( SUCCEEDED ( pPersRefItem->m_pIPersDocRootNode->GetNodeListInfo ( &ListInfo ) ) )
						{
							IDMUSProdProject* pIProject;

							if( ListInfo.bstrName )
							{
								pPersRefItem->m_PersListInfo.strName = ListInfo.bstrName;
								::SysFreeString( ListInfo.bstrName );
							}
							if( ListInfo.bstrDescriptor )
							{
								pPersRefItem->m_PersListInfo.strDescriptor = ListInfo.bstrDescriptor;
								::SysFreeString( ListInfo.bstrDescriptor );
							}
							m_pDMProdFramework->GetNodeFileGUID ( pPersRefItem->m_pIPersDocRootNode, &pPersRefItem->m_PersListInfo.guidFile );
							if( SUCCEEDED ( m_pDMProdFramework->FindProject( pPersRefItem->m_pIPersDocRootNode, &pIProject ) ) )
							{
								BSTR bstrProjectName;

								pPersRefItem->m_PersListInfo.pIProject = pIProject;
	//							pPersRefItem->m_PersListInfo.pIProject->AddRef();		intentionally missing

								if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
								{
									pPersRefItem->m_PersListInfo.strProjectName = bstrProjectName;
									::SysFreeString( bstrProjectName );
								}

								pIProject->Release();
							}
						}

						// Set undo text resource id
						m_pPersRefStrip->m_nLastEdit = IDS_CHANGE_NAME;
						hr = S_OK;
					}
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
			m_pTimeline->StripInvalidateRect( m_pPersRefStrip, NULL, TRUE );
		}

		// Update the property page
		if( m_pPropPageMgr != NULL )
		{
			m_pPropPageMgr->RefreshData();
		}

		SyncWithDirectMusic();
		m_fDirty = TRUE;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CPersRefMgr::GetStripMgrProperty(
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

			pioTrackHeader->guidClassID = CLSID_DirectMusicChordMapTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = NULL;
			pioTrackHeader->fccType = DMUS_FOURCC_PERS_TRACK_LIST;
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
// CPersRefMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CPersRefMgr::SetStripMgrProperty(
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
			if ( m_pPersRefStrip )
			{
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)m_pPersRefStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pPersRefStrip );
			}
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwGroupBits );
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
				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pPersRefStrip, CLSID_DirectMusicChordMapTrack, m_dwGroupBits, 0 );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwGroupBits);

				// Fix measure/beat info for all current personalities
				long lMeasure;
				long lBeat;
				POSITION pos = m_lstPersRefs.GetHeadPosition();
				while( pos )
				{
					CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );

					m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, pPersRefItem->m_mtTime, &lMeasure, &lBeat );
					pPersRefItem->m_dwMeasure = lMeasure;
					pPersRefItem->m_bBeat = (BYTE) lBeat;
				}
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
// CPersRefMgr IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::GetClassID

HRESULT CPersRefMgr::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pClsId == NULL )
	{
		return E_POINTER;
	}

	// return our CLSID
	memcpy( pClsId, &CLSID_PersRefMgr, sizeof( CLSID ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::IsDirty

HRESULT CPersRefMgr::IsDirty()
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
// CPersRefMgr::Load

HRESULT CPersRefMgr::Load( IStream* pIStream )
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

	// Remove all existing personality references
	EmptyPersRefList();

	m_strLastPersName.Empty();

	// Load the Track
	MMCKINFO ck;
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_PERS_TRACK_LIST: // PersRef List
						BOOL fChanged;
						LoadPersRefList(pIRiffStream, &ck, false, 0, fChanged);
						pIRiffStream->Ascend( &ck, 0 );
						hr = S_OK;
						break;
				}
				break;
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

	m_strLastPersName.Empty();

	pIRiffStream->Release();

	SyncWithDirectMusic();
	return hr;
}


HRESULT CPersRefMgr::LoadPersRefList( IDMUSProdRIFFStream* pIRiffStream, 
										MMCKINFO* pckParent,
										bool fPaste,
										MUSIC_TIME mtTime,
										BOOL &fChanged )
{
	MMCKINFO ck;
	HRESULT hr = S_OK;

	if( pIRiffStream == NULL )
	{
		return E_INVALIDARG;
	}

	while ( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0  )
	{
		if( ck.ckid == FOURCC_LIST
		&&  ck.fccType == DMUS_FOURCC_PERS_REF_LIST )
		{
			if( SUCCEEDED( LoadPersRef(pIRiffStream, &ck, fPaste, mtTime) ) )
			{
				fChanged = TRUE;
			}
			pIRiffStream->Ascend( &ck, 0 );
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

	return hr;
}

HRESULT CPersRefMgr::LoadPersRef( IDMUSProdRIFFStream* pIRiffStream, 
									MMCKINFO* pckParent,
									bool fPaste,
									MUSIC_TIME mtTime )
{
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwCurrentFilePos;
	IDMUSProdNode* pIDocRoot = NULL;
	CString strPersName;

	if( pIRiffStream == NULL
	||  pckParent == NULL )
	{
		return E_INVALIDARG;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	if( pIStream == NULL )
	{
		return E_FAIL;
	}

	CPersRefItem* pItem = new CPersRefItem( this );
	if( pItem == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto ON_END;
	}

	dwCurrentFilePos = StreamTell( pIStream );

	while( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0 )
	{
		switch (ck.ckid)
		{
		case DMUS_FOURCC_TIME_STAMP_CHUNK:
			{
				DWORD dwTime;
				DWORD cb;
				hr = pIStream->Read( &dwTime, sizeof( dwTime ), &cb );
				if (FAILED(hr) || cb != sizeof( dwTime ) ) 
				{
					if (SUCCEEDED(hr)) hr = E_FAIL;
					goto ON_END;
				}

				// Bypass if the time is not zero
				if( dwTime != 0 )
				{
					hr = E_FAIL;
					goto ON_END;
				}

				pItem->m_mtTime = dwTime;
				pItem->m_dwMeasure = 0;
				pItem->m_bBeat = 0;
//				if (m_pTimeline)
//				{
//					m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, dwTime, (long*)&pItem->m_dwMeasure, (long*)&pItem->m_bBeat );
//					if (fPaste)
//					{
//						long lMeasure;
//						long lBeat;
//						m_pTimeline->PositionToMeasureBeat( m_dwGroupBits, 0, nXPos, &lMeasure, &lBeat );
//						m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &pItem->m_mtTime );
//						pItem->m_dwMeasure = (DWORD) lMeasure;
//						pItem->m_bBeat = (BYTE) lBeat;
//					}
//				}

			}
			break;

		case FOURCC_DMUSPROD_FILEREF:
		{
			IDMUSProdFileRefChunk* pIFileRef;

			hr = m_pDMProdFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**) &pIFileRef );
			if( FAILED ( hr ) )
			{
				goto ON_END;
			}
			StreamSeek( pIStream, dwCurrentFilePos, 0 );
			pIFileRef->LoadRefChunk( pIStream, &pIDocRoot );
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
					// Store Personality name
					ReadMBSfromWCS( pIStream, ckName.cksize, &strPersName );
				}
			}
			break;
		}

		pIRiffStream->Ascend( &ck, 0 );
		dwCurrentFilePos = StreamTell( pIStream );
	}

	if( pIDocRoot == NULL )
	{
		// Do we have a Personality name?
		if( !strPersName.IsEmpty() )
		{
			// Framework could not resolve Personality file reference
			// so we will ask user to help
			pIDocRoot = FindPersonality( strPersName, pIStream );
		}
	}

	if( pIDocRoot )
	{
		hr = SetPersReference( pIDocRoot, pItem );
		pIDocRoot->Release();

		if( FAILED ( hr ) )
		{
			goto ON_END;
		}
	}

ON_END:
	if( FAILED (hr) )
	{
		if( pItem )
		{
			delete pItem;
		}
	}
	else
	{
		InsertByAscendingTime( pItem, fPaste );
		if( fPaste )
		{
			pItem->m_fSelected = TRUE;
		}
	}

    pIStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::FindPersonality

IDMUSProdNode* CPersRefMgr::FindPersonality( CString strPersName, IStream* pIStream )
{
	IDMUSProdNode*		pIDocRootNode = NULL;
	IDMUSProdNode*		pITargetDirectoryNode = NULL;
	IDMUSProdDocType*	pIDocType = NULL;
	HRESULT				hr;

	ASSERT( m_pDMProdFramework != NULL );
	ASSERT( pIStream != NULL );

	// Get DocType for DLS Collections
	hr = m_pDMProdFramework->FindDocTypeByNodeId( GUID_PersonalityNode, &pIDocType );
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

	// See if there is a FindPersonality named 'strPersName' in this Project
	if( !strPersName.IsEmpty() )
	{
		BSTR bstrPersName = strPersName.AllocSysString();

		if( FAILED ( m_pDMProdFramework->GetBestGuessDocRootNode( pIDocType,
										 						  bstrPersName,
																  pITargetDirectoryNode,
																  &pIDocRootNode ) ) )
		{
			pIDocRootNode = NULL;
		}
	}

	if( pIDocRootNode == NULL )
	{
		// Cannot find the FindPersonality
		// If user cancelled previous search for this FindPersonality, no need to ask again
		if( strPersName.CompareNoCase( m_strLastPersName ) == 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR; 
		}
		m_strLastPersName = strPersName;

		// Determine File Open dialog prompt
		CString	strOpenDlgTitle;
		if( strPersName.IsEmpty() )
		{
			strOpenDlgTitle.LoadString( IDS_FILE_OPEN_ANY_PERS );
		}
		else
		{
			AfxFormatString1( strOpenDlgTitle, IDS_FILE_OPEN_PERS, strPersName );
		}
		BSTR bstrOpenDlgTitle = strOpenDlgTitle.AllocSysString();

		// Display File open dialog
		if( m_pDMProdFramework->OpenFile(pIDocType, bstrOpenDlgTitle, pITargetDirectoryNode, &pIDocRootNode) != S_OK )
		{
			// Did not open a file, or opened file other than Personality file
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
		m_fDirty = TRUE;
	}

ON_ERROR:
	if( pIDocType )
	{
		pIDocType->Release();
	}

	return pIDocRootNode;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::SaveDMRef

HRESULT CPersRefMgr::SaveDMRef( IDMUSProdRIFFStream* pIRiffStream,
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
																  CLSID_DirectMusicChordMap,
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
// CPersRefMgr::SaveProducerRef

HRESULT CPersRefMgr::SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode )
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
// CPersRefMgr::Save

HRESULT CPersRefMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo	StreamInfo;		// Added JHD 4/27/98
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

	// If the PersRef list isn't empty, save it
	if ( !m_lstPersRefs.IsEmpty() )
	{
		// Create a LIST chunk to store the PersRef data
		ckMain.fccType = DMUS_FOURCC_PERS_TRACK_LIST;
		if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		POSITION pos = m_lstPersRefs.GetHeadPosition();
		while( pos )
		{
			CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );

			// Store each PersRef in its own chunk.
			SavePersReference( pIStream, pIRiffStream, pPersRefItem );
		}
		// Ascend out of the PersRef LIST chunk.
		pIRiffStream->Ascend( &ckMain, 0 );
	}

ON_ERROR:
	pIRiffStream->Release();
    return hr;
}


HRESULT CPersRefMgr::SavePersReference(
			IStream* pIStream, IDMUSProdRIFFStream* pIRiffStream, CPersRefItem* pPersRefItem)
{
	HRESULT hr = E_FAIL;

	ASSERT( pPersRefItem != NULL );

	DMUSProdStreamInfo	StreamInfo;
	FileType ftFileType = FT_DESIGN;	// Default to FT_DESIGN so clipboard
										// gets proper file ref chunk
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pIPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pIPersistInfo ) ) )
	{
		pIPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pIPersistInfo->Release();
	}
	if( !(::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ))
	&&  !(::IsEqualGUID( guidDataFormat, GUID_CurrentVersion )) )
	{
		// Should not happen!
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// Write DMUS_FOURCC_PERS_REF_LIST header
	MMCKINFO ckPersList;
	ckPersList.fccType = DMUS_FOURCC_PERS_REF_LIST;
	if( FAILED( pIRiffStream->CreateChunk( &ckPersList, MMIO_CREATELIST ) ) )
	{
		return E_FAIL;
	}

	// Write 'stmp-ck'
	{
		MMCKINFO ck;
		DWORD dwByteCount;

		ck.ckid = mmioFOURCC('s', 't', 'm', 'p');
		if( FAILED( pIRiffStream->CreateChunk( &ck, 0 ) ) )
		{
			return E_FAIL;
		}

		long lTime = pPersRefItem->m_mtTime;
		DWORD dwTime = lTime;
		hr = pIStream->Write( &dwTime, sizeof(dwTime), &dwByteCount );
		if( FAILED ( hr )
		||  dwByteCount != sizeof(dwTime) )
		{
			return E_FAIL;
		}

		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
			return E_FAIL;
		}
	}

	// Write Reference chunk(s)
	if( pPersRefItem->m_pIPersDocRootNode )
	{
		if( ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
		{
			SaveDMRef( pIRiffStream, pPersRefItem->m_pIPersDocRootNode, WL_PRODUCER );
		}
		else if( ::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) )
		{
			SaveDMRef( pIRiffStream, pPersRefItem->m_pIPersDocRootNode, WL_DIRECTMUSIC );
			if( ftFileType == FT_DESIGN )
			{
				SaveProducerRef( pIRiffStream, pPersRefItem->m_pIPersDocRootNode );
			}
		}
	}
	
	if( pIRiffStream->Ascend( &ckPersList, 0 ) != 0 )
	{
		return E_FAIL;
	}
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::SyncWithDirectMusic

HRESULT CPersRefMgr::SyncWithDirectMusic(  )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;
	CPersRefItem* pPersRefItem = NULL;

	// 1. persist the strip to a stream
	IStream* pIMemStream = NULL;
	IPersistStream* pIPersistStream = NULL;
	hr = m_pDMProdFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );
	if( SUCCEEDED ( hr ) )
	{
		StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
		hr = Save(pIMemStream, FALSE);
		if (SUCCEEDED(hr))
		{
			hr = m_pIDMTrack->QueryInterface(IID_IPersistStream, (void**)&pIPersistStream);
			if (SUCCEEDED(hr))
			{
				StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
				// 2. Load the stream into the track.
				hr = pIPersistStream->Load(pIMemStream);
				pIPersistStream->Release();
			}
		}
		pIMemStream->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::GetSizeMax

HRESULT CPersRefMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	// Compute size of stream needed to persist ourself into.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::GetData

// This method is called by CPersRefPropPageMgr to get data to send to the
// PersRef property page.
// The CPersRefStrip::GetData() method is called by CPersRefStripPropPageMgr
// to get the strip's properties (Group Bits, etc.)
HRESULT STDMETHODCALLTYPE CPersRefMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected Personality reference.
	BOOL fMultipleSelect = FALSE;
	CPersRefItem* pFirstPersRefItem = NULL;

	POSITION pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );
		if( pPersRefItem->m_fSelected )
		{
			pFirstPersRefItem = pPersRefItem;
			while( pos )
			{
				pPersRefItem = m_lstPersRefs.GetNext( pos );
				if( pPersRefItem->m_fSelected )
				{
					fMultipleSelect = TRUE;
					pos = NULL;
					break;
				}
			}
			break;
		}
	}

	if( !fMultipleSelect && pFirstPersRefItem )
	{
		CPropPersRef* pPropPersRef = new CPropPersRef( pFirstPersRefItem );
		if( pPropPersRef )
		{
			*ppData = pPropPersRef;
			return S_OK;
		}
	}

	// Nothing selected or multiple items selected.
	*ppData = NULL;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::SetData

// This method is called by CPersRefPropPageMgr in response to user actions
// in the PersRef Property page.  It changes the currenly selected PersRef. 
HRESULT STDMETHODCALLTYPE CPersRefMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected personality reference.
	CPersRefItem* pPersRefItem;
	pPersRefItem = FirstSelectedPersRef();

	if( pPersRefItem )
	{
		CPropPersRef* pPropPersRef = new CPropPersRef( pPersRefItem );
		if ( pPropPersRef )
		{
			// Only update if the data has changed
			if( memcmp( pData, pPropPersRef, sizeof(CPropPersRef) ) )
			{
				CPropPersRef* pPersRef = (CPropPersRef*)pData;
				HRESULT hr;

				// personality has changed, NB, this catches newly created ref node as doc root will be null
				if( pPersRefItem->m_pIPersDocRootNode == NULL
				||  pPersRefItem->m_pIPersDocRootNode != pPersRef->m_pIPersDocRootNode )
				{
					m_pPersRefStrip->m_nLastEdit = IDS_CHANGE;
					hr = SetPersReference( pPersRef->m_pIPersDocRootNode, pPersRefItem );
				}

				// Update the first selected personality reference
				pPersRef->ApplyToPersRefItem( pPersRefItem );

				// Re-insert the pers reference into the list, in case its measure/beat info 
				// changed and it's now out of order w.r.t. the other elements.
				if( RemoveItem( pPersRefItem ) )
				{
					InsertByAscendingTime( pPersRefItem, FALSE );
				}

				// Redraw the personality reference strip
				// BUGBUG: Should be smarter and only redraw the personality reference that changed
				m_pTimeline->StripInvalidateRect( m_pPersRefStrip, NULL, TRUE );

				// Let our hosting editor know about the changes
				m_pTimeline->OnDataChanged( (IPersRefMgr*)this );
			}

			delete pPropPersRef;
			SyncWithDirectMusic();
			return S_OK;
		}
		else
		{
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		return S_FALSE;
	}
}


HRESULT CPersRefMgr::SetPersReference( IDMUSProdNode* pINewPersDocRootNode, CPersRefItem* pItem )
{
	MUSIC_TIME mtTime = pItem->m_mtTime;
	HRESULT hr;

	// Clean up old DocRoot
	if( pItem->m_pIPersDocRootNode )
	{
		// Turn off notifications for this node
		if( pItem->m_fRemoveNotify )
		{
			if( m_pISegmentNode )
			{
				hr = m_pDMProdFramework->RemoveFromNotifyList( pItem->m_pIPersDocRootNode, m_pISegmentNode );
			}
			pItem->m_fRemoveNotify = FALSE;
		}

		// Initialize DocRoot list info
		pItem->m_PersListInfo.pIProject = NULL;
		pItem->m_PersListInfo.strProjectName = "Empty";
		pItem->m_PersListInfo.strName = "Empty";
		pItem->m_PersListInfo.strDescriptor = "Empty";
		pItem->m_dwBits = 0;
		memset( &pItem->m_guidProject, 0, sizeof( pItem->m_guidProject ) );

		// Release DocRoot
		pItem->m_pIPersDocRootNode->Release();
		pItem->m_pIPersDocRootNode = NULL;
	}

	// Set new DocRoot
	if( pINewPersDocRootNode )
	{
		// Turn on notifications
		ASSERT( pItem->m_fRemoveNotify == FALSE );
		if( m_pISegmentNode )
		{
			hr = m_pDMProdFramework->AddToNotifyList( pINewPersDocRootNode, m_pISegmentNode );
			if( SUCCEEDED ( hr ) )
			{
				pItem->m_fRemoveNotify = TRUE;
			}
		}

		// Update DocRoot member variable
		pItem->m_pIPersDocRootNode = pINewPersDocRootNode;
		pItem->m_pIPersDocRootNode->AddRef();

		// Update DocRoot list info
		DMUSProdListInfo ListInfo;
		ZeroMemory( &ListInfo, sizeof(ListInfo) );
		ListInfo.wSize = sizeof(ListInfo);

		if( SUCCEEDED ( pItem->m_pIPersDocRootNode->GetNodeListInfo ( &ListInfo ) ) )
		{
			IDMUSProdProject* pIProject;

			if( ListInfo.bstrName )
			{
				pItem->m_PersListInfo.strName = ListInfo.bstrName;
				::SysFreeString( ListInfo.bstrName );
			}
			if( ListInfo.bstrDescriptor )
			{
				pItem->m_PersListInfo.strDescriptor = ListInfo.bstrDescriptor;
				::SysFreeString( ListInfo.bstrDescriptor );
			}
			m_pDMProdFramework->GetNodeFileGUID ( pItem->m_pIPersDocRootNode, &pItem->m_PersListInfo.guidFile );
			if( SUCCEEDED ( m_pDMProdFramework->FindProject( pItem->m_pIPersDocRootNode, &pIProject ) ) )
			{
				BSTR bstrProjectName;

				pItem->m_PersListInfo.pIProject = pIProject;
//				pItem->m_PersListInfo.pIProject->AddRef();		intentionally missing

				if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
				{
					pItem->m_PersListInfo.strProjectName = bstrProjectName;
					::SysFreeString( bstrProjectName );
				}

				pIProject->Release();
			}
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CPersRefMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		CPersRefPropPageMgr* pPPM = new CPersRefPropPageMgr(m_pDMProdFramework);
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
// CPersRefMgr::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CPersRefMgr::OnRemoveFromPageManager( void)
{
	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::OnDataChanged

HRESULT STDMETHODCALLTYPE CPersRefMgr::OnDataChanged( void)
{
	if ( m_pTimeline == NULL )
	{
		// Will be NULL if editor is closed and Framework 
		// sends notification that a referenced file has changed
		return E_FAIL;
	}

	// Let our hosting editor know about the change
	m_pTimeline->OnDataChanged( (IPersRefMgr*)this );

	return S_OK;
}


HRESULT CPersRefMgr::GetDirectMusicPersonality( IDMUSProdNode* pIPersDocRoot, IDirectMusicChordMap** ppIPers )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = E_FAIL;

	if( pIPersDocRoot )
	{
		hr = pIPersDocRoot->GetObject(CLSID_DirectMusicChordMap, IID_IDirectMusicChordMap, (void**)ppIPers);
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::DeleteSelectedPersRefs

HRESULT CPersRefMgr::DeleteSelectedPersRefs()
{
	POSITION pos2, pos1 = m_lstPersRefs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos1 );
		if ( pPersRefItem->m_fSelected )
		{
			m_lstPersRefs.RemoveAt( pos2 );
			delete pPersRefItem;
		}
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::SaveSelectedPersRefs

HRESULT CPersRefMgr::SaveSelectedPersRefs(LPSTREAM pIStream, CPersRefItem* pPersRefAtDragPoint, BOOL fNormalize)
{
	// if fNormalize is TRUE and pPersRefAtDragPoint is valid, set mtOffset and dwMeasureOffset so that the time pPersRefAtDragPoint is 0.
	// if fNormalize is TRUE and pPersRefAtDragPoint is NULL, set mtOffset and dwMeasureOffset so that the time of the first PersRef is 0.
	// if fNormalize is FALSE, don't change the times at all.
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// If the PersRef list has anything in it, look for selected PersRefs
	if ( !m_lstPersRefs.IsEmpty() )
	{
		long lMeasureOffset = -1;
		
		POSITION pos;
		if ( fNormalize )
		{
			pos = m_lstPersRefs.GetHeadPosition();
			while( pos )
			{
				CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );
				if ( pPersRefItem->m_fSelected && lMeasureOffset == -1)
				{
					if ( (pPersRefItem == pPersRefAtDragPoint) || (pPersRefAtDragPoint == NULL) )
					{
						lMeasureOffset = pPersRefItem->m_dwMeasure;
					}
				}
			}
			if ( lMeasureOffset == -1 )
			{
				hr = S_FALSE;
				goto ON_ERROR;
			}
		}
		else
		{
			lMeasureOffset = 0;
		}

		MMCKINFO ckMain;

		pos = m_lstPersRefs.GetHeadPosition();
		while( pos )
		{
			CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );

			if ( pPersRefItem->m_fSelected )
			{
				// Store each PersRef in its own chunk.
				SavePersReference(pIStream, pIRiffStream, pPersRefItem);
			}

		}
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
// CPersRefMgr::MarkSelectedPersRefs

// marks m_dwUndermined field CPersRefItems in list
void CPersRefMgr::MarkSelectedPersRefs( DWORD dwFlags )
{
	POSITION pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );
		if ( pPersRefItem->m_fSelected )
		{
			pPersRefItem->m_dwBits |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::DeleteMarked

// deletes personality references marked by given flag
void CPersRefMgr::DeleteMarked( DWORD dwFlags )
{
	POSITION pos2, pos1 = m_lstPersRefs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos1 );
		if ( pPersRefItem->m_dwBits & dwFlags )
		{
			m_lstPersRefs.RemoveAt( pos2 );
			delete pPersRefItem;
		}
	}

	// Update the property page
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::UnMarkPersRefs

// unmarks flag m_dwUndermined field CPersRefItems in list
void CPersRefMgr::UnMarkPersRefs( DWORD dwFlags )
{
	POSITION pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );
		pPersRefItem->m_dwBits &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::UnselectAll

bool CPersRefMgr::UnselectAll()
{
	bool fChange = false;

	POSITION pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );
		if ( pPersRefItem->m_fSelected )
		{
			pPersRefItem->m_fSelected = FALSE;
			fChange = true;
		}
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::SelectAll

void CPersRefMgr::SelectAll()
{
	POSITION pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		m_lstPersRefs.GetNext( pos )->m_fSelected = TRUE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::IsSelected

BOOL CPersRefMgr::IsSelected()
{
	// If anything is selected, return TRUE.
	POSITION pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		if ( m_lstPersRefs.GetNext( pos )->m_fSelected )
		{
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::FirstSelectedPersRef

CPersRefItem* CPersRefMgr::FirstSelectedPersRef()
{
	POSITION pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );
		if ( pPersRefItem->m_fSelected )
		{
			return pPersRefItem;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::SelectSegment

BOOL CPersRefMgr::SelectSegment(long lBeginTime, long lEndTime)
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

	POSITION pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );

		pPersRefItem->m_fSelected = FALSE;
		if( ((DWORD)lBeginMeas < pPersRefItem->m_dwMeasure) && (pPersRefItem->m_dwMeasure < (DWORD)lEndMeas) )
		{
			pPersRefItem->m_fSelected = TRUE;
			result = TRUE;
		}
		else if( (DWORD)lBeginMeas == pPersRefItem->m_dwMeasure )
		{
			pPersRefItem->m_fSelected = TRUE;
			result = TRUE;
		}
		else if( (DWORD)lEndMeas == pPersRefItem->m_dwMeasure )
		{
			pPersRefItem->m_fSelected = TRUE;
			result = TRUE;
		}
	}
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::FixPersRefListMeasureBeat

void CPersRefMgr::FixPersRefListMeasureBeat()
{
	if (m_pTimeline)
	{
		long lMeasure;
		long lBeat;
		POSITION pos = m_lstPersRefs.GetHeadPosition();
		while( pos )
		{
			CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos );
			m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, pPersRefItem->m_mtTime, &lMeasure, &lBeat );
			pPersRefItem->m_dwMeasure = (DWORD) lMeasure;
			pPersRefItem->m_bBeat = (BYTE) lBeat;
		}
	}
	IStream* pIMemStream = NULL;
	HRESULT hr = m_pDMProdFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIMemStream );
	if (SUCCEEDED(hr))
	{
		Save(pIMemStream, FALSE);
		StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
		Load(pIMemStream);
		pIMemStream->Release();
	}
}
	
/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::EmptyPersRefList

void CPersRefMgr::EmptyPersRefList(void)
{
	if( !m_lstPersRefs.IsEmpty() )
	{
		CPersRefItem *pPersRefItem;
		while ( !m_lstPersRefs.IsEmpty() )
		{
			pPersRefItem = m_lstPersRefs.RemoveHead();
			delete pPersRefItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::InsertByAscendingTime

void CPersRefMgr::InsertByAscendingTime( CPersRefItem *pPersRef, BOOL fPaste )
{
	ASSERT( pPersRef );
	if ( pPersRef == NULL )
	{
		return;
	}

	;
	POSITION pos2, pos1 = m_lstPersRefs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		CPersRefItem* pPersRefItem = m_lstPersRefs.GetNext( pos1 );

		if( pPersRefItem->m_dwMeasure == pPersRef->m_dwMeasure )
		{
			if( fPaste )
			{
				if( pPersRefItem->m_bBeat == pPersRef->m_bBeat )
				{
					// replace item
					m_lstPersRefs.InsertBefore( pos2, pPersRef );
					m_lstPersRefs.RemoveAt( pos2 );
					delete pPersRefItem;
					return;
				}
			}
			if( pPersRefItem->m_bBeat >= pPersRef->m_bBeat )
			{
				// insert before pos2 (current position of pPersRefItem)
				m_lstPersRefs.InsertBefore( pos2, pPersRef );
				return;
			}
		}
		if( pPersRefItem->m_dwMeasure > pPersRef->m_dwMeasure )
		{
			// insert before pos2 (current position of pPersRefItem)
			m_lstPersRefs.InsertBefore( pos2, pPersRef );
			return;
		}
	}
	// insert at end of list
	m_lstPersRefs.AddTail( pPersRef );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::RemoveItem

BOOL CPersRefMgr::RemoveItem( CPersRefItem* pItem )
{
	POSITION pos2;
	POSITION pos1 = m_lstPersRefs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		if ( m_lstPersRefs.GetNext( pos1 ) == pItem )
		{
			m_lstPersRefs.RemoveAt( pos2 );
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr::GetBoundariesOfSelectedPersRefs

void CPersRefMgr::GetBoundariesOfSelectedPersRefs( long *plStart, long *plEnd )
{
	ASSERT( plStart );
	ASSERT( plEnd );

	long lEnd = -1;
	BOOL fSetStart = FALSE;

	POSITION pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		CPersRefItem* pItem = m_lstPersRefs.GetNext( pos );

		if( pItem->m_fSelected )
		{
			if( lEnd < pItem->m_mtTime )
			{
				lEnd = pItem->m_mtTime;
			}
			if( !fSetStart )
			{
				fSetStart = TRUE;
				*plStart = pItem->m_mtTime;
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
// CPersRefMgr::DeleteBetweenTimes

BOOL CPersRefMgr::DeleteBetweenTimes( long lStart, long lEnd )
{
	BOOL fChanged = FALSE;

	// Iterate through the list
	POSITION pos2, pos = m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		// Save the current position
		pos2 = pos;
		CPersRefItem* pItem = m_lstPersRefs.GetNext( pos );

		// If the tempo occurs between lStart and lEnd, delete it
		if( (pItem->m_mtTime >= lStart) && (pItem->m_mtTime <= lEnd) ) 
		{
			m_lstPersRefs.RemoveAt( pos2 );
			delete pItem;
			fChanged = TRUE;
		}
	}

	return fChanged;
}
