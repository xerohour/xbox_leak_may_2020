/////////////////////////////////////////////////////////////////////////////
//      KEYCUST.CPP
//              Keyboard customization dialog.

#include "stdafx.h"
#include "main.h"
#include "keycust.h"
#include "imeutil.h"
#include "bardockx.h"
#include "barglob.h"
#include "customiz.h"

#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define KCF_INHERITED 0x0400 // FUTURE: move to main.h

// Parameter to InitScopeList to cause init to current packet
#define FILL_SEL_CURRENT -3
// CATEGORY in the category list that contains all commands
#define CATEGORY_ALL_COMMANDS -1

void GetCmdDescription(UINT nCmdID, CString& str)
{
	LPCTSTR szPrompt;
	if (theCmdCache.GetCommandString(nCmdID, STRING_PROMPT, &szPrompt))
		str = szPrompt;
	else
		VERIFY( str.LoadString( IDS_UNDESCRIBED ) );
}

WORD KcFromAccel(const ACCEL& accel)
{
	WORD kc = accel.key;
	ASSERT((kc & 0xff) == kc);
	if ((accel.fVirt & FVIRTKEY) == 0)
		kc |= KCF_ASCII;
	if ((accel.fVirt & FSHIFT) != 0)
		kc |= KCF_SHIFT;
	if ((accel.fVirt & FCONTROL) != 0)
		kc |= KCF_CONTROL;
	if ((accel.fVirt & FALT) != 0)
		kc |= KCF_ALT;
	return kc;
}

void AccelFromKc(ACCEL& acc, WORD kc, UINT nCmdID = 0)
{
	acc.fVirt = 0;
	if ((kc & KCF_ASCII) == 0)
		acc.fVirt |= FVIRTKEY;
	if ((kc & KCF_SHIFT) != 0)
		acc.fVirt |= FSHIFT;
	if ((kc & KCF_CONTROL) != 0)
		acc.fVirt |= FCONTROL;
	if ((kc & KCF_ALT) != 0)
		acc.fVirt |= FALT;
	acc.key = kc & 0xff;
	acc.cmd = (WORD)nCmdID;
}

/////////////////////////////////////////////////////////////////////////////
// CKeyCustDlg dialog

CKeyCustDlg::CKeyCustDlg(AccTable* pAccTable, int nAccTables)
	: CCommandTab(CKeyCustDlg::IDD, IDS_KEYBOARD)
{
	m_iCurScope = -1;
	m_pKeymap = NULL;
	m_pAppToolGroups = NULL;

	//{{AFX_DATA_INIT(CKeyCustDlg)
	//}}AFX_DATA_INIT
}

CKeyCustDlg::~CKeyCustDlg()
{
	if (m_pAppToolGroups != NULL)
		CAppToolGroups::ReleaseAppToolGroups();
}

void CKeyCustDlg::DoDataExchange(CDataExchange* pDX)
{
	CCommandTab::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyCustDlg)
	DDX_Control(pDX, IDC_SCOPE_LIST, m_ScopeList);
	DDX_Control(pDX, IDC_CMD_LIST, m_CmdList);
	DDX_Control(pDX, IDC_CAT_COMBO, m_CatList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKeyCustDlg, CCommandTab)
	//{{AFX_MSG_MAP(CKeyCustDlg)
	ON_WM_VKEYTOITEM()
	ON_CBN_SELCHANGE(IDC_CAT_COMBO, OnSelChangeCatList)
	ON_LBN_SELCHANGE(IDC_CMD_LIST, OnSelChangeCmdList)
	ON_BN_CLICKED(IDC_ASSIGN, OnClickedAssign)
	ON_BN_CLICKED(IDC_REMOVE, OnClickedRemove)
	ON_BN_CLICKED(IDC_RESET_ALL, OnClickedResetAll)
	ON_CBN_SELCHANGE(IDC_SCOPE_LIST, OnSelChangeScopeList)
	ON_LBN_SELCHANGE(IDC_KEY_LIST, OnSelChangeKeyList)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_CONTROL(TAPN_CHANGE, IDC_TAP, OnChangeTap)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyCustDlg operations

int CKeyCustDlg::InitScopeList(int nSel)
{
	CComboBox* pList = (CComboBox*)GetDlgItem(IDC_SCOPE_LIST);
	ASSERT(pList != NULL);
	pList->ResetContent();

	// use these to work out which list entry corresponds to the current editor window
	CPacket *pActivePacket=theApp.m_pActivePacket;
	int nCurrentIndex=-1;
	
	POSITION pos = CKeymap::c_keymaps.GetHeadPosition();
	while (pos != NULL)
	{
		CKeymap* pKeymap = (CKeymap*)(void*)CKeymap::c_keymaps.GetNext(pos);
		if (!pKeymap->m_strName.IsEmpty())
		{
			int index = pList->AddString(pKeymap->m_strName);
			pList->SetItemData(index, (DWORD)pKeymap);

			if(pKeymap->m_pPacket==pActivePacket && nCurrentIndex==-1)
			{
				nCurrentIndex=index;
			}
		}
	}

	if(nSel != FILL_NO_SEL)
	{
		if(	nSel==FILL_SEL_CURRENT)
		{
			if(nCurrentIndex!=-1)
			{
				nSel=nCurrentIndex;
			}
			else
			{
				nSel=0;
			}
		}
		pList->SetCurSel(nSel);
		OnSelChangeScopeList();
	}
	return pList->GetCurSel();
}

int CKeyCustDlg::FillCategoriesList(int nSel)
{
	CComboBox* pList = (CComboBox*)GetDlgItem(IDC_CAT_COMBO);
	ASSERT(pList != NULL);
	pList->ResetContent();

	m_pAppToolGroups->FillGroupList( pList );

	// add all commands at the end of the list
	CString strFilter;
	strFilter.LoadString(IDS_ALLCOMMANDS);
	int nIndex=pList->AddString(strFilter);
	if(nIndex!=LB_ERR)
	{
		pList->SetItemData(nIndex, CATEGORY_ALL_COMMANDS);
	}

	if(nSel != FILL_NO_SEL)
	{
		pList->SetCurSel(nSel);
		OnSelChangeCatList();
	}
	return pList->GetCurSel();
}

int CKeyCustDlg::FillCommandsList(int nCat, int nSel)
{
	ASSERT(nCat >= 0 || nCat==CATEGORY_ALL_COMMANDS); 
	
	CListBox* pList = (CListBox*)GetDlgItem(IDC_CMD_LIST);
	ASSERT(pList != NULL);
	pList->SetRedraw(FALSE);
	pList->ResetContent();

	if(nCat!=CATEGORY_ALL_COMMANDS)
	{
		m_pAppToolGroups->FillCommandList( nCat, pList );
	}
	else
	{
		theCmdCache.FillAllCommandsList(pList);
	}
	
	if(nSel != FILL_NO_SEL)
	{
		pList->SetCurSel(nSel);
		OnSelChangeCmdList();
	}
	pList->SetRedraw(TRUE);
	pList->Invalidate();
	
	return pList->GetCurSel();
}

void CKeyCustDlg::AddKeymapToListBox(CKeymap* pKeymap, CListBox* pList, UINT nID, 
	const char* szPrefix, WORD kcPrefix, BOOL bInherited /* = FALSE */)
{
	POSITION pos = m_pKeymap->GetStartPosition();
	while (pos != NULL)
	{
		WORD kc;
		UINT nCmdID;
		CKeymap* pSubKeymap;
		pKeymap->GetNextKey(pos, kc, nCmdID, pSubKeymap);
		
		ACCEL acc;
		AccelFromKc(acc, kc, nCmdID);
		
		WORD kc2 = kc | (bInherited ? KCF_INHERITED : 0);
		if (nCmdID == nID)
		{
			int index = pList->AddString(CString(szPrefix) + CTap::MakeAccelName(acc));
			pList->SetItemData(index, kcPrefix == 0 ? MAKELONG(kc2, 0) : MAKELONG(kcPrefix, kc2));
		}
		else if (pSubKeymap != NULL)
		{
			AddKeymapToListBox(pSubKeymap, pList, nID, CTap::MakeAccelName(acc) + ", ", kc2);
		}
	}
}

int CKeyCustDlg::FillKeysList(UINT nID, int nSel)
{
	CListBox* pList = (CListBox*)GetDlgItem(IDC_KEY_LIST);
	ASSERT(pList != NULL);
	pList->ResetContent();
	if (m_iCurScope != 0)
	{
		CComboBox* plstScope = (CComboBox*)GetDlgItem(IDC_SCOPE_LIST);
		AddKeymapToListBox((CKeymap*)plstScope->GetItemData(0), pList, nID, "", 0, TRUE);
	}
	AddKeymapToListBox(m_pKeymap, pList, nID, "", 0);

	if(nSel != FILL_NO_SEL)
	{
		if (pList->GetCount() > 0)
			pList->SetCurSel(nSel);
		OnSelChangeKeyList();
	}

	// If the list contains no elements, it should be disabled.
//      pList->EnableWindow(pList->GetCount() > 0);

	return pList->GetCurSel();
}


/////////////////////////////////////////////////////////////////////////////
// CKeyCustDlg message handlers

BOOL CKeyCustDlg::OnInitDialog()
{
	CCommandTab::OnInitDialog();
	
	ReloadMacros();
	m_pAppToolGroups = CAppToolGroups::GetAppToolGroups(TRUE);

	// Create Tap control in place of the placeholder
	// This control must be dynamically created to get all the features I want.

	CWnd* pWnd = GetDlgItem(IDC_PLACEHOLDER);
	if(pWnd == NULL)
		return TRUE;

	CRect rc;
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	CString str = "";       // STUB: generate the name of the accelerator that is initially to be listed

	VERIFY(m_Tap.Create(str, rc, this, IDC_TAP));

	// Copy the help id from the placeholder
	DWORD dwHelpID=pWnd->GetWindowContextHelpId();

	// Move the tap control into the correct place in the tab order
	m_Tap.SetWindowPos( pWnd, 0, 0, 0, 0, 
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE );
	pWnd->DestroyWindow();

	m_Tap.SetWindowContextHelpId(dwHelpID);
	
	// Fill the various list boxes

	InitScopeList(FILL_SEL_CURRENT);
	FillCategoriesList(0);
	int nCmdsList = 0;

	// If we're supposed to auto-select a command (launched from
	//  Tools.Macro), then do it now.
	if (!m_strInitialCommand.IsEmpty())
	{
		CToolGroup* pToolGroup = m_pAppToolGroups->GroupFromCommandName(m_strInitialCommand);
		if (pToolGroup == NULL)
 		{
			ASSERT(FALSE);
			m_strInitialCommand.Empty();
		}
		else
		{
			// We've got the right group, so find its index in the categories list
			nCmdsList = m_CatList.FindStringExact(-1, pToolGroup->m_strGroup);
			if (nCmdsList == CB_ERR)
			{
				// The command's category isn't in the combo.  Can't be!
				ASSERT(FALSE);
				m_strInitialCommand.Empty();
				nCmdsList = 0;
			}
		}
	}

	m_CatList.SetCurSel(nCmdsList);
	OnSelChangeCatList();
	DWORD dwCat = m_CatList.GetItemData(nCmdsList);
	FillCommandsList(dwCat);

	if (!m_strInitialCommand.IsEmpty())
	{
		// Select the command passed in
		m_CmdList.SelectString(-1, m_strInitialCommand);
		OnSelChangeCmdList();

		// Done with m_strInitialCommand, so empty it out
		m_strInitialCommand.Empty();
	}

	pWnd = GetDlgItem(IDC_USEDBY_CAP);
	pWnd->ShowWindow(SW_HIDE);
	
	CFont* pFont = GetStdFont(FALSE);
	GetDlgItem(IDC_USEDBY)->SetFont(pFont);
	GetDlgItem(IDC_DESCRIPTION_KEYBOARD)->SetFont(pFont);

	m_keyList.SubclassDlgItem(IDC_KEY_LIST, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//REVIEW: Support for a dirty command cache
BOOL CKeyCustDlg::Activate (CTabbedDialog* pParentWnd, CPoint position)
{
	if (m_pAppToolGroups != NULL)
	{
		if (m_pAppToolGroups->ScanCommandCache())
		{
			CComboBox* pList = (CComboBox*)GetDlgItem(IDC_CAT_COMBO);
			FillCategoriesList(pList->GetCurSel());
			OnSelChangeCatList();
		}
	}

	return CCommandTab::Activate(pParentWnd, position);
}

/////////////////////////////////////////////////////////////////////////////
// CKeyCustDlg's Listboxes' LBN_SELCHANGE handlers

void CKeyCustDlg::OnSelChangeScopeList()
{
	CComboBox* plstScope = (CComboBox*)GetDlgItem(IDC_SCOPE_LIST);

	m_iCurScope = plstScope->GetCurSel();
	ASSERT(m_iCurScope >= 0);
	m_pKeymap = (CKeymap*)plstScope->GetItemData(m_iCurScope);

	OnSelChangeCmdList();           // will update Keys list
	OnChangeTap();
}

void CKeyCustDlg::OnSelChangeCatList()
{
	CComboBox* plstCat = (CComboBox*)GetDlgItem(IDC_CAT_COMBO);
	int indCat = plstCat->GetCurSel();
	DWORD dwCat = plstCat->GetItemData(indCat);

	CListBox* plstCmd = (CListBox*)GetDlgItem(IDC_CMD_LIST);
	int indCmd = plstCmd->GetCurSel();
	DWORD dwCmd = plstCmd->GetItemData(indCmd);

	FillCommandsList((int)dwCat, FILL_NO_SEL);
	plstCmd->SetCurSel(MaintainListSel(plstCmd, dwCmd));
	OnSelChangeCmdList();
}

void CKeyCustDlg::OnSelChangeCmdList()
{
	CListBox* plstCmd = (CListBox*)GetDlgItem(IDC_CMD_LIST);
	int indCmd = plstCmd->GetCurSel();
	DWORD dwCmd;
	if(indCmd == -1)
	{
		dwCmd = DWORD(-1L);
		m_Tap.EnableWindow(FALSE);              // no command to assign accels to
		EnableButton(GetDlgItem(IDC_ASSIGN), FALSE);
		GetDlgItem(IDC_DESCRIPTION_KEYBOARD)->SetWindowText(""); // clear description
	}
	else
	{
		dwCmd = plstCmd->GetItemData(indCmd);
		m_Tap.EnableWindow(TRUE);               // now valid command to assign new accels to
		BOOL bLegal = m_Tap.IsAccelLegal() && m_Tap.GetAccel().key;

		EnableButton(GetDlgItem(IDC_ASSIGN),bLegal);

		CString str;
		GetCmdDescription((UINT) dwCmd, str);
		GetDlgItem(IDC_DESCRIPTION_KEYBOARD)->SetWindowText(str);
	}

	FillKeysList((int)dwCmd);
}

void CKeyCustDlg::OnSelChangeKeyList()
{
	CListBox* plstKey = (CListBox*)GetDlgItem(IDC_KEY_LIST);
	int indKey = plstKey->GetCurSel();
	DWORD kcPair = plstKey->GetItemData(indKey);
	EnableButton(GetDlgItem(IDC_REMOVE), indKey >= 0 && 
		(kcPair & MAKELONG(KCF_INHERITED, KCF_INHERITED)) == 0);
}

int CKeyCustDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	if(pListBox->GetDlgCtrlID() == IDC_KEY_LIST)
	{
		// FUTURE: key mapping should map to Accel's nChar, not the listbox string
	}
	
	return CCommandTab::OnVKeyToItem(nKey, pListBox, nIndex);
}

BOOL CKeyCustDlg::GetCurrentAssignment(CKeymap* pKeymap, CString& str)
{
	ACCEL accel = m_Tap.GetAccel();
	WORD kc = KcFromAccel(accel);

	if (m_Tap.IsKeySet() && m_Tap.IsAccelLegal())
	{
		UINT nCmdID;

		if (pKeymap->LookupKey(kc, nCmdID, pKeymap))
		{
			if (nCmdID == 0)
			{
				ASSERT(pKeymap != NULL);

				// Lookup second key if we have one...
				if (m_Tap.IsChordSet() && m_Tap.IsChordLegal())
				{
					accel = m_Tap.GetChord();
					kc = KcFromAccel(accel);

					if (pKeymap->LookupKey(kc, nCmdID, pKeymap))
					{
						ASSERT(pKeymap == NULL);
						ASSERT(nCmdID != NULL);
						m_pAppToolGroups->GetCommandName(nCmdID, str);
						return TRUE;
					}
					else
					{
						VERIFY( str.LoadString( IDS_UNASSIGNED ) );
						return FALSE;
					}
				}
				else
				{
					VERIFY( str.LoadString( IDS_PREFIXKEY ) );
					return TRUE;
				}
			}
			else
			{
				m_pAppToolGroups->GetCommandName(nCmdID, str);
				return TRUE;
			}
		}
		else if ( IsMenuAccel(kc) )
		{
			VERIFY( str.LoadString( IDS_MENUACCESS ) );
			return TRUE;
		}
		else
		{
			VERIFY( str.LoadString( IDS_UNASSIGNED ) );
			return FALSE;
		}
	}
	else
	{
		// Leave the string empty...
		return FALSE;
	}

	ASSERT(FALSE); // NOT REACHED
}

// This function used to cache the accelerators. Because of customisation, it can't do that now. This could be
// a speed problem; we shall have to see. If so we'd need to find a way to know if the menu bar had changed.

// The code is further complicated by the need to allow for command bar and menu bar modes. In command bar mode,
// menu accelerators include accelerators for textual toolbar buttons placed on the menu bar, so we must scan the
// real menu bar. In menu bar mode, the menu bar is defined by its current state, since no other representation
// of the menu bar exists any longer

// If we have to do this too often, a menu bar proxy will be in order.
BOOL CKeyCustDlg::IsMenuAccel( WORD kc )
{
	BOOL bFound=FALSE;

	if(Menu::IsShowingCmdBars())
	{
		CASBar *pBar=CASBar::s_pMenuBar;
		ASSERT(pBar);
		if(pBar)
		{
			for (int iMenu=0; iMenu<pBar->GetCount(); iMenu++)
			{
				// if it's not a text button, it can't have an accelerator
				UINT nStyle=pBar->GetButtonStyle(iMenu);
				if ((nStyle & TBBS_TEXTUAL)==0)
				{
					continue;
				}

				// if it's textual, it must have button extra info and a label
				TCHAR chAccel=GLOBAL_DATA::ExtractAccelerator(pBar->GetButtonExtra(iMenu)->GetLabel());
				if(chAccel)
				{
					chAccel=(TCHAR)toupper(chAccel);

					if((chAccel | KCF_ALT)==kc)
					{
						bFound=TRUE;
						break;
					}
				}
			}
		}
	}
	else
	{
		CWnd *pMainFrame=AfxGetMainWnd();
		ASSERT(pMainFrame);
		CMenu *pMenu=pMainFrame->GetMenu();
		ASSERT(pMenu);
		int nCount=pMenu->GetMenuItemCount();

		CString strLabel;

		for (int iMenu=0; iMenu<nCount; iMenu++)
		{
			if(pMenu->GetMenuString(iMenu, strLabel, MF_BYPOSITION)==0)
			{
				continue;
			}

			TCHAR chAccel=GLOBAL_DATA::ExtractAccelerator(strLabel);
			if(chAccel)
			{
				chAccel=(TCHAR)toupper(chAccel);

				if((chAccel | KCF_ALT)==kc)
				{
					bFound=TRUE;
					break;
				}
			}
		}
	}

	return bFound;
}

void CKeyCustDlg::OnChangeTap()
{
	BOOL bLegal = m_Tap.IsAccelLegal() && m_Tap.GetAccel().key;

	EnableButton(GetDlgItem(IDC_ASSIGN),bLegal);

	CString strAssignedTo;
	if (!GetCurrentAssignment(m_pKeymap, strAssignedTo) && m_iCurScope > 0)
	{
		CKeymap* pBaseKeymap = (CKeymap*)m_ScopeList.GetItemData(0);
		ASSERT(pBaseKeymap != NULL);
		GetCurrentAssignment(pBaseKeymap, strAssignedTo);
	}

	CWnd* pUsedBy = GetDlgItem(IDC_USEDBY);
	CString str;
	pUsedBy->GetWindowText(str);
	if (str != strAssignedTo)
	{
		pUsedBy->SetWindowText(strAssignedTo);

		GetDlgItem(IDC_USEDBY_CAP)->ShowWindow(strAssignedTo.IsEmpty() ? SW_HIDE : SW_SHOWNA);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CKeyCustDlg's Buttons' BN_CLICKED handlers

void CKeyCustDlg::OnClickedAssign()
{
	if (!m_Tap.IsAccelLegal())
	{
		MessageBeep(UINT(-1));
		return;
	}

	CListBox* plstCmd = (CListBox*)GetDlgItem(IDC_CMD_LIST);
	int indCmd = plstCmd->GetCurSel();
	if(indCmd < 0)
		return; // no command to bind to

	ACCEL accel = m_Tap.GetAccel();                         // pull accel key from tap,
	ACCEL chord = m_Tap.GetChord();

	accel.cmd = (WORD)plstCmd->GetItemData(indCmd); // and bind it to selected command

	WORD kc = KcFromAccel(accel);

	if (m_Tap.IsChordSet())
	{
		WORD kc2 = KcFromAccel(chord);
		
		m_pKeymap->SetKey(kc, kc2, accel.cmd);
	}
	else
	{
		m_pKeymap->SetKey(kc, accel.cmd);
	}
	
	m_pKeymap->m_bDirty = TRUE;
	
	FillKeysList(accel.cmd);
	m_Tap.ResetAll();
	OnChangeTap();

	// we just got disabled - set focus to the cat list
	CComboBox *plstCat = (CComboBox *)GetDlgItem(IDC_CAT_COMBO);
	plstCat->SetFocus();

	theApp.m_bMenuDirty = TRUE;
}

void CKeyCustDlg::OnClickedRemove()
{
	CListBox* plstKey = (CListBox*)GetDlgItem(IDC_KEY_LIST);
	int indKey = plstKey->GetCurSel();
	ASSERT(indKey >= 0);

	DWORD kcPair = plstKey->GetItemData(indKey) & ~MAKELONG(KCF_INHERITED, KCF_INHERITED);
	m_pKeymap->RemoveKey(LOWORD(kcPair), HIWORD(kcPair));
	m_pKeymap->m_bDirty = TRUE;
	
	CListBox* plstCmd = (CListBox*)GetDlgItem(IDC_CMD_LIST);
	int indCmd = plstCmd->GetCurSel();
	ASSERT(indCmd >= 0);
	FillKeysList((WORD)plstCmd->GetItemData(indCmd));
	OnChangeTap();

	if (plstKey->GetCount())
	{
		plstKey->SetFocus();
	}
	else
	{
		CComboBox *plstCat = (CComboBox *)GetDlgItem(IDC_CAT_COMBO);
		plstCat->SetFocus();
	}
	theApp.m_bMenuDirty = TRUE;
}

void ResetMainKeymap()
{
	((CMainFrame*)theApp.m_pMainWnd)->m_keymap.RemoveAll();

	POSITION pos = theApp.m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		CPackage* pPackage = (CPackage*)theApp.m_packages.GetNext(pos);
		pPackage->LoadGlobalAccelerators();
	}

	((CMainFrame*)theApp.m_pMainWnd)->m_keymap.m_bDirty = TRUE;
	theApp.m_bMenuDirty = TRUE;
}

void CKeyCustDlg::OnClickedResetAll()
{
	CString strPrompt, strEditor;

	m_ScopeList.GetWindowText( strEditor );
	AfxFormatString1( strPrompt, IDS_RESETALLKEYS, strEditor );
	if (AfxMessageBox(strPrompt, MB_YESNO, 0) == IDYES)
	{
		m_pKeymap->RemoveAll();

		if (m_iCurScope == 0)
		{
			// Global
			ResetMainKeymap();
		}
		else
		{
			// Editor specific
			m_pKeymap->LoadEditorAccelerators();
			m_pKeymap->m_bDirty = TRUE;
			theApp.m_bMenuDirty = TRUE;
		}

		// Update the dialog fields...
		OnSelChangeCmdList();
		OnChangeTap();

	}
}

/////////////////////////////////////////////////////////////////////////////
// CKeyCustDlg helper functions

int CKeyCustDlg::MaintainListSel(CListBox* pList, DWORD dwData)
{
	int count = pList->GetCount();
	for(int index = 0; index < count; index++)
	{
		DWORD dw = pList->GetItemData(index);
		if(dw == dwData)
			return index;
	}
	return -1;              // item no longer in list, so deselect all
}

class CHackFrame : public CFrameWnd
{
public:
	void SetAccelTable(HACCEL hAccelTable)
	{
		if (m_hAccelTable != NULL)
			DestroyAcceleratorTable(m_hAccelTable);
		m_hAccelTable = hAccelTable;
	}
};


DWORD CKeyCustDlg::AccelToDWord(ACCEL& accel)
{
	return (DWORD)(accel.cmd << 16) | (accel.fVirt << 8) | (BYTE)accel.key ;
}

ACCEL CKeyCustDlg::DWordToAccel(DWORD dw)
{
	ACCEL a;
	a.key   = (WORD)LOBYTE(LOWORD(dw));
	a.fVirt = (BYTE)HIBYTE(LOWORD(dw));
	a.cmd   = (WORD)HIWORD(dw);
	return a;
}


void CKeyListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	if ((lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) != 0)
	{
		BOOL fDisabled = lpDrawItemStruct->itemID >= 0 && 
			(GetItemData(lpDrawItemStruct->itemID) & MAKELONG(KCF_INHERITED, KCF_INHERITED)) != 0;

		COLORREF newTextColor = fDisabled ? RGB(0x80, 0x80, 0x80) : GetSysColor(COLOR_WINDOWTEXT);      // dark gray
		COLORREF oldTextColor = pDC->SetTextColor(newTextColor);

		COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
		COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

		// Make sure grayed text will show up...
		if (newTextColor == newBkColor)
			pDC->SetTextColor(RGB(0xC0, 0xC0, 0xC0));       // light gray
		
		if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
		{
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		}

		CString strText;
		if (lpDrawItemStruct->itemID >= 0)
			GetText(lpDrawItemStruct->itemID, strText);
		pDC->ExtTextOut(lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, 
			ETO_OPAQUE, &lpDrawItemStruct->rcItem, strText, strText.GetLength(), NULL);
		
		pDC->SetTextColor(oldTextColor);
		pDC->SetBkColor(oldBkColor);
	}
	
	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
	{
		// When the list is empty, so is rcItem...  This sucks, so we hack in a height
		// so the list box looks normal here...
		if (lpDrawItemStruct->rcItem.top == lpDrawItemStruct->rcItem.bottom)
			lpDrawItemStruct->rcItem.bottom += pDC->GetTextExtent("M", 1).cy;

		pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
	}
}

void CKeyListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetStdFont(font_Bold));
	lpMeasureItemStruct->itemHeight = dc.GetTextExtent("M", 1).cy;
	dc.SelectObject(pOldFont);
}
