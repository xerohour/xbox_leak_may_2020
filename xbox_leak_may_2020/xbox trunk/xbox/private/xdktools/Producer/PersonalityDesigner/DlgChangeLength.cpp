// DlgChangeLength.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "personalitydesigner.h"
#include "DlgChangeLength.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeLength dialog


CDlgChangeLength::CDlgChangeLength(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChangeLength::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChangeLength)
	m_strLength = _T("");
	//}}AFX_DATA_INIT

	bValidate = FALSE;
}


void CDlgChangeLength::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChangeLength)
	DDX_Control(pDX, IDC_SPIN1, m_spinLength);
	DDX_Text(pDX, IDC_EDITLENGTH, m_strLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChangeLength, CDialog)
	//{{AFX_MSG_MAP(CDlgChangeLength)
	ON_EN_CHANGE(IDC_EDITLENGTH, OnChangeEditlength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeLength message handlers

BOOL CDlgChangeLength::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_spinLength.SetRange(1,100);
	CEdit* pEdit = static_cast<CEdit*>(GetDlgItem(IDC_EDITLENGTH));
	pEdit->LimitText(3);
	bValidate = FALSE;	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChangeLength::OnChangeEditlength() 
{
	if(bValidate)
	{
		BOOL b;
		short n = (short)(GetDlgItemInt(IDC_EDITLENGTH, &b));
		if(!b || n < 1 || n > 100)
		{
			AfxMessageBox("Illegal Length - resetting to original value");
			m_strLength.Format("%d", m_nChordMapLength);
			UpdateData(FALSE);
		}
	}

}
