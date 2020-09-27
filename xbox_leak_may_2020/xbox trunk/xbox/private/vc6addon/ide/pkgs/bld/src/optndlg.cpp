//
// CProjOptionDlg
//									

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "optndlg.h"	// our local header
#include "prjconfg.h"	// CConfigEnum
#include "optnui.h"		// COptionPageTab
#include "toolsdlg.h"	// the 'Custom Build' page
#include "projpage.h"	// the Tool 'General' page
#include "exttarg.h"	// the External target type
#include "depgraph.h"	// dep. graph
#include "javadbg.h"	// Java debugger support
#include "bldslob.h"
#include "bldnode.h"
#include "vwslob.h"
#include "vproj.hid"

#include <prjapi.h>
#include <prjguid.h>

IMPLEMENT_DYNCREATE(COptionTreeCtl, CTreeCtl)
IMPLEMENT_DYNAMIC(CProjOptionsDlg, CTabbedDialog)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CProjComponentMgr g_prjcompmgr;

// these are our 'pseudo' option handlers
// they can be put in the option handler list (m_listHdlrs)
// so that they are added as tabs to the 'mondo' dialog
// (ie. this is a hack so we can treat non-tool option pages
// just like the regular tool option pages returned from
// each handler)
#define OPTHDLR_TOOL_GENERAL		(COptionHandler *)0x0001
#define OPTHDLR_DEBUG				(COptionHandler *)0x0002
#define OPTHDLR_EXT_OPTS			(COptionHandler *)0x0003
#define OPTHDLR_EXTTARG_OPTS		(COptionHandler *)0x0004
#define OPTHDLR_CUSTOM_BUILD		(COptionHandler *)0x0005
#define OPTHDLR_PRELINK				(COptionHandler *)0x0006
#define OPTHDLR_DEBUG_JAVA			(COptionHandler *)0x0007
#define OPTHDLR_JAVA_TOOL_GENERAL	(COptionHandler *)0x0008
#define OPTHDLR_POSTBUILD			(COptionHandler *)0x0009

#define NEWCFG 1
#define MPROJ 1
void COptionTreeStore::Clear()
{
	POSITION pos = m_lstStates.GetHeadPosition();
	while (pos != (POSITION)NULL)
		delete (CNodeState *) m_lstStates.GetNext(pos);

	m_lstStates.RemoveAll();
}

BOOL COptionTreeStore::Serialize(CArchive & archive)
{
	TRY
	{
		// write out the # of nodes in the store
		int nCount = m_lstStates.GetCount();
		archive << ((WORD)nCount);

		POSITION pos = m_lstStates.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CNodeState * pNodeState = (CNodeState *)m_lstStates.GetNext(pos);

			// write out the node
			archive << pNodeState->m_str;
			archive << ((WORD)pNodeState->m_nLevel);
			archive << ((BYTE)pNodeState->m_fExpanded);
			archive << ((BYTE)pNodeState->m_fSelected);
		}
	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;	// succeed
}

BOOL COptionTreeStore::Deserialize(CArchive & archive)
{
	// clear what we currently have
	Clear();

	CNodeState * pNodeState = NULL;
	TRY
	{
		// read in the # of nodes in the archived store
		int nCount = 0;
		archive >> ((WORD &)nCount);
		while (nCount > 0)
		{
			nCount--;	// next node from the archive
			pNodeState = new CNodeState;
			if (pNodeState == (CNodeState *)NULL)
				continue;	// don't abort, just ignore
		
			// read in the node
			archive >> pNodeState->m_str;
			pNodeState->m_nLevel = 0;
			archive >> ((WORD &)pNodeState->m_nLevel);
			pNodeState->m_fExpanded = pNodeState->m_fSelected = FALSE;
			archive >> ((BYTE &)pNodeState->m_fExpanded);
			archive >> ((BYTE &)pNodeState->m_fSelected);
			m_lstStates.AddTail(pNodeState);
		}
	}
	CATCH_ALL (e)
	{
		if (pNodeState)
			delete pNodeState;

		// failed
		return FALSE;
	}
 	END_CATCH_ALL

	return TRUE;	// succeed
}

BEGIN_MESSAGE_MAP(COptionTreeCtl, CTreeCtl)
	//{{AFX_MSG_MAP(COptionTreeCtl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_VKEYTOITEM()
	//}}AFX_MSG_MAP
	ON_LBN_SETFOCUS(IDCW_TREELISTBOX, OnLbSetFocus)
	ON_LBN_KILLFOCUS(IDCW_TREELISTBOX, OnLbKillFocus)
	ON_LBN_SELCHANGE(IDCW_TREELISTBOX, OnLbSelChange)
END_MESSAGE_MAP()

COptionTreeStore COptionTreeCtl::sm_TreeState;	// our 'remembered' tree state

BOOL COptionTreeCtl::Create(CWnd *pParentWnd, CRect rect)
{
	if (!CWnd::Create(
			NULL,
			NULL,
			WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_CLIPSIBLINGS,
			rect,
			pParentWnd,
			IDC_OPTN_TREECTL,
			NULL
		)) 	return FALSE;
	return TRUE;
}

int COptionTreeCtl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// call our create on the base-class
	if (CTreeCtl::OnCreate(lpCreateStruct) == -1)
	{
		return -1;	// failed to create our tree control
	}

	m_ptrTrgNodes.RemoveAll();

	return 0;	// succeded in creating our option tree control
}

void COptionTreeCtl::OnDestroy()
{
	// remember the state of the tree-control?
	if (m_fDoStateStore)	SaveTreeState();

	// do this before we free all of the tree data (the m_ProxySlob refers to tree data)
	((CProjOptionsDlg *)GetParent())->m_ProxySlob.Clear(FALSE);

	// free-up the tree data
	FreeAllOptTreeNodes();

	CTreeCtl::OnDestroy();
}

void COptionTreeCtl::OnLbSetFocus()
{
	CTreeCtl::InvalSelection();

	// remove default style bit from default button in parent dialog
	// (Project.Settings)
	CProjOptionsDlg * pOptDlg = (CProjOptionsDlg *)GetParent();
	pOptDlg->SetDefButtonIndex(-1/*none*/);
}

void COptionTreeCtl::OnLbKillFocus()
{
	CTreeCtl::InvalSelection();

	// add default style bit to default button in parent dialog
	// (Project.Settings)
	CProjOptionsDlg * pOptDlg = (CProjOptionsDlg *)GetParent();
	pOptDlg->SetDefButtonIndex(0);
}

BOOL COptionTreeCtl::PreTranslateMessage(MSG* pMsg)
{
	// expand or contract?
	if (pMsg->message == WM_KEYDOWN && (char)pMsg->wParam == VK_RETURN)
	{
		OnPrimary();
		return TRUE;
	}

	return CTreeCtl::PreTranslateMessage(pMsg);
}

int COptionTreeCtl::InsertTargetNode(CProject * pProject, const TCHAR * szTarget)
{
	// construct our new OptTreeNode to insert
	// we'll destroy this in the virtual COptionTreeCtl::DeleteNode()
#ifdef NEWCFG
	int nValidConfigs = 0;
	OptTreeNodeList * pNewOptNodeList = new OptTreeNodeList;
	CString strTarget = pProject->GetTargetName();
	// ASSERT(strTarget.CompareNoCase(szTarget)==0);
	ConfigurationRecord * pcr;
	const CPtrArray * pCfgProject = pProject->GetConfigArray();
	int size = pCfgProject->GetSize();
	for (int i = 0; i < size; i++)
	{
		pcr = (ConfigurationRecord *)pCfgProject->GetAt(i);
		OptTreeNode * pNewOptNode = new OptTreeNode;
		pNewOptNode->pItem = pProject;	// root project item
		pNewOptNode->pcr = pcr;
		ASSERT(pcr != NULL);
		pNewOptNodeList->AddHead(pNewOptNode);
		if (pNewOptNode->IsValid())
			nValidConfigs++;
	}

	// Only add this Target if it has at least one valid config
	if (nValidConfigs==0)
	{
		pProject->SetOptionTreeCtl(NULL);
		pNewOptNodeList->RemoveContent();
		delete pNewOptNodeList;
		return LB_ERR;
	}
#else
	OptTreeNode * pNewOptNode = new OptTreeNode;
	pNewOptNode->pItem = pProject;	// root project item
	pNewOptNode->pcr = pProject->ConfigRecordFromConfigName(szTarget);	// our base config. for project
	ASSERT(pNewOptNode->pcr);
#endif

	// inform the project notify slob that our project 'view'
	// wants to be informed of project changes
	pProject->SetOptionTreeCtl(this);

	// insert this as a top-level node (un-expanded)
	CNode * pNodeDummy; int iNodeInserted;
#ifdef NEWCFG
	if (!InsertNode(NULL, -1, szTarget, (DWORD)pNewOptNodeList, pNodeDummy, iNodeInserted))
	{
		pNewOptNodeList->RemoveContent();
		delete pNewOptNodeList;
		return LB_ERR;
	}
#else
	if (!InsertNode(NULL, -1, szTarget, (DWORD)pNewOptNode, pNodeDummy, iNodeInserted))
	{
		delete pNewOptNode;
		return LB_ERR;
	}
#endif

	// remember this one
#ifdef NEWCFG
	m_ptrTrgNodes.AddTail((void *)pNewOptNodeList);
#else
	m_ptrTrgNodes.AddTail(pNewOptNode);
#endif

	return iNodeInserted;
}

void COptionTreeCtl::FreeTargetNode(OptTreeNode * pTargetNode)
{
	ASSERT(0);
	// delete the target, this will delete all of the children
	delete pTargetNode;

	// delete this one in our 'remembered' list
	POSITION pos = m_ptrTrgNodes.Find(pTargetNode);
	ASSERT(pos != (POSITION)NULL);
	m_ptrTrgNodes.RemoveAt(pos);
}

void COptionTreeCtl::FreeAllOptTreeNodes()
{
	// free each target that we remembered that we inserted
	while (!m_ptrTrgNodes.IsEmpty())
	{
		OptTreeNodeList * pOptNodeList = (OptTreeNodeList *) m_ptrTrgNodes.RemoveHead();
		pOptNodeList->RemoveContent();
		delete pOptNodeList;
	}
}

int COptionTreeCtl::FindTargetNode(const CString& strTargetName)
{
#ifdef NEWCFG
	int iIndex = 0;
	CNode* pNode;
	while ((pNode = GetNode(iIndex)) != (CNode *)NULL)
 	{
 		OptTreeNodeList * pOptTreeNodeList = (OptTreeNodeList *)pNode->m_dwData;
		CProjItem* pItem = pOptTreeNodeList->GetItem();
		if (pItem->IsKindOf(RUNTIME_CLASS(CProject)) &&
		    pItem->GetTargetName() == strTargetName)
			return iIndex;
		iIndex++;
	}
#else
	ASSERT(0);  // NYI
	ConfigurationRecord* pcr = m_pProject->ConfigRecordFromConfigName(strTargetName);

	int iIndex = 0;
	CNode* pNode;
	while ((pNode = GetNode(iIndex)) != (CNode *)NULL)
 	{
 		OptTreeNode * pOptTreeNode = (OptTreeNode *)pNode->m_dwData;
		CProjItem* pItem = pOptTreeNode->pItem;
		if (pOptTreeNode->pItem->IsKindOf(RUNTIME_CLASS(CProject)) &&
		    pOptTreeNode->pcr == pcr)
			return iIndex;
		iIndex++;
	}
#endif

	return LB_ERR;
}

void COptionTreeCtl::RefreshTargetNodes()
{
	// do this before we free all of the tree data (the m_ProxySlob refers to tree data)
	((CProjOptionsDlg *)GetParent())->m_ProxySlob.Clear(FALSE);

	// free-up the old option tree control nodes
	FreeAllOptTreeNodes();
	FreeAllTreeData();

		CProject * pProject = m_pProject;
		CString strTarget = pProject->GetTargetName();
#ifdef MPROJ
	// support multiple projects
	const CObList * pProjList = CProject::GetProjectList();
	POSITION pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif

#ifdef NEWCFG
			CString strTarget = pProject->GetTargetName();
			(void) InsertTargetNode(pProject, strTarget);
#else
		POSITION pos = g_theConfigEnum.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CEnumerator	* pEnum = g_theConfigEnum.GetNext(pos);
			if (pEnum->val)	// supported project type?
			{
				if (pProject->ConfigRecordFromConfigName(pEnum->szId, FALSE) != NULL)
					(void) InsertTargetNode(pEnum->szId);
			}
		}
#endif
#ifdef MPROJ
	}
#endif
}

void COptionTreeCtl::GetTreeCtlSelection(CTreeCtl * pTreeCtl, INT * & rgInt, int & nSelItems)
{
	if ((pTreeCtl->GetListBox()->GetStyle() & LBS_EXTENDEDSEL) != 0)
	{
		// multiple-select case

		nSelItems = pTreeCtl->GetSelCount();
		if (nSelItems != 0)
		{
			rgInt = new INT[nSelItems];
			// remember multiple-select
			if (rgInt != (INT *)NULL && pTreeCtl->GetSelItems(nSelItems, rgInt) == LB_ERR)
				nSelItems = 0;	// no selection
		}
	}
	else
	{
		// single-select case

		nSelItems = 1;	// single selection
		rgInt = new INT[nSelItems];	// single selection

		int iSel;
		// if we have a selection then select it
		if (rgInt != (INT *)NULL && (iSel = pTreeCtl->GetCurSel()) == LB_ERR)
			nSelItems = 0;	// no selection
		else
			rgInt[0] = iSel;	// remember single-select
	}
}

void COptionTreeCtl::CopyTreeCtlState(CSlob * pSelection)
{
	int iIndexTarget = LB_ERR;

	if (pSelection != NULL)
	{
		CBuildNode* pBldNode = GetBuildNode();
		CBuildViewSlob * pTargetSlob;
		pBldNode->InitBldNodeEnum();
		while (pBldNode->NextBldNodeEnum((void **)&pTargetSlob))
		{

			ASSERT(pTargetSlob->IsKindOf(RUNTIME_CLASS(CBuildViewSlob)));
			CProjItem * pProjItem = (CProjItem *)pTargetSlob->GetBuildSlob()->GetProjItem();
			ASSERT((pProjItem->IsKindOf(RUNTIME_CLASS(CProject))) ||
				(pProjItem->IsKindOf(RUNTIME_CLASS(CTargetItem))));
			CString strTargetName = pProjItem->GetTargetName();
			iIndexTarget = FindTargetNode(strTargetName);

			// do we have a project tree-control whose state we want to copy?
			if (iIndexTarget == LB_ERR)
				continue;

			UINT fExpandedState = pTargetSlob->GetExpandedState(TRUE /* ignore deps */);
			// Expand node to match BuildView slob
			int iIndex;
			for (iIndex = iIndexTarget; fExpandedState != 0; iIndex++)
			{
				CNode * pNode = GetNode(iIndex);
				if (pNode == NULL) break;

				if (NodeIsExpandable(pNode))
				{
					if ((fExpandedState & 1) != 0)
					{
						Expand(iIndex);	// make sure we have the targ. offset
					}
					fExpandedState >>= 1;
				}
			}
		}
				
		// copy the selection state
		CString strDummy;
		int nSelection = 0;
		POSITION pos1 = pSelection->GetContentList()->GetHeadPosition();
		while (pos1 != NULL)
		{
			CProjItem * pProjItem = (CProjItem *)pSelection->GetContentList()->GetNext(pos1);
			// is this item valid?
			if ((!pProjItem->IsKindOf(RUNTIME_CLASS(CProjItem))) || (!IsValidOptTreeItem(pProjItem, strDummy)))
				continue;
			
			CNode * pNode = (CNode *)NULL;
			int iNode = 0;
			while ((pNode = GetNode(iNode)) != (CNode *)NULL)
			{
#ifdef NEWCFG
				OptTreeNodeList * pOptNodeList = (OptTreeNodeList *)pNode->m_dwData;
				// is this the one?
				if (pOptNodeList->GetItem() == (CProjItem *)pProjItem)
					break;	// yes
#else
				OptTreeNode * pOptNode = (OptTreeNode *)pNode->m_dwData;
				// is this the one?
				if (pOptNode->pcr == pcr && pOptNode->pItem == (CProjItem *)pProjItem)
					break;	// yes
#endif
				iNode++;	// next node to look
			}

			// found it?
			if (pNode != (CNode *)NULL)
			{
				ASSERT(iNode != LB_ERR);
				SetSel(iNode);
			}
		}
		m_fDoStateStore = FALSE;
	}
	else
	{
		// retrieve a selection
		RetrieveTreeState();
		m_fDoStateStore = TRUE;
	}

	// no selection yet, then select all supported target nodes!
	if (!GetSelCount())
	{
		if ((m_pProject==NULL) || (!m_pProject->m_bProjIsExe))
		{
			// go through looking for the targets ('roots')
			int iIndex = 0;
			CNode * pNode;
#ifndef NEWCFG
			CProjType * pprojtype;
#endif
			while ((pNode = GetNode(iIndex)) != (CNode *)NULL)
			{
				if (pNode->m_nLevels == 0)
				{
#ifndef NEWCFG
					// select all supported targets
					OptTreeNode * pOptTreeNode = (OptTreeNode *)pNode->m_dwData;
 					VERIFY(g_prjcompmgr.LookupProjTypeByName(pOptTreeNode->pcr->GetOriginalTypeName(), pprojtype));
			 		if (pprojtype->IsSupported())
#endif
					{
						SetSel(iIndex);	// select this target ('root') node
					}
				}
				iIndex++;
			}
		}
		else
		{
			// can do it the fast way if external project
			SetSel(-1);
		}
	}
}

// retrieve/save our state
void COptionTreeCtl::RetrieveTreeLevel(POSITION & pos, int iBaseIndex, int iBaseLevel)
{
	CNodeState * pState;

	// loop through nodes on this level
	while (pos != (POSITION)NULL)
	{
		POSITION posState = pos;
		pState = (CNodeState *) sm_TreeState.m_lstStates.GetNext(pos);

		// end of this level?
		if (pState->m_nLevel < iBaseLevel)
		{
			pos = posState;	// back-up
			return;
		}

		// search for this in our tree from our base
		CNode * pNode;
		int iIndex = iBaseIndex;
	 	BOOL fFound = FALSE;

		while (
			   // matching node not found already?
			   !fFound &&

			   // node found in actual?
			   ((pNode = GetNode(iIndex)) != (CNode *)NULL) &&

			   // gone through all children?
			   (pNode->m_nLevels >= pState->m_nLevel)
			  )
		{
			// match of actual with saved state?
			if (pState->m_nLevel == pNode->m_nLevels &&
				pState->m_str == pNode->m_szText)
			{
				// select and/or expand this?
				if (pState->m_fSelected)	SetSel(iIndex);
				if (pState->m_fExpanded)	Expand(iIndex);
				fFound = TRUE;
			}
			iIndex++;
		}

		// found this node?
		if (fFound)
		{
			// yes, get next node (if it exists) on this level (peek ahead in the list)
			if (pos != (POSITION)NULL)
			{
				pState = (CNodeState *) sm_TreeState.m_lstStates.GetAt(pos);
				// is this a child?
				if (pState->m_nLevel > iBaseLevel)
					// yes, retrieve the state of the tree for this child's branch
					RetrieveTreeLevel(pos, iIndex, iBaseLevel + 1);	// back-up
			}
		}
		else
		{
			// no, skip the next node(s) (if they exists) at all lower levels
			while (pos != (POSITION)NULL)
			{
				posState = pos;
				pState = (CNodeState *) sm_TreeState.m_lstStates.GetNext(pos);
				if (pState->m_nLevel <= iBaseLevel)
				{
					pos = posState;	// back-up
					break;
				}
			}
		}
	}
}

void COptionTreeCtl::RetrieveTreeState()
{
	if (sm_TreeState.m_lstStates.IsEmpty())
		return;	// tree-state store is clear

	POSITION pos = sm_TreeState.m_lstStates.GetHeadPosition();
	RetrieveTreeLevel(pos, 0, 0);
}

void COptionTreeCtl::SaveTreeState()
{
	// clear the old one
	sm_TreeState.Clear();

	// get the selection state first
	INT * rgInt = NULL;	// our selected item indexes
	int nSelItems;	// # of selected items
	GetTreeCtlSelection(this, rgInt, nSelItems);

	// make a note of the new expansion state
	CNode * pNode;
	int iIndex = 0, nSelection = 0;
	BOOL fSelected;
	while ((pNode = GetNode(iIndex)) != (CNode *)NULL)
	{	
		fSelected = FALSE;	// no selection by default

		if (nSelection < nSelItems)
			if (fSelected = (rgInt[nSelection] == iIndex))
				nSelection++;	// next selected item to look out for

		CNodeState * pState = new CNodeState(pNode->m_szText,
											 pNode->m_nLevels,
											 (pNode->m_nFlags & TF_EXPANDED) != 0,
											 fSelected
											);
		// this node state will get destroyed by the tree state store
		sm_TreeState.m_lstStates.AddTail(pState);
		iIndex++;
	}

	if (rgInt != (INT *)NULL)
		delete [] rgInt;
}

void COptionTreeCtl::OnSelect(int nIndex)
{
	// allow the tree control to select this node
	CTreeCtl::OnSelect(nIndex);

	// tree-control selection changes
	((CProjOptionsDlg *)GetParent())->OnTreeCtlSelChange();
}

int COptionTreeCtl::OnKey(UINT nKey)
{
	// is this the tab key?
	if (nKey == VK_TAB)
	{
		return -2;	// no further action
	}

	return -1;	// pass on to listbox
}

void COptionTreeCtl::OnLbSelChange()
{
	if (m_fIgnoreSelChange)
		return;	// ignore this selection change

	// do we need to validate the tab?
	if (!((CProjOptionsDlg *)GetParent())->ValidateCurrent())
	{
		// reset our selection to what is was before this sel change
		RememberSel();

		return;	// validation failed
	}

	// remember our selection
	NoteSel();
	CTreeCtl::OnLbSelChange();
}

void COptionTreeCtl::NoteSel()
{
	int iSelCount = GetSelCount();
	if (iSelCount == 0)	// no selection
	{
		// maintain at least a single-selection
		SetSel(m_rgLastGoodSel != (int *)NULL && m_cLastGoodSel != 0 ? m_rgLastGoodSel[0] : 0, TRUE);
		return;
	}

	if (m_rgLastGoodSel == (int *)NULL || iSelCount > m_cLastGoodSel)
	{
		if (m_rgLastGoodSel != (int *)NULL)
			delete [] m_rgLastGoodSel;

		m_rgLastGoodSel = new int[iSelCount];
	}
	if (m_rgLastGoodSel != (int *)NULL && GetSelItems(iSelCount, m_rgLastGoodSel) == LB_ERR)
		iSelCount = 0;	// flag, bad selection
	m_cLastGoodSel = iSelCount;	
}

void COptionTreeCtl::RememberSel()
{
	m_fIgnoreSelChange = TRUE;
	SetSel(-1, FALSE);
	for (int i = 0; i < m_cLastGoodSel; i++)
		SetSel(m_rgLastGoodSel[i], TRUE);
	m_fIgnoreSelChange = FALSE;
}

int COptionTreeCtl::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	// place addtional key-input (above and beyond standard CTreeCtl behaviour)
	// here for the option tree control
	return CTreeCtl::OnVKeyToItem(nKey, pListBox, nIndex);
}


BOOL COptionTreeCtl::NodeIsExpandable(CNode *pNode)
{
	CObList * pContent;
	ASSERT(pNode != NULL);

#ifdef NEWCFG
	CProjItem* pItem = ((OptTreeNodeList *)pNode->m_dwData)->GetItem();
#else
	CProjItem* pItem = ((OptTreeNode *)pNode->m_dwData)->pItem;
#endif
	if (pItem->IsKindOf(RUNTIME_CLASS(CProject)) && !((CProject *)pItem)->m_bProjIsExe)
	{
#ifdef NEWCFG
		pItem = pItem->GetTarget();
#else
		ConfigurationRecord* pcr = ((OptTreeNode *)pNode->m_dwData)->pcr;
		CString strTargetName = pcr->GetConfigurationName();
		pItem = m_pProject->GetTarget(strTargetName);
#endif
 	}
	
	pContent = pItem->GetContentList();

	// does this have a content (or an empty content)?
	if (pContent == (CObList *)NULL || pContent->IsEmpty())
		return FALSE;

	// make sure we either have a valid ref. or non ref. item
	CString strTemp;
	POSITION pos = pContent->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CProjItem * pChildItem = (CProjItem *)pContent->GetNext(pos);

		// Get the matching config of the child item for the nodes current
		if (!IsValidOptTreeItem(pChildItem, strTemp))
			continue;

		return TRUE;	// ok, is valid for the current target
	}

	return FALSE;
}

void COptionTreeCtl::GetNodeTitle(CNode *pNode, CString & strDesc)
{
	// for the project show the name of the configuration instead of the name of the makefile,
	// else get the "normal" name of the CProjItem.
#ifdef NEWCFG
	OptTreeNodeList * pOptNodeList = (OptTreeNodeList *)pNode->m_dwData;
	CProjItem * pItem = pOptNodeList->GetItem();
	if (pItem->IsKindOf(RUNTIME_CLASS(CProject)))
		strDesc = ((CProject *)pItem)->GetTargetName();
	else
		pItem->GetStrProp(P_ProjItemName, strDesc);
#else
	OptTreeNode * pOptNode = (OptTreeNode *)pNode->m_dwData;

 	UINT idDesc = pOptNode->pItem->IsKindOf(RUNTIME_CLASS(CProject)) ? P_ProjConfiguration : P_ProjItemName;
	pOptNode->GetStrProp(idDesc, strDesc, OBNone);
#endif
}

void COptionTreeCtl::GetDisplayText(CNode *pNode, CString & strDisplayText)
{
#ifdef NEWCFG
	OptTreeNodeList * pOptNodeList = (OptTreeNodeList *)pNode->m_dwData;
	CProjItem * pItem = pOptNodeList->GetItem();
	if (pItem->IsKindOf(RUNTIME_CLASS(CProject)))
		strDisplayText = ((CProject *)pItem)->GetTargetName();
	else
		pItem->GetStrProp(P_ProjItemName, strDisplayText);
#else
	// Are we updating the nodes at this moment
	OptTreeNode * pOptNode = (OptTreeNode *)pNode->m_dwData;

	// for the project show the name of the configuration instead of the name of the makefile,
	// else get the "normal" name of the CProjItem.
	if (pOptNode->pItem->IsKindOf(RUNTIME_CLASS(CProject)))
	{
		CProject * pProject = (CProject *)(pOptNode->pItem);
		CString strConfig, strTarget;
		VERIFY(pOptNode->GetStrProp(P_ProjConfiguration, strConfig, OBNone));

		// force this to use the option node's configuration
		CProjTempConfigChange projTempConfigChange(pProject);
		projTempConfigChange.ChangeConfig((ConfigurationRecord *)pOptNode->pcr->m_pBaseRecord);

#if 0
		if (pProject->IsExeProject() || pProject->IsExternalTarget())
		{
			CPath * pPath = pProject->GetTargetFileName();
			if (pPath)
			{
				strTarget = pPath->GetFileName();
				delete pPath;
			}
		}
		else
		{
			// try looking right at our actions
			// we may not have a dependency graph so enumerating it will not work
			ConfigurationRecord * pcr = pProject->GetActiveConfig();
			CActionSlobList * pActions = pcr->GetActionList();
			CActionSlobList lstActions;
			POSITION pos = pActions->GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				CActionSlob * pAction = (CActionSlob *)pActions->GetNext(pos);

				// primary output tool?
				if (!pAction->BuildTool()->HasPrimaryOutput())
					continue;	// no

				lstActions.RemoveAll();

				CActionSlob action(pAction->Item(), pAction->BuildTool(), FALSE, pcr);
				lstActions.AddTail(&action);

				// we want the primary target for the cloned (the ones we're working on)
				// to be shown
				int idOldBag = pAction->Item()->UsePropertyBag(CloneBag);
				pAction->BuildTool()->GenerateOutput(AOGO_Primary, lstActions, g_DummyEC);
				pAction->Item()->UsePropertyBag(idOldBag);
									 
#ifndef REFCOUNT_WORK
				const CPtrList * plstFile = action.GetOutput()->GetContent();

				if (plstFile->GetCount())
				{
					const CPath * pPath = g_FileRegistry.GetRegEntry((FileRegHandle)plstFile->GetHead())->GetFilePath();
#else
				if (action.GetOutput()->GetCount())
				{
					FileRegHandle frh = action.GetOutput()->GetFirstFrh();
					const CPath * pPath = g_FileRegistry.GetRegEntry(frh)->GetFilePath();
					frh->ReleaseFRHRef();
#endif
					if (pPath) strTarget = pPath->GetFileName();
					break;
				}
			}
		}
		strDisplayText = strConfig /* + _T(" (") + strTarget + _T(')') */;
#else
		strDisplayText = strConfig;
#endif

	}
	else
	{
		pOptNode->GetStrProp(P_ProjItemName, strDisplayText, OBNone);
	}
#endif
}

BOOL COptionTreeCtl::IsValidOptTreeItem(CProjItem * pItem, CString & str)
{
	// target node is always shown, synonymous with project node
	if (pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
		return TRUE;

	// ignore this item
	// o if it is dependency folder or file
	// o if we can't get the proj item name
	// o if it is a target reference not valid for this target
#ifdef _DEBUG
	str = "<Invalid Item>";
#endif

	// ignore this item
	// o if it is dependency folder or file
	if (pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)) ||
		pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
		return FALSE;

	// ignore this item
	// o if we can't get the proj item name
	if (pItem->GetStrProp(P_ProjItemName, str) == invalid)
		return FALSE;

/*
	// ignore this item
 	// o if it is a file not in the build dep.graph, eg. .doc or .txt
	if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
		CFileDepGraph * pdepgraph = g_buildengine.GetDepGraph(pcr);
		CGrNode * pgrn;
		if (!pdepgraph->LookupNode((NID)pItem->GetFileRegHandle(), pgrn))
			return FALSE;	// not in dep. graph -> not valid
	}
*/

	return TRUE;
}

// a project item's appearance has changed
void COptionTreeCtl::ProjItemChanged(CSlob * pProjItem, UINT uHint)
{
	// is this the ProxySlob? if so then we must use the content
	if (pProjItem->IsKindOf(RUNTIME_CLASS(CProxySlob)))
	{
		POSITION pos = ((CProxySlob *)pProjItem)->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			int iNode = FindNode((DWORD)((CProxySlob *)pProjItem)->GetNext(pos));

			// found it?
			if (iNode != -1)
			{
				// what is the hint?
				if ((uHint == P_ItemExcludedFromBuild) || (uHint == P_ItemIgnoreDefaultTool) || (uHint == P_ItemTools))
					// item excluded from state changes the item glyph
					InvalidateNode(iNode);
			}
		}
	}
	else
	{
		// search for this (config, projitem) pair
#ifndef NEWCFG
		ConfigurationRecord * pcr = (ConfigurationRecord *)((CProjItem *)pProjItem)->GetActiveConfig();
#endif

		CNode * pNode = (CNode *)NULL;
		int iNode = 0;
		while ((pNode = GetNode(iNode)) != (CNode *)NULL)
		{
#ifdef NEWCFG
			OptTreeNodeList * pOptNodeList = (OptTreeNodeList *)pNode->m_dwData;
			// is this the one?
			if (pOptNodeList->GetItem() == (CProjItem *)pProjItem)
				break;	// yes
#else
			OptTreeNode * pOptNode = (OptTreeNode *)pNode->m_dwData;
			// is this the one?
			if (pOptNode->pcr == pcr && pOptNode->pItem == (CProjItem *)pProjItem)
				break;	// yes
#endif
			iNode++;	// next node to look
		}

		// found it?
		if (pNode != (CNode *)NULL)
		{
			// what is the hint?
			if ((uHint == P_ItemExcludedFromBuild) || (uHint == P_ItemIgnoreDefaultTool) || (uHint == P_ItemTools))
			{
				// item excluded from state changes the item glyph
				InvalidateNode(iNode);
			}
			if (uHint == P_TargetName)
			{
				// target file name changes
				DirtyNode(iNode, TF_DIRTY_ITEM);
			}
		}
	}
}

BOOL COptionTreeCtl::InsertLevel(CNode *pParentNode, int nParentIndex, BOOL fExpandAll)
{
	CWaitCursor wc;
#ifdef NEWCFG
	OptTreeNodeList * pOptNodeList = (OptTreeNodeList *)pParentNode->m_dwData;	// get our tree node to expand
	CProjItem * pItem = pOptNodeList->GetItem();
	OptTreeNode * pOptNode = pOptNodeList->GetHead();
	OptTreeNode * pCurOptNode = NULL;
	OptTreeNodeList * pNewOptNodeList = NULL;
	int n = 0;
#else
	OptTreeNode * pOptNode = (OptTreeNode *)pParentNode->m_dwData;	// get our tree node to expand
	CProjItem * pItem = pOptNode->pItem;
#endif

	// is this a container?
	if (!pItem->GetContentList())
		return TRUE;	// don't expand
	
	// HACK ALERT - this code skips over the target node
	if (pItem->IsKindOf(RUNTIME_CLASS(CProject))
#ifdef _DEBUG
		&& !g_bProjDebugView
#endif
		)
	{
#ifdef NEWCFG
		pItem = pItem->GetTarget();
#else
		CString strTargetName = pOptNode->pcr->GetConfigurationName();
		pItem = ((CProject*)pItem)->GetTarget(strTargetName);
#endif
		ASSERT(pItem != NULL);
 	}

	// enumerate through our container or content if already valid
	POSITION pos = pOptNode->m_fValidContent ? pOptNode->m_Content.GetHeadPosition() : pItem->GetHeadPosition();

	// is this container empty?
	while (pos != NULL)
	{
		CString	cstr;
		
		// no
		OptTreeNode *	pNewOptNode;	// new option node
		CNode *			pNode;			// dummy argument
		int				iNode;			// dummy argument
		if (pOptNode->m_fValidContent)
		{
			// we've already created this set of nodes
			pNewOptNode = (OptTreeNode *)pOptNode->m_Content.GetNext(pos);
			if (pNewOptNode->pItem->GetStrProp(P_ProjItemName, cstr) != valid)
			{
				ASSERT(0);
				continue;	// don't add this one
			}
#ifdef NEWCFG
			CProjItem * pChildItem = pNewOptNode->pItem;
			pNewOptNodeList = pOptNodeList->GetDependentList(pChildItem);
			if (pNewOptNodeList == NULL)
			{
				pNewOptNodeList = pOptNodeList->AddDependentList();	  // does a smart alloc
				POSITION pos2 = pOptNodeList->GetHeadPosition();
				ASSERT(pos2);
				while (pos2 != NULL)
				{
					pCurOptNode = pOptNodeList->GetNext(pos2);
					POSITION pos3 = pCurOptNode->m_Content.FindIndex(n);
					ASSERT(pos3);
					pNewOptNode = (OptTreeNode *)pCurOptNode->m_Content.GetAt(pos3);
					ASSERT(pNewOptNode->pItem == pChildItem);
					pNewOptNodeList->AddHead(pNewOptNode);
				}
			}
			n++;

#endif
		}
		else
		{
			CProjItem * pChildItem = (CProjItem *)pItem->GetNext(pos);
			if (!IsValidOptTreeItem(pChildItem, cstr))
				continue;		

#ifdef NEWCFG
			// construct our new OptTreeNode to insert
			// we'll destroy this when we free the target it belongs to
			pNewOptNodeList = pOptNodeList->AddDependentList();	  // does a smart alloc
			POSITION pos2 = pOptNodeList->GetHeadPosition();
			ASSERT(pos2);
			while (pos2 != NULL)
			{
				pCurOptNode = pOptNodeList->GetNext(pos2);
				pNewOptNode = new OptTreeNode(pCurOptNode, pChildItem);
				ASSERT(pNewOptNode->pcr != NULL);
				pNewOptNodeList->AddHead(pNewOptNode);
			}
#else
			pNewOptNode = new OptTreeNode(pOptNode, pChildItem);
			ASSERT(pNewOptNode->pcr != NULL);
#endif
		}

		// insert this option node into the tree-control's concept of a node tree

#ifdef NEWCFG
		if (!InsertNode(pParentNode, nParentIndex, cstr, (DWORD) pNewOptNodeList, pNode, iNode))
#else
		if (!InsertNode(pParentNode, nParentIndex, cstr, (DWORD) pNewOptNode, pNode, iNode))
#endif
		{
			TRACE("CProjTreeCtl::InsertLevel: call to InsertNode failed!\n");
			return FALSE;
		}
		
		if (fExpandAll && !InsertLevel(pNode, iNode, TRUE))	return FALSE;
	}

	// the content of this node is now valid

#ifdef NEWCFG
	POSITION pos2 = pOptNodeList->GetHeadPosition();
	ASSERT(pos2);
	while (pos2 != NULL)
	{
		pCurOptNode = pOptNodeList->GetNext(pos2);
			pCurOptNode->m_fValidContent = TRUE;
	}
#else
	pOptNode->m_fValidContent = TRUE;
#endif

	return TRUE;
}

void COptionTreeCtl::DrawNodeGlyph
(
	CDC* pDC,
	CNode* pNode,
	BOOL bHighlight,
	CPoint pt
)
{
	extern CImageWell g_imageWell;
	
#ifdef NEWCFG
	OptTreeNodeList* pOptNodeList = (OptTreeNodeList *)pNode->m_dwData;
	OptTreeNode* pOptNode = pOptNodeList->GetFirstValidNode();
	CProjItem * pItem = pOptNodeList->GetItem();
#else
	OptTreeNode* pOptNode = (OptTreeNode *)pNode->m_dwData;
	CProjItem * pItem = pOptNode->pItem;
#endif
	CObList * pContent = pItem->GetContentList();
	// is this a folder?
	if ((pContent != NULL) && (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup))))
	{
		// Use default container glyphs...
		CTreeCtl::DrawNodeGlyph(pDC, pNode, bHighlight, pt);
		return;
	}
	
	// 0: part of build, 1: excluded from build; 2: not buildable
	// 6: project node
	int nGlyph = 2;
	if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
		// make sure we get the source tool plus prop. from the node's config.
		if (pOptNode != NULL)
			((CFileItem*)pItem)->ForceConfigActive(pOptNode->pcr);
		BOOL bExcluded = TRUE;
		pItem->GetIntProp(P_ItemExcludedFromBuild, bExcluded);
		if (((CFileItem*)pItem)->GetSourceTool() != (CBuildTool *)NULL)
		{
			nGlyph = bExcluded ? 1 : 0;
		}
		else
		{
			nGlyph = bExcluded ? 1 : 2;
		}
		if (pOptNode != NULL)
			((CFileItem*)pItem)->ForceConfigActive();
	}
	// Display the right glyph for target references
	else if (pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)) ||
		 pItem->IsKindOf(RUNTIME_CLASS(CProject)) ||
		 pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
	{
		CProject * pRefProject = NULL;
		if (pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		{
			// REVIEW: not quite right...
#ifndef NEWCFG
			((CTargetItem*)pOptNode->pItem)->ForceConfigActive(pOptNode->pcr);
#endif
			BOOL bExcluded = TRUE;
			pItem->GetIntProp(P_ItemExcludedFromBuild, bExcluded);
			nGlyph = bExcluded ? 10 : 7;

			CTargetItem * pRefTarget = ((CProjectDependency *)pItem)->GetProjectDep();
			if (pRefTarget != NULL)
				pRefProject = pRefTarget->GetProject();
#ifndef NEWCFG
			((CTargetItem*)pOptNode->pItem)->ForceConfigActive();
#endif
		}
		else
		{
			nGlyph = 4;
			if (pItem->IsKindOf(RUNTIME_CLASS(CProject)))
				pRefProject = (CProject *)pItem;
			else
				pRefProject = pItem->GetProject();
		}
		// different glyphs for different proj types
		if (pRefProject != NULL)
		{
			// need to get path from assoc. project for these
			nGlyph += pRefProject->GetGlyphIndex(); // 0, 1 or 2
		}
	}

	g_imageWell.DrawImage(pDC, pt, nGlyph);
}

void COptionTreeCtl::InvalidateNode (int iNode)
{
	CRect rect;
	GetListBox()->GetItemRect(iNode, rect);
	GetListBox()->InvalidateRect(rect);
}

int COptionTreeCtl::CompareData(const CNode* pNode1, const CNode* pNode2)
{

	if (pNode1 == pNode2)
		return 0;
	
	int ret = CompareData(pNode1->m_pParent, pNode2->m_pParent);
	
	if (ret == 0)
	{
#ifdef NEWCFG
		CProjItem * pItem1 = ((OptTreeNodeList *)pNode1->m_dwData)->GetItem();
		CProjItem * pItem2 = ((OptTreeNodeList *)pNode2->m_dwData)->GetItem();
#else
		CProjItem * pItem1 = ((OptTreeNode *)pNode1->m_dwData)->pItem;
		CProjItem * pItem2 = ((OptTreeNode *)pNode2->m_dwData)->pItem;
#endif

		ret = pItem1->IsKindOf(RUNTIME_CLASS(CDependencyContainer)) -
			  pItem2->IsKindOf(RUNTIME_CLASS(CDependencyContainer));

		if (ret) return ret;

		BOOL bIsGroup2 = pItem2->IsKindOf(RUNTIME_CLASS(CProjGroup));

		if (bIsGroup2)
			return 1;

		BOOL bIsGroup1 = pItem1->IsKindOf(RUNTIME_CLASS(CProjGroup));

		if (bIsGroup1)
			return -1;

		// Put subprojects at the start of this container
		if (pItem2->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
			return 1;

		if (pItem1->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
			return -1;

		CString strExt1, strExt2; 
		BOOL bIsFile2 = pItem2->IsKindOf(RUNTIME_CLASS(CFileItem));
		BOOL bIsFile1 = pItem1->IsKindOf(RUNTIME_CLASS(CFileItem));
		if (bIsFile2)
			strExt2 = pItem2->GetFilePath()->GetExtension();
		if (bIsFile1)
			strExt1 = pItem1->GetFilePath()->GetExtension();

		// Put .lib files last and don't sort them alphabetically
		if (bIsFile1 && (strExt1.CompareNoCase(".lib")==0))
			return 1;

		if (bIsFile2 && (strExt2.CompareNoCase(".lib")==0))
			return -1;

		// Put .obj files last and don't sort them alphabetically
		if (bIsFile1 && (strExt1.CompareNoCase(".obj")==0))
			return 1;

		if (bIsFile2 && (strExt2.CompareNoCase(".obj")==0))
			return -1;

		// default is to compare text
		ret = _tcsicmp(pNode1->m_szText, pNode2->m_szText);

		// as a tie breaker, use the absolute path to at least get consistency
		// with fileview
		if (ret == 0)
		{
			// should be CFileItems by this point
			ASSERT(pItem1->GetFilePath() != NULL);
			ASSERT(pItem2->GetFilePath() != NULL);
			ret = _tcsicmp((LPCTSTR)*pItem1->GetFilePath(), (LPCTSTR)*pItem2->GetFilePath());
		}

		// if we have a match then compare the data
		if (ret == 0 && (pNode1->m_dwData != 0 || pNode2->m_dwData != 0))
		{
			if (pNode1->m_dwData == pNode2->m_dwData)
				ret = 0;
			else
				ret = (int) (pNode1->m_dwData - pNode2->m_dwData);
		}
	}
	
	return ret;
}

UINT g_nActiveEdit = 0;

CMapStringToString CProjOptionsDlg::m_mapLastMiniPage;

CMapStringToPtr CProjOptionsDlg::m_mapActiveFlavours;

BEGIN_MESSAGE_MAP (CProjOptionsDlg, CTabbedDialog)
	//{{AFX_MSG_MAP (CProjOptionsDlg)
	ON_WM_CREATE()
	ON_WM_DESTROY()	
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_SELECT_ITEM, OnSelectItem)
	ON_CBN_SELCHANGE(IDC_TARGET_LISTCOMBO, OnSelChangeTarget)
	ON_EN_SETFOCUS(IDC_CUSTOM_CMDS, OnActivateEditCmds)
	ON_EN_SETFOCUS(IDC_CUSTOM_OUTPUT, OnActivateEditOutput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP	()

//	ON_BN_CLICKED(IDC_CHANGE_TARGET, OnBtnChangeTarget)

//#pragma message("ALERT! : optndlg.cpp : Including Project.Options tree-control hooks for QA...")
int GetNodeLevel(CTreeCtl * pTree, int nLevel)
{
	// move up to a level ? node
	CNode * pNode;
	int iIndex = pTree->GetCurSel();
	while ((pNode = pTree->GetNode(iIndex)) != (CNode *)NULL)
	{
		// are we on a level ? node?
		if (pNode->m_nLevels == nLevel)
			return iIndex;

		iIndex++;
	}

	return -1;	// didn't find
}

int GetFirstChild(CTreeCtl * pTree, int iParent)
{
	if (!pTree->NodeIsExpandable(pTree->GetNode(iParent)))
		return -1;	// didn't find

	// make sure this node is expanded
	if (!pTree->IsExpanded(iParent))
		pTree->Expand(iParent, TRUE);

	// if this parent was expandable then
	// we must have the first child as parent index
	// plus 1
	return iParent + 1;

}

int SelectItem(CTreeCtl * pTree, int iSibling, CString & strTitle, UINT idProp)
{
	// which level are we searching on?
	CNode * pNode = pTree->GetNode(iSibling);
	int nLevel = pNode->m_nLevels;

	CString strNodeTitle;

	while ((pNode = pTree->GetNode(iSibling)) != (CNode *)NULL)
	{
		// desired level?
		if (pNode->m_nLevels == nLevel)
		{
			OptTreeNode * pOptTreeNode = ((OptTreeNodeList *)pNode->m_dwData)->GetHead();
			if (pOptTreeNode->GetStrProp(idProp, strNodeTitle, OBNone) == valid &&
				(strNodeTitle.CompareNoCase(strTitle) == 0))
			{
				pTree->SetSel(-1, FALSE);
				pTree->SetSel(iSibling, TRUE);
				return iSibling;
			}
		}
		iSibling++;
	}

	return -1;	// not found
}

void CProjOptionsDlg::OnActivateEditCmds()
{
	ASSERT(0);
	g_nActiveEdit = IDC_CUSTOM_CMDS;
}

void CProjOptionsDlg::OnActivateEditOutput()
{
	ASSERT(0);
	g_nActiveEdit = IDC_CUSTOM_OUTPUT;
}

void CProjOptionsDlg::UpdateConfigsFromDlg()
{
	OptTreeNode::ResetValidConfigMap();
	POSITION pos = m_mapActiveFlavours.GetStartPosition();
	ASSERT(pos);
	void * pVoid;
	CString strFlavour;
	while (pos != NULL)
	{
		m_mapActiveFlavours.GetNextAssoc(pos, strFlavour, pVoid);

		// iterate over all projects
		CProject * pProject = m_pProject;
		CString strTarget;
		// support multiple projects
#ifdef MPROJ
		const CObList * pProjList = CProject::GetProjectList();
		POSITION pos = pProjList->GetHeadPosition();
		while (pos != NULL)
		{
			pProject = (CProject *)pProjList->GetNext(pos);
			if ((pProject==NULL) || (!pProject->IsLoaded()))
				continue;
#endif

			//
			// Exe projects are not allowed in a multi-project workspace.  If allowed
			// the GetActiveTarget below will return a NULL and cause a crash.
			//
			ASSERT(!pProject->m_bProjIsExe);

			strTarget = pProject->GetTargetName();
			const CPtrArray * pCfgProject = pProject->GetConfigArray();
			int size = pCfgProject->GetSize();

			CString strConfigToMatch = strTarget + " - " + strFlavour;
			for (int i = 0; i < size; i++)
			{
				ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgProject->GetAt(i);
				if (strConfigToMatch.CompareNoCase(pcr->GetConfigurationName())==0)
				{
					ASSERT(pcr==pcr->m_pBaseRecord);
					ConfigurationRecord * pcrTarget = pProject->GetActiveTarget()->ConfigRecordFromBaseConfig(pcr);
					if ((pcrTarget!= NULL) && (pcrTarget->IsSupported()))
					{
						OptTreeNode::SetValidConfig(pcr);
					}
				}
			}
#ifdef MPROJ
		}
#endif
	}
}

BOOL CProjOptionsDlg::UpdateConfigsFromCombo()
{
	int index = m_cbConfigs.GetCurSel();
	if ((index != CB_ERR) && (index != m_nCurTargetSel))
	{
		m_nCurTargetSel = index;
		ConfigurationRecord * pcr;
		CString strFlavour, strConfigToMatch;
		OptTreeNode::ResetValidConfigMap();
		m_cbConfigs.GetLBText(index, strFlavour);
		BOOL bAll =  (strFlavour == m_strAllConfigs);
		CProjOptionsDlg::ResetActiveFlavours();
		
		// iterate over all projects
		CProject * pProject = m_pProject;
		BOOL bProjIsExe = ((m_pProject != NULL) && (m_pProject->m_bProjIsExe));
		CString strTarget;
#ifdef MPROJ
		// support multiple projects
		const CObList * pProjList = CProject::GetProjectList();
		POSITION pos = pProjList->GetHeadPosition();
		while (pos != NULL)
		{
			pProject = (CProject *)pProjList->GetNext(pos);
			if ((pProject==NULL) || (!pProject->IsLoaded()))
				continue;
#endif
			if (bProjIsExe)
			{
				bAll = TRUE;
			}
			else
			{
				strTarget = pProject->GetTargetName();
				strConfigToMatch = strTarget + " - " + strFlavour;
			}
			const CPtrArray * pCfgProject = pProject->GetConfigArray();
			int size = pCfgProject->GetSize();

			for (int i = 0; i < size; i++)
			{
				pcr = (ConfigurationRecord *)pCfgProject->GetAt(i);
				if ((bAll) || (strConfigToMatch.CompareNoCase(pcr->GetConfigurationName())==0))
				{
					ASSERT(pcr==pcr->m_pBaseRecord);
					ConfigurationRecord * pcrTarget = bProjIsExe?NULL:pProject->GetActiveTarget()->ConfigRecordFromBaseConfig(pcr);
					if (bProjIsExe || ((pcrTarget!= NULL) && (pcrTarget->IsSupported())))
					{
						OptTreeNode::SetValidConfig(pcr);
						if (bAll)
						{
							strFlavour  = pcr->GetConfigurationName();
							if (!bProjIsExe)
							{
								int index = strFlavour.Find(" - ");
								strFlavour = strFlavour.Mid(index + 3);
							}
						}
						CProjOptionsDlg::SetActiveFlavour(strFlavour);
					}
				}
			}
#ifdef MPROJ
		}
#endif
		return TRUE;
	}
	return FALSE;
}

void CProjOptionsDlg::OnSelChangeTarget()
{
	//
	// Validate current tab before allowing change.
	//
	if (m_nTabCur != -1)
	{
		CDlgTab* pTab = GetTab(m_nTabCur);
		if ((pTab==NULL) || (!pTab->ValidateTab()))
		{
			//
			// Previous selection.
			//
			m_cbConfigs.SetCurSel(m_nCurTargetSel);
			return;
		}
	}

	int indexMS = m_cbConfigs.FindStringExact(-1, m_strMultiConfigs);
	int index = m_cbConfigs.GetCurSel();
	if ((indexMS != CB_ERR) && (indexMS == index))
	{
		InvokeChangeTargetDlg();
	}
	else if (UpdateConfigsFromCombo())
	{
		m_ProxySlob.Clear(FALSE);
		RefreshTargets();
	}
}

#if 0
void CProjOptionsDlg::OnBtnChangeTarget()
{
	InvokeChangeTargetDlg();
}
#endif

BOOL CProjOptionsDlg::InvokeChangeTargetDlg()
{
	CConfigSelectDlg dlg;
	if ((dlg.DoModal() == IDOK) && (dlg.m_SelState != CConfigSelectDlg::same))
	{
		int index = m_cbConfigs.FindStringExact(-1, m_strMultiConfigs);
		if (dlg.m_SelState == CConfigSelectDlg::multi)
		{
			m_cbConfigs.SetCurSel(index);
			UpdateConfigsFromDlg();
			m_nCurTargetSel = index;

			m_ProxySlob.Clear(FALSE);
			RefreshTargets();
		}
		else
		{
			if (dlg.m_SelState == CConfigSelectDlg::all)
			{
				index = m_cbConfigs.FindStringExact(-1, m_strAllConfigs);
			}
			else
			{
				void * pVoid;
				ASSERT(dlg.m_SelState == CConfigSelectDlg::single);
				POSITION pos = m_mapActiveFlavours.GetStartPosition();
				CString strFlavour;
				m_mapActiveFlavours.GetNextAssoc(pos, strFlavour, pVoid);
				index = m_cbConfigs.FindStringExact(-1, strFlavour);
			}
			ASSERT(index != CB_ERR);
			m_cbConfigs.SetCurSel(index);
			if (UpdateConfigsFromCombo())
			{
				m_ProxySlob.Clear(FALSE);
				RefreshTargets();
			}
		}
		return TRUE;
	}
	m_cbConfigs.SetCurSel(m_nCurTargetSel);
	return FALSE;
}

void CProjOptionsDlg::RefreshTargets()
{
	// Review: could be mroe efficient about not deleting all nodes
	// 1. See if list of valid targets has actually changed (we should do this)
	// 2. Only delete/add the changed nodes (scary)
	m_OptTreeCtl.SaveTreeState();
	m_OptTreeCtl.RefreshTargetNodes();
	m_OptTreeCtl.CopyTreeCtlState(NULL);
	m_OptTreeCtl.NoteSel(); // remember this selection
	OnTreeCtlSelChange();
}

LRESULT CProjOptionsDlg::OnSelectItem(WPARAM type, LPARAM stratomTitle)
{
	if (m_bDirtyTabs)
	{
		// update option tabs here
		OnTreeCtlSelChange();
		m_bDirtyTabs = FALSE;
		return TRUE;
	}

	if (m_OptTreeCtl.m_hWnd == (HWND)NULL)
		return FALSE;	// no tree control

	CString strTitle, strNodeTitle;
	if (!GlobalGetAtomName(ATOM(stratomTitle), strTitle.GetBuffer(200), 200))
		return FALSE;	// can't get title string
	strTitle.ReleaseBuffer();

	int iIndex;

	// do we want to select a target?
	if (type == PRJW_TARGET)
	{	
		if ((iIndex = SelectItem(&m_OptTreeCtl, 0, strTitle, P_ProjConfiguration)) == -1)
			return FALSE;	// didn't find
	}
	// do we want to select a group?
	else if (type == PRJW_GROUP)
	{
		// move up to a level 0 node (target layer)	
		if ((iIndex = GetNodeLevel(&m_OptTreeCtl, 0)) == -1)
			return FALSE;	// didn't find

		if ((iIndex = GetFirstChild(&m_OptTreeCtl, iIndex)) == -1)
			return FALSE;	// didnt' find

		if ((iIndex = SelectItem(&m_OptTreeCtl, iIndex, strTitle, P_GroupName)) == -1)
			return FALSE;	// didn't find
	}
	else if (type == PRJW_FILE)
	{	
		// move up to a level 1 node (group layer)
		if ((iIndex = GetNodeLevel(&m_OptTreeCtl, 1)) == -1)
			return FALSE;	// didn't find

		if ((iIndex = GetFirstChild(&m_OptTreeCtl, iIndex)) == -1)
			return FALSE;	// didn't find

		if ((iIndex = SelectItem(&m_OptTreeCtl, iIndex, strTitle, P_ProjItemName)) == -1)
			return FALSE;	// didn't find
	}
	else
		ASSERT(FALSE);

	// update option tabs here
	OnTreeCtlSelChange();

	return TRUE;	// success
	
}

int CProjOptionsDlg::OnCreate(LPCREATESTRUCT lpcs)
{
	// create our options tree control
	// (we'll adjust the size/position in CProjOptionsDlg::OnSize())
	if (!m_OptTreeCtl.Create(this, CRect(0, 0, 0, 0)))
		return -1;	// failed to create the pane
	if (UseWin4Look())
		m_OptTreeCtl.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	// create our static text
	CString strTxt; strTxt.LoadString(IDS_SETTINGSFOR);
	CRect rectTxt; rectTxt.SetRectEmpty();
	m_txtSettingsFor.Create(strTxt, WS_CHILD | WS_VISIBLE, rectTxt, this);
	if (UseWin4Look())
		m_txtSettingsFor.SetFont(GetStdFont(font_Normal));
	else
		m_txtSettingsFor.SetFont(GetStdFont(font_Bold));

	CRect rectCB; rectCB.SetRect(0,0,200,200);
	m_cbConfigs.Create(CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | ES_AUTOHSCROLL | WS_TABSTOP, rectCB, this, IDC_TARGET_LISTCOMBO);
	m_cbConfigs.SetExtendedUI();

	if (UseWin4Look())
		m_cbConfigs.SetFont(GetStdFont(font_Normal));
	else
		m_cbConfigs.SetFont(GetStdFont(font_Bold));

	CString strCurrentFlavour;
	ConfigurationRecord * pcrCurrent = NULL;
	if (g_pActiveProject)
		pcrCurrent = g_pActiveProject->GetActiveConfig();

	CProject * pProject = m_pProject;

	// take active config from selection if appropriate
	if ((m_pSelectionSlob != NULL) && (!m_pSelectionSlob->GetContentList()->IsEmpty()))
	{
		CProjItem * pProjItem = ((CProjItem *)m_pSelectionSlob->GetContentList()->GetHead())->GetProject();
		pProject = pProjItem->GetProject();
		if (pProject != NULL && pProject->IsLoaded())
		{
			m_pProject = pProject;
			pcrCurrent = pProject->GetActiveConfig();
		}
	}

	BOOL bProjIsExe = ((m_pProject != NULL) && (m_pProject->m_bProjIsExe));

	CString strTarget;
#ifdef MPROJ
	// support multiple projects
	const CObList * pProjList = CProject::GetProjectList();
	POSITION pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif
		CString strFlavour;
		int nTargLen = 0;
		if (!bProjIsExe)
		{
			strTarget = pProject->GetTargetName();
			strTarget += " - ";
			nTargLen = strTarget.GetLength();
			ASSERT(nTargLen > 3);
		}

		POSITION pos = g_theConfigEnum.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CEnumerator	* pEnum = g_theConfigEnum.GetNext(pos);
			if (pEnum->val)	// supported project type?
			{
				CString strConfig = pEnum->szId;
				if ((bProjIsExe) || (strncmp(strTarget, strConfig, nTargLen)==0))
				{
					ConfigurationRecord * pcr = pProject->ConfigRecordFromConfigName(strConfig, FALSE);
					if (pcr != NULL)
					{
						strFlavour = strConfig.Mid(nTargLen);
						int index = CB_ERR;
						if ((index = m_cbConfigs.FindStringExact(-1, strFlavour))==CB_ERR)
						{
							index = m_cbConfigs.AddString(strFlavour);
							ASSERT(index != CB_ERR);
						}
						if (pcr == pcrCurrent)
						{
							strCurrentFlavour = strFlavour;
							m_cbConfigs.SetCurSel(index);
						}
					}
				}
			}
		}
#ifdef MPROJ
	}
#endif
	int x = 0; //default
	m_strAllConfigs.LoadString(IDS_ALL_CONFIGS);
	m_strMultiConfigs.LoadString(IDS_MULTI_CONFIGS);
	int count = m_cbConfigs.GetCount();
	if (bProjIsExe || (count <= 1))
	{
		m_cbConfigs.EnableWindow(FALSE);
	}
	else
	{
		x = m_cbConfigs.AddString(m_strAllConfigs);
		if (count > 2)
		{
			m_cbConfigs.AddString(m_strMultiConfigs);
		}
	}
	// int x1 = m_cbConfigs.FindStringExact(-1, pcrCurrent->GetConfigurationName());
	// if (x1 != CB_ERR) x = x1; // worst case, select all
	if (strCurrentFlavour.IsEmpty())
	{
		m_cbConfigs.SetCurSel(x);
		strCurrentFlavour = m_strAllConfigs;
	}

#if 0
	strTxt.LoadString(IDS_CONFIGS);
	m_bnConfigs.Create(strTxt, WS_CHILD | WS_VISIBLE | WS_TABSTOP, rectTxt, this, IDC_CHANGE_TARGET);
	if (UseWin4Look())
		m_bnConfigs.SetFont(GetStdFont(font_Normal));
	else
		m_bnConfigs.SetFont(GetStdFont(font_Bold));
#endif

	// create our 'No Common Properties' static text dlg
	m_dlgNoProps.Create(this);

	// no previous tab or handlers in the list
	m_listHdlrs.RemoveAll();
	m_strPrevTab.Empty();

	// initialise our options tree control
	// (this'll force a selection change, and tab creation)
	VERIFY(UpdateConfigsFromCombo());
	
	m_OptTreeCtl.RefreshTargetNodes();

    // Need to copy the tree ctl state from build view
#ifdef NEWCFG
	m_OptTreeCtl.CopyTreeCtlState(m_pSelectionSlob);
#else
	m_OptTreeCtl.CopyTreeCtlState(NULL);
#endif

	m_OptTreeCtl.NoteSel(); // remember this selection

#ifdef MPROJ
	// support multiple projects
	pProjList = CProject::GetProjectList();
	pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif
		// clone our current props into a store
		// we'll use to perform prop. ops until OK'ed
		(void) DoBagOpOverProject(pProject, CurrBag, CloneBag, BO_Copy | BO_UseDest);
#ifdef MPROJ
	}
#endif
	// hook the project option engine to us
 	g_prjoptengine.SetPropertyBag(&m_ProxySlob);

	// initialise the selection dependent stuff
	UpdateProxySlob(FALSE);	// don't need inform
	CacheOptionHandlers();
	UpdateOptionTabs();

	// be smart and select the 'tool' page if we were
	// brought up from the popup for a file,
   
	if (m_pSelectionSlob &&
		m_ProxySlob.IsSortOf(RUNTIME_CLASS(CFileItem)) &&	// files selected
		GetTabCount() == 2		// we have a second 'tool' tab
	  )
	  m_nTabCur = 1;	// select the 'tool' tab instead (second)

	if (CTabbedDialog::OnCreate(lpcs) == -1)
		return -1;	// failure

	m_OptTreeCtl.SetContextHelpID(HIDC_PROJBLD_SETTINGS_TREECTL);
	m_txtSettingsFor.SetWindowContextHelpId(HIDC_PROJBLD_SETTINGS_CFGTXT);
	m_cbConfigs.SetWindowContextHelpId(HIDC_PROJBLD_SETTINGS_CONFIGS);
	m_dlgNoProps.SetWindowContextHelpId(HIDC_PROJBLD_SETTINGS_NOPROPS);

	// show/don't show the 'No Common Properties' dlg. on whether common list hdlrs exists.
	m_dlgNoProps.ShowWindow(m_listHdlrs.IsEmpty() ? SW_SHOWNA : SW_HIDE);

	if (m_listHdlrs.IsEmpty())
	{
		// make sure the focus is not on the 'no common props dialog'
		m_OptTreeCtl.SetFocus();
	}

	// cache a pointer to the notify slob
#ifdef MPROJ
	// support multiple projects
	pProjList = CProject::GetProjectList();
	pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif
		// don't allow the project to be dirtied
		pProject->SetOkToDirtyProject(FALSE);
#ifdef MPROJ
	}
#endif

	// we don't allow the recording of props in a modal dialog!
	if (theUndoSlob.IsRecording()) 
	{
		// paause, and remember to re-enable, recording
		theUndoSlob.Pause();
		m_fResumeRecording = TRUE;
	}
	else
	{
		// no need to resume recording, we aren't currently
		m_fResumeRecording = FALSE;
	}

	return 0;	// succeed
}

void CProjOptionsDlg::OnDestroy()
{
	// resume recording?
	if (m_fResumeRecording)
		theUndoSlob.Resume();

	CProject * pProject = m_pProject;
#ifdef MPROJ
	// support multiple projects
	const CObList * pProjList = CProject::GetProjectList();
	POSITION pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif
		// don't want to have our project 'view' informed anymore
		pProject->SetOptionTreeCtl((COptionTreeCtl *)NULL);
#ifdef MPROJ
	}
#endif

#ifdef MPROJ
	pProjList = CProject::GetProjectList();
	pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif
		// do cleanup of clone prop. bags
		(void) DoBagOpOverProject(pProject, UINT(-1), CloneBag, BO_Clear);
		// pProject->SetOkToDirtyProject(TRUE);
#ifdef MPROJ
	}
#endif
}

void Draw3dRect(CDC * pDC, const CRect * prect, BOOL bInset)
{
	int cx = prect->Width()+1;
	int cy = prect->Height()+1;

	// top/left (inside) corner
	CBrush * pOldBrush = pDC->SelectObject(GetSysBrush(bInset ? COLOR_BTNTEXT : COLOR_BTNHIGHLIGHT));
	pDC->PatBlt(prect->left, prect->top, cx-1, 1, PATCOPY);
	pDC->PatBlt(prect->left, prect->top, 1, cy-1, PATCOPY);

	// bottom/right (inside) corner
	pDC->SelectObject(GetSysBrush(bInset ? COLOR_BTNFACE : COLOR_BTNSHADOW));
	pDC->PatBlt(prect->left, prect->bottom, cx, 1, PATCOPY);
	pDC->PatBlt(prect->right, prect->top, 1, cy, PATCOPY);

	// top/left (outside) corner
	pDC->SelectObject(GetSysBrush(bInset ? COLOR_BTNSHADOW : COLOR_BTNFACE));
	pDC->PatBlt(prect->left-1, prect->top-1, cx+1, 1, PATCOPY);
	pDC->PatBlt(prect->left-1, prect->top-1, 1, cy+1, PATCOPY);

	// bottom/right (outside) corner
	pDC->SelectObject(GetSysBrush(bInset ? COLOR_BTNHIGHLIGHT : COLOR_BTNTEXT));
	pDC->PatBlt(prect->left-1, prect->bottom+1, cx+2, 1, PATCOPY);
	pDC->PatBlt(prect->right+1, prect->top-1, 1, cy+2, PATCOPY);

	pDC->SelectObject(pOldBrush);
}

HBRUSH CProjOptionsDlg::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	if ((pWnd->m_hWnd == m_txtSettingsFor.m_hWnd) && (nCtlColor == CTLCOLOR_STATIC))
	{
		pDC->SetBkColor(GetSysColor(COLOR_BTNFACE)); // bkgnd text color
		return (HBRUSH)GetSysBrush(COLOR_BTNFACE)->m_hObject;	// bkgnd brush
	}

	return (HBRUSH)0L;
}

void CProjOptionsDlg::OnPaint()
{
	// call our base-class first, we'll paint our frames over it
	CTabbedDialog::OnPaint();	

	CDC * pDC = GetDC();
	ASSERT(pDC != (CDC *)NULL);

	// paint an inset rect around the tree-control
	CRect rect; m_OptTreeCtl.GetWindowRect(rect);
	ScreenToClient(&rect);
	rect.InflateRect(1, 1);
	rect.right--; rect.bottom--;	// fudge

	if (!UseWin4Look())
		Draw3dRect(pDC, &rect, TRUE);

	// paint an outset rect around the tree-control plus
	// static text label

	rect.InflateRect(CP_OUTER_BORDER, CP_OUTER_BORDER);
	rect.top = m_tabRow.GetRect().top;

	Draw3dRect(pDC, &rect, FALSE);

	VERIFY(ReleaseDC(pDC));
}

void CProjOptionsDlg::OnSize(UINT nType, int cx, int cy)
{
	// size our tabbed dialog first
	CTabbedDialog::OnSize(nType, cx, cy);

	// o move the static text
	// o move and size the tree control
	// o ensure the tabbing order is correct

	CRect rectTab; GetTabSize(rectTab);
	CRect rectTabRow = m_tabRow.GetRect();
	CClientDC dc(this);
	int nTabHeight = m_tabRow.DesiredHeight(&dc);
	
	CRect margins;
	CTabbedDialog::GetMargins(margins);

    // TitoM: BugFix DevStudio #11035
    // GetDC() returns a DC with default attributes for common classes, thus
    // GetTextExtent() returns an incorrect width for the text.  To get correct extend info
    // the lables font needs to be set into the DC used to query the extent
	CString strTxt; strTxt.LoadString(IDS_SETTINGSFOR);
    CDC   *pDC     = m_txtSettingsFor.GetDC();
    CFont *pfnt    = m_txtSettingsFor.GetFont();
    CFont *pfntOld = pDC->SelectObject(pfnt);
	ASSERT(pDC != (CDC *)NULL);
	CSize sizeText = pDC->GetTextExtent(strTxt);
    pDC->SelectObject(pfntOld);
	VERIFY(m_txtSettingsFor.ReleaseDC(pDC));
#if 0
	pDC = m_bnConfigs.GetDC();
	ASSERT(pDC != (CDC *)NULL);
	strTxt.LoadString(IDS_CONFIGS);
	CSize sizeButton = pDC->GetTextExtent(strTxt, strTxt.GetLength()) + CSize(4, 4); // borders
	ASSERT(sizeButton.cx <= (CX_TREECTL/2));
	ASSERT(sizeButton.cy <= (nTabHeight));
	VERIFY(ReleaseDC(pDC));
	int nEditOffset = ((1 + nTabHeight - sizeButton.cy) / 2);
	if (nEditOffset < 0)
		nEditOffset = 0;
	m_bnConfigs.SetWindowPos(NULL, margins.left + CP_OUTER_BORDER + 2, margins.top + 1 + nEditOffset, sizeButton.cx, sizeButton.cy, SWP_NOACTIVATE);
	int cbWidth = CX_TREECTL - sizeButton.cx - 1;
#else
	int nEditOffset = ((1 + nTabHeight - sizeText.cy) / 2);
	if (nEditOffset < 0)
		nEditOffset = 0;
	m_txtSettingsFor.SetWindowPos(NULL, margins.left + CP_OUTER_BORDER,
		margins.top + 1 + nEditOffset,
		sizeText.cx, sizeText.cy, SWP_NOACTIVATE);

	int cbWidth = CX_TREECTL - sizeText.cx;
#endif
	int cbHeight = rectTab.Height() - CP_OUTER_BORDER;
	m_cbConfigs.SetWindowPos(&m_txtSettingsFor, margins.left + CP_OUTER_BORDER /* + 1 */ + sizeText.cx, margins.top + nEditOffset - 1, cbWidth, cbHeight, SWP_NOACTIVATE);

	m_OptTreeCtl.SetWindowPos(&m_cbConfigs,
		margins.left + CP_OUTER_BORDER + 1, margins.top + nTabHeight + 1,
		CX_TREECTL, rectTab.Height() - CP_OUTER_BORDER, SWP_NOACTIVATE);

	// o centre the 'No Common Properties' dlg
	CRect rectNoProps;
	m_dlgNoProps.GetWindowRect(rectNoProps);
	ScreenToClient(rectTab);
	m_dlgNoProps.SetWindowPos(NULL,
		rectTabRow.left + (rectTab.Width() - rectNoProps.Width()) / 2,
		rectTabRow.top + (rectTab.Height() - rectNoProps.Height()) / 2,
		0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);

}

void CProjOptionsDlg::ExitCleanup()
{
	// remember the current category state
	RememberCategories();

	CProject * pProject = m_pProject;
#ifdef MPROJ
	// support multiple projects
	const CObList * pProjList = CProject::GetProjectList();
	POSITION pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif
		pProject->SetOkToDirtyProject();
#ifdef MPROJ
	}
#endif

}

void CProjOptionsDlg::OnOK()
{
	// UNDONE: need to ignore private props and only look at public
	// indetermining r/o handling

	// do we need to validate the tab?
	if (!ValidateCurrent())
		return;	// failed validate

	ExitCleanup();

	BOOL bSettingsChanged = FALSE;

	CProject * pProject = m_pProject;
	CString strTarget = pProject->GetTargetName();
#ifdef MPROJ
	// support multiple projects
	const CObList * pProjList = CProject::GetProjectList();
	POSITION pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif

		pProject->m_bSettingsChanged = DoBagOpOverProject(pProject, CloneBag, CurrBag, BO_Same | BO_PrivateSpecial);
		bSettingsChanged = bSettingsChanged || pProject->m_bSettingsChanged;
#ifdef MPROJ
	}
#endif

	// Flush the undo buffer
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	VERIFY (SUCCEEDED (pProjSysIFace->FlushUndo ()));
	BOOL bCancel = FALSE;
	CStringArray files;
	CPtrList paths;
	const CPath * pPath;

	LPSOURCECONTROL pSccManager;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));

#ifdef MPROJ
	// support multiple projects
	pProjList = CProject::GetProjectList();
	pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()) || (pProject->m_bProjIsExe))
			continue;
#endif

		pPath = pProject->GetFilePath();
		if ((pProject->m_bSettingsChanged==1) && !pProject->IsDirty())
		{
			
			if (pPath->IsReadOnlyOnDisk())
			{
				paths.AddTail(new CPath(*pPath));
				if (pSccManager->IsControlled((const TCHAR *)*pPath) == S_OK)
				{
					files.Add((const TCHAR*)*pPath);
				}
			}
			if (g_bAlwaysExportMakefile)
			{
				CPath path = *pPath;
				path.ChangeExtension(_T(".mak"));
				if (path.IsReadOnlyOnDisk())
				{
					paths.AddTail(new CPath(path));
					if (pSccManager->IsControlled((const TCHAR *)path) == S_OK)
					{
						files.Add((const TCHAR*)path);
					}
				}
				path.ChangeExtension(_T(".dep"));
				if ((g_bAlwaysExportDeps) && path.IsReadOnlyOnDisk())
				{
					paths.AddTail(new CPath(path));
					if (pSccManager->IsControlled((const TCHAR *)path) == S_OK)
					{
						files.Add((const TCHAR*)path);
					}
				}
			}
		}
#ifdef MPROJ
	}
#endif
	BOOL bPossibleCheckOut = FALSE;
	if (files.GetSize() > 0)
	{
		bPossibleCheckOut = (pSccManager->CheckOutReadOnly(files, TRUE, FALSE) == S_OK);
	}

	int nCount = 0;
	CString strPaths;
	while (!paths.IsEmpty())
	{
		pPath = (const CPath *)paths.RemoveHead();
		if ((!bPossibleCheckOut) || pPath->IsReadOnlyOnDisk())
		{
			if (nCount++)
				strPaths += _T("\n");

			strPaths += *pPath;
		}
		delete pPath;
	}
	// now if nCount > 0, then we have some still read-only
	if (nCount)
	{
		UINT nID = (nCount == 1) ? IDS_WARN_PROJ_READ_ONLY : IDS_WARN_PROJS_READ_ONLY;
		CString strMsg;
				
		// give them a chance to attrib or cancel
		bCancel = (MsgBox(Error, MsgText(strMsg, nID, strPaths), MB_OKCANCEL) == IDCANCEL);
#ifdef MPROJ
		pos = pProjList->GetHeadPosition();
		while (pos != NULL)
		{
			pProject = (CProject *)pProjList->GetNext(pos);
			if ((pProject==NULL) || (!pProject->IsLoaded()) || (pProject->m_bProjIsExe))
				continue;
#endif
			if (bCancel)
			{
				pProject->SetOkToDirtyProject(FALSE);
			}
			else if (pProject->m_bSettingsChanged==1)
			{
				// if OK, dirty it silently (already prompted)
				pProject->m_bProjectComplete = FALSE;
				pProject->DirtyProjectEx();
				pProject->m_bProjectComplete = TRUE;
			}
#ifdef MPROJ
		}
#endif
	}

	pSccManager->Release();
	pSccManager = NULL;

	if (bCancel)
	{
		return;
	}

// REVIEW: may need to do something here before commiting changes

#if 0
	// do this here for efficiency before making changes
#ifdef MPROJ
	// support multiple projects
	pProjList = CProject::GetProjectList();
	pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif
		// allow the project to be dirtied
		// pProject->SetOkToDirtyProject();

		pProject->SetOptionTreeCtl((COptionTreeCtl *)NULL);
#ifdef MPROJ
	}
#endif
#endif

#ifdef MPROJ
	// support multiple projects
	pProjList = CProject::GetProjectList();
	pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif
		// we OKed so do the bag copy and inform deps. of any changes
		// as well as doing comparision to see if settings have changed
		if (pProject->m_bSettingsChanged)
		{
			(void)DoBagOpOverProject(pProject, CloneBag, CurrBag, BO_Inform | BO_Copy | BO_Same | BO_UseDest);

		}
		else
		{
			(void)DoBagOpOverProject(pProject, (UINT)-1, CurrBag, BO_UseDest);
		}
 
		ASSERT((pProject->m_bSettingsChanged != 1) || (pProject->IsDirty()));

#ifdef MPROJ
	}
#endif

 	//
	// KLUDGE ALERT, other ok semantics go here
	//
	// our messages
	if (m_idOnOKMsg != (UINT)-1)
		MsgBox(Information, m_idOnOKMsg);

	CTabbedDialog::OnOK();

	// clear the ProxySlob
	m_ProxySlob.Clear(FALSE);

#ifdef MPROJ
	// support multiple projects
	pProjList = CProject::GetProjectList();
	pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif
		// reinit the target include list
		pProject->m_listTargIncl.Init((HBUILDER)pProject);
#ifdef MPROJ
	}
#endif

	if (bSettingsChanged)
		theApp.NotifyPackages(PN_BLD_SETTINGS_CHANGED);
}

void CProjOptionsDlg::OnCancel()
{
 	ExitCleanup();

	// don't save the state of the tree-control
	m_OptTreeCtl.m_fDoStateStore = FALSE;

	CProject * pProject = m_pProject;
	CString strTarget = pProject->GetTargetName();
#ifdef MPROJ
	// support multiple projects
	const CObList * pProjList = CProject::GetProjectList();
	POSITION pos = pProjList->GetHeadPosition();
	while (pos != NULL)
	{
		pProject = (CProject *)pProjList->GetNext(pos);
		if ((pProject==NULL) || (!pProject->IsLoaded()))
			continue;
#endif
		// make sure we're using the old bag
		(void) DoBagOpOverProject(pProject, CloneBag, CurrBag, BO_UseDest);

#ifdef MPROJ
	}
#endif
	//
	// KLUDGE ALERT, other cancel semantics go here
	//

	CTabbedDialog::OnCancel();

	// clear the ProxySlob
	m_ProxySlob.Clear(FALSE);
}

void CProjOptionsDlg::OnSelectTab(int nTab)
{
	// update our notion of what the current tab title is
	m_strPrevTab = nTab == -1 ? "" : m_tabRow.GetTabString(nTab);
}

CButton* CProjOptionsDlg::GetButtonFromIndex(int index)
{
	// Default is either OK/Cancel/Targets/Help or Close/Targets/Help, depending on
	// commit model.  Derived classes may override this, of course.
	if (m_commitModel != commitOnOk && (index > 0))
		index++;

	switch (index)
	{
		case 0:
			return &m_btnOk;

		case 1:
			return &m_btnCancel;

		default:
			return NULL;
	}
}

void CProjOptionsDlg::OnTreeCtlSelChange()
{
	// ignore?
	if (m_fIgnoreSelChange)
		return;

	// update our notion of what the selection is
	if (!m_bDirtyTabs && !UpdateProxySlob(FALSE))
		return;	// no selection change

	// retrieve our new list of option handlers
	CacheOptionHandlers();

	// update our option tabs
	// and inform of the selection change if they
	// is no change in the #/type of option tabs
	if (!UpdateOptionTabs())
 		m_ProxySlob.InformDependants(SN_ALL);

	// show/don't show the 'No Common Properties' dlg. on whether common list hdlrs exists.
	m_dlgNoProps.ShowWindow(m_listHdlrs.IsEmpty() ? SW_SHOW : SW_HIDE);

	// preserve our tab selection
	// decide what the default is
	int nTab = GetTabCount() > 0 ? 0 : -1;

	if (nTab != -1)
	{
		// can we find old tab?
		int nGotTab = -1;
		for (int i = 0 ; i <= m_tabRow.MaxTab() ; i++)
		{
			if (m_strPrevTab == m_tabRow.GetTabString(i))
				nGotTab = i;
		}

		if (nGotTab != -1)
			nTab = nGotTab;	// found

		BOOL fTabNotActive = nTab != m_nTabCur;
		if (fTabNotActive)
		{
			// select this tab, and remember the tab title
			// (preserve focus in tree-control)	
			BOOL fTreeHadFocus = m_OptTreeCtl.HasFocus();

			m_tabRow.SetActiveTab(nTab);
			SelectTab(nTab);	// tab-row gets the focus

			if (fTreeHadFocus && !m_OptTreeCtl.HasFocus())
				m_OptTreeCtl.SetFocus();	// keep our original focus	
		}
		// update the state of the tab?
		// this happens in ActivateTab()
		else // !fTabNotActive
		{
			CDlgTab * pTab = GetTab(nTab);
			if (pTab->IsKindOf(RUNTIME_CLASS(CToolOptionTab)))
			{
				((CToolOptionTab *)pTab)->UpdateState();
			}
			else if (pTab->IsKindOf(RUNTIME_CLASS(CSlobPageTab)))
			{
				((CSlobPageTab *)pTab)->InitPage();
			}
		}
	}
}

void CProjOptionsDlg::CacheOptionHandlers()
{
	// empty our handler cache
	m_listHdlrsCache.RemoveAll();

	// UNDONE: need support for multiple projects
	if ((m_pProject==NULL) || (!m_pProject->m_bProjIsExe))
	{
		// internal target

		// get our available tools for the selection
		CPtrList * * rglistTools = (CPtrList * *)NULL;
		int nSelIndex, nSelCount = m_ProxySlob.m_ptrList.GetCount();

		if (nSelCount != 0)
		{
			if ((rglistTools = new CPtrList *[nSelCount]) == (CPtrList * *)NULL)
				return;

			POSITION pos = m_ProxySlob.m_ptrList.GetHeadPosition();
			nSelIndex = 0;
			while (pos != (POSITION)NULL)
			{
				rglistTools[nSelIndex] = new CPtrList;
				((OptTreeNode *)m_ProxySlob.m_ptrList.GetNext(pos))->GetViableBuildTools(*rglistTools[nSelIndex++]);
			}
		}

		// any handlers for selection?
		BOOL fAtLeastOneHdlr = FALSE;

		// get our common handlers across all of the tools
		// in the selection
		CPtrList m_listHdlrsScratch;
		for (nSelIndex = 0; nSelIndex < nSelCount; nSelIndex++)
		{
			m_listHdlrsScratch.RemoveAll();
			POSITION pos = rglistTools[nSelIndex]->GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				COptionHandler * popthdlr = ((CBuildTool *)rglistTools[nSelIndex]->GetNext(pos))->GetOptionHandler();
				if (popthdlr != (COptionHandler *)NULL)
				{ 
					// got at least one handler non custom tool opthdlr. in the selection?
					if (!popthdlr->m_pAssociatedBuildTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
						fAtLeastOneHdlr = TRUE;

					// is this a common handler or are we the first batch?
					if (nSelIndex == 0 || m_listHdlrsCache.Find(popthdlr))
					{
						ASSERT_VALID(popthdlr);
						m_listHdlrsScratch.AddTail(popthdlr);
					}
					// can we find a common base-option handler?
					// for cross-platform support
					else
					{
						BOOL fMatched = FALSE;
						
						// loop through handlers in cache
						POSITION pos = m_listHdlrsCache.GetHeadPosition();
						while (!fMatched && pos != (POSITION)NULL)
						{
							COptionHandler * popthdlrCache = (COptionHandler *)m_listHdlrsCache.GetNext(pos);
							// loop through base-handlers from cache
							while (!fMatched && popthdlrCache != (COptionHandler *)NULL)
							{
								// loop through base-handlers from current
								COptionHandler * popthdlrCurr = popthdlr;
								while (popthdlrCurr != (COptionHandler *)NULL)
								{
									if (popthdlrCurr == popthdlrCache)
									{
										ASSERT_VALID(popthdlrCurr);
										m_listHdlrsScratch.AddTail(popthdlrCurr);
										fMatched = TRUE;
										break;
									}
									popthdlrCurr = popthdlrCurr->GetBaseOptionHandler();
								}
								popthdlrCache = popthdlrCache->GetBaseOptionHandler();
							}
						}
					}
				}
			}

			// copy our scratch into our actual
			m_listHdlrsCache.RemoveAll();
			m_listHdlrsCache.AddTail(&m_listHdlrsScratch);
		}

		// determine if there's a common project type and if that
		// common project type is Java. If the common project is Java,
		// we'll show a different debug and general tab. If there is a
		// mix of Java and other project types, we won't show the
		// debug tab nor the general tab.
		// (note that this code follows the same model as that used to
		// determine whether to show the external project general tab)
		BOOL bSelectionContainsAtLeastOneJava = FALSE;
		BOOL bSelectionContainsGeneric = FALSE;
		BOOL bSelectionIsAllJava = TRUE;
		BOOL bAtLeastOne = FALSE;
		
		for (POSITION posNode = m_ProxySlob.GetHeadPosition(); posNode != NULL; )
		{
			bAtLeastOne = TRUE;

			// get each option tree node from the selection
			OptTreeNode* pNode = m_ProxySlob.GetNext(posNode);

			// get the project type
			CProjType* pProjType = pNode->GetProjType();

			// is the project type Java?
			if (pProjType != NULL && pProjType->GetUniquePlatformId() == java)
				// there's at least one Java project selected
				bSelectionContainsAtLeastOneJava = TRUE;
			else
				// the selection is not all Java
				bSelectionIsAllJava = FALSE;

			if (pProjType != NULL && pProjType->GetUniqueTypeId() == CProjType::generic)
				bSelectionContainsGeneric = TRUE;
		}
		if (!bAtLeastOne)
			bSelectionIsAllJava = FALSE;

		// determine if the selection is mixed (i.e. C++ and Java)
		BOOL bSelectionIsMixed = !bSelectionIsAllJava && bSelectionContainsAtLeastOneJava;

		// does it have the 'Debugging Options' page?
		if (m_ProxySlob.IsSortOf(RUNTIME_CLASS(CProject)))
		{
			// determine which debug tab will be displayed
			// (C++ or Java) based on the project nodes selected

			// if there is a mix of nodes (C++ and Java) selected,
			// no debug tab will be shown

			// is the selection mixed?
			if (!bSelectionIsMixed && bAtLeastOne)
			{
				// display the appropriate debug tab
				m_listHdlrsCache.AddHead(bSelectionIsAllJava ? OPTHDLR_DEBUG_JAVA : OPTHDLR_DEBUG);
			}
		}

		// Get the common projtype and if this is
		// an external target type then we have the
		// external target type general page
		bAtLeastOne = FALSE;
		BOOL bNotAllExternal = FALSE;
		
		CProjType * pProjType;
 		POSITION pos = m_ProxySlob.GetHeadPosition();
		while (pos)
		{
			OptTreeNode* pNode = m_ProxySlob.GetNext(pos);
			pProjType = pNode->GetProjType();
			if (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
				bAtLeastOne = TRUE;
			else
				bNotAllExternal = TRUE;
		}

		// does it have the Tool 'General' page or the
		// External project 'General' page
		if (m_ProxySlob.IsSortOf(RUNTIME_CLASS(CProject)))
		{
			// are only external projects selected?
			if (bAtLeastOne && !bNotAllExternal)
			{
				// if the selected projects are mixed (i.e. C++ and Java)
				// don't show any General tab
				if (!bSelectionIsMixed)
					// external target options - first tab
					m_listHdlrsCache.AddHead(OPTHDLR_EXTTARG_OPTS);
			}
			// are only internal projects selected?
			else if (!bAtLeastOne && bNotAllExternal)
			{
				// if the selected projects are mixed (i.e. C++ and Java),
				// don't show any General tab
				if (!bSelectionIsMixed)
					// internal target options - first tab
					m_listHdlrsCache.AddHead(bSelectionIsAllJava ? OPTHDLR_JAVA_TOOL_GENERAL : OPTHDLR_TOOL_GENERAL);

				// always have a custom build for the target
				// custom build - last tab

				if (!bSelectionIsMixed && !bSelectionIsAllJava && !bSelectionContainsGeneric)
					m_listHdlrsCache.AddTail(OPTHDLR_CUSTOM_BUILD);

				// internal target options - Pre-link, Post-build tab.
				if (!bSelectionIsMixed && !bSelectionIsAllJava && !bSelectionContainsGeneric)
					m_listHdlrsCache.AddHead(OPTHDLR_PRELINK);

				m_listHdlrsCache.AddHead(OPTHDLR_POSTBUILD);

			}
  		}
		// does it have the Tool 'General' and 'Custom Build' page?
		else if (m_ProxySlob.IsSortOf(RUNTIME_CLASS(CFileItem)) && !m_ProxySlob.IsSortOf(RUNTIME_CLASS(CProjectDependency)))
		{
			// if the selected projects are mixed (i.e. C++ and Java),
			// don't show any General tab
			if (!bSelectionIsMixed)
				// display the appropriate general tab
				m_listHdlrsCache.AddHead(bSelectionIsAllJava ? OPTHDLR_JAVA_TOOL_GENERAL : OPTHDLR_TOOL_GENERAL);

			// only have a custom build for a file if no
			// files in the selection have assoc. tools already
			// and this isn't an external target
			if (!fAtLeastOneHdlr && !bAtLeastOne && bNotAllExternal)
			{
				// custom build - last tab
				m_listHdlrsCache.AddTail(OPTHDLR_CUSTOM_BUILD);
			}
		}

		// delete our tool lists
		if (rglistTools)
		{
			for (nSelIndex = 0; nSelIndex < nSelCount; nSelIndex++)
				delete rglistTools[nSelIndex];
			delete [] rglistTools;
		}
	}
	else
	{
		// executable

		BOOL fDebugTarget = TRUE;	// can we debug this?

 		CString strUIDescription;

		CPlatform * pPlatform;
		if (m_ProxySlob.GetStrProp(P_ExtOpts_Platform, strUIDescription) == valid)
		{
			g_prjcompmgr.LookupPlatformByUIDescription(strUIDescription, pPlatform);

			// can only debug exes for supported platforms
			fDebugTarget = pPlatform->IsSupported();
		}

		if (fDebugTarget)
		{
			// determine which debug tab will be displayed
			// (C++ or Java) based on the platform
			ASSERT(m_pProject);
			ASSERT(m_pProject->GetCurrentPlatform() != NULL);
			if (m_pProject->GetCurrentPlatform()->GetUniqueId() == java)
			{
				// display the Java debug tab
				m_listHdlrsCache.AddHead(OPTHDLR_DEBUG_JAVA);
			}
			else
			{
				// display the C++ debug tab
				m_listHdlrsCache.AddHead(OPTHDLR_DEBUG);
			}
		}

		m_listHdlrsCache.AddTail(OPTHDLR_EXT_OPTS);
	}
}

void CProjOptionsDlg::ValidateContent(OptTreeNode * pNode)
{
	// then create the content
	if (!pNode->m_fValidContent)
	{
		OptTreeNode * pNewOptNode;
		CString cstr;
		POSITION pos = pNode->pItem->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CProjItem * pChildItem = (CProjItem *)pNode->pItem->GetNext(pos);
			if (!m_OptTreeCtl.IsValidOptTreeItem(pChildItem, cstr))
				continue;	// not valid, next!

			// construct our new OptTreeNode for this child item
			// we'll destroy this when we free the target it belongs to
			pNewOptNode = new OptTreeNode(pNode, pChildItem);
			ASSERT(pNewOptNode->pcr != NULL);
			// recurse if this is another group
			if (pChildItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
				ValidateContent(pNewOptNode);
		}

		// content is now valid
		pNode->m_fValidContent = TRUE;
	}
}

void CProjOptionsDlg::AddContent(OptTreeNode * pNode, CPtrList & lstSelectedNodes)
{
	// add this item's content to our selection (CProxySlob)
	POSITION pos = pNode->m_Content.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pChildNode = (OptTreeNode *)pNode->m_Content.GetNext(pos);
		CProjItem * pChildItem = (CProjItem *)pChildNode->pItem;

		if (pChildItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			// recurse if this is another group
			AddContent(pChildNode, lstSelectedNodes);
		}
		else
		{
			// add the child item, this is a file
			lstSelectedNodes.AddHead((void *)pChildNode);
		}
	}
}

BOOL CProjOptionsDlg::UpdateProxySlob(BOOL fInform)
{
	// selection change?
	BOOL fSelChange = FALSE;
	
	// update the Proxy Slob's selection (this may be multiple)
	int nSelCount = m_OptTreeCtl.GetSelCount();
	if (nSelCount == 0)
	{
		// no selection, definitely a selection change!
		m_ProxySlob.Clear(FALSE);
		fSelChange = TRUE;
	}
	else
	{
		// what is our new selection?
		POSITION pos;
		CPtrList lstSelectedNodes;

		INT * rgInt = new INT[nSelCount];
		if (rgInt != (INT *)NULL && m_OptTreeCtl.GetSelItems(nSelCount, rgInt) != LB_ERR)
		{
			for (int nSelItem = nSelCount; nSelItem > 0; nSelItem--)
			{
#ifdef NEWCFG
				OptTreeNodeList	* pNodeList = (OptTreeNodeList *) m_OptTreeCtl.GetData(rgInt[nSelItem-1]);
				int nConfigs = 0;
				POSITION pos1 = pNodeList->GetHeadPosition();
				ASSERT(pos1);
				while (pos1 != NULL)
				{
					OptTreeNode * pNode = pNodeList->GetNext(pos1);
#else
					OptTreeNode	* pNode = (OptTreeNode *) m_OptTreeCtl.GetData(rgInt[nSelItem-1]);
#endif
					// is this a project group, if so then add all of it's children and not itself
					if (pNode->pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
					{
						// make sure this item's content is valid, if it isn't
						// then create the content
						ValidateContent(pNode);

						if (!pNode->IsValid()) // only add the selected ones
							continue;

						// add this item's content to our selection (CProxySlob)
						AddContent(pNode, lstSelectedNodes);
					}
					else
					{
						if (!pNode->IsValid()) // only the selected ones
							continue;

						// add the item, this is a project or file
						lstSelectedNodes.AddTail((void *)pNode);
					}

#ifdef NEWCFG
					nConfigs++;
				}
				ASSERT(nConfigs > 0);
#endif
			}

			// now compare this selection with what we have currently
			fSelChange = lstSelectedNodes.GetCount() != m_ProxySlob.GetCount();
			pos = lstSelectedNodes.GetHeadPosition();
			while (!fSelChange && pos != (POSITION)NULL)
			{
				// currently selected?
				if (!m_ProxySlob.Selected((OptTreeNode *)lstSelectedNodes.GetNext(pos)))
					// no
					fSelChange = TRUE;
			}
		}
		else
		{
			ASSERT(FALSE);		// mem. alloc. or selection retrieve failure
			fSelChange = TRUE; 	// clr. just in case..;-)
		}

		if (rgInt != (INT *)NULL)
			delete [] rgInt;

		if (fSelChange)
		{
   			m_ProxySlob.Clear(FALSE);
			pos = lstSelectedNodes.GetHeadPosition();
			while (pos != (POSITION)NULL)
				m_ProxySlob.Add((OptTreeNode *)lstSelectedNodes.GetNext(pos), FALSE);
		}
	}
	
	// do we need to inform now?
	if (fInform && fSelChange)
		m_ProxySlob.InformDependants(SN_ALL);

	// any change in the selection?
	return fSelChange;
}

void CProjOptionsDlg::RememberCategories()
{
	int nMaxTab = GetTabCount();
	for (int nTab = 0; nTab < nMaxTab; nTab++)
	{
		CString strTitle;
		CDlgTab * pTab = GetTab(nTab);

		// Debug tab now has mini-pages
		if (pTab->IsKindOf(RUNTIME_CLASS(COptsDebugTab))) {
			if (((COptsDebugTab *)pTab)->GetCurrentMiniPage(strTitle))
				m_mapLastMiniPage.SetAt(*((COptsDebugTab *)pTab)->GetCaption(), strTitle);
		}

		if (!pTab->IsKindOf(RUNTIME_CLASS(CToolOptionTab)))
			continue;

		if (((CToolOptionTab *)pTab)->GetCurrentMiniPage(strTitle))
			m_mapLastMiniPage.SetAt(*((CToolOptionTab *)pTab)->GetCaption(), strTitle);
	}
}

void CProjOptionsDlg::GetMargins(CRect & rect)
{
	CTabbedDialog::GetMargins(rect);

	rect.left += CX_TREECTL + CX_TREE_BORDER + CP_OUTER_BORDER * 2;
}

class CDlgStandard : public CDialog
{
public:
	// use the Tool 'General' page as the 'standard' sized page
	CDlgStandard() : CDialog(IDDP_TOOL_GENERAL) {}
	__inline BOOL GetStandardRect(CRect & rect)
	{
		C3dDialogTemplate dt;
		if (dt.Load(m_lpszTemplateName))
			SetStdFont(dt);
		if (!CreateIndirect(dt.GetTemplate()))	// style is NOT VISIBLE
			return FALSE;
		GetWindowRect(rect);
		DestroyWindow();
		return TRUE;
	}
};

void CProjOptionsDlg::GetTabSize(CRect & rect)
{
	CDlgStandard dlg;
	if (!dlg.GetStandardRect(rect))
	{
		// we failed (this will at least provide 'blank'
		// space, I imagine other page tabs will fail creation)
		rect.top = rect.left = 0; rect.right = rect.bottom = 200;
	}
}

int CProjOptionsDlg::DoBagOpOverProject(CProject * pProject, UINT idSrcBag, UINT idDestBag, UINT bagop)
{																		
	// Take a snapshot of our project props
	// do this for all configs for all project items in the project
	CObList obList;
	pProject->FlattenSubtree(obList, CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies);

	// 0 = no change, 1 = public props change, 2 = only private props change
	int fSettingsChanged = FALSE;  // pseudo-boolean

	const CPtrArray * pCfgProject = pProject->GetConfigArray();
	int size = pCfgProject->GetSize();

	CProjTempConfigChange projTempConfigChange(pProject);
	ConfigurationRecord * pcrCurrent = pProject->GetActiveConfig();

	for (int i = 0; i <= size; i++)
	{
		ConfigurationRecord * pcrProject;

		// do the current pcr last
		if (i == size)
		{
			pcrProject = pcrCurrent;
		}
		else
		{
			pcrProject = (ConfigurationRecord *)pCfgProject->GetAt(i);

			// skip current pcr, we'll do it last
			if (pcrProject == pcrCurrent)
				continue;
		}

		// set the project's active config.
		projTempConfigChange.ChangeConfig(pcrProject);

		BOOL fTargetSetsChng = FALSE;
		if (bagop & BO_Same)
		{
			// have the target-level options changed?
			int retval = pcrProject->BagSame (idSrcBag, idDestBag, (UINT)-1, (UINT)-1, TRUE, ((bagop & BO_PrivateSpecial)==0));
			fTargetSetsChng = (retval == 0);
			if (fTargetSetsChng)
			{
				// target-level action options have possibly changed
				CActionSlobList * pActions = pcrProject->GetActionList();
				POSITION pos = pActions->GetHeadPosition();
				while (pos != (POSITION)NULL)
					((CActionSlob *)pActions->GetNext(pos))->m_fPossibleOptionChange = TRUE;

				// settings have changed
			}

			// REVIEW: make these compatible
			// convert from bagsame retval to actual
			if ((fSettingsChanged != 1) && (retval != 1))
			{
				if (fSettingsChanged == 0)
				{
					fSettingsChanged =  1 - retval;
				}
				else if (retval == 0)
				{
					ASSERT(fSettingsChanged==2);
					fSettingsChanged = 1;
				}
			}
		}

		POSITION pos = obList.GetHeadPosition();
		while (pos != NULL)
		{
			CProjItem * pItem = (CProjItem *)obList.GetNext(pos);
			CTargetItem * pTarget = pItem->GetTarget();

			// Ignore configs that don't pertain to the items target.
			if (pTarget != NULL )
			{
				const CPtrArray * pArrayCfg  = pTarget->GetConfigArray();
				int j ;
				ConfigurationRecord * pcr ;

				for ( j = 0 ; j < pArrayCfg->GetSize() ; j++)
				{
					pcr = (ConfigurationRecord *)pArrayCfg->GetAt(j);
					if (pcrProject->m_pBaseRecord == pcr->m_pBaseRecord)
						break;
				}

				if ( j == pArrayCfg->GetSize() || !pcr->IsValid())
					continue;
			}

			ConfigurationRecord * pcrItem = pItem->GetActiveConfig();
			ASSERT(pcrItem != (ConfigurationRecord *)NULL);
	
			if (bagop & BO_Same)
			{
				int retval = 1 - fSettingsChanged; // default
				if (fTargetSetsChng || !(retval = pcrItem->BagSame (idSrcBag, idDestBag, (UINT)-1, (UINT)-1, TRUE, ((bagop & BO_PrivateSpecial)==0))))
				{
					// item-level action options have possibly changed
					CActionSlobList * pActions = pcrItem->GetActionList();
					POSITION pos = pActions->GetHeadPosition();
					while (pos != (POSITION)NULL)
						((CActionSlob *)pActions->GetNext(pos))->m_fPossibleOptionChange = TRUE;

					// settings have changed
				}
				// REVIEW: make these compatible
				// convert from bagsame retval to actual
				if ((fSettingsChanged != 1) && (retval != 1))
				{
					if (fSettingsChanged == 0)
					{
						fSettingsChanged =  1 - retval;
					}
					else if (retval == 0)
					{
						ASSERT(fSettingsChanged==2);
						fSettingsChanged = 1;
					}
				}
			}

			if (bagop & BO_UseDest)
			{
				// have the item use the dest. bag as the current bag

				// special logic for Cancel:
				BOOL bOldNoTool = -1;
				if (bagop == BO_UseDest && idSrcBag == CloneBag)  // Cancel case
				{
					if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
					{
							// need to check the clone bag to see if it has been changed
							pItem->UsePropertyBag(idSrcBag);
							if (pItem->GetIntProp(P_ItemIgnoreDefaultTool, bOldNoTool) != valid)
							bOldNoTool = -1;
					}
				}

				pItem->UsePropertyBag(idDestBag);

				// more special logic for Cancel case
				if (bOldNoTool != -1)
				{
					BOOL bNoTool;
					if ((pItem->GetIntProp(P_ItemIgnoreDefaultTool, bNoTool) == valid) &&
						(bNoTool != bOldNoTool))
					{
						CActionSlob::UnAssignActions(pItem, NULL, pcrProject);
						CActionSlob::AssignActions(pItem, NULL, pcrProject);
					}
				}
			}

			if (bagop & BO_Copy)
			{	
				// copy into the dest. bag
				pcrItem->BagCopy(idSrcBag, idDestBag, (UINT)-1, (UINT)-1, bagop & BO_Inform);
			}

			if (bagop & BO_Clear)
			{
				// clear the dest. bag
				pcrItem->BagEmpty(idDestBag);
			}
		}
	}

	return fSettingsChanged;	// ok
}

BOOL CProjOptionsDlg::UpdateOptionTabs()
{
	POSITION pos;

	// has the number or type of handlers changed?
	BOOL fNoHdlrChange = m_listHdlrs.GetCount() == m_listHdlrsCache.GetCount();

	pos = m_listHdlrsCache.GetHeadPosition();
	while (fNoHdlrChange && (pos != (POSITION)NULL))
	{
		if (!m_listHdlrs.Find(m_listHdlrsCache.GetNext(pos)))
			// a change in type of handler
			fNoHdlrChange = FALSE;
	}

	// a change in number or type of tabs!
	if (fNoHdlrChange)
		return FALSE;	// no change

	// remember the state of all of our current tool option tabs
	RememberCategories();

	// remove all the tabs we currently have
	if (!ClearAllTabs())
		return FALSE;	// can't do this

	// add the new tabs we'll have (maybe none!)
	BOOL fOk = TRUE;

	pos = m_listHdlrsCache.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		// our tab to add into the 'mondo' dialog
		CDlgTab * pTab;

		// get the option handler
		COptionHandler * popthdlrTool = (COptionHandler *)m_listHdlrsCache.GetNext(pos);

		// is this a 'pseudo' option handler?
		if (popthdlrTool == OPTHDLR_TOOL_GENERAL)
		{	
			// Tool 'General' page
			pTab = (CDlgTab *) new CToolGeneralPage;
			((CSlobPageTab *)pTab)->SetupPage(&m_ProxySlob);
			pTab->m_nOrder = 10;
		}
		else if (popthdlrTool == OPTHDLR_CUSTOM_BUILD)
		{
			// 'Custom Build' page
			pTab = (CDlgTab *)new CCustomBuildPage;
			((CCustomBuildPage *)pTab)->SetupPage(&m_ProxySlob);
			pTab->m_nOrder = 95;	// always just before options pages
		}
		else if (popthdlrTool == OPTHDLR_DEBUG)
		{
			// Project debug options page
			pTab = (CDlgTab *) new COptsDebugTab;
			((CSlobPageTab *)pTab)->SetupPage(&m_ProxySlob);
			pTab->m_nOrder = 20;

			// add our 'General' tab to the tool tab
			COptionMiniPage * pPage = new CDebugGeneralPage;
			if (pPage == (CDebugGeneralPage *)NULL)	
				continue;
			pPage->SetupPage(&m_ProxySlob);
			((COptsDebugTab *)pTab)->AddOptionPage(pPage);

			// add our 'Additional Dlls' tab to the tool tab
			pPage = new CDebugAdditionalDllPage;
			if (pPage == (CDebugAdditionalDllPage *)NULL)	
				continue;
			pPage->SetupPage(&m_ProxySlob);
			((COptsDebugTab *)pTab)->AddOptionPage(pPage);
			
			// make it come up with the last mini-page it had when last created
			CString strCurrentMiniPage;
			if (m_mapLastMiniPage.Lookup(*((COptsDebugTab *)pTab)->GetCaption(), strCurrentMiniPage))
				((COptsDebugTab *)pTab)->SetCurrentMiniPage(strCurrentMiniPage);
		}
		// Java: show the Java debug tab
		else if (popthdlrTool == OPTHDLR_DEBUG_JAVA)
		{
			// Project debug options page
			pTab = (CDlgTab *) new COptsDebugTab;
			((CSlobPageTab *)pTab)->SetupPage(&m_ProxySlob);
			pTab->m_nOrder = 22;

			// Java: add the Java 'General' page
			COptionMiniPage* pPage = new CDebugJavaGeneralPage;
			if (pPage == (CDebugJavaGeneralPage*)NULL)
				continue;
			pPage->SetupPage(&m_ProxySlob);
			((COptsDebugTab*)pTab)->AddOptionPage(pPage);

			// add the Java browser page tab
			pPage = new CDebugJavaBrowserPage;
			if (pPage == (CDebugJavaBrowserPage*)NULL)
				continue;
			pPage->SetupPage(&m_ProxySlob);
			((COptsDebugTab*)pTab)->AddOptionPage(pPage);

			// add the Java stand-alone page tab
			pPage = new CDebugJavaStandalonePage;
			if (pPage == (CDebugJavaStandalonePage*)NULL)
				continue;
			pPage->SetupPage(&m_ProxySlob);
			((COptsDebugTab*)pTab)->AddOptionPage(pPage);

			// add the Java 'Additional classes' page to the debug tab
			pPage = new CDebugJavaAddlClassesPage;
			if (pPage == (CDebugJavaAddlClassesPage*)NULL)	
				continue;
			pPage->SetupPage(&m_ProxySlob);
			((COptsDebugTab*)pTab)->AddOptionPage(pPage);

			// make it come up with the last mini-page it had when last created
			CString strCurrentMiniPage;
			if (m_mapLastMiniPage.Lookup(*((COptsDebugTab*)pTab)->GetCaption(), strCurrentMiniPage))
				((COptsDebugTab*)pTab)->SetCurrentMiniPage(strCurrentMiniPage);
		}
		else if (popthdlrTool == OPTHDLR_EXT_OPTS)
		{
			// External project general options page
			pTab = (CDlgTab *) new CExtOptsPage;
			((CSlobPageTab *)pTab)->SetupPage(&m_ProxySlob);
			pTab->m_nOrder = 10;
		}
		else if (popthdlrTool == OPTHDLR_EXTTARG_OPTS)
		{
			// External project general options page
			pTab = (CDlgTab *) new CExtTargetOptsPage;
			((CSlobPageTab *)pTab)->SetupPage(&m_ProxySlob);
			pTab->m_nOrder = 10;
		}
		else if (popthdlrTool == OPTHDLR_PRELINK)
		{
			// 'Pre-link step' page
			pTab = (CDlgTab *)new CPreLinkPage;
			((CPreLinkPage *)pTab)->SetupPage(&m_ProxySlob);
			pTab->m_nOrder = 100;
		}
		else if (popthdlrTool == OPTHDLR_POSTBUILD)
		{
			// 'Post-build step' page
			pTab = (CDlgTab *)new CPostBuildPage;
			((CPostBuildPage *)pTab)->SetupPage(&m_ProxySlob);
			pTab->m_nOrder = 110;
		}
		else if (popthdlrTool == OPTHDLR_JAVA_TOOL_GENERAL)
		{	
			// Java tool 'General' page
			pTab = (CDlgTab *) new CJavaToolGeneralPage;
			((CSlobPageTab *)pTab)->SetupPage(&m_ProxySlob);
			pTab->m_nOrder = 10;
		}
		else
		{
#ifdef _DEBUG
			// make sure we don't miss any 'pseudo' pages
			AfxIsValidAddress(popthdlrTool, sizeof (COptionHandler));
#endif
			// construct our list of base-option handlers with option page tabs first
			CPtrList lstOptHdlr;
			COptionHandler * popthdlr = popthdlrTool;

			BOOL fUseBasePages = TRUE;	// default is used base pages
			while (fUseBasePages && popthdlr != (COptionHandler *)NULL)
			{
				CRuntimeClass * * rgTabs = popthdlr->GetOptionPages(fUseBasePages);

				// no tab-list OR empty tab-list?
				if (rgTabs != (CRuntimeClass * *)NULL && rgTabs[0] != (CRuntimeClass *)NULL)
					lstOptHdlr.AddHead((void *)popthdlr);

				popthdlr = popthdlr->GetBaseOptionHandler();
			}

			// any option page tabs?
			if (lstOptHdlr.IsEmpty())
				continue;	// no
	
			// create our tool tab
			pTab = (CDlgTab *) new CToolOptionTab(popthdlrTool->GetNameID());
			if (pTab == (CToolOptionTab *)NULL)
				continue;

			((CToolOptionTab *)pTab)->SetOptionHandler(popthdlrTool);

			CRuntimeClass * pTabClass;
			POSITION pos = lstOptHdlr.GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				popthdlr = (COptionHandler *)lstOptHdlr.GetNext(pos);

				// runtime classes of our option page tabs
				BOOL fDummy;
				CRuntimeClass * * rgTabs = popthdlr->GetOptionPages(fDummy);

				COptionMiniPage * pPage;
				for (int i = 0; (pTabClass = rgTabs[i]) != (CRuntimeClass *)NULL; i++)
				{
					// the CToolOptionTab will delete these
					if (pPage = (COptionMiniPage *)pTabClass->CreateObject())
					{
						// give the page the correct slob and option handler
						pPage->SetupPage(&m_ProxySlob, popthdlr);
						((CToolOptionTab *)pTab)->AddOptionPage(pPage);
					}
				}
			}

			// make it come up with the last mini-page it had when last created
			CString strCurrentMiniPage;
			if (m_mapLastMiniPage.Lookup(*((CToolOptionTab *)pTab)->GetCaption(), strCurrentMiniPage))
				((CToolOptionTab *)pTab)->SetCurrentMiniPage(strCurrentMiniPage);
		}

		// finally add this tool tab to the options 'mondo' dialog
		AddTab(pTab);
	}

	// remember this as the previous list
	m_listHdlrs.RemoveAll();
	m_listHdlrs.AddTail(&m_listHdlrsCache);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CConfigSelect dialog

CConfigSelectDlg::CConfigSelectDlg(CWnd* pParent /*=NULL*/)
	: C3dDialog(CConfigSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigSelectDlg)
	//}}AFX_DATA_INIT
}


void CConfigSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigSelectDlg)
	DDX_Control(pDX, IDC_TARGET_LIST, m_lbConfigs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigSelectDlg, C3dDialog)
	//{{AFX_MSG_MAP(CConfigSelectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CConfigSelectDlg::OnInitDialog() 
{
	VERIFY(m_lbConfigs.SubclassDlgItem(IDC_TARGET_LIST, this));
	m_lbConfigs.SetRedraw(FALSE);

	if (UseWin4Look())
		m_lbConfigs.SetFont(GetStdFont(font_Normal));
	else
		m_lbConfigs.SetFont(GetStdFont(font_Bold));

	CString strCurrentFlavour;
	POSITION pos = g_theConfigEnum.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CEnumerator	* pEnum = g_theConfigEnum.GetNext(pos);
		if (pEnum->val)	// supported project type?
		{
			CString strFlavour;
			// REVIEW: consider supporting multiple projects
			CString strConfig = pEnum->szId;
			int nTargetLen = strConfig.Find(" - ");
			ASSERT(nTargetLen > 0);
			strFlavour = strConfig.Mid(nTargetLen + 3);
			if (m_lbConfigs.FindStringExact(-1, strFlavour)==LB_ERR)
			{
				int index = m_lbConfigs.AddString(strFlavour);
				ASSERT(index != LB_ERR);
				if (CProjOptionsDlg::IsActiveFlavour(strFlavour))
				{
					m_lbConfigs.SetCurSel(index);
					m_lbConfigs.SetCheck(index, TRUE);
				}
			}
		}
	}

	m_lbConfigs.SetRedraw(TRUE);

	C3dDialog::OnInitDialog();  // do this last to avoid flashing listbox

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigSelectDlg::OnOK()
{
	m_SelState = same; // default
	BOOL bSelChange = FALSE;
	BOOL bOldCheck = FALSE;
	int i, nConfigs = 0;
	CString strFlavour;
	int count = m_lbConfigs.GetCount();
	ASSERT(count);
	for (i = 0; i < count; i++)
	{
		if (!bSelChange)
		{
			m_lbConfigs.GetText(i, strFlavour);
			bOldCheck = (CProjOptionsDlg::IsActiveFlavour(strFlavour));
		}
		
		
		if (m_lbConfigs.GetCheck(i))
		{
			if ((!bSelChange) && !bOldCheck)
				bSelChange = TRUE;
			nConfigs++;
		}
		else if ((!bSelChange) && bOldCheck)
			bSelChange = TRUE;
	}
	if (nConfigs==0)
	{
		MsgBox(Error, IDS_NO_CONFIGS_SELECTED);
		m_lbConfigs.SetFocus();
		return;
	}
	if (bSelChange)
	{
		CProjOptionsDlg::ResetActiveFlavours();
		for (i = 0; i < count; i++)
		{
			if (m_lbConfigs.GetCheck(i))
			{
				m_lbConfigs.GetText(i, strFlavour);
				CProjOptionsDlg::SetActiveFlavour(strFlavour);
			}
		}
		if (nConfigs == 1)
			m_SelState = single;
		else if (nConfigs == count)
			m_SelState = all;
		else
			m_SelState = multi;
	}

	C3dDialog::OnOK();
}
