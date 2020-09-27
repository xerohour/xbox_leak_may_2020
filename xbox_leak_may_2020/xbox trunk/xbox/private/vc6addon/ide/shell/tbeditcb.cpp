///////////////////////////////////////////////////////////////////////////////
//	TBEDITCB.CPP
//		Contains implementation for a generic editable combo on a bar
//

#include "stdafx.h"
#include "shlbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

HWND CreateToolbarEditCombo(CToolbarEditCombo *pCombo, DWORD dwStyle, RECT* lpRect, HWND hwndParent, UINT nID)
{
	if (!pCombo->Create(dwStyle, *lpRect, CWnd::FromHandle(hwndParent), nID))
	{
		TRACE0("Failed to create toolbar combo.\n");

		return NULL;
	}
	
	pCombo->SetExtendedUI();
	return pCombo->m_hWnd;
}

///////////////////////////////////////////////////////////////////////////////
//	CToolbarEditComboEdit
//

IMPLEMENT_DYNAMIC(CToolbarEditComboEdit, CWnd)

WNDPROC* CToolbarEditComboEdit::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

//	CToolbarEditComboEdit::WindowProc
//		Call DkPreHandleMessage, and disable SetCursor if we are in customize
//		mode.

LRESULT CToolbarEditComboEdit::WindowProc(UINT message, WPARAM wParam,	LPARAM lParam)
{
	LONG lResult;

	if (DkPreHandleMessage(GetSafeHwnd(),
		message, wParam, lParam, &lResult))
		return lResult;

	CToolbarEditCombo* pParent = (CToolbarEditCombo*) GetParent();
	ASSERT(pParent != NULL);

	switch (message)
	{
	case WM_SETFOCUS:
		pParent->OnEditSetFocus();
		break;

	case WM_KILLFOCUS:
		pParent->OnEditKillFocus();
		break;

	case WM_HELP:	// forward these to the Find Combo
		return pParent->OnEditHelp();
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
//	CToolbarEditCombo
//

IMPLEMENT_DYNAMIC(CToolbarEditCombo, CComboBox) // not the direct base class, but the next one with rtti.

CToolbarEditCombo::CToolbarEditCombo()
{
}

CToolbarEditCombo::~CToolbarEditCombo()
{
}

BOOL CToolbarEditCombo::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!CToolbarCombo::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	m_nId = nID;
	SetFont(GetStdFont(1));
	UpdateView();

	return TRUE;
}

CWnd* CToolbarEditCombo::SubclassEdit()
{
	// HACK: We have used this way of finding Edit control since
	// v2.0, and it still works on Win95.  This is not likely to
	// change.

	CWnd* pWnd = GetDlgItem(1001);
	if (pWnd != NULL)
	{
		m_pChildEdit = new CToolbarEditComboEdit;
		m_pChildEdit->SubclassWindow(pWnd->GetSafeHwnd());
		return m_pChildEdit;
	}

	return NULL;
}

void CToolbarEditCombo::PostNcDestroy()
{
	delete m_pChildEdit;
	delete this;
}

BOOL CToolbarEditCombo::PreTranslateMessage(MSG* pMsg)
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
		if (GetKeyState(VK_CONTROL) >= 0 || GetKeyState(VK_SHIFT) < 0 || GetKeyState(VK_MENU) < 0)
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

	case VK_ESCAPE:				
		// revert the contents of the find item to what it used to be
		UpdateView();
		break;	// let DkPreHandleMessage set focus.

	}
	
	TranslateMessage(pMsg);
	DispatchMessage(pMsg);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CToolbarEditCombo, CToolbarCombo)
END_MESSAGE_MAP()

