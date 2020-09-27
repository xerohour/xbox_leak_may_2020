// TabScriptScript.cpp : implementation file
//

#include "stdafx.h"
#include "TabScriptScript.h"
#include "ScriptDesignerDll.h"
#include "Script.h"
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript property page

CTabScriptScript::CTabScriptScript( CScriptPropPageManager* pScriptPropPageManager ) : CPropertyPage(CTabScriptScript::IDD)
{
	//{{AFX_DATA_INIT(CTabScriptScript)
	//}}AFX_DATA_INIT
	
	ASSERT( pScriptPropPageManager != NULL );

	m_pScript = NULL;
	m_pPageManager = pScriptPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabScriptScript::~CTabScriptScript()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::SetScript

void CTabScriptScript::SetScript( CDirectMusicScript* pScript )
{
	m_pScript = pScript;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::SetModifiedFlag

void CTabScriptScript::SetModifiedFlag( void ) 
{
	ASSERT( m_pScript != NULL );

	m_pScript->SetModified( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::EnableControls

void CTabScriptScript::EnableControls( BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
	m_comboLanguage.EnableWindow( fEnable );
	m_checkLoadAll.EnableWindow( fEnable );
	m_checkDownLoadAll.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::UpdateControls

void CTabScriptScript::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_editName.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editName.LimitText( DMUS_MAX_NAME );
	m_comboLanguage.LimitText( DMUS_MAX_NAME );

	if( m_pScript )
	{
		EnableControls( TRUE );

		// Set name
		m_editName.SetWindowText( m_pScript->m_strName );

		// Set language
		m_comboLanguage.SetWindowText( m_pScript->m_strLanguage );

		// Set DMUS_SCRIPTIOF_LOAD_ALL_CONTENT flag
		m_checkLoadAll.SetCheck( m_pScript->m_dwFlagsDM & DMUS_SCRIPTIOF_LOAD_ALL_CONTENT );

		// Set DMUS_SCRIPTIOF_DOWNLOAD_ALL_SEGMENTS flag
		m_checkDownLoadAll.SetCheck( m_pScript->m_dwFlagsDM & DMUS_SCRIPTIOF_DOWNLOAD_ALL_SEGMENTS );
	}
	else
	{
		m_editName.SetWindowText( _T("") );
		m_comboLanguage.SetWindowText( _T("") );
		m_checkLoadAll.SetCheck( 0 );
		m_checkDownLoadAll.SetCheck( 0 );

		EnableControls( FALSE );
	}
}


void CTabScriptScript::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabScriptScript)
	DDX_Control(pDX, IDC_LANGUAGE, m_comboLanguage);
	DDX_Control(pDX, IDC_CHECK_LOAD_ALL, m_checkLoadAll);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_ALL, m_checkDownLoadAll);
	DDX_Control(pDX, IDC_NAME, m_editName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabScriptScript, CPropertyPage)
	//{{AFX_MSG_MAP(CTabScriptScript)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillFocusName)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_ALL, OnCheckDownLoadAll)
	ON_BN_DOUBLECLICKED(IDC_CHECK_DOWNLOAD_ALL, OnDoubleClickedCheckDownLoadAll)
	ON_BN_CLICKED(IDC_CHECK_LOAD_ALL, OnCheckLoadAll)
	ON_BN_DOUBLECLICKED(IDC_CHECK_LOAD_ALL, OnDoubleClickedCheckLoadAll)
	ON_CBN_KILLFOCUS(IDC_LANGUAGE, OnKillFocusLanguage)
	ON_CBN_SELCHANGE(IDC_LANGUAGE, OnSelChangeLanguage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript message handlers


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::OnInitDialog

BOOL CTabScriptScript::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();
	
	m_comboLanguage.ResetContent();

	CString strComboItem;
	if( strComboItem.LoadString(IDS_SCRIPT_LANGUAGE_DEFAULT) )
	{
		m_comboLanguage.AddString(strComboItem);
	}
#ifndef DMP_XBOX
	if( strComboItem.LoadString(IDS_SCRIPT_LANGUAGE2) )
	{
		m_comboLanguage.AddString(strComboItem);
	}
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::OnSetActive

BOOL CTabScriptScript::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CScriptPropPageManager::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::OnCreate

int CTabScriptScript::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabScriptScript::OnDestroy

void CTabScriptScript::OnDestroy() 
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
// CTabScriptScript::OnKillFocusName

void CTabScriptScript::OnKillFocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	if( m_pScript )
	{
		CString strName;

		m_editName.GetWindowText( strName );

		// Strip leading and trailing spaces
		strName.TrimRight();
		strName.TrimLeft();

		if( strName.IsEmpty() )
		{
			m_editName.SetWindowText( m_pScript->m_strName );
		}
		else
		{
			if( strName.Compare( m_pScript->m_strName ) != 0 )
			{
				BSTR bstrName = strName.AllocSysString();
				m_pScript->SetNodeName( bstrName );
				theApp.m_pScriptComponent->m_pIFramework->RefreshNode( m_pScript );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::OnKillFocusLanguage

void CTabScriptScript::OnKillFocusLanguage() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	if( m_pScript )
	{
		CString strLanguage;

		m_comboLanguage.GetWindowText( strLanguage );

		// Strip leading and trailing spaces
		strLanguage.TrimRight();
		strLanguage.TrimLeft();

		if( strLanguage.IsEmpty() )
		{
			m_comboLanguage.SetWindowText( m_pScript->m_strLanguage );
		}
		else
		{
			if( strLanguage.Compare( m_pScript->m_strLanguage ) != 0 )
			{
				m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_LANGUAGE );
				m_pScript->m_strLanguage = strLanguage;
				SetModifiedFlag();

				// Sync Script with DirectMusic
				m_pScript->SyncScriptWithDirectMusic();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::OnSelChangeLanguage

void CTabScriptScript::OnSelChangeLanguage() 
{
	CString strLanguage;

	int nPos = m_comboLanguage.GetCurSel();
	if( nPos != CB_ERR )
	{
		m_comboLanguage.GetLBText( nPos, strLanguage );
		m_comboLanguage.SetWindowText( strLanguage );
		OnKillFocusLanguage();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::OnCheckDownLoadAll

void CTabScriptScript::OnCheckDownLoadAll() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		DWORD dwFlags = m_pScript->m_dwFlagsDM;

		if( m_checkDownLoadAll.GetCheck() )
		{
			dwFlags |= DMUS_SCRIPTIOF_DOWNLOAD_ALL_SEGMENTS;
		}
		else
		{
			dwFlags &= ~DMUS_SCRIPTIOF_DOWNLOAD_ALL_SEGMENTS;
		}

		if( m_pScript->m_dwFlagsDM != dwFlags )
		{
			m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_DOWNLOAD_ALL );
			m_pScript->m_dwFlagsDM = dwFlags;
			SetModifiedFlag();

			// Sync Script with DirectMusic
			m_pScript->SyncScriptWithDirectMusic();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::OnDoubleClickedCheckDownLoadAll

void CTabScriptScript::OnDoubleClickedCheckDownLoadAll() 
{
	OnCheckDownLoadAll();
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::OnCheckLoadAll

void CTabScriptScript::OnCheckLoadAll() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		DWORD dwFlags = m_pScript->m_dwFlagsDM;

		if( m_checkLoadAll.GetCheck() )
		{
			dwFlags |= DMUS_SCRIPTIOF_LOAD_ALL_CONTENT;
		}
		else
		{
			dwFlags &= ~DMUS_SCRIPTIOF_LOAD_ALL_CONTENT;
		}

		if( m_pScript->m_dwFlagsDM != dwFlags )
		{
			m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_LOAD_ALL );
			m_pScript->m_dwFlagsDM = dwFlags;
			SetModifiedFlag();

			// Sync Script with DirectMusic
			m_pScript->SyncScriptWithDirectMusic();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript::OnDoubleClickedCheckLoadAll

void CTabScriptScript::OnDoubleClickedCheckLoadAll() 
{
	OnCheckLoadAll();
}
