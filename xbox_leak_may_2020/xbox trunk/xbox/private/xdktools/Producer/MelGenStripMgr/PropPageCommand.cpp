// PropPageCommand.cpp : implementation file
//

#include "stdafx.h"
#include "melgenstripmgr.h"
#include <DMUSProd.h>
#include <Conductor.h>
#include "PropMelGen.h"
#include "PropPageMgr.h"
#include "PropPageCommand.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PropPageCommand property page

IMPLEMENT_DYNCREATE(PropPageCommand, CPropertyPage)

PropPageCommand::PropPageCommand() : CPropertyPage(PropPageCommand::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_DIALOG_COMMAND_PROPPAGE);
	//{{AFX_DATA_INIT(PropPageCommand)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pIFramework = NULL;

	m_fHaveData = FALSE;
	m_fMultipleMelGensSelected = FALSE;
	m_pMelGen = new CPropMelGen;
	m_fNeedToDetach = FALSE;
    m_bEmbCustom = 100;
    m_bLevel = 1;
}

PropPageCommand::~PropPageCommand()
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

void PropPageCommand::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPageCommand)
	DDX_Control(pDX, IDC_SPIN_EMB_CUSTOM, m_spinEmbCustom);
	DDX_Control(pDX, IDC_EDIT_EMB_CUSTOM, m_editEmbCustom);
	DDX_Control(pDX, IDC_SPIN_RANGE, m_spinRange);
	DDX_Control(pDX, IDC_SPIN_LEVEL, m_spinLevel);
	DDX_Control(pDX, IDC_LIST_EMBELLISHMENT, m_listEmbellishment);
	DDX_Control(pDX, IDC_EDIT_RANGE, m_editRange);
	DDX_Control(pDX, IDC_EDIT_LEVEL, m_editLevel);
	DDX_Control(pDX, IDC_CHECK_NO_LEVEL, m_checkNoLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageCommand, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageCommand)
	ON_BN_CLICKED(IDC_CHECK_NO_LEVEL, OnCheckNoLevel)
	ON_LBN_SELCHANGE(IDC_LIST_EMBELLISHMENT, OnSelchangeListEmbellishment)
	ON_EN_CHANGE(IDC_EDIT_EMB_CUSTOM, OnChangeEditEmbCustom)
	ON_EN_CHANGE(IDC_EDIT_LEVEL, OnChangeEditLevel)
	ON_EN_CHANGE(IDC_EDIT_RANGE, OnChangeEditRange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageCommand message handlers

void PropPageCommand::CopyDataToMelGen( CPropMelGen* pMelGen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pMelGen != NULL );

	m_pMelGen->m_dwMeasure		= pMelGen->m_dwMeasure;
	m_pMelGen->m_bBeat			= pMelGen->m_bBeat;
	m_pMelGen->m_dwBits		= pMelGen->m_dwBits;
	m_pMelGen->m_pRepeat		= pMelGen->m_pRepeat;
	m_pMelGen->m_MelGen = pMelGen->m_MelGen;
}


void PropPageCommand::GetDataFromMelGen( CPropMelGen* pMelGen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pMelGen != NULL );

	pMelGen->m_MelGen.Command = m_pMelGen->m_MelGen.Command;

}

// PropPageCommand::UpdateControls

void PropPageCommand::UpdateControls()
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

	// Embellishments
	m_listEmbellishment.SelItemRange(FALSE, 0, 5);
	switch (m_pMelGen->m_MelGen.Command.bCommand)
	{
	case DMUS_COMMANDT_INTRO:
		m_listEmbellishment.SetCurSel(0);
		break;
	case DMUS_COMMANDT_BREAK:
		m_listEmbellishment.SetCurSel(1);
		break;
	case DMUS_COMMANDT_FILL:
		m_listEmbellishment.SetCurSel(2);
		break;
	case DMUS_COMMANDT_END:
		m_listEmbellishment.SetCurSel(3);
		break;
	case DMUS_COMMANDT_GROOVE:
		m_listEmbellishment.SetCurSel(5);
		break;
	default:
		m_listEmbellishment.SetCurSel(4);
	}

	// Groove level and range
    if (m_pMelGen->m_MelGen.Command.bGrooveLevel)
    {
    	m_spinLevel.SetPos( m_pMelGen->m_MelGen.Command.bGrooveLevel );
    }
    else
    {
       	m_editLevel.SetWindowText( _T("") );
    }
	m_spinRange.SetPos( m_pMelGen->m_MelGen.Command.bGrooveRange );

    // Custom embellishment
    if (m_listEmbellishment.GetCurSel() == 4)
    {
	    m_bEmbCustom = m_pMelGen->m_MelGen.Command.bCommand;
    }
	m_spinEmbCustom.SetPos( m_bEmbCustom );

	// Check box (check if level is zero)
	if (m_pMelGen->m_MelGen.Command.bGrooveLevel)
	{
		m_checkNoLevel.SetCheck(BST_UNCHECKED);	
	}
	else
	{
		m_checkNoLevel.SetCheck(BST_CHECKED);	
	}

    // Set enable state of controls
	EnableControls( m_fHaveData );
}

void PropPageCommand::EnableControls( BOOL fEnable ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Always enable embellishments, level check box, and level
	m_listEmbellishment.EnableWindow( fEnable );
	m_checkNoLevel.EnableWindow( fEnable );
	m_spinLevel.EnableWindow( fEnable );
	m_editLevel.EnableWindow( fEnable );

    // enable range only when check box is not checked
	bool fRange = fEnable ? true : false;
	if (m_checkNoLevel.GetCheck() == BST_CHECKED)
	{
		fRange = false;
	}
    m_spinRange.EnableWindow( fRange );
	m_editRange.EnableWindow( fRange );

    // enable custom embellishment only when Custom is the selected embellishment
	bool fEmbCustom = fEnable ? true : false;
    if (m_listEmbellishment.GetCurSel() != 4)
    {
        fEmbCustom = false;
    }
    m_editEmbCustom.EnableWindow( fEmbCustom );
    m_spinEmbCustom.EnableWindow( fEmbCustom );

}


BOOL PropPageCommand::PreTranslateMessage(MSG* pMsg) 
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

BOOL PropPageCommand::OnSetActive( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
	
	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// PropPageCommand::OnCreate

int PropPageCommand::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// PropPageCommand::OnDestroy

void PropPageCommand::OnDestroy() 
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
// PropPageCommand::OnInitDialog

BOOL PropPageCommand::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();

	// Init Embellishments
	m_listEmbellishment.AddString("Intro");
	m_listEmbellishment.AddString("Break");
	m_listEmbellishment.AddString("Fill");
	m_listEmbellishment.AddString("End");
	m_listEmbellishment.AddString("Custom");
	m_listEmbellishment.AddString("None");

	m_editLevel.LimitText( 3 );
	m_spinLevel.SetRange( 1, 100 );

	m_editRange.LimitText( 3 );
	m_spinRange.SetRange( 0, 100 );

    m_bEmbCustom = 100;
	m_editEmbCustom.LimitText( 3 );
	m_spinEmbCustom.SetRange(100, 199);

	m_checkNoLevel.SetCheck(BST_UNCHECKED);	
	// Update the dialog
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(PropPageCommand, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(PropPageCommand)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()



void PropPageCommand::OnSelchangeListEmbellishment() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	switch (m_listEmbellishment.GetCurSel())
	{
	case 0: // Intro
		m_pMelGen->m_MelGen.Command.bCommand = DMUS_COMMANDT_INTRO;
		break;
	case 1: // Break
		m_pMelGen->m_MelGen.Command.bCommand = DMUS_COMMANDT_BREAK;
		break;
	case 2: // Fill
		m_pMelGen->m_MelGen.Command.bCommand = DMUS_COMMANDT_FILL;
		break;
	case 3: // End
		m_pMelGen->m_MelGen.Command.bCommand = DMUS_COMMANDT_END;
		break;
	case 4: // Custom
		m_pMelGen->m_MelGen.Command.bCommand = m_bEmbCustom;
		break;
	case 5: // None
		m_pMelGen->m_MelGen.Command.bCommand = DMUS_COMMANDT_GROOVE;
		break;
	}
	m_pPropPageMgr->UpdateObjectWithMelGenData();
	UpdateControls();
	
}

void PropPageCommand::CheckNoLevel() 
{
    m_bLevel = m_pMelGen->m_MelGen.Command.bGrooveLevel;
	m_pMelGen->m_MelGen.Command.bGrooveLevel = 0;
	m_editLevel.SetWindowText( _T("") );
	m_checkNoLevel.SetCheck(BST_CHECKED);	
    EnableControls(m_fHaveData);
}

void PropPageCommand::UnCheckNoLevel() 
{
	if (m_checkNoLevel.GetCheck() == BST_CHECKED)
	{
        m_pMelGen->m_MelGen.Command.bGrooveLevel = m_bLevel;
	    m_spinLevel.SetPos( m_bLevel );
    }
	m_checkNoLevel.SetCheck(BST_UNCHECKED);	
    EnableControls(m_fHaveData);
}

void PropPageCommand::OnCheckNoLevel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (m_checkNoLevel.GetCheck() == BST_CHECKED)
	{
        UnCheckNoLevel();
    }
	else if (m_checkNoLevel.GetCheck() == BST_UNCHECKED)
	{
        CheckNoLevel();
	}
	m_pPropPageMgr->UpdateObjectWithMelGenData();
	
}

void PropPageCommand::OnChangeEditEmbCustom() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    if (!m_editEmbCustom) return;

    CString strNewEmb;

	m_editEmbCustom.GetWindowText( strNewEmb );

	// Strip leading and trailing spaces
	strNewEmb.TrimRight();
	strNewEmb.TrimLeft();

	if( !strNewEmb.IsEmpty() )
	{
		int iNewEmb = _ttoi( strNewEmb );
		if( iNewEmb > 199 )
		{
			iNewEmb = 199;
			m_spinEmbCustom.SetPos( iNewEmb );
		}
		else if( iNewEmb < 100 )
		{
			iNewEmb = 100;
			m_spinEmbCustom.SetPos( iNewEmb );
		}
		if( (BYTE)iNewEmb != m_bEmbCustom )
		{
            m_bEmbCustom = (BYTE)iNewEmb;
            if (m_listEmbellishment.GetCurSel() == 4)
            {
			    m_pMelGen->m_MelGen.Command.bCommand = m_bEmbCustom;
			    m_pPropPageMgr->UpdateObjectWithMelGenData();
            }
		}
	}
	
}

void PropPageCommand::OnChangeEditLevel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    if (!m_editLevel) return;

	CString strNewLevel;
    int iNewLevel = 0;

	m_editLevel.GetWindowText( strNewLevel );

	// Strip leading and trailing spaces
	strNewLevel.TrimRight();
	strNewLevel.TrimLeft();


	if( !strNewLevel.IsEmpty() )
	{
		iNewLevel = _ttoi( strNewLevel );
		if( iNewLevel > 100 )
		{
			iNewLevel = 100;
			m_spinLevel.SetPos( iNewLevel );
		}
		else if( iNewLevel < 0 )
		{
			iNewLevel = 0;
			m_spinLevel.SetPos( iNewLevel );
	        m_editLevel.SetWindowText( _T("") );
		}
    }
	if( (BYTE)iNewLevel != m_pMelGen->m_MelGen.Command.bGrooveLevel )
	{
		m_pMelGen->m_MelGen.Command.bGrooveLevel = (BYTE)iNewLevel;
		m_pPropPageMgr->UpdateObjectWithMelGenData();
        // Update checkbox
	    if (m_pMelGen->m_MelGen.Command.bGrooveLevel)
	    {
            m_bLevel = m_pMelGen->m_MelGen.Command.bGrooveLevel;
		    UnCheckNoLevel();	
	    }
	    else
	    {
		    CheckNoLevel();	
	    }
    }
	
}

void PropPageCommand::OnChangeEditRange() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    if (!m_editRange) return;

    CString strNewRange;
    int iNewRange = 0;

	m_editRange.GetWindowText( strNewRange );

	// Strip leading and trailing spaces
	strNewRange.TrimRight();
	strNewRange.TrimLeft();

	if( !strNewRange.IsEmpty() )
	{
		iNewRange = _ttoi( strNewRange );
		if( iNewRange > 100 )
		{
			iNewRange = 100;
			m_spinRange.SetPos( iNewRange );
		}
		else if( iNewRange < 0 )
		{
			iNewRange = 0;
			m_spinRange.SetPos( iNewRange );
		}
    }
	if( (BYTE)iNewRange != m_pMelGen->m_MelGen.Command.bGrooveRange )
	{
		m_pMelGen->m_MelGen.Command.bGrooveRange = (BYTE)iNewRange;
		m_pPropPageMgr->UpdateObjectWithMelGenData();
	}
	
}
