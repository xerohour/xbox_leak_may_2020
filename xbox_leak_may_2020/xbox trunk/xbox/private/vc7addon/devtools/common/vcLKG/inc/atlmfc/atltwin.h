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

#ifndef __ATLTWIN_H__
#define __ATLTWIN_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <vcctrls.h>

namespace ATL
{

template <class TBase>
class CTabWindowT : public TBase
{
public:
// Constructors
	CTabWindowT(HWND hWnd = NULL) : TBase(hWnd) { }

	CTabWindowT< TBase >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

// Operations
	HWND CreateTabWnd(HWND hWndParent, DWORD dwStyle = 0)
	{
		m_hWnd = CreateTabWindow(hWndParent, dwStyle);
		return m_hWnd;
	}

	LONG AddChild(HWND hWnd)
	{
		return SendMessage(TBWM_ADDCHILD, (WPARAM) hWnd);
	}

	LONG RemoveChild(HWND hWnd)
	{
		return SendMessage(TBWM_REMOVECHILD, (WPARAM) hWnd);
	}

	LONG CanAddChild(HWND hWnd)
	{
		return SendMessage(TBWM_CANADDCHILD, hWnd);
	}

	void GetChildRect(RECT *pRect)
	{
		SendMessage(TBWM_GETCHILDRECT, 0, (LPARAM) pRect);
	}
};

typedef CTabWindowT<CWindow> CTabWindow;

} // namespace ATL

#endif // __ATLTWIN_H__