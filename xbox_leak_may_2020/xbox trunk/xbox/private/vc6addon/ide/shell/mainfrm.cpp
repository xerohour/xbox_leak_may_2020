// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include <afximpl.h>
#include <dde.h>
#include "main.h"

#include "about.h"
#include "shell.h"
#include "bardockx.h"
#include "bardlgs.h"
#include "barbdr.h"
#include "barglob.h"
#include "undoctl.h"
#include "msgboxes.h"
#include "tools.h"
#include "imeutil.h"
#include "totd.h"
#include "resource.h"
#include "opendocs.h"
#include "ipchost.h"
#include "prxycust.h"
#include "toolexpt.h"

#ifndef _IPC_DOCUMENTS_DISABLED
#include <objext.h>
#include "ipcfrmhk.h"
#endif

#include <utilauto.h>
#include <cmguids.h>
#include <aut1api.h>
#include <aut1guid.h>
#include <aut1api_.h>
#include <aut1gui_.h>
#include "shellrec.h"
#include "autostat.h"  // for g_AutomationState

#include "shlmenu.h"
#include "cmdcache.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _TEST
#ifdef _DEBUG
#ifdef _WIN32
UINT NEAR WM_CMDPERFCHECK = RegisterMessage("CmdPerfCheck");
#endif   // _WIN32
#endif   // _DEBUG
UINT NEAR WM_SETPROP      = RegisterMessage("SetProp");
UINT NEAR WM_GETPROP      = RegisterMessage("GetProp");
UINT NEAR WM_HEAPUSE      = RegisterMessage("HeapUse");
UINT NEAR WM_TESTDOCKABLE = RegisterMessage("TestDockable");

// Project state
UINT NEAR WM_SETITEMPROP  = RegisterMessage("SetItemProp");
UINT NEAR WM_GETITEMPROP  = RegisterMessage("GetItemProp");
UINT NEAR WM_GETPROJPATH  = RegisterMessage("GetProjPath") ;

// Build state
UINT NEAR WM_CANWEBUILD   = RegisterMessage("CanWeBuild");
UINT NEAR WM_GETERRCOUNT  = RegisterMessage("GetErrCount");
UINT NEAR WM_GETWARNCOUNT = RegisterMessage("GetWarnCount");
#endif   // _TEST
#ifdef _DEBUG
UINT NEAR WM_FATALEXIT = RegisterMessage("FatalExit");
#endif

UINT NEAR WM_SETAPPVALIDATE = RegisterMessage("SetAppValidate");
UINT NEAR WM_ISERROR = RegisterMessage("IsError");

UINT NEAR WM_POST_ENABLE_IME = RegisterMessage("PostEnableIME");
UINT DSM_TESTMENU = RegisterMessage("DevStudioTestMenu");

UINT MSG_MOUSEWHEEL = RegisterMessage(MSH_MOUSEWHEEL);

extern UINT DSM_DROPMENU;
// const values shared with mstwrap.cpp
static const int cMenuMenu=0;
static const int cMenuExists=1;
// static const int cMenuGrayed=2; // for some reason, these two aren't implemented.
// static const int cMenuChecked=3;
static const int cMenuEnabled=4;
static const int cMenuCount=5;
static const int cMenuText=6;
static const int cMenuEnd=7;
static const int cMenuRealChecked=8;

CDocTemplate* AFX_EXT_DATADEF g_pProjectTemplate;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
   //{{AFX_MSG_MAP(CMainFrame)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_WM_MOVE()
   ON_WM_CLOSE()
   ON_WM_INITMENU()
   ON_WM_INITMENUPOPUP()
   ON_WM_MENUSELECT()
   ON_WM_ENABLE()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_RBUTTONDOWN()
   ON_WM_RBUTTONUP()
   ON_WM_MOUSEMOVE()
   ON_WM_NCLBUTTONDOWN()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_NCLBUTTONDBLCLK()
   ON_WM_SYSCOMMAND()
   ON_WM_WININICHANGE()
   ON_WM_FONTCHANGE()
   ON_WM_SETFOCUS()
   ON_WM_ACTIVATE()
   ON_WM_TIMER()
   ON_WM_SYSCOLORCHANGE()
    ON_WM_ENDSESSION() // Profile
   ON_WM_ACTIVATEAPP()
   ON_WM_NCACTIVATE()
   ON_WM_MOUSEACTIVATE()
   ON_WM_QUERYNEWPALETTE()
   ON_WM_PALETTECHANGED()
   ON_WM_DROPFILES()
   ON_WM_GETMINMAXINFO()
        ON_WM_SETCURSOR()
        //}}AFX_MSG_MAP

   // status bar indicators
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_LNCOL, OnUpdateIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZOOM, OnUpdateIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_CLOCK, OnUpdateIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_SIZE, OnUpdateIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_POSITION, OnUpdateIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_OFFSET, OnUpdateIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_EXTENT, OnUpdateIndicator)
   ON_UPDATE_COMMAND_UI(ID_SEPARATOR, OnUpdateIndicator)

#ifdef _DEBUG
   ON_COMMAND(IDM_OPENLAYOUT, OnOpenLayout)
#endif
   ON_COMMAND(ID_WINDOW_CLOSE_ALL, OnWindowCloseAll)
   ON_COMMAND(IDM_WINDOW_CLOSE, OnWindowClose)
   ON_COMMAND(IDM_WINDOW_NEXT, OnWindowNext)
   ON_COMMAND(IDM_WINDOW_PREVIOUS, OnWindowPrevious)
   ON_UPDATE_COMMAND_UI(IDM_WINDOW_CLOSE, OnUpdateWindowCommand)
   ON_UPDATE_COMMAND_UI(IDM_WINDOW_NEXT, OnUpdateWindowCommand)
   ON_UPDATE_COMMAND_UI(IDM_WINDOW_PREVIOUS, OnUpdateWindowCommand)
   ON_COMMAND(ID_WINDOW_FULLSCREEN, OnWindowFullScreen)
   ON_UPDATE_COMMAND_UI(ID_WINDOW_FULLSCREEN, OnUpdateFullScreen)
   ON_COMMAND(ID_WINDOW_LIST_DOCS, OnWindowListDocs)
   ON_COMMAND(ID_WINDOW_NEW, OnNewWindow)
   ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW, OnUpdateNewWindow)

   ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
   ON_COMMAND(ID_FASTHELP, OnFastHelp)
   ON_COMMAND(IDM_HELP_CONTENTS, OnHelpContents)
   ON_COMMAND(ID_IV_SEARCH, OnHelpSearch)
#ifdef __DEVHELP98__ 
   ON_COMMAND(ID_HELP98_KEYWORDS, OnHelpKeywords)
#endif
   ON_COMMAND(ID_HELP_KEYBOARD, OnHelpKeyboard)

   ON_COMMAND(ID_DRAG_MOVE, OnDragMove)
   ON_COMMAND(ID_DRAG_COPY, OnDragCopy)
   ON_COMMAND(ID_DRAG_CANCEL, OnDragCancel)

   ON_COMMAND(ID_CANCEL, OnCancel)
   ON_COMMAND(IDM_GOTO_LINE, CmdGoto)


   ON_COMMAND_EX_RANGE(IDM_DOCKSHOW_BASE, IDM_DOCKSHOW_LAST, OnDockWindowShow)

   ON_COMMAND_RANGE(IDM_MENU_FIRSTMAIN, IDM_MENU_LASTMAIN, OnMenuCommand)
   ON_COMMAND_RANGE(IDM_WINDOWS_BASE, IDM_WINDOWS_LAST, OnWindows)
   ON_UPDATE_COMMAND_UI_RANGE(IDM_MENU_FIRSTMAIN, IDM_MENU_LASTMAIN, OnUpdateMenuCommand)
   ON_UPDATE_COMMAND_UI_RANGE(IDM_CUSTOMMENU_BASE, IDM_CUSTOMMENU_LAST, OnUpdateCustomMenuCommand)
   ON_UPDATE_COMMAND_UI_RANGE(IDM_WINDOWS_BASE, IDM_WINDOWS_LAST, OnUpdateWindows)

   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0x0000, 0xffff, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0x0000, 0xffff, OnToolTipText)

   ON_REGISTERED_MESSAGE(WM_SETAPPVALIDATE, OnSetAppValidate)
#ifdef _DEBUG
   ON_REGISTERED_MESSAGE(WM_FATALEXIT, OnFatalExit)
#endif
#ifdef _TEST
    ON_REGISTERED_MESSAGE(WM_SETPROP, OnSetProp)
    ON_REGISTERED_MESSAGE(WM_GETPROP, OnGetProp)
   ON_REGISTERED_MESSAGE(WM_HEAPUSE, OnHeapUse)
    ON_REGISTERED_MESSAGE(WM_TESTDOCKABLE, OnTestDockable)

   // Project state
   ON_REGISTERED_MESSAGE(WM_SETITEMPROP, OnSetItemProp)
    ON_REGISTERED_MESSAGE(WM_GETITEMPROP, OnGetItemProp)
   ON_REGISTERED_MESSAGE(WM_GETPROJPATH, OnGetProjPath)

   // Build state
   ON_REGISTERED_MESSAGE(WM_CANWEBUILD, OnIsBuildPossible)
   ON_REGISTERED_MESSAGE(WM_GETERRCOUNT , OnGetErrorCount)
   ON_REGISTERED_MESSAGE(WM_GETWARNCOUNT, OnGetWarningCount)
#endif
   ON_REGISTERED_MESSAGE(WM_POST_ENABLE_IME, OnPostEnableIME)
   ON_REGISTERED_MESSAGE(DSM_BARSIZECHANGING, OnBarSizeChanging)
   ON_REGISTERED_MESSAGE(DSM_TESTMENU, OnTestMenu)

   ON_REGISTERED_MESSAGE(MSG_MOUSEWHEEL, OnRegMouseWheel)

   ON_MESSAGE(WU_FILECHANGE, OnFileChange)
END_MESSAGE_MAP()

#ifdef _DEBUG
BOOL bQuitting = FALSE;
#endif

void CMainFrame::OnDirChange()
{
   for (CPartFrame* pMDIChild = (CPartFrame*) MDIGetActive(); pMDIChild != NULL;
      pMDIChild = (CPartFrame*) pMDIChild->GetWindow(GW_HWNDNEXT))
   {
      if(!IsValidMDICycleMember(pMDIChild))
         continue;

      ASSERT(pMDIChild->IsKindOf(RUNTIME_CLASS(CPartFrame)));
      pMDIChild->OnUpdateFrameTitle(TRUE);
   }
}

void SetModeName(UINT nModeNameStringID)
{
   CString str;
   if (nModeNameStringID != 0)
      str.LoadString(nModeNameStringID);
   SetModeName(str);
}

void SetModeName(LPCTSTR szModeName)
{
   if (theApp.m_pMainWnd)
   {
      if (((CMainFrame*)theApp.m_pMainWnd)->m_strModeName == szModeName)
         return;

      ((CMainFrame*)theApp.m_pMainWnd)->m_strModeName = szModeName;
      ((CMainFrame*)theApp.m_pMainWnd)->OnUpdateFrameTitle(TRUE);
   }
}

void SetSubTitle(LPCTSTR szSubTitle)
{
   if (theApp.m_pMainWnd)
   {
      if (((CMainFrame*)theApp.m_pMainWnd)->m_strSubTitle == szSubTitle)
         return;

      ((CMainFrame*)theApp.m_pMainWnd)->m_strSubTitle = szSubTitle;
      ((CMainFrame*)theApp.m_pMainWnd)->OnUpdateFrameTitle(TRUE);
   }
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// Don't update the title if we are loading a workspace.
	if (CPartFrame::IsWorkspaceLocked())
		return;

	CString strTitleBar;
	strTitleBar.Empty();

	// if a workspace is open, its name goes first in the title
	LPPROJECTWORKSPACE lpWorkspaceWnd = g_IdeInterface.GetProjectWorkspace();
	if (lpWorkspaceWnd != NULL)
	{
		CString strDocTitle;
		LPCSTR pszDocTitle;
		if (SUCCEEDED(lpWorkspaceWnd->GetWorkspaceDocTitle(&pszDocTitle)))
		{
			CPath path;
			path.Create(pszDocTitle);
			path.GetBaseNameString(strDocTitle);
			strTitleBar += strDocTitle;
		}
	}

	// now add the first part of title loaded at time of frame creation
	if (!strTitleBar.IsEmpty())
		strTitleBar += _T(" - ");
	strTitleBar += m_strTitle;

	if (!m_strSubTitle.IsEmpty())
	{
		ASSERT(!strTitleBar.IsEmpty());
		strTitleBar += _T(" - ");
		strTitleBar += m_strSubTitle;
	}

	if (!m_strModeName.IsEmpty())
	{
		ASSERT(!strTitleBar.IsEmpty());
		strTitleBar += _T(" [");
		strTitleBar += m_strModeName;
		strTitleBar += _T("]");
	}

	if (strTitleBar.CompareNoCase(m_strAppTitle) != 0)
	{
		m_strAppTitle = strTitleBar;
		SetWindowText(strTitleBar);
	}
}

void CMainFrame::RecalcLayout(BOOL bNotify /*=TRUE*/)
{
    if (m_bLockLayout)
                return;

        if (m_bInRecalcLayout)
                return;

        m_bInRecalcLayout = TRUE;
        // clear idle flags for recalc layout if called elsewhere
        if (m_nIdleFlags & idleNotify)
                bNotify = TRUE;
        m_nIdleFlags &= ~(idleLayout|idleNotify);

        // call the layout hook -- OLE support uses this hook
        if (bNotify && m_pNotifyHook != NULL)
                m_pNotifyHook->OnRecalcLayout();

        // reposition all the child windows (regardless of ID)
        if (GetStyle() & FWS_SNAPTOBARS)
        {
                CRect rect(0, 0, 32767, 32767);
                RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery,
                        &rect, &rect, FALSE);
                RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposExtra,
                        &m_rectBorder, &rect, TRUE);
                CalcWindowRect(&rect);
                SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(),
                        SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
        }
        else
        {
                CRect rect, empty;
                empty.SetRectEmpty();
                GetClientRect(&rect);
                // If in doc object mode, we want the status bar
                // at the bottom of the screen...
                if(m_pManager && m_pManager->IsInDocObjectMode())
                {
                        AFX_SIZEPARENTPARAMS layout;
                        layout.bStretch = TRUE;
                        layout.sizeTotal.cx = layout.sizeTotal.cy = 0;
                        layout.rect = rect;
                        layout.hDWP = ::BeginDeferWindowPos(1);
                        ::SendMessage(m_statusBar.m_hWnd, WM_SIZEPARENT, NULL, (LPARAM)&layout);
                        ::EndDeferWindowPos(layout.hDWP);
                        rect = layout.rect;
                }
                rect.left += m_rectBorder.left;
                rect.top += m_rectBorder.top;
                rect.right -= m_rectBorder.right;
                rect.bottom -= m_rectBorder.bottom;
                RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposExtra, &empty, &rect);
        }
        m_bInRecalcLayout = FALSE;

}

BOOL CMainFrame::NegotiateBorderSpace(UINT nBorderCmd, LPRECT lpRectBorder)
{
        switch(nBorderCmd)
        {
        case borderGet:
                ASSERT(lpRectBorder != NULL);
                DkShowManager(FALSE, updDelay);
                RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery,
                        lpRectBorder);  
                DkShowManager(TRUE, updDelay);
                break;
        case borderRequest:
                return TRUE;
                // Disallow the bottom border (causes problems with the status bar)
                if(lpRectBorder->bottom != 0)
                        return FALSE;
                break;

        default:
                return CFrameWnd::NegotiateBorderSpace(nBorderCmd, lpRectBorder);
        }

        return TRUE;
}


BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra,
   AFX_CMDHANDLERINFO* pHandlerInfo)
{
   BOOL bHandled = FALSE;

   // test for active browser popup...
   CWnd * pWndActiveView = GetActiveWindow();
   BOOL fBrowseWindow = pWndActiveView && pWndActiveView->IsKindOf(RUNTIME_CLASS(CFloatingFrameWnd));

   // If there's not an active dockworker or browser window, we can let the normal thing happen.
   if (CDockWorker::s_pActiveDocker == NULL && !fBrowseWindow)
      bHandled = CMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
   else
   {
      // Otherwise, we need some special routing.

      // establish pWndActiveView if necessary...
      if (!fBrowseWindow)
         pWndActiveView = CWnd::FromHandlePermanent(theApp.GetActiveView());

      // then try to handle the command message
      if (pWndActiveView && pWndActiveView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
            bHandled = TRUE;

      if (!bHandled)
         bHandled = CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
   }

   // If we have a toolbar customizer, then disable all commands, but not
   // status bar indicators.  We do this last to allow the normal routing
   // to SetText, SetCheck, etc.

   // Additionally, during customisation we allow the customizer to provide commands. 
   // This is done to support the right button menu on the toolbar or menu for button 
   // actions. martynl 25Mar96, revised 23Jun96

   if (CASBar::s_pCustomizer != NULL) 
   {
                if(nID >= IDM_BUTTON_BASE && nID <= IDM_BUTTON_LAST)
                {
                        // allow button commands to handled by the selected toolbar
                        ASSERT(CASBar::s_pCustomizer->m_pSelectBar!=NULL);
                        
                        bHandled = CASBar::s_pCustomizer->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
                }
                else
                {
                        if (nCode == CN_UPDATE_COMMAND_UI &&
                                (nID < IDM_DOCKSHOW_BASE || nID > IDM_DOCKSHOW_LAST) &&
                                nID != ID_HELP && !IsStatusBarIndicator(nID))
                        {
                                CCmdUI* pCmdUI = (CCmdUI*) pExtra;

								// Menus control their own destiny, even in customisation mode
								CTE *pCTE=theCmdCache.GetCommandEntry(nID);
								if(pCTE && (pCTE->flags & CT_MENU)!=0)
								{
									return TRUE;
								}

                                if (nID != ID_FILE_MRU_FILE1 && nID != ID_PROJ_MRU_FILE1)
                                        pCmdUI->Enable(FALSE);
                                else
                                {
                                        // MFC MRU handled on one CmdMsg.
                                        for (int iMRU = 0; iMRU < _AFX_MRU_COUNT; iMRU++)
                                        {
                                                BOOL bDone=FALSE;
                                                if(CBMenuPopup::IsCBMenuCmdUI(pCmdUI))
                                                {
                                                        CBMenuPopup* pMenu = (CBMenuPopup*)pCmdUI->m_pOther ;
                                                        if(pMenu)
                                                        {
                                                                pMenu->EnableMenuItem(nID + iMRU, MF_GRAYED | MF_BYCOMMAND);
                                                                bDone=TRUE;
                                                        }
                                                }
                                                
                                                if(!bDone)
                                                {
                                                        if(pCmdUI->m_pMenu)
                                                        {
                                                                pCmdUI->m_pMenu->EnableMenuItem(nID + iMRU,     MF_GRAYED | MF_BYCOMMAND);
                                                        }
                                                }
                                        }
                                }
                                return TRUE;
                        }
                }
   }
   return bHandled;
}

static BOOL bControlMenuSelected = FALSE;

void CMainFrame::OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu )
{
   // HACKS: We save this time for DBC property page activation, and
   // our own maximized MDI child close on double-click.  For the MDI child
   // closing, we also set a flag that tells us if the last popup menu
   // selected was the control menu of maximized MDI child.

   theApp.m_dwMenuSelectTime = GetCurrentMessage()->time;

   BOOL bMax = FALSE;
   MDIGetActive(&bMax);

   if (bMax && hSysMenu == ::GetMenu(m_hWnd) && nItemID == 0)
      bControlMenuSelected = TRUE;
   else if (hSysMenu != NULL && (nFlags & MF_POPUP) != 0)
      bControlMenuSelected = FALSE;

   // The shell now supports up to 32 windows in the Windows menu.
   // This means that we can no longer use AFX_IDM_FIRST_MDICHILD to
   // AFX_IDM_FIRST_MDICHILD+10 as the menu ids. One consequence is
   // that we no longer display the status bar text. This little bit 
   // of code here translates one of our windows menu items into 
   // a system windows menu so that we can get the status bar text.
   if (( nFlags != 0xFFFF) && // Menu is not closing.
           !(nFlags & (MF_SEPARATOR|MF_POPUP)) && // not a separator or popup.
           (nItemID >= IDM_WINDOWS_BASE) && (nItemID <= IDM_WINDOWS_LAST)) // Its one of the Windows menu items.
   {
                nItemID = AFX_IDM_FIRST_MDICHILD ;
   }

   CMDIFrameWnd::OnMenuSelect( nItemID, nFlags, hSysMenu );
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
   // If the IDE is iconized then we had better restore it.

   if (IsIconic())
   {
      if (theApp.m_bMaximizeApp)
         ShowWindow(SW_SHOWMAXIMIZED);
      else
         ::OpenIcon(GetSafeHwnd());
   }

   // This code was copied from the base class implementation (CFrameWnd::OnDropFiles).
   // The first line of the base class version (SetActiveWindow) was removed, because it appears
   // to be a bug in MFC.
   // SetActiveWindow causes windows to place our window into a partially active state, and hence
   // we don't get the focus correctly when the user switches to us later on.
   // Once MFC gets fixed, we should go back to calling the base class. martynl 20Mar96
        UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

        CWinApp* pApp = AfxGetApp();
        for (UINT iFile = 0; iFile < nFiles; iFile++)
        {
                TCHAR szFileName[_MAX_PATH];
                ::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
                // set a flag so that we can find out if we were opened using drag-drop or not
                theApp.m_bOpenedFromDrop = TRUE;
                pApp->OpenDocumentFile(szFileName);
                // reset the flag
                theApp.m_bOpenedFromDrop = FALSE;
        }
        ::DragFinish(hDropInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CFullScreenState construction/destruction

_TCHAR CFullScreenState::szKeyName[] = "FullScreen";

CFullScreenState::CFullScreenState ()
{
   m_bFirstTime   = TRUE;
   m_bFullScreen  = FALSE;
   m_bFSWasZoomed = FALSE;
   m_bFSWasIconic = FALSE;
   m_bStatusBar   = (BYTE)theApp.m_bStatusbar;
   m_bFSStatusBar = FALSE;
   m_bFSVertScroll   = FALSE;
   m_bFSHorzScroll   = FALSE;

   ::GetWindowRect (::GetDesktopWindow (), &m_FullScreenWindowRect);
   m_wpFSPrevWinState.length = sizeof m_wpFSPrevWinState;
}

void CFullScreenState::LoadFromReg (LPCTSTR szSection)
{
   HGLOBAL hGlobalDefault = ::GlobalAlloc(GMEM_MOVEABLE, sizeof *this + sizeof WORD);

   if (hGlobalDefault != NULL)
   {
      WORD * pw = (WORD *) ::GlobalLock(hGlobalDefault);
      *pw = sizeof *this;
      operator>> ((BYTE*)(pw + 1));
      ::GlobalUnlock (hGlobalDefault);

      HGLOBAL hGlobalReg = GetRegData (szSection, szKeyName, hGlobalDefault);

      if (hGlobalReg != hGlobalDefault)
      {
         ::GlobalFree (hGlobalDefault);

         pw = (WORD *) ::GlobalLock(hGlobalReg);
         ASSERT (*pw == sizeof *this);
         operator<< ((BYTE *)(pw + 1));
         ::GlobalUnlock (hGlobalReg);
      }

      ::GlobalFree (hGlobalReg);
   }

}

void CFullScreenState::SaveToReg (LPCTSTR szSection)
{
   HGLOBAL hGlobalReg = ::GlobalAlloc(GMEM_MOVEABLE, sizeof *this + sizeof WORD);

   if (hGlobalReg != NULL)
   {
      WORD * pw = (WORD *) ::GlobalLock(hGlobalReg);
      *pw = sizeof *this;
      operator>> ((BYTE*)(pw + 1));
      ::GlobalUnlock (hGlobalReg);

      (void)WriteRegData (szSection, szKeyName, hGlobalReg);

      ::GlobalFree (hGlobalReg);
   }

}

const CFullScreenState& CFullScreenState::operator<< (BYTE * pbSrc)
{
   m_bFullScreen  = *pbSrc;
   m_bFSWasZoomed = (*pbSrc >> 1);
   m_bFSWasIconic = (*pbSrc >> 2);
   m_bStatusBar   = (*pbSrc >> 3);
   m_bFSStatusBar = (*pbSrc >> 4);
   m_bFSVertScroll   = (*pbSrc >> 5);
   m_bFSHorzScroll   = (*pbSrc >> 6);
   m_bFirstTime   = (*pbSrc >> 7);

   memcpy (&m_FullScreenWindowRect, ++pbSrc, sizeof m_FullScreenWindowRect);
   pbSrc += sizeof m_FullScreenWindowRect;

   memcpy (&m_wpFSPrevWinState, pbSrc, sizeof m_wpFSPrevWinState);
   return *this;
}

const CFullScreenState& CFullScreenState::operator>> (BYTE * pbDest)
{
   // Never save as full screen mode.  This has proven to be
   // really frustrating to users who can not figure out how
   // to get out of full screen mode, but can at least shut down
   // the application.  If we save the state, then they are back
   // to the same confusion when they restart the application.

   *pbDest++ =    FALSE /*m_bFullScreen*/
            |  (m_bFSWasZoomed << 1)
            |  (m_bFSWasIconic << 2)
            |  (m_bStatusBar << 3)
            |  (m_bFSStatusBar << 4)
            |  (m_bFSVertScroll << 5)
            |  (m_bFSHorzScroll << 6)
            |  (m_bFirstTime << 7);

   memcpy (pbDest, &m_FullScreenWindowRect, sizeof m_FullScreenWindowRect);
   pbDest += sizeof m_FullScreenWindowRect;

   memcpy (pbDest, &m_wpFSPrevWinState, sizeof m_wpFSPrevWinState);

   return *this;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction


CMainFrame::CMainFrame()
{
   m_bInMenuMode = FALSE;
   m_pManager = NULL;
    m_lastWindowRect = rectDefault;

   m_bLockLayout = FALSE;
   m_bLockManager = FALSE;

   m_nManagerState = 0;
   m_nManagerStateSave = 0;
   for (int i = 0; i < MANAGER_STATES; i++)
      m_ahManagerStates[i] = NULL;
   m_dragDrop = NULL;
   m_hwndAppWiz = NULL;
        m_pIPCmpMainFrm = new CIPCompMainFrame; 

   m_pMacroRecorder = NULL;
   m_pRecorderForShell = NULL;
   m_fInOnEnterState = FALSE;
}


CMainFrame::~CMainFrame()
{
   for (int i = 0; i < MANAGER_STATES; i++)
   {
      if (m_ahManagerStates[i] != NULL)
         ::GlobalFree(m_ahManagerStates[i]);
   }
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

int CMainFrame::OnCreate(LPCREATESTRUCT lpcs)
{
    if (CMDIFrameWnd::OnCreate(lpcs) == -1)
        return -1;

   m_pWndMDIClient = new CMDIClientValidator;
   VERIFY(m_pWndMDIClient->SubclassWindow(m_hWndMDIClient));

   if (!m_statusBar.Create(this) ||
      !m_statusBar.SetIndicators())
   {
      TRACE("Failed to create status bar\n");
      return -1;     // fail to create
   }

   // Put MDIClient on the bottom.
   m_pWndMDIClient->SetWindowPos(&m_statusBar, 0, 0, 0, 0,
      SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

   return 0;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
        if (!IsFullScreen())
        {
        switch (nType)
        {
        case SIZE_RESTORED:
            GetWindowRect(&m_lastWindowRect);
            theApp.m_bMaximizeApp = FALSE;
            break;

        case SIZE_MINIMIZED:
         if (theApp.m_fOEM_MBCS)
            imeMoveConvertWin(m_hWnd, cx, cy);
            break;

        case SIZE_MAXIMIZED:
            theApp.m_bMaximizeApp = TRUE;
            break;
        }
        }

        CMDIFrameWnd::OnSize(nType, cx, cy);
    imeFlush( m_hWnd );

        CWnd *pView = CWnd::FromHandle(theApp.GetActiveView());
    if (pView && pView->IsKindOf( RUNTIME_CLASS(CPartView) ))
    {
                ((CPartView *)pView)->RecalculateIME();
    }
}

void CMainFrame::OnMove(int x, int y)
{
    CMDIFrameWnd::OnMove(x, y);
    if (!IsZoomed() && !IsIconic() && !IsFullScreen())
        GetWindowRect(&m_lastWindowRect);

    imeFlush( m_hWnd );

      CWnd *pView = CWnd::FromHandle(theApp.GetActiveView());
      if (pView && pView->IsKindOf( RUNTIME_CLASS(CPartView) ))
      {
         ((CPartView *)pView)->RecalculateIME();
      }
}

void CMainFrame::OnClose()
{
   // If we are in customize mode, don't close.
   if (CASBar::s_pCustomizer != NULL)
      return;

   // Note: only queries the active document
   CDocument* pDocument = GetActiveDocument();
   if (pDocument != NULL && !pDocument->CanCloseFrame(this))
   {
      // document can't close right now -- don't close it
      return;
   }

   // Give each package a chance to veto the exit...
   POSITION pos = theApp.m_packages.GetHeadPosition();
   while (pos != NULL)
   {
      CPackage* pPackage = (CPackage*)theApp.m_packages.GetNext(pos);
      if ((pPackage->m_flags & PKS_QUERYEXIT) != 0 && !pPackage->CanExit())
         return;
   }

   // try to save all documents
   if (pDocument == NULL && !AfxGetApp()->SaveAllModified())
      return;     // don't close it

   // Any code added after here must quit. All chances to abort the exit must go before here
   // inform any event subscribers that we are definitely going away.
   theApp.m_pAutoApp->FireBeforeApplicationShutDown();

   // All this needs to happen before OnDestroy, because popups
   // get destroyed before their owners.
   if (IsFullScreen())
   {
      OnWindowFullScreen();
      m_FS.m_bFullScreen = TRUE; // HACK!
   }

   if (!theApp.m_bInvokedCommandLine)
   {
		SaveLayout();
		theApp.SavePrivateProfileSettings();

	   // BLOCK: Save all the keymaps...
	   {
		  POSITION pos = CKeymap::c_keymaps.GetHeadPosition();
		  while (pos != NULL)
		  {
			 CKeymap* pKeymap = (CKeymap*)(void*)CKeymap::c_keymaps.GetNext(pos);
			 pKeymap->WriteReg();
		  }
	   }
   }

   CDockWorker::s_pActiveDocker = NULL;   // Make sure we don't try to activate.

   // hide the application's windows before closing all the documents
   ShowOwnedPopups(FALSE); // And the popup windows. (MFC should do this)
   AfxGetApp()->HideApplication();

   // [dolphin #9854 7/5/94 mikemo]
   // This mysteriously fixes the problem where, when you shut down
   // the IDE and the IDE's window disappears, the app behind the
   // IDE gets focus for a moment, and then for some reason loses
   // focus again.  This seems to be caused by the SetFocus in
   // CView::OnActivateView.  That doesn't make sense to me, because
   // in this scenario SetFocus is called when the IDE is invisible;
   // and besides that, I didn't think SetFocus was supposed to bring
   // an app to the foreground.  But in any case, disabling the main
   // frame causes the later SetFocus to fail, and thus the focus
   // problem goes away.
   m_pWndMDIClient->EnableWindow(FALSE);

   DestroyPropertyBrowser();

   // destroy the dock manager to close its documents.
    delete m_pManager;
    m_pManager = NULL;

#ifdef _DEBUG
   bQuitting = TRUE;
#endif

   // close all documents first
   AfxGetApp()->CloseAllDocuments(FALSE);

   // [dolphin #9854 7/5/94 mikemo]
   // To prevent a memory leak in MFC, we now have to re-enable the window
   //EnableWindow(TRUE);

   // Release all package interfaces
   if (m_pMacroRecorder != NULL)
                m_pMacroRecorder->Release();
   m_pMacroRecorder = NULL;
   if (m_pRecorderForShell != NULL)
                m_pRecorderForShell->Release();
   m_pRecorderForShell = NULL;

   // Packages which hold on to OLE objects that lock the app
   //  need to be told that we're exiting now, so they
   //  can release those objects, and MFC's CMDIFrameWnd::OnClose()
   //  will correctly destroy the window, thus calling our CMainFrame::
   //  DestroyWindow() which unloads the packages and calls their
   //  CPackage::OnExit().
   pos = theApp.m_packages.GetHeadPosition();
   while (pos != NULL)
   {
      CPackage* pPackage = (CPackage*)theApp.m_packages.GetNext(pos);
      if ((pPackage->m_flags & PKS_PREEXIT) != 0)
                pPackage->OnPreExit();
   }

   // We zombify the Application's OLE Automation object here, so that
   //  it will do an AfxOleUnlockApp.  If we don't, MFC will not call
   //  our CMainFrame::DestroyWindow(), and we'll never exit.
   theApp.m_pAutoApp->Zombify();
   theApp.m_pAutoApp->Destroy();
   theApp.m_bQuitting = TRUE;

   CMDIFrameWnd::OnClose();
}

BOOL CMainFrame::DestroyWindow()
{
   // Make absolutely certain the Application's OLE Automation object is
   //  zombified by now, in case we get here without an intervening
   //  CMainFrame::OnClose call.  Zombifying does an AfxOleUnlockApp.
   if (theApp.m_pAutoApp != NULL)
                theApp.m_pAutoApp->Zombify();

   // Release all package interfaces (if there was an intervening CMainFrame::
   //  OnClose call, this was already done).
   if (m_pMacroRecorder != NULL)
                m_pMacroRecorder->Release();
   m_pMacroRecorder = NULL;
   if (m_pRecorderForShell != NULL)
                m_pRecorderForShell->Release();
   m_pRecorderForShell = NULL;

   // Close the packages...
   // Packages may supply popups owned by the main frame (Prop Browser).
   //
   POSITION pos = theApp.m_packages.GetHeadPosition();
   while (pos != NULL)
   {
      CPackage *pPackage = (CPackage*)theApp.m_packages.GetNext(pos);
      pPackage->OnExit();
   }

   /*if (theApp.m_pAutoApp != NULL)
   {
      theApp.m_pAutoApp->Destroy();
      theApp.m_pAutoApp = NULL;
   }*/

        // shut down Component manager
        theApp.TermStdCompMgr();
        m_pIPCmpMainFrm->InternalRelease();
                
        return CMDIFrameWnd::DestroyWindow();
}

void CMainFrame::RecordDefaultTextIfNecessary(WORD nID)
{
	// This should only be called if we're currently recording,
	//  which should only happen if devaut1.pkg is loaded,
	//  which implies we have these interfaces.
	ASSERT (m_pRecorderForShell != NULL && m_pMacroRecorder != NULL);
	
	BOOL bWasTextRecorded = TRUE;
	m_pRecorderForShell->EndListenForRecord(&bWasTextRecorded);
	if (!bWasTextRecorded)
	{
		// Nothing was recorded, so try to record default text
		LPCTSTR szCommand = NULL;
		if (ID_PROJ_MRU_FIRST <= nID && nID <= ID_PROJ_MRU_LAST)
		{
			// SPECIAL CASE: Opening a project off of the MRU list results
			//  in a bogus command name from the CTE, so handle that
			//  case manually.  Instead of hard-coding the command string,
			//  we just find the CTE for WorkspaceOpen and use its cmd string
			CTE* pCTE = theCmdCache.GetCommandEntry(IDM_OPEN_WORKSPACE);
			if (pCTE == NULL)
			{
				// Someone recorded opening a workspace from the MRU, but
				//  the OpenWorkspace CTE isn't around?!
				ASSERT(FALSE);
				return;
			}
			else
				szCommand = pCTE->szCommand;
		}
		else
		{
			// No special case here.  Just record the text in the
			//  CTE we get from the cmd ID.
			CTE* pCTE = theCmdCache.GetCommandEntry(nID);
			if (pCTE != NULL)
				szCommand = pCTE->szCommand;
			else
				return;
		}
		ASSERT (szCommand != NULL && *szCommand != '\0');
  		CString strText;
		strText.Format("ExecuteCommand \"%s\"", szCommand);
		m_pMacroRecorder->RecordText(strText, "Shl");
	}
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
   // test for active browser popup...
   CWnd * pWndActiveView = GetActiveWindow();
   BOOL fBrowseWindow = pWndActiveView && pWndActiveView->IsKindOf(RUNTIME_CLASS(CFloatingFrameWnd));
   void *pv;
   UINT cRep = 1;
   BOOL bRet = FALSE;

   BOOL bShouldRecordText = theApp.ShouldRecordText();

   if (theApp.m_RepeatableCmds.Lookup(LOWORD(wParam), pv))
      cRep = max(1, theApp.GetRepCount());

   for (UINT i = 0; i < cRep; i++)
   {
      if (bShouldRecordText)
          {
                 // Shouldn't be in a recording session if devaut1.pkg isn't loaded
                 ASSERT (m_pRecorderForShell != NULL);
         m_pRecorderForShell->BeginListenForRecord();
          }

      if (fBrowseWindow)
      {
         // dispatch the command message to the browser popup first...
         if (AfxWndProc(pWndActiveView->m_hWnd, WM_COMMAND, wParam, lParam))
            bRet = TRUE;
      }
      else if (CDockWorker::s_pActiveDocker != NULL)
      {
         HWND hWndActiveView = theApp.GetActiveView();

         if (CWnd::FromHandlePermanent(hWndActiveView) != NULL &&
            AfxWndProc(hWndActiveView, WM_COMMAND, wParam, lParam) != 0)
               bRet = TRUE;
      }
      else
      {
         // send to MDI child first - will be re-sent through OnCmdMsg later
         CMDIChildWnd* pActiveChild = MDIGetActive();
         if (pActiveChild != NULL && AfxWndProc(pActiveChild->m_hWnd,
           WM_COMMAND, wParam, lParam) != 0)
            bRet = TRUE; // handled by child
      }

          if (bShouldRecordText && bRet == TRUE)
          {
                  // We're recording and the command was handled.  Did the
                  //  package record anything?
                  RecordDefaultTextIfNecessary(LOWORD(wParam));
          }
   }

   if (LOWORD(wParam) < MIN_ID_REPEAT_CMD || LOWORD(wParam) > MAX_ID_REPEAT_CMD)
      theApp.ClearRepCount();

   if (bRet)
      return TRUE;

   if (bShouldRecordText)
   {
          ASSERT (m_pRecorderForShell != NULL);
          m_pRecorderForShell->BeginListenForRecord();
   }

   if (LOWORD(wParam) == ID_HELP)
   {
      if (IsTracking())
         return TRUE; // disable menu help

      // Delegate all help requests to AppWizard if it's around
      if (m_hwndAppWiz != NULL && HIWORD(wParam) == BN_CLICKED)
      {
         ::SendMessage(m_hwndAppWiz, WM_COMMAND, wParam, lParam);

                  if (bShouldRecordText)
                          RecordDefaultTextIfNecessary(LOWORD(wParam));

         return TRUE;
      }
   }

   if (CFrameWnd::OnCommand(wParam, lParam))
   {
      if (bShouldRecordText)
                  RecordDefaultTextIfNecessary(LOWORD(wParam));

      return TRUE; // handled through normal mechanism (MDI child or frame)
   }

   HWND hWndCtrl = (HWND)lParam;

   ASSERT(AFX_IDM_FIRST_MDICHILD == 0xFF00);
   if (hWndCtrl == NULL && (LOWORD(wParam) & 0xf000) == 0xf000)
   {
      // menu or accelerator within range of MDI children
      // default frame proc will handle it
      DefWindowProc(WM_COMMAND, wParam, lParam);

      // DefWindowProc doesn't expect dockable windows, so make sure
      // one is not still active.
      if (CDockWorker::s_pActiveDocker != NULL)
         SetFocus();

      if (bShouldRecordText)
             RecordDefaultTextIfNecessary(LOWORD(wParam));

          return TRUE;
   }

   if (bShouldRecordText)
   {
           // Nothing recorded, so reset macro recorder for shell
           BOOL bDummy;
           m_pRecorderForShell->EndListenForRecord(&bDummy);
   }

   return FALSE;   // not handled
}

// CTestUI
//
// A miniature CCmdUI class which is used to determine whether a
// given submenu should be enabled or disabled (i.e. whether there
// are any enabled commands on the submenu).
//
class CTestUI : public CCmdUI
{
public:
   CTestUI();

   virtual void Enable(BOOL bEnabled = TRUE);
   virtual void SetCheck(int nCheck = 1);   // 0, 1 or 2 (indeterminate)
   virtual void SetRadio(BOOL bOn = TRUE);
   virtual void SetText(LPCSTR lpszText);

   BOOL m_bEnabled;
};

CTestUI::CTestUI()
{
   m_bEnabled = FALSE;
}

void CTestUI::Enable(BOOL bEnabled)
{
   m_bEnabled = m_bEnabled || bEnabled;
}

void CTestUI::SetCheck(int nCheck)
{ }

void CTestUI::SetRadio(BOOL bOn)
{ }

void CTestUI::SetText(LPCSTR lpszText)
{
   m_bEnabled = TRUE;
}

void CMainFrame::OnInitMenu(CMenu* pMenu)
{
   EnsureViewActive();  // Don't let toolbars stay active.

   CMDIFrameWnd::OnInitMenu(pMenu);
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT, BOOL bSysMenu)
{
   // If this is a system menu, do the default thing.  In order to
   // determine whether it's a SysMenu, however, we also have to check
   // the id of the first item, since maximized MDI child windows will
   // not have bSysMenu set for their system menus.
   // (-1 for menuitem means sub popup) -bm
   int nItem = pPopupMenu->GetMenuItemID(0);
   if (bSysMenu || (nItem != -1 && nItem > 0xf000 && nItem < 0xff00))
   {
      pPopupMenu->EnableMenuItem(SC_CLOSE,
         (CASBar::s_pCustomizer == NULL) ? MF_ENABLED : MF_GRAYED);

      if (IsFullScreen())
      {
         pPopupMenu->EnableMenuItem(SC_MAXIMIZE, MF_GRAYED);
         pPopupMenu->EnableMenuItem(SC_MOVE, MF_GRAYED);
         pPopupMenu->EnableMenuItem(SC_SIZE, MF_GRAYED);
      }
      else
         ;  // Let the system enable/disable it.

      return;
   }

        CWnd* pWnd = CWnd::FromHandle(theApp.GetActiveView());
        int cItems = pPopupMenu->GetMenuItemCount();
        // also determine if this is the Window menu
        // and if the MDI window list is on the menu
        BOOL bWndMenu = FALSE;
        BOOL bMDIList = FALSE;
        for (int iItem = 0 ; iItem < cItems ; iItem++)
        {
                int nID = pPopupMenu->GetMenuItemID(iItem);
                // is this the Windows menu?
                // use ID_WINDOW_CASCADE to determine this
                if (nID == ID_WINDOW_CASCADE) {
                        bWndMenu = TRUE;
                }
                // is the MDI window list on the menu?
                // 0xff00 is the ID of the first MDI child window menu item
                if (nID == 0xff00) {
                        bMDIList = TRUE;
                }
        }

   // if this is the Window menu, remove More Windows...
   if (bWndMenu) {
      // (More Windows... has an ID of 0xff09)
      pPopupMenu->RemoveMenu(0xff09, MF_BYCOMMAND);
      pPopupMenu->RemoveMenu(ID_WINDOW_LIST_DOCS, MF_BYCOMMAND);
      // only add a separator if there's no MDI window list
      // and a separator hasn't already been added
      if (!bMDIList && pPopupMenu->GetMenuItemID(pPopupMenu->GetMenuItemCount()-1) != ID_SEPARATOR) {
         pPopupMenu->AppendMenu(MF_SEPARATOR);
      }
      // add the Doc List menu item to the end of the menu
      LPCTSTR lpszCommand;

      theCmdCache.GetCommandString(ID_WINDOW_LIST_DOCS, STRING_MENUTEXT,
         &lpszCommand);
      CString strMenuItem = lpszCommand;

      // get the accelerator for this item
      CString strKey;
      if (GetCmdKeyString(ID_WINDOW_LIST_DOCS, strKey)) {
         strMenuItem += CString(_T("\t")) + strKey;
      }
      // add the menu item to the menu
      pPopupMenu->AppendMenu(MF_ENABLED, ID_WINDOW_LIST_DOCS, strMenuItem);
   }

   // Default processing to send UPDATE_COMMAND_UI messages.
   CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, 0, bSysMenu);
}

int CMainFrame::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
   ASSERT(!bQuitting);

   if (theApp.m_hwndValidate != NULL &&
      ::SendMessage(theApp.m_hwndValidate, WM_ISERROR, TRUE, 0L))
   {
      return MA_NOACTIVATEANDEAT;
   }

   CDockWorker* pDocker = CDockWorker::s_pActiveDocker;
   if (pDocker != NULL && pDocker->GetDock() == dpNil)
   {
      ASSERT(pDocker->m_pWnd != NULL);

      if (nHitTest == HTMENU)
         CDockWorker::LockWorker();
      else if (CDockWorker::s_bLockDocker)
         pDocker->m_pWnd->SetFocus();
   }

   if (CASBar::s_pCustomizer != NULL)
   {
      if (theApp.m_bDeactByOtherApp)
      {
         EnsureCustomizerActive();
         return MA_NOACTIVATEANDEAT;
      }
      else if (nHitTest != HTSYSMENU && nHitTest != HTMENU)
      {
         return MA_NOACTIVATE;
      }
   }

   // Fix for Dolphin #626.  We want to make sure that clicking on the
   // caption/menu bar will activate Dolphin.  [w-amilt]
   switch (nHitTest)
   {
   case HTCAPTION:
      return MA_ACTIVATE;

   case HTMENU:
      return MA_ACTIVATE;

   default:
      break;
   }

   return CMDIFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CMainFrame::OnUpdateIndicator(CCmdUI* pCmdUI)
{
   switch (pCmdUI->m_nID)
   {
      case ID_SEPARATOR:
         pCmdUI->ContinueRouting();
         break;

      case ID_INDICATOR_OVR:
         pCmdUI->Enable(m_statusBar.GetOverStrike());
         break;

      default:
         {
            CString str;
            m_statusBar.GetText(pCmdUI->m_nID, str);
            // CStatusBar::SetPaneText has a 'feature' wherein it doesn't
            // recognize that successive calls with an empty string are the
            // same, so it continues to paint the empty string.
            //
            // So if we're about to set an empty string, make it a single
            // space character instead.
            pCmdUI->SetText(str.IsEmpty() ? " " : (LPCTSTR) str);
            pCmdUI->Enable(TRUE);
         }
         break;
   }
}

#ifdef _DEBUG
void CMainFrame::OnOpenLayout()
{
   SetDockState((m_nManagerState + 1) % MANAGER_STATES);
}
#endif

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
#ifdef _WIN32
   if (message == WM_CMDPERFCHECK)
   {
      if (wParam == 0 || LOWORD(wParam) == 0xFFFF)
         return 0L;     // ignore invalid IDs

      CWnd* pWnd = this;

      __int64 liBegin, liEnd;
      //LARGE_INTEGER liBegin, liEnd;
      QueryPerformanceCounter((LARGE_INTEGER*)&liBegin);

      for (int i = 0; i < 1000; i++)
      {
         CTestUI testUI;
         testUI.m_nID = wParam;

         if (pWnd->OnCmdMsg(testUI.m_nID, CN_UPDATE_COMMAND_UI, &testUI, NULL))
            ASSERT(!testUI.m_bEnableChanged); // not routed

         if (!testUI.m_bEnableChanged)
         {
            AFX_CMDHANDLERINFO info;
            info.pTarget = NULL;

            BOOL bHndler = pWnd->OnCmdMsg(testUI.m_nID, CN_COMMAND, &testUI, &info);
         }
      }

      QueryPerformanceCounter((LARGE_INTEGER*)&liEnd);
      liEnd = liEnd - liBegin;
      return LRESULT(liEnd);
      //liEnd = LargeIntegerSubtract(liEnd, liBegin);
      //return liEnd.LowPart;
   }
#endif
#endif

#ifdef _CMDLOG
   if (theApp.m_bCmdLog && (message == WM_COMMAND))
   {
      theApp.m_cmdTable.Update(LOWORD((DWORD)wParam));
   }
#endif // _CMDLOG

   return CMDIFrameWnd::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////

void SushiCancelModes(HWND hWndRcvr)
{
   extern void PASCAL AfxCancelModes(HWND hWndRcvr);

   // This is the sole reason for having CDropControl be dynamic
   CWnd* pWnd = CWnd::GetFocus();
   if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CDropControl)))
      ((CDropControl*)pWnd)->CancelModes();

   AfxCancelModes(hWndRcvr);
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// steal toolbar navigation keys during bar navigation mode
	if(CASBar::EarlyStealMenuMessage(pMsg))
	{
		return TRUE;
	}

   // check for special cancel modes for ComboBoxes
   CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);
   if ((pMsg->message == WM_LBUTTONDOWN ||   pMsg->message == WM_NCLBUTTONDOWN) &&
      !(pWnd->IsKindOf(RUNTIME_CLASS(CDropControl)) && GetFocus() == pWnd) &&
      !pWnd->IsKindOf(RUNTIME_CLASS(CGhostListBox)))
   {
      SushiCancelModes(pMsg->hwnd);    // filter clicks
   }

   // test for active browser popup...
   CWnd* pWndActiveView = GetActiveWindow();
   BOOL fBrowseWindow = pWndActiveView != NULL &&
      pWndActiveView->IsKindOf(RUNTIME_CLASS(CFloatingFrameWnd));

   // If there's not an active dock worker or browser, we can let the normal thing happen.
   if (CDockWorker::s_pActiveDocker == NULL && !fBrowseWindow)
   {
		//if (CMDIFrameWnd::PreTranslateMessage(pMsg))
		if (MDIFrameWnd_PreTranslateMessage(pMsg))
		{
			return TRUE;
		}
   }
   else
   {
      // Otherwise, we need some special routing.

      // establish current active view if necessary
      if (!fBrowseWindow)
	  {
		pWndActiveView = CWnd::FromHandlePermanent(theApp.GetActiveView());
	  }

      // If any window in the parent chain is disabled, then we do not
      // handle the message.  In case a dialog is active.

      for (CWnd* pWnd = pWndActiveView; pWnd != NULL; pWnd = pWnd->GetParent())
      {
		if (!pWnd->IsWindowEnabled())
			pWndActiveView = NULL;

		if (pWndActiveView == NULL || (pWnd->GetStyle() & WS_POPUP) != 0)
			break;
      }

      if (pWndActiveView != NULL && pWndActiveView->PreTranslateMessage(pMsg))
         return TRUE;

      if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
      {
			// the MDICLIENT window may translate it			
			if (::TranslateMDISysAccel(m_hWndMDIClient, pMsg))
			{
				return TRUE;
			}
      }
   }

   if (m_keymap.TranslateMessage(this, pMsg))
      return TRUE;


   // Are we entering a repeat count?
   if (theApp.FSettingRepeatCount(pMsg) && theApp.FCmdRepeatPretranslate(pMsg))
      return TRUE; // Handled

	// steal alt key combinations before anyone else gets a look in for the use of the menu bar
	if(CASBar::StealMenuMessage(pMsg))
	{
		return TRUE;
	}

	// Orion 96 Bug # 14688 
	// Fixes ALT-F4 when the mouse pointer is over a button.
	if (pMsg->message == WM_SYSCOMMAND && 
		pMsg->wParam == SC_CLOSE &&
		pMsg->hwnd == m_hWnd)
	{
		// The user is attempting to close the app possibly using ALT-F4.
		// If the menubar has the capture set, turn it off. Otherwise, windows
		// will not close the application. 
		// See the SC_CLOSE case in xxxSysCommand in syscmd.c.
		CWnd* pWndWithCapture = GetCapture() ;
		if (pWndWithCapture && pWndWithCapture->IsKindOf(RUNTIME_CLASS(CASBar)))
		{
			// Turn capture off.
			::ReleaseCapture() ;
		}
	}

   return FALSE;
}

///////////////////////////////////////////////////////////
//
// MDIFrameWnd_PreTranslateMessage
//
// This function was stolen directly from MFC. See the
// comment below about the Orion 96 Bug 14688.
//
BOOL CMainFrame::MDIFrameWnd_PreTranslateMessage(MSG* pMsg)
{
	// check for special cancel modes for ComboBoxes
	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_NCLBUTTONDOWN)
		AfxCancelModes(pMsg->hwnd);    // filter clicks

	// allow tooltip messages to be filtered
	if (CWnd::PreTranslateMessage(pMsg))
		return TRUE;

#ifndef _AFX_NO_OLE_SUPPORT
	// allow hook to consume message
	if (m_pNotifyHook != NULL && m_pNotifyHook->OnPreTranslateMessage(pMsg))
		return TRUE;
#endif

	CMDIChildWnd* pActiveChild = MDIGetActive();

	// current active child gets first crack at it
	if (pActiveChild != NULL && pActiveChild->PreTranslateMessage(pMsg))
		return TRUE;

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		// translate accelerators for frame and any children
		if (m_hAccelTable != NULL &&
			::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
		{
			return TRUE;
		}

		// special processing for MDI accelerators last
		// and only if it is not in SDI mode (print preview)
		if (GetActiveView() == NULL)
		{
			if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
			{
				// the MDICLIENT window may translate it

				// Orion 96 Bug 14688: TranslateMDISysAccel will not close a window (CTRL-F4)
				// if another window has captured the mouse. Fool it by turning capture off then
				// back on.
				// 
				BOOL bReDoCapture = FALSE ;
				CWnd* pWndWithCapture = GetCapture() ;
				if (pWndWithCapture && pWndWithCapture->IsKindOf(RUNTIME_CLASS(CASBar)))
				{
					// Turn capture off.
					bReDoCapture = TRUE;
					::ReleaseCapture() ;
				}
				BOOL b = ::TranslateMDISysAccel(m_hWndMDIClient, pMsg) ;				
				if (bReDoCapture)
				{
					// Turn capture on.
					pWndWithCapture->SetCapture() ;
				}
				if (b)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

//
//
//
BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
   if (CASBar::s_pCustomizer != NULL)
      bActive = FALSE;
   // NOTE: This doesn't work Alt-Tabbing back when a palette has the focus.
   //
   else if (!theApp.m_bDeactByOtherApp && IsWindowEnabled())
      bActive = TRUE;

   // CMDIFrameWnd just calls Default(), but we may have changed bActive.
   return (BOOL) DefWindowProc(WM_NCACTIVATE, bActive, 0L);
}

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask)
{
#ifdef _DEBUG
   ASSERT(!bQuitting || !bActive);
   bQuitting = FALSE;   // Stop us from showing continuous stream of asserts.
#endif

   theApp.m_fVCPPIsActive = bActive;
   theApp.m_bDeactByOtherApp = !bActive;

   CMDIFrameWnd::OnActivateApp(bActive, hTask);

   if (bActive)
      EnsurePopupActive();
   else
      CDockWorker::LockWorker();

   SendMessage(WM_NCACTIVATE);
   if (IsWindowVisible())
   {
      // Blows m_lastWindowRect so window comes up in the wrong position
      // if this happens before window is visible.
      SetWindowPos(NULL, 0, 0, 0, 0, SWP_DRAWFRAME |
         SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE);
   }

   if (IsWindowEnabled() && m_pManager != NULL)
      m_pManager->ActivatePalettes(bActive);

   // signal to other packages for OnIdle processing
   if (bActive)
      theApp.m_bActivated = TRUE;

   if(!bActive)
   {
	   if(!theApp.IsCustomizing())
	   {
		   // tell the first visible menu that it is done
			POSITION pos=CBMenuPopup::s_listMenus.GetHeadPosition();
			if(pos)
			{
                CBMenuPopup *pMenu=CBMenuPopup::s_listMenus.GetNext(pos);

				// Don't do anything if the menu is already in the process of shutting down
                if(pMenu && pMenu->ContinueModal())
                {
				   pMenu->Done();
				}
			}
	   }
   }
}

void CMainFrame::OnEnable(BOOL bEnable)
{
   CMDIFrameWnd::OnEnable(bEnable);

   // Make sure we show the right caption color, since DefWindowProc
   // will not do this for us, if a dockable window is active.
   // If the caller is entering a modal state through the Component Mgr,
   // we don't want to do this....
   if (!bEnable && !m_fInOnEnterState)
      SendMessage(WM_NCACTIVATE);

   if (m_pManager != NULL)
      m_pManager->ActivatePalettes(bEnable);

   PostMessage(WM_POST_ENABLE_IME, bEnable);
}

/* OnPostEnableIME
 * We want to turn the IME on when you open a modal dialog or a proppage/browser
 * but because we get disabled inside of the DoModal call, MFC's creation hook
 * is getting sidetracked by the creation of the IME.
 * The solution is to postpone enabling of the IME, using a PostMessage.
 * wParam is bEnable from OnEnable (i.e. FALSE == disable app == enable IME)
 */
LRESULT CMainFrame::OnPostEnableIME(WPARAM wParam, LPARAM lParam)
{
   // This bit of code turns the IME on when you open a modal dialog or a proppage/browser
   static BOOL bDisableIME;
   if( !wParam )
   {
      bDisableIME = !theIME.IsEnabled();
      theIME.EnableIME(TRUE);
   }
   else
   {
      if (bDisableIME)
         theIME.EnableIME(FALSE);
   }
   return 0;
}

/* OnRegMouseWheel
 * craigc
 * this message is the mouse wheel notification for win95
 * which does not support WM_MOUSEWHEEL.
 * we forward this to the guy with the focus
 */

LRESULT CMainFrame::OnRegMouseWheel(WPARAM wParam, LPARAM lParam)
{
    CWnd * pWnd;

    if (pWnd = CWnd::FromHandlePermanent(theApp.GetActiveView()))
    {
        return pWnd->SendMessage(MSG_MOUSEWHEEL,wParam,lParam);
    }
    else
        return 0;
}

void CMainFrame::OnWindowCloseAll()
{
   // First see if the user wants to cancel out of saving.
   // Filter project RC files, since closing their editors will not
   // close the file.
   if (!theApp.SaveAll(TRUE, FILTER_PROJECT | FILTER_PROJ_RC))
      return;

   // Next see if all windows can close.
   CPartFrame* pMDIChild = theApp.MDITypedGetActive(FILTER_PROJECT);
   while (pMDIChild != NULL)
   {
      CPartView* pView = (CPartView*)pMDIChild->GetActiveView();
      if (pView != NULL)
      {
         ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));
         if (!pView->CanClose())
            return;
      }

      pMDIChild = theApp.MDITypedGetNext(pMDIChild, FILTER_PROJECT);
   }

   CWaitCursor wc;

   // Close everything.
   CloseWindows(FILTER_PROJECT);
}

void CMainFrame::OnWindowClose()
{
        CMDIChildWnd *pActive=MDIGetActive();
        if(pActive)
        {
                pActive->SendMessage(WM_CLOSE,0,0);
        }
}

void CMainFrame::OnWindowNext()
{
        CPartFrame *pActive=(CPartFrame *)MDIGetActive();
        if(pActive)
        {
                pActive->MDINextWindow(TRUE, TRUE);
        }
}

void CMainFrame::OnWindowPrevious()
{
        CPartFrame *pActive=(CPartFrame *)MDIGetActive();
        if(pActive)
        {
                pActive->MDINextWindow(FALSE, TRUE);
        }
}

void CMainFrame::OnUpdateWindowCommand(CCmdUI* pCmdUI)
{
        if(MDIGetActive())
        {
                pCmdUI->Enable(TRUE);
        }
        else
        {
                pCmdUI->Enable(FALSE);
        }
}

///    CMainFrame::OnWindowFullScreen()
//
// "Window.Full Screen" menu option.
//
// Toggles full-screen mode.  FS mode means that the MDI client area is enlarged to
//  fill the whole screen.  This is accomplished by resizing the frame window to be larger
// than the screen.  Most of the work below serves to calculate what that size should be.
/////
void CMainFrame::OnWindowFullScreen ()
{
   if (IsFullScreen())
   {
      m_FS.m_bFullScreen = FALSE;

      m_FS.m_bFSStatusBar = (BYTE)theApp.m_bStatusbar;
      ShowStatusBar (theApp.m_bStatusbar = m_FS.m_bStatusBar);

      // If we are returning the app to the Maximized state, SetWindowPlacement()
      // draws the entire app twice - once to its "Normal" state, then once to
      // the maximized state.  The LockWindowUpdate() calls suppress the extra
      // redraw.

      ::LockWindowUpdate(m_hWnd);
      SetWindowPlacement ( &m_FS.m_wpFSPrevWinState );
      CFullScreenState::Notify ntfy;
      theApp.NotifyPackages (PN_FULL_SCREEN_END, NULL, &ntfy);
      SetDockState(~STATE_FULLSCREEN, MASK_FULLSCREEN);
      ::LockWindowUpdate(NULL);

      m_FS.m_bFSVertScroll = (BYTE)ntfy.bVert;
      m_FS.m_bFSHorzScroll = (BYTE)ntfy.bHorz;
   }
   else
   {
      m_FS.m_bFullScreen = TRUE;

      // We'll need these to restore the original state.
      GetWindowPlacement ( &m_FS.m_wpFSPrevWinState );

      // BOGUS! NT 3.51 sets the length field wrong sometimes.
      m_FS.m_wpFSPrevWinState.length = sizeof m_FS.m_wpFSPrevWinState;

      // "Full Screen" means that our client area is the same
      // size as the desktop.
#define SPI_GETWORKAREA            48

      RECT rectDesktop;
      if (!::SystemParametersInfo (SPI_GETWORKAREA, 0, &rectDesktop, 0))
      {
         ::GetWindowRect ( ::GetDesktopWindow(), &rectDesktop );
      }
      else
      {
         // In Win95, when the Task Bar is not in AutoHide mode, the
         // "screen" excludes the Task Bar.  When the Task Bar is at the left
         // or top, this means that our origin must be adjusted in the negative
         // direction by the size of the Task Bar.  SPI_GETWORKAREA provides us
         // with the rectangle for the the physical screen not occupied by the
         // Task Bar.
         rectDesktop.right = ::GetSystemMetrics (SM_CXSCREEN);
         rectDesktop.bottom = ::GetSystemMetrics (SM_CYSCREEN);
         rectDesktop.right -= rectDesktop.left;
         rectDesktop.bottom -= rectDesktop.top;
         rectDesktop.left = -rectDesktop.left;
         rectDesktop.top = -rectDesktop.top;
      }
      ::AdjustWindowRect (&rectDesktop, GetStyle(), !Menu::IsShowingCmdBars());

      rectDesktop.bottom += ::GetSystemMetrics (SM_CYBORDER);   // See "Ask Dr. GUI #10"

      // AdjustWindowRect() cannot adjust for the status bar, so we remove
      // it ourselves.
      //CRect rectStatusBar;
      //m_statusBar.GetWindowRect (&rectStatusBar);
      //rectDesktop.bottom += rectStatusBar.Height();
      m_FS.m_bStatusBar = (BYTE)theApp.m_bStatusbar;
      ShowStatusBar (theApp.m_bStatusbar = m_FS.m_bFSStatusBar);

      // Remember this so that OnGetMinMaxInfo() can set the correct
      // window max sizes.
      m_FS.m_FullScreenWindowRect = rectDesktop;

      // Now we make the change.
      WINDOWPLACEMENT wpNew = m_FS.m_wpFSPrevWinState;
      wpNew.showCmd =  SW_SHOWNORMAL;
      wpNew.rcNormalPosition = rectDesktop;
      ::LockWindowUpdate(m_hWnd);
      SetWindowPlacement ( &wpNew );

      CFullScreenState::Notify ntfy;
      ntfy.rectFullScreen = m_FS.m_FullScreenWindowRect;
      ntfy.bVert        = m_FS.m_bFSVertScroll;
      ntfy.bHorz        = m_FS.m_bFSHorzScroll;
      ntfy.bFirstTime      = m_FS.m_bFirstTime;
      theApp.NotifyPackages (PN_FULL_SCREEN, &ntfy);
      m_FS.m_bFirstTime = FALSE;
      // New layout.
      SetDockState(STATE_FULLSCREEN, MASK_FULLSCREEN);
      ::LockWindowUpdate(NULL);
   }
}


void CMainFrame::OnUpdateFullScreen(CCmdUI* pCmdUI)
{
   // This function handles the UI for both the "Windows.Full Screen"
   // menu item and the Full Screen button.  m_pMenu == NULL detects the
   // button.
#ifndef NO_CMDBARS
        if (CBMenuPopup::IsCBMenuCmdUI(pCmdUI))
        {
                //CBMenuPopup* pMenu = (CBMenuPopup*)pCmdUI->m_pOther ;
                pCmdUI->SetCheck (m_FS.m_bFullScreen);
        }
#else                                           
   if (pCmdUI->m_pMenu)
      pCmdUI->SetCheck (m_FS.m_bFullScreen);
#endif
}


/// CMainFrame::OnGetMinMaxInfo()
//
// When a window gets moved or resized, Windows queries the app via WM_GETMINMAXINFO
// about the max window size.  Since Full Screen mode needs a larger than normal
//  window size, we catch the query here.
/////
void CMainFrame::OnGetMinMaxInfo (MINMAXINFO * pMinMaxInfo)
{
   if (IsFullScreen()) {
      pMinMaxInfo->ptMaxSize.y = m_FS.m_FullScreenWindowRect.Height();
      pMinMaxInfo->ptMaxTrackSize.y = pMinMaxInfo->ptMaxSize.y;
      pMinMaxInfo->ptMaxSize.x = m_FS.m_FullScreenWindowRect.Width();
      pMinMaxInfo->ptMaxTrackSize.x = pMinMaxInfo->ptMaxSize.x;
      }
}

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
        static HCURSOR hcRecord = NULL;
        if (theApp.GetRecordState() == CTheApp::RecordOn)
        {
                // If we're recording, show the record cursor.  For better
                //  performance, cache the record cursor in hcRecord
                if (hcRecord == NULL)
                        hcRecord = AfxLoadCursor(IDC_CURSOR_RECORD);
                if (hcRecord != NULL)
                {
                        ::SetCursor(hcRecord);
                        return TRUE;
                }
                else
                        // Can't get record cursor, but we're recording?!
                        ASSERT(FALSE);
        }
        
        // Not recording, so we don't care
        return CMDIFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}


void CMainFrame::CloseWindows( UINT nFilter /*= FILTER_NONE*/ )
{
   // The MDI client is hidden while windows are closed down to prevent
   // lots of ugly redrawing...  The whole area will be repainted when
   // the window is shown again, but that's just one blit.

        m_pWndMDIClient->ShowWindow(SW_HIDE);

        g_AutomationState.SetSaveChanges(FALSE);
        BOOL bUI = g_AutomationState.DisplayUI();
        g_AutomationState.DisableUI();

        // Don't close the project.
        CMDIChildWnd* pMDIChild;
        while ((pMDIChild = theApp.MDITypedGetActive(nFilter)) != NULL)
        {
                // Also close dock manager handled MDI windows, or the dock manager will
                // be out of synch.  These window should do nothing in SaveModified,
                // so we do not risk repeating the save? message box.
                pMDIChild->SendMessage(WM_CLOSE);
        }
        g_AutomationState.Reset() ;

        if( !bUI ) {
			g_AutomationState.DisableUI();
		}
        m_pWndMDIClient->ShowWindow(SW_SHOWNA);
}

void CMainFrame::LockDockState(BOOL bLock)
{
   if ((m_bLockManager && bLock) || (!m_bLockManager && !bLock))
      return;

   m_bLockManager = bLock;
   if (!m_bLockManager)
      SetDockState(m_nManagerStateSave);
}

void CMainFrame::SetDockState(int nState, int nMask /* = 1 */)
{
        // The mask defines which bits are to affected in the state change.
        // A 1 in the mask means take the new state from the nState parameter
        // A 0 in the mask means take the new state from the old state
        //
        nState = (nMask & nState) | (~nMask & m_nManagerState);

        // If the dock state is locked, just save the requested state.
        m_nManagerStateSave = nState;
        if (m_bLockManager)
                return;

        if (m_nManagerState == nState)
                return;

        SaveManager();
        m_nManagerState = nState;

        // Cancel modes before the switch.
        SendMessage(WM_CANCELMODE);
        SushiCancelModes(m_hWnd);

        LockLayout(TRUE); // Don't allow recalc layout during LoadManager.
        LoadManager();
        LockLayout(FALSE);
        m_pManager->ShowManager(TRUE);

        RebuildMenus();

        // The menu bar can be hidden in full screen mode, but not otherwise; special case code here to sort out its
        // close box. Not the ideal place to put this.
        CDockWorker *pWorker=m_pManager->WorkerFromID(MAKEDOCKID(PACKAGE_SUSHI, IDTB_MENUBAR));
        if(     pWorker &&
                pWorker->GetDock()==dpNil && // if it's floating
                pWorker->m_pWnd)                         // defensive coding - should never fail.
        {
                // parent of the worker, when floating, is the CPaletteWnd that is the floating frame.
                CWnd *pWndParent=pWorker->m_pWnd->GetParent();
                if(pWndParent)
                {
                        CMenu*pMenu=pWndParent->GetSystemMenu(FALSE);

                        if(pMenu)
                        {
                                // disable or enable close box
                                if(!IS_STATE_FULLSCREEN(DkGetDockState()))
                                {
                                        pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
                                }
                                else
                                {
                                        pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
                                }
                        }
                }
        }
}

void CMainFrame::MapDockingCmd(UINT nID, UINT nCmd)
{
   int nNext = m_aDockCmds.GetSize();
   m_aDockCmds.SetAtGrow(nNext++, nCmd);
   m_aDockCmds.SetAtGrow(nNext, nID);
   if (m_pManager != NULL)
      m_pManager->MapCmdToWorker(nCmd, nID);
}

void CMainFrame::MapDockingCmds()
{
   ASSERT_VALID(m_pManager);

   int nCount = m_aDockCmds.GetSize();
   UINT nCmd, nIDWnd;
   for (int i = 0;  i < nCount;)
   {
      nCmd = m_aDockCmds[i++];
      nIDWnd = m_aDockCmds[i++];
      m_pManager->MapCmdToWorker(nCmd, nIDWnd);
   }
}

static char BASED_CODE szLayoutSection[] = "Layout";

BOOL CMainFrame::InitAfterRegistration()
{
   // This function is called once during startup.  It will be called
   // after all packages have registered.

   if (m_lastWindowRect != rectDefault)
   {
      SetWindowPos(NULL, m_lastWindowRect.left, m_lastWindowRect.top,
         m_lastWindowRect.Width(), m_lastWindowRect.Height(),
         SWP_NOACTIVATE | SWP_NOZORDER);
   }

   CVshellPackage* pPackage = (CVshellPackage*) theApp.GetPackage(PACKAGE_SUSHI);
   if (theApp.GetTemplate(CLSID_WorkspaceDocument) != NULL)
      pPackage->SetVisibleMenuItem(ID_PROJ_MRU_FILE1, TRUE);

        // must init this before we rebuild the menus for the very first time
        CDockWorker::s_lpszLayoutSection = szLayoutSection;
    // Initialize menus.
        RebuildMenus();
        InitializeToolsMenu(pPackage);
        InitializeHelpMenu(pPackage);

   // Initialize dock manager info.
   LoadLayout();

   if (!LoadManager())
   {
       m_pManager = new CDockManager;
      if (!m_pManager->Create(this, m_pWndMDIClient))
      {
         delete m_pManager;
         m_pManager = NULL;
         return FALSE;
      }
      MapDockingCmds();

   }

        // Try to get macro recording interfaces
        if (FAILED(theApp.FindInterface(IID_IMacroRecorder, (LPVOID*) &m_pMacroRecorder)) ||
                FAILED(theApp.FindInterface(IID_IRecorderForShell, (LPVOID*) &m_pRecorderForShell)) ||
                m_pMacroRecorder == NULL || m_pRecorderForShell == NULL)
        {
                // All or nothing.  If any recorder interfaces failed to be retrieved,
                //  then act as if recording isn't supported
                m_pMacroRecorder = NULL;
                m_pRecorderForShell = NULL;
        }


   ASSERT(m_pManager != NULL);

   // Set status bar visibility.
   if (!theApp.m_bStatusbar)
      m_statusBar.ShowWindow(SW_HIDE);

    // Create the property sheet.  We need this to load the workspace.
   if (!CreatePropertyBrowser())
      return FALSE;

   // Show everything.
   ShowWindow(theApp.m_nCmdShow);
   if (IsFullScreen())
   {
      m_FS.m_bFullScreen = FALSE;   // Hack!
      OnWindowFullScreen();
   }
   //REVIEW: This needs to be TRUE even if we are hiding the window. 
   // It seems to perform some required initialization.
   m_pManager->ShowManager(TRUE);
   UpdateWindow();

   GetPropertyBrowser()->InitialUpdate(theApp.m_ptPropPage);
   return TRUE;
}

void CMainFrame::InitializeToolsMenu(CVshellPackage* pPackage)
{
   for (int nID = IDM_USERTOOLS_BASE ; nID < IDM_USERTOOLS_LAST ; nID++)
   {
      // To initialize the tools menu at startup, just use the
      // following code for each item:
      //
      //    pPackage->SetVisibleMenuItem(nID, TRUE, "&User tool");
      //
      // The boolean parameter specifies whether the item should appear
      // on the menu bar, while the string parameter is the menu text.
      //
      // You can use the same call anytime after initialization to modify
      // the set of displayed menu items (like when the user brings up the
      // "Tools" dialog to change the available tool commands).
      //
      // The default state is for all IDM_USERTOOLS items to be hidden,
      // so you only need to call SetVisibleMenuItem for the ones which
      // you want to be shown.
      CTool * curTool ;
      if ((curTool = toolList.GetTool(nID-IDM_USERTOOLS_BASE)) != NULL)
         pPackage->SetVisibleMenuItem (nID, TRUE) ;
   }
}

void CMainFrame::InitializeHelpMenu (CVshellPackage* pPackage)
{
   BOOL bState;

   // Tip of the Day
   bState = isTipAvailable();
   ((CPackage*)pPackage)->SetVisibleMenuItem(IDM_HELP_TIPOFTHEDAY, bState);

   // Help.Technical Support
   CString strHelp;
   bState = GetPSSHelpFile(strHelp);
   ((CPackage*)pPackage)->SetVisibleMenuItem(IDM_HELP_MSPROD, bState);

   // Ext Help
   bState = ShowExtHelp() && GetExtHelpFile(strHelp);
   ((CPackage*)pPackage)->SetVisibleMenuItem(IDM_HELP_EXTHELP, bState);
}

void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
   switch( nHitTest )
   {
      case HTMENU:
         // Make sure the menu is valid.
         if( theApp.m_bMenuDirty )
            RebuildMenus();
         break;
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

   CMDIFrameWnd::OnNcLButtonDown(nHitTest, point);

   EnsureCustomizerActive();
   EnsurePopupActive();
}

void CMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
   // HACK: We do our own closing of MDI child windows, since Windows
   // only does this if the MDIClient window is the first child in
   // z-order, and our dock painting is uggly if we wait until after
   // the MDIClient.

   if (bControlMenuSelected && nHitTest == HTMENU &&
      theApp.m_dwMenuSelectTime == GetCurrentMessage()->time)
   {
      BOOL bMax = FALSE;
      CMDIChildWnd* pWnd = MDIGetActive(&bMax);
      ASSERT(bMax && pWnd != NULL);

      CMenu* pMenu = pWnd->GetSystemMenu(FALSE);
      if (pMenu != NULL)
      {
         UINT nMenuState = pMenu->GetMenuState(SC_CLOSE, MF_BYCOMMAND);
         if ((nMenuState & (MF_DISABLED | MF_GRAYED)) == 0)
         {
            pWnd->SendMessage(WM_SYSCOMMAND, SC_CLOSE);
            return;
         }
      }
   }

   CMDIFrameWnd::OnNcLButtonDblClk(nHitTest, point);

   EnsureCustomizerActive();
   EnsurePopupActive();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
   switch (nID)
   {
      case SC_PREVWINDOW:
      case SC_NEXTWINDOW:
         // Break windows Alt+F6 behavior so as not to give floating toolbars
         // the focus.  This is an obscure feature anyway.

         if (LOWORD(lParam) == VK_F6)
         {
            // Add special handling for modeless dialogs hear if desired.
            return;
         }
         break;

      case SC_MINIMIZE:
         if (m_pManager != NULL)
            m_pManager->ShowPalettes(FALSE);
         SetActiveWindow();   // Take activation from customizer dialog.
         break;

      case SC_KEYMENU:
         // Make sure the menu is valid.
         if (theApp.m_bMenuDirty)
            RebuildMenus();
         break;
      case SC_MOVE:
      case SC_SIZE:
      {
         CSlobWnd* pView = (CSlobWnd*)GetActiveFrame()->GetActiveView();
         if( (pView != NULL) && pView->IsKindOf(RUNTIME_CLASS(CSlobWnd)) )
            pView->ObscureSprites();
         break;
      }
   }

   CMDIFrameWnd::OnSysCommand(nID, lParam);

   switch (nID)
   {
   case SC_RESTORE:
   case SC_MAXIMIZE:
      if (m_pManager != NULL)
         m_pManager->ShowPalettes(TRUE);
      EnsureCustomizerActive();
      break;
   }
}

void CMainFrame::EnsureCustomizerActive()
{
   if (CASBar::s_pCustomizer != NULL && !IsIconic())
   {
      ASSERT(CASBar::s_pCustomizer->m_pDialog != NULL);
      CASBar::s_pCustomizer->m_pDialog->SetActiveWindow();
   }
}

void CMainFrame::EnsurePopupActive()
{
   CDockWorker* pDocker = CDockWorker::s_pActiveDocker;
   if (pDocker != NULL && pDocker->GetDock() == dpNil && !IsIconic())
   {
      ASSERT(pDocker->m_pWnd != NULL);
      pDocker->m_pWnd->SetFocus();
   }
}

void CMainFrame::EnsureViewActive()
{
   // Make sure the focus is in the active view.
   HWND hwndView = theApp.GetActiveView();
   HWND hwndFocus = ::GetFocus();

   if (hwndView != NULL && hwndView != hwndFocus &&
         !::IsChild(hwndView, hwndFocus))
   {
      // Only do this if the view is not a floating window.
      CDockWorker* pDocker = CDockWorker::s_pActiveDocker;
      if (pDocker == NULL || pDocker->GetDock() != dpNil)
         ::SetFocus(hwndView);
   }
}

void CMainFrame::OnWinIniChange(LPCSTR lpszSection)
{
   InitMetrics();
   theApp.m_CPLReg.Update();
   globalData.UpdateSysMetrics();

   CMDIFrameWnd::OnWinIniChange(lpszSection);

   // Can't do this inside UpdateSysMetrics
   theCmdCache.FlushMenuSizes();

        CASBar *pBar=CASBar::s_pMenuBar;
        if(pBar)
        {
                pBar->RecalcLayout();
        }

}

void CMainFrame::OnFontChange ()
{
   POSITION pos = theApp.m_packages.GetHeadPosition();
   while (pos != NULL)
      ((CPackage*)theApp.m_packages.GetNext(pos))->OnFontChange();
}

#ifdef _DEBUG
LRESULT CMainFrame::OnFatalExit(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
   ASSERT(FALSE);
#else
   extern CATCHBUF fatalExitBuf;
   TRACE("VSHELL: fatal exit request received from package %d\n", wParam);
   Throw(fatalExitBuf, 1); // NEVER PASS 0!
#endif
   return 0;
}
#endif

BOOL CMainFrame::OnDockWindowShow(UINT nID)
{
   CObArray aWorkers;
   m_pManager->ArrayOfType(dtEmbeddedToolbar, &aWorkers, TRUE, TRUE);

   int nIndex = nID - IDM_DOCKSHOW_BASE;
   ASSERT(nIndex < aWorkers.GetSize());

   CDockWorker* pDocker = (CDockWorker*) aWorkers[nIndex];
   pDocker->ShowWindow(!pDocker->IsVisible());

   return TRUE;
}

// Now that we have the command cache, we can no longer rely on MFC to 
// sort out the prompt strings for MRU menus for us. martynl 13May96
void CMainFrame::GetMessageString(UINT nID, CString& rMessage) const
{
   LPCTSTR szCommand;
   if (nID >= ID_FILE_MRU_FIRST && nID <= ID_FILE_MRU_LAST)
   {
           nID = ID_FILE_MRU_FIRST;
   }

   if (nID >= ID_PROJ_MRU_FIRST && nID <= ID_PROJ_MRU_LAST)
   {
           nID = ID_PROJ_MRU_FIRST;
   }

   if (theCmdCache.GetCommandString(nID, STRING_PROMPT, &szCommand))
   {
      rMessage = szCommand;
   }
   else
           CFrameWnd::GetMessageString(nID, rMessage);
}

void CTheApp::SetEmptyToolTip(NMHDR* pNMHDR)
{
   if (pNMHDR->code == TTN_NEEDTEXTA)
      ((TOOLTIPTEXTA*) pNMHDR)->szText[0] = '\0';
   else
   {
      ASSERT(pNMHDR->code == TTN_NEEDTEXTW);
      ((TOOLTIPTEXTW*) pNMHDR)->szText[0] = L'\0';
   }
}

void CTheApp::AddKeyToToolTip(CString& rstrTextCmd, UINT nID, WORD flagsCmdCache, LPCTSTR szKeyIn)
{
   if (!theApp.m_bToolTipKeys)
      return;

   CString strKey;
   if (szKeyIn == NULL)
   {
          UINT nIDKey = nID;
          if (flagsCmdCache & CT_DYNAMIC_CMD_STRING)
          {
                  // get possible alternate ID for key string from package (useful for toolbar hwnds like findctrl bobz)
                        CTE* pCTE = theCmdCache.GetCommandEntry(nID);
                        ASSERT(pCTE->flags & CT_DYNAMIC_CMD_STRING); 
                        ASSERT(pCTE->GetPack() != NULL);
                        ASSERT(pCTE->GetPack()->PacketID() != PACKET_SHARED);
                        CPackage *pPackage = pCTE->GetPack()->GetPackage();
                        ASSERT(pPackage != NULL);

                        nIDKey = pPackage->CmdIDForToolTipKey(nID);
          }

      if (!GetCmdKeyString(nIDKey, strKey))
         return;
   }
   else
      strKey = szKeyIn;

   rstrTextCmd += " (";
   rstrTextCmd += strKey;
   rstrTextCmd += ")";
}

// this extracts the tooltip text from the resource id

// When this routine was modified to use the command cache, a subtle error
// was introduced. Specifically, the new code assumed that, if szKeyIn was NULL, 
// then the command being 'tipped' was from the command table rather than from
// a loaded resource string. This caused Orion bug #1538, as the grid window
// contains one control which is tipped using this function, but which has no
// keyboard shortcut (new)

// To fix this, we allow szKeyIn to take a third value:
// NULL - as before, implies no key, so look in cmd cache
// nonnull:
//              ptr to empty string: no key, but don't look in cmd cache
//      ptr to non-empty string: use cmd cache, but add the specified key to it.

// martynl 09Apr96

BOOL CTheApp::HandleToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult, LPCTSTR szKeyIn)
{
   ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

   // suppress tip if disabled
   if (!theApp.m_bToolTips)
   {
      SetEmptyToolTip(pNMHDR);
        
      *pResult = 0;
      return TRUE;
   }

   WORD wFlagsCmdCache = CT_NONE;  // used to pass flags from cmdcache to AddKeyToToolTip

   // ASSUMEs top level routing frame already called with GetRoutingFrame to handle the message, if desired

   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
   TCHAR szFullText[256];
   CString strTipText;
   UINT nIDFrom = pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
      pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nIDFrom = ((UINT)(WORD)::GetDlgCtrlID((HWND)nIDFrom));
   }

        if (nIDFrom == 0) // will be zero on a separator
        {
                CTheApp::SetEmptyToolTip(pNMHDR);
                *pResult = 0;
                return TRUE;
        }
        else
        {
                if (szKeyIn == NULL)
                {
                        LPCTSTR szCommand;

                        // Note: do not hold onto the return string of GetCommandString, copy immediately.
                        if (theCmdCache.GetCommandString(nIDFrom, STRING_TIP, &szCommand, &wFlagsCmdCache) &&
                                szCommand != NULL && *szCommand != '\0')
                        {
                                strTipText = szCommand;
                        }
                        else
                        {
                                if (theCmdCache.GetCommandString(nIDFrom, STRING_COMMAND, &szCommand, &wFlagsCmdCache))
                                {
                                   TRACE0("Using menu help text in place of missing tooltip text");
                                   strTipText = szCommand;
                                }
                                else
                                {
                                        CTheApp::SetEmptyToolTip(pNMHDR);
                                        *pResult = 0;
                                        return TRUE;
                                }
                        }
                }
                else
                {
                         AfxLoadString(nIDFrom, szFullText);
                                // this is the command id, not the button index
                         AfxExtractSubString(strTipText, szFullText, 1, '\n');
                }
        }

      // never pass on the empty string special value
   LPCTSTR szPassKey = szKeyIn;
   if (szPassKey != NULL &&  *szPassKey == '\0')
   {
           szPassKey=NULL;
   }

   AddKeyToToolTip(strTipText, nIDFrom, wFlagsCmdCache, szPassKey);


#ifndef _UNICODE
   if (pNMHDR->code == TTN_NEEDTEXTA)
      lstrcpyn(pTTTA->szText, strTipText, _countof(pTTTA->szText));
   else
      _mbstowcsz(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#else
   if (pNMHDR->code == TTN_NEEDTEXTA)
      _wcstombsz(pTTTA->szText, strTipText, _countof(pTTTA->szText));
   else
      lstrcpyn(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#endif

   *pResult = 0;

   return TRUE;    // message was handled
}


BOOL CMainFrame::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	CASBar::ForceReleaseCapture();
	return CTheApp::HandleToolTipText(nID, pNMHDR, pResult);
}

void CMainFrame::OnWindowListDocs()
{
	CHaltRecording haltrec; // DS96 #14956 [CFlaat]: we'll just turn off recording here, as there are too many gremlins

	COpenDocsDlg dlg;
	dlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// Application Palette Management

void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd)
{
   SendMessageToDescendants(WM_PALETTECHANGED,
      (WPARAM)pFocusWnd->GetSafeHwnd(), 0, TRUE, TRUE);
}

BOOL CMainFrame::OnQueryNewPalette()
{
// BOOL bRet = FALSE;
   CMDIChildWnd* pFrame = MDIGetActive();
   if (pFrame != NULL)
   {
      /*bRet =*/ pFrame->SendMessageToDescendants(WM_QUERYNEWPALETTE, 0, 0,
         TRUE, TRUE);

      OnPaletteChanged(pFrame);
   }

// return bRet;
   return TRUE;
}

////////////////////////////////////////////////////////////////////////////


void CMainFrame::OnEndSession(BOOL bEnding)
{
    if (bEnding)
	{
		// We need to save all settings here, this is our last chance.
		OnClose();
	}
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
   CDockWorker* pDocker = CDockWorker::s_pActiveDocker;
   if (pDocker == NULL)
      CMDIFrameWnd::OnSetFocus(pOldWnd);
   else if (pDocker->GetDock() != dpNil)
      pDocker->m_pWnd->SetFocus();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pOtherWnd, BOOL bMinimized)
{
#ifdef _DEBUG
   ASSERT(!bQuitting || nState == WA_INACTIVE);
   bQuitting = FALSE;   // Stop us from showing continuous stream of asserts.
#endif

   CDockWorker* pDocker = CDockWorker::s_pActiveDocker;
   CSheetWnd * pPropSheet = GetPropertyBrowser();

    if (nState != WA_INACTIVE && pPropSheet != NULL && pPropSheet->IsActive())
        pPropSheet->SetActive(FALSE);

        if (pDocker == NULL || nState == WA_INACTIVE || bMinimized)
          CMDIFrameWnd::OnActivate(nState, pOtherWnd, bMinimized);
        else if (pDocker->GetDock() != dpNil)
        {
          // CMDIFrameWnd::OnActivate sets focus to a view.
          CWnd::OnActivate(nState, pOtherWnd, bMinimized);
          pDocker->m_pWnd->SetFocus();
        }

        ASSERT(GetComponentMgr() != NULL);
        if (GetComponentMgr() != NULL && nState != WA_INACTIVE)
                GetComponentMgr()->FOnComponentActivate(GetComponentID());
}

void CMainFrame::OnTimer(UINT nTimerID)
{
   switch(nTimerID)
   {
#ifdef _AUTOSAVE
   case IDT_AUTOSAVE:
      //BLOCK:
      {
         POSITION pos;

         // Don't start a new tick cycle if we're still in one.
         if (gAutosaver.InTick())
            return;
         // Send and autosave tick message to each package
         gAutosaver.EnterTick();
         pos = theApp.m_packages.GetHeadPosition();
         while (pos != NULL)
         {
            CPackage* pGroup = (CPackage*)theApp.m_packages.GetNext(pos);
// If this is ever enabled we need to use a CPackage virtual and PKS_ flag instead.
//          pGroup->SendMessage(WM_ONAUTOSAVETICK, 0, (LPARAM) &gAutosaver);
         }
         gAutosaver.ExitTick();
      }
      break;
#endif

   case IDT_WINDOW_FLASH:
      FlashWindow(TRUE);
      break;

   // Keyboard-chord timers
   case 200: // First timer, 1 sec after keyboard chord initiated
      {
         CString strKey;
         if (CKeymap::c_bImplyAltForNextKey)
            strKey = CKeymap::c_szAlt;
         else
            CKeymap::GetKeyName(CKeymap::c_kcChordPrefix, 0, strKey);
         SetPrompt(strKey);
         KillTimer(200);
         SetTimer(201, 4 * 1000, NULL);
      }
      break;

   case 201: // Second timer, we're giving up on the chord
      CKeymap::CancelChord();
      break;

   case IDT_FILE_CHANGE:
      if( IsWindowEnabled() && theApp.m_fVCPPIsActive && !IsIconic() )
      {
         while( m_plFileChange.GetCount() > 0 )
         {
            FCItem* pFCItem = (FCItem*)m_plFileChange.RemoveHead();
            POSITION posTemplate = theApp.GetFirstDocTemplatePosition();
            while( posTemplate != NULL )
            {
               CPartTemplate* pTemplate = (CPartTemplate*)theApp.GetNextDocTemplate(posTemplate);
               POSITION posDoc = pTemplate->GetFirstDocPosition();
               while( posDoc != NULL )
               {
                  CPartDoc* pDoc = (CPartDoc*)pTemplate->GetNextDoc(posDoc);
                  // the wPackage parameter has been stored in _pbUser1
                  pDoc->OnFileChange((int)pFCItem->_pvUser1, pFCItem);
               }
            }
            delete pFCItem;   // created by original FileChange callback
         }
         KillTimer(IDT_FILE_CHANGE);
      }
      break;

   default:
      CMDIFrameWnd::OnTimer(nTimerID);
   }
   return;
}

void CMainFrame::OnSysColorChange()
{
        extern BOOL bEnable3dControls;
        
        globalData.UpdateSysColors();
        
        POSITION pos = theApp.m_packages.GetHeadPosition();
        while (pos != NULL)
                ((CPackage*)theApp.m_packages.GetNext(pos))->OnColorChange();
        
    ResetSysBrushes();
        
        // So toolbars will update correctly, we unload all the package's existing bitmaps. New ones will be demand loaded
        // as appropriate.
        theApp.UnloadPackageBitmaps();   

        if (!bEnable3dControls)
                theApp.SetDialogBkColor();
        
        CMDIFrameWnd::OnSysColorChange();
        
        m_pManager->SysColorChange(); // Dock managed floating windows.

        CBMenuPopup::SysColorChange();
}

void CMainFrame::InitKeymap()
{
   ASSERT(m_hAccelTable != NULL);
   m_keymap.LoadAccelerators(m_hAccelTable);
   DestroyAcceleratorTable(m_hAccelTable);
   m_hAccelTable = NULL;
   VERIFY( m_keymap.m_strName.LoadString( IDS_MAIN ) );
}

void CMainFrame::OnUpdateNewWindow(CCmdUI* pCmdUI)
{
   // Default is to leave this disabled...  If you want your MDI child
   // to be clone-able, override this in that class...

   pCmdUI->Enable(FALSE);
}

void CMainFrame::OnNewWindow(void)
{
        // we override this only to hook in for recording purposes

        // record it
        GetShellRec().EmitMethodCall(seNewWin);

        // and delegate to our base class
        CMDIFrameWnd::OnWindowNew();
}

///////////////////////////////////////////////////////////////////////////////
// Window validation handling.
//

LRESULT CMainFrame::OnSetAppValidate(WPARAM wParam, LPARAM)
{
   theApp.m_hwndValidate = (HWND) wParam;

   return 0L;
}

static int cyIconBase;

CMDIClientValidator::CMDIClientValidator()
{
   m_cyLast = 0;

   cyIconBase = theApp.m_bWin4 ? 0 : 42;
}

WNDPROC* CMDIClientValidator::GetSuperWndProcAddr()
{
   static WNDPROC NEAR pfnSuper;
   return &pfnSuper;
}

void CMDIClientValidator::PostNcDestroy()
{
   delete this;
}

BEGIN_MESSAGE_MAP(CMDIClientValidator, CWnd)
   ON_WM_PARENTNOTIFY()
   ON_WM_SIZE()
   ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


void CMDIClientValidator::OnParentNotify(UINT message, LPARAM lParam)
{
        if (theApp.m_hwndValidate != NULL &&
                ::SendMessage(theApp.m_hwndValidate, WM_ISERROR, 0, 0L))
        {
                return;
        }
        else if (GetCapture() == NULL &&
                (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN))
        {
                CPoint pt(::GetMessagePos());
                CWnd* pWndActive = ((CMDIFrameWnd*) GetParent())->MDIGetActive();
                CWnd* pWndNotify = WindowFromPoint(pt);  
                // Note that in some stress cases pWndNotify can be NULL!
                if(pWndNotify != NULL)
                {
                        if (pWndActive != NULL &&
                                (pWndActive == pWndNotify || pWndActive->IsChild(pWndNotify)))
                        {
                                if (!pWndActive->IsChild(GetFocus()))
                                pWndActive->SetFocus();
                        }
                        else if (message == WM_RBUTTONDOWN)
                        {
                                while (	pWndNotify!=NULL &&
										pWndNotify != this &&
										!pWndNotify->IsKindOf(RUNTIME_CLASS(CPartFrame)))
                                pWndNotify = pWndNotify->GetParent();

                                if (pWndNotify!=NULL && 
									pWndNotify != this)
                                        ((CPartFrame*) pWndNotify)->MDIActivate();
                        }
                }
        }

        Default();
}

void CMDIClientValidator::OnSize(UINT nType, int, int cy)
{
   Default();

   if (nType != SIZE_RESTORED || cy == m_cyLast)
      return;

   // Move all the icons to try to keep them on screen.

   CRect rectIcon;
   CSize sizeBaseOffset(0, cy - m_cyLast);
   int cyIcon;

   for (CWnd* pWndNext = ((CMDIFrameWnd*) GetParent())->MDIGetActive();
      pWndNext != NULL; pWndNext = pWndNext->GetWindow(GW_HWNDNEXT))
   {
      if(!IsValidMDICycleMember(pWndNext) || !pWndNext->IsIconic())
         continue;

      pWndNext->GetWindowRect(rectIcon);
      ScreenToClient(rectIcon);

      cyIcon = cy - (rectIcon.bottom + cyIconBase);     // magic number.
      CSize sizeOffset = sizeBaseOffset;
      sizeOffset.cy = max(sizeOffset.cy, -rectIcon.top);        // restrict above.
      sizeOffset.cy = min(sizeOffset.cy, cyIcon);       // retrict below.
      rectIcon.OffsetRect(sizeOffset);

      pWndNext->MoveWindow(rectIcon);
      pWndNext->ShowWindow(SW_SHOWNA); // causes text to move.
   }

   m_cyLast = cy;
}

void CMDIClientValidator::OnRButtonUp(UINT nFlags, CPoint pt)
{
	// cause the dock manager to show its menu
	CMainFrame *pFrame=(CMainFrame *)AfxGetMainWnd();
	if(pFrame)
	{
		if(pFrame->m_pManager)
		{
			ClientToScreen(&pt);
			pFrame->m_pManager->ShowMenu(pt);
		}
	}
}

BOOL DkEnsureHidden(UINT nID)
{
   CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
   if (pManager == NULL)
      return FALSE;

   CDockWorker* pWorker = pManager->WorkerFromID(nID);
   if (pWorker == NULL)
      return FALSE;

   if (!pWorker->IsVisible())
      return FALSE;

   pWorker->ShowWindow(FALSE);
   return TRUE;
}

void CMainFrame::OnCancel()
{
   // Hide the property window
   if (IsPropertyBrowserVisible())
   {
      ClosePropertyBrowser();
      return;
   }
   
   // Hide the book window
   if (DkEnsureHidden(MAKEDOCKID(PACKAGE_IV, IDDW_IV)))
      return;

   // Hide the output window
   if (DkEnsureHidden(MAKEDOCKID(PACKAGE_VCPP, IDDW_OUTPUT)))
      return;

   // End full screen mode
   if (IsFullScreen())
   {
      OnWindowFullScreen();
      return;
   }

   MessageBeep(0);
}

void CMainFrame::CmdGoto()
{
   DoGoToDialog(IDD_GOTODIALOG, NULL);
}

/* OnFileChange
 * Called when a FileChange callback does a PostMessage (because the callback
 * is in a different thread).
 * wPackage is a convenient way to narrow down the search, based on which callback called us.
 * lpFCItem is a copy of the FCItem that was passed to the original callback.
 * Note that none of the actual work is done here.  Rather a list of files is
 * built up, and handled when a timer is reached.
 */
LRESULT CMainFrame::OnFileChange(WPARAM wPackage, LPARAM lpFCItem)
{
   ASSERT( lpFCItem != NULL );

   // put off any notifications until our app is active again
   ((FCItem*)lpFCItem)->_pvUser1 = (void*)wPackage;     // HACK! have to store this somewhere
   m_plFileChange.AddTail((FCItem*)lpFCItem);

   if( theApp.m_fVCPPIsActive && !IsIconic() )
      SetTimer(IDT_FILE_CHANGE, 100, NULL);  // set/reset timer for very soon
   else
      SetTimer(IDT_FILE_CHANGE, 1000, NULL); // set/reset timer for a second

   // the FCItem will be deleted in OnTimer
   return 0;
}

// ensure that everyone gets a chance to adapt to a change in toolbar size
LRESULT CMainFrame::OnBarSizeChanging(WPARAM wParam, LPARAM lParam)
{
        SendMessageToDescendants(DSM_BARSIZECHANGING,wParam, lParam, TRUE, FALSE); 

        return 0;
}

void CMainFrame::AddIPCompFrameHook(CIPCompFrameHook * pHook)
{
        ASSERT(pHook != NULL);
        m_plIPCompFrameHook.AddHead(pHook);
}

void CMainFrame::RemoveIPCompFrameHook(CIPCompFrameHook * pHook)
{
        ASSERT(pHook != NULL);
        POSITION pos = m_plIPCompFrameHook.Find(pHook);

        if (pos != NULL)
        {
                m_plIPCompFrameHook.RemoveAt(pos);
        }
}

BOOL CMainFrame::SetComponentManager(IMsoComponentManager * pCmpMgr)
{
        ASSERT(m_pIPCmpMainFrm); 
        return m_pIPCmpMainFrm->SetComponentManager(pCmpMgr); 
}

IMsoComponentManager * CMainFrame::GetComponentMgr()
{
        if (m_pIPCmpMainFrm != NULL)
        {
                return  m_pIPCmpMainFrm->m_srpMsoCompMgr ;
        }
        else
        {
                return NULL ;
        }
        // INTEGRATED BUILD FIX: The new compiler is very picky about the comparison operator.
        // The constructor for the OleRef was getting called and so was the destructor, so this
        // wasn't working. 29 Jul 96 DaleRo
        //return (m_pIPCmpMainFrm ? m_pIPCmpMainFrm->m_srpMsoCompMgr : (IMsoComponentManager *)NULL);
}
   
DWORD CMainFrame::GetComponentID() 
{
        return (m_pIPCmpMainFrm ? m_pIPCmpMainFrm->m_dwComponentID : 0);
}       
        
void CMainFrame::BeginModalState() 
{
        // if we are registered with the component manager and we are the one
        // initiating the modal state (i.e. we are not being currently called back
        // by the ComponentManager via IMsoComponent::OnEnterState -- 
        // m_fInOnEnterState is used as a guard for OnEnterState method) then inform
        // the ComponentManager of the modality change. the ComponentManger
        // will NOT call us back via IMsoComponent::OnEnterState implemented
        // on our CMainFrame because we are the component making the call. thus we
        // need to continue and call CMDIFrameWnd::BeginModalState ourselves.
        if (!m_fInOnEnterState && m_pIPCmpMainFrm && m_pIPCmpMainFrm->m_srpMsoCompMgr)
        {
                m_pIPCmpMainFrm->m_srpMsoCompMgr->OnComponentEnterState(
                                m_pIPCmpMainFrm->m_dwComponentID, msocstateModal,
                                msoccontextAll, 0, NULL, NULL);
        } 

        CMDIFrameWnd::BeginModalState();
}    
   
void CMainFrame::EndModalState() 
{
        // if we are registered with the component manager and we are the one
        // initiating the modal state (i.e. we are not being currently called back
        // by the ComponentManager via IMsoComponent::OnEnterState -- 
        // m_fInOnEnterState is used as a guard for OnEnterState method) then inform
        // the ComponentManager of the modality change. the ComponentManger
        // will NOT call us back via IMsoComponent::OnEnterState implemented
        // on our CMainFrame because we are the component making the call. thus we
        // need to continue and call CMDIFrameWnd::BeginModalState ourselves.
        if (!m_fInOnEnterState && m_pIPCmpMainFrm && m_pIPCmpMainFrm->m_srpMsoCompMgr)
        {
                m_pIPCmpMainFrm->m_srpMsoCompMgr->FOnComponentExitState(
                                m_pIPCmpMainFrm->m_dwComponentID, msocstateModal,
                                msoccontextAll, 0, NULL);
        }

        CMDIFrameWnd::EndModalState();
}       

/////////////////////////////////////////////////////////////////////////////
// Implementation of IMsoComponent

BEGIN_INTERFACE_MAP(CIPCompMainFrame, CCmdTarget)
        INTERFACE_PART(CIPCompMainFrame, IID_IMsoComponent, MsoComponent)
END_INTERFACE_MAP()

CIPCompMainFrame::CIPCompMainFrame()
{
        m_dwComponentID = 0;
}

STDMETHODIMP_(BOOL) CIPCompMainFrame::XMsoComponent::FPreTranslateMessage(MSG *pMsg)
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
        return theApp.PreTranslateMessage(pMsg);
}

STDMETHODIMP_(void) CIPCompMainFrame::XMsoComponent::OnEnterState(ULONG uStateId, BOOL fEnter)
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
        CMainFrame * pMainFrm = (CMainFrame *)AfxGetMainWnd();
        ASSERT(pMainFrm->IsKindOf(RUNTIME_CLASS(CMainFrame)));
        if (uStateId == msocstateModal)
        {
                // All the action for modality management happens when our MainFrame window
                // received a WM_ENABLE message (or CMainFrame::OnEnable is called).
                // if the modality change is initiated by another component in our 
                // process, then our MainFrame's IMsoComponent::OnEnterState method 
                // is called. in this situation we do not want to call back to the 
                // ComponentManager in CMainFrame::BeginModalState which is called by
				// CFrameWnd::OnEnable. the m_fInOnEnterState flag is used to guard this call.
				//
				// Note that we need to check the actual modal state, since we get
				// notified when any component leaves the modal state, but should
				// not re-enable our window until all components have left the modal
				// state.

			BOOL fEnable = !fEnter;

			if (pThis->m_srpMsoCompMgr != NULL)
			{
				// Dont enable the window unless all components have left the Modal state.
				fEnable = fEnable && !pThis->m_srpMsoCompMgr->FInState(msocstateModal, NULL);
			}

            pMainFrm->m_fInOnEnterState = TRUE; // guard against calling ComponentManager
				pMainFrm->EnableWindow(fEnable);
            pMainFrm->m_fInOnEnterState = FALSE;
        }

#ifndef _IPC_DOCUMENTS_DISABLED
        if (!pMainFrm->m_plIPCompFrameHook.IsEmpty())
        {
                CIPCompFrameHook * pHook = (CIPCompFrameHook *)pMainFrm->m_plIPCompFrameHook.GetHead();
                if (pHook != NULL)
                {
                        ASSERT(pHook->m_pActiveItem != NULL);
                        CView * pHookView = pHook->m_pActiveItem->GetActiveView();
                        // notify the active Frame hook, which is associated with the active child window, 
                        // in order to notify the current UI active object within that child window.
                        if (pHookView != NULL && ((HWND)pMainFrm->GetActiveFrame() == (HWND)pHookView->GetParentFrame()))
                                pHook->OnActiveEnableModelss(uStateId, fEnter);

                        if (uStateId != msocstateModal)
                        {
                                // walk thru the list, and notify all inplace components
                                POSITION pos = pMainFrm->m_plIPCompFrameHook.GetHeadPosition();
                                while (pos != NULL)
                                {
                                        // since the active frame hook is always at the head of the list, it will get the 
                                        // the first crack
                                        pHook = (CIPCompFrameHook *)pMainFrm->m_plIPCompFrameHook.GetNext(pos);
                                        pHook->OnEnterState(uStateId, fEnter);
                                }
                        }
                }
        }
#endif
}               

STDMETHODIMP_(void) CIPCompMainFrame::XMsoComponent::OnAppActivate(BOOL fActivate, DWORD dwOtherThreadID)
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
        CMainFrame * pMainFrm = (CMainFrame *)AfxGetMainWnd();
        ASSERT(pMainFrm->IsKindOf(RUNTIME_CLASS(CMainFrame)));

#ifndef _IPC_DOCUMENTS_DISABLED
        // notify the active Frame hook only (which should be associated with the active child window)
        if (!pMainFrm->m_plIPCompFrameHook.IsEmpty())
        {
                CIPCompFrameHook * pHook = (CIPCompFrameHook *)pMainFrm->m_plIPCompFrameHook.GetHead();
                ASSERT(pHook);
                ASSERT(pHook->m_pActiveItem != NULL);
                CView * pHookView = pHook->m_pActiveItem->GetActiveView();
                if (pHookView != NULL && ((HWND)pMainFrm->GetActiveFrame() == (HWND)pHookView->GetParentFrame()))
                        pHook->OnFrameActivate(fActivate);
        }
#endif
}

STDMETHODIMP_(void) CIPCompMainFrame::XMsoComponent::OnLoseActivation()
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
    // we don't have anything to do here.
}

STDMETHODIMP_(void) CIPCompMainFrame::XMsoComponent::OnActivationChange(IMsoComponent *pic, BOOL fSameComponent,
                                                const MSOCRINFO *pcrinfo, BOOL fHostIsActivating,
                                                const MSOCHOSTINFO *pchostinfo, DWORD dwReserved)
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
    // we don't have anything to do here.
}

#ifdef METER_IDLE_TIME
// Let's gather some interesting statistics on idle time processing
extern BOOL s_bIdleTime;
extern CStringList g_strlstPkgsLoaded;
DWORD g_dwIdlePackageTime[40];
DWORD g_IdleProcessingTime;
int g_idxIdlePackage;

BOOL MeteredOnIdle(LONG lIdleCount)
{
        BOOL fRetVal;
        static FILE *outfile = NULL;
        static int iPkgCount = 0;               // Count of packages which want idle time
        DWORD dwTotalTime;

        // Fastest in/out if not requesting logging
        if (!s_bIdleTime)
                return (theApp.OnIdle(lIdleCount));

        g_idxIdlePackage = 0;

        dwTotalTime = ::GetTickCount();
        fRetVal = theApp.OnIdle(lIdleCount);
        dwTotalTime = ::GetTickCount() - dwTotalTime;

        if (!outfile)
        {
                if (!(outfile = fopen("idletime.log", "w")))
                {                       // Should never happen - but don't blow up
                        s_bIdleTime = 0;
                        return fRetVal;
                }
                // Write a header
                fprintf(outfile, "Count");

                POSITION pos = theApp.m_packages.GetHeadPosition();
                int iPkgIndex = 0;
                while (pos != NULL)
                {
                        CPackage* pPackage = (CPackage*)theApp.m_packages.GetNext(pos);
                        if ((pPackage->m_flags & PKS_IDLE)!= 0)
                        {
                                ++iPkgCount;
                                fprintf(outfile, "\t%s",
                                        (LPCTSTR)g_strlstPkgsLoaded.GetAt(g_strlstPkgsLoaded.FindIndex(iPkgIndex-1)));
                        }
                        iPkgIndex++;
                }
                fprintf(outfile, "\tShell\tTotal\n");
        }
        if (dwTotalTime)
        {       // Only output records which show some nonzero data points
                fprintf(outfile, "%d", lIdleCount);
                for (int idx = 0; idx < iPkgCount; idx++)
                        fprintf(outfile, "\t%d", g_dwIdlePackageTime[idx]);
                fprintf(outfile, "\t%d\t%d\n", g_IdleProcessingTime, dwTotalTime);
        }
        return fRetVal;
}
#else   // !METER_IDLE_TIME
#define MeteredOnIdle(x) (theApp.OnIdle(x))
#endif  // !METER_IDLE_TIME

STDMETHODIMP_(BOOL) CIPCompMainFrame::XMsoComponent::FDoIdle(DWORD grfidlef)
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
        ASSERT(pThis->m_srpMsoCompMgr != NULL);
        
        BOOL fMore = FALSE;
        
        LONG lIdleCount = 0;
        CMainFrame * pMainFrm = (CMainFrame *)AfxGetMainWnd();
        ASSERT(pMainFrm->IsKindOf(RUNTIME_CLASS(CMainFrame)));

        if (grfidlef == msoidlefNonPeriodic || grfidlef == msoidlefPriority)
        {
                while (lIdleCount <= PRE_MFC_IDLE_PROCESSING_COUNT)
                {
                        MeteredOnIdle(lIdleCount++);
                        if (!pThis->m_srpMsoCompMgr->FContinueIdle())
                                return TRUE;
                }
        }
        
#ifndef _IPC_DOCUMENTS_DISABLED
        // go thru all the components
        POSITION pos = pMainFrm->m_plIPCompFrameHook.GetHeadPosition();
        CIPCompFrameHook * pHook = NULL;
        while (pos != NULL)
        {
                pHook = (CIPCompFrameHook *)pMainFrm->m_plIPCompFrameHook.GetNext(pos);
                fMore |= pHook->FDoIdle(grfidlef);
                if (!pThis->m_srpMsoCompMgr->FContinueIdle())
                        return TRUE;
        }
#endif

        if (grfidlef == msoidlefNonPeriodic || grfidlef == msoidlefPriority)
        {
                while (fMore |= MeteredOnIdle(lIdleCount))
                {
                        if (!pThis->m_srpMsoCompMgr->FContinueIdle())
                                return TRUE;
                                
                        lIdleCount++;
                }
        }
        
        return fMore;
}
                                                        

STDMETHODIMP_(BOOL) CIPCompMainFrame::XMsoComponent::FContinueMessageLoop(
                                                                                                                ULONG /*uReason*/,
                                                                                                                void* /*pvLoopData*/,
                                                                                                                MSG* /*pMsgPeeked*/)
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
    // The MDI frame window never pushes a message loop.
    //
    return TRUE;
}

STDMETHODIMP_(BOOL) CIPCompMainFrame::XMsoComponent::FQueryTerminate(BOOL fPromptUser)
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
        BOOL fRet = TRUE;
        CIPCompFrameHook * pHook = NULL;
        
        CMainFrame * pMainFrm = (CMainFrame *)AfxGetMainWnd();
        ASSERT(pMainFrm->IsKindOf(RUNTIME_CLASS(CMainFrame)));
        
#ifndef _IPC_DOCUMENTS_DISABLED
        POSITION pos = pMainFrm->m_plIPCompFrameHook.GetHeadPosition();
        while (pos != NULL && fRet)
        {
                // since the active frame hook is always at the head of the list, it will get the 
                // the first crack
                pHook = (CIPCompFrameHook *)pMainFrm->m_plIPCompFrameHook.GetNext(pos);
                fRet = pHook->FQueryClose(fPromptUser);
        }
#endif
        
        return fRet;
}

STDMETHODIMP_(void) CIPCompMainFrame::XMsoComponent::Terminate()
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
        pThis->ReleaseComponentManager();
        ASSERT(pThis->m_srpMsoCompMgr == NULL);
}

STDMETHODIMP_(HWND) CIPCompMainFrame::XMsoComponent::HwndGetWindow(DWORD dwWhich, DWORD dwReserved)
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)

        HWND    hwnd = NULL;

        switch (dwWhich)
        {
                /* MDI Apps should return the MDI frame (not MDI client) or App frame
                        window, and SDI Apps should return the frame window which hosts the
                        component. Basically it should be the topmost window which owns the
                        component. For a toolbar set this will be the toplevel owner of
                        TBS::m_hwnd. */
                case msocWindowFrameToplevel:
                        // fall thru
                        
                /* This is the window which owns the component. It could be same as
                        the window obtained by msocWindowFrameTopLevel or be an owned window
                        of that window. For a toolbar set this will be TBS::m_hwnd. */
                case msocWindowFrameOwner:
                        // fall thru
                        
                /* This is the "main" window of the component (if it has one). */
                case msocWindowComponent:
                        // fall thru
                
                /* Caller wishes to display a dialog to be parented by the component.
                        Component should return a window suitable for use as the dialog's
                        owner window. */  
                case msocWindowDlgOwner:
                {
                        CMainFrame * pMainFrm = (CMainFrame *)AfxGetMainWnd();
                        ASSERT(pMainFrm->IsKindOf(RUNTIME_CLASS(CMainFrame)));
                        hwnd = pMainFrm->GetSafeHwnd();
                        break;
                }

                // no default case (NULL is default behavior which is already handled)
        }

        return hwnd;
}

STDMETHODIMP_(BOOL) CIPCompMainFrame::XMsoComponent::FDebugMessage(HMSOINST hinst, UINT message,
                                                                                        WPARAM wParam, LPARAM lParam)
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
        return TRUE;
}

STDMETHODIMP_(ULONG) CIPCompMainFrame::XMsoComponent::AddRef()
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
        return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CIPCompMainFrame::XMsoComponent::Release()
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
        return pThis->ExternalRelease();
}

STDMETHODIMP CIPCompMainFrame::XMsoComponent::QueryInterface(REFIID riid, void **ppvObj)
{
        METHOD_PROLOGUE(CIPCompMainFrame, MsoComponent)
        return pThis->ExternalQueryInterface(&riid, ppvObj);
}

BOOL CIPCompMainFrame::SetComponentManager(IMsoComponentManager * pMsoCompMgr)
{
    MSOCRINFO crinfo;

        ReleaseComponentManager();
        ASSERT(m_srpMsoCompMgr == NULL);
        BOOL fSuccess = TRUE;
        
        if (pMsoCompMgr)
        {
                m_srpMsoCompMgr = pMsoCompMgr;
                m_srpMsoCompMgr->AddRef();

                crinfo.cbSize = sizeof( crinfo );
        crinfo.grfcrf = msocrfNeedIdleTime         |
                                                msocrfNeedPeriodicIdleTime |
                                                msocrfPreTranslateAll;
        crinfo.grfcadvf = msocadvfModal         |
                          msocadvfRedrawOff     |
                          msocadvfWarningsOff;
        crinfo.uIdleTimeInterval = 1000;

                if(!m_srpMsoCompMgr->FRegisterComponent(&m_xMsoComponent,
                                                                                                &crinfo,
                                                                                                &m_dwComponentID))
                {
                        fSuccess = FALSE;
                }
        }

        return fSuccess;
}

void CIPCompMainFrame::ReleaseComponentManager()
{
        if(m_srpMsoCompMgr)
        {
                m_srpMsoCompMgr->FRevokeComponent(m_dwComponentID);
                m_srpMsoCompMgr.SRelease();
        }
}
/*
        NO HMENU SUPPORT.
        OnCreateClient and LoadFrame are responsible for not loading a menu in
        command bar mode.
*/

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
        // only do this once
        ASSERT_VALID_IDR(nIDResource);
        ASSERT(m_nIDHelp == 0 || m_nIDHelp == nIDResource);

        m_nIDHelp = nIDResource;    // ID for help context (+HID_BASE_RESOURCE)

        CString strFullString;
        if (strFullString.LoadString(nIDResource))
                AfxExtractSubString(m_strTitle, strFullString, 0);    // first sub-string

        if (!AfxDeferRegisterClass(AFX_WNDFRAMEORVIEW_REG))
                return FALSE;

        // attempt to create the window
        LPCTSTR lpszClass = GetIconWndClass(dwDefaultStyle, nIDResource);
        LPCTSTR lpszTitle = m_strTitle;
        if (!Create(lpszClass, lpszTitle, dwDefaultStyle, rectDefault,
                                pParentWnd, 
                                Menu::IsShowingCmdBars() ? NULL : MAKEINTRESOURCE(nIDResource), //OLD_MENU Backwards compatibility hmenu mode
                                0L, pContext))
        {
                return FALSE;   // will self destruct on failure normally
        }

        // save the default menu handle
        ASSERT(m_hWnd != NULL);
        m_hMenuDefault = NULL ; // ::GetMenu(m_hWnd);

        // load accelerator resource
        LoadAccelTable(MAKEINTRESOURCE(nIDResource));

        if (pContext == NULL)   // send initial update
                SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

        return TRUE;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
        if (Menu::IsShowingCmdBars()) 
        {
                CMenu* pMenu = NULL;
                return CreateClient(lpcs, pMenu);
        }

        //OLD_MENU Support
        return CMDIFrameWnd::OnCreateClient(lpcs, pContext) ;
}

void CMainFrame::OnUpdateMenuCommand(CCmdUI* pCmdUI)
{
	::UpdateMenuCommand(pCmdUI);
}

void CMainFrame::OnUpdateCustomMenuCommand(CCmdUI* pCmdUI)
{
	::UpdateMenuCommand(pCmdUI);
}

// open the menu as a popup
void CMainFrame::OnMenuCommand(UINT nID)
{
        const MSG *pMsg=GetCurrentMessage();

        CBMenuPopup *pPopup=theCmdCache.GetMenu(nID);

        if(pPopup)
        {
                pPopup->TrackPopupMenu( 0, pMsg->pt.x, pMsg->pt.y, this);
        }
}

void CMainFrame::OnUpdateWindows(CCmdUI* pCmdUI)
{
        pCmdUI->Enable(TRUE);
}

// open the window
void CMainFrame::OnWindows(UINT nID)
{
        int nFilter = FILTER_PROJECT;
        CPartFrame* pMDIChild = theApp.MDITypedGetActive(nFilter, theApp.m_bWindowMenuSorted);

        BOOL bWasZoomed=pMDIChild && pMDIChild->IsZoomed();

        CString buf ;
        CString strTitle ;
        int iMenuId = IDM_WINDOWS_BASE;
        while (pMDIChild != NULL && iMenuId < nID)
        {
                // get the next MDI window
                pMDIChild = theApp.MDITypedGetNext(pMDIChild, nFilter, theApp.m_bWindowMenuSorted);
                iMenuId++;
        }

        if(pMDIChild)
        {
                if(pMDIChild->IsIconic())
                {
                        if(bWasZoomed)
                        {
                                pMDIChild->MDIMaximize();
                        }
                        else
                        {
                                pMDIChild->MDIRestore();
                        }
                }
                pMDIChild->MDIActivate();
                pMDIChild->SetFocus();
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is test harness code, not used for in the shipping app, but still built into it to allow sniffs to run
// on retail builds

// This code is used by testing harnesses, such as the sniffs, to gain access to the command bar menus. The harness sends a custom
// message (DSM_TESTMENU) to DevStudio, including as a parameter an atom id. The atom contains the string of a menu reference. 


// Given an item string and an already found pMenu, this returns a pointer to the menu item.
// The item string should either be in the form He&adline, or alternatively %A. The string must precisely match (ignoring case) 
// the desired item, while the % notation allows matching by mnemonic alone. Submenus or items are not supported here; the pMenu 
// must have already found the right submenu.
// BUG: Mnemonic checking requires the incoming mnemonic to be in upper case. Easy fix, postponed till next cycle.
static BOOL FindItem(CString strItem, CBMenuPopup *pMenu, CBMenuItem **ppItem)
{
		// case insensitive
        strItem.MakeUpper();

		// no submenu support
        ASSERT(strItem.Find('\\')==-1);

		// If we're looking for a mnemonic, extract it. Note: Doesn't support MBCS at the moment. MBCS can't happen in menus, AFAIK.
        BOOL bMnemonic=FALSE;
        TCHAR chMnemonic=' ';
        if(strItem[0]=='%')
        {
                bMnemonic=TRUE;
                chMnemonic=strItem[1];
        }

        BOOL bFound=FALSE;

		// seach all menu  items
        CBMenuItem *pItem=NULL;

        for(int j=0; j<pMenu->GetMenuItemCount(); ++j)
        {
				// extract name
                pItem=pMenu->GetMenuItem(j);
                CString name=pItem->GetName();
                name.MakeUpper();

                TCHAR chCheckMnemonic='-';
                if(bMnemonic)
                {
                        chCheckMnemonic=GLOBAL_DATA::ExtractAccelerator(name);
                }

				// is it the same?
                if(     (!bMnemonic && name==strItem) ||
                        (bMnemonic && chCheckMnemonic==chMnemonic))
                {
                        bFound=TRUE;
                        break;
                }
        }

        if(bFound)
        {
				// return the item
                *ppItem=pItem;
                return TRUE;
        }
        else
        {
                return FALSE;
        }
}

// Given a menu string, possibly including an item string, this returns pointer to the menu and the item within it, plus an index
// into the main menu bar if appropriate.
// The item string should either be in the form F&ind\He&adline, or alternatively %I\%A. The string must precisely match (ignoring case) 
// the desired menu and item, while the % notation allows matching by mnemonic alone.
// BUG: Mnemonic checking requires the incoming mnemonic to be in upper case. Easy fix, postponed till next cycle.
static BOOL FindMenu(const CString &buffer, CBMenuPopup **ppMenu, CBMenuItem **ppItem, int *pMainItem)
{
        CASBar *pBar=CASBar::s_pMenuBar;

		// Is this a menu\item deal?
        int iBS=buffer.Find('\\');
        
		// extract menu 
        CString strMenu;
        if(iBS==-1)
        {
                strMenu=buffer;
        }
        else
        {
                strMenu=buffer.Left(iBS);
        }
        strMenu.MakeUpper();

        BOOL bFound=FALSE;

		// this tracks where the menu is inside the menubar
        *pMainItem=-1;

        // first search all open and visible menus. This gives us submenu support, but only if the submenu has been opened already
        POSITION pos=CBMenuPopup::s_listMenus.GetTailPosition();
        while(pos)
        {
                CBMenuPopup *pMenu=CBMenuPopup::s_listMenus.GetPrev(pos);

                if(pMenu)
                {
						// Check if this menu is a submenu in any existing menu
                        CBMenuItem *pItem=NULL;

                        if(FindItem(strMenu, pMenu, &pItem))
                        {
                                *ppMenu=pMenu;
                                *ppItem=pItem;
                                return TRUE;
                        }
                }
        }

		// Now check the menu bar
        BOOL bMnemonic=FALSE;
        TCHAR chMnemonic=' ';
        if(strMenu[0]=='%')
        {
                bMnemonic=TRUE;
                chMnemonic=strMenu[1];
        }
        int i=0;
        for(i=0; i<pBar->m_nCount; ++i)
        {
				// check all visible menus that aren't already pressed (to allow finding of x\x)
                UINT nStyle=pBar->GetButtonStyle(i);
                if( (nStyle & TBBS_MENU)!=0 &&
                        (nStyle & TBBS_TEXTUAL)!=0 &&
                        (nStyle & TBBS_PRESSED)==0) 
                {
                        CString menu=pBar->GetButtonExtra(i)->GetLabel();

                        menu.MakeUpper();

                        TCHAR chCheckMnemonic='-';
                        if(bMnemonic)
                        {
                                chCheckMnemonic=GLOBAL_DATA::ExtractAccelerator(menu);
                        }

                        if(     (!bMnemonic && menu==strMenu) ||
                                (bMnemonic && chCheckMnemonic==chMnemonic))
                        {
                                *pMainItem=i;
                                bFound=TRUE;
                                break;
                        }
                }
        }

        if(bFound)
        {
                *ppMenu=theCmdCache.GetMenu(pBar->GetItemID(i));
                if(iBS==-1)
                {
                        return TRUE;
                }
                else
                {
                        // now search the items
                        CString strItem=buffer.Right(buffer.GetLength()-iBS-1);

                        return FindItem(strItem, *ppMenu, ppItem);
                }
        }               
        else
        {
                return FALSE;
        }


}

#define MAXMENUNAMELEN 512

// message sent by test harness
LRESULT CMainFrame::OnTestMenu(WPARAM wParam, LPARAM lParam)
{
        LRESULT rv=0;
        ATOM a=(ATOM)lParam;
        CString buffer;
        CBMenuPopup *pMenu=NULL;
        CBMenuItem*pItem=NULL;
        CASBar *pBar=CASBar::s_pMenuBar;
        int nMainItem;
        switch(wParam)
        {
				// This does the relevant menu item
                case cMenuMenu:
                        if(GlobalGetAtomName(a, buffer.GetBuffer(MAXMENUNAMELEN), MAXMENUNAMELEN))
                        {
                                buffer.ReleaseBuffer();
                                if(FindMenu(buffer, &pMenu, &pItem, &nMainItem))
                                {
                                        if(pItem!=NULL)
                                        {
                                                pMenu->Done(pItem->GetCmdID());
                                        }
                                        else
                                        {
                                                ASSERT(nMainItem!=-1);
                                                if(nMainItem>=0)
                                                {
                                                        pBar->PostMessage(DSM_DROPMENU, nMainItem, FALSE);
                                                }
                                        }
                                }
                        }
                        break;

				// check if the item exists
                case cMenuExists:
                        if(GlobalGetAtomName(a, buffer.GetBuffer(MAXMENUNAMELEN), MAXMENUNAMELEN))
                        {
                                buffer.ReleaseBuffer();
                                if(FindMenu(buffer, &pMenu, &pItem, &nMainItem))
                                {
                                        rv=1;
                                }
                        }

                        break;

				// check if item is enabled
                case cMenuEnabled:
                        if(GlobalGetAtomName(a, buffer.GetBuffer(MAXMENUNAMELEN), MAXMENUNAMELEN))
                        {
                                buffer.ReleaseBuffer();
                                if(FindMenu(buffer, &pMenu, &pItem, &nMainItem))
                                {
                                        if(pItem!=NULL)
                                        {
                                                rv=!pItem->m_bGrayed;
                                        }
                                        else
                                        {
                                                ASSERT(nMainItem!=-1);
                                                UINT nStyle=pBar->GetButtonStyle(nMainItem);
                                                rv=((nStyle & TBBS_DISABLED)==0);
                                        }
                                }
                        }
                        break;

				// is item checked?
                case cMenuRealChecked:
                        if(GlobalGetAtomName(a, buffer.GetBuffer(MAXMENUNAMELEN), MAXMENUNAMELEN))
                        {
                                buffer.ReleaseBuffer();
                                if(FindMenu(buffer, &pMenu, &pItem, &nMainItem))
                                {
                                        if(pItem!=NULL)
                                        {
                                                rv=pItem->m_bChecked;
                                        }
                                        else
                                        {
                                                ASSERT(nMainItem!=-1);
                                                UINT nStyle=pBar->GetButtonStyle(nMainItem);
                                                rv=(nStyle & TBBS_CHECKED)!=0;
                                        }
                                }
                        }
                        break;
				
				// items in menu?
                case cMenuCount:
                        if(CASBar::s_pDroppedBar!=NULL &&
                                CASBar::s_nDroppedItem!=-1)
                        {
                                pMenu=CASBar::s_pDroppedBar->GetItemMenu(CASBar::s_nDroppedItem);
                                if(pMenu)
                                {
                                        pMenu=pMenu->FindLastChild();

                                        int nCount=0;

                                        for(int j=0; j<pMenu->GetMenuItemCount(); ++j)
                                        {
                                                pItem=pMenu->GetMenuItem(j);

                                                if(pItem->GetItemType()!=CBMenuItem::MIT_Separator)
                                                {
                                                        ++nCount;
                                                }
                                        }
                                        rv=nCount;
                                }
                                else
                                {
                                        rv=0;
                                }
                        }
                        break;
				
				// unimplemented, unused.
                case cMenuText:
                        // this is nasty to implement, so I'm leaving it until it's used.
                        ASSERT(FALSE);
                        break;

				// close an active menu
                case cMenuEnd:
                        if(CASBar::s_pDroppedBar!=NULL &&
                                CASBar::s_nDroppedItem!=-1)
                        {
                                pMenu=CASBar::s_pDroppedBar->GetItemMenu(CASBar::s_nDroppedItem);
                                if(pMenu)
                                {
                                        pMenu->Done(0);
                                }
                        }
                        break;
                default:
                        ASSERT(FALSE);
                        break;
        }

        return rv;
}
