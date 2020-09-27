// Splitter.cpp : implementation file
//

#include "stdafx.h"

#include "StyleDesignerDLL.h"
#include "Style.h"
#include "Splitter.h"
#include "StyleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//*****************************************************************************
//***** CSplitter
//*****************************************************************************

BEGIN_MESSAGE_MAP(CSplitter, CWnd)
	//{{AFX_MSG_MAP(CSplitter)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

//*****************************************************************************
CSplitter::CSplitter() :
	m_rcTrack(0, 0, 0, 0),
	m_fTracking(FALSE),
	m_fVisible(FALSE),
	m_nTopBorder(0),
	m_pParent(NULL)
{
}

//*****************************************************************************
CSplitter::~CSplitter()
{
}

//*****************************************************************************
BOOL CSplitter::Create( CWnd *pParent, RECT *rect )
{
	UNREFERENCED_PARAMETER(rect);

	// Save our parent
	m_pParent = pParent;

	// Create our splitter class
	WNDCLASS wc;

	ZeroMemory( &wc, sizeof(wc) );
	wc.lpszClassName = "JzStyleSplitter";
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_PARENTDC;
	wc.lpfnWndProc   = ::DefWindowProc;
	wc.hInstance     = theApp.m_hInstance;
	wc.hCursor       = ::LoadCursor( theApp.m_hInstance, MAKEINTRESOURCE(IDC_SPLITTER) );
	wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );

	// Register our splitter class
	AfxRegisterClass( &wc );

	// Create our splitter
	return CWnd::Create( wc.lpszClassName, "", WS_VISIBLE | WS_CHILD, 
						 CRect(0, 0, 0, 0), pParent, IDC_SPLITTER );
}

//*****************************************************************************
void CSplitter::SetTopBorder( int nTopBorder )
{
	m_nTopBorder = nTopBorder;
}

//*****************************************************************************
void CSplitter::OnPaint()
{
	CPaintDC dc( this );
	CBrush brush;

	if( brush.CreateSolidBrush( GetSysColor(COLOR_BTNFACE) ) )
	{
		CRect rect;
		GetClientRect( &rect );

		CBrush* pOldBrush = dc.SelectObject( &brush );
		dc.FillRect( &rect, &brush );
		dc.SelectObject( pOldBrush );
	}		
}

//*****************************************************************************
void CSplitter::OnLButtonDown( UINT /*nFlags*/, CPoint /*point*/ )
{
	ASSERT( m_pParent != NULL );

	// Capture the mouse
	SetCapture();

	// Set our initial splitter position in our parent's client coordinates
	GetWindowRect( &m_rcTrack );
	m_pParent->ScreenToClient( &m_rcTrack );

	// Set our tracking flag
	m_fTracking = TRUE;

	// Draw our initial tracker
	InvertTracker();
}

//*****************************************************************************
void CSplitter::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	ASSERT( m_pParent != NULL );

	BOOL fWasTracking = m_fTracking;

	OnEndCapture();

	if( fWasTracking )
	{
		CRect rect;
		m_pParent->GetClientRect( &rect );
		static_cast<CStyleDlg*>(m_pParent)->EndTrack( m_rcTrack.top );
	}
}

//*****************************************************************************
void CSplitter::OnMouseMove( UINT /*nFlags*/, CPoint point )
{
	if( m_fTracking )
	{
		ASSERT( m_pParent != NULL );

		// Convert point to client coordinates of our parent window
		ClientToScreen( &point );
		m_pParent->ScreenToClient( &point );

		// Get the client are of our parent
		CRect rcClient;
		m_pParent->GetClientRect( &rcClient );

		// Force the point to be in our client area
		int nDiff = m_nTopBorder + SPLITTER_HEIGHT + 1;
		if( (point.y - nDiff) < rcClient.top )
		{
			point.y = rcClient.top + nDiff;
		}
		nDiff = SPLITTER_HEIGHT + STYLE_DLG_BORDER + 4;
		if( (point.y + nDiff) > rcClient.bottom)
		{
			point.y = rcClient.bottom - nDiff;
		}

		// If the point has changed since the last mouse move, then update change
		if( m_rcTrack.top != point.y )
		{
			InvertTracker();
			m_rcTrack.top  = point.y;
			m_rcTrack.bottom = point.y + SPLITTER_HEIGHT;
			InvertTracker();
		}
	}
}

//*****************************************************************************
void CSplitter::OnCancelMode()
{
	OnEndCapture();
	CWnd::OnCancelMode();
}

//*****************************************************************************
void CSplitter::OnCaptureChanged( CWnd *pWnd )
{
	OnEndCapture();
	CWnd::OnCaptureChanged( pWnd );
}

//*****************************************************************************
void CSplitter::OnEndCapture()
{
	if( m_fVisible )
	{
		InvertTracker();
	}

	if( m_fTracking )
	{
		ReleaseCapture();
		m_fTracking = FALSE;
	}
}

//*****************************************************************************
void CSplitter::InvertTracker()
{
	ASSERT( m_pParent != NULL );

	// Get the DC of our parent window
	CDC *pDC = GetDC();
	ASSERT( pDC != NULL );

	// Create our inverted brush pattern 
	CBrush *pBrush = CDC::GetHalftoneBrush();

	// Select the brush into our DC  
	HBRUSH hBrushStock = NULL;
	if( pBrush != NULL )
	{
		hBrushStock = (HBRUSH)SelectObject( pDC->m_hDC, pBrush->m_hObject );
	}

	// Convert top
	CRect rect( m_rcTrack );

	m_pParent->ClientToScreen( &rect );
	ScreenToClient( &rect );

	// Draw our tracking line
	pDC->PatBlt( m_rcTrack.left, rect.top,
				 m_rcTrack.Width() - (STYLE_DLG_BORDER << 1), m_rcTrack.Height() + 1,
				 PATINVERT );

	// Free our brush and DC
	if( hBrushStock != NULL )
	{
		SelectObject( pDC->m_hDC, hBrushStock );
	}

	ReleaseDC( pDC );

	// Toggle visible flag
	m_fVisible = !m_fVisible;
}

//*****************************************************************************
BOOL CSplitter::OnEraseBkgnd( CDC* pDC ) 
{
	UNREFERENCED_PARAMETER(pDC);

	return FALSE;
}
