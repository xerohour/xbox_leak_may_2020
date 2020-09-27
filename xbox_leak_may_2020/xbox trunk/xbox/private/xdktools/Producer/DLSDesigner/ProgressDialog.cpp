// ProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "Wave.h"
#include "ProgressDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog dialog


CProgressDialog::CProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDialog)
	//}}AFX_DATA_INIT
	m_nRange = 100;
	Create(IDD, pParent);
}


void CProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDialog)
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressBar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog message handlers

void  CProgressDialog::SetRange(int nRange)
{
	m_nRange = nRange;
}



BOOL CProgressDialog::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();
	
	m_ProgressBar.SetRange(0, short(m_nRange));
	m_ProgressBar.SetStep(1);
	m_ProgressBar.SetPos(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CProgressDialog::DestroyWindow() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return CDialog::DestroyWindow();
}
