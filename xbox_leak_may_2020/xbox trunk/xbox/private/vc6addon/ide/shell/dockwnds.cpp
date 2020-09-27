///////////////////////////////////////////////////////////////////////////////
//	DOCKWNDS.CPP
//		This files contains the implementations for the two window classes
//		associated with the window docking architecture (CPaletteWnd, and
//		CDockWnd).
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "shell.h"
#include "about.h"
#include "dockwnds.h"
#include "dockman.h"
#include "bardockx.h"
#include "util.h"
#include "resource.h"

IMPLEMENT_SERIAL(CDockWnd, CWnd, 1)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static BOOL MDIKeyMessage(MSG* pMsg);

extern UINT WM_ISERROR;

// In version 4.x, INSERT_TOLERANCE was also 0. This stops one inserting rows in the middle
// of two dock rows. The original comment by insert tolerance was:

	// Must stay 0 for restoring to work.

// Here's an extract from some email from Brendan on this subject, when I
// asked about raising INSERT_TOLERANCE:
/* 
From: 	Martyn Lovell
To: 	Brendan X. MacLean

You will probably remember that there is a #defined value, INSERT_TOLERANCE, that controls when new rows are inserted. We're getting a lot of complaints that it's hard to insert a bar between two others; and indeed it is, because INSERT_TOLERANCE is 0. A comment in the code says:

#define INSERT_TOLERANCE 0	// Must stay 0 for restoring to work.

Restoring what? Could I perhaps force this value to 0 when restoring, but have it non-zero during normal interaction?

From: 	Brendan X. MacLean
To: 	Martyn Lovell

Take a look at how we restore windows to their correct rows after being hidden.  Each row is assigned an ID at creation time.  The ID is a long.  I don't exactly remember, but let's suppose the first row gets ID 0.  Then if I create a row above it, that row gets ID -1 and one below it gets ID 1.  We continue incrementing/decrementing ad infinitum, knowing that it would take an avid tester several lifetimes to blow out of a long.

When you hide a window, it remembers its row ID, and a relative position in that row.  When you show the window, if the row still exists, it will insert itself among the windows currently in the row (very important when hiding and showing the watch window in the default layout, since it shares a row with the variables window).  This aspect of rows will continue to work, even if you allow inserts.

However, what will break with the current algorithm is persitent row ordering.  Set-up 3 rows in a dock with one window in each.  Hide any window, and the show it again, and it will reappear in the same relative position.  The only way that you can absolutely guarantee this behavior is the current algorithm:  only add rows to the ends, so that comparison of row IDs can be used to determine relative position.

If people really, really want to have row inserts, which I didn't think were all that great without visual feedback that this was going to happen, here is how I would begin thinking about changing the algorithm:

(1) Raise the increment/decrement on the ends to 10 (or so).
(2) When inserting a row give it the average ID of its surrounding neighbors.

Problems:
This is way too simple, since it allows hidden rows to have their IDs taken.  Repro: Insert a new row.  Hide the window.  Insert another window in the same position.  Show the hidden window.  BUG:  It appears in the newly created row, not alone in a row of its own.
What happens when the surrounding rows have adjacent IDs?  The larger you make the row increment the less likely this is to ever happen, but you should to to stay above one tester lifetime to exhaust the ID space.

I would consider doing some sort of row usage ref counting thing, make the increment large enough, and live with poor ordering in the 0.1% case.

*/

#define INSERT_TOLERANCE CDockWorker::GetInsertTolerance()



///////////////////////////////////////////////////////////////////////////////
// OnCommandHelp helper

LRESULT CommandHelp(LPARAM lParam)
{
	if (theApp.CurrentPromptContext() != 0)
	{
		theApp.WinHelp(theApp.CurrentPromptContext());
		return TRUE;
	}
	if (lParam)
	{
		theApp.WinHelp(lParam);
		return TRUE;
	}
	else
	{
		// Handle case in which dockable has the focus.  We do not want to
		// catch this in CMainFrame, since that would require special testing
		// for cases in which a dialog was displayed.
		CPartView* pView = (CPartView*) CWnd::FromHandlePermanent(theApp.GetActiveView());
		if (pView && pView->IsKindOf(RUNTIME_CLASS(CPartView)))
		{
			CWnd * pWndFocus = CWnd::GetFocus();
			if ((pWndFocus == pView) || 
				pView->GetParentFrame()->IsChild(pWndFocus))
			{
				CString str;
				if (pView->GetHelpWord(str))
				{
					theApp.HelpOnKeyword(str);
					return TRUE;
				}
				lParam = pView->GetHelpID();
				if (lParam != 0)
				{
					theApp.HelpOnApplication(lParam,HELPTYPE_WINDOW);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//	CPaletteWnd
//		The CPaletteWnd is the implementation for the palettes used in
//		docking for the floating state.  It is derived from CMiniFrameWnd.
//

// FUTURE : Use MFS_SYNCACTIVE
#define PWS_BASE (WS_CAPTION | WS_SYSMENU | WS_POPUP)

typedef RECT BORDERS;

static BORDERS borderPalette = {0,0,0,0};

void InitPaletteBorders()
{
	CRect rectWnd(0, 0, 100, 100);
	CRect rectClient;
	CMiniFrameWnd* pWndMini = new CMiniFrameWnd;

	if (!pWndMini->Create(NULL, NULL, PWS_BASE | MFS_THICKFRAME, rectWnd, NULL))
	{
		delete pWndMini;
		return;
	}

	pWndMini->GetWindowRect(rectWnd);
	pWndMini->GetClientRect(rectClient);
	pWndMini->ClientToScreen(rectClient);
	rectClient.InflateRect(::GetSystemMetrics(SM_CXBORDER),
		::GetSystemMetrics(SM_CYBORDER));

	borderPalette.left = rectClient.left - rectWnd.left;
	borderPalette.top = rectClient.top - rectWnd.top;
	borderPalette.right = rectWnd.right - rectClient.right;
	borderPalette.bottom = rectWnd.bottom - rectClient.bottom;

	pWndMini->DestroyWindow();	// Auto-deleting;
}

//	CPaletteWnd::GetPaletteRect
//		Given a reference to a rectangle, and (optionally) a pointer to a
//		window, expands the given rectangle (or the window rect) to return
//		a rectangle for a palette that could contain the given rectangle,
//		or window.

/*static*/ void CPaletteWnd::GetPaletteRect(CRect& rect, CWnd* pChild /*=NULL*/)
{
    if (pChild != NULL)
    {
        pChild->GetWindowRect(rect);
        ASSERT(!rect.IsRectEmpty());
    }

	rect.left -= borderPalette.left;
	rect.top -= borderPalette.top;
	rect.right += borderPalette.right;
	rect.bottom += borderPalette.bottom;
}

/*static*/ void CPaletteWnd::GetChildRect(CRect& rect)
{
	rect.left += borderPalette.left;
	rect.top += borderPalette.top;
	rect.right -= borderPalette.right;
	rect.bottom -= borderPalette.bottom;
}

//	CPaletteWnd::CPaletteWnd
//		Constructor; initializes a few variables.

CPaletteWnd::CPaletteWnd(CDockManager* pManager)
{
	ASSERT_VALID(pManager);
    m_pManager = pManager;
    m_hwndFocus = NULL;
}

//	CPaletteWnd::~CPaletteWnd
//		Destructor; destroys the window.

CPaletteWnd::~CPaletteWnd()
{
    DestroyWindow();
}

//	CPaletteWnd::Create
//		Two phase construction.  Creates a palette for the given child
//		window.	Note: rect is the rectangle of the child.

BOOL CPaletteWnd::Create(LPCSTR lpClassName, LPCSTR lpTitle, CWnd* pChild,
    CWnd* pParentWnd, CRect rect, UINT nID /*=0*/, BOOL bHasClose)
{
    DWORD dwStyle = PWS_BASE;
	DWORD dwChildStyle = pChild->GetStyle();

    if (dwChildStyle & DWS_STRETCHY)
    	dwStyle |= WS_THICKFRAME;
    else if (dwChildStyle & DWS_8SIZABLE)
    	dwStyle |= MFS_THICKFRAME;
	else
		dwStyle |= MFS_4THICKFRAME;

    if ((pChild->GetStyle() & (DWS_STRETCHY | DWS_SIZABLE | DWS_8SIZABLE)) == 0)
        dwStyle |= MFS_MOVEFRAME;

	dwStyle |= MFS_BLOCKSYSMENU;

    GetPaletteRect(rect);
	if (!CMiniFrameWnd::Create(NULL, lpTitle, dwStyle,
            rect, pParentWnd))
    {
        return FALSE;
    }

	if (theApp.m_bWin4)
	{
		// Chicago may want to show a system menu for this window, so let's
		// make sure it looks consistent with the rest of our app.

		CMenu* pMenu = GetSystemMenu(FALSE);
		if (pMenu != NULL)
		{
			CString str;
		    if ((dwChildStyle & DWS_STRETCHY) == 0)
				pMenu->DeleteMenu(SC_SIZE, MF_BYCOMMAND);
			else
			{
				str.LoadString(IDS_DOCKABLE_SIZE);
				pMenu->ModifyMenu(SC_SIZE, MF_BYCOMMAND | MF_STRING,
					SC_SIZE, str);
			}

			str.LoadString(IDS_DOCKABLE_MOVE);
			pMenu->ModifyMenu(SC_MOVE, MF_BYCOMMAND | MF_STRING,
				SC_MOVE, str);

			str.LoadString(IDS_DOCKABLE_CLOSE);
			pMenu->ModifyMenu(SC_CLOSE, MF_BYCOMMAND | MF_STRING,
				SC_CLOSE, str);

			if(!bHasClose)
			{
				pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
			}

			pMenu->DeleteMenu(SC_MAXIMIZE, MF_BYCOMMAND);
			pMenu->DeleteMenu(SC_MINIMIZE, MF_BYCOMMAND);
			pMenu->DeleteMenu(SC_RESTORE, MF_BYCOMMAND);
		}
	}

    pChild->SetParent(this);

	// FUTURE : Use MFS_SYNCACTIVE
	ActivateWindow(TRUE);	// Show active caption color.

    CRect rectChild;
    GetClientRect(rectChild);
    rectChild.InflateRect(g_mt.cxBorder, g_mt.cyBorder);
    pChild->SetWindowPos(NULL, rectChild.left, rectChild.top,
        rectChild.Width(), rectChild.Height(),
        SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);

	ActivateNoFocus();

    m_pManager->AddPalette(this);

    return TRUE;
}

//	CPaletteWnd::ActivateWindow
//		Highlights or de-highlights the palette's caption.  Used instead of
//		normal activation/deactivation so that all palettes appear active while
//		the main frame is active.

void CPaletteWnd::ActivateWindow(BOOL bActive)
{
	// FUTURE : Use MFS_SYNCACTIVE
	SetWindowLong(m_hWnd, GWL_STYLE, GetStyle() | MFS_SYNCACTIVE);
	SendMessage(WM_NCACTIVATE, bActive);
}

void CPaletteWnd::ActivateNoFocus()
{
	// Make sure we come up on top, but not above a customize dialog,
	// or the splash screen.
	const CWnd* pWnd = &CWnd::wndTop;
#if 0
	if (PwndSplashScreen() != NULL)
	{
		pWnd = PwndSplashScreen();
	}
	else if (CASBar::s_pCustomizer != NULL &&
		CASBar::s_pCustomizer->m_pDialog != NULL)
	{
		pWnd = CASBar::s_pCustomizer->m_pDialog;
	}
#else
	HWND hWnd = m_pManager->m_pFrame->GetSafeHwnd();
	if (hWnd != NULL)
		hWnd = ::GetLastActivePopup(hWnd);
	CWnd* pWndPopup = FromHandle(hWnd);

	if (PwndSplashScreen() != NULL)
	{
		pWnd = PwndSplashScreen();
	}
	else if (pWndPopup != NULL && (pWndPopup->IsKindOf(RUNTIME_CLASS(CDialog)) ||
		pWndPopup->IsKindOf(RUNTIME_CLASS(CTabbedDialog))))
	{
		pWnd = pWndPopup;
	}
#endif

	SetWindowPos(pWnd, 0, 0, 0, 0,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

void CPaletteWnd::SaveFocus()
{
	// Don't save focus, if the palette contains the active view.
	HWND hwndView = theApp.GetActiveView();
	if (hwndView != NULL && ::IsChild(m_hWnd, hwndView))
		m_hwndFocus = NULL;
	else
		m_hwndFocus = ::GetFocus();

	if (m_hwndFocus != NULL && !::IsChild(m_hWnd, m_hwndFocus))
		m_hwndFocus = NULL;
}

void CPaletteWnd::RestoreFocus()
{
	CWnd* pChild;
	if (m_hwndFocus != NULL)
		::SetFocus(m_hwndFocus);
	else if ((pChild = GetTopWindow()) != NULL)
		pChild->SetFocus();
}

IMPLEMENT_DYNAMIC(CPaletteWnd, CMiniFrameWnd)

BEGIN_MESSAGE_MAP(CPaletteWnd, CMiniFrameWnd)
//{{AFX_MSG_MAP(CPaletteWnd)
    ON_WM_NCACTIVATE()
    ON_WM_NCLBUTTONDOWN()
    ON_WM_NCLBUTTONDBLCLK()
    ON_WM_CLOSE()
    ON_WM_SIZE()
	ON_WM_RBUTTONUP()
	ON_WM_NCRBUTTONUP()
	ON_WM_ACTIVATE()
	ON_WM_KEYDOWN()
	ON_WM_SYSCOMMAND()
	ON_WM_MOUSEACTIVATE()
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
    ON_REGISTERED_MESSAGE(DMM_CHILDSETFOCUS, OnRouteMessage)
    ON_REGISTERED_MESSAGE(DMM_CHILDKILLFOCUS, OnRouteMessage)
    ON_REGISTERED_MESSAGE(DMM_NCLBUTTONDOWN, OnRouteMessage)
    ON_REGISTERED_MESSAGE(DMM_NCLBUTTONDBLCLK, OnRouteMessage)
	ON_REGISTERED_MESSAGE(DSM_BARSIZECHANGING, OnBarSizeChanging)
END_MESSAGE_MAP()

//	CPaletteWnd::PreTranslateMessage
//		Forward MDI keys to MDI.

BOOL CPaletteWnd::PreTranslateMessage(MSG* pMsg)
{
	if (MDIKeyMessage(pMsg))
		return TRUE;

	return CMiniFrameWnd::PreTranslateMessage(pMsg);
}

//	CPaletteWnd::OnRouteMessage
//		Used to route DMM_* messages to the worker for the HWND in wParam.

LRESULT CPaletteWnd::OnRouteMessage(WPARAM wParam, LPARAM lParam)
{
    return m_pManager->WorkerMessage(GetCurrentMessage()->message,
        (HWND) wParam, lParam);
}

LRESULT CPaletteWnd::OnCommandHelp(WPARAM, LPARAM lParam)
{
	return CommandHelp(lParam);
}

//	CPaletteWnd::OnCommand
//		Routes all commands except Help to the palette's owner (the frame).

BOOL CPaletteWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return !!GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
}

//	CPaletteWnd::OnNcActivate
//		Override to prevent caption activation\deactivation.  Use
//		ActivateWindow() instead.

BOOL CPaletteWnd::OnNcActivate(BOOL bActive)
{
	// FUTURE : MFC: Use MFS_SYNCACTIVE
	if (GetStyle() & MFS_SYNCACTIVE)
	{
		SetWindowLong(m_hWnd, GWL_STYLE, GetStyle() & ~MFS_SYNCACTIVE);
		return CMiniFrameWnd::OnNcActivate(bActive);
	}

	return TRUE;
}

void CPaletteWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (nState == WA_INACTIVE)
		SaveFocus();

	// Do not restore focus, if we already have it.
	else if (!IsChild(GetFocus()))
	{
		CDockWorker* pDocker = m_pManager->WorkerFromWindow(GetTopWindow());
		if (pDocker != NULL && pDocker->m_dtWnd != dtEdit && pWndOther == NULL)
			m_pManager->m_pFrame->SetActiveWindow();
		else
			RestoreFocus();
	}
}

// ensure that everyone gets a chance to adapt to a change in toolbar size
LRESULT CPaletteWnd::OnBarSizeChanging(WPARAM wParam, LPARAM lParam)
{
	SendMessageToDescendants(DSM_BARSIZECHANGING, wParam, lParam, TRUE, FALSE); 

	return 0;
}

CDockWorker* CPaletteWnd::GetWorker() const
{
	return m_pManager->WorkerFromWindow(GetTopWindow());
}

//	CPaletteWnd::OnClose
//		Defers to dock manager to get rid of the window.

void CPaletteWnd::OnClose()
{
	CDockWorker* pDocker = GetWorker();
	ASSERT_VALID(pDocker);
	pDocker->ShowWindow(FALSE);
}

//	CPaletteWnd::OnNcLButtonDown
//		Route move and size handling to the child for processing by the
//		CDockWorker.

void CPaletteWnd::OnNcLButtonDown(UINT nHitTest, CPoint pt)
{
	extern void SushiCancelModes(HWND hWndRcvr);
	SushiCancelModes(m_hWnd);

	switch (nHitTest)
	{
    case HTBOTTOM:
    case HTTOP:
    case HTLEFT:
	case HTBOTTOMLEFT:
	case HTTOPLEFT:
    case HTRIGHT:
	case HTBOTTOMRIGHT:
	case HTTOPRIGHT:
    case HTCAPTION:
    	//BLOCK:
    	{
    		CWnd* pChild = GetTopWindow();
    		ASSERT(pChild != NULL);

    		if (nHitTest != HTCAPTION && (pChild->GetStyle() & DWS_STRETCHY) != 0)
    			break;

            pChild->SendMessage(WM_NCLBUTTONDOWN, nHitTest,
                MAKELONG(pt.x, pt.y));
        }
        return;
	}

	CMiniFrameWnd::OnNcLButtonDown(nHitTest, pt);
}

//	CPaletteWnd::OnNcLButtonDblClk
//		Double-click in the caption causes toggle between float and dock.
//		Let the child window/CDockWorker handle it.

void CPaletteWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint pt)
{
    if (nHitTest != HTCAPTION)
		CMiniFrameWnd::OnNcLButtonDblClk(nHitTest, pt);
    else
    {
        CWnd* pChild = GetTopWindow();
        ASSERT(pChild != NULL);
        pChild->SendMessage(WM_NCLBUTTONDBLCLK, nHitTest, MAKELONG(pt.x, pt.y));
    }
}

void CPaletteWnd::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
	if (theApp.m_bWin4)
	{
		// Don't show our menu when Win95 will show its menu.
		CWnd* pWnd = GetFocus();
		if ((pWnd == this || IsChild(pWnd)) && nHitTest == HTCAPTION)
		{
			CMiniFrameWnd::OnNcRButtonUp(nHitTest, point);
			return;
		}
	}

	ScreenToClient(&point);
	CPaletteWnd::OnRButtonUp(0, point);
}

void CPaletteWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	CMiniFrameWnd::OnRButtonUp(nFlags, point);

	ClientToScreen(&point);
	m_pManager->ShowMenu(point);
}

void CPaletteWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE)
		theApp.ReactivateView();

	CMiniFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

//	CPaletteWnd::OnSize
//		Palette makes sure the child is sized to a perfect fit inside its
//		client area when it gets sized.

void CPaletteWnd::OnSize(UINT nType, int cx, int cy)
{
    if (nType == SIZE_RESTORED)
    {
        CRect rect;
        CWnd* pChild;

        GetClientRect(rect);
        rect.InflateRect(g_mt.cxBorder, g_mt.cyBorder);
        if ((pChild = GetTopWindow()) != NULL)
        {
            pChild->SetWindowPos(NULL, rect.left, rect.top,
                rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
        }
    }

	CMiniFrameWnd::OnSize(nType, cx, cy);
}

int CPaletteWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest,
	UINT message)
{
	if (theApp.m_hwndValidate != NULL &&
		::SendMessage(theApp.m_hwndValidate, WM_ISERROR, 0, 0L))
	{
		return MA_NOACTIVATEANDEAT;
	}

	int nReturn = MA_NOACTIVATE;
	CDockWorker* pDocker = m_pManager->WorkerFromWindow(GetTopWindow());
	if (pDocker != NULL && pDocker->m_dtWnd == dtEdit &&
			CASBar::s_pCustomizer == NULL)
		nReturn = MA_ACTIVATE;

	if (theApp.m_bDeactByOtherApp)
	{
		pDocker = CDockWorker::s_pActiveDocker;
		if (pDocker != NULL && pDocker->GetDock() == dpNil)
		{
			ASSERT(pDocker->m_pWnd != NULL);
			pDocker->m_pWnd->SetFocus();
		}
		else
		{
			m_pManager->m_pFrame->SetActiveWindow();
		}
	}

	ActivateNoFocus();
	UpdateWindow();

	return nReturn;
}

void CPaletteWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch (nID & 0xfff0)
	{
	case SC_CLOSE:
		if (lParam == 0L)
		{
			// User pressed Alt+F4. Forward so we close the whole App.
			CWnd* pParent = GetParent();
			ASSERT(pParent != NULL);

//	    	pParent->SetActiveWindow();
	        pParent->SendMessage(WM_SYSCOMMAND, nID, lParam);
//	        SetActiveWindow();	Crashes the application.
			return;
		}
		break;

	case SC_KEYMENU:
		if (lParam != 0L)
			CDockWorker::LockWorker();	// CMiniFrameWnd will forward to frame.
		break;
	}

	CMiniFrameWnd::OnSysCommand(nID, lParam);
}

LRESULT CPaletteWnd::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID(m_pManager);

	m_nIDLastMessage = (UINT)wParam;    // new ID (or 0)
	m_nIDTracking = (UINT)wParam;       // so F1 on toolbar buttons work

	return m_pManager->m_pFrame->SendMessage(WM_SETMESSAGESTRING,
		wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CDockWnd

#ifdef _DEBUG
extern BOOL bChicagoLookOnNT;
inline BOOL OnChicago() { return theApp.m_bWin4 || bChicagoLookOnNT; }
#else
inline BOOL OnChicago() { return theApp.m_bWin4; }
#endif

// static variables
//
HCURSOR CDockWnd::s_hcurVSize = NULL;
HCURSOR CDockWnd::s_hcurHSize = NULL;

CDockWnd::CDockWnd()
{
    if (s_hcurHSize == NULL)
    {
        VERIFY(s_hcurHSize = AfxLoadCursor(AFX_IDC_VSPLITBAR));
        VERIFY(s_hcurVSize = AfxLoadCursor(AFX_IDC_HSPLITBAR));
    }

    m_nAlong = 0;
	m_bInitialized = FALSE;
	m_bChicagoSizingHack = FALSE;
}

void CDockWnd::Serialize(CArchive& ar)
{
	int i;

	if (ar.IsStoring())
	{
		ar << (WORD) m_dp << (DWORD) m_nIDNext
			<< (DWORD) m_nIDPrev << (WORD) m_nRows;

		for (i = 0; i <= m_nRows; i++)
		{
			ar << (WORD) m_arRows[i].nStart
				<< (DWORD) m_arRows[i].nIDRow
				<< (WORD) m_arRows[i].bStretchy
				<< (WORD) m_arRows[i].bMaxy
				<< (WORD) m_arRows[i].bMenuBar
				;

			ASSERT(m_arRows[i].bInit);
			if (m_arRows[i].pWndMax == NULL)
				ar << (DWORD) 0;
			else
			{
				CDockWorker* pDocker = m_pManager->
					WorkerFromWindow(m_arRows[i].pWndMax);
				if (pDocker == NULL)
					ar << (DWORD) 0;
				else
				{
					ar << (DWORD) pDocker->m_nIDWnd;
				}
			}
		}
	}
	else
	{
		WORD w;
		DWORD dw;

		ar >> w >> dw;
		m_dp  = (DOCKPOS) w;
		m_nIDNext = (ULONG) dw;
		ar >> dw >> w;
		m_nIDPrev = (ULONG) dw;
		m_nRows = w;

		m_arRows = new ROWSTRUCT[m_nRows + 1];

		for (i = 0; i <= m_nRows; i++)
		{
			ar >> w >> dw;
			m_arRows[i].nStart = (signed short)w;
			m_arRows[i].nIDRow = (ULONG) dw;
			ar >> w;
			m_arRows[i].bStretchy = w;
			ar >> w;
			m_arRows[i].bMaxy = w;
			ar >> w;
			m_arRows[i].bMenuBar = w;
			ar >> dw;
			m_arRows[i].nIDMaxWindow = dw;
			m_arRows[i].bInit = (i == m_nRows);	// Mark sentinal row as initialized.
		    m_arRows[i].nSplitters = 0;
		    m_arRows[i].pnPositions = NULL;
		}
	}
}

//  CDockWnd::CDockWnd
//      Constructor, initializes statics if this is the first CDockWnd.

CDockWnd::CDockWnd(DOCKPOS dp)
{
    if (s_hcurHSize == NULL)
    {
        VERIFY(s_hcurHSize = AfxLoadCursor(AFX_IDC_VSPLITBAR));
        VERIFY(s_hcurVSize = AfxLoadCursor(AFX_IDC_HSPLITBAR));
    }

    m_dp = dp;
    m_nAlong = 0;
	m_bInitialized = FALSE;
	m_bChicagoSizingHack = FALSE;
	m_nIDNext = ULONG_MAX / 2;
	m_nIDPrev = m_nIDNext - 1;
    m_nRows = 0;

    m_arRows = new ROWSTRUCT[1];
	m_arRows[0].nStart = -g_mt.cyBorder;
	m_arRows[0].nIDRow = m_nIDNext++;
    m_arRows[0].bStretchy = FALSE;
    m_arRows[0].bMenuBar = FALSE;
    m_arRows[0].bMaxy = FALSE;
	m_arRows[0].bInit = TRUE;
    m_arRows[0].pWndMax = NULL;
    m_arRows[0].nSplitters = 0;
    m_arRows[0].pnPositions = NULL;
}

//	CDockWnd::~CDockWnd
//		Destructor; destroys window.

CDockWnd::~CDockWnd()
{
	for (int i = 0; i < m_nRows; i++)
		delete [] m_arRows[i].pnPositions;

	delete [] m_arRows;

    DestroyWindow();
}

BOOL CDockWnd::Create(CFrameWnd* pFrame, UINT nID, CDockManager* pManager)
{
    m_pManager = pManager;

    LONG dwStyle = WS_CHILD | WS_CLIPSIBLINGS;
	if (!OnChicago())
		dwStyle |= WS_BORDER | WS_CLIPCHILDREN;
    if (m_dp != dpHide && pManager->IsVisible())
    	dwStyle |= WS_VISIBLE;

	CSize size(::GetSystemMetrics(SM_CXSCREEN) + 5,
		::GetSystemMetrics(SM_CYSCREEN) + 5);	// +5 to assure that we get recalc'ed.
	DockTranspose(m_dp, &size);
	m_nAlong = size.cx;
	size.cy = 0;
	DockTranspose(m_dp, &size);

    CRect rect(CPoint(-1, -1), size);	// Sized during OnSizeParent.
//    rect.SetRectEmpty();	// Sized during OnSizeParent.
    return CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS,
    						NULL,
    						OnChicago() ? NULL : (HBRUSH)(COLOR_BTNFACE + 1)),
		NULL, dwStyle, rect, pFrame, nID);
}

///////////////////////////////////////////////////////////////////////////////
//	Layout
//

//	CDockWnd::GetRowInfo
//		Creates a null terminated array of _DWI (Dock Window Info) structs
//		containing info for all the windows in a row, or in the whole dock
//		if the row is -1.

_DWI* CDockWnd::GetRowInfo(int iRow /*=-1*/)
{
    // Allocate the structure.
    int cWnds = 0;
    for (CWnd* pWnd = GetTopWindow(); pWnd != NULL;
    	pWnd = pWnd->GetWindow(GW_HWNDNEXT))
        cWnds++;	// Enough space for all windows.

    _DWI* rgdwi = new _DWI[cWnds + 1];  // pWnd == NULL terminated

    // Fill the structure.
    cWnds = 0;
    for (pWnd = GetTopWindow(); pWnd != NULL;
    	pWnd = pWnd->GetWindow(GW_HWNDNEXT))
    {
        GetDockInfo(&rgdwi[cWnds], pWnd);

        if (iRow == -1 || iRow == rgdwi[cWnds].iRow)
        	cWnds++;
    }

    rgdwi[cWnds].pWnd = NULL;

    if (iRow != -1)
    	SortRow(rgdwi, iRow);

    return rgdwi;
}

//  CDockWnd::BeginLayout
//      Returns a pointer to layout structure for defered layout.  This
//      allows for multiple repositionings of windows before we actually
//      need to tell Windows were to put the window.

_DWI* CDockWnd::BeginLayout()
{
    return GetRowInfo();
}

static void ValidateWndDeep(CWnd* pWnd)
{
	pWnd->ValidateRect(NULL);

	// Deep validation.
	for (CWnd* pWndChild = pWnd->GetTopWindow(); pWndChild != NULL;
			pWndChild = pWndChild->GetWindow(GW_HWNDNEXT))
		ValidateWndDeep(pWndChild);
}

// This hacky UpdateWindow() replacement, fixes a problem we had with
// async painting under Chicago.

static void ChicagoUpdateWindowHack(CWnd* pWnd)
{
	// Deep update, since hack in async paint code may cause childred to
	// be skipped.

	for (CWnd* pWndChild = pWnd->GetTopWindow(); pWndChild != NULL;
			pWndChild = pWndChild->GetWindow(GW_HWNDNEXT))
		ChicagoUpdateWindowHack(pWndChild);

	pWnd->UpdateWindow();
}

//  CDockWnd::EndLayout
//      Actually reposition the windows that have been moved in the layout
//      structure.  And deallocate the layout structure.

void CDockWnd::EndLayout(_DWI* rgdwi, BOOL bRecalc /*=TRUE*/)
{
    ASSERT(rgdwi != NULL);

	// Get the style before we hide the window.
    DWORD dwStyle = GetStyle();

    int nWnds = 0;
    for (_DWI* pdwi = rgdwi; pdwi->pWnd != NULL; pdwi++)
    	nWnds++;

    CRect rectWnd;
	
	if (OnChicago() /*&& m_bChicagoSizingHack*/)
		ChicagoUpdateWindowHack(this);
	else
		UpdateWindow();

    ShowWindow(SW_HIDE);

    HDWP hdwp = ::BeginDeferWindowPos(nWnds);
    for (pdwi = rgdwi; pdwi->pWnd != NULL; pdwi++)
    {
        CRect rectNew(CPoint(pdwi->iPos, GetWindowOffset(pdwi->iRow)),
        	CSize(pdwi->cAlong, pdwi->cAcross));
        DockTranspose(m_dp, rectNew);

        pdwi->pWnd->GetWindowRect(rectWnd);
        pdwi->iPos = rectWnd.left;	// Save window coords.
        pdwi->iRow = rectWnd.top;
        pdwi->cAlong = rectWnd.Width();
        pdwi->cAcross = rectWnd.Height();
        ScreenToClient(rectWnd);

        // Only move it if position/size changed, or it needs to be shown.
        if (rectNew != rectWnd)
        {
            hdwp = ::DeferWindowPos(hdwp,
            	pdwi->pWnd->GetSafeHwnd(), NULL,
                rectNew.left, rectNew.top,
                rectNew.Width(), rectNew.Height(),
                SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }
    }
	::EndDeferWindowPos(hdwp);

    if ((dwStyle & WS_VISIBLE) != 0)
	{
		ShowWindow(SW_SHOWNOACTIVATE);

	    // The DockWnd itself may need to be sized.
	    if (bRecalc)
	    	GetParentFrame()->RecalcLayout();

	    for (pdwi = rgdwi; pdwi->pWnd != NULL; pdwi++)
	    {
	        CRect rectNew(CPoint(pdwi->iPos, pdwi->iRow),
	        	CSize(pdwi->cAlong, pdwi->cAcross));
	    	pdwi->pWnd->GetWindowRect(rectWnd);

	    	// Validate windows that haven't moved.
	    	if (rectNew == rectWnd)
		    	ValidateWndDeep(pdwi->pWnd);
	    }

		UpdateWindow();
	}

    delete [] rgdwi;
}

//  CDockWnd::FindDockInfo

_DWI* CDockWnd::FindDockInfo(_DWI* pdwi, CWnd* pWndFind)
{
    ASSERT(pdwi != NULL);

    for ( ; pdwi->pWnd != pWndFind; pdwi++)
        ASSERT(pdwi->pWnd);   // window is not part of layout

    return pdwi;
}

//  CDockWnd::GetDockInfo
//      Given a layout struct pointer, and a window, fills the struct with
//      the windows dock info.

void CDockWnd::GetDockInfo(_DWI* pdwi, CWnd* pWnd)
{
    ASSERT(pdwi != NULL);
    ASSERT_VALID(pWnd);

    CRect rectPane;
	pWnd->GetWindowRect(rectPane);
	ScreenToClient(rectPane);
    DockTranspose(m_dp, rectPane);

	pdwi->pWnd = pWnd;
    pdwi->iRow = rectPane.top;
    pdwi->iPos = rectPane.left;
    pdwi->cAlong = rectPane.Width();
    pdwi->cAcross = rectPane.Height();

    // Convert to canonical row.
    for (int iRow = 0; GetWindowOffset(iRow) != pdwi->iRow && iRow<m_nRows; iRow++)
	{
		// ASSERT(iRow < m_nRows);    // it is not in a row
	}

	if(iRow==m_nRows)
	{
		--iRow;
	}

    pdwi->iRow = iRow;
}

int CDockWnd::GetWindowOffset(int iRow)
{
    CSize sizeBorder(g_mt.cxBorder, g_mt.cyBorder);
    DockTranspose(m_dp, &sizeBorder);

    BOOL bSplitAbove = (m_dp == dpRight || m_dp == dpBottom);
    int nOffset;

	nOffset = m_arRows[iRow].nStart;
	if (m_arRows[iRow].bStretchy && bSplitAbove)
		nOffset += g_mt.nSplitter + sizeBorder.cy;

	return nOffset;
}

void CDockWnd::GetDockRestore(DOCKRESTORE* pdr, CWnd* pWnd)
{
	ASSERT(IsChild(pWnd));

	_DWI dwi;
	GetDockInfo(&dwi, pWnd);
	pdr->iPos = dwi.iPos;
	pdr->cAlong = dwi.cAlong;
	pdr->cAcross = dwi.cAcross;
	pdr->nIDRow = m_arRows[dwi.iRow].nIDRow;
}

//  CDockWnd::AddWnd
//      Give a window pointer and a size, adds the window to the dock at the
//      next available location, sizing it to the given size.

BOOL CDockWnd::AddWnd(CWnd* pWnd, CSize size)
{
    CRect rect;
    int iMaxPos = 0;

    // Determine the maximum extent of all windows in the last dock row.
    for (CWnd* pWndPane = GetTopWindow(); pWndPane != NULL;
        pWndPane = pWndPane->GetWindow(GW_HWNDNEXT))
    {
        _DWI dwi;
        GetDockInfo(&dwi, pWndPane);

        if (dwi.iRow == m_nRows - 1)
        {
            pWndPane->GetWindowRect(rect);
            DockTranspose(m_dp, rect);
            iMaxPos = max(iMaxPos, dwi.iPos + rect.Width());
        }
    }

    CRect rectAdd(-g_mt.cxBorder, -g_mt.cyBorder,
    	size.cx - g_mt.cxBorder, size.cy - g_mt.cyBorder);
    DockTranspose(m_dp, rectAdd);

    GetClientRect(rect);
    DockTranspose(m_dp, rect);

    CSize sizeBorder(g_mt.cxBorder, g_mt.cyBorder);
    DockTranspose(m_dp, &sizeBorder);

    // Determine new x and y.
    if (m_nRows == 0 ||
        (iMaxPos + size.cx - (sizeBorder.cx * 2)) > rect.right)
    {
        rectAdd.OffsetRect(0, GetWindowOffset(m_nRows));
    }
    else
    {
    	// center in last row.
        rectAdd.OffsetRect(iMaxPos,
        	(m_arRows[m_nRows].nStart + m_arRows[m_nRows - 1].nStart) / 2 -
        	(rectAdd.bottom + rectAdd.top) / 2);
    }

    DockTranspose(m_dp, rectAdd);
	ClientToScreen(rectAdd);
	return AddWnd(pWnd, rectAdd);
}

//  CDockWnd::AddWnd
//      Given a window pointer, and a restore struct, attempts to add the
//		window to the dock as consistent as possible with the information
//		in the restore struct, following layout rules.

BOOL CDockWnd::AddWnd(CWnd* pWnd, DOCKRESTORE* pdr)
{
	// Find the row in which the window will be placed.
	int iRowLeastBig = -1;
	for (int iRow = 0; iRow < m_nRows; iRow++)
	{
		if (m_arRows[iRow].nIDRow == pdr->nIDRow)
			break;
		// If INSERT_TOLERANCE is 0 then rows will only be inserted at the
		// top or bottom of the dock, so we are assured that IDs are in
		// sorted order.  This allows us to insert rows in the middle of an
		// existing dock, maintaining row ordering.
		else if (INSERT_TOLERANCE == 0 && m_arRows[iRow].nIDRow > pdr->nIDRow)
		{
			// Because of row wrapping we may have a few stray out of
			// order row IDs, so we attempt to ignore them here.
			//
			if (iRowLeastBig == -1 ||
				m_arRows[iRow].nIDRow < m_arRows[iRowLeastBig].nIDRow)
				iRowLeastBig = iRow;
		}
	}

	if (iRow >= m_nRows && iRowLeastBig != -1)
		iRow = iRowLeastBig;

	// If we are a stretchy window being added to an existing row, we
	// may need to adjust the size the window is requesting, or it could
	// reduce all other stretchy windows to min width.
	//
	if ((pWnd->GetStyle() & DWS_STRETCHY) != 0 &&
		iRow < m_nRows && m_arRows[iRow].nIDRow == pdr->nIDRow)
	{
	    CRect rectDock;
	    GetClientRect(rectDock);
	    CSize sizeBorder(g_mt.cxBorder, g_mt.cyBorder);
	    rectDock.InflateRect(sizeBorder);
	    DockTranspose(m_dp, rectDock);
	    DockTranspose(m_dp, &sizeBorder);

		CPoint ptMin = CDockWorker::GetDefMin();
		DockTranspose(m_dp, &ptMin);

		int nSplitterCount = 1, iWnd = 0;
		int nTotalSize = ptMin.x, nStretchCount = 1;	// Count the window.
		int nLastStretch = -1;

		// Calculate how far off we are.
		_DWI* rgdwi = GetRowInfo(iRow);
		for (_DWI* pdwi = rgdwi; pdwi->pWnd != NULL && pdwi->iRow == iRow;
			pdwi++, iWnd++)
		{
		    if ((pdwi->pWnd->GetStyle() & DWS_STRETCHY) == 0)
		    	nTotalSize += pdwi->cAlong - sizeBorder.cx;
			else
		    {
		    	if (nLastStretch != -1)
		    	{
		    		if (nLastStretch == iWnd - 1)
		    			nSplitterCount++;
		    		else
		    			nSplitterCount += 2;
		    	}

				nStretchCount++;
		    	nTotalSize += ptMin.x - sizeBorder.cx;
	    		nLastStretch = iWnd;
		    }
		}
		delete [] rgdwi;

		if (nTotalSize > 0)
			nTotalSize += sizeBorder.cx + nSplitterCount * (g_mt.nSplitter + 1);

		// If the window wants more than the available stretch space,
		// then it only gets a part.  Yes this is arbitrary, but it is
		// better than having other stretchy windows minimized.
		//
		int nMaxFit = rectDock.Width() - nTotalSize;
		if (pdr->cAlong >= nMaxFit)
			pdr->cAlong = max(ptMin.x, nMaxFit / nStretchCount);
	}

	// Construct the rect we will use to add the window.
    CRect rectAdd;

	if (iRow >= m_nRows)
		rectAdd.top = m_arRows[m_nRows].nStart;
	else
	{
		// Center rect on the row of interest.
		rectAdd.top = (m_arRows[iRow + 1].nStart + m_arRows[iRow].nStart) / 2 -
    		pdr->cAcross / 2;
	}

	rectAdd.left = pdr->iPos;
	rectAdd.bottom = rectAdd.top + pdr->cAcross;
	rectAdd.right = rectAdd.left + pdr->cAlong;

    DockTranspose(m_dp, rectAdd);
	ClientToScreen(rectAdd);

	if (iRow < m_nRows && m_arRows[iRow].nIDRow == pdr->nIDRow)
		return AddWnd(pWnd, rectAdd);
	else
		return AddWnd(pWnd, rectAdd, pdr->nIDRow);	// Force an insert of this row.
}

//  CDockWnd::AddWnd
//      Given a window pointer, and a rect, attempts to add the window to
//      the dock as close as possible to the given rectangle, following
//		layout rules.

BOOL CDockWnd::AddWnd(CWnd* pWnd, const CRect& rect, ULONG nID /*=ULONG_MAX*/)
{
    pWnd->ShowWindow(SW_HIDE);    // Hide the window for SetParent, and SetPos.

    if (pWnd->SetParent(this) == NULL)
    {
    	pWnd->ShowWindow(SW_SHOWNOACTIVATE);
    	return FALSE;
    }

	CRect rectPlace = rect;

	// Add a row, if we don't have any.
	if (m_nRows == 0)
	{
		CSize size(rect.Size());
		DockTranspose(m_dp, &size);

		_DWI dwi;
		dwi.pWnd = NULL;	// Empty DWI structure.
		InsertRow(&dwi, 0, size.cy, nID);

		// Force the placement rect into the new row.
		CPoint ptPlace(0, 0);
		ClientToScreen(&ptPlace);
		if (m_dp == dpLeft || m_dp == dpRight)
			rectPlace.OffsetRect(ptPlace.x - rectPlace.left, 0);
		else
			rectPlace.OffsetRect(0, ptPlace.y - rectPlace.top);
	}

    // Set position to be on a row, so we can just use MoveWnd.
    // And size the window.
	CPoint ptAdd(-2, GetWindowOffset(0));	// -2 forces the window to be moved.
	DockTranspose(m_dp, &ptAdd);
    pWnd->SetWindowPos(NULL,
        ptAdd.x, ptAdd.y, rectPlace.Width(), rectPlace.Height(),
        SWP_NOACTIVATE | SWP_NOZORDER);

	MoveWnd(pWnd, rectPlace, TRUE, nID);

	ASSERT(pWnd->GetStyle() & WS_VISIBLE);	// EndLayout should have shown.

    return TRUE;
}

//  CDockWnd::MoveWnd
//      Given a window pointer, and a rect, moves the window from its current
//      location in the dock to the new specified location (layout rules
//      applied to the rect).

void CDockWnd::MoveWnd(CWnd* pWnd, CRect rect,
	BOOL bAdd /*=FALSE*/, ULONG nID /*=ULONG_MAX*/)
{
	// We used to assert here, to ensure that were were correctly parented to the dock. Sadly, at the 
	// moment, this can happen in 'valid' situations during layout switches. So we
	// simply return
    // ASSERT(IsChild(pWnd));
	if(!IsChild(pWnd))
	{
		return;
	}

    _DWI dwiMove;

    BOOL bInsert =IsInsert(rect, &dwiMove.iRow, (pWnd->GetStyle() & DWS_MENUBAR)!=0);

	BOOL bStretchTest = OnChicago() && (m_dp == dpRight || m_dp == dpBottom);
	BOOL bStretchy;

    ScreenToClient(rect);
    DockTranspose(m_dp, rect);

    dwiMove.pWnd = pWnd;
    dwiMove.iPos = rect.left;
    dwiMove.cAlong = rect.Width();
    dwiMove.cAcross = rect.Height();

    _DWI* pdwi = BeginLayout();

    if (bInsert || (nID != ULONG_MAX && nID != m_arRows[dwiMove.iRow].nIDRow))
        InsertRow(pdwi, dwiMove.iRow, dwiMove.cAcross, nID);

    _DWI* pdwiFind = FindDockInfo(pdwi, pWnd);
    int iRowOld = pdwiFind->iRow;  // Save the old row.
    *pdwiFind = dwiMove;    // Reset the moved window dock info.

    if (!bAdd && dwiMove.iRow != iRowOld)
    {
		bStretchy = m_arRows[iRowOld].bStretchy;	// Stretchy before?

	    if (!IsRowEmpty(pdwi, iRowOld))
	    {
	    	// In case we removed the largest docked window.
	    	RecalcRow(pdwi, iRowOld);
	    }
	    else
	    {
	        RemoveRow(pdwi, iRowOld);

	        // Old row changes if it was after the one we just removed.
	        if (dwiMove.iRow > iRowOld)
	            dwiMove.iRow--;
	    }

		// Did this row's stretchy status change?
		// Note: we have a sentinel non-stretchy row at m_nRows, so this is
		//       is always a valid test.
		if (bStretchTest && iRowOld > 0 &&
			bStretchy != m_arRows[iRowOld].bStretchy)
		{
			SizeRow(pdwi, iRowOld - 1);
		}
	}

	bStretchy = m_arRows[dwiMove.iRow].bStretchy;	// Stretchy before?

    RecalcRow(pdwi, dwiMove.iRow, pWnd);    // This might wrap and add a row.

	// Did this row's stretchy status change?
	if (bStretchTest && dwiMove.iRow > 0 &&
		bStretchy != m_arRows[dwiMove.iRow].bStretchy)
	{
		SizeRow(pdwi, dwiMove.iRow - 1);
	}

    EndLayout(pdwi);
}

BOOL CDockWnd::IsInsert(CRect rect, int* piRow, BOOL bMenu)
{
    ScreenToClient(rect);
    DockTranspose(m_dp, rect);

    int iCompare = rect.top + rect.Height() / 2;

    // Which row are we in?
    for (int iRow = 0; iRow < m_nRows &&
    	m_arRows[iRow + 1].nStart < iCompare; iRow++)
        ;

    BOOL bInsert = FALSE;

	if(m_arRows[iRow].bMenuBar || (bMenu && (iRow!=0 && iRow!=m_nRows-1)))
	{
		// can't drop into same row as menu bar, and menus always insert
		bInsert=TRUE;

		int nBarRowHeight=m_arRows[iRow + 1].nStart-m_arRows[iRow].nStart;
		if(iCompare > m_arRows[iRow].nStart+(nBarRowHeight/2) && iRow<m_nRows)
		{
			iRow++;
		}
	}
	else
	{
		if (iRow >= m_nRows || iCompare < m_arRows[iRow].nStart + INSERT_TOLERANCE)
			bInsert = TRUE;
		else if (iCompare + INSERT_TOLERANCE > m_arRows[iRow + 1].nStart)
		{
			bInsert = TRUE;
			iRow++;
		}
	}

	if (piRow != NULL)
		*piRow = iRow;

	return bInsert;
}

//  CDockWnd::RemoveWnd
//      Given a pointer to a docked window, and a pointer to new parent
//      window, removes the window from the current dock layout, and sets
//      the parent to the new window.

void CDockWnd::RemoveWnd(CWnd* pWnd, CWnd* pNewParent)
{
    ASSERT(IsChild(pWnd));

    _DWI dwi;
    GetDockInfo(&dwi, pWnd);

	if (m_arRows[dwi.iRow].pWndMax == pWnd)
		m_arRows[dwi.iRow].pWndMax = NULL;	// Removing the maximized window.

    pWnd->SetParent(pNewParent);

    _DWI* pdwi = BeginLayout();

	BOOL bStretchTest = OnChicago() && dwi.iRow > 0 &&
		(m_dp == dpRight || m_dp == dpBottom);
	BOOL bStretchy = m_arRows[dwi.iRow].bStretchy;	// Stretchy before?

    if (IsRowEmpty(pdwi, dwi.iRow))
        RemoveRow(pdwi, dwi.iRow);
    else
        RecalcRow(pdwi, dwi.iRow);

	// Did this row's stretchy status change?
	// Note: we have a sentinal non-stretchy row at m_nRows, so this is
	//       is always a valid test.
	if (bStretchTest && bStretchy != m_arRows[dwi.iRow].bStretchy)
		SizeRow(pdwi, dwi.iRow - 1);

    EndLayout(pdwi);
}

//	CDockWnd::GetMaxState

MAX_STATE CDockWnd::GetMaxState(CWnd* pWnd)
{
	_DWI dwi;
	GetDockInfo(&dwi, pWnd);
	if (!m_arRows[dwi.iRow].bMaxy)
		return stateDisabled;
	else if (m_arRows[dwi.iRow].pWndMax == pWnd)
		return stateRestore;
	else
		return stateMaximize;
}

//	CDockWnd::MaximizeRestore

void CDockWnd::MaximizeRestore(CWnd* pWnd)
{
	_DWI dwi;
	GetDockInfo(&dwi, pWnd);
	if (!m_arRows[dwi.iRow].bMaxy)
		return;

	_DWI* rgdwi = BeginLayout();
	SortRow(rgdwi, dwi.iRow);
	
	// If the window is not currently maximized, then maximize it now.
	// Otherwise, restore it.

	if (m_arRows[dwi.iRow].pWndMax != pWnd)
	{
		m_arRows[dwi.iRow].pWndMax = pWnd;
	}
	else
	{
		m_arRows[dwi.iRow].pWndMax = NULL;

		// Make working variables generic to horizontal and vertical dock.
		CRect rectDock;
		GetClientRect(rectDock);
		DockTranspose(m_dp, rectDock);

		// Make all docking windows the same size, and let recalc row
		// resize them to fit.

		_DWI* pdwi;
		for (pdwi = rgdwi; pdwi->pWnd != NULL && pdwi->iRow == dwi.iRow; pdwi++)
		{
			if (pdwi->pWnd->GetStyle() & DWS_STRETCHY)
				pdwi->cAlong = rectDock.Width();
		}
	}

	RecalcRow(rgdwi, dwi.iRow);

	EndLayout(rgdwi);
}

//	CDockWnd::IsRowEmpty
//		Give a layout structure, and row, returns true, if a DWI exists in
//		the array with the given row.

BOOL CDockWnd::IsRowEmpty(_DWI* pdwi, int iRow)
{
    while (pdwi->pWnd != NULL && pdwi->iRow != iRow)
        pdwi++;

    return pdwi->pWnd == NULL;
}

//  CDockWnd::RemoveRow
//      Given a dock info pointer (assumed to point to a valid info array),
//      updates the row info for windows in the array, and updates the DockWnd
//      row member variables to reflect the row removal.  Note: No reallocation
//		here, only in InsertRow.

void CDockWnd::RemoveRow(_DWI* pdwi, int iRow)
{
    ASSERT(pdwi != NULL);

    // All windows in rows after the one removed, get moved back a row.
    for ( ; pdwi->pWnd != NULL; pdwi++)
    {
        ASSERT(pdwi->iRow != iRow); // Never remove a non-empty row.

        if (pdwi->iRow > iRow)
            pdwi->iRow--;
    }

    delete [] m_arRows[iRow].pnPositions;

    // Subtract the size of the row removed from all rows after that removed.
    int iSize = m_arRows[iRow + 1].nStart - m_arRows[iRow].nStart;
    for (int i = iRow; i < m_nRows; i++)
    {
        m_arRows[i] = m_arRows[i + 1];
        m_arRows[i].nStart -= iSize;
    }

    m_nRows--;  // One less row.
}

//  CDockWnd::InsertRow
//      Given a dock info pointer (assumed to point to a valid info array),
//      the row to insert before, and its size, updates affected windows, and
//      rows in row member variables.

void CDockWnd::InsertRow(_DWI* pdwi, int iRow, int iSize,
	ULONG nID /*=ULONG_MAX*/)
{
    ASSERT(pdwi != NULL);

    // Make sure we have the space; allocate and copy to new array.
    ROWSTRUCT* prRows = new ROWSTRUCT[m_nRows + 2];
    memcpy(prRows, m_arRows, (m_nRows + 1) * sizeof(ROWSTRUCT));
    delete [] m_arRows;
    m_arRows = prRows;

    // Increment row for windows in this or following row.
    for ( ; pdwi->pWnd != NULL; pdwi++)
    {
        if (pdwi->iRow >= iRow)
            pdwi->iRow++;
    }

    // Add row size to this and following rows.
    CSize sizeBorder(g_mt.cxBorder, g_mt.cyBorder);
    DockTranspose(m_dp, &sizeBorder);
    for (int i = m_nRows; i >= iRow; i--)
    {
    	m_arRows[i + 1] = m_arRows[i];
        m_arRows[i + 1].nStart += iSize;
        if (!OnChicago())
        	 m_arRows[i + 1].nStart -= g_mt.cyBorder;	// Overlap
    }

	if (nID != ULONG_MAX)
	{
		ASSERT(m_nIDPrev < nID && nID < m_nIDNext);	// Corrupt layout info.
		m_nIDPrev = min(m_nIDPrev, nID - 1);			// Attempt to repair any corruptions.
		m_nIDNext = max(m_nIDNext, nID + 1);
		ASSERT(m_nIDPrev > 100 && ULONG_MAX - 100 > m_nIDNext);	// We are in trouble.
	}
//	Invalid assert if we are wrapping rows.
//	ASSERT(INSERT_TOLERANCE != 0 || nID != ULONG_MAX ||
//		iRow == 0 || iRow == m_nRows);

	if (nID != ULONG_MAX)
		m_arRows[iRow].nIDRow = nID;
	else if (iRow == 0)
		m_arRows[iRow].nIDRow = m_nIDPrev--;
	else
		m_arRows[iRow].nIDRow = m_nIDNext++;

    m_arRows[iRow].bStretchy = FALSE;
    m_arRows[iRow].bMenuBar = FALSE;
	m_arRows[iRow].bMaxy = FALSE;
	m_arRows[iRow].bInit = TRUE;
	m_arRows[iRow].pWndMax = NULL;
    m_arRows[iRow].nSplitters = 0;
    m_arRows[iRow].pnPositions = NULL;
    m_nRows++;  // New row.
}

//  CDockWnd::SizeRow
//      Given a dock info pointer (assumed to point to a valid info array),
//      a row, and (optionally a size), resizes the row to the given size,
//      or to the size of the largest window, if no size is 0, by incrementing
//      following rows by the old - new size difference.

void CDockWnd::SizeRow(_DWI* rgdwi, int iRow, int iSize /*=0*/)
{
    ASSERT(rgdwi != NULL);

    _DWI* pdwi;

    // If no size given, find the largest window size.
    if (iSize)
    {
    	// Make size be the size of the windows in the row.
        if (m_arRows[iRow].bStretchy)
    		iSize -= g_mt.nSplitter;
	}
	else
    {
        for (pdwi = rgdwi ; pdwi->pWnd != NULL; pdwi++)
        {
            if (pdwi->iRow == iRow)
                iSize = max(iSize, pdwi->cAcross);
        }

        // No windows in this row.
        if (!iSize)
        {
        	RemoveRow(rgdwi, iRow);
        	return;
        }
    }

    // Increment following rows.
    int iInc = iSize - (m_arRows[iRow + 1].nStart - m_arRows[iRow].nStart);

    CSize sizeBorder(g_mt.cxBorder, g_mt.cyBorder);
    DockTranspose(m_dp, &sizeBorder);

    BOOL bSplitAbove = (m_dp == dpRight || m_dp == dpBottom);

    if (m_arRows[iRow].bStretchy)
    	iInc += g_mt.nSplitter;
    else
    	iInc -= sizeBorder.cy;	// Overlap.

	// Remove the overlap if the row does not border a splitter below.
	// Note that we have final sentinal row, so that it is always valid
	// to test iRow + 1.
	ASSERT(iRow < m_nRows);

	if (OnChicago() && !m_arRows[bSplitAbove ? iRow + 1 : iRow].bStretchy)
		iInc += sizeBorder.cy;	// Remove overlap.

    if (iInc)
    {
        for (int i = iRow + 1; i <= m_nRows; i++)
            m_arRows[i].nStart += iInc;
    }

    if (m_arRows[iRow].bStretchy)
    {
	    // Resize all stretchy windows.
	    for (pdwi = rgdwi; pdwi->pWnd != NULL; pdwi++)
	    {
	        if (pdwi->iRow == iRow && (pdwi->pWnd->GetStyle() & DWS_STRETCHY) != 0)
            	pdwi->cAcross = iSize;
	    }
	}
}

//	CDockWnd::SortRow
//		Given a layout structure, and a row, sorts the DWI structs for the
//		the row to the top of the array, and by position.  Uses insertion
//		sort, as these are likely to be pretty small sorts.

void CDockWnd::SortRow(_DWI* rgdwi, int iRow, CWnd* pWnd /*=NULL*/)
{
    for (_DWI* pdwi = rgdwi; pdwi->pWnd != NULL; pdwi++)
    {
	    _DWI* pdwiInsert;
	    _DWI dwiTmp;

        if (pdwi->iRow == iRow)
        {
            for (pdwiInsert = rgdwi; pdwiInsert < pdwi &&
            	pdwiInsert->iRow == iRow; pdwiInsert++)
			{
				// We found that people tended to expect small windows dropped
				// in the upper half of large/stretchy windows should sort before
				// them.  Hence the complex comparison below.
				//
				if ((pdwi->pWnd == pWnd && pdwiInsert->cAlong > pdwi->cAlong) ||
					(pdwiInsert->pWnd == pWnd && pdwi->cAlong > pdwiInsert->cAlong))
				{
					if (pdwi->iPos + pdwi->cAlong / 2 <
						pdwiInsert->iPos + pdwiInsert->cAlong / 2)
						break;
				}
				else if (pdwi->iPos < pdwiInsert->iPos)
				{
					break;
				}
            }

            if (pdwiInsert < pdwi)
            {
                dwiTmp = *pdwi;
                memmove(pdwiInsert + 1, pdwiInsert,
                	(pdwi - pdwiInsert) * sizeof(_DWI));
                *pdwiInsert = dwiTmp;
            }
        }
    }
}

//  CDockWnd::RecalcRow
//      Given layout array, row, and optionally window pointer, shifts windows
//      according to layout rules:
//          1. windows don't overlap,
//          2. all windows in a row are visible.  (wrap to new row if off screen)
//          3. windows never extend past left/top of dock.
//          4. only extend off right/bottom if not enough space to fit all windows.
//			5. rows with stretchy windows fill the whole row.

void CDockWnd::RecalcRow(_DWI* rgdwi, int iRow,	CWnd* pWnd /*=NULL*/,
	BOOL bForceWrap /*=FALSE*/)
{
//	TRACE0("CDockWnd::RecalcRow\n");

    // Make working variables generic to horizontal and vertical dock.
    CRect rectDock;
    GetClientRect(rectDock);
    CSize sizeBorder(g_mt.cxBorder, g_mt.cyBorder);
    rectDock.InflateRect(sizeBorder);

	// If the recalc forces the menu bar to change height, this is the ammount we add to nStart in subsequent rows
	// int nMenuBarHeightChange=0;

    DockTranspose(m_dp, rectDock);
    DockTranspose(m_dp, &sizeBorder);

	SortRow(rgdwi, iRow, pWnd);

	m_arRows[iRow].bStretchy = FALSE;
	m_arRows[iRow].bMaxy = FALSE;
	m_arRows[iRow].bMenuBar=FALSE;

	_DWI* pdwi;
	for (pdwi = rgdwi; pdwi->pWnd != NULL && pdwi->iRow == iRow; pdwi++)
	{
		if (pdwi->pWnd->GetStyle() & DWS_STRETCHY)
		{
			if (m_arRows[iRow].bStretchy)
				m_arRows[iRow].bMaxy = TRUE;		// More than one stretchy
			else
				m_arRows[iRow].bStretchy = TRUE;	// At least one stretchy
		}
		if (pdwi->pWnd->GetStyle() & DWS_MENUBAR)
		{
			m_arRows[iRow].bMenuBar = TRUE;			// At least one menu bar
		}
	}

	// Initialize the maximize item if necessary.
	if (!m_arRows[iRow].bInit)
	{
		if (m_arRows[iRow].nIDMaxWindow == 0)
			m_arRows[iRow].pWndMax = NULL;
		else
		{
			CDockWorker* pDocker = m_pManager->
				WorkerFromID(m_arRows[iRow].nIDMaxWindow);
			if (pDocker == NULL)
				m_arRows[iRow].pWndMax = NULL;
			else
				m_arRows[iRow].pWndMax = pDocker->m_pWnd;
		}

		m_arRows[iRow].bInit = TRUE;
	}

	// Handle maximization
	if (!m_arRows[iRow].bMaxy)
		m_arRows[iRow].pWndMax = NULL;
	else if (m_arRows[iRow].pWndMax != NULL)
	{
		_DWI* pdwi = FindDockInfo(rgdwi, m_arRows[iRow].pWndMax);
		ASSERT(pdwi != NULL && pdwi->pWnd->GetStyle() & DWS_STRETCHY);
		
		// Make the maximize window as big as possible, and the dominant
		// window, and let the stretch code resize it back to the right
		// size.

		pWnd = pdwi->pWnd;
		pdwi->cAlong = rectDock.Width();
	}

	// menu bar always fills its row. We find it from its toolbar id, which is reliable, but not intellectually satisfying. REVIEW.
	if(m_arRows[iRow].bMenuBar)
	{
		CDockWorker* pDocker = m_pManager->WorkerFromID(MAKEDOCKID(PACKAGE_SUSHI, IDTB_MENUBAR));
		
		if (pDocker != NULL)
		{
			CWnd *pWndMenu=pDocker->m_pWnd;

			_DWI* pdwi = FindDockInfo(rgdwi, pWndMenu);
			ASSERT(pdwi != NULL && pdwi->pWnd->GetStyle() & DWS_MENUBAR);
			
			// Make the maximize window as big as possible, and the dominant
			// window, and let the stretch code resize it back to the right
			// size.

			pWnd = pdwi->pWnd;
			pdwi->cAlong = rectDock.Width();

			// Don't resize menu bar if we're minimized. Restoring will sort it out later
			BOOL bAppMinimized=AfxGetMainWnd()->IsIconic();

			if((m_dp==dpTop || m_dp==dpBottom) && !bAppMinimized)
			{
				// vertically stretch docked menu bar
				CRect rectCurrent;
				pDocker->m_pWnd->GetWindowRect(&rectCurrent);

				ASSERT(pDocker->m_pWnd->IsKindOf(RUNTIME_CLASS(CASBar)));
				CASBar *pBar=(CASBar *)pDocker->m_pWnd;

				CSize sizeFit=pBar->Fit(pdwi->cAlong, orHorz);

				// dockman will change pos for us
				pdwi->cAcross=sizeFit.cy;
			}
		}
	}

	int rgnTmp[128];
	int nSplitterCount = 0, iWnd = 0;

    // Handle window stretching.
    if (m_arRows[iRow].bStretchy ||
		m_arRows[iRow].bMenuBar)
    {
		int nTotalSize = 0;
		int nLastStretch = -1;

		// Calculate how far off we are.
		for (pdwi = rgdwi; pdwi->pWnd != NULL && pdwi->iRow == iRow; pdwi++, iWnd++)
		{
		    nTotalSize += pdwi->cAlong;
		    if (!OnChicago())
		    	nTotalSize -= sizeBorder.cx;	// Overlap.

		    if (pdwi->pWnd->GetStyle() & DWS_STRETCHY)
		    {
		    	if (nLastStretch != -1)
		    	{
		    		if (nLastStretch == iWnd - 1)
		    			rgnTmp[nSplitterCount++] = nLastStretch;
		    		else
		    		{
		    			rgnTmp[nSplitterCount++] = nLastStretch;
		    			rgnTmp[nSplitterCount++] = iWnd - 1;
		    		}
		    	}

	    		nLastStretch = iWnd;
		    }
		}

		m_arRows[iRow].nSplitters = nSplitterCount;
		delete [] m_arRows[iRow].pnPositions;

		if (nSplitterCount)
			m_arRows[iRow].pnPositions = new int[nSplitterCount];
		else
			m_arRows[iRow].pnPositions = NULL;

		if (nTotalSize > 0)
		{
			if (OnChicago())
				nTotalSize += nSplitterCount * g_mt.nSplitter;
			else
				nTotalSize += sizeBorder.cx + nSplitterCount * (g_mt.nSplitter + 1);
		}

		BOOL bGrow = (rectDock.Width() > nTotalSize);
		CPoint ptMin = CDockWorker::GetDefMin();
		DockTranspose(m_dp, &ptMin);

		// Try to soak up the size difference into the available stretchy windows.
		while (nTotalSize != rectDock.Width())
		{
			BOOL bTerminalCase;
			int nStretchAlong = 0, nStretchCount = 0;
			int nStretchSize, nLoopDelta, nCurDelta;

			// Loop summing the total size of the stretch windows available to
			// soak up the difference. (May differ depending on whether we grow or not.)
			//
			for (pdwi = rgdwi; pdwi->pWnd != NULL && pdwi->iRow == iRow; pdwi++)
			{
			    if (pdwi->pWnd != pWnd &&
			    	(pdwi->pWnd->GetStyle() & (DWS_STRETCHY|DWS_MENUBAR)) != 0 &&
			    	(bGrow || pdwi->cAlong > ptMin.x))
				{
			    	nStretchAlong += pdwi->cAlong;
					nStretchCount++;
				}
			}

			if (nStretchCount == 0)
				break;

			nLoopDelta = rectDock.Width() - nTotalSize;
			bTerminalCase = (abs(nLoopDelta) < nStretchCount);

			for (pdwi = rgdwi; pdwi->pWnd != NULL && pdwi->iRow == iRow;
				pdwi++)
			{
			    if (pdwi->pWnd != pWnd &&
			    	(pdwi->pWnd->GetStyle() & DWS_STRETCHY) != 0 &&
			    	(bGrow || pdwi->cAlong > ptMin.x))
		    	{
					if (bTerminalCase)
						nStretchSize = bGrow ? 1 : -1;
					else
		    			nStretchSize = (nLoopDelta * pdwi->cAlong) / nStretchAlong;

					nCurDelta = rectDock.Width() - nTotalSize;
					if (abs(nStretchSize) > abs(nCurDelta))
						nStretchSize = nCurDelta;

					pdwi->cAlong += nStretchSize;
					nTotalSize += nStretchSize;

					if (pdwi->cAlong < ptMin.x)
					{
						nTotalSize += ptMin.x - pdwi->cAlong;
						pdwi->cAlong = ptMin.x;
					}
			   	}
			}
		}

		// If we still have some left, and the placed window is stretchy, let it soak
		// up the differnce.
		if (pWnd != NULL && nTotalSize != rectDock.Width() &&
			(pWnd->GetStyle() & DWS_STRETCHY) != 0)
		{
			ASSERT(pWnd != NULL);
			pdwi = FindDockInfo(rgdwi, pWnd);
			pdwi->cAlong = max(ptMin.x,
				pdwi->cAlong + (rectDock.Width() - nTotalSize));
		}
	}	// bStretch

	rgnTmp[nSplitterCount] = -1;

    // Find last window in the row.
    for (pdwi = rgdwi; pdwi->pWnd != NULL && pdwi->iRow == iRow; pdwi++)
        ;

    // Walk back making sure everything fits inside max boundary.
    int nPaneMax;
	if (m_arRows[iRow].bStretchy)
		nPaneMax = rectDock.left;
	else
		nPaneMax = rectDock.right;

    while (--pdwi >= rgdwi && pdwi->pWnd != pWnd)
    {
	    if (pdwi->iPos + pdwi->cAlong > nPaneMax)
	        pdwi->iPos = nPaneMax - pdwi->cAlong;

        nPaneMax -= pdwi->cAlong;
        if (!OnChicago())
        	nPaneMax += sizeBorder.cx;	// Border overlap.
    }

    if (pWnd != NULL)
    {
    	ASSERT(pdwi->pWnd == pWnd);	// Window must be in layout.

        if (pdwi->iPos + pdwi->cAlong > nPaneMax)
            pdwi->iPos = nPaneMax - pdwi->cAlong;

        // Walk back removing window overlap.
        if (!m_arRows[iRow].bStretchy)
        	nPaneMax = pdwi->iPos;

        while (--pdwi >= rgdwi)
        {
			if (!OnChicago())
            	nPaneMax += sizeBorder.cx;   // Overlap.
            if (pdwi->iPos + pdwi->cAlong <= nPaneMax)
                break;

            nPaneMax = pdwi->iPos = nPaneMax - pdwi->cAlong;
        }
    }

    // Walk forward through row, removing window overlap.
    nSplitterCount = 0;
	m_arRows[iRow].bStretchy = FALSE;
    int nPaneMin = rectDock.left;   // Force windows to at least the dock min.

    for (iWnd = 0; (++pdwi)->pWnd != NULL && pdwi->iRow == iRow; iWnd++)
    {
		BOOL bMenuBarWrap=m_arRows[iRow].bMenuBar; // TRUE if we are wrapping the row because it contains the menu bar
        if (bMenuBarWrap ||
			((pWnd != NULL  || bForceWrap) &&
        		(nPaneMin > rectDock.right || pdwi->iPos > rectDock.right)))
        {
            // Wrap remainder to new row.
            InsertRow(rgdwi, iRow + 1, 1);  // size the row later.

            nPaneMin = rectDock.left;
            do
            {
				// wrap everything but the menubar
				if(!bMenuBarWrap ||								// if we're not wrapping because of the menu bar, then we want to move even the menu bar to the next line
					(pdwi->pWnd->GetStyle() & DWS_MENUBAR)==0)
				{
					pdwi->iRow = iRow + 1;
					pdwi->iPos = nPaneMin;
					nPaneMin += pdwi->cAlong;
					if (!OnChicago())
                		nPaneMin -= sizeBorder.cx;	// Overlap

					if (pdwi->pWnd->GetStyle() & DWS_STRETCHY)
						m_arRows[iRow + 1].bStretchy = TRUE;
				}
				else
				{
					// menu bar stays in its row, and gets moved to the left
					pdwi->iPos = 0;
				}
            } while ((++pdwi)->pWnd != NULL && pdwi->iRow == iRow);

            if (m_arRows[iRow + 1].bStretchy)
            	RecalcRow(rgdwi, iRow + 1);
            else
		        SizeRow(rgdwi, iRow + 1);

            break;
        }

        if (pdwi->iPos < nPaneMin)
            nPaneMin = (pdwi->iPos = nPaneMin) + pdwi->cAlong;
        else
            nPaneMin = pdwi->iPos + pdwi->cAlong;

        if (!OnChicago() || iWnd == rgnTmp[nSplitterCount])
        	nPaneMin -= sizeBorder.cx;	// Overlap

        // Update splitters as we go.
        if (iWnd == rgnTmp[nSplitterCount])
        {
        	ASSERT(nSplitterCount < m_arRows[iRow].nSplitters);

        	m_arRows[iRow].pnPositions[nSplitterCount++] = nPaneMin;
        	nPaneMin += g_mt.nSplitter + sizeBorder.cx;
        }

		if (pdwi->pWnd->GetStyle() & DWS_STRETCHY)
			m_arRows[iRow].bStretchy = TRUE;
    }

    SizeRow(rgdwi, iRow);
}

// CDockWnd message map.

BEGIN_MESSAGE_MAP(CDockWnd, CWnd)
    //{{AFX_MSG_MAP(CDockWnd)
    ON_WM_WINDOWPOSCHANGED()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SYSCOMMAND()
	ON_WM_PARENTNOTIFY()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
    ON_REGISTERED_MESSAGE(DMM_CHILDSETFOCUS, OnRouteMessage)
    ON_REGISTERED_MESSAGE(DMM_CHILDKILLFOCUS, OnRouteMessage)
    ON_REGISTERED_MESSAGE(DMM_NCLBUTTONDOWN, OnRouteMessage)
    ON_REGISTERED_MESSAGE(DMM_NCLBUTTONDBLCLK, OnRouteMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDockWnd message handlers

//	CDockWnd::PreTranslateMessage
//		Forward MDI keys to MDI.

BOOL CDockWnd::PreTranslateMessage(MSG* pMsg)
{
	if (MDIKeyMessage(pMsg))
		return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

//  CDockWnd::OnRouteMessage
//      Dockable windows send messages (DMM_*) to their parents.  We just
//      forward everything to the manager, to see if it is handled.

LRESULT CDockWnd::OnRouteMessage(WPARAM wParam, LPARAM lParam)
{
    return (BOOL) m_pManager->WorkerMessage(GetCurrentMessage()->message,
        (HWND) wParam, lParam);
}

LRESULT CDockWnd::OnCommandHelp(WPARAM, LPARAM lParam)
{
	return CommandHelp(lParam);
}

//  CDockWnd::OnSizeParent
//      Mostly clipped from CControlBar.  Used to size the CDockWnds, as
//      well as the View/MDI Client, called in CFrameWnd::RecalcLayout()

LRESULT CDockWnd::OnSizeParent(WPARAM, LPARAM lParam)
{
    AFX_SIZEPARENTPARAMS FAR* lpLayout = (AFX_SIZEPARENTPARAMS FAR*)lParam;

    // resize and reposition this control bar based on styles
    DWORD dwStyle = GetStyle();


    if (m_dp != dpHide && (dwStyle & WS_VISIBLE) != 0 &&
    	m_pManager->IsVisible())
    {
        // align the dock
		CSize sizeBorder(g_mt.cxBorder, g_mt.cyBorder);
		if (OnChicago())
			sizeBorder.cx = sizeBorder.cy = 0;

        CRect rect;
        rect.CopyRect(&lpLayout->rect);
        rect.InflateRect(sizeBorder.cx, sizeBorder.cy);

        CSize sizeAvail = rect.Size();  // maximum size available
        CSize size(sizeBorder.cx * 2, sizeBorder.cy * 2);

		DockTranspose(m_dp, &sizeAvail);
		DockTranspose(m_dp, &size);
		size.cx = sizeAvail.cx;
		size.cy += m_arRows[m_nRows].nStart;
		if (OnChicago())
		{
			size.cy -= 1;
			if (size.cy <= 0)
				size.cy = 0;	// Nothing to show.
			else if (m_dp == dpBottom || m_dp==dpTop)
				size.cy += 2;	// Etched border.
		}
		DockTranspose(m_dp, &size);

		switch (m_dp)
		{
		case dpLeft:
            lpLayout->rect.left += size.cx - sizeBorder.cx;
            break;

        case dpTop:
            lpLayout->rect.top += size.cy - sizeBorder.cy;
            break;

        case dpRight:
            rect.left = rect.right - size.cx;
            lpLayout->rect.right -= size.cx - sizeBorder.cx;
			break;

		case dpBottom:
            rect.top = rect.bottom - size.cy;
            lpLayout->rect.bottom -= size.cy - sizeBorder.cy;
            break;

        default:
            ASSERT(FALSE);      // can never happen
            break;
        }

        rect.right = rect.left + size.cx;
        rect.bottom = rect.top + size.cy;

		// only resize the window if doing layout and not just rect query
		if (lpLayout->hDWP != NULL)
			AfxRepositionWindow(lpLayout, m_hWnd, &rect);
    }
    return 0;
}

void CDockWnd::GetSplitterRect(int iRow, CRect& rect, BOOL bTracking /*=FALSE*/)
{
	BOOL bSplitAbove = (m_dp == dpRight || m_dp == dpBottom);

	GetClientRect(rect);
	DockTranspose(m_dp, rect);

	rect.left -= 1;
	rect.right += 1;

	if (bSplitAbove)
	{
		rect.top = m_arRows[iRow].nStart;
		rect.bottom = rect.top + g_mt.nSplitter + 2;
	}
	else
	{
		rect.bottom = m_arRows[iRow + 1].nStart + 1;
		rect.top = rect.bottom - g_mt.nSplitter - 2;
	}

	if (bTracking)
	{
		rect.InflateRect(-1, -1);
		if (OnChicago())
			rect.OffsetRect(0, -1);
	}
	
	DockTranspose(m_dp, rect);
}

void CDockWnd::GetSplitterRect(int iRow, int nPosition, CRect& rect,
	BOOL bTracking /*=FALSE*/)
{
	BOOL bSplitAbove = (m_dp == dpRight || m_dp == dpBottom);

	rect.left = nPosition;
	rect.right = nPosition + g_mt.nSplitter + 2;
	rect.top = m_arRows[iRow].nStart;
	rect.bottom = m_arRows[iRow + 1].nStart - g_mt.nSplitter;

	if (bSplitAbove)
		rect.OffsetRect(0, g_mt.nSplitter + 1);

	if (bTracking)
		rect.InflateRect(-1, -1);
	
	if (OnChicago())
	{
		if (bTracking)
			rect.OffsetRect(-1, 0);
		if (bSplitAbove && !m_arRows[iRow + 1].bStretchy)
			rect.bottom -= 1;
	}

	DockTranspose(m_dp, rect);
}

CPoint CDockWnd::HitTest(CPoint point)
{
	CPoint ptReturn(-1, -1);

	DockTranspose(m_dp, &point);
	for (int i = 0; m_arRows[i].nStart < point.y; i++)
		ASSERT(i <= m_nRows);
	DockTranspose(m_dp, &point);

	if (i == 0)
		return ptReturn;	// Before first row.	

	i--;

	if (m_arRows[i].bStretchy)
	{
		CRect rect;
		GetSplitterRect(i, rect);
		rect.InflateRect(-1, -1);

		if (rect.PtInRect(point))
			ptReturn.x = i;
		else
		{
			for (int j = 0; j < m_arRows[i].nSplitters; j++)
			{
				GetSplitterRect(i, m_arRows[i].pnPositions[j], rect);
				rect.InflateRect(-1, -1);

				if (rect.PtInRect(point))
				{
					ptReturn.x = i;
					ptReturn.y = j;
				}
			}
		}
	}

	return ptReturn;
}

void CDockWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// The hidden dock should not be getting mouse messages, but due to what
	// seems to be a Windows bug, it does when we are using it to track
	// the popup menu, and we bring up another window to break the tracking.
	//
	if (m_dp == dpHide)
	{
		if (GetCapture() == this)
			ReleaseCapture();

		return;
	}


	BOOL bVertDock = (m_dp == dpLeft || m_dp == dpRight);
	CPoint ptTest = HitTest(point);

	if (ptTest.x != -1 && ptTest.y != -1)
		::SetCursor(bVertDock ? s_hcurHSize : s_hcurVSize);
	else if (ptTest.x != -1)
		::SetCursor(bVertDock ? s_hcurVSize : s_hcurHSize);
	else
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	CWnd::OnMouseMove(nFlags, point);
}

void CDockWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_dp == dpHide)
		return;

	CPoint ptTest = HitTest(point);
	if (ptTest.x == -1 && ptTest.y == -1)
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	int nChange;
	ClientToScreen(&point);

	if (!TrackSize(point, ptTest, nChange))
		return;

	_DWI* rgdwi = BeginLayout();

	if (ptTest.y == -1)
	{
		SizeRow(rgdwi, ptTest.x,
			m_arRows[ptTest.x + 1].nStart -
			m_arRows[ptTest.x].nStart + nChange);
		SizeRow(rgdwi, ptTest.x);	//	Snap back to largest non-stretcher.
	}
	else
	{
		SortRow(rgdwi, ptTest.x);
		_DWI* pdwiWnd;
		_DWI* pdwiNext;

		ASSERT(m_arRows[ptTest.x].pnPositions != NULL);
		int nSplitPos = m_arRows[ptTest.x].pnPositions[ptTest.y];

		// Get the sizes right, and then call RecalcRow to make everything
		// adjacent.
		CPoint ptMin = CDockWorker::GetDefMin();
		DockTranspose(m_dp, &ptMin);

		int nScore = 0;
		int nOffset = (nChange < 0) ? -1 : 1;
		nChange *= -nOffset;

		for (_DWI* pdwi = rgdwi; pdwi->pWnd != NULL && pdwi->iRow == ptTest.x;
			pdwi++)
		{
			if (pdwi->pWnd->GetStyle() & DWS_STRETCHY)
			{
				int nNewScore = (nSplitPos - pdwi->iPos) * nOffset;
				if (nNewScore > 0 && (nNewScore < nScore || nScore == 0))
				{
					nScore = nNewScore;
					pdwiWnd = pdwi;
				}
			}
		}

		ASSERT(nScore);

		// Make the size changes.
    	pdwiWnd->cAlong -= nChange;

        for (pdwiNext = pdwiWnd + nOffset;
        	 pdwiNext >= rgdwi &&
        	 pdwiNext->pWnd != NULL &&
        	 pdwiNext->iRow == ptTest.x; pdwiNext += nOffset)
        {
        	if ((pdwiNext->pWnd->GetStyle() & DWS_STRETCHY) == 0)
        		continue;

        	pdwiNext->cAlong += nChange;

        	if (pdwiNext->cAlong < ptMin.x)
        	{
        		nChange = pdwiNext->cAlong - ptMin.x;
        		pdwiNext->cAlong = ptMin.x;
        	}
        	else
        	{
        		nChange = 0;
        		break;
        	}
        }

        pdwiWnd->cAlong += nChange;	// Absorb the remainder.
		m_arRows[ptTest.x].pWndMax = NULL;	// No max window if the user resizes.
    	RecalcRow(rgdwi, ptTest.x);	// Reposition everything.
	}

	EndLayout(rgdwi);
}

BOOL CDockWnd::TrackSize(CPoint ptStart, CPoint ptHitTest, int& nFinalOffset)
{
	// No tracking when we aren't really visible.  Might end up
	// tracking for during OLE activation.
	if (!m_pManager->IsVisible())
		return FALSE;

    CRect rect, rectOld;
    CPoint pt, ptOld;

    BOOL bCancel = FALSE;
	BOOL bVertDock = (m_dp == dpLeft || m_dp == dpRight);
	BOOL bSplitAbove = (m_dp == dpRight || m_dp == dpBottom);
	BOOL bVertSplit = ((bVertDock && ptHitTest.y == -1) ||
		(!bVertDock && ptHitTest.y != -1));

	nFinalOffset = (ptHitTest.y == -1 && bSplitAbove) ? -1 : 1;

	if (bVertSplit)
		ptStart.y = 0;
	else
		ptStart.x = 0;

	CRect rectBound;
	GetBoundaryRect(ptHitTest, rectBound);

    if (ptHitTest.y == -1)
    	GetSplitterRect(ptHitTest.x, rect, TRUE);
    else
    {
    	ASSERT(m_arRows[ptHitTest.x].pnPositions != NULL);
    	GetSplitterRect(ptHitTest.x,
    		m_arRows[ptHitTest.x].pnPositions[ptHitTest.y], rect, TRUE);
    }

    ClientToScreen(rect);
    rectBound.OffsetRect(ptStart.x - rect.left, ptStart.y - rect.top);

	CClientDC dc(AfxGetApp()->m_pMainWnd);

    // Loop while the mouse is down.
    CWnd* pWndTrack = CWnd::GetFocus();	// So we stay the active window.

	// When in DocObject mode, we save the focus, and give it to one of our
	// window. This is because the doc object's window will not accept
	// that we capture the mouse on its behalf.
	CWnd *pWndSaveFocus=NULL;
	if(m_pManager->IsInDocObjectMode())
	{
		pWndSaveFocus = pWndTrack;
		pWndTrack = this;
	}

    if (pWndTrack == NULL)
	{
		// Try to get a valid focused window.
		m_pManager->m_pFrame->SetFocus();
		if ((pWndTrack = CWnd::GetFocus()) == NULL)
			return FALSE;
	}

    pWndTrack->SetCapture();

    ptOld = pt = ptStart;

    // Turn on the drag rectangle.
    DrawTrackRect(&dc, &rect);

    while (!PollForMouseChange(pt, TRUE, bCancel))
    {
    	if (bVertSplit)
    	{
    		pt.x = max(min(pt.x, rectBound.right), rectBound.left);
    		pt.y = 0;
    	}
    	else
    	{
    		pt.x = 0;
    		pt.y = max(min(pt.y, rectBound.bottom), rectBound.top);
    	}

    	if (pt == ptOld)
    		continue;

        rectOld = rect;
    	rect.OffsetRect(pt.x - ptOld.x, pt.y - ptOld.y);

        // Move the drag rect on screen.
    	DrawTrackRect(&dc, &rectOld, &rect);

        ptOld = pt;
    }

    // Turn off drag rectangle.
    DrawTrackRect(&dc, &rect);
    ::ReleaseCapture();

	// Restore focus to docobject window if we took it
	if(pWndSaveFocus != NULL)
		pWndSaveFocus->SetFocus();

    if (bCancel || ptOld == ptStart)
    	return FALSE;
    else if (bVertSplit)
    	nFinalOffset *= ptOld.x - ptStart.x;
    else
    	nFinalOffset *= ptOld.y - ptStart.y;

    return TRUE;
}

void CDockWnd::DrawTrackRect(CDC* pdc, const CRect* prect1,
	const CRect* prect2 /*=NULL*/)
{
	ASSERT_VALID(pdc);

	// invert the brush pattern (looks just like frame window sizing)
	CBrush* pBrush = CDC::GetHalftoneBrush();
	HBRUSH hOldBrush = NULL;
	if (pBrush != NULL)
		hOldBrush = (HBRUSH)SelectObject(pdc->m_hDC, pBrush->m_hObject);

    CRect rect = *prect1;
	AfxGetApp()->m_pMainWnd->ScreenToClient(rect);
	pdc->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);

    if (prect2 != NULL)
    {
    	rect = *prect2;
		AfxGetApp()->m_pMainWnd->ScreenToClient(rect);
		pdc->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
    }

	if (hOldBrush != NULL)
		SelectObject(pdc->m_hDC, hOldBrush);
}

void CDockWnd::GetBoundaryRect(CPoint ptHitTest, CRect& rect)
{
	ASSERT(ptHitTest.x >= 0 && ptHitTest.x < m_nRows);
	ASSERT(m_arRows[ptHitTest.x].bStretchy);

	BOOL bSplitAbove = (m_dp == dpRight || m_dp == dpBottom);

	// Client coordinates used below so that the row offsets used
	// will be valid.
	//
	CRect rectDesk;
	m_pManager->GetDeskRects(rectDesk, rect);
	ScreenToClient(rectDesk);
	DockTranspose(m_dp, rectDesk);

	GetWindowRect(rect);
	ScreenToClient(rect);
	DockTranspose(m_dp, rect);

	CPoint ptMinSize = CDockWorker::GetDefMin();
	DockTranspose(m_dp, &ptMinSize);
	CSize sizeBorder(g_mt.cxBorder, g_mt.cyBorder);
	DockTranspose(m_dp, &sizeBorder);

	if (bSplitAbove)
		rect.top = rectDesk.top;
	else
		rect.bottom = rectDesk.bottom;

	int nPos = 0;

	if (ptHitTest.y != -1)
	{
		ASSERT(m_arRows[ptHitTest.x].pnPositions != NULL);
		nPos = m_arRows[ptHitTest.x].pnPositions[ptHitTest.y];
	}

	_DWI dwi;

	for (CWnd* pWnd = GetTopWindow(); pWnd != NULL;
		pWnd = pWnd->GetWindow(GW_HWNDNEXT))
	{
		GetDockInfo(&dwi, pWnd);

		if (ptHitTest.x == dwi.iRow)
		{
			if ((dwi.pWnd->GetStyle() & DWS_STRETCHY) != 0)
			{
				dwi.cAlong = ptMinSize.x;
				dwi.cAcross = ptMinSize.y;
			}

			if (bSplitAbove)
				rect.bottom = min(rect.bottom,
					m_arRows[ptHitTest.x + 1].nStart - dwi.cAcross + 2);
			else
				rect.top = max(rect.top,
					m_arRows[ptHitTest.x].nStart + dwi.cAcross - 1);

			if (nPos > dwi.iPos)
				rect.left += dwi.cAlong - sizeBorder.cx;
			else
				rect.right -= dwi.cAlong - sizeBorder.cx;
		}
	}

	for (int i = 0; i < m_arRows[ptHitTest.x].nSplitters; i++)
	{
		if (nPos > m_arRows[ptHitTest.x].pnPositions[i])
			rect.left += g_mt.nSplitter + 1;
		else
			rect.right -= g_mt.nSplitter + 1;
	}

	rect.right--;
	rect.bottom -= g_mt.nSplitter + 2;

	DockTranspose(m_dp, rect);
	ClientToScreen(rect);
}

void CDockWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_dp == dpHide)
		return;

	CPoint ptTest = HitTest(point);
	if (ptTest.x != -1 || ptTest.y != -1)
	{
		// In a split rect, so ignore.
		CWnd::OnLButtonDblClk(nFlags, point);
		return;
	}

	if (CASBar::s_pCustomizer == NULL)
	{
		ASSERT(m_pManager->m_pFrame != NULL);
#ifdef _DEBUG
		if ((::GetKeyState(VK_CONTROL) & ~1) != 0)
			m_pManager->m_pFrame->SendMessage(WM_COMMAND, IDM_OPENLAYOUT);
		else
#endif
			m_pManager->m_pFrame->SendMessage(WM_COMMAND, IDM_TOOLBAR_EDIT);
	}
}

void CDockWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags, point);

	ClientToScreen(&point);
	m_pManager->ShowMenu(point);
}

void CDockWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE)
	{
		// DevStudio96 RAID #11708 [patbr]:
		// if there are no MDI child windows open, we want to effectively
		// pass the keypress on to the mainframe's <esc> handler...
		if (theApp.HasOpenDocument(FILTER_NONE))
			theApp.ReactivateView();
		else
			((CMainFrame *)theApp.m_pMainWnd)->OnCancel();
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDockWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (OnChicago())
		OnDrawChicago(&dc);
	else
		OnDrawNT(&dc);
}

void CDockWnd::OnDrawChicago(CDC* pdc)
{
	ASSERT(OnChicago());

	CRect rect;

	// Draw the splitters
	BOOL bVertDock = (m_dp == dpLeft || m_dp == dpRight);

	for (int i = 0; i < m_nRows; i++)
	{
		if (m_arRows[i].bStretchy)
		{
			GetSplitterRect(i, rect);
			DrawSplitter(pdc, rect, bVertDock);
			pdc->ExcludeClipRect(rect);
		}
	}

	for (i = 0; i < m_nRows; i++)
	{
		if (m_arRows[i].bStretchy)
		{
			for (int j = 0; j < m_arRows[i].nSplitters; j++)
			{
				GetSplitterRect(i, m_arRows[i].pnPositions[j], rect);
				DrawSplitter(pdc, rect, !bVertDock);

				DockTranspose(m_dp, rect);
				rect.top -= 1;
				rect.bottom += 1;
				DockTranspose(m_dp, rect);
				pdc->ExcludeClipRect(rect);
			}
		}
	}

	COLORREF colorLt = GetSysColor(COLOR_BTNHIGHLIGHT);
	COLORREF colorDk = GetSysColor(COLOR_BTNSHADOW);

	// Draw the window borders.
	for (CWnd* pWnd = GetTopWindow(); pWnd != NULL;
		pWnd = pWnd->GetWindow(GW_HWNDNEXT))
	{
		pWnd->GetWindowRect(rect);
		ScreenToClient(rect);
		pdc->Draw3dRect(rect, colorLt, colorDk);
		pdc->ExcludeClipRect(rect);
	}

	// Erase the remaining background.
	GetClientRect(rect);
	pdc->FillSolidRect(rect, GetSysColor(COLOR_BTNFACE));

	// This may look really wierd, but it produces the best 3d results for
	// the cost.  Clipping from above, and order of painting are important.

	// Draw borders.
	for (pWnd = GetTopWindow(); pWnd != NULL; pWnd = pWnd->GetWindow(GW_HWNDNEXT))
	{
		pWnd->GetWindowRect(rect);
		ScreenToClient(rect);

		pdc->Draw3dRect(rect, colorLt, colorDk);
	}
}

void CDockWnd::OnDrawNT(CDC* pdc)
{
	// Draw the splitters
	BOOL bVertDock = (m_dp == dpLeft || m_dp == dpRight);

	CRect rect;

	for (int i = 0; i < m_nRows; i++)
	{
		if (m_arRows[i].bStretchy)
		{
			GetSplitterRect(i, rect);
			DrawSplitter(pdc, rect, bVertDock);

			for (int j = 0; j < m_arRows[i].nSplitters; j++)
			{
				GetSplitterRect(i, m_arRows[i].pnPositions[j], rect);
				DrawSplitter(pdc, rect, !bVertDock);
			}
		}
	}
}

//	CDockWnd::OnWindowPosChanged
//		Recalc the rows, if we sized in the stretch direction, so stretchy
//		windows will continue to fill the window.

void CDockWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
    if (m_dp == dpHide || (GetStyle() & WS_VISIBLE) == 0)
		return;

	CSize size(lpwndpos->cx, lpwndpos->cy);

	DockTranspose(m_dp, &size);
    if (size.cx != m_nAlong)
    {
		m_nAlong = size.cx;

		if (m_nRows > 0)
		{
	    	_DWI* pdwi = BeginLayout();

			// Walk back since rows may be deleted if they are empty.
	    	for (int i = m_nRows - 1; i >= 0; i--)
	    		RecalcRow(pdwi, i, NULL, !m_bInitialized);

			// In row stretch shouldn't need to recalc.
			m_bChicagoSizingHack = TRUE;
	    	EndLayout(pdwi, FALSE);
			m_bChicagoSizingHack = FALSE;
    	}

		m_bInitialized = TRUE;
    }
}

void CDockWnd::OnParentNotify(UINT message, LPARAM lParam)
{
	if (theApp.m_hwndValidate != NULL &&
		::SendMessage(theApp.m_hwndValidate, WM_ISERROR, 0, 0L))
	{
		return;
	}
	else if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN)
	{
		CPoint pt(::GetMessagePos());
		CWnd* pWndNotify = WindowFromPoint(pt);
		while (pWndNotify != NULL && pWndNotify->GetParent() != this)
			pWndNotify = pWndNotify->GetParent();

		CDockWorker* pDocker = m_pManager->WorkerFromWindow(pWndNotify);
		if (pDocker != NULL && pDocker != CDockWorker::s_pActiveDocker &&
				pDocker->m_dtWnd == dtEdit && CASBar::s_pCustomizer == NULL)
			pWndNotify->SetFocus();
	}

	CWnd::OnParentNotify(message, lParam);
}

void CDockWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpParams)
{
	if (OnChicago())
	{
		if (m_dp != dpBottom)
        	lpParams->rgrc->top += 2;
		if (m_dp != dpTop)
        	lpParams->rgrc->bottom -= 2;
		return;
	}
        
	CWnd::OnNcCalcSize(bCalcValidRects, lpParams);
}

void CDockWnd::OnNcPaint()
{
	if (OnChicago())
	{
		// Draw etched border
	    CWindowDC dc(this);

		CRect rect;
	    GetWindowRect(rect);
	    rect.OffsetRect(-rect.left, -rect.top);

		COLORREF colorLt = GetSysColor(COLOR_BTNHIGHLIGHT);
		COLORREF colorDk = GetSysColor(COLOR_BTNSHADOW);

		if (m_dp != dpBottom)
		{
			dc.FillSolidRect(rect.left, rect.top, rect.Width(), 1, colorDk);
			dc.FillSolidRect(rect.left, rect.top + 1, rect.Width(), 1, colorLt);
		}

		if (m_dp != dpTop)
		{
			dc.FillSolidRect(rect.left, rect.bottom - 2, rect.Width(), 1, colorDk);
			dc.FillSolidRect(rect.left, rect.bottom - 1, rect.Width(), 1, colorLt);
		}
		return;
	}

	CWnd::OnNcPaint();
}

LRESULT CDockWnd::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID(m_pManager);

	return m_pManager->m_pFrame->SendMessage(WM_SETMESSAGESTRING,
		wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
// 	Utility functions

//	MDIKeyMessage
//		Forward MDI keys to the main window.

BOOL MDIKeyMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_TAB:
		case VK_F4:
		case VK_F6:
			if ((GetKeyState(VK_CONTROL) & ~1) != 0)
			{
				theApp.m_pMainWnd->PostMessage(pMsg->message,
					pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//	Dock coordinate functions
//		Most handling in vertical docks is merely the transpose of that
//		in horizontal docks.  Use these functions to convert to and from
//		canonical coordinates (as if all docks were horizontal).
//

void DockTranspose(DOCKPOS dp, SIZE FAR* lpsize)
{
	if (dp == dpLeft || dp == dpRight)
	{
		SIZE size;

		size.cx = lpsize->cy;
		size.cy = lpsize->cx;

		*lpsize = size;
	}
}

void DockTranspose(DOCKPOS dp, LPPOINT lppt)
{
	if (dp == dpLeft || dp == dpRight)
	{
		POINT pt;

		pt.x = lppt->y;
		pt.y = lppt->x;

		*lppt = pt;
	}
}

void DockTranspose(DOCKPOS dp, LPRECT lprect)
{
	if (dp == dpLeft || dp == dpRight)
	{
		RECT rect;

		rect.left = lprect->top;
		rect.top = lprect->left;
		rect.right = lprect->bottom;
		rect.bottom = lprect->right;

		*lprect = rect;
	}
}

///////////////////////////////////////////////////////////////////////////////
//	CMDIChildDock
//

IMPLEMENT_DYNAMIC(CMDIChildDock, CPartFrame)

CMDIChildDock::CMDIChildDock(CDockManager* pManager)
{
	ASSERT_VALID(pManager);
	m_pManager = pManager;
}

CMDIChildDock::~CMDIChildDock()
{
}

BOOL CMDIChildDock::Create(LPCSTR lpClassName, LPCSTR lpTitle,
	CWnd* pChildWnd, CMDIFrameWnd* pParentWnd,
	CRect rect)
{
	if (!CPartFrame::Create(lpClassName, lpTitle,
			WS_CHILD | WS_OVERLAPPEDWINDOW,	rect, pParentWnd))
		return FALSE;

	// Child docks have a view that will supply the client edge.
	if (theApp.m_bWin4)
		ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

	pChildWnd->ShowWindow(SW_SHOWNOACTIVATE);

	// While the child is in the MDI child, the border window needs
	// to have the ID AFX_IDW_PANE_FIRST, so layout will work correctly.
	// The view also needs this ID so MFC will set it as the active view.
	//
	SetChildID(pChildWnd, AFX_IDW_PANE_FIRST);

	pChildWnd->SetParent(this);
    m_pManager->AddMDIChild(this);
    RecalcLayout();

	return TRUE;
}

void CMDIChildDock::ParentChild(CWnd* pWndNewParent)
{
	CWnd* pChildWnd = GetTopWindow();
	ASSERT_VALID(pChildWnd);

	SetChildID(pChildWnd, m_nIDSave);
	pChildWnd->SetParent(pWndNewParent);
}

void CMDIChildDock::SetChildID(CWnd* pChildWnd, UINT nID)
{
	m_nIDSave = pChildWnd->GetDlgCtrlID();

	// Orion 96 Bug 15325 - GetDescendantWindow is finding an IV window with the id E900
	// which happens to be AFX_IDW_PANE_FIRST. Replaced with another function which 
	// returns the first window with an ID which is also a CPartView.
	CPartView* pView = GetFirstChildWindowWithID(pChildWnd->m_hWnd, m_nIDSave);
	ASSERT(pView != NULL && pView->IsKindOf(RUNTIME_CLASS(CPartView)));
	::SetWindowLong(pChildWnd->m_hWnd, GWL_ID, nID);
	::SetWindowLong(pView->m_hWnd, GWL_ID, nID);
}

CDockWorker* CMDIChildDock::GetWorker() const
{
	return m_pManager->WorkerFromWindow(GetTopWindow());
}

BEGIN_MESSAGE_MAP(CMDIChildDock, CPartFrame)
    //{{AFX_MSG_MAP(CMDIChildDock)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDIChildDock message handlers

//	CMDIChildDock::OnClose
//		Defers to dock manager to get rid of the window.

void CMDIChildDock::OnClose()
{
    CDockWorker* pDocker = GetWorker();
	ASSERT_VALID(pDocker);
	pDocker->ShowWindow(FALSE);
}

