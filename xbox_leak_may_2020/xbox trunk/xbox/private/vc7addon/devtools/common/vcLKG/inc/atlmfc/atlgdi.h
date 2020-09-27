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

#ifndef __ATLGDI_H__
#define __ATLGDI_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
	#error atlgdi.h requires atlbase.h to be included first
#endif

#include <commctrl.h>
#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "comctl32.lib")
#endif  // !_ATL_NO_DEFAULT_LIBS

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning(push)
#pragma warning(disable: 4239)  // Nonstandard extension used
#endif

// protect template members from windowsx.h macros
#ifdef _INC_WINDOWSX
#undef CopyRgn
#undef CreateBrush
#undef CreatePen
#undef SelectBrush
#undef SelectPen
#undef SelectFont
#undef SelectBitmap
#endif //_INC_WINDOWSX

#ifdef UNDER_CE

#ifdef TrackPopupMenu
#undef TrackPopupMenu
#endif //TrackPopupMenu

//REVIEW
BOOL IsMenu(HMENU hMenu)
{
	return (hMenu != NULL);
}

#endif //UNDER_CE


namespace ATL
{

/////////////////////////////////////////////////////////////////////////////
// Forward declarations

class CWinMenuItemInfo;
class CWinDC;
class CWinManagedDC;
class CWinPaintDC;
class CWinClientDC;
class CWinWindowDC;

/////////////////////////////////////////////////////////////////////////////
// CWinMenu

class CWinMenuItemInfo : public MENUITEMINFO
{
public:
	CWinMenuItemInfo()
	{
		memset(this, 0, sizeof(MENUITEMINFO));
		cbSize = sizeof(MENUITEMINFO);
	}
};

class CWinMenu
{
public:
	HMENU m_hMenu;

	CWinMenu(HMENU hMenu = NULL) throw() : 
		m_hMenu(hMenu)
	{
	}
	CWinMenu(const CWinMenu& menu) throw() :
		m_hMenu(menu.m_hMenu)
	{
	}
	~CWinMenu() throw()
	{
		// Use CWinManagedMenu to get automatic cleanup
	}

	CWinMenu& operator=(HMENU hMenu) throw()
	{
		m_hMenu = hMenu;
		return *this;
	}

	void Attach(HMENU hMenuNew) throw()
	{
		ATLASSERT(m_hMenu == NULL);
		m_hMenu = hMenuNew;
	}

	HMENU Detach() throw()
	{
		HMENU hMenu = m_hMenu;
		m_hMenu = NULL;
		return hMenu;
	}

	operator HMENU() const throw() 
	{ 
		return m_hMenu; 
	}

	BOOL CreateMenu()
	{
		ATLASSERT(m_hMenu == NULL);
		m_hMenu = ::CreateMenu();
		return (m_hMenu != NULL) ? TRUE : FALSE;
	}
	BOOL CreatePopupMenu()
	{
		ATLASSERT(m_hMenu == NULL);
		m_hMenu = ::CreatePopupMenu();
		return (m_hMenu != NULL) ? TRUE : FALSE;
	}
	BOOL LoadMenu(LPCTSTR lpszResourceName)
	{
		ATLASSERT(m_hMenu == NULL);
		m_hMenu = ::LoadMenu(_AtlBaseModule.GetResourceInstance(), lpszResourceName);
		return (m_hMenu != NULL) ? TRUE : FALSE;
	}
	BOOL LoadMenu(UINT nIDResource)
	{
		ATLASSERT(m_hMenu == NULL);
		m_hMenu = ::LoadMenu(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(nIDResource));
		return (m_hMenu != NULL) ? TRUE : FALSE;
	}
#ifndef UNDER_CE
	BOOL LoadMenuIndirect(const void* lpMenuTemplate)
	{
		ATLASSERT(m_hMenu == NULL);
		m_hMenu = ::LoadMenuIndirect(lpMenuTemplate);
		return (m_hMenu != NULL) ? TRUE : FALSE;
	}
#endif //!UNDER_CE
	BOOL DestroyMenu()
	{
		if (m_hMenu == NULL)
			return FALSE;
		return ::DestroyMenu(Detach());
	}

// Menu Operations
	BOOL DeleteMenu(UINT nPosition, UINT nFlags)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::DeleteMenu(m_hMenu, nPosition, nFlags);
	}
	BOOL TrackPopupMenu(UINT nFlags, int x, int y, HWND hWnd, LPCRECT lpRect = NULL)
	{
		ATLASSERT(::IsMenu(m_hMenu));
#ifndef UNDER_CE
		return ::TrackPopupMenu(m_hMenu, nFlags, x, y, 0, hWnd, lpRect);
#else // CE specific
		return ::TrackPopupMenuEx(m_hMenu, nFlags, x, y, hWnd, NULL);
#endif //!UNDER_CE
	}
	BOOL TrackPopupMenuEx(UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm = NULL)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::TrackPopupMenuEx(m_hMenu, uFlags, x, y, hWnd, lptpm);
	}

// Menu Item Operations
	BOOL AppendMenu(UINT nFlags, UINT nIDNewItem = 0, LPCTSTR lpszNewItem = NULL)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::AppendMenu(m_hMenu, nFlags, nIDNewItem, lpszNewItem);
	}
#ifndef UNDER_CE
//REVIEW
	BOOL AppendMenu(UINT nFlags, UINT nIDNewItem, HBITMAP hBmp)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::AppendMenu(m_hMenu, nFlags | MF_BITMAP, nIDNewItem, (LPCTSTR)hBmp);
	}
#endif //!UNDER_CE
	UINT CheckMenuItem(UINT nIDCheckItem, UINT nCheck)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return (UINT)::CheckMenuItem(m_hMenu, nIDCheckItem, nCheck);
	}
	UINT EnableMenuItem(UINT nIDEnableItem, UINT nEnable)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::EnableMenuItem(m_hMenu, nIDEnableItem, nEnable);
	}
#ifndef UNDER_CE
	int GetMenuItemCount() const
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::GetMenuItemCount(m_hMenu);
	}
	UINT GetMenuItemID(int nPos) const
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::GetMenuItemID(m_hMenu, nPos);
	}
	UINT GetMenuState(UINT nID, UINT nFlags) const
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::GetMenuState(m_hMenu, nID, nFlags);
	}
	int GetMenuString(UINT nIDItem, LPTSTR lpString, int nMaxCount, UINT nFlags) const
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::GetMenuString(m_hMenu, nIDItem, lpString, nMaxCount, nFlags);
	}
	int GetMenuStringLen(UINT nIDItem, UINT nFlags) const
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::GetMenuString(m_hMenu, nIDItem, NULL, 0, nFlags);
	}
#ifndef _ATL_NO_COM
	BOOL GetMenuString(UINT nIDItem, BSTR& bstrText, UINT nFlags) const
	{
		USES_CONVERSION;
		ATLASSERT(::IsMenu(m_hMenu));
		ATLASSERT(bstrText == NULL);

		int nLen = GetMenuStringLen(nIDItem, nFlags);
		{
			bstrText = ::SysAllocString(OLESTR(""));
			return (bstrText != NULL) ? TRUE : FALSE;
		}

		LPTSTR lpszText = (LPTSTR)_alloca((nLen + 1) * sizeof(TCHAR));

		if(!GetMenuString(nIDItem, lpszText, nLen, nFlags))
			return FALSE;

		bstrText = ::SysAllocString(T2OLE(lpszText));
		return (bstrText != NULL) ? TRUE : FALSE;
	}
#endif //!_ATL_NO_COM
#endif //!UNDER_CE
	CWinMenu GetSubMenu(int nPos) const
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return CWinMenu(::GetSubMenu(m_hMenu, nPos));
	}
#ifndef UNDER_CE
//REVIEW
	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0, LPCTSTR lpszNewItem = NULL)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::InsertMenu(m_hMenu, nPosition, nFlags, nIDNewItem, lpszNewItem);
	}
	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, HBITMAP hBmp)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::InsertMenu(m_hMenu, nPosition, nFlags | MF_BITMAP, nIDNewItem, (LPCTSTR)hBmp);
	}
#endif //!UNDER_CE
#ifndef UNDER_CE
	BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0, LPCTSTR lpszNewItem = NULL)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::ModifyMenu(m_hMenu, nPosition, nFlags, nIDNewItem, lpszNewItem);
	}
	BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, HBITMAP hBmp)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::ModifyMenu(m_hMenu, nPosition, nFlags | MF_BITMAP, nIDNewItem, (LPCTSTR)hBmp);
	}
#endif //!UNDER_CE
	BOOL RemoveMenu(UINT nPosition, UINT nFlags)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::RemoveMenu(m_hMenu, nPosition, nFlags);
	}
#ifndef UNDER_CE
	BOOL SetMenuItemBitmaps(UINT nPosition, UINT nFlags, HBITMAP hBmpUnchecked, HBITMAP hBmpChecked)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::SetMenuItemBitmaps(m_hMenu, nPosition, nFlags, hBmpUnchecked, hBmpChecked);
	}
#endif //!UNDER_CE
	BOOL CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::CheckMenuRadioItem(m_hMenu, nIDFirst, nIDLast, nIDItem, nFlags);
	}

	BOOL GetMenuItemInfo(UINT uItem, BOOL bByPosition, LPMENUITEMINFO lpmii)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return (BOOL)::GetMenuItemInfo(m_hMenu, uItem, bByPosition, lpmii);
	}
	BOOL SetMenuItemInfo(UINT uItem, BOOL bByPosition, LPMENUITEMINFO lpmii)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return (BOOL)::SetMenuItemInfo(m_hMenu, uItem, bByPosition, lpmii);
	}
#ifndef UNDER_CE
	BOOL InsertMenuItem(UINT uItem, BOOL bByPosition, LPMENUITEMINFO lpmii)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return (BOOL)::InsertMenuItem(m_hMenu, uItem, bByPosition, lpmii);
	}
#endif // UNDER_CE

// Context Help Functions
#ifndef UNDER_CE
	BOOL SetMenuContextHelpId(DWORD dwContextHelpId)
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::SetMenuContextHelpId(m_hMenu, dwContextHelpId);
	}
	DWORD GetMenuContextHelpId() const
	{
		ATLASSERT(::IsMenu(m_hMenu));
		return ::GetMenuContextHelpId(m_hMenu);
	}
#endif //!UNDER_CE
};

class CWinManagedMenu :
	public CWinMenu
{
public:
	CWinManagedMenu() throw() :
		CWinMenu(NULL)
	{
	}
	CWinManagedMenu(CWinManagedMenu& menu) throw() :
		CWinMenu(menu.Detach())  // Transfers ownership
	{
	}
	explicit CWinManagedMenu(HMENU hMenu) throw() :
		CWinMenu(hMenu)  // Takes ownership
	{
	}
	~CWinManagedMenu() throw()
	{
		if (m_hMenu != NULL)
			DestroyMenu();
	}

	CWinManagedMenu& operator=(CWinManagedMenu& menu) throw()
	{
		if (m_hMenu != NULL)
			DestroyMenu();
		Attach( menu.Detach() );  // Transfers ownership
		return *this;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CWinPen

class CWinPen
{
public:
	HPEN m_hPen;

	CWinPen(HPEN hPen = NULL) throw() : 
		m_hPen(hPen)
	{ 
	}
	CWinPen(const CWinPen& pen) throw() :
		m_hPen(pen.m_hPen)
	{
	}
	~CWinPen() throw()
	{
		// Use CWinManagedPen to get automatic cleanup
	}

	CWinPen& operator=(HPEN hPen) throw()
	{
		m_hPen = hPen;
		return *this;
	}

	void Attach(HPEN hPen) throw()
	{
		ATLASSERT(m_hPen == NULL);
		m_hPen = hPen;
	}
	HPEN Detach() throw()
	{
		HPEN hPen = m_hPen;
		m_hPen = NULL;
		return hPen;
	}

	operator HPEN() const throw() 
	{ 
		return m_hPen; 
	}

	HPEN CreatePen(int nPenStyle, int nWidth, COLORREF crColor) throw()
	{
		ATLASSERT(m_hPen == NULL);
		m_hPen = ::CreatePen(nPenStyle, nWidth, crColor);
		return m_hPen;
	}
#ifndef UNDER_CE
	HPEN CreatePen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush, int nStyleCount = 0, const DWORD* lpStyle = NULL) throw()
	{
		ATLASSERT(m_hPen == NULL);
		m_hPen = ::ExtCreatePen(nPenStyle, nWidth, pLogBrush, nStyleCount, lpStyle);
		return m_hPen;
	}
#endif //!UNDER_CE
	HPEN CreatePenIndirect(LPLOGPEN lpLogPen) throw()
	{
		ATLASSERT(m_hPen == NULL);
		m_hPen = ::CreatePenIndirect(lpLogPen);
		return m_hPen;
	}

	BOOL DeleteObject() throw()
	{
		ATLASSERT(m_hPen != NULL);
		BOOL bRet = ::DeleteObject(m_hPen);
		if(bRet)
			m_hPen = NULL;
		return bRet;
	}

// Attributes
	int GetLogPen(LOGPEN* pLogPen) const throw()
	{
		ATLASSERT(m_hPen != NULL);
		return ::GetObject(m_hPen, sizeof(LOGPEN), pLogPen);
	}
#ifndef UNDER_CE
	int GetExtLogPen(EXTLOGPEN* pLogPen) const throw()
	{
		ATLASSERT(m_hPen != NULL);
		return ::GetObject(m_hPen, sizeof(EXTLOGPEN), pLogPen);
	}
#endif //!UNDER_CE
};

class CWinManagedPen :
	public CWinPen
{
public:
	CWinManagedPen() throw() :
		CWinPen(NULL)
	{
	}
	CWinManagedPen(CWinManagedPen& pen) throw() :
		CWinPen(pen.Detach())  // Transfers ownership
	{
	}
	explicit CWinManagedPen(HPEN hPen) throw() :
		CWinPen(hPen)  // Takes ownership
	{
	}
	~CWinManagedPen() throw()
	{
		if (m_hPen != NULL)
			DeleteObject();
	}

	CWinManagedPen& operator=(CWinManagedPen& pen) throw()
	{
		if (m_hPen != NULL)
			DeleteObject();
		Attach(pen.Detach());  // Transfers ownership

		return *this;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CWinBrush

class CWinBrush
{
public:
	HBRUSH m_hBrush;

	CWinBrush(HBRUSH hBrush = NULL) throw() : 
		m_hBrush(hBrush)
	{ 
	}
	CWinBrush(const CWinBrush& brush) throw() :
		m_hBrush(brush.m_hBrush)
	{
	}
	~CWinBrush() throw()
	{
		// Use CWinManagedBrush to get automatic cleanup
	}

	CWinBrush& operator=(HBRUSH hBrush) throw()
	{
		m_hBrush = hBrush;
		return *this;
	}

	void Attach(HBRUSH hBrush) throw()
	{
		ATLASSERT(m_hBrush == NULL);
		m_hBrush = hBrush;
	}
	HBRUSH Detach() throw()
	{
		HBRUSH hBrush = m_hBrush;
		m_hBrush = NULL;
		return hBrush;
	}

	operator HBRUSH() const throw() 
	{ 
		return m_hBrush; 
	}

	HBRUSH CreateSolidBrush(COLORREF crColor) throw()
	{
		ATLASSERT(m_hBrush == NULL);
		m_hBrush = ::CreateSolidBrush(crColor);
		return m_hBrush;
	}
#ifndef UNDER_CE
	HBRUSH CreateHatchBrush(int nIndex, COLORREF crColor) throw()
	{
		ATLASSERT(m_hBrush == NULL);
		m_hBrush = ::CreateHatchBrush(nIndex, crColor);
		return m_hBrush;
	}
	HBRUSH CreateBrushIndirect(const LOGBRUSH* lpLogBrush) throw()
	{
		ATLASSERT(m_hBrush == NULL);
		m_hBrush = ::CreateBrushIndirect(lpLogBrush);
		return m_hBrush;
	}
#endif //!UNDER_CE
	HBRUSH CreatePatternBrush(HBITMAP hBitmap) throw()
	{
		ATLASSERT(m_hBrush == NULL);
		m_hBrush = ::CreatePatternBrush(hBitmap);
		return m_hBrush;
	}
#ifndef UNDER_CE
//REVIEW
	HBRUSH CreateDIBPatternBrush(HGLOBAL hPackedDIB, UINT nUsage) throw()
	{
		ATLASSERT(hPackedDIB != NULL);
		const void* lpPackedDIB = ::GlobalLock(hPackedDIB);
		ATLASSERT(lpPackedDIB != NULL);
		m_hBrush = ::CreateDIBPatternBrushPt(lpPackedDIB, nUsage);
		::GlobalUnlock(hPackedDIB);
		return m_hBrush;
	}
#endif //!UNDER_CE
	HBRUSH CreateDIBPatternBrush(const void* lpPackedDIB, UINT nUsage) throw()
	{
		ATLASSERT(m_hBrush == NULL);
		m_hBrush = ::CreateDIBPatternBrushPt(lpPackedDIB, nUsage);
		return m_hBrush;
	}
	HBRUSH CreateSysColorBrush(int nIndex) throw()
	{
		ATLASSERT(m_hBrush == NULL);
		m_hBrush = ::GetSysColorBrush(nIndex);
		return m_hBrush;
	}

	BOOL DeleteObject() throw()
	{
		ATLASSERT(m_hBrush != NULL);
		BOOL bRet = ::DeleteObject(m_hBrush);
		if(bRet)
			m_hBrush = NULL;
		return bRet;
	}

// Attributes
	int GetLogBrush(LOGBRUSH* pLogBrush) const throw()
	{
		ATLASSERT(m_hBrush != NULL);
		return ::GetObject(m_hBrush, sizeof(LOGBRUSH), pLogBrush);
	}
};

class CWinManagedBrush :
	public CWinBrush
{
public:
	CWinManagedBrush() throw() :
		CWinBrush(NULL)
	{
	}
	CWinManagedBrush(CWinManagedBrush& brush) throw() :
		CWinBrush(brush.Detach())  // Transfers ownership
	{
	}
	explicit CWinManagedBrush(HBRUSH hBrush) throw() :
		CWinBrush(hBrush)  // Takes ownership
	{
	}
	~CWinManagedBrush() throw()
	{
		if (m_hBrush != NULL)
			DeleteObject();
	}

	CWinManagedBrush& operator=(CWinManagedBrush& brush) throw()
	{
		if (m_hBrush != NULL)
			DeleteObject();
		Attach(brush.Detach());  // Transfers ownership
		return *this;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CWinFont

class CWinFont
{
public:
	HFONT m_hFont;

	CWinFont(HFONT hFont = NULL) throw() : 
		m_hFont(hFont)
	{ 
	}
	CWinFont(const CWinFont& font) throw() :
		m_hFont(font.m_hFont)
	{
	}
	~CWinFont()
	{
		// Use CWinManagedFont to get automatic cleanup
	}

	CWinFont& operator=(HFONT hFont) throw()
	{
		m_hFont = hFont;
		return *this;
	}

	void Attach(HFONT hFont) throw()
	{
		ATLASSERT(m_hFont == NULL);
		m_hFont = hFont;
	}
	HFONT Detach() throw()
	{
		HFONT hFont = m_hFont;
		m_hFont = NULL;
		return hFont;
	}

	operator HFONT() const throw() 
	{ 
		return m_hFont; 
	}

	HFONT CreateFontIndirect(const LOGFONT* lpLogFont) throw()
	{
		ATLASSERT(m_hFont == NULL);
		m_hFont = ::CreateFontIndirect(lpLogFont);
		return m_hFont;
	}
	HFONT CreateFont(int nHeight, int nWidth, int nEscapement,
			int nOrientation, int nWeight, BYTE bItalic, BYTE bUnderline,
			BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision,
			BYTE nClipPrecision, BYTE nQuality, BYTE nPitchAndFamily,
			LPCTSTR lpszFacename) throw()
	{
		ATLASSERT(m_hFont == NULL);
#ifndef UNDER_CE
		m_hFont = ::CreateFont(nHeight, nWidth, nEscapement,
#else // CE specific
		m_hFont = CreateFont(nHeight, nWidth, nEscapement,
#endif //!UNDER_CE
			nOrientation, nWeight, bItalic, bUnderline, cStrikeOut,
			nCharSet, nOutPrecision, nClipPrecision, nQuality,
			nPitchAndFamily, lpszFacename);
		return m_hFont;
	}
#ifndef UNDER_CE
	HFONT CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, HDC hDC = NULL) throw()
	{
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(LOGFONT));
		logFont.lfCharSet = DEFAULT_CHARSET;
		logFont.lfHeight = nPointSize;
		lstrcpyn(logFont.lfFaceName, lpszFaceName, sizeof(logFont.lfFaceName)/sizeof(TCHAR));
		return CreatePointFontIndirect(&logFont, hDC);
	}
	HFONT CreatePointFontIndirect(const LOGFONT* lpLogFont, HDC hDC = NULL) throw()
	{
		HDC hDC1 = (hDC != NULL) ? hDC : (::GetDC(NULL));

		// convert nPointSize to logical units based on hDC
		LOGFONT logFont = *lpLogFont;
		POINT pt;
		pt.y = ::GetDeviceCaps(hDC1, LOGPIXELSY) * logFont.lfHeight;
		pt.y /= 720;    // 72 points/inch, 10 decipoints/point
		::DPtoLP(hDC1, &pt, 1);
		POINT ptOrg = { 0, 0 };
		::DPtoLP(hDC1, &ptOrg, 1);
		logFont.lfHeight = -abs(pt.y - ptOrg.y);

		if(hDC == NULL)
			::ReleaseDC(NULL, hDC1);

		return CreateFontIndirect(&logFont);
	}
#endif //!UNDER_CE

	BOOL DeleteObject() throw()
	{
		ATLASSERT(m_hFont != NULL);
		BOOL bRet = ::DeleteObject(m_hFont);
		if(bRet)
			m_hFont = NULL;
		return bRet;
	}

// Attributes
	int GetLogFont(LOGFONT* pLogFont) const throw()
	{
		ATLASSERT(m_hFont != NULL);
		return ::GetObject(m_hFont, sizeof(LOGFONT), pLogFont);
	}
};

class CWinManagedFont :
	public CWinFont
{
public:
	CWinManagedFont() throw() :
		CWinFont(NULL)
	{
	}
	CWinManagedFont(CWinManagedFont& font) throw() :
		CWinFont(font.Detach())  // Transfers ownership
	{
	}
	explicit CWinManagedFont(HFONT hFont) throw() :
		CWinFont(hFont)  // Takes ownership
	{
	}
	~CWinManagedFont() throw()
	{
		if (m_hFont != NULL)
			DeleteObject();
	}

	CWinManagedFont& operator=(CWinManagedFont& font) throw()
	{
		if (m_hFont != NULL)
			DeleteObject();
		Attach(font.Detach());  // Transfers ownership
		return *this;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CWinBitmap

class CWinBitmap
{
public:
	HBITMAP m_hBitmap;

	CWinBitmap(HBITMAP hBitmap = NULL) throw() : 
		m_hBitmap(hBitmap)
	{ 
	}
	CWinBitmap(const CWinBitmap& bitmap) throw() :
		m_hBitmap(bitmap.m_hBitmap)
	{
	}
	~CWinBitmap() throw()
	{
		// Use CWinManagedBitmap to get automatic cleanup
	}

	CWinBitmap& operator=(HBITMAP hBitmap) throw()
	{
		m_hBitmap = hBitmap;
		return *this;
	}

	void Attach(HBITMAP hBitmap) throw()
	{
		ATLASSERT(m_hBitmap == NULL);
		m_hBitmap = hBitmap;
	}
	HBITMAP Detach() throw()
	{
		HBITMAP hBitmap = m_hBitmap;
		m_hBitmap = NULL;
		return hBitmap;
	}

	operator HBITMAP() const throw() 
	{ 
		return m_hBitmap; 
	}

	HBITMAP LoadBitmap(LPCTSTR lpszResourceName) throw()
	{
		ATLASSERT(m_hBitmap == NULL);
		m_hBitmap = ::LoadBitmap(_AtlBaseModule.GetResourceInstance(), lpszResourceName);
		return m_hBitmap;
	}
	HBITMAP LoadBitmap(UINT nIDResource) throw()
	{
		ATLASSERT(m_hBitmap == NULL);
		m_hBitmap = ::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(nIDResource));
		return m_hBitmap;
	}
	HBITMAP LoadOEMBitmap(UINT nIDBitmap) throw() // for OBM_/OCR_/OIC_
	{
		ATLASSERT(m_hBitmap == NULL);
		m_hBitmap = ::LoadBitmap(NULL, MAKEINTRESOURCE(nIDBitmap));
		return m_hBitmap;
	}
#ifndef UNDER_CE
	HBITMAP LoadMappedBitmap(UINT nIDBitmap, UINT nFlags = 0, LPCOLORMAP lpColorMap = NULL, int nMapSize = 0) throw()
	{
		ATLASSERT(m_hBitmap == NULL);
		m_hBitmap = ::CreateMappedBitmap(_AtlBaseModule.GetResourceInstance(), nIDBitmap, (WORD)nFlags, lpColorMap, nMapSize);
		return m_hBitmap;
	}
#endif // UNDER_CE
	HBITMAP CreateBitmap(int nWidth, int nHeight, UINT nPlanes, UINT nBitcount, const void* lpBits) throw()
	{
		ATLASSERT(m_hBitmap == NULL);
		m_hBitmap = ::CreateBitmap(nWidth, nHeight, nPlanes, nBitcount, lpBits);
		return m_hBitmap;
	}
#ifndef UNDER_CE
	HBITMAP CreateBitmapIndirect(LPBITMAP lpBitmap) throw()
	{
		ATLASSERT(m_hBitmap == NULL);
		m_hBitmap = ::CreateBitmapIndirect(lpBitmap);
		return m_hBitmap;
	}
#endif //!UNDER_CE
	HBITMAP CreateCompatibleBitmap(HDC hDC, int nWidth, int nHeight) throw()
	{
		ATLASSERT(m_hBitmap == NULL);
		m_hBitmap = ::CreateCompatibleBitmap(hDC, nWidth, nHeight);
		return m_hBitmap;
	}
#ifndef UNDER_CE
	HBITMAP CreateDiscardableBitmap(HDC hDC, int nWidth, int nHeight) throw()
	{
		ATLASSERT(m_hBitmap == NULL);
		m_hBitmap = ::CreateDiscardableBitmap(hDC, nWidth, nHeight);
		return m_hBitmap;
	}
#endif //!UNDER_CE

	BOOL DeleteObject() throw()
	{
		ATLASSERT(m_hBitmap != NULL);
		BOOL bRet = ::DeleteObject(m_hBitmap);
		if(bRet)
			m_hBitmap = NULL;
		return bRet;
	}

// Attributes
	int GetBitmap(BITMAP* pBitMap) const throw()
	{
		ATLASSERT(m_hBitmap != NULL);
		return ::GetObject(m_hBitmap, sizeof(BITMAP), pBitMap);
	}

// Operations
#ifndef UNDER_CE
//REVIEW
	DWORD SetBitmapBits(DWORD dwCount, const void* lpBits) throw()
	{
		ATLASSERT(m_hBitmap != NULL);
		return ::SetBitmapBits(m_hBitmap, dwCount, lpBits);
	}
	DWORD GetBitmapBits(DWORD dwCount, LPVOID lpBits) const throw()
	{
		ATLASSERT(m_hBitmap != NULL);
		return ::GetBitmapBits(m_hBitmap, dwCount, lpBits);
	}
	BOOL SetBitmapDimension(int nWidth, int nHeight, LPSIZE lpSize = NULL) throw()
	{
		ATLASSERT(m_hBitmap != NULL);
		return ::SetBitmapDimensionEx(m_hBitmap, nWidth, nHeight, lpSize);
	}
	BOOL GetBitmapDimension(LPSIZE lpSize) const throw()
	{
		ATLASSERT(m_hBitmap != NULL);
		return ::GetBitmapDimensionEx(m_hBitmap, lpSize);
	}
#endif //!UNDER_CE
};

class CWinManagedBitmap :
	public CWinBitmap
{
public:
	CWinManagedBitmap() throw() :
		CWinBitmap(NULL)
	{
	}
	CWinManagedBitmap(CWinManagedBitmap& bitmap) throw() :
		CWinBitmap(bitmap.Detach())  // Transfers ownership
	{
	}
	explicit CWinManagedBitmap(HBITMAP hBitmap) throw() :
		CWinBitmap(hBitmap)  // Takes ownership
	{
	}
	~CWinManagedBitmap() throw()
	{
		if (m_hBitmap != NULL)
			DeleteObject();
	}

	CWinManagedBitmap& operator=(CWinManagedBitmap& bitmap) throw()
	{
		if (m_hBitmap != NULL)
			DeleteObject();
		Attach(bitmap.Detach());  // Transfers ownership
		return *this;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CWinPalette

class CWinPalette
{
public:
	HPALETTE m_hPalette;

	CWinPalette(HPALETTE hPalette = NULL) throw() : 
		m_hPalette(hPalette)
	{ 
	}
	CWinPalette(const CWinPalette& palette) throw() :
		m_hPalette(palette.m_hPalette)
	{
	}
	~CWinPalette() throw()
	{
	}

	CWinPalette& operator=(HPALETTE hPalette) throw()
	{
		m_hPalette = hPalette;
		return *this;
	}

	void Attach(HPALETTE hPalette) throw()
	{
		ATLASSERT(m_hPalette == NULL);
		m_hPalette = hPalette;
	}
	HPALETTE Detach() throw()
	{
		HPALETTE hPalette = m_hPalette;
		m_hPalette = NULL;
		return hPalette;
	}

	operator HPALETTE() const throw() 
	{ 
		return m_hPalette; 
	}

	HPALETTE CreatePalette(LPLOGPALETTE lpLogPalette) throw()
	{
		ATLASSERT(m_hPalette == NULL);
		m_hPalette = ::CreatePalette(lpLogPalette);
		return m_hPalette;
	}
#ifndef UNDER_CE
	HPALETTE CreateHalftonePalette(HDC hDC) throw()
	{
		ATLASSERT(m_hPalette == NULL);
		ATLASSERT(hDC != NULL);
		m_hPalette = ::CreateHalftonePalette(hDC);
		return m_hPalette;
	}
#endif //!UNDER_CE

	BOOL DeleteObject() throw()
	{
		ATLASSERT(m_hPalette != NULL);
		BOOL bRet = ::DeleteObject(m_hPalette);
		if(bRet)
			m_hPalette = NULL;
		return bRet;
	}

// Attributes
	int GetEntryCount() const throw()
	{
		ATLASSERT(m_hPalette != NULL);
		WORD nEntries;
		::GetObject(m_hPalette, sizeof(WORD), &nEntries);
		return (int)nEntries;
	}
	UINT GetPaletteEntries(UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY lpPaletteColors) const throw()
	{
		ATLASSERT(m_hPalette != NULL);
		return ::GetPaletteEntries(m_hPalette, nStartIndex, nNumEntries, lpPaletteColors);
	}
	UINT SetPaletteEntries(UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY lpPaletteColors) throw()
	{
		ATLASSERT(m_hPalette != NULL);
		return ::SetPaletteEntries(m_hPalette, nStartIndex, nNumEntries, lpPaletteColors);
	}

// Operations
#ifndef UNDER_CE
	void AnimatePalette(UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY lpPaletteColors) throw()
	{
		ATLASSERT(m_hPalette != NULL);
		::AnimatePalette(m_hPalette, nStartIndex, nNumEntries, lpPaletteColors);
	}
	BOOL ResizePalette(UINT nNumEntries) throw()
	{
		ATLASSERT(m_hPalette != NULL);
		return ::ResizePalette(m_hPalette, nNumEntries);
	}
#endif //!UNDER_CE
	UINT GetNearestPaletteIndex(COLORREF crColor) const throw()
	{
		ATLASSERT(m_hPalette != NULL);
		return ::GetNearestPaletteIndex(m_hPalette, crColor);
	}
};

class CWinManagedPalette :
	public CWinPalette
{
public:
	CWinManagedPalette() throw() :
		CWinPalette(NULL)
	{
	}
	CWinManagedPalette(CWinManagedPalette& palette) throw() :
		CWinPalette(palette.Detach())  // Transfers ownership
	{
	}
	explicit CWinManagedPalette(HPALETTE hPalette) throw() :
		CWinPalette(hPalette)  // Takes ownership
	{
	}
	~CWinManagedPalette() throw()
	{
		if (m_hPalette != NULL)
			DeleteObject();
	}

	CWinManagedPalette& operator=(CWinManagedPalette& palette) throw()
	{
		if (m_hPalette != NULL)
			DeleteObject();
		Attach(palette.Detach());  // Transfers ownership
		return *this;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CWinRgn

class CWinRgn
{
public:
	HRGN m_hRgn;

	CWinRgn(HRGN hRgn = NULL) throw() : 
		m_hRgn(hRgn)
	{ 
	}
	CWinRgn(const CWinRgn& rgn) throw() :
		m_hRgn(rgn.m_hRgn)
	{
	}
	~CWinRgn() throw()
	{
		// Use CWinManagedRgn to get automatic cleanup
	}

	CWinRgn& operator=(HRGN hRgn) throw()
	{
		m_hRgn = hRgn;
		return *this;
	}

	void Attach(HRGN hRgn) throw()
	{
		ATLASSERT(m_hRgn == NULL);
		m_hRgn = hRgn;
	}
	HRGN Detach() throw()
	{
		HRGN hRgn = m_hRgn;
		m_hRgn = NULL;
		return hRgn;
	}

	operator HRGN() const throw()
	{ 
		return m_hRgn; 
	}

	HRGN CreateRectRgn(int x1, int y1, int x2, int y2) throw()
	{
		ATLASSERT(m_hRgn == NULL);
		m_hRgn = ::CreateRectRgn(x1, y1, x2, y2);
		return m_hRgn;
	}
	HRGN CreateRectRgnIndirect(LPCRECT lpRect) throw()
	{
		ATLASSERT(m_hRgn == NULL);
		m_hRgn = ::CreateRectRgnIndirect(lpRect);
		return m_hRgn;
	}
#ifndef UNDER_CE
	HRGN CreateEllipticRgn(int x1, int y1, int x2, int y2) throw()
	{
		ATLASSERT(m_hRgn == NULL);
		m_hRgn = ::CreateEllipticRgn(x1, y1, x2, y2);
		return m_hRgn;
	}
	HRGN CreateEllipticRgnIndirect(LPCRECT lpRect) throw()
	{
		ATLASSERT(m_hRgn == NULL);
		m_hRgn = ::CreateEllipticRgnIndirect(lpRect);
		return m_hRgn;
	}
	HRGN CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode) throw()
	{
		ATLASSERT(m_hRgn == NULL);
		m_hRgn = ::CreatePolygonRgn(lpPoints, nCount, nMode);
		return m_hRgn;
	}
	HRGN CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyFillMode) throw()
	{
		ATLASSERT(m_hRgn == NULL);
		m_hRgn = ::CreatePolyPolygonRgn(lpPoints, lpPolyCounts, nCount, nPolyFillMode);
		return m_hRgn;
	}
	HRGN CreateRoundRectRgn(int x1, int y1, int x2, int y2, int x3, int y3) throw()
	{
		ATLASSERT(m_hRgn == NULL);
		m_hRgn = ::CreateRoundRectRgn(x1, y1, x2, y2, x3, y3);
		return m_hRgn;
	}
	HRGN CreateFromPath(HDC hDC) throw()
	{
		ATLASSERT(m_hRgn == NULL);
		ATLASSERT(hDC != NULL);
		m_hRgn = ::PathToRegion(hDC);
		return m_hRgn;
	}
	HRGN CreateFromData(const XFORM* lpXForm, int nCount, const RGNDATA* pRgnData) throw()
	{
		ATLASSERT(m_hRgn == NULL);
		m_hRgn = ::ExtCreateRegion(lpXForm, nCount, pRgnData);
		return m_hRgn;
	}
#endif //!UNDER_CE

	BOOL DeleteObject() throw()
	{
		ATLASSERT(m_hRgn != NULL);
		BOOL bRet = ::DeleteObject(m_hRgn);
		if(bRet)
			m_hRgn = NULL;
		return bRet;
	}

// Operations
	void SetRectRgn(int x1, int y1, int x2, int y2) throw()
	{
		ATLASSERT(m_hRgn != NULL);
		::SetRectRgn(m_hRgn, x1, y1, x2, y2);
	}
	void SetRectRgn(LPCRECT lpRect) throw()
	{
		ATLASSERT(m_hRgn != NULL);
		::SetRectRgn(m_hRgn, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	}
	int CombineRgn(HRGN hRgnSrc1, HRGN hRgnSrc2, int nCombineMode) throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return ::CombineRgn(m_hRgn, hRgnSrc1, hRgnSrc2, nCombineMode);
	}
	int CombineRgn(HRGN hRgnSrc, int nCombineMode) throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return ::CombineRgn(m_hRgn, m_hRgn, hRgnSrc, nCombineMode);
	}
	int CopyRgn(HRGN hRgnSrc) throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return ::CombineRgn(m_hRgn, hRgnSrc, NULL, RGN_COPY);
	}
	BOOL EqualRgn(HRGN hRgn) const throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return ::EqualRgn(m_hRgn, hRgn);
	}
	int OffsetRgn(int x, int y) throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return ::OffsetRgn(m_hRgn, x, y);
	}
	int OffsetRgn(POINT point) throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return ::OffsetRgn(m_hRgn, point.x, point.y);
	}
	int GetRgnBox(LPRECT lpRect) const throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return ::GetRgnBox(m_hRgn, lpRect);
	}
	BOOL PtInRegion(int x, int y) const throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return ::PtInRegion(m_hRgn, x, y);
	}
	BOOL PtInRegion(POINT point) const throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return ::PtInRegion(m_hRgn, point.x, point.y);
	}
	BOOL RectInRegion(LPCRECT lpRect) const throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return ::RectInRegion(m_hRgn, lpRect);
	}
	int GetRegionData(LPRGNDATA lpRgnData, int nDataSize) const throw()
	{
		ATLASSERT(m_hRgn != NULL);
		return (int)::GetRegionData(m_hRgn, nDataSize, lpRgnData);
	}
};

class CWinManagedRgn :
	public CWinRgn
{
public:
	CWinManagedRgn() throw() :
		CWinRgn(NULL)
	{
	}
	CWinManagedRgn(CWinManagedRgn& rgn) throw() :
		CWinRgn(rgn.Detach())  // Transfers ownership
	{
	}
	explicit CWinManagedRgn(HRGN hRgn) throw() :
		CWinRgn(hRgn)  // Takes ownership
	{
	}
	~CWinManagedRgn() throw()
	{
		if (m_hRgn != NULL)
			DeleteObject();
	}

	CWinManagedRgn& operator=(CWinManagedRgn& rgn) throw()
	{
		if (m_hRgn != NULL)
			DeleteObject();
		Attach(rgn.Detach());  // Transfers ownership
		return *this;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CWinDC - The device context class

class CWinDC
{
public:
// Attributes
	HDC m_hDC;


	CWinDC(HDC hDC = NULL) throw() : 
		m_hDC(hDC)
	{
	}
	CWinDC(const CWinDC& dc) throw() :
		m_hDC(dc.m_hDC)
	{
	}
	~CWinDC() throw()
	{
	}
	CWinDC& operator=(HDC hDC) throw()
	{
		m_hDC = hDC;
		return *this;
	}

	void Attach(HDC hDC) throw()
	{
		ATLASSERT(m_hDC == NULL);
		m_hDC = hDC;
	}

	HDC Detach() throw()
	{
		HDC hDC = m_hDC;
		m_hDC = NULL;
		return hDC;
	}

	operator HDC() const throw() 
	{ 
		return m_hDC; 
	}

#ifndef UNDER_CE
	HWND WindowFromDC() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::WindowFromDC(m_hDC);
	}
#endif //!UNDER_CE

	CWinPen GetCurrentPen() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return CWinPen((HPEN)::GetCurrentObject(m_hDC, OBJ_PEN));
	}
	CWinBrush GetCurrentBrush() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return CWinBrush((HBRUSH)::GetCurrentObject(m_hDC, OBJ_BRUSH));
	}
	CWinPalette GetCurrentPalette() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return CWinPalette((HPALETTE)::GetCurrentObject(m_hDC, OBJ_PAL));
	}
	CWinFont GetCurrentFont() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return CWinFont((HFONT)::GetCurrentObject(m_hDC, OBJ_FONT));
	}
	CWinBitmap GetCurrentBitmap() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return CWinBitmap((HBITMAP)::GetCurrentObject(m_hDC, OBJ_BITMAP));
	}

	HDC CreateDC(LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName,
		LPCTSTR lpszOutput, const DEVMODE* lpInitData) throw()
	{
		ATLASSERT(m_hDC == NULL);
		m_hDC = ::CreateDC(lpszDriverName, lpszDeviceName, lpszOutput, lpInitData);
		return m_hDC;
	}

	HDC CreateCompatibleDC(HDC hDC = NULL) throw()
	{
		ATLASSERT(m_hDC == NULL);
		m_hDC = ::CreateCompatibleDC(hDC);
		return m_hDC;
	}

	BOOL DeleteDC() throw()
	{
		if(m_hDC == NULL)
			return FALSE;

		return ::DeleteDC(Detach());
	}

// Device-Context Functions
	int SaveDC() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SaveDC(m_hDC);
	}

	BOOL RestoreDC(int nSavedDC) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::RestoreDC(m_hDC, nSavedDC);
	}

	int GetDeviceCaps(int nIndex) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetDeviceCaps(m_hDC, nIndex);
	}
#ifndef UNDER_CE
	UINT SetBoundsRect(LPCRECT lpRectBounds, UINT flags) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetBoundsRect(m_hDC, lpRectBounds, flags);
	}
	UINT GetBoundsRect(LPRECT lpRectBounds, UINT flags) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetBoundsRect(m_hDC, lpRectBounds, flags);
	}
	BOOL ResetDC(const DEVMODE* lpDevMode) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ResetDC(m_hDC, lpDevMode) != NULL;
	}
#endif //!UNDER_CE

// Drawing-Tool Functions
#ifndef UNDER_CE
	BOOL GetBrushOrg(LPPOINT lpPoint) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetBrushOrgEx(m_hDC, lpPoint);
	}
#endif //!UNDER_CE
	BOOL SetBrushOrg(int x, int y, LPPOINT lpPoint = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetBrushOrgEx(m_hDC, x, y, lpPoint);
	}
	BOOL SetBrushOrg(POINT point, LPPOINT lpPointRet = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetBrushOrgEx(m_hDC, point.x, point.y, lpPointRet);
	}
#ifndef UNDER_CE
	int EnumObjects(int nObjectType, int (CALLBACK* lpfn)(LPVOID, LPARAM), LPARAM lpData) throw()
	{
		ATLASSERT(m_hDC != NULL);
#ifdef STRICT
		return ::EnumObjects(m_hDC, nObjectType, (GOBJENUMPROC)lpfn, lpData);
#else
		return ::EnumObjects(m_hDC, nObjectType, (GOBJENUMPROC)lpfn, (LPVOID)lpData);
#endif
	}
#endif //!UNDER_CE

// Type-safe selection helpers
	HPEN SelectPen(HPEN hPen) throw()
	{
		ATLASSERT(m_hDC != NULL);
#ifndef UNDER_CE
		ATLASSERT(::GetObjectType(hPen) == OBJ_PEN || ::GetObjectType(hPen) == OBJ_EXTPEN);
#else // CE specific
		ATLASSERT(::GetObjectType(hPen) == OBJ_PEN);
#endif //!UNDER_CE
		return (HPEN)::SelectObject(m_hDC, hPen);
	}
	HBRUSH SelectBrush(HBRUSH hBrush) throw()
	{
		ATLASSERT(m_hDC != NULL);
		ATLASSERT(::GetObjectType(hBrush) == OBJ_BRUSH);
		return (HBRUSH)::SelectObject(m_hDC, hBrush);
	}
	HFONT SelectFont(HFONT hFont) throw()
	{
		ATLASSERT(m_hDC != NULL);
		ATLASSERT(::GetObjectType(hFont) == OBJ_FONT);
		return (HFONT)::SelectObject(m_hDC, hFont);
	}
	HBITMAP SelectBitmap(HBITMAP hBitmap) throw()
	{
		ATLASSERT(m_hDC != NULL);
		ATLASSERT(::GetObjectType(hBitmap) == OBJ_BITMAP);
		return (HBITMAP)::SelectObject(m_hDC, hBitmap);
	}
	int SelectRgn(HRGN hRgn) throw()       // special return for regions
	{
		ATLASSERT(m_hDC != NULL);
		ATLASSERT(::GetObjectType(hRgn) == OBJ_REGION);
		return PtrToInt(::SelectObject(m_hDC, hRgn));
	}

	HGDIOBJ SelectStockObject(int nIndex) throw()
	{
		ATLASSERT(m_hDC != NULL);
		HGDIOBJ hObject = ::GetStockObject(nIndex);
		ATLASSERT(hObject != NULL);
		switch(::GetObjectType(hObject))
		{
		case OBJ_PEN:
#ifndef UNDER_CE
/*?*/		case OBJ_EXTPEN:
#endif //!UNDER_CE
			return SelectPen((HPEN)hObject);
		case OBJ_BRUSH:
			return SelectBrush((HBRUSH)hObject);
		case OBJ_FONT:
			return SelectFont((HFONT)hObject);
		default:
			return NULL;
		}
	}

// Color and Color Palette Functions
	COLORREF GetNearestColor(COLORREF crColor) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetNearestColor(m_hDC, crColor);
	}
	HPALETTE SelectPalette(HPALETTE hPalette, BOOL bForceBackground) throw()
	{
		ATLASSERT(m_hDC != NULL);

		HPALETTE hOldPal = ::SelectPalette(m_hDC, hPalette, bForceBackground);
//		if(/*m_bAutoRestore && */m_hOriginalPal == NULL)
//			m_hOriginalPal = hOldPal;
		return hOldPal;
	}
	UINT RealizePalette() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::RealizePalette(m_hDC);
	}
#ifndef UNDER_CE
	void UpdateColors() throw()
	{
		ATLASSERT(m_hDC != NULL);
		::UpdateColors(m_hDC);
	}
#endif //!UNDER_CE

// Drawing-Attribute Functions
	COLORREF GetBkColor() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetBkColor(m_hDC);
	}
	int GetBkMode() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetBkMode(m_hDC);
	}
#ifndef UNDER_CE
	int GetPolyFillMode() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetPolyFillMode(m_hDC);
	}
	int GetROP2() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetROP2(m_hDC);
	}
	int GetStretchBltMode() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetStretchBltMode(m_hDC);
	}
#endif //!UNDER_CE
	COLORREF GetTextColor() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetTextColor(m_hDC);
	}

	COLORREF SetBkColor(COLORREF crColor) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetBkColor(m_hDC, crColor);
	}
	int SetBkMode(int nBkMode) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetBkMode(m_hDC, nBkMode);
	}
#ifndef UNDER_CE
	int SetPolyFillMode(int nPolyFillMode) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetPolyFillMode(m_hDC, nPolyFillMode);
	}
#endif //!UNDER_CE
	int SetROP2(int nDrawMode) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetROP2(m_hDC, nDrawMode);
	}
#ifndef UNDER_CE
	int SetStretchBltMode(int nStretchMode) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetStretchBltMode(m_hDC, nStretchMode);
	}
#endif //!UNDER_CE
	COLORREF SetTextColor(COLORREF crColor) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetTextColor(m_hDC, crColor);
	}

#ifndef UNDER_CE
	BOOL GetColorAdjustment(LPCOLORADJUSTMENT lpColorAdjust) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetColorAdjustment(m_hDC, lpColorAdjust);
	}
	BOOL SetColorAdjustment(const COLORADJUSTMENT* lpColorAdjust) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetColorAdjustment(m_hDC, lpColorAdjust);
	}
#endif //!UNDER_CE

// Mapping Functions
#ifndef UNDER_CE
	int GetMapMode() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetMapMode(m_hDC);
	}
	BOOL GetViewportOrg(LPPOINT lpPoint) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetViewportOrgEx(m_hDC, lpPoint);
	}
	int SetMapMode(int nMapMode) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetMapMode(m_hDC, nMapMode);
	}
	// Viewport Origin
	BOOL SetViewportOrg(int x, int y, LPPOINT lpPoint = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetViewportOrgEx(m_hDC, x, y, lpPoint);
	}
	BOOL SetViewportOrg(POINT point, LPPOINT lpPointRet = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return SetViewportOrg(point.x, point.y, lpPointRet);
	}
	BOOL OffsetViewportOrg(int nWidth, int nHeight, LPPOINT lpPoint = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::OffsetViewportOrgEx(m_hDC, nWidth, nHeight, lpPoint);
	}

	// Viewport Extent
	BOOL GetViewportExt(LPSIZE lpSize) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetViewportExtEx(m_hDC, lpSize);
	}
	BOOL SetViewportExt(int x, int y, LPSIZE lpSize = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetViewportExtEx(m_hDC, x, y, lpSize);
	}
	BOOL SetViewportExt(SIZE size, LPSIZE lpSizeRet = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return SetViewportExt(size.cx, size.cy, lpSizeRet);
	}
	BOOL ScaleViewportExt(int xNum, int xDenom, int yNum, int yDenom, LPSIZE lpSize = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ScaleViewportExtEx(m_hDC, xNum, xDenom, yNum, yDenom, lpSize);
	}

	// Window Origin
	BOOL GetWindowOrg(LPPOINT lpPoint) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetWindowOrgEx(m_hDC, lpPoint);
	}
	BOOL SetWindowOrg(int x, int y, LPPOINT lpPoint = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetWindowOrgEx(m_hDC, x, y, lpPoint);
	}
	BOOL SetWindowOrg(POINT point, LPPOINT lpPointRet = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return SetWindowOrg(point.x, point.y, lpPointRet);
	}
	BOOL OffsetWindowOrg(int nWidth, int nHeight, LPPOINT lpPoint = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::OffsetWindowOrgEx(m_hDC, nWidth, nHeight, lpPoint);
	}

	// Window extent
	BOOL GetWindowExt(LPSIZE lpSize) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetWindowExtEx(m_hDC, lpSize);
	}
	BOOL SetWindowExt(int x, int y, LPSIZE lpSize = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetWindowExtEx(m_hDC, x, y, lpSize);
	}
	BOOL SetWindowExt(SIZE size, LPSIZE lpSizeRet) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return SetWindowExt(size.cx, size.cy, lpSizeRet);
	}
	BOOL ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom, LPSIZE lpSize = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ScaleWindowExtEx(m_hDC, xNum, xDenom, yNum, yDenom, lpSize);
	}

// Coordinate Functions
	BOOL DPtoLP(LPPOINT lpPoints, int nCount = 1) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DPtoLP(m_hDC, lpPoints, nCount);
	}
	BOOL DPtoLP(LPRECT lpRect) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DPtoLP(m_hDC, (LPPOINT)lpRect, 2);
	}
	BOOL DPtoLP(LPSIZE lpSize) const throw()
	{
		SIZE sizeWinExt;
		if(!GetWindowExt(&sizeWinExt))
			return FALSE;
		SIZE sizeVpExt;
		if(!GetViewportExt(&sizeVpExt))
			return FALSE;
		lpSize->cx = MulDiv(lpSize->cx, abs(sizeWinExt.cx), abs(sizeVpExt.cx));
		lpSize->cy = MulDiv(lpSize->cy, abs(sizeWinExt.cy), abs(sizeVpExt.cy));
		return TRUE;
	}
	BOOL LPtoDP(LPPOINT lpPoints, int nCount = 1) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::LPtoDP(m_hDC, lpPoints, nCount);
	}
	BOOL LPtoDP(LPRECT lpRect) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::LPtoDP(m_hDC, (LPPOINT)lpRect, 2);
	}
	BOOL LPtoDP(LPSIZE lpSize) const throw()
	{
		SIZE sizeWinExt;
		if(!GetWindowExt(&sizeWinExt))
			return FALSE;
		SIZE sizeVpExt;
		if(!GetViewportExt(&sizeVpExt))
			return FALSE;
		lpSize->cx = MulDiv(lpSize->cx, abs(sizeVpExt.cx), abs(sizeWinExt.cx));
		lpSize->cy = MulDiv(lpSize->cy, abs(sizeVpExt.cy), abs(sizeWinExt.cy));
		return TRUE;
	}

// Special Coordinate Functions (useful for dealing with metafiles and OLE)
	#define HIMETRIC_INCH   2540    // HIMETRIC units per inch

	void DPtoHIMETRIC(LPSIZE lpSize) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		int nMapMode;
		if((nMapMode = GetMapMode()) < MM_ISOTROPIC && nMapMode != MM_TEXT)
		{
			// when using a constrained map mode, map against physical inch
			const_cast<CWinDC*>(this)->SetMapMode(MM_HIMETRIC);
			DPtoLP(lpSize);
			const_cast<CWinDC*>(this)->SetMapMode(nMapMode);
		}
		else
		{
			// map against logical inch for non-constrained mapping modes
			int cxPerInch = GetDeviceCaps(LOGPIXELSX);
			int cyPerInch = GetDeviceCaps(LOGPIXELSY);
			ATLASSERT(cxPerInch != 0 && cyPerInch != 0);
			lpSize->cx = MulDiv(lpSize->cx, HIMETRIC_INCH, cxPerInch);
			lpSize->cy = MulDiv(lpSize->cy, HIMETRIC_INCH, cyPerInch);
		}
	}

	void HIMETRICtoDP(LPSIZE lpSize) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		int nMapMode;
		if((nMapMode = GetMapMode()) < MM_ISOTROPIC && nMapMode != MM_TEXT)
		{
			// when using a constrained map mode, map against physical inch
			const_cast<CWinDC*>(this)->SetMapMode(MM_HIMETRIC);
			LPtoDP(lpSize);
			const_cast<CWinDC*>(this)->SetMapMode(nMapMode);
		}
		else
		{
			// map against logical inch for non-constrained mapping modes
			int cxPerInch = GetDeviceCaps(LOGPIXELSX);
			int cyPerInch = GetDeviceCaps(LOGPIXELSY);
			ATLASSERT(cxPerInch != 0 && cyPerInch != 0);
			lpSize->cx = MulDiv(lpSize->cx, cxPerInch, HIMETRIC_INCH);
			lpSize->cy = MulDiv(lpSize->cy, cyPerInch, HIMETRIC_INCH);
		}
	}

	void LPtoHIMETRIC(LPSIZE lpSize) const throw()
	{
		LPtoDP(lpSize);
		DPtoHIMETRIC(lpSize);
	}

	void HIMETRICtoLP(LPSIZE lpSize) const throw()
	{
		HIMETRICtoDP(lpSize);
		DPtoLP(lpSize);
	}
#endif //!UNDER_CE

// Region Functions
	BOOL FillRgn(HRGN hRgn, HBRUSH hBrush) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::FillRgn(m_hDC, hRgn, hBrush);
	}
#ifndef UNDER_CE
	BOOL FrameRgn(HRGN hRgn, HBRUSH hBrush, int nWidth, int nHeight) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::FrameRgn(m_hDC, hRgn, hBrush, nWidth, nHeight);
	}
	BOOL InvertRgn(HRGN hRgn) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::InvertRgn(m_hDC, hRgn);
	}
	BOOL PaintRgn(HRGN hRgn) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PaintRgn(m_hDC, hRgn);
	}
#endif //!UNDER_CE

// Clipping Functions
	int GetClipBox(LPRECT lpRect) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetClipBox(m_hDC, lpRect);
	}
#ifndef UNDER_CE
	BOOL PtVisible(int x, int y) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PtVisible(m_hDC, x, y);
	}
	BOOL PtVisible(POINT point) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PtVisible(m_hDC, point.x, point.y);
	}
#endif //!UNDER_CE
	BOOL RectVisible(LPCRECT lpRect) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::RectVisible(m_hDC, lpRect);
	}
	int SelectClipRgn(HRGN hRgn) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SelectClipRgn(m_hDC, (HRGN)hRgn);
	}
	int ExcludeClipRect(int x1, int y1, int x2, int y2) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ExcludeClipRect(m_hDC, x1, y1, x2, y2);
	}
	int ExcludeClipRect(LPCRECT lpRect) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ExcludeClipRect(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	}
#ifndef UNDER_CE
	int ExcludeUpdateRgn(HWND hWnd) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ExcludeUpdateRgn(m_hDC, hWnd);
	}
#endif //!UNDER_CE
	int IntersectClipRect(int x1, int y1, int x2, int y2) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::IntersectClipRect(m_hDC, x1, y1, x2, y2);
	}
	int IntersectClipRect(LPCRECT lpRect) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::IntersectClipRect(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	}
#ifndef UNDER_CE
	int OffsetClipRgn(int x, int y) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::OffsetClipRgn(m_hDC, x, y);
	}
	int OffsetClipRgn(SIZE size) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::OffsetClipRgn(m_hDC, size.cx, size.cy);
	}
	int SelectClipRgn(HRGN hRgn, int nMode) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ExtSelectClipRgn(m_hDC, hRgn, nMode);
	}
#endif //!UNDER_CE

// Line-Output Functions
#ifndef UNDER_CE
//REVIEW
	BOOL GetCurrentPosition(LPPOINT lpPoint) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetCurrentPositionEx(m_hDC, lpPoint);
	}
	BOOL MoveTo(int x, int y, LPPOINT lpPoint = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::MoveToEx(m_hDC, x, y, lpPoint);
	}
	BOOL MoveTo(POINT point, LPPOINT lpPointRet = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return MoveTo(point.x, point.y, lpPointRet);
	}
	BOOL LineTo(int x, int y) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::LineTo(m_hDC, x, y);
	}
	BOOL LineTo(POINT point) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return LineTo(point.x, point.y);
	}
	BOOL Arc(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Arc(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4);
	}
	BOOL Arc(LPCRECT lpRect, POINT ptStart, POINT ptEnd) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Arc(m_hDC, lpRect->left, lpRect->top,
			lpRect->right, lpRect->bottom, ptStart.x, ptStart.y,
			ptEnd.x, ptEnd.y);
	}
#endif //!UNDER_CE
	BOOL Polyline(LPPOINT lpPoints, int nCount) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Polyline(m_hDC, lpPoints, nCount);
	}

#ifndef UNDER_CE
	BOOL AngleArc(int x, int y, int nRadius, float fStartAngle, float fSweepAngle) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::AngleArc(m_hDC, x, y, nRadius, fStartAngle, fSweepAngle);
	}
	BOOL ArcTo(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ArcTo(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4);
	}
	BOOL ArcTo(LPCRECT lpRect, POINT ptStart, POINT ptEnd) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ArcTo(lpRect->left, lpRect->top, lpRect->right,
		lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
	}
	int GetArcDirection() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetArcDirection(m_hDC);
	}
	int SetArcDirection(int nArcDirection) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetArcDirection(m_hDC, nArcDirection);
	}

	BOOL PolyDraw(const POINT* lpPoints, const BYTE* lpTypes, int nCount) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PolyDraw(m_hDC, lpPoints, lpTypes, nCount);
	}
	BOOL PolylineTo(const POINT* lpPoints, int nCount) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PolylineTo(m_hDC, lpPoints, nCount);
	}
	BOOL PolyPolyline(const POINT* lpPoints,
		const DWORD* lpPolyPoints, int nCount) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PolyPolyline(m_hDC, lpPoints, lpPolyPoints, nCount);
	}

	BOOL PolyBezier(const POINT* lpPoints, int nCount) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PolyBezier(m_hDC, lpPoints, nCount);
	}
	BOOL PolyBezierTo(const POINT* lpPoints, int nCount) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PolyBezierTo(m_hDC, lpPoints, nCount);
	}
#endif //!UNDER_CE

// Simple Drawing Functions
	BOOL FillRect(LPCRECT lpRect, HBRUSH hBrush) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::FillRect(m_hDC, lpRect, hBrush);
	}
#ifndef UNDER_CE
	BOOL FrameRect(LPCRECT lpRect, HBRUSH hBrush) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::FrameRect(m_hDC, lpRect, hBrush);
	}
	BOOL InvertRect(LPCRECT lpRect) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::InvertRect(m_hDC, lpRect);
	}
	BOOL DrawIcon(int x, int y, HICON hIcon) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DrawIcon(m_hDC, x, y, hIcon);
	}
	BOOL DrawIcon(POINT point, HICON hIcon) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DrawIcon(m_hDC, point.x, point.y, hIcon);
	}

	BOOL DrawState(POINT pt, SIZE size, HBITMAP hBitmap, UINT nFlags, HBRUSH hBrush = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DrawState(m_hDC, hBrush, NULL, (LPARAM)hBitmap, 0, pt.x, pt.y, size.cx, size.cy, nFlags | DST_BITMAP);
	}
	BOOL DrawState(POINT pt, SIZE size, HICON hIcon, UINT nFlags, HBRUSH hBrush = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DrawState(m_hDC, hBrush, NULL, (LPARAM)hIcon, 0, pt.x, pt.y, size.cx, size.cy, nFlags | DST_ICON);
	}
	BOOL DrawState(POINT pt, SIZE size, LPCTSTR lpszText, UINT nFlags,
		BOOL bPrefixText = TRUE, int nTextLen = 0, HBRUSH hBrush = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DrawState(m_hDC, hBrush, NULL, (LPARAM)lpszText, (WPARAM)nTextLen, pt.x, pt.y, size.cx, size.cy, nFlags | (bPrefixText ? DST_PREFIXTEXT : DST_TEXT));
	}
	BOOL DrawState(POINT pt, SIZE size, DRAWSTATEPROC lpDrawProc,
		LPARAM lData, UINT nFlags, HBRUSH hBrush = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DrawState(m_hDC, hBrush, lpDrawProc, lData, 0, pt.x, pt.y, size.cx, size.cy, nFlags | DST_COMPLEX);
	}
#endif //!UNDER_CE

// Ellipse and Polygon Functions
#ifndef UNDER_CE
	BOOL Chord(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Chord(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4);
	}
	BOOL Chord(LPCRECT lpRect, POINT ptStart, POINT ptEnd) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Chord(m_hDC, lpRect->left, lpRect->top,
			lpRect->right, lpRect->bottom, ptStart.x, ptStart.y,
			ptEnd.x, ptEnd.y);
	}
#endif //!UNDER_CE
	void DrawFocusRect(LPCRECT lpRect) throw()
	{
		ATLASSERT(m_hDC != NULL);
		::DrawFocusRect(m_hDC, lpRect);
	}
	BOOL Ellipse(int x1, int y1, int x2, int y2) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Ellipse(m_hDC, x1, y1, x2, y2);
	}
	BOOL Ellipse(LPCRECT lpRect) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Ellipse(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	}
#ifndef UNDER_CE
	BOOL Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Pie(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4);
	}
	BOOL Pie(LPCRECT lpRect, POINT ptStart, POINT ptEnd) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Pie(m_hDC, lpRect->left, lpRect->top,
			lpRect->right, lpRect->bottom, ptStart.x, ptStart.y,
			ptEnd.x, ptEnd.y);
	}
#endif //!UNDER_CE
	BOOL Polygon(LPPOINT lpPoints, int nCount) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Polygon(m_hDC, lpPoints, nCount);
	}
#ifndef UNDER_CE
	BOOL PolyPolygon(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PolyPolygon(m_hDC, lpPoints, lpPolyCounts, nCount);
	}
#endif //!UNDER_CE
	BOOL Rectangle(int x1, int y1, int x2, int y2) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Rectangle(m_hDC, x1, y1, x2, y2);
	}
	BOOL Rectangle(LPCRECT lpRect) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Rectangle(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	}
	BOOL RoundRect(int x1, int y1, int x2, int y2, int x3, int y3) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::RoundRect(m_hDC, x1, y1, x2, y2, x3, y3);
	}
	BOOL RoundRect(LPCRECT lpRect, POINT point) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::RoundRect(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, point.x, point.y);
	}

// Bitmap Functions
	BOOL PatBlt(int x, int y, int nWidth, int nHeight, DWORD dwRop) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PatBlt(m_hDC, x, y, nWidth, nHeight, dwRop);
	}
	BOOL BitBlt(int x, int y, int nWidth, int nHeight, HDC hSrcDC,
		int xSrc, int ySrc, DWORD dwRop) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::BitBlt(m_hDC, x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, dwRop);
	}
	BOOL StretchBlt(int x, int y, int nWidth, int nHeight, HDC hSrcDC,
		int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwRop) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::StretchBlt(m_hDC, x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, dwRop);
	}
	COLORREF GetPixel(int x, int y) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetPixel(m_hDC, x, y);
	}
	COLORREF GetPixel(POINT point) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetPixel(m_hDC, point.x, point.y);
	}
	COLORREF SetPixel(int x, int y, COLORREF crColor) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetPixel(m_hDC, x, y, crColor);
	}
	COLORREF SetPixel(POINT point, COLORREF crColor) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetPixel(m_hDC, point.x, point.y, crColor);
	}
#ifndef UNDER_CE
	BOOL FloodFill(int x, int y, COLORREF crColor) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::FloodFill(m_hDC, x, y, crColor);
	}
	BOOL ExtFloodFill(int x, int y, COLORREF crColor, UINT nFillType) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ExtFloodFill(m_hDC, x, y, crColor, nFillType);
	}
	BOOL GradientFill(const PTRIVERTEX pVertices, DWORD nVertices, 
		void* pMeshElements, DWORD nMeshElements, DWORD dwMode) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GradientFill(m_hDC, pVertices, nVertices, pMeshElements, nMeshElements, dwMode);
	}
#endif //!UNDER_CE
	BOOL MaskBlt(int x, int y, int nWidth, int nHeight, HDC hSrcDC,
		int xSrc, int ySrc, HBITMAP hMaskBitmap, int xMask, int yMask,
		DWORD dwRop) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::MaskBlt(m_hDC, x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, hMaskBitmap, xMask, yMask, dwRop);
	}
#ifndef UNDER_CE
	BOOL PlgBlt(LPPOINT lpPoint, HDC hSrcDC, int xSrc, int ySrc,
		int nWidth, int nHeight, HBITMAP hMaskBitmap, int xMask, int yMask) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PlgBlt(m_hDC, lpPoint, hSrcDC, xSrc, ySrc, nWidth, nHeight, hMaskBitmap, xMask, yMask);
	}
	BOOL TransparentBlt(int x, int y, int nWidth, int nHeight, HDC hSrcDC, 
		int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, UINT crTransparent) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::TransparentBlt(m_hDC, x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc,
			nSrcWidth, nSrcHeight, crTransparent);
	}
	BOOL AlphaBlend(int x, int y, int nWidth, int nHeight, HDC hSrcDC, int xSrc, 
		int ySrc, int nSrcWidth, int nSrcHeight, BLENDFUNCTION bf) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::AlphaBlend(m_hDC, x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, 
			nSrcWidth, nSrcHeight, bf);
	}
	BOOL SetPixelV(int x, int y, COLORREF crColor) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetPixelV(m_hDC, x, y, crColor);
	}
	BOOL SetPixelV(POINT point, COLORREF crColor) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetPixelV(m_hDC, point.x, point.y, crColor);
	}
#endif //!UNDER_CE

// Text Functions
#ifndef UNDER_CE
	BOOL TextOut(int x, int y, LPCTSTR lpszString, int nCount = -1) throw()
	{
		ATLASSERT(m_hDC != NULL);
		if(nCount == -1)
			nCount = lstrlen(lpszString);
		return ::TextOut(m_hDC, x, y, lpszString, nCount);
	}
#endif //!UNDER_CE
	BOOL ExtTextOut(int x, int y, UINT nOptions, LPCRECT lpRect,
		LPCTSTR lpszString, UINT nCount = -1, LPINT lpDxWidths = NULL) throw()
	{
		ATLASSERT(m_hDC != NULL);
		if(nCount == -1)
			nCount = lstrlen(lpszString);
		return ::ExtTextOut(m_hDC, x, y, nOptions, lpRect, lpszString, nCount, lpDxWidths);
	}
#ifndef UNDER_CE
	SIZE TabbedTextOut(int x, int y, LPCTSTR lpszString, int nCount = -1,
		int nTabPositions = 0, LPINT lpnTabStopPositions = NULL, int nTabOrigin = 0) throw()
	{
		ATLASSERT(m_hDC != NULL);
		if(nCount == -1)
			nCount = lstrlen(lpszString);
		SIZE size;
		LONG lRes = ::TabbedTextOut(m_hDC, x, y, lpszString, nCount, nTabPositions, lpnTabStopPositions, nTabOrigin);
		size.cx = LOWORD(lRes);
		size.cy = HIWORD(lRes);
		return size;
	}
#endif //!UNDER_CE
	int DrawText(LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DrawText(m_hDC, lpszString, nCount, lpRect, nFormat);
	}
	BOOL GetTextExtent(LPCTSTR lpszString, int nCount, LPSIZE lpSize) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		if(nCount == -1)
			nCount = lstrlen(lpszString);
		return ::GetTextExtentPoint32(m_hDC, lpszString, nCount, lpSize);
	}
#ifndef UNDER_CE
	BOOL GetTabbedTextExtent(LPCTSTR lpszString, int nCount,
		int nTabPositions, LPINT lpnTabStopPositions) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		if(nCount == -1)
			nCount = lstrlen(lpszString);
		return ::GetTabbedTextExtent(m_hDC, lpszString, nCount, nTabPositions, lpnTabStopPositions);
	}
	BOOL GrayString(HBRUSH hBrush,
		BOOL (CALLBACK* lpfnOutput)(HDC, LPARAM, int), LPARAM lpData,
			int nCount, int x, int y, int nWidth, int nHeight) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GrayString(m_hDC, hBrush, (GRAYSTRINGPROC)lpfnOutput, lpData, nCount, x, y, nWidth, nHeight);
	}
	UINT GetTextAlign() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetTextAlign(m_hDC);
	}
	UINT SetTextAlign(UINT nFlags) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetTextAlign(m_hDC, nFlags);
	}
#endif //!UNDER_CE
	int GetTextFace(LPTSTR lpszFacename, int nCount) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetTextFace(m_hDC, nCount, lpszFacename);
	}
	int GetTextFaceLen() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetTextFace(m_hDC, 0, NULL);
	}
#ifndef _ATL_NO_COM
#ifdef _OLEAUTO_H_
	BOOL GetTextFace(BSTR& bstrFace) const throw()
	{
		USES_CONVERSION;
		ATLASSERT(m_hDC != NULL);
		ATLASSERT(bstrFace == NULL);

		int nLen = GetTextFaceLen();
		if(nLen == 0)
			return FALSE;

		LPTSTR lpszText = (LPTSTR)_alloca(nLen * sizeof(TCHAR));

		if(!GetTextFace(lpszText, nLen))
			return FALSE;

		bstrFace = ::SysAllocString(T2OLE(lpszText));
		return (bstrFace != NULL) ? TRUE : FALSE;
	}
#endif
#endif //!_ATL_NO_COM
	BOOL GetTextMetrics(LPTEXTMETRIC lpMetrics) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetTextMetrics(m_hDC, lpMetrics);
	}
#ifndef UNDER_CE
	int SetTextJustification(int nBreakExtra, int nBreakCount) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetTextJustification(m_hDC, nBreakExtra, nBreakCount);
	}
	int GetTextCharacterExtra() const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetTextCharacterExtra(m_hDC);
	}
	int SetTextCharacterExtra(int nCharExtra) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetTextCharacterExtra(m_hDC, nCharExtra);
	}
#endif //!UNDER_CE

// Advanced Drawing
	BOOL DrawEdge(LPRECT lpRect, UINT nEdge, UINT nFlags) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DrawEdge(m_hDC, lpRect, nEdge, nFlags);
	}
	BOOL DrawFrameControl(LPRECT lpRect, UINT nType, UINT nState) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DrawFrameControl(m_hDC, lpRect, nType, nState);
	}

// Scrolling Functions
	BOOL ScrollDC(int dx, int dy, LPCRECT lpRectScroll, LPCRECT lpRectClip,
		HRGN hRgnUpdate, LPRECT lpRectUpdate) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ScrollDC(m_hDC, dx, dy, lpRectScroll, lpRectClip, hRgnUpdate, lpRectUpdate);
	}

// Font Functions
#ifndef UNDER_CE
	BOOL GetCharWidth(UINT nFirstChar, UINT nLastChar, LPINT lpBuffer) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetCharWidth(m_hDC, nFirstChar, nLastChar, lpBuffer);
	}
	DWORD SetMapperFlags(DWORD dwFlag) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetMapperFlags(m_hDC, dwFlag);
	}
	BOOL GetAspectRatioFilter(LPSIZE lpSize) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetAspectRatioFilterEx(m_hDC, lpSize);
	}

	BOOL GetCharABCWidths(UINT nFirstChar, UINT nLastChar, LPABC lpabc) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetCharABCWidths(m_hDC, nFirstChar, nLastChar, lpabc);
	}
	DWORD GetFontData(DWORD dwTable, DWORD dwOffset, LPVOID lpData, DWORD cbData) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetFontData(m_hDC, dwTable, dwOffset, lpData, cbData);
	}
	int GetKerningPairs(int nPairs, LPKERNINGPAIR lpkrnpair) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetKerningPairs(m_hDC, nPairs, lpkrnpair);
	}
	UINT GetOutlineTextMetrics(UINT cbData, LPOUTLINETEXTMETRIC lpotm) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetOutlineTextMetrics(m_hDC, cbData, lpotm);
	}
	DWORD GetGlyphOutline(UINT nChar, UINT nFormat, LPGLYPHMETRICS lpgm,
		DWORD cbBuffer, LPVOID lpBuffer, const MAT2* lpmat2) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetGlyphOutline(m_hDC, nChar, nFormat, lpgm, cbBuffer, lpBuffer, lpmat2);
	}

	BOOL GetCharABCWidths(UINT nFirstChar, UINT nLastChar,
		LPABCFLOAT lpABCF) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetCharABCWidthsFloat(m_hDC, nFirstChar, nLastChar, lpABCF);
	}
	BOOL GetCharWidth(UINT nFirstChar, UINT nLastChar,
		float* lpFloatBuffer) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetCharWidthFloat(m_hDC, nFirstChar, nLastChar, lpFloatBuffer);
	}
#endif //!UNDER_CE

// Printer/Device Escape Functions
#ifndef UNDER_CE
	int Escape(int nEscape, int nCount, LPCSTR lpszInData, LPVOID lpOutData) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::Escape(m_hDC, nEscape, nCount, lpszInData, lpOutData);
	}
	int Escape(int nEscape, int nInputSize, LPCSTR lpszInputData,
		int nOutputSize, LPSTR lpszOutputData) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::ExtEscape(m_hDC, nEscape, nInputSize, lpszInputData, nOutputSize, lpszOutputData);
	}
	int DrawEscape(int nEscape, int nInputSize, LPCSTR lpszInputData) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::DrawEscape(m_hDC, nEscape, nInputSize, lpszInputData);
	}

	// Escape helpers
	int StartDoc(LPCTSTR lpszDocName) throw()  // old Win3.0 version
	{
		DOCINFO di;
		memset(&di, 0, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = lpszDocName;
		return StartDoc(&di);
	}
#endif //!UNDER_CE

#ifndef UNDER_CE
	int StartDoc(LPDOCINFO lpDocInfo) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::StartDoc(m_hDC, lpDocInfo);
	}
	int StartPage() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::StartPage(m_hDC);
	}
	int EndPage() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::EndPage(m_hDC);
	}
	int SetAbortProc(BOOL (CALLBACK* lpfn)(HDC, int)) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetAbortProc(m_hDC, (ABORTPROC)lpfn);
	}
	int AbortDoc() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::AbortDoc(m_hDC);
	}
	int EndDoc() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::EndDoc(m_hDC);
	}
#endif // UNDER_CE

// MetaFile Functions
#ifndef UNDER_CE
	BOOL PlayMetaFile(HMETAFILE hMF) throw()
	{
		ATLASSERT(m_hDC != NULL);
		if(::GetDeviceCaps(m_hDC, TECHNOLOGY) == DT_METAFILE)
		{
			// playing metafile in metafile, just use core windows API
			return ::PlayMetaFile(m_hDC, hMF);
		}

		// for special playback, lParam == pDC
		return ::EnumMetaFile(m_hDC, hMF, EnumMetaFileProc, (LPARAM)this);
	}
	BOOL PlayMetaFile(HENHMETAFILE hEnhMetaFile, LPCRECT lpBounds) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::PlayEnhMetaFile(m_hDC, hEnhMetaFile, lpBounds);
	}
	BOOL AddMetaFileComment(UINT nDataSize, const BYTE* pCommentData) throw()  // can be used for enhanced metafiles only
	{
		ATLASSERT(m_hDC != NULL);
		return ::GdiComment(m_hDC, nDataSize, pCommentData);
	}

	// Special handling for metafile playback
	static int CALLBACK EnumMetaFileProc(HDC hDC, HANDLETABLE* pHandleTable, METARECORD* pMetaRec, int nHandles, LPARAM lParam) throw()
	{
		CWinDC* pDC = (CWinDC*)lParam;

		switch (pMetaRec->rdFunction)
		{
		case META_SETMAPMODE:
			pDC->SetMapMode((int)(short)pMetaRec->rdParm[0]);
			break;
		case META_SETWINDOWEXT:
			pDC->SetWindowExt((int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
			break;
		case META_SETWINDOWORG:
			pDC->SetWindowOrg((int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
			break;
		case META_SETVIEWPORTEXT:
			pDC->SetViewportExt((int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
			break;
		case META_SETVIEWPORTORG:
			pDC->SetViewportOrg((int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
			break;
		case META_SCALEWINDOWEXT:
			pDC->ScaleWindowExt((int)(short)pMetaRec->rdParm[3], (int)(short)pMetaRec->rdParm[2], 
				(int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
			break;
		case META_SCALEVIEWPORTEXT:
			pDC->ScaleViewportExt((int)(short)pMetaRec->rdParm[3], (int)(short)pMetaRec->rdParm[2],
				(int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
			break;
		case META_OFFSETVIEWPORTORG:
			pDC->OffsetViewportOrg((int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
			break;
		case META_SAVEDC:
			pDC->SaveDC();
			break;
		case META_RESTOREDC:
			pDC->RestoreDC((int)(short)pMetaRec->rdParm[0]);
			break;
		case META_SETBKCOLOR:
			pDC->SetBkColor(*(UNALIGNED COLORREF*)&pMetaRec->rdParm[0]);
			break;
		case META_SETTEXTCOLOR:
			pDC->SetTextColor(*(UNALIGNED COLORREF*)&pMetaRec->rdParm[0]);
			break;

		// need to watch out for SelectObject(HFONT), for custom font mapping
		case META_SELECTOBJECT:
			{
				HGDIOBJ hObject = pHandleTable->objectHandle[pMetaRec->rdParm[0]];
				UINT nObjType = ::GetObjectType(hObject);
				if(nObjType == 0)
				{
					// object type is unknown, determine if it is a font
					HFONT hStockFont = (HFONT)::GetStockObject(SYSTEM_FONT);
/**/					HFONT hFontOld = (HFONT)::SelectObject(pDC->m_hDC, hStockFont);
/**/					HGDIOBJ hObjOld = ::SelectObject(pDC->m_hDC, hObject);
					if(hObjOld == hStockFont)
					{
						// got the stock object back, so must be selecting a font
						pDC->SelectFont((HFONT)hObject);
						break;  // don't play the default record
					}
					else
					{
						// didn't get the stock object back, so restore everything
/**/						::SelectObject(pDC->m_hDC, hFontOld);
/**/						::SelectObject(pDC->m_hDC, hObjOld);
					}
					// and fall through to PlayMetaFileRecord...
				}
				else if(nObjType == OBJ_FONT)
				{
					// play back as CWinDC::SelectFont(HFONT)
					pDC->SelectFont((HFONT)hObject);
					break;  // don't play the default record
				}
			}
			// fall through...

		default:
			::PlayMetaFileRecord(hDC, pHandleTable, pMetaRec, nHandles);
			break;
		}

		return 1;
	}
#endif //!UNDER_CE

// Path Functions
#ifndef UNDER_CE
	BOOL AbortPath() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::AbortPath(m_hDC);
	}
	BOOL BeginPath() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::BeginPath(m_hDC);
	}
	BOOL CloseFigure() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::CloseFigure(m_hDC);
	}
	BOOL EndPath() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::EndPath(m_hDC);
	}
	BOOL FillPath() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::FillPath(m_hDC);
	}
	BOOL FlattenPath() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::FlattenPath(m_hDC);
	}
	BOOL StrokeAndFillPath() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::StrokeAndFillPath(m_hDC);
	}
	BOOL StrokePath() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::StrokePath(m_hDC);
	}
	BOOL WidenPath() throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::WidenPath(m_hDC);
	}
	BOOL GetMiterLimit(PFLOAT pfMiterLimit) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetMiterLimit(m_hDC, pfMiterLimit);
	}
	BOOL SetMiterLimit(float fMiterLimit) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SetMiterLimit(m_hDC, fMiterLimit, NULL);
	}
	int GetPath(LPPOINT lpPoints, LPBYTE lpTypes, int nCount) const throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::GetPath(m_hDC, lpPoints, lpTypes, nCount);
	}
	BOOL SelectClipPath(int nMode) throw()
	{
		ATLASSERT(m_hDC != NULL);
		return ::SelectClipPath(m_hDC, nMode);
	}
#endif //!UNDER_CE

// Misc Helper Functions
	static CWinManagedBrush PASCAL GetHalftoneBrush() throw()
	{
		HBRUSH halftoneBrush = NULL;
		WORD grayPattern[8];
		for(int i = 0; i < 8; i++)
			grayPattern[i] = (WORD)(0x5555 << (i & 1));
		HBITMAP grayBitmap = CreateBitmap(8, 8, 1, 1, &grayPattern);
		if(grayBitmap != NULL)
		{
			halftoneBrush = ::CreatePatternBrush(grayBitmap);
			DeleteObject(grayBitmap);
		}
		return CWinManagedBrush(halftoneBrush);
	}
	void DrawDragRect(LPCRECT lpRect, SIZE size, LPCRECT lpRectLast, SIZE sizeLast, HBRUSH hBrush = NULL, HBRUSH hBrushLast = NULL) throw()
	{
		// first, determine the update region and select it
		HRGN hRgnNew;
		HRGN hRgnOutside, hRgnInside;
		hRgnOutside = ::CreateRectRgnIndirect(lpRect);
		RECT rect = *lpRect;
		::InflateRect(&rect, -size.cx, -size.cy);
		::IntersectRect(&rect, &rect, lpRect);
		hRgnInside = ::CreateRectRgnIndirect(&rect);
		hRgnNew = ::CreateRectRgn(0, 0, 0, 0);
		::CombineRgn(hRgnNew, hRgnOutside, hRgnInside, RGN_XOR);

		HBRUSH hBrushOld = NULL;
		if(hBrush == NULL)
			hBrush = CWinDC::GetHalftoneBrush();
		if(hBrushLast == NULL)
			hBrushLast = hBrush;

		HRGN hRgnLast, hRgnUpdate;
		if(lpRectLast != NULL)
		{
			// find difference between new region and old region
			hRgnLast = ::CreateRectRgn(0, 0, 0, 0);
			::SetRectRgn(hRgnOutside, lpRectLast->left, lpRectLast->top, lpRectLast->right, lpRectLast->bottom);
			rect = *lpRectLast;
			::InflateRect(&rect, -sizeLast.cx, -sizeLast.cy);
			::IntersectRect(&rect, &rect, lpRectLast);
			::SetRectRgn(hRgnInside, rect.left, rect.top, rect.right, rect.bottom);
			::CombineRgn(hRgnLast, hRgnOutside, hRgnInside, RGN_XOR);

			// only diff them if brushes are the same
			if(hBrush == hBrushLast)
			{
				hRgnUpdate = ::CreateRectRgn(0, 0, 0, 0);
				::CombineRgn(hRgnUpdate, hRgnLast, hRgnNew, RGN_XOR);
			}
		}
		if(hBrush != hBrushLast && lpRectLast != NULL)
		{
			// brushes are different -- erase old region first
			SelectClipRgn(hRgnLast);
			GetClipBox(&rect);
			hBrushOld = SelectBrush(hBrushLast);
			PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
			SelectBrush(hBrushOld);
			hBrushOld = NULL;
		}

		// draw into the update/new region
		SelectClipRgn(hRgnUpdate != NULL ? hRgnUpdate : hRgnNew);
		GetClipBox(&rect);
		hBrushOld = SelectBrush(hBrush);
		PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);

		// cleanup DC
		if(hBrushOld != NULL)
			SelectBrush(hBrushOld);
		SelectClipRgn(NULL);
	}
	void FillSolidRect(LPCRECT lpRect, COLORREF clr) throw()
	{
		ATLASSERT(m_hDC != NULL);

		::SetBkColor(m_hDC, clr);
		::ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	}
	void FillSolidRect(int x, int y, int cx, int cy, COLORREF clr) throw()
	{
		ATLASSERT(m_hDC != NULL);

		::SetBkColor(m_hDC, clr);
		RECT rect = { x, y, x + cx, y + cy };
		::ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	}
	void Draw3dRect(LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight) throw()
	{
		Draw3dRect(lpRect->left, lpRect->top, lpRect->right - lpRect->left,
			lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
	}
	void Draw3dRect(int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight) throw()
	{
		FillSolidRect(x, y, cx - 1, 1, clrTopLeft);
		FillSolidRect(x, y, 1, cy - 1, clrTopLeft);
		FillSolidRect(x + cx, y, -1, cy, clrBottomRight);
		FillSolidRect(x, y + cy, cx, -1, clrBottomRight);
	}
};


class CWinManagedDC : public CWinDC
{
public:
	CWinManagedDC() throw() :
		CWinDC(NULL),
		m_bAutoRestore(TRUE),
		m_hOriginalPen(NULL),
		m_hOriginalBrush(NULL),
		m_hOriginalFont(NULL),
		m_hOriginalBitmap(NULL)
	{
	}
	explicit CWinManagedDC(HDC hDC, BOOL bAutoRestore = TRUE) throw() : 
		CWinDC(hDC), 
		m_bAutoRestore(bAutoRestore),
		m_hOriginalPen(NULL), 
		m_hOriginalBrush(NULL), 
		m_hOriginalFont(NULL), 
		m_hOriginalBitmap(NULL)
	{
	}
	~CWinManagedDC() throw()
	{
		if(m_hDC != NULL)
		{
			if(m_bAutoRestore)
				RestoreAllObjects();
			::DeleteDC(Detach());
		}
	}

	void RestoreAllObjects() throw()
	{
		if(m_hOriginalPen != NULL)
		{
#ifndef UNDER_CE
			ATLASSERT(::GetObjectType(m_hOriginalPen) == OBJ_PEN || ::GetObjectType(m_hOriginalPen) == OBJ_EXTPEN);
#else // CE specific
			ATLASSERT(::GetObjectType(m_hOriginalPen) == OBJ_PEN);
#endif //!UNDER_CE
			::SelectObject(m_hDC, m_hOriginalPen);
			m_hOriginalPen = NULL;
		}
		if(m_hOriginalBrush != NULL)
		{
			ATLASSERT(::GetObjectType(m_hOriginalBrush) == OBJ_BRUSH);
			::SelectObject(m_hDC, m_hOriginalBrush);
			m_hOriginalBrush = NULL;
		}
//		if(m_hOriginalPalette != NULL)
//		{
//			ATLASSERT(::GetObjectType(m_hOriginalPalette) == OBJ_PAL);
//			::SelectPalette(m_hDC, m_hOriginalPalette, FALSE);
//			m_hOriginalPalette = NULL;
//		}
		if(m_hOriginalFont != NULL)
		{
			ATLASSERT(::GetObjectType(m_hOriginalFont) == OBJ_FONT);
			::SelectObject(m_hDC, m_hOriginalFont);
			m_hOriginalFont = NULL;
		}
		if(m_hOriginalBitmap != NULL)
		{
			ATLASSERT(::GetObjectType(m_hOriginalBitmap) == OBJ_BITMAP);
			::SelectObject(m_hDC, m_hOriginalBitmap);
			m_hOriginalBitmap = NULL;
		}
	}
	BOOL DeleteDC() throw()
	{
		if(m_hDC == NULL)
			return FALSE;

		if(m_bAutoRestore)
			RestoreAllObjects();

		return ::DeleteDC(Detach());
	}
// Type-safe selection helpers
	HPEN SelectPen(HPEN hPen) throw()
	{
		ATLASSERT(m_hDC != NULL);
#ifndef UNDER_CE
		ATLASSERT(::GetObjectType(hPen) == OBJ_PEN || ::GetObjectType(hPen) == OBJ_EXTPEN);
#else // CE specific
		ATLASSERT(::GetObjectType(hPen) == OBJ_PEN);
#endif //!UNDER_CE
		HPEN hOldPen = (HPEN)::SelectObject(m_hDC, hPen);
		if(m_hOriginalPen == NULL)
			m_hOriginalPen = hOldPen;
		return hOldPen;
	}
	HBRUSH SelectBrush(HBRUSH hBrush) throw()
	{
		ATLASSERT(m_hDC != NULL);
		ATLASSERT(::GetObjectType(hBrush) == OBJ_BRUSH);
		HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDC, hBrush);
		if(m_hOriginalBrush == NULL)
			m_hOriginalBrush = hOldBrush;
		return hOldBrush;
	}
	HFONT SelectFont(HFONT hFont) throw()
	{
		ATLASSERT(m_hDC != NULL);
		ATLASSERT(::GetObjectType(hFont) == OBJ_FONT);
		HFONT hOldFont = (HFONT)::SelectObject(m_hDC, hFont);
		if(m_hOriginalFont == NULL)
			m_hOriginalFont = hOldFont;
		return hOldFont;
	}
	HBITMAP SelectBitmap(HBITMAP hBitmap) throw()
	{
		ATLASSERT(m_hDC != NULL);
		ATLASSERT(::GetObjectType(hBitmap) == OBJ_BITMAP);
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(m_hDC, hBitmap);
		if(m_hOriginalBitmap == NULL)
			m_hOriginalBitmap = hOldBitmap;
		return hOldBitmap;
	}
	HGDIOBJ SelectStockObject(int nIndex) throw()
	{
		ATLASSERT(m_hDC != NULL);
		HGDIOBJ hObject = ::GetStockObject(nIndex);
		ATLASSERT(hObject != NULL);
		switch(::GetObjectType(hObject))
		{
		case OBJ_PEN:
#ifndef UNDER_CE
/*?*/		case OBJ_EXTPEN:
#endif //!UNDER_CE
			return SelectPen((HPEN)hObject);
		case OBJ_BRUSH:
			return SelectBrush((HBRUSH)hObject);
		case OBJ_FONT:
			return SelectFont((HFONT)hObject);
		default:
			return NULL;
		}
	}

public:
	BOOL m_bAutoRestore;

	HPEN m_hOriginalPen;
	HBRUSH m_hOriginalBrush;
//	HPALETTE m_hOriginalPalette;
	HFONT m_hOriginalFont;
	HBITMAP m_hOriginalBitmap;

private:
	CWinManagedDC(const CWinManagedDC& dc) throw();
	CWinManagedDC& operator=(const CWinManagedDC& dc) throw();
};

/////////////////////////////////////////////////////////////////////////////
// CWinDC Helpers

class CWinPaintDC : 
	public CWinManagedDC
{
public:
	HWND m_hWnd;
	PAINTSTRUCT m_ps;

	CWinPaintDC(HWND hWnd, BOOL bAutoRestore = TRUE) throw() : 
		CWinManagedDC(NULL, bAutoRestore)
	{
		ATLASSERT(::IsWindow(hWnd));
		m_hWnd = hWnd;
		m_hDC = ::BeginPaint(hWnd, &m_ps);
	}
	~CWinPaintDC() throw()
	{
		ATLASSERT(m_hDC != NULL);
		ATLASSERT(::IsWindow(m_hWnd));

		if(m_bAutoRestore)
			RestoreAllObjects();

		::EndPaint(m_hWnd, &m_ps);
		Detach();
	}

private:
	CWinPaintDC(const CWinPaintDC& dc) throw();
	CWinPaintDC& operator=(const CWinPaintDC& dc) throw();
};

class CWinClientDC : 
	public CWinManagedDC
{
public:
	HWND m_hWnd;

	CWinClientDC(HWND hWnd, BOOL bAutoRestore = TRUE) throw() : 
		CWinManagedDC(NULL, bAutoRestore)
	{
		ATLASSERT(hWnd == NULL || ::IsWindow(hWnd));
		m_hWnd = hWnd;
		m_hDC = ::GetDC(hWnd);
	}
	~CWinClientDC() throw()
	{
		ATLASSERT(m_hDC != NULL);

		if(m_bAutoRestore)
			RestoreAllObjects();

		::ReleaseDC(m_hWnd, Detach());
	}

private:
	CWinClientDC(const CWinClientDC& dc) throw();
	CWinClientDC& operator=(const CWinClientDC& dc) throw();
};

class CWinWindowDC : 
	public CWinManagedDC
{
public:
	HWND m_hWnd;

	CWinWindowDC(HWND hWnd, BOOL bAutoRestore = TRUE) throw() : 
		CWinManagedDC(NULL, bAutoRestore)
	{
		ATLASSERT(hWnd == NULL || ::IsWindow(hWnd));
		m_hWnd = hWnd;
		m_hDC = ::GetWindowDC(hWnd);
	}
	~CWinWindowDC() throw()
	{
		ATLASSERT(m_hDC != NULL);

		if(m_bAutoRestore)
			RestoreAllObjects();

		::ReleaseDC(m_hWnd, Detach());
	}

private:
	CWinWindowDC(const CWinWindowDC& dc) throw();
	CWinWindowDC& operator=(const CWinWindowDC& dc) throw();
};

//Enhanced metafile support
/////////////////////////////////////////////////////////////////////////////
class CEnhMetaFileInfo
{
public:
	CEnhMetaFileInfo(HENHMETAFILE hEMF)
	{
		m_pBits = NULL;
		m_pDesc = NULL;
		m_hEMF = hEMF;
	}

	~CEnhMetaFileInfo()
	{
		delete [] m_pBits;
		delete [] m_pDesc;
	}

	BYTE* GetEnhMetaFileBits()
	{
		ATLASSERT(m_hEMF != NULL);
		UINT nBytes = ::GetEnhMetaFileBits(m_hEMF, 0, NULL);
		delete [] m_pBits;
		m_pBits = NULL;
		ATLTRY(m_pBits = new BYTE[nBytes]);
		if (m_pBits != NULL)
			::GetEnhMetaFileBits(m_hEMF, nBytes, m_pBits);
		return m_pBits;
	}
	LPTSTR GetEnhMetaFileDescription()
	{
		ATLASSERT(m_hEMF != NULL);
		UINT nLen = ::GetEnhMetaFileDescription(m_hEMF, 0, NULL);
		delete [] m_pDesc;
		m_pDesc = NULL;
		ATLTRY(m_pDesc = new TCHAR[nLen]);
		if (m_pDesc != NULL)
			nLen = ::GetEnhMetaFileDescription(m_hEMF, nLen, m_pDesc);
		return m_pDesc;
	}
	ENHMETAHEADER* GetEnhMetaFileHeader()
	{
		ATLASSERT(m_hEMF != NULL);
		memset(&m_header, 0, sizeof(m_header));
		m_header.iType = EMR_HEADER;
		m_header.nSize = sizeof(ENHMETAHEADER);
		UINT n = ::GetEnhMetaFileHeader(m_hEMF, sizeof(ENHMETAHEADER), &m_header);
		return (n != 0) ? &m_header : NULL;
	}
	PIXELFORMATDESCRIPTOR* GetEnhMetaFilePixelFormat()
	{
		ATLASSERT(m_hEMF != NULL);
		memset(&m_pfd, 0, sizeof(m_pfd));
		UINT n = ::GetEnhMetaFilePixelFormat(m_hEMF, sizeof(m_pfd), &m_pfd);
		return (n != 0) ? &m_pfd : NULL;
	}
private:
	HENHMETAFILE m_hEMF;
	BYTE* m_pBits;
	TCHAR* m_pDesc;
	ENHMETAHEADER m_header;
	PIXELFORMATDESCRIPTOR m_pfd;
};

template <bool t_bManaged> class CEnhMetaFileT;
typedef CEnhMetaFileT<true>		CEnhMetaFile;
typedef CEnhMetaFileT<false>	CEnhMetaFileTemp;

template <bool t_bManaged>
class CEnhMetaFileT
{
public:
	HENHMETAFILE m_hEMF;

	CEnhMetaFileT(HENHMETAFILE hEMF = NULL) : m_hEMF(hEMF)
	{
	}

	~CEnhMetaFileT()
	{
		if(t_bManaged && m_hEMF != NULL)
			DeleteObject();
	}

	CEnhMetaFileT<t_bManaged>& operator=(HENHMETAFILE hEMF)
	{
		Attach(hEMF);
		return *this;
	}

	void Attach(HENHMETAFILE hEMF)
	{
		if(t_bManaged && m_hEMF != NULL)
			DeleteObject();
		m_hEMF = hEMF;
	}
	HENHMETAFILE Detach()
	{
		HENHMETAFILE hEMF = m_hEMF;
		m_hEMF = NULL;
		return hEMF;
	}

	operator HENHMETAFILE() const { return m_hEMF; }

	BOOL DeleteObject()
	{
		ATLASSERT(m_hEMF != NULL);
		BOOL bRet = ::DeleteEnhMetaFile(m_hEMF);
		m_hEMF = NULL;
		return bRet;
	}

	UINT GetEnhMetaFileBits(UINT cbBuffer, LPBYTE lpbBuffer)
	{
		ATLASSERT(m_hEMF != NULL);
		return ::GetEnhMetaFileBits(m_hEMF, cbBuffer, lpbBuffer);
	}
	UINT GetEnhMetaFileDescription(UINT cchBuffer, LPTSTR lpszDescription)
	{
		ATLASSERT(m_hEMF != NULL);
		return GetEnhMetaFileDescription(m_hEMF, cchBuffer, lpszDescription);
	}
	UINT GetEnhMetaFileHeader(UINT cbBuffer, LPENHMETAHEADER lpemh)
	{
		ATLASSERT(m_hEMF != NULL);
		lpemh->iType = EMR_HEADER;
		lpemh->nSize = sizeof(ENHMETAHEADER);
		return ::GetEnhMetaFileHeader(m_hEMF, sizeof(ENHMETAHEADER), &m_header);
	}
	UINT GetEnhMetaFilePaletteEntries(UINT cEntries, LPPALETTEENTRY lppe)
	{
		ATLASSERT(m_hEMF != NULL);
		return ::GetEnhMetaFilePaletteEntries(cEntries, lppe);
	}
	UINT GetEnhMetaFilePixelFormat(DWORD cbBuffer, PIXELFORMATDESCRIPTOR* ppfd)
	{
		ATLASSERT(m_hEMF != NULL);
		return GetEnhMetaFilePixelFormat(cbBuffer, ppfd);
	}
};

class CWinEnhMetaFileDC : 
	public CWinManagedDC
{
public:
	CWinEnhMetaFileDC(bool bAutoRestore = true) throw() : 
		CWinManagedDC(NULL, bAutoRestore)
	{
	}
	CWinEnhMetaFileDC(HDC hdc, LPCRECT lpRect, bool bAutoRestore = true) throw() : 
		CWinManagedDC(NULL, bAutoRestore)
	{
		Create(hdc, NULL, lpRect, NULL);
	}
	CWinEnhMetaFileDC(HDC hdcRef, LPCTSTR lpFilename, LPCRECT lpRect, 
		LPCTSTR lpDescription, bool bAutoRestore = true) throw() : 
		CWinManagedDC(NULL, bAutoRestore)
	{
		Create(hdcRef, lpFilename, lpRect, lpDescription);
	}

	void Create(HDC hdcRef, LPCTSTR lpFilename, CONST RECT *lpRect, LPCTSTR lpDescription) throw()
	{
		m_hDC = CreateEnhMetaFile(hdcRef, lpFilename, lpRect, lpDescription);
	}
	HENHMETAFILE Close() throw()
	{
		HENHMETAFILE hEMF = NULL;
		if(m_bAutoRestore)
			RestoreAllObjects();
		if (m_hDC != NULL)
		{
			hEMF = ::CloseEnhMetaFile(m_hDC);
			m_hDC = NULL;
		}
		return hEMF;
	}
	~CWinEnhMetaFileDC() throw()
	{
		HENHMETAFILE hEMF = Close();
		if (hEMF != NULL)
			DeleteEnhMetaFile(hEMF);
	}

private:
	CWinEnhMetaFileDC(const CWinEnhMetaFileDC& dc) throw();
	CWinEnhMetaFileDC& operator=(const CWinEnhMetaFileDC& dc) throw();
};

}; //namespace ATL

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning(pop)
#endif

#endif // __ATLGDI_H__
