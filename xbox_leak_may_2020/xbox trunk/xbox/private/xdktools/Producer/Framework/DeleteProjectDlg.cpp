// DeleteProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "DeleteProjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteProjectDlg dialog


CDeleteProjectDlg::CDeleteProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteProjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteProjectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pProject = NULL;
	m_nWhichFiles = DPF_NO_FILES;
	m_fRuntimePrompt = FALSE;
}


void CDeleteProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteProjectDlg)
	DDX_Control(pDX, IDC_PROMPT_RUNTIME, m_checkRuntimePrompt);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_DESIGN_FILES, m_radioDesignFiles);
	DDX_Control(pDX, IDC_ALL_FILES, m_radioAllFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteProjectDlg, CDialog)
	//{{AFX_MSG_MAP(CDeleteProjectDlg)
	ON_BN_CLICKED(IDC_ALL_FILES, OnAllFiles)
	ON_BN_CLICKED(IDC_DESIGN_FILES, OnDesignFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteProjectDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CDeleteProjectDlg::OnInitDialog

BOOL CDeleteProjectDlg::OnInitDialog() 
{
	ASSERT( m_pProject != NULL );
	
	CDialog::OnInitDialog();
	
	CString strText;

	AfxFormatString1( strText, IDS_DELETE_ALL_FILES1, m_pProject->m_strName );
	m_radioDesignFiles.SetWindowText( strText );

	AfxFormatString1( strText, IDS_DELETE_ALL_FILES2, m_pProject->m_strName );
	m_radioAllFiles.SetWindowText( strText );

	m_radioDesignFiles.SetCheck( 1 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CDeleteProjectDlg::OnAllFiles

void CDeleteProjectDlg::OnAllFiles() 
{
	m_checkRuntimePrompt.EnableWindow( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDeleteProjectDlg::OnDesignFiles

void CDeleteProjectDlg::OnDesignFiles() 
{
	m_checkRuntimePrompt.EnableWindow( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CDeleteProjectDlg::OnOK

void CDeleteProjectDlg::OnOK() 
{
	if( m_radioAllFiles.GetCheck() == 1 )
	{
		m_nWhichFiles = DPF_ALL_FILES; 

		if( m_checkRuntimePrompt.GetCheck() == 1 )
		{
			m_fRuntimePrompt = TRUE;
		}
	}
	else if( m_radioDesignFiles.GetCheck() == 1 )
	{
		m_nWhichFiles = DPF_DESIGN_FILES; 
	}
	
	CDialog::OnOK();
}
