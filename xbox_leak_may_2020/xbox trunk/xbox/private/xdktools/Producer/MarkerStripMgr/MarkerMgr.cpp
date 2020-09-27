// MarkerMgr.cpp : implementation file
//

/*--------------
@doc MARKERSAMPLE
--------------*/

#include "stdafx.h"
#include "MarkerMgr.h"
#include "PropPageMgr.h"
#include <RiffStrm.h>
#include <dmusicf.h>
#include "SegmentGUIDs.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Set information about this strip manager
const CLSID CMarkerMgr::m_clsid = CLSID_DirectMusicMarkerTrack;
const DWORD CMarkerMgr::m_ckid = NULL;
const DWORD CMarkerMgr::m_fccType = DMUS_FOURCC_MARKERTRACK_LIST;

/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr constructor/destructor 

CMarkerMgr::CMarkerMgr() : CBaseMgr()
{
	// Create a MarkerStrip
	m_pMarkerStrip = new CMarkerStrip(this);
	ASSERT( m_pMarkerStrip );

	// Copy the pointer to the base strip manager
	m_pBaseStrip = m_pMarkerStrip;

	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
}

CMarkerMgr::~CMarkerMgr()
{
	// Clean up our references
	if( m_pMarkerStrip )
	{
		m_pMarkerStrip->Release();
		m_pMarkerStrip = NULL;
	}

	// Clear the base strip manager's pointer to the strip
	m_pBaseStrip = NULL;

	// Delete all the items in m_lstCueMarkers and m_lstSwitchMarkers
	EmptyMarkerList( m_lstCueMarkers );
	EmptyMarkerList( m_lstSwitchMarkers );
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CMarkerMgr::OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the update isn't for our strip, exit
	if( !(dwGroupBits & m_dwGroupBits) )
	{
		return E_INVALIDARG;
	}

	// All Tracks Added
	if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		// Fix measure/beat of all markers
		RecomputeMeasureBeats();

		m_pTimeline->StripInvalidateRect( m_pMarkerStrip, NULL, TRUE );
		return S_OK;
	}

	// We don't handle whichever notification was passed to us
	return CBaseMgr::OnUpdate( rguidType, dwGroupBits, pData );
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CMarkerMgr::SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	switch( stripMgrProperty )
	{
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
// CMarkerMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::Load

HRESULT CMarkerMgr::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that the stream pointer is non-null
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	// Try and load in the markers
	CTypedPtrList<CPtrList, CMarkerItem*> lstCueMarkers, lstSwitchMarkers;
	HRESULT hr = LoadMarkerList( lstCueMarkers, lstSwitchMarkers, pIStream );

	// If the load succeeded
	if( SUCCEEDED( hr ) )
	{
		// Remove all existing items
		EmptyMarkerList( m_lstCueMarkers );
		EmptyMarkerList( m_lstSwitchMarkers );

		// Copy the markers into the real list
		while( !lstCueMarkers.IsEmpty() )
		{
			// Go ahead and insert the item
			InsertByAscendingTime( lstCueMarkers.RemoveTail() );
		}

		// Copy the markers into the real list
		while( !lstSwitchMarkers.IsEmpty() )
		{
			// Go ahead and insert the item
			InsertByAscendingTime( lstSwitchMarkers.RemoveTail() );
		}

		SyncWithDirectMusic();
	}
	else
	{
		// Delete any markers loaded in
		while( !lstCueMarkers.IsEmpty() )
		{
			delete lstCueMarkers.RemoveHead();
		}

		// Delete any markers loaded in
		while( !lstSwitchMarkers.IsEmpty() )
		{
			delete lstSwitchMarkers.RemoveHead();
		}
	}

	// Return the success/failure status
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::Save

HRESULT CMarkerMgr::Save( IStream* pIStream, BOOL fClearDirty )
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
	// stream that will be loaded into a DirectMusic Marker track (GUID_DirectMusicObject)
	if( !::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) &&
		!::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
	{
		return E_INVALIDARG;
	}

	// Now, finally save ourself
	HRESULT hr = SaveMarkerList( m_lstCueMarkers, m_lstSwitchMarkers, pIStream );

	// If we're supposed to clear our dirty flag, do so now (since the save succeeded)
	if( SUCCEEDED(hr) && fClearDirty )
	{
		m_fDirty = false;
	}

	// Return the success/failure code
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::GetData

// This method is called by CMarkerPropPageMgr to get data to send to the
// Marker property page.
// The CMarkerStrip::GetData() method is called by CGroupBitsPropPageMgr
// to get the strip's properties (currently just Group Bits)
HRESULT STDMETHODCALLTYPE CMarkerMgr::GetData( /* [retval][out] */ void **ppData)
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
	CMarkerItem* pFirstSelectedMarkerItem = NULL;

	// Start iterating through the item list
	POSITION pos = m_lstCueMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CMarkerItem* pMarkerItem = m_lstCueMarkers.GetNext( pos );

		// Check if the item is selected
		if ( pMarkerItem->m_fSelected )
		{
			// If nothing is selected
			if( !pFirstSelectedMarkerItem )
			{
				// Marker is selected - save a pointer to it in pFirstSelectedMarkerItem
				pFirstSelectedMarkerItem = pMarkerItem;
			}
			// If another marker is selected
			else
			{
				// More than one item is selected - set fMultipleSelect to TRUE
				// and break out of this loop
				fMultipleSelect = TRUE;
				break;
			}
		}
	}

	// Iterate through the list of items
	pos = m_lstSwitchMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to this lyic
		CMarkerItem* pMarkerItem = m_lstSwitchMarkers.GetNext( pos );

		// Check if the item is selected
		if ( pMarkerItem->m_fSelected )
		{
			// If nothing is selected
			if( !pFirstSelectedMarkerItem )
			{
				// Marker is selected - save a pointer to it in pFirstSelectedMarkerItem
				pFirstSelectedMarkerItem = pMarkerItem;
			}
			// If another marker is selected
			else
			{
				// Check to see if pMarkerItem or pFirstSelectedMarkerItem is earlier
				if( pMarkerItem->m_mtTime < pFirstSelectedMarkerItem->m_mtTime )
				{
					// pMarkerItem is earlier, so set pFirstSelectedMarkerItem to it
					pFirstSelectedMarkerItem = pMarkerItem;
				}

				// More than one item is selected - set fMultipleSelect to TRUE
				// and break out of this loop
				fMultipleSelect = TRUE;
				break;
			}
		}
	}

	// If at least one item is selected
	if( pFirstSelectedMarkerItem )
	{
		// Copy the first selected item to a CMarkerItem class
		m_SelectedMarkerItem.Copy( pFirstSelectedMarkerItem );

		// If more than one item was selected, set UD_MULTIPLESELECT
		if( fMultipleSelect )
		{
			m_SelectedMarkerItem.m_dwBitsUI |= UD_MULTIPLESELECT;
		}
		else
		{
			// Only one item selected, clear UD_MULTIPLESELECT
			m_SelectedMarkerItem.m_dwBitsUI &= ~UD_MULTIPLESELECT;
		}

		// Set the passed in pointer to point to the CMarkerItem class
		*ppData = &m_SelectedMarkerItem;

		// Return a success code
		return S_OK;
	}

	// Nothing selected, so clear the passed in pointer
	*ppData = NULL;

	// Return a success code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::SetData

// This method is called by CMarkerPropPageMgr in response to user actions
// in the Marker Property page.  It changes the currenly selected Marker. 
HRESULT STDMETHODCALLTYPE CMarkerMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate the pData pointer
	if ( pData == NULL )
	{
		return E_POINTER;
	}

	// Get a pointer to the first selected item.
	CMarkerItem* pMarkerItem = FirstSelectedMarker();

	// If a selected item was found
	if ( pMarkerItem )
	{
		// Convert the passed-in pointer to a CMarkerItem*
		CMarkerItem* pNewMarker = static_cast<CMarkerItem*>(pData);

		// Check to see if the time position of the item changed
		if( (pNewMarker->m_lTick != pMarkerItem->m_lTick) ||
			(pNewMarker->m_lGrid != pMarkerItem->m_lGrid) ||
			(pNewMarker->m_lBeat != pMarkerItem->m_lBeat) ||
			(pNewMarker->m_lMeasure != pMarkerItem->m_lMeasure) )
		{
			// Update the position
			pMarkerItem->m_lTick = pNewMarker->m_lTick;
			pMarkerItem->m_lGrid = pNewMarker->m_lGrid;
			pMarkerItem->m_lBeat = pNewMarker->m_lBeat;
			pMarkerItem->m_lMeasure = pNewMarker->m_lMeasure;

			// Convert measure and beat position to a time value
			MUSIC_TIME mtTime;
			if( SUCCEEDED( MeasureBeatGridTickToClocks( pNewMarker->m_lMeasure, pNewMarker->m_lBeat,
							pNewMarker->m_lGrid, pNewMarker->m_lTick, &mtTime ) ) )
			{
				// Get the length of the segment
				VARIANT varLength;
				m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength );

				// Ensure the item doesn't move past the end of the segment
				mtTime = min( mtTime, V_I4(&varLength) - 1 );

				// Ensure mtTime doesn't go negative
				mtTime = max( mtTime, 0 );

				// Convert time value to a measure and beat position
				long lTick, lGrid, lBeat, lMeasure;
				if( SUCCEEDED( ClocksToMeasureBeatGridTick( mtTime, &lMeasure, &lBeat,
								&lGrid, &lTick ) ) )
				{
					// Set the measure, beat, grid, and tick values
					pMarkerItem->m_lMeasure = lMeasure;
					pMarkerItem->m_lBeat = lBeat;
					pMarkerItem->m_lGrid = lGrid;
					pMarkerItem->m_lTick = lTick;
				}

				// Now, actually set the time of the marker
				pMarkerItem->m_mtTime = mtTime;
			}

			// We just moved the item
			m_nLastEdit = IDS_UNDO_MOVE;

			// Remove the MarkerItem from the list
			POSITION posToRemove = m_lstCueMarkers.Find( pMarkerItem );
			if( posToRemove )
			{
				m_lstCueMarkers.RemoveAt( posToRemove );
			}
			else
			{
				posToRemove = m_lstSwitchMarkers.Find( pMarkerItem );
				ASSERT( posToRemove );

				if( posToRemove )
				{
					m_lstSwitchMarkers.RemoveAt( posToRemove );
				}
			}

			// Re-add the item at its new position - this will overwrite any existing
			// item at this position
			InsertByAscendingTime( pMarkerItem );

			// Clear all selections
			m_pMarkerStrip->m_pSelectedCueGridRegions->Clear();
			m_pMarkerStrip->m_pSelectedSwitchGridRegions->Clear();

			// Select just the changed item
			switch( pMarkerItem->m_typeMarker )
			{
			case MARKER_SWITCH:
				CListSelectedGridRegion_AddRegion(*m_pMarkerStrip->m_pSelectedSwitchGridRegions, *pMarkerItem);
				break;
			case MARKER_CUE:
				CListSelectedGridRegion_AddRegion(*m_pMarkerStrip->m_pSelectedCueGridRegions, *pMarkerItem);
				break;
			}
		}

		// Nothing changed
		else
		{
			// Return a success code saying that nothing happened
			return S_FALSE;
		}

		// Redraw the Marker strip
		m_pTimeline->StripInvalidateRect( m_pMarkerStrip, NULL, TRUE );

		// Let our hosting editor know about the changes
		OnDataChanged();

		// Refresh the property page with new values
		m_pPropPageMgr->RefreshData();

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
// CMarkerMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CMarkerMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		// Create a new Marker property page manager
		CMarkerPropPageMgr* pPPM = new CMarkerPropPageMgr;

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
// CMarkerMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::DeleteSelectedMarkers

void CMarkerMgr::DeleteSelectedMarkers()
{
	// Start iterating through the list of items
	POSITION pos1 = m_lstCueMarkers.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CMarkerItem* pMarkerItem = m_lstCueMarkers.GetNext( pos1 );

		// Check if the current item is selected
		if ( pMarkerItem->m_fSelected )
		{
			// This item is selected, remove it from the list
			m_lstCueMarkers.RemoveAt( pos2 );

			// Now, delete this item
			delete pMarkerItem;
		}
	}

	// Start iterating through the list of items
	pos1 = m_lstSwitchMarkers.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CMarkerItem* pMarkerItem = m_lstSwitchMarkers.GetNext( pos1 );

		// Check if the current item is selected
		if ( pMarkerItem->m_fSelected )
		{
			// This item is selected, remove it from the list
			m_lstSwitchMarkers.RemoveAt( pos2 );

			// Now, delete this item
			delete pMarkerItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::SaveSelectedMarkers

HRESULT CMarkerMgr::SaveSelectedMarkers(LPSTREAM pIStream, CMarkerItem* pMarkerAtDragPoint)
{
	// if pMarkerAtDragPoint is valid, set mtOffset and dwMeasureOffset so that the time pMarkerAtDragPoint is 0.
	// if pMarkerAtDragPoint is NULL, set mtOffset and dwMeasureOffset so that the time of the first Marker is 0.

	// Verify pIStream is valid
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	// If the Marker list has anything in it, look for selected Markers
	if( !m_lstCueMarkers.IsEmpty()
	||	!m_lstSwitchMarkers.IsEmpty() )
	{
		// Initialize the grid offset to an invalid value
		long lGridOffset = -1;

		// If pMarkerAtDragPoint is valid, just use the measure and beat and grid information from it
		if( pMarkerAtDragPoint )
		{
			// Verify that this item is selected
			ASSERT( pMarkerAtDragPoint->m_fSelected );

			// Compute how many grids from the start it is
			MeasureBeatGridToGrids( m_pTimeline, m_dwGroupBits, 0, pMarkerAtDragPoint->m_lMeasure, pMarkerAtDragPoint->m_lBeat, pMarkerAtDragPoint->m_lGrid, lGridOffset );
		}
		// Otherwise look for the first selected item
		else
		{
			// Get a pointer to the first selected item
			CMarkerItem *pFirstSelMarkerItem = FirstSelectedMarker();

			// If we found an item
			if( pFirstSelMarkerItem )
			{
				// Compute how many grids from the start it is
				MeasureBeatGridToGrids( m_pTimeline, m_dwGroupBits, 0, pFirstSelMarkerItem->m_lMeasure, pFirstSelMarkerItem->m_lBeat, pFirstSelMarkerItem->m_lGrid, lGridOffset );
			}
		}

		// pMarkerAtDragPoint is NULL and there are no selected items - return with S_FALSE
		if ( lGridOffset == -1 )
		{
			return S_FALSE;
		}

		// Now, actually save the items
		return SaveSelectedMarkers( pIStream, lGridOffset );
	}
	else
	{
		return S_FALSE; // Nothing in the list
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::MarkSelectedMarkers

// ORs dwFlags with the m_dwBitsUI of each selected item
void CMarkerMgr::MarkSelectedMarkers( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos = m_lstCueMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current item
		CMarkerItem* pMarkerItem = m_lstCueMarkers.GetNext( pos );

		// Check if the item is selected
		if ( pMarkerItem->m_fSelected )
		{
			// It's selected - update m_dwBitsUI
			pMarkerItem->m_dwBitsUI |= dwFlags;
		}
	}

	// Iterate through the list of items
	pos = m_lstSwitchMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current item
		CMarkerItem* pMarkerItem = m_lstSwitchMarkers.GetNext( pos );

		// Check if the item is selected
		if ( pMarkerItem->m_fSelected )
		{
			// It's selected - update m_dwBitsUI
			pMarkerItem->m_dwBitsUI |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::DeleteMarked

// deletes items marked by given flag
void CMarkerMgr::DeleteMarked( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos1 = m_lstCueMarkers.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CMarkerItem* pMarkerItem = m_lstCueMarkers.GetNext( pos1 );

		// Check if any of the specified dwFlags are set in this item
		if ( pMarkerItem->m_dwBitsUI & dwFlags )
		{
			// At least one of the flags in dwFlags is set, remove the item
			m_lstCueMarkers.RemoveAt( pos2 );

			// Now, delete it
			delete pMarkerItem;
		}
	}

	// Iterate through the list of items
	pos1 = m_lstSwitchMarkers.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CMarkerItem* pMarkerItem = m_lstSwitchMarkers.GetNext( pos1 );

		// Check if any of the specified dwFlags are set in this item
		if ( pMarkerItem->m_dwBitsUI & dwFlags )
		{
			// At least one of the flags in dwFlags is set, remove the item
			m_lstSwitchMarkers.RemoveAt( pos2 );

			// Now, delete it
			delete pMarkerItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::UnMarkMarkers

// unmarks flag m_dwUndermined field CMarkerItems in list
void CMarkerMgr::UnMarkMarkers( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos = m_lstCueMarkers.GetHeadPosition();
	while( pos )
	{
		// Clear the selected flags in each item
		m_lstCueMarkers.GetNext( pos )->m_dwBitsUI &= ~dwFlags;
	}

	// Iterate through the list of items
	pos = m_lstSwitchMarkers.GetHeadPosition();
	while( pos )
	{
		// Clear the selected flags in each item
		m_lstSwitchMarkers.GetNext( pos )->m_dwBitsUI &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::UnselectAll

void CMarkerMgr::UnselectAll()
{
	// Iterate through the list of items
	POSITION pos = m_lstCueMarkers.GetHeadPosition();
	while( pos )
	{
		// Clear the selection flag for each item
		m_lstCueMarkers.GetNext( pos )->m_fSelected = FALSE;
	}

	// Iterate through the list of items
	pos = m_lstSwitchMarkers.GetHeadPosition();
	while( pos )
	{
		// Clear the selection flag for each item
		m_lstSwitchMarkers.GetNext( pos )->m_fSelected = FALSE;
	}

	// Clear the list of selected regions
	m_pMarkerStrip->m_pSelectedCueGridRegions->Clear();
	m_pMarkerStrip->m_pSelectedSwitchGridRegions->Clear();
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::SelectAll

void CMarkerMgr::SelectAll()
{
	// Iterate through the list of items
	POSITION pos = m_lstCueMarkers.GetHeadPosition();
	while( pos )
	{
		// Set the selection flag for each item
		m_lstCueMarkers.GetNext( pos )->m_fSelected = TRUE;
	}

	// Iterate through the list of items
	pos = m_lstSwitchMarkers.GetHeadPosition();
	while( pos )
	{
		// Set the selection flag for each item
		m_lstSwitchMarkers.GetNext( pos )->m_fSelected = TRUE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::FirstSelectedMarker

CMarkerItem* CMarkerMgr::FirstSelectedMarker()
{
	// Variable to store a pointer to the selected marker in
	CMarkerItem *pFirstSelectedMarker = NULL;

	// Iterate through the list of items
	POSITION pos = m_lstCueMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to this lyic
		CMarkerItem* pMarkerItem = m_lstCueMarkers.GetNext( pos );

		// Check if the item is selected
		if ( pMarkerItem->m_fSelected )
		{
			// Marker is selected, sae a pointer to it
			pFirstSelectedMarker = pMarkerItem;
			break;
		}
	}

	// Iterate through the list of items
	pos = m_lstSwitchMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to this lyic
		CMarkerItem* pMarkerItem = m_lstSwitchMarkers.GetNext( pos );

		// Check if the item is selected
		if ( pMarkerItem->m_fSelected )
		{
			// Check if there are any selecte Cue markers
			if( pFirstSelectedMarker )
			{
				// Yes, check to see which marker is earlier
				return pMarkerItem->m_mtTime < pFirstSelectedMarker->m_mtTime ? pMarkerItem : pFirstSelectedMarker;
			}
			else
			{
				// No, return a pointer to this marker
				return pMarkerItem;
			}
		}
	}

	// No items are selected in m_lstSwitchMarkers, return pFirstSelectedMarker
	return pFirstSelectedMarker;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::InsertByAscendingTime

void CMarkerMgr::InsertByAscendingTime( CMarkerItem *pMarkerToInsert )
{
	// Ensure the pMarkerToInsert pointer is valid
	if ( pMarkerToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Get a pointer to the list to insert into
	CTypedPtrList<CPtrList, CMarkerItem*> *plstMarkers;
	switch( pMarkerToInsert->m_typeMarker )
	{
	case MARKER_SWITCH:
		plstMarkers = &m_lstSwitchMarkers;
		break;

	case MARKER_CUE:
		plstMarkers = &m_lstCueMarkers;
		break;

	default:
		ASSERT( FALSE );
		return;
	}

	// Iterate through the list of items
	POSITION pos1 = plstMarkers->GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CMarkerItem* pMarkerItem = plstMarkers->GetNext( pos1 );

		// Check if the current item is at the same time or later than the one to be inserted
		if ( pMarkerItem->m_mtTime >= pMarkerToInsert->m_mtTime )
		{
			// insert before pos2 (current position of pMarkerItem)
			plstMarkers->InsertBefore( pos2, pMarkerToInsert );

			// If we've overwritten an existing Marker, remove and delete the existing item
			if( pMarkerItem->m_mtTime == pMarkerToInsert->m_mtTime )
			{
				plstMarkers->RemoveAt( pos2 );
				delete pMarkerItem;
			}

			// Return, since we found the position to insert the item in
			return;
		}
	}

	// pMarkerToInsert is later than all items in the list, add it at the end of the list
	plstMarkers->AddTail( pMarkerToInsert );

	// Return, since we found the position to insert the item in
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::RemoveItem

bool CMarkerMgr::RemoveItem( CMarkerItem* pItem )
{
	// Find the given item
	POSITION posToRemove = m_lstCueMarkers.Find( pItem, NULL );

	// If item wasn't found, try the other list
	if( posToRemove == NULL )
	{
		// Find the given item
		posToRemove = m_lstSwitchMarkers.Find( pItem, NULL );

		// If the item wasn't found, return false
		if( posToRemove == NULL )
		{
			return false;
		}
		else
		{
			// Remove the item from the list - the caller must delete it
			m_lstSwitchMarkers.RemoveAt( posToRemove );
		}
	}
	else
	{
		// Remove the item from the list - the caller must delete it
		m_lstCueMarkers.RemoveAt( posToRemove );
	}

	// Return true since we found the item
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::GetNextGreatestUniqueTime

MUSIC_TIME CMarkerMgr::GetNextGreatestUniqueTime( MARKER_TYPE typeMarker, long lMeasure, long lBeat, long lGrid, long lTick )
{
	// JD: I don't think this is necessary
	//DMUS_TIMESIGNATURE dmTimeSig;
	MUSIC_TIME mtTime;

	// Get a pointer to the list to search
	CTypedPtrList<CPtrList, CMarkerItem*> *plstMarkers;
	switch( typeMarker )
	{
	case MARKER_SWITCH:
		plstMarkers = &m_lstSwitchMarkers;
		break;

	case MARKER_CUE:
		plstMarkers = &m_lstCueMarkers;
		break;

	default:
		ASSERT( FALSE );
		MeasureBeatGridTickToClocks( lMeasure, lBeat, lGrid, lTick, &mtTime );  
		return mtTime ;
	}

	// Iterate throught the Marker list
	POSITION pos = plstMarkers->GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current item
		CMarkerItem* pItem = plstMarkers->GetNext( pos );

		// If this item is beyond the measure we're looking for, exit
		// since the passed in lMeasure and lBeat are empty
		if( pItem->m_lMeasure > lMeasure )
		{
			break;
		}
		// If this item is earlier than the measure we're looking for, continue
		else if( pItem->m_lMeasure < lMeasure )
		{
			continue;
		}

		// JD: I don't think this is necessary
		/*
		// Get the time of the item's measure
		MeasureBeatGridTickToClocks( pItem->m_lMeasure, 0, 0, &mtTime );

		// Get the timesig of the measure the item is in
		m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, mtTime, NULL, &dmTimeSig );

		// If the item's beat is greater than the # of beats in the measure
		if( pItem->m_lBeat > dmTimeSig.bBeatsPerMeasure )
		{
			break;
		}
		*/

		// Check if the measure, beat, and grid values match
		if( pItem->m_lMeasure == lMeasure
		&&  pItem->m_lBeat == lBeat
		&&	pItem->m_lGrid == lGrid )
		{
			// Yes - increment the tick value by one
			lTick = pItem->m_lTick + 1;

			// Take care of measure/beat rollover
			MeasureBeatGridTickToClocks( lMeasure, lBeat, lGrid, lTick, &mtTime );  
			ClocksToMeasureBeatGridTick( mtTime, &lMeasure, &lBeat, &lGrid, &lTick );  
		}
	}

	MeasureBeatGridTickToClocks( lMeasure, lBeat, lGrid, lTick, &mtTime );  
	return mtTime;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::RecomputeMeasureBeats

void CMarkerMgr::RecomputeMeasureBeats()
{
	// Recompute measure/beat of all items
	POSITION pos = m_lstCueMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CMarkerItem* pItem = m_lstCueMarkers.GetNext( pos );

		ClocksToMeasureBeatGridTick( pItem->m_mtTime, &pItem->m_lMeasure, &pItem->m_lBeat, &pItem->m_lGrid, &pItem->m_lTick ); 
	}

	// Recompute measure/beat of all items
	pos = m_lstSwitchMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CMarkerItem* pItem = m_lstSwitchMarkers.GetNext( pos );

		ClocksToMeasureBeatGridTick( pItem->m_mtTime, &pItem->m_lMeasure, &pItem->m_lBeat, &pItem->m_lGrid, &pItem->m_lTick ); 
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::RecomputeTimesHelper

bool CMarkerMgr::RecomputeTimesHelper( MARKER_TYPE typeMarker )
{
	// Get a pointer to the list to use
	CTypedPtrList<CPtrList, CMarkerItem*> *plstMarkers;
	switch( typeMarker )
	{
	case MARKER_SWITCH:
		plstMarkers = &m_lstSwitchMarkers;
		break;

	case MARKER_CUE:
		plstMarkers = &m_lstCueMarkers;
		break;

	default:
		ASSERT( FALSE );
		return false;
	}

	MUSIC_TIME mtTime;
	long lMeasure;
	long lBeat;
	long lGrid;
	long lTick;

	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = plstMarkers->GetHeadPosition();
	while( pos )
	{
		// Save our current position
		POSITION pos2 = pos;

		// Get a pointer to the current item
		CMarkerItem* pMarkerItem = plstMarkers->GetNext( pos );

		// Make sure measure and beat are valid
		if( pMarkerItem->m_lMeasure >= 0
		&&  pMarkerItem->m_lBeat >= 0 )
		{
			// Using the item's current measure and beat settings, determine which measure and beat
			// the item will end up on
			MeasureBeatGridTickToClocks( pMarkerItem->m_lMeasure, pMarkerItem->m_lBeat, pMarkerItem->m_lGrid, pMarkerItem->m_lTick, &mtTime );  
			ClocksToMeasureBeatGridTick( mtTime, &lMeasure, &lBeat, &lGrid, &lTick ); 

			// Check if either the measure, beat, grid, or tick changed
			if( pMarkerItem->m_mtTime != mtTime
			||	pMarkerItem->m_lMeasure != lMeasure
			||	pMarkerItem->m_lBeat != lBeat
			||	pMarkerItem->m_lGrid != lGrid
			||	pMarkerItem->m_lTick != lTick )
			{
				// Remove the item from the list
				plstMarkers->RemoveAt( pos2 );

				// Check if the measure the Marker is in changed
				if( pMarkerItem->m_lMeasure > lMeasure )
				{
					// This would happen when moving from 7/4 to 4/4, for example
					// Markers on beat 7 would end up on next measure's beat 3
					while( pMarkerItem->m_lMeasure != lMeasure )
					{
						// Keep moving back a grid until the measure does not change
						MeasureBeatGridTickToClocks( lMeasure, lBeat, --lGrid, lTick, &mtTime );  
						ClocksToMeasureBeatGridTick( mtTime, &lMeasure, &lBeat, &lGrid, &lTick ); 
					}
				}

				mtTime = GetNextGreatestUniqueTime( typeMarker, lMeasure, lBeat, lGrid, lTick ); 
				ClocksToMeasureBeatGridTick( mtTime, &lMeasure, &lBeat, &lGrid, &lTick ); 

				pMarkerItem->m_mtTime = mtTime;
				pMarkerItem->m_lMeasure = lMeasure;
				pMarkerItem->m_lBeat = lBeat;
				pMarkerItem->m_lGrid = lGrid;
				pMarkerItem->m_lTick = lTick;
				MeasureBeatGridTickToClocks( pMarkerItem->m_lMeasure, pMarkerItem->m_lBeat, pMarkerItem->m_lGrid, pMarkerItem->m_lTick, &pMarkerItem->m_mtTime ); 

				// Flag that something changed
				fChanged = true;

				// Reinsert it into the list
				InsertByAscendingTime( pMarkerItem );
			}
		}
	}

	// Return whether or not anything changed
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::RecomputeTimes

bool CMarkerMgr::RecomputeTimes()
{
	bool fChanged = RecomputeTimesHelper( MARKER_CUE );
	bool fChanged2 = RecomputeTimesHelper( MARKER_SWITCH );
	return fChanged || fChanged2;
}


/////////////////////////////////////////////////////////////////////////////
// DeleteBetweenMeasureBeatGridsHelper

bool DeleteBetweenMeasureBeatGridsHelper( CTypedPtrList<CPtrList, CMarkerItem*>& lstMarkers, long lmStart, long lbStart, long lgStart, long lmEnd, long lbEnd, long lgEnd )
{
	// Initially, nothing changed
	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = lstMarkers.GetHeadPosition();
	while(pos)
	{
		// Save current position
		POSITION posTemp = pos;

		// Get a pointer to the current item
		CMarkerItem* pItem = lstMarkers.GetNext(pos);

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
		&&	( pItem->m_lBeat > lbEnd ) )
		{
			// Too late - done looking (since the list is sorted)
			break;
		}

		// If the item is in the start measure and beat, check if it is before the start grid
		if( ( pItem->m_lMeasure == lmStart )
		&&	( pItem->m_lBeat == lbStart )
		&&	( pItem->m_lGrid < lgStart ) )
		{
			// Too early - keep looking
			continue;
		}

		// If the item is in the end measure, check if it is after the end beat
		if( ( pItem->m_lMeasure == lmEnd)
		&&	( pItem->m_lBeat == lbEnd )
		&&	( pItem->m_lGrid > lgEnd ) )
		{
			// Too late - done looking (since the list is sorted)
			break;
		}

		// Within the given range, delete the item
		lstMarkers.RemoveAt(posTemp);
		delete pItem;
		fChanged = true;
	}

	// Return whether or not anything changed
	return fChanged;
}

/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::DeleteBetweenMeasureBeatGrids

bool CMarkerMgr::DeleteBetweenMeasureBeatGrids(long lmStart, long lbStart, long lgStart, long lmEnd, long lbEnd, long lGend )
{
	bool fChanged = DeleteBetweenMeasureBeatGridsHelper( m_lstCueMarkers, lmStart, lbStart, lgStart, lmEnd, lbEnd, lGend );
	bool fChanged2 = DeleteBetweenMeasureBeatGridsHelper( m_lstSwitchMarkers, lmStart, lbStart, lgStart, lmEnd, lbEnd, lGend );
	return fChanged || fChanged2;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::SaveSelectedMarkers

HRESULT CMarkerMgr::SaveSelectedMarkers( IStream* pIStream, long lGridOffset )
// save selected items and adjust by "offset"
{
	// Verify that the pIStream pointer is valid
	if(pIStream == NULL)
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	// Create lists to store the items to save in
	CTypedPtrList<CPtrList, CMarkerItem*> lstCueMarkersToSave;
	CTypedPtrList<CPtrList, CMarkerItem*> lstSwitchMarkersToSave;

	// Iterate throught the list of items
	POSITION pos = m_lstCueMarkers.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		CMarkerItem* pMarker = m_lstCueMarkers.GetNext(pos);

		// Check if the item is selected
		if( pMarker->m_fSelected )
		{
			// Add the item to the list of items to save
			lstCueMarkersToSave.AddTail(new CMarkerItem(*pMarker));
		}
	}
	// Iterate throught the list of items
	pos = m_lstSwitchMarkers.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		CMarkerItem* pMarker = m_lstSwitchMarkers.GetNext(pos);

		// Check if the item is selected
		if( pMarker->m_fSelected )
		{
			// Add the item to the list of items to save
			lstSwitchMarkersToSave.AddTail(new CMarkerItem(*pMarker));
		}
	}

	//check that anything is selected
	if( lstSwitchMarkersToSave.IsEmpty() && lstCueMarkersToSave.IsEmpty() )
	{
		return S_FALSE;
	}

	// For each item in lstSwitchMarkersToSave, convert to a number of grids and subtract lGridOffset
	NormalizeMarkerList( m_pTimeline, m_dwGroupBits, lstSwitchMarkersToSave, lGridOffset );

	// For each item in lstCueMarkersToSave, convert to a number of grids and subtract lGridOffset
	NormalizeMarkerList( m_pTimeline, m_dwGroupBits, lstCueMarkersToSave, lGridOffset );

	// Now, finally save ourself
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr = E_FAIL;

	// Allocate an IDMUSProdRIFFStream from the IStream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		goto ON_ERROR;
	}

	// Create the main LIST chunk
	MMCKINFO ckMain;
	ckMain.fccType = DMPROD_FOURCC_MARKERTRACK_LIST;
	if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save cue markers, if there are any
	// Get the position of the first play marker
	pos = lstCueMarkersToSave.GetHeadPosition();

	// If there are any play markers, save them
	if( pos )
	{
		// Create a LIST chunk to store the Marker data
		MMCKINFO ckCue;
		ckCue.ckid = DMPROD_FOURCC_PLAYMARKER_CHUNK;
		if( pIRiffStream->CreateChunk( &ckCue, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto CUE_EXIT;
		}

		// Check the amount of data written
		DWORD dwBytesWritten;

		// Write out the size of the structure
		DWORD dwStructureSize = sizeof( DMPROD_IO_PLAY_MARKER );
		hr = pIStream->Write( &dwStructureSize, sizeof(DWORD), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
		{
			// Handle I/O errors by returning an error code
			hr = E_FAIL;
			goto CUE_ON_ERROR;
		}

		// The structure to save
		DMPROD_IO_PLAY_MARKER oPlayMarker;

		// Iterate through the Marker list
		while( pos )
		{
			// Get a pointer to each item
			CMarkerItem* pMarkerItem = lstCueMarkersToSave.GetNext( pos );

			// Initialize the structure
			ZeroMemory( &oPlayMarker, sizeof(DMPROD_IO_PLAY_MARKER) );

			// Copy the time to the structure
			oPlayMarker.mtGrid = pMarkerItem->m_mtTime;
			oPlayMarker.mtTick = pMarkerItem->m_lTick;

			// Write the structure out to the stream
			hr = pIStream->Write( &oPlayMarker, sizeof(DMPROD_IO_PLAY_MARKER), &dwBytesWritten );
			if( FAILED( hr ) || dwBytesWritten != sizeof(DMPROD_IO_PLAY_MARKER) )
			{
				// Handle I/O errors by return an error code
				hr = E_FAIL;
				goto CUE_ON_ERROR;
			}
		}

CUE_ON_ERROR:
		// Ascend out of the play marker chunk.
		pIRiffStream->Ascend( &ckCue, 0 );
	}
CUE_EXIT:

	if( SUCCEEDED( hr ) )
	{
		// Save switch markers, if there are any
		// Get the position of the first start marker
		pos = lstSwitchMarkersToSave.GetHeadPosition();

		// If there are any start markers, save them
		if( pos )
		{
			// Create a LIST chunk to store the Marker data
			MMCKINFO ckSwitch;
			ckSwitch.ckid = DMPROD_FOURCC_VALIDSTART_CHUNK;
			if( pIRiffStream->CreateChunk( &ckSwitch, 0 ) != 0 )
			{
				// If unable to create the chunk, return E_FAIL
				hr = E_FAIL;
				goto SWITCH_EXIT;
			}

			// Check the amount of data written
			DWORD dwBytesWritten;

			// Write out the size of the structure
			DWORD dwStructureSize = sizeof( DMPROD_IO_VALID_START );
			hr = pIStream->Write( &dwStructureSize, sizeof(DWORD), &dwBytesWritten );
			if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
			{
				// Handle I/O errors by returning an error code
				hr = E_FAIL;
				goto SWITCH_ON_ERROR;
			}

			// The structure to save
			DMPROD_IO_VALID_START oValidStart;

			// Iterate through the Marker list
			while( pos )
			{
				// Get a pointer to each item
				CMarkerItem* pMarkerItem = lstSwitchMarkersToSave.GetNext( pos );

				// Initialize the structure
				ZeroMemory( &oValidStart, sizeof(DMPROD_IO_VALID_START) );

				// Copy the time to the structure
				oValidStart.mtGrid = pMarkerItem->m_mtTime;
				oValidStart.mtTick = pMarkerItem->m_lTick;

				// Write the structure out to the stream
				hr = pIStream->Write( &oValidStart, sizeof(DMPROD_IO_VALID_START), &dwBytesWritten );
				if( FAILED( hr ) || dwBytesWritten != sizeof(DMPROD_IO_VALID_START) )
				{
					// Handle I/O errors by return an error code
					hr = E_FAIL;
					goto SWITCH_ON_ERROR;
				}
			}

SWITCH_ON_ERROR:
			// Ascend out of the start marker chunk.
			pIRiffStream->Ascend( &ckSwitch, 0 );
		}
	}
SWITCH_EXIT:

	// Ascend out of the main LIST chunk
	pIRiffStream->Ascend( &ckMain, 0 );


ON_ERROR:
	// Release our pointer to the IDMUSProdRIFFStream
	if( pIRiffStream )
	{
		pIRiffStream->Release();
		// Not necessary
		//pIRiffStream = NULL;
	}

	// Empty the temporary lists of items
	EmptyMarkerList(lstCueMarkersToSave);
	EmptyMarkerList(lstSwitchMarkersToSave);

	// Return whether or not the save succeeded
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// NormalizeMarkerList

void NormalizeMarkerList( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, CTypedPtrList<CPtrList, CMarkerItem*>& list, long lGridOffset )
{
	// Iterate through the list of items
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		CMarkerItem* pMarker = list.GetNext(pos);

		// Convert the start time to a number of grids
		long lGrid;
		MeasureBeatGridToGrids( pITimeline, dwGroupBits, 0, pMarker->m_lMeasure, pMarker->m_lBeat, pMarker->m_lGrid, lGrid );

		// This method only called for drag/drop and cut/copy/paste
		// so it is safe to mess with the values that are stored in time fields

		// Use m_mtTime to store grid offset 
		pMarker->m_mtTime = lGrid - lGridOffset;

		// Clear Measure and Beat and Grid fields, but keep tick field the same
		pMarker->m_lMeasure = 0;
		pMarker->m_lBeat = 0;
		pMarker->m_lGrid = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// SaveMarkerList

HRESULT SaveMarkerList( CTypedPtrList<CPtrList, CMarkerItem*>& lstCueMarkers, CTypedPtrList<CPtrList, CMarkerItem*>& lstSwitchMarkers, IStream* pIStream )
{
	// Now, finally save ourself
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Allocate an IDMUSProdRIFFStream from the IStream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Create the main LIST chunk
	MMCKINFO ckMain;
	ckMain.fccType = DMUS_FOURCC_MARKERTRACK_LIST;
	if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save cue markers, if there are any
	hr = SaveCueMarkers( pIRiffStream, lstCueMarkers );

	if( SUCCEEDED( hr ) )
	{
		// Save switch markers, if there are any
		hr = SaveSwitchMarkers( pIRiffStream, lstSwitchMarkers );
	}

	// Ascend out of the main LIST chunk
	pIRiffStream->Ascend( &ckMain, 0 );


ON_ERROR:
	// Release our pointer to the IDMUSProdRIFFStream
	pIRiffStream->Release();

	// Return the success/failure code
    return hr;
}

HRESULT LoadCopiedMarkerList( CTypedPtrList<CPtrList, CMarkerItem*>& lstCueMarkers, CTypedPtrList<CPtrList, CMarkerItem*>& lstSwitchMarkers, IStream* pIStream )
{
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Try and allocate a RIFF stream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Variables used when loading the Marker track
	MMCKINFO	ckList;
	MMCKINFO	ck;
	DWORD		dwByteCount;

	// Interate through every chunk in the stream
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
		case FOURCC_LIST:
			// Only look into LIST chunks
			switch( ck.fccType )
			{
			case DMPROD_FOURCC_MARKERTRACK_LIST:
				// Found the Marker list chunk - descend into it
				while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
				{
					switch( ckList.ckid )
					{
					case DMPROD_FOURCC_VALIDSTART_CHUNK:
					case DMPROD_FOURCC_PLAYMARKER_CHUNK:
						// Found the start time or play marker chunk - read it
						{
							// Check that we can read in the structure size
							if( ckList.cksize < sizeof(DWORD) )
							{
								hr = E_FAIL;
								goto ON_ERROR;
							}

							// Keep a count of how much data is left in the chunk
							long lBytesLeft = ckList.cksize;

							// Read in the size of the start time structures
							DWORD dwStructureSize;
							hr = pIStream->Read( &dwStructureSize, sizeof(DWORD), &dwByteCount );

							// Handle any I/O error by returning a failure code
							if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
							{
								hr = E_FAIL;
								goto ON_ERROR;
							}

							// Check that the structure size is correct
							switch( ckList.ckid )
							{
							case DMPROD_FOURCC_VALIDSTART_CHUNK:
								if( dwStructureSize != sizeof(DMPROD_IO_VALID_START) )
								{
									hr = E_FAIL;
									goto ON_ERROR;
								}
								break;

							case DMPROD_FOURCC_PLAYMARKER_CHUNK:
								if( dwStructureSize != sizeof(DMPROD_IO_PLAY_MARKER) )
								{
									hr = E_FAIL;
									goto ON_ERROR;
								}
								break;
							}

							// Subtract off the amount of data we read
							lBytesLeft -= dwByteCount;

							// The structures to read
							DMPROD_IO_VALID_START iStart = {0};
							DMPROD_IO_PLAY_MARKER iPlay = {0};

							// Continue while there is still data in the chunk to read
							while( lBytesLeft >= (signed)dwStructureSize )
							{
								switch( ckList.ckid )
								{
								case DMPROD_FOURCC_VALIDSTART_CHUNK:
									// Initialize the structure
									ZeroMemory( &iStart, sizeof(DMPROD_IO_VALID_START) );

									// Read in an item structure
									hr = pIStream->Read( &iStart, dwStructureSize, &dwByteCount );
									break;

								case DMPROD_FOURCC_PLAYMARKER_CHUNK:
									// Initialize the structure
									ZeroMemory( &iPlay, sizeof(DMPROD_IO_PLAY_MARKER) );

									// Read in an item structure
									hr = pIStream->Read( &iPlay, dwStructureSize, &dwByteCount );
									break;
								}

								// Handle any I/O error by returning a failure code
								if( FAILED( hr ) || dwByteCount != dwStructureSize )
								{
									hr = E_FAIL;
									goto ON_ERROR;
								}

								// Subtract off the amount we read in
								lBytesLeft -= dwStructureSize;

								// Create a new item
								CMarkerItem* pItem = new CMarkerItem;

								// Double-check that the memory was allocated
								if ( pItem == NULL )
								{
									hr = E_OUTOFMEMORY;
									goto ON_ERROR;
								}

								switch( ckList.ckid )
								{
								case DMPROD_FOURCC_VALIDSTART_CHUNK:
									// Initialize the item
									pItem->m_mtTime = iStart.mtGrid;
									pItem->m_lTick = iStart.mtTick;
									pItem->m_typeMarker = MARKER_SWITCH;

									// Add the item to the list of cue points
									lstSwitchMarkers.AddTail( pItem );
									break;

								case DMPROD_FOURCC_PLAYMARKER_CHUNK:
									// Initialize the item
									pItem->m_mtTime = iPlay.mtGrid;
									pItem->m_lTick = iPlay.mtTick;
									pItem->m_typeMarker = MARKER_CUE;

									// Add the item to the list of cue points
									lstCueMarkers.AddTail( pItem );
									break;
								}
							}
						}
						break;
					}

					// Ascend out of the chunk in the Marker list chunk
					pIRiffStream->Ascend( &ckList, 0 );
				}
				break;
			}
		}

		// Ascend out of the main chunk
		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
	// Release the RIFF stream pointer
	pIRiffStream->Release();

	// Return the success/failure status
	return hr;
}

HRESULT LoadMarkerList( CTypedPtrList<CPtrList, CMarkerItem*>& lstCueMarkers, CTypedPtrList<CPtrList, CMarkerItem*>& lstSwitchMarkers, IStream* pIStream )
{
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Try and allocate a RIFF stream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Variables used when loading the Marker track
	MMCKINFO	ckList;
	MMCKINFO	ck;
	DWORD		dwByteCount;

	// Interate through every chunk in the stream
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
		case FOURCC_LIST:
			// Only look into LIST chunks
			switch( ck.fccType )
			{
			case DMUS_FOURCC_MARKERTRACK_LIST:
				// Found the Marker list chunk - descend into it
				while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
				{
					switch( ckList.ckid )
					{
					case DMUS_FOURCC_VALIDSTART_CHUNK:
					case DMUS_FOURCC_PLAYMARKER_CHUNK:
						// Found the start time or play marker chunk - read it
						{
							// Check that we can read in the structure size
							if( ckList.cksize < sizeof(DWORD) )
							{
								hr = E_FAIL;
								goto ON_ERROR;
							}

							// Keep a count of how much data is left in the chunk
							long lBytesLeft = ckList.cksize;

							// Read in the size of the start time structures
							DWORD dwStructureSize;
							hr = pIStream->Read( &dwStructureSize, sizeof(DWORD), &dwByteCount );

							// Handle any I/O error by returning a failure code
							if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
							{
								hr = E_FAIL;
								goto ON_ERROR;
							}

							// Subtract off the amount of data we read
							lBytesLeft -= dwByteCount;

							DWORD dwByteSkip = 0;
							switch( ckList.ckid )
							{
							case DMUS_FOURCC_VALIDSTART_CHUNK:
								// Compute the number of bytes to skip (if any)
								dwByteSkip = max( 0, sizeof(DMUS_IO_VALID_START) - dwStructureSize );

								// Compute the number of bytes to read
								dwStructureSize = min( sizeof(DMUS_IO_VALID_START), dwStructureSize );
								break;

							case DMUS_FOURCC_PLAYMARKER_CHUNK:
								// Compute the number of bytes to skip (if any)
								dwByteSkip = max( 0, sizeof(DMUS_IO_PLAY_MARKER) - dwStructureSize );

								// Compute the number of bytes to read
								dwStructureSize = min( sizeof(DMUS_IO_PLAY_MARKER), dwStructureSize );
								break;
							}

							// The structures to read
							DMUS_IO_VALID_START iStart = {0};
							DMUS_IO_PLAY_MARKER iPlay = {0};

							// Continue while there is still data in the chunk to read
							while( lBytesLeft >= (signed)dwStructureSize )
							{
								switch( ckList.ckid )
								{
								case DMUS_FOURCC_VALIDSTART_CHUNK:
									// Initialize the structure
									ZeroMemory( &iStart, sizeof(DMUS_IO_VALID_START) );

									// Read in an item structure
									hr = pIStream->Read( &iStart, dwStructureSize, &dwByteCount );
									break;

								case DMUS_FOURCC_PLAYMARKER_CHUNK:
									// Initialize the structure
									ZeroMemory( &iPlay, sizeof(DMUS_IO_PLAY_MARKER) );

									// Read in an item structure
									hr = pIStream->Read( &iPlay, dwStructureSize, &dwByteCount );
									break;
								}

								// Handle any I/O error by returning a failure code
								if( FAILED( hr ) || dwByteCount != dwStructureSize )
								{
									hr = E_FAIL;
									goto ON_ERROR;
								}

								// Skip data, if necessary
								hr = StreamSeek( pIStream, dwByteSkip, SEEK_CUR );

								// Handle any I/O error by returning a failure code
								if( FAILED( hr ) )
								{
									hr = E_FAIL;
									goto ON_ERROR;
								}

								// Subtract off the amount we read in
								lBytesLeft -= dwByteSkip + dwStructureSize;

								// Create a new item
								CMarkerItem* pItem = new CMarkerItem;

								// Double-check that the memory was allocated
								if ( pItem == NULL )
								{
									hr = E_OUTOFMEMORY;
									goto ON_ERROR;
								}

								switch( ckList.ckid )
								{
								case DMUS_FOURCC_VALIDSTART_CHUNK:
									// Initialize the item
									pItem->m_mtTime = iStart.mtTime;
									pItem->m_typeMarker = MARKER_SWITCH;

									// Add the item to the list of cue points
									lstSwitchMarkers.AddTail( pItem );
									break;

								case DMUS_FOURCC_PLAYMARKER_CHUNK:
									// Initialize the item
									pItem->m_mtTime = iPlay.mtTime;
									pItem->m_typeMarker = MARKER_CUE;

									// Add the item to the list of cue points
									lstCueMarkers.AddTail( pItem );
									break;
								}
							}
						}
						break;
					}

					// Ascend out of the chunk in the Marker list chunk
					pIRiffStream->Ascend( &ckList, 0 );
				}
				break;
			}
		}

		// Ascend out of the main chunk
		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
	// Release the RIFF stream pointer
	pIRiffStream->Release();

	// Return the success/failure status
	return hr;
}

void EmptyMarkerList( CTypedPtrList<CPtrList, CMarkerItem*>& lstMarkers )
{
	// Remove and delete all the CMarkerItems from the given list
	while( !lstMarkers.IsEmpty() )
	{
		delete lstMarkers.RemoveHead();
	}
}

HRESULT GetBoundariesOfMarkers( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, long &lStartTime, long &lEndTime, CTypedPtrList<CPtrList, CMarkerItem*>& lstMarkers)
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
	if( lstMarkers.IsEmpty() )
	{
		return S_FALSE;
	}

	// Initialize our return value to S_OK
	HRESULT hr = S_OK;
	MUSIC_TIME mtTime;

	// Iterate through the list of items
	POSITION pos = lstMarkers.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		CMarkerItem* pItem = lstMarkers.GetNext(pos);

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
		long lMeasure, lBeat, lGrid;
		CMusicTimeGridConverter cmgt = lEndTime;
		hr = cmgt.GetMeasureBeatGrid( lMeasure, lBeat, lGrid, pTimeline, dwGroupBits );

		// If the conversion failed, return the failure code
		if( FAILED( hr ) )
		{
			return hr;
		}

		// Increment the grid value
		lGrid++;

		// Convert from measure and beat back to clocks
		hr = cmgt.SetTime( lMeasure, lBeat, lGrid, pTimeline, dwGroupBits );

		// If the conversion failed, return the failure code
		if( FAILED( hr ) )
		{
			return hr;
		}

		// Set the end time to one tick before the next beat
		lEndTime = cmgt - 1;
	}

	// Return the success or failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// SaveCueMarkers

HRESULT SaveCueMarkers( IDMUSProdRIFFStream* pIRiffStream, CTypedPtrList<CPtrList, CMarkerItem*>& lstMarkers )
{
	// Get the position of the first play marker
	POSITION pos = lstMarkers.GetHeadPosition();

	// Result to return
	HRESULT hr = S_FALSE;

	// If there are any play markers, save them
	if( pos )
	{
		// Create a LIST chunk to store the Marker data
		MMCKINFO ckMain;
		ckMain.ckid = DMUS_FOURCC_PLAYMARKER_CHUNK;
		if( pIRiffStream->CreateChunk( &ckMain, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			return E_FAIL;
		}

		// Get a pointer to the IStream
		IStream *pIStream = pIRiffStream->GetStream();

		// Check the amount of data written
		DWORD dwBytesWritten;

		// Write out the size of the structure
		DWORD dwStructureSize = sizeof( DMUS_IO_PLAY_MARKER );
		hr = pIStream->Write( &dwStructureSize, sizeof(DWORD), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
		{
			// Handle I/O errors by returning an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// The structure to save
		DMUS_IO_PLAY_MARKER oPlayMarker;

		// Iterate through the Marker list
		while( pos )
		{
			// Get a pointer to each item
			CMarkerItem* pMarkerItem = lstMarkers.GetNext( pos );

			// Initialize the structure
			ZeroMemory( &oPlayMarker, sizeof(DMUS_IO_PLAY_MARKER) );

			// Copy the time to the structure
			oPlayMarker.mtTime = pMarkerItem->m_mtTime;

			// Write the structure out to the stream
			hr = pIStream->Write( &oPlayMarker, sizeof(DMUS_IO_PLAY_MARKER), &dwBytesWritten );
			if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_PLAY_MARKER) )
			{
				// Handle I/O errors by return an error code
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

ON_ERROR:
		pIStream->Release();

		// Ascend out of the play marker chunk.
		pIRiffStream->Ascend( &ckMain, 0 );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// SaveSwitchMarkers

HRESULT SaveSwitchMarkers( IDMUSProdRIFFStream* pIRiffStream, CTypedPtrList<CPtrList, CMarkerItem*>& lstMarkers )
{
	// Get the position of the first start marker
	POSITION pos = lstMarkers.GetHeadPosition();

	// Result to return
	HRESULT hr = S_FALSE;

	// If there are any start markers, save them
	if( pos )
	{
		// Create a LIST chunk to store the Marker data
		MMCKINFO ckMain;
		ckMain.ckid = DMUS_FOURCC_VALIDSTART_CHUNK;
		if( pIRiffStream->CreateChunk( &ckMain, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			return E_FAIL;
		}

		// Get a pointer to the IStream
		IStream *pIStream = pIRiffStream->GetStream();

		// Check the amount of data written
		DWORD dwBytesWritten;

		// Write out the size of the structure
		DWORD dwStructureSize = sizeof( DMUS_IO_VALID_START );
		hr = pIStream->Write( &dwStructureSize, sizeof(DWORD), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
		{
			// Handle I/O errors by returning an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// The structure to save
		DMUS_IO_VALID_START oValidStart;

		// Iterate through the Marker list
		while( pos )
		{
			// Get a pointer to each item
			CMarkerItem* pMarkerItem = lstMarkers.GetNext( pos );

			// Initialize the structure
			ZeroMemory( &oValidStart, sizeof(DMUS_IO_VALID_START) );

			// Copy the time to the structure
			oValidStart.mtTime = pMarkerItem->m_mtTime;

			// Write the structure out to the stream
			hr = pIStream->Write( &oValidStart, sizeof(DMUS_IO_VALID_START), &dwBytesWritten );
			if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_VALID_START) )
			{
				// Handle I/O errors by return an error code
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

ON_ERROR:
		pIStream->Release();

		// Ascend out of the start marker chunk.
		pIRiffStream->Ascend( &ckMain, 0 );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::ClocksToMeasureBeatGrid

HRESULT CMarkerMgr::ClocksToMeasureBeatGrid( MUSIC_TIME mtTime,
										    long* plMeasure, long* plBeat, long *plGrid )
{
	ASSERT( plMeasure != NULL );
	ASSERT( plBeat != NULL );
	ASSERT( plGrid != NULL );

	// Find out which measure we're in
	if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, plMeasure, NULL ) ) )
	{
		// Find the time of the start of this measure
		long lTime;
		if( SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, *plMeasure, 0, &lTime ) ) )
		{
			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE ts;
			if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, lTime, NULL, &ts ) ) )
			{
				// Compute the number of clocks in a beat and a grid
				long lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
				long lGridClocks = lBeatClocks / ts.wGridsPerBeat;

				// Convert mtTime into an offset from the start of this measure
				mtTime -= lTime;

				*plBeat = mtTime / lBeatClocks;

				mtTime %= lBeatClocks;
				*plGrid = mtTime / lGridClocks;

				return S_OK;
			}
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::ClocksToMeasureBeatGridTick

HRESULT CMarkerMgr::ClocksToMeasureBeatGridTick( MUSIC_TIME mtTime,
										    long* plMeasure, long* plBeat, long *plGrid, long* plTick )
{
	ASSERT( plMeasure != NULL );
	ASSERT( plBeat != NULL );
	ASSERT( plGrid != NULL );
	ASSERT( plTick != NULL );

	// Find out which measure we're in
	if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, plMeasure, NULL ) ) )
	{
		// Find the time of the start of this measure
		long lTime;
		if( SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, *plMeasure, 0, &lTime ) ) )
		{
			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE ts;
			if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, lTime, NULL, &ts ) ) )
			{
				// Compute the number of clocks in a beat and a grid
				long lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
				long lGridClocks = lBeatClocks / ts.wGridsPerBeat;

				// BUGBUG: This doesn't work correctly if lGridClocks * wGridsPerBeat != lBeatClocks
				// Any notes in the second half of the last grid in a beat will have the WRONG tick
				// (since there are more ticks in the last grid of the beat than in the other grids).

				// Check if the time is in the second half of the grid - if so bump up the start time by one grid
				long lTempTime = (mtTime - lTime) % lBeatClocks;
				BOOL fAddedGrid = FALSE;
				if( (lTempTime % lGridClocks) > ( lGridClocks - (lGridClocks / 2) - 1) )
				{
					mtTime += lGridClocks;
					fAddedGrid = TRUE;
				}

				// Convert mtTime into an offset from the start of this measure
				mtTime -= lTime;

				// If mtTime is as long as or greater than a measure, we must have been in the second half of a grid
				// that was the very last grid in the measure
				if( mtTime >= lBeatClocks * ts.bBeatsPerMeasure )
				{
					ASSERT( fAddedGrid );
					mtTime -= lBeatClocks * ts.bBeatsPerMeasure;
					(*plMeasure)++;
				}
				
				*plBeat = mtTime / lBeatClocks;

				mtTime %= lBeatClocks;
				*plGrid = mtTime / lGridClocks;

				*plTick = mtTime % lGridClocks;
				if( fAddedGrid )
				{
					*plTick -= lGridClocks;
				}

				return S_OK;
			}
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::MeasureBeatGridTickToClocks

HRESULT CMarkerMgr::MeasureBeatGridTickToClocks( long lMeasure, long lBeat, long lGrid,
												 long lTick, MUSIC_TIME* pmtTime )
{
	ASSERT( pmtTime != NULL );

	if( m_pTimeline == NULL )
	{
		return S_FALSE;
	}

	// Find the time of the start of this measure
	long lTime;
	if( SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, 0, &lTime ) ) )
	{
		// Get the TimeSig for this measure
		DMUS_TIMESIGNATURE ts;
		if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, lTime, NULL, &ts ) ) )
		{
			// Compute the number of clocks per beat and per grid
			long lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
			long lGridClocks = lBeatClocks / ts.wGridsPerBeat;

			// Calculate the clock value
			*pmtTime = lTime + lBeatClocks * lBeat + lGridClocks * lGrid + lTick;

			return S_OK;
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerMgr::CycleMarkers

HRESULT CMarkerMgr::CycleMarkers( long lXPos, long lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	long lMeasure, lBeat, lGrid;
	HRESULT hr = m_pTimeline->PositionToClocks( lXPos, &lXPos );
	if( SUCCEEDED( hr ) )
	{
		hr = ClocksToMeasureBeatGrid( lXPos, &lMeasure, &lBeat, &lGrid );

		if( SUCCEEDED ( hr ) ) 
		{
			CTypedPtrList<CPtrList, CMarkerItem*> *plstMarkers;
			if( lYPos < DEFAULT_STRIP_HEIGHT / 2 )
			{
				plstMarkers = &m_lstCueMarkers;
			}
			else
			{
				plstMarkers = &m_lstSwitchMarkers;
			}

			CMarkerItem* pFirstItem = NULL;
			CMarkerItem* pSecondItem = NULL;
			CMarkerItem* pItem;

			hr = E_FAIL;

			POSITION pos = plstMarkers->GetHeadPosition();
			while( pos )
			{
				pItem = plstMarkers->GetNext( pos );

				if( pItem->m_lMeasure == lMeasure
				&&  pItem->m_lBeat == lBeat
				&&	pItem->m_lGrid == lGrid )
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
							// Cycle to next Routine if on same measure/beat
							CMarkerItem* pNextItem = plstMarkers->GetNext( pos );

							if( pNextItem->m_lMeasure == lMeasure
							&&  pNextItem->m_lBeat == lBeat
							&&	pNextItem->m_lGrid == lGrid )
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
	}

	if( SUCCEEDED ( hr ) )
	{
		// Update the selection regions to include just this selected item
		m_pMarkerStrip->SelectRegionsFromSelectedMarkers();

		// Redraw the Marker strip
		m_pTimeline->StripInvalidateRect( m_pMarkerStrip, NULL, TRUE );

		// Update the property page
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// MeasureBeatGridToGrids

HRESULT MeasureBeatGridToGrids( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, DWORD dwIndex, long lMeasure, long lBeat, long lGrid, long &lNumGrids )
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
	lNumGrids = 0;

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
			// Just compute the number of grids to add and break out of the loop
			lNumGrids += lMeasure * TimeSig.bBeatsPerMeasure * TimeSig.wGridsPerBeat;
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
				// Just compute the number of grids to add and break out of the loop
				lNumGrids += lMeasure * TimeSig.bBeatsPerMeasure * TimeSig.wGridsPerBeat;
				break;
			}
			else
			{
				// Compute when to look for the next time signature
				mtTSCur += lMeasureClocks * lTmpMeasures;

				// Compute the number of beats to add
				lNumGrids += lTmpMeasures * TimeSig.bBeatsPerMeasure * TimeSig.wGridsPerBeat;

				// Update the number of measures we have left to traverse
				lMeasure -= lTmpMeasures;
			}
		}
	}
	while( lMeasure > 0 );

	// We've computed all the grids from the lMeasure paramter - now add lBeat
	lNumGrids += lBeat * TimeSig.wGridsPerBeat;

	// Now add lGrid
	lNumGrids += lGrid;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// GridsToMeasureBeatGrid

HRESULT GridsToMeasureBeatGrid( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, DWORD dwIndex, long lNumGrids, long &lMeasure, long &lBeat, long &lGrid )
{
	// Validate parameters
	ASSERT( pITimeline );
	ASSERT( dwGroupBits );
	ASSERT( lNumGrids >= 0 );

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
	lGrid = 0;
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
			lMeasure += lNumGrids / (TimeSig.bBeatsPerMeasure * TimeSig.wGridsPerBeat);
			lNumGrids %= (TimeSig.bBeatsPerMeasure * TimeSig.wGridsPerBeat);
			break;
		}
		// Otherwise it's more complicated
		else
		{
			// Compute the number of clocks in a beat
			const long lBeatClocks = NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );

			// If the next time signature is after the time we're looking for
			if( lNumGrids < TimeSig.wGridsPerBeat * (mtTSNext / lBeatClocks) )
			{
				// Add the number of complete measures between here and there
				lMeasure += lNumGrids / (TimeSig.bBeatsPerMeasure * TimeSig.wGridsPerBeat);

				// lNumGrids now stores an offset from the beginning of the measure
				lNumGrids %= (TimeSig.bBeatsPerMeasure * TimeSig.wGridsPerBeat);
				break;
			}
			// The next time signature is before the time we're looking for
			else
			{
				// Compute how many complete measures there are between now and the next Time signature
				long lMeasureDiff= mtTSNext / (TimeSig.bBeatsPerMeasure * lBeatClocks);

				// Add them to lMeasure
				lMeasure += lMeasureDiff;

				// Subtract off the number of grids between mtTSCur and mtTSNext
				lNumGrids -= lMeasureDiff * (TimeSig.bBeatsPerMeasure * TimeSig.wGridsPerBeat);

				// Change lMeasureDiff from measures to clocks
				lMeasureDiff *= TimeSig.bBeatsPerMeasure * lBeatClocks;

				// Add the clocks of the measures between mtTSCur and mtTSNext to mtTSCur
				mtTSCur += lMeasureDiff;
			}
		}
	}
	// While the grids left is greater than 0
	while ( lNumGrids > 0 );

	// Any leftover grids are first assigned to lBeat
	lBeat = lNumGrids / TimeSig.wGridsPerBeat;

	// The rest of the grids are stored in lGrid
	lGrid = lNumGrids % TimeSig.wGridsPerBeat;

	return S_OK;
}
