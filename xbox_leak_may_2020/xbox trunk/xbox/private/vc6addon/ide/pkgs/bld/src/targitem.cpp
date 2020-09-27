//
//	CTargetItem
//
//	This is a projitem that represents a target.
//
//	[colint]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "resource.h"
#include "exttarg.h"
#include "bldrfile.h"	// our CBuildFile
#include "depgraph.h"	// dep. graph
#include "oleref.h"

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

	// the Project Window icon inset
	m_nIconIdx = 1;

	m_strTargDir = "";
}

CTargetItem::~CTargetItem()
{
	// delete any un-sent notifications
	EmptyNotifyLists();

	// delete our contained file-items
	DeleteContents();

	// un-register with the global registry
	// must do after DeleteContents() as our content
	// relies on the global reg. having this access

	// FUTURE: we really wanted this in ::PreMoveItem()
	// DeleteContents in there instead?
#ifndef REFCOUNT_WORK
	g_FileRegistry.DisallowAccess(&m_TargetRegistry);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Verb Commands

BOOL CTargetItem::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	// Right now we have no property pages so
	// we can't return one

	// Are we dealing with an external target, since external targets do not
	// have input/output pages
	CProjType * pProjType = GetProjType();
	BOOL bIsExternalTarget = (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)));

#if 1	// REVIEW: bogus
	CString strConfig;
	BOOL bMatchingConfig = g_BldSysIFace.GetTargetNameFromFileSet((HFILESET)(this), strConfig, ACTIVE_BUILDER, TRUE);

	bIsExternalTarget = bIsExternalTarget || !bMatchingConfig;
#endif

	// is this the Target 'General' page?
	AddPropertyPage(&g_TargetGeneralPage, this);

	// is this the v 'Inputs' & 'Outputs' pages?
	if( !bIsExternalTarget )
	{
		AddPropertyPage(&g_InputsPage, this);
		AddPropertyPage(&g_OutputsPage, this);
	}

	CProjContainer::SetupPropertyPages(pNewSel, FALSE);
	
	if( bSetCaption )		// We'll call the properties 'Project'
		SetPropertyCaption(CString(MAKEINTRESOURCE(IDS_PROJECT)));

	return TRUE;
}

BOOL CTargetItem::CanAdd ( CSlob* pSlob )
{
	if ((pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)) || pSlob->IsKindOf(RUNTIME_CLASS(CFileItem))
#if 1
		|| pSlob->IsKindOf(RUNTIME_CLASS(CDependencyFile))
#endif
		)
		&& !g_Spawner.SpawnActive())
		return TRUE;

	return CProjContainer::CanAdd(pSlob);
}

BOOL CTargetItem::CanAct(ACTION_TYPE action)
{
	switch (action)
	{
	// case act_insert_into_undo_slob:
	case act_delete:
	case act_cut:
	case act_copy:
	case act_paste:
		return FALSE;
	}

	return CProjContainer::CanAct(action);
}

BOOL CTargetItem::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	ASSERT(fPrjChanged == FALSE); // not supported
	if (pContainer)
	{
		ASSERT(pContainer->IsKindOf(RUNTIME_CLASS(CProject)));
#if 0
		if (fPrjChanged)
		{
			// cycle through and make sure project has all the valid 
			// configs that we need
			// We loop through the config array to get the config recs
			// for each target.
			ConfigurationRecord* pcrItem;
			CString strName,str;
			const CPtrArray* pConfigArray = GetConfigArray();
			int nSize = pConfigArray->GetSize();
			ASSERT(nSize > 0);
		 	for (int nConfig = 0; nConfig < nSize; nConfig++)
			{
				// Get the next config rec. Is this config rec valid for this
				// target item, if so then we write out a target name comment
				pcrItem = (ConfigurationRecord*)pConfigArray->GetAt(nConfig);
				if (pcrItem->IsValid())
				{
					strName = pcrItem->GetConfigurationName();
					pContainer->
				}
			}
		}
#endif
	}

	// do the base-class thing
	if (!CProjContainer::PreMoveItem(pContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	// moving into the project?
	if (!fFromPrj && fToPrj)
	{
		// register with the global register
#ifndef REFCOUNT_WORK
		g_FileRegistry.AllowAccess(&m_TargetRegistry);
#endif
	}

	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
GPT CTargetItem::GetIntProp(UINT idProp, int& val)
{
	// special prop that we defer to our project
	if ((idProp == P_IPkgProject) && (GetProject() != NULL))
	{
		return GetProject()->GetIntProp(idProp, val);
	}

	return CProjContainer::GetIntProp(idProp, val);
}

GPT CTargetItem::GetStrProp ( UINT idProp, CString &str )
{
#if 0
	if (idProp == P_ProjItemFullPath)
	{
		// Get the project
		CProject* pProject = GetProject();
 
		// We can only get this property if we 
		// a. we are an external target type and we can successfully get
		//    the target name and bsc name, since GetTargetFileName will
		//    cause a config rec refresh.

		CString strTemp;
		const CPath * pPath;

		// Get the target file name
		pPath = pProject->GetFilePath();
		if (pPath != NULL)
		{
			str = pPath->GetFullPath();
			return valid;
		}
		return invalid;
	}
	else
#endif
	if (idProp == P_ProjItemDate)
	{
		// Get the project
		CProject* pProject = GetProject();

		CString strTemp;
		const CPath *pPath;

		// Get the target file name
		pPath = pProject->GetFilePath();
		if (pPath != NULL)
		{
			// If we have a target file name then format its time stamp
#ifndef REFCOUNT_WORK
			FileRegHandle frh = g_FileRegistry.LookupFile(*pPath);
#else
			FileRegHandle frh = CFileRegFile::LookupFileHandleByName(*pPath);
#endif
			if (frh != NULL)
			{
				FILETIME ft;
				CTime time;

				if (g_FileRegistry.GetRegEntry(frh)->GetFileTime(ft))
				{
					time = ft;
					CString strDate = theApp.m_CPLReg.Format(time, DATE_ALL);
					CString strTime = theApp.m_CPLReg.Format(time, TIME_ALL);
					str = strTime + _TEXT(" ") + strDate;
				}
				else
					VERIFY(str.LoadString(IDS_PROJ_TRG_NONEXISTANT));

#ifdef REFCOUNT_WORK
				frh->ReleaseFRHRef();
#endif
				return valid;
			}
		}
		return invalid;
	}
	else if (idProp == P_ProjConfiguration || idProp == P_ProjItemFullPath || idProp == P_SccStatus)
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
	BOOL bNameChanged = FALSE;
	CString strChanged;

	// P_ProjItemName is synonym for P_TargetName
	if (idProp == P_ProjItemName)
		idProp = P_TargetName;

	if (idProp == P_TargetName) 
	{
		// error if this name is blank
		if (str.IsEmpty())
		{
			MsgBox (Error, IDS_NO_EMPTY_TARGET);
			InformDependants(idProp);
			return FALSE;
		}
	}
 
 	bRetval = CProjContainer::SetStrProp (idProp, str);

	return bRetval;
}

// Target name prefix written to the makefile
const TCHAR *CTargetItem::pcNamePrefix	= _TEXT("Name ");

// Write out the target name lines. These consist of the target name
// prefix followed by the target name in quotes
void CTargetItem::WriteTargetNames(CMakFileWriter& mw)
{
	// We loop through the config array to get the config recs
	// for each target.
	ConfigurationRecord* pcrItem;
	CString strName,str;
	const CPtrArray* pConfigArray = GetConfigArray();
	int nSize = pConfigArray->GetSize();
	ASSERT(nSize > 0);
 	for (int nConfig = 0; nConfig < nSize; nConfig++)
	{
		// Get the next config rec. Is this config rec valid for this
		// target item, if so then we write out a target name comment
		pcrItem = (ConfigurationRecord*)pConfigArray->GetAt(nConfig);
		if (pcrItem->IsValid())
		{
			strName = pcrItem->GetConfigurationName();
			if (!mw.IsMakefile())
			{
				mw.WriteCommentEx("%s\"%s\"", pcNamePrefix, (const TCHAR *) strName);
			}
			// REVIEW: this breaks compatibility with what VC 4.0 can read so
			// it is switched on a registry entry which is disabled by default
			else
			{
				// UNDONE: won't be needed in the new scheme
				if (str.IsEmpty())
					str = _TEXT("\"$(CFG)\" == \"");
				else
					str += _TEXT("\" || \"$(CFG)\" == \"");

				str += strName;
			}
		}		
	}

	// UNDONE: won't be needed in the new scheme
	if (mw.IsMakefile())
	{
		// Form string "!IF "$(CFG)" == "DEBUG" || "$(CFG)" == "RELEASE"
		ASSERT(!str.IsEmpty());
		str += _TEXT("\"");
	 	mw.EndLine();
		mw.WriteDirective (CMakDirective::DTYP_IF, str);
	}
}

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
		ASSERT(bTargetRegistered);
	}

	// Force the target into it's own config.
 	ConfigurationRecord * pcrOld = pProject->SetActiveConfig(strName);
	ConfigurationRecord * pcrTarg = GetActiveConfig();

	if (GetTargetName().IsEmpty())
	{
		int index = strName.Find(" - ");
		ASSERT(index > 0);
		CString strTargetName = strName.Left(index);
		SetTargetName(strTargetName);
		if (GetProject()->GetTargetName().IsEmpty())
			GetProject()->SetTargetName(strTargetName);
		ASSERT(strTargetName == GetProject()->GetTargetName());
	}

	// create a build instance
	g_buildengine.AddBuildInst(pcrTarg);

 	// inform the graph of this target that it is being created
	g_buildengine.GetDepGraph(pcrTarg)->SetGraphMode(Graph_Creating);

	// ensure that the target has a matching config. and it is valid
	pcrTarg->SetValid(TRUE);

	// we might not be receiving $IF/$ENDIF blocks and so therefore
	// the current config. should be the target's config.
	// don't reset the project's config.
	// (void) pProject->SetActiveConfig(pcrOld);

	// Register this target with the project target manager.
	if (!bTargetRegistered)
		VERIFY(pProject->RegisterTarget(strName, this));

	CTargetItem *pTarget = pProject->GetTarget(strName);
	ASSERT(pTarget == this);
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

BOOL CTargetItem::WriteToMakeFile
( 
	CMakFileWriter& pw, 
	CMakFileWriter& mw, 
	const CDir &BaseDir,
	CNameMunger& nm
)
{
	POSITION pos;
	const TCHAR *pc = GetNamePrefix (this);
	PFILE_TRACE ("CTargetItem at %p writing to CMakFileWriter at %p\n",
														this, &pw);
	if (g_bWriteProject)
	{
		// write group open comment
		// pw.WriteSeperatorString();
		pw.WriteCommentEx("Begin %s", pc);
	 	pw.EndLine();

		// Write out target name comments
		WriteTargetNames(pw);
	}

	// UNDONE: won't be needed when projects in separate makefiles
	if (g_bExportMakefile)
	{
		WriteTargetNames(mw);
	}

	// Get the project
	CProject * pProject = GetProject();

	CPtrArray CfgArray;

	BOOL bFirstCfg = TRUE;
	ConfigurationRecord * pcr;
	CString strConfigName;
	CProjType * pProjType;
	BOOL bValid;

	m_cp = EndOfConfigs;
	int nSize = m_ConfigArray.GetSize();
	for (int nConfig = 0; nConfig < nSize; nConfig++)
	{
		pcr = (ConfigurationRecord *)m_ConfigArray[nConfig];
		bValid = pcr->IsValid();
 		strConfigName = pcr->GetOriginalTypeName();
		if (g_prjcompmgr.LookupProjTypeByName(strConfigName, pProjType))
		{
			if ((pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget))) && (bValid))
			{
				// UNDONE: bug!!!  Not external usually, but gets here anyway!!

				// set each configuration active in turn
				CProjTempConfigChange projTempConfigChange(pProject);
				projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);

				if (nSize)
				{
					// write IF/ELSEIF if writing multiple configs
					m_cp = (bFirstCfg) ? FirstConfig : MiddleConfig;
					if (g_bWriteProject)
						WriteConfigurationIfDef(pw, GetActiveConfig());
					if (g_bExportMakefile)
						WriteConfigurationIfDef(mw, GetActiveConfig());
				}

				if (g_bExportMakefile)
					WriteExternalTargetBuildRule(mw, BaseDir);

				// reset configuration
				ForceConfigActive();


				bFirstCfg = FALSE;
			}
			else if (bValid)
			{
				CfgArray.Add((ConfigurationRecord *)pcr->m_pBaseRecord);
			}
		}
	}

	if (nSize && m_cp != EndOfConfigs)
	{
		// Write final ENDIF if writing multiple configs
		m_cp = EndOfConfigs;
		if (g_bWriteProject)
			WriteConfigurationIfDef(pw, NULL);
		if (g_bExportMakefile)
			WriteConfigurationIfDef(mw, NULL);
	}

	// Ok normal target (i.e. not an external target), so we write out the
	// per-target settings
	if (CfgArray.GetSize() > 0 && !g_buildfile.WriteToolForNMake(this, &nm, &CfgArray))
		return FALSE;	// failed to write tools section for this target
 
	// this will cause our content to get written in a predictable order
	SortContentList();

	for (pos = GetHeadPosition (); pos != NULL;)
		if (!((CProjItem *) GetNext (pos))->WriteToMakeFile(pw, mw, BaseDir, nm))
			return FALSE;	// failed to write out target content

	if (g_bExportMakefile)
	{
	 	mw.EndLine();
		mw.WriteDirective (CMakDirective::DTYP_ENDIF, NULL);
		mw.EndLine ();
	}

	// UNDONE: won't be needed when projects in separate makefiles
	if (g_bWriteProject)
	{
		// write target closing comment
		pw.WriteCommentEx ( "%s %s", EndToken, pc );
	}

	return TRUE;	// success
}

BOOL CTargetItem::ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir)
{
//	Read in this item and any childred from a makfile reader.  The make
//  comment is the first line of our stuff:
//
	CObject *pObject = NULL;
	CString str;

	PFILE_TRACE ("CTargetItem at %p reading from CMakFileWriter at %p\n",
														this, &mr);

	TRY
    {
		pObject = mr.GetNextElement ();
		if (!pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)))
			AfxThrowFileException (CFileException::generic);

		m_cp = EndOfConfigs;
 		delete pObject; pObject = NULL;

		for (; pObject = mr.GetNextElement ();)
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

		CProjContainer * pContainer = this;
		CProject * pProject = GetProject();
		if (pProject->m_bConvertedDS4x || (pProject->m_bProjConverted && (!pProject->m_bConvertedVC20) && (!pProject->m_bConvertedDS5x)))
		{
			CString strName, strFilter;
			strName.LoadString(IDS_SOURCE_FILES);

			CProject *pProj = (CProject *) g_BldSysIFace.GetActiveBuilder ();
			CProjType * pProjType = pProj->GetProjType();

			if (pProjType && pProjType->GetUniqueTypeId() == CProjType::javaapplet)
				strFilter.LoadString(IDS_JAVA_SOURCE_FILES_FILTER);
			else
				strFilter.LoadString(IDS_SOURCE_FILES_FILTER);
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

BOOL CTargetItem::WriteExternalTargetBuildRule
(
	CMakFileWriter & mw, 
	const CDir & BaseDir
)
{
	// Get the project
	CProject * pProject = GetProject();

	// Do the targets : dependents line:
	CString strTargets;
	CObList lstItem; lstItem.AddHead(this);

	MakeQuotedString
	(
		GetTargetPaths(),
		strTargets,
		&BaseDir,
		TRUE,
		NULL,
		NULL, 
 		NULL,TRUE,
 		(const CObList *)&lstItem	
		,FALSE //not sorted
 	);

	mw.WriteDesc(strTargets, _T(""));

	// Write out a CD command to change directory
	CString strDir;
	//VERIFY(pProject->GetStrProp(P_Proj_WorkingDir, strDir));
	strDir = (const TCHAR *)pProject->GetProjDir();
	mw.Indent();
	mw.WriteString (_T("CD ") + strDir);
	mw.EndLine();

	// Write out the actual build command line
 	CString strCmdLine;
	pProject->GetStrProp(P_Proj_CmdLine, strCmdLine);
	mw.Indent ();
	mw.WriteString (strCmdLine);
	mw.EndLine ();

	return TRUE;
}
 
// FUTURE: remove these
CFileRegSet * CTargetItem::GetTargetPaths()
{
	return GetProject()->GetActiveConfig()->GetTargetFilesList();
}
CFileRegSet * CTargetItem::GetOutputPaths()
{
	// This call should not be used.
	ASSERT(0);
	return NULL;
//	return GetProject()->GetActiveConfig()->GetOutputFilesList();
}
// FUTURE: remove these

BOOL CTargetItem::CanContain(CSlob* pSlob)
{
	if (pSlob->IsKindOf(RUNTIME_CLASS(CFileItem)) ||
	    pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)) ||
	    pSlob->IsKindOf(RUNTIME_CLASS(CDependencyContainer)) ||
#if 0
		// converted to a CFileItem later
	    pSlob->IsKindOf(RUNTIME_CLASS(CDependencyFile)) ||
#endif
		pSlob->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		return TRUE;

	return FALSE;
}

void CTargetItem::FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem)
{
	// Exclude targets from the build
	if (fo & flt_RespectTargetExclude)
	{
		ConfigurationRecord * pcr = GetActiveConfig();
		if (pcr && pcr->IsValid())
		{
			fAddItem = FALSE;
			fAddContent = TRUE;
		}
	}
	else
	{
		fAddItem = FALSE;
		fAddContent = TRUE;
	}
}

void CTargetItem::PrepareAddList(CMultiSlob* pAddList, BOOL bPasting)
{
	CMapPtrToPtr mapFileRegs;
	CProjItem * pItem, *pDupItem;
	CObList * pAddListContents = pAddList->GetContentList();
 
 	FileRegHandle hndFileReg;
	CObList * pTargetContents = GetContentList();
	// REVIEW
	
 	POSITION pos = pTargetContents->GetHeadPosition();
	while (pos != NULL)
	{
		pItem = (CFileItem *) pTargetContents->GetNext(pos);
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
			else
			{
				mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
			}
		}
		else if (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			// no problem!
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

void CTargetItem::AddToNotifyList(FileRegHandle frh, BOOL bRemove, BOOL fDepFile /* = FALSE */)
{
	CProject * pProject = GetProject();

	// No notifications are sent during project loading
	// 'cos we'll send out a new PN_NEW_PROJECT / PN_OPEN_PROJECT inform
	if (pProject != (CProject *)NULL && !pProject->m_bProjectComplete)
		return;

    NOTIFYINFO * pNotify, * pCheck;
	UINT id;

	// Are we deleting a file, or adding a file
    if (bRemove)
    {
        pNotify = &m_DelNotifyInfo;
        pCheck = &m_AddNotifyInfo;
		id = PN_DEL_FILE;
    }
    else
    {
        pNotify = &m_AddNotifyInfo;
        pCheck = &m_DelNotifyInfo;
		id = PN_ADD_FILE;
    }

	CPath * pPath = new CPath;
	*pPath = *g_FileRegistry.GetRegEntry(frh)->GetFilePath();

	// Optimize out the case when we add a file that is on our
	// deletion notification list, and vice versa.
    POSITION pos = pCheck->lstFiles.Find(pPath);
    if (pos)
    {
        CPath* pCheckPath = pCheck->lstFiles.GetAt(pos);
        delete pCheckPath;
		delete pPath;
 		pCheck->lstFiles.RemoveAt(pos);
        return;
    }

    // Add the file to the appropriate notification list
	if (fDepFile)
		pNotify->lstDepFiles.AddTail(pPath);
	else
		pNotify->lstFiles.AddTail(pPath);

	// does the project has batching turned on
	BOOL bEmptyLists = TRUE;
	if (pProject != (CProject *)NULL)
		bEmptyLists = !pProject->IsNotifyBatching();

	// Empty the notification lists if we are not batching or the project
	// has batching turned on.    
	if (bEmptyLists)
		EmptyNotifyLists();
}

void CTargetItem::EmptyNotifyLists()
{
	CPath * pPath;
	POSITION pos;
	COleRef<IPkgProject> pPkgProject = NULL;
	VERIFY(SUCCEEDED(g_BldSysIFace.GetPkgProject((HBUILDER)GetProject(), &pPkgProject)));

	// Empty our file change notification lists
	if (!m_AddNotifyInfo.lstFiles.IsEmpty() ||
		!m_AddNotifyInfo.lstDepFiles.IsEmpty())
	{
// make a local copy in case notify results in more batched notifies or another EndBatch()
		NOTIFYINFO notifyInfo;
		notifyInfo.hFileSet = (HFILESET)this;
		notifyInfo.pPkgProject = pPkgProject;

		notifyInfo.lstFiles.AddTail(&m_AddNotifyInfo.lstFiles);
        m_AddNotifyInfo.lstFiles.RemoveAll();
		notifyInfo.lstDepFiles.AddTail(&m_AddNotifyInfo.lstDepFiles);
        m_AddNotifyInfo.lstDepFiles.RemoveAll();

		theApp.NotifyPackages(PN_ADD_FILE, (void *)&notifyInfo);

		pos = notifyInfo.lstFiles.GetHeadPosition();
        while (pos != NULL)
        {
            pPath = notifyInfo.lstFiles.GetNext(pos);
            delete pPath;
        }
		pos = notifyInfo.lstDepFiles.GetHeadPosition();
        while (pos != NULL)
        {
            pPath = notifyInfo.lstDepFiles.GetNext(pos);
            delete pPath;
        }
   }

	if (!m_DelNotifyInfo.lstFiles.IsEmpty() ||
		!m_DelNotifyInfo.lstDepFiles.IsEmpty())
	{
		NOTIFYINFO notifyInfo;
		notifyInfo.hFileSet = (HFILESET)this;
		notifyInfo.pPkgProject = pPkgProject;
	
		notifyInfo.lstFiles.AddTail(&m_DelNotifyInfo.lstFiles);
        m_DelNotifyInfo.lstFiles.RemoveAll();
		notifyInfo.lstDepFiles.AddTail(&m_DelNotifyInfo.lstDepFiles);
        m_DelNotifyInfo.lstDepFiles.RemoveAll();

	 	theApp.NotifyPackages(PN_DEL_FILE, (void *)&notifyInfo);

		pos = notifyInfo.lstFiles.GetHeadPosition();
        while (pos != NULL)
        {
            pPath = notifyInfo.lstFiles.GetNext(pos);
            delete pPath;
        }
		pos = notifyInfo.lstDepFiles.GetHeadPosition();
        while (pos != NULL)
        {
            pPath = notifyInfo.lstDepFiles.GetNext(pos);
            delete pPath;
        }
	}
}


BOOL CTargetItem::IsTargetReachable(CTargetItem * pTarget)
{
	BOOL bReachable = FALSE;

#if 0
	CObList ol;
	FlattenSubtree(ol, flt_Normal | flt_ExcludeGroups | flt_RespectItemExclude | 
				   flt_ExcludeDependencies);
	POSITION pos = ol.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CProjItem* pItem = (CProjItem*)ol.GetNext(pos);

		// If this is a project dependency check its name
 		if (pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		{
 			CString strProject;
			CTargetItem * pDepTarget = ((CProjectDependency *)pItem)->GetProjectDep();

			if (pDepTarget == NULL)
				continue;  // ignore if not a loaded project
 			
			// Have we found the project
			if (pDepTarget == pTarget)
			{
				bReachable = TRUE;
				break;
			}

  			bReachable = bReachable || pDepTarget->IsTargetReachable(pTarget);
		}
	}
#else
	CString strTop; 
	CString strSeek; 
	
	GetProject()->GetName(strTop);
	pTarget->GetProject()->GetName(strSeek);
	bReachable = ::IsReachable( strTop, strSeek );
#endif

	return bReachable;	
}


BOOL CTargetItem::ContainsTarget(CTargetItem * pTarget)
{
	CObList ol;
	FlattenSubtree(ol, flt_Normal | flt_ExcludeGroups | flt_RespectItemExclude | 
				   flt_ExcludeDependencies);
	POSITION pos = ol.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CProjItem* pItem = (CProjItem*)ol.GetNext(pos);

		// If this is a target reference check its name
 		if (pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		{
  			CTargetItem * pDepTarget = ((CProjectDependency *)pItem)->GetProjectDep();
 
			// The names are the same we've found the dependency
			if (pTarget == pDepTarget)
 				return TRUE;				 
  		}
	}

	// No dependencies of the given name
	return FALSE;
}

BOOL CTargetItem::IsProjectFile(LPCTSTR pszFile)
{
	BOOL bRetVal = FALSE;
	CFileRegistry * pRegistry = GetRegistry();
	FileRegHandle frh = pRegistry->LookupFile(pszFile);
	if (frh != NULL)
	{
		bRetVal = IsFileInTarget(frh,TRUE);
		frh->ReleaseFRHRef();
		frh = NULL;
	}
	return bRetVal;
}


BOOL CTargetItem::IsFileInTarget( FileRegHandle frh, BOOL bAsFileItem /*= FALSE */)
{
	BOOL bRet = FALSE;

	if (frh != NULL)
	{
		CFileRegistry * pRegistry = GetRegistry();
		ASSERT(NULL!=pRegistry);

		// Check target registry.
		if ( !pRegistry->RegHandleExists(frh) )
			return FALSE;

		if (bAsFileItem)
		{
			CObList* pFileItemList = pRegistry->GetRegEntry(frh)->plstFileItems;

			if (NULL!=pFileItemList)
			{
				POSITION pos = pFileItemList->GetHeadPosition();
				while (NULL!=pos)
				{
					CFileItem* pFileItem = (CFileItem*)pFileItemList->GetNext(pos);
					ASSERT(NULL!=pFileItem);
					ASSERT(pFileItem->IsKindOf(RUNTIME_CLASS(CFileItem)));
					if (pFileItem->GetTarget()==this)
					{
						bRet = TRUE;
						break;
					}
				}
			}
		}
		else
			bRet = TRUE;
	}
	return bRet;
}
