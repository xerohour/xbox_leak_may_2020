// SegmentStrip.cpp : Implementation of CSegmentStrip
#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackItem.h"
#include "SegmentStripMgr.h"
#include "TrackMgr.h"
#include "PropPageMgr_Item.h"
#include "GroupBitsPPG.h"
#include "BaseMgr.h"
#include "SegmentIO.h"
#include "DLLJazzDataObject.h"
#include <dmusicf.h>
#include "GrayOutRect.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This sets the strip's height
#define DEFAULT_STRIP_HEIGHT 20

const DWORD g_dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
const DWORD g_dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;

/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip constructor/destructor

CSegmentStrip::CSegmentStrip( CTrackMgr* pTrackMgr ) : CBaseStrip( pTrackMgr )
{
	// Validate the pointer to our strip manager
	ASSERT( pTrackMgr );
	if ( pTrackMgr == NULL )
	{
		return;
	}

	// Set our pointer to our strip manager
	m_pTrackMgr = pTrackMgr;

	// Initialize our clipboard formats to 0
	m_cfSegmentTrack = 0;
	m_cfSegment = 0;
	m_cfStyle = 0;

	// Initialize our state variables to false
	m_fLeftMouseDown = false;

	// Initialize the item to toggle (when CTRL-clicking) to NULL
	m_pItemToToggle = NULL;
}

CSegmentStrip::~CSegmentStrip()
{
	// Clear our pointer to our strip manager
	ASSERT( m_pTrackMgr );
	if ( m_pTrackMgr )
	{
		m_pTrackMgr = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::GetTopItemFromMeasureBeat

CTrackItem* CSegmentStrip::GetTopItemFromMeasureBeat( long lMeasure, long lBeat )
{
	CTrackItem* pTheItem = NULL;
	CTrackItem* pFirstItem = NULL;
	CTrackItem* pItem;

	POSITION pos = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

		if( pItem->m_lMeasure == lMeasure
		&&  pItem->m_lBeat == lBeat )
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
// CSegmentStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// GetNextSelectedItem - helper method for Draw()

void GetNextSelectedItem( CTypedPtrList<CPtrList, CTrackItem*>& list, POSITION &posItem )
{
	// Note that if the item at posItem is selected, this method doesn't do anything useful.
	if( posItem )
	{
		// Save the current position
		POSITION posToSave = posItem;

		// Check if we have not run off the end of the list, and if the currently item is unselected
		while( posItem && !list.GetNext( posItem )->m_fSelected )
		{
			// Current item is unselected, save the position of the next item
			posToSave = posItem;
		}

		// Check if we found a selected item
		if( posToSave )
		{
			// Save the position of the selected item
			posItem = posToSave;

		}
		// Otherwise both posToSave and posItem are NULL
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::AdjustTopItem - helper method for Draw()

void CSegmentStrip::AdjustTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CTrackItem* pFirstSelectedItem = NULL;
	CTrackItem* pTopItem = NULL;
	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

		if( pItem->m_lMeasure != lCurrentMeasure
		||  pItem->m_lBeat != lCurrentBeat )
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
// CSegmentStrip::GetNextSelectedTopItem - helper method for Draw()

CTrackItem* CSegmentStrip::GetNextSelectedTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CTrackItem* pNextItem = NULL;
	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

		if( pItem->m_fSelected )
		{
			if( pItem->m_lMeasure != lCurrentMeasure
			||  pItem->m_lBeat != lCurrentBeat )
			{
				pNextItem = GetTopItemFromMeasureBeat( pItem->m_lMeasure, pItem->m_lBeat );
				ASSERT( pNextItem != NULL );	// Should not happen!
				break;
			}
		}
	}

	return pNextItem;
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::GetNextTopItem - helper method for Draw()

CTrackItem* CSegmentStrip::GetNextTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CTrackItem* pNextItem = NULL;
	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

		if( pItem->m_lMeasure != lCurrentMeasure
		||  pItem->m_lBeat != lCurrentBeat )
		{
			pNextItem = GetTopItemFromMeasureBeat( pItem->m_lMeasure, pItem->m_lBeat );
			ASSERT( pNextItem != NULL );	// Should not happen!
			break;
		}
	}

	return pNextItem;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::Draw

HRESULT	STDMETHODCALLTYPE CSegmentStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that our timeline pointer is valid
	if( m_pTrackMgr == NULL
	||	m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Draw bar and beat lines
	CBaseStrip::Draw( hDC, sv, lXOffset );

	// Set a flag if we should use the gutter range for our selection
	bool fUseGutterSelectRange = m_bGutterSelected && (m_lGutterBeginSelect != m_lGutterEndSelect);

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
	m_pTrackMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &lStartTime );

	// Create Italics font
	HFONT hFontItalics = NULL;
	HFONT hFont = (HFONT)::GetCurrentObject( hDC, OBJ_FONT );
	if( hFont )
	{
		LOGFONT logfont;

		::GetObject( hFont, sizeof(LOGFONT), &logfont );
		logfont.lfItalic = TRUE;
		hFontItalics = CreateFontIndirect( &logfont );
	}

	// A structure to store the size of each item in
	CString strText;
	SIZE sizeText;

	// Fields to keep track of top item on current measure/beat
	CTrackItem* pTopItem;

	// Fields to keep track of ghost item
	CTrackItem* pGhostItem = NULL;

	/////////////////////////////////////////////////////////////////////////
	// First, see if there is an item that needs to be ghosted

	// Iterate through the list of items to find the item to be ghosted
	long lPosition;
	long lCurrentMeasure = -1;
	long lCurrentBeat = -1;
	POSITION posItem = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();
	while( posItem )
	{
		// Save position
		POSITION posLast = posItem;

		// Get a pointer to each item
		CTrackItem* pItem = m_pTrackMgr->m_lstTrackItems.GetNext( posItem );

		if( pItem->m_lMeasure != lCurrentMeasure
		||  pItem->m_lBeat != lCurrentBeat )
		{
			lCurrentMeasure = pItem->m_lMeasure;
			lCurrentBeat = pItem->m_lBeat;

			// Cleanup
			AdjustTopItem( posLast, lCurrentMeasure, lCurrentBeat );

			// Get the "top" item on this beat
			pTopItem = GetTopItemFromMeasureBeat( lCurrentMeasure, lCurrentBeat );
			if( pTopItem == NULL )
			{
				ASSERT( 0 );	// Should not happen!
				continue;
			}

			// Convert the measure and beat of each item to a pixel position
			m_pTrackMgr->m_pTimeline->MeasureBeatToPosition( m_pTrackMgr->m_dwGroupBits, 0, lCurrentMeasure, lCurrentBeat, &lPosition );

			// Offset the position by the horizontal offset,
			// and increment it by one pixel so we don't overwrite the bar or beat line
			lPosition += -lXOffset + 1;

			// Check if the left edge of the item text starts after the region we're displaying
			if( lPosition > rectClip.left )
			{
				// This item is after start of the display - break out of the loop
				break;
			}

			// Determine the text to display in the track
			pTopItem->FormatUIText( strText );

			// Get the size of the item text to be written
			::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );

			// Check if the right edge of the item text extends into the region we're displaying
			if( (lPosition + sizeText.cx) <= rectClip.left )
			{
				// Item does not extend into the region we're displaying
				pGhostItem = pTopItem;		// Potential ghost item
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// Now, draw the ghost item and unselected items 

	// Iterate through the list of items
	lCurrentMeasure = -1;
	lCurrentBeat = -1;
	posItem = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();
	while( posItem )
	{
		// Save position
		POSITION posLast = posItem;

		// Get a pointer to each item
		CTrackItem* pItem = m_pTrackMgr->m_lstTrackItems.GetNext( posItem );

		// No need to do anything before we reach the ghost item
		if( pGhostItem
		&&  pGhostItem != pItem )
		{
			continue;
		}

		if( pItem->m_lMeasure != lCurrentMeasure
		||  pItem->m_lBeat != lCurrentBeat )
		{
			lCurrentMeasure = pItem->m_lMeasure;
			lCurrentBeat = pItem->m_lBeat;

			if( pItem == pGhostItem )
			{
				pTopItem = pGhostItem;

				// Determine first visible measure/beat
				long lClocks, lFirstMeasure, lFirstBeat, lFirstTick;
				m_pTrackMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks );
				m_pTrackMgr->ClocksToMeasureBeatTick( lClocks, &lFirstMeasure, &lFirstBeat, &lFirstTick );
				if( lFirstTick > 0 )
				{
					lFirstBeat++;
				}

				// Convert the measure and beat to a pixel position
				m_pTrackMgr->m_pTimeline->MeasureBeatToPosition( m_pTrackMgr->m_dwGroupBits, 0, lFirstMeasure, lFirstBeat, &lPosition );
			}
			else
			{
				// Cleanup
				AdjustTopItem( posLast, lCurrentMeasure, lCurrentBeat );

				// Get the "top" item on this beat
				pTopItem = GetTopItemFromMeasureBeat( lCurrentMeasure, lCurrentBeat );
				if( pTopItem == NULL )
				{
					ASSERT( 0 );	// Should not happen!
					continue;
				}

				// If this item is selected and we are not using the gutter selection, skip it
				if( !fUseGutterSelectRange 
				&&  pTopItem->m_fSelected )
				{
					continue;
				}

				// Convert the measure and beat of each item to a pixel position
				m_pTrackMgr->m_pTimeline->MeasureBeatToPosition( m_pTrackMgr->m_dwGroupBits, 0, lCurrentMeasure, lCurrentBeat, &lPosition );
			}

			// Offset the position by the horizontal offset,
			// and increment it by one pixel so we don't overwrite the bar or beat line
			lPosition += -lXOffset + 1;

			// Check if the left edge of the item text is beyond the region we're displaying
			if( lPosition > rectClip.right )
			{
				// We've gone beyond the right edge of the clipping region - break out of the loop
				break;
			}

			// Set the left edge of the highlight rectangle to the start of the text, minus the horizontal offset
			rectHighlight.left = lPosition;

			// Store original text color
			COLORREF cr = ::GetTextColor( hDC );

			// Use grey if ghost item
			if( pTopItem == pGhostItem )
			{
				::SetTextColor( hDC, RGB(168,168,168) );
			}

			// Use italics if more than one item is on this beat
			HFONT hFontOld = NULL;
			if( hFontItalics )
			{
				if( pTopItem != pItem )
				{
					hFontOld = (HFONT)::SelectObject( hDC, hFontItalics );
				}
				else
				{
					if( posItem )
					{
						POSITION posNext = posItem;
						CTrackItem* pNextItem = m_pTrackMgr->m_lstTrackItems.GetNext( posNext );

						if( pNextItem->m_lMeasure == lCurrentMeasure
						&&  pNextItem->m_lBeat == lCurrentBeat )
						{
							hFontOld = (HFONT)::SelectObject( hDC, hFontItalics );
						}
					}
				}
			}

			// Determine the text to display in the track
			pTopItem->FormatUIText( strText );

			// Get a pointer to the next "top" item occuring AFTER this beat
			CTrackItem* pNextItem = GetNextTopItem( posItem, lCurrentMeasure, lCurrentBeat );

			// Check if there is a item after this one
			if( pNextItem ) 
			{
				// Get the size of the item text to be written
				::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );

				// Get the start position of the next item
				m_pTrackMgr->m_pTimeline->MeasureBeatToPosition( m_pTrackMgr->m_dwGroupBits, 0, pNextItem->m_lMeasure, pNextItem->m_lBeat, &(rectHighlight.right) );

				// Offset this position with lXOffset
				rectHighlight.right -= lXOffset;

				// Check if the current item will run into the next item
				if( (rectHighlight.left + sizeText.cx) > rectHighlight.right )
				{
					// Yes, it will run into the next item - clip it
					::DrawText( hDC, strText, strText.GetLength(), &rectHighlight, (DT_LEFT | DT_NOPREFIX) );
				}
				else
				{
					// It won't run into the next item - just draw it
					::TextOut( hDC, lPosition, 0, strText, strText.GetLength() );
				}
			}
			else
			{
				// No more items after this one - just draw it
				::TextOut( hDC, lPosition, 0, strText, strText.GetLength() );
			}

			if( hFontOld )
			{
				::SelectObject( hDC, hFontOld );
			}

			// Restore original text color
			::SetTextColor( hDC, cr );
		}

		// Clear pGhostItem after it has been displayed
		if( pTopItem == pGhostItem )
		{
			pGhostItem = NULL;
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// Now, draw the selected items

	// Iterate through the list of items, but only if we're not using the gutter selection
	lCurrentMeasure = -1;
	lCurrentBeat = -1;
	posItem = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();
	while( !fUseGutterSelectRange  &&  posItem )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_pTrackMgr->m_lstTrackItems.GetNext( posItem );

		if( pItem->m_lMeasure != lCurrentMeasure
		||  pItem->m_lBeat != lCurrentBeat )
		{
			lCurrentMeasure = pItem->m_lMeasure;
			lCurrentBeat = pItem->m_lBeat;

			// Get the "top" item on this beat
			pTopItem = GetTopItemFromMeasureBeat( lCurrentMeasure, lCurrentBeat );
			if( pTopItem == NULL )
			{
				ASSERT( 0 );	// Should not happen!
				continue;
			}

			// If this item is unselected, skip it
			if( !pTopItem->m_fSelected )
			{
				continue;
			}

			// Convert the measure and beat of each item to a pixel position
			m_pTrackMgr->m_pTimeline->MeasureBeatToPosition( m_pTrackMgr->m_dwGroupBits, 0, lCurrentMeasure, lCurrentBeat, &lPosition );

			// Offset the position by the horizontal offset,
			// and increment it by one pixel so we don't overwrite the bar or beat line
			lPosition += -lXOffset + 1;

			// Check if the left edge of the item text is beyond the region we're displaying
			if( lPosition > rectClip.right )
			{
				// We've gone beyond the right edge of the clipping region - break out of the loop
				break;
			}

			// Use italics if more than one item is on this beat
			HFONT hFontOld = NULL;
			if( hFontItalics )
			{
				if( pTopItem != pItem )
				{
					hFontOld = (HFONT)::SelectObject( hDC, hFontItalics );
				}
				else
				{
					if( posItem )
					{
						POSITION posNext = posItem;
						CTrackItem* pNextItem = m_pTrackMgr->m_lstTrackItems.GetNext( posNext );

						if( pNextItem->m_lMeasure == lCurrentMeasure
						&&  pNextItem->m_lBeat == lCurrentBeat )
						{
							hFontOld = (HFONT)::SelectObject( hDC, hFontItalics );
						}
					}
				}
			}

			// Set the left edge of the highlight rectangle to the start of the text, minus the horizontal offset
			rectHighlight.left = lPosition;

			// Determine the text to display in the track
			pTopItem->FormatUIText( strText );

			// Get a pointer to the next selected "top" item occuring AFTER this beat
			CTrackItem* pNextItem = GetNextSelectedTopItem( posItem, lCurrentMeasure, lCurrentBeat );

			// Check if there is a item after this one
			if( pNextItem ) 
			{
				// Get the size of the text to be written
				::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );

				// Get the start position of the next item
				m_pTrackMgr->m_pTimeline->MeasureBeatToPosition( m_pTrackMgr->m_dwGroupBits, 0, pNextItem->m_lMeasure, pNextItem->m_lBeat, &(rectHighlight.right) );

				// Offset this position with lXOffset
				rectHighlight.right -= lXOffset;

				// Check if the current item will run into the next item
				if( (rectHighlight.left + sizeText.cx) > rectHighlight.right )
				{
					// Yes, it will run into the next item - clip it
					::DrawText( hDC, strText, strText.GetLength(), &rectHighlight, (DT_LEFT | DT_NOPREFIX) );
				}
				else
				{
					// It won't run into the next item - just draw it
					::TextOut( hDC, lPosition, 0, strText, strText.GetLength() );
				}
			}
			// No more selected items
			else
			{
				// Just draw the item
				::TextOut( hDC, lPosition, 0, strText, strText.GetLength() );
			}

			if( hFontOld )
			{
				::SelectObject( hDC, hFontOld );
			}
		}
	}

	// Now, invert the selected regions
	if( m_pSelectedRegions )
	{
		// Keep a running count of the furthest right position we've inverted.
		// This ensures we don't double-invert.
		long lRightMostInvert = 0;

		// Sort the list of selected regions
		m_pSelectedRegions->Sort();

		// Get the start position of the item list
		posItem = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();

		// Get the start position of the region list
		POSITION posRegion = m_pSelectedRegions->GetHeadPosition();
		while( posRegion )
		{
			CTrackItem* pItem = NULL;

			// Get a pointer to the current item, but only when not using the gutter selection
			if( !fUseGutterSelectRange
			&&  posItem )
			{
				// Get the position of the next selected item (it may be the one at posItem)
				GetNextSelectedItem( m_pTrackMgr->m_lstTrackItems, posItem );

				// Check if we found a selected item
				if( posItem )
				{
					// Get a pointer to the selected item
					pItem = m_pTrackMgr->m_lstTrackItems.GetAt( posItem );

					// Make sure we have the top selected item!
					CTrackItem* pTopItem = GetNextSelectedTopItem( posItem, pItem->m_lMeasure, (pItem->m_lBeat - 1) );

					// Check if there is a item after this one
					if( pTopItem->m_lMeasure == pItem->m_lMeasure
					&&  pTopItem->m_lBeat == pItem->m_lBeat ) 
					{
						pItem = pTopItem;
					}
				}
			}

			// Get a pointer to this region
			CSelectedRegion* psr = m_pSelectedRegions->GetNext(posRegion);

			// Get the start and end positions for this region
			rectHighlight.left = psr->BeginPos(m_pTrackMgr->m_pTimeline) - lXOffset;
			rectHighlight.right = psr->EndPos(m_pTrackMgr->m_pTimeline) - lXOffset;

			// Check if we have a pointer to a selected item and if we're not using gutter selection
			if( !fUseGutterSelectRange 
			&&  pItem )
			{
				// Get the clock position of this item
				long lItemClockPos;
				m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( m_pTrackMgr->m_dwGroupBits, 0,
					pItem->m_lMeasure, pItem->m_lBeat, &lItemClockPos );

				while( posItem && (lItemClockPos < psr->End()) )
				{
					// Check if the current item is within this selected region
					if( lItemClockPos >= psr->Beg() )
					{
						// Compute the left side of the item's text display
						long lItemPos;
						m_pTrackMgr->m_pTimeline->ClocksToPosition( lItemClockPos, &lItemPos );

						// Get the size of the text to be written
						pItem->FormatUIText( strText );
						::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );

						// Add the size of the text and subtract the offset to
						// determine the right side of the item's text display.
						lItemPos += sizeText.cx - lXOffset;

						// Make sure we have the top selected item!
						pItem = GetNextSelectedTopItem( posItem, pItem->m_lMeasure, pItem->m_lBeat );

						// Check if we found a selected item
						if( pItem )
						{
							posItem = m_pTrackMgr->m_lstTrackItems.Find( pItem, posItem );

							// Get the clock position of this item
							m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( m_pTrackMgr->m_dwGroupBits, 0,
								pItem->m_lMeasure, pItem->m_lBeat, &lItemClockPos );

							// Compute the start position of the next selected item
							long lNextStartPos;
							m_pTrackMgr->m_pTimeline->ClocksToPosition( lItemClockPos, &lNextStartPos );

							// Ensure that we only exent the selection region up to the
							// start of the next selected item
							lItemPos = min( lNextStartPos, lItemPos );
						}
						else
						{
							posItem = NULL;
						}

						// Update rectHighlight.right, if necessary
						rectHighlight.right = max( lItemPos, rectHighlight.right );
					}
					else
					{
						// Move posItem so we check the next item
						m_pTrackMgr->m_lstTrackItems.GetNext( posItem );

						// Get the position of the next selected item (it may be the one at posItem)
						GetNextSelectedItem( m_pTrackMgr->m_lstTrackItems, posItem );

						// Check if we found a selected item
						if( posItem )
						{
							// Get a pointer to the selected item
							pItem = m_pTrackMgr->m_lstTrackItems.GetAt( posItem );

							// Get the clock position of this item
							m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( m_pTrackMgr->m_dwGroupBits, 0,
								pItem->m_lMeasure, pItem->m_lBeat, &lItemClockPos );
						}
					}
				}
			}

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

	if( hFontItalics )
	{
		::DeleteObject( hFontItalics );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::GetItemFromMeasureBeat

CTrackItem *CSegmentStrip::GetItemFromMeasureBeat( long lMeasure, long lBeat )
{
	// Iterate through the list
	POSITION pos = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

		// Check if it is in the measure and beat we're looking for
		if ( pItem->m_lMeasure == lMeasure &&
			 pItem->m_lBeat == lBeat )
		{
			// We found the item to return
			return pItem;
		}
		// Check if we've passed the measure yet
		else if ( pItem->m_lMeasure > lMeasure )
		{
			// We've passed the measure - break out of the loop
			break;
		}
	}

	// Didn't find an item - return NULL
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::GetItemFromPoint

CTrackItem *CSegmentStrip::GetItemFromPoint( long lPos )
{
	CTrackItem* pItemReturn = NULL;

	if( m_pTrackMgr->m_pTimeline )
	{
		long lMeasure, lBeat;
		if( SUCCEEDED( m_pTrackMgr->m_pTimeline->PositionToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, lPos, &lMeasure, &lBeat ) ) )
		{
			pItemReturn = GetTopItemFromMeasureBeat( lMeasure, lBeat );
		}
	}

	return pItemReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CSegmentStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Initialize the return code to SUCCESS
	HRESULT hr = S_OK;

	// Validate our timeline pointer
	if( m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		// Call a function to handle the left mouse button press
		hr = OnLButtonDown( wParam, lParam, lXPos, lYPos );
		m_lXPos = lXPos;	// need to start drag drop from where selection started
		break;

	case WM_RBUTTONDOWN:
		// Call a function to handle the right mouse button press
		hr = OnRButtonDown( wParam, lParam, lXPos, lYPos );
		break;

	case WM_MOUSEMOVE:
		// Check if the left mouse button is down
		if( m_fLeftMouseDown )
		{
			// The user moved the mouse while the left mouse button was down -
			// do a drag-drop operation.
			hr = DoDragDrop( m_pTrackMgr->m_pTimeline, wParam, m_lXPos);

			// The above method returns after a drop, or after the user
			// cancels the operation.  In either case, we don't want to do
			// a drag-drop operation again.
			m_fLeftMouseDown = false;

			// If the drag-drop operatoin didn't complete
			if( hr != S_OK || m_dwDropEffect == DROPEFFECT_NONE )
			{
				// Store the position that the drag started at, because drag drop eats mouse up
				m_lXPos = lXPos;

				// Check if the control key was down
				if( wParam & MK_CONTROL )
				{
					// Finish the toggle operation on the item that was clicked on
					// when the left mouse button was pressed.
					if( m_pItemToToggle )
					{
						m_pItemToToggle->m_fSelected = !m_pItemToToggle->m_fSelected;
						m_pItemToToggle = NULL;

						// Update the selection regions to include just this selected item
						SelectRegionsFromSelectedItems();
					}
				}
				// Check if the shift key was not down
				else if( !(wParam & MK_SHIFT) )
				{
					// If an item was clicked on
					if(GetItemFromPoint( lXPos ))
					{
						// Update the selection regions to include only the selected items
						SelectRegionsFromSelectedItems();
					}
				}

				// Redraw the strip
				m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

				// Switch the property page to the track item property page
				m_pTrackMgr->OnShowProperties();
			}
			else
			{
				// successful drag drop--make sure that only the regions with
				// selected items are selected
				SelectRegionsFromSelectedItems();
			}

			// Refresh the track item property page, if it exists
			if( m_pTrackMgr->m_pPropPageMgr )
			{
				m_pTrackMgr->m_pPropPageMgr->RefreshData();
			}
		}
		break;

	case WM_LBUTTONUP:
		// Clear the flag so we don't start a drag-drop operation when the mouse moved
		m_fLeftMouseDown = false;

		// Check if the Ctrl key was pressed
		if( wParam & MK_CONTROL )
		{
			// Ctrl key pressed - toggle the selection state of the item that was clicked on
			if( m_pItemToToggle )
			{
				m_pItemToToggle->m_fSelected = !m_pItemToToggle->m_fSelected;
				m_pItemToToggle = NULL;

				// Update the selection regions to include just this selected item
				SelectRegionsFromSelectedItems();
			}
		}
		// Check if the Shift key was not pressed
		else if( !(wParam & MK_SHIFT) )
		{
			// Look for an item at the position clicked on
			CTrackItem* pItem = GetItemFromPoint( lXPos );
			if(pItem)
			{
				// Found an item
				// Unselect all items
				m_pTrackMgr->UnselectAll();

				// Mark the clicked on item as selected
				pItem->m_fSelected = TRUE;

				// Update the selection regions to include just this selected item
				SelectRegionsFromSelectedItems();
			}
		}

		// Update the position to inset at
		m_lXPos = lXPos;

		// Ensure all other strips are unselected
		UnselectGutterRange();

		// Redraw ourself
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

		// Refresh the track item property page, if it exists
		if( m_pTrackMgr->m_pPropPageMgr )
		{
			m_pTrackMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	case WM_COMMAND:
	{
		// We should only get this message in response to a selection in the right-click context menu.
		WORD wNotifyCode = HIWORD( wParam );	// notification code 
		WORD wID		 = LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
			case IDM_CYCLE_ITEMS:
				hr = m_pTrackMgr->CycleItems( m_lXPos );
				break;
			default:
				// Try the default message handler
				CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
			break;
		}
		break;
	}

	case WM_CREATE:
		// Register our clipboard formats
		RegisterClipboardFormats();

		// Fall through to CBaseStrip's message handler, since we want it to also
		// handle the WM_CREATE message

	default:
		// Try the default message handler
		// This handles WM_COMMAND and WM_CREATE
		CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::Copy

HRESULT CSegmentStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
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
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Make sure the clipboard formats have been registered
	if( RegisterClipboardFormats() == FALSE )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Create an IStream to save the selected items in.
	IStream* pStreamCopy;
	hr = CreateStreamOnHGlobal( NULL, TRUE, &pStreamCopy );

	// Verify that we were able to create a stream
	if( FAILED( hr ))
	{
		return E_OUTOFMEMORY;
	}

	// Get the span of the selected items
	long lStartTime, lEndTime;
	m_pSelectedRegions->GetSpan(lStartTime, lEndTime);

	// Convert the start time to a measure and beat
	long lMeasure, lBeats;
	m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0,
								lStartTime, &lMeasure, &lBeats );

	// Convert the start time to a number of beats
	MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, m_pTrackMgr->m_dwGroupBits, 0, lMeasure, lBeats, lBeats );

	// Convert the end time to a measure and beat
	long lEndBeats;
	m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0,
								lEndTime, &lMeasure, &lEndBeats );

	// Convert the end time to a number of beats
	MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, m_pTrackMgr->m_dwGroupBits, 0, lMeasure, lEndBeats, lEndBeats );

	// Compute the span of the region we're copying
	lEndBeats -= lBeats;

	// Save the number of beats in the region we're copying
	pStreamCopy->Write( &lEndBeats, sizeof(long), NULL );

	// Save the selected items into the stream
	hr = m_pTrackMgr->SaveSelectedItems( pStreamCopy, lBeats );

	// Check if the save succeeded
	if( FAILED( hr ))
	{
		// Save failed
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	// The save succeeded - copy the stream to the clipboard
	hr = CopyDataToClipboard( pITimelineDataObject, pStreamCopy, m_cfSegmentTrack, m_pTrackMgr, this );

	// Release our pointer to the stream
	pStreamCopy->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::PasteAt

HRESULT CSegmentStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lPasteTime, bool bDropNotPaste, BOOL &fChanged )
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
		if( m_pTrackMgr->m_pTimeline )
		{
			// Determine the measure and beat we're dropping in
			long lMeasure1, lBeat1;
			if(SUCCEEDED(m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, lPasteTime, &lMeasure1, &lBeat1 )))
			{
				// Compute the measure and beat the drag started from
				long lMeasure2, lBeat2;
				if(SUCCEEDED(m_pTrackMgr->m_pTimeline->PositionToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, m_lStartDragPosition, &lMeasure2, &lBeat2 )))
				{
					// Check if we dropped in the same measure we started the drag from.
					if(lMeasure1 == lMeasure2 && lBeat1 == lBeat2)
					{
						// Didn't move - exit early
						goto Leave;
					}
				}
			}
		}
	}

	// Handle CF_SEGMENTTRACK
	if( pITimelineDataObject->IsClipFormatAvailable( m_cfSegmentTrack ) == S_OK )
	{
		// Try and read the stream that contains the items
		IStream* pIStream;
		if( SUCCEEDED ( pITimelineDataObject->AttemptRead( m_cfSegmentTrack, &pIStream) ) )
		{
			// If we're pasting, read in the number of beats that this selection covers
			long lBeatsToPaste = -1;
			if( !bDropNotPaste )
			{
				pIStream->Read( &lBeatsToPaste, sizeof(long), NULL );
			}

			// Load the stream into a list of items
			CTypedPtrList<CPtrList, CTrackItem*> list;
			hr = LoadList( list, m_pTrackMgr->m_pDMProdFramework, m_pTrackMgr, pIStream );

			// Check if the load operation succeeded
			if ( SUCCEEDED(hr) )
			{
				if( m_pTrackMgr->m_pTimeline )
				{
					// Get the measure and beat of the drop or paste position
					long lMeasurePaste, lBeatPaste;
					if( FAILED( m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, 
																	max( lPasteTime, 0 ), &lMeasurePaste, &lBeatPaste ) ) )
					{
						hr = E_FAIL;
						goto Leave_1;
					}

					long lBeatStart;
					MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, m_pTrackMgr->m_dwGroupBits, 0, lMeasurePaste, lBeatPaste, lBeatStart );

					// The length of the segment, in beats
					long lBeatSegmentLength = LONG_MAX;

					// Check if there are any items to paste
					if( !list.IsEmpty() )
					{
						// Unselect all existing items in this strip so the only selected items are the dropped
						// or pasted ones
						m_pTrackMgr->UnselectAll();

						// Make sure the last item lands in the last beat or sooner
						// Retrieve the clock length of the segment
						VARIANT varLength;
						if( SUCCEEDED( m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
						{
							// Compute the measure and beat length of the segment
							long lMeasure, lBeat;
							if( SUCCEEDED( m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, 
																			V_I4(&varLength), &lMeasure, &lBeat ) ) )
							{
								// Compute the beat length of the segment
								MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, m_pTrackMgr->m_dwGroupBits, 0,
									lMeasure, lBeat, lBeatSegmentLength );

								// Make sure the last item lands in the last beat or sooner
								// list.GetTail()->m_mtTimePhysical stores beat offset of last item
								if( list.GetTail()->m_mtTimePhysical + lBeatStart >= lBeatSegmentLength )
								{
									lBeatStart = lBeatSegmentLength - list.GetTail()->m_mtTimePhysical - 1;
								}
							}
						}

						// list.GetHead()->m_mtTimePhysical stores beat offset of first item
						if( list.GetHead()->m_mtTimePhysical + lBeatStart < 0 )
						{
							lBeatStart = -list.GetHead()->m_mtTimePhysical;
						}
					}

					// Check if we're doing a paste
					if(!bDropNotPaste)
					{
						// We're doing a paste, so get the paste type
						TIMELINE_PASTE_TYPE tlPasteType;
						if( FAILED( m_pTrackMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
						{
							// Failed to get the paste type, so return with error code
							hr = E_FAIL;
							goto Leave_1;
						}

						// Check if we're doing a paste->overwrite
						if( tlPasteType == TL_PASTE_OVERWRITE )
						{
							// Count lBeatsToPaste beats from lBeatStart and convert to a measure and beat value
							long lmEnd, lbEnd;
							BeatsToMeasureBeat( m_pTrackMgr->m_pTimeline, m_pTrackMgr->m_dwGroupBits, 0,
								lBeatStart + lBeatsToPaste, lmEnd, lbEnd );

							// Now, delete any items that fall between the first and last beats we're pasting in
							fChanged = m_pTrackMgr->DeleteBetweenMeasureBeats(lMeasurePaste, lBeatPaste, lmEnd, lbEnd );
						}
					}

					// Iterate throught the list of items we loaded
					while( !list.IsEmpty() )
					{
						// Remove the head of the list
						CTrackItem* pItem = list.RemoveHead();

						// Check if the item will land before the end of the segment
						// pItem->m_mtTimePhysical stores the beat offset of the item
						if( pItem->m_mtTimePhysical + lBeatStart < lBeatSegmentLength )
						{
							MUSIC_TIME mtTime;
							long lMeasure, lBeat;

							// m_mtTimeLogical
							// When we saved this to a stream in CLyricMgr::SaveSelectedLyrics, we stored
							// the beat offset into m_mtTimeLogical.
							BeatsToMeasureBeat( m_pTrackMgr->m_pTimeline, m_pTrackMgr->m_dwGroupBits, 0,
												max( 0, pItem->m_mtTimeLogical + lBeatStart), lMeasure, lBeat );
							m_pTrackMgr->ForceBoundaries( lMeasure, lBeat, 0, &mtTime );
							pItem->SetTimeLogical( mtTime );

							// m_mtTimePhysical
							// When we saved this to a stream in CLyricMgr::SaveSelectedLyrics, we stored
							// the beat offset into m_mtTimePhysical.
							// m_lTick should already contain correct value 
							BeatsToMeasureBeat( m_pTrackMgr->m_pTimeline, m_pTrackMgr->m_dwGroupBits, 0,
												pItem->m_mtTimePhysical + lBeatStart, lMeasure, lBeat );
							m_pTrackMgr->MeasureBeatTickToClocks( lMeasure, lBeat, pItem->m_lTick, &mtTime );
							pItem->SetTimePhysical( mtTime, STP_LOGICAL_NO_ACTION );

							// When pasted or dropped, each item is selected
							pItem->m_fSelected = TRUE;

							// This will overwrite any item that already exists on the measure and
							// beat where pItem will be inserted
							m_pTrackMgr->InsertByAscendingTime( pItem, TRUE );

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

	// Handle CF_SEGMENT and CF_STYLE
	else if( (pITimelineDataObject->IsClipFormatAvailable( m_cfSegment ) == S_OK) 
		 ||  (pITimelineDataObject->IsClipFormatAvailable( m_cfStyle ) == S_OK) )
	{
		IDMUSProdNode* pIDocRootNode;
		IDataObject* pIDataObject;

		// Unselect all existing items in this strip so the only selected items are the dropped
		// or pasted ones
		m_pTrackMgr->UnselectAll();

		hr = pITimelineDataObject->Export( &pIDataObject );
		if( SUCCEEDED ( hr ) )
		{
			hr = m_pTrackMgr->m_pDMProdFramework->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode );
			if( SUCCEEDED ( hr ) )
			{
				GUID guidNodeId;
				if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
				{
					// Get the measure and beat of the drop or paste position
					long lMeasurePaste, lBeatPaste;
					if( SUCCEEDED( m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, 
																	max( lPasteTime, 0 ), &lMeasurePaste, &lBeatPaste ) ) )
					{
						CTrackItem* pItem = new CTrackItem( m_pTrackMgr );
						if( pItem )
						{
							if( IsEqualGUID ( guidNodeId, GUID_StyleNode ) == FALSE )
							{
								// Turn off motif flag
								pItem->m_dwFlagsDM &= ~DMUS_SEGMENTTRACKF_MOTIF;
							}

							hr = pItem->SetFileReference( pIDocRootNode );
							if( SUCCEEDED ( hr ) )
							{
								MUSIC_TIME mtTime;
								m_pTrackMgr->MeasureBeatTickToClocks( lMeasurePaste, lBeatPaste, 0, &mtTime );
								pItem->SetTimePhysical( mtTime, STP_LOGICAL_SET_DEFAULT );

								// When pasted or dropped, each item is selected
								pItem->m_fSelected = TRUE;

								// This will overwrite any item that already exists on the measure and
								// beat where pItem will be inserted
								m_pTrackMgr->InsertByAscendingTime( pItem, TRUE );

								// We changed
								fChanged = TRUE;
							}
							else
							{
								delete pItem;
							}
						}
					}
				}

				RELEASE( pIDocRootNode );
			}

			RELEASE( pIDataObject );
		}
	}

Leave:
	if( m_nStripIsDragDropSource )
	{
		// Drag/drop Target and Source are the same strip
		m_nStripIsDragDropSource = 2;
	}

	if( CSegmentStrip::m_pIDocRootOfDragDropSource
	&&  CSegmentStrip::m_pIDocRootOfDragDropSource == m_pTrackMgr->m_pIDocRootNode )
	{
		// Drag/drop Target and Source are the same file (i.e. Segment)
		CSegmentStrip::m_fDragDropIntoSameDocRoot = TRUE;
	}
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::Paste

HRESULT CSegmentStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

	// Validate our TrackMgr and Timeline pointers
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get a TimelineDataObject, if we don't already have one
	hr = GetTimelineDataObject( pITimelineDataObject, m_pTrackMgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Convert the paste position to clocks
	long lClocks;
	if( FAILED( m_pTrackMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lClocks) ) )
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
		SelectRegionsFromSelectedItems();

		// Notify the containing segment that we did a paste operation
		m_pTrackMgr->m_nLastEdit = IDS_PASTE;
		m_pTrackMgr->OnDataChanged();

		// Redraw our strip
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

		// Ensure the track item property page is visible
		m_pTrackMgr->OnShowProperties();

		// Refresh the track item property page
		if( m_pTrackMgr->m_pPropPageMgr )
		{
			m_pTrackMgr->m_pPropPageMgr->RefreshData();
		}

		// Sync with DirectMusic
		m_pTrackMgr->SyncWithDirectMusic();
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::Insert

HRESULT CSegmentStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Double-check that our timeline pointer is valid
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	if( m_lXPos < 0 )
	{
		return E_FAIL;
	}

	// Unselect all items in this strip 
	m_pTrackMgr->UnselectAll();

	// Unselect all items in other strips
	UnselectGutterRange();

	// Convert the insert position to a measure and beat value
	long lMeasure, lBeat;
	if( FAILED( m_pTrackMgr->m_pTimeline->PositionToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, max( 0, m_lXPos ), &lMeasure, &lBeat ) ) )
	{
		return E_UNEXPECTED;
	}

	CTrackItem* pItem = new CTrackItem( m_pTrackMgr );
	if( pItem == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Set the measure and beat the item was inserted in
	MUSIC_TIME mtTime;
	m_pTrackMgr->MeasureBeatTickToClocks( lMeasure, lBeat, 0, &mtTime );
	pItem->SetTimePhysical( mtTime, STP_LOGICAL_SET_DEFAULT );

	// By default, a newly selected item is selected
	pItem->m_fSelected = TRUE;

	// Insert the item into our list of items, overwriting any existing one
	m_pTrackMgr->InsertByAscendingTime( pItem, FALSE );

	// If it's not already selected, add the beat the item was inserted on
	// to the list of selected regions
	if( !m_pSelectedRegions->Contains( lMeasure, lBeat ) )
	{
		m_pSelectedRegions->AddRegion(m_lXPos);
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

	// Show the property sheet
	ShowPropertySheet();

	// Switch to the track item's property page
	m_pTrackMgr->OnShowProperties();

	// Refresh the track item property page
	if( m_pTrackMgr->m_pPropPageMgr )
	{
		m_pTrackMgr->m_pPropPageMgr->RefreshData();
	}

	// Notify the containing segment that an insert occurred
	m_pTrackMgr->m_nLastEdit = IDS_INSERT;
	m_pTrackMgr->OnDataChanged();

	// No need to sync with DirectMusic - the item starts out as empty

	// Return the success/failure code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::Delete

HRESULT CSegmentStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}

	// Delete all selected items
	m_pTrackMgr->DeleteSelectedItems();

	// Clear all selection ranges
	m_pSelectedRegions->Clear();

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	// Refresh the track item property page, if it exists
	if( m_pTrackMgr->m_pPropPageMgr )
	{
		m_pTrackMgr->m_pPropPageMgr->RefreshData();
	}

	// Notify the containing segment that a delete operation occurred
	m_pTrackMgr->m_nLastEdit = IDS_DELETE;
	m_pTrackMgr->OnDataChanged();

	// Sync with DirectMusic
	m_pTrackMgr->SyncWithDirectMusic();

	// Return that the delete succeeded
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::CanPaste

HRESULT CSegmentStrip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Make sure the clipboard formats have been registered
	if( RegisterClipboardFormats() == FALSE )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a TimelineDataObject, if we don't already have one
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_pTrackMgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check the timeline data object for our clipboard formats
	hr = S_FALSE;
	if( (pITimelineDataObject->IsClipFormatAvailable( m_cfSegmentTrack ) == S_OK)
	||  (pITimelineDataObject->IsClipFormatAvailable( m_cfSegment ) == S_OK) 
	||  (pITimelineDataObject->IsClipFormatAvailable( m_cfStyle ) == S_OK) )
	{
		// If we found our clipboard format, return S_OK
		hr = S_OK;
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return either S_OK or S_FALSE
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::CanInsert

HRESULT CSegmentStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid Timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get the position to paste at
	long lInsertTime;
	if( FAILED( m_pTrackMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lInsertTime) ) )
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
	if( FAILED( m_pTrackMgr->m_pTimeline->ClocksToPosition( lInsertTime, &lPosition ) ) )
	{
		return E_UNEXPECTED;
	}

	// Check to see if there is an existing item at the insert position
	CTrackItem* pItem = GetItemFromPoint( lPosition );

	// Check if we found an item
	if( pItem != NULL )
	{
		// We found an item - return S_FALSE since we can't insert here
		return S_FALSE;
	}

	// Get the length of the segment, in clocks
	VARIANT var;
	if( FAILED( m_pTrackMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var) ) )
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
// CSegmentStrip::DragOver

HRESULT CSegmentStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pt);

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	if( m_pITargetDataObject == NULL )
	{
		ASSERT( FALSE ); // Shouldn't happen - CanPasteFromData will return E_POINTER.
	}

	if( CanPasteFromData( m_pITargetDataObject ) == S_OK )
	{
		BOOL fCopy = FALSE;

		// Does m_pITargetDataObject contain format CF_SEGMENT, CF_STYLE, or CF_MOTIF?
		CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
		if( pDataObject )
		{
			if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( m_pITargetDataObject, m_cfSegment ) ) 
			||  SUCCEEDED ( pDataObject->IsClipFormatAvailable( m_pITargetDataObject, m_cfStyle ) ) )
			{
				fCopy = TRUE;
			}
			pDataObject->Release();
		}

		// Can only copy CF_SEGMENT, CF_STYLE, and CF_MOTIF data!
		if( fCopy )
		{
			dwEffect = DROPEFFECT_COPY;
		}
		else
		{
			if( grfKeyState & MK_RBUTTON )
			{
				dwEffect = *pdwEffect;
			}
			else
			{
				if( grfKeyState & MK_CONTROL )
				{
					dwEffect = DROPEFFECT_COPY;
				}
				else
				{
					if( *pdwEffect & DROPEFFECT_COPY
					&&  *pdwEffect & DROPEFFECT_MOVE )
					{
						dwEffect = DROPEFFECT_MOVE;
					}
					else
					{
						dwEffect = *pdwEffect;
					}
				}
			}
		}
	}

	// Set temp drag over fields
	if( grfKeyState & (MK_RBUTTON | MK_LBUTTON) )
	{
		m_dwOverDragButton = grfKeyState & (MK_RBUTTON | MK_LBUTTON);
		m_dwOverDragEffect = dwEffect;
	}

	*pdwEffect = dwEffect;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::Drop

HRESULT CSegmentStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
		if( SUCCEEDED( m_pTrackMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			// Import the DataObject that was dragged
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				// Compute the time to drop at
				long lPasteTime;
				m_pTrackMgr->m_pTimeline->PositionToClocks( pt.x, &lPasteTime );

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
						SelectRegionsFromSelectedItems();

						if( CSegmentStrip::m_fDragDropIntoSameDocRoot == FALSE
						||  m_dwOverDragEffect != DROPEFFECT_MOVE )
						{
							// Either we are dropping into a different file,
							// or we are doing a "copy" and there will be no change to the source strip, so....
							// Notify the containing segment that a paste operation occurred
							m_pTrackMgr->m_nLastEdit = IDS_PASTE;
							m_pTrackMgr->OnDataChanged();
						}

						// Redraw our strip
						m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
						
						// If the property sheet is visible, make it display the track item's property page
						m_pTrackMgr->OnShowProperties();

						// Update the track item's property page, if it exists
						if( m_pTrackMgr->m_pPropPageMgr )
						{
							m_pTrackMgr->m_pPropPageMgr->RefreshData();
						}

						// Sync with DirectMusic
						m_pTrackMgr->SyncWithDirectMusic();
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
// CSegmentStrip::CanPasteFromData

HRESULT CSegmentStrip::CanPasteFromData(IDataObject* pIDataObject)
{
	// Check that we're passed a valid IDataObject pointer
	if( pIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Get a Timeline DataObject that encapsulates the pIDataObject
	IDMUSProdTimelineDataObject *pITimelineDataObject = NULL;
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_pTrackMgr->m_pTimeline, pIDataObject );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check the timeline data object for our clipboard formats
	hr = S_FALSE;
	if( (pITimelineDataObject->IsClipFormatAvailable( m_cfSegmentTrack ) == S_OK)
	||  (pITimelineDataObject->IsClipFormatAvailable( m_cfSegment ) == S_OK) 
	||  (pITimelineDataObject->IsClipFormatAvailable( m_cfStyle ) == S_OK) )
	{
		// If we found our clipboard format, return S_OK
		hr = S_OK;
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return either S_OK or S_FALSE
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::OnLButtonDown

HRESULT CSegmentStrip::OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);

	// Validate our timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		return E_UNEXPECTED;
	}

	// Initialize the item to toggle to NULL
	m_pItemToToggle = NULL;
	
	// Unselect all items in the other strips
	UnselectGutterRange();

	// Capture mouse so we get the WM_LBUTTONUP message as well.
	// The timeline will release the capture when it receives the
	// WM_LBUTTONUP message.
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Flag that the left mouse button is pressed
	m_fLeftMouseDown = true;

	// Show the track item's property page
	m_fShowItemProps = true;

	// Check if the shift key is pressed
	if( wParam & MK_SHIFT )
	{
		m_pSelectedRegions->ShiftAddRegion(lXPos);
		SelectItemsInSelectedRegions();
	}
	else
	{
		// See if there is an item under the cursor.
		CTrackItem* pItem = GetItemFromPoint( lXPos );
		if( pItem )
		{
			// Found an item under the cursor

			// Check if the control key is down
			if( wParam & MK_CONTROL )
			{
				// Check if the item is not yet selected
				if( pItem->m_fSelected )
				{
					// Set up to unselect this item later, either when we receive a
					// left-button up, or when the user completes a drag-drop operation
					// that does nothing.
					m_pItemToToggle = pItem;
				}
				else
				{
					// Mark the clicked on item as selected
					pItem->m_fSelected = TRUE;
					m_pSelectedRegions->AddRegion(lXPos);
				}
			}
			// Check if the item is unselected (the shift key is up)
			else if( !pItem->m_fSelected )
			{
				// Mark the clicked on item as selected
				m_pTrackMgr->UnselectAll();
				pItem->m_fSelected = TRUE;

				// Update the selection regions to include just this selected item
				SelectRegionsFromSelectedItems();
			}
		}
		// Didn't find an item - check if the Ctrl key is down
		else if( wParam & MK_CONTROL )
		{
			// Toggle the selection state in the region under the cursor
			m_pSelectedRegions->ToggleRegion( lXPos );
		}
		// The shift key is up
		else
		{
			// The user left-clicked on blank space without holding the shift or Ctrl keys down

			// Clear all selection regions
			m_pSelectedRegions->Clear();
			m_pTrackMgr->UnselectAll();

			// Select only the beat clicked on
			m_pSelectedRegions->AddRegion( lXPos );
		}

		// Set anchor for future shift-select operations
		m_pSelectedRegions->SetShiftSelectAnchor( lXPos );
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

	// Switch the property page to the track item's property page
	m_pTrackMgr->OnShowProperties();

	// Refresh the track item property page, if it exists
	if( m_pTrackMgr->m_pPropPageMgr )
	{
		m_pTrackMgr->m_pPropPageMgr->RefreshData();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::OnRButtonDown

HRESULT CSegmentStrip::OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);
	ASSERT( m_pTrackMgr->m_pTimeline != NULL );

	// Unselect all items in the other strips
	UnselectGutterRange();

	// Get the item at the mouse click.
	CTrackItem* pItem = GetItemFromPoint( lXPos );

	if( !(wParam & (MK_CONTROL | MK_SHIFT)) )
	{
		// Neither control or shift were pressed

		// Check if an item was clicked on
		if( pItem )
		{
			// Check if the item is selected
			if( pItem->m_fSelected == FALSE )
			{
				// Item is not selected, unselect all items
				m_pTrackMgr->UnselectAll();

				// Select just this item
				pItem->m_fSelected = TRUE;

				// Add just this beat to the list of selected regions
				m_pSelectedRegions->AddRegion(lXPos);
			}
			// If the item was already selected, do nothing
		}
		else
		{
			// if region not selected, select it, otherwise do nothing

			// Convert the pixel position to a measure and beat
			long lMeasure, lBeat;
			if( SUCCEEDED( m_pTrackMgr->m_pTimeline->PositionToMeasureBeat(m_pTrackMgr->m_dwGroupBits,
															0,lXPos, &lMeasure, &lBeat) ) )
			{
				// Check if this beat is not already selected
				if( !m_pSelectedRegions->Contains(lMeasure, lBeat))
				{
					// This beat is not selected - unselect all beats
					m_pTrackMgr->UnselectAll();

					// Now, select just this beat
					m_pSelectedRegions->AddRegion(lXPos);
				}
			}
		}
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	// Switch to the item's property page
	m_fShowItemProps = true;
	m_pTrackMgr->OnShowProperties(); 

	// Refresh the lyric property page, if it exists
	if( m_pTrackMgr->m_pPropPageMgr )
	{
		m_pTrackMgr->m_pPropPageMgr->RefreshData();
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::CanCycle

BOOL CSegmentStrip::CanCycle( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTrackMgr->m_pTimeline != NULL );
	if( m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	long lMeasure;
	long lBeat;

	if( SUCCEEDED ( m_pTrackMgr->m_pTimeline->PositionToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, m_lXPos, &lMeasure, &lBeat ) ) )
	{
		int nCount = 0;
		CTrackItem* pItem;

		POSITION pos = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();
		while( pos )
		{
			pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

			if( pItem->m_lMeasure == lMeasure
			&&  pItem->m_lBeat == lBeat )
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

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::PostRightClickMenu

HRESULT CSegmentStrip::PostRightClickMenu( POINT pt )
{
	m_fInRightClickMenu = TRUE;

	HMENU hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_EDIT_RMENU) );
	HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

	::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, 2, ( CanPaste( NULL ) == S_OK ) ? MF_ENABLED | MF_BYPOSITION :
			MF_GRAYED | MF_BYPOSITION );
	::EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( CanInsert() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( IsEmpty() ) ? MF_GRAYED :
			MF_ENABLED );
	::EnableMenuItem( hMenuPopup, ID_VIEW_PROPERTIES, MF_ENABLED );
	::EnableMenuItem( hMenuPopup, IDM_CYCLE_ITEMS, ( CanCycle() == TRUE ) ? MF_ENABLED :
			MF_GRAYED );

	m_pTrackMgr->m_pTimeline->TrackPopupMenu( hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE );
	::DestroyMenu( hMenu );

	m_fInRightClickMenu = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::UnselectGutterRange

void CSegmentStrip::UnselectGutterRange( void )
{
	// Make sure everything on the timeline is deselected.

	// Flag that we're unselecting all other strips
	m_fSelecting = TRUE;

	// Set the begin and end selection time to 0
	m_pTrackMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pTrackMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );

	// Flags that we're no longer unselecting all other strips
	m_fSelecting = FALSE;
}


// IDropSource helpers

/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::CreateDataObject

HRESULT	CSegmentStrip::CreateDataObject(IDataObject** ppIDataObject, long lPosition)
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
	if( FAILED( m_pTrackMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
	{
		return E_OUTOFMEMORY;
	}


	// Save Selected items into stream
	HRESULT hr = E_FAIL;

	// Allocate a memory stream
	IStream* pIStream;
	if( SUCCEEDED ( m_pTrackMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// Get the item at the drag point
		CTrackItem* pItemAtDragPoint = GetItemFromPoint( lPosition );

		// mark the items as being dragged: this used later for deleting items in drag move
		m_pTrackMgr->MarkSelectedItems(UD_DRAGSELECT);

		// Save the selected items into a stream
		if( SUCCEEDED ( m_pTrackMgr->SaveSelectedItems( pIStream, pItemAtDragPoint ) ) )
		{
			// Add the stream to the Timeline DataObject
			if( SUCCEEDED ( pITimelineDataObject->AddExternalClipFormat( m_cfSegmentTrack, pIStream ) ) )
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
// CSegmentStrip::DoDragDrop

HRESULT CSegmentStrip::DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos)
{
	// Do a drag'n'drop operation

	// Get the item at the point we're dragging from
	CTrackItem* pItem = GetItemFromPoint( lXPos );

	// Check that we found a item, and that it is selected
	if(!pItem || !pItem->m_fSelected)
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
		hr = CreateDataObject( &m_pISourceDataObject, lXPos );

		// Check if we successfully created a data object
		if(SUCCEEDED(hr))
		{
			// We can always copy items.
			DWORD dwOKDragEffects = DROPEFFECT_COPY;

			// Check if we can cut items
			if(CanCut() == S_OK)
			{
				// If we can Cut(), allow the user to move the items as well.
				dwOKDragEffects |= DROPEFFECT_MOVE;
			}

			// Do the Drag/Drop.

			// Initialize our DragDropSource flag to 1 (means we are the source of a drag-drop)
			m_nStripIsDragDropSource = 1;

			// Initialize our DragDropSource DocRoot fields
			CSegmentStrip::m_pIDocRootOfDragDropSource = m_pTrackMgr->m_pIDocRootNode;
			CSegmentStrip::m_fDragDropIntoSameDocRoot = FALSE;

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
					// We moved some items - delete the items we marked earlier
					m_pTrackMgr->DeleteMarked(UD_DRAGSELECT);
				}

				// If the effect wasn't 'None', set the drop flag to true
				if(dwEffect != DROPEFFECT_NONE)
				{
					fDrop = true;
				}
				break;
			default:
				// User cancelled the operation, or dropped the items somewhere they didn't belong
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
						m_pTrackMgr->m_nLastEdit = IDS_UNDO_MOVE;
					}
					else
					{
						// We did a copy - use the 'Insert' undo text
						m_pTrackMgr->m_nLastEdit = IDS_INSERT;
					}

					// Notify the containing segment that we changed
					m_pTrackMgr->OnDataChanged();

					// Update the selection regions to include the selected items
					SelectRegionsFromSelectedItems();

					// If visible, switch the property sheet to the track item property page
					m_pTrackMgr->OnShowProperties();

					// If it exists, refresh the track item property page
					if( m_pTrackMgr->m_pPropPageMgr )
					{
						m_pTrackMgr->m_pPropPageMgr->RefreshData();
					}

					// Sync with DirectMusic
					m_pTrackMgr->SyncWithDirectMusic();
				}
				else
				{
					// Target strip is different from source strip
					if( dwEffect & DROPEFFECT_MOVE )
					{
						// Check if we dropped in the same segment
						if( CSegmentStrip::m_fDragDropIntoSameDocRoot == TRUE )
						{
							// We did a move operation to another strip in the same segment - use the 'Move' undo text
							m_pTrackMgr->m_nLastEdit = IDS_UNDO_MOVE;
						}
						else
						{
							// We did a move operation to another strip in a different - use the 'Delete' undo text
							m_pTrackMgr->m_nLastEdit = IDS_DELETE;
						}

						// Notify the Segment Designer that we changed
						m_pTrackMgr->OnDataChanged();

						// Sync with DirectMusic
						m_pTrackMgr->SyncWithDirectMusic();
					}
					else
					{
						// Unselect all items, since the user did a copy operation
						m_pTrackMgr->UnselectAll();
					}
				}

				// Refresh our strip display
				m_pTrackMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

				// Return that we actually did a drop
				hr = S_OK;
			}
		}

		// Release the IDropSource interface
		pIDropSource->Release();

		// Clear the DragSelect flag from all items
		m_pTrackMgr->UnMarkItems(UD_DRAGSELECT); 
	}
	else
	{
		// Couldn't query for IDropSource interface - return failure code
		hr = E_FAIL;
	}

	// Clear the DragDropSource fields
	m_nStripIsDragDropSource = 0;
	CSegmentStrip::m_pIDocRootOfDragDropSource = NULL;
	CSegmentStrip::m_fDragDropIntoSameDocRoot = FALSE;
	
	// Return a success or failure code
	return hr;
}


////////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::SelectItemsInSelectedRegions

bool CSegmentStrip::SelectItemsInSelectedRegions()
{
	// Flag set to true if anything changes
	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

		// Check if this item is within a selection region
		if(m_pSelectedRegions->Contains(pItem->m_lMeasure, pItem->m_lBeat))
		{
			// Check if the item is not selected
			if( !pItem->m_fSelected )
			{
				// Item isn't yet selected - select it and set fChanged to true
				pItem->m_fSelected = TRUE;
				fChanged = true;
			}
		}
		else
		{
			// Check if the item is selected
			if( pItem->m_fSelected )
			{
				// Item is selected - deselect it and set fChagned to true
				pItem->m_fSelected = FALSE;
				fChanged = true;
			}
		}
	}

	// return whether or not we changed the selection state of any items
	return fChanged;
}


////////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::SelectRegionsFromSelectedItems

void CSegmentStrip::SelectRegionsFromSelectedItems( void )
{
	// Clear the list of selected regions
	m_pSelectedRegions->Clear();

	// Iterate through the list of items
	POSITION pos = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

		// Check if the item is selected
		if(pItem->m_fSelected)
		{
			// Add the beat the item is in to the list of selected regions.
			CListSelectedRegion_AddRegion(*m_pSelectedRegions, *pItem);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::RegisterClipboardFormats

BOOL CSegmentStrip::RegisterClipboardFormats( void )
{
	// CF_SEGMENTTRACK
	if( m_cfSegmentTrack == 0 )
	{
		m_cfSegmentTrack = RegisterClipboardFormat( CF_SEGMENTTRACK );
	}
	
	// CF_SEGMENT
	if( m_cfSegment == 0 )
	{
		m_cfSegment = RegisterClipboardFormat( CF_SEGMENT );
	}
	
	// CF_STYLE
	if( m_cfStyle == 0 )
	{
		m_cfStyle = RegisterClipboardFormat( CF_STYLE );
	}

	if( m_cfSegmentTrack == 0 
	||  m_cfSegment == 0 
	||  m_cfStyle == 0 )
	{ 
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::IsSelected

bool CSegmentStrip::IsSelected()
{
	if( m_pTrackMgr == NULL )
	{
		ASSERT( 0 );
		return false;
	}

	// If anything is selected, return true
	
	// Iterate through the list of items
	POSITION pos = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Check if this item is selected
		if( m_pTrackMgr->m_lstTrackItems.GetNext( pos )->m_fSelected )
		{
			// Item is selected - return true
			return true;
		}
	}

	// No items selected - return false
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::IsEmpty

bool CSegmentStrip::IsEmpty()
{
	if( m_pTrackMgr == NULL )
	{
		ASSERT( 0 );
		return true;
	}

	return m_pTrackMgr->m_lstTrackItems.IsEmpty() ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentStrip::SelectSegment

// Return true if anything changed
bool CSegmentStrip::SelectSegment( MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime )
{
	// Verify that we have a pointer to the Timeline
	if( m_pTrackMgr == NULL 
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return false;
	}

	// Convert the start position from clocks to a measure and beat value
	long lMeasure, lBeat;
	m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0,
												   mtBeginTime, &lMeasure, &lBeat );

	
	// Convert back to a clock value
	m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( m_pTrackMgr->m_dwGroupBits, 0,
												   lMeasure, lBeat, &mtBeginTime );

	// Convert the end position from clocks to a measure and beat value
	m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0,
												   mtEndTime, &lMeasure, &lBeat );

	
	// Increment the beat so the last beat is selected
	lBeat++;

	// Convert back to a clock value
	m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( m_pTrackMgr->m_dwGroupBits, 0,
												   lMeasure, lBeat, &mtEndTime );

	// Convert the passed in times to a generic time class
	CMusicTimeConverter cmtBeg(mtBeginTime);
	CMusicTimeConverter cmtEnd(mtEndTime);

	// Create a region that contains the selected time
	CSelectedRegion region(cmtBeg, cmtEnd);

	// Add the region to the list of selected regions
	m_pSelectedRegions->AddRegion(region);

	// Select all items in the list of selected regions
	// This will return true if the selection state of any item changed
	return SelectItemsInSelectedRegions();
}
