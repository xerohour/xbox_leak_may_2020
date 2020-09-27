// StyleRefMgr.cpp : implementation file
//

/*--------------
@doc STYLE_REFSAMPLE
--------------*/

#include "stdafx.h"
#include "StyleRefIO.h"
#include "StyleRefStripMgr.h"
#include "StyleRefMgr.h"
#include "timeline.h"
#include "DLLJazzDataObject.h"
#include "PropPageMgr.h"
#include <dmusici.h>
#include <dmusicf.h>
#include <mmreg.h>
#include <StyleDesigner.h>
#include <SegmentDesigner.h>
#include <RiffStrm.h>
#include <initguid.h>
#include "SegmentGuids.h"
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

POSITION GetFirstValidStyleRef(POSITION pos, const CTypedPtrList<CPtrList, CStyleRefItem*> &lstStyleRefs )
{
	CStyleRefItem* pItem;
	POSITION posToReturn;
	while( pos )
	{
		posToReturn = pos;
		pItem = lstStyleRefs.GetNext( pos );
		if( pItem->m_pIStyleDocRootNode )
		{
			return posToReturn;
		}
	}
	return NULL;
}

void CStyleRefMgr::ClocksToMeasure(MUSIC_TIME mtTime, DWORD& rdwMeasure)
{
	//ASSERT( mtTime >= 0 );
	if( mtTime <= 0 )
	{
		rdwMeasure = 0;
		return;
	}

	DMUS_TIMESIGNATURE TimeSignature;

	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	pos = GetFirstValidStyleRef( pos, m_lstStyleRefs );
	if( !pos )
	{
		// Default to 4/4, since this is what the Timeline defaults to
		TimeSignature.bBeatsPerMeasure = 4;
		TimeSignature.bBeat = 4;
		rdwMeasure = ::ClocksToMeasure(mtTime, TimeSignature);
		return;
	}

	CStyleRefItem* pStyleRefItem = NULL;
	MUSIC_TIME mtSRTime = 0;
	DWORD dwCurrentMeasure = 0;

	// Initialize TimeSignature with the TimeSig of the first valid item
	pStyleRefItem = m_lstStyleRefs.GetAt( pos );
	TimeSignature = pStyleRefItem->m_TimeSignature;

	while( pos )
	{
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );

		// This should be a real StyleRef
		ASSERT( !pStyleRefItem->m_StyleListInfo.strName.IsEmpty() );

		MUSIC_TIME mtMeasureClocks = ClocksPerMeasure(TimeSignature);
		long lNumMeasures = pStyleRefItem->m_dwMeasure - dwCurrentMeasure;
		if (mtSRTime + mtMeasureClocks * lNumMeasures > mtTime)
		{
			break;
		}
		else
		{
			mtSRTime += mtMeasureClocks * lNumMeasures;
			dwCurrentMeasure = pStyleRefItem->m_dwMeasure;
			TimeSignature = pStyleRefItem->m_TimeSignature;
		}

		// Get the next valid StyleRef
		pos = GetFirstValidStyleRef( pos, m_lstStyleRefs );
	}

	mtTime -= mtSRTime;
	rdwMeasure = dwCurrentMeasure + ::ClocksToMeasure(mtTime, TimeSignature);
}

MUSIC_TIME CStyleRefMgr::MeasureToClocks(DWORD dwMeasure)
{ 
	DMUS_TIMESIGNATURE TimeSignature;

	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	pos = GetFirstValidStyleRef( pos, m_lstStyleRefs );
	if( !pos )
	{
		// Default to 4/4, since this is what the Timeline defaults to
		TimeSignature.bBeatsPerMeasure = 4;
		TimeSignature.bBeat = 4;
		return ClocksPerMeasure(TimeSignature) * (MUSIC_TIME) dwMeasure; 
	}

	CStyleRefItem* pStyleRefItem = NULL;
	MUSIC_TIME mtSRTime = 0;
	DWORD dwCurrentMeasure = 0;

	// Initialize TimeSignature with the TimeSig of the first valid item
	pStyleRefItem = m_lstStyleRefs.GetAt( pos );
	TimeSignature = pStyleRefItem->m_TimeSignature;

	while( pos )
	{
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );

		// This should be a real StyleRef
		ASSERT( !pStyleRefItem->m_StyleListInfo.strName.IsEmpty() );

		if( pStyleRefItem->m_dwMeasure <= dwMeasure)
		{
			mtSRTime += ClocksPerMeasure(TimeSignature) * 
				(MUSIC_TIME) (pStyleRefItem->m_dwMeasure - dwCurrentMeasure);
			dwCurrentMeasure = pStyleRefItem->m_dwMeasure;
			TimeSignature = pStyleRefItem->m_TimeSignature;
		}
		else break;

		// Get the next valid StyleRef
		pos = GetFirstValidStyleRef( pos, m_lstStyleRefs );
	}
	return 
		mtSRTime + 
		ClocksPerMeasure(TimeSignature) * (MUSIC_TIME) (dwMeasure - dwCurrentMeasure);
}

////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr constructor/destructor 

CStyleRefMgr::CStyleRefMgr()
{
	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pDMProdFramework = NULL;
	m_pPropertyPage = NULL;
	m_pPropPageMgr = NULL;
	m_pCopyDataObject = NULL;
	m_pStyleRefStrip = NULL;
	m_pIDMTrack = NULL;
	m_pISegmentNode = NULL;

	// Initially we don't need to be saved
	m_fDirty = FALSE;

	// By default, belong to Group 1
	m_dwGroupBits = 1;
	m_dwOldGroupBits = 1;

	// Initialize the track flags
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
	m_dwProducerOnlyFlags = 0;

	// By default, don't activate the variation seed
	m_fVariationSeedActive = FALSE;
	m_dwVariationSeed = 1;

	// Create a strip
	m_pStyleRefStrip = new CStyleRefStrip(this);
	ASSERT( m_pStyleRefStrip );
}

CStyleRefMgr::~CStyleRefMgr()
{
	// Delete all the style references in m_lstStyleRefs
	EmptyStyleRefList();

	// Clean up our references
	if( m_pStyleRefStrip )
	{
		m_pStyleRefStrip->Release();
		m_pStyleRefStrip = NULL;
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
// CStyleRefMgr IStyleRefMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::IsMeasureBeatOpen

HRESULT STDMETHODCALLTYPE CStyleRefMgr::IsMeasureOpen( DWORD dwMeasure )
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
	CStyleRefItem* pStyleRefItem;
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );
		ASSERT( pStyleRefItem );
		if( pStyleRefItem->m_dwMeasure > dwMeasure )
		{
			pos = NULL;
		}
		else if( pStyleRefItem->m_pIStyleDocRootNode && (pStyleRefItem->m_dwMeasure == dwMeasure) ) // Styles must be on a measure boundary
		{
			hr = S_FALSE;
			pos = NULL;
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr IDMUSProdStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::GetParam

HRESULT STDMETHODCALLTYPE CStyleRefMgr::GetParam(
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

	if( ::IsEqualGUID(guidType, GUID_TimeSignature) ||
	    ::IsEqualGUID(guidType, GUID_StyleNode) ||
		::IsEqualGUID(guidType, GUID_IDirectMusicStyle) )
	{
		if( ::IsEqualGUID( guidType, GUID_TimeSignature) )
		{
			// This is wrong - if we don't have any valid StyleRefs, we return an error code (not a default value)
			//*(DMUS_TIMESIGNATURE*) pData = m_DefaultTimeSignature;
		}
		else if( ::IsEqualGUID( guidType, GUID_StyleNode) )
		{
			*(IDMUSProdNode**) pData = NULL;
		}
		else
		{
			*(IDirectMusicStyle**) pData = NULL;
		}

		// Initialize pmtNext
		if( pmtNext )
		{
			*pmtNext = 0;
		}

		// Search through StyleRef list for first valid time signature
		CStyleRefItem* pItem = NULL;
		POSITION pos = m_lstStyleRefs.GetHeadPosition();
		pos = GetFirstValidStyleRef( pos, m_lstStyleRefs );

		if( !pos )
		{
			// No valid StyleRefs in this track.
			return DMUS_E_NOT_FOUND;
		}

		// Initialize Previous StyleRef Item
		CStyleRefItem* pPrevious = m_lstStyleRefs.GetAt( pos );

		// Search through StyleRef list for valid StyleRefs
		MUSIC_TIME mtSRTime = 0;
		while( pos )
		{
			pItem = m_lstStyleRefs.GetNext( pos );
			ASSERT( pItem->m_pIStyleDocRootNode );

			mtSRTime = MeasureToClocks( pItem->m_dwMeasure );
			if( mtTime < mtSRTime )
			{
				// Set next	
				if( pmtNext )
				{
					*pmtNext = MeasureToClocks( pItem->m_dwMeasure ) - mtTime;
				}
				break;
			}

			pPrevious = pItem;

			// Get the next valid StyleRef
			pos = GetFirstValidStyleRef( pos, m_lstStyleRefs );
		}

		// We should have found a valid TimeSig
		ASSERT( pPrevious );

		if( ::IsEqualGUID( guidType, GUID_TimeSignature) )
		{
			*(DMUS_TIMESIGNATURE*) pData = pPrevious->m_TimeSignature;
		}
		else if( ::IsEqualGUID( guidType, GUID_StyleNode) )
		{
			IDMUSProdNode *pIStyleNode = pPrevious->m_pIStyleDocRootNode;
			pIStyleNode->AddRef();

			*(IDMUSProdNode**) pData = pIStyleNode;
		}
		else
		{
			IDirectMusicStyle *pIDMStyle = NULL;
			pPrevious->m_pIStyleDocRootNode->GetObject( CLSID_DirectMusicStyle,
													IID_IDirectMusicStyle,
													(void**)&pIDMStyle );

			*(IDirectMusicStyle**) pData = pIDMStyle;
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
		str.LoadString(m_pStyleRefStrip->m_nLastEdit);
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

		// Search through StyleRef list for first valid time signature
		POSITION pos = m_lstStyleRefs.GetHeadPosition();
		pos = GetFirstValidStyleRef( pos, m_lstStyleRefs );

		if( !pos )
		{
			// No valid StyleRefs in this track.
			pDMUSProdReferencedNodes->dwArraySize = 0;
			return S_OK;
		}

		DWORD dwIndex = 0;

		// Search through StyleRef list for valid StyleRefs
		while( pos )
		{
			CStyleRefItem* pItem = m_lstStyleRefs.GetNext( pos );
			ASSERT( pItem->m_pIStyleDocRootNode );

			if( pDMUSProdReferencedNodes->apIDMUSProdNode
			&&	pDMUSProdReferencedNodes->dwArraySize > dwIndex )
			{
				pDMUSProdReferencedNodes->apIDMUSProdNode[dwIndex] = pItem->m_pIStyleDocRootNode;
				pDMUSProdReferencedNodes->apIDMUSProdNode[dwIndex]->AddRef();
			}

			// Get the next valid StyleRef
			pos = GetFirstValidStyleRef( pos, m_lstStyleRefs );
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
// CStyleRefMgr::SetParam

HRESULT STDMETHODCALLTYPE CStyleRefMgr::SetParam(
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
// CStyleRefMgr::IsParamSupported

// support GUID_TimeSignature for maintaining the time signature
// support GUID_DocRootNode for maintaining pointer to DocRoot node
HRESULT STDMETHODCALLTYPE CStyleRefMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	if( ::IsEqualGUID( guidType, GUID_TimeSignature ) 
	||  ::IsEqualGUID( guidType, GUID_DocRootNode )
	||  ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) 
	||  ::IsEqualGUID( guidType, GUID_StyleNode )
	||	::IsEqualGUID( guidType, GUID_IDirectMusicStyle )
	||	::IsEqualGUID( guidType, GUID_Segment_ReferencedNodes ) )
	{
		return S_OK;
	}
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CStyleRefMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pStyleRefStrip != NULL );

	if( (m_pStyleRefStrip == NULL) 
	|| !(dwGroupBits & m_dwGroupBits) )
	{
		return E_FAIL;
	}

	HRESULT hr = E_INVALIDARG;

	// All Tracks Added
	if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		// Make sure host has latest version of data
		// May have changed during load if user prompted to choose Style from File Open dialog
		if( m_fDirty )
		{
			OnDataChanged();
			m_fDirty = FALSE;
		}
		return S_OK;
	}

	// Track deleted
	if( ::IsEqualGUID( rguidType, GUID_Segment_DeletedTrack ) )
	{
		// Notify other StripMgrs that we're removing a Style track, and that the
		// time signature may have changed
		IDMUSProdTimeline *pTimeline = (IDMUSProdTimeline *)pData;
		if( pTimeline )
		{
			pTimeline->NotifyStripMgrs( GUID_IDirectMusicStyle, dwGroupBits, NULL );
			pTimeline->NotifyStripMgrs( GUID_TimeSignature, dwGroupBits, NULL );
		}
		return S_OK;
	}

	// Track created
	if( ::IsEqualGUID( rguidType, GUID_Segment_CreateTrack ) )
	{
		// Notify other StripMgrs that we're creating a Style track, and that the
		// time signature may have changed
		if( m_pTimeline )
		{
			m_pTimeline->NotifyStripMgrs( GUID_IDirectMusicStyle, dwGroupBits, NULL );
			m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, dwGroupBits, NULL );
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

			if( pINode)
			{
				// Find which Style changed
				POSITION pos = m_lstStyleRefs.GetHeadPosition();
				while( pos )
				{
					CStyleRefItem* pStyleRefItem = m_lstStyleRefs.GetNext( pos );

					if( pStyleRefItem
					&&  pStyleRefItem->m_pIStyleDocRootNode == pINode )
					{
						// This Style was removed from the Project Tree
						SetStyleReference( NULL, pStyleRefItem );

						// Set undo text resource id
						if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileDeleted) )
						{
							m_pStyleRefStrip->m_nLastEdit = IDS_DELETE;
							hr = S_OK;
						}
						else
						{
							hr = S_FALSE;
						}
					}
				}
				if( m_pTimeline )
				{
					m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_dwGroupBits, NULL );
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileReplaced) ) 
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find which Style changed
				POSITION pos = m_lstStyleRefs.GetHeadPosition();
				while( pos )
				{
					CStyleRefItem* pStyleRefItem = m_lstStyleRefs.GetNext( pos );

					if( pStyleRefItem
					&&  pStyleRefItem->m_pIStyleDocRootNode == pINode )
					{
						// This Style was replaced in the Project Tree, set to new Style pointer
						SetStyleReference( (IDMUSProdNode *)pFrameworkMsg->pData, pStyleRefItem );

						// This Style may have a new time signature
						IDMUSProdStyleInfo* pIStyleInfo;
						if( SUCCEEDED ( pStyleRefItem->m_pIStyleDocRootNode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pIStyleInfo ) ) )
						{
							DMUSProdTimeSignature timeSig;
							pIStyleInfo->GetTimeSignature( &timeSig ); 

							pStyleRefItem->m_TimeSignature.bBeatsPerMeasure = timeSig.bBeatsPerMeasure;
							pStyleRefItem->m_TimeSignature.bBeat = timeSig.bBeat;
							pStyleRefItem->m_TimeSignature.wGridsPerBeat = timeSig.wGridsPerBeat;

							pIStyleInfo->Release();
						}

						m_pStyleRefStrip->m_nLastEdit = IDS_REPLACE;
						hr = S_OK;
					}
				}
				if( m_pTimeline )
				{
					m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_dwGroupBits, NULL );
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, DOCROOT_GuidChange) 
			 ||  ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileNameChange) ) 
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find which Style changed
				POSITION pos = m_lstStyleRefs.GetHeadPosition();
				while( pos )
				{
					CStyleRefItem* pStyleRefItem = m_lstStyleRefs.GetNext( pos );

					if( pStyleRefItem
					&&  pStyleRefItem->m_pIStyleDocRootNode == pINode )
					{
						// Set undo text resource id
						m_pStyleRefStrip->m_nLastEdit = IDS_CHANGE_LINK;
						hr = S_OK;
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, STYLE_PChannelChange) ) 
		{
			SyncWithDirectMusic();
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, STYLE_NameChange) ) 
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find which Style changed
				POSITION pos = m_lstStyleRefs.GetHeadPosition();
				while( pos )
				{
					CStyleRefItem* pStyleRefItem = m_lstStyleRefs.GetNext( pos );

					if( pStyleRefItem
					&&  pStyleRefItem->m_pIStyleDocRootNode == pINode )
					{
						// This Style was renamed
						DMUSProdListInfo ListInfo;
						ZeroMemory( &ListInfo, sizeof(ListInfo) );
						ListInfo.wSize = sizeof(ListInfo);

						if( SUCCEEDED ( pStyleRefItem->m_pIStyleDocRootNode->GetNodeListInfo ( &ListInfo ) ) )
						{
							IDMUSProdProject* pIProject;

							if( ListInfo.bstrName )
							{
								pStyleRefItem->m_StyleListInfo.strName = ListInfo.bstrName;
								::SysFreeString( ListInfo.bstrName );
							}
							if( ListInfo.bstrDescriptor )
							{
								pStyleRefItem->m_StyleListInfo.strDescriptor = ListInfo.bstrDescriptor;
								::SysFreeString( ListInfo.bstrDescriptor );
							}
							if( SUCCEEDED ( m_pDMProdFramework->FindProject( pStyleRefItem->m_pIStyleDocRootNode, &pIProject ) ) )
							{
								BSTR bstrProjectName;

								pStyleRefItem->m_StyleListInfo.pIProject = pIProject;
	//							pStyleRefItem->m_StyleListInfo.pIProject->AddRef();		intentionally missing

								if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
								{
									pStyleRefItem->m_StyleListInfo.strProjectName = bstrProjectName;
									::SysFreeString( bstrProjectName );
								}

								pIProject->Release();
							}
						}

						// Update DocRoot file GUID
						m_pDMProdFramework->GetNodeFileGUID ( pStyleRefItem->m_pIStyleDocRootNode, &pStyleRefItem->m_StyleListInfo.guidFile );

						// Set undo text resource id
						m_pStyleRefStrip->m_nLastEdit = IDS_CHANGE_NAME;
						hr = S_OK;
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, STYLE_TimeSigChange) )
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find which Style changed
				POSITION pos = m_lstStyleRefs.GetHeadPosition();
				while( pos )
				{
					CStyleRefItem* pStyleRefItem = m_lstStyleRefs.GetNext( pos );

					if( pStyleRefItem
					&&  pStyleRefItem->m_pIStyleDocRootNode == pINode )
					{
						// This Style has a new time signature
						IDMUSProdStyleInfo* pIStyleInfo;
						if( SUCCEEDED ( pStyleRefItem->m_pIStyleDocRootNode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pIStyleInfo ) ) )
						{
							DMUSProdTimeSignature timeSig;
							pIStyleInfo->GetTimeSignature( &timeSig ); 

							pStyleRefItem->m_TimeSignature.bBeatsPerMeasure = timeSig.bBeatsPerMeasure;
							pStyleRefItem->m_TimeSignature.bBeat = timeSig.bBeat;
							pStyleRefItem->m_TimeSignature.wGridsPerBeat = timeSig.wGridsPerBeat;

							pIStyleInfo->Release();
						}

						hr = S_FALSE;
					}
				}
				if( m_pTimeline )
				{
					m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_dwGroupBits, NULL );
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
			m_pTimeline->StripInvalidateRect( m_pStyleRefStrip, NULL, TRUE );
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
// CStyleRefMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CStyleRefMgr::GetStripMgrProperty(
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

			pioTrackHeader->guidClassID = CLSID_DirectMusicStyleTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = NULL;
			pioTrackHeader->fccType = DMUS_FOURCC_STYLE_TRACK_LIST;
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
// CStyleRefMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CStyleRefMgr::SetStripMgrProperty(
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
			if ( m_pStyleRefStrip )
			{
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)m_pStyleRefStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pStyleRefStrip );
			}
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwOldGroupBits );
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
				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pStyleRefStrip, CLSID_DirectMusicStyleTrack, m_dwGroupBits, 0 );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwGroupBits);
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
			m_dwOldGroupBits = pioTrackHeader->dwGroup;
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
// CStyleRefMgr IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::GetClassID

HRESULT CStyleRefMgr::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pClsId == NULL )
	{
		return E_POINTER;
	}

	// return our CLSID
	memcpy( pClsId, &CLSID_StyleRefMgr, sizeof( CLSID ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::IsDirty

HRESULT CStyleRefMgr::IsDirty()
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
// CStyleRefMgr::Load

HRESULT CStyleRefMgr::Load( IStream* pIStream )
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

	// Remove all existing style references
	EmptyStyleRefList();

	m_strLastStyleName.Empty();

	// Clear variation seed info.
	m_fVariationSeedActive = FALSE;
	m_dwVariationSeed = 1;

	// Load the Track
	MMCKINFO ck;
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_STYLE_TRACK_LIST: // StyleRef List
						BOOL fChanged;
						LoadStyleRefList(pIRiffStream, &ck, FALSE, 0, fChanged);
						pIRiffStream->Ascend( &ck, 0 );
						hr = S_OK;
						break;
				}
				break;

			case DMUS_FOURCC_STYLE_REF_DESIGN_CHUNK:
				LoadStyleRefDesign( pIStream, &ck );
				break;

		}

		pIRiffStream->Ascend( &ck, 0 );
	}

	m_strLastStyleName.Empty();

	pIRiffStream->Release();

	SyncWithDirectMusic();

	return hr;
}


HRESULT CStyleRefMgr::LoadStyleRefList( IDMUSProdRIFFStream* pIRiffStream,
										MMCKINFO* pckParent,
										BOOL fPaste,
										MUSIC_TIME mtPasteTime,
										BOOL &fChanged )
{
	MMCKINFO ckMain;
	MMCKINFO ck;
	HRESULT hr = S_OK;

	if( pIRiffStream == NULL )
	{
		return E_INVALIDARG;
	}
	
	IStream* pIStream = pIRiffStream->GetStream();
	if( pIStream == NULL )
	{
		return E_FAIL;
	}

	if( fPaste )
	{
		DWORD dwCurrentFilePos = StreamTell( pIStream );

		MUSIC_TIME mtAdjustment = 0;;

		ASSERT( m_pTimeline != NULL );
		if( m_pTimeline )
		{
			// Get Timeline length
			VARIANT var;
			m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
			MUSIC_TIME mtMaxTimelineLength = V_I4( &var );

			// Determine new paste time to enforce boundaries
			while ( pIRiffStream->Descend( &ckMain, pckParent, 0 ) == 0  )
			{
				if( ckMain.ckid == FOURCC_LIST
				&&  ckMain.fccType == DMUS_FOURCC_STYLE_REF_LIST )
				{

					while( pIRiffStream->Descend( &ck, &ckMain, 0 ) == 0 )
					{
						switch( ck.ckid )
						{
							case DMUS_FOURCC_TIME_STAMP_CHUNK:
							{
								DWORD dwTime;
								DWORD cb;
								hr = pIStream->Read( &dwTime, sizeof( dwTime ), &cb );
								if (FAILED(hr) || cb != sizeof( dwTime ) ) 
								{
									if (SUCCEEDED(hr)) hr = E_FAIL;
									pIRiffStream->Ascend( &ck, 0 );
									goto ON_END;
								}

								MUSIC_TIME mtTime = dwTime;
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
							break;
						}

						pIRiffStream->Ascend( &ck, 0 );
					}

					pIRiffStream->Ascend( &ckMain, 0 );
				}

				pIRiffStream->Ascend( &ckMain, 0 );
			}
		}

		// New paste time which will enforce strip boundaries
		mtPasteTime += mtAdjustment;

		// Restore our position back to the start of the StyleRefs
		StreamSeek( pIStream, dwCurrentFilePos, STREAM_SEEK_SET );
	}

	while ( pIRiffStream->Descend( &ckMain, pckParent, 0 ) == 0  )
	{
		if( ckMain.ckid == FOURCC_LIST
		&&  ckMain.fccType == DMUS_FOURCC_STYLE_REF_LIST )
		{
			if( SUCCEEDED( LoadStyleRef(pIRiffStream, &ckMain, fPaste, mtPasteTime) ) )
			{
				fChanged = TRUE;
			}
			pIRiffStream->Ascend( &ckMain, 0 );
		}

		pIRiffStream->Ascend( &ckMain, 0 );
	}

	// If there are existing StyleRefs, we're not pasting, and there
	// are no valid StyleRefs, update all existing StyleRefs using a TimeSig of 4/4.
	if( !m_lstStyleRefs.IsEmpty() && !fPaste &&
		!GetFirstValidStyleRef( m_lstStyleRefs.GetHeadPosition(), m_lstStyleRefs ) )
	{
		long lMeasureClocks = DMUS_PPQ * 4; // 4 quarter note beats
		POSITION pos = m_lstStyleRefs.GetHeadPosition();
		while( pos )
		{
			CStyleRefItem* pTmpItem = m_lstStyleRefs.GetNext( pos );
			pTmpItem->m_dwMeasure = pTmpItem->m_dwMeasure / lMeasureClocks;
		}
	}

ON_END:
	pIStream->Release();
	return hr;
}

HRESULT CStyleRefMgr::LoadStyleRef( IDMUSProdRIFFStream* pIRiffStream, 
									MMCKINFO* pckParent,
									BOOL fPaste,
									MUSIC_TIME mtPasteTime )
{
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwCurrentFilePos;
	IDMUSProdNode* pIDocRoot = NULL;
	CString strStyleName;
	MUSIC_TIME mtTime = 0;

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

	CStyleRefItem* pItem = new CStyleRefItem( this );
	if( pItem == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto ON_END;
	}

	dwCurrentFilePos = StreamTell( pIStream );


	while( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
		case DMUS_FOURCC_TIME_STAMP_CHUNK:
			{
				DWORD dwTime;
				DWORD cb;
				hr = pIStream->Read( &dwTime, sizeof( dwTime ), &cb );
				if (FAILED(hr) || cb != sizeof( dwTime ) ) 
				{
					if (SUCCEEDED(hr)) hr = E_FAIL;
					pIRiffStream->Ascend( &ck, 0 );
					goto ON_END;
				}

				mtTime = dwTime;
				if( fPaste )
				{
					mtTime += mtPasteTime;
				}
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
					// Store Style name
					ReadMBSfromWCS( pIStream, ckName.cksize, &strStyleName );
				}
			}
			break;
		}

		pIRiffStream->Ascend( &ck, 0 );
		dwCurrentFilePos = StreamTell( pIStream );
	}

	if( pIDocRoot == NULL )
	{
		// Do we have a Style name?
		if( !strStyleName.IsEmpty() )
		{
			// Framework could not resolve Style file reference
			// so we will ask user to help
			pIDocRoot = FindStyle( strStyleName, pIStream );
		}
	}

	if( pIDocRoot )
	{
		hr = SetStyleReference( pIDocRoot, pItem );
		pIDocRoot->Release();

		if( FAILED ( hr ) )
		{
			goto ON_END;
		}
	}

	// Set the measure this StyleRef belongs to
	if( !GetFirstValidStyleRef( m_lstStyleRefs.GetHeadPosition(), m_lstStyleRefs ) )
	{
		// If no valid StyleRefs, we need to use the TimeSig of this item to compute the measure #
		if( pIDocRoot )
		{
			// m_TimeSignature is only valid if pIDocRoot is non-NULL
			pItem->m_dwMeasure = mtTime / ClocksPerMeasure( pItem->m_TimeSignature );
		}
		else
		{
			// If return value is S_FALSE, we're using m_dwMeasure to store the item's time
			pItem->m_dwMeasure = mtTime;
			hr = S_FALSE;
		}
	}
	else
	{
		// Have valid StyleRefs, can use ClocksToMeasure
		ClocksToMeasure( mtTime, (DWORD&)pItem->m_dwMeasure );
	}
	ASSERT( pItem->m_dwMeasure >= 0 );

ON_END:
	if( FAILED ( hr ) )
	{
		if( pItem )
		{
			delete pItem;
		}
	}
	else
	{
		if( pItem->m_dwMeasure == INVALID_MEASURE )
		{
			delete pItem;
		}
		else
		{
			// If there are existing StyleRefs, we're not pasting, we have a valid StyleRef, and there
			// are no valid StyleRefs, update all existing StyleRefs using the TimeSig of this item.
			if( !m_lstStyleRefs.IsEmpty() && !fPaste && pIDocRoot &&
				!GetFirstValidStyleRef( m_lstStyleRefs.GetHeadPosition(), m_lstStyleRefs ) )
			{
				long lMeasureClocks = ClocksPerMeasure( pItem->m_TimeSignature );
				POSITION pos = m_lstStyleRefs.GetHeadPosition();
				while( pos )
				{
					CStyleRefItem* pTmpItem = m_lstStyleRefs.GetNext( pos );
					pTmpItem->m_dwMeasure = pTmpItem->m_dwMeasure / lMeasureClocks;
				}
			}

			InsertByAscendingTime( pItem );
			if( fPaste )
			{
				pItem->SetSelectFlag( TRUE );
			}
		}
	}

    pIStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::FindStyle

IDMUSProdNode* CStyleRefMgr::FindStyle( CString strStyleName, IStream* pIStream )
{
	IDMUSProdNode*		pIDocRootNode = NULL;
	IDMUSProdNode*		pITargetDirectoryNode = NULL;
	IDMUSProdDocType*	pIDocType = NULL;
	HRESULT				hr;

	ASSERT( m_pDMProdFramework != NULL );
	ASSERT( pIStream != NULL );

	// Get DocType for DLS Collections
	hr = m_pDMProdFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType );
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
		pITargetDirectoryNode = m_pISegmentNode;
	}

	// See if there is a Style named 'strStyleName' in this Project
	if( !strStyleName.IsEmpty() )
	{
		BSTR bstrStyleName = strStyleName.AllocSysString();

		if( FAILED ( m_pDMProdFramework->GetBestGuessDocRootNode( pIDocType,
										 						  bstrStyleName,
																  pITargetDirectoryNode,
																  &pIDocRootNode ) ) )
		{
			pIDocRootNode = NULL;
		}
	}

	if( pIDocRootNode == NULL )
	{
		// Cannot find the Style
		// If user cancelled previous search for this Style, no need to ask again
		if( strStyleName.CompareNoCase( m_strLastStyleName ) == 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR; 
		}
		m_strLastStyleName = strStyleName;

		// Determine File Open dialog prompt
		CString	strOpenDlgTitle;
		if( strStyleName.IsEmpty() )
		{
			strOpenDlgTitle.LoadString( IDS_FILE_OPEN_ANY_STYLE );
		}
		else
		{
			AfxFormatString1( strOpenDlgTitle, IDS_FILE_OPEN_STYLE, strStyleName );
		}
		BSTR bstrOpenDlgTitle = strOpenDlgTitle.AllocSysString();

		// Display File open dialog
		if( m_pDMProdFramework->OpenFile(pIDocType, bstrOpenDlgTitle, pITargetDirectoryNode, &pIDocRootNode) != S_OK )
		{
			// Did not open a file, or opened file other than Style file
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
// CStyleRefMgr::SaveDMRef

HRESULT CStyleRefMgr::SaveDMRef( IDMUSProdRIFFStream* pIRiffStream,
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
																  CLSID_DirectMusicStyle,
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
// CStyleRefMgr::SaveProducerRef

HRESULT CStyleRefMgr::SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode )
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
// CStyleRefMgr::Save

HRESULT CStyleRefMgr::Save( IStream* pIStream, BOOL fClearDirty )
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

	// If the StyleRef list isn't empty, save it
	if ( !m_lstStyleRefs.IsEmpty() )
	{
		// Create a LIST chunk to store the StyleRef data
		ckMain.fccType = DMUS_FOURCC_STYLE_TRACK_LIST;
		if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		POSITION pos = m_lstStyleRefs.GetHeadPosition();
		while( pos )
		{
			CStyleRefItem* pStyleRefItem;
			pStyleRefItem = m_lstStyleRefs.GetNext( pos );
			ASSERT( pStyleRefItem );
			if( pStyleRefItem
			&&  pStyleRefItem->m_StyleListInfo.strName.IsEmpty() == FALSE )
			{
				// Store each StyleRef in its own chunk.
				SaveStyleReference( pIStream, pIRiffStream, pStyleRefItem, 0 );
				//m_TimeSignature = pStyleRefItem->m_TimeSignature;
			}
		}
		// Ascend out of the StyleRef LIST chunk.
		pIRiffStream->Ascend( &ckMain, 0 );
	}

	if( ftFileType == FT_DESIGN )
	{
		SaveStyleRefDesign( pIStream, pIRiffStream );
	}

ON_ERROR:
	pIRiffStream->Release();
    return hr;
}


HRESULT CStyleRefMgr::SaveStyleReference(
			IStream* pIStream, IDMUSProdRIFFStream* pIRiffStream, CStyleRefItem* pStyleRefItem, MUSIC_TIME mtOffset )
{
	HRESULT hr = E_FAIL;

	ASSERT( pStyleRefItem != NULL );

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

	// Do not save empty style references to the DirectMusic object
	if( ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) && !pStyleRefItem->m_pIStyleDocRootNode )
	{
		return S_FALSE;
	}

	// Write DMUS_FOURCC_STYLE_REF_LIST header
	MMCKINFO ckStyleList;
	ckStyleList.fccType = DMUS_FOURCC_STYLE_REF_LIST;
	if( FAILED( pIRiffStream->CreateChunk( &ckStyleList, MMIO_CREATELIST ) ) )
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

		long lTime = MeasureToClocks( pStyleRefItem->m_dwMeasure );
		DWORD dwTime = lTime - mtOffset;
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
	if( pStyleRefItem->m_pIStyleDocRootNode )
	{
		if( ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
		{
			SaveDMRef( pIRiffStream, pStyleRefItem->m_pIStyleDocRootNode, WL_PRODUCER );
		}
		else if( ::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) )
		{
			SaveDMRef( pIRiffStream, pStyleRefItem->m_pIStyleDocRootNode, WL_DIRECTMUSIC );
			if( ftFileType == FT_DESIGN )
			{
				SaveProducerRef( pIRiffStream, pStyleRefItem->m_pIStyleDocRootNode );
			}
		}
	}
	
	if( pIRiffStream->Ascend( &ckStyleList, 0 ) != 0 )
	{
		return E_FAIL;
	}
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::SyncTimeSignatures

HRESULT CStyleRefMgr::SyncTimeSignatures( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CStyleRefItem* pStyleRefItem;

	// Sync timesig info
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );

		// Get Style from selected node
		if( pStyleRefItem->m_pIStyleDocRootNode )
		{
			IDMUSProdStyleInfo* pIStyleInfo;
			if( SUCCEEDED ( pStyleRefItem->m_pIStyleDocRootNode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pIStyleInfo ) ) )
			{
				DMUSProdTimeSignature timeSig;
				pIStyleInfo->GetTimeSignature( &timeSig ); 

				pStyleRefItem->m_TimeSignature.bBeatsPerMeasure = timeSig.bBeatsPerMeasure;
				pStyleRefItem->m_TimeSignature.bBeat = timeSig.bBeat;
				pStyleRefItem->m_TimeSignature.wGridsPerBeat = timeSig.wGridsPerBeat;

				pIStyleInfo->Release();
			}
		}
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::SyncWithDirectMusic

HRESULT CStyleRefMgr::SyncWithDirectMusic( void )
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

		pIMemStream->Release();
	}

	if( SUCCEEDED ( hr ) ) 
	{
		SyncTimeSignatures();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::GetSizeMax

HRESULT CStyleRefMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	UNREFERENCED_PARAMETER(pcbSize);
	// Compute size of stream needed to persist ourself into.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::GetData

// This method is called by CStyleRefPropPageMgr to get data to send to the
// StyleRef property page.
// The CStyleRefStrip::GetData() method is called by CStyleRefStripPropPageMgr
// to get the strip's properties (Group Bits, etc.)
HRESULT STDMETHODCALLTYPE CStyleRefMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected Style reference.
	BOOL fMultipleSelect = FALSE;
	CStyleRefItem* pFirstStyleRefItem = NULL;

	CStyleRefItem* pStyleRefItem;
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );
		if( pStyleRefItem->m_fSelected
		&&  pStyleRefItem->m_StyleListInfo.strName.IsEmpty() == FALSE )
		{
			pFirstStyleRefItem = pStyleRefItem;
			while( pos )
			{
				pStyleRefItem = m_lstStyleRefs.GetNext( pos );
				if( pStyleRefItem->m_fSelected
				&&  pStyleRefItem->m_StyleListInfo.strName.IsEmpty() == FALSE )
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

	// Multiple Styles selected
	if( fMultipleSelect )
	{
		CPropStyleRef* pPropStyleRef = new CPropStyleRef;
		if( pPropStyleRef )
		{
			pPropStyleRef->m_dwMeasure = 0xFFFFFFFF;		// Signifies multiple Styles selected
			*ppData = pPropStyleRef;
			hr = S_OK;
		}
	}

	// One Style selected
	else if( pFirstStyleRefItem )
	{
		CPropStyleRef* pPropStyleRef = new CPropStyleRef( pFirstStyleRefItem );
		if( pPropStyleRef )
		{
			*ppData = pPropStyleRef;
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
// CStyleRefMgr::SyncStyleBandAndTempo

HRESULT CStyleRefMgr::SyncStyleBandAndTempo( CStyleRefItem* pStyleRefItem )
{
	if( m_pISegmentNode )
	{
		IDMUSProdSegmentEdit* pISegmentEdit;
		if( SUCCEEDED( m_pISegmentNode->QueryInterface( IID_IDMUSProdSegmentEdit, (void**)&pISegmentEdit ) ) )
		{
			IDMUSProdStripMgr* pIStripMgr;

			// Make sure there is a Band track
			if( SUCCEEDED( m_pTimeline->GetStripMgr( GUID_IDirectMusicBand, m_dwGroupBits, 0, &pIStripMgr ) ) )
			{
				pIStripMgr->Release();
			}
			else
			{
				IUnknown *punkStripMgr;

				// Create a Band track
				if( SUCCEEDED ( pISegmentEdit->AddStrip( CLSID_DirectMusicBandTrack, m_dwGroupBits, &punkStripMgr ) ) )
				{
					punkStripMgr->Release();
				}
			}

			// Make sure there is a Tempo track
			BOOL fHasTempoStrip;
			pISegmentEdit->ContainsTempoStrip( &fHasTempoStrip );
			if( fHasTempoStrip == FALSE )
			{
				IUnknown *punkStripMgr;

				// Create a Tempo track
				if( SUCCEEDED ( pISegmentEdit->AddStrip( CLSID_DirectMusicTempoTrack, m_dwGroupBits, &punkStripMgr ) ) )
				{
					punkStripMgr->Release();
				}
			}

			RELEASE( pISegmentEdit );
		}
	}

	// Notify the other strips that a new Style was just selected
	long lStyleTime = MeasureToClocks( pStyleRefItem->m_dwMeasure );
	m_pTimeline->NotifyStripMgrs( GUID_Segment_NewStyleSelected, m_dwGroupBits, (void *)&lStyleTime );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::SetData

// This method is called by CStyleRefPropPageMgr in response to user actions
// in the StyleRef Property page.  It changes the currenly selected StyleRef. 
HRESULT STDMETHODCALLTYPE CStyleRefMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected style reference.
	CStyleRefItem* pStyleRefItem;
	pStyleRefItem = FirstSelectedRealStyleRef();

	if( pStyleRefItem )
	{
		CPropStyleRef* pPropStyleRef = new CPropStyleRef( pStyleRefItem );
		if ( pPropStyleRef )
		{
			BOOL fNewStyleAdded = FALSE;

			// Only update if the data has changed
			if( memcmp( pData, pPropStyleRef, sizeof(CPropStyleRef) ) )
			{
				CPropStyleRef* pStyleRef = (CPropStyleRef*)pData;
				HRESULT hr;

				// style has changed, NB, this catches newly created ref node as doc root will be null
				if( pStyleRefItem->m_pIStyleDocRootNode == NULL
				||  pStyleRefItem->m_pIStyleDocRootNode != pStyleRef->m_pIStyleDocRootNode )
				{
					m_pStyleRefStrip->m_nLastEdit = IDS_CHANGE;
					hr = SetStyleReference( pStyleRef->m_pIStyleDocRootNode, pStyleRefItem );

					if( pStyleRef->m_pIStyleDocRootNode )
					{
						fNewStyleAdded = TRUE;
					}
				}

				if( pStyleRefItem->m_dwMeasure != pStyleRef->m_dwMeasure )
				{
					m_pStyleRefStrip->m_nLastEdit = IDS_UNDO_MOVE;
				}

				// Update the first selected style reference
				pStyleRef->ApplyToStyleRefItem( pStyleRefItem );

				// Re-insert the style reference into the list, in case its measure info 
				// changed and it's now out of order w.r.t. the other elements.
				if( RemoveItem( pStyleRefItem ) )
				{
					InsertByAscendingTime( pStyleRefItem );
				}

				// Redraw the style reference strip
				// BUGBUG: Should be smarter and only redraw the style reference that changed
				m_pTimeline->StripInvalidateRect( m_pStyleRefStrip, NULL, TRUE );

				// Let our hosting editor know about the changes
				OnDataChanged();

				// Notify the other strips of possible TimeSig change
				m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_dwGroupBits, NULL );

				// Notify the other strips that a new Style was just selected
				if( fNewStyleAdded )
				{
					SyncStyleBandAndTempo( pStyleRefItem );
				}
			}

			delete pPropStyleRef;
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


HRESULT CStyleRefMgr::SetStyleReference( IDMUSProdNode* pINewStyleDocRootNode, CStyleRefItem* pItem )
{
	HRESULT hr = S_OK;

	// Clean up old DocRoot
	if( pItem->m_pIStyleDocRootNode )
	{
		// Turn off notifications for this node
		if( pItem->m_fRemoveNotify )
		{
			if( m_pISegmentNode )
			{
				hr = m_pDMProdFramework->RemoveFromNotifyList( pItem->m_pIStyleDocRootNode, m_pISegmentNode );
			}
			pItem->m_fRemoveNotify = FALSE;
		}

		// Initialize DocRoot list info
		pItem->m_StyleListInfo.pIProject = NULL;
		pItem->m_StyleListInfo.strProjectName.LoadString( IDS_EMPTY_TEXT );
		pItem->m_StyleListInfo.strName.LoadString( IDS_EMPTY_TEXT );
		pItem->m_StyleListInfo.strDescriptor.LoadString( IDS_EMPTY_TEXT );
		pItem->m_dwBits = 0;
		memset( &pItem->m_guidProject, 0, sizeof( pItem->m_guidProject ) );

		// Initialize timesig info
		pItem->m_TimeSignature.mtTime = 0;
		pItem->m_TimeSignature.bBeatsPerMeasure = 4;
		pItem->m_TimeSignature.bBeat = 4;
		pItem->m_TimeSignature.wGridsPerBeat = 4;

		// Release DocRoot
		pItem->m_pIStyleDocRootNode->Release();
		pItem->m_pIStyleDocRootNode = NULL;
	}

	// Set new DocRoot
	if( pINewStyleDocRootNode )
	{
		// Turn on notifications
		ASSERT( pItem->m_fRemoveNotify == FALSE );
		if( m_pISegmentNode )
		{
			hr = m_pDMProdFramework->AddToNotifyList( pINewStyleDocRootNode, m_pISegmentNode );
			if( SUCCEEDED ( hr ) )
			{
				pItem->m_fRemoveNotify = TRUE;
			}
		}
		
		// Update DocRoot member variable
		pItem->m_pIStyleDocRootNode = pINewStyleDocRootNode;
		pItem->m_pIStyleDocRootNode->AddRef();

		// Update DocRoot list info
		DMUSProdListInfo ListInfo;
		ZeroMemory( &ListInfo, sizeof(ListInfo) );
		ListInfo.wSize = sizeof(ListInfo);

		if( SUCCEEDED ( pItem->m_pIStyleDocRootNode->GetNodeListInfo ( &ListInfo ) ) )
		{
			IDMUSProdProject* pIProject;

			if( ListInfo.bstrName )
			{
				pItem->m_StyleListInfo.strName = ListInfo.bstrName;
				::SysFreeString( ListInfo.bstrName );
			}
			if( ListInfo.bstrDescriptor )
			{
				pItem->m_StyleListInfo.strDescriptor = ListInfo.bstrDescriptor;
				::SysFreeString( ListInfo.bstrDescriptor );
			}
			if( SUCCEEDED ( m_pDMProdFramework->FindProject( pItem->m_pIStyleDocRootNode, &pIProject ) ) )
			{
				BSTR bstrProjectName;

				pItem->m_StyleListInfo.pIProject = pIProject;
//				pItem->m_StyleListInfo.pIProject->AddRef();		intentionally missing

				if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
				{
					pItem->m_StyleListInfo.strProjectName = bstrProjectName;
					::SysFreeString( bstrProjectName );
				}

				pIProject->Release();
			}
		}

		// Update DocRoot file GUID
		m_pDMProdFramework->GetNodeFileGUID ( pItem->m_pIStyleDocRootNode, &pItem->m_StyleListInfo.guidFile );

		// Modify timesig info
		IDMUSProdStyleInfo* pIStyleInfo;
		if( SUCCEEDED ( pItem->m_pIStyleDocRootNode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pIStyleInfo ) ) )
		{
			DMUSProdTimeSignature timeSig;
			pIStyleInfo->GetTimeSignature( &timeSig ); 

			pItem->m_TimeSignature.bBeatsPerMeasure = timeSig.bBeatsPerMeasure;
			pItem->m_TimeSignature.bBeat = timeSig.bBeat;
			pItem->m_TimeSignature.wGridsPerBeat = timeSig.wGridsPerBeat;

			pIStyleInfo->Release();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CStyleRefMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		CStyleRefPropPageMgr* pPPM = new CStyleRefPropPageMgr(m_pDMProdFramework, this);
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
// CStyleRefMgr::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CStyleRefMgr::OnRemoveFromPageManager( void)
{
	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::OnDataChanged

HRESULT STDMETHODCALLTYPE CStyleRefMgr::OnDataChanged( void)
{
	if ( m_pTimeline == NULL )
	{
		// Will be NULL if editor is closed and Framework 
		// sends notification that a referenced file has changed
		return E_FAIL;
	}

	// Let our hosting editor know about the change
	m_pTimeline->OnDataChanged( (IStyleRefMgr*)this );

	return S_OK;
}


HRESULT CStyleRefMgr::GetDirectMusicStyle( IDMUSProdNode* pIStyleDocRoot, IDirectMusicStyle** ppIStyle )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = E_FAIL;

	if( pIStyleDocRoot )
	{
		hr = pIStyleDocRoot->GetObject(CLSID_DirectMusicStyle, IID_IDirectMusicStyle, (void**)ppIStyle);
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::DeleteSelectedStyleRefs

HRESULT CStyleRefMgr::DeleteSelectedStyleRefs()
{
	CStyleRefItem* pStyleRefItem;
	POSITION pos2, pos1 = m_lstStyleRefs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		pStyleRefItem = m_lstStyleRefs.GetNext( pos1 );
		ASSERT( pStyleRefItem );
		if( pStyleRefItem )
		{
			if( pStyleRefItem->m_fSelected
			||  pStyleRefItem->m_StyleListInfo.strName.IsEmpty() )
			{
				m_lstStyleRefs.RemoveAt( pos2 );
				delete pStyleRefItem;
			}
		}
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::SaveSelectedStyleRefs

HRESULT CStyleRefMgr::SaveSelectedStyleRefs(LPSTREAM pIStream, MUSIC_TIME mtOffset)
{
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// If the StyleRef list has anything in it, look for selected StyleRefs
	if( !m_lstStyleRefs.IsEmpty() )
	{
		POSITION pos;
		//MMCKINFO ckMain;

		pos = m_lstStyleRefs.GetHeadPosition();
		while( pos )
		{
			CStyleRefItem* pStyleRefItem;
			pStyleRefItem = m_lstStyleRefs.GetNext( pos );

			ASSERT( pStyleRefItem );
			if( pStyleRefItem
			&&  pStyleRefItem->m_fSelected
			&&  pStyleRefItem->m_StyleListInfo.strName.IsEmpty() == FALSE )
			{
				// Store each StyleRef in its own chunk.
				SaveStyleReference( pIStream, pIRiffStream, pStyleRefItem, mtOffset );
			}

		}
		//pIRiffStream->Ascend( &ckMain, 0 );
	}
	else
	{
		hr = S_FALSE; // Nothing in the list
	}

	pIRiffStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::CreateStyleRef

HRESULT CStyleRefMgr::CreateStyleRef( DWORD dwMeasure, CStyleRefItem*& rpStyleRef )
{
	rpStyleRef = new CStyleRefItem( this );
	if( rpStyleRef == NULL )
	{
		return E_OUTOFMEMORY;
	}

	rpStyleRef->m_StyleListInfo.strProjectName.Empty();
	rpStyleRef->m_StyleListInfo.strName.Empty();
	rpStyleRef->m_StyleListInfo.strDescriptor.Empty();

	rpStyleRef->m_dwMeasure = dwMeasure;

	InsertByAscendingTime( rpStyleRef );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::CreateStyleRef

HRESULT CStyleRefMgr::CreateStyleRef( long lXPos, CStyleRefItem*& rpStyleRef )
{
	rpStyleRef = NULL;

	if( m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	rpStyleRef = new CStyleRefItem( this );
	if( rpStyleRef == NULL )
	{
		return E_OUTOFMEMORY;
	}

	long lMeasure = 0;
	long lBeat = 0;

	HRESULT hr;
	hr = m_pTimeline->PositionToMeasureBeat( m_dwGroupBits, 0, lXPos, &lMeasure, &lBeat );
	ASSERT( SUCCEEDED ( hr ) );

	rpStyleRef->m_StyleListInfo.strProjectName.Empty();
	rpStyleRef->m_StyleListInfo.strName.Empty();
	rpStyleRef->m_StyleListInfo.strDescriptor.Empty();

	rpStyleRef->m_dwMeasure = lMeasure;

	//rpStyleRef->SetSelectFlag( TRUE );
	InsertByAscendingTime( rpStyleRef );

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::MarkSelectedStyleRefs

// marks m_dwUndermined field CStyleRefItems in list
void CStyleRefMgr::MarkSelectedStyleRefs( DWORD dwFlags )
{
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pStyleRefItem;
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );
		ASSERT( pStyleRefItem );
		if ( pStyleRefItem )
		{
			if ( pStyleRefItem->m_fSelected )
			{
				pStyleRefItem->m_dwBits |= dwFlags;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::DeleteMarked

// deletes style references marked by given flag
void CStyleRefMgr::DeleteMarked( DWORD dwFlags )
{
	CStyleRefItem* pStyleRefItem;
	POSITION pos2, pos1 = m_lstStyleRefs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		pStyleRefItem = m_lstStyleRefs.GetNext( pos1 );
		ASSERT( pStyleRefItem );
		if ( pStyleRefItem )
		{
			if( (pStyleRefItem->m_dwBits & dwFlags)
			||  (pStyleRefItem->m_StyleListInfo.strName.IsEmpty()) )
			{
				m_lstStyleRefs.RemoveAt( pos2 );
				delete pStyleRefItem;
			}
		}
	}

	// Update the property page
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::UnMarkStyleRefs

// unmarks flag m_dwUndermined field CStyleRefItems in list
void CStyleRefMgr::UnMarkStyleRefs( DWORD dwFlags )
{
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pStyleRefItem;
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );
		ASSERT( pStyleRefItem );
		if ( pStyleRefItem )
		{
			pStyleRefItem->m_dwBits &= ~dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::UnselectAllKeepBits

void CStyleRefMgr::UnselectAllKeepBits()
{
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pStyleRefItem = m_lstStyleRefs.GetNext( pos );

		ASSERT( pStyleRefItem );
		if( pStyleRefItem )
		{
			pStyleRefItem->m_fSelected = FALSE;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::UnselectAll

void CStyleRefMgr::UnselectAll()
{
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pStyleRefItem = m_lstStyleRefs.GetNext( pos );

		ASSERT( pStyleRefItem );
		if( pStyleRefItem )
		{
			pStyleRefItem->SetSelectFlag( FALSE );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::SelectAll

void CStyleRefMgr::SelectAll()
{
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pStyleRefItem;
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );
		ASSERT( pStyleRefItem );
		if( pStyleRefItem
		&&  pStyleRefItem->m_StyleListInfo.strName.IsEmpty() == FALSE )
		{
			// Only select "real" StyleRefs
			pStyleRefItem->SetSelectFlag( TRUE );
		}
		else
		{
			pStyleRefItem->SetSelectFlag( FALSE );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::IsSelected

BOOL CStyleRefMgr::IsSelected()
{
	// If anything "real" is selected, return TRUE.
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pStyleRefItem;
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );
		ASSERT( pStyleRefItem );
		if( pStyleRefItem )
		{
			if( pStyleRefItem->m_fSelected
			&&  pStyleRefItem->m_StyleListInfo.strName.IsEmpty() == FALSE )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::FirstSelectedStyleRef

CStyleRefItem* CStyleRefMgr::FirstSelectedStyleRef()
{
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pStyleRefItem;
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );
		ASSERT( pStyleRefItem );
		if ( pStyleRefItem )
		{
			if ( pStyleRefItem->m_fSelected )
			{
				return pStyleRefItem;
			}
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::FirstSelectedRealStyleRef

CStyleRefItem* CStyleRefMgr::FirstSelectedRealStyleRef()
{
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pStyleRefItem = m_lstStyleRefs.GetNext( pos );

		if ( pStyleRefItem->m_fSelected
		&&   pStyleRefItem->m_StyleListInfo.strName.IsEmpty() == FALSE )
		{
			return pStyleRefItem;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::CurrentlySelectedStyleRef

CStyleRefItem* CStyleRefMgr::CurrentlySelectedStyleRef()
{
	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pStyleRefItem;
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );
		ASSERT( pStyleRefItem );
		if( pStyleRefItem )
		{
			if( pStyleRefItem->m_fSelected
			&& (pStyleRefItem->m_dwBits & UD_CURRENTSELECTION) )
			{
				return pStyleRefItem;
			}
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::SelectSegment

BOOL CStyleRefMgr::SelectSegment(long lBeginTime, long lEndTime)
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

	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pStyleRefItem;
		pStyleRefItem = m_lstStyleRefs.GetNext( pos );
		ASSERT( pStyleRefItem );
		if ( pStyleRefItem )
		{
			pStyleRefItem->SetSelectFlag( FALSE );
			if( (DWORD)lBeginMeas < pStyleRefItem->m_dwMeasure && pStyleRefItem->m_dwMeasure < (DWORD)lEndMeas )
			{
				pStyleRefItem->SetSelectFlag( TRUE );
				result = TRUE;
			}
			else if( (DWORD)lBeginMeas == pStyleRefItem->m_dwMeasure )
			{
				if( (DWORD)lEndMeas == pStyleRefItem->m_dwMeasure )
				{
					pStyleRefItem->SetSelectFlag( TRUE );
					result = TRUE;
				}
				else
				{
					pStyleRefItem->SetSelectFlag( TRUE );
					result = TRUE;
				}
			}
			else if( (DWORD)lEndMeas == pStyleRefItem->m_dwMeasure )
			{
				pStyleRefItem->SetSelectFlag( TRUE );
				result = TRUE;
			}
		}
	}
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::EmptyStyleRefList

void CStyleRefMgr::EmptyStyleRefList(void)
{
	if( !m_lstStyleRefs.IsEmpty() )
	{
		CStyleRefItem *pStyleRefItem;
		while ( !m_lstStyleRefs.IsEmpty() )
		{
			pStyleRefItem = m_lstStyleRefs.RemoveHead();
			delete pStyleRefItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::InsertByAscendingTime

void CStyleRefMgr::InsertByAscendingTime( CStyleRefItem *pStyleRef )
{
	ASSERT( pStyleRef );
	if ( pStyleRef == NULL )
	{
		return;
	}

	CStyleRefItem* pStyleRefItem;
	POSITION pos2, pos1 = m_lstStyleRefs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		pStyleRefItem = m_lstStyleRefs.GetNext( pos1 );
		ASSERT( pStyleRefItem );
		if( pStyleRefItem )
		{
			if( pStyleRefItem->m_dwMeasure == pStyleRef->m_dwMeasure )
			{
				// replace item
				m_lstStyleRefs.InsertBefore( pos2, pStyleRef );
				m_lstStyleRefs.RemoveAt( pos2 );
				delete pStyleRefItem;
				return;
			}
			if( pStyleRefItem->m_dwMeasure > pStyleRef->m_dwMeasure )
			{
				// insert before pos2 (current position of pStyleRefItem)
				m_lstStyleRefs.InsertBefore( pos2, pStyleRef );
				return;
			}
		}
	}
	// insert at end of list
	m_lstStyleRefs.AddTail( pStyleRef );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::RemoveItem

BOOL CStyleRefMgr::RemoveItem( CStyleRefItem* pItem )
{
	POSITION pos2;
	POSITION pos1 = m_lstStyleRefs.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		if ( m_lstStyleRefs.GetNext( pos1 ) == pItem )
		{
			m_lstStyleRefs.RemoveAt( pos2 );
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::GetBoundariesOfSelectedStyleRefs

void CStyleRefMgr::GetBoundariesOfSelectedStyleRefs( long *plStart, long *plEnd )
{
	ASSERT( plStart );
	ASSERT( plEnd );

	HRESULT hr;
	long lClocks;
	long lEnd = -1;
	BOOL fSetStart = FALSE;

	POSITION pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		CStyleRefItem* pItem = m_lstStyleRefs.GetNext( pos );

		if( pItem->m_fSelected ) 
//		&&	pItem->m_StyleListInfo.strName.IsEmpty() == FALSE )
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
// CStyleRefMgr::DeleteBetweenTimes

BOOL CStyleRefMgr::DeleteBetweenTimes( long lStart, long lEnd )
{
	BOOL fResult = FALSE;

	// Iterate through the list
	CStyleRefItem* pItem;
	POSITION pos2, pos = m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		// Save the current position
		pos2 = pos;
		pItem = m_lstStyleRefs.GetNext( pos );

		long lClocks;
		HRESULT hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits,
											   0,
											   pItem->m_dwMeasure,
											   0,
											   &lClocks );
		ASSERT( SUCCEEDED ( hr ) );
		if( SUCCEEDED ( hr ) )
		{
			// If the Style occurs between lStart and lEnd, delete it
			if( (lClocks >= lStart) && (lClocks <= lEnd) ) 
			{
				m_lstStyleRefs.RemoveAt( pos2 );
				delete pItem;
				fResult = TRUE;
			}
		}
	}

	return fResult;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::LoadStyleRefDesign

HRESULT CStyleRefMgr::LoadStyleRefDesign( LPSTREAM pIStream, 
						MMCKINFO* pckParent )
{
	ASSERT( pIStream );
	ASSERT( pckParent );

	DMUS_IO_STYLE_REF_DESIGN iStyleRefDesign;
	iStyleRefDesign.fVariationSeedActive = FALSE;
	iStyleRefDesign.dwVariationSeed = 1;

	DWORD cbRead, dwSize = min( sizeof( DMUS_IO_STYLE_REF_DESIGN ), pckParent->cksize );
	if( FAILED( pIStream->Read( &iStyleRefDesign, dwSize, &cbRead ) )
	||	(cbRead != dwSize) )
	{
		return E_FAIL;
	}

	m_fVariationSeedActive = iStyleRefDesign.fVariationSeedActive;
	m_dwVariationSeed = iStyleRefDesign.dwVariationSeed;

	// Skip over the rest of the chunk
	if( dwSize < pckParent->cksize )
	{
		StreamSeek( pIStream, pckParent->cksize - dwSize, SEEK_CUR );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefMgr::SaveStyleRefDesign

HRESULT CStyleRefMgr::SaveStyleRefDesign( LPSTREAM pIStream, IDMUSProdRIFFStream* pIRiffStream )
{
	ASSERT( pIStream );
	ASSERT( pIRiffStream );

	// Create a chunk to store the StyleRefMgr design data
    MMCKINFO ckDesign;
	ckDesign.ckid = DMUS_FOURCC_STYLE_REF_DESIGN_CHUNK;
	if( pIRiffStream->CreateChunk( &ckDesign, 0 ) != 0 )
	{
		return E_FAIL;
	}

	DMUS_IO_STYLE_REF_DESIGN oStyleRefDesign;
	oStyleRefDesign.dwVariationSeed = m_dwVariationSeed;
	oStyleRefDesign.fVariationSeedActive = m_fVariationSeedActive;
	DWORD cbWritten;
	if( FAILED( pIStream->Write( &oStyleRefDesign, sizeof( DMUS_IO_STYLE_REF_DESIGN ), &cbWritten ) )
	||	(cbWritten != sizeof( DMUS_IO_STYLE_REF_DESIGN ) ) )
	{
		return E_FAIL;
	}

	// Ascend out of the StyleRefMgr design data chunk.
	pIRiffStream->Ascend( &ckDesign, 0 );

	return S_OK;
}
