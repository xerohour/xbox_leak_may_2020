// TabBoundary.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TrackItem.h"
#include "PropPageMgr_Item.h"
#include "TabBoundary.h"
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
// CTabBoundary property page

/////////////////////////////////////////////////////////////////////////////
// CTabBoundary constructor/destructor

CTabBoundary::CTabBoundary(): CPropertyPage(CTabBoundary::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_PROPPAGE_FILEREF);
	//{{AFX_DATA_INIT(CTabBoundary)
	//}}AFX_DATA_INIT

	// Initialize our pointers to NULL
	m_pPropPageMgr = NULL;
	m_pTimeline = NULL;

	// Initialize our class members to FALSE
	m_fValidTrackItem = FALSE;
	m_fNeedToDetach = FALSE;
}

CTabBoundary::~CTabBoundary()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::DoDataExchange

void CTabBoundary::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CTabBoundary)
	DDX_Control(pDX, IDC_RADIO_INVALIDATE, m_radioInvalidate);
	DDX_Control(pDX, IDC_RADIO_INVALIDATEPRI, m_radioInvalidatePri);
	DDX_Control(pDX, IDC_RADIO_NOINVALIDATE, m_radioNoInvalidate);
	DDX_Control(pDX, IDC_STATIC_PROMPT, m_staticPrompt);
	DDX_Control(pDX, IDC_STATIC_NO_MARKERS, m_staticNoMarkers);
	DDX_Control(pDX, IDC_ALIGN_OPTIONS_PROMPT, m_staticAlignPrompt);
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


BEGIN_MESSAGE_MAP(CTabBoundary, CPropertyPage)
	//{{AFX_MSG_MAP(CTabBoundary)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_BOUNDARY, OnCheckMarker)
	ON_BN_DOUBLECLICKED(IDC_CHECK_BOUNDARY, OnDoubleClickedCheckMarker)
	ON_CBN_SELCHANGE(IDC_COMBO_BOUNDARY, OnSelChangeComboBoundary)
	ON_BN_CLICKED(IDC_RADIO_SWITCH_ANY_TIME, OnRadioSwitchAnyTime)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SWITCH_ANY_TIME, OnDoubleClickedRadioSwitchAnyTime)
	ON_BN_CLICKED(IDC_RADIO_SWITCH_ANY_GRID, OnRadioSwitchAnyGrid)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SWITCH_ANY_GRID, OnDoubleClickedRadioSwitchAnyGrid)
	ON_BN_CLICKED(IDC_RADIO_SWITCH_ANY_BEAT, OnRadioSwitchAnyBeat)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SWITCH_ANY_BEAT, OnDoubleClickedRadioSwitchAnyBeat)
	ON_BN_CLICKED(IDC_RADIO_TIMING_AFTERPREPARETIME, OnRadioTimingAfterPrepareTime)
	ON_BN_DOUBLECLICKED(IDC_RADIO_TIMING_AFTERPREPARETIME, OnDoubleClickedRadioTimingAfterPrepareTime)
	ON_BN_CLICKED(IDC_RADIO_TIMING_QUICK, OnRadioTimingQuick)
	ON_BN_DOUBLECLICKED(IDC_RADIO_TIMING_QUICK, OnDoubleClickedRadioTimingQuick)
	ON_BN_CLICKED(IDC_RADIO_SWITCH_ANY_BAR, OnRadioSwitchAnyBar)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SWITCH_ANY_BAR, OnDoubleclickedRadioSwitchAnyBar)
	ON_BN_CLICKED(IDC_RADIO_INVALIDATE, OnRadioInvalidate)
	ON_BN_CLICKED(IDC_RADIO_INVALIDATEPRI, OnRadioInvalidatepri)
	ON_BN_CLICKED(IDC_RADIO_NOINVALIDATE, OnRadioNoinvalidate)
	ON_BN_DOUBLECLICKED(IDC_RADIO_INVALIDATE, OnDoubleclickedRadioInvalidate)
	ON_BN_DOUBLECLICKED(IDC_RADIO_INVALIDATEPRI, OnDoubleclickedRadioInvalidatepri)
	ON_BN_DOUBLECLICKED(IDC_RADIO_NOINVALIDATE, OnDoubleclickedRadioNoinvalidate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabBoundary custom functions

/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::SetTrackItem

void CTabBoundary::SetTrackItem( const CTrackItem* pItem )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the pointer is null, there are no items selected, so disable the property page
	if( pItem == NULL )
	{
		m_fValidTrackItem = FALSE;
		m_TrackItem.Clear();
		EnableControls( FALSE );
		return;
	}

	// Initialize the timeline pointer to NULL
	m_pTimeline = NULL;

	// Initialize our groupbits to all 32 groups
	DWORD dwGroupBits = 0xFFFFFFFF;

	// Verify we have a valid pinter to our Property Page Object
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

	// Flag that we have a valid item
	m_fValidTrackItem = TRUE;

	// Copy the information from the new item
	m_TrackItem.Copy( pItem );

	// Check if the combo control has a valid window handle.
	if( m_comboBoundary.GetSafeHwnd() == NULL )
	{
		// It doesn't have a valid window handle - the property page may have been destroyed (or not yet created).
		// Just exit early
		return;
	}

	// Don't send OnKill/OnUpdate notifications when updating the display
	CLockoutNotification LockoutNotifications( m_hWnd );

	// Check if multiple items are selected
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
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

	// "Prompt"
	CString strPrompt;
	m_TrackItem.FormatUIText( strPrompt );
	m_staticPrompt.SetWindowText( strPrompt );

	// "Marker" check box
	if( (m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_ALIGN)
	||	(m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_QUEUE) )
	{
		m_checkMarker.SetCheck( 0 );
	}
	else
	{
		m_checkMarker.SetCheck( (m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_MARKER) ? 1 : 0 );
	}

	// "Boundary" combo box
	int nCurSel = -1;
	if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_ALIGN )
	{
		if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_BEAT )
		{
			// Align to Beat
			nCurSel = 5;
		}
		else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_MEASURE )
		{
			// Align to Bar
			nCurSel = 6;
		}
		else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_SEGMENTEND )
		{
			// Align to Segment
			nCurSel = 7;
		}
	}
	else
	{
		if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_DEFAULT )
		{
			// Segment's Default
			nCurSel = 0;
		}
		else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_GRID )
		{
			// Grid
			nCurSel = 2;
		}
		else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_BEAT )
		{
			// Beat
			nCurSel = 3;
		}
		else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_MEASURE )
		{
			// Bar
			nCurSel = 4;
		}
		else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_SEGMENTEND )
		{
			// End of Segment
			nCurSel = 8;
		}
		else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_QUEUE )
		{
			// End of Segment Queue
			nCurSel = 9;
		}
		else
		{
			// Immediate
			nCurSel = 1;
		}
	}
	m_comboBoundary.SetCurSel( nCurSel );

	// Set the 'Don't Cutoff' radio button state
	int nRadioButton = IDC_RADIO_INVALIDATE;
	if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_INVALIDATE_PRI )
	{
		nRadioButton = IDC_RADIO_INVALIDATEPRI;
	}
	else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_NOINVALIDATE )
	{
		nRadioButton = IDC_RADIO_NOINVALIDATE;
	}
	CheckRadioButton( IDC_RADIO_NOINVALIDATE, IDC_RADIO_INVALIDATE, nRadioButton );

	// Set align radio buttons
	m_radioSwitchAnyBar.SetCheck( 0 );
	m_radioSwitchAnyBeat.SetCheck( 0 );
	m_radioSwitchAnyGrid.SetCheck( 0 );
	m_radioSwitchAnyTime.SetCheck( 0 );
	if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_ALIGN )
	{
		if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_VALID_START_GRID )
		{
			m_radioSwitchAnyGrid.SetCheck( 1 );
		}
		else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_VALID_START_BEAT )
		{
			m_radioSwitchAnyBeat.SetCheck( 1 );
		}
		else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_VALID_START_MEASURE )
		{
			m_radioSwitchAnyBar.SetCheck( 1 );
		}
		else //if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_VALID_START_TICK )
		{
			m_radioSwitchAnyTime.SetCheck( 1 );
		}
	}

	// "Timing" radio buttons
	m_radioTimingQuick.SetCheck( 0 );
	m_radioTimingAfterPrepareTime.SetCheck( 0 );
	if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_AFTERPREPARETIME )
	{
		m_radioTimingAfterPrepareTime.SetCheck( 1 );
	}
	else
	{
		m_radioTimingQuick.SetCheck( 1 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnCreate

int CTabBoundary::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabBoundary::OnDestroy

void CTabBoundary::OnDestroy() 
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
// CTabBoundary::OnInitDialog

BOOL CTabBoundary::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Call the base class's OnInitDialog method
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::UpdateObject

void CTabBoundary::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if( m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject )
	{
		// Update the Property Page Object with the new item
		m_pPropPageMgr->m_pIPropPageObject->SetData( (void *) &m_TrackItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::EnableControls

void CTabBoundary::EnableControls( BOOL fEnable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if the Beat edit control exists
	if( m_comboBoundary.GetSafeHwnd() == NULL )
	{
		return;
	}

	// Update the controls with the new window state
	m_staticPrompt.EnableWindow( fEnable );
	m_comboBoundary.EnableWindow( fEnable );
	m_radioInvalidate.EnableWindow( fEnable );
	m_radioInvalidatePri.EnableWindow( fEnable );
	m_radioNoInvalidate.EnableWindow( fEnable );
	m_radioTimingQuick.EnableWindow( fEnable );
	m_radioTimingAfterPrepareTime.EnableWindow( fEnable );

	if( (m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_ALIGN)
	||	(m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_QUEUE) )
	{
		m_checkMarker.EnableWindow( FALSE );
		m_checkMarker.SetCheck( 0 );
		m_staticNoMarkers.ShowWindow( SW_HIDE );
	}
	else
	{
		m_checkMarker.EnableWindow( fEnable );
		m_staticNoMarkers.ShowWindow( (m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_MARKER) ? SW_SHOW : SW_HIDE );
	}

	BOOL fEnableSwitchRadioButtons = fEnable;
	if( !(m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_ALIGN) )
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
		// Check if multiple items are selected
		if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
		{
			// Load the 'multiple select' text
			CString strText;
			if( strText.LoadString( IDS_MULTIPLE_SELECT ) )
			{
				// Display the 'multiple select' text
				m_staticPrompt.SetWindowText( strText );
			}
		}
		else
		{
			m_staticPrompt.SetWindowText( NULL );
		}

		m_checkMarker.SetCheck( 0 );
		m_comboBoundary.SetCurSel( -1 );
		m_radioInvalidate.SetCheck( 0 );
		m_radioInvalidatePri.SetCheck( 0 );
		m_radioNoInvalidate.SetCheck( 0 );
		m_radioTimingQuick.SetCheck( 0 );
		m_radioTimingAfterPrepareTime.SetCheck( 0 );
	}

	if( fEnableSwitchRadioButtons == FALSE )
	{
		m_radioSwitchAnyTime.SetCheck( 0 );
		m_radioSwitchAnyGrid.SetCheck( 0 );
		m_radioSwitchAnyBeat.SetCheck( 0 );
		m_radioSwitchAnyBar.SetCheck( 0 );
	}
}


#define FLAGS_THIS_TAB	(DMUS_SEGF_ALIGN | DMUS_SEGF_QUEUE | DMUS_SEGF_SEGMENTEND | DMUS_SEGF_DEFAULT | \
						 DMUS_SEGF_GRID | DMUS_SEGF_BEAT | DMUS_SEGF_MEASURE | \
						 DMUS_SEGF_VALID_START_MEASURE | DMUS_SEGF_VALID_START_GRID | DMUS_SEGF_VALID_START_BEAT | DMUS_SEGF_VALID_START_TICK | \
						 DMUS_SEGF_MARKER | DMUS_SEGF_NOINVALIDATE | DMUS_SEGF_AFTERPREPARETIME )

/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::SetPlayFlags

void CTabBoundary::SetPlayFlags() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		return;
	}

	m_TrackItem.m_dwPlayFlagsDM &= ~FLAGS_THIS_TAB;

	// Boundary combo box
	switch( m_comboBoundary.GetCurSel() )
	{
		// Segment's Default	
		case 0:
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_DEFAULT;
			break;

		// Immediate
		case 1:
			break;

		// Grid
		case 2:
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_GRID;
			break;

		// Beat
		case 3:
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_BEAT;
			break;

		// Bar
		case 4:
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_MEASURE;
			break;

		// Align to Beat
		case 5:
			m_TrackItem.m_dwPlayFlagsDM |= (DMUS_SEGF_ALIGN | DMUS_SEGF_BEAT);
			break;

		// Align to Bar
		case 6:
			m_TrackItem.m_dwPlayFlagsDM |= (DMUS_SEGF_ALIGN | DMUS_SEGF_MEASURE);
			break;

		// Align to Segment
		case 7:
			m_TrackItem.m_dwPlayFlagsDM |= (DMUS_SEGF_ALIGN | DMUS_SEGF_SEGMENTEND);
			break;

		// End of Segment
		case 8:
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_SEGMENTEND;
			break;

		// End of Segment Queue
		case 9:
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_QUEUE;
			break;

		default:
			break;
	}

	if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_ALIGN )
	{
		// Check radio buttons for 'Switch' points
		if( m_radioSwitchAnyTime.GetCheck() )
		{
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_VALID_START_TICK;
		}
		else if( m_radioSwitchAnyGrid.GetCheck() )
		{
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_VALID_START_GRID;
		}
		else if( m_radioSwitchAnyBeat.GetCheck() )
		{
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_VALID_START_BEAT;
		}
		else if( m_radioSwitchAnyBar.GetCheck() )
		{
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_VALID_START_MEASURE;
		}
	}

	if( m_checkMarker.GetCheck() )
	{
		m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_MARKER;
	}

	// Get the 'Don't Cutoff' radio button states
	switch( GetCheckedRadioButton( IDC_RADIO_NOINVALIDATE, IDC_RADIO_INVALIDATE ) )
	{
	case IDC_RADIO_NOINVALIDATE:
		m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_NOINVALIDATE;
		break;
	case IDC_RADIO_INVALIDATEPRI:
		m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_INVALIDATE_PRI;
		break;
	case IDC_RADIO_INVALIDATE:
		// Do nothing
		break;
	}

	if( m_radioTimingAfterPrepareTime.GetCheck() )
	{
		m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_AFTERPREPARETIME;
	}

	UpdateObject();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnOK

// Windows translated presses of the 'Enter' key into presses of the button 'OK'.
// Here we create a hidden 'OK' button and translate presses of it into tabs to the next
// control in the property page.
void CTabBoundary::OnOK() 
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
// CTabBoundary::OnCheckMarker

void CTabBoundary::OnCheckMarker() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleClickedCheckMarker

void CTabBoundary::OnDoubleClickedCheckMarker() 
{
	OnCheckMarker();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnRadioInvalidate

void CTabBoundary::OnRadioInvalidate() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnRadioInvalidatepri

void CTabBoundary::OnRadioInvalidatepri() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnRadioNoinvalidate

void CTabBoundary::OnRadioNoinvalidate() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleclickedRadioInvalidate

void CTabBoundary::OnDoubleclickedRadioInvalidate() 
{
	OnRadioInvalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleclickedRadioInvalidatepri

void CTabBoundary::OnDoubleclickedRadioInvalidatepri() 
{
	OnRadioInvalidatepri();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleclickedRadioNoinvalidate

void CTabBoundary::OnDoubleclickedRadioNoinvalidate() 
{
	OnRadioNoinvalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnSelChangeComboBoundary

void CTabBoundary::OnSelChangeComboBoundary() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnRadioSwitchNextBoundary

void CTabBoundary::OnRadioSwitchNextBoundary() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleClickedRadioSwitchNextBoundary

void CTabBoundary::OnDoubleClickedRadioSwitchNextBoundary() 
{
	OnRadioSwitchNextBoundary();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnRadioSwitchAnyTime

void CTabBoundary::OnRadioSwitchAnyTime() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleClickedRadioSwitchAnyTime

void CTabBoundary::OnDoubleClickedRadioSwitchAnyTime() 
{
	OnRadioSwitchAnyTime();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnRadioSwitchAnyGrid

void CTabBoundary::OnRadioSwitchAnyGrid() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleClickedRadioSwitchAnyGrid

void CTabBoundary::OnDoubleClickedRadioSwitchAnyGrid() 
{
	OnRadioSwitchAnyGrid();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnRadioSwitchAnyBeat

void CTabBoundary::OnRadioSwitchAnyBeat() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleClickedRadioSwitchAnyBeat

void CTabBoundary::OnDoubleClickedRadioSwitchAnyBeat() 
{
	OnRadioSwitchAnyBeat();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnRadioSwitchAnyBar

void CTabBoundary::OnRadioSwitchAnyBar() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleClickedRadioSwitchAnyBar

void CTabBoundary::OnDoubleclickedRadioSwitchAnyBar() 
{
	OnRadioSwitchAnyBar();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnRadioTimingAfterPrepareTime

void CTabBoundary::OnRadioTimingAfterPrepareTime() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleClickedRadioTimingAfterPrepareTime

void CTabBoundary::OnDoubleClickedRadioTimingAfterPrepareTime() 
{
	OnRadioTimingAfterPrepareTime();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnRadioTimingQuick

void CTabBoundary::OnRadioTimingQuick() 
{
	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnDoubleClickedRadioTimingQuick

void CTabBoundary::OnDoubleClickedRadioTimingQuick() 
{
	OnRadioTimingQuick();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnSetActive

BOOL CTabBoundary::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Refresh the page
	if(	m_fValidTrackItem )
	{
		SetTrackItem( &m_TrackItem );
	}
	else
	{
		SetTrackItem( NULL );
	}

	if( m_pPropPageMgr 
	&&  m_pPropPageMgr->m_pTrackMgr 
	&&  m_pPropPageMgr->m_pIPropSheet )
	{
		if( m_pPropPageMgr->m_pTrackMgr->m_fGetActiveTab )
		{
			m_pPropPageMgr->m_pIPropSheet->GetActivePage( &CPropPageMgrItem::sm_nActiveTab );
		}
	}
	
	return CPropertyPage::OnSetActive();
}
