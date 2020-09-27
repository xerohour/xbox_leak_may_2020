///////////////////////////////////////////////////////////////////////////////
//  DOCKAPI.CPP
//      Implementation of message packing functions for dockable windows.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "memory.h"
#include "dockman.h"
#include "barbdr.h"
#include "main.h"
#include "mainfrm.h"

//[multimon] - 18 mar 97 - der - 
// Macros for unpacking LPARAMs in the multimon world.
// These macros are defined in WINDOWSX.h.
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)	((int)(short)LOWORD(lp))
#endif

#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)	((int)(short)HIWORD(lp))
#endif

#ifdef _WIN32
#define _fmemcpy memcpy
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Registered messages.

UINT DWM_GETDATA = RegisterMessage(_TEXT("DockGetWindowData"));
UINT DWM_GETMOVESIZE = RegisterMessage(_TEXT("DockWndMoveSize"));
UINT DWM_GETTRACKSIZE = RegisterMessage(_TEXT("DockWndTrackSize"));
UINT DWM_ENABLECHILD = RegisterMessage(_TEXT("DockEnableChild"));
UINT DWM_SHOWWINDOW = RegisterMessage(_TEXT("DockShowWindow"));
UINT DWM_CLOSE = RegisterMessage(_TEXT("DockWndClose"));

static BOOL DkNcLButtonDown(HWND hwnd, UINT nHitTest, POINT pt);
static BOOL DkNcLButtonDblClk(HWND hwnd, UINT nHitTest, POINT pt);
static BOOL DkNcMouse(UINT message, HWND hwnd, UINT nHitTest, POINT pt);

//	DkInfoData
//		Packs an array of DOCKINIT structs into shared memory, with a prepended
//		count.  For return when a group UI is called with DCM_GETWINDOW
//		and wParam == 0.

HGLOBAL DkInfoData(UINT nInfoCount, DOCKINIT FAR* lpdiArray)
{
	// FUTURE: Stop using HGLOBALS for this.
	HGLOBAL hglob;
	UINT FAR* lpnSend;
	DOCKINIT FAR* lpdiArraySend;
	
	hglob = GlobalAlloc(GMEM_SHARE, 
		sizeof(UINT) + sizeof(DOCKINIT) * nInfoCount);
	lpnSend = (UINT FAR*) GlobalLock(hglob);
	lpdiArraySend = (DOCKINIT FAR*) (lpnSend + 1);
	
	*lpnSend = nInfoCount;
	_fmemcpy(lpdiArraySend, lpdiArray, sizeof(DOCKINIT) * nInfoCount);

    GlobalUnlock(hglob);
    
    return hglob;
}

//	DkToolbarData
//		Packs TOOLBARINIT and array of command IDs into shared memory.  For
//		return when group UI is called with DCM_GETTOOLBAR.

HGLOBAL DkToolbarData(const TOOLBARINIT* ptbi, const UINT FAR* lpIDArray)
{
	// FUTURE: Stop using HGLOBALS for this.
	HGLOBAL hglob;
	TOOLBARINIT FAR* lptbiSend;
	UINT FAR* lpIDArraySend;
	
	hglob = GlobalAlloc(GMEM_SHARE, 
		sizeof(TOOLBARINIT) + sizeof(UINT) * (ptbi->nIDCount));
	lptbiSend = (TOOLBARINIT FAR*) GlobalLock(hglob);
	lpIDArraySend = (UINT FAR*) (lptbiSend + 1);
	
	_fmemcpy(lptbiSend, ptbi, sizeof(TOOLBARINIT));
	if (ptbi->nIDCount)
		_fmemcpy(lpIDArraySend, lpIDArray, sizeof(UINT) * (ptbi->nIDCount));

    GlobalUnlock(hglob);
    
    return hglob;
}

//	DkRegisterWnd
//		Registers a window with the dock manager.
//		Use this when a runtime event makes a window necessary that has not been
//		registered at InitInstance time, using DkInfoData.

BOOL DkRegisterWnd(HWND hwnd, DOCKINIT* pdi)
{
	CDockManager* pManager = ((CMainFrame *)AfxGetMainWnd())->m_pManager;
	if (pManager == NULL) 
		return FALSE;

	return (pManager->RegisterWnd(hwnd, pdi) != NULL);
}

//	DkRegister
//		Registers a toolbar with the dock manager.
//		Use this when a runtime event makes a toolbar necessary that has not been
//		registered at InitInstance time, using DkInfoData.

BOOL DkRegisterToolbar(DOCKINIT* pdi, TOOLBARINIT* ptbi, UINT FAR* lpIDArray)
{
	CDockManager* pManager = ((CMainFrame *)AfxGetMainWnd())->m_pManager;
	if (pManager == NULL) 
		return FALSE;

	return (pManager->RegisterToolbar(pdi, ptbi, lpIDArray) != NULL);
}

//	DkPreHandleMessage
//		All windows that will appear as descendants of the dock windows should
//		call this function at the beginning of their WindowProc, and end
//		processing, returning the value in pResult, if the function returns
//		TRUE (The message has been handled).

BOOL DkPreHandleMessage(HWND hwnd, UINT message,
	UINT wParam, LPARAM lParam, LRESULT* pResult)
{
	POINT pt;

	*pResult = 0;
	
	switch (message)
	{
	default:
		if (message == DMM_CHILDSETFOCUS ||
			message == DMM_CHILDKILLFOCUS ||
			message == DMM_NCLBUTTONDOWN ||
			message == DMM_NCLBUTTONDBLCLK)
		{
			*pResult = SendMessage(GetParent(hwnd), message, (UINT) hwnd, lParam);
			return TRUE;
		}
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_CANCEL)
		{
			// DevStudio96 RAID #11708 [patbr]:
			// if there are no MDI child windows open, we want to effectively
			// pass the cancel message on to the mainframe's <esc> handler...
			if (theApp.HasOpenDocument(FILTER_NONE))
				theApp.ReactivateView();
			else
				((CMainFrame *)theApp.m_pMainWnd)->OnCancel();

			*pResult = TRUE;
			return TRUE;
		}
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		if (wParam != VK_ESCAPE || (GetKeyState(VK_CONTROL) & ~1) != 0)
			break;

    // Fall through.
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
		SendMessage(GetParent(hwnd), message, wParam, lParam);
		return TRUE;

	case WM_SETFOCUS:
		// Just notify.  Don't handle.
		SendMessage(GetParent(hwnd), DMM_CHILDSETFOCUS, (WPARAM) hwnd, wParam);
		break;

	case WM_KILLFOCUS:
		// Just notify.  Don't handle.
		SendMessage(GetParent(hwnd), DMM_CHILDKILLFOCUS, (WPARAM) hwnd, wParam);
		break;

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
#ifdef _WIN32
		//[multimon] - 18 Mar 97 - der - Use macro instead of LOWORD/HIWORD
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ClientToScreen(hwnd, &pt);
		lParam = MAKELONG((short)pt.x, (short)pt.y);
#else
		ClientToScreen(hwnd, &MAKEPOINT(lParam));
#endif

	// Fall through.
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
		//BLOCK:
		{
			HWND hwndParent = GetParent(hwnd);

#ifdef _WIN32
			//[multimon] - 18 Mar 97 - der - Use macro instead of LOWORD/HIWORD
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			ScreenToClient(hwndParent, &pt);
			lParam = MAKELONG((short)pt.x, (short)pt.y);
#else
			ScreenToClient(hwndParent, &MAKEPOINT(lParam));
#endif
			HWND hwndCapture = GetCapture();
			if (hwndCapture != NULL && hwndCapture != hwnd)
				return FALSE;

			if (message == WM_NCRBUTTONDOWN) message = WM_RBUTTONDOWN;
			if (message == WM_NCRBUTTONUP)	 message = WM_RBUTTONUP;
			SendMessage(hwndParent, message, wParam, lParam);
		}
		return TRUE;
		
	case WM_LBUTTONDOWN:
		wParam = HTCLIENT;
#ifdef _WIN32
		//[multimon] - 18 Mar 97 - der - Use macro instead of LOWORD/HIWORD
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ClientToScreen(hwnd, &pt);
		lParam = MAKELONG((short)pt.x, (short)pt.y);
#else
		ClientToScreen(hwnd, &MAKEPOINT(lParam));
#endif
	
	// Fall through.
	case WM_NCLBUTTONDOWN:
#ifdef _WIN32
		//[multimon] - 18 Mar 97 - der - Use macro instead of LOWORD/HIWORD
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
#else
		pt = MAKEPOINT(lParam);
#endif
		return DkNcLButtonDown(hwnd, wParam, pt);
		
	case WM_LBUTTONDBLCLK:
		wParam = HTCLIENT;
#ifdef _WIN32
		//[multimon] - 18 Mar 97 - der - Use macro instead of LOWORD/HIWORD
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ClientToScreen(hwnd, &pt);
		lParam = MAKELONG((short)pt.x, (short)pt.y);
#else
		ClientToScreen(hwnd, &MAKEPOINT(lParam));
#endif
	
	// Fall through.
	case WM_NCLBUTTONDBLCLK:
#ifdef _WIN32
		//[multimon] - 18 Mar 97 - der - Use macro instead of LOWORD/HIWORD
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
#else
		pt = MAKEPOINT(lParam);
#endif
		return DkNcLButtonDblClk(hwnd, wParam, pt);
	}
	
	return FALSE;
}

//	DkNcLButtonDown
//		Used to translate a WM_NCLBUTTONDOWN into a dock manager message,
//		containing the HWND recipient of the message.

BOOL DkNcLButtonDown(HWND hwnd, UINT nHitTest, POINT pt)
{
    return DkNcMouse(DMM_NCLBUTTONDOWN, hwnd, nHitTest, pt);
}

//	DkNcLButtonDblClk
//		Used to translate a WM_NCLBUTTONDBLCLK into a dock manager message,
//		containing the HWND recipient of the message.

BOOL DkNcLButtonDblClk(HWND hwnd, UINT nHitTest, POINT pt)
{
    return DkNcMouse(DMM_NCLBUTTONDBLCLK, hwnd, nHitTest, pt);
}

//	DkNcMouse
//		Packs mouse message data into shared memory and sends message to
//		message to parent.  (Internal use only).

BOOL DkNcMouse(UINT message, HWND hwnd, UINT nHitTest, POINT pt)
{
    HWND hwndParent;
    HGLOBAL hglob;
    MOUSESTRUCT FAR* lpmst;
    LONG lResult;
    
    hwndParent = GetParent(hwnd);
    hglob = GlobalAlloc(GMEM_SHARE, sizeof(MOUSESTRUCT));
    lpmst = (MOUSESTRUCT FAR*) GlobalLock(hglob);
    
    lpmst->nHitTest = nHitTest;
    lpmst->pt = pt;

    lResult = SendMessage(hwndParent, message, (UINT) hwnd,
    	(LPARAM) (LPVOID) hglob);
    
    GlobalUnlock(hglob);
    GlobalFree(hglob);
    
    return (BOOL) lResult;
}

//	DkWGetDock
//		Returns the DOCKPOS of the window.

DOCKPOS DkWGetDock(UINT nID)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager != NULL) 
	{
		CDockWorker* pWorker = pManager->WorkerFromID(nID);
		if (pWorker != NULL)
			return pWorker->GetDock();
	}

	return dpHide;
}

//	DkWShowWindow
//		Tells the dock worker to show its window.

void DkWShowWindow(UINT nID, BOOL bShow)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager == NULL) 
		return;

	CDockWorker* pWorker = pManager->WorkerFromID(nID);
	if (pWorker != NULL)
	{
		pWorker->ShowWindow(bShow);
	}
}

//	DkWToggleMDIState
//		Toggles between MDI and docking view.

void DkWToggleMDIState(UINT nID)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager == NULL) 
		return;

	CDockWorker* pWorker = pManager->WorkerFromID(nID);
	if (pWorker != NULL)
		pWorker->ToggleMDIState();
}

//	DkWMoveWindow
//		Used to move a window within the dock model.  Use dpCurrent, if you
//		want to keep current dock (also allows window to stay hidden if it
//		is already).

void DkWMoveWindow(UINT nID, DOCKPOS dp, LPCRECT lpRect)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager == NULL) 
		return;

	CDockWorker* pWorker = pManager->WorkerFromID(nID);
	if (pWorker != NULL)
	{
		CRect rect(lpRect);
		pWorker->Move(dp, rect);
	}
}

//	DkWGetTitle
//		Returns the title of the worker

void DkWGetTitle(UINT nID, CString& str)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager == NULL)
		return;

	CDockWorker* pWorker = pManager->WorkerFromID(nID);
	if (pWorker != NULL)
		pWorker->GetText(str);
}

//	DkWSetTitle
//		Sets the worker's title.

void DkWSetTitle(UINT nID, const CString& str)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager == NULL)
		return;

	CDockWorker* pWorker = pManager->WorkerFromID(nID);
	if (pWorker != NULL)
		pWorker->SetText(str);
}

//	DkWRemove
//		Allows a group UI to remove a registered window for the dock
//		manager, if necessary.

void DkWRemove(UINT nID)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame *)AfxGetMainWnd())->m_pManager;
	if (pManager != NULL) 
		pManager->RemoveWnd(nID);
}

//	DkWGetMaxState
//		Get the maximize state of a stretchy docked window, for
//		drawing a maximize/restore button, and performing the correct
//		action when the user clicks on the button.

MAX_STATE DkWGetMaxState(UINT nID)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager == NULL)
		return stateDisabled;

	CDockWorker* pWorker = pManager->WorkerFromID(nID);
	if (pWorker == NULL)
		return stateDisabled;

	return pWorker->GetMaxState();
}

//	DkWMaximizeRestore
//		Maximize or restore a stretchy docked window.

void DkWMaximizeRestore(UINT nID)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager == NULL)
		return;

	CDockWorker* pWorker = pManager->WorkerFromID(nID);
	if (pWorker != NULL)
		pWorker->MaximizeRestore();
}

void DkWMapDockingCmd(UINT nID, UINT nCmd)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	((CMainFrame*) AfxGetMainWnd())->MapDockingCmd(nID, nCmd);
}

void DkWHandleDockingCmd(UINT nCmd, BOOL bToggle, CCmdUI* pCmdUI /*=NULL*/)
{
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager != NULL) 
		pManager->HandleWorkerCmd(nCmd, bToggle, pCmdUI);
}

// embeddeds a given worked inside another window, by deparenting it.
HWND DkWEmbed(UINT nID, HWND hwndParent)
{
	ASSERT(HIWORD(nID) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager == NULL)
		return NULL;

	CDockWorker* pWorker = pManager->WorkerFromID(nID);
	if (pWorker != NULL)
	{
		pWorker->ShowWindow(FALSE);
		pWorker->ShowWindow(TRUE);

		pWorker->Move(dpEmbedded, CRect(0,0,0,0));

		pWorker->DeparentWindow(CWnd::FromHandle(hwndParent), NULL);

		return pWorker->m_pWnd->GetSafeHwnd();
	}

	return NULL;
}

void DkWAssociate(UINT nID, UINT nIDHost)
{
	ASSERT(HIWORD(nID) != 0);		// Need package ID in HiWord. Use MAKEDOCKID macro.
	ASSERT(HIWORD(nIDHost) != 0);	// Need package ID in HiWord. Use MAKEDOCKID macro.
	CDockManager* pManager = ((CMainFrame*) AfxGetMainWnd())->m_pManager;
	if (pManager == NULL)
		return;

	CDockWorker* pWorker = pManager->WorkerFromID(nID);
	CDockWorker* pHostWorker = pManager->WorkerFromID(nIDHost);
	if (pWorker != NULL && pHostWorker !=NULL)
	{
		// set up associations
		pHostWorker->Associate(nID, TRUE);
		pWorker->Associate(pHostWorker->m_nIDWnd, FALSE);
	}
}

//	DkRecalcBorders
//		For dtBorder/dtEdit windows that have toolbars (like Locals and
//		Watch windows).  To make everything look symetric, border bars
//		need to change their borders when the toolbar is shown or hidden.

void DkRecalcBorders(CWnd* pWnd)
{
	CBorderBar* pBar = (CBorderBar*) pWnd->GetParent();
	if (pBar != NULL && pBar->IsKindOf(RUNTIME_CLASS(CBorderBar)))
		pBar->RecalcLayout();
}

//	DkGetDockState
//		Check current dock manager layout.

int DkGetDockState()
{
	return ((CMainFrame *)AfxGetMainWnd())->m_nManagerState;
}

//	DkSetDockState
//		Load a particular dock manager layout.

void DkSetDockState(int nState, int nMask /* =1 */)
{
	((CMainFrame *)AfxGetMainWnd())->SetDockState(nState, nMask);
}

void DkShowManager(BOOL bShow, UPDATE_SHOW update /*=updNow*/)
{
	CDockManager* pManager = ((CMainFrame *)AfxGetMainWnd())->m_pManager;
	if (pManager != NULL) 
		pManager->ShowManager(bShow, update);
	
}

BOOL DkDocObjectMode(BOOL bShow)
{
	CDockManager* pManager = ((CMainFrame *)AfxGetMainWnd())->m_pManager;
	if (pManager == NULL) 
		return FALSE;
	return pManager->DocObjectMode(bShow);
}

void DkUpdateAvailableWnds(BOOL bUpdateNow /*=FALSE*/)
{
	CDockManager* pManager = ((CMainFrame *)AfxGetMainWnd())->m_pManager;
	if (pManager == NULL)
		return;

	pManager->SetAvailableWndDirty();
	if (bUpdateNow)
		pManager->IdleUpdateAvailableWnds();
}

//	DkLockWorker
//		Keep the current worker active even though it may lose focus.

void DkLockWorker(BOOL bLock)
{
	CDockWorker::LockWorker(bLock);
}

