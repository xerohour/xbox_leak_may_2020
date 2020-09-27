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

#ifndef __ATLCTRLX_H__
#define __ATLCTRLX_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLWIN_H__
	#error atlctrlx.h requires atlwin.h to be included first
#endif

#include <commctrl.h>

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

namespace ATL
{

/////////////////////////////////////////////////////////////////////////////
// Forward declarations

template <class T, class TBase = CWinButton, class TWinTraits = CWinBitmapButtonTraits> class CWinBitmapButtonImpl;
class CWinBitmapButton;
template <class T, class TBase = CWinListView, class TWinTraits = CWinCheckListViewTraits> class CWinCheckListViewImpl;
class CWinCheckListView;
template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits> class CWinHyperLinkImpl;
class CWinHyperLink;
class CWinWaitCursor;
template <class T, class TBase = CWinStatusBar> class CWinMultiPaneStatusBarImpl;
class CWinMultiPaneStatusBar;


/////////////////////////////////////////////////////////////////////////////
// CWinBitmapButton - bitmap button implementation

template <DWORD t_dwStyle, DWORD t_dwExStyle, UINT t_uTimerInterval>
class CWinBitmapButtonImplTraits
{
public:
	static DWORD GetWndStyle(DWORD dwStyle)
	{
		return dwStyle == 0 ? t_dwStyle : dwStyle;
	}
	static DWORD GetWndExStyle(DWORD dwExStyle)
	{
		return dwExStyle == 0 ? t_dwExStyle : dwExStyle;
	}
	static UINT GetTimerInterval()
	{
		return t_uTimerInterval;
	}
};

typedef CWinBitmapButtonImplTraits<WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, 100>	CWinBitmapButtonTraits;

template <class T, class TBase = CWinButton, class TWinTraits = CWinBitmapButtonTraits>
class CWinBitmapButtonImpl : public CWindowImpl< T, TBase, TWinTraits>
{
public:
	enum
	{
		_nImageNormal = 0,
		_nImagePushed,
		_nImageFocusOrHover,
		_nImageDisabled,
		_nImageCount = 4,
		_nTimerID = 1
	};

	HIMAGELIST m_hImageList;
	int m_nImage[_nImageCount];

	unsigned m_bUseHover:1;
	unsigned m_bMouseOver:1;
	unsigned m_bTimer:1;

	DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

// Constructor/Destructor
	CWinBitmapButtonImpl(BOOL bUseHover = FALSE, HIMAGELIST hImageList = NULL) : 
			m_hImageList(hImageList), m_bUseHover(bUseHover), 
			m_bMouseOver(FALSE), m_bTimer(FALSE)
	{
		m_nImage[_nImageNormal] = -1;
		m_nImage[_nImagePushed] = -1;
		m_nImage[_nImageFocusOrHover] = -1;
		m_nImage[_nImageDisabled] = -1;
	}

	~CWinBitmapButtonImpl()
	{
		if(m_hImageList != NULL)
			::ImageList_Destroy(m_hImageList);
	}

// Attributes
	static UINT GetTimerInterval()
	{
		return TWinTraits::GetTimerInterval();
	}

	HIMAGELIST GetImageList() const
	{
		return m_hImageList;
	}
	HIMAGELIST SetImageList(HIMAGELIST hImageList)
	{
		HIMAGELIST hImageListPrev = m_hImageList;
		m_hImageList = hImageList;
		return hImageListPrev;
	}
	void SetImages(int nNormal, int nPushed = -1, int nFocusOrHover = -1, int nDisabled = -1)
	{
		if(nNormal != -1)
			m_nImage[_nImageNormal] = nNormal;
		if(nPushed != -1)
			m_nImage[_nImagePushed] = nPushed;
		if(nFocusOrHover != -1)
			m_nImage[_nImageFocusOrHover] = nFocusOrHover;
		if(nDisabled != -1)
			m_nImage[_nImageDisabled] = nDisabled;
	}
	BOOL GetHoverMode()
	{
		return m_bUseHover;
	}
	void SetHoverMode(BOOL bUseHover)
	{
		m_bUseHover = bUseHover;
	}

// Implementation
	typedef CWinBitmapButtonImpl< T, TBase, TWinTraits >	thisClass;
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->DrawItem((LPDRAWITEMSTRUCT)lParam);
		return (LRESULT)TRUE;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if(m_bUseHover)
		{
			POINT ptCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(&ptCursor);
			BOOL bUpdate = FALSE;
			if(::WindowFromPoint(ptCursor) == m_hWnd)
			{
				if(!m_bTimer)
				{
					if(SetTimer(_nTimerID, T::GetTimerInterval()) == _nTimerID)
						m_bTimer = TRUE;
				}
				if(!m_bMouseOver)
					bUpdate = TRUE;
				m_bMouseOver = TRUE;
			}
			else
			{
				if(m_bMouseOver)
					bUpdate = TRUE;
				m_bMouseOver = FALSE;
			}

			if(bUpdate)
			{
				Invalidate();
				UpdateWindow();
			}
		}

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(_nTimerID == (int)wParam)
		{
			POINT ptCursor;
			::GetCursorPos(&ptCursor);
			if(::WindowFromPoint(ptCursor) != m_hWnd)
			{
				KillTimer(_nTimerID);
				m_bTimer = FALSE;
				m_bMouseOver = FALSE;
				Invalidate();
				UpdateWindow();
			}
		}
		else
			bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_bTimer)
			KillTimer(_nTimerID);
		bHandled = FALSE;
		return 1;
	}

// Overrideables
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		ATLASSERT(m_hImageList != NULL);	// image list must be set
		ATLASSERT(m_nImage[0] != -1);	// main bitmap must be set

		// set bitmap according to the current button state
		int nImage = m_nImage[_nImageNormal];
		UINT state = lpDrawItemStruct->itemState;
		if((state & ODS_SELECTED) && m_nImage[_nImagePushed] != -1)
			nImage = m_nImage[_nImagePushed];
		else if(!m_bUseHover && (state & ODS_FOCUS) && m_nImage[_nImageFocusOrHover] != -1)
			nImage = m_nImage[_nImageFocusOrHover];
		else if(m_bUseHover && m_bMouseOver && m_nImage[_nImageFocusOrHover] != -1)
			nImage = m_nImage[_nImageFocusOrHover];
		else if((state & ODS_DISABLED) && m_nImage[_nImageDisabled] != -1)
			nImage = m_nImage[_nImageDisabled];

		// draw the whole button
		::ImageList_Draw(m_hImageList, nImage, lpDrawItemStruct->hDC, 0, 0, ILD_NORMAL);
	}
};

class CWinBitmapButton : public CWinBitmapButtonImpl<CWinBitmapButton>
{
public:
// nothing here
};


/////////////////////////////////////////////////////////////////////////////
// CCheckListCtrlView - list view control with check boxes

template <DWORD t_dwStyle, DWORD t_dwExStyle, DWORD t_dwExListViewStyle>
class CWinCheckListViewImplTraits
{
public:
	static DWORD GetWndStyle(DWORD dwStyle)
	{
		return dwStyle == 0 ? t_dwStyle : dwStyle;
	}
	static DWORD GetWndExStyle(DWORD dwExStyle)
	{
		return dwExStyle == 0 ? t_dwExStyle : dwExStyle;
	}
	static DWORD GetExtendedLVStyle()
	{
		return t_dwExListViewStyle;
	}
};

typedef CWinCheckListViewImplTraits<WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT>	CWinCheckListViewTraits;

template <class T, class TBase = CWinListView, class TWinTraits = CWinCheckListViewTraits>
class CWinCheckListViewImpl : public CWindowImpl<T, TBase, TWinTraits>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

// Attributes
	static DWORD GetExtendedLVStyle()
	{
		return TWinTraits::GetExtendedLVStyle();
	}

// Operations
	BOOL SubclassWindow(HWND hWnd)
	{
		BOOL bRet = CWindowImplBaseT< TBase, TWinTraits>::SubclassWindow(hWnd);
		if(bRet)
			SetExtendedListViewStyle(T::GetExtendedLVStyle());
		return bRet;
	}

	void CheckSelectedItems(int nCurrItem)
	{
		// first check if this item is selected
		LVITEM lvi;
		lvi.iItem = nCurrItem;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_STATE;
		lvi.stateMask = LVIS_SELECTED;
		GetItem(&lvi);
		// if item is not selected, don't do anything
		if(!(lvi.state & LVIS_SELECTED))
			return;
		// new check state will be reverse of the current state,
		BOOL bCheck = !GetCheckState(nCurrItem);
		int nItem = -1;
		int nOldItem = -1;
		while((nItem = GetNextItem(nOldItem, LVNI_SELECTED)) != -1)
		{
			if(nItem != nCurrItem)
				SetCheckState(nItem, bCheck);
			nOldItem = nItem;
		}
	}

// Implementation
	typedef CWinCheckListViewImpl< T, TBase, TWinTraits >	thisClass;
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// first let list view control initialize everything
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
		SetExtendedListViewStyle(T::GetExtendedLVStyle());
		return lRet;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		POINT ptMsg = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		LVHITTESTINFO lvh;
		lvh.pt = ptMsg;
		if(HitTest(&lvh) != -1 && lvh.flags == LVHT_ONITEMSTATEICON && ::GetKeyState(VK_CONTROL) >= 0)
			CheckSelectedItems(lvh.iItem);
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(wParam == VK_SPACE)
		{
			int nCurrItem = GetNextItem(-1, LVNI_FOCUSED);
			if(nCurrItem != -1  && ::GetKeyState(VK_CONTROL) >= 0)
				CheckSelectedItems(nCurrItem);
		}
		bHandled = FALSE;
		return 1;
	}
};

class CWinCheckListView : public CWinCheckListViewImpl<CWinCheckListView>
{
public:
// nothing here
};


/////////////////////////////////////////////////////////////////////////////
// CWinHyperLink - hyper link control implementation

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class CWinHyperLinkImpl : public CWindowImpl< T, TBase, TWinTraits>
{
public:
	struct _CursorData
	{
		enum { cxWidth = 32, cyHeight = 32 };
		int xHotSpot;
		int yHotSpot;
		unsigned char arrANDPlane[cxWidth*cyHeight/8];
		unsigned char arrXORPlane[cxWidth*cyHeight/8];
	};

	static const _CursorData m_CurData;
	static HCURSOR m_hCursor;

	LPTSTR m_lpstrLabel;
	LPTSTR m_lpstrHyperLink;
	HFONT m_hFont;
	RECT m_rcLink;
	bool m_bPaintLabel;
	CWinToolTip m_tip;

	DECLARE_WND_CLASS(_T("ATL_HyperLink"))

// Constructor/Destructor
	CWinHyperLinkImpl() : 
			m_lpstrLabel(NULL),
			m_lpstrHyperLink(NULL),
			m_hFont(NULL),
			m_bPaintLabel(true)
	{
		::SetRectEmpty(&m_rcLink);
	}

	~CWinHyperLinkImpl()
	{
		free(m_lpstrLabel);
		free(m_lpstrHyperLink);
		if(m_hFont != NULL)
			::DeleteObject(m_hFont);
	}

// Attributes
	BOOL GetLabel(LPTSTR lpstrBuffer, int nLength) const
	{
		if(m_lpstrLabel == NULL)
			return FALSE;
		ATLASSERT(lpstrBuffer != NULL);
		if(nLength > lstrlen(m_lpstrLabel) + 1)
		{
			lstrcpy(lpstrBuffer, m_lpstrLabel);
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetLabel(LPCTSTR lpstrLabel)
	{
		free(m_lpstrLabel);
		m_lpstrLabel = NULL;
		ATLTRY(m_lpstrLabel = (LPTSTR)malloc((lstrlen(lpstrLabel) + 1) * sizeof(TCHAR)));
		if(m_lpstrLabel == NULL)
			return FALSE;
		lstrcpy(m_lpstrLabel, lpstrLabel);
		CalcLabelRect();
		return TRUE;
	}

	BOOL GetHyperLink(LPTSTR lpstrBuffer, int nLength) const
	{
		if(m_lpstrHyperLink == NULL)
			return FALSE;
		ATLASSERT(lpstrBuffer != NULL);
		if(nLength > lstrlen(m_lpstrHyperLink) + 1)
		{
			lstrcpy(lpstrBuffer, m_lpstrHyperLink);
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetHyperLink(LPCTSTR lpstrLink)
	{
		free(m_lpstrHyperLink);
		m_lpstrHyperLink = NULL;
		ATLTRY(m_lpstrHyperLink = (LPTSTR)malloc((lstrlen(lpstrLink) + 1) * sizeof(TCHAR)));
		if(m_lpstrHyperLink == NULL)
			return FALSE;
		lstrcpy(m_lpstrHyperLink, lpstrLink);
		if(m_lpstrLabel == NULL)
			CalcLabelRect();
		return TRUE;
	}

// Operations
	BOOL SubclassWindow(HWND hWnd)
	{
		ATLASSERT(m_hWnd == NULL);
		ATLASSERT(::IsWindow(hWnd));
		TCHAR lpszBuffer[10];
		if(::GetClassName(hWnd, lpszBuffer, 10))
		{
			if(lstrcmpi(lpszBuffer, _T("static")) == 0)
			{
				CWindow wnd = hWnd;
				wnd.ModifyStyle(0, SS_NOTIFY);
				DWORD dwStyle = wnd.GetStyle() & 0x000000FF;
				if(dwStyle == SS_ICON || dwStyle == SS_BLACKRECT || dwStyle == SS_GRAYRECT || 
						dwStyle == SS_WHITERECT || dwStyle == SS_BLACKFRAME || dwStyle == SS_GRAYFRAME || 
						dwStyle == SS_WHITEFRAME || dwStyle == SS_OWNERDRAW || 
						dwStyle == SS_BITMAP || dwStyle == SS_ENHMETAFILE)
					m_bPaintLabel = false;
			}
		}
		BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
		if(bRet)
			Init();
		return bRet;
	}

	HINSTANCE Navigate()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		HINSTANCE hInst = NULL;
		if(m_lpstrHyperLink != NULL)
			hInst = ::ShellExecute(0, _T("open"), m_lpstrHyperLink, 0, 0, SW_SHOWNORMAL);
		return hInst;
	}

// Message map and handlers
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
//		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnGetDispInfo)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		Init();
		return 0;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_bPaintLabel)
		{
#if (_ATL_VER >= 0x0400)
			HBRUSH hBrush = (HBRUSH)GetParent().SendMessage(WM_CTLCOLORSTATIC, wParam, (LPARAM)m_hWnd);
#else
			HBRUSH hBrush = (HBRUSH)::SendMessage(GetParent(), WM_CTLCOLORSTATIC, wParam, (LPARAM)m_hWnd);
#endif //(_ATL_VER >= 0x0400)
			if(hBrush != NULL)
			{
				CWinDC dc = (HDC)wParam;
				RECT rect;
				GetClientRect(&rect);
				dc.FillRect(&rect, hBrush);
			}
		}
		else
		{
			bHandled = FALSE;
		}
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_bPaintLabel)
		{
			if(wParam != NULL)
			{
				DoPaint((HDC)wParam);
			}
			else
			{
				CWinPaintDC dc(m_hWnd);
				DoPaint(dc);
			}
		}
		else
		{
			bHandled = FALSE;
		}
		return 0;
	}

	LRESULT OnFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_bPaintLabel)
			Invalidate();
		else
			bHandled = FALSE;
		return 0;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if(m_lpstrHyperLink != NULL && ::PtInRect(&m_rcLink, pt))
			::SetCursor(m_hCursor);
		else
			bHandled = FALSE;
		return 0;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if(::PtInRect(&m_rcLink, pt))
		{
			SetFocus();
			SetCapture();
		}
		return 0;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if(GetCapture() == m_hWnd)
		{
			ReleaseCapture();
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if(::PtInRect(&m_rcLink, pt))
				Navigate();
		}
		return 0;
	}

	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		if(m_lpstrHyperLink != NULL && ::PtInRect(&m_rcLink, pt))
		{
			return TRUE;
		}
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(wParam == VK_RETURN || wParam == VK_SPACE)
			Navigate();
		return 0;
	}

	LRESULT OnGetDlgCode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return DLGC_WANTCHARS;
	}

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MSG msg;
		msg.hwnd = m_hWnd;
		msg.message = uMsg;
		msg.wParam = wParam;
		msg.lParam = lParam;
		if(m_tip.IsWindow())
			m_tip.RelayEvent(&msg);
		bHandled = FALSE;
		return 1;
	}

//	LRESULT OnGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
//	{
//		LPNMTTDISPINFO lpDispInfo = (LPNMTTDISPINFO)pnmh;
//		if(lpDispInfo->hdr.hwndFrom == m_tip.m_hWnd)
//		{
//			lpDispInfo->szText[0] = 0;
//			LPTSTR lpstrText = (m_lpstrHyperLink != NULL) ? m_lpstrHyperLink : _T("");
//			lstrcpyn(lpDispInfo->szText, lpstrText, sizeof(lpDispInfo->szText)/sizeof(lpDispInfo->szText[0]));
//		}
//		else
//		{
//			bHandled = FALSE;
//		}
//		return 0;
//	}

// Implementation
	bool Init()
	{
		ATLASSERT(::IsWindow(m_hWnd));

		// create or load a cursor
		if(m_hCursor == NULL)
		{
			_pModule->EnterStaticDataCriticalSection();
			if(m_hCursor == NULL)
#if(WINVER >= 0x0500)
				m_hCursor = ::LoadCursor(NULL, IDC_HAND);
#else
				m_hCursor = ::CreateCursor(_Module.GetResourceInstance(), m_CurData.xHotSpot, m_CurData.yHotSpot, m_CurData.cxWidth, m_CurData.cyHeight, m_CurData.arrANDPlane, m_CurData.arrXORPlane);
#endif //(WINVER >= 0x0500)
			_pModule->LeaveStaticDataCriticalSection();
			ATLASSERT(m_hCursor != NULL);
		}

		// set font
#if (_ATL_VER >= 0x0400)
		CWinFont font = GetParent().GetFont();
#else
		CWindow wnd = GetParent();
		CWinFont font = wnd.GetFont();
#endif //(_ATL_VER >= 0x0400)
		if(font.m_hFont != NULL)
		{
			LOGFONT lf;
			font.GetLogFont(&lf);
			lf.lfUnderline = TRUE;
			m_hFont = ::CreateFontIndirect(&lf);
		}

		// set label (defaults to window text)
		if(m_lpstrLabel == NULL)
		{
			int nLen = GetWindowTextLength();
			if(nLen > 0)
			{
				LPTSTR lpszText = (LPTSTR)_alloca((nLen+1)*sizeof(TCHAR));
				if(GetWindowText(lpszText, nLen+1))
					SetLabel(lpszText);
			}
		}

		// set hyperlink (defaults to label)
		if(m_lpstrHyperLink == NULL && m_lpstrLabel != NULL)
			SetHyperLink(m_lpstrLabel);

		CalcLabelRect();

		// create a tool tip
		m_tip.Create(m_hWnd);
		ATLASSERT(m_tip.IsWindow());
		m_tip.Activate(TRUE);
		m_tip.AddTool(m_hWnd, m_lpstrHyperLink);

		return true;
	}

	bool CalcLabelRect()
	{
		if(!::IsWindow(m_hWnd))
			return false;
		if(m_lpstrLabel == NULL && m_lpstrHyperLink == NULL)
			return false;

		CWinClientDC dc(m_hWnd);
		RECT rect;
		GetClientRect(&rect);
		m_rcLink = rect;
		if(m_bPaintLabel)
		{
			if(m_hFont != NULL)
				dc.SelectFont(m_hFont);
			LPTSTR lpstrText = (m_lpstrLabel != NULL) ? m_lpstrLabel : m_lpstrHyperLink;
			DWORD dwStyle = GetStyle();
			int nDrawStyle = DT_LEFT;
			if (dwStyle & SS_CENTER)
				nDrawStyle = DT_CENTER;
			else if (dwStyle & SS_RIGHT)
				nDrawStyle = DT_RIGHT;
			dc.DrawText(lpstrText, -1, &m_rcLink, nDrawStyle | DT_WORDBREAK | DT_CALCRECT);
		}

		return true;
	}

	void DoPaint(HDC hDC)
	{
		CWinDC dc = hDC;
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHT));
		if(m_hFont != NULL)
			dc.SelectFont(m_hFont);
		LPTSTR lpstrText = (m_lpstrLabel != NULL) ? m_lpstrLabel : m_lpstrHyperLink;
		DWORD dwStyle = GetStyle();
		int nDrawStyle = DT_LEFT;
		if (dwStyle & SS_CENTER)
			nDrawStyle = DT_CENTER;
		else if (dwStyle & SS_RIGHT)
			nDrawStyle = DT_RIGHT;
		dc.DrawText(lpstrText, -1, &m_rcLink, nDrawStyle | DT_WORDBREAK);
		if(GetFocus() == m_hWnd)
			dc.DrawFocusRect(&m_rcLink);
	}

// Kaos support
#ifdef USE_KAOS
	virtual LRESULT ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lResult = 0;
		if(!ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, lResult))
			lResult = DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		return lResult;
	}
#endif //USE_KAOS
};

template <class T, class TBase, class TWinTraits>
__declspec(selectany) const CWinHyperLinkImpl< T, TBase, TWinTraits>::_CursorData CWinHyperLinkImpl< T, TBase, TWinTraits>::m_CurData = 
{
	5, 0, 
	{
		0xF9, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 
		0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xF0, 0x07, 0xFF, 0xFF, 0xF0, 0x01, 0xFF, 0xFF, 
		0xF0, 0x00, 0xFF, 0xFF, 0x10, 0x00, 0x7F, 0xFF, 0x00, 0x00, 0x7F, 0xFF, 0x00, 0x00, 0x7F, 0xFF, 
		0x80, 0x00, 0x7F, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0xE0, 0x00, 0x7F, 0xFF, 
		0xE0, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0xF8, 0x01, 0xFF, 0xFF, 
		0xF8, 0x01, 0xFF, 0xFF, 0xF8, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	},
	{
		0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
		0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0xC0, 0x00, 0x00, 0x06, 0xD8, 0x00, 0x00, 
		0x06, 0xDA, 0x00, 0x00, 0x06, 0xDB, 0x00, 0x00, 0x67, 0xFB, 0x00, 0x00, 0x77, 0xFF, 0x00, 0x00, 
		0x37, 0xFF, 0x00, 0x00, 0x17, 0xFF, 0x00, 0x00, 0x1F, 0xFF, 0x00, 0x00, 0x0F, 0xFF, 0x00, 0x00, 
		0x0F, 0xFE, 0x00, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x03, 0xFC, 0x00, 0x00, 
		0x03, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	}
};

template <class T, class TBase, class TWinTraits>
__declspec(selectany) HCURSOR CWinHyperLinkImpl< T, TBase, TWinTraits>::m_hCursor = NULL;


class CWinHyperLink : public CWinHyperLinkImpl<CWinHyperLink>
{
public:
// nothing here
};


/////////////////////////////////////////////////////////////////////////////
// CWinWaitCursor - displays a wait cursor

class CWinWaitCursor
{
public:
// Data
	HCURSOR m_hWaitCursor;
	HCURSOR m_hOldCursor;
	bool m_bInUse;

// Constructor/destructor
	CWinWaitCursor(bool bSet = true, LPCTSTR lpstrCursor = IDC_WAIT, bool bSys = true) : m_hOldCursor(NULL), m_bInUse(false)
	{
		HINSTANCE hInstance = bSys ? NULL : _Module.GetResourceInstance();
		m_hWaitCursor = ::LoadCursor(hInstance, lpstrCursor);
		ATLASSERT(m_hWaitCursor != NULL);

		if(bSet)
			Set();
	}

	~CWinWaitCursor()
	{
		Restore();
	}

// Methods
	bool Set()
	{
		if(m_bInUse)
			return false;
		m_hOldCursor = ::SetCursor(m_hWaitCursor);
		m_bInUse = true;
		return true;
	}

	bool Restore()
	{
		if(!m_bInUse)
			return false;
		::SetCursor(m_hOldCursor);
		m_bInUse = false;
		return true;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CWinMultiPaneStatusBar - Status Bar with multiple panes

#define ID_DEFAULT_PANE		0       // default status bar pane

template <class T, class TBase = CWinStatusBar>
class CWinMultiPaneStatusBarImpl : public CWindowImpl< T, TBase >
{
public:
	DECLARE_WND_SUPERCLASS(_T("AtlMultiPaneStatusBar"), TBase::GetWndClassName())

// Data
	int m_nPanes;
	int* m_pPane;

// Constructor/destructor
	CWinMultiPaneStatusBarImpl() : m_nPanes(0), m_pPane(NULL)
	{ }

	~CWinMultiPaneStatusBarImpl()
	{
		if(m_pPane != NULL)
			delete [] m_pPane;
	}

// Methods
	HWND Create(HWND hWndParent, LPCTSTR lpstrText, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SBARS_SIZEGRIP, UINT nID = ATL_IDW_STATUS_BAR)
	{
#if (_ATL_VER >= 0x0400)
		return CWindowImpl< T, TBase >::Create(hWndParent, NULL, lpstrText, dwStyle, 0, nID);
#else
		return CWindowImpl< T, TBase >::Create(hWndParent, rcDefault, lpstrText, dwStyle, 0, nID);
#endif //(_ATL_VER >= 0x0400)
	}

	HWND Create(HWND hWndParent, UINT nTextID = ATL_IDS_IDLEMESSAGE, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SBARS_SIZEGRIP, UINT nID = ATL_IDW_STATUS_BAR)
	{
		TCHAR szText[128];	// max text lentgth is 127 for status bars
		szText[0] = 0;
		::LoadString(_Module.GetResourceInstance(), nTextID, szText, 127);
		return Create(hWndParent, szText, dwStyle, nID);
	}

	void SetPanes(int* pPanes, int nPanes, bool bSetText = true)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPanes > 0);

		m_nPanes = nPanes;
		delete [] m_pPane;
		m_pPane = NULL;

		ATLTRY(m_pPane = new int[nPanes]);
		ATLASSERT(m_pPane != NULL);
		memcpy(m_pPane, pPanes, nPanes * sizeof(int));

		int* pPanesPos = NULL;
		ATLTRY(pPanesPos = (int*)_alloca(nPanes * sizeof(int)));
		ATLASSERT(pPanesPos != NULL);

		// get status bar DC and set font
		CWinClientDC dc(m_hWnd);
		dc.SelectFont(GetFont());

		// get status bar borders
		int arrBorders[3];
		GetBorders(arrBorders);

		TCHAR szBuff[256];
		SIZE size;
		int cxLeft = arrBorders[0];
		const int cxInnerBorders = 6;	// 3 pixels each side

		// calculate right edge of each part
		for(int i = 0; i < nPanes; i++)
		{
			if(pPanes[i] == ID_DEFAULT_PANE)
			{
				// will be resized later
				pPanesPos[i] = 100 + cxLeft + arrBorders[2];
			}
			else
			{
				::LoadString(_pModule->GetResourceInstance(), pPanes[i], szBuff, sizeof(szBuff) / sizeof(TCHAR));
				dc.GetTextExtent(szBuff, lstrlen(szBuff), &size);
				pPanesPos[i] = cxLeft + size.cx + arrBorders[2] + cxInnerBorders;
			}
			cxLeft = pPanesPos[i];
		}

		SetParts(nPanes, pPanesPos);

		if(bSetText)
		{
			for(int i = 0; i < nPanes; i++)
			{
				if(pPanes[i] != ID_DEFAULT_PANE)
				{
					::LoadString(_pModule->GetResourceInstance(), pPanes[i], szBuff, sizeof(szBuff) / sizeof(TCHAR));
					SetPaneText(m_pPane[i], szBuff);
				}
			}
		}
	}

	BOOL GetPaneTextLength(int nPaneID, int* pcchLength = NULL, int* pnType = NULL) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		int nLength = GetTextLength(nIndex, pnType);
		if(pcchLength != NULL)
			*pcchLength = nLength;

		return TRUE;
	}

	BOOL GetPaneText(int nPaneID, LPTSTR lpstrText, int* pcchLength = NULL, int* pnType = NULL) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		int nLength = GetText(nIndex, lpstrText, pnType);
		if(pcchLength != NULL)
			*pcchLength = nLength;

		return TRUE;
	}

	BOOL SetPaneText(int nPaneID, LPCTSTR lpstrText, int nType = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		return SetText(nIndex, lpstrText, nType);
	}

	BOOL GetPaneRect(int nPaneID, LPRECT lpRect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		return GetRect(nIndex, lpRect);
	}

	BOOL GetPaneTipText(int nPaneID, LPTSTR lpstrText, int nSize) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		GetTipText(nPaneID, lpstrText, nSize);
		return TRUE;
	}

	BOOL SetPaneTipText(int nPaneID, LPCTSTR lpstrText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		SetTipText(nIndex, lpstrText);
		return TRUE;
	}

	BOOL GetPaneIcon(int nPaneID, HICON& hIcon) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		hIcon = GetIcon(nIndex);
		return TRUE;
	}

	BOOL SetPaneIcon(int nPaneID, HICON hIcon)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		return SetIcon(nIndex, hIcon);
	}

/*	BOOL SetPaneWidth(int nPaneID, int cxWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPaneID != ID_DEFAULT_PANE);	// Can't resize this one
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		// get pane positions
		int* pPanesPos = NULL;
		ATLTRY(pPanesPos = (int*)_alloca(m_nPanes * sizeof(int)));
		GetParts(m_nPanes, pPanesPos);
		// calculate offset
		int cxNext;
		if(nIndex == m_nPanes - 1)
		{
			cxNext = pPanesPos[m_nPanes - 1] + ::GetSystemMetrics(SM_CXVSCROLL) + ::GetSystemMetrics(SM_CXEDGE);
		}
		else
		{
			cxNext = pPanesPos[nIndex + 1];
		}

		int cxOff = cxWidth - (cxNext - pPanesPos[nIndex]);
		// find variable width pane
		int nDef = m_nPanes;
		for(int i = 0; i < m_nPanes; i++)
		{
			if(m_pPane[i] == ID_DEFAULT_PANE)
			{
				nDef = i;
				break;
			}
		}
		if(nIndex < nDef)	// before default pane
		{
			for(int i = nIndex + 1; i < nDef; i++)
				pPanesPos[i] += cxOff;
				
		}
		else			// after default one
		{
			for(int i = nDef + 1; i <= nIndex; i++)
				pPanesPos[i] -= cxOff;
		}
		// set pane postions
		SetParts(m_nPanes, pPanesPos);
		return TRUE;
	}
*/

// Message map and handlers
	BEGIN_MSG_MAP(CWinMultiPaneStatusBarImpl< T >)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
		if(wParam != SIZE_MINIMIZED && m_nPanes > 0)
			UpdatePanesLayout();
		return lRet;
	}

// Implementation
	void UpdatePanesLayout()
	{
		// get pane positions
		int* pPanesPos = NULL;
		ATLTRY(pPanesPos = (int*)_alloca(m_nPanes * sizeof(int)));
		GetParts(m_nPanes, pPanesPos);
		// calculate offset
		RECT rcClient;
		GetClientRect(&rcClient);
		int cxOff = rcClient.right - (pPanesPos[m_nPanes - 1] + ::GetSystemMetrics(SM_CXVSCROLL) + ::GetSystemMetrics(SM_CXEDGE));
		// find variable width pane
		int i;
		for(i = 0; i < m_nPanes; i++)
		{
			if(m_pPane[i] == ID_DEFAULT_PANE)
				break;
		}
		// move all panes from variable one
		for(; i < m_nPanes; i++)
		{
			if((pPanesPos[i] + cxOff) <= pPanesPos[i - 1])
				break;
			pPanesPos[i] += cxOff;
		}
		// set pane postions
		SetParts(m_nPanes, pPanesPos);
	}

	int GetPaneIndexFromID(int nPaneID) const
	{
		for(int i = 0; i < m_nPanes; i++)
		{
			if(m_pPane[i] == nPaneID)
				return i;
		}

		return -1;	// not found
	}
};

class CWinMultiPaneStatusBar : public CWinMultiPaneStatusBarImpl<CWinMultiPaneStatusBar>
{
public:
// nothing here
};

}; //namespace ATL

#endif // __ATLCTRLX_H__
