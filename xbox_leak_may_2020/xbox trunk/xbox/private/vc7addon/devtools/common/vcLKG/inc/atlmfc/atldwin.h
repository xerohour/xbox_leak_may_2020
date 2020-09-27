// This is a part of the Active Template Library.
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLDWIN_H__
#define __ATLDWIN_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <vcctrls.h>

namespace ATL
{

template <class T>
class CDockSiteImpl
{
public:
	DWORD m_dwDockingFlags;

// Constructors
	CDockSiteImpl(DWORD dwDockingFlags = DWS_DOCKANY)
	{
		m_dwDockingFlags = dwDockingFlags;
	}

// Operations
	BOOL InitDockingSite(DWORD dwFlags = DWS_DOCKANY)
	{
		T *pT = static_cast<T *>(this);

		ATLASSERT(::IsWindow(pT->m_hWnd));
		return DockWin_InitDockingSite(pT->m_hWnd, dwFlags);
	}

	LONG SetViewWindow(HWND hWndView)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		ATLASSERT(hWndView==NULL || ::IsWindow(hWndView));

		return pT->SendMessage(DWM_SETVIEWWINDOW, (WPARAM)hWndView);
	}

	BOOL SetOffsets(RECT* pRect)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return (BOOL) pT->SendMessage(DWM_SETOFFSETS, 0, (LPARAM)pRect);
	}

	void UpdateDockingLayout()
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->SendMessage(DWM_UPDATELAYOUT, 0, 0);
	}

	HWND CreateDockingFrame(HWND hWndDockWindow, DOCKINFO* pDockInfo)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return (HWND)pT->SendMessage(DWM_REGISTERDOCKWINDOW, (WPARAM)hWndDockWindow, (LPARAM)pDockInfo);
	}

	HWND CreateDockingFrame(HWND hwndDockWindow, LPTSTR szTitle, RECT* rcUndocked, SIZE* sizeDockHorz, SIZE* sizeDockVert, int nDockSide = DW_DOCK_TOP, int nRow = -1, int nColumn = -1, DWORD dwDockFlags = DWS_DOCKANY)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		DOCKINFO di;
		memset(&di, 0, sizeof(DOCKINFO));
		di.cbSize = sizeof(DOCKINFO);
		di.nDockFlags = dwDockFlags;
		di.lpstrTitle = szTitle;
		di.rectFloat = *rcUndocked;
		di.dockLoc.nDockSide = nDockSide;
		di.dockLoc.nDockCol = nColumn;
		di.dockLoc.nDockRow = nRow;
		di.sizeDockHorizontal = *sizeDockHorz;
		di.sizeDockVertical = *sizeDockVert;
		return CreateDockingFrame(hwndDockWindow, &di);
	}

	LONG GetDockWindowInfo(HWND hWndDockWindow, DOCKINFO* pDockInfo)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return pT->SendMessage(DWM_GETDOCKWINDOWINFO, (WPARAM)hWndDockWindow, (LPARAM)pDockInfo);
	}

	LONG SetDockWindowInfo(HWND hWndDockWindow, DOCKINFO* pDockInfo)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return pT->SendMessage(DWM_SETDOCKWINDOWINFO, (WPARAM)hWndDockWindow, (LPARAM)pDockInfo);
	}

	LONG DockWindow(HWND hWndDockWindow)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return pT->SendMessage(DWM_DOCKWINDOW, (WPARAM)hWndDockWindow);
	}

	LONG FloatWindow(HWND hWndDockWindow)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return pT->SendMessage(DWM_FLOATWINDOW, (WPARAM)hWndDockWindow);
	}

	LONG QueryDockStatus(HWND hWndDockWindow)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return pT->SendMessage(DWM_QUERYDOCKSTATUS, (WPARAM)hWndDockWindow);
	}

	void ShowDockedChildren(BOOL bShow)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		return pT->SendMessage(DWM_SHOWDOCKEDCHILDREN, (WPARAM) bShow);
	}

	void SetSplitterWidth(long nSplitter, long nNewWidth)
	{
		T *pT = static_cast<T *>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->SendMessage(DWM_SETSPLITTERWIDTH, nSplitter, nNewWidth);
	}

	// Message Map
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (!InitDockingSite(m_dwDockingFlags))
			return -1;
		return 0;
	}

	int OnCreate(LPCREATESTRUCT lpCS)
	{
		if (!InitDockingSite(m_dwDockingFlags))
			return -1;
		return 0;
	}

	BEGIN_MSG_MAP(CDockSiteImpl<T>)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()
};

template <class T, class TBase = CWindow, class TWinTraits = CFrameWinTraits>
class CDockSiteWindowImpl : public CWindowImpl< T, TBase, TWinTraits >,
						public CDockSiteImpl<CDockSiteWindowImpl < T, TBase, TWinTraits> >
{
public:
	typedef CDockSiteWindowImpl< T, TBase, TWinTraits >		thisClass;
	typedef CDockSiteImpl<CDockSiteWindowImpl < T, TBase, TWinTraits > > baseClass;

	BEGIN_MSG_MAP(thisClass)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

};

class CDockSiteWindow : public CDockSiteWindowImpl<CDockSiteWindow>
{
public:
};

}; //namespace ATL

#endif // __ATLDWIN_H__
