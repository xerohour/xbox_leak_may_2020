//	DATATIP.CPP

#include "stdafx.h"
#pragma hdrstop

int CDataTip::m_x;
int CDataTip::m_y;
CDataTip *CDataTip::m_pTip = NULL;

/////////////////////////////////////////////////////////////////////////////
// CDataTip - implements tooltip window for CControlBar

BEGIN_MESSAGE_MAP(CDataTip, CWnd)
	//{{AFX_MSG_MAP(CDataTip)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_DISABLEMODAL, OnDisableModal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDataTip::CDataTip()
{
	ASSERT(m_pTip == NULL);
	m_pTip = this;
}

void CDataTip::OnTimer(UINT nTimerID)
{
	CPoint pt;
	GetCursorPos(&pt);

	if (abs(m_x-pt.x) > 2 || abs(m_y-pt.y) > 2)
		DestroyTip();
	else
		SetTimer(1, 100, NULL);
}

void CDataTip::OnPaint()
{
	// get text of window
	TCHAR szText[256];
	UINT nLen = GetWindowText(szText, sizeof(szText)/sizeof(TCHAR));

	// get client rectangle
	CRect rect;
	GetClientRect(rect);

	// setup DC for painting and paint the text
	CPaintDC dc(this);

	CFont * pFontOld = dc.SelectObject(GetStdFont(font_Normal));

	// determine correct colors for background & foreground
	COLORREF crInfoBack, crInfoFore;
	crInfoBack = GetSysColor(COLOR_INFOBK);
	crInfoFore = GetSysColor(COLOR_INFOTEXT);

	HBRUSH hBrush = ::CreateSolidBrush(crInfoBack);
	if (hBrush != NULL)
		::FillRect(dc.m_hDC, &rect, hBrush);
	dc.SetBkMode(TRANSPARENT);
	TCHAR chSpace = ' ';
	CSize size = dc.GetTextExtent(&chSpace, 1);
	dc.SetTextColor(crInfoFore);
	dc.TextOut(size.cx, 1, szText, nLen);

	dc.SelectObject(pFontOld);

	if (hBrush != NULL)
		::DeleteObject(hBrush);
}

LRESULT CDataTip::OnSetText(WPARAM, LPARAM lParam)
{
	Default();
	LPTSTR lpsz = (LPTSTR)lParam;
	ASSERT(lpsz != NULL);

	// size window to fit text
	CClientDC dc(NULL);

	CFont * pOldFont = dc.SelectObject(GetStdFont(font_Normal));;
	CRect rect(CPoint(0, 0), dc.GetTextExtent(lpsz, lstrlen(lpsz)));
	CalcWindowRect(&rect);
	TCHAR chSpace = ' ';
	CSize size = dc.GetTextExtent(&chSpace, 1);
	dc.SelectObject(pOldFont);

	// add a little extra space to left, right, top, and bottom
	rect.InflateRect(size.cx, 1);
	SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(),
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

	SetTimer(1, 100, NULL);

	return TRUE;
}

void CDataTip::DestroyTip()
{
	ASSERT(this != NULL);
	ASSERT(this == CDataTip::m_pTip);

	DestroyWindow();
	delete this;
	CDataTip::m_pTip = NULL;
}

LRESULT CDataTip::OnDisableModal(WPARAM, LPARAM)
{
	DestroyTip();

	// Don't add this window to the "disable list" when a modal dialog
	// 	box comes up, because it has just been destroyed!

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
