// hotlinkc.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "hotlinkc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHotLinkCtrl

CHotLinkCtrl::CHotLinkCtrl()
{
	m_fJumpable = FALSE;
}

CHotLinkCtrl::~CHotLinkCtrl()
{
}

BEGIN_MESSAGE_MAP(CHotLinkCtrl, CWnd)
	//{{AFX_MSG_MAP(CHotLinkCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_NCHITTEST()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CHotLinkCtrl::InitStaticData()
{
	m_hcurHand = ::LoadCursor(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDC_JUMPHAND));

	//
	// Get the jump color from the registry.  We use the WinHelp color.
	//
	CHAR szJumpColorSpec[64];
	GetProfileString("Windows Help", "JumpColor", "000 128 000", szJumpColorSpec, sizeof(szJumpColorSpec) / sizeof(CHAR));
	CHAR* pszJumpSpec = szJumpColorSpec;
	ULONG ulRed = _tcstoul(pszJumpSpec, &pszJumpSpec, 10);
	ULONG ulGreen = _tcstoul(pszJumpSpec, &pszJumpSpec, 10);
	ULONG ulBlue = _tcstoul(pszJumpSpec, &pszJumpSpec, 10);
	m_clrJump = RGB(ulRed, ulGreen, ulBlue);

	CFont* pFont = GetStdFont(font_NormalUnderline);
	ASSERT(pFont);
}

HCURSOR CHotLinkCtrl::m_hcurHand;
CFont CHotLinkCtrl::m_FontJump;
COLORREF CHotLinkCtrl::m_clrJump;

/////////////////////////////////////////////////////////////////////////////
// CHotLinkCtrl message handlers

void CHotLinkCtrl::SetJumpObject(DWORD dwObject)
{
	m_dwJumpObject = dwObject;
	m_strCaption = FormatHex(dwObject);
	m_fJumpable = TRUE;

	if (GetSafeHwnd())
	{
		Invalidate();
	}
}

void CHotLinkCtrl::MarkInvalid()
{
	m_strCaption = ids(IDS_UNAVAILABLE);
	m_fJumpable = FALSE;

	if (GetSafeHwnd())
	{
		Invalidate();
	}
}

void CHotLinkCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	if (IsJumpable())
	{
		GetParent()->SendMessage(WM_HOTLINK,
			(WPARAM)m_dwJumpObject, (LPARAM)GetObjectType());
	}
}

BOOL CHotLinkCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (IsJumpable())
	{
		SetCursor(m_hcurHand);
		return TRUE;
	}
	else
	{
		return CWnd::OnSetCursor(pWnd, nHitTest, message);
	}
}

UINT CHotLinkCtrl::OnNcHitTest(CPoint)
{
	return HTCLIENT;
}

void CHotLinkCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	//
	// Allow the colors (especially the background color)
	// to be set as normal.
	//
	GetParent()->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)dc.m_hDC, (LPARAM)this->m_hWnd);

	if (IsJumpable())
	{
		//
		// Set the jump text color and underlined font.
		//
		dc.SetTextColor(m_clrJump);
		dc.SelectObject(GetStdFont(font_NormalUnderline));
	}
	else
	{
		//
		// Set the default font.  Use the normal static control text
		// color obtained above.
		//
		dc.SelectObject(GetStdFont(font_Normal));
	}

	//
	// Draw the text.  Specify a rectangle that is the size of the
	// whole control so that all of it gets repainted, not just
	// where the new text goes.  Static controls don't have a
	// background brush, so the erase background processing doesn't
	// fill in the background, and we must draw the text opaque.
	//
	RECT rc;
	GetClientRect(&rc);
	dc.ExtTextOut(0, 0, ETO_OPAQUE, &rc, m_strCaption, m_strCaption.GetLength(), NULL);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CHotLinkCtrlWnd

CHotLinkCtrlWnd::CHotLinkCtrlWnd()
{
}

CHotLinkCtrlWnd::~CHotLinkCtrlWnd()
{
}

void CHotLinkCtrlWnd::SetJumpObject(DWORD dwObject)
{
	m_dwJumpObject = dwObject;

	//
	// Format an hwnd using FormatHandle(), not FormatHex().
	// This will return "(None)" if it is null.
	//
	m_strCaption = FormatHandle((HANDLE)dwObject);

	//
	// Null hwnds are not jumpable.
	//
	if (dwObject)
	{
		m_fJumpable = TRUE;
	}
	else
	{
		m_fJumpable = FALSE;
	}

	if (GetSafeHwnd())
	{
		Invalidate();
	}
}

BEGIN_MESSAGE_MAP(CHotLinkCtrlWnd, CHotLinkCtrl)
	//{{AFX_MSG_MAP(CHotLinkCtrlWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CHotLinkCtrlPrc

CHotLinkCtrlPrc::CHotLinkCtrlPrc()
{
}

CHotLinkCtrlPrc::~CHotLinkCtrlPrc()
{
}

BEGIN_MESSAGE_MAP(CHotLinkCtrlPrc, CHotLinkCtrl)
	//{{AFX_MSG_MAP(CHotLinkCtrlPrc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CHotLinkCtrlThd

CHotLinkCtrlThd::CHotLinkCtrlThd()
{
}

CHotLinkCtrlThd::~CHotLinkCtrlThd()
{
}

BEGIN_MESSAGE_MAP(CHotLinkCtrlThd, CHotLinkCtrl)
	//{{AFX_MSG_MAP(CHotLinkCtrlThd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
