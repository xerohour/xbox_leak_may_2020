// MarkerStrip.cpp : Implementation of CMarkerStrip
#include "stdafx.h"
#include "MarkerItem.h"
#include "MarkerStripMgr.h"
#include "MarkerMgr.h"
#include "PropPageMgr.h"
#include "GroupBitsPPG.h"
#include <RiffStrm.h>
#include "BaseMgr.h"
#include "SegmentIO.h"
#include "GrayOutRect.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This sets the color of the markers
#define COLOR_CUE		RGB(0, 100, 255)
#define COLOR_SWITCH	RGB(0, 255, 0)
#define COLOR_CUE_SELECTED		RGB(255, 0, 0)
#define COLOR_SWITCH_SELECTED	RGB(255, 0, 0)

const DWORD g_dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
const DWORD g_dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;

/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip constructor/destructor

CMarkerStrip::CMarkerStrip( CMarkerMgr* pMarkerMgr ) : CBaseStrip( pMarkerMgr )
{
	// Validate the pointer to our strip manager
	ASSERT( pMarkerMgr );
	if ( pMarkerMgr == NULL )
	{
		return;
	}

	// Set our pointer to our strip manager
	m_pMarkerMgr = pMarkerMgr;

	// Initialize our clipboard format to 0
	m_cfMarkerList = 0;

	// Initialize our state variables to false
	m_fLeftMouseDown = false;

	// Initialize the Marker to toggle (when CTRL-clicking) to NULL
	m_pMarkerItemToToggle = NULL;

	m_pSelectedCueGridRegions = NULL;
	m_pSelectedSwitchGridRegions = NULL;
	m_lYPos = 0;
	m_typeShiftAnchor = MARKER_CUE;
}

CMarkerStrip::~CMarkerStrip()
{
	// Clear our pointer to our strip manager
	ASSERT( m_pMarkerMgr );
	if ( m_pMarkerMgr )
	{
		m_pMarkerMgr = NULL;
	}

	if(m_pSelectedCueGridRegions)
	{
		delete m_pSelectedCueGridRegions;
		m_pSelectedCueGridRegions = NULL;
	}

	if(m_pSelectedSwitchGridRegions)
	{
		delete m_pSelectedSwitchGridRegions;
		m_pSelectedSwitchGridRegions = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::GetTopItemFromMeasureBeatGrid

CMarkerItem* CMarkerStrip::GetTopItemFromMeasureBeatGrid( MARKER_TYPE typeMarker, long lMeasure, long lBeat, long lGrid )
{
	// Get a pointer to the list to look through
	CTypedPtrList<CPtrList, CMarkerItem*> *plstMarkers;
	switch( typeMarker )
	{
	case MARKER_SWITCH:
		plstMarkers = &m_pMarkerMgr->m_lstSwitchMarkers;
		break;

	case MARKER_CUE:
		plstMarkers = &m_pMarkerMgr->m_lstCueMarkers;
		break;

	default:
		ASSERT( FALSE );
		return NULL;
	}

	CMarkerItem* pTheItem = NULL;
	CMarkerItem* pFirstItem = NULL;
	CMarkerItem* pItem;

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

			if( pItem->m_wFlagsUI & RF_TOP_ITEM )
			{
				pTheItem = pItem;
				break;
			}
		}

		if( pItem->m_lMeasure > lMeasure )
		{
			break;
		}
	}

	if( pTheItem == NULL )
	{
		pTheItem = pFirstItem;
	}

	return pTheItem;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// GetNextSelectedMarker - helper method for Draw()

void GetNextSelectedMarker( CTypedPtrList<CPtrList, CMarkerItem*>& list, POSITION &posMarker )
{
	// Note that if the item at posMarker is selected, this method doesn't do anything useful.
	if( posMarker )
	{
		// Save the current position
		POSITION posToSave = posMarker;

		// Check if we have not run off the end of the list, and if the currently item is unselected
		while( posMarker && !list.GetNext( posMarker )->m_fSelected )
		{
			// Current item is unselected, save the position of the next item
			posToSave = posMarker;
		}

		// Check if we found a selected item
		if( posToSave )
		{
			// Save the position of the selected item
			posMarker = posToSave;
		}
		// Otherwise both posToSave and posMarker are NULL
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::AdjustTopItem - helper method for Draw()

void CMarkerStrip::AdjustTopItem( MARKER_TYPE typeMarker, POSITION pos, long lCurrentMeasure, long lCurrentBeat, long lCurrentGrid )
{
	// Get a pointer to the list to look through
	CTypedPtrList<CPtrList, CMarkerItem*> *plstMarkers;
	switch( typeMarker )
	{
	case MARKER_SWITCH:
		plstMarkers = &m_pMarkerMgr->m_lstSwitchMarkers;
		break;

	case MARKER_CUE:
		plstMarkers = &m_pMarkerMgr->m_lstCueMarkers;
		break;

	default:
		ASSERT( FALSE );
		return;
	}

	CMarkerItem* pFirstSelectedItem = NULL;
	CMarkerItem* pTopItem = NULL;
	CMarkerItem* pItem;

	while( pos )
	{
		pItem = plstMarkers->GetNext( pos );

		if( pItem->m_lMeasure != lCurrentMeasure
		||  pItem->m_lBeat != lCurrentBeat
		||	pItem->m_lGrid != lCurrentGrid )
		{
			break;
		}

		if( pItem->m_fSelected )
		{
			if( pFirstSelectedItem == NULL )
			{
				pFirstSelectedItem = pItem;
			}
		}

		if( pItem->m_wFlagsUI & RF_TOP_ITEM )
		{
			if( pTopItem == NULL )
			{
				pTopItem = pItem;
			}
			else
			{
				// Can only have one top item
				pItem->m_wFlagsUI &= ~RF_TOP_ITEM;
			}
		}
	}

	if( pTopItem )
	{
		if( pTopItem->m_fSelected == FALSE )
		{
			if( pFirstSelectedItem )
			{
				// pTopItem is NOT selected so it shouldn't be the top item
				pTopItem->m_wFlagsUI &= ~RF_TOP_ITEM;
						
				// pFirstSelectedItem IS selected so make it the top item
				pFirstSelectedItem->m_wFlagsUI |= RF_TOP_ITEM;
			}
		}
	}
	else if( pFirstSelectedItem )
	{
		// Make the selected item the top item
		pFirstSelectedItem->m_wFlagsUI |= RF_TOP_ITEM;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::GetNextSelectedTopItem - helper method for Draw()

CMarkerItem* CMarkerStrip::GetNextSelectedTopItem( MARKER_TYPE typeMarker, POSITION pos, long lCurrentMeasure, long lCurrentBeat, long lCurrentGrid )
{
	// Get a pointer to the list to look through
	CTypedPtrList<CPtrList, CMarkerItem*> *plstMarkers;
	switch( typeMarker )
	{
	case MARKER_SWITCH:
		plstMarkers = &m_pMarkerMgr->m_lstSwitchMarkers;
		break;

	case MARKER_CUE:
		plstMarkers = &m_pMarkerMgr->m_lstCueMarkers;
		break;

	default:
		ASSERT( FALSE );
		return NULL;
	}

	CMarkerItem* pNextItem = NULL;
	CMarkerItem* pItem;

	while( pos )
	{
		pItem = plstMarkers->GetNext( pos );

		if( pItem->m_fSelected )
		{
			if( pItem->m_lMeasure != lCurrentMeasure
			||  pItem->m_lBeat != lCurrentBeat
			||	pItem->m_lGrid != lCurrentGrid )
			{
				pNextItem = GetTopItemFromMeasureBeatGrid( typeMarker, pItem->m_lMeasure, pItem->m_lBeat, pItem->m_lGrid );
				ASSERT( pNextItem != NULL );	// Should not happen!
				break;
			}
		}
	}

	return pNextItem;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::GetNextTopItem - helper method for Draw()

CMarkerItem* CMarkerStrip::GetNextTopItem( MARKER_TYPE typeMarker, POSITION pos, long lCurrentMeasure, long lCurrentBeat, long lCurrentGrid )
{
	// Get a pointer to the list to look through
	CTypedPtrList<CPtrList, CMarkerItem*> *plstMarkers;
	switch( typeMarker )
	{
	case MARKER_SWITCH:
		plstMarkers = &m_pMarkerMgr->m_lstSwitchMarkers;
		break;

	case MARKER_CUE:
		plstMarkers = &m_pMarkerMgr->m_lstCueMarkers;
		break;

	default:
		ASSERT( FALSE );
		return NULL;
	}

	CMarkerItem* pNextItem = NULL;
	CMarkerItem* pItem;

	while( pos )
	{
		pItem = plstMarkers->GetNext( pos );

		if( pItem->m_lMeasure != lCurrentMeasure
		||  pItem->m_lBeat != lCurrentBeat
		||	pItem->m_lGrid != lCurrentGrid )
		{
			pNextItem = GetTopItemFromMeasureBeatGrid( typeMarker, pItem->m_lMeasure, pItem->m_lBeat, pItem->m_lGrid );
			ASSERT( pNextItem != NULL );	// Should not happen!
			break;
		}
	}

	return pNextItem;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::Draw

HRESULT	STDMETHODCALLTYPE CMarkerStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(sv);

	// Verify that our timeline pointer is valid
	if( m_pMarkerMgr == NULL
	||	m_pMarkerMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Draw Measure, Beat, and Grid lines in our strip
	m_pMarkerMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT_GRID, m_pMarkerMgr->m_dwGroupBits, 0, lXOffset );

	// Get our clipping rectange
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// Initialize the top and bottom of the highlight rectangle
	// (it is always the entire strip height).
	RECT rectHighlight;
	rectHighlight.top = 0;
	rectHighlight.bottom = DEFAULT_STRIP_HEIGHT;

	// Find the first time that we are asked to draw
	long lStartTime;
	m_pMarkerMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &lStartTime );

	////
	// Draw the unselected Markers
	DrawMarkers( false, MARKER_CUE, lXOffset, rectClip, hDC );
	DrawMarkers( false, MARKER_SWITCH, lXOffset, rectClip, hDC );

	////
	// Now, invert the selected regions
	InvertSelectedRegion( MARKER_CUE, lXOffset, rectClip, hDC );
	InvertSelectedRegion( MARKER_SWITCH, lXOffset, rectClip, hDC );

	////
	// Now, draw the selected Markers
	DrawMarkers( true, MARKER_CUE, lXOffset, rectClip, hDC );
	DrawMarkers( true, MARKER_SWITCH, lXOffset, rectClip, hDC );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::DrawMarkers

void CMarkerStrip::DrawMarkers( bool fSelected, MARKER_TYPE typeMarker, long lXOffset, RECT &rectClip, HDC hDC )
{
	// Fields to keep track of top item on current measure/beat/grid
	CMarkerItem* pTopItem = NULL;

	// Initialize the top and bottom of the highlight rectangle
	// Initialize the background color
	COLORREF crOldBGColor = ::GetBkColor( hDC );
	COLORREF crStandardColor;
	HBRUSH hbrushHatchOverlapping = ::CreateHatchBrush( HS_FDIAGONAL, crOldBGColor ); 
	RECT rectHighlight;
	switch( typeMarker )
	{
	case MARKER_SWITCH:
		rectHighlight.top = 1 + (DEFAULT_STRIP_HEIGHT / 2);
		rectHighlight.bottom = DEFAULT_STRIP_HEIGHT;
		crStandardColor = ::GetNearestColor( hDC, fSelected ? COLOR_SWITCH_SELECTED : COLOR_SWITCH);
		::SetBkColor( hDC, crStandardColor );
		break;

	case MARKER_CUE:
		rectHighlight.top = 0;
		rectHighlight.bottom = DEFAULT_STRIP_HEIGHT / 2;
		crStandardColor = ::GetNearestColor( hDC, fSelected ? COLOR_CUE_SELECTED : COLOR_CUE);
		::SetBkColor( hDC, crStandardColor );
		break;
	default:
		ASSERT( FALSE );
		return;
	}


	// Get a pointer to the list to look through
	CTypedPtrList<CPtrList, CMarkerItem*> *plstMarkers = NULL;
	switch( typeMarker )
	{
	case MARKER_SWITCH:
		plstMarkers = &m_pMarkerMgr->m_lstSwitchMarkers;
		break;

	case MARKER_CUE:
		plstMarkers = &m_pMarkerMgr->m_lstCueMarkers;
		break;
	}

	// Iterate through the list of Markers
	long lPosition;
	long lCurrentMeasure = -1;
	long lCurrentBeat = -1;
	long lCurrentGrid = -1;
	POSITION posMarker = plstMarkers->GetHeadPosition();
	while( posMarker )
	{
		// Save position
		POSITION posLast = posMarker;

		// Get a pointer to each item
		CMarkerItem* pMarkerItem = plstMarkers->GetNext( posMarker );

		if( pMarkerItem->m_lMeasure != lCurrentMeasure
		||  pMarkerItem->m_lBeat != lCurrentBeat
		||	pMarkerItem->m_lGrid != lCurrentGrid )
		{
			lCurrentMeasure = pMarkerItem->m_lMeasure;
			lCurrentBeat = pMarkerItem->m_lBeat;
			lCurrentGrid = pMarkerItem->m_lGrid;

			// Cleanup
			AdjustTopItem( typeMarker, posLast, lCurrentMeasure, lCurrentBeat, lCurrentGrid );

			// Get the "top" item on this beat
			pTopItem = GetTopItemFromMeasureBeatGrid( typeMarker, lCurrentMeasure, lCurrentBeat, lCurrentGrid );
			if( pTopItem == NULL )
			{
				ASSERT( 0 );	// Should not happen!
				continue;
			}

			// If this item's selection state is note what we want, skip it
			if( pMarkerItem->m_fSelected != fSelected )
			{
				continue;
			}

			// Convert the measure and beat of each item to a pixel position
			m_pMarkerMgr->MeasureBeatGridTickToClocks( lCurrentMeasure, lCurrentBeat, lCurrentGrid, 0, &lPosition );
			m_pMarkerMgr->m_pTimeline->ClocksToPosition( lPosition, &lPosition );

			// Offset the position by the horizontal offset,
			// and increment it by one pixel so we don't overwrite the bar or beat line
			lPosition += -lXOffset + 1;

			// Check if the left edge of the Marker text is beyond the region we're displaying
			if( lPosition > rectClip.right )
			{
				// We've gone beyond the right edge of the clipping region - break out of the loop
				break;
			}

			// Set the left edge of the highlight rectangle to the start of the text, minus the horizontal offset
			rectHighlight.left = lPosition;

			// Flag if more than one marker is on this grid
			bool fMoreThanOne = false;
			if( pTopItem != pMarkerItem )
			{
				fMoreThanOne = true;
			}
			else
			{
				if( posMarker )
				{
					POSITION posNext = posMarker;
					CMarkerItem* pNextItem = plstMarkers->GetNext( posNext );

					if( pNextItem->m_lMeasure == lCurrentMeasure
					&&  pNextItem->m_lBeat == lCurrentBeat
					&&	pNextItem->m_lGrid == lCurrentGrid )
					{
						fMoreThanOne = true;
					}
				}
			}

			// Get the start position of the next Grid
			m_pMarkerMgr->MeasureBeatGridTickToClocks( lCurrentMeasure, lCurrentBeat, lCurrentGrid + 1, 0, &rectHighlight.right );
			m_pMarkerMgr->m_pTimeline->ClocksToPosition( rectHighlight.right, &rectHighlight.right );

			// Offset it by lXOffset
			rectHighlight.right -= lXOffset;

			// Draw the item
			if( fMoreThanOne )
			{
				// More than one marker on this grid
				//::SetBkColor( hDC, crOldBGColor );
				::FillRect( hDC, &rectHighlight, hbrushHatchOverlapping );
				//::SetBkColor( hDC, crStandardColor );
			}
			else
			{
				// Only one marker on this grid
				::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectHighlight, NULL, 0, NULL);
			}
		}
	}

	if( hbrushHatchOverlapping )
	{
		::DeleteObject( hbrushHatchOverlapping );
	}

	::SetBkColor( hDC, crOldBGColor );
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::InvertSelectedRegion

void CMarkerStrip::InvertSelectedRegion( MARKER_TYPE typeMarker, long lXOffset, RECT &rectClip, HDC hDC )
{
	RECT rectHighlight;
	CListSelectedGridRegion *plstSelectedGridRegions;
	switch( typeMarker )
	{
	case MARKER_SWITCH:
		rectHighlight.top = 1 + (DEFAULT_STRIP_HEIGHT / 2);
		rectHighlight.bottom = DEFAULT_STRIP_HEIGHT;
		plstSelectedGridRegions = m_pSelectedSwitchGridRegions;
		break;

	case MARKER_CUE:
		rectHighlight.top = 0;
		rectHighlight.bottom = DEFAULT_STRIP_HEIGHT / 2;
		plstSelectedGridRegions = m_pSelectedCueGridRegions;
		break;
	default:
		return;
	}

	// Check that the selection range is in the clipping rectangle
	if( (rectClip.top > rectHighlight.bottom)
	||	(rectClip.bottom < rectHighlight.top) )
	{
		// Selection range not in the clipping rectangle.
		return;
	}

	// This ensures we don't double-invert.
	long lRightMostInvert = 0;

	// Sort the list of selected regions
	plstSelectedGridRegions->Sort();

	// Since the markers are exactly one grid long, we don't need to worry about
	// ensuring that the entirety of each selected marker is inverted.
	// That should already be taken care of by the setting of the the
	// selected regions.

	// Get the start position of the region list
	POSITION posRegion = plstSelectedGridRegions->GetHeadPosition();
	while(posRegion)
	{
		// Get a pointer to this region
		CSelectedGridRegion* psr = plstSelectedGridRegions->GetNext(posRegion);

		// Get the start and end positions for this region
		rectHighlight.left = psr->BeginPos(m_pMarkerMgr->m_pTimeline) - lXOffset;
		rectHighlight.right = psr->EndPos(m_pMarkerMgr->m_pTimeline) - lXOffset;

		// Check if we've not already inverted beyond this selection region
		if( lRightMostInvert < rectHighlight.right )
		{
			// Update the left side of the region so we don't double-invert
			rectHighlight.left = max( lRightMostInvert, rectHighlight.left );

			// Now, invert the rect
			GrayOutRect( hDC, &rectHighlight );

			// Save the new right side boundary
			lRightMostInvert = rectHighlight.right;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CMarkerStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	bool fRefresh = false;

	switch( sp )
	{
	case SP_BEGINSELECT:
	case SP_ENDSELECT:
		// Verify that we're passed an integer
		if( var.vt != VT_I4)
		{
			return E_FAIL;
		}

		// Update the appropriate member variables with the new information
		if( sp == SP_BEGINSELECT )
		{
			m_lGutterBeginSelect = V_I4( &var );
		}
		else
		{
			m_lGutterEndSelect = V_I4( &var );
		}

		// This flag will be true iff we're trying to clear the selections in all other strips
		if( m_fSelecting )
		{
			break;
		}

		// If the start time and the end time are identical, unselect everything
		if( m_lGutterBeginSelect == m_lGutterEndSelect )
		{	
			// Clear all selections
			if( m_pSelectedCueGridRegions )
			{
				fRefresh = m_pSelectedCueGridRegions->Clear();
			}
			if( m_pSelectedSwitchGridRegions )
			{
				fRefresh |= m_pSelectedSwitchGridRegions->Clear();
			}

			// Deselect all items
			if( SelectItemsInSelectedRegions() || fRefresh )
			{
				// If any items become unselected, redraw the strip
				m_pBaseMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			}
			break;
		}

		// If our gutter is selected
		if( m_bGutterSelected )
		{
			// Clear all current selections
			if( m_pSelectedCueGridRegions )
			{
				m_pSelectedCueGridRegions->Clear();
			}
			if( m_pSelectedSwitchGridRegions )
			{
				m_pSelectedSwitchGridRegions->Clear();
			}

			// Always need to refresh the display
			fRefresh = true;

			// Select all items between the new time range, and deselect all items
			// outside the new time range
			SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
		}
		else
		{
			// Our gutter is not selected
			// Clear all current selections
			if( m_pSelectedCueGridRegions )
			{
				fRefresh = m_pSelectedCueGridRegions->Clear();
			}
			if( m_pSelectedSwitchGridRegions )
			{
				fRefresh |= m_pSelectedSwitchGridRegions->Clear();
			}

			// If any items become unselected, set fRefresh to true
			fRefresh |= SelectItemsInSelectedRegions();
		}

		// If the selection state of any item was changed
		if( fRefresh )
		{
			// Redraw the strip
			m_pBaseMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

			// Update the item property page
			if( m_pMarkerMgr->m_pPropPageMgr != NULL )
			{
				m_pMarkerMgr->m_pPropPageMgr->RefreshData();
			}
		}
		break;

	case SP_GUTTERSELECT:
		// Check if the gutter selection state changed
		if( m_bGutterSelected != V_BOOL(&var) )
		{
			// Save the new state of the gutter selection
			m_bGutterSelected = V_BOOL(&var);

			// If the time range is empty
			if( m_lGutterBeginSelect == m_lGutterEndSelect )
			{	
				// Clear all current selections
				if( m_pSelectedCueGridRegions )
				{
					m_pSelectedCueGridRegions->Clear();
				}
				if( m_pSelectedSwitchGridRegions )
				{
					m_pSelectedSwitchGridRegions->Clear();
				}

				// If any items become unselected, redraw the strip
				if( SelectItemsInSelectedRegions() )
				{
					m_pBaseMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
				}
				break;
			}

			// Check if the gutter became selected
			if( m_bGutterSelected )
			{
				// Clear the list of selected regions
				if( m_pSelectedCueGridRegions )
				{
					m_pSelectedCueGridRegions->Clear();
				}
				if( m_pSelectedSwitchGridRegions )
				{
					m_pSelectedSwitchGridRegions->Clear();
				}

				// Select the segment of the timeline
				// This will return true if the selection state of any item changed
				fRefresh = SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
			}
			else
			{
				// Clear the list of selected regions
				if( m_pSelectedCueGridRegions )
				{
					m_pSelectedCueGridRegions->Clear();
				}
				if( m_pSelectedSwitchGridRegions )
				{
					m_pSelectedSwitchGridRegions->Clear();
				}

				// Clear the selection state of all items
				// This will return true if the selection state of any item changed
				fRefresh = SelectItemsInSelectedRegions();
			}

			// Since the gutter state changed, we need to redraw the strip
			m_pBaseMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

			// However, only update the property page if the selection state of any items
			// changed
			if( fRefresh )
			{
				// Update the property page
				if( m_pMarkerMgr->m_pPropPageMgr != NULL )
				{
					m_pMarkerMgr->m_pPropPageMgr->RefreshData();
				}
			}
		}
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::GetMarkerFromPoint

CMarkerItem *CMarkerStrip::GetMarkerFromPoint( long lPos, long lYPos )
{
	// Validate our timeline pointer
	if( m_pMarkerMgr->m_pTimeline )
	{
		// Convert the position to a measure, beat, and grid value
		long lClocks, lMeasure, lBeat, lGrid;
		if( SUCCEEDED( m_pMarkerMgr->m_pTimeline->PositionToClocks( lPos, &lClocks ) )
		&&	SUCCEEDED( m_pMarkerMgr->ClocksToMeasureBeatGrid( lClocks, &lMeasure, &lBeat, &lGrid ) ) )
		{
			// Conversion was successful, look for an item on that measure and beat and grid
			return GetTopItemFromMeasureBeatGrid( (lYPos < DEFAULT_STRIP_HEIGHT / 2) ? MARKER_CUE : MARKER_SWITCH, lMeasure, lBeat, lGrid );
		}
	}

	// Conversion failed (or there is no Timeline) - return NULL
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CMarkerStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Initialize the return code to SUCCESS
	HRESULT hr = S_OK;

	// Validate our timeline pointer
	if( m_pMarkerMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		// Call a function to handle the left mouse button press
		hr = OnLButtonDown( wParam, lParam, lXPos, lYPos );
		m_lXPos = lXPos;	// need to start drag drop from where selection started
		m_lYPos = lYPos;
		break;

	case WM_RBUTTONDOWN:
		// Call a function to handle the right mouse button press
		hr = OnRButtonDown( wParam, lParam, lXPos, lYPos );
		break;

	case WM_MOUSEMOVE:
		// Check if the left mouse button is downpMarker
		if(m_fLeftMouseDown)
		{
			// The user moved the mouse while the left mouse button was down -
			// do a drag-drop operation.
			hr = DoDragDrop( m_pMarkerMgr->m_pTimeline, wParam, m_lXPos, m_lYPos );

			// The above method returns after a drop, or after the user
			// cancels the operation.  In either case, we don't want to do
			// a drag-drop operation again.
			m_fLeftMouseDown = false;

			// If the drag-drop operatoin didn't complete
			if(hr != S_OK || m_dwDropEffect == DROPEFFECT_NONE)
			{
				// Store the position that the drag started at, because drag drop eats mouse up
				m_lXPos = lXPos;

				// Check if the control key was down
				if(wParam & MK_CONTROL)
				{
					// Finish the toggle operation on the item that was clicked on
					// when the left mouse button was pressed.
					if( m_pMarkerItemToToggle )
					{
						switch( m_pMarkerItemToToggle->m_typeMarker )
						{
						case MARKER_SWITCH:
							m_pSelectedSwitchGridRegions->ToggleRegion( m_pMarkerItemToToggle->m_lMeasure, m_pMarkerItemToToggle->m_lBeat, m_pMarkerItemToToggle->m_lGrid );
							break;
						case MARKER_CUE:
							m_pSelectedCueGridRegions->ToggleRegion( m_pMarkerItemToToggle->m_lMeasure, m_pMarkerItemToToggle->m_lBeat, m_pMarkerItemToToggle->m_lGrid );
							break;
						}
						m_pMarkerItemToToggle = NULL;

						// Update the selection state of the Markers
						SelectItemsInSelectedRegions();
					}
				}
				// Check if the shift key was not down
				else if( !(wParam & MK_SHIFT) )
				{
					// If an item was clicked on
					if(GetMarkerFromPoint( lXPos, lYPos ))
					{
						// Update the selection regions to include only the selected items
						SelectRegionsFromSelectedMarkers();
					}
				}

				// Redraw the strip
				m_pMarkerMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

				// Switch the property page to the Marker property page
				m_pMarkerMgr->OnShowProperties();
			}
			else
			{
				// successful drag drop--make sure that only the regions with
				// selected items are selected
				SelectRegionsFromSelectedMarkers();
			}

			// Refresh the Marker property page, if it exists
			if( m_pMarkerMgr->m_pPropPageMgr )
			{
				m_pMarkerMgr->m_pPropPageMgr->RefreshData();
			}
		}
		break;

	case WM_LBUTTONUP:
		// Clear the flag so we don't start a drag-drop operation when the mouse moved
		m_fLeftMouseDown = false;

		// Check if the Ctrl key was pressed
		if(wParam & MK_CONTROL)
		{
			// Ctrl key pressed - toggle the selection state of the item that was clicked on
			if( m_pMarkerItemToToggle )
			{
				switch( m_pMarkerItemToToggle->m_typeMarker )
				{
				case MARKER_SWITCH:
					m_pSelectedSwitchGridRegions->ToggleRegion( m_pMarkerItemToToggle->m_lMeasure, m_pMarkerItemToToggle->m_lBeat, m_pMarkerItemToToggle->m_lGrid );
					break;
				case MARKER_CUE:
					m_pSelectedCueGridRegions->ToggleRegion( m_pMarkerItemToToggle->m_lMeasure, m_pMarkerItemToToggle->m_lBeat, m_pMarkerItemToToggle->m_lGrid );
					break;
				}
				m_pMarkerItemToToggle = NULL;

				// Select all items that are in the selection regions
				SelectItemsInSelectedRegions();
			}
		}
		// Check if the Shift key was not pressed
		else if( !(wParam & MK_SHIFT) )
		{
			// Look for an item at the position clicked on
			CMarkerItem* pMarker = GetMarkerFromPoint( lXPos, lYPos );
			if(pMarker)
			{
				// Found an item
				// Unselect all items
				m_pMarkerMgr->UnselectAll();

				// Mark the clicked on item as selected
				pMarker->m_fSelected = TRUE;

				// Update the selection regions to include just this selected item
				SelectRegionsFromSelectedMarkers();
			}
		}

		// Update the position to inset at
		m_lXPos = lXPos;
		m_lYPos = lYPos;

		// Ensure all other strips are unselected
		UnselectGutterRange();

		// Redraw ourself
		m_pMarkerMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

		// Refresh the Marker property page, if it exists
		if( m_pMarkerMgr->m_pPropPageMgr )
		{
			m_pMarkerMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	case WM_RBUTTONUP:
		// Save the vertical position of the click so we know where to insert a marker. if Insert is selected.
		// The BaseStrip handles saving the horizontal position.
		m_lYPos = lYPos;

		// Fall through to the default message handler
		hr = CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
		break;

	case WM_COMMAND:
	{
		// We should only get this message in response to a selection in the right-click context menu.
//		WORD wNotifyCode = HIWORD( wParam );	// notification code 
		WORD wID		 = LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
			case IDM_CYCLE_MARKERS:
				hr = m_pMarkerMgr->CycleMarkers( m_lXPos, m_lYPos );
				break;

			case ID_EDIT_MARKALL_MEASURES:
				hr = MarkAllHelper( TRUE, DMUS_SEGF_MEASURE );
				break;

			case ID_EDIT_MARKALL_BEATS:
				hr = MarkAllHelper( TRUE, DMUS_SEGF_BEAT );
				break;

			case ID_EDIT_MARKALL_GRIDS:
				hr = MarkAllHelper( TRUE, DMUS_SEGF_GRID );
				break;

			case ID_EDIT_MARKRANGE_MEASURES:
				hr = MarkRangeHelper( TRUE, DMUS_SEGF_MEASURE );
				break;

			case ID_EDIT_MARKRANGE_BEATS:
				hr = MarkRangeHelper( TRUE, DMUS_SEGF_BEAT );
				break;

			case ID_EDIT_MARKRANGE_GRIDS:
				hr = MarkRangeHelper( TRUE, DMUS_SEGF_GRID );
				break;

			case ID_EDIT_UNMARKALL_MEASURES:
				hr = MarkAllHelper( FALSE, DMUS_SEGF_MEASURE );
				break;

			case ID_EDIT_UNMARKALL_BEATS:
				hr = MarkAllHelper( FALSE, DMUS_SEGF_BEAT );
				break;

			case ID_EDIT_UNMARKALL_GRIDS:
				hr = MarkAllHelper( FALSE, DMUS_SEGF_GRID );
				break;

			case ID_EDIT_UNMARKRANGE_MEASURES:
				hr = MarkRangeHelper( FALSE, DMUS_SEGF_MEASURE );
				break;

			case ID_EDIT_UNMARKRANGE_BEATS:
				hr = MarkRangeHelper( FALSE, DMUS_SEGF_BEAT );
				break;

			case ID_EDIT_UNMARKRANGE_GRIDS:
				hr = MarkRangeHelper( FALSE, DMUS_SEGF_GRID );
				break;

			default:
				// Try the default message handler
				CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
			break;
		}
		break;
	}

	case WM_CREATE:
		// Register our clipboard format, if it hasn't been registered yet
		if( m_cfMarkerList == 0 )
		{
			m_cfMarkerList = RegisterClipboardFormat( CF_MARKERLIST );
		}

		if(!m_pSelectedCueGridRegions)
		{
			m_pSelectedCueGridRegions = new CListSelectedGridRegion(m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits);
		}
		else
		{
			m_pSelectedCueGridRegions->Timeline() = m_pMarkerMgr->m_pTimeline;
			m_pSelectedCueGridRegions->GroupBits() = m_pMarkerMgr->m_dwGroupBits;
		}

		if(!m_pSelectedSwitchGridRegions)
		{
			m_pSelectedSwitchGridRegions = new CListSelectedGridRegion(m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits);
		}
		else
		{
			m_pSelectedSwitchGridRegions->Timeline() = m_pMarkerMgr->m_pTimeline;
			m_pSelectedSwitchGridRegions->GroupBits() = m_pMarkerMgr->m_dwGroupBits;
		}

		// Fall through to CBaseStrip's message handler, since we want it to also
		// handle the WM_CREATE message

	default:
		// Try the default message handler
		// This handles WM_RBUTTONUP, WM_COMMAND and WM_CREATE
		CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::Copy

HRESULT CMarkerStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if there is anything to copy
	HRESULT hr;
	hr = CanCopy();
	if( hr != S_OK )
	{
		// Nothing to copy - return error code
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}

	// Validate our pointer to the timeline
	if( (m_pMarkerMgr == NULL)
	||	(m_pMarkerMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfMarkerList == 0 )
	{
		m_cfMarkerList = RegisterClipboardFormat( CF_MARKERLIST );
		if( m_cfMarkerList == 0 )
		{
			ASSERT(FALSE);
			return E_FAIL;
		}
	}

	// Create an IStream to save the selected Markers in.
	IStream* pStreamCopy;
	hr = CreateStreamOnHGlobal( NULL, TRUE, &pStreamCopy );

	// Verify that we were able to create a stream
	if( FAILED( hr ))
	{
		return E_OUTOFMEMORY;
	}

	// Get the span of the selected Markers
	long lStartTime, lEndTime;
	m_pSelectedCueGridRegions->GetSpan( lStartTime, lEndTime );
	long lStartSwitchTime, lEndSwitchTime;
	m_pSelectedSwitchGridRegions->GetSpan( lStartSwitchTime, lEndSwitchTime );

	// Ensure we're using the maximums and minimums
	lStartTime = min( lStartTime, lStartSwitchTime );
	lEndTime = max( lEndTime, lEndSwitchTime );

	// Convert the start time to a measure and beat and grid
	long lMeasure, lBeat, lGrids;
	m_pMarkerMgr->ClocksToMeasureBeatGrid( lStartTime, &lMeasure, &lBeat, &lGrids );

	// Convert the start time to a number of grids
	MeasureBeatGridToGrids( m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits, 0, lMeasure, lBeat, lGrids, lGrids );

	// Convert the end time to a measure and beat and grid
	long lEndGrids;
	m_pMarkerMgr->ClocksToMeasureBeatGrid( lEndTime, &lMeasure, &lBeat, &lEndGrids );

	// Convert the end time to a number of grids
	MeasureBeatGridToGrids( m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits, 0, lMeasure, lBeat, lEndGrids, lEndGrids );

	// Compute the span of the region we're copying
	lEndGrids -= lGrids;

	// Save the number of grids in the region we're copying
	pStreamCopy->Write( &lEndGrids, sizeof(long), NULL );

	// Save the selected Markers into the stream
	hr = m_pMarkerMgr->SaveSelectedMarkers( pStreamCopy, lGrids );

	// Check if the save succeeded
	if( FAILED( hr ))
	{
		// Save failed
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	// The save succeeded - copy the stream to the clipboard
	hr = CopyMarkerDataToClipboard( pITimelineDataObject, pStreamCopy, m_cfMarkerList, m_pMarkerMgr, this );

	// Release our pointer to the stream
	pStreamCopy->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::PasteAt

HRESULT CMarkerStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lPasteTime, bool bDropNotPaste, BOOL &fChanged )
{
	// Validate the data object pointer
	if( pITimelineDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Initialize the return code to a failure code
	HRESULT hr = E_FAIL;

	// If we're in a drag-drop operation, check if we're dragging from and dropping to this strip
	if( bDropNotPaste
	&&	(m_pITargetDataObject == m_pISourceDataObject) )
	{
		// Ensure we have a valid timeline pointer
		if( m_pMarkerMgr->m_pTimeline )
		{
			// Determine the measure and beat and grid we're dropping in
			long lMeasure1, lBeat1, lGrid1;
			if(SUCCEEDED(m_pMarkerMgr->ClocksToMeasureBeatGrid( lPasteTime, &lMeasure1, &lBeat1, &lGrid1 )))
			{
				// Compute the measure and beat and grid the drag started from
				long lMeasure2, lBeat2, lGrid2, lStartDragClocks;
				if( SUCCEEDED( m_pMarkerMgr->m_pTimeline->PositionToClocks( m_lStartDragPosition, &lStartDragClocks ) )
				&&	SUCCEEDED( m_pMarkerMgr->ClocksToMeasureBeatGrid( lStartDragClocks, &lMeasure2, &lBeat2, &lGrid2 ) ) )
				{
					// Check if we dropped in the same grid we started the drag from.
					if( lMeasure1 == lMeasure2
					&&	lBeat1 == lBeat2
					&&	lGrid1 == lGrid2 )
					{
						// Didn't move - exit early
						goto Leave;
					}
				}
			}
		}
	}

	// Check if there is a Marker list avilable for us
	if( pITimelineDataObject->IsClipFormatAvailable( m_cfMarkerList ) == S_OK )
	{
		// Try and read the stream that contains the Markers
		IStream* pIStream;
		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfMarkerList, &pIStream)))
		{
			// If we're pasting, read in the number of grids that this selection covers
			long lGridsToPaste = -1;
			if( !bDropNotPaste )
			{
				pIStream->Read( &lGridsToPaste, sizeof(long), NULL );
			}

			// Load the stream into a list of items
			CTypedPtrList<CPtrList, CMarkerItem*> lstCueMarkers, lstSwitchMarkers;
			hr = LoadCopiedMarkerList(lstCueMarkers, lstSwitchMarkers, pIStream);

			// Check if the load operation succeeded
			if ( SUCCEEDED(hr) )
			{
				if( m_pMarkerMgr->m_pTimeline )
				{
					// Get the measure and beat and grid of the drop or paste position
					long lMeasureStartPaste, lBeatStartPaste, lGridStartPaste;
					if(FAILED(m_pMarkerMgr->ClocksToMeasureBeatGrid( max( lPasteTime, 0 ), &lMeasureStartPaste, &lBeatStartPaste, &lGridStartPaste )))
					{
						hr = E_FAIL;
						goto Leave_1;
					}

					long lStartPasteInGrids;
					MeasureBeatGridToGrids( m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits, 0, lMeasureStartPaste, lBeatStartPaste, lGridStartPaste, lStartPasteInGrids );

					// The length of the segment, in grids
					long lGridSegmentLength = LONG_MAX;

					// Check if there are any items to paste
					if( !lstCueMarkers.IsEmpty()
					||	!lstSwitchMarkers.IsEmpty() )
					{
						// Unselect all existing Markers in this strip so the only selected Markers are the dropped
						// or pasted ones
						m_pMarkerMgr->UnselectAll();

						// Make sure the last item lands in the last grid or sooner
						// Retrieve the clock length of the segment
						VARIANT varLength;
						if( SUCCEEDED( m_pMarkerMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
						{
							// Compute the measure and beat and grid length of the segment
							long lMeasure, lBeat, lGrid;
							if( SUCCEEDED( m_pMarkerMgr->ClocksToMeasureBeatGrid( V_I4(&varLength), &lMeasure, &lBeat, &lGrid ) ) )
							{
								// Compute the grid length of the segment
								MeasureBeatGridToGrids( m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits, 0,
									lMeasure, lBeat, lGrid, lGridSegmentLength );

								// Make sure the last item lands in the last beat or sooner
								long lLatestGrid;
								if( lstCueMarkers.IsEmpty() )
								{
									lLatestGrid = lstSwitchMarkers.GetTail()->m_mtTime;
								}
								else if( lstSwitchMarkers.IsEmpty() )
								{
									lLatestGrid = lstCueMarkers.GetTail()->m_mtTime;
								}
								else
								{
									lLatestGrid = max( lstCueMarkers.GetTail()->m_mtTime, lstSwitchMarkers.GetTail()->m_mtTime );
								}

								if( lLatestGrid + lStartPasteInGrids >= lGridSegmentLength )
								{
									lStartPasteInGrids = lGridSegmentLength - lLatestGrid - 1;
								}
							}
						}

						// Make sure the first item lands in grid 0 or later
						long lEarliestGrid;
						if( lstCueMarkers.IsEmpty() )
						{
							lEarliestGrid = lstSwitchMarkers.GetHead()->m_mtTime;
						}
						else if( lstSwitchMarkers.IsEmpty() )
						{
							lEarliestGrid = lstCueMarkers.GetHead()->m_mtTime;
						}
						else
						{
							lEarliestGrid = min( lstCueMarkers.GetHead()->m_mtTime, lstSwitchMarkers.GetHead()->m_mtTime );
						}

						if( lEarliestGrid + lStartPasteInGrids < 0 )
						{
							lStartPasteInGrids = -lEarliestGrid;
						}
					}

					// Check if we're doing a paste
					if(!bDropNotPaste)
					{
						// We're doing a paste, so get the paste type
						TIMELINE_PASTE_TYPE tlPasteType;
						if( FAILED( m_pMarkerMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
						{
							// Failed to get the paste type, so return with error code
							hr = E_FAIL;
							goto Leave_1;
						}

						// Check if we're doing a paste->overwrite
						if( tlPasteType == TL_PASTE_OVERWRITE )
						{
							// Count lGridsToPaste - 1 grids from lStartPasteInGrids and convert to a measure and grid value
							long lmEnd, lbEnd, lgEnd;
							GridsToMeasureBeatGrid( m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits, 0,
								lStartPasteInGrids + lGridsToPaste - 1, lmEnd, lbEnd, lgEnd );

							// Now, delete any items that fall between the first and last grids we're pasting in
							fChanged = m_pMarkerMgr->DeleteBetweenMeasureBeatGrids(lMeasureStartPaste, lBeatStartPaste, lGridStartPaste, lmEnd, lbEnd, lgEnd );
						}
					}

					// Iterate through the list of Markers we loaded
					while( !lstCueMarkers.IsEmpty() )
					{
						// Remove the head of the list
						CMarkerItem* pItem = lstCueMarkers.RemoveHead();

						// Check if the item will land before the end of the segment
						if( pItem->m_mtTime + lStartPasteInGrids < lGridSegmentLength )
						{
							// Convert from a number of grids to a measure and beat and grid value
							// When we saved this to a stream in CMarkerMgr::SaveSelectedMarkers, we stored
							// the grid offset into m_mtTime.
							GridsToMeasureBeatGrid( m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits, 0,
								pItem->m_mtTime + lStartPasteInGrids, pItem->m_lMeasure, pItem->m_lBeat, pItem->m_lGrid );

							// Recalc m_mtTime fields
							m_pMarkerMgr->MeasureBeatGridTickToClocks( pItem->m_lMeasure, pItem->m_lBeat, pItem->m_lGrid, pItem->m_lTick,
																  &pItem->m_mtTime );
							m_pMarkerMgr->ClocksToMeasureBeatGridTick( pItem->m_mtTime,
																  &pItem->m_lMeasure, &pItem->m_lBeat, &pItem->m_lGrid, &pItem->m_lTick );

							// When pasted or dropped, each item is selected
							pItem->m_fSelected = TRUE;

							// This will overwrite any item that already exists on the measure and
							// beat and grid where pItem will be inserted
							m_pMarkerMgr->InsertByAscendingTime( pItem );

							// We changed
							fChanged = TRUE;
						}
						else
						{
							// Item would be pasted beyond the end of the segment - delete it
							delete pItem;
						}
					}

					while( !lstSwitchMarkers.IsEmpty() )
					{
						// Remove the head of the list
						CMarkerItem* pItem = lstSwitchMarkers.RemoveHead();

						// Check if the item will land before the end of the segment
						if( pItem->m_mtTime + lStartPasteInGrids < lGridSegmentLength )
						{
							// Convert from a number of grids to a measure and beat and grid value
							// When we saved this to a stream in CMarkerMgr::SaveSelectedMarkers, we stored
							// the grid offset into m_mtTime.
							GridsToMeasureBeatGrid( m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits, 0,
								pItem->m_mtTime + lStartPasteInGrids, pItem->m_lMeasure, pItem->m_lBeat, pItem->m_lGrid );

							// Recalc m_mtTime fields
							m_pMarkerMgr->MeasureBeatGridTickToClocks( pItem->m_lMeasure, pItem->m_lBeat, pItem->m_lGrid, pItem->m_lTick,
																  &pItem->m_mtTime );
							m_pMarkerMgr->ClocksToMeasureBeatGridTick( pItem->m_mtTime,
																  &pItem->m_lMeasure, &pItem->m_lBeat, &pItem->m_lGrid, &pItem->m_lTick );

							// When pasted or dropped, each item is selected
							pItem->m_fSelected = TRUE;

							// This will overwrite any item that already exists on the measure and
							// grid where pItem will be inserted
							m_pMarkerMgr->InsertByAscendingTime( pItem );

							// We changed
							fChanged = TRUE;
						}
						else
						{
							// Item would be pasted beyond the end of the segment - delete it
							delete pItem;
						}
					}
				}
			}
Leave_1:
			pIStream->Release();
		}
	}

Leave:
	if( m_nStripIsDragDropSource )
	{
		// Drag/drop Target and Source are the same Marker strip
		m_nStripIsDragDropSource = 2;
	}

	if( CMarkerStrip::m_pIDocRootOfDragDropSource
	&&  CMarkerStrip::m_pIDocRootOfDragDropSource == m_pMarkerMgr->m_pIDocRootNode )
	{
		// Drag/drop Target and Source are the same file (i.e. Segment)
		CMarkerStrip::m_fDragDropIntoSameDocRoot = TRUE;
	}
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::Paste

HRESULT CMarkerStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that there is something to paste
	HRESULT hr;
	hr = CanPaste( pITimelineDataObject );
	if( hr != S_OK )
	{
		// Nothing to paste - return an error
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Validate our MarkerMgr and Timeline pointers
	if( (m_pMarkerMgr == NULL)
	||	(m_pMarkerMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get a TimelineDataObject, if we don't already have one
	hr = GetTimelineDataObject( pITimelineDataObject, m_pMarkerMgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Convert the paste position to clocks
	long lClocks;
	if( FAILED( m_pMarkerMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lClocks) ) )
	{
		return E_UNEXPECTED;
	}

	// Do the paste
	BOOL fChanged = FALSE;
	hr = PasteAt(pITimelineDataObject, lClocks, false, fChanged);

	// Check if the paste succeeded AND if something was actually pasted
	if( SUCCEEDED(hr) && fChanged )
	{
		// Update the selection regions to only include the selected items
		SelectRegionsFromSelectedMarkers();

		// Notify the Segment Designer that we did a paste operation
		m_pMarkerMgr->m_nLastEdit = IDS_PASTE;
		m_pMarkerMgr->OnDataChanged();

		// Redraw our strip
		m_pMarkerMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

		// Ensure the Marker property page is visible
		m_pMarkerMgr->OnShowProperties();

		// Refresh the Marker property page
		if( m_pMarkerMgr->m_pPropPageMgr )
		{
			m_pMarkerMgr->m_pPropPageMgr->RefreshData();
		}

		// Sync with DirectMusic
		m_pMarkerMgr->SyncWithDirectMusic();
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::Insert

HRESULT CMarkerStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Double-check that our timeline pointer is valid
	if( (m_pMarkerMgr == NULL)
	||	(m_pMarkerMgr->m_pTimeline == NULL) )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	if( m_lXPos < 0 )
	{
		return E_FAIL;
	}

	// Unselect all items in this strip 
	m_pMarkerMgr->UnselectAll();

	// Unselect all items in other strips
	UnselectGutterRange();

	// Convert the insert position to a measure, beat, and grid value
	MUSIC_TIME mtTime;
	if( FAILED( m_pMarkerMgr->m_pTimeline->PositionToClocks( max( 0, m_lXPos ), &mtTime ) ) )
	{
		return E_UNEXPECTED;
	}
	long lMeasure, lBeat, lGrid;
	if( FAILED( m_pMarkerMgr->ClocksToMeasureBeatGrid( mtTime, &lMeasure, &lBeat, &lGrid ) ) )
	{
		return E_UNEXPECTED;
	}

	if( FAILED( m_pMarkerMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, lGrid, 0, &mtTime ) ) )
	{
		return E_UNEXPECTED;
	}

	CMarkerItem* pMarker = new CMarkerItem;
	if(pMarker == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Set the measure and beat and grid the item was inserted in
	pMarker->m_lMeasure = lMeasure;
	pMarker->m_lBeat = lBeat;
	pMarker->m_lGrid = lGrid;
	pMarker->m_typeMarker = (m_lYPos < DEFAULT_STRIP_HEIGHT / 2) ? MARKER_CUE : MARKER_SWITCH;

	// Set the clock time the item was inserted on
	pMarker->m_mtTime = mtTime;

	// By default, a newly selected item is selected
	pMarker->m_fSelected = TRUE;

	// Insert the item into our list of Markers, overwriting any existing one
	m_pMarkerMgr->InsertByAscendingTime(pMarker);

	// If it's not already selected, add the grid the item was inserted on
	// to the list of selected regions
	switch( pMarker->m_typeMarker )
	{
	case MARKER_CUE:
		if( !m_pSelectedCueGridRegions->Contains( lMeasure, lBeat, lGrid ) )
		{
			m_pSelectedCueGridRegions->AddRegion(m_lXPos);
		}
		break;
	case MARKER_SWITCH:
		if( !m_pSelectedSwitchGridRegions->Contains( lMeasure, lBeat, lGrid ) )
		{
			m_pSelectedSwitchGridRegions->AddRegion(m_lXPos);
		}
		break;
	}

	// Redraw our strip
	m_pMarkerMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

	// Show the property sheet
	ShowPropertySheet();

	// Switch to the Marker property page
	m_pMarkerMgr->OnShowProperties();

	// Refresh the Marker property page
	if( m_pMarkerMgr->m_pPropPageMgr )
	{
		m_pMarkerMgr->m_pPropPageMgr->RefreshData();
	}

	// Notify the Segment Designer that an insert occurred
	m_pMarkerMgr->m_nLastEdit = IDS_INSERT;
	m_pMarkerMgr->OnDataChanged();

	// Sync with DirectMusic
	m_pMarkerMgr->SyncWithDirectMusic();

	// Return the success/failure code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::Delete

HRESULT CMarkerStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	/* Ignore this, since we can be in a Cut() operation.  We can cut blank data, but we can't delete it.
	HRESULT hr = CanDelete();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}
	*/

	// Verify that we have a valid timeline pointer
	if( (m_pMarkerMgr == NULL)
	||	(m_pMarkerMgr->m_pTimeline == NULL) )
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}

	// Delete all selected items
	m_pMarkerMgr->DeleteSelectedMarkers();

	// Clear all selection ranges
	m_pSelectedCueGridRegions->Clear();
	m_pSelectedSwitchGridRegions->Clear();

	// Redraw our strip
	m_pMarkerMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	// Refresh the Marker property page, if it exists
	if( m_pMarkerMgr->m_pPropPageMgr )
	{
		m_pMarkerMgr->m_pPropPageMgr->RefreshData();
	}

	// Notify the Segment Designer that a delete operation occurred
	m_pMarkerMgr->m_nLastEdit = IDS_DELETE;
	m_pMarkerMgr->OnDataChanged();

	// Sync with DirectMusic
	m_pMarkerMgr->SyncWithDirectMusic();

	// Return that the delete succeeded
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::SelectAll

HRESULT CMarkerStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pBaseMgr != NULL );
	if( m_pBaseMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pBaseMgr->m_pTimeline != NULL );
	if( m_pBaseMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Select everything
	m_pSelectedCueGridRegions->SelectAll();
	m_pSelectedSwitchGridRegions->SelectAll();

	// Update the selection state of the items
	if( SelectItemsInSelectedRegions() )
	{
		// If an an item became selected, redraw the strip
		m_pBaseMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

		// Update the item property page
		if( m_pMarkerMgr->m_pPropPageMgr != NULL )
		{
			m_pMarkerMgr->m_pPropPageMgr->RefreshData();
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::CanPaste

HRESULT CMarkerStrip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid timeline pointer
	if( (m_pMarkerMgr == NULL)
	||	(m_pMarkerMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfMarkerList == 0 )
	{
		m_cfMarkerList = RegisterClipboardFormat( CF_MARKERLIST );
		if( m_cfMarkerList == 0 )
		{
			return E_FAIL;
		}
	}

	// Get a TimelineDataObject, if we don't already have one
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_pMarkerMgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check the timeline data object for our clipboard format
	hr = pITimelineDataObject->IsClipFormatAvailable( m_cfMarkerList );

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// If we found our clipboard format, return S_OK
	if (hr == S_OK)
	{
		return S_OK;
	}

	// Otherwise, return S_FALSE
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::CanInsert

HRESULT CMarkerStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid Timeline pointer
	if( (m_pMarkerMgr == NULL)
	||	(m_pMarkerMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get the position to paste at
	long lInsertTime;
	if( FAILED( m_pMarkerMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lInsertTime) ) )
	{
		return E_UNEXPECTED;
	}

	// Check if the insert position is invalid, or if the user clicked in the function bar
	if( m_fInFunctionBarMenu || (lInsertTime < 0) )
	{
		// It's invalid - we can't insert
		return S_FALSE;
	}

	// Convert from a time to a pixel position
	long lPosition;
	if( FAILED( m_pMarkerMgr->m_pTimeline->ClocksToPosition( lInsertTime, &lPosition ) ) )
	{
		return E_UNEXPECTED;
	}

	// Check to see if there is an existing item at the insert position
	CMarkerItem* pMarker = GetMarkerFromPoint( lPosition, m_lYPos );

	// Check if we found an item
	if( pMarker != NULL )
	{
		// We found an item - return S_FALSE since we can't insert here
		return S_FALSE;
	}

	// Get the length of the segment, in clocks
	VARIANT var;
	if( FAILED( m_pMarkerMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var) ) )
	{
		return E_UNEXPECTED;
	}

	// If the position to insert is beyond the end of the segment, return S_FALSE since we can't insert there
	if( lInsertTime >= V_I4(&var) )
	{
		return S_FALSE;
	}
	else
	{
		return S_OK;
	}
}


// IDropTarget Methods

/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::Drop

HRESULT CMarkerStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(grfKeyState);

	// Verify that we're passed the data object we're expecting
	if( (m_pITargetDataObject == NULL)
	||	(m_pITargetDataObject != pIDataObject) )
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}

	// Set default values
	HRESULT hr = S_OK;
	*pdwEffect = DROPEFFECT_NONE;

	// Choose effect when right mouse drag - Move, Copy, or Cancel?
	if( m_dwOverDragButton & MK_RBUTTON )
	{
		// Try and load the arrow cursor
		HCURSOR hCursor = ::LoadCursor( AfxGetInstanceHandle(), IDC_ARROW );
		if( hCursor )
		{
			// The the mouse cursor to an arrow cursor
			HCURSOR hCursorOld = ::SetCursor( hCursor );
	
			// Prepare context menu
			HMENU hMenu = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDM_DRAG_RMENU) );
			if( hMenu )
			{
				// Initialize the drop effect to DROPEFFECT_NONE
				m_dwDragRMenuEffect = DROPEFFECT_NONE;

				// Track right context menu for drag-drop via TrackPopupMenu
				HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );

				// Init state of menu items
				if( !(m_dwOverDragEffect & DROPEFFECT_MOVE) )
				{
					::EnableMenuItem( hMenuPopup, IDM_DRAG_MOVE, (MF_GRAYED | MF_BYCOMMAND) );
				}

				// Get a window to attach menu to
				HWND hwnd = GetTimelineHWND();
				if( hwnd )
				{
					// Display and track menu
	 				::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
								  pt.x, pt.y, 0, hwnd, NULL );

					// Destroy the popup menu
					::DestroyMenu( hMenu );

					// Need to process WM_COMMAND from TrackPopupMenu
					MSG msg;
					while( ::PeekMessage( &msg, hwnd, NULL, NULL, PM_REMOVE) )
					{
						if( msg.message == WM_COMMAND )
						{
							OnWMMessage( msg.message, msg.wParam, msg.lParam, 0, 0 );
						}
						else
						{
							TranslateMessage( &msg );
							DispatchMessage( &msg );
						}
					}

					// WM_COMMAND from TrackPopupMenu will have set m_dwDragRMenuEffect
					m_dwOverDragEffect = m_dwDragRMenuEffect;

					// Reset the RMenu's effect to DROPEFFECT_NONE
					m_dwDragRMenuEffect = DROPEFFECT_NONE;
				}
			}

			// Reset the cursor to its previous image
			::SetCursor( hCursorOld );

			// Don't destory the hCursor, since it is a shared cursor
		}
	}

	// Check that the user didn't cancel the drag-drop operation
	if( m_dwOverDragEffect != DROPEFFECT_NONE )
	{
		// Allocate a Timleine DataObject
		IDMUSProdTimelineDataObject *pITimelineDataObject;
		if( SUCCEEDED( m_pMarkerMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			// Import the DataObject that was dragged
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				// Compute the time to drop at
				long lPasteTime;
				m_pMarkerMgr->m_pTimeline->PositionToClocks( pt.x, &lPasteTime );

				// Actually paste the data
				BOOL fChanged = FALSE;
				hr = PasteAt( pITimelineDataObject, lPasteTime, true, fChanged );

				// Check that the paste succeeded, and that something was actually pasted
				if( SUCCEEDED ( hr ) && fChanged )
				{
					*pdwEffect = m_dwOverDragEffect;

					// Check if the target strip is different from the source strip
					if( m_nStripIsDragDropSource != 2 )
					{
						// Update the list of selected regions to include only the selected items
						SelectRegionsFromSelectedMarkers();

						if( CMarkerStrip::m_fDragDropIntoSameDocRoot == FALSE
						||  m_dwOverDragEffect != DROPEFFECT_MOVE )
						{
							// Either we are dropping into a different file,
							// or we are doing a "copy" and there will be no change to the source strip, so....
							// Notify the SegmentDesigner that a paste operation occurred
							m_pMarkerMgr->m_nLastEdit = IDS_PASTE;
							m_pMarkerMgr->OnDataChanged();
						}

						// Redraw our strip
						m_pMarkerMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
						
						// If the property sheet is visible, make it display the Marker property page
						m_pMarkerMgr->OnShowProperties();

						// Update the Marker property page, if it exists
						if( m_pMarkerMgr->m_pPropPageMgr )
						{
							m_pMarkerMgr->m_pPropPageMgr->RefreshData();
						}

						// Sync with DirectMusic
						m_pMarkerMgr->SyncWithDirectMusic();
					}
				}
			}

			// Release our reference to the Timeline DataObject we allocated
			pITimelineDataObject->Release();
		}
	}

	// Cleanup
	DragLeave();

	// Return the success/failure code
	return hr;
}


// IDropTarget helpers

/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::CanPasteFromData

HRESULT CMarkerStrip::CanPasteFromData(IDataObject* pIDataObject)
{
	// Check that we're passed a valid IDataObject pointer
	if( pIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Get a Timeline DataObject that encapsulates the pIDataObject
	IDMUSProdTimelineDataObject *pITimelineDataObject = NULL;
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_pMarkerMgr->m_pTimeline, pIDataObject );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check if our clipboard format is available in the data object
	hr = S_FALSE;
	if( pITimelineDataObject->IsClipFormatAvailable( m_cfMarkerList ) == S_OK )
	{
		// Our format is available = return S_OK
		hr = S_OK;
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return either S_OK or S_FALSE
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::OnLButtonDown

HRESULT CMarkerStrip::OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);

	// Validate our timeline pointer
	if( (m_pMarkerMgr == NULL)
	||	(m_pMarkerMgr->m_pTimeline == NULL) )
	{
		return E_UNEXPECTED;
	}

	// Initialize the item to toggle to NULL
	m_pMarkerItemToToggle = NULL;
	
	// Unselect all items in the other strips
	UnselectGutterRange();

	// Capture mouse so we get the WM_LBUTTONUP message as well.
	// The timeline will release the capture when it receives the
	// WM_LBUTTONUP message.
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pMarkerMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Flag that the left mouse button is pressed
	m_fLeftMouseDown = true;

	// Show the Marker property page
	m_fShowItemProps = true;

	// Check if the shift key is pressed
	if(wParam & MK_SHIFT)
	{
		// Do shift-click selection
		if( lYPos < DEFAULT_STRIP_HEIGHT / 2 )
		{
			// 2nd click was in the top, select Cue markers
			m_pSelectedCueGridRegions->ShiftAddRegion(lXPos);

			if( m_typeShiftAnchor == MARKER_SWITCH )
			{
				// 1st click was in the bottom, select Switch markers
				m_pSelectedSwitchGridRegions->ShiftAddRegion(lXPos);
			}
			else
			{
				// Otherwise, unselect all Switch markers
				m_pSelectedSwitchGridRegions->Clear();
			}
		}
		else
		{
			// 2nd click was in the bottom, select Switch markers
			m_pSelectedSwitchGridRegions->ShiftAddRegion(lXPos);

			if( m_typeShiftAnchor == MARKER_CUE )
			{
				// 1st click was in the top, select Cue markers
				m_pSelectedCueGridRegions->ShiftAddRegion(lXPos);
			}
			else
			{
				// Otherwise, unselect all Cue markers
				m_pSelectedCueGridRegions->Clear();
			}
		}

		// Now, actually select the markers
		SelectItemsInSelectedRegions();
	}
	else
	{
		// See if there is an item under the cursor.
		CMarkerItem* pMarker = GetMarkerFromPoint( lXPos, lYPos );
		if( pMarker )
		{
			// Found an item under the cursor

			// Check if the control key is down
			if(wParam & MK_CONTROL)
			{
				// Check if the item is not yet selected
				if( pMarker->m_fSelected )
				{
					// Set up to unselect this item later, either when we receive a
					// left-button up, or when the user completes a drag-drop operation
					// that does nothing.
					m_pMarkerItemToToggle = pMarker;
				}
				else
				{
					// Not yet selected - select the grid just clicked on
					pMarker->m_fSelected = TRUE;
					switch( pMarker->m_typeMarker )
					{
					case MARKER_CUE:
						m_pSelectedCueGridRegions->AddRegion(lXPos);
						break;
					case MARKER_SWITCH:
						m_pSelectedSwitchGridRegions->AddRegion(lXPos);
						break;
					}
				}
			}
			// Check if the item is unselected (the shift key is up)
			else if( !pMarker->m_fSelected )
			{
				// Mark the clicked on item as selected
				m_pMarkerMgr->UnselectAll();
				pMarker->m_fSelected = TRUE;

				// Update the selection regions to include just this selected item
				SelectRegionsFromSelectedMarkers();
			}
		}
		// Didn't find an item - check if the Ctrl key is down
		else if(wParam & MK_CONTROL)
		{
			// Toggle the selection state in the region under the cursor
			if( lYPos < DEFAULT_STRIP_HEIGHT / 2 )
			{
				m_pSelectedCueGridRegions->ToggleRegion(lXPos);
			}
			else
			{
				m_pSelectedSwitchGridRegions->ToggleRegion(lXPos);
			}
		}
		// The shift key is up
		else
		{
			// The user left-clicked on blank space without holding the shift or Ctrl keys down

			// Clear all selection regions
			m_pSelectedCueGridRegions->Clear();
			m_pSelectedSwitchGridRegions->Clear();
			m_pMarkerMgr->UnselectAll();

			// Select only the grid clicked on
			if( lYPos < DEFAULT_STRIP_HEIGHT / 2 )
			{
				m_pSelectedCueGridRegions->AddRegion(lXPos);
			}
			else
			{
				m_pSelectedSwitchGridRegions->AddRegion(lXPos);
			}
		}

		// Set anchor for future shift-select operations
		m_pSelectedCueGridRegions->SetShiftSelectAnchor( lXPos );
		m_pSelectedSwitchGridRegions->SetShiftSelectAnchor( lXPos );
		m_typeShiftAnchor = (lYPos < DEFAULT_STRIP_HEIGHT / 2) ? MARKER_CUE : MARKER_SWITCH;
	}

	// Redraw our strip
	m_pMarkerMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

	// Switch the property page to the Marker property page
	m_pMarkerMgr->OnShowProperties();

	// Refresh the Marker property page, if it exists
	if( m_pMarkerMgr->m_pPropPageMgr )
	{
		m_pMarkerMgr->m_pPropPageMgr->RefreshData();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::OnRButtonDown

HRESULT CMarkerStrip::OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);
	ASSERT( m_pMarkerMgr->m_pTimeline != NULL );

	// Unselect all items in the other strips
	UnselectGutterRange();

	// Get the item at the mouse click.
	CMarkerItem* pMarker = GetMarkerFromPoint( lXPos, lYPos );

	if( !(wParam & (MK_CONTROL | MK_SHIFT)) )
	{
		// Neither control or shift were pressed

		// Check if an item was clicked on
		if( pMarker )
		{
			// Check if the item is selected
			if( pMarker->m_fSelected == FALSE )
			{
				// Marker is not selected, unselect all items
				m_pMarkerMgr->UnselectAll();

				// Select just this item
				pMarker->m_fSelected = TRUE;

				// Add just this grid to the list of selected regions
				switch( pMarker->m_typeMarker )
				{
				case MARKER_CUE:
					m_pSelectedCueGridRegions->AddRegion(lXPos);
					break;
				case MARKER_SWITCH:
					m_pSelectedSwitchGridRegions->AddRegion(lXPos);
					break;
				}
			}
			// If the item was already selected, do nothing
		}
		else
		{
			// if region not selected, select it, otherwise do nothing

			// Convert the pixel position to a measure and beat and grid
			long lMeasure, lBeat, lGrid;
			if( SUCCEEDED( m_pMarkerMgr->m_pTimeline->PositionToClocks( lXPos, &lMeasure ) )
			&&	SUCCEEDED( m_pMarkerMgr->ClocksToMeasureBeatGrid( lMeasure, &lMeasure, &lBeat, &lGrid ) ) )
			{
				// Check if this beat is not already selected
				if( lYPos < DEFAULT_STRIP_HEIGHT / 2 )
				{
					if( !m_pSelectedCueGridRegions->Contains(lMeasure, lBeat, lGrid))
					{
						// This beat is not selected - unselect all beats
						m_pMarkerMgr->UnselectAll();

						// Now, select just this beat
						m_pSelectedCueGridRegions->AddRegion(lXPos);
					}
				}
				else
				{
					if( !m_pSelectedSwitchGridRegions->Contains(lMeasure, lBeat, lGrid))
					{
						// This beat is not selected - unselect all beats
						m_pMarkerMgr->UnselectAll();

						// Now, select just this beat
						m_pSelectedSwitchGridRegions->AddRegion(lXPos);
					}
				}
			}
		}
	}

	// Redraw our strip
	m_pMarkerMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	// Switch to the Marker property page
	m_fShowItemProps = true;
	m_pMarkerMgr->OnShowProperties(); 
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::CanCycle

BOOL CMarkerStrip::CanCycle( void )
{
	ASSERT( m_pMarkerMgr->m_pTimeline != NULL );
	if( m_pMarkerMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	long lTime, lMeasure, lBeat, lGrid;
	HRESULT hr = m_pMarkerMgr->m_pTimeline->PositionToClocks( m_lXPos, &lTime );
	if( SUCCEEDED( hr ) )
	{
		hr = m_pMarkerMgr->ClocksToMeasureBeatGrid( lTime, &lMeasure, &lBeat, &lGrid );

		if( SUCCEEDED ( hr ) ) 
		{
			CTypedPtrList<CPtrList, CMarkerItem*> *plstMarkers;
			if( m_lYPos < DEFAULT_STRIP_HEIGHT / 2 )
			{
				plstMarkers = &m_pMarkerMgr->m_lstCueMarkers;
			}
			else
			{
				plstMarkers = &m_pMarkerMgr->m_lstSwitchMarkers;
			}

			int nCount = 0;

			POSITION pos = plstMarkers->GetHeadPosition();
			while( pos )
			{
				CMarkerItem* pItem = plstMarkers->GetNext( pos );

				if( pItem->m_lMeasure == lMeasure
				&&  pItem->m_lBeat == lBeat
				&&	pItem->m_lGrid == lGrid )
				{
					if( ++nCount > 1 )
					{
						return TRUE;
					}
				}

				if( pItem->m_lMeasure > lMeasure )
				{
					break;
				}
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::PostRightClickMenu

HRESULT CMarkerStrip::PostRightClickMenu( POINT pt )
{
	m_fInRightClickMenu = TRUE;

	HMENU hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_EDIT_RMENU) );
	HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

	// Modify text of the Insert menu item
	CString strMenuText;
	bool fCue = (m_lYPos < DEFAULT_STRIP_HEIGHT / 2);
	strMenuText.LoadString( fCue ? IDS_INSERT_CUE : IDS_INSERT_SWITCH );
	::ModifyMenu( hMenuPopup, ID_EDIT_INSERT, MF_BYCOMMAND | MF_STRING | MF_ENABLED, ID_EDIT_INSERT, strMenuText );

	// Modify text of the Cycle menu item
	strMenuText.LoadString( fCue ? IDS_CYCLE_CUE : IDS_CYCLE_SWITCH );
	::ModifyMenu( hMenuPopup, IDM_CYCLE_MARKERS, MF_BYCOMMAND | MF_STRING | MF_ENABLED, IDM_CYCLE_MARKERS, strMenuText );

	::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED : MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED : MF_GRAYED );
	::EnableMenuItem( hMenuPopup, 2, ( CanPaste( NULL ) == S_OK ) ? MF_ENABLED | MF_BYPOSITION : MF_GRAYED | MF_BYPOSITION );
	::EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( CanInsert() == S_OK ) ? MF_ENABLED : MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED : MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( IsEmpty() ) ? MF_GRAYED : MF_ENABLED );
	::EnableMenuItem( hMenuPopup, ID_VIEW_PROPERTIES, MF_ENABLED );
	::EnableMenuItem( hMenuPopup, IDM_CYCLE_MARKERS, CanCycle() ? MF_ENABLED : MF_GRAYED );

	// Check if there is a valid selection range
	BOOL fHaveValidRange;
	if( fCue )
	{
		fHaveValidRange = (m_pSelectedCueGridRegions->GetCount() > 0);
	}
	else
	{
		fHaveValidRange = (m_pSelectedSwitchGridRegions->GetCount() > 0);
	}

	// Mark Range
	EnableMenuItem( hMenuPopup, 9, fHaveValidRange ?
					MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );
	// Unmark Range
	EnableMenuItem( hMenuPopup, 11, fHaveValidRange ?
					MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );

	m_pMarkerMgr->m_pTimeline->TrackPopupMenu( hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE );
	::DestroyMenu( hMenu );

	m_fInRightClickMenu = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::UnselectGutterRange

void CMarkerStrip::UnselectGutterRange( void )
{
	// Make sure everything on the timeline is deselected.

	// Flag that we're unselecting all other strips
	m_fSelecting = TRUE;

	// Set the begin and end selection time to 0
	m_pMarkerMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pMarkerMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );

	// Flags that we're no longer unselecting all other strips
	m_fSelecting = FALSE;
}


// IDropSource helpers

/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::CreateDataObject

HRESULT	CMarkerStrip::CreateDataObject(IDataObject** ppIDataObject, long lXPos, long lYPos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we're passed a valid location to store the IDataObject pointer in
	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Initialize the IDataObject pointer to NULL
	*ppIDataObject = NULL;

	// Allocate a Timleine DataObject
	IDMUSProdTimelineDataObject *pITimelineDataObject;
	if( FAILED( m_pMarkerMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
	{
		return E_OUTOFMEMORY;
	}


	// Save Selected Markers into stream
	HRESULT hr = E_FAIL;

	// Allocate a memory stream
	IStream* pIStream;
	if( SUCCEEDED ( m_pMarkerMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// Get the item at the drag point
		CMarkerItem* pMarkerAtDragPoint = GetMarkerFromPoint( lXPos, lYPos );

		// mark the Markers as being dragged: this used later for deleting Markers in drag move
		m_pMarkerMgr->MarkSelectedMarkers(UD_DRAGSELECT);

		// Save the selected items into a stream
		if( SUCCEEDED ( m_pMarkerMgr->SaveSelectedMarkers( pIStream, pMarkerAtDragPoint ) ) )
		{
			// Add the stream to the Timeline DataObject
			if( SUCCEEDED ( pITimelineDataObject->AddExternalClipFormat( m_cfMarkerList, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		// Releas the stream
		pIStream->Release();
	}

	// If we were successful, export the Timeline DataObject into an IDataObject
	if( SUCCEEDED ( hr ) )
	{
		pITimelineDataObject->Export( ppIDataObject );
	}

	// Release the Timeline DataObject
	pITimelineDataObject->Release();

	// Return the success/failure code
	return hr;
}


////////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::DoDragDrop

HRESULT CMarkerStrip::DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos, long lYPos)
{
	// Do a drag'n'drop operation

	// Get the item at the point we're dragging from
	CMarkerItem* pMarker = GetMarkerFromPoint( lXPos, lYPos );

	// Check that we found a item, and that it is selected
	if(!pMarker || !pMarker->m_fSelected)
	{
		// Didn't find a selected item - return that nothing happened
		return S_FALSE;
	}

	// Initialize flag that determines when a successful drop occurs
	bool fDrop = false;

	// Initialize our result code
	HRESULT hr = S_FALSE;

	// Drag-drop will capture mouse, so release it from timeline
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Initialize the drop effect to 'none'
	m_dwDropEffect = DROPEFFECT_NONE;

	// Query ourself for our IDropSource interface
	IDropSource*	pIDropSource;
	if(SUCCEEDED(QueryInterface(IID_IDropSource, (void**)&pIDropSource)))
	{
		// Ensure m_pISourceDataObject is empty
		if( m_pISourceDataObject )
		{
			m_pISourceDataObject->Release();
			m_pISourceDataObject = NULL;
		}

		// Create a data object from the selected items
		hr = CreateDataObject( &m_pISourceDataObject, lXPos, lYPos );

		// Check if we successfully created a data object
		if(SUCCEEDED(hr))
		{
			// We can always copy Markers.
			DWORD dwOKDragEffects = DROPEFFECT_COPY;

			// Check if we can cut Markers
			if(CanCut() == S_OK)
			{
				// If we can Cut(), allow the user to move the Markers as well.
				dwOKDragEffects |= DROPEFFECT_MOVE;
			}

			// Do the Drag/Drop.

			// Initialize our DragDropSource flag to 1 (means we are the source of a drag-drop)
			m_nStripIsDragDropSource = 1;

			// Initialize our DragDropSource DocRoot fields
			CMarkerStrip::m_pIDocRootOfDragDropSource = m_pMarkerMgr->m_pIDocRootNode;
			CMarkerStrip::m_fDragDropIntoSameDocRoot = FALSE;

			// Save the state of the mouse buttons
			m_dwStartDragButton = (unsigned long)wParam;

			// Save the position the drag started at
			// (used in PasteAt to determine if we dropped in the same beat we dragged from)
			m_lStartDragPosition = lXPos;

			// Initialize the drop effect to 'none'
			DWORD dwEffect = DROPEFFECT_NONE;

			// Actually do the drag-drop operation
			hr = ::DoDragDrop(m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect);

			// Drag/Drop completed, clean up
			m_dwStartDragButton = 0;
			m_pISourceDataObject->Release();
			m_pISourceDataObject = NULL;

			// Check what happened during the drag-drop operation
			switch(hr)
			{
			case DRAGDROP_S_DROP:
				// A drop occurred - save the effect
				m_dwDropEffect = dwEffect;

				// Check if we did a move
				if(dwEffect & DROPEFFECT_MOVE)
				{
					// We moved some Markers - delete the Markers we marked earlier
					m_pMarkerMgr->DeleteMarked(UD_DRAGSELECT);
				}

				// If the effect wasn't 'None', set the drop flag to true
				if(dwEffect != DROPEFFECT_NONE)
				{
					fDrop = true;
				}
				break;
			default:
				// User cancelled the operation, or dropped the Markers somewhere they didn't belong
				m_dwDropEffect = DROPEFFECT_NONE;
				break;
			}

			// Check if we really did a drop
			if( fDrop )
			{
				// Check if we dropped in the same strip
				if( m_nStripIsDragDropSource == 2 )
				{
					// Source and target are the same strip
					if( dwEffect & DROPEFFECT_MOVE )
					{
						// We did a move - use the 'Move' undo text
						m_pMarkerMgr->m_nLastEdit = IDS_UNDO_MOVE;
					}
					else
					{
						// We did a copy - use the 'Insert' undo text
						m_pMarkerMgr->m_nLastEdit = IDS_INSERT;
					}

					// Notify the Segment Designer that we changed
					m_pMarkerMgr->OnDataChanged();

					// Update the selection regions to include the selected items
					SelectRegionsFromSelectedMarkers();

					// If visible, switch the property sheet to the Marker property page
					m_pMarkerMgr->OnShowProperties();

					// If it exists, refresh the Marker property page
					if( m_pMarkerMgr->m_pPropPageMgr )
					{
						m_pMarkerMgr->m_pPropPageMgr->RefreshData();
					}

					// Sync with DirectMusic
					m_pMarkerMgr->SyncWithDirectMusic();
				}
				else
				{
					// Target strip is different from source strip
					if( dwEffect & DROPEFFECT_MOVE )
					{
						// Check if we dropped in the same segment
						if( CMarkerStrip::m_fDragDropIntoSameDocRoot == TRUE )
						{
							// We did a move operation to another strip in the same segment - use the 'Move' undo text
							m_pMarkerMgr->m_nLastEdit = IDS_UNDO_MOVE;
						}
						else
						{
							// We did a move operation to another strip in a different segment - use the 'Delete' undo text
							m_pMarkerMgr->m_nLastEdit = IDS_DELETE;
						}

						// Notify the Segment Designer that we changed
						m_pMarkerMgr->OnDataChanged();

						// Sync with DirectMusic
						m_pMarkerMgr->SyncWithDirectMusic();
					}
					else
					{
						// Unselect all items, since the user did a copy operation
						m_pMarkerMgr->UnselectAll();
					}
				}

				// Refresh our strip display
				m_pMarkerMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

				// Return that we actually did a drop
				hr = S_OK;
			}
		}

		// Release the IDropSource interface
		pIDropSource->Release();

		// Clear the DragSelect flag from all items
		m_pMarkerMgr->UnMarkMarkers(UD_DRAGSELECT); 
	}
	else
	{
		// Couldn't query for IDropSource interface - return failure code
		hr = E_FAIL;
	}

	// Clear the DragDropSource flag
	m_nStripIsDragDropSource = 0;
	CMarkerStrip::m_pIDocRootOfDragDropSource = NULL;
	CMarkerStrip::m_fDragDropIntoSameDocRoot = FALSE;

	// Return a success or failure code
	return hr;
}


////////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::SelectItemsInSelectedRegions

bool CMarkerStrip::SelectItemsInSelectedRegions()
{
	// Flag set to true if anything changes
	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = m_pMarkerMgr->m_lstCueMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CMarkerItem* pMarkerItem = m_pMarkerMgr->m_lstCueMarkers.GetNext( pos );

		// Check if this item is within a selection region
		if(m_pSelectedCueGridRegions->Contains(pMarkerItem->m_lMeasure, pMarkerItem->m_lBeat, pMarkerItem->m_lGrid))
		{
			// Check if the item is not selected
			if( !pMarkerItem->m_fSelected )
			{
				// Marker isn't yet selected - select it and set fChanged to true
				pMarkerItem->m_fSelected = TRUE;
				fChanged = true;
			}
		}
		else
		{
			// Check if the item is selected
			if( pMarkerItem->m_fSelected )
			{
				// Marker is selected - deselect it and set fChagned to true
				pMarkerItem->m_fSelected = FALSE;
				fChanged = true;
			}
		}
	}

	pos = m_pMarkerMgr->m_lstSwitchMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CMarkerItem* pMarkerItem = m_pMarkerMgr->m_lstSwitchMarkers.GetNext( pos );

		// Check if this item is within a selection region
		if(m_pSelectedSwitchGridRegions->Contains(pMarkerItem->m_lMeasure, pMarkerItem->m_lBeat, pMarkerItem->m_lGrid))
		{
			// Check if the item is not selected
			if( !pMarkerItem->m_fSelected )
			{
				// Marker isn't yet selected - select it and set fChanged to true
				pMarkerItem->m_fSelected = TRUE;
				fChanged = true;
			}
		}
		else
		{
			// Check if the item is selected
			if( pMarkerItem->m_fSelected )
			{
				// Marker is selected - deselect it and set fChagned to true
				pMarkerItem->m_fSelected = FALSE;
				fChanged = true;
			}
		}
	}

	// return whether or not we changed the selection state of any items
	return fChanged;
}


////////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::SelectRegionsFromSelectedMarkers

void CMarkerStrip::SelectRegionsFromSelectedMarkers()
{
	// Clear the list of selected regions
	m_pSelectedCueGridRegions->Clear();
	m_pSelectedSwitchGridRegions->Clear();

	// Iterate through the list of items
	POSITION pos = m_pMarkerMgr->m_lstCueMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CMarkerItem* pMarkerItem = m_pMarkerMgr->m_lstCueMarkers.GetNext( pos );

		// Check if the item is selected
		if(pMarkerItem->m_fSelected)
		{
			// Add the grid the item is in to the list of selected regions.
			CListSelectedGridRegion_AddRegion(*m_pSelectedCueGridRegions, *pMarkerItem);
		}
	}

	pos = m_pMarkerMgr->m_lstSwitchMarkers.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CMarkerItem* pMarkerItem = m_pMarkerMgr->m_lstSwitchMarkers.GetNext( pos );

		// Check if the item is selected
		if(pMarkerItem->m_fSelected)
		{
			// Add the grid the item is in to the list of selected regions.
			CListSelectedGridRegion_AddRegion(*m_pSelectedSwitchGridRegions, *pMarkerItem);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::IsSelected

bool CMarkerStrip::IsSelected()
{
	// Verify pointer
	if( !m_pMarkerMgr )
	{
		return false;
	}

	// If anything is selected, return true
	
	// Iterate through the list of items
	POSITION pos = m_pMarkerMgr->m_lstCueMarkers.GetHeadPosition();
	while( pos )
	{
		// Check if this item is selected
		if ( m_pMarkerMgr->m_lstCueMarkers.GetNext( pos )->m_fSelected )
		{
			// Marker is selected - return true
			return true;
		}
	}

	// Iterate through the list of items
	pos = m_pMarkerMgr->m_lstSwitchMarkers.GetHeadPosition();
	while( pos )
	{
		// Check if this item is selected
		if ( m_pMarkerMgr->m_lstSwitchMarkers.GetNext( pos )->m_fSelected )
		{
			// Marker is selected - return true
			return true;
		}
	}

	// No items selected - return false
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::IsEmpty

bool CMarkerStrip::IsEmpty()
{
	return m_pMarkerMgr ? ((m_pMarkerMgr->m_lstSwitchMarkers.IsEmpty() && m_pMarkerMgr->m_lstCueMarkers.IsEmpty()) ? true : false) : false;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::SelectSegment

// Return true if anything changed
bool CMarkerStrip::SelectSegment(MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime)
{
	// Verify that we have a pointer to the Timeline
	if( !m_pMarkerMgr || !m_pMarkerMgr->m_pTimeline )
	{
		ASSERT(FALSE);
		return false;
	}

	// Convert the start position from clocks to a measure, beat, and grid value
	long lMeasure, lBeat, lGrid;
	m_pMarkerMgr->ClocksToMeasureBeatGrid( mtBeginTime, &lMeasure, &lBeat, &lGrid );
	
	// Convert back to a clock value
	m_pMarkerMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, lGrid, 0, &mtBeginTime );

	// Convert the end position from clocks to a measure and beat value
	m_pMarkerMgr->ClocksToMeasureBeatGrid( mtEndTime, &lMeasure, &lBeat, &lGrid );
	
	// Increment the grid so the last grid is selected
	lGrid++;

	// Convert back to a clock value
	m_pMarkerMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, lGrid, 0, &mtEndTime );

	// Convert the passed in times to a generic time class
	CMusicTimeGridConverter cmgtBeg(mtBeginTime);
	CMusicTimeGridConverter cmgtEnd(mtEndTime);

	// Create a region that contains the selected time
	CSelectedGridRegion region(cmgtBeg, cmgtEnd);

	// Add the region to the list of selected regions
	m_pSelectedCueGridRegions->AddRegion(region);
	m_pSelectedSwitchGridRegions->AddRegion(region);

	// Select all items in the list of selected regions
	// This will return true if the selection state of any item changed
	return SelectItemsInSelectedRegions();
}


/////////////////////////////////////////////////////////////////////////////
// Helper methods

/////////////////////////////////////////////////////////////////////////////
// CopyMarkerDataToClipboard

HRESULT CopyMarkerDataToClipboard( IDMUSProdTimelineDataObject* pITimelineDataObject, IStream* pStreamCopy, UINT uiClipFormat, CBaseMgr *pBaseMgr, CMarkerStrip *pMarkerStrip )
{
	HRESULT hr;
	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed IDMUSProdTimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( uiClipFormat, pStreamCopy );
		if ( hr != S_OK )
		{
			ASSERT(FALSE);
			return E_FAIL;
		}
	}
	else
	{
		// There is no existing data object, so just create a new one
		hr = pBaseMgr->m_pTimeline->AllocTimelineDataObject(&pITimelineDataObject);
		ASSERT(hr == S_OK);
		if(hr != S_OK)
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lRegionStart, lRegionEnd;
		pMarkerStrip->m_pSelectedCueGridRegions->GetSpan(lRegionStart, lRegionEnd);
		long lSwitchStart, lSwitchEnd;
		pMarkerStrip->m_pSelectedSwitchGridRegions->GetSpan(lSwitchStart, lSwitchEnd);

		// Ensure we're using the maximums and minimums
		lRegionStart = min( lRegionStart, lSwitchStart );
		lRegionEnd = min( lRegionEnd, lSwitchEnd );

		hr = pITimelineDataObject->SetBoundaries(lRegionStart, (lRegionEnd - 1));

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( uiClipFormat, pStreamCopy );
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}

		// get the new IDataObject to place on the clipboard
		IDataObject* pIDataObject;
		hr = pITimelineDataObject->Export(&pIDataObject);

		// Release the ITimelineDataObject
		pITimelineDataObject->Release();

		// Exit if the Export failed
		if(FAILED(hr))
		{
			return E_UNEXPECTED;
		}

		// Send the IDataObject to the clipboard
		hr = OleSetClipboard( pIDataObject );
		if( hr != S_OK )
		{
			pIDataObject->Release();
			return E_FAIL;
		}

		// If we already have a pBaseMgr->m_pCopyDataObject, release it
		if(pBaseMgr->m_pCopyDataObject)
		{
			pBaseMgr->m_pCopyDataObject->Release();
		}

		// set pBaseMgr->m_pCopyDataObject to the object we just copied to the clipboard
		pBaseMgr->m_pCopyDataObject = pIDataObject;

		// Not needed = Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		// pBaseMgr->m_pCopyDataObject->AddRef
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::MarkAllHelper

HRESULT CMarkerStrip::MarkAllHelper( BOOL fInsert, DWORD dwFlags )
{
	// Get the length of the segment, in clocks
	VARIANT var;
	if( FAILED( m_pMarkerMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var) ) )
	{
		return E_UNEXPECTED;
	}

	CWaitCursor waitCursor;
	if( MarkTimeHelper( fInsert, dwFlags, 0, V_I4(&var) - 1 ) )
	{
		// Redraw our strip
		m_pMarkerMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

		// Switch to the Marker property page
		m_pMarkerMgr->OnShowProperties();

		// Refresh the Marker property page
		if( m_pMarkerMgr->m_pPropPageMgr )
		{
			m_pMarkerMgr->m_pPropPageMgr->RefreshData();
		}

		// Notify the Segment Designer that an insert occurred
		m_pMarkerMgr->m_nLastEdit = IDS_INSERT;
		m_pMarkerMgr->OnDataChanged();

		// Sync with DirectMusic
		m_pMarkerMgr->SyncWithDirectMusic();
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::MarkRangeHelper

HRESULT CMarkerStrip::MarkRangeHelper( BOOL fInsert, DWORD dwFlags )
{
	// Get the type of marker to insert
	const MARKER_TYPE typeMarker = (m_lYPos < DEFAULT_STRIP_HEIGHT / 2) ? MARKER_CUE : MARKER_SWITCH;

	CListSelectedGridRegion *plstSelectedGridRegions;
	switch( typeMarker )
	{
	case MARKER_SWITCH:
		plstSelectedGridRegions = m_pSelectedSwitchGridRegions;
		break;

	case MARKER_CUE:
		plstSelectedGridRegions = m_pSelectedCueGridRegions;
		break;
	default:
		ASSERT(FALSE);
		return E_FAIL;
	}

	CWaitCursor waitCursor;

	// Sort the list of selected regions
	plstSelectedGridRegions->Sort();

	// Flag if anything changed
	BOOL fChanged = FALSE;

	// Get the start position of the region list
	POSITION posRegion = plstSelectedGridRegions->GetHeadPosition();
	while(posRegion)
	{
		// Get a pointer to this region
		CSelectedGridRegion* psr = plstSelectedGridRegions->GetNext(posRegion);

		// Get the start and end times for this region
		MUSIC_TIME mtStart = psr->Beg();
		MUSIC_TIME mtEnd = psr->End() - 1;

		fChanged |= MarkTimeHelper( fInsert, dwFlags, mtStart, mtEnd );
	}

	if( fChanged )
	{
		// Redraw our strip
		m_pMarkerMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

		// Switch to the Marker property page
		m_pMarkerMgr->OnShowProperties();

		// Refresh the Marker property page
		if( m_pMarkerMgr->m_pPropPageMgr )
		{
			m_pMarkerMgr->m_pPropPageMgr->RefreshData();
		}

		// Notify the Segment Designer that an insert occurred
		m_pMarkerMgr->m_nLastEdit = IDS_INSERT;
		m_pMarkerMgr->OnDataChanged();

		// Sync with DirectMusic
		m_pMarkerMgr->SyncWithDirectMusic();
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::MarkTimeHelper

BOOL CMarkerStrip::MarkTimeHelper( BOOL fInsert, DWORD dwFlags, MUSIC_TIME mtStart, MUSIC_TIME mtEnd )
{
	// Validate mtStart and mtEnd;
	ASSERT( mtStart <= mtEnd );

	// Validate dwFlags
	ASSERT( dwFlags == DMUS_SEGF_MEASURE
		||	dwFlags == DMUS_SEGF_BEAT
		||	dwFlags == DMUS_SEGF_GRID );

	// Unselect all items in other strips
	UnselectGutterRange();

	// Local time pointer
	MUSIC_TIME mtCurrent = mtStart;

	// Ensure mtCurrent is on an even beat or bar boundary
	long lMeasure, lBeat;
	m_pMarkerMgr->m_pTimeline->ClocksToMeasureBeat( m_pMarkerMgr->m_dwGroupBits, 0,
								mtCurrent, &lMeasure, &lBeat );
	if( dwFlags == DMUS_SEGF_MEASURE )
	{
		// If we start after the first beat, start in the next measure
		if( lBeat != 0 )
		{
			lMeasure++;
			lBeat = 0;
		}

		m_pMarkerMgr->m_pTimeline->MeasureBeatToClocks( m_pMarkerMgr->m_dwGroupBits, 0,
									lMeasure, 0, &mtCurrent );
	}
	else // DMUS_SEGF_BEAT or DMUS_SEGF_GRID
	{
		m_pMarkerMgr->m_pTimeline->MeasureBeatToClocks( m_pMarkerMgr->m_dwGroupBits, 0,
									lMeasure, lBeat, &mtCurrent );
	}

	// Get the type of marker to insert
	const MARKER_TYPE typeMarker = (m_lYPos < DEFAULT_STRIP_HEIGHT / 2) ? MARKER_CUE : MARKER_SWITCH;

	// Flag when something changes
	BOOL fChanged = FALSE;


	while( mtCurrent < mtEnd )
	{
		// Get the TimeSig for this measure
		DMUS_TIMESIGNATURE ts;
		if( FAILED( m_pMarkerMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pMarkerMgr->m_dwGroupBits, 0, mtCurrent, NULL, &ts ) ) )
		{
			break;
		}

		// The number of beats to add to complete the measure
		const long lBeatsLeft = ts.bBeatsPerMeasure - lBeat;

		// Compute the number of clocks in a beat
		const long lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;

		if( dwFlags == DMUS_SEGF_MEASURE )
		{
			if( mtCurrent >= mtStart )
			{
				fChanged |= InsertRemoveMarkerAtTime( fInsert, typeMarker, mtCurrent, lMeasure, 0, 0 );
			}
		}
		else // DMUS_SEGF_BEAT or DMUS_SEGF_GRID
		{
			// Insert Markers at each beat in the bar
			MUSIC_TIME mtBeat = mtCurrent;
			while( lBeat < ts.bBeatsPerMeasure
				&& mtBeat < mtEnd )
			{
				if( mtBeat >= mtStart )
				{
					fChanged |= InsertRemoveMarkerAtTime( fInsert, typeMarker, mtBeat, lMeasure, lBeat, 0 );
				}

				if( dwFlags == DMUS_SEGF_GRID )
				{
					// Insert Markers at grid in this beat
					const long lGridClocks = lBeatClocks / ts.wGridsPerBeat;
					MUSIC_TIME mtGrid = mtBeat + lGridClocks;
					long lGrid = 1;
					while( lGrid < ts.wGridsPerBeat )
					{
						if( mtGrid >= mtStart
						&&	mtGrid < mtEnd )
						{
							fChanged |= InsertRemoveMarkerAtTime( fInsert, typeMarker, mtGrid, lMeasure, lBeat, lGrid );
						}
						mtGrid += lGridClocks;
						lGrid++;
					}
				}

				mtBeat += lBeatClocks;
				lBeat++;
			}

			// Reset the beat counter
			lBeat = 0;
		}

		lMeasure++;
		mtCurrent += lBeatClocks * lBeatsLeft;
	}

	return fChanged;
}


//////////////////////////////////////////////////////////////////////////////
// CMarkerStrip::InsertRemoveMarkerAtTime

BOOL CMarkerStrip::InsertRemoveMarkerAtTime( BOOL fInsert, MARKER_TYPE typeMarker, MUSIC_TIME mtTime, long lMeasure, long lBeat, long lGrid )
{
	if( fInsert )
	{
		// Look for an item on the given lMeasure, lBeat, and lGrid
		CMarkerItem *pMarkerItem = GetTopItemFromMeasureBeatGrid( typeMarker, lMeasure, lBeat, lGrid );

		// If we didn't find an item
		if( !pMarkerItem )
		{
			// Insert one
			pMarkerItem = new CMarkerItem;
			if(pMarkerItem == NULL)
			{
				return FALSE;
			}

			// Set the measure and beat and grid the item was inserted in
			pMarkerItem->m_lMeasure = lMeasure;
			pMarkerItem->m_lBeat = lBeat;
			pMarkerItem->m_lGrid = lGrid;
			pMarkerItem->m_typeMarker = typeMarker;

			// Set the clock time the item was inserted on
			pMarkerItem->m_mtTime = mtTime;

			// By default, a newly inserted item is selected
			pMarkerItem->m_fSelected = TRUE;

			// Insert the item into our list of Markers, overwriting any existing one
			m_pMarkerMgr->InsertByAscendingTime(pMarkerItem);

			// If it's not already selected, add the grid the item was inserted on
			// to the list of selected regions
			switch( typeMarker )
			{
			case MARKER_CUE:
				if( !m_pSelectedCueGridRegions->Contains( lMeasure, lBeat, lGrid ) )
				{
					m_pSelectedCueGridRegions->AddRegion( CSelectedGridRegion( CMusicTimeGridConverter( mtTime ), CMusicTimeGridConverter( lMeasure, lBeat, lGrid, m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits, 0 ) ) );
				}
				break;
			case MARKER_SWITCH:
				if( !m_pSelectedSwitchGridRegions->Contains( lMeasure, lBeat, lGrid ) )
				{
					m_pSelectedSwitchGridRegions->AddRegion( CSelectedGridRegion( CMusicTimeGridConverter( mtTime ), CMusicTimeGridConverter( lMeasure, lBeat, lGrid, m_pMarkerMgr->m_pTimeline, m_pMarkerMgr->m_dwGroupBits, 0 ) ) );
				}
				break;
			}

			// We inserted an item
			return TRUE;
		}
	}
	else
	{
		// Look for an item on the given lMeasure, lBeat, and lGrid
		CMarkerItem *pMarkerItem = GetTopItemFromMeasureBeatGrid( typeMarker, lMeasure, lBeat, lGrid );
		if( pMarkerItem )
		{
			// Continue and remove all items from that lMeasure, lBeat, and lGrid
			while( pMarkerItem )
			{
				m_pMarkerMgr->RemoveItem( pMarkerItem );
				delete pMarkerItem;
				pMarkerItem = GetTopItemFromMeasureBeatGrid( typeMarker, lMeasure, lBeat, lGrid );
			}

			// We removed an item
			return TRUE;
		}
	}

	return FALSE;
}
