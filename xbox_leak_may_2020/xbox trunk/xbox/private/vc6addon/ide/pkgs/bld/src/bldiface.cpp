//
// CBldSysIFace
//
// Build System Interface
//
// [matthewt]
//

#include "stdafx.h"

#include "bldiface.h"	// local header
#include "depgraph.h"	// dep. graph
#include "exttarg.h"    // external target type
#include "awiface.h"    // Appwiz interface
#include "prjdlg.h"     // project/workspace dlg
#include "bldslob.h"	// target nodes in workspace window
#include "bldnode.h"	// build view

#include "optnlink.h"	// to include P_MachineType constant
#include "autoprj.h"

#include <resapi.h>
#include <resguid.h>
#include <prjapi.h>
#include <prjguid.h>
#include <sys\utime.h>
#include "oleref.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CBldSysIFace g_BldSysIFace;	// one 'n' only bld system interface

CProject *g_pActiveProject = NULL;

extern CProjComponentMgr g_prjcompmgr;
extern CFileRegistry g_FileRegistry;
extern BOOL bJavaSupported, bJavaOnce; // defined in vproj.cpp
extern BOOL g_bNoUI;		// defined in project.cpp
extern BOOL g_bConversionPrompted;	// defined in project.cpp

extern CImageWell g_imageWell;	// project item glyphs

CMapStringToOb *CBldSysIFace::pUnknownDepMap = NULL;  //save the unknown dep list temporarily, we need to clean this up properly before we quit a project
CString g_strTarg = _TEXT("");

// Funky appwizard class interface thing. Will go away when the appwizard exports an interface through the shell
class CAppWizardHandler
{
public:
	CAppWizardHandler() {m_hDLL = (HINSTANCE)NULL; m_pAppWizIFace = NULL; }
	~CAppWizardHandler() {if (m_hDLL) FreeLibrary(m_hDLL); m_pAppWizIFace = NULL; }

	BOOL LoadAppWiz();
	__inline CAppWizIFace* GetAppWizIFace() { return m_pAppWizIFace; }

protected:
	HINSTANCE	m_hDLL;
	CAppWizIFace* m_pAppWizIFace;
};

// Loads appwiz, if it hasn't been loaded already.  Most of this code was cut & pasted from
//  the beginning of InvokeAppwizAndCreateTarget.
BOOL CAppWizardHandler::LoadAppWiz()
{
	TCHAR rgchModName[MAX_PATH];
	CPath pathAppwzName;

	GetModuleFileName(GetResourceHandle(), rgchModName, sizeof(rgchModName));
	if (!pathAppwzName.Create(rgchModName))
	{
		// enable file registry file change notification
		g_FileRegDisableFC = FALSE;
		return FALSE;
	}

#ifdef _DEBUG
	pathAppwzName.ChangeFileName(_T("MFCAPWZD.DLL"));
#else
	pathAppwzName.ChangeFileName(_T("MFCAPWZ.DLL"));
#endif

	// check if it's already been loaded, if not we'll attempt to load it
	if (!m_hDLL)
		m_hDLL = SushiLoadLibrary((const TCHAR *)pathAppwzName, TRUE);

	// is this loaded?
	if (!m_hDLL)
		return FALSE;	// failure to load the dll

	// now, set up m_pAppWizIFace.
	PGETAPPWIZIFACE pGetAppWizIFace = (PGETAPPWIZIFACE) GetProcAddress(m_hDLL, GETAPPWIZIFACE);
	ASSERT (pGetAppWizIFace != NULL);

	m_pAppWizIFace = (*pGetAppWizIFace)();
	ASSERT (m_pAppWizIFace != NULL);

	return TRUE;
}

static CAppWizardHandler g_AppWizardHandler;

CAppWizIFace* GetAppWizIFace()
{
	return g_AppWizardHandler.GetAppWizIFace();
}

CProjComponentMgr * CBldSysIFace::GetProjComponentMgr()
{
	return &g_prjcompmgr;
}

CFileRegistry * CBldSysIFace::GetFileRegistry()
{
	return &g_FileRegistry;
}

CBldSysIFace::CBldSysIFace() {
}

CBldSysIFace::~CBldSysIFace() {
    DeleteUnknownDepMap();
}

//
// Builder object conversion
//
CProject * CBldSysIFace::CnvHBuilder(HBUILDER hBld)
{
	CProject * pProject = (CProject *)(hBld != ACTIVE_BUILDER ? hBld : GetActiveBuilder());
	return pProject;
}

CTargetItem * CBldSysIFace::CnvHTarget(HBUILDER hBld, HBLDTARGET hTarg)
{
	if (hTarg==ACTIVE_TARGET)
	{
		if (hBld == ACTIVE_BUILDER)
			hBld = GetActiveBuilder();

		if (hBld != NO_BUILDER)
		{
			return ((CProject *)hBld)->GetActiveTarget();
		}
		return NULL;
	}
	
	if (hTarg==NO_TARGET)
		return NULL;

	if (hBld == ACTIVE_BUILDER)
		hBld = GetBuilder(hTarg); // don't trust caller default

	if (hBld==NO_BUILDER)
		return NULL;

	return CnvHBuilder(hBld)->GetTarget(m_strlstTargets.GetAt((POSITION)hTarg));
}

CTargetItem * CBldSysIFace::CnvHFileSet(HBUILDER hBld, HFILESET hFileSet)
{
	CTargetItem * pTargetItem = (CTargetItem *)(hFileSet != ACTIVE_FILESET ? hFileSet : (HFILESET)CnvHTarget(hBld, GetActiveTarget(hBld)));
	if( pTargetItem == NULL )
		return pTargetItem;

	ASSERT_VALID(pTargetItem);
	return pTargetItem;
}

CFileItem * CBldSysIFace::CnvHBldFile(HFILESET hFileSet, HBLDFILE hFile)
{
	ASSERT_VALID((CFileItem *)hFile);
	return (CFileItem *)hFile;
}

CProjectDependency * CBldSysIFace::CnvHProjDep(HFILESET hFileSet, HPROJDEP hProjDep)
{
	ASSERT_VALID((CProjectDependency *)hProjDep);
	return (CProjectDependency *)hProjDep;
}

HFILESET CBldSysIFace::GetFileSet(HBUILDER hBld, HBLDTARGET hTarg)
{
	return (HFILESET)CnvHTarget(hBld, hTarg);
}

BOOL CBldSysIFace::RenameTarget
(
	const TCHAR * pchTargFrom, 
	const TCHAR * pchTargTo
)
{
	CString strKey = pchTargFrom;
	CString strNewName = pchTargTo;
	strKey.MakeUpper();	// case insensitive lookup
	strNewName.MakeUpper();
 	
	// get our target name
	POSITION pos = m_strlstTargets.Find(strKey);
	if (pos != NULL)
	{
#if 0 // this logic is broken
#ifdef _DEBUG
		// make sure we're not trying to change the builder name here!
		HBUILDER hBld = GetBuilder((HBLDTARGET)pos);
		CTargetItem * pTarget = CnvHBuilder(hBld)->GetTarget(pchTargTo);
		CString strProjName = pTarget->GetTargetName();
		int len = strProjName.GetLength();
		ASSERT(_tcsnicmp(strProjName, pchTargTo, len)==0);
#endif
#endif
		m_strlstTargets.SetAt(pos, strNewName);
		return TRUE;
	}

	return FALSE;
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
	ASSERT(nFirst != -1);
	nFirst += 2; // Skip over hyphen and trailing space

	CString strPlatformAndFlavour = strConfig.Right(strConfig.GetLength() - nFirst);
	CString strRemain;

	do 
	{
		CString strPlatform;
		CPlatform * pPlatform;	
		g_prjcompmgr.InitPlatformEnum();
		while (g_prjcompmgr.NextPlatform(pPlatform))
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
				ASSERT(FALSE);
				return FALSE;
			}
		}
	} while (1);

	return FALSE;
}

//
// Build system enabled?
//
// Dsabled if,
// o no supported platform is installed

BOOL CBldSysIFace::BuildSystemEnabled()
{
	BOOL fSupportedPlatform = FALSE;

	// make sure we have a SUPPORTED platform
	CPlatform * pPlatform;
	g_prjcompmgr.InitPlatformEnum();
	while (g_prjcompmgr.NextPlatform(pPlatform))
	{
		//if (pPlatform->IsPrimaryPlatform())
		if (pPlatform->IsSupported ())
		{
			fSupportedPlatform = TRUE;
			break;
		}
	}

	return fSupportedPlatform;	// ok
}

//
// Builder query API
//

// Get the active builder
HBUILDER CBldSysIFace::GetActiveBuilder() 
{
	return (HBUILDER)g_pActiveProject;
}

HBUILDER CBldSysIFace::GetBuilder(HBLDTARGET hTarg) 
{
	HBUILDER hBld;
	if (m_mapPrjTargets.Lookup(hTarg, (void *&)hBld))
		return (hBld);

	return NO_BUILDER;
}

HBUILDER CBldSysIFace::GetBuilderFromFileSet(HFILESET hFileSet) 
{
	CTargetItem * pTarget = CnvHFileSet(ACTIVE_BUILDER, hFileSet);
	if (pTarget != NULL)
		return (HBUILDER)(pTarget->GetProject());

	// if there is an HFileset the will be a builder
	// ASSERT(0);
	return NO_BUILDER;
}

HBUILDER CBldSysIFace::GetBuilderFromName(const TCHAR *	pchBld ) 
{
	CString strBld = pchBld;
	strBld.MakeUpper();

	CProject *pProject = NULL;
	POSITION pos = CProject::m_lstProjects.GetHeadPosition();  // use our own private pos pointer
	while (pos != NULL)
	{
		pProject = (CProject *)CProject::m_lstProjects.GetNext(pos);
		CString strProjectName;
		// VERIFY(pProject->GetStrProp(P_ProjItemName, strProjectName));

		ConfigurationRecord * pcr = pProject->GetActiveConfig();
		if(pcr)
			pcr->GetProjectName(strProjectName);

		strProjectName.MakeUpper();
		if( strProjectName == strBld ){
			return( (HBUILDER)pProject );
		}
	}

	return NO_BUILDER;
}

CString CBldSysIFace::GetNameFromBuilder(HBUILDER hBld) 
{
	CString strProjectName;
	CProject *pProject = CnvHBuilder(hBld);
	if(pProject != NULL ){
		ConfigurationRecord * pcr = pProject->GetActiveConfig();
		if(pcr)
			pcr->GetProjectName(strProjectName);
	}
	return strProjectName;
}

// Get the type of a builder
BuilderType CBldSysIFace::GetBuilderType
(
	HBUILDER		hBld		// builder to get type of, default='active builder'
)
{
	BuilderType bldType = InternalBuilder;
	CProject *pProject = CnvHBuilder(hBld);
	if(pProject != NULL ){
		if (pProject->IsExeProject())
			bldType = ExeBuilder;
		else
			bldType = InternalBuilder;
	}
	// if there is a build project in the workspace and something else is active 
	// then you can be sure that the build project is internal
    return bldType;
}

// need to dirty the workspace file whenever our changes affect its contents
void SetWorkspaceDocDirty()
{
	LPPROJECTWORKSPACE pProjSysIFace = FindProjWksIFace();
	ASSERT(pProjSysIFace);
	VERIFY(SUCCEEDED(pProjSysIFace->SetWorkspaceDocDirty()));
}

// Get a builders filename
const CPath * CBldSysIFace::GetBuilderFile
(
	HBUILDER		hBld		// builder to get file for, default='active builder'
)
{
	CProject * pProject;
	if (hBld == ACTIVE_BUILDER || hBld == NO_BUILDER)
	{
		// Not a valid assertion (KiP)
		// ASSERT(g_pActiveProject != NULL);
		if( g_pActiveProject == NULL ){
			return NULL;
		}
		pProject = g_pActiveProject;
	}
	else
	{
		pProject = CnvHBuilder(hBld);
	}
	ASSERT_VALID(pProject);
	return pProject->GetFilePath();
}

//
// Builder creation API
//
// Using this API, a builder can be created, targets added, files added
// to those targets and tool settings & properties set for the files.
//
HBUILDER CBldSysIFace::CreateBuilder
(
	const TCHAR *	pchBldPath,	// builder path
	BOOL			fForce,		// force the creation
	BOOL			fOpen		// create and then read from storage (eg. makefile), default=create new
)
{
	//$UNDONE: fForce is no longer used (it used to close the previous workspace).
	// [fabriced] 25jul96
	
// 	// do we have an active builder?
//	HBUILDER hBld = GetActiveBuilder();
//	if (hBld != NO_BUILDER)
//	{
//        // if (!fForce)
//        //    return NO_BUILDER;
//
//	//UNDONE: save old workspace, actually
//        if (fForce && (!SaveBuilder(ACTIVE_BUILDER) || !CloseBuilder(ACTIVE_BUILDER)))
//            return NO_BUILDER;
//	}

	// REVIEW(kperry:9/96)
	// if we alread have a builder with the same name then fail
	// if the workspace is an exe workspace then fail.

	// As of the ne architecture we no longer have the right to set the active project

	// Create the project object
	CProject * pOldProject = g_pActiveProject;
	CProject * pProject;
	pProject = new CProject;
	g_pActiveProject = pProject;

	CPath WksPath;
	CDir WksDir;

	CDir dirOld; dirOld.CreateFromCurrent();
	CPath path;

	if (pchBldPath != (const TCHAR *)NULL && path.Create(pchBldPath))
	{
		// set the current directory to match the pathname 
		if( !_tcsicmp( path.GetExtension(), ".mdp") ) {
			path.ChangeExtension(".mak");
			pchBldPath = (const TCHAR *)path;
		}

		CDir currentDir; currentDir.CreateFromPath(path);
		currentDir.MakeCurrent();
	}

	// create new one or create and open from storage?
	if (fOpen)
	{
		// open an existing document
		if (!pProject->InitFromFile (pchBldPath, FALSE))
 			goto CreationError;

		// Inform the shell that we changed the current directory
		theApp.OnDirChange();

#if 0
		// REVIEW: leave the first project as the current one by default?
		if (pOldProject != NULL)
		{
			ASSERT(!fForce);
			g_pActiveProject = pOldProject;
		}
#endif
	}
	else
 	{
		// No private data to initialize for a new project.
		// Pls. note active target is initialised by calls to CBldSysIFace::AddTarget()
		pProject->m_bPrivateDataInitialized = TRUE;

		if (!pProject->InitNew(NULL))
			goto CreationError;

		if (pchBldPath != NULL)
		{
			CPath pathProjName;

			if	(!pathProjName.Create(pchBldPath) ||
				 !pProject->SetFile(&pathProjName))
				goto CreationError;

			pProject->InformDependants (SN_FILE_NAME);
		}
		else
		{
			CString strProjectName;

			VERIFY(pProject->GetStrProp(P_ProjItemName, strProjectName));
			// UNDONE (colint)
			// SetTitle (strProjectName); 
		}
		
#if 0
		// REVIEW: leave the first project as the current one?
		if (pOldProject != NULL)
		{
			ASSERT(!fForce);
			g_pActiveProject = pOldProject;
		}
#endif

		SetWorkspaceDocDirty();
	}

	// reset the current dir to the workspace directory
	WksPath.Create(*pProject->GetFilePath());
	ProjNameToWksName(WksPath);
	VERIFY(WksDir.CreateFromPath(WksPath));
	WksDir.MakeCurrent();
	// Inform the shell that we may have changed the current directory
	theApp.OnDirChange();

	// return a pointer to this newly created builder
	return (HBUILDER)pProject;

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

	return NO_BUILDER;
}

// Set the dirty state of the builder
BOOL CBldSysIFace::SetDirtyState
(
	HBUILDER	hBld,		// builder to dirty/clean, default='active builder'
	BOOL		fDirty		// default='dirty builder'
)
{
	// get the builder
	CProject * pProject = CnvHBuilder(hBld);
	if (pProject == NULL)
		return FALSE;	// no project, so clean

	// old dirty state?
	BOOL fOldDirty = pProject->IsDirty();

	// dirty or clean?
	if (fDirty)
		pProject->DirtyProject();
	else
		pProject->CleanProject();

	// return old dirty state
	return fOldDirty; 
}

// Get the dirty state of the builder
BOOL CBldSysIFace::GetDirtyState
(
	HBUILDER	hBld		// builder to dirty/clean, default='active builder'
)
{
	// get the builder
	CProject * pProject = CnvHBuilder(hBld);
	if (pProject == NULL)
		return FALSE;	// no project, so clean

	return(pProject->IsDirty());
}

// Save a builder.
BOOL CBldSysIFace::SaveBuilder
(
	HBUILDER		hBld		// builder to save, default='save active builder'
)
{
	BOOL bRetVal = FALSE;

	if (hBld == ACTIVE_BUILDER)
	{
		CString strBuilder = _T("<no name>");
		bRetVal = TRUE; // default
		// special case: close all builders
		InitBuilderEnum();
		while ((bRetVal) && ((hBld = GetNextBuilder(strBuilder, FALSE)) != NO_BUILDER))
		{
			ASSERT(hBld != ACTIVE_BUILDER);
			bRetVal == bRetVal && SaveBuilder(hBld);
		}
		return bRetVal;
	}

	ASSERT(hBld != NO_BUILDER);

	// Get the builder
   	CProject * pProject = CnvHBuilder(hBld);
	ASSERT(pProject);
	if( pProject == NULL )
		return bRetVal;

    // Only need to save internal project file's, i.e. makefiles
    if (GetBuilderType(hBld) == InternalBuilder)
    {
	    // get a builder for this target to be searched in
    	CProject * pProject = CnvHBuilder(hBld);
		const CPath * pPath = pProject->GetFilePath();

		BOOL bIsSccActive = (g_pSccManager->IsSccInstalled() == S_OK);
		BOOL bIsSccOpInProgress = (g_pSccManager->IsSccOpInProgress() == S_OK);
		CStringArray files;
		if (pPath->IsReadOnlyOnDisk() && (g_pSccManager->IsControlled((const TCHAR *)*pPath) == S_OK))
		{
			files.Add((const TCHAR*)*pPath);
		}
		if (g_bAlwaysExportMakefile)
		{
			CPath path = *pPath;
			path.ChangeExtension(_T(".mak"));
			if (path.IsReadOnlyOnDisk() && (g_pSccManager->IsControlled((const TCHAR *)path) == S_OK))
			{
				files.Add((const TCHAR*)path);
			}
			path.ChangeExtension(_T(".dep"));
			if ((g_bAlwaysExportDeps) && (path.IsReadOnlyOnDisk()) && (g_pSccManager->IsControlled((const TCHAR *)path) == S_OK))
			{
				files.Add((const TCHAR*)path);
			}
		}
		if ((!pProject->m_bProjExtConverted) && ((files.GetSize() > 0) || (pPath->IsReadOnlyOnDisk())) && (pProject->IsDirty()) &&
			((!bIsSccActive) || (files.GetSize() == 0) || (g_pSccManager->CheckOutReadOnly(files, TRUE, FALSE) == S_FALSE)))
		{
            if (bIsSccOpInProgress)
			{
				bRetVal = TRUE; // we're probably trying to check this out
			}
			else if (pPath->IsReadOnlyOnDisk())
			{
				CString strMsg;
				MsgText(strMsg, IDS_SAVE_READ_ONLY, (const TCHAR *)*pPath);
				if ((!g_bConversionPrompted) && g_pAutomationState->DisplayUI() && !g_bNoUI) //ShellOM:State
				{
						bRetVal = (MsgBox(Error, strMsg, MB_OKCANCEL)==IDOK);
				}
				else if (theApp.m_bInvokedCommandLine)
				{
					
					theApp.WriteLog(strMsg + _T("\n"), TRUE);
					bRetVal = TRUE; // keep going
				}
				else
				{
					bRetVal = TRUE; // REVIEW: oh well
				}
			}
			else
			{
				bRetVal = TRUE;
			}
		}
#if 0
		else if (pProject->m_bProjExtConverted)
		{
			// Do SaveAs to allow rename if wrapped makefile
			g_bWriteProject = TRUE;
			bRetVal = pProject->DoSaveAs();
		}
#endif
		else
		{
			// if it's still read-only, assume user said no or in scc op
			if (!pPath->IsReadOnlyOnDisk())
			{
				g_bWriteProject = TRUE;
				CPath OptPath(*pPath);
				bRetVal = pProject->DoSave();

				if (bRetVal) pProject->m_bProjMarkedForSave = FALSE;
				// get .mdp path and force time stamp to be newer than
				// .mak. This will allow us to detect an updated .mak when
				// we open the project, and ignore the .mdp contents.
				ProjNameToOptName(OptPath);// UNDONE: bogus
				_utime(OptPath, NULL);
			}
			else
				bRetVal = TRUE;
		}
    }
    else
        bRetVal = TRUE;

	return bRetVal;
}
  
HBUILDER CBldSysIFace::DeactivateBuilder(HBUILDER hBld)
{
	ASSERT(hBld);
	CProject * pProject = (CProject *)hBld;

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
					HBLDTARGET hTarget = GetActiveTarget((HBUILDER)pActiveProject);
					if (hTarget==NO_TARGET)
					{
						CTargetItem * pTargetItem;
						CString strTargetName;
						pActiveProject->InitTargetEnum();
						VERIFY(pActiveProject->NextTargetEnum(strTargetName, pTargetItem));
						hTarget = GetTarget(strTargetName, (HBUILDER)pActiveProject);
						ASSERT (hTarget);
					}
					SetActiveTarget(hTarget, (HBUILDER)pActiveProject);
					break;
				}
				else
				{
					g_pActiveProject = pActiveProject; // at least we have one
				}
			}
		}
	}
	return (HBUILDER)g_pActiveProject;
}

// Close a builder.
BOOL CBldSysIFace::CloseBuilder
(
	HBUILDER	hBld,			// builder to close, if == ACTIVE_BUILDER, then close active builder
	DeletionCause dcCause
)
{
	if (hBld == ACTIVE_BUILDER)
	{
		// actually closing the workspace
#ifdef PERF_MEASUREMENTS
		DWORD dwCloseStartTime = 0;

		if (theApp.m_bPerfQA_Workspace)
		{
			theApp.dwWorkspaceTime = 0;
			dwCloseStartTime = ::GetTickCount( );
		}
#endif // PERF_MEASUREMENTS

		CString strBuilder = _T("<no name>");
		BOOL retval = TRUE; // default
		// special case: close all builders
		InitBuilderEnum();
		while ((retval) && ((hBld = GetNextBuilder(strBuilder, FALSE)) != NO_BUILDER))
		{
			ASSERT(hBld != ACTIVE_BUILDER);
			g_bInProjClose = TRUE;
			retval = retval && CloseBuilder(hBld,dcCause);
			InitBuilderEnum(); // hack: must re-init, since first project was just removed!
		}
		g_bInProjClose = FALSE;

#ifdef PERF_MEASUREMENTS
		if (theApp.m_bPerfQA_Workspace)
		{
			DWORD dwCloseEndTime = ::GetTickCount( );
			theApp.dwWorkspaceTime = dwCloseEndTime - dwCloseStartTime;
		}
#endif // PERF_MEASUREMENTS

		return retval;
	}

	ASSERT(hBld != ACTIVE_BUILDER);
	// Get the builder
   	CProject * pProject = CnvHBuilder(hBld);
	if (pProject == (CProject *)NULL)
		return TRUE;	// nothing to do

	hBld = (HBUILDER)pProject; // make sure not NULL
	if (!pProject->m_bProjectComplete)
	{
		ASSERT(0);
		return FALSE;  // avoid recursion
	}

	if (!theApp.NotifyPackages(PN_QUERY_CLOSE_PROJECT))
		return FALSE;
 
	
	// set active project to something else
	if (pProject == g_pActiveProject)
	{
		DeactivateBuilder((HBUILDER)pProject);
	}

	// Must remove nodes before we say that the project is closed
    // otherwise the prj package will attempt to delete our nodes
    // which we can't handle.
	GetBuildNode()->RemoveNodes(hBld);

#if 1 // REVIEW: may not be needed once ClassView handles PN_CLOSE_PROJECT properly
    if (pProject->GetActiveConfig() != NULL)
    {
	CTargetItem * pTarget = pProject->GetActiveTarget();
	if (pTarget != NULL)
		theApp.NotifyPackages(PN_DEL_FILESET, (void *)(HFILESET)pTarget);
    }
#endif

	CString strName;
	pProject->GetName(strName);
	
	

	// Do NOT move this line.  We must notify other packages of the close 
	// project before we do everything below (like deleting the file registry)

	IBSProject * pIBSProject = pProject->GetInterface();
	IPkgProject *pPkgProj;
	pIBSProject->QueryInterface(IID_IPkgProject, (void **)&pPkgProj);
	theApp.NotifyPackages(PN_CLOSE_PROJECT, (void *)pPkgProj);
	pPkgProj->Release();
	pIBSProject->Release();

	delete pProject;

	// if we are deleting this project from the workspace then delete all the
	// project deps that refer to it.
	if( dcCause == ProjectDelete ){
		::RemoveAllReferences(strName);
	}
	
	// only do this when we're closing the very last project
	if (g_pActiveProject==NULL)
	{
		// olympus 16 (briancr)
		// Clear the global file registry after closing a workspace.
		// Theoretically, we shouldn't have to do this since an FRH gets deleted
		// when its reference count reaches zero.

#ifndef REFCOUNT_WORK
		g_FileRegistry.DeleteAll();
#else
#ifdef _DEBUG
		if (!g_FileRegistry.IsEmpty() )
			AfxDump(&g_FileRegistry);
#endif
		// This assertion is causing problems??
		//	ASSERT(g_FileRegistry.IsEmpty());
#endif

		m_lstDepSet.RemoveAll();
		m_lstFileSet.RemoveAll();
		m_lstFile.RemoveAll();

		// olympus 2424 (briancr)
		// the fix for olympus 16 overlooked clearing pUnknownDepMap, so do it here
		DeleteUnknownDepMap();

		// end olympus 16 fix
	}

	return TRUE;	// success
}

BOOL CBldSysIFace::OnOpenWorkspace(const TCHAR *pszWorkspaceName, BOOL bTemporary, BOOL bInsertingIntoWks)
{
    ASSERT(GetFileError()==CFileException::none); // should already be reset by this point
    // reset global file error
    SetFileError(CFileException::none);
	g_bBadConversion = FALSE;

	// remember the directory we had before builder creation
 	CDir oldDir; oldDir.CreateFromCurrent();

#ifdef PERF_MEASUREMENTS
	DWORD dwOpenStartTime = 0;

	if (theApp.m_bPerfQA_Workspace)
	{
		theApp.dwWorkspaceTime = 0;
		dwOpenStartTime = ::GetTickCount( );
	}
#endif // PERF_MEASUREMENTS

	// disable file registry file change notification during builder creation
	BOOL bOldDisable = g_FileRegDisableFC;
	g_FileRegDisableFC = TRUE;

	CPath MakeFilePath;
	MakeFilePath.Create(pszWorkspaceName);
	if (_tcsicmp(MakeFilePath.GetExtension(), _T(WORKSPACE_EXT))==0)
	{
		// don't even try to read a .wks file here
		ASSERT(0); // should be caught by workspace loader
		return FALSE;
	}

	// open up a builder? (non-appwiz created -> this is part of the workspace)
	HBUILDER hBld;
	hBld = g_BldSysIFace.CreateBuilder(MakeFilePath, !bInsertingIntoWks /* force close old */, TRUE /* open */);

	// do we have a valid builder?
	if (hBld == NO_BUILDER)
	{
		// enable file registry file change notification
		g_FileRegDisableFC = bOldDisable;
		return FALSE;
	}

	CProject *pProject = g_BldSysIFace.CnvHBuilder(hBld);
	IBSProject *pBSProj = pProject->GetInterface();
	COleRef<IPkgProject> pPkgProject;
	pBSProj->QueryInterface(IID_IPkgProject, (void **)&pPkgProject);
	pBSProj->Release();
	if (g_pProjWksIFace && !bInsertingIntoWks)
		g_pProjWksIFace->AddProject(pPkgProject, FALSE);

	pProject = g_pActiveProject;

	// enable file registry file change notification
	g_FileRegDisableFC = bOldDisable;

	// Scan the project dependencies if needed.  We do this here instead
	// of in the project, so the window can be displayed first:
	BOOL bWasReadOnly = pProject->WasReadOnly();

	if (pProject->m_bProjMarkedForScan)
	{
		// If this project was read-only at load time, reset that state
		// now; otherwise, the scan will "dirty" the project and cause
		// a warning message:
		if (bWasReadOnly)
			pProject->ResetReadOnly();

		// Do the scan:
		//BOOL bProjectDirty = pProjDoc->m_pNotifySlob->IsProjectDirty(pProject);
		pProject->DoTopLevelScan();

		if (pProject->m_bProjConverted && (!((pProject->m_bConvertedDS4x) || (pProject->m_bConvertedVC20) || (pProject->m_bConvertedDS5x))))
			pProject->ConvertDeps();

		//if (!bProjectDirty) 
			// This scan should in fact not dirty the project (only if the project is not marked
			// dirty :
		//	pProject->InformDependants(SN_PROJ_PUBLICS_CLEAN);

		// If the project was read-only at load time, recheck that state:
		if (bWasReadOnly)
			pProject->RecheckReadOnly();
	}

	if (!pProject->IsExeProject())
	{
		if (pProject->m_bProjMarkedForSave)
		{
	   		// Opening an newly-created project (Wizard callback)
			if (pProject->m_bProjConverted)
			{
				// Make sure we save the converted project, <Cancel> will
				// abort the open document file
				g_bWriteProject = TRUE;
				pProject->SetOkToDirtyProject();
				pProject->DirtyProject();

				if (pProject->m_bProjExtConverted)
				{
					// Do SaveAs to allow rename if wrapped makefile
					if (!pProject->DoSaveAs())
						return FALSE;
				}
				else if ((!pProject->m_bConvertedDS5x) || (!pProject->GetFilePath()->IsReadOnlyOnDisk())) // scc not init yet!
				{
					pProject->DoSave();
				}

				// Set the name of the new workspace
				LPPROJECTWORKSPACE pProjSysIFace = FindProjWksIFace();
				if (!bInsertingIntoWks)
				{
					CPath Path = *(pProject->GetFilePath());
					Path.ChangeExtension(WORKSPACE_EXT);
	                VERIFY(SUCCEEDED(pProjSysIFace->SetWorkspaceDoc((const TCHAR *)Path)));
				}
				VERIFY(SUCCEEDED(pProjSysIFace->SetWorkspaceDocDirty()));

				if (pProject->m_bConvertedDS4x)
				{
					// Also need to save any converted subprojects
					CProject::InitProjectEnum();
					CString strProject;
					CProject * pNewProject;
					while ((pNewProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL)
					{
						if ((pNewProject != pProject) && (pNewProject->m_bProjMarkedForSave) && (pNewProject->m_bConvertedDS4x))
						{
							pNewProject->SetOkToDirtyProject();
							pNewProject->DirtyProject();
							g_bWriteProject = TRUE;
							pNewProject->DoSave();
						}
					}
                }
			}
			else if (!bWasReadOnly)
			{
				// Re-save project in verbose format
				// FUTURE (karlsi): should do this quietly, if possible
				g_bWriteProject = TRUE;
				pProject->DoSave();
			}
		}
	}

	if (pProject->m_bConvertedDS4x)
	{
		// Also need to save any converted subprojects
		POSITION pos = CProject::m_lstProjects.GetHeadPosition(); // most use our own pointer
		CProject * pNewProject;
		ASSERT(pos != NULL);
		while (pos != NULL)
		{
			pNewProject = (CProject *)CProject::m_lstProjects.GetNext(pos);
			ASSERT(pNewProject);
			if ((pNewProject != NULL) && (pNewProject->m_bProjMarkedForSave))
			{
				pNewProject->m_bProjMarkedForSave = FALSE;

				// Inform packages that we've opened an existing project
				COleRef<IBSProject> pIBSProject = pNewProject->GetInterface();
				COleRef<IPkgProject> pPkgProj;
				pIBSProject->QueryInterface(IID_IPkgProject, (void **)&pPkgProj);
            	if ((g_pProjWksIFace) && (pPkgProj != pPkgProject))
            		g_pProjWksIFace->AddProject(pPkgProj, FALSE);

				if (g_pProjWksIFace && (!bInsertingIntoWks || (pNewProject != g_pActiveProject)))
					theApp.NotifyPackages(PN_OPEN_PROJECT, (void *)pPkgProj);
                pPkgProj.SRelease();

				pNewProject->m_bConvertedDS4x = FALSE;
			}
		}
	}
	else
	{
		if ((!pProject->m_bConvertedDS5x) || (!pProject->GetFilePath()->IsReadOnlyOnDisk())) // scc not init yet!
		{
			pProject->m_bProjMarkedForSave = FALSE;
		}
		else
		{
			ASSERT(pProject->IsDirty());
		}
		
		pProject->m_bConvertedDS5x = FALSE;

			// Inform packages that we've opened an existing project
		COleRef<IBSProject> pIBSProject = pProject->GetInterface();
		COleRef<IPkgProject> pPkgProj;
		pIBSProject->QueryInterface(IID_IPkgProject, (void **)&pPkgProj);
		if (g_pProjWksIFace && !bInsertingIntoWks)
			theApp.NotifyPackages(PN_OPEN_PROJECT, (void *)pPkgProj);
	}

#ifdef _DEBUG
#ifdef USE_LOCAL_BUILD_HEAP
	// heap debug info.
	afxDump << "\nBuild System Node Heap\n";
	CGrNode::g_heapNodes.Dump(afxDump);

	afxDump << "\nBuild System File Heap\n";
	CFileRegEntry::g_heapEntries.Dump(afxDump);
#endif
#endif // _DEBUG

#ifdef PERF_MEASUREMENTS
	if (theApp.m_bPerfQA_Workspace)
	{
		DWORD dwOpenEndTime = ::GetTickCount( );
		theApp.dwWorkspaceTime = dwOpenEndTime - dwOpenStartTime;
	}
#endif // PERF_MEASUREMENTS

	return TRUE;
}

BOOL CBldSysIFace::OpenWorkspace(const TCHAR *pchWorkspace)
{
    return(FALSE);
}

BOOL CBldSysIFace::OnDefaultWorkspace(const TCHAR *pszFileToAdd)
{
	// Create a default project
	CPath pathProjName, pathFileToAdd;

	pathProjName.Create(pszFileToAdd);
	pathProjName.ChangeExtension(_T(BUILDER_EXT)); 

	pathFileToAdd.Create(pszFileToAdd);

	// See if the .PJX file already exists.  If so, warn the user.
	if (pathProjName.ExistsOnDisk())
	{
		CString	strT;

		if (MsgBox(	Question,
					MsgText(strT,
							IDS_WARN_OVERWRITE_EXISTING_PROJ,
							(const TCHAR *)pathProjName), MB_YESNO)
					== IDNO)
			return FALSE;

		// Delete the .PJX file.
		pathProjName.DeleteFromDisk();
 	}

	// create a new builder, force close the old one if it exists
	HBUILDER hBld = g_BldSysIFace.CreateBuilder((const TCHAR *)pathProjName.GetFullPath(), FALSE /* force close old */);
	if (hBld == NO_BUILDER)
		return FALSE;	// failure

	LPSOURCEQUERY pSrcQuery;
	LPSOURCEEDIT pDoc;
	CString Language;
	BOOL fJavaFile = FALSE;

    if (SUCCEEDED (theApp.FindInterface (IID_ISourceQuery, (LPVOID *)&pSrcQuery)))
	{
		if (SUCCEEDED (pSrcQuery->CreateSourceEditForDoc ((PSTR) pszFileToAdd, &pDoc, FALSE)))
		{
			pDoc->GetLanguage (Language);
			if (Language == "Java")
				fJavaFile = TRUE;
			pDoc->Release ();
		}
		pSrcQuery->Release ();
	}
	// Find the 'console' project type:
	//
	CProjType *	pProjType;

	g_prjcompmgr.InitProjTypeEnum();

	if (fJavaFile)
	{
		while (g_prjcompmgr.NextProjType(pProjType))
			if (pProjType->GetUniqueTypeId() == CProjType::javaapplet)
				break;
		
		VERIFY(pProjType->GetUniqueTypeId()==CProjType::javaapplet);
	}
	else
	{
		while (g_prjcompmgr.NextProjType(pProjType))
			if ((pProjType->GetPlatform()->IsPrimaryPlatform())
				&& (pProjType->GetUniqueTypeId()==CProjType::consoleapp))
				break;

		VERIFY(pProjType->GetUniqueTypeId()==CProjType::consoleapp);
	}
	// create the project as a default 'simple' one
	// o not using MFC
	// o not using output directories
	HBLDTARGET hDebugTarg, hReleaseTarg;
	(void) g_BldSysIFace.AddDefaultTargets
			(
				(const TCHAR *)*pProjType->GetPlatformName(),	// 'official' name of this target's platform
				(const TCHAR *)*pProjType->GetTypeName(),		// 'official' name of this target type
				hDebugTarg, hReleaseTarg,						// our newly created targets (returned)
				FALSE,											// we're using 'official' names
				TRUE,											// not using output directories
				FALSE											// not using MFC
			);

	// Add the specified file to the project:
	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);
	pProject->AddFile(&pathFileToAdd);

	// Save the project.
	SaveBuilder(hBld);

	COleRef<IBSProject> pIBSProject = pProject->GetInterface();
	COleRef<IPkgProject> pPkgProj;
	pIBSProject->QueryInterface(IID_IPkgProject, (void **)&pPkgProj);
	if(g_pProjWksIFace)
		g_pProjWksIFace->AddProject(pPkgProj, FALSE);
	theApp.NotifyPackages (PN_NEW_PROJECT, (void *)pPkgProj);
	if(g_pProjWksIFace)
		g_pProjWksIFace->SetActiveProject(pPkgProj, TRUE);
	// Enable config change notifications (because this isn't done in this case otherwise,
	// since we don't try to load a vcp file)
	pProject->m_bNotifyOnChangeConfig = TRUE;

    return TRUE;
}

//
// Fileset API
//

HFILESET CBldSysIFace::GetFileSetFromTargetName
(
	const TCHAR *	pchTarget,		// name of this fileset to get
	HBUILDER		hBld			// handle to the builder containing the target, default='active builder'
)
{
	if (hBld == ACTIVE_BUILDER || hBld == NO_BUILDER )
	{
		// search all builders
		CString strBuilder;
		InitBuilderEnum();
		HFILESET retval = NO_FILESET;
		while ((hBld = GetNextBuilder(strBuilder, TRUE)) != NO_BUILDER)
		{
			ASSERT(hBld != ACTIVE_BUILDER);
			retval = GetFileSetFromTargetName(pchTarget, hBld);
			if (retval != NO_FILESET)
				break;	// got one
		}
		return retval;
	}

	// null-terminated empty string?
	if (*pchTarget == _T('\0'))
		return NO_FILESET;

	CString strFileSetName;
	InitFileSetEnum(hBld);
	HFILESET hFileSet = GetNextFileSet();
	while (hFileSet != NO_FILESET)
	{
		VERIFY(GetFileSetName(hFileSet, strFileSetName, hBld));
		CString strProject = pchTarget;
  		int iDash = strProject.Find(_T(" - "));
  		if (iDash > 0)
  	      strProject = strProject.Left(iDash);

		if (!strFileSetName.CompareNoCase(strProject))
			return hFileSet;

		hFileSet = GetNextFileSet();
	}

	return NO_FILESET;
}

BOOL CBldSysIFace::GetFileSetName
(
	HFILESET		hFileSet,					// Fileset to get name of
	CString &		str,						// Fileset name
	HBUILDER		hBld                		// handle to the builder containing the fileset, default='active builder'
)
{
	// NYI for anything other than the active builder.
	// ASSERT(hBld == ACTIVE_BUILDER);

	// Get the target item associated with the fileset
	CTargetItem * pTarget = CnvHFileSet(hBld, hFileSet);
	if(pTarget == NULL){
		return FALSE;
	}

	// Get the target items config array
	const CPtrArray* pConfigArray = pTarget->GetConfigArray();
	int nSize = pConfigArray->GetSize();

	// Look for the first valid config for this target item
	ConfigurationRecord * pcr = NULL;
	for (int i = 0; i < nSize; i++)
	{
		pcr = (ConfigurationRecord*)pConfigArray->GetAt(i);
		if (pcr->IsValid())
 			break;
	}

	if (pcr == NULL)
		return FALSE;

	// Get the name of the valid config that we found
	CString strConfig = pcr->GetConfigurationName();
	CString strTemp = strConfig;

	// Form the whole configuration name
	int cbLen = strConfig.GetLength();
	int nFirst;
	do
	{
		nFirst = strTemp.Find(_T('-'));
		ASSERT(nFirst != -1);
		strTemp = strConfig.Left(nFirst);
	} while ((nFirst > 0) && (strConfig[nFirst-1]!=_T(' ')));

	ASSERT(strConfig[nFirst+1]==_T(' '));
	nFirst += 2; // Skip over hyphen and trailing space

	CString strPlatformAndFlavour = strConfig.Right(cbLen - nFirst);
	CString strRemain;

	do 
	{
		CString strPlatform;
		CPlatform * pPlatform;	
		g_prjcompmgr.InitPlatformEnum();
		while (g_prjcompmgr.NextPlatform(pPlatform))
		{
			strPlatform = *(pPlatform->GetName());
			if (strPlatform.Compare(strPlatformAndFlavour.Left(strPlatform.GetLength())) == 0)
			{
				// Found the platform.
				str = strConfig.Left(cbLen - strPlatformAndFlavour.GetLength() - 3);
				return TRUE;
 			}
		}
		strRemain = strPlatformAndFlavour;
		strTemp = strRemain;
		do
		{
			nFirst = strTemp.Find(_T('-'));
			if (nFirst != -1)
				strTemp = strRemain.Left(nFirst);
		} while ((nFirst > 0) && (strRemain[nFirst-1]!=_T(' ')));

		if (nFirst != -1 && (strRemain.GetLength()>4))
		{
			ASSERT(strRemain[nFirst+1]==_T(' '));
			nFirst += 2;
			strPlatformAndFlavour = strRemain.Right(strRemain.GetLength() - nFirst);
		}
		else
		{
			// no more '-', we didn't get a standard config name, but...
			// olympus 1991 [patbr] look again using short name for Intel
			CString strWinPlatform("Win32");
			if (strWinPlatform.Compare(strPlatformAndFlavour.Left(strWinPlatform.GetLength())) == 0)
			{
				// Found the platform.
				str = strConfig.Left(cbLen - strPlatformAndFlavour.GetLength() - 3);
				return TRUE;
			}
			else
			{
				ASSERT(FALSE);
				return FALSE;
			}
		}
	} while (1);

	return FALSE;
}

BOOL CBldSysIFace::GetTargetNameFromFileSet
(
	HFILESET		hFileSet,					// Fileset to get name of
	CString &		str,						// Fileset name
	HBUILDER		hBld,						// handle to the builder to match, default='active builder'
	BOOL			bInvalid					// If there is no target that matches the current default project then
												// we just get a valid target, unless this is TRUE, in which case we
												// return "fileset name - No matching configuration", default='FALSE'
)
{
	str.Empty();
 	// Get a project pointer
	CProject * pMatchProject;
	if ((hBld == ACTIVE_BUILDER) || (hBld == NO_BUILDER))
		pMatchProject = (CProject *)GetActiveBuilder();
	else
		pMatchProject = (CProject *)hBld;

	if(pMatchProject == NULL)
	{
		return FALSE;
	}
	
	// Get the name of the active config to match
	ConfigurationRecord * pcr = pMatchProject->GetActiveConfig();
	if (pcr==NULL) // can happen during project creation
	{
		// ASSERT(0);
		return FALSE;
	}

	HBUILDER hTargetBld = GetBuilderFromFileSet(hFileSet); // don't trust default
	CString strActiveConfig = pcr->GetConfigurationName();
	CString strMatchingDescription;
	pcr->GetConfigurationDescription(strMatchingDescription);

	CString strFileSet, strFlavor;

	// Get the fileset name
	if (!GetFileSetName(hFileSet, strFileSet, hTargetBld))
		return FALSE;

	// Get the build flavour name
	GetFlavourFromConfigName(strActiveConfig, strFlavor);
	// filesetname + platform + " "
	strActiveConfig = strActiveConfig.Left(strActiveConfig.GetLength() - strFlavor.GetLength());
	// get platform name
	int nSep = strActiveConfig.ReverseFind(_T('-'));
	strActiveConfig = strActiveConfig.Right(strActiveConfig.GetLength() - nSep + 1);

	// Form the fileset name
	str = strFileSet + strActiveConfig + strFlavor;
	// Lookup the fileset name in the target list
	CString strTargetName = str;
	strTargetName.MakeUpper();	// case insensitive lookup
 	HBLDTARGET hTarg = (HBLDTARGET)m_strlstTargets.Find(strTargetName);
	if (hTarg == NO_TARGET)
	{
		if (!bJavaOnce)
		{
			bJavaOnce=TRUE;
			bJavaSupported = IsPackageLoaded(PACKAGE_LANGJVA);
		}
		if (bJavaSupported)
		{
			// This is a hack of all hacks...
			// If Java is the active config, then anything with the same flavor matches.
			// If not, then we have to check to see if the fileset supports
			// Java.  If it does, then it will match anything with the same flavor.

			CString strSubPlat, 
				    strSubFlavor,
					strWin32Target, strOtherTarget,
			        strJavaPlat = " - Java Virtual Machine " + strFlavor, 
					strWin32Plat = " - Win32 " + strFlavor;
			CStringList strlstTargs;

			GetFileSetTargets (hFileSet, strlstTargs);

			// to get all targets for this fileset 
			POSITION pos = strlstTargs.GetHeadPosition();
			while (pos != NULL)
			{
				// First, is the default platform java and is there a target with
				// the same flavor?

				strTargetName = strlstTargs.GetNext(pos);
				GetFlavourFromConfigName (strTargetName, strSubFlavor);

				int nSep1 = strTargetName.ReverseFind(_T('-'));
				strSubPlat = strTargetName.Right(strTargetName.GetLength() - nSep1 + 1);


				if (strSubPlat == strJavaPlat)
				{
					str = strTargetName;
					return (TRUE); 
				}

				if (strSubPlat == strWin32Plat)
				{
					strWin32Target = strTargetName;
					break;
				}
				if (strOtherTarget.IsEmpty () && strSubFlavor == strFlavor)
					strOtherTarget = strTargetName;					
			}
			if (strActiveConfig == " - Java Virtual Machine ")
			{
				if (!strWin32Target.IsEmpty ())
				{
					str = strWin32Target;
					return (TRUE); 
				}
				else if (!strOtherTarget.IsEmpty ())
				{
					str = strOtherTarget;
					return (TRUE); 
				}
			}
		}

		// No configuration of the fileset that matches the current default project's
		// configuration, so we just pick a configuration that does exist for the fileset
		if (bInvalid)
		{
			CString strNoMatch;
			strNoMatch.LoadString(IDS_NO_MATCHING_CONFIG);

 			str = strFileSet + _T(" - ") + strNoMatch;
			return FALSE;
		}
		else
		{
			CStringList strlstTargs;
			GetFileSetTargets (hFileSet, strlstTargs);

			// to get all targets for this fileset 
			int nLongest = 0;
			POSITION pos = strlstTargs.GetHeadPosition();
			while (pos != NULL)
			{
				// First, is the default platform java and is there a target with
				// the same flavor?

				CString strConfigName = strlstTargs.GetNext(pos);
				// get the platform
				CProject *pProject = (CProject *)hTargetBld;
				ConfigurationRecord * pcr = pProject->ConfigRecordFromConfigName(strConfigName);
				//     how many characters match 
				//     if this is longer than the longest so far,
				//			this is our new longest

				// GetFlavourFromConfigName (strTargetName, strSubFlavor);
				CString strDescription;
				pcr->GetConfigurationDescription ( strDescription );
				int nMax;
				if(strDescription.GetLength() > strMatchingDescription.GetLength() ){
					nMax = strMatchingDescription.GetLength();
				} else {
					nMax = strDescription.GetLength();
				}

				int i=0;
				while( i < nMax && strDescription[i] == strMatchingDescription[i] ){
					i++;
				}
				if( i > nLongest){
					nLongest = i;
					str = pcr->GetConfigurationName();
				}

			}
			if( nLongest )
				return FALSE;

			InitTargetEnum(hTargetBld);
			hTarg = GetNextTarget(str, hTargetBld);
			while (hTarg != NO_TARGET)
			{
				if (GetFileSet(hTargetBld, hTarg) == hFileSet)
				{
					GetTargetName(hTarg, str, hTargetBld);
					return FALSE;
				}

				hTarg = GetNextTarget(str, hTargetBld);
			}
			return FALSE;
		}
	}
 
	return TRUE;
}

BOOL CBldSysIFace::GetTargetFromFileSet
(
	HFILESET		hFileSet,					// Fileset to get current target for
	HBLDTARGET &	hTarg,						// Target for the Fileset
	HBUILDER		hBld,                		// handle to the builder containing the fileset to match, default='active builder'
	BOOL			bInvalid					// If this is TRUE and we don't get a match then we return FALSE, if this
												// is FALSE then we always return some valid target. default=FALSE
)
{
	// Get the name of the fileset 
	CString strTargetName;
	if (!GetTargetNameFromFileSet(hFileSet, strTargetName, hBld, bInvalid) && bInvalid)
		return FALSE;

	// Lookup the fileset name in the target list
	strTargetName.MakeUpper();	// case insensitive lookup
 	hTarg = (HBLDTARGET)m_strlstTargets.Find(strTargetName);
	if (hTarg == NO_TARGET)
		return FALSE;

	// We found it
	return TRUE;
}

// Get the active target if there is on, or NO_TARGET if there isn't.
HBLDTARGET CBldSysIFace::GetActiveTarget
(
	HBUILDER		hBld		// handle to the builder containing the targets, default='active builder'
)
{
	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);

	if (pProject == NULL)
		return NO_TARGET;

	if (pProject->GetActiveConfig() == NULL)
		return NO_TARGET;

	// get our active target name
	CString strTarg = pProject->GetActiveConfig()->GetConfigurationName();

	return GetTarget((const TCHAR *)strTarg, (HBUILDER)pProject);
}

// Set the active target.
BOOL CBldSysIFace::SetActiveTarget
(
	HBLDTARGET		hTarg,		// target to make the active one
	HBUILDER		hBld		// handle to the builder containing the targets, default='active builder'
)
{
	if (g_Spawner.SpawnActive())
		return FALSE;

	ASSERT(hTarg != NO_TARGET);

	// get our target name
	CString strTarg = m_strlstTargets.GetAt((POSITION)hTarg);

	// get a builder for this target to be searched in
	if (hBld == ACTIVE_BUILDER)
		hBld = GetBuilder(hTarg); // don't trust caller default

	if( hBld == NO_BUILDER )
		return FALSE;

	CProject * pProject = CnvHBuilder(hBld);
	ASSERT(pProject != NULL);

	BOOL bRet = TRUE;
	if (pProject != g_pActiveProject)
	{
		COleRef<IPkgProject> pPkgProject;
		LPBSPROJECT pIProj = pProject->GetInterface();
		VERIFY(SUCCEEDED(pIProj->QueryInterface(IID_IPkgProject, (void **)&pPkgProject)));
		pIProj->Release();

		if(g_pProjWksIFace)
			bRet = SUCCEEDED(g_pProjWksIFace->SetActiveProject(pPkgProject, TRUE));

		//
		// review(tomse): Assertion is failing when build a non-active project and 
		// setting active project to the project that is already active.
		// This is probably because g_pActiveProject is the project being built, not
		// the active IPkgProject.
		// The SetActiveProject succeeds because it always succeeds when changing to the
		// already active project.
		//
		ASSERT(!bRet || pProject == g_pActiveProject);

		//
		// QUICK FIX for #20767.
		// Leave assert but return FALSE instead doing further processing for
		// active project that really didn't change.
		//
		if (bRet && pProject != g_pActiveProject)
			return FALSE;
	}

	// set this target as our active one
	if(bRet)
		bRet = g_pActiveProject->SetStrProp(P_ProjActiveConfiguration, strTarg);
	if(bRet)
		pProject->SetActiveConfig(strTarg);

	return bRet;	// ok?
}

BOOL CBldSysIFace::SetDefaultTarget
(
	HBLDTARGET		hTarg,		// target to make the default one
	HBUILDER		hBld		// handle to the builder containing the targets, default='active builder'
)
{
	ASSERT(hTarg != NO_TARGET);

	if (hBld == ACTIVE_BUILDER)
		hBld = GetBuilder(hTarg); // don't trust caller default

	if( hBld == NO_BUILDER )
		return FALSE;

	// get our target name
	CString strTarg;
    if (!GetTargetName(hTarg, strTarg, hBld))
        return FALSE;

	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);

	// set this target as our active one
    pProject->m_strProjDefaultConfiguration = strTarg;

	return TRUE;	// ok?
}

// Get a target with name 'pchTarg' from the builder, 'hBld'.
HBLDTARGET CBldSysIFace::GetTarget
(
	const TCHAR *	pchTarg,	// name of this target to get
	HBUILDER		hBld		// handle to the builder containing the target, default='active builder'
)
{
	// null-terminated empty string?
	if (*pchTarg == _T('\0'))
		return NO_TARGET;

	CString strKey = pchTarg;
	strKey.MakeUpper();	// case insensitive lookup
	HBLDTARGET hTarg = (HBLDTARGET)m_strlstTargets.Find(strKey);
	
	if (hTarg == NO_TARGET)
	{
		// It is okay for someone to ask us for a TARGET when there is no BUILDER. We should
		// just answer no.

		if (hBld != NO_BUILDER)
		{
			hTarg = (HBLDTARGET)m_strlstTargets.AddTail(strKey);
			m_mapPrjTargets.SetAt(hTarg, hBld);
		}
	}

	return hTarg;
}

// Set the active target, Can be NO_BUILDER
BOOL CBldSysIFace::SetActiveBuilder
(
	HBUILDER		hBld		// handle to the builder containing the targets, default='active builder'
)
{
	CProject * pProject;
	if(hBld != NO_BUILDER)
		pProject = CnvHBuilder(hBld);
	else
		pProject = NULL;

	if (pProject == NULL)
	{
		if (NULL != g_pActiveProject)
		{
			g_pActiveProject = pProject;
			// don't notify if project is null!
			// newly activated project will notify
		}
		return TRUE;
	}

	if (pProject->GetActiveConfig() == NULL)
		return FALSE;

	// get our active target name
	CString strTarg = pProject->GetActiveConfig()->GetConfigurationName();

	pProject->SetActiveConfig(strTarg);
	if (pProject != g_pActiveProject)
	{
		g_pActiveProject = pProject;
	}

	// set this target as our active one
	BOOL fRet = g_pActiveProject->SetStrProp(P_ProjActiveConfiguration, strTarg);

	return fRet;	// ok?
}

BOOL CBldSysIFace::GetProjectDir(HBLDTARGET hTarget, HBUILDER hBld, CString &str)
{
	if (hBld == ACTIVE_BUILDER)
		hBld = GetBuilder(hTarget); // don't trust caller default
	if (hBld == NO_BUILDER)
		return FALSE;

	CTargetItem *pTarget = CnvHTarget(hBld, hTarget);

	if  (pTarget)
	{
		str = (const TCHAR *)pTarget->GetTargDir();
		if (str.IsEmpty())
		{
			str = CnvHBuilder(hBld)->GetWorkspaceDir();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CBldSysIFace::GetWorkspaceDir(HBUILDER hBld, CString& str)
{
	CProject * pProject = (CProject *)(hBld != ACTIVE_BUILDER ? hBld : GetActiveBuilder());
	if (pProject)
	{
		str = (const TCHAR *)pProject->GetWorkspaceDir();
		return TRUE;
	}
	return FALSE;	
}


BOOL CBldSysIFace::GetTargetName
(
	HBLDTARGET		hTarg,		// target to get name of
	CString &		str,		// target name
	HBUILDER		hBld		// handle to the builder containing the target, default='active builder'
)
{
	if (hTarg == NO_TARGET)
		return FALSE;

	hBld = GetBuilder(hTarg); // don't trust caller default

	if (hBld == NO_BUILDER)
		return FALSE;

	ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);
    if (pcr != NULL)
	    str = pcr->GetConfigurationName();
 	return (pcr != NULL);
}

// Create a target with name 'pchTarg' and add it to a builder.
// Name of the 'official' platform must be specified in 'pchPlat', eg. "Win32 (x86)"
// Name of the target type must be specified in 'pchType', eg. "Application"
// Create debug or release default settings when a file is added to this target, 'fDebug'?
// A target can mirror another target's, 'hMirrorTarg', fileset.
// Function may fail (HBLDTARGET == NO_TARGET).
HBLDTARGET CBldSysIFace::AddTarget
(
	const TCHAR *	pchTarg,				// name of this target

	const TCHAR *	pchPlat,				// name of this target's platform
	const TCHAR *	pchType,				// name of this target type
	BOOL			fUIDesc,				// platform and type are UI descriptions?
   
   	BOOL			fDebug,					// debug or release settings? default=debug

	TrgCreateOp		trgop,					// clone or mirrors another target? default=no
	HBLDTARGET		hOtherTarg,				// target to clone or mirror

	SettingOp		setop,					// copy or default settings?
	HBLDTARGET		hSettingsTarg,			// target to copy settings from 

	BOOL			bQuiet,					// show any warnings/errors during creation? default=yes
	BOOL			fOutDir,				// output directories? default=no
 	BOOL			fUseMFC,				// use MFC? default=yes
	HBUILDER		hBld					// builder that contains target? default=('current'==ACTIVE_BUILDER) 
)
{
	ASSERT(hBld != ACTIVE_BUILDER);
	if( hBld == ACTIVE_BUILDER || hBld == NO_BUILDER )
		return NO_TARGET;

 	// a string version of our target type, eg. 'Win32 (x86) Application" 
	CString strTargName = CProjType::MakeNameFromPlatformAndType(pchPlat, pchType);

	CProjType * pProjType;
	if ((!fUIDesc && !g_prjcompmgr.LookupProjTypeByName((LPCTSTR)strTargName, pProjType)) ||
		(fUIDesc && !g_prjcompmgr.LookupProjTypeByUIDescription((LPCTSTR)strTargName, pProjType))
	   )
		return NO_TARGET;

	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);
	CProjTempProjectChange projChange(pProject);

	// store away the current config so we can reset it at the end of this
	// operation
	CString strActiveConfig;
	pProject->GetStrProp(P_ProjActiveConfiguration, strActiveConfig);

	// adjust our use of MFC
	fUseMFC = fUseMFC && g_prjcompmgr.MFCIsInstalled();

	// disable notification while we create this configuration
	pProject->m_bNotifyOnChangeConfig = FALSE;

	// a string version of our target name
	CString strTarg;
	if (pchTarg != (const TCHAR *)NULL)
	{
		strTarg = pchTarg;	// user-supplied
	}
	else
	{
		// auto-created by us
 		DefaultConfig(pProject, *(pProjType->GetPlatformUIDescription()), fDebug ? IDS_DEBUG_CONFIG : IDS_RELEASE_CONFIG, strTarg);
	}

	// string versions of our 'other' targets name
	CString strOtherTarg, strSettingsTarg;

	// do we have these?
	if (hOtherTarg != NO_TARGET)
		strOtherTarg = m_strlstTargets.GetAt((POSITION)hOtherTarg);
	else
		ASSERT(trgop == TrgDefault);		// with no other target we must have this

	if (hSettingsTarg != NO_TARGET)
		strSettingsTarg = m_strlstTargets.GetAt((POSITION)hSettingsTarg);
	else
		ASSERT(setop == SettingsDefault);	// with no settings target we must have this!

	CObList olItemList;
	POSITION pos;
	CProjItem *pItem;

	HBLDTARGET hTarg = (HBLDTARGET)GetTarget(strTarg, hBld);
	VERIFY(pProject->CreateTarget(strTarg, pProjType, trgop == TrgMirror ? strOtherTarg : _TEXT("")));

	ConfigurationRecord * pcrNewTarg = GetConfigRecFromTargetI(hTarg, hBld);
	CTargetItem * pNewTarget = pProject->GetTarget(strTarg);
	const CPlatform * pNewPlatform = pProjType->GetPlatform();

	// inform the graph of this target that it is being created
	UINT oldmode = g_buildengine.GetDepGraph(pcrNewTarg)->SetGraphMode(Graph_Creating);

	// set target dir
	if (!g_strTarg.IsEmpty())
		pNewTarget->SetTargDir(g_strTarg);

	if (pNewTarget->GetTargetName().IsEmpty())
	{
		int index = strTarg.Find(" - ");
		ASSERT(index > 0);
		CString strTargetName = strTarg.Left(index);
		pNewTarget->SetTargetName(strTargetName);
		if (pProject->GetTargetName().IsEmpty())
			pProject->SetTargetName(strTargetName);
		ASSERT(strTargetName == pProject->GetTargetName());
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
			int nLen = _tcslen( buffer );

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

		ASSERT(!strOutDirs.IsEmpty());

		// Java doesn't write output to a different directory for now.
		if (pProjType->GetPlatform()->GetUniqueId() == java)
			strOutDirs = "";

		if (!g_strTarg.IsEmpty() )
		{
			CString strTmp;

			if (!strProjDir.IsEmpty())
			{
				if (pProjType->GetPlatform()->GetUniqueId() == java)
					strTmp = strProjDir;
				else
					strTmp = strProjDir + _T("\\") + strOutDirs;
			}
			else
			{
				strTmp = strOutDirs;
			}
			strOutDirs = strTmp;
		}

		// need to set dirs earlier for powermac dll, because we might add files
		// if ((pNewPlatform->GetUniqueId() == macppc) && ((pProjType->GetAttributes() & ImageDLL) != 0))
		{
			pProject->SetStrProp(P_OutDirs_Intermediate, strOutDirs);
			pProject->SetStrProp(P_OutDirs_Target, strOutDirs);
			pProject->SetStrProp(P_Proj_TargDir, strProjDir);
		}
	}

	// assign the build actions
	CActionSlob::AssignActions(pNewTarget, (CPtrList *)NULL, pcrNewTarg);


	// do the target-level settings
	if (setop != SettingsClone)
	{
		// create a fresh configuration here

		// set the debug/release default settings	
		pProject->SetIntProp(P_UseDebugLibs, fDebug); // used for MFC libs

		// set a deferred browser database make
		pProject->SetIntProp(P_NoDeferredBscmake, FALSE);

		if (fDebug)
			VERIFY(g_prjcompmgr.SetDefaultDebugToolOptions(pProject));
		else
			VERIFY(g_prjcompmgr.SetDefaultReleaseToolOptions(pProject));

		pProject->SetStrProp(P_ProjActiveConfiguration, strTarg);
		if (fDebug)
			VERIFY(pProjType->SetDefaultDebugTargetOptions(pProject, pNewTarget, fUseMFC));
		else
			VERIFY(pProjType->SetDefaultReleaseTargetOptions(pProject, pNewTarget, fUseMFC));
	}
	else
	{
		ASSERT(setop == SettingsClone);	// must be clone

		// we are creating a new config by cloning from an existing one

	 	// get the platform for the original 'settings' target
	    ConfigurationRecord * pcrOldTarg = GetConfigRecFromTargetI(hSettingsTarg, hBld);

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
				VERIFY(g_prjcompmgr.SetDefaultDebugToolOptions(pProject));
			else
				VERIFY(g_prjcompmgr.SetDefaultReleaseToolOptions(pProject));
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
			VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Linker),
												 (CBldSysCmp *&)popthdlr));
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
			HFILESET hFileSet = GetFileSet(ACTIVE_BUILDER, hTarg);
			for (POSITION pos = pTarget->GetHeadPosition(); pos != NULL;)
			{
				CObject* pOb = pTarget->GetNext(pos);
				// We copy only fileitems except target references
 				if (pOb->IsKindOf(RUNTIME_CLASS(CFileItem)) && !pOb->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
  					(void)AddFile(hFileSet, ((CFileItem *)pOb)->GetFilePath(), FALSE, hBld);
  			}
		}
		else
		{
            // We are mirroring files so we must preserve deps
			if (!g_bNewUpdateDepModel)
				PreserveDependencies(pNewTarget, pProject);
		}

		// we have to make sure that we don't have multiple .rc and .def
		// defined
		// 
		// First, flatten the subtree
		CObList FileItemList;
		const ConfigurationRecord * pcr = pProject->GetActiveConfig();
		ASSERT_VALID(pNewTarget);
		CFileRegistry* pTargetRegistry = pNewTarget->GetRegistry();
		
		BOOL fMultiRC = pTargetRegistry->GetFileItemList(pNewTarget, pTargetRegistry->GetRCFileList(),
													   FileItemList, FALSE, pcr) && FileItemList.GetCount() > 1;
		BOOL fMultiDEF = pTargetRegistry->GetFileItemList(pNewTarget, pTargetRegistry->GetDEFFileList(),
														FileItemList, FALSE, pcr) && FileItemList.GetCount() > 1;

		// need to make the check?
		if (fMultiRC || fMultiDEF)
		{
			BOOL fInformOfMultiRC = fMultiRC, fInformOfMultiDEF = fMultiDEF;

			pNewTarget->FlattenSubtree(olItemList, CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeGroups);
			for (pos = olItemList.GetHeadPosition(); pos != NULL ;)
			{
				pItem = (CProjItem *) olItemList.GetNext(pos);
				if (!pItem->IsKindOf(RUNTIME_CLASS(CFileItem))) // only interested in file items
					continue;

				CPtrList * pFileList;
				if (pTargetRegistry->GetFileList(pItem->GetFilePath(), (const CPtrList * &)pFileList))	// multiple ext file?
				{
					TCHAR * pExt = (TCHAR *)NULL;
					BOOL fMulti = FALSE;
					if (pFileList == pTargetRegistry->GetRCFileList())
					{
						fMulti = fMultiRC;
						if (fInformOfMultiRC)
						{
							pExt = _TEXT(".rc");
							fInformOfMultiRC = FALSE;
						}
					}
					else if (pFileList == pTargetRegistry->GetDEFFileList())
					{
						fMulti = fMultiDEF;
						if (fInformOfMultiDEF)
						{
							pExt = _TEXT(".def");
							fInformOfMultiDEF = FALSE;
						}
					}
					else
						// need to recognise other multiple file sets!
						ASSERT(FALSE);

					// multiple of these?
					if (!fMulti)
						continue;	// no!

					if (pExt == (TCHAR *)NULL)
					{
						// exclude this item from build	(both for default and current prop. bags)
						pItem->SetIntProp(P_ItemExcludedFromBuild, TRUE);

						int idOldBag = pItem->UsePropertyBag(BaseBag);
						pItem->SetIntProp(P_ItemExcludedFromBuild, TRUE);
						(void) pItem->UsePropertyBag(idOldBag);
					}
					else
					{
						// warn of this file? (only if not in quiet mode)
						if (!bQuiet)
							InformationBox(IDS_FILE_TYPE_EXISTS, pExt);
					}
				}
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
			HFILESET hFileSet = GetFileSet(ACTIVE_BUILDER, hTarg);
			for (POSITION pos = pTarget->GetHeadPosition(); pos != NULL;)
			{
				CObject* pOb = pTarget->GetNext(pos);
 				if (pOb->IsKindOf(RUNTIME_CLASS(CFileItem)) && !pOb->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
  					(void)AddFile(hFileSet, ((CFileItem *)pOb)->GetFilePath(), FALSE, hBld);
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
			ASSERT(trgop == TrgMirror);		// Must be mirror

            // We are mirroring files so we must preserve deps
			if (!g_bNewUpdateDepModel)
				PreserveDependencies(pNewTarget, pProject);

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

	// inform the graph of this target that it is now back in its original state
	(void) g_buildengine.GetDepGraph(pcrNewTarg)->SetGraphMode(oldmode);

	// finally make sure that we remember the 'base' (default) options
 	pProject->SetCurrentConfigAsBase();

	// set 'default' config to first debug config. created
 	if (fDebug &&
 		(pProject->m_strProjDefaultConfiguration.IsEmpty() ||
		 pProjType->GetPlatform()->IsPrimaryPlatform()     ||
		 pProjType->GetPlatform()->GetUniqueId() == java)			 
	   )
	{
		pProject->m_strProjDefaultConfiguration = strTarg;
	}

	// Make sure we are in the same config we were in when we
	// started to do this operation
	if (!strActiveConfig.IsEmpty())
		pProject->SetStrProp(P_ProjActiveConfiguration, strActiveConfig);

	// re-enable config. change notifications
	pProject->m_bNotifyOnChangeConfig = TRUE;

	// Send out a notification to other packages to tell them that
	// there is a new target
	if (pProject->m_bProjectComplete)
    {
		LPPROJECTWORKSPACE pProjSysIFace = FindProjWksIFace();
        BOOL bWorkspaceInitialised = pProjSysIFace->IsWorkspaceInitialised() == S_OK;
		pProject->m_listTargIncl.AddTarget (hTarg);

        if (bWorkspaceInitialised)
    		theApp.NotifyPackages(PN_ADD_TARGET, (void *)hTarg);
    }

	return hTarg;
}

BOOL CBldSysIFace::AddDefaultTargets
(
	const TCHAR *	pchPlat,				// UI or 'official' name of this target's platform
	const TCHAR *	pchType,				// UI or 'official' name of this target type

	HBLDTARGET &	hDebugTarg,				// our created debug target
	HBLDTARGET &	hReleaseTarg,			// our created release target

	BOOL			fUIDesc,				// using UI or 'official' names? default=UI
	BOOL			fOutDir,				// output directories? default=yes
	BOOL			fUseMFC,				// use MFC? default=yes

	HBLDTARGET		hMirrorTarg,			// do we want to 'chain the mirroring'?
	HBUILDER		hBld,					// builder that contains target? default='active builder'
	const TCHAR *	pchTarg 				// name of this target, default use NULL to official names
)
{
	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);
	hBld = (HBUILDER)pProject; // convert ACTIVE_PROJECT to actual value;
	if( hBld == NO_BUILDER )
		return FALSE;

	g_strTarg = pProject->GetWorkspaceDir();
	
	CProjTempProjectChange projChange(pProject);

	hDebugTarg = hReleaseTarg = NO_TARGET;

	CString strTarg;

	// a string version of our target type, eg. 'Win32 (x86) Application" 
	CString strTargName = CProjType::MakeNameFromPlatformAndType(pchPlat, pchType);

	// Get the project type for these targets
	CProjType * pProjType = NULL;
	if ((!fUIDesc && !g_prjcompmgr.LookupProjTypeByName((LPCTSTR)strTargName, pProjType)) ||
		(fUIDesc && !g_prjcompmgr.LookupProjTypeByUIDescription((LPCTSTR)strTargName, pProjType))
	   )
		return FALSE;

	//
	// create our release config. for this project type
	//

	if (pchTarg != NULL)
	{
		CString strMode, strPlatformUIDescription;
		VERIFY(strMode.LoadString(IDS_RELEASE_CONFIG));
		strPlatformUIDescription = *(pProjType->GetPlatformUIDescription());
		strTarg = pchTarg;	// user-supplied
		strTarg += _T(" - ") + strPlatformUIDescription + _T(" ") + strMode;
	}
	else
	{
		strTarg = _TEXT("");
	}

	hReleaseTarg = AddTarget
	(
		(pchTarg!=NULL) ? (LPCTSTR)strTarg:NULL,

		pchPlat,				// 'official' name of this target's platform
		pchType,				// 'official' name of this target type
		fUIDesc,				// we're using 'official' names

		FALSE,					// release settings

		hMirrorTarg == NO_TARGET ? TrgDefault : TrgMirror,	// default or 'chain-the-mirroring' target? default=no chaining
		hMirrorTarg,										// no target to clone or mirror

		SettingsDefault,		// default settings
		NO_TARGET,				// no target to copy settings from
		 
		FALSE,					// show any warnings/errors during creation? default=yes
		fOutDir,				// create output directories?
		fUseMFC,				// use MFC?
		hBld					// builder that contains target? default='active builder'
	);
	if (hReleaseTarg == NO_TARGET)
		return FALSE;	// failure

	//
	// create our debug config. for this project type
	//
	
	if (pchTarg != NULL)
	{
		CString strMode, strPlatformUIDescription;
		VERIFY(strMode.LoadString(IDS_DEBUG_CONFIG));
		strPlatformUIDescription = *(pProjType->GetPlatformUIDescription());
		strTarg = pchTarg;	// user-supplied
		strTarg += _T(" - ") + strPlatformUIDescription + _T(" ") + strMode;
	}
	else
	{
		strTarg= _TEXT("");
	}

	hDebugTarg = AddTarget
	(
		(pchTarg!=NULL) ? (LPCTSTR)strTarg:NULL,

		pchPlat,				// 'official' name of this target's platform
		pchType,				// 'official' name of this target type
		fUIDesc,				// we're using 'official' names

		TRUE,					// debug settings

		TrgMirror,				// default target
		hReleaseTarg,			// mirror other default target

		SettingsDefault,		// default settings
		NO_TARGET,				// no target to copy settings from
		 
		FALSE,					// show any warnings/errors during creation? default=yes
		fOutDir,				// create output directories?
		fUseMFC,				// use MFC?
		hBld					// builder that contains target? default='active builder'
	);
	if (hDebugTarg == NO_TARGET)
		return FALSE;	// failure

 	// set active config to be the default
	pProject->SetStrProp(P_ProjActiveConfiguration, pProject->m_strProjDefaultConfiguration);

#if 0	// Groups temporarily disabled
	// UNDONE: fix default Group logic
	if (fOutDir && fUseMFC && (pchTarg!=NULL))
	{
		// Add default groups
		pProject->AddNewGroup("Source Files","cpp;c;cxx;rc;def");
		pProject->AddNewGroup("Header Files","h;hxx;hpp");
		pProject->AddNewGroup("Resource Files","ico;rc2;bmp"); // UNDONE
	}
#endif
	g_strTarg = "";
 
	return TRUE;	// success
}

// Delete an existing target
void CBldSysIFace::DeleteExeTarget
(
	HBLDTARGET		hTarg		// target to delete
)
{
	m_mapPrjTargets.RemoveKey(hTarg);
	m_strlstTargets.RemoveAt((POSITION)hTarg);

}

// Delete an existing target
BOOL CBldSysIFace::DeleteTarget
(
	HBLDTARGET		hTarg,		// target to delete
	HBUILDER		hBld		// builder that contains target? default='active builder'
)
{
	// get a builder for this target to be searched in

	ASSERT(NO_BUILDER!=hBld);
	HBUILDER hCmpBld = GetBuilder(hTarg);
	ASSERT(NO_BUILDER!=hCmpBld);
	ASSERT(hBld == hCmpBld);
	hBld = hCmpBld;
	ASSERT(hBld);

	CProject * pProject = CnvHBuilder(hBld);
	ASSERT(pProject);

	if( pProject == NULL )
		return FALSE;

	// Get the name of the target to delete
	CString strTarget;
	GetTargetName(hTarg, strTarget, hBld);

	LPPROJECTWORKSPACE pProjSysIFace = FindProjWksIFace();
	BOOL bWorkspaceInitialised = (pProjSysIFace->IsWorkspaceInitialised() == S_OK);

	const CPtrArray* pConfigArray = pProject->GetConfigArray();
	int nSize = pConfigArray->GetSize();
	if (pProject->m_bProjectComplete)
	{
		// if we're destroying the project, make sure it is not active
		if ((nSize==1) && (pProject == g_pActiveProject))
			DeactivateBuilder(hBld);
	}

	// Delete the target
	BOOL bRetVal = pProject->DeleteTarget(strTarget);

	// Remove the target from our list of targets if we succeeded in deleting it
	if (bRetVal)
	{
		m_mapPrjTargets.RemoveKey(hTarg);
 		m_strlstTargets.RemoveAt((POSITION)hTarg);
	}

	pConfigArray = pProject->GetConfigArray();
	nSize = pConfigArray->GetSize();
	if (pProject->m_bProjectComplete)
	{
		// If the delete operation succeeded then send out a notification
		// to other packages to tell them that this target has been deleted
		if (bRetVal && bWorkspaceInitialised)
  	 		theApp.NotifyPackages(PN_DEL_TARGET, (void *) hTarg);

		ASSERT(bRetVal);
		if ((bRetVal !=0) && (nSize==0) && (!g_bInProjClose)) // avoid recursion
		{
			// are we deleting the entire project?
			IBSProject *pBSProj = pProject->GetInterface();
			COleRef<IPkgProject> pPkgProj;
			pBSProj->QueryInterface(IID_IPkgProject, (void **)&pPkgProj);
			//FindProjWksIFace()->SetActiveProject(pPkgProj, TRUE);
			pBSProj->Release();
			bRetVal = SUCCEEDED(g_pProjWksIFace->DeleteProject(pPkgProj, FALSE));
		}

	}

	// success?
	return bRetVal;
 }

// Add a target as a dependency of another target.
HPROJDEP CBldSysIFace::AddProjectDependency
(
	HFILESET		hFileSet,			// fileset in which to *place* dependency
	HFILESET		hDependantFileSet,	// fileset on which to place a dependency
	HBUILDER		hBld, 				// builder that contains hFileSet'
	HBUILDER		hDependantBld /* = NO_BUILDER */	// builder that contains hDependantFileSet (default is to use hBld)
)
{
	// make sure we have a builder for this target to be searched in
	if (hDependantBld == NO_BUILDER)
		hDependantBld = hBld;

	if (hBld == ACTIVE_BUILDER)
		hBld = GetBuilderFromFileSet(hFileSet); // don't trust default

	if (hDependantBld == ACTIVE_BUILDER)
		hDependantBld = GetBuilderFromFileSet(hDependantFileSet); // don't trust default

    // get the target item for the target we are adding to.
	CProject *pTopProject = CnvHBuilder(hBld);
	CProject *pSubProject = CnvHBuilder(hDependantBld);
    CTargetItem* pTarget = CnvHFileSet(hBld, hFileSet);
	CTargetItem* pDependantTarget = CnvHFileSet(hDependantBld, hDependantFileSet);

	if( pTopProject==NULL || pSubProject==NULL || pTarget==NULL || pDependantTarget==NULL ){
		// something is bogus. Bail now !
		return (HPROJDEP)NULL;
	}

    // Create the target dependency
    CProjectDependency* pProjectDep = new CProjectDependency();
    pProjectDep->SetProjectDep(pDependantTarget);

	CString strTop; pTopProject->GetName(strTop);
	CString strSub; pSubProject->GetName(strSub);

	::AddDep( strTop, strSub, pProjectDep );

	// Add the target dependency to the required target
	pTopProject->SetOkToDirtyProject(FALSE);
    pProjectDep->MoveInto(pTarget);
	pTopProject->SetOkToDirtyProject();

	// Return the Target dep as a file (allows doing things like checking
	// for dupes)
	return (HPROJDEP)pProjectDep;
}

BOOL CBldSysIFace::RemoveProjectDependency
(
	HPROJDEP		hProjDep,				// handle to the project dependency
	HFILESET		hFileSet,				// handle to the fileset
	HBUILDER		hBld					// builder that contains target? default='active builder'
)
{
	// Get the file item 
	CProjectDependency * pProjDep = CnvHProjDep(hFileSet, hProjDep);

	// Delete it
	pProjDep->MoveInto(NULL);

	return TRUE;	// ok
}

HFILESET CBldSysIFace::GetDependantProject
(
	HPROJDEP		hProjDep,				// handle to the project dependency
	HFILESET		hFileSet,				// handle to the fileset
	HBUILDER		hBld					// builder that contains target? default='active builder'
)
{
	CProjectDependency * pProjDep = CnvHProjDep(hFileSet, hProjDep);
	
	CTargetItem * pTarget = pProjDep->GetProjectDep();

	return (HFILESET)pTarget;
}

// FUTURE: target deletion API

// Delete an existing target
HFOLDER CBldSysIFace::AddFolder(HFILESET hFileSet, const TCHAR * pchFolder, HBUILDER hBld /* = ACTIVE_BUILDER */, HFOLDER hFolder /* = NO_FOLDER */, const TCHAR * pchFilter /* = NULL */)
{
	// get a builder for this target to be searched in
	CProjContainer * pContainer = (CProjContainer *)hFolder;
	if ((pContainer == NULL) || (!pContainer->IsKindOf(RUNTIME_CLASS(CProjContainer))))
	{
		// get the target item 
		pContainer = (CProjContainer *)CnvHFileSet(hBld, hFileSet);
	}

	return (HFOLDER)(CProjContainer *)(pContainer->AddNewGroup(pchFolder, pchFilter));
}

extern BOOL g_bExcludedNoWarning; // defined in pfilereg.cpp

// Create a file and add it to a target, 'hTarg', with or without default settings, 'fSettings'.
// Function may fail (HBLDFILE == NO_FILE).
HBLDFILE CBldSysIFace::AddFile
(
	HFILESET		hFileSet,				// handle to the fileset
	const CPath *	ppathFile,				// file path
	int				fSettings,			// default settings for this fileitem
	HBUILDER		hBld,					// builder that contains target? default='active builder' 
	HFOLDER			hFolder					// default is NO_FOLDER
)
{
	ASSERT((fSettings & ~(ADB_EXCLUDE_FROM_BUILD | ADB_EXCLUDE_FROM_SCAN))==0);	// NYI

	// get a builder for this target to be added to
	if (hBld == ACTIVE_BUILDER)
		hBld = GetActiveBuilder();

	if( hBld == NO_BUILDER )
		return NO_FILE;

	CProject * pProject = CnvHBuilder(hBld);
	CProjTempProjectChange projChange(pProject);

	// get the target item 
	CTargetItem * pTarget = CnvHFileSet(hBld, hFileSet);
	if (pTarget == NULL)
		return NO_FILE;

	// reject wildcards
	if (_tcspbrk(*ppathFile, _T("*?")))
	{
		return NO_FILE;
	}

	CDir dir;
	// make sure we're not trying to add a directory
	if (dir.CreateFromString((LPCTSTR)*ppathFile) && dir.ExistsOnDisk())
	{
		return NO_FILE;
	}
	// special logic to identify template files so that they go
	// in the right folder and so that ClassView never sees them
	BOOL bIsTemplate = ((fSettings & ADB_EXCLUDE_FROM_SCAN)!=0);
	if ((!bIsTemplate) && (dir.CreateFromPath(*ppathFile)))
	{
		CString strDir = dir;
		strDir.MakeLower();
		int nTemplate, nEnd;
		if ((nTemplate = strDir.Find("\\template")) != -1)
		{
			nEnd = nTemplate + strlen("\\template");
			if ((nEnd >= strDir.GetLength()) || (strDir[nEnd] == '\\'))
			{
				bIsTemplate = TRUE;
				fSettings |= (ADB_EXCLUDE_FROM_SCAN | ADB_EXCLUDE_FROM_BUILD);
			}
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
		while (pos != NULL)
		{
			pItem = (CProjItem *)ol.GetNext(pos);
			ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CProjItem)));
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
	
	// get the targets file registry
	CFileRegistry * pRegistry = pTarget->GetRegistry();
	ASSERT(pRegistry != NULL);

	// check to see if the file is in the target already
	FileRegHandle frh = pRegistry->LookupFile(ppathFile);
	if (frh != (FileRegHandle)NULL)
	{
		CObList * oblist = g_FileRegistry.GetRegEntry(frh)->plstFileItems;
#ifdef REFCOUNT_WORK
		frh->ReleaseFRHRef();
#endif
		if (oblist != NULL)
		{
			// already there, can't do this!
			// ASSERT(FALSE);
			return FALSE;
		}
	}

	// create the file
	CFileItem * pFile;
	if ((pFile = new CFileItem) == (CFileItem *)NULL)
		return NO_FILE;

	// set the file path
	pFile->SetFile(ppathFile);

	// REVIEW: only do this for class-oriented files
	// see if we should exclude this from clsview, etc.
	// Note: must do this before the MoveInto...
	if ((fSettings & ADB_EXCLUDE_FROM_SCAN)!=0)
		pFile->SetIntProp(P_ItemExcludedFromScan, TRUE);

	// avoid gratuitous warning if we're about to exclude anyway
	if ((fSettings & ADB_EXCLUDE_FROM_BUILD)!=0)
		g_bExcludedNoWarning = TRUE;

	// move the file into the target
	pFile->MoveInto(pContainer);

	if ((fSettings & ADB_EXCLUDE_FROM_BUILD)!=0)
		g_bExcludedNoWarning = FALSE;

	// see if we should exclude this from the build
	if (((fSettings & ADB_EXCLUDE_FROM_BUILD)!=0)
//		|| (FileNameMatchesExtension(ppathFile, "rc2"))
		)
	{
		// REVIEW: only do this for class-oriented files
		pFile->SetIntProp(P_ItemExcludedFromScan, TRUE);

		int iSize = pTarget->GetPropBagCount();
		ASSERT(iSize);
		const CPtrArray * pArrayCfg = pTarget->GetConfigArray();
		for (int i = 0; i < iSize; i++)
		{
			ConfigurationRecord * pcr = (ConfigurationRecord *)(*pArrayCfg)[i]; 
			if (pcr->IsValid())
			{
				CProjTempConfigChange projTempConfigChange(pProject);
				projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
				pFile->SetIntProp(P_ItemExcludedFromBuild, TRUE);
				int idOldBag = pFile->UsePropertyBag(BaseBag);
				pFile->SetIntProp(P_ItemExcludedFromBuild, TRUE);
				(void) pFile->UsePropertyBag(idOldBag);
			}
		}
	}

	// return the newly created file
	return (HBLDFILE)pFile;
}

void CBldSysIFace::AddMultiFiles
(
	HFILESET			hFileSet,		// handle to the fileset
	const CPtrList *	plstpathFiles,		// pointer list of file paths (const CPath *'s)
	CPtrList *			plstHBldFiles,		// returned list of HBLDFILEs
	int					fSettings,	// default settings for this fileitem
	HBUILDER			hBld,			// builder that contains target? default='active builder'
	HFOLDER				hFolder			// folder to insert files into; default is NO_FOLDER
)
{
    CProject * pProject = CnvHBuilder(hBld);

	if( pProject == NULL )
		return;	// failure

	CProjTempProjectChange projChange(pProject);

    pProject->BeginBatch();

	// inform the graphs that these targets are now being modified
	g_buildengine.SetGraphMode(Graph_Creating, CnvHBuilder(hBld));

	POSITION pos = plstpathFiles->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		HBLDFILE hFile = AddFile(hFileSet, (const CPath *)plstpathFiles->GetNext(pos), fSettings, hBld, hFolder);
		plstHBldFiles->AddTail((void *)hFile);
	}
	
	// inform the graphs that these targets are now stable
	g_buildengine.SetGraphMode(Graph_Stable, CnvHBuilder(hBld));

    pProject->EndBatch();
}

// Add a dependency file to a fileset, 'hFileSet'.
// ** Note, this doesn't actually add the file as such, it just informs
// other parties, say SCC, that we have a new file that is a 'part of' the fileset.
// Use ::AddFile if you want to *actually* be a part of the fileset.
// Function may fail (HBLDFILE == NO_FILE).
BOOL CBldSysIFace::AddDependencyFile
(
	HFILESET		hFileSet,	// handle to the fileset
	const CPath *	ppathFile,	// dependency file path
	HBUILDER		hBld,		// builder that contains target? default='active builder'
	HFOLDER			hFolder		// folder to insert dep into
)
{
	HBLDFILE hFile = AddFile(hFileSet, ppathFile, 0 /* ADB_EXCLUDE_FROM_BUILD */, hBld, hFolder);
	return (hFile != NO_FILE);// Function may fail (HBLDFILE == NO_FILE).
}

BOOL CBldSysIFace::CheckFile
(
	HFILESET		hFileSet,				// handle to the fileset
	CPath &			pathFile,				// file path
	HBUILDER		hBld					// builder that contains target? default='active builder'
)
{
	CObList lstFile;
	CTargetItem *pTarg =  CnvHFileSet(ACTIVE_BUILDER, hFileSet);
	if( pTarg == NULL )
		return FALSE;

	pTarg->FlattenSubtree(lstFile, CProjItem::flt_Normal | CProjItem::flt_ExcludeGroups);

	BOOL bFileFound, bRetVal = TRUE;
	POSITION pos;

	do
	{
		bFileFound = FALSE;
		
		// Check the files in the fileset
		pos = lstFile.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CFileItem * pItem = (CFileItem *)lstFile.GetNext(pos);
			if (!pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
				continue;

			// Does the file exist in the fileset, if so then
			// we flag the fact that we've found the file, and
			// "warp" the filename, so that the next time thru we
			// hopefully won't find the file.
			if (*(pItem->GetFilePath()) == pathFile)
			{
				bFileFound = TRUE;
				bRetVal = FALSE;
				pathFile.PostFixNumber();
				break;
			}
		}
	} while (bFileFound);

	// Did we need to change the filename
	return bRetVal;
}

// Delete a file 'hFile' from a fileset, 'hFileSet'
BOOL CBldSysIFace::DelFile
(
	HBLDFILE 		hFile,		// handle to file
	HFILESET		hFileSet,	// handle to the fileset
	HBUILDER		hBld		// builder that contains target? default='active builder'
)
{
	// Get the file item 
	CFileItem * pItem = CnvHBldFile(hFileSet, hFile);

	// Delete it
	pItem->MoveInto(NULL);

	return TRUE;	// ok
}

BOOL CBldSysIFace::GetFile
(
	const CPath * 	ppathFile, 	// file path
	HBLDFILE &		hFile,		// handle to the file
	HFILESET		hFileSet,	// handle to the fileset
	HBUILDER		hBld,		// builder that contains target? default='active builder'
	BOOL			bSimple	// if TRUE check includes deps and intermediate target files, default = FALSE,
)
{
	// Get the target item 
	CTargetItem * pTarget = CnvHFileSet(hBld, hFileSet);
	if (pTarget == NULL)
		return FALSE;

	// Get the targets file registry
	CFileRegistry * pRegistry = pTarget->GetRegistry();
	ASSERT(pRegistry != NULL);

	// Check to see if the file is in the target
	FileRegHandle frh = pRegistry->LookupFile(ppathFile);
	if (frh == NULL)
		return FALSE;

	// Do we care about dependencies and intermediate target files?
	if (bSimple)
#ifndef REFCOUNT_WORK
		return TRUE;
#else
	{
		frh->ReleaseFRHRef();
		return TRUE;
	}
#endif

	// The file is there, so now get the associated list of
	// file items for this file reg entry, and if there is
	// no list then this item is not really part of the target
	// (i.e. it is a dependency, or an intermediate target)
	CObList * oblist = g_FileRegistry.GetRegEntry(frh)->plstFileItems;
#ifdef REFCOUNT_WORK
	frh->ReleaseFRHRef();
#endif
	if (oblist == NULL)
		return FALSE;

	POSITION pos = oblist->GetHeadPosition();
	while (pos != NULL)
	{
		// Get the file item and its parent
		CFileItem * pFile = (CFileItem *)oblist->GetNext(pos);
		CTargetItem * pContainer = pFile->GetTarget();

		// Is this file item in our target, if so we've found the file
		if (pContainer == pTarget)
		{
			hFile = (HBLDFILE)pFile;
			return TRUE;
 		}
	}

    // We found an entry in the registry but there is no associated projitem
    // that is contained in the target, so we haven't really found the file
	return FALSE;
}

// Add/subtract tool settings, pchSettings, to a target, 'hTarg'.
// Must specify the 'executable' name of the tool'.
BOOL CBldSysIFace::SetToolSettings
(
	HBLDTARGET		hTarg,					// handle to the target
	const TCHAR * 	pchSettings,			// tool settings
	const TCHAR *	pchTool, 				// executable of the tool that 'owns' these settings
	BOOL			fAdd,					// add or subtact? default=add
	BOOL			fClear,					// clear settings before add or subtract?
	HBUILDER		hBld					// builder that contains target? default='active builder'
)
{
	return SetToolSettingsI(hTarg, CnvHBuilder(hBld), pchSettings, pchTool, fAdd, fClear);
}

// Add/subtract tool settings, pchSettings, to a file, 'hFile'.
// Optional to specify the 'executable' name of the tool'.
BOOL CBldSysIFace::SetToolSettings
(
	HBLDTARGET		hTarg,					// handle to the target that contains the file
	HBLDFILE		hFile,					// handle to the file
	const TCHAR * 	pchSettings,			// tool settings
	const TCHAR *	pchTool, 				// executable of the tool that 'owns' these settings
	BOOL			fAdd,					// add or subtact? default=add
	BOOL			fClear					// clear settings before add or subtract?
)
{
	HFILESET hFileSet = GetFileSet(ACTIVE_BUILDER, hTarg);
	if( hFileSet == NO_FILESET )
		return FALSE;

	return SetToolSettingsI(hTarg, CnvHBldFile(hFileSet, hFile),
							pchSettings, pchTool, fAdd, fClear);
}

void CBldSysIFace::MakeTargPropsDefault
(
	HBLDTARGET		hTarg,					// handle to the target
	HBUILDER		hBld					// builder that contains target? default='active builder'
)
{
	// get this target's config record
	ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);

	CProject * pItem = CnvHBuilder(hBld);
	if( pItem == NULL )
		return; 	// failure

	// force this item into our config., make default current, then reset config.
	pItem->ForceConfigActive(pcr);	
	pItem->SetCurrentConfigAsBase();
	pItem->ForceConfigActive();
}

// Get a string property for this target
BOOL CBldSysIFace::GetTargetProp
(
	HBLDTARGET		hTarg,					// handle to the target
	UINT			idProp,					// identifier of the property
	CString &		str,						// property value
	HBUILDER		hBld					// builder that contains target? default='active builder'
)
{
	BOOL bRetVal = FALSE;
	CPath Path;

    // Handle default builder
    if (hBld == ACTIVE_BUILDER)
        hBld = GetActiveBuilder();

    // We should have a target
    ASSERT(hTarg != NO_TARGET);

	// Get the project item
	CProject * pProject = CnvHBuilder(hBld);

	if( pProject == NULL )
		return FALSE;
		

	// Get the internal prop number if there is one
	UINT idPropI;
	if (!MapPropToPropI(idProp, idPropI))
		return FALSE;

	if (idPropI == 0)
	{
		// Faked properties
		switch (idProp)
		{
			case Prop_TargetDirectory:
 				str = pProject->GetProjDir();
				bRetVal = TRUE;
				break;

			default:
 				// Unknown faked property, so fail the operation
				bRetVal = FALSE;
				break;
		}
	}
	else
	{
		// Real internal property

		// get this target's config record
		ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);

		// force this item into our config and get the property value
		pProject->ForceConfigActive(pcr);	
  		bRetVal = (pProject->GetStrProp(idPropI, str) == valid);
  		pProject->ForceConfigActive();
 	}

	return bRetVal;
}

// Get an integer property for this target
BOOL CBldSysIFace::GetTargetProp
(
	HBLDTARGET		hTarg,					// handle to the target
	UINT			idProp,					// identifier of the property
	int &			i,						// property value
	HBUILDER		hBld					// builder that contains target? default='active builder'
)
{
	BOOL bRetVal = FALSE;
	CPath Path;

    // Handle default builder
    if (hBld == ACTIVE_BUILDER)
        hBld = GetActiveBuilder();

    // We should have a target
    ASSERT(hTarg != NO_TARGET);

	// Get the project item
	CProject * pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;

	// Get the internal prop number if there is one
	UINT idPropI;
	if (!MapPropToPropI(idProp, idPropI))
		return FALSE;

	if (idPropI == 0)
	{
		// Faked properties
 	}
	else
	{
		// Real internal property

		// get this target's config record
		ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);

		// force this item into our config and get the property value
		pProject->ForceConfigActive(pcr);	
  		bRetVal = pProject->GetIntProp(idPropI, i);
  		pProject->ForceConfigActive();
 	}

	return bRetVal;
}

// Set an integer or string property for this target
BOOL CBldSysIFace::SetTargetProp
(
	HBLDTARGET		hTarg,					// handle to the target
	UINT			idProp,					// identifier of the property
	int				i,						// property value
	HBUILDER		hBld					// builder that contains target? default='active builder'
)
{
	BOOL fResetAllDefault = FALSE;

	UINT idPropI;
	if (!MapPropToPropI(idProp, idPropI))
		return FALSE;

	// get this target's config record
	ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);

	CProject * pItem = CnvHBuilder(hBld);
	if( pItem == NULL || pcr == NULL )
		return FALSE;

	CProjTempProjectChange projChange(pItem);

	// pretend that we're complete so that we don't ignore these prop. sets
	// kinda hacky, but appwiz will set these and we will ignore
	// them in CProjItem::InformDependants if we think we're creating a project
	BOOL bOldProjectComplete = pItem->m_bProjectComplete;
	if (idPropI == P_OutDirs_Target || idPropI == P_OutDirs_Intermediate)
		pItem->m_bProjectComplete = TRUE;

	// force this item into our config.
	CProjTempConfigChange projTempConfigChange(pItem->GetProject());
	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	

	// set integer property
	BOOL fRet = pItem->SetIntProp(idPropI, i);
	if (fRet)
		pcr->BagCopy(CurrBag, BaseBag, idPropI, idPropI);

	pItem->m_bProjectComplete = bOldProjectComplete;

	// have the target wizards ensure this is fine..
	CProjType * pprojtype = pItem->GetProjType();

	if (pprojtype != (CProjType *)NULL)
	{
		// Microsoft Foundation Classes Wizard?
		if (idProp == Prop_UseOfMFC)
		{
			pprojtype->PerformSettingsWizard(pItem, i);
			fResetAllDefault = TRUE;
		}
	}

	// reset this item's config.
	projTempConfigChange.Release();

	// make sure we still have all current as defaults?
	if (fResetAllDefault)
		MakeTargPropsDefault(hTarg, hBld != ACTIVE_BUILDER ? hBld : GetActiveBuilder());

	return fRet;	// ok?
}

BOOL CBldSysIFace::SetTargetProp
(
	HBLDTARGET		hTarg,					// handle to the target
	UINT			idProp,					// identifier of the property
	CString &		str,					// property value
	HBUILDER		hBld					// builder that contains target? default='active builder'
)
{
	UINT idPropI;
	if (!MapPropToPropI(idProp, idPropI))
		return FALSE;

	// get this target's config record
	ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);

	CProject * pItem = CnvHBuilder(hBld);

	if( pItem == NULL || pcr == NULL )
		return FALSE;

	// force this item into our config.
	CProjTempConfigChange projTempConfigChange(pItem->GetProject());
	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	

	// set string property
	BOOL fRet = pItem->SetStrProp(idPropI, str);
	if (fRet)
		pcr->BagCopy(CurrBag, BaseBag, idPropI, idPropI);
  	

	return fRet;	// ok?
}

// Returns the state of the target.
// Any files that are considered 'final' products,
// eg. an .exe or .dll, are returned in the 
// path list 'lstPath'. Use FileState() to determine
// the state of these.
UINT CBldSysIFace::TargetState
(
	CObList *	plstPath /* = NULL */,		// paths that are not current (== (CObList *)NULL if not req'd)	
	const TCHAR * pchTarget /* = NULL */,	// default is the primary target (usually .exe)
	HBLDTARGET	hTarg /* = NO_TARGET */,	// handle to the target 
	HBUILDER	hBld /* = ACTIVE_BUILDER */	// handle to the builder that contains this
)
{
	if (!theApp.m_bInvokedCommandLine)
	{
		// checks if there are some 'dirty' project files in the IDE
		// (ie: not saved yet)
		if (!g_VPROJIdeInterface.Initialize())
			return TRG_STAT_Unknown;
		
		if (g_VPROJIdeInterface.GetEditDebugStatus()->ProjectFilesModified() == S_OK)
			return TRG_STAT_Out_Of_Date;
	}

	// do we have an active builder?
    if (hBld == ACTIVE_BUILDER)
	    hBld = GetActiveBuilder();

	if (hTarg == NO_TARGET)
        hTarg = GetActiveTarget(hBld);

	CProject * pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return TRG_STAT_Unknown; // there is no active project

	// not a valid call for EXE-only builders
  	ASSERT(!pProject->m_bProjIsExe);

	// get the target item 
 	CTargetItem * pTarget = CnvHTarget(hBld, hTarg);

	ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);

	// get our project type
	CProjType * pProjType;

	CProjTempConfigChange projTempConfigChange(pProject);
	projTempConfigChange.ChangeConfig(pcr);
	pProjType = pTarget->GetProjType();
	projTempConfigChange.Reset();

	if (pProjType == (CProjType *)NULL)
		return TRG_STAT_Unknown;

	// if the current target is an external target then 
	// assume that we're current
	if (pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
		return TRG_STAT_Current;

	// file registry handle to the target
	FileRegHandle frhTarget = NULL;

	// need to find the primary target?
	if (pchTarget == (const TCHAR *)NULL)
	{
#if 0
		CFileRegSet * psetTarg = (CFileRegSet *)pcr->GetTargetFiles();
		
		// return a (const CPath *)NULL if there are no primary targets
		if (psetTarg == (const CFileRegSet *)NULL)
			return TRG_STAT_Unknown;
		
#ifndef REFCOUNT_WORK
		frhTarget = (FileRegHandle)psetTarg->GetContent()->GetHead(); 
#else
		frhTarget = psetTarg->GetFirstFrh(); 
#endif

		delete psetTarg;
#endif
	}
	else
	{
		// lookup this target
		CFileRegistry * pregistry = g_buildengine.GetRegistry(pcr);
		ASSERT(pregistry != (CFileRegistry *)NULL);

		if ((frhTarget = pregistry->LookupFile(pchTarget)) == (FileRegHandle)NULL)
			return TRG_STAT_Unknown;
	}

	CPtrList lstprojdeprefs;

	projTempConfigChange.ChangeConfig(pcr);

	// munge dep graph to take subprojects into account
	HFILESET hFileSet = GetFileSet(hBld, hTarg);
	POSITION posDep;
	CObList  lstDep;
	InitProjectDepEnum( hFileSet, hBld, lstDep, posDep );

	HPROJDEP hProjDep;
	while ((hProjDep = GetNextProjectDep( hFileSet, hBld, lstDep, posDep )) != NO_PROJDEP)
	{
 		CProjectDependency * pProjDep = CnvHProjDep(hFileSet, hProjDep);
		ConfigurationRecord * pcrProjDep = pProjDep->GetTargetConfig();

		if (pcrProjDep != NULL)
		{
            PROJDEPREFINFO * pProjDepRefInfo = new PROJDEPREFINFO;
            pProjDepRefInfo->pProjDep = pProjDep;
            pProjDepRefInfo->pcr = pcr;

            lstprojdeprefs.AddTail(pProjDepRefInfo);

            CActionSlob::AddRefFileItem(pProjDep);
		}
	}

	projTempConfigChange.Reset();

	UINT nRetVal = TRG_STAT_Current;

	// are we up to date?
	CPtrList lstFrh;
	if (g_buildengine.HasBuildState(pcr, lstFrh, DS_OutOfDate, frhTarget))
	{
		// place out pointer to paths in the list?
		if (plstPath != (CObList *)NULL)
		{
			POSITION pos = lstFrh.GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				FileRegHandle frh = (FileRegHandle)lstFrh.GetNext(pos);
				plstPath->AddTail((CObject *)g_FileRegistry.GetRegEntry(frh)->GetFilePath());
			}
		}

		nRetVal = TRG_STAT_Out_Of_Date;
		goto ExitTargetState;
	}

	// don't check our target dependencies for a specific target
	if (pchTarget == (const TCHAR *)NULL)
	{
		// initial the target dep. enum
		CProjTempConfigChange projTempConfigChange(pProject);
		projTempConfigChange.ChangeConfig(pcr);

		HFILESET hFileSet = GetFileSet(hBld, hTarg);
		InitProjectDepEnum(hFileSet, hBld, lstDep, posDep);

		HPROJDEP hProjDep;
		HBLDTARGET hTargDep;
		while ((hProjDep = GetNextProjectDep(hFileSet, hBld, lstDep, posDep)) != NO_PROJDEP)
		{
			HFILESET hDepFileSet = GetDependantProject(hProjDep, hFileSet, hBld);
			if(hDepFileSet == NULL ) continue;

			HBUILDER hDepBld = GetBuilderFromFileSet(hDepFileSet);
			if (GetTargetFromFileSet(hDepFileSet, hTargDep, hBld, TRUE))
			{
				UINT state = TargetState(plstPath, (const TCHAR *)NULL, hTargDep, hDepBld);
				if (state != TRG_STAT_Current){
					nRetVal = state;
				    goto ExitTargetState;
				}
			}
		}
		
	}

ExitTargetState:

	projTempConfigChange.ChangeConfig(pcr);

	while (!lstprojdeprefs.IsEmpty())
	{
		PROJDEPREFINFO * pProjDepRefInfo = (PROJDEPREFINFO *)lstprojdeprefs.RemoveHead();
		CActionSlob::ReleaseRefFileItem(pProjDepRefInfo->pProjDep);
 	}

	projTempConfigChange.Release();

#ifdef REFCOUNT_WORK
	if (NULL!=frhTarget)
		frhTarget->ReleaseFRHRef();
#endif

	return nRetVal;	// we're current!
}

UINT CBldSysIFace::FileState
(
	const CPath * pathFile				// path of the file to get state for
)
{
	ASSERT(FALSE);
	return TRG_STAT_Unknown;	// unknown state
}

// Given a path to a source file, 'pathSrc', and a list of include directives, 'strlstIncs',
// this function will return a list of paths, 'lstpath', that represent the 'resolved' (or
// absolute) location of the file represented by each include directive.
// Any standard includes, eg. #include <stdio.h>, will be ignored if 'fIgnStdIncs' is TRUE.
// If per-file include directives, eg. compiler /I, exist for the 'pathSrc' then these will be used.
//
// Note: It is up to the caller of this function to deallocate the CPath *'s in 'lstPath'.
//
// Returns FALSE if the operation cannot be performed.
BOOL CBldSysIFace::ResolveIncludeDirectives
(
	const CPath & pathSrc,
	const CStringList & strlstIncs,
	CStringList & lstPath,
	BOOL fIgnStdIncs,
	const CString & strPlat
)
{
	CString strIncDirs;	// our include directories
	static CPath pathSrcLast;
	static fSetLastPath = FALSE;
	
	// remeber last C style file we scan, assuming the parser will call us per CFile order
	// such as C file, then all the includes belong to this C file.
	// The reason we need to do this, is because include file are not in our file set, we can't get per file
	// settings for them, another way is that parser to pass us the source .cpp/c name too...
	if (_tcsicmp(pathSrc.GetExtension(),_TEXT(".cpp"))==0 || _tcsicmp(pathSrc.GetExtension(),_TEXT(".c")) == 0)
	{
		pathSrcLast = pathSrc;
		fSetLastPath = TRUE;
		if (!GetIncludePathI(strIncDirs, strPlat, &pathSrc))
			return FALSE;
	}
	else if (fSetLastPath && (_tcsicmp(pathSrcLast.GetExtension(),_TEXT(".cpp"))==0 || _tcsicmp(pathSrcLast.GetExtension(),_TEXT(".c")) == 0))
	{
		// non C file, h file and last C style file set
		if (!GetIncludePathI(strIncDirs, strPlat, &pathSrcLast))
			return FALSE;	// failed to get include directories
	}
	else
	{
		if (!GetIncludePathI(strIncDirs, strPlat, &pathSrc))
			return FALSE;
	}
		
	// our base directory
	CDir dirBase;
	if (!dirBase.CreateFromPath(pathSrc))
		return FALSE;	// failed to get the base directory for the source

	return ResolveIncludeDirectivesI(strIncDirs, (const CDir &)dirBase, strlstIncs, lstPath, fIgnStdIncs, strPlat, &pathSrc);
	/*
	ResolveIncludeDirectivesI(lstIncDirs, (const CDir &)dirBase, strlstIncs, lstPath, fIgnStdIncs, strPlat, &pathSrc);

	CStringList *pstrlstIncs;

	pstrlstIncs = new CStringList;
	POSITION pos = lstPath.GetHeadPosition();		
	while (pos != (POSITION)NULL)
	{
		CString str;
		CPath * pPath = (CPath *)lstPath.GetNext(pos);
		if (pPath != (CPath *)NULL)
		{
			str = pPath->GetFullPath();
			pstrlstIncs->AddTail(str);
		}

	}

	UpdateDependencyList(pathSrc, (CStringList &)*pstrlstIncs, GetActiveTarget(), GetActiveBuilder());

	return TRUE;
	*/
}

// As above, but given a directory to initially search, 'dir'.
BOOL CBldSysIFace::ResolveIncludeDirectives
(
	const CDir & dirBase,
	const CStringList & strlstIncs,
	CStringList & lstPath,
	BOOL fIgnStdIncs,
	const CString & strPlat
)
{
	CString strIncDirs;	// our include directories
	if (!GetIncludePathI(strIncDirs, strPlat))
		return FALSE;	// failed to get include directories

	return ResolveIncludeDirectivesI(strIncDirs, (const CDir &)dirBase, strlstIncs, lstPath, fIgnStdIncs, strPlat, (const CPath *)NULL);
}

// Given a list path to a source file, 'pathSrc', and a list of include directives, 'strlstIncs',
// this function will return a list of paths, 'lstpath', that represent the 'resolved' (or
// absolute) location of the file represented by each include directive.
// Any standard includes, eg. #include <stdio.h>, will be ignored if 'fIgnStdIncs' is TRUE.
// If per-file include directives, eg. compiler /I, exist for the 'pathSrc' then these will be used.
//
// Note: It is up to the caller of this function to deallocate the CPath *'s in 'lstPath'.
//
// Returns FALSE if the operation cannot be performed.
BOOL CBldSysIFace::ResolveIncludeDirectives
(
	const CObList & lstpathSrc,
	const CStringList & strlstIncs,
	CStringList & lstPath,
	HBLDTARGET hTarget
	)
{
	if (NULL==g_pActiveProject)
		return FALSE;

	POSITION posPath = lstpathSrc.GetHeadPosition();		
	ASSERT (posPath != (POSITION)NULL);
	CPath * pathSrc = (CPath *)lstpathSrc.GetNext(posPath);

	CString strlstIncDirs;	// our include directories
	CDirMgr * pDirMgr = GetDirMgr();
	ASSERT (pDirMgr != (CDirMgr *)NULL);
	pDirMgr->GetDirListString (strlstIncDirs, DIRLIST_INC, -1);

	CProject * pProject = g_pActiveProject;

	ASSERT (pProject);
	CString strIncls;
	pProject->m_listTargIncl.Lookup(hTarget, strIncls);
	strlstIncDirs = strIncls + _T(';') + strlstIncDirs;

	// our base directory
	CDir dirBase;
	if (!dirBase.CreateFromPath(*pathSrc))
		return FALSE;	// failed to get the base directory for the source

	// get the current project
	HBUILDER hBld = GetActiveBuilder();
	CProject * pBuilder = hBld != NO_BUILDER ? CnvHBuilder(hBld) : (CProject *)NULL;

	// include directive information
	CIncludeEntry entry;
	entry.m_OriginalDir = dirBase;
	entry.m_nLineNumber = 0;			// N/A 
	entry.m_bShouldBeScanned = FALSE;	// N/A (not to be scanned)

	CString strInclude, strFile;

	POSITION posInc = strlstIncs.GetHeadPosition();
	while (posInc != (POSITION)NULL)
	{
		// get the include directive,
		// this is of the forms "file" or <file>
		strInclude = strlstIncs.GetNext(posInc);

		int cchInclude = strInclude.GetLength();
		TCHAR * pchInclude = strInclude.GetBuffer(cchInclude + 1);

		if (*pchInclude != _T('<') && *pchInclude != _T('\"'))
		{
			ASSERT(FALSE);
			return FALSE;
		}

		// standard include?
		entry.m_EntryType = (*pchInclude == _T('<')) ?
							IncTypeCheckIncludePath : 
							IncTypeCheckIncludePath | IncTypeCheckOriginalDir | IncTypeCheckParentDir;

		// strip the leading and trailing characters from 'strInclude'
		TCHAR * pchFile = strFile.GetBuffer(cchInclude - 1);
		strncpy(pchFile, pchInclude + 1, cchInclude - 2);
		pchFile[cchInclude - 2] = _T('\0');
		strFile.ReleaseBuffer();

		BOOL fIgnore = TRUE;	// by default ignore

		// is this a system include? ignore?
		if (!g_SysInclReg.IsSysInclude(strFile))
		{
			entry.m_FileReference = strFile;	// the file
			entry.CheckAbsolute();				// check the absoluteness of scanned dep.

			FileRegHandle frhFile;	// the 'resolved' file

			// build directory to make relative to?
			const TCHAR * pchBldDir = pBuilder != (CProject *)NULL ?
									  (const TCHAR *)pBuilder->GetWorkspaceDir() : _TEXT("");

			posPath = lstpathSrc.GetHeadPosition();
			while (posPath != (POSITION)NULL)
			{
				CPath * pPath = (CPath *)lstpathSrc.GetNext(posPath);

				// our base directory
				CDir dirParent;
				if (!dirParent.CreateFromPath(*pPath))
					return FALSE;
				if (entry.FindFile((const TCHAR *)strlstIncDirs, dirParent, pchBldDir, frhFile))
				{	
					CFileRegEntry * pfilereg = g_FileRegistry.GetRegEntry(frhFile);
					ASSERT(pfilereg != (CFileRegEntry *)NULL);

					// add path to our list
					lstPath.AddTail((CString)*pfilereg->GetFilePath());
#ifdef REFCOUNT_WORK
					frhFile->ReleaseFRHRef();
#endif
				  
			 		fIgnore = FALSE;	// don't ignore 
					break;
				}
			}

			// file not found
			if (fIgnore)
			{
				if (frhFile != (FileRegHandle)NULL)
				{
					CFileRegEntry * pfilereg = g_FileRegistry.GetRegEntry(frhFile);
					ASSERT(pfilereg != (CFileRegEntry *)NULL);

					// add path to our list
					lstPath.AddTail((CString)*pfilereg->GetFilePath());
#ifdef REFCOUNT_WORK
					frhFile->ReleaseFRHRef();
#endif
				}
			}
		}
 	}

	return TRUE;	// success
}


// Internal core for CBldSysIFace::ResolveIncludeDirectives() methods.
BOOL CBldSysIFace::ResolveIncludeDirectivesI
(
	CString & strlstIncDirs,
	const CDir & dirBase,
	const CStringList & strlstIncs,
	CStringList & lstPath,
	BOOL fIgnStdIncs,
	const CString & strPlat,
	const CPath *pathSrc
)
{
	HBUILDER hBld = GetActiveBuilder();
	CProject * pBuilder = hBld != NO_BUILDER ? CnvHBuilder(hBld) : (CProject *)NULL;

	// FUTURE: use this information to update our own cache

	// include directive information
	CIncludeEntry entry;
	entry.m_OriginalDir = dirBase;
	entry.m_nLineNumber = 0;			// N/A 
	entry.m_bShouldBeScanned = FALSE;	// N/A (not to be scanned)

	CString strInclude, strFile;

	POSITION posInc = strlstIncs.GetHeadPosition();
	while (posInc != (POSITION)NULL)
	{
		// get the include directive,
		// this is of the forms "file" or <file>
		strInclude = strlstIncs.GetNext(posInc);

		int cchInclude = strInclude.GetLength();
		TCHAR * pchInclude = strInclude.GetBuffer(cchInclude + 1);

		if (*pchInclude != _T('<') && *pchInclude != _T('\"') && *pchInclude != _T('\''))
		{
            // [paulde] change from ASSERT to TRACE because some callers have a 
            // legitimate need to pass names that don't begin with these chars,
            // and for those that care, you at least get a trace message.
			TRACE("CBldSysIFace::ResolveIncludeDirectivesI: file does not begin with [<\"']\n");
			return FALSE;
		}

		// standard include?
		entry.m_EntryType = (*pchInclude == _T('<')) ?
							IncTypeCheckIncludePath : 
							IncTypeCheckIncludePath | IncTypeCheckOriginalDir;

		// strip the leading and trailing characters from 'strInclude'
		TCHAR * pchFile = strFile.GetBuffer(cchInclude - 1);
		strncpy(pchFile, pchInclude + 1, cchInclude - 2);
		pchFile[cchInclude - 2] = _T('\0');
		strFile.ReleaseBuffer();

		BOOL fIgnore = TRUE;	// by default ignore

		// is this a system include? ignore?
		if (!(fIgnStdIncs && g_SysInclReg.IsSysInclude(strFile)))
		{
			entry.m_FileReference = strFile;	// the file
			entry.CheckAbsolute();				// check the absoluteness of scanned dep.

			FileRegHandle frhFile = NULL;	// the 'resolved' file

			// build directory to make relative to?
			const TCHAR * pchBldDir = pBuilder != (CProject *)NULL ?
									  (const TCHAR *)pBuilder->GetWorkspaceDir() : _TEXT("");

			if (entry.FindFile((const TCHAR *)strlstIncDirs, dirBase, pchBldDir, frhFile))
			{	
				CFileRegEntry * pfilereg = g_FileRegistry.GetRegEntry(frhFile);
				ASSERT(pfilereg != (CFileRegEntry *)NULL);

				// add path to our list
				lstPath.AddTail((CString)*pfilereg->GetFilePath());
#ifdef REFCOUNT_WORK
					frhFile->ReleaseFRHRef();
#endif
				  
			 	fIgnore = FALSE;	// don't ignore 
			}
			else
			{
				// otherwise, remember the headers we didn't find, so they can still be on the dep list
				if (pathSrc != (CPath *)NULL)
				{
					CPtrList *pPtrList;

					// get extension, we only record done the missing headers for C files
					// this is kind of incomplete, but it should cover the most common cases and
					// don't think the complete check will worth the work
					TCHAR * pch = _tcschr((TCHAR *)pathSrc->GetFullPath(), '.');
					if ((frhFile != (FileRegHandle)NULL) && pch && (_tcsicmp(pch, ".cpp")==0 || _tcsicmp(pch, ".cxx")==0 || _tcsicmp(pch, ".c")==0 ))					
					{
						if (pUnknownDepMap == NULL)
						{
							// allocate the default map
							pUnknownDepMap = new CMapStringToOb;
						}
						// add/create the mapped list of unknown deps
						if (pUnknownDepMap->Lookup((LPCTSTR)pathSrc->GetFullPath(), (CObject *&)pPtrList))
						{
							pPtrList->AddTail((void *)frhFile);
						}
						else
						{
							pPtrList = new CPtrList;
							pUnknownDepMap->SetAt((LPCTSTR)pathSrc->GetFullPath(), (CObject *)pPtrList);
							pPtrList->AddTail((void *)frhFile);
						}
					}
				}
#ifdef REFCOUNT_WORK
				if (NULL!=frhFile)
					frhFile->ReleaseFRHRef();
#endif
			}
			// release this ref. to the file reg-handle?
			// don't do this currently while somebody needs the *pPath ptrs!
			// could go back to new'ing a copy ourselves ....
			// g_FileRegistry.ReleaseRef(frhFile);
  		}
		
		// tag as ignored?
//		if (fIgnore)
			// ignore this one
//			lstPath.AddTail((CPath *)NULL);
 	}

	return TRUE;	// success
}



// Return the full search path for includes for a file.
// FUTURE: Use per-file include paths, eg. as specified by /i for the linker
BOOL CBldSysIFace::GetIncludePath
(
	CObList & lstIncDirs,
	const CString & strPlat,
	const CPath * ppathSrc,
	const CString & strTool,
	HBUILDER hBld
)
{
	BOOL fGotPerFile = FALSE;

	// make all of these relative to the builder directory
	CProject * pBuilder = CnvHBuilder(hBld);
				
	if (ppathSrc != (const CPath *)NULL && pBuilder != (CProject *)NULL)
	{
		// active fileset?
		HFILESET hFileSet;
		CBuildTool * pBuildTool = NULL;
		if (!strPlat.IsEmpty())
		{
			HBLDTARGET hTarget = GetTarget(strPlat, hBld);
			hFileSet = GetFileSet(hBld, hTarget);

		}
		else
		{
			hFileSet = (HFILESET)pBuilder->GetActiveTarget();
		}

		HBLDFILE hFile;
		CProjItem * pItem = NULL;

		if (hFileSet && GetFile(ppathSrc, hFile, hFileSet, hBld))
		{
			// get the target file CProjItem
			pItem = CnvHBldFile(hFileSet, hFile);
			ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CFileItem)));
			
			CProjTempConfigChange projTempConfigChange(pItem->GetProject());

			ConfigurationRecord * pcr;
			if (!strPlat.IsEmpty())
			{
				// get this target config record
				pcr = pItem->ConfigRecordFromConfigName((const TCHAR *)strPlat, TRUE);
				ASSERT(pcr != (ConfigurationRecord *)NULL);

				// force this item into our config.
				projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	
			}
			pBuildTool = pItem->GetSourceTool();
		}
		else if ( !strTool.IsEmpty() ) {
			// get the include directories for this tool
			CProjType *pProjType = pBuilder->GetProjType();
			CPtrList * pTools = pProjType->GetToolsList();
			POSITION pos = pTools->GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				CBuildTool * pTool = (CBuildTool *)pTools->GetNext(pos);
				CString strTargTool;
	
				if (pTool->GetStrProp(P_ToolExeName, strTargTool) == valid &&
					_tcsicmp((const TCHAR *)strTargTool, (const TCHAR *)strTool) == 0
				   )
				{
					pBuildTool = pTool;
					pItem = pBuilder;
					break;
				}
			}
		}

		// get the include directories for this tool
		if (pBuildTool != (CBuildTool *)NULL)
		{
			CString strIncDirs;
			pBuildTool->GetIncludePaths(pItem, strIncDirs);

			// allow ',;' as join chars.
			COptionList strlstIncDirs(_T(';'), TRUE);
			strlstIncDirs.SetString(strIncDirs);

			// change to the builder directory
			CCurDir curDir(pBuilder->GetWorkspaceDir());

			POSITION pos = strlstIncDirs.GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				CDir * pDir = new CDir;
				if (pDir->CreateFromString(strlstIncDirs.GetNext(pos)))
					// caller must de-allocate these
					lstIncDirs.AddTail(pDir);
			}
			 
			fGotPerFile = TRUE;
		}
	}

	// got per-file includes?
	if (!fGotPerFile)
	{
		CDirMgr * pDirMgr = GetDirMgr();
		ASSERT(pDirMgr != (CDirMgr *)NULL);

		// get the index of the toolset
		int nToolSet = strPlat.IsEmpty() ? (int)-1 : pDirMgr->GetPlatformToolset(strPlat);

		// get the directory list
		const CObList * plstIncDirs = pDirMgr->GetDirList(DIRLIST_INC, nToolSet);
		
		// construct our list (copy this directory manager one into ours)
		POSITION pos = plstIncDirs->GetHeadPosition();
		while (pos != (POSITION)NULL)
			// caller must de-allocate these
			lstIncDirs.AddTail(new CDir((const CDir &)*plstIncDirs->GetNext(pos)));
	}

	return TRUE;	// success 
}

// Parser will call this function. We need to update the dep list for this file if the dep list
// changed. 
void CBldSysIFace::UpdateDependencyList
(
	const CPath & pathSrc,   // source file
	const CStringList & strlstIncs, //list of includes for this file
	HBLDTARGET		hTarg,	// handle to the target
	HBUILDER		hBld	// builder that contains target? default=('current'==ACTIVE_BUILDER) 
)
{
	//
	// Current not using dependency information from NCB.
	//
#if 0

	CPtrList *pdepList;

	CProject * pBuilder = CnvHBuilder(hBld);

	if (pathSrc != NULL && pBuilder != (CProject *)NULL)
	{
		// active fileset?
		HFILESET hFileSet = GetFileSet(hBld, hTarg);
		HBLDFILE hFile;

		if (hFileSet && GetFile(&pathSrc, hFile, hFileSet, hBld))
		{
			// get the target file CFileItem
			CFileItem * pItem = CnvHBldFile(hFileSet, hFile);	

			// loop through the include list, create a list by file handle, so we don't need to have
			// the list being passed in around anymore
			CString strInclude;
			
			pdepList = new CPtrList;
			POSITION posInc = strlstIncs.GetHeadPosition();
			while (posInc != (POSITION)NULL)
			{
				// This should be the full path
				strInclude = strlstIncs.GetNext(posInc);
				// Get the file reg handle for the file, we should have registed this file before we get here
				FileRegHandle frh = CFileRegFile::LookupFileHandleByName(strInclude);
				ASSERT(frh);

				// Add it to out depList
				pdepList->AddTail((void *)frh);
			}
			// add any unknown dep list
			CPtrList *pUnknownDepList;

			if (pUnknownDepMap && pUnknownDepMap->Lookup((LPCTSTR)pathSrc.GetFullPath(), (CObject *&)pUnknownDepList))
			{
				POSITION posInc = pUnknownDepList->GetHeadPosition();
				while (posInc != (POSITION)NULL)
				{
					FileRegHandle frh = (FileRegHandle) pUnknownDepList->GetNext(posInc);
					ASSERT(frh);

					// Add reference frh copied from pUnknownDepList.
					frh->AddFRHRef();
					// Add it to out depList
					pdepList->AddTail((void *)frh);
				}
				pUnknownDepList->RemoveAll();
				delete pUnknownDepList;
				pUnknownDepMap->RemoveKey((LPCTSTR)pathSrc.GetFullPath());
				if (pUnknownDepMap->IsEmpty())
				{
					delete pUnknownDepMap;
					pUnknownDepMap = NULL;
				}
			}

			// queue it up for update dep later
			FileItemDepUpdateQ(pItem, pdepList, NULL, g_FileDepUpdateListQ, FALSE, hTarg);					
			POSITION posFrh = pdepList->GetHeadPosition();
			while (posFrh!=NULL)
			{
				FileRegHandle frh = (FileRegHandle)pdepList->GetNext(posFrh);
				frh->ReleaseFRHRef();
			}
		}
	}
#endif	// #if 0
}

// Internal helper for CBldSysIFace::AddToolSettings() etc.
BOOL CBldSysIFace::SetToolSettingsI
(
	HBLDTARGET		hTarg,					// handle to the target that contains the file
	CProjItem *		pItem,					// pointer to the builder item
	const TCHAR * 	pchSettings,			// tool settings
	const TCHAR *	pchTool,				// executable of the tool that 'owns' these settings
	BOOL			fAdd,					// add or subtact? default=add
	BOOL			fClear					// clear settings before add or subtract?
)
{
	// get our target name
	CString strTarg = m_strlstTargets.GetAt((POSITION)hTarg);

	// get this target config record
	ConfigurationRecord * pcr = pItem->ConfigRecordFromConfigName((const TCHAR *)strTarg);
	CProjTempProjectChange projChange(pItem->GetProject());

	// force this item into our config.
	CProjTempConfigChange projTempConfigChange(pItem->GetProject());
	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	
	
	// get this item's target type for the config.
	CProjType * pProjType = pItem->GetProjType();
	ASSERT(pProjType != (CProjType *)NULL);

	projTempConfigChange.Reset();

	// get the tool we want
	CBuildTool * pBuildTool = NULL;

	if (pchTool == (const TCHAR *)NULL)
	{
		// we can look for the tool for this source
		if (!pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
			return FALSE;	// can't do unnamed tools for non-source

		pBuildTool = pProjType->PickTool(pItem->GetFileRegHandle());
	}
	else
	{
		// if no .exe then add it (simple canonicalise)
		CString strTool = pchTool, strTargTool;
		if (!strTool.CompareNoCase("mfc")){
			if( pchSettings ){
				int iUseOfMFC = atoi(pchSettings);
				if( iUseOfMFC >= 0 && iUseOfMFC < 3 ){
					SetTargetProp(hTarg, Prop_UseOfMFC, iUseOfMFC, (HBUILDER)pItem->GetProject());
					return TRUE;
				}
			}
			return FALSE;
		} else if(!strTool.CompareNoCase("xbox!remotename")) {
		    /* Make sure we're on an xbox project */
            if(pProjType->GetPlatform()->GetUniqueId() != xbox)
                return FALSE;
		    CProject *pProj = pItem->GetProject();
		    CString strTarg(pchSettings);
		    pProj->SetRemoteTargetFileName(strTarg);
		    return TRUE;
        }

		
		if (strTool.GetLength() < 4 || strTool.Right(4).CompareNoCase(".exe"))
			strTool += ".exe";

		// enumerate the tools for this project type
		// and get matching exe name
		CPtrList * pTools = pProjType->GetToolsList();
		POSITION pos = pTools->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CBuildTool * pTool = (CBuildTool *)pTools->GetNext(pos);

			if (pTool->GetStrProp(P_ToolExeName, strTargTool) == valid &&
				_tcsicmp((const TCHAR *)strTargTool, (const TCHAR *)strTool) == 0
			   )
			{
				pBuildTool = pTool;
				break;
			}
		}
	}

	if (pBuildTool == (CBuildTool *)NULL)
		return FALSE;	// no tool for this item
	
	COptionHandler * popthdlr = pBuildTool->GetOptionHandler();
	if (popthdlr == (COptionHandler *)NULL)
		return FALSE;	// no option handler for this item

	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	
	
	// refer our engine to this item and option handler
	g_prjoptengine.SetOptionHandler(popthdlr);
	g_prjoptengine.SetPropertyBag(pItem);

	// get our settings string
	CString strSettings;
	BOOL fSet = TRUE;

	if (!fAdd)
	{
		(void) g_prjoptengine.GenerateString(strSettings, OBNone | OBShowDefault | OBNeedEmptyString /* NOT (OBShowFake | ONInherit) */);

		// remove these settings from the current set
		int iSubStr = strSettings.Find(pchSettings);
		if (iSubStr != -1)
		{
			// remove this 'piece' of the string
			CString strWork;
			strWork = strSettings.Left(iSubStr);
			strWork += strSettings.Right(strSettings.GetLength() - iSubStr - _tcslen(pchSettings));

			// set the new string
			strSettings = strWork;
		}
		else
		{
			// not found, don't bother to set these
			fSet = FALSE;
		}
	}
	else
	{
		// just set these
		strSettings = pchSettings;
	}

	// parse the options?
	if (fSet)
	{
		// clear if explicit or if subtract
		OptBehaviour optbeh = OBNone | OBNeedEmptyString;
		if (fClear || !fAdd) optbeh |= OBClear;

		g_prjoptengine.ParseString(strSettings, optbeh);
	}

	return TRUE;	// success
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

BOOL CBldSysIFace::MapPropToPropI
(
	UINT			idProp,						// external property number
	UINT &			idPropI						// internal (private) property number
)
{
	if ((idProp + 1) > (sizeof(mapPropToPropI) / sizeof(UINT)))
	{
		ASSERT(FALSE);	// alert failure in debug version
		return FALSE;	// failed
	}

	// perform the mapping
	idPropI = mapPropToPropI[idProp];

	// is this property valid
	return TRUE;	// ok
}

// Get the config. record for a file, 'hFile', in a given target, 'hTarg'.
ConfigurationRecord * CBldSysIFace::GetConfigRecFromFileI
(
	HBLDTARGET		hTarg,	// handle to the target
 	HBLDFILE		hFile	// handle to the file to get the config. for
)
{
	// get the target file CProjItem
	CFileItem * pItem = CnvHBldFile(GetFileSet(ACTIVE_BUILDER, hTarg), hFile);
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CFileItem)));

	// get our target name
	CString strTarg = m_strlstTargets.GetAt((POSITION)hTarg);

	// get this target config record (create it if it doesn't exist)
	return pItem->ConfigRecordFromConfigName((const TCHAR *)strTarg, TRUE);
}

// Get the config. record for a target, 'hTarg'
ConfigurationRecord * CBldSysIFace::GetConfigRecFromTargetI
(
	HBLDTARGET		hTarg,	// handle to the target
	HBUILDER		hBld	// builder that contains target? default=('current'==ACTIVE_BUILDER) 
)
{
	// get the target file CProject
	CProject * pItem = CnvHBuilder(hBld);
	if( pItem == NULL )
		return NULL;
	
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CProject)));

	// get our target name
	CString strTarg = m_strlstTargets.GetAt((POSITION)hTarg);

	// get this target config record
	return pItem->ConfigRecordFromConfigName((const TCHAR *)strTarg);
}

// Set an integer or string property for this file
BOOL CBldSysIFace::SetFileProp
(
	HBLDTARGET		hTarg,					// handle to the target that contains the file
	HBLDFILE		hFile,					// handle to the file
	UINT			idProp,					// identifier of the property
	int				i						// property value
)
{
	UINT idPropI;
	if (!MapPropToPropI(idProp, idPropI))
		return FALSE;

	// get this file's config record
	ConfigurationRecord * pcr = GetConfigRecFromFileI(hTarg, hFile);

	CFileItem * pItem = CnvHBldFile(GetFileSet(ACTIVE_BUILDER, hTarg), hFile);
	CProjTempProjectChange projChange(pItem->GetProject());

	// force this item into our config.
	CProjTempConfigChange projTempConfigChange(pItem->GetProject());
	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	

	// set integer property
	BOOL fRet = pItem->SetIntProp(idPropI, i);
	if (fRet)
		pcr->BagCopy(CurrBag, BaseBag, idPropI, idPropI);
	

	return fRet;	// ok?
}

BOOL CBldSysIFace::SetFileProp
(
	HBLDTARGET		hTarg,					// handle to the target that contains the file
	HBLDFILE		hFile,					// handle to the file
	UINT			idProp,					// identifier of the property
	CString &		str						// property value
)
{
	UINT idPropI;
	if (!MapPropToPropI(idProp, idPropI))
		return FALSE;

	// get this file's config record
	ConfigurationRecord * pcr = GetConfigRecFromFileI(hTarg, hFile);

	CFileItem * pItem = CnvHBldFile(GetFileSet(ACTIVE_BUILDER, hTarg), hFile);

	// force this item into our config.
	CProjTempConfigChange projTempConfigChange(pItem->GetProject());
	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	

	// set string property
	BOOL fRet = pItem->SetStrProp(idPropI, str);
	if (fRet)
		pcr->BagCopy(CurrBag, BaseBag, idPropI, idPropI);
	

	return fRet;	// ok?
}
   
void CBldSysIFace::MakeFilePropsDefault
(
	HBLDTARGET		hTarg,					// handle to the target that contains the file
	HBLDFILE		hFile					// handle to the file
)
{
	// get this file's config record
	ConfigurationRecord * pcr = GetConfigRecFromFileI(hTarg, hFile);

	CFileItem * pItem = CnvHBldFile(GetFileSet(ACTIVE_BUILDER, hTarg), hFile);

	// force this item into our config., make default current, then reset config.
	pItem->ForceConfigActive(pcr);	
	pItem->SetCurrentConfigAsBase();
	pItem->ForceConfigActive();
}

// Enumerate projects
void CBldSysIFace::InitBuilderEnum()
{
	CProject::InitProjectEnum();
}

HBUILDER CBldSysIFace::GetNextBuilder(CString & strBuilder, BOOL bOnlyLoaded /* = TRUE */)
{
	return (HBUILDER)(CProject::NextProjectEnum(strBuilder, bOnlyLoaded));
}

// Enumerate targets
void CBldSysIFace::InitTargetEnum(HBUILDER hBld)
{
	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);
	if (pProject == (CProject *)NULL)
		return;

	pProject->InitTargetEnum();
}

HBLDTARGET CBldSysIFace::GetNextTarget(CString & strTarget, HBUILDER hBld)
{
	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);
	if (pProject == (CProject *)NULL)
		return NO_TARGET;

	CTargetItem * pTargetItem;
	if (!pProject->NextTargetEnum(strTarget, pTargetItem))
		return NO_TARGET;

	return GetTarget(strTarget, hBld);
}

void CBldSysIFace::InitProjectDepEnum(HFILESET hFileSet, HBUILDER hBld /* = ACTIVE_BUILDER */)
{
	// FUTURE: extend to allow enum of multiple target dependencies
	// cache
	m_lstDepSet.RemoveAll();
	m_posHDepSet = (POSITION)NULL;

	int fo = CProjItem::flt_OnlyTargRefs;

	CTargetItem *pTarg = CnvHFileSet(hBld, hFileSet);
	ASSERT(pTarg != NULL);
	pTarg->FlattenSubtree(m_lstDepSet, fo);

	// get start of enumeration
	m_posHDepSet = m_lstDepSet.GetHeadPosition();
}

HPROJDEP CBldSysIFace::GetNextProjectDep(HFILESET hFileSet, HBUILDER hBld /* = ACTIVE_BUILDER */)
{
	if (m_posHDepSet == (POSITION)NULL)
		return NO_PROJDEP;

 	CProjectDependency * pProjDep = (CProjectDependency *)m_lstDepSet.GetNext(m_posHDepSet);
 
	return (HPROJDEP)pProjDep;
}

void CBldSysIFace::InitProjectDepEnum(HFILESET hFileSet, HBUILDER hBld, CObList &lstDepSet, POSITION &posHDepSet )
{
	// FUTURE: extend to allow enum of multiple target dependencies
	// cache
	lstDepSet.RemoveAll();
	posHDepSet = (POSITION)NULL;

	int fo = CProjItem::flt_OnlyTargRefs;

	CTargetItem *pTarg = CnvHFileSet(hBld, hFileSet);
	ASSERT(pTarg != NULL);
	pTarg->FlattenSubtree(lstDepSet, fo);

	// get start of enumeration
	posHDepSet = lstDepSet.GetHeadPosition();
}

HPROJDEP CBldSysIFace::GetNextProjectDep(HFILESET hFileSet, HBUILDER hBld, CObList &lstDepSet, POSITION &posHDepSet )
{

	if (posHDepSet == (POSITION)NULL)
		return NO_PROJDEP;

 	CProjectDependency * pProjDep = (CProjectDependency *)lstDepSet.GetNext(posHDepSet);
 
	return (HPROJDEP)pProjDep;
}

DWORD CBldSysIFace::GetDepth(HBLDTARGET){
	return 0;
}

// Enumerate file sets
void CBldSysIFace::InitFileSetEnum(HBUILDER hBld)
{
	// FUTURE: extend to allow enum of multiple builder filesets
	// cache
	m_lstFileSet.RemoveAll();
	m_posHFileSet = (POSITION)NULL;

	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);
	if (pProject == (CProject *)NULL)
		return;

	// get our list to enumerate
	pProject->InitTargetEnum();
	CString strDummy; CTargetItem * pTarget;
	while (pProject->NextTargetEnum(strDummy, pTarget))
	{
		// make sure we only get distinct filesets
		if (m_lstFileSet.Find(pTarget) == (POSITION)NULL)
			m_lstFileSet.AddTail(pTarget);
	}

	// get start of enumeration
	m_posHFileSet = m_lstFileSet.GetHeadPosition();
}

HFILESET CBldSysIFace::GetNextFileSet(HBUILDER hBld)
{
	if (m_posHFileSet == (POSITION)NULL)
		return NO_FILESET;

	return (HFILESET)m_lstFileSet.GetNext(m_posHFileSet);
}

// Enumerate files
void CBldSysIFace::InitFileEnum(HFILESET hFileSet, UINT filter /* = FileEnum_RemoveDeps */)
{
	// FUTURE: extend to allow enum of multiple fileset files
	// cache
	m_lstFile.RemoveAll();
	m_posHFile = (POSITION)NULL;

	// get our list to enumerate
	int fo = 0;
	if (filter & FileEnum_RemoveExcluded)
		fo |= CProjItem::flt_RespectItemExclude;

	if (filter & FileEnum_RemoveDeps) // default is no deps
		fo |= CProjItem::flt_ExcludeDependencies;

	fo |= (CProjItem::flt_ExcludeProjDeps | CProjItem::flt_ExcludeGroups);	// remove project deps
	
	CnvHFileSet(NO_BUILDER, hFileSet)->FlattenSubtree(m_lstFile, fo);

	// get start of enumeration
	m_posHFile = m_lstFile.GetHeadPosition();
}

HBLDFILE CBldSysIFace::GetNextFile(FileRegHandle & frh, HFILESET hFileSet)
{
	if (m_posHFile == (POSITION)NULL)
		return (HBLDFILE)NO_FILE;

	CFileItem * pItem = (CFileItem *)m_lstFile.GetNext(m_posHFile);
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CFileItem)));

	// get the file's file registry handle
	frh = pItem->GetFileRegHandle();
	return (HBLDFILE)pItem;
}												
BOOL CBldSysIFace::IsScanableFile(HBLDFILE hFile)
{
	if (hFile == (HBLDFILE)NO_FILE)
		return FALSE;

	int val;
	CProjItem * pItem = (CProjItem *)hFile;
	if (!pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		return FALSE;

	if ((pItem->GetIntProp(P_ItemExcludedFromScan, val)==valid) && (val!=0))
		return FALSE;

	return TRUE;
}

// Get a string list of possible target names for a file set
BOOL CBldSysIFace::GetFileSetTargets
(
	HFILESET		hFileSet,			// the file set to retrieve the target list for
	CStringList &	strlstTargs			// list of target names
)
{
	ASSERT(hFileSet != NO_FILESET);

	// make sure we have an empty string list
	strlstTargs.RemoveAll();

	const CPtrArray * paryConfigs = CnvHFileSet(ACTIVE_BUILDER, hFileSet)->GetConfigArray();
	int i = 0, iLast = paryConfigs->GetUpperBound();

	while (i <= iLast)
	{
		ConfigurationRecord * pcr = (ConfigurationRecord *) paryConfigs->GetAt(i++);	// get this one and advance

		// add the name if our target to our list if this target is valid
		if (pcr->IsValid())
			strlstTargs.AddTail(pcr->GetConfigurationName());
	}

	return TRUE;	// ok
}

// Enumerate files

// Get a string list of possible platform names for a target type (return *all* platforms when 'pchPlat' is NULL)
const CStringList * CBldSysIFace::GetAvailablePlatforms
(
	const TCHAR *	pchPlat,			// UI or 'official' platform name? default=retrieve across *all* platforms
	BOOL			fUIDesc				// return UI or 'official' names? default=UI
)
{
	return g_prjcompmgr.GetListOfPlatforms(pchPlat, fUIDesc);
}

// Get a string list of possible target type names (across *all* installed platforms).
const CStringList * CBldSysIFace::GetTargetTypeNames
(								   	
	const TCHAR *	pchTarg,		 	// UI or 'official' target name? default=retrieve all platforms
	BOOL			fUIDesc				// return UI or 'official' names? default=UI
)
{
	return g_prjcompmgr.GetListOfTypes(pchTarg, fUIDesc);
}

//
// Builder file section support
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
		pbldsect->m_hBld = GetActiveBuilder();
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
	HBUILDER hBld;

	hBld = pbldsect->m_hBld;
	if (hBld == NULL)
	{
		hBld = GetActiveBuilder();
	}
	strKey.Format("%d:%s", hBld, pbldsect->m_strName);

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

CBldrSection * CBldSysIFace::FindSection(const TCHAR * pchName, HBUILDER hBld /*  = ACTIVE_BUILDER */)
{
	CBldrSection * pbldsect;
	CString strKey;

	if (hBld == ACTIVE_BUILDER)
	{
		hBld = GetActiveBuilder();
	}
	strKey.Format("%d:%s", hBld, pchName);

	if (!m_mapSections.Lookup(strKey, (void * &)pbldsect))
	{
		hBld = ACTIVE_BUILDER; // failed. Look up "global" build section (created before projects). BUG Orion 20232
		strKey.Format("%d:%s", hBld, pchName);
		if (!m_mapSections.Lookup(strKey, (void * &)pbldsect))
			pbldsect = (CBldrSection *)NULL;

	}

	return pbldsect;
}


//
// Builder fileset query support
//

// Resource files
BOOL CBldSysIFace::GetResourceFileList
(
	CPtrList &		listRcPath,		// returned list of CPath *'s
	BOOL			fOnlyBuildable,	// only buildable ones?
	HFILESET		hFileSet,		// handle to the fileset (FUTURE: only does active)
	HBUILDER		hBld			// build that contains target? default='active builder'
)
{
	CProject * pProject = CnvHBuilder(hBld);

	if (pProject == NULL)
		return FALSE;

    if (pProject->IsExeProject())
    {
        // For exe projects we just return the path of the exe.
        listRcPath.AddTail((void *)pProject->GetFilePath());
        return TRUE;
    }

	ConfigurationRecord * pcr = (ConfigurationRecord *)NULL;

	// Convert the fileset to a target
	if (hFileSet != NULL)
	{
		CTargetItem * pTarget = CnvHFileSet(hBld, hFileSet);
		pProject = pTarget->GetProject();

		// Get the target items config array
		const CPtrArray* pConfigArray = pTarget->GetConfigArray();
		int nSize = pConfigArray->GetSize();

		// Look for the first valid config for this target item
		for (int i = 0; i < nSize; i++)
		{
			pcr = (ConfigurationRecord*)pConfigArray->GetAt(i);
			if (pcr->IsValid())
 				break;
		}

		if (pcr == NULL)
			return FALSE;
	}

	// get our list of resource CFileItems
	CObList lstFileItems;
	if (!pProject->GetResourceFileList(lstFileItems, fOnlyBuildable, pcr))
		return FALSE;

	// construct our list of CPath *s
	listRcPath.RemoveAll();
	POSITION pos = lstFileItems.GetHeadPosition();
	while (pos != (POSITION)NULL)
		listRcPath.AddTail((void *)((CProjItem *)lstFileItems.GetNext(pos))->GetFilePath());

	return TRUE;	// ok
}

BOOL CBldSysIFace::GetResourceIncludePath
(
	CPath *pResFile, 
	CString &strIncludes
)
{
	ConfigurationRecord * pcr = (ConfigurationRecord *)NULL;

	// Also need to save any converted subprojects
	CProject::InitProjectEnum();
	CString strProject;
	CProject * pProject;
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL)
	{
		// get our list of resource CFileItems
		CObList lstFileItems;
		if (!pProject->GetResourceFileList(lstFileItems, FALSE, pcr))
			return FALSE;

		POSITION pos = lstFileItems.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CFileItem *pItem = (CFileItem *)lstFileItems.GetNext(pos);
			if (*(pItem->GetFilePath()) == *pResFile)
			{
				return(pProject->GetResourceIncludePath(pItem, strIncludes));
			}
		}
	}

	// get our list of resource CFileItems

	return FALSE;
}


// 
// Mixed-language support
//

// Is the language currently installed?
BOOL CBldSysIFace::LanguageSupported(UINT idLang)
{
	// perform this based on package installed?
	UINT idPkg = 0;

	if (idLang == CPlusPlus)
		idPkg = PACKAGE_LANGCPP;
	else if (idLang == Java)
		idPkg = PACKAGE_LANGJVA;
	else if (idLang == FORTRAN)
		idPkg = PACKAGE_LANGFOR;
	else
	{
		ASSERT(0);
	}
	

	return theApp.GetPackage(idPkg) != (CPackage *)NULL;
}


BOOL CBldSysIFace::OpenResource
(
	const TCHAR *	strResource,				// comma-seperated list of commands (can be macro-ized)
	long 			lType,
	HBLDTARGET		hTarg,					// handle to the target 
	HBUILDER		hBld					// build that contains target? default='active builder'
)
{
	BOOL hr = TRUE;
	CPtrList ptrlist;
	HFILESET hFileSet = GetFileSet(hBld, hTarg);
	GetResourceFileList(ptrlist, FALSE, hFileSet, hBld);
	if ( ptrlist.GetCount() > 0 )
	{
		CPath *pResPath = (CPath *)ptrlist.GetHead();
		pResPath->GetActualCase(TRUE);

		HCOMPOSITION hComp;
		LPRESOURCEIO pInterface;
		if (SUCCEEDED(theApp.FindInterface(IID_IResourceIO, (LPVOID FAR *)&pInterface)))
		{
			if (SUCCEEDED(pInterface->OpenRcFile(hFileSet, pResPath->GetFullPath(), &hComp)))
			{
				if (SUCCEEDED(pInterface->OpenResource(hComp, RT_DIALOG, strResource /* id */, -1 , NULL)))
				{
					hr = FALSE;
				}
			}
			pInterface->Release();
		}
	}
	return hr;
}

//
// Custom Build step support
//

BOOL CBldSysIFace::AssignCustomBuildStep
(
	const TCHAR *	pchCommand,				// comma-seperated list of commands (can be macro-ized)
	const TCHAR *	pchOutput,				// comma-seperated list of output files (can be macro-ized)
	const TCHAR *	pchDescription,			// description (if NULL then default description)
	HBLDTARGET		hTarg,					// handle to the target 
	HBLDFILE		hFile,					// handle to the file (if NO_FILE then custom build step for target)
	HBUILDER		hBld					// build that contains target? default='active builder'
)
{
	// do we have a command and output?
	if (!pchCommand || pchCommand[0] == _T('\0') ||
		!pchOutput || pchOutput[0] == _T('\0')
	   )
		return FALSE;

	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);
    ASSERT(pProject);
	if( pProject == NULL )
		return FALSE;

	CProjTempProjectChange projChange(pProject);

    // get the config rec for the target we depend upon.
    ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);

	CProjItem * pItem = hFile != NO_FILE ? (CProjItem *)CnvHBldFile(GetFileSet(ACTIVE_BUILDER, hTarg), hFile) : pProject;

	// force this item into our config.
	CProjTempConfigChange projTempConfigChange(pProject);
	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	

 	CProjType *pProjType = pProject->GetProjType();
	if ( pProjType && pProjType->GetUniqueTypeId() == CProjType::exttarget ){
		pProject->SetStrProp(P_Proj_CmdLine, pchCommand);
	
		CString str = _T("");
		pProject->SetStrProp(P_Proj_RebuildOpt, pchDescription);
		pProject->SetStrProp(P_Proj_Targ, pchOutput);
		pProject->SetStrProp(P_Caller, pchOutput);
		pProject->SetStrProp(P_Proj_BscName, str);
		//pProject->SetStrProp(P_Proj_WorkingDir, str);
		pProject->SetStrProp(P_WorkingDirectory, str);
		pProject->SetStrProp(P_Args, str);
		pProject->SetIntProp(P_PromptForDlls, 1);
		pProject->SetStrProp(P_RemoteTarget, str);
		return TRUE;	// ok
	}

	// replace all new line characters with comma's
	TCHAR *pStr = _tcsdup(pchCommand);
	TCHAR *pCommand = pStr;
	
	while( *pStr != _T('\0') ){
		if( *pStr == _T('\n')  ){
			*pStr = _T('\t');
		}
		pStr = _tcsinc(pStr);
	}

	pStr = _tcsdup(pchOutput);
	TCHAR *pOutput = pStr;
	
	while( *pStr != _T('\0') ){
		if( *pStr == _T('\n')  ){
			*pStr = _T('\t');
		}
		pStr = _tcsinc(pStr);
	}
	CString strOutput = pOutput;
	CString strCommand = pCommand;
	
	// set custom build step
	BOOL fRet =
		CCustomBuildTool::SetCustomBuildStrProp(pItem,P_CustomCommand, strCommand) &&
		CCustomBuildTool::SetCustomBuildStrProp(pItem,P_CustomOutputSpec, strOutput) &&
		(!pchDescription || pItem->SetStrProp(P_CustomDescription, pchDescription));
	
	if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		pItem->SetIntProp(P_ItemIgnoreDefaultTool, TRUE);

	free(pCommand);
	free(pOutput);

	return fRet;	// ok
}

BOOL CBldSysIFace::UnassignCustomBuildStep
(
	HBLDTARGET		hTarg,					// handle to the target 
	HBLDFILE		hFile,					// handle to the file (if NO_FILE then custom build step for target)
	HBUILDER		hBld					// build that contains target? default='active builder'
)
{
	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);
    ASSERT(pProject);
	if( pProject == NULL )
		return FALSE;

	CProjTempProjectChange projChange(pProject);

    // get the config rec for the target we depend upon.
    ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);

	CProjItem * pItem = hFile != NO_FILE ? (CProjItem *)CnvHBldFile(GetFileSet(ACTIVE_BUILDER, hTarg), hFile) : (CProjItem *)CnvHTarget(hBld, hTarg);

	// force this item into our config.
	CProjTempConfigChange projTempConfigChange(pProject);
	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
		
	// un-assign custom build step
	BOOL fRet =
		pItem->SetStrProp(P_CustomCommand, "") &&
		pItem->SetStrProp(P_CustomOutputSpec, "") &&
		pItem->SetStrProp(P_CustomDescription, "");

	if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		pItem->SetIntProp(P_ItemIgnoreDefaultTool, FALSE);

	return fRet;	// ok
}

BOOL CBldSysIFace::SetUserDefinedDependencies
(
	const TCHAR *	pchUserDeps,				// comma-seperated list of files (can be macro-ized)
	HBLDTARGET		hTarg,					// handle to the target 
	HBLDFILE		hFile,					// handle to the file (if NO_FILE then custom build step for target)
	HBUILDER		hBld					// build that contains target? default='active builder'
)
{
	// get a builder for this target to be searched in
	CProject * pProject = CnvHBuilder(hBld);
    ASSERT(pProject);
	if( pProject == NULL )
		return FALSE;

	CProjTempProjectChange projChange(pProject);

    // get the config rec for the target we depend upon.
    ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);

	CProjItem * pItem = hFile != NO_FILE ? (CProjItem *)CnvHBldFile(GetFileSet(ACTIVE_BUILDER, hTarg), hFile) : pProject;

	// force this item into our config.
	CProjTempConfigChange projTempConfigChange(pProject);
	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	

	// replace all new line characters with tab's
	TCHAR *pCommand;
	if (NULL!=pchUserDeps && 0 != pchUserDeps[0])
	{
		pCommand = _tcsdup(pchUserDeps);
		TCHAR *pStr = pCommand;
		while( *pStr != _T('\0') ){
			if( (*pStr == _T('\n')) || (*pStr == _T(',')) ){
				*pStr = _T('\t');
			}
			pStr = _tcsinc(pStr);
		}
	}
	else
		pCommand = _tcsdup(_T(""));

	

	// set custom build step
	BOOL fRet = pItem->SetStrProp(P_UserDefinedDeps, pCommand);
	
	free(pCommand);

	return fRet;	// ok
}

BOOL CBldSysIFace::GetSelectedFile
(
	CPath * path						// path of file currently selected
)
{
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	CMultiSlob *pMultiSlob;
	VERIFY(SUCCEEDED(pProjSysIFace->GetSelectedSlob(&pMultiSlob)));

	if (pMultiSlob == NULL)
		return FALSE;

	CProjItem * pItem = (CProjItem *)pMultiSlob->GetDominant();
	if (pItem == NULL)
		return FALSE;

	if (!pItem->IsKindOf(RUNTIME_CLASS(CProjItem)))
		return FALSE;
	
	if (pItem->GetContentList() != NULL)
		return FALSE;

	path = (CPath *)pItem->GetFilePath();

	return TRUE;
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

static S_FileId mapFileIdToProp[] =
{
	{P_OutputDir_Pdb, BCID_OptHdlr_Compiler, CActionSlob::szDefVCFile, _TEXT(".pdb")},	// file id 0
	{P_OutputDir_Pch, BCID_OptHdlr_Compiler, CActionSlob::szDefVCFile, _TEXT(".pch")},	// file id 1
	{P_OutputDir_Pdb, BCID_OptHdlr_Compiler, CActionSlob::szDefVCFile, _TEXT(".idb")},	// file id 2
};

BOOL CBldSysIFace::GetBuildFile
(
	UINT idFile,
	TCHAR * szFileName, int cchFileBuffer,
	HBLDTARGET hTarg,
	HBUILDER hBld /* = ACTIVE_BUILDER */
)
{
	if ((idFile + 1) > (sizeof(mapFileIdToProp) / (sizeof(UINT) * 2)))
	{
		ASSERT(FALSE);	// alert failure in debug version
		return FALSE;	// failed
	}

	// get this target's config record
	ConfigurationRecord * pcr = GetConfigRecFromTargetI(hTarg, hBld);

	CString strFile;
	CProject * pItem = CnvHBuilder(hBld);
	if( pItem == NULL )
		return FALSE;

	// force this item into our config.
	CProjTempConfigChange projTempConfigChange(pItem->GetProject());
	projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	

  	BOOL fRet = FALSE;

	COptionHandler * popthdlr;

	BOOL fGotFileName = FALSE;

		// lookup the option handle for the compiler?
	if (g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, mapFileIdToProp[idFile].s_idOptHdlr),
									  (CBldSysCmp *&)popthdlr) &&

		// use it to map the .PDB name string prop?
		pItem->GetStrProp(popthdlr->MapLogical(mapFileIdToProp[idFile].s_idProp), strFile) &&

		// not empty?
		!strFile.IsEmpty()
	   )
	{
		// Okay, the item has an apperently non empty-name entry.  See if its
		// a directory by checking last character.  If it is,
		int len = strFile.GetLength();
		const TCHAR * pc = (const TCHAR *)strFile;
		pc = _tcsdec(pc, (TCHAR *)(pc+len));
	
		// need a filename?
		if (*pc != _T('/') && *pc != _T('\\'))
			fGotFileName = TRUE;	// no ... it doesn't appear to be a dir.
	}

	// got a filename?
	if (!fGotFileName)
	{
		// no, create a default..
		if (mapFileIdToProp[idFile].s_pcDefFilename)
			strFile += mapFileIdToProp[idFile].s_pcDefFilename;
		else
			// just append and change extension later
			strFile += pItem->GetFilePath()->GetFileName();
	}

	CPath pathFile;

	if (pathFile.CreateFromDirAndFilename(pItem->GetProjDir(), strFile))
	{
		// change the extension
		pathFile.ChangeExtension(mapFileIdToProp[idFile].s_pcDefExtension);

		// can we squirt this into the buffer?
		TCHAR * pch = (TCHAR *)(const TCHAR *)pathFile;
		if (_tcslen(pch) < (size_t)cchFileBuffer)
		{
			_tcscpy(szFileName, pch);
			fRet= TRUE;
		}
	}


	return fRet;
}



// For mirrored case, we need to make sure dep gets updated
// For C-syntax files, we just queue them up on the force update list, assuming parser will call us
// the force update list will do update when we write to proj file if parser didn't do anything for us
// For non-C-syntax files, we will just do the scan deps
void CBldSysIFace::PreserveDependencies
	(
		CTargetItem * pNewTarget,
		CProject * pProject
	)
{
//	ASSERT(!g_bNewUpdateDepModel);

	ASSERT(pNewTarget);
	CObList SubList;
	pNewTarget->GetBuildableItems(SubList);
	POSITION pos = SubList.GetHeadPosition();
	while (pos)
	{
		CProjItem *pTempItem = (CProjItem *) SubList.GetNext( pos );
		ASSERT( pTempItem->IsKindOf( RUNTIME_CLASS(CProjItem) ) );
		ASSERT_VALID( pTempItem );
		if (pTempItem->IsKindOf( RUNTIME_CLASS(CFileItem) ))
		{
			CPath * pPath = (CPath *)pTempItem->GetFilePath();
			TCHAR * pch = _tcschr((TCHAR *)pPath->GetFullPath(), '.');

#if 0
			// don't scan the deps for now, hopefully background parser will do this for us quick enough
			if (pch && (_tcsicmp(pch, ".cpp")==0 || _tcsicmp(pch, ".cxx")==0 || _tcsicmp(pch, ".c")==0 ))
			{
				//queue it for update later
				FileItemDepUpdateQ((CFileItem *)pTempItem, NULL, NULL, g_FileForceUpdateListQ);
			}
			else
			{
				//scan deps, we queue on update list, so it will be updated at idle time
				FileItemDepUpdateQ((CFileItem *)pTempItem, NULL, NULL, g_FileForceUpdateListQ);
				FileRegHandle frh = pTempItem->GetFileRegHandle();
				FileItemDepUpdateQ((CFileItem *)pTempItem, NULL, frh, g_FileDepUpdateListQ);
			}
#endif
		}
	}
}

BOOL CBldSysIFace::LoadAppWiz()
{
	// set up to invoke MFCAPPWZ.DLL for C/C++?
	return g_AppWizardHandler.LoadAppWiz();
}


// Return the full search path for includes for a file.
// FUTURE: Use per-file include paths, eg. as specified by /i for the linker
BOOL CBldSysIFace::GetIncludePathI
(
	CString & strIncDirs,
	const CString & strPlat,
	const CPath * ppathSrc
)
{
	BOOL fGotPerFile = FALSE;

	// make all of these relative to the builder directory
	HBUILDER hBld = GetActiveBuilder();
	CProject * pBuilder = hBld != NO_BUILDER ? (CProject *)CnvHBuilder(hBld) : (CProject *)NULL;
				
	if (ppathSrc != (const CPath *)NULL && pBuilder != (CProject *)NULL)
	{
		// active fileset?
		HFILESET hFileSet;
		if (!strPlat.IsEmpty())
		{
			HBLDTARGET hTarget = GetTarget(strPlat, hBld);
			hFileSet = GetFileSet(hBld, hTarget);

		}
		else
		{
			hFileSet = (HFILESET)pBuilder->GetActiveTarget();
		}

		HBLDFILE hFile;
		if( hFileSet ){
			if (GetFile(ppathSrc, hFile, hFileSet, hBld))
			{
				// get the target file CProjItem
				CFileItem * pItem = CnvHBldFile(hFileSet, hFile);
				ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CFileItem)));
				
				ConfigurationRecord * pcr;
				CProjTempConfigChange projTempConfigChange(pItem->GetProject());
	
				if (!strPlat.IsEmpty())
				{
					// get this target config record
					pcr = pItem->ConfigRecordFromConfigName((const TCHAR *)strPlat, TRUE);
					ASSERT(pcr != (ConfigurationRecord *)NULL);
	
					// force this item into our config.
					projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);	
				}
	
				// get the include directories for this tool
				CBuildTool * pTool = pItem->GetSourceTool();
				if (pTool != (CBuildTool *)NULL)
				{
					pTool->GetIncludePaths(pItem, strIncDirs);
					fGotPerFile = TRUE;
				}
			}

		}
	}

	// got per-file includes?
	if (!fGotPerFile)
	{
		CDirMgr * pDirMgr = GetDirMgr();
		ASSERT(pDirMgr != (CDirMgr *)NULL);

		// get the index of the toolset
		int nToolSet = strPlat.IsEmpty() ? (int)-1 : pDirMgr->GetPlatformToolset(strPlat);

		pDirMgr->GetDirListString(strIncDirs, DIRLIST_INC, nToolSet);
	}

	return TRUE;	// success 
}

BOOL CBldSysIFace::IsBuildInProgress()
{
	return g_Spawner.SpawnActive();
}

HRESULT CBldSysIFace::DoSpawn(
	CStringList& lstCmd,		// Commands
	LPCTSTR szDirInit,			// Spawn directory
	BOOL fClear,				// Clear output window?
	BOOL fAsync,					// Asynchronous spawn
	DWORD *pcErr,				// OUT: number of errors
	DWORD *pcWarn				// OUT: number of warnings
)
{
	CDir dir;
	if (!dir.CreateFromString(szDirInit))
		return E_INVALIDARG;

	CErrorContext *pEC;
	if (g_Spawner.SpawnActive () ||	(pEC = g_Spawner.InitSpawn (fClear)) == NULL) 
		return E_UNEXPECTED;

	theApp.NotifyPackages (PN_BEGIN_BUILD, (void *) FALSE);
	// The following will be deleted by CSpawner::DoSpawn
	CCmdLine* pCmdLine = new CCmdLine();

	// Create CCmdLine object 
	CPtrList plCmds;
	POSITION posCmd = lstCmd.GetHeadPosition();
	while (posCmd)	 {
		CString& strCmd = lstCmd.GetNext(posCmd);
		pCmdLine->slCommandLines.AddTail(strCmd);
		pCmdLine->slDescriptions.AddTail("");
	}
	plCmds.AddTail(pCmdLine);

	CString str;
	CEnvironmentVariableList bldenviron;
	CDirMgr * pDirMgr = GetDirMgr();
	pDirMgr->GetDirListString(str, DIRLIST_PATH);
	bldenviron.AddVariable ("path", str);
	// Until the linker is fixed to not need the LIB
	// path for ENC relinks, pass this to the environment
	pDirMgr->GetDirListString ( str, DIRLIST_LIB );
	bldenviron.AddVariable ("lib", str);

	bldenviron.SetVariables();

	HRESULT hResult;
	DWORD errs, warns;
	UINT cmd = g_Spawner.DoSpawn(plCmds, dir, FALSE, FALSE, *pEC, fAsync);
	g_Spawner.GetErrorCount ( errs, warns );

	switch (cmd) {
	case CMD_Complete:
		hResult = NOERROR;
		break;
	case CMD_Canceled:
		hResult = E_ABORT;
		break;
	default:
		hResult = E_FAIL;
		// fixup case where an error occured but the spawner couldn't figure that out.
		// REVIEW: the whole counting of errors scheme should be reworked so we don't have to do this.
		// [vs98 24842]
		if (0 == errs)
			errs ++;
	}

	if (cmd == CMD_Canceled)
		pEC->AddString(IDS_USERCANCELED);

	bldenviron.ResetVariables();
	theApp.NotifyPackages (PN_END_BUILD, (void *) FALSE);
	g_Spawner.TermSpawn();
	*pcErr = errs;
	*pcWarn = warns;
	return hResult;
}

// Returns True if the given file name is in the fileset.
BOOL CBldSysIFace::IsProjectFile
(
	const CPath *	ppathFile,				// file path
	BOOL			bDepsInProject,			// Are dependencies part of the project
	HBUILDER		hBld					// builder that contains target? default='active builder' 
)
{
    // We'd better have a valid file path 
	if (!ppathFile) 
        return FALSE;

	// active builder means check all projects for this file
	if (hBld == ACTIVE_BUILDER)
	{
		// search all builders
		CString strBuilder;
		InitBuilderEnum();
		while ((hBld = GetNextBuilder(strBuilder, TRUE)) != NO_BUILDER)
		{
			ASSERT(hBld != ACTIVE_BUILDER);
			if (IsProjectFile(ppathFile, bDepsInProject, hBld))
				return TRUE;
		}
		return FALSE;
	}

	ASSERT(hBld != ACTIVE_BUILDER);
	// first, check for the project file itself!
	const CPath * ppathBuilder = GetBuilderFile(hBld);
	if ((ppathBuilder != NULL) && (*ppathFile == *ppathBuilder))
		return TRUE;

	// Check all filesets
	InitFileSetEnum(hBld);
	HFILESET hFileSet = GetNextFileSet();
	while (hFileSet != NO_FILESET)
	{
		// get the target item 
		CTargetItem * pTarget = CnvHFileSet(hBld, hFileSet);
	
		// get the targets file registry
		CFileRegistry * pRegistry = pTarget->GetRegistry();
		ASSERT(pRegistry != NULL);
		
		// check to see if the file is in the target already
		FileRegHandle frh = pRegistry->LookupFile(ppathFile);
		//
		// review(tomse):
		// The previous logic for bDepsInProject was to return TRUE if there was a file item
		// list and bDepsInProject is TRUE and to immediately return TRUE if !bDepsInProject.
		// i.e., if bDepsInProject is TRUE, then there also must be an associated file item
		// for IsProjectFile to return TRUE which seems to be the reverse meaning of bDepsInProject.
		// Therefore, bDepsInProject is passed into IsFileInTarget for the
		// bAsFileItem flag so that the file item list will be checked if TRUE.
		// 
		if (NULL!=frh && pTarget->IsFileInTarget(frh,bDepsInProject))
		{
			frh->ReleaseFRHRef();
			return TRUE;
		}
		if (NULL!=frh)
			frh->ReleaseFRHRef();

		hFileSet = GetNextFileSet();
	}
    return FALSE;
}
 
void CBldSysIFace::DeleteUnknownDepMap(void)
{
	if (pUnknownDepMap != NULL) {
		POSITION pos = pUnknownDepMap->GetStartPosition();
		while (pos != (POSITION)NULL) {
			CPtrList	*pUnknownDepList;
			CObject		*pObj;
			CString		key;
			pUnknownDepMap->GetNextAssoc( pos, key, pObj );
			pUnknownDepList = (CPtrList *)pObj;
			pUnknownDepList->RemoveAll();
			delete pUnknownDepList;
			pUnknownDepMap->RemoveKey( (LPCTSTR)key );
		}
		delete pUnknownDepMap;
		pUnknownDepMap = NULL;
	}
}

int CBldSysIFace::GetBuilderToolset(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if (pProject != NULL)
		return(pProject->GetProjectToolset());
	else
		return(-1);
}

CPath *CBldSysIFace::GetTargetFileName(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return NULL;
	return pProject->GetTargetFileName();
}

int CBldSysIFace::GetTargetAttributes(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return NULL;
	return pProject->GetTargetAttributes();
}

void CBldSysIFace::GetRemoteTargetFileName(CString &strTarg, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetRemoteTargetFileName(strTarg);
}

void CBldSysIFace::SetRemoteTargetFileName(CString &strTarg, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->SetRemoteTargetFileName(strTarg);
}

CWnd *CBldSysIFace::GetSlobWnd(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return NULL;
	return pProject->GetSlobWnd();
}

const CPath *CBldSysIFace::GetBrowserDatabaseName(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return NULL;
	return pProject->GetBrowserDatabaseName();
}

void CBldSysIFace::UpdateBrowserDatabase(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->UpdateBrowserDatabase();
}

BOOL CBldSysIFace::GetPreCompiledHeaderName( CString &strPch, HBUILDER hBld )
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;
	return pProject->GetPreCompiledHeaderName(strPch);
}

void CBldSysIFace::GetCallingProgramName(CString &strProg, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetCallingProgramName(strProg);
}

void CBldSysIFace::SetCallingProgramName(CString &strProg, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->SetStrProp(P_Caller, strProg);
}

void CBldSysIFace::GetProgramRunArguments(CString &strArgs, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetProgramRunArguments(strArgs);
}

void CBldSysIFace::SetProgramRunArguments(CString &strArgs, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->SetProgramRunArguments(strArgs);
}

void CBldSysIFace::GetWorkingDirectory(CString &strDir, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetWorkingDirectory(strDir);
}

BOOL CBldSysIFace::TargetIsCaller(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;
	return(pProject->TargetIsCaller());
}

BOOL CBldSysIFace::GetPromptForDlls(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;
	return(pProject->FPromptForDlls());
}

void CBldSysIFace::SetPromptForDlls(BOOL bPrompt, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->SetPromptForDlls(bPrompt);
}

void CBldSysIFace::SetProjectState(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->SetProjectState();
}

BOOL CBldSysIFace::GetClassWizAddedFiles(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;
	return(pProject->m_bClassWizAddedFiles);
}

void CBldSysIFace::SetClassWizAddedFiles(BOOL bAdded, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->m_bClassWizAddedFiles = bAdded;
}

void CBldSysIFace::ScanAllDependencies(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->ScanAllDependencies();
}

void CBldSysIFace::GetVersionInfo(PROJECT_VERSION_INFO *pInfo, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetVersionInfo(pInfo);
}

BOOL CBldSysIFace::GetInitialExeForDebug(BOOL bExecute, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;
	return(pProject->GetInitialExeForDebug(bExecute));
}

BOOL CBldSysIFace::GetInitialRemoteTarget(BOOL bBuild, BOOL fAlways, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;
	return(pProject->GetInitialRemoteTarget(bBuild, fAlways));
}

BOOL CBldSysIFace::UpdateRemoteTarget(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;
	return(pProject->UpdateRemoteTarget());
}

BOOL CBldSysIFace::DoTopLevelBuild(BOOL bBuildAll, CStringList *pConfigs,
			FlagsChangedAction fca, BOOL bVerbose, BOOL bClearOutputWindow, HBUILDER hBld, BOOL bRecurse )
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;
	return(pProject->DoTopLevelBuild(bBuildAll, pConfigs, fca, bVerbose, bClearOutputWindow, bRecurse));
}

void CBldSysIFace::GetEnvironmentVariables(CEnvironmentVariableList *pEnvList, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetEnvironmentVariables(pEnvList);
}

BOOL CBldSysIFace::IsProfileEnabled(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;
	return(pProject->IsProfileEnabled());
}

CPlatform *CBldSysIFace::GetCurrentPlatform(HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return NULL;
	return(pProject ? pProject->GetCurrentPlatform() : NULL);
}

IBuildProject* CBldSysIFace::GetAutoProject(HBUILDER hBld)
{
	// First, we need to find the name of the builder so we can create
	//  an auto project for it.  Iterate through the builders until
	//  we find the name.
	HBUILDER hBldCurr;
	CString strBuilderCurr;
	InitBuilderEnum();
	while ( (hBldCurr = GetNextBuilder(strBuilderCurr)) != NULL )
	{
		if (hBldCurr == hBld)
		{
			// Found the name, so create the project and leave!
			// Don't AddRef the auto project, since it was just created (that implicitly
			//  addrefs it).
			return (IBuildProject*) CAutoProject::Create(hBldCurr, strBuilderCurr)
				->GetIDispatch(FALSE);
		}
	}

	return NULL;				// Not found
}

void CBldSysIFace::GetJavaClassName(CString &str, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetJavaClassName(str);
}

void CBldSysIFace::GetJavaClassFileName(CString &str, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetJavaClassFileName(str);
}

void CBldSysIFace::GetJavaDebugUsing(ULONG* pDebugUsing, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	*pDebugUsing = pProject->GetJavaDebugUsing();
}

void CBldSysIFace::GetJavaBrowser(CString &str, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetJavaBrowser(str);
}

void CBldSysIFace::GetJavaStandalone(CString &str, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetJavaStandalone(str);
}

void CBldSysIFace::GetJavaStandaloneArgs(CString &str, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetJavaStandaloneArgs(str);
}

void CBldSysIFace::GetJavaHTMLPage(CString &str, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetJavaHTMLPage(str);
}

void CBldSysIFace::GetJavaClassPath(CString &str, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	pProject->GetJavaClassPath(str);
}

void CBldSysIFace::GetJavaStandaloneDebug(ULONG* pStandaloneDebug, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return;
	*pStandaloneDebug = pProject->GetJavaStandaloneDebug();
}

BOOL CBldSysIFace::GetInitialJavaInfoForDebug(BOOL bExecute, HBUILDER hBld)
{
	CProject *pProject = CnvHBuilder(hBld);
	if( pProject == NULL )
		return FALSE;
	return pProject->GetInitialJavaInfoForDebug(bExecute);
}


#ifdef _DEBUG
//
// debug function
//
void DebugBldIFace()
{
	TCHAR szBuffer[_MAX_PATH];
	VERIFY(g_BldSysIFace.GetBuildFile(IDFILE_COMPILER_PCH, szBuffer, _MAX_PATH, g_BldSysIFace.GetActiveTarget())); 
}
#endif  

HRESULT CBldSysIFace::GetPkgProject(HBUILDER builder, IPkgProject** outPkgProject)
// outPkgProject should be a COleRef<> and they should better support assignment.
{
	ASSERT(outPkgProject != NULL);
	CProject *pProject = CnvHBuilder(builder);
	if( pProject == NULL )
		return E_FAIL;
	COleRef<IBSProject> bsProject = pProject->GetInterface();
	if (*outPkgProject != NULL) {
		(**outPkgProject).Release();
	}
	return bsProject->QueryInterface(IID_IPkgProject, (void**) outPkgProject);
}
BOOL CBldSysIFace::GetProjectGlyph(HBUILDER hBld, HFILESET hFileSet, CImageWell& imageWell, UINT * pnImage)
{
	if (&imageWell == NULL || pnImage == NULL)
		return FALSE;

	*pnImage = 4;
	VERIFY(imageWell.Load(IDB_SINGLE_ITEM, CSize(16, 16)));
	VERIFY(imageWell.Open());
	VERIFY(imageWell.CalculateMask());

	if ((hFileSet != ACTIVE_FILESET) && (hFileSet != NO_FILESET))
		hBld = GetBuilderFromFileSet(hFileSet);
	
	CProject * pProject = CnvHBuilder(hBld);
	if (pProject == NULL)
		return FALSE;

	// different glyphs for different proj types
	*pnImage += pProject->GetGlyphIndex(); // 0, 1 or 2

	return TRUE;
}
