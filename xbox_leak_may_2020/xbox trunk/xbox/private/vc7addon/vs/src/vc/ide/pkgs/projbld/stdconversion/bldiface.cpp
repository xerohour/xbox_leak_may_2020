//
// CBldSysIFace
//
// Build System Interface
//
// [matthewt]
//

#include "stdafx.h"

#include "bldiface.h"	// local header
#include "optnlink.h"	// to include P_MachineType constant
#include "targitem.h"
#include "project.h"	// g_pActiveProject
#include "projdep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CBldSysIFace g_BldSysIFace;	// one 'n' only bld system interface

CProject *g_pActiveProject = NULL;

extern CProjComponentMgr * g_pPrjcompmgr;
extern BOOL g_bNoUI;		// defined in project.cpp

CString g_strTarg = _TEXT("");

CBldSysIFace::CBldSysIFace() {
}

CBldSysIFace::~CBldSysIFace() {
}

//
// Project object conversion
//
CProject * CBldSysIFace::CnvHProject(HPROJECT hProj)
{
	CProject * pProject = (CProject *)(hProj != ACTIVE_PROJECT ? hProj : GetActiveProject());
	return pProject;
}

CTargetItem * CBldSysIFace::CnvHConfig(HPROJECT hProj, HCONFIGURATION hConfig)
{
	if (hConfig==ACTIVE_CONFIG)
	{
		if (hProj == ACTIVE_PROJECT)
			hProj = (HPROJECT)g_pActiveProject;

		if (hProj != NO_PROJECT)
		{
			return ((CProject *)hProj)->GetActiveTarget();
		}
		return NULL;
	}
	
	if (hConfig==NO_CONFIG)
		return NULL;

	if (hProj == ACTIVE_PROJECT)
		hProj = GetProject(hConfig); // don't trust caller default

	if (hProj==NO_PROJECT)
		return NULL;

	return CnvHProject(hProj)->GetTarget(m_strlstConfigs.GetAt((POSITION)hConfig));
}

CTargetItem * CBldSysIFace::CnvHFileSet(HPROJECT hProj, HFILESET hFileSet)
{
	CTargetItem * pTargetItem = (CTargetItem *)(hFileSet != ACTIVE_FILESET ? hFileSet : (HFILESET)CnvHConfig(hProj, GetActiveConfig(hProj)));
	if( pTargetItem == NULL )
		return pTargetItem;

	ASSERT_VALID(pTargetItem);
	return pTargetItem;
}

HFILESET CBldSysIFace::GetFileSet(HPROJECT hProj, HCONFIGURATION hConfig)
{
	return (HFILESET)CnvHConfig(hProj, hConfig);
}

BOOL CBldSysIFace::GetFlavourFromConfigName
(
	const TCHAR * pchConfigName,
	CString & strFlavour
)
{
	// Form the whole configuration name
	CString strConfig = pchConfigName;
	int nFirst = strConfig.ReverseFind(_T('-'));
	VSASSERT(nFirst != -1, "Malformed config name!");
	nFirst += 2; // Skip over hyphen and trailing space

	CString strPlatformAndFlavour = strConfig.Right(strConfig.GetLength() - nFirst);
	CString strRemain;

	do 
	{
		CString strPlatform;
		CPlatform * pPlatform;	
		g_pPrjcompmgr->InitPlatformEnum();
		while (g_pPrjcompmgr->NextPlatform(pPlatform))
		{
			strPlatform = *(pPlatform->GetName());
			if (strPlatform.Compare(strPlatformAndFlavour.Left(strPlatform.GetLength())) == 0)
			{
				// Found the platform.
				strFlavour = strPlatformAndFlavour.Right(strPlatformAndFlavour.GetLength() - (strPlatform.GetLength() + 1));
				return TRUE;
 			}
		}
		strRemain = strConfig.Left(nFirst-2);
		nFirst = strRemain.ReverseFind(_T('-'));
		if (nFirst != -1 && (strRemain.GetLength()>4))
		{
			nFirst += 2;
			strPlatformAndFlavour = strConfig.Right(strConfig.GetLength() - nFirst);
		}
		else
		{
			// no more '-', we didn't get a standard config name, but...
			// olympus 1991 [patbr] look again using short name for Intel
			CString strWinPlatform("Win32");
			if (strWinPlatform.Compare(strPlatformAndFlavour.Left(strWinPlatform.GetLength())) == 0)
			{
				// Found the platform.
				strFlavour = strPlatformAndFlavour.Right(strPlatformAndFlavour.GetLength() - (strWinPlatform.GetLength() + 1));
				return TRUE;
			}
			else
			{
				VSASSERT(FALSE, "Don't understand any platform but Win32!");
				return FALSE;
			}
		}
	} while (1);

	return FALSE;
}

//
// Project query API
//

// Get the active project
HPROJECT CBldSysIFace::GetActiveProject() 
{
	return (HPROJECT)g_pActiveProject;
}

HPROJECT CBldSysIFace::GetProject(HCONFIGURATION hCfg)
{
	HPROJECT hProj;
	if (m_mapPrjConfigs.Lookup((void*)hCfg, (void *&)hProj))
		return (hProj);

	return NO_PROJECT;
}

//
// Project creation API
//
// Using this API, a project can be created, targets added, files added
// to those targets and tool settings & properties set for the files.
//
HPROJECT CBldSysIFace::CreateProject
(
	const TCHAR *	pchProjPath	// project path
)
{
	// Create the project object
	CProject * pOldProject = g_pActiveProject;
	CProject * pProject;
	pProject = new CProject;
	g_pActiveProject = pProject;

	CDir dirOld; dirOld.CreateFromCurrent();
	CPath path;

	if (pchProjPath != (const TCHAR *)NULL && path.Create(pchProjPath))
	{
		// set the current directory to match the pathname 
		if( !_tcsicmp( path.GetExtension(), ".mdp") ) {
			path.ChangeExtension(".mak");
			pchProjPath = (const TCHAR *)path;
		}

		CDir currentDir; currentDir.CreateFromPath(path);
		currentDir.MakeCurrent();
	}

	// No private data to initialize for a new project.
	// Pls. note active target is initialised by calls to CBldSysIFace::AddTarget()
	pProject->m_bPrivateDataInitialized = TRUE;

	if (!pProject->InitNew(NULL))
		goto CreationError;

	if (pchProjPath != NULL)
	{
		CPath pathProjName;
		if	(!pathProjName.Create(pchProjPath) ||
			 !pProject->SetFile(&pathProjName))
			goto CreationError;
	}
	else
	{
		CString strProjectName;
		BOOL bOK = pProject->GetStrProp(P_ProjItemName, strProjectName);
		VSASSERT(bOK, "Project property bag probably not properly associated.");
	}

	// return a pointer to this newly created project
	return (HPROJECT)pProject;

CreationError:

	g_pActiveProject = pOldProject;

	// Delete the project object
	if (pProject->m_bConvertedDS4x)
	{
		// may also need to delete temp projects
		POSITION pos = CProject::m_lstProjects.GetTailPosition();  // use our own private pos pointer
		while (pos != NULL)
		{
			pProject = (CProject *)CProject::m_lstProjects.GetPrev(pos);
			if (pProject->m_bConvertedDS4x)
			{
				delete pProject;
			}
		}
	}
	else
	{
		delete pProject;
	}
	g_pActiveProject = pOldProject;
 
	// set back the current directory since it fails to open
	dirOld.MakeCurrent();

	return NO_PROJECT;
}

HPROJECT CBldSysIFace::DeactivateProject(HPROJECT hProj)
{
	VSASSERT(hProj, "Invalid input parameter");
	CProject * pProject = (CProject *)hProj;

	// set active project to something else
	if (pProject && pProject == g_pActiveProject)
	{
		CProject * pActiveProject;
		g_pActiveProject = NULL;  	// we may no longer have an active project!
		POSITION pos = CProject::m_lstProjects.GetHeadPosition();  // use our own private pos pointer
		while (pos != NULL)
		{
			pActiveProject = (CProject *)CProject::m_lstProjects.GetNext(pos);
			if ((pActiveProject != NULL) && (pActiveProject != pProject))
			{
				ASSERT_VALID(pActiveProject);
				if (pActiveProject->IsLoaded())
				{
					HCONFIGURATION hConfig = GetActiveConfig((HPROJECT)pActiveProject);
					if (hConfig==NO_CONFIG)
					{
						CTargetItem * pTargetItem;
						CString strTargetName;
						pActiveProject->InitTargetEnum();
						pActiveProject->NextTargetEnum(strTargetName, pTargetItem);
						hConfig = GetConfig(strTargetName, (HPROJECT)pActiveProject);
						VSASSERT (hConfig, "No configs present in project?!?");
					}
					SetActiveConfig(hConfig, (HPROJECT)pActiveProject);
					break;
				}
				else
				{
					g_pActiveProject = pActiveProject; // at least we have one
				}
			}
		}
	}
	return (HPROJECT)g_pActiveProject;
}

// Close a project.
BOOL CBldSysIFace::CloseProject
(
	HPROJECT	hProj,			// project to close, if == ACTIVE_PROJECT, then close active project
	DeletionCause dcCause
)
{
	if (hProj == ACTIVE_PROJECT)
	{
		// actually closing the workspace
		CString strProject = _T("<no name>");
		BOOL retval = TRUE; // default
		// special case: close all projects
		InitProjectEnum();
		while ((retval) && ((hProj = GetNextProject(strProject, FALSE)) != NO_PROJECT))
		{
			VSASSERT(hProj != ACTIVE_PROJECT, "Failed to find the correct project!");
			g_bInProjClose = TRUE;
			retval = retval && CloseProject(hProj,dcCause);
			InitProjectEnum(); // hack: must re-init, since first project was just removed!
		}
		g_bInProjClose = FALSE;

		return retval;
	}

	VSASSERT(hProj != ACTIVE_PROJECT, "Failed to find the correct project!");
	// Get the project
   	CProject * pProject = CnvHProject(hProj);
	if (pProject == (CProject *)NULL)
		return TRUE;	// nothing to do

	hProj = (HPROJECT)pProject; // make sure not NULL
	if (!pProject->m_bProjectComplete)
	{
		VSASSERT(0, "Project not fully loaded before attempt to close it!");
		return FALSE;  // avoid recursion
	}

	// set active project to something else
	if (pProject == g_pActiveProject)
	{
		DeactivateProject((HPROJECT)pProject);
	}

	CString strName;
	pProject->GetName(strName);
	delete pProject;

	// if we are deleting this project from the workspace, then delete all the project deps that refer to it
	if (dcCause == ProjectDelete)
		::RemoveAllReferences(strName);

	// only do this when we're closing the very last project
	if (g_pActiveProject==NULL)
		m_lstFile.RemoveAll();

	return TRUE;	// success
}

// Get the active target if there is on, or NO_CONFIG if there isn't.
HCONFIGURATION CBldSysIFace::GetActiveConfig
(
	HPROJECT		hProj		// handle to the project containing the targets, default='active project'
)
{
	// get a project for this target to be searched in
	CProject * pProject = CnvHProject(hProj);

	if (pProject == NULL)
		return NO_CONFIG;

	if (pProject->GetActiveConfig() == NULL)
		return NO_CONFIG;

	// get our active target name
	CString strTarg = pProject->GetActiveConfig()->GetConfigurationName();

	return GetConfig((const TCHAR *)strTarg, (HPROJECT)pProject);
}

// Set the active target.
BOOL CBldSysIFace::SetActiveConfig
(
	HCONFIGURATION		hConfig,		// target to make the active one
	HPROJECT		hProj		// handle to the project containing the targets, default='active project'
)
{
	VSASSERT(hConfig != NO_CONFIG, "Only works for non-active config!");

	// get our target name
	CString strTarg = m_strlstConfigs.GetAt((POSITION)hConfig);

	// get a project for this target to be searched in
	if (hProj == ACTIVE_PROJECT)
		hProj = GetProject(hConfig); // don't trust caller default

	if( hProj == NO_PROJECT )
		return FALSE;

	CProject * pProject = CnvHProject(hProj);
	VSASSERT(pProject != NULL, "Failed to find even one project!");

	if (pProject != g_pActiveProject && g_pActiveProject)
	{
		if (pProject != g_pActiveProject)
			return FALSE;
	}

	// set this target as our active one
	BOOL bRet = TRUE;
	if (g_pActiveProject)
		bRet = g_pActiveProject->SetStrProp(P_ProjActiveConfiguration, strTarg);
	if(bRet)
		pProject->SetActiveConfig(strTarg);

	return bRet;	// ok?
}

// Get a target with name 'pchTarg' from the project, 'hProj'.
HCONFIGURATION CBldSysIFace::GetConfig
(
	const TCHAR *	pchTarg,	// name of this target to get
	HPROJECT		hProj		// handle to the project containing the target, default='active project'
)
{
	// null-terminated empty string?
	if (*pchTarg == _T('\0'))
		return NO_CONFIG;

	CString strKey = pchTarg;
	strKey.MakeUpper();	// case insensitive lookup
	HCONFIGURATION hConfig = (HCONFIGURATION)m_strlstConfigs.Find(strKey);
	
	if (hConfig == NO_CONFIG)
	{
		// It is okay for someone to ask us for a TARGET when there is no BUILDER. We should
		// just answer no.

		if (hProj != NO_PROJECT)
		{
			hConfig = (HCONFIGURATION)m_strlstConfigs.AddTail(strKey);
			m_mapPrjConfigs.SetAt((void *)hConfig, (void *)hProj);
		}
	}

	return hConfig;
}

BOOL CBldSysIFace::GetConfigName
(
	HCONFIGURATION		hConfig,		// target to get name of
	CString &		str,		// target name
	HPROJECT		hProj		// handle to the project containing the target, default='active project'
)
{
	if (hConfig == NO_CONFIG)
		return FALSE;

	hProj = GetProject(hConfig); // don't trust caller default

	if (hProj == NO_PROJECT)
		return FALSE;

	CConfigurationRecord * pcr = GetConfigRecFromConfigI(hConfig, hProj);
    if (pcr != NULL)
	    str = pcr->GetConfigurationName();
 	return (pcr != NULL);
}

// Create a target with name 'pchTarg' and add it to a project.
// Name of the 'official' platform must be specified in 'pchPlat', eg. "Win32 (x86)"
// Name of the target type must be specified in 'pchType', eg. "Application"
// Create debug or release default settings when a file is added to this target, 'fDebug'?
// A target can mirror another target's, 'hMirrorTarg', fileset.
// Function may fail (HCONFIGURATION == NO_CONFIG).
HCONFIGURATION CBldSysIFace::AddConfig
(
	const TCHAR *	pchTarg,				// name of this target

	const TCHAR *	pchPlat,				// name of this target's platform
	const TCHAR *	pchType,				// name of this target type
	BOOL			fUIDesc,				// platform and type are UI descriptions?
   
   	BOOL			fDebug,					// debug or release settings? default=debug

	TrgCreateOp		trgop,					// clone or mirrors another target? default=no
	HCONFIGURATION		hOtherTarg,				// target to clone or mirror

	SettingOp		setop,					// copy or default settings?
	HCONFIGURATION		hSettingsTarg,			// target to copy settings from 

	BOOL			bQuiet,					// show any warnings/errors during creation? default=yes
	BOOL			fOutDir,				// output directories? default=no
 	BOOL			fUseMFC,				// use MFC? default=yes
	HPROJECT		hProj					// project that contains target? default=('current'==ACTIVE_PROJECT) 
)
{
	VSASSERT(hProj != ACTIVE_PROJECT, "Specific project required!");
	if( hProj == ACTIVE_PROJECT || hProj == NO_PROJECT )
		return NO_CONFIG;

 	// a string version of our target type, eg. 'Win32 (x86) Application" 
	CString strTargName = CProjType::MakeNameFromPlatformAndType(pchPlat, pchType);

	CProjType * pProjType = NULL;
	if ((!fUIDesc && !g_pPrjcompmgr->LookupProjTypeByName((LPCTSTR)strTargName, pProjType)) ||
		(fUIDesc && !g_pPrjcompmgr->LookupProjTypeByUIDescription((LPCTSTR)strTargName, pProjType))
	   )
		return NO_CONFIG;

	// get a project for this target to be searched in
	CProject * pProject = CnvHProject(hProj);
	CProjTempProjectChange projChange(pProject);

	// store away the current config so we can reset it at the end of this
	// operation
	CString strActiveConfig;
	pProject->GetStrProp(P_ProjActiveConfiguration, strActiveConfig);

	// adjust our use of MFC
	fUseMFC = fUseMFC && g_pPrjcompmgr->MFCIsInstalled();

	// a string version of our target name
	CString strTarg;
	if (pchTarg != (const TCHAR *)NULL)
	{
		strTarg = pchTarg;	// user-supplied
		strTarg.TrimLeft();
		strTarg.TrimRight();
	}
	else
	{
		// auto-created by us
 		DefaultConfig(pProject, *(pProjType->GetPlatformUIDescription()), fDebug ? IDS_DEBUG_CONFIG : IDS_RELEASE_CONFIG, strTarg);
	}

	// string versions of our 'other' targets name
	CString strOtherTarg, strSettingsTarg;

	// do we have these?
	if (hOtherTarg != NO_CONFIG)
		strOtherTarg = m_strlstConfigs.GetAt((POSITION)hOtherTarg);
	else
		VSASSERT(trgop == TrgDefault, "with no other target we must have this");

	if (hSettingsTarg != NO_CONFIG)
		strSettingsTarg = m_strlstConfigs.GetAt((POSITION)hSettingsTarg);
	else
		VSASSERT(setop == SettingsDefault, "with no settings target we must have this!");

	CObList olItemList;
	POSITION pos;
	CProjItem *pItem;

	HCONFIGURATION hConfig = (HCONFIGURATION)GetConfig(strTarg, hProj);
	pProject->CreateTarget(strTarg, pProjType, trgop == TrgMirror ? strOtherTarg : _TEXT(""));

	CConfigurationRecord * pcrNewTarg = GetConfigRecFromConfigI(hConfig, hProj);
	CTargetItem * pNewTarget = pProject->GetTarget(strTarg);
	const CPlatform * pNewPlatform = pProjType->GetPlatform();

	// set target dir
	if (!g_strTarg.IsEmpty())
		pNewTarget->SetTargDir(g_strTarg);

	if (pNewTarget->GetTargetName().IsEmpty())
	{
		int index = strTarg.Find(" - ");
		VSASSERT(index > 0, "Malformed config name");
		CString strTargetName = strTarg.Left(index);
		pNewTarget->SetTargetName(strTargetName);
		if (pProject->GetTargetName().IsEmpty())
			pProject->SetTargetName(strTargetName);
		VSASSERT(strTargetName == pProject->GetTargetName(), "Target name doesn't match that for project!");
	}

	// establish output dirs for new target?
	CString strProjDir = _TEXT("");
	CString strOutDirs;
	if (fOutDir)
	{
		if (!g_strTarg.IsEmpty() )
		{
			CPath path;
			CDir  dir;

			TCHAR buffer[MAX_PATH];
			_tcscpy( buffer, (LPCSTR)g_strTarg );
			int nLen = lstrlen( buffer );

			if( *_tcsdec( buffer, buffer+ nLen ) != _T('\\') ) {
				_tcscat( buffer, _T("\\") );
			}
			// add a dummy file for CPath
			_tcscat( buffer, _T("a") );

			path.Create( buffer );
			dir = pProject->GetWorkspaceDir();

			path.GetRelativeName( dir, strProjDir );
			// remove the dumy filename
			strProjDir = strProjDir.Left( strProjDir.GetLength() - 1 );


			// remove the leading ".\"  if GetRelativeName succeeded
			if ( (strProjDir.Find('.') == 0) && (strProjDir.Find('\\') == 1)  ) {
				strProjDir = strProjDir.Right(strProjDir.GetLength()-2);
			}

			// remove ending '\\'
			int ich = strProjDir.ReverseFind('\\');
			if (ich != -1 && ich == strProjDir.GetLength()-1)
			{
				strProjDir = strProjDir.Left(strProjDir.GetLength()-1);	
			}
		}
		if (pchTarg != (const TCHAR *)NULL)
		{
			if (!g_strTarg.IsEmpty())
			{
				CPath path;
				CString strConfig;
				GetFlavourFromConfigName(strTarg, strConfig); 

				if ( (strConfig.Compare("Debug") == 0) || (strConfig.Compare("Release") == 0) )
				{
					strOutDirs = *(pProjType->GetPlatform()->GetDefOutDirPrefix());
					if (strOutDirs.IsEmpty())
					{
						// x86 platform
						strOutDirs += fDebug ? _T("Debug") : _T("Release");
					}
					else
					{
						// others
						strOutDirs += fDebug ? _T("Dbg") : _T("Rel");
					}
					if (path.CreateFromDirAndFilename(pProject->GetProjDir(), strOutDirs) 
						&& path.ExistsOnDisk())
					{
						GetNewOutputDirName(pProject, strTarg, strOutDirs);
					}
				}
				else
				{
					// added config, use config name as default dir
					strOutDirs += strConfig;
				}
			}
			else
			{
				// attempt to generate a unique name 
				GetNewOutputDirName(pProject, strTarg, strOutDirs);
			}
		}
		else
		{
			// set up defaults for intermediate and target directories
			strOutDirs = *(pProjType->GetPlatform()->GetDefOutDirPrefix());
			if (strOutDirs.IsEmpty())
			{
				strOutDirs += fDebug ? _T("Debug") : _T("Release");
			}
			else
			{
				strOutDirs += fDebug ? _T("Dbg") : _T("Rel");
			}
		}

		VSASSERT(!strOutDirs.IsEmpty(), "Output dir must be non-blank!");

		if (!g_strTarg.IsEmpty() )
		{
			CString strTmp;
			if (!strProjDir.IsEmpty())
			{
				strTmp = strProjDir + _T("\\") + strOutDirs;
			}
			else
			{
				strTmp = strOutDirs;
			}
			strOutDirs = strTmp;
		}

		if ( pProjType && 
			( (pProjType->GetUniqueTypeId() == CProjType::exttarget) ||
			  (pProjType->GetUniqueTypeId() == CProjType::generic) )
		   )
		{
			// for makefile or utility projects, the output directory should be the current directory.
			strOutDirs = "";
		}


		CString strBlank;
		pProject->SetStrProp(P_OutDirs_Intermediate, strOutDirs);
		pProject->SetStrProp(P_OutDirs_Target, strOutDirs);
		pProject->SetStrProp(P_Proj_TargDir, strProjDir);
		pProject->SetStrProp(P_ConfigDefaultDebugExe, strBlank);
	}

	// WINSLOWF
	pcrNewTarg->SetBuildToolsMap();
	// assign the build actions
	CBuildAction::AssignActions(pNewTarget, (CVCPtrList *)NULL, pcrNewTarg);
				

	// do the target-level settings
	if (setop != SettingsClone)
	{
		// create a fresh configuration here

		// set the debug/release default settings	
		pProject->SetIntProp(P_UseDebugLibs, fDebug); // used for MFC libs

		// set a deferred browser database make
		pProject->SetIntProp(P_NoDeferredBscmake, FALSE);

		if (fDebug)
			g_pPrjcompmgr->SetDefaultDebugToolOptions(pProject);
		else
			g_pPrjcompmgr->SetDefaultReleaseToolOptions(pProject);

		pProject->SetStrProp(P_ProjActiveConfiguration, strTarg);
		if (fDebug)
			pProjType->SetDefaultDebugTargetOptions(pProject, pNewTarget, fUseMFC);
		else
			pProjType->SetDefaultReleaseTargetOptions(pProject, pNewTarget, fUseMFC);
	}
	else
	{
		VSASSERT(setop == SettingsClone, "must be clone");

		// we are creating a new config by cloning from an existing one

	 	// get the platform for the original 'settings' target
	    CConfigurationRecord * pcrOldTarg = GetConfigRecFromConfigI(hSettingsTarg, hProj);

		CProjTempConfigChange projTempConfigChange(pProject);
		projTempConfigChange.ChangeConfig(pcrOldTarg);

		const CPlatform* pOldPlatform = pProject->GetProjType()->GetPlatform();
		projTempConfigChange.Release();

		// cross-platform?
		// if so then set up our default settings first
		if (pOldPlatform != pNewPlatform)
		{
			// copy in default tool settings
			if (fDebug)
				g_pPrjcompmgr->SetDefaultDebugToolOptions(pProject);
			else
				g_pPrjcompmgr->SetDefaultReleaseToolOptions(pProject);
		}

		// CreateConfig()  creates property bag ONLY in the project level, 
		// and the lower level will be created when SetStrProp(P_ProjActiveConfiguration,..)
		// is called.
		pProject->CopyCommonProps(pProject, strSettingsTarg);
		
		// after the prop bag is cloned
		// we need to reset
		// o original project type
		// o project configuration name
		// o active configuration name
		pProject->SetStrProp(P_ProjOriginalType, *pProjType->GetName());
		pProject->SetStrProp(P_ProjConfiguration, strTarg);
		pProject->SetStrProp(P_ProjActiveConfiguration, strTarg);

		// cross-platform?
		// if so then set up our us of MFC
		if (pOldPlatform != pNewPlatform)
		{
			// if we were using MFC make sure we use the right default
			// o we want NoMFC if none, or the library if dll is not supported
			int iUseMFC, iUseMFCDef;

			if (fUseMFC)
				iUseMFCDef = (pNewPlatform->GetAttributes() & PIA_Allow_MFCinDLL) ? UseMFCInDll : UseMFCInLibrary;
			else
				iUseMFCDef = NoUseMFC;

	 		if (!pProject->GetIntProp(P_ProjUseMFC, iUseMFC) || iUseMFC > iUseMFCDef)
				iUseMFC = iUseMFCDef;

			pProject->SetIntProp(P_ProjUseMFC, iUseMFC);

			// not handled in ::InformDependants do it here 
			if (!pProject->m_bProjectComplete)
				// we need to make sure we have the right setup
				// for an MFC project
				pProjType->PerformSettingsWizard(pProject, iUseMFC);

			// Set up the Machine option as this will be incorrect
			UINT nPlatformId = pNewPlatform->GetUniqueId();
			UINT nMachineProp;
			switch (nPlatformId)
			{
				case win32x86:		nMachineProp = 2;	break;
				case win16x86:		nMachineProp = 1;	break;
				case mac68k:		nMachineProp = 3;	break;
				case macppc:		nMachineProp = 5;	break;
				case win32alpha:	nMachineProp = 4;	break;
				default:			nMachineProp = 2;	break;	// default to x86
 			}

			COptionHandler * popthdlr;
			g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Linker),
				(CBldSysCmp *&)popthdlr);
 			pProject->SetIntProp (popthdlr->MapLogical(P_MachineType), nMachineProp);
 		}

		// also need to inform dependants that things are changing
		pProject->InformDependants(SN_ALL);
	} 

	if (fOutDir)
	{
		pProject->SetStrProp(P_OutDirs_Intermediate, strOutDirs);
		pProject->SetStrProp(P_OutDirs_Target, strOutDirs);
		pProject->SetStrProp(P_Proj_TargDir, strProjDir);
	}

	// do the file-level settings
	if (setop != SettingsClone)
	{
		// Copy any files we need to
		if (trgop == TrgCopy)
		{
			// Get the target to copy from and the target to copy to.
			CTargetItem* pTarget = pProject->GetTarget(strOtherTarg);
 			ASSERT_VALID(pTarget);
			ASSERT_VALID(pNewTarget);

 			// Now copy all the files
			HFILESET hFileSet = GetFileSet(ACTIVE_PROJECT, hConfig);
			for (POSITION pos = pTarget->GetHeadPosition(); pos != NULL;)
			{
				CObject* pOb = pTarget->GetNext(pos);
				// We copy only fileitems except target references
 				if (pOb->IsKindOf(RUNTIME_CLASS(CFileItem)))
  					(void)AddFile(hFileSet, ((CFileItem *)pOb)->GetFilePath(), FALSE, hProj);
  			}
		}
	}
	else
	{
		// Get the target to copy from and the target to copy to.
		CTargetItem* pTarget = pProject->GetTarget(strOtherTarg);
		CTargetItem* pNewTarget = pProject->GetTarget(strTarg);
		ASSERT_VALID(pTarget);
		ASSERT_VALID(pNewTarget);

		// Copy any files we need to
		if (trgop == TrgCopy)
		{
 			// Now copy all the files
			HFILESET hFileSet = GetFileSet(ACTIVE_PROJECT, hConfig);
			for (POSITION pos = pTarget->GetHeadPosition(); pos != NULL;)
			{
				CObject* pOb = pTarget->GetNext(pos);
 				if (pOb->IsKindOf(RUNTIME_CLASS(CFileItem)))
  					(void)AddFile(hFileSet, ((CFileItem *)pOb)->GetFilePath(), FALSE, hProj);
  			}
		}

		// we have to clone the CProjItem's property bags as well
		// but we can only do this AFTER setting the P_ProjActiveConfiguration 
		// property (above), since that is the time where the property bags
		// at the lower lever are created.
		// 
		// First, flatten the destination subtree
 		pNewTarget->FlattenSubtree(olItemList, CProjItem::flt_Normal | 
								   CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjDeps);
		if (trgop == TrgCopy)
		{
			// We are copying files so the src and destination items are NOT the same, so
			// we must flatten the src target subtree too
			CObList olSrcItemList;
			pTarget->FlattenSubtree(olSrcItemList, CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | 
									CProjItem::flt_ExcludeProjDeps);

			// Now we iterate through the subtrees, getting corresponding items from each flattened subtree
			POSITION posSrc, posDest;
            CProjItem *pSrcItem, *pDestItem;
			for (posSrc = olSrcItemList.GetHeadPosition(), posDest = olItemList.GetHeadPosition(); 
				 posSrc != NULL && posDest != NULL ;)
			{
				// Get the destination and src projitems
				pDestItem = (CProjItem *) olItemList.GetNext(posDest);
				pSrcItem = (CProjItem *) olSrcItemList.GetNext(posSrc);

				// Clone the property bag
				pDestItem->CopyCommonProps(pSrcItem, strSettingsTarg);

				// Set the base settings to be the same as the current settings at the
				// time of the copy
				pDestItem->SetCurrentConfigAsBase(); // copy the prop bag to default prop bag

				// also need to inform dependants that things are changing
				pDestItem->InformDependants(SN_ALL);
 			}
		}
		else
		{
			VSASSERT(trgop == TrgMirror, "Must be mirror");

			// We are mirroring which means the src and destination items are the same
			// So loop through the destination list of projitems
			for (pos = olItemList.GetHeadPosition(); pos != NULL ;)
			{
				// Get the src, and destination projitem
				pItem = (CProjItem *) olItemList.GetNext(pos);
			 	
			 	// Copy the current settings
			 	pItem->CopyCommonProps(pItem, strSettingsTarg);

                // Set the base settings to be the same as the current settings at the
                // time of the copy
                pItem->SetCurrentConfigAsBase(); // copt the prop bag to default prop bag

				// also need to inform dependants that things are changing
				pItem->InformDependants(SN_ALL);
 			}
		}
	}

	// finally make sure that we remember the 'base' (default) options
 	pProject->SetCurrentConfigAsBase();

	// set 'default' config to first debug config. created
 	if (fDebug &&
 		(pProject->m_strProjDefaultConfiguration.IsEmpty() ||
		 pProjType->GetPlatform()->IsPrimaryPlatform())			 
	   )
	{
		pProject->m_strProjDefaultConfiguration = strTarg;
	}

	// Make sure we are in the same config we were in when we
	// started to do this operation
	if (!strActiveConfig.IsEmpty())
		pProject->SetStrProp(P_ProjActiveConfiguration, strActiveConfig);

	return hConfig;
}

BOOL CBldSysIFace::AddDefaultConfigs
(
	const TCHAR *	pchPlat,				// UI or 'official' name of this target's platform
	const TCHAR *	pchType,				// UI or 'official' name of this target type

	HCONFIGURATION &	hDebugTarg,				// our created debug target
	HCONFIGURATION &	hReleaseTarg,			// our created release target

	BOOL			fUIDesc,				// using UI or 'official' names? default=UI
	BOOL			fOutDir,				// output directories? default=yes
	BOOL			fUseMFC,				// use MFC? default=yes

	HCONFIGURATION		hMirrorTarg,			// do we want to 'chain the mirroring'?
	HPROJECT		hProj,					// project that contains target? default='active project'
	const TCHAR *	pchTarg 				// name of this target, default use NULL to official names
)
{
	// get a project for this target to be searched in
	CProject * pProject = CnvHProject(hProj);
	hProj = (HPROJECT)pProject; // convert ACTIVE_PROJECT to actual value;
	if( hProj == NO_PROJECT )
		return FALSE;

	g_strTarg = pProject->GetWorkspaceDir();
	
	CProjTempProjectChange projChange(pProject);

	hDebugTarg = hReleaseTarg = NO_CONFIG;

	CString strTarg;

	// a string version of our target type, eg. 'Win32 (x86) Application" 
	CString strTargName = CProjType::MakeNameFromPlatformAndType(pchPlat, pchType);

	// Get the project type for these targets
	CProjType * pProjType = NULL;
	if ((!fUIDesc && !g_pPrjcompmgr->LookupProjTypeByName((LPCTSTR)strTargName, pProjType)) ||
		(fUIDesc && !g_pPrjcompmgr->LookupProjTypeByUIDescription((LPCTSTR)strTargName, pProjType))
	   )
		return FALSE;

	//
	// create our release config. for this project type
	//

	if (pchTarg != NULL)
	{
		CString strMode, strPlatformUIDescription;
		BOOL bOK = strMode.LoadString(IDS_RELEASE_CONFIG);
		VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
		strPlatformUIDescription = *(pProjType->GetPlatformUIDescription());
		strTarg = pchTarg;	// user-supplied
		strTarg += _T(" - ") + strPlatformUIDescription + _T(" ") + strMode;
	}
	else
	{
		strTarg = _TEXT("");
	}

	hReleaseTarg = AddConfig
	(
		(pchTarg!=NULL) ? (LPCTSTR)strTarg:NULL,

		pchPlat,				// 'official' name of this target's platform
		pchType,				// 'official' name of this target type
		fUIDesc,				// we're using 'official' names

		FALSE,					// release settings

		hMirrorTarg == NO_CONFIG ? TrgDefault : TrgMirror,	// default or 'chain-the-mirroring' target? default=no chaining
		hMirrorTarg,										// no target to clone or mirror

		SettingsDefault,		// default settings
		NO_CONFIG,				// no target to copy settings from
		 
		FALSE,					// show any warnings/errors during creation? default=yes
		fOutDir,				// create output directories?
		fUseMFC,				// use MFC?
		hProj					// project that contains target? default='active project'
	);
	if (hReleaseTarg == NO_CONFIG)
		return FALSE;	// failure

	//
	// create our debug config. for this project type
	//
	
	if (pchTarg != NULL)
	{
		CString strMode, strPlatformUIDescription;
		BOOL bOK = strMode.LoadString(IDS_DEBUG_CONFIG);
		VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
		strPlatformUIDescription = *(pProjType->GetPlatformUIDescription());
		strTarg = pchTarg;	// user-supplied
		strTarg += _T(" - ") + strPlatformUIDescription + _T(" ") + strMode;
	}
	else
	{
		strTarg= _TEXT("");
	}

	hDebugTarg = AddConfig
	(
		(pchTarg!=NULL) ? (LPCTSTR)strTarg:NULL,

		pchPlat,				// 'official' name of this target's platform
		pchType,				// 'official' name of this target type
		fUIDesc,				// we're using 'official' names

		TRUE,					// debug settings

		TrgMirror,				// default target
		hReleaseTarg,			// mirror other default target

		SettingsDefault,		// default settings
		NO_CONFIG,				// no target to copy settings from
		 
		FALSE,					// show any warnings/errors during creation? default=yes
		fOutDir,				// create output directories?
		fUseMFC,				// use MFC?
		hProj					// project that contains target? default='active project'
	);
	if (hDebugTarg == NO_CONFIG)
		return FALSE;	// failure

 	// set active config to be the default
	pProject->SetStrProp(P_ProjActiveConfiguration, pProject->m_strProjDefaultConfiguration);
	g_strTarg = "";
 
	return TRUE;	// success
}

// Delete an existing target
BOOL CBldSysIFace::DeleteConfig
(
	HCONFIGURATION	hConfig,		// target to delete
	HPROJECT		hProj			// project that contains target? default='active project'
)
{
	// get a project for this target to be searched in

	VSASSERT(NO_PROJECT!=hProj, "Must specify particular project!");
	HPROJECT hCmpBld = GetProject(hConfig);
	VSASSERT(NO_PROJECT!=hCmpBld, "Must specify particular config!");
	VSASSERT(hProj == hCmpBld, "Config doesn't come from specified project!");
	hProj = hCmpBld;
	VSASSERT(hProj, "Config doesn't belong to a project!");

	CProject * pProject = CnvHProject(hProj);
	VSASSERT(pProject, "Failed to pick up the correct project!");

	if( pProject == NULL )
		return FALSE;

	// Get the name of the target to delete
	CString strTarget;
	GetConfigName(hConfig, strTarget, hProj);

	const CVCPtrArray* pConfigArray = pProject->GetConfigArray();
	INT_PTR nSize = pConfigArray->GetSize();

	if (pProject->m_bProjectComplete)
	{
		// if we're destroying the project, make sure it is not active
		if ((nSize==1) && (pProject == g_pActiveProject))
			DeactivateProject(hProj);
	}

	// Delete the target
	BOOL bRetVal = pProject->DeleteTarget(strTarget);

	// Remove the target from our list of targets if we succeeded in deleting it
	if (bRetVal)
	{
		m_mapPrjConfigs.RemoveKey((void *)hConfig);
 		m_strlstConfigs.RemoveAt((POSITION)hConfig);
	}

	pConfigArray = pProject->GetConfigArray();
	nSize = pConfigArray->GetSize();
	if (pProject->m_bProjectComplete)
		VSASSERT(bRetVal, "Something failed during DeleteConfig!");

	// success?
	return bRetVal;
}

// Create a file and add it to a target, 'hConfig', with or without default settings, 'fSettings'.
// Function may fail (HBLDFILE == NO_FILE).
HBLDFILE CBldSysIFace::AddFile
(
	HFILESET		hFileSet,				// handle to the fileset
	const CPath *	ppathFile,				// file path
	int				fSettings,				// default settings for this fileitem
	HPROJECT		hBld,					// builder that contains target? default='active builder' 
	HFOLDER			hFolder,				// default is NO_FOLDER
	BOOL			fDeploy,				// for deploy outputs allow duplicate files
	BOOL			fNoFilter				// do not filter files by extension
)
{
	// get a builder for this target to be added to
	if (hBld == ACTIVE_PROJECT)
		hBld = GetActiveProject();

	if( hBld == NO_PROJECT )
		return NO_FILE;

	CProject * pProject = CnvHProject(hBld);
	CProjTempProjectChange projChange(pProject);

	// get the target item 
	CTargetItem * pTarget = CnvHFileSet(hBld, hFileSet);
	if (pTarget == NULL)
		return NO_FILE;

	CDir dir;
	// special logic to identify template files so that they go
	// in the right folder and so that ClassView never sees them
	BOOL bIsTemplate = FALSE;
	if (dir.CreateFromPath(*ppathFile))
	{
		CString strDir = dir;
		strDir.MakeLower();
		int nTemplate, nEnd;
		if ((nTemplate = strDir.Find("\\template")) != -1)
		{
			nEnd = nTemplate + lstrlenA("\\template");
			if ((nEnd >= strDir.GetLength()) || (strDir[nEnd] == '\\'))
				bIsTemplate = TRUE;
		}
	}
	CProjContainer * pContainer = (CProjContainer *)hFolder;  // use specified folder, if any
	if ((pContainer == NULL) || (!pContainer->IsKindOf(RUNTIME_CLASS(CProjContainer))))
	{
		pContainer = pTarget;  // by default, just add to the target
		CProjItem * pItem;

		CString strFilter;
		CString strTemplateFilter;
		strTemplateFilter.LoadString(IDS_TEMPLATE_FILES_FILTER);
		CObList ol;
 		pTarget->FlattenSubtree(ol, CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjDeps);
		POSITION pos = ol.GetHeadPosition();
		while (pos != NULL && !fNoFilter)
		{
			pItem = (CProjItem *)ol.GetNext(pos);
			VSASSERT(pItem->IsKindOf(RUNTIME_CLASS(CProjItem)), "Non-CProjItem in project item list!");
			if (pItem->IsKindOf(RUNTIME_CLASS(CProjGroup)))
			{
				if ((pItem->GetStrProp(P_GroupDefaultFilter, strFilter)==valid) && (!strFilter.IsEmpty()))
				{
					if (strFilter == strTemplateFilter) // "<templates>"
					{
						if (bIsTemplate)
						{
							pContainer = (CProjContainer *)pItem;
							break;
						}
					}
					else if (FileNameMatchesExtension(ppathFile, strFilter))
					{
						// found appropriate group to add this too
						if (pContainer == pTarget) // make sure we're overriding the default
							pContainer = (CProjContainer *)pItem;
						if (!bIsTemplate)
							break;
					}
				}
			}
		}
	}
	
	// create the file
	CFileItem * pFile;
	if ((pFile = new CFileItem) == (CFileItem *)NULL)
		return NO_FILE;

	// set the file path
	pFile->SetFile(ppathFile);

	// move the file into the target
	pFile->MoveInto(pContainer);
	// For 98' set deploy property to be true
	pFile->SetIntProp(P_ItemDeployMode, DoNotDeploy);
	{
		pFile->SetIntProp(P_ItemDeployMode, DoNotDeploy);

		// WinslowF -- Bug 
		int iSize = pTarget->GetPropBagCount();
		VSASSERT(iSize, "Target must have at least one property bag!");
		const CVCPtrArray * pArrayCfg = pTarget->GetConfigArray();
		for (int i = 0; i < iSize; i++)
		{
			CConfigurationRecord * pcr = (CConfigurationRecord *)(*pArrayCfg)[i]; 
			if (pcr->IsValid())
			{
				CProjTempConfigChange projTempConfigChange(pProject);
				projTempConfigChange.ChangeConfig((CConfigurationRecord *)pcr->m_pBaseRecord);
				pFile->SetIntProp(P_ItemDeployMode, DoNotDeploy);
				int idOldBag = pFile->UsePropertyBag(BaseBag);
				pFile->SetIntProp(P_ItemDeployMode, DoNotDeploy);
				(void) pFile->UsePropertyBag(idOldBag);
			}
		}
	}

	// return the newly created file
	return (HBLDFILE)pFile;
}

// Map an external property number to an internal property number
static UINT mapPropToPropI[] =
{
	P_ItemExcludedFromBuild,		// property 0
	P_ProjUseMFC,					// property 1
	P_ProjAppWizUseMFC,				// property 2
	0,								// property 3
	0,								// property 4
	P_OutDirs_Intermediate,			// property 5
	P_OutDirs_Target,				// property 6
	P_Proj_TargDefExt,				// property 7
};

// Get the config. record for a target, 'hConfig'
CConfigurationRecord * CBldSysIFace::GetConfigRecFromConfigI
(
	HCONFIGURATION		hConfig,	// handle to the target
	HPROJECT		hProj	// project that contains target? default=('current'==ACTIVE_PROJECT) 
)
{
	// get the target file CProject
	CProject * pItem = CnvHProject(hProj);
	if( pItem == NULL )
		return NULL;
	
	VSASSERT(pItem->IsKindOf(RUNTIME_CLASS(CProject)), "Bad project pointer specified");

	// get our target name
	CString strTarg = m_strlstConfigs.GetAt((POSITION)hConfig);

	// get this target config record
	return pItem->ConfigRecordFromConfigName((const TCHAR *)strTarg);
}

// Enumerate projects
void CBldSysIFace::InitProjectEnum()
{
	CProject::InitProjectEnum();
}

HPROJECT CBldSysIFace::GetNextProject(CString & strProject, BOOL bOnlyLoaded /* = TRUE */)
{
	return (HPROJECT)(CProject::NextProjectEnum(strProject, bOnlyLoaded));
}

// Enumerate targets
void CBldSysIFace::InitConfigEnum(HPROJECT hProj)
{
	// get a project for this target to be searched in
	CProject * pProject = CnvHProject(hProj);
	if (pProject == (CProject *)NULL)
		return;

	pProject->InitTargetEnum();
}

HCONFIGURATION CBldSysIFace::GetNextConfig(CString & strTarget, HPROJECT hProj)
{
	// get a project for this target to be searched in
	CProject * pProject = CnvHProject(hProj);
	if (pProject == (CProject *)NULL)
		return NO_CONFIG;

	CTargetItem * pTargetItem;
	if (!pProject->NextTargetEnum(strTarget, pTargetItem))
		return NO_CONFIG;

	return GetConfig(strTarget, hProj);
}

// Enumerate files
void CBldSysIFace::InitFileEnum(HPROJECT hProj)
{
	// FUTURE: extend to allow enum of multiple fileset files cache
	m_lstFile.RemoveAll();
	m_posHFile = (POSITION)NULL;

	// get our list to enumerate
	int fo = (CProjItem::flt_ExcludeProjDeps | CProjItem::flt_ExcludeGroups);	// remove project deps
	CTargetItem * pTarget = CnvHConfig(hProj, GetActiveConfig(hProj));
	if (pTarget)
		pTarget->FlattenSubtree(m_lstFile, fo);

	// get start of enumeration
	m_posHFile = m_lstFile.GetHeadPosition();
}

HBLDFILE CBldSysIFace::GetNextFile()
{
	if (m_posHFile == (POSITION)NULL)
		return (HBLDFILE)NO_FILE;

	CFileItem * pItem = (CFileItem *)m_lstFile.GetNext(m_posHFile);
	VSASSERT(pItem->IsKindOf(RUNTIME_CLASS(CFileItem)), "Non-file in file set!");
	if (!pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
		return (HBLDFILE)NO_FILE;
	}

	return (HBLDFILE)pItem;
}												

//
// Project file section support
//
// Provide a callback function that will be called when 
// a buffer needs to be provided to be written or a 
// buffer has been read and can be processed.
//
// Pls. note that sections are distinguished by name.

// UNDONE: fix for multiple project model
BOOL CBldSysIFace::RegisterSection(CBldrSection * pbldsect)
{
	void * pvDummy;
	CString strKey;

	if (pbldsect->m_hBld == NULL)
	{
		pbldsect->m_hBld = GetActiveProject();
	}
	strKey.Format("%d:%s", pbldsect->m_hBld, pbldsect->m_strName);


	// check the validity of the section
	if (pbldsect->m_strName.IsEmpty() ||					// got a name?
		m_mapSections.Lookup(strKey, pvDummy)	// unique?
	   )
		return FALSE;

	m_mapSections.SetAt(strKey, (void *)pbldsect);
	return TRUE;	// success
}

BOOL CBldSysIFace::DeregisterSection(const CBldrSection * pbldsect)
{
	CString strKey;
	HPROJECT hProj;

	hProj = pbldsect->m_hBld;
	if (hProj == NULL)
	{
		hProj = GetActiveProject();
	}
	strKey.Format("%d:%s", hProj, pbldsect->m_strName);

	return m_mapSections.RemoveKey(strKey);
}

void CBldSysIFace::InitSectionEnum()
{
	m_posSection = m_mapSections.GetStartPosition();
}

CBldrSection * CBldSysIFace::GetNextSection(CString & strSection)
{
	CBldrSection * pbldsect = (CBldrSection *)NULL;
	strSection = "";

	if (m_posSection != (POSITION)NULL)
		m_mapSections.GetNextAssoc(m_posSection, strSection, (void * &)pbldsect);

	return pbldsect;
}

CBldrSection * CBldSysIFace::FindSection(const TCHAR * pchName, HPROJECT hProj /*  = ACTIVE_PROJECT */)
{
	CBldrSection * pbldsect;
	CString strKey;

	if (hProj == ACTIVE_PROJECT)
	{
		hProj = GetActiveProject();
	}
	strKey.Format("%d:%s", hProj, pchName);

	if (!m_mapSections.Lookup(strKey, (void * &)pbldsect))
	{
		hProj = ACTIVE_PROJECT; // failed. Look up "global" build section (created before projects). BUG Orion 20232
		strKey.Format("%d:%s", hProj, pchName);
		if (!m_mapSections.Lookup(strKey, (void * &)pbldsect))
			pbldsect = (CBldrSection *)NULL;

	}

	return pbldsect;
}

// compiler option handler
#include "optncplr.h"

// Map a build file id to an internal property number and component
typedef struct
{
	UINT s_idProp;
	UINT s_idOptHdlr;
	const TCHAR * s_pcDefFilename;
	const TCHAR * s_pcDefExtension;
} S_FileId;

int CBldSysIFace::GetProjectToolset(HPROJECT hProj)
{
	CProject *pProject = CnvHProject(hProj);
	if (pProject != NULL)
		return(pProject->GetProjectToolset());
	else
		return(-1);
}
