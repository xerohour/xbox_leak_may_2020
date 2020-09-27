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

#ifndef __ATLSPLIT_H__
#define __ATLSPLIT_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLWIN_H__
	#error atlsplit.h requires atlwin.h to be included first
#endif

namespace ATL
{

/////////////////////////////////////////////////////////////////////////////
// Forward declarations

template <class T, BOOL t_bVertical = TRUE> class CSplitterImpl;
template <class T, BOOL t_bVertical = TRUE, class TBase = CWindow, class TWinTraits = CControlWinTraits> class CSplitterWindowImpl;
template <BOOL t_bVertical = TRUE> class CSplitterWindowT;

/////////////////////////////////////////////////////////////////////////////
// Splitter pane constants

#define SPLIT_PANE_LEFT			 0
#define SPLIT_PANE_RIGHT		 1
#define SPLIT_PANE_TOP			 SPLIT_PANE_LEFT
#define SPLIT_PANE_BOTTOM		 SPLIT_PANE_RIGHT
#define SPLIT_PANE_NONE			-1

/////////////////////////////////////////////////////////////////////////////
// CSplitterImpl - Provides splitter support to any window

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

template <class T, BOOL t_bVertical = TRUE>
class CSplitterImpl
{
public:
	enum { m_nPanesCount = 2 };

	HWND m_hWndPane[m_nPanesCount];
	RECT m_rcSplitter;
	int m_xySplitterPos;
	int m_nDefActivePane;
	static int m_cxySplitBar;	// splitter bar width/height
	static HCURSOR m_hCursor;
	int m_cxyMin;			// minimum pane size
	int m_cxyBarEdge;		// splitter bar edge
	BOOL m_bFullDrag;
	int m_cxyDragOffset;

// Constructor
	CSplitterImpl() : m_xySplitterPos(-1), m_nDefActivePane(SPLIT_PANE_NONE), m_cxyMin(0), m_cxyBarEdge(0), m_bFullDrag(TRUE), m_cxyDragOffset(0)
	{
		m_hWndPane[SPLIT_PANE_LEFT] = NULL;
		m_hWndPane[SPLIT_PANE_RIGHT] = NULL;

		m_rcSplitter.left = 0;
		m_rcSplitter.top = 0;
		m_rcSplitter.right = 0;
		m_rcSplitter.bottom = 0;

		if(m_cxySplitBar == 0 || m_hCursor == NULL)
		{
			_pModule->EnterStaticDataCriticalSection();
			if(m_cxySplitBar == 0)
				m_cxySplitBar = ::GetSystemMetrics(t_bVertical ? SM_CXSIZEFRAME : SM_CYSIZEFRAME);
			if(m_hCursor == NULL)
				m_hCursor = ::LoadCursor(NULL, t_bVertical ? IDC_SIZEWE : IDC_SIZENS);
			_pModule->LeaveStaticDataCriticalSection();
		}

		if(!::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &m_bFullDrag, 0))
			m_bFullDrag = TRUE;	// default to full-drag mode
	}

// Attributes
	void SetSplitterRect(LPRECT lpRect = NULL, BOOL bUpdate = TRUE)
	{
		if(lpRect == NULL)
		{
			T* pT = static_cast<T*>(this);
			pT->GetClientRect(&m_rcSplitter);
		}
		else
		{
			m_rcSplitter = *lpRect;
		}

		if(bUpdate)
			UpdateSplitterLayout();
	}

	void GetSplitterRect(LPRECT lpRect)
	{
		ATLASSERT(lpRect != NULL);
		*lpRect = m_rcSplitter;
	}

	BOOL SetSplitterPos(int xyPos = -1, BOOL bUpdate = TRUE)
	{
		if(xyPos == -1)		// -1 == middle
		{
			if(t_bVertical)
				xyPos = (m_rcSplitter.right - m_rcSplitter.left) / 2;
			else
				xyPos = (m_rcSplitter.bottom - m_rcSplitter.top) / 2;
		}

		// Adjust if out of valid range
		int cxyMax = 0;
		if(t_bVertical)
			cxyMax = m_rcSplitter.right - m_rcSplitter.left;
		else
			cxyMax = m_rcSplitter.bottom - m_rcSplitter.top;

		if(xyPos < m_cxyMin + m_cxyBarEdge)
			xyPos = m_cxyMin;
		else if(xyPos > (cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin))
			xyPos = cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin;

		// Set new position and update if requested
		BOOL bRet = (m_xySplitterPos != xyPos);
		m_xySplitterPos = xyPos;

		if(bUpdate && bRet)
			UpdateSplitterLayout();

		return bRet;
	}

	int GetSplitterPos()
	{
		return m_xySplitterPos;
	}

// Operations
	void SetSplitterPanes(HWND hWndLeftTop, HWND hWndRightBottom, BOOL bUpdate = TRUE)
	{
		m_hWndPane[SPLIT_PANE_LEFT] = hWndLeftTop;
		m_hWndPane[SPLIT_PANE_RIGHT] = hWndRightBottom;
		ATLASSERT(m_hWndPane[SPLIT_PANE_LEFT] == NULL || m_hWndPane[SPLIT_PANE_RIGHT] == NULL || m_hWndPane[SPLIT_PANE_LEFT] != m_hWndPane[SPLIT_PANE_RIGHT]);
		if(bUpdate)
			UpdateSplitterLayout();
	}

	BOOL SetSplitterPane(int nPane, HWND hWnd, BOOL bUpdate = TRUE)
	{
		ATLASSERT(nPane == SPLIT_PANE_LEFT || nPane == SPLIT_PANE_RIGHT);

		if(nPane != SPLIT_PANE_LEFT && nPane != SPLIT_PANE_RIGHT)
			return FALSE;
		m_hWndPane[nPane] = hWnd;
		ATLASSERT(m_hWndPane[SPLIT_PANE_LEFT] == NULL || m_hWndPane[SPLIT_PANE_RIGHT] == NULL || m_hWndPane[SPLIT_PANE_LEFT] != m_hWndPane[SPLIT_PANE_RIGHT]);
		if(bUpdate)
			UpdateSplitterLayout();
		return TRUE;
	}

	HWND GetSplitterPane(int nPane)
	{
		ATLASSERT(nPane == SPLIT_PANE_LEFT || nPane == SPLIT_PANE_RIGHT);

		if(nPane != SPLIT_PANE_LEFT && nPane != SPLIT_PANE_RIGHT)
			return FALSE;
		return m_hWndPane[nPane];
	}

	BOOL SetActivePane(int nPane)
	{
		ATLASSERT(nPane == SPLIT_PANE_LEFT || nPane == SPLIT_PANE_RIGHT);

		if(nPane != SPLIT_PANE_LEFT && nPane != SPLIT_PANE_RIGHT)
			return FALSE;
		::SetFocus(m_hWndPane[nPane]);
		m_nDefActivePane = nPane;
		return TRUE;
	}

	int GetActivePane()
	{
		int nRet = SPLIT_PANE_NONE;
		HWND hWndFocus = ::GetFocus();
		if(hWndFocus != NULL)
		{
			for(int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if(hWndFocus == m_hWndPane[nPane])
				{
					nRet = nPane;
					break;
				}
			}
		}
		return nRet;
	}

	BOOL ActivateNextPane(BOOL bNext = TRUE)
	{
		int nPane = SPLIT_PANE_NONE;
		switch(GetActivePane())
		{
		case SPLIT_PANE_LEFT:
			nPane = SPLIT_PANE_RIGHT;
			break;
		case SPLIT_PANE_RIGHT:
			nPane = SPLIT_PANE_LEFT;
			break;
		default:
			nPane = bNext ? SPLIT_PANE_LEFT : SPLIT_PANE_RIGHT;
			break;
		}
		return SetActivePane(nPane);
	}

	BOOL SetDefaultActivePane(int nPane)
	{
		ATLASSERT(nPane == SPLIT_PANE_LEFT || nPane == SPLIT_PANE_RIGHT);

		if(nPane != SPLIT_PANE_LEFT && nPane != SPLIT_PANE_RIGHT)
			return FALSE;
		m_nDefActivePane = nPane;
		return TRUE;
	}

	BOOL SetDefaultActivePane(HWND hWnd)
	{
		for(int nPane = 0; nPane < m_nPanesCount; nPane++)
		{
			if(hWnd == m_hWndPane[nPane])
			{
				m_nDefActivePane = nPane;
				return TRUE;
			}
		}
		return FALSE;	// not found
	}

	int GetDefaultActivePane()
	{
		return m_nDefActivePane;
	}

	void DrawSplitter(HDC hDC)
	{
		ATLASSERT(hDC != NULL);
		if(m_xySplitterPos == -1)
			return;

		T* pT = static_cast<T*>(this);
		pT->DrawSplitterBar(hDC);

		for(int nPane = 0; nPane < m_nPanesCount; nPane++)
		{
			if(m_hWndPane[nPane] == NULL)
				pT->DrawSplitterPane(hDC, nPane);
		}
	}

// Overrideables
	void DrawSplitterBar(HDC hDC)
	{
		RECT rect;
		if(GetSplitterBarRect(&rect))
		{
			::FillRect(hDC, &rect, (HBRUSH)(COLOR_3DFACE + 1));
			if(m_cxyMin == 0)	// draw 3D edge if needed
				DrawSplitterBar3DEdge(hDC, rect);
		}
	}

	// called only if pane is empty
	void DrawSplitterPane(HDC hDC, int nPane)
	{
		RECT rect;
		if(GetSplitterPaneRect(nPane, &rect))
			::FillRect(hDC, &rect, (HBRUSH)(COLOR_APPWORKSPACE + 1));
	}

// Message map and handlers
	typedef CSplitterImpl< T, t_bVertical>	thisClass;
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		if((pT->GetExStyle() & WS_EX_CLIENTEDGE))
			m_cxyBarEdge = 2 * ::GetSystemMetrics(t_bVertical ? SM_CXEDGE : SM_CYEDGE);
		else
			m_cxyMin = 2 * ::GetSystemMetrics(t_bVertical ? SM_CXEDGE : SM_CYEDGE);
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		// try setting position if not set
		if(m_xySplitterPos == -1)
			SetSplitterPos();
		// do painting
		PAINTSTRUCT ps;
		HDC hDC = ::BeginPaint(pT->m_hWnd, &ps);
		DrawSplitter(hDC);
		::EndPaint(pT->m_hWnd, &ps);
		return 0;
	}

	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		if((HWND)wParam == pT->m_hWnd && LOWORD(lParam) == HTCLIENT)
		{
			DWORD dwPos = ::GetMessagePos();
			POINT ptPos = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
			pT->ScreenToClient(&ptPos);
			if(IsOverSplitterBar(ptPos.x, ptPos.y))
				return 1;
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if((wParam & MK_LBUTTON) && ::GetCapture() == pT->m_hWnd)
		{
			int xyNewSplitPos = 0;
			if(t_bVertical)
				xyNewSplitPos = xPos - m_rcSplitter.left - m_cxyDragOffset;
			else
				xyNewSplitPos = yPos - m_rcSplitter.top - m_cxyDragOffset;

			if(xyNewSplitPos == -1)	// avoid -1, that means middle
				xyNewSplitPos = -2;

			if(m_xySplitterPos != xyNewSplitPos)
			{
				if(m_bFullDrag)
				{
					if(pT->SetSplitterPos(xyNewSplitPos, TRUE))
						pT->UpdateWindow();
				}
				else
				{
					DrawGhostBar();
					pT->SetSplitterPos(xyNewSplitPos, FALSE);
					DrawGhostBar();
				}
			}
		}
		else		// not dragging, just set cursor
		{
			if(IsOverSplitterBar(xPos, yPos))
				::SetCursor(m_hCursor);
			bHandled = FALSE;
		}

		return 0;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if(IsOverSplitterBar(xPos, yPos))
		{
			T* pT = static_cast<T*>(this);
			pT->SetCapture();
			::SetCursor(m_hCursor);
			if(!m_bFullDrag)
				DrawGhostBar();
			if(t_bVertical)
				m_cxyDragOffset = xPos - m_rcSplitter.left - m_xySplitterPos;
			else
				m_cxyDragOffset = yPos - m_rcSplitter.top - m_xySplitterPos;
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(!m_bFullDrag)
		{
			DrawGhostBar();
			T* pT = static_cast<T*>(this);
			UpdateSplitterLayout();
			pT->UpdateWindow();
		}
		::ReleaseCapture();
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM, BOOL& bHandled)
	{
		if(m_nDefActivePane == SPLIT_PANE_LEFT || m_nDefActivePane == SPLIT_PANE_RIGHT)
			::SetFocus(m_hWndPane[m_nDefActivePane]);
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		LRESULT lRet = pT->DefWindowProc(uMsg, wParam, lParam);
		if(lRet == MA_ACTIVATE || lRet == MA_ACTIVATEANDEAT)
		{
			DWORD dwPos = ::GetMessagePos();
			POINT pt = { LOWORD(dwPos), HIWORD(dwPos) };
			pT->ScreenToClient(&pt);
			RECT rcPane;
			for(int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if(GetSplitterPaneRect(nPane, &rcPane) && ::PtInRect(&rcPane, pt))
				{
					m_nDefActivePane = nPane;
					break;
				}
			}
		}
		return lRet;
	}

// Internal helpers
	void UpdateSplitterLayout()
	{
		if(m_xySplitterPos == -1)
			return;

		T* pT = static_cast<T*>(this);
		RECT rect = { 0, 0, 0, 0 };
		if(GetSplitterBarRect(&rect))
			pT->InvalidateRect(&rect);

		for(int nPane = 0; nPane < m_nPanesCount; nPane++)
		{
			if(GetSplitterPaneRect(nPane, &rect))
			{
				if(m_hWndPane[nPane] != NULL)
					::SetWindowPos(m_hWndPane[nPane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
				else
					pT->InvalidateRect(&rect);
			}
		}
	}

	BOOL GetSplitterBarRect(LPRECT lpRect)
	{
		ATLASSERT(lpRect != NULL);
		if(m_xySplitterPos == -1)
			return FALSE;

		if(t_bVertical)
		{
			lpRect->left = m_rcSplitter.left + m_xySplitterPos;
			lpRect->top = m_rcSplitter.top;
			lpRect->right = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
			lpRect->bottom = m_rcSplitter.bottom;
		}
		else
		{
			lpRect->left = m_rcSplitter.left;
			lpRect->top = m_rcSplitter.top + m_xySplitterPos;
			lpRect->right = m_rcSplitter.right;
			lpRect->bottom = m_rcSplitter.top + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
		}

		return TRUE;
	}

	BOOL GetSplitterPaneRect(int nPane, LPRECT lpRect)
	{
		ATLASSERT(nPane == SPLIT_PANE_LEFT || nPane == SPLIT_PANE_RIGHT);
		ATLASSERT(lpRect != NULL);
		BOOL bRet = TRUE;
		if(nPane == SPLIT_PANE_LEFT)
		{
			if(t_bVertical)
			{
				lpRect->left = m_rcSplitter.left;
				lpRect->top = m_rcSplitter.top;
				lpRect->right = m_rcSplitter.left + m_xySplitterPos;
				lpRect->bottom = m_rcSplitter.bottom;
			}
			else
			{
				lpRect->left = m_rcSplitter.left;
				lpRect->top = m_rcSplitter.top;
				lpRect->right = m_rcSplitter.right;
				lpRect->bottom = m_rcSplitter.top + m_xySplitterPos;
			}
		}	
		else if(nPane == SPLIT_PANE_RIGHT)
		{
			if(t_bVertical)
			{
				lpRect->left = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
				lpRect->top = m_rcSplitter.top;
				lpRect->right = m_rcSplitter.right;
				lpRect->bottom = m_rcSplitter.bottom;
			}
			else
			{
				lpRect->left = m_rcSplitter.left;
				lpRect->top = m_rcSplitter.top + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
				lpRect->right = m_rcSplitter.right;
				lpRect->bottom = m_rcSplitter.bottom;
			}
		}
		else
		{
			bRet = FALSE;
		}
		return bRet;
	}

	BOOL IsOverSplitterRect(int x, int y)
	{
		// -1 == don't check
		return ((x == -1 || (x >= m_rcSplitter.left && x <= m_rcSplitter.right)) &&
			(y == -1 || (y >= m_rcSplitter.top && y <= m_rcSplitter.bottom)));
	}

	BOOL IsOverSplitterBar(int x, int y)
	{
		if(m_xySplitterPos == -1 || !IsOverSplitterRect(x, y))
			return FALSE;
		int xy = (t_bVertical) ? x : y;
		int xyOff = (t_bVertical) ? m_rcSplitter.left : m_rcSplitter.top;
		return ((xy >= (xyOff + m_xySplitterPos)) && (xy < xyOff + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge));
	}

	void DrawSplitterBar3DEdge(HDC hDC, RECT& rect)
	{
#if (_ATL_VER >= 0x0400) || (_ATLWIN_PREVIEW >= 3)
		CWinManagedPen penWhite;
		penWhite.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHILIGHT));
		CWinManagedPen penGray;
		penGray.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
		CWinManagedPen penBlack;
		penBlack.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW));

		CWinDC dc = hDC;
		CWinPen penOld = dc.GetCurrentPen();
#else
		CPen penWhite;
		penWhite.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHILIGHT));
		CPen penGray;
		penGray.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
		CPen penBlack;
		penBlack.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW));

		CDC dc = hDC;
		HPEN penOld = dc.GetCurrentPen();
#endif //(_ATL_VER >= 0x0400) || (_ATLWIN_PREVIEW >= 3)

		if(t_bVertical)
		{
			dc.SelectPen(penWhite);
			dc.MoveTo(rect.left + 1, rect.top);
			dc.LineTo(rect.left + 1, rect.bottom);
			dc.SelectPen(penGray);
			dc.MoveTo(rect.right - 2, rect.top);
			dc.LineTo(rect.right - 2, rect.bottom);
			dc.SelectPen(penBlack);
			dc.MoveTo(rect.right - 1, rect.top);
			dc.LineTo(rect.right - 1, rect.bottom);
		}
		else
		{
			dc.SelectPen(penWhite);
			dc.MoveTo(rect.left, rect.top + 1);
			dc.LineTo(rect.right, rect.top + 1);
			dc.SelectPen(penGray);
			dc.MoveTo(rect.left, rect.bottom - 2);
			dc.LineTo(rect.right, rect.bottom - 2);
			dc.SelectPen(penBlack);
			dc.MoveTo(rect.left, rect.bottom - 1);
			dc.LineTo(rect.right, rect.bottom - 1);
		}

		dc.SelectPen(penOld);
	}

	void DrawGhostBar()
	{
		RECT rect;
		if(GetSplitterBarRect(&rect))
		{
			// invert the brush pattern (looks just like frame window sizing)
			T* pT = static_cast<T*>(this);
#if (_ATL_VER >= 0x0400) || (_ATLWIN_PREVIEW >= 3)
			CWinWindowDC dc(pT->m_hWnd);
			CWinManagedBrush brush = CWinDC::GetHalftoneBrush();
			CWinBrush brushOld;
#else
			CWindowDC dc(pT->m_hWnd);
			CBrush brush = CDC::GetHalftoneBrush();
			HBRUSH brushOld;
#endif //(_ATL_VER >= 0x0400) || (_ATLWIN_PREVIEW >= 3)
			if(brush.m_hBrush != NULL)
			{
				brushOld = dc.SelectBrush(brush);
				dc.PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
				dc.SelectBrush(brushOld);
			}
		}
	}
};

template <class T, BOOL t_bVertical> int CSplitterImpl< T, t_bVertical>::m_cxySplitBar = 0;
template <class T, BOOL t_bVertical> HCURSOR CSplitterImpl< T, t_bVertical>::m_hCursor = NULL;

/////////////////////////////////////////////////////////////////////////////
// CSplitterWindowImpl - Implements a splitter window

template <class T, BOOL t_bVertical = TRUE, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class CSplitterWindowImpl : public CWindowImpl< T, TBase, TWinTraits >, public CSplitterImpl<CSplitterWindowImpl< T , t_bVertical, TBase, TWinTraits >, t_bVertical>
{
public:
	typedef CSplitterWindowImpl< T , t_bVertical, TBase, TWinTraits >			thisClass;
	typedef CSplitterImpl<CSplitterWindowImpl< T , t_bVertical, TBase, TWinTraits >, t_bVertical>	baseClass;
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(baseClass)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// handled, no background painting needed
		return 1;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(wParam != SIZE_MINIMIZED)
			SetSplitterRect();

		bHandled = FALSE;
		return 1;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CSplitterWindow - Implements a splitter window to be used as is

template <BOOL t_bVertical = TRUE>
class CSplitterWindowT : public CSplitterWindowImpl<CSplitterWindowT<t_bVertical>, t_bVertical>
{
public:
	DECLARE_WND_CLASS_EX(_T("AtlSplitterWindow"), 0, COLOR_WINDOW)
};

typedef CSplitterWindowT<TRUE>	CSplitterWindow;
typedef CSplitterWindowT<FALSE>	CHorSplitterWindow;

}; //namespace ATL

#endif // __ATLSPLIT_H__
