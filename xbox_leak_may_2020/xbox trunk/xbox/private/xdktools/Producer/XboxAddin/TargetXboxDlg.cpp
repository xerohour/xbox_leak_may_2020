// TargetXboxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xboxaddin.h"
#include "TargetXboxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTargetXboxDlg dialog


CTargetXboxDlg::CTargetXboxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTargetXboxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTargetXboxDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTargetXboxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTargetXboxDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTargetXboxDlg, CDialog)
	//{{AFX_MSG_MAP(CTargetXboxDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTargetXboxDlg message handlers

void CTargetXboxDlg::OnOK() 
{
	CString strXboxName;
	GetDlgItemText( IDC_EDIT_XBOX_NAME, strXboxName );

	if( strXboxName.IsEmpty() )
	{
		// Can't have an empty name
		CString strMessage;
		strMessage.LoadString( IDS_ERR_NAME_EMPTY );
		XboxAddinMessageBox( this, strMessage, MB_ICONERROR | MB_OK );
		return;
	}

	theApp.m_strXboxName = strXboxName;
	SetUserRegString( _T("XboxName"), strXboxName );

    DWORD dwDefaultChecked = IsDlgButtonChecked( IDC_CHECK_DEFAULT );
    SetUserRegDWORD( _T("XboxNameSetDefault"), &dwDefaultChecked );

    if( dwDefaultChecked )
    {
        DmSetXboxName( strXboxName );
    }

	CDialog::OnOK();
}

BOOL CTargetXboxDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemText( IDC_EDIT_XBOX_NAME, theApp.m_strXboxName );

    DWORD dwDefaultChecked = TRUE;
    GetUserRegDWORD( _T("XboxNameSetDefault"), &dwDefaultChecked );
    CheckDlgButton( IDC_CHECK_DEFAULT, dwDefaultChecked );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
