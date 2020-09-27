// undoctl.cpp : implementation of undo/redo controls
//

#include "stdafx.h"
#include "undoctl.h"
#include "util.h"
#include "resource.h"
#include "ctl3d.h"
#include "bardockx.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// These two WM_ messages are sent to the current active frame in
// order to get the strings used for filling the dropdown list for
// the undo/redo controls.
UINT NEAR WM_GETUNDOSTRING = RegisterMessage("GetUndoString");
UINT NEAR WM_GETREDOSTRING = RegisterMessage("GetRedoString");
UINT NEAR WM_DESTROYSELF = RegisterMessage("DestroySelf");

/////////////////////////////////////////////////////////////////////////////
// CDropControl

BEGIN_MESSAGE_MAP(CDropControl, CWnd)
	//{{AFX_MSG_MAP(CDropControl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_ENABLE()
	ON_REGISTERED_MESSAGE(DSM_BARSIZECHANGING,OnBarSizeChanging)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CDropControl, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CDropControl construction/destruction

CDropControl::CDropControl()
{
	m_buttonState = BS_DEFAULT;
	m_bShowList = FALSE;
	m_pMainWnd = NULL;

	// adopt correct initial sizes
	SetButtonSize(theApp.m_bLargeToolBars);
}

CDropControl::~CDropControl()
{
}

BOOL CDropControl::Create(DWORD dwStyle, const RECT& rect,
                          CWnd* pParentWnd, UINT nID,
                          CCreateContext* pContext)
{
	CRect rc = CRect(rect.left, rect.top, rect.left + m_width, rect.top + m_height);

	if (! CWnd::Create(NULL, "", dwStyle, rc, pParentWnd, nID, pContext))
		return FALSE;

	m_pMainWnd = pParentWnd;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CDropControl drawing
void CDropControl::OnPaint()
{
	CRect rc;
	GetUpdateRect(&rc);
	if (rc.IsRectEmpty())	// happens when UpdateWindow called.
		return;
	
	CPaintDC dc(this); // device context for painting
	int offset = 3;
	BOOL bEnabled = IsWindowEnabled();
	
	UINT idButton;
	if ((m_buttonState == BS_LCLICK) && PointOnControl(m_mousePos))
	{
		if(m_bLarge) {
			idButton = theApp.m_bWin4 ? IDB_DROPCL95LARGE : IDB_DROPCLLARGE;
		} else {
			idButton = theApp.m_bWin4 ? IDB_DROPCL95 : IDB_DROPCL;
		}
	}
	else if ((m_buttonState == BS_RCLICK) && PointOnControl(m_mousePos))
	{
		if(m_bLarge) {
			idButton = theApp.m_bWin4 ? IDB_DROPCR95LARGE : IDB_DROPCRLARGE;
		} else {
			idButton = theApp.m_bWin4 ? IDB_DROPCR95 : IDB_DROPCR;
		}
	}
	else if (bEnabled)
	{
		if(m_buttonState == BS_EXTRUDED)
		{
			if(m_bLarge) {
				idButton = theApp.m_bWin4 ? IDB_DROP95LARGE : IDB_DROPLARGE;
			} else {
				idButton = theApp.m_bWin4 ? IDB_DROP95 : IDB_DROP;
			}
		}
		else
		{
			if(m_bLarge) {
				idButton = theApp.m_bWin4 ? IDB_DROP95LARGEFLAT : IDB_DROPLARGEFLAT;
			} else {
				idButton = theApp.m_bWin4 ? IDB_DROP95FLAT : IDB_DROPFLAT;
			}
		}
	}
	else
	{
		if(m_bLarge) {
			idButton = theApp.m_bWin4 ? IDB_DROPDISABLED95LARGE : IDB_DROPDISABLEDLARGE;
		} else {
			idButton = theApp.m_bWin4 ? IDB_DROPDISABLED95 : IDB_DROPDISABLED;
		}
	}

	// Stolen and adapted from MFC CToolBar::LoadBitmap [marklam 1-20-95]
	HINSTANCE hInstImageWell = AfxFindResourceHandle(MAKEINTRESOURCE(idButton), RT_BITMAP);
	HRSRC hRsrcImageWell = ::FindResource(hInstImageWell, MAKEINTRESOURCE(idButton), RT_BITMAP);
	HBITMAP hbitmap = AfxLoadSysColorBitmap(hInstImageWell, hRsrcImageWell);
	
	CDC dcTemp;
	dcTemp.CreateCompatibleDC(&dc);
	CBitmap* pOldBitmap = dcTemp.SelectObject(CBitmap::FromHandle(hbitmap));
	dc.BitBlt(rc.left, rc.top, rc.right, rc.bottom, &dcTemp, rc.left, rc.top, SRCCOPY);
	dcTemp.SelectObject(pOldBitmap);
	::DeleteObject(hbitmap);
	
	DrawArrow(dc, dcTemp, offset, offset, bEnabled);
}

void CDropControl::DrawArrow(CDC& dc, CDC& dcTemp, int xdest, int ydest, BOOL bEnabled)
{
	UINT idButton;
	if(m_bLarge) {
		idButton=bEnabled ? m_idLargeEnabledButton : m_idLargeDisabledButton;
	} else {
		idButton=bEnabled ? m_idSmallEnabledButton : m_idSmallDisabledButton;
	}

	// Stolen and adapted from MFC CToolBar::LoadBitmap [marklam 1-21-95]
	HINSTANCE hInstImageWell = AfxFindResourceHandle(MAKEINTRESOURCE(idButton), RT_BITMAP);
	HRSRC hRsrcImageWell = ::FindResource(hInstImageWell, MAKEINTRESOURCE(idButton), RT_BITMAP);
	HBITMAP hbitmap = AfxLoadSysColorBitmap(hInstImageWell, hRsrcImageWell);
	
	CBitmap* pOldBitmap = dcTemp.SelectObject(CBitmap::FromHandle(hbitmap));
	dc.BitBlt(xdest, ydest, m_bLarge ? largeImageWidth : smallImageWidth, 
							m_bLarge ? largeImageHeight : smallImageHeight,
							&dcTemp, 0, 0, SRCCOPY);
	dcTemp.SelectObject(pOldBitmap);
	::DeleteObject(hbitmap);
}

/////////////////////////////////////////////////////////////////////////////
// CDropControl message handlers

LRESULT CDropControl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG lResult;

	if (DkPreHandleMessage(GetSafeHwnd(),
		message, wParam, lParam, &lResult))
	{
		return lResult;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

void CDropControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	ASSERT(PointOnControl(point));
	ASSERT(m_buttonState != BS_LCLICK);
	ASSERT(m_buttonState != BS_RCLICK);

	if (m_buttonState == BS_DEFAULT ||
		m_buttonState == BS_EXTRUDED)
	{
		// user clicks on control; enter LCLICK or RCLICK state as appropriate
		if (PointOnLeft(point))
			SetState(BS_LCLICK);
		else
			SetState(BS_RCLICK);
	}
	else
	{
		// user clicks on control while in drop state; collapse dropdown
		ASSERT(m_buttonState == BS_DROP);
		SetState(BS_EXTRUDED);
	}
	
	m_mousePos = point;
}

void CDropControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	switch(m_buttonState)
	{
		case BS_DEFAULT:
			break;

		case BS_LCLICK:
			if (PointOnControl(point))
				UndoEvents(1);
			break;
		
		case BS_RCLICK:
			if (PointOnControl(point))
			{
				SetState(BS_DROP);
				InvalidateRight();
				return;
			}
			
			// user released the mouse somewhere other than on the control
			// while the dropdown was shown.  We may need to undo a bunch
			// of items.
			if (! (PointOnScrollDown(point) || PointOnScrollUp(point)))
				UndoEvents(CalcSel(point) + 1);
			else
				UndoEvents(m_undoList.GetSel() + 1);
			break;
		
		case BS_DROP:
			// if our mode cancellation code is working, this can't happen
			break;

		case BS_EXTRUDED:
			return;
	}

	SetState(BS_EXTRUDED);
}

void CDropControl::OnEnable(BOOL bEnable)
{
	if(!bEnable)
	{
		if(m_buttonState != BS_DEFAULT)
		{
			SetState(BS_DEFAULT);
		}
	}
}

void CDropControl::OnMouseMove(UINT nFlags, CPoint point)
{
	switch (m_buttonState)
	{
		case BS_LCLICK:
			if (PointOnControl(point) != PointOnControl(m_mousePos))
				InvalidateLeft();
			break;
			
		case BS_RCLICK:
			if (PointOnControl(point) != PointOnControl(m_mousePos))
				InvalidateRight();
			
			if (! (PointOnScrollDown(point) || PointOnScrollUp(point)))
				m_undoList.SetSel(CalcSel(point));
			
			break;
		
		case BS_DROP:
			break;

		case BS_DEFAULT:
			{
				if (theApp.ShouldExtrude())
				{
					SetState(BS_EXTRUDED);
				}
			}
			break;

		case BS_EXTRUDED:
			if(IsWindowEnabled())
			{
				CPoint screenPoint=point;
				ClientToScreen(&point);
				if(CWnd::WindowFromPoint(point)!=this)
				{
					// time to be unextruded
					SetState(BS_DEFAULT);
				}
			}
			break;
		
		default:
			ASSERT(FALSE);
	}
	
	m_mousePos = point;
}

void CDropControl::OnListBoxLButtonDown(UINT nFlags, CPoint point)
{
	ASSERT(m_buttonState == BS_DROP);
	
	// called when the user clicks directly on the listbox.
	SetState(BS_RCLICK);
	m_undoList.SetSel(CalcSel(point));
	m_mousePos = point;
}

void CDropControl::OnListBoxLButtonUp(UINT nFlags, CPoint point)
{
	// FUTURE - if our mode cancellation stuff is working, this should
	// never happen because any LButtonDown event not on one of our
	// windows should collapse the dropdown.
	ASSERT(m_buttonState == BS_DROP);
	CancelModes();
}

LRESULT CDropControl::OnBarSizeChanging(WPARAM wParam, LPARAM lParam)
{
	if(lParam==_getpid())
	{
		SetButtonSize(wParam);

		// resize the window
		SetWindowPos(NULL,0,0,m_width,m_height,SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOREDRAW);
	}

	return 0;
}

void CDropControl::SetButtonSize(BOOL bLarge)
{
	m_bLarge=bLarge;
	if(bLarge) {
		m_width=largeWidth;
		m_height=largeHeight;
		m_leftWidth=largeLeftWidth;
	} else {
		m_width=smallWidth;
		m_height=smallHeight;
		m_leftWidth=smallLeftWidth;
	}
}

int CDropControl::GetExpectedWidth(void)
{
	return(theApp.m_bLargeToolBars ? largeWidth : smallWidth);
}

int CDropControl::GetExpectedHeight(void)
{
	return(theApp.m_bLargeToolBars ? largeHeight : smallHeight);
}

BOOL CDropControl::PointOnControl(CPoint point)
{
	return ((point.x <= m_width) && (point.x >= 0) &&
	        (point.y <= m_height) && (point.y >= 0));
}

BOOL CDropControl::PointOnLeft(CPoint point)
{
	return (point.x <= m_leftWidth);
}

BOOL CDropControl::PointOnListBox(CPoint point)
{
	if (!m_bShowList)
		return FALSE;
	
	CRect rc;
	m_undoList.m_listBox.GetWindowRect(&rc);
	ScreenToClient(&rc);
	
	return rc.PtInRect(point);
}

BOOL CDropControl::PointOnScrollDown(CPoint point)
{
	if (!m_bShowList)
		return FALSE;
	
	CRect rc, rcListBox;
	m_undoList.m_listBox.GetWindowRect(&rcListBox);
	m_undoList.GetWindowRect(&rc);

	ClientToScreen(&point);
	
	return (rc.PtInRect(point) && (point.y >= rcListBox.bottom));
}

BOOL CDropControl::PointOnScrollUp(CPoint point)
{
	if (!m_bShowList)
		return FALSE;
	
	CRect rc, rcListBox;
	m_undoList.m_listBox.GetWindowRect(&rcListBox);
	m_undoList.GetWindowRect(&rc);
	
	// fudge factor to provide a good-sized upwards scroll region
	rc.top = rc.top - CUndoList::STATUSHEIGHT - CUndoList::BORDERSIZE * 2;

	ClientToScreen(&point);
	return (rc.PtInRect(point) && (point.y < rcListBox.top));
}

int CDropControl::CalcSel(CPoint point)
{
	ASSERT (m_bShowList);
	
	ClientToScreen(&point);
	return m_undoList.CalcSel(point);
}

void CDropControl::OnTimer(UINT nIDEvent)
{
	ASSERT(nIDEvent == ID_TIMER);

	if (PointOnScrollDown(m_mousePos))
		m_undoList.ScrollDown();
	else if (PointOnScrollUp(m_mousePos) && !PointOnControl(m_mousePos))
		m_undoList.ScrollUp();
}

void CDropControl::UndoEvents(int n)
{
	// Send an appropriate number of ID_EDIT_UNDO or ID_EDIT_REDO
	// commands to the main frame.
	while (n-- > 0)
		m_pMainWnd->SendMessage(WM_COMMAND, GetDlgCtrlID(), 0);
}

void CDropControl::SetState(int bsState)
{
	// if the dropdown is collapsing, give the focus back to the current
	// active window.
//	FUTURE: No, this will cause a CancelMode in CDropControl::OnKillFocus.
//	if (m_buttonState == BS_DROP)
//		::SetFocus(::GetActiveWindow());
	
	// now adjust the mouse capture, timer state, and show/hide of the
	// dropdown list as appropriate for the new state.

	switch (bsState)
	{
		case BS_LCLICK:
			ASSERT((m_buttonState == BS_DEFAULT) || (m_buttonState == BS_EXTRUDED));
			SetCapture();
			SetTimer(ID_TIMER, MSEC_TIMER, NULL);
			InvalidateLeft();
			break;

		case BS_RCLICK:
			ASSERT((m_buttonState == BS_DEFAULT) || (m_buttonState == BS_EXTRUDED) || (m_buttonState == BS_DROP));
			ShowList(TRUE);
			SetCapture();
			SetTimer(ID_TIMER, MSEC_TIMER, NULL);
			InvalidateRight();
			break;

		case BS_DROP:
			ASSERT(m_buttonState == BS_RCLICK);
			if(GetCapture()==this)
			{
				::ReleaseCapture();
			}
			KillTimer(ID_TIMER);
			m_undoList.SetSel(0);	// select first item by default
			SetFocus();
			break;

		case BS_DEFAULT:
			if ((m_buttonState == BS_DROP) || (m_buttonState == BS_RCLICK))
				ShowList(FALSE);
			
			if ((m_buttonState == BS_LCLICK) || (m_buttonState == BS_RCLICK) || (m_buttonState == BS_EXTRUDED))
			{
				if(GetCapture()==this)
				{
					::ReleaseCapture();
				}
			}

			if ((m_buttonState == BS_LCLICK) || (m_buttonState == BS_RCLICK))
			{
				KillTimer(ID_TIMER);
			}

			BOOL fWasExtruded;
			fWasExtruded = (m_buttonState == BS_EXTRUDED); // order is everything! bobz 
			m_buttonState = bsState;
			if (!fWasExtruded)  // no focus change just turning extruded off
				::SetFocus(theApp.GetActiveView());
			
			Invalidate();
			break;

		case BS_EXTRUDED:
			if ((m_buttonState == BS_DROP) || (m_buttonState == BS_RCLICK))
				ShowList(FALSE);

			if ((m_buttonState == BS_LCLICK) || (m_buttonState == BS_RCLICK))
			{
				KillTimer(ID_TIMER);
			}

			m_buttonState = bsState;
			SetCapture();
			Invalidate();
			break;
		
		default:
			ASSERT(FALSE);
	}
	
	m_buttonState = bsState;
}

void CDropControl::ShowList(BOOL bShow)
{
	if (bShow == m_bShowList)
		return;

	if (bShow)
	{
		CRect rc;
		GetWindowRect(&rc);
		rc.top = rc.bottom + 1;
		rc.bottom = rc.top + 100;
		rc.right = rc.left + 150;
		m_undoList.ShowList(TRUE, rc, this);
		m_bShowList = TRUE;
		SetFocus();
	}
	else
	{
		m_undoList.ShowList(FALSE, CRect(0,0,0,0), NULL);
		m_bShowList = FALSE;
	}
}

void CDropControl::InvalidateLeft()
{
	CRect rc = CRect(0, 0, m_leftWidth, m_height);
	InvalidateRect(&rc);
}

void CDropControl::InvalidateRight()
{
	CRect rc = CRect(m_leftWidth, 0, m_width, m_height);
	InvalidateRect(&rc);
}

void CDropControl::OnKillFocus(CWnd* pNewWnd)
{
	CancelModes();
	CWnd::OnKillFocus(pNewWnd);
}

UINT CDropControl::OnGetDlgCode()
{
	return DLGC_BUTTON | DLGC_WANTARROWS;
}

void CDropControl::CancelModes()
{
//	May be caused by a focus change in UndoEvents.
//	ASSERT(m_buttonState != BS_LCLICK);
//	ASSERT(m_buttonState != BS_RCLICK);
	
	if (m_buttonState != BS_DEFAULT)
		SetState(BS_DEFAULT);
}


BOOL CDropControl::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_SYSKEYDOWN))
	{
		switch (pMsg->wParam)
		{
			case VK_ESCAPE:
				CancelModes();
				return TRUE;
			
			case VK_RETURN:
				UndoEvents(m_undoList.GetSel() + 1);
				CancelModes();
				return TRUE;

			case VK_UP:
				m_undoList.SetSel(m_undoList.GetSel() - 1);
				return TRUE;
			
			case VK_DOWN:
				m_undoList.SetSel(m_undoList.GetSel() + 1);
				return TRUE;

			case VK_PRIOR:
				m_undoList.PageUp();
				return TRUE;

			case VK_NEXT:
				m_undoList.PageDown();
				return TRUE;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CDropControl::PostNcDestroy()
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// CUndoControl and CRedoControl
//
// The generic DropControl class obviously handles nearly everything, but
// there are a few things related to user interface display which must be
// done differently for the Undo and Redo buttons.

BEGIN_MESSAGE_MAP(CUndoControl, CDropControl)
	//{{AFX_MSG_MAP(CUndoControl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CRedoControl, CDropControl)
	//{{AFX_MSG_MAP(CRedoControl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUndoControl and CRedoControl construction/destruction

CUndoControl::CUndoControl()
{ 
	m_idSmallEnabledButton = IDB_UNDO;
	m_idSmallDisabledButton = IDB_UNDODISABLED;
	m_idLargeEnabledButton = IDB_UNDOLARGE;
	m_idLargeDisabledButton = IDB_UNDODISABLEDLARGE;
}

CUndoControl::~CUndoControl()
{ }

CRedoControl::CRedoControl()
{ 
	m_idSmallEnabledButton = IDB_REDO;
	m_idSmallDisabledButton = IDB_REDODISABLED;
	m_idLargeEnabledButton = IDB_REDOLARGE;
	m_idLargeDisabledButton = IDB_REDODISABLEDLARGE;
}

CRedoControl::~CRedoControl()
{ }

/////////////////////////////////////////////////////////////////////////////
// CUndoControl and CRedoControl drawing

void CUndoControl::GetStatusString(int i, CString& str)
{
	switch (i)
	{
		case 0:
			str.LoadString(IDS_UNDO_CANCEL);
			return;
		
		case 1:
			str.LoadString(IDS_UNDO_ONE);
			return;
		
		default:
			{
				CString strTemp;
				strTemp.LoadString(IDS_UNDO_N);
				
				sprintf(str.GetBufferSetLength(strTemp.GetLength() + 4),
				        strTemp, i);
				str.ReleaseBuffer();

				return;
			}
	}
}

void CRedoControl::GetStatusString(int i, CString& str)
{
	switch (i)
	{
		case 0:
			str.LoadString(IDS_UNDO_CANCEL);
			return;
		
		case 1:
			str.LoadString(IDS_REDO_ONE);
			return;
		
		default:
			{
				CString strTemp;
				strTemp.LoadString(IDS_REDO_N);
				
				sprintf(str.GetBufferSetLength(strTemp.GetLength() + 4),
				        strTemp, i);
				str.ReleaseBuffer();

				return;
			}
	}
}

void CUndoControl::FillUndoList(CListBox& listBox)
{
	// Get the full list of undo strings and put them in the specified
	// listbox.
	HWND hwndActive = theApp.GetActiveView();

	char szBuf [256];
	int nLevel = 0;
	
	while ((BOOL) ::SendMessage(hwndActive, WM_GETUNDOSTRING,
	                            nLevel++, (LPARAM)(LPSTR)&szBuf))
	{
		listBox.AddString(szBuf);
	}
}

void CRedoControl::FillUndoList(CListBox& listBox)
{
	// Get the full list of undo strings and put them in the specified
	// listbox.
	HWND hwndActive = theApp.GetActiveView();

	char szBuf [256];
	int nLevel = 0;
	
	while ((BOOL) ::SendMessage(hwndActive, WM_GETREDOSTRING,
	                            nLevel++, (LPARAM)(LPSTR)&szBuf))
	{
		listBox.AddString(szBuf);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CUndoList
//
// This is the dropdown window.  It contains a single listbox plus the
// "status rectangle" which has the "Undo n actions" text in it.

const char* v_pchDropWndClass = NULL;

CUndoList::CUndoList()
{
	if (v_pchDropWndClass == NULL)
		v_pchDropWndClass = AfxRegisterWndClass(CS_DBLCLKS | CS_SAVEBITS);
	
	ASSERT(v_pchDropWndClass != NULL);
}

CUndoList::~CUndoList()
{
}

void CUndoList::ShowList(BOOL bShow, CRect rc, CDropControl* pParent)
{
	if (bShow)
	{
		CString strUndoList;
		VERIFY(strUndoList.LoadString(IDS_UNDOLIST));

		DWORD dwBorder = (theApp.m_bWin4 ? 0L : WS_BORDER);
		DWORD dwEdge = WS_EX_TOPMOST |
			(theApp.m_bWin4 ? WS_EX_DLGMODALFRAME : 0L);

		CreateEx(dwEdge, v_pchDropWndClass, strUndoList, WS_POPUP | dwBorder,
			rc.left, rc.top, rc.Width(), rc.Height(), pParent->GetSafeHwnd(), NULL);

		m_listBox.Create(LBS_EXTENDEDSEL | LBS_NOTIFY | WS_VISIBLE |
			WS_CHILD | WS_VSCROLL | dwBorder,
			CRect(BORDERSIZE, BORDERSIZE, BORDERSIZE, BORDERSIZE),
			this, 0, pParent);

		if (theApp.m_bWin4)
			m_listBox.ModifyStyleEx(0L, WS_EX_CLIENTEDGE);
		else
			m_listBox.SubclassCtl3d(CTL3D_LISTBOX_CTL);

		pParent->FillUndoList(m_listBox);

		int nHeight = m_listBox.GetItemHeight(0);
		CRect rcList(0, 0, 0, nHeight * 6);
		AdjustWindowRectEx(rcList, m_listBox.GetStyle(), FALSE,
			m_listBox.GetExStyle());
		m_listBox.SetWindowPos(NULL, 0, 0,
			rc.Width() - BORDERSIZE * 2, rcList.Height(),
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

		rc.bottom = rc.top + rcList.Height() + BORDERSIZE * 3 + STATUSHEIGHT;

		AdjustWindowRectEx(rc, GetStyle(), FALSE, GetExStyle());

		CDockWorker::LockWorker(TRUE);
		SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height(),
			SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);

		UpdateWindow();
	}
	else
	{
		m_listBox.DestroyWindow();
		PostMessage(WM_DESTROYSELF);
	}
}

BEGIN_MESSAGE_MAP(CUndoList, CWnd)
	//{{AFX_MSG_MAP(CUndoList)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_REGISTERED_MESSAGE(WM_DESTROYSELF, OnDestroySelf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUndoList message handlers

void CUndoList::OnPaint()
{
	// we have to allow for the fact that this message might be delivered after the window has been deleted
	if(GetSafeHwnd()!=NULL)
	{
		CPaintDC dc(this); // device context for painting
		
		// calculate rects for window and status box
		CRect rc;
		GetClientRect(&rc);
		CRect rcStatus = CalcStatusRect(rc);
		
		dc.FillRect(&rc, GetSysBrush(COLOR_BTNFACE));
		dc.Draw3dRect(rcStatus, GetSysColor(COLOR_BTNSHADOW),
			GetSysColor(COLOR_BTNHIGHLIGHT));
		rcStatus.InflateRect(-1, -1);
		
		// draw appropriate text in the status box
		if(m_listBox.GetSafeHwnd()!=NULL)
		{
			int i = m_listBox.GetSelCount();
			ASSERT(i != LB_ERR);
			dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
			dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
			CFont* pFont = dc.SelectObject(GetStdFont(font_Normal));
			
			CString str;
			m_listBox.m_pDropControl->GetStatusString(i, str);
			dc.DrawText(str, -1, &rcStatus,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			dc.SelectObject(pFont);
		}
	}
}

// CalcStatusRect
//
// This calculates the rect used for drawing the status text.  The
// border around the status box is drawn one pixel outside this rect.
CRect CUndoList::CalcStatusRect(CRect rc)
{
	return CRect(rc.left + BORDERSIZE, rc.bottom - STATUSHEIGHT - BORDERSIZE,
				 rc.right - BORDERSIZE, rc.bottom - BORDERSIZE);
}

void CUndoList::SetSel(int index)
{
	int indexMax = m_listBox.GetCount() - 1;
	index = max(min(index, indexMax), -1);
	
	if (index == GetSel())
		return;

	if (index == 0)
		m_listBox.SetSel(0);
	else if (index > 0)
		m_listBox.SelItemRange(TRUE, 0, index);
	
	if (index < indexMax)
		m_listBox.SelItemRange(FALSE, index + 1, indexMax);
	
	// if we must scroll the listbox to keep the max selected
	// item in view, do so.
	if (index > m_listBox.GetBottomIndex())
		m_listBox.SetTopIndex(m_listBox.GetTopIndex() + index - m_listBox.GetBottomIndex());
	else if (index < m_listBox.GetTopIndex())
		m_listBox.SetTopIndex(index);

	// invalidate the status box
	CRect rc;
	GetClientRect(&rc);
	CRect rcStatus = CalcStatusRect(rc);
	rcStatus.InflateRect(-1, -1);
	InvalidateRect(&rcStatus);
}

int CUndoList::GetSel()
{
	return m_listBox.GetSelCount() - 1;
}

// CalcSel
//
// Takes a point (in screen coordinates) and returns the index of the
// indicated item.  A -1 means that no item is currently selected.
int CUndoList::CalcSel(CPoint point)
{
	CRect rc;
	m_listBox.GetWindowRect(&rc);
	
	if (! rc.PtInRect(point))
		return -1;

	// point is definitely in the listbox, compute the index
	int cPixels = point.y - rc.top;
	ASSERT(cPixels >= 0);
	ASSERT(cPixels <= rc.Height());
	
	int itemHeight = m_listBox.GetItemHeight(0);	// height in pixels
	ASSERT(itemHeight != LB_ERR);

	int index = (cPixels / itemHeight) + m_listBox.GetTopIndex();
	int count = m_listBox.GetCount();
	if (index > count - 1)
		index = count - 1;
	return index;
}

void CUndoList::ScrollDown()
{
	// Remember that "SetSel" scrolls the listbox to keep the current
	// selection in view.
	SetSel(m_listBox.GetBottomIndex() + 1);
}

void CUndoList::ScrollUp()
{
	SetSel(m_listBox.GetTopIndex() - 1);
}
// FUTURE: We already filter WM_LBUTTONDOWNS in PreTranslateMessge.  We will
//		  never get this message.
//
void CUndoList::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_listBox.m_pDropControl->CancelModes();
	return;
}

void CUndoList::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_listBox.m_pDropControl->CancelModes();
	return;
}

void CUndoList::PageDown()
{
	// find the index for the bottommost item, scroll it to
	// the top and select it.
	int index = m_listBox.GetBottomIndex();
	m_listBox.SetTopIndex(index);
	SetSel(index);
}

void CUndoList::PageUp()
{
	// find the index for the topmost item, scroll it to
	// the bottom and select the new topmost item.
	int bottomIndex = m_listBox.GetBottomIndex();
	int cRows = bottomIndex - m_listBox.GetTopIndex();
	
	int newTopIndex = max(m_listBox.GetTopIndex() - cRows, 0);
	
	m_listBox.SetTopIndex(newTopIndex);
	SetSel(newTopIndex);
}

LRESULT CUndoList::OnDestroySelf(WPARAM wParam, LPARAM lParam)
{
	DestroyWindow();
	CDockWorker::LockWorker(FALSE);

	return 0L;
}


/////////////////////////////////////////////////////////////////////////////
// CGhostListBox
//
// An almost-normal listbox which forwards all mouse events to its
// owner and won't accept the focus.

IMPLEMENT_DYNAMIC(CGhostListBox, CListBox)

CGhostListBox::CGhostListBox()
{
}

CGhostListBox::~CGhostListBox()
{
}

BOOL CGhostListBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
	UINT nID, CDropControl* pDropControl)
{
	m_pDropControl = pDropControl;
	
	if (!CListBox::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	SetFont(GetStdFont(font_Normal));
	return TRUE;
}

BEGIN_MESSAGE_MAP(CGhostListBox, CListBox)
	//{{AFX_MSG_MAP(CGhostListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGhostListBox message handlers

void CGhostListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	// convert to DropControl client coordinates
	ClientToScreen(&point);
	m_pDropControl->ScreenToClient(&point);
	m_pDropControl->OnListBoxLButtonDown(nFlags, point);
}

void CGhostListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	// convert to DropControl client coordinates
	ClientToScreen(&point);
	m_pDropControl->ScreenToClient(&point);
	m_pDropControl->OnListBoxLButtonUp(nFlags, point);
}

int CGhostListBox::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// we don't want the listbox to steal activation away from the
	// DropControl when the user clicks on it.
	return MA_NOACTIVATE;
}

int CGhostListBox::GetBottomIndex()
{
	// Just like the standart CListBox::GetTopIndex function.
	// The bottom index is the top index plus the total number
	// of rows in the listbox.
	CRect rc;
	GetWindowRect(&rc);

	int itemHeight = GetItemHeight(0);	// height in pixels
	ASSERT(itemHeight != LB_ERR);

	int index = (rc.Height() / itemHeight) + GetTopIndex() - 1;
	index = min(index, GetCount());
	return index;
}
