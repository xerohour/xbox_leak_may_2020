//
//	CTargetItem
//
//	This is a projitem that represents a target.
//
//	[colint]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop

#include "bldrfile.h"	// our CBuildFile
#include "targitem.h"
#include "project.h"
#include "projdep.h"
#include "VCProjConvert.h"

IMPLEMENT_SERIAL (CTargetItem, CProjContainer, SLOB_SCHEMA_VERSION)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// A target item has only one property at the moment,
// its name, and this may be deleted in the future as
// its name should really be the name of the target
// file it generates when built.

#define theClass CTargetItem
BEGIN_SLOBPROP_MAP(CTargetItem, CProjContainer)
END_SLOBPROP_MAP()
#undef theClass

CTargetItem::CTargetItem()
{
	// our current target is us!
	m_pOwnerTarget = this;

	m_strTargDir = "";
}

CTargetItem::~CTargetItem()
{
	// delete our contained file-items
	DeleteContents();
}

///////////////////////////////////////////////////////////////////////////////
// Verb Commands

BOOL CTargetItem::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	VSASSERT(fPrjChanged == FALSE, "PreMoveItem must let fPrjChanged be TRUE"); // not supported
	if (pContainer)
	{
		VSASSERT(pContainer->IsKindOf(RUNTIME_CLASS(CProject)), "Container must be a project");
	}

	// do the base-class thing
	if (!CProjContainer::PreMoveItem(pContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
GPT CTargetItem::GetIntProp(UINT idProp, int& val)
{
	return CProjContainer::GetIntProp(idProp, val);
}

GPT CTargetItem::GetStrProp (UINT idProp, CString &str)
{
	if (idProp == P_ProjConfiguration || idProp == P_ProjItemFullPath || idProp == P_SccStatus)
	{
		// Thunk this property up to the project
		CProject* pProject = GetProject();
		
		return pProject->GetStrProp(idProp, str);
	}
	else
 		return CProjContainer::GetStrProp ((idProp == P_ProjItemName || idProp == P_Title) ? P_TargetName : idProp, str);
}

BOOL CTargetItem::SetStrProp (UINT idProp,  const CString &str)
{
	BOOL bRetval;
	CString strChanged;

	// P_ProjItemName is synonym for P_TargetName
	if (idProp == P_ProjItemName)
		idProp = P_TargetName;

	if (idProp == P_TargetName) 
	{
		// error if this name is blank
		if (str.IsEmpty())
		{
			InformDependants(idProp);
			return FALSE;
		}
	}
 
 	bRetval = CProjContainer::SetStrProp (idProp, str);

	return bRetval;
}

// Target name prefix written to the makefile
const TCHAR *CTargetItem::pcNamePrefix	= _TEXT("Name ");

// Reads target name comments and sets up the config recs accordingly. Also
// registers the target with the target manager.
BOOL CTargetItem::IsNameComment(CObject *&pObject)
{
	// If this is not a makefile comment then give up now!
	if (!pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
		return FALSE;

	CMakComment *pMc = (CMakComment*) pObject;

	// Skip over any initial white space
	TCHAR *pc, *pcEnd;
	pc = pMc->m_strText.GetBuffer(1);
	SkipWhite(pc);

	// We should have the target name prefix at the front of
	// the comment
	if (_tcsnicmp(pcNamePrefix, pc, 4) != 0)
		return FALSE;

	// Ok, we found the prefix so skip over it.
	pc += 4;
	SkipWhite(pc);

	// Get the quoted target name
	if (!GetQuotedString(pc, pcEnd))
		return FALSE;

	*pcEnd = _T('\0');

	CString strName = pc;
	BOOL bTargetRegistered = FALSE;

	// Now we have the target name get the corresponding config rec
	// and set it to be valid for this target item.
	CProject * pProject = GetProject();
	if (pProject->m_bConvertedDS4x)
	{
		// Register this target with the project target manager.
		CProject * pNewProj = NULL;
		CString strProject;
		CProject::InitProjectEnum();
		while ((pNewProj = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL)
		{
			if (pNewProj->RegisterTarget(strName, this))  // see if matching config
			{
				if (pNewProj != pProject)
				{
					// fix up proper containment
					CSlob * pContainer = pNewProj;
					(void)PreMoveItem(pContainer, FALSE, TRUE, FALSE);
					(void)MoveItem(pContainer, (CSlob *)NULL, FALSE, TRUE, FALSE);
					pProject = pNewProj;
				}
				g_pActiveProject = pProject; // later code will depend on this
				bTargetRegistered = TRUE;
				break;
			}
		}
		VSASSERT(bTargetRegistered, "Target must be registered");
	}

	// Force the target into it's own config.
 	pProject->SetActiveConfig(strName);
	CConfigurationRecord * pcrTarg = GetActiveConfig();

	if (GetTargetName().IsEmpty())
	{
		int index = strName.Find(" - ");
		VSASSERT(index > 0, "Malformed target name");
		CString strTargetName = strName.Left(index);
		SetTargetName(strTargetName);
		if (GetProject()->GetTargetName().IsEmpty())
			GetProject()->SetTargetName(strTargetName);
		VSASSERT(strTargetName == GetProject()->GetTargetName(), "Malformed target name");
	}

	// ensure that the target has a matching config. and it is valid
	pcrTarg->SetValid(TRUE);

	// Register this target with the project target manager.
	if (!bTargetRegistered)
		pProject->RegisterTarget(strName, this);

	CTargetItem *pTarget = pProject->GetTarget(strName);
	VSASSERT(pTarget == this, "Our project doesn't acknowledge us!");
	CString strDir;
	if (pProject->GetStrProp(P_Proj_TargDir, strDir)==valid)
	{
		CPath path;
		path.CreateFromDirAndFilename(pProject->GetWorkspaceDir(), strDir);
		strDir = path.GetFullPath();
		pTarget->SetTargDir(strDir);
	}

	delete pObject; pObject = NULL;
	return TRUE;
}

BOOL CTargetItem::ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir)
{
//	Read in this item and any childred from a makfile reader.  The make
//  comment is the first line of our stuff:
//
	CObject *pObject = NULL;
	CString str;

	TRY
    {
		pObject = mr.GetNextElement ();
		if (!pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)))
			AfxThrowFileException (CFileException::generic);

		m_cp = EndOfConfigs;
 		delete pObject; pObject = NULL;

		for (pObject = mr.GetNextElement (); pObject;pObject = mr.GetNextElement ())
	   	{
			if (IsNameComment(pObject))
				continue;
			else if (IsPropComment(pObject))
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
				VSASSERT(m_cp==EndOfConfigs, "Malformed project file");
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

		CProjContainer * pContainer = this;
		CProject * pProject = GetProject();
		if (pProject->m_bConvertedDS4x || (pProject->m_bProjConverted && (!pProject->m_bConvertedVC20) 
			&& (!pProject->m_bConvertedDS5x && !pProject->m_bConvertedDS6 && !pProject->m_bConvertedDS61)))
		{
 			CString strName, strFilter;
			strName = CVCProjConvert::s_strSourceFilesString;

			CProject *pProj = g_pActiveProject;
			CProjType * pProjType = pProj->GetProjType();

			BOOL bOK = strFilter.LoadString(IDS_SOURCE_FILES_FILTER);
			VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
			pContainer = AddNewGroup(strName, strFilter);
		}
		// Read in children:
		if (!pContainer->ReadInChildren(mr, BaseDir))
 			AfxThrowFileException (CFileException::generic);
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

BOOL CTargetItem::CanContain(CSlob* pSlob)
{
	if (pSlob->IsKindOf(RUNTIME_CLASS(CFileItem)) ||
	    pSlob->IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem)) ||
	    pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)) ||
		pSlob->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		return TRUE;

	return FALSE;
}

void CTargetItem::FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem)
{
	// Exclude targets from the build
	if (fo & flt_RespectTargetExclude)
	{
		CConfigurationRecord * pcr = GetActiveConfig();
		if (pcr && pcr->IsValid())
		{
			fAddItem = FALSE;
			fAddContent = TRUE;
		}
	}
	else if (fo & flt_TargetInclude)
	{
		CConfigurationRecord * pcr = GetActiveConfig();
		if (pcr && pcr->IsValid())
		{
			fAddItem = TRUE;
			fAddContent = TRUE;
		}
	}
	else
	{
		fAddItem = FALSE;
		fAddContent = TRUE;
	}
}
