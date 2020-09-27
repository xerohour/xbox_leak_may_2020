// ProjectPropTabFolders.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "ProjectPropTabFolders.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders property page

CProjectPropTabFolders::CProjectPropTabFolders( CProjectPropPageManager* pPageManager ) : CPropertyPage(CProjectPropTabFolders::IDD)
{
	//{{AFX_DATA_INIT(CProjectPropTabFolders)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );

	m_pProject = NULL;
	m_pPageManager = pPageManager;
}

CProjectPropTabFolders::~CProjectPropTabFolders()
{
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders::SetProject

void CProjectPropTabFolders::SetProject( CProject* pProject )
{
	m_pProject = pProject;
}


void CProjectPropTabFolders::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectPropTabFolders)
	DDX_Control(pDX, IDC_FOLDER, m_editFolder);
	DDX_Control(pDX, IDC_FOLDER_LIST, m_lstbxFolders);
	DDX_Control(pDX, IDC_PROMPT, m_staticPrompt);
	DDX_Control(pDX, IDC_BROWSE, m_btnBrowse);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders::EnableControls

void CProjectPropTabFolders::EnableControls( BOOL fEnable ) 
{
	BOOL fBadVersion = FALSE;

	// Disable browse buttons if not Win95 or NT 4.0
	OSVERSIONINFO osvi;

	memset( &osvi, 0, sizeof(OSVERSIONINFO) );
	osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &osvi );
	if( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		if( osvi.dwMajorVersion <= 3 )
		{
			fBadVersion = TRUE;
		}
	}
	
	if( fBadVersion )
	{
		m_btnBrowse.EnableWindow( FALSE );
	}
	else
	{
		m_btnBrowse.EnableWindow( fEnable );
	}

	m_editFolder.EnableWindow( fEnable );
	m_lstbxFolders.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders::UpdateFolder

void CProjectPropTabFolders::UpdateFolder( LPCTSTR szNewFolder )
{
	CJzRuntimeFolder* pJzRuntimeFolder = NULL;

	int nCurSel = m_lstbxFolders.GetCurSel();
	if( nCurSel != LB_ERR )
	{
		pJzRuntimeFolder = (CJzRuntimeFolder *)m_lstbxFolders.GetItemDataPtr( nCurSel );
	}

	if( pJzRuntimeFolder )
	{
		pJzRuntimeFolder->m_strRuntimeFolder = szNewFolder;
	}
	else
	{
		m_pProject->m_strRuntimeDir = szNewFolder;
	}

	FillFolderListBox();
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders::GetDocName

void CProjectPropTabFolders::GetDocName( CString& strName )
{
	int nCurSel = m_lstbxFolders.GetCurSel();

	if( nCurSel == LB_ERR )
	{
		strName.Empty();
	}
	else
	{
		CJzRuntimeFolder* pJzRuntimeFolder = (CJzRuntimeFolder *)m_lstbxFolders.GetItemDataPtr( nCurSel );
		
		if( pJzRuntimeFolder )
		{
			ASSERT( pJzRuntimeFolder->m_pDocTemplate != NULL );
			pJzRuntimeFolder->m_pDocTemplate->GetDocString( strName, CDocTemplate::docName );
		}
		else
		{
			strName.LoadString( IDS_OTHER_TEXT );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders::FillFolderListBox

void CProjectPropTabFolders::FillFolderListBox( void ) 
{
	// Store current selection
	int nCurSel = m_lstbxFolders.GetCurSel();
	if( nCurSel == LB_ERR )
	{
		nCurSel = 0;
	}

	// Empty the listbox
	m_lstbxFolders.ResetContent();

	// Exit if there is not a Project
	if( m_pProject == NULL )
	{
		return;
	}

	CJzRuntimeFolder* pJzRuntimeFolder;
	CString strText;
	CSize sizeText;
	CDC* pDC;
	int nCurExtent;
	int nPos;

	// Add folders to the listbox
	POSITION pos = m_pProject->m_lstRuntimeFolders.GetHeadPosition();
	while( pos )
	{
		pJzRuntimeFolder = m_pProject->m_lstRuntimeFolders.GetNext( pos );

		if( pJzRuntimeFolder->m_pDocTemplate )
		{
			// Format text
			pJzRuntimeFolder->m_pDocTemplate->GetDocString( strText, CDocTemplate::docName );
			strText += _T("       ");
			strText += pJzRuntimeFolder->m_strRuntimeFolder;

			// Add text to listbox
			nPos = m_lstbxFolders.AddString( strText );
			if( nPos >= 0 )
			{
				m_lstbxFolders.SetItemDataPtr( nPos, pJzRuntimeFolder );

				// Set horizontal extent
				nCurExtent = m_lstbxFolders.GetHorizontalExtent();

				pDC = m_lstbxFolders.GetDC();
				if( pDC )
				{
					sizeText = pDC->GetTextExtent( strText );

					if( sizeText.cx > nCurExtent )
					{
						m_lstbxFolders.SetHorizontalExtent( sizeText.cx );
					}

					m_lstbxFolders.ReleaseDC( pDC );
				}
			}
		}
	}

	// Add "Other" entry
	strText.LoadString( IDS_OTHER_TEXT );
	strText += _T("       ");
	strText += m_pProject->m_strRuntimeDir;
	nPos = m_lstbxFolders.AddString( strText );
	if( nPos >= 0 )
	{
		m_lstbxFolders.SetItemDataPtr( nPos, NULL );

		// Set horizontal extent
		nCurExtent = m_lstbxFolders.GetHorizontalExtent();

		pDC = m_lstbxFolders.GetDC();
		if( pDC )
		{
			sizeText = pDC->GetTextExtent( strText );

			if( sizeText.cx > nCurExtent )
			{
				m_lstbxFolders.SetHorizontalExtent( sizeText.cx );
			}

			m_lstbxFolders.ReleaseDC( pDC );
		}
	}

	// Set the current selection
	m_lstbxFolders.SetCurSel( nCurSel );
	OnSelChangeFolderList();
}


BEGIN_MESSAGE_MAP(CProjectPropTabFolders, CPropertyPage)
	//{{AFX_MSG_MAP(CProjectPropTabFolders)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_LBN_SELCHANGE(IDC_FOLDER_LIST, OnSelChangeFolderList)
	ON_EN_KILLFOCUS(IDC_FOLDER, OnKillFocusFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders message handlers

/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders::OnSetActive

BOOL CProjectPropTabFolders::OnSetActive() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( (m_pProject == NULL)
	||  (m_pProject->m_pProjectDoc == NULL) )
	{
		EnableControls( FALSE );
		return CPropertyPage::OnSetActive();
	}

	// Store active tab
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->GetActivePage( &CProjectPropPageManager::sm_nActiveTab );
		pIPropSheet->Release();
	}

	EnableControls( TRUE );

	m_editFolder.LimitText( _MAX_PATH );
	FillFolderListBox();

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// BrowseFolder

int CALLBACK BrowseFolder( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	switch( uMsg )
	{
		case BFFM_INITIALIZED:
			// Set initial folder
			CString strText = (TCHAR *)lpData;
			if( !strText.IsEmpty() )
			{
				if( strText.GetLength() > 3 )
				{
					strText = strText.Left( strText.GetLength() - 1 );
				}
				SendMessage( hwnd, BFFM_SETSELECTION, 1, (LPARAM)(LPCTSTR)strText );
				return( 1 );
			}
			break;
	}

	return( 0 );
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders::OnBrowse

void CProjectPropTabFolders::OnBrowse() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pProject == NULL )
	{
		return;
	}

	CString strRuntimeFolder;
	CString strName;
	LPMALLOC pMalloc;

	// Get the current runtime folder
	m_editFolder.GetWindowText( strRuntimeFolder );

	// Strip leading and trailing spaces
	strRuntimeFolder.TrimRight();
	strRuntimeFolder.TrimLeft();

	// Gets the Shell's default allocator
	if( ::SHGetMalloc(&pMalloc) == NOERROR )
	{
		// Get the title for the dialog
		CString strTitle;
		GetDocName( strName );
		AfxFormatString1( strTitle, IDS_CHOOSE_SPECIFIC_RUNTIME_DIRECTORY, strName );

		// Populate BROWSEINFO structure
		TCHAR achDirectory[MAX_PATH];
		BROWSEINFO bi;

		bi.hwndOwner = theApp.m_pMainWnd->GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = achDirectory;
		bi.lpszTitle = strTitle;
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = BrowseFolder;
		bi.lParam = (LPARAM)(LPCTSTR)strRuntimeFolder;

		// This next call issues the dialog box.
		LPITEMIDLIST pidlBrowse = SHBrowseForFolder( &bi );

		if( pidlBrowse )
		{
			if( ::SHGetPathFromIDList(pidlBrowse, achDirectory) )
			{ 
				// At this point achDirectory contains the selected path
				CString strNewFolder = achDirectory;
				if( strNewFolder.Right(1) != _T("\\") )
				{
					strNewFolder += _T("\\");
				}

				if( strRuntimeFolder.CompareNoCase( strNewFolder ) != 0 )
				{
					// Update tab with change
					UpdateFolder( strNewFolder );
				}
			}
			// Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free( pidlBrowse );
		}

		// Release the shell's allocator.
		pMalloc->Release();
	}

	m_btnBrowse.SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders::OnSelChangeFolderList

void CProjectPropTabFolders::OnSelChangeFolderList() 
{
	if( m_pProject == NULL )
	{
		return;
	}

	int nCurSel = m_lstbxFolders.GetCurSel();
	if( nCurSel == LB_ERR )
	{
		m_editFolder.SetWindowText( _T("") );
		m_editFolder.EnableWindow( FALSE );
		m_btnBrowse.EnableWindow( FALSE );
	}
	else
	{
		CString strName;
		CString strPrompt;

		m_editFolder.EnableWindow( TRUE );
		m_btnBrowse.EnableWindow( TRUE );

		GetDocName( strName );
		AfxFormatString1( strPrompt, IDS_FOLDER_PROMPT, strName );
		m_staticPrompt.SetWindowText( strPrompt );

		CJzRuntimeFolder* pJzRuntimeFolder = (CJzRuntimeFolder *)m_lstbxFolders.GetItemDataPtr( nCurSel );
		if( pJzRuntimeFolder )
		{
			m_editFolder.SetWindowText( pJzRuntimeFolder->m_strRuntimeFolder );
			m_strOrigFolderText = pJzRuntimeFolder->m_strRuntimeFolder;
		}
		else
		{
			m_editFolder.SetWindowText( m_pProject->m_strRuntimeDir );
			m_strOrigFolderText = m_pProject->m_strRuntimeDir;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders::OnKillFocusFolder

void CProjectPropTabFolders::OnKillFocusFolder() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pProject == NULL )
	{
		return;
	}

	CString strNewFolder;

	// Get the newly entered runtime directory
	m_editFolder.GetWindowText( strNewFolder );

	// Strip leading and trailing spaces
	strNewFolder.TrimRight();
	strNewFolder.TrimLeft();

	// Make sure last character is a slash
	if( strNewFolder.Right(1) != _T("\\") )
	{
		strNewFolder += _T("\\");
	}

	// Sync any changes we may have made to the text
	m_editFolder.SetWindowText( strNewFolder );

	// Make sure it contains valid characters
	int iBad = strNewFolder.FindOneOf( _T("*?\"<>;|#%") );
	if( iBad != -1 )
	{
		AfxMessageBox( IDS_ERR_RUNTIME_FOLDER_CHAR );
		m_editFolder.SetFocus();
		m_pPageManager->m_pIPropSheet->Show( TRUE );
		return;
	}

	// Make sure the new directory exists
	DWORD dwAttributes = GetFileAttributes( strNewFolder );

	if( (dwAttributes == 0xFFFFFFFF)
	|| !(dwAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		// Directory does not exist
		if( AfxMessageBox( IDS_ERR_RUNTIME_DIR_NOTFND, MB_OKCANCEL ) == IDOK )
		{
			if( theApp.CreateTheDirectory( strNewFolder ) == FALSE )
			{
				m_editFolder.SetFocus();
				m_pPageManager->m_pIPropSheet->Show( TRUE );
				return;
			}
		}
		else
		{
			m_btnBrowse.SetFocus();
		}
	}

	// Update tab with change
	UpdateFolder( strNewFolder );
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders::PreTranslateMessage

BOOL CProjectPropTabFolders::PreTranslateMessage( MSG* pMsg ) 
{
	if( m_pProject )
	{
		if( pMsg->message == WM_KEYDOWN )
		{
			if( !(pMsg->lParam & 0x40000000) )
			{
				switch( pMsg->wParam )
				{
					case VK_ESCAPE:
					{
						CWnd* pWnd = GetFocus();
						if( pWnd )
						{
							switch( pWnd->GetDlgCtrlID() )
							{
								case IDC_FOLDER: 
								{
									m_editFolder.SetWindowText( m_strOrigFolderText );
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
			}
		}
	}
	
	return CPropertyPage::PreTranslateMessage( pMsg );
}
