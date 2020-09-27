// SPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ODBCMT.h"
#include "SPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewSPDlg dialog


CNewSPDlg::CNewSPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewSPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewSPDlg)
	m_NoThreads = 1;
	m_Delay = 0;
	m_SPName = _T("");
	//}}AFX_DATA_INIT

}


void CNewSPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewSPDlg)
	DDX_Control(pDX, IDC_SPNAME, m_SPEdit);
	DDX_Text(pDX, IDC_NOTHREADS, m_NoThreads);
	DDV_MinMaxUInt(pDX, m_NoThreads, 1, 50);
	DDX_Text(pDX, IDC_DELAY, m_Delay);
	DDV_MinMaxUInt(pDX, m_Delay, 0, 10000);
	DDX_Text(pDX, IDC_SPNAME, m_SPName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewSPDlg, CDialog)
	//{{AFX_MSG_MAP(CNewSPDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewSPDlg message handlers

BOOL CNewSPDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_SPEdit.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
}
