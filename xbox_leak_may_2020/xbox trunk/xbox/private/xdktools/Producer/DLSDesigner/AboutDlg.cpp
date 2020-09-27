// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "dlsdesignerdll.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog


CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Get version information
	TCHAR achJazzExeName[FILENAME_MAX + 1];
	TCHAR achFileVersion[MID_BUFFER];

	if( GetModuleFileName ( theApp.m_hInstance, achJazzExeName, FILENAME_MAX ) )
	{
		if( GetFileVersion( achJazzExeName, achFileVersion, MID_BUFFER ) )
		{
			CString strFileVersion;

			AfxFormatString1( strFileVersion, IDS_DLS_EDITOR_VERSION_TEXT, achFileVersion );
			SetDlgItemText( IDC_FILE_VERSION, strFileVersion );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ==============================================================
// ==============================================================
BOOL CAboutDlg::GetFileVersion( LPTSTR szExeName, LPTSTR szFileVersion, short nFileVersionSize )
{
	DWORD dwBufferSize;
	DWORD dwReserved;
	
	dwBufferSize = GetFileVersionInfoSize( szExeName, &dwReserved );
	if( dwBufferSize > 0 )
	{
		void* pBuffer;

		pBuffer = (void *)malloc( dwBufferSize );
		if( pBuffer )
		{
			CString strTheFileVersion;
			VS_FIXEDFILEINFO* pFixedInfo;
			UINT nInfoSize;

			GetFileVersionInfo( szExeName, dwReserved, dwBufferSize, pBuffer );
			VerQueryValue( pBuffer, _T("\\"), (void **)&pFixedInfo, &nInfoSize );

			WORD wVer1 = HIWORD( pFixedInfo->dwFileVersionMS );
			WORD wVer2 = LOWORD( pFixedInfo->dwFileVersionMS );
			WORD wVer3 = HIWORD( pFixedInfo->dwFileVersionLS );
			WORD wVer4 = LOWORD( pFixedInfo->dwFileVersionLS );

			strTheFileVersion.Format( _T("%u.%u.%u.%u"), wVer1, wVer2, wVer3, wVer4 );

			_tcsncpy( szFileVersion, strTheFileVersion, nFileVersionSize );

			free( pBuffer );
			return TRUE;
		}
	}

	return FALSE;
}
