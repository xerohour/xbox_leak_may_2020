///////////////////////////////////////////////////////////////////////////////
//	BARCTRL.CPP
//		File for special toolbar controls.  At the very least, any control
//		for use in a toolbar must be call DkPreHandleMessage before calling
//		its own WindowProc.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bardockx.h"
#include "shlbar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
//	CDockEdit
//		This is a specialized CEdit which can be created in dockable
//		windows.

BOOL CDockEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!CEdit::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	SetFont(GetStdFont(0));

	SubclassCtl3d();

	return TRUE;
}

LRESULT	CDockEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG lResult;

	if (DkPreHandleMessage(GetSafeHwnd(),
		message, wParam, lParam, &lResult))
		return lResult;

	// forward tooltip text notifications to bar, directly
	if(message==WM_NOTIFY)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if(pNMHDR->code == TTN_NEEDTEXT)
		{
			CWnd *pWnd=GetParent();
			if(pWnd)
			{
				return pWnd->SendMessage(message, wParam, lParam);
			}
		}
	}
		
	return CEdit::WindowProc(message, wParam, lParam);
}

BOOL CDockEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message != WM_KEYDOWN)
		return FALSE;

	switch (pMsg->wParam)
	{
	default:
		return FALSE;
	
	case 'Z':
	case 'X':
	case 'C':
	case 'V':
		if (GetKeyState(VK_CONTROL) >= 0)
			return FALSE;
		break;

	case VK_DELETE:
		if (GetKeyState(VK_CONTROL) < 0 || GetKeyState(VK_MENU) < 0)
			return FALSE;
		break;
		
	case VK_INSERT:
		if (GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0)
			return FALSE;
		break;
		
	case VK_BACK:
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_NEXT:
	case VK_PRIOR:
	case VK_HOME:
	case VK_END:
		break;
	}

	// Getting this far means we should short-circuit the normal
	// accelerator processing and dispatch immediately...
	TranslateMessage(pMsg);
	DispatchMessage(pMsg);

	return TRUE;
}
	
///////////////////////////////////////////////////////////////////////////////
//	CDockCombo
//		This includes a subclass of both the ComboBox window as well as its
//		edit control.

//	CDockComboEdit::GetSuperProcAddr
//		Can't just use CEdit here, since a combo edit control is likely to
//		have a different window proc.

CDockComboEdit::CDockComboEdit(BOOL fDockable /* = TRUE */)
{
	m_fDockable = fDockable; // lets us use the same control in a dialog if FALSE
}


WNDPROC* CDockComboEdit::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

//	CDockComboEdit::WindowProc
//		Call DkPreHandleMessage, and disable SetCursor if we are in customize
//		mode.

LRESULT CDockComboEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG lResult;

	if (m_fDockable && DkPreHandleMessage(GetSafeHwnd(),
		message, wParam, lParam, &lResult))
		return lResult;

	if (message == WM_SETCURSOR && theApp.IsCustomizing())
		return 0L;
	
	// forward tooltip text notifications to bar, directly
	if(message==WM_NOTIFY)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if(pNMHDR->code == TTN_NEEDTEXT)
		{
			CWnd *pWnd=GetParent();
			if(pWnd)
			{
				return pWnd->SendMessage(message, wParam, lParam);
			}
		}
	}
		
	return CWnd::WindowProc(message, wParam, lParam);
}

//	CDockCombo::CDockCombo
//  initialize edit item pointer to null...

CDockCombo::CDockCombo(BOOL fDockable /* = TRUE */)
{
	m_pChildEdit = NULL;
	m_fDockable = fDockable;
}

//	CDockCombo::Create
//		Subclass the edit control.

BOOL CDockCombo::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!CComboBox::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	m_pChildEdit = SubclassEdit();
	SetFont(GetStdFont(0));

	SubclassCtl3d();

	return TRUE;
}

CWnd* CDockCombo::SubclassEdit()
{
	// HACK: We have used this way of finding Edit control since
	// v2.0, and it still works on Win95.  This is not likely to
	// change.

	CWnd* pWnd = GetDlgItem(1001);
	if (pWnd != NULL)
	{
		CDockComboEdit* pEdit = new CDockComboEdit(m_fDockable);
		pEdit->SubclassWindow(pWnd->GetSafeHwnd());
		return pEdit;
	}

	return NULL;
}

void CDockCombo::PostNcDestroy()
{
	delete m_pChildEdit;
}

//	CDockCombo::WindowProc
//		Call DkPreHandleMessage, but allow Alt-Up and Alt-Down to go to the
//		combo.

LRESULT CDockCombo::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message != WM_SYSKEYDOWN ||
		!(wParam == VK_DOWN || wParam == VK_UP))
	{
		LONG lResult;
	
		if (m_fDockable && DkPreHandleMessage(GetSafeHwnd(),
			message, wParam, lParam, &lResult))
			return lResult;
	}
	
	// forward tooltip text notifications to bar, directly
	if(message==WM_NOTIFY)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if(pNMHDR->code == TTN_NEEDTEXT)
		{
			CWnd *pWnd=GetParent();
			if(pWnd)
			{
				return pWnd->SendMessage(message, wParam, lParam);
			}
		}
	}
		
	return CComboBox::WindowProc(message, wParam, lParam);
}


///////////////////////////////////////////////////////////////////////////////
//	CToolbarCombo

BEGIN_MESSAGE_MAP(CToolbarCombo, CDockCombo)
	//{{AFX_MSG_MAP(CToolbarCombo)
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
//	CToolbarCombo message handlers.

//	CToolbarCombo::OnNcHitTest
//		Hack to make gray area between edit and drop button cause a toolbar
//		move.

UINT CToolbarCombo::OnNcHitTest(CPoint point)
{
	ScreenToClient(&point);
	if (ChildWindowFromPoint(point) == this)
	{
		CRect rect;
		GetClientRect(rect);
		
		if (point.x < rect.right - ::GetSystemMetrics(SM_CXVSCROLL))
			return HTCAPTION;
	}
	
	return CDockCombo::OnNcHitTest(point);
}

///////////////////////////////////////////////////////////////////////////////
//	Control utility functions.
//

void SizeComboToContent(CComboBox* pCombo, int nMax)
{
	ASSERT_VALID(pCombo);
	int cyEdit = (int)::SendMessage(pCombo->m_hWnd, CB_GETITEMHEIGHT, UINT(-1), 0);

	CDC* pDC = pCombo->GetDC();
	CFont* pFontOld;
	CFont* pFont = pCombo->GetFont();

	if (pFont != NULL)
		pFontOld = pDC->SelectObject(pFont);

	// FUTURE:  Need better way to get height
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	if (pFont != NULL)
		pDC->SelectObject(pFontOld);

	pCombo->ReleaseDC(pDC);

	int cyHeight = pCombo->GetCount();
	if (nMax > 0 && cyHeight > nMax)
		cyHeight = nMax;

	cyHeight = cyEdit +	cyHeight * tm.tmHeight +
		GetSystemMetrics(SM_CYBORDER) * 3;

	CRect rectCombo;
	pCombo->GetDroppedControlRect(&rectCombo);
	if (theApp.m_bWin4)
	{
		// Win95 does its own size to content.  Leave it alone, if we have
		// enough space.  But they also need a little more space than the
		// old shell, so just add an extra line.

		cyHeight += tm.tmHeight;
		if (rectCombo.Height() > cyHeight)
			cyHeight = rectCombo.Height();
	}

	if (rectCombo.Height() != cyHeight)
	{
		pCombo->SetWindowPos(NULL, 0, 0, rectCombo.Width(), cyHeight,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	}
}
