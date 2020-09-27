// TabScriptInfo.cpp : implementation file
//

#include "stdafx.h"
#include "TabScriptInfo.h"
#include "ScriptDesignerDll.h"
#include "Script.h"
#include <guiddlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo property page

CTabScriptInfo::CTabScriptInfo( CScriptPropPageManager* pScriptPropPageManager ) : CPropertyPage(CTabScriptInfo::IDD)
{
	//{{AFX_DATA_INIT(CTabScriptInfo)
	//}}AFX_DATA_INIT
	
	ASSERT( pScriptPropPageManager != NULL );

	m_pScript = NULL;
	m_pPageManager = pScriptPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabScriptInfo::~CTabScriptInfo()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::SetScript

void CTabScriptInfo::SetScript( CDirectMusicScript* pScript )
{
	m_pScript = pScript;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::SetModifiedFlag

void CTabScriptInfo::SetModifiedFlag( void ) 
{
	ASSERT( m_pScript != NULL );

	m_pScript->SetModified( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::EnableControls

void CTabScriptInfo::EnableControls( BOOL fEnable ) 
{
	m_editAuthor.EnableWindow( fEnable );
	m_editCopyright.EnableWindow( fEnable );
	m_editSubject.EnableWindow( fEnable );
	m_editVersion_1.EnableWindow( fEnable );
	m_editVersion_2.EnableWindow( fEnable );
	m_editVersion_3.EnableWindow( fEnable );
	m_editVersion_4.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::UpdateControls

void CTabScriptInfo::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_editVersion_1.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editVersion_1.LimitText( 4 );
	m_editVersion_2.LimitText( 4 );
	m_editVersion_3.LimitText( 4 );
	m_editVersion_4.LimitText( 4 );

	if( m_pScript )
	{
		CString strText;

		EnableControls( TRUE );

		m_editAuthor.SetWindowText( m_pScript->m_strAuthor );
		m_editCopyright.SetWindowText( m_pScript->m_strCopyright );
		m_editSubject.SetWindowText( m_pScript->m_strSubject );
		
		strText.Format( "%u", ((m_pScript->m_vVersion.dwVersionMS & 0xFFFF0000) >> 16) );
		m_editVersion_1.SetWindowText( strText );
		
		strText.Format( "%u", (m_pScript->m_vVersion.dwVersionMS & 0x0000FFFF) );
		m_editVersion_2.SetWindowText( strText );
		
		strText.Format( "%u", ((m_pScript->m_vVersion.dwVersionLS & 0xFFFF0000) >> 16) );
		m_editVersion_3.SetWindowText( strText );
		
		strText.Format( "%u", (m_pScript->m_vVersion.dwVersionLS & 0x0000FFFF) );
		m_editVersion_4.SetWindowText( strText );
		
		strText.Format( "%u", ((m_pScript->m_vDirectMusicVersion.dwVersionMS & 0xFFFF0000) >> 16) );
		m_editDMVersion_1.SetWindowText( strText );
		
		strText.Format( "%u", (m_pScript->m_vDirectMusicVersion.dwVersionMS & 0x0000FFFF) );
		m_editDMVersion_2.SetWindowText( strText );
		
		strText.Format( "%u", ((m_pScript->m_vDirectMusicVersion.dwVersionLS & 0xFFFF0000) >> 16) );
		m_editDMVersion_3.SetWindowText( strText );
		
		strText.Format( "%u", (m_pScript->m_vDirectMusicVersion.dwVersionLS & 0x0000FFFF) );
		m_editDMVersion_4.SetWindowText( strText );
	}
	else
	{
		m_editAuthor.SetWindowText( _T("") );
		m_editCopyright.SetWindowText( _T("") );
		m_editSubject.SetWindowText( _T(""));
		m_editVersion_1.SetWindowText( _T("") );
		m_editVersion_2.SetWindowText( _T("") );
		m_editVersion_3.SetWindowText( _T("") );
		m_editVersion_4.SetWindowText( _T("") );
		m_editDMVersion_1.SetWindowText( _T("") );
		m_editDMVersion_2.SetWindowText( _T("") );
		m_editDMVersion_3.SetWindowText( _T("") );
		m_editDMVersion_4.SetWindowText( _T("") );

		EnableControls( FALSE );
	}
}


void CTabScriptInfo::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabScriptInfo)
	DDX_Control(pDX, IDC_VERSION_4, m_editVersion_4);
	DDX_Control(pDX, IDC_VERSION_3, m_editVersion_3);
	DDX_Control(pDX, IDC_VERSION_2, m_editVersion_2);
	DDX_Control(pDX, IDC_VERSION_1, m_editVersion_1);
	DDX_Control(pDX, IDC_DM_VERSION_4, m_editDMVersion_4);
	DDX_Control(pDX, IDC_DM_VERSION_3, m_editDMVersion_3);
	DDX_Control(pDX, IDC_DM_VERSION_2, m_editDMVersion_2);
	DDX_Control(pDX, IDC_DM_VERSION_1, m_editDMVersion_1);
	DDX_Control(pDX, IDC_SUBJECT, m_editSubject);
	DDX_Control(pDX, IDC_COPYRIGHT, m_editCopyright);
	DDX_Control(pDX, IDC_AUTHOR, m_editAuthor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabScriptInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CTabScriptInfo)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_AUTHOR, OnKillFocusAuthor)
	ON_EN_KILLFOCUS(IDC_COPYRIGHT, OnKillFocusCopyright)
	ON_EN_KILLFOCUS(IDC_SUBJECT, OnKillFocusSubject)
	ON_EN_KILLFOCUS(IDC_VERSION_1, OnKillFocusVersion_1)
	ON_EN_KILLFOCUS(IDC_VERSION_2, OnKillFocusVersion_2)
	ON_EN_KILLFOCUS(IDC_VERSION_3, OnKillFocusVersion_3)
	ON_EN_KILLFOCUS(IDC_VERSION_4, OnKillFocusVersion_4)
	ON_BN_CLICKED(IDC_EDIT_GUID, OnEditGuid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::OnSetActive

BOOL CTabScriptInfo::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CScriptPropPageManager::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::OnCreate

int CTabScriptInfo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabScriptInfo::OnDestroy

void CTabScriptInfo::OnDestroy() 
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
// CTabScriptInfo::OnKillFocusAuthor

void CTabScriptInfo::OnKillFocusAuthor() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		CString strAuthor;

		m_editAuthor.GetWindowText( strAuthor );

		// Strip leading and trailing spaces
		strAuthor.TrimRight();
		strAuthor.TrimLeft();

		if( strAuthor.Compare( m_pScript->m_strAuthor ) != 0 )
		{
			m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_AUTHOR );
			m_pScript->m_strAuthor = strAuthor;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::OnKillFocusCopyright

void CTabScriptInfo::OnKillFocusCopyright() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		CString strCopyright;

		m_editCopyright.GetWindowText( strCopyright );

		// Strip leading and trailing spaces
		strCopyright.TrimRight();
		strCopyright.TrimLeft();

		if( strCopyright.Compare( m_pScript->m_strCopyright ) != 0 )
		{
			m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_COPYRIGHT );
			m_pScript->m_strCopyright = strCopyright;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::OnKillFocusSubject

void CTabScriptInfo::OnKillFocusSubject() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		CString strSubject;

		m_editSubject.GetWindowText( strSubject );

		// Strip leading and trailing spaces
		strSubject.TrimRight();
		strSubject.TrimLeft();

		if( strSubject.Compare( m_pScript->m_strSubject ) != 0 )
		{
			m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_SUBJECT );
			m_pScript->m_strSubject = strSubject;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::OnKillFocusVersion_1

void CTabScriptInfo::OnKillFocusVersion_1() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		CString strVersion_1;
		CString strScriptVersion_1;

		m_editVersion_1.GetWindowText( strVersion_1 );

		// Strip leading and trailing spaces
		strVersion_1.TrimRight();
		strVersion_1.TrimLeft();

		if( strVersion_1.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", ((m_pScript->m_vVersion.dwVersionMS & 0xFFFF0000) >> 16) );
			m_editVersion_1.SetWindowText( strText );
		}
		else
		{
			strScriptVersion_1.Format( "%u", ((m_pScript->m_vVersion.dwVersionMS & 0xFFFF0000) >> 16) );
			
			if( strVersion_1.Compare( strScriptVersion_1 ) != 0 )
			{
				DWORD dwVersion_1 = _ttoi( strVersion_1 );
				dwVersion_1 = (dwVersion_1 & 0x0000FFFF) << 16;

				m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_VERSION );
				m_pScript->m_vVersion.dwVersionMS &= 0x0000FFFF;
				m_pScript->m_vVersion.dwVersionMS |= dwVersion_1;
				SetModifiedFlag();

				// Sync Script with DirectMusic
				m_pScript->SyncScriptWithDirectMusic();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::OnKillFocusVersion_2

void CTabScriptInfo::OnKillFocusVersion_2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		CString strVersion_2;
		CString strScriptVersion_2;

		m_editVersion_2.GetWindowText( strVersion_2 );

		// Strip leading and trailing spaces
		strVersion_2.TrimRight();
		strVersion_2.TrimLeft();

		if( strVersion_2.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", (m_pScript->m_vVersion.dwVersionMS & 0x0000FFFF) );
			m_editVersion_2.SetWindowText( strText );
		}
		else
		{
			strScriptVersion_2.Format( "%u", (m_pScript->m_vVersion.dwVersionMS & 0x0000FFFF) );
			
			if( strVersion_2.Compare( strScriptVersion_2 ) != 0 )
			{
				DWORD dwVersion_2 = _ttoi( strVersion_2 );
				dwVersion_2 &= 0x0000FFFF;

				m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_VERSION );
				m_pScript->m_vVersion.dwVersionMS &= 0xFFFF0000;
				m_pScript->m_vVersion.dwVersionMS |= dwVersion_2;
				SetModifiedFlag();

				// Sync Script with DirectMusic
				m_pScript->SyncScriptWithDirectMusic();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::OnKillFocusVersion_3

void CTabScriptInfo::OnKillFocusVersion_3() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		CString strVersion_3;
		CString strScriptVersion_3;

		m_editVersion_3.GetWindowText( strVersion_3 );

		// Strip leading and trailing spaces
		strVersion_3.TrimRight();
		strVersion_3.TrimLeft();

		if( strVersion_3.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", ((m_pScript->m_vVersion.dwVersionLS & 0xFFFF0000) >> 16) );
			m_editVersion_3.SetWindowText( strText );
		}
		else
		{
			strScriptVersion_3.Format( "%u", ((m_pScript->m_vVersion.dwVersionLS & 0xFFFF0000) >> 16) );
			
			if( strVersion_3.Compare( strScriptVersion_3 ) != 0 )
			{
				DWORD dwVersion_3 = _ttoi( strVersion_3 );
				dwVersion_3 = (dwVersion_3 & 0x0000FFFF) << 16;

				m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_VERSION );
				m_pScript->m_vVersion.dwVersionLS &= 0x0000FFFF;
				m_pScript->m_vVersion.dwVersionLS |= dwVersion_3;
				SetModifiedFlag();

				// Sync Script with DirectMusic
				m_pScript->SyncScriptWithDirectMusic();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::OnKillFocusVersion_4

void CTabScriptInfo::OnKillFocusVersion_4() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		CString strVersion_4;
		CString strScriptVersion_4;

		m_editVersion_4.GetWindowText( strVersion_4 );

		// Strip leading and trailing spaces
		strVersion_4.TrimRight();
		strVersion_4.TrimLeft();

		if( strVersion_4.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", (m_pScript->m_vVersion.dwVersionLS & 0x0000FFFF) );
			m_editVersion_4.SetWindowText( strText );
		}
		else
		{
			strScriptVersion_4.Format( "%u", (m_pScript->m_vVersion.dwVersionLS & 0x0000FFFF) );
			
			if( strVersion_4.Compare( strScriptVersion_4 ) != 0 )
			{
				DWORD dwVersion_4 = _ttoi( strVersion_4 );
				dwVersion_4 &= 0x0000FFFF;

				m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_VERSION );
				m_pScript->m_vVersion.dwVersionLS &= 0xFFFF0000;
				m_pScript->m_vVersion.dwVersionLS |= dwVersion_4;
				SetModifiedFlag();

				// Sync Script with DirectMusic
				m_pScript->SyncScriptWithDirectMusic();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabScriptInfo::OnEditGUID

void CTabScriptInfo::OnEditGuid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		CGuidDlg dlg;

		memcpy( &dlg.m_guid, &m_pScript->m_guidScript, sizeof(GUID) );

		if( dlg.DoModal() == IDOK )
		{
			m_pScript->m_pUndoMgr->SaveState( m_pScript, theApp.m_hInstance, IDS_UNDO_SCRIPT_GUID );
			memcpy( &m_pScript->m_guidScript, &dlg.m_guid, sizeof(GUID) );
			SetModifiedFlag();

			// Sync Script with DirectMusic
			m_pScript->SyncScriptWithDirectMusic();

			// Notify connected nodes that Script GUID has changed
			theApp.m_pScriptComponent->m_pIFramework->NotifyNodes( m_pScript, DOCROOT_GuidChange, NULL );
		}
	}
}
