// QuantizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SequenceStripMgr.h"
#include <SegmentPrivate.h>
#include "QuantizeDlg.h"
#include <dmusici.h>
#include <ioDMStyle.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_GRIDS_PER_BEAT			24
#define MAX_GRIDS_PER_BEAT_ENTRIES	48

const UINT g_nGridsPerBeatBitmaps[MAX_GRIDS_PER_BEAT_ENTRIES] =
	{ 
	  IDB_GPB1,     IDB_GPB2,     IDB_GPB3,     IDB_GPB4,     IDB_GPB5,		// Beat = quarter note
	  IDB_GPB6,     IDB_GPB7,     IDB_GPB8,     IDB_GPB9,     IDB_GPB10,
	  IDB_GPB11,    IDB_GPB12,    IDB_GPB13,    IDB_GPB14,    IDB_GPB15,
	  IDB_GPB16,    IDB_GPB17,    IDB_GPB18,    IDB_GPB19,    IDB_GPB20,
	  IDB_GPB21,    IDB_GPB22,    IDB_GPB23,    IDB_GPB24,
	  IDB_GPB1_ALT, IDB_GPB2_ALT, IDB_GPB3_ALT, IDB_GPB4_ALT, IDB_GPB5,		// Beat != quarter note
	  IDB_GPB6_ALT, IDB_GPB7,     IDB_GPB8_ALT, IDB_GPB9,     IDB_GPB10,
	  IDB_GPB11,    IDB_GPB12,    IDB_GPB13,    IDB_GPB14,    IDB_GPB15,
	  IDB_GPB16,    IDB_GPB17,    IDB_GPB18,    IDB_GPB19,    IDB_GPB20,
	  IDB_GPB21,    IDB_GPB22,    IDB_GPB23,    IDB_GPB24
	};

/////////////////////////////////////////////////////////////////////////////
// CQuantizeDlg dialog


CQuantizeDlg::CQuantizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuantizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuantizeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_qtTarget = QUANTIZE_TARGET_PART;
	m_lResolution = 4;
	m_dwFlags = PATTERN_QUANTIZE_START_TIME;
	m_lStrength = 100;
	m_bBeat = 4;
	m_fEnableSelected = FALSE;
}


void CQuantizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuantizeDlg)
	DDX_Control(pDX, IDC_SPIN_STRENGTH, m_spinStrength);
	DDX_Control(pDX, IDC_GRID, m_comboGrid);
	DDX_Control(pDX, IDC_EDIT_STRENGTH, m_editStrength);
	DDX_Control(pDX, IDC_CHECK_STARTTIME, m_buttonStartTime);
	DDX_Control(pDX, IDC_CHECK_DURATION, m_buttonDuration);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuantizeDlg, CDialog)
	//{{AFX_MSG_MAP(CQuantizeDlg)
	ON_WM_DRAWITEM()
	ON_EN_KILLFOCUS(IDC_EDIT_STRENGTH, OnKillfocusEditStrength)
	ON_WM_MEASUREITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuantizeDlg message handlers

BOOL CQuantizeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Enable/Disable the Selected Notes radio button
	CWnd* pWnd;
	pWnd = GetDlgItem(IDC_RADIO_SELECTED_NOTES);
	if (pWnd)
	{
		pWnd->EnableWindow(m_fEnableSelected);
	}

	// Set the Quantize Target radio button
	switch( m_qtTarget )
	{
	case QUANTIZE_TARGET_SELECTED:
		ASSERT( m_fEnableSelected );
		CheckRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_SEQUENCE, IDC_RADIO_SELECTED_NOTES );
		break;
	case QUANTIZE_TARGET_PART:
		CheckRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_SEQUENCE, IDC_RADIO_PART );
		break;
	case QUANTIZE_TARGET_SEQUENCE:
		CheckRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_SEQUENCE, IDC_RADIO_SEQUENCE );
		break;
	default:
		break;
	}

	// Fill resolution combo box
	for( int i = MIN_GRIDS_PER_BEAT ;  i <= MAX_GRIDS_PER_BEAT ;  i++ )
	{
		m_comboGrid.AddString( _T("") );
	}

	// Set resolution
	m_comboGrid.SetCurSel( m_lResolution - 1 );
	
	// Set Start Time(s) check box
	if( m_dwFlags & PATTERN_QUANTIZE_START_TIME )
	{
		m_buttonStartTime.SetCheck( 1 );
	}
	else
	{
		m_buttonStartTime.SetCheck( 0 );
	}

	// Set Duration(s) check box
	if( m_dwFlags & PATTERN_QUANTIZE_DURATION )
	{
		m_buttonDuration.SetCheck( 1 );
	}
	else
	{
		m_buttonDuration.SetCheck( 0 );
	}

	// Initialize Strength edit box
	m_editStrength.SetLimitText( 3 );
	ASSERT( (m_lStrength >= 0) && (m_lStrength <= 100) );
	m_spinStrength.SetRange( 0, 100 );
	m_spinStrength.SetPos( m_lStrength );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CQuantizeDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch( nIDCtl )
	{
		case IDC_GRID:
		{
			if( lpDrawItemStruct->itemID == -1 )
			{
				return;
			}

			CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
			if( pDC == NULL )
			{
				return;
			}

            if( lpDrawItemStruct->itemAction & ODA_FOCUS )
			{
		        InflateRect( &lpDrawItemStruct->rcItem, -1, -1 ) ;
                pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
		        InflateRect( &lpDrawItemStruct->rcItem, 1, 1 ) ;
				return;
			}

            if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE
            ||  lpDrawItemStruct->itemAction & ODA_SELECT )
			{
				CBitmap bitmap;
				int nResourceID;

				// Get the resource id for the bitmap
				nResourceID = lpDrawItemStruct->itemID;
				if( m_bBeat != 4 )		// 4 = quarter note gets the beat
				{
					nResourceID += MAX_GRIDS_PER_BEAT;
				}
				ASSERT( (nResourceID >= 0) && (nResourceID <= MAX_GRIDS_PER_BEAT_ENTRIES) ); 

				// Load the bitmap
				HINSTANCE hInstance = AfxGetResourceHandle();
				AfxSetResourceHandle( AfxGetInstanceHandle() );

				if( bitmap.LoadBitmap( g_nGridsPerBeatBitmaps[nResourceID] ) )
				{
					CDC dcBitmap;

					if( dcBitmap.CreateCompatibleDC( pDC ) )
					{
						BITMAP bm;

						// Get the size of the bitmap
						bitmap.GetBitmap( &bm );

						// Clear the rectangle
						pDC->FillRect( &lpDrawItemStruct->rcItem,
									   CBrush::FromHandle( (HBRUSH)GetStockObject(WHITE_BRUSH) ) );

						// Paint the bitmap
						CBitmap* pOldBitmap = dcBitmap.SelectObject( &bitmap );
						pDC->BitBlt( lpDrawItemStruct->rcItem.left + 5, lpDrawItemStruct->rcItem.top + 1,
									 bm.bmWidth, bm.bmHeight,
									 &dcBitmap, 0, 0, SRCCOPY );
						dcBitmap.SelectObject( pOldBitmap );
					}
				}

				AfxSetResourceHandle( hInstance );

				// Invert if selected
                if( lpDrawItemStruct->itemState & ODS_SELECTED )
				{
					pDC->InvertRect( &lpDrawItemStruct->rcItem );
				}
			}

			return;
		}
	}
	
	CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
}

void CQuantizeDlg::OnOK() 
{
	// Set the Quantize Target
	switch( GetCheckedRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_SEQUENCE ) )
	{
	case IDC_RADIO_SELECTED_NOTES:
		m_qtTarget = QUANTIZE_TARGET_SELECTED;
		break;
	case IDC_RADIO_PART:
		m_qtTarget = QUANTIZE_TARGET_PART;
		break;
	case IDC_RADIO_SEQUENCE:
		m_qtTarget = QUANTIZE_TARGET_SEQUENCE;
		break;
	default:
		break;
	}
	
	// Get resolution
	if( m_comboGrid.GetCurSel( ) != CB_ERR )
	{
		m_lResolution = m_comboGrid.GetCurSel( ) + 1;
	}
	
	m_dwFlags = 0;

	// Get Start Time(s) check state
	if( m_buttonStartTime.GetCheck( ) )
	{
		m_dwFlags |= PATTERN_QUANTIZE_START_TIME;
	}

	// Get Duration check state
	if( m_buttonDuration.GetCheck( ) )
	{
		m_dwFlags |= PATTERN_QUANTIZE_DURATION;
	}

	// Get Strength
	m_lStrength = m_spinStrength.GetPos( );

	CDialog::OnOK();
}

void CQuantizeDlg::OnKillfocusEditStrength() 
{
	CString strNewStrength;

	m_editStrength.GetWindowText( strNewStrength );

	// Strip leading and trailing spaces
	strNewStrength.TrimRight();
	strNewStrength.TrimLeft();

	if( strNewStrength.IsEmpty() )
	{
		m_spinStrength.SetPos( m_lStrength );
	}
	else
	{
		int iNewStrength = _ttoi( strNewStrength );
		if( iNewStrength > 100 )
		{
			iNewStrength = 100;
			m_spinStrength.SetPos( iNewStrength );
		}
		else if( iNewStrength < 0 )
		{
			iNewStrength = 0;
			m_spinStrength.SetPos( iNewStrength );
		}
	}
}

void CQuantizeDlg::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	switch( nIDCtl )
	{
		case IDC_GRID:
		{
			// Load the bitmap
			HINSTANCE hInstance = AfxGetResourceHandle();
			AfxSetResourceHandle( AfxGetInstanceHandle() );

			CBitmap bitmap;
			if( bitmap.LoadBitmap( g_nGridsPerBeatBitmaps[0] ) )	// All bitmaps are the same size
			{
				// Get the size of the bitmap
				BITMAP bm;
				bitmap.GetBitmap( &bm );
				lpMeasureItemStruct->itemHeight = max( lpMeasureItemStruct->itemHeight, (UINT)bm.bmHeight + 5 );
			}

			AfxSetResourceHandle( hInstance );
			return;
		}
	}

	
	CDialog::OnMeasureItem( nIDCtl, lpMeasureItemStruct );
}
