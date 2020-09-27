//
//	PROJCNTR.CPP
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop

#include "targitem.h"
#include "project.h"
#include "makread.h"
#include "VCProjConvert.h"

IMPLEMENT_SERIAL (CProjContainer, CProjItem, SLOB_SCHEMA_VERSION)
IMPLEMENT_SERIAL (CTimeCustomBuildItem, CFileItem, SLOB_SCHEMA_VERSION)
IMPLEMENT_SERIAL (CProjGroup, CProjContainer, SLOB_SCHEMA_VERSION)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------ CProjContainer ------------------------------
//                                 
///////////////////////////////////////////////////////////////////////////////
//
//
#define theClass CProjContainer
BEGIN_SLOBPROP_MAP(CProjContainer, CProjItem)
END_SLOBPROP_MAP()
#undef theClass

CProjContainer::CProjContainer ()
{
}

CProjContainer::~CProjContainer ()
{
}

///////////////////////////////////////////////////////////////////////////////
// Verb Commands
// not terribly efficient;  Avoid except when necessary
// optimized for an already sorted list
void CProjContainer::SortContentList()
{
	INT_PTR nCount = m_objects.GetCount();
	if (nCount < 2)
		return;  // nop

	CObList OldList; OldList.AddTail(&m_objects);
	m_objects.RemoveAll();

	m_objects.AddHead(OldList.RemoveHead());  // just insert the first element
	BOOL bAdded;
	CProjItem * pItem;
	CProjItem * pCompItem;
	POSITION pos, curPos;
	while (!OldList.IsEmpty())
	{
		bAdded = FALSE;
		pItem = (CProjItem *)OldList.RemoveHead();
		pos = m_objects.GetTailPosition();
		while (pos != NULL)
		{
			curPos = pos;
			pCompItem = (CProjItem *)m_objects.GetPrev(pos);
			if (pItem->CompareSlob(pCompItem) >= 0)
			{
				m_objects.InsertAfter(curPos, pItem);
				bAdded = TRUE;
				break;
			}
		}
		if (!bAdded)
		{
			m_objects.AddHead(pItem);
		}
	}
	VSASSERT(m_objects.GetCount() == nCount, "Count mismatch!");
	OldList.RemoveAll();
};

CProjGroup *CProjContainer::AddNewGroup (const TCHAR * pszName /* = NULL */, const TCHAR * pszFilter /* = NULL */)
{
	CProjGroup *pGroup;

	if (pszName)
		pGroup = new CProjGroup(pszName);
	else
		pGroup = new CProjGroup;

	CProjContainer * pContainer = this;
	if (pContainer->IsKindOf(RUNTIME_CLASS(CProject)))
	{
		CTargetItem *pTarget;
		pTarget = GetTarget();
		pContainer = (CProjContainer *)pTarget;
	}
	pGroup->MoveInto(pContainer);

	if ((pszFilter) && (*pszFilter))
	{
		CString strFilter = pszFilter;
		pGroup->SetStrProp(P_GroupDefaultFilter, strFilter);
	}

	return pGroup;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjContainer::ReadInChildren(CMakFileReader& mr, const CDir &BaseDir)
{
//	Helper function to read in children from make file.  
	CObject *pObject = NULL;
	CProjItem *pItem = NULL, *pTemp;
	CRuntimeClass *pRTC;
	CMakComment *pMC;

	CProject * pProject = GetProject();

	TRY
    {
		do
		{
			pObject = mr.GetNextElement ();
			VSASSERT(pObject, "NULL object in element list!");

			if (!pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)))
			{
				if ((pObject->IsKindOf(RUNTIME_CLASS(CMakDirective))) &&
					(((CMakDirective *)pObject)->m_dtyp == CMakDirective::DTYP_ENDIF))
						continue;	// orphan ENDIF might be okay

				AfxThrowFileException (CFileException::generic);
			}
			pMC = (CMakComment *) pObject;

			// Done if this is the end marker:
			if (IsEndToken (pObject)) break;

			// Figure out what kind of item this is supposed to be
			// and create it:
			pRTC = GetRTCFromNamePrefix(((CMakComment *) pObject)->m_strText);

			if (pRTC == NULL)
			{  
				delete (pObject);
				pObject = NULL;
				continue;
			}

			pItem = (CProjItem *) pRTC->CreateObject();

			CSlob * pContainer = this;

			// for v2 projects, put group into the target
			if ((pProject->m_bConvertedVC20) && (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup))))
			{
				VSASSERT(pProject==this, "Not in the correct project?!?");
				pContainer = pProject->GetActiveTarget();
				VSASSERT(pContainer, "No active target!");
				CString strFilter = "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90";
				pItem->SetStrProp(P_GroupDefaultFilter, strFilter);
			}

			// make sure this is something that belongs in us
			if (!CanContain(pItem))
				AfxThrowFileException (CFileException::generic);

			pTemp = pItem; pItem = NULL;

			// do a part of the move, ie. just fix up the containment structure
			(void)pTemp->PreMoveItem(pContainer, FALSE, TRUE, FALSE);
			(void)pTemp->MoveItem(pContainer, (CSlob *)NULL, FALSE, TRUE, FALSE);

			mr.UngetElement (pObject);
			pObject = NULL;

			// preserve the current config. while we read in each child
			// to ensure that the child doesn't have the side-effect of 
			// changing this
			CConfigurationRecord * pcrProject = pProject->GetActiveConfig();

			// read in the item data
			// (item will do the post-move)
			if (!pTemp->ReadFromMakeFile (mr, BaseDir))
				AfxThrowFileException (CFileException::generic);

			// preserve the current config.

			(void) pProject->SetActiveConfig(pcrProject);

		} while (TRUE);
 	}
	CATCH (CException, e)
	{
		if (pObject) delete (pObject);
		if (pItem) 	 delete (pItem);
		return FALSE;	// failure
	}
	END_CATCH

	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
//
// -------------------------------- CProjGroup --------------------------------
//                                 
///////////////////////////////////////////////////////////////////////////////
//
//
#define theClass CProjGroup
BEGIN_SLOBPROP_MAP(CProjGroup, CProjContainer)
	STR_PROP(GroupName)
END_SLOBPROP_MAP()
#undef theClass

CProjGroup::CProjGroup ()
{
	m_strGroupName = CVCProjConvert::s_strSourceFilesString;
}

CProjGroup::CProjGroup (const TCHAR * pszName)
{
	m_strGroupName = pszName;
}

void CProjGroup::GetMoniker (CString& cs)
{
	VSASSERT (!m_strGroupName.IsEmpty(), "Project group name not initialized properly!");
	cs = m_strGroupName;
}

///////////////////////////////////////////////////////////////////////////////
// Verb Commands
///////////////////////////////////////////////////////////////////////////////
BOOL CProjGroup::ReadInChildren(CMakFileReader& mr, const CDir &BaseDir)
{
//	Helper function to read in children from make file.  
	CObject *pObject = NULL;
	CProjItem *pItem = NULL, *pTemp;
	CRuntimeClass *pRTC;
	CMakComment *pMC;

	// Get the target to which this belongs, which is the first
	// target
	CProject * pProject = GetProject();
	CProjItem * pContainer = (CProjItem *)GetContainer();
	CTargetItem* pTarget = (CTargetItem *)GetTarget();
	CSlob * pSlob = (CSlob *)this;
;			
#if 1	// UNDONE: remove once we keep V2 groups
	if (pContainer->IsKindOf(RUNTIME_CLASS(CProject)))	// VC 2.0 makefile
	{
		CString strTargetName;
		((CProject *)pContainer)->InitTargetEnum();
		((CProject *)pContainer)->NextTargetEnum(strTargetName, pTarget);
		pSlob = (CSlob *)pTarget;
	}
#endif

	CSlob * pSavedSlob = pSlob;
	TRY
    {
		do
		{
			pSlob = pSavedSlob;
			pObject = mr.GetNextElement ();
			VSASSERT (pObject, "NULL object in elements list!");

			if (!pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)))
					AfxThrowFileException (CFileException::generic);
			pMC = (CMakComment *) pObject;

			// done if this is the end marker
			if (IsEndToken (pObject)) break;

			// figure out what kind of item this is supposed to be and create it
			pRTC = GetRTCFromNamePrefix(((CMakComment *) pObject)->m_strText);

			if (pRTC == NULL)
			{  
				delete (pObject);
				pObject = NULL;
				continue;
			}

			pItem = (CProjItem *) pRTC->CreateObject();

			// make sure this is something that belongs in us
			if (!CanContain(pItem))
			{
				// REVIEW: possible for VC20 projects also?
				if ((pProject->m_bConvertedDS4x) && (pTarget->CanContain(pItem)))
				{
					pSlob = pTarget; // meant to add this to the target instead!
				}
				else
				{
						AfxThrowFileException (CFileException::generic);
				}
			}
			pTemp = pItem; pItem = NULL;


			// do a part of the move, ie. just fix up the containment structure
			// move into the target rather than the group, we'll delete this group
			// after we've finished reading the VC++ 2.0 makefile
			(void)pTemp->PreMoveItem(pSlob, FALSE, TRUE, FALSE);
			(void)pTemp->MoveItem(pSlob, (CSlob *)NULL, FALSE, TRUE, FALSE);

			mr.UngetElement (pObject);
			pObject = NULL;

			// preserve the current config. while we read in each child
			// to ensure that the child doesn't have the side-effect of 
			// changing this
			CConfigurationRecord * pcrProject = GetProject()->GetActiveConfig();

			// read in the item data
			if (!pTemp->ReadFromMakeFile (mr, BaseDir))
				AfxThrowFileException (CFileException::generic);

			// preserve the current config.
			(void) GetProject()->SetActiveConfig(pcrProject);

		} while (TRUE);
	}
	CATCH (CException, e)
	{
		if (pObject) delete (pObject);
		if (pItem) 	 delete (pItem);
		return FALSE;	// failure
	}
	END_CATCH

	return TRUE;	// success
}

GPT CProjGroup::GetStrProp ( UINT idProp, CString &str )
{
	// special per-item props
	if (idProp == P_GroupDefaultFilter || idProp == P_FakeGroupFilter)
	{
		str = m_strDefaultExtensions;
		return valid;
	}
	// P_ProjItemName is a fake prop that just maps to P_GroupName.
	return CProjContainer::GetStrProp ((idProp == P_ProjItemName || idProp == P_FakeProjItemName) ? P_GroupName : idProp, str);
}

BOOL CProjGroup::SetStrProp ( UINT idProp,  const CString &str )
{
	BOOL bRetval;
	BOOL bNameChanged = FALSE;
	CString strChanged;
	if (idProp == P_ProjItemName || idProp == P_GroupName) 
	{
		if (str.IsEmpty())
		{
			InformDependants(P_ProjItemName);
			return FALSE;
		}
	}

	// special per-item props
	if (idProp == P_GroupDefaultFilter)
	{
		m_strDefaultExtensions = str;
		return TRUE;
	}

	// P_ProjItemName is a fake prop that just maps to P_GroupName.
	bRetval = CProjContainer::SetStrProp ((idProp == P_ProjItemName) ? P_GroupName : idProp, (bNameChanged) ? strChanged : str);

	return bRetval;
}

BOOL CProjGroup::ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir)
{
//	Read in this item and any childred from a makfile reader.  The make
//  comment is the first line of our stuff:
//
	TCHAR *pBase;
	CObject *pObject = NULL;
	CString str;

	TRY
    {
		pObject = mr.GetNextElement ();
		if (!pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)))
				AfxThrowFileException (CFileException::generic);

		// Get our name from the comment:
		pBase =	((CMakComment*) pObject)->m_strText.GetBuffer (1);	

		// Skip over the "Begin":
		SkipWhite    (pBase);
		SkipNonWhite (pBase);
		SkipWhite    (pBase);
		// Skip over the Group declaration
		SkipNonWhite (pBase);
		SkipWhite    (pBase);
		

		// Make sure its quoted, and then shave the quotes off:
		if (*pBase++ != _T('"') ||
			((CMakComment*) pObject)->
				m_strText[((CMakComment*) pObject)->m_strText.GetLength()-1] != _T('"'))
		    AfxThrowFileException (CFileException::generic);

		m_cp = EndOfConfigs;
		str = pBase;
		str.ReleaseBuffer ( str.GetLength () - 1 );
		SetStrProp(P_ProjItemName, str);
		delete (pObject); pObject = NULL; 

		for (pObject = mr.GetNextElement (); pObject;pObject = mr.GetNextElement ())
	   	{
			if (IsPropComment(pObject))
				continue;
			else if (IsAddSubtractComment(pObject))
				continue;
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakDirective))) 
			{
				ReadConfigurationIfDef((CMakDirective *)pObject);
			}
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakMacro)))
			{
				SuckMacro((CMakMacro *)pObject, BaseDir);
			}
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakDescBlk)))
			{
				if (!SuckDescBlk((CMakDescBlk *)pObject))
					AfxThrowFileException (CFileException::generic);
			}
			// Comment marks start of children:
			else if (pObject->IsKindOf ( RUNTIME_CLASS(CMakComment))) 
			{
				VSASSERT(m_cp==EndOfConfigs, "Parsing error on reading the make file!");
				mr.UngetElement ( pObject );
				pObject = NULL;
				break;
			}
			else if (pObject->IsKindOf ( RUNTIME_CLASS(CMakEndOfFile))) 
			{
				AfxThrowFileException (CFileException::generic);
			}

	   		delete (pObject); pObject = NULL;
		}				 

		// Read in children:
		if (!ReadInChildren(mr, BaseDir))
 			AfxThrowFileException (CFileException::generic);

		// preserve the current config.
		// REVIEW(kperry) what is this doing ? Should this be a set active config of
		// the config above ? I don't understand.
		(void) GetProject()->GetActiveConfig();
	}
	CATCH ( CException, e)
	{
		if (pObject) delete (pObject);
		return FALSE;	// failure
	}
	END_CATCH

	// do the rest of the move
	(void)PostMoveItem(GetContainer(), (CSlob *)NULL, FALSE, TRUE, FALSE);

	return TRUE;	// success
}

BOOL CProjGroup::CanContain(CSlob* pSlob)
{
	if (pSlob->IsKindOf(RUNTIME_CLASS(CFileItem)))
		return TRUE;

	if (pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)))
	{
		// need to ensure we're not dropping into a descendent
		const CSlob * pContainer;
		for (pContainer = GetContainer(); pContainer != NULL; pContainer = pContainer->GetContainer())
		{
			if (pContainer==pSlob)
				return FALSE;	// can't contain our parent
			else if (!pContainer->IsKindOf(RUNTIME_CLASS(CProjGroup)))
				return TRUE;	// done if we've checked all groups
		}
		return TRUE; // okay if no container
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// -------------------------------- CTimeCustomBuildItem --------------------------------
//                                 
///////////////////////////////////////////////////////////////////////////////
//
//
#define theClass CTimeCustomBuildItem
BEGIN_SLOBPROP_MAP(CTimeCustomBuildItem, CFileItem)
END_SLOBPROP_MAP()
#undef theClass


CTimeCustomBuildItem::CTimeCustomBuildItem ()
{
	Init(NULL);
}

CTimeCustomBuildItem::CTimeCustomBuildItem(const TCHAR * pszName)
{
	Init(pszName);
}

void CTimeCustomBuildItem::Init(const TCHAR* pszName)
{
	m_strName = pszName;
	
	// our file path
	m_pFilePath = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Verb Commands

BOOL CTimeCustomBuildItem::MakeNameUnique(CString & strName)
{
	CProject *pProject;
	CObList ol;
	POSITION pos;
	CTimeCustomBuildItem *pGroup;
	BOOL	bCollision;
	int i = 1;
	CString str;

	pProject = GetProject ();
	if (!pProject) return FALSE;
	pProject->FlattenSubtree (ol, flt_Normal | flt_ExcludeDependencies);
	str = strName;
	CString fmt, key;
	fmt.LoadString (IDS_GROUP_CUSTOMIZER);
	key.LoadString (IDS_GROUP_CUSTOMIZER_KEY);
	
	do
	{
		bCollision = FALSE;

		for (pos = ol.GetHeadPosition (); pos != NULL && !bCollision; )
		{
			pGroup = (CTimeCustomBuildItem *) ol.GetNext (pos);
			if (!pGroup->IsKindOf ( RUNTIME_CLASS ( CTimeCustomBuildItem )))
				continue;

			if (pGroup != this && CompareMonikers (pGroup->m_strName, str) == 0)
				bCollision = TRUE;
		}		

		if ( bCollision )
		{
			int n;

			// If it already ends in "No. #", strip it before adding it again
			if (((n = strName.Find(LPCTSTR(key))) > 0)
				&& (n > (strName.GetLength() - (key.GetLength() + 2))))
				strName = strName.Left(n);

			TCHAR * pc = str.GetBuffer (256);

			// Make sure there is enough space for the customizer:
			if (strName.GetLength () > 255 - fmt.GetLength() -1 ) 
						strName.ReleaseBuffer (255 - fmt.GetLength() -1 );
			_snprintf ( pc, 
						255, 
						(const TCHAR *) fmt, 
						(const TCHAR *) strName,
						i );
			pc[255] = 0;							 
			str.ReleaseBuffer ();
 			i++; 
		}

	} while (bCollision);

	// Look at i to see if there ever was a collision:
	if (i > 1) 
		strName = str;

	return (i > 1);
}

GPT CTimeCustomBuildItem::GetStrProp ( UINT idProp, CString &str )
{
	// special per-item props
	if (idProp == P_ItemCustomBuildTime)
	{
		str = m_strCustomBuildTime;
		return valid;
	}

	return CFileItem::GetStrProp(idProp, str);
}


BOOL CTimeCustomBuildItem::SetStrProp(UINT idProp,  const CString &str)
{
	BOOL bNameChanged = FALSE;
	CString strChanged;
	if (idProp == P_ProjItemName || idProp == P_GroupName) 
	{
		if (str.IsEmpty())
		{
			InformDependants(P_ProjItemName);
			return FALSE;
		}
	}

	if (idProp == P_ItemCustomBuildTime)
	{
		m_strCustomBuildTime = str;
	}

	// If we're changing the group's name, make sure it's still unique:
	if (((idProp == P_ProjItemName) || (idProp == P_GroupName)) && (m_strName.Compare(str)))
	{
		strChanged = str;
		m_strName = str;
		bNameChanged = MakeNameUnique(strChanged);
	}

	return CFileItem::SetStrProp(idProp, (bNameChanged) ? strChanged : str);
}

///////////////////////////////////////////////////////////////////////////////
void CTimeCustomBuildItem::FlattenQuery(int fo, BOOL& bAddContent, BOOL& fAddItem)
{
	if (fo & flt_RespectItemExclude)
	{
		// get excluded from and convert into fAddItem by negating
		GetIntProp(P_ItemExcludedFromBuild, fAddItem);
		fAddItem = !fAddItem;
	}
	else
		fAddItem = TRUE;
}
