//
//	PROJECT.CPP
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop

#include "projdep.h"
#include "project.h"
#include "targitem.h"
#include "bldrfile.h"	// our CBuildFile
#include "optncplr.h"
#include "optnmtl.h"		// so we can automatically add /mktyplib203
#include "optnrc.h"
#include "optnlib.h"
#include "optnbsc.h"
#include "dirmgr.h"

IMPLEMENT_SERIAL (CProject, CProjContainer, SLOB_SCHEMA_VERSION)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CProjComponentMgr *g_pPrjcompmgr;
UINT g_nNextIncompatibleVersion = 700;
extern CBuildPackage * g_pBuildPackage; // defined in prjconfig.cpp
extern CBuildTool *g_pLastTool;  // defined in projitem.cpp

// default extension for builder file
#define BUILDER_EXT	".dsp"

// REVIEW: find a better home for these

CProject * g_pConvertedProject = NULL;
BOOL g_bInProjClose=FALSE;

///////////////////////////////////////////////////////////////////////////////
//
// --------------------------------- CProject ---------------------------------
//
///////////////////////////////////////////////////////////////////////////////
//
//	Top level project class.  May be more than one if there are subprojects.
//

#define theClass CProject
BEGIN_SLOBPROP_MAP(CProject, CProjContainer)
	STR_PROP(ProjActiveConfiguration)
	STR_PROP (ProjItemName)
END_SLOBPROP_MAP()
#undef theClass

BOOL ConvertPropPaths(CProjItem * pItem, UINT idIncludes, BOOL bIsDir = TRUE, BOOL bSubstComma = TRUE)
{
	BOOL retval = FALSE;
	CPath path;
	CDir dir;
	path.SetAlwaysRelative();
	CString strIncludes;
	if ((pItem->GetStrProp(idIncludes, strIncludes) == valid) && (!strIncludes.IsEmpty()))
	{
		// convert include paths
		BOOL bChanged = FALSE;
		CString strNewIncludes;
		LPTSTR pOldInclude = strIncludes.GetBuffer(0);
		LPTSTR pszInclude = _tcstok(pOldInclude, _T(",;"));
		while (pszInclude != NULL && *pszInclude != _T('\0'))
		{
			CString strOldPath = pszInclude;
			CString strNewPath;
			BOOL bRelative = ((*pszInclude != _T('\\')) && (*pszInclude != _T('/'))	&& (pszInclude[1] != _T(':')));
			// try to re-relativize this path;
			if (bRelative && (!bIsDir) && (_tcspbrk(pszInclude, _T("\\/"))==NULL))
			{
				// REVIEW: don't treat simple filenames as relative
				bRelative = FALSE;
			}
			BOOL bUsesMacro = (_tcspbrk(pszInclude, _T("$")) != NULL);
			if (bRelative && (!bUsesMacro))
			{
				if (bIsDir)
				{
					LPCTSTR pchLast = _tcsdec(pszInclude, pszInclude + _tcslen(pszInclude));
					if (*pchLast != _T('\\') && *pchLast != _T('/'))
						strOldPath += _T('\\');
					strOldPath += _T('a'); // dummy file name
				}
				if ((path.CreateFromDirAndFilename(g_pConvertedProject->GetWorkspaceDir(), strOldPath)) && (path.GetRelativeName(pItem->GetProject()->GetWorkspaceDir(), strNewPath)) && (strNewPath != strOldPath))
				{
					bChanged = TRUE;
					if (bIsDir)
					{
						int newlen = strNewPath.GetLength() - 2; // remove '\\a" again
						VSASSERT(newlen > 0, "Dummy path went back to empty!");
						strNewIncludes += strNewPath.Left(newlen);
					}
					else
					{
						strNewIncludes += strNewPath;
					}
				}
				else
				{
					strNewIncludes += pszInclude;
				}
			}
			// validate full paths too to avoid disappointment
			else if (bIsDir && (!bUsesMacro) && (strOldPath.GetLength() > 2 /* avoid 'c:' and '\' cases */) && (dir.CreateFromString(strOldPath)))
			{
				// VC6 #26261: make sure we don't add gratuitous drive letter
				if (_tcsnicmp((LPCTSTR)dir, (LPCTSTR)g_pConvertedProject->GetWorkspaceDir(), 3)==0)
				{
					strNewPath = LPCTSTR(dir) + 2; // strip off drive letter
				}
				else
				{
					strNewPath = dir;
				}
				bChanged =  bChanged || (strOldPath.CompareNoCase(strNewPath) != 0);
				strNewIncludes += strNewPath;
			}
			else
			{
				strNewIncludes += pszInclude;
			}
			
			pszInclude = _tcstok(NULL, _T(",;"));
			if (pszInclude != NULL)
			{
				if (bSubstComma)
					strNewIncludes += _T(',');
				else
					strNewIncludes += _T(';');
			}
		}
		
		strIncludes.ReleaseBuffer(-1);
		if (bChanged)
		{
			retval = TRUE;
			pItem->SetStrProp(idIncludes, strNewIncludes);
		}
	}
	return retval;
}

BOOL CProject::ConvertDirs()
{
	BOOL retval = FALSE;
	VSASSERT(m_bConvertedVC20 || m_bConvertedDS4x, "Should only be here for 2.0 and 4.X conversions");
	if (m_bConvertedVC20 || m_bConvertedDS4x)
	{
		COptionHandler * popthdlr = NULL;
		g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Compiler), (CBldSysCmp *&)popthdlr);
		if( popthdlr == NULL )
		    return retval;

		UINT idCplrIncludes = popthdlr->MapLogical(P_CplrIncludes);
		UINT idOutputCod = popthdlr->MapLogical(P_OutputDir_Cod);
		UINT idOutputSbr = popthdlr->MapLogical(P_OutputDir_Sbr);
		UINT idOutputPch = popthdlr->MapLogical(P_OutputDir_Pch);
		UINT idOutputObj = popthdlr->MapLogical(P_OutputDir_Obj);
		UINT idOutputPdb = popthdlr->MapLogical(P_OutputDir_Pdb);

		g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_MkTypLib), (CBldSysCmp *&)popthdlr);
		UINT idMTLIncludes = popthdlr->MapLogical(P_MTLIncludes);
		UINT idMTLOutputTlb = popthdlr->MapLogical(P_MTLOutputTlb);
		UINT idMTLOutputInc = popthdlr->MapLogical(P_MTLOutputInc);

		g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_RcCompiler), (CBldSysCmp *&)popthdlr);
		UINT idResIncludes = popthdlr->MapLogical(P_ResIncludes);
		UINT idOutNameRes = popthdlr->MapLogical(P_OutNameRes);

		CObList list;
		FlattenSubtree(list, flt_Normal | flt_ExcludeGroups | flt_ExcludeDependencies | flt_RespectTargetExclude);

		POSITION pos = list.GetHeadPosition();
		CProjItem* pItem;
		while (pos != NULL)
		{
			pItem = (CProjItem *)list.GetNext(pos);
			OptBehaviour optbehOld = pItem->GetOptBehaviour();
			pItem->SetOptBehaviour((OptBehaviour)(optbehOld & ~(OBInherit|OBShowDefault|OBShowFake) | OBShowMacro));

			// call helper function to do the actual work
			// compiler props
			retval = ConvertPropPaths(pItem, idCplrIncludes) || retval;
			retval = ConvertPropPaths(pItem, idOutputCod) || retval;
			retval = ConvertPropPaths(pItem, idOutputSbr) || retval;
			retval = ConvertPropPaths(pItem, idOutputPch) || retval;
			retval = ConvertPropPaths(pItem, idOutputObj) || retval;
			retval = ConvertPropPaths(pItem, idOutputPdb) || retval;

			// mtl props
			retval = ConvertPropPaths(pItem, idMTLIncludes) || retval;
			retval = ConvertPropPaths(pItem, idMTLOutputTlb, FALSE) || retval;
			retval = ConvertPropPaths(pItem, idMTLOutputInc, FALSE) || retval;

			// res props
			retval = ConvertPropPaths(pItem, idResIncludes) || retval;
			retval = ConvertPropPaths(pItem, idOutNameRes, FALSE) || retval;

			// REVIEW: other props to convert?

			if (pItem->IsKindOf(RUNTIME_CLASS(CProject)))
			{
				// project-level only properties

				// linker props
				g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Linker), (CBldSysCmp *&)popthdlr);
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_OutName), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_LibPath), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_UsePDBName), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_DefName), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_ImpLibName), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_MapName), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_LinkDelayNoBind), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_LinkDelayUnLoad), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_LinkDelayLoadDLL), FALSE) || retval;

				// lib props
				g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Lib), (CBldSysCmp *&)popthdlr);
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_DefNameLib), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_OutNameLib), FALSE) || retval;

				// bscmake props
				g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_BscMake), (CBldSysCmp *&)popthdlr);
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_BscMakeOutputFilename), FALSE) || retval;

				// REVIEW: other props to convert?
			}
			pItem->SetOptBehaviour(optbehOld);
		}
	}
	return retval;  // TRUE iff anything changed
}

CProject::CProject()
{
	// our current project is us!
	m_pOwnerProject = this;

	m_pFilePath = NULL;
 	m_bPrivateDataInitialized = FALSE;
	m_bProjectComplete = FALSE;
	m_bProjWasReadOnly = FALSE;
	m_bProjConverted = FALSE;
	m_bConvertedVC20 = FALSE;
	m_bConvertedDS4x = FALSE;
	m_bConvertedDS5x = FALSE;
	m_bConvertedDS6 = FALSE;
	m_bConvertedDS61 = FALSE;
	m_bGotUseMFCError = FALSE;
	m_bProjExtConverted = FALSE;
    m_bOkToDirty = TRUE;
    m_bProjectDirty = FALSE;
	m_bDirtyDeferred = FALSE;
	m_nDeferred = 0;
	m_bProj_PerConfigDeps = FALSE;
	m_bHaveODLFiles = FALSE;

 	if (m_lstProjects.IsEmpty()) // init these only for the first project
	{
		g_nIDOptHdlrUnknown = 10; // 0 is reserved fir CCustomBuildTool
		g_nIDToolUnknown = PROJ_PER_CONFIG_UNKNOWN;
	}
	m_lstProjects.AddTail((CObject *)this);

	::InitializeCriticalSection(&m_critSection);
}
///////////////////////////////////////////////////////////////////////////////
void CProject::Destroy()
{
	static bool bWalkedList = false;

	// destroying our project
	BOOL bOldComplete = m_bProjectComplete;
	m_bProjectComplete = FALSE;
 	m_strProjActiveConfiguration.Empty();
	m_strProjDefaultConfiguration.Empty();
	m_strProjStartupConfiguration.Empty();

	m_bOkToDirty = FALSE; // can't do this from here on...

	if( m_bConvertedDS4x && !bWalkedList )
	{
		bWalkedList = true;
		m_bConvertedDS4x = FALSE;

		// may also need to delete temp projects
		POSITION pos = CProject::m_lstProjects.GetTailPosition();  // use our own private pos pointer
		while (pos != NULL)
		{
			CProject * pProject = (CProject *)CProject::m_lstProjects.GetPrev(pos);
			if (pProject->m_bConvertedDS4x)
			{
				delete pProject;
			}
		}
	}
	
	// delete each of the targets which in turn will delete their contents
	CString strConfigName; CTargetItem * pTarget; HCONFIGURATION hConfig;
	POSITION pos;
	while ((pos = m_TargetMap.GetStartPosition()) != (POSITION)NULL)
	{
		m_TargetMap.GetNextAssoc(pos, strConfigName, (void* &)pTarget);
        hConfig = g_BldSysIFace.GetConfig(strConfigName, (HPROJECT)this);
        g_BldSysIFace.DeleteConfig(hConfig, (HPROJECT)this);
	}

	// now delete any configs that might not have been registered (read failed early)
	int n = GetPropBagCount();
	CConfigurationRecord * pcr;
	while (n-- > 0)	// work backwards so array is intact
	{
		pcr = (CConfigurationRecord*) m_ConfigArray[n];
		VSASSERT(pcr, "NULL config record in config array!");

		// Perform the delete...
		DeleteConfigFromRecord(pcr);
	}

	// finished destroying
	// restore previous state
	m_bProjectComplete = bOldComplete;
}
///////////////////////////////////////////////////////////////////////////////
CProject::~CProject ()
{
	if (m_lstProjects.GetCount()==1) // don't do this unless this is the last project!
		g_bInProjClose = TRUE;

	// ** DATA **
	// destroy the contents of the project
	Destroy();

	// ** OTHER DATA **
	// we cannot allow dynamic async. sections to remain registered
	
	// UNDONE: make this per-builder
	CBldrSection * pbldsect;
	g_BldSysIFace.InitSectionEnum();
	CString strName;
	while ((pbldsect = g_BldSysIFace.GetNextSection(strName)) != (CBldrSection *)NULL)
	{
		// dynamic async. sections?
		if (pbldsect->m_pfn == NULL && pbldsect->m_hBld == (HPROJECT)this) // This section must be from this project
		{
			g_BldSysIFace.DeregisterSection((const CBldrSection *)pbldsect);

			if (pbldsect->m_pch != (TCHAR *)NULL)
				free(pbldsect->m_pch);

			delete pbldsect;
		}
  	}

	POSITION pos = m_lstProjects.Find((CObject *)this);
	VSASSERT(pos != NULL, "Failed to find ourself in the projects list!");
	m_lstProjects.RemoveAt(pos);
	// reset pos to make sure it's still valid
	m_posProjects = m_lstProjects.GetHeadPosition();

	if (m_posProjects == NULL)
		GlobalCleanup();	// only do this for the last project

	if (this == g_pActiveProject)
	{
		CProject * pProject;
		g_pActiveProject = NULL;  	// we may no longer have an active project!
		POSITION pos = m_lstProjects.GetHeadPosition();  // use our own private pos pointer
		while (pos != NULL)
		{
			pProject = (CProject *)m_lstProjects.GetNext(pos);
			if (pProject != NULL)
			{
				ASSERT_VALID(pProject);
				if (pProject->IsLoaded())
				{
					HCONFIGURATION hConfig = g_BldSysIFace.GetActiveConfig((HPROJECT)pProject);
					if (hConfig==NO_CONFIG)
					{
						CTargetItem * pTargetItem;
						CString strTargetName;
						pProject->InitTargetEnum();
						pProject->NextTargetEnum(strTargetName, pTargetItem);
						hConfig = g_BldSysIFace.GetConfig(strTargetName, (HPROJECT)pProject);
						VSASSERT (hConfig, "No active config!");
					}
					g_BldSysIFace.SetActiveConfig(hConfig, (HPROJECT)pProject);
					break;
				}
				else
				{
					g_pActiveProject = pProject; // at least we have one
				}
			}
		}
	}

	::DeleteCriticalSection(&m_critSection);

	if (m_pFilePath)
	{
		delete m_pFilePath;
		m_pFilePath = NULL;
	}
	//ReleaseInterface();
	g_bInProjClose = FALSE;

}

// this static function gets called only after all CProjects have been destroyed and deleted
void CProject::GlobalCleanup()
{
    // Clear out all unknown tools from all projtypes
    CProjType * pprojtype;
    g_pPrjcompmgr->InitProjTypeEnum();
    while (g_pPrjcompmgr->NextProjType(pprojtype))
        pprojtype->ClearUnknownTools();

}

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::SetFile(const CPath *pPath, BOOL  /* = FALSE */)
{
	VSASSERT(pPath != (const CPath *)NULL, "SetFile cannot be called with a NULL path");
	VSASSERT(pPath->IsInit(), "SetFile requires an initialized path");

	if (m_pFilePath)
	{
		delete m_pFilePath;
		m_pFilePath = NULL;
	}

	m_pFilePath = new CPath(*pPath);

	if (m_pFilePath != NULL)
	{
		// get new title as well
		m_strProjItemName = GetFilePath()->GetFileName();

		// init m_ProjDir here as well
		BOOL bOK = m_ProjDir.CreateFromPath(*GetFilePath());
		VSASSERT(bOK, "Failed to create project directory path!");

		m_ActiveTargDir = m_ProjDir;
		return TRUE;
	}

	return FALSE;
}

const CPath *CProject::GetFilePath() const
{
	return m_pFilePath;
} 
///////////////////////////////////////////////////////////////////////////////
void CProject::FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem)
{
	if ((fo & flt_ExcludeProjects))
	{
		fAddItem = FALSE;
	}
	else
		fAddItem = TRUE;

	// Always consider contents regardless of whether we consider the
	// project node itself
	fAddContent = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::InitNew(const CVCPtrList * plstProjType)
{
	if (plstProjType != (const CVCPtrList *)NULL)
	{
		VCPOSITION pos = plstProjType->GetHeadPosition();
		VSASSERT(pos != (VCPOSITION)NULL, "must have at least one projtype!");

		HCONFIGURATION hDebugTarg, hReleaseTarg;

		// create a pair of targets for each target type
		while (pos != NULL)
		{
			CProjType * pProjType = (CProjType *)plstProjType->GetNext(pos);
			(void) g_BldSysIFace.AddDefaultConfigs
					(
						(const TCHAR *)*pProjType->GetPlatformName(),	// 'official' name of this target's platform
						(const TCHAR *)*pProjType->GetTypeName(),		// 'official' name of this target type
						hDebugTarg, hReleaseTarg,						// our new targets
						FALSE											// we're using 'official' names
					);
		}
	}

	m_bProjectComplete = TRUE;	// project is now ok to use...

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////

void CProject::SetBuildToolsMap()
{
	const CVCPtrArray * pCfgArray = GetConfigArray();
	INT_PTR cCfgs = pCfgArray->GetSize();
	for (int i=0; i < cCfgs; i++)
	{
		CConfigurationRecord *pConfiguration = (CConfigurationRecord *)pCfgArray->GetAt(i);
		pConfiguration->SetBuildToolsMap();
	}
}

///////////////////////////////////////////////////////////////////////////////

BOOL CProject::InitFromFile (const char *pFileName, BOOL /* bLoadOPTFile  = TRUE */)
{
//	Read in the supplied makefile.  We dont' know yet if its internal or
//  external, and we won't know until we call ReadMakeFile (which looks for
//	the signature string).  If it is an internal file, load silently, regards
//  of whether or not the OPT file has been found.  If its external, and there
//  is a OPT file, also load silently, since presumably this file has been used
//  as external before.  If its an external makefile and there's no OPT, then
//  as if we should import:

	CPath NewPath;
	ReadMakeResults rmr = ReadInternal;
	g_pConvertedProject = NULL;
	CString strMsg;
	CString	strCmdLine;		// holds command line for .exe on command line proj
	CWaitCursor wc ;		// Wait cursor while load is in progress
	BOOL	bProjectDirty = FALSE ;
	CPath::ResetDirMap(); // clear cache for new project
	
	// Dirty until load successfully:
    DirtyProject();

	if (!NewPath.Create (pFileName) || !SetFile (&NewPath) ) return FALSE;
	InformDependants (SN_FILE_NAME);

	{
		rmr = ReadMakeFile (bProjectDirty);

		if (rmr == ReadExportedMakefile)
		{
			// if we opened an exported makefile, try again with the new filename
			rmr = ReadMakeFile(bProjectDirty);
		}

		if (rmr == ReadError)
			return FALSE;
		else if (rmr == ReadExternal)
		{
			// come up with a name for this project
			CString strProjectName;
			NewPath.GetBaseNameString(strProjectName);
 			SetTargetName(strProjectName);
			VSASSERT(GetPropBagCount() == 0, "Property bags should be cleaned up by now");

			m_bProjConverted = m_bConvertedVC20 = FALSE;

 			// Put up the platforms dialog if we have more than one
			// platform installed on the system
			CStringList lstPlatforms;
 			if (g_pPrjcompmgr->GetSupportedPlatformCount() > 1)
			{
				CProjTempProjectChange projTempProjectChange (NULL);
				return FALSE;
			}
			else
			{
				// Create external targets for the primary platform
				CPlatform* pPlatform = g_pPrjcompmgr->GetPrimaryPlatform();
				const CString * pstrPlatformName = pPlatform->GetUIDescription();
				lstPlatforms.AddTail(*pstrPlatformName);
			}

			// Reading an external makefile
			HCONFIGURATION hDebugTarg = NO_CONFIG;
			HCONFIGURATION hReleaseTarg;
			CString strTypeName;
			BOOL bOK = strTypeName.LoadString(IDS_EXTTARG_PROJTYPE);
			VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");

            // Create all the external targets
			CString strPlatformName;
			POSITION pos = lstPlatforms.GetHeadPosition();
			while (pos != NULL)
			{
				strPlatformName = lstPlatforms.GetNext(pos);
			
				// Add debug and release targets
				(void) g_BldSysIFace.AddDefaultConfigs
						((const TCHAR *) strPlatformName,
						(const TCHAR *)strTypeName,
						hDebugTarg, hReleaseTarg,
						TRUE, TRUE, TRUE, hDebugTarg
						);
			}

			// Add the external makefile as a file in the wrapper internal
			// makefile.
			HPROJECT hProj = (HPROJECT)g_pActiveProject;
			HFILESET hFileSet = g_BldSysIFace.GetFileSet(hProj, hDebugTarg);
			g_BldSysIFace.AddFile(hFileSet, &NewPath, FALSE);
			NewPath.ChangeExtension(".dsp");
			SetFile(&NewPath);
			InformDependants (SN_FILE_NAME);

			// Mark project so that we will immediately save it
			m_bProjConverted = TRUE;
			bProjectDirty = TRUE;

			// Flag that the current project was created by converting
			// an external makefile
			m_bProjExtConverted = TRUE;
 		}
		else if (rmr == ReadInternal)
		{
		}
		else
		{
			VSASSERT(FALSE, "Unhandled case in InitFromFile");
			return FALSE;
		}

		RecheckReadOnly();	// See if the .MAK file is read-only
	}

    CleanProject();

	// we can set the project to dirty by now
	if (bProjectDirty)
	{
		// set project as dirty
        DirtyProject();
	}

	CString str, strProjects;
	if (m_bConvertedDS4x)
	{
		CProject * pProject;
		CProject::InitProjectEnum();
		while ((pProject = (CProject *)CProject::NextProjectEnum(str, FALSE)) != NULL)
		{
			// only consider the recently converted ones
			if (pProject->m_bConvertedDS4x)
			{
				pProject->m_bProjectComplete = TRUE;	// projects are now ok to use...
				strProjects += '\t';
				strProjects += *pProject->GetFilePath();
				strProjects += '\n';
			}
		}
	}
	else
	{
 		m_bProjectComplete = TRUE;	// project is now ok to use...
		strProjects += '\t';
		strProjects += *GetFilePath();
		strProjects += '\n';
	}

	// skip this dialog when doing minor conversion from 5.x
	if ((m_bProjConverted) && (rmr == ReadInternal) && (!m_bConvertedDS5x) && (!m_bConvertedDS6) 
		&& (!m_bConvertedDS61))
	{
		CProjTempProjectChange projTempProjectChange (NULL);
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Obtain the toolset for this projects platform type.
INT CProject::GetProjectToolset()
{
	CDirMgr * pDirMgr = GetDirMgr();
	CProjType * pProjType;
	INT nToolset;

	// Get the toolset for the current project platform
	g_pPrjcompmgr->LookupProjTypeByName(GetActiveConfig()->GetOriginalTypeName(), pProjType);

	// If the current toolset is unsupported then we pick the primary platform as a default
	if (!pProjType->IsSupported())
	{
		CPlatform * pPlatform = g_pPrjcompmgr->GetPrimaryPlatform();
		nToolset = pDirMgr->GetPlatformToolset(*(pPlatform->GetName()));
	}
	else
		nToolset = pDirMgr->GetPlatformToolset(*(pProjType->GetPlatformName()));

	return nToolset;
}

///////////////////////////////////////////////////////////////////////////////
CProject::ReadMakeResults CProject::ReadMakeFile (BOOL &bProjectDirty)
{
//	Open the project's makefile and try to read it in as an internal makefile.
//	The makefile reader determines this by looking for our special header in
//	the file.
	g_pLastTool = NULL;
	static UINT idConvReply;

	BOOL bRetval = TRUE, bOkAsInternal = FALSE;
	const CPath *pPath = GetFilePath();
	VSASSERT (pPath, "Project file path required");
	CObject *pObject = NULL;
	CString strMsg, str, str1, str2, str3, strDSSig;
	ReadMakeResults rmr = ReadInternal;

	CMakFileReader mr;

	// FUTURE: remove this when move to CBuildFile in bldrfile.cpp
	g_buildfile.m_pmr = &mr;
	
	CFileException excptFile;
	if (!mr.Open(*pPath, CFile::modeRead | CFile::shareDenyWrite, &excptFile))
	{
		// If e.m_cause == CFileException::none, we didn't even get to the open,
		// meaning the path is bad somehow...

		CProjTempProjectChange projTempProjectChange (NULL);
		rmr = ReadError;
	}
	else																
	{
		try
		{
			pObject = mr.GetNextElement();
			VSASSERT (pObject, "NULL element in CMakFileReader object!");

			// Skip over "Microsoft Developer Studio Project File" signature
			BOOL bOK = strDSSig.LoadString (IDS_DSPROJ_SIG_STRING);
			VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
			if ((pObject->IsKindOf(RUNTIME_CLASS(CMakComment))) &&
				 (_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)strDSSig, strDSSig.GetLength()) == 0))
			{
				// UNDONE: check/assert that the package id is 4
				delete pObject;
				pObject = mr.GetNextElement();
				VSASSERT (pObject, "NULL element in CMakFileReader object!");
			}

			// look for a comment with the signature string
			// to see if it is a Dolphin or Olympus makefile
			// or an Orion project file
			bOK = str.LoadString (IDS_MAKEFILE_SIG_STRING);
			VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
            bOK = str1.LoadString (IDS_BUILDPRJ_SIG_STRING);
			VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
            bOK = str2.LoadString (IDS_OLD_MAKEFILE_SIG_STRING);
			VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
            bOK = str3.LoadString (IDS_EXPORT_SIG_STRING);
			VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
			if ((!pObject->IsKindOf(RUNTIME_CLASS(CMakComment))) ||
				 ((_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str, str.GetLength()) != 0) && 
				 (_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str1, str1.GetLength()) != 0) && 
				 (_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str2, str2.GetLength()) != 0) && 
				 (_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str3, str3.GetLength()) != 0))
			   )
			{
				// Either not a VC makefile at all or is pre-V2.0
				// TODO: give a better error message about why we can't convert here... (pre-V2.0)
				rmr = ReadError;
				AfxThrowFileException (CFileException::generic);
			}
			else if (_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str3, str3.GetLength()) == 0)
			{
				CDir dir;
				CPath path;
				dir.CreateFromPath(*pPath);
				bOK = path.CreateFromDirAndFilename(dir, ((const TCHAR *)((CMakComment *)pObject)->m_strText) + str3.GetLength());
				VSASSERT(bOK, "Failed to create path!");
				delete pObject;
				pObject = (CObject *)NULL;
				if (!path.ExistsOnDisk())
				{
					// file doesn't exist, so give up
					CProjTempProjectChange projTempProjectChange (NULL);
					rmr = ReadExternal;
				}
				else
				{
					SetFile(&path, TRUE);
					InformDependants (SN_FILE_NAME);
					rmr = ReadExportedMakefile;
				}
			}
			else
			{
				//
				// A Dolphin or Olympus makefile
				//

				// FUTURE: UNICODE
				BOOL bV4xSigString = !(_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str, str.GetLength())); 
				BOOL bNewSigString = !(_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str1, str1.GetLength())); 
 				TCHAR * pend;
				UINT fileVer = strtoul ((const TCHAR *)((CMakComment *)pObject)->m_strText
									    + ((bNewSigString ? str1 : (bV4xSigString ? str : str2)).GetLength () / sizeof (TCHAR)), &pend, 10);
				
				// Makefile version parsing. Makefile versions can be of two formats
				// 1. Release format, the version number is of the form "m.nn"
				// 2. Internal format, the version number if of the form mnnii
				// where m = major version number
				//       n = minor version number
				//       i = internal version number
				// We must have the correct major version number but allow a lower
				// limit on the minor & internal version numbers
 				UINT nMajor, nMinor, nFormat;
				BOOL bFutureFormat = FALSE;

				if ((*pend)==_T('.'))
				{
					// Release format makefile version number as it contains a period
					// So get the major and minor versions and check the validity
					nMajor = fileVer;
					nMinor = strtoul(++pend, NULL, 10);
					bOkAsInternal = ( (10000*nMajor+100*nMinor) >= nLastCompatibleMakefile);
  				}
				else
				{
					// Internal format makefile version number. So get the major version
					// number and compare it no the last compatible makefile version.
					nMajor = fileVer / 10000;
					nMinor = fileVer / 100 - (nMajor * 100);
 					bOkAsInternal = (fileVer >= nLastCompatibleMakefile);
 				}
				nFormat = nMajor * 10000 + nMinor * 100;
				bFutureFormat = ((nFormat > nVersionNumber) && (nFormat >= (g_nNextIncompatibleVersion * 100))); // defaults to 700
				bOkAsInternal = bOkAsInternal && !bFutureFormat;

				m_bConvertedVC20 = FALSE;
				m_bConvertedDS4x = FALSE;
				m_bConvertedDS5x = FALSE;
				m_bConvertedDS6 = FALSE;
				m_bConvertedDS61 = FALSE;

				if (nMajor > 6 && !bFutureFormat)
				{
					// not a THING we can do about converting anything past VC6X...
				}
				else if ((nMajor >= 2) && (nMajor <= 6))
				{
					g_pActiveProject = NULL;
					// Prepare to convert
					// VC++ 2.0/4.0 makefile
					if (nMajor == 2)
						m_bConvertedVC20 = TRUE;
					else if (nMajor < 5)
						m_bConvertedDS4x = TRUE;
					else if (nMajor == 5)
						m_bConvertedDS5x = TRUE;
					else if (nMajor == 6)
					{
						if (nMinor == 0)
							m_bConvertedDS6 = TRUE;
						else
							m_bConvertedDS61 = TRUE;
					}

					// Flag bOkAsInternal as TRUE, we are converting!!
					m_bProjConverted = bOkAsInternal = TRUE;

					// we'll need this later...
					g_pConvertedProject = (CProject *)this;
				}
				else if (bFutureFormat)
				{
					// give error if trying to load with old product
					VSASSERT(!bOkAsInternal, "Trying to load with old product");
					rmr = ReadError;
				}

				// Ok this is the latest and greatest makefile format
				if (!bOkAsInternal)
 					AfxThrowFileException (CFileException::generic);
 
				delete (pObject); pObject = (CObject *)NULL;
				VSASSERT(bOkAsInternal, "How'd we get here without a signature?");	// ok as internal (we saw a signature)

				// Read in configurations:
				if (!ReadConfigurationHeader(mr))
				{
					if (m_bConvertedDS5x || m_bConvertedDS6 || m_bConvertedDS61)
						rmr = ReadError; // don't try to wrap
					AfxThrowFileException(CFileException::generic);
				}

				VSASSERT(this == g_pActiveProject, "Current project must be active project during project load");

				// we'll break when we hit the '# Project' delimiter or EOF
				for (;;)
				{
					if (pObject)
					{
						delete pObject;
						pObject = (CObject *)NULL;
					}

					pObject = mr.GetNextElement();
					VSASSERT (pObject != (CObject *)NULL, "NULL element in CMakFileReader object!");

					// reached end of builder file prematurely?
					if (pObject->IsKindOf(RUNTIME_CLASS(CMakEndOfFile)))
	                	AfxThrowFileException(CFileException::generic);

					// read in our sections
					if (pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
					{
						const TCHAR * pchComment = (const TCHAR *)((CMakComment *)pObject)->m_strText;

						// got the '# Project' comment?
						if (GetRTCFromNamePrefix(pchComment) == GetRuntimeClass())
							break;
					}
				}

				mr.UngetElement(pObject); pObject = (CObject *)NULL;
				
				// Read in the bulk of the makefile
				if (!ReadFromMakeFile (mr, GetWorkspaceDir()))
		        	AfxThrowFileException (CFileException::generic);

				// Read in our sections
				// We'll break when we hit EOF
				for (;;)
				{
					if (pObject)
					{
						delete pObject;
						pObject = (CObject *)NULL;
					}

					pObject = mr.GetNextElement();
					VSASSERT (pObject != (CObject *)NULL, "NULL element in CMakFileReader object!");

					// reached end of builder file?
					if (pObject->IsKindOf(RUNTIME_CLASS(CMakEndOfFile)))
	                	break;

					// read in our sections
					if (pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
					{
						const TCHAR * pchComment = (const TCHAR *)((CMakComment *)pObject)->m_strText;

						// got a '# Section <name>' comment?
						// FUTURE: replace this with the szSectionMark in bldrfile.cpp when move there
						if (!_tcsnicmp(pchComment, _TEXT("Section"), 7))
							if (!g_buildfile.ReadSectionForNMake((const TCHAR *)(pchComment + 8)))
								AfxThrowFileException(CFileException::generic);
					}
				}

				if (pObject)
				{
					delete pObject;
					pObject = (CObject *)NULL;
				}

				CProject * pProject = GetProject();
				VSASSERT(pProject == this, "How could we not have the current project have itself as its project?");
				CString strProj;
				if (m_bConvertedDS4x)
				{
					CProject::InitProjectEnum();
					pProject = (CProject *)CProject::NextProjectEnum(strProj, FALSE);
				}
				do {
					const CVCPtrArray * pCfgArray = pProject->GetConfigArray();
					INT_PTR icfg, size = pCfgArray->GetSize();
					if ((pProject->m_bConvertedDS6) || (pProject->m_bConvertedDS61) ||
						(pProject->m_bConvertedDS5x) || (pProject->m_bConvertedDS4x) || m_bConvertedVC20)
					{
						CStringList lststrPlatforms;
						CString strPlatformName;
						bOK = strPlatformName.LoadString(IDS_V4_MACPMC_PLATFORM);
						VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
						lststrPlatforms.AddTail(strPlatformName);
						bOK = strPlatformName.LoadString(IDS_V4_MIPS_PLATFORM);
						VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
						lststrPlatforms.AddTail(strPlatformName);
						bOK = strPlatformName.LoadString(IDS_V2_MAC68K_PLATFORM);
						VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
						lststrPlatforms.AddTail(strPlatformName);
						bOK = strPlatformName.LoadString(IDS_V2_MACPPC_PLATFORM);
						VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
						lststrPlatforms.AddTail(strPlatformName);
						bOK = strPlatformName.LoadString(IDS_V4_PPC_PLATFORM);
						VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
						lststrPlatforms.AddTail(strPlatformName);

						BOOL bFoundSupported = FALSE;
						CString strEmpty;
						icfg = size;
						while (icfg-- > 0)
						{
							CConfigurationRecord * pcr = (CConfigurationRecord *)pCfgArray->GetAt(icfg);
							CString strOriginalName = pcr->GetConfigurationName();
							CString strDebugness = strOriginalName;
							CProjType * pprojtype = NULL;
							CString strOrgType = pcr->GetOriginalTypeName();
							g_pPrjcompmgr->LookupProjTypeByName(strOrgType, pprojtype);
							if (pprojtype->IsSupported())
							{
								bFoundSupported = TRUE;
								continue;
							}

							int index = strOriginalName.Find(_T(" - "));
							if (index != -1)
								strDebugness = strOriginalName.Mid(index + 3);
							POSITION pos = lststrPlatforms.GetHeadPosition();
							while (pos != NULL)
							{
								strPlatformName = lststrPlatforms.GetNext(pos);
								int nLength = strPlatformName.GetLength();
								if 	((_tcsncmp((LPCTSTR)strPlatformName, (LPCTSTR)strOrgType, nLength)==0) ||
									 (_tcsncmp((LPCTSTR)strPlatformName, (LPCTSTR)strDebugness, nLength)==0))
								{
									// delete old configs
									HCONFIGURATION hConfig = g_BldSysIFace.GetConfig(strOriginalName, (HPROJECT)pProject);
									VSASSERT(hConfig != NO_CONFIG, "Need at least one config before we can delete one here.");
									g_BldSysIFace.DeleteConfig(hConfig, (HPROJECT)pProject);
									break;
								}
							}
						}
					}

					size = pCfgArray->GetSize();
					VSASSERT(m_bProjConverted || (size != 0), "Problem during conversion");
					
					if ((size == 0) && (m_bProjConverted))
					{
						// we deleted everything on conversion!
						rmr = ReadError;
						AfxThrowFileException (CFileException::generic);
					}

					for (icfg = 0; icfg < size; icfg++)
					{
						CConfigurationRecord * pcr = (CConfigurationRecord *)pCfgArray->GetAt(icfg);
						CProjTempConfigChange projTempConfigChange(pProject);
						projTempConfigChange.ChangeConfig(pcr);

						// Post-'read custom build' assignment for target-level
						// we couldn't do this during the read because we hadn't set
						// up our targets as we were reading in the project-level blurb....
						// we'll fix this for v4.0 ;-)
						// we'll send this one, we only need one, P_CustomCommand or P_CustomOutputSpec
						// both are supported by custom build
						pProject->InformDependants(P_CustomCommand); 

						if (m_bConvertedVC20 || pProject->m_bConvertedDS4x)
						{
							// Set minimum build and/or MRE deps flags for compiler
							COptionHandler * popthdlr;
							g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Compiler),
																 (CBldSysCmp *&)popthdlr);

							if (m_bConvertedVC20)
								pProject->SetIntProp(popthdlr->MapLogical(P_Enable_MR), TRUE);

							pProject->SetIntProp(popthdlr->MapLogical(P_Enable_FD), TRUE);

							// convert include paths for subprojects
							if (pProject->m_bConvertedDS4x)
								pProject->ConvertDirs();

							// Enable the /mktylib203 flag.
							g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_MkTypLib),
															 (CBldSysCmp *&)popthdlr);

							pProject->SetIntProp(popthdlr->MapLogical(P_MTLMtlCompatible), TRUE);
						}

						if (m_bConvertedVC20 || pProject->m_bConvertedDS4x || m_bConvertedDS5x)
						{
							// V5 -> V6 conversion goes here

							COptionHandler * popthdlr;
							g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Compiler),
																 (CBldSysCmp *&)popthdlr);

							// Check what kind of debug info is allowed by platform
							CPlatform * pPlatform = pProject->GetCurrentPlatform();
							DWORD dwAttrib = pPlatform->GetAttributes();
							BOOL fEnc = dwAttrib & PIA_Supports_ENC;
							if( fEnc )
							{
								// set ENC options for debug targets
								int val;
								// only set for debug configs  (check for /Od)
								if ((pProject->GetIntProp(popthdlr->MapLogical(P_Optimize), val) == valid) && (val == 1))
								{
									UINT nId = popthdlr->MapLogical(P_DebugInfo);
									if ((pProject->GetIntProp(nId, val) == valid) && (val == DebugInfoPdb))
									{
										// also not compatible with /dlp (internal)
										if ((pProject->GetIntProp(popthdlr->MapLogical(P_Enable_dlp), val) != valid) || (val == 0))
										{
											pProject->SetIntProp(nId, DebugInfoENC);
										}
									}
								}
							}
						}
					}
				} while (m_bConvertedDS4x && ((pProject = (CProject *)CProject::NextProjectEnum(strProj, FALSE)) != NULL));

				// Post-'read bulk' VC++ 2.0 conversion?
				// Convert VC++ 2.x target names 
				if (m_bConvertedVC20)
				{
					// Ok. Loop through all targets and turn the v2 names into v3 names.
					// The conversion algorithm, goes like this,
					// v3 names are of the form
					// <project name> - <platform> <debugness bit>
					// so we get the project name from the base name of the v2 makefile.
					// The platform we can determine from the config mgr.
					// Finally we take the original v2 name, strip hyphens from it, and
					// then try to strip a platform name from the front, and the remaining
					// string we set as the debugness bit

					// Get the project name from the makefiles base name
					CString strProjectName;
					pPath->GetBaseNameString(strProjectName);


					// Load the old v2 platform names for later
					CStringList lststrPlatforms;
					CString strPlatformName;
					
					// (WinslowF) When I remove MIPS targets, I commented the following line.
					// We still might need this string to remove MIPS target from impot makefile.
					// If so, you might wannt to take the commnts out of this string in Vproj.rc
					// and resource.h as well.
					bOK = strPlatformName.LoadString(IDS_V2_WIN32X86_PLATFORM);
					VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
					lststrPlatforms.AddTail(strPlatformName);
					bOK = strPlatformName.LoadString(IDS_V2_WIN3280X86_PLATFORM);
					VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
					lststrPlatforms.AddTail(strPlatformName);
					bOK = strPlatformName.LoadString(IDS_V2_WIN32_PLATFORM);
					VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
					lststrPlatforms.AddTail(strPlatformName);
					
					// Loop through all the targets and rename each of them
					const CVCPtrArray * pCfgArray = GetProject()->GetConfigArray();
					INT_PTR icfg, size = pCfgArray->GetSize();
					for (icfg = 0; icfg < size; icfg++)
					{
						CConfigurationRecord * pcr = (CConfigurationRecord *)pCfgArray->GetAt(icfg);
						CProjTempConfigChange projTempConfigChange(this);
						projTempConfigChange.ChangeConfig(pcr);

						// The original v2 target name
						CString strOriginalName = pcr->GetConfigurationName();
 					
						// Get the new platform name
						CPlatform * pPlatform = GetCurrentPlatform();
						CString strPlatform = *(pPlatform->GetUIDescription());

						// If we recognise an old v2 platform name at the front of the old
						// v2 target name then we strip it off
						CString strDebugness = strOriginalName;
						POSITION pos = lststrPlatforms.GetHeadPosition();
						while (pos != NULL)
						{
							strPlatformName = lststrPlatforms.GetNext(pos);
							int nLength = strPlatformName.GetLength();
							if (!strPlatformName.Compare(strDebugness.Left(nLength)))
							{
								strDebugness = strDebugness.Right(strOriginalName.GetLength() - nLength);
								break;
							}
						}

						// If there is a space at the start of the v2 target name then remove it
						if (!strDebugness.IsEmpty() && strDebugness[0] == _T(' '))
							strDebugness = strDebugness.Right(strDebugness.GetLength() - 1);

						// Construct the new name from the basename of the makefile, the new platform name,
						// and the remaining portion of the old v2 target name
						CString strNewName = strProjectName + _T(" - ") + strPlatform + _T(" ") + strDebugness;

						CTargetItem* pTarget = GetTarget(strOriginalName);
						SetTargetName(strProjectName);
						VSASSERT(pTarget != NULL, "No target for the project!");
						if (pTarget != NULL)
							pTarget->SetTargetName(strProjectName);

						// There are some consistency requirements for VC 4.0 projects that were not
						// meaningful in VC 2.0 projects. For example debug MFC requires debug CRT's
						// but there were no debug CRT's in VC 2.0. We call PerformWizardSettings 
						// to do the validity checking and make the neccessary changes.

						int iUseMFC;
						GetIntProp(P_ProjUseMFC, iUseMFC);
					
						GetProjType()->PerformSettingsWizard(this, iUseMFC);
					}

					m_bProjConverted = TRUE;

					// we want to dirty the project because it is not "silent convert"
					bProjectDirty = TRUE;
				}

				// Post-'read bulk' VC++ 4.x, 5.x, or 6.x conversion?
				if (m_bConvertedDS4x || m_bConvertedDS5x || m_bConvertedDS6 || m_bConvertedDS61)
				{
					m_bProjConverted = TRUE;

					// we want to dirty the project because it is not "silent convert"
					bProjectDirty = TRUE;
				}
			}

			mr.Close();
			if (m_bProjConverted)
			{
				CPath newPath = *pPath;
				newPath.ChangeExtension(BUILDER_EXT);
				SetFile(&newPath, TRUE);
				InformDependants (SN_FILE_NAME);
			}
		}
		catch (CFileException* pEx)
		{
			UNUSED_ALWAYS(pEx);

			if (pObject)
			{
				delete pObject;
			}

			mr.Abort();	// Safely close the reader.

			// restore the project to an empty state
			// since we may try to read in an old OPT file
			// for it and use the project as an external project
			Destroy();

			bRetval = FALSE;
		}
	}

	if ((rmr != ReadError) && (rmr != ReadExportedMakefile))
	{
		if (!bRetval || !bOkAsInternal)
		{
			// If we failed to load this project because the
			// project uses mfc and we haven't got mfc installed
			// and the user explicitly said don't load it, then
			// flag the problem as a read error
			// OTHERWISE
			// we flag the problem as being the makefile is
			// external (and will later try to load it as such)
			if (bOkAsInternal && m_bGotUseMFCError)
				rmr = ReadError;
			else if (rmr != ReadExternal)
			{
				// don't offer to wrap unless converted .mak
 				rmr = m_bProjConverted ? ReadExternal : ReadError;
			}
		}
		else if (m_bGotUseMFCError)
		{
			bProjectDirty = TRUE;
		}
	}

	return rmr;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir)
{
	CObject *pObject = NULL;
	CString str;
	CProject * pProject;
	if (m_bConvertedDS4x)
	{
		CProject * pProject;
		CProject::InitProjectEnum();
		while ((pProject = (CProject *)CProject::NextProjectEnum(str, FALSE)) != NULL)
		{
			pProject->m_cp = EndOfConfigs;
		}
	}
	else
	{
		m_cp = EndOfConfigs;
	}
	
	pProject = (CProject *)this;

	TRY
    {
		pObject = mr.GetNextElement ();
		if (!pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)))
				AfxThrowFileException (CFileException::generic);

 		delete (pObject); pObject = NULL;

		for (pObject = mr.GetNextElement (); pObject!=NULL; pObject = mr.GetNextElement ())
	   	{

			if (pProject->IsPropComment(pObject))
				continue;
			else if (pProject->IsAddSubtractComment(pObject))
				continue;
			else if (pObject->IsKindOf ( RUNTIME_CLASS(CMakDirective)))
			{
				pProject->ReadConfigurationIfDef((CMakDirective *)pObject, m_bConvertedDS4x);
				if (m_bConvertedDS4x)
					pProject = g_pActiveProject;	// may have changed
			}
			else if (pObject->IsKindOf ( RUNTIME_CLASS(CMakMacro)))
			{
				if (!pProject->SuckMacro ((CMakMacro *) pObject, BaseDir))
					AfxThrowFileException (CFileException::generic);
			}
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakDescBlk)))
			{
				if (!pProject->SuckDescBlk((CMakDescBlk *)pObject))
					AfxThrowFileException (CFileException::generic);
			}
			else if (pProject->IsCustomBuildMacroComment(pObject)) {
				pProject->ReadCustomBuildMacros(mr);
			}
			// Comment marks start of children:
			else if (pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)))
			{
				VSASSERT( pProject->m_cp==EndOfConfigs, "Malformed project file");
				if( pProject->m_cp != EndOfConfigs )
					AfxThrowFileException (CFileException::generic);

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

		// Pre-'read bulk' VC++ 2.0 conversion?
		if (m_bConvertedVC20)
		{
   			CConfigurationRecord* pcr;
			CString strTargetName;
			CTargetItem* pNewTarget = NULL;

			// All the remaining configs are also mapped to the
			// newly created target node, i.e. they are mirrors
			INT_PTR iNumConfig = m_ConfigArray.GetSize();
			for (INT_PTR i = 0 ; i < iNumConfig; i++)
			{
				pcr = (CConfigurationRecord *) m_ConfigArray[i] ;
				strTargetName = pcr->GetConfigurationName();

				if (i == 0)
				{
					pNewTarget = new CTargetItem();
					pNewTarget->MoveInto(this);
				}

				// force the target into it's own config.
				CProjTempConfigChange projTempConfigChange(this);
				projTempConfigChange.ChangeConfig(strTargetName);
				CConfigurationRecord * pcrProject = GetActiveConfig();
								
				pNewTarget->GetActiveConfig()->SetValid(TRUE);
		
			 	// assign the build actions
				CBuildAction::AssignActions(pNewTarget, (CVCPtrList *)NULL, pcrProject);

				// reset the project config.
				projTempConfigChange.Release();

				// Register this target with the project target manager
				RegisterTarget(strTargetName, pNewTarget);
			}
		}

		g_pActiveProject = (CProject *)this;

		// read in children from the makefile
		if (!ReadInChildren(mr, BaseDir))
			AfxThrowFileException (CFileException::generic);

		g_pActiveProject = (CProject *)this;

		CProjTempConfigChange projTempConfigChange(this);

		projTempConfigChange.Reset();

		pProject = (CProject *)this;
		CString strProject;
		if (m_bConvertedDS4x)
		{
			CProject::InitProjectEnum();
			pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE);
			VSASSERT(pProject, "Failed to find at least one project in the projects list");
		}
		do {
			CProjTempConfigChange projTempConfigChange(pProject);
			g_pActiveProject = pProject;
			CTargetItem* pTarget = pProject->GetActiveTarget();

			// custom build assignment (post-project read!)
			CBuildTool * pTool;
			g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_CustomBuild), (CBldSysCmp *&)pTool);
			CVCPtrList lstSelectTools;
			lstSelectTools.AddTail(pTool);

			// special build tool(pre-link/post-build steps)
			g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_SpecialBuild), (CBldSysCmp *&)pTool);
			lstSelectTools.AddTail(pTool);
			CTimeCustomBuildItem *pPreEvent=NULL;
			CTimeCustomBuildItem *pPostEvent=NULL;

			// perform some post-read target processing
			CString strTargetName;
			pProject->InitTargetEnum();
			while (pProject->NextTargetEnum(strTargetName, pTarget))
			{
				CConfigurationRecord * pcrTarget = pProject->ConfigRecordFromConfigName(strTargetName);
				projTempConfigChange.ChangeConfig(pcrTarget);

				CBuildAction::UnAssignActions(pProject, &lstSelectTools, (CConfigurationRecord  *)NULL, TRUE);
				CBuildAction::AssignActions(pProject, &lstSelectTools, (CConfigurationRecord  *)NULL, TRUE);
	
				// is this makefile wanting to use MFC?
				if (!g_pPrjcompmgr->MFCIsInstalled())
				{
					// trying to use MFC?
					int fUseMFC;
					CProject * pProject = pTarget->GetProject();
					if (pProject->GetIntProp(P_ProjUseMFC, fUseMFC) == valid && fUseMFC)
					{
						// flag this error
						BOOL bOldGotUseMFCError = pProject->m_bGotUseMFCError;
						pProject->m_bGotUseMFCError = TRUE;

						// best of luck!
						pProject->SetIntProp(P_ProjUseMFC, FALSE);
					}
				}
			}
		} while (m_bConvertedDS4x && ((pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL));

 	}
	CATCH (CException, e)
	{
		g_pActiveProject = (CProject *)this;
		if (pObject) delete (pObject);
		return FALSE;	// failure
	}
	END_CATCH

	g_pActiveProject = (CProject *)this;

	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::ReadConfigurationHeader  ( CMakFileReader& mr )
{

//	Look through the configuration header area and try to figure out what
//  configurations there and, along with there project type.  This function
//  leaves us in the last defined configuration:

	CMakDirective *pmd = NULL;
	const TCHAR *pc;
	TCHAR *pstart, *pend;
	CString str, strCfg, strBase, strTarget, strProjName;
	CMapStringToPtr mapTargTypes;
	UINT uniqueId;
	CString strDefaultConfig, strSupportedConfig;
	BOOL bSetDefaultConfig = FALSE;
	BOOL bFoundSupportedConfig = FALSE;
	BOOL bFoundUnsupportedConfig = FALSE;
	BOOL bFoundBuildableConfig = FALSE;
	CPath pathProjFileName;
	CString strProjBaseName;
	CProjType * pSupportedProjType = NULL;
	
	pathProjFileName.CreateFromDirAndFilename(m_ProjDir, m_strProjItemName);
	pathProjFileName.GetBaseNameString(strProjBaseName);
	
	TRY
	{
		// Look for a message directive whose first non-blank is a quote:
		// directives:
		for (;;)
		{
			pmd = (CMakDirective *) mr.GetNextElement ();
			if ((((CMakMacro*)pmd)->IsKindOf ( RUNTIME_CLASS (CMakMacro)))
				&&
				(!((CMakMacro*)pmd)->m_strName.Compare("CFG")))
			{
				// set default config
				if (!m_bConvertedDS4x) // may not be valid if converting		
					m_strProjDefaultConfiguration = ((CMakMacro*)pmd)->m_strValue;
			}
			if (((CMakComment*)pmd)->IsKindOf ( RUNTIME_CLASS (CMakComment))
				&&
					(!((CMakComment*)pmd)->m_strText.Left(9).Compare(_T("TARGTYPE "))))
					{
						str = ((CMakComment*)pmd)->m_strText.Mid(9); // rest of line
						pstart = str.GetBuffer(1);

						if (GetQuotedString (pstart, pend))
						{
							*(pend++) = 0;	// nul terminate
							strTarget = pstart;	// target-type name
#ifdef _WIN64
							mapTargTypes.SetAt(strTarget, (void *)_ttoi64(pend));
#else
							// BUGBUG(MikhailA): why the base is 0 ???
							mapTargTypes.SetAt(strTarget, (void *)(INT_PTR)(_tcstoul(pend, NULL, 0)));
#endif
						}
						else 
							VSASSERT(0, "TARGTYPE makefile comments must be quoted");
						str.Empty();
					}
			if (pmd->IsKindOf ( RUNTIME_CLASS (CMakDirective))
				&&
				pmd->m_dtyp	== CMakDirective::DTYP_MESSAGE
				&&
				!pmd->m_strRemOfLine.IsEmpty ()
				)
			{
				pc = pmd->m_strRemOfLine;
				SkipWhite (pc);
              			if (*pc == _T('"'))  break;
			}
			if (pmd->IsKindOf ( RUNTIME_CLASS (CMakEndOfFile)))
				AfxThrowFileException (CFileException::generic);
			delete (pmd); pmd = NULL;
		}
		
		if (m_bConvertedVC20)
		{
			// come up with a name for this project
			const CPath* ppathProj = GetFilePath();
			CString strProjectName;
			ppathProj->GetBaseNameString(strProjectName);
 			SetTargetName(strProjectName);
		}
		
		BOOL bUsedThisProject = FALSE;
		CProject * pBestFitForThisProject = NULL;
		BOOL bFoundGoodMatch = FALSE;
		BOOL bDelConfigsPrompted = FALSE;
		// Try to read in configs until we see something that's not
		// a message directive.  Each config message line has the config
		// name in the first quoted string and the config orginal type
		// in the second string:
		do
		{
			if (pmd->m_strRemOfLine.GetLength() > 1 )
			{
				// Config name:
				pstart =  pmd->m_strRemOfLine.GetBuffer (1);
				if (!GetQuotedString (pstart, pend ))
					AfxThrowFileException (CFileException::generic);

				*pend = _T('\0');
				strCfg = pstart;
				CProject * pProject = (CProject *)this;
				
				if (m_bConvertedDS4x)
				{
					// if not the current project, create new one to match
					// Form the whole configuration name
					int nFirst = strCfg.Find(_T(" - "));
					VSASSERT(nFirst != -1, "Malformed config name");
					if (nFirst < 1)
						AfxThrowFileException(CFileException::generic);

					// strip out project name for this config
					CString strTarget = strCfg.Left(nFirst);
					CPath pathProj; //, pathProjMak;
					pathProj.CreateFromDirAndFilename(GetWorkspaceDir(), strTarget);
					BOOL bDirExists = (pathProj.ExistsOnDisk() && (!FileExists(pathProj)));

					// If the project we are converting has the same name as the .mak
					// don't try to create the .dsp in the sub-dir even if one exists.
					BOOL bFoundMatchingMak = (strTarget.CompareNoCase(strProjBaseName)==0);
					if (bFoundMatchingMak)
					{
						bDirExists = FALSE;		// REVIEW: remove this test?
					}

					if (TRUE)
					{
						// this might be a subproject config so create a new one
						CString strProject = pathProj;

						// put project in subdir if it exists
						if (bDirExists)
						{
							strProject +=  _T('\\');
							strProject += strTarget;
							pathProj.Create(strProject);
						}
						pathProj.ChangeExtension(_T(BUILDER_EXT));
						// pathProjMak = pathProj; pathProjMak.ChangeExtension(".mak");
						BOOL bFound = FALSE;
						CProject::InitProjectEnum();
						VSASSERT(!strTarget.IsEmpty(), "Malformed config name");

						if (strTarget.CompareNoCase(strProjName)==0)
						{
							// matches the 'main' project
							bFound = TRUE;
						}
						else
						{
							// first see if we already have a project with this name
							while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL)
							{
								if ((*(pProject->GetFilePath()) == pathProj) ||
									(strTarget.CompareNoCase(strProject)==0))
								{
									if (pProject->m_bConvertedDS4x)
									{
										bFound = TRUE;
										break;
									}
									else
									{
										CProjTempProjectChange projTempProjectChange (NULL);
										AfxThrowFileException(CFileException::generic);
									}
								}
							}
						
							if (!bFound)
							{
								pProject = (CProject *)g_BldSysIFace.CreateProject(pathProj);
								VSASSERT(pProject != NULL, "Unable to create the project");
								if (!pProject)
								{
									AfxThrowFileException(CFileException::generic);
								}
	
								pProject->m_bConvertedDS4x = TRUE;
								pProject->m_bProjConverted = TRUE;
								pProject->m_strProjDefaultConfiguration = strCfg;
								pProject->m_strProjStartupConfiguration.Empty();
								pProject->m_bProjectComplete = FALSE;  // not done yet!								
								pProject->SetTargetName(strTarget);
								if ((pBestFitForThisProject == NULL) || ((!bDirExists) && (!bFoundGoodMatch)) || bFoundMatchingMak)
								{
									bFoundGoodMatch = bFoundMatchingMak || !bDirExists;
									// worst case we just use the first one
									pBestFitForThisProject = pProject;
								}
							}
						}
					}
					else
					{
						VSASSERT(0, "dead code");
					}
					// REVIEW: any other tests necessary?
				}
				g_pActiveProject = pProject;
				if (!pProject->CreateConfig(strCfg))
					AfxThrowFileException(CFileException::generic);

				if (((!bSetDefaultConfig) && ((pProject==this) || (pProject==pBestFitForThisProject))) /* || (m_bConvertedDS4x && !bUsedThisProject) */) // if no valid CFG= read, use first one
				{
					strDefaultConfig = strCfg;
					bSetDefaultConfig = TRUE;
				}

				// Orginal config name:
				pstart = _tcsinc ( pend );
				if (!GetQuotedString (pstart, pend ))
								AfxThrowFileException (CFileException::generic);
				*pend = _T('\0');
			   	strBase = pstart;
				uniqueId = 0;

				CProjType * pProjType = NULL;
				if ((!g_pPrjcompmgr->LookupProjTypeByName (strBase, (CProjType *&) pProjType)) || (pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeUnknown))))
				{
					if (pProjType==NULL || !pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeUnknown)))
					{
						// Unrecognized ProjType string so try mapping by UniqueId
						if (!mapTargTypes.Lookup(strBase, (void * &)uniqueId) ||
							!g_pPrjcompmgr->LookupProjTypeByUniqueId(uniqueId, (CProjType *&)pProjType))
						{
							// try to get the platform from the unique id
							CPlatform * pPlatform = NULL;
							if ((uniqueId >> 8) & 255)
								g_pPrjcompmgr->LookupPlatformByUniqueId(((uniqueId >> 8) & 255), pPlatform);

							//
						 	// unknown platform and/or project type so create
							// it to avoid trashing makefile
							//

							// derive platfrom and type names from available information
							int index;

							// 1) By default, strip up to first whitespace or last ')'
							if (((index = strBase.Find(_T(')')))) == -1)
								index = strBase.FindOneOf(_T(" \t"));
							else
								index++;

							CString strType = strBase.Mid(index+1); // default

							if (pPlatform==NULL)
							{
								CString strPlatform = strBase.Left(index);
								pPlatform = new CPlatform(strPlatform, (uniqueId>>8) & 255);
								// Register dynamic platform so deleted on exit
								g_pPrjcompmgr->RegisterUnkProjObj(pPlatform);
							}
							pProjType = new CProjTypeUnknown(strType, uniqueId & 255, pPlatform);
							// Register dynamic projtype so deleted on exit
							g_pPrjcompmgr->RegisterUnkProjObj(pProjType);

							// update name in case it changed
							strBase = *pProjType->GetName();
						}
						else
						{
							// must update to new (supported) targtype name
							// ignore id if either byte is 0
							VSASSERT(uniqueId & 255, "Bad value for uniqueId"); VSASSERT(uniqueId>>8, "Bad value for uniqueId");
							strBase = *pProjType->GetName();
						}
					}
				}
				else if (pProjType && pProjType->GetPlatform() && pProjType->GetPlatform()->GetBuildable())
					bFoundBuildableConfig = TRUE;

				if ((m_bConvertedDS5x || m_bConvertedDS6 || m_bConvertedDS61))
				{	// must allow makefile projects still
					// don't support a particular project type in DevStudio anymore
					CString strTmp = strBase.Left(4);
					if (strTmp.GetLength() == 4)
					{
						strTmp.MakeLower();
						int nCRC = (int)strTmp[0] + (int)strTmp[1] + (int)strTmp[2] + (int)strTmp[3];
						if (nCRC == 418 && strTmp[0] == _T('j'))
							AfxThrowFileException (CFileException::generic);
					}
				}
				// note if we read at least one (un)supported target
				if (pProjType->IsSupported())
				{
					bFoundSupportedConfig = TRUE;
					
					if (pSupportedProjType == NULL)
						pSupportedProjType = pProjType;

					// note the first supported config (default, if supported)
					if (((pProject == this) || (pProject==pBestFitForThisProject)) && ((strSupportedConfig.IsEmpty()) || (strCfg==m_strProjDefaultConfiguration)))
						strSupportedConfig = strCfg;
				}
				else
				{
					bFoundUnsupportedConfig = TRUE;
				}

				// note the first Primary platform we encounter (if any)
				if (pProjType->GetPlatform()->IsPrimaryPlatform() &&
					(pProject->m_strProjStartupConfiguration.IsEmpty() ||	strCfg == pProject->m_strProjDefaultConfiguration))
				{
					pProject->m_strProjStartupConfiguration = strCfg;
				}

				// make this configuration active
				pProject->SetStrProp (P_ProjOriginalType, strBase);
 				pProject->SetStrProp (P_ProjActiveConfiguration, strCfg);
			}

			delete (pmd); pmd = NULL;
			pmd = (CMakDirective *) mr.GetNextElement ();
			VSASSERT(pmd, "NULL element in CMakFileReader object!");

		} while (pmd->IsKindOf ( RUNTIME_CLASS (CMakDirective)) &&
				 pmd->m_dtyp == CMakDirective::DTYP_MESSAGE);

		if (m_bConvertedDS4x && !bUsedThisProject)
		{
			if (!pBestFitForThisProject)
			{
				VSASSERT(0, "Trouble in converting 4.X project");
				AfxThrowFileException(CFileException::generic);
			}
			strSupportedConfig.Empty();
			strDefaultConfig = pBestFitForThisProject->m_strProjDefaultConfiguration;
			SetTargetName(pBestFitForThisProject->GetTargetName());
			// copy pBestFitForThisProject to this and delete
			const CVCPtrArray & ppcr = *pBestFitForThisProject->GetConfigArray();
			CConfigurationRecord * pcr;

			{
				CProjTempConfigChange projTempConfigChange(pBestFitForThisProject);

				CString strConfig, strType;
				INT_PTR ccr = ppcr.GetSize();
				for (INT_PTR icr = 0; icr < ccr; icr++)
				{
					pcr = (CConfigurationRecord *)ppcr[icr];
					projTempConfigChange.ChangeConfig(pcr);
					strConfig = pcr->GetConfigurationName();
			
					if ((!CreateConfig(strConfig)) || (pBestFitForThisProject->GetStrProp(P_ProjOriginalType, strType)!=valid))
						AfxThrowFileException(CFileException::generic);
					// make this configuration active
					SetStrProp (P_ProjOriginalType, strType);
 					SetStrProp (P_ProjActiveConfiguration, strConfig);

					if (strSupportedConfig.IsEmpty() && m_pActiveConfig->IsSupported())
						strSupportedConfig = strConfig;

				}
			}
			pBestFitForThisProject->m_bConvertedDS4x = FALSE;
			delete pBestFitForThisProject;
		}

		// make sure we saw at least one projtype we can support
		if (!bFoundSupportedConfig)
		{
			VSASSERT(bFoundUnsupportedConfig, "Found no project types we can support");
			CProjTempProjectChange projTempProjectChange (NULL);
		}

		// Last things are the !ERROR and the !ENDIF.  Don't need them, so
		// skip unitl we hit comments:
		while (!pmd->IsKindOf ( RUNTIME_CLASS (CMakComment))
				&&
			   !pmd->IsKindOf ( RUNTIME_CLASS (CMakEndOfFile)))
		{
			delete (pmd); pmd = NULL;
			pmd = (CMakDirective *) mr.GetNextElement ();
		}
		// Put the comment back:
		mr.UngetElement ( pmd );

		// valid CFG= if read, else use default
		if (m_strProjDefaultConfiguration.IsEmpty() ||
			(!ConfigRecordFromConfigName(m_strProjDefaultConfiguration)))
		{
			if (strDefaultConfig.IsEmpty())
				m_strProjDefaultConfiguration = strSupportedConfig;
			else
				m_strProjDefaultConfiguration = strDefaultConfig;
			VSASSERT(ConfigRecordFromConfigName(m_strProjDefaultConfiguration), "Failed to find config record for config name");
		}

		if (m_strProjStartupConfiguration.IsEmpty())
		{
			m_strProjStartupConfiguration = strSupportedConfig;
			// could still be empty; use m_strProjDefaultConfiguration if so
		}
	}
	CATCH (CException, e)
	{
		g_pActiveProject = (CProject *)this;
		strBase.Empty(); strCfg.Empty(); str.Empty(); strTarget.Empty();
		strDefaultConfig.Empty(); strSupportedConfig.Empty();
		mapTargTypes.RemoveAll();
		if (pmd) delete (pmd);
		return FALSE;
	}
	END_CATCH
	g_pActiveProject = (CProject *)this;
	mapTargTypes.RemoveAll();
	VSASSERT (bSetDefaultConfig, "Unable to set default config");
	VSASSERT (GetActiveConfig(), "No active config!");
	VSASSERT (!m_ConfigMap.IsEmpty(), "No configs!");
	VSASSERT (m_ConfigArray.GetSize(), "No configs!");
	VSASSERT (m_ConfigArray.GetSize()==m_ConfigMap.GetCount(), "Mismatch between config array and config map!");
	ASSERT_VALID (this);
	SetBuildToolsMap();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
GPT CProject::GetIntProp(UINT idProp, int& val)
{
	// If this isn't a prop we're interested in, just
	// return what our parent does.
	if (idProp != P_NoDeferredBscmake &&
		idProp != P_DeferredMecr)
		return CProjContainer::GetIntProp(idProp, val);

	// See if this is in our prop bag.
	GPT gpt = CProjContainer::GetIntProp(idProp, val);

	if (GetCurrentPlatform()->GetUniqueId() == unsupported)
		gpt = invalid;		// don't understand this project type anymore

	// If the return value wasn't 'valid', the prop isn't
	// in the prop bag (only place it could be).  Return
	// 0 by default, meaning that BSCMAKEs are NOT deferred.
	if (gpt != valid)
		val = 0;

	return valid;
}
///////////////////////////////////////////////////////////////////////////////
GPT CProject::GetStrProp(UINT idProp, CString& val)
{
	if (idProp == P_ProjItemOrgPath || idProp == P_FakeProjItemOrgPath) {
		if (m_strRelPath.IsEmpty())
		{
			CPath pathProj, pathWks;
			CDir dirWks;
			dirWks.CreateFromCurrent();
			const CPath * pPath = GetFilePath();
			VSASSERT(pPath, "No project file path?!?");
			if (pPath)
			{
				pathProj = *pPath;
				pathProj.SetAlwaysRelative(TRUE);
				pathProj.GetRelativeName(dirWks, m_strRelPath);
			}
		}
		val = m_strRelPath;
		return valid;
	}
	else if (idProp == P_ProjSccProjName) {
		val = m_strProjSccProjName;
		return valid;
	}
	else if (idProp == P_ProjSccRelLocalPath) {
		val = m_strProjSccRelLocalPath;
		return valid;
	}
	else if (idProp == P_ProjItemFullPath)
	{
		const CPath * pPath = GetFilePath();

		if (pPath != NULL)
		{
			val = pPath->GetFullPath();
			return valid;
		}
		return invalid;
	}
	else if ((idProp==P_OutDirs_Intermediate) || (idProp==P_OutDirs_Target))
		{
			CString valDummy;
			return (GetOutDirString(valDummy, idProp, &val)) ? valid : invalid;
		}

	// At this point, we're only interested in a couple of props.  If
	// this isn't one of those, return what our base class does.
	if (idProp != P_Caller && 
		idProp != P_PromptForDlls &&
		idProp != P_WorkingDirectory &&
		idProp != P_Args && 
		idProp != P_RemoteTarget &&
        idProp != P_Proj_Targ &&
        idProp != P_ExtOpts_Targ
	   )
		return CProjContainer::GetStrProp(idProp, val);

	// See if this is in our prop bag.
	GPT gpt = CProjContainer::GetStrProp(idProp, val);

	// do we want to fake P_ExtOpts_Targ/P_Proj_Targ?
    if (gpt == invalid && (idProp == P_Proj_Targ || idProp == P_ExtOpts_Targ))
    {
        CPath TargetPath = *GetFilePath();
        TargetPath.ChangeExtension(_T(".exe"));
        val = TargetPath.GetFileName();
        return valid;
    }

	// do we want to fake P_Caller?
	//
	// * internal project *
	// -> shouldn't happen to * exe project *
	// This is expensive for them 'cos we hit the disk to
	// look at the executable in GetTargetAttributes().
	// I figure its more important to track the internal project
	// target (this can change more with output directories etc.)
	//
	// o pick the target name if .EXE
	if (gpt == invalid && idProp == P_Caller)
	{
		// HACK - by default, set this check box
		int nVal;
		CString strVal;
		GPT gpt = CProjContainer::GetIntProp(P_PromptForDlls, nVal);
		if (gpt == invalid) {
 			CProjContainer::SetIntProp(P_PromptForDlls, 1);
		}
		gpt = CProjContainer::GetIntProp(P_DebugAttach, nVal);
		if (gpt == invalid) {
			CProjContainer::SetIntProp(P_DebugAttach,0);
		}
		gpt = CProjContainer::GetIntProp(P_DebugRemote, nVal);
		if (gpt == invalid) {
			CProjContainer::SetIntProp(P_DebugRemote,0);
		}
		gpt = CProjContainer::GetStrProp(P_DebugRemoteHost, strVal);
		if (gpt == invalid) {
			CProjContainer::SetStrProp(P_DebugRemoteHost, "");
		}
		gpt = CProjContainer::GetIntProp(P_DebugNative, nVal);
		if (gpt == invalid) {
			CProjContainer::SetIntProp(P_DebugNative,0);
		}


		// let's see if we've got a valid default squirreled away in the dsp file...
		CString strDebugExe;
		if (CProjContainer::GetStrProp(P_ConfigDefaultDebugExe, strVal) == valid && !strVal.IsEmpty())
		{
			CProjContainer::SetStrProp(P_Caller, strVal);
			val = strVal;
			return valid;
		}

		// only use the target if we think it is an .EXE
		if (GetTargetAttributes() & ImageExe)
		{
			CPath path;	
			COptionHandler * popthdlr;

			g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Linker),
													 (CBldSysCmp *&)popthdlr);
			CString strOutput;
			GetStrProp(popthdlr->MapLogical(P_OutName), strOutput);
			if (!strOutput.IsEmpty())
			{
				path.CreateFromDirAndFilename(GetWorkspaceDir(), strOutput);
				val = path.GetFullPath();
				return valid;
			}
			
		}
	}

	// If the return value wasn't 'valid', the prop isn't
	// in the prop bag (only place it could be).  Return
	// an empty string by default.
	if (gpt != valid)
		val = "";

	return valid;
}


///////////////////////////////////////////////////////////////////////////////
BOOL CProject::SetStrProp(UINT idProp, const CString& val)
{
	// Cached property bag pointer
	CConfigurationRecord *pcr = NULL;

	// Property bag list
	switch( idProp )
	{
		case P_ProjActiveConfiguration:
		{
			// is this different to our current config.?
			if (m_strProjActiveConfiguration.CompareNoCase(val) != 0)
			{
				// do we have this configuration?
				if ((pcr = ConfigRecordFromConfigName(val)) == NULL)
				{
					// we must be initialising the project
					pcr = CreateConfigRecordForConfig(val);
 				}

				// set ourselves in this config. (update the view)
				SetActiveConfig(pcr, FALSE);
			}
		}
		break;

		case P_ProjSccProjName:
		{
			if (((m_strProjSccProjName.IsEmpty()) && (!val.IsEmpty()))
				|| ((!m_strProjSccProjName.IsEmpty()) && (val.IsEmpty())))
			{
			// these scc props are write once for sharability, but
			// this seems too severe.
			// can be changed to empty, perhaps a stepping stone to a real change.
				m_strProjSccProjName = val;
				DirtyProject();
			}
			return TRUE; //??
		}
		break;
		case P_ProjSccRelLocalPath:
		{
			if (((m_strProjSccRelLocalPath.IsEmpty()) && (!val.IsEmpty()))
				|| ((!m_strProjSccRelLocalPath.IsEmpty()) && (val.IsEmpty())))
			{
			// these scc props are write once for sharability, but
			// this seems too severe.
			// can be changed to empty, perhaps a stepping stone to a real change.
				m_strProjSccRelLocalPath = val;
				DirtyProject();
			}
			return TRUE; //??
		}
		break;
		// is the target file name changing?
		case P_ExtOpts_Targ:
		case P_Proj_Targ:
		{
			// Cannot set the property to an empty string, as we must fake it.
			CString temp = val;
			temp.TrimLeft();
			temp.TrimRight();
			if (temp.IsEmpty())
			{
				CPropBag * pBag = GetPropBag();
				VSASSERT(pBag != (CPropBag *)NULL, "No property bag!");

				pBag->RemoveProp(idProp);
				InformDependants(P_ProjItemFullPath);

				return TRUE;
			}

			InformDependants(P_ProjItemFullPath);
		}
		break;
		case P_Proj_CleanLine:
		case P_Proj_CmdLine:
		{
			int i=0;
			CString strNew;
			int len = val.GetLength();
			while( i < len ){
				if( val[i] == _T('\t') ){
					strNew += _T("\r\n");
				}
				else {
					strNew += val[i];
				}
				i++;
			}
			return CProjContainer::SetStrProp(idProp, strNew);
		}
		break;

	}

	// do the base-class (CProjContainer) thing
	return CProjContainer::SetStrProp(idProp, val);
}

CConfigurationRecord *CProject::CreateConfigRecordForConfig(
												const CString & strConfig)
{
	//	Create a configuration record for the supplied name:
	if (ConfigRecordFromConfigName (strConfig) != NULL) return NULL;

	CConfigurationRecord *pcr = new CConfigurationRecord (0, this);
	pcr->m_pBaseRecord = pcr;	// Record is base for itself.

	m_ConfigArray.Add(pcr);
	m_ConfigMap.SetAt ( pcr, pcr );

	// Set the new configuration as active:
	pcr->GetPropBag(CurrBag)->SetStrProp ( this, P_ProjConfiguration, (CString &) strConfig);
	return pcr;
}

BOOL CProject::DeleteConfig ( const CString& strName, const CString& strNewConfig)
{
//	Nuke this configuration.  Note that this is not undoable, so the caller
//  needs to flush the undo stack after doing this.  Also, we don't say
//	anything about subprojects.

	// Find the configuration record to delete?
	CConfigurationRecord * pcr;
	if ((pcr = ConfigRecordFromConfigName(strName)) == (CConfigurationRecord *)NULL)
		return FALSE;

	// killing the active configuration?
	BOOL fKillingActive = pcr == GetActiveConfig();

	// Perform the delete...
	DeleteConfigFromRecord(pcr);

	// Set the active configuration to something other than this deleted one
	if (!m_ConfigMap.IsEmpty())
	{
		VSASSERT(m_ConfigArray.GetSize(), "No configs!");
		VSASSERT(m_ConfigArray.GetSize() == m_ConfigMap.GetCount(), "Config map/array mismatch");

		// Switch if we killed current....
		if (fKillingActive)
		{
			// If not got one to set as the current, then use first
			if (strNewConfig.IsEmpty())
			{
				SetStrProp(P_ProjActiveConfiguration, ((CConfigurationRecord *)m_ConfigArray[0])->GetConfigurationName());
			}
			else
			{
				SetStrProp(P_ProjActiveConfiguration, strNewConfig);
			}
		}

		// Changed the configuration property
		InformDependants (P_ProjConfiguration);
	}

	return TRUE;
}

BOOL CProject::CreateConfig(const CString& strName)
{
	BOOL fOK = FALSE;

	// Create a config. and make it the active one.
	CConfigurationRecord * pcrNewConfig = CreateConfigRecordForConfig(strName);
	if (pcrNewConfig == NULL)
		return FALSE;

 	m_pActiveConfig = pcrNewConfig;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
// IDE support functions:
int CProject::GetTargetAttributes()
{
    // BruceMa 11-05-98
    // There is a chance the debugger got a notification and is calling
    // in to get project attributes.  If the project is not complete yet
    // then just say we don't know
    if (!m_bProjectComplete)
    {
        return TargetUnknown;
    }

    
	// get the target attributes from the cache for the current config.
	CConfigurationRecord * pConfig = (CConfigurationRecord *)GetActiveConfig();
	VSASSERT(pConfig, "No active config!");
	return pConfig->GetTargetAttributes();
}

// the pMatchProject is the parent project or active config
// the this is the subproject or nonactive project
BOOL CProject::GetMatchingConfigName(CProject * pMatchProject, CString &strMatched, BOOL bInvalid)
{
	strMatched.Empty();

 	// Get a project pointer
	if (pMatchProject == (CProject *)ACTIVE_PROJECT )
		pMatchProject = g_pActiveProject;

	if(pMatchProject == NULL)
		return FALSE;
	
	// Get the name of the active config to match
	CString strMatchingDescription;
	CString strProjName;

	CConfigurationRecord * pcrMatch = pMatchProject->GetActiveConfig();
	if (pcrMatch==NULL) // can happen during project creation
		return FALSE;
	pcrMatch->GetConfigurationDescription(strMatchingDescription);

	// Get the fileset name
	CConfigurationRecord * pcr = GetActiveConfig();
	if(pcr == NULL)
		return FALSE;
	pcr->GetProjectName(strProjName);

	// Form the fileset name
	strMatched = strProjName + " - " + strMatchingDescription;

	// Lookup the fileset name in the target list
	if (!g_BldSysIFace.GetConfig(strMatched,NO_PROJECT))
	{
		// No configuration of the fileset that matches the current default project's
		// configuration, so we just pick a configuration that does exist for the fileset
		if (bInvalid)
		{
			CString strNoMatch;
			BOOL bOK = strNoMatch.LoadString(IDS_NO_MATCHING_CONFIG);
			VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
 			strMatched = strProjName + _T(" - ") + strNoMatch;
			return FALSE;
		}
		else
		{
			INT_PTR nSize = m_ConfigArray.GetSize();
			// loop thru the configs to find a good match

			strMatched.Empty();
			int nLongest = 0;
			int nMatchLen = strMatchingDescription.GetLength();
			for (INT_PTR nConfig = 0; nConfig < nSize; nConfig++)
			{
				pcr = (CConfigurationRecord *)m_ConfigArray[nConfig];
				CString strDescription;
				pcr->GetConfigurationDescription( strDescription );

				int nMax = strDescription.GetLength();
				if( nMax > nMatchLen )
					nMax = nMatchLen;

				// how many characters match 
				int i=0;
				while( i < nMax && strDescription[i] == strMatchingDescription[i] ){
					i++;
				}

				//     if this is longer than the longest so far,
				//			this is our new longest
				if( (i > nLongest) || strMatched.IsEmpty() ){
					nLongest = i;
					strMatched = pcr->GetConfigurationName();
				}
			}
			return FALSE;
		}
	}
 
	return TRUE;
}

CPlatform * CProject::GetCurrentPlatform()
{
	return (CPlatform *)GetProjType()->GetPlatform();
}

BOOL CProject::CanContain(CSlob* pSlob)
{
	if (pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)) ||
		pSlob->IsKindOf(RUNTIME_CLASS(CTargetItem)) )
		return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// SetActiveConfig()
///////////////////////////////////////////////////////////////////////////////
CConfigurationRecord * CProject::SetActiveConfig(CConfigurationRecord * pcr, BOOL /*fQuite*/)
{
	::EnterCriticalSection(&m_critSection);

	// make sure this is compatible with us, ie. a project-level config. rec.
	VSASSERT(pcr != (const CConfigurationRecord *)NULL, "NULL config record passed to SetActiveConfig!");
	VSASSERT(pcr->m_pBaseRecord == pcr, "Cannot set the active config to something we don't recognize");

	CConfigurationRecord * pcrOldActive = m_pActiveConfig;
	m_pActiveConfig = pcr;

	::LeaveCriticalSection(&m_critSection);
	return pcrOldActive;	// success, return old active configuration
}

CConfigurationRecord * CProject::SetActiveConfig(const CString& strConfigName, BOOL fQuite)
{
	CString strTarget = strConfigName;
	CConfigurationRecord * pcr = FindTargetConfig(strTarget);
	if (pcr != NULL)
		return SetActiveConfig(pcr, fQuite);	// success?

	VSASSERT (FALSE, "No config record to match config name!");
	return (CConfigurationRecord *)NULL;	// failure
}

///////////////////////////////////////////////////////////////////////////////
// FindTargetConfig() - Find the configuration matching the input string
//						and fix up the case to match the actual config name
///////////////////////////////////////////////////////////////////////////////
CConfigurationRecord * CProject::FindTargetConfig(CString & strTargetName)
{
	CConfigurationRecord * pcr;
	int i, cSize = GetPropBagCount();
	for (i = 0; i < cSize; i++)
	{
		pcr = (CConfigurationRecord *)m_ConfigArray[i];
		if (pcr->GetConfigurationName().CompareNoCase(strTargetName) == 0)
		{
			strTargetName = pcr->GetConfigurationName();
			return pcr;
		}
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// CProjTempConfigChange - used to temporarily change config of the project
//	                       in a thread-safe manner.
///////////////////////////////////////////////////////////////////////////////

CProjTempConfigChange::CProjTempConfigChange(CProject *pProject)
{
	VSASSERT(pProject != NULL, "Cannot set active project to NULL!");
	::EnterCriticalSection(&pProject->m_critSection);
	m_bCritSection = TRUE;
	m_pProject = pProject;
	m_pcrOriginal = pProject->m_pActiveConfig;
}

CProjTempConfigChange::~CProjTempConfigChange( )
{
	Release();
}

// Release resets the original configuration and frees up the critical section
void CProjTempConfigChange::Release(BOOL bReset /* = TRUE*/)
{
	if (m_bCritSection)
	{
		if (bReset)
			Reset();

		m_bCritSection = FALSE;
		::LeaveCriticalSection(&m_pProject->m_critSection);
	}
}

// Reset just sets the config back to the original. We still hold the
// critical section though.
void CProjTempConfigChange::Reset()
{
	VSASSERT(m_bCritSection, "Cannot reset active project while not under critical section!");
	ChangeConfig(m_pcrOriginal);
}


void CProjTempConfigChange::ChangeConfig(CConfigurationRecord *pcr)
{
	VSASSERT(m_bCritSection, "Cannot change config while not under critical section!");
	m_pProject->SetActiveConfig(pcr);
}

void CProjTempConfigChange::ChangeConfig(const CString& strConfigName)
{
	VSASSERT(m_bCritSection, "Cannot change config while not under critical section!");
	m_pProject->SetActiveConfig(strConfigName);
}

///////////////////////////////////////////////////////////////////////////////
// CProjTempProjectChange - used to temporarily change the active project
///////////////////////////////////////////////////////////////////////////////

CProjTempProjectChange::CProjTempProjectChange (CProject *pProject)
{

	m_pPrevProject = g_pActiveProject;
	g_pActiveProject = pProject;
}

CProjTempProjectChange::~CProjTempProjectChange ()
{
	Release ();
}

// Release resets the original project
void CProjTempProjectChange::Release ()
{
	g_pActiveProject = m_pPrevProject;
	m_pPrevProject = NULL; // probably doesn't matter, but what the hell...
}

///////////////////////////////////////////////////////////////////////////////
// GetTarget()
///////////////////////////////////////////////////////////////////////////////
CTargetItem* CProject::GetTarget(const TCHAR * pchTargetName)
{
	// Find the target item corresponding to a target name
	CTargetItem* pTarget;
	CString strKey = pchTargetName;
	strKey.MakeUpper();		// Case insensitive lookup
	if (!m_TargetMap.Lookup(strKey, (void* &)pTarget))
		return NULL;
	else
		return pTarget;
}

///////////////////////////////////////////////////////////////////////////////
// GetActiveTarget()
///////////////////////////////////////////////////////////////////////////////
CTargetItem* CProject::GetActiveTarget()
{
	CConfigurationRecord * pcr = GetActiveConfig();
	if (pcr == NULL)
		return NULL;

	return GetTarget(pcr->GetConfigurationName());
}

///////////////////////////////////////////////////////////////////////////////
// RegisterTarget()
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::RegisterTarget(const CString& strTargetName, CTargetItem* pTarget)
{
	// Check we have a configuration that matches the target
	int i;
	CConfigurationRecord* pcr;
	BOOL bFoundConfig = FALSE;
	int size = GetPropBagCount();
	for (i=0; i < size; i++)
	{
		pcr = (CConfigurationRecord*) m_ConfigArray[i];
		VSASSERT(pcr, "Bad config record in config array!");
		if (pcr->GetConfigurationName().CompareNoCase(strTargetName) == 0)
			bFoundConfig = TRUE;
	}
	if (!bFoundConfig)
		return FALSE;

	// Check the target item node
	ASSERT_VALID(pTarget);
	
	// Add this target to the target map
	CString strKey = strTargetName;
	strKey.MakeUpper();		// Case insensitive lookup
	m_TargetMap.SetAt(strKey, pTarget);

	// Make sure that this target is known by the build system interface
	g_BldSysIFace.GetConfig(strTargetName, (HPROJECT)this);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// DeregisterTarget()
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::DeregisterTarget(const CString& strTargetName, CTargetItem* /*pTarget*/)
{
	// Remove the target from the target map
	CString strKey = strTargetName;
	strKey.MakeUpper();		// Case insensitive lookup
 	return m_TargetMap.RemoveKey(strKey);
}

///////////////////////////////////////////////////////////////////////////////
// CreateTarget()
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::CreateTarget(const CString& strTargetName, CProjType* pProjType, const CString& strMirrorFrom /* = _T("") */)
{
	// Create the configuration for this target
	BOOL bRetVal = CreateConfig(strTargetName);

	CTargetItem* pTarget;
	if (strMirrorFrom.IsEmpty())
	{
		// Create the target item node for this target and
		// move it into the project tree
		pTarget = new CTargetItem();
	}
	else
	{
		// Mirroring a target
		pTarget = GetTarget(strMirrorFrom);
	}

	VSASSERT(pTarget != NULL, "No target?!?");

	// make this the target's config. current
	CProjTempConfigChange projTempConfigChange(this);
	projTempConfigChange.ChangeConfig(strTargetName);	

	SetStrProp(P_ProjOriginalType, *pProjType->GetName());

	if (strMirrorFrom.IsEmpty())
	{
		// move this new target into the project
		// (this will un-assign the build actions)
		pTarget->MoveInto(this);
	}
	else
	{
		// already got the target in the project, no
		// need for a move
	}

	// make this config. valid
	CConfigurationRecord * pcrTarg = pTarget->GetActiveConfig(TRUE);	// create
	VSASSERT(pcrTarg != NULL, "No target for active config?!?");
	pcrTarg->SetValid(TRUE);

	// reset the project's config.
	projTempConfigChange.Release();

	return RegisterTarget(strTargetName, pTarget);
}

///////////////////////////////////////////////////////////////////////////////
// DeleteTarget()
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::DeleteTarget(const CString& strTargetName)
{
	// Delete a target, by first removing any references
	// to the target, and by removing the target item node
	// itself

	// If we only have 1 valid config for this target item
	// then we can remove the target item, otherwise not
	HCONFIGURATION hConfig = g_BldSysIFace.GetConfig(strTargetName, (HPROJECT)this);
	HCONFIGURATION hConfigNew = NO_CONFIG;
	int nTargets = 0;
	CTargetItem* pTarget = GetTarget(strTargetName);
	VSASSERT(pTarget != NULL, "No CTargetItem for specified target name");
	const CVCPtrArray* pConfigArray = pTarget->GetConfigArray();
	INT_PTR nSize = pConfigArray->GetSize();
	for (INT_PTR i = 0; i < nSize; i++)
	{
		CConfigurationRecord* pcr = (CConfigurationRecord*)pConfigArray->GetAt(i);
		if (pcr->IsValid())
		{
			nTargets++;
			if (hConfigNew == NO_CONFIG)
			{
				// remember the target other than to be delted one
				CString strConfigName = pcr->GetConfigurationName();
				hConfigNew = g_BldSysIFace.GetConfig(strConfigName, (HPROJECT)this);
				if (hConfigNew == hConfig)
					hConfigNew = NO_CONFIG;	
			}
		}
	} 
	
	// make this the target's config. current
	CProjTempConfigChange projTempConfigChange(this);
	CConfigurationRecord * pcrOld = GetActiveConfig();

	projTempConfigChange.ChangeConfig(strTargetName);
	CConfigurationRecord * pcrProj = GetActiveConfig();

	if (nTargets == 1)
	{
		// Remove any dependencies on this fileset
		CProject * pProject;
		POSITION pos1 = m_lstProjects.GetHeadPosition();  // use our own private pos pointer
		while (pos1 != NULL)
		{
			pProject = (CProject *)m_lstProjects.GetNext(pos1);
			if ((pProject == NULL) || (!pProject->IsLoaded()))
			{
				continue ;
			}

			// Search the project tree for any nodes to delete
			CObList ol;
			int fo = CProjItem::flt_OnlyTargRefs;
			pProject->FlattenSubtree(ol, fo);

			POSITION pos = ol.GetHeadPosition();
			while (pos != NULL)
			{
				CProjectDependency * pProjDep = (CProjectDependency *)ol.GetNext(pos);
				
				// Is this a dependency on the fileset we are deleting if so then remove
				// the dependency
				if (pProjDep->GetProjectDep() == pTarget)
					pProjDep->MoveInto(NULL);
			}
		}
 
		// move this old target out of the project
		// (this will un-assign the build actions)
		pTarget->MoveInto(NULL);
	}
	else
	{
		// un-assign the build actions for this mirored target
		CBuildAction::UnAssignActions(pTarget, (CVCPtrList *)NULL, pcrProj);
	}

	// Reset only if pcrOld is not NULL, it might have been deleted already.
	projTempConfigChange.Release(pcrOld != (const CConfigurationRecord *)NULL);

	BOOL bRetVal = DeregisterTarget(strTargetName, pTarget);

	// If the delete operation succeeded then save the target
	// so other packages can be notified.
	HCONFIGURATION hSaveTarget;
	if (m_bProjectComplete)
		hSaveTarget = g_BldSysIFace.GetConfig(strTargetName, (HPROJECT)this);

	// Delete the configuration associated with this target
	bRetVal = bRetVal && DeleteConfig(strTargetName, "");

	// If we have deleted the default config then set the default config
	// to be a different one
	if (!m_strProjDefaultConfiguration.CompareNoCase(strTargetName))
	{
		CString strActiveProject;
		GetStrProp(P_ProjActiveConfiguration, strActiveProject);
		m_strProjDefaultConfiguration = strActiveProject;
	}

	return bRetVal;
}

void CProject::DirtyProjectEx()
{
	VSASSERT(!m_bProjectDirty, "Can only call DirtyProjectEx if project not already dirty");
	m_bOkToDirty = TRUE;
	m_bProjectDirty = TRUE;
}

CDir & CProject::GetProjDir(CConfigurationRecord * pcr)
{
    if (pcr == NULL)
        pcr = GetActiveConfig();

    // Make sure we are dealing with a base config record
    pcr = (CConfigurationRecord *)pcr->m_pBaseRecord;

	CTargetItem * pTarget = GetTarget(pcr->GetConfigurationName());
	if (pTarget != (CTargetItem *)NULL)
	{
		const TCHAR * pchTargDir = (const TCHAR *)pTarget->GetTargDir();
		if (*pchTargDir != _T('\0'))
		{
			m_ActiveTargDir.CreateFromString(pchTargDir);
			return m_ActiveTargDir;
		}
	}

	m_ActiveTargDir = m_ProjDir;

	// it is reset by specific target, init in SetFile w/ m_ProjDir
	return m_ActiveTargDir;  
}

BOOL CProject::IsCustomBuildMacroComment(CObject* pObject)
{
	// the object must be a comment
	if (!pObject->IsKindOf(RUNTIME_CLASS(CMakComment))) {
		return FALSE;
	}
	CMakComment* pMakComment = (CMakComment *)pObject;

	// get the comment text and remove leading white space
	TCHAR* pc = pMakComment->m_strText.GetBuffer(1);
	SkipWhite(pc);

	// look for the 'Begin Custom Macros' prefix
	CString strBeginMacros;
	BOOL bOK = strBeginMacros.LoadString(IDS_MAK_CUSTOM_BEGIN);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	if (_tcsnicmp(strBeginMacros, pc, strBeginMacros.GetLength()) == 0) {
		return TRUE;
	}
	return FALSE;
}

BOOL CProject::ReadCustomBuildMacros(CMakFileReader& mr)
{
	// we can just ignore these macros because they're only
	// written out to satisfy nmake

	// read until we hit a comment (should be 'End Custom Macros')
	for (CObject* pObject = mr.GetNextElement();
		!pObject->IsKindOf(RUNTIME_CLASS(CMakComment));
		pObject = mr.GetNextElement()) {
		delete pObject;
	}

	// verify the ending comment is 'End Custom Macros'
	CMakComment* pMakComment = (CMakComment *)pObject;

	// get the comment text and remove leading white space
	TCHAR* pc = pMakComment->m_strText.GetBuffer(1);
	SkipWhite(pc);

	// look for the 'End Custom Macros' prefix
	BOOL bResult = FALSE;
	CString strEndMacros;
	BOOL bOK = strEndMacros.LoadString(IDS_MAK_CUSTOM_END);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	if (_tcsnicmp(strEndMacros, pc, strEndMacros.GetLength()) == 0) {
		bResult = TRUE;
	}

	delete pObject;
	return bResult;
}

POSITION CProject::m_posProjects = NULL;
CObList CProject::m_lstProjects;

void CProject::InitProjectEnum()
{
	m_posProjects = m_lstProjects.GetHeadPosition();
}

const CProject * CProject::NextProjectEnum(CString & strProject, BOOL bOnlyLoaded /* = TRUE */)
{
	// FUTURE: handle bOnlyLoaded flag

	// serious hack
	BOOL bNoName = FALSE;
	if (strProject.Compare(_T("<no name>"))==0)
		bNoName = TRUE;
	else
		strProject.Empty();

	const CProject * pProject;
	while (m_posProjects!=NULL)
	{
		pProject = (const CProject *)m_lstProjects.GetNext(m_posProjects);
		if ((pProject != NULL) && (!bOnlyLoaded || pProject->IsLoaded()))
		{
			if (!bNoName) // hack: fails during destruction
			{
				// pProject->GetStrProp(P_ProjItemName, strProject);
				strProject = ((CProject *)pProject)->GetTargetName();
			}

			return pProject;
		}
	}
	
	return (const CProject *)NULL;
}

const CString & CProject::GetTargetName() { return m_strTargetName; }

//////////////////////////////////////////////////////////////////////////
