#include "stdafx.h"

#include "util.h"
#include "resource.h"

#include <prjapi.h>
#include <prjguid.h>
#include <oleipcid.h>
#include "ipcslob.h"
#include <ocdevo.h>

#include "sheet.h"
#include "allpage2.h"

// IID_IForm needed for ISBug: 7249 - defined in forms3.h which wont compile
// in the devstudio tree
// {04598fc8-866c-11cf-ab7c-00aa00c08fcf}
DEFINE_GUID(IID_IForm, 0x04598fc8, 0x866c, 0x11cf, 0xab, 0x7c, 0x0, 0xaa, 0x0, 0xc0, 0x8f, 0xcf);
#include "shldocs_.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Globals

CSheetWnd *g_PropSheet;
BOOL g_PropSheetVisible = FALSE;

/////////////////////////////////////////////////////////////////////////////
//	C3dPropertyPage

IMPLEMENT_DYNAMIC(C3dPropertyPage, CDialog)

C3dPropertyPage::C3dPropertyPage()
{
	m_pSlob = NULL;
	m_pSheetWnd = NULL;
}

BOOL C3dPropertyPage::SetupPage(CSheetWnd* pSheetWnd, CSlob* pSlob)
{
	m_pSheetWnd = pSheetWnd;
//	m_pSlob = pSlob;

	if (!IsPageActive())
	{
		if (!Create(0, pSheetWnd))
		{
			m_pSheetWnd->FlushUnusedPages();

			if (!Create(0, pSheetWnd))
			{
				// PPMOVE : [matthewt]
				// VSHELL currently does not support this
				// SetUserEmergency(TRUE);
				return FALSE;
			}
		}

		CSheetWnd::s_listLoadedPages.AddTail(this);
		
		CPoint ptPos(cxWndMargin + cxPageMargin - GetSystemMetrics(SM_CYBORDER),
			pSheetWnd->s_cyBreakLine + cyPageMargin - GetSystemMetrics(SM_CYBORDER));
		MovePage(CRect(ptPos, GetPageSize()));
		
		if (m_hWnd != NULL)
		{
			// If this assert fires, you need to fix your dialog template...
			// Each page must be a borderless, disabled child window
			ASSERT(GetStyle() & (WS_DISABLED | WS_CHILD | WS_BORDER) ==
				WS_DISABLED | WS_CHILD);
		
			EnableWindow();
		}
	}

	// Calculate desired SheetWnd client area size.
	// Also store away the maximum page size, so the pages can expand during
	// InitializePage() if they want to.

	CSize sizeSheet = m_pSheetWnd->CalcMaxPageSize();
	sizeSheet.cx += (cxPageMargin + cxWndMargin) * 2;
	sizeSheet.cy += CSheetWnd::s_cyBreakLine +
		cyPageMargin * 2 + cyWndMargin;

	// Adjust to get window size.
	CRect rectWnd, rectClient;
	pSheetWnd->GetWindowRect(rectWnd);
	pSheetWnd->GetClientRect(rectClient);

	// find out if we need to adjust the height to account
	// for the html level combo box in the property dialog
	// if level combo box was shown, we reduce that height
	int		iLevelComboHeight = m_pSheetWnd->GetLevelComboHeight();
	if (iLevelComboHeight > 0)
	{
		rectWnd.bottom -= iLevelComboHeight;
		rectClient.bottom -= iLevelComboHeight;
	}

	pSheetWnd->ClientToScreen(rectClient);
	sizeSheet.cx += (rectClient.left - rectWnd.left) +
		(rectWnd.right - rectClient.right);
	sizeSheet.cy += (rectClient.top - rectWnd.top) +
		(rectWnd.bottom - rectClient.bottom);

	if (sizeSheet != rectWnd.Size())
	{
		// Calculate the sheet rect by anchoring the current rect by
		// the corner closest to the edge, and sizing to the new size.

		CRect rectSheet;
		int cxScreen = GetSystemMetrics(SM_CXSCREEN);
		int cyScreen = GetSystemMetrics(SM_CYSCREEN);

		if (rectWnd.left < cxScreen - rectWnd.right)
			rectSheet.left = rectWnd.left;
		else
			rectSheet.left = rectWnd.right - sizeSheet.cx;

		if (rectWnd.top < cyScreen - rectWnd.bottom)
			rectSheet.top = rectWnd.top;
		else
			rectSheet.top = rectWnd.bottom - sizeSheet.cy;

		rectSheet.right = rectSheet.left + sizeSheet.cx;
		rectSheet.bottom = rectSheet.top + sizeSheet.cy;

		// Keep the sheet from growing off screen.
		ForceRectOnScreen(rectSheet);

		// if the level combo box has a height, add it to the
		// height so that there is space on the dialog to show
		// the combo box
		m_pSheetWnd->SetWindowPos(NULL, rectSheet.left, rectSheet.top,
			rectSheet.Width(), rectSheet.Height() + iLevelComboHeight,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}
	
	return TRUE;
}

void C3dPropertyPage::InitializePage()
{
	// Default is to do nothing...

	// Override this to do special processing for page initialization.
}

void C3dPropertyPage::TermPage()
{
	m_pSlob = NULL;
}

void C3dPropertyPage::Activate(UINT, CSlob *)
{
	// Default is to do nothing...

	// Override this to do special processing just prior to the property
	// window being activated/deactivated.
}

BOOL C3dPropertyPage::ShowPage(int nCmdShow)
{
	BOOL bShow = ShowWindow(nCmdShow);

	if (nCmdShow == SW_SHOWNORMAL)
		SetFocus();
		
	return bShow;
}

void C3dPropertyPage::MovePage(const CRect& rect)
{
	MoveWindow(rect);
}

void C3dPropertyPage::OnActivate()
{
	// Default is to do nothing...

	// Override this to do special processing when the property
	// window is activated.
}

void C3dPropertyPage::OnDeactivate()
{
	// Default is to do nothing...

	// Override this to do special processing when the property
	// window is deactivated.
}

LRESULT C3dPropertyPage::OnPageHelp(WPARAM wParam, LPARAM lParam)
{
	return SendMessage(WM_COMMANDHELP, wParam, lParam);
}

void C3dPropertyPage::GetPageName(CString& strName)
{
	VERIFY(strName.LoadString(IDS_UNNAMED));
}

CSize C3dPropertyPage::GetPageSize()
{
	CRect rect;
	if (m_hWnd == NULL)
		rect.SetRectEmpty();
	else
		GetWindowRect(rect);

	return rect.Size();
}

/////////////////////////////////////////////////////////////////////////////
//	CSlobPage

IMPLEMENT_DYNAMIC(CSlobPage, C3dPropertyPage)

BEGIN_IDE_CONTROL_MAP(CSlobPage, 0, IDS_UNNAMED)
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CSlobPage, C3dPropertyPage)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

CSlobPage::CSlobPage()
{
	m_nValidateID = NULL;

	m_bIgnoreChange = FALSE;
	m_bEnabled = TRUE;
}

BOOL CSlobPage::Create(UINT nIDSheet, CWnd* pWndOwner)
{
	if (nIDSheet == 0)
		nIDSheet = GetControlMap()->m_nCtlID;

	// May need to set our own help ID, since we're creating from a
	// template rather than letting CDialog do its own load code.
	if (m_nIDHelp == 0)
		m_nIDHelp = nIDSheet;

	C3dDialogTemplate dt;
	VERIFY(dt.Load(MAKEINTRESOURCE(nIDSheet)));
	SetStdFont(dt);
	return C3dPropertyPage::CreateIndirect(dt.GetTemplate(), pWndOwner);
}

static BOOL CALLBACK EnableChildWnd(HWND hwnd, LPARAM lParam) 
{
	::EnableWindow(hwnd, lParam);

	return TRUE ;
}

BOOL CSlobPage::EnablePage(BOOL bEnable)
{
	m_bIgnoreChange = TRUE;

	if (!bEnable)
		InitPage();	// init. before disable

	if( (m_bEnabled && !bEnable) || (!m_bEnabled && bEnable) )
		::EnumChildWindows(m_hWnd,
			    (WNDENUMPROC)::EnableChildWnd,
			    (long) bEnable);	

	if (bEnable)
		InitPage();	// init. after enable

	m_bIgnoreChange = FALSE;

	BOOL bOldEnable = m_bEnabled;
	m_bEnabled = bEnable;
	return bOldEnable;
}

void CSlobPage::InitializePage()
{
	EnablePage(m_bEnabled);
}

// Initialize the values of the controls in the property dialog.
// FUTURE:	The need to do this will be eliminated by simply having the
//			selection announce changes to all properties when the selected
//			object changes!

void CSlobPage::InitPage()
{
	CControlMap* pCtlMap;

	if (m_pSlob == NULL)
		return;

	// If several slob types share the same page we may need to change
	// the help ID, so the context sensitive help can reflect the
	// correct slob type.
	//
	UINT nHelpID = GetControlMap()->m_nProp;
	if (nHelpID)
		SetHelpID(nHelpID);

	for (pCtlMap = GetControlMap() + 1;
		pCtlMap->m_nCtlType != CControlMap::null; pCtlMap += 1)
	{
		CEnum* pEnum = NULL;
		if( (pCtlMap->m_dwFlags & CM_EXTRA_MASK) == CM_EXTRA_ENUM )
			pEnum = (CEnum*)pCtlMap->m_nExtra;

		switch (pCtlMap->m_nCtlType)
		{
		// FUTURE: use the code from CSlobPageTab for check-list
		// property page controls
		case CControlMap::checkList:
			ASSERT(FALSE);	// NYI
			break;

		case CControlMap::comboList:
		case CControlMap::comboText:
		case CControlMap::autoComboList:
			if (pEnum != NULL)
			{
				pEnum->FillComboBox((CComboBox*)GetDlgItem(pCtlMap->m_nCtlID),
					TRUE, m_pSlob);
			}
			((CComboBox*)GetDlgItem(pCtlMap->m_nCtlID))->SetExtendedUI();
			goto LThinFont;
			break;

		case CControlMap::list:
		case CControlMap::listText:
			if (pEnum != NULL)
			{
				pEnum->FillListBox((CListBox*)GetDlgItem(pCtlMap->m_nCtlID),
					TRUE, m_pSlob);
			}
			goto LThinFont;
			break;

		case CControlMap::edit:
		case CControlMap::editInt:
		case CControlMap::editNum:
		case CControlMap::autoEdit:
			if ((pCtlMap->m_dwFlags & CM_EXTRA_MASK) == CM_EXTRA_LIMIT)
			{
				((CEdit*)GetDlgItem(pCtlMap->m_nCtlID))->LimitText(pCtlMap->m_nExtra);
			}
			// FALL THROUGH

		case CControlMap::editStrCap:
		case CControlMap::thinText:
		case CControlMap::pathText:
LThinFont:
			GetDlgItem(pCtlMap->m_nCtlID)->SetFont(GetStdFont(font_Normal));
			break;

		case CControlMap::thickText:
			break;
		}
		OnPropChange(pCtlMap->m_nProp);
	}

	// may also need to show/hide SCC status text
	CWnd * pWnd = GetDlgItem(IDC_SCC_STATUS);
	if (pWnd)
	{
		LPSOURCECONTROLSTATUS pInterface;
		BOOL bIsSccActive = FALSE;
		if (SUCCEEDED(theApp.FindInterface(IID_ISourceControlStatus, (LPVOID FAR*)&pInterface)))
		{
			bIsSccActive = (pInterface->IsSccInstalled() == S_OK);
			pInterface->Release();
		}
		pWnd->ShowWindow(bIsSccActive ? SW_SHOWNA : SW_HIDE);
		VERIFY(pWnd = GetDlgItem(IDC_SCC_STATUS_TEXT));
		pWnd->ShowWindow(bIsSccActive ? SW_SHOWNA : SW_HIDE);
	}
}

void CSlobPage::GetPageName(CString& strName)
{
	if (!strName.LoadString(GetControlMap()->m_nLastCtlID))
		VERIFY(strName.LoadString(IDS_UNNAMED));
}

BOOL CSlobPage::PreTranslateMessage(MSG* pMsg)
{
	BOOL bForwardAccel = FALSE;
	
	if (pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_BACK ||
			(pMsg->wParam >= '1' && pMsg->wParam <= '9') ||		// Debug windows.
			(pMsg->wParam >= VK_F1 && pMsg->wParam <= VK_F12))
		{
			bForwardAccel = TRUE; // Forward Alt+Backspace for Undo.
		}
		else if (pMsg->hwnd != m_hWnd && !::IsChild(m_hWnd, pMsg->hwnd))
		{
			// mnemonic key passed to us by the frame.

			HWND hWnd = pMsg->hwnd;
			pMsg->hwnd = GetTopWindow()->GetSafeHwnd();
			if (IsDialogMessage(pMsg))
				return TRUE;
			pMsg->hwnd = hWnd;
		}
	}
	else if (pMsg->message == WM_KEYDOWN)
	{
		ASSERT(!bQuitting);
		
		// If the control key is down, and we are not just pressing the
		// control, shift, or alt key, or its a function key, forward the
		// accelerator.
		//
		// LOCALIZE: This may cause problems for Kanji, since Scott says
		//		They use right control key to generate some chars.
		if ((GetKeyState(VK_CONTROL) < 0 &&
#ifdef _WIN32
			 GetKeyState(VK_MENU) >= 0 &&	// Check for right Alt.
#endif
			 pMsg->wParam != VK_CONTROL && pMsg->wParam != VK_SHIFT &&
			 pMsg->wParam != VK_MENU) ||
			(pMsg->wParam >= VK_F1 && pMsg->wParam <= VK_F12))
		{
			bForwardAccel = TRUE;
		}

		switch (pMsg->wParam)
		{
		case VK_PRIOR:
		case VK_NEXT:
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_F1:
			// Pass these along to the property window
			return FALSE;

		case VK_TAB:
			if (GetKeyState(VK_CONTROL) < 0)
				// Pass these along to the property window
				return FALSE;

			if (!IsChild(GetFocus()))
			{
				if (IsPageDisabled())
					::MessageBeep((UINT) -1);
				// Restore the focus to the first tabstop in the sheet.
				else if (GetKeyState(VK_SHIFT) >= 0)
					SetFocus();
				// Restore the focus to the last tabstop in the sheet.
				else
				{
					CWnd* pWnd = GetTopWindow();
					ASSERT_VALID(pWnd);
					pWnd = pWnd->GetWindow(GW_HWNDLAST);
					while (pWnd != NULL)
					{
						if ((pWnd->GetStyle() & (WS_DISABLED | WS_TABSTOP)) ==
							WS_TABSTOP)
						{
							GotoDlgCtrl(pWnd);
							break;
						}
		
						pWnd = pWnd->GetWindow(GW_HWNDPREV);
					}
				}

				return TRUE;
			}
			else if (IsTabNextFocus(this, GetKeyState(VK_SHIFT) < 0 ?
				GW_HWNDPREV : GW_HWNDNEXT))
			{
				// fix for default button border
				DWORD dwDefID = GetDefID();
				if (HIWORD(dwDefID) == DC_HASDEFID)
				{
					CWnd *pDefBtn = GetDlgItem(LOWORD(dwDefID));
					if (pDefBtn != NULL && pDefBtn->IsWindowEnabled())
						GotoDlgCtrl(pDefBtn);
				}

				if (m_pSheetWnd->SetFocus())
					return TRUE;
			}
			break;

		case VK_DOWN:
		case VK_UP:
		case VK_LEFT:
		case VK_RIGHT:
			bForwardAccel = FALSE;
			break;

		// LOCALIZE:  Don't forward cut, copy, paste.
		case 'X':
		case 'C':
		case 'V':
		case VK_INSERT:	// Ctrl+Insert - copy.
		case VK_HOME:	// Ctrl+Home - MLE beginning of text.
		case VK_END:	// Ctrl+End - MLE end of text.
			bForwardAccel = FALSE;
			break;
		}
	}

	if (bForwardAccel)
	{
		// Uses CSheetWnd::Validate to avoid two alerts.
		if (pMsg->wParam == 'Z' || pMsg->wParam == VK_BACK ||
			m_pSheetWnd->Validate())
		{
			CWnd* pWnd = GetActiveSlobWnd();
			if (pWnd == NULL)
				pWnd = theApp.m_pMainWnd;
				
			pWnd->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		}
		return TRUE;
	}

	return C3dPropertyPage::PreTranslateMessage(pMsg);
}

//	CSlobPage::OnSysCommand
//		Routes Menu command keys to the MiniFrameWnd, which in turn
//		will route them to the parent FrameWnd.

void CSlobPage::OnSysCommand(UINT nID, LONG lParam)
{
	if(nID == SC_KEYMENU)
		m_pSheetWnd->SendMessage(WM_SYSCOMMAND, nID, lParam);
	else
		C3dPropertyPage::OnSysCommand(nID, lParam);
}

BOOL CSlobPage::UndoPendingValidate()
{
	// HACK: Cancel any pending property browser validations.  This
	// will NOT be redoable!
	
	if (m_nValidateID == NULL)
		return FALSE;

	CControlMap* pCtlMap = FindControl(m_nValidateID);
	ASSERT(pCtlMap != NULL);

	if( pCtlMap->m_nCtlType == CControlMap::autoEdit )
	{
		// autoEdit means that we've already called SetStrProp with the user's 
		// change.  To "cancel" this, we need to undo.
		ASSERT( (m_pSlob != NULL) && (m_pSlob->GetSlobWnd() != NULL) );
		theUndoSlob.DoUndo(m_pSlob->GetSlobWnd());
	}
	
	BOOL bRet = OnPropChange(FindControl(m_nValidateID)->m_nProp);
	m_nValidateID = NULL;

	return bRet;
}

// Validate
//
// This gets called when the user clicks on something that might change
// the property page.  This should make sure the property page is valid,
// and, if for some reason it isn't, bring up an error dialog, and set the
// focus back to the appropriate error location.
//
// Other windows (specifically, editor windows which contain objects that
// are viewed by the property page) should call this function before
// changing the selection.	This must not be called inside a setfocus or
// killfocus routine, and this function CAN fail.  If the function fails,
// it *will* have changed the windows focus back to the property sheet,
// in the field that failed..  It also may have put up a message box
// alerting the user to the problem.

BOOL CSlobPage::Validate()
{
	if (m_nValidateID == NULL)
		return TRUE;

	ASSERT(!bQuitting);
	ASSERT(m_pSlob != NULL);

	CControlMap* pCtlMap = FindControl(m_nValidateID);
	ASSERT(pCtlMap != NULL);

	if( pCtlMap->m_nCtlType == CControlMap::autoEdit )
		return TRUE;	// has already been validated automatically

	CWnd* pCtl = GetDlgItem(pCtlMap->m_nCtlID);
	ASSERT(pCtl != NULL);

	BOOL bString = (pCtlMap->m_nCtlType != CControlMap::comboList);

	CString strBuf;
	CString strCurVal;
	int val, curVal;

	if (bString)
	{
		pCtl->GetWindowText(strBuf);
		m_pSlob->GetStrProp(pCtlMap->m_nProp, strCurVal);
	}
	else
	{
		ASSERT(pCtlMap->m_nCtlType == CControlMap::comboList);
		int iSel = ((CComboBox*)pCtl)->GetCurSel();
		val = ((CComboBox*)pCtl)->GetItemData(iSel);
		m_pSlob->GetIntProp(pCtlMap->m_nProp, curVal);
	}

	// special case the string page caption control because
	// what it displays in the edit is not always what is stored
	// in the property
	if (pCtlMap->m_nCtlType == CControlMap::editStrCap)
	{
		StringEditorEditToProp(strBuf.GetBuffer(1025));		// FUTURE(davidga) a string can be much longer
		strBuf.ReleaseBuffer();
		// if there are backslashes in the string we need to go
		// ahead and update
		if (strCurVal == strBuf && strCurVal.Find("\\") == -1)
		{
			m_nValidateID = NULL;
			return TRUE;
		}
	}
	else if (  (bString && (strCurVal == strBuf))
	         || ((!bString) && (curVal == val)))
	{
		m_nValidateID = NULL;
		return TRUE;
	}

	switch (pCtlMap->m_nCtlType)
	{
#ifdef _DEBUG
	default:
		ASSERT(FALSE);
#endif
	case CControlMap::editStrCap:
	case CControlMap::comboText:
	case CControlMap::comboList:
	case CControlMap::symbolCombo:
	case CControlMap::edit:
	case CControlMap::editInt:
	case CControlMap::editNum:
		break;
	}

	theUndoSlob.BeginUndo(IDS_UNDO_PROPEDIT);
	BOOL bValid;
	if (bString)
		bValid = m_pSlob->SetStrProp(pCtlMap->m_nProp, strBuf);
	else
		bValid = m_pSlob->SetIntProp(pCtlMap->m_nProp, val);
	theUndoSlob.EndUndo();

	if(m_pSlob == NULL)
		return FALSE;
		
	if (bValid)
		m_nValidateID = NULL;
	else
	{
		// Only set focus if we don't already have it, or we will have
		// cases where combo boxes go into an infinite loop.

		CWnd* pWndFocus = GetFocus();
		if (pWndFocus == NULL || (pCtl != pWndFocus && !pCtl->IsChild(pWndFocus)))
			pCtl->SetFocus();
	}

	// Since the user made a change on this page, use it as the default...
	m_pSheetWnd->SetDefaultPage();

	return bValid;
}

//	CSlobPage::OnCommand
//		Handle a control notification from a property dialog.

BOOL CSlobPage::OnCommand(UINT wParam, LONG lParam)
{
	// HACK -- Since Combos always send their messages to their original
	// parent, rather than their current parent, the symbol combo's
	// original parent (the nullPage) is forced to forward all messages
	// to the appropriate property page.

	if ((this == &(m_pSheetWnd->m_nullPage)) &&
		(LOWORD(wParam) == IDC_ID) &&
		m_pSheetWnd->m_pCurPage->GetSafeHwnd() != NULL &&
		m_pSheetWnd->m_pCurPage != this)
	{
		CSlobPage *pSlobPage = STATIC_DOWNCAST(CSlobPage, m_pSheetWnd->m_pCurPage);
		return pSlobPage->OnCommand(wParam, lParam);
	}

	CControlMap* pCtlMap = FindControl(LOWORD(wParam));
	CString strBuf;
	int val;
	BOOL bString = FALSE;
	BOOL bSetProp = FALSE;
	BOOL bValidateReq = FALSE;

	if (m_pSlob == NULL || pCtlMap == NULL ||
		(m_nValidateID && LOWORD(wParam) != ((UINT) m_nValidateID) &&
		pCtlMap->m_nCtlType != CControlMap::check &&
		pCtlMap->m_nCtlType != CControlMap::radio))
	{
		// If the control is not found in the map, just pass this
		// message on for default processing...
		// Or if we have a validate ID already, this message is from
		// another control.
LDefault:
		return C3dPropertyPage::OnCommand(wParam, lParam);
	}

#ifdef _WIN32
	HWND hWndCtl = (HWND)(lParam);
	UINT nNotify = HIWORD(wParam);
#else
	HWND hWndCtl = (HWND)LOWORD(lParam);
	UINT nNotify = HIWORD(lParam);
#endif

	ASSERT(m_pSlob != NULL);

	switch (pCtlMap->m_nCtlType)
	{
	default:
		goto LDefault;

	case CControlMap::check:
		if (nNotify != BN_CLICKED)
			goto LDefault;

		val = (int)::SendMessage(hWndCtl, BM_GETCHECK, 0, 0);
		
		// ambiguous becomes checked
		// checked becomes unckeched
		// unchecked becomes checked...
		val = (val + 1) % 2;	// 2->1; 1->0; 0->1; 1->0...
		bSetProp = TRUE;
		break;

	case CControlMap::radio:
		if (nNotify != BN_CLICKED)
			goto LDefault;

		ASSERT( (pCtlMap->m_dwFlags & CM_EXTRA_MASK) == CM_EXTRA_OFFSET );
		val = LOWORD(wParam) - pCtlMap->m_nCtlID + pCtlMap->m_nExtra;
		bSetProp = TRUE;
		break;

	case CControlMap::list:
	case CControlMap::listText:
	{
		if (nNotify != LBN_SELCHANGE)
			goto LDefault;

		int isel = (int)::SendMessage(hWndCtl, LB_GETCURSEL, 0, 0);
		val = (int)::SendMessage(hWndCtl, LB_GETITEMDATA, isel, 0);
		bSetProp = TRUE;

		if (pCtlMap->m_nCtlType == CControlMap::listText)
		{
			((CListBox*)CWnd::FromHandle(hWndCtl))->GetText(val, strBuf);
			bString = TRUE;
		}
		break;
	}

	case CControlMap::autoComboList:
	{
		if (nNotify != CBN_SELCHANGE)
			goto LDefault;
		int isel = (int)::SendMessage(hWndCtl, CB_GETCURSEL, 0, 0);
		val = (int)::SendMessage(hWndCtl, CB_GETITEMDATA, isel, 0);
		bSetProp = TRUE;
		break;
	}

	case CControlMap::autoEdit:
		if( m_bIgnoreChange )
			goto LDefault;

		if( nNotify == EN_CHANGE )
		{
			bSetProp = TRUE;
			CEdit::FromHandle(hWndCtl)->GetWindowText(strBuf);
			bString = TRUE;
			m_nValidateID = pCtlMap->m_nCtlID;
		}
		else if (nNotify == EN_KILLFOCUS)
			bValidateReq = TRUE;
		else
			goto LDefault;
		break;

	case CControlMap::comboList:
	case CControlMap::comboText:
	case CControlMap::symbolCombo:
		if ((nNotify == CBN_EDITCHANGE || nNotify == CBN_SELCHANGE) && !m_bIgnoreChange)
			m_nValidateID = pCtlMap->m_nCtlID;
		else if (nNotify == CBN_SELENDCANCEL || nNotify == CBN_SELENDOK)
			bValidateReq = TRUE;
		break;

	case CControlMap::editStrCap:
	case CControlMap::editInt:
	case CControlMap::edit:
	case CControlMap::editNum:
		if (nNotify == EN_CHANGE && !m_bIgnoreChange)
			m_nValidateID = pCtlMap->m_nCtlID;
		else if (nNotify == EN_KILLFOCUS)
			bValidateReq = TRUE;
		break;
	}

	if (bSetProp)
	{
		ASSERT(!bValidateReq);

		CMultiSlob* pSelection = NULL;
		CSlobWnd* pSlobWnd = m_pSlob->GetSlobWnd();
		if (pSlobWnd != NULL)
			pSelection = (CMultiSlob*)pSlobWnd->GetSelection();
		if (pSelection != NULL && !pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
			pSelection = NULL;

		theUndoSlob.BeginUndo(IDS_UNDO_PROPEDIT);
		if (bString)
			m_pSlob->SetStrProp(pCtlMap->m_nProp, strBuf);
		else
			m_pSlob->SetIntProp(pCtlMap->m_nProp, val);
		theUndoSlob.EndUndo();
	}
	else if( bValidateReq && (m_nValidateID != NULL) )
	{
		m_pSheetWnd->ValidateRequest();
		return C3dPropertyPage::OnCommand(wParam, lParam);
	}

	// Since the user made a change on this page, use it as the default...

	m_pSheetWnd->SetDefaultPage();

	return TRUE;
}

//	CSlobPage::FindControl
//		Given a control (window) id, return the control map entry for it...

CControlMap* CSlobPage::FindControl(UINT nCtlID)
{
	CControlMap* pCtlEntry;

	for (pCtlEntry = GetControlMap() + 1;
		pCtlEntry->m_nCtlType != CControlMap::null; pCtlEntry += 1)
	{
		if (nCtlID >= pCtlEntry->m_nCtlID &&
			nCtlID <= pCtlEntry->m_nLastCtlID)
		{
			return pCtlEntry;
		}
	}

	return NULL;
}

// Given a property id, return the control map entry for it.
CControlMap* CSlobPage::FindProp(UINT nProp)
{
	CControlMap* pCtlEntry;

	for (pCtlEntry = GetControlMap() + 1;
		pCtlEntry->m_nCtlType != CControlMap::null; pCtlEntry += 1)
	{
		if (pCtlEntry->m_nProp == nProp)
			return pCtlEntry;
	}

	return NULL;
}

/* EnableControl
 *	if gpt is valid or ambiguous, the control is visible and enabled.
 *	if gpt is invalid, control is visible but disabled.
 *	if gpt is invisible or avoid, control is invisible and disabled.
 */
void EnableControl(CWnd* pWnd, GPT gpt)
{
	if( pWnd == NULL )
		return;
	pWnd->EnableWindow( (gpt == valid) || (gpt == ambiguous) );
	if( (gpt == invisible) || (gpt == avoid) )
	{
		pWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		pWnd->ShowWindow(SW_SHOWNA);
		pWnd->Invalidate(FALSE);
	}
}

/* ApplyControlMapFlags
 *	There are flags in CControlMap::m_dwFlags that can affect gpt.
 *	In particular, CM_NOMULTISEL disables a control (by making gpt invalid)
 *	if CSlobPage::m_pSlob is a multiple selection.
 *	Also, CM_NOAMBIGUOUS disables a control if the original gpt was ambiguous.
 */
void ApplyControlMapFlags(CControlMap* pCtlMap, GPT& rgpt, CSlob* pSlob)
{
	// CM_NOMULTISEL takes precedence over CM_NOAMBIGUOUS
	if( (pCtlMap->m_dwFlags & CM_NOMULTISEL) && 
		pSlob->IsKindOf(RUNTIME_CLASS(CMultiSlob)) && 
		(pSlob->GetContentList()->GetCount() > 1) )
	{
		// Crazy idea to fix DevStudio96 bug #2861:
		// It isn't really necessary to invalidate a field in a multi-selection
		// if only one item in that selection uses it, and all the others 
		// return invisible.  To test for this, we'd have to loop through all 
		// of the items in pSlob->GetContentList(), and call GetStrProp() 
		// (or GetIntProp() for some types of pCtlMap->m_nCtlType), and see if 
		// more than one returns a gpt != invisible.  -- DavidGa

		rgpt = min(rgpt, invalid);
	}
	else if( (pCtlMap->m_dwFlags & CM_NOAMBIGUOUS) && (rgpt == ambiguous) )
	{
		rgpt = invalid; 
	}
}

// Update the control that reflects a given property in the dialog.
// Called whenever any property of any of the selected objects changes.
BOOL CSlobPage::OnPropChange(UINT nProp)
{
	BOOL bChanged = TRUE;

	// we need to check to make sure the current page isn't pointing
	// at a NULL editor (which would be the case if this is the NUL
	// or MULTIPLE SELECITON page).   If so, do nothing.

	if (m_pSlob == NULL)
		return FALSE;

	CControlMap* pCtlMap = FindProp(nProp);
	if (pCtlMap == NULL)
	{
		if (nProp == P_QuoteName)
			pCtlMap = FindProp(P_ID);

		if (pCtlMap == NULL)
		{
			TRACE("No control map!\n");
			return FALSE;
		}
	}

	CWnd* pWndCtl = GetDlgItem(pCtlMap->m_nCtlID);
	CWnd* pPromptWnd = GetDlgItem(pCtlMap->m_nCtlID - 1);

	if (pWndCtl == NULL)
	{
		TRACE1("Control %d is in the map but not the dialog!\n",
			pCtlMap->m_nCtlID);
		return FALSE;
	}

	GPT gpt;
	int val;
	CString str;
	UINT id;

	switch (pCtlMap->m_nCtlType)
	{
		case CControlMap::check:
		//BLOCK:
		{
			gpt = m_pSlob->GetIntProp(pCtlMap->m_nProp, val);
			ApplyControlMapFlags(pCtlMap, gpt, m_pSlob);

			if( gpt == ambiguous )
				val = 2;	// indeterminate state
			else if( gpt == invalid )
				val = 0;	// disabled state

			((CButton*)pWndCtl)->SetCheck(val);
			EnableControl(pWndCtl, gpt);
		}
		break;

		case CControlMap::editStrCap:
		{
			CString strWindow;
			gpt = m_pSlob->GetStrProp(pCtlMap->m_nProp, str);
			ApplyControlMapFlags(pCtlMap, gpt, m_pSlob);

			if( gpt != valid )
				str.Empty();

			pWndCtl->GetWindowText(strWindow);

            //
            // going from property into edit window
            //
            StringEditorPropToEdit(str);          // GLOBAL func

			if (str != strWindow)
			{
				m_bIgnoreChange = TRUE;
				pWndCtl->SetWindowText(str);
				m_bIgnoreChange = FALSE;
			}
			else
			{
				bChanged = FALSE;
			}

			EnableControl(pWndCtl, gpt);
			EnableControl(pPromptWnd, gpt);
		}
        break;

		case CControlMap::editInt:
		case CControlMap::edit:
		case CControlMap::editNum:
		case CControlMap::autoEdit:
		case CControlMap::thinText:
		case CControlMap::thickText:
		case CControlMap::pathText:
		{
			CString strWindow;
			gpt = m_pSlob->GetStrProp(pCtlMap->m_nProp, str);
			ApplyControlMapFlags(pCtlMap, gpt, m_pSlob);

			if( gpt != valid )
				str.Empty();

			if (pCtlMap->m_nCtlType == CControlMap::pathText)
			{
				// If this is a pathText then we may
				// truncate the text for display

				CRect rect;
				TEXTMETRIC tm;
				int MaxWidth = 0;
				CPath path;
				CString strPath;

				pWndCtl->GetWindowRect(&rect);

				CDC * pDC = pWndCtl->GetDC();
				ASSERT(pDC != (CDC *)NULL);
				pDC->GetTextMetrics(&tm);
				VERIFY(pWndCtl->ReleaseDC(pDC));

				MaxWidth = rect.Width() / tm.tmAveCharWidth;
				// Make sure it hasn't alredy been adjusted first.
				if (str.Find("...") < 0 && path.Create(str))
				{
					path.GetDisplayNameString(strPath, MaxWidth);
					str = strPath;
				}
			}
			pWndCtl->GetWindowText(strWindow);
			if (str != strWindow)
			{
				m_bIgnoreChange = TRUE;
				if (pCtlMap->m_nCtlType != CControlMap::thinText &&
					pCtlMap->m_nCtlType != CControlMap::thickText)
					((CEdit*)pWndCtl)->LimitText(0); // remove limit

				pWndCtl->SetWindowText(str);

				if (pCtlMap->m_nCtlType != CControlMap::thinText &&
					pCtlMap->m_nCtlType != CControlMap::thickText)
				{
					if ((pCtlMap->m_dwFlags & CM_EXTRA_MASK) == CM_EXTRA_LIMIT)
					{
						((CEdit*)pWndCtl)->LimitText(pCtlMap->m_nExtra);
					}
				}

				m_bIgnoreChange = FALSE;
			}
			else
			{
				bChanged = FALSE;
			}
			EnableControl(pWndCtl, gpt);
			EnableControl(pPromptWnd, gpt);
		}
		break;

		case CControlMap::listText:
		case CControlMap::comboText:
// PPMOVE SPECIAL: [matthewt]
// Does this resource handling need to be moved back into VRES?
		case CControlMap::symbolCombo:
		{
			CString strWindow;
			gpt = m_pSlob->GetStrProp(pCtlMap->m_nProp, str);
			ApplyControlMapFlags(pCtlMap, gpt, m_pSlob);

			if( gpt != valid )
				str.Empty();

			if( (pCtlMap->m_nCtlType == CControlMap::symbolCombo) && 
				((gpt == ambiguous) || 
				m_pSlob->IsKindOf(RUNTIME_CLASS(CMultiSlob))) )
			{
				// symbols on multiple selections are dangerous
				gpt = min(gpt, invalid);
			}

			pWndCtl->GetWindowText(strWindow);
			if (str != strWindow)
			{
				m_bIgnoreChange = TRUE;
				if (pCtlMap->m_nCtlType == CControlMap::listText)
				{
					if( gpt == ambiguous )
						((CListBox*)pWndCtl)->SetCurSel(-1);
					else
					{
						int iCurSel = ((CListBox*)pWndCtl)->
							FindStringExact(-1, str);
						if( iCurSel!=LB_ERR )
							((CListBox*)pWndCtl)->SetCurSel(iCurSel);
					}
				}
				else
				{
					if( gpt == ambiguous )
						((CListBox*)pWndCtl)->SetCurSel(-1);
					else
					{
						int iCurSel = ((CComboBox*)pWndCtl)->
							FindStringExact(-1, str);
						if( iCurSel!=CB_ERR )
							((CComboBox*)pWndCtl)->SetCurSel(iCurSel);
					}
				}
				pWndCtl->SetWindowText(str);
				m_bIgnoreChange = FALSE;
			}
			else
			{
				bChanged = FALSE;
			}
			EnableControl(pWndCtl, gpt);
			EnableControl(pPromptWnd, gpt);
		}
		break;

		case CControlMap::radio:
		{
			gpt = m_pSlob->GetIntProp(pCtlMap->m_nProp, val);
			ApplyControlMapFlags(pCtlMap, gpt, m_pSlob);

			if( gpt != valid )
				val = -1;
			
			ASSERT( (pCtlMap->m_dwFlags & CM_EXTRA_MASK) == CM_EXTRA_OFFSET );
			CheckRadioButton(pCtlMap->m_nCtlID, pCtlMap->m_nLastCtlID,
				val == -1 ? val : pCtlMap->m_nCtlID + val - pCtlMap->m_nExtra);
			for (id = pCtlMap->m_nCtlID; id <= pCtlMap->m_nLastCtlID;
				id += 1)
			{
				EnableControl(GetDlgItem(id), gpt);
			}
			break;
		}

		case CControlMap::list:
		{
			gpt = m_pSlob->GetIntProp(pCtlMap->m_nProp, val);
			ApplyControlMapFlags(pCtlMap, gpt, m_pSlob);

			if( gpt != valid )
				val = -1;

			if (val != -1)
			{
				int icnt = ((CListBox*)pWndCtl)->GetCount();
				for (int iitem = 0; iitem < icnt; iitem++)
				{
					int idata = ((CListBox*)pWndCtl)->GetItemData(iitem);
					if (val == idata)
					{
						val = iitem; break;
					}
				}

				// found matching item?
				if (iitem == icnt)
					val = -1; // no so remove selection
			}
			((CListBox*)pWndCtl)->SetCurSel(val);
			EnableControl(pWndCtl, gpt);
			EnableControl(pPromptWnd, gpt);
			break;
		}

		case CControlMap::comboList:
		case CControlMap::autoComboList:
		{
			gpt = m_pSlob->GetIntProp(pCtlMap->m_nProp, val);
			ApplyControlMapFlags(pCtlMap, gpt, m_pSlob);

			if( gpt != valid )
				val = -1;

			if (val != -1)
			{
				int icnt = ((CComboBox*)pWndCtl)->GetCount();
				for (int iitem = 0; iitem < icnt; iitem++)
				{
					int idata = ((CComboBox*)pWndCtl)->GetItemData(iitem);
					if (val == idata)
					{
						val = iitem; break;
					}
				}

				// found matching item?
				if (iitem == icnt)
					val = -1; // no so remove selection
			}
			((CComboBox*)pWndCtl)->SetCurSel(val);
			EnableControl(pWndCtl, gpt);
			EnableControl(pPromptWnd, gpt);
			break;
		}

		case CControlMap::bitmap:
		{
			gpt = m_pSlob->GetIntProp(pCtlMap->m_nProp, val);
			ApplyControlMapFlags(pCtlMap, gpt, m_pSlob);

//			if( gpt == ambiguous )	// REVIEW(davidga): instead of this, I made
//				gpt == invisible;	// CM_NOAMBIGUOUS part of the MAP_BITMAP macro

			EnableControl(pWndCtl, gpt);
			EnableControl(pPromptWnd, gpt);
			GetDlgItem(pCtlMap->m_nCtlID)->Invalidate();
			break;
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////
// Simple Enumeration Implementation
//
// Simple Enumerations are used to fill list and combo boxes with a static
// array of strings.  Create a new class derived from CEnum to deal with
// variable arrays...
//

BOOL CEnum::ContainsVal(int val)
{
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CEnumerator* pEnum = GetNext(pos);
		if (pEnum->val == val)
			return TRUE;
	}

	return FALSE;
}

POSITION CEnum::GetHeadPosition()
{
	CEnumerator* pEnum = GetList();
	if (pEnum->szId == NULL)
		return NULL;
	return (POSITION)pEnum;
}

CEnumerator* CEnum::GetNext(POSITION& pos)
{
	CEnumerator* pEnum = (CEnumerator*)pos;
	ASSERT(pEnum != NULL);
	pos = (POSITION) (pEnum + 1);
	if (((CEnumerator*)pos)->szId == NULL)
		pos = NULL;
	return pEnum;
}

void CEnum::FillListBox(CListBox* pWnd, BOOL bClear, CSlob*)
{
	ASSERT(pWnd != NULL);

	pWnd->SetRedraw(FALSE);

	if (bClear)
		pWnd->ResetContent();

	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CEnumerator* pEnum = GetNext(pos);
		pWnd->SetItemData(pWnd->AddString(pEnum->szId), pEnum->val);
	}

	pWnd->SetRedraw();
	pWnd->Invalidate();
}

void CEnum::FillCheckList(CPropCheckList* pWnd, BOOL bClear, CSlob*)
{
	ASSERT(pWnd != NULL);

	pWnd->SetRedraw(FALSE);

	if (bClear)
		pWnd->ResetContent();

	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CEnumerator* pEnum = GetNext(pos);
		pWnd->AddItem(pEnum->szId, pEnum->val);
	}

	pWnd->SetRedraw();
	pWnd->Invalidate();
}

void CEnum::FillComboBox(CComboBox* pWnd, BOOL bClear, CSlob*)
{
	ASSERT(pWnd != NULL);

	pWnd->SetRedraw(FALSE);

	if (bClear)
		pWnd->ResetContent();

	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CEnumerator* pEnum = GetNext(pos);
		pWnd->SetItemData(pWnd->AddString(pEnum->szId), pEnum->val);
	}

	pWnd->SetRedraw();
	pWnd->Invalidate();
}

// a CEnum like class for localized enumerations
static char BASED_CODE szEmpty[] = "";
CLocalizedEnumerator::CLocalizedEnumerator(UINT aId, int aVal) : szId(szEmpty), id(aId), val(aVal)
{
}

BOOL CLocalizedEnum::ContainsVal(int val)
{
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CLocalizedEnumerator* pEnum = GetNextL(pos);
		if (pEnum->val == val)
			return TRUE;
	}

	return FALSE;
}

POSITION CLocalizedEnum::GetHeadPosition()
{
	CLocalizedEnumerator* pEnum = GetListL();
	if (!pEnum->id)
		return NULL;
	return (POSITION)pEnum;
}


CLocalizedEnumerator* CLocalizedEnum::GetNextL(POSITION& pos)
{
	CLocalizedEnumerator* pEnum = (CLocalizedEnumerator*)pos;
	ASSERT(pEnum != NULL);
	pos = (POSITION) (pEnum + 1);
	if (!((CLocalizedEnumerator*)pos)->id)
		pos = NULL;

	VERIFY(pEnum->szId.LoadString(pEnum->id));
	return pEnum;
}

void CLocalizedEnum::FillListBox(CListBox* pWnd, BOOL bClear, CSlob*)
{
	ASSERT(pWnd != NULL);

	pWnd->SetRedraw(FALSE);

	if (bClear)
		pWnd->ResetContent();

	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CLocalizedEnumerator* pEnum = GetNextL(pos);

		pWnd->SetItemData(pWnd->AddString(pEnum->szId), pEnum->val);
	}

	pWnd->SetRedraw();
	pWnd->Invalidate();
}

void CLocalizedEnum::FillCheckList(CPropCheckList* pWnd, BOOL bClear, CSlob*)
{
	ASSERT(pWnd != NULL);

	pWnd->SetRedraw(FALSE);

	if (bClear)
		pWnd->ResetContent();

	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CLocalizedEnumerator* pEnum = GetNextL(pos);
		pWnd->AddItem(pEnum->szId, pEnum->val);
	}

	pWnd->SetRedraw();
	pWnd->Invalidate();
}

void CLocalizedEnum::FillComboBox(CComboBox* pWnd, BOOL bClear, CSlob*)
{
	ASSERT(pWnd != NULL);

	pWnd->SetRedraw(FALSE);

	if (bClear)
		pWnd->ResetContent();

	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CLocalizedEnumerator* pEnum = GetNextL(pos);
		pWnd->SetItemData(pWnd->AddString(pEnum->szId), pEnum->val);
	}

	pWnd->SetRedraw();
	pWnd->Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// Utility functions

// copy out ofs edit in propert sheet into property
// called from  CSlobPage::OnCommand

void StringEditorEditToProp(char* szBuf)
{
    char *pc = szBuf;
    char *pcOut = szBuf;

	// since the final string length is limited to 255 bytes, and 
	// each byte may be represented by up to 4 characters, the maximum
	// possible length is (4*255) + 1
	// FUTURE(davidga): the string length is now limited to 65535, not 255
    char *pcLimit = szBuf + (4*255) + 1;

    for (; *pc; pc++)
    {
        if (*pc == 0x0d)
        {
            pc++;                       // skip first 0d
            if (*pc == 0x0d)
            {
                pc++;                   // skip second od
                if (*pc == 0x0a)
                    pc++;               // skip 0a
                continue;               // skip soft line breaks
            }
        }

        // CR, LF so use a single LF

        if (*pc == 0)                  // unusual case
        {
            *pcOut++ = '\n';
            if (pcOut > pcLimit) pcOut = pcLimit;
            break;
        }

        if (IsDBCSLeadByte(*pc))
        {
        	*pcOut++ = *pc++;
        	*pcOut++ = *pc;
            if (pcOut > pcLimit)
            	pcOut = pcLimit;
            continue;
        }

        if (*pc == '\\')
        {
            WORD    w;

            pc++;                       // skip first back-slash

            if (*pc == '\\')
            {
                *pcOut++ = '\\';              // convert \\ to single back-slash
                if (pcOut > pcLimit) pcOut = pcLimit;
                continue;
            }

            if (*pc < '0' || *pc > '7')                // not an octal
            {
                switch((unsigned char) *pc)
                {
                case 'a':
                    *pcOut++ = 0x08;
                    break;
                case 'n':
                    *pcOut++ = '\n';
                    break;
                case 'r':
                    *pcOut++ = '\r';
                    break;
                case 't':
                    *pcOut++ = '\t';
                    break;
                default:
                    *pcOut++ = '\\';                    // output the first back slash
                    pc--;                               // process next character as is
                }
                if (pcOut > pcLimit) pcOut = pcLimit;
                continue;
            }

            w = *pc++ - '0';            // first octal digit

            if (*pc < '0' || *pc > '7')
            {
                *pcOut++ = (char)(w & 0x00ff);
                if (pcOut > pcLimit) pcOut = pcLimit;
                pc--;
                continue;
            }
            w = (w<<3) + *pc++ - '0';   // second

            if (*pc < '0' || *pc > '7')
            {
                *pcOut++ = (char)(w & 0x00ff);
                if (pcOut > pcLimit) pcOut = pcLimit;
                pc--;
                continue;
            }

            w = (w<<3) + *pc - '0';     // third

            *pcOut++ = (char)(w & 0x00ff);
            if (pcOut > pcLimit) pcOut = pcLimit;
            continue;
        }
        *pcOut++ = *pc;
        if (pcOut > pcLimit) pcOut = pcLimit;
    }

    *pcOut = 0;
}

// from property into edit
// called from  CSlobPage::OnPropChange

void StringEditorPropToEdit(CString& str)
{
	CString strResult;

    const char *pc = (const char*) str;

    //
    //  unprintable characters as \777 octal
    //
    for (; *pc; pc++)
    {
        if (IsDBCSLeadByte(*pc))
        {
        	strResult += *pc++;
        	strResult += *pc;
        	continue;
        }
        
        if (*pc == '\\')
        {
            strResult += '\\';
            strResult += '\\';
            continue;
        }

        switch((unsigned char) *pc)
        {
        case 0x08:
            strResult += "\\a";
            continue;
        case '\r':
            strResult += "\\r";
            continue;
        case '\n':
            strResult += "\\n";
            continue;
        case '\t':
            strResult += "\\t";
            continue;
        }

        if ((unsigned char)*pc < 0x20)
        {
            WORD w = (unsigned char) *pc;

            strResult += '\\';
            strResult += (char)('0' + (w >> 6));
            strResult += (char)('0' + ((w >> 3) & 7));
            strResult += (char)('0' + (w & 7));
            continue;
        }
        strResult += *pc;
    }

	str = strResult;
}

/////////////////////////////////////////////////////////////////////////////
//	COlePage helper functions

CString _AfxGetHelpDir(const CString& strClsid)
{
	CString strHelp;
	TCHAR szBuff[_MAX_PATH];
	TCHAR szKey[_MAX_PATH];
	HKEY hKey;

	if (!strClsid.IsEmpty())
	{
		wsprintf(szKey, _T("CLSID\\%s"), (LPCTSTR) strClsid);

		if (RegOpenKey(HKEY_CLASSES_ROOT, szKey, &hKey) == ERROR_SUCCESS)
		{
			LONG cb = sizeof(szBuff) / sizeof(TCHAR);

			if (RegQueryValue(hKey, _T("helpdir"), szBuff, &cb) ==
				ERROR_SUCCESS)
			{
				// Found a helpdir key.  Use it.

				strHelp = szBuff;
			}
			else if (RegQueryValue(hKey, _T("InprocServer32"), szBuff, &cb) ==
				ERROR_SUCCESS)
			{
				// Found an InprocServer[32] key.  Strip down to dir name only.

				strHelp = StripName(szBuff);
			}

			RegCloseKey(hKey);
		}
	}

	// If we didn't get anything, just use current directory.
	if (strHelp.IsEmpty())
		strHelp = _T(".");

	return strHelp;
}

/////////////////////////////////////////////////////////////////////////////
//	COlePage

IMPLEMENT_DYNAMIC(COlePage, C3dPropertyPage)

BEGIN_INTERFACE_MAP(COlePage, C3dPropertyPage)
	INTERFACE_PART(COlePage, IID_IPropertyPageSite, PropertyPageSite)
END_INTERFACE_MAP()

//	Page caching.
#define MAX_PAGELIST 20
CObList COlePage::s_listPages;
COleAllPage *COlePage::s_pAllPage = NULL;
LPUNKNOWN *COlePage::s_pObjectCurrent = NULL;
LPUNKNOWN COlePage::s_pSingleObject = NULL;
ULONG COlePage::s_nObjectCurrent;
UINT COlePage::s_nPagesCurrent;
LPCLSID COlePage::s_lpClsID = NULL;

//REVIEW (scotg) HACK HACK HACK  get rid of this when the resource package
// changes its databound controls handling
BOOL COlePage::s_fShowAllPage = TRUE;
BOOL COlePage::GetShowAllPage() 
{ 
	return s_fShowAllPage; 
}
void COlePage::SetShowAllPage(BOOL fSet)
{ 
	s_fShowAllPage = fSet; 
}

// Flag which is set when we may have released all pointers to the
// server of some OLE control (in which case we should call
// ::CoFreeUnusedLibraries before starting a build).
BOOL COlePage::s_bPossibleUnusedServers = FALSE;

BOOL COlePage::InPageCache(ULONG nUnkCnt, LPUNKNOWN *pprgUnk)
{
	BOOL fReturn = FALSE;
	ASSERT(pprgUnk && pprgUnk[0]);
	if(NULL == pprgUnk || NULL == pprgUnk[0])
		return FALSE;

	COleRef<ISpecifyPropertyPages>	srpSPP;
	COleMalloc<GUID> srpElems;
	// optimized for single selection case
	if(1 != nUnkCnt || nUnkCnt != s_nObjectCurrent ||
			s_pSingleObject != *pprgUnk)
		return FALSE;

	// if we have only one page, is it the all page
	if(NULL != s_lpClsID && 1 == s_nPagesCurrent && 
		::IsEqualCLSID(*s_lpClsID, CLSID_NULL))
		return TRUE;

	if(FAILED(pprgUnk[0]->QueryInterface(
		IID_ISpecifyPropertyPages, (void**)&srpSPP)))
		return FALSE;

	CAUUID caguid = {0, (GUID *)NULL};
	if(FAILED(srpSPP->GetPages(&caguid)))
		return FALSE;

	srpElems = caguid.pElems;
	// make sure cElems accounts for the allpage
	if(s_nPagesCurrent == (caguid.cElems + 1) &&
		0 == ::memcmp(s_lpClsID, caguid.pElems, sizeof(GUID *) * caguid.cElems))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UINT COlePage::LoadPages(ULONG nUnkCnt, LPUNKNOWN *pprgUnk)
{
	ASSERT(NULL != pprgUnk);
	if(NULL == pprgUnk)
		return 0;

	// optimize for the single selection case
	if(1 == nUnkCnt && InPageCache(nUnkCnt, pprgUnk))
		return s_nPagesCurrent;

	if(s_lpClsID != NULL)
	{
		AfxFreeTaskMem(s_lpClsID);
		s_lpClsID = NULL;
	}

	// we've special cased this so that if only one
	// unk is passed in, it is assumed that it was
	// not dynamically allocated.  Greater than one
	// means a previous allocation that we are
	// responsible for freeing
	/*
	// The above comment is no longer true, the ownership of
	// the punk array is NOT transfered to the property page.
	// if is up to the slob that allocated the punk array to
	// free this memory up, as it was who allocated it in the
	// first place.
	if(s_nObjectCurrent > 1)
	{
		ASSERT(s_pObjectCurrent);
		AfxFreeTaskMem(s_pObjectCurrent);
	}
	*/
	s_nObjectCurrent = nUnkCnt;
	if(1 == s_nObjectCurrent)
	{
		s_pSingleObject = *pprgUnk;
		s_pObjectCurrent = &s_pSingleObject;
	}
	else
	{
		s_pSingleObject = NULL;
		s_pObjectCurrent = pprgUnk;
	}


	COleRef<ISpecifyPropertyPages>	srpSPP;
	CAUUID caGUID = {0, (GUID*)NULL};
	ULONG nPages = 0;

	for(ULONG nCnt = 0; nCnt < nUnkCnt; ++nCnt)
	{
		if(SUCCEEDED(s_pObjectCurrent[nCnt]->QueryInterface(
			IID_ISpecifyPropertyPages, (void**)&srpSPP)))
		{
			// get pages and if clsid's exist in caGUID, do
			// page intersection
			DoPageIntersection(srpSPP, &caGUID);
			srpSPP.SRelease();
			// if we don't have any elements, then there
			// is no need to continue
			if(0 == caGUID.cElems)
				break;
		}
		else	
		{
			// one within a possible group has failed
			// we must bail on trying to display individual
			// property pages and try for an all page
			caGUID.cElems = 0;
			break;
		}
	}

	if(0 == caGUID.cElems && NULL != caGUID.pElems)
	{
		AfxFreeTaskMem(caGUID.pElems);
		caGUID.pElems = NULL;
	}

	nPages = caGUID.cElems;		
	s_lpClsID = caGUID.pElems;

	LPCLSID lpClsIDNext = s_lpClsID;
	for (int i = 0; i < (int) nPages; i++)
	{
		// Start: IStudio Bug 7249 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// THIS CODE IS ASSOCIATED WITH THE CODE IN ALLPAGE2.CPP (MapPropertyToPage
		// static function).
		// The following code is provided to fix a bug in ISCTRLS.OCX.  The Stock Font,
		// Color and Picture property pages provided by MFC does not work because,
		// the ITypeInfos are not merged in ISCTRLS.OCX and so, the enumeration of
		// properties does not work with these property pages, but they do work in the
		// MS Stock Propperty Pages.  So we remove these property pages and let the user
		// use the AllPage to edit those properties.
		
		CFrameWnd*	pFrame = ((CFrameWnd*)AfxGetMainWnd())->GetActiveFrame();
		ASSERT(pFrame);
		if(pFrame)
		{
			CDocument*	pDoc = pFrame->GetActiveDocument();
			if(pDoc != NULL && pDoc->IsKindOf(RUNTIME_CLASS(CIPCompDoc)))
			{
				// Only the Layout & Object Editor docs contain forms96==IID_IForm.
				// QI for IForm.  If successful then we know we have a Layout ||
				// Object Editor Doc, since IID_IForm is Forms96 only.
				LPOLEOBJECT pOleObj = ((CIPCompDoc*)pDoc)->GetOleObject(); // Un-AddRefed
				if(pOleObj != NULL)
				{
					COleRef<IUnknown> srpForms96;
					if(SUCCEEDED(pOleObj->QueryInterface(IID_IForm, (void**)&srpForms96)))
					{
						if(IsEqualCLSID(s_lpClsID[i], CLSID_CFontPropPage) ||
							IsEqualCLSID(s_lpClsID[i], CLSID_CColorPropPage) ||
							IsEqualCLSID(s_lpClsID[i], CLSID_CPicturePropPage))
							continue;
					}	
				}
			}
		}
		// End: IStudio Bug 7249 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		
		// Remove CLSIDs for pages that fail to load.
		if (GetPropPage(s_lpClsID[i]) == NULL)
			continue;

		if (lpClsIDNext != s_lpClsID + i)
			*lpClsIDNext = s_lpClsID[i];

		lpClsIDNext++;
	}

	UINT nPagesCurrent = (UINT) (lpClsIDNext - s_lpClsID);

//REVIEW (scotg) HACK HACK HACK get rid of this when the resource package
// changes how it handles data bound controls
	if(GetShowAllPage())
	{
		s_nPagesCurrent = AddAllPageToList(nPagesCurrent, &caGUID);
		s_lpClsID = caGUID.pElems;		// AddAllPageToList reallocs
	}
	else
		s_nPagesCurrent = nPagesCurrent;

	return s_nPagesCurrent;
}

UINT COlePage::LoadAllPageOnly(ULONG nUnkCnt, LPUNKNOWN *pprgUnk)
{
	ASSERT(NULL != pprgUnk);
	if(NULL == pprgUnk)
		return 0;

	// Store the array of IUnknown's

	/*
	// Ownership of the punk array is NOT transfered to the property page.
	// if is up to the slob that allocated the punk array to
	// free this memory up, as it was who allocated it in the
	// first place.
	if(s_nObjectCurrent > 1)
	{
		ASSERT(s_pObjectCurrent);
		AfxFreeTaskMem(s_pObjectCurrent);
	}
	*/
	s_nObjectCurrent = nUnkCnt;
	if(1 == s_nObjectCurrent)
	{
		s_pSingleObject = *pprgUnk;
		s_pObjectCurrent = &s_pSingleObject;
	}
	else
	{
		s_pSingleObject = NULL;
		s_pObjectCurrent = pprgUnk;
	}

	// Free old CLSID array of pages

	if(s_lpClsID != NULL)
	{
		AfxFreeTaskMem(s_lpClsID);
		s_lpClsID = NULL;
	}

	// Create a new CLSID array of pages with just the all page in it

	if(GetShowAllPage())
	{
		CAUUID caGUID = {0, (GUID*)NULL};
		s_nPagesCurrent = AddAllPageToList(0, &caGUID);
		s_lpClsID = caGUID.pElems;		// AddAllPageToList reallocs
	}
	else
		s_nPagesCurrent = 0;

	return s_nPagesCurrent;
}

UINT COlePage::AddAllPageToList(UINT nPagesCurrent, CAUUID *pcaGUID)
{

	// make sure all the objects support IDispatch
	// before we go off and create the all page object
	COleRef<IPerPropertyBrowsing>	srpPPB;
	for(ULONG nCnt = 0; nCnt < s_nObjectCurrent; ++nCnt)
	{
		if(FAILED(s_pObjectCurrent[nCnt]->QueryInterface(
			IID_IDispatch, (void**)&srpPPB)))
		{
			// OK, no all page
			return nPagesCurrent;
		}
		srpPPB.SRelease();
	}

	// look to see if I have space available in the
	// GUID array, if so use it, else resize the array
	// to make room for the allpage
	if(nPagesCurrent < pcaGUID->cElems)
	{
		// use the memory we already have
		pcaGUID->pElems[nPagesCurrent] = CLSID_NULL;
		++nPagesCurrent;
	}
	else
	{
		ASSERT(nPagesCurrent == pcaGUID->cElems);
		LPVOID pVoid = ::CoTaskMemRealloc(pcaGUID->pElems, 
								(sizeof(GUID) * (pcaGUID->cElems + 1)));
		ASSERT(NULL != pVoid);
		if(NULL == pVoid)
			return nPagesCurrent;

		pcaGUID->pElems = (GUID*)pVoid;
		pcaGUID->pElems[pcaGUID->cElems] = CLSID_NULL;
		++(pcaGUID->cElems);
		++nPagesCurrent;
	}

	return nPagesCurrent;
}

HRESULT COlePage::DoPageIntersection(ISpecifyPropertyPages *pSPP, 
	CAUUID *pcaGUID)
{
	ASSERT(NULL != pSPP);
	ASSERT(NULL != pcaGUID);

	if (pcaGUID != NULL)
		pcaGUID->cElems = 0;

	if(NULL == pSPP || NULL == pcaGUID)
		return E_INVALIDARG;

	CAUUID caguid;
	HRESULT hr;

	// Initialize caguid here because at least one ActiveX Control
	// (The MS Investor NewsClient Control) returns hr == 1 from the
	// ISpecifyPropertyPages::GetPages() call, and doesn't fill in
	// the caguid structure.  -mgrier 11/03/96
	caguid.cElems = 0;
	caguid.pElems = NULL;

	hr = pSPP->GetPages(&caguid);
	if (FAILED(hr))
		return hr;

	if(0 == caguid.cElems)
	{
		ASSERT(NULL == caguid.pElems);
		pcaGUID->cElems = 0;
		return S_OK;
	}

	ASSERT(caguid.pElems);
	// is this the first time?
	if(0 == pcaGUID->cElems)
	{
		pcaGUID->cElems = caguid.cElems;
		pcaGUID->pElems = caguid.pElems;
		return S_OK;
	}

	// ok, so this is not the first time, now 
	// we've got to actually do some real work
	// grab some heap to store the common guids
	ULONG nNext = 0;
	for(ULONG nCnt1 = 0; nCnt1 < pcaGUID->cElems; ++nCnt1)
	{
		for(ULONG nCnt2 = 0; nCnt2 < caguid.cElems; ++nCnt2)
		{
			if(::IsEqualCLSID(caguid.pElems[nCnt2], pcaGUID->pElems[nCnt1]))
			{
				pcaGUID->pElems[nNext] = pcaGUID->pElems[nCnt1];
				++nNext;
				break;
			}
		}
	}

	// rebuild the common list
	if(!nNext)
	{
		AfxFreeTaskMem(pcaGUID->pElems);
		pcaGUID->pElems = NULL;
	}
	pcaGUID->cElems = nNext;
 
	return S_OK;
}

void COlePage::SetUnusedServers(BOOL fUnused /*= TRUE*/) 
{ 
	s_bPossibleUnusedServers = fUnused; 
}

C3dPropertyPage* COlePage::GetPropPage(UINT iPage)
{
	ASSERT(s_pObjectCurrent != NULL);
	ASSERT(s_lpClsID != NULL);
	ASSERT(iPage < s_nPagesCurrent);
	
	return GetPropPage(s_lpClsID[iPage]);
}

C3dPropertyPage* COlePage::GetPropPage(REFCLSID clsid)
{
	COlePage* pPage;

	// see if we want the all page
	if(::IsEqualCLSID(CLSID_NULL, clsid))
	{
		if(NULL == s_pAllPage)
		{
			s_pAllPage = new COleAllPage();
		}
		return s_pAllPage;
	}

	// See if there is a matching page in the cache.
	POSITION pos = s_listPages.GetHeadPosition();
	while (pos != NULL)
	{
		pPage = (COlePage*) s_listPages.GetAt(pos);
		if (IsEqualCLSID(clsid, pPage->m_clsid))
			break;
		s_listPages.GetNext(pos);
	}

	if (pos != NULL)
		s_listPages.RemoveAt(pos);
	else
	{
		// If there was no match, create a new page.
		pPage = new COlePage(clsid);
		if (!pPage->IsUsable())			// not registered, or other problem
		{
			delete pPage;
			return NULL;
		}

		// If the cache size reaches its limit, remove the least recently
		// used page.

		if (s_listPages.GetCount() >= MAX_PAGELIST)
		{
			COlePage* pLRUPage = (COlePage*) s_listPages.RemoveTail();
			pLRUPage->m_pSheetWnd->RemovePageFromUse(pLRUPage);
			delete pLRUPage;
		}
	}

	// Promote the current page to most recently used status.
	s_listPages.AddHead(pPage);

	return pPage;
}

void COlePage::Cleanup()
{
	C3dPropertyPage* pCurPage = NULL;
	CSheetWnd* pSheetWnd = GetPropertyBrowser();

	if(pSheetWnd != NULL && ::IsWindow(pSheetWnd->m_hWnd))
	{
		// Force the current page to be updated.
		UpdatePropertyBrowser();
		pCurPage = pSheetWnd->GetCurrentPage();
	}

	BOOL bReplaceCurPage = FALSE;

	// clean up the OLE page cache.
	while (!s_listPages.IsEmpty())
	{
		COlePage *pPage = (COlePage *)s_listPages.RemoveHead();
		ASSERT(pPage->IsKindOf(RUNTIME_CLASS(COlePage)));

		if (pPage == pCurPage)
		{
			// Cannot delete the page because it is the currently visible one.
			// We will delete all the others, then put this one back in the cache.
			bReplaceCurPage = TRUE;
		}
		else
		{
			delete pPage;
		}
	}
	if (bReplaceCurPage)
	{
		s_listPages.AddHead(pCurPage);
	}

	if(NULL != s_pAllPage && s_pAllPage != pCurPage)
	{
		delete s_pAllPage;
		s_pAllPage = NULL;
	}

	// we've special cased this so that if only one
	// unk is passed in, it is assumed that it was
	// not dynamically allocated.  Greater than one
	// means a previous allocation that we are
	// responsible for freeing
	/*
	// The above comment is no longer true, the ownership of
	// the punk array is NOT transfered to the property page.
	// if is up to the slob that allocated the punk array to
	// free this memory up, as it was who allocated it in the
	// first place.
	if(s_nObjectCurrent > 1)
	{
		AfxFreeTaskMem(s_pObjectCurrent);
	}
	*/
	s_pObjectCurrent = NULL;
	s_nObjectCurrent = 0;

	if (s_lpClsID != NULL)
	{
		AfxFreeTaskMem(s_lpClsID);
		s_lpClsID = NULL;
	}

	if (s_bPossibleUnusedServers)
	{
		// We might have released all pointers owned by an OLE control
		// server; if so we want the system to unload it now so it
		// can be rebuilt if the user desires.
		::CoFreeUnusedLibraries();
		s_bPossibleUnusedServers = FALSE;
	}
}

COlePage::COlePage(REFCLSID clsid)
	: m_size(0, 0)
{
	USES_CONVERSION;

	m_bActive = FALSE;
	m_bVisible = FALSE;
	m_bTranslatingAccel = FALSE;
	m_clsid = clsid;

	// Attempt to create the Property Page object
	HRESULT hr = ::CoCreateInstance(m_clsid, NULL, CLSCTX_INPROC_SERVER,
		(REFIID)IID_IPropertyPage, (void **)&m_lpPropPage);
		
	if (FAILED(hr))
	{
		m_lpPropPage = NULL;
		ASSERT(!IsUsable());
		return;				// caveat caller
	}

	// Bind the Property Page to its objects
	try
	{
		m_lpPropPage->SetPageSite(&m_xPropertyPageSite);
	}
	catch(...)
	{
	}

	PROPPAGEINFO pgi;
	pgi.cb = sizeof(PROPPAGEINFO);
	BOOL bGetPageInfo = FALSE;
	try
	{
		hr = m_lpPropPage->GetPageInfo(&pgi);
		if (SUCCEEDED(hr))
			bGetPageInfo = TRUE;
	}
	catch(...)
	{
	}
	if( bGetPageInfo )
	{
		m_size = pgi.size;
		if (pgi.pszTitle != NULL)
		{
			// HACK: Keep OLE controls from using "General" as Tab text.
			//		 Use "Control" instead.
			CString strGeneral;
			strGeneral.LoadString(IDS_GENERAL);
			if (strGeneral.CompareNoCase(W2A(pgi.pszTitle)) == 0)
				m_strName.LoadString(IDS_PROP_CONTROL);
			else
				m_strName = pgi.pszTitle;
		}

		AfxFreeTaskMem(pgi.pszTitle);
		AfxFreeTaskMem(pgi.pszDocString);
		AfxFreeTaskMem(pgi.pszHelpFile);
	}

	if (m_strName.IsEmpty())
		C3dPropertyPage::GetPageName(m_strName);
}

COlePage::~COlePage()
{

	// since we can't get mfc based controls to release the addref
	// they are holding on us from a previous call to their
	// IPropertyPage::SetPageSite(&m_xPropertyPageSite),
	// we manage the ref count here in our destructor
	ASSERT(m_dwRef > 0);
	m_dwRef = 9999;			// add a ref count guard

	if (m_bActive)
		DestroyWindow();

	if (m_lpPropPage != NULL)
	{
		// OLE protocol requires a SetObjects will
		// before the page is released.  This gives the
		// page a chance to cleanup.
		try
		{
			// we should be calling IPropertyPage::SetPageSite(NULL)
			// to get the control to release an addref it is holding 
			// on our prop page site (from a previous call to 
			// m_lpPropPage->SetPageSite(&m_xPropertyPageSite);)
			// We can't do this due to an MFC bug that does not 
			// correctly handle the call (they crash).  Therefore
			// we're explicitly taking ownership of the addref and
			// performing a release on ourselves.  By the way, VB5 
			// controls do the right thing.  They handle a NULL
			// page site.
			m_lpPropPage->SetObjects(0, NULL);
			m_lpPropPage->Release();
		}
		catch(...)
		{
		}
		m_lpPropPage = NULL;
		s_bPossibleUnusedServers = TRUE;
	}

	s_bPossibleUnusedServers = TRUE;
	// account for not being able to call SetPageSite(NULL)
	m_dwRef = 1;
}

BOOL COlePage::Create(UINT, CWnd* pWndOwner)
{
	ASSERT_VALID(pWndOwner);
	ASSERT(m_lpPropPage != NULL);

	// We must initialize the page (i.e. set its object) before creating it ...
	// some controls will crash if their property pages are invoked with 0
	// objects.
	// 
	
	// IStudio bug #210, components can have a setobjects call fail.  when
	// this occurs they will assert if Activate is called.  since 
	// InitializePage doen't return a value a setobjects call has been
	// placed here as well as in the InializePage
	//InitializePage();
	try
	{
		if(FAILED(m_lpPropPage->SetObjects(s_nObjectCurrent, s_pObjectCurrent)))
			return FALSE;

		CRect rect(0, 0, m_size.cx, m_size.cy);
		if (FAILED(m_lpPropPage->Activate(pWndOwner->m_hWnd, rect, FALSE)))
			return FALSE;
	}
	catch(...)
	{
		return FALSE;
	}

	m_bActive = TRUE;
	return TRUE;
}

BOOL COlePage::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(m_lpPropPage != NULL);
	
	BOOL bForwardAccel = FALSE;
	
	if (pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_BACK ||
			(pMsg->wParam >= '1' && pMsg->wParam <= '9') ||		// Debug windows.
			(pMsg->wParam >= VK_F1 && pMsg->wParam <= VK_F12))
		{
			bForwardAccel = TRUE; // Forward Alt+Backspace for Undo.
		}
	}
	else if (pMsg->message == WM_KEYDOWN)
	{
		// If the control key is down, and we are not just pressing the
		// control, shift, or alt key, or its a function key, forward the
		// accelerator.
		//
		// LOCALIZE: This may cause problems for Kanji, since Scott says
		//		They use right control key to generate some chars.
		if ((GetKeyState(VK_CONTROL) < 0 &&
			 GetKeyState(VK_MENU) >= 0 &&	// Check for right Alt.
			 pMsg->wParam != VK_CONTROL && pMsg->wParam != VK_SHIFT &&
			 pMsg->wParam != VK_MENU) ||
			(pMsg->wParam >= VK_F1 && pMsg->wParam <= VK_F12))
		{
			bForwardAccel = TRUE;
		}

		switch (pMsg->wParam)
		{
		case VK_PRIOR:
		case VK_NEXT:
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_F1:
			// Pass these along to the property window
			return FALSE;

		case VK_TAB:
			if (GetKeyState(VK_CONTROL) < 0)
				// Pass these along to the property window
				return FALSE;

		case VK_DOWN:
		case VK_UP:
		case VK_LEFT:
		case VK_RIGHT:
			bForwardAccel = FALSE;
			break;

		// LOCALIZE:  Don't forward cut, copy, paste.
		case 'X':
		case 'C':
		case 'V':
		case VK_INSERT:	// Ctrl+Insert - copy.
		case VK_HOME:	// Ctrl+Home - MLE beginning of text.
		case VK_END:	// Ctrl+End - MLE end of text.
			bForwardAccel = FALSE;
			break;

		}
	}

	if (bForwardAccel)
	{
		// Uses CSheetWnd::Validate to avoid two alerts.
		if (pMsg->wParam == 'Z' || pMsg->wParam == VK_BACK ||
			m_pSheetWnd->Validate())
		{
			CWnd* pWnd = GetActiveSlobWnd();
			if (pWnd == NULL)
				pWnd = theApp.m_pMainWnd;
				
			pWnd->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		}
		return TRUE;
	}

	BOOL bTranslated = FALSE;
	if (!m_bTranslatingAccel)
	{
		m_bTranslatingAccel = TRUE;
		bTranslated = (m_lpPropPage->TranslateAccelerator(pMsg) == S_OK);
		m_bTranslatingAccel = FALSE;
	}

	// REVIEW: Possibly we should call the base class here
	return bTranslated;
}

void COlePage::InitializePage()
{
	ASSERT(m_lpPropPage != NULL);

	if (m_bVisible)
		m_pSheetWnd->UpdateWindow();

	try
	{
		m_lpPropPage->SetObjects(s_nObjectCurrent, s_pObjectCurrent);
	}
	catch(...)
	{
	}

	C3dPropertyPage::InitializePage();
}

void COlePage::TermPage()
{
	try
	{
		if(m_lpPropPage)
			m_lpPropPage->SetObjects(0, NULL);
	}
	catch(...)
	{
	}

	C3dPropertyPage::TermPage();
}

BOOL COlePage::ShowPage(int nCmdShow)
{
	ASSERT(m_lpPropPage != NULL);

	BOOL bSuccess = FALSE;
	try
	{
		if( SUCCEEDED(m_lpPropPage->Show(nCmdShow)) )
			bSuccess = TRUE;
	}
	catch(...)
	{
	}

	if (bSuccess)
		m_bVisible = !(nCmdShow == SW_HIDE);

	return bSuccess;
}

void COlePage::MovePage(const CRect& rect)
{
	ASSERT(m_lpPropPage != NULL);

	try
	{
		m_lpPropPage->Move(rect);
	}
	catch(...)
	{
	}
}

BOOL COlePage::DestroyWindow()
{
	if (m_bActive)
	{
		ASSERT(m_lpPropPage != NULL);

		try
		{
			m_lpPropPage->Deactivate();
			m_bActive = FALSE;
		}
		catch(...)
		{
			m_bActive = FALSE;
		}
	}
	return TRUE;
}

BOOL COlePage::Validate()
{
	ASSERT(m_lpPropPage != NULL);

	try
	{
		if (m_lpPropPage->IsPageDirty() == S_FALSE)
			return TRUE;
		else
		{
			COleRef<IPropertyPageUndoString> srpPPUS;
			LPOLESTR pStr = NULL;
			m_pSlob->BeginUndo(IDS_UNDO_OLE_PROPS);
			BOOL bSuccess = SUCCEEDED(m_lpPropPage->Apply());
			// give inplace components a chance to provide there own
			// undo string.  they can do this if they provide an
			// IPropertyPageUndoString interface
			if(bSuccess && m_pSlob->IsKindOf(RUNTIME_CLASS(CIPCompSlob)) && 
				SUCCEEDED(m_lpPropPage->QueryInterface(IID_IPropertyPageUndoString, 
				(void**)&srpPPUS)))
			{
				ASSERT(srpPPUS != NULL);
				((CIPCompSlob*)m_pSlob)->SetUndoStringMgr(srpPPUS);
			}
			m_pSlob->EndUndo(!bSuccess);

			return bSuccess;
		}
	}
	catch(...)
	{
		return FALSE;
	}
}

BOOL COlePage::UndoPendingValidate()
{
	ASSERT(m_lpPropPage != NULL);

	// Should not try to validate if the page is not visible (the
	// corresponding object might have been deleted (Bug Dev Stud 96 #743)
	try
	{
		if (!IsPropertyBrowserVisible() || m_lpPropPage->IsPageDirty() == S_FALSE)
			return FALSE;

		return SUCCEEDED(m_lpPropPage->SetObjects(s_nObjectCurrent, s_pObjectCurrent));
	}
	catch(...)
	{
	}
	return FALSE;
}

LRESULT COlePage::OnPageHelp(WPARAM wParam, LPARAM lParam)
{
	USES_CONVERSION;

	ASSERT(m_lpPropPage != NULL);
	LPOLESTR wzTemp = NULL;
	CString str1;

	try
	{
		if(SUCCEEDED(::StringFromCLSID(m_clsid, &wzTemp)) &&
			SUCCEEDED(m_lpPropPage->Help(A2CW((LPCTSTR)_AfxGetHelpDir(CString(OLE2T(wzTemp)))))))
				goto noerror;
	}
	catch(...)
	{
	}

	str1.LoadString(IDS_PROP_ERROR_NOPAGEHELP);
	AfxMessageBox(str1);

noerror:
	if(NULL != wzTemp)
		AfxFreeTaskMem(wzTemp);
	return S_OK;
}

void COlePage::GetPageName(CString& strName)
{
	ASSERT(!m_strName.IsEmpty());

	strName = m_strName;
}

CSize COlePage::GetPageSize()
{
	return m_size;
}

void COlePage::EditProperty(DISPID dispid)
{
	ASSERT(m_lpPropPage != NULL);

	LPPROPERTYPAGE2 lpPropPage2;
	try
	{
		if (FAILED(m_lpPropPage->QueryInterface(IID_IPropertyPage2,
			(LPVOID*) &lpPropPage2)))
		{
			return;
		}
	}
	catch(...)
	{
		return;
	}

	lpPropPage2->EditProperty(dispid);
	lpPropPage2->Release();
}

/////////////////////////////////////////////////////////////////////////////
// COlePage::XPropertyPageSite

STDMETHODIMP COlePage::XPropertyPageSite::OnStatusChange(DWORD dwFlags)
{
	METHOD_PROLOGUE_EX(COlePage, PropertyPageSite)

	ASSERT_VALID(pThis);

	// Hack to work around ImageList version 5 control which sends an OnStatusChange to its
	// site when getting the property page information.  If the name of the page
	// hasn't initialized to non-empty, we don't actually do anything.
	// -mgrier 10/28/96

	if (!pThis->m_strName.IsEmpty())
	{
		ASSERT(pThis->m_lpPropPage != NULL);

		if (dwFlags & PROPPAGESTATUS_VALIDATE)
			pThis->m_pSheetWnd->ValidateRequest();
	}

	return NOERROR;
}

STDMETHODIMP COlePage::XPropertyPageSite::GetLocaleID(
	LCID FAR* pLocaleID)
{
	METHOD_PROLOGUE_EX(COlePage, PropertyPageSite)
	ASSERT_VALID(pThis);
	ASSERT(pLocaleID != NULL);

	if (pLocaleID == NULL)
		return ResultFromScode(E_POINTER);

	*pLocaleID = GetThreadLocale();
	return NOERROR;
}


STDMETHODIMP COlePage::XPropertyPageSite::GetPageContainer(
	LPUNKNOWN FAR* ppUnk)
{
	METHOD_PROLOGUE_EX(COlePage, PropertyPageSite)
	ASSERT_VALID(pThis);
	ASSERT(ppUnk != NULL);

	if (ppUnk != NULL)
		return ResultFromScode(E_POINTER);

	*ppUnk = NULL;
	return ResultFromScode(E_FAIL);
}


STDMETHODIMP COlePage::XPropertyPageSite::TranslateAccelerator(
	LPMSG lpMsg)
{
	METHOD_PROLOGUE_EX(COlePage, PropertyPageSite)
	ASSERT_VALID(pThis);
	ASSERT(pThis->m_lpPropPage != NULL);

	if (lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_TAB)
	{
		pThis->m_pSheetWnd->SetFocus();
		return NOERROR;
	}

	if (pThis->m_pSheetWnd->PreTranslateMessage(lpMsg))
		return NOERROR;

	return ResultFromScode(S_FALSE);
}


STDMETHODIMP_(ULONG) COlePage::XPropertyPageSite::AddRef()
{
	// Delegate to our exported AddRef.

	METHOD_PROLOGUE_EX(COlePage, PropertyPageSite)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) COlePage::XPropertyPageSite::Release()
{
	// Delegate to our exported Release.

	METHOD_PROLOGUE_EX(COlePage, PropertyPageSite)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP COlePage::XPropertyPageSite::QueryInterface(
	REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.

	METHOD_PROLOGUE_EX(COlePage, PropertyPageSite)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
//	Global property window API

BOOL CreatePropertyBrowser()
{
	ASSERT(g_PropSheet == NULL);
	
	g_PropSheet = new CSheetWnd;
	if (!g_PropSheet->Create())
		return FALSE;

	//  PPMOVE : [matthewt]
	// need to do this in a VSHELL across package kinda way
	// ReadPropertyBrowserProfile();
	
	return TRUE;
}

void DestroyPropertyBrowser()
{
	if (g_PropSheet != NULL)
	{
		g_PropSheet->DestroyWindow();
		g_PropSheet = NULL;
	}
}

CSheetWnd* GetPropertyBrowser()
{
	return g_PropSheet;
}

//	SetPropertyBrowserVisible
//		Used for operations that need to hide the property window
//		temporarily, but do not want to change the push pin state.

void SetPropertyBrowserVisible(BOOL bVisible)
{
	// IStudio needs to progamatically show and hide
	// the property browser,  but under certain conditions
	// (ie. when processing File.Exit, istudio will 
	// call this routine to hide the browser, but the
	// browser has already been destroyed by the shell.
	if(!g_PropSheet || !::IsWindow(g_PropSheet->m_hWnd))
		return;

	g_PropSheetVisible = bVisible;
	if (!bVisible)
		ShowPropertyBrowser(FALSE);
}

void ShowPropertyBrowser(BOOL bShow /* = TRUE */)
{
	ASSERT_VALID(g_PropSheet);

	BOOL fPoppingProperties;
	theApp.m_theAppSlob.GetIntProp(P_PoppingProperties, fPoppingProperties);

	g_PropSheet->ShowWindow(bShow ? (fPoppingProperties ? SW_SHOW : SW_SHOWNOACTIVATE) : SW_HIDE);
}

void PinPropertyBrowser(BOOL bPin /* = TRUE */)
{
	if(!g_PropSheet || !::IsWindow(g_PropSheet->m_hWnd) || !IsPropertyBrowserVisible())
		return;
	g_PropSheet->SetPushPin(bPin);
}

BOOL IsPropertyBrowserPinned()
{
	// IStudio needs to progamatically Pin 
	// the property browser,  but under certain conditions
	// (ie. when processing File.Exit, istudio will 
	// call this routine to hide the browser, but the
	// browser has already been destroyed by the shell.
	if(!g_PropSheet || !::IsWindow(g_PropSheet->m_hWnd))
		return FALSE;
		
	ASSERT_VALID(g_PropSheet);
	if(!IsPropertyBrowserVisible())
	{
		ASSERT(!g_PropSheet->IsPinned());
		return FALSE;
	}
	return g_PropSheet->IsPinned();
}

void InvalidatePropertyBrowser()
{
	ASSERT_VALID(g_PropSheet);
	
	g_PropSheet->Invalidate(FALSE);
}

void UpdatePropertyBrowser()
{
	ASSERT_VALID(g_PropSheet);
	
	g_PropSheet->IdleUpdate();
}

BOOL IsPropertyBrowserVisible()
{
	ASSERT_VALID(g_PropSheet);
	
	return g_PropSheetVisible;
}

BOOL ValidatePropertyBrowser()
{
	if (g_PropSheet == NULL)
		return TRUE;
	
	return g_PropSheet->Validate();
}

void ClosePropertyBrowser()
{
	ASSERT_VALID(g_PropSheet);
	
	g_PropSheet->SendMessage(WM_CLOSE);
}

void SetPropertyBrowserSelection(CSlob* pSlob, int nPage /* = -1 */, 
	CSheetWnd::SELSTYLE style /* = editor */)
{
	if (g_PropSheet == NULL) // happens during exit...
		return;

	ASSERT_VALID(g_PropSheet);
	
	if (pSlob == NULL)
		style = CSheetWnd::none;
	
	g_PropSheet->SelectionChange(style, pSlob, nPage);
	
	if (pSlob == NULL)
		UpdatePropertyBrowser();

	g_PropSheet->NukeInforms();
}

void ResetPropertyBrowserSelectionSlob(CSlob* pSlob)
{
	SetPropertyBrowserSelection(pSlob);
}

void ActivatePropertyBrowser()
{
	__try 
	{
		ASSERT_VALID(g_PropSheet);
		ASSERT(!bQuitting);
		
		g_PropSheet->IdleUpdate();	// Make sure slobs are up to date.
		g_PropSheet->SetActiveWindow();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		AfxMessageBox (IDS_ERROR_PROPERTYPAGES, MB_ICONEXCLAMATION |MB_OK);
		TRACE ("Error: caught exception in proppage.cpp, ActivatePropertyBrowser()\n");
	}
}

void DeactivatePropertyBrowser()
{
	if (g_PropSheet != NULL &&
		CWnd::GetActiveWindow() == g_PropSheet)
	{
		ASSERT(!bQuitting);
		AfxGetApp()->m_pMainWnd->SetFocus();
	}
}

void CancelPropertyBrowser()
{
	ASSERT_VALID(g_PropSheet);
	
	g_PropSheet->CancelPage();
}

BOOL BeginPropertyBrowserEdit(UINT nProp, UINT nChar, UINT nRepeat, UINT nFlags)
{
	ASSERT_VALID(g_PropSheet);
	
	CWnd* pEdit = g_PropSheet->SetEditFocus(nProp);
	if (pEdit == NULL)
		return FALSE;
	
	g_PropSheetVisible = TRUE;
	
	if (nChar != 0)
		pEdit->PostMessage(WM_CHAR, nChar, MAKELPARAM(nRepeat, nFlags));
	
	return TRUE;
}

CWnd* GetPropertyBrowserControl(UINT nID)
{
	ASSERT_VALID(g_PropSheet);
	
	return g_PropSheet->GetControl(nID);
}

C3dPropertyPage* GetNullPropertyPage()
{
	ASSERT_VALID(g_PropSheet);
	
	return &g_PropSheet->m_nullPage;
}

C3dPropertyPage* GetCurrentPropertyPage()
{
	ASSERT_VALID(g_PropSheet);
	
	return g_PropSheet->GetCurrentPage();
}

void SetPropertyBrowserDefPage()
{
	ASSERT_VALID(g_PropSheet);
	
	g_PropSheet->SetDefaultPage();
}

BOOL IsPropertyBrowserInCancel()
{
	ASSERT_VALID(g_PropSheet);
	
	return g_PropSheet->IsInCancel();
}

BOOL IsPropertyBrowserValidating()
{
	ASSERT_VALID(g_PropSheet);
	
	return g_PropSheet->IsValidating();
}

void PropPageIdle()
{
	if (GetPropertyBrowser() != NULL)
	{
		// PPMOVE : [matthewt]
		// VSHELL currently does not support this
		/*
		if (InEmergencyState())
		{
			// Hide the popups when we're in an emergency state. Note
			// that the user will have to explicitly show these windows
			// after they have been closed due to an emergency.  (This
			// prevents them from flashing on and off when hiding them
			// frees just enough memory to show them again.)

			if (m_pMainwnd->GetLastActivePopup() == pPropSheet)
			{
				CancelPropertyBrowser();
				ClosePropertyBrowser();
			}
		}
		else
			*/
		{
			// If one window is open, and the use minimizes it, we
			// need this final line of defense to null out the properties.
			//
			CWnd* pFrame = GetActiveSlobWnd();
			if (pFrame != NULL)
			{
				pFrame = pFrame->GetParentFrame();
					if (pFrame != NULL &&
						pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) &&
						pFrame->IsIconic())
						theApp.m_theAppSlob.SetSlobProp(P_CurrentSlob, NULL);
			}

			// If the page will be visible, update it.
			BOOL bShowProps = IsPropertyBrowserVisible();
			if (bShowProps)
				UpdatePropertyBrowser();

			ShowPropertyBrowser(bShowProps);
		}
	}
}

//
// Methods used during CSlob::SetupPropertyPages()
//

int AddPropertyPage(C3dPropertyPage* pPage, CSlob* pSlob)
{
	ASSERT( GetPropertyBrowser() != NULL );
	return GetPropertyBrowser()->AddPropertyPage(pPage, pSlob);
}

void SetPropertyCaption(LPCTSTR sz)
{
	ASSERT( GetPropertyBrowser() != NULL );
	GetPropertyBrowser()->SetCaption(sz);
}

BOOL AppendExtraPropertyPages(void)
{
	ASSERT( GetPropertyBrowser() != NULL );
	return GetPropertyBrowser()->AppendExtraPages();
}

BOOL InhibitExtraPropertyPages(BOOL bInhibit)
{
	ASSERT( GetPropertyBrowser() != NULL );
	return GetPropertyBrowser()->InhibitExtraPages(bInhibit);
}

int StartNewPropertyPageSet(void)
{
	ASSERT( GetPropertyBrowser() != NULL );
	return GetPropertyBrowser()->StartNewPageSet();
}

int MergePropertyPageSets(void)
{
	ASSERT( GetPropertyBrowser() != NULL );
	return GetPropertyBrowser()->MergePageSets();
}
