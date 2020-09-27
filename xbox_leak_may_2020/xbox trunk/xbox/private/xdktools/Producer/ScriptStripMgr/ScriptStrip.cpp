// ScriptStrip.cpp : Implementation of CScriptStrip
#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackItem.h"
#include "ScriptStripMgr.h"
#include "TrackMgr.h"
#include "PropPageMgr_Item.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include "BaseMgr.h"
#include "SegmentIO.h"
#include "DLLJazzDataObject.h"
#include "GrayOutRect.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This sets the strip's height
#define DEFAULT_STRIP_HEIGHT_MUSICTIME	20
#define DEFAULT_STRIP_HEIGHT_REFTIME	32
#define DEFAULT_RECT_TOP_REFTIME		15

const DWORD g_dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
const DWORD g_dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;

/////////////////////////////////////////////////////////////////////////////
// CScriptStrip constructor/destructor

CScriptStrip::CScriptStrip( CTrackMgr* pTrackMgr ) : CBaseStrip( pTrackMgr )
{
	// Validate the pointer to our strip manager
	ASSERT( pTrackMgr );
	if ( pTrackMgr == NULL )
	{
		return;
	}

	// Set our pointer to our strip manager
	m_pTrackMgr = pTrackMgr;

	// Initialize our clipboard format to 0
	m_cfScriptTrack = 0;
	m_cfScript = 0;

	// Initialize our state variables to false
	m_fLeftMouseDown = false;

	// Initialize the Script to toggle (when CTRL-clicking) to NULL
	m_pTrackItemToToggle = NULL;
}

CScriptStrip::~CScriptStrip()
{
	// Clear our pointer to our strip manager
	ASSERT( m_pTrackMgr );
	if ( m_pTrackMgr )
	{
		m_pTrackMgr = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::GetTopItem

CTrackItem* CScriptStrip::GetTopItem( long lMeasure, long lBeat )
{
	CTrackItem* pTheItem = NULL;
	CTrackItem* pFirstItem = NULL;
	CTrackItem* pItem;

	// Measure/beat will equal minute/second in REFERENCE_TIME tracks
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
// CScriptStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// GetNextSelectedItem - helper method for Draw()

void GetNextSelectedItem( CTypedPtrList<CPtrList, CTrackItem*>& list, POSITION &posScript )
{
	// Note that if the item at posScript is selected, this method doesn't do anything useful.
	if( posScript )
	{
		// Save the current position
		POSITION posToSave = posScript;

		// Check if we have not run off the end of the list, and if the currently item is unselected
		while( posScript && !list.GetNext( posScript )->m_fSelected )
		{
			// Current item is unselected, save the position of the next item
			posToSave = posScript;
		}

		// Check if we found a selected item
		if( posToSave )
		{
			// Save the position of the selected item
			posScript = posToSave;

		}
		// Otherwise both posToSave and posScript are NULL
	}
}

/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::AdjustTopItem - helper method for Draw()

void CScriptStrip::AdjustTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CTrackItem* pFirstSelectedItem = NULL;
	CTrackItem* pTopItem = NULL;
	CTrackItem* pItem;

	// Measure/beat will equal minute/second in REFERENCE_TIME tracks
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
// CScriptStrip::GetNextSelectedTopItem - helper method for Draw()

CTrackItem* CScriptStrip::GetNextSelectedTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CTrackItem* pNextItem = NULL;
	CTrackItem* pItem;

	// Measure/beat will equal minute/second in REFERENCE_TIME tracks
	while( pos )
	{
		pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

		if( pItem->m_fSelected )
		{
			if( pItem->m_lMeasure != lCurrentMeasure
			||  pItem->m_lBeat != lCurrentBeat )
			{
				pNextItem = GetTopItem( pItem->m_lMeasure, pItem->m_lBeat );
				ASSERT( pNextItem != NULL );	// Should not happen!
				break;
			}
		}
	}

	return pNextItem;
}

/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::GetNextTopItem - helper method for Draw()

CTrackItem* CScriptStrip::GetNextTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CTrackItem* pNextItem = NULL;
	CTrackItem* pItem;

	// Measure/beat will equal minute/second in REFERENCE_TIME tracks
	while( pos )
	{
		pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

		if( pItem->m_lMeasure != lCurrentMeasure
		||  pItem->m_lBeat != lCurrentBeat )
		{
			pNextItem = GetTopItem( pItem->m_lMeasure, pItem->m_lBeat );
			ASSERT( pNextItem != NULL );	// Should not happen!
			break;
		}
	}

	return pNextItem;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::Draw

HRESULT	STDMETHODCALLTYPE CScriptStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that our timeline pointer is valid
	if( m_pTrackMgr == NULL
	||	m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Draw bar, beat, grid lines
	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		DrawClockVerticalLines( hDC, lXOffset );
	}
	else
	{
		m_pTrackMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pTrackMgr->m_dwGroupBits, 0, lXOffset );
	}

	// Set a flag if we should use the gutter range for our selection
	bool fUseGutterSelectRange = m_bGutterSelected && (m_lGutterBeginSelect != m_lGutterEndSelect);

	// Get our clipping rectange
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// Initialize the top and bottom of the highlight rectangle
	// (it is always the entire strip height).
	RECT rectHighlight;
	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		rectHighlight.top = DEFAULT_RECT_TOP_REFTIME;
		rectHighlight.bottom = DEFAULT_STRIP_HEIGHT_REFTIME;
	}
	else
	{
		rectHighlight.top = 0;
		rectHighlight.bottom = DEFAULT_STRIP_HEIGHT_MUSICTIME;
	}

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

	// Fields to keep track of top item on current measure/beat (or current minute/second)
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

			// Get the "top" item on this beat (or second)
			pTopItem = GetTopItem( lCurrentMeasure, lCurrentBeat );
			if( pTopItem == NULL )
			{
				ASSERT( 0 );	// Should not happen!
				continue;
			}

			// Convert the measure/beat (or minute/sec) of each item to a pixel position
			m_pTrackMgr->SnapUnknownTimeToDisplayPosition( pTopItem->m_rtTimePhysical, &lPosition );

			// Offset the position by the horizontal offset,
			// and increment it by one pixel so we don't overwrite the bar or beat line
			lPosition += -lXOffset + 1;

			// Check if the left edge of the Routine text starts after the region we're displaying
			if( lPosition > rectClip.left )
			{
				// This item is after start of the display - break out of the loop
				break;
			}

			// Determine the text to display in the track
			pTopItem->FormatUIText( strText );

			// Get the size of the Routine text to be written
			::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );

			// Check if the right edge of the Routine text extends into the region we're displaying
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

				// Determine first visible measure/beat (or minute/second)
				long lClocks;
				REFERENCE_TIME rtTime;
				m_pTrackMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks );
				if( m_pTrackMgr->IsRefTimeTrack() )
				{
					m_pTrackMgr->m_pTimeline->ClocksToRefTime( lClocks, &rtTime );
				}
				else
				{
					rtTime = lClocks;
				}
				m_pTrackMgr->SnapUnknownTimeToDisplayPosition( rtTime, &lPosition );
			}
			else
			{
				// Cleanup
				AdjustTopItem( posLast, lCurrentMeasure, lCurrentBeat );

				// Get the "top" item on this beat (or second)
				pTopItem = GetTopItem( lCurrentMeasure, lCurrentBeat );
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

				// Convert the measure/beat (or minute/sec) of each item to a pixel position
				m_pTrackMgr->SnapUnknownTimeToDisplayPosition( pTopItem->m_rtTimePhysical, &lPosition );
			}

			// Offset the position by the horizontal offset,
			// and increment it by one pixel so we don't overwrite the bar or beat line
			lPosition += -lXOffset + 1;

			// Check if the left edge of the Routine text is beyond the region we're displaying
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

			// Use italics if more than one routine is on this beat (or second)
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

			// Get a pointer to the next "top" item occuring AFTER this beat (or second)
			CTrackItem* pNextItem = GetNextTopItem( posItem, lCurrentMeasure, lCurrentBeat );

			// Check if there is a item after this one
			if( pNextItem ) 
			{
				// Get the size of the Routine text to be written
				::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );

				// Get the start position of the next item
				m_pTrackMgr->SnapUnknownTimeToDisplayPosition( pNextItem->m_rtTimePhysical, &(rectHighlight.right) );

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
					::TextOut( hDC, lPosition, rectHighlight.top, strText, strText.GetLength() );
				}
			}
			else
			{
				// No more items after this one - just draw it
				::TextOut( hDC, lPosition, rectHighlight.top, strText, strText.GetLength() );
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

			// Get the "top" item on this beat (or second)
			pTopItem = GetTopItem( lCurrentMeasure, lCurrentBeat );
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

			// Convert the measure/beat (or minute/sec) of each item to a pixel position
			m_pTrackMgr->SnapUnknownTimeToDisplayPosition( pTopItem->m_rtTimePhysical, &lPosition );

			// Offset the position by the horizontal offset,
			// and increment it by one pixel so we don't overwrite the bar or beat line
			lPosition += -lXOffset + 1;

			// Check if the left edge of the Routine text is beyond the region we're displaying
			if( lPosition > rectClip.right )
			{
				// We've gone beyond the right edge of the clipping region - break out of the loop
				break;
			}

			// Use italics if more than one routine is on this beat (or second)
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

			// Get a pointer to the next selected "top" item occuring AFTER this beat (or second)
			CTrackItem* pNextItem = GetNextSelectedTopItem( posItem, lCurrentMeasure, lCurrentBeat );

			// Check if there is a item after this one
			if( pNextItem ) 
			{
				// Get the size of the text to be written
				::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );

				// Get the start position of the next item
				m_pTrackMgr->SnapUnknownTimeToDisplayPosition( pNextItem->m_rtTimePhysical, &(rectHighlight.right) );

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
					::TextOut( hDC, lPosition, rectHighlight.top, strText, strText.GetLength() );
				}
			}
			// No more selected items
			else
			{
				// Just draw the Routine
				::TextOut( hDC, lPosition, rectHighlight.top, strText, strText.GetLength() );
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

		// Get the start position of the Routine list
		posItem = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();

		// Get the start position of the region list
		POSITION posRegion = m_pSelectedRegions->GetHeadPosition();
		while( posRegion )
		{
			CTrackItem* pItem = NULL;

			// Get a pointer to the current Routine, but only when not using the gutter selection
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
				if( m_pTrackMgr->IsRefTimeTrack() )
				{
					REFERENCE_TIME rtTime;
					m_pTrackMgr->MinSecMsToRefTime( pItem->m_lMeasure, pItem->m_lBeat, 0, &rtTime ); 
					m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtTime, &lItemClockPos );
				}
				else
				{
					m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( m_pTrackMgr->m_dwGroupBits, 0,
						pItem->m_lMeasure, pItem->m_lBeat, &lItemClockPos );
				}

				while( posItem && (lItemClockPos < psr->End()) )
				{
					// Check if the current item is within this selected region
					if( lItemClockPos >= psr->Beg() )
					{
						// Compute the left side of the Routine's text display
						long lItemPos;
						m_pTrackMgr->m_pTimeline->ClocksToPosition( lItemClockPos, &lItemPos );

						// Get the size of the text to be written
						pItem->FormatUIText( strText );
						::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );

						// Add the size of the text and subtract the offset to
						// determine the right side of the Routine's text display.
						lItemPos += sizeText.cx - lXOffset;

						// Make sure we have the top selected item!
						pItem = GetNextSelectedTopItem( posItem, pItem->m_lMeasure, pItem->m_lBeat );

						// Check if we found a selected item
						if( pItem )
						{
							posItem = m_pTrackMgr->m_lstTrackItems.Find( pItem, posItem );

							// Get the clock position of this Routine
							if( m_pTrackMgr->IsRefTimeTrack() )
							{
								REFERENCE_TIME rtTime;
								m_pTrackMgr->MinSecMsToRefTime( pItem->m_lMeasure, pItem->m_lBeat, 0, &rtTime ); 
								m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtTime, &lItemClockPos );
							}
							else
							{
								m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( m_pTrackMgr->m_dwGroupBits, 0,
									pItem->m_lMeasure, pItem->m_lBeat, &lItemClockPos );
							}

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

							// Get the clock position of this Routine
							if( m_pTrackMgr->IsRefTimeTrack() )
							{
								REFERENCE_TIME rtTime;
								m_pTrackMgr->MinSecMsToRefTime( pItem->m_lMeasure, pItem->m_lBeat, 0, &rtTime ); 
								m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtTime, &lItemClockPos );
							}
							else
							{
								m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( m_pTrackMgr->m_dwGroupBits, 0,
									pItem->m_lMeasure, pItem->m_lBeat, &lItemClockPos );
							}
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


#define SECOND_LINE_PENSTYLE	PS_SOLID
#define SECOND_LINE_WIDTH		2
#define SECOND_LINE_COLOR		RGB(0,0,0)

/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::DrawClockVerticalLines

void CScriptStrip::DrawClockVerticalLines( HDC hDC, long lXOffset ) // const RECT &rectClip )
{
	// Create the "second" pen
	HPEN hpenSecond = ::CreatePen( SECOND_LINE_PENSTYLE, SECOND_LINE_WIDTH, SECOND_LINE_COLOR );
	if( hpenSecond == NULL )
	{
		return;
	}

	// Save the current pen and switch to the "second" pen
	HPEN hpenOld = static_cast<HPEN>( ::SelectObject( hDC, hpenSecond ) );

	// Set BkMode
	int nOldBkMode = ::SetBkMode( hDC, TRANSPARENT );

	// Get clipping rectangle
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// Determine end position
	VARIANT var;
	long lEndPosition;
	m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	m_pTrackMgr->m_pTimeline->ClocksToPosition( V_I4(&var), &lEndPosition );

	// Determine start time
	REFERENCE_TIME refTime, refSecond;
	m_pTrackMgr->m_pTimeline->PositionToRefTime( rectClip.left + lXOffset, &refTime );
	refSecond = refTime / REFCLOCKS_PER_SECOND;

	char achText[20];
	long lCurPosition;
	long lSecondPosition;

	do
	{
		refTime = refSecond * REFCLOCKS_PER_SECOND;
		m_pTrackMgr->m_pTimeline->RefTimeToPosition( refTime, &lCurPosition );

		if( lCurPosition < lEndPosition )
		{
			// Save position for "second" text
			lSecondPosition = lCurPosition - lXOffset;

			// Draw "second" line
			::SelectObject( hDC, hpenSecond );
			::MoveToEx( hDC, lSecondPosition, 0, NULL );
			::LineTo( hDC, lSecondPosition, DEFAULT_STRIP_HEIGHT_REFTIME );

			// Draw "second" text
			if( refSecond < 60 )
			{
				_snprintf( achText, 10, "%02d", (int)refSecond );
			}
			else
			{
				_snprintf( achText, 10, "%d:%02d", (int)refSecond/60, (int)refSecond%60 );
			}
			::TextOut( hDC, lSecondPosition, 0, achText, strlen(achText) );
		}

		refSecond++;

	} while( lCurPosition < rectClip.right + lXOffset );

	::SetBkMode( hDC, nOldBkMode);
	::SelectObject( hDC, hpenOld );
	::DeleteObject( hpenSecond );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::GetItemFromPoint

CTrackItem *CScriptStrip::GetItemFromPoint( long lPos )
{
	CTrackItem* pItemReturn = NULL;

	long lMeasure, lBeat;
	if( SUCCEEDED ( m_pTrackMgr->SnapPositionToUnknownIncrements( lPos, &lMeasure, &lBeat ) ) )
	{
		pItemReturn = GetTopItem( lMeasure, lBeat );
	}

	return pItemReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::GetStripProperty

HRESULT CScriptStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( NULL == pvar )
	{
		return E_POINTER;
	}

	switch( sp )
	{
		case SP_DEFAULTHEIGHT:
		case SP_MAXHEIGHT:
		case SP_MINHEIGHT:
			pvar->vt = VT_INT;
			if( m_pTrackMgr->IsRefTimeTrack() )
			{
				V_INT(pvar) = DEFAULT_STRIP_HEIGHT_REFTIME;
			}
			else
			{
				V_INT(pvar) = DEFAULT_STRIP_HEIGHT_MUSICTIME;
			}
			break;

		default:
			return CBaseStrip::GetStripProperty( sp, pvar );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CScriptStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
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

	case WM_LBUTTONDBLCLK:
		hr = m_pTrackMgr->CallSelectedRoutines();
		break;

	case WM_RBUTTONDOWN:
		// Call a function to handle the right mouse button press
		hr = OnRButtonDown( wParam, lParam, lXPos, lYPos );
		break;

	case WM_MOUSEMOVE:
		// Check if the left mouse button is down
		if(m_fLeftMouseDown)
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
					if( m_pTrackItemToToggle )
					{
						m_pTrackItemToToggle->m_fSelected = !m_pTrackItemToToggle->m_fSelected;
						m_pTrackItemToToggle = NULL;

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

				// Switch the property page to the Routine property page
				m_pTrackMgr->OnShowProperties();
			}
			else
			{
				// successful drag drop--make sure that only the regions with
				// selected items are selected
				SelectRegionsFromSelectedItems();
			}

			// Refresh the Routine property page, if it exists
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
			if( m_pTrackItemToToggle )
			{
				m_pTrackItemToToggle->m_fSelected = !m_pTrackItemToToggle->m_fSelected;
				m_pTrackItemToToggle = NULL;

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

		// Refresh the Routine property page, if it exists
		if( m_pTrackMgr->m_pPropPageMgr )
		{
			m_pTrackMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	case WM_KEYDOWN:
		hr = OnKeyDown( wParam, lParam );
		break;

	case WM_COMMAND:
	{
		// We should only get this message in response to a selection in the right-click context menu.
		WORD wNotifyCode = HIWORD( wParam );	// notification code 
		WORD wID		 = LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
			case IDM_CALL_ROUTINE:
				hr = m_pTrackMgr->CallSelectedRoutines();
				break;
			case IDM_CYCLE_ROUTINES:
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
		// Register our clipboard formats, if they haven't been registered yet
		if( m_cfScriptTrack == 0 )
		{
			m_cfScriptTrack = RegisterClipboardFormat( CF_SCRIPTTRACK );
		}
		if( m_cfScript == 0 )
		{
			m_cfScript = RegisterClipboardFormat( CF_SCRIPT );
		}

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
// CScriptStrip::OnKeyDown

HRESULT CScriptStrip::OnKeyDown( WPARAM wParam, LPARAM lParam )
{
	if( lParam & 0x40000000 )
	{
		// Key held down and is repeating
		return S_OK;
	}

	HRESULT hr = S_OK;

	switch( wParam )
	{
		case 0x52:  // VK_R		(Call Routine)
			if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
			{
				hr = m_pTrackMgr->CallSelectedRoutines();
			}
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::Copy

HRESULT CScriptStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if there is anything to copy
	HRESULT hr = CanCopy();
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

	// If the clipboard formats haven't been registered yet, do it now.
	if( m_cfScriptTrack == 0 )
	{
		m_cfScriptTrack = RegisterClipboardFormat( CF_SCRIPTTRACK );
		if( m_cfScriptTrack == 0 )
		{
			ASSERT(FALSE);
			return E_FAIL;
		}
	}
	if( m_cfScript == 0 )
	{
		m_cfScript = RegisterClipboardFormat( CF_SCRIPT );
		if( m_cfScript == 0 )
		{
			ASSERT(FALSE);
			return E_FAIL;
		}
	}

	long lStartTime;
	long lEndTime;
	m_pSelectedRegions->GetSpan( lStartTime, lEndTime );

	// Compute the start offset
	REFERENCE_TIME rtStartOffset;
	long lMeasure;
	long lBeat;
	m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, lStartTime, &lMeasure, &lBeat );
	m_pTrackMgr->MeasureBeatTickToUnknownTime( lMeasure, lBeat, 0, &rtStartOffset);
	CTrackItem* pFirstSelectedItem = m_pTrackMgr->FirstSelectedItem();
	if( pFirstSelectedItem )
	{
		rtStartOffset = pFirstSelectedItem->m_rtTimePhysical - rtStartOffset;
	}

	// Create an IStream to save the selected items in.
	IStream* pStreamCopy;
	hr = CreateStreamOnHGlobal( NULL, TRUE, &pStreamCopy );
	if( FAILED( hr ))
	{
		return E_OUTOFMEMORY;
	}

	// Save the selected Scripts into the stream
	hr = m_pTrackMgr->SaveSelectedItems( pStreamCopy, rtStartOffset, NULL );
	if( FAILED( hr ) )
	{
		// Save failed
		pStreamCopy->Release();
		return hr;
	}

	// The save succeeded - copy the stream to the clipboard
	hr = CopyDataToClipboard( pITimelineDataObject, pStreamCopy, m_cfScriptTrack, m_pTrackMgr, this );

	// Release our pointer to the stream
	pStreamCopy->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::CalculateDropTime

REFERENCE_TIME CScriptStrip::CalculateDropTime( MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI )
{
	REFERENCE_TIME rtDropTime;

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtTime, &rtTime );

		long lMinute, lSec, lMs;
		m_pTrackMgr->RefTimeToMinSecMs( rtTime, &lMinute, &lSec, &lMs );

		m_pTrackMgr->MinSecMsToRefTime( lMinute,
										lSec,
										pCopyPasteUI->m_lRefTimeMs,
										&rtTime );

		m_pTrackMgr->RefTimeToUnknownTime( rtTime, &rtDropTime );
	}
	else
	{
		long lMeasure, lBeat;
		m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );

		m_pTrackMgr->MeasureBeatTickToClocks( lMeasure,
											  lBeat,
											  pCopyPasteUI->m_lMusicTimeTick,
											  &mtTime );

		m_pTrackMgr->ClocksToUnknownTime( mtTime, &rtDropTime );
	}

	return rtDropTime;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::CalculatePasteTime

REFERENCE_TIME CScriptStrip::CalculatePasteTime( MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI )
{
	REFERENCE_TIME rtDropTime;

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtTime, &rtTime );

		long lMinute, lSec, lMs;
		m_pTrackMgr->RefTimeToMinSecMs( rtTime, &lMinute, &lSec, &lMs );

		m_pTrackMgr->MinSecMsToRefTime( lMinute,
										lSec,
										pCopyPasteUI->m_lRefTimeMs,
										&rtTime );

		m_pTrackMgr->RefTimeToUnknownTime( rtTime, &rtDropTime );
	}
	else
	{
		long lMeasure, lBeat;
		m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );

		m_pTrackMgr->MeasureBeatTickToClocks( lMeasure,
											  lBeat,
											  pCopyPasteUI->m_lMusicTimeTick,
											  &mtTime );

		m_pTrackMgr->ClocksToUnknownTime( mtTime, &rtDropTime );
	}

	return rtDropTime;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::PasteAt

HRESULT CScriptStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lPasteTime, bool bDropNotPaste, BOOL &fChanged )
{
	// Validate the data object pointer
	if( pITimelineDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Initialize the return code to a failure code
	HRESULT hr = E_FAIL;

	// Calc rtSnapPasteTime 
	REFERENCE_TIME rtSnapPasteTime;
	if( FAILED( m_pTrackMgr->ClocksToUnknownTime( lPasteTime, &rtSnapPasteTime ) ) 
	||  FAILED( m_pTrackMgr->SnapUnknownTime( rtSnapPasteTime, &rtSnapPasteTime ) ) )
	{
		return E_UNEXPECTED;
	}

	// If we're in a drag-drop operation, check if we're dragging from and dropping to this strip
	if( bDropNotPaste
	&& (m_pITargetDataObject == m_pISourceDataObject) )
	{
		// Ensure we have a valid timeline pointer
		if( m_pTrackMgr->m_pTimeline )
		{
			// Compute the measure/beat (or minute/second) the drag started from
			REFERENCE_TIME rtSnapTime;
			if( SUCCEEDED ( m_pTrackMgr->SnapPositionToUnknownTime( m_lStartDragPosition, &rtSnapTime ) ) )
			{
				// Check if we dropped in the same measure/beat (or minute/second) we started the drag from.
				if( rtSnapPasteTime == rtSnapTime )
				{
					// Didn't move - exit early
					goto Leave;
				}
			}
		}
	}

	// Handle CF_SCRIPTTRACK
	if( pITimelineDataObject->IsClipFormatAvailable( m_cfScriptTrack ) == S_OK )
	{
		// Try and read the stream that contains the Scripts
		IStream* pIStream;
		if( SUCCEEDED ( pITimelineDataObject->AttemptRead( m_cfScriptTrack, &pIStream) ) )
		{
			// Read DMUS_FOURCC_COPYPASTE_UI_CHUNK
			IDMUSProdRIFFStream* pIRiffStream;
			MMCKINFO ck;
			DWORD dwByteCount;
			DWORD dwSize;

			ioCopyPasteUI iCopyPasteUI;
			ZeroMemory( &iCopyPasteUI, sizeof(ioCopyPasteUI) );

			hr = AllocRIFFStream( pIStream, &pIRiffStream );
			if( SUCCEEDED( hr ) )
			{
				DWORD dwPos = StreamTell( pIStream );

				ck.ckid = DMUS_FOURCC_COPYPASTE_UI_CHUNK;
				if( pIRiffStream->Descend( &ck, NULL, MMIO_FINDCHUNK ) == 0 )
				{
					dwSize = min( ck.cksize, sizeof(ioCopyPasteUI) );
					if( FAILED ( pIStream->Read( &iCopyPasteUI, dwSize, &dwByteCount ) )
					||  dwByteCount != dwSize )
					{
						hr = E_FAIL;
					}
				}
				RELEASE( pIRiffStream );

				StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
			}

			if( SUCCEEDED ( hr ) )
			{
				// Load the stream into a list of items
				CTypedPtrList<CPtrList, CTrackItem*> list;
				hr = LoadList( list, m_pTrackMgr->m_pDMProdFramework, m_pTrackMgr, pIStream );

				// Fix reference times
				if( iCopyPasteUI.m_fRefTimeTrack )
				{
					POSITION pos = list.GetHeadPosition();
					while( pos )
					{
						CTrackItem* pItem = list.GetNext( pos );

						pItem->m_rtTimePhysical *= REFCLOCKS_PER_MILLISECOND;
					}
				}

				// Do we need to convert times?
				if( iCopyPasteUI.m_fRefTimeTrack != m_pTrackMgr->IsRefTimeTrack() )
				{
					REFERENCE_TIME rtOrigTimeLogical;
					POSITION pos = list.GetHeadPosition();
					while( pos )
					{
						CTrackItem* pItem = list.GetNext( pos );

						rtOrigTimeLogical = pItem->m_rtTimeLogical;
						pItem->SwitchTimeBase();
						pItem->m_rtTimeLogical = rtOrigTimeLogical;
					}
				}

				// Check if the load operation succeeded
				if ( SUCCEEDED(hr) )
				{
					if( m_pTrackMgr->m_pTimeline )
					{
						// Calc rtPasteTime 
						REFERENCE_TIME rtPasteTime;
						if( bDropNotPaste )
						{
							rtPasteTime = CalculateDropTime( lPasteTime, &iCopyPasteUI );
						}
						else
						{
							rtPasteTime = CalculatePasteTime( lPasteTime, &iCopyPasteUI );
						}

						// The length of the segment, in beats, in music time
						MUSIC_TIME mtSegmentLength = LONG_MAX;
						REFERENCE_TIME rtSegmentLength;
						m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtSegmentLength, &rtSegmentLength ); 

						// Check if there are any items to paste
						if( !list.IsEmpty() )
						{
							// Unselect all existing Scripts in this strip so the only selected Scripts are the dropped
							// or pasted ones
							m_pTrackMgr->UnselectAll();

							// Make sure the last item lands in the last beat or sooner
							// Retrieve the clock length of the segment
							VARIANT varLength;
							if( SUCCEEDED( m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
							{
								mtSegmentLength = V_I4(&varLength);
								m_pTrackMgr->ClocksToUnknownTime( mtSegmentLength, &rtSegmentLength );

								// Make sure the last item lands in the last beat or sooner
								REFERENCE_TIME rtTimeLastItem = list.GetTail()->m_rtTimePhysical + rtPasteTime;
								if( rtTimeLastItem >= rtSegmentLength )
								{
									rtPasteTime -= (rtTimeLastItem - rtSegmentLength);
									m_pTrackMgr->SnapUnknownTime( rtPasteTime, &rtPasteTime );
								}
							}

							// Make sure the first item lands in the first beat or later
							REFERENCE_TIME rtTimeFirstItem = list.GetHead()->m_rtTimePhysical + rtPasteTime;
							if( rtTimeFirstItem < 0 )
							{
								rtPasteTime += -rtTimeFirstItem;
								m_pTrackMgr->SnapUnknownTime( rtPasteTime, &rtPasteTime );
							}
						}

						// Check if we're doing a paste
						if( bDropNotPaste == FALSE )
						{
							// We're doing a paste, so get the paste type
							TIMELINE_PASTE_TYPE tlPasteType;
							if( FAILED( m_pTrackMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
							{
								// Failed to get the paste type, so return with error code
								EmptyList( list );
								hr = E_FAIL;
								goto Leave_1;
							}

							// Check if we're doing a paste->overwrite
							if( tlPasteType == TL_PASTE_OVERWRITE )
							{
								long lMeasureStart, lBeatStart;
								m_pTrackMgr->SnapUnknownTimeToUnknownIncrements( rtPasteTime,
																				 &lMeasureStart, &lBeatStart );
								REFERENCE_TIME rtEnd;
								if( m_pTrackMgr->IsRefTimeTrack() )
								{
									rtEnd = rtPasteTime + iCopyPasteUI.m_rtPasteOverwriteRange;
								}
								else
								{
									rtEnd = rtPasteTime + iCopyPasteUI.m_mtPasteOverwriteRange;
								}

								long lMeasureEnd, lBeatEnd;
								m_pTrackMgr->SnapUnknownTimeToUnknownIncrements( rtEnd,
																				 &lMeasureEnd, &lBeatEnd );

								// Now, delete any items that fall between
								// the first and last beats (or seconds) we're pasting in
								fChanged = m_pTrackMgr->DeleteBetweenMeasureBeats( lMeasureStart, lBeatStart,
																				   lMeasureEnd, lBeatEnd );
							}
						}

						// Iterate throught the list of Scripts we loaded
						while( !list.IsEmpty() )
						{
							// Remove the head of the list
							CTrackItem* pItem = list.RemoveHead();

							// Check if the item will land before the end of the segment
							pItem->m_rtTimePhysical += rtPasteTime;
							if( pItem->m_rtTimePhysical < rtSegmentLength )
							{
								// Recalc m_mtTime fields
								pItem->SetTimePhysical( pItem->m_rtTimePhysical, STP_LOGICAL_FROM_BEAT_OFFSET );

								// When pasted or dropped, each item is selected
								pItem->m_fSelected = TRUE;

								// This will overwrite any item that already exists 
								// with the same time of pItem
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
			}
Leave_1:
			pIStream->Release();
		}
	}

	// Handle CF_SCRIPT
	else if( pITimelineDataObject->IsClipFormatAvailable( m_cfScript ) == S_OK )
	{
		IDMUSProdNode* pIDocRootNode;
		IDataObject* pIDataObject;

		// Unselect all existing Scripts in this strip so the only selected Scripts are the dropped
		// or pasted ones
		m_pTrackMgr->UnselectAll();

		hr = pITimelineDataObject->Export( &pIDataObject );
		if( SUCCEEDED ( hr ) )
		{
			hr = m_pTrackMgr->m_pDMProdFramework->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode );
			if( SUCCEEDED ( hr ) )
			{
				CTrackItem* pItem = new CTrackItem( m_pTrackMgr );
				if( pItem == NULL )
				{
					hr = E_OUTOFMEMORY;
				}
				else
				{
					hr = pItem->SetFileReference( pIDocRootNode );
					if( SUCCEEDED ( hr ) )
					{
						rtSnapPasteTime = max( rtSnapPasteTime, 0 );
						hr = pItem->SetTimePhysical( rtSnapPasteTime, STP_LOGICAL_SET_DEFAULT );
						if( SUCCEEDED ( hr ) )
						{

							// When pasted or dropped, each item is selected
							pItem->m_fSelected = TRUE;

							// This will overwrite any item that already exists on the measure and
							// beat where pItem will be inserted
							m_pTrackMgr->InsertByAscendingTime( pItem, TRUE );

							// We changed
							fChanged = TRUE;
						}
					}
					if( FAILED ( hr ) )
					{
						delete pItem;
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
		// Drag/drop Target and Source are the same Script strip
		m_nStripIsDragDropSource = 2;
	}

	if( CScriptStrip::m_pIDocRootOfDragDropSource
	&&  CScriptStrip::m_pIDocRootOfDragDropSource == m_pTrackMgr->m_pIDocRootNode )
	{
		// Drag/drop Target and Source are the same file (i.e. Segment)
		CScriptStrip::m_fDragDropIntoSameDocRoot = TRUE;
	}
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::Paste

HRESULT CScriptStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
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
	hr = PasteAt( pITimelineDataObject, lClocks, false, fChanged );

	// Check if the paste succeeded AND if something was actually pasted
	if( SUCCEEDED(hr) && fChanged )
	{
		// Update the selection regions to only include the selected items
		SelectRegionsFromSelectedItems();

		// Notify the Segment Designer that we did a paste operation
		m_pTrackMgr->m_nLastEdit = IDS_PASTE;
		m_pTrackMgr->OnDataChanged();

		// Redraw our strip
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

		// Ensure the Script property page is visible
		m_pTrackMgr->OnShowProperties();

		// Refresh the Script property page
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
// CScriptStrip::Insert

HRESULT CScriptStrip::Insert( void )
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
	REFERENCE_TIME rtSnapTime;
	if( FAILED ( m_pTrackMgr->SnapPositionToUnknownTime( max( 0, m_lXPos ), &rtSnapTime ) ) )
	{
		return E_UNEXPECTED;
	}

	CTrackItem* pItem = new CTrackItem( m_pTrackMgr );
	if( pItem == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Set the time the item was inserted in
	pItem->SetTimePhysical( rtSnapTime, STP_LOGICAL_SET_DEFAULT );

	// By default, a newly selected item is selected
	pItem->m_fSelected = TRUE;

	// Insert the item into our list of Scripts, overwriting any existing one
	m_pTrackMgr->InsertByAscendingTime( pItem, FALSE );

	// If it's not already selected, add the beat the item was inserted on
	// to the list of selected regions
	if( !CListSelectedRegion_Contains(pItem->m_rtTimePhysical) )
	{
		CListSelectedRegion_AddRegion( pItem );
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

	// Show the property sheet
	ShowPropertySheet();

	// Switch to the Script property page
	m_pTrackMgr->OnShowProperties();

	// Refresh the Script property page
	if( m_pTrackMgr->m_pPropPageMgr )
	{
		m_pTrackMgr->m_pPropPageMgr->RefreshData();
	}

	// Notify the Segment Designer that an insert occurred
	m_pTrackMgr->m_nLastEdit = IDS_INSERT;
	m_pTrackMgr->OnDataChanged();

	// No need to sync with DirectMusic - the routine starts out as empty

	// Return the success/failure code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::Delete

HRESULT CScriptStrip::Delete( void )
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

	// Refresh the Script property page, if it exists
	if( m_pTrackMgr->m_pPropPageMgr )
	{
		m_pTrackMgr->m_pPropPageMgr->RefreshData();
	}

	// Notify the Segment Designer that a delete operation occurred
	m_pTrackMgr->m_nLastEdit = IDS_DELETE;
	m_pTrackMgr->OnDataChanged();

	// Sync with DirectMusic
	m_pTrackMgr->SyncWithDirectMusic();

	// Return that the delete succeeded
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::CanPaste

HRESULT CScriptStrip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// If the clipboard formats haven't been registered yet, do it now.
	if( m_cfScriptTrack == 0 )
	{
		m_cfScriptTrack = RegisterClipboardFormat( CF_SCRIPTTRACK );
		if( m_cfScriptTrack == 0 )
		{
			return E_FAIL;
		}
	}
	if( m_cfScript == 0 )
	{
		m_cfScript = RegisterClipboardFormat( CF_SCRIPT );
		if( m_cfScript == 0 )
		{
			return E_FAIL;
		}
	}

	// Get a TimelineDataObject, if we don't already have one
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_pTrackMgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check the timeline data object for our clipboard format
	if( (pITimelineDataObject->IsClipFormatAvailable( m_cfScriptTrack ) == S_OK)
	||  (pITimelineDataObject->IsClipFormatAvailable( m_cfScript ) == S_OK) )
	{
		// If we found our clipboard format, return S_OK
		hr = S_OK;
	}
	else
	{
		// Otherwise return S_FALSE;
		hr = S_FALSE;
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::CanInsert

HRESULT CScriptStrip::CanInsert( void )
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
// CScriptStrip::DragOver

HRESULT CScriptStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
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
		BOOL fCF_SCRIPT = FALSE;

		// Does m_pITargetDataObject contain format CF_STYLE?
		CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
		if( pDataObject )
		{
			if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( m_pITargetDataObject, m_cfScript ) ) )
			{
				fCF_SCRIPT = TRUE;
			}
			pDataObject->Release();
		}

		// Can only copy fCF_SCRIPT data!
		if( fCF_SCRIPT )
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
// CScriptStrip::Drop

HRESULT CScriptStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
				m_lXPos = pt.x;

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

						if( CScriptStrip::m_fDragDropIntoSameDocRoot == FALSE
						||  m_dwOverDragEffect != DROPEFFECT_MOVE )
						{
							// Either we are dropping into a different file,
							// or we are doing a "copy" and there will be no change to the source strip, so....
							// Notify the SegmentDesigner that a paste operation occurred
							m_pTrackMgr->m_nLastEdit = IDS_PASTE;
							m_pTrackMgr->OnDataChanged();
						}

						// Redraw our strip
						m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
						
						// If the property sheet is visible, make it display the Script property page
						m_pTrackMgr->OnShowProperties();

						// Update the Script property page, if it exists
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


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::GetData

HRESULT CScriptStrip::GetData( void **ppData )
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameter
	if( (ppData == NULL) || (*ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = CBaseStrip::GetData( ppData );

	// Make sure masks are set correctly
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(*ppData);
	if( *pdwIndex == GROUPBITSPPG_INDEX )
	{
		PPGTrackParams *pPPGTrackParams = static_cast<PPGTrackParams *>(*ppData);
		pPPGTrackParams->dwPropPageFlags = GROUPBITSPPG_GROUPBITS_RO;
	}
	// Already set - see g_dwTrackExtrasMask and g_dwProducerOnlyMask
	//pPPGTrackParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
	//pPPGTrackParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::SetData

HRESULT CScriptStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameter
	if( pData == NULL )
	{
		return E_POINTER;
	}

	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	if( *pdwIndex == TRACKFLAGSPPG_INDEX )
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

		// Update our track extras flags, if necessary
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pTrackMgr->m_dwTrackExtrasFlags )
		{
			bool fWasRefTimeTrack = m_pTrackMgr->IsRefTimeTrack();
			m_pTrackMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;
			bool fIsRefTimeTrack = m_pTrackMgr->IsRefTimeTrack();

			if( fWasRefTimeTrack != fIsRefTimeTrack )
			{
				SwitchTimeBase();

				// Set strip height and redraw the strip
				VARIANT var;
				var.vt = VT_I4;
				if( fIsRefTimeTrack )
				{
					V_I4(&var) = DEFAULT_STRIP_HEIGHT_REFTIME;
				}
				else
				{
					V_I4(&var) = DEFAULT_STRIP_HEIGHT_MUSICTIME;
				}
				m_pTrackMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, var );
			}

			// Notify our editor that we've changed
			m_pTrackMgr->m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pTrackMgr->OnDataChanged();

			// Redraw our strip
			m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

			// Sync with DirectMusic
			m_pTrackMgr->SyncWithDirectMusic();

			return S_OK;
		}
	}

	return CBaseStrip::SetData( pData );
}


// IDropTarget helpers

/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::CanPasteFromData

HRESULT CScriptStrip::CanPasteFromData(IDataObject* pIDataObject)
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

	// Check if our clipboard format is available in the data object
	hr = S_FALSE;
	if( (pITimelineDataObject->IsClipFormatAvailable( m_cfScriptTrack ) == S_OK)
	||  (pITimelineDataObject->IsClipFormatAvailable( m_cfScript ) == S_OK) )
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
// CScriptStrip::OnLButtonDown

HRESULT CScriptStrip::OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
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
	m_pTrackItemToToggle = NULL;
	
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

	// Show the Script property page
	m_fShowItemProps = true;

	// Check if the shift key is pressed
	if( wParam & MK_SHIFT )
	{
		CListSelectedRegion_ShiftAddRegion( lXPos );
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
					m_pTrackItemToToggle = pItem;
				}
				else
				{
					// Mark the clicked on item as selected
					pItem->m_fSelected = TRUE;
					CListSelectedRegion_AddRegion( pItem );
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
			CListSelectedRegion_ToggleRegion( lXPos );
		}
		// The shift key is up
		else
		{
			// The user left-clicked on blank space without holding the shift or Ctrl keys down

			// Clear all selection regions
			m_pSelectedRegions->Clear();
			m_pTrackMgr->UnselectAll();

			// Select only the beat (or second) clicked on
			CListSelectedRegion_AddRegion( lXPos );
		}

		// Set anchor for future shift-select operations
		m_pSelectedRegions->SetShiftSelectAnchor( lXPos );
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

	// Switch the property page to the Script property page
	m_pTrackMgr->OnShowProperties();

	// Refresh the Script property page, if it exists
	if( m_pTrackMgr->m_pPropPageMgr )
	{
		m_pTrackMgr->m_pPropPageMgr->RefreshData();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::OnRButtonDown

HRESULT CScriptStrip::OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
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
				// Routine is not selected, unselect all items
				m_pTrackMgr->UnselectAll();

				// Select just this item
				pItem->m_fSelected = TRUE;
				CListSelectedRegion_AddRegion( pItem );
			}
			// If the item was already selected, do nothing
		}
		else
		{
			// if region not selected, select it, otherwise do nothing

			// Convert the pixel position to a measure/beat (or minute/second)
			REFERENCE_TIME rtSnapTime;
			if( SUCCEEDED ( m_pTrackMgr->SnapPositionToUnknownTime( lXPos, &rtSnapTime ) ) )
			{
				// Check if this time is not already selected
				if( !CListSelectedRegion_Contains(rtSnapTime) )
				{
					// This beat is not selected - unselect all beats (or seconds)
					m_pTrackMgr->UnselectAll();

					// Now, select just this beat (or second)
					CListSelectedRegion_AddRegion( lXPos );
				}
			}
		}
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	// Switch to the Script property page
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
// CScriptStrip::CanCycle

BOOL CScriptStrip::CanCycle( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTrackMgr->m_pTimeline != NULL );
	if( m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	long lMeasure;
	long lBeat;

	if( SUCCEEDED ( m_pTrackMgr->SnapPositionToUnknownIncrements( m_lXPos, &lMeasure, &lBeat ) ) )
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
// CScriptStrip::SwitchTimeBase

void CScriptStrip::SwitchTimeBase( void )
{
	POSITION pos = m_pTrackMgr->m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_pTrackMgr->m_lstTrackItems.GetNext( pos );

		pItem->SwitchTimeBase();
	}

	SelectRegionsFromSelectedItems();
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::PostRightClickMenu

HRESULT CScriptStrip::PostRightClickMenu( POINT pt )
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
	::EnableMenuItem( hMenuPopup, IDM_CALL_ROUTINE, ( CanCopy() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, IDM_CYCLE_ROUTINES, ( CanCycle() == TRUE ) ? MF_ENABLED :
			MF_GRAYED );

	m_pTrackMgr->m_pTimeline->TrackPopupMenu( hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE );
	::DestroyMenu( hMenu );

	m_fInRightClickMenu = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::UnselectGutterRange

void CScriptStrip::UnselectGutterRange( void )
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
// CScriptStrip::CreateDataObject

HRESULT	CScriptStrip::CreateDataObject( IDataObject** ppIDataObject, long lPosition )
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


	// Save Selected Scripts into stream
	HRESULT hr = E_FAIL;

	// Allocate a memory stream
	IStream* pIStream;
	if( SUCCEEDED ( m_pTrackMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		REFERENCE_TIME rtOffset = 0;

		// Get the item at the drag point
		CTrackItem* pItemAtDragPoint = GetItemFromPoint( lPosition );
		if( pItemAtDragPoint == NULL )
		{
			// If no pItemAtDragPoint use the first selected item
			pItemAtDragPoint = m_pTrackMgr->FirstSelectedItem();
		}
		ASSERT( pItemAtDragPoint->m_fSelected );

		// Compute offset
		MUSIC_TIME mtTimeXPos;
		m_pTrackMgr->m_pTimeline->PositionToClocks( lPosition, &mtTimeXPos );
		m_pTrackMgr->ClocksToUnknownTime( mtTimeXPos, &rtOffset );
		rtOffset = pItemAtDragPoint->m_rtTimePhysical - rtOffset;

		// mark the items as being dragged: this used later for deleting items in drag move
		m_pTrackMgr->MarkSelectedItems(UD_DRAGSELECT);

		// Save the selected items into a stream
		if( SUCCEEDED ( m_pTrackMgr->SaveSelectedItems( pIStream, rtOffset, pItemAtDragPoint ) ) )
		{
			// Add the stream to the Timeline DataObject
			if( SUCCEEDED ( pITimelineDataObject->AddExternalClipFormat( m_cfScriptTrack, pIStream ) ) )
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
// CScriptStrip::DoDragDrop

HRESULT CScriptStrip::DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos)
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
			CScriptStrip::m_pIDocRootOfDragDropSource = m_pTrackMgr->m_pIDocRootNode;
			CScriptStrip::m_fDragDropIntoSameDocRoot = FALSE;

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

					// Notify the Segment Designer that we changed
					m_pTrackMgr->OnDataChanged();

					// Update the selection regions to include the selected items
					SelectRegionsFromSelectedItems();

					// If visible, switch the property sheet to the Routine property page
					m_pTrackMgr->OnShowProperties();

					// If it exists, refresh the Routine property page
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
						if( CScriptStrip::m_fDragDropIntoSameDocRoot == TRUE )
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
	CScriptStrip::m_pIDocRootOfDragDropSource = NULL;
	CScriptStrip::m_fDragDropIntoSameDocRoot = FALSE;
	
	// Return a success or failure code
	return hr;
}


////////////////////////////////////////////////////////////////////////////////
// CScriptStrip::SelectItemsInSelectedRegions

bool CScriptStrip::SelectItemsInSelectedRegions()
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
		if( CListSelectedRegion_Contains(pItem->m_rtTimePhysical) )
		{
			// Check if the item is not selected
			if( !pItem->m_fSelected )
			{
				// Routine isn't yet selected - select it and set fChanged to true
				pItem->m_fSelected = TRUE;
				fChanged = true;
			}
		}
		else
		{
			// Check if the item is selected
			if( pItem->m_fSelected )
			{
				// Routine is selected - deselect it and set fChagned to true
				pItem->m_fSelected = FALSE;
				fChanged = true;
			}
		}
	}

	// return whether or not we changed the selection state of any items
	return fChanged;
}


////////////////////////////////////////////////////////////////////////////////
// CScriptStrip::SelectRegionsFromSelectedItems

void CScriptStrip::SelectRegionsFromSelectedItems()
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
		if( pItem->m_fSelected )
		{
			// Add the beat the item is in to the list of selected regions.
			CListSelectedRegion_AddRegion( pItem );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::IsSelected

bool CScriptStrip::IsSelected()
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
		if ( m_pTrackMgr->m_lstTrackItems.GetNext( pos )->m_fSelected )
		{
			// Routine is selected - return true
			return true;
		}
	}

	// No items selected - return false
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::IsEmpty

bool CScriptStrip::IsEmpty()
{
	if( m_pTrackMgr == NULL )
	{
		ASSERT( 0 );
		return true;
	}

	return m_pTrackMgr->m_lstTrackItems.IsEmpty() ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::SelectSegment

// Return true if anything changed
bool CScriptStrip::SelectSegment( MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime )
{
	if( m_pTrackMgr == NULL 
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return false;
	}

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		long lMinute, lSecond, lMs;
		REFERENCE_TIME rtBeginTime, rtEndTime;
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtBeginTime, &rtBeginTime );
		m_pTrackMgr->RefTimeToMinSecMs( rtBeginTime, &lMinute, &lSecond, &lMs );
		m_pTrackMgr->MinSecMsToRefTime( lMinute, lSecond, 0, &rtBeginTime );
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtBeginTime, &mtBeginTime );

		m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtEndTime, &rtEndTime );
		m_pTrackMgr->RefTimeToMinSecMs( rtEndTime, &lMinute, &lSecond, &lMs );
		m_pTrackMgr->MinSecMsToRefTime( lMinute, (lSecond + 1), 0, &rtEndTime );
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtEndTime, &mtEndTime );
		mtEndTime--;
	}
	else
	{
		long lMeasure, lBeat;
		m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, mtBeginTime, &lMeasure, &lBeat );
		m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( m_pTrackMgr->m_dwGroupBits, 0, lMeasure, lBeat, &mtBeginTime );

		m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, mtEndTime, &lMeasure, &lBeat );
		m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( m_pTrackMgr->m_dwGroupBits, 0, lMeasure, (lBeat + 1), &mtEndTime );
		mtEndTime--;
	}

	// Create a region that contains the selected time
	CMusicTimeConverter cmtBeg( mtBeginTime );
	CMusicTimeConverter cmtEnd( mtEndTime );
	CSelectedRegion region( cmtBeg, cmtEnd );

	// Add the region to the list of selected regions
	m_pSelectedRegions->AddRegion( region );

	// Select all items in the list of selected regions
	// This will return true if the selection state of any item changed
	return SelectItemsInSelectedRegions();
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::CListSelectedRegion_AddRegion

void CScriptStrip::CListSelectedRegion_AddRegion( long lXPos )
{
	ASSERT( m_pTrackMgr != NULL );

	MUSIC_TIME mtBeg;
	MUSIC_TIME mtEnd;

	// Snap XPos to measure/beat (or minute/second) boundary
	CMusicTimeConverter cmtBegX;
	CMusicTimeConverter cmtEndX;
	m_pTrackMgr->SnapPositionToStartEndClocks( lXPos, &mtBeg, &mtEnd );
	cmtBegX = mtBeg;
	cmtEndX = mtEnd;

	CSelectedRegion* psr = new CSelectedRegion( cmtBegX, cmtEndX );
	m_pSelectedRegions->AddHead( psr );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::CListSelectedRegion_AddRegion

void CScriptStrip::CListSelectedRegion_AddRegion( CTrackItem* pItem )
{
	CMusicTimeConverter cmtBeg;
	CMusicTimeConverter cmtEnd;

	MUSIC_TIME mtTime;
	REFERENCE_TIME rtTime;

	ASSERT( m_pTrackMgr != NULL );
	ASSERT( m_pTrackMgr->m_pTimeline != NULL );

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		// Determine start time
		m_pTrackMgr->MinSecMsToRefTime( pItem->m_lMeasure, pItem->m_lBeat, 0, &rtTime );
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtTime, &mtTime );
		cmtBeg = mtTime;

		// Determine end time
		m_pTrackMgr->MinSecMsToRefTime( pItem->m_lMeasure, (pItem->m_lBeat + 1), 0, &rtTime );
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtTime, &mtTime );
		cmtEnd = mtTime;
	}
	else
	{
		// Determine start time
		m_pTrackMgr->MeasureBeatTickToClocks( pItem->m_lMeasure, pItem->m_lBeat, 0, &mtTime );
		cmtBeg = mtTime;

		// Determine end time
		m_pTrackMgr->MeasureBeatTickToClocks( pItem->m_lMeasure, (pItem->m_lBeat + 1), 0, &mtTime );
		cmtEnd = mtTime;
	}

	CSelectedRegion* psr = new CSelectedRegion( cmtBeg, cmtEnd );
	m_pSelectedRegions->AddHead( psr );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::CListSelectedRegion_ToggleRegion

void CScriptStrip::CListSelectedRegion_ToggleRegion( long lXPos )
{
	ASSERT( m_pTrackMgr != NULL );

	MUSIC_TIME mtBeg;
	MUSIC_TIME mtEnd;

	// Snap XPos to measure/beat (or minute/second) boundary
	CMusicTimeConverter cmtBeg;
	CMusicTimeConverter cmtEnd;
	m_pTrackMgr->SnapPositionToStartEndClocks( lXPos, &mtBeg, &mtEnd );
	cmtBeg = mtBeg;
	cmtEnd = mtEnd;

	bool fToggled = false;

	// Get a pointer to the first item in the list
	POSITION posCur;
	POSITION pos = m_pSelectedRegions->GetHeadPosition();
	while( pos )
	{
		posCur = pos;
		CSelectedRegion* psr = m_pSelectedRegions->GetNext( pos );

		if( cmtBeg >= psr->Beg()
		&&  cmtBeg < psr->End() )
		{
			if( (psr->Beg() == cmtBeg) 
			&&  (psr->End() == cmtEnd) )
			{
				// Only this beat (or second) is in the selected regions, delete it
				m_pSelectedRegions->RemoveAt( posCur );
				delete psr;
			}
			else if( psr->Beg() == cmtBeg )
			{
				// The region to toggle is at the start of psr - shorten psr
				psr->Beg() = cmtEnd;

				// Check if this region is empty.  If so, delete it
				if( psr->Beg() == psr->End() )
				{
					m_pSelectedRegions->RemoveAt( posCur );
					delete psr;
				}
			}
			else
			{
				// Create a new CSelectedRegion for the first half
				CSelectedRegion *psrNew = new CSelectedRegion( psr->Beg(), cmtBeg );
				m_pSelectedRegions->InsertBefore( posCur, psrNew );

				// Modify the existing CSelectedRegion to display the second half
				psr->Beg() = cmtEnd;
			}
			fToggled = true;
			break;
		}
	}

	if( fToggled == false )
	{
		CListSelectedRegion_AddRegion( lXPos );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::CListSelectedRegion_ShiftAddRegion

void CScriptStrip::CListSelectedRegion_ShiftAddRegion( long lXPos )
{
	MUSIC_TIME mtBeg;
	MUSIC_TIME mtEnd;

	// Snap XPos to measure/beat (or minute/second) boundary
	CMusicTimeConverter cmtBegX;
	CMusicTimeConverter cmtEndX;
	m_pTrackMgr->SnapPositionToStartEndClocks( lXPos, &mtBeg, &mtEnd );
	cmtBegX = mtBeg;
	cmtEndX = mtEnd;

	// Get the shift-select anchor position
	if( m_pSelectedRegions->IsEmpty() )
	{
		m_pSelectedRegions->SetShiftSelectAnchor( 0 );
	}
	long lShiftSelectAnchorPosition = m_pSelectedRegions->GetShiftSelectAnchor();

	// Snap anchor to measure/beat (or minute/second) boundary
	CMusicTimeConverter cmtBegAnchor;
	CMusicTimeConverter cmtEndAnchor;
	m_pTrackMgr->SnapPositionToStartEndClocks( lShiftSelectAnchorPosition, &mtBeg, &mtEnd );
	cmtBegAnchor = mtBeg;
	cmtEndAnchor = mtEnd;

	// Determine shift-select range
	if( cmtBegAnchor > cmtBegX )
	{
		cmtBegAnchor = cmtBegX;
	}
	if( cmtEndAnchor < cmtEndX )
	{
		cmtEndAnchor = cmtEndX;
	}

	// Delete all items in the selected region list
	while( !m_pSelectedRegions->IsEmpty() )
	{
		CSelectedRegion* psr = m_pSelectedRegions->RemoveHead();
		delete psr;
	}

	// Create a selected region and add to list
	CSelectedRegion* psr = new CSelectedRegion( cmtBegAnchor, cmtEndAnchor );
	m_pSelectedRegions->AddTail( psr );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptStrip::CListSelectedRegion_Contains

bool CScriptStrip::CListSelectedRegion_Contains( REFERENCE_TIME rtTime )
{
	MUSIC_TIME mtTime;
	m_pTrackMgr->SnapUnknownTimeToDisplayClocks( rtTime, &mtTime );

	if( m_pSelectedRegions->Contains( mtTime ) )
	{
		return true;
	}

	return false;
}
