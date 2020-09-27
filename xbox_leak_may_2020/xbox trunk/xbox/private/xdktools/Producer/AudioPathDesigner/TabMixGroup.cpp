// TabMixGroup.cpp : implementation file
//

#include "stdafx.h"
#include "audiopathdesignerdll.h"
#include "TabMixGroup.h"
#include "AudioPath.h"
#include "AudioPathCtl.h"
#include "ItemInfo.h"
#include "DlgEditSynth.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabMixGroup property page

IMPLEMENT_DYNCREATE(CTabMixGroup, CPropertyPage)

CTabMixGroup::CTabMixGroup() : CPropertyPage(CTabMixGroup::IDD)
{
	//{{AFX_DATA_INIT(CTabMixGroup)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_fNeedToDetach = false;
	m_pIPropPageObject = NULL;
}

CTabMixGroup::~CTabMixGroup()
{
	// Weak reference - don't release it!
	//RELEASE(m_pIPropPageObject)
}

void CTabMixGroup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabMixGroup)
	DDX_Control(pDX, ID_STATIC_SYNTH, m_groupSynth);
	DDX_Control(pDX, IDC_BUTTON_SYNTH, m_buttonSynth);
	DDX_Control(pDX, IDC_EDIT_MIXGROUP_NAME, m_editMixGroupName);
	DDX_Control(pDX, IDC_COMBO_SYNTH, m_comboSynth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabMixGroup, CPropertyPage)
	//{{AFX_MSG_MAP(CTabMixGroup)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SYNTH, OnButtonSynth)
	ON_EN_KILLFOCUS(IDC_EDIT_MIXGROUP_NAME, OnKillfocusEditMixgroupName)
	ON_CBN_SELCHANGE(IDC_COMBO_SYNTH, OnSelchangeComboSynth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabMixGroup message handlers

BOOL CTabMixGroup::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->GetActivePage( &CMixGroupPPGMgr::sm_nActiveTab );
		RELEASE( pIPropSheet );
	}
	
	return CPropertyPage::OnSetActive();
}

int CTabMixGroup::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
		m_fNeedToDetach = true;
	}

	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CTabMixGroup::OnDestroy() 
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

		m_fNeedToDetach = false;
	}

	CPropertyPage::OnDestroy();
}

BOOL CTabMixGroup::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();

	m_editMixGroupName.LimitText( DMUS_MAX_NAME );

	// Remove all ports from the combobox
	m_comboSynth.ResetContent();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabMixGroup::SetItem

void CTabMixGroup::SetItem( MixGroupInfoForPPG* pMixGroupInfoForPPG, IDMUSProdPropPageObject* pINewPropPageObject )
{
	if( pMixGroupInfoForPPG )
	{
		m_MixGroupInfoForPPG.Copy( *pMixGroupInfoForPPG );
	}
	else
	{
		m_MixGroupInfoForPPG.m_fValid = false;
	}

	m_pIPropPageObject = pINewPropPageObject;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabMixGroup::EnableControls

void CTabMixGroup::EnableControls( bool fEnable ) 
{
	m_editMixGroupName.EnableWindow( (m_MixGroupInfoForPPG.m_dwFlags & MGI_DONT_EDIT_NAME) ? FALSE : TRUE );

	m_groupSynth.EnableWindow( (m_MixGroupInfoForPPG.m_dwFlags & MGI_HAS_SYNTH) ? TRUE : FALSE );

	if( fEnable
	&&	(m_MixGroupInfoForPPG.m_dwFlags & MGI_HAS_SYNTH) )
	{
		m_comboSynth.EnableWindow( TRUE );
		m_buttonSynth.EnableWindow( TRUE );
	}
	else
	{
		m_comboSynth.EnableWindow( FALSE );
		m_buttonSynth.EnableWindow( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabMixGroup::UpdateControls

void CTabMixGroup::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_editMixGroupName.m_hWnd) == FALSE )
	{
		return;
	}

	// Remove all ports from the combobox
	m_comboSynth.ResetContent();

	// Update controls
	if( m_MixGroupInfoForPPG.m_fValid && m_pIPropPageObject )
	{
		EnableControls( true );

		// Set name
		m_editMixGroupName.SetWindowText( m_MixGroupInfoForPPG.m_strMixGroupName );

		if( m_MixGroupInfoForPPG.m_pAudioPath
		&&	(m_MixGroupInfoForPPG.m_dwFlags & MGI_HAS_SYNTH) )
		{
			// Iterate through all ports
			POSITION posPort = m_MixGroupInfoForPPG.m_pAudioPath->m_lstPortOptions.GetHeadPosition();
			while( posPort )
			{
				PortOptions *pPortOptions = m_MixGroupInfoForPPG.m_pAudioPath->m_lstPortOptions.GetNext( posPort );
				int nIndex = m_comboSynth.AddString( pPortOptions->m_strName );

				m_comboSynth.SetItemDataPtr( nIndex, pPortOptions );

				if( m_MixGroupInfoForPPG.m_PortOptions.IsEqual( pPortOptions ) )
				{
					m_comboSynth.SetCurSel( nIndex );
				}
			}
		}
	}
	else
	{
		m_editMixGroupName.SetWindowText( _T("") );
		m_comboSynth.SetCurSel( -1 );

		EnableControls( false );
	}
}

void CTabMixGroup::OnButtonSynth() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !(m_MixGroupInfoForPPG.m_dwFlags & MGI_CAN_EDIT_SYNTH) )
	{
		AfxMessageBox( IDS_ERR_CANT_EDIT_SYNTH );
		return;
	}

	// Get Producer hwnd
	HWND hwndProducer = m_MixGroupInfoForPPG.m_pAudioPath->m_pAudioPathCtrl->m_hWndContainer;
	if( hwndProducer == NULL )
	{
		ASSERT( 0 );
		return;
	}


	CDlgEditSynth dlgEditSynth( CWnd::FromHandle( hwndProducer ) );
	dlgEditSynth.m_dwEffects = m_MixGroupInfoForPPG.m_PortOptions.m_dwEffects;
	dlgEditSynth.m_dwSupportedEffects = m_MixGroupInfoForPPG.m_PortOptions.m_dwSupportedEffects;
	dlgEditSynth.m_dwSampleRate = m_MixGroupInfoForPPG.m_PortOptions.m_dwSampleRate;
	dlgEditSynth.m_dwVoices = m_MixGroupInfoForPPG.m_PortOptions.m_dwVoices;
	dlgEditSynth.m_dwMaxVoices = m_MixGroupInfoForPPG.m_PortOptions.m_dwMaxVoices;
	dlgEditSynth.m_strName = m_MixGroupInfoForPPG.m_PortOptions.m_strName;

	if( dlgEditSynth.DoModal() == IDOK )
	{
		if( m_MixGroupInfoForPPG.m_PortOptions.m_dwEffects != dlgEditSynth.m_dwEffects
		||	m_MixGroupInfoForPPG.m_PortOptions.m_dwSampleRate != dlgEditSynth.m_dwSampleRate
		||	m_MixGroupInfoForPPG.m_PortOptions.m_dwVoices != dlgEditSynth.m_dwVoices )
		{
			m_MixGroupInfoForPPG.m_PortOptions.m_dwEffects = dlgEditSynth.m_dwEffects;
			m_MixGroupInfoForPPG.m_PortOptions.m_dwSampleRate = dlgEditSynth.m_dwSampleRate;
			m_MixGroupInfoForPPG.m_PortOptions.m_dwVoices = dlgEditSynth.m_dwVoices;

			// Nofiy the PPO that the data changed
			m_MixGroupInfoForPPG.m_dwChanged = CH_MIXGROUP_SYNTH;
			m_pIPropPageObject->SetData( &m_MixGroupInfoForPPG );
		}
	}
}

void CTabMixGroup::OnKillfocusEditMixgroupName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !m_MixGroupInfoForPPG.m_fValid || !m_pIPropPageObject )
	{
		return;
	}

	CString strNewText;
	m_editMixGroupName.GetWindowText( strNewText );

	// Strip leading and trailing spaces
	strNewText.TrimLeft();
	strNewText.TrimRight();

	if( strNewText.IsEmpty() )
	{
		m_editMixGroupName.SetWindowText( m_MixGroupInfoForPPG.m_strMixGroupName );
	}
	else if( strNewText != m_MixGroupInfoForPPG.m_strMixGroupName )
	{
		m_MixGroupInfoForPPG.m_strMixGroupName = strNewText;
		m_MixGroupInfoForPPG.m_dwChanged = CH_MIXGROUP_NAME;
		m_pIPropPageObject->SetData( &m_MixGroupInfoForPPG );
	}
}

void CTabMixGroup::OnSelchangeComboSynth() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !m_MixGroupInfoForPPG.m_fValid || !m_pIPropPageObject )
	{
		return;
	}

	// Get the new index
	int nIndex = m_comboSynth.GetCurSel();
	if( nIndex != CB_ERR )
	{
		// Get a pointer to the new synth GUID
		PortOptions *pPortOptions = static_cast<PortOptions *>( m_comboSynth.GetItemDataPtr( nIndex ) );

		// Copy the information for the new synth
		m_MixGroupInfoForPPG.m_PortOptions.Copy( pPortOptions );

		// Nofiy the PPO that the data changed
		m_MixGroupInfoForPPG.m_dwChanged = CH_MIXGROUP_SYNTH;
		m_pIPropPageObject->SetData( &m_MixGroupInfoForPPG );
	}
}
