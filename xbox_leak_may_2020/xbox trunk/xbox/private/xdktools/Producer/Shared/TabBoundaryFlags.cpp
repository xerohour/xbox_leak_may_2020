// TabBoundaryFlags.cpp : implementation file
//

#include "stdafx.h"
#include "TabBoundaryFlags.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

short* CTabBoundaryFlags::sm_pnActiveTab = NULL;

/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags property page

CTabBoundaryFlags::CTabBoundaryFlags( IDMUSProdPropPageManager* pIPageManager ) : CPropertyPage(CTabBoundaryFlags::IDD)
{
	//{{AFX_DATA_INIT(CTabBoundaryFlags)
	//}}AFX_DATA_INIT

	ASSERT( pIPageManager != NULL );

	m_pIPropPageObject = NULL;
	m_pIPageManager = pIPageManager;
	m_fNeedToDetach = FALSE;
	m_fUseTransitionPrompts = FALSE;
	m_rpIPropSheet = NULL;
}

CTabBoundaryFlags::~CTabBoundaryFlags()
{
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->Release();
		m_pIPropPageObject = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::UseTransitionPrompts

void CTabBoundaryFlags::UseTransitionPrompts( BOOL fUseTransitionPrompts )
{
	m_fUseTransitionPrompts = fUseTransitionPrompts;
}


void CTabBoundaryFlags::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabBoundaryFlags)
	DDX_Control(pDX, IDC_RADIO_INVALIDATE, m_radioInvalidate);
	DDX_Control(pDX, IDC_RADIO_INVALIDATEPRI, m_radioInvalidatePri);
	DDX_Control(pDX, IDC_RADIO_NOINVALIDATE, m_radioNoInvalidate);
	DDX_Control(pDX, IDC_STATIC_PROMPT, m_staticPrompt);
	DDX_Control(pDX, IDC_STATIC_NO_MARKERS, m_staticNoMarkers);
	DDX_Control(pDX, IDC_ALIGN_OPTIONS_PROMPT, m_staticAlignPrompt);
	DDX_Control(pDX, IDC_CHECK_SEG_DEFAULT, m_checkSegDefault);
	DDX_Control(pDX, IDC_CHECK_BOUNDARY, m_checkMarker);
	DDX_Control(pDX, IDC_COMBO_BOUNDARY, m_comboBoundary);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_TIME, m_radioSwitchAnyTime);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_GRID, m_radioSwitchAnyGrid);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_BEAT, m_radioSwitchAnyBeat);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_BAR, m_radioSwitchAnyBar);
	DDX_Control(pDX, IDC_RADIO_TIMING_QUICK, m_radioTimingQuick);
	DDX_Control(pDX, IDC_RADIO_TIMING_AFTERPREPARETIME, m_radioTimingAfterPrepareTime);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::SetObject

void CTabBoundaryFlags::SetObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->Release();
	}
	m_pIPropPageObject = pIPropPageObject;
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->AddRef();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::UpdateObject

void CTabBoundaryFlags::UpdateObject()
{
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->SetData( (void *)&m_PPGTabBoundaryFlags );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::RefreshTab

void CTabBoundaryFlags::RefreshTab( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	CString strPrompt;
	m_staticPrompt.SetWindowText( NULL );
	if( m_fUseTransitionPrompts )
	{
		strPrompt.LoadString( IDS_TRANSITION_PROMPT1 );
		m_checkMarker.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_TRANSITION_PROMPT2 );
		m_staticNoMarkers.SetWindowText( strPrompt );
	}
	else
	{
		strPrompt.LoadString( IDS_START_PROMPT1 );
		m_checkMarker.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_START_PROMPT2 );
		m_staticNoMarkers.SetWindowText( strPrompt );
	}

	if( m_pIPropPageObject == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	PPGTabBoundaryFlags *pPPGTabBoundaryFlags = &m_PPGTabBoundaryFlags;
	if( FAILED( m_pIPropPageObject->GetData( (void**)&pPPGTabBoundaryFlags ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	m_staticPrompt.SetWindowText( m_PPGTabBoundaryFlags.strPrompt );

	if( !(m_PPGTabBoundaryFlags.dwFlagsUI & PROPF_HAVE_VALID_DATA) )
	{
		EnableControls( FALSE );
		return;
	}

	// Prevent control notifications from being dispatched during RefreshObject
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;

	EnableControls( TRUE );

	// "Marker" check box
	if( (m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_ALIGN)
	||	(m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_QUEUE) )
	{
		m_checkMarker.SetCheck( 0 );
	}
	else
	{
		m_checkMarker.SetCheck( (m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_MARKER) ? 1 : 0 );
	}

	// "Segment Default" check box
	m_checkSegDefault.SetCheck( (m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_DEFAULT) ? 1 : 0 );

	// "Boundary" combo box
	int nCurSel = -1;
	if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_DEFAULT )
	{
		// Default to Barline
		nCurSel = 3; 
	}
	else
	{
		if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_ALIGN )
		{
			if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_BEAT )
			{
				// Align to Beat
				nCurSel = 4;
			}
			else if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_MEASURE )
			{
				// Align to Barline
				nCurSel = 5;
			}
			else if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_SEGMENTEND )
			{
				// Align to Segment
				nCurSel = 6;
			}
		}
		else
		{
			if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_GRID )
			{
				// Grid
				nCurSel = 1;
			}
			else if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_BEAT )
			{
				// Beat
				nCurSel = 2;
			}
			else if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_MEASURE )
			{
				// Barline
				nCurSel = 3;
			}
			else if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_SEGMENTEND )
			{
				// End of Segment
				nCurSel = 7;
			}
			else if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_QUEUE )
			{
				// End of Segment Queue
				nCurSel = 8;
			}
			else
			{
				// Immediate
				nCurSel = 0;
			}
		}
	}
	m_comboBoundary.SetCurSel( nCurSel );

	// Set the 'Don't Cutoff' radio button state
	int nRadioButton = IDC_RADIO_INVALIDATE;
	if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_INVALIDATE_PRI )
	{
		nRadioButton = IDC_RADIO_INVALIDATEPRI;
	}
	else if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_NOINVALIDATE )
	{
		nRadioButton = IDC_RADIO_NOINVALIDATE;
	}
	CheckRadioButton( IDC_RADIO_NOINVALIDATE, IDC_RADIO_INVALIDATE, nRadioButton );

	// "Switch" radio buttons
	if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_ALIGN )
	{
		m_radioSwitchAnyTime.SetCheck( 0 );
		m_radioSwitchAnyGrid.SetCheck( 0 );
		m_radioSwitchAnyBeat.SetCheck( 0 );
		m_radioSwitchAnyBar.SetCheck( 0 );
		if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_VALID_START_GRID )
		{
			m_radioSwitchAnyGrid.SetCheck( 1 );
		}
		else if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_VALID_START_BEAT )
		{
			m_radioSwitchAnyBeat.SetCheck( 1 );
		}
		else if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_VALID_START_MEASURE )
		{
			m_radioSwitchAnyBar.SetCheck( 1 );
		}
		else //if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_VALID_START_TICK )
		{
			m_radioSwitchAnyTime.SetCheck( 1 );
		}
	}

	// "Timing" radio buttons
	m_radioTimingQuick.SetCheck( 0 );
	m_radioTimingAfterPrepareTime.SetCheck( 0 );
	if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_AFTERPREPARETIME )
	{
		m_radioTimingAfterPrepareTime.SetCheck( 1 );
	}
	else
	{
		m_radioTimingQuick.SetCheck( 1 );
	}

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::EnableControls

void CTabBoundaryFlags::EnableControls( BOOL fEnable ) 
{
	m_staticPrompt.EnableWindow( fEnable );
	if( !fEnable
	||	(m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_ALIGN)
	||	(m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_QUEUE) )
	{
		m_checkMarker.EnableWindow( FALSE );
		m_staticNoMarkers.ShowWindow( SW_HIDE );
	}
	else
	{
		m_checkMarker.EnableWindow( TRUE );
		m_staticNoMarkers.ShowWindow( (m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_MARKER) ? SW_SHOW : SW_HIDE );
	}

	m_radioInvalidate.EnableWindow( fEnable );
	m_radioInvalidatePri.EnableWindow( fEnable );
	m_radioNoInvalidate.EnableWindow( fEnable );
	m_radioTimingQuick.EnableWindow( fEnable );
	m_radioTimingAfterPrepareTime.EnableWindow( fEnable );

	BOOL fEnableSegDefault = fEnable;
	if( m_PPGTabBoundaryFlags.dwFlagsUI & PROPF_OMIT_SEGF_DEFAULT )
	{
		m_PPGTabBoundaryFlags.dwPlayFlags &= ~DMUS_SEGF_DEFAULT;
		fEnableSegDefault = FALSE;
	}
	m_checkSegDefault.EnableWindow( fEnableSegDefault );

	BOOL fEnableComboBoundary = fEnable;
	if( m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_DEFAULT )
	{
		fEnableComboBoundary = FALSE;
	}
	m_comboBoundary.EnableWindow( fEnableComboBoundary );

	BOOL fEnableSwitchRadioButtons = fEnable;
	if( !(m_PPGTabBoundaryFlags.dwPlayFlags & DMUS_SEGF_ALIGN) )
	{
		fEnableSwitchRadioButtons = FALSE;
	}
	m_staticAlignPrompt.EnableWindow( fEnableSwitchRadioButtons );
	m_radioSwitchAnyTime.EnableWindow( fEnableSwitchRadioButtons );
	m_radioSwitchAnyGrid.EnableWindow( fEnableSwitchRadioButtons );
	m_radioSwitchAnyBeat.EnableWindow( fEnableSwitchRadioButtons );
	m_radioSwitchAnyBar.EnableWindow( fEnableSwitchRadioButtons );

	if( fEnable == FALSE )
	{
		m_checkMarker.SetCheck( 0 );
		m_comboBoundary.SetCurSel( -1 );
		m_radioInvalidate.SetCheck( 0 );
		m_radioInvalidatePri.SetCheck( 0 );
		m_radioNoInvalidate.SetCheck( 0 );
		m_radioTimingQuick.SetCheck( 0 );
		m_radioTimingAfterPrepareTime.SetCheck( 0 );
	}

	if( fEnableSegDefault == FALSE )
	{
		m_checkSegDefault.SetCheck( 0 );
	}

	if( fEnableSwitchRadioButtons == FALSE )
	{
		m_radioSwitchAnyTime.SetCheck( 0 );
		m_radioSwitchAnyGrid.SetCheck( 0 );
		m_radioSwitchAnyBeat.SetCheck( 0 );
		m_radioSwitchAnyBar.SetCheck( 0 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::HandlePlayFlags

void CTabBoundaryFlags::HandlePlayFlags() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Determine new flags
	DWORD dwNewPlayFlags = 0;

	if( m_checkSegDefault.GetCheck() )
	{
		dwNewPlayFlags |= DMUS_SEGF_DEFAULT;
	}
	else
	{
		switch( m_comboBoundary.GetCurSel() )
		{
			// Immediate
			case 0:
				dwNewPlayFlags = 0;
				break;

			// Grid
			case 1:
				dwNewPlayFlags = DMUS_SEGF_GRID;
				break;

			// Beat
			case 2:
				dwNewPlayFlags = DMUS_SEGF_BEAT;
				break;

			// Barline
			case 3:
				dwNewPlayFlags = DMUS_SEGF_MEASURE;
				break;

			// Align to Beat
			case 4:
				dwNewPlayFlags = (DMUS_SEGF_ALIGN | DMUS_SEGF_BEAT);
				break;

			// Align to Barline
			case 5:
				dwNewPlayFlags = (DMUS_SEGF_ALIGN | DMUS_SEGF_MEASURE);
				break;

			// Align to Segment
			case 6:
				dwNewPlayFlags = (DMUS_SEGF_ALIGN | DMUS_SEGF_SEGMENTEND);
				break;

			// End of Segment
			case 7:
				dwNewPlayFlags = DMUS_SEGF_SEGMENTEND;
				break;

			// End of Segment Queue
			case 8:
				dwNewPlayFlags = DMUS_SEGF_QUEUE;
				break;

			default:
				break;
		}
	}

	// Clear "Marker" check box, if necessary
	if( (dwNewPlayFlags & DMUS_SEGF_ALIGN)
	||	(dwNewPlayFlags & DMUS_SEGF_QUEUE) )
	{
		m_checkMarker.SetCheck( 0 );
	}

	if( dwNewPlayFlags & DMUS_SEGF_ALIGN )
	{
		// Check radio buttons for 'Switch' points
		if( m_radioSwitchAnyTime.GetCheck() )
		{
			dwNewPlayFlags |= DMUS_SEGF_VALID_START_TICK;
		}
		else if( m_radioSwitchAnyGrid.GetCheck() )
		{
			dwNewPlayFlags |= DMUS_SEGF_VALID_START_GRID;
		}
		else if( m_radioSwitchAnyBeat.GetCheck() )
		{
			dwNewPlayFlags |= DMUS_SEGF_VALID_START_BEAT;
		}
		else if( m_radioSwitchAnyBar.GetCheck() )
		{
			dwNewPlayFlags |= DMUS_SEGF_VALID_START_MEASURE;
		}
	}

	if( m_checkMarker.GetCheck() )
	{
		dwNewPlayFlags |= DMUS_SEGF_MARKER;
	}

	// Get the 'Don't Cutoff' radio button states
	switch( GetCheckedRadioButton( IDC_RADIO_NOINVALIDATE, IDC_RADIO_INVALIDATE ) )
	{
	case IDC_RADIO_NOINVALIDATE:
		dwNewPlayFlags |= DMUS_SEGF_NOINVALIDATE;
		break;
	case IDC_RADIO_INVALIDATEPRI:
		dwNewPlayFlags |= DMUS_SEGF_INVALIDATE_PRI;
		break;
	case IDC_RADIO_INVALIDATE:
		// Do nothing
		break;
	}

	if( m_radioTimingAfterPrepareTime.GetCheck() )
	{
		dwNewPlayFlags |= DMUS_SEGF_AFTERPREPARETIME;
	}

	// If flags have changed, update the object
	if( dwNewPlayFlags != m_PPGTabBoundaryFlags.dwPlayFlags ) 
	{
		m_PPGTabBoundaryFlags.dwPlayFlags = dwNewPlayFlags;
		UpdateObject();
		RefreshTab();
	}
}


BEGIN_MESSAGE_MAP(CTabBoundaryFlags, CPropertyPage)
	//{{AFX_MSG_MAP(CTabBoundaryFlags)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CHECK_BOUNDARY, OnCheckMarker)
	ON_BN_DOUBLECLICKED(IDC_CHECK_BOUNDARY, OnDoubleClickedCheckMarker)
	ON_BN_CLICKED(IDC_CHECK_SEG_DEFAULT, OnCheckSegDefault)
	ON_BN_DOUBLECLICKED(IDC_CHECK_SEG_DEFAULT, OnDoubleClickedCheckSegDefault)
	ON_CBN_SELCHANGE(IDC_COMBO_BOUNDARY, OnSelChangeComboBoundary)
	ON_BN_CLICKED(IDC_RADIO_SWITCH_ANY_BAR, OnRadioSwitchAnyBar)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SWITCH_ANY_BAR, OnDoubleClickedRadioSwitchAnyBar)
	ON_BN_CLICKED(IDC_RADIO_SWITCH_ANY_BEAT, OnRadioSwitchAnyBeat)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SWITCH_ANY_BEAT, OnDoubleClickedRadioSwitchAnyBeat)
	ON_BN_CLICKED(IDC_RADIO_SWITCH_ANY_GRID, OnRadioSwitchAnyGrid)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SWITCH_ANY_GRID, OnDoubleClickedRadioSwitchAnyGrid)
	ON_BN_CLICKED(IDC_RADIO_SWITCH_ANY_TIME, OnRadioSwitchAnyTime)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SWITCH_ANY_TIME, OnDoubleClickedRadioSwitchAnyTime)
	ON_BN_CLICKED(IDC_RADIO_TIMING_AFTERPREPARETIME, OnRadioTimingAfterPrepareTime)
	ON_BN_DOUBLECLICKED(IDC_RADIO_TIMING_AFTERPREPARETIME, OnDoubleClickedRadioTimingAfterPrepareTime)
	ON_BN_CLICKED(IDC_RADIO_TIMING_QUICK, OnRadioTimingQuick)
	ON_BN_DOUBLECLICKED(IDC_RADIO_TIMING_QUICK, OnDoubleClickedRadioTimingQuick)
	ON_BN_CLICKED(IDC_RADIO_INVALIDATE, OnRadioInvalidate)
	ON_BN_CLICKED(IDC_RADIO_INVALIDATEPRI, OnRadioInvalidatepri)
	ON_BN_CLICKED(IDC_RADIO_NOINVALIDATE, OnRadioNoinvalidate)
	ON_BN_DOUBLECLICKED(IDC_RADIO_INVALIDATE, OnDoubleClickedRadioInvalidate)
	ON_BN_DOUBLECLICKED(IDC_RADIO_INVALIDATEPRI, OnDoubleClickedRadioInvalidatepri)
	ON_BN_DOUBLECLICKED(IDC_RADIO_NOINVALIDATE, OnDoubleClickedRadioNoinvalidate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnSetActive

BOOL CTabBoundaryFlags::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	RefreshTab();

	// Store active tab
	if( sm_pnActiveTab && m_rpIPropSheet )
	{
		m_rpIPropSheet->GetActivePage( sm_pnActiveTab );
	}
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDestroy

void CTabBoundaryFlags::OnDestroy() 
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
// CTabBoundaryFlags::OnCreate

int CTabBoundaryFlags::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabBoundaryFlags::OnCheckMarker

void CTabBoundaryFlags::OnCheckMarker() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedCheckMarker

void CTabBoundaryFlags::OnDoubleClickedCheckMarker() 
{
	OnCheckMarker();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnCheckSegDefault

void CTabBoundaryFlags::OnCheckSegDefault() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedCheckSegDefault

void CTabBoundaryFlags::OnDoubleClickedCheckSegDefault() 
{
	OnCheckSegDefault();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnRadioInvalidate

void CTabBoundaryFlags::OnRadioInvalidate() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnRadioInvalidatepri

void CTabBoundaryFlags::OnRadioInvalidatepri() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnRadioNoinvalidate

void CTabBoundaryFlags::OnRadioNoinvalidate() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedRadioInvalidate

void CTabBoundaryFlags::OnDoubleClickedRadioInvalidate() 
{
	OnRadioInvalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedRadioInvalidatepri

void CTabBoundaryFlags::OnDoubleClickedRadioInvalidatepri() 
{
	OnRadioInvalidatepri();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedRadioNoinvalidate

void CTabBoundaryFlags::OnDoubleClickedRadioNoinvalidate() 
{
	OnRadioNoinvalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnSelChangeComboBoundary

void CTabBoundaryFlags::OnSelChangeComboBoundary() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnRadioSwitchAnyBar

void CTabBoundaryFlags::OnRadioSwitchAnyBar() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedRadioSwitchAnyBar

void CTabBoundaryFlags::OnDoubleClickedRadioSwitchAnyBar() 
{
	OnRadioSwitchAnyBar();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnRadioSwitchAnyBeat

void CTabBoundaryFlags::OnRadioSwitchAnyBeat() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedRadioSwitchAnyBeat

void CTabBoundaryFlags::OnDoubleClickedRadioSwitchAnyBeat() 
{
	OnRadioSwitchAnyBeat();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnRadioSwitchAnyGrid

void CTabBoundaryFlags::OnRadioSwitchAnyGrid() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedRadioSwitchAnyGrid

void CTabBoundaryFlags::OnDoubleClickedRadioSwitchAnyGrid() 
{
	OnRadioSwitchAnyGrid();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnRadioSwitchAnyTime

void CTabBoundaryFlags::OnRadioSwitchAnyTime() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedRadioSwitchAnyTime

void CTabBoundaryFlags::OnDoubleClickedRadioSwitchAnyTime() 
{
	OnRadioSwitchAnyTime();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnRadioSwitchNextBoundary

void CTabBoundaryFlags::OnRadioSwitchNextBoundary() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedRadioSwitchNextBoundary

void CTabBoundaryFlags::OnDoubleClickedRadioSwitchNextBoundary() 
{
	OnRadioSwitchNextBoundary();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnRadioTimingAfterPrepareTime

void CTabBoundaryFlags::OnRadioTimingAfterPrepareTime() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedRadioTimingAfterPrepareTime

void CTabBoundaryFlags::OnDoubleClickedRadioTimingAfterPrepareTime() 
{
	OnRadioTimingAfterPrepareTime();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnRadioTimingQuick

void CTabBoundaryFlags::OnRadioTimingQuick() 
{
	HandlePlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundaryFlags::OnDoubleClickedRadioTimingQuick

void CTabBoundaryFlags::OnDoubleClickedRadioTimingQuick() 
{
	OnRadioTimingQuick();
}
