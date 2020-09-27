//	PROJITEM.CPP
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop

#include "optncplr.h"
#include "targitem.h"
#include "dirmgr.h"
#include "project.h"
#include "makread.h"
#include "projdep.h"

IMPLEMENT_SERIAL (CProjItem, CProjSlob, SLOB_SCHEMA_VERSION)
IMPLEMENT_SERIAL (CFileItem, CProjItem, SLOB_SCHEMA_VERSION)
IMPLEMENT_DYNAMIC (CConfigurationRecord, CObject)

extern BOOL ConvertMacros(CString& rstrVal);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
const LPCTSTR g_pszUIDllName = _T("vcprojcnvtd.dll");
#else
const LPCTSTR g_pszUIDllName = _T("vcprojcnvt.dll");
#endif

// this variable is part of a MAJOR HACK involving the BuildCmds Macro and
// unsupported platforms in makefiles. find a betterr work around for this problem ASAP
// olympus bug #1103 for details.
CBuildTool *g_pBuildTool=NULL;

// 04-10-96 jimsc This is used by SuckMacro, but ReadMakeFile has to set it in project.cpp
CBuildTool *g_pLastTool = NULL;

//
// Helper function that test for a container being non-NULL or that the
// undo slob is recording(which mean that the undo slob could be the container)
//
static __inline BOOL CouldBeContainer(CSlob* pContainer)
{
	return (NULL!=pContainer || theUndoSlob.IsRecording());
}

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------- CConfigurationRecord ------------------------------
//
///////////////////////////////////////////////////////////////////////////////
CConfigurationRecord::CConfigurationRecord
(
	const CConfigurationRecord * pBaseRecord, 
	CProjItem * pOwner
) :
// increase the size of the hash table for project-level,
// these bags have the most props, to a more efficient value
// than the default of 17 and reduce for CfileItems and CDepItems
  m_BasePropBag(pOwner->GetBagHashSize()),
  m_CurrPropBag(pOwner->GetBagHashSize())
{
	// Owner project item
	m_pOwner = pOwner;

	// Record linkage
	m_pBaseRecord = pBaseRecord;
	#ifdef _DEBUG
	if (pBaseRecord) ASSERT_VALID (pBaseRecord);
	#endif
	// default is *NO* attributes which are dirty
	m_iAttrib = 0;
	m_fAttribDirty = TRUE;

	// Config rec is not valid for the owning item by default
	m_fValid = FALSE;

	// Actions list is allocated on demand..
	m_pActions = (CBuildActionList *)NULL;

	// TODO: REVIEW: don't new this until we need it !
	m_pClonePropBag = new CPropBag(pOwner->GetBagHashSize());

	m_pToolsMap = NULL;
}

CConfigurationRecord::~CConfigurationRecord()
{
	// delete our actions and actions list
	if (m_pActions)
	{
		VCPOSITION pos = m_pActions->GetHeadPosition();
		while (pos != (VCPOSITION)NULL)
		{
			CBuildAction * pAction = (CBuildAction *)m_pActions->GetNext(pos);

			// must not be bound, unless it's an unknown tool
			// we *just* directly added these, so we can *just* directly
			// delete them, no CBuildAction::UnAssign or CBuildAction::Assign used!
			VSASSERT(pAction->m_pTool == (CBuildTool *)NULL ||
				   pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)), "The only tools left assigned to actions at this point should be derived from CUnknownTool");
			delete pAction;
		}
		delete m_pActions;
	}
	
	if( m_pClonePropBag )
		delete m_pClonePropBag;
	
	if (m_pToolsMap)
	{
		VCPOSITION pos = m_pToolsMap->GetHeadPosition();
		while (pos)
		{
			CToolsMap * pToolMap = (CToolsMap * )m_pToolsMap->GetNext(pos);
			delete pToolMap;
		}
		delete m_pToolsMap;
	}
}

CBuildActionList * CConfigurationRecord::GetActionList()
{
	if (m_pActions == (CBuildActionList *)NULL)
		m_pActions = new CBuildActionList;
	return m_pActions;
}

BOOL CConfigurationRecord::IsSupported()
{
	CProjType * pprojtype;
	
	if (!g_pPrjcompmgr->LookupProjTypeByName(GetOriginalTypeName(), pprojtype))
		return FALSE;

	return pprojtype->IsSupported();
}

CString &CConfigurationRecord::GetConfigurationName () const
{
	VSASSERT (m_pBaseRecord, "Base record not set properly on config record");
	const CPropBag * pBag = ((CConfigurationRecord *)m_pBaseRecord)->GetPropBag(CurrBag);

	CStringProp *pProp = (CStringProp *)pBag->FindProp (P_ProjConfiguration);
	VSASSERT (pProp, "Failed to find P_ProjConfiguration property");
	VSASSERT (pProp->m_nType == string, "P_ProjConfiguration property not a string as expected!");
	return pProp->m_strVal;
}

void CConfigurationRecord::GetProjectName (CString &strProject) const
{
	VSASSERT (m_pBaseRecord, "Base record not set properly on config record");
	CProject * pProject = (CProject *)m_pBaseRecord->m_pOwner;
	VSASSERT(pProject, "Project not set properly on config record");
	const CPropBag * pBag = ((CConfigurationRecord *)m_pBaseRecord)->GetPropBag(CurrBag);

	CStringProp *pProp = (CStringProp *)pBag->FindProp (P_ProjConfiguration);
	VSASSERT (pProp, "Failed to find P_ProjConfiguration property");
	VSASSERT (pProp->m_nType == string, "P_ProjConfiguration property not a string as expected!");

	strProject = (const char *)((CStringProp *)pProp)->m_strVal;
	int iDash = strProject.Find(_T(" - "));
	if (iDash > 0)
  	  strProject = strProject.Left(iDash);

	return ;
}

void CConfigurationRecord::GetConfigurationDescription ( CString &strDescription ) const
{
	VSASSERT (m_pBaseRecord, "Base record not set properly on config record");
	const CPropBag * pBag = ((CConfigurationRecord *)m_pBaseRecord)->GetPropBag(CurrBag);

	CStringProp *pProp = (CStringProp *)pBag->FindProp (P_ProjConfiguration);
	VSASSERT (pProp, "Failed to find P_ProjConfiguration property");
	VSASSERT (pProp->m_nType == string, "P_ProjConfiguration property not a string as expected!");
	strDescription = pProp->m_strVal;

    strDescription = (const char *)((CStringProp *)pProp)->m_strVal;
	int nSep = strDescription.Find(_T(" - "));
	strDescription = strDescription.Right(strDescription.GetLength() - nSep - 3 );

	return;
}

CString &CConfigurationRecord::GetOriginalTypeName() const
{
	VSASSERT (m_pBaseRecord, "Base record not set properly on config record");
	const CPropBag * pBag = ((CConfigurationRecord *)m_pBaseRecord)->GetPropBag(CurrBag);

	// format is '<Platform> <ProjType>'
	CStringProp *pProp = (CStringProp *)pBag->FindProp (P_ProjOriginalType);
	VSASSERT (pProp, "Failed to find P_ProjConfiguration property");
	VSASSERT (pProp->m_nType == string, "P_ProjConfiguration property not a string as expected!");

	return pProp->m_strVal;
}

BOOL CConfigurationRecord::GetExtListFromToolName(CString strTool, CString *strExt)
{
	if (m_pToolsMap == NULL)
		return FALSE;

	CToolsMap * pToolMap;
	for (VCPOSITION pos = m_pToolsMap->GetHeadPosition(); pos != NULL;)
	{
		pToolMap = (CToolsMap *)m_pToolsMap->GetNext(pos);

		if(pToolMap->IsExternalTool())
			continue; //return FALSE;
		if(strTool == ((CBuildTool *)(pToolMap->m_pBuildTool))->GetToolName())
		{
			*strExt = pToolMap->m_strExtensions;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CConfigurationRecord::SetExtListFromToolName(CString strTool, CString strExt)
{
	if (m_pToolsMap == NULL)
		return FALSE;

	CToolsMap * pToolMap;
	for (VCPOSITION pos = m_pToolsMap->GetHeadPosition(); pos != NULL;)
	{
		pToolMap = (CToolsMap *)m_pToolsMap->GetNext(pos);

		if(pToolMap->IsExternalTool())
			continue; // return FALSE;
		if(strTool == ((CBuildTool *)(pToolMap->m_pBuildTool))->GetToolName())
		{
			pToolMap->m_strExtensions = strExt;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CConfigurationRecord::SetBuildToolsMap()
{
	CProjType* pProjType = m_pOwner->GetProjType();
	if (pProjType != NULL)
	{

		CBuildTool * pTool;
		const CVCPtrList * plistTools = pProjType->GetToolsList();
		for (VCPOSITION pos = plistTools->GetHeadPosition(); pos != NULL;)
		{
			pTool = (CBuildTool *)plistTools->GetNext(pos);
			CToolsMap * pToolMap = new CToolsMap();
			pToolMap->m_pBuildTool = pTool;
			pTool->GetDefaultExtList(&(pToolMap->m_strExtensions));
			if (m_pToolsMap == NULL)
				m_pToolsMap = new CVCPtrList();
			m_pToolsMap->AddTail(pToolMap);
		}
	}
	return TRUE;
}

int CConfigurationRecord::GetTargetAttributes()
{
	BOOL			fForce = FALSE;

	// do we need to force ourselves into the same config.?
	if (m_pOwner->GetActiveConfig() != this)
	{
		m_pOwner->ForceConfigActive(this);
 		fForce = TRUE;
	}

	// are our target attributes dirty?
	if (m_fAttribDirty)
	{
		m_iAttrib = 0;

		// for an internal project
		// cache our target attributes
		CProjType * pProjType = m_pOwner->GetProjType(); VSASSERT(pProjType != (CProjType *)NULL, "Invalid project type!");
		m_iAttrib = pProjType->GetAttributes();
		CPlatform *pPlatform = pProjType->GetPlatform();
		if (!pPlatform->IsSupported() || !pPlatform->GetBuildable())
			m_iAttrib &= ~TargetIsDebugable;        

		m_fAttribDirty = FALSE;
	}


	if (fForce)
		m_pOwner->ForceConfigActive();

	return m_iAttrib;	// return the attributes
}

void CConfigurationRecord::BagCopy(UINT idBagSrc, UINT idBagDest, UINT nPropMin, UINT nPropMax, BOOL fInform)
{
	CPropBag * pBagSrc = GetPropBag(idBagSrc), * pBagDest = GetPropBag(idBagDest);
	VSASSERT(pBagSrc != NULL, "Cannot copy from a NULL bag!");
	VSASSERT(pBagDest != NULL, "Cannot copy to a NULL bag!");

	CProp * ppropSrc, * ppropDest;

	// need to make sure we are in the right config in case of inform
	if (fInform)
		m_pOwner->SetManualBagSearchConfig(this);

	// copy from our source to the destination the props in
	// our range by either adding or replacing prop. values
	POSITION pos = pBagSrc->GetStartPosition();
	while (pos != NULL)
	{
		BOOL fDifferent = TRUE;

		UINT id;
		// get the next prop from our source
		pBagSrc->GetNextAssoc(pos, id, ppropSrc);

		// make range checkes against minimum and maximum props
		if ((nPropMin != -1 && id < nPropMin) ||
			(nPropMax != -1 && id > nPropMax))
			continue;

		// get the equiv. in our destination
		ppropDest = pBagDest->FindProp(id);

		// make sure the types are the same if found in both bags
		VSASSERT(ppropDest == (CProp *)NULL || ppropSrc->m_nType == ppropDest->m_nType, "Property type mismatch found");

		// do the copy (FUTURE: no Undo)
		switch (ppropSrc->m_nType)
		{
			case integer:
				if (ppropDest != (CProp *)NULL)
				{
					// exists in destination, just do in-place set and check for equivalence
					fDifferent = ((CIntProp*)ppropDest)->m_nVal != ((CIntProp*)ppropSrc)->m_nVal;
					((CIntProp*)ppropDest)->m_nVal = ((CIntProp*)ppropSrc)->m_nVal;
				}
				else
				{
					// doesn't exist, add the prop.
					pBagDest->AddProp(id, new CIntProp(((CIntProp*)ppropSrc)->m_nVal));
				}
				break;

			case string:
				if (ppropDest != (CProp *)NULL)
				{
					// exists in destination, just do in-place set and check for equivalence
					fDifferent = ((CStringProp*)ppropDest)->m_strVal != ((CStringProp*)ppropSrc)->m_strVal;
					((CStringProp*)ppropDest)->m_strVal = ((CStringProp*)ppropSrc)->m_strVal;
				}
				else
				{
					// doesn't exist, add the prop.
					pBagDest->AddProp(id, new CStringProp(((CStringProp*)ppropSrc)->m_strVal));
				}
				break;
		
			default:
				VSASSERT(FALSE, "Unhandled prop type");
				break;
		}

		if (fInform && fDifferent)
		{
			m_pOwner->InformDependants(id);

			// got an option handle for this?
			COptionHandler * popthdlr = g_pPrjcompmgr->FindOptionHandler(id);
			if (popthdlr != (COptionHandler *)NULL)
			{
				popthdlr->SetSlob(m_pOwner);

				// integer changes
				if (ppropSrc->m_nType == integer)
					popthdlr->OnOptionIntPropChange(id, ((CIntProp*)ppropSrc)->m_nVal);

				// string changes
				else if (ppropSrc->m_nType == string)
					popthdlr->OnOptionStrPropChange(id, ((CStringProp*)ppropSrc)->m_strVal);

				// reset the option handler CSlob
				popthdlr->ResetSlob();
			}
		}
	}

	// remove from our destination all those props. not copied
	pos = pBagDest->GetStartPosition();
	while (pos != NULL)
	{
		UINT id;
		// get the next prop in our destination
		pBagDest->GetNextAssoc(pos, id, ppropSrc);

		// make range checkes against minimum and maximum props
		if ((nPropMin != -1 && id < nPropMin) ||
			(nPropMax != -1 && id > nPropMax))
			continue;

		// not in our source bag, so delete (it exists!)
		if (!pBagSrc->FindProp(id) && pBagDest->FindProp(id))
		{
			if (fInform)
			{
				// m_pOwner->InformDependants(id);

				// review: should this also be deferred until after prop is changed

				// got an option handle for this?
				COptionHandler * popthdlr = g_pPrjcompmgr->FindOptionHandler(id);
				if (popthdlr != (COptionHandler *)NULL)
				{
					popthdlr->SetSlob(m_pOwner);

					// integer changes
					if (ppropSrc->m_nType == integer)
						popthdlr->OnOptionIntPropChange(id, 0);

					// string changes
					else if (ppropSrc->m_nType == string)
						popthdlr->OnOptionStrPropChange(id, "");

					// reset the option handler CSlob
					popthdlr->ResetSlob();
				}
			}

			pBagDest->RemoveProp(id);

			// defer inform until after prop is actually changed
			if (fInform)
			{
				m_pOwner->InformDependants(id);
			}
		}
	}

	// restore to previous state
	if (fInform)
		m_pOwner->ResetManualBagSearchConfig();
}

#ifdef _DEBUG
void CConfigurationRecord::AssertValid() const
{
	// should point to top level record
	VSASSERT(m_pBaseRecord == m_pBaseRecord->m_pBaseRecord, "Config record should point to top level record");
}

void CConfigurationRecord::Dump (CDumpContext &DC) const
{
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// --------------------------------	CProjItem ---------------------------------
//
///////////////////////////////////////////////////////////////////////////////
#define theClass CProjItem
BEGIN_SLOBPROP_MAP(CProjItem, CProjSlob)
END_SLOBPROP_MAP()
#undef theClass

const CString & CProjItem::GetTargetName() { return GetTarget()->GetTargetName(); }

void CProjItem::DeleteContents()
{
	// delete all its children
	// NOTE: We cannot use a normal GetHeadPosition, GetNext loop here,
	// as removing one item may result in removing another item in the list
	// This situation occurs when we remove a file item and this results in
	// the removal of the corresponding dependency container
	CObList * pContentList = GetContentList();
	if (pContentList)
	{
		while (!pContentList->IsEmpty())
		{
 	    	// any contained slobs should also be items..
			CProjItem * pItem = (CProjItem *) pContentList->GetHead();
			VSASSERT(pItem != NULL, "NULL pointer in content list!");
			ASSERT_VALID(pItem);
			RemoveDependant(pItem);

			// delete
			pItem->MoveInto(NULL);
		}
	}

	// delete our configuration records
	int nIndex, nSize = GetPropBagCount();
	for (nIndex = 0; nIndex < nSize; nIndex++)
		delete (CConfigurationRecord *)m_ConfigArray[nIndex];

	m_ConfigArray.RemoveAll();
	m_ConfigMap.RemoveAll();

	m_pActiveConfig = NULL;
}
///////////////////////////////////////////////////////////////////////////////
CProjItem::CProjItem ()	: m_ConfigMap (3)
{
	m_ConfigMap.InitHashTable(3);

	// our current 'owner' project is undefined currently!
	m_pOwnerProject = (CProject *)NULL;

	// our current 'owner' target is undefined currently!
	m_pOwnerTarget = (CTargetItem *)NULL;

	// by default prop search uses inheritance
	// by default prop search uses default
	// by default we are not anti-options
	// no manual bag find
	m_optbeh = OBInherit | OBShowDefault;

	// by default we are not figuring a dep!
	m_fPropsDepFigure = FALSE;

	// default is outside config switches
	m_cp = EndOfConfigs;

	// no active or forced configuration
	m_pActiveConfig = m_pForcedConfig = NULL;

	// no manual config rec. by deault
	m_pManualConfigRec = NULL;

	// not getting a list string using ::GetStrProp
 	m_fGettingListStr = FALSE;

	// Default is to use the current property bag for each config. rec.
	m_idBagInUse = CurrBag;

    m_pArchyHandle = NULL;
}

///////////////////////////////////////////////////////////////////////////////
CProjItem::~CProjItem()
{
}

///////////////////////////////////////////////////////////////////////////////
CTargetItem * CProjItem::GetTarget()
{
	// do we have one cached?
	if (m_pOwnerTarget != (CTargetItem *)NULL)
		return m_pOwnerTarget;

	// no, so try to find it given the active target
	CConfigurationRecord * pcr = GetActiveConfig();
	CProject * pProject = GetProject();
	if (pcr == (CConfigurationRecord *)NULL || pProject == (CProject *)NULL)
		return (CTargetItem *)NULL;

	// the active one
	return pProject->GetTarget(pcr->GetConfigurationName());
}
///////////////////////////////////////////////////////////////////////////////
// Removes project item from view 
BOOL CProjItem::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	return TRUE;	// success
}

// Alters the containment and fixes configs. of item,
// but not 'officially' in or out-of project
BOOL CProjItem::MoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	BOOL fRefreshDeps = FALSE;

	CTargetItem * pTarget = GetTarget();
	CProject * pProject = GetProject();

	// moving from a project?
	if ((fFromPrj) && (pProject != NULL))
	{
 
		VSASSERT(pTarget != NULL, "Project item not connected to a target!  Deep trouble.");

		// set the project to into each valid config. for this target
		const CVCPtrArray * pCfgArray = pTarget->GetConfigArray();
		INT_PTR iSize = pCfgArray->GetSize();
		for (int i = 0; i < iSize; i++)
		{
			CConfigurationRecord * pcr = (CConfigurationRecord *)pCfgArray->GetAt(i);

			if ( pcr->IsValid())
			{
				// FUTURE: optimize by only doing this if its moving to a new target...
				
				// un-assign actions to each of this items configurations
				// *OR* if we going into an undo then un-bind actions from the build instance
				if (fPrjChanged || (!CouldBeContainer(pContainer) && !fToPrj))
					CBuildAction::UnAssignActions(this, (CVCPtrList *)NULL, pcr, CouldBeContainer(pContainer));
				else if (!fToPrj)
					CBuildAction::UnBindActions(this, (CVCPtrList *)NULL, pcr, TRUE /* CouldBeContainer(pContainer) */ );

			}
		}

		// removing item altogether from project?
		// and project not being created/destroyed?
		if ((!fToPrj || fPrjChanged) && GetProject()->m_bProjectComplete)
		{
			// if we're removing a project item from the project we need to 
			// isolate it from the container
			IsolateFromProject(); // Rvw: overkill for drag & drop
		}
	}	

	// fix our 'cached' containment
	// so that any notifies from below work
	// moved to a project?
	if (fToPrj)
	{
		// Search for our new 'owner' project correctly
		CSlob * pSlob = pContainer;
		CTargetItem * pNewTarg = NULL;
		while (!pSlob->IsKindOf(RUNTIME_CLASS(CProject)))
		{
			// can we find the target 'owner'?
			if (pSlob->IsKindOf(RUNTIME_CLASS(CTargetItem)))
				pNewTarg = (CTargetItem *)pSlob;

			// must be able to find a project 'owner'
			pSlob = pSlob->GetContainer();
			VSASSERT(pSlob != (CSlob *)NULL, "No owner!");
		}

		if ((pNewTarg==NULL) && (IsKindOf(RUNTIME_CLASS(CTargetItem))))
			pNewTarg = (CTargetItem *)this; // hey, we are the target

		// Cache our notion of the 'owner' project
		SetOwnerProject((CProject *)pSlob, pNewTarg);
	}
	// else from a project

	// alter our containment
	// if pContainer == NULL then this will *delete*
	// ourselves, do *not* access object data from here
	// on out in this situation
	if (!CSlob::SetSlobProp(P_Container, pContainer))
		return FALSE;

	// fix our 'cached' containment
	// so that any notifies from below work
	// moved to a project?
	if (fToPrj)
	{
		// moving to a project by removing from a project or undo'ing?
		// yes, then match the new container's configurations.
		// (This may require the deletion of some)
		if (fFromPrj || fPrjChanged || (pOldContainer == NULL) || pOldContainer == &theUndoSlob)
			CloneConfigs((const CProjItem *)pContainer);
	}
	// else from a project into something else
	else if (theUndoSlob.IsRecording())
	{
		// *Not* a project item -> *No* owner project -> *No* owner target,
		// most likely contained in the undo CSlob, or some such thing
		VSASSERT((pContainer == (CSlob *)NULL) || pContainer == &theUndoSlob || pContainer->IsKindOf(RUNTIME_CLASS(CClipboardSlob)), "Bad state");
		SetOwnerProject(NULL, NULL);
	}

	return TRUE;	// success
}

// Make item item 'officially' in or out-of project    
BOOL CProjItem::PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// moving into the project?
	if (fToPrj)
	{
		CTargetItem * pTarget = GetTarget();
		CProject * pProject = GetProject();

		VSASSERT(pTarget!=NULL, "No target?  Deep trouble.");
		VSASSERT(pProject!=NULL, "No project?  Deep trouble.");
		if ((pTarget==NULL) || (pProject==NULL))
			return FALSE; // can happen with bogus makefile

		// Set the project to into each valid config. for this target
		const CVCPtrArray * pCfgArray = pTarget->GetConfigArray();
		INT_PTR iSize = pCfgArray->GetSize();
		for (int i = 0; i < iSize; i++)
		{
			CConfigurationRecord * pcr = (CConfigurationRecord *)pCfgArray->GetAt(i);

			if ( pcr->IsValid())
			{
				// assign actions to each of the items configurations or
				// *OR* if we coming in from an undo then re-bind to the build instance
				if (((!CouldBeContainer(pOldContainer)) && (!fFromPrj)) || fPrjChanged)
					CBuildAction::AssignActions(this, (CVCPtrList *)NULL, pcr, CouldBeContainer(pOldContainer));

				else if (!fFromPrj)
					CBuildAction::BindActions(this, (CVCPtrList *)NULL, pcr, CouldBeContainer(pOldContainer));

			}
		}
	}

	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
void CProjItem::GetMoniker(CString& cs) 
{
	GetStrProp (P_ProjItemName, cs);
}
///////////////////////////////////////////////////////////////////////////////
int CProjItem::CompareMonikers (CString& str1, CString& str2)
{
	return _tcsicmp ((const char *)str1, (const char *)str2);
}	
///////////////////////////////////////////////////////////////////////////////
const CPath *CProjItem::GetFilePath() const
{
	return NULL;
} 
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SetFile (const CPath *pPath, BOOL bCaseApproved /* = FALSE */)
{
	TRACE ( "CProjItem::SetFile() called for non-File class %s\n", 
												GetRuntimeClass()->m_lpszClassName );
	return FALSE;
}

// FUTURE: move these into bldrfile.cpp
const TCHAR *CProjItem::EndToken = _TEXT ("End");
const TCHAR *CProjItem::pcFileMacroName = _TEXT ("SOURCE");
const TCHAR *CProjItem::pcFileMacroNameEx = _TEXT ("$(SOURCE)"); 	

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir)
{
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
const CProjItem::PropWritingRecord CProjItem::nMakeFileProps[] = {
	{P_ProjUseMFC,	 			_TEXT("Use_MFC"),				integer, PropRead | PropWrite | All},
	{P_UseDebugLibs,	 		_TEXT("Use_Debug_Libraries"),	integer, PropRead | PropWrite | All},
	{P_OutDirs_Target, 			_TEXT("Output_Dir"),			string, PropRead | PropWrite | All},
	{P_OutDirs_Intermediate,	_TEXT("Intermediate_Dir"),		string, PropRead | PropWrite | All},
	{P_Proj_TargDefExt,	 		_TEXT("Target_Ext"),			string, PropRead | PropWrite | Project},
	{P_Proj_CmdLine,			_TEXT("Cmd_Line"),				string, PropRead | PropWrite | Project},
	{P_Proj_RebuildOpt,			_TEXT("Rebuild_Opt"),			string, PropRead | PropWrite | Project},
	{P_Proj_CleanLine,			_TEXT("Clean_Line"),			string, PropRead | PropWrite | Project},
	{P_Proj_Targ,				_TEXT("Target_File"),			string, PropRead | PropWrite | Project},
	{P_Proj_BscName,			_TEXT("Bsc_Name"),				string, PropRead | PropWrite | Project},
	{P_Proj_ComPlus,			_TEXT("ComPlus"),				integer, PropRead | PropWrite | Project},
	{P_ProjItemOrgPath,			_TEXT("RelativePath"),			string, Project | NotPerConfig },
	{P_Proj_UserToolsName,		_TEXT("User_Tools_Name"),		string, PropRead | PropWrite | Project},
	{P_ItemExcludedFromBuild,	_TEXT("Exclude_From_Build"),	integer, PropRead | PropWrite | File | TargetRef},
	{P_ItemExcludedFromScan,	_TEXT("Exclude_From_Scan"),	integer, PropRead | PropWrite | File | Group | NotPerConfig},
	{P_ProjClsWzdName,			_TEXT("Classwizard_Name"),		string, PropRead | PropWrite | Project},
	{P_GroupDefaultFilter,		_TEXT("Default_Filter"),	string, PropRead | PropWrite | Group | NotPerConfig },
	{P_Proj_PerConfigDeps,		_TEXT("AllowPerConfigDependencies"),	integer, PropRead | PropWrite | Project | NotPerConfig },
	{P_Proj_IgnoreExportLib,	_TEXT("Ignore_Export_Lib"),		integer, PropRead | PropWrite | Project },
	{P_ItemIgnoreDefaultTool,	_TEXT("Ignore_Default_Tool"),		integer, PropRead | PropWrite | File },
	{P_ItemCustomBuildExt,		_TEXT("Custom_Build_Extension"),	string, PropRead | PropWrite | File | NotPerConfig},
	{P_ItemCustomBuildTime,		_TEXT("Custom_Build_Time"),			string, PropRead | PropWrite | File},
	{P_ItemBuildTool,			_TEXT("Build_Tool"),				string, PropRead | PropWrite | File},

// Scc
	{P_ProjSccProjName,			_TEXT("Scc_ProjName"),	string, PropRead | PropWrite | NotPerConfig | Project},
	{P_ProjSccRelLocalPath,		_TEXT("Scc_LocalPath"),	string, PropRead | PropWrite | NotPerConfig | Project},

// NB: if you add or remove any Writeable props here, you MUST also
// update the constant nWriteableProps constant below:
#define nWriteableProps (25)

// we should eliminate this redundancy, put in a marker item or put the two types of props in separate arrays

	// Really, these are project only.  Also, they are not per config, so 
	// they would not be written out properly.  However, they are only read in:
	{P_ProjMarkedForScan,		_TEXT("Marked_For_Scan"),		integer, PropRead | Project | NotPerConfig },
	{P_ProjMarkedForSave,		_TEXT("Marked_For_Save"),		integer, PropRead | Project | NotPerConfig },
	{P_ProjHasNoOPTFile,		_TEXT("Has_No_OPT_File"),		integer, PropRead | Project | NotPerConfig },
	{P_TargetRefName,		_TEXT("Target_Ref_Name"),		string, PropRead | TargetRef },
	{P_Proj_TargDir,	 		_TEXT("Target_Dir"),			string, PropRead | Project},	// only needed for conversions (ignored)
	{P_ConfigDefaultDebugExe,	_T("Debug_Exe"),				string, PropRead | Target },	// this one IS per config
// NB: if you add or remove any Read-only props here, you MUST also
// update the constant nReadOnlyProps constant below:
#define nReadOnlyProps (6) // should be eliminated, as above
	{P_END_MAP, NULL, null},
};

// FUTURE: move these into bldfile.cpp
const TCHAR *CProjItem::pcPropertyPrefix 	= _TEXT("PROP ");
const TCHAR *CProjItem::pcADD 				= _TEXT("ADD ");
const TCHAR *CProjItem::pcSUBTRACT			= _TEXT("SUBTRACT ");
const TCHAR *CProjItem::pcBASE				= _TEXT("BASE ");

// Consider: use this elsewhere
static CString & StripTrailingSlash(CString & str)
{
	int len = str.GetLength();
	_TCHAR * pchStart = str.GetBuffer(len);
	_TCHAR * pchEnd = pchStart + len;
	if (len > 1)
	{
		pchEnd = _tcsdec(pchStart, pchEnd);
		if ((pchEnd > pchStart) && ((*pchEnd =='/') || (*pchEnd == '\\')))
		{
			*pchEnd = 0;
			len = (int)(pchEnd - pchStart);
		}
	}

	str.ReleaseBuffer(len);
	return str;
}

BOOL CProjItem::GetOutDirString
(
	CString & str,
	CString * pstrValue, /* = NULL */
	BOOL bAllowItemMacro, /* = FALSE */
	BOOL fRecurse /* = FALSE */
)
{
	VSASSERT(!bAllowItemMacro, "bAllowItemMacro = true no longer supported"); // no longer supported

	// default is target prop for project
	UINT PropId = P_OutDirs_Target;

	CProjItem * pItem = GetProject();
	VSASSERT(pItem != (CProjItem *)NULL, "No project!  Deep trouble.");

	BOOL fForced = FALSE;

	// are we not a project or target item?
	if (this != pItem && this != GetTarget())
	{
		// no
		PropId = P_OutDirs_Intermediate;

		// can we get the property?
		if (GetPropBag()->FindProp(PropId))
		{
			// if (!bAllowItemMacro)
			return FALSE;

			// yes so use us!
			// pItem = (CProjItem *)this;
		}
		else
		{
			// try to get it from our parent project instead
			// (base-record *is* project config. record)
			VSASSERT(GetActiveConfig(), "No active config?!?  Deep trouble.");
			if (pItem->GetActiveConfig() != GetActiveConfig()->m_pBaseRecord)
			{
				pItem->ForceConfigActive((CConfigurationRecord *)GetActiveConfig()->m_pBaseRecord);
				fForced = TRUE;
			}
		}
	}

	BOOL fRet = pItem->GetOutDirString(str, PropId, pstrValue, TRUE, fRecurse);

	if (fForced) // restore original config.?
		pItem->ForceConfigActive();

	return fRet;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::GetOutDirString
(
	CString & str,
	UINT PropId,
	CString * pstrValue, /* = NULL */
	BOOL bMacro, /* = TRUE */
	BOOL fRecurse /* = FALSE */
)
{	
	// the rest are output directory related
	// find one somewhere in the hierarchy
	CProjItem * pItem = this;

	CProp * pprop = pItem->GetPropBag()->FindProp(PropId);
	while (fRecurse && pprop == (CProp *)NULL)
	{
		// *chain* the proper. config.
		CProjItem * pItemOld = pItem;
		pItem = (CProjItem *)pItem->GetContainerInSameConfig();
		if (pItemOld != (CProjItem *)this)
			pItemOld->ResetContainerConfig();

		if (pItem == (CProjItem *)NULL)
			break;	// not found in hierarchy

		pprop = pItem->GetPropBag()->FindProp(PropId);
	}

	// reset the last container we found
	if (pItem != (CProjItem *)NULL && pItem != (CProjItem *)this)
		pItem->ResetContainerConfig();

	// did we get one? fail if this isn't the target level
	if (pprop == (CProp *)NULL && !IsKindOf(RUNTIME_CLASS(CProject)))
		return FALSE;	// failure

	if (pstrValue)
	{
		// get the value
		CString strOutDir = pprop == (CProp *)NULL ? _TEXT("") : ((CStringProp *)pprop)->m_strVal;

		if (strOutDir.IsEmpty())
		{
			*pstrValue = _T('.');
		}
		else
		{
			strOutDir.TrimLeft();
			pstrValue->GetBuffer(strOutDir.GetLength() + 3);
			pstrValue->ReleaseBuffer(0);

			const TCHAR * pch = (const TCHAR *)strOutDir;

			// prepend ".\\" if not absolute or UNC path or already relative
			if ((pch[0] != _T('\\')) && 
				(pch[0] != _T('/')) &&
				(!(pch[0] == _T('.') && pch[1] == _T('\\'))) &&
				(!IsDBCSLeadByte (pch[0]) && (pch[1] != _T(':'))) || (IsDBCSLeadByte (pch[0])) 
				)
			{
				*pstrValue += _T(".\\");
			}

			*pstrValue += strOutDir;
			StripTrailingSlash(*pstrValue);
		}
	}

	str.GetBuffer(13); str.ReleaseBuffer(0);

	if (bMacro) str += _T("$(");

	// get the output directory macro name
	str += PropId == P_OutDirs_Intermediate ? _T("INTDIR") : _T("OUTDIR");

	if (bMacro) str += _T(')');

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SubstOutDirMacro(CString & str, const CConfigurationRecord * pcr)
{
	// substitute for $(OUTDIR) macro if nessessary

	if (pcr==NULL)	// this is not optional
		return FALSE;

	int nFndLoc;
	CString strOutDirMacro, strOutDirValue;
	BOOL retval = FALSE;
	if (pcr != GetActiveConfig())
		SetManualBagSearchConfig((CConfigurationRecord *)pcr);

	if (GetOutDirString(strOutDirMacro, &strOutDirValue))
	{
		nFndLoc = str.Find(strOutDirMacro);
		if (nFndLoc != -1)
		{
			str = str.Left(nFndLoc) + strOutDirValue + str.Mid(nFndLoc + strOutDirMacro.GetLength());
			retval = TRUE;
		}
	}

	if (pcr != GetActiveConfig())
		ResetManualBagSearchConfig();

	return retval;
}

			///////////////////////////////////////////////////////////////////////////////
// FUTURE (karlsi): move into class CProp::operator==
BOOL CompareProps(const CProp * pProp1, const CProp * pProp2)
{
	if ((pProp1==NULL) || (pProp2==NULL))
		return (pProp1==pProp2);

	VSASSERT(pProp1->m_nType==pProp2->m_nType, "Property type mismatch");

	switch (pProp1->m_nType)
	{
		case integer:
			return (((CIntProp *)pProp1)->m_nVal==((CIntProp *)pProp2)->m_nVal);
		case booln:
			return (((CBoolProp *)pProp1)->m_bVal==((CBoolProp *)pProp2)->m_bVal);
		case string:
			return (((CStringProp *)pProp1)->m_strVal==((CStringProp *)pProp2)->m_strVal);
		default:
			VSASSERT(0, "Unhandled prop type");	// other types not implemented
	};

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::IsPropComment (CObject *&pObject)
{
	// Pls. note that this will digest props regardless of whether or not they are
	// in the prop bag or not....
	if (!pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
		return FALSE; 

	BOOL retval = FALSE; // assume failure

	CMakComment * pMakComment = (CMakComment *)pObject;

	// get the comment and skip leading whitespace
	TCHAR * pc = pMakComment->m_strText.GetBuffer(1);
	SkipWhite(pc);

	// look for the 'Begin Custom Build' prefix
	if (_tcsnicmp(g_pcBeginCustomBuild, pc, _tcslen(g_pcBeginCustomBuild)) == 0)
	{
		CProject * pProject = GetProject();
		if (pProject->m_bConvertedDS6 || pProject->m_bConvertedDS5x || pProject->m_bConvertedDS4x)
		{
			CString strExt = GetFilePath()->GetExtension();
			if (strExt.CompareNoCase(_T(".idl"))==0) // REVIEW
			{
				SetIntProp(P_ItemIgnoreDefaultTool, TRUE);
			}
		}

		// read in the custom build rule
		BOOL fRet = CCustomBuildTool::DoReadBuildRule(this, pc);

		delete pObject; pObject = (CObject *)NULL;
		return fRet;
	}

	// look for the 'Begin Special Build Tool' prefix
	if (_tcsnicmp(g_pcBeginSpecialBuildTool, pc, _tcslen(g_pcBeginSpecialBuildTool)) == 0)
	{
		// read in the custom build rule
		BOOL fRet = CSpecialBuildTool::DoReadBuildRule(this, pc,(m_cp == EndOfConfigs));

		delete pObject; pObject = (CObject *)NULL;
		return fRet;
	}

	// look for the 'PROP' prefix
	if (_tcsnicmp(pcPropertyPrefix, pc, 4) != 0)
		return FALSE;

	int i, j, k, nConfigs=0;
	CString str;
	TCHAR *pcEnd;

	// Get the property name and look for it in the nMakeFileProps array:
	pc += 4;
	SkipWhite (pc);

	// check if BASE property
	BOOL fUseBase = _tcsnicmp(pcBASE, pc, 5) == 0; 
	if (fUseBase)
	{
		// skip over the "BASE" keyword
		pc += 5; SkipWhite(pc);
	}

 	// set the prop. bag usage
	int idOldBag = UsePropertyBag(fUseBase ? BaseBag : CurrBag);

	BOOL bBlankProp = FALSE;
	GetMakToken(pc, pcEnd);
 	bBlankProp = (*pcEnd == _T('\0'));
	*pcEnd = _T('\0');

	for (i=0; nMakeFileProps[i].idProp != P_END_MAP; i++)
	{
		if (!(nMakeFileProps[i].nRdWrt & PropRead)) continue;
		if ( _tcsicmp (nMakeFileProps[i].pPropName, pc) ==0 ) break;
	}
	if (nMakeFileProps[i].idProp != P_END_MAP )
	{
 		if (bBlankProp)
			pc = pcEnd;
		else
			pc = ++pcEnd;
        
        // Get the end of the prop line
        while (*pcEnd)
            pcEnd = _tcsinc(pcEnd);        

		if (m_cp == EndOfConfigs)
		{
			// global across *all configs* so
			// make sure we have all of our configs matching the project
			CreateAllConfigRecords();

			nConfigs = GetPropBagCount();
		}

		switch (nMakeFileProps[i].type)
		{
			case string:
			{
                // We strip quotes around the property, this is only
                // for compatability with previous makefiles, and should
                // not really be needed here.
                // FUTURE (colint): Delete this when we have a v2 bld convtr
                // that does this, and when most internal v3 makefiles have
                // been saved out to not have surrounding quotes!
                if (*pc == _T('"'))
                {
                    pc++;
                    if (*_tcsdec(pc, pcEnd) == _T('"'))
                        *(--pcEnd) = _T('\0');
                    else
                        VSASSERT(FALSE, "unmatched quotes");
                }

				// copy the string arg. between quotes into buffer
				str = pc;

				// may need to re-relativize paths if converted project
				if (GetProject()->m_bConvertedDS4x)
				{
					int id = nMakeFileProps[i].idProp;
					if ((id == P_OutDirs_Target) || (id == P_OutDirs_Intermediate) || (id == P_Proj_TargDir))
					{
						// Use special workspace dir (not actual wkspCDir)
						if ((!str.IsEmpty()) && (str[0] != _T('\\')) && (str[0] != _T('/')) && ((str.GetLength() < 2) || (str[1] != _T(':'))))
						{
							// REVIEW: should really add a temp filename here
							VSASSERT(g_pConvertedProject, "Global converted project pointer not set up properly for 4.X conversion");
							VSASSERT(*(LPCTSTR)g_pConvertedProject->GetWorkspaceDir(), "Workspace for 4.X conversion project not set up properly");
							CPath pathDir;
							pathDir.SetAlwaysRelative();
							if (pathDir.CreateFromDirAndFilename(g_pConvertedProject->GetWorkspaceDir(), str))
							{
								if (_tcsicmp((LPCTSTR)pathDir, (LPCTSTR)GetProject()->GetWorkspaceDir())==0)
									str = ".";
								else
									pathDir.GetRelativeName(GetProject()->GetWorkspaceDir(), str);
							}
						}
					}
				}
				if ((m_cp == EndOfConfigs) && !(nMakeFileProps[i].nRdWrt & NotPerConfig))
				{
					for (k=0; k < nConfigs; k++)
					{
						// Force config. active, set prop, and then restore.
						ForceConfigActive((CConfigurationRecord *)m_ConfigArray[k]);
						SetStrProp(nMakeFileProps[i].idProp, str);
						ForceConfigActive();
					}
				}
				else
				{
					VSASSERT((m_cp == EndOfConfigs) || !(nMakeFileProps[i].nRdWrt & NotPerConfig), "Malformed project file");
					SetStrProp ( nMakeFileProps[i].idProp, str );
				}
				str.Empty ();
				break;
			}

			case integer:
			case booln:
			{
				BOOL b = FALSE;
				j = atoi (pc);

				// using MFC property?
   				if (nMakeFileProps[i].idProp == P_ProjUseMFC)
				{
					// adjust...
					// do we have a use of MFC by AppWiz (LSB3 set)
					b = j & 0x4; j &= 0x3;
				}
										    
				if ((m_cp == EndOfConfigs) && !(nMakeFileProps[i].nRdWrt & NotPerConfig))
				{
					for (k=0; k < nConfigs; k++)
					{
						// Force config. active, set prop, and then restore.
						ForceConfigActive((CConfigurationRecord *)m_ConfigArray[k]);

						SetIntProp(nMakeFileProps[i].idProp, j);

						if (nMakeFileProps[i].idProp == P_ProjUseMFC)
							SetIntProp(P_ProjAppWizUseMFC, b);

						ForceConfigActive();
					}
				}
				else
				{
					VSASSERT((m_cp == EndOfConfigs) || !(nMakeFileProps[i].nRdWrt & NotPerConfig), "Malformed project file");
					SetIntProp(nMakeFileProps[i].idProp, j);

					if (nMakeFileProps[i].idProp == P_ProjUseMFC)
						SetIntProp(P_ProjAppWizUseMFC, b);
				}
			 	break;
			}

			default:
				VSASSERT (FALSE, "Unsupported type"); // Other types not supported.
				break;

		}
	}

	// we'll just ignore those props that we don't recognise
	// rather than flag the .MAK as unreadable	
	retval = TRUE; // indicate success

   	// reset the prop. bag usage
 	UsePropertyBag(idOldBag);

	delete pObject; pObject = (CObject *)NULL;

	return retval;
}

///////////////////////////////////////////////////////////////////////////////
BOOL  CProjItem::IsEndToken (CObject *& pToken )
{
	if (pToken->IsKindOf ( RUNTIME_CLASS (CMakEndOfFile)))
	{
		delete (pToken); pToken = NULL;
		AfxThrowFileException (CFileException::generic);
	}
	
	if (!pToken->IsKindOf ( RUNTIME_CLASS (CMakComment))) return FALSE;

	const TCHAR *pc = ((CMakComment *) pToken)->m_strText;
	SkipWhite (pc);
	if ( _tcsnicmp ( pc, EndToken, (sizeof(EndToken) -1) / sizeof(TCHAR)) == 0) 
	{
		delete (pToken); pToken = NULL;
		return TRUE;
	}
	return FALSE; 
}
///////////////////////////////////////////////////////////////////////////////
CRuntimeClass *CProjItem::GetRTCFromNamePrefix (const TCHAR *pcPrefix)
{
	// Skip over the "Begin":
	SkipWhite    (pcPrefix);
	SkipNonWhite (pcPrefix);
	SkipWhite    (pcPrefix);

	if ( *pcPrefix == '\0')
		return NULL;

 	// Get the runtime class by looking at the name.
 	if (_tcsnicmp(pcPrefix, _TEXT ("Source File"), 11) == 0)
		return RUNTIME_CLASS ( CFileItem );
 	else if (_tcsnicmp(pcPrefix, _TEXT ("Time Custom Build Step"), 17) == 0)
		return RUNTIME_CLASS ( CTimeCustomBuildItem );
	else if (_tcsnicmp(pcPrefix, _TEXT ("Group"), 5) == 0)
		return RUNTIME_CLASS ( CProjGroup );
	else if (_tcsnicmp(pcPrefix, _TEXT ("Project Dependency"), 18) == 0)
		return RUNTIME_CLASS ( CProjectDependency );
	else if (_tcsnicmp(pcPrefix, _TEXT ("Project"), 7) == 0)
		return RUNTIME_CLASS ( CProject );
	else if (_tcsnicmp(pcPrefix, _TEXT ("Target"), 6) == 0)
		return RUNTIME_CLASS ( CTargetItem );
 
 	TRACE ( "CProjItem::GetRTCFromNamePrefix got unknown prefix %s\n",
						pcPrefix );
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SuckDescBlk(CMakDescBlk *pDescBlk)
{
	BOOL retVal = TRUE;
	BOOL bOldInherit;
	CString str = pDescBlk->m_strTool;

	// get a project type
	CProjType * pprojtype = GetProjType();
	int len = str.GetLength();

	// ignore blk if pseudo-rule for OUTDIR/INTDIR/INTDIR_SRC
	if ((len >= 6) && (!str.Mid(2,4).Compare(_T("TDIR"))))
		return TRUE;

	if ( ( (_tcsicmp(_T("ALL"), pDescBlk->m_strTargets) == 0) ||
		   (_tcsicmp(_T("CLEAN"), pDescBlk->m_strTargets) == 0) )  && 
		 pprojtype->IsSupported()
	   )
		return TRUE;

	if ((len == 0) && (len = pDescBlk->m_strTargets.Find(_T('_'))) > 0)
	{
		// FUTURE: consider handling this in ParseDescBlk instead
		// also look for a "Fake" target of the form TOOLPREFIX_xxx
		str = pDescBlk->m_strTargets.Left(len);
		VSASSERT(len==str.GetLength(), "Size mismatch");
	}

	INT_PTR i = 0, nMax = m_cp == EndOfConfigs ? GetPropBagCount() : 1;

	// bounds for iteration
	// iterate through all configs if m_cp == EndOfConfigs
	if (m_cp == EndOfConfigs)
	{ 
		// global across *all configs* so
		// make sure we have all of our configs matching the project
		CreateAllConfigRecords();
	
	 	(void) ForceConfigActive((CConfigurationRecord *)m_ConfigArray[0]);
	}

	// nothing to do unless unsupported projtype-specific block
	TRY
	{
		for (;;) // iterate through configs if necessary based on m_cp
		{
			// get a tool for the prefix?
			CBuildTool * pTool = len > 0 ? pprojtype->GetToolFromCodePrefix(str, len) : (CBuildTool *)NULL;

 			// get the action list
			VSASSERT(GetActiveConfig(), "No active config?!?");
			CBuildActionList * pActions = GetActiveConfig()->GetActionList();

			// currently got a tool?
			// FUTURE: support multiple actions per-item
			CBuildTool * pCurrentTool = (CBuildTool *)NULL;
			if (!pActions->IsEmpty() && !IsKindOf(RUNTIME_CLASS(CProject)))
				pCurrentTool = ((CBuildAction *)pActions->GetHead())->BuildTool();

			// preserve this tool-specific block
			// o we recognise the block as for an unknown tool, or
			// o we don't have a tool already for the block
			if ((!pTool || pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool))))
			{
				// if don't already have a tool, then use the current one
				// else create it
				if (!pCurrentTool)
				{
					if (!pTool)
					{
						// try to get an unknown tool
						const TCHAR * pchPrefix = len > 0 ? (const TCHAR *)str : _TEXT("UNK");
						pTool = pprojtype->GetToolFromCodePrefix(pchPrefix, 3);
						
						if( (_tcsicmp(pchPrefix,"BuildCmds") == 0) && (pTool == NULL) ){
							pTool = g_pBuildTool;
						} 
						
						if (pTool == (CBuildTool *)NULL)
						{
							// no, unknown one, create new projtype-specific 'unknown' tool
							pTool = new CUnknownTool(*pprojtype->GetPlatformName(), pchPrefix);
							pprojtype->AddTool(pTool);
						}
					}

					// add an action if we don't already have one for this tool...
					if (!pActions->Find(pTool))
					{
						// explicitly add an action for this tool
						CBuildAction * pAction;
						pAction = new CBuildAction(this, pTool, FALSE, GetActiveConfig());
						pActions->AddTail(pAction);
					}
				}
				else
				{
					// use the current tool
					pTool = pCurrentTool;
				}

				// read in the raw-data for this descriptor block

				if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)) && ( _tcsicmp(_T("ALL"), pDescBlk->m_strTargets) != 0))
				{
					str.Empty();
					bOldInherit = EnablePropInherit(FALSE);
					if (GetStrProp(((CUnknownTool *)pTool)->GetUnknownPropId(), str)!=valid)
						str.Empty();

					// make sure a blank line precedes the build rule
					int len1 = str.GetLength();
					VSASSERT(len1==0 || len1 >= 4, "Blank line needed before build rule");
					if ((len1==0) || ((len1 >= 4) && (_tcscmp((LPCTSTR(str) + len1 - 4), _T("\r\n\r\n"))!=0)))
						str += _T("\r\n");

					str += pDescBlk->m_strRawData;
					SetStrProp(((CUnknownTool *)pTool)->GetUnknownPropId(), str);
					EnablePropInherit(bOldInherit);
				}
			}

			// repeat for next config, if necessary
			if (++i >= nMax) break;

			ForceConfigActive();
			ForceConfigActive((CConfigurationRecord *)m_ConfigArray[i]);
		}
	}
	CATCH (CException, e)
	{
		retVal = FALSE;	// failure
	}
	END_CATCH

	ForceConfigActive();
	return retVal;
}

enum { ONOPTION, ONPATH, NOTFOUND };


///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SuckMacro(CMakMacro * pMacro, const CDir & BaseDir, BOOL bCaseApproved /*=FALSE*/)
{
//	Examine a macro and figure out what its for.  Either its the dependencies
//	macro (in which case it starts with DEP) or it belongs to some tool, in 
//  which case we can get the tool from the macro's prefix:
//  for a tool

	const TCHAR * pMacName = pMacro->m_strName;

	// ignore OUTDIR and INTDIR macros
	if ((_tcsnicmp(_TEXT("OUTDIR"), pMacName, 6) == 0) ||
	    (_tcsnicmp(_TEXT("INTDIR"), pMacName, 6) == 0)
	   )
	{
		return TRUE;	// ok
	}

	BOOL retVal = TRUE;
	BOOL fPerConfig = m_cp != EndOfConfigs;

	TRY
	{	
		// dependency macro?
		if (_tcsnicmp(_TEXT("USERDEP_"), pMacName, 8) == 0) // user-defined dependency
		{
			// scanned or missing dependency?
			UINT depType = DEP_UserDefined;
			pMacName += (8 * sizeof(TCHAR));

			// if we haven't got any actions then this must be for an unknown tool
			VSASSERT(GetActiveConfig(), "No active config?!?  Project must not be set up properly yet");
			CBuildActionList * pActions = GetActiveConfig()->GetActionList();
			if (pActions->IsEmpty())
			{
				CBuildTool * pTool;

				// search for tool prefix
				TCHAR * pUnderScore = _tcschr(pMacName, _T('_'));
				int index = pUnderScore != (TCHAR *)NULL ? (int)(pUnderScore - pMacName) : -1;

				// found the tool prefix?
				CProjType *	pprojtype = GetProjType();

				TCHAR * pchPrefix;
				if (index == -1)
				{
					// we'll have to default to 'UNK' (unknown)
					pchPrefix = _TEXT("UNK");
					index = 3;
				}
				else
				{
					// we'll get the tool for the tool-prefix
					pchPrefix = (TCHAR *)pMacName;
				}

				// no tool?
				pTool = pprojtype->GetToolFromCodePrefix(pchPrefix, 3);
				
				if (pTool == (CBuildTool *)NULL)
				{
					// create new projtype-specific 'unknown' tool
					pTool = new CUnknownTool(*pprojtype->GetPlatformName(), pchPrefix);
					pprojtype->AddTool(pTool);
					g_pLastTool = pTool;
				}

				// make sure we have this tool in our action-list if it's unknown
				if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)) &&
					!pActions->Find(pTool)
				   )
				{
					// explicitly add an action for this tool
					CBuildAction * pAction;
					VSASSERT(GetActiveConfig(), "No active config?  Project must not be set up properly yet");
					pAction = new CBuildAction(this, pTool, FALSE, GetActiveConfig());
					pActions->AddTail(pAction);
				}
			}

			CConfigurationRecord * pcrDest;

			// We're going to want this across all configs
			if (!fPerConfig)
				CreateAllConfigRecords();	// make sure we have all of the configs...

			// FUTURE: handle more than one tool on this source file
			VSASSERT(pActions->GetCount() == 1, "Can only have one tool on a source file at a time");	  
			CBuildAction * pAction = (CBuildAction *)pActions->GetHead();

			// Parse the dependants list to get our file name and dependents:

			// This is the accepted way to get a mungeable pointer a CString's
			// data.  Note we never call ReleaseBuffer:
			TCHAR * pBase = pMacro->m_strValue.GetBuffer (1);	

			// Squirrel away the length of the deps string, since we'll be 
			// writing all over it:
			const TCHAR * pDepEnd =  (const TCHAR *)pMacro->m_strValue + pMacro->m_strValue.GetLength();
	
			// possible token start and end
			TCHAR * pTokenEnd = max(pBase, (TCHAR *) pDepEnd-1);
			
			// generic buffer	
			CString str;

			for ( ; pTokenEnd < pDepEnd; pBase = _tcsinc (pTokenEnd) )
			{
				// skip whitespace
		  		SkipWhite (pBase);

				TCHAR * pTokenStart = pBase;

				// skip lead-quote?
				if (pBase[0] == _T('"'))
					pBase++;

				// get .mak token
				GetMakToken(pTokenStart, pTokenEnd);

				// back-up over end-quote?
				if (*(pTokenEnd - 1) == _T('"'))
					*(pTokenEnd - 1) = _T('\0');
				else
					*pTokenEnd = _T('\0'); 

				// get our dependency from this string
				BOOL fDepIsPerConfig = TRUE;	// we want the first one, so pretend per-config

				// per-configuration
				if (fPerConfig)
				{
					// Add user-defined dep to configuration record.
					CString strDeps;
					if (invalid == GetStrProp(P_UserDefinedDeps, strDeps) )
						strDeps.Empty();
					strDeps += pBase;
					strDeps += _T("\r\n");
					SetStrProp(P_UserDefinedDeps, strDeps);
				}
				else
				{
					INT_PTR nSize = m_ConfigArray.GetSize();
					
					CProjTempConfigChange projTempConfigChange(GetProject());

					// add this dep. to all configs.
					for (int nConfig = 0; nConfig < nSize; nConfig++)
					{
						pcrDest = (CConfigurationRecord *)m_ConfigArray[nConfig];

						// Get dep list to copy to
						projTempConfigChange.ChangeConfig((CConfigurationRecord *)pcrDest->m_pBaseRecord);

						// Should we do this?
						VSASSERT(GetActiveConfig(), "No active config?  Project must not be set up properly yet");
						if( !GetTarget() )
							continue;

						if( !GetTarget()->GetActiveConfig() || !GetTarget()->GetActiveConfig()->IsValid() )
							continue;

						CBuildActionList * pActions = GetActiveConfig()->GetActionList();
						if (!pActions->IsEmpty())
						{
							// FUTURE: handle more than one tool on this source file
							VSASSERT(pActions->GetCount() == 1, "Only one tool at a time can be supported for a file");	  
							CBuildAction * pAction = (CBuildAction *)pActions->GetHead();

							// Add user-defined dep to configuration record.
							CString strDeps;
							if (invalid == GetStrProp(P_UserDefinedDeps, strDeps) )
								strDeps.Empty();
							strDeps += pBase;
							strDeps += _T("\r\n");
							SetStrProp(P_UserDefinedDeps, strDeps);
						}
					}
				}
			}
		}
		else if (_tcsnicmp(_TEXT("NODEP_"), pMacName, 6) == 0 ||	// missing dependency
			_tcsnicmp(_TEXT("DEP_"), pMacName, 4) == 0)			// scanned dependency
		{
			// specifically want to just skip these
		}
		// tool macro or some tool option macro?
		else
		{
			// probably a tool definition or tool macro, or some random macro
			CBuildTool * pTool = NULL;
			int index = pMacro->m_strName.Find(_T('_'));
			INT_PTR i = 0, nMax = fPerConfig ? 1 : GetPropBagCount();

			// tool macro?
			if (index == -1 && IsKindOf(RUNTIME_CLASS(CProject)))
			{
				for (;;) // iterate through configs if necessary based on m_cp
				{
					if (!fPerConfig)
						ForceConfigActive((CConfigurationRecord *)m_ConfigArray[i]);

					// tool definition; extract tool name
					CProjType * pprojtype = GetProjType();
					pTool = pprojtype->GetToolFromCodePrefix( pMacName, lstrlen(pMacName) );
					if ( pTool == NULL )
					{
						// create new projtype-specific 'unknown' tool
						pTool = new CUnknownTool(*pprojtype->GetPlatformName(),
										pMacro->m_strName, pMacro->m_strValue);

						pprojtype->AddTool(pTool);
						g_pLastTool = pTool;
					}

					// repeat for next config, if necessary
					if (++i >= nMax)
						break;
				}
			}
	 		else if (index != -1)
			{
				for (;;) // iterate through configs if necessary based on m_cp
				{
					if (!fPerConfig)
						ForceConfigActive((CConfigurationRecord *)m_ConfigArray[i]);

					if ((pTool = GetProjType()->GetToolFromCodePrefix(pMacName, index)) != NULL)
					{
						g_pLastTool = pTool;	// save most recently used tool
					}
					else
					{
						pTool = g_pLastTool;	// worst-case default
					}

					// found macro for some tool?
					if (pTool)
					{
						VSASSERT(GetActiveConfig(), "No active config?  Project must not be set up properly yet");
						CBuildActionList * pActions = GetActiveConfig()->GetActionList();

						// make sure we have this tool in our action-list if it's unknown
						if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)) &&
							!pActions->Find(pTool)
						   )
						{
							// explicitly add an action for this tool
							CBuildAction * pAction;
							pAction = new CBuildAction(this, pTool, FALSE, GetActiveConfig());
							pActions->AddTail(pAction);
						}

						if (!pTool->ReadToolMacro(pMacro, this))
							AfxThrowFileException(CFileException::generic);
					}

					// repeat for next config, if necessary
					if (++i >= nMax)
						break;
				}
			}
			else if( (_tcsicmp(pMacName,"BuildCmds") == 0) && (g_pBuildTool != NULL) 
					&& g_pBuildTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)) ){
				// append the whole macro to the unknow prop in the last tool!
				g_pBuildTool->ReadToolMacro(pMacro,this);
			}
		}
	}
	CATCH (CException, e)
	{
		retVal = FALSE;	// failure
	}
	END_CATCH

	ForceConfigActive();

	return retVal;	// success?
}
///////////////////////////////////////////////////////////////////////////////

BOOL CProjItem::IsAddSubtractComment (CObject *&pObject)
{
	if ( !pObject->IsKindOf ( RUNTIME_CLASS (CMakComment))) return FALSE; 
	TCHAR *pc, *pcOld=NULL;
	CBuildTool *pbt;
	BOOL bRetval = FALSE, bSubtract;
	BOOL bBase = FALSE;
	int i, nRepCount = 1;

	pc = ((CMakComment *) pObject)->m_strText.GetBuffer (1);
	SkipWhite (pc);

	// Look for the ADD or SUBTRACT prefix:
	if (  _tcsnicmp(pcADD, pc, 4) == 0 ) 
	{
		bSubtract = FALSE;
		pc += 4;
	}
	else if	(  _tcsnicmp(pcSUBTRACT, pc, 9) == 0 ) 
	{
		bSubtract = TRUE;
		pc += 9;
	}
	else return FALSE;

	SkipWhite (pc);

	// Check for BASE flag
	if (!_tcsnicmp(pcBASE, pc, 5))
	{
		bBase = TRUE;
		pc += 5;	// skip past base flag
		SkipWhite (pc);
	}
	
	// global across *all configs* so
	// make sure we have all of our configs matching the project
	CreateAllConfigRecords();

	if (m_cp == EndOfConfigs)
	{
		// FUTURE (karlsi): this is not very efficient, since we have to
		// reparse the same line n times.  Ideally, we could
		// defer this to when we set the properties, except
		// different configs might have different tools, and it
		// might get tricky. Reevaluate for V5.

		nRepCount = GetPropBagCount();
		pcOld = pc;	// save char pointer for each iteration
	}

	for (i = 0; i < nRepCount; i++)
	{
		if (m_cp == EndOfConfigs)
		{
			pc = pcOld;	// restore pointer to tool code-prefix

			// force config active
			ForceConfigActive((CConfigurationRecord *)m_ConfigArray[i]);
		}

		// Ask the current project type which one of its tools
		// matches the tool code-prefix.
		CProjType * pprojtype = GetProjType(); 
		g_pBuildTool = pbt = pprojtype->GetToolFromCodePrefix(pc, lstrlen (pc));
		

		if (pbt != NULL)
		{
			// Skip to start of flags to end of string
			while (*pc != _T('\0') && !_istspace(*pc))
				pc = _tcsinc(pc);	// next MBC character
	
			bRetval = pbt->ProcessAddSubtractString(this, pc, bSubtract, bBase);

			// FUTURE (karlsi): Do we want to allow/ignore failures
			//         when m_cp == EndOfConfigs?
			if (!bRetval)
			{
				// reset config.?
				if (m_cp == EndOfConfigs)
					ForceConfigActive();

				break; // quit if fails for any Config
			}
		}

		// reset config.?
		if (m_cp == EndOfConfigs)
			ForceConfigActive();
	}

	delete pObject;	pObject = NULL;

	// Something went wrong, but this token was for us:
	if (!bRetval)
	{
		VSASSERT(GetProject(), "Something went wrong in project load");
		if (GetProject() && (!GetProject()->m_bProjConverted))
			AfxThrowFileException (CFileException::generic);
	}
	return TRUE;	

}

///////////////////////////////////////////////////////////////////////////////
// (throw CFileException if unknown confiugration)
void CProjItem::ReadConfigurationIfDef(CMakDirective *pmd, BOOL bCheckProj /* = FALSE */)
{
	// N.B. FUTURE (karlsi): Currently we don't handle nested switches
	TCHAR *pstart, *pend;

	if (pmd->m_dtyp == CMakDirective::DTYP_ENDIF)
	{
		if (bCheckProj)
		{
			VSASSERT(IsKindOf(RUNTIME_CLASS(CProject)), "Should only be calling this part of ReadConfigurationIfDef if we're a project");
			CString strProject;
			CProject * pProject;
			CProject::InitProjectEnum();
			while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL)
			{
				pProject->m_cp = EndOfConfigs;
			}
		}
		else
		{
			m_cp = EndOfConfigs;
		}
	}
	else if (pmd->m_dtyp == CMakDirective::DTYP_ELSEIF ||
			 pmd->m_dtyp == CMakDirective::DTYP_IF)
	{
		// Skip over "$(CFG)" part
		pstart	= pmd->m_strRemOfLine.GetBuffer(1);
		if (!GetQuotedString (pstart, pend) ||
			_tcsncmp ( _TEXT("$(CFG)"), pstart, 6) != 0)
			AfxThrowFileException (CFileException::generic);

		pstart =_tcsinc(pend);
		if (!GetQuotedString(pstart, pend))
			AfxThrowFileException (CFileException::generic);

		*pend = _T('\0');

		// put the whole project into this configuration
		if (bCheckProj)
		{
			VSASSERT(IsKindOf(RUNTIME_CLASS(CProject)), "Should only be calling this part of ReadConfigurationIfDef if we're a project");
			CString strProject;
			BOOL bFoundProject = FALSE;
			CProject * pProject = (CProject *)this;
			CProject::InitProjectEnum();
			while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL)
			{
				if (!bFoundProject)
				{
					int i;
					CConfigurationRecord* pcr;
					int size = pProject->GetPropBagCount();
					for (i=0; i < size; i++)
					{
						pcr = (CConfigurationRecord*) pProject->GetConfigArray()->GetAt(i);
						VSASSERT(pcr, "NULL config record in config array!");
						if (pcr->GetConfigurationName().CompareNoCase(pstart) == 0)
						{
							bFoundProject = TRUE;
							g_pActiveProject = pProject;
							break;
						}
					}
				}
				pProject->m_cp = (pmd->m_dtyp == CMakDirective::DTYP_IF) ? 
						FirstConfig : MiddleConfig;									 
			}

			VSASSERT(bFoundProject, "No project?!?");
			if (!bFoundProject)
				AfxThrowFileException (CFileException::generic);

			g_pActiveProject->SetActiveConfig(pstart);
		}
		else	
		{
			GetProject()->SetActiveConfig(pstart);
			m_cp = (pmd->m_dtyp == CMakDirective::DTYP_IF) ? 
					FirstConfig : MiddleConfig;									 
		}

	}
}

///////////////////////////////////////////////////////////////////////////////
void CProjItem::InformDependants(UINT idChange, DWORD_PTR dwHint)
{
	CSlob::InformDependants(idChange, dwHint);

	//
	// Inform other non-CSlob, dependents here ....
	//
	CProject * pProject = GetProject();

	if (idChange == P_ProjUseMFC &&	pProject->m_bProjectComplete)
	{
		// we need to make sure we have the right setup
		// for an MFC project
		CProjType * pprojtype = pProject->GetProjType();
		int val;
		if (pprojtype != (CProjType *)NULL &&
			pProject->GetIntProp(P_ProjUseMFC, val)
		   )
		{
			pprojtype->PerformSettingsWizard(this, val);
		}
	}

	// 
	// Inform other, non-CSlob, dependents here ....
	//
	if (m_idBagInUse == CurrBag)
	{
		// inform any associated actions so that they may add/remove inputs and outputs?
		// or that the file may become a part of the build
		if (idChange == P_ItemExcludedFromBuild)
		{
		}
  		
		// inform our actions that their output state is likely to have changed?
 		// (many output files are derived from the output directories and/or remote target)
		else if (idChange == P_OutDirs_Target || idChange == P_OutDirs_Intermediate ||
				 idChange == P_TargetName || idChange == P_RemoteTarget)
		{
			CProject * pProject = GetProject();
			// only inform if the project is complete, ie. not constructing/destructing
			if (pProject != (CProject *)NULL && pProject->m_bProjectComplete)
			{
				// FUTURE:
				// this is a hack for project-level custom-build steps
				// informing that the inputs (.objs) to the .exe has changed
				// will remove the .exe and then re-create, we need
				// to re-affix the project-level custom-build step thus...
				CBuildTool * pTool;
				g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_CustomBuild), (CBldSysCmp *&)pTool);

				CVCPtrList lstSelectTools;
				lstSelectTools.AddTail(pTool);

				// Special build tool to handle pre-link/post-build.
				g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_SpecialBuild), (CBldSysCmp *&)pTool);
				lstSelectTools.AddTail(pTool);

				// un-assign any project-level custom build tool
				CBuildAction::UnAssignActions(pProject, &lstSelectTools, (CConfigurationRecord  *)NULL, FALSE);

				// try to re-assign any project-level custom build tool
				CBuildAction::AssignActions(pProject, &lstSelectTools, (CConfigurationRecord  *)NULL, FALSE);

			}
		}

		// configuration name changes may need to be forwarded to targets/target references displaying
		// the configuration name that has changed
		else if (idChange == P_ProjConfiguration && GetProject() && GetProject()->m_bProjectComplete)
		{
			CString strConfig;
			GPT gptVal = GetStrProp(P_ProjConfiguration, strConfig);
			VSASSERT(gptVal == valid, "P_ProjConfiguration is required property!");
		}

		// break else here... to test for SN_ALL

		// re-assign custom build actions changes
		if (idChange == P_CustomCommand || idChange == P_CustomOutputSpec ||	// semantic change
			idChange == P_UserDefinedDeps ||

			idChange == SN_ALL ||												// global change

			idChange == P_TargetName ||											// target name (binding)
			idChange == P_OutDirs_Target ||										// target output (binding)
			idChange == P_PreLink_Command ||
			idChange == P_PostBuild_Command ||
			idChange == P_RemoteTarget											// target output (binding)
		   )
		{
			CProject * pProject = GetProject();

			if (pProject != (CProject *)NULL && pProject->m_bProjectComplete)	// project complete
			{
				CBuildTool * pTool = NULL;
				g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_CustomBuild), (CBldSysCmp *&)pTool);

				CVCPtrList lstSelectTools;
				lstSelectTools.AddTail(pTool);

				// Special build tool to handle pre-link/post-build.
				g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_SpecialBuild), (CBldSysCmp *&)pTool);
				lstSelectTools.AddTail(pTool);

				// un-assign any custom build tool
				CBuildAction::UnAssignActions(this, &lstSelectTools, (CConfigurationRecord  *)NULL, FALSE);

				// try to re-assign a custom build tool
				CBuildAction::AssignActions(this, &lstSelectTools, (CConfigurationRecord  *)NULL, FALSE);

				// possible target-name change
				if (idChange == P_TargetName)
				{
					CString strConfig;
					GPT gptVal = GetStrProp(P_ProjConfiguration, strConfig);
					VSASSERT(gptVal == valid, "P_ProjConfiguration is required property!");
				}
			}
		}

		// break else here... to test for SN_ALL

		// global action changes
 		// (SN_ALL or builder filename change)
		if (idChange == SN_ALL ||				// global change
			idChange == P_BldrFileName ||
			idChange == P_Proj_TargDefExt		// default target extension
		   )
		{
			COptionHandler * pOptHdlr;
			CVCPtrList lstTool;	 

			g_pPrjcompmgr->InitOptHdlrEnum();
			while (g_pPrjcompmgr->NextOptHdlr(pOptHdlr))
			{
				CBuildTool * pTool = pOptHdlr->m_pAssociatedBuildTool;

				// affects the outputs?
				if (pTool != (CBuildTool *)NULL && (idChange == SN_ALL))
					lstTool.AddTail(pTool);
			}

			VCPOSITION pos = lstTool.GetHeadPosition();
			while (pos != (VCPOSITION)NULL)
				// local, only affects outputs of those actions interested in tool
				((CBuildTool *)lstTool.GetNext(pos))->InformDependants(P_ToolOutput, (DWORD_PTR)this);
		}
	}

	if (idChange == P_ItemIgnoreDefaultTool)
	{
		VSASSERT(IsKindOf(RUNTIME_CLASS(CFileItem)), "Can only ignore default tool on files");
		if (GetProject() && GetProject()->m_bProjectComplete)
		{
			CConfigurationRecord * pcr = (CConfigurationRecord *)GetActiveConfig()->m_pBaseRecord;
			CBuildAction::UnAssignActions(this, NULL, pcr);
			CBuildAction::AssignActions(this, NULL, pcr);
		}
	}
}

CSlob * CProjItem::GetContainerInSameConfig()
{
	CProjItem * pItem = (CProjItem *)GetContainer();
	if (pItem != (CSlob *)NULL)
	{
		// if we have a manual than our containe uses the same manual,
		// else it uses our current if it needs to

		CConfigurationRecord * pRec = (CConfigurationRecord *)NULL;

		// use the same manual config. if we have one
		if (m_pManualConfigRec != (CConfigurationRecord *)NULL)
			pRec = m_pManualConfigRec;

		// don't do anything if we don't need to
		else
		{
			VSASSERT(GetActiveConfig(), "No active config?  Something not initialized properly");
			if (pItem->GetActiveConfig(TRUE)->m_pBaseRecord != GetActiveConfig()->m_pBaseRecord)
				pRec = m_pActiveConfig;
		}

		pItem->SetManualBagSearchConfig(pRec);
	}
	return pItem;
}

void CProjItem::SetCurrentConfigAsBase(COptionHandler * popthdlr)
{
	CConfigurationRecord * pRec = GetActiveConfig();
	VSASSERT(pRec != (CConfigurationRecord *)NULL, "No active config?  Item wasn't initialized properly");

	while (popthdlr != (COptionHandler *)NULL)
	{
		UINT nMinProp, nMaxProp ;
		popthdlr->GetMinMaxOptProps (nMinProp, nMaxProp) ;
		pRec->BagCopy(CurrBag, BaseBag, nMinProp, nMaxProp);
		// get the base-handler
		popthdlr = popthdlr->GetBaseOptionHandler();
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetPropBag
////////////////////////////////////////////////////////////////////////////////
int CProjItem::GetPropBagCount() const
{
	const CVCPtrArray * pCfgArray = GetConfigArray();
	return (int) pCfgArray->GetSize();
}

CPropBag* CProjItem::GetPropBag(int nBag)
{
	// look for the *current* property bag?
	if (nBag == -1) /* get current */
	{
		CConfigurationRecord *pcr;
		pcr = GetActiveConfig(TRUE);
		if (pcr)
		{
			return (pcr->GetPropBag(m_idBagInUse));	// our prop bag for current active config
		}
		return(NULL);
	}

	// get a particular property bag (using index)
	CConfigurationRecord *pcr;
	VSASSERT(nBag >= 0, "No prop bag in use!");
	if (nBag > m_ConfigArray.GetUpperBound())
		return (CPropBag *)NULL ;

	pcr = (CConfigurationRecord *)m_ConfigArray[nBag];
	return pcr->GetPropBag(m_idBagInUse);
}

CPropBag * CProjItem::GetPropBag(const CString & strBagName)
{
	CConfigurationRecord * pcr = ConfigRecordFromConfigName(strBagName); 
	return pcr != (CConfigurationRecord *)NULL ? pcr->GetPropBag(m_idBagInUse) : (CPropBag *)NULL;
};
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::ConfigurationChanged(const CConfigurationRecord *pcr )
{
	POSITION pos;
#ifdef _DEBUG
	void * pv;
#endif
	BOOL retVal = FALSE; // return true if and only if "new" cr added
	CProjItem *	pProjItem;
	CConfigurationRecord * pcrNew;
	CString strType, strName;

	VSASSERT (pcr, "Cannot pass NULL to ConfigurationChanged");
	VSASSERT (pcr->m_pBaseRecord == pcr, "Can only pass top level config record to ConfigurationChanged" ); // pcr should be top level!

	// FUTURE (karlsi): Change subproject and warn if there's state if they don't
	// have a matching config.

	// This is where items other than projects find out that there's a new
	// config:  They're told to switch to a config they don't know about,
	// so they create it:
	if (!m_ConfigMap.Lookup((void *) pcr, (void *&) m_pActiveConfig))
	{
		strType = pcr->GetOriginalTypeName();
		strName = pcr->GetConfigurationName();
		pcrNew = ConfigRecordFromConfigName(strName, FALSE, FALSE);
		if ((pcrNew) && (pcrNew->GetOriginalTypeName()==strType))
		{
			// matched by name & projtype, so reuse this one
			m_pActiveConfig = pcrNew;
			m_ConfigMap.RemoveKey((void*)m_pActiveConfig->m_pBaseRecord); // readded for new base below
			m_pActiveConfig->m_pBaseRecord = pcr->m_pBaseRecord;
		}
		else
		{
			m_pActiveConfig = new CConfigurationRecord (pcr, this);
			INT_PTR x = m_ConfigArray.Add(m_pActiveConfig);
			VSASSERT(((CConfigurationRecord *)m_ConfigArray[x])->GetConfigurationName()==pcr->GetConfigurationName(), "Name mismatch on config!");
			retVal = TRUE;
		}
		VSASSERT(!m_ConfigMap.Lookup((void *)m_pActiveConfig->m_pBaseRecord, (void *&)pv), "Config record not in config map!");
		m_ConfigMap.SetAt((void *)m_pActiveConfig->m_pBaseRecord, m_pActiveConfig);
		VSASSERT(m_ConfigMap.GetCount()==m_ConfigArray.GetSize(), "Config map/array size mismatch!");
	}
	ASSERT_VALID (m_pActiveConfig);

	// notify all of our children of this change
	// don't do this if we don't have a content list
	if (
		!GetContentList()
	   )
		return retVal;

	for (pos = GetHeadPosition(); pos != NULL; )
	{
		pProjItem = (CProjItem *)GetNext(pos);
		BOOL bChildAdded = pProjItem->ConfigurationChanged(pcr);
	}
	return retVal;

}
///////////////////////////////////////////////////////////////////////////////
// make sure we have all of our configs matching the project
void CProjItem::CreateAllConfigRecords()
{
	CTargetItem *pTarget = GetTarget();
	CProject *pProject = GetProject();
	const CVCPtrArray * pCfgArray = NULL;
	BOOL fUseProjectConfig = FALSE;

	if (pTarget != NULL && pTarget != this && pProject != this)
	{
		pCfgArray = pTarget->GetConfigArray();
	}
	else if (pProject != NULL)
	{
		pCfgArray = pProject->GetConfigArray();
		fUseProjectConfig = TRUE;
	}

	if (pCfgArray != NULL)
	{
		INT_PTR icfg, size = pCfgArray->GetSize();
		for (icfg = 0; icfg < size; icfg++)
		{
			CConfigurationRecord * pcr = (CConfigurationRecord *)pCfgArray->GetAt(icfg);
			if (fUseProjectConfig || pcr->IsValid())
				(void)ConfigRecordFromBaseConfig((CConfigurationRecord *)pcr->m_pBaseRecord, TRUE);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
CConfigurationRecord * CProjItem::GetActiveConfig(BOOL bCreate /* = FALSE */)
{
	// return a forced configuration?
	if (m_pForcedConfig != (CConfigurationRecord *) NULL)
		return m_pForcedConfig;

	if (m_pManualConfigRec)
	{
		CConfigurationRecord * pRec;
		if (m_ConfigMap.Lookup( 
				(void  *) m_pManualConfigRec->m_pBaseRecord,
				(void *&) pRec
				))
			return pRec;
	}

	// do we have a matching config. with our owner project?
	// (compare base records)
	CProjItem * pItem = GetProject();
	
	// Is this item in a project, if not then it doesn't have
	// an active config
	if (pItem == NULL)
		return NULL;

	if (pItem != this)
	{
		CConfigurationRecord * pcrItem = pItem->GetActiveConfig(bCreate);
		if (!m_pActiveConfig || pcrItem != m_pActiveConfig->m_pBaseRecord)
		{
			if (pcrItem != NULL)
			{
				// get a config. record, create if needs be
				m_pActiveConfig = ConfigRecordFromBaseConfig(pcrItem, bCreate);
				if (bCreate)
				{
					VSASSERT(m_pActiveConfig, "Active config not set!  Bad initialization");
					ASSERT_VALID(m_pActiveConfig);
				}
			} 
			else
			{
				m_pActiveConfig = NULL;
			}
		}
	}

	// return the active configuration...
	return m_pActiveConfig;
}
///////////////////////////////////////////////////////////////////////////////
CConfigurationRecord* CProjItem::ConfigRecordFromBaseConfig
(
	CConfigurationRecord *	pcrBase,
	BOOL					fCreate /* = FALSE */
)
{
	CConfigurationRecord * pcr;

	// not found matching?
	if (!m_ConfigMap.Lookup((void *)pcrBase, (void *&)pcr))
	{
		// create?
		if (fCreate)
		{
			CString strType = pcrBase->GetOriginalTypeName();
			CString strName = pcrBase->GetConfigurationName();

			pcr = ConfigRecordFromConfigName(strName);
			if (pcr && pcr->GetOriginalTypeName() == strType)
			{
				// matched by name & projtype, so reuse this one
				m_ConfigMap.RemoveKey((void*)pcr->m_pBaseRecord); // readded for new base below
				pcr->m_pBaseRecord = pcrBase;
			}
			else
			{			
				// not  matched, so create a new one
				pcr = new CConfigurationRecord(pcrBase, this);
				INT_PTR ipcr = m_ConfigArray.Add(pcr);
				VSASSERT(((CConfigurationRecord *)m_ConfigArray[ipcr])->GetConfigurationName() == pcrBase->GetConfigurationName(), "Config name mismatch");
			}

			// add this into our config. map (it's already in our array)
#ifdef _DEBUG
			void * pv;
#endif // _DEBUG
			VSASSERT(!m_ConfigMap.Lookup((void *)pcr->m_pBaseRecord, (void *&)pv), "Config record not in config map!");
			m_ConfigMap.SetAt((void *)pcr->m_pBaseRecord, pcr);

			VSASSERT(m_ConfigMap.GetCount() == m_ConfigArray.GetSize(), "Config map/array size mismatch");
		}
		else
			pcr = (CConfigurationRecord *)NULL;
	}

	return pcr;
}
///////////////////////////////////////////////////////////////////////////////
CConfigurationRecord *CProjItem::ConfigRecordFromConfigName
(
	const TCHAR *	pszConfig,
	BOOL			fCreate /* = FALSE */,
	BOOL fMatchExact /* = TRUE */
)
{
	CString strConfigToMatch, strConfig;
	TCHAR * pszChar;
	CConfigurationRecord * pcrMatch = NULL;
	if (!fMatchExact)
	{
		TCHAR * pszChar = _tcsstr(pszConfig, _T(" - "));
		if (pszChar != NULL)
		{
			strConfigToMatch = pszChar + 3;
		}
		else
		{
			fMatchExact = TRUE;
		}
	}
	int size = GetPropBagCount();
	for (int i = 0; i < size; i++)
	{
		CConfigurationRecord * pcr = (CConfigurationRecord *)m_ConfigArray[i];
		strConfig = pcr->GetConfigurationName();
		if (strConfig.CompareNoCase(pszConfig) == 0)
			return pcr;

		if ((!fMatchExact) && (pcrMatch == NULL))
		{
			pszChar = _tcsstr(strConfig, _T(" - "));
			if ((pszChar != NULL) && (strConfigToMatch.CompareNoCase(pszChar + 3) == 0))
			{
				// found possible match
				pcrMatch = pcr;
			}
		}
	}

	if (pcrMatch != NULL)
		return pcrMatch;

	// not found, so create?
	if (fCreate)
	{
		// must have one in project, so create one based on this project's config.
		return ConfigRecordFromBaseConfig(GetProject()->ConfigRecordFromConfigName(pszConfig), TRUE);
	}

	return NULL;
}

CProjType * CProjItem::GetProjType ()
{
	CProject * pProject = GetProject();
	VSASSERT(pProject != (CProject *)NULL, "Project not set!");
	
	CConfigurationRecord * pcr = GetActiveConfig();
	if (pcr == (CConfigurationRecord *)NULL)
		return (CProjType *)NULL;	// no current configuration -> no project type!

	VSASSERT(pcr->m_pBaseRecord, "Config record not initialized properly");

	CStringProp * pProp = (CStringProp *) ((CConfigurationRecord *)pcr->m_pBaseRecord)
						 ->GetPropBag(CurrBag)->FindProp (P_ProjOriginalType);
	if (pProp==NULL)
		return NULL;

	VSASSERT(pProp->m_nType == string, "Expected string prop!"); 

	// ignore return
	CProjType * pprojtype;
	if (!g_pPrjcompmgr->LookupProjTypeByName(pProp->m_strVal, pprojtype))
		return (CProjType *)NULL;

	return pprojtype;
}
///////////////////////////////////////////////////////////////////////////////
void CProjItem::FlattenSubtree (CObList &rObList, int fo) 
{
 	BOOL fAddItem = FALSE, fAddContent = FALSE;

	FlattenQuery(fo, fAddContent, fAddItem);

	// If we have items contained then flatten each of these as well
	if (fAddContent && GetContentList())
	{
		POSITION pos;
 		for (pos = GetHeadPosition(); pos != (POSITION)NULL;)
		{
			CProjItem * pProjItem = (CProjItem *) GetNext (pos);
			if (!(fo & flt_ExpandSubprojects) && pProjItem->IsKindOf(RUNTIME_CLASS(CProject)))
				continue;

			pProjItem->FlattenSubtree(rObList, fo);
		}
	}

	if ((fo & flt_OnlyTargRefs) && !IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		return;	// only include target references!

	// Add the item if we need to
	if (fAddItem)
		rObList.AddTail(this);
}

void CProjItem::FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem)
{
	fAddContent = FALSE;
	fAddItem = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// CanContain - our internal (within VPROJ) CanAdd method
 
BOOL CProjItem::CanContain(CSlob* pSlob)
{
	// Must override
	VSASSERT(FALSE, "CanContain must be overridden");
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
GPT CProjItem::GetIntProp(UINT idProp, int& val)
{
//#pragma message ("ALERT! : projitem.cpp : Including P_QA_ProjItemType hook for QA...")
	// return to QA what the type of this project item is
	if (idProp == P_QA_ProjItemType)
	{
		if (IsKindOf(RUNTIME_CLASS(CProject)))
			val = QA_TypeIs_Target;
		else if (IsKindOf(RUNTIME_CLASS(CProjGroup)))
			val = QA_TypeIs_Group;
		else if (IsKindOf(RUNTIME_CLASS(CFileItem)))
			val = QA_TypeIs_File;
		else 
			VSASSERT(FALSE, "unrecognized type!");

		return valid;	// success
	}

	GPT gpt;
	// want to handle special case here:
	if (idProp == P_ItemOptState || idProp == P_ItemOptChanged)
	{
		if (IsKindOf (RUNTIME_CLASS (CProject)) || IsKindOf (RUNTIME_CLASS (CFileItem)) || 
			IsKindOf (RUNTIME_CLASS (CProjGroup)) || IsKindOf (RUNTIME_CLASS (CTargetItem)))
		{
			gpt = CSlob::GetIntProp (P_ItemOptState, val);
			if (gpt == invalid)
				val = 1;
			else 
			{
				if (idProp == P_ItemOptState)
					val = val & 0x7fffffff; // masking off the highest bit
				else
					val = (val & 0x80000000) ? 1 : 0; // get the highest bit
			}
			return valid;
		}
		else
			return invalid;
	}

	if (idProp == P_ItemExcludedFromScan)
	{
		// not per config and not valid by default
		// override to do anything special
		val = FALSE;
		return invalid;
	}

	if ((idProp == P_Proj_PerConfigDeps) && (GetProject() != NULL))
	{
		val = GetProject()->m_bProj_PerConfigDeps;
		return valid;
	}

	// get the option handler
	COptionHandler * popthdlr = g_pPrjcompmgr->FindOptionHandler(idProp);

	if (popthdlr != (COptionHandler *)NULL)
	{
		// if we have an option handler for this prop we must be an option
		// check it's dependencies
		if (!(m_optbeh & OBNoDepCheck))
		{
			BOOL fCheckDepOK;

			// the DepCheck needs to view the whole 'context' ie.
			// the tool options with inheritance
			// prevent the DepCheck() recursing
			OptBehaviour optbehOld = m_optbeh;
			m_optbeh |= OBInherit | OBShowDefault | OBNoDepCheck;
		
			// make sure the option handling refers to us!
			popthdlr->SetSlob(this);
			fCheckDepOK = popthdlr->CheckDepOK(idProp);
			popthdlr->ResetSlob();

			m_optbeh = optbehOld;

			// is this an invalid option?
			if (!fCheckDepOK)	return invalid;
		}
	}

	// usual CSlob property behaviour
	// ask the base-class to actual find the prop.
	gpt = CSlob::GetIntProp(idProp, val);

	// do we want to supply this ourselves?
	if (gpt == invalid && (idProp == P_ItemExcludedFromBuild))
	{
		val = FALSE; gpt = valid;	// valid and FALSE by default
	}

	// does this property exist for this CSlob?
	if (gpt == invalid)
	{
		// are we not at the root and inheriting?
		if (m_optbeh & OBInherit)
		{
			// automatically detect if we need to do a manual config?
			// we don't need to if we already have a manual config rec. to use
			CProjItem * pContainer = (CProjItem *) GetContainerInSameConfig();
			while (pContainer != (CSlob *)NULL)
			{
				// yes, so let's search in our container CSlob if we have one
				OptBehaviour optbehOld = pContainer->m_optbeh;
 				int idOldBag = pContainer->UsePropertyBag(UsePropertyBag());

				// make them use our option behaviour, but return any context sensitive
				// information such as defaults .. we'll do the inheritance chaining here!
				pContainer->m_optbeh = (OptBehaviour)((m_optbeh | OBBadContext | OBNoDepCheck) & ~OBInherit);

				GPT gptContainer = pContainer->GetIntProp(idProp, val);

				pContainer->m_optbeh = optbehOld;
 	   			(void) pContainer->UsePropertyBag(idOldBag);

				if (gptContainer == valid)
				{
					pContainer->ResetContainerConfig();
					return valid;
				}

				// chain our config.
				// look in the next container
				CProjItem * pNextContainer = (CProjItem *) pContainer->GetContainerInSameConfig();
				pContainer->ResetContainerConfig();
				pContainer = pNextContainer;
			}
		}

		//
		// *** base-case ***
		//
		// right context?
		if (!(m_optbeh & OBBadContext))
		{
			if (popthdlr != (COptionHandler *)NULL)
			{
				// make sure the option handling refers to us!
				popthdlr->SetSlob(this);

				// we need to find this in our default property map
				// ask our helper which map it is in

				// always show fake props, if inheriting
				if (((m_optbeh & OBInherit) && popthdlr->IsFakeProp(idProp)) ||
					(m_optbeh & OBShowDefault))
				{
					OptBehaviour optbehOld = m_optbeh;

					m_optbeh |= OBShowDefault;
					gpt = popthdlr->GetDefIntProp(idProp, val);
					m_optbeh = optbehOld;
				}
				
				// reset the option handler CSlob
				popthdlr->ResetSlob();
			}
		}
	}

 	// mustn't be OBAnti, the option handler takes care of this
	VSASSERT((m_optbeh & OBAnti) == 0, "Bad state for m_optbeh.  Option handler should handle this case.");

	return gpt;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SetIntProp(UINT idProp, int val)
{
	BOOL fOk;

	if ((idProp == P_Proj_PerConfigDeps) && (GetProject() != NULL))
	{
		GetProject()->m_bProj_PerConfigDeps = val;
	}

	// mustn't be OBAnti, the option handler takes care of this for integer props
	VSASSERT((m_optbeh & OBAnti) == 0, "Mustn't be OBAnti.  Those belong to option handler");

	// need to do special handling here
	int valTemp = 0;
	GPT gpt;
	if (idProp == P_ItemOptState)
	{
		gpt = GetIntProp (P_ItemOptChanged, valTemp);
		if (gpt == valid)
		{
			val = valTemp ? 0x80000000 | val : val; // set the high bit if valTemp is set
			fOk = CSlob::SetIntProp(P_ItemOptState, val);
		}
		else
		{
			VSASSERT (FALSE, "Invalid value for P_ItemOptState!");
			return FALSE;
		}
	}
	else if (idProp == P_ItemOptChanged)
	{
		gpt = GetIntProp (P_ItemOptState, valTemp);
		if (gpt == valid)
		{
			val = val ? 0x80000000 | valTemp : valTemp;
			fOk = CSlob::SetIntProp(P_ItemOptState, val);
		}
		else
		{
			VSASSERT (FALSE, "Invalid value for P_ItemOptState!");
			return FALSE;
		}
	}
	else
	{
		fOk = CSlob::SetIntProp(idProp, val);
	}

	if (fOk && idProp == P_ItemExcludedFromBuild)
	{
		// FUTURE: Do THIS?
		// Finally, we want to mark the item's project to suggest
		// a relink.  Do this by specifying the last build flags
		// string to be empty:
		CProject * pProject = GetProject();
		VSASSERT(pProject != (CProject *)NULL, "Project not set!  Item in bad state.");

		// broadcast to our option handlers
		COptionHandler * popthdlr;
		g_pPrjcompmgr->InitOptHdlrEnum();
		while (g_pPrjcompmgr->NextOptHdlr(popthdlr))
		{
			// inform of this option change
			// make sure the option handling refers to us!
			popthdlr->SetSlob(this);
			popthdlr->OnOptionIntPropChange(P_ItemExcludedFromBuild, val);

			// reset the option handler CSlob
			popthdlr->ResetSlob();
		}
	}

	// get the option handler for the prop
	COptionHandler * popthdlr;
	if (fOk && ((popthdlr = g_pPrjcompmgr->FindOptionHandler(idProp)) != (COptionHandler *)NULL))
	{
		// inform of this option change
		// make sure the option handling refers to us!
		popthdlr->SetSlob(this);
		popthdlr->OnOptionIntPropChange(idProp, val);

		// reset the option handler CSlob
		popthdlr->ResetSlob();
	}

	return fOk;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SetStrProp(UINT idProp, const CString & str)
{
	// get the option handler for this prop
	COptionHandler * popthdlr = g_pPrjcompmgr->FindOptionHandler(idProp);

	BOOL fIsListStrProp = (popthdlr != (COptionHandler *)NULL) && popthdlr->IsListStrProp(idProp);

	// only string lists and unk. opt. hdlr strings are available as anti-options
	VSASSERT ((m_optbeh & OBAnti) == 0 || fIsListStrProp || popthdlr->IsKindOf(RUNTIME_CLASS(COptHdlrUnknown)), "Only string lists and unk. opt. hdlr strings are available as anti-options!");

	if (popthdlr != (COptionHandler *)NULL)
	{
		// do we to do a type conversion ourselves?
		// ie. does the CSlob *not* support this tool option type?
		if (popthdlr->NeedsSubtypeConversion(idProp))
		{	
			// make sure the option handling refers to us!
			popthdlr->SetSlob(this);

			int nVal;

			BOOL fRet;

			// is this the empty string? if so we want
			// to nuke it from our property bag
			if (str.IsEmpty())
			{
				CPropBag * pPropBag = GetPropBag();
				VSASSERT(pPropBag != (CPropBag *)NULL, "Failed to get property bag!");

				pPropBag->RemoveProp(idProp);
				fRet = TRUE;
			}
			else if (!popthdlr->ConvertFromStr(idProp, str, nVal))
			{
				// no change, inform dependants of this
				InformDependants(idProp);
				fRet = FALSE;
			}
			else
				fRet = SetIntProp(idProp, nVal);

			// reset option handler CSlob
			popthdlr->ResetSlob();
			
			return fRet;
		}
 	}

	// setting the output directories?
	if (idProp == P_OutDirs_Target || idProp == P_OutDirs_Intermediate)
	{
		// if we're setting the output dirs props to an empty string, remove
		// the prop if it exists, only if we're a child that way the removal
		// will reset the prop to inherit from the parent
		if (m_pContainer != (CSlob *)NULL && str.IsEmpty())
		{
			GetPropBag()->RemovePropAndInform(idProp, this);
			return TRUE; 
		}
	}

	BOOL fOk = FALSE;

	// do we have a list of strings for this property?
	if (fIsListStrProp)
	{
		// figure add and subtract string props
		popthdlr->SetListStrProp(this, idProp, (CString &)str, m_optbeh & OBInherit, m_optbeh & OBAnti);
		fOk = TRUE;
	}
	else
	{
		fOk = CSlob::SetStrProp(idProp, str);
	}

	// inform of this option change
	if (fOk && popthdlr != (COptionHandler *)NULL)
	{
		// make sure the option handling refers to us!
		popthdlr->SetSlob(this);
		popthdlr->OnOptionStrPropChange(idProp, str);

		// reset option handler CSlob
		popthdlr->ResetSlob();
	}

	return fOk;
}
///////////////////////////////////////////////////////////////////////////////
GPT CProjItem::GetStrProp(UINT idProp, CString& val)
{
	// fake prop that is actually maintained by the Scc Manager
	if (idProp == P_SccStatus)
	{
		VSASSERT(FALSE, "P_SccStatus should be handled by Scc manager!");
		return invalid;
	}

	if (idProp == P_Title) // alias
		return GetStrProp(P_ProjItemName, val);

	// get the option handler for this prop
	COptionHandler * popthdlr = g_pPrjcompmgr->FindOptionHandler(idProp);

	// are we a string list prop?
	BOOL fIsListStrProp = (popthdlr != (COptionHandler *)NULL) && popthdlr->IsListStrProp(idProp);

	// only string lists and unk. opt. hdlr strings are available as anti-options
	VSASSERT ((m_optbeh & OBAnti) == 0 || fIsListStrProp || popthdlr->IsKindOf(RUNTIME_CLASS(COptHdlrUnknown)), "Only string lists and unk. opt. hdlr strings are available as anti-options");
	
	if (popthdlr != (COptionHandler *)NULL)
	{
		// do we to do a type conversion ourselves?
		// ie. does the CSlob *not* support this tool option type?
		if (popthdlr->NeedsSubtypeConversion(idProp))
		{	
			// make sure the option handling refers to us!
			popthdlr->SetSlob(this);
			GPT gptRet;

			// is this a valid prop?
			int nVal;
			if (GetIntProp(idProp, nVal) != valid)
			{
				gptRet = invalid;	
			}
			else
			{
				// is this a default, ie. not in any bag?
				OptBehaviour optbehOld = m_optbeh;
				m_optbeh &= ~OBShowDefault;
				GPT gpt = GetIntProp(idProp, nVal);
				m_optbeh = optbehOld;

				if (gpt != valid)
				{
					val = "";	// for defaults return the empty string
					gptRet = valid;
				}
				else
					gptRet = popthdlr->ConvertToStr(idProp, nVal, val) ? valid : invalid;		
			}

			// reset the option handler CSlob
			popthdlr->ResetSlob();

			return gptRet;
		}

	 	// if we have a default option map this must be an option
		// check it's dependencies
		if (!(m_optbeh & OBNoDepCheck))
		{
			BOOL fCheckDepOK;

			// the DepCheck needs to view the whole 'context' ie.
			// the tool options with inheritance
			// prevent the DepCheck() recursing
			OptBehaviour optbehOld = m_optbeh;
			m_optbeh |= OBInherit | OBShowDefault | OBNoDepCheck;

			// make sure the option handling refers to us!
			popthdlr->SetSlob(this);
			fCheckDepOK = popthdlr->CheckDepOK(idProp);
			popthdlr->ResetSlob();

			m_optbeh = optbehOld;

			// is this an invalid option?
			if (!fCheckDepOK)	return invalid;
		}
	}

  	GPT gpt = invalid;	// default is the prop. find is invalid

	// do we have a list of strings for this property?
	// if so then we'll want to show this list
	if (fIsListStrProp && !m_fGettingListStr)
	{
		SUBTYPE st = popthdlr->GetDefOptionSubType(idProp);

		// join our directory string lists with ';'
		TCHAR chJoin = st == dirST ? _T(';') : _T(',');

		// show add and/or subtract props combined for this
		m_fGettingListStr = TRUE;
		gpt = popthdlr->GetListStrProp(this, idProp, val, m_optbeh & OBInherit, m_optbeh & OBAnti, chJoin);
		m_fGettingListStr = FALSE;
	}
	else
	{
		// usual CSlob property behaviour
		// ask the base-class to actual find the prop.
		gpt = CSlob::GetStrProp(idProp, val);
	}

	// does this property exist for this CSlob?
	if (gpt == invalid)
	{
		// fake prop for custom build description .. <- no inheritance!
		if ((m_optbeh & OBShowDefault) && idProp == P_CustomDescription)
		{
			// let the new project system handle the name
			return invalid;
		}

		// are we not at the root and inheriting?
		if (m_optbeh & OBInherit)
		{
			// automatically detect if we need to do a manual config?
			// we don't need to if we already have a manual config rec. to use
			CProjItem * pContainer = (CProjItem *) GetContainerInSameConfig();
			while (pContainer != (CSlob *)NULL)
			{
				// yes, so let's search in our container CSlob if we have one
				OptBehaviour optbehOld = pContainer->m_optbeh;
				int idOldBag = pContainer->UsePropertyBag(UsePropertyBag());

				// make them use our option behaviour, but return any context sensitive
				// information such as defaults .. we'll do the inheritance chaining here!
				pContainer->m_optbeh = (OptBehaviour)((m_optbeh | OBBadContext | OBNoDepCheck) & ~OBInherit);

				GPT gptContainer = pContainer->GetStrProp(idProp, val);

				pContainer->m_optbeh = optbehOld;
   				(void) pContainer->UsePropertyBag(idOldBag);

				if (gptContainer == valid)
				{
					pContainer->ResetContainerConfig();
					return valid;
				}

				// chain our config.
				// look in the next container
				CProjItem * pNextContainer = (CProjItem *) pContainer->GetContainerInSameConfig();
				pContainer->ResetContainerConfig();
				pContainer = pNextContainer;
			}
		}

		//
		// *** base-case ***
		//
		// right context?
		if (!(m_optbeh & OBBadContext))
		{
			if (popthdlr != (COptionHandler *)NULL)
			{
				// make sure the option handling refers to us!
				popthdlr->SetSlob(this);

				// we need to find this in our default property map
				// ask our helper which map it is in

				// always show fake props, if inheriting
				if (((m_optbeh & OBInherit) && popthdlr->IsFakeProp(idProp)) ||
					(m_optbeh & OBShowDefault))
				{
					OptBehaviour optbehOld = m_optbeh;

					m_optbeh |= OBShowDefault;
					gpt = popthdlr->GetDefStrProp(idProp, val);
					m_optbeh = optbehOld;
				}

				// reset the option handler CSlob
				popthdlr->ResetSlob();
			}
			// show the defaults for non-tool option props?
			else if (m_optbeh & OBShowDefault)
			{
				// only have target directory for project
				// don't have intermediate directories for groups
				if (idProp == P_OutDirs_Target || idProp == P_OutDirs_Intermediate)
				{
					val = ""; // default is nothing for the int/trg directories
					gpt = valid;
				}
			}
		}
	}

	return gpt;
}

GPT CProjItem::GetConvertedStrProp(UINT idProp, CString& val)
{
	GPT gptRet = GetStrProp(idProp, val);
	if (gptRet == valid)
		::ConvertMacros(val);
	return gptRet;
}

void CProjItem::SetOwnerProject(CProject * pProject, CTargetItem * pTarget)
{
	m_pOwnerProject = pProject;
	m_pOwnerTarget = pTarget;
	
	// repeat for all children*
	if (GetContentList())
	{
		POSITION pos;
	 	for (pos=GetHeadPosition(); pos!=NULL;)
	  	{
			// Any contained slobs should also be items:
			((CProjItem *)GetNext(pos))->SetOwnerProject(pProject, pTarget);
		}
	}
}

void CProjItem::IsolateFromProject()
{
	// update each config record
	UINT nIndex, nSize = GetPropBagCount();

	// copying into a non-projitem, so remove any links to Project
	for (nIndex = 0; nIndex < nSize; nIndex++)
	{
		// copy important pieces from project base record
		CConfigurationRecord * pcr = (CConfigurationRecord *)m_ConfigArray[nIndex];
		ForceConfigActive(pcr);

		theUndoSlob.Pause();	// don't record SetStrProp within SetSlopProp
		SetStrProp(P_ProjConfiguration,	pcr->m_pBaseRecord->GetConfigurationName());
		SetStrProp(P_ProjOriginalType, pcr->m_pBaseRecord->GetOriginalTypeName());
		theUndoSlob.Resume();
		
		// now set ourselves as our own Base record
		m_ConfigMap.RemoveKey((void*)pcr->m_pBaseRecord);
		pcr->m_pBaseRecord = pcr;
		m_ConfigMap.SetAt((void *)pcr->m_pBaseRecord, (void *)pcr);

		// reset config.
		ForceConfigActive();
	}

	// repeat for all children*
	if (GetContentList())
	{
		POSITION pos;
	 	for (pos=GetHeadPosition(); pos!=NULL;)
	  	{
			// Any contained slobs should also be items:
			((CProjItem *)GetNext(pos))->IsolateFromProject();
		}
	}
}

BOOL CProjItem::SetSlobProp(UINT idProp, CSlob * val)
{
	// only interested in container property changes
	if (idProp != P_Container)
		// pass on to the base-class
		return CSlob::SetSlobProp(idProp, val);

	// old container?
	CSlob * pOldContainer = m_pContainer; 

	// REVIEW: this currently happens during UNDO.  Bogus Undo record
	if (pOldContainer == val)
		return TRUE; // nothing to do

	// from a project?
	BOOL fFromPrj = m_pContainer && m_pContainer->IsKindOf(RUNTIME_CLASS(CProjItem)) && (((CProjItem *)m_pContainer)->GetProject() != NULL);

	// into a project?
	BOOL fToPrj = val && val->IsKindOf(RUNTIME_CLASS(CProjItem));

	BOOL fPrjChanged = FALSE;
	if (fFromPrj && fToPrj)
	{
		fPrjChanged = (((CProjItem *)val)->GetProject() != ((CProjItem *)m_pContainer)->GetProject());
	}
	// pre-move item
	if (!PreMoveItem(val, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	// do the 'containment' part of the move
	if (!MoveItem(val, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	// pos-move item, only call if not deleted!
	// ie. not deleted if moving to another container or
	// we're recording if, and hence moving into the undo slob.
	if (CouldBeContainer(val) &&
		!PostMoveItem(val, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	return TRUE;	// success
}
///////////////////////////////////////////////////////////////////////////////
const CString CProjItem::EndOfBagsRecord = "\x08\x14\x53\x53\x42\x52"; // :-)

///////////////////////////////////////////////////////////////////////////////
//
//	Override CSlob's serialize function to save only private properties:

BOOL CProjItem::SerializePropMapFilter(int nProp)
{
	return ::IsPropPrivate (nProp);
}

///////////////////////////////////////////////////////////////////////////////
// Return a container-less, symbol-less, clone of 'this'
CSlob* CProjItem::Clone()
{
	CSlob * pClone = CSlob::Clone(); // call base class to creat clone
	VSASSERT(pClone->IsKindOf (RUNTIME_CLASS (CProjItem)), "Can only clone CProjItem objects");

	((CProjItem*)pClone)->SetFile(GetFilePath());

	if (GetProject() != NULL)
		((CProjItem *)pClone)->IsolateFromProject();

	return pClone;
}

///////////////////////////////////////////////////////////////////////////////
void CProjItem::CloneConfigs(const CProjItem * pItem)
{
//	We have moved into a new project tree and need to adopt its configurations
//	We'll enumerate throught the new parents configurations and create new
//	configs for any we don't have, and delete any configurations that the 
//	parent doesn't support.
//
//	We say one our configurations matches a parent configuration if its based
//	on the same project type and has the same name.
//
//	In the special case that we have no configurations, just blindly copy the
//	parent's:

	if (GetPropBagCount() == 0)
	{
		UINT nSize = pItem->GetPropBagCount();

		// create the new configuration records
		for (UINT nIndex = 0; nIndex < nSize; nIndex++)
		{
			const CConfigurationRecord * pcr = (CConfigurationRecord *) pItem->m_ConfigArray[nIndex];
			ASSERT_VALID(pcr);
			ASSERT_VALID(pcr->m_pBaseRecord);

			ConfigurationChanged(pcr->m_pBaseRecord);

			// validate newly-created CR
			pcr = (CConfigurationRecord *)m_ConfigArray[nIndex]; 
			ASSERT_VALID(pcr);
			ASSERT_VALID(pcr->m_pBaseRecord);													
		}
		VSASSERT(GetPropBagCount()==pItem->GetPropBagCount(), "Not all property bags were created!"); // make sure they all got created

		// Finally, make sure we have the same active config:
		if (pItem->m_pActiveConfig)	// May be null during desrialize
		{		
			ConfigurationChanged(pItem->m_pActiveConfig->m_pBaseRecord);
			ASSERT_VALID (m_pActiveConfig);
		}
		else
		{
			m_pActiveConfig = NULL; // don't leave as bogus value
		}
	}
	else
	{
		// if we got here we must be undo/redo'ing, dragging/dropping or cloning
		// if (!theUndoSlob.InUndoRedo())
		{
			// Configuration Matching. This is only used by cut/paste & drag/drop
			
			// For drag and drop the configurations should be identical
			// so we check this here!
			// However, for clipboard cut/paste,
			// the configurations will not necessarily match identically
			// and we will have to be somewhat cleverer than we already are. 

			int nIndex, nSize = pItem->GetPropBagCount();
			const CConfigurationRecord *pcr;

			for (nIndex = 0; nIndex < nSize; nIndex++)
			{
				pcr = (const CConfigurationRecord*)pItem->m_ConfigArray[nIndex];
				ASSERT_VALID (pcr);
				ASSERT_VALID (pcr->m_pBaseRecord);

				// look for matching config
				if (ConfigurationChanged(pcr->m_pBaseRecord))
				{
					// FUTURE: try for partial match
					// Last cr in m_ConfigArray is new
					// try to initialize based on projtype
				}
			}

			// Delete any unreferenced configs and reorder, if necessary
			FixConfigs(pItem);
			
			// Finally, make sure we have the same active config:
			if (pItem->m_pActiveConfig)	// May be null during desrialize
			{
				ConfigurationChanged (pItem->m_pActiveConfig->m_pBaseRecord);
				ASSERT_VALID (m_pActiveConfig);
			}
  		}
	}
	VSASSERT(GetPropBagCount()==pItem->GetPropBagCount(), "Not all property bags were created!");
}

///////////////////////////////////////////////////////////////////////////////
void CProjItem::FixConfigs(const CProjItem * pItem)
{
// Garbage collection

	// delete any unneeded source configs
	CVCPtrList deathRow;
	CConfigurationRecord * pcr;
	const CConfigurationRecord * pcrBase;
	int nSize = GetPropBagCount();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		// look up the base config for pItem that matches each entry
		// by name.  If the base configs do not match, then this is
		// an unneeded entry that should be deleted.

		pcr = (CConfigurationRecord *)m_ConfigArray[nIndex];
		ASSERT_VALID (pcr);
		ASSERT_VALID (pcr->m_pBaseRecord);

		pcrBase = ((CProjItem *)pItem)->ConfigRecordFromConfigName(pcr->GetConfigurationName());
		if (!pcrBase || pcr->m_pBaseRecord != pcrBase->m_pBaseRecord)
		{
			// this one needs to be deleted
			deathRow.AddTail((void *)pcr->m_pBaseRecord);
		}
	}

	while (!deathRow.IsEmpty())
		DeleteConfigFromRecord((CConfigurationRecord *)deathRow.RemoveHead(), FALSE);

	VSASSERT(GetPropBagCount()==pItem->GetPropBagCount(), "Not all property bags are present!");

	// Now reorder remaining configs, if necessary, based on pItem
	nSize = pItem->GetPropBagCount();
	VSASSERT(nSize==GetPropBagCount(), "Not all property bags are present!"); // consistency check
	for (nIndex = 0; nIndex < nSize; nIndex++)
	{	
		pcrBase = ((CConfigurationRecord *)pItem->m_ConfigArray[nIndex])->m_pBaseRecord;
		m_ConfigMap.Lookup((void*)pcrBase, (void *&)pcr);
		m_ConfigArray[nIndex] = pcr;
	}
	
	//	Repeat for all children:
	if (GetContentList ())
	{
		POSITION pos;
		for (pos=GetHeadPosition(); pos!=NULL;)
		{
			// Any contained slobs should also be cleaned up
			((CProjItem *)GetNext(pos))->FixConfigs(pItem);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
void CProjItem::DeleteConfigFromRecord (CConfigurationRecord *prec, BOOL bChildren)
{
//	Delete the configuration and tell our kids to do the same.  Note that
//  subprojects that get called for a parents config will do nothing since
// 	they won't find prec.

	// Notify target items about config deletions
	if (this->IsKindOf(RUNTIME_CLASS(CTargetItem)) ||
		this->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
 		InformDependants(SN_DESTROY_CONFIG, (DWORD_PTR)prec);
	}
 
	if (bChildren && GetContentList())
	{
		for (POSITION pos = GetHeadPosition(); pos != (POSITION)NULL;)
			((CProjItem *) GetNext (pos))->DeleteConfigFromRecord (prec);
	}

	// We may not have the config rec that we want to delete, in which case we
	// don't do anything (however we do still recurse to our children just above,
	// in case they have the config rec).
	CConfigurationRecord *pMyRec;
	if (m_ConfigMap.Lookup(prec, (void *&) pMyRec))
	{
		ASSERT_VALID(pMyRec);
		VSASSERT(pMyRec->m_pBaseRecord == prec, "Inconsistent config records!");

		// remove from both Map and Array
		int i = 0;
		int size = GetPropBagCount();

		m_ConfigMap.RemoveKey(prec);
		while (i < size)
		{
			if (pMyRec == m_ConfigArray.GetAt(i))
			{
				m_ConfigArray.RemoveAt(i);
				break;	// stop if found
			}

			i++;
		}

		// if we're deleting our current active then 
		// set it to NULL which will cause ::GetActiveConfig()
		// to re-figure it out
		if (m_pActiveConfig == pMyRec)
			m_pActiveConfig = (CConfigurationRecord *)NULL;

		delete pMyRec;
	}
}

///////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CProjItem::AssertValid () const
{
	CProjItem * ncthis = (CProjItem *)this; // Brute force since we're const.

	// validate base component
	CObject::AssertValid();

	// validate content list
	if (ncthis->GetContentList()) ASSERT_VALID (ncthis->GetContentList());

	// validate our path
	if (GetFilePath ()) ASSERT_VALID (GetFilePath());
}
///////////////////////////////////////////////////////////////////////////////
void CProjItem::Dump (CDumpContext &DC) const
{
	//ASSERT_VALID(this);
	CObject *pObject;
	CConfigurationRecord *pcr;
	void *pv;
	TCHAR buf[257]; buf[256]=0;
	CProjItem *ncthis = (CProjItem *) this; // Brute force since we're const.

	if (DC.GetDepth () > 1) DC << 
	"************************************************************************";

	_snprintf ( buf, 256, "Dump of %s at %p:"
		" File path = \"%s\"",
		(const char *) GetRuntimeClass()->m_lpszClassName, (void *) this,
		(GetFilePath ()) ? ((const char *) *GetFilePath ()) : " (no file) " );
	DC << buf;

	int i;
	int size = GetPropBagCount();
	if (!size)	DC << 	"ConfigArray is empty\n";
	else
	{
		_snprintf ( buf, 256, "Config array has %i entries. Active is %p",
				size, (void *)((CProjItem *)this)->GetActiveConfig());
		DC << buf;	
		for (i =0; i < size; i++)
		{
			DC << (CConfigurationRecord *)m_ConfigArray[i];
		}
	}
	if (m_ConfigMap.IsEmpty ())	 DC << 	"ConfigMap is empty\n";
	else
	{
		_snprintf ( buf, 256, "Config map has %i entries. Active is %p",
				m_ConfigMap.GetCount (), (void *) ((CProjItem *)this)->GetActiveConfig());
		DC << buf;	
		for (VCPOSITION pos = m_ConfigMap.GetStartPosition (); pos != NULL;)
		{
			m_ConfigMap.GetNextAssoc ( pos, pv, (void *&) pcr );
			DC << pcr;
		}
	}

	if (ncthis->GetContentList()) 
	{
		if (ncthis->GetContentList()->IsEmpty()) 
									DC << "Contents list is empty.";
		else
		{
			DC << "Contents list:";
			for (POSITION pos = ncthis->GetContentList()->GetHeadPosition(); pos != NULL;)
			{
				pObject = ncthis->GetContentList()->GetNext (pos);
				if (DC.GetDepth () > 1)	pObject->Dump ( DC );
				else
				{
					_snprintf (buf, 256, "     a %s at %p",
				 		(const char *) pObject->GetRuntimeClass()->m_lpszClassName,
						(void *) pObject);
					DC << buf; 
				}
			}
		}
	}
}
#endif

void CProjItem::CopyCommonProps(CProjItem * pSrcItem, const CString & strTarget)
{
    CConfigurationRecord * pcrSrc = pSrcItem->ConfigRecordFromConfigName(strTarget, FALSE, FALSE);
    CConfigurationRecord * pcrDest = GetActiveConfig();
	if( pcrSrc == NULL ){
		// this is a sanity check since ConfigR...gName() can return NULL
		// if it does return NULL then there is no configuration to copy.
		// this should be ok.
		return;
	}

    CPropBag * pBagSrc = pcrSrc->GetPropBag(m_idBagInUse);
    CPropBag * pBaseBagSrc = pcrSrc->GetPropBag(BaseBag);
    CPropBag * pBagDest = pcrDest->GetPropBag(m_idBagInUse);
    CPropBag * pBaseBagDest = pcrDest->GetPropBag(BaseBag);

    // Copy all of the properties here
    //pBagDest->Clone(this, pBagSrc, FALSE);

    // Now we have to check for string list properties and potentially munge
    // the copied versions so that we only copy common settings!
    CProp * ppropSrc, * ppropBaseSrc, * ppropDest, * ppropBaseDest;

    POSITION pos = pBagSrc->GetStartPosition();
    while (pos != NULL)
    {
        UINT id;
		BOOL bInform;
        pBagSrc->GetNextAssoc(pos, id, ppropSrc);

		switch (ppropSrc->m_nType)

		{
			case integer:
				//
				// Inform dependants if setting P_ItemExcludedFromBuild to TRUE and
				// proprety for P_ItemExcludedFromBuild wasn't previously in pBagDest.
				// This is needed because CProjItem::GetIntProp has a hack to return
				// P_ItemExcludedFromBuild as TRUE when the proprety doesn't exist.  This
				// causes AssignActions to do an AddRefRef that needs to be undone.
				//
				bInform = P_ItemExcludedFromBuild==id && 
					NULL==pBagDest->FindProp(P_ItemExcludedFromBuild) &&
					((CIntProp *)ppropSrc)->m_nVal;

				pBagDest->SetIntProp(this, id, ((CIntProp *)ppropSrc)->m_nVal);

				if (bInform)
					InformDependants(id);
				break;

			case string:
			{
				COptionHandler * popthdlr = g_pPrjcompmgr->FindOptionHandler(id);
				BOOL fIsListStrProp = (popthdlr != (COptionHandler *)NULL) && popthdlr->IsListStrProp(id-1);
				if (!fIsListStrProp)
 					pBagDest->SetStrProp(this, id, ((CStringProp *)ppropSrc)->m_strVal);
 				else
				{
					// Ok now we may need to start munging to get correct props

					// Get all of our properties from the various bags
					ppropBaseSrc = pBaseBagSrc->FindProp(id);
					ppropDest = pBagDest->FindProp(id);
					ppropBaseDest = pBaseBagDest->FindProp(id);

					CString strNewProp;
					if (ppropDest)
					{
						// Work out the differences we need to apply to the destination bag
						COptionList optlstAdd, optlstSub;
						COptionList optlist;
						optlist.SetString(((CStringProp *)ppropBaseSrc)->m_strVal);

						optlist.Components(((CStringProp *)ppropSrc)->m_strVal, optlstAdd, optlstSub);

						// Apply the differences to the base prop val in the destination bag
						COptionList optlstDest;
						optlstDest.SetString(((CStringProp *)ppropDest)->m_strVal);
						optlstDest.Subtract(optlstSub);
						optlstDest.Append(optlstAdd);

						// Get the new prop string
						optlstDest.GetString(strNewProp);
					}
					else
						strNewProp = ((CStringProp *)ppropSrc)->m_strVal;

					// set the prop value
					pBagDest->SetStrProp(this, id, strNewProp);
				}
                break;
			}

			default:
				VSASSERT(FALSE, "Unhandled case");
				break;
		}
    }
}

int CProjItem::CompareSlob(CSlob * pCmpSlob)
{
	VSASSERT(pCmpSlob->IsKindOf(RUNTIME_CLASS(CProjItem)), "Can only compare CProjItem slobs!");

	// Put Groups at the start of this container
	if (((CProjItem *)pCmpSlob)->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		return 1;

	if (IsKindOf(RUNTIME_CLASS(CProjGroup)))
		return -1;

	// Put subprojects at the start of this container
	if (((CProjItem *)pCmpSlob)->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		return 1;

	if (IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		return -1;

	CString strExt, strCmpExt; 
	BOOL bCmpIsFileItem = (((CProjItem *)pCmpSlob)->IsKindOf(RUNTIME_CLASS(CFileItem)));
	BOOL bIsFileItem = (IsKindOf(RUNTIME_CLASS(CFileItem)));
	if (bCmpIsFileItem)
		strCmpExt = ((CFileItem *)pCmpSlob)->GetFilePath()->GetExtension();
	if (bIsFileItem)
		strExt = GetFilePath()->GetExtension();

	// Put .lib files last and don't sort them alphabetically
	if (bIsFileItem && (strExt.CompareNoCase(".lib")==0))
		return 1;

	if (bCmpIsFileItem && (strCmpExt.CompareNoCase(".lib")==0))
		return -1;

	// Put .obj files last and don't sort them alphabetically
	if (bIsFileItem && (strExt.CompareNoCase(".obj")==0))
		return 1;

	if (bCmpIsFileItem && (strCmpExt.CompareNoCase(".obj")==0))
		return -1;

	// Everything else is sorted based on their P_Title property, i.e alphabetically

	int retval = CProjSlob::CompareSlob(pCmpSlob);

	// as a tie breaker, use the absolute path to at least get consistency
	if (retval == 0)
	{
		VSASSERT(GetFilePath() != NULL, "File path not initialized properly!");
		VSASSERT(((CProjItem *)pCmpSlob)->GetFilePath() != NULL, "File path not initialized properly!");
		retval = _tcsicmp((LPCTSTR)*GetFilePath(), (LPCTSTR)*((CProjItem *)pCmpSlob)->GetFilePath());
	}
	VSASSERT(retval != 0, "Two compared items can never be equal!"); // should never be equal
	return retval;
}

///////////////////////////////////////////////////////////////////////////////
//
// --------------------------------	CFileItem ---------------------------------
//
///////////////////////////////////////////////////////////////////////////////
#define theClass CFileItem
BEGIN_SLOBPROP_MAP(CFileItem, CProjItem)
END_SLOBPROP_MAP()
#undef theClass

CFileItem::CFileItem () 
{
	// our file path
	m_pFilePath = NULL;

	// display name dirty to begin with
	m_fDisplayName_Dirty = TRUE;
}

BOOL CFileItem::SetIntProp(UINT idProp, int val)
{
	// which prop are we setting?
	switch (idProp)
	{
		case P_ItemExcludedFromBuild:
			break;

		case P_ItemIgnoreDefaultTool:
		{
			BOOL retval = CProjItem::SetIntProp(idProp, val);
			if (retval && GetProject() && GetProject()->m_bProjectComplete)
			{
				CConfigurationRecord * pcr = GetActiveConfig();
				CBuildAction::UnAssignActions(this, (CVCPtrList *)NULL, (CConfigurationRecord *)pcr->m_pBaseRecord);
				CBuildAction::AssignActions(this, (CVCPtrList *)NULL, (CConfigurationRecord *)pcr->m_pBaseRecord);
			}
			return retval;
		}
		break;
		default:
			break;
	}

	// pass on to base-class
	return CProjItem::SetIntProp(idProp, val);
}

BOOL CFileItem::SetStrProp(UINT idProp, const CString & str)
{
	if (idProp == P_ProjItemOrgPath)
	{
		if (str.IsEmpty())
		{
			GetMoniker(m_strOrgPath);  // default
			return TRUE;
		}

		// UNDONE: much further validation needed here!!!

		m_strOrgPath = str;
		InformDependants(idProp);
		GetProject()->DirtyProject();
		return TRUE;
	}
	return CProjItem::SetStrProp(idProp, str);
}

GPT CFileItem::GetStrProp(UINT idProp, CString& val)
{
	// is this a fake prop?
	switch (idProp)
	{
		case P_ProjItemName:	// yes
			if (m_fDisplayName_Dirty)
			{
				VSASSERT(m_pFilePath != NULL, "FilePath not initialized!");
				m_strDisplayName = m_pFilePath->GetFileName();
				m_fDisplayName_Dirty = FALSE;
			}
			val = m_strDisplayName;
			return valid;

		case P_ProjItemOrgPath:
		case P_FakeProjItemOrgPath:
			VSASSERT(m_pFilePath != NULL, "FilePath not initialized!");
			if (m_strOrgPath.IsEmpty())
				GetMoniker(m_strOrgPath);

			val = m_strOrgPath;
			return valid;

		case P_FileName:
		case P_ProjItemFullPath:
			VSASSERT(m_pFilePath != NULL, "FilePath not initialized!");
			val = (const TCHAR*) m_pFilePath->GetFullPath();
			return valid;

		// not allowed for file items
		case P_OutDirs_Target:
			if ((m_optbeh & OBShowMacro) && (GetProject()->GetOutDirString(val)))
				return valid; // config doesn't matter since always "$(OUTDIR)"
			else
				return invalid;

		case P_OutDirs_Intermediate:
			{
				// must only use INTDIR Macro, never INTDIR_SRC if defined
				if ((m_optbeh & OBShowMacro) && (GetOutDirString(val)))
					return valid;
				break;
			}
		default:
			break;
	}

	// no, pass on to base class
	return CProjItem::GetStrProp(idProp, val);
}

GPT CFileItem::GetIntProp(UINT idProp, int& val)
{
	// is this a fake prop?
	switch (idProp)
	{
		case P_ItemExcludedFromBuild:
		{
			// If this is an external target then all files are excluded from the build
			CProject * pProject = GetProject();

			// FUTURE: the top half of this 'if' may be redundant
            CProjType * pProjType;
            if (m_pForcedConfig != (CConfigurationRecord *) NULL)
            {
                pProject->ForceConfigActive((CConfigurationRecord *)m_pForcedConfig->m_pBaseRecord);
				pProjType = pProject->GetProjType();
                pProject->ForceConfigActive();
			}
            else
                pProjType = /*pProject->*/GetProjType();
			// fall through 
		}

		case P_ItemIgnoreDefaultTool:
		{
			GPT retval = CProjItem::GetIntProp(idProp, val);
			if (retval != valid)
			{
				val = 0;
				retval = valid;	// always valid if not excluded
			}
			return retval;
		}
		break;
		default:
			break;
	}

	// no, pass on to base class
	return CProjItem::GetIntProp(idProp, val);
}

// Return a container-less, symbol-less, clone of 'this'
CSlob * CFileItem::Clone()
{
	// call base class to create clone
	CSlob * pClone = CProjItem::Clone();
	VSASSERT(pClone->IsKindOf(RUNTIME_CLASS(CFileItem)), "Can only clone CFileItem objects!");

	return pClone;
}

//////////////////////////////////////////////////////////////////////////
BOOL CFileItem::SetFile(const CPath *pPath, BOOL bCaseApproved /*=FALSE*/) 
{
	// need to refresh this cache
	m_fDisplayName_Dirty = TRUE;

	// keep copy of old file path
	const CPath* pFilePathOld = m_pFilePath;

	BOOL bChanged = (pFilePathOld != pPath);

	// release a ref. to the old file
	if (bChanged)
	{
		if (pFilePathOld)
			delete pFilePathOld;
		if (pPath)
		{
			m_pFilePath = new CPath(*pPath);

			// invalidate cached relative path
			m_strMoniker.Empty();
			if ((GetProject()!= NULL) && (GetProject()->m_bProjectComplete))
			{
				m_strOrgPath.Empty();
				GetProject()->DirtyProject();
			}
			if ( (GetProject()!= NULL) && GetProject()->m_bConvertedDS4x)
			{
				m_strOrgPath.Empty();
			}


			((CPath *)GetFilePath())->SetAlwaysRelative();

			// this kind of gyration only works because we're not allowed to change the project after it is
			// loaded while still doing conversion.  Otherwise, the hack below would easily become outdated...
			// We're also making the basic assumption that if we find an .rc file, then there will be at least
			// one buildable one per config.  If not, oh, well, we're going to end up with additional resource
			// includes that we wouldn't otherwise have added.  Small bug.
			if (GetProject() && !GetProject()->m_bHaveODLFiles)
			{
				CString strODL = _T(".odl");
				CString strIDL = _T(".idl");
				CString strExt = GetFilePath()->GetExtension();
				strExt.MakeLower();
				GetProject()->m_bHaveODLFiles = (strODL == strExt || strIDL == strExt);
			}
		}
		else
			m_pFilePath = NULL;
	}

	return m_pFilePath != (const CPath*)NULL;	// success?
}
///////////////////////////////////////////////////////////////////////////////
const CPath *CFileItem::GetFilePath() const
{
	return m_pFilePath;
} 
///////////////////////////////////////////////////////////////////////////////
BOOL CFileItem::MoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// moving into or out of the project?
	// project not being created/destroyed?
	VSASSERT((CProjItem *)(fToPrj ? pContainer : pOldContainer) != (CProjItem *)NULL, "Bad state for moving an item!");

	return CProjItem::MoveItem(pContainer, pOldContainer, fFromPrj, fToPrj, fPrjChanged);
}

BOOL CFileItem::PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
 	if (!CProjItem::PostMoveItem(pContainer, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;
 
	if (IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem)))	// nothing more to do for build events
		return TRUE;

	// moving into or out of the project?
	// project not being created/destroyed?
	CProjItem * pProjCntr = (CProjItem *)(fToPrj ? pContainer : pOldContainer);
	VSASSERT(pProjCntr != (CProjItem *)NULL, "No container!");

	if ((fFromPrj || fToPrj) && pProjCntr->GetProject()->m_bProjectComplete)
	{
		// Send out 'addition' or 'deletion' of file notification
		if (fFromPrj && fToPrj)
		{
			VSASSERT(pOldContainer != pContainer, "Cannot move out of and into same container!");
			if (fPrjChanged)
			{
				VSASSERT(((CProjContainer *)pOldContainer)->GetTarget()!=((CProjContainer *)pContainer)->GetTarget(), "Cannot move out of and into same container!");
				
				// invalidate the moniker beacuse it changed projects.
				if( fPrjChanged ){
					m_strOrgPath.Empty();
					m_strMoniker.Empty();
				}
			}
			else
			{
				VSASSERT(((CProjContainer *)pOldContainer)->GetTarget()==((CProjContainer *)pContainer)->GetTarget(), "Cannot move out of and into same container!");
				// REVIEW: do nothing in this case
			}
		}
	}

	return TRUE;	// sucess
}
///////////////////////////////////////////////////////////////////////////////
void CFileItem::FlattenQuery(int fo, BOOL& bAddContent, BOOL& fAddItem)
{
	if (fo & flt_OnlyCustomStep)
		return;

	if (fo & flt_RespectItemExclude)
	{
		// get excluded from and convert into fAddItem by negating
		GetIntProp(P_ItemExcludedFromBuild, fAddItem);
		fAddItem = !fAddItem;
	}
	else
		fAddItem = TRUE;
}
///////////////////////////////////////////////////////////////////////////////
void CFileItem::GetMoniker( CString& cs )
{
	if (m_strMoniker.IsEmpty())
	{
		// Get the file's name relative to its project directory:
		const CPath *pPath = GetFilePath ();	
		VSASSERT (pPath, "Path not initialized!");
		if (!pPath->GetAlwaysRelative()) // this could fail if shared from SCC, etc.
			((CPath *)pPath)->SetAlwaysRelative(); // needed for GetRelativeName() call

		if (!pPath->GetRelativeName(GetProject()->GetWorkspaceDir(), m_strMoniker))
			{
				m_strMoniker = *pPath;
			}

		VSASSERT(!m_strMoniker.IsEmpty(), "Unable to get moniker!");
	}

	cs = m_strMoniker;
}

///////////////////////////////////////////////////////////////////////////////
//	Read in this file from the makefile reader
//

struct Pair {
	CObject *pObject;
	CConfigurationRecord *pcr;
	CBuildTool *pTool;
};


BOOL CFileItem::ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir)
{
	CVCPtrList ListMacros;
	BOOL bPerConfigDeps = TRUE, bPerConfigDesc = TRUE;
	TCHAR *pBase, *pTokenEnd;
	CObject *pObject = NULL;
	Pair *pPair;

	TRY
	{
		// The next element better be a comment marking our start:
		pObject = mr.GetNextElement ();
		VSASSERT (pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)), "Malformed project file");

		delete pObject;

		// The next item should be a macro with our file name:
		pObject = mr.GetNextElement ();
		if (!pObject->IsKindOf ( RUNTIME_CLASS(CMakMacro))
				||
			((CMakMacro*) pObject)->m_strName.CompareNoCase (pcFileMacroName)
			)  AfxThrowFileException (CFileException::generic);

		// This is the accpeted way to get a mungable pointer a CString's
		// data.  Note we never call ReleaseBuffer:

		pBase =	((CMakMacro*) pObject)->m_strValue.GetBuffer (1);	
		SkipWhite (pBase);
		GetMakToken (pBase, pTokenEnd);

		*pTokenEnd = _T('\0');

		CPath cpFile;
		m_strOrgPath = pBase;

		// translate $(MACRO_NAME) here.
		CString strBase = pBase;
#if 0
		if( !ReplaceEnvVars(strBase) ){
			strBase = pBase;
		}
#endif

		if (pBase == pTokenEnd ||
			!cpFile.CreateFromDirAndRelative (BaseDir, strBase))
			AfxThrowFileException (CFileException::generic);

		// preserve relative path if it begins with "..\"
		// if ((pBase[0] == _T('.')) && (pBase[1] == _T('.')))
		cpFile.SetAlwaysRelative();

		if (!SetFile(&cpFile, TRUE))
			AfxThrowFileException (CFileException::generic);

		delete pObject;

		m_cp = EndOfConfigs;
	   	for (pObject = mr.GetNextElement(); !IsEndToken(pObject); pObject = mr.GetNextElement())
	   	{
			if (IsPropComment(pObject) || IsAddSubtractComment(pObject))
				continue;

			if (pObject->IsKindOf(RUNTIME_CLASS(CMakMacro)))
			{
				const TCHAR * pMacName = (const TCHAR *)((CMakMacro *)pObject)->m_strName;
				
				// Per-config deps.?
				if (_tcsnicmp("DEP_", pMacName, 4) == 0 ||
					_tcsnicmp("NODEP_", pMacName, 6) == 0 ||
					_tcsnicmp(_TEXT("USERDEP_"), pMacName, 8) == 0) // user-defined dependency
				{
					if (m_cp == EndOfConfigs)
						bPerConfigDeps = FALSE;
				}

				// Got a tool macro, store it away for later processing, after
				// we have created all the actions
				pPair = new Pair;
				pPair->pObject = pObject;
				pPair->pcr = GetActiveConfig();
				pPair->pTool = g_pBuildTool;
				ListMacros.AddTail( pPair );

				pObject = (CObject *)NULL;
				continue;
			}
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakDescBlk)))
			{
				// Got a tool desc. block macro, store it away for later processing, after
				// we have created all the actions
				if (m_cp == EndOfConfigs)
					bPerConfigDesc = FALSE;

				pPair = new Pair;
				pPair->pObject = pObject;
				pPair->pcr = GetActiveConfig();
				pPair->pTool = NULL;
				ListMacros.AddTail( pPair );

				pObject = (CObject *)NULL;
				continue;
			}

			if (pObject->IsKindOf(RUNTIME_CLASS(CMakDirective))) 
			{
				// Setup this configuration
				ReadConfigurationIfDef((CMakDirective *)pObject);
			}

	   		delete pObject; pObject = (CObject *)NULL;
		}				 

		// Make sure we got a file name:
		VSASSERT (GetFilePath () != NULL, "Failed to get file path!");

		// Do rest of the move
		(void)PostMoveItem(GetContainer(), (CSlob *)NULL, FALSE, TRUE, FALSE);

		//
		// Process tool macros now
		//
 		CConfigurationRecord * pcr;
		CProject * pProject = GetProject();
		CProjTempConfigChange projTempConfigChange(pProject);

		while( !ListMacros.IsEmpty() )
		{
			pPair =(Pair *)ListMacros.RemoveHead();
			pObject = pPair->pObject;
			pcr = pPair->pcr;
			g_pBuildTool = pPair->pTool;
			delete pPair;

			// Make sure we're in the right configuration
			projTempConfigChange.ChangeConfig((CConfigurationRecord *)pcr->m_pBaseRecord);

			// Descriptor block or macro?
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakMacro)))
			{
				if (!bPerConfigDeps)
					m_cp = EndOfConfigs;
				else
					m_cp = FirstConfig;

				if (!SuckMacro((CMakMacro *)pObject, BaseDir, TRUE))
					AfxThrowFileException(CFileException::generic);
			}
			else
			{
				if (!bPerConfigDesc)
					m_cp = EndOfConfigs;
				else
					m_cp = FirstConfig;

				if (!SuckDescBlk((CMakDescBlk *)pObject))
					AfxThrowFileException(CFileException::generic);				
			}
				
			delete pObject; pObject = (CObject *)NULL;
		}
	}
	CATCH ( CException, e)
	{
		// free the object that we were working on when the exception occured.
		if (pObject != (CObject *)NULL)
		{
	   		delete pObject; pObject = (CObject *)NULL;
		}

		// free all Tool macros that we hadn't processed yet.
		while( !ListMacros.IsEmpty() )
		{
			pPair = (Pair *)ListMacros.RemoveHead();
			delete pPair->pObject;
			delete pPair;
		}
		
		return FALSE;	// failure
	}
	END_CATCH

	return TRUE;	// success
}

