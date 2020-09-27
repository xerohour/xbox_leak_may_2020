// msgfdlg.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"
#include "findtool.h"
#include "filtrdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFiltersMessageDlgTab dialog

//
// The message group table that describes the different message groups
// that a message can belong to.  This table is indexed by the message
// type constants (MT_*).  If it is changed, these indices must be
// updated as well!
//
MSGGROUP CFiltersMessageDlgTab::m_aMsgGroup[] =
{
	//
	// The first entry is the miscellaneous group.  This one will not
	// have a corresponding checkbox in the Message Filters dialog
	// because they cannot be selected as a group.
	//
	{0,				0, 0},	// MT_MISC
	{IDC_FMT_DDE,	0, 0},	// MT_DDE
	{IDC_FMT_CLIP,	0, 0},	// MT_CLIP
	{IDC_FMT_MOUSE,	0, 0},	// MT_MOUSE
	{IDC_FMT_NC,	0, 0},	// MT_NC
	{IDC_FMT_KEYBD,	0, 0},	// MT_KEYBD
	{IDC_FMT_BM,	0, 0},	// MT_BM
	{IDC_FMT_CB,	0, 0},	// MT_CB
	{IDC_FMT_EM,	0, 0},	// MT_EM
	{IDC_FMT_LB,	0, 0},	// MT_LB
	{IDC_FMT_STM,	0, 0},	// MT_STM
	{IDC_FMT_MDI,	0, 0},	// MT_MDI
	{IDC_FMT_SBM,	0, 0},	// MT_SBM
	{IDC_FMT_IME,	0, 0},	// MT_IME
	{IDC_FMT_DLG,	0, 0},	// MT_DLG
	{IDC_FMT_ANI, 	0, 0},	// MT_ANI
	{IDC_FMT_HDR, 	0, 0},	// MT_HDR
	{IDC_FMT_HK,  	0, 0},	// MT_HK
	{IDC_FMT_LV,  	0, 0},	// MT_LV
	{IDC_FMT_PROG,	0, 0},	// MT_PRG
	{IDC_FMT_STAT,	0, 0},	// MT_STB
	{IDC_FMT_TRK, 	0, 0},	// MT_TB
	{IDC_FMT_TB,  	0, 0},	// MT_TLB
	{IDC_FMT_TAB, 	0, 0},	// MT_TAB
	{IDC_FMT_TT,  	0, 0},	// MT_TT
	{IDC_FMT_TV,  	0, 0},	// MT_TV
	{IDC_FMT_UD,  	0, 0},	// MT_UD
};

int CFiltersMessageDlgTab::m_cMsgGroups = sizeof(CFiltersMessageDlgTab::m_aMsgGroup) / sizeof(MSGGROUP);
BOOL CFiltersMessageDlgTab::m_fInitializedStatic = FALSE;



CFiltersMessageDlgTab::~CFiltersMessageDlgTab()
{
	if (m_paiSelected)
	{
		delete [] m_paiSelected;
	}

	if (m_paiSelected2)
	{
		delete [] m_paiSelected2;
	}
}



BEGIN_MESSAGE_MAP(CFiltersMessageDlgTab, CDlgTab)
	//{{AFX_MSG_MAP(CFiltersMessageDlgTab)
	ON_BN_CLICKED(IDC_FMT_ALL, OnClickedMsgFiltersAll)
	ON_BN_CLICKED(IDC_FMT_NONE, OnClickedMsgFiltersNone)
	ON_BN_CLICKED(IDC_FMT_BM, OnClickedMsgFiltersBM)
	ON_BN_CLICKED(IDC_FMT_CB, OnClickedMsgFiltersCB)
	ON_BN_CLICKED(IDC_FMT_CLIP, OnClickedMsgFiltersClip)
	ON_BN_CLICKED(IDC_FMT_DDE, OnClickedMsgFiltersDDE)
	ON_BN_CLICKED(IDC_FMT_EM, OnClickedMsgFiltersEM)
	ON_BN_CLICKED(IDC_FMT_KEYBD, OnClickedMsgFiltersKeybd)
	ON_BN_CLICKED(IDC_FMT_LB, OnClickedMsgFiltersLB)
	ON_BN_CLICKED(IDC_FMT_MOUSE, OnClickedMsgFiltersMouse)
	ON_BN_CLICKED(IDC_FMT_NC, OnClickedMsgFiltersNC)
	ON_BN_CLICKED(IDC_FMT_STM, OnClickedMsgFiltersSTM)
	ON_BN_CLICKED(IDC_FMT_MDI, OnClickedMsgFiltersMDI)
	ON_BN_CLICKED(IDC_FMT_SBM, OnClickedMsgFiltersSBM)
	ON_BN_CLICKED(IDC_FMT_IME, OnClickedMsgFiltersIME)
	ON_BN_CLICKED(IDC_FMT_DLG, OnClickedMsgFiltersDlg)
	ON_BN_CLICKED(IDC_FMT_ANI, OnClickedMsgFiltersAni)
	ON_BN_CLICKED(IDC_FMT_HDR, OnClickedMsgFiltersHdr)
	ON_BN_CLICKED(IDC_FMT_HK, OnClickedMsgFiltersHK)
	ON_BN_CLICKED(IDC_FMT_LV, OnClickedMsgFiltersLV)
	ON_BN_CLICKED(IDC_FMT_PROG, OnClickedMsgFiltersProg)
	ON_BN_CLICKED(IDC_FMT_STAT, OnClickedMsgFiltersStat)
	ON_BN_CLICKED(IDC_FMT_TAB, OnClickedMsgFiltersTab)
	ON_BN_CLICKED(IDC_FMT_TB, OnClickedMsgFiltersTB)
	ON_BN_CLICKED(IDC_FMT_TRK, OnClickedMsgFiltersTrk)
	ON_BN_CLICKED(IDC_FMT_TT, OnClickedMsgFiltersTT)
	ON_BN_CLICKED(IDC_FMT_TV, OnClickedMsgFiltersTV)
	ON_BN_CLICKED(IDC_FMT_UD, OnClickedMsgFiltersUD)
	ON_LBN_SELCHANGE(IDC_FMT_LIST, OnSelchangeMsgFiltersList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//-----------------------------------------------------------------------------
// CFiltersMessageDlgTab::InitStaticData
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

void CFiltersMessageDlgTab::InitStaticData()
{
	INT i;

	//
	// Calculate the counts in the message groups.  This is best
	// done at run time to be safe.
	//
	for (i = 0; i < CMsgDoc::m_cMsgs; i++)
	{
		//
		// Increment the total for the group that this message
		// belongs to.
		//
		m_aMsgGroup[CMsgDoc::m_aMsgDesc[i].iMsgType].cMsgs++;
	}

	m_fInitializedStatic = TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CFiltersMessageDlgTab message handlers

BOOL CFiltersMessageDlgTab::OnInitDialog()
{
	int i;
	int iSel;
	PMSGDESC pmd;
	CListBox* pMsgsListBox = (CListBox*)GetDlgItem(IDC_FMT_LIST);

	CDlgTab::OnInitDialog();

	BeginWaitCursor();

	if (!m_fInitializedStatic)
	{
		InitStaticData();
	}

	CFont* pFont = GetStdFont(font_Normal);
	ASSERT(pFont);
	HFONT hfont = (HFONT)pFont->m_hObject;

	SendDlgItemMessage(IDC_FMT_LIST, WM_SETFONT, (WPARAM)hfont, FALSE);

	m_pDoc = ((CFiltersTabbedDialog*)GetParent())->m_pDoc;

	//
	// Allocate arrays of integers large enough to hold the indexes
	// of all the messages in the message table.
	//
	m_paiSelected = new int[CMsgDoc::m_cMsgs];
	m_paiSelected2 = new int[CMsgDoc::m_cMsgs];

	for (i = 0; i < m_cMsgGroups; i++)
	{
		m_aMsgGroup[i].cUseCount = 0;
	}

	for (i = 0, pmd = CMsgDoc::m_aMsgDesc; i < CMsgDoc::m_cMsgs; i++, pmd++)
	{
		iSel = pMsgsListBox->AddString(pmd->pszMsg);
		pMsgsListBox->SetItemDataPtr(iSel, pmd);

		if (pmd->msg < WM_USER)
		{
			if (m_pDoc->m_afSelected[pmd->msg])
			{
				pMsgsListBox->SetSel(iSel);
				m_aMsgGroup[pmd->iMsgType].cUseCount++;
			}
		}
		else
		{
			switch(pmd->iMsgType)
			{
				case MT_DLG:

					if (m_pDoc->m_afDlgSelected[pmd->msg - WM_USER])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;
#ifndef DISABLE_WIN95_MESSAGES
				case MT_ANI:

					if (m_pDoc->m_afAniSelected[pmd->msg - (WM_USER + 100)])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_HDR:

					if (m_pDoc->m_afHdrSelected[pmd->msg - HDM_FIRST])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_HK:

					if (m_pDoc->m_afHKSelected[pmd->msg - (WM_USER + 1)])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_LV:

					if (m_pDoc->m_afLVSelected[pmd->msg - LVM_FIRST])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_PRG:

					if (m_pDoc->m_afProgSelected[pmd->msg - (WM_USER + 1)])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_STB:

					if (m_pDoc->m_afStatSelected[pmd->msg - (WM_USER + 1)])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_TB:

					if (m_pDoc->m_afTrkSelected[pmd->msg - WM_USER])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_TLB:

					if (m_pDoc->m_afTBSelected[pmd->msg - (WM_USER + 1)])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_TAB:

					if (m_pDoc->m_afTabSelected[pmd->msg - TCM_FIRST])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_TT:

					if (m_pDoc->m_afTTSelected[pmd->msg - (WM_USER + 1)])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_TV:

					if (m_pDoc->m_afTVSelected[pmd->msg - TV_FIRST])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;

				case MT_UD:

					if (m_pDoc->m_afUpDnSelected[pmd->msg - (WM_USER + 101)])
					{
						pMsgsListBox->SetSel(iSel);
						m_aMsgGroup[pmd->iMsgType].cUseCount++;
					}
					break;
#endif	// DISABLE_WIN95_MESSAGES
			}
		}
	}

	//
	// Set the selection rectangle to the first item in the listbox.
	//
	pMsgsListBox->SetCaretIndex(0);

	//
	// Loop through all the message groups.
	//
	for (i = 0; i < m_cMsgGroups; i++)
	{
		//
		// Is at least one message in the group selected, and is
		// there a checkbox for the group?
		//
		if (m_aMsgGroup[i].cUseCount && m_aMsgGroup[i].idCheckBox)
		{
			//
			// Check the corresponding checkbox.  If all messages
			// in the group are selected, the checkbox is checked.
			// If only some are selected, the checkbox is set to
			// grayed (3-state).
			//
			CheckDlgButton(m_aMsgGroup[i].idCheckBox, (m_aMsgGroup[i].cUseCount == m_aMsgGroup[i].cMsgs) ? 1 : 2);
		}
	}

	if (m_pDoc->m_fMsgsUser)
	{
		CheckDlgButton(IDC_FMT_USER, 1);
	}

	if (m_pDoc->m_fMsgsRegistered)
	{
		CheckDlgButton(IDC_FMT_REGISTERED, 1);
	}

	if (m_pDoc->m_fMsgsUndocumented)
	{
		CheckDlgButton(IDC_FMT_UNDOCUMENTED, 1);
	}

	m_cItemsSave = pMsgsListBox->GetSelItems(CMsgDoc::m_cMsgs, m_paiSelected);

	EndWaitCursor();

	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CFiltersMessageDlgTab::OnClickedMsgFiltersAll()
{
	int i;
	CListBox* pMsgsListBox = (CListBox*)GetDlgItem(IDC_FMT_LIST);

	//
	// Select all lines in the listbox.
	//
	pMsgsListBox->SetSel(-1, TRUE);

	for (i = 0; i < m_cMsgGroups; i++)
	{
		m_aMsgGroup[i].cUseCount = m_aMsgGroup[i].cMsgs;
		if (m_aMsgGroup[i].idCheckBox)
		{
			CheckDlgButton(m_aMsgGroup[i].idCheckBox, 1);
		}
	}

	m_cItemsSave = pMsgsListBox->GetSelItems(CMsgDoc::m_cMsgs, m_paiSelected);

	CheckDlgButton(IDC_FMT_USER, 1);
	CheckDlgButton(IDC_FMT_REGISTERED, 1);
	CheckDlgButton(IDC_FMT_UNDOCUMENTED, 1);
}



void CFiltersMessageDlgTab::OnClickedMsgFiltersNone()
{
	int i;
	CListBox* pMsgsListBox = (CListBox*)GetDlgItem(IDC_FMT_LIST);

	//
	// Unselect all lines in the listbox.
	//
	pMsgsListBox->SetSel(-1, FALSE);

	for (i = 0; i < m_cMsgGroups; i++)
	{
		m_aMsgGroup[i].cUseCount = 0;
		if (m_aMsgGroup[i].idCheckBox)
		{
			CheckDlgButton(m_aMsgGroup[i].idCheckBox, 0);
		}
	}

	m_cItemsSave = 0;

	CheckDlgButton(IDC_FMT_USER, 0);
	CheckDlgButton(IDC_FMT_REGISTERED, 0);
	CheckDlgButton(IDC_FMT_UNDOCUMENTED, 0);
}



void CFiltersMessageDlgTab::OnClickedMsgFiltersBM()
{
	OnClickedGroup(MT_BM);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersCB()
{
	OnClickedGroup(MT_CB);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersClip()
{
	OnClickedGroup(MT_CLIP);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersDDE()
{
	OnClickedGroup(MT_DDE);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersEM()
{
	OnClickedGroup(MT_EM);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersKeybd()
{
	OnClickedGroup(MT_KEYBD);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersLB()
{
	OnClickedGroup(MT_LB);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersMouse()
{
	OnClickedGroup(MT_MOUSE);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersNC()
{
	OnClickedGroup(MT_NC);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersSTM()
{
	OnClickedGroup(MT_STM);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersMDI()
{
	OnClickedGroup(MT_MDI);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersSBM()
{
	OnClickedGroup(MT_SBM);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersIME()
{
	OnClickedGroup(MT_IME);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersDlg() 
{
	OnClickedGroup(MT_DLG);
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersAni() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_ANI);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersHdr() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_HDR);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersHK() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_HK);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersLV() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_LV);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersProg() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_PRG);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersStat() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_STB);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersTab() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_TAB);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersTB() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_TLB);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersTrk() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_TB);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersTT() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_TT);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersTV() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_TV);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedMsgFiltersUD() 
{
#ifndef DISABLE_WIN95_MESSAGES
	OnClickedGroup(MT_UD);
#endif	// DISABLE_WIN95_MESSAGES
}

void CFiltersMessageDlgTab::OnClickedGroup(int iGroup)
{
	int i;
	BOOL fChecked;
	int iTopIndex;
	int cItems;
	int iSel;
	BOOL fSel;
	PMSGGROUP pmg;
	PMSGDESC pmd;
	CListBox* pMsgsListBox = (CListBox*)GetDlgItem(IDC_FMT_LIST);

	ASSERT(iGroup < m_cMsgGroups);
	pmg = &m_aMsgGroup[iGroup];

	fChecked = IsDlgButtonChecked(pmg->idCheckBox);
	if (fChecked == 1)
		fChecked = FALSE;
	else
		fChecked = TRUE;

	if (fChecked)
	{
		pMsgsListBox->SetRedraw(FALSE);
		iTopIndex = pMsgsListBox->GetTopIndex();
	}

	//
	// Get the list of currently selected items.
	//
	cItems = pMsgsListBox->GetSelItems(CMsgDoc::m_cMsgs, m_paiSelected);

	//
	// Look for all the messages in this group.
	//
	for (i = 0, iSel = 0; i < CMsgDoc::m_cMsgs; i++)
	{
		pmd = (PMSGDESC)pMsgsListBox->GetItemDataPtr(i);
		ASSERT(pmd);
		if (pmd->iMsgType == iGroup)
		{
			//
			// Bump up through the list of selected items, looking
			// to see if this item is currently selected.
			//
			for (fSel = FALSE; iSel < cItems && m_paiSelected[iSel] <= i; iSel++)
			{
				//
				// A match was found.  The item is selected.
				//
				if (m_paiSelected[iSel] == i)
				{
					fSel = TRUE;
					break;
				}
			}

			//
			// Is the current selection state of the item
			// different from the desired selection state?
			//
			if (fSel != fChecked)
			{
				//
				// Update the use count of the group that contains
				// this message.
				//
				m_aMsgGroup[pmd->iMsgType].cUseCount += fChecked ? 1 : -1;

				//
				// Select/deselect the message in the list box.
				//
				pMsgsListBox->SetSel(i, fChecked);
			}
		}
	}

	//
	// Be sure that the checkboxes reflect the updated
	// status of the message group use counts.
	//
	UpdateCheckBoxes();

	if (fChecked)
	{
		pMsgsListBox->SetTopIndex(iTopIndex);
		pMsgsListBox->SetRedraw();
		pMsgsListBox->Invalidate(FALSE);
	}

	m_cItemsSave = pMsgsListBox->GetSelItems(CMsgDoc::m_cMsgs, m_paiSelected);
}



void CFiltersMessageDlgTab::OnSelchangeMsgFiltersList()
{
	int i;
	int iSel;
	int cItems;
	int cSelItemsMax;
	BOOL fSel;
	PMSGDESC pmd;
	CListBox* pMsgsListBox = (CListBox*)GetDlgItem(IDC_FMT_LIST);

	cItems = pMsgsListBox->GetSelItems(CMsgDoc::m_cMsgs, m_paiSelected2);
	if (cItems == m_cItemsSave)
	{
		//
		// Nothing changed except for the selection
		// rectangle moving.  We are done.
		//
		return;
	}

	if (cItems > m_cItemsSave)
	{
		//
		// A message was selected.  Look for it.
		//
		for (i = 0; i < m_cItemsSave &&	m_paiSelected[i] == m_paiSelected2[i]; i++);

		iSel = m_paiSelected2[i];
		fSel = TRUE;
	}
	else
	{
		//
		// A message was unselected.  Look for it.
		//
		for (i = 0; i < cItems && m_paiSelected[i] == m_paiSelected2[i]; i++);

		iSel = m_paiSelected[i];
		fSel = FALSE;
	}

	//
	// Get the currently selected item.  It was either
	// just turned on or off.
	//
	pmd = (PMSGDESC)pMsgsListBox->GetItemDataPtr(iSel);

	//
	// Update the use count for the group that this
	// message belongs to.
	//
	m_aMsgGroup[pmd->iMsgType].cUseCount += fSel ? 1 : -1;

	//
	// Be sure that the checkboxes reflect the updated
	// status of the message group use counts.
	//
	UpdateCheckBoxes();

	//
	// Save away the new selected item array.
	//
	cSelItemsMax = max(cItems, m_cItemsSave);
	for (i = 0; i < cSelItemsMax; i++)
	{
		m_paiSelected[i] = m_paiSelected2[i];
	}

	m_cItemsSave = cItems;
}


void CFiltersMessageDlgTab::UpdateCheckBoxes()
{
	int i;
	int fState;

	for (i = 0; i < m_cMsgGroups; i++)
	{
		if (m_aMsgGroup[i].idCheckBox)
		{
			if (m_aMsgGroup[i].cUseCount == m_aMsgGroup[i].cMsgs)
				fState = 1;
			else if (m_aMsgGroup[i].cUseCount == 0)
				fState = 0;
			else
				fState = 2;

			CheckDlgButton(m_aMsgGroup[i].idCheckBox, fState);
		}
	}
}



void CFiltersMessageDlgTab::CommitTab()
{
	int cItems;
	int i;
	PMSGDESC pmd;
	CListBox* pMsgsListBox = (CListBox*)GetDlgItem(IDC_FMT_LIST);

	cItems = pMsgsListBox->GetSelItems(CMsgDoc::m_cMsgs, m_paiSelected);

	//
	// Unselect all messages first.
	//
	for (i = 0; i < CMsgDoc::m_cMsgs; i++)
	{
		if (CMsgDoc::m_aMsgDesc[i].msg < WM_USER)
			m_pDoc->m_afSelected[CMsgDoc::m_aMsgDesc[i].msg] = FALSE;
		else
		{
			switch(CMsgDoc::m_aMsgDesc[i].iMsgType)
			{
				case MT_DLG:

					m_pDoc->m_afDlgSelected[CMsgDoc::m_aMsgDesc[i].msg - WM_USER] = FALSE;
					break;
#ifndef DISABLE_WIN95_MESSAGES
				case MT_ANI:

					m_pDoc->m_afAniSelected[CMsgDoc::m_aMsgDesc[i].msg - (WM_USER + 100)] = FALSE;
					break;

				case MT_HDR:

					m_pDoc->m_afHdrSelected[CMsgDoc::m_aMsgDesc[i].msg - HDM_FIRST] = FALSE;
					break;

				case MT_HK:

					m_pDoc->m_afHKSelected[CMsgDoc::m_aMsgDesc[i].msg - (WM_USER + 1)] = FALSE;
					break;

				case MT_LV:

					m_pDoc->m_afLVSelected[CMsgDoc::m_aMsgDesc[i].msg - LVM_FIRST] = FALSE;
					break;

				case MT_PRG:

					m_pDoc->m_afProgSelected[CMsgDoc::m_aMsgDesc[i].msg - (WM_USER + 1)] = FALSE;
					break;

				case MT_STB:

					m_pDoc->m_afStatSelected[CMsgDoc::m_aMsgDesc[i].msg - (WM_USER + 1)] = FALSE;
					break;

				case MT_TB:

					m_pDoc->m_afTrkSelected[CMsgDoc::m_aMsgDesc[i].msg - WM_USER] = FALSE;
					break;

				case MT_TLB:

					m_pDoc->m_afTBSelected[CMsgDoc::m_aMsgDesc[i].msg - (WM_USER + 1)] = FALSE;
					break;

				case MT_TAB:

					m_pDoc->m_afTabSelected[CMsgDoc::m_aMsgDesc[i].msg - TCM_FIRST] = FALSE;
					break;

				case MT_TT:

					m_pDoc->m_afTTSelected[CMsgDoc::m_aMsgDesc[i].msg - (WM_USER + 1)] = FALSE;
					break;

				case MT_TV:

					m_pDoc->m_afTVSelected[CMsgDoc::m_aMsgDesc[i].msg - TV_FIRST] = FALSE;
					break;

				case MT_UD:

					m_pDoc->m_afUpDnSelected[CMsgDoc::m_aMsgDesc[i].msg - (WM_USER + 101)] = FALSE;
					break;
#endif	// DISABLE_WIN95_MESSAGES
			}
		}
	}

	//
	// Mark all the messages that are selected.
	//
	for (i = 0; i < cItems; i++)
	{
		pmd = (PMSGDESC)pMsgsListBox->GetItemDataPtr(m_paiSelected[i]);
		ASSERT(pmd);
		if (pmd->msg < WM_USER)
			m_pDoc->m_afSelected[pmd->msg] = TRUE;
		else
		{
			switch(pmd->iMsgType)
			{
				case MT_DLG:

					m_pDoc->m_afDlgSelected[pmd->msg - WM_USER] = TRUE;
					break;
#ifndef DISABLE_WIN95_MESSAGES
				case MT_ANI:

					m_pDoc->m_afAniSelected[pmd->msg - (WM_USER + 100)] = TRUE;
					break;

				case MT_HDR:

					m_pDoc->m_afHdrSelected[pmd->msg - HDM_FIRST] = TRUE;
					break;

				case MT_HK:

					m_pDoc->m_afHKSelected[pmd->msg - (WM_USER + 1)] = TRUE;
					break;

				case MT_LV:

					m_pDoc->m_afLVSelected[pmd->msg - LVM_FIRST] = TRUE;
					break;

				case MT_PRG:

					m_pDoc->m_afProgSelected[pmd->msg - (WM_USER + 1)] = TRUE;
					break;

				case MT_STB:

					m_pDoc->m_afStatSelected[pmd->msg - (WM_USER + 1)] = TRUE;
					break;

				case MT_TB:

					m_pDoc->m_afTrkSelected[pmd->msg - WM_USER] = TRUE;
					break;

				case MT_TLB:

					m_pDoc->m_afTBSelected[pmd->msg - (WM_USER + 1)] = TRUE;
					break;

				case MT_TAB:

					m_pDoc->m_afTabSelected[pmd->msg - TCM_FIRST] = TRUE;
					break;

				case MT_TT:

					m_pDoc->m_afTTSelected[pmd->msg - (WM_USER + 1)] = TRUE;
					break;

				case MT_TV:

					m_pDoc->m_afTVSelected[pmd->msg - TV_FIRST] = TRUE;
					break;

				case MT_UD:

					m_pDoc->m_afUpDnSelected[pmd->msg - (WM_USER + 101)] = TRUE;
					break;
#endif	// DISABLE_WIN95_MESSAGES
			}
		}
	}

	m_pDoc->m_fMsgsUser = IsDlgButtonChecked(IDC_FMT_USER);
	m_pDoc->m_fMsgsRegistered = IsDlgButtonChecked(IDC_FMT_REGISTERED);
	m_pDoc->m_fMsgsUndocumented = IsDlgButtonChecked(IDC_FMT_UNDOCUMENTED);

	//
	// If they have selected "Save as Default", copy the
	// current settings to the default settings.
	//
	if (IsDlgButtonChecked(IDC_FMT_SAVE))
	{
		CMsgDoc::m_fMsgsUserDef = m_pDoc->m_fMsgsUser;
		CMsgDoc::m_fMsgsRegisteredDef = m_pDoc->m_fMsgsRegistered;
		CMsgDoc::m_fMsgsUndocumentedDef = m_pDoc->m_fMsgsUndocumented;

		memcpy(m_pDoc->m_afSelectedDef,     m_pDoc->m_afSelected,     MAX_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afDlgSelectedDef,  m_pDoc->m_afDlgSelected,  MAX_DLG_MESSAGES * sizeof(BYTE));
#ifndef DISABLE_WIN95_MESSAGES
		memcpy(m_pDoc->m_afAniSelectedDef,  m_pDoc->m_afAniSelected,  MAX_ANI_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afHdrSelectedDef,  m_pDoc->m_afHdrSelected,  MAX_HDR_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afTBSelectedDef,   m_pDoc->m_afTBSelected,   MAX_TB_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afTTSelectedDef,   m_pDoc->m_afTTSelected,   MAX_TT_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afStatSelectedDef, m_pDoc->m_afStatSelected, MAX_STAT_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afTrkSelectedDef,  m_pDoc->m_afTrkSelected,  MAX_TRK_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afUpDnSelectedDef, m_pDoc->m_afUpDnSelected, MAX_UD_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afProgSelectedDef, m_pDoc->m_afProgSelected, MAX_PROG_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afHKSelectedDef,   m_pDoc->m_afHKSelected,   MAX_HK_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afLVSelectedDef,   m_pDoc->m_afLVSelected,   MAX_LV_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afTVSelectedDef,   m_pDoc->m_afTVSelected,   MAX_TV_MESSAGES * sizeof(BYTE));
		memcpy(m_pDoc->m_afTabSelectedDef,  m_pDoc->m_afTabSelected,  MAX_TAB_MESSAGES * sizeof(BYTE));
#endif	// DISABLE_WIN95_MESSAGES
	}

	CDlgTab::CommitTab();
}
