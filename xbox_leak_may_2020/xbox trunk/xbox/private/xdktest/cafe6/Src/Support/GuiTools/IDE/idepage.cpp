/////////////////////////////////////////////////////////////////////////////
// idepage.cpp
//
// email	date		change
// briancr	11/10/94	created
//
// copyright 1994 Microsoft

// Implementation of the CIDEPage class

#include "stdafx.h"
#include "idepage.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIDEPage property page

IMPLEMENT_DYNCREATE(CIDEPage, CPropertyPage)

CIDEPage::CIDEPage(CSettings* pSettings /*= NULL*/, CIDE* pIDE /*= NULL*/)
: CPropertyPage(CIDEPage::IDD),
  m_pSettings(pSettings),
  m_pIDE(pIDE)
{
	//{{AFX_DATA_INIT(CIDEPage)
	m_bClose = FALSE;
	m_bDebugProcess = FALSE;
	m_bIgnoreASSERT = FALSE;
	m_bBreakASSERT = FALSE;
	m_bNewApp = FALSE;
	m_bPersist = FALSE;
	m_bUseKey = FALSE;
	m_strCmdLine = _T("");
	m_strFilename = _T("");
	m_strKey = _T("");
	m_strWorkDir = _T("");
	//}}AFX_DATA_INIT
}

CIDEPage::~CIDEPage()
{
}

void CIDEPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIDEPage)
	DDX_Check(pDX, IDC_IDEClose, m_bClose);
	DDX_Check(pDX, IDC_IDEDebugProcess, m_bDebugProcess);
	DDX_Check(pDX, IDC_IDEIgnoreASSERT, m_bIgnoreASSERT);
	DDX_Check(pDX, IDC_IDEBreakASSERT, m_bBreakASSERT);
	DDX_Check(pDX, IDC_IDENewApp, m_bNewApp);
	DDX_Check(pDX, IDC_IDEPersist, m_bPersist);
	DDX_Check(pDX, IDC_IDEUseKey, m_bUseKey);
	DDX_Text(pDX, IDE_IDECmdLine, m_strCmdLine);
	DDX_Text(pDX, IDE_IDEFilename, m_strFilename);
	DDX_Text(pDX, IDE_IDEKey, m_strKey);
	DDX_Text(pDX, IDE_IDEWorkDir, m_strWorkDir);
	//}}AFX_DATA_MAP

}

BOOL CIDEPage::OnSetActive(void)
{
	// call the base class
	if (!CPropertyPage::OnSetActive()) {
		return FALSE;
	}

	// the settings object must exist
	ASSERT(m_pSettings);

	// fill in the UI from the settings object
	m_bIgnoreASSERT = m_pSettings->GetBooleanValue(settingIgnoreASSERT);
	m_bBreakASSERT = m_pSettings->GetBooleanValue(settingBreakASSERT);
	m_bNewApp = m_pSettings->GetBooleanValue(settingNewApp);
	m_bClose = m_pSettings->GetBooleanValue(settingClose);
	m_bDebugProcess = m_pSettings->GetBooleanValue(settingDebugProcess);
	m_bPersist = m_pSettings->GetBooleanValue(settingPersist);
	m_bUseKey = m_pSettings->GetBooleanValue(settingIDEUseKey);

	// Get the filename based on whether we're gonna run debug or retail.
	if (m_pSettings->GetBooleanValue(settingDebugBuild))
		m_strFilename = m_pSettings->GetTextValue(settingDebugFilename);
	else
		m_strFilename = m_pSettings->GetTextValue(settingFilename);

	m_strCmdLine = m_pSettings->GetTextValue(settingCmdLine);
	m_strWorkDir = m_pSettings->GetTextValue(settingWorkingDir);
	m_strKey = m_pSettings->GetTextValue(settingIDEKey);

	UpdateData(FALSE);

	OnIDEUseKey();
	OnIDEDebugProcess();

	return TRUE;
}

void CIDEPage::OnOK(void)
{
	// call the base class
	CPropertyPage::OnOK();

	// the settings object must exist
	ASSERT(m_pSettings);

	// retrieve the settings from the UI
	UpdateData(TRUE);

	// update the settings object from the UI
	m_pSettings->SetBooleanValue(settingIgnoreASSERT, m_bIgnoreASSERT);
	m_pSettings->SetBooleanValue(settingBreakASSERT, m_bBreakASSERT);
	m_pSettings->SetBooleanValue(settingNewApp, m_bNewApp);
	m_pSettings->SetBooleanValue(settingClose, m_bClose);
	m_pSettings->SetBooleanValue(settingDebugProcess, m_bDebugProcess);
	m_pSettings->SetBooleanValue(settingPersist,m_bPersist);
	m_pSettings->SetBooleanValue(settingIDEUseKey, m_bUseKey);

	if (m_pSettings->GetBooleanValue(settingDebugBuild))
		m_pSettings->SetTextValue(settingDebugFilename, m_strFilename);
	else
		m_pSettings->SetTextValue(settingFilename, m_strFilename);

	m_pSettings->SetTextValue(settingCmdLine, m_strCmdLine);
	m_pSettings->SetTextValue(settingWorkingDir, m_strWorkDir);
	m_pSettings->SetTextValue(settingIDEKey, m_strKey);

	// write settings to the registry
	m_pSettings->WriteRegistry();
}

BEGIN_MESSAGE_MAP(CIDEPage, CPropertyPage)
	//{{AFX_MSG_MAP(CIDEPage)
	ON_BN_CLICKED(IDC_IDEUseKey, OnIDEUseKey)
	ON_BN_CLICKED(IDB_IDEConnections, OnIDEConnections)
	ON_BN_CLICKED(IDC_IDEDebugProcess, OnIDEDebugProcess)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIDEPage message handlers

void CIDEPage::OnIDEUseKey()
{
	// retrieve the settings from the UI
	UpdateData(TRUE);

	// get pointers to the connector and baud rate drop downs and text
	CWnd* pKeyText = GetDlgItem(IDS_IDEKeyText);
	CWnd* pKeyEdit = GetDlgItem(IDE_IDEKey);

	// enable connector and baud rate based on setting of use com port
	pKeyText->EnableWindow(m_bUseKey);
	pKeyEdit->EnableWindow(m_bUseKey);
}

void CIDEPage::OnIDEConnections()
{
	m_pIDE->SetConnections(m_hWnd);
}

void CIDEPage::OnIDEDebugProcess()
{
	// Set the current state of the member vars to match those of what's being displayed.
	UpdateData(TRUE);

	if (m_bDebugProcess == FALSE)
		GetDlgItem(IDC_IDEClose)->EnableWindow();
	else
	{
		m_bClose = TRUE;
		GetDlgItem(IDC_IDEClose)->EnableWindow(FALSE);
		UpdateData(FALSE);
	}
}
