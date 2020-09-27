// TabStyleInfo.cpp : implementation file
//

#include "stdafx.h"
#include "TabStyleInfo.h"
#include "StyleDesignerDll.h"
#include "Style.h"
#include <guiddlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo property page

CTabStyleInfo::CTabStyleInfo( CStylePropPageManager* pStylePropPageManager ) : CPropertyPage(CTabStyleInfo::IDD)
{
	//{{AFX_DATA_INIT(CTabStyleInfo)
	//}}AFX_DATA_INIT
	
	ASSERT( pStylePropPageManager != NULL );

	m_pStyle = NULL;
	m_pPageManager = pStylePropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabStyleInfo::~CTabStyleInfo()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::SetStyle

void CTabStyleInfo::SetStyle( CDirectMusicStyle* pStyle )
{
	m_pStyle = pStyle;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::SetModifiedFlag

void CTabStyleInfo::SetModifiedFlag( void ) 
{
	ASSERT( m_pStyle != NULL );

	m_pStyle->SetModified( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::EnableControls

void CTabStyleInfo::EnableControls( BOOL fEnable ) 
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
// CTabStyleInfo::UpdateControls

void CTabStyleInfo::UpdateControls() 
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

	if( m_pStyle )
	{
		CString strText;

		EnableControls( TRUE );

		m_editAuthor.SetWindowText( m_pStyle->m_strAuthor );
		m_editCopyright.SetWindowText( m_pStyle->m_strCopyright );
		m_editInfo.SetWindowText( m_pStyle->m_strInfo );
		m_editSubject.SetWindowText( m_pStyle->m_strSubject );
		
		strText.Format( "%u", ((m_pStyle->m_dwVersionMS & 0xFFFF0000) >> 16) );
		m_editVersion_1.SetWindowText( strText );
		
		strText.Format( "%u", (m_pStyle->m_dwVersionMS & 0x0000FFFF) );
		m_editVersion_2.SetWindowText( strText );
		
		strText.Format( "%u", ((m_pStyle->m_dwVersionLS & 0xFFFF0000) >> 16) );
		m_editVersion_3.SetWindowText( strText );
		
		strText.Format( "%u", (m_pStyle->m_dwVersionLS & 0x0000FFFF) );
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


void CTabStyleInfo::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabStyleInfo)
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


BEGIN_MESSAGE_MAP(CTabStyleInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CTabStyleInfo)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_AUTHOR, OnKillFocusAuthor)
	ON_EN_KILLFOCUS(IDC_COPYRIGHT, OnKillFocusCopyright)
	ON_EN_KILLFOCUS(IDC_INFO, OnKillFocusInfo)
	ON_EN_KILLFOCUS(IDC_SUBJECT, OnKillFocusSubject)
	ON_EN_KILLFOCUS(IDC_VERSION_1, OnKillFocusVersion_1)
	ON_EN_KILLFOCUS(IDC_VERSION_2, OnKillFocusVersion_2)
	ON_EN_KILLFOCUS(IDC_VERSION_3, OnKillFocusVersion_3)
	ON_EN_KILLFOCUS(IDC_VERSION_4, OnKillFocusVersion_4)
	ON_BN_CLICKED(IDC_EDIT_GUID, OnEditGuid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::OnSetActive

BOOL CTabStyleInfo::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CStylePropPageManager::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::OnCreate

int CTabStyleInfo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabStyleInfo::OnDestroy

void CTabStyleInfo::OnDestroy() 
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
// CTabStyleInfo::OnKillFocusAuthor

void CTabStyleInfo::OnKillFocusAuthor() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CString strAuthor;

		m_editAuthor.GetWindowText( strAuthor );

		// Strip leading and trailing spaces
		strAuthor.TrimRight();
		strAuthor.TrimLeft();

		if( strAuthor.Compare( m_pStyle->m_strAuthor ) != 0 )
		{
			m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_AUTHOR );
			m_pStyle->m_strAuthor = strAuthor;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::OnKillFocusCopyright

void CTabStyleInfo::OnKillFocusCopyright() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CString strCopyright;

		m_editCopyright.GetWindowText( strCopyright );

		// Strip leading and trailing spaces
		strCopyright.TrimRight();
		strCopyright.TrimLeft();

		if( strCopyright.Compare( m_pStyle->m_strCopyright ) != 0 )
		{
			m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_COPYRIGHT );
			m_pStyle->m_strCopyright = strCopyright;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::OnKillFocusInfo

void CTabStyleInfo::OnKillFocusInfo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CString strInfo;

		m_editInfo.GetWindowText( strInfo );

		// Strip leading and trailing spaces
		strInfo.TrimRight();
		strInfo.TrimLeft();

		if( strInfo.Compare( m_pStyle->m_strInfo ) != 0 )
		{
			m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_INFO );
			m_pStyle->m_strInfo = strInfo;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::OnKillFocusSubject

void CTabStyleInfo::OnKillFocusSubject() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CString strSubject;

		m_editSubject.GetWindowText( strSubject );

		// Strip leading and trailing spaces
		strSubject.TrimRight();
		strSubject.TrimLeft();

		if( strSubject.Compare( m_pStyle->m_strSubject ) != 0 )
		{
			m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_SUBJECT );
			m_pStyle->m_strSubject = strSubject;
			SetModifiedFlag();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::OnKillFocusVersion_1

void CTabStyleInfo::OnKillFocusVersion_1() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CString strVersion_1;
		CString strStyleVersion_1;

		m_editVersion_1.GetWindowText( strVersion_1 );

		// Strip leading and trailing spaces
		strVersion_1.TrimRight();
		strVersion_1.TrimLeft();

		if( strVersion_1.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", ((m_pStyle->m_dwVersionMS & 0xFFFF0000) >> 16) );
			m_editVersion_1.SetWindowText( strText );
		}
		else
		{
			strStyleVersion_1.Format( "%u", ((m_pStyle->m_dwVersionMS & 0xFFFF0000) >> 16) );
			
			if( strVersion_1.Compare( strStyleVersion_1 ) != 0 )
			{
				DWORD dwVersion_1 = _ttoi( strVersion_1 );
				dwVersion_1 = (dwVersion_1 & 0x0000FFFF) << 16;

				m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_VERSION );
				m_pStyle->m_dwVersionMS &= 0x0000FFFF;
				m_pStyle->m_dwVersionMS |= dwVersion_1;
				SetModifiedFlag();

				// Sync Style with DirectMusic
				m_pStyle->SyncStyleWithDirectMusic();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::OnKillFocusVersion_2

void CTabStyleInfo::OnKillFocusVersion_2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CString strVersion_2;
		CString strStyleVersion_2;

		m_editVersion_2.GetWindowText( strVersion_2 );

		// Strip leading and trailing spaces
		strVersion_2.TrimRight();
		strVersion_2.TrimLeft();

		if( strVersion_2.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", (m_pStyle->m_dwVersionMS & 0x0000FFFF) );
			m_editVersion_2.SetWindowText( strText );
		}
		else
		{
			strStyleVersion_2.Format( "%u", (m_pStyle->m_dwVersionMS & 0x0000FFFF) );
			
			if( strVersion_2.Compare( strStyleVersion_2 ) != 0 )
			{
				DWORD dwVersion_2 = _ttoi( strVersion_2 );
				dwVersion_2 &= 0x0000FFFF;

				m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_VERSION );
				m_pStyle->m_dwVersionMS &= 0xFFFF0000;
				m_pStyle->m_dwVersionMS |= dwVersion_2;
				SetModifiedFlag();

				// Sync Style with DirectMusic
				m_pStyle->SyncStyleWithDirectMusic();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::OnKillFocusVersion_3

void CTabStyleInfo::OnKillFocusVersion_3() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CString strVersion_3;
		CString strStyleVersion_3;

		m_editVersion_3.GetWindowText( strVersion_3 );

		// Strip leading and trailing spaces
		strVersion_3.TrimRight();
		strVersion_3.TrimLeft();

		if( strVersion_3.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", ((m_pStyle->m_dwVersionLS & 0xFFFF0000) >> 16) );
			m_editVersion_3.SetWindowText( strText );
		}
		else
		{
			strStyleVersion_3.Format( "%u", ((m_pStyle->m_dwVersionLS & 0xFFFF0000) >> 16) );
			
			if( strVersion_3.Compare( strStyleVersion_3 ) != 0 )
			{
				DWORD dwVersion_3 = _ttoi( strVersion_3 );
				dwVersion_3 = (dwVersion_3 & 0x0000FFFF) << 16;

				m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_VERSION );
				m_pStyle->m_dwVersionLS &= 0x0000FFFF;
				m_pStyle->m_dwVersionLS |= dwVersion_3;
				SetModifiedFlag();

				// Sync Style with DirectMusic
				m_pStyle->SyncStyleWithDirectMusic();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabStyleInfo::OnKillFocusVersion_4

void CTabStyleInfo::OnKillFocusVersion_4() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CString strVersion_4;
		CString strStyleVersion_4;

		m_editVersion_4.GetWindowText( strVersion_4 );

		// Strip leading and trailing spaces
		strVersion_4.TrimRight();
		strVersion_4.TrimLeft();

		if( strVersion_4.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", (m_pStyle->m_dwVersionLS & 0x0000FFFF) );
			m_editVersion_4.SetWindowText( strText );
		}
		else
		{
			strStyleVersion_4.Format( "%u", (m_pStyle->m_dwVersionLS & 0x0000FFFF) );
			
			if( strVersion_4.Compare( strStyleVersion_4 ) != 0 )
			{
				DWORD dwVersion_4 = _ttoi( strVersion_4 );
				dwVersion_4 &= 0x0000FFFF;

				m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_VERSION );
				m_pStyle->m_dwVersionLS &= 0xFFFF0000;
				m_pStyle->m_dwVersionLS |= dwVersion_4;
				SetModifiedFlag();

				// Sync Style with DirectMusic
				m_pStyle->SyncStyleWithDirectMusic();
			}
		}
	}
}

void CTabStyleInfo::OnEditGuid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		CGuidDlg dlg;
		memcpy( &dlg.m_guid, &m_pStyle->m_guidStyle, sizeof(GUID) );
		if( dlg.DoModal() == IDOK )
		{
			m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_GUID );
			memcpy( &m_pStyle->m_guidStyle, &dlg.m_guid, sizeof(GUID) );
			SetModifiedFlag();

			// Sync Style with DirectMusic
			m_pStyle->SyncStyleWithDirectMusic();

			// Notify connected nodes that Style GUID has changed
			theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( m_pStyle, DOCROOT_GuidChange, NULL );
		}
	}
}
