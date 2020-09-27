/////////////////////////////////////////////////////////////////////////
//
//	MWBAR.CPP - Memory Window Toolbar Handler
//
//	Copyright (C)1994, Microsoft Corporation
//
//	Purpose: To display and update the memory windows.
//
//  Revision History:
//       Written by: Mark A. Brodsky
//
// 	Notes:
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop


////////////////////////////////////////////////////////////////////////
//
// CMemoryParent
//
// holds the gizmos at the top of the Memory window
//

IMPLEMENT_DYNAMIC(CMemoryParent, CWnd);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMemoryParent, CWnd)
	//{{AFX_MSG_MAP(CMemoryParent))
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMemoryParent::CMemoryParent()
{
	m_pView = NULL;
}

int CMemoryParent::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// now create our toolbar...

	m_toolbar.SetDockableID(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_MEMORY_WIN));
	if (!m_toolbar.Create(this, IDD_MEMORYVIEW_DIALOGBAR,
		CBRS_ALIGN_TOP | CBRS_BORDER_3D, AFX_IDW_TOOLBAR))
	{		
		TRACE("Failed to Create/Init Toolbar\n");
		return -1;
	}

	// must manually initialize the dialog bar because there will be no
	// WM_INIT message as there is no dialog per se.  We are just a dialog
	// template...

	if (!m_toolbar.OnInitDialog())
	{
		TRACE("Failed to Init Toolbar\n");
		return -1;
	}

	if (!runDebugParams.fMemoryToolbar)
	{
		m_toolbar.ShowWindow(SW_HIDE);
	}

	return 0;
}

void CMemoryParent::PostNcDestroy()
{
	delete this;	// Dockable windows delete themselves.
}

LRESULT CMemoryParent::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	if (DkPreHandleMessage(m_hWnd, message, wParam, lParam, &lResult))
		return lResult;
	return CWnd::WindowProc(message, wParam, lParam);
}

void CMemoryParent::OnSetFocus(CWnd* pOldWnd)
{
	ASSERT(m_pView != NULL && m_pView->m_hWnd != NULL);
	m_pView->SetFocus();
}

void CMemoryParent::OnSize(UINT nType, int cx, int cy)
{
	if (nType != SIZE_MINIMIZED && cx > 0 && cy > 0)
		RecalcLayout();

	CWnd::OnSize(nType, cx, cy);
}


void CMemoryParent::RecalcLayout()
{
	DOCKPOS dp = DkWGetDock(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_MEMORY_WIN));
	if (UseWin4Look())
		m_toolbar.RecalcBarLayout(dp);

	UINT nIdMemory = IDDW_VCPP_MEMORY_WIN;
	if (dp == dpMDI)
		nIdMemory = AFX_IDW_PANE_FIRST;
	RepositionBars(0, 0xffff, nIdMemory);

	// now do the horizontal computation to move the items to take up available space...

	CRect rcBar, rcEdit, rcBut;
	m_toolbar.GetWindowRect(&rcBar);
	m_toolbar.m_edit.GetWindowRect(&rcEdit);

	int wAvail = rcBar.right - rcBar.left;	 // width of window
	int iOffs  = rcEdit.left - rcBar.left;	 // offset of edit item

	wAvail -= iOffs + 6;			// remove used space + padding

// the following code is here as a reference for when the memory window's
// toolbar has buttons.  The code is just copied out of watchwin.c's toolbar
// stuff
//	int spBut  = rcBut.left - rcEdit.right;	 // spacing to button
//	int wBut   = rcBut.right - rcBut.left;   // width of button
//	wAvail -= iOffs + spBut + wBut + 6;	 	// remove used space + padding

	// we now know how wide we can make the edit item, but lets not
	// make it too small... we'd rather let the button fall off the edge
	// if its going to be too small

//	if (wAvail < 150) wAvail = 150;
	if (wAvail < 24) wAvail = 24;
	if (wAvail == rcEdit.Width()) return;

	HDWP hdwp = ::BeginDeferWindowPos(1);
	hdwp = ::DeferWindowPos(hdwp,m_toolbar.m_edit.m_hWnd,NULL,0,0,wAvail,rcEdit.Height(),
		SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	m_toolbar.m_edit.ShowWindow(SW_HIDE);	// 3-d fix - keep window invalid.
	::EndDeferWindowPos(hdwp);
	m_toolbar.m_edit.ShowWindow(SW_SHOWNOACTIVATE);
}

BOOL CMemoryParent::PreTranslateMessage(MSG *pmsg)
{
	if (pmsg->message == WM_SYSKEYDOWN && m_toolbar.PreTranslateSysKey(pmsg))
		return TRUE;

	return CWnd::PreTranslateMessage(pmsg);	
}

BOOL CMemoryParent::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// Route notifies to main window, for tool tip text handling.
	*pResult = AfxGetMainWnd()->SendMessage(WM_NOTIFY, wParam, lParam);

	return TRUE;
}

//////
// CMemoryDialogBar
//

BEGIN_MESSAGE_MAP(CMemoryDialogBar, C3dDialogBar)
	//{{AFX_MSG_MAP(CMemoryDialogBar))
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CMemoryDialogBar::OnInitDialog()
{
	if (!m_edit.SubclassDlgItem(IDM_CTL_MEMORY_EDIT, this))
	{
		return FALSE;
	}

	m_edit.SetFont(GetStdFont(font_Normal));
	if (UseWin4Look())
	{
		// Use client edge instead of border, but make sure the client are
		// size remains constant.

		CRect rectClient;
		m_edit.GetClientRect(rectClient);

		m_edit.ModifyStyle(WS_BORDER, 0);
		m_edit.ModifyStyleEx(0, WS_EX_CLIENTEDGE);	// Dialog bars don't get 3d dialog.

		::AdjustWindowRectEx(rectClient, m_edit.GetStyle(),
			FALSE, m_edit.GetExStyle());

		m_edit.SetWindowPos(NULL, 0, 0, rectClient.Width(), rectClient.Height(),
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOREDRAW);
	}
	
	RecalcBarLayout(DkWGetDock(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_MEMORY_WIN)));

	return TRUE;
}

void CMemoryDialogBar::OnDestroy()
{
	// FUTURE: MFC workaround remove when Olympus #2139 is fixed [rm]
	// this could cancel someone elses tooltip but that's OK since
	// some UI just happened to get rid of this window anyway
	CControlBar::CancelToolTips();
}

void CMemoryDialogBar::OnUpdateCmdUI(CFrameWnd*, BOOL)
{
}

///////
//
// CMemoryEditItem
//

BOOL CMemoryEditItem::PreTranslateMessage(MSG *pmsg)
{
	switch (pmsg->message)
	{
	case WM_KEYDOWN:
		if (pmsg->wParam == VK_RETURN)
		{
			char	sz[ axMax ];
			MWI		mwi = {0};
			CString	str;

			GetWindowText( str );

			_ftcsncpy( sz, (LPCTSTR)str, sizeof( sz ) );
		    sz[ sizeof( sz ) - 1 ] = '\0';

			mwi.szExpr = sz;

		    if ( MWSetAddrFromExpr( &mwi, runDebugParams.iMWAutoTypes ) )
		    {
				// If we get here the window MUST be open already
				ASSERT( pViewMemory );

				// Update the global address
				MWSaveSettings( sz );

				// Update the memory window
				pViewMemory->SendMessage( WM_COMMAND, 0, 0L );
			}
			else
			{
				CVMessage( ERRORMSG, BADADDR, MSGSTRING, (char FAR *)sz );
				DebuggerMessage( Error, 0, (LPSTR)sz, SHOW_IMMEDIATE );
				SetFocus();
				SetSel( 0, -1, TRUE );
			}

			return TRUE;
		}
		else if ( pmsg->wParam == VK_ESCAPE &&
			!( GetKeyState( VK_SHIFT ) & ~1 ) &&
			!( GetKeyState( VK_CONTROL ) & ~1 ))
			{
				ASSERT( pViewMemory );

				PMWI	pmwi = &((CMemoryView *)pViewMemory)->m_mwi;

				SetWindowText( (LPCSTR)pmwi->szExpr );
				SetSel( 0, -1, TRUE );
				pViewMemory->SetFocus();
				return TRUE;
			}
	}

	return CDockEdit::PreTranslateMessage(pmsg);	
}
