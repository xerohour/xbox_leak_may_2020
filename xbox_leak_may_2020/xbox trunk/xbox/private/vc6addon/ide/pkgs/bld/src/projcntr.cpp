//
//	PROJCNTR.CPP
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop

#include "newgrp.h"		// For New Group dialog.
#include "projcomp.h"	// For g_Spawner
#include "resource.h"
#include "bldslob.h"
#include "bldnode.h"

#include <prjapi.h>
#include <prjguid.h>

IMPLEMENT_SERIAL (CProjContainer, CProjItem, SLOB_SCHEMA_VERSION)
IMPLEMENT_SERIAL (CProjGroup, CProjContainer, SLOB_SCHEMA_VERSION)
IMPLEMENT_SERIAL (CDependencyContainer, CProjContainer, SLOB_SCHEMA_VERSION)

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
	m_pFindItemCache = NULL;
}

CProjContainer::~CProjContainer ()
{
	if (m_pFindItemCache != NULL)
	{
		delete m_pFindItemCache;
		m_pFindItemCache = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Verb Commands

void CProjContainer::CmdAddGroup()
{
	// Build must not be in progress
	ASSERT(!g_Spawner.SpawnActive());

	CNewGroupDlg dlg;
	
	if (dlg.DoModal() == IDOK)
	{
		// Add a new group now
		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
		pProjSysIFace->BeginUndo(IDS_UNDO_ADD_GROUP);

		CProjGroup *pGroup = AddNewGroup(dlg.m_strGroup, dlg.m_strGroupFilter);
	
		// The recently added group should be at the end of our contents list:
		pProjSysIFace->EndUndo();
	}
}


CDependencyContainer * CProjContainer::GetDependencyContainer (BOOL bMake)
{
//
//	Return dependency container. Make one if there isn't one:
//

	POSITION pos, posSave;
	CSlob *pSlob;
	CDependencyContainer *pDepCntr;
	CProject * pProject = GetProject();
	BOOL bLast = TRUE;
	CObList * pContentList = GetContentList();
	for (pos = pContentList->GetTailPosition(); pos != NULL; )
	{
		posSave = pos;
		pSlob = (CSlob *)pContentList->GetPrev (pos);
		if (pSlob->IsKindOf ( RUNTIME_CLASS ( CDependencyContainer )))
        {
            pDepCntr = (CDependencyContainer*) pSlob;
			ConfigurationRecord * pcrBase = pProject->GetActiveConfig();
            if (pDepCntr->IsValid(pcrBase))
			{
				if (!bLast)
				{
					// make sure it is at the end of list
					pContentList->RemoveAt(posSave);
					pContentList->AddTail(pSlob);
				}		
				return (CDependencyContainer *) pSlob;
			}
		}
		bLast = FALSE;
	}

//
//	Have to make it:
//
	if ( bMake == TRUE)
	{
		// don't want to record this action
		theUndoSlob.Pause();

		pDepCntr = new CDependencyContainer();

        // Set up the config for which this container is active
        ConfigurationRecord * pcr = GetActiveConfig();
        pDepCntr->SetValid((ConfigurationRecord *)pcr->m_pBaseRecord);
        
		pDepCntr->MoveInto (this);

		theUndoSlob.Resume();
		return pDepCntr;
	}
	return NULL;
}			

BOOL CProjContainer::CanRemove ( CSlob *pRemoveSlob )
{
	return TRUE;
}

void CProjContainer::Add ( CSlob * pAddSlob )
{
	CProjItem::Add ( pAddSlob );
}

void CProjContainer::Remove ( CSlob * pRemoveSlob )
{
	CProjItem::Remove (pRemoveSlob);
};

// not terribly efficient;  Avoid except when necessary
// optimized for an already sorted list
void CProjContainer::SortContentList()
{
	int nCount = m_objects.GetCount();
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
	ASSERT(m_objects.GetCount() == nCount);
	OldList.RemoveAll();
};

void CProjContainer::RefreshAllMirroredDepCntrs()
{
    CProject * pProject = GetProject();
	if (pProject->IsExeProject())
		return;	// we should not have gotten here

	if (!IsKindOf(RUNTIME_CLASS(CTargetItem)))
	{
		ASSERT(GetTarget() != NULL);
		GetTarget()->RefreshAllMirroredDepCntrs();
		return;
	}
	// REVIEW: move to CTargetItem

	// For this item we want to go through all configs
	// and for any configs that are valid, we put the
	// project into this config and call RefreshDependencyContainer
	// finally resetting the config back.
	// This will refresh all dep containers for a given
	// container
	ConfigurationRecord * pcr, * pcrBase;
	CProjTempConfigChange projTempConfigChange(pProject);

	int nSize = m_ConfigArray.GetSize();
	for (int nConfig = 0; nConfig < nSize; nConfig++)
	{
		pcr = (ConfigurationRecord *)m_ConfigArray[nConfig];
		if (pcr->IsValid())
		{
            pcrBase = (ConfigurationRecord *)pcr->m_pBaseRecord;
			projTempConfigChange.ChangeConfig(pcrBase);
			RefreshDependencyContainer();
		}
	}
}

void CProjContainer::RefreshDependencyContainer() 
{
	if (!IsKindOf(RUNTIME_CLASS(CTargetItem)))
	{
		ASSERT(GetTarget() != NULL);
		GetTarget()->RefreshDependencyContainer();
		return;
	}
	// REVIEW: move to CTargetItem

	CFileRegistry * pRegistry = ((CTargetItem *)this)->GetRegistry();

	// our list of source + scanned dependencies across all actions
	FileRegHandle frh, frh2;
	CPtrList lstDeps;
    CProject * pProject = GetProject();
	ASSERT(NULL!=pProject);
	CObList ol;
	FlattenSubtree( ol, flt_Normal | flt_ExcludeProjects | flt_ExcludeGroups | flt_RespectItemExclude | flt_ExcludeDependencies | flt_RespectTargetExclude | flt_ExcludeProjDeps);

	POSITION pos = ol.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CFileRegSet * psetDep;

		// the contained project item
		CProjItem * pItem = (CProjItem *)ol.GetNext(pos);

		// get the actions for the item in this config.
		CActionSlobList * pActions = pItem->GetActiveConfig()->GetActionList();
		if (pActions->IsEmpty())
			continue;	// do nothing

		{
			POSITION pos1 = pActions->GetHeadPosition();
			while (pos1 != (POSITION)NULL)
			{
				CActionSlob * pAction = (CActionSlob *)pActions->GetNext(pos1);

				// get our scanned deps
				psetDep = pAction->GetScannedDep();
				psetDep->InitFrhEnum();
				while ((frh = psetDep->NextFrh()) != (FileRegHandle)NULL)
				{
					// don't add deps that are already in the project
					// UNDONE: handle multiple projects properly!
					frh2 = pRegistry->LookupFile(frh->GetFilePath());
					if (frh2 != NULL)
					{
						ASSERT(frh2==frh);
						if (GetTarget()->IsFileInTarget(frh2,TRUE))
						{
#ifdef REFCOUNT_WORK
							frh->ReleaseFRHRef();
							frh2->ReleaseFRHRef();
#endif
							continue;
						}
#ifdef REFCOUNT_WORK
						frh2->ReleaseFRHRef();
#endif
					}
					lstDeps.AddTail(frh);
				}

				// get our source deps
				psetDep = pAction->GetSourceDep();
				psetDep->InitFrhEnum();
				while ((frh = psetDep->NextFrh()) != (FileRegHandle)NULL)
				{
					// don't add deps that are already in the project
					// UNDONE: handle multiple projects properly!
					frh2 = pRegistry->LookupFile(frh->GetFilePath());
					if (frh2 != NULL)
					{
						ASSERT(frh2==frh);
						if (GetTarget()->IsFileInTarget(frh2,TRUE))
						{
#ifdef REFCOUNT_WORK
							frh2->ReleaseFRHRef();
							frh->ReleaseFRHRef();
#endif
							continue;
						}
#ifdef REFCOUNT_WORK
						frh2->ReleaseFRHRef();
#endif
					}
					lstDeps.AddTail(frh);
				}
			}
		}
	}

	// a map to allow us to remove duplicates and deps. no longer required
	CMapPtrToPtr ptrMap(31 /* increase map size */);
																				
	// try to get a dependency container (*don't* create it)
	CDependencyContainer * pDepCntr = GetDependencyContainer(FALSE);
	ASSERT(IsKindOf(RUNTIME_CLASS(CTargetItem)));

	CTargetItem* pTarget = (CTargetItem*)this;

	BOOL bHasExternalDepFolder = FALSE;
	if (valid!=pTarget->GetIntProp(P_TargNeedsDepUpdate,bHasExternalDepFolder))
		bHasExternalDepFolder = FALSE;

	if (pDepCntr == (CDependencyContainer *)NULL)
	{
		// don't need any deps?
		if (!bHasExternalDepFolder && lstDeps.IsEmpty())
			return;

		// create the container
		pDepCntr = GetDependencyContainer(TRUE);
		ASSERT(pDepCntr != (CDependencyContainer *)NULL);
	}
	else
	{
		// enter all the existing dependencies as unsused
		POSITION pos1 = pDepCntr->GetHeadPosition();
		while (pos1 != (POSITION)NULL)
			ptrMap.SetAt(((CDependencyFile *)pDepCntr->GetNext(pos1))->GetFileRegHandle(), FALSE);
	}

	// don't want to record these dependency container changes
	theUndoSlob.Pause();

	pos = lstDeps.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		frh = (FileRegHandle)lstDeps.GetNext(pos);

		// Filter out files that should not be in dep folder(e.g., .pch files).
		if (FileNameMatchesExtension(frh->GetFilePath(), "pch"))
		{
			frh->ReleaseFRHRef();
			continue;
		}
		BOOL fDepUsed;
		if (!ptrMap.Lookup((void *)frh, (void *&)fDepUsed))
		{
			// create the dependency file and move it into the 
			// dependency container
			CDependencyFile * pDepFile = new CDependencyFile;
			pDepFile->SetFileRegHandle(frh);
			pDepFile->MoveInto(pDepCntr);

			// mark dep. file as used
			ptrMap.SetAt((void *)frh, (void *)TRUE);
		}
		// mark dep. file as used
		else if (!fDepUsed)
		{
			ptrMap.SetAt((void *)frh, (void *)TRUE);
		}
#ifdef REFCOUNT_WORK
		frh->ReleaseFRHRef();
#endif
	}

	// go through the list of dependancy files and remove any
	// that were never referenced
	pos = pDepCntr->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CDependencyFile * pDepFile = (CDependencyFile *)pDepCntr->GetNext(pos);
		
		BOOL fDepUsed;
		VERIFY(ptrMap.Lookup((void *)pDepFile->GetFileRegHandle(), (void *&)fDepUsed));
		
		// delete the dependency file?
		if (!fDepUsed)
			pDepFile->MoveInto(NULL);
	}


	//
	// Update 
	if (!bHasExternalDepFolder && pProject->m_bPrivateDataInitialized)
	{
		//
		// Saving P_HasExternalDeps
		//
		pProject->SetIntProp(P_HasExternalDeps,NULL!=pDepCntr->GetHeadPosition());
	}
	bHasExternalDepFolder = bHasExternalDepFolder || NULL!=pDepCntr->GetHeadPosition();

	// if there are no dependencies, then delete the dependency container
	if (!bHasExternalDepFolder && NULL==pDepCntr->GetHeadPosition())
		pDepCntr->MoveInto(NULL);

	// allow recording again
	theUndoSlob.Resume();
}

void CProjContainer::DoAddFiles(BOOL bSubgroups)
{
	// this function should no longer be called since adding files to a
	// project is now controlled and performed from within the shell.
	ASSERT(FALSE);
}

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
#if 1
		ASSERT(0);  // should never get here
		pTarget = GetTarget();
#else
		CString strTargetName;
		((CProject *)pContainer)->InitTargetEnum();
		VERIFY(((CProject *)pContainer)->NextTargetEnum(strTargetName, pTarget));
#endif;
		pContainer = (CProjContainer *)pTarget;
	}
	pGroup->MoveInto(pContainer);

	if ((pszFilter) && (*pszFilter))
	{
		CString strFilter = pszFilter;
		VERIFY(pGroup->SetStrProp(P_GroupDefaultFilter, strFilter));
	}

	return pGroup;

	// FUTURE (rkern): fix
 	/*
 	// select this group in our view
   	((CProjectView *)GetSlobWnd())->m_TreeWnd.SetSelectedContainer(pGroup);

	// if the property sheet is not visible 
	// then show it for this newly added group
	if (!IsPropertyBrowserVisible())
		ShowPropertyBrowser(TRUE);
	*/
}

BOOL CProjContainer::ScanDependencies (BOOL bUseAnyMethod /* FALSE */, 
	BOOL bWriteOutput /* = TRUE */ )
{
//	Simply go through and scan every memeber.
	BOOL rc;

	g_ScannerCache.BeginCache();
	ConfigCacheEnabler EnableCaching;

	rc = g_ScannerCache.UpdateDependencies( this, bUseAnyMethod, bWriteOutput );

	g_ScannerCache.EndCache();

	return rc;
}


CProjGroup *CProjContainer::FindGroup (const TCHAR * pszName /* = NULL */)
{
	CProject *pProject;
	CObList ol;
	POSITION pos;
	CProjGroup *pGroup;
	BOOL	bFound = FALSE;
	CString strGroupName;
	CString str(pszName);

	pProject = GetProject ();
	if (!pProject) return NULL;
	pProject->FlattenSubtree ( ol, flt_Normal | flt_ExcludeDependencies );
	
	do
	{
		for (pos = ol.GetHeadPosition (); 
			 pos != NULL && !bFound; )
		{
			pGroup = (CProjGroup *) ol.GetNext (pos);
			if (!pGroup->IsKindOf ( RUNTIME_CLASS ( CProjGroup )))
				continue;
			pGroup->GetStrProp(P_GroupName, strGroupName);
			if (pGroup != this 	&&	CompareMonikers (strGroupName, str)==0)
				bFound = TRUE;
		}		
	} while (!bFound);

	if (bFound)
		return (CProjGroup *) pGroup;
	return NULL;
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
			VERIFY (pObject = mr.GetNextElement ());

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
				ASSERT(pProject==this);
				pContainer = pProject->GetActiveTarget();
				ASSERT(pContainer);
				CString strFilter = "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90";
				VERIFY(pItem->SetStrProp(P_GroupDefaultFilter, strFilter));
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
			ConfigurationRecord * pcrProject = pProject->GetActiveConfig();

			// read in the item data
			// (item will do the post-move)
			if (!pTemp->ReadFromMakeFile (mr, BaseDir))
				AfxThrowFileException (CFileException::generic);

			// preserve the current config.

			(void) pProject->SetActiveConfig(pcrProject);

		} while (TRUE);

		ConfigurationRecord * pcr;
		CProjTempConfigChange projTempConfigChange(pProject);

		int nSize = m_ConfigArray.GetSize();
		for (int nConfig = 0; nConfig < nSize; nConfig++)
		{
			// Put project into right config
			pcr = (ConfigurationRecord *)m_ConfigArray[nConfig];
			projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);

 			// Update the dependency container
//			if (IsKindOf(RUNTIME_CLASS(CTargetItem)))
//				RefreshDependencyContainer();	// Show kids dependencies.

		}
		if ((pProject==this)  && (pProject->m_bConvertedVC20))
		{
			ConvertDeps();
 		}
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
//  Stuff for serialization:  A projitem needs to write out its "private"
//	binary properties to the OPT file (done through an CArchive) and
//	label its data so that it can be later retrieved.  Remember that the
//  actual project tree is stored in the "global" makefile, so there
//	may be items in the OPT file that don't exist in the make file and visa
//	versa.  
//
//	A simple CProjItem (ie no contents) record consists of:
//				Item's relative moniker ( a CString) 
//				Items's runtime class (as string)
//				Items's props

// A container's record looks like:
//				Item's relative moniker ( a CString) 
//				Items's runtime class (as string)
//				Items's props
//
//					0 or more contained items.
//				EndOfSerializeRecord
//
///////////////////////////////////////////////////////////////////////////////
const CString CProjContainer::EndOfSerializeRecord = "\x08\x14\x44\x4A\x57"; // :-)

BOOL CProjContainer::SerializeProps(CArchive& ar)
{
	POSITION pos;
	CProjItem *pProjItem, *pTempItem = NULL;
	CString str;
	CRuntimeClass *pRTC;
	UINT wSchemaNum = 0l;

	// write our base-class properties first
	CProjItem::SerializeProps (ar);

	TRY
	{
		if (ar.IsStoring())
		{
			//	When were storing, write out our contents followed by and end 
			//	of record mark. 
			for (pos=GetHeadPosition(); pos != NULL;)
			{
				pProjItem = (CProjItem *) GetNext (pos);
				ASSERT (pProjItem->IsKindOf (RUNTIME_CLASS (CProjItem)));
			
				//	Do not save subprojects--the're save seperately to their own
				//	OPT file:
				if (pProjItem->IsKindOf (RUNTIME_CLASS (CProject))) continue;

				if( pProjItem->IsKindOf (RUNTIME_CLASS (CFileItem)) ) continue;

				pProjItem->GetMoniker ( str );
				ar << str;
				(pProjItem->GetRuntimeClass())->Store (ar);

				if (!pProjItem->SerializeProps(ar))
					AfxThrowArchiveException (CArchiveException::generic);
			}

			ar << EndOfSerializeRecord;
		}
		else
		{
			while ( ar >> str, str != EndOfSerializeRecord)
			{
				pRTC = CRuntimeClass::Load ( ar, &wSchemaNum );

				if (pRTC == NULL || wSchemaNum != pRTC->m_wSchema )
					AfxThrowArchiveException (CArchiveException::badSchema);

				pProjItem = FindItem (str, pRTC);

				// If pProjItem = NULL, then the associacted item has been reomved 
				// from the make file.  However, we still need to read the object's
				// data to keep the archive working, so we create a temporray 
				// object of the appropriate class, let it read its stuff, and then
				// delete it:

				if (pProjItem == NULL )
				{
					PFILE_TRACE ("A %s at %p  in CProjContainer::SerializeProps "
						"could not find child with moniker %s and class %s.\n",
						GetRuntimeClass()->m_lpszClassName, this, 
						(const char *) str, pRTC->m_lpszClassName );

					pTempItem = (CProjItem *) pRTC->CreateObject ();
					if (!pTempItem->SerializeProps(ar))	   	// Recursion!
						AfxThrowArchiveException (CArchiveException::generic);
					delete (pTempItem);
					pTempItem = NULL;
				}
				else if (!pProjItem->SerializeProps(ar))	// Recursion!
					AfxThrowArchiveException (CArchiveException::generic);
		 	}
		}
	}
	CATCH(CException, e)
	{
		if (pTempItem) delete (pTempItem);
		return FALSE;	// failure
	}
	END_CATCH

	// Delete the map that FindItem created to cache info about projitems.
	delete m_pFindItemCache;
	m_pFindItemCache = NULL;

	return TRUE;	// success
}
///////////////////////////////////////////////////////////////////////////////
CProjItem *CProjContainer::FindItem (CString strMoniker, CRuntimeClass *pRTC )
{
//	This is slow, essentially O[n].  Note that monikers are compared 
//	_wihhout_ regard to case:

	CProjItem *pProjItem;

	if (m_pFindItemCache == NULL)
	{
		// Initialise our cached info map.

		POSITION pos;
		CString str;

		m_pFindItemCache = new CMapStringToPtr;
		for (pos=GetHeadPosition(); pos != NULL;)
		{
			pProjItem = (CProjItem *) GetNext (pos);
			ASSERT (pProjItem->IsKindOf (RUNTIME_CLASS (CProjItem)));
			pProjItem->GetMoniker(str);
			str.MakeUpper();
			m_pFindItemCache->SetAt(str, pProjItem);
		}
			
	}

	strMoniker.MakeUpper();		// make sure monikers are unique (case-insensitive)
	
	// Lookup may fail.
	if (m_pFindItemCache->Lookup(strMoniker, (void *&) pProjItem))
	{
		ASSERT(pProjItem->IsKindOf (RUNTIME_CLASS (CProjItem)));
		if (pProjItem->GetRuntimeClass() == pRTC)
			return pProjItem;
	}

	return NULL;
}

extern BOOL g_bExcludedNoWarning; // defined in pfilereg.cpp

void CProjContainer::ConvertDeps()
{
	CProject * pProject = GetProject();
	ASSERT(pProject);
	ASSERT(pProject->m_bProjConverted || pProject->m_bConvertedVC20 || pProject->m_bConvertedDS4x);
	{
		FileRegHandle frh;
		CTargetItem * pTarget = GetTarget();
		ASSERT(pTarget);
		CFileRegistry * pRegistry = pTarget->GetRegistry();

		BOOL bJava = FALSE;
		CProject *pProj = (CProject *) g_BldSysIFace.GetActiveBuilder ();
		CProjType * pProjType = pProj->GetProjType();

		if (pProjType && pProjType->GetUniqueTypeId() == CProjType::javaapplet)
			bJava = TRUE;

		CString strName, strHeaderFilter, strResFilter;
		strName.LoadString(IDS_HEADER_FILES);
		strHeaderFilter.LoadString(IDS_HEADER_FILES_FILTER);

		CProjGroup * pHeaders = NULL;
		if (!bJava)
			pHeaders = pTarget->AddNewGroup(strName, strHeaderFilter);

		strName.LoadString(IDS_RESOURCE_FILES);
		strResFilter.LoadString(IDS_RESOURCE_FILES_FILTER);

		CProjGroup *pResources = pTarget->AddNewGroup(strName, strResFilter);

		// ones we might use...
		CProjGroup *pHelpFiles = NULL;
		CProjGroup *pTemplates = NULL;

		pTarget->RefreshAllMirroredDepCntrs();
			
		// REVIEW: do this for all configs

		CDependencyContainer * pDepCntr = pTarget->GetDependencyContainer(FALSE);
		if (pDepCntr != NULL)
		{
			POSITION pos = pDepCntr->GetHeadPosition();
			while (pos != NULL)
			{
				CDependencyFile * pDepFile = (CDependencyFile *)pDepCntr->GetNext(pos);
				frh = pRegistry->LookupFile(*pDepFile->GetFilePath());
				if (frh != NULL)
				{
					if (pTarget->IsFileInTarget(frh,TRUE))
					{
						// already in project
						frh->ReleaseFRHRef();
						frh = NULL;
						continue;
					}
					else
					{
						//
						// Ignore files that are generated during the build.
						// Assume that any file with a dep graph node is a generated file.
						//
						CFileDepGraph* pDepGraph = g_buildengine.GetDepGraph(GetActiveConfig());
						if (NULL!=pDepGraph)
						{
							CDepGrNode* pgrn;
							if (pDepGraph->FindDep(frh,pgrn))
							{
								// found in depgraph for this configuration, ignore.
								frh->ReleaseFRHRef();
								frh = NULL;
								continue;
							}
						}
					}
					CProjContainer * pNewContainer = NULL;

					// See if this belongs in the project
					// and if so, in what folder
					CPath path = *pDepFile->GetFilePath();
					CDir dir;
					dir.CreateFromPath(path);
					CString strDir = dir;
					// 1. if in project dir
					if (GetProject()->GetWorkspaceDir() == dir)
					{
						pNewContainer = pTarget; // default
					}
					else
					{
						// 2. Check subdirs: res, inc, include, hlp, template
						CString strPrj = pProject->GetWorkspaceDir();
						CString strTmp;
						if (strDir.Right(3).CompareNoCase("res")==0)
							pNewContainer = pResources;
						else
						{
							strTmp = strPrj + _T("\\inc"); //inc
							if (strTmp.CompareNoCase(strDir)==0)
								pNewContainer = pHeaders;
							else
							{
								strTmp = strPrj + _T("\\include"); //include
								if (strTmp.CompareNoCase(strDir)==0)
									pNewContainer = pHeaders;
								else
								{
									strTmp = strPrj + _T("\\hlp"); //hlp
									if (strTmp.CompareNoCase(strDir)==0)
									{
										if (pHelpFiles == NULL)
										{
											CString strHelpName; strHelpName.LoadString(IDS_HELP_FILES);
											pHelpFiles = pTarget->AddNewGroup(strHelpName);
											ASSERT(pHelpFiles);
										}
										pNewContainer = pHelpFiles;
									}
									else
									{
										strTmp = strPrj + _T("\\template"); //template
										if (strTmp.CompareNoCase(strDir)==0)
										{
											if (pTemplates == NULL)
											{
												CString strTemplateName; strTemplateName.LoadString(IDS_TEMPLATE_FILES);
												CString strTemplateFilter; strTemplateFilter.LoadString(IDS_TEMPLATE_FILES_FILTER);
												pTemplates = pTarget->AddNewGroup(strTemplateName,strTemplateFilter);
												ASSERT(pTemplates);
											}
											pNewContainer = pTemplates;
										}
										// 3. if ChangeExtension to .cpp is in project
										else if ((_tcsnicmp((LPCTSTR)path.GetExtension(), _T(".h"),2)==0) && ((path.ChangeExtension(".cpp"), (pTarget->IsProjectFile(path))) ||
												 (path.ChangeExtension(".c"), (pTarget->IsProjectFile(path))) ||
												 (path.ChangeExtension(".cxx"), (pTarget->IsProjectFile(path)))
												))
										{
											pNewContainer = pTarget; // default
										}
										else
										{
											// FUTURE: add other tests here
										}
									}
								}
							}
							
						}
					}

					if (pNewContainer != NULL)
					{
						if (pNewContainer == pTarget) // try to improve on default
						{
							if (FileNameMatchesExtension(pDepFile->GetFilePath(), strHeaderFilter))
								pNewContainer = pHeaders;
							else if (FileNameMatchesExtension(pDepFile->GetFilePath(), strResFilter))
								pNewContainer = pResources;


							ASSERT(pNewContainer);
						}

						BOOL bNoScan = (strDir.Right(9).CompareNoCase("\\template")==0);	
						BOOL bExclude = (bNoScan || FileNameMatchesExtension(pDepFile->GetFilePath(), "odl;idl;c;cxx;cpp;rc;for;f90"));
						CFileItem * pFile = new CFileItem;
						pFile->SetFile(pDepFile->GetFilePath(), TRUE);
						if (bExclude)
							g_bExcludedNoWarning = TRUE;
						pFile->MoveInto(pNewContainer);

						if (bNoScan)
							pFile->SetIntProp(P_ItemExcludedFromScan, TRUE);
						if (bExclude)
						{
							g_bExcludedNoWarning = FALSE;
							int iSize = pTarget->GetPropBagCount();
							ASSERT(iSize);
							const CPtrArray * pArrayCfg = pTarget->GetConfigArray();
							for (int j = 0; j < iSize; j++)
							{
								ConfigurationRecord * pcr = (ConfigurationRecord *)(*pArrayCfg)[j]; 
								if (pcr->IsValid())
								{
									CProjTempConfigChange projTempConfigChange(pProject);
									projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
									pFile->SetIntProp(P_ItemExcludedFromBuild, TRUE);
									int idOldBag = pFile->UsePropertyBag(BaseBag);
									pFile->SetIntProp(P_ItemExcludedFromBuild, TRUE);
									(void) pFile->UsePropertyBag(idOldBag);

									// HACK: avoid adding to dep graph
									BOOL bSavedComp = pProject->m_bProjectComplete;
									pProject->m_bProjectComplete = TRUE;
									pFile->InformDependants(P_ItemExcludedFromBuild);
									pProject->m_bProjectComplete = bSavedComp;
								}
							}
						}
					}
					frh->ReleaseFRHRef();
					frh = NULL;
				}
			}
		}

		// VS98 #878: Make sure resource.h is put in project also
		CPath path; path.CreateFromDirAndFilename(GetProject()->GetWorkspaceDir(), _T("resource.h"));
		if ((pHeaders!=NULL) && (path.ExistsOnDisk()))
		{
			frh = pRegistry->LookupFile(path);
			if ((frh == NULL) || (!pTarget->IsFileInTarget(frh,TRUE)))
			{
				// not already in project
				CFileItem * pFile = new CFileItem;
				pFile->SetFile(&path, FALSE);
				pFile->MoveInto(pHeaders);
			}
			if (frh!= NULL)
			{
				frh->ReleaseFRHRef();
				frh = NULL;
			}
		}

		pTarget->RefreshAllMirroredDepCntrs();

#if 0	/* DISABLED: Empty groups left in project so user knows where to put these types of files. */
		//
		// Delete unused groups
		//
		if (NULL!=pHeaders && pHeaders->GetContentList()->IsEmpty())
			pHeaders->MoveInto(NULL);

		if (NULL!=pResources && pResources->GetContentList()->IsEmpty())
			pResources->MoveInto(NULL);
#endif
	}
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
	VERIFY (m_strGroupName.LoadString (IDS_BASE_GROUP_NAME));

	// the Project Window icon inset
	m_nIconIdx = 1;
}

CProjGroup::CProjGroup (const TCHAR * pszName)
{
	m_strGroupName = pszName;

	// the Project Window icon inset
	m_nIconIdx = 1;
}

void CProjGroup::GetMoniker (CString& cs)
{
	ASSERT (!m_strGroupName.IsEmpty());
	cs = m_strGroupName;
}

///////////////////////////////////////////////////////////////////////////////
// Verb Commands
void CProjGroup::CmdAddFiles()
{
	DoAddFiles ( FALSE );
}

BOOL CProjGroup::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	AddPropertyPage(&g_GrpGeneralPage, this);

	if( bSetCaption )
		SetPropertyCaption(CString(MAKEINTRESOURCE(IDS_GROUP)));

	return TRUE;
}

BOOL CProjGroup::MakeGroupNameUnique(CString & strGroupName)
{
	CProject *pProject;
	CObList ol;
	POSITION pos;
	CProjGroup *pGroup;
	BOOL	bCollision;
	int i = 1;
	CString str;

	pProject = GetProject ();
	if (!pProject) return FALSE;
	pProject->FlattenSubtree ( ol, flt_Normal | flt_ExcludeDependencies );
	str = strGroupName;
	CString fmt, key;
	fmt.LoadString (IDS_GROUP_CUSTOMIZER);
	key.LoadString (IDS_GROUP_CUSTOMIZER_KEY);
	
	do
	{
		bCollision = FALSE;

		for (pos = ol.GetHeadPosition (); 
			 pos != NULL && !bCollision; )
		{
			pGroup = (CProjGroup *) ol.GetNext (pos);
			if (!pGroup->IsKindOf ( RUNTIME_CLASS ( CProjGroup )))
								continue;

			if (pGroup != this 
					&&
				CompareMonikers (pGroup->m_strGroupName, str)==0
				)
							bCollision = TRUE;
		}		

		if ( bCollision )
		{
			int n;

			// If it already ends in "No. #", strip it before adding it again
			if (((n = strGroupName.Find(LPCTSTR(key))) > 0)
				&& (n > (strGroupName.GetLength() - (key.GetLength() + 2))))
				strGroupName = strGroupName.Left(n);

			TCHAR * pc = str.GetBuffer (256);

			// Make sure there is enough space for the customizer:
			if (strGroupName.GetLength () > 255 - fmt.GetLength() -1 ) 
						strGroupName.ReleaseBuffer (255 - fmt.GetLength() -1 );
			_snprintf ( pc, 
						255, 
						(const TCHAR *) fmt, 
						(const TCHAR *) strGroupName,
						i );
			pc[255] = 0;							 
			str.ReleaseBuffer ();
 			i++; 
		}

	} while (bCollision);

	// Look at i to see if there ever was a collision:
	if (i > 1) 
		strGroupName = str;

	return (i > 1);
}

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
		VERIFY(((CProject *)pContainer)->NextTargetEnum(strTargetName, pTarget));
		pSlob = (CSlob *)pTarget;
	}
#endif

	CSlob * pSavedSlob = pSlob;
	TRY
    {
		do
		{
			pSlob = pSavedSlob;

			VERIFY (pObject = mr.GetNextElement ());

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
			ConfigurationRecord * pcrProject = GetProject()->GetActiveConfig();

			// read in the item data
			if (!pTemp->ReadFromMakeFile (mr, BaseDir))
				AfxThrowFileException (CFileException::generic);

			// preserve the current config.
			(void) GetProject()->SetActiveConfig(pcrProject);

		} while (TRUE);

		if (pProject->m_bConvertedDS4x || (pProject->m_bProjConverted && (!pProject->m_bConvertedVC20) && (!pProject->m_bConvertedDS5x)))
		{
			ConvertDeps();
#if 0
			ConfigurationRecord* pcr;
			int nSize = m_ConfigArray.GetSize();
			for (int nConfig = 0; nConfig < nSize; nConfig++)
			{
				pcr = (ConfigurationRecord *)m_ConfigArray[nConfig];
				ForceConfigActive(pcr);
				RefreshDependencyContainer ();	// Show kids dependencies.
				ForceConfigActive();
			}
#endif
		}
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

BOOL CProjGroup::CanAdd ( CSlob* pSlob )
{
	if (g_Spawner.SpawnActive())
		return FALSE;

	if (pSlob->IsKindOf(RUNTIME_CLASS(CFileItem)))
		return TRUE;

	if (!pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		return CProjContainer::CanAdd(pSlob);

	CProjItem * pContainer = this;
	while ((pContainer != NULL) && (pContainer->IsKindOf(RUNTIME_CLASS(CProjGroup))))
	{
		if (pContainer == (CProjItem *)pSlob)
			return FALSE; // can't add this group!!
		pContainer = (CProjItem *)pContainer->GetContainer();
	}
	return TRUE;
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
			MsgBox (Error, IDS_NO_EMPTY_GROUP) ;
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

	// If we're changing the group's name, make sure it's still unique:
	if (((idProp == P_ProjItemName) || (idProp == P_GroupName)) && (m_strGroupName.Compare(str)))
	{
		strChanged = str;
		bNameChanged = MakeGroupNameUnique(strChanged);
	}

	// P_ProjItemName is a fake prop that just maps to P_GroupName.
	bRetval = CProjContainer::SetStrProp ((idProp == P_ProjItemName) ? P_GroupName : idProp, (bNameChanged) ? strChanged : str);

	return bRetval;
}

BOOL CProjGroup::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// do the base-class thing
	if (!CProjContainer::PreMoveItem(pContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	if (pContainer == NULL) // just bail if deleted
		return TRUE;

#if 0
	// if that worked and not deleted, process content as well
	CProjItem * pItem;
	CObList * pGrpContents = GetContentList();
 	POSITION pos = pGrpContents->GetHeadPosition();
	if (pos != NULL)
	{
		theUndoSlob.Pause();
		while (pos != NULL)
		{
			pItem = (CProjItem *)pGrpContents->GetNext(pos);
			ASSERT(pItem);
			if (!pItem->PreMoveItem(pContainer, fFromPrj, fToPrj, fPrjChanged))
			{
				ASSERT(0);
				continue;
			}
		}
		theUndoSlob.Resume();
	}
#endif
	return TRUE;
}

BOOL CProjGroup::MoveItem(CSlob * pContainer, CSlob *pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// do the base-class thing
	if (!CProjContainer::MoveItem(pContainer, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	if (pContainer == NULL) // just bail if deleted
		return TRUE;

#if 0
	// if that worked and not deleting, move the content as well
	CProjItem * pItem;
	CObList * pGrpContents = GetContentList();
 	POSITION pos = pGrpContents->GetHeadPosition();
	if (pos != NULL)
	{
		theUndoSlob.Pause();
		while (pos != NULL)
		{
			pItem = (CProjItem *)pGrpContents->GetNext(pos);
			ASSERT(pItem);
			if (!pItem->MoveItem(this, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
			{
				ASSERT(0);
				continue;
			}
		}
		theUndoSlob.Resume();
	}
#endif
	return TRUE;
}

BOOL CProjGroup::PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// let the base-class do it's thing
	if (!CProjContainer::PostMoveItem(pContainer, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	if (pContainer == NULL) // just bail if deleted
		return TRUE;

#if 0
	// if that worked, process content as well
	if ((GetProject()) && (GetProject()->m_bProjectComplete))
	{
		CProjItem * pItem;
		CObList * pGrpContents = GetContentList();
 		POSITION pos = pGrpContents->GetHeadPosition();
		if (pos != NULL)
		{
			theUndoSlob.Pause();
			while (pos != NULL)
			{
				pItem = (CProjItem *)pGrpContents->GetNext(pos);
				ASSERT(pItem);
				if (!pItem->PostMoveItem(this, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
				{
					ASSERT(0);
					continue;
				}
			}
			theUndoSlob.Resume();
		}
	}
#endif
	// if we are being moved into a new container, make sure there isn't a group
	// with our name already
	if (fToPrj)
	{
		CString strGroupName = m_strGroupName;
		if (MakeGroupNameUnique(strGroupName))
		{
			// this allows the undo slob to record name change
			SetStrProp(P_GroupName, strGroupName);
			InformDependants(P_GroupName);
		}

		// don't do this during project load
		if (GetProject()->m_bProjectComplete)
			RefreshAllMirroredDepCntrs();
	}

	return TRUE;	// success
}

BOOL CProjGroup::WriteToMakeFile
( 
	CMakFileWriter& pw, 
	CMakFileWriter& mw, 
	const CDir &BaseDir,
	CNameMunger& nm
)
{
	BOOL retval = TRUE;
	POSITION pos;
	const TCHAR *pc = GetNamePrefix (this);
	PFILE_TRACE ("CProjGroup at %p writing to CMakFileWriter at %p\n",
														this, &mw);
	if (g_bWriteProject)
	{
		// write group open comment
		// pw.WriteSeperatorString();
		pw.WriteCommentEx("Begin %s \"%s\"", pc, (const TCHAR *) m_strGroupName);
		pw.EndLine();
	}

	if (g_bWriteProject)
	{
#if 0
		if (!g_buildfile.WriteToolForNMake(this, &nm))
		{
			// failed to write tools section for this group
			ASSERT(0);
			retval = FALSE;
		}
#else
		// only allow common Group props for now
		WritePerItemProperties(pw);
#endif
	}

	// this will cause our content to get written in a predictable order
	SortContentList();

	pos = GetHeadPosition ();
	while (pos != NULL)
	{
		if (!((CProjItem *) GetNext (pos))->WriteToMakeFile(pw, mw, BaseDir, nm))
		{
			ASSERT(0);
			retval = FALSE;	// failed to write out group content
		}
	}

	if (g_bWriteProject)
	{
		// write group closing comment
		pw.WriteCommentEx ( "%s %s", EndToken, pc );
	}

	return retval;	// success
}

BOOL CProjGroup::ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir)
{
//	Read in this item and any childred from a makfile reader.  The make
//  comment is the first line of our stuff:
//
	TCHAR *pBase;
	CObject *pObject = NULL;
	CString str;

	PFILE_TRACE ("CProjGroup at %p reading from CMakFileWriter at %p\n",
														this, &mr);

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

		for (; pObject = mr.GetNextElement ();)
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
				ASSERT(m_cp==EndOfConfigs);
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

		// preserve the current config. while we read in each child
		// to ensure that the child doesn't have the side-effect of 
		// changing this
		ConfigurationRecord * pcrProject = GetProject()->GetActiveConfig();

		// Read in children:
		if (!ReadInChildren(mr, BaseDir))
 			AfxThrowFileException (CFileException::generic);

		// preserve the current config.
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

void CProjGroup::PrepareAddList(CMultiSlob* pAddList, BOOL bPasting)
{
	CMapPtrToPtr mapFileRegs;
	CProjItem * pItem, *pDupItem;
	CObList * pAddListContents = pAddList->GetContentList();
	CTargetItem * pTarget = GetTarget();
	ASSERT(pTarget);
 
 	FileRegHandle hndFileReg;
	CObList * pGrpContents = GetContentList();
	
 	POSITION pos = pGrpContents->GetHeadPosition();
	while (pos != NULL)
	{
		pItem = (CFileItem *) pGrpContents->GetNext(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		{
			VERIFY(hndFileReg = pItem->GetFileRegHandle());
			mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
		}
	}
	// Now cycle through items to be added (backwards!) and eliminate any dupes
	pos = pAddListContents->GetTailPosition();
	while (pos != NULL)
	{
		pItem = (CProjItem *)pAddListContents->GetPrev(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		{
			FileRegHandle hndFileReg = pItem->GetFileRegHandle();
			ASSERT(hndFileReg); // if in file reg, check if in this group
			if (mapFileRegs.Lookup((void *)hndFileReg, (void * &) pDupItem))
			{
				// new behaviour: duplicate: remove original, replace in map
				mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
				if (pDupItem->GetContainer()==this)
					{	// if removing an existing item, move into UndoSlob
						pDupItem->MoveInto(NULL);
					}
				else	// if dup is also from AddList, just delete it
					{
						theUndoSlob.Pause();
						pAddList->Remove(pDupItem);	// removes dependency
						delete pDupItem;			// prevent memory leaks
						theUndoSlob.Resume();
					}
			}
			else if (pTarget->IsFileInTarget(hndFileReg, TRUE))
			{
				// keep the original in this case
				theUndoSlob.Pause();
				pAddList->Remove(pItem);	// removes dependency
				delete pDupItem;			// prevent memory leaks
				theUndoSlob.Resume();
			}
			else
			{
				mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
			}
		}
		else if (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			BOOL bReject = FALSE;
			CProjItem * pContainer = this;
			while ((pContainer != NULL) && (pContainer->IsKindOf(RUNTIME_CLASS(CProjGroup))))
			{
				if (pContainer == pItem)
				{
					// can't add group to itself or its children
					theUndoSlob.Pause();
					pAddList->Remove(pItem);
					delete pItem;				// prevent memory leaks
					bReject = TRUE;
					theUndoSlob.Resume();
					break;
				}
				pContainer = (CProjItem *)pContainer->GetContainer();
			}
			// reject duplicates
			if (!bReject)
			{
				// reject duplicates in group content
				CObList ol;
				pItem->FlattenSubtree( ol, flt_Normal | flt_ExcludeProjects | flt_ExcludeGroups | flt_ExcludeDependencies | flt_RespectTargetExclude | flt_ExcludeProjDeps);
				POSITION pos = ol.GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					CFileRegSet * psetDep;

					// the contained project item
					CProjItem * pContainedItem = (CProjItem *)ol.GetNext(pos);
					ASSERT(pContainedItem->IsKindOf(RUNTIME_CLASS(CFileItem)));
					if (!pContainedItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
						continue;

					FileRegHandle hndFileReg = pContainedItem->GetFileRegHandle();
					ASSERT(hndFileReg); // if in file reg, check if in this group
					if (mapFileRegs.Lookup((void *)hndFileReg, (void * &) pContainedItem))
					{
						theUndoSlob.Pause();
						pAddList->Remove(pContainedItem);	// removes dependency
						delete pItem;			// prevent memory leaks
						theUndoSlob.Resume();
					}
					else if (pTarget->IsFileInTarget(hndFileReg, TRUE))
					{
						theUndoSlob.Pause();
						pAddList->Remove(pContainedItem);	// removes dependency
						delete pItem;			// prevent memory leaks
						theUndoSlob.Resume();
					}
					else
					{
						mapFileRegs.SetAt((void *)hndFileReg, (void *)pContainedItem);
					}
				}
				ol.RemoveAll();
			}
		}
		else
		{
			// ignore all non-fileitems
			theUndoSlob.Pause();
			pAddList->Remove(pItem); 	// removes dependency
			delete pItem;				// prevent memory leaks
			theUndoSlob.Resume();
		}	
	}
}

BOOL CProjGroup::CanContain(CSlob* pSlob)
{
	if (pSlob->IsKindOf(RUNTIME_CLASS(CFileItem)) ||
	    pSlob->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
		return TRUE;

	// FUTURE: allow CDependencyFiles (with conversion)

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
// Drag and Drop support

void CProjGroup::PrepareDrop(CDragNDrop* pInfo)
{
	CSlob* pDragSlob;
 	
	pDragSlob = pInfo->m_dragSlob;

	CMapPtrToPtr mapFileRegs;
	CProjItem * pItem, * pDupItem;
	CObList * pAddListContents = pDragSlob->GetContentList();
	CTargetItem * pTarget = GetTarget();
	ASSERT(pTarget);

 	FileRegHandle hndFileReg;
	CObList * pGrpContents = GetContentList();
	
	ASSERT(!theUndoSlob.IsRecording());	// no need for pause/resume for Remove()

	CProject * pProject = (CProject *)GetContainer();	
	if (!pProject->AreFilesUnique(pDragSlob))
	{
		InformationBox(IDS_DUPES_IN_SELECTION);
	}

 	POSITION pos = pGrpContents->GetHeadPosition();
	while (pos != NULL)
	{
		pItem = (CFileItem *) pGrpContents->GetNext(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		{
			VERIFY(hndFileReg = pItem->GetFileRegHandle());
			mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
		}
	}

	// Can't delete slobs just yet, so just keep track of which ones to
	// delete later in DoDrop()
	ASSERT(m_DeathRow.IsEmpty());

	// Now cycle through items to be added (backwards!) and reject any we can't drop
	pos = pAddListContents->GetTailPosition();
	while (pos != NULL)
	{
		pItem = (CProjItem *)pAddListContents->GetPrev(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		{
			FileRegHandle hndFileReg = pItem->GetFileRegHandle();
			ASSERT(hndFileReg); // if in file reg, check if in this group
			if (mapFileRegs.Lookup((void *)hndFileReg, (void * &) pDupItem))
			{
				// duplicate: remove from group
				if (pItem == pDupItem)
				{
					// Dropping into the same container
					// so we just remove from add list
					pDragSlob->Remove(pItem);
				}
				else
				{
					// replace duplicate with pItem
					if (pDupItem->GetContainer()==this)
					{
						// remove duplicate from group
						m_DeathRow.AddHead((void *)pDupItem);
					}
					else
					{
						// the dupe is something we were adding,
						// so we must remove it from the list
						pDragSlob->Remove(pDupItem);

						// only delete it if we're doing a move
						// or copying a clone
						if (pDupItem->GetContainer()==NULL)
						{
							delete pDupItem;	// delete clone	
						}
						else if (!pInfo->IsCopy())
						{
							m_DeathRow.AddHead((void *)pDupItem);
						}
					}
					mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
				}			
			}
			else if (pTarget->IsFileInTarget(hndFileReg, TRUE))
			{
				// keep the original in this case
				pDragSlob->Remove(pItem);
			}
			else
			{
				mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
			}
		}
		else if (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			BOOL bReject = FALSE;
			CProjItem * pContainer = this;
			while ((pContainer != NULL) && (pContainer->IsKindOf(RUNTIME_CLASS(CProjGroup))))
			{
				if (pContainer == pItem)
				{
					// can't add group to itself or its children
					pDragSlob->Remove(pItem);
					bReject = TRUE;
					break;
				}
				pContainer = (CProjItem *)pContainer->GetContainer();
			}
			if (!bReject)
			{
				// reject duplicates in group content
				CObList ol;
				pItem->FlattenSubtree( ol, flt_Normal | flt_ExcludeProjects | flt_ExcludeGroups | flt_ExcludeDependencies | flt_RespectTargetExclude | flt_ExcludeProjDeps);
				POSITION pos = ol.GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					CFileRegSet * psetDep;

					// the contained project item
					CProjItem * pContainedItem = (CProjItem *)ol.GetNext(pos);
					ASSERT(pContainedItem->IsKindOf(RUNTIME_CLASS(CFileItem)));
					if (!pContainedItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
						continue;

					FileRegHandle hndFileReg = pContainedItem->GetFileRegHandle();
					ASSERT(hndFileReg); // if in file reg, check if in this group
					if (mapFileRegs.Lookup((void *)hndFileReg, (void * &) pContainedItem))
					{
						m_DeathRow.AddHead((void *)pContainedItem);
					}
					else if (pTarget->IsFileInTarget(hndFileReg, TRUE))
					{
						m_DeathRow.AddHead((void *)pContainedItem);
					}
					else
					{
						mapFileRegs.SetAt((void *)hndFileReg, (void *)pContainedItem);
					}
				}
				ol.RemoveAll();
			}
		}
		else
		{
			// ignore all non-fileitems
			pDragSlob->Remove(pItem); 	// removes dependency
		}	
	}
}

CSlobDragger* CProjGroup::CreateDragger(CSlob* pDragObject, CPoint screenPoint)
{
	return new CProjDragger;
}

void CProjGroup::DoDrop(CSlob *pSlob, CSlobDragger *pDragger)
{
	// We are actually dropping some files onto a group

	CMultiSlob* pSelection;
	POSITION pos;
	pSelection = new CMultiSlob;

	// Remove any unneeded slobs identified in PrepareDrop()
	while (!m_DeathRow.IsEmpty())
	{
		((CSlob*)m_DeathRow.RemoveHead())->MoveInto(NULL);
	}

    GetBuildNode()->HoldUpdates();
	
	pos = pSlob->GetHeadPosition();
	while (pos != NULL)
	{
		CSlob* pItemSlob = pSlob->GetNext(pos);
		
		if (CanAdd(pItemSlob))
		{
			pItemSlob->MoveInto(this);
			pSelection->Add(pItemSlob);
		}
		else
		{
			pItemSlob->MoveInto(NULL);
		}
	}
		
	// Clear the current selection and then select the items that
	// have been dropped.
    // FUTURE (colint): Fix to work with new build view (but only if
    // we ever reincarnate this group code!!)
	// m_pPSWnd->UpdateSelection(pSelection);

	delete pSelection;

    GetBuildNode()->EnableUpdates();
}

void CProjGroup::DoDrag(CSlob *pSlob, CSlobDragger *pDragger, CPoint screenPoint)
{
	pDragger->Show(TRUE);
}

BOOL CProjGroup::CanAct(ACTION_TYPE action)
{
	// no manipulating of groups while building
	if (g_Spawner.SpawnActive())
		return FALSE;

	return CProjContainer::CanAct(action);
}


#if 0 // defined inline
void CProjGroup::FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem)
{
	if (!(fo & flt_ExcludeGroups))
	{
		fAddItem = TRUE;
	}

	// Always consider contents regardless of whether we consider the
	// group node itself
	fAddContent = TRUE;
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// -------------------------- CDependencyContainer ----------------------------
//
///////////////////////////////////////////////////////////////////////////////
#define theClass CDependencyContainer
BEGIN_SLOBPROP_MAP(CDependencyContainer, CProjContainer)
END_SLOBPROP_MAP()
#undef theClass

CDependencyContainer::CDependencyContainer()
{
	// the Project Window icon inset
	m_nIconIdx = 4;
}

void CDependencyContainer::GetMoniker( CString& cs) 
{
	cs = "dep";		// Only ever one of these in a group.
}

GPT CDependencyContainer::GetStrProp(UINT idProp, CString& val)
{
	// is this a fake prop?
	switch (idProp)
	{
		case P_ProjItemName:	// yes
			VERIFY(val.LoadString(IDS_EXT_DEPENDENCIES));
			return valid;
	}

	// no, pass on to base class
	return CProjContainer::GetStrProp(idProp, val);
}

BOOL CDependencyContainer::CanAdd ( CSlob *pAddSlob )
{
	// The user cannot add directly to the dependency container

	return FALSE;
}

BOOL CDependencyContainer::CanAct(ACTION_TYPE action)
{
	switch (action)
	{
	// refuse to be inserted into the undo slob since
	// we can disappear spontaneously.
	case act_insert_into_undo_slob:
		return FALSE;

	case act_drag:
	case act_drop:
	case act_delete:
	case act_cut:
	case act_copy:
	case act_paste:
		return FALSE;
	}

	return CProjContainer::CanAct(action);
}

BOOL CDependencyContainer::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	// no pages

	CProjContainer::SetupPropertyPages(pNewSel, FALSE);
	
	if( bSetCaption )
		SetPropertyCaption(CString(MAKEINTRESOURCE(IDS_DEPENDENCIES)));

	return TRUE;
}

BOOL CDependencyContainer::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// go down the project from pContainer to find where we should go.
	if (fToPrj)
	{
		CDependencyContainer * pOldDepCntr;

		if (pContainer->IsKindOf(RUNTIME_CLASS(CProjContainer)))
		{
			pOldDepCntr = ((CProjContainer *)pContainer)->GetDependencyContainer(FALSE);

			// an existing container?
			if (pOldDepCntr == (CSlob *)NULL)
				// no, do the base-class thing
				return CProjContainer::PreMoveItem(pContainer, fFromPrj, fToPrj, fPrjChanged);

			// move into the old container
			pContainer = pOldDepCntr;
		}

		if (pContainer->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
		{
			// move each member into the existing group
			POSITION pos = GetHeadPosition();

			while (pos != (POSITION)NULL)
				GetNext(pos)->MoveInto(pContainer);

			// we're no londer needed...
			pContainer = (CSlob *)NULL;
		}

	}

	// do the base-class thing
	return CProjContainer::PreMoveItem(pContainer, fFromPrj, fToPrj, fPrjChanged);
}

BOOL CDependencyContainer::CanContain(CSlob * pSlob)
{
	if (pSlob->IsKindOf(RUNTIME_CLASS(CDependencyFile)))
		return TRUE;

	return FALSE;
}

void CDependencyContainer::FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem)
{
	if (!(fo & flt_ExcludeDependencies))
	{
		fAddItem = TRUE;
		fAddContent = TRUE;
	}
}

BOOL CDependencyContainer::IsValid(ConfigurationRecord * pcrBase /* = NULL */)
{
	// Get the base config of the active project and compare it to our pcrBase

	// Get the currently active project
	if (pcrBase == NULL)
	{
		CProject * pProject = g_pActiveProject;
		ASSERT(pProject);
		pcrBase = pProject->GetActiveConfig();
	}

 	return (pcrBase == m_pcrBase);
}
