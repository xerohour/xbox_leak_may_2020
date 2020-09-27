// VSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "VSplitter.h"
#include "resource.h"
#include "XboxAddinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*****************************************************************************
//***** CVSplitter
//*****************************************************************************

BEGIN_MESSAGE_MAP(CVSplitter, CWnd)
	//{{AFX_MSG_MAP(CVSplitter)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

//*****************************************************************************
CVSplitter::CVSplitter() :
	m_rcTrack(0, 0, 0, 0),
	m_fTracking(FALSE),
	m_fVisible(FALSE),
	m_pParent(NULL),
	m_nWidth(0),
	m_rcFirstPane(0, 0, 0, 0),
	m_rcSecondPane(0, 0, 0, 0),
	m_pFirstPane(NULL),
	m_pSecondPane(NULL)
{
}

//*****************************************************************************
CVSplitter::~CVSplitter()
{
}

//*****************************************************************************
BOOL CVSplitter::Create( CWnd *pParent, RECT *rect )
{
	// Save our parent
	m_pParent = pParent;

	if( rect )
	{
		RECT tempRect;
		m_pParent->GetWindowRect( &tempRect );
				
		m_rcTrack = *rect;
		m_rcSecondPane = m_rcFirstPane = m_rcTrack;

		m_rcFirstPane.left = 0;
		m_rcFirstPane.right = m_rcTrack.left-1;
		m_rcSecondPane.left = m_rcTrack.right+1;
		m_rcSecondPane.right = tempRect.right;

		m_rcFirstPane.top = 0;
		m_rcSecondPane.top = 0;

		m_nWidth = rect->right-rect->left;
		ASSERT( m_nWidth > 0 );
	}

	// Create our splitter class
	WNDCLASS wc;

	ZeroMemory( &wc, sizeof(wc) );
	wc.lpszClassName = "DittoVSplitterClass";
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = ::DefWindowProc;
	wc.hInstance     = AfxGetInstanceHandle();
	wc.hCursor       = LoadCursor( NULL, IDC_SIZEWE );
	wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );

	// Register our splitter class
	AfxRegisterClass( &wc );

	// Create our splitter
	return CWnd::Create( wc.lpszClassName, "", WS_VISIBLE | WS_CHILD, 
						 m_rcTrack, pParent, IDC_VSPLITTER );
}

//*****************************************************************************
void CVSplitter::OnPaint()
{
	RECT rc;

	// Make sure Splitter bar and panes are updated when
	// the parent resizes.
	m_pParent->GetClientRect( &rc );
	m_rcTrack.bottom = rc.bottom;

	MoveWindow( &m_rcTrack, FALSE );

	// Resize First Pane
	m_rcFirstPane.right = m_rcTrack.left;
	m_rcFirstPane.bottom = m_rcTrack.bottom;

	// Resize Second Pane
	m_rcSecondPane.left = m_rcTrack.right;
	m_rcSecondPane.right = rc.right;
	m_rcSecondPane.bottom = m_rcTrack.bottom;

	// Resize First Pane
	m_pFirstPane->MoveWindow( &m_rcFirstPane );

	// Resize Second Pane Window
	m_pSecondPane->MoveWindow( &m_rcSecondPane );

	// Paint Scroll Bar
	CPaintDC dc( this );
	GetClientRect( &rc );
	dc.Draw3dRect( &rc, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
}

//*****************************************************************************
void CVSplitter::OnLButtonDown( UINT /*nFlags*/, CPoint /*point*/ )
{
	ASSERT( m_pParent != NULL );

	// Capture the mouse
	SetCapture();

	// Set our initial splitter position in our main frame's client coordinates
	GetWindowRect( &m_rcTrack );
	m_pParent->ScreenToClient( &m_rcTrack );
	//m_rcTrack.top++; m_rcTrack.bottom--; m_rcTrack.right--;

	// Set our tracking flag
	m_fTracking = TRUE;

	// Draw our initial tracker
	InvertTracker();
}

//*****************************************************************************
void CVSplitter::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	ASSERT( m_pParent != NULL );
	BOOL fWasTracking = m_fTracking;

	OnEndCapture();
	
	if(fWasTracking)
	{
		RECT rect;
		m_pParent->GetClientRect( &rect );
		static_cast<CXboxAddinDlg*>(m_pParent)->EndTrack( m_rcTrack.right );

		MoveWindow(	&m_rcTrack );
		OnPaint();

		m_pFirstPane->InvalidateRect( NULL );
		m_pSecondPane->InvalidateRect( NULL );
	}
}

//*****************************************************************************
void CVSplitter::OnMouseMove( UINT /*nFlags*/, CPoint point )
{
	if( m_fTracking )
	{
		// Convert our point to client coordinates of our frame
		ClientToScreen( &point );
		m_pParent->ScreenToClient( &point );

		// Get the client are of our frame
		CRect rcClient;
		m_pParent->GetClientRect( &rcClient );

		// Force the point to be in our client area
		if( (point.x + 34) > rcClient.right )
		{
			point.x = rcClient.right - 34;
		}
		if( (point.x - 31) < rcClient.left )
		{
			point.x = rcClient.left + 31;
		}

		// If the point has changed since the last mouse move, then update change
		if( m_rcTrack.left != (point.x - 1) )
		{
			InvertTracker();
//			if(point.x < CPersonalityCtrl::MinSplitterXPos)
//			{
//				point.x = CPersonalityCtrl::MinSplitterXPos;
//			}
			m_rcTrack.left  = point.x - (m_nWidth>>1);
			m_rcTrack.right = point.x + (m_nWidth>>1);
			m_rcTrack.bottom = rcClient.bottom;
			InvertTracker();
		}
	}
}

//*****************************************************************************
void CVSplitter::OnCancelMode()
{
	OnEndCapture();
	CWnd::OnCancelMode();
}

//*****************************************************************************
void CVSplitter::OnCaptureChanged( CWnd *pWnd )
{
	OnEndCapture();
	CWnd::OnCaptureChanged( pWnd );
}

//*****************************************************************************
void CVSplitter::OnEndCapture()
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
void CVSplitter::InvertTracker()
{
	ASSERT( m_pParent );

	// Get the DC of our main frame
	CDC *pDC = m_pParent->GetDC();

	ASSERT( pDC );

	CRgn rgnNew, rgnOrig;
	rgnOrig.CreateRectRgn( 0, 0, 1, 1 );
	RECT rect;
	m_pParent->GetClientRect( &rect );
	rgnNew.CreateRectRgn( rect.left, rect.top, rect.right, rect.bottom );
	::GetClipRgn( pDC->GetSafeHdc(), rgnOrig );
	pDC->SelectClipRgn( &rgnNew, RGN_COPY );

	pDC->GetClipBox( &rect );

	// Create our inverted brush pattern (looks just like frame window sizing)
	CBrush *pBrush = CDC::GetHalftoneBrush();

	// Select the brush into our DC  
	HBRUSH hBrushStock = NULL;
	if( pBrush != NULL )
	{
		hBrushStock = (HBRUSH)SelectObject( pDC->m_hDC, pBrush->m_hObject );
	}

	// Draw our tracking line
	pDC->PatBlt( m_rcTrack.left, m_rcTrack.top, m_rcTrack.Width(), m_rcTrack.Height(), PATINVERT );

	// Free our brush and DC
	if (hBrushStock != NULL)
	{
		SelectObject( pDC->m_hDC, hBrushStock );
	}
	pDC->SelectClipRgn( &rgnOrig );
	ReleaseDC( pDC );

	// Toggle visible flag
	m_fVisible = !m_fVisible;

	rgnNew.DeleteObject();
	rgnOrig.DeleteObject();
}

BOOL CVSplitter::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return (pWnd == this)?CWnd::OnSetCursor(pWnd, nHitTest, message):FALSE;
}

RECT CVSplitter::GetFirstPaneRect()
{	
	return m_rcFirstPane;
}

RECT CVSplitter::GetSecondPaneRect()
{
	return m_rcSecondPane;
}

void CVSplitter::SetFirstPane( CWnd *pPane )
{
	m_pFirstPane = pPane;
}

void CVSplitter::SetSecondPane( CWnd *pPane )
{
	m_pSecondPane = pPane;
}
