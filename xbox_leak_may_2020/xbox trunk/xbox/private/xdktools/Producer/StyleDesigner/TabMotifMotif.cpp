// TabMotifMotif.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "Pattern.h"
#include "PatternLengthDlg.h"
#include "TimeSignatureDlg.h"
#include "TabMotifMotif.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif property page

CTabMotifMotif::CTabMotifMotif( CMotifPropPageManager* pMotifPropPageManager ) : CPropertyPage(CTabMotifMotif::IDD)
{
	//{{AFX_DATA_INIT(CTabMotifMotif)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	ASSERT( pMotifPropPageManager != NULL );

	m_pMotif = NULL;
	m_pPageManager = pMotifPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabMotifMotif::~CTabMotifMotif()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::RefreshTab

void CTabMotifMotif::RefreshTab( IDMUSProdPropPageObject* pIPropPageObject )
{
	PPGMotif ppgMotif;
	PPGMotif* pPPGMotif = &ppgMotif;

	if( pIPropPageObject
	&& ( SUCCEEDED ( pIPropPageObject->GetData((void **)&pPPGMotif ) ) ) )
	{
		m_pMotif = ppgMotif.pMotif;
	}
	else
	{
		m_pMotif = NULL;
	}

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::EnableControls

void CTabMotifMotif::EnableControls( BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
	m_btnLength.EnableWindow( fEnable );

	m_editStartBar.EnableWindow( fEnable );
	m_spinStartBar.EnableWindow( fEnable );
	m_editStartBeat.EnableWindow( fEnable );
	m_spinStartBeat.EnableWindow( fEnable );
	m_editStartGrid.EnableWindow( fEnable );
	m_spinStartGrid.EnableWindow( fEnable );
	m_editStartTick.EnableWindow( fEnable );
	m_spinStartTick.EnableWindow( fEnable );
	m_btnTimeSignature.EnableWindow( fEnable );

	m_btnResetVarOrder.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::UpdatePlayStartControls

void CTabMotifMotif::UpdatePlayStartControls()
{
	if( m_pMotif )
	{
		// Get Bar, Beat, Grid, Tick of mtPlayStart
		long lBar;
		long lBeat;
		long lGrid;
		long lTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtPlayStart, &lBar, &lBeat, &lGrid, &lTick );

		// Set Start Bar, beat, Grid, Tick
		m_spinStartBar.SetPos( lBar );
		m_spinStartBeat.SetPos( lBeat );
		m_spinStartGrid.SetPos( lGrid );
		m_spinStartTick.SetPos( lTick );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::UpdateControls

void CTabMotifMotif::UpdateControls()
{
	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editName.LimitText( DMUS_MAX_NAME );

	if( m_pMotif )
	{
		EnableControls( TRUE );

		// Set Name
		m_editName.SetWindowText( m_pMotif->m_strName );
		
		// Set Length
		CString strLength;
		strLength.Format( "%d", m_pMotif->m_wNbrMeasures );
		m_btnLength.SetWindowText( strLength );

		// Set Bar, Beat, Grid, Tick
		UpdatePlayStartControls();

		// Update bitmap on time signature button
		SetTimeSignatureBitmap();

		// Update the Reset Variation Order checkbox (the button state is the inverse of the flag setting)
		m_btnResetVarOrder.SetCheck( m_pMotif->m_dwFlags & DMUS_PATTERNF_PERSIST_CONTROL ? BST_UNCHECKED : BST_CHECKED );
	}
	else
	{
		m_editName.SetWindowText( _T("") );
		m_btnLength.SetWindowText( _T("") );
		m_spinStartBar.SetPos( 1 );
		m_spinStartBeat.SetPos( 1 );
		m_spinStartGrid.SetPos( 1 );
		m_spinStartTick.SetPos( 0 );

		m_btnResetVarOrder.SetCheck( BST_UNCHECKED );

		EnableControls( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::SetTimeSignatureBitmap

void CTabMotifMotif::SetTimeSignatureBitmap( void )
{
	if( !::IsWindow( m_btnTimeSignature.m_hWnd ) )
	{
		return;
	}

	HBITMAP hNewBits = NULL;

	ASSERT( m_pMotif != NULL );

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
							 m_pMotif->m_TimeSignature.m_bBeatsPerMeasure,
							 m_pMotif->m_TimeSignature.m_bBeat );
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
		int nResourceID = m_pMotif->m_TimeSignature.m_wGridsPerBeat - 1;
		if( m_pMotif->m_TimeSignature.m_bBeat != 4 )		// 4 = quarter note gets the beat
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


void CTabMotifMotif::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabMotifMotif)
	DDX_Control(pDX, IDC_CHECK_RESET_VAR_ORDER, m_btnResetVarOrder);
	DDX_Control(pDX, IDC_TIME_SIGNATURE, m_btnTimeSignature);
	DDX_Control(pDX, IDC_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_BAR, m_editStartBar);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_BEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_GRID, m_editStartGrid);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_TICK, m_editStartTick);
	DDX_Control(pDX, IDC_LENGTH, m_btnLength);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_BAR, m_spinStartBar);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_BEAT, m_spinStartBeat);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_GRID, m_spinStartGrid);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_TICK, m_spinStartTick);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabMotifMotif, CPropertyPage)
	//{{AFX_MSG_MAP(CTabMotifMotif)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillFocusName)
	ON_BN_CLICKED(IDC_LENGTH, OnLength)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_BAR, OnKillFocusEditPlayStartBar)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_BEAT, OnKillFocusEditPlayStartBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_GRID, OnKillFocusEditPlayStartGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_TICK, OnKillFocusEditPlayStartTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_BAR, OnDeltaPosSpinPlayStartBar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_BEAT, OnDeltaPosSpinPlayStartBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_GRID, OnDeltaPosSpinPlayStartGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_TICK, OnDeltaPosSpinPlayStartTick)
	ON_BN_CLICKED(IDC_TIME_SIGNATURE, OnTimeSignature)
	ON_BN_CLICKED(IDC_CHECK_RESET_VAR_ORDER, OnCheckResetVarOrder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnSetActive

BOOL CTabMotifMotif::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnCreate

int CTabMotifMotif::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabMotifMotif::OnDestroy

void CTabMotifMotif::OnDestroy() 
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
// CTabMotifMotif::OnInitDialog

BOOL CTabMotifMotif::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Set control ranges
	m_spinStartBar.SetRange( 1, 32767 );
	m_editStartBar.LimitText( 5 );

	m_spinStartBeat.SetRange( 1, 256 );
	m_editStartBeat.LimitText( 3 );

	m_spinStartGrid.SetRange( 1, 256 );
	m_editStartGrid.LimitText( 3 );

	m_spinStartTick.SetRange( 0, 32767 );
	m_editStartTick.LimitText( 5 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnKillFocusName

void CTabMotifMotif::OnKillFocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( m_pMotif )
	{
		CString strName;

		m_editName.GetWindowText( strName );

		// Strip leading and trailing spaces
		strName.TrimRight();
		strName.TrimLeft();

		if( strName.IsEmpty() )
		{
			m_editName.SetWindowText( m_pMotif->m_strName );
		}
		else
		{
			if( strName.Compare( m_pMotif->m_strName ) != 0 )
			{
				BSTR bstrName = strName.AllocSysString();
				m_pMotif->SetNodeName( bstrName );
				theApp.m_pStyleComponent->m_pIFramework->RefreshNode( m_pMotif );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnLength

void CTabMotifMotif::OnLength() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		CPatternLengthDlg plDlg( m_pMotif );

		if( plDlg.DoModal() == IDOK )
		{
			CString strLength;

			strLength.Format( "%d", m_pMotif->m_wNbrMeasures );
			m_btnLength.SetWindowText( strLength );
		}

		m_btnLength.SetFocus();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnKillFocusEditPlayStartBar

void CTabMotifMotif::OnKillFocusEditPlayStartBar() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Get original Bar, Beat, Grid, Tick of mtPlayStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtPlayStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get string from control
		CString strNewStartBar;
		m_editStartBar.GetWindowText( strNewStartBar );

		// Strip leading and trailing spaces
		strNewStartBar.TrimRight();
		strNewStartBar.TrimLeft();

		if( strNewStartBar.IsEmpty() )
		{
			m_spinStartBar.SetPos( lOrigBar );
		}
		else
		{
			long lNewBar = _ttoi( strNewStartBar );
			lNewBar = max( lNewBar, 1 );
			lNewBar = min( lNewBar, m_pMotif->m_wNbrMeasures );

			// Set new Play Start 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lNewBar, lOrigBeat, lOrigGrid, lOrigTick,
												  &mtNewMusicTime );
			if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
			{
				lNewBar--;
				m_pMotif->BarBeatGridTickToMusicTime( lNewBar, lOrigBeat, lOrigGrid, lOrigTick,
													  &mtNewMusicTime );
			}
			m_pMotif->SetMotifPlayStart( mtNewMusicTime );

			// Update Play Start Bar, Beat, Grid, Tick controls
			UpdatePlayStartControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnKillFocusEditPlayStartBeat

void CTabMotifMotif::OnKillFocusEditPlayStartBeat() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Get original Bar, Beat, Grid, Tick of mtPlayStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtPlayStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get string from control
		CString strNewStartBeat;
		m_editStartBeat.GetWindowText( strNewStartBeat );

		// Strip leading and trailing spaces
		strNewStartBeat.TrimRight();
		strNewStartBeat.TrimLeft();

		if( strNewStartBeat.IsEmpty() )
		{
			m_spinStartBeat.SetPos( lOrigBeat );
		}
		else
		{
			long lNewBeat = _ttoi( strNewStartBeat );
			lNewBeat = max( lNewBeat, 1 );
			lNewBeat = min( lNewBeat, m_pMotif->m_TimeSignature.m_bBeatsPerMeasure );

			// Set new Play Start 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
												  &mtNewMusicTime );
			if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
			{
				lNewBeat--;
				m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
													  &mtNewMusicTime );
			}
			m_pMotif->SetMotifPlayStart( mtNewMusicTime );

			// Update Play Start Bar, Beat, Grid, Tick controls
			UpdatePlayStartControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnKillFocusEditPlayStartGrid

void CTabMotifMotif::OnKillFocusEditPlayStartGrid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Get original Bar, Beat, Grid, Tick of mtPlayStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtPlayStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get string from control
		CString strNewStartGrid;
		m_editStartGrid.GetWindowText( strNewStartGrid );

		// Strip leading and trailing spaces
		strNewStartGrid.TrimRight();
		strNewStartGrid.TrimLeft();

		if( strNewStartGrid.IsEmpty() )
		{
			m_spinStartGrid.SetPos( lOrigGrid );
		}
		else
		{
			long lNewGrid = _ttoi( strNewStartGrid );
			lNewGrid = max( lNewGrid, 1 );
			lNewGrid = min( lNewGrid, m_pMotif->m_TimeSignature.m_wGridsPerBeat );

			// Set new Play Start 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
												  &mtNewMusicTime );
			if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
			{
				lNewGrid--;
				m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
													  &mtNewMusicTime );
			}
			m_pMotif->SetMotifPlayStart( mtNewMusicTime );

			// Update Play Start Bar, Beat, Grid, Tick controls
			UpdatePlayStartControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnKillFocusEditPlayStartTick

void CTabMotifMotif::OnKillFocusEditPlayStartTick() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		long lClocksPerBeat = DM_PPQNx4 / (long)m_pMotif->m_TimeSignature.m_bBeat;
		long lClocksPerGrid = lClocksPerBeat / (long)m_pMotif->m_TimeSignature.m_wGridsPerBeat;
				
		// Get original Bar, Beat, Grid, Tick of mtPlayStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtPlayStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get string from control
		CString strNewStartTick;
		m_editStartTick.GetWindowText( strNewStartTick );

		// Strip leading and trailing spaces
		strNewStartTick.TrimRight();
		strNewStartTick.TrimLeft();

		if( strNewStartTick.IsEmpty() )
		{
			m_spinStartTick.SetPos( lOrigTick );
		}
		else
		{
			long lNewTick = _ttoi( strNewStartTick );
			lNewTick = max( lNewTick, 0 );
			lNewTick = min( lNewTick, lClocksPerGrid - 1 );

			// Set new Play Start 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lOrigGrid, lNewTick,
												  &mtNewMusicTime );
			if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
			{
				m_pMotif->SetMotifPlayStart( (long)m_pMotif->m_dwLength - 1 );
			}
			else
			{
				m_pMotif->SetMotifPlayStart( mtNewMusicTime );
			}

			// Update Play Start Bar, Beat, Grid, Tick controls
			UpdatePlayStartControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnDeltaPosSpinPlayStartBar

void CTabMotifMotif::OnDeltaPosSpinPlayStartBar( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtPlayStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtPlayStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Bar
		long lNewBar = m_spinStartBar.GetPos() + pNMUpDown->iDelta;
		lNewBar = max( lNewBar, 1 );
		lNewBar = min( lNewBar, m_pMotif->m_wNbrMeasures );

		// Set new Play Start 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lNewBar, lOrigBeat, lOrigGrid, lOrigTick,
											  &mtNewMusicTime );

		if( m_pMotif->SetMotifPlayStart( mtNewMusicTime ) == FALSE )
		{
			m_editStartBar.SetFocus();
		}

		// Update Play Start Bar, Beat, Grid, Tick controls
		UpdatePlayStartControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnDeltaPosSpinPlayStartBeat

void CTabMotifMotif::OnDeltaPosSpinPlayStartBeat( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtPlayStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtPlayStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Beat
		long lNewBeat = m_spinStartBeat.GetPos() + pNMUpDown->iDelta;
		lNewBeat = max( lNewBeat, 1 );

		// Set new Play Start 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
											  &mtNewMusicTime );
		if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
		{
			lNewBeat--;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
												  &mtNewMusicTime );
		}

		if( m_pMotif->SetMotifPlayStart( mtNewMusicTime ) == FALSE )
		{
			m_editStartBeat.SetFocus();
		}

		// Update Play Start Bar, Beat, Grid, Tick controls
		UpdatePlayStartControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnDeltaPosSpinPlayStartGrid

void CTabMotifMotif::OnDeltaPosSpinPlayStartGrid( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtPlayStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtPlayStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Grid
		long lNewGrid = m_spinStartGrid.GetPos() + pNMUpDown->iDelta;
		lNewGrid = max( lNewGrid, 1 );

		// Set new Play Start 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
											  &mtNewMusicTime );
		if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
		{
			lNewGrid--;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
												  &mtNewMusicTime );
		}

		if( m_pMotif->SetMotifPlayStart( mtNewMusicTime ) == FALSE )
		{
			m_editStartGrid.SetFocus();
		}

		// Update Play Start Bar, Beat, Grid, Tick controls
		UpdatePlayStartControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnDeltaPosSpinPlayStartTick

void CTabMotifMotif::OnDeltaPosSpinPlayStartTick( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtPlayStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtPlayStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Tick
		long lNewTick = m_spinStartTick.GetPos() + pNMUpDown->iDelta;
		lNewTick = max( lNewTick, 0 );

		// Set new Play Start 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lOrigGrid, lNewTick,
											  &mtNewMusicTime );
		if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
		{
			mtNewMusicTime = (long)m_pMotif->m_dwLength - 1;
		}

		if( m_pMotif->SetMotifPlayStart( mtNewMusicTime ) == FALSE )
		{
			m_editStartTick.SetFocus();
		}

		// Update Play Start Bar, Beat, Grid, Tick controls
		UpdatePlayStartControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif::OnTimeSignature

void CTabMotifMotif::OnTimeSignature() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		CTimeSignatureDlg tsDlg;

		tsDlg.m_TimeSignature = m_pMotif->m_TimeSignature;
		tsDlg.m_nContext = IDS_MOTIF_TEXT;

		if( tsDlg.DoModal() == IDOK )
		{
			// Update time signature
			AfxMessageBox( "Not yet implemented." );
// AMC??	m_pMotif->SetTimeSignature( tsDlg.m_TimeSignature, FALSE );

			// Update bitmap on time signature button
			SetTimeSignatureBitmap();
		}

		if( ::IsWindow( m_btnTimeSignature.m_hWnd ) )
		{
			m_btnTimeSignature.SetFocus();
		}
	}
}

void CTabMotifMotif::OnCheckResetVarOrder() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		DWORD dwFlags = m_pMotif->m_dwFlags;

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

		m_pMotif->SetFlags( dwFlags );
	}
}
