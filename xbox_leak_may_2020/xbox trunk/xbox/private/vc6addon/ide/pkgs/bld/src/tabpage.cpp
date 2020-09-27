// tabpage.cpp : implementation file
//

#include "stdafx.h"

#include "util.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MAX_TEXTSIZE 8192 // normal edit controls

//
// CStringListGridWnd_WithChange is a CStringListGridWnd that sends GLN_CHANGE when
// a change has occurred.
//
void CStringListGridWnd_WithChange::Send_GLN_CHANGE()
{
	CWnd* pParent = GetParent();
	ASSERT_VALID(pParent);
	
	pParent->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), GLN_CHANGE), (LPARAM)m_hWnd);
}


BOOL CStringListGridWnd_WithChange::OnChange(int nIndex)
{
	if (CStringListGridWnd::OnChange(nIndex))
	{
		Send_GLN_CHANGE();
		return TRUE;
	}
	return FALSE;
}


BOOL CStringListGridWnd_WithChange::OnAddString(int nIndex)
{
	if (CStringListGridWnd::OnAddString(nIndex))
	{
		Send_GLN_CHANGE();
		return TRUE;
	}
	return FALSE;
}


BOOL CStringListGridWnd_WithChange::OnDeleteString(int nIndex)
{
	if (CStringListGridWnd::OnDeleteString(nIndex))
	{
		Send_GLN_CHANGE();
		return TRUE;
	}
	return FALSE;
}


BOOL CStringListGridWnd_WithChange::OnMove(int nSrcIndex, int nDestIndex)
{
	if (CStringListGridWnd::OnMove(nSrcIndex, nDestIndex))
	{
		Send_GLN_CHANGE();
		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
// CSlobPageWatch -- our 'watcher' CSlob for the CSlobPageTab
void CSlobPageWatch::OnInform(CSlob * pChangedSlbo, UINT nIDProp, DWORD dwHint)
{
	m_pSlobPageTab->OnPropChange(nIDProp);
}

////////////////////////////////////////////////////////////////////////////
// CSlobPageTab -- one page showing properties of a tabbed dialog

IMPLEMENT_DYNAMIC(CSlobPageTab, CDlgTab)

BEGIN_MESSAGE_MAP(CSlobPageTab, CDlgTab)
	ON_MESSAGE(WM_USER_VALIDATEREQ, OnValidateReq)
END_MESSAGE_MAP()

BEGIN_IDE_CONTROL_MAP(CSlobPageTab, 0, IDS_GENERAL)
END_IDE_CONTROL_MAP()

CSlobPageTab::CSlobPageTab() : CDlgTab()
{
	// initialise our own data
	m_pSlob = (CSlob *)NULL;
	m_isValidating = FALSE;
	m_nValidateID = NULL;
	m_bIgnoreChange = FALSE;

	// setup our watcher
	m_slobWatch.m_pSlobPageTab = this;
}

CSlobPageTab::~CSlobPageTab()
{
}

BOOL CSlobPageTab::OnInitDialog()
{
	// clear our cache of CHECK_LISTs
	m_listCheckList.RemoveAll();

	// sub-class any CHECK_LISTs we see in the dialog
	for (CControlMap * pCtlMap = GetControlMap() + 1;
		pCtlMap->m_nCtlType != CControlMap::null; pCtlMap += 1)
	{
		if (pCtlMap->m_nCtlType == CControlMap::checkList)
		{
			m_PropCheckList.SubclassDlgItem(pCtlMap->m_nCtlID, this);

			// put this in our cache
			m_listCheckList.AddHead((void *)pCtlMap);
		}
	}

	// call our base-class
	return CDlgTab::OnInitDialog();
}

BOOL CSlobPageTab::SetupPage(CSlob * pSlob)
{
	// initialise our CDlgTab data
	GetName(m_strCaption);
	// initialised ok?
	if ((m_lpszTemplateName = MAKEINTRESOURCE(GetDlgID())) == (char *)NULL)
		return FALSE;
 
 	// Default help ID is based on the template ID, of course.
 	m_nIDHelp = LOWORD(GetDlgID());

	// initialise our own data
	m_pSlob = pSlob;
	ASSERT(m_pSlob != (CSlob *)NULL);

	// check-list looks at this CSlob
	m_PropCheckList.SetSlob(m_pSlob);

	// need to init. page
	m_bInit = FALSE;

	return TRUE;
}

BOOL CSlobPageTab::Activate(CTabbedDialog * pParentWnd, CPoint position)
{
	// call our base-class first
	CDlgTab::Activate(pParentWnd, position);

	m_pTabbedDialog = pParentWnd;

	// do we need to init. page?
	if (!m_bInit)
	{
		// make ourself a dependant of the current CSlob
		m_pSlob->AddDependant(&m_slobWatch);
 
		// fill the controls
		InitPage();

		// initialized
		m_bInit = TRUE;
	}

	// ok
	return TRUE;
}

void CSlobPageTab::CommitTab()
{
	// called only when entire dialog dismissed by OK/Close button
}

BOOL CSlobPageTab::ValidateTab()
{
	if (m_isValidating)
		return FALSE;	// can't do during validation!

	// validate this page, if we can't don't deactivate
	m_isValidating = TRUE;
	BOOL fResult = Validate();
	m_isValidating = FALSE;
	if (!fResult)
		return FALSE;	// can't do!

	// remove our dependancy
	ASSERT(m_pSlob != (CSlob *)NULL);
	m_pSlob->RemoveDependant(&m_slobWatch);

	// terminate this page
	TermPage();

	// allow deactivation
	return CDlgTab::ValidateTab();
}

void CSlobPageTab::CancelTab()
{
	// remove our dependancy
	ASSERT(m_pSlob != (CSlob *)NULL);
	m_pSlob->RemoveDependant(&m_slobWatch);

	// terminate this page
	TermPage();

	CDlgTab::CancelTab();
}

//
// OnValidateReq
//
// This responds to a "special" message which is posted when an edit
// loses focus, and validation of that edit's contents need to be
// done. It may change the focus back to the edit.
//

afx_msg LONG CSlobPageTab::OnValidateReq(UINT /* w */, LONG /* l */)
{
	// our parent has the 'Cancel' button, ie. the CTabbedDialog frame
	CWnd * pWndParent = GetParent(); ASSERT(pWndParent != (CWnd *)NULL);
	CWnd * pWnd = pWndParent->GetDlgItem(IDCANCEL);
	CWnd * pWndFocus = GetFocus();

	if (pWnd != (CWnd *)NULL && pWndFocus != (CWnd *)NULL &&
		pWndFocus->m_hWnd == pWnd->m_hWnd
	   )
		return TRUE;	// ignore, we're in a cancel

	BOOL fResult = TRUE;

	if (!m_isValidating)
	{
		m_isValidating = TRUE;
		fResult = Validate();
		m_isValidating = FALSE;
	}

	return TRUE;
}

void CSlobPageTab::TermPage()
{
	m_bInit = FALSE;
}

//
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
//

BOOL CSlobPageTab::Validate()
{
	BOOL bValid = TRUE;
	CWnd* pCtl;

	if (m_nValidateID == NULL)
		return TRUE;

	ASSERT(m_pSlob != NULL);

	CControlMap* pCtlMap = FindControl(m_nValidateID);
	ASSERT(pCtlMap != NULL);

	if( pCtlMap->m_nCtlType == CControlMap::autoEdit )
		return TRUE;	// has already been validated automatically

	pCtl = GetDlgItem(pCtlMap->m_nCtlID);
	ASSERT(pCtl != NULL);

	CString strBuf;
	pCtl->GetWindowText(strBuf);
	CString strCurVal;
	if (m_pSlob->GetStrProp(pCtlMap->m_nProp, strCurVal) == valid)
	{
		// special case the string page caption control because
		// what it displays in the edit is not always what is stored
		// in the property
		if (pCtlMap->m_nCtlType == CControlMap::editStrCap)
		{
			StringEditorEditToProp(strBuf.GetBuffer(1025));
			strBuf.ReleaseBuffer();
			// if there are backslashes in the string we need to go
			// ahead and update
			if (strCurVal == strBuf && strCurVal.Find("\\") == -1)
			{
				m_nValidateID = NULL;
				return TRUE;
			}
		}
		else if (strCurVal == strBuf)
		{
			m_nValidateID = NULL;
			return TRUE;
		}
	}

	switch (pCtlMap->m_nCtlType)
	{
#ifdef _DEBUG
	default:
		ASSERT(FALSE);
#endif

	case CControlMap::editStrCap:
	case CControlMap::comboText:
	case CControlMap::symbolCombo:
	case CControlMap::edit:
	case CControlMap::editInt:
	case CControlMap::editNum:
		break;
	}

	CMultiSlob* pSelection = NULL;
	CSlobWnd* pSlobWnd = m_pSlob->GetSlobWnd();
	if (pSlobWnd != NULL)
		pSelection = (CMultiSlob*)pSlobWnd->GetSelection();
	if (pSelection != NULL && !pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
		pSelection = NULL;

	bValid = m_pSlob->SetStrProp(pCtlMap->m_nProp, strBuf);

	OnPropChange(pCtlMap->m_nProp);

	if (bValid)
		m_nValidateID = NULL;
	else
		pCtl->SetFocus();

	return bValid;
}

// Given a control (window) id, return the control map entry for it...
CControlMap* CSlobPageTab::FindControl(UINT nCtlID)
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

// Handle a control notification from a property dialog.
BOOL CSlobPageTab::OnCommand(UINT wParam, LONG lParam)
{
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
		return CDlgTab::OnCommand(wParam, lParam);
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
	{
		if (nNotify != BN_CLICKED)
			goto LDefault;

		val = (int)::SendMessage(hWndCtl, BM_GETCHECK, 0, 0);
		if (val == 2)
			val = FALSE;
		else
			val = !val;
		bSetProp = TRUE;
		break;
	}

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

	case CControlMap::comboList:
	{
		if (nNotify != CBN_SELCHANGE)
			goto LDefault;
		int isel = (int)::SendMessage(hWndCtl, CB_GETCURSEL, 0, 0);
		val = (int)::SendMessage(hWndCtl, CB_GETITEMDATA, isel, 0);
		bSetProp = TRUE;
		break;
	}

	case CControlMap::comboText:
	case CControlMap::symbolCombo:
		if ((nNotify == CBN_EDITCHANGE || nNotify == CBN_SELCHANGE) && !m_bIgnoreChange)
		{
			m_nValidateID = pCtlMap->m_nCtlID;
		}
		else if (nNotify == CBN_SELENDCANCEL || nNotify == CBN_SELENDOK)
		{
			m_nValidateID = pCtlMap->m_nCtlID;
			bValidateReq = TRUE;
		}
		break;

	case CControlMap::autoEdit:
		if (nNotify != EN_CHANGE || m_bIgnoreChange)
			goto LDefault;

		bSetProp = TRUE;
		CEdit::FromHandle(hWndCtl)->GetWindowText(strBuf);
		bString = TRUE;
		m_nValidateID = pCtlMap->m_nCtlID;
		break;

	case CControlMap::editStrCap:
		if (nNotify == EN_CHANGE && !m_bIgnoreChange)
			m_nValidateID = pCtlMap->m_nCtlID;
		else if (nNotify == EN_KILLFOCUS)
			bValidateReq = TRUE;
        break;

	case CControlMap::editInt:
	case CControlMap::edit:
	case CControlMap::editNum:
		if (nNotify == EN_CHANGE && !m_bIgnoreChange)
			m_nValidateID = pCtlMap->m_nCtlID;
		else if (nNotify == EN_KILLFOCUS)
			bValidateReq = TRUE;
		break;

	case CControlMap::radio:
		if (nNotify != BN_CLICKED)
			goto LDefault;
		val = LOWORD(wParam) - pCtlMap->m_nCtlID + pCtlMap->m_nExtra;
		bSetProp = TRUE;
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

		if (bString)
			m_pSlob->SetStrProp(pCtlMap->m_nProp, strBuf);
		else
			m_pSlob->SetIntProp(pCtlMap->m_nProp, val);

		OnPropChange(pCtlMap->m_nProp);
	}
	else if (bValidateReq)
	{
		PostMessage(WM_USER_VALIDATEREQ);
		return CDlgTab::OnCommand(wParam, lParam);
	}

	return TRUE;
}


// Initialize the values of the controls in the property dialog.
// FUTURE:	The need to do this will be eliminated by simply having the
//			selection announce changes to all properties when the selected
//			object changes!

void CSlobPageTab::InitPage()
{
	CControlMap* pCtlMap;

	// If several slob types share the same page we may need to change
	// the help ID, so the context sensitive help can reflect the
	// correct slob type.
	//
	UINT nHelpID = GetControlMap()->m_nProp;
	if ( nHelpID )
		SetHelpID( nHelpID );

	for (pCtlMap = GetControlMap() + 1;
		pCtlMap->m_nCtlType != CControlMap::null; pCtlMap += 1)
	{
		CEnum* pEnum = NULL;
		if( (pCtlMap->m_dwFlags & CM_EXTRA_MASK) == CM_EXTRA_ENUM )
			pEnum = (CEnum*)pCtlMap->m_nExtra;

		switch (pCtlMap->m_nCtlType)
		{
		case CControlMap::checkList:
			if (pEnum != NULL)
			{
				pEnum->FillCheckList((CPropCheckList*)GetDlgItem(pCtlMap->m_nCtlID),
					TRUE, m_pSlob);
			}
			goto LThinFont;
			break;

		case CControlMap::comboList:
		case CControlMap::comboText:
			if (pEnum != NULL)
			{
				pEnum->FillComboBox((CComboBox*)GetDlgItem(pCtlMap->m_nCtlID),
					TRUE, m_pSlob);
			}
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
			((CEdit*)GetDlgItem(pCtlMap->m_nCtlID))->LimitText(
				pCtlMap->m_nExtra != 0 ? pCtlMap->m_nExtra : MAX_TEXTSIZE);
			// FALL THROUGH

		case CControlMap::editStrCap:
		case CControlMap::thinText:
		case CControlMap::pathText:
LThinFont:
			GetDlgItem(pCtlMap->m_nCtlID)->SetFont(GetStdFont(font_Normal));
			break;
		}

		OnPropChange(pCtlMap->m_nProp);
	}
}

// Given a property id, return the control map entry for it.
CControlMap* CSlobPageTab::FindProp(UINT nProp)
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

extern void EnableControl(CWnd* pWnd, GPT gpt);		// implemented in proppage.cpp

// Update the control that reflects a given property in the dialog.
// Called whenever any property of any of the selected objects changes.
BOOL CSlobPageTab::OnPropChange(UINT nProp)
{
	// have all the properties changed?
	if (nProp == SN_ALL)
	{
		// require a full update
		InitPage();
		return TRUE;
	}

	BOOL bChanged = TRUE;

	CControlMap* pCtlMap = FindProp(nProp);

	if (pCtlMap == NULL)
	{
		// not found in the control map, is this a Check-List?
		// if so look for the prop in each Check-List enum

		BOOL fFoundCheck = FALSE;
		POSITION pos = m_listCheckList.GetHeadPosition();
		while ((pos != (POSITION)NULL) && !fFoundCheck)
		{
			pCtlMap = (CControlMap*)m_listCheckList.GetNext(pos);
			CEnum* pEnum = (CEnum*)pCtlMap->m_nExtra;
			ASSERT((pCtlMap->m_dwFlags & CM_EXTRA_MASK) == CM_EXTRA_ENUM);
			
			fFoundCheck = pEnum->ContainsVal(nProp);
		}

		if (!fFoundCheck) pCtlMap = (CControlMap*)NULL;
	}

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

	int val;
	CString str;
	UINT id;

	switch (pCtlMap->m_nCtlType)
	{
		//
		// checkboxes
		//
		case CControlMap::check:
		{
			GPT gpt;
			gpt = m_pSlob->GetIntProp(pCtlMap->m_nProp, val);
			if (gpt == ambiguous || gpt == invalid)
				val = 2;	// indeterminate state

			((CButton*)pWndCtl)->SetCheck(val);
			EnableControl(pWndCtl, gpt);
			break;
		}

	   	// 
		// check-lists
		//
		case CControlMap::checkList:
			// let the CPropCheckList handle everything
			((CPropCheckList*)pWndCtl)->OnPropChanged(nProp);
			break;

		// 
		// edit-box cap
		//
		case CControlMap::editStrCap:
		{
			GPT gpt = m_pSlob->GetStrProp(pCtlMap->m_nProp, str);

			CString strWindow;
			pWndCtl->GetWindowText(strWindow);

            //
            // going from property into edit window
            //
			if (gpt == invalid || gpt == ambiguous)
				str = "";	// indeterminate or invalid state
			else
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

			break;
		}

		//
		// edit-boxes
		//
		case CControlMap::editInt:
		case CControlMap::edit:
		case CControlMap::editNum:
		case CControlMap::autoEdit:
		case CControlMap::thinText:
		case CControlMap::thickText:
		case CControlMap::pathText:
		{
			GPT gpt = m_pSlob->GetStrProp(pCtlMap->m_nProp, str);

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
				if (path.Create(str))
				{
					path.GetDisplayNameString(strPath, MaxWidth);
					str = strPath;
				}
			}

			CString strWindow;
			pWndCtl->GetWindowText(strWindow);

			if (gpt == invalid || gpt == ambiguous)
				str = "";	// invalid or indeterminate

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
					((CEdit*)pWndCtl)->LimitText(pCtlMap->m_nExtra != 0 ?
						pCtlMap->m_nExtra : MAX_TEXTSIZE);
				}

				m_bIgnoreChange = FALSE;
			}
			else
			{
				bChanged = FALSE;
			}
			EnableControl(pWndCtl, gpt);
			EnableControl(pPromptWnd, gpt);

			break;
		}

		//
		// listboxes (selected by string prop)
		//
		case CControlMap::listText:
		case CControlMap::comboText:
		// PPMOVE SPECIAL: [matthewt]
		// Does this resource handling need to be moved back into VRES?
		case CControlMap::symbolCombo:
		{
			GPT gpt = m_pSlob->GetStrProp(pCtlMap->m_nProp, str);

			CString strWindow;
			pWndCtl->GetWindowText(strWindow);

			if (gpt == invalid || gpt == ambiguous)
				str = "";	// invalid or indeterminate

			if (str != strWindow)
			{
				int iCurSel;
				m_bIgnoreChange = TRUE;
				if (pCtlMap->m_nCtlType == CControlMap::listText)
				{
					iCurSel = str == "" ? LB_ERR : ((CListBox*)pWndCtl)->FindStringExact(-1, str);
					((CListBox*)pWndCtl)->SetCurSel(iCurSel);
				}
				else
				{
					iCurSel = str == "" ? CB_ERR : ((CComboBox*)pWndCtl)->FindStringExact(-1, str);
					((CComboBox*)pWndCtl)->SetCurSel(iCurSel);
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

			break;
		}

		//
		// radio-buttons
		//
		case CControlMap::radio:
		{
			GPT gpt = m_pSlob->GetIntProp(pCtlMap->m_nProp, val);

			if (gpt == invalid || gpt == ambiguous)
				val = -1;	// invalid or indeterminate

			CheckRadioButton(pCtlMap->m_nCtlID, pCtlMap->m_nLastCtlID,
				val == -1 ? val : pCtlMap->m_nCtlID + val - pCtlMap->m_nExtra);

			for (id = pCtlMap->m_nCtlID; id <= pCtlMap->m_nLastCtlID; id += 1)
			{
				EnableControl(GetDlgItem(id), gpt);
			}

			break;
		}

		// 
		// listboxes (selected by index)
		//
		case CControlMap::list:
		{
			GPT gpt = m_pSlob->GetIntProp(pCtlMap->m_nProp, val);

			if (gpt == invalid || gpt == ambiguous)
				val = -1;	// invalid or indeterminate

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
			}

			((CListBox*)pWndCtl)->SetCurSel(val);

			GetDlgItem(pCtlMap->m_nCtlID)->EnableWindow(gpt != invalid);
			if ((pPromptWnd = GetDlgItem(pCtlMap->m_nCtlID - 1)) != NULL)
				pPromptWnd->Invalidate(FALSE);

			break;
		}

		//
		// combo listboxes (selected by index)
		//
		case CControlMap::comboList:
		{
			GPT gpt = m_pSlob->GetIntProp(pCtlMap->m_nProp, val);

			if (gpt == invalid || gpt == ambiguous)
				val = -1;	// invalid or indeterminate

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
			}

			((CComboBox*)pWndCtl)->SetCurSel(val);

			EnableControl(pWndCtl, gpt);
			EnableControl(pPromptWnd, gpt);

			break;
		}
	}

	return bChanged;
}

void CSlobPageTab::GetName(CString & strName)
{
	if (strName.IsEmpty() && !strName.LoadString(GetControlMap()->m_nLastCtlID))
		VERIFY( strName.LoadString( IDS_UNNAMED ) );
}
