// LoopPPG.cpp : implementation file
//

#include "stdafx.h"
#include "LoopPPG.h"
#include "SegmentPPGMgr.h"
#include "dmusicf.h"
#include "segment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NOTE_TO_CLOCKS(note, ppq)	( (ppq) * 4 / (note) )

void TimeToBarBeatGridTick( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, MUSIC_TIME mtTime, DWORD &dwMeasure, BYTE &bBeat, WORD &wGrid, WORD &wTick )
{
	ASSERT( pTimeline );

	if( mtTime <= 0 )
	{
		dwMeasure = 0;
		bBeat = 0;
		wGrid = 0;
		wTick = 0;
		return;
	}

	// Find out which measure we're in
	if( SUCCEEDED( pTimeline->ClocksToMeasureBeat( dwGroupBits, 0, mtTime, (long *)&dwMeasure, NULL ) ) )
	{
		// Find the time of the start of this measure
		long lTime;
		if( SUCCEEDED( pTimeline->MeasureBeatToClocks( dwGroupBits, 0, dwMeasure, 0, &lTime ) ) )
		{
			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE ts;
			if( SUCCEEDED( pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, lTime, NULL, &ts ) ) )
			{
				// Compute the number of clocks in a beat and a grid
				long lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
				long lGridClocks = lBeatClocks / ts.wGridsPerBeat;

				// Convert mtTime into an offset from the start of this measure
				mtTime -= lTime;

				bBeat = BYTE(min( UCHAR_MAX, mtTime / lBeatClocks));

				mtTime %= lBeatClocks;
				wGrid =  WORD(min( USHRT_MAX, mtTime / lGridClocks));

				wTick = WORD(min( USHRT_MAX, mtTime % lGridClocks));
			}
		}
	}
}

void BarBeatGridTickToTime( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, DWORD dwMeasure, long lBeat, long lGrid, long lTick, MUSIC_TIME &mtTime )
{
	ASSERT( pTimeline );

	// Find the time of the start of this measure
	if( SUCCEEDED( pTimeline->MeasureBeatToClocks( dwGroupBits, 0, dwMeasure, 0, &mtTime ) ) )
	{
		// Get the TimeSig for this measure
		DMUS_TIMESIGNATURE ts;
		if( SUCCEEDED( pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, mtTime, NULL, &ts ) ) )
		{
			// Compute the number of clocks in a beat and a grid
			const long lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
			const long lGridClocks = lBeatClocks / ts.wGridsPerBeat;

			// Compute mtTime
			mtTime += lBeatClocks * lBeat + lGridClocks * lGrid + lTick;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CLoopPPG property page

IMPLEMENT_DYNCREATE(CLoopPPG, CPropertyPage)

CLoopPPG::CLoopPPG() : CPropertyPage(CLoopPPG::IDD)
{
	//{{AFX_DATA_INIT(CLoopPPG)
	m_dwClockLoopStart = 0;
	m_dwClockLoopEnd = 0;
	//}}AFX_DATA_INIT

	m_pPPGSegment	= NULL;
	m_pPageManager	= NULL;
	m_wMaxStartBeat = 0;
	m_wMaxStartGrid = 0;
	m_dwMaxStartTick = 0;
	m_wMaxEndBeat = 0;
	m_wMaxEndGrid = 0;
	m_dwMaxEndTick = 0;
	m_dwLastLoopRepeatCount = 0;
	m_fNeedToDetach = FALSE;
}

CLoopPPG::~CLoopPPG()
{
	if(m_pPPGSegment != NULL)
	{
		delete m_pPPGSegment;
		m_pPPGSegment = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLoopPPG::SetData

HRESULT CLoopPPG::SetData(const struct PPGSegment *pSegment)
{
	CString str;

	if(pSegment == NULL)
	{
		// Set m_pPPGSegment to NULL
		if(m_pPPGSegment != NULL)
		{
			delete m_pPPGSegment;
		}
		m_pPPGSegment = NULL;
		return S_OK;
	}

	if(m_pPPGSegment == NULL)
	{
		m_pPPGSegment = new PPGSegment;
		if( m_pPPGSegment == NULL )
		{
			return E_OUTOFMEMORY;
		}
	}

	// Copy the PPGSegment
	m_pPPGSegment->Copy( pSegment );

	m_pPPGSegment->dwFlags = (m_pPPGSegment->dwFlags & PPGT_NONVALIDFLAGS) | PPGT_VALIDLOOP;

	// Set the states of the controls
	if( !IsWindow(m_hWnd) )
	{
		return S_OK;
	}

	if( m_pPPGSegment->dwLoopRepeats == DMUS_SEG_REPEAT_INFINITE )
	{
		m_editLoopRepeat.SetWindowText( NULL );
		CheckDlgButton( IDC_CHECK_REPEAT_FOREVER, BST_CHECKED );
		m_dwLastLoopRepeatCount = 0;
	}
	else
	{
		m_spinRepeat.SetPos( m_pPPGSegment->dwLoopRepeats );
		CheckDlgButton( IDC_CHECK_REPEAT_FOREVER, BST_UNCHECKED );
		m_dwLastLoopRepeatCount = m_pPPGSegment->dwLoopRepeats;
	}

	m_spinStartBar.SetPos( m_pPPGSegment->dwLoopStartMeasure + 1 );
	m_spinStartBeat.SetPos( m_pPPGSegment->bLoopStartBeat + 1 );
	m_spinStartGrid.SetPos( m_pPPGSegment->wLoopStartGrid + 1 );
	m_spinStartTick.SetPos( m_pPPGSegment->wLoopStartTick );

	m_spinEndBar.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
	m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
	m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
	m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );

//#ifdef DMP_XBOX
    CheckDlgButton( IDC_CHECK_LOOP_CLOCKTIME, (m_pPPGSegment->dwSegmentFlags & DMUS_SEGIOF_REFLOOP) ? BST_CHECKED : BST_UNCHECKED );
    SetDlgItemInt( IDC_EDIT_LOOP_CLOCKSTART,(UINT) (m_pPPGSegment->rtLoopStart / 10000),FALSE); 
    SetDlgItemInt( IDC_EDIT_LOOP_CLOCKEND,(UINT) (m_pPPGSegment->rtLoopEnd / 10000),FALSE); 
    m_dwClockLoopStart = (DWORD) m_pPPGSegment->rtLoopStart / 10000;
    m_dwClockLoopEnd = (DWORD) m_pPPGSegment->rtLoopEnd / 10000;
//#endif
	EnableControls( TRUE );

	ResetStartRanges();
	ResetEndRanges();

	UpdateData(FALSE);
	return S_OK;
}

void CLoopPPG::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoopPPG)
	DDX_Control(pDX, IDC_RESET_LOOP, m_btnResetLoop);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_TICK, m_editStartTick);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_GRID, m_editStartGrid);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_BEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_BAR, m_editStartBar);
	DDX_Control(pDX, IDC_EDIT_LOOP_REPEAT, m_editLoopRepeat);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_TICK, m_editEndTick);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_GRID, m_editEndGrid);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_BEAT, m_editEndBeat);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_BAR, m_editEndBar);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_TICK, m_spinStartTick);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_GRID, m_spinStartGrid);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_BEAT, m_spinStartBeat);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_BAR, m_spinStartBar);
	DDX_Control(pDX, IDC_SPIN_LOOP_REPEAT, m_spinRepeat);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_TICK, m_spinEndTick);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_GRID, m_spinEndGrid);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_BEAT, m_spinEndBeat);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_BAR, m_spinEndBar);
	DDX_Text(pDX, IDC_EDIT_LOOP_CLOCKSTART, m_dwClockLoopStart);
	DDX_Text(pDX, IDC_EDIT_LOOP_CLOCKEND, m_dwClockLoopEnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoopPPG, CPropertyPage)
	//{{AFX_MSG_MAP(CLoopPPG)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_BAR, OnKillfocusEditLoopEndBar)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_BEAT, OnKillfocusEditLoopEndBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_GRID, OnKillfocusEditLoopEndGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_TICK, OnKillfocusEditLoopEndTick)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_REPEAT, OnKillfocusEditLoopRepeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_BAR, OnKillfocusEditLoopStartBar)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_BEAT, OnKillfocusEditLoopStartBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_GRID, OnKillfocusEditLoopStartGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_TICK, OnKillfocusEditLoopStartTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_BAR, OnDeltaposSpinLoopEndBar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_BEAT, OnDeltaposSpinLoopEndBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_GRID, OnDeltaposSpinLoopEndGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_TICK, OnDeltaposSpinLoopEndTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_REPEAT, OnDeltaposSpinLoopRepeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_BAR, OnDeltaposSpinLoopStartBar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_BEAT, OnDeltaposSpinLoopStartBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_GRID, OnDeltaposSpinLoopStartGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_TICK, OnDeltaposSpinLoopStartTick)
	ON_BN_CLICKED(IDC_RESET_LOOP, OnResetLoop)
	ON_BN_DOUBLECLICKED(IDC_RESET_LOOP, OnDoubleClickedResetLoop)
	ON_EN_CHANGE(IDC_EDIT_LOOP_REPEAT, OnChangeEditLoopRepeat)
	ON_BN_CLICKED(IDC_CHECK_REPEAT_FOREVER, OnCheckRepeatForever)
	ON_BN_CLICKED(IDC_CHECK_LOOP_CLOCKTIME, OnCheckLoopClocktime)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_CLOCKEND, OnKillfocusEditLoopClockend)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_CLOCKSTART, OnKillfocusEditLoopClockstart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoopPPG::OnInitDialog

BOOL CLoopPPG::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();
	
	m_editLoopRepeat.EnableWindow( TRUE );
	m_spinRepeat.EnableWindow( TRUE );
	m_spinRepeat.SetRange( 0, 32767 );

	m_spinStartTick.SetRange( 0, 32767 );
	m_spinStartGrid.SetRange( 1, 256 );
	m_spinStartBeat.SetRange( 1, 256 );
	m_spinStartBar.SetRange( 1, 32767 );
	m_spinEndTick.SetRange( 0, 32767 );
	m_spinEndGrid.SetRange( 1, 256 );
	m_spinEndBeat.SetRange( 1, 256 );
	m_spinEndBar.SetRange( 1, 32767 );

/*#ifndef DMP_XBOX
    GetDlgItem( IDC_EDIT_LOOP_CLOCKSTART )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_EDIT_LOOP_CLOCKEND )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_CHECK_LOOP_CLOCKTIME )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_CLOCK_TIME_BORDER )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_CLOCK_TIME_START_NAME )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_CLOCK_TIME_END_NAME )->ShowWindow(SW_HIDE );
#endif*/
    SetData(m_pPPGSegment);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CLoopPPG::EnableControls

void CLoopPPG::EnableControls(BOOL fEnable) 
{
	BOOL fEnableRepeat = fEnable;
	if( fEnableRepeat )
	{
		if( m_pPPGSegment->pITimelineCtl == NULL )
		{
			fEnableRepeat = FALSE;
		}
	}

	if( fEnableRepeat
	&&	(m_pPPGSegment->dwLoopRepeats != DMUS_SEG_REPEAT_INFINITE) )
	{
		m_editLoopRepeat.EnableWindow( TRUE );
		m_spinRepeat.EnableWindow( TRUE );
		EnableItem( IDC_CHECK_REPEAT_FOREVER, TRUE );
	}
	else
	{
		m_editLoopRepeat.EnableWindow( FALSE );
		m_spinRepeat.EnableWindow( FALSE );
		EnableItem( IDC_CHECK_REPEAT_FOREVER, fEnableRepeat );
	}

	if( fEnable )
	{
		if( (m_pPPGSegment->pITimelineCtl == NULL)
		||  (m_pPPGSegment->dwLoopRepeats == 0 ))
		{
			fEnable = FALSE;
		}
	}

    EnableItem( IDC_RESET_LOOP, fEnable );

    bool fMusicTime = fEnable && !(m_pPPGSegment->dwSegmentFlags & DMUS_SEGIOF_REFLOOP);
    bool fClockTime = fEnable && (m_pPPGSegment->dwSegmentFlags & DMUS_SEGIOF_REFLOOP);

	EnableItem( IDC_EDIT_LOOP_START_BAR, fMusicTime );
	EnableItem( IDC_EDIT_LOOP_START_BEAT, fMusicTime );
	EnableItem( IDC_EDIT_LOOP_START_GRID, fMusicTime );
	EnableItem( IDC_EDIT_LOOP_START_TICK, fMusicTime );
	EnableItem( IDC_SPIN_LOOP_START_BAR, fMusicTime );
	EnableItem( IDC_SPIN_LOOP_START_BEAT, fMusicTime );
	EnableItem( IDC_SPIN_LOOP_START_GRID, fMusicTime );
	EnableItem( IDC_SPIN_LOOP_START_TICK, fMusicTime );

	EnableItem( IDC_EDIT_LOOP_END_BAR, fMusicTime );
	EnableItem( IDC_EDIT_LOOP_END_BEAT, fMusicTime );
	EnableItem( IDC_EDIT_LOOP_END_GRID, fMusicTime );
	EnableItem( IDC_EDIT_LOOP_END_TICK, fMusicTime );
	EnableItem( IDC_SPIN_LOOP_END_BAR, fMusicTime );
	EnableItem( IDC_SPIN_LOOP_END_BEAT, fMusicTime );
	EnableItem( IDC_SPIN_LOOP_END_GRID, fMusicTime );
	EnableItem( IDC_SPIN_LOOP_END_TICK, fMusicTime );

	EnableItem( IDC_EDIT_LOOP_CLOCKSTART, fClockTime );
	EnableItem( IDC_EDIT_LOOP_CLOCKEND, fClockTime );

}

/////////////////////////////////////////////////////////////////////////////
// CLoopPPG::ResetLoopControls

void CLoopPPG::ResetLoopControls()
{
	m_pPPGSegment->dwLoopStartMeasure = 0;
	m_pPPGSegment->bLoopStartBeat = 0;
	m_pPPGSegment->wLoopStartGrid = 0;
	m_pPPGSegment->wLoopStartTick = 0;

	m_pPPGSegment->dwLoopEndMeasure = 0;
	m_pPPGSegment->bLoopEndBeat = 0;
	m_pPPGSegment->wLoopEndGrid = 0;
	m_pPPGSegment->wLoopEndTick = 0;

	m_spinStartBar.SetPos( m_pPPGSegment->dwLoopStartMeasure + 1 );
	m_spinStartBeat.SetPos( m_pPPGSegment->bLoopStartBeat + 1 );
	m_spinStartGrid.SetPos( m_pPPGSegment->wLoopStartGrid + 1 );
	m_spinStartTick.SetPos( m_pPPGSegment->wLoopStartTick );

	m_spinEndBar.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
	m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
	m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
	m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );
//#ifdef DMP_XBOX
    SetDlgItemInt( IDC_EDIT_LOOP_CLOCKSTART,(UINT) (m_pPPGSegment->rtLoopStart / 10000),FALSE); 
    SetDlgItemInt( IDC_EDIT_LOOP_CLOCKEND,(UINT) (m_pPPGSegment->rtLoopEnd / 10000),FALSE); 
    m_dwClockLoopStart = (DWORD) m_pPPGSegment->rtLoopStart / 10000;
    m_dwClockLoopEnd = (DWORD) m_pPPGSegment->rtLoopEnd / 10000;
//#endif
}

/////////////////////////////////////////////////////////////////////////////
// CLoopPPG message handlers

/////////////////////////////////////////////////////////////////////////////
// CLoopPPG::OnSetActive

BOOL CLoopPPG::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pPPGSegment == NULL)
	{
		EnableControls(FALSE);
		return CPropertyPage::OnSetActive();
	}

	EnableControls(TRUE);

	// Set the controls in case they have changed since this was last activated
	// and RefreshData has not yet been called.
	SetData(m_pPPGSegment);

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CSegmentPPGMgr::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CLoopPPG::OnClose

void CLoopPPG::OnClose() 
{	
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnClose();
}


/////////////////////////////////////////////////////////////////////////////
// CLoopPPG::OnCreate

int CLoopPPG::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	if(CPropertyPage::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CLoopPPG::OnDestroy

void CLoopPPG::OnDestroy() 
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
// CLoopPPG::OnDestroy

void CLoopPPG::EnableItem(int nItem, BOOL fEnable)
{
	CWnd* pWnd;
	pWnd = GetDlgItem(nItem);
	if (pWnd)
	{
		BOOL fRes = pWnd->EnableWindow( fEnable );
		if ( (fRes && fEnable) || ( !fRes && !fEnable ) )
		{
			// BUGBUG: Is this necessary?
		//	RECT rect;
		//	pWnd->GetWindowRect(&rect);
		//	ScreenToClient(&rect);
		//	InvalidateRect(&rect);
		}
	}
}

void CLoopPPG::OnResetLoop() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	DMUS_TIMESIGNATURE TimeSig;
	if( GetTimeSig( TimeSig, m_pPPGSegment->dwMeasureLength - 1 ) )
	{
		// Set the End time to be m_pPPGSegment->mtSegmentLength
		TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, m_pPPGSegment->mtSegmentLength,
		m_pPPGSegment->dwLoopEndMeasure, m_pPPGSegment->bLoopEndBeat, m_pPPGSegment->wLoopEndGrid, m_pPPGSegment->wLoopEndTick );

		m_spinEndBar.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
		m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
		m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
		m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );

		// Reset Start to 0
		m_pPPGSegment->dwLoopStartMeasure = 0;
		m_pPPGSegment->bLoopStartBeat = 0;
		m_pPPGSegment->wLoopStartGrid = 0;
		m_pPPGSegment->wLoopStartTick = 0;
		m_spinStartTick.SetPos( 0 );
		m_spinStartBar.SetPos( 1 );
		m_spinStartBeat.SetPos( 1 );
		m_spinStartGrid.SetPos( 1 );

		ResetEndRanges();
		ResetStartRanges();
		UpdateSegment();
	}
}

void CLoopPPG::OnDoubleClickedResetLoop() 
{
	OnResetLoop();
}

void CLoopPPG::OnKillfocusEditLoopEndBar() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		if( m_pPPGSegment->pITimelineCtl )
		{
			CString strNewEndBar;

			m_editEndBar.GetWindowText( strNewEndBar );

			// Strip leading and trailing spaces
			strNewEndBar.TrimRight();
			strNewEndBar.TrimLeft();

			if( strNewEndBar.IsEmpty() )
			{
				m_spinEndBar.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
			}
			else
			{
				long lNewEndBar = _ttoi( strNewEndBar );
				lNewEndBar--;
				if( lNewEndBar < 0 )
				{
					lNewEndBar = 0;
					m_spinEndBar.SetPos( 1 );
				}
				else if( lNewEndBar > (long) m_pPPGSegment->dwMeasureLength )
				{
					lNewEndBar = m_pPPGSegment->dwMeasureLength;
					m_spinEndBar.SetPos( lNewEndBar + 1 );
				}
				if( (DWORD)lNewEndBar != m_pPPGSegment->dwLoopEndMeasure )
				{
					m_pPPGSegment->dwLoopEndMeasure = (DWORD)lNewEndBar;
					ResetEndRanges();
					ValidateEndTime();
					UpdateSegment();
				}
			}
		}
	}
}

void CLoopPPG::OnKillfocusEditLoopEndBeat() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strNewEndBeat;

	m_editEndBeat.GetWindowText( strNewEndBeat );

	// Strip leading and trailing spaces
	strNewEndBeat.TrimRight();
	strNewEndBeat.TrimLeft();

	if( strNewEndBeat.IsEmpty() )
	{
		m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
	}
	else
	{
		int iNewEndBeat = _ttoi( strNewEndBeat );
		if( iNewEndBeat > m_wMaxEndBeat )
		{
			iNewEndBeat = m_wMaxEndBeat;
			m_spinEndBeat.SetPos( iNewEndBeat );
		}
		else if( iNewEndBeat < 1 )
		{
			iNewEndBeat = 1;
			m_spinEndBeat.SetPos( iNewEndBeat );
		}
		iNewEndBeat--;
		if( iNewEndBeat != m_pPPGSegment->bLoopEndBeat )
		{
			m_pPPGSegment->bLoopEndBeat = (BYTE)iNewEndBeat;
			ValidateEndTime();
			UpdateSegment();
		}
	}
}

void CLoopPPG::OnKillfocusEditLoopEndGrid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strNewEndGrid;

	m_editEndGrid.GetWindowText( strNewEndGrid );

	// Strip leading and trailing spaces
	strNewEndGrid.TrimRight();
	strNewEndGrid.TrimLeft();

	if( strNewEndGrid.IsEmpty() )
	{
		m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
	}
	else
	{
		int iNewEndGrid = _ttoi( strNewEndGrid );
		if( iNewEndGrid > m_wMaxEndGrid )
		{
			iNewEndGrid = m_wMaxEndGrid;
			m_spinEndGrid.SetPos( iNewEndGrid );
		}
		else if( iNewEndGrid < 1 )
		{
			iNewEndGrid = 1;
			m_spinEndGrid.SetPos( iNewEndGrid );
		}
		iNewEndGrid--;
		if( iNewEndGrid != m_pPPGSegment->wLoopEndGrid )
		{
			m_pPPGSegment->wLoopEndGrid = (WORD)iNewEndGrid;
			ValidateEndTime();
			UpdateSegment();
		}
	}
}

void CLoopPPG::OnKillfocusEditLoopEndTick() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strNewEndTick;

	m_editEndTick.GetWindowText( strNewEndTick );

	// Strip leading and trailing spaces
	strNewEndTick.TrimRight();
	strNewEndTick.TrimLeft();

	if( strNewEndTick.IsEmpty() )
	{
		m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );
	}
	else
	{
		int iNewEndTick = _ttoi( strNewEndTick );
		if( iNewEndTick > (int)m_dwMaxEndTick )
		{
			iNewEndTick = m_dwMaxEndTick;
			m_spinEndTick.SetPos( iNewEndTick );
		}
		else if( iNewEndTick < 0 )
		{
			iNewEndTick = 0;
			m_spinEndTick.SetPos( iNewEndTick );
		}
		if( iNewEndTick != m_pPPGSegment->wLoopEndTick )
		{
			m_pPPGSegment->wLoopEndTick = (WORD) iNewEndTick;
			ValidateEndTime();
			UpdateSegment();
		}
	}
}

void CLoopPPG::OnKillfocusEditLoopRepeat() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		CString strNewLoopRepeat;

		m_editLoopRepeat.GetWindowText( strNewLoopRepeat );

		// Strip leading and trailing spaces
		strNewLoopRepeat.TrimRight();
		strNewLoopRepeat.TrimLeft();

		if( strNewLoopRepeat.IsEmpty() )
		{
			m_spinRepeat.SetPos( m_pPPGSegment->dwLoopRepeats );
		}
		else
		{
			DWORD dwNewLoopRepeat = _ttoi( strNewLoopRepeat );
			if( dwNewLoopRepeat < 0 )
			{
				dwNewLoopRepeat = 0;
				m_spinRepeat.SetPos( dwNewLoopRepeat );
			}
			else if( dwNewLoopRepeat > 32767 )
			{
				dwNewLoopRepeat = 32767;
				m_spinRepeat.SetPos( dwNewLoopRepeat );
			}
			if( dwNewLoopRepeat != m_pPPGSegment->dwLoopRepeats )
			{
				if( !m_pPPGSegment->dwLoopRepeats && dwNewLoopRepeat )
				{
					DMUS_TIMESIGNATURE TimeSig;
					if( GetTimeSig( TimeSig, m_pPPGSegment->dwMeasureLength - 1 ) )
					{
						// Set the End time to be m_pPPGSegment->mtSegmentLength
						TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, m_pPPGSegment->mtSegmentLength,
						m_pPPGSegment->dwLoopEndMeasure, m_pPPGSegment->bLoopEndBeat, m_pPPGSegment->wLoopEndGrid, m_pPPGSegment->wLoopEndTick );

						m_spinEndBar.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
						m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
						m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
						m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );

						// Reset Start to 0
						m_pPPGSegment->dwLoopStartMeasure = 0;
						m_pPPGSegment->bLoopStartBeat = 0;
						m_pPPGSegment->wLoopStartGrid = 0;
						m_pPPGSegment->wLoopStartTick = 0;
						m_spinStartTick.SetPos( 0 );
						m_spinStartBar.SetPos( 1 );
						m_spinStartBeat.SetPos( 1 );
						m_spinStartGrid.SetPos( 1 );

						ResetEndRanges();
						ResetStartRanges();
					}
				}
				m_pPPGSegment->dwLoopRepeats = dwNewLoopRepeat;
				m_dwLastLoopRepeatCount = dwNewLoopRepeat;
				if( m_pPPGSegment->dwLoopRepeats == 0 )
				{
					ResetLoopControls();
				}
				UpdateSegment();
				EnableControls( TRUE );
			}
		}
	}
}

void CLoopPPG::OnKillfocusEditLoopStartBar() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		if( m_pPPGSegment->pITimelineCtl )
		{
			CString strNewStartBar;

			m_editStartBar.GetWindowText( strNewStartBar );

			// Strip leading and trailing spaces
			strNewStartBar.TrimRight();
			strNewStartBar.TrimLeft();

			if( strNewStartBar.IsEmpty() )
			{
				m_spinStartBar.SetPos( m_pPPGSegment->dwLoopStartMeasure + 1 );
			}
			else
			{
				DWORD dwNewStartBar = _ttoi( strNewStartBar );
				if( dwNewStartBar < 1 )
				{
					dwNewStartBar = 1;
					m_spinStartBar.SetPos( dwNewStartBar );
				}
				else if( dwNewStartBar > m_pPPGSegment->dwMeasureLength )
				{
					dwNewStartBar = m_pPPGSegment->dwMeasureLength;
					m_spinStartBar.SetPos( dwNewStartBar );
				}
				dwNewStartBar--;
				if( dwNewStartBar != m_pPPGSegment->dwLoopStartMeasure )
				{
					m_pPPGSegment->dwLoopStartMeasure = dwNewStartBar;
					ResetStartRanges();
					ValidateStartTime();
					UpdateSegment();
				}
			}
		}
	}
}

void CLoopPPG::OnKillfocusEditLoopStartBeat() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strNewStartBeat;

	m_editStartBeat.GetWindowText( strNewStartBeat );

	// Strip leading and trailing spaces
	strNewStartBeat.TrimRight();
	strNewStartBeat.TrimLeft();

	if( strNewStartBeat.IsEmpty() )
	{
		m_spinStartBeat.SetPos( m_pPPGSegment->bLoopStartBeat + 1 );
	}
	else
	{
		int iNewStartBeat = _ttoi( strNewStartBeat );
		if( iNewStartBeat > m_wMaxStartBeat )
		{
			iNewStartBeat = m_wMaxStartBeat;
			m_spinStartBeat.SetPos( iNewStartBeat );
		}
		else if( iNewStartBeat < 1 )
		{
			iNewStartBeat = 1;
			m_spinStartBeat.SetPos( iNewStartBeat );
		}
		iNewStartBeat--;
		if( iNewStartBeat != m_pPPGSegment->bLoopStartBeat )
		{
			m_pPPGSegment->bLoopStartBeat = (BYTE)iNewStartBeat;
			ValidateStartTime();
			UpdateSegment();
		}
	}
}

void CLoopPPG::OnKillfocusEditLoopStartGrid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strNewStartGrid;

	m_editStartGrid.GetWindowText( strNewStartGrid );

	// Strip leading and trailing spaces
	strNewStartGrid.TrimRight();
	strNewStartGrid.TrimLeft();

	if( strNewStartGrid.IsEmpty() )
	{
		m_spinStartGrid.SetPos( m_pPPGSegment->wLoopStartGrid + 1 );
	}
	else
	{
		int iNewStartGrid = _ttoi( strNewStartGrid );
		if( iNewStartGrid > m_wMaxStartGrid )
		{
			iNewStartGrid = m_wMaxStartGrid;
			m_spinStartGrid.SetPos( iNewStartGrid );
		}
		else if( iNewStartGrid < 1 )
		{
			iNewStartGrid = 1;
			m_spinStartGrid.SetPos( iNewStartGrid );
		}
		iNewStartGrid--;
		if( iNewStartGrid != m_pPPGSegment->wLoopStartGrid )
		{
			m_pPPGSegment->wLoopStartGrid = (WORD)iNewStartGrid;
			ValidateStartTime();
			UpdateSegment();
		}
	}
}

void CLoopPPG::OnKillfocusEditLoopStartTick() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strNewStartTick;

	m_editStartTick.GetWindowText( strNewStartTick );

	// Strip leading and trailing spaces
	strNewStartTick.TrimRight();
	strNewStartTick.TrimLeft();

	if( strNewStartTick.IsEmpty() )
	{
		m_spinStartTick.SetPos( m_pPPGSegment->wLoopStartTick );
	}
	else
	{
		int iNewStartTick = _ttoi( strNewStartTick );
		if( iNewStartTick > (int)m_dwMaxStartTick )
		{
			iNewStartTick = m_dwMaxStartTick;
			m_spinStartTick.SetPos( iNewStartTick );
		}
		else if( iNewStartTick < 0 )
		{
			iNewStartTick = 0;
			m_spinStartTick.SetPos( iNewStartTick );
		}
		if( iNewStartTick != m_pPPGSegment->wLoopStartTick )
		{
			m_pPPGSegment->wLoopStartTick = (WORD) iNewStartTick;
			ValidateStartTime();
			UpdateSegment();
		}
	}
}

void CLoopPPG::OnDeltaposSpinLoopEndBar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		long lNewValue = m_spinEndBar.GetPos() + pNMUpDown->iDelta;
		lNewValue--;
		if( lNewValue < 0 )
		{
			lNewValue = 0;
		}
		else if( lNewValue > (long) m_pPPGSegment->dwMeasureLength )
		{
			lNewValue = m_pPPGSegment->dwMeasureLength;
		}

		if( (DWORD)lNewValue != m_pPPGSegment->dwLoopEndMeasure )
		{
			m_pPPGSegment->dwLoopEndMeasure = (DWORD)lNewValue;
			ResetEndRanges();
			ValidateEndTime();
			UpdateSegment();
		}
	}

	*pResult = 0;
}

void CLoopPPG::OnDeltaposSpinLoopEndBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pResult = 1;

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_spinEndBeat.GetPos() + pNMUpDown->iDelta - 1;

		MUSIC_TIME mtTime;
		BarBeatGridTickToTime( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup,
			m_pPPGSegment->dwLoopEndMeasure, nNewValue, m_pPPGSegment->wLoopEndGrid, m_pPPGSegment->wLoopEndTick,
			mtTime );
		mtTime = max( 0, min( m_pPPGSegment->mtSegmentLength, mtTime ));
		DWORD dwLoopEndMeasure;
		BYTE bLoopEndBeat;
		WORD wLoopEndGrid, wLoopEndTick;
		TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, mtTime,
			dwLoopEndMeasure, bLoopEndBeat, wLoopEndGrid, wLoopEndTick );

		if( (dwLoopEndMeasure != m_pPPGSegment->dwLoopEndMeasure)
		||	(bLoopEndBeat != m_pPPGSegment->bLoopEndBeat)
		||	(wLoopEndGrid != m_pPPGSegment->wLoopEndGrid)
		||	(wLoopEndTick != m_pPPGSegment->wLoopEndTick) )
		{
			m_pPPGSegment->dwLoopEndMeasure = dwLoopEndMeasure;
			m_pPPGSegment->bLoopEndBeat = bLoopEndBeat;
			m_pPPGSegment->wLoopEndGrid = wLoopEndGrid;
			m_pPPGSegment->wLoopEndTick = wLoopEndTick;
			ValidateEndTime();
			UpdateSegment();
			SetData(m_pPPGSegment);
		}
	}
}

void CLoopPPG::OnDeltaposSpinLoopEndGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pResult = 1;

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_spinEndGrid.GetPos() + pNMUpDown->iDelta - 1;

		MUSIC_TIME mtTime;
		BarBeatGridTickToTime( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup,
			m_pPPGSegment->dwLoopEndMeasure, m_pPPGSegment->bLoopEndBeat, nNewValue, m_pPPGSegment->wLoopEndTick,
			mtTime );
		mtTime = max( 0, min( m_pPPGSegment->mtSegmentLength, mtTime ));
		DWORD dwLoopEndMeasure;
		BYTE bLoopEndBeat;
		WORD wLoopEndGrid, wLoopEndTick;
		TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, mtTime,
			dwLoopEndMeasure, bLoopEndBeat, wLoopEndGrid, wLoopEndTick );

		if( (dwLoopEndMeasure != m_pPPGSegment->dwLoopEndMeasure)
		||	(bLoopEndBeat != m_pPPGSegment->bLoopEndBeat)
		||	(wLoopEndGrid != m_pPPGSegment->wLoopEndGrid)
		||	(wLoopEndTick != m_pPPGSegment->wLoopEndTick) )
		{
			m_pPPGSegment->dwLoopEndMeasure = dwLoopEndMeasure;
			m_pPPGSegment->bLoopEndBeat = bLoopEndBeat;
			m_pPPGSegment->wLoopEndGrid = wLoopEndGrid;
			m_pPPGSegment->wLoopEndTick = wLoopEndTick;
			ValidateEndTime();
			UpdateSegment();
			SetData(m_pPPGSegment);
		}
	}
}

void CLoopPPG::OnDeltaposSpinLoopEndTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pResult = 1;

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_spinEndTick.GetPos() + pNMUpDown->iDelta;

		MUSIC_TIME mtTime;
		BarBeatGridTickToTime( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup,
			m_pPPGSegment->dwLoopEndMeasure, m_pPPGSegment->bLoopEndBeat, m_pPPGSegment->wLoopEndGrid, nNewValue,
			mtTime );
		mtTime = max( 0, min( m_pPPGSegment->mtSegmentLength, mtTime ));
		DWORD dwLoopEndMeasure;
		BYTE bLoopEndBeat;
		WORD wLoopEndGrid, wLoopEndTick;
		TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, mtTime,
			dwLoopEndMeasure, bLoopEndBeat, wLoopEndGrid, wLoopEndTick );

		if( (dwLoopEndMeasure != m_pPPGSegment->dwLoopEndMeasure)
		||	(bLoopEndBeat != m_pPPGSegment->bLoopEndBeat)
		||	(wLoopEndGrid != m_pPPGSegment->wLoopEndGrid)
		||	(wLoopEndTick != m_pPPGSegment->wLoopEndTick) )
		{
			m_pPPGSegment->dwLoopEndMeasure = dwLoopEndMeasure;
			m_pPPGSegment->bLoopEndBeat = bLoopEndBeat;
			m_pPPGSegment->wLoopEndGrid = wLoopEndGrid;
			m_pPPGSegment->wLoopEndTick = wLoopEndTick;
			ValidateEndTime();
			UpdateSegment();
			SetData(m_pPPGSegment);
		}
	}
}

void CLoopPPG::OnDeltaposSpinLoopRepeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_spinRepeat.GetPos() + pNMUpDown->iDelta;
		if( nNewValue < 0 )
		{
			nNewValue = 0;
		}
		else if( nNewValue > 32767 )
		{
			nNewValue = 32767;
		}

		if( (DWORD)nNewValue != m_pPPGSegment->dwLoopRepeats )
		{
			if( !m_pPPGSegment->dwLoopRepeats && nNewValue )
			{
				DMUS_TIMESIGNATURE TimeSig;
				if( GetTimeSig( TimeSig, m_pPPGSegment->dwMeasureLength - 1 ) )
				{
					// Set the End time to be m_pPPGSegment->mtSegmentLength
					TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, m_pPPGSegment->mtSegmentLength,
					m_pPPGSegment->dwLoopEndMeasure, m_pPPGSegment->bLoopEndBeat, m_pPPGSegment->wLoopEndGrid, m_pPPGSegment->wLoopEndTick );

					m_spinEndBar.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
					m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
					m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
					m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );

					// Reset Start to 0
					m_pPPGSegment->dwLoopStartMeasure = 0;
					m_pPPGSegment->bLoopStartBeat = 0;
					m_pPPGSegment->wLoopStartGrid = 0;
					m_pPPGSegment->wLoopStartTick = 0;
					m_spinStartTick.SetPos( 0 );
					m_spinStartBar.SetPos( 1 );
					m_spinStartBeat.SetPos( 1 );
					m_spinStartGrid.SetPos( 1 );

					ResetEndRanges();
					ResetStartRanges();
				}
			}
			m_pPPGSegment->dwLoopRepeats = nNewValue;
			m_dwLastLoopRepeatCount = nNewValue;
			if( m_pPPGSegment->dwLoopRepeats == 0 )
			{
				ResetLoopControls();
			}
			UpdateSegment();
			EnableControls( TRUE );
		}
	}

	*pResult = 0;
}

void CLoopPPG::OnDeltaposSpinLoopStartBar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		long lNewValue = m_spinStartBar.GetPos() + pNMUpDown->iDelta;
		if( lNewValue < 1 )
		{
			lNewValue = 1;
		}
		else if( lNewValue > (long) m_pPPGSegment->dwMeasureLength )
		{
			lNewValue = m_pPPGSegment->dwMeasureLength;
		}
		lNewValue--;

		if( (DWORD)lNewValue != m_pPPGSegment->dwLoopStartMeasure )
		{
			m_pPPGSegment->dwLoopStartMeasure = (DWORD)lNewValue;
			ResetStartRanges();
			ValidateStartTime();
			UpdateSegment();
		}
	}

	*pResult = 0;
}

void CLoopPPG::OnDeltaposSpinLoopStartBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pResult = 1;

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_spinStartBeat.GetPos() + pNMUpDown->iDelta - 1;

		MUSIC_TIME mtTime;
		BarBeatGridTickToTime( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup,
			m_pPPGSegment->dwLoopStartMeasure, nNewValue, m_pPPGSegment->wLoopStartGrid, m_pPPGSegment->wLoopStartTick,
			mtTime );
		mtTime = max( 0, min( m_pPPGSegment->mtSegmentLength, mtTime ));
		DWORD dwLoopStartMeasure;
		BYTE bLoopStartBeat;
		WORD wLoopStartGrid, wLoopStartTick;
		TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, mtTime,
			dwLoopStartMeasure, bLoopStartBeat, wLoopStartGrid, wLoopStartTick );

		if( (dwLoopStartMeasure != m_pPPGSegment->dwLoopStartMeasure)
		||	(bLoopStartBeat != m_pPPGSegment->bLoopStartBeat)
		||	(wLoopStartGrid != m_pPPGSegment->wLoopStartGrid)
		||	(wLoopStartTick != m_pPPGSegment->wLoopStartTick) )
		{
			m_pPPGSegment->dwLoopStartMeasure = dwLoopStartMeasure;
			m_pPPGSegment->bLoopStartBeat = bLoopStartBeat;
			m_pPPGSegment->wLoopStartGrid = wLoopStartGrid;
			m_pPPGSegment->wLoopStartTick = wLoopStartTick;
			ValidateStartTime();
			UpdateSegment();
			SetData(m_pPPGSegment);
		}
	}
}

void CLoopPPG::OnDeltaposSpinLoopStartGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pResult = 1;

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_spinStartGrid.GetPos() + pNMUpDown->iDelta - 1;

		MUSIC_TIME mtTime;
		BarBeatGridTickToTime( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup,
			m_pPPGSegment->dwLoopStartMeasure, m_pPPGSegment->bLoopStartBeat, nNewValue, m_pPPGSegment->wLoopStartTick,
			mtTime );
		mtTime = max( 0, min( m_pPPGSegment->mtSegmentLength, mtTime ));
		DWORD dwLoopStartMeasure;
		BYTE bLoopStartBeat;
		WORD wLoopStartGrid, wLoopStartTick;
		TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, mtTime,
			dwLoopStartMeasure, bLoopStartBeat, wLoopStartGrid, wLoopStartTick );

		if( (dwLoopStartMeasure != m_pPPGSegment->dwLoopStartMeasure)
		||	(bLoopStartBeat != m_pPPGSegment->bLoopStartBeat)
		||	(wLoopStartGrid != m_pPPGSegment->wLoopStartGrid)
		||	(wLoopStartTick != m_pPPGSegment->wLoopStartTick) )
		{
			m_pPPGSegment->dwLoopStartMeasure = dwLoopStartMeasure;
			m_pPPGSegment->bLoopStartBeat = bLoopStartBeat;
			m_pPPGSegment->wLoopStartGrid = wLoopStartGrid;
			m_pPPGSegment->wLoopStartTick = wLoopStartTick;
			ValidateStartTime();
			UpdateSegment();
			SetData(m_pPPGSegment);
		}
	}
}

void CLoopPPG::OnDeltaposSpinLoopStartTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pResult = 1;

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_spinStartTick.GetPos() + pNMUpDown->iDelta;

		MUSIC_TIME mtTime;
		BarBeatGridTickToTime( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup,
			m_pPPGSegment->dwLoopStartMeasure, m_pPPGSegment->bLoopStartBeat, m_pPPGSegment->wLoopStartGrid, nNewValue,
			mtTime );
		mtTime = max( 0, min( m_pPPGSegment->mtSegmentLength, mtTime ));
		DWORD dwLoopStartMeasure;
		BYTE bLoopStartBeat;
		WORD wLoopStartGrid, wLoopStartTick;
		TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, mtTime,
			dwLoopStartMeasure, bLoopStartBeat, wLoopStartGrid, wLoopStartTick );

		if( (dwLoopStartMeasure != m_pPPGSegment->dwLoopStartMeasure)
		||	(bLoopStartBeat != m_pPPGSegment->bLoopStartBeat)
		||	(wLoopStartGrid != m_pPPGSegment->wLoopStartGrid)
		||	(wLoopStartTick != m_pPPGSegment->wLoopStartTick) )
		{
			m_pPPGSegment->dwLoopStartMeasure = dwLoopStartMeasure;
			m_pPPGSegment->bLoopStartBeat = bLoopStartBeat;
			m_pPPGSegment->wLoopStartGrid = wLoopStartGrid;
			m_pPPGSegment->wLoopStartTick = wLoopStartTick;
			ValidateStartTime();
			UpdateSegment();
			SetData(m_pPPGSegment);
		}
	}
}

void CLoopPPG::ResetEndRanges()
{
	DMUS_TIMESIGNATURE TimeSig;
	if( GetTimeSig( TimeSig, m_pPPGSegment->dwLoopEndMeasure ) )
	{
		long lTemp;

		// Limit end beat
		lTemp = TimeSig.bBeatsPerMeasure;
		if( m_pPPGSegment->bLoopEndBeat > lTemp - 1 )
		{
			m_spinEndBeat.SetPos( lTemp );
			m_pPPGSegment->bLoopEndBeat = BYTE(lTemp - 1);
		}
		m_wMaxEndBeat = (WORD)lTemp;

		// Limit end grid
		lTemp = TimeSig.wGridsPerBeat;
		if( m_pPPGSegment->wLoopEndGrid > lTemp - 1 )
		{
			m_spinEndGrid.SetPos( lTemp );
			m_pPPGSegment->wLoopEndGrid = WORD(lTemp - 1);
		}
		m_wMaxEndGrid = (WORD)lTemp;

		// Limit end tick
		lTemp = (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat) - 1;
		if( m_pPPGSegment->wLoopEndTick > lTemp )
		{
			m_spinEndTick.SetPos( lTemp );
			m_pPPGSegment->wLoopEndTick = (WORD)lTemp;
		}
		m_dwMaxEndTick = lTemp;
	}

	// Limit end bar
	CString strText;
	m_spinEndBar.SetRange( 1, m_pPPGSegment->dwMeasureLength + 1 );
	strText.Format("%d",m_pPPGSegment->dwMeasureLength);
	m_editEndBar.LimitText( strText.GetLength() );

	// Limit end beat
	EnableItem( IDC_SPIN_LOOP_END_BEAT, TRUE );
	m_spinEndBeat.SetRange( 1, m_wMaxEndBeat );
	strText.Format("%d",m_wMaxEndBeat);
	m_editEndBeat.LimitText( strText.GetLength() );
	m_spinEndBeat.Invalidate();

	// Limit end grid
	EnableItem( IDC_SPIN_LOOP_END_GRID, TRUE );
	m_spinEndGrid.SetRange( 1, m_wMaxEndGrid );
	strText.Format("%d",m_wMaxEndGrid);
	m_editEndGrid.LimitText( strText.GetLength() );
	m_spinEndGrid.Invalidate();

	// Limit end tick
	EnableItem( IDC_SPIN_LOOP_END_TICK, TRUE );
	m_spinEndTick.SetRange( 0, m_dwMaxEndTick );
	strText.Format("%d",m_dwMaxEndTick);
	m_editEndTick.LimitText( strText.GetLength() );
	m_spinEndTick.Invalidate();
}

void CLoopPPG::ResetStartRanges()
{
	DMUS_TIMESIGNATURE TimeSig;
	if( GetTimeSig( TimeSig, m_pPPGSegment->dwLoopStartMeasure ) )
	{
		long lTemp;
		CString strText;

		// Limit start bar
		lTemp = m_pPPGSegment->dwMeasureLength;
		m_spinStartBar.SetRange( 1, lTemp );
		strText.Format("%d",lTemp);
		m_editStartBar.LimitText( strText.GetLength() );

		lTemp = TimeSig.bBeatsPerMeasure;
		if( m_pPPGSegment->bLoopStartBeat > lTemp - 1 )
		{
			m_spinStartBeat.SetPos( lTemp );
			m_pPPGSegment->bLoopStartBeat = BYTE(lTemp - 1);
		}
		m_wMaxStartBeat = (WORD)lTemp;
		m_spinStartBeat.SetRange( 1, lTemp );
		EnableItem( IDC_SPIN_LOOP_START_BEAT, m_wMaxStartBeat > 1 );
		strText.Format("%d",lTemp);
		m_editStartBeat.LimitText( strText.GetLength() );

		lTemp = TimeSig.wGridsPerBeat;
		if( m_pPPGSegment->wLoopStartGrid > lTemp - 1 )
		{
			m_spinStartGrid.SetPos( lTemp );
			m_pPPGSegment->wLoopStartGrid = WORD(lTemp - 1);
		}
		m_wMaxStartGrid = (WORD) lTemp;
		m_spinStartGrid.SetRange( 1, lTemp );
		EnableItem( IDC_SPIN_LOOP_START_GRID, m_wMaxStartGrid > 1 );
		strText.Format("%d",lTemp);
		m_editStartGrid.LimitText( strText.GetLength() );

		lTemp = (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat) - 1;
		if( m_pPPGSegment->wLoopStartTick > lTemp )
		{
			m_spinStartTick.SetPos( lTemp );
			m_pPPGSegment->wLoopStartTick = (WORD)lTemp;
		}
		m_dwMaxStartTick = lTemp;
		m_spinStartTick.SetRange( 0, lTemp );
		EnableItem( IDC_SPIN_LOOP_START_TICK, m_dwMaxStartTick > 0 );
		strText.Format("%d",lTemp);
		m_editStartTick.LimitText( strText.GetLength() );
	}
}

BOOL CLoopPPG::GetTimeSig( DMUS_TIMESIGNATURE &TimeSig, DWORD dwMeasure )
{
	if( m_pPPGSegment )
	{
		if( m_pPPGSegment->pITimelineCtl )
		{
			MUSIC_TIME mt;
			if( SUCCEEDED( m_pPPGSegment->pITimelineCtl->MeasureBeatToClocks( m_pPPGSegment->dwTrackGroup, 0, dwMeasure, 0, &mt ) ) )
			{
				if( SUCCEEDED( m_pPPGSegment->pITimelineCtl->GetParam( GUID_TimeSignature, m_pPPGSegment->dwTrackGroup, 0, mt, NULL, &TimeSig ) ) )
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void CLoopPPG::UpdateSegment()
{
	if( m_pPageManager )
	{
		if( m_pPageManager->m_pIPropPageObject )
		{
			if( m_pPPGSegment )
			{
				m_pPageManager->m_pIPropPageObject->SetData( m_pPPGSegment );
			}
		}
	}
}

BOOL CLoopPPG::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment == NULL )
	{
		return CPropertyPage::PreTranslateMessage( pMsg );
	}

	switch( pMsg->message )
	{
		case WM_KEYDOWN:
			if( pMsg->lParam & 0x40000000 )
			{
				break;
			}

			switch( pMsg->wParam )
			{
				case VK_ESCAPE:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_EDIT_LOOP_REPEAT: 
								m_spinRepeat.SetPos( m_pPPGSegment->dwLoopRepeats );
								break;

							case IDC_EDIT_LOOP_END_BAR:
								m_spinEndBar.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
								break;

							case IDC_EDIT_LOOP_END_BEAT:
								m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
								break;

							case IDC_EDIT_LOOP_END_GRID:
								m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
								break;

							case IDC_EDIT_LOOP_END_TICK:
								m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );
								break;

							case IDC_EDIT_LOOP_START_BAR:
								m_spinStartBar.SetPos( m_pPPGSegment->dwLoopStartMeasure + 1 );
								break;

							case IDC_EDIT_LOOP_START_BEAT:
								m_spinStartBeat.SetPos( m_pPPGSegment->bLoopStartBeat + 1 );
								break;

							case IDC_EDIT_LOOP_START_GRID:
								m_spinStartGrid.SetPos( m_pPPGSegment->wLoopStartGrid + 1 );
								break;

							case IDC_EDIT_LOOP_START_TICK:
								m_spinStartTick.SetPos( m_pPPGSegment->wLoopStartTick );
								break;
						}
					}
					return TRUE;
				}

				case VK_RETURN:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						if( pWnd->GetDlgCtrlID() == IDC_EDIT_LOOP_REPEAT )
						{
							OnKillfocusEditLoopRepeat();
						}

						CWnd* pWndNext = GetNextDlgTabItem( pWnd );
						if( pWndNext )
						{
							pWndNext->SetFocus();
						}
					}
					return TRUE;
				}
			}
			break;
	}
	
	return CPropertyPage::PreTranslateMessage( pMsg );
}

// return TRUE if changes something, FALSE if nothing changes
BOOL CLoopPPG::ValidateStartTime()
{
	if( !m_pPPGSegment )
	{
		return FALSE;
	}

	// If the start measure is before the end measure, no need to check further
	if( m_pPPGSegment->dwLoopStartMeasure < m_pPPGSegment->dwLoopEndMeasure )
	{
		return FALSE;
	}

	// Restrict the Start time to be before m_pPPGSegment->mtSegmentLength
	DWORD dwMaxLoopStartMeasure;
	BYTE bMaxLoopStartBeat;
	WORD wMaxLoopStartGrid, wMaxLoopStartTick;

	TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, m_pPPGSegment->mtSegmentLength - 1,
		dwMaxLoopStartMeasure, bMaxLoopStartBeat, wMaxLoopStartGrid, wMaxLoopStartTick );

	if( m_pPPGSegment->dwLoopStartMeasure >= dwMaxLoopStartMeasure )
	{
		if( m_pPPGSegment->dwLoopStartMeasure != dwMaxLoopStartMeasure )
		{
			m_pPPGSegment->dwLoopStartMeasure = dwMaxLoopStartMeasure;
			::PostMessage(m_spinStartBar.m_hWnd, UDM_SETPOS, 0, MAKELPARAM(m_pPPGSegment->dwLoopStartMeasure + 1, 0));
		}

		if( m_pPPGSegment->bLoopStartBeat >= bMaxLoopStartBeat )
		{
			if( m_pPPGSegment->bLoopStartBeat != bMaxLoopStartBeat )
			{
				m_pPPGSegment->bLoopStartBeat = bMaxLoopStartBeat;
				::PostMessage(m_spinStartBeat.m_hWnd, UDM_SETPOS, 0, MAKELPARAM(m_pPPGSegment->bLoopStartBeat + 1, 0));
			}

			if( m_pPPGSegment->wLoopStartGrid >= wMaxLoopStartGrid )
			{
				if( m_pPPGSegment->wLoopStartGrid != wMaxLoopStartGrid )
				{
					m_pPPGSegment->wLoopStartGrid = wMaxLoopStartGrid;
					::PostMessage(m_spinStartGrid.m_hWnd, UDM_SETPOS, 0, MAKELPARAM(m_pPPGSegment->wLoopStartGrid + 1, 0));
				}

				if( m_pPPGSegment->wLoopStartTick > wMaxLoopStartTick )
				{
					m_pPPGSegment->wLoopStartTick = wMaxLoopStartTick;
					::PostMessage(m_spinStartTick.m_hWnd, UDM_SETPOS, 0, MAKELPARAM(m_pPPGSegment->wLoopStartTick, 0));
				}
			}
		}
	}

	// If the start measure is after the end measure, set the end loop point
	// the next available tick
	if( m_pPPGSegment->dwLoopStartMeasure > m_pPPGSegment->dwLoopEndMeasure )
	{
		SetEndLoopToNextAvailableTick();
		return TRUE;
	}

	// Otherwise the start measure == the end measure
	ASSERT( m_pPPGSegment->dwLoopStartMeasure == m_pPPGSegment->dwLoopEndMeasure );

	// If the end beat is after the start beat, no need to check further
	if( m_pPPGSegment->bLoopEndBeat > m_pPPGSegment->bLoopStartBeat )
	{
		return FALSE;
	}

	// If end beat is less than start beat, set end loop to next available tick
	if( m_pPPGSegment->bLoopEndBeat < m_pPPGSegment->bLoopStartBeat )
	{
		SetEndLoopToNextAvailableTick();
		return TRUE;
	}

	// Otherwise the start beat == the end beat
	ASSERT( m_pPPGSegment->bLoopStartBeat == m_pPPGSegment->bLoopEndBeat );
	
	// If the end grid is after the start grid, no need to check further
	if( m_pPPGSegment->wLoopEndGrid > m_pPPGSegment->wLoopStartGrid )
	{
		return FALSE;
	}

	// If end grid is less than start grid, set end loop to next available tick
	if( m_pPPGSegment->wLoopEndGrid < m_pPPGSegment->wLoopStartGrid )
	{
		SetEndLoopToNextAvailableTick();
		return TRUE;
	}

	// Otherwise the start grid == the end grid
	ASSERT( m_pPPGSegment->wLoopStartGrid == m_pPPGSegment->wLoopEndGrid );

	// If the end tick is after the start tick, no need to check further
	if( m_pPPGSegment->wLoopEndTick > m_pPPGSegment->wLoopStartTick )
	{
		return FALSE;
	}

	// Otherwise, end tick is less or equal to start tick.
	// Set end loop to next available tick
	ASSERT( m_pPPGSegment->wLoopEndTick <= m_pPPGSegment->wLoopStartTick );
	SetEndLoopToNextAvailableTick();
	return TRUE;
}

void CLoopPPG::SetEndLoopToNextAvailableTick( void )
{
	if( m_pPPGSegment->wLoopStartTick < m_dwMaxStartTick )
	{
		m_pPPGSegment->wLoopEndTick = WORD(m_pPPGSegment->wLoopStartTick + 1);
		m_pPPGSegment->wLoopEndGrid = m_pPPGSegment->wLoopStartGrid;
		m_pPPGSegment->bLoopEndBeat = m_pPPGSegment->bLoopStartBeat;
		m_pPPGSegment->dwLoopEndMeasure = m_pPPGSegment->dwLoopStartMeasure;
	}
	else if( m_pPPGSegment->wLoopStartGrid < (m_wMaxStartGrid - 1) )
	{
		m_pPPGSegment->wLoopEndTick = 0;
		m_pPPGSegment->wLoopEndGrid = WORD(m_pPPGSegment->wLoopStartGrid + 1);
		m_pPPGSegment->bLoopEndBeat = m_pPPGSegment->bLoopStartBeat;
		m_pPPGSegment->dwLoopEndMeasure = m_pPPGSegment->dwLoopStartMeasure;
	}
	else if( m_pPPGSegment->bLoopStartBeat < (m_wMaxStartBeat - 1) )
	{
		m_pPPGSegment->wLoopEndTick = 0;
		m_pPPGSegment->wLoopEndGrid = 0;
		m_pPPGSegment->bLoopEndBeat = BYTE(m_pPPGSegment->bLoopStartBeat + 1);
		m_pPPGSegment->dwLoopEndMeasure = m_pPPGSegment->dwLoopStartMeasure;
	}
	else
	{
		ASSERT( m_pPPGSegment->dwLoopStartMeasure < m_pPPGSegment->dwMeasureLength );
		m_pPPGSegment->wLoopEndTick = 0;
		m_pPPGSegment->wLoopEndGrid = 0;
		m_pPPGSegment->bLoopEndBeat = 0;
		m_pPPGSegment->dwLoopEndMeasure = m_pPPGSegment->dwLoopStartMeasure + 1;
	}
	m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );
	m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
	m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
	m_spinEndBar.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
}

// return TRUE if changes something, FALSE if nothing changes
BOOL CLoopPPG::ValidateEndTime()
{
	if( !m_pPPGSegment )
	{
		return FALSE;
	}

	// Restrict the End time to be m_pPPGSegment->mtSegmentLength or earlier
	DWORD dwMaxLoopEndMeasure;
	BYTE bMaxLoopEndBeat;
	WORD wMaxLoopEndGrid, wMaxLoopEndTick;

	TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, m_pPPGSegment->mtSegmentLength,
		dwMaxLoopEndMeasure, bMaxLoopEndBeat, wMaxLoopEndGrid, wMaxLoopEndTick );

	if( m_pPPGSegment->dwLoopEndMeasure >= dwMaxLoopEndMeasure )
	{
		if( m_pPPGSegment->dwLoopEndMeasure != dwMaxLoopEndMeasure )
		{
			m_pPPGSegment->dwLoopEndMeasure = dwMaxLoopEndMeasure;
			::PostMessage(m_spinEndBar.m_hWnd, UDM_SETPOS, 0, MAKELPARAM(m_pPPGSegment->dwLoopEndMeasure + 1, 0));
		}

		if( m_pPPGSegment->bLoopEndBeat >= bMaxLoopEndBeat )
		{
			if( m_pPPGSegment->bLoopEndBeat != bMaxLoopEndBeat )
			{
				m_pPPGSegment->bLoopEndBeat = bMaxLoopEndBeat;
				::PostMessage(m_spinEndBeat.m_hWnd, UDM_SETPOS, 0, MAKELPARAM(m_pPPGSegment->bLoopEndBeat + 1, 0));
			}

			if( m_pPPGSegment->wLoopEndGrid >= wMaxLoopEndGrid )
			{
				if( m_pPPGSegment->wLoopEndGrid != wMaxLoopEndGrid )
				{
					m_pPPGSegment->wLoopEndGrid = wMaxLoopEndGrid;
					::PostMessage(m_spinEndGrid.m_hWnd, UDM_SETPOS, 0, MAKELPARAM(m_pPPGSegment->wLoopEndGrid + 1, 0));
				}

				if( m_pPPGSegment->wLoopEndTick > wMaxLoopEndTick )
				{
					m_pPPGSegment->wLoopEndTick = wMaxLoopEndTick;
					::PostMessage(m_spinEndTick.m_hWnd, UDM_SETPOS, 0, MAKELPARAM(m_pPPGSegment->wLoopEndTick, 0));
				}
			}
		}
	}

	// If the start measure is before the end measure, no need to check further
	if( m_pPPGSegment->dwLoopStartMeasure < m_pPPGSegment->dwLoopEndMeasure )
	{
		return FALSE;
	}

	// If the start measure is after the end measure, set the start loop point
	// the next available tick
	if( m_pPPGSegment->dwLoopStartMeasure > m_pPPGSegment->dwLoopEndMeasure )
	{
		SetStartLoopToNextAvailableTick();
		return TRUE;
	}

	// Otherwise the start measure == the end measure
	ASSERT( m_pPPGSegment->dwLoopStartMeasure == m_pPPGSegment->dwLoopEndMeasure );

	// If the end beat is after the start beat, no need to check further
	if( m_pPPGSegment->bLoopEndBeat > m_pPPGSegment->bLoopStartBeat )
	{
		return FALSE;
	}

	// If end beat is less than start beat, set start loop to next available tick
	if( m_pPPGSegment->bLoopEndBeat < m_pPPGSegment->bLoopStartBeat )
	{
		SetStartLoopToNextAvailableTick();
		return TRUE;
	}

	// Otherwise the start beat == the end beat
	ASSERT( m_pPPGSegment->bLoopStartBeat == m_pPPGSegment->bLoopEndBeat );
	
	// If the end grid is after the start grid, no need to check further
	if( m_pPPGSegment->wLoopEndGrid > m_pPPGSegment->wLoopStartGrid )
	{
		return FALSE;
	}

	// If end grid is less than start grid, set start loop to next available tick
	if( m_pPPGSegment->wLoopEndGrid < m_pPPGSegment->wLoopStartGrid )
	{
		SetStartLoopToNextAvailableTick();
		return TRUE;
	}

	// Otherwise the start grid == the end grid
	ASSERT( m_pPPGSegment->wLoopStartGrid == m_pPPGSegment->wLoopEndGrid );

	// If the end tick is after the start tick, no need to check further
	if( m_pPPGSegment->wLoopEndTick > m_pPPGSegment->wLoopStartTick )
	{
		return FALSE;
	}

	// Otherwise, end tick is less or equal to start tick.
	// Set start loop to next available tick
	ASSERT( m_pPPGSegment->wLoopEndTick <= m_pPPGSegment->wLoopStartTick );
	SetStartLoopToNextAvailableTick();
	return TRUE;
}

void CLoopPPG::SetStartLoopToNextAvailableTick( void )
{
	if( m_pPPGSegment->wLoopEndTick > 0 )
	{
		m_pPPGSegment->wLoopStartTick = WORD(m_pPPGSegment->wLoopEndTick - 1);
		m_pPPGSegment->wLoopStartGrid = m_pPPGSegment->wLoopEndGrid;
		m_pPPGSegment->bLoopStartBeat = m_pPPGSegment->bLoopEndBeat;
		m_pPPGSegment->dwLoopStartMeasure = m_pPPGSegment->dwLoopEndMeasure;
	}
	else if( m_pPPGSegment->wLoopEndGrid > 0 )
	{
		m_pPPGSegment->wLoopStartTick = WORD(m_dwMaxEndTick);
		m_pPPGSegment->wLoopStartGrid = WORD(m_pPPGSegment->wLoopStartGrid - 1);
		m_pPPGSegment->bLoopStartBeat = m_pPPGSegment->bLoopEndBeat;
		m_pPPGSegment->dwLoopStartMeasure = m_pPPGSegment->dwLoopEndMeasure;
	}
	else if( m_pPPGSegment->bLoopEndBeat > 0 )
	{
		m_pPPGSegment->wLoopStartTick = WORD(m_dwMaxEndTick);
		m_pPPGSegment->wLoopStartGrid = m_wMaxEndGrid;
		m_pPPGSegment->bLoopStartBeat = BYTE(m_pPPGSegment->bLoopEndBeat - 1);
		m_pPPGSegment->dwLoopStartMeasure = m_pPPGSegment->dwLoopEndMeasure;
	}
	else
	{
		if( m_pPPGSegment->dwLoopEndMeasure == 0 )
		{
			m_pPPGSegment->wLoopEndTick = 1;
			m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );
			m_pPPGSegment->wLoopStartTick = 0;
			m_pPPGSegment->wLoopStartGrid = 0;
			m_pPPGSegment->bLoopStartBeat = 0;
			m_pPPGSegment->dwLoopStartMeasure = 0;
		}
		else
		{
			DMUS_TIMESIGNATURE TimeSig;
			if( GetTimeSig( TimeSig, m_pPPGSegment->dwLoopEndMeasure - 1 ) )
			{
				m_pPPGSegment->wLoopStartTick = (WORD)((long) ((long) NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / (long) TimeSig.wGridsPerBeat) - 1);
				m_pPPGSegment->wLoopStartGrid = WORD(TimeSig.wGridsPerBeat - 1);
				m_pPPGSegment->bLoopStartBeat = BYTE(TimeSig.bBeatsPerMeasure - 1);
				m_pPPGSegment->dwLoopStartMeasure = m_pPPGSegment->dwLoopEndMeasure - 1;
			}
			else
			{
				TRACE("CLoopPPG::SetStartLoopToNextAvailableTick unable to get TimeSig.\n");
				ASSERT( FALSE );
				m_pPPGSegment->wLoopStartTick = 0;
				m_pPPGSegment->wLoopStartGrid = 0;
				m_pPPGSegment->bLoopStartBeat = 0;
				m_pPPGSegment->dwLoopStartMeasure = m_pPPGSegment->dwLoopEndMeasure - 1;
			}
		}
	}
	m_spinStartTick.SetPos( m_pPPGSegment->wLoopStartTick );
	m_spinStartGrid.SetPos( m_pPPGSegment->wLoopStartGrid + 1 );
	m_spinStartBeat.SetPos( m_pPPGSegment->bLoopStartBeat + 1 );
	m_spinStartBar.SetPos( m_pPPGSegment->dwLoopStartMeasure + 1 );
}

void CLoopPPG::OnChangeEditLoopRepeat() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment && ::IsWindow(m_hWnd) && ::IsWindow(m_editLoopRepeat.m_hWnd) )
	{
		CString strNewLoopRepeat;

		m_editLoopRepeat.GetWindowText( strNewLoopRepeat );

		// Strip leading and trailing spaces
		strNewLoopRepeat.TrimRight();
		strNewLoopRepeat.TrimLeft();

		if( !strNewLoopRepeat.IsEmpty() )
		{
			DWORD dwNewLoopRepeat = _ttoi( strNewLoopRepeat );
			if( dwNewLoopRepeat < 0 )
			{
				dwNewLoopRepeat = 0;
			}
			else if( dwNewLoopRepeat > 32767 )
			{
				dwNewLoopRepeat = 32767;
			}

			// If going from 0 repeats to > 0 repeats
			if( !m_pPPGSegment->dwLoopRepeats && dwNewLoopRepeat )
			{
				DMUS_TIMESIGNATURE TimeSig;
				if( GetTimeSig( TimeSig, m_pPPGSegment->dwMeasureLength - 1 ) )
				{
					// Set the End time to be m_pPPGSegment->mtSegmentLength
					TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, m_pPPGSegment->mtSegmentLength,
					m_pPPGSegment->dwLoopEndMeasure, m_pPPGSegment->bLoopEndBeat, m_pPPGSegment->wLoopEndGrid, m_pPPGSegment->wLoopEndTick );

					m_spinEndBar.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
					m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
					m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
					m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );

					// Reset Start to 0
					m_pPPGSegment->dwLoopStartMeasure = 0;
					m_pPPGSegment->bLoopStartBeat = 0;
					m_pPPGSegment->wLoopStartGrid = 0;
					m_pPPGSegment->wLoopStartTick = 0;
					m_spinStartTick.SetPos( 0 );
					m_spinStartBar.SetPos( 1 );
					m_spinStartBeat.SetPos( 1 );
					m_spinStartGrid.SetPos( 1 );

					ResetEndRanges();
					ResetStartRanges();
				}
			}
			// If going from > 0 repeats to 0 repeats
			else if( m_pPPGSegment->dwLoopRepeats && !dwNewLoopRepeat )
			{
				// Leave the loop values the same
				//ResetLoopControls();
			}

			BOOL fEnable = (dwNewLoopRepeat != 0);
			EnableItem( IDC_EDIT_LOOP_START_BAR, fEnable );
			EnableItem( IDC_EDIT_LOOP_START_BEAT, fEnable );
			EnableItem( IDC_EDIT_LOOP_START_GRID, fEnable );
			EnableItem( IDC_EDIT_LOOP_START_TICK, fEnable );
			EnableItem( IDC_SPIN_LOOP_START_BAR, fEnable );
			EnableItem( IDC_SPIN_LOOP_START_BEAT, fEnable );
			EnableItem( IDC_SPIN_LOOP_START_GRID, fEnable );
			EnableItem( IDC_SPIN_LOOP_START_TICK, fEnable );

			EnableItem( IDC_EDIT_LOOP_END_BAR, fEnable );
			EnableItem( IDC_EDIT_LOOP_END_BEAT, fEnable );
			EnableItem( IDC_EDIT_LOOP_END_GRID, fEnable );
			EnableItem( IDC_EDIT_LOOP_END_TICK, fEnable );
			EnableItem( IDC_SPIN_LOOP_END_BAR, fEnable );
			EnableItem( IDC_SPIN_LOOP_END_BEAT, fEnable );
			EnableItem( IDC_SPIN_LOOP_END_GRID, fEnable );
			EnableItem( IDC_SPIN_LOOP_END_TICK, fEnable );

			EnableItem( IDC_RESET_LOOP, fEnable );
		}
	}
}

void CLoopPPG::OnCheckRepeatForever() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		bool fButtonChecked = IsDlgButtonChecked( IDC_CHECK_REPEAT_FOREVER ) ? true : false;
		DWORD dwNewLoopRepeat = fButtonChecked ? DMUS_SEG_REPEAT_INFINITE : m_dwLastLoopRepeatCount;

		if( dwNewLoopRepeat != m_pPPGSegment->dwLoopRepeats )
		{
			if( !fButtonChecked )
			{
				m_editLoopRepeat.EnableWindow( TRUE );
				m_spinRepeat.EnableWindow( TRUE );
				SetDlgItemInt( IDC_EDIT_LOOP_REPEAT, dwNewLoopRepeat, FALSE );
				m_spinRepeat.SetPos( dwNewLoopRepeat );
			}

			if( !m_pPPGSegment->dwLoopRepeats && dwNewLoopRepeat )
			{
				DMUS_TIMESIGNATURE TimeSig;
				if( GetTimeSig( TimeSig, m_pPPGSegment->dwMeasureLength - 1 ) )
				{
					// Set the End time to be m_pPPGSegment->mtSegmentLength
					TimeToBarBeatGridTick( m_pPPGSegment->pITimelineCtl, m_pPPGSegment->dwTrackGroup, m_pPPGSegment->mtSegmentLength,
					m_pPPGSegment->dwLoopEndMeasure, m_pPPGSegment->bLoopEndBeat, m_pPPGSegment->wLoopEndGrid, m_pPPGSegment->wLoopEndTick );

					m_spinEndBar.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
					m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
					m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
					m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );

					// Reset Start to 0
					m_pPPGSegment->dwLoopStartMeasure = 0;
					m_pPPGSegment->bLoopStartBeat = 0;
					m_pPPGSegment->wLoopStartGrid = 0;
					m_pPPGSegment->wLoopStartTick = 0;
					m_spinStartTick.SetPos( 0 );
					m_spinStartBar.SetPos( 1 );
					m_spinStartBeat.SetPos( 1 );
					m_spinStartGrid.SetPos( 1 );

					ResetEndRanges();
					ResetStartRanges();
				}
			}
			m_pPPGSegment->dwLoopRepeats = dwNewLoopRepeat;
			if( m_pPPGSegment->dwLoopRepeats == 0 )
			{
				ResetLoopControls();
			}
			UpdateSegment();
			EnableControls( TRUE );
		}
	}
}


void CLoopPPG::OnCheckLoopClocktime() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		if( IsDlgButtonChecked( IDC_CHECK_LOOP_CLOCKTIME ) )
		{
            m_pPPGSegment->dwSegmentFlags |= DMUS_SEGIOF_REFLOOP;
        }
        else
        {
            m_pPPGSegment->dwSegmentFlags &= ~DMUS_SEGIOF_REFLOOP;
        }
        EnableControls( TRUE );
        m_pPPGSegment->dwFlags = (m_pPPGSegment->dwFlags & PPGT_NONVALIDFLAGS) | PPGT_VALIDLOOP;
        UpdateSegment();

    }	
}

void CLoopPPG::OnKillfocusEditLoopClockend() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
        UpdateData(TRUE);
        m_pPPGSegment->rtLoopEnd = m_dwClockLoopEnd * 10000;
        m_pPPGSegment->dwFlags = (m_pPPGSegment->dwFlags & PPGT_NONVALIDFLAGS) | PPGT_VALIDLOOP;
        UpdateSegment();
    }    	
}


void CLoopPPG::OnKillfocusEditLoopClockstart() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
        UpdateData(TRUE);
        m_pPPGSegment->rtLoopStart = m_dwClockLoopStart * 10000;
        m_pPPGSegment->dwFlags = (m_pPPGSegment->dwFlags & PPGT_NONVALIDFLAGS) | PPGT_VALIDLOOP;
        UpdateSegment();
    }    	
}

