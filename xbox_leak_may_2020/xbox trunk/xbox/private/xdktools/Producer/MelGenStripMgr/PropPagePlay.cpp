// PropPagePlay.cpp : implementation file
//

#include "stdafx.h"
#include "melgenstripmgr.h"
#include <DMUSProd.h>
#include <Conductor.h>
#include "PropMelGen.h"
#include "PropPageMgr.h"
#include "PropPagePlay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// PropPagePlay property page

IMPLEMENT_DYNCREATE(PropPagePlay, CPropertyPage)

PropPagePlay::PropPagePlay() : CPropertyPage(PropPagePlay::IDD)
{
	//{{AFX_DATA_INIT(PropPagePlay)
	//}}AFX_DATA_INIT
	m_pIFramework = NULL;

	m_fHaveData = FALSE;
	m_fMultipleMelGensSelected = FALSE;
	m_pMelGen = new CPropMelGen;
	m_fNeedToDetach = FALSE;
}

PropPagePlay::~PropPagePlay()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pIFramework )
	{
		m_pIFramework->Release();
	}
	
	if( m_pMelGen )
	{
		delete m_pMelGen;
	}
}

void PropPagePlay::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropPagePlay)
	DDX_Control(pDX, IDC_LIST_TRANSPOSE, m_listTransposeIntervals);
	DDX_Control(pDX, IDC_LIST_PLAY_OPTIONS, m_listPlayOptions);
	DDX_Control(pDX, IDC_COMBO_REPEAT_OPTIONS, m_comboRepeatOptions);
	DDX_Control(pDX, IDC_COMBO_PLAYMODE2, m_comboPlaymode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPagePlay, CPropertyPage)
	//{{AFX_MSG_MAP(PropPagePlay)
	ON_CBN_SELCHANGE(IDC_COMBO_PLAYMODE2, OnSelchangeComboPlaymode2)
	ON_CBN_SELCHANGE(IDC_COMBO_REPEAT_OPTIONS, OnSelchangeComboRepeatOptions)
	ON_LBN_SELCHANGE(IDC_LIST_PLAY_OPTIONS, OnSelchangeListPlayOptions)
	ON_LBN_SELCHANGE(IDC_LIST_TRANSPOSE, OnSelchangeListTranspose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPagePlay message handlers

void PropPagePlay::CopyDataToMelGen( CPropMelGen* pMelGen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pMelGen != NULL );

	m_pMelGen->m_dwMeasure		= pMelGen->m_dwMeasure;
	m_pMelGen->m_bBeat			= pMelGen->m_bBeat;
	m_pMelGen->m_dwBits		= pMelGen->m_dwBits;
	m_pMelGen->m_pRepeat		= pMelGen->m_pRepeat;
	m_pMelGen->m_MelGen = pMelGen->m_MelGen;
}


void PropPagePlay::GetDataFromMelGen( CPropMelGen* pMelGen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pMelGen != NULL );

	pMelGen->m_MelGen.dwFragmentFlags |= m_pMelGen->m_MelGen.dwFragmentFlags & MELGEN_PLAY_MASK;
	pMelGen->m_MelGen.dwPlayModeFlags = m_pMelGen->m_MelGen.dwPlayModeFlags;
	pMelGen->m_MelGen.dwTransposeIntervals = m_pMelGen->m_MelGen.dwTransposeIntervals;

}

// PropPagePlay::UpdateControls

void PropPagePlay::UpdateControls()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Controls not created yet
	if( IsWindow( m_hWnd ) == 0 )
	{
		return;
	}

	ASSERT( m_pMelGen );
	if( m_pMelGen == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	// Update the property page based on the new data.

	// Set enable state of controls
	EnableControls( m_fHaveData );

	// Repeat Options
	if (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_SCALE)
	{
		m_comboRepeatOptions.SetCurSel(1);
	}
	else if (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_CHORD)
	{
		m_comboRepeatOptions.SetCurSel(2);
	}
	else // not aligning to scale or chord, so use playmode
	{
		m_comboRepeatOptions.SetCurSel(0);
	}

	// Play Options
	m_listPlayOptions.SelItemRange(FALSE, 0, 2);
	if (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_ANTICIPATE)
	{
		m_listPlayOptions.SetSel(0);
	}
	if (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_INVERT)
	{
		m_listPlayOptions.SetSel(1);
	}
	if (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_REVERSE)
	{
		m_listPlayOptions.SetSel(2);
	}

	// Transpose Intervals
	m_listTransposeIntervals.SelItemRange(FALSE, 0, 5);
	for (int n = 0; n < 6; n++)
	{
		if ( m_pMelGen->m_MelGen.dwTransposeIntervals & (1 << n) )
		{
			m_listTransposeIntervals.SetSel(n);
		}
	}

	// Playmode
	switch (m_pMelGen->m_MelGen.dwPlayModeFlags)
	{
	case DMUS_PLAYMODE_NONE:
		m_comboPlaymode.SetCurSel(0);
		break;
	case 0:
		m_comboPlaymode.SetCurSel(1);
		break;
	case DMUS_PLAYMODE_FIXEDTOCHORD:
		m_comboPlaymode.SetCurSel(2);
		break;
	case DMUS_PLAYMODE_FIXEDTOKEY:
		m_comboPlaymode.SetCurSel(3);
		break;
	case DMUS_PLAYMODE_MELODIC:
		m_comboPlaymode.SetCurSel(4);
		break;
	case DMUS_PLAYMODE_NORMALCHORD:
		m_comboPlaymode.SetCurSel(5);
		break;
	case DMUS_PLAYMODE_ALWAYSPLAY:
		m_comboPlaymode.SetCurSel(6);
		break;
	case DMUS_PLAYMODE_PEDALPOINT:
		m_comboPlaymode.SetCurSel(7);
		break;
	case DMUS_PLAYMODE_PEDALPOINTCHORD:
		m_comboPlaymode.SetCurSel(8);
		break;
	case DMUS_PLAYMODE_PEDALPOINTALWAYS:
		m_comboPlaymode.SetCurSel(9);
		break;
	}

}


void PropPagePlay::EnableControls( BOOL fEnable ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// These are always enabled
	m_listPlayOptions.EnableWindow( fEnable );
	m_comboPlaymode.EnableWindow( fEnable );
	m_listTransposeIntervals.EnableWindow( fEnable );

	// Only enable repeat options and transition intervals if "use repeat" has been enabled
	bool fRepeat = (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT) ? true : false;
	m_comboRepeatOptions.EnableWindow( fEnable && fRepeat );
	m_listTransposeIntervals.EnableWindow( fEnable && fRepeat );
}

BOOL PropPagePlay::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !m_fHaveData )
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

BOOL PropPagePlay::OnSetActive( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// PropPagePlay::OnCreate

int PropPagePlay::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// PropPagePlay::OnDestroy

void PropPagePlay::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs to be done here
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
// PropPagePlay::OnInitDialog

BOOL PropPagePlay::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();

	// Init Repeat Options
	m_comboRepeatOptions.AddString("Use Playmode");
	m_comboRepeatOptions.AddString("Align to Scale");
	m_comboRepeatOptions.AddString("Align to Chord");

	// Init Play Options
	m_listPlayOptions.AddString("Anticipate Chord");
	m_listPlayOptions.AddString("Invert");
	m_listPlayOptions.AddString("Reverse");

	// Init Transpose Intervals
	m_listTransposeIntervals.AddString("2nd");
	m_listTransposeIntervals.AddString("3rd");
	m_listTransposeIntervals.AddString("4th");
	m_listTransposeIntervals.AddString("5th");
	m_listTransposeIntervals.AddString("6th");
	m_listTransposeIntervals.AddString("7th");

	
	// Init Playmode
	m_comboPlaymode.AddString("None");
	m_comboPlaymode.AddString("Fixed");
	m_comboPlaymode.AddString("Fixed to Chord");
	m_comboPlaymode.AddString("Fixed to Key");
	m_comboPlaymode.AddString("Melodic");
	m_comboPlaymode.AddString("Normal Chord");
	m_comboPlaymode.AddString("Always Play");
	m_comboPlaymode.AddString("Pedalpoint");
	m_comboPlaymode.AddString("Pedalpoint Chord");
	m_comboPlaymode.AddString("Pedalpoint Always Play");

	// Update the dialog
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(PropPagePlay, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(PropPagePlay)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void PropPagePlay::OnSelchangeComboPlaymode2() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	switch (m_comboPlaymode.GetCurSel())
	{
	case 0:
		m_pMelGen->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_NONE;
		break;
	case 1:
		m_pMelGen->m_MelGen.dwPlayModeFlags = 0;
		break;
	case 2:
		m_pMelGen->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_FIXEDTOCHORD;
		break;
	case 3:
		m_pMelGen->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_FIXEDTOKEY;
		break;
	case 4:
		m_pMelGen->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_MELODIC;
		break;
	case 5:
		m_pMelGen->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_NORMALCHORD;
		break;
	case 6:
		m_pMelGen->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_ALWAYSPLAY;
		break;
	case 7:
		m_pMelGen->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_PEDALPOINT;
		break;
	case 8:
		m_pMelGen->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_PEDALPOINTCHORD;
		break;
	case 9:
		m_pMelGen->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_PEDALPOINTALWAYS;
		break;
	}
	m_pPropPageMgr->UpdateObjectWithMelGenData();
	
}



void PropPagePlay::OnSelchangeComboRepeatOptions() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_pMelGen->m_MelGen.dwFragmentFlags &= ~DMUS_FRAGMENTF_USE_PLAYMODE;
	m_pMelGen->m_MelGen.dwFragmentFlags &= ~DMUS_FRAGMENTF_SCALE;
	m_pMelGen->m_MelGen.dwFragmentFlags &= ~DMUS_FRAGMENTF_CHORD;
	switch (m_comboRepeatOptions.GetCurSel())
	{
	case 0:
		m_pMelGen->m_MelGen.dwFragmentFlags |= DMUS_FRAGMENTF_USE_PLAYMODE;
		break;
	case 1:
		m_pMelGen->m_MelGen.dwFragmentFlags |= DMUS_FRAGMENTF_SCALE;
		break;
	case 2:
		m_pMelGen->m_MelGen.dwFragmentFlags |= DMUS_FRAGMENTF_CHORD;
		break;
	}
	m_pPropPageMgr->UpdateObjectWithMelGenData();
	EnableControls(TRUE);
}

void PropPagePlay::OnSelchangeListPlayOptions() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_pMelGen->m_MelGen.dwFragmentFlags &= ~DMUS_FRAGMENTF_ANTICIPATE;
	m_pMelGen->m_MelGen.dwFragmentFlags &= ~DMUS_FRAGMENTF_INVERT;
	m_pMelGen->m_MelGen.dwFragmentFlags &= ~DMUS_FRAGMENTF_REVERSE;

	int nSelect = m_listPlayOptions.GetSelCount();
	if (nSelect > 0)
	{
		int* anSelections = new int[nSelect];
		if (anSelections)
		{
			m_listPlayOptions.GetSelItems(nSelect, anSelections);
			for (int n = 0; n < nSelect; n++)
			{
				switch (anSelections[n])
				{
				case 0:
					m_pMelGen->m_MelGen.dwFragmentFlags |= DMUS_FRAGMENTF_ANTICIPATE;
					break;
				case 1:
					m_pMelGen->m_MelGen.dwFragmentFlags |= DMUS_FRAGMENTF_INVERT;
					break;
				case 2:
					m_pMelGen->m_MelGen.dwFragmentFlags |= DMUS_FRAGMENTF_REVERSE;
					break;
				}
			}

			delete [] anSelections;
		}
	}
	m_pPropPageMgr->UpdateObjectWithMelGenData();	

}

void PropPagePlay::OnSelchangeListTranspose() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_pMelGen->m_MelGen.dwTransposeIntervals = 0;
	int nSelect = m_listTransposeIntervals.GetSelCount();
	if (nSelect > 0)
	{
		int* anSelections = new int[nSelect];
		if (anSelections)
		{
			m_listTransposeIntervals.GetSelItems(nSelect, anSelections);
			for (int n = 0; n < nSelect; n++)
			{
				m_pMelGen->m_MelGen.dwTransposeIntervals |= 1 << anSelections[n];
			}

			delete [] anSelections;
		}
	}
	m_pPropPageMgr->UpdateObjectWithMelGenData();	
}
