// $$frame_ifile$$.cpp : implementation of the $$FRAME_CLASS$$ class
//

#include "stdafx.h"
#include "$$root$$.h"

#include "$$frame_hfile$$.h"
$$IF(PROJTYPE_SDI)
$$IF(PROJTYPE_EXPLORER)
#include "$$treeview_hfile$$.h"
#include "$$view_hfile$$.h"
$$ENDIF
$$ENDIF

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$FRAME_CLASS$$

$$IF(PROJTYPE_MDI || NODOCVIEW)
IMPLEMENT_DYNAMIC($$FRAME_CLASS$$, $$FRAME_BASE_CLASS$$)
$$ELSE
IMPLEMENT_DYNCREATE($$FRAME_CLASS$$, $$FRAME_BASE_CLASS$$)
$$ENDIF

BEGIN_MESSAGE_MAP($$FRAME_CLASS$$, $$FRAME_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$FRAME_CLASS$$)
$$IF(VERBOSE)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
$$ENDIF
	ON_WM_CREATE()
$$IF(PROJTYPE_SDI)
$$IF(NODOCVIEW)
	ON_WM_SETFOCUS()
$$ENDIF 
$$ENDIF 
	//}}AFX_MSG_MAP
$$IF(HELP)
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, $$FRAME_BASE_CLASS$$::OnHelpFinder)
	ON_COMMAND(ID_HELP, $$FRAME_BASE_CLASS$$::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, $$FRAME_BASE_CLASS$$::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, $$FRAME_BASE_CLASS$$::OnHelpFinder)
$$ENDIF
$$IF(PROJTYPE_SDI)
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
$$ENDIF
$$ENDIF
$$ENDIF
END_MESSAGE_MAP()
$$IF(STATUSBAR)

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
$$IF(DO_KANA)
	ID_INDICATOR_KANA,
$$ENDIF	// DO_KANA
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
$$ENDIF //STATUSBAR

/////////////////////////////////////////////////////////////////////////////
// $$FRAME_CLASS$$ construction/destruction

$$FRAME_CLASS$$::$$FRAME_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: add member initialization code here
	
$$ENDIF
}

$$FRAME_CLASS$$::~$$FRAME_CLASS$$()
{
}

int $$FRAME_CLASS$$::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ($$FRAME_BASE_CLASS$$::OnCreate(lpCreateStruct) == -1)
		return -1;
$$IF(PROJTYPE_SDI)
$$IF(NODOCVIEW)
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
$$ENDIF 
$$ENDIF //SDI + NODOCVIEW
$$IF(TOOLBAR)
	
$$IF(REBAR)
	if (!m_wndToolBar.CreateEx(this) ||
$$ELSE
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
$$ENDIF //REBAR
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
$$IF(REBAR)
$$IF( CONTAINER || CONTAINER_SERVER)
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() & ~CBRS_HIDE_INPLACE);
$$ENDIF // ANY CONTAINER
	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}
$$IF( CONTAINER || CONTAINER_SERVER)
	m_wndReBar.SetBarStyle(m_wndReBar.GetBarStyle() & ~CBRS_HIDE_INPLACE);
$$ENDIF// ANY CONTAINER
$$ENDIF //REBAR
$$ENDIF //TOOLBAR
$$IF(STATUSBAR)

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
$$ENDIF //STATUSBAR
$$IF(TOOLBAR)

$$IF(REBAR)
$$IF(VERBOSE)
	// TODO: Remove this if you don't want tool tips
$$ENDIF
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	m_wndDlgBar.SetBarStyle(m_wndDlgBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
$$ENDIF//MINI_SERVER || FULL_SERVER || CONTAINER_SERVER
$$ELSE
$$IF(VERBOSE)
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
$$ENDIF
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
$$ENDIF //REBAR
$$ENDIF //TOOLBAR	

	return 0;
}
$$IF(SPLITTER_SDI || PROJTYPE_EXPLORER )
$$IF(PROJTYPE_SDI)

BOOL $$FRAME_CLASS$$::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
$$IF(PROJTYPE_EXPLORER)
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS($$TREEVIEW_CLASS$$), CSize(100, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS($$VIEW_CLASS$$), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
$$ELSE //!PROJTYPE_EXPLORER
	return m_wndSplitter.Create(this,
$$IF(VERBOSE)
		2, 2,               // TODO: adjust the number of rows, columns
		CSize(10, 10),      // TODO: adjust the minimum pane size
$$ELSE //!VERBOSE
		2, 2,
		CSize(10, 10),
$$ENDIF //VERBOSE
		pContext);
$$ENDIF //PROJTYPE_EXPLORER
}
$$ENDIF //SDI
$$ENDIF //SPLITTER || EXPLORER

BOOL $$FRAME_CLASS$$::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !$$FRAME_BASE_CLASS$$::PreCreateWindow(cs) )
		return FALSE;
$$IF(VERBOSE)
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

$$ENDIF //VERBOSE
$$IF(FRAME_STYLES)
	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		$$FRAME_STYLE_FLAGS$$;

$$ENDIF //FRAME_STYLES
$$IF(PROJTYPE_SDI)
$$IF(NODOCVIEW)
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
$$ENDIF 
$$ENDIF //SDI + NODOCVIEW
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// $$FRAME_CLASS$$ diagnostics

#ifdef _DEBUG
void $$FRAME_CLASS$$::AssertValid() const
{
	$$FRAME_BASE_CLASS$$::AssertValid();
}

void $$FRAME_CLASS$$::Dump(CDumpContext& dc) const
{
	$$FRAME_BASE_CLASS$$::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// $$FRAME_CLASS$$ message handlers
$$IF(PROJTYPE_SDI)
$$IF(NODOCVIEW)
void $$FRAME_CLASS$$::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL $$FRAME_CLASS$$::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
$$ENDIF 
$$ENDIF //SDI + NODOCVIEW

$$IF(PROJTYPE_SDI)
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
$$VIEW_CLASS$$* $$FRAME_CLASS$$::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	$$VIEW_CLASS$$* pView = DYNAMIC_DOWNCAST($$VIEW_CLASS$$, pWnd);
	return pView;
}

void $$FRAME_CLASS$$::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
$$IF(VERBOSE)
	// TODO: customize or extend this code to handle choices on the
	// View menu.
$$ENDIF //VERBOSE

	$$VIEW_CLASS$$* pView = GetRightPane(); 

$$IF(VERBOSE)
	// if the right-hand pane hasn't been created or isn't a view,
	// disable commands in our range
$$ENDIF //VERBOSE

	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

$$IF(VERBOSE)
		// if the command is ID_VIEW_LINEUP, only enable command
		// when we're in LVS_ICON or LVS_SMALLICON mode

$$ENDIF //VERBOSE
		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
$$IF(VERBOSE)
			// otherwise, use dots to reflect the style of the view
$$ENDIF //VERBOSE
			pCmdUI->Enable();
			BOOL bChecked = FALSE;

			switch (pCmdUI->m_nID)
			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
				break;

			case ID_VIEW_SMALLICON:
				bChecked = (dwStyle == LVS_SMALLICON);
				break;

			case ID_VIEW_LARGEICON:
				bChecked = (dwStyle == LVS_ICON);
				break;

			case ID_VIEW_LIST:
				bChecked = (dwStyle == LVS_LIST);
				break;

			default:
				bChecked = FALSE;
				break;
			}

			pCmdUI->SetRadio(bChecked ? 1 : 0);
		}
	}
}


void $$FRAME_CLASS$$::OnViewStyle(UINT nCommandID)
{
$$IF(VERBOSE)
	// TODO: customize or extend this code to handle choices on the
	// View menu.
$$ENDIF //VERBOSE
	$$VIEW_CLASS$$* pView = GetRightPane();

$$IF(VERBOSE)
	// if the right-hand pane has been created and is a $$VIEW_CLASS$$,
	// process the menu commands...
$$ENDIF //VERBOSE
	if (pView != NULL)
	{
		DWORD dwStyle = -1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
$$IF(VERBOSE)
				// ask the list control to snap to grid
$$ENDIF //VERBOSE
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;

$$IF(VERBOSE)
		// other commands change the style on the list control
$$ENDIF //VERBOSE
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;

		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;

		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;

		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

$$IF(VERBOSE)
		// change the style; window will repaint automatically
$$ENDIF //VERBOSE
		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
	}
}
$$ENDIF
$$ENDIF
$$ENDIF
