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

#ifndef __ATLCTRLS_H__
#define __ATLCTRLS_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <atlwin.h>
#include <atlgdi.h>
#include <commctrl.h>

#ifndef UNDER_CE
#include <richedit.h>
#include <richole.h>
#endif //!UNDER_CE

// protect template members from windowsx.h macros
#ifdef _INC_WINDOWSX
#undef GetNextSibling
#undef GetPrevSibling
#endif //_INC_WINDOWSX


namespace ATL
{

/////////////////////////////////////////////////////////////////////////////
// Forward declarations

template <class Base> class CWinStaticT;
template <class Base> class CWinButtonT;
template <class Base> class CWinListBoxT;
template <class Base> class CWinComboBoxT;
template <class Base> class CWinEditT;
template <class Base> class CWinScrollBarT;
class CWinImageList;
template <class Base> class CWinListViewT;
template <class Base> class CWinTreeViewT;
class CWinTreeItem;
template <class Base> class CWinTreeViewExT;
template <class Base> class CWinHeaderT;
template <class Base> class CWinToolBarT;
template <class Base> class CWinStatusBarT;
template <class Base> class CWinTabT;
class CWinToolInfo;
template <class Base> class CWinToolTipT;
template <class Base> class CWinTrackBarT;
template <class Base> class CWinUpDownT;
template <class Base> class CWinProgressBarT;
template <class Base> class CWinHotKeyT;
template <class Base> class CWinAnimateT;
#ifndef UNDER_CE
template <class Base> class CWinRichEditT;
#endif //!UNDER_CE
template <class Base> class CWinDragListBoxT;
template <class T> class CWinDragListNotifyImpl;
template <class Base> class CWinReBarT;
template <class Base> class CWinComboBoxExT;
template <class Base> class CWinDateTimePickerT;
template <class Base> class CWinMonthCalendarT;
#if (_WIN32_IE >= 0x0400)
template <class Base> class CWinFlatScrollBarT;
template <class Base> class CWinIPAddressT;
template <class Base> class CWinPagerT;
#endif //(_WIN32_IE >= 0x0400)
template <class T> class CCustomDraw;
#ifdef UNDER_CE
template <class Base> class CCommandBarT;
template <class Base> class CCommandBandsT;
#endif //UNDER_CE

// --- Standard Windows controls ---

/////////////////////////////////////////////////////////////////////////////
// CWinStatic - client side for a Windows STATIC control

template <class Base>
class CWinStaticT : public Base
{
public:
// Constructors
	CWinStaticT(HWND hWnd = NULL) throw() : Base(hWnd) { }

	CWinStaticT< Base >& operator=(HWND hWnd) throw()
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL) throw()
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName() throw()
	{
		return _T("STATIC");
	}

	HICON SetIcon(HICON hIcon) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HICON)::SendMessage(m_hWnd, STM_SETICON, (WPARAM)hIcon, 0L);
	}
	HICON GetIcon() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HICON)::SendMessage(m_hWnd, STM_GETICON, 0, 0L);
	}

	HENHMETAFILE SetEnhMetaFile(HENHMETAFILE hMetaFile) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HENHMETAFILE)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_ENHMETAFILE, (LPARAM)hMetaFile);
	}
	HENHMETAFILE GetEnhMetaFile() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HENHMETAFILE)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_ENHMETAFILE, 0L);
	}
	CWinBitmap SetBitmap(HBITMAP hBitmap) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinBitmap((HBITMAP)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap));
	}
	CWinBitmap GetBitmap() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinBitmap((HBITMAP)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_BITMAP, 0L));
	}
	HCURSOR SetCursor(HCURSOR hCursor) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HCURSOR)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_CURSOR, (LPARAM)hCursor);
	}
	HCURSOR GetCursor() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HCURSOR)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_CURSOR, 0L);
	}
};

typedef CWinStaticT<CWindow>	CWinStatic;

/////////////////////////////////////////////////////////////////////////////
// CWinButton - client side for a Windows BUTTON control

template <class Base>
class CWinButtonT : public Base
{
public:
// Constructors
	CWinButtonT(HWND hWnd = NULL) throw() : Base(hWnd) { }

	CWinButtonT< Base >& operator=(HWND hWnd) throw()
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL) throw()
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName() throw()
	{
		return _T("BUTTON");
	}

	UINT GetState() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0L);
	}
	void SetState(BOOL bHighlight) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, BM_SETSTATE, bHighlight, 0L);
	}
	int GetCheck() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, BM_GETCHECK, 0, 0L);
	}
	void SetCheck(int nCheck) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, BM_SETCHECK, nCheck, 0L);
	}
	UINT GetButtonStyle() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::GetWindowLong(m_hWnd, GWL_STYLE) & 0xff;
	}
	void SetButtonStyle(UINT nStyle, BOOL bRedraw = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, BM_SETSTYLE, nStyle, (LPARAM)bRedraw);
	}

	HICON SetIcon(HICON hIcon) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HICON)::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
	}
	HICON GetIcon() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HICON)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_ICON, 0L);
	}
	CWinBitmap SetBitmap(HBITMAP hBitmap) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinBitmap((HBITMAP)::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap));
	}
	CWinBitmap GetBitmap() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinBitmap((HBITMAP)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_BITMAP, 0L));
	}
#ifndef UNDER_CE
	HCURSOR SetCursor(HCURSOR hCursor) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HCURSOR)::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_CURSOR, (LPARAM)hCursor);
	}
	HCURSOR GetCursor() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HCURSOR)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_CURSOR, 0L);
	}
#endif //!UNDER_CE
};

typedef CWinButtonT<CWindow>	CWinButton;

/////////////////////////////////////////////////////////////////////////////
// CWinListBox - client side for a Windows LISTBOX control

template <class Base>
class CWinListBoxT : public Base
{
public:
// Constructors
	CWinListBoxT(HWND hWnd = NULL) throw() : Base(hWnd) { }

	CWinListBoxT< Base >& operator=(HWND hWnd) throw()
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL) throw()
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName() throw()
	{
		return _T("LISTBOX");
	}

	// for entire listbox
	int GetCount() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETCOUNT, 0, 0L);
	}
	int GetHorizontalExtent() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETHORIZONTALEXTENT, 0, 0L);
	}
	void SetHorizontalExtent(int cxExtent) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, LB_SETHORIZONTALEXTENT, cxExtent, 0L);
	}
	int GetTopIndex() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETTOPINDEX, 0, 0L);
	}
	int SetTopIndex(int nIndex) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_SETTOPINDEX, nIndex, 0L);
	}
	LCID GetLocale() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LCID)::SendMessage(m_hWnd, LB_GETLOCALE, 0, 0L);
	}
	LCID SetLocale(LCID nNewLocale) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LCID)::SendMessage(m_hWnd, LB_SETLOCALE, (WPARAM)nNewLocale, 0L);
	}
	int InitStorage(int nItems, UINT nBytes) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_INITSTORAGE, (WPARAM)nItems, nBytes);
	}
	UINT ItemFromPoint(POINT pt, BOOL& bOutside) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		DWORD dw = (DWORD)::SendMessage(m_hWnd, LB_ITEMFROMPOINT, 0, MAKELPARAM(pt.x, pt.y));
		bOutside = !!HIWORD(dw);
		return LOWORD(dw);
	}

	// for single-selection listboxes
	int GetCurSel() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(!(GetStyle() & LBS_MULTIPLESEL));
		return (int)::SendMessage(m_hWnd, LB_GETCURSEL, 0, 0L);
	}
	int SetCurSel(int nSelect) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(!(GetStyle() & LBS_MULTIPLESEL));
		return (int)::SendMessage(m_hWnd, LB_SETCURSEL, nSelect, 0L);
	}

	// for multiple-selection listboxes
	int GetSel(int nIndex) const throw()           // also works for single-selection
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETSEL, nIndex, 0L);
	}
	int SetSel(int nIndex, BOOL bSelect = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LBS_MULTIPLESEL);
		return (int)::SendMessage(m_hWnd, LB_SETSEL, bSelect, nIndex);
	}
	int GetSelCount() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LBS_MULTIPLESEL);
		return (int)::SendMessage(m_hWnd, LB_GETSELCOUNT, 0, 0L);
	}
	int GetSelItems(int nMaxItems, LPINT rgIndex) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LBS_MULTIPLESEL);
		return (int)::SendMessage(m_hWnd, LB_GETSELITEMS, nMaxItems, (LPARAM)rgIndex);
	}
	void SetAnchorIndex(int nIndex) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LBS_MULTIPLESEL);
		::SendMessage(m_hWnd, LB_SETANCHORINDEX, nIndex, 0L);
	}
	int GetAnchorIndex() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LBS_MULTIPLESEL);
		return (int)::SendMessage(m_hWnd, LB_GETANCHORINDEX, 0, 0L);
	}

	// for listbox items
	DWORD_PTR GetItemData(int nIndex) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LB_GETITEMDATA, nIndex, 0L);
	}
	int SetItemData(int nIndex, DWORD_PTR dwItemData) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_SETITEMDATA, nIndex, (LPARAM)dwItemData);
	}
	void* GetItemDataPtr(int nIndex) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LPVOID)::SendMessage(m_hWnd, LB_GETITEMDATA, nIndex, 0L);
	}
	int SetItemDataPtr(int nIndex, void* pData) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItemData(nIndex, (DWORD_PTR)(LPVOID)pData);
	}
	int GetItemRect(int nIndex, LPRECT lpRect) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETITEMRECT, nIndex, (LPARAM)lpRect);
	}
	int GetText(int nIndex, LPTSTR lpszBuffer) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETTEXT, nIndex, (LPARAM)lpszBuffer);
	}
#ifndef _ATL_NO_COM
#ifdef _OLEAUTO_H_
	BOOL GetTextBSTR(int nIndex, BSTR& bstrText) const throw()
	{
		USES_CONVERSION;
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(bstrText == NULL);

		int nLen = GetTextLen(nIndex);
		if(nLen == LB_ERR)
			return FALSE;

		LPTSTR lpszText = (LPTSTR)_alloca((nLen + 1) * sizeof(TCHAR));

		if(GetText(nIndex, lpszText) == LB_ERR)
			return FALSE;

		bstrText = ::SysAllocString(T2OLE(lpszText));
		return (bstrText != NULL) ? TRUE : FALSE;
	}
#endif // _OLEAUTO_H_
#endif //!_ATL_NO_COM
	int GetTextLen(int nIndex) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETTEXTLEN, nIndex, 0L);
	}

	// Settable only attributes
	void SetColumnWidth(int cxWidth) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LB_SETCOLUMNWIDTH, cxWidth, 0L);
	}
	BOOL SetTabStops(int nTabStops, LPINT rgTabStops) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LB_SETTABSTOPS, nTabStops, (LPARAM)rgTabStops);
	}
	BOOL SetTabStops() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LB_SETTABSTOPS, 0, 0L);
	}
	BOOL SetTabStops(const int& cxEachStop) throw()    // takes an 'int'
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LB_SETTABSTOPS, 1, (LPARAM)(LPINT)&cxEachStop);
	}

	int SetItemHeight(int nIndex, UINT cyItemHeight) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0));
	}
	int GetItemHeight(int nIndex) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETITEMHEIGHT, nIndex, 0L);
	}
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind);
	}
	int GetCaretIndex() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETCARETINDEX, 0, 0);
	}
	int SetCaretIndex(int nIndex, BOOL bScroll = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_SETCARETINDEX, nIndex, MAKELONG(bScroll, 0));
	}

// Operations
	// manipulating listbox items
	int AddString(LPCTSTR lpszItem) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_ADDSTRING, 0, (LPARAM)lpszItem);
	}
	int DeleteString(UINT nIndex) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_DELETESTRING, nIndex, 0L);
	}
	int InsertString(int nIndex, LPCTSTR lpszItem) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_INSERTSTRING, nIndex, (LPARAM)lpszItem);
	}
	void ResetContent() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, LB_RESETCONTENT, 0, 0L);
	}
#ifndef UNDER_CE
	int Dir(UINT attr, LPCTSTR lpszWildCard) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_DIR, attr, (LPARAM)lpszWildCard);
	}
#endif // UNDER_CE

	// selection helpers
	int FindString(int nStartAfter, LPCTSTR lpszItem) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_FINDSTRING, nStartAfter, (LPARAM)lpszItem);
	}
	int SelectString(int nStartAfter, LPCTSTR lpszItem) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_SELECTSTRING, nStartAfter, (LPARAM)lpszItem);
	}
	int SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return bSelect ? (int)::SendMessage(m_hWnd, LB_SELITEMRANGEEX, nFirstItem, nLastItem) : (int)::SendMessage(m_hWnd, LB_SELITEMRANGEEX, nLastItem, nFirstItem);
	}
};

typedef CWinListBoxT<CWindow>	CWinListBox;

/////////////////////////////////////////////////////////////////////////////
// CWinComboBox - client side for a Windows COMBOBOX control

template <class Base>
class CWinComboBoxT : public Base
{
public:
// Constructors
	CWinComboBoxT(HWND hWnd = NULL) throw() : Base(hWnd) { }

	CWinComboBoxT< Base >& operator=(HWND hWnd) throw()
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL) throw()
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName() throw()
	{
		return _T("COMBOBOX");
	}

	// for entire combo box
	int GetCount() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETCOUNT, 0, 0L);
	}
	int GetCurSel() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETCURSEL, 0, 0L);
	}
	int SetCurSel(int nSelect) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETCURSEL, nSelect, 0L);
	}
	LCID GetLocale() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LCID)::SendMessage(m_hWnd, CB_GETLOCALE, 0, 0L);
	}
	LCID SetLocale(LCID nNewLocale) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LCID)::SendMessage(m_hWnd, CB_SETLOCALE, (WPARAM)nNewLocale, 0L);
	}
	int GetTopIndex() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETTOPINDEX, 0, 0L);
	}
	int SetTopIndex(int nIndex) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETTOPINDEX, nIndex, 0L);
	}
	int InitStorage(int nItems, UINT nBytes) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_INITSTORAGE, (WPARAM)nItems, nBytes);
	}
	void SetHorizontalExtent(UINT nExtent) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, CB_SETHORIZONTALEXTENT, nExtent, 0L);
	}
	UINT GetHorizontalExtent() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, CB_GETHORIZONTALEXTENT, 0, 0L);
	}
	int SetDroppedWidth(UINT nWidth) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETDROPPEDWIDTH, nWidth, 0L);
	}
	int GetDroppedWidth() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETDROPPEDWIDTH, 0, 0L);
	}

	// for edit control
	DWORD GetEditSel() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, CB_GETEDITSEL, 0, 0L);
	}
	BOOL LimitText(int nMaxChars) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CB_LIMITTEXT, nMaxChars, 0L);
	}
	BOOL SetEditSel(int nStartChar, int nEndChar) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CB_SETEDITSEL, 0, MAKELONG(nStartChar, nEndChar));
	}

	// for combobox item
	DWORD_PTR GetItemData(int nIndex) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, CB_GETITEMDATA, nIndex, 0L);
	}
	int SetItemData(int nIndex, DWORD_PTR dwItemData) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETITEMDATA, nIndex, (LPARAM)dwItemData);
	}
	void* GetItemDataPtr(int nIndex) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LPVOID)GetItemData(nIndex);
	}
	int SetItemDataPtr(int nIndex, void* pData) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItemData(nIndex, (DWORD_PTR)(LPVOID)pData);
	}
	int GetLBText(int nIndex, LPTSTR lpszText) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETLBTEXT, nIndex, (LPARAM)lpszText);
	}
#ifndef _ATL_NO_COM
	BOOL GetLBTextBSTR(int nIndex, BSTR& bstrText) const throw()
	{
		USES_CONVERSION;
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(bstrText == NULL);

		int nLen = GetLBTextLen(nIndex);
		if(nLen == CB_ERR)
			return FALSE;

		LPTSTR lpszText = (LPTSTR)_alloca((nLen + 1) * sizeof(TCHAR));

		if(GetLBText(nIndex, lpszText) == CB_ERR)
			return FALSE;

		bstrText = ::SysAllocString(T2OLE(lpszText));
		return (bstrText != NULL) ? TRUE : FALSE;
	}
#endif //!_ATL_NO_COM
	int GetLBTextLen(int nIndex) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETLBTEXTLEN, nIndex, 0L);
	}

	int SetItemHeight(int nIndex, UINT cyItemHeight) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0));
	}
	int GetItemHeight(int nIndex) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETITEMHEIGHT, nIndex, 0L);
	}
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind);
	}
	int SetExtendedUI(BOOL bExtended = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETEXTENDEDUI, bExtended, 0L);
	}
	BOOL GetExtendedUI() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CB_GETEXTENDEDUI, 0, 0L);
	}
	void GetDroppedControlRect(LPRECT lprect) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, CB_GETDROPPEDCONTROLRECT, 0, (DWORD)lprect);
	}
	BOOL GetDroppedState() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CB_GETDROPPEDSTATE, 0, 0L);
	}

// Operations
	// for drop-down combo boxes
	void ShowDropDown(BOOL bShowIt = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, CB_SHOWDROPDOWN, bShowIt, 0L);
	}

	// manipulating listbox items
	int AddString(LPCTSTR lpszString) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_ADDSTRING, 0, (LPARAM)lpszString);
	}
	int DeleteString(UINT nIndex) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_DELETESTRING, nIndex, 0L);
	}
	int InsertString(int nIndex, LPCTSTR lpszString) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_INSERTSTRING, nIndex, (LPARAM)lpszString);
	}
	void ResetContent() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, CB_RESETCONTENT, 0, 0L);
	}
	int Dir(UINT attr, LPCTSTR lpszWildCard) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_DIR, attr, (LPARAM)lpszWildCard);
	}

	// selection helpers
	int FindString(int nStartAfter, LPCTSTR lpszString) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_FINDSTRING, nStartAfter, (LPARAM)lpszString);
	}
	int SelectString(int nStartAfter, LPCTSTR lpszString) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SELECTSTRING, nStartAfter, (LPARAM)lpszString);
	}

	// Clipboard operations
	void Clear() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_CLEAR, 0, 0L);
	}
	void Copy() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_COPY, 0, 0L);
	}
	void Cut() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_CUT, 0, 0L);
	}
	void Paste() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_PASTE, 0, 0L);
	}
};

typedef CWinComboBoxT<CWindow>	CWinComboBox;

/////////////////////////////////////////////////////////////////////////////
// CWinEdit - client side for a Windows EDIT control

template <class Base>
class CWinEditT : public Base
{
public:
// Constructors
	CWinEditT(HWND hWnd = NULL) throw() : Base(hWnd) { }

	CWinEditT< Base >& operator=(HWND hWnd) throw()
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL) throw()
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName() throw()
	{
		return _T("EDIT");
	}

	BOOL CanUndo() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_CANUNDO, 0, 0L);
	}
	int GetLineCount() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0L);
	}
	BOOL GetModify() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_GETMODIFY, 0, 0L);
	}
	void SetModify(BOOL bModified = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETMODIFY, bModified, 0L);
	}
	void GetRect(LPRECT lpRect) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_GETRECT, 0, (LPARAM)lpRect);
	}
	DWORD GetSel() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, EM_GETSEL, 0, 0L);
	}
	void GetSel(int& nStartChar, int& nEndChar) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&nStartChar, (LPARAM)&nEndChar);
	}
	HLOCAL GetHandle() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HLOCAL)::SendMessage(m_hWnd, EM_GETHANDLE, 0, 0L);
	}
	void SetHandle(HLOCAL hBuffer) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETHANDLE, (WPARAM)hBuffer, 0L);
	}
	void SetMargins(UINT nLeft, UINT nRight) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELONG(nLeft, nRight));
	}
	DWORD GetMargins() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, EM_GETMARGINS, 0, 0L);
	}
	void SetLimitText(UINT nMax) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETLIMITTEXT, nMax, 0L);
	}
	UINT GetLimitText() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, EM_GETLIMITTEXT, 0, 0L);
	}
	POINT PosFromChar(UINT nChar) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		DWORD dwRet = (DWORD)::SendMessage(m_hWnd, EM_POSFROMCHAR, nChar, 0);
		POINT point = { LOWORD(dwRet), HIWORD(dwRet) };
		return point;
	}
	int CharFromPos(POINT pt, int* pLine = NULL) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		DWORD dwRet = (DWORD)::SendMessage(m_hWnd, EM_CHARFROMPOS, 0, MAKELPARAM(pt.x, pt.y));
		if(pLine != NULL)
			*pLine = (int)HIWORD(dwRet);
		return (int)LOWORD(dwRet);
	}

	// NOTE: first word in lpszBuffer must contain the size of the buffer!
	int GetLine(int nIndex, LPTSTR lpszBuffer) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
	}
	int GetLine(int nIndex, LPTSTR lpszBuffer, int nMaxLength) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		*(LPWORD)lpszBuffer = (WORD)nMaxLength;
		return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
	}

// Operations
	void EmptyUndoBuffer() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0L);
	}
	BOOL FmtLines(BOOL bAddEOL) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_FMTLINES, bAddEOL, 0L);
	}
	void LimitText(int nChars = 0) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_LIMITTEXT, nChars, 0L);
	}
	int LineFromChar(int nIndex = -1) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_LINEFROMCHAR, nIndex, 0L);
	}
	int LineIndex(int nLine = -1) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_LINEINDEX, nLine, 0L);
	}
	int LineLength(int nLine = -1) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_LINELENGTH, nLine, 0L);
	}
	void LineScroll(int nLines, int nChars = 0) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_LINESCROLL, nChars, nLines);
	}
	void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText);
	}
	void SetPasswordChar(TCHAR ch) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETPASSWORDCHAR, ch, 0L);
	}
	void SetRect(LPCRECT lpRect) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETRECT, 0, (LPARAM)lpRect);
	}
	void SetRectNP(LPCRECT lpRect) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETRECTNP, 0, (LPARAM)lpRect);
	}
	void SetSel(DWORD dwSelection, BOOL bNoScroll = FALSE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETSEL, LOWORD(dwSelection), HIWORD(dwSelection));
		if(!bNoScroll)
			::SendMessage(m_hWnd, EM_SCROLLCARET, 0, 0L);
	}
	void SetSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETSEL, nStartChar, nEndChar);
		if(!bNoScroll)
			::SendMessage(m_hWnd, EM_SCROLLCARET, 0, 0L);
	}
	BOOL SetTabStops(int nTabStops, LPINT rgTabStops) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_SETTABSTOPS, nTabStops, (LPARAM)rgTabStops);
	}
	BOOL SetTabStops() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, EM_SETTABSTOPS, 0, 0L);
	}
	BOOL SetTabStops(const int& cxEachStop) throw()    // takes an 'int'
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_SETTABSTOPS, 1, (LPARAM)(LPINT)&cxEachStop);
	}

	// Additional operations
	void ScrollCaret() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SCROLLCARET, 0, 0L);
	}
	void InsertText(int nInsertAfterChar, LPCTSTR lpstrText, BOOL bNoScroll = FALSE, BOOL bCanUndo = FALSE) throw()
	{
		SetSel(nInsertAfterChar, nInsertAfterChar, bNoScroll);
		ReplaceSel(lpstrText, bCanUndo);
	}
	void AppendText(LPCTSTR lpstrText, BOOL bNoScroll = FALSE, BOOL bCanUndo = FALSE) throw()
	{
		InsertText(GetWindowTextLength(), lpstrText, bNoScroll, bCanUndo);
	}

	// Clipboard operations
	BOOL Undo() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_UNDO, 0, 0L);
	}
	void Clear() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_CLEAR, 0, 0L);
	}
	void Copy() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_COPY, 0, 0L);
	}
	void Cut() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_CUT, 0, 0L);
	}
	void Paste() throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_PASTE, 0, 0L);
	}

	BOOL SetReadOnly(BOOL bReadOnly = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_SETREADONLY, bReadOnly, 0L);
	}
	int GetFirstVisibleLine() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0L);
	}
	TCHAR GetPasswordChar() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (TCHAR)::SendMessage(m_hWnd, EM_GETPASSWORDCHAR, 0, 0L);
	}
};

typedef CWinEditT<CWindow>		CWinEdit;

/////////////////////////////////////////////////////////////////////////////
// CWinScrollBar - client side for a Windows SCROLLBAR control

template <class Base>
class CWinScrollBarT : public Base
{
public:
// Constructors
	CWinScrollBarT(HWND hWnd = NULL) throw() : Base(hWnd) { }

	CWinScrollBarT< Base >& operator=(HWND hWnd) throw()
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL) throw()
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName() throw()
	{
		return _T("SCROLLBAR");
	}

	int GetScrollPos() const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::GetScrollPos(m_hWnd, SB_CTL);
	}
	int SetScrollPos(int nPos, BOOL bRedraw = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SetScrollPos(m_hWnd, SB_CTL, nPos, bRedraw);
	}
	void GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::GetScrollRange(m_hWnd, SB_CTL, lpMinPos, lpMaxPos);
	}
	void SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SetScrollRange(m_hWnd, SB_CTL, nMinPos, nMaxPos, bRedraw);
	}
	void ShowScrollBar(BOOL bShow = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::ShowScrollBar(m_hWnd, SB_CTL, bShow);
	}

	BOOL EnableScrollBar(UINT nArrowFlags = ESB_ENABLE_BOTH) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::EnableScrollBar(m_hWnd, SB_CTL, nArrowFlags);
	}

	int SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SetScrollInfo(m_hWnd, SB_CTL, lpScrollInfo, bRedraw);
	}
	BOOL GetScrollInfo(LPSCROLLINFO lpScrollInfo) const throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::GetScrollInfo(m_hWnd, SB_CTL, lpScrollInfo);
	}
	int GetScrollLimit() const throw()
	{
		int nMin, nMax;
		::GetScrollRange(m_hWnd, SB_CTL, &nMin, &nMax);
		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_PAGE;
		if(::GetScrollInfo(m_hWnd, SB_CTL, &info))
			nMax -= ((info.nPage-1) > 0) ? (info.nPage-1) : 0;

		return nMax;
	}
};

typedef CWinScrollBarT<CWindow>	CWinScrollBar;


// --- Windows Common Controls ---

/////////////////////////////////////////////////////////////////////////////
// CWinImageList

class CWinImageList
{
public:
	HIMAGELIST m_hImageList;

// Constructors
	CWinImageList(HIMAGELIST hImageList = NULL) throw() : 
		m_hImageList(hImageList) 
	{ 
	}
	CWinImageList(const CWinImageList& il) throw() :
		m_hImageList(il.m_hImageList)
	{
	}
	~CWinImageList() throw()
	{
		// Use CWinManagedImageList to get automatic cleanup
	}

	CWinImageList& operator=(HIMAGELIST hImageList) throw()
	{
		m_hImageList = hImageList;
		return *this;
	}

	operator HIMAGELIST() const throw() { return m_hImageList; }

	BOOL Create(int cx, int cy, UINT nFlags, int nInitial, int nGrow) throw()
	{
		ATLASSERT(m_hImageList == NULL);
		m_hImageList = ImageList_Create(cx, cy, nFlags, nInitial, nGrow);
		return (m_hImageList != NULL) ? TRUE : FALSE;
	}
	BOOL Create(UINT nBitmapID, int cx, int nGrow, COLORREF crMask) throw()
	{
		ATLASSERT(m_hImageList == NULL);
		ATLASSERT(HIWORD(nBitmapID) == 0);
		m_hImageList = ImageList_LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(nBitmapID), cx, nGrow, crMask);
		return (m_hImageList != NULL) ? TRUE : FALSE;
	}
	BOOL Create(LPCTSTR lpszBitmapID, int cx, int nGrow, COLORREF crMask) throw()
	{
		ATLASSERT(m_hImageList == NULL);
		m_hImageList = ImageList_LoadBitmap(_AtlBaseModule.GetResourceInstance(), lpszBitmapID, cx, nGrow, crMask);
		return (m_hImageList != NULL) ? TRUE : FALSE;
	}
	BOOL Merge(HIMAGELIST hImageList1, int nImage1, HIMAGELIST hImageList2, int nImage2, int dx, int dy) throw()
	{
		ATLASSERT(m_hImageList == NULL);
		m_hImageList = ImageList_Merge(hImageList1, nImage1, hImageList2, nImage2, dx, dy);
		return (m_hImageList != NULL) ? TRUE : FALSE;
	}

// Attributes

	void Attach(HIMAGELIST hImageList) throw()
	{
		ATLASSERT(m_hImageList == NULL);
		m_hImageList = hImageList;
	}
	HIMAGELIST Detach() throw()
	{
		HIMAGELIST hImageList = m_hImageList;
		m_hImageList = NULL;
		return hImageList;
	}

	int GetImageCount() const throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_GetImageCount(m_hImageList);
	}
	COLORREF SetBkColor(COLORREF cr) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_SetBkColor(m_hImageList, cr);
	}
	COLORREF GetBkColor() const throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_GetBkColor(m_hImageList);
	}
	BOOL GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_GetImageInfo(m_hImageList, nImage, pImageInfo);
	}

// Operations
	BOOL Destroy() throw()
	{
		if (m_hImageList == NULL)
			return FALSE;
		BOOL bRet = ImageList_Destroy(Detach());
		if(bRet)
			m_hImageList = NULL;
		return bRet;
	}

	int Add(HBITMAP hBitmap, HBITMAP hBitmapMask) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_Add(m_hImageList, hBitmap, hBitmapMask);
	}
	int Add(HBITMAP hBitmap, COLORREF crMask) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_AddMasked(m_hImageList, hBitmap, crMask);
	}
	BOOL Remove(int nImage) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_Remove(m_hImageList, nImage);
	}
	BOOL Replace(int nImage, HBITMAP hBitmap, HBITMAP hBitmapMask) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_Replace(m_hImageList, nImage, hBitmap, hBitmapMask);
	}
	int AddIcon(HICON hIcon) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_AddIcon(m_hImageList, hIcon);
	}
	int ReplaceIcon(int nImage, HICON hIcon) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_ReplaceIcon(m_hImageList, nImage, hIcon);
	}
	HICON ExtractIcon(int nImage) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_ExtractIcon(NULL, m_hImageList, nImage);
	}
	BOOL Draw(HDC hDC, int nImage, POINT pt, UINT nStyle) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		ATLASSERT(hDC != NULL);
		return ImageList_Draw(m_hImageList, nImage, hDC, pt.x, pt.y, nStyle);
	}
	BOOL DrawEx(int nImage, HDC hDC, int x, int y, int dx, int dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		ATLASSERT(hDC != NULL);
		return ImageList_DrawEx(m_hImageList, nImage, hDC, x, y, dx, dy, rgbBk, rgbFg, fStyle);
	}
	BOOL SetOverlayImage(int nImage, int nOverlay) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_SetOverlayImage(m_hImageList, nImage, nOverlay);
	}

// Drag APIs
	BOOL BeginDrag(int nImage, POINT ptHotSpot) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_BeginDrag(m_hImageList, nImage, ptHotSpot.x, ptHotSpot.y);
	}
	static void PASCAL EndDrag() throw()
	{
		ImageList_EndDrag();
	}
	static BOOL PASCAL DragMove(POINT pt) throw()
	{
		return ImageList_DragMove(pt.x, pt.y);
	}
	BOOL SetDragCursorImage(int nDrag, POINT ptHotSpot) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_SetDragCursorImage(m_hImageList, nDrag, ptHotSpot.x, ptHotSpot.y);
	}
	static BOOL PASCAL DragShowNolock(BOOL bShow) throw()
	{
		return ImageList_DragShowNolock(bShow);
	}
	static CWinImageList PASCAL GetDragImage(LPPOINT lpPoint, LPPOINT lpPointHotSpot) throw()
	{
		return CWinImageList(ImageList_GetDragImage(lpPoint, lpPointHotSpot));
	}
	static BOOL PASCAL DragEnter(HWND hWnd, POINT point) throw()
	{
		return ImageList_DragEnter(hWnd, point.x, point.y);
	}
	static BOOL PASCAL DragLeave(HWND hWnd) throw()
	{
		return ImageList_DragLeave(hWnd);
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
	CWinImageList Duplicate() throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return CWinImageList(ImageList_Duplicate(m_hImageList));
	}
	static CWinImageList Duplicate(HIMAGELIST hImageList) throw()
	{
		ATLASSERT(hImageList != NULL);
		return CWinImageList(ImageList_Duplicate(hImageList));
	}
	BOOL SetImageCount(UINT uNewCount) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_SetImageCount(m_hImageList, uNewCount);
	}
	BOOL Copy(int nSrc, int nDst, UINT uFlags = ILCF_MOVE) throw()
	{
		ATLASSERT(m_hImageList != NULL);
		return ImageList_Copy(m_hImageList, nDst, m_hImageList, nSrc, uFlags);
	}
	static BOOL DrawIndirect(IMAGELISTDRAWPARAMS* pimldp) throw()
	{
		return ImageList_DrawIndirect(pimldp);
	}
#endif //(_WIN32_IE >= 0x0400)
};

class CWinManagedImageList :
	public CWinImageList
{
public:
	CWinManagedImageList() throw() :
		CWinImageList(NULL)
	{
	}
	CWinManagedImageList(CWinManagedImageList& il) throw() :
		CWinImageList(il.Detach())
	{
	}
	explicit CWinManagedImageList(HIMAGELIST hImageList) throw() :
		CWinImageList(hImageList)
	{
	}
	~CWinManagedImageList() throw()
	{
		if (m_hImageList != NULL)
		{
			Destroy();
		}
	}

	CWinManagedImageList& operator=(CWinManagedImageList& il) throw()
	{
		if (m_hImageList != NULL)
		{
			Destroy();
		}
		Attach(il.Detach());

		return *this;
	}
};


#ifndef UNDER_CE
/////////////////////////////////////////////////////////////////////////////
// CWinToolTip

class CWinToolInfo : public TOOLINFO
{
public:
/**/	TCHAR m_szText[256];

/**/	operator LPTOOLINFO() { return this; }
/**/	operator LPARAM() { return (LPARAM)this; }

	void Fill(HWND hWnd, UINT nIDTool)
	{
		memset(this, 0, sizeof(TOOLINFO));
		cbSize = sizeof(TOOLINFO);
		if(nIDTool == 0)
		{
			hwnd = ::GetParent(hWnd);
			uFlags = TTF_IDISHWND;
			uId = (UINT_PTR)hWnd;
		}
		else
		{
			hwnd = hWnd;
			uFlags = 0;
			uId = nIDTool;
		}
	}
};

template <class Base>
class CWinToolTipT : public Base
{
public:
// Constructors
	CWinToolTipT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinToolTipT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return TOOLTIPS_CLASS;
	}

	void GetText(LPTOOLINFO lpToolInfo) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_GETTEXT, 0, (LPARAM)&lpToolInfo);
	}
	void GetText(LPTSTR lpstrText, HWND hWnd, UINT nIDTool = 0) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(hWnd != NULL);

//		TOOLINFO ti;
//		FillInToolInfo(ti, hWnd, nIDTool);
		CWinToolInfo ti;
		ti.Fill(hWnd, nIDTool);
		ti.lpszText = lpstrText;	// should be 256 characters long
		::SendMessage(m_hWnd, TTM_GETTEXT, 0, ti);
	}
	BOOL GetToolInfo(LPTOOLINFO lpToolInfo) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TTM_GETTOOLINFO, 0, (LPARAM)lpToolInfo);
	}
	BOOL GetToolInfo(HWND hWnd, UINT nIDTool, UINT* puFlags, LPRECT lpRect, LPTSTR lpstrText) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(hWnd != NULL);

		TOOLINFO ti;
		FillInToolInfo(ti, hWnd, nIDTool);
		ti.lpszText = lpstrText;
		BOOL bRet = (BOOL)::SendMessage(m_hWnd, TTM_GETTOOLINFO, 0, (LPARAM)&ti);
		if(bRet)
		{
			*puFlags = ti.uFlags;
			memcpy(lpRect, &(ti.rect), sizeof(RECT));
		}
		return bRet;
	}
	void SetToolInfo(LPTOOLINFO lpToolInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_SETTOOLINFO, 0, (LPARAM)lpToolInfo);
	}
	void SetToolRect(LPTOOLINFO lpToolInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_NEWTOOLRECT, 0, (LPARAM)lpToolInfo);
	}
	void SetToolRect(HWND hWnd, UINT nIDTool, LPCRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(hWnd != NULL);
		ATLASSERT(nIDTool != 0);

		TOOLINFO ti;
		FillInToolInfo(ti, hWnd, nIDTool);
		memcpy(&ti.rect, lpRect, sizeof(RECT));
		::SendMessage(m_hWnd, TTM_NEWTOOLRECT, 0, (LPARAM)&ti);
	}
	int GetToolCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TTM_GETTOOLCOUNT, 0, 0L);
	}

// Operations
	void Activate(BOOL bActivate)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_ACTIVATE, bActivate, 0L);
	}
	BOOL AddTool(LPTOOLINFO lpToolInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)lpToolInfo);
	}
	BOOL AddTool(HWND hWnd, LPCTSTR lpszText = LPSTR_TEXTCALLBACK, LPCRECT lpRectTool = NULL, UINT nIDTool = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(hWnd != NULL);
		ATLASSERT(lpszText != NULL);
		// the toolrect and toolid must both be zero or both valid
		ATLASSERT((lpRectTool != NULL && nIDTool != 0) || (lpRectTool == NULL && nIDTool == 0));

		TOOLINFO ti;
		FillInToolInfo(ti, hWnd, nIDTool);
		if(lpRectTool != NULL)
			memcpy(&ti.rect, lpRectTool, sizeof(RECT));
		ti.hinst = _AtlBaseModule.GetResourceInstance();	// needed only if lpszText is from MAKEINTRESOURCE
		ti.lpszText = (LPTSTR)lpszText;
		return (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
	}
	BOOL AddTool(HWND hWnd, UINT nIDText, LPCRECT lpRectTool = NULL, UINT nIDTool = 0)
	{
		ATLASSERT(nIDText != 0);
		return AddTool(hWnd, MAKEINTRESOURCE(nIDText), lpRectTool, nIDTool);
	}
	void DelTool(LPTOOLINFO lpToolInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_DELTOOL, 0, (LPARAM)lpToolInfo);
	}
	void DelTool(HWND hWnd, UINT nIDTool = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(hWnd != NULL);

		TOOLINFO ti;
		FillInToolInfo(ti, hWnd, nIDTool);
		::SendMessage(m_hWnd, TTM_DELTOOL, 0, (LPARAM)&ti);
	}
	BOOL HitTest(LPTTHITTESTINFO lpHitTestInfo) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TTM_HITTEST, 0, (LPARAM)lpHitTestInfo);
	}
	BOOL HitTest(HWND hWnd, POINT pt, LPTOOLINFO lpToolInfo) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(hWnd != NULL);
		ATLASSERT(lpToolInfo != NULL);

		TTHITTESTINFO hti;
		memset(&hti, 0, sizeof(hti));
		hti.ti.cbSize = sizeof(TOOLINFO);
		hti.hwnd = hWnd;
		hti.pt.x = pt.x;
		hti.pt.y = pt.y;
		if((BOOL)::SendMessage(m_hWnd, TTM_HITTEST, 0, (LPARAM)&hti))
		{
			memcpy(lpToolInfo, &hti.ti, sizeof(TOOLINFO));
			return TRUE;
		}
		return FALSE;
	}
	void RelayEvent(LPMSG lpMsg)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_RELAYEVENT, 0, (LPARAM)lpMsg);
	}
	void SetDelayTime(UINT nDelay)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_SETDELAYTIME, 0, nDelay);
	}
	void UpdateTipText(LPTOOLINFO lpToolInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_UPDATETIPTEXT, 0, (LPARAM)lpToolInfo);
	}
	void UpdateTipText(LPCTSTR lpszText, HWND hWnd, UINT nIDTool = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(hWnd != NULL);

		TOOLINFO ti;
		FillInToolInfo(ti, hWnd, nIDTool);
		ti.hinst = _AtlBaseModule.GetResourceInstance();
		ti.lpszText = (LPTSTR)lpszText;
		::SendMessage(m_hWnd, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
	}
	void UpdateTipText(UINT nIDText, HWND hWnd, UINT nIDTool = 0)
	{
		ATLASSERT(nIDText != 0);
		UpdateTipText(MAKEINTRESOURCE(nIDText), hWnd, nIDTool);
	}

// Implementation
/**/	static void FillInToolInfo(TOOLINFO& ti, HWND hWnd, UINT nIDTool)
	{
		memset(&ti, 0, sizeof(ti));
		ti.cbSize = sizeof(ti);
		if (nIDTool == 0)
		{
			ti.hwnd = ::GetParent(hWnd);
			ti.uFlags = TTF_IDISHWND;
			ti.uId = (UINT)hWnd;
		}
		else
		{
			ti.hwnd = hWnd;
			ti.uFlags = 0;
			ti.uId = nIDTool;
		}
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
	int GetDelayTime(DWORD dwType)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TTM_GETDELAYTIME, dwType, 0L);
	}
	void SetDelayTime(DWORD dwType, int nTime)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_SETDELAYTIME, dwType, MAKELPARAM(nTime, 0));
	}
	void GetMargin(LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_GETMARGIN, 0, (LPARAM)lpRect);
	}
	void SetMargin(LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_SETMARGIN, 0, (LPARAM)lpRect);
	}
	int GetMaxTipWidth()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TTM_GETMAXTIPWIDTH, 0, 0L);
	}
	int SetMaxTipWidth(int nWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, nWidth);
	}
	COLORREF GetTipBkColor()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, TTM_GETTIPBKCOLOR, 0, 0L);
	}
	void SetTipBkColor(COLORREF clr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_SETTIPBKCOLOR, (WPARAM)clr, 0L);
	}
	COLORREF GetTipTextColor()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, TTM_GETTIPTEXTCOLOR, 0, 0L);
	}
	void SetTipTextColor(COLORREF clr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_SETTIPTEXTCOLOR, (WPARAM)clr, 0L);
	}
	BOOL GetCurrentTool(LPTOOLINFO lpToolInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TTM_GETCURRENTTOOL, 0, (LPARAM)lpToolInfo);
	}
	void Pop()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_POP, 0, 0L);
	}
	void TrackActivate(LPTOOLINFO lpToolInfo, BOOL bActivate)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_TRACKACTIVATE, bActivate, (LPARAM)lpToolInfo);
	}
	void TrackPosition(int xPos, int yPos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TTM_TRACKPOSITION, 0, MAKELPARAM(xPos, yPos));
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinToolTipT<CWindow>		CWinToolTip;
#endif // UNDER_CE

/////////////////////////////////////////////////////////////////////////////
// CWinHeader

template <class Base>
class CWinHeaderT : public Base
{
public:
// Constructors
	CWinHeaderT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinHeaderT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_HEADER;
	}

	int GetItemCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, HDM_GETITEMCOUNT, 0, 0L);
	}
	BOOL GetItem(int nIndex, HD_ITEM* pHeaderItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, HDM_GETITEM, nIndex, (LPARAM)pHeaderItem);
	}
	BOOL SetItem(int nIndex, HD_ITEM* pHeaderItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, HDM_SETITEM, nIndex, (LPARAM)pHeaderItem);
	}

// Operations
	int InsertItem(int nIndex, HD_ITEM* phdi)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, HDM_INSERTITEM, nIndex, (LPARAM)phdi);
	}
	BOOL DeleteItem(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, HDM_DELETEITEM, nIndex, 0L);
	}
	BOOL Layout(HD_LAYOUT* pHeaderLayout)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, HDM_LAYOUT, 0, (LPARAM)pHeaderLayout);
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
	CWinImageList GetImageList()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, HDM_GETIMAGELIST, 0, 0L));
	}
	CWinImageList SetImageList(HIMAGELIST hImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, HDM_SETIMAGELIST, 0, (LPARAM)hImageList));
	}
	BOOL GetOrderArray(int nSize, int* lpnArray)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, HDM_GETORDERARRAY, nSize, (LPARAM)lpnArray);
	}
	BOOL SetOrderArray(int nSize, int* lpnArray)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, HDM_SETORDERARRAY, nSize, (LPARAM)lpnArray);
	}
	int OrderToIndex(int nOrder)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, HDM_ORDERTOINDEX, nOrder, 0L);
	}
	CWinImageList CreateDragImage(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, HDM_CREATEDRAGIMAGE, nIndex, 0L));
	}
	BOOL GetItemRect(int nIndex, LPRECT lpItemRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, HDM_GETITEMRECT, nIndex, (LPARAM)lpItemRect);
	}
	int SetHotDivider(BOOL bPos, DWORD dwInputValue)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, HDM_SETHOTDIVIDER, bPos, dwInputValue);
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinHeaderT<CWindow>		CWinHeader;

/////////////////////////////////////////////////////////////////////////////
// CWinListView

template <class Base>
class CWinListViewT : public Base
{
public:
// Constructors
	CWinListViewT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinListViewT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_LISTVIEW;
	}

	COLORREF GetBkColor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, LVM_GETBKCOLOR, 0, 0L);
	}
	BOOL SetBkColor(COLORREF cr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETBKCOLOR, 0, cr);
	}
	CWinImageList GetImageList(int nImageListType) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, LVM_GETIMAGELIST, nImageListType, 0L));
	}
	CWinImageList SetImageList(HIMAGELIST hImageList, int nImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, LVM_SETIMAGELIST, nImageList, (LPARAM)hImageList));
	}
	int GetItemCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETITEMCOUNT, 0, 0L);
	}
	BOOL GetItem(LV_ITEM* pItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)pItem);
	}
	BOOL SetItem(const LV_ITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)pItem);
	}
	BOOL SetItemState(int nItem, UINT nState, UINT nStateMask)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItem(nItem, 0, LVIF_STATE, NULL, 0, nState, nStateMask, 0);
	}
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItem(nItem, nSubItem, LVIF_TEXT, lpszText, 0, 0, 0, 0);
	}
	BOOL SetItemData(int nItem, DWORD_PTR dwData)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItem(nItem, 0, LVIF_PARAM, NULL, 0, 0, 0, (LPARAM)dwData);
	}
	int InsertItem(const LV_ITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_INSERTITEM, 0, (LPARAM)pItem);
	}
	int InsertItem(int nItem, LPCTSTR lpszItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return InsertItem(LVIF_TEXT, nItem, lpszItem, 0, 0, 0, 0);
	}
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return InsertItem(LVIF_TEXT|LVIF_IMAGE, nItem, lpszItem, 0, 0, nImage, 0);
	}
	BOOL DeleteItem(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_DELETEITEM, nItem, 0L);
	}
	BOOL DeleteAllItems()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_DELETEALLITEMS, 0, 0L);
	}
	UINT GetCallbackMask() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, LVM_GETCALLBACKMASK, 0, 0L);
	}
	BOOL SetCallbackMask(UINT nMask)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETCALLBACKMASK, nMask, 0L);
	}
	int GetNextItem(int nItem, int nFlags) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETNEXTITEM, nItem, MAKELPARAM(nFlags, 0));
	}
	int FindItem(LV_FINDINFO* pFindInfo, int nStart) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_FINDITEM, nStart, (LPARAM)pFindInfo);
	}
	int HitTest(LV_HITTESTINFO* pHitTestInfo) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_HITTEST, 0, (LPARAM)pHitTestInfo);
	}
	BOOL SetItemPosition(int nItem, POINT pt)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETITEMPOSITION32, nItem, (LPARAM)&pt);
	}
	BOOL GetItemPosition(int nItem, LPPOINT lpPoint) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETITEMPOSITION, nItem, (LPARAM)lpPoint);
	}
	int GetStringWidth(LPCTSTR lpsz) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETSTRINGWIDTH, 0, (LPARAM)lpsz);
	}
	BOOL EnsureVisible(int nItem, BOOL bPartialOK)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_ENSUREVISIBLE, nItem, MAKELPARAM(bPartialOK, 0));
	}
	BOOL Scroll(SIZE size)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SCROLL, size.cx, size.cy);
	}
	BOOL RedrawItems(int nFirst, int nLast)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_REDRAWITEMS, nFirst, nLast);
	}
	BOOL Arrange(UINT nCode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_ARRANGE, nCode, 0L);
	}
	CWinEdit EditLabel(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinEdit((HWND)::SendMessage(m_hWnd, LVM_EDITLABEL, nItem, 0L));
	}
	CWinEdit GetEditControl() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinEdit((HWND)::SendMessage(m_hWnd, LVM_GETEDITCONTROL, 0, 0L));
	}
	BOOL GetColumn(int nCol, LV_COLUMN* pColumn) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETCOLUMN, nCol, (LPARAM)pColumn);
	}
	BOOL SetColumn(int nCol, const LV_COLUMN* pColumn)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETCOLUMN, nCol, (LPARAM)pColumn);
	}
	int InsertColumn(int nCol, const LV_COLUMN* pColumn)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_INSERTCOLUMN, nCol, (LPARAM)pColumn);
	}
	BOOL DeleteColumn(int nCol)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_DELETECOLUMN, nCol, 0L);
	}
	int GetColumnWidth(int nCol) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETCOLUMNWIDTH, nCol, 0L);
	}
	BOOL SetColumnWidth(int nCol, int cx)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETCOLUMNWIDTH, nCol, MAKELPARAM(cx, 0));
	}
	BOOL GetViewRect(LPRECT lpRect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETVIEWRECT, 0, (LPARAM)lpRect);
	}
	COLORREF GetTextColor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, LVM_GETTEXTCOLOR, 0, 0L);
	}
	BOOL SetTextColor(COLORREF cr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETTEXTCOLOR, 0, cr);
	}
	COLORREF GetTextBkColor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, LVM_GETTEXTBKCOLOR, 0, 0L);
	}
	BOOL SetTextBkColor(COLORREF cr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETTEXTBKCOLOR, 0, cr);
	}
	int GetTopIndex() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETTOPINDEX, 0, 0L);
	}
	int GetCountPerPage() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETCOUNTPERPAGE, 0, 0L);
	}
	BOOL GetOrigin(LPPOINT lpPoint) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETORIGIN, 0, (LPARAM)lpPoint);
	}
	BOOL Update(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_UPDATE, nItem, 0L);
	}
	BOOL SetItemState(int nItem, LV_ITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETITEMSTATE, nItem, (LPARAM)pItem);
	}
	UINT GetItemState(int nItem, UINT nMask) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, LVM_GETITEMSTATE, nItem, nMask);
	}
	void SetItemCount(int nItems)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, LVM_SETITEMCOUNT, nItems, 0L);
	}
	BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SORTITEMS, dwData, (LPARAM)pfnCompare);
	}
	UINT GetSelectedCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, LVM_GETSELECTEDCOUNT, 0, 0L);
	}

	BOOL GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		lpRect->left = nCode;
		return (BOOL)::SendMessage(m_hWnd, LVM_GETITEMRECT, (WPARAM)nItem, (LPARAM)lpRect);
	}

	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
	{
		LV_COLUMN column;
		column.mask = LVCF_TEXT|LVCF_FMT;
		column.pszText = (LPTSTR)lpszColumnHeading;
		column.fmt = nFormat;
		if (nWidth != -1)
		{
			column.mask |= LVCF_WIDTH;
			column.cx = nWidth;
		}
		if (nSubItem != -1)
		{
			column.mask |= LVCF_SUBITEM;
			column.iSubItem = nSubItem;
		}
		return InsertColumn(nCol, &column);
	}

	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_ITEM item;
		item.mask = nMask;
		item.iItem = nItem;
		item.iSubItem = 0;
		item.pszText = (LPTSTR)lpszItem;
		item.state = nState;
		item.stateMask = nStateMask;
		item.iImage = nImage;
		item.lParam = lParam;
		return InsertItem(&item);
	}

	int HitTest(POINT pt, UINT* pFlags) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_HITTESTINFO hti;
		hti.pt = pt;
		int nRes = (int)::SendMessage(m_hWnd, LVM_HITTEST, 0, (LPARAM)&hti);
		if (pFlags != NULL)
			*pFlags = hti.flags;
		return nRes;
	}

	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
		int nImage, UINT nState, UINT nStateMask, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_ITEM lvi;
		lvi.mask = nMask;
		lvi.iItem = nItem;
		lvi.iSubItem = nSubItem;
		lvi.stateMask = nStateMask;
		lvi.state = nState;
		lvi.pszText = (LPTSTR) lpszItem;
		lvi.iImage = nImage;
		lvi.lParam = lParam;
		return (BOOL)::SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)&lvi);
	}

#ifndef _ATL_NO_COM
	BOOL GetItemText(int nItem, int nSubItem, BSTR& bstrText) const
	{
		USES_CONVERSION;
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(bstrText == NULL);
		LV_ITEM lvi;
		memset(&lvi, 0, sizeof(LV_ITEM));
		lvi.iSubItem = nSubItem;

		LPTSTR lpstrText = NULL;
		int nRes = 0;
		for(int nLen = 256; ; nLen *= 2)
		{
			ATLTRY(lpstrText = new TCHAR[nLen]);
			if(lpstrText == NULL)
				break;
			lpstrText[0] = NULL;
			lvi.cchTextMax = nLen;
			lvi.pszText = lpstrText;
			nRes  = (int)::SendMessage(m_hWnd, LVM_GETITEMTEXT, (WPARAM)nItem, (LPARAM)&lvi);
			if(nRes < nLen - 1)
				break;
			delete [] lpstrText;
			lpstrText = NULL;
		}

		if(lpstrText != NULL)
		{
			if(nRes != 0)
				bstrText = ::SysAllocString(T2OLE(lpstrText));
			delete [] lpstrText;
		}

		return (bstrText != NULL) ? TRUE : FALSE;
	}
#endif //!_ATL_NO_COM

	int GetItemText(int nItem, int nSubItem, LPTSTR lpszText, int nLen) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_ITEM lvi;
		memset(&lvi, 0, sizeof(LV_ITEM));
		lvi.iSubItem = nSubItem;
		lvi.cchTextMax = nLen;
		lvi.pszText = lpszText;
		return (int)::SendMessage(m_hWnd, LVM_GETITEMTEXT, (WPARAM)nItem, (LPARAM)&lvi);
	}

	DWORD_PTR GetItemData(int nItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_ITEM lvi;
		memset(&lvi, 0, sizeof(LV_ITEM));
		lvi.iItem = nItem;
		lvi.mask = LVIF_PARAM;
		::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)&lvi);
		return lvi.lParam;
	}

	CWinImageList RemoveImageList(int nImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, LVM_SETIMAGELIST, (WPARAM)nImageList, NULL));
	}

	CWinImageList CreateDragImage(int nItem, LPPOINT lpPoint)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, LVM_CREATEDRAGIMAGE, nItem, (LPARAM)lpPoint));
	}

	BOOL AddColumn(LPCTSTR strItem,int nItem,int nSubItem = -1,
			int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
			int nFmt = LVCFMT_LEFT)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_COLUMN lvc;
		lvc.mask = nMask;
		lvc.fmt = nFmt;
		lvc.pszText = (LPTSTR)strItem;
		lvc.cx = GetStringWidth(lvc.pszText) + 15;
		if(nMask & LVCF_SUBITEM)
		{
			if(nSubItem != -1)
				lvc.iSubItem = nSubItem;
			else
				lvc.iSubItem = nItem;
		}
		return InsertColumn(nItem, &lvc);
	}

	BOOL AddItem(int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex = -1)
	{
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = nItem;
		lvItem.iSubItem = nSubItem;
		lvItem.pszText = (LPTSTR) strItem;
		if(nImageIndex != -1){
			lvItem.mask |= LVIF_IMAGE;
			lvItem.iImage = nImageIndex;
		}
		if(nSubItem == 0)
			return InsertItem(&lvItem);
		return SetItem(&lvItem);
	}

	// single-selection only
	int GetSelectedIndex() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LVS_SINGLESEL);

		return (int)::SendMessage(m_hWnd, LVM_GETNEXTITEM, (WPARAM)-1, MAKELPARAM(LVNI_ALL | LVNI_SELECTED, 0));
	}

	BOOL GetSelectedItem(LV_ITEM* pItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LVS_SINGLESEL);
		ATLASSERT(pItem != NULL);

		pItem->iItem = (int)::SendMessage(m_hWnd, LVM_GETNEXTITEM, (WPARAM)-1, MAKELPARAM(LVNI_ALL | LVNI_SELECTED, 0));
		if(pItem->iItem == -1)
			return FALSE;

		return (BOOL)::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)pItem);
	}

	BOOL SelectItem(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LVS_SINGLESEL);

		BOOL bRet = SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		if(bRet)
			bRet = EnsureVisible(nIndex, FALSE);
		return bRet;
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
#ifndef UNDER_CE
	BOOL GetBkImage(LPLVBKIMAGE plvbki)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETBKIMAGE, 0, (LPARAM)plvbki);
	}
	BOOL SetBkImage(LPLVBKIMAGE plvbki)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETBKIMAGE, 0, (LPARAM)plvbki);
	}
#endif
	DWORD GetExtendedListViewStyle()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0L);
	}
	// dwExMask = 0 means all styles
	DWORD SetExtendedListViewStyle(DWORD dwExStyle, DWORD dwExMask = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, dwExMask, dwExStyle);
	}
	HCURSOR GetHotCursor()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HCURSOR)::SendMessage(m_hWnd, LVM_GETHOTCURSOR, 0, 0L);
	}
	HCURSOR SetHotCursor(HCURSOR hHotCursor)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HCURSOR)::SendMessage(m_hWnd, LVM_SETHOTCURSOR, 0, (LPARAM)hHotCursor);
	}
	int GetHotItem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_GETHOTITEM, 0, 0L);
	}
	int SetHotItem(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_SETHOTITEM, nIndex, 0L);
	}
	int GetSelectionMark()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_GETSELECTIONMARK, 0, 0L);
	}
	int SetSelectionMark(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_SETSELECTIONMARK, 0, nIndex);
	}
	BOOL GetWorkAreas(int nWorkAreas, LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETWORKAREAS, nWorkAreas, (LPARAM)lpRect);
	}
	BOOL SetWorkAreas(int nWorkAreas, LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETWORKAREAS, nWorkAreas, (LPARAM)lpRect);
	}
	BOOL GetColumnOrderArray(int nCount, int* lpnArray)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETCOLUMNORDERARRAY, nCount, (LPARAM)lpnArray);
	}
	BOOL SetColumnOrderArray(int nCount, int* lpnArray)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETCOLUMNORDERARRAY, nCount, (LPARAM)lpnArray);
	}
	DWORD GetHoverTime()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetExtendedListViewStyle() & (LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE));
		return ::SendMessage(m_hWnd, LVM_GETHOVERTIME, 0, 0L);
	}
	DWORD SetHoverTime(DWORD dwHoverTime)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetExtendedListViewStyle() & (LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE));
		return ::SendMessage(m_hWnd, LVM_SETHOVERTIME, 0, dwHoverTime);
	}
	CWinHeader GetHeader()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinHeader((HWND)::SendMessage(m_hWnd, LVM_GETHEADER, 0, 0L));
	}
	BOOL GetSubItemRect(int nItem, int nSubItem, int nFlag, LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LVS_REPORT);
		ATLASSERT(lpRect != NULL);
		lpRect->top = nSubItem;
		lpRect->left = nFlag;
		return (BOOL)::SendMessage(m_hWnd, LVM_GETSUBITEMRECT, nItem, (LPARAM)lpRect);
	}
	BOOL GetCheckState(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetExtendedListViewStyle() & LVS_EX_CHECKBOXES);
		UINT uRet = GetItemState(nIndex, LVIS_STATEIMAGEMASK);
//REVIEW
		return (uRet >> 12) - 1;
	}
	BOOL SetCheckState(int nItem, BOOL bCheck)
	{
		int nCheck = bCheck ? 2 : 1;	// one based index
		return SetItemState(nItem, INDEXTOSTATEIMAGEMASK(nCheck), LVIS_STATEIMAGEMASK);
	}
	DWORD ApproximateViewRect(int cx = -1, int cy = -1, int nCount = -1)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_APPROXIMATEVIEWRECT, nCount, MAKELPARAM(cx, cy));
	}
	BOOL GetNumberOfWorkAreas(int* pnWorkAreas)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETNUMBEROFWORKAREAS, 0, (LPARAM)pnWorkAreas);
	}
	DWORD SetIconSpacing(int cx, int cy)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LVS_ICON);
		return ::SendMessage(m_hWnd, LVM_SETICONSPACING, 0, MAKELPARAM(cx, cy));
	}
	void SetItemCountEx(int nItems, DWORD dwFlags)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT((GetStyle() & LVS_OWNERDATA) && (GetStyle() & (LVS_REPORT | LVS_LIST)));
		::SendMessage(m_hWnd, LVM_SETITEMCOUNT, nItems, dwFlags);
	}
	int SubItemHitTest(LPLVHITTESTINFO lpInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_SUBITEMHITTEST, 0, (LPARAM)lpInfo);
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinListViewT<CWindow>		CWinListView;

/////////////////////////////////////////////////////////////////////////////
// CWinTreeView

template <class Base>
class CWinTreeViewT : public Base
{
public:
// Constructors
	CWinTreeViewT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinTreeViewT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Operations
	static LPCTSTR GetWndClassName()
	{
		return WC_TREEVIEW;
	}

	HTREEITEM InsertItem(LPTV_INSERTSTRUCT lpInsertStruct)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)lpInsertStruct);
	}
	HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage,
		int nSelectedImage, HTREEITEM hParent, HTREEITEM hInsertAfter)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE, lpszItem, nImage, nSelectedImage, 0, 0, 0, hParent, hInsertAfter); 
	}
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST)
	{ 
		ATLASSERT(::IsWindow(m_hWnd));
		return InsertItem(TVIF_TEXT, lpszItem, 0, 0, 0, 0, 0, hParent, hInsertAfter);
	}
	BOOL DeleteItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_DELETEITEM, 0, (LPARAM)hItem);
	}
	BOOL DeleteAllItems()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
	}
	BOOL Expand(HTREEITEM hItem, UINT nCode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_EXPAND, nCode, (LPARAM)hItem);
	}
	UINT GetCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, TVM_GETCOUNT, 0, 0L);
	}
	UINT GetIndent() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, TVM_GETINDENT, 0, 0L);
	}
	void SetIndent(UINT nIndent)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TVM_SETINDENT, nIndent, 0L);
	}
	CWinImageList GetImageList(UINT nImageList) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)(::SendMessage(m_hWnd, TVM_GETIMAGELIST, (UINT)nImageList, 0L)));
	}
	CWinImageList SetImageList(HIMAGELIST hImageList, int nImageListType)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)(::SendMessage(m_hWnd, TVM_SETIMAGELIST, (UINT)nImageListType, (LPARAM)hImageList)));
	}
	HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode)
	{ 
		ATLASSERT(::IsWindow(m_hWnd)); 
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, nCode, (LPARAM)hItem);
	}
	HTREEITEM GetChildItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
	}
	HTREEITEM GetNextSiblingItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem); 
	}
	HTREEITEM GetPrevSiblingItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUS, (LPARAM)hItem);
	}
	HTREEITEM GetParentItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem); 
	}
	HTREEITEM GetFirstVisibleItem()
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0L);
	}
	HTREEITEM GetNextVisibleItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_NEXTVISIBLE, (LPARAM)hItem);
	}
	HTREEITEM GetPrevVisibleItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUSVISIBLE, (LPARAM)hItem);
	}
	HTREEITEM GetSelectedItem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0L);
	}
	HTREEITEM GetDropHilightItem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0L);
	}
	HTREEITEM GetRootItem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_ROOT, 0L);
	}
	BOOL Select(HTREEITEM hItem, UINT nCode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_SELECTITEM, nCode, (LPARAM)hItem);
	}
	BOOL SelectItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hItem);
	}
	BOOL SelectDropTarget(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)hItem);
	}
	BOOL SelectSetFirstVisible(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_SELECTITEM, TVGN_FIRSTVISIBLE, (LPARAM)hItem);
	}
	BOOL GetItem(TV_ITEM* pItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)pItem);
	}
	BOOL SetItem(TV_ITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_SETITEM, 0, (LPARAM)pItem);
	}
	BOOL SetItemText(HTREEITEM hItem, LPCTSTR lpszItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItem(hItem, TVIF_TEXT, lpszItem, 0, 0, 0, 0, NULL);
	}
	BOOL SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItem(hItem, TVIF_IMAGE|TVIF_SELECTEDIMAGE, NULL, nImage, nSelectedImage, 0, 0, NULL);
	}
	BOOL SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItem(hItem, TVIF_STATE, NULL, 0, 0, nState, nStateMask, NULL);
	}
	BOOL SetItemData(HTREEITEM hItem, DWORD_PTR dwData)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItem(hItem, TVIF_PARAM, NULL, 0, 0, 0, 0, (LPARAM)dwData);
	}
	CWinEdit EditLabel(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinEdit((HWND)::SendMessage(m_hWnd, TVM_EDITLABEL, 0, (LPARAM)hItem));
	}
	HTREEITEM HitTest(TV_HITTESTINFO* pHitTestInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_HITTEST, 0, (LPARAM)pHitTestInfo);
	}
	CWinEdit GetEditControl() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinEdit((HWND)::SendMessage(m_hWnd, TVM_GETEDITCONTROL, 0, 0L));
	}
	UINT GetVisibleCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, TVM_GETVISIBLECOUNT, 0, 0L);
	}
	BOOL SortChildren(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_SORTCHILDREN, 0, (LPARAM)hItem);
	}
	BOOL EnsureVisible(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_ENSUREVISIBLE, 0, (LPARAM)hItem);
	}
	BOOL SortChildrenCB(LPTV_SORTCB pSort)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_SORTCHILDRENCB, 0, (LPARAM)pSort);
	}

	BOOL GetItemRect(HTREEITEM hItem, LPRECT lpRect, BOOL bTextOnly) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		*(HTREEITEM*)lpRect = hItem;
		return (BOOL)::SendMessage(m_hWnd, TVM_GETITEMRECT, (WPARAM)bTextOnly, (LPARAM)lpRect);
	}

#ifndef _ATL_NO_COM
#ifndef UNDER_CE
	BOOL GetItemText(HTREEITEM hItem, BSTR& bstrText) const
	{
		USES_CONVERSION;
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(bstrText == NULL);
		TV_ITEM item;
		item.hItem = hItem;
		item.mask = TVIF_TEXT;

		LPTSTR lpstrText = NULL;
		BOOL bRet = FALSE;
		for(int nLen = 256; ; nLen *= 2)
		{
			ATLTRY(lpstrText = new TCHAR[nLen]);
			if(lpstrText == NULL)
				break;
			lpstrText[0] = NULL;
			item.pszText = lpstrText;
			item.cchTextMax = nLen;
			bRet = ::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item) != 0;
			if(!bRet || (lstrlen(item.pszText) < nLen - 1))
				break;
			delete [] lpstrText;
			lpstrText = NULL;
		}

		if(lpstrText != NULL)
		{
			if(bRet)
				bstrText = ::SysAllocString(T2OLE(lpstrText));
			delete [] lpstrText;
		}

		return (bstrText != NULL) ? TRUE : FALSE;
	}
#endif // UNDER_CE
#endif //!_ATL_NO_COM

	BOOL GetItemImage(HTREEITEM hItem, int& nImage, int& nSelectedImage) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TV_ITEM item;
		item.hItem = hItem;
		item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		BOOL bRes = (BOOL)::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item);
		if (bRes)
		{
			nImage = item.iImage;
			nSelectedImage = item.iSelectedImage;
		}
		return bRes;
	}

	UINT GetItemState(HTREEITEM hItem, UINT nStateMask) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TV_ITEM item;
		item.hItem = hItem;
		item.mask = TVIF_STATE;
		item.stateMask = nStateMask;
		item.state = 0;
		::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item);
		return item.state;
	}

	DWORD_PTR GetItemData(HTREEITEM hItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TV_ITEM item;
		item.hItem = hItem;
		item.mask = TVIF_PARAM;
		::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item);
		return item.lParam;
	}

	BOOL ItemHasChildren(HTREEITEM hItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TV_ITEM item;
		item.hItem = hItem;
		item.mask = TVIF_CHILDREN;
		::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item);
		return item.cChildren;
	}

	BOOL SetItem(HTREEITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage,
		int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TV_ITEM item;
		item.hItem = hItem;
		item.mask = nMask;
		item.pszText = (LPTSTR) lpszItem;
		item.iImage = nImage;
		item.iSelectedImage = nSelectedImage;
		item.state = nState;
		item.stateMask = nStateMask;
		item.lParam = lParam;
		return (BOOL)::SendMessage(m_hWnd, TVM_SETITEM, 0, (LPARAM)&item);
	}

	HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage,
		int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
		HTREEITEM hParent, HTREEITEM hInsertAfter)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TV_INSERTSTRUCT tvis;
		tvis.hParent = hParent;
		tvis.hInsertAfter = hInsertAfter;
		tvis.item.mask = nMask;
		tvis.item.pszText = (LPTSTR) lpszItem;
		tvis.item.iImage = nImage;
		tvis.item.iSelectedImage = nSelectedImage;
		tvis.item.state = nState;
		tvis.item.stateMask = nStateMask;
		tvis.item.lParam = lParam;
		return (HTREEITEM)::SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)&tvis);
	}

	HTREEITEM HitTest(POINT pt, UINT* pFlags)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TV_HITTESTINFO hti;
		hti.pt = pt;
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_HITTEST, 0, (LPARAM)&hti);
		if (pFlags != NULL)
			*pFlags = hti.flags;
		return hTreeItem;
	}

	CWinImageList RemoveImageList(int nImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, TVM_SETIMAGELIST, (WPARAM)nImageList, NULL));
	}

	CWinImageList CreateDragImage(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, TVM_CREATEDRAGIMAGE, 0, (LPARAM)hItem));
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
#ifndef UNDER_CE
	CWinToolTip GetToolTips()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinToolTip((HWND)::SendMessage(m_hWnd, TVM_GETTOOLTIPS, 0, 0L));
	}
	CWinToolTip SetToolTips(HWND hWndTT)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinToolTip((HWND)::SendMessage(m_hWnd, TVM_SETTOOLTIPS, (WPARAM)hWndTT, 0L));
	}
#endif // UNDER_CE
	BOOL Expand(HTREEITEM hItem, int nType)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TVM_EXPAND, nType, (LPARAM)hItem);
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinTreeViewT<CWindow>		CWinTreeView;

/////////////////////////////////////////////////////////////////////////////
// CWinTreeViewEx

typedef CWinTreeViewExT<CWindow> CWinTreeViewEx;	// forward declaration

class CWinTreeItem
{
public:
	HTREEITEM m_hTreeItem;
	CWinTreeViewEx* m_pTreeView;

// Construction
	CWinTreeItem(HTREEITEM hTreeItem = NULL, CWinTreeViewEx* pTreeView = NULL) throw() : 
		m_hTreeItem(hTreeItem), 
		m_pTreeView(pTreeView)
	{ 
	}
 
	CWinTreeItem(const CWinTreeItem& posSrc) throw()
	{
		*this = posSrc;
	}

	operator HTREEITEM() const throw() { return m_hTreeItem; }

	CWinTreeItem& operator=(const CWinTreeItem& itemSrc) throw()
	{
		m_hTreeItem = itemSrc.m_hTreeItem;
		m_pTreeView = itemSrc.m_pTreeView;
		return *this;
	}

// Attributes
	CWinTreeViewEx* GetTreeView() const throw() { return m_pTreeView; }

	BOOL operator !() const throw() { return m_hTreeItem == NULL; }
	BOOL IsNull() const throw() { return m_hTreeItem == NULL; }
	
	BOOL GetRect(LPRECT lpRect, BOOL bTextOnly) const throw();
#ifndef _ATL_NO_COM
	BOOL GetText(BSTR& bstrText) const;
#endif //!_ATL_NO_COM
	BOOL GetImage(int& nImage, int& nSelectedImage) const throw();
	UINT GetState(UINT nStateMask) const throw();
	DWORD_PTR GetData() const throw();
	BOOL SetItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam);
	BOOL SetText(LPCTSTR lpszItem);
	BOOL SetImage(int nImage, int nSelectedImage);
	BOOL SetState(UINT nState, UINT nStateMask);
	BOOL SetData(DWORD_PTR dwData);

// Operations
	CWinTreeItem InsertAfter(LPCTSTR lpstrItem, HTREEITEM hItemAfter, int nImageIndex)
	{
		return _Insert(lpstrItem, nImageIndex, hItemAfter);
	}
	CWinTreeItem AddHead(LPCTSTR lpstrItem, int nImageIndex)
	{
		return _Insert(lpstrItem, nImageIndex, TVI_FIRST);
	}
	CWinTreeItem AddTail(LPCTSTR lpstrItem, int nImageIndex)
	{
		return _Insert(lpstrItem, nImageIndex, TVI_LAST);
	}

	CWinTreeItem GetChild() const;
	CWinTreeItem GetNext(UINT nCode) const;
	CWinTreeItem GetNextSibling() const;
	CWinTreeItem GetPrevSibling() const;
	CWinTreeItem GetParent() const;
	CWinTreeItem GetFirstVisible() const;
	CWinTreeItem GetNextVisible() const;
	CWinTreeItem GetPrevVisible() const;
	CWinTreeItem GetSelected() const;
	CWinTreeItem GetDropHilight() const;
	CWinTreeItem GetRoot() const;
	BOOL HasChildren() const;
	BOOL Delete();
	BOOL Expand(UINT nCode = TVE_EXPAND);
	BOOL Select(UINT nCode);
	BOOL Select();
	BOOL SelectDropTarget();
	BOOL SelectSetFirstVisible();
	HWND EditLabel();
	HIMAGELIST CreateDragImage();
	BOOL SortChildren();
	BOOL EnsureVisible();

	CWinTreeItem _Insert(LPCTSTR lpstrItem, int nImageIndex, HTREEITEM hItemAfter);
	int GetImageIndex();
};


template <class Base>
class CWinTreeViewExT : public CWinTreeViewT< Base >
{
public:
// Constructors
	CWinTreeViewExT(HWND hWnd = NULL) : CWinTreeViewT< Base >(hWnd) { }

	CWinTreeViewExT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

// Operations (overides that return CWinTreeItem)
	CWinTreeItem InsertItem(LPTV_INSERTSTRUCT lpInsertStruct)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)lpInsertStruct);
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem InsertItem(LPCTSTR lpszItem, int nImage,
		int nSelectedImage, HTREEITEM hParent, HTREEITEM hInsertAfter)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE, lpszItem, nImage, nSelectedImage, 0, 0, 0, hParent, hInsertAfter); 
	}
	CWinTreeItem InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST)
	{ 
		ATLASSERT(::IsWindow(m_hWnd));
		return InsertItem(TVIF_TEXT, lpszItem, 0, 0, 0, 0, 0, hParent, hInsertAfter);
	}
	CWinTreeItem GetNextItem(HTREEITEM hItem, UINT nCode)
	{ 
		ATLASSERT(::IsWindow(m_hWnd)); 
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, nCode, (LPARAM)hItem);
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem GetChildItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
		return CWinTreeItem(hTreeItem, this); 
	}
	CWinTreeItem GetNextSiblingItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem); 
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem GetPrevSiblingItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUS, (LPARAM)hItem);
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem GetParentItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem); 
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem GetFirstVisibleItem()
	{
		ATLASSERT(::IsWindow(m_hWnd)); 
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0L);
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem GetNextVisibleItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_NEXTVISIBLE, (LPARAM)hItem);
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem GetPrevVisibleItem(HTREEITEM hItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUSVISIBLE, (LPARAM)hItem);
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem GetSelectedItem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0L);
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem GetDropHilightItem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0L);
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem GetRootItem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_ROOT, 0L);
		return CWinTreeItem(hTreeItem, this);
	}
	CWinTreeItem HitTest(TV_HITTESTINFO* pHitTestInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_HITTEST, 0, (LPARAM)pHitTestInfo);
		return CWinTreeItem(hTreeItem, this);
	}

	CWinTreeItem InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage,
		int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
		HTREEITEM hParent, HTREEITEM hInsertAfter)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TV_INSERTSTRUCT tvis;
		tvis.hParent = hParent;
		tvis.hInsertAfter = hInsertAfter;
		tvis.item.mask = nMask;
		tvis.item.pszText = (LPTSTR) lpszItem;
		tvis.item.iImage = nImage;
		tvis.item.iSelectedImage = nSelectedImage;
		tvis.item.state = nState;
		tvis.item.stateMask = nStateMask;
		tvis.item.lParam = lParam;
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)&tvis);
		return CWinTreeItem(hTreeItem, this);
	}

	CWinTreeItem HitTest(POINT pt, UINT* pFlags)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TV_HITTESTINFO hti;
		hti.pt = pt;
		HTREEITEM hTreeItem = (HTREEITEM)::SendMessage(m_hWnd, TVM_HITTEST, 0, (LPARAM)&hti);
		if (pFlags != NULL)
			*pFlags = hti.flags;
		return CWinTreeItem(hTreeItem, this);
	}
};


// CWinTreeItem inline methods
inline BOOL CWinTreeItem::GetRect(LPRECT lpRect, BOOL bTextOnly) const throw()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetItemRect(m_hTreeItem,lpRect,bTextOnly);
}
inline CWinTreeItem CWinTreeItem::GetNext(UINT nCode) const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetNextItem(m_hTreeItem,nCode);
}
inline CWinTreeItem CWinTreeItem::GetChild() const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetChildItem(m_hTreeItem);
}
inline CWinTreeItem CWinTreeItem::GetNextSibling() const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetNextSiblingItem(m_hTreeItem);
}
inline CWinTreeItem CWinTreeItem::GetPrevSibling() const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetPrevSiblingItem(m_hTreeItem);
}
inline CWinTreeItem CWinTreeItem::GetParent() const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetParentItem(m_hTreeItem);
}
inline CWinTreeItem CWinTreeItem::GetFirstVisible() const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetFirstVisibleItem();
}
inline CWinTreeItem CWinTreeItem::GetNextVisible() const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetNextVisibleItem(m_hTreeItem);
}
inline CWinTreeItem CWinTreeItem::GetPrevVisible() const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetPrevVisibleItem(m_hTreeItem);
}
inline CWinTreeItem CWinTreeItem::GetSelected() const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetSelectedItem();
}
inline CWinTreeItem CWinTreeItem::GetDropHilight() const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetDropHilightItem();
}
inline CWinTreeItem CWinTreeItem::GetRoot() const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetRootItem();
}
#ifndef _ATL_NO_COM
#ifdef _OLEAUTO_H_
inline BOOL CWinTreeItem::GetText(BSTR& bstrText) const
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetItemText(m_hTreeItem, bstrText);
}
#endif // _OLEAUTO_H_
#endif //!_ATL_NO_COM
inline BOOL CWinTreeItem::GetImage(int& nImage, int& nSelectedImage) const throw()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetItemImage(m_hTreeItem,nImage,nSelectedImage);
}
inline UINT CWinTreeItem::GetState(UINT nStateMask) const throw()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetItemState(m_hTreeItem,nStateMask);
}
inline DWORD_PTR CWinTreeItem::GetData() const throw()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->GetItemData(m_hTreeItem);
}
inline BOOL CWinTreeItem::SetItem(UINT nMask, LPCTSTR lpszItem, int nImage,
		int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam)
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->SetItem(m_hTreeItem, nMask, lpszItem, nImage, nSelectedImage, nState, nStateMask, lParam);
}
inline BOOL CWinTreeItem::SetText(LPCTSTR lpszItem)
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->SetItemText(m_hTreeItem,lpszItem);
}
inline BOOL CWinTreeItem::SetImage(int nImage, int nSelectedImage)
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->SetItemImage(m_hTreeItem,nImage,nSelectedImage);
}
inline BOOL CWinTreeItem::SetState(UINT nState, UINT nStateMask)
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->SetItemState(m_hTreeItem,nState,nStateMask);
}
inline BOOL CWinTreeItem::SetData(DWORD_PTR dwData)
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->SetItemData(m_hTreeItem,dwData);
}
inline BOOL CWinTreeItem::HasChildren() const throw()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->ItemHasChildren(m_hTreeItem);
}
inline BOOL CWinTreeItem::Delete()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->DeleteItem(m_hTreeItem);
}
inline BOOL CWinTreeItem::Expand(UINT nCode)
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->Expand(m_hTreeItem,nCode);
}
inline BOOL CWinTreeItem::Select(UINT nCode)
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->Select(m_hTreeItem,nCode);
}
inline BOOL CWinTreeItem::Select()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->SelectItem(m_hTreeItem);
}
inline BOOL CWinTreeItem::SelectDropTarget()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->SelectDropTarget(m_hTreeItem);
}
inline BOOL CWinTreeItem::SelectSetFirstVisible()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->SelectSetFirstVisible(m_hTreeItem);
}
inline HWND CWinTreeItem::EditLabel()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->EditLabel(m_hTreeItem);
}
inline HIMAGELIST CWinTreeItem::CreateDragImage()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->CreateDragImage(m_hTreeItem);
}
inline BOOL CWinTreeItem::SortChildren()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->SortChildren(m_hTreeItem);
}
inline BOOL CWinTreeItem::EnsureVisible()
{
	ATLASSERT(m_pTreeView != NULL);
	return m_pTreeView->EnsureVisible(m_hTreeItem);
}

inline CWinTreeItem CWinTreeItem::_Insert(LPCTSTR lpstrItem, int nImageIndex, HTREEITEM hItemAfter)
{
	ATLASSERT(m_pTreeView != NULL);
	TV_INSERTSTRUCT ins;
	ins.hParent = m_hTreeItem;
	ins.hInsertAfter = hItemAfter;
	ins.item.mask = TVIF_TEXT;
	ins.item.pszText = (LPTSTR)lpstrItem;
	if(nImageIndex != -1)
	{
		ins.item.mask |= TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		ins.item.iImage = nImageIndex;
		ins.item.iSelectedImage = nImageIndex;
	}
	return CWinTreeItem(m_pTreeView->InsertItem(&ins), m_pTreeView);
}

inline int CWinTreeItem::GetImageIndex()
{
	ATLASSERT(m_pTreeView != NULL);
	TV_ITEM item;
	item.mask = TVIF_HANDLE | TVIF_IMAGE;
	item.hItem = m_hTreeItem;
	m_pTreeView->GetItem(&item);
	return item.iImage;
}


/////////////////////////////////////////////////////////////////////////////
// CWinToolBar

template <class Base>
class CWinToolBarT : public Base
{
public:
// Construction
	CWinToolBarT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinToolBarT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return TOOLBARCLASSNAME;
	}

	BOOL IsButtonEnabled(int nID) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_ISBUTTONENABLED, nID, 0L);
	}
	BOOL IsButtonChecked(int nID) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_ISBUTTONCHECKED, nID, 0L);
	}
	BOOL IsButtonPressed(int nID) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_ISBUTTONPRESSED, nID, 0L);
	}
	BOOL IsButtonHidden(int nID) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return(BOOL) ::SendMessage(m_hWnd, TB_ISBUTTONHIDDEN, nID, 0L);
	}
	BOOL IsButtonIndeterminate(int nID) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_ISBUTTONINDETERMINATE, nID, 0L);
	}
	BOOL SetState(int nID, UINT nState)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_SETSTATE, nID, MAKELPARAM(nState, 0));
	}
	int GetState(int nID) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_GETSTATE, nID, 0L);
	}
	BOOL GetButton(int nIndex, LPTBBUTTON lpButton) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_GETBUTTON, nIndex, (LPARAM)lpButton);
	}
	int GetButtonCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_BUTTONCOUNT, 0, 0L);
	}
	BOOL GetItemRect(int nIndex, LPRECT lpRect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_GETITEMRECT, nIndex, (LPARAM)lpRect);
	}
	void SetButtonStructSize(int nSize = sizeof(TBBUTTON))
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TB_BUTTONSTRUCTSIZE, nSize, 0L);
	}
	BOOL SetButtonSize(SIZE size)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(size.cx, size.cy));
	}
	BOOL SetBitmapSize(SIZE size)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_SETBITMAPSIZE, 0, MAKELPARAM(size.cx, size.cy));
	}
#ifndef UNDER_CE
	CWinToolTip GetToolTips() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinToolTip((HWND)::SendMessage(m_hWnd, TB_GETTOOLTIPS, 0, 0L));
	}
#endif // UNDER_CE
	void SetToolTips(HWND hWndToolTip)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TB_SETTOOLTIPS, (WPARAM)hWndToolTip, 0L);
	}
	void SetNotifyWnd(HWND hWnd)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TB_SETPARENT, (WPARAM)hWnd, 0L);
	}
	void SetRows(int nRows, BOOL bLarger, LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TB_SETROWS, MAKELPARAM(nRows, bLarger), (LPARAM)lpRect);
	}
	int GetRows() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_GETROWS, 0, 0L);
	}
	BOOL SetCmdID(int nIndex, UINT nID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_SETCMDID, nIndex, nID);
	}
	UINT GetBitmapFlags() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, TB_GETBITMAPFLAGS, 0, 0L);
	}

// Operations
	BOOL EnableButton(int nID, BOOL bEnable = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_ENABLEBUTTON, nID, MAKELPARAM(bEnable, 0));
	}
	BOOL CheckButton(int nID, BOOL bCheck = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_CHECKBUTTON, nID, MAKELPARAM(bCheck, 0));
	}
	BOOL PressButton(int nID, BOOL bPress = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_PRESSBUTTON, nID, MAKELPARAM(bPress, 0));
	}
	BOOL HideButton(int nID, BOOL bHide = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_HIDEBUTTON, nID, MAKELPARAM(bHide, 0));
	}
	BOOL Indeterminate(int nID, BOOL bIndeterminate = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_INDETERMINATE, nID, MAKELPARAM(bIndeterminate, 0));
	}
	int AddBitmap(int nNumButtons, UINT nBitmapID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TBADDBITMAP tbab;
		tbab.hInst = _AtlBaseModule.GetResourceInstance();
		ATLASSERT(tbab.hInst != NULL);
		tbab.nID = nBitmapID;
		return (int)::SendMessage(m_hWnd, TB_ADDBITMAP, (WPARAM)nNumButtons, (LPARAM)&tbab);
	}
	int AddBitmap(int nNumButtons, HBITMAP hBitmap)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TBADDBITMAP tbab;
		tbab.hInst = NULL;
		tbab.nID = (UINT)hBitmap;
		return (int)::SendMessage(m_hWnd, TB_ADDBITMAP, (WPARAM)nNumButtons, (LPARAM)&tbab);
	}
	BOOL AddButtons(int nNumButtons, LPTBBUTTON lpButtons)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_ADDBUTTONS, nNumButtons, (LPARAM)lpButtons);
	}
	BOOL InsertButton(int nIndex, LPTBBUTTON lpButton)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_INSERTBUTTON, nIndex, (LPARAM)lpButton);
	}
	BOOL DeleteButton(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_DELETEBUTTON, nIndex, 0L);
	}
	UINT CommandToIndex(UINT nID) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, TB_COMMANDTOINDEX, nID, 0L);
	}
	void SaveState(HKEY hKeyRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TBSAVEPARAMS tbs;
		tbs.hkr = hKeyRoot;
		tbs.pszSubKey = lpszSubKey;
		tbs.pszValueName = lpszValueName;
		::SendMessage(m_hWnd, TB_SAVERESTORE, (WPARAM)TRUE, (LPARAM)&tbs);
	}
	void RestoreState(HKEY hKeyRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		TBSAVEPARAMS tbs;
		tbs.hkr = hKeyRoot;
		tbs.pszSubKey = lpszSubKey;
		tbs.pszValueName = lpszValueName;
		::SendMessage(m_hWnd, TB_SAVERESTORE, (WPARAM)FALSE, (LPARAM)&tbs);
	}
	void Customize()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TB_CUSTOMIZE, 0, 0L);
	}
	int AddString(UINT nStringID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_ADDSTRING, (WPARAM)_AtlBaseModule.GetResourceInstance(), (LPARAM)nStringID);
	}
	int AddStrings(LPCTSTR lpszStrings)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_ADDSTRING, 0, (LPARAM)lpszStrings);
	}
	void AutoSize()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TB_AUTOSIZE, 0, 0L);
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
	int HitTest(LPPOINT lpPoint)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_HITTEST, 0, (LPARAM)lpPoint);
	}
	BOOL GetAnchorHighlight()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_GETANCHORHIGHLIGHT, 0, 0L);
	}
	BOOL SetAnchorHighlight(BOOL bEnable = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_SETANCHORHIGHLIGHT, bEnable, 0L);
	}
	BOOL GetButtonInfo(int nID, LPTBBUTTONINFO lptbbi)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_GETBUTTONINFO, nID, (LPARAM)lptbbi);
	}
	BOOL SetButtonInfo(int nID, LPTBBUTTONINFO lptbbi)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_SETBUTTONINFO, nID, (LPARAM)lptbbi);
	}
	CWinImageList GetImageList()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, TB_GETIMAGELIST, 0, 0L));
	}
	CWinImageList SetImageList(HIMAGELIST hImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, TB_SETIMAGELIST, 0, (LPARAM)hImageList));
	}
	CWinImageList GetDisabledImageList()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, TB_GETDISABLEDIMAGELIST, 0, 0L));
	}
	CWinImageList SetDisabledImageList(HIMAGELIST hImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)hImageList));
	}
	CWinImageList GetHotImageList()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, TB_GETHOTIMAGELIST, 0, 0L));
	}
	CWinImageList SetHotImageList(HIMAGELIST hImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, TB_SETHOTIMAGELIST, 0, (LPARAM)hImageList));
	}
	int GetHotItem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_GETHOTITEM, 0, 0L);
	}
	int SetHotItem(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_SETHOTITEM, nItem, 0L);
	}
	DWORD GetStyle()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, TB_GETSTYLE, 0, 0L);
	}
//REVIEW - return value?
	void SetStyle(DWORD dwStyle)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TB_SETSTYLE, 0, dwStyle);
	}
	DWORD GetBitmapFlags()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, TB_GETBITMAPFLAGS, 0, 0L);
	}
	DWORD GetButtonSize()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, TB_GETBUTTONSIZE, 0, 0L);
	}
	HRESULT GetObject(REFIID iid, LPVOID* ppvObject)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HRESULT)::SendMessage(m_hWnd, TB_GETOBJECT, (WPARAM)&iid, (LPARAM)ppvObject);
	}
	BOOL GetRect(int nID, LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_GETRECT, nID, (LPARAM)lpRect);
	}
	int GetTextRows()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_GETTEXTROWS, 0, 0L);
	}
	BOOL IsButtonHighlighted(int nButtonID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_ISBUTTONHIGHLIGHTED, nButtonID, 0L);
	}
	int LoadImages(int nBitmapID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_LOADIMAGES, nBitmapID, (LPARAM)_AtlBaseModule.GetResourceInstance());
	}
	int LoadStdImages(int nBitmapID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TB_LOADIMAGES, nBitmapID, (LPARAM)HINST_COMMCTRL);
	}
	BOOL ReplaceBitmap(LPTBREPLACEBITMAP ptbrb)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_REPLACEBITMAP, 0, (LPARAM)ptbrb);
	}
	BOOL SetButtonWidth(int cxMin, int cxMax)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_SETBUTTONWIDTH, 0, MAKELPARAM(cxMin, cxMax));
	}
	DWORD SetDrawTextFlags(DWORD dwMask, DWORD dwFlags)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, TB_SETDRAWTEXTFLAGS, dwMask, dwFlags);
	}
	BOOL SetIndent(int nIndent)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_SETINDENT, nIndent, 0L);
	}
	BOOL SetMaxTextRows(int nMaxTextRows)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TB_SETMAXTEXTROWS, nMaxTextRows, 0L);
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinToolBarT<CWindow>		CWinToolBar;

/////////////////////////////////////////////////////////////////////////////
// CWinStatusBar

template <class Base>
class CWinStatusBarT : public Base
{
public:
// Constructors
	CWinStatusBarT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinStatusBarT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Methods
	static LPCTSTR GetWndClassName()
	{
		return STATUSCLASSNAME;
	}

	BOOL SetParts(int nParts, int* pWidths)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, SB_SETPARTS, nParts, (LPARAM)pWidths);
	}
	int GetParts(int nParts, int* pParts) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, SB_GETPARTS, nParts, (LPARAM)pParts);
	}
	BOOL SetText(int nPane, LPCTSTR lpszText, int nType = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPane < 256);
		return (BOOL)::SendMessage(m_hWnd, SB_SETTEXT, (nPane | nType), (LPARAM)lpszText);
	}
	int GetText(int nPane, LPTSTR lpszText, int* pType = NULL) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPane < 256);
		DWORD dwRet = ::SendMessage(m_hWnd, SB_GETTEXT, (WPARAM)nPane, (LPARAM)lpszText);
		if(pType != NULL)
			*pType = HIWORD(dwRet);
		return LOWORD(dwRet);
	}
#ifndef _ATL_NO_COM
	BOOL GetTextBSTR(int nPane, BSTR& bstrText, int* pType = NULL) const
	{
		USES_CONVERSION;
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPane < 256);
		ATLASSERT(bstrText == NULL);
		int nLength = LOWORD(::SendMessage(m_hWnd, SB_GETTEXTLENGTH, (WPARAM)nPane, 0L));
		if(nLength == 0)
			return FALSE;

		LPTSTR lpszText = (LPTSTR)_alloca((nLength + 1) * sizeof(TCHAR));
		if(!GetText(nPane, lpszText, pType))
			return FALSE;

		bstrText = ::SysAllocString(T2OLE(lpszText));
		return (bstrText != NULL) ? TRUE : FALSE;
	}
#endif //!_ATL_NO_COM
	int GetTextLength(int nPane, int* pType = NULL) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPane < 256);
		DWORD dwRet = ::SendMessage(m_hWnd, SB_GETTEXTLENGTH, (WPARAM)nPane, 0L);
		if (pType != NULL)
			*pType = HIWORD(dwRet);
		return LOWORD(dwRet);
	}
	BOOL GetRect(int nPane, LPRECT lpRect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPane < 256);
		return (BOOL)::SendMessage(m_hWnd, SB_GETRECT, nPane, (LPARAM)lpRect);
	}
	BOOL GetBorders(int* pBorders) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, SB_GETBORDERS, 0, (LPARAM)pBorders);
	}
	BOOL GetBorders(int& nHorz, int& nVert, int& nSpacing) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int borders[3];
		BOOL bResult = (BOOL)::SendMessage(m_hWnd, SB_GETBORDERS, 0, (LPARAM)&borders);
		if(bResult)
		{
			nHorz = borders[0];
			nVert = borders[1];
			nSpacing = borders[2];
		}
		return bResult;
	}
	void SetMinHeight(int nMin)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SB_SETMINHEIGHT, nMin, 0L);
	}
	BOOL SetSimple(BOOL bSimple = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, SB_SIMPLE, bSimple, 0L);
	}
	BOOL IsSimple() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, SB_ISSIMPLE, 0, 0L);
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
	COLORREF SetBkColor(COLORREF clrBk)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, SB_SETBKCOLOR, 0, (LPARAM)clrBk);
	}
	BOOL GetUnicodeFormat() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, SB_GETUNICODEFORMAT, 0, 0L);
	}
	BOOL SetUnicodeFormat(BOOL bUnicode = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, SB_SETUNICODEFORMAT, bUnicode, 0L);
	}
	void GetTipText(int nPane, LPTSTR lpstrText, int nSize) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPane < 256);
		::SendMessage(m_hWnd, SB_GETTIPTEXT, MAKEWPARAM(nPane, nSize), (LPARAM)lpstrText);
	}
	void SetTipText(int nPane, LPCTSTR lpstrText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPane < 256);
		::SendMessage(m_hWnd, SB_SETTIPTEXT, nPane, (LPARAM)lpstrText);
	}
	HICON GetIcon(int nPane) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPane < 256);
		return (HICON)::SendMessage(m_hWnd, SB_GETICON, nPane, 0L);
	}
	BOOL SetIcon(int nPane, HICON hIcon)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPane < 256);
		return (BOOL)::SendMessage(m_hWnd, SB_SETICON, nPane, (LPARAM)hIcon);
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinStatusBarT<CWindow>		CWinStatusBar;

/////////////////////////////////////////////////////////////////////////////
// CWinTab

template <class Base>
class CWinTabT : public Base
{
public:
// Constructors
	CWinTabT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinTabT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_TABCONTROL;
	}

	CWinImageList GetImageList() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, TCM_GETIMAGELIST, 0, 0L));
	}
	CWinImageList SetImageList(HIMAGELIST hImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, TCM_SETIMAGELIST, 0, (LPARAM)hImageList));
	}
	int GetItemCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TCM_GETITEMCOUNT, 0, 0L);
	}
	BOOL GetItem(int nItem, TC_ITEM* pTabCtrlItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TCM_GETITEM, nItem, (LPARAM)pTabCtrlItem);
	}
	BOOL SetItem(int nItem, TC_ITEM* pTabCtrlItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TCM_SETITEM, nItem, (LPARAM)pTabCtrlItem);
	}
	BOOL GetItemRect(int nItem, LPRECT lpRect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TCM_GETITEMRECT, nItem, (LPARAM)lpRect);
	}
	int GetCurSel() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TCM_GETCURSEL, 0, 0L);
	}
	int SetCurSel(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TCM_SETCURSEL, nItem, 0L);
	}
	SIZE SetItemSize(SIZE size)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		DWORD dwSize = ::SendMessage(m_hWnd, TCM_SETITEMSIZE, 0, MAKELPARAM(size.cx, size.cy));
		SIZE sizeRet;
		sizeRet.cx = LOWORD(dwSize);
		sizeRet.cy = HIWORD(dwSize);
		return sizeRet;
	}
	void SetPadding(SIZE size)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TCM_SETPADDING, 0, MAKELPARAM(size.cx, size.cy));
	}
	int GetRowCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TCM_GETROWCOUNT, 0, 0L);
	}
#ifndef UNDER_CE
	CWinToolTip GetTooltips() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinToolTip((HWND)::SendMessage(m_hWnd, TCM_GETTOOLTIPS, 0, 0L));
	}
#endif // UNDER_CE
	void SetTooltips(HWND hWndToolTip)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TCM_SETTOOLTIPS, (WPARAM)hWndToolTip, 0L);
	}
	int GetCurFocus() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TCM_GETCURFOCUS, 0, 0L);
	}

// Operations
	BOOL InsertItem(int nItem, TC_ITEM* pTabCtrlItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TCM_INSERTITEM, nItem, (LPARAM)pTabCtrlItem);
	}
	BOOL DeleteItem(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TCM_DELETEITEM, nItem, 0L);
	}
	BOOL DeleteAllItems()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TCM_DELETEALLITEMS, 0, 0L);
	}
	void AdjustRect(BOOL bLarger, LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TCM_ADJUSTRECT, bLarger, (LPARAM)lpRect);
	}
	void RemoveImage(int nImage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TCM_REMOVEIMAGE, nImage, 0L);
	}
	int HitTest(TC_HITTESTINFO* pHitTestInfo) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TCM_HITTEST, 0, (LPARAM)pHitTestInfo);
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
	DWORD GetExtendedStyle()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, TCM_GETEXTENDEDSTYLE, 0, 0L);
	}
	DWORD SetExtendedStyle(DWORD dwExMask, DWORD dwExStyle)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, TCM_SETEXTENDEDSTYLE, dwExMask, dwExStyle);
	}
	int SetMinTabWidth(int nWidth = -1)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TCM_SETMINTABWIDTH, 0, nWidth);
	}
	void DeselectAll(BOOL bExcludeFocus = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TCM_DESELECTALL, bExcludeFocus, 0L);
	}
	BOOL HighlightItem(int nIndex, BOOL bHighlight = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TCM_HIGHLIGHTITEM, nIndex, MAKELPARAM(bHighlight, 0));
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinTabT<CWindow>	CWinTab;

/////////////////////////////////////////////////////////////////////////////
// CWinTrackBar

template <class Base>
class CWinTrackBarT : public Base
{
public:
// Constructors
	CWinTrackBarT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinTrackBarT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return TRACKBAR_CLASS;
	}

	int GetLineSize() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TBM_GETLINESIZE, 0, 0L);
	}
	int SetLineSize(int nSize)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TBM_SETLINESIZE, 0, nSize);
	}
	int GetPageSize() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TBM_GETPAGESIZE, 0, 0L);
	}
	int SetPageSize(int nSize)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TBM_SETPAGESIZE, 0, nSize);
	}
	int GetRangeMax() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TBM_GETRANGEMAX, 0, 0L);
	}
	int GetRangeMin() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TBM_GETRANGEMIN, 0, 0L);
	}
	void GetRange(int& nMin, int& nMax) const
	{
		nMin = GetRangeMin();
		nMax = GetRangeMax();
	}
	void SetRangeMin(int nMin, BOOL bRedraw = FALSE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_SETRANGEMIN, bRedraw, nMin);
	}
	void SetRangeMax(int nMax, BOOL bRedraw = FALSE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_SETRANGEMAX, bRedraw, nMax);
	}
	void SetRange(int nMin, int nMax, BOOL bRedraw = FALSE)
	{
		SetRangeMin(nMin, bRedraw);
		SetRangeMax(nMax, bRedraw);
	}
	int GetSelStart() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, TBM_GETSELSTART, 0, 0L);
	}
	int GetSelEnd() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, TBM_GETSELEND, 0, 0L);
	}
	void GetSelection(int& nMin, int& nMax) const
	{
		nMin = GetSelStart();
		nMax = GetSelEnd();
	}
	void SetSelStart(int nMin)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_SETSELSTART, 0, (LPARAM)nMin);
	}
	void SetSelEnd(int nMax)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_SETSELEND, 0, (LPARAM)nMax);
	}
	void SetSelection(int nMin, int nMax)
	{
		SetSelStart(nMin);
		SetSelEnd(nMax);
	}
	void GetChannelRect(LPRECT lprc) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_GETCHANNELRECT, 0, (LPARAM)lprc);
	}
	void GetThumbRect(LPRECT lprc) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_GETTHUMBRECT, 0, (LPARAM)lprc);
	}
	int GetPos() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TBM_GETPOS, 0, 0L);
	}
	void SetPos(int nPos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_SETPOS, TRUE, nPos);
	}
	UINT GetNumTics() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, TBM_GETNUMTICS, 0, 0L);
	}
	DWORD* GetTicArray() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD*)::SendMessage(m_hWnd, TBM_GETPTICS, 0, 0L);
	}
	int GetTic(int nTic) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TBM_GETTIC, nTic, 0L);
	}
	int GetTicPos(int nTic) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TBM_GETTICPOS, nTic, 0L);
	}
	BOOL SetTic(int nTic)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, TBM_SETTIC, 0, nTic);
	}
	void SetTicFreq(int nFreq)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_SETTICFREQ, nFreq, 0L);
	}

// Operations
	void ClearSel(BOOL bRedraw = FALSE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_CLEARSEL, bRedraw, 0L);
	}
	void VerifyPos()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_SETPOS, FALSE, 0L);
	}
	void ClearTics(BOOL bRedraw = FALSE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_CLEARTICS, bRedraw, 0L);
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
	CWindow GetBuddy(BOOL bLeft = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWindow((HWND)::SendMessage(m_hWnd, TBM_GETBUDDY, bLeft, 0L));
	}
	CWindow SetBuddy(HWND hWndBuddy, BOOL bLeft = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWindow((HWND)::SendMessage(m_hWnd, TBM_SETBUDDY, bLeft, (LPARAM)hWndBuddy));
	}
#ifndef UNDER_CE
	CWinToolTip GetToolTips()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinToolTip((HWND)::SendMessage(m_hWnd, TBM_GETTOOLTIPS, 0, 0L));
	}
#endif // UNDER_CE
	void SetToolTips(HWND hWndTT)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, TBM_SETTOOLTIPS, (WPARAM)hWndTT, 0L);
	}
	int SetTipSide(int nSide)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, TBM_SETTIPSIDE, nSide, 0L);
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinTrackBarT<CWindow>		CWinTrackBar;

/////////////////////////////////////////////////////////////////////////////
// CWinUpDown

template <class Base>
class CWinUpDownT : public Base
{
public:
// Constructors
	CWinUpDownT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinUpDownT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return UPDOWN_CLASS;
	}

	BOOL SetAccel(int nAccel, UDACCEL* pAccel)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)LOWORD(::SendMessage(m_hWnd, UDM_SETACCEL, nAccel, (LPARAM)pAccel));
	}
	UINT GetAccel(int nAccel, UDACCEL* pAccel) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)LOWORD(::SendMessage(m_hWnd, UDM_GETACCEL, nAccel, (LPARAM)pAccel));
	}
	int SetBase(int nBase)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, UDM_SETBASE, nBase, 0L);
	}
	UINT GetBase() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)LOWORD(::SendMessage(m_hWnd, UDM_GETBASE, 0, 0L));
	}
	CWindow SetBuddy(HWND hWndBuddy)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWindow((HWND)::SendMessage(m_hWnd, UDM_SETBUDDY, (WPARAM)hWndBuddy, 0L));
	}
	CWindow GetBuddy() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWindow((HWND)::SendMessage(m_hWnd, UDM_GETBUDDY, 0, 0L));
	}
	int SetPos(int nPos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)(short)LOWORD(::SendMessage(m_hWnd, UDM_SETPOS, 0, MAKELPARAM(nPos, 0)));
	}
	int GetPos() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, UDM_GETPOS, 0, 0L);
	}
	void SetRange(int nLower, int nUpper)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, UDM_SETRANGE, 0, MAKELPARAM(nUpper, nLower));
	}
	DWORD GetRange() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, UDM_GETRANGE, 0, 0L);
	}
	void GetRange(int& nLower, int& nUpper) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		DWORD dwRet = ::SendMessage(m_hWnd, UDM_GETRANGE, 0, 0L);
		nLower = (int)(short)HIWORD(dwRet);
		nUpper = (int)(short)LOWORD(dwRet);
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
	void SetRange32(int nLower, int nUpper)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, UDM_SETRANGE32, nLower, nUpper);
	}
	void GetRange32(int& nLower, int& nUpper) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, UDM_GETRANGE32, (WPARAM)&nLower, (LPARAM)&nUpper);
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinUpDownT<CWindow>		CWinUpDown;

/////////////////////////////////////////////////////////////////////////////
// CWinProgressBar

template <class Base>
class CWinProgressBarT : public Base
{
public:
// Constructors
	CWinProgressBarT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinProgressBarT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return PROGRESS_CLASS;
	}

	DWORD SetRange(int nLower, int nUpper)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, PBM_SETRANGE, 0, MAKELPARAM(nLower, nUpper));
	}
	int SetPos(int nPos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)LOWORD(::SendMessage(m_hWnd, PBM_SETPOS, nPos, 0L));
	}
	int OffsetPos(int nPos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)LOWORD(::SendMessage(m_hWnd, PBM_DELTAPOS, nPos, 0L));
	}
	int SetStep(int nStep)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)LOWORD(::SendMessage(m_hWnd, PBM_SETSTEP, nStep, 0L));
	}

// Operations
	int StepIt()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int) LOWORD(::SendMessage(m_hWnd, PBM_STEPIT, 0, 0L));
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
	UINT GetPos()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, PBM_GETPOS, 0, 0L);
	}
	void GetRange(PPBRANGE pPBRange)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pPBRange != NULL);
		::SendMessage(m_hWnd, PBM_GETRANGE, TRUE, (LPARAM)pPBRange);
	}
	int GetRangeLimit(BOOL bLimit)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, PBM_GETRANGE, bLimit, (LPARAM)NULL);
	}
	DWORD SetRange32(int nMin, int nMax)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, PBM_SETRANGE32, nMin, nMax);
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinProgressBarT<CWindow>	CWinProgressBar;

/////////////////////////////////////////////////////////////////////////////
// CWinHotKey

template <class Base>
class CWinHotKeyT : public Base
{
public:
// Constructors
	CWinHotKeyT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinHotKeyT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return HOTKEY_CLASS;
	}

	void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, HKM_SETHOTKEY, MAKEWORD(wVirtualKeyCode, wModifiers), 0L);
	}
	DWORD GetHotKey() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, HKM_GETHOTKEY, 0, 0L);
	}
	void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		DWORD dw = ::SendMessage(m_hWnd, HKM_GETHOTKEY, 0, 0L);
		wVirtualKeyCode = LOBYTE(LOWORD(dw));
		wModifiers = HIBYTE(LOWORD(dw));
	}

// Operations
	void SetRules(WORD wInvalidComb, WORD wModifiers)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, HKM_SETRULES, wInvalidComb, MAKELPARAM(wModifiers, 0));
	}
};

typedef CWinHotKeyT<CWindow>		CWinHotKey;

/////////////////////////////////////////////////////////////////////////////
// CWinAnimate

template <class Base>
class CWinAnimateT : public Base
{
public:
// Constructors
	CWinAnimateT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinAnimateT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Operations
	static LPCTSTR GetWndClassName()
	{
		return ANIMATE_CLASS;
	}

	BOOL Open(LPCTSTR lpszFileName)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, ACM_OPEN, 0, (LPARAM)lpszFileName);
	}
	BOOL Open(UINT nID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, ACM_OPEN, 0, (LPARAM)MAKEINTRESOURCE(nID));
	}
	BOOL Play(UINT nFrom, UINT nTo, UINT nRep)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, ACM_PLAY, nRep, MAKELPARAM(nFrom, nTo));
	}
	BOOL Stop()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, ACM_STOP, 0, 0L);
	}
	BOOL Close()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, ACM_OPEN, 0, 0L);
	}
	BOOL Seek(UINT nTo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, ACM_PLAY, 0, MAKELPARAM(nTo, nTo));
	}
};

typedef CWinAnimateT<CWindow>		CWinAnimate;

/////////////////////////////////////////////////////////////////////////////
// CWinRichEdit

#ifndef UNDER_CE

#ifdef _UNICODE
#if (_RICHEDIT_VER == 0x0100)
#undef RICHEDIT_CLASS
#define RICHEDIT_CLASS	L"RICHEDIT"
#endif //(_RICHEDIT_VER == 0x0100)
#endif //_UNICODE

template <class Base>
class CWinRichEditT : public Base
{
public:
// Constructors
	CWinRichEditT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinRichEditT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return RICHEDIT_CLASS;
	}

	static LPCTSTR GetLibraryName()
	{
#if (_RICHEDIT_VER >= 0x0200)
		return _T("RICHED20.DLL");
#else
		return _T("RICHED32.DLL");
#endif
	}

	BOOL CanUndo() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_CANUNDO, 0, 0L);
	}
	int GetLineCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0L);
	}
	BOOL GetModify() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_GETMODIFY, 0, 0L);
	}
	void SetModify(BOOL bModified = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETMODIFY, bModified, 0L);
	}
	void GetRect(LPRECT lpRect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_GETRECT, 0, (LPARAM)lpRect);
	}
	DWORD GetOptions() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, EM_GETOPTIONS, 0, 0L);
	}
	DWORD SetOptions(WORD wOperation, DWORD dwOptions)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, EM_SETOPTIONS, wOperation, dwOptions);
	}

	// NOTE: first word in lpszBuffer must contain the size of the buffer!
	int GetLine(int nIndex, LPTSTR lpszBuffer) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
	}
	int GetLine(int nIndex, LPTSTR lpszBuffer, int nMaxLength) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		*(LPINT)lpszBuffer = nMaxLength;
		return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
	}

	BOOL CanPaste(UINT nFormat = 0) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_CANPASTE, nFormat, 0L);
	}
	void GetSel(LONG& nStartChar, LONG& nEndChar) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		CHARRANGE cr;
		::SendMessage(m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
		nStartChar = cr.cpMin;
		nEndChar = cr.cpMax;
	}
	void GetSel(CHARRANGE &cr) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
	}
	void LimitText(LONG nChars = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_EXLIMITTEXT, 0, nChars);
	}
	int LineFromChar(LONG nIndex) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_EXLINEFROMCHAR, 0, nIndex);
	}
	int SetSel(LONG nStartChar, LONG nEndChar)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		CHARRANGE cr = { nStartChar, nEndChar };
		return (int)::SendMessage(m_hWnd, EM_EXSETSEL, 0, (LPARAM)&cr);
	}
	int SetSel(CHARRANGE &cr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_EXSETSEL, 0, (LPARAM)&cr);
	}
	DWORD GetDefaultCharFormat(CHARFORMAT& cf) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		cf.cbSize = sizeof(CHARFORMAT);
		return (DWORD)::SendMessage(m_hWnd, EM_GETCHARFORMAT, 0, (LPARAM)&cf);
	}
	DWORD GetSelectionCharFormat(CHARFORMAT& cf) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		cf.cbSize = sizeof(CHARFORMAT);
		return (DWORD)::SendMessage(m_hWnd, EM_GETCHARFORMAT, 1, (LPARAM)&cf);
	}
	DWORD GetEventMask() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, EM_GETEVENTMASK, 0, 0L);
	}
	LONG GetLimitText() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LONG)::SendMessage(m_hWnd, EM_GETLIMITTEXT, 0, 0L);
	}
	DWORD GetParaFormat(PARAFORMAT& pf) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		pf.cbSize = sizeof(PARAFORMAT);
		return (DWORD)::SendMessage(m_hWnd, EM_GETPARAFORMAT, 0, (LPARAM)&pf);
	}
	// richedit EM_GETSELTEXT is ANSI
	LONG GetSelText(LPSTR lpBuf) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LONG)::SendMessage(m_hWnd, EM_GETSELTEXT, 0, (LPARAM)lpBuf);
	}
#ifndef _ATL_NO_COM
	BOOL GetSelTextBSTR(BSTR& bstrText) const
	{
		USES_CONVERSION;
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(bstrText == NULL);

		CHARRANGE cr;
		cr.cpMin = cr.cpMax = 0;
		::SendMessage(m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
		LPSTR lpstrText = (char*)_alloca((cr.cpMax - cr.cpMin + 1) * 2);
		lpstrText[0] = 0;
		if(::SendMessage(m_hWnd, EM_GETSELTEXT, 0, (LPARAM)lpstrText) == 0)
			return FALSE;

		bstrText = ::SysAllocString(A2W(lpstrText));
		return (bstrText != NULL) ? TRUE : FALSE;
	}
#endif //!_ATL_NO_COM
	WORD GetSelectionType() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (WORD)::SendMessage(m_hWnd, EM_SELECTIONTYPE, 0, 0L);
	}
	COLORREF SetBackgroundColor(COLORREF cr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, EM_SETBKGNDCOLOR, 0, cr);
	}
	COLORREF SetBackgroundColor()	// sets to system background
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, EM_SETBKGNDCOLOR, 1, 0);
	}
	BOOL SetDefaultCharFormat(CHARFORMAT& cf)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		cf.cbSize = sizeof(CHARFORMAT);
		return (BOOL)::SendMessage(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
	}
	BOOL SetSelectionCharFormat(CHARFORMAT& cf)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		cf.cbSize = sizeof(CHARFORMAT);
		return (BOOL)::SendMessage(m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	}
	BOOL SetWordCharFormat(CHARFORMAT& cf)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		cf.cbSize = sizeof(CHARFORMAT);
		return (BOOL)::SendMessage(m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);
	}
	DWORD SetEventMask(DWORD dwEventMask)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, EM_SETEVENTMASK, 0, dwEventMask);
	}
	BOOL SetParaFormat(PARAFORMAT& pf)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		pf.cbSize = sizeof(PARAFORMAT);
		return (BOOL)::SendMessage(m_hWnd, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
	}
	BOOL SetTargetDevice(HDC hDC, int cxLineWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_SETTARGETDEVICE, (WPARAM)hDC, cxLineWidth);
	}
	int GetTextLength() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, WM_GETTEXTLENGTH, 0, 0L);
	}
	BOOL SetReadOnly(BOOL bReadOnly = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_SETREADONLY, bReadOnly, 0L);
	}
	int GetFirstVisibleLine() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0L);
	}

// Operations
	POINT PosFromChar(LONG nChar) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		POINT point;
		::SendMessage(m_hWnd, EM_POSFROMCHAR, (WPARAM)&point, nChar);
		return point;
	}
	int CharFromPos(POINT pt) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_CHARFROMPOS, 0, MAKELPARAM(pt.x, pt.y));
	}
	void EmptyUndoBuffer()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0L);
	}
	int LineIndex(int nLine = -1) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_LINEINDEX, nLine, 0L);
	}
	int LineLength(int nLine = -1) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, EM_LINELENGTH, nLine, 0L);
	}
	BOOL LineScroll(int nLines, int nChars = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_LINESCROLL, nChars, nLines);
	}
	void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText);
	}
	void SetRect(LPCRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETRECT, 0, (LPARAM)lpRect);
	}

	BOOL DisplayBand(LPRECT pDisplayRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_DISPLAYBAND, 0, (LPARAM)pDisplayRect);
	}
	LONG FindText(DWORD dwFlags, FINDTEXT& ft) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LONG)::SendMessage(m_hWnd, EM_FINDTEXT, dwFlags, (LPARAM)&ft);
	}
	LONG FindText(DWORD dwFlags, FINDTEXTEX& ft) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LONG)::SendMessage(m_hWnd, EM_FINDTEXTEX, dwFlags, (LPARAM)&ft);
	}
	LONG FormatRange(FORMATRANGE& fr, BOOL bDisplay = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LONG)::SendMessage(m_hWnd, EM_FORMATRANGE, bDisplay, (LPARAM)&fr);
	}
	void HideSelection(BOOL bHide = TRUE, BOOL bChangeStyle = FALSE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_HIDESELECTION, bHide, bChangeStyle);
	}
	void PasteSpecial(UINT uClipFormat, DWORD dwAspect = 0, HMETAFILE hMF = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		REPASTESPECIAL reps = { dwAspect, (DWORD)hMF };
		::SendMessage(m_hWnd, EM_PASTESPECIAL, uClipFormat, (LPARAM)&reps);
	}
	void RequestResize()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_REQUESTRESIZE, 0, 0L);
	}
	LONG StreamIn(UINT uFormat, EDITSTREAM& es)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LONG)::SendMessage(m_hWnd, EM_STREAMIN, uFormat, (LPARAM)&es);
	}
	LONG StreamOut(UINT uFormat, EDITSTREAM& es)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LONG)::SendMessage(m_hWnd, EM_STREAMOUT, uFormat, (LPARAM)&es);
	}

	// Additional operations
	void ScrollCaret()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SCROLLCARET, 0, 0L);
	}
	int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, BOOL bCanUndo = FALSE)
	{
		int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
		ReplaceSel(lpstrText, bCanUndo);
		return nRet;
	}
	int AppendText(LPCTSTR lpstrText, BOOL bCanUndo = FALSE)
	{
		return InsertText(GetWindowTextLength(), lpstrText, bCanUndo);
	}

	// Clipboard operations
	BOOL Undo()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_UNDO, 0, 0L);
	}
	void Clear()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_CLEAR, 0, 0L);
	}
	void Copy()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_COPY, 0, 0L);
	}
	void Cut()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_CUT, 0, 0L);
	}
	void Paste()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_PASTE, 0, 0L);
	}

// OLE support
	IRichEditOle* GetOleInterface() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		IRichEditOle *pRichEditOle = NULL;
		::SendMessage(m_hWnd, EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle);
		return pRichEditOle;
	}
	BOOL SetOleCallback(IRichEditOleCallback* pCallback)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_SETOLECALLBACK, 0, (LPARAM)pCallback);
	}

// Rich Edit 2.0 support
#if (_RICHEDIT_VER >= 0x0200)
	DWORD GetDefaultCharFormat(CHARFORMAT2& cf) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		cf.cbSize = sizeof(CHARFORMAT2);
		return (DWORD)::SendMessage(m_hWnd, EM_GETCHARFORMAT, 0, (LPARAM)&cf);
	}
	DWORD GetSelectionCharFormat(CHARFORMAT2& cf) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		cf.cbSize = sizeof(CHARFORMAT2);
		return (DWORD)::SendMessage(m_hWnd, EM_GETCHARFORMAT, 1, (LPARAM)&cf);
	}
	BOOL SetDefaultCharFormat(CHARFORMAT2& cf)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		cf.cbSize = sizeof(CHARFORMAT2);
		return (BOOL)::SendMessage(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
	}
	BOOL SetSelectionCharFormat(CHARFORMAT2& cf)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		cf.cbSize = sizeof(CHARFORMAT2);
		return (BOOL)::SendMessage(m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	}
	BOOL SetWordCharFormat(CHARFORMAT2& cf)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		cf.cbSize = sizeof(CHARFORMAT2);
		return (BOOL)::SendMessage(m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);
	}

	DWORD GetParaFormat(PARAFORMAT2& pf) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		pf.cbSize = sizeof(PARAFORMAT2);
		return (DWORD)::SendMessage(m_hWnd, EM_GETPARAFORMAT, 0, (LPARAM)&pf);
	}
	BOOL SetParaFormat(PARAFORMAT2& pf)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		pf.cbSize = sizeof(PARAFORMAT2);
		return (BOOL)::SendMessage(m_hWnd, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
	}

	TEXTMODE GetTextMode() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (TEXTMODE)::SendMessage(m_hWnd, EM_GETTEXTMODE, 0, 0L);
	}
	BOOL SetTextMode(TEXTMODE enumTextMode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return !(BOOL)::SendMessage(m_hWnd, EM_SETTEXTMODE, enumTextMode, 0L);
	}
	UNDONAMEID GetUndoName() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UNDONAMEID)::SendMessage(m_hWnd, EM_GETUNDONAME, 0, 0L);
	}
	UNDONAMEID GetRedoName() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UNDONAMEID)::SendMessage(m_hWnd, EM_GETREDONAME, 0, 0L);
	}
	BOOL CanRedo() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_CANREDO, 0, 0L);
	}
	BOOL Redo()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_REDO, 0, 0L);
	}
	BOOL EnableAutoURLDetect(BOOL bEnable = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return !(BOOL)::SendMessage(m_hWnd, EM_AUTOURLDETECT, bEnable, 0L);
	}
	UINT SetUndoLimit(UINT uUndoLimit)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, EM_SETUNDOLIMIT, uUndoLimit, 0L);
	}
	void StopGroupTyping()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_STOPGROUPTYPING, 0, 0L);
	}
#endif //(_RICHEDIT_VER >= 0x0200)
};

typedef CWinRichEditT<CWindow>		CWinRichEdit;

#endif //!UNDER_CE

/////////////////////////////////////////////////////////////////////////////
// CWinDragListBox

template <class Base>
class CWinDragListBoxT : public CWinListBoxT< Base >
{
public:
// Constructors
	CWinDragListBoxT(HWND hWnd = NULL) : CWinListBoxT< Base >(hWnd) { }

	CWinDragListBoxT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		HWND hWnd = CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		HWND hWnd = CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
		if(hWnd != NULL)
			MakeDragList();
		return hWnd;
	}

// Operations
	BOOL MakeDragList()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(!(GetStyle() & LBS_MULTIPLESEL));
		return ::MakeDragList(m_hWnd);
	}

	int LBItemFromPt(POINT pt, BOOL bAutoScroll = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::LBItemFromPt(m_hWnd, pt, bAutoScroll);
	}

	void DrawInsert(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::DrawInsert(GetParent(), m_hWnd, nItem);
	}

	static UINT GetDragListMessage()
	{
		static UINT m_uDragListMessage = 0;
		if(m_uDragListMessage == 0)
		{
			_pModule->EnterStaticDataCriticalSection();
			if(m_uDragListMessage == 0)
				m_uDragListMessage = ::RegisterWindowMessage(DRAGLISTMSGSTRING);
			_pModule->LeaveStaticDataCriticalSection();
		}
		ATLASSERT(m_uDragListMessage != 0);
		return m_uDragListMessage;
	}
};

typedef CWinDragListBoxT<CWindow>	CWinDragListBox;

template <class T>
class CWinDragListNotifyImpl
{
public:
	BEGIN_MSG_MAP(CWinDragListNotifyImpl< T >)
		MESSAGE_HANDLER(CWinDragListBox::GetDragListMessage(), OnDragListNotify)
	END_MSG_MAP()

	LRESULT OnDragListNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		uMsg;	// avoid level 4 warning
		ATLASSERT(uMsg == CWinDragListBox::GetDragListMessage());
		T* pT = static_cast<T*>(this);
		LPDRAGLISTINFO lpDragListInfo = (LPDRAGLISTINFO)lParam;
		LRESULT lRet = 0;
		switch(lpDragListInfo->uNotification)
		{
		case DL_BEGINDRAG:
			lRet = (LPARAM)pT->OnBeginDrag((int)wParam, lpDragListInfo->hWnd, lpDragListInfo->ptCursor);
			break;
		case DL_CANCELDRAG:
			pT->OnCancelDrag((int)wParam, lpDragListInfo->hWnd, lpDragListInfo->ptCursor);
			break;
		case DL_DRAGGING:
			lRet = (LPARAM)pT->OnDragging((int)wParam, lpDragListInfo->hWnd, lpDragListInfo->ptCursor);
			break;
		case DL_DROPPED:
			pT->OnDropped((int)wParam, lpDragListInfo->hWnd, lpDragListInfo->ptCursor);
			break;
		default:
			ATLTRACE2(atlTraceWindowing, 0, _T("Unknown DragListBox notification\n"));
			bHandled = FALSE;	// don't handle it
			break;
		}
		return lRet;
	}

// Overrideables
	BOOL OnBeginDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT /*ptCursor*/)
	{
		return TRUE;	// allow dragging
	}
	void OnCancelDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT /*ptCursor*/)
	{
		// nothing to do
	}
	int OnDragging(int /*nCtlID*/, HWND /*hWndDragList*/, POINT /*ptCursor*/)
	{
		return 0;	// don't change cursor
	}
	void OnDropped(int /*nCtlID*/, HWND /*hWndDragList*/, POINT /*ptCursor*/)
	{
		// nothing to do
	}
};


// --- New Windows Common Controls ---

/////////////////////////////////////////////////////////////////////////////
// CWinReBar

template <class Base>
class CWinReBarT : public Base
{
public:
// Constructors
	CWinReBarT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinReBarT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return REBARCLASSNAME;
	}

#if (_WIN32_IE >= 0x0400)
	COLORREF GetTextColor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, RB_GETTEXTCOLOR, 0, 0L);
	}
	COLORREF SetTextColor(COLORREF clr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, RB_SETTEXTCOLOR, 0, (LPARAM)clr);
	}
	COLORREF GetBkColor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, RB_GETBKCOLOR, 0, 0L);
	}
	COLORREF SetBkColor(COLORREF clr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, RB_SETBKCOLOR, 0, (LPARAM)clr);
	}
#endif //(_WIN32_IE >= 0x0400)
	UINT GetBandCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, RB_GETBANDCOUNT, 0, 0L);
	}
	BOOL GetBandInfo(int nBand, LPREBARBANDINFO lprbbi) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, RB_GETBANDINFO, nBand, (LPARAM)lprbbi);
	}
	BOOL SetBandInfo(int nBand, LPREBARBANDINFO lprbbi)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, RB_SETBANDINFO, nBand, (LPARAM)lprbbi);
	}
	BOOL GetBarInfo(LPREBARINFO lprbi) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, RB_GETBARINFO, 0, (LPARAM)lprbi);
	}
	BOOL SetBarInfo(LPREBARINFO lprbi)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, RB_SETBARINFO, 0, (LPARAM)lprbi);
	}
#if (_WIN32_IE >= 0x0400)
	UINT GetBarHeight() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, RB_GETBARHEIGHT, 0, 0L);
	}
	BOOL GetRect(int nBand, LPRECT lpRect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, RB_GETRECT, nBand, (LPARAM)lpRect);
	}
#endif //(_WIN32_IE >= 0x0400)
	UINT GetRowCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, RB_GETROWCOUNT, 0, 0L);
	}
	UINT GetRowHeight(int nBand) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, RB_GETROWHEIGHT, nBand, 0L);
	}
#if (_WIN32_IE >= 0x0400)
#ifndef UNDER_CE
	CWinToolTip GetToolTips() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinToolTip((HWND)::SendMessage(m_hWnd, RB_GETTOOLTIPS, 0, 0L));
	}
#endif // UNDER_CE
	void SetToolTips(HWND hwndToolTip)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, RB_SETTOOLTIPS, (WPARAM)hwndToolTip, 0L);
	}
#endif //(_WIN32_IE >= 0x0400)

// Operations
#if (_WIN32_IE >= 0x0400)
	void BeginDrag(int nBand, DWORD dwPos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, RB_BEGINDRAG, nBand, dwPos);
	}
	void BeginDrag(int nBand, int xPos, int yPos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, RB_BEGINDRAG, nBand, MAKELPARAM(xPos, yPos));
	}
	void EndDrag()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, RB_ENDDRAG, 0, 0L);
	}
	void DragMove(DWORD dwPos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, RB_DRAGMOVE, 0, dwPos);
	}
	void DragMove(int xPos, int yPos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, RB_DRAGMOVE, 0, MAKELPARAM(xPos, yPos));
	}
	void GetDropTarget(IDropTarget** ppDropTarget) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, RB_GETDROPTARGET, 0, (LPARAM)ppDropTarget);
	}
#endif //(_WIN32_IE >= 0x0400)
	BOOL InsertBand(int nBand, LPREBARBANDINFO lprbbi)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		lprbbi->cbSize = sizeof(REBARBANDINFO);
		return (BOOL)::SendMessage(m_hWnd, RB_INSERTBAND, nBand, (LPARAM)lprbbi);
	}
	BOOL DeleteBand(int nBand)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, RB_DELETEBAND, nBand, 0L);
	}
#if (_WIN32_IE >= 0x0400)
	void MaximizeBand(int nBand)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, RB_MAXIMIZEBAND, nBand, 0L);
	}
	void MinimizeBand(int nBand)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, RB_MINIMIZEBAND, nBand, 0L);
	}
	BOOL SizeToRect(LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, RB_SIZETORECT, 0, (LPARAM)lpRect);
	}
#endif //(_WIN32_IE >= 0x0400)
	CWindow SetNotifyWnd(HWND hWnd)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWindow((HWND)::SendMessage(m_hWnd, RB_SETPARENT, (WPARAM)hWnd, 0L));
	}
#if (_WIN32_IE >= 0x0400)
	int IdToIndex(UINT uBandID) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, RB_IDTOINDEX, uBandID, 0L);
	}
	int HitTest(LPRBHITTESTINFO lprbht) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, RB_HITTEST, 0, (LPARAM)lprbht);
	}
	BOOL ShowBand(int nBand, BOOL bShow)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, RB_SHOWBAND, nBand, bShow);
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CWinReBarT<CWindow>		CWinReBar;

/////////////////////////////////////////////////////////////////////////////
// CWinComboBoxEx

template <class Base>
class CWinComboBoxExT : public CWinComboBoxT< Base >
{
public:
// Constructors
	CWinComboBoxExT(HWND hWnd = NULL) : CWinComboBoxT< Base >(hWnd) { }

	CWinComboBoxExT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_COMBOBOXEX;
	}

#if (_WIN32_IE >= 0x0400)
	DWORD GetExtendedStyle() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, CBEM_GETEXTENDEDSTYLE, 0, 0L);
	}
	DWORD SetExtendedStyle(DWORD dwExMask, DWORD dwExStyle)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, CBEM_SETEXTENDEDSTYLE, dwExMask, dwExStyle);
	}
#endif //(_WIN32_IE >= 0x0400)
	CWinImageList GetImageList() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, CBEM_GETIMAGELIST, 0, 0L));
	}
	CWinImageList SetImageList(HIMAGELIST hImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinImageList((HIMAGELIST)::SendMessage(m_hWnd, CBEM_SETIMAGELIST, 0, (LPARAM)hImageList));
	}

// Operations
#ifndef UNDER_CE
	int InsertItem(const COMBOBOXEXITEM FAR* lpcCBItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CBEM_INSERTITEM, 0, (LPARAM)lpcCBItem);
	}
#endif // UNDER_CE
	int DeleteItem(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CBEM_DELETEITEM, nIndex, 0L);
	}
#ifndef UNDER_CE
	BOOL GetItem(PCOMBOBOXEXITEM pCBItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBEM_GETITEM, 0, (LPARAM)pCBItem);
	}
#endif // UNDER_CE
#ifndef UNDER_CE
	BOOL SetItem(const COMBOBOXEXITEM FAR* lpcCBItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBEM_SETITEM, 0, (LPARAM)lpcCBItem);
	}
#endif // UNDER_CE
	CWinComboBox GetComboCtrl() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinComboBox((HWND)::SendMessage(m_hWnd, CBEM_GETCOMBOCONTROL, 0, 0L));
	}
	CWinEdit GetEditCtrl() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinEdit((HWND)::SendMessage(m_hWnd, CBEM_GETEDITCONTROL, 0, 0L));
	}
	BOOL HasEditChanged() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBEM_HASEDITCHANGED, 0, 0L);
	}
};

typedef CWinComboBoxExT<CWindow>		CWinComboBoxEx;

/////////////////////////////////////////////////////////////////////////////
// CWinMonthCalendar

template <class Base>
class CWinMonthCalendarT : public Base
{
public:
// Constructors
	CWinMonthCalendarT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinMonthCalendarT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return MONTHCAL_CLASS;
	}

	COLORREF GetColor(int nColorType) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, MCM_GETCOLOR, nColorType, 0L);
	}
	COLORREF SetColor(int nColorType, COLORREF clr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, MCM_SETCOLOR, nColorType, clr);
	}
	BOOL GetCurSel(LPSYSTEMTIME lpSysTime) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, MCM_GETCURSEL, 0, (LPARAM)lpSysTime);
	}
	BOOL SetCurSel(LPSYSTEMTIME lpSysTime)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, MCM_SETCURSEL, 0, (LPARAM)lpSysTime);
	}
	int GetFirstDayOfWeek(BOOL* pbLocaleVal = NULL) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		DWORD dwRet = ::SendMessage(m_hWnd, MCM_GETFIRSTDAYOFWEEK, 0, 0L);
		if(pbLocaleVal != NULL)
			*pbLocaleVal = HIWORD(dwRet);
		return (int)LOWORD(dwRet);
	}
	int SetFirstDayOfWeek(int nDay, BOOL* pbLocaleVal = NULL)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		DWORD dwRet = ::SendMessage(m_hWnd, MCM_SETFIRSTDAYOFWEEK, 0, nDay);
		if(pbLocaleVal != NULL)
			*pbLocaleVal = HIWORD(dwRet);
		return (int)LOWORD(dwRet);
	}
	int GetMaxSelCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, MCM_GETMAXSELCOUNT, 0, 0L);
	}
	BOOL SetMaxSelCount(int nMax)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, MCM_SETMAXSELCOUNT, nMax, 0L);
	}
	int GetMonthDelta() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, MCM_GETMONTHDELTA, 0, 0L);
	}
	int SetMonthDelta(int nDelta)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, MCM_SETMONTHDELTA, nDelta, 0L);
	}
	DWORD GetRange(LPSYSTEMTIME lprgSysTimeArray) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, MCM_GETRANGE, 0, (LPARAM)lprgSysTimeArray);
	}
	BOOL SetRange(DWORD dwFlags, LPSYSTEMTIME lprgSysTimeArray)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, MCM_SETRANGE, dwFlags, (LPARAM)lprgSysTimeArray);
	}
	BOOL GetSelRange(LPSYSTEMTIME lprgSysTimeArray) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, MCM_GETSELRANGE, 0, (LPARAM)lprgSysTimeArray);
	}
	BOOL SetSelRange(LPSYSTEMTIME lprgSysTimeArray)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, MCM_SETSELRANGE, 0, (LPARAM)lprgSysTimeArray);
	}
	BOOL GetToday(LPSYSTEMTIME lpSysTime) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, MCM_GETTODAY, 0, (LPARAM)lpSysTime);
	}
	void SetToday(LPSYSTEMTIME lpSysTime)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, MCM_SETTODAY, 0, (LPARAM)lpSysTime);
	}
	BOOL GetMinReqRect(LPRECT lpRectInfo) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, MCM_GETMINREQRECT, 0, (LPARAM)lpRectInfo);
	}

// Operations
	int GetMonthRange(DWORD dwFlags, LPSYSTEMTIME lprgSysTimeArray) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, MCM_GETMONTHRANGE, dwFlags, (LPARAM)lprgSysTimeArray);
	}
	BOOL SetDayState(int nMonths, LPMONTHDAYSTATE lpDayStateArray)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, MCM_SETDAYSTATE, nMonths, (LPARAM)lpDayStateArray);
	}
	DWORD HitTest(PMCHITTESTINFO pMCHitTest) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, MCM_HITTEST, 0, (LPARAM)pMCHitTest);
	}
};

typedef CWinMonthCalendarT<CWindow>		CWinMonthCalendar;

/////////////////////////////////////////////////////////////////////////////
// CWinDateTimePicker

template <class Base>
class CWinDateTimePickerT : public Base
{
public:
// Constructors
	CWinDateTimePickerT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinDateTimePickerT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Operations
	static LPCTSTR GetWndClassName()
	{
		return DATETIMEPICK_CLASS;
	}

	BOOL SetFormat(LPTSTR lpszFormat)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, DTM_SETFORMAT, 0, (LPARAM)lpszFormat);
	}
	COLORREF GetMonthCalColor(int nColorType) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, DTM_GETMCCOLOR, nColorType, 0L);
	}
	COLORREF SetMonthCalColor(int nColorType, COLORREF clr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, DTM_SETMCCOLOR, nColorType, clr);
	}
#if (_WIN32_IE >= 0x0400)
	CWinFont GetMonthCalFont() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinFont((HFONT)::SendMessage(m_hWnd, DTM_GETMCFONT, 0, 0L));
	}
	void SetMonthCalFont(HFONT hFont, BOOL bRedraw = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_SETMCFONT, (WPARAM)hFont, MAKELPARAM(bRedraw, 0));
	}
#endif //(_WIN32_IE >= 0x0400)
	DWORD GetRange(LPSYSTEMTIME lpSysTimeArray) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, DTM_GETRANGE, 0, (LPARAM)lpSysTimeArray);
	}
	BOOL SetRange(DWORD dwFlags, LPSYSTEMTIME lpSysTimeArray)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, DTM_SETRANGE, dwFlags, (LPARAM)lpSysTimeArray);
	}
	DWORD GetSystemTime(LPSYSTEMTIME lpSysTime) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)lpSysTime);
	}
	BOOL SetSystemTime(DWORD dwFlags, LPSYSTEMTIME lpSysTime)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, DTM_SETSYSTEMTIME, dwFlags, (LPARAM)lpSysTime);
	}
	CWinMonthCalendar GetMonthCal() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWinMonthCalendar((HWND)::SendMessage(m_hWnd, DTM_GETMONTHCAL, 0, 0L));
	}
};

typedef CWinDateTimePickerT<CWindow>		CWinDateTimePicker;

/////////////////////////////////////////////////////////////////////////////
// CWinFlatScrollBar

#if (_WIN32_IE >= 0x0400)

template <class Base>
class CWinFlatScrollBarT : public Base
{
public:
// Constructors
	CWinFlatScrollBarT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinFlatScrollBarT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

// Initialization
	BOOL FlatSB_Initialize()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::InitializeFlatSB(m_hWnd);
	}
	HRESULT FlatSB_Uninitialize()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::UninitializeFlatSB(m_hWnd);
	}

// Attributes
	BOOL FlatSB_GetScrollProp(UINT uIndex, LPINT lpnValue) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::FlatSB_GetScrollProp(m_hWnd, uIndex, lpnValue);
	}
	BOOL FlatSB_SetScrollProp(UINT uIndex, int nValue, BOOL bRedraw = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::FlatSB_SetScrollProp(m_hWnd, uIndex, nValue, bRedraw);
	}

	int FlatSB_GetScrollPos(int nBar) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::FlatSB_GetScrollPos(m_hWnd, nBar);
	}

	int FlatSB_SetScrollPos(int nBar, int nPos, BOOL bRedraw = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::FlatSB_SetScrollPos(m_hWnd, nBar, nPos, bRedraw);
	}

	BOOL FlatSB_GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::FlatSB_GetScrollRange(m_hWnd, nBar, lpMinPos, lpMaxPos);
	}

	BOOL FlatSB_SetScrollRange(int nBar, int nMinPos, int nMaxPos, BOOL bRedraw = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::FlatSB_SetScrollRange(m_hWnd, nBar, nMinPos, nMaxPos, bRedraw);
	}

	BOOL FlatSB_GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::FlatSB_GetScrollInfo(m_hWnd, nBar, lpScrollInfo);
	}

	int FlatSB_SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::FlatSB_SetScrollInfo(m_hWnd, nBar, lpScrollInfo, bRedraw);
	}

// Operations
	BOOL FlatSB_ShowScrollBar(UINT nBar, BOOL bShow = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::FlatSB_ShowScrollBar(m_hWnd, nBar, bShow);
	}

	BOOL FlatSB_EnableScrollBar(UINT uSBFlags, UINT uArrowFlags = ESB_ENABLE_BOTH)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::FlatSB_EnableScrollBar(m_hWnd, uSBFlags, uArrowFlags);
	}
};

typedef CWinFlatScrollBarT<CWindow>	CWinFlatScrollBar;

#endif //(_WIN32_IE >= 0x0400)

/////////////////////////////////////////////////////////////////////////////
// CWinIPAddress

#if (_WIN32_IE >= 0x0400)

template <class Base>
class CWinIPAddressT : public Base
{
public:
// Constructors
	CWinIPAddressT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinIPAddressT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Atteributes
	static LPCTSTR GetWndClassName()
	{
		return WC_IPADDRESS;
	}

	BOOL IsBlank() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, IPM_ISBLANK, 0, 0L);
	}
	int GetAddress(LPDWORD lpdwAddress) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM)lpdwAddress);
	}
	void SetAddress(DWORD dwAddress)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, IPM_SETADDRESS, 0, dwAddress);
	}
	void ClearAddress()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, IPM_CLEARADDRESS, 0, 0L);
	}
	void SetRange(int nField, WORD wRange)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, IPM_SETRANGE, nField, wRange);
	}
	void SetRange(int nField, BYTE nMin, BYTE nMax)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, IPM_SETRANGE, nField, MAKEIPRANGE(nMin, nMax));
	}
	void SetFocus(int nField)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, IPM_SETFOCUS, nField, 0L);
	}
};

typedef CWinIPAddressT<CWindow>		CWinIPAddress;

#endif //(_WIN32_IE >= 0x0400)

/////////////////////////////////////////////////////////////////////////////
// CWinPager

#if (_WIN32_IE >= 0x0400)

template <class Base>
class CWinPagerT : public Base
{
public:
// Constructors
	CWinPagerT(HWND hWnd = NULL) : Base(hWnd) { }

	CWinPagerT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
#if (_ATL_VER >= 0x0400)
		return CWindow::Create(GetWndClassName(), hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
#else
		return CWindow::Create(GetWndClassName(), hWndParent, *rect.m_lpRect, szWindowName, dwStyle, dwExStyle, (UINT)MenuOrID.m_hMenu, lpCreateParam);
#endif //(_ATL_VER >= 0x0400)
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_PAGESCROLLER;
	}

	int GetButtonSize()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, PGM_GETBUTTONSIZE, 0, 0L);
	}
	int SetButtonSize(int nButtonSize)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, PGM_SETBUTTONSIZE, 0, nButtonSize);
	}
	DWORD GetButtonState(int nButton)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nButton == PGB_TOPORLEFT || nButton == PGB_BOTTOMORRIGHT);
		return (DWORD)::SendMessage(m_hWnd, PGM_GETBUTTONSTATE, 0, nButton);
	}
	COLORREF GetBkColor()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, PGM_GETBKCOLOR, 0, 0L);
	}
	COLORREF SetBkColor(COLORREF clrBk)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, PGM_SETBKCOLOR, 0, (LPARAM)clrBk);
	}
	int GetBorder()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, PGM_GETBORDER, 0, 0L);
	}
	int SetBorder(int nBorderSize)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, PGM_SETBORDER, 0, nBorderSize);
	}
	int GetPos()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, PGM_GETPOS, 0, 0L);
	}
	int SetPos(int nPos)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, PGM_SETPOS, 0, nPos);
	}

// Operations
	void SetChild(HWND hWndChild)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PGM_SETCHILD, 0, (LPARAM)hWndChild);
	}
	void ForwardMouse(BOOL bForward = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PGM_FORWARDMOUSE, bForward, 0L);
	}
	void RecalcSize()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PGM_RECALCSIZE, 0, 0L);
	}
	void GetDropTarget(IDropTarget** ppDropTarget)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(ppDropTarget != NULL);
		::SendMessage(m_hWnd, PGM_GETDROPTARGET, 0, (LPARAM)ppDropTarget);
	}
};

typedef CWinPagerT<CWindow>		CWinPager;

#endif //(_WIN32_IE >= 0x0400)

/////////////////////////////////////////////////////////////////////////////
// CCustomDraw - MI class for custom-draw support

template <class T>
class CCustomDraw
{
public:
	BEGIN_MSG_MAP(CCustomDraw< T >)
		NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnCustomDraw)
	ALT_MSG_MAP(1)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnCustomDraw)
	END_MSG_MAP()

// message handler
	LRESULT OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		LPNMCUSTOMDRAW lpNMCustomDraw = (LPNMCUSTOMDRAW)pnmh;
		DWORD dwRet = 0;
		switch(lpNMCustomDraw->dwDrawStage)
		{
		case CDDS_PREPAINT:
			dwRet = pT->OnPrePaint(idCtrl, lpNMCustomDraw);
			break;
		case CDDS_POSTPAINT:
			dwRet = pT->OnPostPaint(idCtrl, lpNMCustomDraw);
			break;
		case CDDS_PREERASE:
			dwRet = pT->OnPreErase(idCtrl, lpNMCustomDraw);
			break;
		case CDDS_POSTERASE:
			dwRet = pT->OnPostErase(idCtrl, lpNMCustomDraw);
			break;
		case CDDS_ITEMPREPAINT:
			dwRet = pT->OnItemPrePaint(idCtrl, lpNMCustomDraw);
			break;
		case CDDS_ITEMPOSTPAINT:
			dwRet = pT->OnItemPostPaint(idCtrl, lpNMCustomDraw);
			break;
		case CDDS_ITEMPREERASE:
			dwRet = pT->OnItemPreErase(idCtrl, lpNMCustomDraw);
			break;
		case CDDS_ITEMPOSTERASE:
			dwRet = pT->OnItemPostErase(idCtrl, lpNMCustomDraw);
			break;
		default:
			bHandled = FALSE;
			break;
		}
		return dwRet;
	}

// overrideables
	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}
	DWORD OnPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}
	DWORD OnPreErase(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}
	DWORD OnPostErase(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}
	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}
	DWORD OnItemPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}
	DWORD OnItemPreErase(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}
	DWORD OnItemPostErase(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}
};


// --- Windows CE common controls ---

#ifdef UNDER_CE

/////////////////////////////////////////////////////////////////////////////
// CCommandBar

template <class Base>
class CCommandBarT : public Base
{
public:
// Constructors
	CCommandBarT(HWND hWnd = NULL) : Base(hWnd) { }

	CCommandBarT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

// Attributes
	BOOL IsVisible() const
	{
		return IsWindowVisible();
	}
	int GetHeight() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_Height(m_hWnd);
	}
	HMENU GetMenu(int nButton) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_GetMenu(m_hWnd, nButton);
	}

// Operations
	HWND Create(HWND hWndParent, int nCmdBarID)
	{
		ATLASSERT(!::IsWindow(m_hWnd));
		return CommandBar_Create(_pModule->GetModuleInstance(), hWndParent, nCmdBarID);
	}
	void Destroy()
	{
		DestroyWindow();
	}
	BOOL Show(BOOL bShow = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_Show(m_hWnd, bShow);
	}
	BOOL DrawMenuBar(int nButton)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_DrawMenuBar(m_hWnd, nButton);
	}

	BOOL AddAdornments(DWORD dwFlags = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_AddAdornments(m_hWnd, dwFlags, 0);
	}
	int AddBitmap(int nBitmapID, int nNumImages)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_AddBitmap(m_hWnd, _AtlBaseModule.GetResourceInstance(), nBitmapID, nNumImages, 0, 0);
	}
	BOOL AddButtons(UINT uNumButtons, LPTBBUTTON lpButtons)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_AddButtons(m_hWnd, uNumButtons, lpButtons);
	}
	BOOL AddToolTips(UINT uNumToolTips, LPTSTR lpToolTips)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_AddToolTips(m_hWnd, uNumToolTips, lpToolTips);
	}
	BOOL InsertButton(int nButton, LPTBBUTTON lpButton)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_InsertButton(m_hWnd, nButton, lpButton);
	}
	HWND InsertComboBox(int nWidth, UINT dwStyle, WORD wComboBoxID, int nButton)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_InsertComboBox(m_hWnd, _pModule->GetModuleInstance(), nWidth, dwStyle, wComboBoxID, nButton);
	}
	BOOL InsertMenubar(WORD wMenuID, int nButton)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_InsertMenubar(m_hWnd, _AtlBaseModule.GetResourceInstance(), wMenuID, nButton);
	}
	BOOL InsertMenubarEx(WORD wMenuID, int nButton)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_InsertMenubarEx(m_hWnd, _AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(wMenuID), nButton);
	}
	BOOL InsertMenubarEx(LPTSTR pszMenu, int nButton)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBar_InsertMenubarEx(m_hWnd, _AtlBaseModule.GetResourceInstance(), pszMenu, nButton);
	}
};

typedef CCommandBarT<CWindow>	CCommandBar;

/////////////////////////////////////////////////////////////////////////////
// CCommandBands

template <class Base>
class CCommandBandsT : public Base
{
public:
// Constructors
	CCommandBandsT(HWND hWnd = NULL) : Base(hWnd) { }

	CCommandBandsT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

// Attributes
	BOOL IsVisible() const
	{
		return IsWindowVisible();
	}
#if (_WIN32_IE >= 0x0400)
	UINT GetHeight() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBands_Height(m_hWnd);
	}
#endif //(_WIN32_IE >= 0x0400)
	HWND GetCommandBar(UINT uBand) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBands_GetCommandBar(m_hWnd, uBand);
	}
	BOOL GetRestoreInformation(UINT uBand, LPCOMMANDBANDSRESTOREINFO pcbr) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBands_GetRestoreInformation(m_hWnd, uBand, pcbr);
	}

// Operations
	HWND Create(HWND hWndParent, UINT wID, DWORD dwStyles, HIMAGELIST hImageList = NULL)
	{
		ATLASSERT(!::IsWindow(m_hWnd));
		return CommandBands_Create(_pModule->GetModuleInstance(), hWndParent, wID, dwStyles, hImageList);
	}
	BOOL AddAdornments(DWORD dwFlags = 0, LPREBARBANDINFO prbbi = NULL)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBands_AddAdornments(m_hWnd, _pModule->GetModuleInstance(), dwFlags, prbbi);
	}
	BOOL AddBands(UINT uBandCount, LPREBARBANDINFO prbbi)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBands_AddBands(m_hWnd, _pModule->GetModuleInstance(), uBandCount, prbbi);
	}
	BOOL Show(BOOL bShow = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CommandBands_Show(m_hWnd, bShow);
	}
};

typedef CCommandBandsT<CWindow>	CCommandBands;

#endif //UNDER_CE

}; //namespace ATL

#endif // __ATLCTRLS_H__
