// BandMgr.cpp : implementation file
//

/*--------------
@doc BANDSAMPLE
--------------*/

#include "stdafx.h"
#include "BandIO.h"
#include "BandMgr.h"
#include "DLLJazzDataObject.h"
#include "PropPageMgr.h"
#include <dmusici.h>
#include <dmusicf.h>
#include <mmreg.h>
#include <RiffStrm.h>
#include <DMUSProd.h>
#include <DMPPrivate.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBandMgr constructor/destructor 

CBandMgr::CBandMgr()
{
	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pDMProdFramework = NULL;
	m_pDMProdSegmentNode = NULL;
	m_pIPerformance = NULL;
	m_pPropPageMgr = NULL;
	m_pCopyDataObject = NULL;
	m_pBandStrip = NULL;
	m_pIDMTrack = NULL;
	m_pSegmentState = NULL;
	m_pBandItemForEditBandButton = NULL;
	m_dwPChannelForEditBandButton = 0;
	m_fNoUpdateSegment = false;

	// Initially we don't need to be saved
	m_fDirty = FALSE;

	// Initialize autodownload to FALSE
	m_bAutoDownload = FALSE;

	// By default, belong to Group 1 only
	m_dwGroupBits = 1;
	m_dwOldGroupBits = 1;

	// Initialize the track flags
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
	m_dwProducerOnlyFlags = 0;

	// Create a BandStrip
	m_pBandStrip = new CBandStrip(this);
	ASSERT( m_pBandStrip );
}

CBandMgr::~CBandMgr()
{
	// Clean up our references
	RELEASE(m_pSegmentState);
	RELEASE(m_pDMProdFramework);
	RELEASE(m_pIPerformance);

	// No Release!
	/*
	if( m_pDMProdSegmentNode )
	{
		m_pDMProdSegmentNode->Release();
		m_pDMProdSegmentNode = NULL;
	}
	*/

	ASSERT( m_pTimeline == NULL );

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
	if( m_pBandStrip )
	{
		m_pBandStrip->Release();
		m_pBandStrip = NULL;
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

	// Delete all the bands in m_lstBands
	EmptyBandList();
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr IBandMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::IsMeasureBeatOpen

HRESULT STDMETHODCALLTYPE CBandMgr::IsMeasureBeatOpen( DWORD dwMeasure, BYTE bBeat )
{
	HRESULT hr = S_FALSE;
	CBandItem* pBandItem;
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		pBandItem = m_lstBands.GetNext( pos );
		ASSERT( pBandItem );
		if( pBandItem->m_dwMeasure > dwMeasure )
		{
			pos = NULL;
		}
		else if( pBandItem->m_dwMeasure == dwMeasure &&
				 pBandItem->m_bBeat == bBeat )
		{
			hr = S_OK;
			pos = NULL;
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr IDMUSProdStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::ClocksToMeasureBeatTick

HRESULT CBandMgr::ClocksToMeasureBeatTick( MUSIC_TIME mtTime, long* plMeasure, long* plBeat, long* plTick )
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
// CBandMgr::MeasureBeatTickToClocks

HRESULT CBandMgr::MeasureBeatTickToClocks( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmtTime )
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
// GetFirstValidBand

POSITION GetFirstValidBand( POSITION pos, const CTypedPtrList<CPtrList, CBandItem*> &lstBands )
{
	CBandItem* pBandItem;
	POSITION posToReturn;

	while( pos )
	{
		posToReturn = pos;
		pBandItem = lstBands.GetNext( pos );

		if( pBandItem->m_pIBandNode )
		{
			return posToReturn;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// GetBandForPChannelAndTime

bool GetBandForPChannelAndTime( const CTypedPtrList<CPtrList, CBandItem*> &lstBands, DWORD dwPChannel, MUSIC_TIME mtTime, CBandItem **ppBandItem, CBandItem **ppBandItemNext )
{
	if( ppBandItem )
	{
		*ppBandItem = NULL;
	}

	if( ppBandItemNext )
	{
		*ppBandItemNext = NULL;
	}

	// Search through Band list for first "real" Band
	POSITION pos = lstBands.GetHeadPosition();
	pos = GetFirstValidBand( pos, lstBands );

	// Now, find the first "real" band that plays on the given PChannel
	IDMUSProdBandEdit8 *pIDMUSProdBandEdit8;
	bool fFoundPatch = false;
	DWORD dwTempPatch;
	while( pos )
	{
		// Get a pointer to each band item
		CBandItem* pBandItem = lstBands.GetNext( pos );

		// Check if the item exists, and is before or at mtTime
		if( pBandItem->m_pIBandNode
		&&	pBandItem->m_mtTimePhysical <= mtTime
		&&	SUCCEEDED( pBandItem->m_pIBandNode->QueryInterface( IID_IDMUSProdBandEdit8, (void **)&pIDMUSProdBandEdit8 ) ) )
		{
			// Check if the PChannel we're looking for exists
			if( SUCCEEDED( pIDMUSProdBandEdit8->GetPatchForPChannel( dwPChannel, &dwTempPatch ) ) )
			{
				fFoundPatch = true;
				if( ppBandItem )
				{
					*ppBandItem = pBandItem;
				}
			}
			pIDMUSProdBandEdit8->Release();
		}

		// Check if this item is after mtTime, and either we've not found an item, or ppBandItemNext is NULL
		if( mtTime < pBandItem->m_mtTimePhysical
		&&	(!fFoundPatch || !ppBandItemNext) )
		{
			// Break out of the loop
			break;
		}


		// Set next
		// TODO: JD - I'm not sure this check is necessary (i.e., can we assume that if
		// mtTime < pBandItem->m_mtTimePhysical, then ppBandItemNext is valid?)
		if( ppBandItemNext )
		{
			// Check for an item after mtTime
			if( pBandItem->m_pIBandNode
			&&	mtTime < pBandItem->m_mtTimePhysical
			&&	pBandItem->m_mtTimePhysical < mtTime
			&&	SUCCEEDED( pBandItem->m_pIBandNode->QueryInterface( IID_IDMUSProdBandEdit8, (void **)&pIDMUSProdBandEdit8 ) ) )
			{
				// Check if the PChannel we're looking for exists
				if( SUCCEEDED( pIDMUSProdBandEdit8->GetPatchForPChannel( dwPChannel, &dwTempPatch ) ) )
				{
					*ppBandItemNext = pBandItem;
					pIDMUSProdBandEdit8->Release();
					break;
				}
				pIDMUSProdBandEdit8->Release();
			}
		}

		// Get the next "real" Band
		pos = GetFirstValidBand( pos, lstBands );
	}

	return fFoundPatch;
}

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::GetParam

HRESULT STDMETHODCALLTYPE CBandMgr::GetParam(
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

	if( ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		BSTR bstr;
		CString str;
		str.LoadString(m_pBandStrip->m_nLastEdit);
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

	else if( ::IsEqualGUID( guidType, GUID_BandNode ) )
	{
		*(IDMUSProdNode**) pData = NULL;

		// Initialize pmtNext
		if( pmtNext )
		{
			*pmtNext = 0;
		}

		// Search through Band list for first "real" Band
		POSITION pos = m_lstBands.GetHeadPosition();
		pos = GetFirstValidBand( pos, m_lstBands );
		if( pos == NULL )
		{
			// No valid Bands in this track.
			return DMUS_E_NOT_FOUND;
		}

		// Initialize Previous Band Item
		CBandItem* pPrevious = m_lstBands.GetAt( pos );

		// Search through Band list for "real" Bands
		CBandItem* pBandItem = NULL;
		while( pos )
		{
			pBandItem = m_lstBands.GetNext( pos );
			ASSERT( pBandItem->m_pIBandNode );

			if( mtTime < pBandItem->m_mtTimePhysical )
			{
				// Set next	
				if( pmtNext )
				{
					*pmtNext = pBandItem->m_mtTimePhysical - mtTime;
				}
				break;
			}

			pPrevious = pBandItem;

			// Get the next "real" Band
			pos = GetFirstValidBand( pos, m_lstBands );
		}

		// We should have found a valid Band
		ASSERT( pPrevious );
		pPrevious->m_pIBandNode->AddRef();

		*(IDMUSProdNode**) pData = pPrevious->m_pIBandNode;
		return S_OK;
	}

	else if( ::IsEqualGUID( guidType, GUID_BandInterfaceForPChannel )
		 ||  ::IsEqualGUID( guidType, GUID_BandStrip_InstrumentItem )
		 ||  ::IsEqualGUID( guidType, GUID_BandMgrEditForPChannel ) )
	{
		DWORD dwPChannel = 0;
		if( ::IsEqualGUID( guidType, GUID_BandInterfaceForPChannel )
		||	::IsEqualGUID( guidType, GUID_BandMgrEditForPChannel ) )
		{
			DMUSPROD_INTERFACEFORPCHANNEL *pDMUSPROD_INTERFACEFORPCHANNEL = (DMUSPROD_INTERFACEFORPCHANNEL *)pData;
			dwPChannel = pDMUSPROD_INTERFACEFORPCHANNEL->dwPChannel;
			pDMUSPROD_INTERFACEFORPCHANNEL->punkInterface = NULL;
		}
		else
		{
			BandStrip_InstrumentItem *pBandStrip_InstrumentItem = (BandStrip_InstrumentItem *)pData;
			dwPChannel = pBandStrip_InstrumentItem->dwPChannel;
		}

		// Initialize pmtNext
		if( pmtNext )
		{
			*pmtNext = 0;
		}

		// Search through Band list for first "real" Band
		POSITION pos = m_lstBands.GetHeadPosition();
		pos = GetFirstValidBand( pos, m_lstBands );

		// Now, find the first "real" band that plays on the given PChannel
		IDMUSProdBandEdit8 *pIDMUSProdBandEdit8 = NULL;
		bool fFoundPatch = false;
		DWORD dwTempPatch;
		while( pos )
		{
			// Get a pointer to each band item
			CBandItem* pBandItem = m_lstBands.GetNext( pos );

			// Check if the item exists, and is before or at mtTime
			if( pBandItem->m_pIBandNode
			&&	pBandItem->m_mtTimePhysical <= mtTime
			&&	SUCCEEDED( pBandItem->m_pIBandNode->QueryInterface( IID_IDMUSProdBandEdit8, (void **)&pIDMUSProdBandEdit8 ) ) )
			{
				// Check if the PChannel we're looking for exists
				if( SUCCEEDED( pIDMUSProdBandEdit8->GetPatchForPChannel( dwPChannel, &dwTempPatch ) ) )
				{
					fFoundPatch = true;
					if( ::IsEqualGUID( guidType, GUID_BandInterfaceForPChannel ) )
					{
						DMUSPROD_INTERFACEFORPCHANNEL *pDMUSPROD_INTERFACEFORPCHANNEL = (DMUSPROD_INTERFACEFORPCHANNEL *)pData;
						pDMUSPROD_INTERFACEFORPCHANNEL->punkInterface = pBandItem->m_pIBandNode;
						pDMUSPROD_INTERFACEFORPCHANNEL->punkInterface->AddRef();
					}
					else if( ::IsEqualGUID( guidType, GUID_BandMgrEditForPChannel ) )
					{
						DMUSPROD_INTERFACEFORPCHANNEL *pDMUSPROD_INTERFACEFORPCHANNEL = (DMUSPROD_INTERFACEFORPCHANNEL *)pData;
						pDMUSPROD_INTERFACEFORPCHANNEL->punkInterface = static_cast<IDMUSProdBandMgrEdit *>(this);
						pDMUSPROD_INTERFACEFORPCHANNEL->punkInterface->AddRef();
					}
					else
					{
						BandStrip_InstrumentItem *pBandStrip_InstrumentItem = (BandStrip_InstrumentItem *)pData;
						pBandStrip_InstrumentItem->dwPatch = dwTempPatch;
						pBandStrip_InstrumentItem->lPhysicalTime = pBandItem->m_mtTimePhysical;
					}
				}
				pIDMUSProdBandEdit8->Release();
			}

			// Check if this item is after mtTime, and either we've not found an item, or pmtNext is NULL
			if( mtTime < pBandItem->m_mtTimePhysical
			&&	(!fFoundPatch || !pmtNext) )
			{
				// Break out of the loop
				break;
			}


			// Set next
			// TODO: JD - I'm not sure this check is necessary (i.e., can we assume that if
			// mtTime < pBandItem->m_mtTimePhysical, then pmtNext is valid?)
			if( pmtNext )
			{
				// Check for an item after mtTime
				if( pBandItem->m_pIBandNode
				&&	mtTime < pBandItem->m_mtTimePhysical
				&&	pBandItem->m_mtTimePhysical < mtTime
				&&	SUCCEEDED( pBandItem->m_pIBandNode->QueryInterface( IID_IDMUSProdBandEdit8, (void **)&pIDMUSProdBandEdit8 ) ) )
				{
					// Check if the PChannel we're looking for exists
					if( SUCCEEDED( pIDMUSProdBandEdit8->GetPatchForPChannel( dwPChannel, &dwTempPatch ) ) )
					{
						*pmtNext = pBandItem->m_mtTimePhysical - mtTime;
						pIDMUSProdBandEdit8->Release();
						break;
					}
					pIDMUSProdBandEdit8->Release();
				}
			}

			// Get the next "real" Band
			pos = GetFirstValidBand( pos, m_lstBands );
		}

		if( !fFoundPatch )
		{
			// No valid patch before mtTime
			return DMUS_E_NOT_FOUND;
		}

		return S_OK;
	}

	if( ::IsEqualGUID( guidType, GUID_Segment_ReferencedNodes ) )
	{
		DMUSProdReferencedNodes *pDMUSProdReferencedNodes = (DMUSProdReferencedNodes *)pData;

		// Search through Band list for first "real" Band
		POSITION pos = m_lstBands.GetHeadPosition();
		pos = GetFirstValidBand( pos, m_lstBands );
		if( pos == NULL )
		{
			// No valid Bands in this track.
			pDMUSProdReferencedNodes->dwArraySize = 0;
			return S_OK;
		}

		// Index into pDMUSProdReferencedNodes->apIDMUSProdNode to start writing the nodes at
		// Doubles as the count of nodes that we've written into the array
		DWORD dwIndex = 0;

		// Search through Band list for "real" Bands
		while( pos )
		{
			CBandItem* pBandItem = m_lstBands.GetNext( pos );
			ASSERT( pBandItem->m_pIBandNode );

			// Query the band node for its referenced DLS collections
			IDMUSProdGetReferencedNodes *pIDMUSProdGetReferencedNodes;
			if( pBandItem->m_pIBandNode
			&&	SUCCEEDED( pBandItem->m_pIBandNode->QueryInterface( IID_IDMUSProdGetReferencedNodes, (void **)&pIDMUSProdGetReferencedNodes ) ) )
			{
				// If we have an array of nodes, and we haven't filled it yet
				if( pDMUSProdReferencedNodes->apIDMUSProdNode
				&&	pDMUSProdReferencedNodes->dwArraySize > dwIndex )
				{
					// Calculate the size left in the array
					DWORD dwSize = pDMUSProdReferencedNodes->dwArraySize - dwIndex;

					// Ask the band node to fill in the array
					HRESULT hr = pIDMUSProdGetReferencedNodes->GetReferencedNodes( &dwSize, &(pDMUSProdReferencedNodes->apIDMUSProdNode[dwIndex]), pDMUSProdReferencedNodes->dwErrorLength, pDMUSProdReferencedNodes->wcstrErrorText );

					// If successful, increment dwIndex
					if( SUCCEEDED(hr) )
					{
						dwIndex += dwSize;
					}
				}
				else
				{
					// Ask the band node how many array items it required
					DWORD dwSize = 0;
					HRESULT hr = pIDMUSProdGetReferencedNodes->GetReferencedNodes( &dwSize, NULL, pDMUSProdReferencedNodes->dwErrorLength, pDMUSProdReferencedNodes->wcstrErrorText );

					// If successful, increment dwIndex
					if( SUCCEEDED(hr) )
					{
						dwIndex += dwSize;
					}
				}
				pIDMUSProdGetReferencedNodes->Release();
			}

			// Get the next "real" Band
			pos = GetFirstValidBand( pos, m_lstBands );
		}

		HRESULT hr = pDMUSProdReferencedNodes->apIDMUSProdNode && (pDMUSProdReferencedNodes->dwArraySize < dwIndex) ? S_FALSE : S_OK;

		// Store the number of nodes we returned (or that we require)
		pDMUSProdReferencedNodes->dwArraySize = dwIndex;

		return hr;
	}
	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::SetParam

HRESULT STDMETHODCALLTYPE CBandMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	UNREFERENCED_PARAMETER(mtTime);
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		m_pDMProdSegmentNode = reinterpret_cast<IDMUSProdNode*>(pData);
		// No addref!
		//m_pDMProdSegmentNode->AddRef();
		return S_OK;
	}

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::IsParamSupported

HRESULT STDMETHODCALLTYPE CBandMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	if( ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR )
	||	::IsEqualGUID( guidType, GUID_DocRootNode )
	||	::IsEqualGUID( guidType, GUID_BandStrip_InstrumentItem ) // Signifies a Band instrument in the track
	||  ::IsEqualGUID( guidType, GUID_BandNode ) 				 // Signifies a Band in the track
	||	::IsEqualGUID( guidType, GUID_BandInterfaceForPChannel ) // Signifies a Band in the track on a given PChannel
	||	::IsEqualGUID( guidType, GUID_BandMgrEditForPChannel )	 // Signifies the BandMgr, if there is a Band in the track on a given PChannel
	||  ::IsEqualGUID( guidType, GUID_IDirectMusicBand )		 // Signifies a Band track
	||	::IsEqualGUID( guidType, GUID_Segment_ReferencedNodes ) )

	{
		return S_OK;
	}
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CBandMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = E_FAIL;

	if( !(dwGroupBits & m_dwGroupBits) )
	{
		return E_INVALIDARG;
	}

	// TimeSig change
	if( ::IsEqualGUID( rguidType, GUID_TimeSignature ) )
	{
		// Update the m_mtTime value of all Bands
		if( RecomputeBandTimes() )
		{
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->RefreshData();
			}
			OnDataChanged();
		}

		// Redraw our strip
		m_pTimeline->StripInvalidateRect( m_pBandStrip, NULL, TRUE );
		return S_OK;
	}

	// All Tracks Added
	if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		// Fix measure/beat of all Bands
		RecomputeBandMeasureBeats();

		// Redraw our strip
		m_pTimeline->StripInvalidateRect( m_pBandStrip, NULL, TRUE );
		return S_OK;
	}

	// New Style just added to Style strip
	if( ::IsEqualGUID( rguidType, GUID_Segment_NewStyleSelected ) )
	{
		MUSIC_TIME* pmtTime = (MUSIC_TIME *)pData;
		IDMUSProdNode* pIStyleNode;
		IDMUSProdNode* pIDefaultBandNode;
		IDMUSProdStyleInfo* pIStyleInfo;

		// Use dwGroupBits handed to this method
		// dwGroupBits contains GroupBits of the Style track that sent the notification
		if( SUCCEEDED ( m_pTimeline->GetParam( GUID_StyleNode, dwGroupBits, 0, 
											   *pmtTime, NULL, (void *)&pIStyleNode ) ) )
		{
			if( SUCCEEDED ( pIStyleNode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pIStyleInfo ) ) )
			{
				IUnknown *punkBandNode;

				if( SUCCEEDED ( pIStyleInfo->GetDefaultBand( &punkBandNode ) ) )
				{
					if( SUCCEEDED ( punkBandNode->QueryInterface( IID_IDMUSProdNode, (void**)&pIDefaultBandNode ) ) )
					{
						m_pBandStrip->AddBandNode( pIDefaultBandNode, ((*pmtTime) - 1) );
						RELEASE( pIDefaultBandNode );
					}
					RELEASE( punkBandNode );
				}
				RELEASE( pIStyleInfo );
			}
			RELEASE( pIStyleNode );
		}
		OnDataChanged();

		// Redraw our strip
		m_pTimeline->StripInvalidateRect( m_pBandStrip, NULL, TRUE );
		return S_OK;
	}

	if( ::IsEqualGUID(rguidType, GUID_Segment_Start) )
	{
		RELEASE(m_pSegmentState);

		if( pData )
		{
			static_cast<IUnknown *>(pData)->QueryInterface( IID_IDirectMusicSegmentState8, (void **)&m_pSegmentState );
		}

		UpdateBandNodesWithAudiopath();

		return S_OK;
	}

	if( ::IsEqualGUID(rguidType, GUID_Segment_Stop) )
	{
		RELEASE(m_pSegmentState);
		UpdateBandNodesWithAudiopath();
		return S_OK;
	}

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

		// Validate Band name change
		if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, BAND_ValidateNameChange) )  
		{
			// Any name is ok
			hr = S_OK;
		}

		// Band name change
		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, BAND_NameChange) )  
		{
			if( pINode )
			{
				BOOL fUpdate = FALSE;

				POSITION pos = m_lstBands.GetHeadPosition();
				while( pos )
				{
					CBandItem* pBandItem = m_lstBands.GetNext( pos );

					if( pBandItem->m_pIBandNode == pINode )
					{
						BSTR bstrBandName;
						if( SUCCEEDED ( pBandItem->m_pIBandNode->GetNodeName( &bstrBandName ) ) )
						{
							CString strNewName = bstrBandName; 
							::SysFreeString( bstrBandName );

							if( pBandItem->m_strText.Compare( strNewName ) != 0 )
							{
								pBandItem->m_strText = strNewName;
								fUpdate = TRUE;
							}
						}
					}
				}

				if( fUpdate )
				{
					// Redraw our strip
					m_pTimeline->StripInvalidateRect( m_pBandStrip, NULL, TRUE );

					if( !m_fNoUpdateSegment )
					{
						// Get state of TP_FREEZE_UNDO
						BOOL fFreezeUndo = FALSE;
						VARIANT var;
						if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
						{
							fFreezeUndo = V_BOOL(&var);
						}

						// If TP_FREEZE_UNDO is set, set it to FALSE
						if( fFreezeUndo )
						{
							// Need to clear TP_FREEZE_UNDO or the segment will not add an undo state for us
							var.vt = VT_BOOL;
							V_BOOL(&var) = FALSE;
							m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
						}

						// Set the undo string
						if( m_pBandStrip )
						{
							m_pBandStrip->m_nLastEdit = IDS_UNDO_BANDNAME;
						}

						// Let our hosting editor know about the changes
						m_pTimeline->OnDataChanged( (IBandMgr*)this );

						if( fFreezeUndo )
						{
							// Need to set TP_FREEZE_UNDO back to its original setting
							var.vt = VT_BOOL;
							V_BOOL(&var) = TRUE;
							m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
						}
					}

					SyncWithDirectMusic();
					m_fDirty = TRUE;
					hr = S_OK;
				}
			}
		}

		// Band change
		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, GUID_BAND_ChangeNotifyMsg) )  
		{
			if( pINode )
			{
				// Find which band changed
				BOOL fFound = FALSE;
				POSITION pos = m_lstBands.GetHeadPosition();
				while( pos && !fFound )
				{
					CBandItem* pBandItem = m_lstBands.GetNext( pos );
					ASSERT( pBandItem );
					if( pBandItem && (pBandItem->m_pIBandNode == pINode) )
					{
						// This band changed
						fFound = TRUE;
						// Found the band - don't update
						hr = S_FALSE;

						// Validate our performance and segment state pointers
						if( m_pIPerformance && m_pSegmentState )
						{
							// Get the current segment
							IDirectMusicSegment *pSegment;
							if( SUCCEEDED( m_pSegmentState->GetSegment( &pSegment ) ) )
							{
								// Check if our track is in the currenly playing segment
								DWORD dwGroupBits;
								if( SUCCEEDED( pSegment->GetTrackGroup( m_pIDMTrack, &dwGroupBits ) ) )
								{
									// The segment we're in is currently playing - get the current offset
									MUSIC_TIME mtSeek;
									if( SUCCEEDED( m_pSegmentState->GetSeek( &mtSeek ) ) )
									{
										// If this band played before the current offset
										if( pBandItem->m_mtTimePhysical < mtSeek )
										{
											// if there is a next band
											if( pos )
											{
												// Check if the next band is also before the current offset
												pBandItem = m_lstBands.GetNext( pos );
												while( pos && !pBandItem->m_pIBandNode )
												{
													pBandItem = m_lstBands.GetNext( pos );
												}
												if( !pBandItem->m_pIBandNode || (pBandItem->m_mtTimePhysical > mtSeek) )
												{
													// No next band, or next band after the current offset - send band
													// change messages
													hr = S_OK;
												}
											}
											else
											{
												// No next band - do update
												hr = S_OK;
											}
										}
									}
								}
								pSegment->Release();
							}
						}
						else
						{
							// Found the band - do update
							hr = S_OK;
						}
					}
				}

				if( fFound )
				{
					if( !m_fNoUpdateSegment )
					{
						// Set the undo string
						if( m_pBandStrip )
						{
							m_pBandStrip->m_nLastEdit = IDS_UNDO_BANDCHANGE;
						}

						// Get state of TP_FREEZE_UNDO
						BOOL fFreezeUndo = FALSE;
						VARIANT var;
						if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
						{
							fFreezeUndo = V_BOOL(&var);
						}

						// If TP_FREEZE_UNDO is set, set it to FALSE
						if( fFreezeUndo )
						{
							// Need to clear TP_FREEZE_UNDO or the segment will not add an undo state for us
							var.vt = VT_BOOL;
							V_BOOL(&var) = FALSE;
							m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
						}

						// Let our hosting editor know about the changes
						m_pTimeline->OnDataChanged( (IBandMgr*)this );

						if( fFreezeUndo )
						{
							// Need to set TP_FREEZE_UNDO back to its original setting
							var.vt = VT_BOOL;
							V_BOOL(&var) = TRUE;
							m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
						}
					}

					// Sequence strip will need to repaint FBar to catch latest DLS region wave names
					m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_dwGroupBits, NULL );

					SyncWithDirectMusic();
					m_fDirty = TRUE;
				}
			}
		}

		if( pINode )
		{
			pINode->Release();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CBandMgr::GetStripMgrProperty(
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

			pioTrackHeader->guidClassID = CLSID_DirectMusicBandTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = NULL;
			pioTrackHeader->fccType = DMUS_FOURCC_BANDTRACK_FORM;
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
// CBandMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CBandMgr::SetStripMgrProperty(
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
			if( V_UNKNOWN( &variant ) == NULL )
			{
				if( m_pDMProdFramework )
				{
					// Editor is closing, so close all open Band windows
					POSITION pos = m_lstBands.GetHeadPosition();
					while( pos )
					{
						CBandItem* pBandItem = m_lstBands.GetNext( pos );

						if( pBandItem->m_pIBandNode )
						{
							m_pDMProdFramework->CloseEditor( pBandItem->m_pIBandNode );
						}
					}
				}
			}

			m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->Release();
				m_pPropPageMgr = NULL;
			}
			if ( m_pBandStrip )
			{
				if (!V_UNKNOWN( &variant ) && m_pBandStrip->m_pStripNode) // the strip is being removed, so release its strip node.
				{
					m_pBandStrip->m_pStripNode->Release();
					m_pBandStrip->m_pStripNode = NULL;
				}
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)m_pBandStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pBandStrip );
			}
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_NewStyleSelected, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Start, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Stop, m_dwOldGroupBits );
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
				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pBandStrip, CLSID_DirectMusicBandTrack, m_dwGroupBits, 0 );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_FrameworkMsg, m_dwGroupBits);
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_NewStyleSelected, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Start, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Stop, m_dwGroupBits );

				// Set notifications for all Bands
				POSITION pos = m_lstBands.GetHeadPosition();
				while( pos )
				{
					CBandItem* pBandItem = m_lstBands.GetNext( pos );

					if( m_pBandStrip->m_pStripNode
					&&  pBandItem->m_pIBandNode )
					{
						m_pDMProdFramework->AddToNotifyList(pBandItem->m_pIBandNode, m_pBandStrip->m_pStripNode);
					}
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
		if( V_UNKNOWN( &variant )
		&&	SUCCEEDED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pDMProdFramework) )
		&&	(m_pDMProdFramework != NULL) )
		{
			IDMUSProdComponent* pIComponent;
			if( SUCCEEDED( m_pDMProdFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ) )
			{
				IDMUSProdConductor *pIConductor;
				if( SUCCEEDED( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pIConductor ) ) )
				{
					IUnknown *punkPerformance;
					if( SUCCEEDED( pIConductor->GetPerformanceEngine( &punkPerformance ) ) )
					{
						punkPerformance->QueryInterface( IID_IDirectMusicPerformance, (void**)&m_pIPerformance );
						punkPerformance->Release();
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
// CBandMgr IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::GetClassID

HRESULT CBandMgr::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pClsId == NULL )
	{
		return E_POINTER;
	}

	// return our CLSID
	memcpy( pClsId, &CLSID_BandMgr, sizeof( CLSID ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::IsDirty

HRESULT CBandMgr::IsDirty()
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
// CBandMgr::Load

HRESULT CBandMgr::Load( IStream* pIStream )
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

	// Remove all existing bands
	EmptyBandList();

	MMCKINFO ckMain;

	ckMain.fccType = DMUS_FOURCC_BANDTRACK_FORM;
	hr = pIRiffStream->Descend(&ckMain, NULL, MMIO_FINDRIFF);

	if(FAILED(hr))
	{
		pIRiffStream->Release();
		return hr;
	}
	
//	DWORD cbRead;
//	DWORD cbSize;
	
	MMCKINFO ckNext;
	
	ckNext.ckid = 0;
	ckNext.fccType = 0;
		
	while(pIRiffStream->Descend(&ckNext, &ckMain, 0) == 0)
	{
		switch(ckNext.ckid)
		{
			// For now, just ignore this chunk
//			case DMUS_FOURCC_BANDTRACK_CHUNK:
//				DMUS_IO_BAND_TRACK_HEADER ioDMBndTrkHdr;
//				cbSize = min(sizeof(DMUS_IO_BAND_TRACK_HEADER), ckNext.cksize);
//				hr = pIStream->Read(&ioDMBndTrkHdr, cbSize, &cbRead);
//				if(SUCCEEDED(hr) && cbRead == cbSize)
//				{
//					m_bAutoDownload = ioDMBndTrkHdr.bAutoDownload ? true : false;
//
//				}
//				else
//				{
//					hr = E_FAIL;
//				}
//				break;

			case FOURCC_LIST:
				switch(ckNext.fccType)
				{
					case  DMUS_FOURCC_BANDS_LIST:
						BOOL fChanged;
						hr = BuildDirectMusicBandList(pIStream, pIRiffStream, ckNext, fChanged);
						break;

					default:
						break;
				}
			default:
				break;

		}
    
		if(SUCCEEDED(hr) && pIRiffStream->Ascend(&ckNext, 0) == 0)
		{
			ckNext.ckid = 0;
			ckNext.fccType = 0;
		}
	}

	pIRiffStream->Release();
	
	if (SUCCEEDED( hr) )
	{
		hr = SyncWithDirectMusic();
	}
	return hr;
}

//////////////////////////////////////////////////////////////////////
// CBandMgr::BuildDirectMusicBandList

HRESULT CBandMgr::BuildDirectMusicBandList(IStream* pIStream,
										   IDMUSProdRIFFStream *pIDirectMusicStream,
										   MMCKINFO& ckParent,
										   BOOL& fChanged,
										   bool fPaste,
										   MUSIC_TIME mtPasteTime)
{
	ASSERT(pIDirectMusicStream);
	ASSERT(pIStream);
	HRESULT hr;

	MMCKINFO ckMain;
	MMCKINFO ck;

	if( fPaste )
	{
		DWORD dwCurrentFilePos = StreamTell( pIStream );

		MUSIC_TIME mtAdjustment = 0;

		ASSERT( m_pTimeline != NULL );
		if( m_pTimeline )
		{
			// Get Timeline length
			VARIANT var;
			m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
			MUSIC_TIME mtMaxTimelineLength = V_I4( &var );
			mtMaxTimelineLength--;

			// Get start clock for the last beat in the timeline;
			long lMeasure;
			long lBeat;
			m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtMaxTimelineLength, &lMeasure, &lBeat );
			m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &mtMaxTimelineLength );
	
			ckMain.ckid = 0;
			ckMain.fccType = 0;

			// Determine new paste time to enforce boundaries
			while ( pIDirectMusicStream->Descend( &ckMain, &ckParent, 0 ) == 0  )
			{
				if( ckMain.ckid == FOURCC_LIST
				&&  ckMain.fccType == DMUS_FOURCC_BAND_LIST )
				{
					while( pIDirectMusicStream->Descend( &ck, &ckMain, 0 ) == 0 )
					{
						switch( ck.ckid )
						{
							case DMUS_FOURCC_BANDITEM_CHUNK2:
							{
								DMUS_IO_BAND_ITEM_HEADER2 ioDMBndItemHdr2;
								DWORD cbRead;
								DWORD cbSize;
								
								cbSize = min( sizeof(DMUS_IO_BAND_ITEM_HEADER2), ck.cksize );
								hr = pIStream->Read( &ioDMBndItemHdr2, cbSize, &cbRead );
								if( FAILED(hr) || cbRead != cbSize ) 
								{
									pIDirectMusicStream->Ascend( &ck, 0 );
									goto ON_END;
								}

								MUSIC_TIME mtTime = ioDMBndItemHdr2.lBandTimePhysical;

								// Adjust mtTime so that Tick offset remains unchanged after the paste
								if( mtTime < 0 )
								{
									long lClocksPerBeat;

									m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, 0, 1, &lClocksPerBeat );
									MUSIC_TIME mtMod = mtTime % lClocksPerBeat;
									mtTime = mtTime - mtMod;
									if( mtMod <= -(lClocksPerBeat >> 1) )
									{
										mtTime -= lClocksPerBeat;
									}
								}
								else
								{
									m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
									m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &mtTime );
								}

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
										mtAdjustment = mtTime;
									}
								}
							}
							break;

							case DMUS_FOURCC_BANDITEM_CHUNK:
							{
								DMUS_IO_BAND_ITEM_HEADER ioDMBndItemHdr;
								DWORD cbRead;
								DWORD cbSize;
								
								cbSize = min( sizeof(DMUS_IO_BAND_ITEM_HEADER), ck.cksize );
								hr = pIStream->Read( &ioDMBndItemHdr, cbSize, &cbRead );
								if( FAILED(hr) || cbRead != cbSize ) 
								{
									pIDirectMusicStream->Ascend( &ck, 0 );
									goto ON_END;
								}

								MUSIC_TIME mtTime = ioDMBndItemHdr.lBandTime;

								// Adjust mtTime so that Tick offset remains unchanged after the paste
								if( mtTime < 0 )
								{
									long lClocksPerBeat;

									m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, 0, 1, &lClocksPerBeat );
									MUSIC_TIME mtMod = mtTime % lClocksPerBeat;
									mtTime = mtTime - mtMod;
									if( mtMod <= -(lClocksPerBeat >> 1) )
									{
										mtTime -= lClocksPerBeat;
									}
								}
								else
								{
									m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
									m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &mtTime );
								}

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
										mtAdjustment = mtTime;
									}
								}
							}
							break;
						}

						pIDirectMusicStream->Ascend( &ck, 0 );
					}

					pIDirectMusicStream->Ascend( &ckMain, 0 );
				}

				pIDirectMusicStream->Ascend( &ckMain, 0 );
			}
		}

		// New paste time which will enforce strip boundaries
		mtPasteTime += mtAdjustment;

		// Restore our position back to the start of the Bands
		StreamSeek( pIStream, dwCurrentFilePos, STREAM_SEEK_SET );
	}

	ckMain.ckid = 0;
	ckMain.fccType = 0;
    
	while( pIDirectMusicStream->Descend(&ckMain, &ckParent, 0) == 0 )
	{
		switch(ckMain.ckid)
		{
			case FOURCC_LIST :
				switch(ckMain.fccType)
				{
					case DMUS_FOURCC_BAND_LIST:
						hr = ExtractBand( pIStream, pIDirectMusicStream, ckMain, fPaste, mtPasteTime );
						if( SUCCEEDED( hr ) )
						{
							fChanged = TRUE;
						}
						break;
					
					default:
						break;

				}
				break;

			default:
				break;

		}
    
		if(SUCCEEDED(hr) && pIDirectMusicStream->Ascend(&ckMain, 0) == 0)
		{
			ckMain.ckid = 0;
			ckMain.fccType = 0;
		}
	}

ON_END:
	return hr;
}


//////////////////////////////////////////////////////////////////////
// CBandMgr::ExtractBand

HRESULT CBandMgr::ExtractBand(IStream* pStream, 
								 IDMUSProdRIFFStream *pIDirectMusicStream, 
								 MMCKINFO& ckParent,
								 bool fPaste,
								 MUSIC_TIME mtPasteTime)
{
	ASSERT(pIDirectMusicStream);
	ASSERT(pStream);
	
	MMCKINFO ckNext;
	ckNext.ckid = 0;
	ckNext.fccType = 0;
	
	DWORD cbRead;    
	DWORD cbSize;

	BOOL fInsertedItem = FALSE; // Set to true when the pItem is inserted in the band list

	CBandItem *pItem = new CBandItem(m_pBandStrip);
	if(pItem == NULL)
	{
		return E_OUTOFMEMORY;
	}
	pItem->m_strText.Empty();

	HRESULT	hr;
	long lMeasure = 0;
	long lBeat = 0;
	long lTick = 0;

	while(pIDirectMusicStream->Descend(&ckNext, &ckParent, 0) == 0)
	{
		switch(ckNext.ckid)
		{
			case DMUS_FOURCC_BANDITEM_CHUNK2:
				DMUS_IO_BAND_ITEM_HEADER2 ioDMBndItemHdr2;
				cbSize = min(sizeof(DMUS_IO_BAND_ITEM_HEADER2), ckNext.cksize);
				hr = pStream->Read(&ioDMBndItemHdr2, cbSize, &cbRead);
				if(SUCCEEDED(hr) && cbRead == cbSize)
				{
					pItem->m_mtTimePhysical = ioDMBndItemHdr2.lBandTimePhysical;
					pItem->m_mtTimeLogical = ioDMBndItemHdr2.lBandTimeLogical;
					if( fPaste )
					{
						pItem->SetTimePhysical( (pItem->m_mtTimePhysical + mtPasteTime), STP_LOGICAL_FROM_BEAT_OFFSET );
						if( pItem->m_mtTimePhysical < -MAX_TICK )
						{
							CleanUpPropBand( pItem );
							delete pItem;
							return S_OK;
						}
					}
					else
					{
						pItem->SetTimePhysical( ioDMBndItemHdr2.lBandTimePhysical, STP_LOGICAL_RECALC_MEASURE_BEAT );
					}
				}
				else if(SUCCEEDED(hr))
				{
					hr = E_FAIL;
				}
				break;
			
			case DMUS_FOURCC_BANDITEM_CHUNK:
				ASSERT( fPaste == FALSE );	// Paste should always use DMUS_FOURCC_BANDITEM_CHUNK2
				DMUS_IO_BAND_ITEM_HEADER ioDMBndItemHdr;
				cbSize = min(sizeof(DMUS_IO_BAND_ITEM_HEADER), ckNext.cksize);
				hr = pStream->Read(&ioDMBndItemHdr, cbSize, &cbRead);
				if(SUCCEEDED(hr) && cbRead == cbSize)
				{
					pItem->m_mtTimePhysical = ioDMBndItemHdr.lBandTime;
					pItem->m_mtTimeLogical = ioDMBndItemHdr.lBandTime;
				}
				else if(SUCCEEDED(hr))
				{
					hr = E_FAIL;
				}
				break;
			
			case FOURCC_RIFF:
				switch(ckNext.fccType)
				{
					case DMUS_FOURCC_BAND_FORM:
						LARGE_INTEGER li;
						ULARGE_INTEGER ul;

						li.HighPart = 0;
						li.LowPart = 0;

						hr = pStream->Seek(li, STREAM_SEEK_CUR, &ul);

						if(SUCCEEDED(hr))
						{
							li.HighPart = 0;
							li.LowPart = ul.LowPart - sizeof(FOURCC) * 2 - sizeof(DWORD);

							hr = pStream->Seek(li, STREAM_SEEK_SET, &ul);
						}
						
						if(SUCCEEDED(hr))
						{
							IDMUSProdComponent* pIComponent = NULL;
							hr = m_pDMProdFramework->FindComponent(CLSID_BandComponent, &pIComponent);
							if(SUCCEEDED(hr))
							{
								IDMUSProdRIFFExt* pRiff = NULL;
								hr = pIComponent->QueryInterface(IID_IDMUSProdRIFFExt, (void**)&pRiff);
								if( SUCCEEDED( hr ) )
								{
									IDMUSProdNode* pNode = NULL;
									hr = pRiff->LoadRIFFChunk(pStream, &pNode);
									pRiff->Release();
									if (SUCCEEDED(hr))
									{
										pItem->m_pIBandNode = pNode;
										// BUGBUG error checking; cleanup!
										BSTR bstrName;
										pItem->m_pIBandNode->GetNodeName( &bstrName );
										pItem->m_strText = bstrName;
										if( fPaste )
										{
											pItem->SetSelectFlag( TRUE );
										}
										if (m_pBandStrip->m_pStripNode)
										{
											m_pDMProdFramework->AddToNotifyList(pItem->m_pIBandNode, m_pBandStrip->m_pStripNode);
										}
										ASSERT( !fInsertedItem );
										InsertByAscendingTime( pItem, fPaste );
										fInsertedItem = TRUE;
										SysFreeString(bstrName);
									}
								}
								pIComponent->Release();
							}

						}					
						break;

					default:
						break;
				}
				break;
			default:
				break;

		}
    
		if( SUCCEEDED(hr) && pIDirectMusicStream->Ascend(&ckNext, 0) == 0 )
		{
			ckNext.ckid = 0;
			ckNext.fccType = 0;
		}
	}

	if( !fInsertedItem )
	{
		delete pItem;
		return E_FAIL;
	}
	else
	{
		return S_OK;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::Save

HRESULT CBandMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo	StreamInfo;//Added ECW 4/24/98
	FileType ftFileType = FT_DESIGN;	// Default to FT_DESIGN so clipboard
										// gets proper DLS file ref chunk
										// when Band is saved
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
	// stream that will be loaded into a DirectMusicBandTrack (GUID_DirectMusicObject)
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

	// If the Band list isn't empty, save it
	//if ( !m_lstBands.IsEmpty() )
	{
		// Create a RIFF chunk to store the Band data
		ckMain.fccType = DMUS_FOURCC_BANDTRACK_FORM;
		if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
		SaveBandWrapper(pIStream, pIRiffStream);

		// Ascend out of the Band RIFF chunk.
		pIRiffStream->Ascend( &ckMain, 0 );
	}
	m_fDirty = FALSE;

ON_ERROR:
	pIRiffStream->Release();
    return hr;
}

HRESULT CBandMgr::SaveBandWrapper( IStream* pIStream, IDMUSProdRIFFStream* pIRiffStream )
{
    MMCKINFO ckList;
	HRESULT hr = S_OK;
//	DWORD				dwByteCount;

	// For now, do not write this chunk
	//if (m_bAutoDownload)
//	{
//		// Band track header
//		MMCKINFO ckTrackHeader;
//		ckTrackHeader.ckid = DMUS_FOURCC_BANDTRACK_CHUNK;
//		if( FAILED( pIRiffStream->CreateChunk( &ckTrackHeader, 0 ) ) )
//		{
//			hr = E_FAIL;
//			goto ON_ERR;
//		}
//
//		DMUS_IO_BAND_TRACK_HEADER oBandTrackHeader;
//		oBandTrackHeader.bAutoDownload = m_bAutoDownload ? TRUE : FALSE;
//
//		hr = pIStream->Write( &oBandTrackHeader, sizeof(oBandTrackHeader), &dwByteCount );
//		if( FAILED(hr) || dwByteCount != sizeof(oBandTrackHeader) )
//		{
//			hr = E_FAIL;
//			goto ON_ERR;
//		}
//
//		if( pIRiffStream->Ascend( &ckTrackHeader, 0 ) != 0 )
//		{
//			hr = E_FAIL;
//			goto ON_ERR;
//		}
//	}
	//else
	if ( !m_lstBands.IsEmpty() )
	{
		// Create a LIST chunk to wrap the Bands
		ckList.fccType = DMUS_FOURCC_BANDS_LIST;
		if( pIRiffStream->CreateChunk( &ckList, MMIO_CREATELIST ) != 0 )
		{
			return E_FAIL;
		}

		POSITION pos = m_lstBands.GetHeadPosition();
		while( pos )
		{
			CBandItem* pBandItem;
			pBandItem = m_lstBands.GetNext( pos );
			ASSERT( pBandItem );
			if( pBandItem && pBandItem->m_pIBandNode )
			{
				// Store each Band in its own chunk.
				SaveBand( pIStream, pIRiffStream, pBandItem, 0, 0 );
			}
		}

		// Ascend out of the Band LIST chunk.
		pIRiffStream->Ascend( &ckList, 0 );
	}

//ON_ERR:
    return hr;
}

HRESULT CBandMgr::SaveBand( IStream* pIStream, IDMUSProdRIFFStream* pIRiffStream,
						    CBandItem* pBandItem, MUSIC_TIME mtOffset, short nReason )
{
    MMCKINFO ckList;
	HRESULT hr = S_OK;

	// Create a LIST chunk for the Band
	ckList.fccType = DMUS_FOURCC_BAND_LIST;
	if( pIRiffStream->CreateChunk( &ckList, MMIO_CREATELIST ) != 0 )
	{
		return E_FAIL;
	}

	// Write 'bd2h-ck'
	MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_BANDITEM_CHUNK2;
	if( FAILED( pIRiffStream->CreateChunk( &ck, 0 ) ) )
	{
		return E_FAIL;
	}
	{
		DWORD dwByteCount;
		DMUS_IO_BAND_ITEM_HEADER2 header2;

		header2.lBandTimePhysical = pBandItem->m_mtTimePhysical - mtOffset;

		if( nReason == SSB_COPY
		||  nReason == SSB_DRAG )
		{
			long lMeasure, lBeat, lTick;

			// Snap physical time to number of beats
			long lPhysicalBeats;
			ClocksToMeasureBeatTick( pBandItem->m_mtTimePhysical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pTimeline, m_dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Convert logical time to number of beats
			long lLogicalBeats;
			ClocksToMeasureBeatTick( pBandItem->m_mtTimeLogical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pTimeline, m_dwGroupBits, 0, lMeasure, lBeat, lLogicalBeats );

			// Use lBandTimeLogical to store beat difference between physical and logical times
			header2.lBandTimeLogical = lLogicalBeats - lPhysicalBeats;
		}
		else
		{
			header2.lBandTimeLogical = pBandItem->m_mtTimeLogical;
		}

		hr = pIStream->Write(&header2, sizeof(header2), &dwByteCount);
		if(FAILED(hr) || dwByteCount != sizeof(header2))
		{
			return E_FAIL;
		}

		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
			return E_FAIL;
		}

		// now I need to save the band; I should be able to call the band node's save method.
		IPersistStream* pStream = NULL;
		hr = pBandItem->m_pIBandNode->QueryInterface(IID_IPersistStream, (void**)&pStream);
		if (SUCCEEDED(hr))
		{
			hr = pStream->Save(pIStream, FALSE); // BUGBUG is FALSE ok here?
			pStream->Release();
		}
	}

	// Ascend out of the Band LIST chunk.
	pIRiffStream->Ascend( &ckList, 0 );

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::CleanUpPropBand

void CBandMgr::CleanUpPropBand( CPropBand* pPropBand )
{
	ASSERT( pPropBand != NULL );

	// Close Band editor
	if( pPropBand->m_pIBandNode )
	{
		if( m_pDMProdFramework  )
		{
			m_pDMProdFramework->CloseEditor( pPropBand->m_pIBandNode );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::CreateBand

HRESULT CBandMgr::CreateBand( DWORD dwMeasure, BYTE bBeat, CBandStrip* pBandStrip, CBandItem*& rpBand )
{

	rpBand = NULL;
	HRESULT hr = S_OK;

	if (!m_pTimeline) return E_FAIL;

	rpBand = new CBandItem(pBandStrip);
	if(rpBand == NULL)
	{
		hr =  E_OUTOFMEMORY;
	}
	else
	{
		long lClocks;
		MeasureBeatTickToClocks( dwMeasure, bBeat, 0, &lClocks );
		rpBand->SetTimePhysical( lClocks, STP_LOGICAL_SET_DEFAULT );
		rpBand->m_strText.Empty();
		rpBand->m_strText = "";
		rpBand->m_pIBandNode = NULL;
		AddBand(rpBand);
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::CreateBand

HRESULT CBandMgr::CreateBand( long lXPos, CBandStrip* pBandStrip, CBandItem*& rpBand )
{

	rpBand = NULL;
	HRESULT hr = S_OK;

	if (!m_pTimeline) return E_FAIL;

	rpBand = new CBandItem(pBandStrip);
	if(rpBand == NULL)
	{
		hr =  E_OUTOFMEMORY;
	}
	else
	{
		long lMeasure = 0;
		long lBeat = 0;
		hr = m_pTimeline->PositionToMeasureBeat( m_dwGroupBits, 0, lXPos, &lMeasure, &lBeat );
		ASSERT(SUCCEEDED(hr));

		long lClocks;
		MeasureBeatTickToClocks( lMeasure, lBeat, 0, &lClocks );
		rpBand->SetTimePhysical( lClocks, STP_LOGICAL_SET_DEFAULT );
		rpBand->m_strText.Empty();
		rpBand->m_strText = "";
		rpBand->m_pIBandNode = NULL;
		AddBand(rpBand);
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::GetSizeMax

HRESULT CBandMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	// Compute size of stream needed to persist ourself into.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::GetData

// This method is called by CBandPropPageMgr to get data to send to the
// Band property page.
// The CBandStrip::GetData() method is called by CBandStripPropPageMgr
// to get the strip's properties (Group Bits, etc.)
HRESULT STDMETHODCALLTYPE CBandMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected band.
	BOOL fMultipleSelect = FALSE;
	CBandItem* pFirstBandItem = NULL;

	CBandItem* pBandItem;
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		pBandItem = m_lstBands.GetNext( pos );
		if( pBandItem->m_fSelected
		&&  pBandItem->m_strText.IsEmpty() == FALSE )
		{
			pFirstBandItem = pBandItem;
			while( pos )
			{
				pBandItem = m_lstBands.GetNext( pos );
				if( pBandItem->m_fSelected
				&&  pBandItem->m_strText.IsEmpty() == FALSE )
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

	// Multiple Bands selected
	if( fMultipleSelect )
	{
		CPropBand* pPropBand = new CPropBand;
		if( pPropBand )
		{
			pPropBand->m_dwMeasure = 0xFFFFFFFF;		// Signifies multiple Bands selected
			*ppData = pPropBand;
			hr = S_OK;
		}
	}

	// One Band selected
	else if( pFirstBandItem )
	{
		CPropBand* pPropBand = new CPropBand( pFirstBandItem );
		if( pPropBand )
		{
			*ppData = pPropBand;
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
// CBandMgr::ForceBoundaries

HRESULT CBandMgr::ForceBoundaries( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmtTime )
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
// CBandMgr::GetGroupBits

DWORD CBandMgr::GetGroupBits( void )
{
	return m_dwGroupBits;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::IsRefTimeTrack

bool CBandMgr::IsRefTimeTrack( void )
{
	return (m_dwTrackExtrasFlags & DMUS_TRACKCONFIG_PLAY_CLOCKTIME) ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::SetData

// This method is called by CBandPropPageMgr in response to user actions
// in the Band Property page.  It changes the currenly selected Band. 
HRESULT STDMETHODCALLTYPE CBandMgr::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected band.
	CBandItem* pBandItem;
	pBandItem = FirstSelectedRealBand();

	if( pBandItem )
	{
		CPropBand *pPropBand;
		pPropBand = new CPropBand( pBandItem );

		if( pPropBand )
		{
			CPropBand* pBand = (CPropBand*)pData;

			bool fChanged = false;

			// m_mtTimePhysical
			if( pBand->m_dwMeasure != pBandItem->m_dwMeasure
			||  pBand->m_bBeat != pBandItem->m_bBeat
			||  pBand->m_lTick != pBandItem->m_lTick )
			{
				char chBeat = pBand->m_bBeat;	// May contain negative number
		
				MUSIC_TIME mtNewTimePhysical;
				if( SUCCEEDED ( ForceBoundaries( pBand->m_dwMeasure, chBeat, pBand->m_lTick, &mtNewTimePhysical ) ) )
				{
					if( mtNewTimePhysical != pBandItem->m_mtTimePhysical )
					{
						m_pBandStrip->m_nLastEdit = IDS_UNDO_MOVE;
						pBandItem->SetTimePhysical( mtNewTimePhysical, STP_LOGICAL_ADJUST );

						POSITION pos = m_lstBands.Find( pBandItem );
						ASSERT( pos != NULL );
						if( pos )
						{
							m_lstBands.RemoveAt( pos );
							InsertByAscendingTime( pBandItem, FALSE );
						}

						fChanged = true;
					}
				}
			}

			// m_mtTimeLogical
			else if( pBand->m_lLogicalMeasure != pBandItem->m_lLogicalMeasure
				 ||  pBand->m_lLogicalBeat != pBandItem->m_lLogicalBeat )
			{
				MUSIC_TIME mtNewTimeLogical;
				if( SUCCEEDED ( ForceBoundaries( pBand->m_lLogicalMeasure, pBand->m_lLogicalBeat, 0, &mtNewTimeLogical ) ) )
				{
					if( mtNewTimeLogical != pBandItem->m_mtTimeLogical )
					{
						m_pBandStrip->m_nLastEdit = IDS_UNDO_LOGICAL_TIME;
						pBandItem->SetTimeLogical( mtNewTimeLogical );

						fChanged = true;
					}
				}
			}

			// m_strName
			else if( pBand->m_strText.Compare( pBandItem->m_strText ) != 0 )
			{
				m_pBandStrip->m_nLastEdit = IDS_UNDO_BANDNAME;
				pBand->ApplyToBandItem( pBandItem );

				if( pBandItem->m_pIBandNode )
				{
					BSTR bstrBandName = pBandItem->m_strText.AllocSysString();
					pBandItem->m_pIBandNode->SetNodeName( bstrBandName );
					m_pDMProdFramework->RefreshNode( pBandItem->m_pIBandNode );
				}

				fChanged = true;
			}

			if( fChanged )
			{
				// Redraw the band strip
				m_pTimeline->StripInvalidateRect( m_pBandStrip, NULL, TRUE );

				// Let our hosting editor know about the changes
				m_pTimeline->OnDataChanged( (IBandMgr*)this );

				// Sequence strip will need to repaint FBar to catch latest DLS region wave names
				m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_dwGroupBits, NULL );

				// Update the property page
				if( m_pPropPageMgr )
				{
					m_pPropPageMgr->RefreshData();
				}

				SyncWithDirectMusic();
			}

			// pPropBand is a temp work field that has pointer
			// to the exact same m_pIBandNode interface as pBandItem.
			// Clean up pPropBand->m_pIBandNode now so that
			// CleanUpPropBand() does not close the Band editor
			if( pPropBand->m_pIBandNode )
			{
				pPropBand->m_pIBandNode->Release();
				pPropBand->m_pIBandNode = NULL;
			}
			CleanUpPropBand( pPropBand );
			delete pPropBand;
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


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CBandMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		CBandPropPageMgr* pPPM = new CBandPropPageMgr( this );
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
// CBandMgr IDMUSProdBandMgrEdit implementation

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::DisplayEditBandButton

HRESULT CBandMgr::DisplayEditBandButton( DWORD dwPChannel, LONG lXPos, 
									   LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTimeline && m_pBandStrip )
	{
		// Get 'now'
		MUSIC_TIME mtNow;
		if( FAILED( m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtNow ) ) )
		{
			return E_UNEXPECTED;
		}

		// Load the menu
		HMENU hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_BANDMENU) );
		HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

		// Get the band item to modify
		m_pBandItemForEditBandButton = NULL;
		m_dwPChannelForEditBandButton = dwPChannel;
		GetBandForPChannelAndTime( m_lstBands, dwPChannel, mtNow, &m_pBandItemForEditBandButton, NULL );

		// Enable the "New Instrument" item if this is true
		bool fEnableNewInstrument = true;

		// Look for a band at exactly tick -1 of the current beat
		long lMeasure, lBeat, lTmpTime;
		if( m_pBandItemForEditBandButton
		&&	SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtNow, &lMeasure, &lBeat ) )
		&&	SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &lTmpTime ) ) )
		{
			lTmpTime--;

			// If we found a band that's after tick -1 of this beat, but before mtNow
			if( m_pBandItemForEditBandButton
			&&	m_pBandItemForEditBandButton->m_mtTimePhysical > lTmpTime )
			{
				// Disable the 'Insert New Instrument' option
				fEnableNewInstrument = false;
			}
		}

		::EnableMenuItem( hMenuPopup, ID_OPEN_BANDEDITOR, m_pBandItemForEditBandButton ? MF_ENABLED : MF_GRAYED );
		::EnableMenuItem( hMenuPopup, ID_LOCATE_BAND, m_pBandItemForEditBandButton ? MF_ENABLED : MF_GRAYED );
		::EnableMenuItem( hMenuPopup, ID_INSERT_NEWINSTRUMENT, fEnableNewInstrument ? MF_ENABLED : MF_GRAYED );

		// Display the menu
		m_pTimeline->TrackPopupMenu( hMenuPopup, lXPos, lYPos, m_pBandStrip, FALSE );

		// Cleanup
		::DestroyMenu( hMenu );

		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::AddBand

HRESULT STDMETHODCALLTYPE CBandMgr::AddBand( CBandItem* pBand )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	InsertByAscendingTime(pBand, FALSE);
	hr = SyncWithDirectMusic();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::SyncWithDirectMusic

HRESULT CBandMgr::SyncWithDirectMusic(  )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// 1. persist the strip as a band track to a stream
	IStream* pIMemStream = NULL;
	IPersistStream* pIPersistStream = NULL;
	hr = m_pDMProdFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );
	if( SUCCEEDED ( hr ) )
	{
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

					if( m_pDMProdSegmentNode )
					{
						IDirectMusicSegment *pSegment;
						if( SUCCEEDED( m_pDMProdSegmentNode->GetObject(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pSegment ) ) )
						{
							m_pIDMTrack->Init( pSegment );
							pSegment->Release();
						}
					}

					pIPersistStream->Release();
				}
			}
		}
		pIMemStream->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CBandMgr::OnRemoveFromPageManager( void)
{
	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::OnDataChanged

HRESULT STDMETHODCALLTYPE CBandMgr::OnDataChanged( void)
{
	ASSERT( m_pTimeline );
	if ( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Let our hosting editor know about the change
	m_pTimeline->OnDataChanged( (IBandMgr*)this );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::DeleteSelectedBands

HRESULT CBandMgr::DeleteSelectedBands()
{
	CBandItem* pBandItem;
	POSITION pos2, pos1 = m_lstBands.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		pBandItem = m_lstBands.GetNext( pos1 );
		ASSERT( pBandItem );
		if ( pBandItem )
		{
			if ( pBandItem->m_fSelected )
			{
				m_lstBands.RemoveAt( pos2 );
				CleanUpPropBand( pBandItem );
				delete pBandItem;
			}
		}
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::SaveSelectedBands

HRESULT CBandMgr::SaveSelectedBands(LPSTREAM pIStream, MUSIC_TIME mtOffset, short nReason)
{
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// If the Band list has anything in it, look for selected Bands
	if ( !m_lstBands.IsEmpty() )
	{
		POSITION pos;
		MMCKINFO ckMain;

		// Create a LIST chunk to wrap the Bands
		ckMain.fccType = DMUS_FOURCC_BANDS_LIST;
		if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}


		pos = m_lstBands.GetHeadPosition();
		while( pos )
		{
			CBandItem* pBandItem;
			pBandItem = m_lstBands.GetNext( pos );
			ASSERT( pBandItem );
			if( pBandItem
			&&  pBandItem->m_pIBandNode
			&&  pBandItem->m_fSelected )
			{
				// Store each Band in its own chunk.
				hr = SaveBand( pIStream, pIRiffStream, pBandItem, mtOffset, nReason );
				if( FAILED ( hr ) )
				{
					goto ON_ERROR;
				}
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
// CBandMgr::MarkSelectedBands

// marks m_dwUndermined field CBandItems in list
void CBandMgr::MarkSelectedBands( DWORD dwFlags )
{
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem;
		pBandItem = m_lstBands.GetNext( pos );
		ASSERT( pBandItem );
		if ( pBandItem )
		{
			if ( pBandItem->m_fSelected )
			{
				pBandItem->m_dwBits |= dwFlags;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::DeleteMarked

// deletes bands marked by given flag
void CBandMgr::DeleteMarked( DWORD dwFlags )
{
	CBandItem* pBandItem;
	POSITION pos2, pos1 = m_lstBands.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		pBandItem = m_lstBands.GetNext( pos1 );
		ASSERT( pBandItem );
		if ( pBandItem )
		{
			if ( (pBandItem->m_dwBits & dwFlags) || !pBandItem->m_pIBandNode )
			{
				m_lstBands.RemoveAt( pos2 );
				CleanUpPropBand( pBandItem );
				delete pBandItem;
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
// CBandMgr::UnMarkBands

// unmarks flag m_dwUndermined field CBandItems in list
void CBandMgr::UnMarkBands( DWORD dwFlags )
{
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem;
		pBandItem = m_lstBands.GetNext( pos );
		ASSERT( pBandItem );
		if ( pBandItem )
		{
			pBandItem->m_dwBits &= ~dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::UnselectAllKeepBits

void CBandMgr::UnselectAllKeepBits()
{
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem = m_lstBands.GetNext( pos );

		ASSERT( pBandItem );
		if( pBandItem )
		{
			pBandItem->m_fSelected = FALSE;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::UnselectAll

void CBandMgr::UnselectAll()
{
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem = m_lstBands.GetNext( pos );

		ASSERT( pBandItem );
		if ( pBandItem )
		{
			pBandItem->SetSelectFlag( FALSE );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::SelectAll

void CBandMgr::SelectAll()
{
	// UnselectAll to ensure only real bands are selected
	UnselectAll();
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem = m_lstBands.GetNext( pos );

		ASSERT( pBandItem );
		if ( pBandItem && pBandItem->m_pIBandNode )
		{
			pBandItem->SetSelectFlag( TRUE );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::IsSelected()

BOOL CBandMgr::IsSelected()
{
	// If anything real is selected, return TRUE.
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem = m_lstBands.GetNext( pos );

		ASSERT( pBandItem );
		if ( pBandItem )
		{
			if ( pBandItem->m_pIBandNode && pBandItem->m_fSelected )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::FirstSelectedBand

CBandItem* CBandMgr::FirstSelectedBand()
{
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem;
		pBandItem = m_lstBands.GetNext( pos );
		ASSERT( pBandItem );
		if ( pBandItem )
		{
			if ( pBandItem->m_fSelected )
			{
				return pBandItem;
			}
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::FirstSelectedRealBand

CBandItem* CBandMgr::FirstSelectedRealBand()
{
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem = m_lstBands.GetNext( pos );

		if( pBandItem->m_fSelected
		&&  pBandItem->m_pIBandNode )
		{
			return pBandItem;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CBandMgr::CurrentlySelectedBand

CBandItem* CBandMgr::CurrentlySelectedBand()
{
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem;
		pBandItem = m_lstBands.GetNext( pos );
		ASSERT( pBandItem );
		if ( pBandItem )
		{
			if ( pBandItem->m_fSelected  && (pBandItem->m_dwBits & UD_CURRENTSELECTION))
			{
				return pBandItem;
			}
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::FindBand

CBandItem* CBandMgr::FindBand( long lMeasure, long lBeat ) const
{
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem = m_lstBands.GetNext( pos );

		if( pBandItem->m_dwMeasure == (DWORD)lMeasure
		&&  pBandItem->m_bBeat == (unsigned)lBeat )
		{
			return pBandItem;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::FindBandAtTime

CBandItem* CBandMgr::FindBandAtTime( MUSIC_TIME mtTime ) const
{
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem = m_lstBands.GetNext( pos );

		if( pBandItem->m_mtTimePhysical == mtTime )
		{
			return pBandItem;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::CreateUniqueName

BSTR CBandMgr::CreateUniqueName()
{
	// Get "Band" text
	CString strBand;
	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( _Module.GetModuleInstance() );
	strBand.LoadString( IDS_BAND_TEXT );
	AfxSetResourceHandle( hInstance );

	int n = 0;
	POSITION pos = m_lstBands.GetHeadPosition();
	while (pos)
	{
		CBandItem* pBandItem;
		pBandItem = m_lstBands.GetNext( pos );
		ASSERT( pBandItem );
		if ( pBandItem && pBandItem->m_pIBandNode )
		{
			n++;
		}
	}
	char szNum[4];
	CString strName;
	bool fUnique = false;
	while (!fUnique)
	{
		_itoa(n, szNum, 10);
		strName = strBand + szNum;
		fUnique = true;
		POSITION pos = m_lstBands.GetHeadPosition();
		while( pos )
		{
			CBandItem* pBandItem;
			pBandItem = m_lstBands.GetNext( pos );
			ASSERT( pBandItem );
			if ( pBandItem && pBandItem->m_pIBandNode )
			{
				if ( strName == pBandItem->m_strText )
				{
					fUnique = false;
					break;
				}
			}
		}
		n++;
	}
	return strName.AllocSysString();
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::SelectSegment

BOOL CBandMgr::SelectSegment(long lBeginTime, long lEndTime)
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

	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem;
		pBandItem = m_lstBands.GetNext( pos );
		ASSERT( pBandItem );
		if ( pBandItem )
		{
			pBandItem->SetSelectFlag( FALSE );
			if( (unsigned)lBeginMeas < (signed)pBandItem->m_dwMeasure && (signed)pBandItem->m_dwMeasure < lEndMeas )
			{
				pBandItem->SetSelectFlag( TRUE );
				result = TRUE;
			}
			else if( lBeginMeas == (signed)pBandItem->m_dwMeasure )
			{
				if( lEndMeas == (signed)pBandItem->m_dwMeasure )
				{
					if( lBeginBeat <= pBandItem->m_bBeat && pBandItem->m_bBeat <= lEndBeat )
					{
						pBandItem->SetSelectFlag( TRUE );
						result = TRUE;
					}
				}
				else
				{
					if( lBeginBeat <= pBandItem->m_bBeat )
					{
						pBandItem->SetSelectFlag( TRUE );
						result = TRUE;
					}
				}
			}
			else if( lEndMeas == (signed)pBandItem->m_dwMeasure )
			{
				if( pBandItem->m_bBeat <= lEndBeat )
				{
					pBandItem->SetSelectFlag( TRUE );
					result = TRUE;
				}
			}
		}
	}
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::EmptyBandList

void CBandMgr::EmptyBandList(void)
{
	if( !m_lstBands.IsEmpty() )
	{
		CBandItem *pBandItem;
		while ( !m_lstBands.IsEmpty() )
		{
			pBandItem = m_lstBands.RemoveHead();
			CleanUpPropBand( pBandItem );
			delete pBandItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::InsertByAscendingTime

void CBandMgr::InsertByAscendingTime( CBandItem *pBand, BOOL fPaste )
{
	ASSERT( pBand );
	if ( pBand == NULL )
	{
		return;
	}

	CBandItem* pBandItem;
	POSITION posCurrent, posNext = m_lstBands.GetHeadPosition();
	while( posNext )
	{
		posCurrent = posNext;
		pBandItem = m_lstBands.GetNext( posNext );
		ASSERT( pBandItem );
		if( pBandItem )
		{
			if( fPaste )
			{
				if( pBandItem->m_mtTimePhysical == pBand->m_mtTimePhysical )
				{
					// replace item
					m_lstBands.InsertBefore( posCurrent, pBand );
					m_lstBands.RemoveAt( posCurrent );
					CleanUpPropBand( pBandItem );
					delete pBandItem;
					return;
				}
			}
			if( pBandItem->m_mtTimePhysical > pBand->m_mtTimePhysical )
			{
				// insert before posCurrent (which is the position of pBandItem)
				m_lstBands.InsertBefore( posCurrent, pBand );
				return;
			}
		}
	}
	// insert at end of list
	m_lstBands.AddTail( pBand );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::RemoveItem

BOOL CBandMgr::RemoveItem( CBandItem* pItem )
{
	POSITION pos2;
	POSITION pos1 = m_lstBands.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		CBandItem* pAt = m_lstBands.GetNext( pos1 );
		if ( pAt == pItem )
		{
			m_lstBands.RemoveAt( pos2 );
			CleanUpPropBand( pAt );
			delete pAt;
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::RecomputeBandMeasureBeats

void CBandMgr::RecomputeBandMeasureBeats()
{
	// Recompute measure/beat of all Bands
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem = m_lstBands.GetNext( pos );

		pBandItem->SetTimePhysical( pBandItem->m_mtTimePhysical, STP_LOGICAL_ADJUST );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::GetNextGreatestUniqueTime

MUSIC_TIME CBandMgr::GetNextGreatestUniqueTime( long lMeasure, long lBeat, long lTick )
{
	DMUS_TIMESIGNATURE dmTimeSig;
	MUSIC_TIME mtTime;

	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem = m_lstBands.GetNext( pos );

		if( pBandItem->m_dwMeasure > (DWORD)lMeasure )
		{
			break;
		}

		MeasureBeatTickToClocks( pBandItem->m_dwMeasure, 0, 0, &mtTime );
		m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, mtTime, NULL, &dmTimeSig );
		if( pBandItem->m_bBeat > dmTimeSig.bBeatsPerMeasure )
		{
			break;
		}

		if( pBandItem->m_dwMeasure == (DWORD)lMeasure
		&&  pBandItem->m_bBeat == (unsigned)lBeat )
		{
			lTick = pBandItem->m_lTick + 1;

			// Take care of measure/beat rollover
			MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );  
			ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick );  
		}
	}

	MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );  
	return mtTime;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::RecomputeBandTimes

BOOL CBandMgr::RecomputeBandTimes()
{
	CBandItem* pBandItem;
	MUSIC_TIME mtTime;
	long lMeasure;
	long lBeat;
	long lTick;
	POSITION pos2;

	BOOL fChanged = FALSE;

	// For all items, call SetPropBandTime to update their m_mtTime member
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		pBandItem = m_lstBands.GetNext( pos );

		if( !pBandItem->m_pIBandNode )
		{
			m_lstBands.RemoveAt( pos2 );
			delete pBandItem;
		}
		// 26731: Need to skip items that have invalid measure and beat values.
		// This should only happen when this method is called before the AllTrackAdded
		// notification (which updates the measure and beat values to valid ones) was received.
		else if( (signed(pBandItem->m_dwMeasure) >= 0) && (signed(pBandItem->m_bBeat) >= 0) )
		{
			MeasureBeatTickToClocks( pBandItem->m_dwMeasure, pBandItem->m_bBeat, pBandItem->m_lTick, &mtTime );  
			ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick ); 

			if( pBandItem->m_mtTimePhysical != mtTime
			||	pBandItem->m_dwMeasure != (DWORD)lMeasure
			||	pBandItem->m_bBeat != (BYTE)lBeat
			||	pBandItem->m_lTick != lTick )
			{
				m_lstBands.RemoveAt( pos2 );

				if( pBandItem->m_dwMeasure != (DWORD)lMeasure )
				{
					// This would happen when moving from 7/4 to 4/4, for example
					// Bands on beat 7 would end up on next measure's beat 3
					while( pBandItem->m_dwMeasure != (DWORD)lMeasure )
					{
						// Keep moving back a beat until the measure does not change
						MeasureBeatTickToClocks( lMeasure, --lBeat, lTick, &mtTime );  
						ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick ); 
					}
				}

				mtTime = GetNextGreatestUniqueTime( lMeasure, lBeat, lTick ); 
				pBandItem->SetTimePhysical( mtTime, STP_LOGICAL_ADJUST );
				fChanged = TRUE;

				InsertByAscendingTime( pBandItem, FALSE );
			}
			else
			{
				// Recompute logical time measure/beat
				pBandItem->SetTimeLogical( pBandItem->m_mtTimeLogical );
			}
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::GetBoundariesOfSelectedBands

void CBandMgr::GetBoundariesOfSelectedBands( long *plStart, long *plEnd )
{
	ASSERT( plStart );
	ASSERT( plEnd );

	MUSIC_TIME mtTime;

	long lEnd = -MAX_TICK + -1;
	BOOL fSetStart = FALSE;

	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pItem = m_lstBands.GetNext( pos );

		if( pItem->m_fSelected )
		{
			MeasureBeatTickToClocks( pItem->m_dwMeasure, pItem->m_bBeat, 0, &mtTime );

			if( lEnd < mtTime )
			{
				lEnd = mtTime;
			}
			if( !fSetStart )
			{
				fSetStart = TRUE;
				*plStart = mtTime;
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
// CBandMgr::DeleteBetweenTimes

BOOL CBandMgr::DeleteBetweenTimes( long lStart, long lEnd )
{
	BOOL fChanged = FALSE;

	// Iterate through the list
	POSITION pos2, pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		// Save the current position
		pos2 = pos;
		CBandItem* pItem = m_lstBands.GetNext( pos );

		MUSIC_TIME mtTime;
		MeasureBeatTickToClocks( pItem->m_dwMeasure, pItem->m_bBeat, 0, &mtTime );

		// If the Band occurs between lStart and lEnd, delete it
		if( (mtTime >= lStart) && (mtTime <= lEnd) ) 
		{
			m_lstBands.RemoveAt( pos2 );
			CleanUpPropBand( pItem );
			delete pItem;
			fChanged = TRUE;
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::CanCycle

BOOL CBandMgr::CanCycle( long lXPos  )
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
		int nCount = 0;
		CBandItem* pBandItem;

		POSITION pos = m_lstBands.GetHeadPosition();
		while( pos )
		{
			pBandItem = m_lstBands.GetNext( pos );

			if( pBandItem->m_dwMeasure == (DWORD)lMeasure
			&&  pBandItem->m_bBeat == (unsigned)lBeat
			&&  pBandItem->m_strText.IsEmpty() == FALSE )
			{
				if( ++nCount > 1 )
				{
					return TRUE;
				}
			}

			if( pBandItem->m_dwMeasure > (DWORD)lMeasure )
			{
				break;
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::CycleBands

HRESULT CBandMgr::CycleBands( long lXPos  )
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
		CBandItem* pFirstBandItem = NULL;
		CBandItem* pSecondBandItem = NULL;
		CBandItem* pBandItem;

		hr = E_FAIL;

		POSITION pos = m_lstBands.GetHeadPosition();
		while( pos )
		{
			pBandItem = m_lstBands.GetNext( pos );

			if( pBandItem->m_dwMeasure == (DWORD)lMeasure
			&&  pBandItem->m_bBeat == (unsigned)lBeat
			&&  pBandItem->m_strText.IsEmpty() == FALSE )
			{
				if( pFirstBandItem == NULL )
				{
					pFirstBandItem = pBandItem;
				}
				else if( pSecondBandItem == NULL )
				{
					pSecondBandItem = pBandItem;
				}

				if( pBandItem->m_wFlags & BF_TOPBAND )
				{
					if( pos )
					{
						// Cycle to next Band if on same measure/beat
						CBandItem* pNextBandItem = m_lstBands.GetNext( pos );

						if( pNextBandItem->m_dwMeasure == (DWORD)lMeasure
						&&  pNextBandItem->m_bBeat == (unsigned)lBeat
						&&  pNextBandItem->m_strText.IsEmpty() == FALSE )
						{
							UnselectAll();
							pBandItem->m_wFlags &= ~BF_TOPBAND;
							pNextBandItem->SetSelectFlag( TRUE );
							pNextBandItem->m_wFlags |= BF_TOPBAND;
							hr = S_OK;
							break;
						}
					}

					// Cycle to first Band on same measure/beat
					UnselectAll();
					pBandItem->m_wFlags &= ~BF_TOPBAND;
					pFirstBandItem->SetSelectFlag( TRUE );
					pFirstBandItem->m_wFlags |= BF_TOPBAND;
					hr = S_OK;
					break;
				}
			}

			if( pBandItem->m_dwMeasure > (DWORD)lMeasure
			||  pos == NULL )
			{
				UnselectAll();
				if( pSecondBandItem )
				{
					pSecondBandItem->SetSelectFlag( TRUE );
					pSecondBandItem->m_wFlags |= BF_TOPBAND;
					hr = S_OK;
				}
				else if( pFirstBandItem )
				{
					pFirstBandItem->SetSelectFlag( TRUE );
					pFirstBandItem->m_wFlags |= BF_TOPBAND;
					hr = S_OK;
				}
				break;
			}
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		// Redraw the band strip
		m_pTimeline->StripInvalidateRect( m_pBandStrip, NULL, TRUE );

		// Update the property page
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::SetDocRootForAllBands

void CBandMgr::SetDocRootForAllBands()
{
	if( m_pBandStrip )
	{
		POSITION pos = m_lstBands.GetHeadPosition();
		while( pos )
		{
			CBandItem* pBandItem = m_lstBands.GetNext( pos );
			if( pBandItem->m_pIBandNode )
			{
				pBandItem->m_pIBandNode->SetDocRootNode( m_pBandStrip->m_pStripNode );
			}
		}
	}
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


/////////////////////////////////////////////////////////////////////////////
// CBandMgr::UpdateBandNodesWithAudiopath

void CBandMgr::UpdateBandNodesWithAudiopath( void )
{
	IUnknown *punkAudiopath = NULL;
	if( m_pSegmentState )
	{
		m_pSegmentState->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH, 0, GUID_NULL, 0, IID_IUnknown, (void **)&punkAudiopath );
	}

	IDMUSProdBandEdit8a *pIDMUSProdBandEdit8a;
	POSITION pos = m_lstBands.GetHeadPosition();
	while( pos )
	{
		CBandItem* pBandItem = m_lstBands.GetNext( pos );

		if( pBandItem->m_pIBandNode
		&&	SUCCEEDED( pBandItem->m_pIBandNode->QueryInterface( IID_IDMUSProdBandEdit8a, (void **)&pIDMUSProdBandEdit8a ) ) )
		{
			pIDMUSProdBandEdit8a->SetAudiopath( punkAudiopath );
			pIDMUSProdBandEdit8a->Release();
		}
	}

	if( punkAudiopath )
	{
		punkAudiopath->Release();
	}
}
