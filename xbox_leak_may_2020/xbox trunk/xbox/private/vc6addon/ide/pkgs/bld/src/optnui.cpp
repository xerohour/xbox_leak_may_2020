//
// COptionPage, COptionPageTab
//									  

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "resource.h"
#include "prjoptn.h"	// our project option table
#include "optnui.h"		// our local class declaration
#include "optndlg.h"	// our CProjOptionsDlg
#include "projcomp.h"	// our spawner
#include "projprop.h"

// our tool options UI
#ifdef _MBCS
#include "mbctype.h"
#endif

#include "util.h"

#define MAX_TEXTLEN 8192		// normal edit controls

IMPLEMENT_DYNAMIC(CToolOptionTab, CDlgTab)
IMPLEMENT_DYNAMIC(COptionMiniPage, CDialog)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// our tool options dialog tab

BEGIN_MESSAGE_MAP(CToolOptionTab, CDlgTab)
	//{{AFX_MSG_MAP(CToolOptionTab)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SET_DEFAULT, OnSetDefault)
	ON_CBN_SELCHANGE(IDC_MINI_PAGE, OnMiniPageSelect)
	ON_EN_UPDATE(IDC_OPTSTR, Update)
	ON_EN_KILLFOCUS(IDC_OPTSTR, Validate_Dest)
	ON_MESSAGE(WM_CTLCOLOREDIT, OnOptStrEditPaint)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CToolOptionTab::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDlgTab::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CToolOptionTab::OnInitDialog()
{
	// call the base-class
	if (!CDlgTab::OnInitDialog())
		return FALSE;

 	// get our listbox
	ValidateMiniPageComboBox();
	ASSERT(m_pcboMiniPage != (CWnd *)NULL);
		
	// init. one-time stuff yet?
	if (!m_pProxySlob)
	{
		// remember our parent's CProxySlob
		m_pProxySlob = &((CProjOptionsDlg *)GetParent())->m_ProxySlob;

		// get the width/height of our tool option tan
		CRect rectTab; GetWindowRect(rectTab);

		// get the rects (relative to tab) of the 'Options:' static text plus
		// assoc. editbox
		CWnd * pWndTxt, * pWndBox; CRect rectTxt, rectBox;
		pWndTxt = GetDlgItem(IDC_OPTIONTXT); ASSERT(pWndTxt != (CWnd *)NULL);
		pWndTxt->GetWindowRect(rectTxt); ScreenToClient(&rectTxt);
		m_cyPageTop = rectTxt.top;
		
		pWndBox = GetDlgItem(IDC_OPTSTR); ASSERT(pWndBox != (CWnd *)NULL);
		pWndBox->GetWindowRect(rectBox); ScreenToClient(&rectBox);
		((CEdit*)pWndTxt)->LimitText(65000);

		// get the first page we looked at
		// and find it's size, make the tool option tab
		// big enough for this page plus the tool option tab
		// controls in the reserved 'top-half' of the tab
		
		// create the first page		
		COptionMiniPage * pPage = CreatePage(0);
		ASSERT(pPage != (COptionMiniPage *)NULL);

		CRect page; pPage->GetWindowRect(page);
		int cyPage = page.Height() - 1;
		SetWindowPos(NULL, 0, 0, max(rectTab.Width(), page.Width()), rectTab.Height() + cyPage, 
					 SWP_NOMOVE | SWP_NOACTIVATE);

		// move the 'Options:' static text plus assoc. editbox down
		pWndTxt->SetWindowPos(pPage, rectTxt.left, rectTxt.top + cyPage, 0, 0, 
					 		  SWP_NOSIZE | SWP_NOACTIVATE);
		pWndBox->SetWindowPos(pWndTxt, rectBox.left, rectBox.top + cyPage, 0, 0, 
					 		  SWP_NOSIZE | SWP_NOACTIVATE);
	}

	// sort entries and fill our listbox
	COptionMiniPage * pPage;
	CString strPageName;
	CObArray rgPages;

	int i, iUpper = m_rgPages.GetUpperBound();
	for (i = 0; i <= iUpper; i++)
	{
		// add the names of our mini-pages into the combo-box
		pPage = (COptionMiniPage *)m_rgPages[i];
		
		// first one is called 'General'
		if (i == 0)
		{
			// always goes first
			strPageName.LoadString(IDS_GENERAL);

			rgPages.Add((CObject *)pPage);	// no elements in array yet, so just add
			m_pcboMiniPage->InsertString(0 /*first*/, strPageName);
		}
		else
		{
			// insert sort
			pPage->GetName(strPageName);

			CString strNewPageName;
			int iInsert = 1;	// we should alread have one entry in list
			for (; iInsert <= rgPages.GetUpperBound(); iInsert++)
			{
				// get this name
				((COptionMiniPage *)rgPages[iInsert])->GetName(strNewPageName);

				// insert before?
				if (strPageName <= strNewPageName)
					break;
			}

			// insert either at end if we are alphabetically > all in current list, or insert
			// before insertion index found
			if (iInsert > rgPages.GetUpperBound())
				rgPages.Add((CObject *)pPage);	// put at end!
			else
				rgPages.InsertAt(iInsert, (CObject *)pPage);	// insert

			m_pcboMiniPage->InsertString(iInsert > rgPages.GetUpperBound() ? -1 : iInsert, strPageName);
		}
	}

	// we'll use this new order
	m_rgPages.RemoveAll();
	for (i = 0; i <= iUpper; i++)
		m_rgPages.Add(rgPages[i]);

	// do we need to hide or show the 'General' listbox?
	GetDlgItem(IDC_PAGE_CATEGORY)->ShowWindow(iUpper > 0 ? SW_SHOW : SW_HIDE);	// static text
	m_pcboMiniPage->ShowWindow(iUpper > 0 ? SW_SHOW : SW_HIDE);	// listbox
	
	return TRUE; // success
}

CBrush brBkGnd;

LRESULT CToolOptionTab::OnOptStrEditPaint(WPARAM wParam, LPARAM lParam)
{
	CWnd * pWndEdit = GetDlgItem(IDC_OPTSTR);
	if (pWndEdit == (CWnd *)NULL || pWndEdit->m_hWnd != (HWND)lParam)
		return (LRESULT)0;	// not processed by us

	// o set the background color to be button face for the description so
	//   that it looks like static text
	// o " ditto " if we have a multiple selection
	// o set the background color to be window background for regular option string
	// we must return a handle to a brush
	BOOL fGrayBkGnd = !m_pProxySlob->IsSingle() || m_pProxySlob->GetContainer() != (CSlob *)NULL;
	COLORREF clrBkGnd = ::GetSysColor(fGrayBkGnd ? COLOR_BTNFACE : COLOR_WINDOW);

	// delete the old brush if we have one
	if (brBkGnd.m_hObject != (HGDIOBJ)NULL)
		brBkGnd.DeleteObject();

	// create the new brush
	if (!brBkGnd.CreateSolidBrush(clrBkGnd))
		return (LRESULT)0;	// couldn't create brush, can't do

	// if we are disabled we'd like to gray our text
	// so we don't look quite so stupid when the whole prop. page
	// is disabled during a build
	(void) ::SetTextColor((HDC)wParam, ::GetSysColor(::IsWindowEnabled((HWND)lParam) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));

	// set our text background color,
	// and return the paint background color
	(void) ::SetBkColor((HDC)wParam, clrBkGnd);

	return (LRESULT)(HBRUSH)brBkGnd.GetSafeHandle();
}

void CToolOptionTab::OnDestroy()
{
	// Should be auto-called in the CToolOptionTab::Deactivate() method by CTabbedDialog
	DeactivatePage(m_nCurrPage);

	// destroy any of the mini pages we created
	for (int i = 0; i <= m_rgPages.GetUpperBound(); i++)
	{
		// destroy the window
		COptionMiniPage * pPage = (COptionMiniPage *)m_rgPages[i];
		if (pPage->m_hWnd != (HWND)NULL)
			pPage->DestroyWindow();
	}
}

BOOL CToolOptionTab::AddOptionPage(COptionMiniPage * pPage)
{
	if (pPage == (COptionMiniPage *)NULL)
		return FALSE;	// no creation

	m_rgPages.Add(pPage);
	return TRUE;
}

void CToolOptionTab::RemoveAllPages()
{
	// remove any of the mini pages we created
	for (int i = 0; i <= m_rgPages.GetUpperBound(); i++)
	{
		// destroy the window
		delete (COptionMiniPage *)m_rgPages[i];
	}
}

BOOL CToolOptionTab::Activate(CTabbedDialog * pParentWnd, CPoint position)
{
	// call our base-class first
	if (!CDlgTab::Activate(pParentWnd, position))
		return FALSE;

	// m_pcboMiniPage should be valid.
	ASSERT( ValidateMiniPageComboBox() );

	if (NULL==m_pcboMiniPage)
		return FALSE;

	// find the category to select if first activation of this tab
	if (m_nCurrPage == -1)
		if ((m_nCurrPage = m_pcboMiniPage->FindStringExact(-1, m_strInitCategory)) == CB_ERR)
			m_nCurrPage = 0;	// if can't find category, default to first

	// select the page for this cateogyr in our listbox of pages
	m_pcboMiniPage->SetCurSel(m_nCurrPage);

	// make sure that while this tab is up 
	// the option parser/generator is using
	// the approp. COptionHandler
	if (m_popthdlr != (COptionHandler *)NULL)
		g_prjoptengine.SetOptionHandler(m_popthdlr);
 
	// make sure the ProxySlob knows about us
	((CProxySlob *)g_prjoptengine.GetPropertyBag())->SetOptionHandler(m_popthdlr);

	// activate our first page
	if (!ActivatePage(m_nCurrPage))
		return FALSE;
		
	// make sure our state is correct
	UpdateState();

	// ok
	return TRUE;
}

void CToolOptionTab::Deactivate(CTabbedDialog* pParentWnd)
{
	// call our base-class first
	CDlgTab::Deactivate(pParentWnd);

	// de-activate our current page
	DeactivatePage(m_nCurrPage);
}

BOOL CToolOptionTab::SetCurrentMiniPage(CString & strTitle)
{
	// have we activated a page yet?
	int iSel;

	// Make sure that m_pcboMiniPage is valid before using it.
	if (m_pcboMiniPage && ValidateMiniPageComboBox())
		if ((iSel = m_pcboMiniPage->FindStringExact(-1, strTitle)) == CB_ERR)
			return FALSE;
	
	if (m_nCurrPage == -1)
		m_strInitCategory = strTitle;	// prime before activation of tab
	else
		SelectPage(iSel); // select new page
	return TRUE;
}

BOOL CToolOptionTab::GetCurrentMiniPage(CString & strTitle)
{
	if (!m_pcboMiniPage  || !ValidateMiniPageComboBox())	return FALSE;

	int iSel;
	if ((iSel = m_pcboMiniPage->GetCurSel()) == CB_ERR)	return FALSE;

	// return the text of the current selection
	m_pcboMiniPage->GetLBText(iSel, strTitle);
	return TRUE;
}

COptionMiniPage * CToolOptionTab::CreatePage(UINT nPage)
{
	COptionMiniPage * pPage = GetOptionPage(nPage);

	// need to create?
	if (pPage->m_hWnd == (HWND)NULL)
	{
		// can we create?
		if (!pPage->Create(this))
			return (COptionMiniPage *)NULL;

		// show the page in the correct position
		// (we use the 'Options:' static text as the ref. point
		// the original 'Options:' static text plus edit-box
		// are moved down)
		CWnd * pwndReset = GetDlgItem(IDC_SET_DEFAULT); ASSERT(pwndReset != (CWnd *)NULL);
		pPage->SetWindowPos(pwndReset, 0, m_cyPageTop, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return pPage;
}

BOOL CToolOptionTab::ActivatePage(UINT nPage)
{
	COptionMiniPage * pPage = CreatePage(nPage);
	if (pPage == (COptionMiniPage *)NULL)	return FALSE;

	// initialise the contents of the page
	pPage->InitPage();

	// show the page visible
	pPage->ShowWindow(SW_SHOWNOACTIVATE);	
	pPage->EnableWindow(TRUE);

	// Help will correspond to the currently active page
 	m_nIDHelp = pPage->GetDlgID();

	return TRUE;
}

void CToolOptionTab::DeactivatePage(UINT nPage)
{
	COptionMiniPage * pPage = GetOptionPage(nPage);

	// hide this page
	pPage->EnableWindow(FALSE);
	pPage->ShowWindow(SW_HIDE);

	// terminate
	pPage->TermPage();
}

void CToolOptionTab::UpdateState()
{
	// init. our font for option text
	//
	CWnd * pWnd;

	pWnd = GetDlgItem(IDC_OPTSTR);
	ASSERT(pWnd != (CWnd *)NULL);
	pWnd->SetFont(GetStdFont(font_Normal));

	// is the text read-only?
	BOOL fReadOnly = TRUE;

	// item label text to display?
	CString strText, strItem;
	
	BOOL fGenDescription = m_pProxySlob->GetContainer() != (CSlob *)NULL;
	strText.LoadString(IDS_OPTIONTXT);
	 
	// are we multiple selection?
	if (m_pProxySlob->IsSingle())
	{
	 	// change the per-item text
		if (m_pProxySlob->IsSortOf(RUNTIME_CLASS(CProject)))
			strItem.LoadString(IDS_PROJECT);
		else if (m_pProxySlob->IsSortOf(RUNTIME_CLASS(CProjGroup)))
			strItem.LoadString(IDS_GROUP);
		else if (m_pProxySlob->IsSortOf(RUNTIME_CLASS(CFileItem)))
			strItem.LoadString(IDS_FILEITEM);
		
		// descriptions are read-only
		fReadOnly = fGenDescription;
	}
	else
	{
		// we don't show per-file for a multiple selection!
		// for what it's worth show the regular text
		// with 'Common' prepended
		strItem.LoadString(IDS_COMMON);
	}

	pWnd = GetDlgItem(IDC_OPTIONTXT);
	ASSERT(pWnd != (CWnd *)NULL);
	// we want to show the right label 'context' + 'view'
	pWnd->SetWindowText(strItem + ' ' + strText);

	// make sure we show the correct option text window .. description or raw
	// the difference is we make sure that the Read-Only attributes are correct
	CEdit * pEdit = (CEdit *)GetDlgItem(IDC_OPTSTR);
	pEdit->SetReadOnly(fReadOnly);

	// refresh our options string to show the content of this new selection
	Refresh();
}

void CToolOptionTab::OnSetDefault()
{
	// give user a chance to bail...
	if (MsgBox(Question,IDS_WARN_RESET_DEFAULT_PROPERTIES, MB_YESNO) == IDNO)
		return;

	// we set to default by copying the prop bag from the base
	// prop bag to the current propbag
	POSITION pos = m_pProxySlob->GetHeadPosition();
	while (pos != (POSITION)NULL)
		m_popthdlr->ResetPropsForConfig(m_pProxySlob->GetNext(pos)->pcr);

	COptionMiniPage * pPage = GetOptionPage(m_nCurrPage); ASSERT(pPage != (COptionMiniPage *)NULL);
	pPage->InitPage();

	// the 'Reset...' button that had the focus should be disabled
	// (we just reset! so give the focus to the next tab item
	CWnd * pwndParent = pPage;
	CWnd * pwnd = pwndParent->GetWindow(GW_CHILD);
	if (pwnd != (CWnd *)NULL)	// got children in page?
	{
		// start search from one before first child
		pwnd = pwndParent->GetNextDlgTabItem(pwnd, TRUE);
	}

	if (pwnd == (CWnd *)NULL)	// got tabstop in page?
	{
		// start search from 'Reset...' button
		pwnd = GetDlgItem(IDC_SET_DEFAULT);
		pwndParent = this;
	}

	ASSERT(pwnd != (CWnd *)NULL);
	CWnd * pwndNext = pwndParent->GetNextDlgTabItem(pwnd);
	if (pwndNext != (CWnd *)NULL)
		pwndNext->SetFocus();

	Refresh();
}

void CToolOptionTab::OnMiniPageSelect()
{
	// Make sure that m_pcboMiniPage is valid.
	ASSERT( ValidateMiniPageComboBox() );
	if (NULL==m_pcboMiniPage) return;

	int iPage = m_pcboMiniPage->GetCurSel();
	ASSERT (iPage != CB_ERR);	// can't have no selection!
	SelectPage(iPage);
}

void CToolOptionTab::SelectPage(UINT nPage)
{
	// no-op if there is no current page change
	if (nPage == m_nCurrPage)
		return;

	COptionMiniPage * pPage = GetOptionPage(m_nCurrPage);

	// activate the new one
	if (!ActivatePage(nPage))
		return;

	// deactivate the old one
	DeactivatePage(m_nCurrPage);

	// this is now our current
	m_nCurrPage = nPage;
}

// our option 'watcher' CSlob for the COptionMiniPage

void COptionWatcher::OnInform(CSlob * pChangedSlob, UINT nIDProp, DWORD dwHint)
{
	// the option pages would like to know about these changes
	// -> they affect the value shown in the dialog controls

	m_pOptionPage->OnPropChange(nIDProp);
	m_pOptionPage->m_pToolOptionTab->Refresh();
}

// mini-page used to show option properties

BEGIN_MESSAGE_MAP(COptionMiniPage, CDialog)
	ON_MESSAGE(WM_USER_VALIDATEREQ, OnValidateReq)
END_MESSAGE_MAP()

BEGIN_IDE_CONTROL_MAP(COptionMiniPage, 0, IDS_GENERAL)
END_IDE_CONTROL_MAP()

COptionMiniPage::COptionMiniPage()
{
	// initialise our own data
	m_pSlob = (CSlob *)NULL;
	m_isValidating = FALSE;
	m_nValidateID = NULL;
	m_bIgnoreChange = FALSE;
	m_bGotSlobHook = FALSE;
	m_popthdlr = (COptionHandler *)NULL;

	// setup our option 'watcher'
	m_optionWatcher.m_pOptionPage = this;
}

COptionMiniPage::~COptionMiniPage()
{
}

BOOL COptionMiniPage::OnInitDialog()
{
	// currently editing this control
 	m_nCurrentEditID = 0;

	// remember our parent tab
	m_pToolOptionTab = (CToolOptionTab *)GetParent();
	
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
	return CDialog::OnInitDialog();
}

BOOL COptionMiniPage::SetupPage(CSlob * pSlob, COptionHandler * popthdlr)
{
	// initialise our data
	m_pSlob = pSlob;
	ASSERT(m_pSlob != (CSlob *)NULL);

	// check-list looks at this CSlob
	m_PropCheckList.SetSlob(m_pSlob);

	// do we have an option handler?
	m_popthdlr = popthdlr;
	if (m_popthdlr == (COptionHandler *)NULL)
		return TRUE;	// done

	CControlMap * pCtlMap = GetControlMap(), * pCtlMapCtls = pCtlMap + 1;

	// convert all of our control map
	// logical to actual properties?
	BOOL fMapCtlMap;
	if (fMapCtlMap = (pCtlMap->m_nExtra == 0))
	{
		// remember the depth of option props in this control map
		if (pCtlMapCtls->m_nCtlType == CControlMap::null)
			pCtlMap->m_nExtra = (UINT)-1;
		else
		{
			// skip over any non-tool options
			while (pCtlMapCtls->m_nProp >= PROJ_BUILD_OPTION_PRIVATE_FIRST)
				pCtlMapCtls++;

			// FUTURE: handle if first control in map doesn't conform
			// to standard 'pCtlMap->m_nProp == property for control'
			ASSERT (pCtlMap->m_nCtlType != CControlMap::icon &&
					pCtlMap->m_nCtlType != CControlMap::checkList);
			pCtlMap->m_nExtra = OHGetDepth(pCtlMapCtls->m_nProp) + 1;
		}
	}

	// adjust option handler used to maybe use a base-one?
	// n.b. this will be needed when a page is returned
	// by a non-root option handler that uses base option props.
	if (pCtlMapCtls->m_nCtlType != CControlMap::null)
		while (m_popthdlr->GetDepth() != (pCtlMap->m_nExtra - 1) /* depth */)
			m_popthdlr = m_popthdlr->GetBaseOptionHandler();

	ASSERT(m_popthdlr != (COptionHandler *)NULL);

	// adjust the tool option props
	// have we already adjusted this static map?
	// (i.e. m_nExtra != 0)
	if (fMapCtlMap)
		while (pCtlMapCtls->m_nCtlType != CControlMap::null)
		{
			// should we bother to map this?
			if (pCtlMapCtls->m_nProp < PROJ_BUILD_OPTION_PRIVATE_FIRST)
			{
				// do something special for checklists
				if (pCtlMapCtls->m_nCtlType == CControlMap::checkList)
				{
					CLocalizedEnum* pEnum = NULL;
					if( (pCtlMapCtls->m_dwFlags & CM_EXTRA_MASK) == CM_EXTRA_ENUM )
						pEnum = (CLocalizedEnum *)pCtlMapCtls->m_nExtra;
					// ONLY for CLocalizedEnums //
					// ASSERT(pEnum->IsKindOf(RUNTIME_CLASS(CLocalizedEnum)));

					// map our list of properties
					CLocalizedEnumerator * pList = pEnum->GetListL();
					while (pList->id != 0)
					{
						pList->val = MapLogical(pList->val);
						pList++;	// next one
					}
				}
				else
					// map our property
					pCtlMapCtls->m_nProp = MapLogical(pCtlMapCtls->m_nProp);
			}
  
			pCtlMapCtls++; // next control
		}

	return TRUE;
}

BOOL COptionMiniPage::Create(CToolOptionTab * pParent)
{
	C3dDialogTemplate dt;
	if (dt.Load(MAKEINTRESOURCE(GetDlgID())))
		SetStdFont(dt);

	return CreateIndirect(dt.GetTemplate(), pParent);
}

BOOL COptionMiniPage::Create(COptsDebugTab * pParent)
{
	C3dDialogTemplate dt;
	if (dt.Load(MAKEINTRESOURCE(GetDlgID())))
		SetStdFont(dt);
	return CreateIndirect(dt.GetTemplate(), pParent);
}

void COptionMiniPage::OnOK()
{
	// Overide for OK processing, but do NOT call EndDialog!
	
	// NB. don't use SendMessage here
	GetParent()->PostMessage(WM_COMMAND, IDOK);
}

void COptionMiniPage::OnCancel()
{
	// Overide for Cancel processing, but do NOT call EndDialog!
	
	// NB. don't use SendMessage here
	GetParent()->PostMessage(WM_COMMAND, IDCANCEL);
}

//
// OnValidateReq
//
// This responds to a "special" message which is posted when an edit
// loses focus, and validation of that edit's contents need to be
// done. It may change the focus back to the edit.
//

afx_msg LONG COptionMiniPage::OnValidateReq(UINT /* w */, LONG /* l */)
{
	// our parent's parent has the 'Cancel', ie. the CTabbedDialog frame
	CWnd * pWndParent = GetParent(); ASSERT(pWndParent != (CWnd *)NULL);
	pWndParent = pWndParent->GetParent(); ASSERT(pWndParent != (CWnd *)NULL);
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

void COptionMiniPage::TermPage()
{
	if (m_bGotSlobHook)
		m_pSlob->RemoveDependant(&m_optionWatcher);
	m_bGotSlobHook = FALSE;
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

BOOL COptionMiniPage::Validate()
{
	BOOL bValid = TRUE;
	CWnd* pCtl;

	// no we have an ID to validate, or is this the option string?
	// in the latter case accept, a derived class should override
	// COptionMiniPage::Validate() in order to do special validation.
	if (m_nValidateID == NULL || m_nValidateID == IDC_OPTSTR)
		return TRUE;

	ASSERT(m_pSlob != NULL);

	CControlMap* pCtlMap = FindControl(m_nValidateID);
	ASSERT(pCtlMap != NULL);

	pCtl = GetDlgItem(pCtlMap->m_nCtlID);
	ASSERT(pCtl != NULL);

	CString strBuf;
	pCtl->GetWindowText(strBuf);

	CString strCurVal;
	if (m_pSlob->GetStrProp(pCtlMap->m_nProp, strCurVal) == valid)
	{
		if (strCurVal == strBuf)
		{
			m_nValidateID = NULL;
			return TRUE;
		}
	}

	ASSERT(pCtlMap->m_nCtlType == CControlMap::comboText ||
		   pCtlMap->m_nCtlType == CControlMap::symbolCombo ||
		   pCtlMap->m_nCtlType == CControlMap::edit ||
		   pCtlMap->m_nCtlType == CControlMap::editInt ||
		   pCtlMap->m_nCtlType == CControlMap::editNum
		  );

	CMultiSlob* pSelection = NULL;
	CSlobWnd* pSlobWnd = m_pSlob->GetSlobWnd();
	if (pSlobWnd != NULL)
		pSelection = (CMultiSlob*)pSlobWnd->GetSelection();
	if (pSelection != NULL && !pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
		pSelection = NULL;

	// FUTURE: no property bag undo (for now?)
	// theUndoSlob.BeginUndo(IDS_UNDO_PROPEDIT);
	bValid = m_pSlob->SetStrProp(pCtlMap->m_nProp, strBuf);
	// theUndoSlob.EndUndo();

	if (bValid)
		m_nValidateID = NULL;
	else
		pCtl->SetFocus();

	return bValid;
}

// Given a control (window) id, return the control map entry for it...
CControlMap* COptionMiniPage::FindControl(UINT nCtlID)
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
BOOL COptionMiniPage::OnCommand(UINT wParam, LONG lParam)
{	
	CWnd * pwndFocus = GetFocus();

	if (HIWORD(wParam) == EN_CHANGE &&			// changing an edit-control?
		m_nCurrentEditID != LOWORD(wParam) &&	// not already noted?
		pwndFocus != (CWnd *)NULL && 			// somebody have focus?
		pwndFocus->GetDlgCtrlID() == LOWORD(wParam)	// only if we have focus!
	   )
		m_nCurrentEditID = LOWORD(wParam);

	else if (HIWORD(wParam) == EN_KILLFOCUS)
		m_nCurrentEditID = 0;					// no longer have focus in edit-control

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
		return CDialog::OnCommand(wParam, lParam);
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
		
		// FUTURE: no undo for property bags (for now?)
		// theUndoSlob.BeginUndo(IDS_UNDO_PROPEDIT);
		if (bString)
			m_pSlob->SetStrProp(pCtlMap->m_nProp, strBuf);
		else
			m_pSlob->SetIntProp(pCtlMap->m_nProp, val);
		// theUndoSlob.EndUndo();
	}
	else if (bValidateReq)
	{
		PostMessage(WM_USER_VALIDATEREQ);
		return CDialog::OnCommand(wParam, lParam);
	}

	return TRUE;
}


// Initialize the values of the controls in the property dialog.
// FUTURE:	The need to do this will be eliminated by simply having the
//			selection announce changes to all properties when the selected
//			object changes!

void COptionMiniPage::InitPage()
{
	// make ourself a dependant of the current CSlob?
	if (!m_bGotSlobHook)
	{
		m_pSlob->AddDependant(&m_optionWatcher);
		m_bGotSlobHook = TRUE;
	}
 
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

		case CControlMap::editStrCap:
			ASSERT(FALSE);	// NOT SUPPORTED
			break;

		case CControlMap::edit:
		case CControlMap::editInt:
		case CControlMap::editNum:
		case CControlMap::autoEdit:
			((CEdit*)GetDlgItem(pCtlMap->m_nCtlID))->LimitText(
				pCtlMap->m_nExtra != 0 ? pCtlMap->m_nExtra : MAX_TEXTLEN);
			// FALL THROUGH

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
CControlMap* COptionMiniPage::FindProp(UINT nProp)
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

// Update the control that reflects a given property in the dialog.
// Called whenever any property of any of the selected objects changes.
BOOL COptionMiniPage::OnPropChange(UINT nProp)
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
			ASSERT( (pCtlMap->m_dwFlags & CM_EXTRA_MASK) == CM_EXTRA_ENUM );
			CEnum* pEnum = (CEnum*)pCtlMap->m_nExtra;

			fFoundCheck	= pEnum->ContainsVal(nProp);
		}

		if (!fFoundCheck) pCtlMap = (CControlMap*)NULL;
	}

	if (pCtlMap == NULL)
	{
		if (nProp == P_QuoteName)
			pCtlMap = FindProp(P_ID);

		if (pCtlMap == NULL)
			return FALSE;
	}

	CWnd* pWndCtl = GetDlgItem(pCtlMap->m_nCtlID);

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
			if (gpt == ambiguous)
				val = 2;	// indeterminate state
			else if (gpt == invalid)
				val = 0;	// disabled state

			((CButton*)pWndCtl)->SetCheck(val);
			pWndCtl->EnableWindow(gpt != invalid);
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
					((CEdit*)pWndCtl)->LimitText(pCtlMap->m_nExtra != 0 ? pCtlMap->m_nExtra : MAX_TEXTLEN);

				m_bIgnoreChange = FALSE;
			}
			else
			{
				bChanged = FALSE;
			}

			GetDlgItem(pCtlMap->m_nCtlID)->EnableWindow(gpt != invalid);
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

			GetDlgItem(pCtlMap->m_nCtlID)->EnableWindow(gpt != invalid);
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
				GetDlgItem(id)->EnableWindow(gpt != invalid);

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

			GetDlgItem(pCtlMap->m_nCtlID)->EnableWindow(gpt != invalid);
			break;
		}
	}

	return bChanged;
}

UINT COptionMiniPage::GetNameID()
{
	return GetControlMap()->m_nLastCtlID;
}

void COptionMiniPage::GetName(CString & strName)
{
	if (!strName.LoadString(GetControlMap()->m_nLastCtlID))
		strName = "<No Name>";
}

//
// validation for the CToolOptionTab
//

BOOL Validate_D_Helper(CString & strDest, UINT idMsg, const TCHAR * pchExt, BOOL fMustBeDir, BOOL fCantBeDir)
{
 	// nothing to do?
	if (strDest.IsEmpty())	return TRUE;

	enum {fail, pass, neither} result = neither;
	CString strDestName;
	VERIFY(strDestName.LoadString(idMsg));

	TCHAR * pchStart = (TCHAR *)(const TCHAR *)strDest;
	TCHAR * pchT;

	pchT = pchStart + _tcslen(pchStart);	// point to nul terminator
	pchT = _tcsdec(pchStart, pchT);			// back up one char, DBCS safe

	BOOL fIsDir = (*pchT == _T('\\') || *pchT == _T('/'));

	// is this definately a directory ?
	if (fIsDir)
	{
		// yes
		if (fCantBeDir)
		{
			// can't be a directory!
			InformationBox(IDS_DEST_CANNOT_BE_DIR, (LPCSTR)strDestName);
			result = fail;
		}
		else
		{
			result = pass;
		}
	}
	else
	{
		CPath pathDest;
		if (pathDest.Create(strDest))
		{
			// no
			if (fMustBeDir)
			{
				// assume this a directory
				{
					// correct -> form a directory, append a '/'
					strDest += _T('/');
					result = pass;
				}
			}

			if (result == neither)			// still not determined?			
			{
				if ((*pchExt != _T('\0')) &&	// need to match extension?
					!FileNameMatchesExtension(&pathDest, pchExt) // match extension?
			   	   )
				{
					const TCHAR * pchDestExt = pathDest.GetExtension();
					if (!fCantBeDir && (*pchDestExt == _T('\0')))
					{
						// is this a directory?
						int iReply = QuestionBox(IDS_DEST_IS_DIR, MB_YESNO, strDestName);
						if (iReply == IDYES)
						{
							// correct -> form a directory, append a '/'
							strDest += _T('/');
							result = pass;
						}
					}

					// still not determined
					if (result == neither)
					{
						// must match extension if not a directory
						InformationBox(IDS_DEST_BAD_EXTENSION, (LPCSTR)strDestName, (LPCSTR)pchExt);

						// correct -> change the extension to the correct one

						// extent of path to keep?
						int cch = strDest.GetLength() + _tcslen(pchExt) - _tcslen(pchDestExt) + 1;
						pathDest.ChangeExtension(pchExt);

						strDest = (const TCHAR *)pathDest;
						strDest = strDest.Right(cch);

						result = pass;
					}
				}
				else
					result = pass;
			}
		}
		else
		{
			// bad path
			InformationBox(IDS_DEST_BAD_PATH, (LPCSTR)strDestName, (LPCSTR)strDest);
			result = fail;
		}
	}

	ASSERT(result != neither);

	return (result == pass);
}

BOOL Validate_DestinationP(CSlob * pSlob, UINT idProp, UINT idOption,
						   CDialog * pDlg, UINT idPropDest, UINT idMsg, BOOL * pfRefresh,
						   const TCHAR * pchExt, BOOL fMustBeDir, BOOL fCantBeDir)
{
	CString strDest;
	// an invalid destination can't be validated, and are valid by default
	if (pSlob->GetStrProp(idProp, strDest) != valid)	return TRUE;

	// remember our destination, it might be changed in order
	// to make it valid, eg. appending a '/' to make it a directory
	CString strOld = strDest;
	BOOL fReturn = Validate_D_Helper(strDest, idMsg, pchExt, fMustBeDir, fCantBeDir);

	// did we pass?
	if (fReturn)
	{
		// update (possibly) changed prop
		if (strDest != strOld)
			pSlob->SetStrProp(idProp, strDest);
	}
	else
	{
		// get the single option 
		CString strSglOption;

		g_prjoptengine.GenerateSingleOption(idOption, strSglOption, OBShowFake);

		// get the option string
		CString strOption;
		CWnd * pWnd = pDlg->GetDlgItem(idPropDest);
		ASSERT(pWnd != (CWnd *)NULL);
		pWnd->GetWindowText(strOption);

		// try to find the single option in the options string
		// if we find it, then try to figure the start and end
		// of the destination
		int ich, ichStart = 0, ichEnd = -1;
		ich = strOption.Find(strSglOption);
		if (ich != -1)
		{
			ich += strSglOption.Find(strDest);
			ichStart = ich; ichEnd = ichStart + strDest.GetLength();
			
			// if we finish on a '\' and the next chars. are
			// '\"' then incr. one char.
			if (ichEnd &&	// make sure that 'ichStart != ichEnd != 0'
				strOption[ichEnd-1] == _T('\\') &&
				strOption[ichEnd] == _T('\\') &&
				strOption[ichEnd+1] == _T('"')
			   )
				ichEnd++;
		}
		else
		{
			// we should find this option!
			ASSERT(FALSE);
		}

		// keep focus + select the prop string in the text
		pWnd->SetFocus();
		((CEdit *)pWnd)->SetSel(ichStart, ichEnd);
	} 

	return fReturn;
}

BOOL Validate_Destination(CDialog * pDlg, UINT idEditDest, UINT idMsg,
						  const TCHAR * pchExt, BOOL fMustBeDir, BOOL fCantBeDir)
{
	CWnd * pWnd = pDlg->GetDlgItem(idEditDest);
	ASSERT(pWnd != (CWnd *)NULL);
	CString strDest;
	pWnd->GetWindowText(strDest);

	BOOL fReturn = Validate_D_Helper(strDest, idMsg, pchExt, fMustBeDir, fCantBeDir);

	// Get pWnd again since it may have been deleted during OnIdle processing 
	// if Validate_D_Helper displayed a message.  (SPEED:4808)
	pWnd = pDlg->GetDlgItem(idEditDest);

	// did we pass?
	if (fReturn)
	{
		// update a (possibly) changed text
		pWnd->SetWindowText(strDest);
	}
	else
	{
		// keep focus + full-selection
		pWnd->SetFocus();
		((CEdit *)pWnd)->SetSel(0, -1);
	}

	return fReturn;
}

BOOL CToolOptionTab::ValidateTab()
{
	BOOL fValid;	// valid?

	// validate current COptionMiniPage
	COptionMiniPage * pPage = GetOptionPage(m_nCurrPage);

	if (!pPage->m_isValidating)
	{
		pPage->m_isValidating = TRUE;
		fValid = pPage->Validate();	// validate page
		pPage->m_isValidating = FALSE;	
	}
	else
		fValid = FALSE;	// can't do during validation

 	// reset 'to-do' validation if the we are valid
	if (fValid)
		pPage->m_nCurrentEditID = 0;

	return fValid;
}

void CToolOptionTab::Validate_Dest()
{
	// ignore if we have a description
	if (m_pProxySlob->GetContainer() != (CSlob  *)NULL)
		return;

	// we might have caused this due to a SetFocus in the
	// validation failure of other edits
	COptionMiniPage * pPage = (COptionMiniPage *)GetOptionPage(m_nCurrPage);
	if (!pPage->m_isValidating)
	{
		// force validation of the options string
		pPage->m_nValidateID = IDC_OPTSTR;
		Refresh();
		pPage->PostMessage(WM_USER_VALIDATEREQ);
	}
}

BOOL COptionMiniPage::Validate_DestinationProp
(
	UINT idProp,
	UINT idOption, 
	UINT idEditDest,
	UINT idMsg,
	const TCHAR * pchExt,
	BOOL fMustBeDir,
	BOOL fCantBeDir
)
{
	// we need to map the actual prop to a logical one
	// (do some cunning arithmetic so we know how deep to go)
	COptionHandler * popthdlr = m_popthdlr;
	UINT cStep = popthdlr->GetDepth() - OHGetDepth(idProp);  
	while (cStep--)
		popthdlr = popthdlr->GetBaseOptionHandler();

	UINT idPropL = popthdlr->MapLogical(idProp);
 
	BOOL fReturn = ::Validate_DestinationP(m_pSlob, idPropL, idOption,
										   (idEditDest == IDC_OPTSTR) ? (CDialog *)GetParent() : this,
										   idEditDest, idMsg, &(m_pToolOptionTab->m_fRefresh),
								  		   pchExt, fMustBeDir, fCantBeDir);

	// we're not using the standard page validation so do our
	// own setting of state vars here...:-(
	// clear the validation ID check!
	if (fReturn && (idEditDest == IDC_OPTSTR))
		m_nValidateID = NULL;
	return fReturn;
}

BOOL COptionMiniPage::Validate_Destination
(
	UINT idEditDest,
	UINT idMsg, 
	const TCHAR * pchExt,
	BOOL fMustBeDir,
	BOOL fCantBeDir
)
{
	BOOL fReturn = ::Validate_Destination(this, idEditDest, idMsg, pchExt, fMustBeDir, fCantBeDir);

	// we're not using the standard page validation so do our
	// own setting of state vars here...:-(
	// clear the validation ID check!
	if (fReturn && (idEditDest == IDC_OPTSTR))
		m_nValidateID = NULL;
	return fReturn;
}

void CToolOptionTab::Refresh()
{
	if (!m_fRefresh)	return;	// can't do this now
	
	CString strText; strText = "";

	CWnd * pWnd = GetDlgItem(IDC_OPTSTR);
	ASSERT(pWnd != (CWnd *)NULL);

	BOOL fEnableSetDefault;

	// are we generating an option string or a descriptive text?
	if (m_pProxySlob->GetContainer() == (CSlob *)NULL)
	{
		g_prjoptengine.GenerateString(strText, OBShowFake);

		// preserve the selection if we have the focus and we have full-select
		int nStartChar, nEndChar;
		((CEdit *)pWnd)->GetSel(nStartChar, nEndChar);
		CWnd * pwndFocus = GetFocus();
		BOOL fPreserveSel = (pwndFocus != (CWnd *)NULL) &&					// has focus
							(pWnd->m_hWnd == pwndFocus->m_hWnd) &&	// edit has focus
							(nStartChar == 0) &&			// full-select
							(nEndChar == pWnd->GetWindowTextLength());

		m_fRefresh = FALSE;
		pWnd->SetWindowText(strText);
		m_fRefresh = TRUE;

		// full-select, leave caret at end->start
		if (fPreserveSel)
			((CEdit *)pWnd)->SetSel(0, -1);
	}
	else
	{
		// are we multi-select?
		if (m_pProxySlob->IsSingle())
		{
			BOOL fDummy;
			g_prjoptengine.GenerateDescription(strText, fDummy);
		}
		else
			strText.LoadString(IDS_PEROPT_NA);
										 
		pWnd->SetWindowText(strText);
	}

	fEnableSetDefault = FALSE;	// by default, no
	POSITION pos = m_pProxySlob->GetHeadPosition();
	while (!fEnableSetDefault && (pos != (POSITION)NULL))
		fEnableSetDefault = m_popthdlr->CanResetPropsForConfig(m_pProxySlob->GetNext(pos)->pcr);

	// do we have a 'Set Defaults' button, if so enable/disable approp.
	if ((pWnd = GetDlgItem(IDC_SET_DEFAULT)) != (CWnd *)NULL)
		EnableButton(pWnd,fEnableSetDefault);
}

void CToolOptionTab::Update()
{
	// do we need to ignore this?
	if (!m_fRefresh)	return;

	m_fRefresh = FALSE;

	CString	strOption;

	ConfigurationRecord* pPcrBase = const_cast<ConfigurationRecord*>(m_pProxySlob->GetBaseConfig());
	CProject* pProject = m_pProxySlob->GetProject();

	ASSERT(NULL!=pProject);

	CProjTempConfigChange projTempConfigChange(pProject);

	if (NULL!=pPcrBase)
		projTempConfigChange.ChangeConfig(pPcrBase);

	// get our options string from the edit-item and parse it
	GetDlgItem(IDC_OPTSTR)->GetWindowText(strOption);
	g_prjoptengine.ParseString(strOption);

	m_fRefresh = TRUE;
}

BOOL COptionMiniPage::PreTranslateMessage(MSG* pMsg)
{
	// all tab events and accelerators are handled by the CTabbedDialog
	if (((CTabbedDialog*)m_pToolOptionTab->GetParent())->MessageStolenByTabbedDialog(pMsg))
		return TRUE;

	// call our base class
	return CDialog::PreTranslateMessage(pMsg);
}
