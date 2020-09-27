// QuantizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MIDIStripMgr.h"
#include "Pattern.h"
#include "QuantizeDlg.h"
#include "MIDIStripMgrApp.h"
#include <dmusici.h>
#include <ioDMStyle.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	m_fEnableSelected = FALSE;

	m_TimeSignature.m_bBeatsPerMeasure = 4;
	m_TimeSignature.m_bBeat = 4;
	m_TimeSignature.m_wGridsPerBeat = 4;
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
		CheckRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_PATTERN, IDC_RADIO_SELECTED_NOTES );
		break;
	case QUANTIZE_TARGET_PART:
		CheckRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_PATTERN, IDC_RADIO_PART );
		break;
	case QUANTIZE_TARGET_PATTERN:
		CheckRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_PATTERN, IDC_RADIO_PATTERN );
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

				// Get the resource id for the bitmap
				int nResourceID = lpDrawItemStruct->itemID;
				if( m_TimeSignature.m_bBeat != 4 )		// 4 = quarter note gets the beat
				{
					nResourceID += MAX_GRIDS_PER_BEAT;
				}
				ASSERT( (nResourceID >= 0) && (nResourceID <= MAX_GRIDS_PER_BEAT_ENTRIES) ); 

				// Load the bitmap
				HINSTANCE hInstance = AfxGetResourceHandle();
				AfxSetResourceHandle( AfxGetInstanceHandle() );

				if( bitmap.LoadBitmap( g_nGridsPerBeatBitmaps[nResourceID]  ) )
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
	switch( GetCheckedRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_PATTERN ) )
	{
	case IDC_RADIO_SELECTED_NOTES:
		m_qtTarget = QUANTIZE_TARGET_SELECTED;
		break;
	case IDC_RADIO_PART:
		m_qtTarget = QUANTIZE_TARGET_PART;
		break;
	case IDC_RADIO_PATTERN:
		m_qtTarget = QUANTIZE_TARGET_PATTERN;
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
			if( bitmap.LoadBitmap( IDB_GPB1 ) )	// All bitmaps are the same size
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
