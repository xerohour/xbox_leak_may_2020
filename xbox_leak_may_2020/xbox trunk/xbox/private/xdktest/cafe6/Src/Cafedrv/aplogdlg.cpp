// clogdlg.cpp : implementation file
//

#include "stdafx.h"
#include "aplogdlg.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAppLogDlg dialog


CAppLogDlg::CAppLogDlg(CAppLog* pAppLog /*= NULL*/, CWnd* pParent /*=NULL*/)
	: CDialog(CAppLogDlg::IDD, pParent),
	  m_pAppLog(pAppLog)
{
	//{{AFX_DATA_INIT(CAppLogDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAppLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppLogDlg)
	DDX_Text(pDX, IDE_AppLog, m_strAppLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAppLogDlg, CDialog)
	//{{AFX_MSG_MAP(CAppLogDlg)
	ON_BN_CLICKED(IDB_Copy, OnCopy)
	ON_BN_CLICKED(IDB_Clear, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAppLogDlg::OnInitDialog(void)
{
	DWORD ccBuf;

	// call the base class
	CDialog::OnInitDialog();

	// fill in the edit box

	// get the length necessary to store the text
	ccBuf = 0;
	m_pAppLog->Read(m_strAppLog.GetBuffer(0), &ccBuf);

	// get the data from the file
	if (m_pAppLog->Read(m_strAppLog.GetBuffer(ccBuf+1), &ccBuf)) {
		m_strAppLog.ReleaseBuffer();
	}
	else {
		m_strAppLog.ReleaseBuffer(0);
	}

	// refresh the dialog
	UpdateData(FALSE);

	// get a pointer to the edit control
	CEdit* pEdit = (CEdit*)GetDlgItem(IDE_AppLog);

	// REVIEW(briancr): there should be a much better way of doing this,
	// but I don't know what it is
	// scroll the edit control to the last line (SetSel doesn't do it for some reason)
	int nLines = pEdit->GetLineCount();
	// -12 because there are 12 lines displayed in the edit control
	pEdit->LineScroll(nLines-12);
	// position the caret at the end of the text
	int nChar = pEdit->LineIndex(nLines-1);
	pEdit->SetSel(nChar-2, nChar, FALSE);

	// set focus to the edit control
	pEdit->SetFocus();

	// return FALSE (0) because we set focus to a control
	return FALSE;
}

void CAppLogDlg::OnCopy(void)
{
	// put the data in the clipboard
	((CEdit *)GetDlgItem(IDE_AppLog))->Copy();
}

void CAppLogDlg::OnClear(void)
{
	if (AfxMessageBox("Do you want to clear all data from the CAFE application log?", MB_OKCANCEL) == IDOK) {
		// clear the log
		m_pAppLog->Clear();

		m_strAppLog.Empty();

		// refresh the dialog
		UpdateData(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAppLogDlg message handlers
