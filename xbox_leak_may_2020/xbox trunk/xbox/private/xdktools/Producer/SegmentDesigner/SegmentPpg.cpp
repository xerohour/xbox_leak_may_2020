// SegmentPPG.cpp : implementation file
//

#include "stdafx.h"
#include "SegmentDesignerDll.h"
#include "SegmentPPG.h"
#include "Filestructs.h"
#include "SegmentLength.h"
#include "segment.h"
#include <dmusici.h>
#include "LoopPPG.h"
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NOTE_TO_CLOCKS(note, ppq)	( (ppq) * 4 / (note) )

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPG property page

IMPLEMENT_DYNCREATE(CSegmentPPG, CPropertyPage)

CSegmentPPG::CSegmentPPG() : CPropertyPage(CSegmentPPG::IDD)
{
	//{{AFX_DATA_INIT(CSegmentPPG)
	m_nNumMeasures = 0;
	//}}AFX_DATA_INIT

	m_fNeedToDetach		= FALSE;
	m_pPPGSegment		= NULL;
	m_pPageManager		= NULL;
	m_wMaxBeat = 0;
	m_wMaxGrid = 0;
	m_dwMaxTick = 0;
}

CSegmentPPG::~CSegmentPPG()
{
	if(m_pPPGSegment != NULL)
	{
		delete m_pPPGSegment;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPG::SetData

HRESULT CSegmentPPG::SetData(const struct PPGSegment *pSegment)
{
	if(pSegment == NULL)
	{
		// Set m_pSegment to NULL
		if ( m_pPPGSegment )
		{
			delete m_pPPGSegment;
		}
		m_pPPGSegment = NULL;
		return S_OK;
	}

	if( m_pPPGSegment == NULL )
	{
		m_pPPGSegment = new PPGSegment;
		if( m_pPPGSegment == NULL )
		{
			return E_OUTOFMEMORY;
		}
	}

	// Copy the PPGSegment
	m_pPPGSegment->Copy( pSegment );

	m_pPPGSegment->dwFlags = (m_pPPGSegment->dwFlags & PPGT_NONVALIDFLAGS) | PPGT_VALIDSEGMENT;

	UpdateControls();

	return S_OK;
}

void CSegmentPPG::UpdateControls()
{
	// Set the states of the controls
	if( !IsWindow(m_hWnd) )
	{
		return;
	}

	ASSERT( m_pPPGSegment );

	if( !m_pPPGSegment )
	{
		return;
	}

	ResetStartRanges();

	EnableControls( TRUE );
	/*
	// prevent control notifications from being dispatched during UpdateData
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;
	*/


	m_editName.SetWindowText( m_pPPGSegment->strSegmentName );

	// Update the length button text
	UpdateLengthButtonText();

	m_spinStartBar.SetPos( m_pPPGSegment->dwPlayStartMeasure + 1 );
	m_spinStartBeat.SetPos( m_pPPGSegment->bPlayStartBeat + 1 );
	m_spinStartGrid.SetPos( m_pPPGSegment->wPlayStartGrid + 1 );
	m_spinStartTick.SetPos( m_pPPGSegment->wPlayStartTick );
	ResetStartRanges();

	DWORD dwGroup = m_pPPGSegment->dwTrackGroup;
	long lTrackGroup = 0;
	while ( dwGroup )
	{
		lTrackGroup++;
		dwGroup = dwGroup>>1;
	}
	m_spinTrackGroup.SetPos( lTrackGroup );

	m_buttonAudioPath.SetCheck( ((m_pPPGSegment->dwFlags & PPGT_HAVEAUDIOPATH) && (m_pPPGSegment->dwResolution & DMUS_SEGF_USE_AUDIOPATH)) ? 1 : 0 );
	m_buttonExtendTimeSig.SetCheck( (m_pPPGSegment->dwResolution & DMUS_SEGF_TIMESIG_ALWAYS) ? 1 : 0 );

/*	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;*/
	return;
}

void CSegmentPPG::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSegmentPPG)
	DDX_Control(pDX, IDC_CHECK_EXTEND_TIMESIG, m_buttonExtendTimeSig);
	DDX_Control(pDX, IDC_CHECK_AUDIOPATH, m_buttonAudioPath);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_TRACK_GROUP, m_editTrackGroup);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_TICK, m_editStartTick);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_GRID, m_editStartGrid);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_BEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_BAR, m_editStartBar);
	DDX_Control(pDX, IDC_SPIN_TRACK_GROUP, m_spinTrackGroup);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_TICK, m_spinStartTick);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_GRID, m_spinStartGrid);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_BEAT, m_spinStartBeat);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_BAR, m_spinStartBar);
	DDX_Control(pDX, IDC_BUTTON_LENGTH, m_buttonLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSegmentPPG, CPropertyPage)
	//{{AFX_MSG_MAP(CSegmentPPG)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_LENGTH, OnLength)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_BAR, OnKillfocusEditPlayStartBar)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_BEAT, OnKillfocusEditPlayStartBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_GRID, OnKillfocusEditPlayStartGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_TICK, OnKillfocusEditPlayStartTick)
	ON_EN_KILLFOCUS(IDC_EDIT_TRACK_GROUP, OnKillfocusEditTrackGroup)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_BAR, OnDeltaposSpinPlayStartBar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_BEAT, OnDeltaposSpinPlayStartBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_GRID, OnDeltaposSpinPlayStartGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_TICK, OnDeltaposSpinPlayStartTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRACK_GROUP, OnDeltaposSpinTrackGroup)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnKillfocusEditName)
	ON_BN_CLICKED(IDC_CHECK_AUDIOPATH, OnCheckAudiopath)
	ON_BN_CLICKED(IDC_CHECK_EXTEND_TIMESIG, OnCheckExtendTimesig)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPG::OnInitDialog

BOOL CSegmentPPG::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();
	
	m_editName.LimitText( DMUS_MAX_NAME - 1 );

	m_spinTrackGroup.SetRange( 1, 32 );
	m_editTrackGroup.LimitText( 2 );

	m_spinStartBar.SetRange( 1, 32767 );
	m_editStartBar.LimitText( 5 );

	m_spinStartBeat.SetRange( 1, 256 );
	m_editStartBeat.LimitText( 3 );

	m_spinStartGrid.SetRange( 1, 256 );
	m_editStartGrid.LimitText( 3 );

	m_spinStartTick.SetRange( 0, 32767 );
	m_editStartTick.LimitText( 5 );

	SetData( m_pPPGSegment );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPG::EnableControls

void CSegmentPPG::EnableControls(BOOL fEnable) 
{
	m_editName.EnableWindow( fEnable );

	if ( fEnable && m_pPPGSegment && m_pPPGSegment->pITimelineCtl )
	{
		m_buttonLength.EnableWindow( TRUE );
		m_editStartBar.EnableWindow( TRUE );
		m_spinStartBar.EnableWindow( TRUE );
		m_editStartBeat.EnableWindow( TRUE );
		m_spinStartBeat.EnableWindow( TRUE );
		m_editStartGrid.EnableWindow( TRUE );
		m_spinStartGrid.EnableWindow( TRUE );
		m_editStartTick.EnableWindow( TRUE );
		m_spinStartTick.EnableWindow( TRUE );
		m_editTrackGroup.EnableWindow( TRUE );
		m_spinTrackGroup.EnableWindow( TRUE );
		m_buttonAudioPath.EnableWindow( m_pPPGSegment->dwFlags & PPGT_HAVEAUDIOPATH ? TRUE : FALSE );
		m_buttonExtendTimeSig.EnableWindow( TRUE );
	}
	else if( fEnable && m_pPPGSegment )
	{
		m_buttonAudioPath.EnableWindow( m_pPPGSegment->dwFlags & PPGT_HAVEAUDIOPATH ? TRUE : FALSE );
		m_buttonExtendTimeSig.EnableWindow( TRUE );
	}
	else
	{
		m_buttonLength.EnableWindow( FALSE );
		m_editStartBar.EnableWindow( FALSE );
		m_spinStartBar.EnableWindow( FALSE );
		m_editStartBeat.EnableWindow( FALSE );
		m_spinStartBeat.EnableWindow( FALSE );
		m_editStartGrid.EnableWindow( FALSE );
		m_spinStartGrid.EnableWindow( FALSE );
		m_editStartTick.EnableWindow( FALSE );
		m_spinStartTick.EnableWindow( FALSE );
		m_editTrackGroup.EnableWindow( FALSE );
		m_spinTrackGroup.EnableWindow( FALSE );
		m_buttonAudioPath.EnableWindow( FALSE );
		m_buttonExtendTimeSig.EnableWindow( FALSE );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPG message handlers

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPG::OnSetActive

BOOL CSegmentPPG::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !m_pPPGSegment )
	{
		EnableControls(FALSE);
		return CPropertyPage::OnSetActive();
	}

	EnableControls(TRUE);

	// Set the controls in case they have changed since this was last activated
	// and RefreshData has not yet been called.
	SetData( m_pPPGSegment );

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CSegmentPPGMgr::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentPPG::OnCreate

int CSegmentPPG::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CSegmentPPG::OnDestroy

void CSegmentPPG::OnDestroy() 
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
// CSegmentPPG::SetWarnUser

/*
void CSegmentPPG::SetWarnUser(BOOL bWarnUser)
{
	ASSERT(m_pPPGSegment != NULL);
	if(m_pPPGSegment == NULL)
	{
		return;
	}
	if(bWarnUser)
	{
		m_pPPGSegment->dwFlags |= PPGT_WARNUSER;
	}
	else
	{
		m_pPPGSegment->dwFlags &= ~PPGT_WARNUSER;
	}
}
*/

void CSegmentPPG::OnLength() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if ( !m_pPPGSegment )
	{
		return;
	}

	CSegmentLength dlg;
	dlg.m_dwLength = m_pPPGSegment->dwMeasureLength;
	dlg.m_dwNbrExtraBars = m_pPPGSegment->dwExtraBars;
	dlg.m_fPickupBar = m_pPPGSegment->fPickupBar;
	dlg.m_fClockTime = m_pPPGSegment->dwSegmentFlags & DMUS_SEGIOF_REFLENGTH;
	dlg.m_rtLength = m_pPPGSegment->rtSegmentLength;
	dlg.m_pTimeline = m_pPPGSegment->pITimelineCtl;
	dlg.m_dwGroupBits = m_pPPGSegment->dwTrackGroup;

	UINT rc = dlg.DoModal();
	if(rc == IDOK)
	{
		m_pPPGSegment->dwMeasureLength = dlg.m_dwLength;
		m_pPPGSegment->dwExtraBars = dlg.m_dwNbrExtraBars;
		m_pPPGSegment->fPickupBar = dlg.m_fPickupBar;
		m_pPPGSegment->rtSegmentLength = dlg.m_rtLength;

		if( dlg.m_fClockTime )
		{
			m_pPPGSegment->dwSegmentFlags |= DMUS_SEGIOF_REFLENGTH;
		}
		else
		{
			m_pPPGSegment->dwSegmentFlags &= ~DMUS_SEGIOF_REFLENGTH;
		}

		// Update the length button text
		UpdateLengthButtonText();

		// Update the loop PPG
		/* No need - handled by CSegment::SetData()
		if( m_pPageManager && m_pPageManager->m_pLoopPPG )
		{
			m_pPageManager->m_pLoopPPG->SetLength( m_pPPGSegment->dwMeasureLength );
		}
		*/

		// Update the Segment
		UpdateSegment();
	}
	else
	{
		// Update the length button text
		UpdateLengthButtonText();
	}
}

void CSegmentPPG::UpdateSegment()
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

void CSegmentPPG::OnKillfocusEditPlayStartBar() 
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
				m_spinStartBar.SetPos( m_pPPGSegment->dwPlayStartMeasure + 1 );
			}
			else
			{
				DWORD dwNewStartBar = _ttoi( strNewStartBar );
				if( dwNewStartBar < 1)
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
				if( dwNewStartBar != m_pPPGSegment->dwPlayStartMeasure )
				{
					m_pPPGSegment->dwPlayStartMeasure = dwNewStartBar;
					ResetStartRanges();
					UpdateSegment();
				}
			}
		}
	}
}

void CSegmentPPG::OnKillfocusEditPlayStartBeat() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strNewStartBeat;

	m_editStartBeat.GetWindowText( strNewStartBeat );

	// Strip leading and trailing spaces
	strNewStartBeat.TrimRight();
	strNewStartBeat.TrimLeft();

	if( strNewStartBeat.IsEmpty() )
	{
		m_spinStartBeat.SetPos( m_pPPGSegment->bPlayStartBeat + 1 );
	}
	else
	{
		int iNewStartBeat = _ttoi( strNewStartBeat );
		if( iNewStartBeat > m_wMaxBeat )
		{
			iNewStartBeat = m_wMaxBeat;
			m_spinStartBeat.SetPos( iNewStartBeat );
		}
		else if( iNewStartBeat < 1 )
		{
			iNewStartBeat = 1;
			m_spinStartBeat.SetPos( iNewStartBeat );
		}
		iNewStartBeat--;
		if( iNewStartBeat != m_pPPGSegment->bPlayStartBeat )
		{
			m_pPPGSegment->bPlayStartBeat = (BYTE)iNewStartBeat;
			UpdateSegment();
		}
	}
}

void CSegmentPPG::OnKillfocusEditPlayStartGrid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strNewStartGrid;

	m_editStartGrid.GetWindowText( strNewStartGrid );

	// Strip leading and trailing spaces
	strNewStartGrid.TrimRight();
	strNewStartGrid.TrimLeft();

	if( strNewStartGrid.IsEmpty() )
	{
		m_spinStartGrid.SetPos( m_pPPGSegment->wPlayStartGrid + 1 );
	}
	else
	{
		int iNewStartGrid = _ttoi( strNewStartGrid );
		if( iNewStartGrid > m_wMaxGrid )
		{
			iNewStartGrid = m_wMaxGrid;
			m_spinStartGrid.SetPos( iNewStartGrid );
		}
		else if( iNewStartGrid < 1 )
		{
			iNewStartGrid = 1;
			m_spinStartGrid.SetPos( iNewStartGrid );
		}
		iNewStartGrid--;
		if( iNewStartGrid != m_pPPGSegment->wPlayStartGrid )
		{
			m_pPPGSegment->wPlayStartGrid = (WORD)iNewStartGrid;
			UpdateSegment();
		}
	}
}

void CSegmentPPG::OnKillfocusEditPlayStartTick() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strNewStartTick;

	m_editStartTick.GetWindowText( strNewStartTick );

	// Strip leading and trailing spaces
	strNewStartTick.TrimRight();
	strNewStartTick.TrimLeft();

	if( strNewStartTick.IsEmpty() )
	{
		m_spinStartTick.SetPos( m_pPPGSegment->wPlayStartTick );
	}
	else
	{
		int iNewStartTick = _ttoi( strNewStartTick );
		if( iNewStartTick > (int)m_dwMaxTick )
		{
			iNewStartTick = m_dwMaxTick;
			m_spinStartTick.SetPos( iNewStartTick );
		}
		else if( iNewStartTick < 0 )
		{
			iNewStartTick = 0;
			m_spinStartTick.SetPos( iNewStartTick );
		}
		if( iNewStartTick != m_pPPGSegment->wPlayStartTick )
		{
			m_pPPGSegment->wPlayStartTick = (WORD) iNewStartTick;
			UpdateSegment();
		}
	}
}

void CSegmentPPG::OnKillfocusEditTrackGroup() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment == NULL )
	{
		return;
	}

	CString strNewTrackGroup;

	m_editTrackGroup.GetWindowText( strNewTrackGroup );

	// Strip leading and trailing spaces
	strNewTrackGroup.TrimRight();
	strNewTrackGroup.TrimLeft();

	if( strNewTrackGroup.IsEmpty() )
	{
		DWORD dwGroup = m_pPPGSegment->dwTrackGroup;
		long lTrackGroup = 0;
		while ( dwGroup )
		{
			lTrackGroup++;
			dwGroup = dwGroup>>1;
		}
		m_spinTrackGroup.SetPos( lTrackGroup );
	}
	else
	{
		int nNewTrackGroup = _ttoi( strNewTrackGroup );
		if( nNewTrackGroup > 32 )
		{
			nNewTrackGroup = 32;
			m_spinTrackGroup.SetPos( nNewTrackGroup );
		}
		else if( nNewTrackGroup < 1 )
		{
			nNewTrackGroup = 1;
			m_spinTrackGroup.SetPos( nNewTrackGroup );
		}
		nNewTrackGroup--;
		DWORD dwNewBits = 1 << nNewTrackGroup;
		if( dwNewBits != m_pPPGSegment->dwTrackGroup )
		{
			m_pPPGSegment->dwTrackGroup = dwNewBits;
			ResetStartRanges();
			UpdateSegment();
		}
	}
}

void CSegmentPPG::OnDeltaposSpinPlayStartBar(NMHDR* pNMHDR, LRESULT* pResult) 
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
		else if( lNewValue > 32767 )
		{
			lNewValue = 32767;
		}
		lNewValue--;

		if( (DWORD)lNewValue != m_pPPGSegment->dwPlayStartMeasure )
		{
			m_pPPGSegment->dwPlayStartMeasure = (DWORD)lNewValue;
			ResetStartRanges();
			UpdateSegment();
		}
	}

	*pResult = 0;
}

void CSegmentPPG::OnDeltaposSpinPlayStartBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_spinStartBeat.GetPos() + pNMUpDown->iDelta;
		if( nNewValue < 1 )
		{
			nNewValue = 1;
		}
		else if( nNewValue > m_wMaxBeat )
		{
			nNewValue = m_wMaxBeat;
		}
		nNewValue--;

		if( nNewValue != m_pPPGSegment->bPlayStartBeat )
		{
			m_pPPGSegment->bPlayStartBeat = (BYTE)nNewValue;
			UpdateSegment();
		}
	}

	*pResult = 0;
}

void CSegmentPPG::OnDeltaposSpinPlayStartGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_spinStartGrid.GetPos() + pNMUpDown->iDelta;
		if( nNewValue < 1 )
		{
			nNewValue = 1;
		}
		else if( nNewValue > m_wMaxGrid )
		{
			nNewValue = m_wMaxGrid;
		}
		nNewValue--;

		if( nNewValue != m_pPPGSegment->wPlayStartGrid )
		{
			m_pPPGSegment->wPlayStartGrid = (WORD)nNewValue;
			UpdateSegment();
		}
	}

	*pResult = 0;
}

void CSegmentPPG::OnDeltaposSpinPlayStartTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_spinStartTick.GetPos() + pNMUpDown->iDelta;
		if( nNewValue < 0 )
		{
			nNewValue = 0;
		}
		else if( nNewValue > (int)m_dwMaxTick )
		{
			nNewValue = m_dwMaxTick;
		}

		if( nNewValue != m_pPPGSegment->wPlayStartTick )
		{
			m_pPPGSegment->wPlayStartTick = (WORD) nNewValue;
			UpdateSegment();
		}
	}

	*pResult = 0;
}

void CSegmentPPG::OnDeltaposSpinTrackGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nTrackIndex = m_spinTrackGroup.GetPos() + pNMUpDown->iDelta - 1;
		DWORD dwNewGroup = 1 << nTrackIndex;
		if( dwNewGroup != m_pPPGSegment->dwTrackGroup )
		{
			m_pPPGSegment->dwTrackGroup = dwNewGroup;
			ResetStartRanges();
			UpdateSegment();
			*pResult = 1;
			return;
		}
	}

	*pResult = 0;
}

BOOL CSegmentPPG::PreTranslateMessage(MSG* pMsg) 
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
							case IDC_EDIT_NAME: 
								m_editName.SetWindowText( m_pPPGSegment->strSegmentName );
								break;

							case IDC_EDIT_TRACK_GROUP:
								{
									DWORD dwGroup = m_pPPGSegment->dwTrackGroup;
									long lTrackGroup = 0;
									while ( dwGroup )
									{
										lTrackGroup++;
										dwGroup = dwGroup>>1;
									}
									m_spinTrackGroup.SetPos( lTrackGroup );
								}
								break;

							case IDC_EDIT_PLAY_START_BAR: 
								m_spinStartBar.SetPos( m_pPPGSegment->dwPlayStartMeasure + 1 );
								break;

							case IDC_EDIT_PLAY_START_BEAT: 
								m_spinStartBeat.SetPos( m_pPPGSegment->bPlayStartBeat + 1 );
								break;

							case IDC_EDIT_PLAY_START_GRID: 
								m_spinStartGrid.SetPos( m_pPPGSegment->wPlayStartGrid + 1 );
								break;

							case IDC_EDIT_PLAY_START_TICK: 
								m_spinStartTick.SetPos( m_pPPGSegment->wPlayStartTick );
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

void CSegmentPPG::OnKillfocusEditName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment )
	{
		CString strName;

		m_editName.GetWindowText( strName );

		// Strip leading and trailing spaces
		strName.TrimRight();
		strName.TrimLeft();

		if( strName.IsEmpty() )
		{
			m_editName.SetWindowText( m_pPPGSegment->strSegmentName );
		}
		else
		{
			if( strName.Compare( m_pPPGSegment->strSegmentName ) != 0 )
			{
				m_pPPGSegment->strSegmentName = strName;
				UpdateSegment();
			}
		}
	}
}

void CSegmentPPG::ResetStartRanges()
{
	DMUS_TIMESIGNATURE TimeSig;
	if( GetTimeSig( TimeSig ) )
	{
		long lTemp;
		CString strText;

		// Limit start bar
		lTemp = m_pPPGSegment->dwMeasureLength;
		m_spinStartBar.SetRange( 1, lTemp );
		strText.Format("%d",lTemp);
		m_editStartBar.LimitText( strText.GetLength() );

		// Limit start beat
		lTemp = TimeSig.bBeatsPerMeasure;
		if( m_pPPGSegment->bPlayStartBeat > lTemp - 1 )
		{
			m_spinStartBeat.SetPos( lTemp );
			m_pPPGSegment->bPlayStartBeat = BYTE(lTemp - 1);
		}
		m_wMaxBeat = (WORD)lTemp;
		m_spinStartBeat.SetRange( 1, lTemp );
		strText.Format("%d",lTemp);
		m_editStartBeat.LimitText( strText.GetLength() );

		// Limit start grid
		lTemp = TimeSig.wGridsPerBeat;
		if( m_pPPGSegment->wPlayStartGrid > lTemp - 1 )
		{
			m_spinStartGrid.SetPos( lTemp );
			m_pPPGSegment->wPlayStartGrid = WORD(lTemp - 1);
		}
		m_wMaxGrid = (WORD)lTemp;
		m_spinStartGrid.SetRange( 1, lTemp );
		strText.Format("%d",lTemp);
		m_editStartGrid.LimitText( strText.GetLength() );

		// Limit start tick
		lTemp = (NOTE_TO_CLOCKS(TimeSig.bBeat, DMUS_PPQ) / TimeSig.wGridsPerBeat) - 1;
		if( m_pPPGSegment->wPlayStartTick > lTemp )
		{
			m_spinStartTick.SetPos( lTemp );
			m_pPPGSegment->wPlayStartTick = (WORD)lTemp;
		}
		m_dwMaxTick = lTemp;
		m_spinStartTick.SetRange( 0, lTemp );
		strText.Format("%d",lTemp);
		m_editStartTick.LimitText( strText.GetLength() );
	}
}

BOOL CSegmentPPG::GetTimeSig(DMUS_TIMESIGNATURE &TimeSig)
{
	if( m_pPPGSegment )
	{
		if( m_pPPGSegment->pITimelineCtl )
		{
			MUSIC_TIME mt;
			if( SUCCEEDED( m_pPPGSegment->pITimelineCtl->MeasureBeatToClocks( m_pPPGSegment->dwTrackGroup, 0, m_pPPGSegment->dwPlayStartMeasure, 0, &mt ) ) )
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

void CSegmentPPG::OnCheckAudiopath() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment && m_buttonAudioPath.GetSafeHwnd() )
	{
		BOOL fChange = FALSE;
		if( m_buttonAudioPath.GetCheck() == 1 )
		{
			if( 0 == (m_pPPGSegment->dwResolution & DMUS_SEGF_USE_AUDIOPATH) )
			{
				fChange = TRUE;
				m_pPPGSegment->dwResolution |= DMUS_SEGF_USE_AUDIOPATH;
			}
		}
		else
		{
			if( m_pPPGSegment->dwResolution & DMUS_SEGF_USE_AUDIOPATH )
			{
				fChange = TRUE;
				m_pPPGSegment->dwResolution &= ~DMUS_SEGF_USE_AUDIOPATH;
			}
		}

		if( fChange )
		{
			UpdateSegment();
		}
	}
}

void CSegmentPPG::OnCheckExtendTimesig() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPPGSegment && m_buttonExtendTimeSig.GetSafeHwnd() )
	{
		BOOL fChange = FALSE;
		if( m_buttonExtendTimeSig.GetCheck() == 1 )
		{
			if( 0 == (m_pPPGSegment->dwResolution & DMUS_SEGF_TIMESIG_ALWAYS) )
			{
				fChange = TRUE;
				m_pPPGSegment->dwResolution |= DMUS_SEGF_TIMESIG_ALWAYS;
			}
		}
		else
		{
			if( m_pPPGSegment->dwResolution & DMUS_SEGF_TIMESIG_ALWAYS )
			{
				fChange = TRUE;
				m_pPPGSegment->dwResolution &= ~DMUS_SEGF_TIMESIG_ALWAYS;
			}
		}

		if( fChange )
		{
			UpdateSegment();
		}
	}
}

void CSegmentPPG::UpdateLengthButtonText()
{
	// Set the number of measures
	CString str;
	if( m_pPPGSegment->dwSegmentFlags & DMUS_SEGIOF_REFLENGTH )
	{
		CString strFormat;
		strFormat.LoadString( IDS_LENGTH_CLOCKTIME_TEXT );

		REFERENCE_TIME rtMillisecondLength = (m_pPPGSegment->rtSegmentLength + 5000) / 10000;

		str.Format( strFormat, DWORD(rtMillisecondLength / 60000), DWORD((rtMillisecondLength / 1000) % 60), DWORD(rtMillisecondLength % 1000) );
	}
	else
	{
		if( m_pPPGSegment->dwMeasureLength > 1 )
		{
			CString strFormat;
			strFormat.LoadString( IDS_LENGTH_BARS_TEXT );
			str.Format(strFormat, (int) m_pPPGSegment->dwMeasureLength);
		}
		else
		{
			str.LoadString( IDS_LENGTH_BAR_TEXT );
		}
	}
	m_buttonLength.SetWindowText(str);
}
