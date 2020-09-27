// menubtn.cpp : implementation file for the menu button control
//

// This has been converted to allow command bar menus, but can use the old type as some
// menus use loadmenu

#include "stdafx.h"
#include "resource.h"
#include "util.h"
#include "utilctrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CMenuBtn

CMenuBtn::CMenuBtn() :
	m_bMenuShown(FALSE),
	m_pMenuParent(NULL),
	m_bUseTraditional(FALSE),
	m_pTraditionalMenu(FALSE),
	m_hLastActive(NULL)
{
}

CMenuBtn::~CMenuBtn()
{
	if(m_bUseTraditional)
	{
		if(m_pTraditionalMenu)
		{
			delete m_pTraditionalMenu;
		}
	}
	else
	{
		if(m_pNewMenu)
		{
			delete m_pNewMenu;
		}
	}
}

const UINT WM_SHOWMENU = ::RegisterMessage("WM_SHOWMENU");
BEGIN_MESSAGE_MAP(CMenuBtn, C3dBitmapButton)
	//{{AFX_MSG_MAP(CMenuBtn)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_REGISTERED_MESSAGE(WM_SHOWMENU, OnShowMenu)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_LBUTTONUP()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOLORCHANGE()
END_MESSAGE_MAP()

BOOL CMenuBtn::SubclassDlgItem(UINT nID, CWnd* pParent)
{
	// Subclass the button.
	if (!C3dBitmapButton::SubclassDlgItem(nID, pParent))
		return FALSE;

	// It must be an owner-draw button.
	ASSERT(GetButtonStyle() & BS_OWNERDRAW != 0);

	m_bToggle = FALSE;
	if (!DrawBitmaps())
		return FALSE;

	ASSERT(m_bitmap.m_hObject != NULL);
	return TRUE;
}

void CMenuBtn::SetPopup(POPDESC* ppop)
{
	if(m_bUseTraditional)
	{
		if(m_pTraditionalMenu)
		{
			delete m_pTraditionalMenu;
		}
	}

	m_bUseTraditional=FALSE;
	m_pNewMenu=new CBContextPopupMenu;
	m_pNewMenu->Create(ppop);
}

BOOL CMenuBtn::LoadMenu(UINT idMenuResource)
{
	if(!m_bUseTraditional)
	{
		if(m_pNewMenu)
		{
			delete m_pNewMenu;
		}
	}

	m_bUseTraditional=TRUE;
	m_pTraditionalMenu=new CContextPopupMenu;
	return m_pTraditionalMenu->LoadMenu(idMenuResource);
}

void CMenuBtn::SetPos(int pos)
{
	ASSERT(m_bUseTraditional);

	m_pTraditionalMenu->SetPos(pos);
}

/////////////////////////////////////////////////////////////////////////////
// CMenuBtn message handlers

void CMenuBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Do we need to display the menu?
	if (!m_bMenuShown && lpDrawItemStruct->itemState & ODS_SELECTED != 0)
	{
		// Must show the popup...
		PostMessage(WM_SHOWMENU);
		m_bMenuShown = TRUE;
	}

	C3dBitmapButton::DrawItem(lpDrawItemStruct);
}

BOOL CMenuBtn::DrawBitmaps()
{
	// delete old bitmaps (if present)
	m_bitmap.DeleteObject();
	m_bitmapSel.DeleteObject();
	m_bitmapFocus.DeleteObject();
	m_bitmapDisabled.DeleteObject();

	// Get the window text.
	CString strWindow;
	GetWindowText(strWindow);

	// Get the window rect.
	CRect rcWindow;
	GetWindowRect(rcWindow);

	// Create the "Up" bitmap.
	HBITMAP hbm = CreateButtonBitmap(TBBS_BUTTON, rcWindow, NULL, strWindow);
	if (hbm == NULL)
		return FALSE; // need at least one image...

	m_bitmap.Attach(hbm);

	// Create the "Pressed" bitmap.  The 0x80 style is focus rect.
	hbm = CreateButtonBitmap(TBBS_BUTTON | TBBS_PRESSED | 0x80, rcWindow, NULL, strWindow);
	if (hbm != NULL)
		m_bitmapSel.Attach(hbm);

	// Create the "Focused" bitmap
	hbm = CreateButtonBitmap(TBBS_BUTTON | 0x80, rcWindow, NULL, strWindow);
	if (hbm != NULL)
		m_bitmapFocus.Attach(hbm);

	// Create the "Disabled" bitmap
	hbm = CreateButtonBitmap(TBBS_BUTTON | TBBS_DISABLED, rcWindow, NULL, strWindow);
	if (hbm != NULL)
		m_bitmapDisabled.Attach(hbm);

	return TRUE;
}

HBITMAP CMenuBtn::CreateButtonBitmap(UINT nStyle,  LPCRECT lpRect,
		CBitmap* pbmGlyph /*= NULL*/, LPCTSTR lpszText /*= NULL*/)
{
	return ::CreateButtonBitmap(this, nStyle, lpRect, pbmGlyph, lpszText, TRUE);
}

void CMenuBtn::ShowMenu()
{
	// since this is a union, this checks both pointers
	if(m_bUseTraditional)
	{
		if (m_pTraditionalMenu== NULL)
		{
			return;
		}
		else
		{
			ASSERT(m_pTraditionalMenu);
			ShowMenu(*m_pTraditionalMenu);
		}
	}
	else
	{
		if (m_pTraditionalMenu== NULL)
		{
			return;
		}
		else
		{
			ASSERT(m_pNewMenu);
			ShowMenu(*m_pNewMenu);
		}
	}
}

void CMenuBtn::ShowMenu(CContextPopupMenu &menuPopup)
{
	// Get the parent window.
	CWnd* pParent = m_pMenuParent==NULL ? GetParent() : m_pMenuParent;

	// Get the window text.
	CString strWindow;
	GetWindowText(strWindow);

	// Display the menu.
	CRect rect;
	GetWindowRect(rect);
	// determine which way to pop up the menu (no text, pop right)
	int xMenu = strWindow.IsEmpty() ? rect.right : rect.left;
	int yMenu = strWindow.IsEmpty() ? rect.top : rect.bottom;
	UINT nCmd = (UINT)menuPopup.TrackPopup(TPM_RETURNCMD | TPM_LEFTALIGN |
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON, xMenu, yMenu, pParent);
	m_bMenuShown = FALSE;

	// Reset the button.
	SetState(FALSE);
	UpdateWindow();

	// Since the WM_LBUTTONUP is eaten by the menu,
	// send a fake one to the button.  Otherwise,
	// the button will depress whenever the user
	// moves the mouse over the button.
	SendMessage(WM_LBUTTONUP);

	// Send the command message.
	if (nCmd > 0 && pParent != NULL)
		pParent->SendMessage(WM_COMMAND, MAKEWPARAM(nCmd, 0));
}

void CMenuBtn::ShowMenu(CBContextPopupMenu &menuPopup)
{
	// Get the parent window.
	CWnd* pParent = m_pMenuParent==NULL ? GetParent() : m_pMenuParent;

	// Get the window text.
	CString strWindow;
	GetWindowText(strWindow);

	// Since the WM_LBUTTONUP is eaten by the menu,
	// send a fake one to the button.  Otherwise,
	// the button will depress whenever the user
	// moves the mouse over the button.
	SendMessage(WM_LBUTTONUP);

	// Display the menu.
	CRect rect;
	GetWindowRect(rect);
	// determine which way to pop up the menu (no text, pop right)
	int xMenu = strWindow.IsEmpty() ? rect.right : rect.left;
	int yMenu = strWindow.IsEmpty() ? rect.top : rect.bottom;
	UINT nCmd = (UINT)menuPopup.TrackPopup(TPM_RETURNCMD | TPM_LEFTALIGN |
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON, xMenu, yMenu, pParent, rect);
	m_bMenuShown = FALSE;

	// Reset the button.
	SetState(FALSE);
	UpdateWindow();


	// Send the command message.
	if (nCmd > 0 && pParent != NULL)
		pParent->SendMessage(WM_COMMAND, MAKEWPARAM(nCmd, 0));
}

LRESULT CMenuBtn::OnShowMenu(WPARAM wParam, LPARAM lParam)
{
	ShowMenu();

	return 0;
}

void CMenuBtn::OnClicked()
{
	if (!m_bMenuShown)
	{
		PostMessage(WM_SHOWMENU);
		m_bMenuShown = TRUE;
	}
}

void CMenuBtn::OnSetFocus(CWnd *pOldWindow)
{
	if (pOldWindow != NULL)
	{
		m_hLastActive = pOldWindow->m_hWnd;
		C3dBitmapButton::OnSetFocus(pOldWindow);
	}
	else
		m_hLastActive = NULL;
}

void CMenuBtn::OnLButtonUp(UINT n1, CPoint pt)
{
	if(m_hLastActive && ::IsWindow(m_hLastActive))
	{
		::SetFocus(m_hLastActive);
	}
	m_hLastActive = NULL;
	C3dBitmapButton::OnLButtonUp(n1,pt);
}

void CMenuBtn::OnSysColorChange()
{
	DrawBitmaps();

	Invalidate(TRUE);
}

