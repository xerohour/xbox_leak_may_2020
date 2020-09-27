#include "stdafx.h"
#include "resource.h"

#include "shell.hid" // 26 Jun 96 - Dynamic context menu ids for Next/Goto Button

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CGoToDialog dialog

// static member used to prevent multiple instances. Must be declared at file scope
CGoToDialog * CGoToDialog::c_pGotodialog = NULL; // used to prevent multiple instances of modeless dialog

#ifdef DEBUG  // flags to check that needed functions are called by package onselect
BOOL fOnSelCalledButtonStyle = FALSE;
BOOL fOnSelCalledControlStyle = FALSE;
BOOL fOnSelCalledPrompt = FALSE;
#endif

// Constructor (MUST HAVE HELP ID!!!)
CGoToDialog :: CGoToDialog(UINT idHelp, LPCTSTR szItemInit, CWnd* pParent /*=NULL*/)
	: C3dDialog(CGoToDialog::IDD, pParent)
{
	SetHelpID( idHelp );
	c_pGotodialog = this;
	m_fFirstMoveMsg = FALSE; // will be set true in initdialog
	m_fForcePinForHelp = FALSE;

	ASSERT(m_strItemInit.IsEmpty());
	if (szItemInit != NULL && *szItemInit != '\0')
	{
		m_strItemInit = szItemInit; // copy string
		ASSERT(!m_strItemInit.IsEmpty());
	}
}


// destructor
CGoToDialog::~CGoToDialog()
{
	c_pGotodialog = NULL;

}
// this is a static member function
CGoToDialog *CGoToDialog::PGoToDialogCur()
{
	return (c_pGotodialog);

}

void CGoToDialog::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGoToDialog)
	DDX_Control(pDX, IDC_GOTOCOMBO, m_gotoCombo);
	DDX_Control(pDX, IDC_GOTOHELPTEXT, m_helpText);
	DDX_Control(pDX, IDC_GOTOCAPTION, m_gotoCaption);
	DDX_Control(pDX, IDC_GOTOWHAT, m_gotoWhat);
	DDX_Control(pDX, IDC_GOTOLIST, m_gotoList);
	DDX_Control(pDX, IDC_GOTOEDIT, m_gotoEdit);
	//}}AFX_DATA_MAP
}

IMPLEMENT_DYNAMIC(CGoToDialog, CDialog)  // skip C3dDialog since not dynamic. Randy says this is ok

BEGIN_MESSAGE_MAP(CGoToDialog, C3dDialog)
	//{{AFX_MSG_MAP(CGoToDialog)
	ON_LBN_SELCHANGE(IDC_GOTOLIST, OnSelchangeGoToList)
	ON_LBN_SELCHANGE(IDC_GOTOWHAT, OnSelchangeGoToWhat)
	ON_CBN_SELCHANGE(IDC_GOTOCOMBO, OnSelchangeGoToCombo)
	ON_BN_CLICKED(ID_PREVIOUS, OnPrevious)
	ON_EN_CHANGE(IDC_GOTOEDIT, OnChangeGoToEdit)
	ON_CBN_EDITCHANGE(IDC_GOTOCOMBO, OnEditchangeGoToCombo)
	ON_WM_ACTIVATE()
	ON_WM_MOVE()
	ON_BN_CLICKED(ID_GOTO_GOTOANDNEXT, OnGotoGotoAndNext)
	ON_WM_ACTIVATEAPP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoToDialog message handlers

BOOL CGoToDialog::OnHelpInfo(HELPINFO* pInfo) 
{
	// need to prevent this dialog going away when decativated by help.
	// set this flag, which causes deactivate code to not take down the dialog. The 
	// flag is cleared in the activate or activateapp code. one of which we will receive.
	// this is rather peculiar, but gets us past a multitude of evils, like the pin button changing
	// state when clicked in the ? state.  bobz 9/25/96

	m_fForcePinForHelp = TRUE;  // cleared in OnActivate

	return (DoHelpInfo(this, m_nIDHelp, pInfo));
}


BOOL CGoToDialog::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);

		return C3dDialog::PreTranslateMessage(pMsg);
	}
}

void CGoToDialog::OnChangeGoToEdit() 
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	ASSERT (m_pGoToItemCur != NULL);

	CString str;

	GetSelectData(str, NULL, NULL, NULL, FALSE /* fComboSelChange */);
	m_pGoToItemCur->OnSelectChange(str, FALSE, dwItemDataInvalid);

	m_cLockSelectAtActivate--;  // allow reset on activation
}


void CGoToDialog::OnSelchangeGoToList() 
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	ASSERT (m_pGoToItemCur != NULL);

	CString str;
	DWORD dwItemData;
	BOOL fSelInList;

	GetSelectData(str, &dwItemData, NULL, &fSelInList, FALSE /* fComboSelChange */);
	m_pGoToItemCur->OnSelectChange(str, fSelInList, dwItemData);

	m_cLockSelectAtActivate--;  // allow reset on activation
}

void CGoToDialog::OnSelchangeGoToCombo() 
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	ASSERT (m_pGoToItemCur != NULL);

	CString str;
	DWORD dwItemData;
	BOOL fSelInList;

	GetSelectData(str, &dwItemData, NULL, &fSelInList, TRUE /* fComboSelChange */);
	m_pGoToItemCur->OnSelectChange(str, fSelInList, dwItemData);
		
	m_cLockSelectAtActivate--;  // allow reset on activation
}

void CGoToDialog::OnEditchangeGoToCombo() 
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	ASSERT (m_pGoToItemCur != NULL);

	CString str;
	DWORD dwItemData;
	BOOL fSelInList;

	GetSelectData(str, &dwItemData, NULL, &fSelInList, FALSE /* fComboSelChange */);
	m_pGoToItemCur->OnSelectChange(str, fSelInList, dwItemData);

	m_cLockSelectAtActivate--;  // allow reset on activation

}


// Helper: gets text and item data and the dialog id from the current selection control
// NOTE: for a combo, we return the text in the selected item in the
// list box if the edit text is a prefix of a list box entry, and the edit control text otherwise
// fComboSelChanged is used for a combo, because on a CBN_SELCHANGE, the selection is updated but the
// edit text is not, where on a CBN_EDITCHANGE, the edit text change and the selection has not, so you
// need to know where to look for data in that case
void CGoToDialog::GetSelectData(CString &strData, 
			DWORD *pdwItemData, 
			int *pidSelect,
			BOOL *pfSelInList,
			BOOL fComboSelChanged) 
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	ASSERT (m_pGoToItemCur != NULL);

	int iItem;
	DWORD dwItemData = dwItemDataInvalid;
	BOOL fSelInList = FALSE;
	int idSelect = -1;

	strData.Empty();

	if (m_csSelection == csDropList)
	{
		iItem = m_gotoList.GetCurSel();
		if (iItem != CB_ERR)
		{
			m_gotoList.GetText(iItem, strData);
			dwItemData = m_gotoList.GetItemData(iItem);
			idSelect = IDC_GOTOLIST;
			fSelInList = TRUE;
		}

	} // droplist
	else if (m_csSelection == csDropCombo)
	{
		idSelect = IDC_GOTOCOMBO;

		// use FindString to determine if the edit text is a prefix of a list entry.
		// if so, treat the list entry as being selected. This handles cases when the list box is not
		// visible and no actual selection exists

		// if we change the combo list selection, we get a selchange BEFORE the edit text is updated.
		// in that case, get the text from the selection. Otherwise use the text in the edit control
		// and use findstring to see if there is a prefix match

		if (fComboSelChanged)
		{
			iItem = m_gotoCombo.GetCurSel();
			if (iItem != CB_ERR)  // use selection in list box, if any
			{
				fSelInList = TRUE;
				// if there is a selection, use full LB text rather than
				// contents of edit control
				m_gotoCombo.GetLBText(iItem, strData);
				dwItemData = m_gotoCombo.GetItemData(iItem);
			}
		}
		else  // edit text changed, or not in a changing state (OK?Previous)
		{
			m_gotoCombo.GetWindowText(strData);
			if (!strData.IsEmpty())  // defaults are all correct for the empty case
			{
				iItem = m_gotoCombo.FindString(-1, strData); // look for prefix match in list
				if (iItem != CB_ERR)  // If no match, strData is correct (edit text), and other args are correctly set up now
				{
					fSelInList = TRUE;
					// if there is a selection, use full LB text rather than
					// contents of edit control
					m_gotoCombo.GetLBText(iItem, strData);
					dwItemData = m_gotoCombo.GetItemData(iItem);
				}
			}
		}
	}  // combo
	else 
	{
		ASSERT (m_csSelection == csEdit);
		m_gotoEdit.GetWindowText(strData);
		idSelect = IDC_GOTOEDIT;
	}

	if (pdwItemData != NULL)
			*pdwItemData = dwItemData;
	if (pidSelect != NULL)
			*pidSelect = idSelect;
	if (pfSelInList != NULL)
			*pfSelInList = fSelInList;
	
	m_cLockSelectAtActivate--;  // allow reset on activation
}

// handle change in selection of Go To What list
void CGoToDialog::OnSelchangeGoToWhat() 
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated
	DoSelchangeGoToWhat();
	GotoDlgCtrl(&m_gotoWhat);  // grab focus back
	m_cLockSelectAtActivate--;  // allow reset on activation
}

// handle change in selection of Go To What list
void CGoToDialog::DoSelchangeGoToWhat() 
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	// store the active view whenever we come up. Could change while up (as in goto bookmark can change
	// the file you are looking at, and changing to gotoline will need new view.

	m_pActiveView = (CPartView *) FromHandle(theApp.GetActiveView());

	int iItem = m_gotoWhat.GetCurSel();

	m_pGoToItemCur = (CGoToItem *)m_gotoWhat.GetItemDataPtr(iItem);
	if (m_pGoToItemCur == NULL || iItem == LB_ERR)
	{
		ASSERT(FALSE);  // should never happen
		return;
	}

	m_helpText.SetWindowText(""); // because package may not support SetHelperText
	// debug flags to ensure that package called necessary functions
#ifdef DEBUG
	fOnSelCalledButtonStyle = FALSE;
	fOnSelCalledControlStyle = FALSE;
	fOnSelCalledPrompt = FALSE;
#endif

	m_pGoToItemCur->OnSelectWhat();

#ifdef DEBUG
	ASSERT(fOnSelCalledButtonStyle);  
	ASSERT(fOnSelCalledControlStyle);
	ASSERT(fOnSelCalledPrompt);

#endif

	BOOL fEnableItem;
	fEnableItem =  m_pGoToItemCur->CanGoViewActive(m_pActiveView);  // just so each package does not need to do this in OnSelect
	SetItemEnabled(m_pGoToItemCur, fEnableItem); // enable/disable relevant controls
	
	if (fEnableItem)
	{
		// if item enabled, force button state settings to reflect initial contents of selection control
		CString str;
		DWORD dwItemData;
		BOOL fSelInList;

		GetSelectData(str, &dwItemData, NULL, &fSelInList, FALSE /* fComboSelChange */);
		m_pGoToItemCur->OnSelectChange(str, fSelInList, dwItemData);
	}
	// select selection control if any
	GotoCtlSel();
	m_cLockSelectAtActivate--;  // allow reset on activation
}

// Note: since close button id == IDCANCEL, close box, esc and the Close button go through here
void CGoToDialog::OnCancel() 
{
	// if we come in multiple times - possible due to calling this on deactivate, get out or we destroy ourselves again
	if (GetSafeHwnd() == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	if (m_cBusyNoClose != 0) // goto operation still in progress
	{
		ErrorBox(ERR_Goto_Busy);
		m_cLockSelectAtActivate--;  // allow reset on activation
		return;  // leave dialog up
	}
	else
	{
		if (m_btnPushPin.GetSafeHwnd() != NULL)	// handles failure case in InitDialog
			theApp.m_bPinGoToDlg = m_btnPushPin.IsButtonDown();  // save in global

		// delete all the items we received
		int i;
		int cItem =  m_gotoWhat.GetCount();
		for (i = 0; i < cItem; i++)
			{
			ASSERT(m_gotoWhat.GetItemDataPtr(i) != NULL);
			delete m_gotoWhat.GetItemDataPtr(i);
			}
		
		m_gotoWhat.ResetContent();

		CPartView *pActiveView = m_pActiveView;

		m_cLockSelectAtActivate--;  // allow reset on activation
		if (GetSafeHwnd() != NULL)	// safety measure in case of already down when we get this message
			DestroyWindow();  // since modeless, don't call default which calls EndDialog

		if (pActiveView != NULL)
			pActiveView->SetFocus();
	}
}

void CGoToDialog::OnPrevious() 
{
	OnOKPrevious(goPrevious); // OnOkPrevious sets m_cLockSelectAtActivate. don;t need it here
}

/* See OnGotoGotoAndNext...
void CGoToDialog::OnOK() 
{
		// OK button can be goto or next
	GOTO_TYPE go = (m_bsCur == bsGoTo ? goGoTo: goNext);
	OnOKPrevious(go);  // OnOkPrevious sets m_cLockSelectAtActivate. don;t need it here
}
*/

void CGoToDialog::OnOKPrevious(GOTO_TYPE go) 
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated
	m_cBusyNoClose++;  // prevents closing dialog. See fat note below

	ASSERT (m_pGoToItemCur != NULL);
	CString str;
	DWORD dwItemData;
	int idSelect;
	BOOL fSelInList;
	CGoToItem * pGoToItemCur; // local copies of dialog globals
	CPartView * pActiveView;  

	// handle case when you click on ok button when inactive and out of context
	if (m_pGoToItemCur == NULL || (!m_pGoToItemCur->CanGoViewActive(m_pActiveView)))
	{
		::MessageBeep(0);
		goto LRet;
	}

	GetSelectData(str, &dwItemData, &idSelect, &fSelInList, FALSE /* fComboSelChange */);

	// FAT NOTE (bobz): The universe can change between CanGoTo and OnGoTo. For example,
	// GotoReference can spawn a build to create a bsc file. While that is going on, you could
	// change the doc you are pointing to, and click on the goto dialog again, among
	// other things. The dialog global state, like m_pGoToItemCur and m_pGoToItemCur might
	// have changed. One solution would be to dump CanGoTo and put all the check and
	// go code into OnGoTo, but that is a fairly radical change. So instead this is the scheme:
	// We save local copies of the dialog state (globals). After CanGoTo, if the current global
	// state matches the local state, we do on goto, otherwise we act as if CanGoTo had failed.
	// We resync up the dialog and the view after CanGoTo, whether or not we did OnGoTo

	// Actually what is even worse,is that the user could close the dialog during the CanGoTo Processing
	// and OnGoTo would crash. So we now set a counter to effectively disable the Close button 
	// around can/on goto and reset at the end. This disables ESC and the X (Close) button too.

	// So we allow the dialog to be used while something like a build is on, but it can't be closed
	// until process completes - an error box comes up in that case.


	pGoToItemCur = m_pGoToItemCur; // save local copy of global state in case globals change
	pActiveView = m_pActiveView;

	if (m_pGoToItemCur->CanGoTo(idSelect, go, str, fSelInList, dwItemData))
	{
		// implied else clause is to just not do the go to
		if (pGoToItemCur == m_pGoToItemCur &&  pActiveView == m_pActiveView)
		{
			CString strCur;
			GetSelectData(strCur, NULL, NULL, NULL, FALSE /* fComboSelChange */);
			if (str == strCur)
				m_pGoToItemCur->OnGoTo(go, str, fSelInList, dwItemData);  
		}
	}

	// above may cause a change in view. Determine if a change, if so, call change view function
	// then update m_pActiveView

	{  // BLOCK
	CPartView *pView = (CPartView *) FromHandle(theApp.GetActiveView());
	ASSERT((pView == (CPartView *)NULL) || pView->IsKindOf(RUNTIME_CLASS(CPartView)));
	if (pView != m_pActiveView)  // deal with changing view. If item needs to reset itself on view
								 // change, do that in CanGoViewActive 
	{
		BOOL fEnableItem;

		pGoToItemCur = m_pGoToItemCur; // save local copy of global state in case globals change
		pActiveView = m_pActiveView;	// don;t bother checking string text as well in this case bobz

		fEnableItem = m_pGoToItemCur->CanGoViewActive(pView);  // call before resetting dialog state so CanGo can check differences

		if (pGoToItemCur != m_pGoToItemCur ||  pActiveView != m_pActiveView)
		{
			ASSERT(FALSE);  // unexpected change, just disable for now
			fEnableItem = FALSE;
		}

		SetItemEnabled(m_pGoToItemCur, fEnableItem); // enable/disable relevant controls

		m_pActiveView = pView;
	}
		// scroll out of the way if in a text view
	if ((pView != (CPartView *)NULL))
		pView->SetDialogPos(FALSE /* fFirstTime */, this->m_hWnd);
	}


	// Set the focus back to the selection control, if any
	GotoCtlSel();

  // do not call OnOk to dismiss - let close do that (modeless)
LRet:
	m_cBusyNoClose--;	// allows dialog to be closed
	ASSERT(m_cBusyNoClose == 0);
  	m_cLockSelectAtActivate--;  // allow reset on activation
	ASSERT(m_cLockSelectAtActivate >= 0);

	if (!m_btnPushPin.IsButtonDown())
		OnCancel();
}


afx_msg void CGoToDialog::OnMove(int cx, int cy)
{
	C3dDialog::OnMove(cx, cy);

	if (m_fFirstMoveMsg)
	{
		CPartView *pView = (CPartView *) FromHandle(theApp.GetActiveView());
		if ((pView != (CPartView *)NULL))
			pView->SetDialogPos(TRUE /* fFirstTime */, this->m_hWnd);
	}
	m_fFirstMoveMsg = FALSE;
}


void CGoToDialog::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
// This is rather complex because we want to force reinitialization sometimes, mainly when the
// user clicks away from the dialog, but not other times, like when an error box comes up or
// a goto action sets the focus on the doc. So the current scheme reinits the selection on
// activation if the active view has changed. 
// bobz

	C3dDialog::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_INACTIVE)
	{
		// change in behavior (bobz) to act more like property sheets, this dialog will disappear
		// on deactivation if it is unpinned.
		// 	m_fForcePinForHelp is reset in the OnActivateApp handler and in the ACTIVE side of this code
		if (!m_fForcePinForHelp)
			if ((m_btnPushPin.GetSafeHwnd() != NULL) && !m_btnPushPin.IsButtonDown())
			{
				if (GetSafeHwnd() != NULL)	// safety measure in case of already down when we get this message
					PostMessage(WM_CLOSE); // take it down
			}
		return;
	}

	m_fForcePinForHelp = FALSE; // stay up but let us come down next time

	// Consider activation a time to reestablish the selection, since the basis for selection
	// may have changed. However, since we can lose activation for things like message boxes, we
	// don't always want to reset on activation, as we may throw away useful state. So we set a
	// sentinal (m_cLockSelectAtActivate) when we want to prevent reselection

	// Note there are some scenarios. like Bookmarks, where the act of going to a new doc may take the
	// focus from this dialog. If this happens, and the item wants to reclaim the focus for the dialog,
	// it must be careful, because activation will take place.The sentinal will be set in such cases. Be
	// sure to restablish the dialog's m_pActiveView in that case however.


	CPartView *pView = (CPartView *) FromHandle(theApp.GetActiveView());
	ASSERT((pView == (CPartView *)NULL) || pView->IsKindOf(RUNTIME_CLASS(CPartView)));
	m_pActiveView = pView;

	if (!m_cLockSelectAtActivate)  // counter, not flag. Setters use ++ and --
	{
		m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

		DoSelchangeGoToWhat(); // reestablish current what selection. Will reset m_pActiveView as well

		m_cLockSelectAtActivate--;  // allow reset on activation
	}
}

BOOL CGoToDialog::OnInitDialog() 
{
// wrapper function for DoInitDialog so code does not have to worry about exiting w/o resetting mode flag
	m_fInInitDialog = TRUE; // mode flag
	m_cLockSelectAtActivate = 1; // prevent reset if we are inactivated and reactivated. Initial setting
	m_cBusyNoClose = 0; // set in OK handler to prevent dialog close while process underway

	BOOL fRet = DoInitDialog();

	m_fInInitDialog = FALSE; // mode flag
	m_cLockSelectAtActivate--;  // allow reset on activation
	ASSERT(m_cLockSelectAtActivate == 0);

	return fRet;
}

BOOL CGoToDialog::DoInitDialog() 
{
	// mode flags were set in OnInitDialog

	BOOL fRet = C3dDialog::OnInitDialog();

	// 1. init dialog variables.
	m_pctlSelection = NULL;
	m_csSelection = csUninit;
	m_pGoToItemCur = NULL;
	m_bsCur = bsGoTo;
	// 26 Jun 96 - This button needs two different help context ids.
	// The ids can be found in Shell.hid.
	// This line will set up the default help id for this button.
	GetDlgItem(ID_GOTO_GOTOANDNEXT)->SetWindowContextHelpId(HIDC_GOTO_GOTO) ;

	m_fFirstMoveMsg = TRUE;

	// be sure this is NULL so WM_ACTIVATE will force initial selection
	m_pActiveView = NULL;

	// set up pushpin. This does a SubclassDlgItem call
	if (!m_btnPushPin.AutoLoad(IDC_GOTO_PUSHPIN, this, TRUE)) 
	{
		CString str;
		str.LoadString(IDS_ERROR_NO_GOTOITEMS);
		AfxMessageBox(str, MB_OK | MB_ICONEXCLAMATION);
		EndDialog(IDCANCEL);  // enddialog in Initdialog works even for a modeless dialog
		return FALSE;
	}

	m_btnPushPin.SetChecked(theApp.m_bPinGoToDlg);
	
	// Create the ToolTip control.
	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);

	TCHAR szFullText[256];
	CString strTipText;
	AfxLoadString(ID_PROP_PUSHPIN, szFullText);
		// this is the tt text which follows the status line text
	AfxExtractSubString(strTipText, szFullText, 1, '\n');

	m_tooltip.AddTool((CWnd *)&m_btnPushPin, strTipText);

	// 2. walk the packages andd call AddGoToItems. 

	POSITION pos = theApp.m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		CPackage* pPackage = (CPackage*)theApp.m_packages.GetNext(pos);
		pPackage->AddGoToItems(this);
	}

	// can't bring up dialog if nothing to see.
	if (m_gotoWhat.GetCount() < 1)
	{
		CString str;
		str.LoadString(IDS_ERROR_NO_GOTOITEMS);
		AfxMessageBox(str, MB_OK | MB_ICONEXCLAMATION);
		EndDialog(IDCANCEL);  // enddialog in Initdialog works even for a modeless dialog
		return FALSE;
	}
		
	// 3. Get the What string from the items and determine the original selection from
	//		the current editor

	// If we received a strItemInit, and it is in the list, use that as
	// the initial selection. Otherwise look for an appropriate entry

	int iInit = LB_ERR;

	if (!m_strItemInit.IsEmpty())
	 	iInit = m_gotoWhat.FindStringExact(-1, m_strItemInit); // look for exact match

	if (iInit == LB_ERR)
	{
		int i;

		for (i = 0; i < m_gotoWhat.GetCount(); i++)
		{
			CGoToItem * pItem = (CGoToItem *)m_gotoWhat.GetItemDataPtr(i);
			ASSERT (pItem != NULL);  // additem should ensure this

			// seek initial selection based on the first item that can be active
			if (pItem->CanGoViewActive(m_pActiveView))
				{
					iInit = i;
					break;  // don't need to look any longer
				}
		}

		if (iInit == LB_ERR)
			// if still no active items, arbitrarily select first in list
			iInit = 0;
	}

	ASSERT (iInit != LB_ERR);

	// 4. select the appropriate "what" entry

	m_gotoWhat.SetCurSel(iInit);

	DoSelchangeGoToWhat(); // establish current what selection. Will reset m_pActiveView as well

	CenterWindow();
	return 0;  // focus set explicitly, so return 0
}


// call this from CPackage:AddGoToItems for each item to add (editor-
// specific items should be added even if the right kind of editor 
// is not currently added.

// Note that we screen these entries carefully before adding them, so we can trust them later.
void CGoToDialog::AddItem(CGoToItem *pItemNew)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	ASSERT (pItemNew != NULL);  // warning to implementor in package (harmless though: bobz)
	if (pItemNew != NULL)
	{
		LPCTSTR szNew;

		szNew = pItemNew->GetItemString();	
		ASSERT(szNew != NULL);	
		if (szNew != NULL)	
		{
			// disallow duplicate names in list. User should never see this error

			int i;	
			CGoToItem * pItem;
			LPCTSTR szInList;
			for (i = 0; i < m_gotoWhat.GetCount(); i++)
			{
				pItem = (CGoToItem *)m_gotoWhat.GetItemDataPtr(i);
				szInList = pItem->GetItemString();
				// lstrcmp should be ok even for dbcs since it is a byte by byte comparison bobz
				if (lstrcmp(szNew, szInList) == 0)  // same
				{
					ErrorBox(ERR_Duplicate_AddItem, szNew);
					goto LRet;
				}
			}

			// ok - so add it

			i = m_gotoWhat.AddString(szNew);
			if (i != LB_ERR)
				m_gotoWhat.SetItemDataPtr(i, (void *)pItemNew);
		}
	}
LRet:
	m_cLockSelectAtActivate--;  // allow reset on activation
}

// call this from the CGoToItem callbacks to change the style of buttons
// being used. (This must be called from CGoToItem::OnSelect at the very least!)

void CGoToDialog::SetButtonStyle (BUTTON_STYLE buttonstyle)
{
m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

#ifdef DEBUG
	fOnSelCalledButtonStyle = TRUE;  // for assert that OnSelect called me
#endif

	if (buttonstyle != m_bsCur)
	{
		CString str;
		if (buttonstyle == bsGoTo)
		{
			str.LoadString(IDS_GOTO_GOTO);
			GetDlgItem(ID_GOTO_GOTOANDNEXT)->SetWindowText(str);
			// 26 Jun 96 - This button needs two different help context ids.
			// The ids can be found in Shell.hid.
			GetDlgItem(ID_GOTO_GOTOANDNEXT)->SetWindowContextHelpId(HIDC_GOTO_GOTO) ;
		}
		else
		{
			ASSERT(buttonstyle == bsNextPrev);
			str.LoadString(IDS_GOTO_NEXT);
			GetDlgItem(ID_GOTO_GOTOANDNEXT)->SetWindowText(str);
			GetDlgItem(ID_GOTO_GOTOANDNEXT)->SetWindowContextHelpId(HIDC_GOTO_NEXT) ;
		}

	m_bsCur = buttonstyle;
	}
	// reset possible changed enable states
	GetDlgItem(ID_GOTO_GOTOANDNEXT)->EnableWindow(TRUE);
	BOOL fEnablePrev = (buttonstyle == bsNextPrev);
	GetDlgItem(ID_PREVIOUS)->EnableWindow(fEnablePrev);

	m_cLockSelectAtActivate--;  // allow reset on activation

}

// call this from the CGoToItem callbacks to change the style of selection control
// being used. (This must be called from CGoToItem::OnSelect at the very least!)

void CGoToDialog::SetControlStyle (CONTROL_STYLE controlstyle)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated


#ifdef DEBUG
	fOnSelCalledControlStyle = TRUE;  // for assert that OnSelect called me
#endif

	ASSERT (controlstyle != csUninit);
	if (controlstyle != m_csSelection)  // only change stuff if changed
	{
		m_csSelection = controlstyle;

		if (m_pctlSelection != NULL) // hide previous control
			m_pctlSelection->ShowWindow(SW_HIDE);

		if (controlstyle == csDropList)
			m_pctlSelection = &m_gotoList;
		else if (controlstyle == csDropCombo)
			m_pctlSelection = &m_gotoCombo;
		else if (controlstyle == csEdit)
			m_pctlSelection = &m_gotoEdit;
		else
			{
			ASSERT(controlstyle == csNone);
			m_pctlSelection = NULL;
			goto LRet;  // no need to set up control
			}

		m_pctlSelection->EnableWindow(TRUE);
		m_pctlSelection->InvalidateRect(NULL);  // NT sometimes does not redraw
		m_pctlSelection->ShowWindow(SW_SHOW);
		
	}
LRet:
	m_cLockSelectAtActivate--;  // allow reset on activation

}
 
// call this to change the selection prompt. (This must be called from 
// CGoToItem::OnSelect at the very least!)
void CGoToDialog::SetPrompt (const TCHAR * szPrompt)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

#ifdef DEBUG
	fOnSelCalledPrompt = TRUE;  // for assert that OnSelect called me
#endif

	ASSERT (szPrompt != NULL);
	if (szPrompt == NULL)
		m_gotoCaption.SetWindowText("");
	else
		m_gotoCaption.SetWindowText(szPrompt);

	m_gotoCaption.EnableWindow(TRUE);

	m_cLockSelectAtActivate--;  // allow reset on activation

}
 
// call this to enable or disable dialog controls depending on whether the item
// can be active based on the active editor. This is called internally after
// CGoToItem::CanGoViewActive is called. Ignored if pGoToItem is not the current
// selection in the Go To What list.
void CGoToDialog::SetItemEnabled(CGoToItem *pGoToItem, BOOL fEnabledItem)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	ASSERT(m_pGoToItemCur == pGoToItem); // harmless warning to implementor something may be wrong
	if (m_pGoToItemCur == pGoToItem)
	{
		// always do the enables in case some package messes up
		ASSERT(m_pctlSelection != NULL || m_csSelection == csNone); 

		GetDlgItem(ID_GOTO_GOTOANDNEXT)->EnableWindow(fEnabledItem);
		GotoDlgCtrl(GetDlgItem(ID_GOTO_GOTOANDNEXT)); // keeps default appearance better
		if (m_bsCur == bsNextPrev)  //otherwise this button always disabled
			GetDlgItem(ID_PREVIOUS)->EnableWindow(fEnabledItem);
		else
			GetDlgItem(ID_PREVIOUS)->EnableWindow(FALSE);

		m_gotoCaption.EnableWindow(fEnabledItem);
		if (m_pctlSelection != NULL)
		{
			m_pctlSelection->EnableWindow(fEnabledItem);
			GotoCtlSel();
		}

	}
	m_cLockSelectAtActivate--;  // allow reset on activation
}


// call this to change the helper text. If not called from 
// CGoToItem::OnSelect it will be blank by default.
void CGoToDialog::SetHelperText (const TCHAR * szHelperText)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	ASSERT (szHelperText != NULL);
	if (szHelperText == NULL)
		m_helpText.SetWindowText("");
	else
		m_helpText.SetWindowText(szHelperText);

	m_cLockSelectAtActivate--;  // allow reset on activation
}

// call this to empty the selection list when csDropList or csDropCombo are used.
void CGoToDialog::EmptyList ()
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	BOOL fValid;

	fValid = (m_pctlSelection != NULL) && 
			 (m_csSelection == csDropList || m_csSelection == csDropCombo);
	ASSERT (fValid);

	if (fValid)
		{
		((CComboBox *)m_pctlSelection)->ResetContent();
		}

	m_cLockSelectAtActivate--;  // allow reset on activation
}

// call this to add a string to the selection list
void CGoToDialog::AddString (const TCHAR * szSelect, DWORD dwItemData)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	BOOL fValid;

	fValid = (m_pctlSelection != NULL) && (szSelect != NULL) &&
			 (m_csSelection == csDropList || m_csSelection == csDropCombo);
	ASSERT (fValid);

	if (fValid)
	{
		int i;

		CComboBox * pCombo = (CComboBox *)m_pctlSelection;
		i = pCombo->AddString(szSelect);
		if (dwItemData != 0 && i != CB_ERR && i != CB_ERRSPACE)
			pCombo->SetItemData(i, dwItemData);
	}
	m_cLockSelectAtActivate--;  // allow reset on activation

}

// call this to lock/unlock the selection list when csDropList or csDropCombo are used.
// Typically called while the control is being filled.
void CGoToDialog::LockList (BOOL fLock)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	BOOL fValid;

	fValid = (m_pctlSelection != NULL) && (m_csSelection == csDropList || m_csSelection == csDropCombo);
	ASSERT (fValid);
	if (fValid)
		m_pctlSelection->SetRedraw(!fLock);

	m_cLockSelectAtActivate--;  // allow reset on activation
}

// call this to get the dialog id of the "selection control" in the rare
// case when the List and Select functions are not adequate
// pcs will be set to the control style of the control, since it can be
// either and edit control . a combo or a list box. Use GetDlgItem to 
// get a pointer to the selection control.
// In case of csNone, we will return the id for the edit control, which will be invisible
int CGoToDialog::IdCtlSelection (CONTROL_STYLE *pcs)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	CString str;
	int id;

	*pcs = 	m_csSelection;
	GetSelectData(str, NULL, &id, NULL, FALSE /* fComboSelChange */);

	m_cLockSelectAtActivate--;  // allow reset on activation

	return id;
}

// call one of these to initialize the current selection control
void CGoToDialog::Select (const TCHAR * szSelect)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	BOOL fValid;

	fValid = (m_pctlSelection != NULL) && (szSelect != NULL) 
		 && (m_csSelection == csEdit || m_csSelection == csDropCombo);
	ASSERT (fValid);

	if (fValid)
	{
		m_pctlSelection->SetWindowText(szSelect);
	}

	m_cLockSelectAtActivate--;  // allow reset on activation
}

void CGoToDialog::Select (int iSelect)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	BOOL fValid;

	fValid = (m_pctlSelection != NULL) && (iSelect >= 0) 
		 && (m_csSelection == csDropList || m_csSelection == csDropCombo);
	ASSERT (fValid);

	if (fValid)
	{
		CComboBox * pCombo = (CComboBox *)m_pctlSelection;
		pCombo->SetCurSel(iSelect);
	}

	m_cLockSelectAtActivate--;  // allow reset on activation
}

// call this when a CGoToItem::OnSelectChange needs to change the button state
// OR the msk values into grfButton to set the state of one or more buttons

void CGoToDialog::EnableButton (WORD grfButton, BOOL fEnable)
{
	m_cLockSelectAtActivate++; // prevent reset if we are inactivated and reactivated

	// if bsGoTo, Previous button is always disabled, so we ignore any change to that button

	if (grfButton & mskButtonGoToNext)
		GetDlgItem(ID_GOTO_GOTOANDNEXT)->EnableWindow(fEnable);

	if (grfButton & mskButtonPrevious && m_bsCur != bsGoTo)
		GetDlgItem(ID_PREVIOUS)->EnableWindow(fEnable);

	if (grfButton & mskButtonClose)
		GetDlgItem(IDCANCEL)->EnableWindow(fEnable);

	if (grfButton & mskButtonHelp)
		GetDlgItem(ID_HELP)->EnableWindow(fEnable);

	m_cLockSelectAtActivate--;  // allow reset on activation
}

void CGoToDialog::GotoCtlSel()
{
	if (m_pctlSelection != NULL && m_pctlSelection->IsWindowEnabled())
		GotoDlgCtrl(m_pctlSelection);
	else
		GotoDlgCtrl(&m_gotoWhat);
}

void CGoToDialog::PostNcDestroy()
{
	delete this;
}


/////////////////////////////////////////////////////////////////////////////
// CGoToItem

// constructor
CGoToItem::CGoToItem(CGoToDialog * pDlg)
{
	ASSERT(pDlg != NULL);
	m_pDlg = pDlg;
}

 void CGoToItem::OnSelectWhat() 
 {}

 BOOL CGoToItem::CanGoViewActive(CPartView * pActiveView)
 {
 return FALSE;
 }

 void CGoToItem::OnSelectChange(const TCHAR * szSelect, 
		BOOL fSelInList,
		DWORD dwItemData)
 {}

 BOOL CGoToItem::CanGoTo(int idDlgItemError, 
						GOTO_TYPE go, 
						const TCHAR * szSelect, 
						BOOL fSelInList,
						DWORD dwItemData)
 {
 return FALSE;
 }

 void CGoToItem::OnGoTo(GOTO_TYPE go,
						const TCHAR * szSelect,
						BOOL fSelInList,
						DWORD dwItemData)
 {}


////////////////////////////////////////////////////////////////////
// Generic goto dialog utility routines

void DoGoToDialog(int iddHelp, LPCTSTR szItemInit) 
{
	CGoToDialog * pDlgPrev = CGoToDialog::PGoToDialogCur();
	if (pDlgPrev != NULL)
		{

		CPartView *pView = (CPartView *) pDlgPrev->FromHandle(theApp.GetActiveView());
			// scroll out of the way 
		if ((pView != (CPartView *)NULL))
			pView->SetDialogPos(TRUE /* fFirstTime */, pDlgPrev->m_hWnd);
		pDlgPrev->SetFocus();
		pDlgPrev->GotoCtlSel();  // go to either the edit control or goto what list
		}
	else
	{
		CGoToDialog *pDlg = new CGoToDialog(iddHelp,  szItemInit);
		pDlg->Create(MAKEINTRESOURCE(IDD_GOTODIALOG)); // self-deleting dialog
	}

}

//
// 26 Jun 96 - ID_GOTO_GOTOANDNEXT was IDOK, however, we cannot use IDOK with
// a unique help ID, because it gets eaten inside the help.cpp. 
//
void CGoToDialog::OnGotoGotoAndNext() 
{
	// ID_GOTO_GOTOANDNEXT button can be goto or next
	GOTO_TYPE go = (m_bsCur == bsGoTo ? goGoTo: goNext);
	OnOKPrevious(go);  // OnOkPrevious sets m_cLockSelectAtActivate. don;t need it here	
}

void CGoToDialog::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	
	// this is called when context help has gone away. If we were unpinned at
	// context help time, we force a pin so deactivation will not close
	// the dialog. Since we can't otherwise tell when the help goes away, we
	// set a flag and reset here and in OnActivate bobz
	if (bActive)
	{
		C3dDialog::OnActivateApp(bActive, hTask);
	
		// we are being reactivated indirectly after context help and had been unpinned
		// so we'd like to take the dialog down but can't tell if we would be reactivated so
		// jsut turn off the flag.
		if (m_fForcePinForHelp)
		{
			m_fForcePinForHelp = FALSE;
		}
	}  

}
