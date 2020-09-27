// GetNodesErrorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xboxaddin.h"
#include "GetNodesErrorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetNodesErrorDlg dialog


CGetNodesErrorDlg::CGetNodesErrorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetNodesErrorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetNodesErrorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CGetNodesErrorDlg, CDialog)
	//{{AFX_MSG_MAP(CGetNodesErrorDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetNodesErrorDlg message handlers

BOOL CGetNodesErrorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemText( IDC_EDIT_REFERENCES, m_strErrorText );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
