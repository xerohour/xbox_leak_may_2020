//NONSHIP

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXCBAR_H__
#define __AFXCBAR_H__

#pragma once

#ifndef _AFX
	#error afxcbar.h requires MFC to be included
#endif

#include <vcctrls.h>

// We need to have AFX_IDW_CMDBAR in afxres.h (do we?)
// for now, tmp define here:
#define AFX_IDW_CMDBAR		0xE8FF


class CCommandBar : public CControlBar
{
public:
	DECLARE_DYNAMIC_INLINE(CCommandBar, CControlBar)

	//{{AFX_MSG(CCommandBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP_INLINE(CControlBar)

public:
// Constructors
	CCommandBar()
	{
	}

// Attributes
	CMenu* GetMenu() const
	{
		ASSERT(::IsWindow(m_hWnd));
		return CMenu::FromHandle((HMENU)::SendMessage(m_hWnd, CBRM_GETMENU, 0, 0L));
	}

	COLORREF GetImageMaskColor() const
	{
		ASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, CBRM_GETIMAGEMASKCOLOR, 0, 0L);
	}
	COLORREF SetImageMaskColor(COLORREF clrMask) const
	{
		ASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, CBRM_SETIMAGEMASKCOLOR, 0, (LPARAM)clrMask);
	}

	BOOL GetImagesVisible() const
	{
		ASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_GETIMAGESVISIBLE, 0, 0L);
	}
	BOOL SetImagesVisible(BOOL bVisible)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_SETIMAGESVISIBLE, bVisible, 0L);
	}

	CSize GetImageSize() const
	{
		ASSERT(::IsWindow(m_hWnd));
		DWORD dwRet = (DWORD)::SendMessage(m_hWnd, CBRM_GETIMAGESIZE, 0, 0L);
		SIZE size = { LOWORD(dwRet), HIWORD(dwRet) };
		return size;
	}
	BOOL SetImageSize(SIZE size)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_SETIMAGESIZE, 0, MAKELPARAM(size.cx, size.cy));
	}

// Methods
	BOOL Create(CWnd* pParentWnd, LPCTSTR lpszWindowName = NULL, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, DWORD dwExStyle = 0, UINT nID = AFX_IDW_CMDBAR, LPVOID lpCreateParam = NULL)
	{
		ASSERT(m_hWnd == NULL);
		ASSERT(::IsWindow(pParentWnd->GetSafeHwnd()));
		m_dwStyle = (dwStyle & CBRS_ALL);
		RECT rect = { 0, 0, 100, 100 };
		return CWnd::CreateEx(dwExStyle, COMMANDBARCLASSNAME, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpCreateParam);
	}

	BOOL AttachToWindow(HWND hWnd)
	{
		ASSERT(m_hWnd == NULL);
		ASSERT(::IsWindow(hWnd));
		BOOL bRet = ::CommandBar_AttachToWindow(hWnd);
		if(bRet)
			m_hWnd = hWnd;
		return bRet;
	}

	BOOL AttachToWindow(CWnd* pWnd)
	{
		return AttachToWindow(pWnd->GetSafeHwnd());
	}

	BOOL LoadMenu(LPCTSTR lpstrMenuName)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_LOADMENU, (WPARAM)AfxGetResourceHandle(), (LPARAM)lpstrMenuName);
	}
	BOOL LoadMenu(UINT nResourceID)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_LOADMENU, (WPARAM)AfxGetResourceHandle(), (LPARAM)MAKEINTRESOURCE(nResourceID));
	}

	CMenu* AttachMenu(CMenu* pMenu)
	{
		ASSERT(::IsWindow(m_hWnd));
		ASSERT(::IsMenu(pMenu->GetSafeHmenu()));
		return CMenu::FromHandle((HMENU)::SendMessage(m_hWnd, CBRM_ATTACHMENU, 0, (LPARAM)pMenu->GetSafeHmenu()));
	}

	BOOL LoadImages(LPCTSTR lpstrImageName)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_LOADIMAGES, (WPARAM)AfxGetResourceHandle(), (LPARAM)lpstrImageName);
	}
	BOOL LoadImages(UINT nResourceID)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_LOADIMAGES, (WPARAM)AfxGetResourceHandle(), (LPARAM)MAKEINTRESOURCE(nResourceID));
	}

	BOOL AddBitmap(LPCTSTR lpstrBitmapName, int nCommandID)
	{
		ASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_RESOURCE | CBRAB_BITMAP;
		cbrai.hInstance = AfxGetResourceHandle();
		cbrai.lpstrResource = lpstrBitmapName;
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_ADDIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL AddBitmap(UINT nBitmapID, int nCommandID)
	{
		ASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_RESOURCE | CBRAB_BITMAP;
		cbrai.hInstance = AfxGetResourceHandle();
		cbrai.lpstrResource = MAKEINTRESOURCE(nBitmapID);
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_ADDIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL AddBitmap(CBitmap* pBitmap, int nCommandID)
	{
		ASSERT(::IsWindow(m_hWnd));
		ASSERT(pBitmap->GetSafeHandle() != NULL);
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_HANDLE | CBRAB_BITMAP;
		cbrai.hBitmap = (HBITMAP)pBitmap->GetSafeHandle();
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_ADDIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL AddIcon(HICON hIcon, int nCommandID)
	{
		ASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_HANDLE | CBRAB_ICON;
		cbrai.hIcon = hIcon;
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_ADDIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL ReplaceBitmap(LPCTSTR lpstrBitmapName, int nCommandID)
	{
		ASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_RESOURCE | CBRAB_BITMAP;
		cbrai.hInstance = AfxGetResourceHandle();
		cbrai.lpstrResource = lpstrBitmapName;
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_REPLACEIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL ReplaceBitmap(UINT nBitmapID, int nCommandID)
	{
		ASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_RESOURCE | CBRAB_BITMAP;
		cbrai.hInstance = AfxGetResourceHandle();
		cbrai.lpstrResource = MAKEINTRESOURCE(nBitmapID);
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_REPLACEIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL ReplaceBitmap(CBitmap* pBitmap, int nCommandID)
	{
		ASSERT(::IsWindow(m_hWnd));
		ASSERT(pBitmap->GetSafeHandle() != NULL);
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_HANDLE | CBRAB_BITMAP;
		cbrai.hBitmap = (HBITMAP)pBitmap->GetSafeHandle();
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_REPLACEIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL ReplaceIcon(HICON hIcon, int nCommandID)
	{
		ASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_HANDLE | CBRAB_ICON;
		cbrai.hIcon = hIcon;
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_REPLACEIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL RemoveImage(int nCommandID)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_REMOVEIMAGE, nCommandID, 0L);
	}

	BOOL RemoveAllImages()
	{
		ASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_REMOVEALLIMAGES, 0, 0L);
	}

	BOOL TrackPopupMenu(CMenu* pMenu, UINT uFlags, int x, int y, LPTPMPARAMS lpParams = NULL)
	{
		ASSERT(::IsWindow(m_hWnd));
		ASSERT(::IsMenu(pMenu->GetSafeHmenu()));
		CBRPOPUPMENU CBRPopupMenu;
		CBRPopupMenu.cbSize = sizeof(CBRPOPUPMENU);
		CBRPopupMenu.hMenu = pMenu->GetSafeHmenu();
		CBRPopupMenu.uFlags = uFlags;
		CBRPopupMenu.x = x;
		CBRPopupMenu.y = y;
		CBRPopupMenu.lptpm = lpParams;
		return (BOOL)::SendMessage(m_hWnd, CBRM_TRACKPOPUPMENU, 0, (LPARAM)&CBRPopupMenu);
	}

	BOOL SetMDIClient(HWND hWndMDIClient)
	{
		ASSERT(::IsWindow(m_hWnd));
		ASSERT(::IsWindow(hWndMDIClient));
		return (BOOL)::SendMessage(m_hWnd, CBRM_SETMDICLIENT, 0, (LPARAM)hWndMDIClient);
	}

// Implementation
	virtual ~CCommandBar()
	{
	}

	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHandler*/)
	{
		// nothing to do
	}

//	afx_msg void OnNcPaint()
//	{
//		EraseNonClient();
//	}

	afx_msg void OnPaint()
	{
		Default();	// do the default painting, bypass CControlBar::OnPaint
	}

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz)
	{
		SIZE size = { 0, 0 };
		if(m_hWnd != NULL)
			::SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)&size);
/**/		size.cy += 4;
		return size;
	}

//test only
	CSize CalcDynamicLayout(int, DWORD nMode)
	{
		return CalcFixedLayout(nMode & LM_STRETCH, nMode & LM_HORZ);
	}

	virtual BOOL IsDockBar() const
	{
// should do something else?
		return FALSE;
	}
};

IMPLEMENT_DYNAMIC_INLINE(CCommandBar, CControlBar)

BEGIN_MESSAGE_MAP_INLINE(CCommandBar, CControlBar)
	//{{AFX_MSG_MAP(CCommandBar)
	//}}AFX_MSG_MAP
//	ON_WM_NCPAINT()
	ON_WM_PAINT()
END_MESSAGE_MAP()

#endif // __AFXCBAR_H__
