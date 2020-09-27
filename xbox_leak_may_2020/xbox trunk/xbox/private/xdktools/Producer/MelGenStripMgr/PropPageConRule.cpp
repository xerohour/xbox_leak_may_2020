// PropPageConRule.cpp : implementation file
//

#include "stdafx.h"
#include "melgenstripmgr.h"
#include <DMUSProd.h>
#include <Conductor.h>
#include "PropMelGen.h"
#include "PropPageMgr.h"
#include "PropPageConRule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PropPageConRule property page

IMPLEMENT_DYNCREATE(PropPageConRule, CPropertyPage)

PropPageConRule::PropPageConRule() : CPropertyPage(PropPageConRule::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_CONNECTION_RULES_PROPPAGE);
	//{{AFX_DATA_INIT(PropPageConRule)
	//}}AFX_DATA_INIT
	m_pIFramework = NULL;

	m_fHaveData = FALSE;
	m_fMultipleMelGensSelected = FALSE;
	m_pMelGen = new CPropMelGen;
	m_fNeedToDetach = FALSE;
}

PropPageConRule::~PropPageConRule()
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

void PropPageConRule::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPageConRule)
	DDX_Control(pDX, IDC_LIST_TRANSITIONS, m_listTransIntervals);
	DDX_Control(pDX, IDC_LIST_TRANSITION_OPTIONS, m_listTransOptions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageConRule, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageConRule)
	ON_LBN_SELCHANGE(IDC_LIST_TRANSITION_OPTIONS, OnSelchangeListTransitionOptions)
	ON_LBN_SELCHANGE(IDC_LIST_TRANSITIONS, OnSelchangeListTransitions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageConRule message handlers

void PropPageConRule::CopyDataToMelGen( CPropMelGen* pMelGen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pMelGen != NULL );

	m_pMelGen->m_dwMeasure		= pMelGen->m_dwMeasure;
	m_pMelGen->m_bBeat			= pMelGen->m_bBeat;
	m_pMelGen->m_dwBits		= pMelGen->m_dwBits;
	m_pMelGen->m_pRepeat		= pMelGen->m_pRepeat;
	m_pMelGen->m_MelGen = pMelGen->m_MelGen;
}


void PropPageConRule::GetDataFromMelGen( CPropMelGen* pMelGen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pMelGen != NULL );

	pMelGen->m_MelGen.ConnectionArc = m_pMelGen->m_MelGen.ConnectionArc;

}

// PropPageConRule::UpdateControls

void PropPageConRule::UpdateControls()
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

	// Transition Options
	m_listTransOptions.SelItemRange(FALSE, 0, 1);
	if (m_pMelGen->m_MelGen.ConnectionArc.dwFlags & DMUS_CONNECTIONF_OVERLAP)
	{
		m_listTransOptions.SetSel(0);
	}
	if (m_pMelGen->m_MelGen.ConnectionArc.dwFlags & DMUS_CONNECTIONF_INTERVALS)
	{
		m_listTransOptions.SetSel(1);
	}

	// Transition Intervals
	m_listTransIntervals.SelItemRange(FALSE, 0, 11);
	for (int n = 0; n < 12; n++)
	{
		if ( m_pMelGen->m_MelGen.ConnectionArc.dwIntervals & (1 << n) )
		{
			m_listTransIntervals.SetSel(n);
		}
	}


}


void PropPageConRule::EnableControls( BOOL fEnable ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Always enable transition options
	m_listTransOptions.EnableWindow( fEnable );

	// Only enable transition intervals if "use intervals" option is selected
	bool fIntervals = (m_pMelGen->m_MelGen.ConnectionArc.dwFlags & DMUS_CONNECTIONF_INTERVALS) ? true : false;
	m_listTransIntervals.EnableWindow( fEnable && fIntervals );
}

BOOL PropPageConRule::PreTranslateMessage(MSG* pMsg) 
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

BOOL PropPageConRule::OnSetActive( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// PropPageConRule::OnCreate

int PropPageConRule::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// PropPageConRule::OnDestroy

void PropPageConRule::OnDestroy() 
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
// PropPageConRule::OnInitDialog

BOOL PropPageConRule::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();

	// Init Transition Options
	m_listTransOptions.AddString("Dovetail");
	m_listTransOptions.AddString("Use Intervals");

	// Init Transition Intervals
	m_listTransIntervals.AddString("Unis");
	m_listTransIntervals.AddString("mi2");
	m_listTransIntervals.AddString("Ma2");
	m_listTransIntervals.AddString("mi3");
	m_listTransIntervals.AddString("Ma3");
	m_listTransIntervals.AddString("4th");
	m_listTransIntervals.AddString("#4/b5");
	m_listTransIntervals.AddString("5th");
	m_listTransIntervals.AddString("mi6");
	m_listTransIntervals.AddString("Ma6");
	m_listTransIntervals.AddString("mi7");
	m_listTransIntervals.AddString("Ma7");

	// Update the dialog
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(PropPageConRule, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(PropPageConRule)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void PropPageConRule::OnSelchangeListTransitionOptions() 
{
	m_pMelGen->m_MelGen.ConnectionArc.dwFlags &= ~DMUS_CONNECTIONF_OVERLAP;
	m_pMelGen->m_MelGen.ConnectionArc.dwFlags &= ~DMUS_CONNECTIONF_INTERVALS;

	int nSelect = m_listTransOptions.GetSelCount();
	if (nSelect > 0)
	{
		int* anSelections = new int[nSelect];
		if (anSelections)
		{
			m_listTransOptions.GetSelItems(nSelect, anSelections);
			for (int n = 0; n < nSelect; n++)
			{
				switch (anSelections[n])
				{
				case 0:
					m_pMelGen->m_MelGen.ConnectionArc.dwFlags |= DMUS_CONNECTIONF_OVERLAP;
					break;
				case 1:
					m_pMelGen->m_MelGen.ConnectionArc.dwFlags |= DMUS_CONNECTIONF_INTERVALS;
					break;
				}
			}

			delete [] anSelections;
		}
	}
	m_pPropPageMgr->UpdateObjectWithMelGenData();	
	EnableControls( m_fHaveData );

}

void PropPageConRule::OnSelchangeListTransitions() 
{
	m_pMelGen->m_MelGen.ConnectionArc.dwIntervals = 0;
	int nSelect = m_listTransIntervals.GetSelCount();
	if (nSelect > 0)
	{
		int* anSelections = new int[nSelect];
		if (anSelections)
		{
			m_listTransIntervals.GetSelItems(nSelect, anSelections);
			for (int n = 0; n < nSelect; n++)
			{
				m_pMelGen->m_MelGen.ConnectionArc.dwIntervals |= 1 << anSelections[n];
			}

			delete [] anSelections;
		}
	}
	m_pPropPageMgr->UpdateObjectWithMelGenData();	
}
