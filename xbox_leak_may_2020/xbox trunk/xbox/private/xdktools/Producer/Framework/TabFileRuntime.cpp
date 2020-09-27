// TabFileRuntime.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "TabFileRuntime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime property page

CTabFileRuntime::CTabFileRuntime( CFilePropPageManager* pPageManager ) : CPropertyPage(CTabFileRuntime::IDD)
{
	//{{AFX_DATA_INIT(CTabFileRuntime)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );

	m_pFileNode = NULL;
	m_pPageManager = pPageManager;
}

CTabFileRuntime::~CTabFileRuntime()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime::SetFile

void CTabFileRuntime::SetFile( CFileNode* pFileNode )
{
	m_pFileNode = pFileNode;
}


void CTabFileRuntime::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabFileRuntime)
	DDX_Control(pDX, IDC_RUNTIME_FILE, m_editRuntimeFile);
	DDX_Control(pDX, IDC_RUNTIME_FILENAME, m_editRuntimeFileName);
	DDX_Control(pDX, IDC_RUNTIME_CREATE, m_staticRuntimeCreate);
	DDX_Control(pDX, IDC_RUNTIME_DIR, m_editRuntimeDir);
	DDX_Control(pDX, IDC_RUNTIME_BROWSE, m_btnRuntimeBrowse);
	DDX_Control(pDX, IDC_RUNTIME_SIZE, m_staticRuntimeSize);
	DDX_Control(pDX, IDC_RUNTIME_MODIFIED, m_staticRuntimeModified);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime::EnableControls

void CTabFileRuntime::EnableControls( BOOL fEnable ) 
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
		m_btnRuntimeBrowse.EnableWindow( FALSE );
	}
	else
	{
		m_btnRuntimeBrowse.EnableWindow( fEnable );
	}

	m_editRuntimeDir.EnableWindow( fEnable );
	m_editRuntimeFileName.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime::GetDefaultFolderText

void CTabFileRuntime::GetDefaultFolderText( CString& strDefaultText )
{
	CString strFilterName;

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( theApp.m_hInstance );

	CString strExt;
	int nFindPos = m_pFileNode->m_strName.ReverseFind( (TCHAR)'.' );
	if( nFindPos != -1 )
	{
		strExt = m_pFileNode->m_strName.Right( m_pFileNode->m_strName.GetLength() - nFindPos );
	}

	IDMUSProdDocType* pIDocType;
	BSTR bstrExt = strExt.AllocSysString();

	if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
	{
		CJazzDocTemplate* pTemplate = theApp.FindDocTemplate( pIDocType );
		if( pTemplate )
		{
			pTemplate->GetDocString( strFilterName, CDocTemplate::docName );
		}
		pIDocType->Release();
	}

	if( strFilterName.IsEmpty() )
	{
		strFilterName.LoadString( IDS_OTHER_TEXT );
	}

	AfxFormatString1( strDefaultText, IDS_DEFAULT_RUNTIME_FOLDER, strFilterName );
	
	AfxSetResourceHandle( hInstance );
}


BEGIN_MESSAGE_MAP(CTabFileRuntime, CPropertyPage)
	//{{AFX_MSG_MAP(CTabFileRuntime)
	ON_EN_KILLFOCUS(IDC_RUNTIME_FILENAME, OnKillFocusRuntimeFileName)
	ON_EN_KILLFOCUS(IDC_RUNTIME_DIR, OnKillFocusRuntimeDir)
	ON_BN_CLICKED(IDC_RUNTIME_BROWSE, OnRuntimeBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime::OnSetActive

BOOL CTabFileRuntime::OnSetActive() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pFileNode == NULL )
	{
		EnableControls( FALSE );
		return CPropertyPage::OnSetActive();
	}

	// Store active tab
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->GetActivePage( &CFilePropPageManager::sm_nActiveTab );
		pIPropSheet->Release();
	}

	EnableControls( TRUE );

	m_editRuntimeFileName.LimitText( MAX_LENGTH_FILE_NAME );

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( theApp.m_hInstance );

	// Set runtime file name, modified date, size
	CString strFile;
	CString strFileCreate;
	CString strFileModified;
	CString strFileSize;

	strFile = m_pFileNode->m_strRuntimeFile;
	strFileCreate.Empty();
	strFileModified.Empty();
	strFileSize.Empty();

	if( strFile.IsEmpty() )
	{
		strFile.LoadString( IDS_NOT_SAVED );
	}
	else
	{
		HANDLE hFile = ::CreateFile( strFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 								 FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile != INVALID_HANDLE_VALUE )
		{
			FILETIME ftCreate;
			FILETIME ftModified;
			DWORD dwFileSize;

			if( ::GetFileTime(hFile, &ftCreate, NULL, &ftModified) )
			{
				CTime timeFile;
				
				timeFile = ftCreate;
				strFileCreate = timeFile.Format( "%A, %B %d, %Y   %I:%M:%S %p" );
				
				timeFile = ftModified;
				strFileModified = timeFile.Format( "%A, %B %d, %Y   %I:%M:%S %p" );
			}

			dwFileSize = GetFileSize( hFile, NULL );

			CString strSize;
			CString strBytes;
			strSize.Format( "%u", dwFileSize );

			int i, j;
			int nLength = strSize.GetLength();

			for( i=0, j=nLength;  i < nLength ; i++ )
			{
				strBytes += strSize[i];
				j--;
				if( (j > 0)
				&& !(j % 3) )
				{
					strBytes += ',';
				}
			}
			
			if( dwFileSize < 1024 )
			{
				AfxFormatString1( strFileSize, IDS_SIZE_BYTES, strBytes );
			}
			else
			{
				CString strKB;
				
				double dblKB = dwFileSize / 1024.0;
				strKB.Format( "%.2f", dblKB );

				AfxFormatString2( strFileSize, IDS_SIZE_KB, strKB, strBytes );
			}

			CloseHandle( hFile );
		}
		else
		{
			strFileCreate.LoadString( IDS_UNKNOWN );
			strFileModified.LoadString( IDS_UNKNOWN );
			strFileSize.LoadString( IDS_UNKNOWN );
		}
	}
	m_editRuntimeFile.SetWindowText( strFile );
	m_staticRuntimeCreate.SetWindowText( strFileCreate );
	m_staticRuntimeModified.SetWindowText( strFileModified );
	m_staticRuntimeSize.SetWindowText( strFileSize );

	// Set runtime folder
	CString strText;

	strText = m_pFileNode->m_strRuntimeFolder;
	if( strText.IsEmpty() ) 
	{
		GetDefaultFolderText( strText );
	}
	m_editRuntimeDir.SetWindowText( strText );

	// Set runtime filename
	m_editRuntimeFileName.SetWindowText( m_pFileNode->m_strRuntimeFileName);
	
	AfxSetResourceHandle( hInstance );
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime::OnKillFocusRuntimeFileName

void CTabFileRuntime::OnKillFocusRuntimeFileName() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pFileNode == NULL )
	{
		return;
	}

	CString strFileName;
	CString strNewExt;
	CString strRuntimeExt;

	// Determine the proper runtime extension
	theApp.AdjustFileName( FT_RUNTIME, m_pFileNode->m_strName, strFileName );
	int nFindPos = strFileName.ReverseFind( (TCHAR)'.' );
	if( nFindPos != -1 )
	{
		strRuntimeExt = strFileName.Right( strFileName.GetLength() - nFindPos );
	}
	ASSERT( !strRuntimeExt.IsEmpty() );

	// Get the newly entered runtime filename
	m_editRuntimeFileName.GetWindowText( strFileName );

	// Strip leading and trailing spaces
	strFileName.TrimRight();
	strFileName.TrimLeft();

	// Sync any changes we may have made to the text
	m_editRuntimeFileName.SetWindowText( strFileName );

	// No need to edit if nothing was changed
	if( strFileName.CompareNoCase( m_pFileNode->m_strRuntimeFileName ) == 0 )
	{
		return;
	}

	// Make sure it contains valid characters
	int iBad = strFileName.FindOneOf( _T("\\/:*?\"<>;|%") );
	if( iBad != -1 )
	{
		AfxMessageBox( IDS_ERR_FILE_CHAR );
		m_editRuntimeFileName.SetFocus();
		m_pPageManager->m_pIPropSheet->Show( TRUE );
		return;
	}

	// If necessary, add extension to new name
	strNewExt = strFileName.Right( strRuntimeExt.GetLength() );
	if( strNewExt != strRuntimeExt )
	{
		strFileName += strRuntimeExt;
		m_editRuntimeFileName.SetWindowText( strFileName );
	}

	// Make sure a default runtime file with the same name does not already exist
	IDMUSProdProject* pIProject;
	if( SUCCEEDED ( theApp.m_pFramework->FindProject( m_pFileNode, &pIProject ) ) )
	{
		CProject* pProject = (CProject *)pIProject;

		CString strPathName;

		m_pFileNode->ConstructRuntimePath( strPathName );
		strPathName += strFileName;

		CFileNode* pFileNode = pProject->GetFileByRuntimeFileName( strPathName, m_pFileNode, TRUE );
		if( pFileNode )
		{
			CString strRelativePath;
			CString strMsg;

			pFileNode->ConstructRelativePath( strRelativePath );
			if( !strRelativePath.IsEmpty() )
			{
				if( strRelativePath.Right(1) != _T("\\") )
				{
					strRelativePath += _T("\\");
				}
			}
			strRelativePath += pFileNode->m_strName;
			pFileNode->Release();
			pFileNode = NULL;

			AfxFormatString1( strMsg, IDS_ERR_RUNTIME_FILE_DUPE, strRelativePath );
			if( AfxMessageBox( strMsg, MB_OKCANCEL ) == IDCANCEL )
			{
				m_editRuntimeFileName.SetFocus();
				m_pPageManager->m_pIPropSheet->Show( TRUE );

				pIProject->Release();
				return;
			}
		}

		pIProject->Release();
	}

	// Use the new filename
	m_pFileNode->m_strRuntimeFileName = strFileName;
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime::OnKillFocusRuntimeDir

void CTabFileRuntime::OnKillFocusRuntimeDir() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pFileNode == NULL )
	{
		return;
	}

	ASSERT( m_pFileNode->m_pProject != NULL );

	CString strNewFolder;
	CString strDefaultText;
	CString strDefaultRuntimeFolder;
	CString strExt;

	// Get the newly entered runtime directory
	m_editRuntimeDir.GetWindowText( strNewFolder );

	// Strip leading and trailing spaces
	strNewFolder.TrimRight();
	strNewFolder.TrimLeft();

	// Sync any changes we may have made to the text
	m_editRuntimeDir.SetWindowText( strNewFolder );

	// See if we are using the default folder for this file extension
	GetDefaultFolderText( strDefaultText );
	if( strNewFolder.IsEmpty()
	||  strNewFolder.CompareNoCase( strDefaultText ) == 0 )
	{
		m_editRuntimeDir.SetWindowText( strDefaultText );
		m_pFileNode->m_strRuntimeFolder.Empty();
		return;
	}

	// Make sure last character is a slash
	if( strNewFolder.Right(1) != _T("\\") )
	{
		strNewFolder += _T("\\");
	}

	// Sync any changes we may have made to the text
	m_editRuntimeDir.SetWindowText( strNewFolder );

	// Get default runtime folder for this file extension
	int nFindPos = m_pFileNode->m_strName.ReverseFind( (TCHAR)'.' );
	if( nFindPos != -1 )
	{
		strExt = m_pFileNode->m_strName.Right( m_pFileNode->m_strName.GetLength() - nFindPos );
	}
	ASSERT( !strExt.IsEmpty() );
	m_pFileNode->m_pProject->GetDefaultRuntimeFolderByExt( strExt, strDefaultRuntimeFolder );

	// See if we are using the default folder for this file extension
	if( strNewFolder.CompareNoCase( strDefaultRuntimeFolder ) == 0 )
	{
		m_editRuntimeDir.SetWindowText( strDefaultText );
		m_pFileNode->m_strRuntimeFolder.Empty();
		return;
	}

	// Make sure it contains valid characters
	int iBad = strNewFolder.FindOneOf( _T("*?\"<>;|#%") );
	if( iBad != -1 )
	{
		AfxMessageBox( IDS_ERR_RUNTIME_FOLDER_CHAR );
		m_editRuntimeDir.SetFocus();
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
				m_editRuntimeDir.SetFocus();
				m_pPageManager->m_pIPropSheet->Show( TRUE );
				return;
			}
		}
		else
		{
			m_btnRuntimeBrowse.SetFocus();
		}
	}

	// Use the new directory (whether or not it exists)
	m_pFileNode->m_strRuntimeFolder = strNewFolder;
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime::OnRuntimeBrowse

void CTabFileRuntime::OnRuntimeBrowse() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pFileNode == NULL )
	{
		return;
	}

	ASSERT( m_pFileNode->m_pProject != NULL );

	CString strRuntimeFolder;
	LPMALLOC pMalloc;

	// Get the current runtime folder
	m_pFileNode->ConstructRuntimePath( strRuntimeFolder );

	// Gets the Shell's default allocator
	if( ::SHGetMalloc(&pMalloc) == NOERROR )
	{
		// Get the title for the dialog
		TCHAR achTitle[MID_BUFFER];
		LoadString( theApp.m_hInstance, IDS_CHOOSE_RUNTIME_DIRECTORY, achTitle, sizeof(achTitle));

		// Populate BROWSEINFO structure
		TCHAR achDirectory[MAX_PATH];
		BROWSEINFO bi;

		bi.hwndOwner = theApp.m_pMainWnd->GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = achDirectory;
		bi.lpszTitle = achTitle;
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
					CString strExt;
					CString strDefaultRuntimeFolder;

					int nFindPos = m_pFileNode->m_strName.ReverseFind( (TCHAR)'.' );
					if( nFindPos != -1 )
					{
						strExt = m_pFileNode->m_strName.Right( m_pFileNode->m_strName.GetLength() - nFindPos );
					}
					ASSERT( !strExt.IsEmpty() );
					m_pFileNode->m_pProject->GetDefaultRuntimeFolderByExt( strExt, strDefaultRuntimeFolder );

					// See if we are using the default folder for this file extension
					if( strNewFolder.CompareNoCase( strDefaultRuntimeFolder ) == 0 )
					{
						CString strDefaultText;

						GetDefaultFolderText( strDefaultText );

						// Set new Runtime directory
						m_pFileNode->m_strRuntimeFolder.Empty();
						m_editRuntimeDir.SetWindowText( strDefaultText );
					}
					else
					{
						// Set new Runtime directory
						m_pFileNode->m_strRuntimeFolder = strNewFolder;
						m_editRuntimeDir.SetWindowText( strNewFolder );
					}
				}
			}
			// Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free( pidlBrowse );
		}

		// Release the shell's allocator.
		pMalloc->Release();
	}

	m_btnRuntimeBrowse.SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime::PreTranslateMessage

BOOL CTabFileRuntime::PreTranslateMessage( MSG* pMsg ) 
{
	if( m_pFileNode )
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
								case IDC_RUNTIME_DIR: 
								{
									CString strText;

									strText = m_pFileNode->m_strRuntimeFolder;
									if( strText.IsEmpty() ) 
									{
										GetDefaultFolderText( strText );
									}
									m_editRuntimeDir.SetWindowText( strText );
									break;
								}

								case IDC_RUNTIME_FILENAME: 
								{
									m_editRuntimeFileName.SetWindowText( m_pFileNode->m_strRuntimeFileName );
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
