// VarChoicesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "VarChoices.h"
#include "VarChoicesCtl.h"
#include "VarChoicesDlg.h"
#include <ioDMStyle.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SMALL_BTN			0
#define MEDIUM_BTN			6
#define LARGE_BTN			12

#define BIT_ON				0
#define BIT_ON_SEL			1
#define BIT_ON_DIS			2
#define BIT_OFF				3
#define BIT_OFF_SEL			4
#define BIT_OFF_DIS			5

#define BTN_SM_BIT_ON		0
#define BTN_SM_BIT_ON_SEL	1
#define BTN_SM_BIT_ON_DIS	2
#define BTN_SM_BIT_OFF		3
#define BTN_SM_BIT_OFF_SEL	4
#define BTN_SM_BIT_OFF_DIS	5
#define BTN_MD_BIT_ON		6
#define BTN_MD_BIT_ON_SEL	7
#define BTN_MD_BIT_ON_DIS	8
#define BTN_MD_BIT_OFF		9
#define BTN_MD_BIT_OFF_SEL	10
#define BTN_MD_BIT_OFF_DIS	11
#define BTN_LG_BIT_ON		12
#define BTN_LG_BIT_ON_SEL	13
#define BTN_LG_BIT_ON_DIS	14
#define BTN_LG_BIT_OFF		15
#define BTN_LG_BIT_OFF_SEL	16
#define BTN_LG_BIT_OFF_DIS	17
#define NBR_BTN_BITMAPS		18

// Button bitmap array
static CBitmap	abmpButton[NBR_BTN_BITMAPS];

// Button sizes
static int sg_nBtnHeight = 0;
static int sg_nSmallBtnWidth = 0;
static int sg_nMediumBtnWidth = 0;
static int sg_nLargeBtnWidth = 0;

// Button bitmaps
static CBitmap sg_bmpRowOnUp;
static CBitmap sg_bmpRowOnDown;


// Used when tracking the mouse
static short sg_nSetBitFlagState = -1;
static short sg_nSetBitSelectState = -1;
static short sg_nFirstSelectedRow = -1;
static short sg_nFirstSelectedBtnRow = -1;
static short sg_nFirstSelectedBtnColumn = -1;


// DirectMusic mode /////////////////////////////////////////
#define DM_NBR_COLUMNS		30

// X positions associated with DirectMusic buttons
static short sga_DM_XPos[DM_NBR_COLUMNS];

// Text associated with DirectMusic buttons
static TCHAR sga_DM_Text[DM_NBR_COLUMNS][SMALL_BUFFER];

// Bitmaps associated with DirectMusic buttons
const static short sga_DM_AssociatedBmp[DM_NBR_COLUMNS] =
		{ SMALL_BTN, SMALL_BTN, SMALL_BTN,
		  SMALL_BTN, SMALL_BTN, SMALL_BTN,
		  SMALL_BTN, SMALL_BTN, SMALL_BTN,
		  SMALL_BTN, SMALL_BTN, SMALL_BTN,
		  SMALL_BTN, SMALL_BTN, SMALL_BTN,
		  SMALL_BTN, SMALL_BTN, SMALL_BTN,
		  SMALL_BTN, SMALL_BTN, SMALL_BTN,
		  MEDIUM_BTN, MEDIUM_BTN, MEDIUM_BTN, SMALL_BTN, SMALL_BTN, MEDIUM_BTN,
		  MEDIUM_BTN, MEDIUM_BTN, MEDIUM_BTN };


// IMA mode /////////////////////////////////////////////////
#define IMA_NBR_COLUMNS		15

// X positions associated with IMA buttons
static short sga_IMA_XPos[IMA_NBR_COLUMNS];

// Text associated with IMA buttons
static TCHAR sga_IMA_Text[IMA_NBR_COLUMNS][SMALL_BUFFER];

// Bitmaps associated with IMA buttons
const static short sga_IMA_AssociatedBmp[IMA_NBR_COLUMNS] =
		{ LARGE_BTN,
		  LARGE_BTN,
		  LARGE_BTN,
		  LARGE_BTN,
		  LARGE_BTN,
		  LARGE_BTN,
		  LARGE_BTN,
		  MEDIUM_BTN, SMALL_BTN, SMALL_BTN, MEDIUM_BTN, MEDIUM_BTN, MEDIUM_BTN,
		  LARGE_BTN, LARGE_BTN };


// Various types of modes (rows) ////////////////////////////
#define IMA_ROWTYPE		0
#define DM_ROWTYPE		1
#define NBR_ROWTYPES	2

// Arrays associated with buttons
const static short* sga_nBtnXPos[NBR_ROWTYPES] = {			// Button X positions
	&sga_IMA_XPos[0],
	&sga_DM_XPos[0] };
const static short* sga_nBtnBmp[NBR_ROWTYPES] = {			// Associated bitmaps
	&sga_IMA_AssociatedBmp[0],
	&sga_DM_AssociatedBmp[0] };
// This array is defined in SharedPattern.cpp
extern const short* sga_nBtnBit[NBR_ROWTYPES];	// Associated dwVariationChoices bits


/////////////////////////////////////////////////////////////////////////////
// Function DrawBitMap

static void DrawBitMap( CDC* pDC, CBitmap* pBitmap, int nDestX, int nDestY,
						int nSourceX, int nSourceY )
{
	// This is a much quicker and simpler way to send a bitmap to the screen.
	// Note: You MUST set the dimensions earlier via a call to SetBitmapDimension()!
	pDC->DrawState( CPoint(nDestX, nDestY), pBitmap->GetBitmapDimension(), pBitmap, DSS_NORMAL );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn

CVarChoicesFlagsBtn::CVarChoicesFlagsBtn()
{
}

CVarChoicesFlagsBtn::~CVarChoicesFlagsBtn()
{
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::IMA_DrawButtonText

void CVarChoicesFlagsBtn::IMA_DrawButtonText( CDC* pDC, CRect* pRect, short nRow, short nColumn )
{
    pDC->DrawText( sga_IMA_Text[nColumn], -1, pRect, (DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX) );
} 


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::DM_DrawButtonText

void CVarChoicesFlagsBtn::DM_DrawButtonText( CDC* pDC, CRect* pRect, short nRow, short nColumn )
{
	CFont* pFontOld;
	BOOL fItalics = FALSE;

	ASSERT( m_pVarChoicesDlg != NULL );
	ASSERT( m_pVarChoicesDlg->m_pVarChoices != NULL );

	if( nColumn < 21
	&& (nColumn % 3) == 2 )
	{
		fItalics = TRUE;
	}

	if( fItalics )
	{
		if( m_pVarChoicesDlg->IsBitOn( nRow, nColumn )
		||  m_pVarChoicesDlg->IsRowDisabled( nRow ) )
		{
			pFontOld = pDC->SelectObject( m_pVarChoicesDlg->m_pFontItalicsBold );
		}
		else
		{
			pFontOld = pDC->SelectObject( m_pVarChoicesDlg->m_pFontItalics );
		}
	}

    pDC->DrawText( sga_DM_Text[nColumn], -1, pRect, (DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX) );

	if( fItalics )
	{
		pDC->SelectObject( pFontOld );
	}
} 


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::DrawOneButton

void CVarChoicesFlagsBtn::DrawOneButton( CDC* pDC, short nRow, short nColumn, BOOL fDrawOne )
{
	CFont* pFontOld;
	int nBkModeOld;
	CRect rect;

	ASSERT( m_pVarChoicesDlg != NULL );
	ASSERT( m_pVarChoicesDlg->m_pVarChoices != NULL );

	// Determine what kind of buttons are in the row
	short nRowType = m_pVarChoicesDlg->GetRowType( nRow );

    // Determine rectangle of button
	if( fDrawOne )
	{
        rect.top = sg_nBtnHeight * nRow;
	}
    else
	{
        rect.top = 0;
	}
    rect.bottom = rect.top + sg_nBtnHeight;
    rect.left   = sga_nBtnXPos[nRowType][nColumn];
    rect.right  = rect.left + m_pVarChoicesDlg->GetBtnWidth( nRow, nColumn );
	
	// Draw the background
	short nAssociatedBmp = sga_nBtnBmp[nRowType][nColumn];

	if( m_pVarChoicesDlg->IsBtnSelected( nRow, nColumn ) )
    {
		// Button is selected
		if( m_pVarChoicesDlg->IsBitOn( nRow, nColumn ) )
		{
		    // Bit is on
			pDC->SetTextColor( RGB(0,0,0) );
			DrawBitMap( pDC, &abmpButton[nAssociatedBmp + BIT_ON_SEL], rect.left, rect.top, 0, 0 );
			rect.top  += 2; 
			rect.left += 2;
		}
		else
		{
		    // Bit is off
			pDC->SetTextColor( ::GetSysColor(COLOR_BTNSHADOW) );
			DrawBitMap( pDC, &abmpButton[nAssociatedBmp + BIT_OFF_SEL], rect.left, rect.top, 0, 0 );
		}
    }
    else
    {
		// Button is not selected
		if( m_pVarChoicesDlg->IsRowDisabled( nRow ) )
		{
			// Row is disabled
			pDC->SetTextColor( ::GetSysColor(COLOR_BTNSHADOW) );
			if( m_pVarChoicesDlg->IsBitOn( nRow, nColumn ) )
			{
				// Bit is on
				DrawBitMap( pDC, &abmpButton[nAssociatedBmp + BIT_ON_DIS], rect.left, rect.top, 0, 0 );
				rect.top  += 2; 
				rect.left += 2;
			}
			else
			{
			    // Bit is off
				DrawBitMap( pDC, &abmpButton[nAssociatedBmp + BIT_OFF_DIS], rect.left, rect.top, 0, 0 );
			}
		}
		else if( m_pVarChoicesDlg->IsBitOn( nRow, nColumn ) )
		{
		    // Bit is on
		    pDC->SetTextColor( RGB(0,0,0) );
			DrawBitMap( pDC, &abmpButton[nAssociatedBmp + BIT_ON], rect.left, rect.top, 0, 0 );
			rect.top  += 2; 
			rect.left += 2;
		}
		else
		{
		    // Bit is off
			pDC->SetTextColor( ::GetSysColor(COLOR_BTNSHADOW) );
			DrawBitMap( pDC, &abmpButton[nAssociatedBmp + BIT_OFF], rect.left, rect.top, 0, 0 );
		}
    }

	rect.left++;

	if( fDrawOne )
	{
        nBkModeOld = pDC->SetBkMode( TRANSPARENT );
	}

	if( m_pVarChoicesDlg->IsBitOn( nRow, nColumn )
	||  m_pVarChoicesDlg->IsRowDisabled( nRow ) )
	{
		pFontOld = pDC->SelectObject( m_pVarChoicesDlg->m_pFontBold );
	}
	else
	{
		pFontOld = pDC->SelectObject( m_pVarChoicesDlg->m_pFont );
	}

	switch( nRowType )
	{
		case IMA_ROWTYPE:
			IMA_DrawButtonText( pDC, &rect, nRow, nColumn );
			break;

		case DM_ROWTYPE:
			DM_DrawButtonText( pDC, &rect, nRow, nColumn );
			break;

		default:
			ASSERT( 0 );
	}

	pDC->SelectObject( pFontOld );

    if( fDrawOne )
    {
        pDC->SetBkMode( nBkModeOld );
    }
} 


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::DrawRowOfButtons

void CVarChoicesFlagsBtn::DrawRowOfButtons( CDC* pDC, short nRow )
{
	CDC cdcMem;
	CBitmap bmpMem;
	CBitmap* pBitmapOld;
	CFont* pFontOld;
	int nBkModeOld;
	CRect rect;

	GetClientRect( &rect );
	int nRowWidth = rect.Width();

	ASSERT( m_pVarChoicesDlg != NULL );

	if( cdcMem.CreateCompatibleDC( pDC ) == FALSE
	||  bmpMem.CreateCompatibleBitmap( pDC, nRowWidth, sg_nBtnHeight ) == FALSE )
	{
		return;
	}

    pBitmapOld = cdcMem.SelectObject( &bmpMem );
	pFontOld = cdcMem.SelectObject( m_pVarChoicesDlg->m_pFont );
    nBkModeOld = cdcMem.SetBkMode( TRANSPARENT );

    rect.left   = 0;
    rect.top    = 0;
    rect.right  = nRowWidth;
    rect.bottom = sg_nBtnHeight; 

	cdcMem.FillSolidRect( &rect, ::GetSysColor(COLOR_BTNFACE) );

    short nNbrColumns = m_pVarChoicesDlg->GetNbrColumns( nRow );

	for( short nColumn = 0 ;  nColumn < nNbrColumns ;  nColumn++ )
    {
        DrawOneButton( &cdcMem, nRow, nColumn, FALSE );
    }

    rect.top = sg_nBtnHeight * nRow;
    rect.bottom = rect.top + sg_nBtnHeight; 
    pDC->BitBlt( 0, rect.top, nRowWidth, rect.bottom,
				 &cdcMem, 0, 0, SRCCOPY );

    cdcMem.SelectObject( pBitmapOld );
    cdcMem.SelectObject( pFontOld );
    cdcMem.SetBkMode( nBkModeOld );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::DrawAllButtons

void CVarChoicesFlagsBtn::DrawAllButtons( CDC* pDC )
{
    for( short nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
    {
        DrawRowOfButtons( pDC, nRow );
    }
}


BEGIN_MESSAGE_MAP(CVarChoicesFlagsBtn, CButton)
	//{{AFX_MSG_MAP(CVarChoicesFlagsBtn)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn message handlers

/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::OnLButtonDown

void CVarChoicesFlagsBtn::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pVarChoicesDlg != NULL );
	ASSERT( m_pVarChoicesDlg->m_pVarChoices != NULL );

	short nRow = m_pVarChoicesDlg->FindRow( point.y );

	if( nRow != -1 )
	{
		short nColumn = m_pVarChoicesDlg->FindColumn( nRow, point.x );

		if( nColumn != -1 )
		{
			// First set capture
			m_pVarChoicesDlg->CaptureMouse( this );

			// Select this button
			if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
			{
				// CTRL key is down
				sg_nFirstSelectedBtnRow = nRow;
				sg_nFirstSelectedBtnColumn = nColumn;

				// Set sg_nSetBitSelectState
				if( m_pVarChoicesDlg->IsBtnSelected( nRow, nColumn ) )
				{
					sg_nSetBitSelectState = FALSE;
				}
				else
				{
					sg_nSetBitSelectState = TRUE;
				}

				// Unselect all "row" buttons
				for( int i = 0 ;  i < NBR_VARIATIONS ;  i++ )
				{
					m_pVarChoicesDlg->m_pVarChoices->m_bSelectedRowBtns[i] = 0;
				}

				// Toggle the state of this button
				m_pVarChoicesDlg->m_pVarChoices->m_dwSelectedFlagBtns[nRow] ^= (1 << nColumn);
				CDC* pDC = GetDC();
				if( pDC )
				{
					DrawOneButton( pDC, nRow, nColumn, TRUE );
					ReleaseDC( pDC );
				}

				// Redraw "row" buttons
				m_pVarChoicesDlg->m_btnRows.InvalidateRect( NULL, FALSE );
				m_pVarChoicesDlg->m_btnRows.UpdateWindow();
			}
			else if( GetAsyncKeyState(VK_SHIFT) & 0x8000 )
			{
				int i;
				int j;

				// SHIFT key is down
				if( sg_nFirstSelectedBtnRow == -1
				||  sg_nFirstSelectedBtnColumn == -1 )
				{
					sg_nFirstSelectedBtnRow = nRow;
					sg_nFirstSelectedBtnColumn = nColumn;
				}

				// Unselect all buttons
				for( i = 0 ;  i < NBR_VARIATIONS ;  i++ )
				{
					m_pVarChoicesDlg->m_pVarChoices->m_bSelectedRowBtns[i] = 0;
					m_pVarChoicesDlg->m_pVarChoices->m_dwSelectedFlagBtns[i] = 0x0000000;
				}

				int nFirstRow;
				int nFirstRowColumn;
				int nFirstXPos;
				int nLastRow;
				int nLastRowColumn;
				int nLastXPos;

				// Determine first/last fields
				if( nRow <= sg_nFirstSelectedBtnRow )
				{
					nFirstRow = nRow; 
					nFirstRowColumn = nColumn;
					nLastRow = sg_nFirstSelectedBtnRow;
					nLastRowColumn = sg_nFirstSelectedBtnColumn;
				}
				else
				{
					nFirstRow = sg_nFirstSelectedBtnRow;
					nFirstRowColumn = sg_nFirstSelectedBtnColumn;
					nLastRow = nRow; 
					nLastRowColumn = nColumn;
				}

				short nFirstRowType = m_pVarChoicesDlg->GetRowType( nFirstRow );
				short nLastRowType = m_pVarChoicesDlg->GetRowType( nLastRow );

				int nFirstRowXPos = sga_nBtnXPos[nFirstRowType][nFirstRowColumn];
				int nLastRowXPos = sga_nBtnXPos[nLastRowType][nLastRowColumn];
			
				if( nFirstRowXPos <= nLastRowXPos )
				{
					nFirstXPos = nFirstRowXPos;
					nLastXPos = nLastRowXPos;
					nLastXPos += m_pVarChoicesDlg->GetBtnWidth( nLastRow, nLastRowColumn );
				}
				else
				{
					nFirstXPos = nLastRowXPos;
					nLastXPos = nFirstRowXPos;
					nLastXPos += m_pVarChoicesDlg->GetBtnWidth( nFirstRow, nFirstRowColumn );
				}

				int nXPos;
				int nNbrColumns;
				int nRowType;

				// Select buttons inside the range
				for( i = 0 ;  i < NBR_VARIATIONS ;  i++ )
				{
					if( i >= nFirstRow
					&&  i <= nLastRow )
					{
						nRowType = m_pVarChoicesDlg->GetRowType( i );
						nNbrColumns = m_pVarChoicesDlg->GetNbrColumns( i );
		
						for( j = 0 ;  j < nNbrColumns ;  j++ )
						{
							nXPos = sga_nBtnXPos[nRowType][j];

							if( nXPos >= nFirstXPos 
							&&  nXPos < nLastXPos )
							{
								m_pVarChoicesDlg->m_pVarChoices->m_dwSelectedFlagBtns[i] |= (1 << j);
							}
						}
					}
				}

				// Redraw buttons
				m_pVarChoicesDlg->m_btnRows.InvalidateRect( NULL, FALSE );
				m_pVarChoicesDlg->m_btnRows.UpdateWindow();

				InvalidateRect( NULL, FALSE );
				UpdateWindow();
			}
			else
			{
				// Unselect all buttons
				m_pVarChoicesDlg->UnselectAllButtons( TRUE );

				// Set sg_nSetBitFlagState
				if( m_pVarChoicesDlg->IsBitOn( nRow, nColumn ) )
				{
					sg_nSetBitFlagState = FALSE;
				}
				else
				{
					sg_nSetBitFlagState = TRUE;
				}

				// Set the bit
				m_pVarChoicesDlg->SetBit( nRow, nColumn );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::OnLButtonDblClk

void CVarChoicesFlagsBtn::OnLButtonDblClk( UINT nFlags, CPoint point ) 
{
	OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::OnLButtonUp

void CVarChoicesFlagsBtn::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pVarChoicesDlg != NULL );

	if( CWnd::GetCapture() == this )
	{
		m_pVarChoicesDlg->ReleaseMouse( this );
	}

	sg_nSetBitFlagState = -1;
	sg_nSetBitSelectState = -1;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::OnRButtonDown

void CVarChoicesFlagsBtn::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	CButton::OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::OnRButtonDblClk

void CVarChoicesFlagsBtn::OnRButtonDblClk( UINT nFlags, CPoint point ) 
{
	OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::OnMouseMove

void CVarChoicesFlagsBtn::OnMouseMove( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pVarChoicesDlg != NULL );

	if( CWnd::GetCapture() == this )
	{
		short nRow = m_pVarChoicesDlg->FindRow( point.y );

		if( nRow != -1 )
		{
			short nColumn = m_pVarChoicesDlg->FindColumn( nRow, point.x );

			if( nColumn != -1 )
			{
				if( sg_nSetBitFlagState == -1 )
				{
					// CTRL/SHIFT key down in WM_LBUTTONDOWN
					if( sg_nSetBitSelectState )
					{
						m_pVarChoicesDlg->m_pVarChoices->m_dwSelectedFlagBtns[nRow] |= (1 << nColumn);
					}
					else
					{
						m_pVarChoicesDlg->m_pVarChoices->m_dwSelectedFlagBtns[nRow] &= ~(1 << nColumn);
					}
					CDC* pDC = GetDC();
					if( pDC )
					{
						DrawOneButton( pDC, nRow, nColumn, TRUE );
						ReleaseDC( pDC );
					}
				}
				else
				{
					// Set the bit
					m_pVarChoicesDlg->SetBit( nRow, nColumn );
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::OnEraseBkgnd

BOOL CVarChoicesFlagsBtn::OnEraseBkgnd( CDC* pDC ) 
{
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesFlagsBtn::DrawItem

void CVarChoicesFlagsBtn::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC )
	{
	    DrawAllButtons( pDC );
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn

CVarChoicesRowsBtn::CVarChoicesRowsBtn()
{
}

CVarChoicesRowsBtn::~CVarChoicesRowsBtn()
{
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn::DrawOneButton

void CVarChoicesRowsBtn::DrawOneButton( CDC* pDC, short nRow )
{
	int nDestY;

	ASSERT( m_pVarChoicesDlg != NULL );
	ASSERT( m_pVarChoicesDlg->m_pVarChoices != NULL );

    // Determine rectangle of button
    nDestY = sg_nBtnHeight * nRow;

	if( m_pVarChoicesDlg->IsRowSelected( nRow ) )
    {
		// Button is selected - draw down state
		DrawBitMap( pDC, &sg_bmpRowOnDown, 0, nDestY, 0, 0 );
    }
	else
	{
		// Button is not selected - draw up state
		DrawBitMap( pDC, &sg_bmpRowOnUp, 0, nDestY, 0, 0 );
    }
} 


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn::DrawAllButtons

void CVarChoicesRowsBtn::DrawAllButtons( CDC* pDC )
{
	CDC cdcMem;
	CBitmap bmpMem;
	CBitmap* pBitmapOld;
	CRect rect;

	GetClientRect( &rect );

	if( cdcMem.CreateCompatibleDC( pDC ) == FALSE
	||  bmpMem.CreateCompatibleBitmap( pDC, rect.Width(), rect.Height() ) == FALSE )
	{
		return;
	}

    pBitmapOld = cdcMem.SelectObject( &bmpMem );
	cdcMem.FillSolidRect( &rect, ::GetSysColor(COLOR_BTNFACE) );

    for( short nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
    {
        DrawOneButton( &cdcMem, nRow );
    }

    pDC->BitBlt( 0, 0, rect.Width(), rect.Height(),
				 &cdcMem, 0, 0, SRCCOPY );

    cdcMem.SelectObject( pBitmapOld );
}


BEGIN_MESSAGE_MAP(CVarChoicesRowsBtn, CButton)
	//{{AFX_MSG_MAP(CVarChoicesRowsBtn)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn message handlers

/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn::OnLButtonDown

void CVarChoicesRowsBtn::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pVarChoicesDlg != NULL );
	ASSERT( m_pVarChoicesDlg->m_pVarChoices != NULL );

	short nRow = m_pVarChoicesDlg->FindRow( point.y );

	if( nRow != -1 )
	{
		// First set capture
		m_pVarChoicesDlg->CaptureMouse( this );

		// Select this row
		if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
		{
			// CTRL key is down - toggle select state of this row
			// See how many rows are selected
			short nNbrSelectedRows = 0;

			for( short nTheRow = 0 ;  nTheRow < NBR_VARIATIONS ;  nTheRow++ )
			{
				if( m_pVarChoicesDlg->IsRowSelected( nTheRow ) )
				{
					nNbrSelectedRows++;
				}
			}

			// If we are selecting the first row unselect all other buttons
			if( nNbrSelectedRows == 0 )
			{
				m_pVarChoicesDlg->UnselectAllButtons( TRUE );
			}

			if( m_pVarChoicesDlg->IsRowSelected( nRow ) )
			{
				m_pVarChoicesDlg->SelectRow( nRow, FALSE );
			}
			else
			{
				m_pVarChoicesDlg->SelectRow( nRow, TRUE );
			}
		}
		else if( GetAsyncKeyState(VK_SHIFT) & 0x8000 )
		{
			// SHIFT key is down
			if( sg_nFirstSelectedRow == -1 )
			{
				sg_nFirstSelectedRow = nRow;
			}

			int nFirstRow = min( sg_nFirstSelectedRow, nRow ); 
			int nLastRow = max( sg_nFirstSelectedRow, nRow ); 

			for( int i = 0 ;  i < NBR_VARIATIONS ;  i++ )
			{
				if( i >= nFirstRow
				&&  i <= nLastRow )
				{
					// Select rows inside the range
					m_pVarChoicesDlg->SelectRow( i, TRUE );
				}
				else
				{
					// Unselect rows outside of range
					m_pVarChoicesDlg->SelectRow( i, FALSE );
				}
			}
		}
		else
		{
			short nNbrSelectedRows = 0;

			if( m_pVarChoicesDlg->IsRowSelected( nRow ) )
			{
				for( short nTheRow = 0 ;  nTheRow < NBR_VARIATIONS ;  nTheRow++ )
				{
					if( m_pVarChoicesDlg->IsRowSelected( nTheRow ) )
					{
						nNbrSelectedRows++;
					}
				}
			}

			// Unselect all buttons
			m_pVarChoicesDlg->UnselectAllButtons( TRUE );

			if( nNbrSelectedRows != 1 )
			{
				sg_nFirstSelectedRow = nRow;
				m_pVarChoicesDlg->SelectRow( sg_nFirstSelectedRow, TRUE );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn::OnLButtonDblClk

void CVarChoicesRowsBtn::OnLButtonDblClk( UINT nFlags, CPoint point ) 
{
	OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn::OnLButtonUp

void CVarChoicesRowsBtn::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pVarChoicesDlg != NULL );

	if( CWnd::GetCapture() == this )
	{
		m_pVarChoicesDlg->ReleaseMouse( this );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn::OnRButtonDown

void CVarChoicesRowsBtn::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pVarChoicesDlg != NULL );
	ASSERT( m_pVarChoicesDlg->m_pVarChoices != NULL );

	short nRow = m_pVarChoicesDlg->FindRow( point.y );

	if( nRow != -1 )
	{
		// Select this row
		if( !(GetAsyncKeyState(VK_CONTROL) & 0x8000)
		&&  !(GetAsyncKeyState(VK_SHIFT) & 0x8000) )
		{
			// If this row is selected don't do anything
			if( m_pVarChoicesDlg->IsRowSelected( nRow ) == FALSE )
			{
				// Unselect all buttons
				m_pVarChoicesDlg->UnselectAllButtons( TRUE );

				sg_nFirstSelectedRow = nRow;
				m_pVarChoicesDlg->SelectRow( sg_nFirstSelectedRow, TRUE );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn::OnRButtonDblClk

void CVarChoicesRowsBtn::OnRButtonDblClk( UINT nFlags, CPoint point ) 
{
	OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn::OnMouseMove

void CVarChoicesRowsBtn::OnMouseMove( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pVarChoicesDlg != NULL );

	if( CWnd::GetCapture() == this )
	{
		short nRow = m_pVarChoicesDlg->FindRow( point.y );

		if( nRow != -1 )
		{
			// Select this row
			m_pVarChoicesDlg->SelectRow( nRow, TRUE );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn::OnEraseBkgnd

BOOL CVarChoicesRowsBtn::OnEraseBkgnd( CDC* pDC ) 
{
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesRowsBtn::DrawItem

void CVarChoicesRowsBtn::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC )
	{
	    DrawAllButtons( pDC );
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg

IMPLEMENT_DYNCREATE(CVarChoicesDlg, CFormView)

CVarChoicesDlg::CVarChoicesDlg()
	: CFormView(CVarChoicesDlg::IDD)
{
	//{{AFX_DATA_INIT(CVarChoicesDlg)
	//}}AFX_DATA_INIT

	m_pVarChoicesCtrl = NULL;
	m_pVarChoices = NULL;

	m_fRowRightMenu = FALSE;
	m_pFont = NULL;
	m_pFontBold = NULL;
	m_pFontItalics = NULL;
	m_pFontItalicsBold = NULL;
	m_fDirty = false;
}

CVarChoicesDlg::~CVarChoicesDlg()
{
}

void CVarChoicesDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVarChoicesDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVarChoicesDlg, CFormView)
	//{{AFX_MSG_MAP(CVarChoicesDlg)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg diagnostics

#ifdef _DEBUG
void CVarChoicesDlg::AssertValid() const
{
	CFormView::AssertValid();
}

void CVarChoicesDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::Create

BOOL CVarChoicesDlg::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext ) 
{
	// Create normal Font
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

	// Create bold Font
	m_pFontBold = new CFont;
	if( m_pFontBold == NULL )
	{
		return FALSE;
	}
	if( m_pFontBold->CreateFont( 10, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
 							   DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
							   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) == FALSE )
	{
		return FALSE;
	}

	// Create italics Font
	m_pFontItalics = new CFont;
	if( m_pFontItalics == NULL )
	{
		return FALSE;
	}
	if( m_pFontItalics->CreateFont( 10, 0, 0, 0, FW_NORMAL, TRUE, 0, 0,
 							   DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
							   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) == FALSE )
	{
		return FALSE;
	}

	// Create italics bold Font
	m_pFontItalicsBold = new CFont;
	if( m_pFontItalicsBold == NULL )
	{
		return FALSE;
	}
	if( m_pFontItalicsBold->CreateFont( 10, 0, 0, 0, FW_SEMIBOLD, TRUE, 0, 0,
 							   DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
							   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) == FALSE )
	{
		return FALSE;
	}

	if( !CFormView::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext ) )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::OnInitialUpdate

void CVarChoicesDlg::OnInitialUpdate() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pVarChoices != NULL );
	ASSERT( m_pFont != NULL );

	CFormView::OnInitialUpdate();

	// Subclass controls
	m_btnRows.SubclassDlgItem( IDC_SELECT_ROWS, this );
	m_btnRows.m_pVarChoicesDlg = this;

	m_btnFlags.SubclassDlgItem( IDC_FLAGS, this );
	m_btnFlags.m_pVarChoicesDlg = this;

	// Resize controls
	CWnd* pWnd = GetDlgItem( IDC_ROW_HEADINGS );
	if( pWnd )
	{
	    pWnd->MoveWindow( 6, 16, 22, 576, FALSE );
	}
	m_btnRows.MoveWindow( 30, 16, 12, 576, FALSE );
	m_btnFlags.MoveWindow( 42, 16, 760, 576, FALSE );

	// Set static variables
	if( sg_nSmallBtnWidth == 0 )
	{
		CRect rect;

		sg_nSmallBtnWidth = 20;
		sg_nMediumBtnWidth = sg_nSmallBtnWidth * 2;
		sg_nLargeBtnWidth = sg_nSmallBtnWidth * 3;

		m_btnFlags.GetClientRect( &rect );
		sg_nBtnHeight = rect.Height() / NBR_VARIATIONS;

		// Create button bitmaps
		CreateBtnBitmaps();

		// Prepare the arrays used to manage the buttons
		PrepareXPosArray();
		PrepareTextArrays();
	}

	// Size column headings
	PrepareColumnHeadings();
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::OnDestroy

void CVarChoicesDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Set m_hWndEditor to NULL to avoid recirsive call to OnDestroy()
	ASSERT( m_pVarChoices != NULL );
	if( m_pVarChoices )
	{
		m_pVarChoices->SetEditorWindow( NULL );
	}

	// Delete normal font
	if( m_pFont )
	{
		m_pFont->DeleteObject();
		delete m_pFont;
		m_pFont = NULL;
	}

	// Delete bold font
	if( m_pFontBold )
	{
		m_pFontBold->DeleteObject();
		delete m_pFontBold;
		m_pFontBold = NULL;
	}

	// Delete italics font
	if( m_pFontItalics )
	{
		m_pFontItalics->DeleteObject();
		delete m_pFontItalics;
		m_pFontItalics = NULL;
	}

	// Delete italics bold font
	if( m_pFontItalicsBold )
	{
		m_pFontItalicsBold->DeleteObject();
		delete m_pFontItalicsBold;
		m_pFontItalicsBold = NULL;
	}

	CFormView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::CreateBtnBitmaps

BOOL CVarChoicesDlg::CreateBtnBitmaps() 
{
	CDC* pDC;
	CDC cdcMem;
	CBitmap* pBitmapOld;
	CRect rect;
	BOOL fResult = FALSE;

	int nBtnWidth;
	int nOffset;

	pDC = GetDC();
	if( pDC )
	{
		if( cdcMem.CreateCompatibleDC( pDC ) )
		{
			CDC cdcMono;
			CDC cdcBackground;
			CBitmap bmpDisabledMono;
			CBitmap bmpDisabled;
			CBitmap bmpSelectedMono;
			CBitmap bmpSelected;
			BOOL fBackgroundBmp = FALSE;

			// Prepare background bitmaps
			if( bmpDisabled.LoadBitmap( IDB_DISABLED )
			&&  bmpSelected.LoadBitmap( IDB_SELECTED ) )
			{
				BITMAP bmDisabled;
				BITMAP bmSelected;

				bmpDisabled.GetBitmap( &bmDisabled );
				bmpSelected.GetBitmap( &bmSelected );

				if( cdcBackground.CreateCompatibleDC( pDC )
				&&  cdcMono.CreateCompatibleDC( pDC )
				&&  bmpDisabledMono.CreateBitmap( bmDisabled.bmWidth, bmDisabled.bmHeight, 1, 1, NULL )
				&&  bmpSelectedMono.CreateBitmap( bmSelected.bmWidth, bmSelected.bmHeight, 1, 1, NULL ) )
				{
					fBackgroundBmp = TRUE;
				}
			}

			// Create flag buttons
			for( int i = 0 ;  i < NBR_BTN_BITMAPS ;  i++ )
			{	
				// Determine button width
				if( i >= BTN_LG_BIT_ON )
				{
					nBtnWidth = sg_nLargeBtnWidth + 1;
				}
				else if( i >= BTN_MD_BIT_ON )
				{
					nBtnWidth = sg_nMediumBtnWidth + 1;
				}
				else
				{
					nBtnWidth = sg_nSmallBtnWidth + 1;
				}

				if( abmpButton[i].CreateCompatibleBitmap( pDC, nBtnWidth, sg_nBtnHeight ) )
				{
					abmpButton[i].SetBitmapDimension( nBtnWidth, sg_nBtnHeight );
					pBitmapOld = cdcMem.SelectObject( &abmpButton[i] );

					rect.left = 0;
					rect.top = 0;
					rect.right = nBtnWidth;
					rect.bottom = sg_nBtnHeight;

					// Draw black frame
					cdcMem.FrameRect( &rect, CBrush::FromHandle((HBRUSH)::GetStockObject(BLACK_BRUSH)) );
					rect.InflateRect( -1, -1 );

					int nMod = i % 6;	// Six different button states

					if( nMod == BIT_ON_SEL
					||  nMod == BIT_OFF_SEL )
					{
						// Selected
						cdcMem.FillSolidRect( &rect, RGB(255,0,0) );
					}
					else
					{
						// Not selected
						cdcMem.FillSolidRect( &rect, ::GetSysColor(COLOR_BTNFACE) );
					}

					if( nMod == BIT_ON
					||  nMod == BIT_ON_SEL
					||  nMod == BIT_ON_DIS )
					{
						// Sunken
						cdcMem.Draw3dRect( &rect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT) );
						rect.InflateRect( -1, -1 );
						cdcMem.Draw3dRect( &rect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT) );
						rect.InflateRect( -1, -1 );
					}
					else
					{
						// Raised
						cdcMem.Draw3dRect( &rect, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNSHADOW) );
						rect.InflateRect( -1, -1 );
						cdcMem.Draw3dRect( &rect, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNSHADOW) );
						rect.InflateRect( -1, -1 );
					}

					if( nMod == BIT_ON_DIS
					||  nMod == BIT_OFF_DIS )
					{
						if( fBackgroundBmp )
						{

							CBitmap* pbmpOld = cdcBackground.SelectObject( &bmpDisabled );
							CBitmap* pbmpOldMono = cdcMono.SelectObject( &bmpDisabledMono );
							
							cdcBackground.SetBkColor( RGB(255,0,255) );
							cdcMem.SetBkColor( RGB(255,255,255) );
							rect.right--;
							rect.bottom--;

							if( nMod == BIT_ON_DIS )
							{
								// Raised
								nOffset = 4;
							}
							else
							{
								// Raised
								nOffset = 3;
							}

							cdcMono.BitBlt( 0, 0, rect.Width(), rect.Height(),
											&cdcBackground, 0, 0, SRCCOPY);
							cdcMem.BitBlt( nOffset, nOffset, rect.Width(), rect.Height(),
											&cdcBackground, 0, 0, SRCINVERT ) ;
							cdcMem.BitBlt( nOffset, nOffset, rect.Width(), rect.Height(),
											&cdcMono, 0, 0, SRCAND ) ;
							cdcMem.BitBlt( nOffset, nOffset, rect.Width(), rect.Height(),
											&cdcBackground, 0, 0, SRCINVERT ) ;

							cdcMono.SelectObject( pbmpOldMono ) ;
							cdcBackground.SelectObject( pbmpOld );
						}
						else
						{
							cdcMem.FillRect( &rect, CBrush::FromHandle((HBRUSH)::GetStockObject(DKGRAY_BRUSH)) );
						}
					}

					if( nMod == BIT_ON )
					{
						if( fBackgroundBmp )
						{

							CBitmap* pbmpOld = cdcBackground.SelectObject( &bmpSelected );
							CBitmap* pbmpOldMono = cdcMono.SelectObject( &bmpSelectedMono );
							
							cdcBackground.SetBkColor( RGB(255,0,255) );
							cdcMem.SetBkColor( RGB(255,255,255) );
							rect.right--;
							rect.bottom--;

							// Sunken
							nOffset = 4;

							cdcMono.BitBlt( 0, 0, rect.Width(), rect.Height(),
											&cdcBackground, 0, 0, SRCCOPY);
							cdcMem.BitBlt( nOffset, nOffset, rect.Width(), rect.Height(),
											&cdcBackground, 0, 0, SRCINVERT ) ;
							cdcMem.BitBlt( nOffset, nOffset, rect.Width(), rect.Height(),
											&cdcMono, 0, 0, SRCAND ) ;
							cdcMem.BitBlt( nOffset, nOffset, rect.Width(), rect.Height(),
											&cdcBackground, 0, 0, SRCINVERT ) ;

							cdcMono.SelectObject( pbmpOldMono ) ;
							cdcBackground.SelectObject( pbmpOld );
						}
						else
						{
							cdcMem.FillRect( &rect, CBrush::FromHandle((HBRUSH)::GetStockObject(WHITE_BRUSH)) );
						}
					}

					cdcMem.SelectObject( pBitmapOld );
				}
			}

			m_btnRows.GetClientRect( &rect );
			int nSelWidth = rect.Width() - 1;
			
			if( sg_bmpRowOnUp.CreateCompatibleBitmap( pDC, nSelWidth, sg_nBtnHeight )
			&&  sg_bmpRowOnDown.CreateCompatibleBitmap( pDC, nSelWidth, sg_nBtnHeight ) )
			{

				// Create enabled "up" row selector
				cdcMem.SelectObject( &sg_bmpRowOnUp );
				rect.left = 0;
				rect.top = 0;
				rect.right = nSelWidth;
				rect.bottom = sg_nBtnHeight;
				cdcMem.FrameRect( &rect, CBrush::FromHandle((HBRUSH)::GetStockObject(BLACK_BRUSH)) );
				rect.InflateRect( -1, -1 );
				cdcMem.Draw3dRect( &rect, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNSHADOW) );
				rect.InflateRect( -1, -1 );
				cdcMem.Draw3dRect( &rect, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNSHADOW) );
				rect.InflateRect( -1, -1 );
				cdcMem.FillSolidRect( &rect, ::GetSysColor(COLOR_BTNFACE) );

				// Create enabled "down" row selector
				cdcMem.SelectObject( &sg_bmpRowOnDown );
				rect.left = 0;
				rect.top = 0;
				rect.right = nSelWidth;
				rect.bottom = sg_nBtnHeight;
				cdcMem.FrameRect( &rect, CBrush::FromHandle((HBRUSH)::GetStockObject(BLACK_BRUSH)) );
				rect.InflateRect( -1, -1 );
				cdcMem.Draw3dRect( &rect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT) );
				rect.InflateRect( -1, -1 );
				cdcMem.Draw3dRect( &rect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT) );
				rect.InflateRect( -1, -1 );
				cdcMem.FillSolidRect( &rect, RGB(255,0,0) );
			}
		}
		
		ReleaseDC( pDC );
	}

	return fResult;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::PrepareXPosArray

void CVarChoicesDlg::PrepareXPosArray( void )
{
	int i, j;

	// Initialized already to these values
	//sga_nBtnXPos[IMA_ROWTYPE] = &sga_IMA_XPos[0];
	//sga_nBtnXPos[DM_ROWTYPE] = &sga_DM_XPos[0];

	// Set X positions for IMA row of buttons
	sga_IMA_XPos[0] = 0;

	for( i = 0, j = 1 ;  j < IMA_NBR_COLUMNS ;  i++, j++ )
	{
		switch( sga_IMA_AssociatedBmp[i] )
		{
			case SMALL_BTN:
				sga_IMA_XPos[j] = sga_IMA_XPos[i] + sg_nSmallBtnWidth;
				break;

			case MEDIUM_BTN:
				sga_IMA_XPos[j] = sga_IMA_XPos[i] + sg_nMediumBtnWidth;
				break;

			case LARGE_BTN:
				sga_IMA_XPos[j] = sga_IMA_XPos[i] + sg_nLargeBtnWidth;
				break;
		}

		// Add extra pixel to separate groups (i.e. function, root, type, dest)
		if( i == 6		// Last "function" button
		||  i == 10		// Last "root" button
		||  i == 12 )	// Last "type" button
		{
			sga_IMA_XPos[j] += 2;
		}
	}

	// Set X positions for DirectMusic row of buttons
	sga_DM_XPos[0] = 0;

	for( i = 0, j = 1 ;  j < DM_NBR_COLUMNS ;  i++, j++ )
	{
		switch( sga_DM_AssociatedBmp[i] )
		{
			case SMALL_BTN:
				sga_DM_XPos[j] = sga_DM_XPos[i] + sg_nSmallBtnWidth;
				break;

			case MEDIUM_BTN:
				sga_DM_XPos[j] = sga_DM_XPos[i] + sg_nMediumBtnWidth;
				break;

			case LARGE_BTN:
				sga_DM_XPos[j] = sga_DM_XPos[i] + sg_nLargeBtnWidth;
				break;
		}

		// Add extra pixel to separate groups (i.e. function, root, type, dest)
		if( i == 20		// Last "function" button
		||  i == 23		// Last "root" button
		||  i == 26 )	// Last "type" button
		{
			sga_DM_XPos[j] += 2;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::PrepareTextArrays

void CVarChoicesDlg::PrepareTextArrays( void )
{
	int i;

	// Fill "IMA" text array
	for( i = 0 ;  i < IMA_NBR_COLUMNS ;  i++ )
	{
		::LoadString( theApp.m_hInstance, i + IDS_IMA_FLAGS_TEXT_1, &sga_IMA_Text[i][0], SMALL_BUFFER );
	}

	// Fill "DM" text array
	for( i = 0 ;  i < DM_NBR_COLUMNS ;  i++ )
	{
		::LoadString( theApp.m_hInstance, i + IDS_DM_FLAGS_TEXT_1, &sga_DM_Text[i][0], SMALL_BUFFER );
	}
}



/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::PrepareColumnHeadings

void CVarChoicesDlg::PrepareColumnHeadings( void )
{
	CWnd* pWnd;
	CRect rect;
	int nX;
	int nCX;
	int nXOffset;

	m_btnFlags.GetClientRect( &rect );
	m_btnFlags.ClientToScreen( &rect );
	ScreenToClient( &rect );
	nXOffset = rect.left;

	pWnd = GetDlgItem( IDC_HEADING_1 );
	if( pWnd )
	{
		nX = nXOffset + sga_DM_XPos[0];
		nCX = sga_DM_XPos[21] - sga_DM_XPos[0];

		pWnd->GetClientRect( &rect );
	    pWnd->MoveWindow( nX, rect.top, nCX, rect.Height(), FALSE );
	}

	pWnd = GetDlgItem( IDC_HEADING_2 );
	if( pWnd )
	{
		nX = nXOffset + sga_DM_XPos[21];
		nCX = sga_DM_XPos[24] - sga_DM_XPos[21];

		pWnd->GetClientRect( &rect );
	    pWnd->MoveWindow( nX, rect.top, nCX, rect.Height(), FALSE );
	}

	pWnd = GetDlgItem( IDC_HEADING_3 );
	if( pWnd )
	{
		nX = nXOffset + sga_DM_XPos[24];
		nCX = sga_DM_XPos[27] - sga_DM_XPos[24];

		pWnd->GetClientRect( &rect );
	    pWnd->MoveWindow( nX, rect.top, nCX, rect.Height(), FALSE );
	}

	pWnd = GetDlgItem( IDC_HEADING_4 );
	if( pWnd )
	{
		nX = nXOffset + sga_DM_XPos[27];
		nCX = sga_DM_XPos[DM_NBR_COLUMNS - 1] - sga_DM_XPos[27] + 2;

		switch( sga_DM_AssociatedBmp[DM_NBR_COLUMNS - 1] )
		{
			case SMALL_BTN:
				nCX += sg_nSmallBtnWidth;
				break;

			case MEDIUM_BTN:
				nCX += sg_nMediumBtnWidth;
				break;

			case LARGE_BTN:
				nCX += sg_nLargeBtnWidth;
				break;
		}

		pWnd->GetClientRect( &rect );
	    pWnd->MoveWindow( nX, rect.top, nCX, rect.Height(), FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::SyncPattern

void CVarChoicesDlg::SyncPattern()
{
	ASSERT( m_pVarChoices != NULL );

	if( m_pVarChoices->m_pCallback )
	{
		m_pVarChoices->m_pCallback->OnDataChanged( (IPersistStream *)m_pVarChoices );
	}

	m_fDirty = false;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::RefreshRowOfButtons

void CVarChoicesDlg::RefreshRowOfButtons( short nRow )
{
	CDC* pDC = m_btnRows.GetDC();
	if( pDC )
	{
		m_btnRows.DrawOneButton( pDC, nRow );
		m_btnRows.ReleaseDC( pDC );
	}

	pDC = m_btnFlags.GetDC();
	if( pDC )
	{
		m_btnFlags.DrawRowOfButtons( pDC, nRow );
		m_btnFlags.ReleaseDC( pDC );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::GetRowType

short CVarChoicesDlg::GetRowType( short nRow )
{
	ASSERT( m_pVarChoices != NULL );

	return (short)((m_pVarChoices->m_dwVariationChoices[nRow] & DM_VF_MODE_BITS) >> 29);
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::GetNbrColumns

short CVarChoicesDlg::GetNbrColumns( short nRow )
{
	short nNbrColumns = 0;
	
	switch( GetRowType(nRow) )
	{
		case IMA_ROWTYPE:
			nNbrColumns = IMA_NBR_COLUMNS;
			break;

		case DM_ROWTYPE:
			nNbrColumns = DM_NBR_COLUMNS;
			break;
	}

	ASSERT( nNbrColumns != 0 );
	return nNbrColumns;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::GetBtnWidth

short CVarChoicesDlg::GetBtnWidth( short nRow, short nColumn )
{
	short nBtnWidth = 0;
	
	switch( GetRowType(nRow) )
	{
		case IMA_ROWTYPE:
			switch( sga_IMA_AssociatedBmp[nColumn] )
			{
				case SMALL_BTN:
					nBtnWidth = sg_nSmallBtnWidth;
					break;

				case MEDIUM_BTN:
					nBtnWidth = sg_nMediumBtnWidth;
					break;

				case LARGE_BTN:
					nBtnWidth = sg_nLargeBtnWidth;
					break;
			}
			break;

		case DM_ROWTYPE:
			switch( sga_DM_AssociatedBmp[nColumn] )
			{
				case SMALL_BTN:
					nBtnWidth = sg_nSmallBtnWidth;
					break;

				case MEDIUM_BTN:
					nBtnWidth = sg_nMediumBtnWidth;
					break;

				case LARGE_BTN:
					nBtnWidth = sg_nLargeBtnWidth;
					break;
			}
			break;
	}

	ASSERT( nBtnWidth != 0 );
	return nBtnWidth;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::FindRow

short CVarChoicesDlg::FindRow( int nYPos )
{
	short nRow;

	if( nYPos < 0 )
	{
		return -1;
	}

	nRow = nYPos / sg_nBtnHeight;

	if( nRow >= NBR_VARIATIONS )
	{
		return -1;
	}

	return nRow;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::FindColumn

short CVarChoicesDlg::FindColumn( short nRow, int nXPos )
{
	if( nXPos < 0 )
	{
		return -1;
	}
   
	short nRowType = GetRowType( nRow );
	short nNbrColumns = GetNbrColumns( nRow );

	CRect rectBtn;

	for( int i = 0 ;  i < nNbrColumns ;  i++ )
	{
	    rectBtn.left = sga_nBtnXPos[nRowType][i];
		rectBtn.right = rectBtn.left + GetBtnWidth( nRow, i );

		if( nXPos <= rectBtn.right )
		{
			return i;
		}
	}

	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::SetBit

void CVarChoicesDlg::SetBit( short nRow, short nColumn )
{
	ASSERT( sg_nSetBitFlagState != -1 );
	ASSERT( m_pVarChoices != NULL );

	// Determine bit offset
	short nRowType = GetRowType( nRow );
	DWORD dwOffset = sga_nBtnBit[nRowType][nColumn];

	BOOL fUpdated = FALSE;

	// Set the bit
	if( IsBitOn( nRow, nColumn ) )
	{
		if( sg_nSetBitFlagState == FALSE )
		{
			short nThisRowType;
			BOOL fAllDisabled = TRUE;

			// Cannot disable all variations
			for( short nThisRow = 0 ;  nThisRow < NBR_VARIATIONS ;  nThisRow++ )
			{
				if( nThisRow == nRow )
				{
					DWORD dwVariationChoices = m_pVarChoices->m_dwVariationChoices[nRow] & ~(1 << dwOffset);

					switch( nRowType )
					{
						case IMA_ROWTYPE:
							if( (dwVariationChoices & IMA_VF_FLAG_BITS) != 0 )
							{
								fAllDisabled = FALSE;
							}
							break;

						case DM_ROWTYPE:
							if( (dwVariationChoices & DM_VF_FLAG_BITS) != 0 )
							{
								fAllDisabled = FALSE;
							}
							break;

						default:
							ASSERT( 0 );
					}
				}
				else
				{
					nThisRowType = GetRowType( nThisRow );
					switch( nThisRowType )
					{
						case IMA_ROWTYPE:
							if( (m_pVarChoices->m_dwVariationChoices[nThisRow] & IMA_VF_FLAG_BITS) != 0 )
							{
								fAllDisabled = FALSE;
							}
							break;

						case DM_ROWTYPE:
							if( (m_pVarChoices->m_dwVariationChoices[nThisRow] & DM_VF_FLAG_BITS) != 0 )
							{
								fAllDisabled = FALSE;
							}
							break;

						default:
							ASSERT( 0 );
					}
				}

				if( fAllDisabled == FALSE )
				{
					break;
				}
			}

			if( fAllDisabled == FALSE )
			{
				m_pVarChoices->m_nUndoText = IDS_UNDO_PATTERN_VARCHOICES;
				m_pVarChoices->m_dwVariationChoices[nRow] &= ~(1 << dwOffset);
				fUpdated = TRUE;
			}
		}
	}
	else
	{
		if( sg_nSetBitFlagState == TRUE )
		{
			m_pVarChoices->m_nUndoText = IDS_UNDO_PATTERN_VARCHOICES;
			m_pVarChoices->m_dwVariationChoices[nRow] |= (1 << dwOffset);
			fUpdated = TRUE;
		}
	}

	if( fUpdated )
	{
		// Redraw the button
		CDC* pDC = m_btnFlags.GetDC();
		if( pDC )
		{
			m_btnFlags.DrawRowOfButtons( pDC, nRow );
			m_btnFlags.ReleaseDC( pDC );
		}

		m_fDirty = true;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::SetSelectedBits

void CVarChoicesDlg::SetSelectedBits( short nNewState )
{
	short nRowType;
    short nNbrColumns;
	DWORD dwOffset;

	ASSERT( m_pVarChoices != NULL );

	short nFirstRow = -1;
	DWORD dwFirstRowVarChoices = 0;

	BOOL fStateSaved = FALSE;

	for( short nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
	{
		nRowType = GetRowType( nRow );
		nNbrColumns = GetNbrColumns( nRow );

		for( short nColumn = 0 ; nColumn < nNbrColumns ;  nColumn++ )
		{
			// Only attempt to change selected buttons
			if( IsBtnSelected( nRow, nColumn ) )
			{
				// Determine bit offset
				dwOffset = sga_nBtnBit[nRowType][nColumn];

				// Set the bit
				if( IsBitOn( nRow, nColumn ) )
				{
					if( nNewState == FALSE )
					{
						if( fStateSaved == FALSE )
						{
							m_pVarChoices->m_nUndoText = IDS_UNDO_PATTERN_VARCHOICES_DISABLE;
							
							nFirstRow = nRow;
							dwFirstRowVarChoices = m_pVarChoices->m_dwVariationChoices[nRow];
							fStateSaved = TRUE;
						}
						m_pVarChoices->m_dwVariationChoices[nRow] &= ~(1 << dwOffset);
						m_fDirty = true;
					}
				}
				else
				{
					if( nNewState == TRUE )
					{
						if( fStateSaved == FALSE )
						{
							m_pVarChoices->m_nUndoText = IDS_UNDO_PATTERN_VARCHOICES_ENABLE;
							
							nFirstRow = nRow;
							dwFirstRowVarChoices = m_pVarChoices->m_dwVariationChoices[nRow];
							fStateSaved = TRUE;
						}
						m_pVarChoices->m_dwVariationChoices[nRow] |= (1 << dwOffset);
						m_fDirty = true;
					}
				}
			}
		}
	}

	// Cannot disable all variations
	if( nNewState == FALSE
	&&  nFirstRow != -1 )
	{
		BOOL fAllDisabled = TRUE;

		for( short nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
		{
			nRowType = GetRowType( nRow );

			switch( nRowType )
			{
				case IMA_ROWTYPE:
					if( (m_pVarChoices->m_dwVariationChoices[nRow] & IMA_VF_FLAG_BITS) != 0 )
					{
						fAllDisabled = FALSE;
					}
					break;

				case DM_ROWTYPE:
					if( (m_pVarChoices->m_dwVariationChoices[nRow] & DM_VF_FLAG_BITS) != 0 )
					{
						fAllDisabled = FALSE;
					}
					break;

				default:
					ASSERT( 0 );
			}

			if( fAllDisabled ==  FALSE )
			{
				break;
			}
		}

		if( fAllDisabled == TRUE )
		{
			m_pVarChoices->m_dwVariationChoices[nFirstRow] = dwFirstRowVarChoices;
		}
	}	
	
	RefreshControls();
	if( m_fDirty )
	{
		SyncPattern();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::SelectRow

void CVarChoicesDlg::SelectRow( short nRow, BOOL fSelect )
{
	ASSERT( m_pVarChoices != NULL );

	if( nRow != -1 )
	{
		if( fSelect )
		{
			m_pVarChoices->m_bSelectedRowBtns[nRow] = 1;
			m_pVarChoices->m_dwSelectedFlagBtns[nRow] = 0xFFFFFFFF;
		}
		else
		{
			m_pVarChoices->m_bSelectedRowBtns[nRow] = 0;
			m_pVarChoices->m_dwSelectedFlagBtns[nRow] = 0x00000000;
		}

		RefreshRowOfButtons( nRow );
	}
}



/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::UnselectAllButtons
 
void CVarChoicesDlg::UnselectAllButtons( BOOL fRedraw )
{
	ASSERT( m_pVarChoices != NULL );

	short nRow;

	if( fRedraw )
	{
		short nColumn;
		short nNbrColumns;

		CDC* pDC = m_btnRows.GetDC();
		if( pDC )
		{
			for( nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
			{
				if( IsRowSelected( nRow ) )
				{
					m_pVarChoices->m_bSelectedRowBtns[nRow] = 0;
					m_btnRows.DrawOneButton( pDC, nRow );
				}
			}

			m_btnRows.ReleaseDC( pDC );
		}

		pDC = m_btnFlags.GetDC();
		if( pDC )
		{
			for( nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
			{
				nNbrColumns = GetNbrColumns( nRow );

				for( nColumn = 0 ;  nColumn < nNbrColumns ; nColumn++ )
				{
					if( IsBtnSelected( nRow, nColumn ) )
					{
						m_pVarChoices->m_dwSelectedFlagBtns[nRow] &= ~(1 << nColumn);
						m_btnFlags.DrawOneButton( pDC, nRow, nColumn, TRUE );
					}
				}

				// Make sure all bits beyond nNbrColumns are turned off
				m_pVarChoices->m_dwSelectedFlagBtns[nRow] = 0x00000000;
			}

			m_btnFlags.ReleaseDC( pDC );
		}
	}
	else
	{
		for( nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
		{
			m_pVarChoices->m_bSelectedRowBtns[nRow] = 0;
			m_pVarChoices->m_dwSelectedFlagBtns[nRow] = 0x0000000;
		}
	}

	sg_nFirstSelectedRow = -1;
	sg_nFirstSelectedBtnRow = -1;
	sg_nFirstSelectedBtnColumn = -1;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::RowsAreSelected

BOOL CVarChoicesDlg::RowsAreSelected()
{
	ASSERT( m_pVarChoices != NULL );

	for( short nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
	{
		if( m_pVarChoices->m_bSelectedRowBtns[nRow] )
		{
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::BtnsAreSelected

BOOL CVarChoicesDlg::BtnsAreSelected()
{
	ASSERT( m_pVarChoices != NULL );

	for( short nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
	{
		if( m_pVarChoices->m_dwSelectedFlagBtns[nRow] )
		{
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::IsBtnSelected

BOOL CVarChoicesDlg::IsBtnSelected( short nRow, short nColumn )
{
	ASSERT( m_pVarChoices != NULL );

	if( m_pVarChoices->m_dwSelectedFlagBtns[nRow] & (1 << nColumn) )
	{
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::IsRowSelected

BOOL CVarChoicesDlg::IsRowSelected( short nRow )
{
	ASSERT( m_pVarChoices != NULL );

	if( m_pVarChoices->m_bSelectedRowBtns[nRow] )
	{
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::IsRowDisabled

BOOL CVarChoicesDlg::IsRowDisabled( short nRow )
{
	ASSERT( m_pVarChoices != NULL );

	short nRowType = GetRowType( nRow );
    short nNbrColumns = GetNbrColumns( nRow );
	DWORD dwOffset;

	for( short nColumn = 0 ;  nColumn < nNbrColumns ;  nColumn++ )
    {
		dwOffset = sga_nBtnBit[nRowType][nColumn];

		if( m_pVarChoices->m_dwVariationChoices[nRow] & (1 << dwOffset) )
		{
			return FALSE;
		}
    }

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::IsBitOn

BOOL CVarChoicesDlg::IsBitOn( short nRow, short nColumn )
{
	ASSERT( nRow != -1 );
	ASSERT( nColumn != -1 );
	ASSERT( m_pVarChoices != NULL );

	short nRowType = GetRowType( nRow );
	DWORD dwOffset = sga_nBtnBit[nRowType][nColumn];

	if( m_pVarChoices->m_dwVariationChoices[nRow] & (1 << dwOffset ) )
	{
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::OnUpdateEditCopy

void CVarChoicesDlg::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pVarChoices != NULL );

	if( m_pVarChoices->CanCopy() == S_OK )
	{
		pCmdUI->Enable( TRUE );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::OnEditCopy

void CVarChoicesDlg::OnEditCopy() 
{
	ASSERT( m_pVarChoices != NULL );

	if( m_pVarChoices->CanCopy() == S_OK )
	{
		IDataObject* pIDataObject;

		if( SUCCEEDED ( m_pVarChoices->CreateDataObject( &pIDataObject ) ) )
		{
			theApp.PutDataInClipboard( pIDataObject, m_pVarChoices );

			RELEASE( pIDataObject );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::OnUpdateEditPaste

void CVarChoicesDlg::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pVarChoices != NULL );

	if( RowsAreSelected() )
	{
		IDataObject* pIDataObject;

		// Get the IDataObject
		if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
		{
			// Determine if VarChoices editor can paste this object
			BOOL fWillSetReference;
			HRESULT hr = m_pVarChoices->CanPasteFromData( pIDataObject, &fWillSetReference );
			
			RELEASE( pIDataObject );
			
			if( hr == S_OK )
			{
				pCmdUI->Enable( TRUE );
				return;
			}
		}
	}
	
	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::OnEditPaste

void CVarChoicesDlg::OnEditPaste() 
{
	ASSERT( m_pVarChoices != NULL );

	IDataObject* pIDataObject;

	// Get the IDataObject
	if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
	{
		// Paste the data
		if( SUCCEEDED ( m_pVarChoices->PasteFromData( pIDataObject ) ) )
		{
			RefreshControls();
		}
		else
		{
			m_btnRows.InvalidateRect( NULL, FALSE );
			m_btnRows.UpdateWindow();

			m_btnFlags.InvalidateRect( NULL, FALSE );
			m_btnFlags.UpdateWindow();
		}

		RELEASE( pIDataObject );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::OnViewProperties

BOOL CVarChoicesDlg::OnViewProperties( void )
{
	IDMUSProdPropSheet* pIPropSheet;

	ASSERT( m_pVarChoices != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( FAILED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		return FALSE;
	}

	if( pIPropSheet->IsShowing() != S_OK )
	{
		RELEASE( pIPropSheet );
		return TRUE;
	}

	BOOL fSuccess = FALSE;

	if( SUCCEEDED ( m_pVarChoices->OnShowProperties() ) )
	{
		fSuccess = TRUE;
	}

	RELEASE( pIPropSheet );

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::OnDrawItem

void CVarChoicesDlg::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	switch( nIDCtl )
	{
		case IDC_HEADING_1:
		case IDC_HEADING_2:
		case IDC_HEADING_3:
		case IDC_HEADING_4:
		{
			CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
			if( pDC == NULL )
			{
				return;
			}

			CWnd* pWnd = GetDlgItem( nIDCtl );
			if( pWnd )
			{
				CPen pen1;
				CPen pen2;
				CPen* pPenOld = NULL;
				CString strText;
				CRect rect( &lpDrawItemStruct->rcItem );

				pWnd->GetWindowText( strText );
				strText.TrimRight();
				strText.TrimLeft();
				strText = _T(" ") + strText + _T(" ");

				pDC->FillSolidRect( &rect, ::GetSysColor(COLOR_BTNFACE) );

				if( pen1.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW) ) )
				{
					int nY = rect.Height() >> 1;

					pPenOld = pDC->SelectObject( &pen1 );
					pDC->MoveTo( 3, ++nY + 4 );
					pDC->LineTo( 3, nY );
					pDC->LineTo( (rect.Width() - 4), nY );
					pDC->LineTo( (rect.Width() - 4), nY + 5 );

					if( pen2.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT) ) )
					{
						pDC->SelectObject( &pen2 );
						pDC->MoveTo( 2, --nY + 4 );
						pDC->LineTo( 2, nY );
						pDC->LineTo( (rect.Width() - 5), nY );
						pDC->LineTo( (rect.Width() - 5), nY + 5 );
					}
				}

				pDC->SetTextColor( RGB(0,0,0) );
				pDC->DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX) );

				if( pPenOld )
				{
					pDC->SelectObject( pPenOld );
				}
			}
			return;
		}

		case IDC_ROW_HEADINGS:
		{
			CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
			if( pDC == NULL )
			{
				return;
			}

			CString strText;
			CRect rect( &lpDrawItemStruct->rcItem );

			pDC->FillSolidRect( &rect, ::GetSysColor(COLOR_BTNFACE) );
			pDC->SetTextColor( RGB(0,0,0) );

			rect.right -= 2;
		    rect.bottom = rect.top + sg_nBtnHeight;
			
			for( int i = 0 ;  i < NBR_VARIATIONS ;  i++ )
			{
				strText.Format( "%d", i + 1 );
				pDC->DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_RIGHT | DT_VCENTER | DT_NOPREFIX) );
				rect.top += sg_nBtnHeight;
			    rect.bottom = rect.top + sg_nBtnHeight;
			}
			return;
		}
	}
	
	CFormView::OnDrawItem( nIDCtl, lpDrawItemStruct );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::OnContextMenu

void CVarChoicesDlg::OnContextMenu( CWnd* pWnd, CPoint point ) 
{
	ASSERT( pWnd != NULL );
	ASSERT( m_pVarChoices != NULL );

	switch( pWnd->GetDlgCtrlID() )
	{
		case IDC_FLAGS:
		case IDC_SELECT_ROWS:
		{
			CMenu menu;

			if( menu.LoadMenu( IDM_VARCHOICES_RMENU ) )
			{
				CMenu* pMenuPopup = menu.GetSubMenu( 0 );
				if( pMenuPopup )
				{
					pMenuPopup->EnableMenuItem( IDM_PROPERTIES, (MF_GRAYED | MF_BYCOMMAND) );

					if( BtnsAreSelected() )
					{
						pMenuPopup->EnableMenuItem( IDM_ENABLE, (MF_ENABLED | MF_BYCOMMAND) );
						pMenuPopup->EnableMenuItem( IDM_DISABLE, (MF_ENABLED| MF_BYCOMMAND) );
					}
					else
					{
						pMenuPopup->EnableMenuItem( IDM_ENABLE, (MF_GRAYED | MF_BYCOMMAND) );
						pMenuPopup->EnableMenuItem( IDM_DISABLE, (MF_GRAYED| MF_BYCOMMAND) );
					}

					if( RowsAreSelected() )
					{
						IDataObject* pIDataObject;

						pMenuPopup->EnableMenuItem( ID_EDIT_COPY, (MF_ENABLED | MF_BYCOMMAND) );
						pMenuPopup->EnableMenuItem( ID_EDIT_PASTE, (MF_GRAYED| MF_BYCOMMAND) );
						pMenuPopup->EnableMenuItem( IDM_IMA_MODE, (MF_ENABLED | MF_BYCOMMAND) );
						pMenuPopup->EnableMenuItem( IDM_DM_MODE, (MF_ENABLED | MF_BYCOMMAND) );

						// Get the IDataObject
						if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
						{
							// Determine if VarChoices editor can paste this object
							BOOL fWillSetReference;
							HRESULT hr = m_pVarChoices->CanPasteFromData( pIDataObject, &fWillSetReference );
							
							RELEASE( pIDataObject );
							
							if( hr == S_OK )
							{
								pMenuPopup->EnableMenuItem( ID_EDIT_PASTE, (MF_ENABLED | MF_BYCOMMAND) );
							}
						}

						short nFirstSelectedRow = -1;
						short nNbrSelectedRows = 0;

						for( short nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
						{
							if( IsRowSelected( nRow ) )
							{
								if( nFirstSelectedRow == -1 )
								{
									nFirstSelectedRow = nRow;
								}
								nNbrSelectedRows++;
							}
						}

						if( nNbrSelectedRows == 1 )
						{
							short nRowType = GetRowType( nFirstSelectedRow );
							
							UINT nCheck = (nRowType == IMA_ROWTYPE) ? MF_CHECKED : MF_UNCHECKED;
							pMenuPopup->CheckMenuItem( IDM_IMA_MODE, (nCheck | MF_BYCOMMAND) );

							nCheck = (nRowType == DM_ROWTYPE) ? MF_CHECKED : MF_UNCHECKED;
							pMenuPopup->CheckMenuItem( IDM_DM_MODE, (nCheck | MF_BYCOMMAND) );
						}
						else
						{
							pMenuPopup->CheckMenuItem( IDM_IMA_MODE, (MF_UNCHECKED | MF_BYCOMMAND) );
							pMenuPopup->CheckMenuItem( IDM_DM_MODE, (MF_UNCHECKED | MF_BYCOMMAND) );
						}
					}
					else
					{
						pMenuPopup->EnableMenuItem( ID_EDIT_COPY, (MF_GRAYED | MF_BYCOMMAND) );
						pMenuPopup->EnableMenuItem( ID_EDIT_PASTE, (MF_GRAYED | MF_BYCOMMAND) );
						pMenuPopup->CheckMenuItem( IDM_IMA_MODE, (MF_UNCHECKED | MF_BYCOMMAND) );
						pMenuPopup->EnableMenuItem( IDM_IMA_MODE, (MF_GRAYED | MF_BYCOMMAND) );
						pMenuPopup->CheckMenuItem( IDM_DM_MODE, (MF_UNCHECKED | MF_BYCOMMAND) );
						pMenuPopup->EnableMenuItem( IDM_DM_MODE, (MF_GRAYED | MF_BYCOMMAND) );
					}

					m_fRowRightMenu = TRUE;
					pMenuPopup->TrackPopupMenu( (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
												point.x, point.y, this, NULL );
				}
			}
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::OnCommand

BOOL CVarChoicesDlg::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	ASSERT( m_pVarChoices != NULL );

	if( HIWORD(wParam) == 0 )	// menu command
	{
		if( m_fRowRightMenu )
		{
			//CJazzUndoMan* pUndoMgr = m_pVarChoices->m_pPattern->m_pUndoMgr;

			switch( LOWORD(wParam) )
			{
				BOOL fStateSaved;
				short nRow;

				case IDM_ENABLE:
					SetSelectedBits( TRUE );
					break;

				case IDM_DISABLE:
					SetSelectedBits( FALSE );
					break;

				case ID_EDIT_COPY:
					OnEditCopy();
					break;

				case ID_EDIT_PASTE:
					OnEditPaste();
					break;

				case IDM_PROPERTIES:
					m_pVarChoices->OnShowProperties();
					break;

				case IDM_IMA_MODE:
					fStateSaved = FALSE;
					for( nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
					{
						if( IsRowSelected( nRow ) )
						{
							if( GetRowType( nRow ) != IMA_ROWTYPE )
							{
								if( fStateSaved == FALSE )
								{
									m_pVarChoices->m_nUndoText = IDS_UNDO_PATTERN_VARCHOICES_MODE;
									fStateSaved = TRUE;
								}
								m_pVarChoices->m_dwVariationChoices[nRow] = DM_VF_FLAG_BITS;
								m_fDirty = true;
							}
						}
					}
					RefreshControls();
					if( m_fDirty )
					{
						SyncPattern();
					}
					break;

				case IDM_DM_MODE:
					fStateSaved = FALSE;
					for( nRow = 0 ;  nRow < NBR_VARIATIONS ;  nRow++ )
					{
						if( IsRowSelected( nRow ) )
						{
							if( GetRowType( nRow ) != DM_ROWTYPE )
							{
								if( fStateSaved == FALSE )
								{
									m_pVarChoices->m_nUndoText = IDS_UNDO_PATTERN_VARCHOICES_MODE;
									fStateSaved = TRUE;
								}
								m_pVarChoices->m_dwVariationChoices[nRow] = DM_VF_FLAG_BITS;
								m_pVarChoices->m_dwVariationChoices[nRow] |= (DM_ROWTYPE << 29);
								m_fDirty = true;
							}
						}
					}
					RefreshControls();
					if( m_fDirty )
					{
						SyncPattern();
					}
					break;
			}

			m_fRowRightMenu = FALSE;
			return TRUE;
		}
	}
	
	return CFormView::OnCommand( wParam, lParam );
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::CaptureMouse

void CVarChoicesDlg::CaptureMouse( CWnd *pWnd ) 
{
	// First set capture
	pWnd->SetCapture();
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::ReleaseMouse

void CVarChoicesDlg::ReleaseMouse( CWnd *pWnd ) 
{
	// First release capture
	::ReleaseCapture();

	if( m_fDirty )
	{
		SyncPattern();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::RefreshControls

void CVarChoicesDlg::RefreshControls()
{
	ASSERT( m_pVarChoices != NULL );

	UnselectAllButtons( FALSE );

	m_btnRows.InvalidateRect( NULL, FALSE );
	m_btnRows.UpdateWindow();

	m_btnFlags.InvalidateRect( NULL, FALSE );
	m_btnFlags.UpdateWindow();
}


/////////////////////////////////////////////////////////////////////////////
// CVarChoicesDlg::HandleKeyDown

BOOL CVarChoicesDlg::HandleKeyDown( MSG* pMsg ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pVarChoices != NULL );

	switch( pMsg->message )
	{
		case WM_KEYDOWN:
			if( pMsg->lParam & 0x40000000 )
			{
				break;
			}

			switch( pMsg->wParam )
			{
				case 0x43:  // VK_C		(Copy)
					if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
					{
						OnEditCopy();
						return TRUE;
					}
					break;

				case 0x56:	// VK_V		(Paste)
					if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
					{
						OnEditPaste();
						return TRUE;
					}
					break;
			}
			break;
	}
	
	return FALSE;
}
