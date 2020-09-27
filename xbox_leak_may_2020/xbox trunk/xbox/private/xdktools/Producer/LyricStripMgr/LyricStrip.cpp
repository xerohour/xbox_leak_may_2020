/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// LyricStrip.cpp : Implementation of CLyricStrip
#include "stdafx.h"
#include <RiffStrm.h>
#include "LyricStripMgr.h"
#include "LyricMgr.h"
#include "PropPageMgr.h"
#include "TrackFlagsPPG.h"
#include "BaseMgr.h"
#include "SegmentIO.h"
#include "GrayOutRect.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This sets the strip's height
#define DEFAULT_STRIP_HEIGHT 20

// This sets the masks for the track flags property page
const DWORD g_dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
const DWORD g_dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;

/////////////////////////////////////////////////////////////////////////////
// CLyricStrip constructor/destructor

CLyricStrip::CLyricStrip( CLyricMgr* pLyricMgr ) : CBaseStrip( pLyricMgr )
{
	// Validate the pointer to our strip manager
	ASSERT( pLyricMgr );
	if ( pLyricMgr == NULL )
	{
		return;
	}

	// Set our pointer to our strip manager
	m_pLyricMgr = pLyricMgr;

	// Initialize our clipboard format to 0
	m_cfLyricList = 0;

	// Initialize our state variables to false
	m_fLeftMouseDown = false;

	// Initialize the lyric to toggle (when CTRL-clicking) to NULL
	m_pLyricItemToToggle = NULL;
}

CLyricStrip::~CLyricStrip()
{
	// Clear our pointer to our strip manager
	ASSERT( m_pLyricMgr );
	if ( m_pLyricMgr )
	{
		m_pLyricMgr = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::GetTopItemFromMeasureBeat

CLyricItem* CLyricStrip::GetTopItemFromMeasureBeat( long lMeasure, long lBeat )
{
	CLyricItem* pTheItem = NULL;
	CLyricItem* pFirstItem = NULL;
	CLyricItem* pItem;

	POSITION pos = m_pLyricMgr->m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		pItem = m_pLyricMgr->m_lstLyrics.GetNext( pos );

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
// CLyricStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// GetNextSelectedLyric - helper method for Draw()

void GetNextSelectedLyric( CTypedPtrList<CPtrList, CLyricItem*>& list, POSITION &posLyric )
{
	// Note that if the lyric at posLyric is selected, this method doesn't do anything useful.
	if( posLyric )
	{
		// Save the current position
		POSITION posToSave = posLyric;

		// Check if we have not run off the end of the list, and if the currently lyric is unselected
		while( posLyric && !list.GetNext( posLyric )->m_fSelected )
		{
			// Current lyric is unselected, save the position of the next lyric
			posToSave = posLyric;
		}

		// Check if we found a selected lyric
		if( posToSave )
		{
			// Save the position of the selected lyric
			posLyric = posToSave;
		}
		// Otherwise both posToSave and posLyric are NULL
	}
}

/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::AdjustTopItem - helper method for Draw()

void CLyricStrip::AdjustTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CLyricItem* pFirstSelectedItem = NULL;
	CLyricItem* pTopItem = NULL;
	CLyricItem* pItem;

	while( pos )
	{
		pItem = m_pLyricMgr->m_lstLyrics.GetNext( pos );

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
// CLyricStrip::GetNextSelectedTopItem - helper method for Draw()

CLyricItem* CLyricStrip::GetNextSelectedTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CLyricItem* pNextItem = NULL;
	CLyricItem* pItem;

	while( pos )
	{
		pItem = m_pLyricMgr->m_lstLyrics.GetNext( pos );

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
// CLyricStrip::GetNextTopItem - helper method for Draw()

CLyricItem* CLyricStrip::GetNextTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CLyricItem* pNextItem = NULL;
	CLyricItem* pItem;

	while( pos )
	{
		pItem = m_pLyricMgr->m_lstLyrics.GetNext( pos );

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
// CLyricStrip::Draw

HRESULT	STDMETHODCALLTYPE CLyricStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that our timeline pointer is valid
	if( m_pLyricMgr == NULL
	||	m_pLyricMgr->m_pTimeline == NULL )
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
	m_pLyricMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &lStartTime );

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

	// A structure to store the size of each lyric in
	SIZE sizeText;

	// Fields to keep track of top item on current measure/beat
	CLyricItem* pTopItem;

	////
	// Draw the unselected lyrics first

	// Iterate through the list of lyrics
	long lPosition;
	long lCurrentMeasure = -1;
	long lCurrentBeat = -1;
	POSITION posLyric = m_pLyricMgr->m_lstLyrics.GetHeadPosition();
	while( posLyric )
	{
		// Save position
		POSITION posLast = posLyric;

		// Get a pointer to each lyric item
		CLyricItem* pLyricItem = m_pLyricMgr->m_lstLyrics.GetNext( posLyric );

		if( pLyricItem->m_lMeasure != lCurrentMeasure
		||  pLyricItem->m_lBeat != lCurrentBeat )
		{
			lCurrentMeasure = pLyricItem->m_lMeasure;
			lCurrentBeat = pLyricItem->m_lBeat;

			// Cleanup
			AdjustTopItem( posLast, lCurrentMeasure, lCurrentBeat );

			// Get the "top" item on this beat
			pTopItem = GetTopItemFromMeasureBeat( lCurrentMeasure, lCurrentBeat );
			if( pTopItem == NULL )
			{
				ASSERT( 0 );	// Should not happen!
				continue;
			}

			// If this lyric is selected and we are not using the gutter selection, skip it
			if( !fUseGutterSelectRange
			&&  pTopItem->m_fSelected )
			{
				continue;
			}

			// Convert the measure and beat of each lyric to a pixel position
			m_pLyricMgr->m_pTimeline->MeasureBeatToPosition( m_pLyricMgr->m_dwGroupBits, 0, lCurrentMeasure, lCurrentBeat, &lPosition );

			// Offset the position by the horizontal offset,
			// and increment it by one pixel so we don't overwrite the bar or beat line
			lPosition += -lXOffset + 1;

			// Check if the left edge of the lyric text is beyond the region we're displaying
			if( lPosition > rectClip.right )
			{
				// We've gone beyond the right edge of the clipping region - break out of the loop
				break;
			}

			// Set the left edge of the highlight rectangle to the start of the text, minus the horizontal offset
			rectHighlight.left = lPosition;

			// Use italics if more than one routine is on this beat
			HFONT hFontOld = NULL;
			if( hFontItalics )
			{
				if( pTopItem != pLyricItem )
				{
					hFontOld = (HFONT)::SelectObject( hDC, hFontItalics );
				}
				else
				{
					if( posLyric )
					{
						POSITION posNext = posLyric;
						CLyricItem* pNextItem = m_pLyricMgr->m_lstLyrics.GetNext( posNext );

						if( pNextItem->m_lMeasure == lCurrentMeasure
						&&  pNextItem->m_lBeat == lCurrentBeat )
						{
							hFontOld = (HFONT)::SelectObject( hDC, hFontItalics );
						}
					}
				}
			}

			// Get a pointer to the next "top" item occuring AFTER this beat
			CLyricItem* pNextItem = GetNextTopItem( posLyric, lCurrentMeasure, lCurrentBeat );

			// Check if there is a lyric after this one
			if( pNextItem ) 
			{
				// Get the size of the lyric text to be written
				::GetTextExtentPoint32( hDC, pTopItem->m_strText, pTopItem->m_strText.GetLength(), &sizeText );

				// Get the start position of the next item
				m_pLyricMgr->m_pTimeline->MeasureBeatToPosition( m_pLyricMgr->m_dwGroupBits, 0, pNextItem->m_lMeasure, pNextItem->m_lBeat, &(rectHighlight.right) );

				// Offset this position with lXOffset
				rectHighlight.right -= lXOffset;

				// Check if the current lyric will run into the next lyric
				if( (rectHighlight.left + sizeText.cx) > rectHighlight.right )
				{
					// Yes, it will run into the next lyric - clip it
					::DrawText( hDC, pTopItem->m_strText, pTopItem->m_strText.GetLength(), &rectHighlight, (DT_LEFT | DT_NOPREFIX) );
				}
				else
				{
					// It won't run into the next lyric - just draw it
					::TextOut( hDC, lPosition, 0, pTopItem->m_strText, pTopItem->m_strText.GetLength() );
				}
			}
			else
			{
				// No more lyrics after this one - just draw it
				::TextOut( hDC, lPosition, 0, pTopItem->m_strText, pTopItem->m_strText.GetLength() );
			}

			if( hFontOld )
			{
				::SelectObject( hDC, hFontOld );
			}
		}
	}

	////
	// Now, draw the selected lyrics

	// Iterate through the list of lyrics, but only if we're not using the gutter selection
	lCurrentMeasure = -1;
	lCurrentBeat = -1;
	posLyric = m_pLyricMgr->m_lstLyrics.GetHeadPosition();
	while( !fUseGutterSelectRange && posLyric )
	{
		// Get a pointer to each lyric item
		CLyricItem* pLyricItem = m_pLyricMgr->m_lstLyrics.GetNext( posLyric );

		if( pLyricItem->m_lMeasure != lCurrentMeasure
		||  pLyricItem->m_lBeat != lCurrentBeat )
		{
			lCurrentMeasure = pLyricItem->m_lMeasure;
			lCurrentBeat = pLyricItem->m_lBeat;

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

			// Convert the measure and beat of each lyric to a pixel position
			m_pLyricMgr->m_pTimeline->MeasureBeatToPosition( m_pLyricMgr->m_dwGroupBits, 0, lCurrentMeasure, lCurrentBeat, &lPosition );

			// Offset the position by the horizontal offset,
			// and increment it by one pixel so we don't overwrite the bar or beat line
			lPosition += -lXOffset + 1;

			// Check if the left edge of the lyric text is beyond the region we're displaying
			if( lPosition > rectClip.right )
			{
				// We've gone beyond the right edge of the clipping region - break out of the loop
				break;
			}

			// Use italics if more than one routine is on this beat
			HFONT hFontOld = NULL;
			if( hFontItalics )
			{
				if( pTopItem != pLyricItem )
				{
					hFontOld = (HFONT)::SelectObject( hDC, hFontItalics );
				}
				else
				{
					if( posLyric )
					{
						POSITION posNext = posLyric;
						CLyricItem* pNextItem = m_pLyricMgr->m_lstLyrics.GetNext( posNext );

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

			// Get a pointer to the next selected "top" item occuring AFTER this beat
			CLyricItem* pNextItem = GetNextSelectedTopItem( posLyric, lCurrentMeasure, lCurrentBeat );

			// Check if there is a selected lyric after the current one
			if( pNextItem )
			{
				// Get the size of the lyric text to be written
				::GetTextExtentPoint32( hDC, pTopItem->m_strText, pTopItem->m_strText.GetLength(), &sizeText );

				// Get the start position of the next lyric
				m_pLyricMgr->m_pTimeline->MeasureBeatToPosition( m_pLyricMgr->m_dwGroupBits, 0, pNextItem->m_lMeasure, pNextItem->m_lBeat, &(rectHighlight.right) );

				// Offset this position with lXOffset
				rectHighlight.right -= lXOffset;

				// Check if the current lyric will run into the next lyric
				if( (rectHighlight.left + sizeText.cx) > rectHighlight.right )
				{
					// Yes, it will run into the next lyric - clip it
					::DrawText( hDC, pTopItem->m_strText, pTopItem->m_strText.GetLength(), &rectHighlight, (DT_LEFT | DT_NOPREFIX) );
				}
				else
				{
					// It won't run into the next lyric - just draw it
					::TextOut( hDC, lPosition, 0, pTopItem->m_strText, pTopItem->m_strText.GetLength() );
				}
			}
			// No more selected lyrics
			else
			{
				// Just draw the lyric
				::TextOut( hDC, lPosition, 0, pTopItem->m_strText, pTopItem->m_strText.GetLength() );
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

		// Get the start position of the lyric list
		posLyric = m_pLyricMgr->m_lstLyrics.GetHeadPosition();

		// Get the start position of the region list
		POSITION posRegion = m_pSelectedRegions->GetHeadPosition();
		while( posRegion )
		{
			// Get a pointer to the current lyric, but only when not using the gutter selection
			CLyricItem* pLyricItem = NULL;

			if( !fUseGutterSelectRange
			&&  posLyric )
			{
				// Get the position of the next selected lyric (it may be the one at posLyric)
				GetNextSelectedLyric( m_pLyricMgr->m_lstLyrics, posLyric );

				// Check if we found a selected lyric
				if( posLyric )
				{
					// Get a pointer to the selected lyric
					pLyricItem = m_pLyricMgr->m_lstLyrics.GetAt( posLyric );

					// Make sure we have the top selected item!
					CLyricItem* pTopItem = GetNextSelectedTopItem( posLyric, pLyricItem->m_lMeasure, (pLyricItem->m_lBeat - 1) );

					// Check if there is a item after this one
					if( pTopItem->m_lMeasure == pLyricItem->m_lMeasure
					&&  pTopItem->m_lBeat == pLyricItem->m_lBeat ) 
					{
						pLyricItem = pTopItem;
					}
				}
			}

			// Get a pointer to this region
			CSelectedRegion* psr = m_pSelectedRegions->GetNext(posRegion);

			// Get the start and end positions for this region
			rectHighlight.left = psr->BeginPos(m_pLyricMgr->m_pTimeline) - lXOffset;
			rectHighlight.right = psr->EndPos(m_pLyricMgr->m_pTimeline) - lXOffset;

			// Check if we have a pointer to a selected lyric and if we're not using gutter selection
			if( !fUseGutterSelectRange
			&&  pLyricItem )
			{
				// Get the clock position of this lyric
				long lLyricClockPos;
				m_pLyricMgr->m_pTimeline->MeasureBeatToClocks( m_pLyricMgr->m_dwGroupBits, 0,
					pLyricItem->m_lMeasure, pLyricItem->m_lBeat, &lLyricClockPos );

				while( posLyric && (lLyricClockPos < psr->End()) )
				{
					// Check if the current lyric item is within this selected region
					if( lLyricClockPos >= psr->Beg() )
					{
						// Compute the left side of the lyric's text display
						long lLyricPos;
						m_pLyricMgr->m_pTimeline->ClocksToPosition( lLyricClockPos, &lLyricPos );

						// Get the size of the lyric text to be written
						::GetTextExtentPoint32( hDC, pLyricItem->m_strText, pLyricItem->m_strText.GetLength(), &sizeText );

						// Add the size of the lyric text and subtract the offset to
						// determine the right side of the lyric's text display.
						lLyricPos += sizeText.cx - lXOffset;

						// Make sure we have the top selected item!
						pLyricItem = GetNextSelectedTopItem( posLyric, pLyricItem->m_lMeasure, pLyricItem->m_lBeat );

						// Check if we found a selected item
						if( pLyricItem )
						{
							posLyric = m_pLyricMgr->m_lstLyrics.Find( pLyricItem, posLyric );

							// Get the clock position of this lyric
							m_pLyricMgr->m_pTimeline->MeasureBeatToClocks( m_pLyricMgr->m_dwGroupBits, 0,
								pLyricItem->m_lMeasure, pLyricItem->m_lBeat, &lLyricClockPos );

							// Compute the start position of the next selected lyric
							long lNextStartPos;
							m_pLyricMgr->m_pTimeline->ClocksToPosition( lLyricClockPos, &lNextStartPos );

							// Ensure that we only exent the selection region up to the
							// start of the next selected lyric
							lLyricPos = min( lNextStartPos, lLyricPos );
						}
						else
						{
							posLyric = NULL;
						}

						// Update rectHighlight.right, if necessary
						rectHighlight.right = max( lLyricPos, rectHighlight.right );
					}
					else
					{
						// Move posLyric so we check the next lyric item
						m_pLyricMgr->m_lstLyrics.GetNext( posLyric );

						// Get the position of the next selected lyric (it may be the one at posLyric)
						GetNextSelectedLyric( m_pLyricMgr->m_lstLyrics, posLyric );

						// Check if we found a selected lyric
						if( posLyric )
						{
							// Get a pointer to the selected lyric
							pLyricItem = m_pLyricMgr->m_lstLyrics.GetAt( posLyric );

							// Get the clock position of this lyric
							m_pLyricMgr->m_pTimeline->MeasureBeatToClocks( m_pLyricMgr->m_dwGroupBits, 0,
								pLyricItem->m_lMeasure, pLyricItem->m_lBeat, &lLyricClockPos );
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
// CLyricStrip::GetLyricFromMeasureBeat

CLyricItem *CLyricStrip::GetLyricFromMeasureBeat( long lMeasure, long lBeat )
{
	// Iterate through the lyric list
	POSITION pos = m_pLyricMgr->m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each lyric item
		CLyricItem* pLyricItem = m_pLyricMgr->m_lstLyrics.GetNext( pos );

		// Check if it is in the measure and beat we're looking for
		if ( pLyricItem->m_lMeasure == lMeasure &&
			 pLyricItem->m_lBeat == lBeat )
		{
			// We found the lyric to return
			return pLyricItem;
		}
		// Check if we've passed the measure yet
		else if ( pLyricItem->m_lMeasure > lMeasure )
		{
			// We've passed the measure - break out of the loop
			break;
		}
	}

	// Didn't find a lyric - return NULL
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::GetLyricFromPoint

CLyricItem *CLyricStrip::GetLyricFromPoint( long lPos )
{
	CLyricItem* pItemReturn = NULL;

	// Validate our timeline pointer
	if( m_pLyricMgr->m_pTimeline )
	{
		// Convert the position to a measure and beat value
		long lMeasure, lBeat;
		if( SUCCEEDED( m_pLyricMgr->m_pTimeline->PositionToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0, lPos, &lMeasure, &lBeat ) ) )
		{
			// Conversion was successful, look for a lyric on that measure and beat
			pItemReturn = GetTopItemFromMeasureBeat( lMeasure, lBeat );
		}
	}

	return pItemReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CLyricStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Initialize the return code to SUCCESS
	HRESULT hr = S_OK;

	// Validate our timeline pointer
	if( m_pLyricMgr->m_pTimeline == NULL )
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
		// Check if the left mouse button is downpLyric
		if(m_fLeftMouseDown)
		{
			// The user moved the mouse while the left mouse button was down -
			// do a drag-drop operation.
			hr = DoDragDrop( m_pLyricMgr->m_pTimeline, wParam, m_lXPos);

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
					// Finish the toggle operation on the lyric that was clicked on
					// when the left mouse button was pressed.
					if( m_pLyricItemToToggle )
					{
						m_pSelectedRegions->ToggleRegion( m_pLyricItemToToggle->m_lMeasure, m_pLyricItemToToggle->m_lBeat );
						m_pLyricItemToToggle = NULL;

						// Update the selection state of the lyrics
						SelectItemsInSelectedRegions();
					}
				}
				// Check if the shift key was not down
				else if( !(wParam & MK_SHIFT) )
				{
					// If a lyric was clicked on
					if(GetLyricFromPoint( lXPos ))
					{
						// Update the selection regions to include only the selected lyrics
						SelectRegionsFromSelectedLyrics();
					}
				}

				// Redraw the strip
				m_pLyricMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

				// Switch the property page to the lyric property page
				m_pLyricMgr->OnShowProperties();
			}
			else
			{
				// successful drag drop--make sure that only the regions with
				// selected lyrics are selected
				SelectRegionsFromSelectedLyrics();
			}

			// Refresh the lyric property page, if it exists
			if( m_pLyricMgr->m_pPropPageMgr )
			{
				m_pLyricMgr->m_pPropPageMgr->RefreshData();
			}
		}
		break;

	case WM_LBUTTONUP:
		// Clear the flag so we don't start a drag-drop operation when the mouse moved
		m_fLeftMouseDown = false;

		// Check if the Ctrl key was pressed
		if(wParam & MK_CONTROL)
		{
			// Ctrl key pressed - toggle the selection state of the lyric that was clicked on
			if( m_pLyricItemToToggle )
			{
				m_pSelectedRegions->ToggleRegion( m_pLyricItemToToggle->m_lMeasure, m_pLyricItemToToggle->m_lBeat );
				m_pLyricItemToToggle = NULL;

				// Update the selection regions from which lyric are selected
				SelectItemsInSelectedRegions();
			}
		}
		// Check if the Shift key was not pressed
		else if( !(wParam & MK_SHIFT) )
		{
			// Look for a lyric at the position clicked on
			CLyricItem* pLyric = GetLyricFromPoint( lXPos );
			if(pLyric)
			{
				// Found a lyric
				// Unselect all lyrics
				m_pLyricMgr->UnselectAll();

				// Mark the clicked on lyric as selected
				pLyric->m_fSelected = TRUE;

				// Update the selection regions to include just this selected lyric
				SelectRegionsFromSelectedLyrics();
			}
		}

		// Update the position to inset at
		m_lXPos = lXPos;

		// Ensure all other strips are unselected
		UnselectGutterRange();

		// Redraw ourself
		m_pLyricMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

		// Refresh the lyric property page, if it exists
		if( m_pLyricMgr->m_pPropPageMgr )
		{
			m_pLyricMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	case WM_COMMAND:
	{
		// We should only get this message in response to a selection in the right-click context menu.
//		WORD wNotifyCode = HIWORD( wParam );	// notification code 
		WORD wID		 = LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
			case IDM_CYCLE_LYRICS:
				hr = m_pLyricMgr->CycleItems( m_lXPos );
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
		if( m_cfLyricList == 0 )
		{
			m_cfLyricList = RegisterClipboardFormat( CF_LYRICLIST );
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
// CLyricStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::Copy

HRESULT CLyricStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
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
	if( (m_pLyricMgr == NULL)
	||	(m_pLyricMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfLyricList == 0 )
	{
		m_cfLyricList = RegisterClipboardFormat( CF_LYRICLIST );
		if( m_cfLyricList == 0 )
		{
			ASSERT(FALSE);
			return E_FAIL;
		}
	}

	// Create an IStream to save the selected lyrics in.
	IStream* pStreamCopy;
	hr = CreateStreamOnHGlobal( NULL, TRUE, &pStreamCopy );

	// Verify that we were able to create a stream
	if( FAILED( hr ))
	{
		return E_OUTOFMEMORY;
	}

	// Get the span of the selected lyrics
	long lStartTime, lEndTime;
	m_pSelectedRegions->GetSpan(lStartTime, lEndTime);

	// Convert the start time to a measure and beat
	long lMeasure, lBeats;
	m_pLyricMgr->m_pTimeline->ClocksToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0,
								lStartTime, &lMeasure, &lBeats );

	// Convert the start time to a number of beats
	MeasureBeatToBeats( m_pLyricMgr->m_pTimeline, m_pLyricMgr->m_dwGroupBits, 0, lMeasure, lBeats, lBeats );

	// Convert the end time to a measure and beat
	long lEndBeats;
	m_pLyricMgr->m_pTimeline->ClocksToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0,
								lEndTime, &lMeasure, &lEndBeats );

	// Convert the end time to a number of beats
	MeasureBeatToBeats( m_pLyricMgr->m_pTimeline, m_pLyricMgr->m_dwGroupBits, 0, lMeasure, lEndBeats, lEndBeats );

	// Compute the span of the region we're copying
	lEndBeats -= lBeats;

	// Save the number of beats in the region we're copying
	pStreamCopy->Write( &lEndBeats, sizeof(long), NULL );

	// Save the selected lyrics into the stream
	hr = m_pLyricMgr->SaveSelectedLyrics( pStreamCopy, lBeats );

	// Check if the save succeeded
	if( FAILED( hr ))
	{
		// Save failed
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	// The save succeeded - copy the stream to the clipboard
	hr = CopyDataToClipboard( pITimelineDataObject, pStreamCopy, m_cfLyricList, m_pLyricMgr, this );

	// Release our pointer to the stream
	pStreamCopy->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::PasteAt

HRESULT CLyricStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lPasteTime, bool bDropNotPaste, BOOL &fChanged )
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
		if( m_pLyricMgr->m_pTimeline )
		{
			// Determine the measure and beat we're dropping in
			long lMeasure1, lBeat1;
			if(SUCCEEDED(m_pLyricMgr->m_pTimeline->ClocksToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0, lPasteTime, &lMeasure1, &lBeat1 )))
			{
				// Compute the measure and beat the drag started from
				long lMeasure2, lBeat2;
				if(SUCCEEDED(m_pLyricMgr->m_pTimeline->PositionToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0, m_lStartDragPosition, &lMeasure2, &lBeat2 )))
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

	// Check if there is a lyric list avilable for us
	if( pITimelineDataObject->IsClipFormatAvailable( m_cfLyricList ) == S_OK )
	{
		// Try and read the stream that contains the lyrics
		IStream* pIStream;
		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfLyricList, &pIStream)))
		{
			// If we're pasting, read in the number of beats that this selection covers
			long lBeatsToPaste = -1;
			if( !bDropNotPaste )
			{
				pIStream->Read( &lBeatsToPaste, sizeof(long), NULL );
			}

			// Load the stream into a list of lyric items
			CTypedPtrList<CPtrList, CLyricItem*> list;
			hr = LoadLyricList(list, pIStream, m_pLyricMgr);

			// Check if the load operation succeeded
			if ( SUCCEEDED(hr) )
			{
				if( m_pLyricMgr->m_pTimeline )
				{
					// Get the measure and beat of the drop or paste position
					long lMeasurePaste, lBeatPaste;
					if(FAILED(m_pLyricMgr->m_pTimeline->ClocksToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0, 
																	max( lPasteTime, 0 ), &lMeasurePaste, &lBeatPaste )))
					{
						hr = E_FAIL;
						goto Leave_1;
					}

					long lBeatStart;
					MeasureBeatToBeats( m_pLyricMgr->m_pTimeline, m_pLyricMgr->m_dwGroupBits, 0, lMeasurePaste, lBeatPaste, lBeatStart );

					// The length of the segment, in beats
					long lBeatSegmentLength = LONG_MAX;

					// Check if there are any lyrics to paste
					if( !list.IsEmpty() )
					{
						// Unselect all existing lyrics in this strip so the only selected lyrics are the dropped
						// or pasted ones
						m_pLyricMgr->UnselectAll();

						// Make sure the last lyric lands in the last beat or sooner
						// Retrieve the clock length of the segment
						VARIANT varLength;
						if( SUCCEEDED( m_pLyricMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
						{
							// Compute the measure and beat length of the segment
							long lMeasure, lBeat;
							if( SUCCEEDED( m_pLyricMgr->m_pTimeline->ClocksToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0, 
																			V_I4(&varLength), &lMeasure, &lBeat ) ) )
							{
								// Compute the beat length of the segment
								MeasureBeatToBeats( m_pLyricMgr->m_pTimeline, m_pLyricMgr->m_dwGroupBits, 0,
									lMeasure, lBeat, lBeatSegmentLength );

								// Make sure the last lyric lands in the last beat or sooner
								if( list.GetTail()->m_mtTimePhysical + lBeatStart >= lBeatSegmentLength )
								{
									lBeatStart = lBeatSegmentLength - list.GetTail()->m_mtTimePhysical - 1;
								}
							}
						}

						// Make sure the first lyric lands in beat 0 or later
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
						if( FAILED( m_pLyricMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
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
							BeatsToMeasureBeat( m_pLyricMgr->m_pTimeline, m_pLyricMgr->m_dwGroupBits, 0,
								lBeatStart + lBeatsToPaste, lmEnd, lbEnd );

							// Now, delete any lyrics that fall between the first and last beats we're pasting in
							fChanged = m_pLyricMgr->DeleteBetweenMeasureBeats(lMeasurePaste, lBeatPaste, lmEnd, lbEnd );
						}
					}

					// Iterate throught the list of lyrics we loaded
					while( !list.IsEmpty() )
					{
						// Remove the head of the list
						CLyricItem* pItem = list.RemoveHead();

						// Check if the item will land before the end of the segment
						if( pItem->m_mtTimePhysical + lBeatStart < lBeatSegmentLength )
						{
							MUSIC_TIME mtTime;
							long lMeasure, lBeat;

							// m_mtTimeLogical
							// When we saved this to a stream in CLyricMgr::SaveSelectedLyrics, we stored
							// the beat offset into m_mtTimeLogical.
							BeatsToMeasureBeat( m_pLyricMgr->m_pTimeline, m_pLyricMgr->m_dwGroupBits, 0,
												max( 0, pItem->m_mtTimeLogical + lBeatStart), lMeasure, lBeat );
							m_pLyricMgr->ForceBoundaries( lMeasure, lBeat, 0, &mtTime );
							pItem->SetTimeLogical( mtTime );

							// m_mtTimePhysical
							// When we saved this to a stream in CLyricMgr::SaveSelectedLyrics, we stored
							// the beat offset into m_mtTimePhysical.
							// m_lTick should already contain correct value 
							BeatsToMeasureBeat( m_pLyricMgr->m_pTimeline, m_pLyricMgr->m_dwGroupBits, 0,
												pItem->m_mtTimePhysical + lBeatStart, lMeasure, lBeat );
							m_pLyricMgr->MeasureBeatTickToClocks( lMeasure, lBeat, pItem->m_lTick, &mtTime );
							pItem->SetTimePhysical( mtTime, STP_LOGICAL_NO_ACTION );

							// When pasted or dropped, each lyric is selected
							pItem->m_fSelected = TRUE;

							// This will overwrite any lyric that already exists on the measure, beat, and
							// tick where pItem will be inserted
							m_pLyricMgr->InsertByAscendingTime( pItem, TRUE );

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
		// Drag/drop Target and Source are the same Lyric strip
		m_nStripIsDragDropSource = 2;
	}

	if( CLyricStrip::m_pIDocRootOfDragDropSource
	&&  CLyricStrip::m_pIDocRootOfDragDropSource == m_pLyricMgr->m_pIDocRootNode )
	{
		// Drag/drop Target and Source are the same file (i.e. Segment)
		CLyricStrip::m_fDragDropIntoSameDocRoot = TRUE;
	}
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::Paste

HRESULT CLyricStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

	// Validate our LyricMgr and Timeline pointers
	if( (m_pLyricMgr == NULL)
	||	(m_pLyricMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get a TimelineDataObject, if we don't already have one
	hr = GetTimelineDataObject( pITimelineDataObject, m_pLyricMgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Convert the paste position to clocks
	long lClocks;
	if( FAILED( m_pLyricMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lClocks) ) )
	{
		return E_UNEXPECTED;
	}

	// Do the paste
	BOOL fChanged = FALSE;
	hr = PasteAt(pITimelineDataObject, lClocks, false, fChanged);

	// Check if the paste succeeded AND if something was actually pasted
	if( SUCCEEDED(hr) && fChanged )
	{
		// Update the selection regions to only include the selected lyrics
		SelectRegionsFromSelectedLyrics();

		// Notify the Segment Designer that we did a paste operation
		m_pLyricMgr->m_nLastEdit = IDS_PASTE;
		m_pLyricMgr->OnDataChanged();

		// Redraw our strip
		m_pLyricMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

		// Ensure the lyric property page is visible
		m_pLyricMgr->OnShowProperties();

		// Refresh the lyric property page
		if( m_pLyricMgr->m_pPropPageMgr )
		{
			m_pLyricMgr->m_pPropPageMgr->RefreshData();
		}

		// Sync with DirectMusic
		m_pLyricMgr->SyncWithDirectMusic();
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::Insert

HRESULT CLyricStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Double-check that our timeline pointer is valid
	if( (m_pLyricMgr == NULL)
	||	(m_pLyricMgr->m_pTimeline == NULL) )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	if( m_lXPos < 0 )
	{
		return E_FAIL;
	}

	// Unselect all items in this strip 
	m_pLyricMgr->UnselectAll();

	// Unselect all items in other strips
	UnselectGutterRange();

	// Convert the insert position to a measure and beat value
	long	lMeasure, lBeat;
	if( FAILED( m_pLyricMgr->m_pTimeline->PositionToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0, max( 0, m_lXPos ), &lMeasure, &lBeat ) ) )
	{
		return E_UNEXPECTED;
	}

	CLyricItem* pLyric = new CLyricItem( m_pLyricMgr );
	if(pLyric == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Try and load the default text
	if( !pLyric->m_strText.LoadString( IDS_DEFAULT_LYRIC_TEXT ) )
	{
		// Failed to load the text, just use a default english string
		pLyric->m_strText = _T("Empty");
	}

	// Set the time that the lyric was inserted in
	MUSIC_TIME mtTime;
	m_pLyricMgr->MeasureBeatTickToClocks( lMeasure, lBeat, 0, &mtTime );
	pLyric->SetTimePhysical( mtTime, STP_LOGICAL_SET_DEFAULT );

	// By default, a newly selected lyric is selected
	pLyric->m_fSelected = TRUE;

	// Insert the lyric into our list of lyics, overwriting any existing one
	m_pLyricMgr->InsertByAscendingTime( pLyric, FALSE );

	// If it's not already selected, add the beat the lyric was inserted on
	// to the list of selected regions
	if( !m_pSelectedRegions->Contains( lMeasure, lBeat ) )
	{
		m_pSelectedRegions->AddRegion(m_lXPos);
	}

	// Redraw our strip
	m_pLyricMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

	// Show the property sheet
	ShowPropertySheet();

	// Switch to the lyric property page
	m_pLyricMgr->OnShowProperties();

	// Refresh the lyric property page
	if( m_pLyricMgr->m_pPropPageMgr )
	{
		m_pLyricMgr->m_pPropPageMgr->RefreshData();
	}

	// Notify the Segment Designer that an insert occurred
	m_pLyricMgr->m_nLastEdit = IDS_INSERT;
	m_pLyricMgr->OnDataChanged();

	// No need to sync with DirectMusic - the lyric starts out as empty

	// Return the success/failure code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::Delete

HRESULT CLyricStrip::Delete( void )
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
	if( (m_pLyricMgr == NULL)
	||	(m_pLyricMgr->m_pTimeline == NULL) )
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}

	// Delete all selected lyrics
	m_pLyricMgr->DeleteSelectedLyrics();

	// Clear all selection ranges
	m_pSelectedRegions->Clear();

	// Redraw our strip
	m_pLyricMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	// Refresh the lyric property page, if it exists
	if( m_pLyricMgr->m_pPropPageMgr )
	{
		m_pLyricMgr->m_pPropPageMgr->RefreshData();
	}

	// Notify the Segment Designer that a delete operation occurred
	m_pLyricMgr->m_nLastEdit = IDS_DELETE;
	m_pLyricMgr->OnDataChanged();

	// Sync with DirectMusic
	m_pLyricMgr->SyncWithDirectMusic();

	// Return that the delete succeeded
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::CanPaste

HRESULT CLyricStrip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid timeline pointer
	if( (m_pLyricMgr == NULL)
	||	(m_pLyricMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfLyricList == 0 )
	{
		m_cfLyricList = RegisterClipboardFormat( CF_LYRICLIST );
		if( m_cfLyricList == 0 )
		{
			return E_FAIL;
		}
	}

	// Get a TimelineDataObject, if we don't already have one
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_pLyricMgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check the timeline data object for our clipboard format
	hr = pITimelineDataObject->IsClipFormatAvailable( m_cfLyricList );

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
// CLyricStrip::CanInsert

HRESULT CLyricStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid Timeline pointer
	if( (m_pLyricMgr == NULL)
	||	(m_pLyricMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get the position to paste at
	long lInsertTime;
	if( FAILED( m_pLyricMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lInsertTime) ) )
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
	if( FAILED( m_pLyricMgr->m_pTimeline->ClocksToPosition( lInsertTime, &lPosition ) ) )
	{
		return E_UNEXPECTED;
	}

	// Check to see if there is an existing lyric at the insert position
	CLyricItem* pLyric = GetLyricFromPoint( lPosition );

	// Check if we found a lyric
	if( pLyric != NULL )
	{
		// We found a lyric - return S_FALSE since we can't insert here
		return S_FALSE;
	}

	// Get the length of the segment, in clocks
	VARIANT var;
	if( FAILED( m_pLyricMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var) ) )
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
// CLyricStrip::Drop

HRESULT CLyricStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
		if( SUCCEEDED( m_pLyricMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			// Import the DataObject that was dragged
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				// Compute the time to drop at
				long lPasteTime;
				m_pLyricMgr->m_pTimeline->PositionToClocks( pt.x, &lPasteTime );

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
						// Update the list of selected regions to include only the selected lyrics
						SelectRegionsFromSelectedLyrics();

						if( CLyricStrip::m_fDragDropIntoSameDocRoot == FALSE
						||  m_dwOverDragEffect != DROPEFFECT_MOVE )
						{
							// Either we are dropping into a different file,
							// or we are doing a "copy" and there will be no change to the source strip, so....
							// Notify the SegmentDesigner that a paste operation occurred
							m_pLyricMgr->m_nLastEdit = IDS_PASTE;
							m_pLyricMgr->OnDataChanged();
						}

						// Redraw our strip
						m_pLyricMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
						
						// If the property sheet is visible, make it display the lyric property page
						m_pLyricMgr->OnShowProperties();

						// Update the lyric property page, if it exists
						if( m_pLyricMgr->m_pPropPageMgr )
						{
							m_pLyricMgr->m_pPropPageMgr->RefreshData();
						}

						// Sync with DirectMusic
						m_pLyricMgr->SyncWithDirectMusic();
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
// CLyricStrip::CanPasteFromData

HRESULT CLyricStrip::CanPasteFromData(IDataObject* pIDataObject)
{
	// Check that we're passed a valid IDataObject pointer
	if( pIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Get a Timeline DataObject that encapsulates the pIDataObject
	IDMUSProdTimelineDataObject *pITimelineDataObject = NULL;
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_pLyricMgr->m_pTimeline, pIDataObject );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check if our clipboard format is available in the data object
	hr = S_FALSE;
	if( pITimelineDataObject->IsClipFormatAvailable( m_cfLyricList ) == S_OK )
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
// CLyricStrip::OnLButtonDown

HRESULT CLyricStrip::OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);

	// Validate our timeline pointer
	if( (m_pLyricMgr == NULL)
	||	(m_pLyricMgr->m_pTimeline == NULL) )
	{
		return E_UNEXPECTED;
	}

	// Initialize the lyric to toggle to NULL
	m_pLyricItemToToggle = NULL;
	
	// Unselect all items in the other strips
	UnselectGutterRange();

	// Capture mouse so we get the WM_LBUTTONUP message as well.
	// The timeline will release the capture when it receives the
	// WM_LBUTTONUP message.
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pLyricMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Flag that the left mouse button is pressed
	m_fLeftMouseDown = true;

	// Show the Lyric property page
	m_fShowItemProps = true;

	// Check if the shift key is pressed
	if( wParam & MK_SHIFT )
	{
		m_pSelectedRegions->ShiftAddRegion( lXPos );
		SelectItemsInSelectedRegions();
	}
	else
	{
		// See if there is a lyric under the cursor.
		CLyricItem* pLyric = GetLyricFromPoint( lXPos );
		if( pLyric )
		{
			// Found a lyric under the cursor

			// Check if the control key is down
			if( wParam & MK_CONTROL )
			{
				// Check if the lyric is not yet selected
				if( pLyric->m_fSelected )
				{
					// Set up to unselect this item later, either when we receive a
					// left-button up, or when the user completes a drag-drop operation
					// that does nothing.
					m_pLyricItemToToggle = pLyric;
				}
				else
				{
					// Mark the clicked on item as selected
					pLyric->m_fSelected = TRUE;
					m_pSelectedRegions->AddRegion(lXPos);
				}
			}
			// Check if the lyric is unselected (the shift key is up)
			else if( !pLyric->m_fSelected )
			{
				// Mark the clicked on item as selected
				m_pLyricMgr->UnselectAll();
				pLyric->m_fSelected = TRUE;

				// Update the selection regions to include just this selected item
				SelectRegionsFromSelectedLyrics();
			}
		}
		// Didn't find a lyric - check if the Ctrl key is down
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
			m_pLyricMgr->UnselectAll();

			// Select only the beat clicked on
			m_pSelectedRegions->AddRegion( lXPos );
		}

		// Set anchor for future shift-select operations
		m_pSelectedRegions->SetShiftSelectAnchor( lXPos );
	}

	// Redraw our strip
	m_pLyricMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

	// Switch the property page to the lyric property page
	m_pLyricMgr->OnShowProperties();

	// Refresh the lyric property page, if it exists
	if( m_pLyricMgr->m_pPropPageMgr )
	{
		m_pLyricMgr->m_pPropPageMgr->RefreshData();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::OnRButtonDown

HRESULT CLyricStrip::OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);
	ASSERT( m_pLyricMgr->m_pTimeline != NULL );

	// Unselect all items in the other strips
	UnselectGutterRange();

	// Get the item at the mouse click.
	CLyricItem* pLyric = GetLyricFromPoint( lXPos );

	if( !(wParam & (MK_CONTROL | MK_SHIFT)) )
	{
		// Neither control or shift were pressed

		// Check if a lyric was clicked on
		if( pLyric )
		{
			// Check if the lyric is selected
			if( pLyric->m_fSelected == FALSE )
			{
				// Lyric is not selected, unselect all lyrics
				m_pLyricMgr->UnselectAll();

				// Select just this lyric
				pLyric->m_fSelected = TRUE;

				// Add just this beat to the list of selected regions
				m_pSelectedRegions->AddRegion(lXPos);
			}
			// If the lyric was already selected, do nothing
		}
		else
		{
			// if region not selected, select it, otherwise do nothing

			// Convert the pixel position to a measure and beat
			long lMeasure, lBeat;
			if( SUCCEEDED( m_pLyricMgr->m_pTimeline->PositionToMeasureBeat(m_pLyricMgr->m_dwGroupBits,
															0,lXPos, &lMeasure, &lBeat) ) )
			{
				// Check if this beat is not already selected
				if( !m_pSelectedRegions->Contains(lMeasure, lBeat))
				{
					// This beat is not selected - unselect all beats
					m_pLyricMgr->UnselectAll();

					// Now, select just this beat
					m_pSelectedRegions->AddRegion(lXPos);
				}
			}
		}
	}

	// Redraw our strip
	m_pLyricMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	// Switch to the lyric property page
	m_fShowItemProps = true;
	m_pLyricMgr->OnShowProperties(); 

	// Refresh the lyric property page, if it exists
	if( m_pLyricMgr->m_pPropPageMgr )
	{
		m_pLyricMgr->m_pPropPageMgr->RefreshData();
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::CanCycle

BOOL CLyricStrip::CanCycle( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pLyricMgr->m_pTimeline != NULL );
	if( m_pLyricMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	long lMeasure;
	long lBeat;

	if( SUCCEEDED ( m_pLyricMgr->m_pTimeline->PositionToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0, m_lXPos, &lMeasure, &lBeat ) ) )
	{
		int nCount = 0;
		CLyricItem* pItem;

		POSITION pos = m_pLyricMgr->m_lstLyrics.GetHeadPosition();
		while( pos )
		{
			pItem = m_pLyricMgr->m_lstLyrics.GetNext( pos );

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
// CLyricStrip::PostRightClickMenu

HRESULT CLyricStrip::PostRightClickMenu( POINT pt )
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
	::EnableMenuItem( hMenuPopup, IDM_CYCLE_LYRICS, ( CanCycle() == TRUE ) ? MF_ENABLED :
			MF_GRAYED );

	m_pLyricMgr->m_pTimeline->TrackPopupMenu( hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE );
	::DestroyMenu( hMenu );

	m_fInRightClickMenu = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::UnselectGutterRange

void CLyricStrip::UnselectGutterRange( void )
{
	// Make sure everything on the timeline is deselected.

	// Flag that we're unselecting all other strips
	m_fSelecting = TRUE;

	// Set the begin and end selection time to 0
	m_pLyricMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pLyricMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );

	// Flags that we're no longer unselecting all other strips
	m_fSelecting = FALSE;
}


// IDropSource helpers

/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::CreateDataObject

HRESULT	CLyricStrip::CreateDataObject(IDataObject** ppIDataObject, long lPosition)
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
	if( FAILED( m_pLyricMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
	{
		return E_OUTOFMEMORY;
	}


	// Save Selected Lyrics into stream
	HRESULT hr = E_FAIL;

	// Allocate a memory stream
	IStream* pIStream;
	if( SUCCEEDED ( m_pLyricMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// Get the lyric at the drag point
		CLyricItem* pLyricAtDragPoint = GetLyricFromPoint( lPosition );

		// mark the lyrics as being dragged: this used later for deleting lyrics in drag move
		m_pLyricMgr->MarkSelectedLyrics(UD_DRAGSELECT);

		// Save the selected lyrics into a stream
		if( SUCCEEDED ( m_pLyricMgr->SaveSelectedLyrics( pIStream, pLyricAtDragPoint ) ) )
		{
			// Add the stream to the Timeline DataObject
			if( SUCCEEDED ( pITimelineDataObject->AddExternalClipFormat( m_cfLyricList, pIStream ) ) )
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
// CLyricStrip::DoDragDrop

HRESULT CLyricStrip::DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos)
{
	// Do a drag'n'drop operation

	// Get the lyric at the point we're dragging from
	CLyricItem* pLyric = GetLyricFromPoint( lXPos );

	// Check that we found a lyric item, and that it is selected
	if(!pLyric || !pLyric->m_fSelected)
	{
		// Didn't find a selected lyric - return that nothing happened
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

		// Create a data object from the selected lyrics
		hr = CreateDataObject( &m_pISourceDataObject, lXPos );

		// Check if we successfully created a data object
		if(SUCCEEDED(hr))
		{
			// We can always copy lyrics.
			DWORD dwOKDragEffects = DROPEFFECT_COPY;

			// Check if we can cut lyrics
			if(CanCut() == S_OK)
			{
				// If we can Cut(), allow the user to move the lyrics as well.
				dwOKDragEffects |= DROPEFFECT_MOVE;
			}

			// Do the Drag/Drop.

			// Initialize our DragDropSource flag to 1 (means we are the source of a drag-drop)
			m_nStripIsDragDropSource = 1;

			// Initialize our DragDropSource DocRoot fields
			CLyricStrip::m_pIDocRootOfDragDropSource = m_pLyricMgr->m_pIDocRootNode;
			CLyricStrip::m_fDragDropIntoSameDocRoot = FALSE;

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
					// We moved some lyrics - delete the lyrics we marked earlier
					m_pLyricMgr->DeleteMarked(UD_DRAGSELECT);
				}

				// If the effect wasn't 'None', set the drop flag to true
				if(dwEffect != DROPEFFECT_NONE)
				{
					fDrop = true;
				}
				break;
			default:
				// User cancelled the operation, or dropped the lyrics somewhere they didn't belong
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
						m_pLyricMgr->m_nLastEdit = IDS_UNDO_MOVE;
					}
					else
					{
						// We did a copy - use the 'Insert' undo text
						m_pLyricMgr->m_nLastEdit = IDS_INSERT;
					}

					// Notify the Segment Designer that we changed
					m_pLyricMgr->OnDataChanged();

					// Update the selection regions to include the selected lyrics
					SelectRegionsFromSelectedLyrics();

					// If visible, switch the property sheet to the lyric property page
					m_pLyricMgr->OnShowProperties();

					// If it exists, refresh the lyric property page
					if( m_pLyricMgr->m_pPropPageMgr )
					{
						m_pLyricMgr->m_pPropPageMgr->RefreshData();
					}

					// Sync with DirectMusic
					m_pLyricMgr->SyncWithDirectMusic();
				}
				else
				{
					// Target strip is different from source strip
					if( dwEffect & DROPEFFECT_MOVE )
					{
						// Check if we dropped in the same segment
						if( CLyricStrip::m_fDragDropIntoSameDocRoot == TRUE )
						{
							// We did a move operation to another strip in the same segment - use the 'Move' undo text
							m_pLyricMgr->m_nLastEdit = IDS_UNDO_MOVE;
						}
						else
						{
							// We did a move operation to another strip in a different segment - use the 'Delete' undo text
							m_pLyricMgr->m_nLastEdit = IDS_DELETE;
						}

						// Notify the Segment Designer that we changed
						m_pLyricMgr->OnDataChanged();

						// Sync with DirectMusic
						m_pLyricMgr->SyncWithDirectMusic();
					}
					else
					{
						// Unselect all lyrics, since the user did a copy operation
						m_pLyricMgr->UnselectAll();
					}
				}

				// Refresh our strip display
				m_pLyricMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

				// Return that we actually did a drop
				hr = S_OK;
			}
		}

		// Release the IDropSource interface
		pIDropSource->Release();

		// Clear the DragSelect flag from all lyrics
		m_pLyricMgr->UnMarkLyrics(UD_DRAGSELECT); 
	}
	else
	{
		// Couldn't query for IDropSource interface - return failure code
		hr = E_FAIL;
	}

	// Clear the DragDropSource flag
	m_nStripIsDragDropSource = 0;
	CLyricStrip::m_pIDocRootOfDragDropSource = NULL;
	CLyricStrip::m_fDragDropIntoSameDocRoot = FALSE;

	// Return a success or failure code
	return hr;
}


////////////////////////////////////////////////////////////////////////////////
// CLyricStrip::SelectItemsInSelectedRegions

bool CLyricStrip::SelectItemsInSelectedRegions()
{
	// Flag set to true if anything changes
	bool fChanged = false;

	// Iterate through the list of lyrics
	POSITION pos = m_pLyricMgr->m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each lyric item
		CLyricItem* pLyricItem = m_pLyricMgr->m_lstLyrics.GetNext( pos );

		// Check if this lyric is within a selection region
		if(m_pSelectedRegions->Contains(pLyricItem->m_lMeasure, pLyricItem->m_lBeat))
		{
			// Check if the lyric is not selected
			if( !pLyricItem->m_fSelected )
			{
				// Lyric isn't yet selected - select it and set fChanged to true
				pLyricItem->m_fSelected = TRUE;
				fChanged = true;
			}
		}
		else
		{
			// Check if the lyric is selected
			if( pLyricItem->m_fSelected )
			{
				// Lyric is selected - deselect it and set fChagned to true
				pLyricItem->m_fSelected = FALSE;
				fChanged = true;
			}
		}
	}

	// return whether or not we changed the selection state of any lyrics
	return fChanged;
}


////////////////////////////////////////////////////////////////////////////////
// CLyricStrip::SelectRegionsFromSelectedLyrics

void CLyricStrip::SelectRegionsFromSelectedLyrics()
{
	// Clear the list of selected regions
	m_pSelectedRegions->Clear();

	// Iterate through the list of lyrics
	POSITION pos = m_pLyricMgr->m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each lyric item
		CLyricItem* pLyricItem = m_pLyricMgr->m_lstLyrics.GetNext( pos );

		// Check if the lyric is selected
		if(pLyricItem->m_fSelected)
		{
			// Add the beat the lyric is in to the list of selected regions.
			CListSelectedRegion_AddRegion(*m_pSelectedRegions, *pLyricItem);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::IsSelected

bool CLyricStrip::IsSelected()
{
	if( m_pLyricMgr == NULL )
	{
		ASSERT( 0 );
		return false;
	}

	// If anything is selected, return true
	
	// Iterate through the list of lyrics
	POSITION pos = m_pLyricMgr->m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Check if this lyric is selected
		if( m_pLyricMgr->m_lstLyrics.GetNext( pos )->m_fSelected )
		{
			// Lyric is selected - return true
			return true;
		}
	}

	// No lyrics selected - return false
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::IsEmpty

bool CLyricStrip::IsEmpty()
{
	if( m_pLyricMgr == NULL )
	{
		ASSERT( 0 );
		return true;
	}

	return m_pLyricMgr->m_lstLyrics.IsEmpty() ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::SelectSegment

// Return true if anything changed
bool CLyricStrip::SelectSegment(MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime)
{
	// Verify that we have a pointer to the Timeline
	if( m_pLyricMgr == NULL 
	||  m_pLyricMgr->m_pTimeline == NULL )
	{
		ASSERT(FALSE);
		return false;
	}

	// Convert the start position from clocks to a measure and beat value
	long lMeasure, lBeat;
	m_pLyricMgr->m_pTimeline->ClocksToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0,
												   mtBeginTime, &lMeasure, &lBeat );

	
	// Convert back to a clock value
	m_pLyricMgr->m_pTimeline->MeasureBeatToClocks( m_pLyricMgr->m_dwGroupBits, 0,
												   lMeasure, lBeat, &mtBeginTime );

	// Convert the end position from clocks to a measure and beat value
	m_pLyricMgr->m_pTimeline->ClocksToMeasureBeat( m_pLyricMgr->m_dwGroupBits, 0,
												   mtEndTime, &lMeasure, &lBeat );

	
	// Increment the beat so the last beat is selected
	lBeat++;

	// Convert back to a clock value
	m_pLyricMgr->m_pTimeline->MeasureBeatToClocks( m_pLyricMgr->m_dwGroupBits, 0,
												   lMeasure, lBeat, &mtEndTime );

	// Convert the passed in times to a generic time class
	CMusicTimeConverter cmtBeg(mtBeginTime);
	CMusicTimeConverter cmtEnd(mtEndTime);

	// Create a region that contains the selected time
	CSelectedRegion region(cmtBeg, cmtEnd);

	// Add the region to the list of selected regions
	m_pSelectedRegions->AddRegion(region);

	// Select all items in the list of selected regions
	// This will return true if the selection state of any lyric changed
	return SelectItemsInSelectedRegions();
}
