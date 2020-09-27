// TabMotifLoop.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "Pattern.h"
#include "TabMotifLoop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop property page

CTabMotifLoop::CTabMotifLoop( CMotifPropPageManager* pMotifPropPageManager ) : CPropertyPage(CTabMotifLoop::IDD)
{
	//{{AFX_DATA_INIT(CTabMotifLoop)
	//}}AFX_DATA_INIT
	
	ASSERT( pMotifPropPageManager != NULL );

	m_pMotif = NULL;
	m_pPageManager = pMotifPropPageManager;
	m_fNeedToDetach = FALSE;
	m_dwLastLoopRepeatCount = 0;
}

CTabMotifLoop::~CTabMotifLoop()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::RefreshTab

void CTabMotifLoop::RefreshTab( IDMUSProdPropPageObject* pIPropPageObject )
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
// CTabMotifLoop::EnableControls

void CTabMotifLoop::EnableControls( BOOL fEnable ) 
{
	// Disable Repeats edit/spin controls if Motif repeats is DMUS_SEG_REPEAT_INFINITE
	BOOL fEnableRepeats = fEnable;
	if( m_pMotif
	&&  m_pMotif->m_dwRepeats == DMUS_SEG_REPEAT_INFINITE )
	{
		fEnableRepeats = FALSE;
	}
	m_editRepeats.EnableWindow( fEnableRepeats );
	m_spinRepeats.EnableWindow( fEnableRepeats );

	// Disable Loop Point buttons if Motif repeats is zero
	if( m_pMotif
	&&  m_pMotif->m_dwRepeats == 0 )
	{
		fEnable = FALSE;
	}

	m_btnResetLoop.EnableWindow( fEnable );

	m_editStartBar.EnableWindow( fEnable );
	m_spinStartBar.EnableWindow( fEnable );
	m_editStartBeat.EnableWindow( fEnable );
	m_spinStartBeat.EnableWindow( fEnable );
	m_editStartGrid.EnableWindow( fEnable );
	m_spinStartGrid.EnableWindow( fEnable );
	m_editStartTick.EnableWindow( fEnable );
	m_spinStartTick.EnableWindow( fEnable );

	m_editEndBar.EnableWindow( fEnable );
	m_spinEndBar.EnableWindow( fEnable );
	m_editEndBeat.EnableWindow( fEnable );
	m_spinEndBeat.EnableWindow( fEnable );
	m_editEndGrid.EnableWindow( fEnable );
	m_spinEndGrid.EnableWindow( fEnable );
	m_editEndTick.EnableWindow( fEnable );
	m_spinEndTick.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::UpdateLoopStartEndControls

void CTabMotifLoop::UpdateLoopStartEndControls()
{
	long lBar;
	long lBeat;
	long lGrid;
	long lTick;

	if( m_pMotif )
	{
		// Get Bar, Beat, Grid, Tick of mtLoopStart
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopStart, &lBar, &lBeat, &lGrid, &lTick );

		// Set Start Bar, beat, Grid, Tick
		m_spinStartBar.SetPos( lBar );
		m_spinStartBeat.SetPos( lBeat );
		m_spinStartGrid.SetPos( lGrid );
		m_spinStartTick.SetPos( lTick );

		// Get Bar, Beat, Grid, Tick of mtLoopEnd
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopEnd, &lBar, &lBeat, &lGrid, &lTick );

		// Set End Bar, beat, Grid, Tick
		m_spinEndBar.SetPos( lBar );
		m_spinEndBeat.SetPos( lBeat );
		m_spinEndGrid.SetPos( lGrid );
		m_spinEndTick.SetPos( lTick );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::UpdateControls

void CTabMotifLoop::UpdateControls()
{
	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	if( m_pMotif )
	{
		EnableControls( TRUE );

		// Set Repeats
		if( m_pMotif->m_dwRepeats == DMUS_SEG_REPEAT_INFINITE )
		{
			m_checkInfinite.SetCheck( 1 );
			m_editRepeats.SetWindowText( NULL );
			m_dwLastLoopRepeatCount = 0;
		}
		else
		{
			m_checkInfinite.SetCheck( 0 );
			m_spinRepeats.SetPos( m_pMotif->m_dwRepeats );
			m_dwLastLoopRepeatCount = m_pMotif->m_dwRepeats;
		}

		// Set Loop Start/End Bar, Beat, Grid, Tick
		UpdateLoopStartEndControls();
	}
	else
	{
		m_spinRepeats.SetPos( 0 );
		m_editRepeats.SetWindowText( NULL );
		m_checkInfinite.SetCheck( 0 );

		m_spinStartBar.SetPos( 1 );
		m_spinStartBeat.SetPos( 1 );
		m_spinStartGrid.SetPos( 1 );
		m_spinStartTick.SetPos( 0 );
		
		m_spinEndBar.SetPos( 1 );
		m_spinEndBeat.SetPos( 1 );
		m_spinEndGrid.SetPos( 1 );
		m_spinEndTick.SetPos( 0 );

		EnableControls( FALSE );
	}
}


void CTabMotifLoop::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabMotifLoop)
	DDX_Control(pDX, IDC_CHECK_REPEAT_FOREVER, m_checkInfinite);
	DDX_Control(pDX, IDC_RESET_LOOP, m_btnResetLoop);
	DDX_Control(pDX, IDC_EDIT_LOOP_REPEAT, m_editRepeats);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_BAR, m_editStartBar);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_BEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_GRID, m_editStartGrid);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_TICK, m_editStartTick);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_BAR, m_editEndBar);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_BEAT, m_editEndBeat);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_GRID, m_editEndGrid);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_TICK, m_editEndTick);
	DDX_Control(pDX, IDC_SPIN_LOOP_REPEAT, m_spinRepeats);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_BAR, m_spinStartBar);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_BEAT, m_spinStartBeat);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_GRID, m_spinStartGrid);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_TICK, m_spinStartTick);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_BAR, m_spinEndBar);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_BEAT, m_spinEndBeat);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_GRID, m_spinEndGrid);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_TICK, m_spinEndTick);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabMotifLoop, CPropertyPage)
	//{{AFX_MSG_MAP(CTabMotifLoop)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_REPEAT, OnKillFocusEditRepeats)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_REPEAT, OnDeltaPosSpinRepeats)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_BAR, OnKillFocusEditLoopStartBar)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_BEAT, OnKillFocusEditLoopStartBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_GRID, OnKillFocusEditLoopStartGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_TICK, OnKillFocusEditLoopStartTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_BAR, OnDeltaPosSpinLoopStartBar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_BEAT, OnDeltaPosSpinLoopStartBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_GRID, OnDeltaPosSpinLoopStartGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_TICK, OnDeltaPosSpinLoopStartTick)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_BAR, OnKillFocusEditLoopEndBar)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_BEAT, OnKillFocusEditLoopEndBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_GRID, OnKillFocusEditLoopEndGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_TICK, OnKillFocusEditLoopEndTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_BAR, OnDeltaPosSpinLoopEndBar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_BEAT, OnDeltaPosSpinLoopEndBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_GRID, OnDeltaPosSpinLoopEndGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_TICK, OnDeltaPosSpinLoopEndTick)
	ON_BN_CLICKED(IDC_RESET_LOOP, OnResetLoop)
	ON_BN_DOUBLECLICKED(IDC_RESET_LOOP, OnDoubleClickedResetLoop)
	ON_BN_CLICKED(IDC_CHECK_REPEAT_FOREVER, OnCheckInfinite)
	ON_BN_DOUBLECLICKED(IDC_CHECK_REPEAT_FOREVER, OnDoubleClickedInfinite)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnSetActive

BOOL CTabMotifLoop::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnCreate

int CTabMotifLoop::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabMotifLoop::OnDestroy

void CTabMotifLoop::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CTabMotifLoop::OnInitDialog

BOOL CTabMotifLoop::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Set control ranges

	m_spinRepeats.SetRange( 0, 32767 );
	m_editRepeats.LimitText( 5 );

	m_spinStartBar.SetRange( 1, 32767 );
	m_editStartBar.LimitText( 5 );
	m_spinStartBeat.SetRange( 1, 256 );
	m_editStartBeat.LimitText( 3 );
	m_spinStartGrid.SetRange( 1, 256 );
	m_editStartGrid.LimitText( 3 );
	m_spinStartTick.SetRange( 0, 32767 );
	m_editStartTick.LimitText( 5 );
	
	m_spinEndBar.SetRange( 1, 32767 );
	m_editEndBar.LimitText( 5 );
	m_spinEndBeat.SetRange( 1, 256 );
	m_editEndBeat.LimitText( 3 );
	m_spinEndGrid.SetRange( 1, 256 );
	m_editEndGrid.LimitText( 3 );
	m_spinEndTick.SetRange( 0, 32767 );
	m_editEndTick.LimitText( 5 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnKillFocusEditRepeats

void CTabMotifLoop::OnKillFocusEditRepeats() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Get string from control
		CString strNewRepeats;
		m_editRepeats.GetWindowText( strNewRepeats );

		// Strip leading and trailing spaces
		strNewRepeats.TrimRight();
		strNewRepeats.TrimLeft();

		if( strNewRepeats.IsEmpty() )
		{
			m_spinRepeats.SetPos( m_pMotif->m_dwRepeats );
		}
		else
		{
			long lNewRepeats = _ttoi( strNewRepeats );
			lNewRepeats = max( lNewRepeats, 0 );
			lNewRepeats = min( lNewRepeats, 32767 );

			// Set new Repeats 
			m_pMotif->SetMotifRepeats( lNewRepeats );
			m_dwLastLoopRepeatCount = m_pMotif->m_dwRepeats;
			EnableControls( TRUE );

			// Update controls
			m_checkInfinite.SetCheck( 0 );
			m_spinRepeats.SetPos( m_pMotif->m_dwRepeats );
			if( m_pMotif->m_dwRepeats == 0 )
			{
				OnResetLoop();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDeltaPosSpinRepeats

void CTabMotifLoop::OnDeltaPosSpinRepeats( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get new Repeats
		long lNewRepeats = m_spinRepeats.GetPos() + pNMUpDown->iDelta;
		lNewRepeats = max( lNewRepeats, 0 );
		lNewRepeats = min( lNewRepeats, 32767 );

		// Set new Repeats 
		m_pMotif->SetMotifRepeats( lNewRepeats );
		m_dwLastLoopRepeatCount = m_pMotif->m_dwRepeats;
		EnableControls( TRUE );

		// Update controls
		m_checkInfinite.SetCheck( 0 );
		m_spinRepeats.SetPos( m_pMotif->m_dwRepeats );
		if( m_pMotif->m_dwRepeats == 0 )
		{
			OnResetLoop();
		}
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnCheckInfinite

void CTabMotifLoop::OnCheckInfinite() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Set new Repeats 
		m_pMotif->SetMotifRepeats( m_checkInfinite.GetCheck() ? DMUS_SEG_REPEAT_INFINITE : m_dwLastLoopRepeatCount );
		EnableControls( TRUE );

		// Update controls
		if( m_checkInfinite.GetCheck() )
		{
			m_editRepeats.SetWindowText( NULL );
		}
		else
		{
			SetDlgItemInt( IDC_EDIT_LOOP_REPEAT, m_pMotif->m_dwRepeats, FALSE );
		}
		if( m_pMotif->m_dwRepeats == 0 )
		{
			OnResetLoop();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDoubleClickedInfinite

void CTabMotifLoop::OnDoubleClickedInfinite() 
{
	OnCheckInfinite();
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnResetLoop

void CTabMotifLoop::OnResetLoop() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Reset Loop Start/End 
		m_pMotif->ResetMotifLoopPoints();

		// Update Loop Start Bar, Beat, Grid, Tick controls
		UpdateLoopStartEndControls();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDoubleClickedResetLoop

void CTabMotifLoop::OnDoubleClickedResetLoop() 
{
	OnResetLoop();
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnKillFocusEditLoopStartBar

void CTabMotifLoop::OnKillFocusEditLoopStartBar() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Get original Bar, Beat, Grid, Tick of mtLoopStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopStart,
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

			// Set new Loop Start 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lNewBar, lOrigBeat, lOrigGrid, lOrigTick,
												  &mtNewMusicTime );
			m_pMotif->SetMotifLoopStart( mtNewMusicTime );

			// Update Loop Start Bar, Beat, Grid, Tick controls
			UpdateLoopStartEndControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnKillFocusEditLoopStartBeat

void CTabMotifLoop::OnKillFocusEditLoopStartBeat() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Get original Bar, Beat, Grid, Tick of mtLoopStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopStart,
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

			// Set new Loop Start 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
												  &mtNewMusicTime );
			if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
			{
				lNewBeat--;
				m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
													  &mtNewMusicTime );
			}
			m_pMotif->SetMotifLoopStart( mtNewMusicTime );

			// Update Loop Start Bar, Beat, Grid, Tick controls
			UpdateLoopStartEndControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnKillFocusEditLoopStartGrid

void CTabMotifLoop::OnKillFocusEditLoopStartGrid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Get original Bar, Beat, Grid, Tick of mtLoopStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopStart,
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

			// Set new Loop Start 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
												  &mtNewMusicTime );
			if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
			{
				lNewGrid--;
				m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
													  &mtNewMusicTime );
			}
			m_pMotif->SetMotifLoopStart( mtNewMusicTime );

			// Update Loop Start Bar, Beat, Grid, Tick controls
			UpdateLoopStartEndControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnKillFocusEditLoopStartTick

void CTabMotifLoop::OnKillFocusEditLoopStartTick() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		long lClocksPerBeat = DM_PPQNx4 / (long)m_pMotif->m_TimeSignature.m_bBeat;
		long lClocksPerGrid = lClocksPerBeat / (long)m_pMotif->m_TimeSignature.m_wGridsPerBeat;
				
		// Get original Bar, Beat, Grid, Tick of mtLoopStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopStart,
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

			// Set new Loop Start 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lOrigGrid, lNewTick,
												  &mtNewMusicTime );
			if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
			{
				m_pMotif->SetMotifLoopStart( (long)m_pMotif->m_dwLength - 1 );
			}
			else
			{
				m_pMotif->SetMotifLoopStart( mtNewMusicTime );
			}

			// Update Loop Start Bar, Beat, Grid, Tick controls
			UpdateLoopStartEndControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDeltaPosSpinLoopStartBar

void CTabMotifLoop::OnDeltaPosSpinLoopStartBar( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtLoopStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Bar
		long lNewBar = m_spinStartBar.GetPos() + pNMUpDown->iDelta;
		lNewBar = max( lNewBar, 1 );
		lNewBar = min( lNewBar, m_pMotif->m_wNbrMeasures );

		// Set new Loop Start 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lNewBar, lOrigBeat, lOrigGrid, lOrigTick,
											  &mtNewMusicTime );
		if( m_pMotif->SetMotifLoopStart( mtNewMusicTime ) == FALSE )
		{
			m_editStartBar.SetFocus();
		}

		// Update Loop Start Bar, Beat, Grid, Tick controls
		UpdateLoopStartEndControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDeltaPosSpinLoopStartBeat

void CTabMotifLoop::OnDeltaPosSpinLoopStartBeat( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtLoopStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Beat
		long lNewBeat = m_spinStartBeat.GetPos() + pNMUpDown->iDelta;
		lNewBeat = max( lNewBeat, 1 );

		// Set new Loop Start 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
											  &mtNewMusicTime );
		if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
		{
			lNewBeat--;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
												  &mtNewMusicTime );
		}

		if( m_pMotif->SetMotifLoopStart( mtNewMusicTime ) == FALSE )
		{
			m_editStartBeat.SetFocus();
		}

		// Update Loop Start Bar, Beat, Grid, Tick controls
		UpdateLoopStartEndControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDeltaPosSpinLoopStartGrid

void CTabMotifLoop::OnDeltaPosSpinLoopStartGrid( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtLoopStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Grid
		long lNewGrid = m_spinStartGrid.GetPos() + pNMUpDown->iDelta;
		lNewGrid = max( lNewGrid, 1 );

		// Set new Loop Start 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
											  &mtNewMusicTime );
		if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
		{
			lNewGrid--;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
												  &mtNewMusicTime );
		}

		if( m_pMotif->SetMotifLoopStart( mtNewMusicTime ) == FALSE )
		{
			m_editStartGrid.SetFocus();
		}

		// Update Loop Start Bar, Beat, Grid, Tick controls
		UpdateLoopStartEndControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDeltaPosSpinLoopStartTick

void CTabMotifLoop::OnDeltaPosSpinLoopStartTick( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtLoopStart
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopStart,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Tick
		long lNewTick = m_spinStartTick.GetPos() + pNMUpDown->iDelta;
		lNewTick = max( lNewTick, 0 );

		// Set new Loop Start 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lOrigGrid, lNewTick,
											  &mtNewMusicTime );
		if( mtNewMusicTime >= (long)m_pMotif->m_dwLength )
		{
			mtNewMusicTime = (long)m_pMotif->m_dwLength - 1;
		}

		if( m_pMotif->SetMotifLoopStart( mtNewMusicTime ) == FALSE )
		{
			m_editStartTick.SetFocus();
		}

		// Update Loop Start Bar, Beat, Grid, Tick controls
		UpdateLoopStartEndControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnKillFocusEditLoopEndBar

void CTabMotifLoop::OnKillFocusEditLoopEndBar() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Get original Bar, Beat, Grid, Tick of mtLoopEnd
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopEnd,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get string from control
		CString strNewEndBar;
		m_editEndBar.GetWindowText( strNewEndBar );

		// Strip leading and trailing spaces
		strNewEndBar.TrimRight();
		strNewEndBar.TrimLeft();

		if( strNewEndBar.IsEmpty() )
		{
			m_spinEndBar.SetPos( lOrigBar );
		}
		else
		{
			long lNewBar = _ttoi( strNewEndBar );
			lNewBar = max( lNewBar, 1 );
			lNewBar = min( lNewBar, m_pMotif->m_wNbrMeasures + 1 );

			// Set new Loop End 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lNewBar, lOrigBeat, lOrigGrid, lOrigTick,
												  &mtNewMusicTime );
			m_pMotif->SetMotifLoopEnd( mtNewMusicTime );

			// Update Loop End Bar, Beat, Grid, Tick controls
			UpdateLoopStartEndControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnKillFocusEditLoopEndBeat

void CTabMotifLoop::OnKillFocusEditLoopEndBeat() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Get original Bar, Beat, Grid, Tick of mtLoopEnd
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopEnd,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get string from control
		CString strNewEndBeat;
		m_editEndBeat.GetWindowText( strNewEndBeat );

		// Strip leading and trailing spaces
		strNewEndBeat.TrimRight();
		strNewEndBeat.TrimLeft();

		if( strNewEndBeat.IsEmpty() )
		{
			m_spinEndBeat.SetPos( lOrigBeat );
		}
		else
		{
			long lNewBeat = _ttoi( strNewEndBeat );
			lNewBeat = max( lNewBeat, 1 );
			lNewBeat = min( lNewBeat, m_pMotif->m_TimeSignature.m_bBeatsPerMeasure );

			// Set new Loop End 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
												  &mtNewMusicTime );
			if( mtNewMusicTime > (long)m_pMotif->m_dwLength )
			{
				lNewBeat--;
				m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
													  &mtNewMusicTime );
			}
			m_pMotif->SetMotifLoopEnd( mtNewMusicTime );

			// Update Loop End Bar, Beat, Grid, Tick controls
			UpdateLoopStartEndControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnKillFocusEditLoopEndGrid

void CTabMotifLoop::OnKillFocusEditLoopEndGrid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		// Get original Bar, Beat, Grid, Tick of mtLoopEnd
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopEnd,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get string from control
		CString strNewEndGrid;
		m_editEndGrid.GetWindowText( strNewEndGrid );

		// Strip leading and trailing spaces
		strNewEndGrid.TrimRight();
		strNewEndGrid.TrimLeft();

		if( strNewEndGrid.IsEmpty() )
		{
			m_spinEndGrid.SetPos( lOrigGrid );
		}
		else
		{
			long lNewGrid = _ttoi( strNewEndGrid );
			lNewGrid = max( lNewGrid, 1 );
			lNewGrid = min( lNewGrid, m_pMotif->m_TimeSignature.m_wGridsPerBeat );

			// Set new Loop End 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
												  &mtNewMusicTime );
			if( mtNewMusicTime > (long)m_pMotif->m_dwLength )
			{
				lNewGrid--;
				m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
													  &mtNewMusicTime );
			}
			m_pMotif->SetMotifLoopEnd( mtNewMusicTime );

			// Update Loop End Bar, Beat, Grid, Tick controls
			UpdateLoopStartEndControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnKillFocusEditLoopEndTick

void CTabMotifLoop::OnKillFocusEditLoopEndTick() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		long lClocksPerBeat = DM_PPQNx4 / (long)m_pMotif->m_TimeSignature.m_bBeat;
		long lClocksPerGrid = lClocksPerBeat / (long)m_pMotif->m_TimeSignature.m_wGridsPerBeat;
				
		// Get original Bar, Beat, Grid, Tick of mtLoopEnd
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopEnd,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get string from control
		CString strNewEndTick;
		m_editEndTick.GetWindowText( strNewEndTick );

		// Strip leading and trailing spaces
		strNewEndTick.TrimRight();
		strNewEndTick.TrimLeft();

		if( strNewEndTick.IsEmpty() )
		{
			m_spinEndTick.SetPos( lOrigTick );
		}
		else
		{
			long lNewTick = _ttoi( strNewEndTick );
			lNewTick = max( lNewTick, 0 );
			lNewTick = min( lNewTick, lClocksPerGrid - 1 );

			// Set new Loop End 
			MUSIC_TIME mtNewMusicTime;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lOrigGrid, lNewTick,
												  &mtNewMusicTime );
			if( mtNewMusicTime > (long)m_pMotif->m_dwLength )
			{
				m_pMotif->SetMotifLoopEnd( (long)m_pMotif->m_dwLength );
			}
			else
			{
				m_pMotif->SetMotifLoopEnd( mtNewMusicTime );
			}

			// Update Loop End Bar, Beat, Grid, Tick controls
			UpdateLoopStartEndControls();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDeltaPosSpinLoopEndBar

void CTabMotifLoop::OnDeltaPosSpinLoopEndBar( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtLoopEnd
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopEnd,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Bar
		long lNewBar = m_spinEndBar.GetPos() + pNMUpDown->iDelta;
		lNewBar = max( lNewBar, 1 );
		lNewBar = min( lNewBar, m_pMotif->m_wNbrMeasures + 1 );

		// Set new Loop End 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lNewBar, lOrigBeat, lOrigGrid, lOrigTick,
											  &mtNewMusicTime );
		if( m_pMotif->SetMotifLoopEnd( mtNewMusicTime ) == FALSE )
		{
			m_editEndBar.SetFocus();
		}

		// Update Loop End Bar, Beat, Grid, Tick controls
		UpdateLoopStartEndControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDeltaPosSpinLoopEndBeat

void CTabMotifLoop::OnDeltaPosSpinLoopEndBeat( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtLoopEnd
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopEnd,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Beat
		long lNewBeat = m_spinEndBeat.GetPos() + pNMUpDown->iDelta;
		lNewBeat = max( lNewBeat, 1 );

		// Set new Loop End 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
											  &mtNewMusicTime );
		if( mtNewMusicTime > (long)m_pMotif->m_dwLength )
		{
			lNewBeat--;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lNewBeat, lOrigGrid, lOrigTick,
												  &mtNewMusicTime );
		}

		if( m_pMotif->SetMotifLoopEnd( mtNewMusicTime ) == FALSE )
		{
			m_editEndBeat.SetFocus();
		}

		// Update Loop End Bar, Beat, Grid, Tick controls
		UpdateLoopStartEndControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDeltaPosSpinLoopEndGrid

void CTabMotifLoop::OnDeltaPosSpinLoopEndGrid( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtLoopEnd
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopEnd,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Grid
		long lNewGrid = m_spinEndGrid.GetPos() + pNMUpDown->iDelta;
		lNewGrid = max( lNewGrid, 1 );

		// Set new Loop End 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
											  &mtNewMusicTime );
		if( mtNewMusicTime > (long)m_pMotif->m_dwLength )
		{
			lNewGrid--;
			m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lNewGrid, lOrigTick,
												  &mtNewMusicTime );
		}

		if( m_pMotif->SetMotifLoopEnd( mtNewMusicTime ) == FALSE )
		{
			m_editEndGrid.SetFocus();
		}

		// Update Loop End Bar, Beat, Grid, Tick controls
		UpdateLoopStartEndControls();
	}

	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop::OnDeltaPosSpinLoopEndTick

void CTabMotifLoop::OnDeltaPosSpinLoopEndTick( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pMotif )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Get original Bar, Beat, Grid, Tick of mtLoopEnd
		long lOrigBar;
		long lOrigBeat;
		long lOrigGrid;
		long lOrigTick;
		m_pMotif->MusicTimeToBarBeatGridTick( m_pMotif->m_mtLoopEnd,
											  &lOrigBar, &lOrigBeat, &lOrigGrid, &lOrigTick );

		// Get new Tick
		long lNewTick = m_spinEndTick.GetPos() + pNMUpDown->iDelta;
		lNewTick = max( lNewTick, 0 );

		// Set new Loop End 
		MUSIC_TIME mtNewMusicTime;
		m_pMotif->BarBeatGridTickToMusicTime( lOrigBar, lOrigBeat, lOrigGrid, lNewTick,
											  &mtNewMusicTime );
		if( mtNewMusicTime > (long)m_pMotif->m_dwLength )
		{
			mtNewMusicTime = (long)m_pMotif->m_dwLength;
		}

		if( m_pMotif->SetMotifLoopEnd( mtNewMusicTime ) == FALSE )
		{
			m_editEndTick.SetFocus();
		}

		// Update Loop End Bar, Beat, Grid, Tick controls
		UpdateLoopStartEndControls();
	}

	*pResult = 1;
}
