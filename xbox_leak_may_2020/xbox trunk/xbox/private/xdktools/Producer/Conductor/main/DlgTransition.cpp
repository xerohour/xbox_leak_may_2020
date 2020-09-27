// DlgTransition.cpp : implementation file
//

#include "stdafx.h"
#include "cconduct.h"
#include "DlgTransition.h"
#include "Toolbar.h"
#include "TREntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_EMB_CUSTOM_ID	100
#define MAX_EMB_CUSTOM_ID	199

/////////////////////////////////////////////////////////////////////////////
// DlgTransition dialog


DlgTransition::DlgTransition(CWnd* pParent /*=NULL*/)
	: CDialog(DlgTransition::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgTransition)
	//}}AFX_DATA_INIT
}


void DlgTransition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgTransition)
	DDX_Control(pDX, IDC_CHECK_TRANS_NOTHING, m_buttonTransNothing);
	DDX_Control(pDX, IDC_RADIO_AFTER_PREPARE, m_radioAfterPrepare);
	DDX_Control(pDX, IDC_RADIO_QUICK_RESPONSE, m_radioQuickResponse);
	DDX_Control(pDX, IDC_CHECK_SEG_DEFAULT, m_buttonSegDefault);
	DDX_Control(pDX, IDC_COMBO_TRANS_SEGMENT, m_comboSegment);
	DDX_Control(pDX, IDC_STATIC_NO_MARKERS, m_staticNoMarkers);
	DDX_Control(pDX, IDC_CHECK_BOUNDARY, m_buttonBoundary);
	DDX_Control(pDX, IDC_ALIGN_OPTIONS_PROMPT, m_staticAlignPrompt);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_TIME, m_radioSwitchAnyTime);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_GRID, m_radioSwitchAnyGrid);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_BEAT, m_radioSwitchAnyBeat);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ANY_BAR, m_radioSwitchAnyBar);
	DDX_Control(pDX, IDC_CUSTOM_PROMPT, m_staticCustomIdPrompt);
	DDX_Control(pDX, IDC_RADIO_ENTIRE_PATTERN, m_radioEntirePattern);
	DDX_Control(pDX, IDC_RADIO_ENTIRE_ADDITIONAL, m_radioEntireAdditional);
	DDX_Control(pDX, IDC_RADIO_ONE_BAR_PATTERN, m_radio1BarPattern);
	DDX_Control(pDX, IDC_RADIO_ONE_BAR_ADDITIONAL, m_radio1BarAdditional);
	DDX_Control(pDX, IDC_CUSTOM_ID_SPIN, m_spinCustomId);
	DDX_Control(pDX, IDC_CUSTOM_ID, m_editCustomId);
	DDX_Control(pDX, IDC_COMBO_PATTERN, m_comboPattern);
	DDX_Control(pDX, IDC_COMBO_BOUNDARY, m_comboBoundary);
	DDX_Control(pDX, IDC_CHECK_MODULATE, m_buttonModulate);
	DDX_Control(pDX, IDC_CHECK_LONG, m_buttonLong);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgTransition, CDialog)
	//{{AFX_MSG_MAP(DlgTransition)
	ON_BN_CLICKED(IDC_CHECK_LONG, OnCheckLong)
	ON_CBN_SELCHANGE(IDC_COMBO_PATTERN, OnSelChangeComboPattern)
	ON_EN_KILLFOCUS(IDC_CUSTOM_ID, OnKillFocusCustomId)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CUSTOM_ID_SPIN, OnDeltaPosCustomIdSpin)
	ON_CBN_SELCHANGE(IDC_COMBO_BOUNDARY, OnSelChangeComboBoundary)
	ON_BN_CLICKED(IDC_CHECK_BOUNDARY, OnCheckBoundary)
	ON_CBN_SELCHANGE(IDC_COMBO_TRANS_SEGMENT, OnSelchangeComboTransSegment)
	ON_BN_CLICKED(IDC_RADIO_ENTIRE_ADDITIONAL, OnRadioEntireAdditional)
	ON_BN_CLICKED(IDC_RADIO_ENTIRE_PATTERN, OnRadioEntirePattern)
	ON_BN_CLICKED(IDC_RADIO_ONE_BAR_ADDITIONAL, OnRadioOneBarAdditional)
	ON_BN_CLICKED(IDC_RADIO_ONE_BAR_PATTERN, OnRadioOneBarPattern)
	ON_BN_CLICKED(IDC_CHECK_MODULATE, OnCheckModulate)
	ON_BN_CLICKED(IDC_CHECK_SEG_DEFAULT, OnCheckSegDefault)
	ON_BN_CLICKED(IDC_CHECK_TRANS_NOTHING, OnCheckTransNothing)
	ON_BN_CLICKED(IDC_RADIO_NOINVALIDATE, OnRadioInvalidateBtn)
	ON_BN_CLICKED(IDC_RADIO_INVALIDATEPRI, OnRadioInvalidateBtn)
	ON_BN_CLICKED(IDC_RADIO_INVALIDATE, OnRadioInvalidateBtn)
	ON_BN_CLICKED(IDC_RADIO_AFTER_PREPARE, OnRadioPrepareBtn)
	ON_BN_CLICKED(IDC_RADIO_QUICK_RESPONSE, OnRadioPrepareBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgTransition message handlers

void DlgTransition::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_TransitionOptions.dwBoundaryFlags = 0;
	m_TransitionOptions.wPatternType = 0;
	m_TransitionOptions.dwFlags = 0;

	if( m_buttonSegDefault.GetCheck() )
	{
		m_TransitionOptions.dwBoundaryFlags = DMUS_COMPOSEF_DEFAULT;
	}
	else
	{
		switch( m_comboBoundary.GetCurSel() )
		{
		case 0:
			m_TransitionOptions.dwBoundaryFlags = DMUS_COMPOSEF_IMMEDIATE;
			break;
		case 1:
			m_TransitionOptions.dwBoundaryFlags = DMUS_COMPOSEF_GRID;
			break;
		case 2:
			m_TransitionOptions.dwBoundaryFlags = DMUS_COMPOSEF_BEAT;
			break;
		case 3:
			m_TransitionOptions.dwBoundaryFlags = DMUS_COMPOSEF_MEASURE;
			break;
		case 4:
			m_TransitionOptions.dwBoundaryFlags = (DMUS_COMPOSEF_ALIGN | DMUS_COMPOSEF_BEAT);
			break;
		case 5:
			m_TransitionOptions.dwBoundaryFlags = (DMUS_COMPOSEF_ALIGN | DMUS_COMPOSEF_MEASURE);
			break;
		case 6:
			m_TransitionOptions.dwBoundaryFlags = (DMUS_COMPOSEF_ALIGN | DMUS_COMPOSEF_SEGMENTEND);
			break;
		case 7:
			m_TransitionOptions.dwBoundaryFlags = DMUS_COMPOSEF_SEGMENTEND;
			break;
		case 8:
			m_TransitionOptions.dwFlags = TRANS_END_OF_SEGMENT; // End of Segment Queue
			break;
		default:
			break;
		}

		switch( GetCheckedRadioButton( IDC_RADIO_NOINVALIDATE, IDC_RADIO_INVALIDATE ) )
		{
		case IDC_RADIO_NOINVALIDATE:
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_NOINVALIDATE;
			break;
		case IDC_RADIO_INVALIDATEPRI:
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_INVALIDATE_PRI;
			break;
		case IDC_RADIO_INVALIDATE:
			// Do nothing
			break;
		}

		if( m_radioAfterPrepare.GetCheck() )
		{
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_AFTERPREPARETIME;
		}
	}

	if( !(m_TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT) )
	{
		switch( m_comboPattern.GetCurSel() )
		{
		int nCustomId;

		case 0:
			m_TransitionOptions.wPatternType = DMUS_COMMANDT_INTRO;
			break;
		case 1:
			m_TransitionOptions.wPatternType = DMUS_COMMANDT_BREAK;
			break;
		case 2:
			m_TransitionOptions.wPatternType = DMUS_COMMANDT_FILL;
			break;
		case 3:
			m_TransitionOptions.wPatternType = DMUS_COMMANDT_END;
			break;
		case 4:
			nCustomId = m_spinCustomId.GetPos();
			// If ASSERT fires, validation in control handlers is broken
			ASSERT( (nCustomId >= MIN_EMB_CUSTOM_ID)  &&  (nCustomId <= MAX_EMB_CUSTOM_ID) );
			m_TransitionOptions.wPatternType = (WORD)nCustomId;
			break;
		case 5:
			m_TransitionOptions.wPatternType = DMUS_COMMANDT_ENDANDINTRO;
			break;
		case 6:
			m_TransitionOptions.dwFlags |= TRANS_REGULAR_PATTERN;
			break;
		case 7:
			m_TransitionOptions.dwFlags |= TRANS_NO_TRANSITION;
			break;
		default:
			break;
		}
	}
	else
	{
		m_TransitionOptions.dwFlags |= TRANS_NO_TRANSITION;
	}

	// If doing a pattern transition, always set DMUS_COMPOSEF_USE_AUDIOPATH
	if( 0 == (m_TransitionOptions.dwFlags & TRANS_NO_TRANSITION) )
	{
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_USE_AUDIOPATH;
	}

	// Default to no transition segment
	m_TransitionOptions.pDMUSProdNodeSegmentTransition = NULL;

	if( !(m_TransitionOptions.dwFlags & TRANS_NO_TRANSITION) )
	{
		// Check radio buttons for 'Transition' pattern
		if( m_radioEntirePattern.GetCheck() )
		{
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_ENTIRE_TRANSITION;
		}
		else if( m_radio1BarPattern.GetCheck() )
		{
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_1BAR_TRANSITION;
		}

		if( m_buttonLong.GetCheck() )
		{
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_LONG;

			// Check radio buttons for 'Additional' pattern
			if( m_radioEntireAdditional.GetCheck() )
			{
				m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_ENTIRE_ADDITION;
			}
			else if( m_radio1BarAdditional.GetCheck() )
			{
				m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_1BAR_ADDITION;
			}
		}

		if( m_buttonModulate.GetCheck() )
		{
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_MODULATE;
		}

		if( m_buttonTransNothing.GetCheck() )
		{
			m_TransitionOptions.dwFlags |= TRANS_TO_NOTHING;
		}
	}
	else
	{
		// Check for a transition segment
		int nCurSel = m_comboSegment.GetCurSel();
		if( nCurSel != CB_ERR )
		{
			ITransportRegEntry *pDisplayedRegEntry = static_cast<ITransportRegEntry *>(m_comboSegment.GetItemDataPtr( nCurSel ));
			if( pDisplayedRegEntry != NULL )
			{
				IDMUSProdTransport*  pTmpTransport;
				IDMUSProdNode* pTmpDMUSProdNode;
				pDisplayedRegEntry->GetTransport( &pTmpTransport );
				if( SUCCEEDED( pTmpTransport->QueryInterface( IID_IDMUSProdNode, (void **)&pTmpDMUSProdNode ) ) )
				{
					IDirectMusicSegment *pTmpDirectMusicSegment;
					if( SUCCEEDED( pTmpDMUSProdNode->GetObject( CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pTmpDirectMusicSegment ) ) )
					{
						m_TransitionOptions.pDMUSProdNodeSegmentTransition = pTmpDMUSProdNode;

						pTmpDirectMusicSegment->Release();
					}

					pTmpDMUSProdNode->Release();
				}
			}
		}
	}

	if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN )
	{
		// Check radio buttons for 'Switch' points
		if( m_radioSwitchAnyTime.GetCheck() )
		{
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_VALID_START_TICK;
		}
		else if( m_radioSwitchAnyGrid.GetCheck() )
		{
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_VALID_START_GRID;
		}
		else if( m_radioSwitchAnyBeat.GetCheck() )
		{
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_VALID_START_BEAT;
		}
		else if( m_radioSwitchAnyBar.GetCheck() )
		{
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_VALID_START_MEASURE;
		}
	}

	if( m_buttonBoundary.GetCheck() )
	{
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_MARKER;
	}

	CDialog::OnOK();
}

BOOL DlgTransition::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::OnInitDialog();

	m_editCustomId.LimitText( 3 );
	m_spinCustomId.SetRange( MIN_EMB_CUSTOM_ID, MAX_EMB_CUSTOM_ID );
	
	m_TransitionOptions = g_pconductor->m_TransitionOptions;
	{
		if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_IMMEDIATE )
		{
			m_comboBoundary.SetCurSel( 0 );
		}
		else if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_GRID )
		{
			m_comboBoundary.SetCurSel( 1 );
		}
		else if( (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_BEAT)
			 && !(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN) )
		{
			m_comboBoundary.SetCurSel( 2 );
		}
		else if( (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MEASURE)
			 && !(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN) )
		{
			m_comboBoundary.SetCurSel( 3 );
		}
		else if( (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_BEAT)
			 &&  (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN) )
		{
			m_comboBoundary.SetCurSel( 4 );
		}
		else if( (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MEASURE)
			 &&  (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN) )
		{
			m_comboBoundary.SetCurSel( 5 );
		}
		else if( (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_SEGMENTEND)
			 &&  (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN) )
		{
			m_comboBoundary.SetCurSel( 6 );
		}
		else if( (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_SEGMENTEND)
			 && !(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN) )
		{
			m_comboBoundary.SetCurSel( 7 );
		}
		else if( m_TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT )
		{
			m_comboBoundary.SetCurSel( 8 );
		}
		else if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT )
		{
			m_comboBoundary.SetCurSel( 3 );
		}
		else
		{
			// This shouldn't happen!
			ASSERT(0);
			m_comboBoundary.SetCurSel( 3 );
		}

		// Update the state of the pattern transition controls
		UpdateSegmentAndPatternTransitionControls();

		// Add the 'None' text to the Transition Segment combobox
		CString strText;
		if( strText.LoadString( IDS_NO_TRANS_SEGMENT ) )
		{
			int nIndex = m_comboSegment.AddString( strText );
			m_comboSegment.SetItemDataPtr( nIndex, NULL );

			// Default to having 'none' selected (if the segment can't be found in the loop below)
			m_comboSegment.SetCurSel( nIndex );
		}

		// Add the text for the other segments to the Transition Segment combobox
		if ( g_pconductor->m_pToolbarHandler )
		{
			IDMUSProdTransport*  pTmpTransport;
			IDMUSProdNode* pTmpDMUSProdNode;
			IDirectMusicSegment *pTmpDirectMusicSegment;
			POSITION pos = g_pconductor->m_lstITransportRegEntry.GetHeadPosition();
			while( pos )
			{
				ITransportRegEntry *pITransportRegEntry = g_pconductor->m_lstITransportRegEntry.GetNext( pos );
				pITransportRegEntry->GetTransport( &pTmpTransport );
				if( SUCCEEDED( pTmpTransport->QueryInterface( IID_IDMUSProdNode, (void **)&pTmpDMUSProdNode ) ) )
				{
					if( SUCCEEDED( pTmpDMUSProdNode->GetObject( CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pTmpDirectMusicSegment ) ) )
					{
						BSTR  bstrName;
						CString strName;
						// If the transport doens't have a name, use a default name
						if ( SUCCEEDED(	pTmpTransport->GetName( &bstrName ) ) )
						{
							strName = bstrName;
							SysFreeString( bstrName );
						}
						else
						{
							strName.LoadString( IDS_DEFAULT_TRANSPORT_NAME );
						}
						int nIndex = m_comboSegment.AddString( strName );
						m_comboSegment.SetItemDataPtr( nIndex, pITransportRegEntry );

						if( m_TransitionOptions.pDMUSProdNodeSegmentTransition == pTmpDMUSProdNode )
						{
							m_comboSegment.SetCurSel( nIndex );
						}

						pTmpDirectMusicSegment->Release();
					}

					pTmpDMUSProdNode->Release();
				}
			}
		}

		if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN )
		{
			m_staticAlignPrompt.EnableWindow( TRUE );
			m_radioSwitchAnyTime.EnableWindow( TRUE );
			m_radioSwitchAnyGrid.EnableWindow( TRUE );
			m_radioSwitchAnyBar.EnableWindow( TRUE );
			m_radioSwitchAnyBeat.EnableWindow( TRUE );

			if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_GRID )
			{
				CheckRadioButton( IDC_RADIO_SWITCH_ANY_TIME, IDC_RADIO_SWITCH_ANY_BAR, IDC_RADIO_SWITCH_ANY_GRID );
			}
			else if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_BEAT )
			{
				CheckRadioButton( IDC_RADIO_SWITCH_ANY_TIME, IDC_RADIO_SWITCH_ANY_BAR, IDC_RADIO_SWITCH_ANY_BEAT );
			}
			else if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_MEASURE )
			{
				CheckRadioButton( IDC_RADIO_SWITCH_ANY_TIME, IDC_RADIO_SWITCH_ANY_BAR, IDC_RADIO_SWITCH_ANY_BAR );
			}
			else //if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_TICK )
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
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DlgTransition::OnCheckLong() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_buttonLong.GetCheck() )
	{
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_LONG | DMUS_COMPOSEF_ENTIRE_ADDITION;
		m_TransitionOptions.dwBoundaryFlags &= ~DMUS_COMPOSEF_1BAR_ADDITION;

		// Enable radio buttons for the 'Additional' pattern
		m_radioEntireAdditional.EnableWindow( TRUE );
		m_radioEntireAdditional.SetCheck( 1 );
		m_radio1BarAdditional.EnableWindow( TRUE );
		m_radio1BarAdditional.SetCheck( 0 );
	}
	else
	{
		m_TransitionOptions.dwBoundaryFlags &= ~DMUS_COMPOSEF_LONG;

		// Disable radio buttons for the 'Additional' pattern
		m_radioEntireAdditional.SetCheck( 0 );
		m_radioEntireAdditional.EnableWindow( FALSE );
		m_radio1BarAdditional.SetCheck( 0 );
		m_radio1BarAdditional.EnableWindow( FALSE );
	}
}

void DlgTransition::OnSelChangeComboPattern() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_TransitionOptions.dwFlags &= ~(TRANS_REGULAR_PATTERN | TRANS_NO_TRANSITION);
	m_TransitionOptions.wPatternType = 0;

	switch( m_comboPattern.GetCurSel() )
	{
	case 0:
		m_TransitionOptions.wPatternType = DMUS_COMMANDT_INTRO;
		break;
	case 1:
		m_TransitionOptions.wPatternType = DMUS_COMMANDT_BREAK;
		break;
	case 2:
		m_TransitionOptions.wPatternType = DMUS_COMMANDT_FILL;
		break;
	case 3:
		m_TransitionOptions.wPatternType = DMUS_COMMANDT_END;
		break;
	case 4:
		m_TransitionOptions.wPatternType = MIN_EMB_CUSTOM_ID;
		break;
	case 5:
		m_TransitionOptions.wPatternType = DMUS_COMMANDT_ENDANDINTRO;
		break;
	case 6:
		m_TransitionOptions.dwFlags |= TRANS_REGULAR_PATTERN;
		break;
	case 7:
		m_TransitionOptions.dwFlags |= TRANS_NO_TRANSITION;
		break;
	default:
		break;
	}

	// Enable/Disable the controls appropriately
	UpdateSegmentAndPatternTransitionControls();
}

void DlgTransition::OnKillFocusCustomId() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strNewCustomId;

	m_editCustomId.GetWindowText( strNewCustomId );

	// Strip leading and trailing spaces
	strNewCustomId.TrimRight();
	strNewCustomId.TrimLeft();

	if( strNewCustomId.IsEmpty() )
	{
		m_spinCustomId.SetPos( MIN_EMB_CUSTOM_ID );
		m_TransitionOptions.wPatternType = MIN_EMB_CUSTOM_ID;
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
		m_TransitionOptions.wPatternType = WORD(nNewCustomId);
	}
}

void DlgTransition::OnDeltaPosCustomIdSpin( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

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
	}
	
	*pResult = 1;
}

void DlgTransition::OnSelChangeComboBoundary() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_TransitionOptions.dwBoundaryFlags &= ~(DMUS_COMPOSEF_IMMEDIATE|DMUS_COMPOSEF_GRID|DMUS_COMPOSEF_BEAT|DMUS_COMPOSEF_MEASURE|DMUS_COMPOSEF_SEGMENTEND|DMUS_COMPOSEF_ALIGN);
	m_TransitionOptions.dwFlags &= ~TRANS_END_OF_SEGMENT;

	switch( m_comboBoundary.GetCurSel() )
	{
	case 0:
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_IMMEDIATE;
		break;
	case 1:
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_GRID;
		break;
	case 2:
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_BEAT;
		break;
	case 3:
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_MEASURE;
		break;
	case 4:
		m_TransitionOptions.dwBoundaryFlags |= (DMUS_COMPOSEF_ALIGN | DMUS_COMPOSEF_BEAT);
		break;
	case 5:
		m_TransitionOptions.dwBoundaryFlags |= (DMUS_COMPOSEF_ALIGN | DMUS_COMPOSEF_MEASURE);
		break;
	case 6:
		m_TransitionOptions.dwBoundaryFlags |= (DMUS_COMPOSEF_ALIGN | DMUS_COMPOSEF_SEGMENTEND);
		break;
	case 7:
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_SEGMENTEND;
		break;
	case 8:
		m_TransitionOptions.dwFlags |= TRANS_END_OF_SEGMENT | TRANS_NO_TRANSITION;
		m_TransitionOptions.dwFlags &= ~TRANS_REGULAR_PATTERN;
		m_TransitionOptions.wPatternType = 0;
		break;
	}

	// Update pattern controls
	UpdateSegmentAndPatternTransitionControls();
}

void DlgTransition::OnCheckBoundary() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_buttonBoundary.GetCheck() )
	{
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_MARKER;
	}
	else
	{
		m_TransitionOptions.dwBoundaryFlags &= ~DMUS_COMPOSEF_MARKER;
	}

	m_staticNoMarkers.ShowWindow( (m_buttonBoundary.GetCheck()) ? SW_SHOW : SW_HIDE );
}

void DlgTransition::OnSelchangeComboTransSegment() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nCurSel = m_comboSegment.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		ITransportRegEntry *pDisplayedRegEntry = static_cast<ITransportRegEntry *>(m_comboSegment.GetItemDataPtr( nCurSel ));

		if( pDisplayedRegEntry == NULL )
		{
			m_TransitionOptions.pDMUSProdNodeSegmentTransition = NULL;
		}
		else
		{
			IDMUSProdTransport*  pTmpTransport;
			IDMUSProdNode* pTmpDMUSProdNode;
			pDisplayedRegEntry->GetTransport( &pTmpTransport );
			if( SUCCEEDED( pTmpTransport->QueryInterface( IID_IDMUSProdNode, (void **)&pTmpDMUSProdNode ) ) )
			{
				IDirectMusicSegment *pTmpDirectMusicSegment;
				if( SUCCEEDED( pTmpDMUSProdNode->GetObject( CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pTmpDirectMusicSegment ) ) )
				{
					m_TransitionOptions.pDMUSProdNodeSegmentTransition = pTmpDMUSProdNode;

					pTmpDirectMusicSegment->Release();
				}

				pTmpDMUSProdNode->Release();
			}
		}

		// Enable/Disable the controls appropriately
		UpdateSegmentAndPatternTransitionControls();
	}
}

void DlgTransition::UpdateSegmentAndPatternTransitionControls( void )
{
	BOOL fEnablePatternTransition = FALSE;

	if( !(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT)
	&&	(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN)
	&&	(m_TransitionOptions.dwBoundaryFlags & (DMUS_COMPOSEF_BEAT | DMUS_COMPOSEF_MEASURE)) )
	{
		m_comboSegment.EnableWindow( FALSE );

		for( int i=m_comboSegment.GetCount() - 1; i >=0; i-- )
		{
			if( m_comboSegment.GetItemDataPtr( i ) == NULL )
			{
				m_comboSegment.SetCurSel( i );
				break;
			}
		}

		m_TransitionOptions.pDMUSProdNodeSegmentTransition = NULL;
		m_TransitionOptions.wPatternType = 0;
		m_TransitionOptions.dwFlags |= TRANS_NO_TRANSITION;
		m_TransitionOptions.dwFlags &= ~TRANS_REGULAR_PATTERN;
	}
	else
	{
		m_comboSegment.EnableWindow( (m_TransitionOptions.dwFlags & (TRANS_END_OF_SEGMENT | TRANS_NO_TRANSITION)) ? TRUE : FALSE );

		if( !m_TransitionOptions.pDMUSProdNodeSegmentTransition
		&&	!(m_TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT) )
		{
			// Not using a transition segment or transition on queue boundary - enable all available settings
			fEnablePatternTransition = TRUE;
		}
	}

	//
	// Enable/Disable the custom embellishment controls and pattern combobox
	//
	if( fEnablePatternTransition
	&&	m_TransitionOptions.wPatternType >= MIN_EMB_CUSTOM_ID
	&&  m_TransitionOptions.wPatternType <= MAX_EMB_CUSTOM_ID )
	{
		// Transition uses a custom embellishment pattern
		m_staticCustomIdPrompt.EnableWindow( TRUE );
		m_editCustomId.EnableWindow( TRUE );
		m_spinCustomId.EnableWindow( TRUE );
		m_spinCustomId.SetPos( m_TransitionOptions.wPatternType );

		// Set 'Transition Pattern' combo box selection to "Custom" 
		m_comboPattern.EnableWindow( TRUE );
		m_comboPattern.SetCurSel( 4 );
	}
	else
	{		
		// Transition does not use a custom embellishment pattern
		m_spinCustomId.SetPos( MIN_EMB_CUSTOM_ID );
		m_staticCustomIdPrompt.EnableWindow( FALSE );
		m_editCustomId.EnableWindow( FALSE );
		m_spinCustomId.EnableWindow( FALSE );

		// Set 'Transition Pattern' combo box selection
		switch( m_TransitionOptions.wPatternType )
		{
		case DMUS_COMMANDT_INTRO:
			m_comboPattern.SetCurSel( 0 );
			break;
		case DMUS_COMMANDT_BREAK:
			m_comboPattern.SetCurSel( 1 );
			break;
		case DMUS_COMMANDT_FILL:
			m_comboPattern.SetCurSel( 2 );
			break;
		case DMUS_COMMANDT_END:
			m_comboPattern.SetCurSel( 3 );
			break;
		case DMUS_COMMANDT_ENDANDINTRO:
			m_comboPattern.SetCurSel( 5 );
			break;
		case DMUS_COMMANDT_GROOVE:
		default:
			if( m_TransitionOptions.dwFlags & TRANS_REGULAR_PATTERN )
			{
				m_comboPattern.SetCurSel( 6 );
			}
			else if( m_TransitionOptions.dwFlags & TRANS_NO_TRANSITION )
			{
				m_comboPattern.SetCurSel( 7 );
			}
			break;
		}

		m_comboPattern.EnableWindow( fEnablePatternTransition );
	}

	// Enable 'Segment's Default' checkbox
	m_buttonSegDefault.EnableWindow( TRUE );
	m_buttonSegDefault.SetCheck( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT ? TRUE : FALSE );

	// Enable/Disable the boundary combobox correctly
	m_comboBoundary.EnableWindow( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT ? FALSE : TRUE );

	if( !(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT)
	&&	(m_TransitionOptions.dwBoundaryFlags  & DMUS_COMPOSEF_ALIGN) )
	{
		// Enable controls that set set switch points
		m_staticAlignPrompt.EnableWindow( TRUE );
		m_radioSwitchAnyTime.EnableWindow( TRUE );
		m_radioSwitchAnyGrid.EnableWindow( TRUE );
		m_radioSwitchAnyBeat.EnableWindow( TRUE );
		m_radioSwitchAnyBar.EnableWindow( TRUE );
		// Make sure one of the buttons is checked
		if( !m_radioSwitchAnyTime.GetCheck() 
		&&  !m_radioSwitchAnyGrid.GetCheck() 
		&&  !m_radioSwitchAnyBeat.GetCheck()
		&&	!m_radioSwitchAnyBar.GetCheck() )
		{
			m_radioSwitchAnyTime.SetCheck( 1 );
		}
	}
	else
	{
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
	}

	// Enable 'Trans at next marker' checkbox
	BOOL fEnableBoundaryCheckbox = ((m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT)
								||	(!(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN) &&
									 !(m_TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT))) ? TRUE : FALSE;
	m_buttonBoundary.EnableWindow( fEnableBoundaryCheckbox );
	m_buttonBoundary.SetCheck( (fEnableBoundaryCheckbox && (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MARKER)) ? 1 : 0 );
	m_staticNoMarkers.ShowWindow( (fEnableBoundaryCheckbox && (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MARKER)) ? SW_SHOW : SW_HIDE );

	//
	// Enable/Disable the pattern transition options
	// Enable/Disable the segment transition options
	//
	if( !fEnablePatternTransition
	||	(m_TransitionOptions.dwFlags & TRANS_NO_TRANSITION) )
	{
		// Disable radio buttons for the 'Transition' pattern
		m_radioEntirePattern.SetCheck( 0 );
		m_radioEntirePattern.EnableWindow( FALSE );
		m_radio1BarPattern.SetCheck( 0 );
		m_radio1BarPattern.EnableWindow( FALSE );

		// 'Long' is not checked
		m_buttonLong.SetCheck( 0 );
		m_buttonLong.EnableWindow( FALSE );

		// Disable radio buttons for the 'Additional' pattern
		m_radioEntireAdditional.SetCheck( 0 );
		m_radioEntireAdditional.EnableWindow( FALSE );
		m_radio1BarAdditional.SetCheck( 0 );
		m_radio1BarAdditional.EnableWindow( FALSE );

		// Disable Modulate button
		m_buttonModulate.EnableWindow( FALSE );
		m_buttonModulate.SetCheck( 0 );

		// Diable 'to nothing' button
		m_buttonTransNothing.EnableWindow( FALSE );
		m_buttonTransNothing.SetCheck( 0 );
	}
	else
	{
		// Make sure one of the 'Transition' pattern radio buttons will be checked
		if( !(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ENTIRE_TRANSITION)
		&&  !(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_1BAR_TRANSITION) )
		{
			m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_ENTIRE_TRANSITION;
		}

		// Enable radio buttons for the 'Transition' pattern
		m_radioEntirePattern.EnableWindow( TRUE );
		m_radioEntirePattern.SetCheck( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ENTIRE_TRANSITION );
		m_radio1BarPattern.EnableWindow( TRUE );
		m_radio1BarPattern.SetCheck( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_1BAR_TRANSITION );

		// Enable the 'Long' checkbox
		m_buttonLong.EnableWindow( TRUE );

		if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_LONG )
		{
			// 'Long' is checked
			m_buttonLong.SetCheck( 1 );

			// Make sure one of the 'Additional' pattern radio buttons will be checked
			if( !(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ENTIRE_ADDITION)
			&&  !(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_1BAR_ADDITION) )
			{
				m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_ENTIRE_ADDITION;
			}

			// Enable radio buttons for the 'Additional' pattern
			m_radioEntireAdditional.EnableWindow( TRUE );
			m_radioEntireAdditional.SetCheck( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ENTIRE_ADDITION );
			m_radio1BarAdditional.EnableWindow( TRUE );
			m_radio1BarAdditional.SetCheck( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_1BAR_ADDITION );
		}
		else
		{
			// 'Long' is not checked
			m_buttonLong.SetCheck( 0 );

			// Disable radio buttons for the 'Additional' pattern
			m_radioEntireAdditional.SetCheck( 0 );
			m_radioEntireAdditional.EnableWindow( FALSE );
			m_radio1BarAdditional.SetCheck( 0 );
			m_radio1BarAdditional.EnableWindow( FALSE );
		}

		// Enable the 'Modulate' checkbox
		m_buttonModulate.SetCheck( (m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MODULATE) ? 1 : 0 );
		m_buttonModulate.EnableWindow( TRUE );

		if( m_TransitionOptions.wPatternType == DMUS_COMMANDT_END )
		{
			// Enable the 'to nothing' checkbox
			m_buttonTransNothing.SetCheck( (m_TransitionOptions.dwFlags & TRANS_TO_NOTHING) ? 1: 0 );
			m_buttonTransNothing.EnableWindow( TRUE );
		}
		else
		{
			// Diable 'to nothing' button
			m_buttonTransNothing.EnableWindow( FALSE );
			m_buttonTransNothing.SetCheck( 0 );
		}
	}

	// If "Use segment's default" is not checked (Added to fix bug 52362)
	if( !(m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT) )
	{
		// Enable the "response" and "cut off" radio buttons
		m_radioQuickResponse.EnableWindow( TRUE );
		m_radioAfterPrepare.EnableWindow( TRUE );
		m_radioAfterPrepare.SetCheck( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_AFTERPREPARETIME ? 1 : 0 );
		m_radioQuickResponse.SetCheck( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_AFTERPREPARETIME ? 0 : 1 );


		// Initialize 'Don't Cut Off' radio buttons
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_RADIO_NOINVALIDATE ), TRUE );
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_RADIO_INVALIDATEPRI ), TRUE );
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_RADIO_INVALIDATE ), TRUE );
		int nRadioButton = IDC_RADIO_INVALIDATE;
		if( m_TransitionOptions.dwBoundaryFlags  & DMUS_COMPOSEF_INVALIDATE_PRI )
		{
			nRadioButton = IDC_RADIO_INVALIDATEPRI;
		}
		else if( m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_NOINVALIDATE )
		{
			nRadioButton = IDC_RADIO_NOINVALIDATE;
		}
		CheckRadioButton( IDC_RADIO_NOINVALIDATE, IDC_RADIO_INVALIDATE, nRadioButton );
	}
	else
	{
		// Disable the "response" and "cut off" radio buttons
		m_radioQuickResponse.EnableWindow( FALSE );
		m_radioQuickResponse.SetCheck( 0 );
		m_radioAfterPrepare.EnableWindow( FALSE );
		m_radioAfterPrepare.SetCheck( 0 );
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_RADIO_NOINVALIDATE ), FALSE );
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_RADIO_INVALIDATEPRI ), FALSE );
		::EnableWindow( ::GetDlgItem( GetSafeHwnd(), IDC_RADIO_INVALIDATE ), FALSE );
		::SendMessage( ::GetDlgItem( GetSafeHwnd(), IDC_RADIO_NOINVALIDATE ), BM_SETCHECK, 0, 0 );
		::SendMessage( ::GetDlgItem( GetSafeHwnd(), IDC_RADIO_INVALIDATEPRI ), BM_SETCHECK, 0, 0 );
		::SendMessage( ::GetDlgItem( GetSafeHwnd(), IDC_RADIO_INVALIDATE ), BM_SETCHECK, 0, 0 );
	}
}

void DlgTransition::OnRadioEntireAdditional() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_ENTIRE_ADDITION;
	m_TransitionOptions.dwBoundaryFlags &= ~DMUS_COMPOSEF_1BAR_ADDITION;
}

void DlgTransition::OnRadioOneBarAdditional() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_TransitionOptions.dwBoundaryFlags &= ~DMUS_COMPOSEF_ENTIRE_ADDITION;
	m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_1BAR_ADDITION;
}

void DlgTransition::OnRadioEntirePattern() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_ENTIRE_TRANSITION;
	m_TransitionOptions.dwBoundaryFlags &= ~DMUS_COMPOSEF_1BAR_TRANSITION;
}

void DlgTransition::OnRadioOneBarPattern() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_TransitionOptions.dwBoundaryFlags &= ~DMUS_COMPOSEF_ENTIRE_TRANSITION;
	m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_1BAR_TRANSITION;
}

void DlgTransition::OnCheckModulate() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_buttonModulate.GetCheck() )
	{
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_MODULATE;
	}
	else
	{
		m_TransitionOptions.dwBoundaryFlags &= ~DMUS_COMPOSEF_MODULATE;
	}
}

void DlgTransition::OnCheckSegDefault() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_buttonSegDefault.GetCheck() )
	{
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_DEFAULT;
	}
	else
	{
		m_TransitionOptions.dwBoundaryFlags &= ~DMUS_COMPOSEF_DEFAULT;
	}

	UpdateSegmentAndPatternTransitionControls();
}

void DlgTransition::OnCheckTransNothing() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_buttonTransNothing.GetCheck() )
	{
		m_TransitionOptions.dwFlags |= TRANS_TO_NOTHING;
	}
	else
	{
		m_TransitionOptions.dwFlags &= ~TRANS_TO_NOTHING;
	}
}

void DlgTransition::OnRadioPrepareBtn() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_radioAfterPrepare.GetCheck() )
	{
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_AFTERPREPARETIME;
	}
	else
	{
		m_TransitionOptions.dwBoundaryFlags &= ~DMUS_COMPOSEF_AFTERPREPARETIME;
	}
}

void DlgTransition::OnRadioInvalidateBtn() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Clear the invalidate flags
	m_TransitionOptions.dwBoundaryFlags &= ~(DMUS_COMPOSEF_NOINVALIDATE|DMUS_COMPOSEF_INVALIDATE_PRI);

	// Set the appropriate invalidate flags
	switch( GetCheckedRadioButton( IDC_RADIO_NOINVALIDATE, IDC_RADIO_INVALIDATE ) )
	{
	case IDC_RADIO_NOINVALIDATE:
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_NOINVALIDATE;
		break;
	case IDC_RADIO_INVALIDATEPRI:
		m_TransitionOptions.dwBoundaryFlags |= DMUS_COMPOSEF_INVALIDATE_PRI;
		break;
	case IDC_RADIO_INVALIDATE:
		// Do nothing
		break;
	}
}
