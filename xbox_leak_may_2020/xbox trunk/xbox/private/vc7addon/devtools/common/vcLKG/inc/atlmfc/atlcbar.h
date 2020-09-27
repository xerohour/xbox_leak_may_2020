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

#ifndef __ATLCBAR_H__
#define __ATLCBAR_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
	#error atlcbar.h requires atlbase.h to be included first
#endif

#include <vcctrls.h>


namespace ATL
{

/////////////////////////////////////////////////////////////////////////////
// CWinCommandBar - ATL wrapper class for the Command Bar

template <class TBase>
class CWinCommandBarT : public TBase
{
public:
// Constructors
	CWinCommandBarT(HWND hWnd = NULL) : TBase(hWnd) { }

	CWinCommandBarT< TBase >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return COMMANDBARCLASSNAME;
	}

	CWinMenu GetMenu() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinMenu((HMENU)::SendMessage(m_hWnd, CBRM_GETMENU, 0, 0L));
	}

	COLORREF GetImageMaskColor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, CBRM_GETIMAGEMASKCOLOR, 0, 0L);
	}
	COLORREF SetImageMaskColor(COLORREF clrMask) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, CBRM_SETIMAGEMASKCOLOR, 0, (LPARAM)clrMask);
	}

	BOOL GetImagesVisible() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_GETIMAGESVISIBLE, 0, 0L);
	}
	BOOL SetImagesVisible(BOOL bVisible)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_SETIMAGESVISIBLE, bVisible, 0L);
	}

	void GetImageSize(SIZE& size) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		DWORD dwRet = (DWORD)::SendMessage(m_hWnd, CBRM_GETIMAGESIZE, 0, 0L);
		size.cx = LOWORD(dwRet);
		size.cy = HIWORD(dwRet);
	}
	BOOL SetImageSize(SIZE& size)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_SETIMAGESIZE, 0, MAKELPARAM(size.cx, size.cy));
	}

// Methods
	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = ATL_IDW_COMMAND_BAR, LPVOID lpCreateParam = NULL)
	{
		ATLASSERT(m_hWnd == NULL);
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

	HWND Create(HWND hWndParent, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, int nID = ATL_IDW_COMMAND_BAR)
	{
		ATLASSERT(m_hWnd == NULL);
		m_hWnd = ::CommandBar_Create(hWndParent, dwStyle, nID);
		return m_hWnd;
	}

	BOOL AttachToWindow(HWND hWnd)
	{
		ATLASSERT(m_hWnd == NULL);
		ATLASSERT(::IsWindow(hWnd));
		BOOL bRet = ::CommandBar_AttachToWindow(hWnd);
		if(bRet)
			m_hWnd = hWnd;
		return bRet;
	}

	BOOL LoadMenu(LPCTSTR lpstrMenuName)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_LOADMENU, (WPARAM)_pModule->GetResourceInstance(), (LPARAM)lpstrMenuName);
	}
	BOOL LoadMenu(UINT nResourceID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_LOADMENU, (WPARAM)_pModule->GetResourceInstance(), (LPARAM)MAKEINTRESOURCE(nResourceID));
	}

	CWinMenu AttachMenu(HMENU hMenu)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(::IsMenu(hMenu));
		return CWinMenu((HMENU)::SendMessage(m_hWnd, CBRM_ATTACHMENU, 0, (LPARAM)hMenu));
	}

	BOOL LoadImages(LPCTSTR lpstrImageName)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_LOADIMAGES, (WPARAM)_pModule->GetResourceInstance(), (LPARAM)lpstrImageName);
	}
	BOOL LoadImages(UINT nResourceID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_LOADIMAGES, (WPARAM)_pModule->GetResourceInstance(), (LPARAM)MAKEINTRESOURCE(nResourceID));
	}

	BOOL AddBitmap(LPCTSTR lpstrBitmapName, int nCommandID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_RESOURCE | CBRAB_BITMAP;
		cbrai.hInstance = _pModule->GetResourceInstance();
		cbrai.lpstrResource = lpstrBitmapName;
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_ADDIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL AddBitmap(UINT nBitmapID, int nCommandID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_RESOURCE | CBRAB_BITMAP;
		cbrai.hInstance = _pModule->GetResourceInstance();
		cbrai.lpstrResource = MAKEINTRESOURCE(nBitmapID);
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_ADDIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL AddBitmap(HBITMAP hBitmap, int nCommandID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_HANDLE | CBRAB_BITMAP;
		cbrai.hBitmap = hBitmap;
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_ADDIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL AddIcon(HICON hIcon, int nCommandID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
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
		ATLASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_RESOURCE | CBRAB_BITMAP;
		cbrai.hInstance = _pModule->GetResourceInstance();
		cbrai.lpstrResource = lpstrBitmapName;
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_REPLACEIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL ReplaceBitmap(UINT nBitmapID, int nCommandID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_RESOURCE | CBRAB_BITMAP;
		cbrai.hInstance = _pModule->GetResourceInstance();
		cbrai.lpstrResource = MAKEINTRESOURCE(nBitmapID);
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_REPLACEIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL ReplaceBitmap(HBITMAP hBitmap, int nCommandID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		CBRADDIMAGE cbrai;
		cbrai.cbSize = sizeof(CBRADDIMAGE);
		cbrai.fMask = CBRAB_HANDLE | CBRAB_BITMAP;
		cbrai.hBitmap = hBitmap;
		cbrai.nCmdCount = 1;
		cbrai.arrCommands = &nCommandID;
		return (BOOL)::SendMessage(m_hWnd, CBRM_REPLACEIMAGE, 0, (LPARAM)&cbrai);
	}

	BOOL ReplaceIcon(HICON hIcon, int nCommandID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
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
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_REMOVEIMAGE, nCommandID, 0L);
	}

	BOOL RemoveAllImages()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBRM_REMOVEALLIMAGES, 0, 0L);
	}

	BOOL TrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, LPTPMPARAMS lpParams = NULL)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(::IsMenu(hMenu));
		CBRPOPUPMENU CBRPopupMenu;
		CBRPopupMenu.cbSize = sizeof(CBRPOPUPMENU);
		CBRPopupMenu.hMenu = hMenu;
		CBRPopupMenu.uFlags = uFlags;
		CBRPopupMenu.x = x;
		CBRPopupMenu.y = y;
		CBRPopupMenu.lptpm = lpParams;
		return (BOOL)::SendMessage(m_hWnd, CBRM_TRACKPOPUPMENU, 0, (LPARAM)&CBRPopupMenu);
	}

	BOOL SetMDIClient(HWND hWndMDIClient)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(::IsWindow(hWndMDIClient));
		return (BOOL)::SendMessage(m_hWnd, CBRM_SETMDICLIENT, 0, (LPARAM)hWndMDIClient);
	}
};

typedef CWinCommandBarT<CWindow>	CWinCommandBar;

}; //namespace ATL

#endif // __ATLCBAR_H__
