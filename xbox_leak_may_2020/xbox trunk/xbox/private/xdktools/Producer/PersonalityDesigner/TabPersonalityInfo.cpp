// TabPersonalityInfo.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(push)
#pragma warning(disable:4201)
#include "personalitydesigner.h"
#include "Personality.h"
#pragma warning(push)
#include <guiddlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo property page


CTabPersonalityInfo::CTabPersonalityInfo(CPersonalityPageManager* pPageManager) : CPropertyPage(CTabPersonalityInfo::IDD)
{
	//{{AFX_DATA_INIT(CTabPersonalityInfo)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );

	m_pPersonality = NULL;
	m_pPageManager = pPageManager;
	m_fNeedToDetach = FALSE;

}

CTabPersonalityInfo::~CTabPersonalityInfo()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::SetPersonality

void CTabPersonalityInfo::SetPersonality( CPersonality* pPersonality )
{
	m_pPersonality = pPersonality;

	UpdateControls();
}

void CTabPersonalityInfo::SetModifiedFlag()
{
	ASSERT(m_pPersonality != NULL);
	m_pPersonality->Modified() = TRUE;;
}

/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::EnableControls

void CTabPersonalityInfo::EnableControls( BOOL fEnable ) 
{
	m_editAuthor.EnableWindow( fEnable );
	m_editCopyright.EnableWindow( fEnable );
	m_editInfo.EnableWindow( fEnable );
	m_editSubject.EnableWindow( fEnable );
	m_editVersion_1.EnableWindow( fEnable );
	m_editVersion_2.EnableWindow( fEnable );
	m_editVersion_3.EnableWindow( fEnable );
	m_editVersion_4.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::UpdateControls

void CTabPersonalityInfo::UpdateControls() 
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

	if( m_pPersonality )
	{
		CString strText;

		EnableControls( TRUE );

		m_editAuthor.SetWindowText( m_pPersonality->m_strAuthor );
		m_editCopyright.SetWindowText( m_pPersonality->m_strCopyright );
		m_editInfo.SetWindowText( m_pPersonality->m_strInfo );
		m_editSubject.SetWindowText( m_pPersonality->m_strSubject );
		
		strText.Format( "%u", ((m_pPersonality->m_dwVersionMS & 0xFFFF0000) >> 16) );
		m_editVersion_1.SetWindowText( strText );
		
		strText.Format( "%u", (m_pPersonality->m_dwVersionMS & 0x0000FFFF) );
		m_editVersion_2.SetWindowText( strText );
		
		strText.Format( "%u", ((m_pPersonality->m_dwVersionLS & 0xFFFF0000) >> 16) );
		m_editVersion_3.SetWindowText( strText );
		
		strText.Format( "%u", (m_pPersonality->m_dwVersionLS & 0x0000FFFF) );
		m_editVersion_4.SetWindowText( strText );
	}
	else
	{
		m_editAuthor.SetWindowText( _T("") );
		m_editCopyright.SetWindowText( _T("") );
		m_editInfo.SetWindowText( _T("") );
		m_editSubject.SetWindowText( _T(""));
		m_editVersion_1.SetWindowText( _T("") );
		m_editVersion_2.SetWindowText( _T("") );
		m_editVersion_3.SetWindowText( _T("") );
		m_editVersion_4.SetWindowText( _T("") );

		EnableControls( FALSE );
	}
}



void CTabPersonalityInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabPersonalityInfo)
	DDX_Control(pDX, IDC_VERSION_4, m_editVersion_4);
	DDX_Control(pDX, IDC_VERSION_3, m_editVersion_3);
	DDX_Control(pDX, IDC_VERSION_2, m_editVersion_2);
	DDX_Control(pDX, IDC_VERSION_1, m_editVersion_1);
	DDX_Control(pDX, IDC_SUBJECT, m_editSubject);
	DDX_Control(pDX, IDC_INFO, m_editInfo);
	DDX_Control(pDX, IDC_COPYRIGHT, m_editCopyright);
	DDX_Control(pDX, IDC_AUTHOR, m_editAuthor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabPersonalityInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CTabPersonalityInfo)
	ON_EN_KILLFOCUS(IDC_VERSION_4, OnKillFocusVersion_4)
	ON_EN_KILLFOCUS(IDC_VERSION_3, OnKillFocusVersion_3)
	ON_EN_KILLFOCUS(IDC_VERSION_2, OnKillFocusVersion_2)
	ON_EN_KILLFOCUS(IDC_VERSION_1, OnKillFocusVersion_1)
	ON_EN_KILLFOCUS(IDC_SUBJECT, OnKillFocusSubject)
	ON_EN_KILLFOCUS(IDC_INFO, OnKillFocusInfo)
	ON_EN_KILLFOCUS(IDC_COPYRIGHT, OnKillFocusCopyright)
	ON_EN_KILLFOCUS(IDC_AUTHOR, OnKillFocusAuthor)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_EDIT_GUID, OnEditGuid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo message handlers
/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::OnKillFocusAuthor

void CTabPersonalityInfo::OnKillFocusAuthor() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality )
	{
		CString strAuthor;

		m_editAuthor.GetWindowText( strAuthor );

		// Strip leading and trailing spaces
		strAuthor.TrimRight();
		strAuthor.TrimLeft();

		if( strAuthor.Compare( m_pPersonality->m_strAuthor ) != 0 )
		{
			m_pPersonality->m_pUndoMgr->SaveState( m_pPersonality, theApp.m_hInstance, IDS_UNDO_Personality_AUTHOR );
			m_pPersonality->m_strAuthor = strAuthor;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::OnKillFocusCopyright

void CTabPersonalityInfo::OnKillFocusCopyright() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality )
	{
		CString strCopyright;

		m_editCopyright.GetWindowText( strCopyright );

		// Strip leading and trailing spaces
		strCopyright.TrimRight();
		strCopyright.TrimLeft();

		if( strCopyright.Compare( m_pPersonality->m_strCopyright ) != 0 )
		{
			m_pPersonality->m_pUndoMgr->SaveState( m_pPersonality, theApp.m_hInstance, IDS_UNDO_Personality_COPYRIGHT );
			m_pPersonality->m_strCopyright = strCopyright;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::OnKillFocusInfo

void CTabPersonalityInfo::OnKillFocusInfo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality )
	{
		CString strInfo;

		m_editInfo.GetWindowText( strInfo );

		// Strip leading and trailing spaces
		strInfo.TrimRight();
		strInfo.TrimLeft();

		if( strInfo.Compare( m_pPersonality->m_strInfo ) != 0 )
		{
			m_pPersonality->m_pUndoMgr->SaveState( m_pPersonality, theApp.m_hInstance, IDS_UNDO_Personality_INFO );
			m_pPersonality->m_strInfo = strInfo;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::OnKillFocusSubject

void CTabPersonalityInfo::OnKillFocusSubject() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality )
	{
		CString strSubject;

		m_editSubject.GetWindowText( strSubject );

		// Strip leading and trailing spaces
		strSubject.TrimRight();
		strSubject.TrimLeft();

		if( strSubject.Compare( m_pPersonality->m_strSubject ) != 0 )
		{
			m_pPersonality->m_pUndoMgr->SaveState( m_pPersonality, theApp.m_hInstance, IDS_UNDO_Personality_SUBJECT );
			m_pPersonality->m_strSubject = strSubject;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::OnKillFocusVersion_1

void CTabPersonalityInfo::OnKillFocusVersion_1() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality )
	{
		CString strVersion_1;
		CString strPersonalityVersion_1;

		m_editVersion_1.GetWindowText( strVersion_1 );

		// Strip leading and trailing spaces
		strVersion_1.TrimRight();
		strVersion_1.TrimLeft();

		if( strVersion_1.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", ((m_pPersonality->m_dwVersionMS & 0xFFFF0000) >> 16) );
			m_editVersion_1.SetWindowText( strText );
		}
		else
		{
			strPersonalityVersion_1.Format( "%u", ((m_pPersonality->m_dwVersionMS & 0xFFFF0000) >> 16) );
			
			if( strVersion_1.Compare( strPersonalityVersion_1 ) != 0 )
			{
				DWORD dwVersion_1 = _ttoi( strVersion_1 );
				dwVersion_1 = (dwVersion_1 & 0x0000FFFF) << 16;

				m_pPersonality->m_pUndoMgr->SaveState( m_pPersonality, theApp.m_hInstance, IDS_UNDO_Personality_VERSION );
				m_pPersonality->m_dwVersionMS &= 0x0000FFFF;
				m_pPersonality->m_dwVersionMS |= dwVersion_1;
				SetModifiedFlag();

				// Sync personality (with new version) to engine
				m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::OnKillFocusVersion_2

void CTabPersonalityInfo::OnKillFocusVersion_2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality )
	{
		CString strVersion_2;
		CString strPersonalityVersion_2;

		m_editVersion_2.GetWindowText( strVersion_2 );

		// Strip leading and trailing spaces
		strVersion_2.TrimRight();
		strVersion_2.TrimLeft();

		if( strVersion_2.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", (m_pPersonality->m_dwVersionMS & 0x0000FFFF) );
			m_editVersion_2.SetWindowText( strText );
		}
		else
		{
			strPersonalityVersion_2.Format( "%u", (m_pPersonality->m_dwVersionMS & 0x0000FFFF) );
			
			if( strVersion_2.Compare( strPersonalityVersion_2 ) != 0 )
			{
				DWORD dwVersion_2 = _ttoi( strVersion_2 );
				dwVersion_2 &= 0x0000FFFF;

				m_pPersonality->m_pUndoMgr->SaveState( m_pPersonality, theApp.m_hInstance, IDS_UNDO_Personality_VERSION );
				m_pPersonality->m_dwVersionMS &= 0xFFFF0000;
				m_pPersonality->m_dwVersionMS |= dwVersion_2;
				SetModifiedFlag();

				// Sync personality (with new version) to engine
				m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::OnKillFocusVersion_3

void CTabPersonalityInfo::OnKillFocusVersion_3() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality )
	{
		CString strVersion_3;
		CString strPersonalityVersion_3;

		m_editVersion_3.GetWindowText( strVersion_3 );

		// Strip leading and trailing spaces
		strVersion_3.TrimRight();
		strVersion_3.TrimLeft();

		if( strVersion_3.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", ((m_pPersonality->m_dwVersionLS & 0xFFFF0000) >> 16) );
			m_editVersion_3.SetWindowText( strText );
		}
		else
		{
			strPersonalityVersion_3.Format( "%u", ((m_pPersonality->m_dwVersionLS & 0xFFFF0000) >> 16) );
			
			if( strVersion_3.Compare( strPersonalityVersion_3 ) != 0 )
			{
				DWORD dwVersion_3 = _ttoi( strVersion_3 );
				dwVersion_3 = (dwVersion_3 & 0x0000FFFF) << 16;

				m_pPersonality->m_pUndoMgr->SaveState( m_pPersonality, theApp.m_hInstance, IDS_UNDO_Personality_VERSION );
				m_pPersonality->m_dwVersionLS &= 0x0000FFFF;
				m_pPersonality->m_dwVersionLS |= dwVersion_3;
				SetModifiedFlag();

				// Sync personality (with new version) to engine
				m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonalityInfo::OnKillFocusVersion_4

void CTabPersonalityInfo::OnKillFocusVersion_4() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality )
	{
		CString strVersion_4;
		CString strPersonalityVersion_4;

		m_editVersion_4.GetWindowText( strVersion_4 );

		// Strip leading and trailing spaces
		strVersion_4.TrimRight();
		strVersion_4.TrimLeft();

		if( strVersion_4.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", (m_pPersonality->m_dwVersionLS & 0x0000FFFF) );
			m_editVersion_4.SetWindowText( strText );
		}
		else
		{
			strPersonalityVersion_4.Format( "%u", (m_pPersonality->m_dwVersionLS & 0x0000FFFF) );
			
			if( strVersion_4.Compare( strPersonalityVersion_4 ) != 0 )
			{
				DWORD dwVersion_4 = _ttoi( strVersion_4 );
				dwVersion_4 &= 0x0000FFFF;

				m_pPersonality->m_pUndoMgr->SaveState( m_pPersonality, theApp.m_hInstance, IDS_UNDO_Personality_VERSION );
				m_pPersonality->m_dwVersionLS &= 0xFFFF0000;
				m_pPersonality->m_dwVersionLS |= dwVersion_4;
				SetModifiedFlag();

				// Sync personality (with new version) to engine
				m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);
			}
		}
	}
}


int CTabPersonalityInfo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

BOOL CTabPersonalityInfo::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CPersonalityPageManager::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}

void CTabPersonalityInfo::OnDestroy() 
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

BOOL CTabPersonalityInfo::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality == NULL )
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
				case VK_ESCAPE:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_AUTHOR: 
								m_editAuthor.SetWindowText( m_pPersonality->m_strAuthor );
								break;

							case IDC_COPYRIGHT: 
								m_editCopyright.SetWindowText( m_pPersonality->m_strCopyright );
								break;

							case IDC_INFO: 
								m_editInfo.SetWindowText( m_pPersonality->m_strInfo );
								break;

							case IDC_SUBJECT: 
								m_editSubject.SetWindowText( m_pPersonality->m_strSubject );
								break;

							case IDC_VERSION_1:
							{
								CString strText;

								strText.Format( "%u", ((m_pPersonality->m_dwVersionMS & 0xFFFF0000) >> 16) );
								m_editVersion_1.SetWindowText( strText );
								break;
							}

							case IDC_VERSION_2: 
							{
								CString strText;
	
								strText.Format( "%u", (m_pPersonality->m_dwVersionMS & 0x0000FFFF) );
								m_editVersion_2.SetWindowText( strText );
								break;
							}

							case IDC_VERSION_3: 
							{
								CString strText;
	
								strText.Format( "%u", ((m_pPersonality->m_dwVersionLS & 0xFFFF0000) >> 16) );
								m_editVersion_3.SetWindowText( strText );
								break;
							}

							case IDC_VERSION_4: 
							{
								CString strText;
	
								strText.Format( "%u", (m_pPersonality->m_dwVersionLS & 0x0000FFFF) );
								m_editVersion_4.SetWindowText( strText );
								break;
							}
						}
					}
					return TRUE;
				}

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

void CTabPersonalityInfo::OnEditGuid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality )
	{
		CGuidDlg dlg;
		memcpy( &dlg.m_guid, &m_pPersonality->m_guidPersonality, sizeof(GUID) );
		if( dlg.DoModal() == IDOK )
		{
			m_pPersonality->m_pUndoMgr->SaveState( m_pPersonality, theApp.m_hInstance, IDS_UNDO_GUID );
			memcpy( &m_pPersonality->m_guidPersonality, &dlg.m_guid, sizeof(GUID) );
			SetModifiedFlag();

			// Sync personality (with new GUID) to engine
			m_pPersonality->SyncPersonalityWithEngine(CPersonality::syncPersonality);

			// Notify connected nodes that Personality GUID has changed
			m_pPersonality->m_pComponent->m_pIFramework->NotifyNodes( m_pPersonality, DOCROOT_GuidChange, NULL );
		}
	}
}
