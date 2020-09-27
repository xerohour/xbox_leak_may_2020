// EditSPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EditDlg.h"
#include "ODBCMT.h"
#include "ODBCThrd.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditSPDlg dialog


CEditSPDlg::CEditSPDlg(CODBCMTDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CEditSPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditSPDlg)
	m_Delay = 0;
	m_NoThreads = 0;
	m_SPName = _T("");
	//}}AFX_DATA_INIT
	m_pDoc=pDoc;
}


void CEditSPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditSPDlg)
	DDX_Control(pDX, IDC_DSN, m_DSNCombo);
	DDX_Control(pDX, IDC_SPNAME, m_SPEdit);
	DDX_Text(pDX, IDC_DELAY, m_Delay);
	DDV_MinMaxUInt(pDX, m_Delay, 0, 10000);
	DDX_Text(pDX, IDC_NOTHREADS, m_NoThreads);
	DDV_MinMaxUInt(pDX, m_NoThreads, 1, 50);
	DDX_Text(pDX, IDC_SPNAME, m_SPName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditSPDlg, CDialog)
	//{{AFX_MSG_MAP(CEditSPDlg)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_CBN_SELCHANGE(IDC_DSN, OnSelchangeDsn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditSPDlg message handlers

void CEditSPDlg::OnApply() 
{
	CComboBox *pComboBox = (CComboBox*)GetDlgItem(IDC_DSN);
	ASSERT(pComboBox!=NULL);
	int icol=pComboBox->GetCurSel();
	if(icol==CB_ERR)
		return;
	UpdateData(TRUE);
	CODBCThread *pTempThreadPTR = (CODBCThread*)m_pDoc->m_ThreadArray.GetAt(icol);
	pTempThreadPTR->m_pSPName=m_SPName;
	pTempThreadPTR->m_msecs=m_Delay;
	pTempThreadPTR->m_Threads=m_NoThreads;
}

BOOL CEditSPDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//fill the combo with the proper value from database
	CODBCThread *pTempThreadPTR;
	CComboBox *pComboBox = (CComboBox*)GetDlgItem(IDC_DSN);
	ASSERT(pComboBox!=NULL);
	pComboBox->ResetContent( ); // clear old columns
	for(long icol=0; icol<m_pDoc->m_iCountConect; icol++)
	{
		pTempThreadPTR = (CODBCThread*)m_pDoc->m_ThreadArray.GetAt(icol);
		CString name=pTempThreadPTR->m_pSPName+" : ";
		name=name+pTempThreadPTR->m_Connect;
	 	pComboBox->AddString(name);//add its name to the list
	}
	pComboBox->SetCurSel(m_pDoc->m_iCountConect-1); //last one selected

	m_SPName=pTempThreadPTR->m_pSPName;
	m_Delay=pTempThreadPTR->m_msecs;
	m_NoThreads=pTempThreadPTR->m_Threads;
	UpdateData(FALSE);
	m_SPEdit.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	
}

void CEditSPDlg::OnSelchangeDsn() 
{
	CComboBox *pComboBox = (CComboBox*)GetDlgItem(IDC_DSN);
	ASSERT(pComboBox!=NULL);
	long icol=pComboBox->GetCurSel();
	if(icol==CB_ERR)
		return;
//	OnApply(); //applies the changes to the new selection

	CODBCThread *pTempThreadPTR = (CODBCThread*)m_pDoc->m_ThreadArray.GetAt(icol);
	m_SPName=pTempThreadPTR->m_pSPName;
	m_Delay=pTempThreadPTR->m_msecs;
	m_NoThreads=pTempThreadPTR->m_Threads;
	UpdateData(FALSE);
}

void CEditSPDlg::OnOK() 
{
	OnApply();
	CDialog::OnOK();
}
