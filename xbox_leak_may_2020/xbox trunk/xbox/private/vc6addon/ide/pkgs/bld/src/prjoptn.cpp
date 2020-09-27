//
// COptionTable
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "mbctype.h"

IMPLEMENT_DYNAMIC(OptTreeNodeList, CPtrList);
IMPLEMENT_DYNAMIC(CProxySlob, CProjItem);
IMPLEMENT_DYNAMIC(COptionHandler, CBldSysCmp);
IMPLEMENT_DYNAMIC(COptHdlrUnknown, COptionHandler);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CProjComponentMgr g_prjcompmgr;

CMapPtrToPtr OptTreeNode::m_ValidPcrMap;

//////////////////////////////////////////////////////////////////////////////
OptTreeNodeList::~OptTreeNodeList()
{
	while (!m_lstDeps.IsEmpty())
	{
		delete (OptTreeNodeList *) m_lstDeps.RemoveHead();
	}
}

void OptTreeNodeList::RemoveContent()
{
	while (!IsEmpty())
	{
		delete (OptTreeNode *) RemoveHead();
	}

	while (!m_lstDeps.IsEmpty())
	{
		delete (OptTreeNodeList *) m_lstDeps.RemoveHead();
	}
}

OptTreeNodeList * OptTreeNodeList::AddDependentList() 
{ 
	OptTreeNodeList * pDepList = new OptTreeNodeList; 
	m_lstDeps.AddTail((void *)pDepList); 
	return pDepList; 
}

OptTreeNodeList * OptTreeNodeList::GetDependentList(const CProjItem * pItem)
{
	OptTreeNodeList * pList;
	POSITION pos = m_lstDeps.GetHeadPosition();
	while (pos != NULL)
	{
		pList = (OptTreeNodeList *)m_lstDeps.GetNext(pos);
		if (pList->GetItem() == pItem)
			return pList;
	}
	return NULL;
}

OptTreeNode * OptTreeNodeList::GetFirstValidNode()
{
	OptTreeNode * pNode;
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		pNode = GetNext(pos);
		if (pNode->IsValid())
			return pNode;
	} 
	ASSERT(0);
	return NULL;
}

CProjType * OptTreeNodeList::GetProjType()
{
	CProjType * pProjType = NULL;
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		OptTreeNode * pNode = GetNext(pos);
		if (pNode->IsValid())
		{
			if (pProjType == NULL)
				pProjType = pNode->GetProjType();
			else if (pProjType != pNode->GetProjType())
				return NULL;
		}
	}
	return pProjType;
}

BOOL OptTreeNodeList::SetIntProp(UINT idProp, int val, OptBehaviour optbeh)
{
	BOOL retval = TRUE;
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		OptTreeNode * pNode = GetNext(pos);
		if (pNode->IsValid())
		{
			retval = pNode->SetIntProp(idProp, val, optbeh) && retval;
		}
	}
	return retval;
}

BOOL OptTreeNodeList::SetStrProp(UINT idProp, const CString & val, OptBehaviour optbeh)
{
	BOOL retval = TRUE;
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		OptTreeNode * pNode = GetNext(pos);
		if (pNode->IsValid())
		{
			retval = pNode->SetStrProp(idProp, val, optbeh) && retval;
		}
	}
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
OptTreeNode::OptTreeNode()
{
	// we don't have a container yet
	pContainerNode = (OptTreeNode *)NULL;
	m_fValidContent = FALSE;
	pcr = NULL;
	pItem = NULL;
}

OptTreeNode::~OptTreeNode()
{
	// disconnect from our children (if we have any)
	POSITION pos = m_Content.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pNode = (OptTreeNode *)m_Content.GetNext(pos);
		pNode->pContainerNode = (OptTreeNode *)NULL;
		delete pNode;	// delete the child
	}
		
	// inform dependants of this node deletion
	pItem->InformDependants(SN_DESTROY_OPTNODE, (DWORD)this);
}

OptTreeNode::OptTreeNode(OptTreeNode * pParentNode, CProjItem * pNodeItem)
{
	// we don't have a container yet
	pContainerNode = (OptTreeNode *)NULL;
	m_fValidContent = FALSE;

	// our container item
	pItem = pNodeItem;

	// our container of this Node
	pContainerNode = pParentNode;

	// chain our config. by matching with parent
	pcr = pItem->ConfigRecordFromBaseConfig((ConfigurationRecord *)pParentNode->pcr->m_pBaseRecord,TRUE);
	ASSERT(pcr != NULL);

	// make sure the container has a reference to this node
	pParentNode->m_Content.AddTail((void *)this);
}

CProjType * OptTreeNode::GetProjType()
{
	CProjType * pProjType;
	pItem->SetManualBagSearchConfig(pcr);
	if (!g_prjcompmgr.GetProjTypefromProjItem(pItem, pProjType))
		pProjType = (CProjType *)NULL;	// may be external project
	pItem->ResetManualBagSearchConfig();
	return pProjType;
}

// get the tools applicable to this node
void OptTreeNode::GetViableBuildTools(CPtrList & listTools)
{
	// empty our list
	listTools.RemoveAll();

	//
	// is this a CProject?
	if (pItem->IsKindOf(RUNTIME_CLASS(CProject)))
	{
		// get our list of tools from the project type
		CProjType * pProjType = GetProjType();

		// append this project's list of tools
		listTools.AddTail((CPtrList *)pProjType->GetToolsList());
	}
	//
	// is this a CProjGroup?
	else if (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
	{
		// no pages what so ever!
	}
	//
	// is this a CFileItem?
	else if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
		// append our source tool for this item in this config.
		// (don't refresh the cache when getting the source tool)
		CBuildTool * pBuildTool = pcr->GetBuildTool();
		if (pBuildTool != (CBuildTool *)NULL)
			listTools.AddTail(pBuildTool);
	}
}

BOOL OptTreeNode::SetIntProp(UINT idProp, int val, OptBehaviour optbeh)
{
	pItem->SetManualBagSearchConfig(pcr);
	OptBehaviour optbehOld = pItem->SetOptBehaviour(optbeh);

	BOOL fRet = pItem->SetIntProp(idProp, val);

	(void )pItem->SetOptBehaviour(optbehOld);
	pItem->ResetManualBagSearchConfig();
	return fRet;
}

BOOL OptTreeNode::SetStrProp(UINT idProp, const CString& val, OptBehaviour optbeh)
{
	pItem->SetManualBagSearchConfig(pcr);

	//
	// Fix for SPEED:52.
	// GetProject()->ActiveConfig must match pcr->m_pBaseRecord so that functions called
	// as a result of pItem->SetStrProp (.e.g. CActionSlob::AssignActions ) will use the
	// correct project configuration.
	//
	pItem->GetProject()->SetManualBagSearchConfig((ConfigurationRecord*)pcr->m_pBaseRecord);

	OptBehaviour optbehOld = pItem->SetOptBehaviour(optbeh);

	BOOL fRet = pItem->SetStrProp(idProp, val);

	(void )pItem->SetOptBehaviour(optbehOld);

	pItem->GetProject()->ResetManualBagSearchConfig();

	pItem->ResetManualBagSearchConfig();
	return fRet;
}

GPT OptTreeNode::GetIntProp(UINT idProp, int &val, OptBehaviour optbeh)
{
	pItem->SetManualBagSearchConfig(pcr);
	OptBehaviour optbehOld = pItem->SetOptBehaviour(optbeh);

	GPT gpt = pItem->GetIntProp(idProp, val);

	(void )pItem->SetOptBehaviour(optbehOld);
	pItem->ResetManualBagSearchConfig();
	return gpt;
}

GPT OptTreeNode::GetStrProp(UINT idProp, CString &val, OptBehaviour optbeh)
{
	pItem->SetManualBagSearchConfig(pcr);
	OptBehaviour optbehOld = pItem->SetOptBehaviour(optbeh);

	GPT gpt = pItem->GetStrProp(idProp, val);

	(void )pItem->SetOptBehaviour(optbehOld);
	pItem->ResetManualBagSearchConfig();
	return gpt;
}

CPropBag * OptTreeNode::GetPropBag(int nBag)
{
	pItem->SetManualBagSearchConfig(pcr);
	CPropBag * pBag = pItem->GetPropBag(nBag);
	pItem->ResetManualBagSearchConfig();
	return pBag;
}

CProxySlob::CProxySlob()
{
 	m_pParentProxySlob = m_pChildProxySlob = (CProxySlob *)NULL;
	m_popthdlr = (COptionHandler *)NULL;

	// our owner project?
	// FUTURE: figure this using selection given multiple projects
	m_pOwnerProject = g_pActiveProject;
}

CProxySlob::~CProxySlob()
{
	if (m_pParentProxySlob)
		delete m_pParentProxySlob;
}

BOOL CProxySlob::IsSortOf(const CRuntimeClass* pClass)
{
	POSITION pos = m_ptrList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(pos);
		if (!pNode->pItem->IsKindOf(pClass))
			return FALSE;
	}

	return TRUE;
}

CProjType * CProxySlob::GetProjType()
{
	CProjType * pProjType = (CProjType *)NULL;
	POSITION pos = m_ptrList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CProjType * pProjTypeT = ((OptTreeNode *)m_ptrList.GetNext(pos))->GetProjType();
		if (pProjType != (CProjType *)NULL && pProjType != pProjTypeT)
			return (CProjType *)NULL;
		else
			pProjType = pProjTypeT;
	}
 	return pProjType;
}

const CPlatform * CProxySlob::NextPlatform()
{
	const CPlatform * pPlatform = (const CPlatform *)NULL;

	// have we anymore?
	if (m_posList != (POSITION)NULL)
	{
		// get a node out of the selection
		OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(m_posList);

		// Otherwise if the project is an external makefile then
		// we have to rely on the P_ExtOpts_Platform property
		// to obtain the appropriate platform.
		CProject * pProject = pNode->pItem->GetProject();

		if (pProject->m_bProjIsExe)
		{
			CString strUIDescription;

			// make sure the project is in the same config. as the selected node	
			// n.b. item's base config. record *is* project's config. record.
			pProject->SetManualBagSearchConfig((ConfigurationRecord *)pNode->pcr->m_pBaseRecord);
			pProject->GetStrProp(P_ExtOpts_Platform, strUIDescription);
			pProject->ResetManualBagSearchConfig();

			g_prjcompmgr.LookupPlatformByUIDescription(strUIDescription, (CPlatform *&)pPlatform);
		}
		// If the project is an internal makefile then we can get the
		// platform by going through the project directly.
		else
		{
			// return the platform for this node's project type
			pPlatform = pNode->GetProjType()->GetPlatform();
		}
	}

	return pPlatform;
}


const ConfigurationRecord * CProxySlob::GetBaseConfig()
{	
	ConfigurationRecord * pcr = (ConfigurationRecord *)NULL;
	POSITION pos = m_ptrList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(pos);
		if (pcr != (ConfigurationRecord *)NULL)
		{
			if (pNode->pcr->m_pBaseRecord != pcr)
				return (const ConfigurationRecord *)NULL;
		}
		else
			pcr = (ConfigurationRecord *)pNode->pcr->m_pBaseRecord;
	}

	return (const ConfigurationRecord *)pcr;
}

//////////////////////////////////////////////////////////////////////////////
void CProxySlob::Clear(BOOL fInform)
{
	POSITION pos = m_ptrList.GetHeadPosition();
	while (pos != (POSITION)NULL)
		Remove((OptTreeNode *)m_ptrList.GetNext(pos), fInform);
}

void CProxySlob::Add(OptTreeNode * pNode, BOOL fInform)
{
	// don't add of already exists
	if (m_ptrList.Find((void *)pNode) != (POSITION)NULL)
		return;
		
	m_ptrList.AddTail((void *)pNode);
	if (m_pChildProxySlob)	m_pChildProxySlob->FixContainment();	

	// add this node's container to our container CProxySlob
	if (pNode->pContainerNode)
	{
		if (!m_pParentProxySlob)
		{
			m_pParentProxySlob = new CProxySlob;
			m_pParentProxySlob->m_pChildProxySlob = this;
		}

		m_pParentProxySlob->Add(pNode->pContainerNode, FALSE);
		FixContainment();
	}

	// our active configuration
	m_pActiveConfig = IsSingle() ? GetSingle()->pcr : (ConfigurationRecord *)NULL;

	if (fInform)	InformDependants(SN_ALL);
}

void CProxySlob::Remove(OptTreeNode * pNode, BOOL fInform)
{
	// remove this node from our list
	POSITION pos = m_ptrList.Find((void *)pNode);
	if (pos == NULL)	return; // not in our selection, do nothing!
	
	m_ptrList.RemoveAt(pos);
	if (m_pChildProxySlob)	m_pChildProxySlob->FixContainment();	

	// remove this node's container from our container CProxySlob
	if (pNode->pContainerNode)
	{
		m_pParentProxySlob->Remove(pNode->pContainerNode, FALSE);
		FixContainment();	
	}

	// set our active configuration
	m_pActiveConfig = IsSingle() ? GetSingle()->pcr : (ConfigurationRecord *)NULL;

	if (fInform)	InformDependants(SN_ALL);
}

// property management across multiple OptTreeNodes
BOOL CProxySlob::SetIntProp(UINT idProp, int val)
{
	if (m_ptrList.GetCount() == 0)
		return TRUE;
	
	BOOL fOk = TRUE;
	POSITION pos = m_ptrList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(pos);
		if (!pNode->SetIntProp(idProp, val, m_optbeh))
		{
			fOk = FALSE;
			break;	// we'll fail to set the others as well, don't bother
		}
	}

	if ( fOk )
		InformDependants( idProp );

	return fOk;
}

BOOL CProxySlob::SetStrProp(UINT idProp, const CString& val)
{
	if (m_ptrList.GetCount() == 0)
		return TRUE;

	BOOL fOk = TRUE;
	POSITION pos = m_ptrList.GetHeadPosition();

	// is this an option list?
	BOOL fDoMultiOptList = FALSE;
	TCHAR chJoin;		// join char
	BOOL fUseDefJoins;	// use default join string?

	// only need this for multiple-select option lists
	if (!IsSingle())
	{
		UINT idUnknownOption, idUnknownString;
		COptionHandler * popthdlr = m_popthdlr;
		while (popthdlr != (COptionHandler *)NULL)
		{
			popthdlr->GetSpecialOptProps(idUnknownOption, idUnknownString, m_optbeh);
			if (idProp == idUnknownString || idProp == idUnknownOption)
			{
				// final option list will *always* be space-sep,
				// we will *not* use the default seps to distinguish indi. elements
				chJoin = _T(' ');
				fDoMultiOptList = TRUE;
				fUseDefJoins = FALSE;
				break;
			}
			else if (popthdlr->IsListStrProp(idProp))
			{
				// final option list will *always* be comma-sep,
				// we will use the default seps to distinguish indi. elements
				chJoin = _T(',');
				fDoMultiOptList = fUseDefJoins =TRUE;
				break;
			}
			popthdlr = popthdlr->GetBaseOptionHandler();
		}
	}

	if (fDoMultiOptList)
	{
		// yes, append this elements to what we already have

		// create an array of option lists to hold our 'rememebered' node values
		COptionList * rgoptlstNode = new COptionList[m_ptrList.GetCount()];
		if (rgoptlstNode == (COptionList *)NULL)
			return FALSE;

		COptionList optlstCommon(chJoin, fUseDefJoins);
		CString valNode, valResult;
		BOOL fFirst = TRUE;
		int iNode;

		// get the common component, and the 'old' option list values

		iNode = 0;
		while (pos != (POSITION)NULL)
		{
			OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(pos);

			GPT gpt;
			if ((gpt = pNode->GetStrProp(idProp, valNode, m_optbeh)) == valid)
			{
				rgoptlstNode[iNode].SetJoinUsage(chJoin, fUseDefJoins);
				rgoptlstNode[iNode].SetString(valNode);
				if (fFirst)
				{
					optlstCommon = rgoptlstNode[iNode];
					fFirst = FALSE;
				}
				else
					optlstCommon.Common(rgoptlstNode[iNode]);
			}

			iNode++;
		}

		// set the 'new' option list values

		COptionList optlstVal(val, chJoin, fUseDefJoins);
		iNode = 0;
		pos = m_ptrList.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(pos);

			rgoptlstNode[iNode].Subtract(optlstCommon);	// subtract our 'old' common
			rgoptlstNode[iNode].Append(optlstVal);		// append our 'new' common
			rgoptlstNode[iNode].GetString(valResult);	// get the resultant string

			if (!pNode->SetStrProp(idProp, valResult, m_optbeh))
			{
				fOk = FALSE;
				break;	// we'll fail to set the others as well, don't bother
			} 

			iNode++;
		}

		// destroy our 'remembered' node values
		delete [] rgoptlstNode;
	}
	else
	{
		// no, set this value in all

		while (pos != (POSITION)NULL)
		{
			OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(pos);
			if (!pNode->SetStrProp(idProp, val, m_optbeh))
			{
				fOk = FALSE;
				break;	// we'll fail to set the others as well, don't bother
			}
		}
	}

	if ( fOk )
		InformDependants( idProp );

	return fOk;
} 

GPT CProxySlob::GetStrProp(UINT idProp, CString& val)
{
	if (m_ptrList.GetCount() == 0)
		return invalid;

	CString valNode;
	BOOL fFirst = TRUE;
	POSITION pos = m_ptrList.GetHeadPosition();

	// is this an option list?
	BOOL fDoMultiOptList = FALSE;
	TCHAR chJoin;		// join char
	BOOL fUseDefJoins;	// use default join string?

	// only need this for multiple-select option lists
	if (!IsSingle())
	{
		UINT idUnknownOption, idUnknownString;
		COptionHandler * popthdlr = m_popthdlr;
		while (popthdlr != (COptionHandler *)NULL)
		{
			popthdlr->GetSpecialOptProps(idUnknownOption, idUnknownString, m_optbeh);
			if (idProp == idUnknownString || idProp == idUnknownOption)
			{
				// final option list will *always* be space-sep,
				// we will *not* use the default seps to distinguish indi. elements
				chJoin = _T(' ');
				fDoMultiOptList = TRUE;
				fUseDefJoins = FALSE;
				break;
			}
			else if (popthdlr->IsListStrProp(idProp))
			{
				// final option list will *always* be comma-sep,
				// we will use the default seps to distinguish indi. elements
				chJoin = _T(',');
				fDoMultiOptList = fUseDefJoins =TRUE;
				break;
			}
			popthdlr = popthdlr->GetBaseOptionHandler();
		}
	}

	if (fDoMultiOptList)
	{
		// yes, find the common option elements in the lists

		COptionList optlstCommon(chJoin, fUseDefJoins);
		while (pos != (POSITION)NULL)
		{
			OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(pos);

			GPT gpt;
			if ((gpt = pNode->GetStrProp(idProp, valNode, m_optbeh)) != valid)
				return gpt;

			if (fFirst)
			{
				optlstCommon.SetString(valNode);
				fFirst = FALSE;
			}
			else
			{							
				COptionList optlstNode(valNode, chJoin, fUseDefJoins);
				optlstCommon.Common(optlstNode);
			}
		}
		optlstCommon.GetString(val);
	}
	else
	{
		// no, find the common value

		while (pos != (POSITION)NULL)
		{
			OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(pos);

			GPT gpt;
			if ((gpt = pNode->GetStrProp(idProp, valNode, m_optbeh)) != valid)
				return gpt;

			if (fFirst)
			{
				val = valNode;
				fFirst = FALSE;
			}
			else if (val != valNode)
				return ambiguous;
		}
	}

	return valid;
}

GPT CProxySlob::GetIntProp(UINT idProp, int& val)
{
	if (m_ptrList.GetCount() == 0)
		return invalid;

	BOOL fFirst = TRUE;
	int valNode;
	POSITION pos = m_ptrList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(pos);

		GPT gpt;
		if ((gpt = pNode->GetIntProp(idProp, valNode, m_optbeh)) != valid)
			return gpt;

		if (fFirst)
		{
			val = valNode;
			fFirst = FALSE;
		}
		else if (val != valNode)
			return ambiguous;
	}

	return valid;
}

void CProxySlob::OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint)
{
	if (idChange == SN_DESTROY_OPTNODE)
	{
		// remove from our selection
		Remove((OptTreeNode *)dwHint, TRUE);
		return;	// don't pass this on
	}
	
	InformDependants(idChange, dwHint);
}

CPropBag * CProxySlob::GetPropBag(int nBag)
{
	if (m_ptrList.GetCount() == 0)
		return (CPropBag *)NULL;

	OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetHead();
	return pNode->GetPropBag(nBag);
}

BOOL CProxySlob::Selected(OptTreeNode * pNode)
{
	return m_ptrList.Find(pNode) != (POSITION)NULL;
}

BOOL CProxySlob::Selected(CSlob * pSlob, ConfigurationRecord * pcr)
{
	POSITION pos = m_ptrList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pNode = (OptTreeNode *)m_ptrList.GetNext(pos);
		if (pNode->pItem == pSlob && pNode->pcr == pcr)
			return TRUE;
	}

	return FALSE;	// no
}


//////////////////////////////////////////////////////////////////////////////
// implementation of the COptionList class
#pragma intrinsic(memcpy)

void COptionList::SetString(const TCHAR * pch)
{
	ASSERT(pch != (const TCHAR *)NULL);

	// empty the old one
	Empty();

	const TCHAR * rgchJoin = (const TCHAR *)m_strJoin;

	//
	// Use rgchJoinAndSpace to eliminate spaces surrounding separators.  This is a quick
	// fix to prevent the usability problem of leading and trailing spaces in tokens. (Speed:49).
	//
	// NOTE: This fix will prevent the obscure but legal use of path names with leading spaces.
	//

	// allocate enough to copy grchJoin and append space.
	TCHAR * rgchJoinAndSpace = new TCHAR[_tcslen(rgchJoin)+2];
	_tcscpy(rgchJoinAndSpace,rgchJoin);
	_tcscat(rgchJoinAndSpace,_T(" "));

	// skip leading join chars and spaces.
	while (*pch != _T('\0') && _tcschr(rgchJoinAndSpace, (unsigned int)(int)*pch) != (TCHAR *)NULL)	
		pch++;	// join chars assumed to be SBC

	// anything to do?
	if (*pch == _T('\0'))
	{
		delete [] rgchJoinAndSpace;
		return; // do nothing
	}

	TCHAR chVal, chJoin = m_strJoin[0];
	ASSERT(chJoin != _T('\0'));

	BOOL fQuoted = FALSE;
	const TCHAR * pchWord = pch;
	UINT cch, cchWord = 0;
	do
	{
		chVal = *pch;

		// are we quoted?
		if (chVal == _T('"'))
			fQuoted = !fQuoted;

		// look for a non-quoted join char, or terminator
		if (chVal == _T('\0') || (_tcschr(rgchJoin, chVal) != NULL && (chVal == _T('\t') || !fQuoted)))
		{
			// skip any extra join chars (and spaces) with this one, ie. ' , '
			if (chVal != _T('\0'))
			{
				chVal = *(++pch);	// join chars assumed to be SBC
				while (chVal != _T('\0') && _tcschr(rgchJoinAndSpace, chVal) != NULL)
				{												
					pch++; chVal = *pch;	// join chars assumed to be SBC
				}
			}

			// Remove trailing blanks from pchWord.
			while (0 != cchWord && 0 == _tccmp( _tcsdec(pchWord,pchWord+cchWord),_T(" ")) )
				--cchWord;

			// do we have any word characters?
			if (cchWord != 0)	Append(pchWord, cchWord);

			cchWord = 0;	// ready for start of next word
			pchWord = pch;
		}
		else
		{
			// skip non-join char
			cch = _tclen(pch);
			cchWord += cch; pch += cch;
		}
	}
	while (chVal != _T('\0'));

	delete [] rgchJoinAndSpace;
	return;
}

void COptionList::GetString(CString & str)
{
	TCHAR chJoin = m_strJoin[0];
	ASSERT(chJoin != _T('\0'));

	// allocate our buffer
	TCHAR * pch;
	if (!m_cchStr ||	// empty string?
		(pch = new TCHAR[m_cchStr]) == (TCHAR *)NULL
	   )
	{
		str = _TEXT(""); // in case of error
		return;
	}

	TCHAR * pchWord = pch;
	POSITION pos = m_lstStr.GetHeadPosition();
	ASSERT(pos != (POSITION)NULL);
	for (;;)	// we'll break (more efficient!)
	{
		OptEl * poptel = (OptEl *)m_lstStr.GetNext(pos);

		register int cch = poptel->cch;
		memcpy(pchWord, poptel->pch, cch);

		if (pos != (POSITION)NULL)
			pchWord[cch-1] = chJoin;	// join *not* terminate
		else
			break;	// terminate!

		pchWord += cch;
	}

	// return string and then delete our local buffer
	str = pch;
	delete [] pch;
}

void COptionList::Empty()
{
	// free-up our string memory
	POSITION pos = m_lstStr.GetHeadPosition(), posCurrent;
	while (pos != (POSITION)NULL)
	{
	 	posCurrent = pos;
		OptEl * poptel = (OptEl *)m_lstStr.GetNext(pos);
		VERIFY(Delete(poptel, posCurrent));
	}
}																			 

void COptionList::Add(OptEl * poptel)
{
	// put this into our list
	poptel->cref++;	// bump. ref. count.
	POSITION posOurWord = m_lstStr.AddTail((void *)poptel);
	m_cchStr += poptel->cch;

	// remember this for quick 'does string exist in list?' query
	// (in the case of dupes, we might nuke the old duplicate but that
	//  doesn't matter, it'll still solve its purpose)
	m_mapStrToPtr.SetAt(poptel->pch, (void *)posOurWord);
}

void COptionList::Append(const TCHAR * pch, int cch)
{
	OptEl * poptel = (OptEl *)new BYTE[sizeof(OptEl) + cch];
	if (poptel == (OptEl *)NULL)	return;

	// initialise with a ref. count of 1
	memcpy(poptel->pch, pch, cch);
	poptel->pch[cch] = _T('\0');
	poptel->cch = cch + 1;
	poptel->cref = 0;

	// can we add, ie. 
	// do we want to check for duplicates?
	if (!Exists(poptel->pch))
		Add(poptel);	// put this into our list
	else
		delete [] (BYTE *)poptel;	// duplicate
}

void COptionList::Append(COptionList & optlst)
{
	POSITION pos = optlst.m_lstStr.GetHeadPosition();
	while (pos != NULL)
	{
		OptEl * poptel = (OptEl *)optlst.m_lstStr.GetNext(pos);
		if (!Exists(poptel->pch))
			Add(poptel);
	}
}

void COptionList::Common(COptionList & optlst)
{
	POSITION pos = m_lstStr.GetHeadPosition(), posCurrent;
	while (pos != (POSITION)NULL)
	{
	 	posCurrent = pos;
		OptEl * poptel = (OptEl *)m_lstStr.GetNext(pos);
		if (!optlst.Exists(poptel->pch))
			VERIFY(Delete(poptel, posCurrent));
	}
}

BOOL COptionList::Delete(OptEl * poptel, POSITION posOurWord)
{
	// pre-loaded position to delete, or found this OptEl?
	if (!posOurWord && !m_mapStrToPtr.Lookup(poptel->pch, (void *&)posOurWord))
		return FALSE;	// no

	// retrieve our optel for this (might be the same)
	poptel = (OptEl *)m_lstStr.GetAt(posOurWord);

	m_lstStr.RemoveAt(posOurWord);
	(void) m_mapStrToPtr.RemoveKey(poptel->pch);

	m_cchStr -= poptel->cch;
	poptel->cref--;	// decr. the ref. count
	if (poptel->cref == 0)	// if zero, then free-up the element
		delete [] (BYTE *)poptel;

	return TRUE;
}

BOOL COptionList::Subtract(const TCHAR * pch)
{
	// does this exist?
	POSITION posOurWord;
	if (!m_mapStrToPtr.Lookup(pch, (void *&)posOurWord))	return FALSE;	// no

	// remove this from our list
	VERIFY(Delete((OptEl *)m_lstStr.GetAt(posOurWord), posOurWord));

	return TRUE; // ok
}

BOOL COptionList::Subtract(COptionList & optlst)
{
	BOOL fFound = FALSE;

	POSITION pos = optlst.m_lstStr.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptEl * poptel = (OptEl *)optlst.m_lstStr.GetNext(pos);
		// removed this from our list?
		if (Delete(poptel))
			fFound = TRUE;
	}

	return fFound;
}

void COptionList::Components(const TCHAR * pch, COptionList & optlstAdd, COptionList & optlstSub)
{
	COptionList optlst(_T(' '), FALSE);

	// set the option string to find components for
	optlst.m_strJoin = m_strJoin;
	optlst.SetString(pch);

	POSITION pos = m_lstStr.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptEl * poptel = (OptEl *)m_lstStr.GetNext(pos);
		// exist in us?
		if (!optlst.Delete(poptel))		// can do an addition comp.?
			optlstSub.Add(poptel);		// must be a subtract comp.
	}

	// return
	optlstAdd = optlst;
}

#pragma function(memcpy)

//////////////////////////////////////////////////////////////////////////////
// implementation of the COptionHandler class
COptionHandler::COptionHandler(CBuildTool * pBuildTool, COptionHandler * popthdlrBase)
{
	m_pAssociatedBuildTool = pBuildTool;
	m_popthdlrBase = (COptionHandler *)popthdlrBase;	// no base option handler

	// make sure we got a valid build tool
	ASSERT_VALID(m_pAssociatedBuildTool);

	// Initialize our hierachy depth
	m_cDepth = 0;

	// Init. our slob stack
	m_sStk = 0;

	// assoc. ourselves with the build tool
	m_pAssociatedBuildTool->SetOptionHandler(this);

	// our associated CSlobs
	m_pSlob = (CSlob *)NULL;
}

COptionHandler::COptionHandler
(
	const TCHAR * szPkg, WORD id,
	const TCHAR * szPkgTool, WORD idTool,
	const TCHAR * szPkgBase, WORD idBase
)
{
	// set our id.
	SetId(GenerateComponentId(g_prjcompmgr.GenerateBldSysCompPackageId(szPkg), id));

	// Initialize our hierachy depth
	m_cDepth = 0;

	// Init. our slob stack
	m_sStk = 0;

	// our associated build tool (if we have one)
	m_pAssociatedBuildTool = (CBuildTool *)NULL;
	if (idTool != 0)
	{
		if (!g_prjcompmgr.LookupBldSysComp(GenerateComponentId(g_prjcompmgr.GenerateBldSysCompPackageId(szPkgTool), idTool),
										   (CBldSysCmp *&)m_pAssociatedBuildTool))
			ASSERT(FALSE);	// failed

		// make sure we got a valid build tool
		ASSERT_VALID(m_pAssociatedBuildTool);
		ASSERT(m_pAssociatedBuildTool->IsKindOf(RUNTIME_CLASS(CBuildTool)));

		// assoc. ourselves with the build tool
		m_pAssociatedBuildTool->SetOptionHandler(this);
	}

	// our base option handler (if we have one)
	m_popthdlrBase = (COptionHandler *)NULL;
	if (idBase != 0)
	{
		if (!g_prjcompmgr.LookupBldSysComp(GenerateComponentId(g_prjcompmgr.GenerateBldSysCompPackageId(szPkgBase), idBase),
										   (CBldSysCmp *&)m_popthdlrBase))
			ASSERT(FALSE);	// failed

		// make sure we got a valid base option handler
		ASSERT_VALID(m_popthdlrBase);
		ASSERT(m_popthdlrBase->IsKindOf(RUNTIME_CLASS(COptionHandler)));

		// calc. our depth
		COptionHandler * popthdlr = m_popthdlrBase;
		do
		{
			m_cDepth++;	// inc. depth (we have at least 1)
		}
		while (popthdlr = popthdlr->GetBaseOptionHandler());

		// If we have a tool then reset the option handler
		// as our depth has now changed. The depth must be
		// in ssync with the option handler in the tool.
		if (idTool != 0)
			m_pAssociatedBuildTool->SetOptionHandler(this);
	}


	// our associated CSlobs
	m_pSlob = (CSlob *)NULL;
}

COptionHandler::~COptionHandler()
{
	WORD key;
	CDefOptionProp * pdefopt;

	// destroy all of our default props for the map
	for (POSITION pos = m_mapDefOptions.GetStartPosition() ; pos != NULL;)
	{
		m_mapDefOptions.GetNextAssoc(pos, key, (void *&)pdefopt);
		delete pdefopt;
	}

	m_mapDefOptions.RemoveAll();

	ASSERT(m_pdirtyProps);
	delete m_pdirtyProps;
	delete [] m_pStrListPropIds;
}

BOOL COptionHandler::Initialise()
{
	// convert all table logical props to actual props
	COptStr	* poptstr = GetOptionStringTable();
	while (poptstr->idOption != (UINT)-1)
	{
		for (WORD cArgs = 0; cArgs < MAX_OPT_ARGS; cArgs++)
		{
			UINT * pidArg = poptstr->rgidArg + cArgs;
			if (*pidArg != (UINT)-1)
				*pidArg = MapLogical(*pidArg);
		}

		poptstr++;	// next
	}

	// create our default option map
	UINT nRange = GetOptPropRange();

	COptionLookup * poptlookup = GetOptionLookup();
	if ((poptlookup == (COptionLookup *)NULL) || GetOptionLookup()->FInit(GetOptionStringTable()))
	{
		// make the map hash size close to a prime no. matching the # of props.
		UINT nHash = (UINT)-1;
		if (nHash > 90)
			nHash = 89;
		else if (nHash > 60)
			nHash = 59;
		else if (nHash > 30)
			nHash = 29;
		if (nHash != (UINT)-1)
			m_mapDefOptions.InitHashTable(nHash);

		// fill the default option map
		CreateOptDefMap();
	}
	
	// create our dirty prop array
	m_pdirtyProps = new CByteArray; m_pdirtyProps->SetSize(nRange);
	m_pStrListPropIds = new UINT[nRange];	// max. required

	// create our cache of string list props
	m_cStrListPropIds = 0;
	UINT idPropMin, idPropMax; GetMinMaxOptProps(idPropMin, idPropMax);
	for (UINT idProp = idPropMin; idProp <= idPropMax; idProp++)
		if (IsListStrProp(idProp))
			m_pStrListPropIds[m_cStrListPropIds++] = idProp;

	return TRUE;	// ok
}

void COptionHandler::GetSpecialOptProps(UINT & nIDUnkOpt, UINT & nIDUnkStr, OptBehaviour optbeh)
{
	UINT nIDUnkOptB, nIDUnkStrB;
	GetSpecialLogicalOptProps(nIDUnkOpt, nIDUnkStr, optbeh);

	COptionHandler * popthdlr = GetBaseOptionHandler();

	if (nIDUnkOpt != (UINT)-1)
	{
		if (nIDUnkOpt == (UINT)NULL && popthdlr != (COptionHandler *)NULL)
			popthdlr->GetSpecialOptProps(nIDUnkOpt, nIDUnkStrB);
		else
			nIDUnkOpt = MapLogical(nIDUnkOpt);
	}
		
	if (nIDUnkStr != (UINT)-1)
	{
		if (nIDUnkStr == (UINT)NULL && popthdlr != (COptionHandler *)NULL)
			popthdlr->GetSpecialOptProps(nIDUnkOptB, nIDUnkStr);
		else
			nIDUnkStr = MapLogical(nIDUnkStr);
	}
}

int COptionHandler::GetTabDlgOrder()
{
	COptionHandler * popthdlr = GetBaseOptionHandler();
	if (popthdlr != (COptionHandler *)NULL)
		return popthdlr->GetTabDlgOrder();

	return 1000;	// default is at end of tabs
}

UINT COptionHandler::GetNameID()
{
	COptionHandler * popthdlr = GetBaseOptionHandler();
	if (popthdlr != (COptionHandler *)NULL)
		return popthdlr->GetNameID();

	return IDS_GENERAL;	// 'General' default
}

void COptionHandler::ResetPropsForConfig(ConfigurationRecord * pcr)
{
	// reset our base-handler props first
	COptionHandler * popthdlr = GetBaseOptionHandler();
	if (popthdlr != (COptionHandler *)NULL)
		popthdlr->ResetPropsForConfig(pcr);

	// reset our own props.
	UINT nPropMin, nPropMax;
	GetMinMaxOptProps(nPropMin, nPropMax);
	pcr->BagCopy(BaseBag, CloneBag, nPropMin, nPropMax, TRUE);
}

BOOL COptionHandler::CanResetPropsForConfig(ConfigurationRecord * pcr)
{
	// check our base-handler props first
	COptionHandler * popthdlr = GetBaseOptionHandler();
	if (popthdlr != (COptionHandler *)NULL)
		if (popthdlr->CanResetPropsForConfig(pcr))
			return TRUE;	// no need to check other handlers

	UINT nPropMin, nPropMax;
	GetMinMaxOptProps(nPropMin, nPropMax);
	return !pcr->BagSame(BaseBag, CloneBag, nPropMin, nPropMax, TRUE);
}
 
GPT COptionHandler::GetDefIntProp(UINT idProp, int & val)
{
	CDefOptionProp * pdefopt;
	if (!m_mapDefOptions.Lookup(idProp, (void *&)pdefopt)) return invalid;

	if (pdefopt->m_pprop->m_nType == string)
	{
		if (!ConvertFromStr(idProp, ((CStringProp *)pdefopt->m_pprop)->m_strVal, val))
			return invalid;
	}
	else
	{
		ASSERT(pdefopt->m_pprop->m_nType == booln || pdefopt->m_pprop->m_nType == integer);
		val = (pdefopt->m_pprop->m_nType == booln) ?
			   ((CBoolProp *)pdefopt->m_pprop)->m_bVal : ((CIntProp *)pdefopt->m_pprop)->m_nVal;
	}

	return valid;
}

GPT COptionHandler::GetDefStrProp(UINT idProp, CString & val)
{
	CDefOptionProp * pdefopt;
	if (!m_mapDefOptions.Lookup(idProp, (void *&)pdefopt)) return invalid;

	if (pdefopt->m_pprop->m_nType == integer)
	{
		if (!ConvertToStr(idProp, ((CIntProp *)pdefopt->m_pprop)->m_nVal, val))
			return invalid;
	}
	else
	{
		ASSERT(pdefopt->m_pprop->m_nType == string);
		val = ((CStringProp *)pdefopt->m_pprop)->m_strVal;
	}

	return valid;
}

BOOL COptionHandler::IsDefaultIntProp(UINT idProp, int & nVal)
{
	int nOurVal;
	return (GetDefIntProp(idProp, nOurVal) == valid) &&
		   (nOurVal == nVal);
}

BOOL COptionHandler::IsDefaultStringProp(UINT idProp, CString & strVal)
{
	CString strOurVal;
	if (GetDefStrProp(idProp, strOurVal) != valid)
		return FALSE;

   TCHAR * pch1 = (TCHAR *)(const TCHAR *)strOurVal;
   TCHAR * pch2 = (TCHAR *)(const TCHAR *)strVal;
   BOOL fEscaped = FALSE;
   TCHAR ch1, ch2;
   while ((ch1 = *pch1) != _T('\0') && (ch2 = *pch2) != _T('\0'))
   {
		// case insensitive compare (most likely for tool options)
   		if (_totupper(ch1) != _totupper(ch2))
   		{
			// treat a '\' and an '/' as equivalent if not escaped '\'
			// this is the case for common option args which are filenames
			if (fEscaped ||
				(ch1 != _T('\\') && ch1 != _T('/')) ||
				(ch2 != _T('\\') && ch2 != _T('/'))
			   )
	   			return FALSE;	// failed
   		}

		int cch = _tclen(pch1);
		pch1 += cch; pch2 += cch;

   		fEscaped = !fEscaped && (ch1 == _T('\\'));
   }

   return *pch1 == _T('\0') && *pch2 == _T('\0');
}

// conversion of int, oct,hex values into a text form
// FUTURE: I'd like to put this elsewhere, but where?
BOOL COptionHandler::ConvertToStr(UINT idProp, int nVal, CString & strVal)
{
	CDefOptionProp * pdefopt;
	if (!m_mapDefOptions.Lookup(idProp, (void *&)pdefopt)) return FALSE;

	SUBTYPE subtype = pdefopt->subtypeprop;
	int nBase;
	if (subtype == hexST)
	{
		nBase = 16; strVal = "0x";
	}
	else if (subtype == octST)
	{
		nBase = 8; strVal = "0";
	}
	else
	{
		ASSERT(subtype == intST);
		nBase = 10; strVal = "";
	}

	char	szVal[34];	// max. of 33 chars

	// we use unsigned longs, no negative ints allowed
	strVal += _ultoa((unsigned long)(unsigned int)nVal, szVal, nBase);

	return TRUE;
}

// what terminators do we have?
// o zero terminator ('\0') for regular number strings
// o '>' terminator for option table number strings
#define IsStrTerminator(ch) (ch == _T('\0') || ch == _T('>'))

BOOL COptionHandler::ConvertFromStr(UINT idProp, const TCHAR * pchVal, int & nVal)
{
	CDefOptionProp * pdefopt;
	if (!m_mapDefOptions.Lookup(idProp, (void *&)pdefopt)) return FALSE;

	SUBTYPE subtype = pdefopt->subtypeprop;
	TCHAR * pch = (TCHAR *)pchVal;

	// strip the prefix
	if (subtype == hexST)
	{
		// we'll allow a leading 'x' (a hack BTW)
		if (*pchVal == _T('x') || *pchVal == _T('X'))
			pch++;
		else if (*pchVal == _T('0') && (*(pchVal+1) == _T('x') || *(pchVal+1) == _T('X')))
			pch += 2;
		else
			subtype = intST;	// no leading hex prefix, we'll assume decimal
	}
	else if (subtype == octST)
	{
		// we'll allow a leading '0' (a hack BTW)
		if (*pchVal == _T('0'))
			pch++;
	}
	else
	{
		ASSERT(subtype == intST);
		if (*pchVal == _T('+'))
			pch++;	// '+' is a single byte DBC
	}

	UINT iOut = 0;

	// init. the value
	nVal = 0;

	if (subtype == hexST)
	{
		for (; !IsStrTerminator(pch[iOut]); iOut++)
		{
			// check for well-formed hex number
			if (iOut > 7 || !isxdigit((unsigned char)pch[iOut]) || nVal & 0xf0000000)
			{
				// not a hex-digit or 
				// too many hex-digits (only 32-bit, ie. 8 digit hex-numbers supported) or
				// next shift would cause overflow!
				InformationBox(IDS_BAD_HEX_NUMBER, pchVal);
				return FALSE;
			}

			nVal <<= 4;
			nVal += (toupper((unsigned char)pch[iOut]) -
					 (isdigit(toupper((unsigned char)pch[iOut])) ? '0' : 'A' - 10));
		}
	}
	else if (subtype == octST)
	{
		for (; !IsStrTerminator(pch[iOut]); iOut++)
		{
			// check for well-formed hex number
			if (iOut > 10 || !isdigit((unsigned char)pch[iOut]) ||
				pch[iOut] == _T('8') || pch[iOut] == _T('9') || nVal & 0xe0000000)
			{
				// we found '8' or '9'
				// too may oct-digits (only 32-bit, ie. 11 digit oct-numbers supported)
				// next shift would cause overflow!
				InformationBox(IDS_BAD_OCT_NUMBER, pchVal);
				return FALSE;
			}

			nVal <<= 3;
			nVal += (toupper((unsigned char)pch[iOut]) - '0');
		}
	}
	else
	{
		ASSERT(subtype == intST);

		for (; !IsStrTerminator(pch[iOut]); iOut++)
		{
			// check for well-formed dec. number
			if (iOut > 9 || !isdigit((unsigned char)pch[iOut]))
			{
				// only want positive numbers
				// too may decimal digits (only 32-bit, ie. 10 digit dec-numbers supported)
				InformationBox(IDS_BAD_DEC_NUMBER, pchVal);
				return FALSE;
			}

  			int nDigit = (toupper((unsigned char)pch[iOut]) - '0');
			
			// possible overflow?
			if ((nVal >= 429496729 && nDigit > 5) || (nVal > 429496730))
			{
				// number too big (only 32-bit, max. is 4,294,967,295)
				InformationBox(IDS_BAD_DEC_NUMBER, pchVal);
				return FALSE;
			}

			nVal *= 10;
			nVal += nDigit;
		}
	}

	return TRUE;
}

void COptionHandler::MungeListStrProp
(
	UINT nIDProp,
	const CString & strVal,
	BOOL fAppend,
	char chJoin
)
{
	// get our comma-sep list
	CString str;
	m_pSlob->GetStrProp(nIDProp, str);
	COptionList optlst(str, chJoin);

	// perform munge
	optlst.Subtract((const TCHAR *)strVal);
	if (fAppend)
		optlst.Append((const TCHAR *)strVal, strVal.GetLength());

	// set our 'munged' list
	optlst.GetString(str);
	m_pSlob->SetStrProp(nIDProp, str);
}

void COptionHandler::SetListStrProp
(
	CSlob * pSlob,
	UINT nIDExtraProp,
	CString & strVal,
	BOOL fInherit,
	BOOL fAnti,
	TCHAR chJoin
)
{
	// do nothing
	if (pSlob == (CSlob *)NULL)	return;

	CPropBag * pBag = pSlob->GetPropBag();

	// it is easy to set an anti prop
	if (fAnti)
	{
		pBag->SetStrProp(pSlob, ListStr_SubPart(nIDExtraProp), strVal);
	}
	else
	{
		// get the currently existing option list for our parent
		CString strParent; strParent = "";
		CSlob * pSlobParent = ((CProjItem *)pSlob)->GetContainerInSameConfig();
		if (pSlobParent != (CSlob *)NULL)
		{
			(void) GetListStrProp(pSlobParent, nIDExtraProp, strParent, fInherit, fAnti, chJoin);
			((CProjItem *)pSlobParent)->ResetContainerConfig();
		}

		// now figure out what we have subtracted or added
		COptionList optlstParent(strParent, chJoin);
		COptionList optlstAdd(chJoin), optlstSub(chJoin);

		// break into the component parts
		optlstParent.Components(strVal, optlstAdd, optlstSub);

		// we now have our add and subtract, convert back into strings
		CString strAddPart, strSubPart;
		optlstAdd.GetString(strAddPart);
		optlstSub.GetString(strSubPart);

		// set the properties
		pBag->SetStrProp(pSlob, ListStr_AddPart(nIDExtraProp), strAddPart);
		pBag->SetStrProp(pSlob, ListStr_SubPart(nIDExtraProp), strSubPart);
	}

	// inform people of a change to pseudo prop. nIDExtraProp
	pSlob->InformDependants(nIDExtraProp);
}

GPT COptionHandler::GetListStrProp
(
	CSlob * pOrigSlob,
	UINT nIDExtraProp,
	CString & strVal,
	BOOL fInherit,
	BOOL fAnti,
	char chJoin
)
{
	// do nothing?
	if (pOrigSlob == (CSlob *)NULL)	return invalid;

	// clear the buffer ready for concatenation
	strVal = "";
	
	// won't allow inheritance plus anti for string lists
	if (fAnti)	fInherit = FALSE;

	CSlob * pSlob = pOrigSlob;
	CObList obList;
	if (fInherit)
	{
		// we are a CProjItem and we are inheriting!
		// get the prop for each of our ancestor's in elder first order
		while (pSlob != (CSlob *)NULL)
		{
			obList.AddHead(pSlob);
			pSlob = ((CProjItem *)pSlob)->GetContainerInSameConfig();	// get our container
		}
	}
	else
	{
		// we can do a no-inheritance thing!
		obList.AddHead(pSlob);
	}

	// if we are a CProjItem and we are inheriting then do our own thing
	// ie. go through each CSlob in the container hierarchy and append each part to 
	// our options

	COptionList optlstVal(chJoin);
	POSITION pos = obList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		pSlob = (CSlob *)obList.GetNext(pos);

		// CProjGroups don't support tool options!
		if (!pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			CPropBag * pBag = pSlob->GetPropBag();
			CStringProp * pStrProp;

			// disable inheriting so we can do it!
			BOOL fPropInheritOld = ((CProjItem *)pSlob)->EnablePropInherit(FALSE);

			// using string lists
			COptionList optlstSub(chJoin);
			if ((pStrProp = (CStringProp *)pBag->FindProp(ListStr_SubPart(nIDExtraProp))) != (CStringProp *)NULL)
				optlstSub.SetString(pStrProp->m_strVal);

			// if *not* anti,
			// - append our 'add part'
			// - subtract our 'subtract part'
			// else
			// - add our 'subtract part'
			if (fAnti) /* anti-option */
			{
				CString strParent;
				CSlob * pSlobParent = ((CProjItem *)pSlob)->GetContainerInSameConfig();
				if (pSlobParent != (CSlob *)NULL)
				{
					(void) GetListStrProp(pSlobParent, nIDExtraProp, strParent, TRUE, FALSE);
					((CProjItem *)pSlobParent)->ResetContainerConfig();
				}

				// remove from the subtract option anything that
				// doesn't exist in our parent's list
				COptionList optlstParent(strParent, chJoin);
				optlstSub.Common(optlstParent);
				optlstVal.Append(optlstSub);
			}
			else /* not anti-option */
			{
				COptionList optlstAdd(chJoin);
				if ((pStrProp = (CStringProp *)pBag->FindProp(ListStr_AddPart(nIDExtraProp))) != (CStringProp *)NULL)
					optlstAdd.SetString(pStrProp->m_strVal);

				if (fInherit)	optlstVal.Subtract(optlstSub);	// no subtract, if no inherit
				optlstVal.Append(optlstAdd);
			}

			// re-set inheriting to old value
			((CProjItem *)pSlob)->EnablePropInherit(fPropInheritOld);
		}

		// only need to do this if we we'rere a got container
		// (ie. we're not the first node in this list)
		if (pSlob != pOrigSlob)
			((CProjItem *)pSlob)->ResetContainerConfig();
	}
	
	if (!fAnti && IsFakeProp(nIDExtraProp))
	{
		CString strAdd;

		SetSlob (pOrigSlob);
		GetDefStrProp (nIDExtraProp, strAdd);
		COptionList optlstAdd (strAdd);
		ResetSlob();

		optlstVal.Append (optlstAdd);
	} 
	// we need to convert our string list into a usable string
	// this will ensure all our join chars are chJoin
	optlstVal.GetString(strVal);
	
	return valid;
}

void COptionHandler::ClearStringLists(CPropBag * ppropbag)
{
	if (ppropbag == (CPropBag *)NULL)
		ppropbag = m_pSlob->GetPropBag();

	for (UINT i = 0; i < m_cStrListPropIds; i++)
	{
		// delete string list prop from the current property bag
		UINT idProp = m_pStrListPropIds[i];

		// always have both add/subtract if any at all - just check for add part
		if (ppropbag->FindProp(ListStr_AddPart(idProp)) != (CProp *)NULL)
		{
			// remove and inform of this prop change (update option view?)
			ppropbag->RemoveProp(ListStr_AddPart(idProp));
			ppropbag->RemoveProp(ListStr_SubPart(idProp));

			m_pSlob->InformDependants(idProp);
			OnOptionStrPropChange(idProp, _TEXT(""));
		}
	}
}

void COptionHandler::ClearCleanArgs(CPropBag * ppropbag, OptBehaviour optbeh)
{
	UINT nPropMin, nPropMax;
	GetMinMaxOptProps(nPropMin, nPropMax);

	if (ppropbag == (CPropBag *)NULL)
		ppropbag = m_pSlob->GetPropBag();

	UINT nIDUnknownOption, nIDUnknownString;
	GetSpecialOptProps(nIDUnknownOption, nIDUnknownString, optbeh);

	CProp * pprop;
	for (UINT nProp = nPropMin; nProp <= nPropMax; nProp++)
	{
		// don't do this for the unknown, or string options, or not dirtied args
		if (nProp == nIDUnknownOption || nProp == nIDUnknownString ||
			m_pdirtyProps->GetAt(nProp - nPropMin)
		   )
			continue;

		// was this a dirtied arg, if not then remove it
		if ((pprop = ppropbag->FindProp(nProp)) != (CProp *)NULL)
		{
			PROP_TYPE type = pprop->m_nType; 	// remember before delete

			// remove and inform of this prop deletion (it exists!)
			ppropbag->RemovePropAndInform(nProp, m_pSlob);

			if (type == integer || type == booln)
				OnOptionIntPropChange(nProp, 0);
			else
				OnOptionStrPropChange(nProp, _TEXT(""));
		}
	}
}

void COptionHandler::ClearSpecialOptProps(CPropBag * ppropbag, OptBehaviour optbeh)
{
	if (ppropbag == (CPropBag *)NULL)
		ppropbag = m_pSlob->GetPropBag();

	UINT nIDUnknownOption, nIDUnknownString;
	GetSpecialOptProps(nIDUnknownOption, nIDUnknownString, optbeh);

	// remove and inform deps. if the unknown props. exist in our bag
	ppropbag->RemovePropAndInform(nIDUnknownOption, m_pSlob);
	ppropbag->RemovePropAndInform(nIDUnknownString, m_pSlob);
}

BOOL COptionHandler::HasAnyOfOurProperties (CProjItem *pProjItem, BOOL bCheckFakeProps /* = FALSE */)
{
	ASSERT (!pProjItem->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	ASSERT (pProjItem->IsKindOf(RUNTIME_CLASS(CProjItem)));

	// if this is a CTargetItem then convert into a CProject...
	if (pProjItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
		pProjItem = pProjItem->GetProject();

	CPropBag * pPropBag = pProjItem->GetPropBag();

	UINT nPropMin, nPropMax;
	GetMinMaxOptProps(nPropMin, nPropMax);

	int idOldBag = pProjItem->UsePropertyBag(BaseBag);
	CPropBag * pBasePropBag = pProjItem->GetPropBag();
	(void) pProjItem->UsePropertyBag(idOldBag);

	BOOL retVal = FALSE;

	// additional data only used if bCheckFakeProps is TRUE (init below)
	CProject * pProject;
	OptBehaviour optbehOld, optbehProjOld;
	CPropBag  * pProjBag;
	CString strProp, strProjProp;

	// only implemented for projects
	bCheckFakeProps = bCheckFakeProps && !pProjItem->IsKindOf(RUNTIME_CLASS(CProject));

	if (bCheckFakeProps)
	{	
		// additional optional initialization needed only for bCheckFakeProps
		pProject = pProjItem->GetProject();
		ASSERT(pProject);

		// is this project item in a forced config.? if so then force the project in the same...
		if (pProjItem->m_pForcedConfig)
			pProject->ForceConfigActive((ConfigurationRecord *)pProjItem->m_pForcedConfig->m_pBaseRecord);

		pProjBag = ((ConfigurationRecord *)pProject->GetActiveConfig())->GetPropBag(CurrBag);
		ASSERT(pProjBag == ((ConfigurationRecord *)pProjItem->GetActiveConfig()->m_pBaseRecord)->GetPropBag(CurrBag));

		// set our own option behaviour, and our project's
		optbehOld = pProjItem->SetOptBehaviour(OBShowMacro | OBInherit | OBShowDefault);
		optbehProjOld = pProject->SetOptBehaviour(OBShowMacro | OBInherit | OBShowDefault);
	}

	// for all of our properties that this table uses, check to
	// see if we have any of them in our bag
	for (UINT nProp = nPropMin; nProp <= nPropMax; nProp++)
	{
		// is this in the current prop bag, or in the base prop bag?
		if ((pPropBag->FindProp(nProp) != (CProp *)NULL) ||
			(pBasePropBag->FindProp(nProp) != (CProp *)NULL))
		{
			retVal = TRUE;
			break;
		}

		// optionally also check fake props (not for ADD/SUB lines)
		if (bCheckFakeProps && IsFakeProp(nProp) &&
			((pProjBag->FindProp(nProp)==NULL) || IsListStrProp(nProp))
		   )
		{
			if (!SetsFakePropValue(nProp))
			{
				// the slob is hinting to us that its value for this property is
				// the same as the container's.
				ASSERT(!retVal);
				continue;
			}
			switch (GetDefOptionType(nProp)) {
				case string :
					if ((GetDefStrProp(nProp, strProp)==valid) && (!strProp.IsEmpty()))
					{
						ASSERT(m_pSlob==pProjItem);
						if (pProjItem->m_pForcedConfig)	// may have been reset by GetDefStrProp call above!
							pProject->ForceConfigActive((ConfigurationRecord *)pProjItem->m_pForcedConfig->m_pBaseRecord);

						m_pSlob = (CSlob *) pProject;
						if ((GetDefStrProp(nProp, strProjProp)!=valid) || (strProjProp.IsEmpty()) || (strProjProp!=strProp))
							retVal = TRUE;
						m_pSlob = (CSlob *) pProjItem;
					}
					break;
				case integer :
				case booln :
					// ignore default int props for now
					break;
				default :
					ASSERT(0);	// only types implemented
			};
			if (retVal)
				break;
		}
	}
	if (bCheckFakeProps)
	{
		pProjItem->SetOptBehaviour(optbehOld);
		pProject->SetOptBehaviour(optbehProjOld);
		pProject->ForceConfigActive();
	}

	if (retVal)
		return TRUE;

	// FUTURE: only need to check OutDirs once, not in each recursion!

	// Also look for the output directories props.  These are really
	// props that can affect all tools.  No need to check Base props here.
	if (pPropBag->FindProp(P_OutDirs_Intermediate) && !m_pAssociatedBuildTool->m_fTargetTool)
		return TRUE;

	if (pPropBag->FindProp(P_OutDirs_Target) && m_pAssociatedBuildTool->m_fTargetTool)
		return TRUE;

	// if we can't find anything, check if it has a BaseOptionHandler and 
	// it does, check if the base has the properties
	COptionHandler * popthdlrBase = GetBaseOptionHandler();
	if (popthdlrBase)
		return popthdlrBase->HasAnyOfOurProperties(pProjItem, bCheckFakeProps);

	return FALSE;
}

BOOL CompareProps(const CProp *, const CProp *); // Rvw: defined in projitem.cpp

BOOL COptionHandler::HasPerConfigProperties(CProjItem *pProjItem, BOOL bCheckBase /* = TRUE */ )
{
	ASSERT (!pProjItem->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	ASSERT (pProjItem->IsKindOf(RUNTIME_CLASS(CProjItem)));
	int numBags = pProjItem->GetPropBagCount();
	if (numBags < 2)
		return 0;	// nothing to compare

	int retFromBase = 0, retVal = 0; // defaults
	UINT nPropMin, nPropMax;
	GetMinMaxOptProps(nPropMin, nPropMax);

	if (nPropMin != (UINT)-1 && nPropMax != (UINT)-1)
	{
		const CPtrArray & ppcr = *pProjItem->GetConfigArray();
		ConfigurationRecord * pcr;
		CProp * pProp, * pCmpProp, * pCmpBaseProp;
		int i;

		// for all of our properties that this table uses, check to
		// see if we have any of them in our bag
		for (UINT nProp = nPropMin; nProp <= nPropMax; nProp++)
		{
			for (i = 0; i < numBags; i++)
			{
				pcr = (ConfigurationRecord *)ppcr[i];

				pProp = pcr->GetPropBag(CurrBag)->FindProp(nProp);

				if (i==0)
					pCmpProp = pProp;
				else if (pProp)
				{
					if ((!pCmpProp) || !CompareProps(pProp, pCmpProp))
						return 1;
				}
				else if (pCmpProp)
					return 1;

				if (bCheckBase && (!retVal))
				{
					// note: we can ignore fake props for base props
					pProp = pcr->GetPropBag(BaseBag)->FindProp(nProp);
					if (i==0)
						pCmpBaseProp = pProp;
					else if ((pProp==NULL) || (pCmpBaseProp==NULL))
					{
						if (pProp!=pCmpBaseProp)
							retVal = 2;
					}
					else if (!CompareProps(pProp, pCmpBaseProp))
						retVal = 2;
				}
			}
		}
	}

	// if we can't find anything, check if it has a BaseOptionHandler and 
	// it does, check if the base has the properties
	COptionHandler * popthdlrBase = GetBaseOptionHandler();
	if (popthdlrBase)
		retFromBase = popthdlrBase->HasPerConfigProperties(pProjItem, bCheckBase);

	return (retFromBase) ? retFromBase : retVal;
}

//////////////////////////////////////////////////////////////////////////////
// implementation of the COptHdlrUnknown class
// mechanism to allow for unknown option handlers

// we use this to retrieve a unique option property value each
// time an 'Uknown' option handler is CTORed
UINT g_nIDOptHdlrUnknown = 0;

// small option string table used to parse and generate uknown option lists
static COptStr g_poptstrOptHdlrUknown[] = {
	IDOPT_UNKNOWN_STRING,	"",		NO_OPTARGS, single,
	(UINT)-1,				NULL,	NO_OPTARGS, single
};

#define P_UnkHdkr_UnkStr_AddL	0
#define P_UnkHdlr_UnkStr_SubL	1

COptHdlrUnknown::COptHdlrUnknown(CBuildTool * pBuildTool) : COptionHandler(pBuildTool)
{
	// our base
	SetOptPropBase(PROJ_OPT_UNKNOWN + g_nIDOptHdlrUnknown);

	// we'll be using 2 off from this base
	// logical prop 0 is unknown string (add component)	== P_UnkHdkr_UnkStr_AddL
	// logical prop 1 is unknown string (subtract component) == P_UnkHdlr_UnkStr_SubL
	g_nIDOptHdlrUnknown += 2;

	ASSERT(g_nIDOptHdlrUnknown <= 100); // 100 should be plenty (allow for 50 unknown tools)

	Initialise();
}	

void COptHdlrUnknown::GetSpecialLogicalOptProps(UINT & nIDUnkOpt, UINT & nIDUnkStr, OptBehaviour optbeh)
{
	ASSERT_VALID(m_pSlob);

	nIDUnkOpt = (UINT) -1;
	nIDUnkStr = (optbeh & OBAnti) ? P_UnkHdlr_UnkStr_SubL : P_UnkHdkr_UnkStr_AddL;
}

// retrieive our min. and max. option prop IDs
void COptHdlrUnknown::GetMinMaxLogicalOptProps(UINT & nIDMinProp, UINT & nIDMaxProp)
{
	nIDMinProp = P_UnkHdkr_UnkStr_AddL; nIDMaxProp = P_UnkHdlr_UnkStr_SubL;
}

COptStr * COptHdlrUnknown::GetOptionStringTable()
{
	return (COptStr *)g_poptstrOptHdlrUknown;
}

//////////////////////////////////////////////////////////////////////////////
// implementation of the COptHdlrMapper class
COptHdlrMapper::COptHdlrMapper()
{
}

COptHdlrMapper::~COptHdlrMapper()
{
}

// get the right mapper option handler (used by COptHdlrMapper::MapLogical())
void COptHdlrMapper::SetMapperOptionHandler(UINT cDepth)
{
	UINT cStep = m_popthdlr->GetDepth() - cDepth;  
	m_popthdlrMapper = m_popthdlr;
	while (cStep--)
		m_popthdlrMapper = m_popthdlrMapper->GetBaseOptionHandler();

	m_cDepth = cDepth;
}

//////////////////////////////////////////////////////////////////////////////
// implementation of the COptionLookup class
// optimized lookup for the options
COptionLookup::~COptionLookup()
{
	WORD		keyDummy;
	CLookupEntry *	pentry;
	POSITION pos = m_mapChToPtr.GetStartPosition();

	while (pos != (POSITION)NULL)
	{
		 m_mapChToPtr.GetNextAssoc(pos, keyDummy, (void *&)pentry);
		 delete pentry;
	}
}

BOOL COptionLookup::FInit(COptStr * poptstr)
{
	COptStr	* poptstrOur = poptstr;

	//
	// create our lookup struct. for 'based on ID'
	//

	// what is the lowest ID we have in this table?
	m_idLowest = UINT_MAX;	// this is the highest
	while (poptstrOur->idOption != (UINT)-1)
	{
		// no need to do these!
		if (poptstrOur->idOption != IDOPT_UNKNOWN_OPTION &&
	    	poptstrOur->idOption != IDOPT_UNKNOWN_STRING)				
		{
			if (poptstrOur->idOption < m_idLowest)	m_idLowest = poptstrOur->idOption;
		}
		poptstrOur++;
	}

	// now using this lowest ID as the base create our
	// pointer array
	poptstrOur = poptstr;
	while (poptstrOur->idOption != (UINT)-1)
	{
		// no need to do these
		if (poptstrOur->idOption != IDOPT_UNKNOWN_OPTION &&
	    	poptstrOur->idOption != IDOPT_UNKNOWN_STRING)				
		{
			m_ptrIDArray.SetAtGrow(poptstrOur->idOption - m_idLowest, (void *)poptstrOur);
		}
		poptstrOur++;
	}

	// free up any extra
	m_ptrIDArray.FreeExtra();

	//
	// create our lookup struct. for 'based on char'
	//

	// init. our lookup char.
	m_chLookup = _T('\0');

	// now use an insertion sort to alphabetically sort our options (on first letter only)

	int iChArray = 0;
	unsigned char ch;

	poptstrOur = poptstr;

	while (poptstrOur->idOption != (UINT)-1)
	{
		// don't do this for our special ones!
		if (poptstrOur->idOption == IDOPT_UNKNOWN_OPTION ||
		    poptstrOur->idOption == IDOPT_UNKNOWN_STRING)
		{
			poptstrOur++;
			continue;
		}

		COptStr * poptstr;

		ch = (unsigned char)*(poptstrOur->szOption);

		iChArray = 0;
		while (iChArray <= m_ptrChArray.GetUpperBound())
		{
			poptstr = (COptStr *)m_ptrChArray[iChArray];

			// UNICODE ALERT:             
 			if ((unsigned char)*(poptstr->szOption) > ch)
				break; // break-out here

			iChArray++;
		}

		// insert at this position
		m_ptrChArray.InsertAt(iChArray, (void *)poptstrOur);
		poptstrOur++;
	}

	// free up any extra
	m_ptrChArray.FreeExtra();

	// now go through our list once more and for each first char.
	// put it's index into our map

	CLookupEntry * pentry = (CLookupEntry *)NULL;
	iChArray = 0; ch = _T('\0');	// last char.

	m_mapChToPtr.InitHashTable(53);	// prime no. >= 26 * 2 (alpha letters upper+lower)
	while (iChArray <= m_ptrChArray.GetUpperBound())
	{
		// don't do this for our special ones!
		if (poptstrOur->idOption == IDOPT_UNKNOWN_OPTION ||
		    poptstrOur->idOption == IDOPT_UNKNOWN_STRING)
		{
			iChArray++;
			continue;
		}

		COptStr * poptstr;

		poptstr = (COptStr *)m_ptrChArray[iChArray];

		// UNICODE ALERT:             
		// N.B. guaranteed to hit on first iteration
		if ((unsigned char)*(poptstr->szOption) != ch)
		{
			// fill in 'first char'
			pentry = new CLookupEntry;
			pentry->poptstr = poptstr;
			pentry->ioptstr = (UINT)-1;	// by default next is not available

			ch = (unsigned char)*(poptstr->szOption);
			m_mapChToPtr.SetAt((WORD)ch, (void *)pentry);
		}
		else if (pentry != (CLookupEntry *)NULL)
		{
			// fill in 'next char'
			pentry->ioptstr = iChArray;
			pentry = (CLookupEntry *)NULL;
		}

 		iChArray++;
	}

	return TRUE;
}

inline COptStr * COptionLookup::Lookup(UINT nID)
{
	// no need to do these!
	if (nID == IDOPT_UNKNOWN_OPTION ||
    	nID == IDOPT_UNKNOWN_STRING)
	{
		ASSERT(FALSE);
		return (COptStr *)NULL;
	}
	
	// must be within our bounds
	if ((nID >= m_idLowest) && (m_ptrIDArray.GetUpperBound() >= (int)(nID - m_idLowest)))
		return (COptStr *)m_ptrIDArray[nID - m_idLowest];

	return (COptStr *)NULL;	 // not found
}

inline COptStr * COptionLookup::LookupFirst(unsigned char ch, BOOL fCaseSensitive)
{
	CLookupEntry * pentry;

	if (m_mapChToPtr.Lookup((WORD)ch, (void *&)pentry) ||
		(!fCaseSensitive &&
		 m_mapChToPtr.Lookup((WORD)(isupper(ch) ? tolower(ch) : toupper(ch)), (void *&)pentry)))
	{
		// remember our char. plus first position
		m_chLookup = ch; m_ioptstr = pentry->ioptstr;
		return pentry->poptstr;
	}

	return (COptStr *)NULL;
}

inline COptStr * COptionLookup::LookupNext(int cchLead)
{
	// no lookup entry, or no next one to lookup
	if (m_ioptstr == (UINT)-1)	return (COptStr *)NULL;

	COptStr * poptstr = (COptStr *)m_ptrChArray[m_ioptstr];

	// is this next one valid?
	TCHAR * pch = (TCHAR *)poptstr->szOption;
	TCHAR ch = *pch;
	if (ch == m_chLookup ||
		((isupper(ch) ? tolower(ch) : toupper(ch)) == m_chLookup))
	{
		if ((int)++m_ioptstr > m_ptrChArray.GetUpperBound())
			m_ioptstr = (UINT)-1;
	}
	else
		m_ioptstr = (UINT)-1;	// no longer available

	// check the lead char. minimum
	if (poptstr)
	{
		int cchThisLead = 0;
		while (*pch != _T('\0') && *pch != _T('%'))
		{
			cchThisLead++;
			pch++;	// N.B. no Kanjii in tool option specifications
		}

		if (cchLead > cchThisLead)
			poptstr = (COptStr *)NULL;
	}
                          
	return poptstr;
}

// implementation of the COptionTable class

COptionTable::COptionTable()
{
	// init. our arg property list index
	for (UINT i = 0 ; i < MAX_OPT_ARGS ; i++)
		m_rgProps[i].idArg = (UINT)-1;

	// our list string cache
	m_pBagCache = (CPropBag *)NULL;

	// our default option prefixes
	strPrefixes = "/-";

	// our arg. value allocated to be 64
	pchArgValue = new char [cchArgValue = 64];

	// set up our default option behaviour
	m_optbeh = OBNone;
}

COptionTable::~COptionTable()
{
	// delete our arg property list
	ClearPropValList();

	// delete our arg. value if we had one
	if (pchArgValue != (TCHAR *)NULL)
		delete [] pchArgValue;
}


// set the option handler given a package name and component pair
COptionHandler * COptionTable::SetOptionHandler(const TCHAR * szPkg, WORD idOptHdlr)
{
	// generate this option handler's component id
	DWORD bsc_idOptHdlr = GenerateComponentId(g_prjcompmgr.GenerateBldSysCompPackageId(szPkg), idOptHdlr);
	
	// look it up
	COptionHandler * popthdlr;
	if (!g_prjcompmgr.LookupBldSysComp(bsc_idOptHdlr, (CBldSysCmp * &)popthdlr))
		popthdlr = (COptionHandler *)NULL;

	ASSERT(popthdlr->IsKindOf(RUNTIME_CLASS(COptionHandler)));

	// set this option handler?
	if (popthdlr != (COptionHandler *)NULL)
		SetOptionHandler(popthdlr);

	// return it
	return popthdlr;
}

void COptionTable::ClearPropertyBag()
{
	COptStr *	poptstr;

	poptstr = m_popthdlr->GetOptionStringTable();
	ASSERT(poptstr != (COptStr *)NULL);
	while (poptstr->idOption != (UINT)-1)
	{
		ClearPropVal(poptstr->rgidArg);
		poptstr++;	// next option args. to clear from prop.bag
	}
}


BOOL COptionTable::ParseString(CString & strOption, OptBehaviour optbeh)
{
	ASSERT(m_popthdlr);

	// set our option flags
	m_optbeh = optbeh;

	// can't have set defaults for root CSlob's
	if (m_pSlob->GetContainer() == (CSlob *)NULL)
		m_optbeh &= ~OBSetDefault;

	// don't clear props for anti-options!
	if (m_optbeh & OBAnti)
		m_optbeh &= ~OBClear;

 	// set the prop. bag usage
	int idOldBag = ((CProjItem *)m_pSlob)->UsePropertyBag((m_optbeh & OBBasePropBag) ? BaseBag : CurrBag);

	CPropBag * 	ppropbag = m_pSlob->GetPropBag();
	COptionHandler * popthdlr;

	popthdlr = m_popthdlr;
	while (popthdlr != (COptionHandler *)NULL)
	{
		// reset
		popthdlr->ResetDirtyArgs();

		// FUTURE: optimize this?
	 	// clear our string list arg. components for each option handler
		if (m_optbeh & OBClear)
			popthdlr->ClearStringLists(ppropbag);

		popthdlr = popthdlr->GetBaseOptionHandler(); // get our base-handler
	}		

	// create our unknown option/string buffers
	CString	strUnknownOption, strUnknownString, strPart;
	UINT nIDUnknownOption, nIDUnknownString;
	m_popthdlr->GetSpecialOptProps(nIDUnknownOption, nIDUnknownString, m_optbeh);

	// We want to append to any previously-read options/strings for this projitem
	// N.B. AppWizard depends on this behavior!

	// append any previous unknown strings/options if we are not clearing
	// ie. this might be multiple Add/Subtract lines in the makefile
	// (AppWizard generates .MAKs like this)
	if (!(m_optbeh & OBClear))
	{
		// don't want inheritance enabled for this
		OptBehaviour optbehOld = ((CProjItem *)m_pSlob)->GetOptBehaviour();
		((CProjItem *)m_pSlob)->SetOptBehaviour(optbehOld & ~OBInherit);

		if (nIDUnknownOption != (UINT)-1 &&
			m_pSlob->GetStrProp(nIDUnknownOption, strUnknownOption) == valid && !strUnknownOption.IsEmpty()
		   )
			strUnknownOption += _T(' ');

		if (nIDUnknownString != (UINT)-1 &&
			m_pSlob->GetStrProp(nIDUnknownString, strUnknownString) == valid && !strUnknownString.IsEmpty()
		   )
			strUnknownString += _T(' ');

		((CProjItem *)m_pSlob)->SetOptBehaviour(optbehOld);
	}

	TCHAR chSrc, chPrefix;
	BOOL fReturn = TRUE;	// default is to pass

	// reset for this new option string
	strSrc = strOption; ichSrc = 0;

	// break out on parse completion, ie. found terminator (chSrc == _T('\0'))
	for (;;)
	{
		// skip any leading whitespace
		cchAdvSrc = 0; SkipWhiteSpace(); ichSrc += cchAdvSrc;

		// reset our advancement pointers
		cchAdvSrc = cchAdvSrcToArg = 0;

		chSrc = NewGetAt(strSrc, ichSrc);

		// parsed all of the source?
		if (chSrc == _T('\0'))	break;

		BOOL fStringTrash = TRUE;	// default is 'it is trash!'

		// are we on a option prefix?
		BOOL fHasPrefix = strPrefixes.Find(chSrc) != -1;
		if (fHasPrefix)
		{
			// remember this prefix
			chPrefix = chSrc;

			// yes, this is an option
			BOOL		fFoundMatch = FALSE;

			ichSrc++;	// move over prefix

			// our option to try and parse with
			COptStr * poptstr;

			// our current fast-lookup object (one for each option handler)
			COptionLookup * poptlookup;

			// for each of our option handler lookup objects do this
			COptionHandler * popthdlrOld = m_popthdlr;	// start off at ourselves

			while (!fFoundMatch && m_popthdlr != (COptionHandler *)NULL)
			{
				// cache some values from this option handler
				m_popthdlr->GetMinMaxOptProps(m_nPropMin, m_nPropMax);
				m_fCaseSensitive = m_popthdlr->IsCaseSensitive();

				// get our fast-lookup object
				poptlookup = m_popthdlr->GetOptionLookup();

				// yes, so try to match each of our options
				poptstr = poptlookup ?
						  poptlookup->LookupFirst(NewGetAt(strSrc, ichSrc), m_fCaseSensitive) : (COptStr *)NULL;

				// we share the lookup list with our base + derived option tables
				// is this a valid option string?
				int cchMinLead = 0;
				while (!fFoundMatch && poptstr != (COptStr *)NULL)
				{
					fFoundMatch = MatchOption(poptstr);

					// did we find a match?
					if (fFoundMatch)
					{
						// yes, advance our source pointer
						ichSrc += cchAdvSrc;
					}
					else
					{
						if (cchAdvSrcToArg > cchMinLead)	cchMinLead = cchAdvSrcToArg;

						// no, get next option to try to match
						// (only get those options with *more* leading chars
						// we could match, ie. not /F% if we match /FI%)
						poptstr = poptlookup->LookupNext(cchMinLead);
					}

					cchAdvSrc = cchAdvSrcToArg = 0;	// reset our advancement pointers
				}

				m_popthdlr = m_popthdlr->GetBaseOptionHandler(); // get our base-handler
			}

			m_popthdlr = popthdlrOld;

			// did we find a match? ie. is this option trash?
			fStringTrash = !fFoundMatch;

			if (fStringTrash && nIDUnknownOption != (UINT)-1)
			{
				// collect together the unknown option
				strPart = ""; CollectOptionPart(strPart, FALSE, TRUE, FALSE, TRUE);

				// append part, terminate with a space
				AdjustBuffer(strUnknownOption);
				strUnknownOption += chPrefix;
				strUnknownOption += strPart;
				strUnknownOption += _T(' ');

				ichSrc += cchAdvSrc;	// advance our source pointer
				fStringTrash = FALSE;	// processed as 'option' string trash
			}
		}
		
		// is this just string trash?
		if (fStringTrash)
		{
			// collect together the trash (note: any collection of characters)
			strPart = ""; CollectOptionPart(strPart, FALSE, TRUE, TRUE, TRUE);
			ichSrc += cchAdvSrc;	// advance our source pointer

			// must have at least an unknown string unless we're Anti
			ASSERT((nIDUnknownString != (UINT)- 1) || (m_optbeh & OBAnti));
			if (nIDUnknownString != (UINT)-1)
			{
				// append part, terminate with a space
				AdjustBuffer(strUnknownString);
				if (fHasPrefix)	strUnknownString += chPrefix;
			  	strUnknownString += strPart;
				strUnknownString += _T(' ');
			}
		}
	}

	// set the unknown option property, and
	// set the unknown string property
	if (nIDUnknownOption != (UINT)-1)
		m_pSlob->SetStrProp(nIDUnknownOption, strUnknownOption);

	if (nIDUnknownString != (UINT)-1)
		m_pSlob->SetStrProp(nIDUnknownString, strUnknownString);

	// do we want to clear out non-dirty items?
	// clear any properties we didn't set
	if (m_optbeh & OBClear)
	{
		popthdlr = m_popthdlr;
		while (popthdlr != (COptionHandler *)NULL)
		{
			// FUTURE: optimize this?
			popthdlr->ClearCleanArgs(ppropbag);
			popthdlr = popthdlr->GetBaseOptionHandler(); // get our base-handler
		}		
	}

   	// reset the prop. bag usage
 	(void) ((CProjItem *)m_pSlob)->UsePropertyBag(idOldBag);

	return fReturn;	// success or failure?
}

void COptionTable::GenerateAppendUnknown(CString & strOption, UINT idOpt)
{
	CString strVal;

	// we need to inheriting!
	// get the prop for each of our ancestor's in elder first order
	CSlob * pSlob = m_pSlob;
	CObList obList;
	if (m_optbeh & OBInherit)
	{
		while (pSlob != (CSlob *)NULL)
		{
			obList.AddHead(pSlob);
			pSlob = ((CProjItem *)pSlob)->GetContainerInSameConfig();	// get our container
		}
	}
	else
	{
		// we can do a no-inheritance thing!
		obList.AddHead(pSlob);
	}

	// if we are a CProjItem and we are inheriting then do our own thing
	// ie. go through each CSlob in the container hierarchy and append each part to 
	// our options
	CString strExtraPart;

	POSITION pos = obList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		pSlob = (CSlob *)obList.GetNext(pos);

		// CProjGroups don't support tool options!
		if (!pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			// disable inheriting so we can do it!
			BOOL fPropInheritOld = ((CProjItem *)pSlob)->EnablePropInherit(FALSE);

			// use strings
			if (pSlob->GetStrProp(idOpt, strExtraPart) != valid)
				strExtraPart = "";
			else
			if (!strExtraPart.IsEmpty())
			{
				if (!strVal.IsEmpty())
					strVal += _T(' ');
	
				strVal += strExtraPart;	// append
			}

			// re-set inheriting to old value
			((CProjItem *)pSlob)->EnablePropInherit(fPropInheritOld);
		}

		if (pSlob != m_pSlob)
			((CProjItem *)pSlob)->ResetContainerConfig();
	}

	// anything to append?
	if (!strVal.IsEmpty())
	{
		// yes

		UINT cch = strOption.GetLength();
		const TCHAR * pc = (const TCHAR *)NULL;
		if (cch != 0)
		{
			pc = (const TCHAR *) strOption + cch;
			pc = _tcsdec((const TCHAR *) strOption, (TCHAR *)pc);
		}

		// do we need to prepend a space?
		AdjustBuffer(strOption);
		if ((pc != NULL) && (*pc != _T(' '))) strOption += _T(' ');

		strOption += strVal;

		// we know strVal is non-empty so don't check cch != 0
		cch = strVal.GetLength();
		pc = (const TCHAR *) strVal + cch;
		pc = _tcsdec((const TCHAR *) strVal, (TCHAR *)pc);
		// do we need to append a space?
		if ((pc != NULL) && (*pc != _T(' '))) strOption += _T(' ');
	}
}

BOOL COptionTable::GenerateString(CString & strOption, OptBehaviour optbeh)
{
	ASSERT(m_popthdlr);

	// original 'option handler'
	COptionHandler * popthdlrOld = m_popthdlr;

	// set our option flags
	m_optbeh = optbeh;

	// being a root node implies inheritance by default
	if (m_pSlob->GetContainer() == (CSlob *)NULL)
		m_optbeh |= OBInherit;

	// no inheritance for anti-options
	if (m_optbeh & OBAnti)
		m_optbeh &= ~OBInherit;

	// don't use defaults to gen. with
	m_fUseDefault = FALSE;

	// set up our option behaviour
	// (selectively mask the current behaviour)
	OptBehaviour optbehOld = ((CProjItem *)m_pSlob)->GetOptBehaviour();

	optbeh = optbehOld;

	if (m_optbeh & OBInherit)
		optbeh |= OBInherit;
	else
		optbeh &= ~OBInherit;

 	(void)((CProjItem *)m_pSlob)->SetOptBehaviour(optbeh);

	// set the prop. bag usage
	int idOldBag = ((CProjItem *)m_pSlob)->UsePropertyBag((m_optbeh & OBBasePropBag) ? BaseBag : CurrBag);

	// iterate through our option table trying to generate
	// each option into a string and append it to our options

	// prefix to use
	CString strPrefix(strPrefixes[0]);

	UINT nIDUnknownOption, nIDUnknownString;
	m_popthdlr->GetSpecialOptProps(nIDUnknownOption, nIDUnknownString, m_optbeh);

	// create our list of pointers to option handlers
	CPtrList lstopthdlrs; UINT iopthdlr = 0;
	COptionHandler * popthdlr = m_popthdlr;
	while (popthdlr != (COptionHandler *)NULL)
	{
		lstopthdlrs.AddHead(popthdlr);
		popthdlr = popthdlr->GetBaseOptionHandler();
	}

	// our stack of remembered positions in option handler string
	// tables, this is initially empty
	CPtrList stkpoptstr;

	// ptr. into the string table for this handler
	COptStr * poptstr = (COptStr *)NULL;

	for (;;)
	{
	OptHdlr_Recurse:

		// get the next current option handler
		POSITION pos = lstopthdlrs.FindIndex(iopthdlr);
		ASSERT(pos != (POSITION)NULL);	// must exist

		m_popthdlr = (COptionHandler *)lstopthdlrs.GetAt(pos);

		// begin a new table?
		if (poptstr == (COptStr *)NULL)
			poptstr = m_popthdlr->GetOptionStringTable();	// begin a new table

		while (poptstr->idOption != (UINT)-1)
		{									
			// is this a request to start processing the derived option handler string table?
			if (poptstr->idOption == IDOPT_DERIVED_OPTHDLR)
			{
				poptstr++;
				
				// can we get a derived option handler?
				if (iopthdlr == (UINT)(lstopthdlrs.GetCount() - 1))
 					continue;	// can't do, continue
					
				// remember our next position in this table, and our current option handler
				stkpoptstr.AddTail(poptstr); ++iopthdlr; poptstr = (COptStr *)NULL;
				goto OptHdlr_Recurse;
			}
			// is this option the an collection of unknown strings or options?
			else if (poptstr->idOption == IDOPT_UNKNOWN_OPTION)
			{
				// yes, append our unknown options
				if (nIDUnknownOption != (UINT)-1 && !(m_optbeh & OBAnti))
					GenerateAppendUnknown(strOption, nIDUnknownOption);
			}
			else if (poptstr->idOption == IDOPT_UNKNOWN_STRING)
			{
				// yes, append out unknown strings (only for unk. opt. hdlr anti options)
				if (nIDUnknownString != (UINT)-1 &&
					(!(m_optbeh & OBAnti) || m_popthdlr->IsKindOf(RUNTIME_CLASS(COptHdlrUnknown)))
				   )
					GenerateAppendUnknown(strOption, nIDUnknownString);
			}
			else
			{
				(void) GenerateOption(strOption, poptstr, strPrefix, TRUE);
			}

			// next option to try to generate
			poptstr++;
		}

		// is this the last option string table?
		if (stkpoptstr.IsEmpty())
			break;

		// finished with this option handler and string table, restore last one
		poptstr = (COptStr *)stkpoptstr.RemoveTail();
		iopthdlr--;
	}

   	// reset the prop. bag usage							 
 	(void) ((CProjItem *)m_pSlob)->UsePropertyBag(idOldBag);

	// reset our option behaviour
	((CProjItem *)m_pSlob)->SetOptBehaviour(optbehOld);

	// reset our current option handler
	m_popthdlr = popthdlrOld;

	return TRUE;	// *always* success
}

BOOL COptionTable::GenerateSingleOption(UINT id, CString & strOption, OptBehaviour optbeh)
{
	ASSERT(m_popthdlr);

	COptStr * poptstr = m_popthdlr->GetOptionStringTable();
	ASSERT(poptstr != (COptStr *)NULL);

	// set our option flags
	m_optbeh = optbeh;
	// being a root node implies inheritance by default
	if (m_pSlob->GetContainer() == (CSlob *)NULL)
		m_optbeh |= OBInherit;

	// don't use defaults
	m_fUseDefault = FALSE;

 	// set the prop. bag usage
	int idOldBag = ((CProjItem *)m_pSlob)->UsePropertyBag((m_optbeh & OBBasePropBag) ? BaseBag : CurrBag);

	// prefix to use
	CString strPrefix(strPrefixes[0]);

	// set up the inheritance (if we can!)
	BOOL fPropInheritOld = ((CProjItem *)m_pSlob)->EnablePropInherit(m_optbeh & OBInherit);

	// remember the old option handler
	COptionHandler * popthdlrOld = m_popthdlr;

	// find the option we want then try to generate
	BOOL		fReturn = FALSE;	// default is to fail
	COptionHandler * popthdlr = m_popthdlr;
	while (!fReturn && popthdlr != (COptionHandler *)NULL)
	{
		COptionLookup * poptlookup = popthdlr->GetOptionLookup();
		poptstr = poptlookup ? poptlookup->Lookup(id) : (COptStr *)NULL;
		if (poptstr != (COptStr *)NULL)
		{
			// use this option handler to generate the single option string
			m_popthdlr = popthdlr;
	
			// generate the option
			fReturn = GenerateOption(strOption, poptstr, strPrefix, FALSE);
		}

		popthdlr = popthdlr->GetBaseOptionHandler();
	}

	// reset to the old option handler
	m_popthdlr = popthdlrOld;

	// reset the inheritance (if we can!)
	(void) ((CProjItem *)m_pSlob)->EnablePropInherit(fPropInheritOld);

   	// reset the prop. bag usage
 	(void) ((CProjItem *)m_pSlob)->UsePropertyBag(idOldBag);

	return fReturn;	// success or failure?
}

BOOL COptionTable::GenerateDescription
(
	CString &	strOut,
	BOOL &		fBlank
)
{
	// original 'option handler'
	COptionHandler * popthdlrOld = m_popthdlr;

	// we want to show the defaults if we are a child CSlob, otherwise don't
	CSlob * pSlobParent = m_pSlob->GetContainer();
	BOOL fSlobIsChild = pSlobParent != (CSlob *)NULL;
	m_optbeh = fSlobIsChild ? OBShowFake | OBShowDefault : OBNone;

	// set up the inheritance (if we can!)
	BOOL fPropInheritOldChild = ((CProjItem *)m_pSlob)->EnablePropInherit(TRUE);
 	BOOL fPropInheritOldParent = ((CProjItem *)pSlobParent)->EnablePropInherit(TRUE);

	// our prefix to use
	CString strPrefix(strPrefixes[0]), strEnglish;
	CString strOptionParent, strOptionChild;

	CString strLangAnd;	VERIFY(strLangAnd.LoadString(IDS_PROJECT_AND));
	CString strLangReplace;	VERIFY(strLangReplace.LoadString(IDS_PROJECT_REPLACES));
	CString strLangAndNot; VERIFY(strLangAndNot.LoadString(IDS_PROJECT_ANDNOT));

	// the options we inherit from. if any!
	if (!fSlobIsChild)
	{
		strOut.LoadString(IDS_TXT_DEF_OPT);
	}
	else if ((pSlobParent->IsKindOf(RUNTIME_CLASS(CProxySlob)) &&
			  ((CProxySlob *)pSlobParent)->IsSortOf(RUNTIME_CLASS(CProject)))
			 ||
			  pSlobParent->IsKindOf(RUNTIME_CLASS(CProject))
			 ||
			 (pSlobParent->IsKindOf(RUNTIME_CLASS(CProxySlob)) &&
			  ((CProxySlob *)pSlobParent)->IsSortOf(RUNTIME_CLASS(CProjGroup)))
			 ||
			 pSlobParent->IsKindOf(RUNTIME_CLASS(CProjGroup))
			 ||
			 (pSlobParent->IsKindOf(RUNTIME_CLASS(CProxySlob)) &&
			  ((CProxySlob *)pSlobParent)->IsSortOf(RUNTIME_CLASS(CTargetItem)))
			 ||
			 pSlobParent->IsKindOf(RUNTIME_CLASS(CTargetItem))
			)
	{
		strOut.LoadString(IDS_TXT_PRJ_OPT);
	}
	else
	{
		// This must be a multiple-selection CProxySlob with different container types.
		// Currently we don't generate a description for a multiple-selection and this
		// function is not called in this situation.
		ASSERT(FALSE);
	}

	// default is blank
	fBlank = TRUE;

	// create our list of pointers to option handlers
	CPtrList lstopthdlrs; UINT iopthdlr = 0;
	COptionHandler * popthdlr = m_popthdlr;
	while (popthdlr != (COptionHandler *)NULL)
	{
		lstopthdlrs.AddHead(popthdlr);
		popthdlr = popthdlr->GetBaseOptionHandler();
	}

	// our stack of remembered positions in option handler string
	// tables, this is initially empty
	CPtrList stkpoptstr;

	// ptr. into the string table for this handler
	COptStr * poptstr = (COptStr *)NULL;

	for (;;)
	{
	OptHdlr_Recurse:

		// get the next current option handler
		m_popthdlr = (COptionHandler *)lstopthdlrs.GetAt(lstopthdlrs.FindIndex(iopthdlr));

		// begin a new table?
		if (poptstr == (COptStr *)NULL)
			poptstr = m_popthdlr->GetOptionStringTable();	// begin a new table

		while (poptstr->idOption != (UINT)-1)
		{									
			// don't do this for the 'special' options!
			if (poptstr->idOption == IDOPT_UNKNOWN_OPTION ||
				poptstr->idOption == IDOPT_UNKNOWN_STRING)
			{
				poptstr++;

				continue;
			}
			// is this a request to start processing the derived option handler string table?
			else if (poptstr->idOption == IDOPT_DERIVED_OPTHDLR)
			{
				poptstr++;
				// can we get a derived option handler?
				if (iopthdlr == (UINT)(lstopthdlrs.GetCount() - 1))
 					continue;	// can't do, continue

				// remember our next position in this table, and our current option handler
				stkpoptstr.AddTail(poptstr); ++iopthdlr; poptstr = (COptStr *)NULL;
				goto OptHdlr_Recurse;
			}
		
			strOptionChild = "";	// clear

			// disable inheriting if we were a multiple option
			BOOL fMultipleOption;
			if (fMultipleOption = (poptstr->type == multiple))
				((CProjItem *)m_pSlob)->EnablePropInherit(FALSE);

			// generate the child option (don't use defaults!)
			m_fUseDefault = FALSE;

			(void) GenerateOption(strOptionChild, poptstr, strPrefix, FALSE);

			// can't do this if we are not a child CSlob
			if (fSlobIsChild)
			{ 
				// we had this option in the child CSlob
				CSlob * pSlob = m_pSlob;
				SetPropertyBag(pSlobParent);	// use our parent CSlob

				strOptionParent = "";

				// did we add an option?
				// if we did, try to generate the equivalent in the parent CSlob
				if (!strOptionChild.IsEmpty())
				{
					// don't do the replace for string lists, ie. multiple options
					if (poptstr->type != multiple) /* single option */
					{
						// generate the parent option (use defaults!)
						m_fUseDefault = TRUE;
				  		(void) GenerateOption(strOptionParent, poptstr, strPrefix, FALSE);
					}

					// shall we do the and text?
					if (strOptionParent.IsEmpty() || 
						strOptionChild != strOptionParent)	/* do and */
					{
						// let's use the ', and ' prefix first off
						strOut += strLangAnd;
						strOut += strOptionChild;
						if (poptstr->type != multiple &&
							!strOptionParent.IsEmpty())	 /* do replace */
						{
							// let's use the ' replaces ' prefix
							strOut += strLangReplace;
							strOut += strOptionParent;
						}
						fBlank = FALSE;
					}
				}
				else
				{
					// we didn't have this option in the child CSlob
					// did we have properties though?	
					if (m_fGenUsedProps)
					{						
						// generate the parent option (use defaults!)
						m_fUseDefault = TRUE;
				  		(void) GenerateOption(strOptionParent, poptstr, strPrefix, FALSE);

						// now construct our English text
						if (!strOptionParent.IsEmpty())
						{
							// let's use the ', and not ' prefix
							strOut += strLangAndNot;
							strOut += strOptionParent;
							fBlank = FALSE;
						}
					}
				}

	 			SetPropertyBag(pSlob);			// reset to our child CSlob

				// do we have a string list 'subtract' part? ie. is this a multiple option
				// (the above comment is strange .. some might say hacky .. but who gives a s**t!)
				if (fMultipleOption)
				{
					strOptionChild = "";

					// we get the subtract part using anti-options
					m_optbeh |= OBAnti; // set
			  		(void) GenerateOption(strOptionChild, poptstr, strPrefix, FALSE);

					// now construct our English text
					if (!strOptionChild.IsEmpty())
					{
						// let's use the ', and not ' prefix
						strOut += strLangAndNot;
						strOut += strOptionChild;
						fBlank = FALSE;
					}

					m_optbeh &= ~OBAnti; // clear
				}
			}
			else
			{
				// for non-child CSlob's do this
				// let's use the ', and ' prefix first off
				strOut += strLangAnd;
				strOut += strOptionChild;
				fBlank = FALSE;
			}
 
			// re-enable inheriting if we were a multiple option
			if (fMultipleOption)
				((CProjItem *)m_pSlob)->EnablePropInherit(TRUE);

			poptstr++;
		}

		// is this the last option string table?
		if (stkpoptstr.IsEmpty())
			break;

		// finished with this option handler and string table, restore last one
		poptstr = (COptStr *)stkpoptstr.RemoveTail();
		iopthdlr--;
	}

	// reset the inheritance (if we can!)
	(void) ((CProjItem *)m_pSlob)->EnablePropInherit(fPropInheritOldChild);
	(void) ((CProjItem *)pSlobParent)->EnablePropInherit(fPropInheritOldParent);

	// reset our current option handler
	m_popthdlr = popthdlrOld;

	return TRUE;
}

BOOL COptionTable::CollectOptionPart
(
	CString &	strOut,			// the collected option part
	BOOL		fAlphaNumOnly,	// allow only alpha-numeric
	BOOL		fAllowPunct,	// allow punctuation
	BOOL		fAllowKanji,	// allow Kanji characters
	BOOL		fKeepQuotes,	// keep the quotes?
	CString		strDelim		// any delimiters (could be empty)
)
{
	BOOL			fQuoted, fEscaped;
	UINT			ichFirst, cchAdvSrcOrig;
	TCHAR			* pchSrc, ch;

	// figure first char.
	ichFirst = ichSrc + cchAdvSrc; cchAdvSrcOrig = cchAdvSrc;

CollectOptionPart:

	fEscaped = FALSE;
	pchSrc = (TCHAR *)(const TCHAR *)strSrc + ichSrc + cchAdvSrc; 

	// move over the first quote if we have one
	if (fQuoted = (*pchSrc == _T('"')))
	{	
		// we're a quoted string
		pchSrc++; cchAdvSrc++;
		if (!fKeepQuotes)
		{
			ichFirst++; cchAdvSrcOrig++;
		}
	}

	// check for '/OPTION" /' where we are parsing
	// a partial typed string, we'll assume that whitespace
	// followed by an option delimiter is not a valid arg.
	int ichLeadWhiteSpace = cchAdvSrc;	// assume whitespace for quoted

	// the end of the option part is
	// o whitespace (if not quoted) or null terminator
	// o a double-quote (if not escaped)
	// o a non alpha-numeric if fAlphaNumOnly = TRUE
	// o a punctuation character ~= '=' and ~= ',' not a flag prefix
	// o we hit a delimiter
	while ((ch = *pchSrc) != _T('\0'))
	{
		int cch = _tclen(pchSrc); pchSrc += cch;

		// allow only alpha-numeric
		if (fAlphaNumOnly)
		{
			if (!isalnum(ch) && ch != _T('-') && ch != _T('+'))
				break;
		}
		// allow all alpha
		else if (isalpha(ch))
		{
			ichLeadWhiteSpace = 0;
		}
		else
		{
	#ifdef _MBCS
			if (!fAllowKanji && (_ismbbkana(ch) || _ismbblead(ch)))
				break;
	#endif

			// currently parsing a quoted string?
			if (fQuoted)
			{
				// yes
				// FUTURE: do we allow escaped 'quotes' in tool options?
				if (/*!fEscaped &&*/ ch == _T('"'))
					break;

				if (ichLeadWhiteSpace)
				{
					if (!_istspace(ch))
					{
						if (strPrefixes.Find(ch) != -1)
						{
							// back-up to the quote before the whitespace
							cchAdvSrc = ichLeadWhiteSpace;
							break;	// break here, we found '/OPTION"   /'
						}

				 		ichLeadWhiteSpace = 0;
					}
				}
			}
			else
			{
				//no
				// allow punctuation inside a quoted string!
				if (!fAllowPunct && ispunct(ch))
				{
					// FUTURE (matthewt): when we process '=' for defines correctly
					// then maybe we want to split the args, for now treat
					// as one arg.
					if (ch == _T('=') || ch == _T(','))
						break;

					if (strPrefixes.Find(ch) != -1)
						break;
				}

				if (_istspace(ch) || strDelim.Find(ch) != -1)
					break;	// break here, we found option delimiter (incls. whitespace)
			}
		}

		cchAdvSrc += cch;

		// do we have '\\' at the end of our string?
		if (fEscaped && fQuoted && ch == _T('\\') && *pchSrc == _T('"'))
			// skip back over the last one (ie. ignore it)
			cchAdvSrcOrig++;

		fEscaped = !fEscaped && ch == _T('\\');
	}

	UINT cchOut = cchAdvSrc - cchAdvSrcOrig;

 	// eat the final quote if we have one
	if (fQuoted && ch == _T('"'))
	{
		// we're a quoted string
		cchAdvSrc++;
		if (fKeepQuotes)	cchOut++;
	}

	// do it for the 2nd half of an arg., eg. /D foo="bar"
	if (!fQuoted && ch == _T('='))
	{
		// we're an equals sign
		cchAdvSrc++;

		// keep the quotes (if they exist) for second half of assignment
		// (ignore return of FALSE - we don't care if empty)
		fKeepQuotes = TRUE;
		goto CollectOptionPart;	// next half
	}

	// have we actually found any characters?
	if (!cchOut)
	{
		// do we need to have a quoted empty string
		// in order to succeed?
		if ((m_optbeh & OBNeedEmptyString) && !fQuoted)
			return FALSE;

		strOut = "";
	}
	else
	{
		ASSERT(ichFirst >= 0 && cchOut > 0);
		strOut += strSrc.Mid(ichFirst, cchOut);
	}

	return TRUE;	// got a match
}

void COptionTable::ExtractOptionArg(char * & pchExpr, UINT & cchExpr)
{
	char	chExpr = *pchExpr;
	TCHAR *	pch;
	UINT	cchArg = 0;

	// argument value pattern?
	//
	patArgValue = none;

	if (chExpr == _T('T') || chExpr == _T('F') ||
		chExpr == _T('t') || chExpr == _T('f'))
	{
		// booln arg
		pch = pchExpr++; cchArg++; // move over booln arg.
		patArgValue = booln;
	}
	else
	if (chExpr == _T('{') ||
		chExpr == _T('<'))
	{
		// what is our terminator char?
		TCHAR chTerm = chExpr == _T('{') ? _T('}') : _T('>');

		// choice or absolute arg
		pch = ++pchExpr; cchExpr--; // skip over '{' or '<'
		patArgValue = chExpr == _T('{') ? choice : absolute;

		char	chExpr, * pch;
									  
		// extract our '<...>' or '{...}' chunk
		chExpr = *pchExpr; pch = pchExpr;
		while (chExpr != chTerm)
		{
			chExpr = *pchExpr;	// we want' to stop just after the '}' or '>'!
			pchExpr = _tcsinc(pchExpr);
		}
		cchArg = pchExpr - pch;
	}

	cchExpr -= cchArg;

	// ensure that the arg buffer is big enough
	// for debug purposes only, the option table arg
	// 'chunks' are static
	ASSERT(cchArg < cchArgValue);
	_tcsncpy(pchArgValue, pch, cchArg); 
	pchArgValue[cchArg] = _T('\0');	// terminate
}

void COptionTable::ExtractPart(char * & pchExpr, UINT & cchExpr, char * & pchPart, UINT & cchPart)
{
	UINT	cBrackets = 1;		// used to count brackets
	char	chExpr;

	cchPart = 0;	// init. the count of chars in the part
	pchPart = ++pchExpr; cchExpr--;	// skip the '[', and remember the position

	// search for the corresponding close bracket
	while (TRUE)
	{
		chExpr = *pchExpr;
		ASSERT(chExpr != _T('\0'));	// must be able to find one!

		if (chExpr == _T('['))	cBrackets++;	// bump up bracket count
		else
		if (chExpr == _T(']'))	cBrackets--;	// bump down bracket count

		// have we found the match?
		if (cBrackets == 0)
			break;	// yes, so break-out of here

		UINT	cch = _tclen(pchExpr);
		cchPart += cch; pchExpr += cch; cchExpr -= cch;
	}

	pchExpr++; cchExpr--;	// terminate on the ']'
}

BOOL COptionTable::MatchChoice(UINT &nChoiceMatch, char * pchChoice, UINT * pidArg)
{
	UINT cchChoice;
	int cchAdvSrcKeep;
	char * pchStart;

	// signal any kind of match
	nChoiceMatch = 0;	// no match

	for (UINT nChoice = 1 ; /* we'll break out! */ ; nChoice++ /* next choice */)
	{
		pchStart = pchChoice;

		// find our choice expression
		while (*pchChoice != _T('|') && *pchChoice != _T('}'))
			pchChoice = _tcsinc(pchChoice);

		// do we have a choice to match?
		if (cchChoice = (pchChoice - pchStart))
		{
			cchAdvSrcKeep = cchAdvSrc;
			if (MatchExpression(pchStart, cchChoice, pidArg))
			{
				nChoiceMatch = nChoice;		// matched, or last resort match (cchAdvSrc == cchAdvSrcKeep)

				if (cchAdvSrc != cchAdvSrcKeep)	return TRUE;	// matched option

				// this choice matched no chars, ie. was purely arg matching
				// we will use this nChoice if no other option matches
			}
			cchAdvSrc = cchAdvSrcKeep; 	// no match
		}
		// this is an empty option
		// we will use this nChoice if no other option matches
		else
			nChoiceMatch = nChoice;	// last resort match	

		// skip over the '|'
		if (*pchChoice++ == _T('}'))
			return (nChoiceMatch != 0);	// no match possible?
										// ran out of choices, maybe last resort choice?
	}

	return TRUE;
}

BOOL COptionTable::GenerateChoice(CString & strOptionBit, UINT nChoice, char * pchChoice, UINT * pidArg)
{
	if (!nChoice)	return FALSE;	// no generation possible, no choice

	// look through the options for our one
	while (--nChoice != 0)
	{
		while (*pchChoice != _T('|') && *pchChoice != _T('}'))
			pchChoice = _tcsinc(pchChoice);

		// skip over the '|'
		if (*pchChoice++ == _T('}'))	return FALSE;	// no generation possible, ran out of choices
	}

	// now try to generate this option
	char *	pchStart = pchChoice;
	UINT	cchChoice = 0, cch;

	while (*pchChoice != _T('|') && *pchChoice != _T('}'))
	{
		cch = _tclen(pchChoice);
		pchChoice += cch; cchChoice += cch;
	}

	// FUTURE: do we want to generate 'empty' option bits (eg. empty string "")
	if (cchChoice == 0) return TRUE;

	// try option generation
	return GenerateExpression(strOptionBit, pchStart, cchChoice, pidArg);
}

																	   
BOOL COptionTable::MatchExpression(const char * pchExprConst, UINT cchExpr, UINT * pidArg)
{
	UINT cch, cArgs = 0;
	char * pchExpr = (char *)pchExprConst;
	unsigned char chExpr; // expression character

	CString	strOut; // output string
    
    char * pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our source                                        
                               
	// have we finished this pattern? we will break out on failure (return)
	while (cchExpr != 0)
	{
  		chExpr = *pchExpr;

 		// do we have an argument specified?
		if (chExpr == _T('%'))
		{
			// increment the number of args. found
			// first?
			if (++cArgs == 1)
				cchAdvSrcToArg = cchAdvSrc;	// remember the adv. src. to arg.

			cch = _tclen(pchExpr);
			pchExpr += cch; cchExpr -= cch; // move over arg. spec.
			chExpr = *pchExpr;

			// argument value?
			ExtractOptionArg(pchExpr, cchExpr);

			// argument index
			//
			UINT	iArg;		// our arg. index
			UINT	idArgProp;	// our arg. property
								  
			chExpr = *pchExpr;
			ASSERT(isdigit(chExpr));	// must be an arg. index!
			iArg = chExpr - '1';
			idArgProp = pidArg[iArg];	// retrieve this arg's property #
			cch = _tclen(pchExpr);
 			pchExpr += cch; cchExpr -= cch;	// move over arg. index

			BOOL fFake = m_popthdlr->IsFakeProp(idArgProp);
			BOOL fAlwaysShowDefault = m_popthdlr->AlwaysShowDefault(idArgProp);

			// what type of property is this?
			// we want to match in the source the property value
			switch (m_popthdlr->GetDefOptionType(idArgProp) /* property type */)
			{
				case ::integer:
				{
					int nVal;

					// do we want to match an choice?
					if (patArgValue == choice)
					{
						UINT uVal;

						// match a choice in the string
						if (!MatchChoice(uVal, pchArgValue, pidArg))
						{
							// for 0th choices we might have used subtract
							if (!(m_optbeh & OBAnti) || _tcsncmp(pchSrc, _TEXT("<none>"), 6))
								return FALSE;	// no match, failed to find choice

							cchAdvSrc += 6;	// skip over

							// consume this source
							pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our 'new' source                                        

							uVal = 0;	// 0th
						}

						nVal = (int)uVal;
					}
					// do we want to set an absolute?
					else if (patArgValue == absolute)
					{
						// convert this absolute into a numeric to suit
						// the subtype of this integer 
					   	VERIFY(m_popthdlr->ConvertFromStr(idArgProp, pchArgValue, nVal));
					}
					else
					{
						// just match an integer in the string
						if (!CollectOptionPart(strOut, TRUE) || strOut.IsEmpty())
							return FALSE;	// no match, failed to collect

						// got a match, consume this source
						pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our 'new' source                                        

						// do the conversion from a string into an integer
						// based on the sub-type
					   	if (!m_popthdlr->ConvertFromStr(idArgProp, strOut, nVal))
						{
							// couldn't convert due to bad numeric format
							// use existing value if one exists!
							CProp * pProp = m_pSlob->GetPropBag()->FindProp(idArgProp);
							if (pProp == (CProp *)NULL)
							{
								AddToPropList(iArg, idArgProp, NULL);
								break;	// doesn't exist, ignore ie. no pass, no fail
							}
							nVal = ((CIntProp *)pProp)->m_nVal;
						}
					}

					// don't set defaults, or fake props
					BOOL fSet;
					if (fFake)
						fSet = fAlwaysShowDefault;
					else
						fSet = fAlwaysShowDefault || (m_optbeh & OBSetDefault);

					if (!fSet && m_popthdlr->IsDefaultIntProp(idArgProp, nVal))
						break;

					CIntProp *	ppropInt = new CIntProp(nVal);
					AddToPropList(iArg, idArgProp, ppropInt);
					break;
				}
				
				case ::booln:
				{
					BOOL	bVal;
					BOOL	fSoftBool = FALSE;

					// do we want to match an choice?
					if (patArgValue == choice)
					{	
						UINT	nVal;

						// match a choice in the string
						if (!MatchChoice(nVal, pchArgValue, pidArg))
							return FALSE;	// no match, failed to find choice

						// got a match, consume this source
						pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our 'new' source 
						                                       
						// match an choice in the string
						bVal = (nVal == 1) ? FALSE : TRUE;	// convert to a booln
					}
					else
					{
						ASSERT(patArgValue == booln);
						// just set the property based in the booln arg value
						bVal = (*pchArgValue == _T('T') || (fSoftBool = (*pchArgValue == _T('t'))));
					}

					// don't set defaults, or fake props
					BOOL fSet;
					if (fFake)
						fSet = fAlwaysShowDefault;
					else
						fSet = fAlwaysShowDefault || (m_optbeh & OBSetDefault);

					if (!fSet && m_popthdlr->IsDefaultBoolProp(idArgProp, bVal))
						break;

					// do we need to invert the integer value?
					// FUTURE: don't do this for softbool values (remove when per-prop remove in .PRJ)
					if ((m_optbeh & OBAnti) && !fSoftBool) 
						bVal = !bVal;	// invert

					CBoolProp *	ppropBool = new CBoolProp(bVal);
					AddToPropList(iArg, idArgProp, ppropBool);
					break;
				}

				case ::string:
				{
					if (!CollectOptionPart(strOut))
						return FALSE;	// no match, failed to collect

					// got a match, consume this source
					pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our 'new' source                                        

					// don't set defaults, or fake props
					BOOL fSet;
					if (fFake)
						fSet = fAlwaysShowDefault;
					else
						fSet = fAlwaysShowDefault || (m_optbeh & OBSetDefault);

					if (!fSet && m_popthdlr->IsDefaultStringProp(idArgProp, strOut))
						break;

					CStringProp * ppropString = new CStringProp(strOut);
					AddToPropList(iArg, idArgProp, ppropString);
					break;
				}
					 
				case ::longint:	// not yet needed!
				case ::number:
					break;

				default:
					ASSERT(FALSE);
					break;
			}
		}
		// do we have an open bracket (optional part)?
		else if (chExpr == _T('['))
		{
			char * pchPart;
			UINT cchPart;

			// extract the optional part
			ExtractPart(pchExpr, cchExpr, pchPart, cchPart);

			// don't bother matching empty optional parts
			if (cchPart != 0)
			{
				NoChangedPropValList();	// reset, so we recognise args to back-out
				if (MatchExpression(pchPart, cchPart, pidArg))
					// got a match, consume this source
				    pchSrc = (char *)(const char *)strSrc + ichSrc + cchAdvSrc;	// our 'new' source                                        
				else
					// back-out args set in no match optional part
					ClearPropValList(TRUE);
			}

			// continue, this is matching an optional part
		}
		// is this a 'just' whitespace candidate?
		// only do this for 'single whitespace expressions'
		else if (cchExpr == 1 && chExpr == _T(' '))
		{
			UINT cchAdvSrcOrig = cchAdvSrc;

			SkipWhiteSpace();	// skip any  whitespace
			pchExpr++; cchExpr--;	// skip over this space in the expression
			pchSrc += (cchAdvSrc - cchAdvSrcOrig);	// advance our source
		}
		// else should just try to match this
		else
		{
			if (_tccmp(pchExpr, pchSrc) != 0)	// not a char. match
			{
				unsigned char chSrc = *pchSrc;

				// did this not match 'cos of only case insensitivity
				if (m_fCaseSensitive || (toupper(chExpr) != toupper(chSrc)))
					return FALSE;
			}

			cch = _tclen(pchExpr);
			pchExpr += cch; cchExpr -= cch;

			cch = _tclen(pchSrc);
			pchSrc += cch; cchAdvSrc += cch;
		}

		// continue trying to match source and expression
	}
	return TRUE;	// successful match
}

BOOL COptionTable::GenerateExpression(CString & strOptionBit, const char * pchExprConst, UINT cchExpr, UINT * pidArg)
{
	UINT	cch;
	char * 	pchExpr = (char *)pchExprConst;
	unsigned char chExpr;	// expression character

	if (cchExpr == 0)
		return FALSE;

	// have we finished this pattern? we will break out on failure (return)
	while (cchExpr != 0)
	{
		chExpr = *pchExpr;

		// do we have an argument specified?
		if (chExpr == _T('%'))
		{
			cch = _tclen(pchExpr);
			pchExpr += cch; cchExpr -= cch; // move over arg. spec.
			chExpr = *pchExpr;

			// argument value?
			ExtractOptionArg(pchExpr, cchExpr);

			// argument index
			//
			UINT	idArgProp;	// our arg. property
								  
			chExpr = *pchExpr;
			ASSERT(isdigit(chExpr));	// must be an arg. index!
			idArgProp = pidArg[chExpr - '1'];	// retrieve this arg's property #
			cch = _tclen(pchExpr);
 			pchExpr += cch; cchExpr -= cch;	// move over arg. index

			BOOL fFake = m_popthdlr->IsFakeProp(idArgProp);
			BOOL fAlwaysShowDefault = m_popthdlr->AlwaysShowDefault(idArgProp);

			// what type of property is this?
			// we want to generate in the output the property value
			switch (m_popthdlr->GetDefOptionType(idArgProp) /* property type */)
			{
				case ::integer:
				{
					int		nVal;
					BOOL	fAbort;

					ASSERT(!m_fMultipleGen);	// only for single

					// want to show the option defaults when generating an option string?
					BOOL fPropDefaultOld = ((CProjItem *)m_pSlob)->UseOptionDefaults
 										   	(!(m_optbeh & OBAnti) && (m_fUseDefault || fAlwaysShowDefault));

					// get the property value
					// can't generate invalid props!
					BOOL fValid = m_pSlob->GetIntProp(idArgProp, nVal) == valid;

 					// we don't want to show the option defaults when generating an option string!
					(void) ((CProjItem *)m_pSlob)->UseOptionDefaults(fPropDefaultOld);

  					// can't generate invalid props!
					if (!fValid)	return FALSE;

					// used a prop!
					m_fGenUsedProps = TRUE;

					BOOL fIgnore;

					if (fFake)
						fIgnore = fAlwaysShowDefault || (m_optbeh & OBShowFake);
					else
						fIgnore = fAlwaysShowDefault || (m_optbeh & OBShowDefault);

					// is this a default?
					// don't bother checking for always showing defaults
					// don't bother for fake props.
					BOOL fGenerate = fIgnore || !m_popthdlr->IsDefaultIntProp(idArgProp, nVal);
					if (!fGenerate)
						return FALSE;

					// do we want to generate a choice?
					if (patArgValue == choice)
					{
						if (fAbort = !GenerateChoice(strOptionBit, nVal, pchArgValue, pidArg))
						{
							// for 0th choices  we'll need to use subtract
							if (!nVal && (m_optbeh & OBAnti))
							{
								strOptionBit += _TEXT("<none>");	// none
								fAbort = FALSE;
							}
						}
					}
					// do we have a match in the absolute value?
					else if (patArgValue == absolute)
					{
						// convert this absolute into a numeric to suit
						// the subtype of this integer 
						int nAbsoluteVal;
					   	VERIFY(m_popthdlr->ConvertFromStr(idArgProp, pchArgValue, nAbsoluteVal));
						fAbort = (nVal != nAbsoluteVal);
					}
					// do we want to generate an integer?
					else
					{
						ASSERT(patArgValue == none);
						CString strVal;

						// do the conversion from an integer into a string
						// based on the sub-type
					   	VERIFY(m_popthdlr->ConvertToStr(idArgProp, nVal, strVal));
						strOptionBit += strVal;
						fAbort = FALSE;
					}

					if (fAbort)	return FALSE;	// no generate, failed to generate integer value
					break;
				}

				case ::booln:
				{
					ASSERT(!m_fMultipleGen);	// only for single

					BOOL bVal, fAbort;
					BOOL fSoftBool = patArgValue == booln &&
									(*pchArgValue == _T('t') ||
									 *pchArgValue == _T('f'));

					// want to show the option defaults when generating an option string?
					BOOL fPropDefaultOld = ((CProjItem *)m_pSlob)->UseOptionDefaults
										   	(!(m_optbeh & OBAnti) && (fSoftBool || m_fUseDefault || fAlwaysShowDefault));

					// get the property
					BOOL fValid = m_pSlob->GetIntProp(idArgProp, bVal) == valid;

					(void) ((CProjItem *)m_pSlob)->UseOptionDefaults(fPropDefaultOld);

					// can't generate invalid props!
					// FUTURE: don't do this for invalid anti-softbool (fix when per-prop sub. in .PRJ)
					if (!fValid && !(fSoftBool && (m_optbeh & OBAnti)))
						return FALSE;

					// used a prop.
					m_fGenUsedProps = TRUE;

 					BOOL fIgnore;

					if (fFake)
						fIgnore = fAlwaysShowDefault || (m_optbeh & OBShowFake);
					else
						fIgnore = fAlwaysShowDefault || (m_optbeh & OBShowDefault) || fSoftBool;

	  				// do we need to invert the returned value?
					// FUTURE: don't do this for softbool values (fix when per-prop sub. in .PRJ)
					if ((m_optbeh & OBAnti) && !fSoftBool)
						bVal = !bVal;	// invert

					// is this a default?
					// don't bother checking for booln 't' and 'f'
					// don't bother checking for always showing defaults
					// don't bother for fake props.
					BOOL fGenerate = fIgnore || !m_popthdlr->IsDefaultBoolProp(idArgProp, bVal);
					if (!fGenerate)
						return FALSE;

					// do we want to generate a choice?
					if (patArgValue == choice)
					{
						fAbort = !GenerateChoice(strOptionBit, bVal ? 2 : 1, pchArgValue, pidArg);
					}
					// do we have a match in the booln value?
					else
					{
						ASSERT(patArgValue == booln);
						fAbort = (bVal != (*pchArgValue == _T('T') || *pchArgValue == _T('t')));
					}

					if (fAbort)	return FALSE;	// no generate, failed to generate booln value
					break;
				}

				case ::string:
				{
					// anti-options can only be used for string lists
					if (!m_fMultipleGen && (m_optbeh & OBAnti))
						return FALSE;

					CString	strVal;

					// generate this string value (quoted)
					// if we are a multiple option attempt to pluck out the arg. in the
					// comma sep. list we want
					if (m_fMultipleGen)
					{
						CPropBag * pBag = m_pSlob->GetPropBag();
						BOOL fUseCache =
							(
							 // same property bag?
							 pBag == m_pBagCache &&

							 // same multiple prop. for this property bag?
							 idArgProp == m_idArgPropCache &&

							 // subsequent iteration?
							 m_iGenIteration >= m_iGenIterationCache &&

							 // same option behaviour?
							 m_optbeh == m_optbehCache
						    );

						if (!fUseCache)
						{
							CString str;

							// get the property
							// can't generate invalid props!

							// set to retrieve subtract if anti-option
							BOOL fPropAntiOld = ((CProjItem *)m_pSlob)->EnableAntiProps(m_optbeh & OBAnti);

							// get the property
							BOOL fValid = m_pSlob->GetStrProp(idArgProp, str) == valid;
							
							(void) ((CProjItem *)m_pSlob)->EnableAntiProps(fPropAntiOld);

							if (!fValid) return FALSE;

							m_optlstCache.SetString(str);
							m_pBagCache = pBag;
							m_posCache = m_optlstCache.GetHeadPosition();
							m_iGenIterationCache = 0;
							m_idArgPropCache = idArgProp;
							m_optbehCache = m_optbeh;
						}

						if (m_posCache == (POSITION)NULL)
						{
							m_fStopMultipleGen = TRUE;
							return (m_iGenIteration != 0);	// can't generate this iteration
						}

						do					
						{
							strVal = m_optlstCache.GetNext(m_posCache);
						}
						while (m_posCache != (POSITION)NULL && m_iGenIteration != m_iGenIterationCache++);

  						if (m_posCache == (POSITION)NULL && m_iGenIteration != m_iGenIterationCache)
						{
							m_fStopMultipleGen = TRUE;
							return TRUE;	// can't generate this iteration
						}
																											
 						m_fGenUsedProps = TRUE;	// used a property!
						fAlwaysShowDefault = FALSE;	// no defaults for option lists
					}
					else
					{
						// want to show the option defaults when generating an option string?
						BOOL fPropDefaultOld = ((CProjItem *)m_pSlob)->UseOptionDefaults(m_fUseDefault || fAlwaysShowDefault);

						// get the property
						// can't generate invalid props!
						BOOL fValid = m_pSlob->GetStrProp(idArgProp, strVal) == valid;

						(void) ((CProjItem *)m_pSlob)->UseOptionDefaults(fPropDefaultOld);

						if (!fValid) return FALSE;
					}

					// gen. used a prop.!
					m_fGenUsedProps = TRUE;

					// is this a default or, in the case of strings, empty?
					if (strVal.IsEmpty() && !(m_optbeh & OBNeedEmptyString))
						return FALSE;	// don't generate empty strings

					AdjustBuffer(strOptionBit);

					// don't bother doing this for multiple string lists!
					if (!m_fMultipleGen)
					{
						BOOL fIgnore;

						if (fFake)
							fIgnore = fAlwaysShowDefault || (m_optbeh & OBShowFake);
						else
							fIgnore = fAlwaysShowDefault || (m_optbeh & OBShowDefault);

						if (!fIgnore && m_popthdlr->IsDefaultStringProp(idArgProp, strVal))
							return FALSE;	// don't generate the default

#if 0
						// we want to make sure that we don't get 
						// 'foobar\"', this needs to be 'foobar\\"'
						int cch = strVal.GetLength();
						if (cch != 0)	// don't bother with empty ""
						{
							// check for a single '\' at the end of the string
							const TCHAR * pc = (const TCHAR *)strVal + cch;
							pc = _tcsdec((const TCHAR *)strVal, (TCHAR *)pc);
							if (pc && *pc == _T('\\'))
							{
								pc = _tcsdec((const TCHAR *)strVal, (TCHAR *)pc);
								if (!pc || *pc != _T('\\'))
									strVal += _T('\\');	// need an extra
							}
						}
#endif
					}
 
					// only do the check for $(INTDIR) and $(OUTDIR) on 
					// string props with subtype 'pathST' or 'dirST'
					SUBTYPE st = m_popthdlr->GetDefOptionSubType(idArgProp);

					BOOL fQuote = TRUE;

					// path or dir subtype ('pathST')
					if (st == pathST || st == dirST)
					{
						// possibility of a macro of form '$(MACRO)' in this?
						if ((m_optbeh & OBShowMacro) != 0)
						{
							// got the '$(MACRO)'?
							if (strVal[0] == _T('$') && strVal[1] == _T('('))
							{
								// we have a macro
								// it doesn't matter if we quote this, 
								// nmake will expand the macro
							}	
							else
							// check for a path that we can place a macro in..
							{
								// property of output directory we may want to convert to a macro
								UINT idProp;

								// uses output directory?
								if ((idProp = m_popthdlr->GetFakePathDirProp(idArgProp)) != (UINT)-1)
								{
									CString strMacro, strValue;
									if (((CProjItem *)m_pSlob)->GetOutDirString(strMacro, idProp, &strValue, TRUE, TRUE))
									{
										// get the output and project's directories
										CPath pathOutput;
										CDir dirOutput, dirProj = ((CProjItem *)m_pSlob)->GetProject()->GetWorkspaceDir();

										// convert our targ value into a file
										strValue += _TEXT("\\x");

										if (pathOutput.CreateFromDirAndFilename((const CDir &)dirProj, strValue) &&
											dirOutput.CreateFromPath(pathOutput))
										{
											CPath path;
											CString strPath = strVal;	// temp. path (used as path storage initially)

											TCHAR * pch = (TCHAR *)(const TCHAR *)strPath;
											pch = _tcsdec(pch, pch + _tcslen(pch));

											BOOL fDir;
											if (fDir = (*pch == _T('\\') || *pch == _T('/')))
												strPath += _TEXT("x");	// strip 'bogus' filename later

											// need to append a '/x'?
											if (!fDir && st == dirST)
												strPath += _TEXT("/x");	// strip 'bogus filename later

												// use the target's working directory
											if (path.CreateFromDirAndFilename((const CDir &)dirProj, strPath) &&
												// try to relativize it
												path.GetRelativeName(dirOutput, strPath, FALSE, TRUE) &&
												// == _T('.') if was made *subdir* relative to output directory
												strPath[0] == _T('.'))
											{
												// skip '.'s and replace '\' with '/'
												int nDots = 1;	// at least one
												if (strPath[1] == _T('.'))
													nDots++;

												// strip out the 'bogus' filename?
												if (fDir)
												{
													int iNewLen = strPath.GetLength() - 1;
													TCHAR * pch = strPath.GetBufferSetLength(iNewLen);
													strPath.ReleaseBuffer(iNewLen);
												}

												// construct our new value
												strVal = strMacro;
												strVal += _T('\\');

												// automatically quoted (see below, fQuote == TRUE)
												// (skip over the '.'s and '\'
												strVal += ((const TCHAR *)strPath + nDots + 1);
											}
										}
									}
								}
							}
						}
					}
					else
					{
						// not a directory or a filename
						// don't quote an assignment, ie. we want to keep
						// a option of the form /D WIN32=1 or /D SZTITLE="Foo Bar"
						fQuote = strVal.Find(_T('=')) == -1;
					}

					// don't bother doing this for multiple string lists!
					if (!m_fMultipleGen)
					{
						// we want to make sure that we don't get 
						// 'foobar\"', this needs to be 'foobar\\"'
						int cch = strVal.GetLength();
						if (cch != 0)	// don't bother with empty ""
						{
							// check for a single '\' at the end of the string
							const TCHAR * pc = (const TCHAR *)strVal + cch;
							pc = _tcsdec((const TCHAR *)strVal, (TCHAR *)pc);
							if (pc && *pc == _T('\\'))
							{
								pc = _tcsdec((const TCHAR *)strVal, (TCHAR *)pc);
								if (!pc || *pc != _T('\\'))
									strVal += _T('\\');	// need an extra
							}
						}
					}
 
					// prepend the '"' (if needed) before the string
					// ie. "foo"
					if (fQuote)	strOptionBit += _T('"'); 
					strOptionBit += strVal;
					if (fQuote)
					{
						BOOL bAddBackSlash = FALSE;
						LPCTSTR pchStart = strOptionBit;
						int length = _tcslen(pchStart);

						//
						// Already added quote.
						//
						ASSERT(0!=length);

						//
						// If last char is backslash and 2nd to last is not backslash,
						// append an extra backslash before quote.
						//
						// review(tomse): We are in trouble if user 
						// adds more multiple trailing backslashes.
						TCHAR* pchLast = _tcsdec(pchStart,pchStart+length);
						if (_T('\\')==*pchLast)
						{
							ASSERT(1<length);
							bAddBackSlash = _T('\\')!=*_tcsdec(pchStart,pchLast);
						}

						if (bAddBackSlash)
							strOptionBit += _T('\\');
						strOptionBit += _T('"');
					}
					break;
				}

				case ::longint:	// not yet needed!
				case ::number:
					break;

				default:
					ASSERT(FALSE);
					break;
			}
		}
		// do we have an open bracket (optional part?)
		else if (chExpr == _T('['))
		{
			char * pchPart;
			UINT cchPart;

			// extract the optional part
			ExtractPart(pchExpr, cchExpr, pchPart, cchPart);

			UINT	cch = strOptionBit.GetLength();

			// ignore if any option part used props
			// and it failed
			BOOL fOldGenUsedProps = m_fGenUsedProps;
			if (!GenerateExpression(strOptionBit, pchPart, cchPart, pidArg))
			{
				// back-out our we had props flag
				m_fGenUsedProps = fOldGenUsedProps;
				// back-out the optional part strOptionBit changes
				strOptionBit.ReleaseBuffer(cch);
				// continue, this was optional!
			}
		}
		// else just put this into the option bit
		else
		{		
			AdjustBuffer(strOptionBit);
			strOptionBit += chExpr;

			cch = _tclen(pchExpr);
			pchExpr += cch; cchExpr -= cch;
		}

		// continue trying to generate this option
	}

	return TRUE;	// successful generation
}

BOOL COptionTable::MatchOption(COptStr * poptstr)
{
	BOOL	fMatched;

	// what type of option it this? single or multiple?
 	m_fMultipleGen = (poptstr->type == multiple);

	// can we match this expression?
	fMatched = MatchExpression(poptstr->szOption, _tcslen(poptstr->szOption), poptstr->rgidArg);

	// must terminate in a whitespace or EOS
	unsigned char	chTerm = NewGetAt(strSrc, ichSrc + cchAdvSrc);

	fMatched = fMatched && (chTerm == _T('\0') || _istspace(chTerm));
	
	if (fMatched)	
		// yes, so set up these properties in our bag
		SetPropValListInPropBag(poptstr->rgidArg);

	// clear our property-value list (may be empty)
	ClearPropValList();

	return fMatched;
}

BOOL COptionTable::GenerateOption
(
	CString & strOption,
	COptStr * poptstr,
	CString & strPrefix,
	BOOL fAddTrailSpace
)
{
	CString	strOptionBit;
	BOOL	fGenerated, fContinue;

	// this will be our first generation iteraton
	m_iGenIteration = 0;

	// what type of option it this? single or multiple?
	m_fMultipleGen = (poptstr->type == multiple);
	m_fStopMultipleGen = FALSE;	// not stop multiple (for singles never changed)

	// do the generation until we need to stop
	do
	{
		// make sure we clear our option bit before generating into it!
		strOptionBit = "";

		// init. whether this gen had props used in the generation
		m_fGenUsedProps = FALSE;
		fGenerated = GenerateExpression(strOptionBit,
										poptstr->szOption,
										_tcslen(poptstr->szOption),
										poptstr->rgidArg);

		// are we in anti-options and have we generated one?
		// if so try to generate the 'regular' equivalent
		// don't do this for multiple options
		if ((m_optbeh & OBAnti) && !m_fMultipleGen && 
			fGenerated && !strOptionBit.IsEmpty())
		{
			CString strRegOptionBit;
			BOOL fAntiHasRegularRep = FALSE;

			// temp. turn off (ie. regular options)
			m_optbeh &= ~OBAnti;

			// generate the 'regular' equivalent
			fAntiHasRegularRep = GenerateExpression(strRegOptionBit,
												    poptstr->szOption,
								   				    _tcslen(poptstr->szOption),
								   				    poptstr->rgidArg) &&
								 !strRegOptionBit.IsEmpty();

			// turn back on again (ie. anti options)
			m_optbeh |= OBAnti;

			// if the regular representation exists ignore this
			if (fAntiHasRegularRep)	return TRUE;	// no error, just no generate
		}


		// did we generate this?
		if (fContinue = fGenerated && !m_fStopMultipleGen && !strOptionBit.IsEmpty())
		{
			AdjustBuffer(strOption);

			// append the requested prefix, eg. '/' or ', and /'
			strOption += strPrefix;

			// append this option bit to our current option
			strOption += strOptionBit;

			// place a trailing space if requested
			if (fAddTrailSpace)
				strOption += _T(' ');

			// next invocation
			m_iGenIteration++;
 		}
	}
	while (m_fMultipleGen && fContinue);

	return fGenerated;	// success or failure?
}

void COptionTable::ClearPropVal(UINT *poptarg)
{
	CPropBag * 	ppropbag = m_pSlob->GetPropBag();

	for (WORD cArgs = 0; cArgs < MAX_OPT_ARGS; cArgs++, poptarg++)
	{
		// do we need to remove this prop?
		if (*poptarg != (UINT)-1)
		{
			if (m_popthdlr->IsListStrProp(*poptarg))
			{
				BOOL fExists = FALSE;

				// delete add/subtract prop from the current property bag (if they exist)
			 	if (ppropbag->FindProp(ListStr_AddPart(*poptarg)) != (CProp *)NULL)
				{
					ppropbag->RemoveProp(ListStr_AddPart(*poptarg));
					ppropbag->RemoveProp(ListStr_SubPart(*poptarg));
					// inform of this prop change (update option view?)
					m_pSlob->InformDependants(*poptarg);
				}
			}
		 	else
		 	{
				// delete this from the current property bag (if it exists)
				ppropbag->RemovePropAndInform(*poptarg, m_pSlob);
		 	}
		}
	}
}

void COptionTable::ClearPropValList(BOOL fOnlyThoseChanged)
{
	for (WORD cArgs = 0 ; cArgs < MAX_OPT_ARGS ; cArgs++)
		if (m_rgProps[cArgs].idArg != (UINT)-1 && 
		    (!fOnlyThoseChanged || m_rgProps[cArgs].fChanged)
		   )
		{
			if (m_rgProps[cArgs].pProp)
				CProp::DeleteCProp(m_rgProps[cArgs].pProp);
			m_rgProps[cArgs].idArg = (UINT)-1;
		}
}

void COptionTable::SetPropValListInPropBag(UINT * poptarg)
{
	CPropBag * 	ppropbag = m_pSlob->GetPropBag();
	CProp * ppropExisting; 

	for (WORD cArgs = 0; cArgs < MAX_OPT_ARGS ; cArgs++, poptarg++)
	{
		UINT	idArg = m_rgProps[cArgs].idArg;
		BOOL	fExists = (ppropExisting = ppropbag->FindProp(*poptarg)) != (CProp *)NULL;

		// has this arg. been set?
		if (idArg == (UINT)-1)
		{
			UINT	idOptArg = *poptarg;

			// should we clear the arg? we need to do this for args that exist for the option
			// but which have not been set
			if (idOptArg != (UINT)-1)
			{
				// yes, clear it, but for multiple options just append an empty prop. value
				if (m_fMultipleGen)
				{
					// get the prop value
					CString	strVal;

					// set to retrieve subtract if anti-option
					BOOL fPropAntiOld = ((CProjItem *)m_pSlob)->EnableAntiProps(m_optbeh & OBAnti);

					// get the existing comma sep. list of args if *we* have one
					// ie. we only want to do this for *our* propbag (not the parents)
					if (ppropbag->FindProp(ListStr_AddPart(idOptArg)) ||
						ppropbag->FindProp(ListStr_SubPart(idOptArg)))
					{
						(void) m_pSlob->GetStrProp(idOptArg, strVal);

						// append a sep
						strVal += _T(',');
					}

					// append a comma sep
					m_pSlob->SetStrProp(idOptArg, strVal); 

					(void) ((CProjItem *)m_pSlob)->EnableAntiProps(fPropAntiOld);
				}
				// remove this prop if we have one
				else if (fExists)
				{
					// delete this from the current property bag (it exists!)
					ppropbag->RemovePropAndInform(idOptArg, m_pSlob);
				}
			}

			continue;
		}

		CProp *	pprop = m_rgProps[cArgs].pProp;

		// if we have NULL here then ignore
		// we want to use the previous existing value (if one exists)
		if (pprop == (CProp *)NULL)
			continue;

		// dirty this prop we are about to set

		// for string list props we need to delete the components
		// *not* the pseudo prop
		if (m_fMultipleGen && m_popthdlr->IsListStrProp(idArg))
		{
			m_popthdlr->DirtyArg(ListStr_AddPart(idArg) - m_nPropMin);
			m_popthdlr->DirtyArg(ListStr_SubPart(idArg) - m_nPropMin);
		}
		else
		{
			m_popthdlr->DirtyArg(idArg - m_nPropMin);
		}

		// what is the property type?
		// don't set property values if current property has this value!
		switch (pprop->m_nType /* property type */)
		{	
			case ::integer:
				ASSERT(!m_fMultipleGen);	// only for single options
				if (!fExists ||
					(((CIntProp *)pprop)->m_nVal != ((CIntProp *)ppropExisting)->m_nVal)
				   )
				{
					m_pSlob->SetIntProp(idArg, ((CIntProp *)pprop)->m_nVal); 
				}
				break;

			case ::booln:
				ASSERT(!m_fMultipleGen);	// only for single options
				if (!fExists ||
					(((CBoolProp *)pprop)->m_bVal != ((CBoolProp *)ppropExisting)->m_bVal)
				   )
				{
					m_pSlob->SetIntProp(idArg, ((CBoolProp *)pprop)->m_bVal); 
				}
				break;

			case ::string:
			{
				// if we are a multiple option, append using comma seps.
				if (m_fMultipleGen)
				{	
					// the option sep. list
					CString	strVal;

					// set to retrieve subtract if anti-option
					BOOL fPropAntiOld = ((CProjItem *)m_pSlob)->EnableAntiProps(m_optbeh & OBAnti);

					// get the existing comma sep. list of args if *we* have one
					// and append to that
					if (m_pSlob->GetStrProp(idArg, strVal) == valid)
					{
						if (!strVal.IsEmpty())
							strVal += _T(',');
						strVal += ((CStringProp *)pprop)->m_strVal;

						m_pSlob->SetStrProp(idArg, strVal); 
					}

					(void) ((CProjItem *)m_pSlob)->EnableAntiProps(fPropAntiOld);
				}
				else
				{
					// anti-option only for multiple string lists
					if (m_optbeh & OBAnti)
						break;

					if (!fExists ||
						(((CStringProp *)pprop)->m_strVal != ((CStringProp *)ppropExisting)->m_strVal)
					   )
					{
						m_pSlob->SetStrProp(idArg, ((CStringProp *)pprop)->m_strVal); 
					}
				}

				break;
			}

			case ::longint:	// not yet needed!
			case ::number:
				break;

			default:
				ASSERT(FALSE);
				break;
		} 
	}
}

int COptionTable::GenerateHashValue (CString & str)
{
	UINT uHashValue = 0;

	for (int i = 0; i < str.GetLength(); i++)
		uHashValue += (UINT) i ^ (UINT)str[i];
	return (int) uHashValue;
}	 
		
