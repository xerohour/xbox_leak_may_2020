// DlgSecondaryStart.cpp : implementation file
//

#include "stdafx.h"
#include <dmusici.h>
#include "DlgSecondaryStart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSecondaryStart dialog


DlgSecondaryStart::DlgSecondaryStart(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSecondaryStart::IDD, pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//{{AFX_DATA_INIT(DlgSecondaryStart)
	//}}AFX_DATA_INIT

	m_dwSegmentFlags = 0;
}


void DlgSecondaryStart::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgSecondaryStart)
	DDX_Control(pDX, IDC_CHECK_BOUNDARY, m_checkBoundary);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_TIME, m_radioSwitchAnyTime);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_GRID, m_radioSwitchAnyGrid);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_BEAT, m_radioSwitchAnyBeat);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_BAR, m_radioSwitchAnyBar);
	DDX_Control(pDX, IDC_STATIC_NO_MARKERS, m_staticNoMarkers);
	DDX_Control(pDX, IDC_ALIGN_OPTIONS_PROMPT, m_staticAlignPrompt);
	DDX_Control(pDX, IDC_COMBO_BOUNDARY, m_comboBoundary);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSecondaryStart, CDialog)
	//{{AFX_MSG_MAP(DlgSecondaryStart)
	ON_BN_CLICKED(IDC_CHECK_BOUNDARY, OnCheckBoundary)
	ON_CBN_SELCHANGE(IDC_COMBO_BOUNDARY, OnSelchangeComboBoundary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgSecondaryStart message handlers

BOOL DlgSecondaryStart::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::OnInitDialog();
	
	// Set the 'Controlling' button state
	CheckDlgButton( IDC_CONTROL, (m_dwSegmentFlags & DMUS_SEGF_CONTROL) ? BST_CHECKED : BST_UNCHECKED );
	
	// Set the 'Don't Cutoff' radio button state
	int nRadioButton = IDC_RADIO_INVALIDATE;
	if( m_dwSegmentFlags & DMUS_SEGF_INVALIDATE_PRI )
	{
		nRadioButton = IDC_RADIO_INVALIDATEPRI;
	}
	else if( m_dwSegmentFlags & DMUS_SEGF_NOINVALIDATE )
	{
		nRadioButton = IDC_RADIO_NOINVALIDATE;
	}
	CheckRadioButton( IDC_RADIO_NOINVALIDATE, IDC_RADIO_INVALIDATE, nRadioButton );

	// Set the Quick Response/After Prepare Time radio button state
	CheckRadioButton( IDC_RADIO_TIMING_QUICK, IDC_RADIO_TIMING_AFTERPREPARETIME, (m_dwSegmentFlags & DMUS_SEGF_AFTERPREPARETIME) ? IDC_RADIO_TIMING_AFTERPREPARETIME : IDC_RADIO_TIMING_QUICK );

	// Set the Immediate/Grid/Beat/Bar/Default radio button state
	if( m_dwSegmentFlags & DMUS_SEGF_ALIGN )
	{
		m_staticAlignPrompt.EnableWindow( TRUE );
		m_radioSwitchAnyTime.EnableWindow( TRUE );
		m_radioSwitchAnyGrid.EnableWindow( TRUE );
		m_radioSwitchAnyBeat.EnableWindow( TRUE );
		m_radioSwitchAnyBar.EnableWindow( TRUE );

		if( m_dwSegmentFlags & DMUS_SEGF_BEAT )
		{
			m_comboBoundary.SetCurSel( 5 );
		}
		else if( m_dwSegmentFlags & DMUS_SEGF_MEASURE )
		{
			m_comboBoundary.SetCurSel( 6 );
		}
		else if( m_dwSegmentFlags & DMUS_SEGF_SEGMENTEND )
		{
			m_comboBoundary.SetCurSel( 7 );
		}

		if( m_dwSegmentFlags & DMUS_SEGF_VALID_START_GRID )
		{
			CheckRadioButton( IDC_RADIO_SWITCH_ANY_TIME, IDC_RADIO_SWITCH_ANY_BAR, IDC_RADIO_SWITCH_ANY_GRID );
		}
		else if( m_dwSegmentFlags & DMUS_SEGF_VALID_START_BEAT )
		{
			CheckRadioButton( IDC_RADIO_SWITCH_ANY_TIME, IDC_RADIO_SWITCH_ANY_BAR, IDC_RADIO_SWITCH_ANY_BEAT );
		}
		else if( m_dwSegmentFlags & DMUS_SEGF_VALID_START_MEASURE )
		{
			CheckRadioButton( IDC_RADIO_SWITCH_ANY_TIME, IDC_RADIO_SWITCH_ANY_BAR, IDC_RADIO_SWITCH_ANY_BAR );
		}
		else //if( m_dwSegmentFlags & DMUS_SEGF_VALID_START_TICK )
		{
			CheckRadioButton( IDC_RADIO_SWITCH_ANY_TIME, IDC_RADIO_SWITCH_ANY_BAR, IDC_RADIO_SWITCH_ANY_TIME );
		}
	}
	else
	{
		m_staticAlignPrompt.EnableWindow( FALSE );
		m_radioSwitchAnyTime.SetCheck( 0 );
		m_radioSwitchAnyTime.EnableWindow( FALSE );
		m_radioSwitchAnyGrid.SetCheck( 0 );
		m_radioSwitchAnyGrid.EnableWindow( FALSE );
		m_radioSwitchAnyBeat.SetCheck( 0 );
		m_radioSwitchAnyBeat.EnableWindow( FALSE );
		m_radioSwitchAnyBar.SetCheck( 0 );
		m_radioSwitchAnyBar.EnableWindow( FALSE );

		if( m_dwSegmentFlags & DMUS_SEGF_GRID )
		{
			m_comboBoundary.SetCurSel( 2 );
		}
		else if( m_dwSegmentFlags & DMUS_SEGF_BEAT )
		{
			m_comboBoundary.SetCurSel( 3 );
		}
		else if( m_dwSegmentFlags & DMUS_SEGF_MEASURE )
		{
			m_comboBoundary.SetCurSel( 4 );
		}
		else if( m_dwSegmentFlags & DMUS_SEGF_SEGMENTEND )
		{
			m_comboBoundary.SetCurSel( 8 );
		}
		else if( m_dwSegmentFlags & DMUS_SEGF_QUEUE )
		{
			m_comboBoundary.SetCurSel( 9 );
		}
		else if( m_dwSegmentFlags & DMUS_SEGF_DEFAULT )
		{
			m_comboBoundary.SetCurSel( 0 );
		}
		else
		{
			// Immediate
			m_comboBoundary.SetCurSel( 1 );
		}
	}
		
	if( (m_dwSegmentFlags & DMUS_SEGF_ALIGN)
	||	(m_dwSegmentFlags & DMUS_SEGF_QUEUE) )
	{
		// Set the 'marker' static text and button state
		m_checkBoundary.SetCheck( BST_UNCHECKED );
		m_staticNoMarkers.ShowWindow( SW_HIDE );
		m_checkBoundary.EnableWindow( FALSE );
		m_dwSegmentFlags &= ~DMUS_SEGF_MARKER;
	}
	else
	{
		// Set the 'marker' static text and button state
		m_checkBoundary.SetCheck( (m_dwSegmentFlags & DMUS_SEGF_MARKER) ? BST_CHECKED : BST_UNCHECKED );
		m_staticNoMarkers.ShowWindow( (m_dwSegmentFlags & DMUS_SEGF_MARKER) ? SW_SHOW : SW_HIDE );
		m_checkBoundary.EnableWindow( TRUE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DlgSecondaryStart::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Get the 'Controlling' button state
	m_dwSegmentFlags = IsDlgButtonChecked( IDC_CONTROL ) ? DMUS_SEGF_CONTROL : 0;
	
	// Get the 'Don't Cutoff' radio button states
	switch( GetCheckedRadioButton( IDC_RADIO_NOINVALIDATE, IDC_RADIO_INVALIDATE ) )
	{
	case IDC_RADIO_NOINVALIDATE:
		m_dwSegmentFlags |= DMUS_SEGF_NOINVALIDATE;
		break;
	case IDC_RADIO_INVALIDATEPRI:
		m_dwSegmentFlags |= DMUS_SEGF_INVALIDATE_PRI;
		break;
	case IDC_RADIO_INVALIDATE:
		// Do nothing
		break;
	}

	// Get the Quick Response/After Prepare Time radio button state
	m_dwSegmentFlags |= IsDlgButtonChecked( IDC_RADIO_TIMING_AFTERPREPARETIME ) ? DMUS_SEGF_AFTERPREPARETIME : 0;

	// Get the 'marker' button state
	m_dwSegmentFlags |= IsDlgButtonChecked( IDC_CHECK_BOUNDARY ) ? DMUS_SEGF_MARKER : 0;

	// Get the boundary flags
	switch( m_comboBoundary.GetCurSel() )
	{
	case 0:
		m_dwSegmentFlags |= DMUS_SEGF_DEFAULT;
		break;
	case 1:
		// Do nothing
		//m_dwSegmentFlags |= 0;
		break;
	case 2:
		m_dwSegmentFlags |= DMUS_SEGF_GRID;
		break;
	case 3:
		m_dwSegmentFlags |= DMUS_SEGF_BEAT;
		break;
	case 4:
		m_dwSegmentFlags |= DMUS_SEGF_MEASURE;
		break;
	case 5:
		m_dwSegmentFlags |= DMUS_SEGF_ALIGN | DMUS_SEGF_BEAT;
		break;
	case 6:
		m_dwSegmentFlags |= DMUS_SEGF_ALIGN | DMUS_SEGF_MEASURE;
		break;
	case 7:
		m_dwSegmentFlags |= DMUS_SEGF_ALIGN | DMUS_SEGF_SEGMENTEND;
		break;
	case 8:
		m_dwSegmentFlags |= DMUS_SEGF_SEGMENTEND;
		break;
	case 9:
		m_dwSegmentFlags |= DMUS_SEGF_QUEUE;
		break;
	default:
		break;
	}

	// Get default alignment flags
	if( m_dwSegmentFlags & DMUS_SEGF_ALIGN )
	{
		// Check radio buttons for 'Switch' points
		if( m_radioSwitchAnyTime.GetCheck() )
		{
			m_dwSegmentFlags |= DMUS_SEGF_VALID_START_TICK;
		}
		else if( m_radioSwitchAnyGrid.GetCheck() )
		{
			m_dwSegmentFlags |= DMUS_SEGF_VALID_START_GRID;
		}
		else if( m_radioSwitchAnyBeat.GetCheck() )
		{
			m_dwSegmentFlags |= DMUS_SEGF_VALID_START_BEAT;
		}
		else if( m_radioSwitchAnyBar.GetCheck() )
		{
			m_dwSegmentFlags |= DMUS_SEGF_VALID_START_MEASURE;
		}
	}

	CDialog::OnOK();
}

void DlgSecondaryStart::OnCheckBoundary() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( IsDlgButtonChecked( IDC_CHECK_BOUNDARY ) )
	{
		m_dwSegmentFlags |= DMUS_SEGF_MARKER;
	}
	else
	{
		m_dwSegmentFlags &= ~DMUS_SEGF_MARKER;
	}

	m_staticNoMarkers.ShowWindow( IsDlgButtonChecked( IDC_CHECK_BOUNDARY ) ? SW_SHOW : SW_HIDE );
}

void DlgSecondaryStart::OnSelchangeComboBoundary() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Get the boundary flags
	const int nSel = m_comboBoundary.GetCurSel();
	switch( nSel )
	{
	case 5:		// "Align to Beat"
	case 6:		// "Align to Bar"
	case 7:		// "Align to Segment"
		// Enable controls that set set switch points
		m_staticAlignPrompt.EnableWindow( TRUE );
		m_radioSwitchAnyBeat.EnableWindow( TRUE );
		m_radioSwitchAnyBar.EnableWindow( TRUE );
		m_radioSwitchAnyTime.EnableWindow( TRUE );
		m_radioSwitchAnyGrid.EnableWindow( TRUE );

		// Make sure one of the buttons is checked
		if( !m_radioSwitchAnyTime.GetCheck() 
		&&  !m_radioSwitchAnyGrid.GetCheck() 
		&&  !m_radioSwitchAnyBeat.GetCheck()
		&&  !m_radioSwitchAnyBar.GetCheck() )
		{
			m_radioSwitchAnyTime.SetCheck( BST_CHECKED );
		}
		break;

	default:
		// Disable controls that set set switch points
		m_staticAlignPrompt.EnableWindow( FALSE );
		m_radioSwitchAnyTime.SetCheck( 0 );
		m_radioSwitchAnyTime.EnableWindow( FALSE );
		m_radioSwitchAnyGrid.SetCheck( 0 );
		m_radioSwitchAnyGrid.EnableWindow( FALSE );
		m_radioSwitchAnyBeat.SetCheck( 0 );
		m_radioSwitchAnyBeat.EnableWindow( FALSE );
		m_radioSwitchAnyBar.SetCheck( 0 );
		m_radioSwitchAnyBar.EnableWindow( FALSE );
		break;
	}

	if( ((nSel >= 5) && (nSel <= 7))
	||	(nSel == 9) )
	{
		// Disable the 'Start at next Marker' checkbox
		m_checkBoundary.SetCheck( BST_UNCHECKED );
		m_staticNoMarkers.ShowWindow( SW_HIDE );
		m_checkBoundary.EnableWindow( FALSE );

	}
	else
	{
		// Enable the 'Start at next Marker' checkbox
		m_checkBoundary.SetCheck( (m_dwSegmentFlags & DMUS_SEGF_MARKER) ? BST_CHECKED : BST_UNCHECKED );
		m_staticNoMarkers.ShowWindow( (m_dwSegmentFlags & DMUS_SEGF_MARKER) ? SW_SHOW : SW_HIDE );
		m_checkBoundary.EnableWindow( TRUE );
	}
}
