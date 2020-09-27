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

#ifndef __ATLLDRES_H__
#define __ATLLDRES_H__

#pragma once

namespace ATL
{

inline HACCEL AtlLoadAccelerators(LPCTSTR lpTableName)
{
	return ::LoadAccelerators(_pModule->GetResourceInstance(), lpTableName);
}

inline HACCEL AtlLoadAccelerators(UINT uID)
{
	return ::LoadAccelerators(_pModule->GetResourceInstance(), MAKEINTRESOURCE(uID));
}

inline CWinMenu AtlLoadMenu(LPCTSTR lpMenuName)
{
	return CWinMenu(::LoadMenu(_pModule->GetResourceInstance(), lpMenuName));
}

inline CWinMenu AtlLoadMenu(UINT uID)
{
	return CWinMenu(::LoadMenu(_pModule->GetResourceInstance(), MAKEINTRESOURCE(uID)));
}

inline CWinBitmap AtlLoadBitmap(LPCTSTR lpBitmapName)
{
	return CWinBitmap(::LoadBitmap(_pModule->GetResourceInstance(), lpBitmapName));
}

inline CWinBitmap AtlLoadBitmap(UINT uID)
{
	return CWinBitmap(::LoadBitmap(_pModule->GetResourceInstance(), MAKEINTRESOURCE(uID)));
}

#ifdef OEMRESOURCE
inline CWinBitmap AtlLoadSysBitmap(LPCTSTR lpBitmapName)
{
#ifdef _DEBUG
	WORD wID = (WORD)lpBitmapName;
	ATLASSERT(wID >= 32734 && wID <= 32767);
#endif //_DEBUG
	return CWinBitmap(::LoadBitmap(NULL, lpBitmapName));
}
#endif //OEMRESOURCE

inline HCURSOR AtlLoadCursor(LPCTSTR lpCursorName)
{
	return ::LoadCursor(_pModule->GetResourceInstance(), lpCursorName);
}

inline HCURSOR AtlLoadCursor(UINT uID)
{
	return ::LoadCursor(_pModule->GetResourceInstance(), MAKEINTRESOURCE(uID));
}

inline HCURSOR AtlLoadSysCursor(LPCTSTR lpCursorName)
{
	ATLASSERT(lpCursorName == IDC_ARROW || lpCursorName == IDC_IBEAM || lpCursorName == IDC_WAIT ||
		lpCursorName == IDC_CROSS || lpCursorName == IDC_UPARROW || lpCursorName == IDC_SIZE ||
		lpCursorName == IDC_ICON || lpCursorName == IDC_SIZENWSE || lpCursorName == IDC_SIZENESW ||
		lpCursorName == IDC_SIZEWE || lpCursorName == IDC_SIZENS || lpCursorName == IDC_SIZEALL ||
		lpCursorName == IDC_NO || lpCursorName == IDC_APPSTARTING || lpCursorName == IDC_HELP);
	return ::LoadCursor(NULL, lpCursorName);
}

inline HICON AtlLoadIcon(LPCTSTR lpIconName)
{
	return ::LoadIcon(_pModule->GetResourceInstance(), lpIconName);
}

inline HICON AtlLoadIcon(UINT uID)
{
	return ::LoadIcon(_pModule->GetResourceInstance(), MAKEINTRESOURCE(uID));
}

inline HICON AtlLoadSysIcon(LPCTSTR lpIconName)
{
	ATLASSERT(lpIconName == IDI_APPLICATION ||
		lpIconName == IDI_ASTERISK ||
		lpIconName == IDI_EXCLAMATION ||
		lpIconName == IDI_HAND ||
		lpIconName == IDI_QUESTION ||
		lpIconName == IDI_WINLOGO);
	return ::LoadIcon(NULL, lpIconName);
}

inline HANDLE AtlLoadImage(LPCTSTR lpszName, UINT uType, UINT fuLoad = LR_DEFAULTCOLOR | LR_DEFAULTSIZE, int cxDesired = 0, int cyDesired = 0)
{
	ATLASSERT(uType == IMAGE_BITMAP || uType == IMAGE_CURSOR || uType == IMAGE_ICON);
	return ::LoadImage(_pModule->GetResourceInstance(), lpszName, uType, cxDesired, cyDesired, fuLoad);
}

inline HANDLE AtlLoadImage(UINT uID, UINT uType, UINT fuLoad = LR_DEFAULTCOLOR | LR_DEFAULTSIZE, int cxDesired = 0, int cyDesired = 0)
{
	ATLASSERT(uType == IMAGE_BITMAP || uType == IMAGE_CURSOR || uType == IMAGE_ICON);
	ATLASSERT((fuLoad & LR_LOADFROMFILE) == 0);	// this one doesn't load from a file
	return ::LoadImage(_pModule->GetResourceInstance(), MAKEINTRESOURCE(uID), uType, cxDesired, cyDesired, fuLoad);
}

#ifdef OEMRESOURCE
inline HANDLE AtlLoadSysImage(LPCTSTR lpszName, UINT uType, UINT fuLoad = LR_DEFAULTCOLOR | LR_DEFAULTSIZE, int cxDesired = 0, int cyDesired = 0)
{
#ifdef _DEBUG
	WORD wID = (WORD)lpszName;
	if(uType == IMAGE_BITMAP)
		ATLASSERT(wID >= 32734 && wID <= 32767);
	else if(uType == IMAGE_CURSOR)
		ATLASSERT((wID >= 32512 && wID <= 32516) || (wID >= 32640 && wID <= 32648) || (wID == 32650));
	else if(uType == IMAGE_ICON)
		ATLASSERT(wID >= 32512 && wID <= 32517);
	else
		ATLASSERT(FALSE);	// invalid uType value
#endif //_DEBUG
	ATLASSERT((fuLoad & LR_LOADFROMFILE) == 0);	// this one doesn't load from a file
	return ::LoadImage(NULL, lpszName, uType, cxDesired, cyDesired, fuLoad);
}
#endif //OEMRESOURCE

inline int AtlLoadString(UINT uID, LPTSTR lpBuffer, int nBufferMax)
{
	return ::LoadString(_pModule->GetResourceInstance(), uID, lpBuffer, nBufferMax);
}

#if (_ATL_VER >= 0x0400)
inline BOOL AtlLoadStringBSTR(UINT uID, BSTR& bstrText)
{
	return CComBSTR::LoadStringResource(_pModule->GetResourceInstance(), uID, bstrText);
}
#endif //(_ATL_VER >= 0x0400)

}; //namespace ATL

#endif //__ATLLDRES_H__
