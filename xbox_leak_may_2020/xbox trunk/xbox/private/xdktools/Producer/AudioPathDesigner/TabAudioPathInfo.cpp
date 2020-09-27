// TabAudioPathInfo.cpp : implementation file
//

#include "stdafx.h"
#include "TabAudioPathInfo.h"
#include "AudioPathDesignerDll.h"
#include "AudioPath.h"
#include <guiddlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo property page

CTabAudioPathInfo::CTabAudioPathInfo( CAudioPathPropPageManager* pAudioPathPropPageManager ) : CPropertyPage(CTabAudioPathInfo::IDD)
{
	//{{AFX_DATA_INIT(CTabAudioPathInfo)
	//}}AFX_DATA_INIT
	
	ASSERT( pAudioPathPropPageManager != NULL );

	m_pAudioPath = NULL;
	m_pPageManager = pAudioPathPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabAudioPathInfo::~CTabAudioPathInfo()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::SetAudioPath

void CTabAudioPathInfo::SetAudioPath( CDirectMusicAudioPath* pAudioPath )
{
	m_pAudioPath = pAudioPath;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::SetModifiedFlag

void CTabAudioPathInfo::SetModifiedFlag( void ) 
{
	ASSERT( m_pAudioPath != NULL );

	m_pAudioPath->SetModified( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::EnableControls

void CTabAudioPathInfo::EnableControls( BOOL fEnable ) 
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
// CTabAudioPathInfo::UpdateControls

void CTabAudioPathInfo::UpdateControls() 
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

	if( m_pAudioPath )
	{
		CString strText;

		EnableControls( TRUE );

		m_editAuthor.SetWindowText( m_pAudioPath->m_strAuthor );
		m_editCopyright.SetWindowText( m_pAudioPath->m_strCopyright );
		m_editSubject.SetWindowText( m_pAudioPath->m_strSubject );
		
		strText.Format( "%u", ((m_pAudioPath->m_vVersion.dwVersionMS & 0xFFFF0000) >> 16) );
		m_editVersion_1.SetWindowText( strText );
		
		strText.Format( "%u", (m_pAudioPath->m_vVersion.dwVersionMS & 0x0000FFFF) );
		m_editVersion_2.SetWindowText( strText );
		
		strText.Format( "%u", ((m_pAudioPath->m_vVersion.dwVersionLS & 0xFFFF0000) >> 16) );
		m_editVersion_3.SetWindowText( strText );
		
		strText.Format( "%u", (m_pAudioPath->m_vVersion.dwVersionLS & 0x0000FFFF) );
		m_editVersion_4.SetWindowText( strText );
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

		EnableControls( FALSE );
	}
}


void CTabAudioPathInfo::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabAudioPathInfo)
	DDX_Control(pDX, IDC_VERSION_4, m_editVersion_4);
	DDX_Control(pDX, IDC_VERSION_3, m_editVersion_3);
	DDX_Control(pDX, IDC_VERSION_2, m_editVersion_2);
	DDX_Control(pDX, IDC_VERSION_1, m_editVersion_1);
	DDX_Control(pDX, IDC_SUBJECT, m_editSubject);
	DDX_Control(pDX, IDC_COPYRIGHT, m_editCopyright);
	DDX_Control(pDX, IDC_AUTHOR, m_editAuthor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabAudioPathInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CTabAudioPathInfo)
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
// CTabAudioPathInfo message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::OnSetActive

BOOL CTabAudioPathInfo::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CAudioPathPropPageManager::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::OnCreate

int CTabAudioPathInfo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabAudioPathInfo::OnDestroy

void CTabAudioPathInfo::OnDestroy() 
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
// CTabAudioPathInfo::OnKillFocusAuthor

void CTabAudioPathInfo::OnKillFocusAuthor() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		CString strAuthor;

		m_editAuthor.GetWindowText( strAuthor );

		// Strip leading and trailing spaces
		strAuthor.TrimRight();
		strAuthor.TrimLeft();

		if( strAuthor.Compare( m_pAudioPath->m_strAuthor ) != 0 )
		{
			m_pAudioPath->SaveUndoState( IDS_UNDO_AUDIOPATH_AUTHOR );
			m_pAudioPath->m_strAuthor = strAuthor;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::OnKillFocusCopyright

void CTabAudioPathInfo::OnKillFocusCopyright() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		CString strCopyright;

		m_editCopyright.GetWindowText( strCopyright );

		// Strip leading and trailing spaces
		strCopyright.TrimRight();
		strCopyright.TrimLeft();

		if( strCopyright.Compare( m_pAudioPath->m_strCopyright ) != 0 )
		{
			m_pAudioPath->SaveUndoState( IDS_UNDO_AUDIOPATH_COPYRIGHT );
			m_pAudioPath->m_strCopyright = strCopyright;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::OnKillFocusSubject

void CTabAudioPathInfo::OnKillFocusSubject() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		CString strSubject;

		m_editSubject.GetWindowText( strSubject );

		// Strip leading and trailing spaces
		strSubject.TrimRight();
		strSubject.TrimLeft();

		if( strSubject.Compare( m_pAudioPath->m_strSubject ) != 0 )
		{
			m_pAudioPath->SaveUndoState( IDS_UNDO_AUDIOPATH_SUBJECT );
			m_pAudioPath->m_strSubject = strSubject;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::OnKillFocusVersion_1

void CTabAudioPathInfo::OnKillFocusVersion_1() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		CString strVersion_1;
		CString strAudioPathVersion_1;

		m_editVersion_1.GetWindowText( strVersion_1 );

		// Strip leading and trailing spaces
		strVersion_1.TrimRight();
		strVersion_1.TrimLeft();

		if( strVersion_1.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", ((m_pAudioPath->m_vVersion.dwVersionMS & 0xFFFF0000) >> 16) );
			m_editVersion_1.SetWindowText( strText );
		}
		else
		{
			strAudioPathVersion_1.Format( "%u", ((m_pAudioPath->m_vVersion.dwVersionMS & 0xFFFF0000) >> 16) );
			
			if( strVersion_1.Compare( strAudioPathVersion_1 ) != 0 )
			{
				DWORD dwVersion_1 = _ttoi( strVersion_1 );
				dwVersion_1 = (dwVersion_1 & 0x0000FFFF) << 16;

				m_pAudioPath->SaveUndoState( IDS_UNDO_AUDIOPATH_VERSION );
				m_pAudioPath->m_vVersion.dwVersionMS &= 0x0000FFFF;
				m_pAudioPath->m_vVersion.dwVersionMS |= dwVersion_1;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::OnKillFocusVersion_2

void CTabAudioPathInfo::OnKillFocusVersion_2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		CString strVersion_2;
		CString strAudioPathVersion_2;

		m_editVersion_2.GetWindowText( strVersion_2 );

		// Strip leading and trailing spaces
		strVersion_2.TrimRight();
		strVersion_2.TrimLeft();

		if( strVersion_2.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", (m_pAudioPath->m_vVersion.dwVersionMS & 0x0000FFFF) );
			m_editVersion_2.SetWindowText( strText );
		}
		else
		{
			strAudioPathVersion_2.Format( "%u", (m_pAudioPath->m_vVersion.dwVersionMS & 0x0000FFFF) );
			
			if( strVersion_2.Compare( strAudioPathVersion_2 ) != 0 )
			{
				DWORD dwVersion_2 = _ttoi( strVersion_2 );
				dwVersion_2 &= 0x0000FFFF;

				m_pAudioPath->SaveUndoState( IDS_UNDO_AUDIOPATH_VERSION );
				m_pAudioPath->m_vVersion.dwVersionMS &= 0xFFFF0000;
				m_pAudioPath->m_vVersion.dwVersionMS |= dwVersion_2;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::OnKillFocusVersion_3

void CTabAudioPathInfo::OnKillFocusVersion_3() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		CString strVersion_3;
		CString strAudioPathVersion_3;

		m_editVersion_3.GetWindowText( strVersion_3 );

		// Strip leading and trailing spaces
		strVersion_3.TrimRight();
		strVersion_3.TrimLeft();

		if( strVersion_3.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", ((m_pAudioPath->m_vVersion.dwVersionLS & 0xFFFF0000) >> 16) );
			m_editVersion_3.SetWindowText( strText );
		}
		else
		{
			strAudioPathVersion_3.Format( "%u", ((m_pAudioPath->m_vVersion.dwVersionLS & 0xFFFF0000) >> 16) );
			
			if( strVersion_3.Compare( strAudioPathVersion_3 ) != 0 )
			{
				DWORD dwVersion_3 = _ttoi( strVersion_3 );
				dwVersion_3 = (dwVersion_3 & 0x0000FFFF) << 16;

				m_pAudioPath->SaveUndoState( IDS_UNDO_AUDIOPATH_VERSION );
				m_pAudioPath->m_vVersion.dwVersionLS &= 0x0000FFFF;
				m_pAudioPath->m_vVersion.dwVersionLS |= dwVersion_3;

			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::OnKillFocusVersion_4

void CTabAudioPathInfo::OnKillFocusVersion_4() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		CString strVersion_4;
		CString strAudioPathVersion_4;

		m_editVersion_4.GetWindowText( strVersion_4 );

		// Strip leading and trailing spaces
		strVersion_4.TrimRight();
		strVersion_4.TrimLeft();

		if( strVersion_4.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", (m_pAudioPath->m_vVersion.dwVersionLS & 0x0000FFFF) );
			m_editVersion_4.SetWindowText( strText );
		}
		else
		{
			strAudioPathVersion_4.Format( "%u", (m_pAudioPath->m_vVersion.dwVersionLS & 0x0000FFFF) );
			
			if( strVersion_4.Compare( strAudioPathVersion_4 ) != 0 )
			{
				DWORD dwVersion_4 = _ttoi( strVersion_4 );
				dwVersion_4 &= 0x0000FFFF;

				m_pAudioPath->SaveUndoState( IDS_UNDO_AUDIOPATH_VERSION );
				m_pAudioPath->m_vVersion.dwVersionLS &= 0xFFFF0000;
				m_pAudioPath->m_vVersion.dwVersionLS |= dwVersion_4;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudioPathInfo::OnEditGUID

void CTabAudioPathInfo::OnEditGuid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath )
	{
		CGuidDlg dlg;

		memcpy( &dlg.m_guid, &m_pAudioPath->m_guidAudioPath, sizeof(GUID) );

		if( dlg.DoModal() == IDOK )
		{
			m_pAudioPath->SaveUndoState( IDS_UNDO_AUDIOPATH_GUID );
			memcpy( &m_pAudioPath->m_guidAudioPath, &dlg.m_guid, sizeof(GUID) );

			// Notify connected nodes that AudioPath GUID has changed
			theApp.m_pAudioPathComponent->m_pIFramework->NotifyNodes( m_pAudioPath, DOCROOT_GuidChange, NULL );
		}
	}
}
