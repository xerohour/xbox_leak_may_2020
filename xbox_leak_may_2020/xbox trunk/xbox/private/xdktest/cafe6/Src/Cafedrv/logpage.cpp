/////////////////////////////////////////////////////////////////////////////
// logpage.cpp
//
// email	date		change
// briancr	11/10/94	created
//
// copyright 1994 Microsoft

// Implementation of the CLogPage class

#include "stdafx.h"
#include "logpage.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogPage property page

IMPLEMENT_DYNCREATE(CLogPage, CPropertyPage)

CLogPage::CLogPage()
: CPropertyPage(CLogPage::IDD)
{
	//{{AFX_DATA_INIT(CLogPage)
	m_nMaxFail = 0;
	m_bPost = FALSE;
	m_bCom = FALSE;
	m_strConnector = _T("");
	m_strBaudRate = _T("");
	m_bComments = FALSE;
	m_strResultsFile = _T("");
	m_strSummaryFile = _T("");
	m_strWorkDir = _T("");
	m_bDebugOutput = FALSE;
	//}}AFX_DATA_INIT
}

CLogPage::~CLogPage()
{
}

void CLogPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogPage)
	DDX_Text(pDX, IDC_LogMaxFail, m_nMaxFail);
	DDX_Check(pDX, IDC_LogPost, m_bPost);
	DDX_Check(pDX, IDC_LogToCom, m_bCom);
	DDX_CBString(pDX, IDC_LogConnector, m_strConnector);
	DDX_CBString(pDX, IDC_LogBaudRate, m_strBaudRate);
	DDX_Check(pDX, IDC_LogComments, m_bComments);
	DDX_Text(pDX, IDE_LogResultsFile, m_strResultsFile);
	DDX_Text(pDX, IDE_LogSummaryFile, m_strSummaryFile);
	DDX_Text(pDX, IDE_LogWorkingDir, m_strWorkDir);
	DDX_Check(pDX, IDC_LogDebugOutput, m_bDebugOutput);
	//}}AFX_DATA_MAP
}

BOOL CLogPage::OnSetActive(void)
{
	// call the base class
	if (!CPropertyPage::OnSetActive()) {
		return FALSE;
	}

	// set up UI
	OnLogToCom();

	return TRUE;
}

BEGIN_MESSAGE_MAP(CLogPage, CPropertyPage)
	//{{AFX_MSG_MAP(CLogPage)
	ON_BN_CLICKED(IDC_LogToCom, OnLogToCom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLogPage message handlers

void CLogPage::OnLogToCom() 
{
	// retrieve the settings from the UI
	UpdateData(TRUE);

	// since we don't support the COM options yet, just gray out the check box, too
	CWnd* pCOMPort = GetDlgItem(IDC_LogToCom);
	pCOMPort->EnableWindow(FALSE);

	// get pointers to the connector and baud rate drop downs and text
	CWnd* pConnectorText = GetDlgItem(IDC_LogConnectorText);
	CWnd* pConnector = GetDlgItem(IDC_LogConnector);
	CWnd* pBaudRateText = GetDlgItem(IDC_LogBaudRateText);
	CWnd* pBaudRate = GetDlgItem(IDC_LogBaudRate);

	// enable connector and baud rate based on setting of use com port
	pConnectorText->EnableWindow(m_bCom);
	pConnector->EnableWindow(m_bCom);
	pBaudRateText->EnableWindow(m_bCom);
	pBaudRate->EnableWindow(m_bCom);
}
