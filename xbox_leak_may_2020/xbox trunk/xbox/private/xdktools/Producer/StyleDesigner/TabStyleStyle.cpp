// TabStyleStyle.cpp : implementation file
//

#include "stdafx.h"
#include "TabStyleStyle.h"
#include "StyleDesignerDll.h"
#include "Style.h"
#include "TimeSignatureDlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle property page

CTabStyleStyle::CTabStyleStyle( CStylePropPageManager* pStylePropPageManager ) : CPropertyPage(CTabStyleStyle::IDD)
{
	//{{AFX_DATA_INIT(CTabStyleStyle)
	//}}AFX_DATA_INIT
	
	ASSERT( pStylePropPageManager != NULL );

	m_pStyle = NULL;
	m_pPageManager = pStylePropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabStyleStyle::~CTabStyleStyle()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::SetStyle

void CTabStyleStyle::SetStyle( CDirectMusicStyle* pStyle )
{
	m_pStyle = pStyle;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::SetModifiedFlag

void CTabStyleStyle::SetModifiedFlag( void ) 
{
	ASSERT( m_pStyle != NULL );

	m_pStyle->SetModified( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::SetTempoControlText

void CTabStyleStyle::SetTempoControlText( void ) 
{
	if( m_pStyle
	&&	::IsWindow( m_editTempo.m_hWnd ) )
	{
		CString strTempo;

		strTempo.Format( "%.2f", m_pStyle->m_dblTempo );
		m_editTempo.SetWindowText( strTempo );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::EnableControls

void CTabStyleStyle::EnableControls( BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
	m_editTempo.EnableWindow( fEnable );
	m_spinTempo.EnableWindow( fEnable );
	m_btnTimeSignature.EnableWindow( fEnable );
	m_cmbxCategory.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::UpdateControls

void CTabStyleStyle::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_editName.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editName.LimitText( DMUS_MAX_NAME );
	m_spinTempo.SetRange( DMUS_TEMPO_MIN, DMUS_TEMPO_MAX );
	m_editTempo.LimitText( 6 ); // 350.00

	if( m_pStyle )
	{
		EnableControls( TRUE );

		// Set name
		m_editName.SetWindowText( m_pStyle->m_strName );

		// Set tempo
		SetTempoControlText();

		// Set category
		int nPos = m_cmbxCategory.FindStringExact( -1, m_pStyle->m_strCategoryName );
		if( nPos == CB_ERR )
		{
			TCHAR achNone[MID_BUFFER];

			::LoadString( theApp.m_hInstance, IDS_NONE_TEXT, achNone, MID_BUFFER );
			nPos = m_cmbxCategory.FindStringExact( -1, achNone );

		}
		m_cmbxCategory.SetCurSel( nPos );

		// Update bitmap on time signature button
		SetTimeSignatureBitmap();
	}
	else
	{
		m_editName.SetWindowText( _T("") );
		m_spinTempo.SetPos( MIN_TEMPO );

		EnableControls( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::SetTimeSignatureBitmap

void CTabStyleStyle::SetTimeSignatureBitmap( void )
{
	if( !::IsWindow( m_btnTimeSignature.m_hWnd ) )
	{
		return;
	}

	HBITMAP hNewBits = NULL;

	ASSERT( m_pStyle != NULL );

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
							 m_pStyle->m_TimeSignature.m_bBeatsPerMeasure,
							 m_pStyle->m_TimeSignature.m_bBeat );
	rect.left += 6;
	cdcDest.SetTextColor( COLOR_BTNTEXT );
	cdcDest.DrawText( strTimeSignature, -1, &rect, (DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX) );
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
		int nResourceID = m_pStyle->m_TimeSignature.m_wGridsPerBeat - 1;
		if( m_pStyle->m_TimeSignature.m_bBeat != 4 )		// 4 = quarter note gets the beat
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


void CTabStyleStyle::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabStyleStyle)
	DDX_Control(pDX, IDC_TIME_SIGNATURE, m_btnTimeSignature);
	DDX_Control(pDX, IDC_TEMPO_SPIN, m_spinTempo);
	DDX_Control(pDX, IDC_TEMPO, m_editTempo);
	DDX_Control(pDX, IDC_CATEGORY, m_cmbxCategory);
	DDX_Control(pDX, IDC_NAME, m_editName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabStyleStyle, CPropertyPage)
	//{{AFX_MSG_MAP(CTabStyleStyle)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillFocusName)
	ON_EN_KILLFOCUS(IDC_TEMPO, OnKillFocusTempo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TEMPO_SPIN, OnDeltaPosTempoSpin)
	ON_CBN_KILLFOCUS(IDC_CATEGORY, OnKillFocusCategory)
	ON_BN_CLICKED(IDC_TIME_SIGNATURE, OnTimeSignature)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle message handlers


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::OnInitDialog

BOOL CTabStyleStyle::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();
	
	// Fill Category list box
	TCHAR achCategory[MID_BUFFER];
	int i;

	for ( i = IDS_CATEGORY1 ;  i <= IDS_CATEGORY15 ;  i++ )
	{
		::LoadString( theApp.m_hInstance, i, achCategory, MID_BUFFER );
		m_cmbxCategory.AddString( achCategory );
	}

	::LoadString( theApp.m_hInstance, IDS_NONE_TEXT, achCategory, MID_BUFFER );
	m_cmbxCategory.AddString( achCategory );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::OnSetActive

BOOL CTabStyleStyle::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CStylePropPageManager::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::OnCreate

int CTabStyleStyle::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
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
// CTabStyleStyle::OnDestroy

void CTabStyleStyle::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CTabStyleStyle::OnKillFocusName

void CTabStyleStyle::OnKillFocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( m_pStyle )
	{
		CString strName;

		m_editName.GetWindowText( strName );

		// Strip leading and trailing spaces
		strName.TrimRight();
		strName.TrimLeft();

		if( strName.IsEmpty() )
		{
			m_editName.SetWindowText( m_pStyle->m_strName );
		}
		else
		{
			if( strName.Compare( m_pStyle->m_strName ) != 0 )
			{
				BSTR bstrName = strName.AllocSysString();
				m_pStyle->SetNodeName( bstrName );
				theApp.m_pStyleComponent->m_pIFramework->RefreshNode( m_pStyle );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::OnKillFocusCategory

void CTabStyleStyle::OnKillFocusCategory() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CString strCategoryName;
		TCHAR achNone[MID_BUFFER];

		int nPos = m_cmbxCategory.GetCurSel();
		if( nPos != CB_ERR )
		{
			m_cmbxCategory.GetLBText( nPos, strCategoryName );

			::LoadString( theApp.m_hInstance, IDS_NONE_TEXT, achNone, MID_BUFFER );
			if( strCategoryName.Compare( achNone ) == 0 )
			{
				strCategoryName.Empty();
			}
		}

		m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_CATEGORY );
		m_pStyle->m_strCategoryName = strCategoryName;
		SetModifiedFlag();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::OnKillFocusTempo

void CTabStyleStyle::OnKillFocusTempo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CString strNewTempo;

		m_editTempo.GetWindowText( strNewTempo );

		// Strip leading and trailing spaces
		strNewTempo.TrimRight();
		strNewTempo.TrimLeft();

		if( strNewTempo.IsEmpty() )
		{
			SetTempoControlText();
		}
		else
		{
			double dblNewTempo;

			TCHAR* pszTempo;
			pszTempo = new TCHAR[strNewTempo.GetLength() + 1];

			if( pszTempo )
			{
				pszTempo[0] = 0;

				if( _stscanf( strNewTempo, "%lf%s", &dblNewTempo, pszTempo ) )
				{
					// Check bounds
					if( dblNewTempo > DMUS_TEMPO_MAX )
					{
						dblNewTempo = DMUS_TEMPO_MAX;
					}
					else if( dblNewTempo < DMUS_TEMPO_MIN )
					{
						dblNewTempo = DMUS_TEMPO_MIN;
					}

					m_pStyle->SetTempo( dblNewTempo, FALSE );
				}

				SetTempoControlText();	// Make sure edit control
										// reflects the tempo of m_pStyle
				delete pszTempo;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::OnDeltaPosTempoSpin

void CTabStyleStyle::OnDeltaPosTempoSpin( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		// Need to do this in case the user clicked the spin control immediately after
		// typing in a value
		OnKillFocusTempo();

		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		double dblNewTempo = floor( m_pStyle->m_dblTempo + pNMUpDown->iDelta );

		// If too small, wrap to DMUS_TEMPO_MAX
		if( dblNewTempo < DMUS_TEMPO_MIN )
		{
			dblNewTempo = DMUS_TEMPO_MAX;
		}
		// If too large, wrap to DMUS_TEMPO_MIN
		else if( dblNewTempo > DMUS_TEMPO_MAX )
		{
			dblNewTempo = DMUS_TEMPO_MIN;
		}

		// Sync edit control so OnKillFocus doesn't change it back
		CString strTempo;
		strTempo.Format( "%.2f", dblNewTempo );
		m_editTempo.SetWindowText( strTempo );

		m_pStyle->SetTempo( dblNewTempo, FALSE );
		SetTempoControlText();	// Make sure edit control
								// reflects results of SetTempo()
	}
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle::OnTimeSignature

void CTabStyleStyle::OnTimeSignature() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CTimeSignatureDlg tsDlg;

		tsDlg.m_TimeSignature = m_pStyle->m_TimeSignature;
		tsDlg.m_nContext = IDS_STYLE_TEXT;

		if( tsDlg.DoModal() == IDOK )
		{
			// Update time signature
			m_pStyle->SetTimeSignature( tsDlg.m_TimeSignature, FALSE );

			// Update bitmap on time signature button
			SetTimeSignatureBitmap();
		}

		if( ::IsWindow( m_btnTimeSignature.m_hWnd ) )
		{
			m_btnTimeSignature.SetFocus();
		}
	}
}
