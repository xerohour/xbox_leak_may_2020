// Splitter.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "Splitter.h"
#include "PersonalityDesigner.h"
#include "PersonalityCtl.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPersonalityDesignerApp theApp;

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
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

//*****************************************************************************
CSplitter::CSplitter() :
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
	AFX_MANAGE_STATE(_afxModuleAddrThis);
}

//*****************************************************************************
CSplitter::~CSplitter()
{
}

//*****************************************************************************
BOOL CSplitter::Create( CWnd *pParent, RECT *rect )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

		m_nWidth = rect->right-rect->left;
		ASSERT( m_nWidth > 0 );
	}

	// Create our splitter class
	WNDCLASS wc;

	ZeroMemory( &wc, sizeof(wc) );
	wc.lpszClassName = "DittoSplitterClass";
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = ::DefWindowProc;
	wc.hInstance     = AfxGetInstanceHandle();
	wc.hCursor       = theApp.LoadCursor( MAKEINTRESOURCE(IDC_SPLITTER) );
	wc.hbrBackground = (HBRUSH)( COLOR_BTNFACE + 1 );

	// Register our splitter class
	AfxRegisterClass( &wc );

	// Create our splitter
	return CWnd::Create( wc.lpszClassName, "", WS_VISIBLE | WS_CHILD, 
						 m_rcTrack, pParent, IDC_SPLITTER );
}

//*****************************************************************************
void CSplitter::OnPaint()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
	m_rcSecondPane.bottom = m_rcTrack.bottom;
	m_rcSecondPane.right = rc.right;

	// Resize First Pane
	m_pFirstPane->MoveWindow( &m_rcFirstPane );

	// Resize First Pane Window
	m_pSecondPane->MoveWindow( &m_rcSecondPane );

	// Paint Scroll Bar
	CPaintDC dc( this );
	GetClientRect( &rc );
	dc.Draw3dRect( &rc, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
}

//*****************************************************************************
void CSplitter::OnLButtonDown( UINT /*nFlags*/, CPoint /*point*/ )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
void CSplitter::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pParent != NULL );
	BOOL fWasTracking = m_fTracking;

	OnEndCapture();
	
	if(fWasTracking)
	{
		RECT rect;
		m_pParent->GetClientRect( &rect );
		static_cast<CPersonalityCtrl*>(m_pParent)->EndTrack( m_rcTrack.left );

		MoveWindow(	&m_rcTrack );
		OnPaint();
	}
}

//*****************************************************************************
void CSplitter::OnMouseMove( UINT /*nFlags*/, CPoint point )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fTracking )
	{
		// Convert out point to client coordinates of our frame
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
void CSplitter::OnCancelMode()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	OnEndCapture();
	CWnd::OnCancelMode();
}

//*****************************************************************************
void CSplitter::OnCaptureChanged( CWnd *pWnd )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pParent );

	// Get the DC of our main frame
	CDC *pDC = m_pParent->GetDC();

	ASSERT( pDC );

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
	ReleaseDC( pDC );

	// Toggle visible flag
	m_fVisible = !m_fVisible;
}

BOOL CSplitter::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return (pWnd == this)?CWnd::OnSetCursor(pWnd, nHitTest, message):FALSE;
}

RECT CSplitter::GetFirstPaneRect()
{	
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return m_rcFirstPane;
}

RECT CSplitter::GetSecondPaneRect()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return m_rcSecondPane;
}

void CSplitter::SetFirstPane( CWnd *pPane )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pFirstPane = pPane;
}

void CSplitter::SetSecondPane( CWnd *pPane )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pSecondPane = pPane;
}



void CSplitter::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}
