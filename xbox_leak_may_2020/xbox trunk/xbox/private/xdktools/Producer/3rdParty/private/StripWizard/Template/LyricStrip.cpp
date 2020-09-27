// $$Safe_root$$Strip.cpp : Implementation of C$$Safe_root$$Strip
#include "stdafx.h"
#include "$$Safe_root$$Item.h"
#include "$$Safe_root$$StripMgr.h"
#include "$$Safe_root$$Mgr.h"
#include "PropPageMgr.h"
#include "GroupBitsPPG.h"
#include <RiffStrm.h>
#include "BaseMgr.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This sets the strip's height
#define DEFAULT_STRIP_HEIGHT 20

/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip constructor/destructor

C$$Safe_root$$Strip::C$$Safe_root$$Strip( C$$Safe_root$$Mgr* p$$Safe_root$$Mgr ) : CBaseStrip( p$$Safe_root$$Mgr )
{
	// Validate the pointer to our strip manager
	ASSERT( p$$Safe_root$$Mgr );
	if ( p$$Safe_root$$Mgr == NULL )
	{
		return;
	}

	// Set our pointer to our strip manager
	m_p$$Safe_root$$Mgr = p$$Safe_root$$Mgr;

	// Initialize our clipboard format to 0
	m_cf$$Safe_root$$List = 0;

	// Initialize our state variables to false
	m_fLeftMouseDown = false;

	// Initialize the $$Safe_root$$ to toggle (when CTRL-clicking) to NULL
	m_p$$Safe_root$$ItemToToggle = NULL;
}

C$$Safe_root$$Strip::~C$$Safe_root$$Strip()
{
	// Clear our pointer to our strip manager
	ASSERT( m_p$$Safe_root$$Mgr );
	if ( m_p$$Safe_root$$Mgr )
	{
		m_p$$Safe_root$$Mgr = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// GetNextSelected$$Safe_root$$ - helper method for Draw()

void GetNextSelected$$Safe_root$$( CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list, POSITION &pos$$Safe_root$$ )
{
	// Note that if the item at pos$$Safe_root$$ is selected, this method doesn't do anything useful.
	if( pos$$Safe_root$$ )
	{
		// Save the current position
		POSITION posToSave = pos$$Safe_root$$;

		// Check if we have not run off the end of the list, and if the currently item is unselected
		while( pos$$Safe_root$$ && !list.GetNext( pos$$Safe_root$$ )->m_fSelected )
		{
			// Current item is unselected, save the position of the next item
			posToSave = pos$$Safe_root$$;
		}

		// Check if we found a selected item
		if( posToSave )
		{
			// Save the position of the selected item
			pos$$Safe_root$$ = posToSave;
		}
		// Otherwise both posToSave and pos$$Safe_root$$ are NULL
	}
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::Draw

HRESULT	STDMETHODCALLTYPE C$$Safe_root$$Strip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that our timeline pointer is valid
	if( m_p$$Safe_root$$Mgr == NULL
	||	m_p$$Safe_root$$Mgr->m_pTimeline == NULL )
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
	m_p$$Safe_root$$Mgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &lStartTime );

	// A structure to store the size of each item in
	SIZE sizeText;

	////
	// Draw the unselected $$Safe_root$$s first

	// Iterate through the list of $$Safe_root$$s
	POSITION pos$$Safe_root$$ = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos$$Safe_root$$ )
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetNext( pos$$Safe_root$$ );

		// If this item is selected and we are not using the gutter selection, skip it
		if( !fUseGutterSelectRange && p$$Safe_root$$Item->m_fSelected )
		{
			continue;
		}

		// Convert the measure and beat of each item to a pixel position
		long lPosition;
		m_p$$Safe_root$$Mgr->m_pTimeline->MeasureBeatToPosition( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, p$$Safe_root$$Item->m_lMeasure, p$$Safe_root$$Item->m_lBeat, &lPosition );

		// Offset the position by the horizontal offset,
		// and increment it by one pixel so we don't overwrite the bar or beat line
		lPosition += -lXOffset + 1;

		// Check if the left edge of the $$Safe_root$$ text is beyond the region we're displaying
		if( lPosition > rectClip.right )
		{
			// We've gone beyond the right edge of the clipping region - break out of the loop
			break;
		}

		// Set the left edge of the highlight rectangle to the start of the text, minus the horizontal offset
		rectHighlight.left = lPosition;

		// Check if there is a item after this one
		if (pos$$Safe_root$$) 
		{
			// Get the size of the $$Safe_root$$ text to be written
			::GetTextExtentPoint32( hDC, p$$Safe_root$$Item->m_strText, p$$Safe_root$$Item->m_strText.GetLength(), &sizeText );

			// Get a pointer to the next item
			C$$Safe_root$$Item* pNext$$Safe_root$$ = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetAt( pos$$Safe_root$$ );

			// Get the start position of the next item
			m_p$$Safe_root$$Mgr->m_pTimeline->MeasureBeatToPosition( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, pNext$$Safe_root$$->m_lMeasure, pNext$$Safe_root$$->m_lBeat, &(rectHighlight.right) );

			// Offset this position with lXOffset
			rectHighlight.right -= lXOffset;

			// Check if the current item will run into the next item
			if( (rectHighlight.left + sizeText.cx) > rectHighlight.right )
			{
				// Yes, it will run into the next item - clip it
				::DrawText( hDC, p$$Safe_root$$Item->m_strText, p$$Safe_root$$Item->m_strText.GetLength(), &rectHighlight, DT_LEFT );
			}
			else
			{
				// It won't run into the next item - just draw it
				::TextOut( hDC, lPosition, 0, p$$Safe_root$$Item->m_strText, p$$Safe_root$$Item->m_strText.GetLength() );
			}
		}
		else
		{
			// No more items after this one - just draw it
			::TextOut( hDC, lPosition, 0, p$$Safe_root$$Item->m_strText, p$$Safe_root$$Item->m_strText.GetLength() );
		}
	}

	////
	// Now, draw the selected $$Safe_root$$s

	// Iterate through the list of $$Safe_root$$s, but only if we're not using the gutter selection
	pos$$Safe_root$$ = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetHeadPosition();
	while( !fUseGutterSelectRange && pos$$Safe_root$$ )
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetNext( pos$$Safe_root$$ );

		// If this item is unselected, skip it
		if( !p$$Safe_root$$Item->m_fSelected )
		{
			continue;
		}

		// Convert the measure and beat of each $$Safe_root$$ to a pixel position
		long lPosition;
		m_p$$Safe_root$$Mgr->m_pTimeline->MeasureBeatToPosition( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, p$$Safe_root$$Item->m_lMeasure, p$$Safe_root$$Item->m_lBeat, &lPosition );

		// Offset the position by the horizontal offset,
		// and increment it by one pixel so we don't overwrite the bar or beat line
		lPosition += -lXOffset + 1;

		// Check if the left edge of the $$Safe_root$$ text is beyond the region we're displaying
		if( lPosition > rectClip.right )
		{
			// We've gone beyond the right edge of the clipping region - break out of the loop
			break;
		}

		// Set the left edge of the highlight rectangle to the start of the text, minus the horizontal offset
		rectHighlight.left = lPosition;

		// Get the position of the next selected item (it may be the one at pos$$Safe_root$$
		GetNextSelected$$Safe_root$$( m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s, pos$$Safe_root$$ );

		// Check if there is a selected item after the current one
		if( pos$$Safe_root$$ )
		{
			// Get the size of the text to be written
			::GetTextExtentPoint32( hDC, p$$Safe_root$$Item->m_strText, p$$Safe_root$$Item->m_strText.GetLength(), &sizeText );

			// Get a pointer to the next item
			C$$Safe_root$$Item* pNext$$Safe_root$$ = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetAt( pos$$Safe_root$$ );

			// Get the start position of the next item
			m_p$$Safe_root$$Mgr->m_pTimeline->MeasureBeatToPosition( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, pNext$$Safe_root$$->m_lMeasure, pNext$$Safe_root$$->m_lBeat, &(rectHighlight.right) );

			// Offset this position with lXOffset
			rectHighlight.right -= lXOffset;

			// Check if the current item will run into the next item
			if( (rectHighlight.left + sizeText.cx) > rectHighlight.right )
			{
				// Yes, it will run into the next item - clip it
				::DrawText( hDC, p$$Safe_root$$Item->m_strText, p$$Safe_root$$Item->m_strText.GetLength(), &rectHighlight, DT_LEFT );
			}
			else
			{
				// It won't run into the next item - just draw it
				::TextOut( hDC, lPosition, 0, p$$Safe_root$$Item->m_strText, p$$Safe_root$$Item->m_strText.GetLength() );
			}
		}
		// No more selected items
		else
		{
			// Just draw the $$Safe_root$$
			::TextOut( hDC, lPosition, 0, p$$Safe_root$$Item->m_strText, p$$Safe_root$$Item->m_strText.GetLength() );
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

		// Get the start position of the $$Safe_root$$ list
		pos$$Safe_root$$ = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetHeadPosition();

		// Get the start position of the region list
		POSITION posRegion = m_pSelectedRegions->GetHeadPosition();
		while(posRegion)
		{
			// Get a pointer to the current $$Safe_root$$, but only when not using the gutter selection
			C$$Safe_root$$Item* p$$Safe_root$$Item = NULL;
			if( !fUseGutterSelectRange && pos$$Safe_root$$ )
			{
				// Get the position of the next selected item (it may be the one at pos$$Safe_root$$)
				GetNextSelected$$Safe_root$$( m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s, pos$$Safe_root$$ );

				// Check if we found a selected item
				if( pos$$Safe_root$$ )
				{
					// Get a pointer to the selected item
					p$$Safe_root$$Item = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetAt( pos$$Safe_root$$ );
				}
			}

			// Get a pointer to this region
			CSelectedRegion* psr = m_pSelectedRegions->GetNext(posRegion);

			// Get the start and end positions for this region
			rectHighlight.left = psr->BeginPos(m_p$$Safe_root$$Mgr->m_pTimeline) - lXOffset;
			rectHighlight.right = psr->EndPos(m_p$$Safe_root$$Mgr->m_pTimeline) - lXOffset;

			// Check if we have a pointer to a selected item and if we're not using gutter selection
			if( p$$Safe_root$$Item && !fUseGutterSelectRange )
			{
				// Get the clock position of this item
				long l$$Safe_root$$ClockPos;
				m_p$$Safe_root$$Mgr->m_pTimeline->MeasureBeatToClocks( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0,
					p$$Safe_root$$Item->m_lMeasure, p$$Safe_root$$Item->m_lBeat, &l$$Safe_root$$ClockPos );

				while( pos$$Safe_root$$ && (l$$Safe_root$$ClockPos < psr->End()) )
				{
					// Check if the current item is within this selected region
					if( l$$Safe_root$$ClockPos >= psr->Beg() )
					{
						// Compute the left side of the $$Safe_root$$'s text display
						long l$$Safe_root$$Pos;
						m_p$$Safe_root$$Mgr->m_pTimeline->ClocksToPosition( l$$Safe_root$$ClockPos, &l$$Safe_root$$Pos );

						// Get the size of the text to be written
						::GetTextExtentPoint32( hDC, p$$Safe_root$$Item->m_strText, p$$Safe_root$$Item->m_strText.GetLength(), &sizeText );

						// Add the size of the text and subtract the offset to
						// determine the right side of the $$Safe_root$$'s text display.
						l$$Safe_root$$Pos += sizeText.cx - lXOffset;

						// Move pos$$Safe_root$$ so we check the next item
						m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetNext( pos$$Safe_root$$ );

						// Get the position of the next selected item (it may be the one at pos$$Safe_root$$)
						GetNextSelected$$Safe_root$$( m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s, pos$$Safe_root$$ );

						// Check if we found a selected item
						if( pos$$Safe_root$$ )
						{
							// Get a pointer to the selected item
							p$$Safe_root$$Item = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetAt( pos$$Safe_root$$ );

							// Get the clock position of this $$Safe_root$$
							m_p$$Safe_root$$Mgr->m_pTimeline->MeasureBeatToClocks( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0,
								p$$Safe_root$$Item->m_lMeasure, p$$Safe_root$$Item->m_lBeat, &l$$Safe_root$$ClockPos );

							// Compute the start position of the next selected item
							long lNextStartPos;
							m_p$$Safe_root$$Mgr->m_pTimeline->ClocksToPosition( l$$Safe_root$$ClockPos, &lNextStartPos );

							// Ensure that we only exent the selection region up to the
							// start of the next selected item
							l$$Safe_root$$Pos = min( lNextStartPos, l$$Safe_root$$Pos );
						}

						// Update rectHighlight.right, if necessary
						rectHighlight.right = max( l$$Safe_root$$Pos, rectHighlight.right );
					}
					else
					{
						// Move pos$$Safe_root$$ so we check the next item
						m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetNext( pos$$Safe_root$$ );

						// Get the position of the next selected item (it may be the one at pos$$Safe_root$$)
						GetNextSelected$$Safe_root$$( m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s, pos$$Safe_root$$ );

						// Check if we found a selected item
						if( pos$$Safe_root$$ )
						{
							// Get a pointer to the selected item
							p$$Safe_root$$Item = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetAt( pos$$Safe_root$$ );

							// Get the clock position of this $$Safe_root$$
							m_p$$Safe_root$$Mgr->m_pTimeline->MeasureBeatToClocks( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0,
								p$$Safe_root$$Item->m_lMeasure, p$$Safe_root$$Item->m_lBeat, &l$$Safe_root$$ClockPos );
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
				::InvertRect( hDC, &rectHighlight );

				// Save the new right side boundary
				lRightMostInvert = rectHighlight.right;
			}
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::Get$$Safe_root$$FromMeasureBeat

C$$Safe_root$$Item *C$$Safe_root$$Strip::Get$$Safe_root$$FromMeasureBeat( long lMeasure, long lBeat )
{
	// Iterate through the list
	POSITION pos = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetNext( pos );

		// Check if it is in the measure and beat we're looking for
		if ( p$$Safe_root$$Item->m_lMeasure == lMeasure &&
			 p$$Safe_root$$Item->m_lBeat == lBeat )
		{
			// We found the item to return
			return p$$Safe_root$$Item;
		}
		// Check if we've passed the measure yet
		else if ( p$$Safe_root$$Item->m_lMeasure > lMeasure )
		{
			// We've passed the measure - break out of the loop
			break;
		}
	}

	// Didn't find an item - return NULL
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::Get$$Safe_root$$FromPoint

C$$Safe_root$$Item *C$$Safe_root$$Strip::Get$$Safe_root$$FromPoint( long lPos )
{
	// Validate our timeline pointer
	if( m_p$$Safe_root$$Mgr->m_pTimeline )
	{
		// Convert the position to a measure and beat value
		long lMeasure, lBeat;
		if( SUCCEEDED( m_p$$Safe_root$$Mgr->m_pTimeline->PositionToMeasureBeat( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, lPos, &lMeasure, &lBeat ) ) )
		{
			// Conversion was successful, look for an item on that measure and beat
			return Get$$Safe_root$$FromMeasureBeat( lMeasure, lBeat );
		}
	}

	// Conversion failed (or there is no Timeline) - return NULL
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::OnWMMessage

HRESULT STDMETHODCALLTYPE C$$Safe_root$$Strip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Initialize the return code to SUCCESS
	HRESULT hr = S_OK;

	// Validate our timeline pointer
	if( m_p$$Safe_root$$Mgr->m_pTimeline == NULL )
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
		// Check if the left mouse button is downp$$Safe_root$$
		if(m_fLeftMouseDown)
		{
			// The user moved the mouse while the left mouse button was down -
			// do a drag-drop operation.
			hr = DoDragDrop( m_p$$Safe_root$$Mgr->m_pTimeline, wParam, m_lXPos);

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
					if( m_p$$Safe_root$$ItemToToggle )
					{
						m_pSelectedRegions->ToggleRegion( m_p$$Safe_root$$ItemToToggle->m_lMeasure, m_p$$Safe_root$$ItemToToggle->m_lBeat );
						m_p$$Safe_root$$ItemToToggle = NULL;

						// Update the selection state of the $$Safe_root$$s
						SelectItemsInSelectedRegions();
					}
				}
				// Check if the shift key was not down
				else if( !(wParam & MK_SHIFT) )
				{
					// If an item was clicked on
					if(Get$$Safe_root$$FromPoint( lXPos ))
					{
						// Update the selection regions to include only the selected items
						SelectRegionsFromSelected$$Safe_root$$s();
					}
				}

				// Redraw the strip
				m_p$$Safe_root$$Mgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

				// Switch the property page to the $$Safe_root$$ property page
				m_p$$Safe_root$$Mgr->OnShowProperties();
			}
			else
			{
				// successful drag drop--make sure that only the regions with
				// selected items are selected
				SelectRegionsFromSelected$$Safe_root$$s();
			}

			// Refresh the $$Safe_root$$ property page, if it exists
			if( m_p$$Safe_root$$Mgr->m_pPropPageMgr )
			{
				m_p$$Safe_root$$Mgr->m_pPropPageMgr->RefreshData();
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
			if( m_p$$Safe_root$$ItemToToggle )
			{
				m_pSelectedRegions->ToggleRegion( m_p$$Safe_root$$ItemToToggle->m_lMeasure, m_p$$Safe_root$$ItemToToggle->m_lBeat );
				m_p$$Safe_root$$ItemToToggle = NULL;

				// Select all items that are in the selection regions
				SelectItemsInSelectedRegions();
			}
		}
		// Check if the Shift key was not pressed
		else if( !(wParam & MK_SHIFT) )
		{
			// Look for an item at the position clicked on
			C$$Safe_root$$Item* p$$Safe_root$$ = Get$$Safe_root$$FromPoint( lXPos );
			if(p$$Safe_root$$)
			{
				// Found an item
				// Unselect all items
				m_p$$Safe_root$$Mgr->UnselectAll();

				// Mark the clicked on item as selected
				p$$Safe_root$$->m_fSelected = TRUE;

				// Update the selection regions to include just this selected item
				SelectRegionsFromSelected$$Safe_root$$s();
			}
		}

		// Update the position to inset at
		m_lXPos = lXPos;

		// Ensure all other strips are unselected
		UnselectGutterRange();

		// Redraw ourself
		m_p$$Safe_root$$Mgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

		// Refresh the $$Safe_root$$ property page, if it exists
		if( m_p$$Safe_root$$Mgr->m_pPropPageMgr )
		{
			m_p$$Safe_root$$Mgr->m_pPropPageMgr->RefreshData();
		}
		break;

	case WM_CREATE:
		// Register our clipboard format, if it hasn't been registered yet
		if( m_cf$$Safe_root$$List == 0 )
		{
			m_cf$$Safe_root$$List = RegisterClipboardFormat( CF_$$SAFE_ROOT$$LIST );
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
// C$$Safe_root$$Strip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::Copy

HRESULT C$$Safe_root$$Strip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
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
	if( (m_p$$Safe_root$$Mgr == NULL)
	||	(m_p$$Safe_root$$Mgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cf$$Safe_root$$List == 0 )
	{
		m_cf$$Safe_root$$List = RegisterClipboardFormat( CF_$$SAFE_ROOT$$LIST );
		if( m_cf$$Safe_root$$List == 0 )
		{
			ASSERT(FALSE);
			return E_FAIL;
		}
	}

	// Create an IStream to save the selected $$Safe_root$$s in.
	IStream* pStreamCopy;
	hr = CreateStreamOnHGlobal( NULL, TRUE, &pStreamCopy );

	// Verify that we were able to create a stream
	if( FAILED( hr ))
	{
		return E_OUTOFMEMORY;
	}

	// Get the span of the selected $$Safe_root$$s
	long lStartTime, lEndTime;
	m_pSelectedRegions->GetSpan(lStartTime, lEndTime);

	// Convert the start time to a measure and beat
	long lMeasure, lBeats;
	m_p$$Safe_root$$Mgr->m_pTimeline->ClocksToMeasureBeat( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0,
								lStartTime, &lMeasure, &lBeats );

	// Convert the start time to a number of beats
	MeasureBeatToBeats( m_p$$Safe_root$$Mgr->m_pTimeline, m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, lMeasure, lBeats, lBeats );

	// Convert the end time to a measure and beat
	long lEndBeats;
	m_p$$Safe_root$$Mgr->m_pTimeline->ClocksToMeasureBeat( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0,
								lEndTime, &lMeasure, &lEndBeats );

	// Convert the end time to a number of beats
	MeasureBeatToBeats( m_p$$Safe_root$$Mgr->m_pTimeline, m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, lMeasure, lEndBeats, lEndBeats );

	// Compute the span of the region we're copying
	lEndBeats -= lBeats;

	// Save the number of beats in the region we're copying
	pStreamCopy->Write( &lEndBeats, sizeof(long), NULL );

	// Save the selected $$Safe_root$$s into the stream
	hr = m_p$$Safe_root$$Mgr->SaveSelected$$Safe_root$$s( pStreamCopy, lBeats );

	// Check if the save succeeded
	if( FAILED( hr ))
	{
		// Save failed
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	// The save succeeded - copy the stream to the clipboard
	hr = CopyDataToClipboard( pITimelineDataObject, pStreamCopy, m_cf$$Safe_root$$List, m_p$$Safe_root$$Mgr, this );

	// Release our pointer to the stream
	pStreamCopy->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::PasteAt

HRESULT C$$Safe_root$$Strip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lPasteTime, bool bDropNotPaste, BOOL &fChanged )
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
		if( m_p$$Safe_root$$Mgr->m_pTimeline )
		{
			// Determine the measure and beat we're dropping in
			long lMeasure1, lBeat1;
			if(SUCCEEDED(m_p$$Safe_root$$Mgr->m_pTimeline->ClocksToMeasureBeat( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, lPasteTime, &lMeasure1, &lBeat1 )))
			{
				// Compute the measure and beat the drag started from
				long lMeasure2, lBeat2;
				if(SUCCEEDED(m_p$$Safe_root$$Mgr->m_pTimeline->PositionToMeasureBeat( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, m_lStartDragPosition, &lMeasure2, &lBeat2 )))
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

	// Check if there is a $$Safe_root$$ list avilable for us
	if( pITimelineDataObject->IsClipFormatAvailable( m_cf$$Safe_root$$List ) == S_OK )
	{
		// Try and read the stream that contains the $$Safe_root$$s
		IStream* pIStream;
		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cf$$Safe_root$$List, &pIStream)))
		{
			// If we're pasting, read in the number of beats that this selection covers
			long lBeatsToPaste = -1;
			if( !bDropNotPaste )
			{
				pIStream->Read( &lBeatsToPaste, sizeof(long), NULL );
			}

			// Load the stream into a list of items
			CTypedPtrList<CPtrList, C$$Safe_root$$Item*> list;
			hr = Load$$Safe_root$$List(list, pIStream);

			// Check if the load operation succeeded
			if ( SUCCEEDED(hr) )
			{
				if( m_p$$Safe_root$$Mgr->m_pTimeline )
				{
					// Get the measure and beat of the drop or paste position
					long lMeasurePaste, lBeatPaste;
					if(FAILED(m_p$$Safe_root$$Mgr->m_pTimeline->ClocksToMeasureBeat( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, 
																	max( lPasteTime, 0 ), &lMeasurePaste, &lBeatPaste )))
					{
						hr = E_FAIL;
						goto Leave_1;
					}

					long lBeatStart;
					MeasureBeatToBeats( m_p$$Safe_root$$Mgr->m_pTimeline, m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, lMeasurePaste, lBeatPaste, lBeatStart );

					// The length of the segment, in beats
					long lBeatSegmentLength = LONG_MAX;

					// Check if there are any items to paste
					if( !list.IsEmpty() )
					{
						// Unselect all existing $$Safe_root$$s in this strip so the only selected $$Safe_root$$s are the dropped
						// or pasted ones
						m_p$$Safe_root$$Mgr->UnselectAll();

						// Make sure the last item lands in the last beat or sooner
						// Retrieve the clock length of the segment
						VARIANT varLength;
						if( SUCCEEDED( m_p$$Safe_root$$Mgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
						{
							// Compute the measure and beat length of the segment
							long lMeasure, lBeat;
							if( SUCCEEDED( m_p$$Safe_root$$Mgr->m_pTimeline->ClocksToMeasureBeat( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, 
																			V_I4(&varLength), &lMeasure, &lBeat ) ) )
							{
								// Compute the beat length of the segment
								MeasureBeatToBeats( m_p$$Safe_root$$Mgr->m_pTimeline, m_p$$Safe_root$$Mgr->m_dwGroupBits, 0,
									lMeasure, lBeat, lBeatSegmentLength );

								// Make sure the last item lands in the last beat or sooner
								if( list.GetTail()->m_lMeasure + lBeatStart >= lBeatSegmentLength )
								{
									lBeatStart = lBeatSegmentLength - list.GetTail()->m_lMeasure - 1;
								}
							}
						}

						// Make sure the first item lands in beat 0 or later
						if( list.GetHead()->m_lMeasure + lBeatStart < 0 )
						{
							lBeatStart = -list.GetHead()->m_lMeasure;
						}
					}

					// Check if we're doing a paste
					if(!bDropNotPaste)
					{
						// We're doing a paste, so get the paste type
						TIMELINE_PASTE_TYPE tlPasteType;
						if( FAILED( m_p$$Safe_root$$Mgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
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
							BeatsToMeasureBeat( m_p$$Safe_root$$Mgr->m_pTimeline, m_p$$Safe_root$$Mgr->m_dwGroupBits, 0,
								lBeatStart + lBeatsToPaste, lmEnd, lbEnd );

							// Now, delete any items that fall between the first and last beats we're pasting in
							fChanged = m_p$$Safe_root$$Mgr->DeleteBetweenMeasureBeats(lMeasurePaste, lBeatPaste, lmEnd, lbEnd );
						}
					}

					// Iterate throught the list of $$Safe_root$$s we loaded
					while( !list.IsEmpty() )
					{
						// Remove the head of the list
						C$$Safe_root$$Item* pItem = list.RemoveHead();

						// Check if the item will land before the end of the segment
						if( pItem->m_lMeasure + lBeatStart < lBeatSegmentLength )
						{
							// Convert from a number of beats to a measure and beat value
							// When we saved this to a stream in C$$Safe_root$$Mgr::SaveSelected$$Safe_root$$s, we stored
							// the beat offset into m_dwMeasure.
							BeatsToMeasureBeat( m_p$$Safe_root$$Mgr->m_pTimeline, m_p$$Safe_root$$Mgr->m_dwGroupBits, 0,
								pItem->m_lMeasure + lBeatStart, pItem->m_lMeasure, pItem->m_lBeat );

							// When pasted or dropped, each item is selected
							pItem->m_fSelected = TRUE;

							// This will overwrite any item that already exists on the measure and
							// beat where pItem will be inserted
							m_p$$Safe_root$$Mgr->InsertByAscendingTime( pItem );

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
		// Drag/drop Target and Source are the same $$Safe_root$$ strip
		m_nStripIsDragDropSource = 2;
	}
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::Paste

HRESULT C$$Safe_root$$Strip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

	// Validate our $$Safe_root$$Mgr and Timeline pointers
	if( (m_p$$Safe_root$$Mgr == NULL)
	||	(m_p$$Safe_root$$Mgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get a TimelineDataObject, if we don't already have one
	hr = GetTimelineDataObject( pITimelineDataObject, m_p$$Safe_root$$Mgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Convert the paste position to clocks
	long lClocks;
	if( FAILED( m_p$$Safe_root$$Mgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lClocks) ) )
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
		SelectRegionsFromSelected$$Safe_root$$s();

		// Notify the Segment Designer that we did a paste operation
		m_p$$Safe_root$$Mgr->m_nLastEdit = IDS_PASTE;
		m_p$$Safe_root$$Mgr->OnDataChanged();

		// Redraw our strip
		m_p$$Safe_root$$Mgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

		// Ensure the $$Safe_root$$ property page is visible
		m_p$$Safe_root$$Mgr->OnShowProperties();

		// Refresh the $$Safe_root$$ property page
		if( m_p$$Safe_root$$Mgr->m_pPropPageMgr )
		{
			m_p$$Safe_root$$Mgr->m_pPropPageMgr->RefreshData();
		}
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::Insert

HRESULT C$$Safe_root$$Strip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Double-check that our timeline pointer is valid
	if( (m_p$$Safe_root$$Mgr == NULL)
	||	(m_p$$Safe_root$$Mgr->m_pTimeline == NULL) )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Unselect all items in this strip 
	m_p$$Safe_root$$Mgr->UnselectAll();

	// Unselect all items in other strips
	UnselectGutterRange();

	// Convert the insert position to a measure and beat value
	long	lMeasure, lBeat;
	if( FAILED( m_p$$Safe_root$$Mgr->m_pTimeline->PositionToMeasureBeat( m_p$$Safe_root$$Mgr->m_dwGroupBits, 0, max( 0, m_lXPos ), &lMeasure, &lBeat ) ) )
	{
		return E_UNEXPECTED;
	}

	C$$Safe_root$$Item* p$$Safe_root$$ = new C$$Safe_root$$Item;
	if(p$$Safe_root$$ == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Try and load the default text
	if( !p$$Safe_root$$->m_strText.LoadString( IDS_DEFAULT_$$SAFE_ROOT$$_TEXT ) )
	{
		// Failed to load the text, just use a default english string
		p$$Safe_root$$->m_strText = _T("Empty");
	}

	// Set the measure and beat the item was inserted in
	p$$Safe_root$$->m_lMeasure = lMeasure;
	p$$Safe_root$$->m_lBeat = lBeat;

	// By default, a newly selected item is selected
	p$$Safe_root$$->m_fSelected = TRUE;

	// Insert the item into our list of $$Safe_root$$s, overwriting any existing one
	m_p$$Safe_root$$Mgr->InsertByAscendingTime(p$$Safe_root$$);

	// If it's not already selected, add the beat the item was inserted on
	// to the list of selected regions
	if( !m_pSelectedRegions->Contains( lMeasure, lBeat ) )
	{
		m_pSelectedRegions->AddRegion(m_lXPos);
	}

	// Redraw our strip
	m_p$$Safe_root$$Mgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

	// Show the property sheet
	ShowPropertySheet();

	// Switch to the $$Safe_root$$ property page
	m_p$$Safe_root$$Mgr->OnShowProperties();

	// Refresh the $$Safe_root$$ property page
	if( m_p$$Safe_root$$Mgr->m_pPropPageMgr )
	{
		m_p$$Safe_root$$Mgr->m_pPropPageMgr->RefreshData();
	}

	// Notify the Segment Designer that an insert occurred
	m_p$$Safe_root$$Mgr->m_nLastEdit = IDS_INSERT;
	m_p$$Safe_root$$Mgr->OnDataChanged();

	// Return the success/failure code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::Delete

HRESULT C$$Safe_root$$Strip::Delete( void )
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
	if( (m_p$$Safe_root$$Mgr == NULL)
	||	(m_p$$Safe_root$$Mgr->m_pTimeline == NULL) )
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}

	// Delete all selected items
	m_p$$Safe_root$$Mgr->DeleteSelected$$Safe_root$$s();

	// Clear all selection ranges
	m_pSelectedRegions->Clear();

	// Redraw our strip
	m_p$$Safe_root$$Mgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	// Refresh the $$Safe_root$$ property page, if it exists
	if( m_p$$Safe_root$$Mgr->m_pPropPageMgr )
	{
		m_p$$Safe_root$$Mgr->m_pPropPageMgr->RefreshData();
	}

	// Notify the Segment Designer that a delete operation occurred
	m_p$$Safe_root$$Mgr->m_nLastEdit = IDS_DELETE;
	m_p$$Safe_root$$Mgr->OnDataChanged();

	// Return that the delete succeeded
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::CanPaste

HRESULT C$$Safe_root$$Strip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid timeline pointer
	if( (m_p$$Safe_root$$Mgr == NULL)
	||	(m_p$$Safe_root$$Mgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cf$$Safe_root$$List == 0 )
	{
		m_cf$$Safe_root$$List = RegisterClipboardFormat( CF_$$SAFE_ROOT$$LIST );
		if( m_cf$$Safe_root$$List == 0 )
		{
			return E_FAIL;
		}
	}

	// Get a TimelineDataObject, if we don't already have one
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_p$$Safe_root$$Mgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check the timeline data object for our clipboard format
	hr = pITimelineDataObject->IsClipFormatAvailable( m_cf$$Safe_root$$List );

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
// C$$Safe_root$$Strip::CanInsert

HRESULT C$$Safe_root$$Strip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid Timeline pointer
	if( (m_p$$Safe_root$$Mgr == NULL)
	||	(m_p$$Safe_root$$Mgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get the position to paste at
	long lInsertTime;
	if( FAILED( m_p$$Safe_root$$Mgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lInsertTime) ) )
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
	if( FAILED( m_p$$Safe_root$$Mgr->m_pTimeline->ClocksToPosition( lInsertTime, &lPosition ) ) )
	{
		return E_UNEXPECTED;
	}

	// Check to see if there is an existing item at the insert position
	C$$Safe_root$$Item* p$$Safe_root$$ = Get$$Safe_root$$FromPoint( lPosition );

	// Check if we found an item
	if( p$$Safe_root$$ != NULL )
	{
		// We found an item - return S_FALSE since we can't insert here
		return S_FALSE;
	}

	// Get the length of the segment, in clocks
	VARIANT var;
	if( FAILED( m_p$$Safe_root$$Mgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var) ) )
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
// C$$Safe_root$$Strip::Drop

HRESULT C$$Safe_root$$Strip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
		if( SUCCEEDED( m_p$$Safe_root$$Mgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			// Import the DataObject that was dragged
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				// Compute the time to drop at
				long lPasteTime;
				m_p$$Safe_root$$Mgr->m_pTimeline->PositionToClocks( pt.x, &lPasteTime );

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
						SelectRegionsFromSelected$$Safe_root$$s();

						// Notify the SegmentDesigner that a paste operation occurred
						m_p$$Safe_root$$Mgr->m_nLastEdit = IDS_PASTE;
						m_p$$Safe_root$$Mgr->OnDataChanged();

						// Redraw our strip
						m_p$$Safe_root$$Mgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
						
						// If the property sheet is visible, make it display the $$Safe_root$$ property page
						m_p$$Safe_root$$Mgr->OnShowProperties();

						// Update the $$Safe_root$$ property page, if it exists
						if( m_p$$Safe_root$$Mgr->m_pPropPageMgr )
						{
							m_p$$Safe_root$$Mgr->m_pPropPageMgr->RefreshData();
						}
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
// C$$Safe_root$$Strip::CanPasteFromData

HRESULT C$$Safe_root$$Strip::CanPasteFromData(IDataObject* pIDataObject)
{
	// Check that we're passed a valid IDataObject pointer
	if( pIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Get a Timeline DataObject that encapsulates the pIDataObject
	IDMUSProdTimelineDataObject *pITimelineDataObject = NULL;
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_p$$Safe_root$$Mgr->m_pTimeline, pIDataObject );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check if our clipboard format is available in the data object
	hr = S_FALSE;
	if( pITimelineDataObject->IsClipFormatAvailable( m_cf$$Safe_root$$List ) == S_OK )
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
// C$$Safe_root$$Strip::OnLButtonDown

HRESULT C$$Safe_root$$Strip::OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);

	// Validate our timeline pointer
	if( (m_p$$Safe_root$$Mgr == NULL)
	||	(m_p$$Safe_root$$Mgr->m_pTimeline == NULL) )
	{
		return E_UNEXPECTED;
	}

	// Initialize the item to toggle to NULL
	m_p$$Safe_root$$ItemToToggle = NULL;
	
	// Unselect all items in the other strips
	UnselectGutterRange();

	// Capture mouse so we get the WM_LBUTTONUP message as well.
	// The timeline will release the capture when it receives the
	// WM_LBUTTONUP message.
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_p$$Safe_root$$Mgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Flag that the left mouse button is pressed
	m_fLeftMouseDown = true;

	// Show the $$Safe_root$$ property page
	m_fShowItemProps = true;

	// Check if the shift key is pressed
	if(wParam & MK_SHIFT)
	{
		m_pSelectedRegions->ShiftAddRegion(lXPos);
		SelectItemsInSelectedRegions();
	}
	else
	{
		// See if there is an item under the cursor.
		C$$Safe_root$$Item* p$$Safe_root$$ = Get$$Safe_root$$FromPoint( lXPos );
		if( p$$Safe_root$$ )
		{
			// Found an item under the cursor

			// Check if the control key is down
			if(wParam & MK_CONTROL)
			{
				// Check if the item is not yet selected
				if( !p$$Safe_root$$->m_fSelected )
				{
					// Not yet selected - select the beat just clicked on
					m_pSelectedRegions->AddRegion(lXPos);
				}
				else
				{
					// Set up to unselect this item later, either when we receive a
					// left-button up, or when the user completes a drag-drop operation
					// that does nothing.
					m_p$$Safe_root$$ItemToToggle = p$$Safe_root$$;
				}
			}
			// Check if the item is unselected (the shift key is up)
			else if( !p$$Safe_root$$->m_fSelected )
			{
				// Clear all selection regions
				m_pSelectedRegions->Clear();

				// Select only the beat clicked on
				m_pSelectedRegions->AddRegion(lXPos);
			}
		}
		// Didn't find an item - check if the Ctrl key is down
		else if(wParam & MK_CONTROL)
		{
			// Toggle the selection state in the region under the cursor
			m_pSelectedRegions->ToggleRegion(lXPos);
		}
		// The shift key is up
		else
		{
			// The user left-clicked on blank space without holding the shift or Ctrl keys down

			// Clear all selection regions
			m_pSelectedRegions->Clear();

			// Select only the beat clicked on
			m_pSelectedRegions->AddRegion(lXPos);
		}

		// Ensure all items are now unselected
		SelectItemsInSelectedRegions();
	}

	// Redraw our strip
	m_p$$Safe_root$$Mgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

	// Switch the property page to the $$Safe_root$$ property page
	m_p$$Safe_root$$Mgr->OnShowProperties();

	// Refresh the $$Safe_root$$ property page, if it exists
	if( m_p$$Safe_root$$Mgr->m_pPropPageMgr )
	{
		m_p$$Safe_root$$Mgr->m_pPropPageMgr->RefreshData();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::OnRButtonDown

HRESULT C$$Safe_root$$Strip::OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);
	ASSERT( m_p$$Safe_root$$Mgr->m_pTimeline != NULL );

	// Unselect all items in the other strips
	UnselectGutterRange();

	// Get the item at the mouse click.
	C$$Safe_root$$Item* p$$Safe_root$$ = Get$$Safe_root$$FromPoint( lXPos );

	if( !(wParam & (MK_CONTROL | MK_SHIFT)) )
	{
		// Neither control or shift were pressed

		// Check if an item was clicked on
		if( p$$Safe_root$$ )
		{
			// Check if the item is selected
			if( p$$Safe_root$$->m_fSelected == FALSE )
			{
				// $$Safe_root$$ is not selected, unselect all items
				m_p$$Safe_root$$Mgr->UnselectAll();

				// Select just this item
				p$$Safe_root$$->m_fSelected = TRUE;

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
			if( SUCCEEDED( m_p$$Safe_root$$Mgr->m_pTimeline->PositionToMeasureBeat(m_p$$Safe_root$$Mgr->m_dwGroupBits,
															0,lXPos, &lMeasure, &lBeat) ) )
			{
				// Check if this beat is not already selected
				if( !m_pSelectedRegions->Contains(lMeasure, lBeat))
				{
					// This beat is not selected - unselect all beats
					m_p$$Safe_root$$Mgr->UnselectAll();

					// Now, select just this beat
					m_pSelectedRegions->AddRegion(lXPos);
				}
			}
		}
	}

	// Redraw our strip
	m_p$$Safe_root$$Mgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	// Switch to the $$Safe_root$$ property page
	m_fShowItemProps = true;
	m_p$$Safe_root$$Mgr->OnShowProperties(); 
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::UnselectGutterRange

void C$$Safe_root$$Strip::UnselectGutterRange( void )
{
	// Make sure everything on the timeline is deselected.

	// Flag that we're unselecting all other strips
	m_fSelecting = TRUE;

	// Set the begin and end selection time to 0
	m_p$$Safe_root$$Mgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_p$$Safe_root$$Mgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );

	// Flags that we're no longer unselecting all other strips
	m_fSelecting = FALSE;
}


// IDropSource helpers

/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::CreateDataObject

HRESULT	C$$Safe_root$$Strip::CreateDataObject(IDataObject** ppIDataObject, long lPosition)
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
	if( FAILED( m_p$$Safe_root$$Mgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
	{
		return E_OUTOFMEMORY;
	}


	// Save Selected $$Safe_root$$s into stream
	HRESULT hr = E_FAIL;

	// Allocate a memory stream
	IStream* pIStream;
	if( SUCCEEDED ( m_p$$Safe_root$$Mgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// Get the item at the drag point
		C$$Safe_root$$Item* p$$Safe_root$$AtDragPoint = Get$$Safe_root$$FromPoint( lPosition );

		// mark the $$Safe_root$$s as being dragged: this used later for deleting $$Safe_root$$s in drag move
		m_p$$Safe_root$$Mgr->MarkSelected$$Safe_root$$s(UD_DRAGSELECT);

		// Save the selected items into a stream
		if( SUCCEEDED ( m_p$$Safe_root$$Mgr->SaveSelected$$Safe_root$$s( pIStream, p$$Safe_root$$AtDragPoint ) ) )
		{
			// Add the stream to the Timeline DataObject
			if( SUCCEEDED ( pITimelineDataObject->AddExternalClipFormat( m_cf$$Safe_root$$List, pIStream ) ) )
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
// C$$Safe_root$$Strip::DoDragDrop

HRESULT C$$Safe_root$$Strip::DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos)
{
	// Do a drag'n'drop operation

	// Get the item at the point we're dragging from
	C$$Safe_root$$Item* p$$Safe_root$$ = Get$$Safe_root$$FromPoint( lXPos );

	// Check that we found a item, and that it is selected
	if(!p$$Safe_root$$ || !p$$Safe_root$$->m_fSelected)
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
			// We can always copy $$Safe_root$$s.
			DWORD dwOKDragEffects = DROPEFFECT_COPY;

			// Check if we can cut $$Safe_root$$s
			if(CanCut() == S_OK)
			{
				// If we can Cut(), allow the user to move the $$Safe_root$$s as well.
				dwOKDragEffects |= DROPEFFECT_MOVE;
			}

			// Do the Drag/Drop.

			// Initialize our DragDropSource flag to 1 (means we are the source of a drag-drop)
			m_nStripIsDragDropSource = 1;

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
					// We moved some $$Safe_root$$s - delete the $$Safe_root$$s we marked earlier
					m_p$$Safe_root$$Mgr->DeleteMarked(UD_DRAGSELECT);
				}

				// If the effect wasn't 'None', set the drop flag to true
				if(dwEffect != DROPEFFECT_NONE)
				{
					fDrop = true;
				}
				break;
			default:
				// User cancelled the operation, or dropped the $$Safe_root$$s somewhere they didn't belong
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
						m_p$$Safe_root$$Mgr->m_nLastEdit = IDS_UNDO_MOVE;
					}
					else
					{
						// We did a copy - use the 'Insert' undo text
						m_p$$Safe_root$$Mgr->m_nLastEdit = IDS_INSERT;
					}

					// Notify the Segment Designer that we changed
					m_p$$Safe_root$$Mgr->OnDataChanged();

					// Update the selection regions to include the selected items
					SelectRegionsFromSelected$$Safe_root$$s();

					// If visible, switch the property sheet to the $$Safe_root$$ property page
					m_p$$Safe_root$$Mgr->OnShowProperties();

					// If it exists, refresh the $$Safe_root$$ property page
					if( m_p$$Safe_root$$Mgr->m_pPropPageMgr )
					{
						m_p$$Safe_root$$Mgr->m_pPropPageMgr->RefreshData();
					}
				}
				else
				{
					// Target strip is different from source strip
					if( dwEffect & DROPEFFECT_MOVE )
					{
						// We did a move operation to another strip - use the 'Delete' undo text
						m_p$$Safe_root$$Mgr->m_nLastEdit = IDS_DELETE;

						// Notify the Segment Designer that we changed
						m_p$$Safe_root$$Mgr->OnDataChanged();
					}
					else
					{
						// Unselect all items, since the user did a copy operation
						m_p$$Safe_root$$Mgr->UnselectAll();
					}
				}

				// Refresh our strip display
				m_p$$Safe_root$$Mgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

				// Return that we actually did a drop
				hr = S_OK;
			}
		}

		// Release the IDropSource interface
		pIDropSource->Release();

		// Clear the DragSelect flag from all items
		m_p$$Safe_root$$Mgr->UnMark$$Safe_root$$s(UD_DRAGSELECT); 
	}
	else
	{
		// Couldn't query for IDropSource interface - return failure code
		hr = E_FAIL;
	}

	// Clear the DragDropSource flag
	m_nStripIsDragDropSource = 0;

	// Return a success or failure code
	return hr;
}


////////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::SelectItemsInSelectedRegions

bool C$$Safe_root$$Strip::SelectItemsInSelectedRegions()
{
	// Flag set to true if anything changes
	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetNext( pos );

		// Check if this item is within a selection region
		if(m_pSelectedRegions->Contains(p$$Safe_root$$Item->m_lMeasure, p$$Safe_root$$Item->m_lBeat))
		{
			// Check if the item is not selected
			if( !p$$Safe_root$$Item->m_fSelected )
			{
				// $$Safe_root$$ isn't yet selected - select it and set fChanged to true
				p$$Safe_root$$Item->m_fSelected = TRUE;
				fChanged = true;
			}
		}
		else
		{
			// Check if the item is selected
			if( p$$Safe_root$$Item->m_fSelected )
			{
				// $$Safe_root$$ is selected - deselect it and set fChagned to true
				p$$Safe_root$$Item->m_fSelected = FALSE;
				fChanged = true;
			}
		}
	}

	// return whether or not we changed the selection state of any items
	return fChanged;
}


////////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Strip::SelectRegionsFromSelected$$Safe_root$$s

void C$$Safe_root$$Strip::SelectRegionsFromSelected$$Safe_root$$s()
{
	// Clear the list of selected regions
	m_pSelectedRegions->Clear();

	// Iterate through the list of items
	POSITION pos = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_p$$Safe_root$$Mgr->m_lst$$Safe_root$$s.GetNext( pos );

		// Check if the item is selected
		if(p$$Safe_root$$Item->m_fSelected)
		{
			// Add the beat the item is in to the list of selected regions.
			CListSelectedRegion_AddRegion(*m_pSelectedRegions, *p$$Safe_root$$Item);
		}
	}
}
