// TargetDirectoryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xboxaddin.h"
#include "TargetDirectoryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTargetDirectoryDlg dialog


CTargetDirectoryDlg::CTargetDirectoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTargetDirectoryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTargetDirectoryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTargetDirectoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTargetDirectoryDlg)
	DDX_Control(pDX, IDC_SYNCHRONIZE, m_btnSynchronize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTargetDirectoryDlg, CDialog)
	//{{AFX_MSG_MAP(CTargetDirectoryDlg)
	ON_BN_CLICKED(IDC_SYNCHRONIZE, OnSynchronize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTargetDirectoryDlg message handlers

BOOL CTargetDirectoryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int nBackSlashIndex = theApp.m_strXboxDestination.Find('\\');

	// Shouldn't happen
	ASSERT( nBackSlashIndex != -1 );

	// Strip off everything before the first \, including the backslash
	CString strDestination = theApp.m_strXboxDestination.Right( theApp.m_strXboxDestination.GetLength() - nBackSlashIndex - 1 );

	SetDlgItemText( IDC_EDIT_DIRECTORY, strDestination );

	m_btnSynchronize.EnableWindow( theApp.m_fConnected );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTargetDirectoryDlg::OnOK() 
{
	CString strDestination;
	GetDlgItemText( IDC_EDIT_DIRECTORY, strDestination );

	// Remove trailing backslashes
	while( !strDestination.IsEmpty()
	&&	_T('\\') == strDestination.GetAt( strDestination.GetLength() - 1 ) )
	{
		strDestination = strDestination.Left(strDestination.GetLength() - 1);
	}

	// Remove leading backslashes
	while( !strDestination.IsEmpty()
	&&	_T('\\') == strDestination.GetAt( 0 ) )
	{
		strDestination = strDestination.Right(strDestination.GetLength() - 1);
	}

	// Add T:\ to the front of the directory name
	strDestination = _T("T:\\") + strDestination;
	
	if( strDestination.IsEmpty() )
	{
		// Can't have an empty directory
		CString strMessage;
		strMessage.LoadString( IDS_ERR_PATH_EMPTY );
		XboxAddinMessageBox( this, strMessage, MB_ICONERROR | MB_OK );
		return;
	}

	bool fContinue = true;

	if( theApp.m_fConnected )
	{
		DM_FILE_ATTRIBUTES dmFileAttributes;
		HRESULT hr = DmGetFileAttributes( strDestination, &dmFileAttributes );
		if( FAILED(hr) )
		{
			// Directory does not exist
			CString strMessage;
			strMessage.FormatMessage( IDS_ERR_PATH_NOEXIST, strDestination );
			if( IDOK == XboxAddinMessageBox( this, strMessage, MB_ICONERROR | MB_OKCANCEL ) )
			{
				if( FAILED( DmMkdir( strDestination ) ) )
				{
					strMessage.FormatMessage( IDS_ERR_PATH_CANTCREATE, strDestination );
					XboxAddinMessageBox( this, strMessage, MB_ICONERROR | MB_OK );
					fContinue = false;
				}
			}
			else
			{
				fContinue = false;
			}
		}
		else if( !(dmFileAttributes.Attributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			// File with same name exists
			CString strMessage;
			strMessage.LoadString( IDS_ERR_PATH_FILEEXIST );
			XboxAddinMessageBox( this, strMessage, MB_ICONERROR | MB_OK );
			fContinue = false;
		}
	}

	if( fContinue )
	{
		theApp.m_strXboxDestination = strDestination;
		SetUserRegString( _T("XboxPath"), theApp.m_strXboxDestination );
		theApp.UpdateTargetDirectory();
		CDialog::OnOK();
	}
}

void CTargetDirectoryDlg::OnSynchronize() 
{
	OnOK();
}
