///////////////////////////////////////////////////////////////////////////////
//	FINDCTRL.CPP
//		Contains implementation for the customizable find combo.
//

#include "stdafx.h"
#include "findctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
//	Global interface to the find control.
//

HWND CreateFindCombo(DWORD dwStyle, RECT* lpRect, HWND hwndParent, UINT nID)
{
	CFindCombo* pCombo = new CFindCombo;
	HWND hwndCombo=::CreateToolbarEditCombo(pCombo, dwStyle, lpRect, hwndParent, nID);
	if(hwndCombo==NULL)
	{
		delete pCombo;
		return NULL;
	}
	else
	{
		return hwndCombo;
	}
}

void SetFindFocus()
{
	CFindCombo::SetFindFocus();
}

void UpdateFindCombos()
{
	CFindCombo::UpdateAllViews();
}

BOOL IsFindComboActive()
{
	CWnd* pWnd = CWnd::GetFocus();
	CWnd* pWndParent = pWnd==NULL ? NULL : pWnd->GetParent();
	return pWndParent != NULL && pWndParent->IsKindOf(RUNTIME_CLASS(CFindCombo));
}

///////////////////////////////////////////////////////////////////////////////
//	CFindCombo
//

IMPLEMENT_DYNAMIC(CFindCombo, CToolbarEditCombo)

CObList CFindCombo::s_listCombos;

CFindCombo::CFindCombo()
//:	m_nId(0)
{
	s_listCombos.AddTail(this);
}

CFindCombo::~CFindCombo()
{
	CObject* pObj;

	POSITION pos = s_listCombos.GetHeadPosition();
	while (pos != NULL)
	{       
		pObj = s_listCombos.GetAt(pos);
		if (pObj == this)
		{
			s_listCombos.RemoveAt(pos);
			break;
		}

		s_listCombos.GetNext(pos);
	}
}

BOOL CFindCombo::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!CToolbarEditCombo::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	LimitText(LIMIT_FIND_COMBO_TEXT);

	return TRUE;
}

BOOL CFindCombo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message != WM_KEYDOWN)
		return FALSE;

	switch (pMsg->wParam)
	{
	case VK_RETURN:
		if ((pMsg->lParam & 0x4000000) == 0)	// Not a repeat.
		{
			// See if there's something to find
			if (UpdateFindInfo(TRUE))
			{
				SetCurSel(0);

				HWND hwndView = theApp.GetActiveView();
				if (hwndView == NULL)
					return 0L;

				CPartView* pView = (CPartView*) FromHandlePermanent(hwndView);
				ASSERT(pView != NULL && pView->IsKindOf(RUNTIME_CLASS(CPartView)));

				// Shift+Enter means search up.
				if (GetKeyState (VK_SHIFT) & 0x8000)
					findReplace.goUp = TRUE;
				else
					findReplace.goUp = FALSE;

				pView->OnFindNext();
			}
		}
		return TRUE;  
	}
	
	return CToolbarEditCombo::PreTranslateMessage(pMsg);
}

BOOL CFindCombo::UpdateFindInfo(BOOL bUpdateDropDown)
{
	char szTmp[LIMIT_FIND_COMBO_TEXT + 1];
	SendMessage(WM_GETTEXT, LIMIT_FIND_COMBO_TEXT, (long)(LPSTR)szTmp);
	if (szTmp[0] == '\0')
		return FALSE;

	lstrcpy(findReplace.findWhat, szTmp); // update the find string
	findReplace.DisposeOfPattern(); // invalidate the old regex pattern compiled previously

	ShellPickList().InsertEntry(FIND_PICK, &findReplace);	// This updates all views.

	if (bUpdateDropDown)
		UpdateDropDown();

	return TRUE;
}

void CFindCombo::UpdateDropDown()
{
	LPCSTR lpsz;

	ResetContent();

	for (int i = 0 ; i < ShellPickList().GetEntryCount(FIND_PICK); i++)
	{
		VERIFY(lpsz = ShellPickList().GetEntry(FIND_PICK,i));
		InsertString(-1, lpsz);
	}

	SetCurSel(0);
}

void CFindCombo::UpdateView()
{
	CString strCurrent;
	GetWindowText(strCurrent);

	if (ShellPickList().GetEntryCount(FIND_PICK) > 0)
	{
		LPCSTR lpsz;

		VERIFY(lpsz = ShellPickList().GetEntry(FIND_PICK,0));
		if (strCurrent != lpsz)
			SetWindowText(lpsz);
	}
	else if (!strCurrent.IsEmpty())
	{
		SetWindowText("");
	}
}

void CFindCombo::UpdateAllViews()
{
	CFindCombo* pCombo;

	POSITION pos = s_listCombos.GetHeadPosition();
	while (pos != NULL)
	{       
		pCombo = (CFindCombo*) s_listCombos.GetNext(pos);
		ASSERT(pCombo != NULL);
		pCombo->UpdateView();
	}
}

void CFindCombo::SetFindFocus()
{
	CFindCombo* pCombo;
	BOOL fComboFound = FALSE;

	POSITION pos = s_listCombos.GetHeadPosition();
	while (pos != NULL)
	{       
		pCombo = (CFindCombo*) s_listCombos.GetNext(pos);
		ASSERT(pCombo != NULL);

		if (pCombo->IsWindowVisible() && pCombo->IsWindowEnabled())
		{
			pCombo->SetFocus();
			fComboFound = TRUE;
			break;
		}
	}

	if (!fComboFound)
	{
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, MAKEWPARAM((WORD)ID_EDIT_FIND, 1));
	}
}

BOOL CFindCombo::DoHelp(void)
{
	if (UpdateFindInfo(FALSE)) {
		theApp.HelpOnKeyword(findReplace.findWhat);
	}
	else {
		theApp.HelpOnApplication(m_nId,HELPTYPE_COMMAND);
	}
	// Grab focus if no view has gotten it
	if (theApp.GetActiveView() == NULL)
	{
		// and no flosting window (like Query results) is active.
		CWnd * pwnd = GetActiveWindow();
		if (pwnd && pwnd->IsKindOf(RUNTIME_CLASS(CFloatingFrameWnd)))
			return TRUE;
		SetFindFocus();
	}
	return TRUE;
}

void CFindCombo::OnEditKillFocus(void)
{
	if (!UpdateFindInfo(FALSE))
	{
		SetCurSel(0);
	}
}

void CFindCombo::OnEditSetFocus(void)
{
	UpdateDropDown();
}

BOOL CFindCombo::OnEditHelp(void)
{
	return DoHelp();
}

BEGIN_MESSAGE_MAP(CFindCombo, CToolbarEditCombo)
END_MESSAGE_MAP()

