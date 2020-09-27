// partfrm.cpp : implementation of the CPartFrame class
//

#include "stdafx.h"

#include "imeutil.h"
#include "AutoWinD.h"
#include "bardockx.h"
#include "docobfrm.h"
#include "docobvw.h"
#include "toolexpt.h"
#include <aut1api.h>
#include <aut1guid.h>
#include "shellrec.h"
#include "ipcmpvw.h"
#include "shldocs_.h"

IMPLEMENT_DYNCREATE(CPartFrame, CMDIChildWnd)
IMPLEMENT_DYNCREATE(CDocObjectFrame, CPartFrame)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Internally used messages.
//
#define FRM_DOCOBJACTIVATE	WM_USER+1	// Delay activation for DocObjects.
#define FRM_AUTOOBJACTIVATE  WM_USER+2	// Delay automation object notification of activation.

/////////////////////////////////////////////////////////////////////////////
// CPartFrame

BOOL CPartFrame::s_bLockFrame = FALSE;
WORKSPACE_INFO AFX_EXT_DATADEF* CPartFrame::s_pwi = NULL;
HWND CPartFrame::s_hwndAfter = NULL;

void CPartFrame::LockFrame(BOOL bLock)
{
	s_bLockFrame = bLock;
	s_hwndAfter = NULL;
}

void CPartFrame::LockFrame(HWND hwndAfter)
{
	s_bLockFrame = TRUE;
	s_hwndAfter = hwndAfter;
}

void CPartFrame::LockWorkspace(BOOL bLock)
{
	if ((!bLock && s_pwi == NULL) || (bLock && s_pwi != NULL))
		return;

	theApp.LockRecentFileList(bLock);

	if (bLock)
	{
		s_pwi = new WORKSPACE_INFO;
		s_pwi->pWnd = NULL;
		s_pwi->pInitData = s_pwi->pCurData = NULL;
	}
	else
	{
		delete s_pwi;
		s_pwi = NULL;

		// If we are unlocking the workspace, call OnMDIActivate to refresh
		// menus, titles, and status bar.
		CMDIFrameWnd* pMain = (CMDIFrameWnd*) AfxGetApp()->m_pMainWnd;
		CPartFrame* pFrame = (CPartFrame*) pMain->MDIGetActive();
		if (pFrame != NULL)
		{
			ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));
			pFrame->OnMDIActivate(TRUE, pFrame, NULL);
		}
	}

}

CPartFrame::CPartFrame() : CMDIChildWnd()
{
	m_pAutoFrameObj = NULL ;
	m_hwndView = NULL;
	m_pToolbar = NULL;
}

//
// Destructor - Added 12 Apr 96 - DER.
//
CPartFrame::~CPartFrame()
{
 	//OLEAUTO: Inform the associated automation object that we are gone.
	if (m_pAutoFrameObj != NULL)
	{
		m_pAutoFrameObj->AssociatedObjectReleased() ;
	}
}


void CPartFrame::SetInitData(BYTE* pData)
{
	ASSERT(s_pwi != NULL);
	ASSERT(s_pwi->pInitData == NULL || s_pwi->pInitData != pData);
	delete [] s_pwi->pInitData;
	s_pwi->pInitData = s_pwi->pCurData = pData;

	s_pwi->pWnd = NULL;	// Reset the window pointer as well.
}

BOOL CPartFrame::IsInWorkspace()
{
	CPartDoc* pDoc = (CPartDoc*) GetActiveDocument();
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));

	// if the document doesn't want to display its path, its path is probably not valid or useful.
	// we dont want to save this window under this situation.
	if (pDoc != NULL && !pDoc->FDisplayTitle())	 
		return (!pDoc->GetPathName().IsEmpty());
	else
		return FALSE;
}

BOOL CPartFrame::IsDependent()
{
	// Delegate this question to the active view.
	CPartView* pView = (CPartView*) GetActiveView();
	ASSERT_VALID(pView);
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));

	return pView->IsDependent();
}

BYTE* CPartFrame::GetInitData()
{
	// Delegate this to the active view.
	CPartView* pView = (CPartView*) GetActiveView();
	ASSERT_VALID(pView);
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));

	return pView->GetInitData();
}

void CPartFrame::OnLoadWorkspace()
{
	CPartView* pView = (CPartView*)GetActiveView();
	ASSERT_VALID(pView);
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));

	pView->OnLoadWorkspace();
}

CString CPartFrame::GetWorkspaceMoniker(BOOL bRelative)
{
	static char BASED_CODE chRelativeDir = '.';

	CDocument* pDoc = GetActiveDocument();
	ASSERT_VALID(pDoc);
	CString strMoniker = pDoc->GetPathName();
	CPath path;
	CDir dir;

	IProjectWorkspace *pInterface;
	pInterface = g_IdeInterface.GetProjectWorkspace();
	ASSERT(pInterface != NULL);
	LPCSTR pszWorkspace;
	CString strWorkspace;
	if (SUCCEEDED(pInterface->GetWorkspaceDocPathName(&pszWorkspace)))
		strWorkspace = pszWorkspace;

	if (!strWorkspace.IsEmpty())
		VERIFY(dir.CreateFromPath(strWorkspace));
	else
		VERIFY(dir.CreateFromCurrent());

	if (bRelative && path.Create(strMoniker))
	{
		CString strRelative;
		path.GetRelativeName(dir, strRelative);

		// Beware of GetRelativeName stripping the drive label.
		if (strRelative[0] == chRelativeDir)
			strMoniker = strRelative;
	}

	return strMoniker;
}

void CPartFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle && pDocument != NULL)
	{
		CString strOld;
		GetWindowText(strOld);

		CString strTitle = pDocument->GetPathName();
		if (strTitle.IsEmpty())
			strTitle = pDocument->GetTitle();
		else
			strTitle = GetDisplayName(strTitle,	AFX_ABBREV_FILENAME_LEN);

		LPTSTR lpsz = strTitle.GetBuffer(256);
		if (m_nWindow > 0)
			wsprintf(lpsz + lstrlen(lpsz), _T(":%d"), m_nWindow);
		if (pDocument->IsModified())
		{
			lstrcat(lpsz, _T(" *"));
		}
		strTitle.ReleaseBuffer();

		// set title if changed, but don't remove completely
		if (strTitle != strOld)
			SetWindowText(strTitle);
	}
}

void CPartFrame::OnToolbarStatus()
{
	CWnd* pWnd = GetDescendantWindow(AFX_IDW_TOOLBAR);
	if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(C3dDialogBar)))
		m_pToolbar = (C3dDialogBar*) pWnd;
}

BEGIN_MESSAGE_MAP(CPartFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CPartFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_MDIACTIVATE()
	ON_WM_PARENTNOTIFY()
	ON_WM_SETFOCUS()
	ON_WM_SYSCOMMAND()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_GETMINMAXINFO()
	ON_WM_MENUSELECT()

	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_MESSAGE(FRM_DOCOBJACTIVATE, OnDocObjActivate)
	ON_MESSAGE(FRM_AUTOOBJACTIVATE, OnAutoObjActivate)

	ON_COMMAND(ID_MDI_CYCLE_FORWARD, OnMDICycleForward)

	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0x0000, 0xffff, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0x0000, 0xffff, OnToolTipText)
END_MESSAGE_MAP()

BOOL CPartFrame::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYUP:
		if (pMsg->wParam == VK_CONTROL)
			s_hwndAfter = NULL;
		break;

	case WM_KEYDOWN:
		if (pMsg->wParam == VK_CONTROL && (pMsg->lParam & 0x40000000) == 0)
			s_hwndAfter = NULL;
	    else if (pMsg->wParam == VK_RETURN && IsIconic())
	    {
			ActivateFrame();
	        return TRUE;
	    }
		else if (m_pToolbar != NULL &&
			(pMsg->wParam == VK_RETURN ||
			pMsg->wParam == VK_ESCAPE) &&
			(GetKeyState(VK_CONTROL) & ~1) == 0 &&
			::IsChild(m_pToolbar->m_hWnd, ::GetFocus()))
		{
			SetFocus();	// Frame sets focus to active view.
		}
		break;

	case WM_SYSKEYDOWN:
		if (m_pToolbar != NULL && m_pToolbar->PreTranslateSysKey(pMsg))
			return TRUE;
		break;
	}

	return CMDIChildWnd::PreTranslateMessage(pMsg);
}

int CPartFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nCreate = CMDIChildWnd::OnCreate(lpCreateStruct);

	if (s_pwi != NULL)
	{
		if (nCreate == -1)
			s_pwi->pWnd = NULL;
		else
			s_pwi->pWnd = this;
	}

	return nCreate;
}

void CPartFrame::OnSize(UINT nType, int cx, int cy)
{
	switch (nType)
	{
	case SIZE_MINIMIZED:
		EnableToolTips(TRUE);
		break;

	case SIZE_RESTORED:
	case SIZE_MAXIMIZED:
		EnableToolTips(FALSE);
		if (GetMDIFrame()->MDIGetActive() == this)
			theApp.m_bMaximizeDoc = (nType == SIZE_MAXIMIZED);
		break;
	}

	CMDIChildWnd::OnSize(nType, cx, cy);

	// recalc the menu bar to sort out MDI gadgets
	CASBar *pMenuBar=CASBar::GetMenuBar();
	if(pMenuBar)
	{
		pMenuBar->EnsureMDIControls();
	}
}

void CPartFrame::OnMove(int x, int y)
{
	if (theApp.m_fOEM_MBCS && (GetMDIFrame()->MDIGetActive() == this))
		imeMoveConvertWin(m_hWnd, theIME.m_xPix, theIME.m_yPix);
}

void CPartFrame::OnGetMinMaxInfo (MINMAXINFO * pMinMaxInfo)
{
	CMDIChildWnd::OnGetMinMaxInfo (pMinMaxInfo);

	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	if (pFrame->IsFullScreen())
	{
		// Make sure we don't get restricted by the tracking size.
		pMinMaxInfo->ptMaxTrackSize.x = max(pMinMaxInfo->ptMaxTrackSize.x,
			pMinMaxInfo->ptMaxSize.x);
		pMinMaxInfo->ptMaxTrackSize.y = max(pMinMaxInfo->ptMaxTrackSize.y,
			pMinMaxInfo->ptMaxSize.y);
	}
}

void CPartFrame::ActivateFrame(int nCmdShow)
{
	if(s_bInActivation)
		return;

	CMDIFrameWnd* pFrameWnd = GetMDIFrame();
	CMDIChildWnd* pChildTop = pFrameWnd->MDIGetActive();

	if (s_bLockFrame && pChildTop != NULL)
	{
		ASSERT(nCmdShow != SW_HIDE);

		CMDIChildWnd* pChildAfter;

		if (s_hwndAfter == NULL)
			pChildAfter = pChildTop;
		else
			pChildAfter = (CMDIChildWnd*) FromHandle(s_hwndAfter);

		ASSERT(pChildAfter != NULL &&
			pChildAfter->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));
		
		if (!IsIconic())
		{
			SetWindowPos(pChildAfter, 0, 0, 0, 0,
				SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			ShowWindow(SW_SHOWNOACTIVATE);
		}
		else
		{
			// Windows will always put the window on top, if it is restored
			// from an icon.
			CWnd* pWndClient = GetParent();
			ASSERT(pWndClient != NULL);
			pWndClient->ShowWindow(SW_HIDE);

			ShowWindow(SW_SHOWNOACTIVATE);
			pChildTop->BringWindowToTop();
			pChildTop->SetFocus();

			if (pChildAfter != pChildTop)
			{
				SetWindowPos(pChildAfter, 0, 0, 0, 0,
					SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			}
			
			pWndClient->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		BOOL bVisibleThen = (GetStyle() & WS_VISIBLE) != 0;

		if (nCmdShow == -1 && theApp.m_bMaximizeDoc)
			nCmdShow = SW_SHOWMAXIMIZED;

		if (!IsWindowVisible())
			ShowWindow((nCmdShow == -1) ? SW_SHOW : nCmdShow); // show it
		else if (IsIconic())
			ShowWindow((nCmdShow == -1) ? SW_RESTORE : nCmdShow);
		else if (nCmdShow == SW_HIDE)
			ShowWindow(nCmdShow);

		if (GetStyle() & WS_VISIBLE)
		{
			BringWindowToTop();
			HWND hWndLastPop = ::GetLastActivePopup(m_hWnd);
			if (hWndLastPop != NULL && hWndLastPop != m_hWnd)
				::BringWindowToTop(hWndLastPop);

			// If after above calls, a dock worker is still active, set focus.
			if (CDockWorker::s_pActiveDocker != NULL)
				SetFocus();
		}

		// update the Window menu to reflect new child window
		::SendMessage(pFrameWnd->m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);


		// Note: Update the m_bPseudoInactive flag.  This is used to handle the
		//  last MDI child getting hidden.  Windows provides no way to deactivate
		//  an MDI child window.

		BOOL bVisibleNow = (GetStyle() & WS_VISIBLE) != 0;
		if (bVisibleNow == bVisibleThen)
			return;

		if (!bVisibleNow)
		{
			// get current active window according to Windows MDI
			HWND hWnd = (HWND)::SendMessage(pFrameWnd->m_hWndMDIClient, 
				WM_MDIGETACTIVE, 0, 0);
			if (hWnd != m_hWnd)
			{
				// not active any more -- window must have been deactivated
				ASSERT(!m_bPseudoInactive);
				return;
			}

			// check next window
			ASSERT(hWnd != NULL);
			pFrameWnd->MDINext();

			// see if it has been deactivated now...
			hWnd = (HWND)::SendMessage(pFrameWnd->m_hWndMDIClient, 
				WM_MDIGETACTIVE, 0, 0);
			if (hWnd == m_hWnd)
			{
				// still active -- fake deactivate it
				ASSERT(hWnd != NULL);
				OnMDIActivate(FALSE, NULL, this);
				m_bPseudoInactive = TRUE;   // so MDIGetActive returns NULL
			}
		}
		else if (m_bPseudoInactive)
		{
			// if state transitioned from not visible to visible, but
			//  was pseudo deactivated -- send activate notify now
			OnMDIActivate(TRUE, this, NULL);
			ASSERT(!m_bPseudoInactive); // should get set in OnMDIActivate!
		}
	}
}

COleFrameHook *CPartFrame::s_pFrmHookActivate = NULL;
COleFrameHook *CPartFrame::s_pFrmHookDeactivate = NULL;

CPartFrame *CPartFrame::s_pLastActiveFrame = NULL;
BOOL CPartFrame::s_bInActivation = FALSE;
BOOL CPartFrame::s_bInDocObjClose = FALSE;

void CPartFrame::FinishActivation()
{
	if(s_pFrmHookActivate == NULL && s_pFrmHookDeactivate == NULL)
	{
		return;
	}

	s_bInActivation = TRUE;
	((CMainFrame *)theApp.m_pMainWnd)->LockLayout(TRUE);
	CWaitCursor waitCur;
	if(s_pFrmHookActivate && !s_pFrmHookDeactivate)
	{
		Menu::UseOLE(TRUE);		// We are entering DocObject mode
		Menu::UpdateMode(FALSE); // and update the mode
		DkDocObjectMode(TRUE);
	}
	if(s_pFrmHookDeactivate)
		s_pFrmHookDeactivate->OnDocActivate(FALSE);
	if(s_pFrmHookActivate)
		s_pFrmHookActivate->OnDocActivate(TRUE);

	if(!s_pFrmHookActivate)
	{
		Menu::UseOLE(FALSE);	// We are leaving DocObject mode
		Menu::UpdateMode(FALSE); // and update the mode
		DkDocObjectMode(FALSE);
	}
	if(((CMainFrame*) theApp.m_pMainWnd)->m_pManager)
		((CMainFrame*) theApp.m_pMainWnd)->m_pManager->IdleUpdateAvailableWnds();

	((CMainFrame *)theApp.m_pMainWnd)->LockLayout(FALSE);
	((CMainFrame *)theApp.m_pMainWnd)->RecalcLayout();
	theApp.m_pMainWnd->RedrawWindow(NULL, NULL,
	            RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
	
	s_pFrmHookActivate = NULL;
	s_pFrmHookDeactivate = NULL;
	s_bInActivation = FALSE;
}


void CPartFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	// Workspace load will do one final activation with
	// lock off to update everything.
	//
	if (IsWorkspaceLocked())
		return;

	// Do not try to activate another window while we switch DocObjects
	if(s_bInActivation)
		return;

	s_bInActivation = TRUE;
	
	if (bActivate  && GetMDIFrame() != GetActiveWindow())
	{
		// MFC does not do OnActivateView for us, if the main window
		// is not active.  This is to support OLE servers.  We are not
		// an OLE server, and we need the activate notification for the
		// property page.

		CPartView* pView = (CPartView*)GetActiveView();
		ASSERT(pView == NULL || pView->IsKindOf(RUNTIME_CLASS(CPartView)));

		if (pView != NULL)
			pView->OnActivateView(TRUE, pView, pView);
	}


	// This part stolen from MFC 4.2 CMDIChildWnd::OnMDIActivate()
	m_bPseudoInactive = FALSE;  // must be happening for real

	// make sure MDI client window has correct client edge
	UpdateClientEdge();

	// send deactivate notification to active view
	CPartView* pActiveView = (CPartView *)GetActiveView();
	if (!bActivate && pActiveView != NULL)
		pActiveView->OnActivateView(FALSE, pActiveView, pActiveView);

	// allow hook to short circuit normal activation
	BOOL bHooked = FALSE;

//	We don't want to activate DocObjects just yet
	if(bActivate)
	{
		// We should not have a pending activation
		ASSERT(s_pFrmHookActivate == NULL);
		bHooked = TRUE;
		s_pFrmHookActivate = m_pNotifyHook;
			
		s_pLastActiveFrame = this;
	}
	else
	{
		// Can we short circuit an activation?
		// Yes if: we are de-activating a non-DocObj and last activation
		// was a non-DocObj 
		// OR
		// if we are de-activating a DocObj which matches the last activation
		// Note: I cannot assert that m_pNotifyHook==s_pFrmHookActivate because
		// in the case of a WM_CLOSE, m_pNotifyHook is already NULL'd when we get here.
		if((s_pLastActiveFrame == this) &&
			((!s_pFrmHookActivate && !m_pNotifyHook) || s_pFrmHookActivate))
		{
			s_pFrmHookActivate = NULL;
			s_pLastActiveFrame = NULL;
		}
		else
		{
			// We should not have a deactivation pending
			ASSERT(s_pFrmHookDeactivate == NULL);

			// Make sure we deactivate the currently active object
			if(s_pLastActiveFrame != NULL)
				s_pFrmHookDeactivate = s_pLastActiveFrame->m_pNotifyHook;

			s_pLastActiveFrame = NULL;
		}
	}

	// Do not defer activation for in-place components
	if(m_pNotifyHook && pActiveView &&
		  !pActiveView->DeferActivation())
	{
		m_pNotifyHook->OnDocActivate(bActivate);
		if(bActivate)
			s_pFrmHookActivate = NULL;
		else
			s_pFrmHookDeactivate = NULL;
	}



	// update titles (don't AddToTitle if deactivate last)
	if (!bHooked)
		OnUpdateFrameTitle(bActivate || (pActivateWnd != NULL));

	// re-activate the appropriate view
	if (bActivate)
	{
		if (pActiveView != NULL && GetMDIFrame() == GetActiveWindow())
			pActiveView->OnActivateView(TRUE, pActiveView, pActiveView);
	}

	OnUpdateFrameMenu(bActivate, pActivateWnd, NULL);
	GetMDIFrame()->DrawMenuBar();

	// recalc the menu bar to sort out MDI gadgets
	CASBar *pMenuBar=CASBar::GetMenuBar();
	if(pMenuBar)
	{
		// invalidate the MDI icon, since RecalcLayout doesn't imply redraw, and we 
		// might have changed to a new doc type
		pMenuBar->EnsureMDIControls();
		pMenuBar->UpdateMDIIcon();
	}

	// Do not defer deactivation if we are closing the window, because
	// the link to the OLE server is about to be severed.
	if(s_bInDocObjClose)
	{
		// If the window is invisible, that means we are
		// shutting down the IDE, so do not try to activate anything.
		if(!((CMainFrame*)theApp.m_pMainWnd)->IsWindowVisible())
			s_pFrmHookActivate = NULL;

		// Do not deactivate another view
		if(bActivate || (!bActivate && s_pFrmHookDeactivate == m_pNotifyHook))
			FinishActivation();
	}
	// HACK! when we close a doc-object using File.Close, the NotifyHook is
	// NULL by the time we get here, so let's make sure we switch contexts
	else if(!bActivate && IsKindOf(RUNTIME_CLASS(CDocObjectFrame)) &&
		m_pNotifyHook == NULL && 
		(!pActivateWnd || !pActivateWnd->IsKindOf(RUNTIME_CLASS(CDocObjectFrame))) )
	{
		Menu::UseOLE(FALSE);	// We are leaving DocObject mode
		Menu::UpdateMode(FALSE); // and update the mode
		DkDocObjectMode(FALSE);
	}
	else if(bActivate || pActivateWnd == NULL)
	{
	// Let's defer Doc-object activation by sending ourselves a message 
	// (which will trigger the activation).
	// The message arrives after the dragging messages, if any, so that we
	// can change the doc-abject activation after a SizeMove drag.
		PostMessage(FRM_DOCOBJACTIVATE, 0, 0);
	}

	s_bInActivation = FALSE;

	// Fire the appropriate event
	// We delay the firing of the event...see the message handler.
	PostMessage(FRM_AUTOOBJACTIVATE, bActivate, 0) ;

}

void CPartFrame::OnSetFocus(CWnd* pOldWnd)
{
	if (m_pViewActive != NULL)
	{
		// Make sure we call OnActivateView, or we may not do the right thing
		// coming back from docking views.
		//

		// Deactivate any dock worker that is still active at this point.
		if (CDockWorker::s_pActiveDocker != NULL)
		{
			CDockWorker::s_bLockDocker = FALSE;
			CDockWorker::s_pActiveDocker->OnWindowActivate(FALSE, this);
		}

		// Not a friend of CView.
		((CPartView*)m_pViewActive)->OnActivateView(TRUE,
			m_pViewActive, m_pViewActive);
	}
	else
	{
		CWnd::OnSetFocus(pOldWnd);
	}
}

void CPartFrame::OnUpdateFrameMenu(BOOL bActivate, CWnd* pActivateWnd,
	HMENU hMenuAlt)
{
	if (IsWorkspaceLocked())
		return;

	if (hMenuAlt != NULL)
	{
		CMDIChildWnd::OnUpdateFrameMenu(bActivate, pActivateWnd, hMenuAlt);
		return;
	}

	CPartView* pActiveView = (CPartView*) GetActiveView();
	if (pActiveView == NULL)
		return;

	ASSERT(pActiveView->IsKindOf(RUNTIME_CLASS(CPartView)));

	if (bActivate)
		theApp.ActivatePacket(pActiveView->GetPacket());
	else if (pActivateWnd == NULL)
	{
		theApp.DeactivatePacket();
		theApp.m_theAppSlob.SetSlobProp(P_CurrentSlob, NULL);
	}
	else
	{
		// Refresh MDI Window menu (even if non-shared menu)
		::SendMessage(GetMDIFrame()->m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);
	}

	// If OLE deactivation has left us without a menu, rebuild now.
	if (theApp.m_bMenuDead)
		((CMainFrame*) GetMDIFrame())->RebuildMenus();
}

void CPartFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch(nID & 0xFFF0)
	{
		case SC_NEXTWINDOW:
			MDINextWindow(TRUE);
			return;

		case SC_PREVWINDOW:
			MDINextWindow(FALSE);
			return;

		case SC_MINIMIZE:
		case SC_MAXIMIZE:
		case SC_RESTORE:
			DkUpdateAvailableWnds();
			break;

		case SC_MOVE:
			// why do this? Because it happens when you single click on an icon before the double
			// click to deminimize the icon
			CBMenuPopup::SetSystemClick(GetMessageTime(), CPoint(LOWORD(lParam), HIWORD(lParam)));
		case SC_SIZE:
			{
				CSlobWnd* pView = (CSlobWnd*)GetActiveView();
				if( (pView != NULL) && pView->IsKindOf(RUNTIME_CLASS(CSlobWnd)) )
					pView->ObscureSprites();
			}
			break;

		case SC_MOUSEMENU:
			CBMenuPopup::SetSystemClick(GetMessageTime(), CPoint(LOWORD(lParam), HIWORD(lParam)));
		case SC_KEYMENU:
			if(	((nID & 0xFFF0)==SC_MOUSEMENU || lParam=='-') &&
				CASBar::s_pMenuBar->GetSafeHwnd()!=NULL &&
				Menu::IsShowingCmdBars())
			{
				CASBar::s_pMenuBar->KeyDown(VK_SUBTRACT,0,0);
				return;
			}
			break;
	}

	CMDIChildWnd::OnSysCommand(nID, lParam);
}

void CPartFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	switch( nHitTest )
	{
		case HTCAPTION:
		case HTLEFT:
		case HTTOP:
		case HTRIGHT:
		case HTBOTTOM:
		case HTTOPLEFT:
		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
		case HTBOTTOMRIGHT:
		{
			CSlobWnd* pView = (CSlobWnd*)GetActiveFrame()->GetActiveView();
			if( (pView != NULL) && pView->IsKindOf(RUNTIME_CLASS(CSlobWnd)) )
				pView->ObscureSprites();
			break;
		}
	}

	CMDIChildWnd::OnNcLButtonDown(nHitTest, point);
}

static BOOL s_bInSizeMove = FALSE;

void CPartFrame::OnDocObjActivate(UINT, LONG)
{
	if(!s_bInSizeMove)
		FinishActivation();
}

//
// Delay notifying the automation object about the activation change
// We do this because the user may attempt to put up UI in the activation
// handler and then mouse up's will not get handled by the original window.
// See DevStudio Bug # 8443
//
void CPartFrame::OnAutoObjActivate(UINT bActivate, LONG)
{
	if(bActivate)
	{
		// Orion Bug# 12176 - We need to send the NewDocument event only after the
		// document been completely created and is ready for prime time.
		// Therefore, we delay sending the creations event, either NewDocument or
		// OpenDocument until we are ready to send the activation event.
		
		// There is a related problem. Suppose the user attempts to delete the
		// document inside the NewDocument handler. We don't want to crash.
		// However, if this happens this Frame will be deleted. The solution
		// is to NOT use ANY member variables after we call FireCreateEvent below.

		IDispatch* pAssociatedAutoObj = GetAutomationObject(TRUE) ;
		CAutoWindowObj* pAutoFrameObjTemp = m_pAutoFrameObj ;
		
		//UGLY NOTE: pAssociatedAutoObj is the IDispatch pointer to m_pAutoFrameObj.
		// Of course, since we are using MFC we can't just cast.

		// Get the Document which gets this message.
		CPartView* pActiveView = (CPartView*)GetActiveView();
		if (pActiveView != NULL && pActiveView->IsKindOf(RUNTIME_CLASS(CPartView)))
		{
			CPartDoc* pDoc = pActiveView->GetDocument();
			pActiveView = NULL ;

//----- DO NOT USE ANY MEMBER VARIABLES PAST THIS POINT -----

			// Fire the creation event. This may delete the document and hence the frame.
			if (pDoc != NULL)
			{
				pDoc->FireCreationEvent() ;
				pDoc = NULL ;
			}
		}

		if(pAssociatedAutoObj !=  NULL)
		{
			// Fire the event on the application. These may delete the frame.
			theApp.m_pAutoApp->FireWindowActivate(pAssociatedAutoObj);
			pAutoFrameObjTemp->FireActivate();
			pAssociatedAutoObj->Release();
		}
	}
	else
	{
		// Get pointer to the automation object.
		IDispatch* pAssociatedAutoObj = GetAutomationObject(TRUE) ;
		CAutoWindowObj* pAutoFrameObjTemp = m_pAutoFrameObj ;
		ASSERT(m_pAutoFrameObj != NULL) ;

		if(pAssociatedAutoObj !=  NULL)
		{
			// Fire the event on the application.
			theApp.m_pAutoApp->FireWindowDeactivate(pAssociatedAutoObj);
			pAutoFrameObjTemp->FireDeactivate();
			pAssociatedAutoObj->Release();
		}
	}
}


void CPartFrame::OnEnterSizeMove(UINT, LONG)
{
	s_bInSizeMove = TRUE;
}

void CPartFrame::OnExitSizeMove(UINT, LONG)
{
	s_bInSizeMove = FALSE;
	FinishActivation();
}

void CPartFrame::MDINextWindow(BOOL bNext, BOOL bLinear /*=FALSE*/)
{
	// Get the current window at bottom
	CWnd* pWndBottom = GetWindow(GW_HWNDLAST);

	CWnd *pWndAfter, *pWndNext;
	if (s_hwndAfter == NULL || bLinear)
		pWndAfter = this;
	else
		pWndAfter = FromHandle(s_hwndAfter);

	// Find the next window to be shown to the user.
	if(bNext)
		pWndNext = pWndAfter->GetWindow(GW_HWNDNEXT);
	else
		pWndNext = pWndAfter;

	do
	{
		// We got no window, so we will cycle.
		if(pWndNext == NULL)
		{
			if(bNext)
				// get the first one
				pWndNext = this;
			else
				// Get the last one instead
				pWndNext = pWndBottom;
			// Just go to next loop
			continue;
		}

		// Break when we find a valid window that is not current.
		if(pWndNext != this && IsValidMDICycleMember(pWndNext))
			break;

		if (bNext)
			pWndNext = pWndNext->GetWindow(GW_HWNDNEXT);
		else
			pWndNext = pWndNext->GetWindow(GW_HWNDPREV);

    } while (pWndNext != pWndAfter) ;

	// If the window is already on top, do nothing.
    if(pWndNext == this)
    {
   		MessageBeep(0);
   		return;
    }

	// Get the window that will be used to restore the position of the
	// next window.

	CWnd* pWndNextAfter = pWndNext->GetWindow(GW_HWNDPREV);

	while (pWndNextAfter != NULL && !IsValidMDICycleMember(pWndNextAfter))
		pWndNextAfter = pWndNextAfter->GetWindow(GW_HWNDPREV);

	if (pWndNextAfter == NULL)
    {
   		MessageBeep(0);
   		return;
    }

	// Check for a full cycle through the document windows, and make hwndAfter
	// for the first window in the cycle a no-op.

	if (bNext)
	{
		if (pWndNextAfter == pWndAfter)
			pWndNextAfter = this;	// no cycle
		else
			pWndNextAfter = pWndNext;	// cycle
	}
	else if (pWndNextAfter == this)
		pWndNextAfter = pWndNext;		// cycle

	if(bLinear)
	{
		s_hwndAfter=NULL;
	}
	else
	{
		s_hwndAfter = pWndNextAfter->m_hWnd;
	}

    // Get the window flags
	CWnd* pMDIClient = GetParent();
	ASSERT(pMDIClient != NULL);

	// Check if it is maximized
	BOOL bMaximized;		// Window was maximized
	if(bMaximized = IsZoomed())
		pMDIClient->ShowWindow(SW_HIDE);

	HDWP hdwp = ::BeginDeferWindowPos(2);

	// Put current window back where it was.
	if(!bLinear ||	// nonlinear means we need to move the old window
		bNext)	// forward linear means the old needs to be at the back
	{
		if( m_hWnd != pWndAfter->m_hWnd || bLinear )
			hdwp = ::DeferWindowPos(hdwp, m_hWnd, bLinear ? HWND_BOTTOM : pWndAfter->m_hWnd, 0, 0, 0, 0,
	    		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}

	// Move next window to top.
	hdwp = ::DeferWindowPos(hdwp, pWndNext->m_hWnd, HWND_TOP, 0, 0, 0, 0,
    	SWP_NOMOVE | SWP_NOSIZE);

	EndDeferWindowPos(hdwp);

   	if(bMaximized)
		pMDIClient->ShowWindow(SW_SHOW);
}

void CPartFrame::OnClose()
{
	CPartView* pView = (CPartView*)GetActiveView();
	if (pView != NULL)
	{
		ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));
		if (!pView->CanClose())
			return;
	}
#ifdef _DEBUG
	else
		ASSERT(m_hwndView == NULL);
#endif

	s_hwndAfter = NULL;	// Clear ctrl+tab info.

	HWND hWndSave = m_hWnd; // save hWnd to see if we got closed/destroyed

	{
		CHaltRecording halt; // otherwise, we get bogus activations recorded

		CMDIChildWnd::OnClose();
	}

	if (!::IsWindow(hWndSave)) // if the window was closed, the ::IsWindow call will fail
	{
/*		IMacroRecorder *mr;
		theApp.FindInterface(IID_IMacroRecorder, reinterpret_cast<void**>(&mr));
		mr->RecordText("ActiveWindow.Close", "Shell");
		AfxOutputDebugString("ActiveWindow.Close\n");*/

		GetShellRec().EmitMethodCall(seCloseWin, CShellConstArg(scSaveChangesPrompt));
	}
}

void CPartFrame::OnDestroy()
{
	if (s_pwi != NULL && s_pwi->pWnd == this)
		s_pwi->pWnd = NULL;

	if (m_hwndView != NULL)
	{
		// We don't really own the view window, but we do have an HWND map
		// entry that we need to manually remove.
		CWnd* pView = CWnd::FromHandlePermanent(m_hwndView);
		if (pView != NULL)
		{
			ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));
			pView->Detach();
			m_hwndView = NULL;

			delete pView;
		}
	}

	CMDIChildWnd::OnDestroy();
}

void CPartFrame::OnParentNotify(UINT message, LPARAM lParam)
{
	if (message == WM_DESTROY && (HWND)lParam == m_hwndView)
	{
		// If the package destroys the view for some reason, we need
		// to clean up after ourselves...

		CWnd* pView = CWnd::FromHandlePermanent(m_hwndView);
		if (pView != NULL && pView->IsKindOf(RUNTIME_CLASS(CPartView)))
		{
			ASSERT(pView != NULL);
			pView->Detach();
			m_hwndView = NULL;

			SetActiveView(NULL);
			delete pView;
		}

		PostMessage(WM_CLOSE);
	}

	CMDIChildWnd::OnParentNotify(message, lParam);
}

void CPartFrame::OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu )
{
	theApp.m_dwMenuSelectTime = GetCurrentMessage()->time;
	
	CMDIChildWnd::OnMenuSelect( nItemID, nFlags, hSysMenu );
}

LRESULT CPartFrame::OnCommandHelp(WPARAM, LPARAM lParam)
{
	static BOOL bInCommandHelp;

	if(bInCommandHelp)
		return FALSE;

	bInCommandHelp = TRUE;
	ASSERT(m_dwPromptContext == 0);
	if (theApp.CurrentPromptContext() != 0)
	{
		theApp.HelpOnApplication(theApp.CurrentPromptContext(),HELPTYPE_WINDOW);
		goto success;
	}

	if (lParam)
	{
		// id already calculated -- menu tracking or dialog up
		theApp.WinHelp(lParam);
		goto success;
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
					goto success;
				}

				lParam = pView->GetHelpID();
			}
		}
	}

	if (lParam != 0)
	{
		theApp.HelpOnApplication(lParam,HELPTYPE_WINDOW);
		goto success;
	}

	bInCommandHelp = FALSE;
	return FALSE;

success:
	bInCommandHelp = FALSE;
	return TRUE;
}

BOOL IsValidMDICycleMember(CWnd* pWnd)
{
	if (pWnd == NULL)
		return FALSE;

	LONG lStyle = pWnd->GetStyle();

	// Window owned means an icon title window.
	return	(!pWnd->GetWindow(GW_OWNER) &&
			// Must not be disabled
			!(lStyle & WS_DISABLED) &&
			// Must be visible
		   	(lStyle & WS_VISIBLE));
}

void CPartFrame::OnNewPalette()
{
	CPartView* pView = (CPartView*)GetActiveView();
	ASSERT(pView == NULL || pView->IsKindOf(RUNTIME_CLASS(CPartView)));

	if (pView != NULL)
		pView->OnNewPalette();
}

BOOL CPartFrame::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// allow top level routing frame to handle the message
	if (GetRoutingFrame() != NULL)
		return FALSE;

	return CTheApp::HandleToolTipText(nID, pNMHDR, pResult);
}

void CPartFrame::OnMDICycleForward(void)
{
	((CMainFrame*)theApp.m_pMainWnd)->MDINext();
}

/////////////////////////////////////////////////////////////////////////////////////////
// 
//  OLE Automation support functions.
//

//
// Override to return the automation object corresponding to this object.
// Default creates an automation object and returns it.
//
IDispatch* CPartFrame::GetAutomationObject(BOOL bAddRef)
{
	if (m_pAutoFrameObj)
	{
		return m_pAutoFrameObj->GetIDispatch(bAddRef);
	}

	m_pAutoFrameObj= new CAutoWindowDefault(this) ; // Change to Window
	return m_pAutoFrameObj->GetIDispatch(FALSE); // creation is an implicit AddRef
}

void CPartFrame::AutomationObjectReleased()
{
	m_pAutoFrameObj = NULL ;
}

//
// The following function was stolen from opendocs.cpp where it was
// used to close windows displayed in the window list. The code
// was also needed in the automation Window object, so it was centralized
// here.
//
void CPartFrame::CloseFrame()
{
	// get a pointer to the active view in the frame
	CPartView* pActiveView = (CPartView*)GetActiveView();
	ASSERT_KINDOF(CPartView, pActiveView) ;
	// can we close this view?
	if (pActiveView->CanClose())
	{
		// get a pointer to the view's document
		CPartDoc* pDoc = pActiveView->GetDocument();
		ASSERT_KINDOF(CPartDoc, pDoc) ;

		// only save frame's that are saveable
		BOOL bSaved = TRUE;
		if (!IsKindOf(RUNTIME_CLASS(CMDIChildDock))) // Was Is Frame Saveable
		{
			// DevStudio96 Bug # 8243 We will only save if this is the document's only
			// view.
			BOOL bFoundOurSelves = FALSE ;
			int iViewCount = 0 ;
			POSITION pos = pDoc->GetFirstViewPosition() ;
			while (pos != NULL)
			{
				CView* pView = pDoc->GetNextView(pos) ;
				iViewCount++ ;
#ifdef _DEBUG
				if (pView == pActiveView)
				{
					// This code is purely for definsive programming.
					bFoundOurSelves = TRUE ;
				}
#endif
			}

			// If we were not in the list, something is terribly wrong.
			ASSERT(bFoundOurSelves) ;
			ASSERT(iViewCount != 0) ;

			if (iViewCount == 1)
			{
				// Olympus:14702
				// store the current directory and restore it after saving
				// in case the directory is changed during Save As
				TCHAR szDir[MAX_PATH];
				DWORD dwResult = GetCurrentDirectory(MAX_PATH-1, szDir);
				// save it, if necessary
				bSaved = pDoc->SaveModified();
				if (dwResult)
				{
					// set the directory back to its original
					SetCurrentDirectory(szDir);

					// get a pointer to the main frame
					CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
					ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

					// update all window titles
					pMainFrame->OnDirChange();
				}
			}
		}
		if (bSaved)
		{
			// close the window
			if (IsKindOf(RUNTIME_CLASS(CMDIChildDock)))
			{
				SendMessage(WM_CLOSE);
			}
			else
			{
				DestroyWindow();
			}
		}
	}
}
// Override to allow hwnd buttons to nominate an id for their tooltip, depending
// on their context
int CPartFrame::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);
	int nHit=-1;

	CPartView* pActiveView = (CPartView*)GetActiveView();
	ASSERT_KINDOF(CPartView, pActiveView) ;
	// get a pointer to the view's document
	CPartDoc* pDoc = pActiveView->GetDocument();
	if(IsIconic())
	{
		if( pDoc &&
			!pDoc->GetPathName().IsEmpty())
		{
			pTI->uFlags=TTF_IDISHWND;
			pTI->uId = (UINT)GetSafeHwnd();
			pTI->hwnd = GetParent()->GetSafeHwnd();
			pTI->lpszText = _strdup(pDoc->GetPathName());
			nHit=pTI->uId;
		}
	}

	return nHit;
}

/////////////////////////////////////////////////////////////////////////////////////////
// 
//  CDocObjectFrame class.
//
BEGIN_MESSAGE_MAP(CDocObjectFrame, CPartFrame)
	//{{AFX_MSG_MAP(CDocObjectFrame)
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDocObjectFrame::CDocObjectFrame()
{
}

void CDocObjectFrame::OnClose()
{
	// skip the first part of the CPartFrame::OnClose which 
	// is unnecessary for docobject and Inplace components.
	s_hwndAfter = NULL;	// Clear ctrl+tab info.
	
	s_bInDocObjClose = TRUE;

	CMDIChildWnd::OnClose();
	
	s_bInDocObjClose = FALSE;
}

void CDocObjectFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CPartDoc * pDocument = (CPartDoc *)GetActiveDocument();
	if (pDocument != NULL && bAddToTitle)
	{
		ASSERT(pDocument->IsKindOf(RUNTIME_CLASS(CPartDoc)));
		if (pDocument->FDisplayTitle())	// default PartFrame tries to display doc path rather than title
		{
			// update our parent window first
			GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

			if ((GetStyle() & FWS_ADDTOTITLE) == 0)
				return;     // leave child window alone!

			CString strOld;
			GetWindowText(strOld);

			CString strTitle = pDocument->GetTitle();
			LPTSTR lpsz = strTitle.GetBuffer(256);
			ASSERT(m_nWindow <= 0);
			if (pDocument->IsModified())
			{
				lstrcat(lpsz, _T(" *"));
			}
			strTitle.ReleaseBuffer();

			// set title if changed, but don't remove completely
			if (strTitle != strOld)
				SetWindowText(strTitle);

			return;
		}
	}
	
	CPartFrame::OnUpdateFrameTitle(bAddToTitle);
}


void CDocObjectFrame::OnGetMinMaxInfo (MINMAXINFO * pMinMaxInfo)
{
	CPartFrame::OnGetMinMaxInfo(pMinMaxInfo);
	
	// Allow the InPlace doc to set MinMaxInfo
	CIPCompDoc* pDocument = (CIPCompDoc*)GetActiveDocument();
	if(pDocument != NULL && pDocument->IsKindOf(RUNTIME_CLASS(CIPCompDoc)))
		pDocument->GetMinMaxInfo(pMinMaxInfo);
}

