// WhichProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "WhichProjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWhichProjectDlg dialog


CWhichProjectDlg::CWhichProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWhichProjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWhichProjectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWhichProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWhichProjectDlg)
	DDX_Control(pDX, IDC_WHICH_PROJECT_PROMPT, m_staticPrompt);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_USE_PROJECT, m_radioUseProject);
	DDX_Control(pDX, IDC_NEW_PROJECT, m_radioNewProject);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWhichProjectDlg, CDialog)
	//{{AFX_MSG_MAP(CWhichProjectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWhichProjectDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CWhichProjectDlg::OnOK

void CWhichProjectDlg::OnOK() 
{
	if(	m_radioUseProject.GetCheck() == 1 )
	{
		EndDialog( IDC_USE_PROJECT );
		return;
	}

	EndDialog( IDC_NEW_PROJECT );
}


/////////////////////////////////////////////////////////////////////////////
// CWhichProjectDlg::OnInitDialog

BOOL CWhichProjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_radioUseProject.SetCheck( 1 );
	m_radioNewProject.SetCheck( 0 );

	// Remove path from filename
	CString strFileName = m_strFileName;
	int nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
	if( nFindPos != -1 )
	{
		strFileName = strFileName.Right( strFileName.GetLength() - nFindPos - 1 );
	}
	
	CString strPrompt;
	AfxFormatString1( strPrompt, IDS_WHICH_PROJECT_PROMPT, strFileName );
	m_staticPrompt.SetWindowText( strPrompt );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
