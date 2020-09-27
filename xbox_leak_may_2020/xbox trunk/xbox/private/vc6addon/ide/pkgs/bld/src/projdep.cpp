///////////////////////////////////////////////////////////////////////////////
//
//	CProjectDependency
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bldrfile.h"	// our CBuildFile
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

CPtrList g_lstprojdeps;


void InitWorkspaceDeps( void ){
	POSITION pos = g_lstprojdeps.GetHeadPosition();
	while (pos != NULL)
	
	{
		POSITION posSave = pos;
		CTargetItem * pTargDep = NULL;
		
		CProjDepInfo * pProjDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		HFILESET hFileSet = g_BldSysIFace.GetFileSetFromTargetName(pProjDepInfo->strTarget, ACTIVE_BUILDER /* check them all */);
		if (hFileSet) // UNDONE: could be missing
		{
			HBUILDER hBld = g_BldSysIFace.GetBuilderFromFileSet(hFileSet);
			pTargDep = g_BldSysIFace.CnvHFileSet(hBld, hFileSet);
		}
		
		if( pProjDepInfo->pProjDep == NULL ){
			CProject *pProj = (CProject *) g_BldSysIFace.GetBuilderFromName(pProjDepInfo->strProject);
			if( pProj ) {
				CTargetItem * pTarget = pProj->GetActiveTarget();
				pProjDepInfo->pProjDep = new CProjectDependency();
				// remove from the list if found

				pProj->SetOkToDirtyProject(FALSE);
				pProjDepInfo->pProjDep->SetProjectDep(pTargDep, pProjDepInfo->strTarget);
				pProjDepInfo->pProjDep->MoveInto(pTarget);
				pProj->SetOkToDirtyProject(TRUE);
			}
		} else {
			// if found this one should already exisit in the project
			// REVIEW(KiP:9/96): is this needed ?
			pProjDepInfo->pProjDep->SetProjectDep(pTargDep, pProjDepInfo->strTarget);
		}
	}
}

CProjDepInfo * AddDep( CString strTop, CString strSub, CProjectDependency *pProjectDep ){
	POSITION pos = g_lstprojdeps.GetHeadPosition();
	BOOL bFound = FALSE;
	while (pos != NULL)
	{
		CProjDepInfo *pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->strProject == strTop && pDepInfo->strTarget == strSub ){
			pDepInfo->pProjDep = pProjectDep;
			bFound = TRUE;
			break;
		}
	}
	if( bFound == FALSE ){
		CProjDepInfo *pNewDep = new CProjDepInfo;
		pNewDep->pProjDep = pProjectDep;
		pNewDep->strTarget = strSub;
		pNewDep->strProject = strTop;
		g_lstprojdeps.AddTail(pNewDep);
		return pNewDep;
	}
	return NULL;
}

CProjDepInfo * AddDep( CProjectDependency *pProjectDep ){
	// Could be coming from the Undo slob. If so, it wont be in the list.
	CProjDepInfo *pDepInfo;
	CString strProject;
	CString strSub;
	pProjectDep->GetProject()->GetName(strProject);
	pProjectDep->GetStrProp(P_ProjItemName, strSub);

	POSITION pos = g_lstprojdeps.GetHeadPosition();
	BOOL bFound = FALSE;
	while (pos != NULL)
	{
		pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->pProjDep == pProjectDep ){
			bFound = TRUE;
			break;
		}
	}
	if( bFound == FALSE ){
		pDepInfo = new CProjDepInfo;
		pDepInfo->pProjDep = pProjectDep;
		pDepInfo->strTarget = strSub;
		pDepInfo->strProject = strProject;
		g_lstprojdeps.AddTail( pDepInfo );
		return pDepInfo;
	}
	return NULL;
}

BOOL RemoveDep( CString strTop, CString strSub ){
	POSITION oldPos;
	POSITION pos = g_lstprojdeps.GetHeadPosition();
	while (pos != NULL)
	{
		oldPos = pos;
		CProjDepInfo *pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->strProject == strTop && pDepInfo->strTarget == strSub ){
			g_lstprojdeps.RemoveAt( oldPos );
			delete pDepInfo;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL RemoveAllReferences( CString strProj ) {
	POSITION oldPos;
	POSITION pos = g_lstprojdeps.GetHeadPosition();
	while (pos != NULL)
	{
		oldPos = pos;
		CProjDepInfo *pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->strProject == strProj || pDepInfo->strTarget == strProj ){
			g_lstprojdeps.RemoveAt( oldPos );
			delete pDepInfo;
		}
	}
	return FALSE;
}

BOOL RemoveDep(  CProjectDependency *pProjDep  ){
	POSITION oldPos;
	POSITION pos = g_lstprojdeps.GetHeadPosition();
	while (pos != NULL)
	{
		oldPos = pos;
		CProjDepInfo *pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->pProjDep == pProjDep ){
			g_lstprojdeps.RemoveAt( oldPos );
			delete pDepInfo;
			return TRUE;
		}
	}
	return FALSE;
}

void GetSubProjects( CString &strTop, CStringList &listSub ){
	POSITION pos = g_lstprojdeps.GetHeadPosition();
	while (pos != NULL)
	{
		CProjDepInfo *pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->strProject == strTop  ){
			listSub.AddTail(pDepInfo->strTarget);
		}
	}
}

	 
BOOL IsReachable(CString &strTop, CString &strSeek) {
	BOOL bReachable = FALSE;

	CStringList subProjects;
	
	GetSubProjects(strTop, subProjects);

	// REVIEW(KiP:9/96) brute force approach. Significantly WORSE than N^2. fix if too slow.
	while ( !subProjects.IsEmpty() ) {
		CString strSub = subProjects.RemoveHead();
		if( strSub == strSeek ){
			bReachable = TRUE;
			return bReachable;	
		}
		GetSubProjects(strSub, subProjects);
	};
	return bReachable;	
}

BOOL CProjectDependency::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// WARNING! This does not fall through to a message map.  You must
	// handle both CN_COMMAND and CN_UPDATE_COMMAND_UI if you want your
	// commands to be enabled.

	if (nCode == CN_COMMAND && pExtra == NULL)
	{
		if (nID == IDM_PROJITEM_OPEN)
		{
			OnOpen();
			return TRUE;
		}
	}
	else if (nCode == CN_UPDATE_COMMAND_UI)
	{
		ASSERT(pExtra != NULL);
		CCmdUI* pCmdUI = (CCmdUI*)pExtra;
		ASSERT(pCmdUI->m_nID == nID);           // sanity assert

		if (nID == IDM_PROJITEM_OPEN)
		{
			pCmdUI->Enable(TRUE);
			return TRUE;
		}
	}

	// pass on to the default handler
	return CProjItem::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

CProjectDependency::CProjectDependency() 
{
	// the Project Window item inset
	m_nIconIdx = 5;
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
	POSITION oldPos;
	POSITION pos = g_lstprojdeps.GetHeadPosition();
	while (pos != NULL)
	{
		oldPos = pos;
		CProjDepInfo *pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
		if( pDepInfo->pProjDep == this ){
			pDepInfo->pProjDep = NULL;
			break;
		}
	}
}

BOOL CProjectDependency::SetStrProp(UINT idProp, const CString &str)
{
	// pass on to base-class
	/*
	switch (idProp)
	{
		case P_ProjItemName:	// yes
			str = m_strName;
			return TRUE;
 		default:
			break;
	}
	*/

	return CProjItem::SetStrProp(idProp, str);
}
  
GPT CProjectDependency::GetStrProp(UINT idProp, CString& val)
{
	// is this a fake prop?
	
	switch (idProp)
	{
		case P_ProjItemName:	// yes
				if (m_pTarget != NULL)
				{
#if 0
					g_BldSysIFace.GetFileSetName((HFILESET)m_pTarget, val, (HBUILDER)m_pTarget->GetProject());
 					m_strName = val;
#else
					m_strName = val = m_pTarget->GetTargetName();
#endif
				}
				else
				{
					val = m_strName;  // as read from project file
				}
				return valid;
		default:
			break;
	}
	
 	// no, pass on to base class
	return CProjItem::GetStrProp(idProp, val);
}

// Target name prefix written to the makefile
const TCHAR *CProjectDependency::pcDepPrefix = _TEXT("Project_Dep_Name ");
 
BOOL CProjectDependency::WriteToMakeFile
( 
	CMakFileWriter& pw, 
	CMakFileWriter& mw, 
	const CDir &BaseDir,
	CNameMunger& nm
)
{
	CProject * pProject = GetProject();

	// FUTURE (karlsi): remove BaseDir and just use CProject::GetProjDir()
	ASSERT(BaseDir==pProject->GetWorkspaceDir());
	
	CString strSrc, str, strOut;
	BOOL bNamedTool= FALSE;
	CBuildTool * pBuildTool = NULL;
	const TCHAR *pc = GetNamePrefix (this);

	PFILE_TRACE ("CProjectDependency \"%s\" at %p writing to CMakFileWriter at %p\n",
				(const TCHAR *)	*GetFilePath(),this, &pw);
	
	TRY			  
	{
		if (g_bExportMakefile)
		{
#ifdef _DEBUG
			const ConfigurationRecord * pcrSave = GetActiveConfig();
			const ConfigurationRecord * pcrSaveBase = pcrSave->m_pBaseRecord;
#endif

			int cCFGs = 0;
			ConfigurationRecord * pcr;

			const CPtrArray & ppcr = *GetConfigArray();
			CProjTempConfigChange projTempConfigChange(pProject);

			for (int k = 0; k < ppcr.GetSize(); k++)
			{
				pcr = (ConfigurationRecord *)ppcr[k];

				projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
 
				// Only write out sections for the valid configs
				if (GetTarget()->GetActiveConfig()->IsValid())
				{	
					// write IF/ELSEIF if writing multiple configs
					m_cp = (cCFGs == 0) ? FirstConfig : MiddleConfig;
					if (g_bExportMakefile)
						WriteConfigurationIfDef(mw, GetActiveConfig());

					// increment the # configs we write out
					cCFGs++;
					WriteRule(FALSE,mw,BaseDir);
					WriteRule(TRUE,mw,BaseDir);
				}
				
 			}

			projTempConfigChange.Release();
			
			// Write final ENDIF if writing multiple configs (if we need to)
			if (cCFGs != 0)
			{
				m_cp = EndOfConfigs;
				if (g_bExportMakefile)
					WriteConfigurationIfDef(mw, NULL);
			}
			
#ifdef _DEBUG
			ASSERT(pcrSave == GetActiveConfig());
			ASSERT(pcrSaveBase == pcrSave->m_pBaseRecord);
#endif
		}

	}
	CATCH(CException, e)
	{
		return FALSE;	// failure
	}
	END_CATCH

	return TRUE;	// success
}



void CProjectDependency::WriteRule(
	BOOL bClean,
	CMakFileWriter& mw, 
	const CDir &BaseDir
	)
{
	CProject * pProject = GetProject();
  	ConfigurationRecord * pcrProjDep = GetTargetConfig();
	if (pcrProjDep != NULL)
	{
		CString strTargetRefName = pcrProjDep->GetConfigurationName();
		// Write out pseudo target dependancy

		CString strPseudoTarg;
		if( bClean )
			strPseudoTarg = _T('\"') + strTargetRefName + _T("CLEAN") + _T('\"');
		else
			strPseudoTarg = _T('\"') + strTargetRefName + _T('\"');
		
		if (g_bExportMakefile)
			mw.WriteDesc(strPseudoTarg, _T(""));

		CPath Path = *(m_pTarget->GetProject()->GetFilePath());
		CDir Dir = m_pTarget->GetProject()->GetWorkspaceDir();
		TCHAR DriveW = ((LPCTSTR)pProject->GetWorkspaceDir())[0];
		TCHAR DriveP = ((LPCTSTR)Dir)[0];
		BOOL bWriteDrive = ((DriveP != DriveW) && (DriveP != _T('\\')));
		if (g_bExportMakefile)
		{
			CString strMakefileName;
			Path.ChangeExtension(_T(".mak"));
			if (bWriteDrive)
			{

				CString strPushD = DriveP;
				strPushD += _T(":");
				mw.Indent ();
				mw.WriteString(strPushD);
				mw.EndLine ();
			}
			CString strBuildLine0 = _T("cd ");


			// relativise sub project dir
			CPath dummyPath;
			dummyPath.CreateFromDirAndRelative(Dir,".\\dummy$$$" );
			CString strRel;
			dummyPath.SetAlwaysRelative();
			dummyPath.GetRelativeName(BaseDir, strRel, FALSE,TRUE);
			strRel = strRel.Left( strRel.GetLength() - 9 );
			strBuildLine0 += "\"" + strRel + "\"";

			mw.Indent ();
			mw.WriteString(strBuildLine0);
			mw.EndLine ();

			CString strBuildLine;
			CProjType * pProjType = m_pTarget->GetProject()->GetProjType();
			if( pProjType->GetUniqueTypeId() != exttarget ){
				Path.GetRelativeName(Dir, strMakefileName, TRUE /* quotes */);
				if( bClean )
					strBuildLine = _T("$(MAKE) /$(MAKEFLAGS) /F ") + strMakefileName + _T(" CFG=\"") + strTargetRefName + _T("\" RECURSE=1 CLEAN ");
				else
					strBuildLine = _T("$(MAKE) /$(MAKEFLAGS) /F ") + strMakefileName + _T(" CFG=\"") + strTargetRefName + _T("\" ");
				// mw.EndLine ();
			} else {
				// get the command line
				if( bClean )
					strBuildLine = _T("");
				else {
					CProjTempConfigChange projTempConfigChange(m_pTarget->GetProject());
					projTempConfigChange.ChangeConfig(pcrProjDep);
					m_pTarget->GetProject()->GetStrProp (P_Proj_CmdLine, strBuildLine);
				}
			}
			if( !strBuildLine.IsEmpty() ) {
				mw.Indent ();
				mw.WriteString(strBuildLine);
				mw.EndLine ();
			}

			if ((bWriteDrive) && (DriveW != _T('\\')))
			{

				CString strPopD = DriveW;
				strPopD += _T(":");
				mw.Indent ();
				mw.WriteString(strPopD);
				mw.EndLine ();
			}

			CString strBuildLine1 = _T("cd ");

			// relativise calling project dir
			strRel = "";
			dummyPath.SetAlwaysRelative();
			dummyPath.CreateFromDirAndRelative(pProject->GetWorkspaceDir(),".\\dummy$$$" );
			dummyPath.GetRelativeName(Dir, strRel, FALSE);
			strRel = strRel.Left( strRel.GetLength() - 9 );
			strBuildLine1 += "\"" + strRel + "\"";

			mw.Indent ();
			mw.WriteString(strBuildLine1);
			mw.EndLine ();

		}
	}
}



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
	PFILE_TRACE ("CTargetReference at %p reading from CMakFileReader at %p\n",
														this, &mr);
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
	::SetWorkspaceDocDirty();

	return TRUE;	// success
}

// Opening up a target reference opens up the actual target
// for v3 this means switching active config. to that of the target
void CProjectDependency::OnOpen()
{
    // FUTURE (colint): do we need to do something more appropriate here
    MessageBeep(0);
}

// Return a container-less, symbol-less, clone of 'this'
CSlob* CProjectDependency::Clone()
{
	CSlob * pClone = CProjItem::Clone(); // call base class to create clone
	ASSERT(pClone->IsKindOf (RUNTIME_CLASS (CProjItem)));
	ASSERT(((CProjItem *)pClone)->GetFileRegHandle()==GetFileRegHandle());
	
	// Set up the clones target reference
	CString strTarget;
	VERIFY(GetStrProp(P_TargetRefName, strTarget));
	VERIFY(pClone->SetStrProp(P_TargetRefName, strTarget));

 	return pClone;
}

BOOL CProjectDependency::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	BOOL bRefreshName = FALSE;

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
		
		if( ::AddDep( this ) != NULL ){
			::SetWorkspaceDocDirty();
		}
	}
	if( pContainer == NULL ){
		// it is being moved into the Undo slob at this point
		::SetWorkspaceDocDirty();

		// find it on the list and remove it.
		::RemoveDep( this );
	}

	// do the base-class thing
	return CProjItem::PostMoveItem(pContainer, pOldContainer, fFromPrj, fToPrj, fPrjChanged);
}

ConfigurationRecord * CProjectDependency::GetTargetConfig()
{
	if (m_pTarget==NULL)
		return NULL;

	CString str;
	CProject* pProject = m_pTarget->GetProject();
	g_BldSysIFace.GetTargetNameFromFileSet((HFILESET)m_pTarget, str, (HBUILDER)GetProject(), TRUE);

	ConfigurationRecord * pcr = pProject->ConfigRecordFromConfigName(str);

	return pcr;
}

ConfigurationRecord * CProjectDependency::GetCompatibleTargetConfig(ConfigurationRecord * pcr)
{
	if (m_pTarget==NULL)
		return NULL;

	CString strName;
	CProject* pProject = m_pTarget->GetProject();
	ConfigurationRecord *pcrPrj = pProject->GetActiveConfig();
	pcrPrj->GetProjectName(strName);

	CString str;

	HBLDTARGET hTarget = g_BldSysIFace.GetActiveTarget ((HBUILDER) pProject);
	HFILESET hFileSet = g_BldSysIFace.GetFileSet ((HBUILDER) pProject, hTarget);
	if (hFileSet == NO_FILESET)
		return NULL;

	g_BldSysIFace.GetTargetNameFromFileSet (hFileSet, str, ACTIVE_BUILDER);

	pcr = pProject->ConfigRecordFromConfigName(str);

	return pcr;
}

///////////////////////////////////////////////////////////////////////////////
void CProjectDependency::FlattenQuery(int fo, BOOL& bAddContent, BOOL& fAddItem)
{
	if (fo & flt_ExcludeProjDeps)
		fAddItem = FALSE;
	else
		fAddItem = TRUE;
}

BOOL CProjectDependency::CanAct(ACTION_TYPE action)
{
	switch (action)
	{
	// refuse to be inserted into the undo slob since
	// we can disappear spontaneously.
	case act_insert_into_undo_slob:
		return TRUE;

	case act_delete:
		return (!g_Spawner.SpawnActive());

	case act_drag:
	case act_cut:
	case act_copy:
	case act_paste:
		return FALSE;
	}

	return CProjItem::CanAct(action);
}

BOOL CProjectDependency::PreAct(ACTION_TYPE action)
{
	if (action == act_delete)
	{
		return (!g_Spawner.SpawnActive());
	}

	return CProjItem::PreAct(action);
}

