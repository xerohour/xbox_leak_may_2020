/////////////////////////////////////////////////////////////////////////////
// cafepage.cpp
//
// email	date		change
// briancr	11/03/94	created
//
// copyright 1994 Microsoft

// Implementation of the CCAFEPage class

#include "stdafx.h"
#include "cafepage.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCAFEPage property page

IMPLEMENT_DYNCREATE(CCAFEPage, CPropertyPage)

CCAFEPage::CCAFEPage(CSettings* pSettings /*= NULL*/)
: CPropertyPage(CCAFEPage::IDD),
  m_pSettings(pSettings)
{
	//{{AFX_DATA_INIT(CCAFEPage)
	m_bStdToolbar = FALSE;
	m_bSubSuiteToolbar = FALSE;
	m_bViewportToolbar = FALSE;
	m_bStatusBar = FALSE;
	//}}AFX_DATA_INIT
}

CCAFEPage::~CCAFEPage()
{
}

void CCAFEPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCAFEPage)
	DDX_Check(pDX, IDC_CAFEStdToolbar, m_bStdToolbar);
	DDX_Check(pDX, IDC_CAFESubSuiteToolbar, m_bSubSuiteToolbar);
	DDX_Check(pDX, IDC_CAFEVwprtToolbar, m_bViewportToolbar);
	DDX_Check(pDX, IDC_CAFEStatusBar, m_bStatusBar);
	//}}AFX_DATA_MAP
}

BOOL CCAFEPage::OnSetActive(void)
{
	// call the base class
	if (!CPropertyPage::OnSetActive()) {
		return FALSE;
	}

	// the settings object must exist
	ASSERT(m_pSettings);

	// initialize the page
	m_bStdToolbar = m_pSettings->GetBooleanValue(settingStdToolbar);
	m_bSubSuiteToolbar = m_pSettings->GetBooleanValue(settingSubSuiteToolbar);
	m_bViewportToolbar = m_pSettings->GetBooleanValue(settingViewportToolbar);
	m_bStatusBar = m_pSettings->GetBooleanValue(settingStatusBar);

	// copy the settings into the appropriate variables
	UpdateData(FALSE);

	return TRUE;
}

void CCAFEPage::OnOK(void)
{
	// call the base class
	CPropertyPage::OnOK();

	// the settings object must exist
	ASSERT(m_pSettings);

	// retrieve the settings from the UI
	UpdateData(TRUE);

	m_pSettings->SetBooleanValue(settingStdToolbar, m_bStdToolbar);
	m_pSettings->SetBooleanValue(settingSubSuiteToolbar, m_bSubSuiteToolbar);
	m_pSettings->SetBooleanValue(settingViewportToolbar, m_bViewportToolbar);
	m_pSettings->SetBooleanValue(settingStatusBar, m_bStatusBar);

	// write settings to the registry
	m_pSettings->WriteRegistry();
}

BEGIN_MESSAGE_MAP(CCAFEPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCAFEPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCAFEPage message handlers
