// TabPatternPattern.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Pattern.h"
#include "PatternLengthDlg.h"
#include "TimeSignatureDlg.h"
#include "TabPatternPattern.h"
#include "MIDIStripMgrApp.h"
#include "DMUSProd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

short *CTabPatternPattern::sm_pnActiveTab = NULL;

/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern property page

CTabPatternPattern::CTabPatternPattern( void ) : CPropertyPage(CTabPatternPattern::IDD)
{
	//{{AFX_DATA_INIT(CTabPatternPattern)
	//}}AFX_DATA_INIT
	
	m_pPattern = NULL;
	m_fNeedToDetach = FALSE;
	m_pIPropSheet = NULL;
}

CTabPatternPattern::~CTabPatternPattern()
{
	// Work around 27331
	m_pPattern = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::SetPattern

void CTabPatternPattern::SetPattern( CDirectMusicPattern* pPattern )
{
	m_pPattern = pPattern;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::EnableControls

void CTabPatternPattern::EnableControls( BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
	m_btnLength.EnableWindow( fEnable );
	m_btnTimeSignature.EnableWindow( fEnable );
	m_btnResetVarOrder.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::UpdateControls

void CTabPatternPattern::UpdateControls()
{
	// Make sure controls have been created
	if( ::IsWindow(m_editName.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editName.LimitText( DMUS_MAX_NAME );

	if( m_pPattern )
	{
		EnableControls( TRUE );

		// Set name
		m_editName.SetWindowText( m_pPattern->m_strName );
		
		// Set length
		CString strLength;
		strLength.Format( "%d", m_pPattern->m_wNbrMeasures );
		m_btnLength.SetWindowText( strLength );

		// Update bitmap on time signature button
		SetTimeSignatureBitmap();

		// Update the Reset Variation Order checkbox (the button state is the inverse of the flag setting)
		m_btnResetVarOrder.SetCheck( m_pPattern->m_dwFlags & DMUS_PATTERNF_PERSIST_CONTROL ? BST_UNCHECKED : BST_CHECKED );
	}
	else
	{
		m_editName.SetWindowText( _T("") );
		m_btnLength.SetWindowText( _T("") );
		m_btnResetVarOrder.SetCheck( BST_UNCHECKED );

		EnableControls( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::SetTimeSignatureBitmap

void CTabPatternPattern::SetTimeSignatureBitmap( void )
{
	if( !::IsWindow( m_btnTimeSignature.m_hWnd ) )
	{
		return;
	}

	HBITMAP hNewBits = NULL;

	ASSERT( m_pPattern != NULL );

	RECT rect;
	m_btnTimeSignature.GetClientRect( &rect );

	// Create a DC for the new bitmap
	// a DC for the 'Grids Per Beat' bitmap
	// a Bitmap for the new bits
	CDC cdcDest;
	CDC cdcGridsPerBeat;
	CBitmap bmpNewBits;
	CBitmap bmpGridsPerBeat;

	CDC* pDC = m_btnTimeSignature.GetDC();
	if( pDC )
	{

		if( cdcDest.CreateCompatibleDC( pDC ) == FALSE
		||  cdcGridsPerBeat.CreateCompatibleDC( pDC ) == FALSE
		||  bmpNewBits.CreateCompatibleBitmap( pDC, rect.right, rect.bottom ) == FALSE )
		{
			m_btnTimeSignature.ReleaseDC( pDC );
			return;
		}

		m_btnTimeSignature.ReleaseDC( pDC );
	}

	// Create the new bitmap
	CBitmap* pbmpOldMem = cdcDest.SelectObject( &bmpNewBits );

	// Fill Rect with button color
	cdcDest.SetBkColor( ::GetSysColor(COLOR_BTNFACE) );
	cdcDest.ExtTextOut( 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

	// Write text
	CString strTimeSignature;

	CFont font;
	CFont* pfontOld = NULL;
	
	if( font.CreateFont( 10, 0, 0, 0, FW_NORMAL, 0, 0, 0,
 						DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) )
	{
		pfontOld = cdcDest.SelectObject( &font );
	}

	strTimeSignature.Format( "%d/%d",
							 m_pPattern->m_TimeSignature.m_bBeatsPerMeasure,
							 m_pPattern->m_TimeSignature.m_bBeat );
	rect.left += 6;
	cdcDest.SetTextColor( COLOR_BTNTEXT );
	cdcDest.DrawText( strTimeSignature, -1, &rect, (DT_SINGLELINE | DT_LEFT | DT_VCENTER) );
	rect.left -= 6;

	if( pfontOld )
	{
		cdcDest.SelectObject( pfontOld );
		font.DeleteObject();
	}

	// Set x coord for 'Grids Per Beat' image
	CSize sizeText = cdcDest.GetTextExtent( strTimeSignature );
	int nX = max( 48, (sizeText.cx + 8) );

	// Draw "splitter"
	{
		CPen pen1;
		CPen pen2;
		CPen* ppenOld;

		int nPlace = nX - 6;
		int nModeOld = cdcDest.SetROP2( R2_COPYPEN );

		// Highlight
		if( pen1.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW) ) )
		{
			ppenOld = cdcDest.SelectObject( &pen1 );
			cdcDest.MoveTo( nPlace, (rect.top + 3) );
			cdcDest.LineTo( nPlace, (rect.bottom - 3) );
			cdcDest.SelectObject( ppenOld );
		}

		// Shadow
		if( pen2.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT) ) )
		{
			ppenOld = cdcDest.SelectObject( &pen2 );
			cdcDest.MoveTo( ++nPlace, (rect.top + 3) );
			cdcDest.LineTo( nPlace, (rect.bottom - 3) );
			cdcDest.SelectObject( ppenOld );
		}

		if( nModeOld )
		{
			cdcDest.SetROP2( nModeOld );
		}
	}

	// Add 'Grids Per Beat' bitmap
	{
		int nResourceID = m_pPattern->m_TimeSignature.m_wGridsPerBeat - 1;
		if( m_pPattern->m_TimeSignature.m_bBeat != 4 )		// 4 = quarter note gets the beat
		{
			nResourceID += MAX_GRIDS_PER_BEAT;
		}
		ASSERT( (nResourceID >= 0) && (nResourceID <= MAX_GRIDS_PER_BEAT_ENTRIES) );

		if( bmpGridsPerBeat.LoadBitmap( g_nGridsPerBeatBitmaps[nResourceID] ) )
		{
			BITMAP bm;

			bmpGridsPerBeat.GetBitmap( &bm );

			int nY = ((rect.bottom - rect.top) - bm.bmHeight) >> 1;

			CBitmap* pbmpOld = cdcGridsPerBeat.SelectObject( &bmpGridsPerBeat );

			{
				CDC cdcMono;
				CBitmap bmpMono;

				if( cdcMono.CreateCompatibleDC( &cdcDest )
				&&  bmpMono.CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL ) )
				{
					CBitmap* pbmpOldMono = cdcMono.SelectObject( &bmpMono );
					
					cdcGridsPerBeat.SetBkColor( RGB(255,255,255) );
					cdcDest.SetBkColor( RGB(255,255,255) );

					cdcMono.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight,
									&cdcGridsPerBeat, 0, 0, SRCCOPY);
					cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
									&cdcGridsPerBeat, 0, 0, SRCINVERT ) ;
					cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
									&cdcMono, 0, 0, SRCAND ) ;
					cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
									&cdcGridsPerBeat, 0, 0, SRCINVERT ) ;

					cdcMono.SelectObject( pbmpOldMono ) ;
				}
			}

			cdcGridsPerBeat.SelectObject( pbmpOld );
		}
	}

	cdcDest.SelectObject( pbmpOldMem );

	// Set the new bitmap
	hNewBits = (HBITMAP)bmpNewBits.Detach();
	if( hNewBits )
	{
		HBITMAP hBitmapOld = m_btnTimeSignature.SetBitmap( hNewBits );
		if( hBitmapOld )
		{
			::DeleteObject( hBitmapOld );
		}
	}
}


void CTabPatternPattern::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabPatternPattern)
	DDX_Control(pDX, IDC_CHECK_RESET_VAR_ORDER, m_btnResetVarOrder);
	DDX_Control(pDX, IDC_TIME_SIGNATURE, m_btnTimeSignature);
	DDX_Control(pDX, IDC_NAME, m_editName);
	DDX_Control(pDX, IDC_LENGTH, m_btnLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabPatternPattern, CPropertyPage)
	//{{AFX_MSG_MAP(CTabPatternPattern)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillFocusName)
	ON_BN_CLICKED(IDC_LENGTH, OnLength)
	ON_BN_CLICKED(IDC_TIME_SIGNATURE, OnTimeSignature)
	ON_BN_CLICKED(IDC_CHECK_RESET_VAR_ORDER, OnCheckResetVarOrder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnSetActive

BOOL CTabPatternPattern::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	UpdateControls();

	// Store active tab
	if( sm_pnActiveTab && m_pIPropSheet )
	{
		m_pIPropSheet->GetActivePage( sm_pnActiveTab );
	}

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnCreate

int CTabPatternPattern::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	if( !FromHandlePermanent( m_hWnd ) )
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		Attach( hWnd );
		m_fNeedToDetach = TRUE;
	}

	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnDestroy

void CTabPatternPattern::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Delete the time signature button's bitmap
	HBITMAP hBitmap = m_btnTimeSignature.GetBitmap();
	if( hBitmap )
	{
		::DeleteObject( hBitmap );
	}

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.
	if( m_fNeedToDetach && m_hWnd )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
	}

	CPropertyPage::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnKillFocusName

void CTabPatternPattern::OnKillFocusName() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPattern )
	{
		CString strName;

		m_editName.GetWindowText( strName );

		// Strip leading and trailing spaces
		strName.TrimRight();
		strName.TrimLeft();

		if( strName.IsEmpty() )
		{
			m_editName.SetWindowText( m_pPattern->m_strName );
		}
		else
		{
			if( strName.Compare( m_pPattern->m_strName ) != 0 )
			{
				m_pPattern->SetNodeName( strName );

				// TODO: Refresh property page name
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnLength

void CTabPatternPattern::OnLength() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPattern )
	{
		CPatternLengthDlg plDlg( m_pPattern );

		if( plDlg.DoModal() == IDOK )
		{
			CString strLength;

			if( ::IsWindow( m_btnLength.m_hWnd ) )
			{
				strLength.Format( "%d", m_pPattern->m_wNbrMeasures );
				m_btnLength.SetWindowText( strLength );
			}
		}

		if( ::IsWindow( m_btnLength.m_hWnd ) )
		{
			m_btnLength.SetFocus();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnTimeSignature

void CTabPatternPattern::OnTimeSignature() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPattern )
	{
		CTimeSignatureDlg tsDlg;

		tsDlg.m_TimeSignature = m_pPattern->m_TimeSignature;
		tsDlg.m_nContext = IDS_PATTERN_TEXT;

		if( tsDlg.DoModal() == IDOK )
		{
			// Update time signature
			m_pPattern->SetTimeSignature( tsDlg.m_TimeSignature, FALSE );

			// TODO: Refresh property page

			// Update bitmap on time signature button
			SetTimeSignatureBitmap();
		}

		if( ::IsWindow( m_btnTimeSignature.m_hWnd ) )
		{
			m_btnTimeSignature.SetFocus();
		}
	}
}

void CTabPatternPattern::OnCheckResetVarOrder() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		DWORD dwFlags = m_pPattern->m_dwFlags;

		// Get the state of the Reset Variation Order checkbox
		// (the button state is the inverse of the flag setting)
		if( m_btnResetVarOrder.GetCheck() == BST_CHECKED )
		{
			dwFlags &= ~DMUS_PATTERNF_PERSIST_CONTROL;
		}
		else
		{
			dwFlags |= DMUS_PATTERNF_PERSIST_CONTROL;
		}

		m_pPattern->SetFlags( dwFlags );
	}
}
