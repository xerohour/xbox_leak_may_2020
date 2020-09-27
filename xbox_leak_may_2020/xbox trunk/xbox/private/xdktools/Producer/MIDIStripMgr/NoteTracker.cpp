// NoteTracker.cpp : implementation file
//

#include "stdafx.h"
#include "MIDIStripMgr.h"

#include "MIDIMgr.h"
#include "PianoRollScrollBar.h"

#include "NoteTracker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CNoteTracker constructor/ destructor

CNoteTracker::CNoteTracker(CWnd* pWnd, IDMUSProdTimeline* pITimeline, CPianoRollStrip* pPianoRollStrip, short /*nAction*/,
							 CRect* initialRect)
{
	ASSERT( pITimeline != NULL );
	ASSERT( pPianoRollStrip != NULL );

	m_pITimeline = pITimeline;
	m_pITimeline->AddRef();

	m_pPianoRollStrip = pPianoRollStrip;
	m_pPianoRollStrip->AddRef();

	m_pWnd = pWnd;
	
	m_dwScrollTick = 0;
	m_fSetStartPoint = TRUE;

	if (initialRect != NULL) {
		m_rect = *initialRect;
	}
	m_nStyle = resizeInside;
}

// construct a note tracker for the SOLE purpose of
// doing hit testing or drawing
CNoteTracker::CNoteTracker(CRect* initialRect) {
	ASSERT(initialRect != NULL);
	
	m_pITimeline = NULL;
	m_pPianoRollStrip = NULL;
	m_pWnd = NULL;
	m_rect = *initialRect;
	m_nStyle = resizeInside;
}

CNoteTracker::~CNoteTracker()
{
	if( m_pITimeline )
	{
		m_pITimeline->Release();
	}

	if( m_pPianoRollStrip )
	{
		m_pPianoRollStrip->Release();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNoteTracker::DoAction

WORD CNoteTracker::DoAction( void )
{
	CRect rectTracker(m_ptStart, m_ptEnd);
	rectTracker.NormalizeRect();

	return m_pPianoRollStrip->SelectNotesInRect( &rectTracker, NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CNoteTracker::ScrollPianoRoll

void CNoteTracker::ScrollPianoRoll( void )
{
	// Reset m_dwScrollTick to zero
	DWORD dwScrollTick = m_dwScrollTick;
	m_dwScrollTick = 0;

	// Get cursor position
	CPoint point;
	::GetCursorPos( &point );
	m_pWnd->ScreenToClient( &point );

	// Get PianoRoll Strip rectangle
	CRect rectStrip;
	if (!m_pPianoRollStrip->GetStripRect(rectStrip)) {
		return;
	}

	// Get Piano Roll scrollbar values
	int nScrollMin;
	int nScrollMax;
	int nScrollPos;
	CScrollBar* pScrollBar = (CScrollBar *)m_pWnd->GetDlgItem( 103 );	// Timeline HORZ scoll bar
	nScrollPos = pScrollBar->GetScrollPos();
	pScrollBar->GetScrollRange( &nScrollMin, &nScrollMax );

	// Does position of cursor indicate user wants to scroll?
	short nDirection = -1;
	if( point.x > rectStrip.right )
	{
		if( nScrollPos < nScrollMax )
		{
			nDirection = SB_LINERIGHT;
		}
	}
	else if( point.x < rectStrip.left )
	{
		if( nScrollPos > nScrollMin )
		{
			nDirection = SB_LINELEFT;
		}
	}
	if( nDirection == -1 )
	{
		return;
	}

	// We may need to scroll
	DWORD dwTick = GetTickCount();
	if( dwScrollTick )
	{
		// scroll only if the proper amount of time has elapsed
		// since the last scroll
		if( dwTick >= dwScrollTick )
		{
			CSize size(0, 0);

			m_pWnd->SendMessage( WM_HSCROLL,
								 MAKELONG(nDirection, nScrollPos), (LPARAM)pScrollBar->m_hWnd );
			m_pWnd->UpdateWindow();
			CDC* pDC = m_pWnd->GetDC();
			pDC->DrawDragRect( m_rectLast, m_sizeLast, m_rectLast, size );
			m_pWnd->ReleaseDC( pDC );

			m_dwScrollTick = dwTick + 350;	// wait 350 ms
		}
		else
		{
			m_dwScrollTick = dwScrollTick;	// still waiting...
		}
	}
	else
	{
		m_dwScrollTick = dwTick + 200;		// wait 200 ms
	}
}


/////////////////////////////////////////////////////////////////////////////
// CNoteTracker::SetStartEndPoints
//
// Sets the start and end points after restricting them within the strip.
void CNoteTracker::SetStartEndPoints( void )
{
	// Get PianoRoll Strip rectangle
	CRect rectStrip;
	if (!m_pPianoRollStrip->GetStripRect(rectStrip)) {
		return;
	}

	// Get PianoRoll Strip left position
	long lLeftPosition;
	m_pITimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
	m_pITimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );

	// Make sure m_rect.top is within this PianoRollStrip
	if( m_rect.top < rectStrip.top )
	{
		m_rect.top = rectStrip.top;
	}
	if( m_rect.top >= rectStrip.bottom )
	{
		m_rect.top = rectStrip.bottom;
	}

	// Make sure m_rect.bottom is within this PianoRollStrip
	if( m_rect.bottom > rectStrip.bottom )
	{
		m_rect.bottom = rectStrip.bottom;
	}
	if( m_rect.bottom <= rectStrip.top )
	{
		m_rect.bottom = rectStrip.top;
	}
	
	// Make tracker rect relative to beginning of the strip
	CRect rectTracker = m_rect;
	rectTracker.OffsetRect( -rectStrip.left, -rectStrip.top );
	rectTracker.OffsetRect( lLeftPosition, 0 );

	// Get the strip rect relative to its beginning
	CRect rectStripAdjusted = rectStrip;
	rectStripAdjusted.OffsetRect( -rectStrip.left, -rectStrip.top );
	rectStripAdjusted.OffsetRect( lLeftPosition, 0 );
	
	// Adjust leftmost point
	if( rectTracker.left <= rectTracker.right )
	{
		if( rectTracker.left < rectStripAdjusted.left )
		{
			rectTracker.left = rectStripAdjusted.left;
		}
	}
	else
	{
		if( rectTracker.right < rectStripAdjusted.left )
		{
			rectTracker.right = rectStripAdjusted.left;
		}
	}
	
	
	// Store starting point if we haven't set it yet
	if (m_fSetStartPoint)
	{
		m_fSetStartPoint = FALSE;

		m_ptStart = rectTracker.TopLeft();
	}
	
	// Store ending point
	m_ptEnd = rectTracker.BottomRight();

	// Enforce minimum width
	int nNewWidth = m_ptEnd.x - m_ptStart.x;
	int nAbsWidth = m_bAllowInvert ? abs(nNewWidth) : nNewWidth;
	if( nAbsWidth < m_sizeMin.cx )
	{
		if( m_ptStart.x <= m_ptEnd.x )
		{
			if( m_ptEnd.x > (rectStripAdjusted.right - m_sizeMin.cx) )
			{
				m_ptStart.x = m_ptEnd.x - m_sizeMin.cx;
			}
			else
			{
				m_ptEnd.x = m_ptStart.x + m_sizeMin.cx;
			}
		}
		else
		{
			if( m_ptStart.x > (rectStripAdjusted.right - m_sizeMin.cx) )
			{
				m_ptEnd.x = m_ptStart.x - m_sizeMin.cx;
			}
			else
			{
				m_ptStart.x = m_ptEnd.x + m_sizeMin.cx;
			}
		}
	}

	// Enforce minimum height
	int nNewHeight = m_ptEnd.y - m_ptStart.y;
	int nAbsHeight = m_bAllowInvert ? abs(nNewHeight) : nNewHeight;
	if( nAbsHeight < m_sizeMin.cy )
	{
		if( m_ptStart.y <= m_ptEnd.y )
		{
			if( m_ptEnd.y > (rectStripAdjusted.bottom - m_sizeMin.cy) )
			{
				m_ptStart.y = m_ptEnd.y - m_sizeMin.cy;
			}
			else
			{
				m_ptEnd.y = m_ptStart.y + m_sizeMin.cy;
			}
		}
		else
		{
			if( m_ptStart.y > (rectStripAdjusted.bottom - m_sizeMin.cy) )
			{
				m_ptEnd.y = m_ptStart.y - m_sizeMin.cy;
			}
			else
			{
				m_ptStart.y = m_ptEnd.y + m_sizeMin.cy;
			}
		}
	}

	// put the values back into m_rect
	m_rect = CRect(m_ptStart, m_ptEnd);
	m_rect.OffsetRect( rectStrip.left, rectStrip.top );
	m_rect.OffsetRect( -lLeftPosition, 0 );
}


/////////////////////////////////////////////////////////////////////////////
// CNoteTracker::AdjustRect

void CNoteTracker::AdjustRect( int nHandle, RECT* pRect )
{
	CRectTracker::AdjustRect( nHandle, pRect );

	// Scroll if necessary
	ScrollPianoRoll();

	// Set the Start and End points
	SetStartEndPoints();
}


/////////////////////////////////////////////////////////////////////////////
// CNoteTracker::DrawTrackerRect

void CNoteTracker::DrawTrackerRect( LPCRECT lpRect, CWnd* /*pWndClipTo*/, CDC* pDC, CWnd* /*pWnd*/ )
{
	// clip to the strip
	CRect rectStrip;
	if (!m_pPianoRollStrip->GetStripRect(rectStrip)) {
		return;
	}
	
	VARIANT var;
	CRect rectFBar;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if( FAILED ( m_pITimeline->StripGetTimelineProperty( (IDMUSProdStrip *)m_pPianoRollStrip, STP_FBAR_RECT, &var ) ) )
	{
		return;
	}
	rectFBar.InflateRect(1, 1);
	rectStrip.left = rectFBar.right;

	if ( m_bErase == FALSE )
	{
		int iSavedDC = pDC->SaveDC();
		ASSERT( iSavedDC );

		WORD wRefreshUI = DoAction();

		if( iSavedDC )
		{
			pDC->RestoreDC( iSavedDC );
			iSavedDC = 0;
		}

		if( wRefreshUI )
		{
			//CSize size(0, 0);
			iSavedDC = pDC->SaveDC();
			ASSERT( iSavedDC );

			//m_pPianoRollStrip->InvalidatePianoRoll();
			m_pPianoRollStrip->m_pMIDIMgr->RefreshPartDisplay( m_pPianoRollStrip->m_pPartRef->m_pDMPart, ALL_VARIATIONS, FALSE, FALSE );
			//m_pPianoRollStrip->RefreshCurvePropertyPage();

			if( iSavedDC )
			{
				pDC->RestoreDC( iSavedDC );
				iSavedDC = 0;
			}

			m_pWnd->UpdateWindow();
			// redraw the drag rect that was erased during the refresh
			//pDC->DrawDragRect( m_rectLast, m_sizeLast, m_rectLast, size );
			//pDC->Rectangle(m_rectLast);
		}
	}

	CRgn rgn;
	rgn.CreateRectRgn(rectStrip.left, rectStrip.top, rectStrip.right, rectStrip.bottom);
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();
		
	// prepare DC for dragging
	pDC->SelectObject(CBrush::FromHandle((HBRUSH)::GetStockObject(NULL_BRUSH)));
	pDC->SetROP2(R2_XORPEN);
	CPen* pOldPen = NULL;
	CPen dragPen;
	if (dragPen.CreatePen(PS_DOT, 0, RGB(0, 0, 0))) {
		pOldPen = pDC->SelectObject(&dragPen);
	}

	// draw the rect
	pDC->Rectangle( lpRect );

	// cleanup
	if (pOldPen)
	{
		pDC->SelectObject(pOldPen);
		dragPen.DeleteObject();
	}

	//CRectTracker::DrawTrackerRect( lpRect, pWndClipTo, pDC, pWnd );
}
