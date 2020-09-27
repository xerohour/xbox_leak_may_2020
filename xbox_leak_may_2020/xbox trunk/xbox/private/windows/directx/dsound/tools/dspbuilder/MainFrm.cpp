/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	MainFrm.cpp

Abstract:

	Implementation of the CMainFrame class

Author:

	Robert Heitkamp (robheit) 16-Jul-2001

Revision History:

	16-Jul-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"
#include "MainFrm.h"

//------------------------------------------------------------------------------
//	Debug defines
//------------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//	Implementation
//------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

//------------------------------------------------------------------------------
//	Message Map
//------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_UPDATE_COMMAND_UI(IDS_STATUS_BAR_TEXT, OnUpdateStatusBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};

//------------------------------------------------------------------------------
//	CMainFrame::CMainFrame
//------------------------------------------------------------------------------
CMainFrame::CMainFrame(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
}

//------------------------------------------------------------------------------
//	CMainFrame::~CMainFrame
//------------------------------------------------------------------------------
CMainFrame::~CMainFrame(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
}

//------------------------------------------------------------------------------
//	CMainFrame::OnCreate
//------------------------------------------------------------------------------
int 
CMainFrame::OnCreate(
					 IN LPCREATESTRUCT lpCreateStruct
					 )
/*++

Routine Description:

	Handles the creation event.

Arguments:

	IN lpCreateStruct -	Construction parameters

Return Value:

	0 on success, any other value on error

--*/
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
						  CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | 
							   CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | 
							   CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) || 
		!m_wndStatusBar.SetIndicators(indicators, 
									  sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetPaneInfo(1, IDS_STATUS_BAR_TEXT, SBPS_NORMAL, 400);
	m_wndStatusBar.SetPaneText(1, _T("MyText"));

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

//------------------------------------------------------------------------------
//	CMainFrame::PreCreateWindow
//------------------------------------------------------------------------------
BOOL 
CMainFrame::PreCreateWindow(
							IN CREATESTRUCT& cs
							)
/*++

Routine Description:

	Called before the window is created

Arguments:

	IN cs -	Create data

Return Value:

	FALSE on error, TRUE on success

--*/
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

//	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

#ifdef _DEBUG
//------------------------------------------------------------------------------
//	CMainFrame::AssertValid
//------------------------------------------------------------------------------
void 
CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

//------------------------------------------------------------------------------
//	CMainFrame::Dump
//------------------------------------------------------------------------------
void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

//------------------------------------------------------------------------------
//	CMainFrame::OnSetFocus
//------------------------------------------------------------------------------
void 
CMainFrame::OnSetFocus(
					   IN CWnd*
					   )
/*++

Routine Description:

	Called when the window gains focus

Arguments:

	IN pOldWnd -	Prevoous window

Return Value:

	None

--*/
{
	// Forward focus to the view window
	m_wndView.SetFocus();
}

//------------------------------------------------------------------------------
//	CMainFrame::OnCmdMsg
//------------------------------------------------------------------------------
BOOL 
CMainFrame::OnCmdMsg(
					 IN UINT				nID, 
					 IN int					nCode, 
					 IN void*				pExtra, 
					 IN AFX_CMDHANDLERINFO*	pHandlerInfo
					 )
/*++

Routine Description:

	Handles all command messages

Arguments:

	IN nID -			ID
	IN nCode -			Code
	IN pExtra -			Extra data
	IN pHandlerInfo -	Handler info

Return Value:

	TRUE if the message is handled; otherwise FALSE

--*/
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//------------------------------------------------------------------------------
//	CMainFrame::OnInitMenuPopup
//------------------------------------------------------------------------------
void 
CMainFrame::OnInitMenuPopup(
							IN CMenu*	pPopupMenu, 
							IN UINT		nIndex, 
							IN BOOL		bSysMenu
							)
/*++

Routine Description:

	Handles popup menus. Basically ensure the right items are checked

Arguments:

	IN pPopupMenu -	Menu
	IN nIndex -		Menu index
	IN bSysMenu -	TRUE if the system menu, FALSE othereise

Return Value:

	None

--*/
{
	CWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	if(!bSysMenu)
	{
		// View menu?
		if(nIndex == 1)
		{
			// Status bar
			if(m_wndStatusBar.IsWindowVisible())
				pPopupMenu->CheckMenuItem(ID_VIEW_STATUS_BAR, MF_CHECKED);
			else
				pPopupMenu->CheckMenuItem(ID_VIEW_STATUS_BAR, MF_UNCHECKED);

			// Toolbar
			if(m_wndToolBar.IsWindowVisible())
				pPopupMenu->CheckMenuItem(ID_VIEW_TOOLBAR, MF_CHECKED);
			else
				pPopupMenu->CheckMenuItem(ID_VIEW_TOOLBAR, MF_UNCHECKED);
		}

		// All other menus
		m_wndView.EnableMenuItem(pPopupMenu, nIndex);
	}
}

//------------------------------------------------------------------------------
//	CMainFrame::OnAppExit
//------------------------------------------------------------------------------
void
CMainFrame::OnAppExit(void)
/*++

Routine Description:

	Handles exit requests

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_wndView.Quit())
		PostMessage(WM_CLOSE, 0, 0);
}

//------------------------------------------------------------------------------
//	CMainFrame::OnUpdateStatusBar
//------------------------------------------------------------------------------
void
CMainFrame::OnUpdateStatusBar(
							  IN CCmdUI*	pCmdUI
							  )
/*++

Routine Description:

	Upates the text in the status bar

Arguments:

	IN pCmdUI -	ON_UPDATE_COMMAND_UI handler	

Return Value:

	None

--*/
{
	pCmdUI->SetText(m_wndView.GetStatusBarText());
}
