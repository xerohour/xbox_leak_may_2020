// floatwnd.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern UINT WM_ISERROR;

IMPLEMENT_DYNAMIC(CFloatingFrameBar, C3dDialogBar)

CFloatingFrameBar::CFloatingFrameBar()
{
}

BOOL CFloatingFrameBar::Create(CWnd* pParentWnd, UINT nIDTemplate)
{
	// parameters: (CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID );
	return C3dDialogBar::Create(pParentWnd, nIDTemplate, CBRS_TOP, AFX_IDW_TOOLBAR);
}

BOOL CFloatingFrameBar::OnInitDialog()
{
	if (!m_btnPushPin.AutoLoad(ID_PROP_PUSHPIN, this, TRUE) ||
		!m_btnHelp.AutoLoad(ID_HELP, this))
	{
		return FALSE;
	}

	// Override dialog bar sizing, so all our bars are the same size
	m_sizeDefault.cy = CY_TOOLBAR + CY_BORDER;
	if (theApp.m_bWin4)
		m_sizeDefault.cy += CY_BORDER;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CToggleBitmapButton
//

CToggleBitmapButton::CToggleBitmapButton()
{
	m_fDown = FALSE;	// button starts up
}

void CToggleBitmapButton::SetChecked(BOOL fDown)
{
	if (fDown == m_fDown)
		return;

	// exchange the up and down bitmaps...
	m_fDown = !m_fDown;
	HGDIOBJ hbitmap1 = m_bitmap.Detach();
	HGDIOBJ hbitmap2 = m_bitmapSel.Detach();
	m_bitmap.Attach(hbitmap2);
	m_bitmapSel.Attach(hbitmap1);

	InvalidateRect(NULL, FALSE);
}

BEGIN_MESSAGE_MAP(CToggleBitmapButton, CBitmapButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CToggleBitmapButton::OnLButtonUp(UINT n1, CPoint pt)
//
// the user has just released the button, we must now swap the bitmap
// images to get the toggle behaviour -- i.e. the nuetral state becomes
// down if it used to be up and up if it used to be down BUT ONLY
// IF THE USER RELEASED THE BUTTON WITH THE CURSOR IN THE CLIENT AREA
//
{
	CRect rect;
	GetClientRect(&rect);

	if (GetCapture() == this && rect.PtInRect(pt))
	{
		// exchange the up and down bitmaps...
		m_fDown = !m_fDown;
		HGDIOBJ hbitmap1 = m_bitmap.Detach();
		HGDIOBJ hbitmap2 = m_bitmapSel.Detach();
		m_bitmap.Attach(hbitmap2);
		m_bitmapSel.Attach(hbitmap1);
	}

	// do default processing for this message...
	C3dBitmapButton::OnLButtonUp(n1, pt);
}


/////////////////////////////////////////////////////////////////////////////
// CFloatingFrameWnd
//
// This class implements floating frame windows with push pins, such
// as the browser window.  The shell identifies these windows using
// IsKindOf(), so all floating frame windows should be derived from
// this class.

IMPLEMENT_DYNAMIC(CFloatingFrameWnd, CFrameWnd)


BEGIN_MESSAGE_MAP(CFloatingFrameWnd, CFrameWnd)

	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_MOUSEACTIVATE()

	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(ID_PROP_PUSHPIN, OnPushPin)
	ON_BN_DOUBLECLICKED(ID_PROP_PUSHPIN, OnPushPin)

END_MESSAGE_MAP()


CFloatingFrameWnd::CFloatingFrameWnd()
{
	m_pToolBar = NULL;
}

CFloatingFrameWnd::~CFloatingFrameWnd()
{
	if (m_pToolBar != NULL)
	{
		delete m_pToolBar;
		m_pToolBar = NULL;
	}
}

BOOL CFloatingFrameWnd::IsWindowPinned()
{
	CFloatingFrameBar* pBar = GetToolBar();
	ASSERT(pBar && pBar->IsKindOf( RUNTIME_CLASS(CFloatingFrameBar) ));

	return pBar->m_btnPushPin.IsButtonDown();
}

void CFloatingFrameWnd::SetPushPin(BOOL bPinned)
{
	CFloatingFrameBar* pBar = GetToolBar();
	ASSERT(pBar && pBar->IsKindOf( RUNTIME_CLASS(CFloatingFrameBar) ));

	pBar->m_btnPushPin.SetChecked(bPinned);
}

void CFloatingFrameWnd::OnActivate(UINT nState, CWnd* pWndNew, BOOL b)
{
	if (nState == WA_INACTIVE)
	{
		// If the window isn't pinned and our app is retaining the focus,
		// close this window.
		if (!IsWindowPinned() && pWndNew != NULL)
			PostMessage(WM_CLOSE, 0, 0L);
	}

	CFrameWnd::OnActivate(nState, pWndNew, b);
}

void CFloatingFrameWnd::OnActivateApp(BOOL bActive, HTASK)
{
	// if button is not down... we must activate now or
	// we'll go away regardless of where the user clicks

	if (!IsWindowPinned())
		SetFocus();
}

int CFloatingFrameWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest,
	UINT message)
{
	if (theApp.m_hwndValidate != NULL &&
		::SendMessage(theApp.m_hwndValidate, WM_ISERROR, 0, 0L))
	{
		return MA_NOACTIVATEANDEAT;
	}

	return CFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

BOOL CFloatingFrameWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_SYSKEYDOWN)
	{
#if 0
		// Don't do this.  These windows usually have a system menu.
		// Under Win95, the system menu shows the accelerator Alt+F4
		// for the close menuitem.  The windows are more like modeless
		// dialogs which can be closed using Alt+F4.

		// forward Alt+F4 to the shell window
		if (pMsg->wParam == VK_F4) 
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0L);
			return TRUE;
		}
#endif
		if (GetToolBar()->PreTranslateSysKey(pMsg))
		{
			return TRUE;
		}
		if (::IsChild(m_hWnd, pMsg->hwnd))
		{
			PostMessage(WM_SYSKEYDOWN, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
	}

	return CFrameWnd::PreTranslateMessage(pMsg);	
}

BOOL CFloatingFrameWnd::OnCreateClient(LPCREATESTRUCT lpCreateStruct, CCreateContext* pCreateContext)
{
	// If a derived class hasn't already created the toolbar, create it

	if (GetToolBar() == NULL)
		m_pToolBar = new CFloatingFrameBar;

	if ((GetToolBar()->m_hWnd == NULL) &&
	    !GetToolBar()->Create(this, IDD_PUSHPIN_DIALOGBAR))
	{
		TRACE("Failed to Create/Init Toolbar\n");
		return FALSE;
	}

	// must manually initialize the dialog bar because there will be no
	// WM_INIT message as there is no dialog per se.  We are just a dialog
	// template...
	if (!GetToolBar()->OnInitDialog())
	{
		TRACE("Failed to Init Toolbar\n");
		return FALSE;
	}

	return CFrameWnd::OnCreateClient(lpCreateStruct, pCreateContext);
}

afx_msg void CFloatingFrameWnd::OnHelp()
{
	// Each floating window derived class will override the GetHelpID
	// virtual function to provide the proper help context ID.
	//
	DWORD helpID = GetHelpID();

	// orion  bug 24 bobz: help button causes unpinned browser to vanish,
	// taking help with it.
	// Seems to me that this is the simplest fix: see if window is pinned; if not pin it
	// restore state afterward. Much simpler than messing with activation code
	// etc.

	BOOL fWasPinned = IsWindowPinned();
	if (!fWasPinned) 
		SetPushPin(TRUE);

	if (helpID != 0)
		theApp.HelpOnApplication(helpID);

#ifdef _DEBUG
	else
	{
		::MessageBeep(0);	// no help available, apparently
		AfxMessageBox("This CFloatingFrameWnd derived class has not provided a help context ID");
	}
#endif	// _DEBUG

	if (!fWasPinned) 	// restore if required
		SetPushPin(FALSE);

}

afx_msg void CFloatingFrameWnd::OnPushPin()
{
	// The push-pin control handles its own visual appearance.
}
