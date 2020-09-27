// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#pragma hdrstop

#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	//
	// CAUTION!  If you add/remove buttons, you MUST update
	// the INDEX_START_IMAGE and INDEX_STOP_IMAGE defines
	// in SPYXX.H!
	//

#ifdef OLD_TOOLBAR
	// same order as in the bitmap 'toolbarx.bmp'
	ID_SPY_MESSAGES,
	ID_SPY_WINDOWS,
	ID_SPY_PROCESSES,
	ID_SPY_THREADS,
		ID_SEPARATOR,
	ID_MESSAGES_STARTSTOP,
	ID_MESSAGES_OPTIONS,
	ID_MESSAGES_CLEAR,
		ID_SEPARATOR,
	ID_SPY_FINDWINDOW,
		ID_SEPARATOR,
	ID_SEARCH_FIND,
	ID_SEARCH_FINDNEXT,
	ID_SEARCH_FINDPREV,
		ID_SEPARATOR,
	ID_TREE_EXPAND_ONE_LEVEL,
	ID_TREE_EXPAND_BRANCH,
	ID_TREE_EXPAND_ALL,
	ID_TREE_COLLAPSE,
		ID_SEPARATOR,
	ID_WINDOW_CASCADE,
	ID_WINDOW_TILE_HORZ,
	ID_WINDOW_TILE_VERT
#else	// OLD_TOOLBAR
	// same order as in the bitmap 'toolbar.bmp'
	ID_SPY_WINDOWS,
	ID_SPY_PROCESSES,
	ID_SPY_THREADS,
		ID_SEPARATOR,
	ID_SPY_MESSAGES,
		ID_SEPARATOR,
	ID_MESSAGES_STARTSTOP,
	ID_MESSAGES_OPTIONS,
	ID_MESSAGES_CLEAR,
		ID_SEPARATOR,
	ID_SPY_FINDWINDOW,
		ID_SEPARATOR,
	ID_TREE_EXPAND_ONE_LEVEL,
	ID_TREE_EXPAND_BRANCH,
	ID_TREE_EXPAND_ALL,
	ID_TREE_COLLAPSE,
		ID_SEPARATOR,
	ID_SEARCH_FIND,
	ID_SEARCH_FINDNEXT,
	ID_SEARCH_FINDPREV,
		ID_SEPARATOR,
	ID_WINDOW_CASCADE,
	ID_WINDOW_TILE_HORZ,
	ID_WINDOW_TILE_VERT
#endif	// OLD_TOOLBAR
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,			// status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowLong(m_hWnd, GWL_USERDATA, SPYXX_WINDOW_SIGNATURE);

	if (!m_wndToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) || !m_wndToolBar.SetButtons(buttons, sizeof(buttons)/sizeof(UINT)))
	{
		TRACE("Failed to create toolbar\n");
		return -1;	  // fail to create
	}

	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE("Failed to create status bar\n");
		return -1;	  // fail to create
	}

	return 0;
}



void CMainFrame::OnClose()
{
	theApp.m_wndplMain.length = sizeof(WINDOWPLACEMENT);
	theApp.m_fMainWndplValid = GetWindowPlacement(&theApp.m_wndplMain);

	CMDIFrameWnd::OnClose();
}



/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnSysColorChange()
{
	CMDIFrameWnd::OnSysColorChange();

	//
	// Recreate the Window color brush with the new system colors.
	//
	theApp.m_BrushWindow.DeleteObject();
	theApp.m_BrushWindow.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
}
