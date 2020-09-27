// RhythmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DllBasePropPageManager.h"
#include "Conductor.h"		// Conductor interface and CLSID
#include "Pattern.h"
#include "RhythmDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Static variables
static int sg_nBeatWidth = 0;
static int sg_nBeatHeight = 0;
static int sg_nBeatTop = 0;
static int sg_nMeasureTop = 0;

static int sg_nMinScrollPos = 0;
static int sg_nMaxScrollPos = 0;
static int sg_nBeatsPerScrollPos = 0;

static long sg_lFirstSelectedBeat = -1;
static long sg_lLastSelectedBeat = -1;
static int sg_nSetBeatState = -1;

static int sg_dwScrollTick = 0;

/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid button

/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid constructor/destructor

CRhythmGrid::CRhythmGrid()
{
}

CRhythmGrid::~CRhythmGrid()
{
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::GetBeatsPerRhythmGrid

int CRhythmGrid::GetBeatsPerRhythmGrid( void )
{
	// Get grid rectangle
	CRect rectGrid;
	GetGridRect( &rectGrid );

	// Determine number of beats that can fit in the grid
	return rectGrid.Width() / sg_nBeatWidth;
}



/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::GetGridRect

void CRhythmGrid::GetGridRect( CRect* pRectGrid )
{
	// Get grid rectangle
	GetClientRect( pRectGrid );
	pRectGrid->InflateRect( -2, -4 );	// Leave room for embossing
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::SetClipRect

void CRhythmGrid::SetClipRect( CDC* pDC )
{
	// Get grid rectangle
	CRect rectGrid;
	GetGridRect( &rectGrid );

	// Create region
	CRgn rgn;
	rgn.CreateRectRgn( rectGrid.left, rectGrid.top, rectGrid.right, rectGrid.bottom );
	pDC->SelectClipRgn( &rgn );
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::DrawOneBeat

void CRhythmGrid::DrawOneBeat( CDC* pDC, long lBeat, BOOL fDrawOne )
{
	int nScrollPos = m_pRhythmDlg->m_scrollGrid.GetScrollPos();
	long lStartBeat = nScrollPos * sg_nBeatsPerScrollPos;

	long lNbrBeats = (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure *
					 (long)m_pRhythmDlg->m_wNbrMeasures;
	if( lBeat < lStartBeat
	||  lBeat >= lNbrBeats )
	{
		return;
	}

	// Calc measure and beat offset
	long lMeasure = lBeat / (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure;
	long lBeatOffset = lBeat % (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure;

	// Get grid rectangle
	CRect rectGrid;
	GetGridRect( &rectGrid );

	// Get beat rectangle
	CRect rectBeat;
	rectBeat.top = sg_nBeatTop;
	rectBeat.bottom = rectBeat.top + sg_nBeatHeight;
	rectBeat.left = rectGrid.left + ((lBeat - lStartBeat) * sg_nBeatWidth) + 1;
	rectBeat.right = rectBeat.left + sg_nBeatWidth;
	rectBeat.InflateRect( -5, -4 );

	// Draw 'X'
	SetClipRect( pDC );
	if( lBeatOffset >= 32 )
	{
		CBrush brushHatch;

		// DirectMusic does not support chord rhythms for BPM's over 32
		if( brushHatch.CreateHatchBrush( HS_DIAGCROSS, pDC->GetNearestColor(RGB(50,50,50)) ) )
		{
			int nOldBackgroundMode = pDC->SetBkMode( TRANSPARENT );
			CBrush* pOldBrush = pDC->SelectObject( &brushHatch );
			pDC->SetBkColor( 0 );
			rectBeat.InflateRect( 5, 3 );
			pDC->PatBlt( rectBeat.left, rectBeat.top, rectBeat.right - rectBeat.left, rectBeat.bottom - rectBeat.top, PATINVERT );
			rectBeat.InflateRect( -5, -3 );
			pDC->SetBkMode( nOldBackgroundMode );
			pDC->SelectObject( pOldBrush );
			brushHatch.DeleteObject();
		}
	}
	else if( m_pRhythmDlg->m_pRhythmMap[lMeasure] & (1 << lBeatOffset)  ) 
	{
		// Prepare the pen
		CPen pen;
		CPen *pPenOld = NULL;

		if( fDrawOne )
		{
			if( pen.CreatePen( PS_SOLID, 2, ::GetSysColor(COLOR_WINDOWTEXT) ) )
			{
				 pPenOld = pDC->SelectObject(&pen);
			}
		}

		// Draw the 'X'
		pDC->MoveTo( rectBeat.left, rectBeat.top );
		pDC->LineTo( rectBeat.right, rectBeat.bottom );
		pDC->MoveTo( rectBeat.right, rectBeat.top );
		pDC->LineTo( rectBeat.left, rectBeat.bottom );

		// Delete the pen
		if( pPenOld )
		{
			pDC->SelectObject( pPenOld );
			pen.DeleteObject();
		}
	}
	else
	{
		rectBeat.InflateRect( 1, 1 );
		pDC->FillSolidRect( &rectBeat, ::GetSysColor(COLOR_WINDOW) );
		rectBeat.InflateRect( -1, -1 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::DrawBeats

void CRhythmGrid::DrawBeats( CDC* pDC, long lStartBeat )
{
	// Determine the last beat currently being displayed
	long lNbrBeats = (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure *
					 (long)m_pRhythmDlg->m_wNbrMeasures;
	long lMaxBeat = min( lStartBeat + GetBeatsPerRhythmGrid(), lNbrBeats ); 

	// Prepare the pen
	CPen pen;
	CPen *pPenOld = NULL;

	if( pen.CreatePen( PS_SOLID, 2, ::GetSysColor(COLOR_WINDOWTEXT) ) )
	{
		 pPenOld = pDC->SelectObject(&pen);
	}

	// Draw the measure/beat lines
	long lBeat = lStartBeat;
	while( lBeat <= lMaxBeat )
	{
		DrawOneBeat( pDC, lBeat, FALSE );
		lBeat++;
	}

	// Delete the pen
	if( pPenOld )
	{
		pDC->SelectObject( pPenOld );
		pen.DeleteObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::DrawMeasureBeatLines

void CRhythmGrid::DrawMeasureBeatLines( CDC* pDC, long lStartBeat )
{
	long lBeat = lStartBeat;
	long lNbrBeats = (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure *
					 (long)m_pRhythmDlg->m_wNbrMeasures;

	// Get grid rectangle
	CRect rectGrid;
	GetGridRect( &rectGrid );

	// Determine max right
	int nMaxRight = min( rectGrid.right, rectGrid.left + ((lNbrBeats - lBeat) * sg_nBeatWidth) );

	// Set drawing rectangle for lines
	CRect rectBeatLine;
	rectBeatLine.top = sg_nBeatTop;
	rectBeatLine.bottom = rectBeatLine.top + sg_nBeatHeight;
	rectBeatLine.left = rectGrid.left;
	rectBeatLine.right = rectBeatLine.left + sg_nBeatWidth;
	rectBeatLine.InflateRect( 0, -1 );

	// Draw top and bottom lines
	pDC->MoveTo( rectBeatLine.left, rectBeatLine.top );
	pDC->LineTo( nMaxRight, rectBeatLine.top );
	pDC->MoveTo( rectBeatLine.left, rectBeatLine.bottom );
	pDC->LineTo( nMaxRight, rectBeatLine.bottom );

	// Create the pens
	CPen* pPenMeasureLine = new CPen( PS_SOLID, 1, MEASURE_LINE_COLOR );
	ASSERT( pPenMeasureLine != NULL );
	CPen* pPenBeatLine = new CPen( PS_SOLID, 1, BEAT_LINE_COLOR );
	ASSERT( pPenBeatLine != NULL );

	// Set text color
	pDC->SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );

	// Save the current pen
	CPen* pPenOld = pDC->SelectObject( pPenMeasureLine );
	
	// Draw the measure/beat lines
	while( rectBeatLine.right <= nMaxRight)
	{
		long lRemainder = lBeat % (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure;
		
		if( lRemainder == 0 )
		{
			// Get measure number text
			CString strMeasure;
			long lMeasure = lBeat / (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure;
			strMeasure.Format( "%d", lMeasure + 1 );

			// Set drawing rectangle for measure number
			CRect rectNbr;
			rectNbr.top = sg_nMeasureTop;
			rectNbr.bottom = sg_nBeatTop - 1;
			rectNbr.left = rectBeatLine.left;
			rectNbr.right = rectNbr.left + (sg_nBeatWidth << 1);

			// Draw measure number
			pDC->DrawText( strMeasure, -1, &rectNbr, (DT_SINGLELINE | DT_LEFT | DT_BOTTOM | DT_NOPREFIX) );
		}

		if( lRemainder == (m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure - 1) )
		{
			// Draw end of measure line
			pDC->SelectObject( pPenMeasureLine );
		}
		else
		{
			// Draw beat line
			pDC->SelectObject( pPenBeatLine );
		}

		pDC->MoveTo( rectBeatLine.right, rectBeatLine.top );
		pDC->LineTo( rectBeatLine.right, rectBeatLine.bottom );

		
		rectBeatLine.left = rectBeatLine.right;
		rectBeatLine.right = rectBeatLine.left + sg_nBeatWidth;
		lBeat++;
	}

	// Restore the previous pen
	pDC->SelectObject( pPenOld );

	// Cleanup
	if( pPenMeasureLine )
	{
		pPenMeasureLine->DeleteObject();
		delete pPenMeasureLine;
	}
	if( pPenBeatLine )
	{
		pPenBeatLine->DeleteObject();
		delete pPenBeatLine;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::DrawEntireGrid

void CRhythmGrid::DrawEntireGrid( CDC* pDC )
{
	int nScrollPos = m_pRhythmDlg->m_scrollGrid.GetScrollPos();
	long lStartBeat = nScrollPos * sg_nBeatsPerScrollPos;

	// Draw measure/beat markers
	DrawMeasureBeatLines( pDC, lStartBeat );

	// Draw beats
	DrawBeats( pDC, lStartBeat );
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::FindBeat

long CRhythmGrid::FindBeat( int nXPos, int nYPos )
{
	long lTheBeat = -1;

	// Find the beat
	if( nYPos > (sg_nBeatTop - 4)
	&&  nYPos < ((sg_nBeatTop + sg_nBeatHeight) + 4) )
	{
		// Store the first beat that is displaying
		int nScrollPos = m_pRhythmDlg->m_scrollGrid.GetScrollPos();
		long lStartBeat = nScrollPos * sg_nBeatsPerScrollPos;

		// Calc number of beats in the Pattern
		long lNbrBeats = (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure *
						 (long)m_pRhythmDlg->m_wNbrMeasures;

		// Get grid rectangle
		CRect rectGrid;
		GetGridRect( &rectGrid );
		nXPos -= rectGrid.left;

		// Calc the beat
		long lBeat = lStartBeat + (nXPos / sg_nBeatWidth);
		if( lBeat < lNbrBeats )
		{
			lTheBeat = lBeat;
		}
	}

	return lTheBeat;
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::IsBeatChecked

BOOL CRhythmGrid::IsBeatChecked( long lBeat )
{
	long lNbrBeats = (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure *
					 (long)m_pRhythmDlg->m_wNbrMeasures;

	if( lBeat >= 0
	&&  lBeat < lNbrBeats )
	{
		// Calc measure and beat offset
		long lMeasure = lBeat / (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure;
		long lBeatOffset = lBeat % (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure;

		// See whether or not beat is checked
		if( (lBeatOffset < 32)
		&&  (m_pRhythmDlg->m_pRhythmMap[lMeasure] & (1 << lBeatOffset)) )
		{
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::SetBeat

void CRhythmGrid::SetBeat( long lBeat, BOOL fRedraw )
{
	ASSERT( sg_nSetBeatState != -1 );

	long lNbrBeats = (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure *
					 (long)m_pRhythmDlg->m_wNbrMeasures;

	if( lBeat >= 0
	&&  lBeat < lNbrBeats )
	{
		// Calc measure and beat offset
		long lMeasure = lBeat / (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure;
		long lBeatOffset = lBeat % (long)m_pRhythmDlg->m_TimeSignature.m_bBeatsPerMeasure;
		
		// Check the beeat according to sg_nSetBeatState 
		if( lBeatOffset < 32 )
		{
			// Set the beat
			if( sg_nSetBeatState )
			{
				m_pRhythmDlg->m_pRhythmMap[lMeasure] |= (1 << lBeatOffset);
			}
			else
			{
				m_pRhythmDlg->m_pRhythmMap[lMeasure] &= ~(1 << lBeatOffset);
			}

			// Redraw the beat
			if( fRedraw )
			{
				CDC* pDC = GetDC();
				if( pDC )
				{
					DrawOneBeat( pDC, lBeat, TRUE );
					ReleaseDC( pDC );
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::ShiftClick

void CRhythmGrid::ShiftClick( long lBeat ) 
{
	ASSERT( m_pRhythmDlg != NULL );

	if( sg_lFirstSelectedBeat == -1 )
	{
		sg_lFirstSelectedBeat = lBeat;

		// Set sg_nSetBeatState
		if( IsBeatChecked( lBeat ) )
		{
			sg_nSetBeatState = FALSE;
		}
		else
		{
			sg_nSetBeatState = TRUE;
		}
	}
	sg_lLastSelectedBeat = lBeat;

	long lFirstBeat = sg_lFirstSelectedBeat <= sg_lLastSelectedBeat ?
					  sg_lFirstSelectedBeat : sg_lLastSelectedBeat; 
	long lLastBeat = sg_lFirstSelectedBeat <= sg_lLastSelectedBeat ?
					  sg_lLastSelectedBeat : sg_lFirstSelectedBeat; 

	// Set beats inside the range
	for( lBeat = lFirstBeat ;  lBeat <= lLastBeat ;  lBeat++ )
	{
		// Set the beat
		SetBeat( lBeat, FALSE );
	}

	// Initialize fields to force a new SHIFT-click operation
	if( lFirstBeat != lLastBeat )
	{
		sg_lFirstSelectedBeat = -1;
		sg_lLastSelectedBeat = -1;
		sg_nSetBeatState = -1;
	}

	Invalidate();
	UpdateWindow();
}


BEGIN_MESSAGE_MAP(CRhythmGrid, CButton)
	//{{AFX_MSG_MAP(CRhythmGrid)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid message handlers

/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::OnLButtonDown

void CRhythmGrid::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	UNREFERENCED_PARAMETER(nFlags);

	ASSERT( m_pRhythmDlg != NULL );

	// Get the beat under the cursor
	long lBeat = FindBeat( point.x, point.y );
	if( lBeat != -1 )
	{
		// First set capture
		SetCapture();

		// See if the SHIFT key is down
		if( GetAsyncKeyState(VK_SHIFT) & 0x8000 )
		{
			// SHIFT key is down
			ShiftClick( lBeat );
		}
		else
		{
			// Reset SHIFT-click fields
			sg_lFirstSelectedBeat = -1;
			sg_lLastSelectedBeat = -1;

			// Toggle this beat
			if( IsBeatChecked( lBeat ) )
			{
				sg_nSetBeatState = FALSE;
			}
			else
			{
				sg_nSetBeatState = TRUE;
			}
			SetBeat( lBeat, TRUE );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::OnLButtonDblClk

void CRhythmGrid::OnLButtonDblClk( UINT nFlags, CPoint point ) 
{
	OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::OnLButtonUp

void CRhythmGrid::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);

	ASSERT( m_pRhythmDlg != NULL );

	if( CWnd::GetCapture() == this )
	{
		::ReleaseCapture();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::OnMouseMove

void CRhythmGrid::OnMouseMove( UINT nFlags, CPoint point ) 
{
	UNREFERENCED_PARAMETER(nFlags);

	ASSERT( m_pRhythmDlg != NULL );

	if( CWnd::GetCapture() == this )
	{
		// Reset m_dwScrollTick to zero
		DWORD dwScrollTick = sg_dwScrollTick;
		sg_dwScrollTick = 0;

		// Get grid rect
		CRect rectGrid;
		GetGridRect( &rectGrid );

		// Get scroll position
		int nScrollPos = m_pRhythmDlg->m_scrollGrid.GetScrollPos();
		
		// Does position of cursor indicate user wants to scroll?
		short nDirection = -1;
		if( point.x > rectGrid.right )
		{
			if( nScrollPos < sg_nMaxScrollPos )
			{
				nDirection = SB_PAGERIGHT;
			}
		}
		else if( point.x < rectGrid.left )
		{
			if( nScrollPos > sg_nMinScrollPos )
			{
				nDirection = SB_PAGELEFT;
			}
		}
		if( nDirection != -1 )
		{
			// We may need to scroll
			DWORD dwTick = GetTickCount();
			if( dwScrollTick )
			{
				// scroll only if the proper amount of time has elapsed
				// since the last scroll
				if( dwTick >= dwScrollTick )
				{
					m_pRhythmDlg->SendMessage( WM_HSCROLL,
											   MAKELONG(nDirection, nScrollPos),
											   (LPARAM)m_pRhythmDlg->m_scrollGrid.m_hWnd );

					sg_dwScrollTick = dwTick + 350;	// wait 350 ms

					// Get new scroll position
					int nNewScrollPos = m_pRhythmDlg->m_scrollGrid.GetScrollPos();

					if( nNewScrollPos != nScrollPos )
					{
						long lScrollPosBeat = nScrollPos * sg_nBeatsPerScrollPos;
						long lNewScrollPosBeat = nNewScrollPos * sg_nBeatsPerScrollPos;
						if( nDirection == SB_PAGERIGHT )
						{
							lNewScrollPosBeat += (GetBeatsPerRhythmGrid() - 1);
							lScrollPosBeat += (GetBeatsPerRhythmGrid() - 1);
						}

						// Determine first/last beats
						long lFirstBeat = lNewScrollPosBeat <= lScrollPosBeat ?
										  lNewScrollPosBeat : lScrollPosBeat; 
						long lLastBeat  = lNewScrollPosBeat <= lScrollPosBeat ?
										  lScrollPosBeat : lNewScrollPosBeat; 

						// Update beats within the range
						for( long lBeat = lFirstBeat ;  lBeat <= lLastBeat ;  lBeat++ )
						{
							// Set the beat
							SetBeat( lBeat, FALSE );
						}

						// Redraw the grid
						Invalidate();
						UpdateWindow();
					}
				}
				else
				{
					sg_dwScrollTick = dwScrollTick;	// still waiting...
				}
			}
			else
			{
				sg_dwScrollTick = dwTick + 200;		// wait 200 ms
			}
			return;
		}

		// Handle the mouse move
		long lBeat = FindBeat( point.x, point.y );
		if( lBeat != -1 )
		{
			if( sg_nSetBeatState == -1 )
			{
				// Set sg_nSetBeatState
				if( IsBeatChecked( lBeat ) )
				{
					sg_nSetBeatState = FALSE;
				}
				else
				{
					sg_nSetBeatState = TRUE;
				}
			}

			// Set the beat
			SetBeat( lBeat, TRUE );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::OnEraseBkgnd

BOOL CRhythmGrid::OnEraseBkgnd( CDC* pDC ) 
{
	UNREFERENCED_PARAMETER(pDC);

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmGrid::DrawItem

void CRhythmGrid::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	ASSERT( m_pRhythmDlg != NULL );

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC )
	{
		CDC cdcMem;
		CBitmap bmpMem;
		CBitmap* pBitmapOld;
		CFont* pFontOld;
		int nBkModeOld;
		CRect rectGrid;

		// Calc grid width/height
		m_pRhythmDlg->m_btnGrid.GetClientRect( &rectGrid );
		int nGridWidth = rectGrid.Width();
		int nGridHeight = rectGrid.Height();

		// Create memory DC and bitmap
		if( cdcMem.CreateCompatibleDC( pDC ) == FALSE
		||  bmpMem.CreateCompatibleBitmap( pDC, nGridWidth, nGridHeight ) == FALSE )
		{
			return;
		}

		// Prepare memory DC
		pBitmapOld = cdcMem.SelectObject( &bmpMem );
		pFontOld = cdcMem.SelectObject( m_pRhythmDlg->m_pFont );
		nBkModeOld = cdcMem.SetBkMode( TRANSPARENT );
		
		// Erase memory DC background
		cdcMem.FillSolidRect( &rectGrid, ::GetSysColor(COLOR_WINDOW) );
		cdcMem.DrawEdge( &rectGrid, EDGE_SUNKEN, BF_RECT );

		// Draw the grid
		DrawEntireGrid( &cdcMem );
		pDC->BitBlt( 0, 0, nGridWidth, nGridHeight,
					 &cdcMem, 0, 0, SRCCOPY );

		// Cleanup memory DC
		cdcMem.SelectObject( pBitmapOld );
		cdcMem.SelectObject( pFontOld );
		cdcMem.SetBkMode( nBkModeOld );
		cdcMem.DeleteDC();
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg dialog

CRhythmDlg::CRhythmDlg( CWnd* pParent /*=NULL*/ )
			: CDialog( CRhythmDlg::IDD, pParent )
{
	//{{AFX_DATA_INIT(CRhythmDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pRhythmMap = NULL;
	m_wNbrMeasures = 0;
	m_pFont = NULL;
}

CRhythmDlg::~CRhythmDlg()
{
	if( m_pRhythmMap )
	{
		delete [] m_pRhythmMap;
	}
}


void CRhythmDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRhythmDlg)
	DDX_Control(pDX, IDC_GRID_SCROLL, m_scrollGrid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRhythmDlg, CDialog)
	//{{AFX_MSG_MAP(CRhythmDlg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_HALF, OnHalf)
	ON_BN_CLICKED(IDC_QUARTER, OnQuarter)
	ON_BN_CLICKED(IDC_WHOLE, OnWhole)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg::OnInitDialog

BOOL CRhythmDlg::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();
	
	ASSERT( m_pRhythmMap != NULL );
	ASSERT( m_pFont != NULL );

	// Subclass controls
	m_btnGrid.SubclassDlgItem( IDC_GRID, this );
	m_btnGrid.m_pRhythmDlg = this;

	// Get grid rectangle
	CRect rectGrid;
	m_btnGrid.GetGridRect( &rectGrid );

	// Set static variables
	{
		sg_nBeatWidth = 16;
		sg_nBeatHeight = 17;

		sg_nBeatTop = rectGrid.bottom - sg_nBeatHeight - 1; 
		sg_nMeasureTop = sg_nBeatTop - sg_nBeatHeight - 1;
		
		sg_lFirstSelectedBeat = -1;
		sg_lLastSelectedBeat = -1;
		sg_nSetBeatState = -1;

		long lNbrBeats = (long)m_TimeSignature.m_bBeatsPerMeasure * (long)m_wNbrMeasures;
		sg_nBeatsPerScrollPos = min( m_TimeSignature.m_bBeatsPerMeasure, m_btnGrid.GetBeatsPerRhythmGrid() );
		sg_nMinScrollPos = 0;
		sg_nMaxScrollPos = lNbrBeats / sg_nBeatsPerScrollPos;
		if( !(lNbrBeats % sg_nBeatsPerScrollPos) )
		{
			sg_nMaxScrollPos--;
		}

		sg_dwScrollTick = 0;
	}

	// Set scroll range
	m_scrollGrid.SetScrollRange( sg_nMinScrollPos, sg_nMaxScrollPos, FALSE );
	m_scrollGrid.SetScrollPos( sg_nMinScrollPos );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg::OnCreate

int CRhythmDlg::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Create Font
	m_pFont = new CFont;
	if( m_pFont == NULL )
	{
		return FALSE;
	}
	if( m_pFont->CreateFont( 10, 0, 0, 0, FW_NORMAL, 0, 0, 0,
 							 DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
							 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) == FALSE )
	{
		return FALSE;
	}

	if( CDialog::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}
	
	// TODO: Add your specialized creation code here
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg::OnDestroy

void CRhythmDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnDestroy();

	// Delete normal font
	if( m_pFont )
	{
		m_pFont->DeleteObject();
		delete m_pFont;
		m_pFont = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg::OnClear

void CRhythmDlg::OnClear() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(	m_pRhythmMap )
	{
		for( int i = 0 ;  i < m_wNbrMeasures ;  ++i )
		{
			m_pRhythmMap[i] = 0;
		}

		m_btnGrid.Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg::OnHalf

void CRhythmDlg::OnHalf() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pRhythmMap )
	{
		int nHalfway = m_TimeSignature.m_bBeatsPerMeasure >> 1;
		if( m_TimeSignature.m_bBeatsPerMeasure % 2 )
		{
			nHalfway++;
		}
		if( nHalfway >= 32 )
		{
			nHalfway = 0;
		}

		DWORD dwRhythm = (1 << nHalfway) | 0x01;	// Turn on halfway point and start of measure

		for( int i = 0 ;  i < m_wNbrMeasures ;  ++i )
		{
			m_pRhythmMap[i] = dwRhythm;
		}

		m_btnGrid.Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg::OnQuarter

void CRhythmDlg::OnQuarter() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pRhythmMap )
	{
		for( int i = 0 ;  i < m_wNbrMeasures ;  ++i )
		{
			m_pRhythmMap[i] = 0xFFFFFFFF;
		}

		m_btnGrid.Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg::OnWhole

void CRhythmDlg::OnWhole() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pRhythmMap )
	{
		for( int i = 0 ;  i < m_wNbrMeasures ;  ++i )
		{
			m_pRhythmMap[i] = 0x01;
		}

		m_btnGrid.Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRhythmDlg::OnHScroll

void CRhythmDlg::OnHScroll( UINT nSBCode, UINT nThumbPos, CScrollBar* pScrollBar ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	if( pScrollBar->GetDlgCtrlID() != IDC_GRID_SCROLL )
	{
		CDialog::OnHScroll( nSBCode, nThumbPos, pScrollBar );
		return;
	}
	int nCurPos = pScrollBar->GetScrollPos();

	int nNewPos = nCurPos;
	
	switch( nSBCode )
	{
		case SB_LEFT:
			nNewPos = sg_nMinScrollPos;
			break;

		case SB_RIGHT:
			nNewPos = sg_nMaxScrollPos;
			break;

		case SB_LINELEFT:
			nNewPos--;
			nNewPos = max( nNewPos, sg_nMinScrollPos ); 
			break;

		case SB_LINERIGHT:
			nNewPos++;
			nNewPos = min( nNewPos, sg_nMaxScrollPos ); 
			break;

		case SB_PAGELEFT:
			nNewPos = nCurPos - (m_btnGrid.GetBeatsPerRhythmGrid() / sg_nBeatsPerScrollPos);
			nNewPos = max( nNewPos, sg_nMinScrollPos ); 
			break;

		case SB_PAGERIGHT:
			nNewPos = nCurPos + (m_btnGrid.GetBeatsPerRhythmGrid() / sg_nBeatsPerScrollPos);
			nNewPos = min( nNewPos, sg_nMaxScrollPos ); 
			break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			nNewPos = nThumbPos;
			break;
	}

	if( nNewPos != nCurPos )
	{
		pScrollBar->SetScrollPos( nNewPos );
		m_btnGrid.Invalidate();
		m_btnGrid.UpdateWindow();
	}
}
