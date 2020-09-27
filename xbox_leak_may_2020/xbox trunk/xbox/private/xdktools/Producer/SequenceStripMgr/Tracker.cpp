// Tracker.cpp : implementation file
//

#include "stdafx.h"
#include "SequenceStripMgr.h"

#include "SequenceMgr.h"
#include "CurveStrip.h"

#include "Tracker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCurveTracker constructor/ destructor

CCurveTracker::CCurveTracker(CWnd* pWnd, IDMUSProdTimeline* pITimeline, CCurveStrip* pCurveStrip, short nAction,
							 CRect* initialRect)
{
	ASSERT( pITimeline != NULL );
	ASSERT( pCurveStrip != NULL );
	ASSERT( pCurveStrip->m_pSequenceStrip != NULL );

	m_pITimeline = pITimeline;
	m_pITimeline->AddRef();

	m_pCurveStrip = pCurveStrip;
	m_pCurveStrip->AddRef();

	m_pWnd = pWnd;
	m_nAction = nAction;

	m_lPartClockLength = pCurveStrip->m_pSequenceStrip->m_mtLength;
	
	if (m_nAction == CTRK_DRAGGING) {
		// a little trick to avoid resizing when not intended...
		m_nHandleSize = 0;
	}

	m_pDMCurve = NULL;
	m_pCurveList = NULL;
	m_nXOffset = 0;

	m_dwScrollTick = 0;
	m_fSetStartPoint = TRUE;

	if (initialRect != NULL) {
		m_rect = *initialRect;
	}
	m_nStyle = resizeInside;
	m_fScrolledPianoRoll = FALSE;

	m_sizeMin.cx = 0;
	m_sizeMin.cy = 0;
}

// construct a curve tracker for the SOLE purpose of
// doing hit testing or drawing
CCurveTracker::CCurveTracker(CRect* initialRect)
{
	ASSERT(initialRect != NULL);
	
	m_pITimeline = NULL;
	m_pCurveStrip = NULL;
	m_pWnd = NULL;
	m_nAction = CTRK_NOTHING;
	m_lPartClockLength = 0;
	m_pDMCurve = NULL;
	m_pCurveList = NULL;
	m_nXOffset = 0;
	m_rect = *initialRect;
	m_nStyle = resizeInside;
	m_fScrolledPianoRoll = FALSE;
}

CCurveTracker::~CCurveTracker()
{
	if( m_pITimeline )
	{
		m_pITimeline->Release();
	}

	if( m_pCurveStrip )
	{
		m_pCurveStrip->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveTracker::SetCurve

void CCurveTracker::SetCurve( CCurveItem* pDMCurve )
{
	ASSERT( pDMCurve != NULL );
	m_pDMCurve = pDMCurve;
}

/////////////////////////////////////////////////////////////////////////////
// CCurveTracker::SetCurveList
//
void CCurveTracker::SetCurveList(CTypedPtrList<CPtrList, CCurveItem*>* pCurveList)
{
	ASSERT(pCurveList != NULL);
	m_pCurveList = pCurveList;
}

/////////////////////////////////////////////////////////////////////////////
// CCurveTracker::SetXOffset
//
void CCurveTracker::SetXOffset(int nXOffset)
{
	ASSERT(nXOffset >= 0);
	m_nXOffset = nXOffset;
}

/////////////////////////////////////////////////////////////////////////////
// CCurveTracker::DoAction

WORD CCurveTracker::DoAction( void )
{
	CRect rectTracker(m_ptStart, m_ptEnd);
	rectTracker.NormalizeRect();

	WORD wRefreshUI = FALSE;

	switch( m_nAction )
	{
		case CTRK_SELECTING:
			wRefreshUI = m_pCurveStrip->SelectCurvesInRect( &rectTracker );
			break;

		case CTRK_INSERTING:
		{
			CPoint ptLeft;
			CPoint ptRight;

			if( m_ptStart.x < m_ptEnd.x )
			{
				ptLeft.x = m_ptStart.x;
				ptLeft.y = m_ptStart.y;
				ptRight.x = m_ptEnd.x;
				ptRight.y = m_ptEnd.y;
			}
			else
			{
				ptLeft.x = m_ptEnd.x;
				ptLeft.y = m_ptEnd.y;
				ptRight.x = m_ptStart.x;
				ptRight.y = m_ptStart.y;
			}

			m_pCurveStrip->OnUpdateInsertCurveValue( ptLeft, ptRight );
			break;
		}

		case CTRK_DRAGGING:
		case CTRK_DRAGGING_EDGE:
			m_pCurveStrip->OnUpdateDragCurveValue( *this, m_nAction );
			break;

	}

	return wRefreshUI;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveTracker::ScrollPianoRoll

void CCurveTracker::ScrollPianoRoll( void )
{
	// Reset m_dwScrollTick to zero
	DWORD dwScrollTick = m_dwScrollTick;
	m_dwScrollTick = 0;

	// Get cursor position
	CPoint point;
	::GetCursorPos( &point );
	m_pWnd->ScreenToClient( &point );

	// Get Curve Strip rectangle
	CRect rectStrip;
	if( !m_pCurveStrip->GetStripRect( rectStrip ) )
	{
		return;
	}
	
	// Get Curve Strip max right
	long lMaxXPosition;
	long lLeftPosition;
	CRect rectFBar;
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if( FAILED ( m_pITimeline->StripGetTimelineProperty( (IDMUSProdStrip *)m_pCurveStrip, STP_FBAR_RECT, &var ) ) )
	{
		return;
	}
	m_pITimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
	m_pITimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );
	m_pITimeline->ClocksToPosition( m_lPartClockLength, &lMaxXPosition );
	lMaxXPosition += rectFBar.right - 1;
	lMaxXPosition -= lLeftPosition;

	// Exit if point.x is already at end of Curve Strip
	if( m_nAction != CTRK_SELECTING )
	{
		if( point.x >= lMaxXPosition )
		{
			return;
		}
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
			m_pWnd->SendMessage( WM_HSCROLL,
								 MAKELONG(nDirection, nScrollPos), (LPARAM)pScrollBar->m_hWnd );
			m_pWnd->UpdateWindow();
			m_fScrolledPianoRoll = TRUE;

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
// CCurveTracker::SetStartEndPoints
//
// Sets the start and end points after restricting them within the strip.
void CCurveTracker::SetStartEndPoints( int nHandle )
{
	// Get Curve Strip rectangle
	CRect rectStrip;
	if( !m_pCurveStrip->GetStripRect(rectStrip) )
	{
		return;
	}

	// Get Curve Strip left position
	long lLeftPosition;
	m_pITimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
	m_pITimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );

	// Make sure m_rect.top is within this CurveStrip
	if( m_rect.top < rectStrip.top )
	{
		m_rect.top = rectStrip.top;
	}
	if( m_rect.top >= rectStrip.bottom )
	{
		m_rect.top = rectStrip.bottom;
	}

	// Make sure m_rect.bottom is within this CurveStrip
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
	if( m_nAction == CTRK_DRAGGING_EDGE )
	{
		if( nHandle == hitTopLeft
		||  nHandle == hitBottomLeft
		||  nHandle == hitLeft )
		{
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
		}
	}
	
	bool bKeepLeft = false;
	bool bKeepRight = false;

	
	if (!m_fSetStartPoint && (m_nAction == CTRK_DRAGGING_EDGE)) {
		switch (nHandle) {
		case hitTopLeft:
		case hitBottomLeft:
		case hitLeft:
			bKeepRight = true;
			break;
		case hitTopRight:
		case hitBottomRight:
		case hitRight:
			bKeepLeft = true;
			break;

		}
	}

	// Store starting point if we haven't set it yet
	if (m_fSetStartPoint || m_nAction == CTRK_DRAGGING_EDGE)
	{
		m_fSetStartPoint = FALSE;

		if (bKeepLeft) {
			m_ptStart.y = rectTracker.top;
		}
		else
		{
			m_ptStart = rectTracker.TopLeft();
		}
	}

	// Store ending point
	if (bKeepRight) {
		m_ptEnd.y = rectTracker.bottom;
	}
	else {
		m_ptEnd = rectTracker.BottomRight();
	}

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
// CCurveTracker::LimitRects
//
// Used when m_nAction = CTRK_DRAGGING
// Limits m_rect so that all selected curves will be within the boundaries
// of the strip.
void CCurveTracker::LimitRects() 
{
	// Get Curve Strip rectangle
	CRect rectStrip;
	if (!m_pCurveStrip->GetStripRect(rectStrip)) {
		return;
	}
	
	// Get Curve Strip left position
	long lLeftPosition;
	m_pITimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
	m_pITimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );
	long lOurLeftPosition = 0;
	if( m_pCurveStrip->m_pSequenceStrip->m_fPickupBar )
	{
		DMUS_TIMESIGNATURE dmTimeSig;
		GetTimeSig( 0, &dmTimeSig, m_pITimeline, m_pCurveStrip->m_pSequenceMgr->m_dwGroupBits );

		lOurLeftPosition = -dmTimeSig.bBeatsPerMeasure * ((DMUS_PPQ * 4) / dmTimeSig.bBeat);
		m_pITimeline->ClocksToPosition( lOurLeftPosition, &lOurLeftPosition );
	}
	if( lLeftPosition > 0 )
	{
		rectStrip.left -= lLeftPosition;
	}
	else if( lLeftPosition < lOurLeftPosition )
	{
		rectStrip.left += lOurLeftPosition - lLeftPosition;
	}

	// Get Curve Strip max right
	long lMaxPosition;
	m_pITimeline->ClocksToPosition( m_lPartClockLength, &lMaxPosition );
	
	CRect rectFBar;
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if( FAILED ( m_pITimeline->StripGetTimelineProperty( (IDMUSProdStrip *)m_pCurveStrip, STP_FBAR_RECT, &var ) ) )
	{
		return;
	}
	rectStrip.right = lMaxPosition + rectFBar.right - 1;
	rectStrip.right -= lLeftPosition;

	// adjust tracker rect
	CRect rectTracker = m_rect;
	rectTracker.OffsetRect( -m_nXOffset, 0 );

	// make a rect that includes all the curve rects
	CRect rectAllCurves = m_pDMCurve->m_rectFrame;
	rectAllCurves.OffsetRect(rectStrip.TopLeft());
	if( lLeftPosition < 0 )
	{
		if( lLeftPosition < lOurLeftPosition )
		{
			rectAllCurves.left -= lOurLeftPosition;
			rectAllCurves.right -= lOurLeftPosition;
		}
		else
		{
			rectAllCurves.left -= lLeftPosition;
			rectAllCurves.right -= lLeftPosition;
		}
	}
	CSize curveOffset =  rectTracker.CenterPoint() - rectAllCurves.CenterPoint();
	rectAllCurves.right = rectAllCurves.left;
	
	// iterate through the list of selected curves and update curveRect
	POSITION listPos;
	listPos = m_pCurveList->GetHeadPosition();
	while (listPos != NULL) {
		CCurveItem* pCurve = m_pCurveList->GetNext(listPos);
		
		CRect newRect = pCurve->m_rectFrame;
		newRect.OffsetRect(rectStrip.TopLeft());
		
		rectAllCurves.left = min(rectAllCurves.left, newRect.left);
		rectAllCurves.right = max(rectAllCurves.right, newRect.left);
		rectAllCurves.top = min(rectAllCurves.top, newRect.top);
		rectAllCurves.bottom = max(rectAllCurves.bottom, newRect.bottom);
	}
	
	rectAllCurves.OffsetRect(curveOffset);
	
	// figure out how much we are going to have to offset m_rect to make it
	// within the strip.
	CSize offset(0, 0);

	// make sure curveRect is within the bounds of the strip
	if (rectAllCurves.top < rectStrip.top) {
		offset.cy = rectStrip.top - rectAllCurves.top;
	}
	else if (rectAllCurves.bottom > rectStrip.bottom) {
		offset.cy = rectStrip.bottom - rectAllCurves.bottom;
	}
	if (rectAllCurves.left < rectStrip.left) {
		offset.cx = rectStrip.left - rectAllCurves.left;
	}
	else if (rectAllCurves.right > rectStrip.right) {
		offset.cx = rectStrip.right - rectAllCurves.right;
	}
	m_rect.OffsetRect(offset);
	m_offset = curveOffset + offset;
}

/////////////////////////////////////////////////////////////////////////////
// CCurveTracker::AdjustRect

void CCurveTracker::AdjustRect( int nHandle, RECT* pRect )
{
	CRectTracker::AdjustRect( nHandle, pRect );

	// Scroll if necessary
	ScrollPianoRoll();

	if (m_nAction == CTRK_DRAGGING) {
		LimitRects();
	}
	else {
		// Set the Start and End points
		SetStartEndPoints(nHandle);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveTracker::DrawTrackerRect

void CCurveTracker::DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd )
{
	UNREFERENCED_PARAMETER(pWnd);
	UNREFERENCED_PARAMETER(pWndClipTo);

	// Do not draw if we just scrolled the PianoRoll
	if( m_fScrolledPianoRoll )
	{
		m_fScrolledPianoRoll = FALSE;
		return;
	}

	// clip to the strip
	CRect rectStrip;
	if( !m_pCurveStrip->GetStripRect(rectStrip) )
	{
		return;
	}
	
	VARIANT var;
	CRect rectFBar;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if( FAILED ( m_pITimeline->StripGetTimelineProperty( (IDMUSProdStrip *)m_pCurveStrip, STP_FBAR_RECT, &var ) ) )
	{
		return;
	}
	rectFBar.InflateRect(1, 1);
	rectStrip.left = rectFBar.right;
	CRgn rgn;
	rgn.CreateRectRgn(rectStrip.left, rectStrip.top, rectStrip.right, rectStrip.bottom);
	pDC->SelectClipRgn(&rgn);
		
	// prepare DC for dragging
	pDC->SelectObject(CBrush::FromHandle((HBRUSH)::GetStockObject(NULL_BRUSH)));
	pDC->SetROP2(R2_XORPEN);
	CPen* pOldPen = NULL;
	CPen dragPen;
	if (dragPen.CreatePen(PS_DOT, 0, RGB(0, 0, 0))) {
		pOldPen = pDC->SelectObject(&dragPen);
	}

	if ( m_bErase == FALSE )
	{
		WORD wRefreshUI = DoAction();

		if( wRefreshUI )
		{
			m_pCurveStrip->RefreshCurveStrips();
			m_pCurveStrip->RefreshCurvePropertyPage();
			m_pWnd->UpdateWindow();
		}
	}

	
	if (m_nAction == CTRK_DRAGGING)
	{
		CRect rectFrame;

		// draw the other rects being dragged
		rectFrame = m_pDMCurve->m_rectFrame;
		rectFrame.OffsetRect( m_nXOffset, 0 );
		CSize offset = rectFrame.CenterPoint() - CRect(lpRect).CenterPoint();
		
		// iterate through the list of selected curves
		POSITION listPos;
		listPos = m_pCurveList->GetHeadPosition();
		while (listPos != NULL)
		{
			CCurveItem* pCurve = m_pCurveList->GetNext(listPos);
			rectFrame = pCurve->m_rectFrame;
			rectFrame.OffsetRect( m_nXOffset, 0 );
			pDC->Rectangle(rectFrame - offset);
		}
	}

	// draw the rect 
	pDC->Rectangle( lpRect );

	// cleanup
	if (pOldPen) {
		pDC->SelectObject(pOldPen);
		dragPen.DeleteObject();
	}
}
