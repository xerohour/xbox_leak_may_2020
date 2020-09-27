// Splitter.cpp : implementation file
//

#include "stdafx.h"

#include "ToolGraphDesignerDLL.h"
#include "Graph.h"
#include "Splitter.h"
#include "GraphDlg.h"

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
	m_nWidth(0),
	m_nID(0),
	m_pParent(NULL)
{
}

//*****************************************************************************
CSplitter::~CSplitter()
{
}

//*****************************************************************************
int CSplitter::GetWidth( void )
{
	return m_nWidth;
}

//*****************************************************************************
BOOL CSplitter::Create( CWnd *pParent, int nWidth, int nID, RECT *rect )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UNREFERENCED_PARAMETER(rect);

	// Save our parent
	m_pParent = pParent;

	// Set our width
	m_nWidth = nWidth;

	// Set our control ID
	m_nID = nID;

	// Create our splitter class
	WNDCLASS wc;

	ZeroMemory( &wc, sizeof(wc) );
	wc.lpszClassName = "JzGraphSplitter";
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
void CSplitter::OnPaint()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPaintDC dc( this );
	CRect rc;
	CBrush brush;

	GetClientRect( &rc );
	rc.InflateRect( 0, 1 );
	dc.Draw3dRect( &rc, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW) );
	rc.InflateRect( 0, -1 );

	if( brush.CreateSolidBrush( GetSysColor(COLOR_BTNFACE) ) )
	{
		CBrush* pOldBrush = dc.SelectObject( &brush );
		rc.InflateRect( -1, 0 );
		dc.FillRect( &rc, &brush );
		dc.SelectObject( pOldBrush );
	}		
}

//*****************************************************************************
void CSplitter::OnLButtonDown( UINT /*nFlags*/, CPoint /*point*/ )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pParent != NULL );

	BOOL fWasTracking = m_fTracking;

	OnEndCapture();

	if( fWasTracking )
	{
		CRect rect;
		m_pParent->GetClientRect( &rect );
		static_cast<CGraphDlg*>(m_pParent)->EndTrack( m_nID, m_rcTrack.left );
	}
}

//*****************************************************************************
void CSplitter::OnMouseMove( UINT /*nFlags*/, CPoint point )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
		if( point.x < rcClient.left )
		{
			point.x = rcClient.left;
		}
		if( point.x > (rcClient.right - m_nWidth) )
		{
			point.x = (rcClient.right - m_nWidth);
		}

		// If the point has changed since the last mouse move, then update change
		if( m_rcTrack.left != point.x )
		{
			InvertTracker();
			m_rcTrack.left  = point.x;
			m_rcTrack.right = point.x + m_nWidth;
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
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

	CRect rect( m_rcTrack );
	m_pParent->ClientToScreen( &rect );
	ScreenToClient( &rect );

	// Draw our tracking line
	pDC->PatBlt( rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT );

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
