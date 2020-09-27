// findtool.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "findtool.h"
#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"
#include "filtrdlg.h"
#include "findwdlg.h"
#include "srchdlgs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL IsPointInWindow(HWND hwnd, CPoint point)
{
	RECT rc;

	GetWindowRect(hwnd, &rc);

	return(PtInRect(&rc, point));

	if (point.x >= rc.left && point.x <= rc.right &&
		point.y >= rc.top && point.y <= rc.bottom)
	{
		return TRUE;
	}
	else
		return FALSE;
}

HWND TopChildWindowFromPoint(HWND hwnd, CPoint point)
{
	if (hwnd == NULL)
		return(NULL);

	HWND hwndFirstChild = ::GetWindow(hwnd, GW_CHILD);
	if (hwndFirstChild == NULL)
		return(NULL);

	HWND hwndLastChild = ::GetWindow(hwndFirstChild, GW_HWNDLAST);
	if (IsPointInWindow(hwndLastChild, point))
	{
		return(hwndLastChild);
	}
	else
	{
		HWND hwndPrev = hwndLastChild;
		while ((hwndPrev = ::GetWindow(hwndPrev, GW_HWNDPREV)) != NULL)
		{
			if (IsPointInWindow(hwndPrev, point))
				return(hwndPrev);
		}
	}
	return(NULL);
}

HWND TopSiblingWindowFromPoint(HWND hwnd, CPoint point)
{
	if (hwnd == NULL)
		return(NULL);

	HWND hwndLastSibling = ::GetWindow(hwnd, GW_HWNDLAST);
	if (IsPointInWindow(hwndLastSibling, point))
	{
		return(hwndLastSibling);
	}
	else
	{
		HWND hwndPrev = hwndLastSibling;
		while ((hwndPrev = ::GetWindow(hwndPrev, GW_HWNDPREV)) != NULL)
		{
			if (IsPointInWindow(hwndPrev, point))
				return(hwndPrev);
		}
	}
	// shouldn't ever get here...
	ASSERT(FALSE);
	return(NULL);
}

BOOL IsWindowRectInWindowRect(HWND hwndContainee, HWND hwndContainer)
{
	RECT rcContainee;
	RECT rcContainer;

	GetWindowRect(hwndContainee, &rcContainee);
	GetWindowRect(hwndContainer, &rcContainer);

	if (rcContainee.top >= rcContainer.top && rcContainee.bottom <= rcContainer.bottom &&
		rcContainee.left >= rcContainer.left && rcContainee.right <= rcContainer.right)
	{
		return TRUE;
	}
	else
		return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CFindToolIcon

void CFindToolIcon::Initialize(CFiltersWindowDlgTab* pFiltersWindowDlgTab)
{
	m_pFiltersWindowDlgTab = pFiltersWindowDlgTab;
	m_pFindWindowDlg = NULL;
	m_pSearchWindowDlg = NULL;
	m_pSearchMessageDlg = NULL;
	m_hwndFlash = NULL;
	m_fHilite = FALSE;
	m_hwndSubjectLast = (HWND)-1;
}

void CFindToolIcon::Initialize(CFindWindowDlg* pFindWindowDlg)
{
	m_pFindWindowDlg = pFindWindowDlg;
	m_pFiltersWindowDlgTab = NULL;
	m_pSearchWindowDlg = NULL;
	m_pSearchMessageDlg = NULL;
	m_hwndFlash = NULL;
	m_fHilite = FALSE;
	m_hwndSubjectLast = (HWND)-1;
}

void CFindToolIcon::Initialize(CSearchWindowDlg* pSearchWindowDlg)
{
	m_pSearchWindowDlg = pSearchWindowDlg;
	m_pFiltersWindowDlgTab = NULL;
	m_pFindWindowDlg = NULL;
	m_pSearchMessageDlg = NULL;
	m_hwndFlash = NULL;
	m_fHilite = FALSE;
	m_hwndSubjectLast = (HWND)-1;
}

void CFindToolIcon::Initialize(CSearchMessageDlg* pSearchMessageDlg)
{
	m_pSearchMessageDlg = pSearchMessageDlg;
	m_pSearchWindowDlg = NULL;
	m_pFiltersWindowDlgTab = NULL;
	m_pFindWindowDlg = NULL;
	m_hwndFlash = NULL;
	m_fHilite = FALSE;
	m_hwndSubjectLast = (HWND)-1;
}

BEGIN_MESSAGE_MAP(CFindToolIcon, CWnd)
	//{{AFX_MSG_MAP(CFindToolIcon)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFindToolIcon message handlers

void CFindToolIcon::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	::SetCursor(theApp.m_hcurFindTool);
	SendMessage(STM_SETICON, (WPARAM)theApp.m_hiconFindTool2);
	UpdateWindow();
	if (m_pFiltersWindowDlgTab)
		m_pFiltersWindowDlgTab->BeginFindWindowTracking();
	else if (m_pFindWindowDlg)
		m_pFindWindowDlg->BeginFindWindowTracking();
	else if (m_pSearchMessageDlg)
		m_pSearchMessageDlg->BeginFindWindowTracking();
	else
		m_pSearchWindowDlg->BeginFindWindowTracking();

	CWnd::OnLButtonDown(nFlags, point);
}

void CFindToolIcon::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetCapture())
	{
		HWND  hwndSubject;
		DWORD pidSpyxx;
		DWORD pidHwndFound;

		ClientToScreen(&point);

		hwndSubject = ::WindowFromPoint(point);

		//
		// Don't find windows of the Spy++ app itself.
		//
		if (hwndSubject)
		{
			GetWindowThreadProcessId(theApp.m_pMainWnd->m_hWnd, &pidSpyxx);
			GetWindowThreadProcessId(hwndSubject, &pidHwndFound);
			if (pidHwndFound == pidSpyxx)
			{
				hwndSubject = NULL;
			}
		}

		HWND hwndChild = TopChildWindowFromPoint(hwndSubject, point);
		while (hwndChild != NULL && hwndChild != hwndSubject)
		{
			hwndSubject = TopSiblingWindowFromPoint(hwndChild, point);
			hwndChild = TopChildWindowFromPoint(hwndSubject, point);
		}

		DWORD dwStyles = GetWindowLong(hwndSubject, GWL_STYLE);
		if (dwStyles & WS_CHILD)
			hwndSubject = TopSiblingWindowFromPoint(hwndSubject, point);

		//
		// Only update if something changed.
		//
		if (hwndSubject != m_hwndSubjectLast)
		{
			StopFlash();
			if (m_pFiltersWindowDlgTab)
				m_pFiltersWindowDlgTab->UpdateFindWindowTracking(hwndSubject);
			else if (m_pFindWindowDlg)
				m_pFindWindowDlg->UpdateFindWindowTracking(hwndSubject);
			else if (m_pSearchMessageDlg)
				m_pSearchMessageDlg->UpdateFindWindowTracking(hwndSubject);
			else
				m_pSearchWindowDlg->UpdateFindWindowTracking(hwndSubject);
			m_hwndSubjectLast = hwndSubject;
			StartFlash(hwndSubject);
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CFindToolIcon::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	StopFlash();
	SendMessage(STM_SETICON, (WPARAM)theApp.m_hiconFindTool);
	if (m_pFiltersWindowDlgTab)
		m_pFiltersWindowDlgTab->EndFindWindowTracking();
	else if (m_pFindWindowDlg)
		m_pFindWindowDlg->EndFindWindowTracking();
	else if (m_pSearchMessageDlg)
		m_pSearchMessageDlg->EndFindWindowTracking();
	else
		m_pSearchWindowDlg->EndFindWindowTracking();

	CWnd::OnLButtonUp(nFlags, point);
}

UINT CFindToolIcon::OnNcHitTest(CPoint)
{
	return HTCLIENT;
}

void CFindToolIcon::DrawWindowFrame(HWND hwnd)
{
	HDC hdc;
	RECT rc;
	HPEN hpen;
	HPEN hpenOld;
	HBRUSH hbrOld;
	int cxBorder = ::GetSystemMetrics(SM_CXBORDER);
	int cyBorder = ::GetSystemMetrics(SM_CYBORDER);
	int cxFrame  = ::GetSystemMetrics(SM_CXFRAME);
	int cyFrame  = ::GetSystemMetrics(SM_CYFRAME);
	int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);

	::GetWindowRect(hwnd, &rc);
	hdc = ::GetWindowDC(hwnd);
	::SetROP2(hdc, R2_NOT);
	hpen = ::CreatePen(PS_INSIDEFRAME, 3 * cxBorder, RGB(0, 0, 0));

	hpenOld = (HPEN)::SelectObject(hdc, hpen);
	hbrOld = (HBRUSH)::SelectObject(hdc, ::GetStockObject(NULL_BRUSH));
	if (::IsZoomed(hwnd))
		::Rectangle(hdc, cxFrame, cyFrame, cxScreen + cxFrame, cyScreen + cyFrame);
	else
		::Rectangle(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top);
	::SelectObject(hdc, hbrOld);
	::SelectObject(hdc, hpenOld);

	::ReleaseDC(hwnd, hdc);
	::DeleteObject(hpen);
}

void CFindToolIcon::StartFlash(HWND hwnd)
{
	if (hwnd)
	{
		DrawWindowFrame(hwnd);
		m_hwndFlash = hwnd;
		m_fHilite = TRUE;
	}
}

void CFindToolIcon::StopFlash()
{
	if (m_fHilite)
		DrawWindowFrame(m_hwndFlash);

	m_fHilite = FALSE;
	m_hwndFlash = NULL;
}

/////////////////////////////////////////////////////////////////////////////
