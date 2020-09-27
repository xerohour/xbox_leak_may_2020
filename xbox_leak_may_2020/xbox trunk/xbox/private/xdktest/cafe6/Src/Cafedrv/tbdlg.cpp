///////////////////////////////////////////////////////////////////////////////
//	tbdlg.cpp
//
//	Created by :			Date :
//		BrianCr				5/24/95
//
//	Description :
//		Implementation of the CToolbarsDlg class
//

#include "stdafx.h"
#include "tbDlg.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolbarsDlg dialog


CToolbarsDlg::CToolbarsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolbarsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolbarsDlg)
	m_bShowCAFEToolbar = FALSE;
	m_bShowStatusBar = FALSE;
	m_bShowSubSuiteToolbar = FALSE;
	m_bShowViewportToolbar = FALSE;
	//}}AFX_DATA_INIT
}


void CToolbarsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolbarsDlg)
	DDX_Check(pDX, IDC_ShowCAFEToolbar, m_bShowCAFEToolbar);
	DDX_Check(pDX, IDC_ShowStatusBar, m_bShowStatusBar);
	DDX_Check(pDX, IDC_ShowSubSuiteToolbar, m_bShowSubSuiteToolbar);
	DDX_Check(pDX, IDC_ShowViewportToolbar, m_bShowViewportToolbar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolbarsDlg, CDialog)
	//{{AFX_MSG_MAP(CToolbarsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolbarsDlg message handlers

BOOL CToolbarsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
