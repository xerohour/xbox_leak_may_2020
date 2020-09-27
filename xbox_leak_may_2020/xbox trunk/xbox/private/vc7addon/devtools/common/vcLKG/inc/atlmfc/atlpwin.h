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

// ATL 3.0 - Test: Persistent Window Support

#ifndef __ATLPWIN_H__
#define __ATLPWIN_H__

#pragma once

#define WNDPERS_SYSCURSOR	0x00000001
#define WNDPERS_SYSBKBRUSH	0x00000002	//?
#define WNDPERS_TOOLBAR		0x00000004
#define WNDPERS_STATUSBAR	0x00000008
#define WNDPERS_ACCELERATORS	0x00000010

struct _AtlPersistentWindow
{
	int m_cbSize;
// Internal flags
	DWORD m_dwFlags;
// WndClass data
	UINT m_uClsStyle;
	UINT m_uCommonResourceID;
	UINT m_nCursorID;
	UINT m_nBkgndBrush;
// Window data
	int m_x;
	int m_y;
	int m_cx;
	int m_cy;
	DWORD m_dwStyle;
	DWORD m_dwExStyle;
};


// MI into a class derived from CFrameWindowImpl
template <class T>
class CPersistentWindowImpl
{
public:
	BOOL SaveWindow()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));

		_AtlPersistentWindow pw;
		pw.m_cbSize = sizeof(pw);

/**/		// Should actually go to resources
/**/		LPCTSTR lpstrFilePath = _T("window.dat");

// open file
		HANDLE hFile = ::CreateFile(lpstrFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if(hFile == NULL)
			return FALSE;

// fill struct
		CFrameWndClassInfo& wci = pT->GetWndClassInfo();

		pw.m_uClsStyle = wci.m_wc.style;
		pw.m_uCommonResourceID = wci.m_uCommonResourceID;
/**/		pw.m_dwFlags = WNDPERS_SYSCURSOR | WNDPERS_SYSBKBRUSH;
/**/		pw.m_nCursorID = LOWORD(wci.m_lpszCursorID);	// expects int???
/**/		pw.m_nBkgndBrush = (UINT)wci.m_wc.hbrBackground - 1;

		RECT rect;
		pT->GetWindowRect(&rect);
		pw.m_x = rect.left;
		pw.m_y = rect.top;
		pw.m_cx = rect.right - rect.left;
		pw.m_cy = rect.bottom - rect.top;
		pw.m_dwStyle = pT->GetStyle();
		pw.m_dwExStyle = pT->GetExStyle();

// write to file
		DWORD dwCount = 0;
		if(!::WriteFile(hFile, &pw, sizeof(pw), &dwCount, NULL))
		{
			::CloseHandle(hFile);
			return FALSE;
		}

// close file
		return ::CloseHandle(hFile);
	}

	BOOL LoadWindow()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(!::IsWindow(pT->m_hWnd));

		_AtlPersistentWindow pw;
		::ZeroMemory(&pw, sizeof(pw));

/**/		// Should actually go to resources
/**/		LPCTSTR lpstrFilePath = _T("window.dat");

// open file
		HANDLE hFile = ::CreateFile(lpstrFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if(hFile == NULL)
			return FALSE;

// read file
		DWORD dwCount = 0;
		if(!::ReadFile(hFile, &pw, sizeof(pw), &dwCount, NULL))
		{
			::CloseHandle(hFile);
			return FALSE;
		}

// close file
		::CloseHandle(hFile);	// no error checking?

		ATLASSERT(pw.m_cbSize == sizeof(pw));
		if(pw.m_cbSize < sizeof(pw))
			return FALSE;

// create window
		WNDCLASSEX wc;

		wc.cbSize = sizeof(wc);
		wc.style = pw.m_uClsStyle;
		wc.lpfnWndProc = pT->StartWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = _pModule->GetModuleInstance();
		wc.hIcon = (HICON)::LoadImage(_pModule->GetResourceInstance(), MAKEINTRESOURCE(pw.m_uCommonResourceID), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
		HINSTANCE hInst = NULL;
		if(!(pw.m_dwFlags & WNDPERS_SYSCURSOR))
			hInst = _pModule->GetResourceInstance();
		wc.hCursor = ::LoadCursor(hInst, MAKEINTRESOURCE(pw.m_nCursorID));
/**/		wc.hbrBackground = (HBRUSH)(pw.m_nBkgndBrush + 1);
		wc.lpszMenuName = NULL;
		TCHAR szClassName[256];
		wsprintf(szClassName, "ATL_P:%8.8X", (DWORD)pT);
		wc.lpszClassName = szClassName;
		wc.hIconSm = (HICON)::LoadImage(_pModule->GetResourceInstance(), MAKEINTRESOURCE(pw.m_uCommonResourceID), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

		ATOM atom = ::RegisterClassEx(&wc);
		if(!atom)
			return FALSE;

		TCHAR szWindowName[256];
		szWindowName[0] = 0;
		::LoadString(_pModule->GetResourceInstance(), pw.m_uCommonResourceID, szWindowName, 255);

		HMENU hMenu = ::LoadMenu(_pModule->GetResourceInstance(), MAKEINTRESOURCE(pw.m_uCommonResourceID));

		RECT rcPos = { pw.m_x, pw.m_y, pw.m_x + pw.m_cx, pw.m_y + pw.m_cy };

//		HWND hWnd = ((CWindowImplBaseT< T >*)pT)->Create(NULL, rcPos, szWindowName, pw.m_dwStyle, pw.m_dwExStyle, (UINT)hMenu, atom, NULL);
		HWND hWnd = ((CWindowImplBase*)pT)->Create(NULL, rcPos, szWindowName, pw.m_dwStyle, pw.m_dwExStyle, (UINT)hMenu, atom, NULL);

		if(hWnd != NULL)
			pT->m_hAccel = ::LoadAccelerators(_pModule->GetResourceInstance(), MAKEINTRESOURCE(pw.m_uCommonResourceID));

		return (BOOL)hWnd;
	}
};

#endif //__ATLPWIN_H__
