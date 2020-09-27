// TabLoop.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TrackItem.h"
#include "PropTrackItem.h"
#include "PropPageMgr_Item.h"
#include "TabLoop.h"
#include "TabVariations.h"
#include "TabPerformance.h"
#include <dmusici.h>
#include <dmusicf.h>
#include "LockoutNotification.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Length (in characters) of the string to allocate to store text entered in
// the measure and beat edit boxes
#define DIALOG_EDIT_LEN 15

/////////////////////////////////////////////////////////////////////////////
// CTabLoop property page

IMPLEMENT_DYNCREATE(CTabLoop, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CTabLoop constructor/destructor

CTabLoop::CTabLoop(): CPropertyPage(CTabLoop::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_PROPPAGE_WAVE_LOOP);
	//{{AFX_DATA_INIT(CTabLoop)
	//}}AFX_DATA_INIT

	// Initialize our pointers to NULL
	m_pIWaveTimelineDraw = NULL;
	m_pPropPageMgr = NULL;
	m_pTimeline = NULL;

	// Initialize our class members to FALSE
	m_fValidTrackItem = FALSE;
	m_fNeedToDetach = FALSE;

	m_lStartMeasure = 0;
	m_lStartBeat = 0;
	m_lStartGrid = 0;
	m_lStartTick = 0;

	m_lEndMeasure = 0;
	m_lEndBeat = 0;
	m_lEndGrid = 0;
	m_lEndTick = 0;
	
	m_lLengthMeasure = 0;
	m_lLengthBeat = 0;
	m_lLengthGrid = 0;
	m_lLengthTick = 0;
	m_dwLengthNbrSamples = 0;

	m_lSourceStartMeasure = 0;
	m_lSourceStartBeat = 0;
	m_lSourceStartGrid = 0;
	m_lSourceStartTick = 0;

	m_lSourceEndMeasure = 0;
	m_lSourceEndBeat = 0;
	m_lSourceEndGrid = 0;
	m_lSourceEndTick = 0;
	
	m_lSourceLengthMeasure = 0;
	m_lSourceLengthBeat = 0;
	m_lSourceLengthGrid = 0;
	m_lSourceLengthTick = 0;
	m_dwSourceLengthNbrSamples = 0;
}

CTabLoop::~CTabLoop()
{
	RELEASE( m_pIWaveTimelineDraw );
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::DoDataExchange

void CTabLoop::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CTabLoop)
	DDX_Control(pDX, IDC_CHECK_LOOP, m_checkLoop);
	DDX_Control(pDX, IDC_LOCK_LENGTH, m_btnLockLength);
	DDX_Control(pDX, IDC_PROMPT_GRID, m_staticPromptGrid);
	DDX_Control(pDX, IDC_PROMPT_TICK, m_staticPromptTick);
	DDX_Control(pDX, IDC_PROMPT_BEAT, m_staticPromptBeat);
	DDX_Control(pDX, IDC_PROMPT_BAR, m_staticPromptBar);
	DDX_Control(pDX, IDC_EDIT_START_TICK, m_editStartTick);
	DDX_Control(pDX, IDC_EDIT_START_GRID, m_editStartGrid);
	DDX_Control(pDX, IDC_EDIT_START_BEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_EDIT_START_BAR, m_editStartMeasure);
	DDX_Control(pDX, IDC_EDIT_START_SAMPLE, m_editStartSample);
	DDX_Control(pDX, IDC_EDIT_END_TICK, m_editEndTick);
	DDX_Control(pDX, IDC_EDIT_END_GRID, m_editEndGrid);
	DDX_Control(pDX, IDC_EDIT_END_BEAT, m_editEndBeat);
	DDX_Control(pDX, IDC_EDIT_END_BAR, m_editEndMeasure);
	DDX_Control(pDX, IDC_EDIT_END_SAMPLE, m_editEndSample);
	DDX_Control(pDX, IDC_EDIT_LENGTH_TICK, m_editLengthTick);
	DDX_Control(pDX, IDC_EDIT_LENGTH_GRID, m_editLengthGrid);
	DDX_Control(pDX, IDC_EDIT_LENGTH_BEAT, m_editLengthBeat);
	DDX_Control(pDX, IDC_EDIT_LENGTH_BAR, m_editLengthMeasure);
	DDX_Control(pDX, IDC_EDIT_LENGTH_NBR_SAMPLES, m_editLengthNbrSamples);
	DDX_Control(pDX, IDC_SPIN_START_TICK, m_spinStartTick);
	DDX_Control(pDX, IDC_SPIN_START_GRID, m_spinStartGrid);
	DDX_Control(pDX, IDC_SPIN_START_BEAT, m_spinStartBeat);
	DDX_Control(pDX, IDC_SPIN_START_BAR, m_spinStartMeasure);
	DDX_Control(pDX, IDC_SPIN_START_SAMPLE, m_spinStartSample);
	DDX_Control(pDX, IDC_SPIN_END_TICK, m_spinEndTick);
	DDX_Control(pDX, IDC_SPIN_END_GRID, m_spinEndGrid);
	DDX_Control(pDX, IDC_SPIN_END_BEAT, m_spinEndBeat);
	DDX_Control(pDX, IDC_SPIN_END_BAR, m_spinEndMeasure);
	DDX_Control(pDX, IDC_SPIN_END_SAMPLE, m_spinEndSample);
	DDX_Control(pDX, IDC_SPIN_LENGTH_TICK, m_spinLengthTick);
	DDX_Control(pDX, IDC_SPIN_LENGTH_GRID, m_spinLengthGrid);
	DDX_Control(pDX, IDC_SPIN_LENGTH_BEAT, m_spinLengthBeat);
	DDX_Control(pDX, IDC_SPIN_LENGTH_BAR, m_spinLengthMeasure);
	DDX_Control(pDX, IDC_SPIN_LENGTH_NBR_SAMPLES, m_spinLengthNbrSamples);
	DDX_Control(pDX, IDC_STATIC_SOURCE_START_TICK, m_staticSourceStartTick);
	DDX_Control(pDX, IDC_STATIC_SOURCE_START_GRID, m_staticSourceStartGrid);
	DDX_Control(pDX, IDC_STATIC_SOURCE_START_BEAT, m_staticSourceStartBeat);
	DDX_Control(pDX, IDC_STATIC_SOURCE_START_BAR, m_staticSourceStartMeasure);
	DDX_Control(pDX, IDC_STATIC_SOURCE_START_SAMPLE, m_staticSourceStartSample);
	DDX_Control(pDX, IDC_STATIC_SOURCE_END_TICK, m_staticSourceEndTick);
	DDX_Control(pDX, IDC_STATIC_SOURCE_END_GRID, m_staticSourceEndGrid);
	DDX_Control(pDX, IDC_STATIC_SOURCE_END_BEAT, m_staticSourceEndBeat);
	DDX_Control(pDX, IDC_STATIC_SOURCE_END_BAR, m_staticSourceEndMeasure);
	DDX_Control(pDX, IDC_STATIC_SOURCE_END_SAMPLE, m_staticSourceEndSample);
	DDX_Control(pDX, IDC_STATIC_SOURCE_LENGTH_TICK, m_staticSourceLengthTick);
	DDX_Control(pDX, IDC_STATIC_SOURCE_LENGTH_GRID, m_staticSourceLengthGrid);
	DDX_Control(pDX, IDC_STATIC_SOURCE_LENGTH_BEAT, m_staticSourceLengthBeat);
	DDX_Control(pDX, IDC_STATIC_SOURCE_LENGTH_BAR, m_staticSourceLengthMeasure);
	DDX_Control(pDX, IDC_STATIC_SOURCE_LENGTH_NBR_SAMPLES, m_staticSourceLengthNbrSamples);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabLoop, CPropertyPage)
	//{{AFX_MSG_MAP(CTabLoop)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_TICK, OnSpinStartTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_GRID, OnSpinStartGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_BEAT, OnSpinStartBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_BAR, OnSpinStartMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_SAMPLE, OnSpinStartSample)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_TICK, OnSpinEndTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_GRID, OnSpinEndGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_BEAT, OnSpinEndBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_BAR, OnSpinEndMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_SAMPLE, OnSpinEndSample)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH_TICK, OnSpinLengthTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH_GRID, OnSpinLengthGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH_BEAT, OnSpinLengthBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH_BAR, OnSpinLengthMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH_NBR_SAMPLES, OnSpinLengthNbrSamples)
	ON_EN_KILLFOCUS(IDC_EDIT_START_TICK, OnKillFocusStartTick)
	ON_EN_KILLFOCUS(IDC_EDIT_START_GRID, OnKillFocusStartGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_START_BEAT, OnKillFocusStartBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_START_BAR, OnKillFocusStartMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_START_SAMPLE, OnKillFocusStartSample)
	ON_EN_KILLFOCUS(IDC_EDIT_END_TICK, OnKillFocusEndTick)
	ON_EN_KILLFOCUS(IDC_EDIT_END_GRID, OnKillFocusEndGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_END_BEAT, OnKillFocusEndBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_END_BAR, OnKillFocusEndMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_END_SAMPLE, OnKillFocusEndSample)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH_TICK, OnKillFocusLengthTick)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH_GRID, OnKillFocusLengthGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH_BEAT, OnKillFocusLengthBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH_BAR, OnKillFocusLengthMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH_NBR_SAMPLES, OnKillFocusLengthNbrSamples)
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnCheckLoop)
	ON_BN_DOUBLECLICKED(IDC_CHECK_LOOP, OnDoubleClickedCheckLoop)
	ON_WM_KILLFOCUS()
	ON_BN_DOUBLECLICKED(IDC_LOCK_LENGTH, OnDoubleClickedLockLength)
	ON_BN_CLICKED(IDC_LOCK_LENGTH, OnLockLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabLoop custom functions

/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SetPropTrackItem

void CTabLoop::SetPropTrackItem( const CPropTrackItem* pPropItem )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the pointer is null, there are no items selected, so disable the property page
	// If pIDocRootNode is null, a wave has not been assigned to the item, so disable the property page
	if( pPropItem == NULL
	||  pPropItem->m_Item.m_FileRef.pIDocRootNode == NULL )
	{
		m_fValidTrackItem = FALSE;
		m_PropItem.Clear();
		EnableControls( FALSE );
		return;
	}

	// Store IDMUSProdWaveTimelineDraw interface pointer
	RELEASE( m_pIWaveTimelineDraw );
	if( FAILED ( pPropItem->m_Item.m_FileRef.pIDocRootNode->QueryInterface( IID_IDMUSProdWaveTimelineDraw, (void **)&m_pIWaveTimelineDraw ) ) )
	{
		// Cannot update properties without this interface
		m_fValidTrackItem = FALSE;
		m_PropItem.Clear();
		EnableControls( FALSE );
		return;
	}

	// Initialize the timeline pointer to NULL
	m_pTimeline = NULL;

	// Flag that we have a valid item
	m_fValidTrackItem = TRUE;

	// Copy the information from the new item
	m_PropItem.Copy( pPropItem );
	m_PropItem.m_nPropertyTab = TAB_LOOP;
	m_PropItem.m_dwChanged = 0;

	// Compute values for controls displaying "time"
	// Determine measure, beat, grid, tick
	// OR
	// Determine minute, second, grid, millisecond
	m_dwLengthNbrSamples = (m_PropItem.m_Item.m_dwLoopEndUI - m_PropItem.m_Item.m_dwLoopStartUI) + 1;
	RecomputeLoopTimes();

	// Check if the edit control has a valid window handle.
	if( m_editStartBeat.GetSafeHwnd() == NULL )
	{
		// It doesn't have a valid window handle - the property page may have been destroyed (or not yet created).
		// Just exit early
		return;
	}

	// Don't send OnKill/OnUpdate notifications when updating the display
	CLockoutNotification LockoutNotifications( m_hWnd );

	// Check if wave length is less than minimum loop length
	if( m_dwLengthNbrSamples < (MIN_LOOP_LENGTH + 1) )
	{
		// Yes - disable the property page and return
		EnableControls( FALSE );
		return;
	}

	// Update the property page based on the new data.
	// It is usually profitable to check to see if the data actually changed
	// before updating the controls in the property page.

	// Enable all the controls
	EnableControls( TRUE );

	// Update Loop check box
	if( m_PropItem.m_dwUndetermined_TabLoop & UNDT_IS_LOOPED )
	{
		m_checkLoop.SetCheck( 2 );
	}
	else
	{
		m_checkLoop.SetCheck( m_PropItem.m_Item.m_fLoopedUI ? 1 : 0 );
	}

	if( m_PropItem.m_dwBits & PROPF_MULTIPLESELECT )
	{
		// For now don't set value of other controls when multiple waves selected
		return;
	}

	// Set nMeasureBeatOffset
	int nMeasureBeatOffset;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		nMeasureBeatOffset = 0;
	}
	else
	{
		nMeasureBeatOffset = 1;
	}

	// Set GRID controls
	CWaveStrip* pWaveStrip = m_PropItem.m_Item.GetWaveStripForPropSheet();
	if( pWaveStrip
	&&  pWaveStrip->IsOneGrid() )
	{
		m_editStartGrid.SetWindowText( NULL );
		m_editStartGrid.EnableWindow( FALSE );
		m_editEndGrid.SetWindowText( NULL );
		m_editEndGrid.EnableWindow( FALSE );
		m_editLengthGrid.SetWindowText( NULL );
		m_editLengthGrid.EnableWindow( FALSE );
	}
	else
	{
		SetEditControl( m_editStartGrid, m_lStartGrid, 1 );
		SetEditControl( m_editEndGrid, m_lEndGrid, 1 );
		SetEditControl( m_editLengthGrid, m_lLengthGrid, 0 );
	}
	
	// Update SOURCE controls
	if( m_PropItem.m_Item.m_WaveInfo.fIsLooped )
	{
		// Set GRID controls
		if( pWaveStrip
		&&  pWaveStrip->IsOneGrid() )
		{
			m_staticSourceStartGrid.SetWindowText( NULL );
			m_staticSourceEndGrid.SetWindowText( NULL );
			m_staticSourceLengthGrid.SetWindowText( NULL );
		}
		else
		{
			SetStaticControl( m_staticSourceStartGrid, m_lSourceStartGrid, 1 );
			SetStaticControl( m_staticSourceEndGrid, m_lSourceEndGrid, 1 );
			SetStaticControl( m_staticSourceLengthGrid, m_lSourceLengthGrid, 0 );
		}

		// Update START controls
		SetStaticControl( m_staticSourceStartMeasure, m_lSourceStartMeasure, nMeasureBeatOffset );
		SetStaticControl( m_staticSourceStartBeat, m_lSourceStartBeat, nMeasureBeatOffset );
		SetStaticControl( m_staticSourceStartTick, m_lSourceStartTick, 0 );
		SetStaticControl( m_staticSourceStartSample, m_PropItem.m_Item.m_WaveInfo.dwLoopStart, 0 );

		// Update END controls
		SetStaticControl( m_staticSourceEndMeasure, m_lSourceEndMeasure, nMeasureBeatOffset );
		SetStaticControl( m_staticSourceEndBeat, m_lSourceEndBeat, nMeasureBeatOffset );
		SetStaticControl( m_staticSourceEndTick, m_lSourceEndTick, 0 );
		SetStaticControl( m_staticSourceEndSample, m_PropItem.m_Item.m_WaveInfo.dwLoopEnd, 0 );

		// Update LENGTH controls
		SetStaticControl( m_staticSourceLengthMeasure, m_lSourceLengthMeasure, 0 );
		SetStaticControl( m_staticSourceLengthBeat, m_lSourceLengthBeat, 0 );
		SetStaticControl( m_staticSourceLengthTick, m_lSourceLengthTick, 0 );
		SetStaticControl( m_staticSourceLengthNbrSamples, ((m_PropItem.m_Item.m_WaveInfo.dwLoopEnd - m_PropItem.m_Item.m_WaveInfo.dwLoopStart) + 1), 0 );
	}
	else
	{
		// Update START controls
		m_staticSourceStartMeasure.SetWindowText( NULL );
		m_staticSourceStartBeat.SetWindowText( NULL );
		m_staticSourceStartGrid.SetWindowText( NULL );
		m_staticSourceStartTick.SetWindowText( NULL );
		m_staticSourceStartSample.SetWindowText( NULL );

		// Update END controls
		m_staticSourceEndMeasure.SetWindowText( NULL );
		m_staticSourceEndBeat.SetWindowText( NULL );
		m_staticSourceEndGrid.SetWindowText( NULL );
		m_staticSourceEndTick.SetWindowText( NULL );
		m_staticSourceEndSample.SetWindowText( NULL );

		// Update LENGTH controls
		m_staticSourceLengthMeasure.SetWindowText( NULL );
		m_staticSourceLengthBeat.SetWindowText( NULL );
		m_staticSourceLengthGrid.SetWindowText( NULL );
		m_staticSourceLengthTick.SetWindowText( NULL );
		m_staticSourceLengthNbrSamples.SetWindowText( NULL );
	}

	// Update START controls
	SetEditControl( m_editStartMeasure, m_lStartMeasure, nMeasureBeatOffset );
	SetEditControl( m_editStartBeat, m_lStartBeat, nMeasureBeatOffset );
	SetEditControl( m_editStartTick, m_lStartTick, 0 );
	SetEditControl( m_editStartSample, m_PropItem.m_Item.m_dwLoopStartUI, 0 );
	m_spinStartSample.SetPos( UD_MAXVAL >> 1 );

	// Update END controls
	SetEditControl( m_editEndMeasure, m_lEndMeasure, nMeasureBeatOffset );
	SetEditControl( m_editEndBeat, m_lEndBeat, nMeasureBeatOffset );
	SetEditControl( m_editEndTick, m_lEndTick, 0 );
	SetEditControl( m_editEndSample, m_PropItem.m_Item.m_dwLoopEndUI, 0 );
	m_spinEndSample.SetPos( UD_MAXVAL >> 1 );

	// Update LENGTH controls
	SetEditControl( m_editLengthMeasure, m_lLengthMeasure, 0 );
	SetEditControl( m_editLengthBeat, m_lLengthBeat, 0 );
	SetEditControl( m_editLengthTick, m_lLengthTick, 0 );
	SetEditControl( m_editLengthNbrSamples, m_dwLengthNbrSamples, 0 );
	m_spinLengthNbrSamples.SetPos( UD_MAXVAL >> 1 );

	// Update LOCK LENGTH controls
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pTrackMgr )
	{
		CBitmap* pBitmap = m_pPropPageMgr->m_pTrackMgr->GetLockBitmap( m_PropItem.m_Item.m_fLockLoopLengthUI );
		if( pBitmap )
		{
			m_btnLockLength.SetBitmap( (HBITMAP)pBitmap->GetSafeHandle() );
		}
	}
	if( m_PropItem.m_Item.m_fLockLoopLengthUI )
	{
		m_editLengthMeasure.EnableWindow( FALSE );
		m_spinLengthMeasure.EnableWindow( FALSE );
		m_editLengthBeat.EnableWindow( FALSE );
		m_spinLengthBeat.EnableWindow( FALSE );
		m_editLengthGrid.EnableWindow( FALSE );
		m_spinLengthGrid.EnableWindow( FALSE );
		m_editLengthTick.EnableWindow( FALSE );
		m_spinLengthTick.EnableWindow( FALSE );
		m_editLengthNbrSamples.EnableWindow( FALSE );
	}

	// Set ranges of spin controls
	SetControlRanges();
}
	

/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SetEditControl

void CTabLoop::SetEditControl( CEdit& edit, long lValue, int nOffset )
{
	TCHAR tcstrTmp[DIALOG_EDIT_LEN];
	BOOL fTransSuccess;

	edit.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

	// Convert from text to an integer
	long lCurValue = GetDlgItemInt( edit.GetDlgCtrlID(), &fTransSuccess, TRUE );

	// Check if the conversion failed, the text is empty, or if the values are different
	if( !fTransSuccess
	|| (tcstrTmp[0] == NULL)
	|| (lCurValue != lValue + nOffset) )
	{
		// Update the displayed number
		SetDlgItemInt( edit.GetDlgCtrlID(), lValue + nOffset );
	}
}
	

/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SetStaticControl

void CTabLoop::SetStaticControl( CStatic& stat, long lValue, int nOffset )
{
	// Update the displayed number
	SetDlgItemInt( stat.GetDlgCtrlID(), lValue + nOffset );
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SetLoopStart

void CTabLoop::SetLoopStart( DWORD dwNewLoopStart ) 
{
	dwNewLoopStart = max( dwNewLoopStart, 0 );
	dwNewLoopStart = min( dwNewLoopStart, m_PropItem.m_Item.m_WaveInfo.dwWaveEnd - MIN_LOOP_LENGTH );
	
	DWORD dwNewLoopEnd;
	if( m_PropItem.m_Item.m_fLockLoopLengthUI )
	{
		dwNewLoopEnd = dwNewLoopStart + (m_dwLengthNbrSamples - 1);
	}
	else
	{
		dwNewLoopEnd = m_PropItem.m_Item.m_dwLoopEndUI;
	}
	if( (dwNewLoopEnd < dwNewLoopStart)
	||  (dwNewLoopEnd - dwNewLoopStart) < MIN_LOOP_LENGTH )
	{
		dwNewLoopEnd = dwNewLoopStart + MIN_LOOP_LENGTH;
	}
	if( (dwNewLoopEnd - dwNewLoopStart) > MAX_LOOP_LENGTH )
	{
		dwNewLoopEnd = dwNewLoopStart + MAX_LOOP_LENGTH;
	}

	EnforceLoopBoundaries( dwNewLoopStart, dwNewLoopEnd );

	if( (m_PropItem.m_dwUndetermined_TabLoop & UNDT_LOOP_START)
	||  (m_PropItem.m_dwUndetermined_TabLoop & UNDT_LOOP_END)
	||  (m_PropItem.m_Item.m_dwLoopStartUI != dwNewLoopStart)
	||  (m_PropItem.m_Item.m_dwLoopEndUI != dwNewLoopEnd) )
	{
		m_PropItem.m_Item.m_dwLoopStartUI = dwNewLoopStart;
		m_PropItem.m_Item.m_dwLoopEndUI = dwNewLoopEnd;

		// Now, update the object with the new value(s)
		m_PropItem.m_dwChanged = CHGD_LOOP;
		UpdateObject();
	}
	else
	{
		// Display original values
		SetPropTrackItem( &m_PropItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SetLoopEnd

void CTabLoop::SetLoopEnd( DWORD dwNewLoopEnd ) 
{
	dwNewLoopEnd = max( dwNewLoopEnd, MIN_LOOP_LENGTH );
	dwNewLoopEnd = min( dwNewLoopEnd, m_PropItem.m_Item.m_WaveInfo.dwWaveEnd );
	
	DWORD dwNewLoopStart;
	if( m_PropItem.m_Item.m_fLockLoopLengthUI )
	{
		if( m_PropItem.m_Item.m_dwLoopStartUI > (dwNewLoopEnd - (m_dwLengthNbrSamples - 1)) )
		{
			dwNewLoopStart = dwNewLoopEnd - (m_dwLengthNbrSamples - 1);
		}
		else
		{
			dwNewLoopStart = 0;
			dwNewLoopEnd = m_PropItem.m_Item.m_dwLoopEndUI - m_PropItem.m_Item.m_dwLoopStartUI;
		}
	}
	else
	{
		dwNewLoopStart = m_PropItem.m_Item.m_dwLoopStartUI;
	}
	if( (dwNewLoopEnd < dwNewLoopStart)
	||  (dwNewLoopEnd - dwNewLoopStart) < MIN_LOOP_LENGTH )
	{
		dwNewLoopStart = dwNewLoopEnd - MIN_LOOP_LENGTH;
	}
	if( (dwNewLoopEnd - dwNewLoopStart) > MAX_LOOP_LENGTH )
	{
		dwNewLoopEnd = dwNewLoopStart + MAX_LOOP_LENGTH;
	}

	EnforceLoopBoundaries( dwNewLoopStart, dwNewLoopEnd );

	if( (m_PropItem.m_dwUndetermined_TabLoop & UNDT_LOOP_START)
	||  (m_PropItem.m_dwUndetermined_TabLoop & UNDT_LOOP_END)
	||  (m_PropItem.m_Item.m_dwLoopStartUI != dwNewLoopStart)
	||  (m_PropItem.m_Item.m_dwLoopEndUI != dwNewLoopEnd) )
	{
		m_PropItem.m_Item.m_dwLoopStartUI = dwNewLoopStart;
		m_PropItem.m_Item.m_dwLoopEndUI = dwNewLoopEnd;

		// Now, update the object with the new value(s)
		m_PropItem.m_dwChanged = CHGD_LOOP;
		UpdateObject();
	}
	else
	{
		// Display original values
		SetPropTrackItem( &m_PropItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SetLoopLength

void CTabLoop::SetLoopLength( DWORD dwNewLoopLength ) 
{
	ASSERT( m_PropItem.m_Item.m_fLockLoopLengthUI == FALSE );

	DWORD dwMaxLoopLength = min( (m_PropItem.m_Item.m_WaveInfo.dwWaveEnd + 1), (MAX_LOOP_LENGTH + 1) );

	dwNewLoopLength = max( dwNewLoopLength, (MIN_LOOP_LENGTH + 1) );
	dwNewLoopLength = min( dwNewLoopLength, dwMaxLoopLength );

	DWORD dwNewLoopStart = m_PropItem.m_Item.m_dwLoopStartUI;
	DWORD dwNewLoopEnd = (m_PropItem.m_Item.m_dwLoopStartUI + dwNewLoopLength) - 1;

	EnforceLoopBoundaries( dwNewLoopStart, dwNewLoopEnd );

	if( (m_PropItem.m_dwUndetermined_TabLoop & UNDT_LOOP_START)
	||  (m_PropItem.m_dwUndetermined_TabLoop & UNDT_LOOP_END)
	||  (m_PropItem.m_Item.m_dwLoopStartUI != dwNewLoopStart)
	||  (m_PropItem.m_Item.m_dwLoopEndUI != dwNewLoopEnd) )
	{
		m_PropItem.m_Item.m_dwLoopStartUI = dwNewLoopStart;
		m_PropItem.m_Item.m_dwLoopEndUI = dwNewLoopEnd;

		// Now, update the object with the new value(s)
		m_PropItem.m_dwChanged = CHGD_LOOP;
		UpdateObject();
	}
	else
	{
		// Display original values
		SetPropTrackItem( &m_PropItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::EnforceLoopBoundaries

void CTabLoop::EnforceLoopBoundaries( DWORD& dwNewLoopStart, DWORD& dwNewLoopEnd ) 
{
	int nShift;

	if( dwNewLoopStart < 0 )
	{
		nShift = 0 - dwNewLoopStart;
		dwNewLoopEnd += nShift;
		dwNewLoopStart += nShift;
	}

	if( dwNewLoopEnd > m_PropItem.m_Item.m_WaveInfo.dwWaveEnd )
	{
		nShift = dwNewLoopEnd - m_PropItem.m_Item.m_WaveInfo.dwWaveEnd;
		dwNewLoopStart -= nShift;
		dwNewLoopEnd -= nShift;
	}

	ASSERT( dwNewLoopStart >= 0 );
	ASSERT( dwNewLoopEnd <= m_PropItem.m_Item.m_WaveInfo.dwWaveEnd );
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::StartTimeToSample

DWORD CTabLoop::StartTimeToSample( void )
{
	ASSERT( m_pIWaveTimelineDraw != NULL );

	// Get the reference time
	REFERENCE_TIME rtTime;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		m_pPropPageMgr->m_pTrackMgr->MinSecGridMsToRefTime( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															m_lStartMeasure, m_lStartBeat, m_lStartGrid, m_lStartTick,
															&rtTime );
	}
	else
	{
		MUSIC_TIME mtTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lStartMeasure, m_lStartBeat, m_lStartGrid, m_lStartTick, &mtTime );
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtTime, &rtTime  );
	}

	// Subtract out start time
	REFERENCE_TIME rtTimePhysical;
	m_pPropPageMgr->m_pTrackMgr->UnknownTimeToRefTime( m_PropItem.m_Item.m_rtTimePhysical, &rtTimePhysical );
	rtTime -= rtTimePhysical;

	// Convert reference time to sample
	DWORD dwNewLoopStart;
	m_pIWaveTimelineDraw->RefTimeToSample( max(0, rtTime), &dwNewLoopStart, m_PropItem.m_Item.m_lPitch );

	return dwNewLoopStart;
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::EndTimeToSample

DWORD CTabLoop::EndTimeToSample( void )
{
	ASSERT( m_pIWaveTimelineDraw != NULL );

	// Get the reference time
	REFERENCE_TIME rtTime;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		m_pPropPageMgr->m_pTrackMgr->MinSecGridMsToRefTime( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															m_lEndMeasure, m_lEndBeat, m_lEndGrid, m_lEndTick,
															&rtTime );
	}
	else
	{
		MUSIC_TIME mtTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lEndMeasure, m_lEndBeat, m_lEndGrid, m_lEndTick, &mtTime );
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtTime, &rtTime  );
	}

	// Subtract out start time
	REFERENCE_TIME rtTimePhysical;
	m_pPropPageMgr->m_pTrackMgr->UnknownTimeToRefTime( m_PropItem.m_Item.m_rtTimePhysical, &rtTimePhysical );
	rtTime -= rtTimePhysical;

	// Convert reference time to sample
	DWORD dwNewLoopEnd;
	m_pIWaveTimelineDraw->RefTimeToSample( max(0, rtTime), &dwNewLoopEnd, m_PropItem.m_Item.m_lPitch );

	return dwNewLoopEnd;
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::LengthTimeToNbrSamples

DWORD CTabLoop::LengthTimeToNbrSamples( void )
{
	ASSERT( m_pIWaveTimelineDraw != NULL );

	// Get the reference time
	REFERENCE_TIME rtTime;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		m_pPropPageMgr->m_pTrackMgr->MinSecGridMsToRefTime( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															m_lLengthMeasure, m_lLengthBeat, m_lLengthGrid, m_lLengthTick,
															&rtTime );
	}
	else
	{
		REFERENCE_TIME rtTimePhysical;
		MUSIC_TIME mtTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lLengthMeasure, m_lLengthBeat, m_lLengthGrid, m_lLengthTick, &mtTime );
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->ClocksToRefTime( (MUSIC_TIME)m_PropItem.m_Item.m_rtTimePhysical, &rtTimePhysical  );
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->ClocksToRefTime( ((MUSIC_TIME)m_PropItem.m_Item.m_rtTimePhysical + mtTime), &rtTime  );
		rtTime -= rtTimePhysical;
	}

	// Convert reference time to sample
	DWORD dwNewLoopLength;
	m_pIWaveTimelineDraw->RefTimeToSample( max(0, rtTime), &dwNewLoopLength, m_PropItem.m_Item.m_lPitch );

	return dwNewLoopLength;
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::RecomputeLoopTimes

void CTabLoop::RecomputeLoopTimes( void )
{
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		SampleToRefTime();
	}
	else
	{
		SampleToMusicTime();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SampleToRefTime

void CTabLoop::SampleToRefTime( void )
{
	ASSERT( m_pIWaveTimelineDraw != NULL );

	REFERENCE_TIME rtTimePhysical;
	m_pPropPageMgr->m_pTrackMgr->UnknownTimeToRefTime( m_PropItem.m_Item.m_rtTimePhysical, &rtTimePhysical );

	REFERENCE_TIME rtTime;

	// START
	m_pIWaveTimelineDraw->SampleToRefTime( m_PropItem.m_Item.m_dwLoopStartUI, &rtTime, m_PropItem.m_Item.m_lPitch );
	rtTime += rtTimePhysical;
	m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
														rtTime,
														&m_lStartMeasure, &m_lStartBeat, &m_lStartGrid, &m_lStartTick );
	

	// END
	m_pIWaveTimelineDraw->SampleToRefTime( m_PropItem.m_Item.m_dwLoopEndUI, &rtTime, m_PropItem.m_Item.m_lPitch );
	rtTime += rtTimePhysical;
	m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
														rtTime,
														&m_lEndMeasure, &m_lEndBeat, &m_lEndGrid, &m_lEndTick );
	

	// LENGTH
	m_pIWaveTimelineDraw->SampleToRefTime( m_dwLengthNbrSamples, &rtTime, m_PropItem.m_Item.m_lPitch );
	m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
														rtTime,
														&m_lLengthMeasure, &m_lLengthBeat, &m_lLengthGrid, &m_lLengthTick );
	

	// SOURCE START
	m_pIWaveTimelineDraw->SampleToRefTime( m_PropItem.m_Item.m_WaveInfo.dwLoopStart, &rtTime, m_PropItem.m_Item.m_lPitch );
	rtTime += rtTimePhysical;
	m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
														rtTime,
														&m_lSourceStartMeasure, &m_lSourceStartBeat, &m_lSourceStartGrid, &m_lSourceStartTick );
	

	// SOURCE END
	m_pIWaveTimelineDraw->SampleToRefTime( m_PropItem.m_Item.m_WaveInfo.dwLoopEnd, &rtTime, m_PropItem.m_Item.m_lPitch );
	rtTime += rtTimePhysical;
	m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
														rtTime,
														&m_lSourceEndMeasure, &m_lSourceEndBeat, &m_lSourceEndGrid, &m_lSourceEndTick );
	

	// SOURCE LENGTH
	m_pIWaveTimelineDraw->SampleToRefTime( ((m_PropItem.m_Item.m_WaveInfo.dwLoopEnd - m_PropItem.m_Item.m_WaveInfo.dwLoopStart) + 1), &rtTime, m_PropItem.m_Item.m_lPitch );
	m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
														rtTime,
														&m_lSourceLengthMeasure, &m_lSourceLengthBeat, &m_lSourceLengthGrid, &m_lSourceLengthTick );
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SampleToMusicTime

void CTabLoop::SampleToMusicTime( void )
{
	ASSERT( m_pIWaveTimelineDraw != NULL );

//	MUSIC_TIME mtTimePhysical;
//	m_pPropPageMgr->m_pTrackMgr->UnknownTimeToClocks( m_PropItem.m_Item.m_rtTimePhysical, &mtTimePhysical );

	REFERENCE_TIME rtTimePhysical;
	m_pPropPageMgr->m_pTrackMgr->UnknownTimeToRefTime( m_PropItem.m_Item.m_rtTimePhysical, &rtTimePhysical );

	REFERENCE_TIME rtTime;
//	MUSIC_TIME mtTime;
	MUSIC_TIME mtStart;
	MUSIC_TIME mtEnd;

	// START
	m_pIWaveTimelineDraw->SampleToRefTime( m_PropItem.m_Item.m_dwLoopStartUI, &rtTime, m_PropItem.m_Item.m_lPitch );
	m_pPropPageMgr->m_pTrackMgr->m_pTimeline->RefTimeToClocks( (rtTimePhysical + rtTime), &mtStart );
	m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( mtStart, &m_lStartMeasure, &m_lStartBeat, &m_lStartGrid, &m_lStartTick );

	// END
	m_pIWaveTimelineDraw->SampleToRefTime( m_PropItem.m_Item.m_dwLoopEndUI, &rtTime, m_PropItem.m_Item.m_lPitch );
	m_pPropPageMgr->m_pTrackMgr->m_pTimeline->RefTimeToClocks( (rtTimePhysical + rtTime), &mtEnd );
	m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( mtEnd, &m_lEndMeasure, &m_lEndBeat, &m_lEndGrid, &m_lEndTick );

	// LENGTH
	m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( (mtEnd - mtStart), &m_lLengthMeasure, &m_lLengthBeat, &m_lLengthGrid, &m_lLengthTick );

	// SOURCE START
	m_pIWaveTimelineDraw->SampleToRefTime( m_PropItem.m_Item.m_WaveInfo.dwLoopStart, &rtTime, m_PropItem.m_Item.m_lPitch );
	m_pPropPageMgr->m_pTrackMgr->m_pTimeline->RefTimeToClocks( (rtTimePhysical + rtTime), &mtStart );
	m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( mtStart, &m_lSourceStartMeasure, &m_lSourceStartBeat, &m_lSourceStartGrid, &m_lSourceStartTick );

	// SOURCE END
	m_pIWaveTimelineDraw->SampleToRefTime( m_PropItem.m_Item.m_WaveInfo.dwLoopEnd, &rtTime, m_PropItem.m_Item.m_lPitch );
	m_pPropPageMgr->m_pTrackMgr->m_pTimeline->RefTimeToClocks( (rtTimePhysical + rtTime), &mtEnd );
	m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( mtEnd, &m_lSourceEndMeasure, &m_lSourceEndBeat, &m_lSourceEndGrid, &m_lSourceEndTick );

	// SOURCE LENGTH
	m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( (mtEnd - mtStart), &m_lSourceLengthMeasure, &m_lSourceLengthBeat, &m_lSourceLengthGrid, &m_lSourceLengthTick );
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SetControlRanges

void CTabLoop::SetControlRanges( void )
{
	// Get track group bits
	DWORD dwGroupBits = 0xFFFFFFFF;
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pIPropPageObject )
	{
		// Query the Property Page Object for an IDMUSProdStripMgr interface
		IDMUSProdStripMgr *pStripMgr;
		if( SUCCEEDED( m_pPropPageMgr->m_pIPropPageObject->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgr ) ) )
		{
			// Ask the strip manager for an IDMUSProdTimeline pointer
			VARIANT variant;
			if( SUCCEEDED( pStripMgr->GetStripMgrProperty( SMP_ITIMELINECTL, &variant ) ) )
			{
				// Query the returned IUnknown pointer for an IDMUSProdTimeline Poiter
				if( SUCCEEDED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pTimeline ) ) )
				{
					// Successfully got an IDMUSProdTimeline pointer.
					// Now, release it since we only want a weak reference
					m_pTimeline->Release();
				}

				// Release the returned IUnknown pointer
				V_UNKNOWN( &variant )->Release();
			}

			// Ask the strip manager for its track header, so we can read the group bits from it
			DMUS_IO_TRACK_HEADER ioTrackHeader;
			variant.vt = VT_BYREF;
			V_BYREF( &variant ) = &ioTrackHeader;
			if( SUCCEEDED( pStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &variant ) ) )
			{
				dwGroupBits = ioTrackHeader.dwGroup;
			}

			// Release the returned IDMUSProdStripMgr pointer
			pStripMgr->Release();
		}
	}

	// Set ranges for "time" controls
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		SetRefTimeRanges();
	}
	else
	{
		SetMusicTimeRanges( dwGroupBits );
	}

	// Limit samples 
	// Just using deltas from these controls
	// Boundary checks will happen in delta change handler
	m_spinStartSample.SetRange( 0, UD_MAXVAL );
	m_editStartSample.LimitText( 10 );
	m_spinEndSample.SetRange( 0, UD_MAXVAL );
	m_editEndSample.LimitText( 10 );
	m_spinLengthNbrSamples.SetRange( 0, UD_MAXVAL );
	m_editLengthNbrSamples.LimitText( 10 );
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SetRefTimeRanges

void CTabLoop::SetRefTimeRanges( void )
{
	// Limit the minute number to positive numbers
	m_spinStartMeasure.SetRange( 1, 32767 );
	m_editStartMeasure.LimitText( 5 );
	m_spinEndMeasure.SetRange( 1, 32767 );
	m_editEndMeasure.LimitText( 5 );
	m_spinLengthMeasure.SetRange( 0, 32767 );
	m_editLengthMeasure.LimitText( 5 );

	// Limit the second number to 1-60
	m_spinStartBeat.SetRange( 1, 60 );
	m_editStartBeat.LimitText( 2 );
	m_spinEndBeat.SetRange( 1, 60 );
	m_editEndBeat.LimitText( 2 );
	m_spinLengthBeat.SetRange( 0, 60 );
	m_editLengthBeat.LimitText( 2 );

	// Limit the grid number to MIN_NBR_GRIDS_PER_SECOND - MAX_NBR_GRIDS_PER_SECOND
	m_spinStartGrid.SetRange( MIN_NBR_GRIDS_PER_SECOND, MAX_NBR_GRIDS_PER_SECOND );
	m_editStartGrid.LimitText( 3 );
	m_spinEndGrid.SetRange( MIN_NBR_GRIDS_PER_SECOND, MAX_NBR_GRIDS_PER_SECOND );
	m_editEndGrid.LimitText( 3 );
	m_spinLengthGrid.SetRange( (MIN_NBR_GRIDS_PER_SECOND - 1), MAX_NBR_GRIDS_PER_SECOND );
	m_editLengthGrid.LimitText( 3 );

	// Limit the millisecond number to 1-1000
	m_spinStartTick.SetRange( 1, 1000 );
	m_editStartTick.LimitText( 4 );
	m_spinEndTick.SetRange( 1, 1000 );
	m_editEndTick.LimitText( 4 );
	m_spinLengthTick.SetRange( 1, 1000 );
	m_editLengthTick.LimitText( 4 );
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::SetMusicTimeRanges

void CTabLoop::SetMusicTimeRanges( DWORD dwGroupBits )
{
	long lMaxMeasure, lMaxBeat, lMaxGrid;

	// Get the maximum measure value
	if( m_pTimeline == NULL )
	{
		// No timeline pointer - use default values
		lMaxMeasure = 32767;
		lMaxBeat = 256;
		lMaxGrid = 256;
	}
	else
	{
		lMaxGrid = 256;	// Fix this!!!

		// Get the length of the timeline, in clocks
		VARIANT var;
		m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );

		// Convert from clocks to a measure and beat value
		m_pTimeline->ClocksToMeasureBeat( dwGroupBits, 0,
										  V_I4( &var ), &lMaxMeasure, &lMaxBeat );

		// If the beat value is zero, check if the segment is exactly lMaxMeasure in length
		if( lMaxBeat == 0 )
		{
			// Convert from lMaxMeasure to a clock value
			long lClockLength;
			m_pTimeline->MeasureBeatToClocks( dwGroupBits, 0, lMaxMeasure, 0, &lClockLength );

			// Check if this clock value is equal to the length
			if( lClockLength == V_I4( &var ) )
			{
				// Exactly lMaxMeasures long.  Get the measure and beat value for the previous tick
				m_pTimeline->ClocksToMeasureBeat( dwGroupBits, 0,
												  V_I4( &var ) - 1, &lMaxMeasure, &lMaxBeat );
			}
		}

		long lItemMeasure, lItemBeat, lItemGrid, lItemTick;
		m_pPropPageMgr->m_pTrackMgr->UnknownTimeToMeasureBeatGridTick( m_PropItem.m_Item.m_rtTimePhysical,
												&lItemMeasure, &lItemBeat, &lItemGrid, &lItemTick );

		// If the item is not in the last measure
		if( lItemMeasure < lMaxMeasure )
		{
			// Convert from a Measure value to a clock value
			long lClockForMeasure;
			m_pTimeline->MeasureBeatToClocks( dwGroupBits, 0, lItemMeasure, 0, &lClockForMeasure );

			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE TimeSig;
			if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, lClockForMeasure, NULL, &TimeSig ) ) )
			{
				lMaxBeat = TimeSig.bBeatsPerMeasure - 1;
			}
		}
		// Else the item is in the last measure and lMaxBeat is already set

		// Now convert the measure and beat from 0-based to 1-based
		lMaxMeasure++;
		lMaxBeat++;
	}

	// Update the range for the measure spin control
	m_spinStartMeasure.SetRange( 1, lMaxMeasure );
	m_editStartMeasure.LimitText( 5 );
	m_spinEndMeasure.SetRange( 1, 32767 );
	m_editEndMeasure.LimitText( 5 );
	m_spinLengthMeasure.SetRange( 0, 32767 );
	m_editLengthMeasure.LimitText( 5 );

	// Update the range for the beat spin control
	m_spinStartBeat.SetRange( 1, lMaxBeat );
	m_editStartBeat.LimitText( 3 );
	m_spinEndBeat.SetRange( 1, lMaxBeat );
	m_editEndBeat.LimitText( 3 );
	m_spinLengthBeat.SetRange( 0, lMaxBeat );
	m_editLengthBeat.LimitText( 3 );

	// Update the range for the grid spin control
	m_spinStartGrid.SetRange( 1, lMaxGrid );
	m_editStartGrid.LimitText( 3 );
	m_spinEndGrid.SetRange( 1, lMaxGrid );
	m_editEndGrid.LimitText( 3 );
	m_spinLengthGrid.SetRange( 0, lMaxGrid );
	m_editLengthGrid.LimitText( 3 );

	// Update the range for the tick spin control
	m_spinStartTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editStartTick.LimitText( 5 );
	m_spinEndTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editEndTick.LimitText( 5 );
	m_spinLengthTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editLengthTick.LimitText( 5 );
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnCreate

int CTabLoop::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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

	// Call the CPropertyPage's OnCreate method to actually create all our controls from our dialog resource.
	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnDestroy

void CTabLoop::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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

	// Call the CPropertyPage's OnDestroy method to actually destroy all our child controls.
	CPropertyPage::OnDestroy();	
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnInitDialog

BOOL CTabLoop::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Call the base class's OnInitDialog method
	CPropertyPage::OnInitDialog();

	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pTrackMgr )
	{
		CBitmap* pBitmap = m_pPropPageMgr->m_pTrackMgr->GetLockBitmap( 0 );
		if( pBitmap )
		{
			m_btnLockLength.SetBitmap( (HBITMAP)pBitmap->GetSafeHandle() );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinStartTick

void CTabLoop::OnSpinStartTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartTick ) )
	{
		SetLoopStart( StartTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinStartGrid

void CTabLoop::OnSpinStartGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartGrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartGrid ) )
	{
		SetLoopStart( StartTimeToSample() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinStartBeat

void CTabLoop::OnSpinStartBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartBeat ) )
	{
		SetLoopStart( StartTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinStartMeasure

void CTabLoop::OnSpinStartMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartMeasure ) )
	{
		SetLoopStart( StartTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinStartSample

void CTabLoop::OnSpinStartSample(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartSample();

	DWORD dwNewLoopStart = m_PropItem.m_Item.m_dwLoopStartUI;
	if( HandleDeltaChange( pNMHDR, pResult, dwNewLoopStart ) )
	{
		SetLoopStart( dwNewLoopStart );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinEndTick

void CTabLoop::OnSpinEndTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndTick ) )
	{
		SetLoopEnd( EndTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinEndGrid

void CTabLoop::OnSpinEndGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndGrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndGrid ) )
	{
		SetLoopEnd( EndTimeToSample() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinEndBeat

void CTabLoop::OnSpinEndBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndBeat ) )
	{
		SetLoopEnd( EndTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinEndMeasure

void CTabLoop::OnSpinEndMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndMeasure ) )
	{
		SetLoopEnd( EndTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinEndSample

void CTabLoop::OnSpinEndSample(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndSample();

	DWORD dwNewLoopEnd = m_PropItem.m_Item.m_dwLoopEndUI;
	if( HandleDeltaChange( pNMHDR, pResult, dwNewLoopEnd ) )
	{
		SetLoopEnd( dwNewLoopEnd );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinLengthTick

void CTabLoop::OnSpinLengthTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLengthTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lLengthTick ) )
	{
		SetLoopLength( LengthTimeToNbrSamples() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinLengthGrid

void CTabLoop::OnSpinLengthGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLengthGrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_lLengthGrid ) )
	{
		SetLoopLength( LengthTimeToNbrSamples() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinLengthBeat

void CTabLoop::OnSpinLengthBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLengthBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_lLengthBeat ) )
	{
		SetLoopLength( LengthTimeToNbrSamples() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinLengthMeasure

void CTabLoop::OnSpinLengthMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLengthMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_lLengthMeasure ) )
	{
		SetLoopLength( LengthTimeToNbrSamples() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSpinLengthNbrSamples

void CTabLoop::OnSpinLengthNbrSamples(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLengthNbrSamples();

	DWORD dwNewLoopLength = m_dwLengthNbrSamples;
	if( HandleDeltaChange( pNMHDR, pResult, dwNewLoopLength ) )
	{
		SetLoopLength( dwNewLoopLength );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::HandleDeltaChange
//
// Generic handler for deltapos changes
bool CTabLoop::HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return false;
	}

	bool fChanged = false;

	// If the value changed - update the selected item
	if( ((NM_UPDOWN* )pNMHDR)->iDelta != 0 ) 
	{
		// Update the value
		lUpdateVal += ((NM_UPDOWN* )pNMHDR)->iDelta;
		fChanged = true;
	}

	// Set the result to 1 to show that we handled this message
	*pResult = 1;

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::HandleDeltaChange
//
// Generic handler for deltapos changes
bool CTabLoop::HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, DWORD& dwUpdateVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return false;
	}

	bool fChanged = false;

	// If the value changed - update the selected item
	if( ((NM_UPDOWN* )pNMHDR)->iDelta != 0 ) 
	{
		// Update the value
		dwUpdateVal += ((NM_UPDOWN* )pNMHDR)->iDelta;
		fChanged = true;
	}

	// Set the result to 1 to show that we handled this message
	*pResult = 1;

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusStartTick

void CTabLoop::OnKillFocusStartTick() 
{
	if( HandleKillFocus( m_spinStartTick, m_lStartTick, 0 ) )
	{
		SetLoopStart( StartTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusStartGrid

void CTabLoop::OnKillFocusStartGrid() 
{
	if( HandleKillFocus( m_spinStartGrid, m_lStartGrid, 1 ) )
	{
		SetLoopStart( StartTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusStartBeat

void CTabLoop::OnKillFocusStartBeat() 
{
	int nOffset;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		nOffset = 0;
	}
	else
	{
		nOffset = 1;
	}

	if( HandleKillFocus( m_spinStartBeat, m_lStartBeat, nOffset ) )
	{
		SetLoopStart( StartTimeToSample() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusStartMeasure

void CTabLoop::OnKillFocusStartMeasure() 
{
	int nOffset;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		nOffset = 0;
	}
	else
	{
		nOffset = 1;
	}

	if( HandleKillFocus( m_spinStartMeasure, m_lStartMeasure, nOffset ) )
	{
		SetLoopStart( StartTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusStartSample

void CTabLoop::OnKillFocusStartSample() 
{
	DWORD dwNewLoopStart = m_PropItem.m_Item.m_dwLoopStartUI;

	if( HandleKillFocus( m_editStartSample, dwNewLoopStart, 0 ) )
	{
		SetLoopStart( dwNewLoopStart );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusEndTick

void CTabLoop::OnKillFocusEndTick() 
{
	if( HandleKillFocus( m_spinEndTick, m_lEndTick, 0 ) )
	{
		SetLoopEnd( EndTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusEndGrid

void CTabLoop::OnKillFocusEndGrid() 
{
	if( HandleKillFocus( m_spinEndGrid, m_lEndGrid, 1 ) )
	{
		SetLoopEnd( EndTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusEndBeat

void CTabLoop::OnKillFocusEndBeat() 
{
	int nOffset;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		nOffset = 0;
	}
	else
	{
		nOffset = 1;
	}

	if( HandleKillFocus( m_spinEndBeat, m_lEndBeat, nOffset ) )
	{
		SetLoopEnd( EndTimeToSample() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusEndMeasure

void CTabLoop::OnKillFocusEndMeasure() 
{
	int nOffset;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		nOffset = 0;
	}
	else
	{
		nOffset = 1;
	}

	if( HandleKillFocus( m_spinEndMeasure, m_lEndMeasure, nOffset ) )
	{
		SetLoopEnd( EndTimeToSample() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusEndSample

void CTabLoop::OnKillFocusEndSample() 
{
	DWORD dwNewLoopEnd = m_PropItem.m_Item.m_dwLoopEndUI;

	if( HandleKillFocus( m_editEndSample, dwNewLoopEnd, 0 ) )
	{
		SetLoopEnd( dwNewLoopEnd );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusLengthTick

void CTabLoop::OnKillFocusLengthTick() 
{
	if( HandleKillFocus( m_spinLengthTick, m_lLengthTick, 0 ) )
	{
		SetLoopLength( LengthTimeToNbrSamples() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusLengthGrid

void CTabLoop::OnKillFocusLengthGrid() 
{
	if( HandleKillFocus( m_spinLengthGrid, m_lLengthGrid, 0 ) )
	{
		SetLoopLength( LengthTimeToNbrSamples() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusLengthBeat

void CTabLoop::OnKillFocusLengthBeat() 
{
	if( HandleKillFocus( m_spinLengthBeat, m_lLengthBeat, 0 ) )
	{
		SetLoopLength( LengthTimeToNbrSamples() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusLengthMeasure

void CTabLoop::OnKillFocusLengthMeasure() 
{
	if( HandleKillFocus( m_spinLengthMeasure, m_lLengthMeasure, 0 ) )
	{
		SetLoopLength( LengthTimeToNbrSamples() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnKillFocusLengthNbrSamples

void CTabLoop::OnKillFocusLengthNbrSamples() 
{
	DWORD dwNewLoopLength = m_dwLengthNbrSamples;

	if( HandleKillFocus( m_editLengthNbrSamples, dwNewLoopLength, 0 ) )
	{
		SetLoopLength( dwNewLoopLength );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::HandleKillFocus
//
// Generic handler for KillFocus changes
bool CTabLoop::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return false;
	}

	// Check if the spin control is a valid window
	if (!::IsWindow(spin.m_hWnd))
	{
		return false;
	}

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_EDIT_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	if( pEditCtrl == NULL )
	{
		return false;
	}
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

	bool fChanged = false;

	// Handle empty text
	if( tcstrTmp[0] == NULL )
	{
		// Set it back to the minimum value
		spin.SetPos( nOffset );

		// Check if the value changed
		if( lUpdateVal != 0 )
		{
			// Update the item with the new value
			lUpdateVal = 0;
			fChanged = true;
		}
	}
	else
	{
		// Convert from text to an integer
		BOOL fTransSuccess;
		long lNewValue = GetDlgItemInt( pEditCtrl->GetDlgCtrlID(), &fTransSuccess, TRUE );

		// If unable to translate (if dialog contains invalid data, such as '-'), exit early
		if( !fTransSuccess )
		{
			// If conversion failed, update dialog item text
			SetDlgItemInt( pEditCtrl->GetDlgCtrlID(), lUpdateVal + nOffset );
		}
		// If conversion succeeded, ensure the value stays in range
		else
		{
			// Get the valid range
			int iLower, iUpper;
			spin.GetRange( iLower, iUpper );

			// Check if the value is out of range
			if( lNewValue > iUpper )
			{
				// Value out of range, update dialog item text
				SetDlgItemInt( pEditCtrl->GetDlgCtrlID(), iUpper );

				// Update the value to the top of the range
				lNewValue = iUpper;
			}

			// Check if the value changed
			if( lUpdateVal != (lNewValue - nOffset) )
			{
				// Update the item with the new value
				lUpdateVal = (lNewValue - nOffset);
				fChanged = true;
			}
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::HandleKillFocus
//
// Generic handler for KillFocus changes
bool CTabLoop::HandleKillFocus( CEdit& edit, DWORD& dwUpdateVal, int nOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return false;
	}

	// Check if the edit control is a valid window
	if (!::IsWindow(edit.m_hWnd))
	{
		return false;
	}

	// Get text from edit control
	CString strText;
	edit.GetWindowText( strText );
	strText.TrimRight();
	strText.TrimLeft();

	bool fChanged = false;

	// Handle empty text
	if( strText.IsEmpty() )
	{
		// Check if the value changed
		if( dwUpdateVal != 0 )
		{
			// Update the item with the new value
			dwUpdateVal = 0;
			fChanged = true;
		}
	}
	else
	{
		// Min/max enforced in EnforceLoopBoundaries()
		DWORD dwNewValue = (DWORD)_ttol( strText );

		// Check if the value changed
		if( dwUpdateVal != (dwNewValue - nOffset) )
		{
			// Update the item with the new value
			dwUpdateVal = (dwNewValue - nOffset);
			fChanged = true;
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::UpdateObject

void CTabLoop::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pIPropPageObject )
	{
		// Update the Property Page Object with the new item
		m_pPropPageMgr->m_pIPropPageObject->SetData( (void *)&m_PropItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::EnableControls

void CTabLoop::EnableControls( BOOL fEnable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if the Beat edit control exists
	if( m_editStartBeat.GetSafeHwnd() == NULL )
	{
		return;
	}

	// Cannot loop streaming waves
	/*if( m_PropItem.m_Item.m_WaveInfo.fIsStreaming )
	{
		fEnable = FALSE;
	}*/

	// Loop checked?
	BOOL fEnableLoopCheckBox = fEnable;
	if( fEnable
	&&  m_PropItem.m_Item.m_fLoopedUI == FALSE )
	{
		fEnable = FALSE;
	}

	// Check if multiple items are selected
	if( m_PropItem.m_dwBits & PROPF_MULTIPLESELECT )
	{
		// Yes - disable the rest of the controls
		fEnable = FALSE;
	}

	// Reference time or music time?
	CString strPrompt;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		strPrompt.LoadString( IDS_MIN_TEXT );
		m_staticPromptBar.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_SEC_TEXT );
		m_staticPromptBeat.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_GRID_TEXT );
		m_staticPromptGrid.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_MS_TEXT );
		m_staticPromptTick.SetWindowText( strPrompt );
	}
	else
	{
		strPrompt.LoadString( IDS_BAR_TEXT );
		m_staticPromptBar.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_BEAT_TEXT );
		m_staticPromptBeat.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_GRID_TEXT );
		m_staticPromptGrid.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_TICK_TEXT );
		m_staticPromptTick.SetWindowText( strPrompt );
	}

	// Update the controls with the new window state
	m_checkLoop.EnableWindow( fEnableLoopCheckBox );
	m_btnLockLength.EnableWindow( fEnable );
	
	m_spinStartTick.EnableWindow( fEnable );
	m_spinStartGrid.EnableWindow( fEnable );
	m_spinStartBeat.EnableWindow( fEnable );
	m_spinStartMeasure.EnableWindow( fEnable );
	m_spinStartSample.EnableWindow( fEnable );
	m_editStartTick.EnableWindow( fEnable );
	m_editStartGrid.EnableWindow( fEnable );
	m_editStartBeat.EnableWindow( fEnable );
	m_editStartMeasure.EnableWindow( fEnable );
	m_editStartSample.EnableWindow( fEnable );

	m_spinEndTick.EnableWindow( fEnable );
	m_spinEndGrid.EnableWindow( fEnable );
	m_spinEndBeat.EnableWindow( fEnable );
	m_spinEndMeasure.EnableWindow( fEnable );
	m_spinEndSample.EnableWindow( fEnable );
	m_editEndTick.EnableWindow( fEnable );
	m_editEndGrid.EnableWindow( fEnable );
	m_editEndBeat.EnableWindow( fEnable );
	m_editEndMeasure.EnableWindow( fEnable );
	m_editEndSample.EnableWindow( fEnable );

	m_spinLengthTick.EnableWindow( fEnable );
	m_spinLengthGrid.EnableWindow( fEnable );
	m_spinLengthBeat.EnableWindow( fEnable );
	m_spinLengthMeasure.EnableWindow( fEnable );
	m_spinLengthNbrSamples.EnableWindow( fEnable );
	m_editLengthTick.EnableWindow( fEnable );
	m_editLengthGrid.EnableWindow( fEnable );
	m_editLengthBeat.EnableWindow( fEnable );
	m_editLengthMeasure.EnableWindow( fEnable );
	m_editLengthNbrSamples.EnableWindow( fEnable );

	// Check if we're disabling
	if( !fEnable )
	{
		// Check if multiple items are selected
		if( m_PropItem.m_dwBits & PROPF_MULTIPLESELECT )
		{
			// Load the 'multiple select' text
			CString strText;
			if( strText.LoadString( IDS_MULTIPLE_SELECT ) )
			{
				// Display the 'multiple select' text
//				m_staticPromptMultiple.SetWindowText( strText );
//				return;
			}
		}
		else
		{
//			m_staticPromptMultiple.SetWindowText( _T("") );
		}

		// No items are selected, so clear the controls
		m_editStartTick.SetWindowText( NULL );
		m_editStartGrid.SetWindowText( NULL );
		m_editStartBeat.SetWindowText( NULL );
		m_editStartMeasure.SetWindowText( NULL );
		m_editStartSample.SetWindowText( NULL );

		m_editEndTick.SetWindowText( NULL );
		m_editEndGrid.SetWindowText( NULL );
		m_editEndBeat.SetWindowText( NULL );
		m_editEndMeasure.SetWindowText( NULL );
		m_editEndSample.SetWindowText( NULL );

		m_editLengthTick.SetWindowText( NULL );
		m_editLengthGrid.SetWindowText( NULL );
		m_editLengthBeat.SetWindowText( NULL );
		m_editLengthMeasure.SetWindowText( NULL );
		m_editLengthNbrSamples.SetWindowText( NULL );

		m_staticSourceStartTick.SetWindowText( NULL );
		m_staticSourceStartGrid.SetWindowText( NULL );
		m_staticSourceStartBeat.SetWindowText( NULL );
		m_staticSourceStartMeasure.SetWindowText( NULL );
		m_staticSourceStartSample.SetWindowText( NULL );

		m_staticSourceEndTick.SetWindowText( NULL );
		m_staticSourceEndGrid.SetWindowText( NULL );
		m_staticSourceEndBeat.SetWindowText( NULL );
		m_staticSourceEndMeasure.SetWindowText( NULL );
		m_staticSourceEndSample.SetWindowText( NULL );

		m_staticSourceLengthTick.SetWindowText( NULL );
		m_staticSourceLengthGrid.SetWindowText( NULL );
		m_staticSourceLengthBeat.SetWindowText( NULL );
		m_staticSourceLengthMeasure.SetWindowText( NULL );
		m_staticSourceLengthNbrSamples.SetWindowText( NULL );

		if( m_pPropPageMgr
		&&  m_pPropPageMgr->m_pTrackMgr )
		{
			CBitmap* pBitmap = m_pPropPageMgr->m_pTrackMgr->GetLockBitmap( 0 );
			if( pBitmap )
			{
				m_btnLockLength.SetBitmap( (HBITMAP)pBitmap->GetSafeHandle() );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnCheckLoop

void CTabLoop::OnCheckLoop( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// Check if the Loop check box exists
	if( m_checkLoop.GetSafeHwnd() == NULL )
	{
		return;
	}

	switch( IsDlgButtonChecked( IDC_CHECK_LOOP ) )
	{
		case 0:		// Currently unchecked
		case 2:		// Currently undetermined
			CheckDlgButton( IDC_CHECK_LOOP, 1 );
			m_PropItem.m_Item.m_fLoopedUI = TRUE;
			break;

		case 1:		// Currently checked
			CheckDlgButton( IDC_CHECK_LOOP, 0 );
			m_PropItem.m_Item.m_fLoopedUI = FALSE;
			break;
	}

	m_PropItem.m_dwChanged = CHGD_IS_LOOPED;
	UpdateObject();
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnDoubleClickedCheckLoop

void CTabLoop::OnDoubleClickedCheckLoop( void ) 
{
	OnCheckLoop();
}



/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnOK

// Windows translated presses of the 'Enter' key into presses of the button 'OK'.
// Here we create a hidden 'OK' button and translate presses of it into tabs to the next
// control in the property page.
void CTabLoop::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CWnd* pWnd = GetFocus();
	if( pWnd )
	{
		CWnd* pWndNext = GetNextDlgTabItem( pWnd );
		if( pWndNext )
		{
			pWndNext->SetFocus();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnSetActive

BOOL CTabLoop::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Refresh the page
	if(	m_fValidTrackItem )
	{
		SetPropTrackItem( &m_PropItem );
	}
	else
	{
		SetPropTrackItem( NULL );
	}

	if( m_pPropPageMgr 
	&&  m_pPropPageMgr->m_pTrackMgr 
	&&  m_pPropPageMgr->m_pIPropSheet )
	{
		m_pPropPageMgr->m_pIPropSheet->GetActivePage( &CPropPageMgrItem::sm_nActiveTab );
	}
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnLockLength

void CTabLoop::OnLockLength() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// Check if the Loop check box exists
	if( m_checkLoop.GetSafeHwnd() == NULL )
	{
		return;
	}

	// Update the item with the new value
	m_PropItem.m_Item.m_fLockLoopLengthUI = !m_PropItem.m_Item.m_fLockLoopLengthUI;

	// Now, update the object with the new value
	m_PropItem.m_dwChanged = CHGD_LOOP_LOCK_LENGTH;
	UpdateObject();
}


/////////////////////////////////////////////////////////////////////////////
// CTabLoop::OnDoubleClickedLockLength

void CTabLoop::OnDoubleClickedLockLength() 
{
	OnLockLength();
}
