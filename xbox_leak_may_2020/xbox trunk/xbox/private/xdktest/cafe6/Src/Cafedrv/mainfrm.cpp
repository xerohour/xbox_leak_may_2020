///////////////////////////////////////////////////////////////////////////////
//	MAINFRM.CPP
//
//	Created by :			Date :
//		DavidGa					8/13/93
//
//	Description :
//		Implementation of the CMainFrame class
//

#include "stdafx.h"
#include "mainfrm.h"
#include "cafedrv.h"
#include "tbdlg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define TOOLBAR_KEY "Toolbars"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
    ON_COMMAND(IDM_OptionsToolbars, OnOptionsToolbars)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDM_HelpContents, CMDIFrameWnd::OnHelpIndex)

	ON_CONTROL(CBN_SELCHANGE, IDC_CAFEBarToolset, OnToolset)
	ON_CONTROL(CBN_SELCHANGE, IDC_CAFEBarBuildType, OnBuildType)
	ON_CONTROL(CBN_SELCHANGE, IDC_CAFEBarLanguage, OnLanguage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,			// status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_psettingsWnd(NULL)
{
	// get a pointer to the app
	CCAFEDriver *pApp = ((CCAFEDriver *)AfxGetApp());

	m_ptbCAFEBar = new CCAFEBar(pApp->GetSettings());
	m_ptbSubSuiteBar = new CSubSuiteBar;
	m_ptbViewportBar = new CViewportBar;
}

CMainFrame::~CMainFrame()
{
	// delete the toolbars
	delete m_ptbCAFEBar;
	delete m_ptbSubSuiteBar;
	delete m_ptbViewportBar;

	// delete the settings object
	if (m_psettingsWnd) {
		delete m_psettingsWnd;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame operations

/////////////////////////////////////////////////////////////////////////////
// CMainFrame initialization

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// create toolbars and status bar
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ptbCAFEBar->CreateBar(this)) {
		return -1;
	}
	if (!m_ptbSubSuiteBar->CreateBar(this)) {
		return -1;
	}
	if (!m_ptbViewportBar->CreateBar(this)) {
		return -1;
	}

	if (!CreateStatusBar())
		return -1;

	// enable docking in this frame
	EnableDocking(CBRS_ALIGN_ANY);

	// initialize settings
	InitializeSettings();

	// position this frame
	MoveWindow(m_psettingsWnd->GetIntValue(settingCAFEWndX),
			m_psettingsWnd->GetIntValue(settingCAFEWndY),
			m_psettingsWnd->GetIntValue(settingCAFEWndWidth),
			m_psettingsWnd->GetIntValue(settingCAFEWndHeight));

	// default to docking all the toolbars
	DockControlBar(m_ptbCAFEBar);
	DockControlBar(m_ptbSubSuiteBar);
	DockControlBar(m_ptbViewportBar);

	// REVIEW(briancr): hide the viewport toolbar, since it's not supported yet
//	ShowControlBar(m_ptbViewportBar, FALSE, FALSE);

	LoadBarState(TOOLBAR_KEY);

	return 0;
}

void CMainFrame::OnClose() 
{
	// save the toolbar settings
	SaveBarState(TOOLBAR_KEY);

	// update settings based on UI
	UpdateSettings();

	// call the base class
	CMDIFrameWnd::OnClose();
}

/////////////////////////////////////////////////////////////////////////////
// Operations

/////////////////////////////////////////////////////////////////////////////
// Operations (internal)

BOOL CMainFrame::InitializeSettings(void)
{
	// get a pointer to the app
	CCAFEDriver *pApp = ((CCAFEDriver *)AfxGetApp());

	// if the window settings object already exists, delete it
	if (m_psettingsWnd) {
		delete m_psettingsWnd;
	}

	// create a new window settings object
	m_psettingsWnd = new CSettings(pApp->GetSettings()->GetTextValue(settingCAFEKey)+"\\Windows");

	// set up window defaults
	m_psettingsWnd->SetIntValue(settingCAFEWndX, 10);
	m_psettingsWnd->SetIntValue(settingCAFEWndY, 10);
	m_psettingsWnd->SetIntValue(settingCAFEWndWidth, 700);
	m_psettingsWnd->SetIntValue(settingCAFEWndHeight, 600);

	// read registry settings
	m_psettingsWnd->ReadRegistry();
	// write the settings back out to initialize registry
	m_psettingsWnd->WriteRegistry();

	#ifdef _DEBUG
		m_psettingsWnd->DumpSettings("Window settings");
	#endif // _DEBUG

	return TRUE;
}

BOOL CMainFrame::UpdateSettings(void)
{
	CRect rectWnd;

	// the settings object must be valid
	ASSERT(m_psettingsWnd);
	if (!m_psettingsWnd) {
		return FALSE;
	}

	// update frame window settings
	GetWindowRect(&rectWnd);
	m_psettingsWnd->SetIntValue(settingCAFEWndX, rectWnd.left);
	m_psettingsWnd->SetIntValue(settingCAFEWndY, rectWnd.top);
	m_psettingsWnd->SetIntValue(settingCAFEWndWidth, rectWnd.Width());
	m_psettingsWnd->SetIntValue(settingCAFEWndHeight, rectWnd.Height());

	// write settings to the registry
	m_psettingsWnd->WriteRegistry();

	return TRUE;
}

BOOL CMainFrame::CreateStatusBar(void)
{
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE("Failed to create status bar\n");
		return FALSE;		// fail to create
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnOptionsToolbars()
{
	CToolbarsDlg dlgToolbars;

	// if the style says the toolbar is visible then it is visible.
	dlgToolbars.m_bShowCAFEToolbar = ((m_ptbCAFEBar->GetStyle() & WS_VISIBLE) != 0);
	dlgToolbars.m_bShowSubSuiteToolbar = ((m_ptbSubSuiteBar->GetStyle() & WS_VISIBLE) != 0);
	dlgToolbars.m_bShowViewportToolbar = ((m_ptbViewportBar->GetStyle() & WS_VISIBLE) != 0);
	dlgToolbars.m_bShowStatusBar = ((m_wndStatusBar.GetStyle() & WS_VISIBLE) != 0);

	if (dlgToolbars.DoModal() == IDOK) {
		ShowControlBar(m_ptbCAFEBar, dlgToolbars.m_bShowCAFEToolbar, FALSE);
		ShowControlBar(m_ptbSubSuiteBar, dlgToolbars.m_bShowSubSuiteToolbar, FALSE);
		ShowControlBar(m_ptbViewportBar, dlgToolbars.m_bShowViewportToolbar, FALSE);
		ShowControlBar(&m_wndStatusBar, dlgToolbars.m_bShowStatusBar, FALSE);
	}
}

void CMainFrame::OnToolset(void)
{
	m_ptbCAFEBar->UpdateToolset();
}

void CMainFrame::OnBuildType(void)
{
	m_ptbCAFEBar->UpdateBuildType();
}

void CMainFrame::OnLanguage(void)
{
	m_ptbCAFEBar->UpdateLanguage();
}
