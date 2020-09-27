// $$child_frame_ifile$$.cpp : implementation of the $$CHILD_FRAME_CLASS$$ class
//

#include "stdafx.h"
#include "$$root$$.h"

#include "$$child_frame_hfile$$.h"
$$IF(PROJTYPE_EXPLORER)
#include "$$treeview_hfile$$.h"
#include "$$view_hfile$$.h"
$$ENDIF

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$CHILD_FRAME_CLASS$$

IMPLEMENT_DYNCREATE($$CHILD_FRAME_CLASS$$, $$CHILD_FRAME_BASE_CLASS$$)

BEGIN_MESSAGE_MAP($$CHILD_FRAME_CLASS$$, $$CHILD_FRAME_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$CHILD_FRAME_CLASS$$)
$$IF(VERBOSE)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
$$ENDIF
$$IF(NODOCVIEW)
		ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
		ON_WM_SETFOCUS()
		ON_WM_CREATE()
$$ENDIF
$$IF( REBAR )
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
		ON_WM_CREATE()
$$ENDIF // (ANY SERVER)
$$ENDIF // REBAR
	//}}AFX_MSG_MAP
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
$$ENDIF
$$ENDIF
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$CHILD_FRAME_CLASS$$ construction/destruction

$$CHILD_FRAME_CLASS$$::$$CHILD_FRAME_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: add member initialization code here
	
$$ENDIF
}

$$CHILD_FRAME_CLASS$$::~$$CHILD_FRAME_CLASS$$()
{
}
$$IF(SPLITTER_MDI || PROJTYPE_EXPLORER)

BOOL $$CHILD_FRAME_CLASS$$::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
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
	return m_wndSplitter.Create( this,
$$IF(VERBOSE)
		2, 2,                 // TODO: adjust the number of rows, columns
		CSize( 10, 10 ),      // TODO: adjust the minimum pane size
$$ELSE //!VERBOSE
		2, 2,
		CSize( 10, 10 ),
$$ENDIF //VERBOSE
		pContext );
$$ENDIF //PROJTYPE_EXPLORER
}
$$ENDIF //SPLITTER || EXPLORER

BOOL $$CHILD_FRAME_CLASS$$::PreCreateWindow(CREATESTRUCT& cs)
{
$$IF(VERBOSE)
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

$$ENDIF //VERBOSE
	if( !$$CHILD_FRAME_BASE_CLASS$$::PreCreateWindow(cs) )
		return FALSE;

$$IF(CHILD_FRAME_STYLES)
	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE$$CHILD_FRAME_STYLE_FLAGS$$;

$$ENDIF //CHILD_FRAME_STYLES
$$IF(NODOCVIEW)
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

$$ENDIF
	return TRUE;
}

$$IF(CHILD_FRAME_MAXIMIZED || CHILD_FRAME_MINIMIZED)
void $$CHILD_FRAME_CLASS$$::ActivateFrame(int nCmdShow)
{
$$IF(VERBOSE)
	// TODO: Modify this function to change how the frame is activated.

$$ENDIF //VERBOSE
$$IF(CHILD_FRAME_MAXIMIZED)
	nCmdShow = SW_SHOWMAXIMIZED;
$$ENDIF //CHILD_FRAME_MAXIMIZED
$$IF(CHILD_FRAME_MINIMIZED)
	nCmdShow = SW_SHOWMINIMIZED;
$$ENDIF //CHILD_FRAME_MINIMIZED
	$$CHILD_FRAME_BASE_CLASS$$::ActivateFrame(nCmdShow);
}
$$ENDIF //CHILD_FRAME_MAXIMIZED || CHILD_FRAME_MINIMIZED


/////////////////////////////////////////////////////////////////////////////
// $$CHILD_FRAME_CLASS$$ diagnostics

#ifdef _DEBUG
void $$CHILD_FRAME_CLASS$$::AssertValid() const
{
	$$CHILD_FRAME_BASE_CLASS$$::AssertValid();
}

void $$CHILD_FRAME_CLASS$$::Dump(CDumpContext& dc) const
{
	$$CHILD_FRAME_BASE_CLASS$$::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// $$CHILD_FRAME_CLASS$$ message handlers
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
$$VIEW_CLASS$$* $$CHILD_FRAME_CLASS$$::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	$$VIEW_CLASS$$* pView = DYNAMIC_DOWNCAST($$VIEW_CLASS$$, pWnd);
	return pView;
}

void $$CHILD_FRAME_CLASS$$::OnUpdateViewStyles(CCmdUI* pCmdUI)
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


void $$CHILD_FRAME_CLASS$$::OnViewStyle(UINT nCommandID)
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
$$IF(NODOCVIEW)
void $$CHILD_FRAME_CLASS$$::OnFileClose() 
{
$$IF(VERBOSE)
	// To close the frame, just send a WM_CLOSE, which is the equivalent
	// choosing close from the system menu.
$$ENDIF

	SendMessage(WM_CLOSE);
}

int $$CHILD_FRAME_CLASS$$::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, 
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	return 0;
}

void $$CHILD_FRAME_CLASS$$::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);

	m_wndView.SetFocus();
}

BOOL $$CHILD_FRAME_CLASS$$::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	
	// otherwise, do default handling
	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

$$ENDIF
