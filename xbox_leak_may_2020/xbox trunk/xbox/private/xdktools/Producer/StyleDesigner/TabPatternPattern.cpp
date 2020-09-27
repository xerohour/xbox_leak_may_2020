// TabPatternPattern.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "Pattern.h"
#include "PatternLengthDlg.h"
#include "TimeSignatureDlg.h"
#include "RhythmDlg.h"
#include "TabPatternPattern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern property page

CTabPatternPattern::CTabPatternPattern( CPatternPropPageManager* pPatternPropPageManager ) : CPropertyPage(CTabPatternPattern::IDD)
{
	//{{AFX_DATA_INIT(CTabPatternPattern)
	//}}AFX_DATA_INIT
	
	ASSERT( pPatternPropPageManager != NULL );

	m_pPattern = NULL;
	m_pPageManager = pPatternPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabPatternPattern::~CTabPatternPattern()
{
	// Work around 27331
	m_pPattern = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::RefreshTab

void CTabPatternPattern::RefreshTab( IDMUSProdPropPageObject* pIPropPageObject )
{
	PPGPattern ppgPattern;
	PPGPattern* pPPGPattern = &ppgPattern;

	if( pIPropPageObject
	&& ( SUCCEEDED ( pIPropPageObject->GetData((void **)&pPPGPattern ) ) ) )
	{
		m_pPattern = ppgPattern.pPattern;
	}
	else
	{
		m_pPattern = NULL;
	}

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::EnableControls

void CTabPatternPattern::EnableControls( BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
	m_btnLength.EnableWindow( fEnable );
	m_editGrooveBottom.EnableWindow( fEnable );
	m_spinGrooveBottom.EnableWindow( fEnable );
	m_editGrooveTop.EnableWindow( fEnable );
	m_spinGrooveTop.EnableWindow( fEnable );
	m_editDestBottom.EnableWindow( fEnable );
	m_spinDestBottom.EnableWindow( fEnable );
	m_editDestTop.EnableWindow( fEnable );
	m_spinDestTop.EnableWindow( fEnable );
	m_btnIntro.EnableWindow( fEnable );
	m_btnFill.EnableWindow( fEnable );
	m_btnBreak.EnableWindow( fEnable );
	m_btnEnd.EnableWindow( fEnable );
	m_btnCustom.EnableWindow( fEnable );
	m_editCustomId.EnableWindow( fEnable );
	m_spinCustomId.EnableWindow( fEnable );
	m_btnTimeSignature.EnableWindow( fEnable );
	m_btnCustomDlg.EnableWindow( fEnable );
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
	m_editGrooveBottom.LimitText( 3 );
	m_editGrooveTop.LimitText( 3 );
	m_editDestBottom.LimitText( 3 );
	m_editDestTop.LimitText( 3 );
	m_editCustomId.LimitText( 3 );

	if( m_pPattern )
	{
		EnableControls( TRUE );

		// Set name
		m_editName.SetWindowText( m_pPattern->m_strName );
		
		// Set length
		CString strLength;
		strLength.Format( "%d", m_pPattern->m_wNbrMeasures );
		m_btnLength.SetWindowText( strLength );

		// Set Bottom Groove
		m_spinGrooveBottom.SetRange( MIN_GROOVE, MAX_GROOVE );
		m_spinGrooveBottom.SetPos( m_pPattern->m_bGrooveBottom );

		// Set Top Groove 
		m_spinGrooveTop.SetRange( MIN_GROOVE, MAX_GROOVE );
		m_spinGrooveTop.SetPos( m_pPattern->m_bGrooveTop );

		// Set Destination Bottom Groove
		m_spinDestBottom.SetRange( MIN_GROOVE, MAX_GROOVE );
		m_spinDestBottom.SetPos( m_pPattern->m_bDestGrooveBottom );

		// Set Destination Top Groove 
		m_spinDestTop.SetRange( MIN_GROOVE, MAX_GROOVE );
		m_spinDestTop.SetPos( m_pPattern->m_bDestGrooveTop );

		// Set Embellishments 
		m_btnIntro.SetCheck( (m_pPattern->m_wEmbellishment & EMB_INTRO) ? TRUE : FALSE );
		m_btnFill.SetCheck( (m_pPattern->m_wEmbellishment & EMB_FILL) ? TRUE : FALSE );
		m_btnBreak.SetCheck( (m_pPattern->m_wEmbellishment & EMB_BREAK) ? TRUE : FALSE );
		m_btnEnd.SetCheck( (m_pPattern->m_wEmbellishment & EMB_END) ? TRUE : FALSE );

		// Set User-defined Embellishment
		BOOL fCustomEmbellishment = FALSE;
		if( HIBYTE(m_pPattern->m_wEmbellishment) >= MIN_EMB_CUSTOM_ID
		&&  HIBYTE(m_pPattern->m_wEmbellishment) <= MAX_EMB_CUSTOM_ID )
		{
			fCustomEmbellishment = TRUE;
			ASSERT( LOBYTE(m_pPattern->m_wEmbellishment) == 0 );
		}
		m_btnCustom.SetCheck( fCustomEmbellishment );
		m_spinCustomId.SetRange( MIN_EMB_CUSTOM_ID, MAX_EMB_CUSTOM_ID );
		if( fCustomEmbellishment )
		{
			m_editCustomId.EnableWindow( TRUE );
			m_spinCustomId.EnableWindow( TRUE );
			m_spinCustomId.SetPos( HIBYTE(m_pPattern->m_wEmbellishment) );
		}
		else
		{
			m_spinCustomId.SetPos( m_pPattern->m_nLastCustomId );
			m_editCustomId.EnableWindow( FALSE );
			m_spinCustomId.EnableWindow( FALSE );
		}

		// Draw rhythm map
		m_btnRhythmMap.Invalidate();

		// Update bitmap on time signature button
		SetTimeSignatureBitmap();
	}
	else
	{
		m_editName.SetWindowText( _T("") );
		m_btnLength.SetWindowText( _T("") );
		m_spinGrooveBottom.SetRange( MIN_GROOVE, MAX_GROOVE );
		m_spinGrooveBottom.SetPos( MIN_GROOVE );
		m_spinGrooveTop.SetRange( MIN_GROOVE, MAX_GROOVE );
		m_spinGrooveTop.SetPos( MAX_GROOVE );
		m_spinDestBottom.SetRange( MIN_GROOVE, MAX_GROOVE );
		m_spinDestBottom.SetPos( MIN_GROOVE );
		m_spinDestTop.SetRange( MIN_GROOVE, MAX_GROOVE );
		m_spinDestTop.SetPos( MAX_GROOVE );
		m_btnIntro.SetCheck( 0 );
		m_btnFill.SetCheck( 0 );
		m_btnBreak.SetCheck( 0 );
		m_btnEnd.SetCheck( 0 );
		m_btnCustom.SetCheck( 0 );
		m_spinCustomId.SetRange( MIN_EMB_CUSTOM_ID, MAX_EMB_CUSTOM_ID );
		m_spinCustomId.SetPos( MIN_EMB_CUSTOM_ID );
		m_btnRhythmMap.Invalidate();

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
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabPatternPattern)
	DDX_Control(pDX, IDC_EMB_CUSTOM, m_btnCustom);
	DDX_Control(pDX, IDC_CUSTOM_ID, m_editCustomId);
	DDX_Control(pDX, IDC_CUSTOM_ID_SPIN, m_spinCustomId);
	DDX_Control(pDX, IDC_DEST_TOP_SPIN, m_spinDestTop);
	DDX_Control(pDX, IDC_DEST_BOTTOM_SPIN, m_spinDestBottom);
	DDX_Control(pDX, IDC_DEST_TOP, m_editDestTop);
	DDX_Control(pDX, IDC_DEST_BOTTOM, m_editDestBottom);
	DDX_Control(pDX, IDC_TIME_SIGNATURE, m_btnTimeSignature);
	DDX_Control(pDX, IDC_RHYTHM_MAP, m_btnRhythmMap);
	DDX_Control(pDX, IDC_CUSTOM_DLG, m_btnCustomDlg);
	DDX_Control(pDX, IDC_EMB_INTRO, m_btnIntro);
	DDX_Control(pDX, IDC_EMB_FILL, m_btnFill);
	DDX_Control(pDX, IDC_EMB_END, m_btnEnd);
	DDX_Control(pDX, IDC_EMB_BREAK, m_btnBreak);
	DDX_Control(pDX, IDC_GROOVE_TOP_SPIN, m_spinGrooveTop);
	DDX_Control(pDX, IDC_GROOVE_TOP, m_editGrooveTop);
	DDX_Control(pDX, IDC_GROOVE_BOTTOM_SPIN, m_spinGrooveBottom);
	DDX_Control(pDX, IDC_GROOVE_BOTTOM, m_editGrooveBottom);
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
	ON_EN_KILLFOCUS(IDC_GROOVE_BOTTOM, OnKillFocusGrooveBottom)
	ON_NOTIFY(UDN_DELTAPOS, IDC_GROOVE_BOTTOM_SPIN, OnDeltaPosGrooveBottomSpin)
	ON_EN_KILLFOCUS(IDC_GROOVE_TOP, OnKillFocusGrooveTop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_GROOVE_TOP_SPIN, OnDeltaPosGrooveTopSpin)
	ON_BN_CLICKED(IDC_EMB_INTRO, OnEmbIntro)
	ON_BN_CLICKED(IDC_EMB_FILL, OnEmbFill)
	ON_BN_CLICKED(IDC_EMB_BREAK, OnEmbBreak)
	ON_BN_CLICKED(IDC_EMB_END, OnEmbEnd)
	ON_BN_CLICKED(IDC_CUSTOM_DLG, OnCustomDlg)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_TIME_SIGNATURE, OnTimeSignature)
	ON_EN_KILLFOCUS(IDC_DEST_BOTTOM, OnKillFocusDestBottom)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEST_BOTTOM_SPIN, OnDeltaPosDestBottomSpin)
	ON_EN_KILLFOCUS(IDC_DEST_TOP, OnKillFocusDestTop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEST_TOP_SPIN, OnDeltaPosDestTopSpin)
	ON_BN_CLICKED(IDC_EMB_CUSTOM, OnEmbCustom)
	ON_EN_KILLFOCUS(IDC_CUSTOM_ID, OnKillFocusCustomId)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CUSTOM_ID_SPIN, OnDeltaPosCustomIdSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnSetActive

BOOL CTabPatternPattern::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnCreate

int CTabPatternPattern::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CTabPatternPattern::OnKillFocusName

void CTabPatternPattern::OnKillFocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

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
				BSTR bstrName = strName.AllocSysString();
				m_pPattern->SetNodeName( bstrName );
				theApp.m_pStyleComponent->m_pIFramework->RefreshNode( m_pPattern );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnLength

void CTabPatternPattern::OnLength() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

			// Redraw rhythm map
			if( ::IsWindow( m_btnRhythmMap.m_hWnd ) )
			{
				m_btnRhythmMap.Invalidate();
				m_btnRhythmMap.UpdateWindow();
			}
		}

		if( ::IsWindow( m_btnLength.m_hWnd ) )
		{
			m_btnLength.SetFocus();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnKillFocusGrooveBottom

void CTabPatternPattern::OnKillFocusGrooveBottom() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		CString strNewGrooveBottom;

		m_editGrooveBottom.GetWindowText( strNewGrooveBottom );

		// Strip leading and trailing spaces
		strNewGrooveBottom.TrimRight();
		strNewGrooveBottom.TrimLeft();

		if( strNewGrooveBottom.IsEmpty() )
		{
			m_spinGrooveBottom.SetPos( m_pPattern->m_bGrooveBottom );
		}
		else
		{
			int nNewGrooveBottom = _ttoi( strNewGrooveBottom );
			int nNewGrooveTop = m_pPattern->m_bGrooveTop;
		
			if( nNewGrooveBottom < MIN_GROOVE)
			{
				nNewGrooveBottom = MIN_GROOVE;
			}
		
			if( nNewGrooveBottom > MAX_GROOVE)
			{
				nNewGrooveBottom = MAX_GROOVE;
			}

			m_spinGrooveBottom.SetPos( nNewGrooveBottom );

			if( nNewGrooveBottom > nNewGrooveTop )
			{
				nNewGrooveTop = nNewGrooveBottom;
				m_spinGrooveTop.SetPos( nNewGrooveTop );
			}

			m_pPattern->SetGrooveRange( (BYTE)nNewGrooveBottom, (BYTE)nNewGrooveTop );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnDeltaPosGrooveBottomSpin

void CTabPatternPattern::OnDeltaPosGrooveBottomSpin( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nGrooveBottom = m_spinGrooveBottom.GetPos();

		if( HIWORD(nGrooveBottom) == 0 )
		{
			int nNewGrooveBottom = LOWORD(nGrooveBottom) + pNMUpDown->iDelta;
			int nNewGrooveTop = m_pPattern->m_bGrooveTop;

			if( nNewGrooveBottom < MIN_GROOVE)
			{
				nNewGrooveBottom = MIN_GROOVE;
			}
		
			if( nNewGrooveBottom > MAX_GROOVE)
			{
				nNewGrooveBottom = MAX_GROOVE;
			}

			m_spinGrooveBottom.SetPos( nNewGrooveBottom );

			if( nNewGrooveBottom > nNewGrooveTop )
			{
				nNewGrooveTop = nNewGrooveBottom;
				m_spinGrooveTop.SetPos( nNewGrooveTop );
			}

			m_pPattern->SetGrooveRange( (BYTE)nNewGrooveBottom, (BYTE)nNewGrooveTop );
		}
	}
	
	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnKillFocusGrooveTop

void CTabPatternPattern::OnKillFocusGrooveTop() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		CString strNewGrooveTop;

		m_editGrooveTop.GetWindowText( strNewGrooveTop );

		// Strip leading and trailing spaces
		strNewGrooveTop.TrimRight();
		strNewGrooveTop.TrimLeft();

		if( strNewGrooveTop.IsEmpty() )
		{
			m_spinGrooveTop.SetPos( m_pPattern->m_bGrooveTop );
		}
		else
		{
			int nNewGrooveTop = _ttoi( strNewGrooveTop );
			int nNewGrooveBottom = m_pPattern->m_bGrooveBottom;

			if( nNewGrooveTop < MIN_GROOVE)
			{
				nNewGrooveTop = MIN_GROOVE;
			}

			if( nNewGrooveTop > MAX_GROOVE)
			{
				nNewGrooveTop = MAX_GROOVE;
			}

			m_spinGrooveTop.SetPos( nNewGrooveTop );

			if( nNewGrooveTop < nNewGrooveBottom )
			{
				nNewGrooveBottom = nNewGrooveTop;
				m_spinGrooveBottom.SetPos( nNewGrooveBottom );
			}

			m_pPattern->SetGrooveRange( (BYTE)nNewGrooveBottom, (BYTE)nNewGrooveTop );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnDeltaPosGrooveTopSpin

void CTabPatternPattern::OnDeltaPosGrooveTopSpin( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nGrooveTop = m_spinGrooveTop.GetPos();

		if( HIWORD(nGrooveTop) == 0 )
		{
			int nNewGrooveTop = LOWORD(nGrooveTop) + pNMUpDown->iDelta;
			int nNewGrooveBottom = m_pPattern->m_bGrooveBottom;

			if( nNewGrooveTop < MIN_GROOVE)
			{
				nNewGrooveTop = MIN_GROOVE;
			}

			if( nNewGrooveTop > MAX_GROOVE)
			{
				nNewGrooveTop = MAX_GROOVE;
			}

			m_spinGrooveTop.SetPos( nNewGrooveTop );

			if( nNewGrooveTop < nNewGrooveBottom )
			{
				nNewGrooveBottom = nNewGrooveTop;
				m_spinGrooveBottom.SetPos( nNewGrooveBottom );
			}

			m_pPattern->SetGrooveRange( (BYTE)nNewGrooveBottom, (BYTE)nNewGrooveTop );
		}
	}
	
	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnEmbIntro

void CTabPatternPattern::OnEmbIntro() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		if( m_btnIntro.GetCheck() )
		{
			m_pPattern->SetEmbellishment( EMB_INTRO, 0, 0 );
		}
		else
		{
			m_pPattern->SetEmbellishment( 0, EMB_INTRO, 0 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnEmbFill

void CTabPatternPattern::OnEmbFill() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		if( m_btnFill.GetCheck() )
		{
			m_pPattern->SetEmbellishment( EMB_FILL, 0, 0 );
		}
		else
		{
			m_pPattern->SetEmbellishment( 0, EMB_FILL, 0 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnEmbBreak

void CTabPatternPattern::OnEmbBreak() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		if( m_btnBreak.GetCheck() )
		{
			m_pPattern->SetEmbellishment( EMB_BREAK, 0, 0 );
		}
		else
		{
			m_pPattern->SetEmbellishment( 0, EMB_BREAK, 0 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnEmbEnd

void CTabPatternPattern::OnEmbEnd() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		if( m_btnEnd.GetCheck() )
		{
			m_pPattern->SetEmbellishment( EMB_END, 0, 0 );
		}
		else
		{
			m_pPattern->SetEmbellishment( 0, EMB_END, 0 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnCustomDlg

void CTabPatternPattern::OnCustomDlg() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		// Display rhythmDlg
		CRhythmDlg rhythmDlg;
		rhythmDlg.m_TimeSignature = m_pPattern->m_TimeSignature;
		rhythmDlg.m_wNbrMeasures = m_pPattern->m_wNbrMeasures;
		rhythmDlg.m_pRhythmMap = new DWORD[m_pPattern->m_wNbrMeasures];
		if( rhythmDlg.m_pRhythmMap )
		{
			for( int i = 0 ;  i < m_pPattern->m_wNbrMeasures ;  ++i )
			{
				rhythmDlg.m_pRhythmMap[i] = m_pPattern->m_pRhythmMap[i];
			}

			if( rhythmDlg.DoModal() == IDOK )
			{
				// Update rhythm map
				m_pPattern->SetRhythmMap( rhythmDlg.m_pRhythmMap );

				// Redraw rhythm map
				if( ::IsWindow( m_btnRhythmMap.m_hWnd ) )
				{
					m_btnRhythmMap.Invalidate();
					m_btnRhythmMap.UpdateWindow();
				}
			}
		}

		if( ::IsWindow( m_btnCustomDlg.m_hWnd ) )
		{
			m_btnCustomDlg.SetFocus();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnDrawItem

void CTabPatternPattern::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern == NULL )
	{
		CPropertyPage::OnDrawItem( nIDCtl, lpDrawItemStruct );
		return;
	}

	switch( nIDCtl )
	{
		case IDC_RHYTHM_MAP:
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

            if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE
            ||  lpDrawItemStruct->itemAction & ODA_SELECT )
			{
				int i, j;
				int nMaxRight = lpDrawItemStruct->rcItem.right - 4;

				CRect rect( lpDrawItemStruct->rcItem );
				rect.right = rect.left;
				rect.InflateRect( 0, -3 );
				int nTickHeight = (rect.Height() >> 1) - 1;
				for( i = 0 ;  i < m_pPattern->m_wNbrMeasures ; i++ )
				{
					for( j = 0 ;  j < 32 ;  j++ )
					{
						if( j >= m_pPattern->m_TimeSignature.m_bBeatsPerMeasure )
						{
							break;
						}

						rect.left  = rect.right + 2;
						rect.right = rect.left + 1;
						if( rect.left >= nMaxRight )
						{
							break;
						}

						if( m_pPattern->m_pRhythmMap[i] & (1 << j) )
						{
							pDC->FillSolidRect( &rect, RGB(0,0,0) );
						}
						else
						{
							rect.InflateRect( 0, -nTickHeight );
							pDC->FillSolidRect( &rect, RGB(0,0,0) );
							rect.InflateRect( 0, nTickHeight );
						}
					}
					
					rect.left += 3;
					rect.right += 3;
					if( rect.left >= nMaxRight )
					{
						break;
					}
				}
				rect.InflateRect( 0, 3 );
			}

			return;
		}
	}
	
	CPropertyPage::OnDrawItem( nIDCtl, lpDrawItemStruct );
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnTimeSignature

void CTabPatternPattern::OnTimeSignature() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		CTimeSignatureDlg tsDlg;

		tsDlg.m_TimeSignature = m_pPattern->m_TimeSignature;
		tsDlg.m_nContext = IDS_PATTERN_TEXT;

		if( tsDlg.DoModal() == IDOK )
		{
			// Update time signature
			AfxMessageBox( "Not yet implemented." );
// AMC??	m_pPattern->SetTimeSignature( tsDlg.m_TimeSignature, FALSE );

			// Update bitmap on time signature button
			SetTimeSignatureBitmap();
		}

		if( ::IsWindow( m_btnTimeSignature.m_hWnd ) )
		{
			m_btnTimeSignature.SetFocus();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnKillFocusDestBottom

void CTabPatternPattern::OnKillFocusDestBottom() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		CString strNewDestBottom;

		m_editDestBottom.GetWindowText( strNewDestBottom );

		// Strip leading and trailing spaces
		strNewDestBottom.TrimRight();
		strNewDestBottom.TrimLeft();

		if( strNewDestBottom.IsEmpty() )
		{
			m_spinDestBottom.SetPos( m_pPattern->m_bDestGrooveBottom );
		}
		else
		{
			int nNewDestBottom = _ttoi( strNewDestBottom );
			int nNewDestTop = m_pPattern->m_bDestGrooveTop;
		
			if( nNewDestBottom < MIN_GROOVE)
			{
				nNewDestBottom = MIN_GROOVE;
			}
		
			if( nNewDestBottom > MAX_GROOVE)
			{
				nNewDestBottom = MAX_GROOVE;
			}

			m_spinDestBottom.SetPos( nNewDestBottom );

			if( nNewDestBottom > nNewDestTop )
			{
				nNewDestTop = nNewDestBottom;
				m_spinDestTop.SetPos( nNewDestTop );
			}

			m_pPattern->SetDestGrooveRange( (BYTE)nNewDestBottom, (BYTE)nNewDestTop );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnDeltaPosDestBottomSpin

void CTabPatternPattern::OnDeltaPosDestBottomSpin( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nDestBottom = m_spinDestBottom.GetPos();
		int nNewDestTop = m_pPattern->m_bDestGrooveTop;

		if( HIWORD(nDestBottom) == 0 )
		{
			int nNewDestBottom = LOWORD(nDestBottom) + pNMUpDown->iDelta;

			if( nNewDestBottom < MIN_GROOVE)
			{
				nNewDestBottom = MIN_GROOVE;
			}
		
			if( nNewDestBottom > MAX_GROOVE)
			{
				nNewDestBottom = MAX_GROOVE;
			}

			m_spinDestBottom.SetPos( nNewDestBottom );

			if( nNewDestBottom > nNewDestTop )
			{
				nNewDestTop = nNewDestBottom;
				m_spinDestTop.SetPos( nNewDestTop );
			}

			m_pPattern->SetDestGrooveRange( (BYTE)nNewDestBottom, (BYTE)nNewDestTop );
		}
	}
	
	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnKillFocusDestTop

void CTabPatternPattern::OnKillFocusDestTop() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		CString strNewDestTop;

		m_editDestTop.GetWindowText( strNewDestTop );

		// Strip leading and trailing spaces
		strNewDestTop.TrimRight();
		strNewDestTop.TrimLeft();

		if( strNewDestTop.IsEmpty() )
		{
			m_spinDestTop.SetPos( m_pPattern->m_bDestGrooveTop );
		}
		else
		{
			int nNewDestTop = _ttoi( strNewDestTop );
			int nNewDestBottom = m_pPattern->m_bDestGrooveBottom;

			if( nNewDestTop < MIN_GROOVE)
			{
				nNewDestTop = MIN_GROOVE;
			}

			if( nNewDestTop > MAX_GROOVE)
			{
				nNewDestTop = MAX_GROOVE;
			}

			m_spinDestTop.SetPos( nNewDestTop );

			if( nNewDestTop < nNewDestBottom )
			{
				nNewDestBottom = nNewDestTop;
				m_spinDestBottom.SetPos( nNewDestBottom );
			}

			m_pPattern->SetDestGrooveRange( (BYTE)nNewDestBottom, (BYTE)nNewDestTop );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnDeltaPosDestTopSpin

void CTabPatternPattern::OnDeltaPosDestTopSpin( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nDestTop = m_spinDestTop.GetPos();

		if( HIWORD(nDestTop) == 0 )
		{
			int nNewDestTop = LOWORD(nDestTop) + pNMUpDown->iDelta;
			int nNewDestBottom = m_pPattern->m_bDestGrooveBottom;

			if( nNewDestTop < MIN_GROOVE)
			{
				nNewDestTop = MIN_GROOVE;
			}

			if( nNewDestTop > MAX_GROOVE)
			{
				nNewDestTop = MAX_GROOVE;
			}

			m_spinDestTop.SetPos( nNewDestTop );

			if( nNewDestTop < nNewDestBottom )
			{
				nNewDestBottom = nNewDestTop;
				m_spinDestBottom.SetPos( nNewDestBottom );
			}

			m_pPattern->SetDestGrooveRange( (BYTE)nNewDestBottom, (BYTE)nNewDestTop );
		}
	}
	
	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnEmbCustom

void CTabPatternPattern::OnEmbCustom() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		if( m_btnCustom.GetCheck() )
		{
			m_pPattern->SetEmbellishment( 0, EMB_ALL, m_pPattern->m_nLastCustomId );
		}
		else
		{
			m_pPattern->SetEmbellishment( 0, EMB_ALL, 0 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnKillFocusCustomId

void CTabPatternPattern::OnKillFocusCustomId() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		CString strNewCustomId;

		m_editCustomId.GetWindowText( strNewCustomId );

		// Strip leading and trailing spaces
		strNewCustomId.TrimRight();
		strNewCustomId.TrimLeft();

		if( strNewCustomId.IsEmpty() )
		{
			ASSERT( HIBYTE(m_pPattern->m_wEmbellishment) >= MIN_EMB_CUSTOM_ID
			    &&  HIBYTE(m_pPattern->m_wEmbellishment) <= MAX_EMB_CUSTOM_ID );
			m_spinCustomId.SetPos( HIBYTE(m_pPattern->m_wEmbellishment) );
		}
		else
		{
			int nNewCustomId = _ttoi( strNewCustomId );
		
			if( nNewCustomId < MIN_EMB_CUSTOM_ID)
			{
				nNewCustomId = MIN_EMB_CUSTOM_ID;
			}
		
			if( nNewCustomId > MAX_EMB_CUSTOM_ID)
			{
				nNewCustomId = MAX_EMB_CUSTOM_ID;
			}

			m_spinCustomId.SetPos( nNewCustomId );
			m_pPattern->SetEmbellishment( 0, EMB_ALL, (BYTE)nNewCustomId );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPatternPattern::OnDeltaPosCustomIdSpin

void CTabPatternPattern::OnDeltaPosCustomIdSpin( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPattern )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nCustomId = m_spinCustomId.GetPos();

		if( HIWORD(nCustomId) == 0 )
		{
			int nNewCustomId = LOWORD(nCustomId) + pNMUpDown->iDelta;

			if( nNewCustomId < MIN_EMB_CUSTOM_ID)
			{
				nNewCustomId = MIN_EMB_CUSTOM_ID;
			}

			if( nNewCustomId > MAX_EMB_CUSTOM_ID)
			{
				nNewCustomId = MAX_EMB_CUSTOM_ID;
			}

			m_spinCustomId.SetPos( nNewCustomId );
			m_pPattern->SetEmbellishment( 0, EMB_ALL, (BYTE)nNewCustomId );
		}
	}
	
	*pResult = 1;
}
