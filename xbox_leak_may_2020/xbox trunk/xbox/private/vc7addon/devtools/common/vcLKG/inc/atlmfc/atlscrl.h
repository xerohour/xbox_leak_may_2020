//NONSHIP

// This is a part of the Active Template Library.
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLSCRL_H__
#define __ATLSCRL_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLWIN_H__
	#error atlscrl.h requires atlwin.h to be included first
#endif

namespace ATL
{

template <class T>
class CScrollWindowImpl
{
public:
	POINT m_ptOffset;
	SIZE m_sizeAll;
	SIZE m_sizeLine;
	SIZE m_sizePage;
	SIZE m_sizeClient;

// Constructor
	CScrollWindowImpl()
	{
		m_ptOffset.x = 0;
		m_ptOffset.y = 0;
		m_sizeAll.cx = 0;
		m_sizeAll.cy = 0;
		m_sizePage.cx = 0;
		m_sizePage.cy = 0;
		m_sizeLine.cx = 0;
		m_sizeLine.cy = 0;
		m_sizeClient.cx = 0;
		m_sizeClient.cy = 0;
	}

// Attributes & Operations
	// offset operations
	void ScrollSetOffset(int x, int y, BOOL bRedraw = TRUE)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));

		m_ptOffset.x = x;
		m_ptOffset.y = y;

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;

		si.nPos = m_ptOffset.x;
		pT->SetScrollInfo(SB_HORZ, &si, bRedraw);

		si.nPos = m_ptOffset.y;
		pT->SetScrollInfo(SB_VERT, &si, bRedraw);

		if(bRedraw)
			pT->Invalidate();
	}
	void ScrollSetOffset(POINT ptOffset, BOOL bRedraw = TRUE)
	{
		ScrollSetOffset(ptOffset.x, ptOffset.y, bRedraw);
	}
	void ScrollGetOffset(POINT& ptOffset)
	{
		ptOffset = m_ptOffset;
	}

	// size operations
	void ScrollSetSize(int cx, int cy, BOOL bRedraw = TRUE)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));

		m_sizeAll.cx = cx;
		m_sizeAll.cy = cy;

		m_ptOffset.x = 0;
		m_ptOffset.y = 0;

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;

		si.nMax = m_sizeAll.cx - 1;
		si.nPage = m_sizeClient.cx;
		si.nPos = m_ptOffset.x;
		pT->SetScrollInfo(SB_HORZ, &si, bRedraw);

		si.nMax = m_sizeAll.cy - 1;
		si.nPage = m_sizeClient.cy;
		si.nPos = m_ptOffset.y;
		pT->SetScrollInfo(SB_VERT, &si, bRedraw);

		ScrollSetLine(0, 0);
		ScrollSetPage(0, 0);

		if(bRedraw)
			pT->Invalidate();
	}
	void ScrollSetSize(SIZE size, BOOL bRedraw = TRUE)
	{
		ScrollSetSize(size.cx, size.cy, bRedraw);
	}
	void ScrollGetSize(SIZE& sizeWnd)
	{
		sizeWnd = m_sizeAll;
	}

	// line operations
	void ScrollSetLine(int cxLine, int cyLine)
	{
		ATLASSERT(cxLine >= 0 && cyLine >= 0);
		ATLASSERT(m_sizeAll.cx != 0 && m_sizeAll.cy != 0);

		m_sizeLine.cx = CalcLineOrPage(cxLine, m_sizeAll.cx, 100);
		m_sizeLine.cy = CalcLineOrPage(cyLine, m_sizeAll.cy, 100);
	}
	void ScrollSetLine(SIZE sizeLine)
	{
		ScrollSetLine(sizeLine.cx, sizeLine.cy);
	}
	void ScrollGetLine(SIZE& sizeLine)
	{
		sizeLine = m_sizeLine;
	}

	// page operations
	void ScrollSetPage(int cxPage, int cyPage)
	{
		ATLASSERT(cxPage >= 0 && cyPage >= 0);
		ATLASSERT(m_sizeAll.cx != 0 && m_sizeAll.cy != 0);

		m_sizePage.cx = CalcLineOrPage(cxPage, m_sizeAll.cx, 10);
		m_sizePage.cy = CalcLineOrPage(cyPage, m_sizeAll.cy, 10);
	}
	void ScrollSetPage(SIZE sizePage)
	{
		ScrollSetPage(sizePage.cx, sizePage.cy);
	}
	void ScrollGetPage(SIZE& sizePage)
	{
		sizePage = m_sizePage;
	}

	// commands
	void ScrollLineDown()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_VERT, SB_LINEDOWN, (int&)m_ptOffset.y, m_sizeAll.cy, m_sizePage.cy, m_sizeLine.cy);
	}

	void ScrollLineUp()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_VERT, SB_LINEUP, (int&)m_ptOffset.y, m_sizeAll.cy, m_sizePage.cy, m_sizeLine.cy);
	}

	void ScrollPageDown()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_VERT, SB_PAGEDOWN, (int&)m_ptOffset.y, m_sizeAll.cy, m_sizePage.cy, m_sizeLine.cy);
	}

	void ScrollPageUp()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_VERT, SB_PAGEUP, (int&)m_ptOffset.y, m_sizeAll.cy, m_sizePage.cy, m_sizeLine.cy);
	}

	void ScrollTop()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_VERT, SB_TOP, (int&)m_ptOffset.y, m_sizeAll.cy, m_sizePage.cy, m_sizeLine.cy);
	}

	void ScrollBottom()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_VERT, SB_BOTTOM, (int&)m_ptOffset.y, m_sizeAll.cy, m_sizePage.cy, m_sizeLine.cy);
	}

	void ScrollLineRight()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_HORZ, SB_LINEDOWN, (int&)m_ptOffset.x, m_sizeAll.cx, m_sizePage.cx, m_sizeLine.cx);
	}

	void ScrollLineLeft()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_HORZ, SB_LINEUP, (int&)m_ptOffset.x, m_sizeAll.cx, m_sizePage.cx, m_sizeLine.cx);
	}

	void ScrollPageRight()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_HORZ, SB_PAGEDOWN, (int&)m_ptOffset.x, m_sizeAll.cx, m_sizePage.cx, m_sizeLine.cx);
	}

	void ScrollPageLeft()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_HORZ, SB_PAGEUP, (int&)m_ptOffset.x, m_sizeAll.cx, m_sizePage.cx, m_sizeLine.cx);
	}

	void ScrollAllLeft()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_HORZ, SB_TOP, (int&)m_ptOffset.x, m_sizeAll.cx, m_sizePage.cx, m_sizeLine.cx);
	}

	void ScrollAllRight()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_HORZ, SB_BOTTOM, (int&)m_ptOffset.x, m_sizeAll.cx, m_sizePage.cx, m_sizeLine.cx);
	}

	BEGIN_MSG_MAP(CScrollWindowImpl< T >)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnVScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_VERT, LOWORD(wParam), (int&)m_ptOffset.y, m_sizeAll.cy, m_sizePage.cy, m_sizeLine.cy);
		return 0;
	}

	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_HORZ, LOWORD(wParam), (int&)m_ptOffset.x, m_sizeAll.cx, m_sizePage.cx, m_sizeLine.cx);
		return 0;
	}

	LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
//REVIEW - temp hack
		short zDelta = HIWORD(wParam);
		int nScrollCode = (zDelta > 0) ? SB_LINEUP : SB_LINEDOWN;
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoScroll(SB_VERT, nScrollCode, (int&)m_ptOffset.y, m_sizeAll.cy, m_sizePage.cy, m_sizeLine.cy);
		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));

		m_sizeClient.cx = GET_X_LPARAM(lParam);
		m_sizeClient.cy = GET_Y_LPARAM(lParam);

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_PAGE | SIF_POS;

		si.nPage = m_sizeClient.cx;
		si.nPos = m_ptOffset.x;
		pT->SetScrollInfo(SB_HORZ, &si, FALSE);

		si.nPage = m_sizeClient.cy;
		si.nPos = m_ptOffset.y;
		pT->SetScrollInfo(SB_VERT, &si, FALSE);

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		CWinPaintDC dc(pT->m_hWnd);
		dc.SetViewportOrg(-m_ptOffset.x, -m_ptOffset.y);
		pT->DoPaint(dc);
		return 0;
	}

// Overrideables
	void DoPaint(HDC /*hDC*/)
	{
		// must be implemented in a derived class
		ATLASSERT(FALSE);
	}

// Implementation
	void DoScroll(int nType, int nScrollCode, int& nOffset, int nSizeAll, int nSizePage, int nSizeLine)
	{
		T* pT = static_cast<T*>(this);
		BOOL bUpdate = TRUE;
		int nScroll = 0;

		switch(nScrollCode)
		{
		case SB_TOP:
			nScroll = -nOffset;
			nOffset = 0;
			break;
		case SB_BOTTOM:
			nScroll = nSizeAll - 1 - nOffset;
			nOffset = nSizeAll - 1;
			break;
		case SB_LINEDOWN:
			if(nOffset < nSizeAll - 1 - nSizeLine)
			{
				nScroll = -nSizeLine;
				nOffset += nSizeLine;
			}
			else
			{
				nScroll = nOffset - (nSizeAll - 1);
				nOffset = nSizeAll - 1;
			}
			break;
		case SB_LINEUP:
			if(nOffset >= nSizeLine)
			{
				nScroll = nSizeLine;
				nOffset -= nSizeLine;
			}
			else
			{
				nScroll = nOffset;
				nOffset = 0;
			}
			break;
		case SB_PAGEDOWN:
			if(nOffset < nSizeAll - 1 - nSizePage)
			{
				nScroll = -nSizePage;
				nOffset += nSizePage;
			}
			else
			{
				nScroll = nOffset - (nSizeAll - 1);
				nOffset = nSizeAll - 1;
			}
			break;
		case SB_PAGEUP:
			if(nOffset >= nSizePage)
			{
				nScroll = nSizePage;
				nOffset -= nSizePage;
			}
			else
			{
				nScroll = nOffset;
				nOffset = 0;
			}
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			{
				SCROLLINFO si;
				si.cbSize = sizeof(SCROLLINFO);
				si.fMask = SIF_TRACKPOS;
				if(pT->GetScrollInfo(nType, &si))
				{
					nScroll = nOffset - si.nTrackPos;
					nOffset = si.nTrackPos;
				}
			}
			break;
		case SB_ENDSCROLL:
			bUpdate = FALSE;
			break;
		default:
			bUpdate = FALSE;
			break;
		}

		if(bUpdate && nScroll != 0)
		{
			pT->SetScrollPos(nType, nOffset, TRUE);
			int cxScroll = 0;
			int cyScroll = 0;
			if(nType == SB_VERT)
				cyScroll = nScroll;
			else
				cxScroll = nScroll;
			pT->ScrollWindowEx(cxScroll, cyScroll, SW_INVALIDATE | SW_ERASE);
/**/			ATLTRACE2(atlTraceWindowing, 0, "%%%Update: nOffset = %i, nScroll = %i\n", nOffset, nScroll);
		}
	}
	int CalcLineOrPage(int nVal, int nMax, int nDiv)
	{
		if(nVal == 0)
		{
			nVal = nMax / nDiv;
			if(nVal < 1)
				nVal = 1;
		}
		else if(nVal > nMax)
			nVal = nMax;

		return nVal;
	}
};


template <class T>
class CMapScrollWindowImpl : public CScrollWindowImpl< T >
{
public:
	int m_nMapMode;
	RECT m_rectLogAll;
	SIZE m_sizeLogLine;
	SIZE m_sizeLogPage;

// Constructor
	CMapScrollWindowImpl() : m_nMapMode(MM_TEXT)
	{
		m_rectLogAll.left = 0;
		m_rectLogAll.top = 0;
		m_rectLogAll.right = 0;
		m_rectLogAll.bottom = 0;
		m_sizeLogPage.cx = 0;
		m_sizeLogPage.cy = 0;
		m_sizeLogLine.cx = 0;
		m_sizeLogLine.cy = 0;
	}

// Attributes & Operations
	// mapping mode operations
	void ScrollSetMapMode(int nMapMode)
	{
		ATLASSERT(nMapMode >= MM_MIN && nMapMode <= MM_MAX_FIXEDSCALE);
		m_nMapMode = nMapMode;
	}
	int ScrollGetMapMode()
	{
		ATLASSERT(m_nMapMode >= MM_MIN && m_nMapMode <= MM_MAX_FIXEDSCALE);
		return m_nMapMode;
	}

	// offset operations
	void ScrollSetOffset(int x, int y, BOOL bRedraw = TRUE)
	{
		ATLASSERT(m_nMapMode >= MM_MIN && m_nMapMode <= MM_MAX_FIXEDSCALE);
		POINT ptOff = { x, y };
		// block: convert logical to device units
		{
			CWinWindowDC dc(NULL);
			dc.SetMapMode(m_nMapMode);
			dc.LPtoDP(&ptOff);
		}
		CScrollWindowImpl< T >::ScrollSetOffset(ptOff, bRedraw);
	}
	void ScrollSetOffset(POINT ptOffset, BOOL bRedraw = TRUE)
	{
		ScrollSetOffset(ptOffset.x, ptOffset.y, bRedraw);
	}
	void ScrollGetOffset(POINT& ptOffset)
	{
		ATLASSERT(m_nMapMode >= MM_MIN && m_nMapMode <= MM_MAX_FIXEDSCALE);
		ptOffset = m_ptOffset;
		// block: convert logical to device units
		{
			CWinWindowDC dc(NULL);
			dc.SetMapMode(m_nMapMode);
			dc.DPtoLP(&ptOffset);
		}
	}

	// size operations
	void ScrollSetSize(int xMin, int yMin, int xMax, int yMax, BOOL bRedraw = TRUE)
	{
/**/		ATLASSERT(xMax > xMin && yMax > yMin);
		ATLASSERT(m_nMapMode >= MM_MIN && m_nMapMode <= MM_MAX_FIXEDSCALE);

		m_rectLogAll.left = xMin;
		m_rectLogAll.top = yMax;
		m_rectLogAll.right = xMax;
		m_rectLogAll.bottom = yMin;

		SIZE sizeAll;
		sizeAll.cx = xMax - xMin + 1;
		sizeAll.cy = yMax - xMin + 1;
		// block: convert logical to device units
		{
			CWinWindowDC dc(NULL);
			dc.SetMapMode(m_nMapMode);
			dc.LPtoDP(&sizeAll);
		}
		CScrollWindowImpl< T >::ScrollSetSize(sizeAll, bRedraw);
		ScrollSetLine(0, 0);
		ScrollSetPage(0, 0);
	}
	void ScrollSetSize(RECT& rcScroll, BOOL bRedraw = TRUE)
	{
		ScrollSetSize(rcScroll.left, rcScroll.top, rcScroll.right, rcScroll.bottom, bRedraw);
	}
	void ScrollSetSize(int cx, int cy, BOOL bRedraw = TRUE)
	{
		ScrollSetSize(0, 0, cx, cy, bRedraw);
	}
	void ScrollSetSize(SIZE size, BOOL bRedraw = NULL)
	{
		ScrollSetSize(0, 0, size.cx, size.cy, bRedraw);
	}
	void ScrollGetSize(RECT& rcScroll)
	{
		ATLASSERT(m_nMapMode >= MM_MIN && m_nMapMode <= MM_MAX_FIXEDSCALE);
		rcScroll = m_rectLogAll;
	}

	// line operations
	void ScrollSetLine(int cxLine, int cyLine)
	{
		ATLASSERT(cxLine >= 0 && cyLine >= 0);
		ATLASSERT(m_nMapMode >= MM_MIN && m_nMapMode <= MM_MAX_FIXEDSCALE);

		m_sizeLogLine.cx = cxLine;
		m_sizeLogLine.cy = cyLine;
		SIZE sizeLine = m_sizeLogLine;
		// block: convert logical to device units
		{
			CWinWindowDC dc(NULL);
			dc.SetMapMode(m_nMapMode);
			dc.LPtoDP(&sizeLine);
		}
		CScrollWindowImpl< T >::ScrollSetLine(sizeLine);
	}
	void ScrollSetLine(SIZE sizeLine)
	{
		ScrollSetLine(sizeLine.cx, sizeLine.cy);
	}
	void ScrollGetLine(SIZE& sizeLine)
	{
		ATLASSERT(m_nMapMode >= MM_MIN && m_nMapMode <= MM_MAX_FIXEDSCALE);
		sizeLine = m_sizeLogLine;
	}

	// page operations
	void ScrollSetPage(int cxPage, int cyPage)
	{
		ATLASSERT(cxPage >= 0 && cyPage >= 0);
		ATLASSERT(m_nMapMode >= MM_MIN && m_nMapMode <= MM_MAX_FIXEDSCALE);

		m_sizeLogPage.cx = cxPage;
		m_sizeLogPage.cy = cyPage;
		SIZE sizePage = m_sizeLogPage;
		// block: convert logical to device units
		{
			CWinWindowDC dc(NULL);
			dc.SetMapMode(m_nMapMode);
			dc.LPtoDP(&sizePage);
		}
		CScrollWindowImpl< T >::ScrollSetPage(sizePage);
	}
	void ScrollSetPage(SIZE sizePage)
	{
		ScrollSetPage(sizePage.cx, sizePage.cy);
	}
	void ScrollGetPage(SIZE& sizePage)
	{
		ATLASSERT(m_nMapMode >= MM_MIN && m_nMapMode <= MM_MAX_FIXEDSCALE);
		sizePage = m_sizeLogPage;
	}

	BEGIN_MSG_MAP(CMapScrollWindowImpl< T >)
		MESSAGE_HANDLER(WM_VSCROLL, CScrollWindowImpl< T >::OnVScroll)
		MESSAGE_HANDLER(WM_HSCROLL, CScrollWindowImpl< T >::OnHScroll)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, CScrollWindowImpl< T >::OnMouseWheel)
		MESSAGE_HANDLER(WM_SIZE, CScrollWindowImpl< T >::OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		CWinPaintDC dc(pT->m_hWnd);
		dc.SetMapMode(m_nMapMode);
		if(m_nMapMode == MM_TEXT)
			dc.SetViewportOrg(-m_ptOffset.x, -m_ptOffset.y);
		else
			dc.SetViewportOrg(-m_ptOffset.x, -m_ptOffset.y + m_sizeAll.cy);
		dc.SetWindowOrg(m_rectLogAll.left, m_rectLogAll.bottom);
/**/		ATLTRACE2(atlTraceWindowing, 0, "viewport org = (%i, %i)\n", -m_ptOffset.x, -m_ptOffset.y + m_sizeAll.cy);
		pT->DoPaint(dc);
		return 0;
	}
};

}; //namespace ATL

#endif //__ATLSCRL_H__
