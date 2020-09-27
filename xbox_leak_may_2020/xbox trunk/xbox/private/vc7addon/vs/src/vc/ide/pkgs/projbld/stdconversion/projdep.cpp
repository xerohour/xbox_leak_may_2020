///////////////////////////////////////////////////////////////////////////////
//
//	CProjectDependency
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "targitem.h"
#include "project.h"
#include "bldrcvtr.h"
#include "makread.h"
#include "projdep.h"

IMPLEMENT_SERIAL (CProjectDependency, CProjItem, SLOB_SCHEMA_VERSION)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define theClass CProjectDependency
BEGIN_SLOBPROP_MAP(CProjectDependency, CProjItem)
END_SLOBPROP_MAP()
#undef theClass

CVCPtrList g_lstprojdeps;

CProjDepInfo * AddDep(CProjectDependency *pProjectDep)
{
	// Could be coming from the Undo slob. If so, it wont be in the list.
	CProjDepInfo *pDepInfo;
	CString strProject;
	CString strSub;
	pProjectDep->GetProject()->GetName(strProject);
	pProjectDep->GetStrProp(P_ProjItemName, strSub);

	VCPOSITION pos = g_lstprojdeps.GetHeadPosition();
	BOOL bFound = FALSE;
	while (pos != NULL)
	{
		pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->pProjDep == pProjectDep )
		{
			bFound = TRUE;
			break;
		}
	}
	if( bFound == FALSE )
	{
		pDepInfo = new CProjDepInfo;
		pDepInfo->pProjDep = pProjectDep;
		pDepInfo->strTarget = strSub;
		pDepInfo->strProject = strProject;
		g_lstprojdeps.AddTail( pDepInfo );
		return pDepInfo;
	}
	return NULL;
}

BOOL RemoveAllReferences( CString strProj ) 
{
	VCPOSITION pos = g_lstprojdeps.GetHeadPosition();
	while (pos != NULL)
	{
		VCPOSITION oldPos = pos;
		CProjDepInfo *pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->strProject == strProj || pDepInfo->strTarget == strProj )
		{
			g_lstprojdeps.RemoveAt( oldPos );
			delete pDepInfo;
		}
	}
	return FALSE;
}

BOOL RemoveDep(  CProjectDependency *pProjDep  )
{
	VCPOSITION pos = g_lstprojdeps.GetHeadPosition();
	while (pos != NULL)
	{
		VCPOSITION oldPos = pos;
		CProjDepInfo *pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->pProjDep == pProjDep )
		{
			g_lstprojdeps.RemoveAt( oldPos );
			delete pDepInfo;
			return TRUE;
		}
	}
	return FALSE;
}

CProjectDependency::CProjectDependency() 
{
	m_pTarget = NULL;
}

CProjectDependency::~CProjectDependency()
{
	DeleteContents();

	// Make sure that if we are really going away
	// that if there is an active project (we could
	// be on the clipboard) does not have us on its
	// dependancy list
	CProject* pProject = GetProject();
	if (pProject)
		pProject->RemoveDependant(this);

	// now remove the pointer from the global collection
	VCPOSITION pos = g_lstprojdeps.GetHeadPosition();
	while (pos != NULL)
	{
		CProjDepInfo *pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->pProjDep == this ){
			pDepInfo->pProjDep = NULL;
			break;
		}
	}
}

GPT CProjectDependency::GetStrProp(UINT idProp, CString& val)
{
	// is this a fake prop?
	
	if (idProp == P_ProjItemName)	// yes
	{
		if (m_pTarget != NULL)
			m_strName = val = m_pTarget->GetTargetName();
		else
			val = m_strName;  // as read from project file
		return valid;
	}
	
 	// no, pass on to base class
	return CProjItem::GetStrProp(idProp, val);
}

// Target name prefix written to the makefile
const TCHAR *CProjectDependency::pcDepPrefix = _TEXT("Project_Dep_Name ");
 
// Reads target name comments and sets up the config recs accordingly. Also
// registers the target with the target manager.
BOOL CProjectDependency::IsDepComment(CObject *&pObject)
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
	if (_tcsnicmp(pcDepPrefix, pc, _tcslen(pcDepPrefix)) != 0)
		return FALSE;

	// Ok, we found the prefix so skip over it.
	pc += _tcslen(pcDepPrefix);
	SkipWhite(pc);

	// Get the quoted fileset name
	if (!GetQuotedString(pc, pcEnd))
		return FALSE;

	*pcEnd = _T('\0');


	CProjDepInfo * pProjDepInfo = new CProjDepInfo;
	pProjDepInfo->pProjDep = this;

	CString strProject = GetTargetName();
	int iDash = strProject.Find(_T(" - "));
	if (iDash > 0)
		strProject = strProject.Left(iDash);
	pProjDepInfo->strProject = strProject;

	CString strName = pc;
	pProjDepInfo->strTarget = strName;
	m_strName = strName;

	g_lstprojdeps.AddTail(pProjDepInfo);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjectDependency::ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir)
{
//	Read in this item and  a makfile reader. 
//
	CObject *pObject = NULL;
	CPath cpFile;
	
	TRY
	{
		// The next element better be a comment marking our start:
		pObject = mr.GetNextElement ();
		ASSERT (pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)));

		delete pObject;

		m_cp = EndOfConfigs;
	   	for (pObject = mr.GetNextElement(); !IsEndToken(pObject); pObject = mr.GetNextElement())
	   	{
			// set the target we reference (m_pcr) by reading in P_TargetRefName
			if (IsDepComment(pObject)){
				delete (pObject); pObject = NULL;
				continue;
			}
 
			// make sure we set which target this target reference is contained in
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakDirective))) 
			{
				// read in the configuration information
				ReadConfigurationIfDef((CMakDirective *)pObject);
			}

	   		delete (pObject); pObject = NULL;
		}				 
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

// Return a container-less, symbol-less, clone of 'this'
CSlob* CProjectDependency::Clone()
{
	CSlob * pClone = CProjItem::Clone(); // call base class to create clone
	VSASSERT(pClone->IsKindOf (RUNTIME_CLASS (CProjItem)), "Can only clone into a CProjItem!");
	
	// Set up the clones target reference
	CString strTarget;
	GPT gptRet = GetStrProp(P_TargetRefName, strTarget);
	VSASSERT(gptRet == valid, "Failed to get P_TargetRefName!");
	BOOL bSuccess = pClone->SetStrProp(P_TargetRefName, strTarget);
	VSASSERT(bSuccess, "Failed to set P_TargetRefName!");

 	return pClone;
}

BOOL CProjectDependency::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// removing from a project?
	if (fFromPrj)
	{
		// the item is being removed from the project
		// (most likely to be moved onto the undo slob)
		// so we need to remove us as a dependant of the project
		GetProject()->RemoveDependant(this);
	}

	// do the base-class thing
	return CProjItem::PreMoveItem(pContainer, fFromPrj, fToPrj, fPrjChanged);
}

BOOL CProjectDependency::PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// moving into a project?
	if (fToPrj)
	{
		// we are being added to the project
		// so we need to add ourselves as a dependant of the project
		GetProject()->AddDependant(this);
		
		(VOID)::AddDep(this);
	}
	if( pContainer == NULL )
	{
		// find it on the list and remove it.
		::RemoveDep(this);
	}

	// do the base-class thing
	return CProjItem::PostMoveItem(pContainer, pOldContainer, fFromPrj, fToPrj, fPrjChanged);
}

///////////////////////////////////////////////////////////////////////////////
void CProjectDependency::FlattenQuery(int fo, BOOL& bAddContent, BOOL& fAddItem)
{
	if (fo & flt_ExcludeProjDeps)
		fAddItem = FALSE;
	else
		fAddItem = TRUE;
}
