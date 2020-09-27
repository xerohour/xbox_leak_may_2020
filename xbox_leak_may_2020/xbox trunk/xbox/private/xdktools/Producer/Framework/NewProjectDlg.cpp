// NewProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "NewProjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg dialog


CNewProjectDlg::CNewProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewProjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewProjectDlg)
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg::IsValidProjectName

BOOL CNewProjectDlg::IsValidProjectName( void ) 
{
	CString strProjectName;

	m_editProjectName.GetWindowText( strProjectName );

	// Strip leading and trailing spaces
	strProjectName.TrimRight();
	strProjectName.TrimLeft();

	int iBad = strProjectName.FindOneOf( _T("\\/:*?\"<>;|#%.") );

	if( (strProjectName.IsEmpty())
	||  (iBad != -1) )
	{
		AfxMessageBox( IDS_ERR_PROJECT_NAME );
		m_editProjectName.SetFocus();
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg::IsValidProjectDir

BOOL CNewProjectDlg::IsValidProjectDir( void ) 
{
	CString strProjectName;
	CString strProjectDir;

	m_editProjectName.GetWindowText( strProjectName );

	// Strip leading and trailing spaces
	strProjectName.TrimRight();
	strProjectName.TrimLeft();

	GetProjectDir( strProjectDir );

	// String cannot be empty
	if( strProjectDir.IsEmpty() )
	{
		AfxMessageBox( IDS_ERR_PROJECT_DIR );
		m_editProjectDir.SetFocus();
		return FALSE;
	}

	// Path cannot contain any of these characters
	TCHAR achDir[_MAX_DIR];

	_tsplitpath( strProjectDir, NULL, achDir, NULL, NULL );
	CString strDir = achDir;

	int iBad = strDir.FindOneOf( _T(":*?\"<>;|#%") );
	if( iBad != -1 )
	{
		AfxMessageBox( IDS_ERR_PROJECTDIR_CHAR );
		m_editProjectDir.SetFocus();
		return FALSE;
	}

	// Get Project file extension
	CString strFilterExt;

	CJazzDocTemplate* pTemplate = theApp.FindProjectDocTemplate();
	ASSERT( pTemplate != NULL );
	pTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt );

	// Make sure path/name is not too long
	if( strProjectDir.Right(1) != _T("\\") )
	{
		strProjectDir += _T("\\");
	}
	strProjectDir = strProjectDir + strProjectName + strFilterExt;

	if( strProjectDir.GetLength() >= _MAX_DIR )
	{
		AfxMessageBox( IDS_ERR_FILENAME_LENGTH );
		return FALSE;
	}

	// Path cannot already contain a .pro file
	WIN32_FIND_DATA	fd;

	int nFindPos = strProjectDir.ReverseFind( (TCHAR)'\\' );
	if( nFindPos != -1 )
	{
		strProjectDir = strProjectDir.Left( nFindPos + 1 );
	}
	strProjectDir = strProjectDir + _T("*") + strFilterExt;
	
	HANDLE hFind = FindFirstFile( strProjectDir, &fd );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		AfxMessageBox( IDS_ERR_PROJECT_EXISTS );
		m_editProjectName.SetFocus();
		FindClose( hFind );
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg::GetProjectName

void CNewProjectDlg::GetProjectName( CString& strName )
{
	m_editProjectName.GetWindowText( strName );

	// Strip leading and trailing spaces
	strName.TrimRight();
	strName.TrimLeft();
}


/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg::GetProjectDir

void CNewProjectDlg::GetProjectDir( CString& strProjectDir )
{
	CString strName;
	CString strTempName;
	CString strTemp;

	m_editProjectName.GetWindowText( strName );

	// Strip leading and trailing spaces
	strName.TrimRight();
	strName.TrimLeft();

	m_editProjectDir.GetWindowText( strProjectDir );

	// Strip leading and trailing spaces
	strProjectDir.TrimRight();
	strProjectDir.TrimLeft();

	strTempName = _T("\\") + strName;
	strTemp = strProjectDir.Right( strTempName.GetLength() );

	if( strTemp != strTempName )
	{
		if( strProjectDir.Right(1) != _T("\\") )
		{
			strProjectDir += _T("\\");
		}
		strProjectDir += strName;
	}
}


void CNewProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewProjectDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_PROJECT_DIR, m_editProjectDir);
	DDX_Control(pDX, IDC_PROJECT_NAME, m_editProjectName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewProjectDlg, CDialog)
	//{{AFX_MSG_MAP(CNewProjectDlg)
	ON_BN_CLICKED(IDC_PROJECT_BROWSE, OnProjectDirBrowse)
	ON_EN_CHANGE(IDC_PROJECT_NAME, OnChangeProjectName)
	ON_EN_CHANGE(IDC_PROJECT_DIR, OnChangeProjectDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg::OnInitDialog

BOOL CNewProjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set the window title
	SetWindowText( m_strTitle );
	
	// Create/get "DMUSProducer" default Project directory
	theApp.GetDefaultProjectDir( m_strProjectDir );
	::SetCurrentDirectory( m_strProjectDir );

	CString strProjectName = theApp.m_strNewFileName;
	if( strProjectName.IsEmpty() )
	{
		CString strProjectDir;
		CString strProject;
		CString strNbr;

		// Load "Project" text
		strProject.LoadString( IDS_PROJECT_TEXT );
		int i = 0;

		// Get Project file extension
		CString strFilterExt;
		CJazzDocTemplate* pTemplate = theApp.FindProjectDocTemplate();
		ASSERT( pTemplate != NULL );
		pTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt );

		for( ; ; )
		{
			// Something is wrong if we have already tried 250 times
			if( ++i > 250 )
			{
				strProjectName.Empty();
				break;
			}

			strNbr.Format( "%d", i ); 
			strProjectName = strProject + strNbr;

			// Path cannot already contain a .pro file
			WIN32_FIND_DATA	fd;
			strProjectDir =  m_strProjectDir + strProjectName + _T("\\") + _T("*") + strFilterExt;
			HANDLE hFind = FindFirstFile( strProjectDir, &fd );
			if( hFind == INVALID_HANDLE_VALUE )
			{
				break;
			}
			FindClose( hFind );
		}
	}

	m_editProjectName.LimitText( MAX_LENGTH_PROJECT_NAME - 4 );
	m_editProjectName.SetWindowText( strProjectName );

	m_editProjectDir.LimitText( MAX_LENGTH_PATH_NAME );
	m_editProjectDir.SetWindowText( m_strProjectDir + strProjectName );

	// Disable browse buttons if not Win95 or NT 4.0
	OSVERSIONINFO osvi;

	memset( &osvi, 0, sizeof(OSVERSIONINFO) );
	osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &osvi );
	if( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		if( osvi.dwMajorVersion <= 3 )
		{
			m_editProjectDir.EnableWindow( FALSE );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg::OnProjectDirBrowse

void CNewProjectDlg::OnProjectDirBrowse() 
{
	LPMALLOC pMalloc;

	// Gets the Shell's default allocator
	if( ::SHGetMalloc(&pMalloc) == NOERROR )
	{
		// Get the title for the dialog
		TCHAR achTitle[MID_BUFFER];
		LoadString( theApp.m_hInstance, IDS_CHOOSE_PROJECT_LOCATION, achTitle, sizeof(achTitle));

		// Populate BROWSEINFO structure
		TCHAR achDirectory[MAX_PATH];
		BROWSEINFO bi;

		bi.hwndOwner = theApp.m_pMainWnd->GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = achDirectory;
		bi.lpszTitle = achTitle;
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = BrowseFolder;
		bi.lParam = (LPARAM)(LPCTSTR)m_strProjectDir;

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
				strNewFolder.MakeLower();

				if( m_strProjectDir != strNewFolder )
				{
					// Set new Project directory
					m_strProjectDir = strNewFolder;
					OnChangeProjectName();
					m_editProjectName.SetFocus();
				}
			}
			// Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free( pidlBrowse );
		}

		// Release the shell's allocator.
		pMalloc->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg::OnChangeProjectName

void CNewProjectDlg::OnChangeProjectName() 
{
	CString strProjectName;

	m_editProjectName.GetWindowText( strProjectName );

	// Strip leading and trailing spaces
	strProjectName.TrimRight();
	strProjectName.TrimLeft();

	if( m_strProjectDir.Right(1) != _T("\\") )
	{
		m_strProjectDir += _T("\\");
	}
	m_editProjectDir.SetWindowText( m_strProjectDir + strProjectName );

	m_btnOK.EnableWindow( strProjectName.IsEmpty() == FALSE ); 
}


/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg::OnChangeProjectDir

void CNewProjectDlg::OnChangeProjectDir() 
{
	CString strProjectName;
	CString strProjectDir;

	m_editProjectName.GetWindowText( strProjectName );

	// Strip leading and trailing spaces
	strProjectName.TrimRight();
	strProjectName.TrimLeft();

	m_editProjectDir.GetWindowText( strProjectDir );

	// Strip leading and trailing spaces
	strProjectDir.TrimRight();
	strProjectDir.TrimLeft();

	CString strTempName = _T("\\") + strProjectName;
	CString strTemp = strProjectDir.Right( strTempName.GetLength() );

	if( strTemp == strTempName )
	{
		m_strProjectDir = strProjectDir.Left( strProjectDir.GetLength() - strTempName.GetLength() + 1 );
	}
	else
	{
		m_strProjectDir = strProjectDir;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg::OnOK

void CNewProjectDlg::OnOK() 
{
	// Validate Project name
	if( IsValidProjectName() == FALSE )
	{
		return;
	}

	// Validate Project directory 
	if( IsValidProjectDir() == FALSE )
	{
		return;
	}
	
	// Everything is OK
	GetProjectName( m_strName );
	GetProjectDir( m_strProjectDir );

	CDialog::OnOK();
}
