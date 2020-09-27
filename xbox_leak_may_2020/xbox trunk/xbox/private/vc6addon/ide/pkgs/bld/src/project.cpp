//
//	PROJECT.CPP
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
#include <regstr.h>
#include "projcomp.h"
#include "dirmgr.h"
#include "resource.h"
#include "targdlg.h"
#include "optndlg.h"
#include "toolsdlg.h"
#include "batchdlg.h"
#include "miscdlg.h"
#include "exttarg.h"
#include "targctrl.h"
#include "bldslob.h"
#include "vwslob.h"
#include "bldnode.h"
#include "toolcplr.h"
#include "oleref.h"

#include <utilbld_.h>
#include <prjapi.h>
#include <prjguid.h>

#include "bldrfile.h"	// our CBuildFile

#ifdef DEPGRAPH_VIEW
#include "grview.h"		// our dependency graph view
#endif

#include "depgraph.h"	// our dependency graph
#include "awiface.h"    // appwz constants
#include "prjdlg.h"     // CNewProjectCopyDlg
#include "updepdlg.h"
#include "configs.h"

#include "optnlink.h"	// to include P_Profile constant
#include "toolbsc.h"		// so we can do bscmake even if link fails
#include "optnmtl.h"		// so we can automatically add /mktyplib203
#include "optnrc.h"
#include "optnlib.h"

#include "mrdepend.h"
#include "autoprj.h"

#include <bldapi.h>

#include "profdlg.h"
#include "projitem.h"

#include "utilauto.h" // For g_pAutomationState 

LPPROJECTWORKSPACE g_pProjWksIFace = NULL;
LPPROJECTWORKSPACEWINDOW g_pProjWksWinIFace = NULL;

LPSOURCECONTROL g_pSccManager = NULL;

IMPLEMENT_SERIAL (CProject, CProjContainer, SLOB_SCHEMA_VERSION)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern void RefreshTargetCombos(BOOL bEmpty = FALSE);


extern BOOL __cdecl DumpHeap();
extern CProjComponentMgr g_prjcompmgr;
extern CTestableSection g_sectionDepUpdateQ;
extern CTestableSection g_sectionFileChangeQ;
extern BOOL g_bFileDepULQInUse;
extern BOOL g_bHTMLLog;
extern BOOL g_bInProjLoad; // defined in pfilereg.cpp
extern UINT g_nNextIncompatibleVersion;	// defined in vproj.cpp
//extern BOOL LoadOPTFile ( const CPath* pPath, CProject *pAssociatedProject  );
//extern BOOL SaveOPTFile ( const CPath* pPath, CProject *pAssociatedProject );
extern CPackage * g_pBuildPackage; // defined in vproj.cpp
BOOL   g_bInProjClose=FALSE;
BOOL g_bNoUI = FALSE;
extern CBuildTool *g_pLastTool;  // defined in projitem.cpp

int CProject::m_nBatch = 0;
CProject * g_pConvertedProject = NULL;
BOOL g_bConvDontTranslate = FALSE;

BOOL GetPlatformDescriptionString(CString & strExeName, CString & strPlatDesc, BOOL * pbIsJava = NULL);

// {180C58F2-F704-11cf-927C-00A0C9138C45}
static const CLSID BuildProjCLSID = 
{ 0x180c58f2, 0xf704, 0x11cf, { 0x92, 0x7c, 0x0, 0xa0, 0xc9, 0x13, 0x8c, 0x45 } };

// REVIEW: find a better home for these

BOOL g_bBadConversion = FALSE;
BOOL g_bAlwaysExportMakefile = FALSE;
BOOL g_bAlwaysExportDeps = FALSE;
BOOL g_bExportMakefile = FALSE;
BOOL g_bExportDeps = FALSE;
BOOL g_bWriteProject = FALSE;
BOOL g_bWriteBuildLog = FALSE;
BOOL g_bAllowFastExport = FALSE;  // NYI
int  g_nSharability = SHARE_NORM;
int  g_nBuildProcessors = 1;
BOOL g_bErrorPrompted = FALSE;
BOOL g_bConversionPrompted = FALSE;
BOOL g_bConversionCanceled = FALSE;

// Java: private helper functions for Java functionality
BOOL GetBrowserPath(CString& strBrowserPath);
BOOL GetStandalonePath(CString& strStandalonePath);

void InitExternalDepFolders()
{
	CProject * pProject;
	CProject::InitProjectEnum();
	CString strProject;
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL)
	{
		ConfigurationRecord* pcrBase;
		CProjTempConfigChange projTempConfigChange(pProject);

		int nSize = pProject->GetConfigArray()->GetSize();
		for (int nConfig = 0; nConfig < nSize; nConfig++)
		{
			pcrBase = (ConfigurationRecord *)(*pProject->GetConfigArray())[nConfig];
			if (pcrBase->IsBuildable())	// only do this for supported/buildable configs
			{
				projTempConfigChange.ChangeConfig(pcrBase);
				BOOL bNeedsDepUpdate = FALSE;
				if (valid==pProject->GetIntProp(P_HasExternalDeps,bNeedsDepUpdate) && bNeedsDepUpdate)
				{
					CTargetItem* pTarget = pProject->GetActiveTarget();
					if (NULL!=pTarget)
						pTarget->SetIntProp(P_TargNeedsDepUpdate,TRUE);
				}
			
				pProject->RefreshDependencyContainer();
			}
		}
	}
}

// lifted from CFileSaver
static void RemoveFileIfPossible(LPCTSTR szFileName)
{
	TRY
	{
		CFile::Remove(szFileName);
	}
	CATCH_ALL(e)
	{
		// Just silently ignore if we cant remove the file.
	}
	END_CATCH_ALL
}

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
	BOOL_PROP(ProjMarkedForScan)
	BOOL_PROP(ProjMarkedForSave)
	BOOL_PROP(ProjHasNoOPTFile)
END_SLOBPROP_MAP()
#undef theClass

CString CProject::s_strWebBrowser;
CString CProject::s_strTestContainer;

BOOL CProject::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// Enable deffered build tools items, don't do this for external projects!
	if	(!m_bProjIsExe &&
		 nCode == CN_UPDATE_COMMAND_UI &&
		 (nID >=IDM_PROJECT_TOOL_CMD_FIRST && nID <= IDM_PROJECT_TOOL_CMD_LAST)
		)
	{
		BOOL fAllowSchmoozeCmd = g_Spawner.CanSpawn();
		// Set menu text for the deferable Schmooze tools.  Get the tools
		// project type and iterate through the tools list, looking for
		// CSchmmooze tools.  For each one, ask it if its defereble.  If it
		// is, and its menu ID matches the one we want to update, then
		// go ahead and do the menu text update
		const CPtrList * pol = GetProjType ()->GetToolsList ();
		for (POSITION pos = pol->GetHeadPosition(); pos != NULL;)
		{
			CBuildTool * pTool = (CBuildTool *)pol->GetNext (pos);
			if (pTool->m_fDeferredTool && (pTool->GetMenuID() == nID))
			{
				CActionSlob action(this, pTool);

				CString str;
				(void)pTool->DeferredAction(&action, &str);
				if (!str.IsEmpty())
				{
					((CCmdUI *)pExtra)->SetText(str);
					break;	// found a match, there are no duplicates
				}
			}
		}

		((CCmdUI *)pExtra)->Enable(fAllowSchmoozeCmd);
		return TRUE;
	}

	// Is this a menu item to activate a defered schmooze tool?
	// We don't place these menu cmds on the menu popup for external
	// projects so this will never be invoked for external projects
	if (nCode == CN_COMMAND	&& pExtra == NULL &&
		(nID >=IDM_PROJECT_TOOL_CMD_FIRST && nID <=	IDM_PROJECT_TOOL_CMD_LAST)
	   )
	{
		ASSERT(!m_bProjIsExe);

		POSITION pos;
		const CPtrList * pol;
		CSchmoozeTool * pTool;
		CString str;

		// Bail if we're already doing a build:
		if (g_Spawner.SpawnActive()) return TRUE;

		pol = GetProjType ()->GetToolsList ();
		for ( pos = pol->GetHeadPosition(); pos != NULL; )
		{
			pTool = (CSchmoozeTool *) pol->GetNext (pos);
			if (!pTool->IsKindOf(RUNTIME_CLASS(CSchmoozeTool)))
				continue;
			if (pTool->GetMenuID ()== nID)
			{
				TopLevelRunDeferedBuildTool (pTool);
				return TRUE;
			}
		}

	}

	// WARNING! This does not fall through to a message map.  You must
	// handle both CN_COMMAND, and CN_UPDATE_COMMAND_UI if you want your
	// commands to be enabled.

	if (nCode == CN_COMMAND && pExtra == NULL)
	{
		switch (nID)
		{
		S_ON_COMMAND(IDM_PROJECT_DEFAULT_PROJ, OnSelectTarget)
#ifdef DEPGRAPH_VIEW
		S_ON_COMMAND(IDM_PROJECT_BUILDGRAPH, OnBuildGraph)
#endif // DEPGRAPH_VIEW
		S_ON_COMMAND(IDM_PROJECT_SETTINGS, OnSettings)
		S_ON_COMMAND(IDM_DEBUG_SETTINGS, OnSettings)
		S_ON_COMMAND(IDM_PROJECT_CONFIGURATIONS, OnConfigurations)
		S_ON_COMMAND(IDM_PROJECT_SUBPROJECTS, OnSubprojects)
		S_ON_COMMAND(IDM_PROJECT_TOOLS, OnTools)
		S_ON_COMMAND(IDM_PROJITEM_ADD_GROUP, OnNewGroup)
		S_ON_COMMAND(IDM_PROJECT_SETTINGS_POP, OnSettings)
		S_ON_COMMAND(IDM_INSERT_FILES_POP, OnPopupFilesIntoProject)
		S_ON_COMMAND(IDM_PROJITEM_COMPILE, OnCompilePostMsg)
		S_ON_COMMAND(IDM_PROJITEM_BUILD, OnBuildPostMsg)
		S_ON_COMMAND(IDM_PROJITEM_REBUILD_ALL, OnRebuildAllPostMsg)
		S_ON_COMMAND(IDM_PROJITEM_BATCH_BUILD, OnBatchBuildPostMsg)
		S_ON_COMMAND(IDM_PROJECT_STOP_BUILD, OnStopBuild)
		S_ON_COMMAND(IDM_BUILD_TOGGLE, OnToggleBuild)
		S_ON_COMMAND(IDM_PROJITEM_SCAN, OnScan)
		S_ON_COMMAND(IDM_PROJECT_SCAN_ALL, OnScanAll)
		S_ON_COMMAND(IDM_PROJECT_TOOL_MECR, OnToolMecr)
		S_ON_COMMAND(ID_TARGET_COMBO, OnSelectTarget)
		S_ON_COMMAND(ID_CONFIG_COMBO, OnSelectConfig)
		S_ON_COMMAND(ID_PROJITEM_COMPILE, OnCompile)
		S_ON_COMMAND(ID_PROJITEM_BUILD, OnBuild)
		S_ON_COMMAND(IDM_PROJECT_CLEAN_ACTIVE, OnClean)
		S_ON_COMMAND(ID_PROJITEM_REBUILD_ALL, OnRebuildAll)
		S_ON_COMMAND(ID_PROJITEM_BATCH_BUILD, OnBatchBuild)
		S_ON_COMMAND(IDM_PROJECT_EXPORT, OnExport)
		S_ON_COMMAND(IDM_PROJECT_WRITE, OnProjectWrite)
		S_ON_COMMAND(IDM_RUN_PROFILE, OnProfile)
		S_ON_COMMAND(IDM_DUMP_HEAP, OnDumpHeap)

		}
	}
	else if (nCode == CN_UPDATE_COMMAND_UI)
	{
		ASSERT(pExtra != NULL);
		CCmdUI* pCmdUI = (CCmdUI*)pExtra;
		ASSERT(pCmdUI->m_nID == nID);           // sanity assert

		switch (nID)
		{
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_DEFAULT_PROJ, OnUpdateTargetCombo)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJITEM_ADD_GROUP, OnUpdateNewGroup)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_SETTINGS, OnUpdateSettings)
		S_ON_UPDATE_COMMAND_UI(IDM_DEBUG_SETTINGS, OnUpdateSettings)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_CONFIGURATIONS, OnUpdateConfigurations)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_SUBPROJECTS, OnUpdateSubprojects)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_SETTINGS_POP, OnUpdateSettingsPop)
		S_ON_UPDATE_COMMAND_UI(IDM_INSERT_FILES_POP, OnUpdateFilesIntoProject)
#ifdef DEPGRAPH_VIEW
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_BUILDGRAPH, OnUpdateBuildGraph)
#endif // DEPGRAPH_VIEW
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_TOOLS, OnUpdateTools)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJITEM_COMPILE, OnUpdateCompile)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJITEM_BUILD, OnUpdateBuild)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_CLEAN_ACTIVE, OnUpdateClean)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJITEM_BATCH_BUILD, OnUpdateBatchBuild)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJITEM_REBUILD_ALL, OnUpdateRebuildAll)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_STOP_BUILD, OnUpdateStopBuild)
		S_ON_UPDATE_COMMAND_UI(IDM_BUILD_TOGGLE, OnUpdateToggleBuild)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJITEM_SCAN, OnUpdateScan)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_SCAN_ALL, OnUpdateScanAll)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_TOOL_MECR, OnUpdateToolMecr)
		S_ON_UPDATE_COMMAND_UI(ID_TARGET_COMBO, OnUpdateTargetCombo)
		S_ON_UPDATE_COMMAND_UI(ID_CONFIG_COMBO, OnUpdateConfigCombo)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_EXPORT, OnUpdateExport)
		S_ON_UPDATE_COMMAND_UI(IDM_PROJECT_WRITE, OnUpdateProjectWrite)
		S_ON_UPDATE_COMMAND_UI(IDM_RUN_PROFILE, OnUpdateProfile)
		S_ON_UPDATE_COMMAND_UI(IDM_DUMP_HEAP, OnUpdateDumpHeap)
		S_ON_UPDATE_COMMAND_UI(IDM_USE_WEBBROWSER, OnUpdateUseWebBrowser)
		S_ON_UPDATE_COMMAND_UI(IDM_USE_TESTCONTAINER, OnUpdateUseTestContainer)

		case IDM_BROWSE_FOR_DEBUG_EXE:
			pCmdUI->Enable(TRUE); 
			return TRUE;
		}
	}

	// pass on to the default handler
	return CProjContainer::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CProject::OnUpdateUseWebBrowser(CCmdUI *pCmdUI)
{
	static bFirst = TRUE;

	if (bFirst)
	{
#if 1
		if (!GetBrowserPath(s_strWebBrowser))
			s_strWebBrowser.Empty();
#else
		HKEY hKey;
		LONG lresult = RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("http\\shell\\open\\command"), 0,
			KEY_READ, &hKey );
		if (ERROR_SUCCESS==lresult)
		{
			TCHAR szPath[2*_MAX_PATH];
			DWORD dwSize = 2*_MAX_PATH*sizeof(TCHAR);
			DWORD dwType;
			lresult = RegQueryValueEx( hKey, _T(""), NULL, &dwType, (LPBYTE)szPath, &dwSize);
			if (ERROR_SUCCESS==lresult && REG_SZ==dwType)
			{
				s_strWebBrowser = szPath;
			}
			VERIFY(ERROR_SUCCESS==RegCloseKey(hKey));

			//
			// Try to remove parameters from command-line.
			// If command line starts with a double quote, remove the double quote and
			// everything after the 2nd quote.
			// If not a double quote, just remove everything after next space.
			// 
			s_strWebBrowser.TrimLeft();
			int iEnd;
			if (0==s_strWebBrowser.Find(_T('"')))
			{
				s_strWebBrowser = s_strWebBrowser.Mid(1);
				iEnd = s_strWebBrowser.Find(_T('"'));
				if (0<iEnd)
					s_strWebBrowser = s_strWebBrowser.Left(iEnd);
			}
			else
			{
				iEnd = s_strWebBrowser.Find(_T("\t "));
				if (0<iEnd)
					s_strWebBrowser = s_strWebBrowser.Left(iEnd);
			}
			s_strWebBrowser.TrimRight();
		}

		//
		// Try to get this string.
		//
		if (s_strWebBrowser.IsEmpty() && !GetBrowserPath(s_strWebBrowser))
			s_strWebBrowser.Empty();
#endif
		bFirst = FALSE;
	}

	pCmdUI->Enable(!s_strWebBrowser.IsEmpty());
}

extern BOOL (WINAPI *g_pGetFileAttributesEx_DevBld)( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation);
__inline BOOL Compatible_GetFileAttributesEx_DevBld( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation)
{
	return (*g_pGetFileAttributesEx_DevBld)( lpFileName, fInfoLevelId, lpFileInformation);
}

static BOOL GetTestContainerName(CString& strTestContainer)
{
	char szPath[2*_MAX_PATH];
	WIN32_FILE_ATTRIBUTE_DATA find;
	LPTSTR temp;
	DWORD dw;
	HKEY hKey;
	int iLastSlash;

	//
	// Search order:
	//	0) shared tools dir
	//	1) vc directory
	//	2) msdev.exe directory
	//	3) Use SearchPath()

	//
	// review(tomse): It would be nice to look in sharedIde\bin for tstcnt32.exe but
	// it can be installed in vc or vj.
	// Instead try looking in the vc path.
	//
	// shared tools dir
	//
	///////////////////////////////////
	// must build this key ourselves.  we can not use theApp.GetRegistryKeyName()
	// as this builds a key that takes into account the /i switch.  since these
	// settings are in HKLM and /i is an HKCU setting, we would end up with 
	// a bogus HKLM path.

	CString strKey = _T("Software\\Microsoft\\Shared Tools\\");
	strKey += _T("tstcon32.exe");

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType, cbData = 2*MAX_PATH*sizeof(TCHAR);
		if (RegQueryValueEx (hKey, _T ("Path"), NULL,
				&dwType, (LPBYTE) szPath, &cbData) == ERROR_SUCCESS)
		{
			ASSERT (dwType == REG_SZ);
			szPath [cbData] = 0;
			strTestContainer = szPath;

			if ((!strTestContainer.IsEmpty()) && !Compatible_GetFileAttributesEx_DevBld( (LPCTSTR)strTestContainer, GetFileExInfoStandard, &find))
				strTestContainer.Empty();

		}
		VERIFY(ERROR_SUCCESS==RegCloseKey(hKey));

		if (!strTestContainer.IsEmpty())
			return TRUE;
	}	

	// Try vc\bin
	//
	///////////////////////////////////
	// must build this key ourselves.  we can not use theApp.GetRegistryKeyName()
	// as this builds a key that takes into account the /i switch.  since these
	// settings are in HKLM and /i is an HKCU setting, we would end up with 
	// a bogus HKLM path.

	strKey = _T("Software\\Microsoft\\");
	strKey += theApp.GetExeString(DefaultRegKeyName);
	strKey += _T("\\Products\\");
	strKey += _T("Microsoft Visual C++");

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType, cbData = 2*MAX_PATH*sizeof(TCHAR);
		if (RegQueryValueEx (hKey, _T ("ProductDir"), NULL,
				&dwType, (LPBYTE) szPath, &cbData) == ERROR_SUCCESS)
		{
			ASSERT (dwType == REG_SZ);
			szPath [cbData] = 0;
			strTestContainer = szPath;

			// append backslash if necessary.
			iLastSlash = strTestContainer.ReverseFind(_T('\\'));
			if (strTestContainer.GetLength() != iLastSlash+1)
				strTestContainer += _T("\\");

			strTestContainer += _T("bin\\tstcon32.exe");
			if (!Compatible_GetFileAttributesEx_DevBld( (LPCTSTR)strTestContainer, GetFileExInfoStandard, &find))
				strTestContainer.Empty();

		}
		VERIFY(ERROR_SUCCESS==RegCloseKey(hKey));

		if (!strTestContainer.IsEmpty())
			return TRUE;
	}	

	//
	// Try msdev.exe directory
	//
	if (GetModuleFileName(NULL, szPath, sizeof(szPath)))
	{
		char szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
		_splitpath(szPath, szDrive, szDir, NULL, NULL);
		_makepath(szPath, szDrive, szDir, _T("tstcon32.exe"), NULL);
	}
	strTestContainer = szPath;

	if (!Compatible_GetFileAttributesEx_DevBld( (LPCTSTR)strTestContainer, GetFileExInfoStandard, &find))
	{
		dw = SearchPath(NULL, _T("tstcon32.exe"),NULL, _MAX_PATH, szPath, &temp);
		if ( 0 < dw && _MAX_PATH > dw )
			strTestContainer = szPath;
		else
			strTestContainer.Empty();
	}

	return (!strTestContainer.IsEmpty());
}

void CProject::OnUpdateUseTestContainer(CCmdUI *pCmdUI)
{
	static bFirst = TRUE;

	if (bFirst)
	{
		if (!GetTestContainerName(s_strTestContainer))
			s_strTestContainer.Empty();

		bFirst = FALSE;
	}
	
	pCmdUI->Enable(!s_strTestContainer.IsEmpty());
}

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
						ASSERT(newlen > 0);
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
			VERIFY(pItem->SetStrProp(idIncludes, strNewIncludes));
		}
	}
	return retval;
}

BOOL CProject::ConvertDirs()
{
	BOOL retval = FALSE;
	ASSERT(m_bConvertedVC20 || m_bConvertedDS4x);
	if (m_bConvertedVC20 || m_bConvertedDS4x)
	{
		COptionHandler * popthdlr;
		VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Compiler), (CBldSysCmp *&)popthdlr));
		UINT idCplrIncludes = popthdlr->MapLogical(P_CplrIncludes);
		UINT idOutputCod = popthdlr->MapLogical(P_OutputDir_Cod);
		UINT idOutputSbr = popthdlr->MapLogical(P_OutputDir_Sbr);
		UINT idOutputPch = popthdlr->MapLogical(P_OutputDir_Pch);
		UINT idOutputObj = popthdlr->MapLogical(P_OutputDir_Obj);
		UINT idOutputPdb = popthdlr->MapLogical(P_OutputDir_Pdb);

		VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_MkTypLib), (CBldSysCmp *&)popthdlr));
		UINT idMTLIncludes = popthdlr->MapLogical(P_MTLIncludes);
		UINT idMTLOutputTlb = popthdlr->MapLogical(P_MTLOutputTlb);
		UINT idMTLOutputInc = popthdlr->MapLogical(P_MTLOutputInc);

		VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_RcCompiler), (CBldSysCmp *&)popthdlr));
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
			pItem->SetOptBehaviour(optbehOld & ~(OBInherit|OBShowDefault|OBShowFake) | OBShowMacro);

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
				VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Linker), (CBldSysCmp *&)popthdlr));
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_OutName), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_LibPath), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_UsePDBName), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_DefName), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_ImpLibName), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_MapName), FALSE) || retval;

				// lib props
				VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Lib), (CBldSysCmp *&)popthdlr));
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_DefNameLib), FALSE) || retval;
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_OutNameLib), FALSE) || retval;

				// bscmake props
				VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_BscMake), (CBldSysCmp *&)popthdlr));
				retval = ConvertPropPaths(pItem, popthdlr->MapLogical(P_BscMakeOutputFilename), FALSE) || retval;

				// REVIEW: other props to convert?
			}
			pItem->SetOptBehaviour(optbehOld);
		}
	}
	return retval;  // TRUE iff anything changed
}

CProject::CProject ()
{
	// our current project is us!
	m_pOwnerProject = this;

	m_hndFileReg = NULL;
 	m_bProjIsExe = FALSE;
	m_bClassWizAddedFiles = FALSE;
 	m_bPrivateDataInitialized = FALSE;
	m_bPrivateDataInitializedFromOptFile = FALSE;
	m_bProjectComplete = FALSE;
	m_bNotifyOnChangeConfig = FALSE;
	m_bProjMarkedForScan = FALSE;
	m_bProjMarkedForSave = FALSE;
	m_bProjHasNoOPTFile = FALSE;
	m_bCareAboutRCFiles = TRUE;
	m_bCareAboutDefFiles = TRUE;
	m_bProjWasReadOnly = FALSE;
	m_bProjConverted = FALSE;
	m_bConvertedVC20 = FALSE;
	m_bConvertedDS4x = FALSE;
	m_bConvertedDS5x = FALSE;
	m_bGotUseMFCError = FALSE;
	m_bProjIsSupported = TRUE;	// safe default
	m_hOPTData = NULL;
	m_bProjExtConverted = FALSE;
    m_pOptTreeCtl = NULL;
    m_bOkToDirty = TRUE;
    m_bProjectDirty = FALSE;
	m_bSettingsChanged = FALSE;
	m_bDirtyDeferred = FALSE;
	m_nDeferred = 0;
	m_bPostMakeFileRead = FALSE;
	m_pIProject = NULL;
	m_nScannedConfigIndex = -1;
	m_bProj_PerConfigDeps = FALSE;

#ifdef VB_MAKEFILES
	m_bVB = FALSE;
	m_pVBinfo=NULL;
#endif

 	if (m_lstProjects.IsEmpty()) // init these only for the first project
	{
		g_nIDOptHdlrUnknown = 10; // 0 is reserved fir CCustomBuildTool
		g_nIDToolUnknown = PROJ_PER_CONFIG_UNKNOWN;
		m_nBatch = 0;
	}
	m_lstProjects.AddTail((CObject *)this);

	::InitializeCriticalSection(&m_critSection);
}
///////////////////////////////////////////////////////////////////////////////
void CProject::Destroy()
{
	// destroying our project
	BOOL bOldComplete = m_bProjectComplete;
	BOOL bOldNotify = m_bNotifyOnChangeConfig;
	m_bProjectComplete = m_bNotifyOnChangeConfig = FALSE;
 	m_strProjActiveConfiguration.Empty();
	m_strProjDefaultConfiguration.Empty();
	m_strProjStartupConfiguration.Empty();
   	m_bProjIsSupported = FALSE;	// safe default

	m_bOkToDirty = FALSE; // can't do this from here on...

	// disable file registry file change notification during builder destroy
	BOOL bOldDisable = g_FileRegDisableFC;
	g_FileRegDisableFC = TRUE;

	if (m_bConvertedDS4x)
	{
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
	
	// if we are an exe project, 
	// then delete our contents and any fake targets we might have created.
	// else delete each of the targets which in turn
	// will delete their contents
	if (m_bProjIsExe)
	{
		// external, ie. no targets
        HBLDTARGET hTarg;
        hTarg = g_BldSysIFace.GetActiveTarget((HBUILDER)this);
        ASSERT(hTarg != NO_TARGET);
        if( hTarg ){
            g_BldSysIFace.DeleteExeTarget(hTarg);
        }
		// external, ie. no targets
		DeleteContents();
	}
	else
	{
		// internal, ie. we have targets
		CString strTargetName; CTargetItem * pTarget; HBLDTARGET hTarg;
		POSITION pos;
		while ((pos = m_TargetMap.GetStartPosition()) != (POSITION)NULL)
		{
			m_TargetMap.GetNextAssoc(pos, strTargetName, (void* &)pTarget);
            hTarg = g_BldSysIFace.GetTarget(strTargetName, (HBUILDER)this);
            g_BldSysIFace.DeleteTarget(hTarg, (HBUILDER)this);
		}

		// now delete any configs that might not have been registered (read failed early)
		int n = GetPropBagCount();
		ConfigurationRecord * pcr;
		while (n-- > 0)	// work backwards so array is intact
		{
			pcr = (ConfigurationRecord*) m_ConfigArray[n];
			ASSERT(pcr);

			// Perform the delete...
			DeleteConfigFromRecord(pcr);
		}
	}

	// remove the dep update queues
	POSITION pos = g_FileForceUpdateListQ.GetHeadPosition();
	if (pos != (POSITION)NULL)
	{
		while (pos != (POSITION)NULL)
		{
			CDepUpdateRec *pdepRec = (CDepUpdateRec *)g_FileForceUpdateListQ.GetNext(pos);
			//remove the record with same file item and target
			delete pdepRec;
		}
		g_FileForceUpdateListQ.RemoveAll();
	}

	pos = g_FileDepUpdateListQ.GetHeadPosition();
	if (pos != (POSITION)NULL)
	{
		CritSectionT csFC(g_sectionFileChangeQ);
		CritSectionT cs(g_sectionDepUpdateQ);
		if (g_bFileDepULQInUse)
		{
			ASSERT(0);
			m_bProjectComplete = bOldComplete;
			m_bNotifyOnChangeConfig = bOldNotify;
			g_FileRegDisableFC = bOldDisable;
			return;
		}
		else
			g_bFileDepULQInUse = TRUE;
		pos = g_FileDepUpdateListQ.GetHeadPosition(); // must update after Crit
		while (pos != (POSITION)NULL)
		{
			CDepUpdateRec *pdepRec = (CDepUpdateRec *)g_FileDepUpdateListQ.GetNext(pos);
			delete pdepRec;
		} //while

		// done these, remove them
		g_FileDepUpdateListQ.RemoveAll();
	} //if

	// re-enable file registry file change notification
	g_FileRegDisableFC = bOldDisable;
	g_bFileDepULQInUse = FALSE;

	// finished destroying
	// restore previous state
	m_bProjectComplete = bOldComplete;
	m_bNotifyOnChangeConfig = bOldNotify;
}
///////////////////////////////////////////////////////////////////////////////
CProject::~CProject ()
{
	CWaitCursor wc;		// Display hourglass until this object destroyed
	
	if (m_lstProjects.GetCount()==1) // don't do this unless this is the last project!
	{
		g_bInProjClose = TRUE;

		// ** UI **
		// delete our deferred commands from the menu
		UpdateDeferredCmds(TRUE);
	}

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
		if (pbldsect->m_pfn == NULL && pbldsect->m_hBld == (HBUILDER)this) // This section must be from this project
		{
			VERIFY(g_BldSysIFace.DeregisterSection((const CBldrSection *)pbldsect));

			if (pbldsect->m_pch != (TCHAR *)NULL)
				free(pbldsect->m_pch);

			delete pbldsect;
		}
  	}

	POSITION pos = m_lstProjects.Find((CObject *)this);
	ASSERT(pos != NULL);
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
					HBLDTARGET hTarget = g_BldSysIFace.GetActiveTarget((HBUILDER)pProject);
					if (hTarget==NO_TARGET)
					{
						CTargetItem * pTargetItem;
						CString strTargetName;
						pProject->InitTargetEnum();
						VERIFY(pProject->NextTargetEnum(strTargetName, pTargetItem));
						hTarget = g_BldSysIFace.GetTarget(strTargetName, (HBUILDER)pProject);
						ASSERT (hTarget);
					}
					g_BldSysIFace.SetActiveTarget(hTarget, (HBUILDER)pProject);
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

#ifdef REFCOUNT_WORK
	if (m_hndFileReg)
	{
		m_hndFileReg->ReleaseFRHRef();
		m_hndFileReg = NULL;
	}
#endif
	ReleaseInterface();
	g_LinkAutoObjToProject.OnProjectDestroyed(this);
	g_bInProjClose = FALSE;

}

// this static function gets called only after all CProjects have been destroyed and deleted
void CProject::GlobalCleanup()
{
    // Clear out all unknown tools from all projtypes
    CProjType * pprojtype;
    g_prjcompmgr.InitProjTypeEnum();
    while (g_prjcompmgr.NextProjType(pprojtype))
        pprojtype->ClearUnknownTools();

}

///////////////////////////////////////////////////////////////////////////////
// we want to update the content of the 'Project' menu
// with our deferred commands. these will always exist
// if a tool has a deferred command, but it may be
// grayed if that deferred command is not applicable
// in the current project state
// by setting fNuke == TRUE, this function will
// Nuke them all

void CProject::UpdateDeferredCmds(BOOL fNuke)
{
	ASSERT(g_pBuildPackage != (CPack *)NULL);
	ASSERT(g_pBuildPackage == theApp.GetPackage(PACKAGE_VPROJ));

	// nuke them all
 	for (int nID = IDM_PROJECT_TOOL_CMD_FIRST; nID <= IDM_PROJECT_TOOL_CMD_LAST; nID++)
		g_pBuildPackage->SetVisibleMenuItem(nID, FALSE);

	// do we want to make any visible?
	// don't make any visible if we are external!
	if (fNuke || m_bProjIsExe)	return;	// no

	CProjType * pprojtype = GetProjType();
	if (pprojtype == (CProjType *)NULL)
		return;	// no project type->no commands

	// for all of our schmooze tools for this project type
	// we want to set the menu visible if we have a deferred command
	const CPtrList * pol = pprojtype->GetToolsList();
	for (POSITION pos = pol->GetHeadPosition(); pos != NULL;)
	{
		CBuildTool * pTool = (CBuildTool *)pol->GetNext (pos);
		if (pTool->m_fDeferredTool)
		{
			CActionSlob action(this, pTool);

			CString str;
			(void)pTool->DeferredAction(&action, &str);
			if (!str.IsEmpty())
				g_pBuildPackage->SetVisibleMenuItem(pTool->GetMenuID(), TRUE);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
CProjItem * CProject::EnsureFileInProject(CPath * pPath, BOOL bAddIfNecessary)
{
	ASSERT(pPath != (CPath *)NULL);

	CFileItem * pItem = (CFileItem *)NULL;

	HFILESET hFileSet = g_BldSysIFace.GetFileSet(ACTIVE_BUILDER, g_BldSysIFace.GetActiveTarget());
	HBLDFILE hFile;
 	if (!g_BldSysIFace.GetFile((const CPath *)pPath, hFile, hFileSet, (HBUILDER)this))
	{
		CString strError;

		// not there?
		// want to add it?
		if (!bAddIfNecessary ||
			MsgBox(Question,
				   MsgText(strError, IDS_NOT_IN_PROJ_QUERY_ADD, pPath->GetFileName()),
				   MB_YESNO) == IDNO
		   )
			return (CProjItem *)NULL;

		hFile = g_BldSysIFace.AddFile(hFileSet, (const CPath *)pPath);
	}

	if (hFile != NO_FILE)
		pItem = g_BldSysIFace.CnvHBldFile(hFileSet, hFile);

	return pItem;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::SetFile (const CPath *pPath, BOOL  /* = FALSE */)
{
	ASSERT(pPath != (const CPath *)NULL);
	ASSERT(pPath->IsInit());

	if (m_hndFileReg)
#ifndef REFCOUNT_WORK
		g_FileRegistry.ReleaseRegRef (m_hndFileReg);
		
	if ((m_hndFileReg = g_FileRegistry.RegisterFile(pPath, TRUE)) != NULL)
#else
		m_hndFileReg->ReleaseFRHRef();
		
	if ((m_hndFileReg = CFileRegFile::GetFileHandle(*pPath)) != NULL)
#endif
	{
		// get new title as well
		char buf[_MAX_FNAME];
		if (::GetFileTitle((const char *) *GetFilePath(), buf, _MAX_FNAME)==0)
			m_strProjItemName = buf;
		else
			m_strProjItemName = GetFilePath()->GetFileName();

		// init m_ProjDir here as well
		VERIFY(m_ProjDir.CreateFromPath(*GetFilePath()));

		m_ActiveTargDir = m_ProjDir;

		// make sure we're watching file-changes
		// we force this even if we're reading the .mak
		// in which case g_FileRegDisableFC == TRUE
		BOOL oldFileRegDisableFC = g_FileRegDisableFC;
		g_FileRegDisableFC = FALSE;
		BOOL fDummy;
		((CFileRegFile *)g_FileRegistry.GetRegEntry(m_hndFileReg))->WatchFile(fDummy);
		g_FileRegDisableFC = oldFileRegDisableFC;

		return TRUE;
	}

	return FALSE;
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
BOOL CProject::CanAct(ACTION_TYPE  action)
{
	switch (action)
	{
	default:
		return CProjContainer::CanAct(action);

	// case act_insert_into_undo_slob:
	case act_delete:
	case act_cut:
		return ((GetContainer()!=NULL) && (GetContainer()!=this));

	case act_copy:
	case act_paste:
		return FALSE;
	}
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::CanAdd ( CSlob * pAddSlob)
{
	// This method checks to see if the user
	// can add whatever is in the pAddSlob

#if 0	// can no longer add directly to the project
	// we should be able to add any group, since if the name conflicts we
	// just rename it
	if ( pAddSlob->IsKindOf ( RUNTIME_CLASS ( CProjGroup )) && !g_Spawner.SpawnActive())
		return TRUE;
#endif

	return CProjContainer::CanAdd (pAddSlob);
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::SetupPropertyPages(CSlob* , BOOL bSetCaption)
{
	// no pages

	if( bSetCaption )
		SetPropertyCaption(CString(MAKEINTRESOURCE(IDS_PROJECT)));

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
void CProject::CloneConfigs(const CProjItem * pItem)
{
//	Instead of creating or deleting configurations, just try to set ourselves
//  into one that matches pItem:
	MatchConfig((const ConfigurationRecord *)((CProjItem *)pItem)->GetActiveConfig());
}
///////////////////////////////////////////////////////////////////////////////
void CProject::MatchConfig(const ConfigurationRecord * )
{
//	Try to switch to configuation the "matches" pcr.  For subproject, this
//  means finding a confiugation that has the same name (but not nessesarily
//  the same project type as pcr.

	// FUTURE (karlsi): NYI
	ASSERT (FALSE);
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::GetResourceFileList(CObList & filelist, BOOL fOnlyBuildable /*= FALSE*/, const ConfigurationRecord *pcr /*= NULL*/)
{
	CTargetItem * pTarget;
	if (pcr == (ConfigurationRecord *)NULL)
		pTarget = GetTarget(GetActiveConfig()->GetConfigurationName());
	else
		pTarget = GetTarget(pcr->GetConfigurationName());

	if (pTarget == (CTargetItem *)NULL)
		return FALSE;
	
	CFileRegistry * pRegistry = pTarget->GetRegistry();
	return pRegistry->GetFileItemList(pTarget, pRegistry->GetRCFileList(), filelist, fOnlyBuildable, pcr);
}
///////////////////////////////////////////////////////////////////////////////
// Return the project's resource /I (includes) + Include Path for the specified
// resource file. Returns TRUE if can do, or FALSE if resource file not valid
BOOL CProject::GetResourceIncludePath(CFileItem * pFileItem, CString & strInclude)
{
	if (pFileItem == (CFileItem *)NULL)
	{
		ASSERT(FALSE);
		return FALSE;	// not valid!
	}

	ASSERT(pFileItem->GetFilePath());
	// should be an .RC file!
	if (_tcsicmp(pFileItem->GetFilePath()->GetExtension(), _TEXT(".rc")) != 0)
	{
		return FALSE;	// not an .RC file!
	}

	// get our semi-colon sep. list of include paths
	CProjType * pProjType; CBuildTool * pSrcTool;
	VERIFY (g_prjcompmgr.LookupProjTypeByName(GetActiveConfig()->GetOriginalTypeName(), pProjType));
	if( !pProjType || !pProjType->IsSupported())
		return FALSE;

	if (pProjType->GetUniqueTypeId() == CProjType::generic)
		return FALSE;

	// get the build tool for the .RC file
	if ((pSrcTool = pProjType->PickTool(pFileItem->GetFileRegHandle())) == (CBuildTool *)NULL)
		return FALSE;	// no build tool found (no resource compiler tool for config?)

	pSrcTool->GetIncludePaths(pFileItem, strInclude);

	return TRUE;	// success
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// do the base-class thing
	if (!CProjContainer::PreMoveItem(pContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	return TRUE;	// success
}
  
BOOL CProject::PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// do the base-class thing
	if (!CProjContainer::PostMoveItem(pContainer, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
// Verb Commands

void CProject::OnUpdateNewGroup(CCmdUI * pCmdUI)
{
	// [patbr] this command should never be enabled from here
	BOOL bEnable = FALSE;
	pCmdUI->Enable(bEnable);
}

void CProject::OnUpdateSettings(CCmdUI * pCmdUI)
{
	// This is only available if we are an external makefile or a supported
	// internal projtype, and a build isn't in progress.
	pCmdUI->Enable(m_bProjIsSupported && !g_Spawner.SpawnActive());
}

void CProject::OnUpdateConfigurations(CCmdUI *pCmdUI)
{
	// This is only available if we are an external makefile or a supported
	// internal projtype, and a build isn't in progress.
	pCmdUI->Enable(!m_bProjIsExe && !g_Spawner.SpawnActive());
}

void CProject::OnUpdateSubprojects(CCmdUI *pCmdUI)
{
	// This is only available if we are an external makefile or a supported
	// internal projtype, and a build isn't in progress.
	pCmdUI->Enable(!m_bProjIsExe && !g_Spawner.SpawnActive());
}

void CProject::OnUpdateTools(CCmdUI * pCmdUI)
{
	// only available if we are an internal makefile
	// (ie. not an exe-only project) and a build isn't in progress
	pCmdUI->Enable(!m_bProjIsExe && !g_Spawner.SpawnActive());
}

void CProject::OnUpdateSettingsPop(CCmdUI * pCmdUI)
{
	// This is only available if we are an external makefile or a supported
	// internal projtype, and a build isn't in progress.

	pCmdUI->Enable((m_bProjIsExe || GetProjType()->IsSupported())
		 && m_bProjIsSupported && !g_Spawner.SpawnActive());
}


void CProject::OnUpdateFilesIntoProject(CCmdUI * pCmdUI)
{
	// The New Group command is available only if this is
	// an internal project and we're not building.
	CString strTarget;
	BOOL bEnable = FALSE;
	BOOL bContextMenu = ::IsContextMenu(pCmdUI);
	
	// Initialization.  If this is an external makefile, disable
	// always.  Also, no compiles are allowed from the browse window
	CWnd* pWndActiveView = CWnd::GetActiveWindow();
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	if (!m_bProjIsExe &&
		!g_Spawner.SpawnActive() &&
		!(pWndActiveView &&
		pWndActiveView->IsKindOf(RUNTIME_CLASS(CFloatingFrameWnd))))
	{
		// Find the current selection.
		BOOL bActive = pProjSysIFace->IsWindowActive() == S_OK;

		bEnable = bActive; // new behaviour
	}

	if (bContextMenu)
	{

		CString			strGroupMenu;
		CSlob * pSlob;
		CMultiSlob* pMultiSlob = NULL;
	
		VERIFY(SUCCEEDED(pProjSysIFace->GetSelectedSlob(&pMultiSlob)));
		pSlob = pMultiSlob->GetDominant();
		if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
			strGroupMenu.LoadString(IDS_ADD_FILES_INTO_FOLDER);
		else 	
			strGroupMenu.LoadString(IDS_ADD_FILES_INTO_PROJECT);
		pCmdUI->SetText(strGroupMenu);
	}

	pCmdUI->Enable(bEnable);
}

static TCHAR szTextEditorC[]	= _T("Text Editor\\Tabs/Language Settings\\C/C++");
static TCHAR szTextEditorFor[]	= _T("Text Editor\\Tabs/Language Settings\\Fortran");
static TCHAR szFileExtensions[]	= _T("FileExtensions");

#define MENU_TEXT_CCH 200

void CProject::OnUpdateCompile(CCmdUI * pCmdUI)
{
	CString strTarget;
	BOOL bEnable = FALSE;
	BOOL bMenu = ::IsMenu(pCmdUI);
	BOOL bContextMenu = ::IsContextMenu(pCmdUI);

	// This is the quickest test possible so short cut the longer test.
	CProjType *pPt = GetProjType();
	if( !bMenu && !( pPt && pPt->IsSupported() && pPt->GetPlatform() && pPt->GetPlatform()->GetBuildable() && g_Spawner.CanSpawn()) ){
		pCmdUI->Enable( FALSE );
		return;
	}

	// Initialization.  If this is an external makefile, disable
	// always.  Also, no compiles are allowed from the browse window
	CWnd* pWndActiveView = CWnd::GetActiveWindow();

	if (!m_bProjIsExe &&
		!(pWndActiveView &&
		pWndActiveView->IsKindOf(RUNTIME_CLASS(CFloatingFrameWnd))))
	{
		// Find the current selection.
		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();

		CMultiSlob*	pMultiSlob = NULL;
		BOOL bActive = pProjSysIFace->IsWindowActive() == S_OK;
		if (bActive)
			VERIFY(SUCCEEDED(pProjSysIFace->GetSelectedSlob(&pMultiSlob)));


		// Enable if an appropriate file item is selected or if an
		// appropriate source window is selected.
		if (bActive)
		{
			CProjItem *		pProjItem = NULL;
			CBuildTool *	pTool;

			if (pMultiSlob != NULL)
			{
				// Note that we won't even consider enabling the command
				// if there is nothing in the selection, or if there is
				// more than one item in the selection.  Since doing a
				// compile is asynchronous, we'd have to keep a list of
				// the files to be compiled (much like the Batch Build
				// dialog), which is a lot more complication than this
				// feature probably merits.
				if (pMultiSlob->GetContentList()->GetCount() == 1)
				{
					CSlob * pSlob = pMultiSlob->GetDominant();
					if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
						pProjItem = ((CBuildSlob *)pSlob)->GetProjItem();
				}
			}

			if (pProjItem != (CProjItem *)NULL &&
				pProjItem->IsKindOf(RUNTIME_CLASS(CFileItem)) &&
				(pTool = pProjItem->GetSourceTool()) != (CBuildTool *)NULL
			   )
			{
				if (bMenu)
				{
					ASSERT(pProjItem->GetFilePath());
					strTarget = pProjItem->GetFilePath()->GetFileName();
				}
				bEnable = TRUE;
			}
		}
		else
		{
			CDocument *	pDoc = g_Spawner.GetLastDocWin();
			// We check here to see if the file is in the project,
			// Wait until the command is actually requested and deal with
			// the possibility that we don't know what to do with it then.

			if	(
				(pDoc != NULL) &&
				(!pDoc->GetPathName().IsEmpty())
				)
			{
				CString strPathName = pDoc->GetPathName();

				CPath *pPath = NULL;
#ifndef REFCOUNT_WORK
				FileRegHandle frh = g_FileRegistry.LookupFile( strPathName );
#else
				FileRegHandle frh = CFileRegFile::LookupFileHandleByName(strPathName);
#endif
				if (frh != NULL)
				{
					 pPath = (CPath *)g_FileRegistry.GetRegEntry(frh)->GetFilePath();
#ifdef REFCOUNT_WORK
					frh->ReleaseFRHRef();
#endif
				}


				// strPath.Create( strPathName );

				// now we must check if the file is part of the default project.
				// if it is, then thats good enough for now.
				// if it isn't then see if it can use a known supported tool.
				HFILESET 	hFileSet = NULL;
				HBLDFILE	hFile = NULL;
				HBUILDER 	hBld = g_BldSysIFace.GetActiveBuilder();

				HBLDTARGET  	hTarg = g_BldSysIFace.GetActiveTarget( hBld );
				if( hTarg != NULL ) {
					hFileSet = g_BldSysIFace.GetFileSet(hBld, hTarg);
				}

				if ((pPath!=NULL) && (g_BldSysIFace.GetFile ( pPath, hFile, hFileSet, hBld ))
					&& ( hFile != NULL )) {
					if (bMenu)
						strTarget = StripPath(strPathName);
					bEnable = TRUE;
				}
				else
				{	
					// the file is not the project. We still may know what to do.
					CString strExt = GetExtension(strPathName);
					CPath path; path.Create(strPathName);
					if	(g_BldSysIFace.LanguageSupported( CPlusPlus ))
					{
						CString strFileExts = GetRegString(szTextEditorC, szFileExtensions, _T("cpp;cxx;c;rc"));
						strFileExts += _T(";r;odl");
						if (FileNameMatchesExtension(&path, strFileExts))
						{
							if (bMenu)
								strTarget = StripPath(strPathName);
							bEnable = TRUE;
						}
					}
					if	(g_BldSysIFace.LanguageSupported( FORTRAN ))
					{
						CString strFileExts = GetRegString(szTextEditorFor, szFileExtensions, _T("f;for;f90;rc"));
						strFileExts += _T(";r;odl");
						if (FileNameMatchesExtension(&path, strFileExts))
						{
							if (bMenu)
								strTarget = StripPath(strPathName);
							bEnable = TRUE;
						}
					}
				}
			}
		}
	}

	if (bMenu)
	{
		TCHAR			szMenuText[MENU_TEXT_CCH];
		CString			strCompileMenu;

		if (bContextMenu)
			strCompileMenu.LoadString(IDS_CPOP_COMPILE);
		else
		{
			strCompileMenu.LoadString(IDS_PROJMENU_COMPILE);
			GetCmdKeyString(IDM_PROJITEM_COMPILE, strCompileMenu);
		}

		strTarget = strTarget.Left(MENU_TEXT_CCH - strCompileMenu.GetLength());
		MenuFileText(strTarget);
		wsprintf(szMenuText, strCompileMenu, (LPCTSTR) strTarget);
		pCmdUI->SetText(szMenuText);
	}

	pCmdUI->Enable(bEnable && GetProjType()->IsSupported() && g_Spawner.CanSpawn());
}

void CProject::OnUpdateBuild(CCmdUI * pCmdUI)
{

	BOOL bMenu = ::IsMenu(pCmdUI);
	BOOL bContextMenu = ::IsContextMenu(pCmdUI);

	if (bMenu)
	{
		TCHAR		szMenuText[MENU_TEXT_CCH];
		CString		strBuildMenu, strTarget;
		CPath *		pPath;

		if (bContextMenu)
			strBuildMenu.LoadString(IDS_CPOP_BUILD);
		else
		{
			strBuildMenu.LoadString(IDS_PROJMENU_BUILD);
			GetCmdKeyString(IDM_PROJITEM_BUILD, strBuildMenu);
		}

		pPath = GetTargetFileName();
		if (pPath == NULL)
		{
			CProjType * pProjType = GetProjType();

			if (pProjType && pProjType->GetUniqueTypeId() == CProjType::javaapplet)
			{
				const ConfigurationRecord* pcr = GetActiveConfig();

				strTarget = pcr->GetConfigurationName();
				int nSep = strTarget.Find (_T(" - "));
				if (nSep > 0)
					strTarget = strTarget.Left(nSep);
			}
		}
		else
		{
			strTarget = pPath->GetFileName();
			delete pPath;
		}
		strTarget = strTarget.Left(MENU_TEXT_CCH - strBuildMenu.GetLength());
		MenuFileText(strTarget);

		wsprintf(szMenuText, strBuildMenu, (LPCTSTR) strTarget);

		pCmdUI->SetText(szMenuText);
	}

	CProjType *pPt = GetProjType();
	if( !( pPt && pPt->GetPlatform() && pPt->GetPlatform()->GetBuildable()) ){
	    pCmdUI->Enable( FALSE );
		return;
	}

	pCmdUI->Enable((!m_bProjIsExe) && (GetProjType()->IsSupported()) && CanDoTopLevelBuild());
}

void CProject::OnUpdateRebuildAll(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(!m_bProjIsExe && GetProjType()->IsSupported() && GetProjType()->GetPlatform() && GetProjType()->GetPlatform()->GetBuildable() && CanDoTopLevelBuild());
}

void CProject::OnUpdateClean(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(!m_bProjIsExe && GetProjType()->IsSupported() && GetProjType()->GetPlatform() && GetProjType()->GetPlatform()->GetBuildable() && CanDoTopLevelBuild());
}

void CProject::OnUpdateBatchBuild(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(!m_bProjIsExe && m_bProjIsSupported && CanDoTopLevelBuild());
}

void CProject::OnUpdateStopBuild(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(g_Spawner.SpawnActive() > 0);
}

// A combined build and stop build command for the menus
void CProject::OnUpdateToggleBuild(CCmdUI * pCmdUI)
{
	BOOL bBuilding=g_Spawner.SpawnActive() > 0;
	if(bBuilding)
	{
		CString strBuild;
		strBuild.LoadString(IDS_BUILD_STOP);

		GetCmdKeyString(IDM_PROJECT_STOP_BUILD, strBuild);

		pCmdUI->SetText(strBuild);
		pCmdUI->Enable(TRUE);
	}
	else
	{
		// use the existing code to get the enabling and string
		OnUpdateBuild(pCmdUI);
	}
}

void CProject::OnUpdateExport(CCmdUI * pCmdUI)
{
	if (GetCurrentPlatform ()->GetUniqueId () == java)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(!m_bProjIsExe && m_bProjIsSupported && CanDoTopLevelBuild());
}

void CProject::OnUpdateScan(CCmdUI * pCmdUI)
{
	// If this is an exe project, or we are an unknown 
	// target, or during a build then disable always.
	if (m_bProjIsExe ||					// exe project
		g_Spawner.SpawnActive()	||		// doing a build
		!GetProjType()->IsSupported()	// unknown target?
	   )
	{
		pCmdUI->Enable(FALSE);
		return;
	}


	// Enable if a file item is selected or if an appropriate source
	// window is selected.
	BOOL bEnable = FALSE;
	
	// UNDONE (colint): Need to fix test for active project view
	if (FALSE)
	{
		CMultiSlob *	pMultiSlob;
		CProjItem *		pProjItem = NULL;

		// Find the current selection.
		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
		VERIFY(SUCCEEDED(pProjSysIFace->GetSelectedSlob(&pMultiSlob)));

 		if (pMultiSlob != NULL)
		{
			// Note that we won't even consider enabling the command
			// if there is nothing in the selection, or if there is
			// more than one item in the selection.  This is done to
			// be consistent with the Compile command, where supporting
			// multiple selection is quite difficult.
			if (pMultiSlob->GetContentList()->GetCount())
            {
                CSlob * pSlob = pMultiSlob->GetDominant();
                if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
                    pProjItem = ((CBuildSlob *)pSlob)->GetProjItem();
            }
		}
		bEnable = TRUE;
	}
	else
	{
		CDocument *		pDoc = g_Spawner.GetLastDocWin();

		// We don't check here to see if the file is in the project,
		// because it can be somewhat expensive.  Wait until the
		// command is actually requested and deal with the possibility
		// that the file isn't in the project then.
		//
		// Because we don't check if the file is in the project or not
		// we don't check if the file has a tool either for the same reason
		// Hence, if you have .r file open but not a Macintosh project type
		// this is still enabled.
		if	((pDoc != NULL) &&
			 (!pDoc->GetPathName().IsEmpty())
			)
		{
			CString strPathName = pDoc->GetPathName();
			CString strExt = GetExtension(strPathName);
			bEnable = ((_tcsicmp(strExt, _T(".c")) == 0) ||
					   (_tcsicmp(strExt, _T(".cpp")) == 0) ||
				       (_tcsicmp(strExt, _T(".cxx")) == 0) ||
					   (_tcsicmp(strExt, _T(".r")) == 0) ||
					   (_tcsicmp(strExt, _T(".odl")) == 0)
			          );
		}
	}

	pCmdUI->Enable(bEnable);
}

void CProject::OnUpdateScanAll(CCmdUI * pCmdUI)
{
	// If this is an exe project, disable always.
	if (m_bProjIsExe)			// exe project
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	// unknown target?
	CProjType * pProjType = GetProjType();
	ASSERT(pProjType != (CProjType *)NULL);

	// This is enabled if this is an internal makefile and the
	// project object itself can do a scan.
	pCmdUI->Enable(CanDoScan() &&			// can scan this item?
				   pProjType->IsSupported()	// supported platform
				  );
}

void CProject::OnUpdateToolMecr(CCmdUI * pCmdUI)
{
	CString str;
	BOOL bEnable = FALSE;

	// Set the text for the remote update tool menu option
	str.LoadString(IDS_DEFFERED_MECR);
	pCmdUI->SetText(str);

	CPlatform * pPlatform;

	// Enable the menu option if the platform associated with
	// the current project is the Macintosh 68k.
	if (m_bProjIsExe)
	{
 		CString strUIDescription;
 		VERIFY(GetStrProp(P_ExtOpts_Platform, strUIDescription));
		g_prjcompmgr.LookupPlatformByUIDescription(strUIDescription, pPlatform);
	}
	else
		pPlatform = (CPlatform *)GetProjType()->GetPlatform();

	// mfile is currently supported only on the mac (68k and ppc)
	bEnable = pPlatform->GetUniqueId() == mac68k ||
			  pPlatform->GetUniqueId() == macppc;

	// Enable only if we aren't building already
    if (!m_bProjIsExe)
    	bEnable = bEnable && CanDoTopLevelBuild();

	// Enable/Disable the menu option
	pCmdUI->Enable(bEnable);
}

void CProject::OnUpdateConfigCombo(CCmdUI * pCmdUI)
{
	// Not enabled for exe projects
	if (m_bProjIsExe || !CanDoTopLevelBuild())
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CProject::OnUpdateTargetCombo(CCmdUI * pCmdUI)
{
	// Not enabled for exe projects
	if (m_bProjIsExe || !CanDoTopLevelBuild())
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////

void CProject::OnPopupFilesIntoProject()
{
	// Find the current selection.
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();

	BOOL bActive = pProjSysIFace->IsWindowActive() == S_OK;
	ASSERT(bActive);
	
	CSlob * pSlob;
	CSlob * pContainer;
	CMultiSlob* pMultiSlob = NULL;
	long lpPkgProject = NULL;
	CString strFolderName;
	BOOL bProject = TRUE;

	// Enable if an appropriate file item is selected or if an
	// appropriate source window is selected.
	if (bActive)
	{
		VERIFY(SUCCEEDED(pProjSysIFace->GetSelectedSlob(&pMultiSlob)));
		if (pMultiSlob != NULL)
		{
			// Note that we won't even consider enabling the command
			// if there is nothing in the selection
			if (pMultiSlob->GetContentList()->GetCount() > 0)
			{
				pSlob = pMultiSlob->GetDominant();
				if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
				{
					//pSlob = ((CBuildViewSlob *)pSlob)->GetBuildSlob();
					bProject = FALSE;
					pContainer = (CBuildViewSlob *)pSlob;
					while (pContainer)
					{
					// Did we get it?
						if (( ((CBuildViewSlob *)pContainer)->GetLongProp(P_IPkgProject, lpPkgProject) == valid) && (lpPkgProject != NULL))
							break;
						else
							pContainer = (CBuildViewSlob *)pContainer->GetContainer();
					}
				}
				else if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildViewSlob)))
				{
					bProject = TRUE;
					pContainer = pSlob;
					while (pContainer)
					{
						// Did we get it?
						if ((pContainer->GetLongProp(P_IPkgProject, lpPkgProject) == valid) && (lpPkgProject != NULL))
						//if (pIPkgProject != NULL)
							break;
						else
							pContainer = pContainer->GetContainer();
					}
				}
			}
		}
	}
	ASSERT(pContainer != NULL);
	ASSERT(pContainer->IsKindOf(RUNTIME_CLASS(CProjSlob)));
	if (pContainer != NULL )
	{
		// Build must not be in progress
		ASSERT(!g_Spawner.SpawnActive());

		pSlob->GetStrProp(P_Title, strFolderName);
		if (strFolderName == "")
			strFolderName = "Default";

		CAddFilesDialog dlg((IPkgProject *)lpPkgProject, strFolderName);

		if (dlg.DoModal() == IDOK)
		{

			USES_CONVERSION;

			int iMaxFile = dlg.m_saFileNames.GetSize();
			if (iMaxFile > 0)
			{
				LPOLESTR *ppszFiles = new LPOLESTR[iMaxFile];

				for (int iFile = 0; iFile < iMaxFile; iFile++)
				{
					LPOLESTR wstr = T2W(dlg.m_saFileNames[iFile]);
					ppszFiles[iFile] = (LPOLESTR)malloc((wcslen(wstr)+1) * sizeof(wchar_t));
					wcscpy(ppszFiles[iFile], wstr);
				}
				LPCOLESTR pszDefaultFolder = NULL;
				VERIFY(SUCCEEDED(dlg.GetProject()->AddFiles((LPCOLESTR *)ppszFiles, iMaxFile, (bProject)? NULL:T2W(strFolderName))));
				for (iFile = 0; iFile < iMaxFile; iFile++)
					free(ppszFiles[iFile]);
				delete[] ppszFiles;
			}
		}
	}
}


void CProject::OnNewGroup()
{
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();

	CMultiSlob* pMultiSlob = NULL;
	VERIFY(SUCCEEDED(pProjSysIFace->GetSelectedSlob(&pMultiSlob)));
	if (pMultiSlob != NULL)
	{
		// Note that we won't even consider enabling the command
		// if there is nothing in the selection
		if (pMultiSlob->GetContentList()->GetCount() > 0)
		{
			CSlob * pSlob = pMultiSlob->GetDominant();
			if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildViewSlob)))
			{
				 pSlob = ((CBuildViewSlob *)pSlob)->GetBuildSlob();
			}
			if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
			{
				CProjItem *pContainer = (CProjItem *)((CBuildSlob *)pSlob)->GetProjItem();
				while (pContainer != NULL)
				{
				 	if ((pContainer->IsKindOf(RUNTIME_CLASS(CProjGroup))) ||
				 		(pContainer->IsKindOf(RUNTIME_CLASS(CTargetItem))))
						break;
					pContainer = (CProjItem *)pContainer->GetContainer();
				}

				ASSERT(pContainer != NULL);
				ASSERT(pContainer->IsKindOf(RUNTIME_CLASS(CProjContainer)));
				if (pContainer != NULL)
					((CProjContainer *)pContainer)->CmdAddGroup();
			}
		}
	}
}


////////////////////////////////////////////////////////////////////
// Function OnXXXPostMsg() simply post message to the main window
// and in turn it will call the 'real' function OnXXX()
// The reason we do this (bug fix 11217) is	because MFC keeps these
// toolbar buttons down during the command execution, and we want the
// button to return to normal right away.
////////////////////////////////////////////////////////////////////
void CProject::OnCompilePostMsg()
{
	AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_PROJITEM_COMPILE);
}

void CProject::OnBuildPostMsg()
{
	AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_PROJITEM_BUILD);
}

void CProject::OnRebuildAllPostMsg()
{
	AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_PROJITEM_REBUILD_ALL);
}

void CProject::OnCleanPostMsg()
{
	AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_PROJITEM_CLEAN);
}

void CProject::OnBatchBuildPostMsg()
{
	AfxGetMainWnd()->PostMessage (WM_COMMAND, ID_PROJITEM_BATCH_BUILD);
}

void CProject::OnCompile()
{
	CProjItem * pItemToCompile = NULL;
	CBuildTool * pTool;

	// If the project window is NOT active, we must have been enabled
	// because of a last active source window.
	CDocument *	pDoc;
	CPath		path;

	// Is project window active?
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	BOOL bActive = pProjSysIFace->IsWindowActive() == S_OK;

	if	(bActive || 
		 ((pDoc = g_Spawner.GetLastDocWin()) == NULL) ||
		 pDoc->GetPathName().IsEmpty() ||
		 !path.Create(pDoc->GetPathName())
		)
	{
		CMultiSlob *	pMultiSlob = NULL;
		
		// Find the current selection.
		pProjSysIFace->GetSelectedSlob(&pMultiSlob);

		if( (pMultiSlob != NULL) && (pMultiSlob->GetContentList()->GetCount() == 1) )
		{
            CSlob * pSlob = pMultiSlob->GetDominant();
            if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob))) {
                pItemToCompile = ((CBuildSlob *)pSlob)->GetProjItem();
			}
		}

		if( pItemToCompile == NULL ){
			ASSERT(FALSE); // item should exist!
		    return;
		}

	}
	else if ((pItemToCompile = EnsureFileInProject(&path, TRUE)) == NULL)
	{
		CString	strError;

		MsgBox(	Information,
				MsgText(strError, IDS_FILE_NOT_IN_PROJ_CANT_COMPILE, path.GetFileName()));
		return;
	}

	// At this point, we have a CProjItem.  Before we try to compile it, we
	// need to make sure:
	//	- It's a CFileItem
	//	- It has an associated tool

	if (!pItemToCompile->IsKindOf(RUNTIME_CLASS(CFileItem)) ||
		((pTool = pItemToCompile->GetSourceTool()) == (CBuildTool *)NULL)
	   )
	{
		CString	strError;
		MsgBox(	Information,
				MsgText(strError,
						IDS_CANT_COMPILE_NO_TOOL,
						(const TCHAR *)*pItemToCompile->GetFilePath()));
		return;
	}

	// Also make sure the item itself thinks it's OK to build.
	if (!pItemToCompile->CanDoTopLevelBuild())
	{
		ASSERT(FALSE);	// Command should have been disabled
		return;
	}

	// FINALLY, we can compile the damn thing.
	pItemToCompile->CmdCompile(TRUE);
}

void CProject::OnBuild()
{
	if (g_Spawner.SpawnActive())
	  	g_Spawner.DoSpawnCancel();

	// UNDONE: need to wait for build to finish before restarting

	CmdBuild();
}

void CProject::OnRebuildAll()
{
	CmdReBuild();
}

void CProject::OnClean()
{
	CmdClean();
}

void CProject::OnBatchBuild()
{
	// get the batch build dialog
	static CBatchBldDlg dlg;

	dlg.m_pBuildConfigs = &m_BuildConfigs;
	dlg.m_pBuildConfigs->RemoveAll();

	if (dlg.DoModal() == IDOK)
		CmdBatchBuild(dlg.m_bRebuild, &m_BuildConfigs, !dlg.m_bSelectionOnly, dlg.m_bClean);
}

BOOL GetSelectedProjects(CSlob & SelectionSlob)
{
	SelectionSlob.GetContentList()->RemoveAll();

	// multi-select projects
	// Is project window active?
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	BOOL bActive = pProjSysIFace->IsWindowActive() == S_OK;

	if	(bActive)
	{
		CMultiSlob *	pMultiSlob = NULL;
		
		// Find the current selection.
		pProjSysIFace->GetSelectedSlob(&pMultiSlob);

		if( (pMultiSlob != NULL) && (pMultiSlob->GetContentList()->GetCount() > 0) )
		{
			
			POSITION pos = pMultiSlob->GetContentList()->GetHeadPosition();
			while (pos != NULL)
			{
				CSlob * pSlob = (CSlob *)pMultiSlob->GetContentList()->GetNext(pos);
				if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildViewSlob)))
					pSlob = ((CBuildViewSlob *)pSlob)->GetBuildSlob();
			
				else if (!pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
					continue; // ignore ones that aren't ours

				pSlob = ((CBuildSlob *)pSlob)->GetProjItem();
				if (pSlob->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
					pSlob = ((CProjectDependency *)pSlob)->GetProjectDep();
				else if (!pSlob->IsKindOf(RUNTIME_CLASS(CTargetItem)))
					continue;

				pSlob = ((CProjItem *)pSlob)->GetProject();

				SelectionSlob.GetContentList()->AddTail(pSlob);
			}
		}
	}
	return (!SelectionSlob.GetContentList()->IsEmpty());
}

void CProject::OnExport()
{
	g_bExportDeps = g_bAlwaysExportDeps;

	// skip calling the dialog if Automation or only 1 project
	int nProjects = m_lstProjects.GetCount();
	BOOL bUseDialog = ((nProjects > 0) && (g_pAutomationState->DisplayUI() && !g_bNoUI)); //ShellOM:State

	CProject * pProject;
	if (bUseDialog)
	{
		// need to make sure we're not counting Java projects
		POSITION pos = m_lstProjects.GetHeadPosition();
		while (pos != NULL)
		{
			// count only valid C projects
			pProject = (CProject *)m_lstProjects.GetNext(pos);
			if ((pProject == NULL) || (!pProject->IsLoaded()) || (pProject->GetCurrentPlatform()->GetUniqueId()==java))
				nProjects--;
		}
		bUseDialog = (nProjects > 0);
	}

	// REVIEW: ExportAs dialog?
	CProjectsDlg dlg;
	dlg.m_bExport = TRUE;
	dlg.m_bExportDeps = g_bAlwaysExportDeps;
	CMultiSlob SelectionSlob;

	// copy selection to dialog, if any; otherwise use saved value
	if (bUseDialog)
	{
		if (GetSelectedProjects(SelectionSlob))
		{
			dlg.m_mapSelectedProjects.RemoveAll();
			POSITION pos = SelectionSlob.GetContentList()->GetHeadPosition();
			while (pos != NULL)
			{
				CProject * pProject = (CProject *)SelectionSlob.GetContentList()->GetNext(pos);
				ASSERT(pProject->IsKindOf(RUNTIME_CLASS(CProject)));
				dlg.m_mapSelectedProjects.SetAt(pProject->GetTargetName(), pProject);
			}
			ASSERT(!dlg.m_mapSelectedProjects.IsEmpty());
			SelectionSlob.GetContentList()->RemoveAll();
		}

		ASSERT(SelectionSlob.GetContentList()->IsEmpty());

		if (dlg.DoModal() != IDOK)
			return;

		g_bExportDeps = dlg.m_bExportDeps;
	}

	ASSERT(!g_bWriteProject);

	// m_bProjectDirty = TRUE;

	BOOL bExportDeps = g_bExportDeps; // save for iteration
	
	CString strProject;
	BOOL bIsSccActive = (g_pSccManager->IsSccInstalled() == S_OK);
	if (bUseDialog && bIsSccActive)
	{
		CStringArray files;
		CProject::InitProjectEnum();
		while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
		{
			// use selected projects from dlg
			ASSERT(!dlg.m_mapSelectedProjects.IsEmpty());
			CProject * pCmpProject;
			if (!dlg.m_mapSelectedProjects.Lookup(strProject, (void * &)pCmpProject))
			{
				continue;
			}
			else
			{
				ASSERT(pProject == pCmpProject);
			}

			if( pProject->m_bProjIsExe ) 
			{
				continue;
			}

			CPath path = *pProject->GetFilePath();
			path.ChangeExtension(_T(".mak"));
			if (path.IsReadOnlyOnDisk() && (g_pSccManager->IsControlled((const TCHAR *)path) == S_OK))
			{
				files.Add((const TCHAR*)path);
			}
			path.ChangeExtension(_T(".dep"));
			if (bExportDeps && path.IsReadOnlyOnDisk() && (g_pSccManager->IsControlled((const TCHAR *)path) == S_OK))
			{
				files.Add((const TCHAR*)path);
			}
		}
		if (files.GetSize() > 0)
		{
			g_pSccManager->CheckOutReadOnly(files, TRUE, TRUE); // ignore errors
		}
	}

	CProject::InitProjectEnum();
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		// use selected projects from dlg
		ASSERT((!bUseDialog) || (!dlg.m_mapSelectedProjects.IsEmpty()));
		if (bUseDialog)
		{
			CProject * pCmpProject;
			if (!dlg.m_mapSelectedProjects.Lookup(strProject, (void * &)pCmpProject))
			{
				continue;
			}
			else
			{
				ASSERT(pProject == pCmpProject);
			}
		}

		if( pProject->m_bProjIsExe ) 
		{
			ASSERT(0);
			continue;
		}

		g_bExportMakefile = TRUE;
		g_bExportDeps = bExportDeps;

		BOOL bSavedDirty = pProject->m_bProjectDirty;

#if 0	// REVIEW
		// Also write project file if dirty
		if (m_bProjectDirty)
			g_bWriteProject = TRUE;
#endif
		// save as makefile
		pProject->DoSave();
#if 1
		pProject->m_bProjectDirty = bSavedDirty;
		// REVIEW: dirty status changed?
#endif

	}
}

void CProject::OnScan()
{
	CProjItem *pItemToScan;

	// If the project window is NOT active, we must have been enabled
	// because of a last active source window.
	// UNDONE (colint): Need to fix for testing if proj view active
	if (TRUE)
	{
		CDocument *pDoc;
		CPath	path;

		if	(((pDoc = g_Spawner.GetLastDocWin()) == NULL) ||
					(pDoc->GetPathName().IsEmpty()) ||
					(!path.Create(pDoc->GetPathName())))
		{
			// We shouldn't have gotten here.  (The command shouldn't
			// have been enabled.)
			ASSERT(FALSE);
			return;
		}

		if ((pItemToScan = EnsureFileInProject(&path, TRUE)) == NULL)
		{
			CString	strError;

			MsgBox(	Information,
					MsgText(strError, IDS_FILE_NOT_IN_PROJ_CANT_SCAN, path.GetFileName()));
			return;
		}

		// Also make sure the item itself thinks it can be scanned.
		if (!pItemToScan->CanDoScan())
		{
			ASSERT(FALSE);	// Command should have been disabled
			return;
		}

		g_ScannerCache.UpdateDependencies( pItemToScan );
	}
	else
	{
		CMultiSlob *pMultiSlob;

		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
		VERIFY(SUCCEEDED(pProjSysIFace->GetSelectedSlob(&pMultiSlob)));

		if (!pMultiSlob->GetContentList()->GetCount())
		{
			ASSERT(FALSE);	// This command shouldn't have been enabled
			return;
		}

		// PERFORMANCE: Make sure pMultiSlob doesn't contain any CFileItem's groups
		// UNDONE (colint): Need to fix this up so that it gets a list of CProjItems
		// and not a list of CBuildSlobs
		//g_ScannerCache.UpdateDependencies( pMultiSlob );
	}
}

BOOL CProject::DoTopLevelScan(BOOL bWriteOutput)
{
#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "devbldd.dll", "DoTopLevelScan()", letypeBegin, 0);
#endif
	BOOL ret = g_ScannerCache.UpdateDependencies( this, FALSE, bWriteOutput );
#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "devbldd.dll", "DoTopLevelScan()", letypeEnd, 0);
#endif
	return ret;
}

void CProject::OnScanAll()
{
	if (g_Spawner.SpawnActive())
	{
		ASSERT(FALSE);	// Command should have been disabled
		return;
	}

	// save all before update all. Just in case.
	BOOL bQuery;
	if (g_Spawner.IsSaveBeforeRunningTools(&bQuery))
	{
		// save all files since we may have both internal and external
		theApp.SaveAll(bQuery, FILTER_DEBUG | FILTER_PROJECT | FILTER_NEW_FILE);
	}

	CUpdateDepDlg dlg;

	dlg.DoModal();
/*
	g_ScannerCache.UpdateDependencies( this );
*/

}

void CProject::OnSelectTarget()
{
	CSelectTargetDlg dlg(NULL, IDD_SELECT_TARGET, IDD_PROJ_SELECT_TARGET, NULL, FALSE);
	int rc = dlg.DoModal();
	if (rc == IDOK)
	{
		// Set the active target
		HBLDTARGET hTarget = g_BldSysIFace.GetTarget(dlg.m_strSelectedTarget, NO_BUILDER);
		ASSERT(hTarget);
		HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);
		ASSERT(hBld != NO_BUILDER);
	
		g_BldSysIFace.SetActiveTarget(hTarget, hBld);
		
		// REVIEW: this should no longer be needed:
    	// Refresh the target combos
	    CObList * plstCombos = CTargetCombo::GetCombos();
    	POSITION pos = plstCombos->GetHeadPosition();
	    while (pos != NULL)
	    {
		    CTargetCombo * pTargetCombo = (CTargetCombo *)plstCombos->GetNext(pos);
    		pTargetCombo->UpdateView();
	    }
	}
	else if (rc == IDABORT)
	{
		// Put up message box and quit
		MsgBox(Information, IDS_NO_TARGET_DEPS_TO_ADD);
	}
}

void CProject::OnSelectConfig()
{
	CSelectTargetDlg dlg(NULL, IDD_SELECT_TARGET, IDD_PROJ_SELECT_TARGET, NULL, FALSE);
	int rc = dlg.DoModal();
	if (rc == IDOK)
	{
		// Set the active target
		HBLDTARGET hTarget = g_BldSysIFace.GetTarget(dlg.m_strSelectedTarget, NO_BUILDER);
		ASSERT(hTarget);
		HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);
		ASSERT(hBld != NO_BUILDER);
	
		g_BldSysIFace.SetActiveTarget(hTarget, hBld);
		
		// REVIEW: this should no longer be needed:
    	// Refresh the target combos
	    CObList * plstCombos = CTargetCombo::GetCombos();
    	POSITION pos = plstCombos->GetHeadPosition();
	    while (pos != NULL)
	    {
		    CTargetCombo * pTargetCombo = (CTargetCombo *)plstCombos->GetNext(pos);
    		pTargetCombo->UpdateView();
	    }
	}
	else if (rc == IDABORT)
	{
		// Put up message box and quit
		MsgBox(Information, IDS_NO_TARGET_DEPS_TO_ADD);
	}
}

void CProject::OnToolMecr()
{
	UpdateRemoteTarget();
}

///////////////////////////////////////////////////////////////////////////////
// External interface for OnSettings()
BOOL CProject::ShowProjSettingsPage(ProjSettingsPage prjpage)
{
	switch (prjpage)
	{
		case Project_General:
		{
			// select IDS_GENERAL tab
			// in the Project Settings 'mondo' dialog
			// create the options dialog
			CProjOptionsDlg dlg(NULL, 0);	// first tab

			// do it
			dlg.DoModal();
			break;
		}

		case Project_DebugInfo_Settings:
		{
			if (m_bProjIsExe)
			{
				// select IDS_DEBUG_OPTIONS tab
				// in the Project Settings 'mondo' dialog
				// create the options dialog
				CProjOptionsDlg dlg(NULL, 1);	// second tab

				// do it
				dlg.DoModal();
			}
			else
			{
				// select IDS_GENERAL tab
				// in the Project Settings 'mondo' dialog
				// initial FOCUS is IDC_PROGARGS_OR_CALLER;
				CProjOptionsDlg dlg(NULL, 0);	// first tab

				// do it
				dlg.DoModal();
			}
			break;
		}

		default:
			ASSERT(FALSE);
			return FALSE;	// unknown page!
	}

	return TRUE;
}

// REVIEW (KPerry) this function is useless now. move the three lines
//         		   for CProjOptionsDlg to ::OnSettings
void CProject::OnSettingsCommon(CSlob * pSelection)
{
	// property notifications other than config. changing

	// o platform for exe projects
	// FUTURE (colint): Do we still need the platform field for exe projects
	CString strExtOptsPlatform;
	if (m_bProjIsExe)
		if (GetStrProp(P_ExtOpts_Platform, strExtOptsPlatform) != valid)
			strExtOptsPlatform = "";

	// create the options dialog
	CProjOptionsDlg dlg;

	dlg.SetSelection(pSelection);

	// do it
	dlg.DoModal();

	// o platform for exe projects
	CString strExtOptsPlatform_Post;
	if (m_bProjIsExe)
	{
		if (GetStrProp(P_ExtOpts_Platform, strExtOptsPlatform_Post) != valid)
			strExtOptsPlatform_Post = "";

		// has the platform changed?
		if (strExtOptsPlatform_Post != strExtOptsPlatform)
			theApp.NotifyPackages(PN_CHANGE_PLATFORM);
	}
}

void CProject::OnSettings()
{
	// multi-select targets
	// Is project window active?
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	BOOL bActive = pProjSysIFace->IsWindowActive() == S_OK;

	CMultiSlob SelectionSlob;
	CSlob * pSelection = NULL;
	if	(bActive)
	{
		CMultiSlob *	pMultiSlob = NULL;
		
		// Find the current selection.
		pProjSysIFace->GetSelectedSlob(&pMultiSlob);

		if( (pMultiSlob != NULL) && (pMultiSlob->GetContentList()->GetCount() > 0) )
		{
			
            CSlob * pDominantSlob = pMultiSlob->GetDominant();
			POSITION pos = pMultiSlob->GetContentList()->GetHeadPosition();
			while (pos != NULL)
			{
				CSlob * pSlob = (CSlob *)pMultiSlob->GetContentList()->GetNext(pos);
				if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildViewSlob)))
					pSlob = ((CBuildViewSlob *)pSlob)->GetBuildSlob();
			
				else if (!pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
					continue; // ignore ones that aren't ours
				pSlob = ((CBuildSlob *)pSlob)->GetProjItem();
				if ((pSlob->IsKindOf(RUNTIME_CLASS(CDependencyContainer))) ||
					(pSlob->IsKindOf(RUNTIME_CLASS(CDependencyFile))))
					continue;  // ignore deps
				if (pSlob->IsKindOf(RUNTIME_CLASS(CTargetItem)))
					pSlob = ((CProjItem *)pSlob)->GetProject();

				SelectionSlob.GetContentList()->AddTail(pSlob);
			}
		}
		if (!SelectionSlob.GetContentList()->IsEmpty())
			pSelection = &SelectionSlob;
	}

	OnSettingsCommon(pSelection);
}

void CProject::OnConfigurations()
{
	// Show the dialog.
	CProjectConfigurationsDlg dlg;

	dlg.DoModal();
}

void CProject::OnBuildGraph()
{
#ifdef DEPGRAPH_VIEW
	CGraphDialog grpdlg(g_buildengine.GetDepGraph(GetActiveConfig()));
	grpdlg.DoModal();
#endif
}

void CProject::OnUpdateBuildGraph(CCmdUI * pCmdUI)
{
	// This is only available if we are an internal makefile or a supported
	// internal projtype, and a build isn't in progress.
#ifdef DEPGRAPH_VIEW
	pCmdUI->Enable(!m_bProjIsExe && m_bProjIsSupported && !g_Spawner.SpawnActive());
#else
	pCmdUI->Enable(FALSE);
#endif
}

void CProject::OnDumpHeap()
{
#ifdef _DEBUG
	DumpHeap();
#endif
}


void CProject::OnProjectWrite()
{

	// skip calling the dialog if Automation or only 1 project
	int nProjects = m_lstProjects.GetCount();
	BOOL bUseDialog = ((nProjects > 1) && (g_pAutomationState->DisplayUI() && !g_bNoUI)); //ShellOM:State

	// REVIEW: ExportAs dialog?
	CProjectsDlg dlg;
	dlg.m_bExport = FALSE;
	CMultiSlob SelectionSlob;

	// copy selection to dialog, if any; otherwise use saved value
	if (bUseDialog)
	{
		if (GetSelectedProjects(SelectionSlob))
		{
			dlg.m_mapSelectedProjects.RemoveAll();
			POSITION pos = SelectionSlob.GetContentList()->GetHeadPosition();
			while (pos != NULL)
			{
				CProject * pProject = (CProject *)SelectionSlob.GetContentList()->GetNext(pos);
				ASSERT(pProject->IsKindOf(RUNTIME_CLASS(CProject)));
				dlg.m_mapSelectedProjects.SetAt(pProject->GetTargetName(), pProject);
			}
			ASSERT(!dlg.m_mapSelectedProjects.IsEmpty());
			SelectionSlob.GetContentList()->RemoveAll();
		}

		ASSERT(SelectionSlob.GetContentList()->IsEmpty());

		if (dlg.DoModal() != IDOK)
			return;
	}

	CProject::InitProjectEnum();
	CString strProject;
	CProject * pProject;
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		// use selected projects from dlg
		ASSERT((!bUseDialog) || (!dlg.m_mapSelectedProjects.IsEmpty()));
		if (bUseDialog)
		{
			CProject * pCmpProject;
			if (!dlg.m_mapSelectedProjects.Lookup(strProject, (void * &)pCmpProject))
			{
				continue;
			}
			else
			{
				ASSERT(pProject == pCmpProject);
			}
		}

		if( pProject->m_bProjIsExe ) 
		{
			ASSERT(0);
			continue;
		}

		// Always write the project file, even if it's not dirty
		g_bWriteProject = TRUE;
		pProject->m_bProjectDirty = TRUE;
		pProject->DoSave();
	}
}

void CProject::OnUpdateProjectWrite(CCmdUI * pCmdUI)
{
	// This is only available if we are an internal makefile or a supported
	// internal projtype, and a build isn't in progress.
#ifndef _SHIP
	pCmdUI->Enable(!m_bProjIsExe && m_bProjIsSupported && !g_Spawner.SpawnActive() && (GetFilePath()!=NULL));
#else
	pCmdUI->Enable(FALSE);
#endif
}

void CProject::OnUpdateDumpHeap(CCmdUI * pCmdUI)
{
	// This is only available if we are an internal makefile or a supported
	// internal projtype, and a build isn't in progress.
#ifndef _SHIP
	pCmdUI->Enable(TRUE);
#else
	pCmdUI->Enable(FALSE);
#endif
}

#if 0
void CProject::OnPopupSettings()
{
    // UNDONE (colint): Need to fix this to work with the new build view
	// create the options dialog
	//CSlobWnd * pSlobWnd = GetSlobWnd();
	//ASSERT(pSlobWnd->IsKindOf(RUNTIME_CLASS(CProjectView)));
	// copy the project window tree-control state
	//OnSettingsCommon(&((CProjectView *)pSlobWnd)->m_TreeWnd);

	// Is project window active?
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	BOOL bActive = pProjSysIFace->IsWindowActive() == S_OK;

	CMultiSlob SelectionSlob;
	CSlob * pSelection = NULL;
	if	(bActive)
	{
		CMultiSlob *	pMultiSlob = NULL;
		
		// Find the current selection.
		pProjSysIFace->GetSelectedSlob(&pMultiSlob);

		if( (pMultiSlob != NULL) && (pMultiSlob->GetContentList()->GetCount() > 0) )
		{
			
           	CSlob * pSlob = pMultiSlob->GetDominant();
			// UNDONE: not quite right
			{
				if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildViewSlob)))
					pSlob = ((CBuildViewSlob *)pSlob)->GetBuildSlob();
			
				if (pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
				{
					pSlob = ((CBuildSlob *)pSlob)->GetProjItem();
					// ignore deps
					if ((!pSlob->IsKindOf(RUNTIME_CLASS(CDependencyContainer))) &&
						(!pSlob->IsKindOf(RUNTIME_CLASS(CDependencyFile))))
					{
						// convert target to to project
						if (pSlob->IsKindOf(RUNTIME_CLASS(CTargetItem)))
							pSlob = ((CProjItem *)pSlob)->GetProject();

						SelectionSlob.GetContentList()->AddTail(pSlob);
					}
				}
			}
		}
		if (!SelectionSlob.GetContentList()->IsEmpty())
			pSelection = &SelectionSlob;
	}

    OnSettingsCommon(pSelection);
	// OnSettings();
}
#endif

void CProject::OnTools()
{
/*
R.I.P. for v4.0 with VBA?
	// get the tools dialog and show it as modal
	CToolsDlg dlg(this);
	dlg.DoModal();
*/
}

BOOL g_bStopBuild = FALSE;
BOOL g_bMultiBuild = FALSE;

void CProject::OnStopBuild()
{
	// only do this if spawner active!
	if (g_Spawner.SpawnActive())
	  	g_Spawner.DoSpawnCancel();

	if( g_bMultiBuild == TRUE)
		g_bStopBuild=TRUE;
}	 

// Start or stop a build
void CProject::OnToggleBuild()
{
	if (g_Spawner.SpawnActive() > 0)
	{
		OnStopBuild();
	}
	else
	{
		OnBuildPostMsg();
	}
}	 

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::InitNew(const CPtrList * plstProjType)
{
	m_listTargIncl.Init((HBUILDER)this); // REVIEW: do part of this in ctor

	if (plstProjType != (const CPtrList *)NULL)
	{
		POSITION pos = plstProjType->GetHeadPosition();
		ASSERT(pos != (POSITION)NULL);	// must have at least one projtype!

		HBLDTARGET hDebugTarg, hReleaseTarg;

		// create a pair of targets for each target type
		while (pos != NULL)
		{
			CProjType * pProjType = (CProjType *)plstProjType->GetNext(pos);
			(void) g_BldSysIFace.AddDefaultTargets
					(
						(const TCHAR *)*pProjType->GetPlatformName(),	// 'official' name of this target's platform
						(const TCHAR *)*pProjType->GetTypeName(),		// 'official' name of this target type
						hDebugTarg, hReleaseTarg,						// our new targets
						FALSE											// we're using 'official' names
					);
		}
	}

	m_bProjectComplete = TRUE;	// project is now ok to use...

	// add our deferred commands to the menu
	UpdateDeferredCmds(FALSE);

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////

// Given the name of a .EXE file (which need not exist), return our best
// guess as to the platform DESCRIPTION string (not the "official" platform
// string).  If the .EXE file doesn't exist, or if we can't figure out what
// type it is, return the platform description string for the primary platform.
BOOL GetPlatformDescriptionString(CString & strExeName, CString & strPlatDesc, BOOL * pbIsJava)
{
	// Default is the .EXE file doesn't exist, or we don't know its type.
	// Use the description string for the primary platform.
	strPlatDesc = *(g_prjcompmgr.GetPrimaryPlatform()->GetUIDescription());

	HANDLE hfile;
	DWORD dwBytes;
	IMAGE_DOS_HEADER doshdr;
	IMAGE_FILE_HEADER PEHeader;
	DWORD dwMagic;
	UINT nPlatformId;

	if ((hfile = CreateFile((const TCHAR *)strExeName, GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
		return TRUE;

	/* Look for XBE first */
	CString strExt = strExeName.Right(4);
	strExt.MakeLower();
	if(strExt == ".xbe") {
		ReadFile(hfile, &dwMagic, 4, &dwBytes, NULL);
		if(dwBytes == 4 && dwMagic == 0x48454258) {
			nPlatformId = xbox;
			goto GotPlatId;
		}
		SetFilePointer(hfile, 0, NULL, FILE_BEGIN);
	}

	ReadFile(hfile, (LPVOID) &doshdr, sizeof(IMAGE_DOS_HEADER), &dwBytes, NULL);
	if (dwBytes == sizeof(IMAGE_DOS_HEADER))
	{
		SetFilePointer(hfile, doshdr.e_lfanew, NULL, FILE_BEGIN);
		ReadFile(hfile, (LPVOID) &dwMagic, sizeof(dwMagic), &dwBytes, NULL);
		if (dwBytes == sizeof(dwMagic) || dwMagic != IMAGE_NT_SIGNATURE)
		{
			ReadFile(hfile, (LPVOID) &PEHeader, sizeof(IMAGE_FILE_HEADER), &dwBytes, NULL);
			if (dwBytes == sizeof(IMAGE_FILE_HEADER))
			{
				if (PEHeader.Machine != IMAGE_FILE_MACHINE_UNKNOWN)
				{
					// this may require putting the IMAGE_... value inside the CPlatform
					// object so that we can iterate through the CPlatform objects looking
					// for one with a matching value
					nPlatformId = g_prjcompmgr.GetIdFromMachineImage(PEHeader.Machine);
GotPlatId:
					if (nPlatformId != unknown_platform)
					{
						CPlatform *	pPlatform;

						g_prjcompmgr.InitPlatformEnum();
						while (g_prjcompmgr.NextPlatform(pPlatform))
						{
							if (pPlatform->GetUniqueId() == nPlatformId)
							{
								strPlatDesc = *(pPlatform->GetUIDescription());
								CloseHandle (hfile);
								return pPlatform->IsSupported();	// supported platform?
							}
						}
						// recognized but unsupported platform, so need to create
						// one dynamically and inform the caller of this fact (return FALSE)

						// register dynamic platform so deleted on exit
						pPlatform = new CPlatform(_T("<Unknown>"), nPlatformId);
						g_prjcompmgr.RegisterUnkProjObj(pPlatform);

						strPlatDesc = *(pPlatform->GetUIDescription());

						CloseHandle (hfile);
						return FALSE;	// unknown platform
					}
				}
				CloseHandle (hfile);
				return (TRUE);
			}
		}
	}
	// If we make it this far, the file exists and isn't an exe.  Maybe
	// its a java file.
	unsigned long ulMagic;

	SetFilePointer(hfile, 0, NULL, FILE_BEGIN);
	ReadFile(hfile, (LPVOID) &ulMagic, sizeof(unsigned long), &dwBytes, NULL);
	if (dwBytes == sizeof(unsigned long))
	{
		/*
		** Java: check for Java magic number 0xcafebabe
		*/
		if (ulMagic == 0xbebafeca)
		{

			// find the java platform...

			CPlatform *	pPlatform;

			g_prjcompmgr.InitPlatformEnum();
			while (g_prjcompmgr.NextPlatform(pPlatform))
			{
				if (pPlatform->GetUniqueId() == java)
				{
					strPlatDesc = *(pPlatform->GetUIDescription());
					CloseHandle (hfile);
					if (pbIsJava)
						*pbIsJava = TRUE;
					return pPlatform->IsSupported();	// supported platform?
				}
			}
			CloseHandle (hfile);
			return FALSE; // known but not supported.
		}
	}

	CloseHandle (hfile);
	return TRUE;	// unknown platform
}

void CProject::InitPrivateData()
{
	// Initailize private data based on makefile type:
	// If this is an external make file, create configurations named
	// "DEBUG" and "RELEASE", leaving DEBUG active, then prompt the
	// user for building info:
	CString str, str2;

    if (m_bProjIsExe)
	{
		CPath pathT;
		CPath pathTemp;

		pathT = *GetFilePath();
		pathTemp = pathT;

		// Setup the initial "Debug" target...

		str.LoadString (IDS_DEBUG_CONFIG);
		CreateConfig(str);
		SetStrProp ( P_ProjActiveConfiguration, str);

		// Set the default properties for the debug configuration:
		str = "NMAKE /f ";
		pathTemp.ChangeExtension(".mak");
		str += pathTemp.GetFileName();
		SetStrProp(P_ExtOpts_CmdLine, str);

		str = "/a";
		SetStrProp(P_ExtOpts_RebuildOpt, str);

		str = pathT.GetFileName();
		SetStrProp(P_ExtOpts_Targ, str);
		SetStrProp(P_Caller, str);

		// get the platform description string, if we
		// can't then this executable is known but unsupported
		// by an installed platform
		BOOL bIsJava = FALSE;

		if (!GetPlatformDescriptionString(str, str2, &bIsJava))
			InformationBox(IDS_EXE_NOT_SUPPORTED, str);
		SetStrProp(P_ExtOpts_Platform, str2);

		if (bIsJava)
		{
			// undo the exe stuff...

			str = _T("");

			SetStrProp(P_ExtOpts_CmdLine, str);
	   		SetStrProp(P_ExtOpts_RebuildOpt, str);
			SetStrProp(P_Caller, str);
		}

		pathTemp.ChangeExtension(".bsc");
		str = pathTemp.GetFileName();
		SetStrProp(P_ExtOpts_BscName, str);

		pathTemp.ChangeExtension(".clw");
		
		// This property is now defunct
		//str = pathTemp.GetFileName();
		//SetStrProp(P_ExtOpts_ClsWzdName, str);

		str = _T("");
		SetStrProp(P_WorkingDirectory, str);
		//SetStrProp(P_Args, str);
		SetIntProp(P_PromptForDlls, 1);
		SetStrProp(P_RemoteTarget, str);

		// set up default Java settings for .class projects
		if (bIsJava) {

			CString strEmpty = _T("");
			CString strTemp;

			// class name
			CString strClassName;
			pathT.GetBaseNameString(strClassName);
			SetStrProp(P_Java_ClassFileName, strClassName);

			// debug using browser/stand-alone interpreter
			SetIntProp(P_Java_DebugUsing, Java_DebugUsing_Standalone);

			// browser name
			// (GetStrProp will set this property as a side effect)
			GetStrProp(P_Java_Browser, strTemp);

			// stand-alone interpreter name
			// (GetStrProp will set this property as a side effect)
			GetStrProp(P_Java_Standalone, strTemp);
					
			// param source: html page or user
			SetIntProp(P_Java_ParamSource, Java_ParamSource_User);

			// HTML page
			SetStrProp(P_Java_HTMLPage, strEmpty);
					
			// temp HTML page
			GetStrProp(P_Java_TempHTMLPage, strTemp);
					
			// args in HTML form
			SetStrProp(P_Java_HTMLArgs, strEmpty);

			// command line args
			// set from command line args
			CString strArgs;
			GetStrProp(P_Args, strArgs);
			SetStrProp(P_Java_Args, strArgs);

			// stand-alone interpreter args
			SetStrProp(P_Java_StandaloneArgs, strEmpty);
						
			// debug stand-alone as applet or application
			SetIntProp(P_Java_DebugStandalone, Java_DebugStandalone_Application);
		}


		SetCurrentConfigAsBase() ;	// copying the current prop bag to the default
									// prop bag
	}
	else
	{
		// This is an internal makefile and we lost the OPT file, or
		// the project stream was out of date.  We'll  just quietly
		// generate default information as needed.

		if (!m_bProjHasNoOPTFile)
		{
			PFILE_TRACE("Couldn't read project stream from OPT file. Using defaults.\n");
		}
		else
		{
			PFILE_TRACE("As expected, we couldn't read project stream from OPT file.\n");

		}

		ASSERT_VALID (this);

		// we set the default configuration to  the first 'debug' one
		// or as read from the makefile
		CString strConfig;
		if ((GetStrProp(P_ProjActiveConfiguration, strConfig) != valid) || (strConfig.IsEmpty()))
		{
			if (!m_strProjStartupConfiguration.IsEmpty())
				SetStrProp(P_ProjActiveConfiguration,m_strProjStartupConfiguration);
			else
				SetStrProp(P_ProjActiveConfiguration,m_strProjDefaultConfiguration);
		}
	}

	// N.B. Can't touch m_bPrivateDataInitialized yet! (tree control uses it)
	m_bPrivateDataInitialized = TRUE;
	m_bNotifyOnChangeConfig = TRUE;
}
///////////////////////////////////////////////////////////////////////////////
void CProject::InitPrivateDataForExe ()
{
	// Currently this just does standard external makefile initialization.
	//
	// At some point, we may want to do something different, which is why
	// this method exists.
	InitPrivateData();
}

#ifdef VB_MAKEFILES

// vb project support
typedef enum {VBFILETYPE_NONE, VBFILETYPE_FORM, VBFILETYPE_MODULE, VBFILETYPE_CLASS, VBFILETYPE_DOCOBJ, VBFILETYPE_USERCTL, VBFILETYPE_PROPPAGE, VBFILETYPE_RES, VBFILETYPE_DESIGNER, VBFILETYPE_LASTVALUE } VBFILETYPE;


VBFILETYPE vbFileTypeFromHeaderName(CString headerName)
{
	if (headerName == "Form")
		return VBFILETYPE_FORM;
	else if (headerName == "Module")
		return VBFILETYPE_MODULE;
	else if (headerName == "Class")
		return VBFILETYPE_CLASS;
	else if (headerName == "UserDocument")
		return VBFILETYPE_DOCOBJ;
	else if (headerName == "UserControl")
		return VBFILETYPE_USERCTL;
	else if (headerName == "PropertyPage")
		return VBFILETYPE_PROPPAGE;
	else if (headerName == "ResFile32")
		return VBFILETYPE_RES;
	else if (headerName == "Designer")
		return VBFILETYPE_DESIGNER;
	else return VBFILETYPE_NONE;
}

int vbProjTypeFromHeaderName(CString value)
{
	if (value == "Type=Exe" || value == "Type=OleExe" )
		return 0;

	else if ( value == "Type=Control" )
		return 1;

	else if (value == "Type=OleDll"   )
		return 2;
	else
		return 0;
}

CString headerName(CString headerLine)
{
	return headerLine.Left(headerLine.Find('='));
}

CString headerValue(CString headerLine)
{
	CString result, newResult;

	result = headerLine.Right(headerLine.GetLength() - headerLine.ReverseFind('=') - 1);
	// remove quotation marks if necessary
	if (result[0] == '\"')
		result = result.Mid(1, result.GetLength() - 2);
	return result;
}

CString getStringFromLine(CString headerLine)
{
	CString result;

	result = headerLine.Right(headerLine.GetLength() - headerLine.ReverseFind('=') - 1);
	// remove quotation marks if necessary
	if (result[0] == '\"')
		result = result.Mid(1, result.GetLength() - 2);
	return result;
}

BOOL getBoolFromLine(CString headerLine)
{
	CString result;

	result = headerLine.Right(headerLine.GetLength() - headerLine.ReverseFind('=') - 1);
	// remove quotation marks if necessary
	if (result[0] == '0')
		return FALSE;
	return TRUE;
}

int getIntFromLine(CString headerLine)
{
	CString result;

	result = headerLine.Right(headerLine.GetLength() - headerLine.ReverseFind('=') - 1);
	// remove quotation marks if necessary
	if (result[0] == '0')
		return FALSE;
	return TRUE;
}

void CProject::OpenVBProject(const TCHAR *szFileName)
{
	CStdioFile projectFile(szFileName, CFile::modeRead | CFile::typeText);
	CString projectData, relativePath;
	CDir vbpDir;
	CPath filePath;
	VBFILETYPE fileType;
	int projType=0;
	
    int i;
	BOOL filesFound[VBFILETYPE_LASTVALUE];
	for(i=0;i<VBFILETYPE_LASTVALUE;i++)
		filesFound[i] = FALSE;

	
	CString vbExeName;
	CString vbExeRelativePath; 
	CString vbProjName;
	
	CStringList vbProjFileNames;
	m_pVBinfo = new VBInfo;

	vbpDir.CreateFromPath(szFileName);
	while (projectFile.ReadString(projectData)) {
		CString projectHeaderName = headerName(projectData);
		fileType = vbFileTypeFromHeaderName(projectHeaderName);
		if (fileType != VBFILETYPE_NONE) {
			if (fileType == VBFILETYPE_FORM || fileType == VBFILETYPE_DOCOBJ || fileType == VBFILETYPE_USERCTL || fileType == VBFILETYPE_PROPPAGE|| fileType == VBFILETYPE_RES|| fileType == VBFILETYPE_DESIGNER)
				relativePath = headerValue(projectData);
			else if (fileType == VBFILETYPE_MODULE || fileType == VBFILETYPE_CLASS)
				relativePath = projectData.Right(projectData.GetLength() - projectData.ReverseFind(';') - 2);

			filePath.CreateFromDirAndFilename(vbpDir, relativePath);
			vbProjFileNames.AddTail(filePath.GetFullPath());
			filesFound[fileType] = TRUE;

		} 
		else {
			if (projectHeaderName == "ExeName32"){
				vbExeName = headerValue(projectData);
				m_pVBinfo->m_ExeName32 = getStringFromLine(projectData);
			}
			else if (projectHeaderName == "Path32"){
				vbExeRelativePath = headerValue(projectData);
				m_pVBinfo->m_Path32 = getStringFromLine(projectData);
			}
			else if (projectHeaderName == "Name"){
				vbProjName = headerValue(projectData);
				m_pVBinfo->m_Name = getStringFromLine(projectData);
			}
			else if (projectHeaderName == "Type") {
				projType = vbProjTypeFromHeaderName(projectData);
				m_pVBinfo->m_Type = getStringFromLine(projectData);
			}
			else if (projectHeaderName == "IconForm")
				m_pVBinfo->m_IconForm = getStringFromLine(projectData);
			else if (projectHeaderName == "Startup")
				m_pVBinfo->m_Startup = getStringFromLine(projectData);
			else if (projectHeaderName == "HelpFile")
				m_pVBinfo->m_HelpFile = getStringFromLine(projectData);
			else if (projectHeaderName == "Title")
				m_pVBinfo->m_Title = getStringFromLine(projectData);
			else if (projectHeaderName == "Command32")
				m_pVBinfo->m_Command32 = getStringFromLine(projectData);
			else if (projectHeaderName == "HelpContextID")
				m_pVBinfo->m_HelpContextID = getStringFromLine(projectData);
			else if (projectHeaderName == "CompatibleMode")
				m_pVBinfo->m_CompatibleMode = getStringFromLine(projectData);
			else if (projectHeaderName == "MajorVer")
				m_pVBinfo->m_MajorVer = getBoolFromLine(projectData);
			else if (projectHeaderName == "MinorVer")
				m_pVBinfo->m_MinorVer = getBoolFromLine(projectData);
			else if (projectHeaderName == "RevisionVer")
				m_pVBinfo->m_RevisionVer = getBoolFromLine(projectData);
			else if (projectHeaderName == "AutoIncrementVer")
				m_pVBinfo->m_AutoIncrementVer = getBoolFromLine(projectData);
			else if (projectHeaderName == "ServerSupportFiles")
				m_pVBinfo->m_ServerSupportFiles = getBoolFromLine(projectData);
			else if (projectHeaderName == "VersionCompanyName")
				m_pVBinfo->m_VersionCompanyName = getStringFromLine(projectData);
			else if (projectHeaderName == "CompilationType")
				m_pVBinfo->m_CompilationType = getBoolFromLine(projectData);
			else if (projectHeaderName == "OptimizationType")
				m_pVBinfo->m_OptimizationType = getBoolFromLine(projectData);
			else if (projectHeaderName == "FavorPentiumPro(tm)")
				m_pVBinfo->m_FavorPentiumPro = getBoolFromLine(projectData);
			else if (projectHeaderName == "CodeViewDebugInfo")
				m_pVBinfo->m_CodeViewDebugInfo = getBoolFromLine(projectData);
			else if (projectHeaderName == "NoAliasing")
				m_pVBinfo->m_NoAliasing = getBoolFromLine(projectData);
			else if (projectHeaderName == "BoundsCheck")
				m_pVBinfo->m_BoundsCheck = getBoolFromLine(projectData);
			else if (projectHeaderName == "OverflowCheck")
				m_pVBinfo->m_OverflowCheck = getBoolFromLine(projectData);
			else if (projectHeaderName == "FlPointCheck")
				m_pVBinfo->m_FlPointCheck = getBoolFromLine(projectData);
			else if (projectHeaderName == "FDIVCheck")
				m_pVBinfo->m_FDIVCheck = getBoolFromLine(projectData);
			else if (projectHeaderName == "UnroundedFP")
				m_pVBinfo->m_UnroundedFP = getBoolFromLine(projectData);
			else if (projectHeaderName == "StartMode")
				m_pVBinfo->m_StartMode = getBoolFromLine(projectData);
			else if (projectHeaderName == "Unattended")
				m_pVBinfo->m_Unattended = getBoolFromLine(projectData);
			else if (projectHeaderName == "ThreadPerObject")
				m_pVBinfo->m_ThreadPerObject = getBoolFromLine(projectData);
			else if (projectHeaderName == "MaxNumberOfThreads")
				m_pVBinfo->m_MaxNumberOfThreads = getIntFromLine(projectData);
		}
	}
	projectFile.Close();

	CDir exeDir;
	CString relativeFilename;
	
	if (vbExeRelativePath.IsEmpty())
		relativeFilename = vbExeName;
	else relativeFilename = vbExeRelativePath + "\\" + vbExeName;

	CPath vbExeFullPath;
	vbExeFullPath.CreateFromDirAndFilename(m_ActiveTargDir, relativeFilename);
	
	// then make this a makefile/vb project
	
	// Reading an external makefile
	HBLDTARGET hDebugTarg = NO_TARGET;
	HBLDTARGET hReleaseTarg;
	CString strTypeName;
	strTypeName.LoadString(IDS_EXTTARG_PROJTYPE);

	// Create all the external targets
	CString strPlatformName;
	strPlatformName = "Win32";

	// Add debug and release targets
	(void) g_BldSysIFace.AddDefaultTargets
			((const TCHAR *) strPlatformName,
			(const TCHAR *)strTypeName,
			hDebugTarg, hReleaseTarg,
			TRUE, TRUE, TRUE, hDebugTarg
			);

	// At this point loop through creating folders if appropriate (with filters)
	if( filesFound[VBFILETYPE_FORM] == TRUE )
		GetActiveTarget()->AddNewGroup("Forms","frm");
	if( filesFound[VBFILETYPE_MODULE] == TRUE )
		GetActiveTarget()->AddNewGroup("Modules","bas");
	if( filesFound[VBFILETYPE_CLASS] == TRUE )
		GetActiveTarget()->AddNewGroup("Class Modules","cls");
	if( filesFound[VBFILETYPE_DOCOBJ] == TRUE )
		GetActiveTarget()->AddNewGroup("Doc Objects","dob");
	if( filesFound[VBFILETYPE_USERCTL] == TRUE )
		GetActiveTarget()->AddNewGroup("User Controls","ctl");
	if( filesFound[VBFILETYPE_PROPPAGE] == TRUE )
		GetActiveTarget()->AddNewGroup("Property Pages","pag");
	if( filesFound[VBFILETYPE_DESIGNER] == TRUE )
		GetActiveTarget()->AddNewGroup("Designers","dsr");
	if( filesFound[VBFILETYPE_RES] == TRUE )
		GetActiveTarget()->AddNewGroup("Resources","res");

	// Then loop through the files adding them
	HBUILDER hBld = g_BldSysIFace.GetActiveBuilder();
	HFILESET hFileSet = g_BldSysIFace.GetFileSet(hBld, hDebugTarg);
	POSITION pos = vbProjFileNames.GetHeadPosition();
	while (pos != NULL)
	{
		CPath pathFile;
		pathFile.Create(vbProjFileNames.GetNext(pos));
		g_BldSysIFace.AddFile(hFileSet, &pathFile, FALSE);
	}

	// then set the debug and make information.
	CProjType *pProjType = GetProjType();
	const CPtrArray * pCfgArray = GetConfigArray();
	int cCfgs = pCfgArray->GetSize();
	CProjTempConfigChange cfgChange(this);
	for ( i=0; i < cCfgs; i++)
	{
		cfgChange.ChangeConfig((ConfigurationRecord *)(*pCfgArray)[i]);
		((CProjTypeExternalTarget*)pProjType)->SetDefaultVBTargetOptions(this,GetActiveTarget(),projType,m_pVBinfo);
	}
	m_strVBProjFile = szFileName;
	m_bVB = TRUE;
}
#endif

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
	ReadMakeResults rmr;
	g_bErrorPrompted = FALSE;
	g_pConvertedProject = NULL;
	g_bConvDontTranslate = FALSE;
	g_bNoUI = theApp.m_bRunInvisibly || !g_pAutomationState->DisplayUI(); //ShellOM:State
	CString strMsg;
	CString	strCmdLine;		// holds command line for .exe on command line proj
	CWaitCursor wc ;		// Wait cursor while load is in progress
	BOOL	bProjectDirty = FALSE ;
	CPath::ResetDirMap(); // clear cache for new project
	
	// Dirty until load successfully:
    DirtyProject();

	if (!NewPath.Create (pFileName) || !SetFile (&NewPath) ) return FALSE;
	InformDependants (SN_FILE_NAME);

	// See if this is an exe file.  If so, there's no point in trying
	// to read the makefile.
	if (IsExecutableFile(pFileName))
	{
		m_bProjIsExe = TRUE;

		if (!theApp.m_argList.IsEmpty())
		{
			// indicates we are processing an exe on the command line!
			strCmdLine = theApp.m_lpCmdLine;
			CString	strDoc = theApp.m_strCurArg;

			strCmdLine.MakeUpper();
			strDoc.MakeUpper();

			int	ich = strCmdLine.Find(strDoc);

			ASSERT ( ich != -1 );	// we must find the substring in this case
			if ( ich >= 0 )
			{
				// get past the doc name
				ich += strDoc.GetLength();
				// skip one (and only one) blank after the doc name
				if (ich < strCmdLine.GetLength() && (_istspace(strCmdLine[ich]) || 
					(ich!=0 && !IsDBCSLeadByte(strCmdLine[ich-1]) && strCmdLine[ich] == _T('"'))))
					++ich;
				// extract the rest of the command line	after restoring the
				// command line to original form
				strCmdLine = theApp.m_lpCmdLine;
				CString	strT = strCmdLine.Right(strCmdLine.GetLength() - ich);
				strCmdLine = strT;
			}
		}
	}

#ifdef VB_MAKEFILES
	else if( !_tcsicmp( NewPath.GetExtension(),".vbp") )
	{
		// attempt to make a makefile type project out of this vb project
		OpenVBProject(NewPath.GetFullPath());
		
		NewPath.ChangeExtension(".dsp");
		SetFile(&NewPath);
		InformDependants (SN_FILE_NAME);
		// Mark project so that we will immediately save it
		m_bProjMarkedForSave = TRUE;
		m_bProjConverted = TRUE;
		m_bProjHasNoOPTFile  = TRUE;
		bProjectDirty = TRUE;
		::SetWorkspaceDocDirty();
	}
#endif

	else
	{
		// disable file registry file change notification during builder load
		//g_FileRegDisableFC = TRUE;

		rmr = ReadMakeFile (bProjectDirty);

		if (rmr == ReadExportedMakefile)
		{
			// if we opened an exported makefile, try again with the new filename
			rmr = ReadMakeFile(bProjectDirty);
		}
		// enable file registry file change notification
		//g_FileRegDisableFC = FALSE;

		if (rmr == ReadError)
		{
			// make sure we scc knows we failed
			if (g_pSccManager->IsSccInstalled() == S_OK)
			{
				g_pSccManager->CancelGetProject(pFileName);
			}

			return FALSE;
		}
		else if (rmr == ReadExternal)
		{
			// No longer is an external makefile

			// make sure scc knows we failed
			if (g_pSccManager->IsSccInstalled() == S_OK)
			{
				g_pSccManager->CancelGetProject(pFileName);
			}

			if ((g_prjcompmgr.GetSupportedPlatformCount () == 1) &&
				(g_prjcompmgr.GetPrimaryPlatform ()->GetUniqueId () == java))
			{
				CProjTempProjectChange projTempProjectChange (NULL);
				MsgBox (Error, IDS_CANT_WRAP_JAVA);
				return FALSE;
			}

			UINT nID = m_bProjConverted ? IDS_CONVERSION_FAILED : IDS_MAKEFILE_BAD_FORMAT;
			CString str1;
			MsgText (str1, nID, (const char *) NewPath);
			if (g_pAutomationState->DisplayUI() && !g_bNoUI) //ShellOM:State
			{
				// Give warning about opening this makefile as an external
				// makefile
				CString str;
				CProjTempProjectChange projTempProjectChange (NULL);
				if (MsgBox (Question, MsgText ( str, IDS_WRAP_MAKEFILE, (const TCHAR *) str1), MB_YESNO) == IDNO)
				{
					return FALSE;
				}
			}
			else
			{
				// This operation is being run by Automation.
				// We cannot put up UI, so we will answer NO to the prompt.
				g_pAutomationState->SetError(DS_E_PROJECT_EXTERNAL_MAKEFILE) ;
				if (theApp.m_bInvokedCommandLine)
				{
					theApp.WriteLog(str1, TRUE);
				}
				return FALSE ;
			}

			// come up with a name for this project
			CString strProjectName;
			NewPath.GetBaseNameString(strProjectName);
 			SetTargetName(strProjectName);
			ASSERT(GetPropBagCount() == 0); // should be cleaned up by now

			m_bProjConverted = m_bConvertedVC20 = FALSE;

 			// Put up the platforms dialog if we have more than one
			// platform installed on the system
			CStringList lstPlatforms;
			if (g_prjcompmgr.GetSupportedPlatformCount() > 1)
			{
				CPlatformsDlg dlg(&lstPlatforms);
				CProjTempProjectChange projTempProjectChange (NULL);
				if (dlg.DoModal() != IDOK)
					return FALSE;
			}
			else
			{
				// Create external targets for the primary platform
				CPlatform* pPlatform = g_prjcompmgr.GetPrimaryPlatform();
				const CString * pstrPlatformName = pPlatform->GetUIDescription();
				lstPlatforms.AddTail(*pstrPlatformName);
			}

			// Reading an external makefile
			HBLDTARGET hDebugTarg = NO_TARGET;
			HBLDTARGET hReleaseTarg;
			CString strTypeName;
			strTypeName.LoadString(IDS_EXTTARG_PROJTYPE);

            // Create all the external targets
			CString strPlatformName;
			POSITION pos = lstPlatforms.GetHeadPosition();
			while (pos != NULL)
			{
				strPlatformName = lstPlatforms.GetNext(pos);
			
				// Add debug and release targets
				(void) g_BldSysIFace.AddDefaultTargets
						((const TCHAR *) strPlatformName,
						(const TCHAR *)strTypeName,
						hDebugTarg, hReleaseTarg,
						TRUE, TRUE, TRUE, hDebugTarg
						);
			}

			// Add the external makefile as a file in the wrapper internal
			// makefile.
			HBUILDER hBld = g_BldSysIFace.GetActiveBuilder();
			HFILESET hFileSet = g_BldSysIFace.GetFileSet(hBld, hDebugTarg);
			g_BldSysIFace.AddFile(hFileSet, &NewPath, FALSE);
			NewPath.ChangeExtension(".dsp");
			SetFile(&NewPath);
			InformDependants (SN_FILE_NAME);

			// Mark project so that we will immediately save it
 			m_bProjMarkedForSave = TRUE;
			m_bProjConverted = TRUE;
			m_bProjHasNoOPTFile  = TRUE;
			bProjectDirty = TRUE;
			::SetWorkspaceDocDirty();

			// Flag that the current project was created by converting
			// an external makefile
			m_bProjExtConverted = TRUE;

			// Force us to ignore the OPT file if one exists

			// Try to get info from any existing OPT file
			//ConvertOPTFile(&lstDebugConfigs, &lstReleaseConfigs);
 		}
		else if (rmr == ReadInternal)
		{
		}
		else
		{
			ASSERT(FALSE);
			// make sure scc knows we failed
			if (g_pSccManager->IsSccInstalled() == S_OK)
			{
				g_pSccManager->CancelGetProject(pFileName);
			}

			return FALSE;
		}

		if (m_bConvertedVC20)
		{
			// If the imported VC2 project has any browse info turned on, we will ask the user whether
			// it should be turned off.
			//
			BOOL fQueriedUser = FALSE;
			BOOL fTurnOffBrowse;

			const CPtrArray * pCfgArray = GetConfigArray();
			int cCfgs = pCfgArray->GetSize();
			for (int i=0; i < cCfgs; i++)
			{
				ConfigurationRecord *pConfiguration = (ConfigurationRecord *)pCfgArray->GetAt(i);
				ASSERT_VALID(pConfiguration);

				POSITION pos = pConfiguration->GetActionList()->GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					CActionSlob * pAction = (CActionSlob *)
											pConfiguration->GetActionList()->GetNext(pos);
					ASSERT_VALID(pAction);

					pAction->m_dwEvent = FALSE;	// assume browse is off
					if (pAction->m_pTool != (CBuildTool *)NULL)
					{
						pAction->m_pTool->OnActionEvent(ACTEVT_QueryBrowse, pAction);
					}
					if (!pAction->m_dwEvent)
					{
						continue;	// ignore this action (browsing not turned on)
					}

					if (!fQueriedUser)
					{
						CString strMsg;
						strMsg.LoadString(IDS_TURN_OFF_BROWSE);
						CProjTempProjectChange projTempProjectChange (NULL);
						fTurnOffBrowse = (AfxMessageBox(strMsg, MB_YESNO) == IDYES);
						fQueriedUser = TRUE;
					}

					if (fTurnOffBrowse)
					{
						// Turn off compiler browsing for this action
						CProjTempConfigChange projTempConfigChange(this);
						projTempConfigChange.ChangeConfig(pConfiguration);

						pAction->m_dwEvent = FALSE;
						pAction->m_pTool->OnActionEvent(ACTEVT_SetBrowse, pAction);
						pAction->DirtyCommand();
					}
					else
					{
						// Turn on bscmake for this config

						CProjTempConfigChange projTempConfigChange(this);
						projTempConfigChange.ChangeConfig(pConfiguration);

						// get our browser database tool and create our action
						CBuildTool * pTool;
						VERIFY(g_prjcompmgr.LookupBldSysComp(
							GenerateComponentId(idAddOnGeneric, BCID_Tool_BscMake),
							(CBldSysCmp *&)pTool));
						CActionSlob action(this, pTool);
						pTool->SetDeferredAction(&action, dabNotDeferred);

						// we're done with this config
						break;
					}
				}
			}
		}

		RecheckReadOnly();	// See if the .MAK file is read-only
	}

    CleanProject();

    // UNDONE (colint): We need to get this to work properly under the
    // new project loading mechanism
 	//if (!m_bProjExtConverted && bLoadOPTFile)
 	//	LoadOPTFiles ();

	PFILE_TRACE ("Initialzing makefile from %s. ReadMakeFile returned %i, "
		"m_bPrivateDataInitialized = %i\n", pFileName, rmr,
		m_bPrivateDataInitialized );

	// add our deferred commands to the menu
	UpdateDeferredCmds(FALSE);

	#ifdef _DEBUG
	BOOL b = FALSE;
	if (b) AfxDump (this);
	#endif

	if (m_bProjIsExe)
 	{
		CString str;
		str.LoadString (IDS_DEBUG_CONFIG);
		CreateConfig(str);
		g_BldSysIFace.GetTarget(str,(HBUILDER)this);

		// we will always initialize P_Args here
		SetStrProp(P_Args, strCmdLine);
	}

	// Make sure that we have initialised the current target on view
//	if (m_pPSWnd->m_TreeWnd.GetContainer() == (CSlob *)NULL)
//	{
		// Tree-control
		//SetActiveConfig(m_strProjActiveConfiguration, FALSE);

		// Active configuration
		//InformDependants(P_ProjActiveConfiguration);
//	}

	// we can set the project to dirty by now
	if (bProjectDirty)
	{
		// set project as dirty
        DirtyProject();
	}

	BOOL bFoundNonJavaProject = FALSE;
	CString str, strProjects;
	if (m_bConvertedDS4x)
	{
		ASSERT(IsKindOf(RUNTIME_CLASS(CProject)));
		CProject * pProject;
		CProject::InitProjectEnum();
		while ((pProject = (CProject *)CProject::NextProjectEnum(str, FALSE)) != NULL)
		{
			// only consider the recently converted ones
			if (pProject->m_bConvertedDS4x)
			{
				pProject->m_bProjectComplete = TRUE;	// projects are now ok to use...
				// get the current platform
				CPlatform* pPlatform = pProject->GetCurrentPlatform();
				ASSERT(pPlatform != NULL);
				if ((pPlatform == NULL) || (pPlatform->GetUniqueId() != java))
				{
					bFoundNonJavaProject = TRUE; // found non-Java project
				}

				strProjects += '\t';
				strProjects += *pProject->GetFilePath();
				strProjects += '\n';
			}
		}
	}
	else
	{
		bFoundNonJavaProject = TRUE; // no Java until 4.x
 		m_bProjectComplete = TRUE;	// project is now ok to use...
		strProjects += '\t';
		strProjects += *GetFilePath();
		strProjects += '\n';
	}

	// skip this dialog when doing minor conversion from 5.x
	if ((m_bProjConverted) && (rmr == ReadInternal) && (!m_bConvertedDS5x))
	{
		CString strOut, str2, str3, str4, str5;
		MsgText(strOut, IDS_CONVERTED1, NewPath.GetFileName());
		strOut += strProjects;

		if (bFoundNonJavaProject) // if not just Java
		{
			MsgText(str2, IDS_CONVERTED2, (LPCSTR)BUILDER_EXT, (LPCSTR)BUILDER_EXT);
			MsgText(str3, IDS_CONVERTED3);
			MsgText(str4, IDS_CONVERTED4);
			MsgText(str5, IDS_CONVERTED5);
			strOut += str2 + str3 + str4 + str5;
		}
		CProjTempProjectChange projTempProjectChange (NULL);
		MsgBox(Information, strOut);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void CProject::GetOPTFileName(CPath & OptPath)
{
 	const CPath *pPath;
	pPath = GetFilePath ();
	ASSERT (pPath);
	//	Get app to save OPT file:
	OptPath = *pPath;
	ProjNameToOptName (OptPath);
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::DoSave (BOOL /* bSavePrivate */ )
{
	ASSERT (!m_bProjIsExe);
	if( m_bProjIsExe ) 
		return TRUE;

	// This will change the active project.  The destructor will change it back.
	CProjTempProjectChange projTempProjectChange (this); 
	
	ASSERT (GetFilePath() != NULL);	// Should have a file name by now.

	// Only save if the project is dirty
	if (!IsDirty() && !g_bExportMakefile)
	{
		// reset flag for next time
		g_bWriteProject = FALSE;
		return TRUE;
	}

	CPath path = *GetFilePath();
	CIgnoreFile ignoreDspFile(GetFileChange(), path, BldSrcFileChangeCallBack);

	// just in case, do the .mak file too
	path.ChangeExtension(_T(".mak"));
	CIgnoreFile ignoreMakFile(GetFileChange(), path, BldSrcFileChangeCallBack);

	// disable file registry file change notification during builder save
	BOOL bOldDisable = g_FileRegDisableFC;
	g_FileRegDisableFC = TRUE;

#if 0
	// FUTURE: this should really be handled using notifications
	BOOL bIsSccInstalled = (g_pSccManager->IsSccInstalled() == S_OK);
	if (bIsSccInstalled)
	{
		g_pSccManager->OnProjSave((HPROJECT)this); // may dirty project
	}
#endif

	ASSERT(g_bExportMakefile || g_bWriteProject || g_bExportDeps);
	if (!g_bExportMakefile && !g_bExportDeps)
		g_bWriteProject = TRUE; // default

	// always write external makefile if user requests this behavior
	if (g_bAlwaysExportMakefile)
		g_bExportMakefile = TRUE;

 	CProjType *pProjType = GetProjType();
	if( pProjType ) {
		if ( pProjType->GetUniqueTypeId() == CProjType::exttarget ||
			 pProjType->GetUniqueTypeId() == CProjType::javaapplet 
		   )
		{
			// can't do this for makefile projects or java projects
			g_bExportMakefile = FALSE;
		}
	}


	CPath pathDep = *GetFilePath();
	pathDep.ChangeExtension("dep");
	// always write a .dep file if we're writing a .mak and it doesn't exist
	if (g_bExportMakefile && !pathDep.ExistsOnDisk())
		g_bExportDeps = TRUE;

	//
	// review(tomse): This is the slow part!
	//
	if (g_bExportDeps)
	{
		CWaitCursor waitCur;

		// Calling BeginCache/EndCache prevents scanner cache from being cleared
		// in-between configurations.  This prevents files from being scanned for
		// each configuration.
		//
		g_ScannerCache.BeginCache();

		const CPtrArray * pCfgArray = GetConfigArray();
		int icfg, size = pCfgArray->GetSize();

//
// #ifdef USE_LATEST_IDB code is used if CFileItem::ScanDependencies supports
// adding minimal rebuild dependencies to the scanner cache.
//
	if (!m_bProj_PerConfigDeps)	// fast export if not per-config deps
	{
		//
		// review(tomse) : checked in disabled because of risk that special files
		// such as the .pch that are configuration specific could be erroneously added
		// as a dependency to another configuration.  This would be bad for files that
		// were dependent only one the source and the pch files.
		//
		int iLatestBuiltCfg = -1; // default if no .idb
		int iSupportedConfig = -1;
		FILETIME ftLatest = {0,0};

		//
		// Find configuration with most recent .idb first
		//
		for (icfg = 0; icfg < size; icfg++)
		{
			ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(icfg);
			CProjTempConfigChange projTempConfigChange(this);
			projTempConfigChange.ChangeConfig(pcr);

			if ((iSupportedConfig == -1) && (pcr->IsSupported()) && (pcr->IsBuildable()))
				iSupportedConfig = icfg;

			CPath pathIdb = GetMrePath(pcr);
			FileRegHandle frh = CFileRegFile::LookupFileHandleByName(pathIdb);
			if (NULL!=frh)
			{
				FILETIME ft;
				if (frh->GetFileTime(ft))
				{
					//
					// New canidate for most recent build
					//
					if (ft > ftLatest)
					{
						iLatestBuiltCfg = icfg;
						ftLatest = ft;
					}
				}
				frh->ReleaseFRHRef();
			}
		}

		// make sure we have a good default
		if (iLatestBuiltCfg == -1)
			iLatestBuiltCfg = (iSupportedConfig == -1) ? 0 : iSupportedConfig;

		//
		// Scan configuration with most recent .idb first
		//
		if (iLatestBuiltCfg != m_nScannedConfigIndex)
		{
			m_nScannedConfigIndex = iLatestBuiltCfg;
			ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(iLatestBuiltCfg);
			CProjTempConfigChange projTempConfigChange(this);
			projTempConfigChange.ChangeConfig(pcr);
			ScanDependencies(TRUE,FALSE);
		}
	}
	else	// if per-config deps, get the rest too
	{
	// don't need this with fast exports
		//
		// Update dependencies for all configurations using any method available before exporting
		// makefile.
		//
		for (icfg = 0; icfg < size; icfg++)
		{
#ifdef USE_LATEST_IDB
			// Skip iLatestBuiltCfg since it has alredy been scanned.
			if (icfg==iLatestBuiltCfg)
				continue;
#endif

			ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(icfg);
			CProjTempConfigChange projTempConfigChange(this);
			projTempConfigChange.ChangeConfig(pcr);

			ScanDependencies(TRUE,FALSE);
		}
	}	// if per-config deps
	}

	BOOL bRetVal = TRUE;
	// Rewrite the builder's makefile.
	if (g_bExportMakefile || g_bWriteProject)
	{
		if (g_buildfile.WriteBuildFile(this))
			CleanProject();
		else
			bRetVal = FALSE;
	}

	if (g_bExportDeps)
	{
		g_ScannerCache.EndCache();
	}

	// reset flags for next time
	g_bWriteProject = g_bExportMakefile = g_bExportDeps = FALSE;

	// update timestamp now so we'll be sure to ignore bogus file change notification
	((CFileRegFile *)g_FileRegistry.GetRegEntry(GetFileRegHandle()))->RefreshAttributes();

	// enable file registry file change notification
	g_FileRegDisableFC = bOldDisable;

	return bRetVal;
}

static UINT projIDS[] = { IDS_FILTER_PROJECTS, 0 };

class CTempProjDoc : public CPartDoc
{
public:
	CTempProjDoc() { }
	~CTempProjDoc() { }
	virtual BOOL GetValidFilters(UINT** paIDS, UINT * pnInitial) { *paIDS = projIDS; *pnInitial = 0; return TRUE; }
};

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::DoSaveAs (BOOL /* bSavePrivate */)
{
	ASSERT (!m_bProjIsExe);
	ASSERT (!m_strProjItemName.IsEmpty ());

	CPath NewPath;
	CString newName;

	if (GetFilePath())
	{
		newName = (const char *) *GetFilePath ();
	}
	else
	{
		newName = m_strProjItemName;

		if (newName.GetLength() > 8)
			newName.ReleaseBuffer(8);
		int iBad = newName.FindOneOf(" #%;/\\");    // dubious filename
		if (iBad != -1)
			newName.ReleaseBuffer(iBad);

		ASSERT ( iBad == -1 || iBad > 0 );
		// append the default suffix if there is one
	}

	if (m_bProjExtConverted)
	{
		// Change default so that it is not the same as the existing external
		// makefile
        if (!NewPath.Create(newName))
            return FALSE;

        NewPath.PostFixNumber();
        NewPath.ChangeExtension(_T(BUILDER_EXT));
        newName = (const char *)NewPath;
	}

	// Try to get an ok file name to use to do the save
	CPath OriginalPath = NewPath;
	BOOL bNameOK;
	do
	{
		bNameOK = TRUE;

		// Do prompt will add the extension .bld even if an extension already exists
		if (!NewPath.Create(newName))
			return FALSE;

 
        NewPath.ChangeExtension(_T("."));
        newName = (const char *)NewPath;

		// UNDONE: need a doc template for last argument
#if 0
		if (!AfxGetApp()->DoPromptFileName(newName, AFX_IDS_SAVEFILE,
			OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOREADONLYRETURN, FALSE, NULL))
 			return FALSE;
#else
		UINT nSaveType;
		CTempProjDoc projDoc;
		if (!theApp.DoPromptFileSave(newName, AFX_IDS_SAVEFILE,
			OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOREADONLYRETURN, &projDoc, &nSaveType))
 			return FALSE;
#endif

		
		if (!NewPath.Create(newName))
			return FALSE;

		NewPath.ChangeExtension(_T(BUILDER_EXT));
   
		// If we are saving an internal makefile created by loading an external makefile
		// then we do NOT allow the user to save using the file name of the external
		// makefile.
		if (m_bProjExtConverted && NewPath == *GetFilePath())
		{
			CString strMsg;
			MsgBox(Information, MsgText(strMsg, IDS_EXTTARG_BAD_PROJ_NAME, (const TCHAR *) *GetFilePath()));
			bNameOK = FALSE;
	        newName = (const char *)OriginalPath;
			continue;
		}

		if (m_bProjExtConverted)
		{
			// this assumes the all external makefiles generate ".exe" 's !!! 
			CPath pathExe = NewPath;

			CString strExe;
			pathExe.ChangeExtension(_T(".exe"));
			strExe = pathExe.GetFileName();

			CTargetItem* pTarget = GetActiveTarget();
			CString strProjectName;

			NewPath.GetBaseNameString(strProjectName);
			SetTargetName(strProjectName);
			ASSERT(pTarget != NULL);
			if (pTarget != NULL)
				pTarget->SetTargetName(strProjectName);

			CString strBsc;
			pathExe.ChangeExtension(_T(".bsc"));
			strBsc = pathExe.GetFileName();

			const CPtrArray * pCfgArray = GetConfigArray();
			for (int nConfig = 0; nConfig < pCfgArray->GetSize(); nConfig++)
			{
				CProjTempConfigChange projTempConfigChange(this);
				projTempConfigChange.ChangeConfig((ConfigurationRecord *)pCfgArray->GetAt(nConfig));

				SetStrProp(P_Proj_Targ, strExe);
				SetStrProp(P_Caller, strExe);
				SetStrProp(P_Proj_BscName, strBsc);

				CString strName;
				pathExe.GetBaseNameString( strName );

				CString strFrom = GetActiveConfig()->GetConfigurationName();

				int nFirst = strFrom.Find(_T('-'));
				ASSERT(nFirst != -1);
				strName += strFrom.Right(strFrom.GetLength() - nFirst + 1 );

				RenameTarget(strFrom, strName);

			}

		}

		// Must set the path after doing the external makefile check
		if (!SetFile(&NewPath))
			return FALSE;
	} while (!bNameOK);

	// SetPathName adds new name to MRU list
	InformDependants(SN_FILE_NAME);

	::SetWorkspaceDocDirty();

	// inform dependents of this builder filename change
	CString str; CTargetItem * pTarget;
	InitTargetEnum();
	while (NextTargetEnum(str, pTarget))
	{
		const CPtrArray * pConfigArray = pTarget->GetConfigArray();
		int nSize = pConfigArray->GetSize();
		
		ConfigurationRecord * pcr = NULL;
		for (int i = 0; i < nSize; i++)
		{
			pcr = (ConfigurationRecord *)pConfigArray->GetAt(i);
			CActionSlob::InformActions(pTarget, P_ToolOutput, pcr);
		}
	}

    // UNDONE (colint): This will be done by the project package
	//if (bSavePrivate)
	//	SaveOPTFiles();

	// do the save
	DoSave();

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
// Get version info for App studio:
void CProject::GetVersionInfo(PROJECT_VERSION_INFO* ppvi)
{
	ppvi->dwBuildType = VS_FF_DEBUG;		// 0 for release.


	int i = GetTargetAttributes ();
	if (i & ImageExe ) 				ppvi->dwTargetType = VFT_APP;
	else if (i & ImageDLL )			ppvi->dwTargetType = VFT_DLL;
	else if (i & ImageStaticLib )	ppvi->dwTargetType = VFT_STATIC_LIB;
	else 							ppvi->dwTargetType = VFT_UNKNOWN;

	ppvi->dwTargetSubtype = VFT2_UNKNOWN;	// only nonzero for font/drv types
	ppvi->dwTargetOS = VOS_NT_WINDOWS32;
	ppvi->dwVersionNumberLS = MAKELONG(1, 0);
	ppvi->dwVersionNumberMS = MAKELONG(0, 1);

	CString str;

	CPath * pPath = GetTargetFileName();
	if (pPath)
	{
		str = pPath->GetFileName();
		delete pPath;
	}

	strncpy(ppvi->szTargetFileName, str, CCHMAX_TARGET_FILE_NAME);
	ppvi->szTargetFileName[CCHMAX_TARGET_FILE_NAME - 1] = '\0';

	// strip off the extension and build the product name field
	int iBad = str.Find('.');
	if (iBad != -1)
		str.ReleaseBuffer(iBad);

	strncpy(ppvi->szTargetProductName, str, CCHMAX_TARGET_PRODUCT_NAME);
	ppvi->szTargetProductName[CCHMAX_TARGET_PRODUCT_NAME - 1] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
// Obtain the toolset for this projects platform type.
INT CProject::GetProjectToolset()
{
	CDirMgr * pDirMgr = GetDirMgr();
	CProjType * pProjType;
	INT nToolset;

	if (m_bProjIsExe)
	{
		CString str;
		CPlatform * pPlatform;

		// FUTURE (colint): Do we still need this for exe projects. Will anyone
		// ever ask for the toolset for an exe project, I think not!
		VERIFY(GetStrProp(P_ExtOpts_Platform, str) == valid);
		g_prjcompmgr.LookupPlatformByUIDescription((const TCHAR *)str, pPlatform);

		// If the current toolset is unsupported then we pick the primary platform as a default
		if (!pPlatform->IsSupported())
			pPlatform = g_prjcompmgr.GetPrimaryPlatform();

		nToolset = pDirMgr->GetPlatformToolset(*(pPlatform->GetName()));
	}
	else
	{
		// Get the toolset for the current project platform
		VERIFY (g_prjcompmgr.LookupProjTypeByName(GetActiveConfig()->GetOriginalTypeName(), pProjType));

		// If the current toolset is unsupported then we pick the primary platform as a default
		if (!pProjType->IsSupported())
		{
			CPlatform * pPlatform = g_prjcompmgr.GetPrimaryPlatform();
			nToolset = pDirMgr->GetPlatformToolset(*(pPlatform->GetName()));
		}
		else
			nToolset = pDirMgr->GetPlatformToolset(*(pProjType->GetPlatformName()));
	}

	return nToolset;
}

///////////////////////////////////////////////////////////////////////////////
// Setup the project's envirnoment variables.  Right now, we just copy them out
// of the dir manager, but this function is here in case we someday want per
// project or per config paths:
void CProject::GetEnvironmentVariables (CEnvironmentVariableList *pEnvList)
{
	CDirMgr * pDirMgr = GetDirMgr();
    CPlatform * pPlatform = GetCurrentPlatform();
	CString str;

	pDirMgr->GetDirListString ( str, DIRLIST_PATH );
	pEnvList->AddVariable ("path", str );

    if (pPlatform && (pPlatform->GetUniqueId() == java))
    {
        // for Java, we "steal" the INCLUDE slot for the classpath...
        pDirMgr->GetDirListString ( str, DIRLIST_INC );
        pEnvList->AddVariable ("classpath" , str );
    }
    else
    {
		pDirMgr->GetDirListString ( str, DIRLIST_INC );
		pEnvList->AddVariable ("include", str );

		pDirMgr->GetDirListString ( str, DIRLIST_LIB );
		pEnvList->AddVariable ("lib",str);
	}

	// Add OUTDIR and INTDIR to environment
	CString strMacro;
	if (GetOutDirString(strMacro, P_OutDirs_Target, &str, FALSE))
		pEnvList->AddVariable (strMacro, str);
	if (GetOutDirString(strMacro, P_OutDirs_Intermediate, &str, FALSE))
		pEnvList->AddVariable (strMacro, str);
}

///////////////////////////////////////////////////////////////////////////////
#ifdef PROJ_LOG
void LogProjectState (
					const CObList &rlist,
					CProjItem	*pItemStart,
					CProject *pProject
					);
#endif

CProject::BuildResults CProject::DoBuild
(
	CProjItem * pItemStart,
	UINT buildType,
	BOOL bContinueAfterErrors,
	FlagsChangedAction & fca,
	CTempMaker & rTM,
	CErrorContext & EC,
	BuildType bt /* = NormalBuild */,
	BOOL bVerbose /* = TRUE */,
	BOOL bRecurse /* = TRUE */
)
{
	//	Called to build part of a project starting at the specified item (which
	//	may be the project itself).  Note that this routine may recurse.
	BuildResults brResult = NoBuild;

    UINT cmdRet;
    FileRegHandle frhStart;
    DWORD aob;
    CProjType * pProjType;
    CString str;

	// This will change the active project.  The destructor will change it back.
	CProjTempProjectChange projTempProjectChange (this); 

	// Spawner should be active:
	ASSERT (g_Spawner.SpawnActive ());

	// Get flattened list of all items, with subprojects at the front, but
	// not expanded
   	CObList lstItems;
	pItemStart->FlattenSubtree(lstItems, bt == Compile ? flt_Normal | flt_ExcludeDependencies :
	                           flt_Normal | flt_ExcludeGroups | flt_RespectItemExclude | flt_ExcludeDependencies | flt_RespectTargetExclude);

    CPtrList lstprojdeprefs;
	CString strCurrentConfig = GetActiveConfig()->GetConfigurationName();

	pProjType = GetProjType();
	BOOL bIsExternalTarget = (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)));
	if (buildType != TOB_Clean && !bIsExternalTarget && CheckForDuplicateTargets(lstItems, IDS_SAME_TARGET) && !bContinueAfterErrors )
	{
		brResult = BuildError;
		goto EndBuild;
	}

	{
		// Build all target dependencies first...
		for (POSITION pos = lstItems.GetHeadPosition(); pos != NULL;)
 		{
			CProjItem * pItem = (CProjItem *) lstItems.GetNext (pos) ;
			if (pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
			{
 				ConfigurationRecord * pcrProjDep = ((CProjectDependency*)pItem)->GetCompatibleTargetConfig(GetActiveConfig());
				if (pcrProjDep != NULL)
				{
					PROJDEPREFINFO * pProjDepRefInfo = new PROJDEPREFINFO;
					pProjDepRefInfo->pProjDep = pItem;
					pProjDepRefInfo->pcr = GetActiveConfig();

					lstprojdeprefs.AddTail(pProjDepRefInfo);

					CPtrList lstOutputs;
					CActionSlob::AddRefFileItem(pItem,&lstOutputs);

					if( bRecurse ){
						CString strConfigName = pcrProjDep->GetConfigurationName();
						void *pDummy=NULL;
						if (!CBuildIt::m_mapConfigurationsBuilt.Lookup(strConfigName, pDummy))
						{
							CBuildIt::m_mapConfigurationsBuilt.SetAt(strConfigName, NULL);

							// Build the target dependency?
							CProject * pDepProject = (CProject *)pcrProjDep->m_pBaseRecord->m_pOwner;
							ASSERT(pDepProject);
							ASSERT(pDepProject->IsKindOf(RUNTIME_CLASS(CProject)));
	
							CProjTempConfigChange tempChange(pDepProject);
							tempChange.ChangeConfig(pcrProjDep);
	
							pProjType = pDepProject->GetProjType();
	
							// If the dependency is on an external target type then it is always
							// out of date.
							CObList lstPath;
							if (pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)) || 
								buildType != TOB_Build ||
								g_BldSysIFace.TargetState(NULL, NULL, NO_TARGET, (HBUILDER)pDepProject) != TRG_STAT_Current
							   )
							{
								// int type;
								//if( buildType == TOB_ReBuild ) type = TOB_Build;
								//else type = buildType;
								brResult = pDepProject->DoBuild (pDepProject,
													buildType,
													bContinueAfterErrors,
													fca,
													rTM,
													EC,
													bt,
													bVerbose);
	
								if (brResult == BuildCanceled ||
									(brResult == BuildError && !bContinueAfterErrors)) 
								{
									// SetStrProp(P_ProjActiveConfiguration, strCurrentConfig);
									goto EndBuild;
								}
							}
						}
						// Stat the outputs here
						POSITION pos = lstOutputs.GetHeadPosition();
						while (pos)
						{
							FileRegHandle frh = (FileRegHandle) lstOutputs.GetNext( pos );
							frh->RefreshAttributes();
						}
					}
				}
			}
		}
	}

	// Continue building the current target
	// SetStrProp(P_ProjActiveConfiguration, strCurrentConfig);
	
	if (buildType != TOB_Clean)
	{
		if(g_bHTMLLog )
			LogTrace("<h3>\n");
		EC.AddString(MsgText(str, IDS_BUILD_TARGETNAME, (const char *) strCurrentConfig));
		if(g_bHTMLLog )
			LogTrace("</h3>\n");
	}
	else
	{
		EC.AddString(MsgText(str, IDS_DESC_CLEANING, (const char *) strCurrentConfig));
	}

	// If this is an external makefile then spawn make and go:
	pProjType = GetProjType();
	if (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
	{

		if (buildType != TOB_Clean)
		{
			brResult = BuildAsExternalMakefile (buildType, EC );
		}
		goto EndBuild;
	}

	ASSERT (!GetContentList()->IsEmpty());

	// \/ Fake v3.0 API hooking

	aob = bContinueAfterErrors ? AOB_Default | AOB_IgnoreErrors : AOB_Default;

	if (bt == Compile)
		aob |= AOB_Forced;  

	// /\ Fake v3.0 API hooking

	if( g_bWriteBuildLog ){
		CString strLog;
		strLog.LoadString(IDS_BUILD_LOG);
		SetPrompt(strLog, TRUE);
		// LogProjectState (lstItems, pItemStart, this);
		SetPrompt("", TRUE);
	}


	frhStart = (FileRegHandle)NULL;
	if (pItemStart->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
		// FUTURE: 
		// short term fix .. build an output
		ConfigurationRecord * pcr = pItemStart->GetActiveConfig();
		if (pcr == (ConfigurationRecord *)NULL)
		{
			brResult = BuildError;
			goto EndBuild;
		}

		CActionSlobList * pActions = pcr->GetActionList();
		if (pActions->IsEmpty())
		{
			brResult = BuildError;
			goto EndBuild;
		}

		ASSERT(pActions->GetCount() == 1);

		CFileRegSet * pfrsOutput = ((CActionSlob *)pActions->GetHead())->GetOutput();
#ifndef REFCOUNT_WORK
		if (pfrsOutput->GetContent()->IsEmpty())
#else
		if (pfrsOutput->IsEmpty())
#endif
		{
			brResult = BuildComplete;
			goto EndBuild;
		}

		pfrsOutput->InitFrhEnum();
		frhStart = pfrsOutput->NextFrh();
#ifdef REFCOUNT_WORK
		// Not sure where this should be released.  Here to be safe.
		if (NULL!=frhStart)
			frhStart->ReleaseFRHRef();
#endif
	}

	cmdRet = g_buildengine.DoBuild
	(
		GetProject()->GetActiveConfig(),
		EC,					// error context
		frhStart,			// file to build
		aob,				// attribs of build, eg. forced?
		buildType,			// build or rebuild
		(bt != Compile),	// recurse
		bVerbose			// verbose
	);

#if 0
#pragma message( "Using temporary code to enable/disable new update dependency code")
	{
		if (g_bNewUpdateDepModel)
		{
			// Get IMreDependencies* so that it is cached.
			IMreDependencies* pMreDepend = NULL;
			CPath pathIdb = GetMrePath(GetActiveConfig());
			if (!pathIdb.IsEmpty())
			{
				// Minimal rebuild dependency interface.
				pMreDepend = 
					IMreDependencies::GetMreDependenciesIFace((LPCTSTR)pathIdb);
			}

			// Update dependencies.
			// Set up a new cache for this target			
			g_ScannerCache.BeginCache();

			CTargetItem* pTarget = GetActiveTarget();

			CObList SubList;
			pTarget->GetBuildableItems(SubList);
			if (!SubList.IsEmpty())
			{
				// Test for a /Yc file, and parse the PCH file first.
				CProjItem *pYcItem = NULL;

				int nTotal = SubList.GetCount();
				int nCurr = 0;
				int nLast = 0;
				BOOL fRefreshDepCtr = FALSE;
				POSITION pos = SubList.GetHeadPosition();
				while (pos)
				{
					CProjItem *pTempItem = (CProjItem *) SubList.GetNext( pos );
					ASSERT( pTempItem->IsKindOf( RUNTIME_CLASS(CProjItem) ) );

					if( pTempItem == pYcItem ) continue;

					ASSERT_VALID( pTempItem );
					nCurr++;
					if (pTempItem->IsKindOf( RUNTIME_CLASS(CFileItem) ))
					{
						pTempItem->ScanDependencies();
						fRefreshDepCtr = TRUE;

						// update the indicator 20 times at most
						int nNew = nCurr * 100 / nTotal;
						if( (nNew/5) > nLast ){
							StatusPercentDone( nNew );
							nLast = nNew/5;
						}
					}
				}
				if (fRefreshDepCtr)
					pTarget->RefreshDependencyContainer();
			}
			g_ScannerCache.EndCache();
			if (NULL!=pMreDepend)
				pMreDepend->Release();
		}
	}
#endif	// Use dependencies??

	switch (cmdRet)
	{
		case CMD_Complete:	brResult = BuildComplete;	break;
		case CMD_Canceled:	brResult = BuildCanceled;	break;
		case CMD_Error:		brResult = BuildError;		break;

		default:
			ASSERT(FALSE);	
			break;
	}

EndBuild:
	CProjTempConfigChange projTempConfigChange(this);

	while (!lstprojdeprefs.IsEmpty())
	{
		PROJDEPREFINFO * pProjDepRefInfo = (PROJDEPREFINFO *)lstprojdeprefs.RemoveHead();
		projTempConfigChange.ChangeConfig(pProjDepRefInfo->pcr);
		CActionSlob::ReleaseRefFileItem(pProjDepRefInfo->pProjDep);
		delete pProjDepRefInfo;
	}
	return brResult;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::TopLevelRunDeferedBuildTool (CSchmoozeTool *pTool, BOOL fAlways /* = FALSE */)
{
//  Called to run a defered build tool like BSCMAKE.  Function sets up spawner
//  and calls RunDeferedBuildTool.
	CErrorContext *pEC;
	DWORD errs,warns;
	CProject::BuildResults br;
	CString str, strName;
	CFileRegSet regset;
	BuildStates bs;
	CEnvironmentVariableList ProjEnvironment;
	CPath * pPath ;


	ASSERT (!m_bProjIsExe);

	if (!theApp.NotifyPackages(PN_QUERY_BEGIN_BUILD) ||
		!g_Spawner.CanSpawn ()) return FALSE;

	// if we can't get the primary output name
	// then just use the filename
	CPtrList plOutput;
	FileRegHandle frh;

	// FUTURE: remove this hack
	CActionSlobList lstActions;
	CActionSlob action(this, pTool);
	lstActions.AddTail(&action);
	pTool->GenerateOutput(AOGO_Default, lstActions, g_DummyEC);

	CFileRegSet * pSet = action.GetOutput();
#ifndef REFCOUNT_WORK
	if (!pSet->GetContent()->IsEmpty())
	{
		frh = (FileRegHandle)pSet->GetContent()->GetHead();
		pPath = (CPath *)g_FileRegistry.GetRegEntry(frh)->GetFilePath();
#else
	if (!pSet->IsEmpty())
	{
		frh = (FileRegHandle)pSet->GetFirstFrh();
		pPath = (CPath *)g_FileRegistry.GetRegEntry(frh)->GetFilePath();
		frh->ReleaseFRHRef();
#endif
		ASSERT(pPath);
		strName = pPath->GetFileName() ;
	}
	else
	{
    	GetStrProp(P_ProjItemName, strName);
	}

	// HACK HACK HACK
	// Since we don't support proper remote targets we have to
	// special case the mecr tool's output name, since it fakes the
	// build system using a pseudo target which we don't really
	// want the user to know about!!!!

	// UNDONE
	if (pTool->IsMecrTool())
	{
		TRACE("UNDONE: Use of CBuildTool::IsMecrTool() in %s @ %d", __FILE__, __LINE__);
		CPath* pPath = GetTargetFileName();
		ASSERT(NULL!=pPath);
		strName = pPath->GetFileName();
		delete pPath;
	}

	// Initialize the spawner and output window:
	ASSERT (!g_Spawner.SpawnActive ());
	if ((pEC = g_Spawner.InitSpawn ()) == NULL ) return FALSE;
	theApp.NotifyPackages (PN_BEGIN_BUILD, (void *) FALSE);

	// Get the build state of the tool.  If a consumable file is missing,
	// then suggest a regular build instead:
	ConfigCacheEnabler Cacher;	// No properties can change

	// What is the build state of this action?
 	bs = fAlways ? OutOfDate : GetBuildStateForTool(pTool, *action.GetInput(), *pEC);

	if (bs == SourceMissing)
	{
		CString str;
		int rv = QuestionBox (IDS_SOURCE_MISSING,
							 MB_YESNO,
						     (const TCHAR *) *pTool->GetName()
						    );
		theApp.NotifyPackages (PN_END_BUILD, (void *) FALSE);
		g_Spawner.TermSpawn ();

		if (rv == IDYES)
		{
			DAB dab = pTool->DeferredAction(&action);

			Cacher.EndCacheNow();
			pTool->SetDeferredAction(&action, dabNotDeferred);
			rv = DoTopLevelBuild (FALSE);

			// reset
			pTool->SetDeferredAction(&action, dab);
			return rv;
		}
		else
			return FALSE;
	}
	else if (bs != Current)
	{
		GetEnvironmentVariables ( &ProjEnvironment );
		ProjEnvironment.SetVariables ();

		CTempMaker TM;

		br = RunSchmoozeTool (pTool, TRUE, GetWorkspaceDir(), TM, *pEC);

		TM.NukeFiles (*pEC );

		ProjEnvironment.ResetVariables ();	// May do nothing.
	}
	g_Spawner.GetErrorCount ( errs, warns );

	pEC->AddString(_T(""));

	if (br == BuildCanceled)
		pEC->AddString(IDS_USERCANCELED);
	else
		pEC->AddString(MsgText(str, IDS_BUILD_COMPLETE, (const char *) strName, errs, warns));
	theApp.NotifyPackages (PN_END_BUILD, (void *) FALSE);
	g_Spawner.TermSpawn ();
	Cacher.EndCacheNow ();
	MessageBeep (UINT(-1));
	return (br == CProject::BuildComplete ) ? TRUE : FALSE;
}
///////////////////////////////////////////////////////////////////////////////
CProject::BuildResults CProject::RunSchmoozeTool
(
	CSchmoozeTool * pTool,
	BOOL /* bContinueAfterErrors */,
	CDir & rMyDir,
	CTempMaker & /*rTM*/,
	CErrorContext & EC
)
{
	CPtrList plCommandLines;
	BuildResults brResult = NoBuild;

	// find the action for this tool
	CActionSlobList * pActions = GetActiveConfig()->GetActionList();
	POSITION pos = pActions->GetHeadPosition();
	CActionSlobList lstActions;
	while (pos != (POSITION)NULL)
	{
		CActionSlob * pAction = (CActionSlob *)pActions->GetNext(pos); 
		if (pAction->m_pTool == pTool)
		{
			BOOL fDummy;
			pAction->RefreshCommandOptions(fDummy);

			lstActions.AddTail(pAction);
			break;
		}
	}

	if (!pTool->GetCommandLines(lstActions,	plCommandLines, 0, EC))
	{
		// display the notification that the
		// 'get command-line' operation failed
		CString str;
		EC.AddString (::MsgText (
								str,
								IDS_GET_COMMAND_LINE_FAILED,
								(const TCHAR *) *pTool->GetName ()
								));
		// log this as an error
		g_Spawner.m_dwProjError++;
	}
	// nothing to do?
	else if (!plCommandLines.IsEmpty())
	{

		// execute them...
		UINT cmd = g_Spawner.DoSpawn(plCommandLines, rMyDir, FALSE, FALSE, EC);

		if (cmd != CMD_Canceled && cmd != CMD_Complete)
			cmd = CMD_Error; 

		if (cmd == CMD_Canceled)
			brResult = BuildCanceled;

		else if (cmd == CMD_Complete)
			brResult = BuildComplete;

		else if (cmd == CMD_Error)
			brResult = BuildError;
	}

	// clean up to avoid leaks
	while (!plCommandLines.IsEmpty())
	{
		CCmdLine * pCmdLine = (CCmdLine *)plCommandLines.RemoveTail();
		delete pCmdLine;
	}

	return brResult;
}
///////////////////////////////////////////////////////////////////////////////
BuildStates CProject::GetBuildStateForTool
(
	CSchmoozeTool * pSTool,
	CFileRegSet & regset,
	CErrorContext& /* EC =g_DummyEC */
)
{
	FILETIME ft, ftConsumable;
	BOOL bHaveConsumables = FALSE;

 	// Flush any pending file change events
	FileChangeDeQ();

	//and possible dep update
	FileItemDepUpdateDeQ();

	// Do we have any consumables that the tool wants?
	FileRegHandle frh;
	regset.InitFrhEnum();
	while ((frh = regset.NextFrh()) != (FileRegHandle)NULL)
	{
		if (pSTool != NULL &&
			!pSTool->IsConsumableFile (g_FileRegistry.GetRegEntry(frh)->GetFilePath()))
#ifndef REFCOUNT_WORK
			continue;
#else
		{
			frh->ReleaseFRHRef();
			continue;
		}
		frh->ReleaseFRHRef();
#endif

		bHaveConsumables = TRUE;
	}

	// If we don't have any consumables then the tool is upto date
	if (!bHaveConsumables)
	{
		#ifdef PROJ_LOG
		PBUILD_TRACE ("Project \"%s\" build state declared current for"
			" tool %s\n", (const TCHAR *) *GetFilePath (), (const TCHAR *) *pSTool->GetName());
		#endif
		return Current;
	}

	// Compare each of the consumables files and get the time of the newest consumable
	//
	ftConsumable.dwLowDateTime = 0;
	ftConsumable.dwHighDateTime = 0;
	regset.InitFrhEnum();
	while ((frh = regset.NextFrh()) != (FileRegHandle)NULL)
	{
		if (pSTool != NULL &&
			!pSTool->IsConsumableFile (g_FileRegistry.GetRegEntry(frh)->GetFilePath()))
#ifndef REFCOUNT_WORK
			continue;
#else
		{
			frh->ReleaseFRHRef();
			continue;
		}
#endif

		if (!g_FileRegistry.GetRegEntry(frh)->GetFileTime(ft))
		{
			#ifdef PROJ_LOG
			PBUILD_TRACE ("Project \"%s\" build state declared SourceMissing for"
				" tool %s because of consumable \"%s\"\n",
				(const TCHAR *) *GetFilePath (),
				(const TCHAR *) *pSTool->GetName(),
				(const TCHAR *) *g_FileRegistry.GetRegEntry(frh)->GetFilePath());
			#endif
#ifdef REFCOUNT_WORK
			frh->ReleaseFRHRef();
#endif

			return SourceMissing;
		}

		if (CompareFileTime(&ft, &ftConsumable) > 0)
			ftConsumable = ft;

#ifdef REFCOUNT_WORK
			frh->ReleaseFRHRef();
#endif
	}

	CFileRegSet * pregset = GetTargetPaths();
	pregset->InitFrhEnum();
	while ((frh = pregset->NextFrh()) != (FileRegHandle)NULL)
	{
		// Skip files tool doesn't make:	
 		if (pSTool != NULL &&
			!pSTool->IsProductFile (g_FileRegistry.GetRegEntry(frh)->GetFilePath()))
#ifndef REFCOUNT_WORK
			continue;
#else
		{
			frh->ReleaseFRHRef();
			continue;
		}
#endif

		if (!g_FileRegistry.GetRegEntry(frh)->GetFileTime(ft))
		{		
			#ifdef PROJ_LOG
			PBUILD_TRACE ("Project \"%s\" build state declared OutOfDate for"
				" tool %s because couldn't access target \"%s\"\n",
				(const TCHAR *) *GetFilePath(),
				(const TCHAR *) *pSTool->GetName(),
				(const TCHAR *) *g_FileRegistry.GetRegEntry(frh)->GetFilePath());
			#endif
#ifdef REFCOUNT_WORK
			frh->ReleaseFRHRef();
#endif
			return OutOfDate	;
		}
  		// Compare each target against newest consumable:
		if (CompareFileTime (&ft, &ftConsumable) < 0) // intentionally "< 0" not "<= 0"
		{
			#ifdef PROJ_LOG
			PBUILD_TRACE ("Project \"%s\" build state declared OutOfDate for"
				" tool %s because of consumable \"%s\"\n",
				(const TCHAR *) *GetFilePath(),
				(const TCHAR *) *pSTool->GetName(),
				(const TCHAR *) *g_FileRegistry.GetRegEntry(frh)->GetFilePath());
 			#endif
#ifdef REFCOUNT_WORK
			frh->ReleaseFRHRef();
#endif
			return OutOfDate;
		}

#ifdef REFCOUNT_WORK
		frh->ReleaseFRHRef();
#endif
	}
	

	#ifdef PROJ_LOG
	PBUILD_TRACE ("Project \"%s\" build state declared current for"
		" tool %s\n", (const TCHAR *) *GetFilePath (), (const TCHAR *) *pSTool->GetName());
	#endif

	return Current;
}
///////////////////////////////////////////////////////////////////////////////
CProject::BuildResults CProject::BuildAsExternalMakefile
(
	BOOL bBuildAll,
	CErrorContext & EC
)
{
	CEnvironmentVariableList environProj;
	CString strCmdLine, str;

	// Get the external target working directory, and
	// stick a dummy file name on the end
	CDir spawnDir;

    CProjType * pProjType = GetProjType();
    if (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
    {
		spawnDir = GetProjDir();
	}
	else
	{
		spawnDir = GetWorkspaceDir();
	}

    if (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
        GetStrProp (P_Proj_CmdLine, strCmdLine);
    else 
    	GetStrProp (P_ExtOpts_CmdLine, strCmdLine);

	if (bBuildAll)
	{
        if (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
            GetStrProp(P_Proj_RebuildOpt, str);
        else
    		GetStrProp(P_ExtOpts_RebuildOpt, str);
		strCmdLine = strCmdLine + " " + str;
	}

	ASSERT (GetFilePath());

	GetEnvironmentVariables(&environProj);
	environProj.SetVariables();

	CPtrList plCmds;
	CCmdLine *pCmdLine = new CCmdLine;
	pCmdLine->slCommandLines.AddHead(strCmdLine);

	// our description
	pCmdLine->slDescriptions.AddHead("");
	pCmdLine->nProcessor = 1;
	plCmds.AddTail(pCmdLine);

	UINT cmd = g_Spawner.DoSpawn(plCmds, spawnDir, TRUE, FALSE, EC);

	environProj.ResetVariables();	// May do nothing.

	// this external build may have modified the target NT image
	// -> dirty the target attributes
	GetActiveConfig()->DirtyTargetAttributes();

	BuildResults brResult;

	if (cmd != CMD_Canceled && cmd != CMD_Complete)
		cmd = CMD_Error; 

	if (cmd == CMD_Complete)
		brResult = BuildComplete;

	else if (cmd == CMD_Canceled)
		brResult = BuildCanceled;

	else if (cmd == CMD_Error)
		brResult = BuildError;

	return brResult;
}

#define dwExtEOR	((DWORD)0x47414D00)

BOOL CProject::LoadFromOPT(CStateSaver& stateSave)
{
	BOOL bRetVal;

	if (m_bPrivateDataInitializedFromOptFile)
	{
		// don't reread this (in case we're reloading another project)
		ASSERT(m_bPrivateDataInitialized);
		return TRUE;
	}

	// Figure out if the .mak is newer than the .mdp.
	ASSERT(stateSave.GetFileName());
	
	CPath  MDPPath;
	BOOL bNewerMak = TRUE;

	if (MDPPath.Create(stateSave.GetFileName()) && MDPPath.ExistsOnDisk())
	{
		FILETIME ftMdp, ftMak;

		VERIFY(MDPPath.GetFileTime(&ftMdp));

		CFileRegFile * pRegFile = (CFileRegFile *)g_FileRegistry.GetRegEntry(m_hndFileReg);
		if (pRegFile->GetFileTime(ftMak))
		{
		 	if (CFileRegEntry::MyCompareFileTime(&ftMak, &ftMdp) <= 0)
				bNewerMak = FALSE;
		}
	}

	// If the .mak is newer than the .mdp, the cached CActionSlob option strings
	// are not valid. Tell the CActionSlob's to ignore them. 

#if 0 // Olympus:1556, Don't ever read cached options. 
	if (bNewerMak)
		CActionSlob::IgnoreCachedOptions(TRUE);
#else
	CActionSlob::IgnoreCachedOptions(TRUE);
#endif

  	// Load from appropriate stream.
	if (IsExeProject())
		bRetVal = LoadExternalFromOPT(stateSave, IsExeProject());
	else
		bRetVal = LoadInternalFromOPT(stateSave, IsExeProject(), FALSE );
 
#if 0   // See above.
	CActionSlob::IgnoreCachedOptions(FALSE);
#endif

 	// Note: At this point, if bRetval is false, then the OPT file is only
	// partially loaded and we may be in some sort of weird state, especially
	// w.r.t Conifiguration.
	if (bRetVal)
	{
		ASSERT_VALID (this);

		m_bProjectComplete = TRUE;
		m_bPrivateDataInitialized = TRUE;
		m_bPrivateDataInitializedFromOptFile = TRUE;
	}
	else
	{
		COptionTreeCtl::sm_TreeState.Clear();

		if (!m_bProjIsExe)
			InitPrivateData();
		else
			InitPrivateDataForExe();
	}

	//  At this point, we can enable the change config notification
	SetActiveConfig(m_strProjActiveConfiguration, FALSE);
	m_bNotifyOnChangeConfig = TRUE;
    	if (FindProjWksIFace()->IsWorkspaceInitialised() == S_OK)
		theApp.NotifyPackages (PN_CHANGE_CONFIG);

	PFILE_TRACE ( "CProject::LoadFromOPT returning %i\n",bRetVal);
	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function for setting string props when converting a OPT file for an
// external makefile
BOOL CProject::SetStrPropConfigs(CPtrList * plstpcr, UINT idProp, const CString & val)
{
	BOOL bRetVal = TRUE;
	ConfigurationRecord * pcr;

	if (plstpcr)
	{
		POSITION pos = plstpcr->GetHeadPosition();
		while (pos != NULL)
		{
			pcr = (ConfigurationRecord *)plstpcr->GetNext(pos);

			CProjTempConfigChange projTempConfigChange(this);
			projTempConfigChange.ChangeConfig(pcr);

			bRetVal = bRetVal && SetStrProp(idProp, val);
		}
	}

	return bRetVal;
}

BOOL CProject::SetIntPropConfigs(CPtrList * plstpcr, UINT idProp, int val)
{
	BOOL bRetVal = TRUE;
	ConfigurationRecord * pcr;

	if (plstpcr)
	{
		POSITION pos = plstpcr->GetHeadPosition();
		while (pos != NULL)
		{
			pcr = (ConfigurationRecord *)plstpcr->GetNext(pos);

			CProjTempConfigChange projTempConfigChange(this);
			projTempConfigChange.ChangeConfig(pcr);

			bRetVal = bRetVal && SetIntProp(idProp, val);
		}
	}

	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////
// Convert the OPT file for an external makefile so that the contained
// settings can be retained on a corresponding internal makefile generated
// when the external makefile is loaded.
BOOL CProject::ConvertOPTFile(CArchive & ar)
{
	CPtrList lstDebugConfigs;
    CPtrList lstReleaseConfigs;
	BOOL bRetval = TRUE;
	BOOL bDataExists = FALSE;
	CString strDebugConfig, strReleaseConfig, str;

	VERIFY(strDebugConfig.LoadString(IDS_DEBUG_CONFIG));
	VERIFY(strReleaseConfig.LoadString(IDS_RELEASE_CONFIG));
 
	// Construct lists of debug/release configs
	ConfigurationRecord * pcr;
	CString strConfigName;
	const CPtrArray & ppcr = *GetConfigArray();
	int ccr = m_ConfigArray.GetSize();
	for (int icr = 0; icr < ccr; icr++)
	{
		pcr = (ConfigurationRecord *)ppcr[icr];
		strConfigName = pcr->GetConfigurationName();
		if (strConfigName.Find(strDebugConfig) > 0)
			lstDebugConfigs.AddTail(pcr);
		if (strConfigName.Find(strReleaseConfig) > 0)
			lstReleaseConfigs.AddTail(pcr);
	}

  	// Is there any data in this stream.
	ar >> ((long&) bDataExists);

	if (!bDataExists)
	{
		// No data, so we may need to fail the operation
		// so that we will init this data.
  	}
	else
	{
 		BOOL		bHasReleaseConfig = FALSE;
 		DWORD		iConfig, cConfigs;
		CPtrList *	plstpcr;

		// Load the state of the builder
		if (!CActionSlob::LoadFromArchive(ar))
	 		AfxThrowArchiveException (CArchiveException::badSchema);

		// Load the state of the Project.Settings option tree-control
		if (!COptionTreeCtl::LoadOptTreeStore(ar))
 	 		AfxThrowArchiveException (CArchiveException::badSchema);

 		ar >> cConfigs;

		for (iConfig=0 ; iConfig<cConfigs ; ++iConfig)
		{
			// Read the name of the configuration, create that
			// configuration, and make it active:
			ar >> str;

 			// Do we recognize the configuration name as one of our
			// standard configs, i.e. debug/release, if so then we
			// can retain the information
			if (str == strDebugConfig)
 				plstpcr = &lstDebugConfigs;
  			else if (str == strReleaseConfig)
 				plstpcr = &lstReleaseConfigs;
			else
				plstpcr = NULL;
 
			// Set the other properties for this configuration:
			ar >> str;
			SetStrPropConfigs(plstpcr, P_Proj_CmdLine, str);
			ar >> str;
			SetStrPropConfigs(plstpcr, P_Proj_RebuildOpt, str);
			ar >> str;
			SetStrPropConfigs(plstpcr, P_Proj_Targ, str);
			ar >> str;
			SetStrPropConfigs(plstpcr, P_Proj_BscName, str);
			ar >> str;
			SetStrPropConfigs(plstpcr, P_ProjClsWzdName, str);

 			// Skip over platform name, we cannot change the platform
			// for projects in internal makefiles.
			ar >> str;

			// Set debug page properties
  			ar >> str;
			SetStrPropConfigs(plstpcr, P_Caller, str);
			ar >> str;
			SetStrPropConfigs(plstpcr, P_WorkingDirectory, str);
			ar >>  str;
 			SetStrPropConfigs(plstpcr, P_Args, str);
			
			// Read the additional DLLs but we don't set them like
			// we used to
			ar >> str;
 			
			ar >> str;
			SetStrPropConfigs(plstpcr, P_RemoteTarget, str);
		}

		// Get the name of the current configuration:
		ar >> str;  // We don't use it!
 
		// Check the EOR marker.
		DWORD dwT;

		ar >> dwT;
		if (dwT != dwExtEOR)
			AfxThrowArchiveException (CArchiveException::badSchema);
	}

	return bRetval;
}


///////////////////////////////////////////////////////////////////////////////
#define STREAM_PROJINTINFO	"IPI_"
#define STREAM_PROJEXTINFO	"External_Project_Info"
///////////////////////////////////////////////////////////////////////////////
// Load from our external project stream.
BOOL CProject::LoadExternalFromOPT(CStateSaver& stateSave, BOOL bIsExternal)
{
	BOOL bRetval = TRUE;
	BOOL bDataExists = FALSE;
	CString strDebugConfig, strReleaseConfig;
	CInitFile fileInit;
	ASSERT(stateSave.IsLoading());

	ASSERT(bIsExternal);
	if (!stateSave.OpenStream(fileInit, STREAM_PROJEXTINFO ))
		return FALSE;

	CArchive ar ( &fileInit, CArchive::load | CArchive::bNoFlushOnDelete);
	CString str, str2;
	int fPromptForDlls;
	UINT nFormatVersion;
	// Have to set this to 0 because of a bug in CRuntimeClass::Load which
	// only loads a word:
	UINT wSchemaNum = 0l;

	const UINT nDolphinVersionNumber = 0x00020054;
	const UINT nDolphinReleaseVersion = 0x00020000;
	const UINT nDolphinLastCompatOPT = 0x00020051;

	VERIFY(strDebugConfig.LoadString(IDS_DEBUG_CONFIG));
	VERIFY(strReleaseConfig.LoadString(IDS_RELEASE_CONFIG));

	TRY
	{
		ar >> ((long&) nFormatVersion);

		// For the OPT files, the formats must match exactly.
		if ((nFormatVersion != nVersionNumber) && (nFormatVersion < nLastCompatibleOPTFile || nFormatVersion > nVersionNumber))
		{
			if ((nFormatVersion != nDolphinReleaseVersion) && (nFormatVersion < nDolphinLastCompatOPT|| nFormatVersion > nDolphinVersionNumber))
				AfxThrowArchiveException (CArchiveException::badSchema);
			else
			{
				// Convert old vc 2.0 settings here, but only if we are loading 
				// an external project--if so then we can read the data properly.
				if (bIsExternal)
					ConvertOPTFile(ar);
			}
		}


		// Is there any data in this stream.
		ar >> ((long&) bDataExists);

		if (!bDataExists)
		{
			// No data, so we may need to fail the operation
			// so that we will init this data.
			m_hOPTData = NULL;
			bRetval = !bIsExternal;
 		}
		else
		{
			// We have data, but are we loading an external
			// project, if so then we can read it properly.
			if (bIsExternal)
			{
				// Load the state of the builder
//				if (!CActionSlob::LoadFromArchive(ar))
//	 				AfxThrowArchiveException (CArchiveException::badSchema);

				// Load the state of the Project.Settings option tree-control
				if (!COptionTreeCtl::LoadOptTreeStore(ar))
		 			AfxThrowArchiveException (CArchiveException::badSchema);

				BOOL	bHasReleaseConfig = FALSE;
				DWORD	iConfig, cConfigs;

				ar >> cConfigs;

				for (iConfig=0 ; iConfig<cConfigs ; ++iConfig)
				{
					// Read the name of the configuration, create that
					// configuration, and make it active:
					ar >> str;

					// Keep a count of whether we have
					// a config named release or not.
					bHasReleaseConfig = (bHasReleaseConfig || (str == strReleaseConfig));

					CreateConfig(str);
					SetStrProp(P_ProjActiveConfiguration, str);

					// Set the other properties for this configuration:
					ar >> str;
					SetStrProp(P_ExtOpts_CmdLine, str);
					ar >> str;
					SetStrProp(P_ExtOpts_RebuildOpt, str);
					ar >> str;
					SetStrProp(P_ExtOpts_Targ, str);
					ar >> str;
					SetStrProp(P_ExtOpts_BscName, str);
					
					// This property is now defunct
					// ar >> str;
					//SetStrProp(P_ExtOpts_ClsWzdName, str);

                    ar >> str2;

					ar >> str;
					SetStrProp(P_Caller, str);

		 			CPlatform *	pPlatform;
					// need to translate from "official name" to "UI description"
					// also, platform may not exist
 					if (g_prjcompmgr.LookupPlatformByOfficialName(str2, pPlatform))
					{
						SetStrProp(P_ExtOpts_Platform, *(pPlatform->GetUIDescription()));
						if (!pPlatform->IsSupported())
 							InformationBox(IDS_EXE_NOT_SUPPORTED, str);
					}
					else
					{
						// Display a message box stating that we don't support this
						// platform
 						InformationBox(IDS_EXE_NOT_SUPPORTED, str);

						CPlatform * pPlatform = new CPlatform(str2, unknown_platform);
						// Register dynamic platform so deleted on exit
						g_prjcompmgr.RegisterUnkProjObj(pPlatform);

						SetStrProp(P_ExtOpts_Platform, *pPlatform->GetUIDescription());
					}

					ar >> str;
					SetStrProp(P_WorkingDirectory, str);
					ar >> str;

					// If the args are already set then we don't
					// override them. This allows args from the cmd line to
					// override those in the OPT file
					CString strOldArgs;
					GetStrProp(P_Args, strOldArgs);
					if (strOldArgs.IsEmpty())
						SetStrProp(P_Args, str);

					ar >> (LONG&)fPromptForDlls;
					SetIntProp(P_PromptForDlls, fPromptForDlls);
					ar >> str;
					SetStrProp(P_RemoteTarget, str);

					// read in the Java properties

					// get the current platform
//					CPlatform* pPlatform = GetCurrentPlatform();
					ASSERT(pPlatform != NULL);
					if (pPlatform != NULL && pPlatform->GetUniqueId() == java)
					{
						int nValue;

						// class name
						ar >> str;
						SetStrProp(P_Java_ClassFileName, str);

						// debug using browser/stand-alone interpreter
						ar >> (LONG&)nValue;
						SetIntProp(P_Java_DebugUsing, nValue);

						// browser name
						ar >> str;
						SetStrProp(P_Java_Browser, str);

						// stand-alone interpreter name
						ar >> str;
						SetStrProp(P_Java_Standalone, str);
					
						// param source: html page or user
						ar >> (LONG&)nValue;
						SetIntProp(P_Java_ParamSource, nValue);

						// HTML page
						ar >> str;
						SetStrProp(P_Java_HTMLPage, str);
					
						// temp HTML page
						ar >> str;
						SetStrProp(P_Java_TempHTMLPage, str);
					
						// args in HTML form
						ar >> str;
						SetStrProp(P_Java_HTMLArgs, str);

						// command line args
						ar >> str;
						// allow command line args to override those in the VCP file
						CString strJavaOldArgs;
						GetStrProp(P_Java_Args, strJavaOldArgs);
						if (strJavaOldArgs.IsEmpty())
							SetStrProp(P_Java_Args, str);

						// stand-alone interpreter args
						ar >> str;
						SetStrProp(P_Java_StandaloneArgs, str);
						
						// debug stand-alone as applet or application
						ar >> (LONG&)nValue;
						SetIntProp(P_Java_DebugStandalone, nValue);
					}
				}

				// Did we find a release config, if not
				// and we are not an exe-project then we
				// had better create a release config.
				if (!m_bProjIsExe && !bHasReleaseConfig)
				{
					// Add the release config!

					CPath pathT;

					pathT = *GetFilePath();

					CreateConfig(strReleaseConfig);
					SetStrProp(P_ProjActiveConfiguration, strReleaseConfig);

					// Set the default properties for the release configuration
					str = "NMAKE /f ";
					pathT.ChangeExtension(".mak");
					str += pathT.GetFileName();
					SetStrProp(P_ExtOpts_CmdLine, str);

					str = "/a";
					SetStrProp(P_ExtOpts_RebuildOpt, str);

					pathT.ChangeExtension(".exe");
					str = pathT.GetFileName();
					SetStrProp(P_ExtOpts_Targ, str);
					SetStrProp(P_Caller, str);

					(void) GetPlatformDescriptionString(str, str2);
					SetStrProp(P_ExtOpts_Platform, str2);

					pathT.ChangeExtension(".bsc");
					str = pathT.GetFileName();
					SetStrProp(P_ExtOpts_BscName, str);

					pathT.ChangeExtension(".clw");
				
					// This property is now defunct.
					//str = pathT.GetFileName();
					//SetStrProp(P_ExtOpts_ClsWzdName, str);

					str = _T("");
					SetStrProp(P_WorkingDirectory, str);
					SetStrProp(P_Args, str);
					SetIntProp(P_PromptForDlls, 1);
					SetStrProp(P_RemoteTarget, str);
					SetCurrentConfigAsBase();
				}

				// Get the name of the current configuration:
				ar >> str;
				SetStrProp(P_ProjActiveConfiguration, str);

				// Check the EOR marker.
				DWORD dwT;

				ar >> dwT;
				if (dwT != dwExtEOR)
					AfxThrowArchiveException (CArchiveException::badSchema);
			}
			else
			{
				// We found data but we are not really
				// loading an external project so just
				// get a pointer to the external data
				// so we can write it back later.
				m_hOPTData = fileInit.GetInitData();
			}
		}

		ar.Close ();
		fileInit.Close();
	}
	CATCH_ALL(e)
	{
		#ifdef _DEBUG
		AfxDump(e);
		TRACE("\n");
		#endif
		fileInit.Abort();
		bRetval = FALSE;
	}
	END_CATCH_ALL

	return bRetval;
}

///////////////////////////////////////////////////////////////////////////////
// Load from out internal project stream
BOOL CProject::LoadInternalFromOPT(CStateSaver& stateSave, BOOL bIsExternal, BOOL bWorkspace )
{
	BOOL bRetval = TRUE;
	BOOL bDataExists = FALSE;
	CInitFile fileInit;
	ASSERT(stateSave.IsLoading());

	ConfigurationRecord * pcr = GetActiveConfig();
	CString strProjectName;
	if(pcr)
		pcr->GetProjectName(strProjectName);

	CString strStream;
	if( bWorkspace ){
		strStream = STREAM_PROJINTINFO;
	} else {
		strStream = STREAM_PROJINTINFO + strProjectName;
	}
	if (!stateSave.OpenStream(fileInit, strStream ))
		return FALSE;

	CArchive ar ( &fileInit, CArchive::load | CArchive::bNoFlushOnDelete);
	CString str, str2, strFirstConfig, strCurConfig;
	UINT nFormatVersion;
	// Have to set this to 0 because of a bug in CRuntimeClass::Load which
	// only loads a word:
	UINT wSchemaNum = 0l;

	TRY
	{
		ar >> ((long&) nFormatVersion);

		// For the OPT files, the formats must match exactly
		if ((nFormatVersion != nVersionNumber) && (nFormatVersion < nLastCompatibleOPTFile || nFormatVersion > nVersionNumber))
			AfxThrowArchiveException (CArchiveException::badSchema);

		// Do we have any data in this stream.
		ar >> ((long&) bDataExists);
		if (!bDataExists)
		{
			// No we don't have any data, so we
			// may have to fail the loading of the
			// OPT file in order to init the data
			m_hOPTData = NULL;
			bRetval = bIsExternal;
 		}
		else
		{
			// We have some data in the stream but
			// are we attempting to load an internal
			// project, if so then we load the data
			if (!bIsExternal)
			{
				if( bWorkspace ){
					// Load the state of the Project.Settings option tree-control
					if (!COptionTreeCtl::LoadOptTreeStore(ar))
						AfxThrowArchiveException (CArchiveException::badSchema);

					// Load the state of the BatchBuild dialog
					if (!CBatchBldDlg::LoadBatchBldOpt(ar))
						AfxThrowArchiveException (CArchiveException::badSchema);

					// Load the state of the UpdateDeps dialog
					if (!CUpdateDepDlg::LoadSelFromOpt(ar))
						AfxThrowArchiveException (CArchiveException::badSchema);

					// Load the state of the Export dialog
					if (!CProjectsDlg::LoadProjectsSelOpt(ar))
						AfxThrowArchiveException (CArchiveException::badSchema);

					// Load the state of the Profiler dialog
					if (!CProfileDlg::LoadProfOptStore(ar))
						AfxThrowArchiveException (CArchiveException::badSchema);

				} else {
					// Load the state of the builder
					//if (!CActionSlob::LoadFromArchive(ar))
					//	AfxThrowArchiveException (CArchiveException::badSchema);

					ar >> strCurConfig;
					if (ConfigRecordFromConfigName ( strCurConfig ) == NULL )
					{
						CString strmes;
						GetStrProp  (P_ProjActiveConfiguration, strFirstConfig);
						MsgBox (  Information,
								  MsgText ( strmes,
											IDS_OPT_UNKNOWN_CONFIG,
											(const TCHAR *) strCurConfig,
											(const TCHAR *) strFirstConfig
										   )
								);
						strmes.Empty ();
						strCurConfig = strFirstConfig;
	
					}

				// Make sure this stream is for us:
					ar >> str;
					GetMoniker (str2);
					if (CompareMonikers(str, str2) != 0 ||
						CRuntimeClass::Load (ar, &wSchemaNum) != GetRuntimeClass() ||
						wSchemaNum != GetRuntimeClass()->m_wSchema
					   )
						AfxThrowArchiveException(CArchiveException::badSchema);

					if (!SerializeProps(ar))
						AfxThrowArchiveException(CArchiveException::generic);
				}
			}
			else
			{
				// Not loading an internal project so
				// just get a pointer to the stream
				// data so we can later restore it.
				m_hOPTData = fileInit.GetInitData();
			}
		}

		ar.Close ();
		fileInit.Close();
	}
	CATCH_ALL(e)
	{
		#ifdef _DEBUG
		AfxDump(e);
		TRACE("\n");
		#endif
		fileInit.Abort();
		bRetval = FALSE;
	}
	END_CATCH_ALL

#if 0
	//  If the current configuration read in from the make is different from what
	//  we got from the MSF file, we need to set it again with SetStrProp.  This is
	//  Because Serialize props doesn't call SetXXXProp as it reads the props in:
	if (bRetval &&
		!bIsExternal &&	// don't need to check this for external makefiles
		(GetActiveConfig()->GetConfigurationName () != strCurConfig ||
		 m_strProjActiveConfiguration != strCurConfig)
	   )
	{			   
		SetStrProp(P_ProjActiveConfiguration, strCurConfig);
	}
#endif

	return bRetval;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::SaveToOPT(CStateSaver& stateSave)
{
	BOOL bRetVal;

	// We may need to save to both internal and external streams.
	if (IsExeProject())
		bRetVal = SaveExternalToOPT(stateSave, IsExeProject());
	else
		bRetVal = SaveInternalToOPT(stateSave, IsExeProject(),FALSE);

	PFILE_TRACE ( "CProject::SaveToOPT returning %i\n",bRetVal);
	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////
// Save to external project stream.
BOOL CProject::SaveExternalToOPT(CStateSaver& stateSave, BOOL bIsExternal)
{
	BOOL bRetval = TRUE;
	CInitFile fileInit;

	ASSERT(stateSave.IsStoring());

	ASSERT(bIsExternal);
	if (!stateSave.OpenStream(fileInit, STREAM_PROJEXTINFO ))
	{
		SetFileError(CFileException::generic);
		return FALSE;
	}

	CArchive ar ( &fileInit, CArchive::store | CArchive::bNoFlushOnDelete );
	CString str;

	TRY
	{
		// Save format version:
		if (bIsExternal || m_hOPTData == NULL)
		{
 			ar << ((long) nVersionNumber);
			ar << ((long) (bIsExternal || m_hOPTData != NULL));
		}

		// If we are saving an external project then let's
		// really save the data.
		if (bIsExternal)
		{
			// Save the state of the builder
//			if (!CActionSlob::SaveToArchive(ar))
//	 			AfxThrowArchiveException (CArchiveException::badSchema);

			// Save the state of the Project.Settings option tree-control
			if (!COptionTreeCtl::SaveOptTreeStore(ar))
	 			AfxThrowArchiveException (CArchiveException::badSchema);

			int icr, ccr;
			CStringProp *pProp;
			ConfigurationRecord *pcr;
			const CPtrArray & ppcr = *GetConfigArray();

			ccr = m_ConfigArray.GetSize();
			ar << (DWORD)ccr;

			// Loop through all configurations, writing out information about each.
			for (icr=0 ; icr<ccr ; ++icr)
			{
				pcr = (ConfigurationRecord *)ppcr[icr];
				ar << pcr->GetConfigurationName();

 				// Get the 'current' property bag for this configuration
				CPropBag * pBag = pcr->GetPropBag(CurrBag);

				VERIFY(pProp = (CStringProp *)pBag->FindProp(P_ExtOpts_CmdLine));
				ASSERT(pProp->m_nType == string );
				ar << pProp->m_strVal;

				VERIFY(pProp = (CStringProp *)pBag->FindProp(P_ExtOpts_RebuildOpt));
				ASSERT(pProp->m_nType == string );
				ar << pProp->m_strVal;

				VERIFY(pProp = (CStringProp *)pBag->FindProp(P_ExtOpts_Targ));
				ASSERT(pProp->m_nType == string );
				ar << pProp->m_strVal;

				VERIFY(pProp = (CStringProp *)pBag->FindProp(P_ExtOpts_BscName));
				ASSERT(pProp->m_nType == string );
				ar << pProp->m_strVal;

				// This property is now defunct
				//VERIFY(pProp = (CStringProp *)pBag->FindProp(P_ExtOpts_ClsWzdName));
				//ASSERT(pProp->m_nType == string );
				//ar << pProp->m_strVal;

				VERIFY(pProp = (CStringProp *)pBag->FindProp(P_ExtOpts_Platform));
				ASSERT(pProp->m_nType == string);
				{
					CPlatform *	pPlatform;
					// the prop val is the "UI description" of the platform
					// we want to write the official name
					VERIFY(g_prjcompmgr.LookupPlatformByUIDescription(pProp->m_strVal, pPlatform));
					ar << *(pPlatform->GetName());
				}

				VERIFY(pProp = (CStringProp *)pBag->FindProp(P_Caller));
				ASSERT(pProp->m_nType == string);
				ar << pProp->m_strVal;

				VERIFY(pProp = (CStringProp *)pBag->FindProp(P_WorkingDirectory));
				ASSERT(pProp->m_nType == string);
				ar << pProp->m_strVal;

				VERIFY(pProp = (CStringProp *)pBag->FindProp(P_Args));
				ASSERT(pProp->m_nType == string);
				ar << pProp->m_strVal;

				CIntProp *pIntProp;
				VERIFY(pIntProp = (CIntProp *)pBag->FindProp(P_PromptForDlls));
				ASSERT(pIntProp->m_nType == integer);
				ar << (LONG)pIntProp->m_nVal;

				VERIFY(pProp = (CStringProp *)pBag->FindProp(P_RemoteTarget));
				ASSERT(pProp->m_nType == string);
				ar << pProp->m_strVal;

				// write out the Java properties

				// get the current platform
				CPlatform* pPlatform = GetCurrentPlatform();
				ASSERT(pPlatform != NULL);
				if (pPlatform != NULL && pPlatform->GetUniqueId() == java)
				{
					// class name
					VERIFY(pProp = (CStringProp*)pBag->FindProp(P_Java_ClassFileName));
					ASSERT(pProp->m_nType == string);
					ar << pProp->m_strVal;

					// debug using browser/stand-alone interpreter
					VERIFY(pIntProp = (CIntProp*)pBag->FindProp(P_Java_DebugUsing));
					ASSERT(pIntProp->m_nType == integer);
					ar << (LONG)pIntProp->m_nVal;

					// browser name
					VERIFY(pProp = (CStringProp*)pBag->FindProp(P_Java_Browser));
					ASSERT(pProp->m_nType == string);
					ar << pProp->m_strVal;

					// stand-alone interpreter name
					VERIFY(pProp = (CStringProp*)pBag->FindProp(P_Java_Standalone));
					ASSERT(pProp->m_nType == string);
					ar << pProp->m_strVal;
				
					// param source: html page or user
					VERIFY(pIntProp = (CIntProp*)pBag->FindProp(P_Java_ParamSource));
					ASSERT(pIntProp->m_nType == integer);
					ar << (LONG)pIntProp->m_nVal;

					// HTML page
					VERIFY(pProp = (CStringProp*)pBag->FindProp(P_Java_HTMLPage));
					ASSERT(pProp->m_nType == string);
					ar << pProp->m_strVal;
				
					// temp HTML page
					VERIFY(pProp = (CStringProp*)pBag->FindProp(P_Java_TempHTMLPage));
					ASSERT(pProp->m_nType == string);
					ar << pProp->m_strVal;
				
					// args in HTML form
					VERIFY(pProp = (CStringProp*)pBag->FindProp(P_Java_HTMLArgs));
					ASSERT(pProp->m_nType == string);
					ar << pProp->m_strVal;

					// command line args
					VERIFY(pProp = (CStringProp*)pBag->FindProp(P_Java_Args));
					ASSERT(pProp->m_nType == string);
					ar << pProp->m_strVal;

					// stand-alone interpreter args
					VERIFY(pProp = (CStringProp*)pBag->FindProp(P_Java_StandaloneArgs));
					ASSERT(pProp->m_nType == string);
					ar << pProp->m_strVal;
					
					// debug stand-alone as applet or application
					VERIFY(pIntProp = (CIntProp*)pBag->FindProp(P_Java_DebugStandalone));
					ASSERT(pIntProp->m_nType == integer);
					ar << (LONG)pIntProp->m_nVal;
				}
			}

			// Write the name of the active configuration.
			GetStrProp  (P_ProjActiveConfiguration, str);
			ar << str;

			// Write the end-of-record marker.
			ar << dwExtEOR;
		}
		else
		{
			// Not really saving an external project, so do
			// we have any data that we need to preserve.
			if (m_hOPTData != NULL && !m_bProjConverted)
			{
				// Just write out the data we read in.
				WORD FAR* lpw = (WORD FAR*) ::GlobalLock(m_hOPTData);
				UINT nFileSize = (WORD) *lpw++;
				ar.Write(lpw, nFileSize);
				::GlobalUnlock(m_hOPTData);
			}
		}

		ar.Close ();
		fileInit.Close ();
	}
	CATCH_ALL(e)
	{
		#ifdef _DEBUG
		AfxDump(e);
		TRACE("\n");
		#endif

		if (e->IsKindOf(RUNTIME_CLASS(CFileException)))
			SetFileError(((CFileException*) e)->m_cause);
		else
			SetFileError(CFileException::generic);

		fileInit.Abort(); // will not throw an exception
		bRetval = FALSE;
	}
	END_CATCH_ALL

	return bRetval;
}

///////////////////////////////////////////////////////////////////////////////
// Save to internal project stream.
BOOL CProject::SaveInternalToOPT(CStateSaver& stateSave, BOOL bIsExternal, BOOL bWorkspace)
{
	BOOL bRetval = TRUE;
	CInitFile fileInit;

	ASSERT(stateSave.IsStoring());

	ConfigurationRecord * pcr = GetActiveConfig();
	CString strProjectName;
	if(pcr)
		pcr->GetProjectName(strProjectName);

	CString strStream;

	if( bWorkspace ){
		strStream = STREAM_PROJINTINFO;
	} else {
		strStream = STREAM_PROJINTINFO + strProjectName;
	}

	if (!stateSave.OpenStream(fileInit, strStream ))
	{
		SetFileError(CFileException::generic);
		return FALSE;
	}

	CArchive ar ( &fileInit, CArchive::store | CArchive::bNoFlushOnDelete );
	CString str;

	TRY
	{
		// Save format version:
		if (!bIsExternal || m_hOPTData == NULL)
		{
 			ar << ((long) nVersionNumber);
			ar << ((long) (!bIsExternal || m_hOPTData != NULL));
		}

		// If we are saving an internal project then let's
		// really save the data.
		if (!bIsExternal)
		{
			if( bWorkspace ){
					
				// Load the state of the Project.Settings option tree-control
				if (!COptionTreeCtl::SaveOptTreeStore(ar))
					AfxThrowArchiveException (CArchiveException::badSchema);

				// Save the state of the BatchBuild dialog
				if (!CBatchBldDlg::SaveBatchBldOpt(ar))
					AfxThrowArchiveException (CArchiveException::badSchema);
		
				// Save the state of the UpdateDeps dialog
				if (!CUpdateDepDlg::SaveSelToOpt(ar))
					AfxThrowArchiveException (CArchiveException::badSchema);

				// Save the state of the Export dialog
				if (!CProjectsDlg::SaveProjectsSelOpt(ar))
					AfxThrowArchiveException (CArchiveException::badSchema);

				// (WinslowF) Save the state of the Profiler dialog
				if (!CProfileDlg::SaveProfOptStore(ar))
					AfxThrowArchiveException (CArchiveException::badSchema);

			} else {
				// Save the state of the builder
				// if (!CActionSlob::SaveToArchive(ar))
				//	AfxThrowArchiveException (CArchiveException::badSchema);

				// do this for all subprojects
				GetStrProp  (P_ProjActiveConfiguration, str);
				ar << str;
				
				GetMoniker (str);
				ar << str;
				GetRuntimeClass()->Store (ar);
	
				if (!SerializeProps(ar))
					AfxThrowArchiveException(CArchiveException::generic);
			}

		}
		else
		{
			// Not saving an internal project but we
			// may have some data that need's to be
			// preserved!
			if (m_hOPTData != NULL)
			{
				// Just write out the data we read in.
				WORD FAR* lpw = (WORD FAR*) ::GlobalLock(m_hOPTData);
				UINT nFileSize = (WORD) *lpw++;
				ar.Write(lpw, nFileSize);
				::GlobalUnlock(m_hOPTData);
			}
		}

		ar.Close ();
		fileInit.Close ();
	}
	CATCH_ALL(e)
	{
		#ifdef _DEBUG
		AfxDump(e);
		TRACE("\n");
		#endif

		if (e->IsKindOf(RUNTIME_CLASS(CFileException)))
			SetFileError(((CFileException*) e)->m_cause);
		else
			SetFileError(CFileException::generic);

		fileInit.Abort(); // will not throw an exception
		bRetval = FALSE;
	}
	END_CATCH_ALL

	return bRetval;
}

///////////////////////////////////////////////////////////////////////////////
CProject::ReadMakeResults CProject::ReadMakeFile (BOOL &bProjectDirty)
{
//	Open the project's makefile and try to read it in as an internal makefile.
//	The makefile reader determines this by looking for our special header in
//	the file.

	// 04-10-96 jimsc  SuckMacro uses this, so we have to make sure it doesn't
	// point to a deleted object.

	g_pLastTool = NULL;
	g_bErrorPrompted = FALSE;
	g_bConversionCanceled = FALSE;
	static UINT idConvReply;
	BOOL bIsWorkspaceInit = (FindProjWksIFace()->IsWorkspaceInitialised() == S_OK);

#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "devbldd.dll", "ReadMakeFile()", letypeBegin, 0);
#endif

	BOOL bRetval = TRUE, bOkAsInternal = FALSE;
	const CPath *pPath = GetFilePath();
	ASSERT (pPath);
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
		if (excptFile.m_cause == CFileException::none)
			MsgBox(Error, IDS_BAD_DIR_SPEC);
		else if (excptFile.m_cause == CFileException::hardIO)
			MsgBox (Error, MsgText (strMsg, IDS_INVALID_PROJECT_LOCALPATH, *pPath));
		else
			MsgBox(Error, MsgText (strMsg, *pPath, IDS_OPEN_PROJECT_FILE, &excptFile));

		rmr = ReadError;
	}
	else																
	{
		CException e;

		TRY
		{
			pObject = mr.GetNextElement();
			ASSERT (pObject);

			// Skip over "Microsoft Developer Studio Project File" signature
			VERIFY (strDSSig.LoadString (IDS_DSPROJ_SIG_STRING));
			if ((pObject->IsKindOf(RUNTIME_CLASS(CMakComment))) &&
				 (_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)strDSSig, strDSSig.GetLength()) == 0))
			{
				// UNDONE: check/assert that the package id is 4
				delete pObject;
				pObject = mr.GetNextElement();
				ASSERT (pObject);
			}

			// look for a comment with the signature string
			// to see if it is a Dolphin or Olympus makefile
			// or an Orion project file
			VERIFY (str.LoadString (IDS_MAKEFILE_SIG_STRING));
            VERIFY (str1.LoadString (IDS_BUILDPRJ_SIG_STRING));
            VERIFY (str2.LoadString (IDS_OLD_MAKEFILE_SIG_STRING));
            VERIFY (str3.LoadString (IDS_EXPORT_SIG_STRING));
			if ((!pObject->IsKindOf(RUNTIME_CLASS(CMakComment))) ||
				 ((_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str, str.GetLength()) != 0) && 
				 (_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str1, str1.GetLength()) != 0) && 
				 (_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str2, str2.GetLength()) != 0) && 
				 (_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str3, str3.GetLength()) != 0))
			   )
			{
				//					  
				// not a Dolphin makefile can we convert this pre- v2.0 builder makefile?
				// We'll ask each builder converter registered to make a check.
				//

 				// parse it whilst ignoring line continuation characters and not showing a status bar
				// (old v1.x makefiles incorrectly use '\' in their macro values, eg. 'PROJPATH=c:\tmp\appwiz\')
				mr.SetAttributes((MakRW_Default | MakR_IgnLineCont) & ~MakRW_ShowStatus);

				// unget this comment line
				mr.UngetElement(pObject);

				// builder conversion information block
				CNV_INFO cnvinfo = {0};		// blank out

				cnvinfo.pPath = (const CPath *)pPath;	// path to builder
				cnvinfo.pProject = this;				// builder object
				cnvinfo.nUseMFC = g_prjcompmgr.MFCIsInstalled() ? UseMFCInLibrary : NoUseMFC;

				CBuilderConverter * pbldrcnvtr;

				// initialise each converter
				// our list to try the conversion on
				CPtrList lstBldrCnvtrs;
				g_prjcompmgr.InitBuilderConverterEnum();
				while (g_prjcompmgr.NextBuilderConverter(pbldrcnvtr))
				{
					if (pbldrcnvtr->FInitialise())
						lstBldrCnvtrs.AddTail((void *)pbldrcnvtr);
				}

				rmr = ReadExternal;
				// read through the makefile until converters exhausted
				BOOL fEOF = FALSE;

				while (!fEOF)
				{
					pObject = mr.GetNextElement();
					ASSERT (pObject);

					// EOF makefile?
					fEOF = pObject->IsKindOf(RUNTIME_CLASS(CMakEndOfFile));

					// ask each converter in turn...
					POSITION pos = lstBldrCnvtrs.GetHeadPosition();

					// out of converters?
					fEOF = fEOF || (pos == (POSITION)NULL);
					if (!fEOF)
					{
						while (pos != (POSITION)NULL)
						{
							POSITION posbldrcnvtr = pos;
							pbldrcnvtr = (CBuilderConverter *)lstBldrCnvtrs.GetNext(pos);

							// is this converter still attempting to recognise this builder?
							if (pbldrcnvtr->CanConvert(pObject, mr.GetCurrentLine(), &cnvinfo))
							{
								// yes and
								// wants to convert?
								if (cnvinfo.nStatus == 1)
								{
									// yes
									cnvinfo.pbldrcnvtr = pbldrcnvtr;
									fEOF = TRUE;
								}
								// wants to abort?
								else if (cnvinfo.nStatus == -1)
								{
									// yes, flag this as makefile read error
									rmr = ReadError;
									fEOF = TRUE;
								}
							}
							else
							{
								// no, so remove from our list
								lstBldrCnvtrs.RemoveAt(posbldrcnvtr);
							}
						}
					}

  					delete pObject;
					pObject = (CObject *)NULL;
				}

				// have to jump out of the previous percent-dones before
				// starting the conversion, or else the status bar will
				// not reflect the progress of the conversion.
				StatusEndPercentDone();

				// doesn't look like a Visual C++ makefile?
				if (cnvinfo.pbldrcnvtr == (CBuilderConverter *)NULL)
					AfxThrowFileException (CFileException::generic);

				// re-open the makefile so that we parse from the beginning again
				// show the status
				mr.SetAttributes(MakRW_Default | MakR_IgnLineCont);
				mr.ReOpen();

				// read through the makefile until error or EOF
				fEOF = FALSE;

				while (!fEOF)
				{
					pObject = mr.GetNextElement();
					ASSERT (pObject);

					// EOF makefile?
					fEOF = pObject->IsKindOf(RUNTIME_CLASS(CMakEndOfFile));

					// have the builder converter perform the conversion
					if (!fEOF)
					{
						if (!cnvinfo.pbldrcnvtr->DoConvert(pObject, mr.GetCurrentLine(), &cnvinfo))
						AfxThrowFileException (CFileException::generic);
					}

  					delete pObject;
					pObject = (CObject *)NULL;
				}

				// is this makefile we are to convert wanting to use MFC?
				if (cnvinfo.nUseMFC != NoUseMFC && !g_prjcompmgr.MFCIsInstalled())
				{
					// flag this error
					m_bGotUseMFCError = TRUE;

					if (QuestionBox(IDS_MFC_NOT_INSTALLED, MB_YESNO) == IDNO)
						AfxThrowFileException (CFileException::generic);

					cnvinfo.nUseMFC = NoUseMFC;	// best of luck!
					bProjectDirty = TRUE;
				}

				// finish the conversion
				if (!cnvinfo.pbldrcnvtr->FinishConvert(&cnvinfo))
					AfxThrowFileException (CFileException::generic);

				// scanning and saving gets done later
				m_bProjMarkedForScan = TRUE;
				m_bProjMarkedForSave = !cnvinfo.fSilent;
				m_bProjHasNoOPTFile  = TRUE;

				// flag the fact that we just converted,
				// and ok the builder as internal (we're converting it!)
				m_bProjConverted = bOkAsInternal = TRUE;
				rmr = ReadInternal;

				if (!cnvinfo.fSilent)
					// we want to dirty the project if it is not "silent convert"
					bProjectDirty = TRUE;
			}
			else if (_tcsncmp((const TCHAR *)((CMakComment *) pObject)->m_strText, (const TCHAR *)str3, str3.GetLength()) == 0)
			{
				CDir dir;
				CPath path;
				dir.CreateFromPath(*pPath);
				VERIFY(path.CreateFromDirAndFilename(dir, ((const TCHAR *)((CMakComment *)pObject)->m_strText) + str3.GetLength()));
				delete pObject;
				pObject = (CObject *)NULL;
				if (!path.ExistsOnDisk())
				{
					// file doesn't exist, so give up
					g_bErrorPrompted = TRUE;
					CProjTempProjectChange projTempProjectChange (NULL);
					MsgText (strMsg, IDS_READ_EXPORTED_FILE, (const char *) path);
					MsgBox(Error, strMsg);
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

#define MAJOR_VERSION_MATCH_NOT_REQUIRED
#ifdef MAJOR_VERSION_MATCH_NOT_REQUIRED
#pragma message ("Same major version not yet required for internal format reading")
#endif
				if ((*pend)==_T('.'))
				{
					// Release format makefile version number as it contains a period
					// So get the major and minor versions and check the validity
					nMajor = fileVer;
					nMinor = strtoul(++pend, NULL, 10);
#ifdef MAJOR_VERSION_MATCH_NOT_REQUIRED
					bOkAsInternal = ( (10000*nMajor+100*nMinor) >= nLastCompatibleMakefile);
#else
					bOkAsInternal = ((nMajor == nVersionMajor) && (nMinor >= nLastCompatibleMakefileMinor));
#endif
  				}
				else
				{
					// Internal format makefile version number. So get the major version
					// number and compare it no the last compatible makefile version.
					nMajor = fileVer / 10000;
					nMinor = fileVer / 100 - (nMajor * 100);
#ifdef MAJOR_VERSION_MATCH_NOT_REQUIRED
 					bOkAsInternal = (fileVer >= nLastCompatibleMakefile);
#else
 					bOkAsInternal = ((nMajor == nVersionMajor) &&(fileVer >= nLastCompatibleMakefile));
#endif
 				}
				nFormat = nMajor * 10000 + nMinor * 100;
				bFutureFormat = ((nFormat > nVersionNumber) && (nFormat >= (g_nNextIncompatibleVersion * 100))); // defaults to 700
				bOkAsInternal = bOkAsInternal && !bFutureFormat;

				m_bConvertedVC20 = FALSE;
				m_bConvertedDS4x = FALSE;
				m_bConvertedDS5x = FALSE;

				if ((nMajor >= 2) && (nMajor < nVersionMajor))  // REVIEW
				{
					// If assert this fires, need to add 6.x conversion code
					ASSERT(nVersionMajor <= 6);

					UINT idMsg = (nMajor == 2) ? IDS_CONVERT_MAKEFILE : IDS_CONVERT_MAKEFILE4;

					CString strMsg, strMsg2, strMsg3, strMsg4;
					VERIFY(strMsg2.LoadString(IDS_CONVERT_PORT2));
					if (!bIsWorkspaceInit)
					{
						// for workspace load, Yes applies to all projects
						VERIFY(strMsg4.LoadString(IDS_CONVERT_PORT4));
						strMsg2 += strMsg4;
					}
					CPath backup(*pPath);
					CString strBackExt;
					UINT n = 1;
					do {
						strBackExt.Format(_T(".%03d"), n);
						backup.ChangeExtension(strBackExt);
					} while ((!backup.CanCreateOnDisk()) && (++n < 1000));
					ASSERT(n < 1000);  // write failure

					MsgText(strMsg3,IDS_BACKUP_MAKEFILE,(const TCHAR *)backup);
					g_pActiveProject = NULL;
					MsgText(strMsg, idMsg, (const TCHAR *)*pPath);
					if (g_pAutomationState->DisplayUI() && !g_bNoUI) //ShellOM:State
					{
						g_pActiveProject = this;
						CProjTempProjectChange projTempProjectChange (NULL);
						if (!g_bConversionPrompted)
						{
							idConvReply = MsgBox(Question, strMsg +strMsg3 +strMsg2, MB_YESNOCANCEL);
							// avoid prompting for conversion multiple times
							// during workspace load
							// FUTURE: add true YESTOALL UI
							g_bConversionPrompted = ((idConvReply == IDYES) && (!bIsWorkspaceInit));
						}
					}
					else
					{
						if (theApp.m_bInvokedCommandLine && (nMajor >= 5))
						{
							idConvReply = IDYES; // just convert it
							theApp.WriteLog(strMsg, TRUE);
							theApp.WriteLog(strMsg3 + _T("\n"), TRUE);
						}
						else
						{
							// The automation engine cannot cope with this error message.
							// The safest thing is to return an error and quit.
							g_pAutomationState->SetError((nMajor == 2) ? DS_E_PROJECT_OLD_MAKEFILE_VC : DS_E_PROJECT_OLD_MAKEFILE_DEVSTUDIO) ;

							// Pretend that the user pressed Cancel.
							idConvReply = IDCANCEL ;
							if (theApp.m_bInvokedCommandLine)
							{
								// REVIEW: should we support this?
								CString strOut;
								MsgText(strOut, IDS_ERR_CANT_CONVERT, (LPCTSTR)*pPath);
								theApp.WriteLog(strOut, TRUE);
							}
						}
					}

					// Want to convert?
					if (idConvReply == IDYES)
					{
						// REVIEW (KiP: 10/96) we should offer a checkbox on the previous dialog asking if we should backup.
						CopyFile( (const TCHAR *)*pPath, (const TCHAR *)backup, TRUE );
						
						if (nMajor < 5) // REVIEW
						{
							// make sure we get rid of any .opt files from a previous conversion attempt
							CPath OPTPath(*pPath);
							OPTPath.ChangeExtension(WSOPTIONS_EXT);
							if (OPTPath.ExistsOnDisk())
								RemoveFileIfPossible(OPTPath);
						}
						
						// Prepare to convert
						// VC++ 2.0/4.0 makefile
						if (nMajor == 2)
						{
						
							m_bConvertedVC20 = TRUE;
						}
						else if (nMajor < 5)
						{
							m_bConvertedDS4x = TRUE;
						}
						else if (nMajor == 5)
						{
							m_bConvertedDS5x = TRUE;
						}

						// Don't use OemToAnsi translation for versions < 6.0
						if (nMajor < 6)
						{
							g_bConvDontTranslate = TRUE;
						}

						// Flag bOkAsInternal as TRUE, we are converting!!
						m_bProjConverted = bOkAsInternal = TRUE;

						// we'll need this later...
						g_pConvertedProject = (CProject *)this;
					}
					else if ((idConvReply == IDCANCEL) || (nMajor >= 5))
					{
						g_pAutomationState->SetSavePromptCanceled() ; // Inform automation engine that operation was canceled

						// flag this as makefile read error
						bOkAsInternal = FALSE;
						rmr = ReadError;
						g_bConversionCanceled = (idConvReply == IDCANCEL);
					}
					else
					{
						ASSERT(nMajor < 5);
						// offer to wrap it
						bOkAsInternal = FALSE;
						rmr = ReadExternal;
					}
				}
				else if (bFutureFormat)
				{
					// give error if trying to load with old product
					ASSERT(!bOkAsInternal);
					rmr = ReadError;
					UINT idReply ;
					CString strMsg;
					MsgText(strMsg, IDS_READ_FUTURE_FORMAT, (const TCHAR *)*pPath);
					if (g_pAutomationState->DisplayUI() && !g_bNoUI) //ShellOM:State
					{
						g_pActiveProject = this;
						CProjTempProjectChange projTempProjectChange (NULL);
						idReply = MsgBox(Error, strMsg, MB_OKCANCEL);
					}
					else
					{
						if (theApp.m_bInvokedCommandLine)
						{
							idReply = IDOK; // just ignore it
							theApp.WriteLog(strMsg, TRUE);
						}
						else
						{
							// The automation engine cannot cope with this error message.
							// The safest thing is to return an error and quit.
							g_pAutomationState->SetError(DS_E_PROJECT_FUTURE_FORMAT);
							g_pAutomationState->SetSavePromptCanceled() ; // Inform automation engine that operation was canceled

							// Pretend that the user pressed Cancel.
							idReply = IDCANCEL ;
						}
					}
					g_bConversionCanceled = (idReply == IDCANCEL);

				}

				// Ok this is the latest and greatest makefile format
				if (!bOkAsInternal)
 					AfxThrowFileException (CFileException::generic);
 
				delete (pObject); pObject = (CObject *)NULL;
				ASSERT(bOkAsInternal);	// ok as internal (we saw a signature)

				// Read in configurations:
				if (!ReadConfigurationHeader(mr))
				{
					if (m_bConvertedDS5x)
						rmr = ReadError; // don't try to wrap
					AfxThrowFileException(CFileException::generic);
				}

				ASSERT(this == g_pActiveProject);

				// we'll break when we hit the '# Project' delimiter or EOF
				for (;;)
				{
					if (pObject)
					{
						delete pObject;
						pObject = (CObject *)NULL;
					}

					pObject = mr.GetNextElement();
					ASSERT(pObject != (CObject *)NULL);

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
				
				// Init the dep caches before really reading anything.
				InitDependencyCaches();

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
					ASSERT(pObject != (CObject *)NULL);

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


				// Post-'read bulk' VC++ 2.0 conversion?
				// FUTURE: move this into a CBuilderConverter::FinishConvert()
				// when (if) we develop a 'true' v2.x .mak converter
#if 0
				if (m_bConvertedVC20)
				{
					// Collect any groups in the project together
					CObList ol;
					POSITION pos = GetHeadPosition();
					while (pos != NULL)
					{
						CProjGroup* pGroup = (CProjGroup*)GetNext(pos);
						if (pGroup->IsKindOf(RUNTIME_CLASS(CProjGroup)))
 							ol.AddTail(pGroup);	
 					}

#if 0
					CProjGroup * pGroup;
					CTargetItem * pTarget;
					CString strTargetName;
#endif
					// process the groups
					while (!ol.IsEmpty())
					{
						CProjGroup* pGroup = (CProjGroup*)ol.RemoveHead();
#if 1
						// Delete the groups
						pGroup->MoveInto(NULL);
#else
						// UNDONE: We now keep any groups since v5 can handle them
				        InitTargetEnum();
      					VERIFY(NextTargetEnum(strTargetName, pTarget));
						pGroup->MoveInto(pTarget);
#endif
					}
				}
#endif

				CProject * pProject = GetProject();
				ASSERT(pProject == this);
				CString strProj;
				if (m_bConvertedDS4x)
				{
					CProject::InitProjectEnum();
					pProject = (CProject *)CProject::NextProjectEnum(strProj, FALSE);
				}
				do {
					const CPtrArray * pCfgArray = pProject->GetConfigArray();
					int icfg, size = pCfgArray->GetSize();
					int nDeletedJavaCfg = 0;
#if 1
					if ((pProject->m_bConvertedDS5x) || (pProject->m_bConvertedDS4x) || m_bConvertedVC20)
					{
						CStringList lststrPlatforms;
						CString strPlatformName;
						strPlatformName.LoadString(IDS_V4_MACPMC_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);
						// strPlatformName.LoadString(IDS_V2_WIN32MIPS_PLATFORM);
						// lststrPlatforms.AddTail(strPlatformName);
						strPlatformName.LoadString(IDS_V4_MIPS_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);
						strPlatformName.LoadString(IDS_V2_MAC68K_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);
						strPlatformName.LoadString(IDS_V2_MACPPC_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);
						strPlatformName.LoadString(IDS_V4_PPC_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);

						// don't support Java in DevStudio anymore
						CString strJavaPlatformName;
						strJavaPlatformName.LoadString(IDS_V5_JAVA_PLATFORM);
						lststrPlatforms.AddTail(strJavaPlatformName);
					
						BOOL bFoundSupported = FALSE;
						CString strEmpty;
						icfg = size;
						while (icfg-- > 0)
						{
							ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(icfg);
							CString strOriginalName = pcr->GetConfigurationName();
							CString strDebugness = strOriginalName;
							CProjType * pprojtype = NULL;
							CString strOrgType = pcr->GetOriginalTypeName();
							VERIFY(g_prjcompmgr.LookupProjTypeByName(strOrgType, pprojtype));
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
									if (strPlatformName == strJavaPlatformName)
									{
										nDeletedJavaCfg++;
									}
									// delete old configs
									HBLDTARGET hTarg = g_BldSysIFace.GetTarget(strOriginalName, (HBUILDER)pProject);
									ASSERT(hTarg != NO_TARGET);
									VERIFY(g_BldSysIFace.DeleteTarget(hTarg, (HBUILDER)pProject));
									// pProject->DeleteConfig(strDebugness, strEmpty);
									break;
								}
							}
						}
					}
#endif
					size = pCfgArray->GetSize();
					ASSERT(m_bProjConverted || (size != 0));
					
					if ((nDeletedJavaCfg > 0) && (size > 0))
					{
						// give belated warning that we're deleting Java configs from a VC makefile project
						// REVIEW: use a more specific error message?
						CString strMsg;
						int i = IDS_DISCARD_UNSUPPORTED;
						strMsg.LoadString(i);
						CProjTempProjectChange projTempProjectChange (NULL);
						MsgBox (Information, strMsg, MB_OK);
					}
					else if ((size == 0) && (m_bProjConverted))
					{
						// we deleted everything on conversion!
						CProjTempProjectChange projTempProjectChange (NULL);
						UINT mbButtons = (g_pAutomationState->DisplayUI() && !g_bNoUI) ? MB_OKCANCEL : MB_OK;
						if (MsgBox (Error, IDS_NOGOOD_PLATFORMS, mbButtons)==IDCANCEL)
							g_bConversionCanceled = TRUE;
						else
							g_bBadConversion = TRUE;

						g_bErrorPrompted = TRUE;
						rmr = ReadError;
						AfxThrowFileException (CFileException::generic);
					}

					for (icfg = 0; icfg < size; icfg++)
					{
						ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(icfg);
						CProjTempConfigChange projTempConfigChange(pProject);
						projTempConfigChange.ChangeConfig(pcr);

						// Post-'read custom build' assignment for target-level
						// we couldn't do this during the read because we hadn't set
						// up our targets as we were reading in the project-level blurb....
						// we'll fix this for v4.0 ;-)
						// we'll send this one, we only need one, P_CustomCommand or P_CustomOutputSpec
						// both are supported by custom build
						pProject->InformDependants(P_CustomCommand); 

						// FUTURE: move this into a CBuilderConverter::FinishConvert()
						// when (if) we develop a 'true' v2.x .mak converter
						if (m_bConvertedVC20 || pProject->m_bConvertedDS4x)
						{
							// Set minimum build and/or MRE deps flags for compiler
							COptionHandler * popthdlr;
							VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Compiler),
																 (CBldSysCmp *&)popthdlr));

							if (m_bConvertedVC20)
								pProject->SetIntProp(popthdlr->MapLogical(P_Enable_MR), TRUE);

							pProject->SetIntProp(popthdlr->MapLogical(P_Enable_FD), TRUE);

							// convert include paths for subprojects
							if (pProject->m_bConvertedDS4x)
								pProject->ConvertDirs();

							// Enable the /mktylib203 flag.
							VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_MkTypLib),
															 (CBldSysCmp *&)popthdlr));

							pProject->SetIntProp(popthdlr->MapLogical(P_MTLMtlCompatible), TRUE);
						}

						if (m_bConvertedVC20 || pProject->m_bConvertedDS4x || m_bConvertedDS5x)
						{
							// V5 -> V6 conversion goes here

							COptionHandler * popthdlr;
							VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Compiler),
																 (CBldSysCmp *&)popthdlr));


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
					strPlatformName.LoadString(IDS_V2_WIN32X86_PLATFORM);
					lststrPlatforms.AddTail(strPlatformName);
					strPlatformName.LoadString(IDS_V2_WIN3280X86_PLATFORM);
					lststrPlatforms.AddTail(strPlatformName);
					strPlatformName.LoadString(IDS_V2_WIN32_PLATFORM);
					lststrPlatforms.AddTail(strPlatformName);
					
					// Loop through all the targets and rename each of them
					const CPtrArray * pCfgArray = GetProject()->GetConfigArray();
					int icfg, size = pCfgArray->GetSize();
					for (icfg = 0; icfg < size; icfg++)
					{
						ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(icfg);
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
						ASSERT(pTarget != NULL);
						if (pTarget != NULL)
							pTarget->SetTargetName(strProjectName);

						// Rename the target
 						RenameTarget(strOriginalName, strNewName);

						// There are some consistency requirements for VC 4.0 projects that were not
						// meaningful in VC 2.0 projects. For example debug MFC requires debug CRT's
						// but there were no debug CRT's in VC 2.0. We call PerformWizardSettings 
						// to do the validity checking and make the neccessary changes.

						int iUseMFC;
						GetIntProp(P_ProjUseMFC, iUseMFC);
					
						GetProjType()->PerformSettingsWizard(this, iUseMFC);
					}

					// saving gets done later
					m_bProjMarkedForSave = TRUE;
					m_bProjHasNoOPTFile  = TRUE;
					m_bProjConverted = TRUE;

					// we want to dirty the project because it is not "silent convert"
					bProjectDirty = TRUE;
					::SetWorkspaceDocDirty();
				}

				// Post-'read bulk' VC++ 4.x conversion?
				if (m_bConvertedDS4x)
				{
					// saving gets done later
					m_bProjMarkedForSave = TRUE;
					m_bProjHasNoOPTFile  = TRUE;
					m_bProjConverted = TRUE;

					// we want to dirty the project because it is not "silent convert"
					bProjectDirty = TRUE;
					::SetWorkspaceDocDirty();
				}

				// Post-'read bulk' VC++ 5.x conversion?
				if (m_bConvertedDS5x)
				{
					// saving gets done later
					m_bProjMarkedForSave = TRUE;
					m_bProjHasNoOPTFile  = TRUE; // REVIEW
					m_bProjConverted = TRUE;

					// we want to dirty the project because it is not "silent convert"
					bProjectDirty = TRUE;
					::SetWorkspaceDocDirty();
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
		CATCH (CException, e)
		{
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
		END_CATCH
	}

	// We are not going to read any dependency information
	// after this point so free these up.
	FreeDependencyCaches( );

	if ((rmr != ReadError) && (rmr != ReadExportedMakefile))
	{
		if (!bRetval || !bOkAsInternal)
		{
			// if we don't have a particular error flagged then
			// this looks like a failure to read one of our projects -> likely to have been modified
			if (bOkAsInternal && (!m_bGotUseMFCError) && (!g_bErrorPrompted) && (!m_bProjConverted))
			{
				CProjTempProjectChange projTempProjectChange (NULL);
				MsgBox (Error, MsgText (strMsg, IDS_READ_PROJECT_FILE, (const char *) *pPath));
				g_bErrorPrompted = TRUE;
			}

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

#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "devbldd.dll", "ReadMakeFile()", letypeEnd, 0);
#endif
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
		LPCSTR pszPath;
		if ((!(SUCCEEDED(g_pProjWksIFace->GetWorkspaceDocPathName(&pszPath)))) || (pszPath == NULL) || (!*pszPath))
		{
			// need this for conversion to work properly
			CPath path = *GetFilePath();
			ProjNameToWksName(path);
			g_pProjWksIFace->SetWorkspaceDoc(path);
		}
		ASSERT(IsKindOf(RUNTIME_CLASS(CProject)));
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
		// disabled FileRegistry notifications during load
		g_bInProjLoad = TRUE;

		pObject = mr.GetNextElement ();
		if (!pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)))
				AfxThrowFileException (CFileException::generic);

 		delete (pObject); pObject = NULL;

		for (; pObject = mr.GetNextElement ();)
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
				if ((!pProject->ReadMasterDepsLine((CMakDescBlk *)pObject, BaseDir)) ||
				   (!pProject->SuckDescBlk((CMakDescBlk *)pObject)))
					AfxThrowFileException (CFileException::generic);
			}
			else if (pProject->IsCustomBuildMacroComment(pObject)) {
				pProject->ReadCustomBuildMacros(mr);
			}
			// Comment marks start of children:
			else if (pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)))
			{
				ASSERT( pProject->m_cp==EndOfConfigs);
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
   			ConfigurationRecord* pcr;
			CString strTargetName;
			CTargetItem* pNewTarget;

			// All the remaining configs are also mapped to the
			// newly created target node, i.e. they are mirrors
			int iNumConfig = m_ConfigArray.GetSize();
			for (int i = 0 ; i < iNumConfig; i++)
			{
				pcr = (ConfigurationRecord *) m_ConfigArray[i] ;
				strTargetName = pcr->GetConfigurationName();

				if (i == 0)
				{
					pNewTarget = new CTargetItem();
					pNewTarget->MoveInto(this);
				}

				// force the target into it's own config.
				CProjTempConfigChange projTempConfigChange(this);
				projTempConfigChange.ChangeConfig(strTargetName);
				ConfigurationRecord * pcrProject = GetActiveConfig();
								
				pNewTarget->GetActiveConfig()->SetValid(TRUE);
		
				// create a build instance
				g_buildengine.AddBuildInst(pcrProject);

			 	// inform the graph of this target that it is being created
				g_buildengine.GetDepGraph(pcrProject)->SetGraphMode(Graph_Creating);

			 	// assign the build actions
				CActionSlob::AssignActions(pNewTarget, (CPtrList *)NULL, pcrProject);

				// reset the project config.
				projTempConfigChange.Release();

				// Register this target with the project target manager
				VERIFY(RegisterTarget(strTargetName, pNewTarget));
			}
		}

		g_pActiveProject = (CProject *)this;

		// read in children from the makefile
		if (!ReadInChildren(mr, BaseDir))
			AfxThrowFileException (CFileException::generic);

		g_pActiveProject = (CProject *)this;

		CProjTempConfigChange projTempConfigChange(this);

		// We are now in the post read section
		m_bPostMakeFileRead = TRUE;

		// reenable FileRegistry notifications before DepGraph calculations
		g_bInProjLoad = FALSE;

		SetPrompt(IDS_UPDATING_DEPENDENCIES); // truth in advertising

		projTempConfigChange.Reset();

		pProject = (CProject *)this;
		CString strProject;
		if (m_bConvertedDS4x)
		{
			CProject::InitProjectEnum();
			VERIFY(pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE));
		}
		do {
			CProjTempConfigChange projTempConfigChange(pProject);
			g_pActiveProject = pProject;

			// We are now in the post read section
			pProject->m_bPostMakeFileRead = TRUE;

			// custom build assignment (post-project read!)
			CBuildTool * pTool;
			VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_CustomBuild), (CBldSysCmp *&)pTool));
			CPtrList lstSelectTools;
			lstSelectTools.AddTail(pTool);

			// special build tool(pre-link/post-build steps)
			VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_SpecialBuild), (CBldSysCmp *&)pTool));
			lstSelectTools.AddTail(pTool);

			// perform some post-read target processing
			CString strTargetName;
			CTargetItem* pTarget;
			pProject->InitTargetEnum();
			while (pProject->NextTargetEnum(strTargetName, pTarget))
			{
				ConfigurationRecord * pcrTarget = pProject->ConfigRecordFromConfigName(strTargetName);
				projTempConfigChange.ChangeConfig(pcrTarget);

                // no longer necessary
				// try to assign a custom build tools
				// to both project-level and target files

				//
				// REVIEW(tomse); I added UnAssignActions and re-enabled AssignActions on the
				// project because everything but the 4.0 makfile conversion sniff test seemed to
				// work after changing the order of tools in CProjType(prjconfg.cpp).  I'm adding
				// a hack back, but it fixes a dogfood bug.
				//
				// assign to contained = TRUE
				CActionSlob::UnAssignActions(pProject, &lstSelectTools, (ConfigurationRecord  *)NULL, TRUE);
				CActionSlob::AssignActions(pProject, &lstSelectTools, (ConfigurationRecord  *)NULL, TRUE);

//				CActionSlob::UnAssignActions(pTarget, &lstSelectTools, (ConfigurationRecord  *)NULL, TRUE);
//				CActionSlob::AssignActions(pTarget, &lstSelectTools, (ConfigurationRecord  *)NULL, TRUE, TRUE);

		 		// inform the graph of this target that it is now stable
				g_buildengine.GetDepGraph(pcrTarget)->SetGraphMode(Graph_Stable);
				
				// is this makefile wanting to use MFC?
				if (!g_prjcompmgr.MFCIsInstalled())
				{
					// trying to use MFC?
					int fUseMFC;
					CProject * pProject = pTarget->GetProject();
					if (pProject->GetIntProp(P_ProjUseMFC, fUseMFC) == valid && fUseMFC)
					{
						// flag this error
						BOOL bOldGotUseMFCError = pProject->m_bGotUseMFCError;
						pProject->m_bGotUseMFCError = TRUE;

						if (!bOldGotUseMFCError && QuestionBox(IDS_MFC_NOT_INSTALLED, MB_YESNO) == IDNO)
							AfxThrowFileException (CFileException::generic);

						// best of luck!
						pProject->SetIntProp(P_ProjUseMFC, FALSE);
					}
				}
			}
		} while (m_bConvertedDS4x && ((pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL));

 	}
	CATCH (CException, e)
	{
		// reenable FileRegistry notifications
		g_bInProjLoad = FALSE;
		g_pActiveProject = (CProject *)this;
		m_bPostMakeFileRead = FALSE;
		if (pObject) delete (pObject);
		return FALSE;	// failure
	}
	END_CATCH

	g_pActiveProject = (CProject *)this;
	m_bPostMakeFileRead = FALSE;

	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::ReadMasterDepsLine(CMakDescBlk * pObject, const CDir & BaseDir)
{
	// special case: process master Deps line ALL:
	// for unsupported project types
	if (_tcsicmp(_T("ALL"), pObject->m_strTargets) != 0)
		return TRUE;

	BOOL retVal = TRUE;
	ConfigurationRecord * pcr = GetActiveConfig();
	CProjType * pprojtype = GetProjType();

	int i = 0, nMax = 1;
	
	TRY
	{
		if (m_cp == EndOfConfigs)
		{
			nMax = GetPropBagCount();
			pcr = (ConfigurationRecord *)m_ConfigArray[0];
			ASSERT_VALID(pcr);
			VERIFY(g_prjcompmgr.LookupProjTypeByName(pcr->GetOriginalTypeName(), pprojtype));
		}
		for (;;) // iterate through configs if necessary based on m_cp
		{
			if (!pprojtype->IsSupported())
			{
				FileRegHandle frh;
				CPath pt;
				CString str;
				const CString * pstrDeps = &(pObject->m_strDeps);
				int index = 0, len, nMaxIndex = pstrDeps->GetLength();
				while (index < nMaxIndex)
				{
					if (_istspace((*pstrDeps)[index]))
					{
						index++; // skip white
						continue;
					}
					str = pstrDeps->Mid(index);
					if (str[0]==_T('"'))
					{
						// strip quotes
						str = str.Mid(1);
						str = str.SpanExcluding(_T("\""));
						len = str.GetLength() + 2;	// two quotes!
					} 
					else 
					{
						str = str.SpanExcluding(_T(" \t\r\n"));
						len = str.GetLength() + 1;
					}

					// substitute for $(OUTDIR) macro if nessessary
					SubstOutDirMacro(str, pcr);
					
					if (!pt.CreateFromDirAndFilename(BaseDir, str))
						AfxThrowFileException (CFileException::generic);

#ifndef REFCOUNT_WORK
					if ((frh = g_FileRegistry.RegisterFile(&pt)) != (FileRegHandle)NULL)
#else
					if ((frh = CFileRegFile::GetFileHandle(pt)) != (FileRegHandle)NULL)
#endif
					{
						if (NULL == pcr->m_pTargetFiles )
							pcr->m_pTargetFiles = new CFileRegSet(1);
						VERIFY(pcr->m_pTargetFiles->AddRegHandle(frh));
#ifdef REFCOUNT_WORK
						frh->ReleaseFRHRef();
#endif
					}
					index += len;
				}
			}

			// repeat for next config, if necessary
			if (++i >= nMax)
				break;

			pcr = (ConfigurationRecord *)m_ConfigArray[i];
			ASSERT_VALID(pcr);
			VERIFY(g_prjcompmgr.LookupProjTypeByName(pcr->GetOriginalTypeName(), pprojtype));
		}
	}
	CATCH (CException, e)
	{
		retVal = FALSE;	// failure
	}
	END_CATCH

	return retVal;
}

int g_nProjTypesToWrite; // used in projtool.cpp

///////////////////////////////////////////////////////////////////////////////
BOOL CProject::WriteConfigurationHeader ( CMakFileWriter& mw )
{
//	Write out header to makefile which lists the configrations and will warn
//  then user if he doesn't specify one:

	CString str;
	CStringProp *pProp;
	int j,k;
	CMapStringToPtr mapTargTypes;
	const CPtrArray & ppcr = *GetConfigArray();
	j = m_ConfigArray.GetSize();

	TRY
	{
		if (!mw.IsMakefile())
		{
			void * pVoid;
			_TCHAR buf[256];
			POSITION pos;
			CProjType * pProjType;

			// Write list of all projtypes and UniqueId's (for compatibility)
			// N.B. we now depend on this to support reading unknown platforms
			for (k=0 ;k < j ; k++)
			{
				str = ((ConfigurationRecord *)ppcr[k])->GetOriginalTypeName();
				if (g_prjcompmgr.LookupProjTypeByName(str,pProjType))
				{
					// record each Projtype and (valid) UniqueId in use by this makefile
					if (pProjType->GetUniqueTypeId() && pProjType->GetUniquePlatformId())
						mapTargTypes.SetAt(str, (void *)pProjType->GetUniqueId());
				}
			}

			g_nProjTypesToWrite = mapTargTypes.GetCount();

			if (!mapTargTypes.IsEmpty()) // write all target types
			{
				pos = mapTargTypes.GetStartPosition();
				while (pos != NULL)
				{
					mapTargTypes.GetNextAssoc(pos, str, pVoid);
					_stprintf(buf,_T("TARGTYPE \"%s\" %#06x"), (LPCTSTR)str, (UINT)pVoid);
					mw.WriteComment(buf);	// Write target-type name & id
				}
				mw.EndLine ( );
			}
			mapTargTypes.RemoveAll();
		}

		CPath MakPath = *GetFilePath(); // REVIEW: get from actual path
		MakPath.ChangeExtension(".mak");

		if (mw.IsMakefile())
		{
			// Form string "!IF "$(CFG)" != ""
			// str = "\"$(CFG)\" == \"\"";
			str.LoadString ( IDS_CONFIG_DEFAULT_HEADER0 );
			mw.WriteDirective (CMakDirective::DTYP_IF, str);

			// Form string	CFG = DEBUG
			str.LoadString ( IDS_CONFIG_DEFAULT_HEADER1 );
			mw.WriteMacro (str, (const TCHAR *) m_strProjDefaultConfiguration);

			// Form string !MESSAGE No configuation specified Defaulting to configuration DEBUG
			MsgText ( str, IDS_CONFIG_DEFAULT_HEADER2,
					(const TCHAR *) m_strProjDefaultConfiguration);
			mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str);

			mw.WriteDirective (CMakDirective::DTYP_ENDIF, NULL);
			mw.EndLine ();

			// Form string "!IF "$(CFG)" != "DEBUG" && "$(CFG)" != "RELEASE"
			str = _TEXT("\"$(CFG)\" != \"");
			for (k=0 ;k <j ; k++)
			{
				str += ((ConfigurationRecord *)ppcr[k])->GetConfigurationName ();
				if (k < j-1) str += _TEXT("\" && \"$(CFG)\" != \"");
			}
			str += _TEXT("\"");

			mw.WriteDirective (CMakDirective::DTYP_IF, str);

			// Invalid configuration "Ladeda" specified.
			str.LoadString ( IDS_CONFIG_HEADER0 );
			mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str);
		}
		else
		{
			// Form string	CFG = DEBUG
			str.LoadString ( IDS_CONFIG_DEFAULT_HEADER1 );
			mw.WriteMacro (str, (const TCHAR *) m_strProjDefaultConfiguration);
			str.LoadString ( IDS_CONFIG_HEADER0_INT );
			mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str);
			str.LoadString ( IDS_CONFIG_HEADER1_INT );
			mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str);
			mw.WriteDirective (CMakDirective::DTYP_MESSAGE, NULL);
			str.LoadString ( IDS_CONFIG_HEADER2_INT );
			str += MakPath.GetFileName();
			str += _T("\".");
			mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str);
			mw.WriteDirective (CMakDirective::DTYP_MESSAGE, NULL);
		}

		// You can specify a configuration when running NMAKE on this makefile
		str.LoadString ( IDS_CONFIG_HEADER1 );
		mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str);

		// by defining the macro CFG on the command line.  For example:
		str.LoadString ( IDS_CONFIG_HEADER2 );
		mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str);
		mw.WriteDirective (CMakDirective::DTYP_MESSAGE, NULL);

		// NMAKE /f "filename" CFG="DEBUG".
		CString strDefaultCfg = m_strProjDefaultConfiguration;
		MsgText ( str, IDS_CONFIG_HEADER3, MakPath.GetFileName(),
							 (const TCHAR *)strDefaultCfg);
		mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str);
		mw.WriteDirective (CMakDirective::DTYP_MESSAGE, NULL);

		// Possible choices for configuration are:
		str.LoadString ( IDS_CONFIG_HEADER4 );
		mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str);
		mw.WriteDirective (CMakDirective::DTYP_MESSAGE, NULL);

		// "Ladeeda" (based on "Windows 32 bit exe")
		for (k=0 ;k <j ; k++)
		{
			pProp = (CStringProp *) ((ConfigurationRecord *)ppcr[k])->GetPropBag(CurrBag)->FindProp (P_ProjOriginalType);
			ASSERT (pProp);
			ASSERT (pProp->m_nType == string );
			CString strCfg = ((ConfigurationRecord *)ppcr[k])->GetConfigurationName();
			MsgText ( str, IDS_CONFIG_HEADER5,
							 (const TCHAR *) strCfg,
							 (const TCHAR *) pProp->m_strVal);
			mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str );
		}
		mw.WriteDirective (CMakDirective::DTYP_MESSAGE, NULL);

		if (mw.IsMakefile())
		{
			// !ERROR No configuration specified.
			str.LoadString ( IDS_CONFIG_HEADER6 );
			mw.WriteDirective (CMakDirective::DTYP_ERROR, str );

			// !ENDIF
			mw.WriteDirective (CMakDirective::DTYP_ENDIF, NULL);
			mw.EndLine ();

			// Write out hack for checking for existance of directories.
	 		// !IF "$(OS)" == "Windows_NT"
			str.LoadString ( IDS_OUTDIR_NULL );
			mw.WriteDirective (CMakDirective::DTYP_IF, str);

			// Write the macro for windows nt
			// NULL=
	 		CString strNull;
			str.LoadString ( IDS_NULL );
			mw.WriteMacro (str, (const TCHAR *)strNull);

			// !ELSE
			mw.WriteDirective (CMakDirective::DTYP_ELSE, NULL);
			
			// Write the win95 macro
			// NULL=nul
			strNull.LoadString ( IDS_WIN95_NULL );
			mw.WriteMacro (str, (const TCHAR *)strNull);

			// !ENDIF
			mw.WriteDirective (CMakDirective::DTYP_ENDIF, NULL);

#if 0	// moved to bldrfile.cpp
	 		// !IF "$(NO_EXTERNAL_DEPS)" != "1"
			mw.EndLine ();
			str = "\"$(NO_EXTERNAL_DEPS)\" != \"1\"";
			mw.WriteDirective (CMakDirective::DTYP_IF, str);

			CPath makPathDep = *GetFilePath(); // REVIEW: get from actual path
			makPathDep.ChangeExtension(".dep");
			str = makPathDep.GetFileName();
			str = "\"" + str + "\"";

			// !IF EXISTS("foo.dep")
			CString str2 = "EXISTS("; str2 += str; str2 += ")";
			mw.WriteDirective (CMakDirective::DTYP_IF, str2);

			// !INCLUDE "foo.dep"
			mw.WriteDirective (CMakDirective::DTYP_INCLUDE, str);

			// !ELSE
			mw.WriteDirective (CMakDirective::DTYP_ELSE, NULL);

			// !MESSAGE Warning: cannot find "devbld.dep"
			str2 = "Warning: cannot find " + str;
			mw.WriteDirective (CMakDirective::DTYP_MESSAGE, str2);

			mw.WriteDirective (CMakDirective::DTYP_ENDIF, NULL);
			mw.WriteDirective (CMakDirective::DTYP_ENDIF, NULL);
#endif

		}
		mw.EndLine ();

	}
	CATCH (CException, e)
	{
		return FALSE;	// failure
	}
	END_CATCH

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
							mapTargTypes.SetAt(strTarget, (void *)_tcstoul(pend, NULL, 0));
						}
						else ASSERT(0);
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
					int cbLen = strCfg.GetLength();
					int nFirst = strCfg.Find(_T(" - "));
					ASSERT(nFirst != -1);
					if (nFirst < 1)
						AfxThrowFileException(CFileException::generic);

					// strip out project name for this config
					CString strTarget = strCfg.Left(nFirst);

					// make sure there wasn't already a project of ths name
					// in the workspace
					void * pProjID;
					if (g_pProjWksIFace->CanAddProject(strTarget) != S_OK)
					{
						g_bErrorPrompted = TRUE;
						{
							CProjTempProjectChange projTempProjectChange (NULL);
							MsgBox(Error, IDS_ERR_DUPE_PROJ);
						}
						AfxThrowArchiveException(CArchiveException::generic);
					}
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
							VERIFY(pathProj.Create(strProject));
						}
						pathProj.ChangeExtension(_T(BUILDER_EXT));
						// pathProjMak = pathProj; pathProjMak.ChangeExtension(".mak");
						BOOL bFound = FALSE;
						CProject::InitProjectEnum();
						ASSERT(!strTarget.IsEmpty());

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
										// REVIEW
										g_bErrorPrompted = TRUE;
										CProjTempProjectChange projTempProjectChange (NULL);
										MsgBox(Error, IDS_ERR_DUPE_PROJ);
										AfxThrowFileException(CFileException::generic);
									}
								}
							}
						
							if (!bFound)
							{
								pProject = (CProject *)g_BldSysIFace.CreateBuilder(pathProj, FALSE, FALSE);
								ASSERT(pProject != NULL);
								if (!pProject)
								{
									AfxThrowFileException(CFileException::generic);
								}
	
								pProject->m_bConvertedDS4x = TRUE;
 								pProject->m_bProjMarkedForSave = TRUE;
								pProject->m_bProjConverted = TRUE;
								pProject->m_bProjHasNoOPTFile  = TRUE;			
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
						ASSERT(0); // dead code
#if 0
						// just use this project
						bUsedThisProject = TRUE;
						strProjName = strTarget;  // save for later comparison
						SetTargetName(strTarget);
						pBestFitForThisProject = this;
						m_strProjStartupConfiguration.Empty();
						bSetDefaultConfig = FALSE;
						strSupportedConfig.Empty();
#endif
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
				if ((!g_prjcompmgr.LookupProjTypeByName (strBase, (CProjType *&) pProjType)) || (pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeUnknown))))
				{
					if ((!bDelConfigsPrompted) && (m_bConvertedVC20 || m_bConvertedDS4x))
					{
						CStringList lststrPlatforms;
						CString strPlatformName;
						strPlatformName.LoadString(IDS_V4_MACPMC_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);
						// strPlatformName.LoadString(IDS_V2_WIN32MIPS_PLATFORM);
						// lststrPlatforms.AddTail(strPlatformName);
						strPlatformName.LoadString(IDS_V4_MIPS_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);
						strPlatformName.LoadString(IDS_V2_MAC68K_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);
						strPlatformName.LoadString(IDS_V2_MACPPC_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);
						strPlatformName.LoadString(IDS_V4_PPC_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);

						// don't support Java in DevStudio anymore
						strPlatformName.LoadString(IDS_V5_JAVA_PLATFORM);
						lststrPlatforms.AddTail(strPlatformName);

						POSITION pos = lststrPlatforms.GetHeadPosition();
						while (pos != NULL)
						{
							strPlatformName = lststrPlatforms.GetNext(pos);
							int nLength = strPlatformName.GetLength();
							if (_tcsncmp((LPCTSTR)strPlatformName, (LPCTSTR)strBase, nLength)==0)
							{
								bDelConfigsPrompted = TRUE;
								CString strT, strMsg;
								int i;
								i = IDS_DISCARD_UNSUPPORTED;
								while (i < IDS_DISCARD_UNSUPPORTED_LAST)
								{
									strT.LoadString(i);
									strMsg += strT;
									i++;
								}
								if (g_pAutomationState->DisplayUI() && !g_bNoUI) //ShellOM:State
								{
									// append question
									strT.LoadString(IDS_DISCARD_UNSUPPORTED_LAST);
									strMsg += strT;

									CProjTempProjectChange projTempProjectChange (NULL);
									if (MsgBox (Question, strMsg, MB_YESNO) != IDYES)
									{
										g_bErrorPrompted = TRUE;
										AfxThrowFileException (CFileException::generic);
									}
								}
								else if (theApp.m_bInvokedCommandLine)
								{
									theApp.WriteLog(strMsg, TRUE);
								}
								break;
							}
						}
					}
					if (pProjType==NULL || !pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeUnknown)))
					{
						// Unrecognized ProjType string so try mapping by UniqueId
						if (!mapTargTypes.Lookup(strBase, (void * &)uniqueId) ||
							!g_prjcompmgr.LookupProjTypeByUniqueId(uniqueId, (CProjType *&)pProjType))
						{
							// try to get the platform from the unique id
							CPlatform * pPlatform = NULL;
							if ((uniqueId >> 8) & 255)
								g_prjcompmgr.LookupPlatformByUniqueId(((uniqueId >> 8) & 255), pPlatform);

							/*
							// [matthewt] 05/11/95
							// we will have FORTRAN project types for the Intelx86 platform
							// so we can't do this for post v2.x
							//
							if (pPlatform != (CPlatform *)NULL &&
								pPlatform->IsSupported())
							{
								// Unknown application for supported platform, so give error
								g_bErrorPrompted = TRUE;
								{
									CProjTempProjectChange projTempProjectChange (NULL);
									MsgBox (
									Information,
									MsgText (
											str,
											IDS_MAKEFILE_UNKNOWN_BASE,
											(const TCHAR *) strBase,
											(const TCHAR *) strCfg
											)
									);
								}
								AfxThrowFileException (CFileException::generic);
							}
							else
							*/

							{
								//
						 		// unknown platform and/or project type so create
								// it to avoid trashing makefile
								//

								// derive platfrom and type names from available information
								int index;

								// 1) By default, strip up to first whitespace or last ')'
								if (((index = strBase.Find(_T(')')))) == -1)
									VERIFY((index = strBase.FindOneOf(_T(" \t"))) > 0);
								else
									index++;

								CString strType = strBase.Mid(index+1); // default

	#if 0
								// 2) Also look up known application types, and compare against end of string
								int newindex, savedindex = 9999;
								const CStringList * pTypeList = g_prjcompmgr.GetListOfTypes();
								POSITION pos = pTypeList->GetHeadPosition();
								while (pos != NULL)
								{
									newindex = strType.Find(pTypeList->GetNext(pos));
									if ((newindex != -1) && (newindex < savedindex))
										savedindex = newindex;
								}
								if ((savedindex != 0) && (savedindex != 9999))
								{
									index += savedindex;
									strType = strBase.Mid(index+1);
								}
	#endif

								if (pPlatform==NULL)
								{
									CString strPlatform = strBase.Left(index);
									pPlatform = new CPlatform(strPlatform, (uniqueId>>8) & 255);
									// Register dynamic platform so deleted on exit
									g_prjcompmgr.RegisterUnkProjObj(pPlatform);
								}
								pProjType = new CProjTypeUnknown(strType, uniqueId & 255, pPlatform);
								// Register dynamic projtype so deleted on exit
								g_prjcompmgr.RegisterUnkProjObj(pProjType);

								// update name in case it changed
								strBase = *pProjType->GetName();
							}
						}
						else
						{
							// must update to new (supported) targtype name
							// ignore id if either byte is 0
							ASSERT(uniqueId & 255); ASSERT(uniqueId>>8);
							strBase = *pProjType->GetName();
						}
					}
				}
				BOOL bIsExternalTarget = (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)));
				if ((m_bConvertedDS5x) && (!bIsExternalTarget)) // must allow makefile projects still
				{
					// don't support Java in DevStudio anymore
					CString strPlatformName;
					strPlatformName.LoadString(IDS_V5_JAVA_PLATFORM);
					int nLength = strPlatformName.GetLength();
					if (_tcsncmp((LPCTSTR)strPlatformName, (LPCTSTR)strBase, nLength)==0)
					{
						CString strT, strMsg;
						int i = IDS_DISCARD_JAVA;
						while (i <= IDS_DISCARD_JAVA_LAST)
						{
							strT.LoadString(i);
							strMsg += strT;
							i++;
						}
						CProjTempProjectChange projTempProjectChange (NULL);
						UINT mbButtons = (g_pAutomationState->DisplayUI() && !g_bNoUI) ? MB_OKCANCEL : MB_OK;
						if (MsgBox (Error, strMsg, mbButtons) == IDCANCEL)
							g_bConversionCanceled = TRUE;
						else
							g_bBadConversion = TRUE;

						g_bErrorPrompted = TRUE;
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
			VERIFY (pmd = (CMakDirective *) mr.GetNextElement ());

		} while (pmd->IsKindOf ( RUNTIME_CLASS (CMakDirective)) &&
				 pmd->m_dtyp == CMakDirective::DTYP_MESSAGE);

		if (m_bConvertedDS4x && !bUsedThisProject)
		{
			if (!pBestFitForThisProject)
			{
				ASSERT(0);
				AfxThrowFileException(CFileException::generic);
			}
			strSupportedConfig.Empty();
			strDefaultConfig = pBestFitForThisProject->m_strProjDefaultConfiguration;
			SetTargetName(pBestFitForThisProject->GetTargetName());
			// copy pBestFitForThisProject to this and delete
			const CPtrArray & ppcr = *pBestFitForThisProject->GetConfigArray();
			ConfigurationRecord * pcr;

			{
				CProjTempConfigChange projTempConfigChange(pBestFitForThisProject);

				CString strConfig, strType;
				int ccr = ppcr.GetSize();
				for (int icr = 0; icr < ccr; icr++)
				{
					pcr = (ConfigurationRecord *)ppcr[icr];
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
			ASSERT(bFoundUnsupportedConfig);
			m_bProjIsSupported = FALSE;
			CProjTempProjectChange projTempProjectChange (NULL);
			MsgBox(Information, IDS_MAKEFILE_NO_KNOWN_TARGETS);
		}
#if 0	// this is too cumbersome, since all samples now contain unknown targs
		else if (bFoundUnsupportedConfig)
		{
			MsgBox(Information, IDS_MAKEFILE_UNKNOWN_TARGETS);
		}
#endif

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
			ASSERT(ConfigRecordFromConfigName(m_strProjDefaultConfiguration));
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
	ASSERT (bSetDefaultConfig);
	ASSERT (GetActiveConfig());
	ASSERT (!m_ConfigMap.IsEmpty());
	ASSERT (m_ConfigArray.GetSize());
	ASSERT (m_ConfigArray.GetSize()==m_ConfigMap.GetCount());
	ASSERT_VALID (this);
	return TRUE;
}

static FileRegHandle frhCustom = NULL;
///////////////////////////////////////////////////////////////////////////////
// Filter function for writing the ALL dependency line in the makefile.
// If you don't want a target to appear on this line here is the place to
// filter it out.
BOOL FilterMasterDepsLine ( DWORD /* dw */, FileRegHandle frh)
{
	// check for custom step for custom appwiz, quick fix
	// if the location are not standard such as Template, then, it won't write out macro
	CPath * pPath = (CPath *)g_FileRegistry.GetRegEntry(frh)->GetFilePath();
	CProject *pProject = (CProject *)g_BldSysIFace.GetActiveBuilder();
	ASSERT(pPath);
	
	if (_tcsicmp(pPath->GetExtension(), _TEXT(".awx")) == 0)
	{
		CString strOut;
		CString strCustomOutSpec;
		const CPtrArray & ppcr = *pProject->GetConfigArray();
		ConfigurationRecord * pcr;

		CProjTempConfigChange projTempConfigChange(pProject);

		int ccr = ppcr.GetSize();
		{
			for (int icr = 0; icr < ccr; icr++)
			{
				pcr = (ConfigurationRecord *)ppcr[icr];
				projTempConfigChange.ChangeConfig(pcr);
				pProject->GetStrProp(P_CustomOutputSpec, strCustomOutSpec);
				if (!strCustomOutSpec.IsEmpty())
				{
					int nLen = _tcslen(strCustomOutSpec);
					TCHAR *tcsStr = _tcsdec( strCustomOutSpec, (const TCHAR *)strCustomOutSpec + nLen );
					if( *tcsStr == _TCHAR('\t') ) {
						ASSERT( nLen > 1 );
						strCustomOutSpec = strCustomOutSpec.Left( nLen-1 );
					}

					CActionSlobList * pActions = pcr->GetActionList();
					CActionSlob * pAction = (CActionSlob *)pActions->GetHead();
					ExpandMacros(&strOut, strCustomOutSpec, pAction);
					break;
				}
 			}
		}

		if (strOut.CompareNoCase(pPath->GetFullPath()) == 0)
		{
			frhCustom = frh;
			return FALSE;
		}
	}
	else if (_tcsicmp(pPath->GetExtension(), _TEXT(".trg")) == 0)
	{
		int i;
		VERIFY(pProject->GetIntProp(P_DeferredMecr, i));
		if (i != dabNotDeferred)
		{
			return FALSE;
		}
	}


	// All other files are ok.
	return TRUE;
}

int __cdecl CmpCStrings(const void * pstr1, const void * pstr2);

BOOL CProject::WriteMasterDepsLine(CMakFileWriter & mw, const CDir & BaseDir, BOOL bPostBuildDep)
{
	CString str;  // the all line
	CString strCustomOutSpec;
	CStringArray strOutArray;

	TRY
	{
		CObList lstItem; lstItem.AddHead(this);

		// write the ALL : targets .MAK line
		// FUTURE: given targets that are in the $(INTDIR) directory
		// this will not work, we prepend them here with $(OUTDIR)
		// , we pass in the project object. This is fine for V2
		// 'cos the targets are .bsc and .exe/.lib/.dll

		CProjType *pProjType = GetProjType();
		if (pProjType && pProjType->GetUniqueTypeId() == CProjType::exttarget) {
			CFileRegSet * pregset = GetTargetPaths();
			MakeQuotedString(
							 pregset,
							 str, &BaseDir,
							 TRUE, FilterMasterDepsLine, NULL,
							 NULL, TRUE,		// only first item
							 (const CObList *)&lstItem
							,FALSE	// must not be sorted!!
							);

		} else {
			CFileRegSet setTargets;
			DWORD dw = (DWORD)&setTargets;
			g_buildengine.GetDepGraph(GetActiveConfig())->PerformOperation(CFileDepGraph::GetPrimaryTargetNoCustom, dw);
			
			CMapStringToPtr mapStrCustomOut;
			DWORD dw2=(DWORD)&mapStrCustomOut;
			g_buildengine.GetDepGraph(GetActiveConfig())->PerformOperation(CFileDepGraph::GetCustomOutput, dw2);
	
			MakeQuotedString(
							 &setTargets,
							 str, &BaseDir,
							 TRUE, FilterMasterDepsLine, NULL,
							 NULL, FALSE,		// all items
							 (const CObList *)&lstItem
							,FALSE	// must not be sorted!!
							);
			if (frhCustom)
			{
				CProject *pProject = g_pActiveProject;
				const CPtrArray & ppcr = *pProject->GetConfigArray();
	
				ConfigurationRecord * pcr;
				CProjTempConfigChange projTempConfigChange(pProject);
				int ccr = ppcr.GetSize();
	
				{
					for (int icr = 0; icr < ccr; icr++)
					{
						pcr = (ConfigurationRecord *)ppcr[icr];
						projTempConfigChange.ChangeConfig(pcr);
						pProject->GetStrProp(P_CustomOutputSpec, strCustomOutSpec);
						if (!strCustomOutSpec.IsEmpty())
							break;
					}
				}

				BOOL bAlreadyQuoted = (strCustomOutSpec[0] == _T('"'));
				LPCTSTR pch = strCustomOutSpec;
				if (bAlreadyQuoted) pch++;
				const TCHAR c = *pch;
				BOOL bMakeRel = ((c != _T('\\') && c != _T('/') && (!(c == _T('.') && pch[1] == _T('\\'))) && c != _T('$') &&
					(!IsDBCSLeadByte(c) && (pch[1] != _T(':')))) || (IsDBCSLeadByte(c)));
//					(!(!IsDBCSLeadByte(c)) && (pch[1] == _T(':')))) || (IsDBCSLeadByte(c)));
				if (bMakeRel && bAlreadyQuoted)
				{
					strCustomOutSpec = _T("\".\\") + strCustomOutSpec.Mid(1);
				}
				else
				{
					if (bMakeRel)
						strCustomOutSpec = _T(".\\") + strCustomOutSpec;
					// quote if not already
					if (!bAlreadyQuoted)
						strCustomOutSpec = _T('\"') + strCustomOutSpec + _T('\"');
				}

				str += _T(' ');
				str += strCustomOutSpec;
			}

			POSITION pos;
			pos = mapStrCustomOut.GetStartPosition();
			while ( pos != (POSITION)NULL)
			{
				CString strCustomOut;
				void *end;
				mapStrCustomOut.GetNextAssoc(pos, strCustomOut, end);
				strOutArray.Add(strCustomOut);
			}
			qsort(strOutArray.GetData(), strOutArray.GetSize(), sizeof(CString *), CmpCStrings);
			
			int i;
			for( i=0; i<strOutArray.GetSize();i++)
			{
				CString strCustomOut;
				strCustomOut = strOutArray.GetAt(i);

				// custom build steps on files go at the front.
				// custom steps on outputs go at the back
				void *end;
				mapStrCustomOut.Lookup( strCustomOut, end);
				BOOL bAlreadyQuoted = (strCustomOut[0] == _T('"'));
				LPCTSTR pch = strCustomOut;
				if (bAlreadyQuoted) pch++;
				TCHAR c = *pch;
				BOOL bMakeRel = ((c != _T('\\') && c != _T('/') && !(c == _T('.') ) && c != _T('$') &&
					(!IsDBCSLeadByte(c) && (pch[1] != _T(':')))) || (IsDBCSLeadByte(c)));
//					(!(!IsDBCSLeadByte(c)) && (pch[1] == _T(':')))) || (IsDBCSLeadByte(c)));
				if (bMakeRel && bAlreadyQuoted)
				{
					strCustomOut = _T("\".\\") + strCustomOut.Mid(1);
				}
				else
				{
					if (bMakeRel)
						strCustomOut = _T(".\\") + strCustomOut;
					// quote if not already
					if (!bAlreadyQuoted)
						strCustomOut = _T('\"') + strCustomOut + _T('\"');
				}

				if( (BOOL)end ){
					str += _T(' ');
					str += strCustomOut;
				} else {

					str = strCustomOut + _T(' ') + str;
				}
			}
		}

		// If one of the ALL dependencies is built by a custom build
		// rule, then we need to write out any macros used by the
		// custom build rule (since the macros are used to specify the
		// dependency).
		WriteCustomBuildMacros(mw, str);


		// add in any per-target dependencies
		CObList list;
		FlattenSubtree(list, flt_Normal | flt_ExcludeGroups | flt_RespectItemExclude | 
						flt_ExcludeDependencies | flt_RespectTargetExclude);

		CString strNoDeps = str;
		BOOL bDeps = FALSE;
		POSITION pos = list.GetHeadPosition();
		CProjectDependency* pProjDep;
		while (pos != NULL)
		{
			pProjDep = (CProjectDependency *)list.GetNext(pos);
			if (pProjDep->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
			{
				// Add to the master deps line
                ConfigurationRecord * pcrProjDep = pProjDep->GetTargetConfig();

                if (pcrProjDep != NULL)
				    str = _T('"') + pcrProjDep->GetConfigurationName() + _T("\" ") + str;

				bDeps = TRUE;
			}
		}
		if ( GetProjType()->IsSupported() ){

			if (!bPostBuildDep)
			{
				if (bDeps)
				{
					CString strIf;
					strIf = _TEXT("\r\n!IF \"$(RECURSE)\" == \"0\" ");
					mw.WriteString(strIf);
					mw.WriteDesc(_TEXT ("ALL"), strNoDeps);
					strIf = _TEXT("\r\n!ELSE ");
					mw.WriteString(strIf);
					mw.WriteDesc(_TEXT ("ALL"), str);
					strIf = _TEXT("\r\n!ENDIF ");
					mw.WriteString(strIf);
					mw.EndLine();
				}
				else
				{
 					ASSERT(str == strNoDeps);
					mw.WriteDesc(_TEXT ("ALL"), str);
					mw.EndLine();
				}
			}
			else
				mw.WriteDesc(_TEXT ("$(DS_POSTBUILD_DEP)"), str);

			// write the CLEAN : line
			if (!bPostBuildDep)
			{
				CFileRegSet setOutputs;
				DWORD dw = (DWORD)&setOutputs;
				g_buildengine.GetDepGraph(GetActiveConfig())->PerformOperation(CFileDepGraph::GetAllOutputExceptCustom, dw);

				CObList fileItems;
				CActionSlobList lstActions;
				CProjItem * pItem = this;
				fileItems.AddHead(this); // CProject at head defines ActiveConfig

				FileRegHandle frh;
				setOutputs.InitFrhEnum();
				while ((frh = setOutputs.NextFrh()) != (FileRegHandle)NULL)
				{
					if ((g_buildengine.GetDepGraph(GetActiveConfig())->RetrieveOutputActions(lstActions, g_DummyEC, frh)==CMD_Complete) && (lstActions.GetCount() >= 1))
					{
						// okay for this to be NULL
						pItem = ((CActionSlob *)lstActions.GetHead())->Item();
					}
					else
					{
						pItem = NULL;
					}
					fileItems.AddTail(pItem); // must always add something

#ifdef REFCOUNT_WORK
					frh->ReleaseFRHRef();
#endif
				}

				if (mw.IsMakefile())
				{
					// initialise our string
					str = _TEXT("\r\n\t-@erase ");

					// get a quoted string
					MakeQuotedString(
									 &setOutputs,
									 str, &BaseDir,
									 TRUE, FilterMasterDepsLine, NULL,
									 _TEXT("\r\n\t-@erase "), FALSE	// all items
									,&fileItems
									,TRUE // sorted
									);

					if (frhCustom)
					{
						frhCustom = NULL;
						BOOL bQuote = (strCustomOutSpec[0] != _T('\"'));
						str += _TEXT("\r\n\t-@erase ");
						if (bQuote)
							str += _TEXT('\"');
						str += strCustomOutSpec;
						if (bQuote)
							str += _TEXT('\"');
					}
					
				    int i;
					for( i=0; i<strOutArray.GetSize();i++)
					{
						CString strCustomOut;
						strCustomOut = strOutArray.GetAt(i);

						BOOL bQuote = (strCustomOut[0] != _T('\"'));
						str += _TEXT("\r\n\t-@erase ");
						if (bQuote)
							str += _TEXT('\"');
						str += strCustomOut;
						if (bQuote)
							str += _TEXT('\"');
					}

					POSITION pos = list.GetHeadPosition();
					CProjectDependency* pProjDep;
					if( bDeps ){
						CString strHead;
						strHead = _TEXT("\r\n!IF \"$(RECURSE)\" == \"1\" ");
						strHead += _TEXT("\r\nCLEAN :");
						while (pos != NULL)
						{
							pProjDep = (CProjectDependency *)list.GetNext(pos);
							if (pProjDep->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
							{
								// Add to the master deps line
								ConfigurationRecord * pcrProjDep = pProjDep->GetTargetConfig();
	
								if (pcrProjDep != NULL)
									strHead += _T("\"") + pcrProjDep->GetConfigurationName() + _T("CLEAN\" ");
							}
						}
						strHead += _TEXT("\r\n!ELSE ");
						strHead += _TEXT("\r\nCLEAN :");
						strHead += _TEXT("\r\n!ENDIF ");
						str = strHead + str +_TEXT("\r\n");
					}
					else {
						str = _TEXT("\r\nCLEAN :") + str +_TEXT("\r\n");
					}
					mw.WriteString(str);
				}
			}
		}

		if (!bPostBuildDep)
			mw.EndLine();
	}
	CATCH (CException, e)
	{
		return FALSE;	// failure
	}
	END_CATCH

	return TRUE;	// success
}
///////////////////////////////////////////////////////////////////////////////
void SkipInitialWSpace(CString & strSrc, CString & strDest)
{
	// strip initial whitespace
	int cchWhiteSpace = 0;
	TCHAR * pch = (TCHAR *)(const char *)strSrc; TCHAR ch = *pch;
	while (ch != _T('\0'))
	{
		// is this whitespace?
		if (ch != _T(' ') && ch != _T('\t') &&
			ch != _T('\r') && ch != _T('\n')) break;

		cchWhiteSpace += _tclen(pch);
		ch = *(pch + cchWhiteSpace);
	}

	strDest = (TCHAR *)(const char *)strSrc + cchWhiteSpace;
}
/////////////////////////////////////////////////////////////////////////////////
void GetFirstFile (CString & strSrc, CString & strFirstFile, CString & strRemainder)
{
	// get rid of white space first
	CString	strTemp ;
	SkipInitialWSpace (strSrc, strTemp) ;
	if (!strTemp.IsEmpty() && strTemp[0]==_T('\"')) // For NTFS command args, need to handled quoted names
	{
	   	strFirstFile = _T('\"') + strTemp.Mid(1).SpanExcluding(_T("\"")) + _T('\"');
		BOOL bBatchFile = (strFirstFile.Find(_TEXT(".bat")) != -1) ||
						  (strFirstFile.Find(_TEXT(".cmd")) != -1);
		if (strFirstFile.CompareNoCase(_TEXT("\"\""))==0)
		{
			bBatchFile = (strTemp.Find(_TEXT(".bat")) != -1) ||
						  (strTemp.Find(_TEXT(".cmd")) != -1);
		}
		if (bBatchFile && (strTemp.GetAt(strTemp.GetLength()-1) == _T('\"')))
		{
			// we are looking at batch file, we need to take the whole command
			strFirstFile = strTemp;
		}
	}
	else
		strFirstFile = strTemp.SpanExcluding (_TEXT(" \r\t\n")) ;
	strRemainder = strTemp.Mid(strFirstFile.GetLength());
}
///////////////////////////////////////////////////////////////////////////////
GPT CProject::GetIntProp(UINT idProp, int& val)
{
	// If this isn't a prop we're interested in, just
	// return what our parent does.
	if (idProp != P_NoDeferredBscmake &&
		idProp != P_DeferredMecr &&
		idProp != P_Java_DebugUsing &&
		idProp != P_Java_ParamSource &&
		idProp != P_IPkgProject &&
#ifdef VB_MAKEFILES
		idProp != P_Proj_IsVB &&
#endif
		idProp != P_Java_DebugStandalone)
		return CProjContainer::GetIntProp(idProp, val);

	// UNDONE: this is much to slow
	// need to pull this into CProject!!!

	if (idProp == P_IPkgProject)
	{
		IBSProject *pBSProj = GetInterface();
		COleRef<IPkgProject> pPkgProj;
		pBSProj->QueryInterface(IID_IPkgProject, (void **)&pPkgProj);
		pBSProj->Release();
		val = (int)(IPkgProject *)pPkgProj; // REVIEW: refcount?
		return valid;
	}

#ifdef VB_MAKEFILES
	if ( idProp == P_Proj_IsVB ){
		val = m_bVB;
		return valid;
	}
#endif

	// See if this is in our prop bag.
	GPT gpt = CProjContainer::GetIntProp(idProp, val);

	// Java: we only want to check for specific Java properties if this is a Java project.

	// for EXE projects, before attempting to get the current platform, make sure the
	// platform is set; it's possible for GetStrProp to be called before P_ExtOpts_Platform is set.
	// So, if we don't know the platform, assume it's not Java.

	CString strDummyPlatform;
	if (!m_bProjIsExe || (m_bProjIsExe && GetStrProp(P_ExtOpts_Platform, strDummyPlatform) == valid))
	{

		// only check Java-specific properties if this is a Java project
		if (GetCurrentPlatform()->GetUniqueId() == java)
		{

			// do we want to fake P_Java_DebugUsing?
			if (gpt == invalid && idProp == P_Java_DebugUsing)
			{
				// if debug app using is not in our prop bag,
				// default to debug app using browser if a browser name is specified
				// otherwise default to debug using stand-alone interpreter
				CString strBrowser;
				GetStrProp(P_Java_Browser, strBrowser);
				val = strBrowser.IsEmpty() ? Java_DebugUsing_Standalone : Java_DebugUsing_Browser;
				return valid;
			}

			// do we want to fake P_Java_ParamSource?
			if (gpt == invalid && idProp == P_Java_ParamSource)
			{
				// if the param source is not in our prop bag,
				// default to HTML page, if one exists; otherwise, default to user params

				// first, set the param source to be the HTML page
				SetIntProp(P_Java_ParamSource, Java_ParamSource_HTMLPage);

				// get the HTML page
				CString strHTMLPage;
				GetStrProp(P_Java_HTMLPage, strHTMLPage);
				val = Java_ParamSource_HTMLPage;

				// if the HTML page is empty, set the param source to be the param grid (user)
				if (strHTMLPage.IsEmpty()) {

					// set the param source
					SetIntProp(P_Java_ParamSource, Java_ParamSource_User);
					val = Java_ParamSource_User;
				}

				return valid;
			}

			// do we want to fake P_Java_DebugStandalone?
			if (gpt == invalid && idProp == P_Java_DebugStandalone)
			{
				// default to application debugging
				val = Java_DebugStandalone_Application;
				return valid;
			}

		}
	}

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
	if (idProp == P_ProjSccProjName) {
		val = m_strProjSccProjName;
		return valid;
	}
	else if (idProp == P_ProjSccRelLocalPath) {
		val = m_strProjSccRelLocalPath;
		return valid;
	}
#ifdef VB_MAKEFILES
	else if (idProp == P_VBProjName) {
		if( !m_bVB )
			return invalid;

		val = m_strVBProjFile;
		return valid;
	}
#endif
//	else if (idProp == P_ProjSccProjAux) {
//		val = m_strProjSccProjAux;
//		return valid;
//	}
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
	else if (idProp == P_ProjItemDate)
	{
		const CPath *pPath = GetFilePath();
		if (pPath != NULL)
		{
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
					val = strTime + _TEXT(" ") + strDate;
				}
				else
					VERIFY(val.LoadString(IDS_PROJ_TRG_NONEXISTANT));

#ifdef REFCOUNT_WORK
				frh->ReleaseFRHRef();
#endif
				return valid;
			}
		}
		return invalid;
	}
	else if ((m_optbeh & OBShowMacro) &&
			((idProp==P_OutDirs_Intermediate) || (idProp==P_OutDirs_Target)))
		{
			return (GetOutDirString(val, idProp)) ? valid : invalid;
		}
	else if (m_bProjIsExe &&
		 	 (idProp == P_ExtOpts_CmdLine || idProp == P_ExtOpts_RebuildOpt)
			)
	{
		// These props aren't valid for an external "EXE" makefile.
		return invalid;
	}

	// At this point, we're only interested in a couple of props.  If
	// this isn't one of those, return what our base class does.
	if (idProp != P_Caller && 
		idProp != P_PromptForDlls &&
		idProp != P_WorkingDirectory &&
		idProp != P_Args && 
		idProp != P_RemoteTarget &&
        idProp != P_Proj_Targ &&
        idProp != P_ExtOpts_Targ &&
		idProp != P_Java_ClassFileName &&
		idProp != P_Java_Browser &&
		idProp != P_Java_Standalone &&
		idProp != P_Java_HTMLPage &&
		idProp != P_Java_TempHTMLPage &&
		idProp != P_Java_Args &&
		idProp != P_Java_HTMLArgs &&
		idProp != P_Java_StandaloneArgs
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
		ASSERT(!m_bProjIsExe);

		// HACK - by default, set this check box
		int nVal;
		GPT gpt = CProjContainer::GetIntProp(P_PromptForDlls, nVal);
		if (gpt == invalid) {
 			CProjContainer::SetIntProp(P_PromptForDlls, 1);
		}

		int iAttrib = GetTargetAttributes();

		// only use the target if we think it is an .EXE
		if (iAttrib & ImageExe)
		{
		// link to linker/lib output name. Can't do GetTargetFileName in setting dialog because not yet committed
	
			CPath path;	
			COptionHandler * popthdlr;
			CString strOutput;

			if(iAttrib & ImageXbe) {
				/* This is an Xbox image, so see if we can find the XBE builder
				 * and ask it what its output is */
				/* THIS IS A HACK IF I EVER SAW ONE */
				WORD idAddOnXbox =
					g_prjcompmgr.GenerateBldSysCompPackageId("Microsoft Xbox C/C++ v1.0");
				if(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnXbox,
						523), (CBldSysCmp*&)popthdlr))
					GetStrProp(popthdlr->MapLogical(0), strOutput);
			}

			if(strOutput.IsEmpty()) {
				VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Linker),
														 (CBldSysCmp *&)popthdlr));
				GetStrProp(popthdlr->MapLogical(P_OutName), strOutput);
			}
			if (!strOutput.IsEmpty())
			{
				path.CreateFromDirAndFilename(GetWorkspaceDir(), strOutput);
				val = path.GetFullPath();
				return valid;
			}
			
		}
	}

	// Java: we only want to check for specific Java properties if this is a Java project.

	// for EXE projects, before attempting to get the current platform, make sure the
	// platform is set; it's possible for GetStrProp to be called before P_ExtOpts_Platform is set.
	// So, if we don't know the platform, assume it's not Java.

	CString strDummyPlatform;
	if (!m_bProjIsExe || (m_bProjIsExe && GetStrProp(P_ExtOpts_Platform, strDummyPlatform) == valid))
	{

		// only check Java-specific properties if this is a Java project
		if (GetCurrentPlatform()->GetUniqueId() == java)
		{

			// do we want to fake P_Java_Browser?
			if (gpt == invalid && idProp == P_Java_Browser)
			{
				GetBrowserPath(val);
				// set this property so we don't have to compute it every time
				SetStrProp(P_Java_Browser, val);

				return valid;
			}

			// do we want to fake P_Java_Standalone?
			if (gpt == invalid && idProp == P_Java_Standalone)
			{
				GetStandalonePath(val);
				// set this property so we don't have to compute it every time
				SetStrProp(P_Java_Standalone, val);

				return valid;
			}

			// do we want to fake P_Java_HTMLPage?
			if (idProp == P_Java_HTMLPage)
			{
				// what is the param source?
				int nParamSource = Java_ParamSource_Unknown;
				GetIntProp(P_Java_ParamSource, nParamSource);
				if (nParamSource == Java_ParamSource_HTMLPage)
				{

					// fake this property if it doesn't already have a value
					if (gpt == invalid)
					{
						CPath pathHTMLPage;

						// first try the class name.html
						CString strClassName;
						GetStrProp(P_Java_ClassFileName, strClassName);
						pathHTMLPage.CreateFromDirAndFilename(GetProjDir(), strClassName + _T(".html"));
						if (pathHTMLPage.ExistsOnDisk())
						{
							val = pathHTMLPage.GetFileName();
						}
						else
						{
							// try the project name.html
							const CPath* ppathProj = GetFilePath();
							CString strBaseName;
							ppathProj->GetBaseNameString(strBaseName);
							pathHTMLPage.CreateFromDirAndFilename(GetProjDir(), strBaseName + _T(".html"));
							if (pathHTMLPage.ExistsOnDisk())
							{
								val = pathHTMLPage.GetFileName();
							}
							else
							{
								val = "";
							}
						}

						// store the HTML name in the P_Java_HTMLPage prop
						SetStrProp(P_Java_HTMLPage, val);

						return valid;
					}
				}
				else
				{
					// this prop is invalid except when param source is from HTML page
					val = "";
					return invalid;
				}
			}

			// do we want to fake P_Java_TempHTMLPage?
			if (gpt == invalid && idProp == P_Java_TempHTMLPage)
			{
				// generate a unique name
				TCHAR szTempHTMLPage[_MAX_PATH];
				if (GetTempFileName((LPCTSTR)GetProjDir(), _T("VJ"), 0, szTempHTMLPage))
				{

					// delete the file that's created on the disk
					VERIFY(DeleteFile(szTempHTMLPage));

					// create a path
					CPath pathTempHTMLPage;
					pathTempHTMLPage.Create(szTempHTMLPage);

					// change the extension to .html
					pathTempHTMLPage.ChangeExtension(_T(".html"));

					val = pathTempHTMLPage.GetFullPath();

					// store the temp name in the P_Java_TempHTMLPage prop
					SetStrProp(P_Java_TempHTMLPage, val);
				}
				else
				{
					val = "";
				}

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
	ConfigurationRecord *pcr;

	// Property bag list
	CPtrList m_listPropBag;

	if (idProp == P_ProjSccProjName) {
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
	else if (idProp == P_ProjSccRelLocalPath) {
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
//	else if (idProp == P_ProjSccProjAux) {
//		m_strProjSccProjAux = val;
//		return TRUE; //??
//	}
	// is the configuration changing?
	else if (idProp == P_ProjActiveConfiguration)
	{
		// is this different to our current config.?
		if (m_strProjActiveConfiguration.CompareNoCase(val) != 0)
		{
			BOOL fInformOfConfigChanged = FALSE;
 
			// do we have this configuration?
			if ((pcr = ConfigRecordFromConfigName(val)) == NULL)
			{
				// we must be initialising the project
				pcr = CreateConfigRecordForConfig(val);
 			}
			else
			{
				// this is changing most likely due to user-interaction
				// with UI that allows the picking of a config.
				// from a list of choices

				// can we do this?
				if (!theApp.NotifyPackages(PN_QUERY_CHANGE_CONFIG))
				{
					// if we have the prop. browser up make sure
					// we undo the recent control change
					RefreshTargetCombos();
					InformDependants(idProp);
					return FALSE;
				}

				fInformOfConfigChanged = m_bPrivateDataInitialized && (FindProjWksIFace()->IsWorkspaceInitialised() == S_OK);
			}

			// set ourselves in this config. (update the view)
			SetActiveConfig(pcr, FALSE);

		 	// do we want to inform the packages of this config. change?
			if (fInformOfConfigChanged && m_bNotifyOnChangeConfig)
 				theApp.NotifyPackages(PN_CHANGE_CONFIG);
 
			// Update the deferred tool menu items
			UpdateDeferredCmds(FALSE);
		}
	}

	// is the target file name changing?
	else if (idProp == P_ExtOpts_Targ || idProp == P_Proj_Targ)
	{
		// Cannot set the property to an empty string, as we must fake it.
		CString temp = val;
		temp.TrimLeft();
		temp.TrimRight();
		if (temp.IsEmpty())
		{
			CPropBag * pBag = GetPropBag();
			ASSERT(pBag != (CPropBag *)NULL);

			pBag->RemoveProp(idProp);
			InformDependants(P_ProjItemFullPath);

			return TRUE;
		}

		InformDependants(P_ProjItemFullPath);
	}
#ifdef VB_MAKEFILES
	else if (idProp == P_VBProjName) {
		m_strVBProjFile = val;
		return TRUE;
	}
#endif

	// do the base-class (CProjContainer) thing
	return CProjContainer::SetStrProp(idProp, val);
}

BOOL CProject::SetIntProp(UINT idProp, int val)
{
#ifdef VB_MAKEFILES
	if ( idProp == P_Proj_IsVB ){
		m_bVB = val;
		return TRUE;
	}
#endif
	return CProjContainer::SetIntProp (idProp, val);
}

ConfigurationRecord *CProject::CreateConfigRecordForConfig(
												const CString & strConfig)
{
	//	Create a configuration record for the supplied name:
	if (ConfigRecordFromConfigName (strConfig) != NULL) return NULL;

	ConfigurationRecord *pcr = new ConfigurationRecord (0, this);
	pcr->m_pBaseRecord = pcr;	// Record is base for itself.

	m_ConfigArray.Add(pcr);
	m_ConfigMap.SetAt ( pcr, pcr );

	PCFG_TRACE (" CProject at %p creating new config %s at %p.\n",
												this, strConfig, pcr);

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
	ConfigurationRecord * pcr;
	if ((pcr = ConfigRecordFromConfigName(strName)) == (ConfigurationRecord *)NULL)
		return FALSE;

	// killing the active configuration?
	BOOL fKillingActive = pcr == GetActiveConfig();

	// Perform the delete...
	DeleteConfigFromRecord(pcr);

	// Set the active configuration to something other than this deleted one
	if (!m_ConfigMap.IsEmpty())
	{
		ASSERT(m_ConfigArray.GetSize());
		ASSERT(m_ConfigArray.GetSize() == m_ConfigMap.GetCount());

		// Switch if we killed current....
		if (fKillingActive)
		{
			// If not got one to set as the current, then use first
			if (strNewConfig.IsEmpty())
			{
				SetStrProp(P_ProjActiveConfiguration, ((ConfigurationRecord *)m_ConfigArray[0])->GetConfigurationName());
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
	// Create a config. and make it the active one.
	ConfigurationRecord * pcrNewConfig = CreateConfigRecordForConfig(strName);
	if (pcrNewConfig == NULL)
		return FALSE;

 	m_pActiveConfig = pcrNewConfig;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
// IDE support functions:
void CProject::ScanAllDependencies ()
{
#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "devbldd.dll", "ScanAllDependencies()", letypeBegin, 0);
#endif
	OnScanAll();
#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "devbldd.dll", "ScanAllDependencies()", letypeEnd, 0);
#endif
}

BOOL CProject::ScanItemDependencies(CFileItem * pItem)
{
	g_ScannerCache.UpdateDependencies( pItem );

	// Success!
	return TRUE;
}

void CProject::SetProjectState ()
{
	// NYI
}

int CProject::GetTargetAttributes()
{
	// get the target attributes from the cache for the current config.
	ConfigurationRecord * pConfig = (ConfigurationRecord *)GetActiveConfig();
	ASSERT(pConfig);
	return pConfig->GetTargetAttributes();
}

BOOL CProject::TargetIsCaller()
{
	CPath pathCaller;

	CString strCaller;
	GetStrProp(P_Caller, strCaller);

	if (!strCaller.IsEmpty())
		pathCaller.CreateFromDirAndFilename(GetWorkspaceDir(), strCaller);

	// are they the same?
	CPath * ppathTrg = GetTargetFileName();
	if (ppathTrg == (const CPath *)NULL)
		return FALSE;

	BOOL bResult = pathCaller == *ppathTrg;
	delete ppathTrg;
	return bResult;
}

CPath * CProject::GetTargetFileName()
{
	CProjType * pProjType = GetProjType();
	FileRegHandle frh = (FileRegHandle)NULL;
	
	if (m_bProjIsExe)
	{
		CString strTargetFile;
		if (GetStrProp(P_ExtOpts_Targ, strTargetFile) == valid)
		{
			CPath path;
			if (path.CreateFromDirAndFilename(GetWorkspaceDir(), strTargetFile))
#ifndef REFCOUNT_WORK
				frh = g_FileRegistry.RegisterFile((const CPath *)&path);
#else
				frh = CFileRegFile::GetFileHandle(path);
#endif
		}
	}
	else if (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
    {
        CString strTargetFile;
        if (GetStrProp(P_Proj_Targ, strTargetFile) == valid)
        {
            CPath path;
            if (path.CreateFromDirAndFilename(GetWorkspaceDir(), strTargetFile))
#ifndef REFCOUNT_WORK
				frh = g_FileRegistry.RegisterFile((const CPath *)&path);
#else
				frh = CFileRegFile::GetFileHandle(path);
#endif
        }
    }
	else
	{
		// try looking right at our actions
		// we may not have a dependency graph so enumerating it will not work
		CActionSlobList * pActions = GetActiveConfig()->GetActionList();
		POSITION pos = pActions->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CActionSlob * pAction = (CActionSlob *)pActions->GetNext(pos);

			// primary output tool?
			if (!pAction->m_pTool->HasPrimaryOutput())
				continue;	// no

			CFileRegSet * pSet = pAction->GetOutput();
#ifndef REFCOUNT_WORK
			const CPtrList * plstFile = pSet->GetContent();

			if (plstFile->GetCount())
			{
				frh = (FileRegHandle)plstFile->GetHead();
				if (!pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
					break;
			}
#else
			if (pSet->GetCount())
			{
				if (frh != NULL)
					frh->ReleaseFRHRef();
				frh = (FileRegHandle)pSet->GetFirstFrh();
				if (!pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
					break;
			}
#endif
		}
	}

#ifndef REFCOUNT_WORK
	return frh != (FileRegHandle)NULL ? new CPath(*g_FileRegistry.GetRegEntry(frh)->GetFilePath()) : (CPath *)NULL;
#else
	CPath* pPathReturned = NULL;
	if (NULL!=frh)
	{
		pPathReturned = new CPath(*g_FileRegistry.GetRegEntry(frh)->GetFilePath());
		frh->ReleaseFRHRef();
	}
	return pPathReturned;
#endif
}

const CPath * CProject::GetBrowserDatabaseName()
{
	if (!m_bProjIsSupported)
		return NULL;

	CFileRegSet * pregset = GetTargetPaths();

	FileRegHandle frh;
	pregset->InitFrhEnum();
	while ((frh = pregset->NextFrh()) != (FileRegHandle)NULL)
	{
		const CPath * pPath = g_FileRegistry.GetRegEntry(frh)->GetFilePath();

		if (pPath && FileNameMatchesExtension (pPath, _TEXT ("bsc")))
#ifndef REFCOUNT_WORK
			return pPath;
#else
		{
			frh->ReleaseFRHRef();
			return pPath;
		}
		frh->ReleaseFRHRef();
#endif
	}

	// No bsc file!
	return NULL;
}

#include "optncplr.h"
BOOL CProject::GetPreCompiledHeaderName(CString &strPch)
{
	if (!m_bProjIsSupported)
		return FALSE;

	COptionHandler * popthdlr;
	VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Compiler),
										 (CBldSysCmp *&)popthdlr));
	// UNDONE:
	if (GetStrProp (popthdlr->MapLogical(P_PchUseUptoHeader), strPch) == valid){
		if (!strPch.IsEmpty() )
			return TRUE;
	}
	
	return FALSE;
}

void CProject::GetCallingProgramName(CString &str)
{
	// Java: the calling program name in Java is stored in the
	// P_Java_ClassFileName property instead of P_Caller

	// determine which property to use; get the platform type
	if (GetCurrentPlatform()->GetUniqueId() == java)
	{
		// For Java, we can't get the full path to the class file, because
		// it may not exist on the user's machine (i.e. it could be
		// on the web).
		GetStrProp(P_Java_ClassFileName, str);
	}
	else
	{
		CPath pt;
		CString strCaller, strQuote;
		GetStrProp(P_Caller, strCaller);

		// strip quotes before passing to CPath
		if (!strCaller.IsEmpty() && strCaller[0]==_T('\"'))
		{
			strQuote = _T('\"');
			strCaller = strCaller.Mid(1, strCaller.GetLength()-2);
		}

		if (!strCaller.IsEmpty() &&
			pt.CreateFromDirAndFilename(GetWorkspaceDir(), strCaller)
		   )
		{
			str = strQuote + (const _TCHAR *)pt + strQuote;	// reattach any quotes
		}
		else
		{
			str.Empty ();
		}
	}
}

void CProject::GetWorkingDirectory (CString &str)
{
	CString strWorkingDir;
	GetStrProp(P_WorkingDirectory, strWorkingDir);

	if (!strWorkingDir.IsEmpty())
	{
		CPath path;
		// Deal with the case where the directory specified ends with a '\', etc
		if (strWorkingDir.Right (1) == _TEXT("\\"))
			strWorkingDir = strWorkingDir.Left(strWorkingDir.GetLength () - 1);

		if (path.CreateFromDirAndFilename(GetWorkspaceDir(), strWorkingDir + _T("\\x")))
		{
			CDir dirWorking;
			dirWorking.CreateFromPath(path);

			str = dirWorking;
		}
		else
			str.Empty();
	}
	else
	{
		str.Empty();
	}
}

void CProject::GetProgramRunArguments (CString &str)
{
	// if this is a Java project, we'll return P_Java_Args
	GetStrProp((GetCurrentPlatform()->GetUniqueId() == java) ? P_Java_Args : P_Args, str);
}

void CProject::SetProgramRunArguments (CString &str)
{
	SetStrProp(P_Args, str);
}

BOOL CProject::FPromptForDlls (void)
{
	int fPrompt;
	GetIntProp(P_PromptForDlls, fPrompt);
	return (BOOL) fPrompt;
}

VOID CProject::SetPromptForDlls (BOOL fPrompt)
{
	SetIntProp(P_PromptForDlls, fPrompt);
}

void CProject::GetJavaClassName(CString& str)
{
	GetStrProp(P_Java_ClassFileName, str);
}

void CProject::GetJavaClassPath(CString& str)
{
	// concatonate: output directory + project directory + extra class path + class path

	// get the directory manager
	CDirMgr* pDirMgr = GetDirMgr();

	// get the class path
	pDirMgr->GetDirListString(str, DIRLIST_INC);

	// get the extra class path
	CString strExtraClassPath;
	GetStrProp(P_Java_ExtraClasses, strExtraClassPath);
	if (!strExtraClassPath.IsEmpty())
	{
		str = strExtraClassPath + _T(";") + str;
	}

	// get the output directory
	CString strOutputDir;
	GetStrProp(P_OutDirs_Target, strOutputDir);

	// get the project directory
	CString strProjDir = GetProjDir();
	if (!strProjDir.IsEmpty())
	{
		str = strProjDir + _T(";") + str;

		if (!strOutputDir.IsEmpty())
		{
			// Only prepend the project dir if the output dir is relative.

			// In order for CPath to work right, it has to have a filename, so
			// append a bogus one here.
			// (We use CPath because it's the safest way to do this path munging stuff.)
			CPath pathOutputDir;
			CDir dirProjDir;
			dirProjDir.CreateFromString(strProjDir);

			// if strOutputDir has a trailing \ then CreateFromDirAndFilename has problems 
			if (strOutputDir.Right (1) == _TEXT ("\\"))
				strOutputDir = strOutputDir.Left (strOutputDir.GetLength () - 1);
			pathOutputDir.CreateFromDirAndFilename(dirProjDir, strOutputDir + _T("\\x"));

			// We just need the path part, not the full path + file name.
			CDir dirOutputDir;
			dirOutputDir.CreateFromPath(pathOutputDir);
			strOutputDir = dirOutputDir;
		}
	}

	// Add on the output directory.
	if (!strOutputDir.IsEmpty())
	{
		str = strOutputDir + _T(";") + str;
	}
}

BOOL CProject::GetJavaClassFileName(CString& str)
{
	str.Empty();

	// get the class name
	CString strClassName;
	GetStrProp(P_Java_ClassFileName, strClassName);

	// get the class path
	CString strClassPath;
	GetJavaClassPath(strClassPath);

	// search the class path for the class
	TCHAR szClassFileName[_MAX_PATH];
	if (SearchPath(strClassPath, strClassName, _T(".class"), _MAX_PATH, szClassFileName, NULL) != 0)
	{
		str = CString(szClassFileName);
		return TRUE;
	}

	return FALSE;
}

int CProject::GetJavaDebugUsing()
{
	// this should only be called if the project is Java
	ASSERT(GetCurrentPlatform()->GetUniqueId() == java);

	int nDebugUsing = Java_DebugUsing_Unknown;
	GetIntProp(P_Java_DebugUsing, nDebugUsing);

	return nDebugUsing;
}

void CProject::GetJavaBrowser(CString& str)
{
	// this should only be called if the project is Java
	ASSERT(GetCurrentPlatform()->GetUniqueId() == java);

	CString strBrowser;
	GetStrProp(P_Java_Browser, strBrowser);

	// ensure the browser has an extension (if it doesn't add .exe)
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(strBrowser, NULL, NULL, NULL, szExt);
	if (szExt[0] == _T('\0')) {
		strBrowser += _T(".exe");
	}

	str = strBrowser;
}

void CProject::GetJavaStandalone(CString& str)
{
	// this should only be called if the project is Java
	ASSERT(GetCurrentPlatform()->GetUniqueId() == java);

	CString strStandalone;
	GetStrProp(P_Java_Standalone, strStandalone);

	// ensure the stand-alone interpreter has an extension
	// (if it doesn't add .exe)
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(strStandalone, NULL, NULL, NULL, szExt);
	if (szExt[0] == _T('\0')) {
		strStandalone += _T(".exe");
	}

	str = strStandalone;
}

void CProject::GetJavaStandaloneArgs(CString& str)
{
	// this should only be called if the project is Java
	ASSERT(GetCurrentPlatform()->GetUniqueId() == java);

	// this should only be called if we're debugging using a stand-alone interpreter
#ifdef _DEBUG
	int nDebugUsing = Java_DebugUsing_Unknown;
	GetIntProp(P_Java_DebugUsing, nDebugUsing);
	ASSERT(nDebugUsing == Java_DebugUsing_Standalone);
#endif

	GetStrProp(P_Java_StandaloneArgs, str);
}

int CProject::GetJavaStandaloneDebug()
{
	// this should only be called if the project is Java
	ASSERT(GetCurrentPlatform()->GetUniqueId() == java);

	// this should only be called if we're debugging using a stand-alone interpreter
#ifdef _DEBUG
	int nDebugUsing = Java_DebugUsing_Unknown;
	GetIntProp(P_Java_DebugUsing, nDebugUsing);
	ASSERT(nDebugUsing == Java_DebugUsing_Standalone);
#endif

	int nDebugStandalone = Java_DebugStandalone_Unknown;
	GetIntProp(P_Java_DebugStandalone, nDebugStandalone);

#ifndef STANDALONE_APPLET
	nDebugStandalone = Java_DebugStandalone_Application;
#endif

	return nDebugStandalone;
}

// this function is guaranteed to return an HTML page;
// it is intented to be used with the src package for debug/execution.
// If no page is specified by P_Java_HTMLPage, we'll generate a page with
// the name specified by P_Java_TempHTMLPage. If no page is specified
// by P_Java_TempHTMLPage, we'll generate a temporary name.
// If we're unable to generate the HTML page, this function returns FALSE.
BOOL CProject::GetJavaHTMLPage(CString& str)
{
	// this should only be called if the project is Java
	ASSERT(GetCurrentPlatform()->GetUniqueId() == java);

	// this should only be called if we're debugging using a browser
#ifdef _DEBUG
	int nDebugUsing = Java_DebugUsing_Unknown;
	GetIntProp(P_Java_DebugUsing, nDebugUsing);
	ASSERT(nDebugUsing == Java_DebugUsing_Browser);
#endif

	str.Empty();

	// do we need to generate a temporary HTML page (P_Java_ParamSource == Java_ParamSource_User)?
	int nParamSource = Java_ParamSource_Unknown;
	GetIntProp(P_Java_ParamSource, nParamSource);
	BOOL bUseTempHTMLPage = (nParamSource == Java_ParamSource_User);

	CString strHTMLPage;
	
	// are we using a user-specified HTML page (i.e. not a temp HTML page)
	if (!bUseTempHTMLPage)
	{
		// get the user's HTML page
		GetStrProp(P_Java_HTMLPage, strHTMLPage);

		// if no page is specified, use the temp HTML page
		if (strHTMLPage.IsEmpty())
		{
			bUseTempHTMLPage = TRUE;
		}
	}

	// are we using a temp HTML page
	if (bUseTempHTMLPage)
	{
		// get the temp HTML page
		GetStrProp(P_Java_TempHTMLPage, strHTMLPage);

		// if no temp name is specified, we can't return an HTML page
		if (strHTMLPage.IsEmpty())
			return FALSE;

		// set the P_Java_ParamSource prop
		// (do this because we may be using a temp HTML page
		// because the user never specified an actual HTML page)
		SetIntProp(P_Java_ParamSource, Java_ParamSource_User);
	}

	// get a full path to the page, if one's not specified
	CPath pathHTMLPage;
	if (pathHTMLPage.CreateFromDirAndFilename(GetProjDir(), strHTMLPage))
	{
		strHTMLPage = pathHTMLPage.GetFullPath();
	}

	// are we using a temp HTML page?
	if (bUseTempHTMLPage)
	{
		// if the temporary file is read only, generate a new temporary file
		CFileStatus statusHTMLPage;
		if (CFile::GetStatus(strHTMLPage, statusHTMLPage))
		{
			if (statusHTMLPage.m_attribute & CFile::readOnly)
			{
				// generate a new temporary file
				// generate a unique name
				TCHAR szTempHTMLPage[_MAX_PATH];
				if (GetTempFileName((LPCTSTR)GetProjDir(), _T("VJ"), 0, szTempHTMLPage))
				{
					// delete the file that's created on the disk
					VERIFY(DeleteFile(szTempHTMLPage));

					// create a path
					CPath pathTempHTMLPage;
					pathTempHTMLPage.Create(szTempHTMLPage);

					// change the extension to .html
					pathTempHTMLPage.ChangeExtension(_T(".html"));

					strHTMLPage = pathTempHTMLPage.GetFullPath();

					// store the temp name in the P_Java_TempHTMLPage prop
					SetStrProp(P_Java_TempHTMLPage, strHTMLPage);
				}
			}
		}

		// generate HTML that describes the applet

		// open the file
		CFile fileTempHTMLPage;
		if (fileTempHTMLPage.Open(strHTMLPage, CFile::modeCreate | CFile::modeWrite))
		{

			// get the class file name
			CString strClassFileName;
			GetStrProp(P_Java_ClassFileName, strClassFileName);
			// get the params
			CString strHTMLArgs;
			GetStrProp(P_Java_HTMLArgs, strHTMLArgs);

			// generate the page
			CString strHTMLFormat;
			CString strT;
			int i;

			i = IDS_JAVA_TEMP_HTML_PAGE;
			while (i <= IDS_JAVA_TEMP_HTML_PAGE_LAST)
			{
				strT.LoadString(i);
				strHTMLFormat += strT;
				i++;
			}
			CString strHTML;
			strHTML.Format(strHTMLFormat, (LPCSTR)strClassFileName, (LPCSTR)strClassFileName, (LPCSTR)strHTMLArgs);

			// write out the HTML
			fileTempHTMLPage.Write(strHTML, strHTML.GetLength());

			// close the file
			fileTempHTMLPage.Close();
		}
		else
		{
			return FALSE;
		}
	}

	str = strHTMLPage;

	return TRUE;
}

BOOL CProject::ClassWizardName (CPath & path)
{
	CString strName;

    CProjType * pProjType = GetProjType();

	if (m_bProjIsExe)
	{
		// The P_ExtOpts_ClsWzdName property is now defunct
   		//VERIFY (GetStrProp (P_ExtOpts_ClsWzdName, strName) == valid);
		//if (strName.IsEmpty())
			path = *GetFilePath();	// use the basename of project
		//else
		//	path.Create (strName);				// use user-supplied name
	}
	else
	{
		if (GetStrProp (P_ProjClsWzdName, strName) != valid ||
			strName.IsEmpty()
		   )
		{
			// try to find a buildable .RC file
			CObList oblistRCFile;
			g_FileRegistry.GetFileItemList(GetActiveTarget(), g_FileRegistry.GetRCFileList(),
										   oblistRCFile);
			if (oblistRCFile.GetCount() == 1)
				path = *((CFileItem *)oblistRCFile.GetHead())->GetFilePath();	// use basename of .RC
			else
				path = *GetFilePath();	// use the basename of project
		}
		else
			path.Create (strName);			// use user-supplied name
	}

	// make sure we have the right extension
	path.ChangeExtension(".clw");
	return TRUE;
}

void CProject::GetRemoteTargetFileName(CString &str)
{
	GetStrProp(P_RemoteTarget, str);
}

void CProject::SetRemoteTargetFileName (CString &str)
{
	SetStrProp(P_RemoteTarget, str);
}

void CProject::AddFile (const CPath *pPath )
{
	FileRegHandle frh;
 	CFileItem *pItem;
	POSITION pos;
	
	CTargetItem* pTarget = GetActiveTarget();
	ASSERT(pTarget != NULL);

#ifndef REFCOUNT_WORK
	frh = g_FileRegistry.LookupFile(*pPath);
#else
	frh = CFileRegFile::LookupFileHandleByName(*pPath);
#endif
	if (frh != NULL)
	{
	 	for (pos = pTarget->GetHeadPosition (); pos != NULL;)
		{
			pItem = (CFileItem *) pTarget->GetNext (pos);
			ASSERT (pItem->IsKindOf ( RUNTIME_CLASS ( CProjItem )));
			if (pItem->GetFileRegHandle () == frh )	   // BINGO
			{
				ASSERT (pItem->IsKindOf ( RUNTIME_CLASS ( CFileItem )));
#ifdef REFCOUNT_WORK
				frh->ReleaseFRHRef();
#endif
				return;
			}
		}
#ifdef REFCOUNT_WORK
		frh->ReleaseFRHRef();
#endif
	}

	// Make the build pane active and set the focus to it
	GetBuildNode()->ShowBuildPane(TRUE);

	// N.B. must explicitly pass our ProjView, since we may not be active
	LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
	pProjSysIFace->BeginUndo(IDS_UNDO_ADD_FILES);

	pItem = new CFileItem ();
	pItem->SetFile (pPath);
	pItem->MoveInto (pTarget);

	pProjSysIFace->EndUndo();
}

BOOL CProject::IsProjectFile (const CPath *pPath )
{
	// Can't simply look to see if the file appears in the file registry,
	// since it could be on the Clipboard or on the Undo stack.
	// Second check is designed to do this.

#ifndef REFCOUNT_WORK
	FileRegHandle frh = g_FileRegistry.LookupFile(*pPath);
#else
	FileRegHandle frh = CFileRegFile::LookupFileHandleByName(*pPath);
	if (NULL!=frh)
		frh->ReleaseFRHRef();
#endif

	CTargetItem* pTarget = GetActiveTarget();
	return NULL==pTarget?FALSE:pTarget->IsFileInTarget(frh);

}

BOOL CProject::IsExeProject () const
{
	// (Added after several requests):
	// FUTURE (colint): Need to remove reliance on this as much as possible
	return m_bProjIsExe;
}

BOOL CProject::IsExternalTarget() 
{
	CProjType * pProjType = GetProjType();
	return (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)));
}

CPlatform * CProject::GetCurrentPlatform()
{
	if (m_bProjIsExe)
	{
		CString		strPlatform;
		CPlatform *	pPlatform;
		CPlatform *	pPlatformRet = NULL;

		VERIFY(GetStrProp(P_ExtOpts_Platform, strPlatform) == valid);

		// Search based on the "UI description" string (that's what's
		// stored in P_ExtOpts_Platform).

		g_prjcompmgr.InitPlatformEnum();

		while (g_prjcompmgr.NextPlatform(pPlatform))
		{
			if (*pPlatform->GetUIDescription() == strPlatform)
			{
				pPlatformRet = pPlatform;
				break;
			}
		}

		return pPlatformRet;
	}
	else
	{
		return (CPlatform *)GetProjType()->GetPlatform();
	}
}

BOOL CProject::CanContain(CSlob* pSlob)
{
	if (pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)) ||
		pSlob->IsKindOf(RUNTIME_CLASS(CTargetItem)) )
		return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Drag and Drop support

CSlobDragger* CProject::CreateDragger(CSlob* /*pDragObject*/, CPoint /*screenPoint*/)
{
	return NULL;
}

void CProject::DoDrop(CSlob * /*pSlob*/, CSlobDragger * /*pDragger */)
{
	return;
}

void CProject::DoDrag(CSlob * /*pSlob*/, CSlobDragger * /*pDragger*/, CPoint /*screenPoint*/)
{
	return;
}

BOOL CProject::AreFilesUnique(const CSlob * pSlob, BOOL bTopLevel /* = TRUE */)
{
	BOOL retval = TRUE;
	FileRegHandle hndFileReg;
	static CMapPtrToPtr mapFileRegs;
	if (bTopLevel)
	{
		ASSERT(mapFileRegs.IsEmpty());
	}
	CProjItem * pItem, *pDupItem;
	POSITION pos = ((CSlob *)pSlob)->GetContentList()->GetHeadPosition();
	while (pos != NULL)
	{
		pItem = (CProjItem *)((CSlob *)pSlob)->GetContentList()->GetNext(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
		{
			pItem = ((CBuildSlob *)pItem)->GetProjItem();
			ASSERT(pItem != NULL);
		}

		if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		{
			hndFileReg = pItem->GetFileRegHandle();
			if (mapFileRegs.Lookup((void *)hndFileReg, (void * &) pDupItem))
			{
				ASSERT(pDupItem!=pItem);
				retval = FALSE;
				break;
			}
			else
			{
				mapFileRegs.SetAt((void *)hndFileReg, (void *)pItem);
			}
		}
		else if ((pItem->IsKindOf(RUNTIME_CLASS(CProjGroup))) && (!pItem->GetContentList()->IsEmpty()))
		{
			retval = AreFilesUnique(pItem, FALSE);
			if (!retval)
				break;
		}
	}
	if (bTopLevel)
		mapFileRegs.RemoveAll();

	return retval;
}

///////////////////////////////////////////////////////////////////////////////
#ifdef EXT_FILES
BOOL CProject::BuildFilesList ()
{
	return TRUE;
}
#endif
#if (0)
BOOL SpawnToolAndGetFiles (
						const CDir &rDir,
						CPtrList &pl,
						const TCHAR *pCommandLine
						);
/*
class CPossibleGroup : public CObject
{
public:
	CPossibleGroup ( const TCHAR *pGroupName, const TCHAR *pGroupExtensions );
*/
BOOL CProject::BuildFilesList ()
{
	CPtrList pl(100);		// List of file reghangles.

	if (!SpawnToolAndGetFiles (GetWorkspaceDir(), pl, )) return FALSE;

}
BOOL SpawnToolAndGetFiles (
						const CDir &rDir,
						CPtrList &pl,
						const TCHAR *pCommandLine
						)
{
}
#endif

///////////////////////////////////////////////////////////////////////////////
// UpdateBrowserDatabase
//
// This function will update the browser database, tho' a link
// will also happen. 
// FUTURE: when all this old build system code goes perhaps
// we can just call g_buildengine.DoBuild("foobar.bsc") ;-)

// Dialog we use
class CUpdateBrowserDlg : public C3dDialog
{
	DECLARE_MESSAGE_MAP()

public:
	CUpdateBrowserDlg(CWnd* pParent /*=NULL*/)
	: C3dDialog(IDD_UPDATE_BROWSE, pParent)
	{
	}

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM, LPARAM);

	const TCHAR * m_pchBscFile;
};

BEGIN_MESSAGE_MAP(CUpdateBrowserDlg, C3dDialog)
END_MESSAGE_MAP()

BOOL CUpdateBrowserDlg::OnInitDialog()
{
	if (!C3dDialog::OnInitDialog())
		return FALSE;

	// set our title
	SetWindowText(theApp.m_pszAppName);

	// set our exclamation mark icon
	((CStatic *)GetDlgItem(IDC_UPATE_BROWSE_ICON))->SetIcon(::LoadIcon(NULL, IDI_EXCLAMATION));

	// set our .bsc file name text
	GetDlgItem(IDC_BSCNAME)->SetWindowText(m_pchBscFile);

	return TRUE;	// ok
}

BOOL CUpdateBrowserDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// not a 'response' button
	if (wParam != IDYES && wParam != IDNO && wParam != IDCANCEL)
		return C3dDialog::OnCommand(wParam, lParam);

	// end the dialog on this response
	EndDialog(wParam);
	return TRUE;
}

BOOL CProject::UpdateBrowserDatabase()
{
	// can't explicitly update the database
	// o for external targets
	// o for .EXE projects
	// o if we can't do a build for some other reason
	CProjType * pProjType;

	if (m_bProjIsExe ||
		(pProjType = GetProjType()) == (CProjType *)NULL ||
		pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)) ||
		!CanDoTopLevelBuild()
	   )
	{
	   return TRUE;		// nothing to do
	}

	// get the browser database name
	const CPath * pPath = GetBrowserDatabaseName();
	if (pPath == (const CPath *)NULL)
		return FALSE;	// don't have a project .bsc!

	BOOL fSuccess = TRUE;	// assume .bsc is up-to-date

	// is it up to date?
	UINT trgState = g_BldSysIFace.TargetState((CObList *)NULL, (const TCHAR *)*pPath);
	if (trgState == TRG_STAT_Out_Of_Date || trgState == TRG_STAT_Unknown)
	{	
		// get our browser database tool
		// and create our action
		CBuildTool * pTool;
		VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_BscMake),
											 (CBldSysCmp *&)pTool));
		CActionSlob action(this, pTool);

		// actions have browser on?
		CActionSlobList * pActions = GetActiveConfig()->GetActionList();
		CActionSlob * pAction;

		POSITION pos;

		// ask user if wants to build before browse
		CUpdateBrowserDlg dlg(theApp.m_pMainWnd);
		dlg.m_pchBscFile = (const TCHAR *)*pPath;

		// do it
		WORD wResult = dlg.DoModal();

		// do build?
		if (wResult == IDYES)
		{
			// tmp. change browser options
			pTool->SetDeferredAction(&action, dabNotDeferred);

			pos = pActions->GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				pAction = (CActionSlob *)pActions->GetNext(pos);
				pAction->m_dwEvent = TRUE;	// turn-on browse
				if (pAction->m_pTool != (CBuildTool *)NULL)
					pAction->m_pTool->OnActionEvent(ACTEVT_SetBrowse, pAction);
			}

			// possible option change due to the above set-browse change
			CActionSlob::DirtyAllCommands(GetActiveTarget());

			// do the build
			fSuccess = DoTopLevelBuild(FALSE, NULL, fcaRebuild, FALSE);
		}
	}

	return fSuccess;
}

///////////////////////////////////////////////////////////////////////////////
// UpdateRemoteTarget
//
// This function attempts to update the copy of the target on the
// remote platform. This will be called when we try to load the
// debuggee and find that the debuggee is out of date.
//
// NOTE: currently we can only do this for the mac.

BOOL CProject::UpdateRemoteTarget()
{
#if 0
	// this is no longer supported

	// Currently we can only do this for the mac
	CPlatform * pPlatform;

	// Make sure we have a remote executable name...
	if (!GetInitialRemoteTarget())
		return FALSE;

	BOOL bIsExternalTarget;
	CProjType* pProjType = GetProjType();
	bIsExternalTarget = (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)));
	if (m_bProjIsExe || bIsExternalTarget)
	{
		CString str;

		if (m_bProjIsExe)
		{
 			VERIFY(GetStrProp(P_ExtOpts_Platform, str));
			g_prjcompmgr.LookupPlatformByUIDescription(str, pPlatform);
		}
		else
			pPlatform = (CPlatform *)pProjType->GetPlatform();
 
 		if (pPlatform->GetUniqueId() == mac68k ||
			pPlatform->GetUniqueId() == macppc)
 		{
			CErrorContext *pEC;
			CEnvironmentVariableList ProjEnvironment;
			CProject::BuildResults brResult;
			DWORD errs,warns;

			// Mfile tool output file
			CPath pathOutput;
			if (!GetStrProp(bIsExternalTarget ? P_Proj_Targ : P_ExtOpts_Targ, str) ||
				!pathOutput.CreateFromDirAndFilename(GetWorkspaceDir(), str)
			   )
				return FALSE;

			// Mfile tool input file
			CPath* pPath = GetTargetFileName();
			ASSERT(NULL!=pPath);
			CString strName(pPath->GetFileName());
			delete pPath;

#ifndef REFCOUNT_WORK
			FileRegHandle frh = g_FileRegistry.RegisterFile(&pathOutput);
#else
			FileRegHandle frh = CFileRegFile::GetFileHandle(pathOutput);
#endif

			if (!theApp.NotifyPackages(PN_QUERY_BEGIN_BUILD) ||
				!g_Spawner.CanSpawn()
			   )
				return FALSE;

			// UNDONE
			// find the any old damn mfile tool we want....
			CBuildTool * ptool = (CBuildTool *)NULL;
			g_prjcompmgr.InitBuildToolEnum();
			while (g_prjcompmgr.NextBuildTool(ptool))
			{
				if (ptool->IsMecrTool())
					break;
			}
			ASSERT(ptool != (CBuildTool *)NULL);

			// Initialize the spawner and output window:
			ASSERT (!g_Spawner.SpawnActive());

			if ((pEC = g_Spawner.InitSpawn ()) == NULL)
				return FALSE;

			theApp.NotifyPackages (PN_BEGIN_BUILD, (void *) FALSE);

			ConfigCacheEnabler Cacher;	// No properties can change

			GetEnvironmentVariables ( &ProjEnvironment );
			ProjEnvironment.SetVariables ();

			CTempMaker TM;

			CActionSlob * pAction = new CActionSlob(this, ptool);

			pAction->DirtyInput(FALSE);
			CFileRegSet * psetInput = pAction->GetInput();
			psetInput->AddRegHandle(frh, FALSE);
#ifdef REFCOUNT_WORK
			frh->ReleaseFRHRef();
#endif

			CActionSlobList lstActions;
			lstActions.AddTail(pAction);

			CStringList slCommandLines, slDescriptions;
			if (!ptool->GetCommandLines(lstActions,	slCommandLines, slDescriptions, 0, *pEC))
			{
				// display the notification that the
				// 'get command-line' operation failed
				CString str;
				pEC->AddString(::MsgText(str,
										 IDS_GET_COMMAND_LINE_FAILED,
										 (const TCHAR *) *ptool->GetName ()
										));
				// log this as an error
				g_Spawner.m_dwProjError++;
			}

			// anything to do?
			if (!slCommandLines.IsEmpty())
			{
				UINT cmd = g_Spawner.DoSpawn(slCommandLines, slDescriptions, GetWorkspaceDir(), FALSE, FALSE, *pEC);

				if (cmd != CMD_Canceled && cmd != CMD_Complete)
					cmd = CMD_Error; 

				if (cmd == CMD_Canceled)
					brResult = BuildCanceled;

				else if (cmd == CMD_Complete)
					brResult = BuildComplete;

				else if (cmd == CMD_Error)
					brResult = BuildError;
			}

			delete pAction;

			TM.NukeFiles (*pEC );

			ProjEnvironment.ResetVariables ();	// May do nothing.
			g_Spawner.GetErrorCount ( errs, warns );

			pEC->AddString(_T(""));

			if (brResult == BuildCanceled)
				pEC->AddString(IDS_USERCANCELED);
			else
				pEC->AddString(MsgText(str, IDS_BUILD_COMPLETE, (const char *) strName, errs, warns));
			theApp.NotifyPackages (PN_END_BUILD, (void *) FALSE);
			g_Spawner.TermSpawn ();

			return (brResult == CProject::BuildComplete ) ? TRUE : FALSE;
		}
	}
	else
	{
		pPlatform = (CPlatform *)pProjType->GetPlatform();
		if (pPlatform->GetUniqueId() == mac68k ||
			pPlatform->GetUniqueId() == macppc)
		{
			// Get the build tools for this platform only
 			CPtrList * pToolsList = pProjType->GetToolsList();

			// Now look for the mecr/mfile tool, of which we
			// should only have one.
			CBuildTool * pTool;
 			POSITION pos = pToolsList->GetHeadPosition();
			while (pos != NULL)
			{
				pTool = (CBuildTool *)pToolsList->GetNext(pos);
				if (pTool->IsMecrTool())
				{	
					TRACE("UNDONE: Use of CBuildTool::IsMecrTool() in %s @ %d", __FILE__, __LINE__);
					ASSERT(pTool->IsKindOf(RUNTIME_CLASS(CSchmoozeTool)));
					return TopLevelRunDeferedBuildTool((CSchmoozeTool *)pTool, TRUE);
				}
			}
			return FALSE;
		}
	}
#endif
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// GetInitialRemoteTarget

BOOL CProject::GetInitialRemoteTarget(BOOL bBuild /* = FALSE */, BOOL fAlways /* = FALSE */)
{
	// don't ask for the name if we're runable or debugable!
	CProjType * pProjType = GetProjType();
	if (pProjType != (CProjType *)NULL && !(pProjType->GetAttributes() & TargetIsDebugable))
		return TRUE;	// no need to!

	// don't ask for name if not empty and we don't want to always 
	// ask for the name
	CString strRemote;
	if (GetStrProp(P_RemoteTarget, strRemote))
		if (!(fAlways || strRemote.IsEmpty()))
			return TRUE;	// no need to!

	// ask for name
	CRemoteTargetDlg dlg(bBuild);

	return dlg.DoModal() == IDOK;
}

///////////////////////////////////////////////////////////////////////////////
// GetInitialExeForDebug

BOOL CProject::GetInitialExeForDebug(BOOL bExecute /* = FALSE */)
{
	CCallingExeDlg dlg(bExecute);

	if (dlg.DoModal() == IDOK)
		return TRUE;

	// User didn't tell us what the executable file name is!
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// GetInitialJavaInfoForDebug

BOOL CProject::GetInitialJavaInfoForDebug(BOOL bExecute /*= FALSE*/)
{
	CJavaDebugInfoDlg dlg(bExecute);

	return (dlg.DoModal() == IDOK);
}

///////////////////////////////////////////////////////////////////////////////
// check if P_Profile is set or not
// NOTE: P_Profile is common linker property
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::IsProfileEnabled()
{
	BOOL bProfile ;

	// UNDONE:
	TRACE("\r\nUNDONE: Reference to P_MTLOutputTlb in %s @ %d", __FILE__, __LINE__);
	
	COptionHandler * popthdlr;
	VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_OptHdlr_Linker),
										 (CBldSysCmp *&)popthdlr));
	// UNDONE:
	if (GetIntProp (popthdlr->MapLogical(P_Profile), bProfile) == valid)
		return bProfile ;
	else
		return FALSE ;
}

///////////////////////////////////////////////////////////////////////////////
// SetActiveConfig()
///////////////////////////////////////////////////////////////////////////////
ConfigurationRecord * CProject::SetActiveConfig(ConfigurationRecord * pcr, BOOL /*fQuite*/)
{
	::EnterCriticalSection(&m_critSection);

	// make sure this is compatible with us, ie. a project-level config. rec.
	ASSERT(pcr != (const ConfigurationRecord *)NULL);
	ASSERT(pcr->m_pBaseRecord == pcr);

	ConfigurationRecord * pcrOldActive = m_pActiveConfig;
	m_pActiveConfig = pcr;

	::LeaveCriticalSection(&m_critSection);
	return pcrOldActive;	// success, return old active configuration
}

ConfigurationRecord * CProject::SetActiveConfig(const CString& strConfigName, BOOL fQuite)
{
	CString strTarget = strConfigName;
	ConfigurationRecord * pcr = FindTargetConfig(strTarget);
	if (pcr != NULL)
		return SetActiveConfig(pcr, fQuite);	// success?

	ASSERT (FALSE);
	return (ConfigurationRecord *)NULL;	// failure
}

///////////////////////////////////////////////////////////////////////////////
// FindTargetConfig() - Find the configuration matching the input string
//						and fix up the case to match the actual config name
///////////////////////////////////////////////////////////////////////////////
ConfigurationRecord * CProject::FindTargetConfig(CString & strTargetName)
{
	ConfigurationRecord * pcr;
	int i, cSize = GetPropBagCount();
	for (i = 0; i < cSize; i++)
	{
		pcr = (ConfigurationRecord *)m_ConfigArray[i];
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
	ASSERT(pProject != NULL);
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
	ASSERT(m_bCritSection);
	ChangeConfig(m_pcrOriginal);
}


void CProjTempConfigChange::ChangeConfig(ConfigurationRecord *pcr)
{
	ASSERT(m_bCritSection);
	m_pProject->SetActiveConfig(pcr);
}

void CProjTempConfigChange::ChangeConfig(const CString& strConfigName)
{
	ASSERT(m_bCritSection);
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
	ConfigurationRecord * pcr = GetActiveConfig();
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
	ConfigurationRecord* pcr;
	BOOL bFoundConfig = FALSE;
	int size = GetPropBagCount();
	for (i=0; i < size; i++)
	{
		pcr = (ConfigurationRecord*) m_ConfigArray[i];
		ASSERT(pcr);
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
	g_BldSysIFace.GetTarget(strTargetName, (HBUILDER)this);

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
// RenameTarget()
///////////////////////////////////////////////////////////////////////////////
BOOL CProject::RenameTarget(const CString& strTargetFrom, const CString& strTargetTo)
{
	// Get the old Target info structure
	CTargetItem* pTarget = GetTarget(strTargetFrom);

 	if (pTarget == NULL)
		return FALSE;

	// Remove the old entry in the map
	CString strKey = strTargetFrom;
	strKey.MakeUpper();
	m_TargetMap.RemoveKey(strKey);

	// Add the new entry	
	strKey = strTargetTo;
	strKey.MakeUpper();
	m_TargetMap.SetAt(strKey, pTarget);
	ASSERT(pTarget == GetTarget(strTargetTo));

	g_BldSysIFace.RenameTarget(strTargetFrom, strTargetTo);

	ConfigurationRecord * pcrConfig = ConfigRecordFromConfigName(strTargetFrom);
	ASSERT(pcrConfig != (ConfigurationRecord *)NULL);

	CProjTempConfigChange projTempConfigChange(this);
	projTempConfigChange.ChangeConfig(pcrConfig);
	VERIFY(SetStrProp(P_ProjConfiguration, strTargetTo));
	projTempConfigChange.Release();

	// are we renaming the active configuration?
	CString strActiveProject;
	GetStrProp(P_ProjActiveConfiguration, strActiveProject);

 	if (strTargetFrom == strActiveProject)
		SetStrProp(P_ProjActiveConfiguration, strTargetTo);

	// if we just renamed the default config, update it
	if (!m_strProjDefaultConfiguration.CompareNoCase(strTargetFrom))
		m_strProjDefaultConfiguration = strTargetTo;

	if (!m_strProjStartupConfiguration.CompareNoCase(strTargetFrom))
		m_strProjStartupConfiguration = strTargetTo;

	// Refresh the target combos
	CObList * plstCombos = CTargetCombo::GetCombos();
	POSITION pos = plstCombos->GetHeadPosition();
	while (pos != NULL)
	{
		CTargetCombo * pTargetCombo = (CTargetCombo *)plstCombos->GetNext(pos);
		pTargetCombo->RefreshTargets();
	}

	return TRUE;
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
		if (pProjType && 
		   (pProjType->GetUniqueTypeId() == CProjType::staticlib))
		{
			m_bCareAboutDefFiles = FALSE;
		}
		// Create the target item node for this target and
		// move it into the project tree
		pTarget = new CTargetItem();
	}
	else
	{
		// Mirroring a target
		pTarget = GetTarget(strMirrorFrom);
	}

	ASSERT(pTarget != NULL);

	// make this the target's config. current
	CProjTempConfigChange projTempConfigChange(this);
	projTempConfigChange.ChangeConfig(strTargetName);	

	// Set up the project type
	if (pProjType->IsSupported()) // may be adding first supported projtype
		m_bProjIsSupported = TRUE;

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
	ConfigurationRecord * pcrTarg = pTarget->GetActiveConfig(TRUE);	// create
	ASSERT(pcrTarg != NULL);
	pcrTarg->SetValid(TRUE);

	// create a build instance
	g_buildengine.AddBuildInst(GetActiveConfig());

	// reset the project's config.
	projTempConfigChange.Release();

	bRetVal = RegisterTarget(strTargetName, pTarget);

	// Get the state of the workspace window
	LPPROJECTWORKSPACE pProjSysIFace = FindProjWksIFace();
    BOOL bWorkspaceInitialised = pProjSysIFace->IsWorkspaceInitialised() == S_OK;

	// If we have done the PN_WORKSPACE_INIT and we are not initialising or destroying the
	// project and we are creating a new fileset then send a notification
	if (bWorkspaceInitialised && m_bProjectComplete && strMirrorFrom.IsEmpty())
        theApp.NotifyPackages(PN_ADD_FILESET, (void *)pTarget);

    return bRetVal;
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
	HBLDTARGET hTarget = g_BldSysIFace.GetTarget(strTargetName, (HBUILDER)this);
	HBLDTARGET hTargetNew = NO_TARGET;
	int nTargets = 0;
	CTargetItem* pTarget = GetTarget(strTargetName);
	ASSERT(pTarget != NULL);
	const CPtrArray* pConfigArray = pTarget->GetConfigArray();
	int nSize = pConfigArray->GetSize();
	for (int i = 0; i < nSize; i++)
	{
		ConfigurationRecord* pcr = (ConfigurationRecord*)pConfigArray->GetAt(i);
		if (pcr->IsValid())
		{
			nTargets++;
			if (hTargetNew == NO_TARGET)
			{
				// remember the target other than to be delted one
				CString strConfigName = pcr->GetConfigurationName();
				hTargetNew = g_BldSysIFace.GetTarget(strConfigName, (HBUILDER)this);
				if (hTargetNew == hTarget)
					hTargetNew = NO_TARGET;	
			}
		}
	} 
	
	// make this the target's config. current
	CProjTempConfigChange projTempConfigChange(this);
	ConfigurationRecord * pcrOld = GetActiveConfig();

	projTempConfigChange.ChangeConfig(strTargetName);
	ConfigurationRecord * pcrProj = GetActiveConfig();

	ConfigurationRecord * pcrTarg = pTarget->GetActiveConfig();

	// inform the graph of this target that it is being deleted
	g_buildengine.GetDepGraph(pcrProj)->SetGraphMode(Graph_Destroying);

	if (nTargets == 1)
	{
        // If we are not initialising or destroying the project then
		// send a notification to say that we have deleted a fileset
		if (!theApp.NotifyPackages(PN_QUERY_DEL_FILESET, (void *)pTarget))
				return FALSE;

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
 
		if (m_bProjectComplete)
            theApp.NotifyPackages(PN_DEL_FILESET, (void *)pTarget);

		// move this old target out of the project
		// (this will un-assign the build actions)
		pTarget->MoveInto(NULL);
	}
	else
	{
		// un-assign the build actions for this mirored target
		CActionSlob::UnAssignActions(pTarget, (CPtrList *)NULL, pcrProj);
	}

	// remove pending dep update for this target
	RemoveItemsFromUpdateQueue((UINT)hTarget);

	// delete the build instance (graph + project)
	g_buildengine.RemoveBuildInst(pcrProj);

	// Reset only if pcrOld is not NULL, it might have been deleted already.
	projTempConfigChange.Release(pcrOld != (const ConfigurationRecord *)NULL);

	// filter the node that represent the target to be deleted if there is more target to show
	CBuildNode* pBldNode = GetBuildNode();
	CBuildViewSlob * pTargetSlob;
	pBldNode->InitBldNodeEnum();
	while (pBldNode->NextBldNodeEnum((void **)&pTargetSlob))
	{
		if (pTargetSlob->GetTarget() == hTarget)
		{
			if (nTargets > 1)
			{
				pTargetSlob->SetFilterTarget(hTargetNew);
				break;
			}
		}
	}

	BOOL bRetVal = DeregisterTarget(strTargetName, pTarget);

	// If the delete operation succeeded then save the target
	// so other packages can be notified.
	HBLDTARGET hSaveTarget;
	if (m_bProjectComplete)
		hSaveTarget = g_BldSysIFace.GetTarget(strTargetName, (HBUILDER)this);

	// Delete the configuration associated with this target
	bRetVal = bRetVal && DeleteConfig(strTargetName, "");

	// Refresh the target combos
	CObList * plstCombos = CTargetCombo::GetCombos();
	POSITION pos = plstCombos->GetHeadPosition();
	while (pos != NULL)
	{
		CTargetCombo * pTargetCombo = (CTargetCombo *)plstCombos->GetNext(pos);
		pTargetCombo->RefreshTargets();
	}

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

void CProject::EndBatch()
{
	if (--m_nBatch <= 0)
	{
		CString strTarget;
		CTargetItem * pTarget;
		CString strProject;
		HBUILDER hBld;

	 	// For all filesets we force the corresponding target items
		// to empty their notfication lists
		// iterate over all projects
		g_BldSysIFace.InitBuilderEnum();
		while ((hBld = g_BldSysIFace.GetNextBuilder(strProject, TRUE)) != NO_BUILDER)
		{
			g_BldSysIFace.InitFileSetEnum(hBld);
			HFILESET hFileSet = g_BldSysIFace.GetNextFileSet(hBld);
			while (hFileSet)
			{
				pTarget = g_BldSysIFace.CnvHFileSet(hBld, hFileSet);
				pTarget->EmptyNotifyLists();
				hFileSet = g_BldSysIFace.GetNextFileSet(hBld);
			}
		}
	
		// Turn off batching
		m_nBatch = 0;
	}
}
void CProject::DirtyProjectEx()
{
	ASSERT(!m_bProjectDirty);
	m_bOkToDirty = FALSE; // prevents recursion
	if (m_bProjectComplete)
	{
		const CPath * pPath = GetFilePath();
		if (pPath->IsReadOnlyOnDisk() &&
			(!m_bProjIsExe) &&
			(_tcsicmp(pPath->GetExtension(), ".exe") != 0))
		{
			BOOL bIsSccActive = (g_pSccManager->IsBuilderControlled(HPROJECT(this)) == S_OK);

			CStringArray files;
			files.Add((const TCHAR*)*pPath);
			// if (g_bAlwaysExportMakefile)  // always do this
			{
				CPath path = *pPath;
				path.ChangeExtension(_T(".mak"));
				if (path.IsReadOnlyOnDisk() && (g_pSccManager->IsControlled((const TCHAR *)path) == S_OK))
				{
					files.Add((const TCHAR*)path);
				}
				path.ChangeExtension(_T(".dep"));
				if (g_bAlwaysExportDeps && path.IsReadOnlyOnDisk() && (g_pSccManager->IsControlled((const TCHAR *)path) == S_OK))
				{
					files.Add((const TCHAR*)path);
				}
			}
			if ((!m_bProjExtConverted) && ((!bIsSccActive) || (g_pSccManager->CheckOutReadOnly(files, FALSE, FALSE) != S_OK) || (pPath->IsReadOnlyOnDisk())))
			{
				CString strMsg;
				MsgBox(Information, MsgText(strMsg, IDS_WARN_PROJ_READ_ONLY, (const TCHAR *)*pPath));
			}
		}
	}
	m_bOkToDirty = TRUE;
	m_bProjectDirty = TRUE;
}

CDir & CProject::GetProjDir(ConfigurationRecord * pcr)
{
    if (pcr == NULL)
        pcr = GetActiveConfig();

    // Make sure we are dealing with a base config record
    pcr = (ConfigurationRecord *)pcr->m_pBaseRecord;

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
	strBeginMacros.LoadString(IDS_MAK_CUSTOM_BEGIN);
	if (_tcsnicmp(strBeginMacros, pc, strBeginMacros.GetLength()) == 0) {
		return TRUE;
	}
	return FALSE;
}

CCustomBuildTool g_DefaultCustomBuildTool;

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
	strEndMacros.LoadString(IDS_MAK_CUSTOM_END);
	if (_tcsnicmp(strEndMacros, pc, strEndMacros.GetLength()) == 0) {
		bResult = TRUE;
	}

	delete pObject;
	return bResult;
}

BOOL CProject::WriteCustomBuildMacros(CMakFileWriter& mw, CString &strFiles )
{
	// Iterate through all the actions/tools for the current configuration
	// and set up the array of macros to write out.

	// The array of macros (fMacros) is a set of all possible macros.
	// Only those flagged are written to the makefile. This allows us
	// to easily combine multiple custom build rules and filter out
	// macros we don't want.

	BYTE fMacros[IDMACRO_LAST-IDMACRO_FIRST];
	memset(fMacros, 0, sizeof(BYTE)*(IDMACRO_LAST-IDMACRO_FIRST));

	// mark all macros unused initially
	SetMacroIdUsage(FALSE);
	
    // We need a custom build step from this project anyone will do.
	// This only checks the project level.
	CActionSlob* pCustomAction = NULL;
	CActionSlobList* pActions = GetActiveConfig()->GetActionList();
	for (POSITION pos = pActions->GetHeadPosition(); pos != NULL; ) {
		// get the action
		CActionSlob * pAction = (CActionSlob *)pActions->GetNext(pos);
		pCustomAction = pAction;
		break;

#if 0

		// get the action's tool
		CBuildTool* pTool = pAction->m_pTool;

		// macros are only valid for custom build tools
		if (pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool))) {

			pCustomAction = pAction;

			// To figure out which macros are used, we get the command
			// and output strings for the custom build rule, mark all
			// macros as unused, expand the macros (which marks those
			// used), and then call GetMacroIdUsage.
			
			// get custom build outputs
			CProjItem* pItem = pAction->Item();
			CString strOutput;
			if (!((CCustomBuildTool*)pTool)->GetCustomBuildStrProp(pItem, P_CustomOutputSpec, strOutput))
				continue;	// can't get the outputs
		}

#endif

	}

	if (ExpandMacros((CString *)NULL, strFiles, NULL)) {

		// write out any macros the custom build rule might need
		for (int idMacro = IDMACRO_FIRST; idMacro <= IDMACRO_LAST; idMacro++) {
			// Don't write IDEDIR (it's in the environment).
			// Only write macros that are used.
			// Always write INPUTFILE because it's always used by $(SOURCE)
			if (idMacro != IDMACRO_IDEDIR &&
				(idMacro == IDMACRO_INPTFILE || GetMacroIdUsage(idMacro))) {
				fMacros[idMacro-IDMACRO_FIRST] = TRUE;
			}
		}
	}
			
	// Filter out those macros we don't want.
	// For now, we don't want: IDMACRO_INPTDIR, IDMACRO_INPTFILE, IDMACRO_INPTBASE
	fMacros[IDMACRO_INPTDIR-IDMACRO_FIRST] = 0;
	fMacros[IDMACRO_INPTFILE-IDMACRO_FIRST] = 0;
	fMacros[IDMACRO_INPTBASE-IDMACRO_FIRST] = 0;

	// determine if there are any macros to write
	BOOL bMacros = FALSE;
	for (int i = 0; i < IDMACRO_LAST-IDMACRO_FIRST; i++) {
		if (fMacros[i]) {
			bMacros = TRUE;
			break;
		}
	}

	// write out the macros
	if (pCustomAction && bMacros) {
		CString strComment;
		strComment.LoadString(IDS_MAK_CUSTOM_BEGIN);
		mw.WriteComment(strComment);
		g_DefaultCustomBuildTool.WriteMacros(mw, fMacros, pCustomAction);
		// ((CCustomBuildTool*)(pCustomAction->m_pTool))->WriteMacros(mw, fMacros, pCustomAction);
		strComment.LoadString(IDS_MAK_CUSTOM_END);
		mw.WriteComment(strComment);
	}

	return TRUE;
}

CPath CProject::GetMrePath(ConfigurationRecord* pcr)
{
	CPath pathIdb;

	pcr = (ConfigurationRecord*)pcr->m_pBaseRecord;

	// Get compiler tool.
	CBuildTool* pTool = NULL;
	CActionSlobList* pActions = pcr->GetActionList();
	if (NULL==pActions)
		return pathIdb;

	POSITION pos = pActions->GetHeadPosition();
	if (NULL==pos)
		return pathIdb;

	// Get CActionSlob* from pcr that matches this tool.  Assume that
	// the CActionSlobList can have multiple CActionSlob's since we
	// may be allowing more than one tool to be associated with a tool
	// eventually.
	while (NULL!=pos)
	{
		CActionSlob* pLastAction = (CActionSlob*)pActions->GetNext(pos);

		if (pLastAction->m_pTool->IsKindOf(RUNTIME_CLASS(CCCompilerTool)))
		{
			pTool = pLastAction->m_pTool;
			break;
		}
	}

	if (NULL==pTool)
		return pathIdb;


	// Get filename of minimal rebuild file(vc40.idb).
	CString strIdbFile;
	if (GetStrProp (pTool->MapLogical(P_OutputDir_Pdb), strIdbFile) == valid && !strIdbFile.IsEmpty() )
	{
		// Create target directory.
		CDir dirProj = GetWorkspaceDir();

        /* If this is an Xbox project, we need to use vc70 instead of
         * vc60 */
    	CProjType * pprojtype;
        pprojtype = pcr->m_pOwner->GetProjType();
        ASSERT(pprojtype);
        if(pprojtype && pprojtype->GetPlatform()->GetUniqueId() == xbox)
            strIdbFile += "vc70";
        else
		    strIdbFile += CActionSlob::szDefVCFile;
		VERIFY(pathIdb.CreateFromDirAndFilename(dirProj,strIdbFile));
		pathIdb.ChangeExtension(_T(".idb"));
	}
	return pathIdb;
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
	const CPath * pPath;
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

const CString & CProject::GetTargetName() { return m_bProjIsExe ? GetActiveConfig()->GetConfigurationName() : m_strTargetName; }

void ProjNameToWksName(CPath &rPath)
{
	// Get proper workspace dir
	ASSERT(g_pProjWksIFace);
	LPCSTR pszPath;
	VERIFY(SUCCEEDED(g_pProjWksIFace->GetWorkspaceDocPathName(&pszPath)));
	CPath pathWkspc;
	if ((pszPath!=NULL) && (*pszPath) && (pathWkspc.Create(pszPath)))
		rPath = pathWkspc;
	else
		rPath.ChangeExtension(WORKSPACE_EXT); // default for brand new workspace
}

///////////////////////////////////////////////////////////////////////////////
// WinslowF   Adding profiler back.

void CProject::OnUpdateProfile (CCmdUI* pCmdUI)
{
CString strMenuText;

	strMenuText.LoadString (FProfiling() ? IDS_STOP_PROFILE : IDS_PROFILE);

	pCmdUI->SetText (strMenuText);

	COleRef<IBuildSystem> pBldSysIFace;
	theApp.FindInterface(IID_IBuildSystem, (void**) pBldSysIFace.InOut());
// crappy to findinterface(self) but at least it fixes leak

	UINT tfl;
	UINT iTL;
	uniq_platform upID;
	COleRef<IBuildPlatforms> pIBldPlatforms;
	
	if (SUCCEEDED(theApp.FindInterface(IID_IBuildPlatforms, (void **)pIBldPlatforms.InOut()))) {
		pIBldPlatforms->GetCurrentPlatform(ACTIVE_BUILDER, &upID);
		pIBldPlatforms->GetPlatformLocalTLIndex(upID, &iTL);
//	    ASSERT(iTL != -1);	// iTL==-1 for unsupported platforms.
		if (iTL == -1)
		{
			pCmdUI->Enable(FALSE);
			return;
		}
		pIBldPlatforms->GetPlatformTLFlags(upID, iTL, &tfl);
	}
	
	BOOL fMtrcRemoteTL = !(tfl & TFL_LOCAL);

	BOOL g_fProfilerInstalled = TRUE;

	// Is the profiler installed?
	CString strProf;
	strProf.LoadString(INI_ProfilerInstalled);
	g_fProfilerInstalled = theApp.GetProfileInt("General", strProf, 0);

	int iAttrs;
	// now we want to keep the profile menu enabled al the time.
	pCmdUI->Enable (g_fProfilerInstalled && (FProfiling() ||
					((pBldSysIFace != NULL && (pBldSysIFace->IsActiveBuilderValid() == S_OK) &&
						(SUCCEEDED(pBldSysIFace->GetTargetAttributes(ACTIVE_BUILDER, &iAttrs))) &&
							(iAttrs & ImageExe))&&
								/*(pBldSysIFace->IsProfileEnabled(ACTIVE_BUILDER) == S_OK) && */
									!(pIBldPlatforms && fMtrcRemoteTL))));

}


///////////////////////////////////////////////////////////////////////////////
void CProject::OnProfile()
{
	CProfileDlg pDlg;

	// no build system or active project?
	COleRef<IBuildSystem> pBldSysIFace; // makes no sense in build package JAYK
	theApp.FindInterface(IID_IBuildSystem, (void **)pBldSysIFace.InOut());

	if (pBldSysIFace == NULL || (pBldSysIFace->IsActiveBuilderValid() != S_OK))
	{
		InformationBox(IDS_NO_PROFILE);
		return;
	}

	if (FProfiling())
	{
		ASSERT(g_pSpawner != NULL);
		g_pSpawner->CancelSpawn();
	}
	else
	{
		int iBuildType;
		pBldSysIFace->GetBuilderType(ACTIVE_BUILDER, &iBuildType);
		if (iBuildType == InternalBuilder)
		{
			UINT uState;
			pBldSysIFace->TargetState(&uState, NULL, NULL, NO_TARGET, ACTIVE_BUILDER);
			if (pBldSysIFace->IsProfileEnabled(ACTIVE_BUILDER) == S_OK && (uState == TRG_STAT_Current))
				pDlg.DoModal();
			else
				InformationBox(IDS_NO_PROFILE);
		}
		else
		{
			pDlg.DoModal();

		}
	}
}

// the following code is for loading old MDP and VCP files -- do not modify

BOOL CProject::LoadFromVCP(CStateSaver& stateSave)
{
	BOOL bRetVal;

	// Figure out if the .mak is newer than the .mdp.
	ASSERT(stateSave.GetFileName());
	
	CPath  MDPPath;
	BOOL bNewerMak = TRUE;

	if (MDPPath.Create(stateSave.GetFileName()) && MDPPath.ExistsOnDisk())
	{
		FILETIME ftMdp, ftMak;

		VERIFY(MDPPath.GetFileTime(&ftMdp));

		CFileRegFile * pRegFile = (CFileRegFile *)g_FileRegistry.GetRegEntry(m_hndFileReg);
		if (pRegFile->GetFileTime(ftMak))
		{
		 	if (CFileRegEntry::MyCompareFileTime(&ftMak, &ftMdp) <= 0)
				bNewerMak = FALSE;
		}
	}

	// If the .mak is newer than the .mdp, the cached CActionSlob option strings
	// are not valid. Tell the CActionSlob's to ignore them. 

	CActionSlob::IgnoreCachedOptions(TRUE);

  	// Try to load from both streams.
	bRetVal = LoadExternalFromVCP(stateSave, IsExeProject());
	bRetVal = LoadInternalFromVCP(stateSave, IsExeProject()) && bRetVal;
 
 	// Note: At this point, if bRetval is false, then the VCP file is only
	// partially loaded and we may be in some sort of weird state, especially
	// w.r.t Conifiguration.
	if (bRetVal)
	{
		ASSERT_VALID (this);

		m_bProjectComplete = TRUE;
		m_bPrivateDataInitialized = TRUE;
		m_bPrivateDataInitializedFromOptFile = TRUE;
	}
	else
	{
		COptionTreeCtl::sm_TreeState.Clear();

		if (!m_bProjIsExe)
			InitPrivateData();
		else
			InitPrivateDataForExe();
	}

	//  At this point, we can enable the change config notification
	SetActiveConfig(m_strProjActiveConfiguration, FALSE);

	m_bNotifyOnChangeConfig = TRUE;
    	if (FindProjWksIFace()->IsWorkspaceInitialised() == S_OK)
		theApp.NotifyPackages (PN_CHANGE_CONFIG) ;

	PFILE_TRACE ( "CProject::LoadFromVCP returning %i\n",bRetVal);
	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////
// Convert the vcp file for an external makefile so that the contained
// settings can be retained on a corresponding internal makefile generated
// when the external makefile is loaded.
BOOL CProject::ConvertVCPFile(CArchive & ar)
{
	CPtrList lstDebugConfigs;
    CPtrList lstReleaseConfigs;
	BOOL bRetval = TRUE;
	BOOL bDataExists = FALSE;
	CString strDebugConfig, strReleaseConfig, str;

	VERIFY(strDebugConfig.LoadString(IDS_DEBUG_CONFIG));
	VERIFY(strReleaseConfig.LoadString(IDS_RELEASE_CONFIG));
 
	// Construct lists of debug/release configs
	ConfigurationRecord * pcr;
	CString strConfigName;
	const CPtrArray & ppcr = *GetConfigArray();
	int ccr = m_ConfigArray.GetSize();
	for (int icr = 0; icr < ccr; icr++)
	{
		pcr = (ConfigurationRecord *)ppcr[icr];
		strConfigName = pcr->GetConfigurationName();
		if (strConfigName.Find(strDebugConfig) > 0)
			lstDebugConfigs.AddTail(pcr);
		if (strConfigName.Find(strReleaseConfig) > 0)
			lstReleaseConfigs.AddTail(pcr);
	}

  	// Is there any data in this stream.
	ar >> ((long&) bDataExists);

	if (!bDataExists)
	{
		// No data, so we may need to fail the operation
		// so that we will init this data.
  	}
	else
	{
 		BOOL		bHasReleaseConfig = FALSE;
 		DWORD		iConfig, cConfigs;
		CPtrList *	plstpcr;

		// Load the state of the builder
//		if (!CActionSlob::LoadFromArchiveVCP(ar))
//	 		AfxThrowArchiveException (CArchiveException::badSchema);

		// Load the state of the Project.Settings option tree-control
		if (!COptionTreeCtl::LoadOptTreeStore(ar))
 	 		AfxThrowArchiveException (CArchiveException::badSchema);

 		ar >> cConfigs;

		for (iConfig=0 ; iConfig<cConfigs ; ++iConfig)
		{
			// Read the name of the configuration, create that
			// configuration, and make it active:
			ar >> str;

 			// Do we recognize the configuration name as one of our
			// standard configs, i.e. debug/release, if so then we
			// can retain the information
			if (str == strDebugConfig)
 				plstpcr = &lstDebugConfigs;
  			else if (str == strReleaseConfig)
 				plstpcr = &lstReleaseConfigs;
			else
				plstpcr = NULL;
 
			// Set the other properties for this configuration:
			ar >> str;
			SetStrPropConfigs(plstpcr, P_Proj_CmdLine, str);
			ar >> str;
			SetStrPropConfigs(plstpcr, P_Proj_RebuildOpt, str);
			ar >> str;
			SetStrPropConfigs(plstpcr, P_Proj_Targ, str);
			ar >> str;
			SetStrPropConfigs(plstpcr, P_Proj_BscName, str);
			ar >> str;
			SetStrPropConfigs(plstpcr, P_ProjClsWzdName, str);

 			// Skip over platform name, we cannot change the platform
			// for projects in internal makefiles.
			ar >> str;

			// Set debug page properties
  			ar >> str;
			SetStrPropConfigs(plstpcr, P_Caller, str);
			ar >> str;
			SetStrPropConfigs(plstpcr, P_WorkingDirectory, str);
			ar >>  str;
 			SetStrPropConfigs(plstpcr, P_Args, str);
			
			// Read the additional DLLs but we don't set them like
			// we used to
			ar >> str;
 			
			ar >> str;
			SetStrPropConfigs(plstpcr, P_RemoteTarget, str);
		}

		// Get the name of the current configuration:
		ar >> str;  // We don't use it!
 
		// Check the EOR marker.
		DWORD dwT;

		ar >> dwT;
		if (dwT != dwExtEOR)
			AfxThrowArchiveException (CArchiveException::badSchema);
	}

	return bRetval;
}

///////////////////////////////////////////////////////////////////////////////
// Load from our external project stream.
BOOL CProject::LoadExternalFromVCP(CStateSaver& stateSave, BOOL bIsExternal)
{
	BOOL bRetval = TRUE;
	BOOL bDataExists = FALSE;
	CString strDebugConfig, strReleaseConfig;
	CInitFile fileInit;
	ASSERT(stateSave.IsLoading());
	// Open file usually takes two strings in case we are writing to
	// the reg DB, but the project should never write to the reg DB.
	//
	if (!stateSave.OpenFile(fileInit, PROJ_MSF_PROJEXTINFO, NULL, NULL))
		return FALSE;

	CArchive ar ( &fileInit, CArchive::load | CArchive::bNoFlushOnDelete);
	CString str, str2;
	int fPromptForDlls;
	UINT nFormatVersion;
	// Have to set this to 0 because of a bug in CRuntimeClass::Load which
	// only loads a word:
	UINT wSchemaNum = 0l;

	const UINT nDolphinVersionNumber = 0x00020054;
	const UINT nDolphinReleaseVersion = 0x00020000;
	const UINT nDolphinLastCompatVCP = 0x00020051;

	VERIFY(strDebugConfig.LoadString(IDS_DEBUG_CONFIG));
	VERIFY(strReleaseConfig.LoadString(IDS_RELEASE_CONFIG));

	TRY
	{
		ar >> ((long&) nFormatVersion);

		// For the VCP files, the formats must match exactly.
		if (nFormatVersion < 40000)
		{
			if ((nFormatVersion != nDolphinReleaseVersion) && (nFormatVersion < nDolphinLastCompatVCP|| nFormatVersion > nDolphinVersionNumber))
				AfxThrowArchiveException (CArchiveException::badSchema);
			else
			{
				// Convert old vc 2.0 settings here, but only if we are loading 
				// an external project--if so then we can read the data properly.
				if (bIsExternal)
					ConvertVCPFile(ar);
			}
		}
		else if (nFormatVersion >= 50000)
		{
			// This is more or a feel good kind of thing.  This will probably never pass this test,
			// but it's a good sanity check
			AfxThrowArchiveException (CArchiveException::badSchema);
		}

		// Is there any data in this stream.
		ar >> ((long&) bDataExists);

		if (!bDataExists)
		{
			// No data, so we may need to fail the operation
			// so that we will init this data.
			m_hOPTData = NULL;
			bRetval = !bIsExternal;
 		}
		else
		{
			// We have data, but are we loading an external
			// project, if so then we can read it properly.
			if (bIsExternal)
			{
				// Load the state of the builder
				if (!CActionSlob::LoadFromArchiveVCP(ar))
	 				AfxThrowArchiveException (CArchiveException::badSchema);

				// Load the state of the Project.Settings option tree-control
				if (!COptionTreeCtl::LoadOptTreeStore(ar))
		 			AfxThrowArchiveException (CArchiveException::badSchema);

				BOOL	bHasReleaseConfig = FALSE;
				DWORD	iConfig, cConfigs;

				ar >> cConfigs;

				for (iConfig=0 ; iConfig<cConfigs ; ++iConfig)
				{
					// Read the name of the configuration, create that
					// configuration, and make it active:
					ar >> str;

					// Keep a count of whether we have
					// a config named release or not.
					bHasReleaseConfig = (bHasReleaseConfig || (str == strReleaseConfig));

					CreateConfig(str);
					SetStrProp(P_ProjActiveConfiguration, str);

					// Set the other properties for this configuration:
					ar >> str;
					SetStrProp(P_ExtOpts_CmdLine, str);
					ar >> str;
					SetStrProp(P_ExtOpts_RebuildOpt, str);
					ar >> str;
					SetStrProp(P_ExtOpts_Targ, str);
					ar >> str;
					SetStrProp(P_ExtOpts_BscName, str);
					
					// This property is now defunct
					// ar >> str;
					//SetStrProp(P_ExtOpts_ClsWzdName, str);

                    ar >> str2;

					ar >> str;
					SetStrProp(P_Caller, str);

		 			CPlatform *	pPlatform;
					// need to translate from "official name" to "UI description"
					// also, platform may not exist
 					if (g_prjcompmgr.LookupPlatformByOfficialName(str2, pPlatform))
					{
						SetStrProp(P_ExtOpts_Platform, *(pPlatform->GetUIDescription()));
						if (!pPlatform->IsSupported())
 							InformationBox(IDS_EXE_NOT_SUPPORTED, str);
					}
					else
					{
						// Display a message box stating that we don't support this
						// platform
 						InformationBox(IDS_EXE_NOT_SUPPORTED, str);

						CPlatform * pPlatform = new CPlatform(str2, unknown_platform);
						// Register dynamic platform so deleted on exit
						g_prjcompmgr.RegisterUnkProjObj(pPlatform);

						SetStrProp(P_ExtOpts_Platform, *pPlatform->GetUIDescription());
					}

					ar >> str;
					SetStrProp(P_WorkingDirectory, str);
					ar >> str;

					// If the args are already set then we don't
					// override them. This allows args from the cmd line to
					// override those in the vcp file
					CString strOldArgs;
					GetStrProp(P_Args, strOldArgs);
					if (strOldArgs.IsEmpty())
						SetStrProp(P_Args, str);

					ar >> (LONG&)fPromptForDlls;
					SetIntProp(P_PromptForDlls, fPromptForDlls);
					ar >> str;
					SetStrProp(P_RemoteTarget, str);

					// read in the Java properties

					// get the current platform
//					CPlatform* pPlatform = GetCurrentPlatform();
					ASSERT(pPlatform != NULL);
					if (pPlatform != NULL && pPlatform->GetUniqueId() == java)
					{
						int nValue;

						// class name
						ar >> str;
						SetStrProp(P_Java_ClassFileName, str);

						// debug using browser/stand-alone interpreter
						ar >> (LONG&)nValue;
						SetIntProp(P_Java_DebugUsing, nValue);

						// browser name
						ar >> str;
						SetStrProp(P_Java_Browser, str);

						// stand-alone interpreter name
						ar >> str;
						SetStrProp(P_Java_Standalone, str);
					
						// param source: html page or user
						ar >> (LONG&)nValue;
						SetIntProp(P_Java_ParamSource, nValue);

						// HTML page
						ar >> str;
						SetStrProp(P_Java_HTMLPage, str);
					
						// temp HTML page
						ar >> str;
						SetStrProp(P_Java_TempHTMLPage, str);
					
						// args in HTML form
						ar >> str;
						SetStrProp(P_Java_HTMLArgs, str);

						// command line args
						ar >> str;
						// allow command line args to override those in the VCP file
						CString strJavaOldArgs;
						GetStrProp(P_Java_Args, strJavaOldArgs);
						if (strJavaOldArgs.IsEmpty())
							SetStrProp(P_Java_Args, str);

						// stand-alone interpreter args
						ar >> str;
						SetStrProp(P_Java_StandaloneArgs, str);
						
						// debug stand-alone as applet or application
						ar >> (LONG&)nValue;
						SetIntProp(P_Java_DebugStandalone, nValue);
					}

				}

				// Did we find a release config, if not
				// and we are not an exe-project then we
				// had better create a release config.
				if (!m_bProjIsExe && !bHasReleaseConfig)
				{
					// Add the release config!

					CPath pathT;

					pathT = *GetFilePath();

					CreateConfig(strReleaseConfig);
					SetStrProp(P_ProjActiveConfiguration, strReleaseConfig);

					// Set the default properties for the release configuration
					str = "NMAKE /f ";
					pathT.ChangeExtension(".mak");
					str += pathT.GetFileName();
					SetStrProp(P_ExtOpts_CmdLine, str);

					str = "/a";
					SetStrProp(P_ExtOpts_RebuildOpt, str);

					pathT.ChangeExtension(".exe");
					str = pathT.GetFileName();
					SetStrProp(P_ExtOpts_Targ, str);
					SetStrProp(P_Caller, str);

					(void) GetPlatformDescriptionString(str, str2);
					SetStrProp(P_ExtOpts_Platform, str2);

					pathT.ChangeExtension(".bsc");
					str = pathT.GetFileName();
					SetStrProp(P_ExtOpts_BscName, str);

					pathT.ChangeExtension(".clw");
				
					// This property is now defunct.
					//str = pathT.GetFileName();
					//SetStrProp(P_ExtOpts_ClsWzdName, str);

					str = _T("");
					SetStrProp(P_WorkingDirectory, str);
					SetStrProp(P_Args, str);
					SetIntProp(P_PromptForDlls, 1);
					SetStrProp(P_RemoteTarget, str);
					SetCurrentConfigAsBase();
				}

				// Get the name of the current configuration:
				ar >> str;
				SetStrProp(P_ProjActiveConfiguration, str);

				// Check the EOR marker.
				DWORD dwT;

				ar >> dwT;
				if (dwT != dwExtEOR)
					AfxThrowArchiveException (CArchiveException::badSchema);
			}
			else
			{
				// We found data but we are not really
				// loading an external project so just
				// get a pointer to the external data
				// so we can write it back later.
				m_hOPTData = fileInit.GetInitData();
			}
		}

		ar.Close ();
		fileInit.Close();
	}
	CATCH_ALL(e)
	{
		#ifdef _DEBUG
		AfxDump(e);
		TRACE("\n");
		#endif
		fileInit.Abort();
		bRetval = FALSE;
	}
	END_CATCH_ALL

	return bRetval;
}

///////////////////////////////////////////////////////////////////////////////
// Load from out internal project stream
BOOL CProject::LoadInternalFromVCP(CStateSaver& stateSave, BOOL bIsExternal)
{
	BOOL bRetval = TRUE;
	BOOL bDataExists = FALSE;
	CInitFile fileInit;
	ASSERT(stateSave.IsLoading());
	// Open file usually takes two strings in case we are writing to
	// the reg DB, but the project should never write to the reg DB.
	//
	if (!stateSave.OpenFile(fileInit, PROJ_MSF_PROJINTINFO, NULL, NULL))
		return FALSE;

	CArchive ar ( &fileInit, CArchive::load | CArchive::bNoFlushOnDelete);
	CString str, str2, strFirstConfig, strCurConfig;
	UINT nFormatVersion;
	// Have to set this to 0 because of a bug in CRuntimeClass::Load which
	// only loads a word:
	UINT wSchemaNum = 0l;

	TRY
	{
		ar >> ((long&) nFormatVersion);

		if ((nFormatVersion < 40000) || (nFormatVersion >= 50000))
			AfxThrowArchiveException (CArchiveException::badSchema);

		// Do we have any data in this stream.
		ar >> ((long&) bDataExists);
		if (!bDataExists)
		{
			// No we don't have any data, so we
			// may have to fail the loading of the
			// vcp file in order to init the data
			m_hOPTData = NULL;
			bRetval = bIsExternal;
 		}
		else
		{
			// We have some data in the stream but
			// are we attempting to load an internal
			// project, if so then we load the data
			if (!bIsExternal)
			{
				// Load the state of the builder
				if (!CActionSlob::LoadFromArchiveVCP(ar))
	 				AfxThrowArchiveException (CArchiveException::badSchema);

				// Load the state of the Project.Settings option tree-control
				if (!COptionTreeCtl::LoadOptTreeStore(ar))
		 			AfxThrowArchiveException (CArchiveException::badSchema);

				ar >> strCurConfig;
				if (ConfigRecordFromConfigName ( strCurConfig ) == NULL )
				{
					CString strmes;
					GetStrProp  (P_ProjActiveConfiguration, strFirstConfig);
/* no longer meaningful
					MsgBox (  Information,
							  MsgText ( strmes,
										IDS_VCP_UNKNOWN_CONFIG,
										(const TCHAR *) strCurConfig,
										(const TCHAR *) strFirstConfig
									   )
							);
 */
					strmes.Empty ();
					strCurConfig = strFirstConfig;

				}

				// Make sure this stream is for us:
				ar >> str;
				GetMoniker (str2);
				// GetMonker will most likely return a .dsp file.  The problem is that we are looking
				// for a .mak file.  Solution: hack hack hack and change the extension.

				CPath cpTmp;
				cpTmp.Create (str2);
				cpTmp.ChangeExtension ("mak");
				str2 = cpTmp.GetFileName ();

				if (CompareMonikers(str, str2) != 0 ||
					CRuntimeClass::Load (ar, &wSchemaNum) != GetRuntimeClass() ||
					wSchemaNum != GetRuntimeClass()->m_wSchema
				   )
					AfxThrowArchiveException(CArchiveException::badSchema);

				if (!SerializeProps(ar))
					AfxThrowArchiveException(CArchiveException::generic);
			}
			else
			{
				// Not loading an internal project so
				// just get a pointer to the stream
				// data so we can later restore it.
				m_hOPTData = fileInit.GetInitData();
			}
		}

		ar.Close ();
		fileInit.Close();
	}
	CATCH_ALL(e)
	{
		#ifdef _DEBUG
		AfxDump(e);
		TRACE("\n");
		#endif
		fileInit.Abort();
		bRetval = FALSE;
	}
	END_CATCH_ALL

	//  If the current configuration read in from the make is different from what
	//  we got from the MSF file, we need to set it again with SetStrProp.  This is
	//  Because Serialize props doesn't call SetXXXProp as it reads the props in:
	if (bRetval &&
		!bIsExternal &&	// don't need to check this for external makefiles
		(GetActiveConfig()->GetConfigurationName () != strCurConfig ||
		 m_strProjActiveConfiguration != strCurConfig)
	   )
	{			   
		SetStrProp(P_ProjActiveConfiguration, strCurConfig);
	}

	return bRetval;
}

UINT CProject::GetGlyphIndex()
{
	if (IsExeProject() || (GetProjType() == NULL))
		return 0;
	else if (GetProjType()->GetPlatform()->GetUniqueId() == java)
		return 2;
#ifdef VB_MAKEFILES
	else if ( GetProjType()->GetUniqueTypeId() == CProjType::exttarget && m_bVB == TRUE )
		return 1;
#endif
	else if ( /* UNDONE: test for C++ project */ 0 )
		return 1;


	return 0;
}
///////////////////////////////////////////////////////////////////////////////
#ifdef PROJ_LOG
void LogProjectState (
					const CObList &rlist,
					CProjItem	*pItemStart,
					CProject *pProject
					)
{
#if 0
	POSITION pos;
	CProjType *pProjType;
	const CPtrList *pol;
	CString str, str1;
	CBuildTool * pSchTool;
	COptionHandler *popthdlr;


	if (pProject == pItemStart)
	{
	    CString strName;
	    pProject->GetName(strName);
		PBUILD_TRACE ("Begining build with project \"%s\"\n", (const TCHAR *)strName );
		ASSERT (!pProject->m_bProjIsExe);
	}
	else
	{
		pItemStart->GetStrProp ( P_ProjItemName, str );
		PBUILD_TRACE ("Begining build with project \"%s\", with item \"%s\"\n",
			(const TCHAR *) *pProject->GetFilePath(), (const TCHAR *) str );

	}
	pItemStart->GetStrProp (P_ProjOriginalType, str );
	pItemStart->GetStrProp (P_ProjOriginalType, str1 );
	PBUILD_TRACE ("Active configuration is %s (based on %s)\n\n",
		(const TCHAR *) str, (const TCHAR *) str1 );

	if (pProject == pItemStart)
	{
		PBUILD_TRACE ("Project's tools are:\n");
		VERIFY ( g_prjcompmgr.LookupProjTypeByName( str1, pProjType ));

		pol = pProjType->GetToolsList ();
		for ( pos = pol->GetHeadPosition (); pos != NULL; )
		{
			pSchTool = (CBuildTool *) pol->GetNext (pos);
			// if (!pSchTool->IsKindOf ( RUNTIME_CLASS ( CSchmoozeTool ))) continue;
			popthdlr = pSchTool->GetOptionHandler();
			str.Empty();
			if (popthdlr)
			{
				g_prjoptengine.SetOptionHandler (popthdlr);
				g_prjoptengine.SetPropertyBag (pProject);
				g_prjoptengine.GenerateString (str);
			}
			else str = " (No Flags) ";

			PBUILD_TRACE ("			\"%s\" with flags \"%s\"\n",
						  (const TCHAR *) *pSchTool->GetName(), (const TCHAR *) str);
		}
	}
	PBUILD_TRACE ("\n");
#endif
#if 0
	for (pos = rlist.GetHeadPosition (); pos != NULL; )
	{
		pItem = (CFileItem *) rlist.GetNext (pos);
		if (!pItem->IsKindOf ( RUNTIME_CLASS ( CFileItem ))) continue;
		PBUILD_TRACE ("File Item: \"%s\"\n",
			(const TCHAR *) *pItem->GetFilePath () );
		str = "";
		MakeQuotedString (pItem->GetActiveConfig()->GetTargetFilesArray (), str );
		PBUILD_TRACE ("       Targets: %s\n", (const TCHAR *) str);

		PBUILD_TRACE ("\n");
	}
#endif
}

#endif // PROJ_LOG

//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////
CTargIncl::CTargIncl()
{
	m_pHead = NULL;
	m_hBld = NO_BUILDER;
}
///////////////////////////////////////////
// destructor:
// remove all elements
///////////////////////////////////////////
CTargIncl::~CTargIncl()
{
	RemoveAll();
}

///////////////////////////////////////////
// remove all elements
///////////////////////////////////////////
void CTargIncl::RemoveAll()
{
	m_cs.Lock();
	CItem * pCur = m_pHead;
	while (pCur)
	{
		m_pHead = pCur->m_pNext;
		delete pCur;
		pCur = m_pHead;
	}
	m_pHead = NULL;
	m_cs.Unlock();
}

///////////////////////////////////////////
// lookup for a particular target
///////////////////////////////////////////
BOOL CTargIncl::Lookup (HBLDTARGET hTarget, CString & strIncls)
{
	BOOL bRet = FALSE;
	m_cs.Lock();
	CItem * pCur = m_pHead;
	while (pCur)
	{
		if (pCur->m_hTarget == hTarget)
		{
			strIncls = pCur->m_strIncls;
			bRet = TRUE;
			break;
		}
		pCur = pCur->m_pNext;
	}
	m_cs.Unlock();
	return bRet;
}

/////////////////////////////////////
// remove an element given a target
/////////////////////////////////////
BOOL CTargIncl::Remove (HBLDTARGET hTarget)
{
	BOOL bRet = FALSE;
	m_cs.Lock();
	CItem * pCur = m_pHead;
	CItem * pPrev = m_pHead;
	while (pCur)
	{
		// found the element
		// so must remove
		if (pCur->m_hTarget == hTarget)
		{
			// if it is in the head position
			// adjust head location,
			// otherwise connect prev with
			// the next element
			if (pCur == m_pHead)
				m_pHead = pCur->m_pNext;
			else
			{
				ASSERT (pPrev);
				pPrev->m_pNext = pCur->m_pNext;
			}
			bRet = TRUE;
			// removing the element
			delete pCur;
			break;
		}
		// still not found, so move
		// forward.
		pPrev = pCur;
		pCur = pCur->m_pNext;
	}
	m_cs.Unlock();
	return bRet;
}

////////////////////////////////////
// insert an element in the list
// replacing it if the hTarget is already
// in the list
////////////////////////////////////
void CTargIncl::SetAt (HBLDTARGET hTarget, CString & strIncls)
{
	BOOL bFound = FALSE;
	m_cs.Lock();
	CItem * pCur = m_pHead;
	// search if hTarget element is in the list
	while (pCur)
	{
		// if it is in the list, just
		// replace the string includes:
		if (pCur->m_hTarget == hTarget)
		{
			pCur->m_strIncls = strIncls;
			bFound = TRUE;
			break;
		}
		pCur = pCur->m_pNext;
	}
	// not in the list, so must create a new item.
	if (!bFound)
	{
		pCur = new CItem;
		pCur->m_hTarget = hTarget;
		pCur->m_strIncls = strIncls;
		pCur->m_pNext = m_pHead;
		m_pHead = pCur;
	}
	m_cs.Unlock();
}
////////////////////////////////////
// Initialize the list with hTarget and includes
////////////////////////////////////
void CTargIncl::Init (HBUILDER hBld)
{
	// clean up:
	CString strIncls;
	HBLDTARGET hTarget;
	CString strTargetName;
	CTargetItem * pTargetItem;
	CBuildTool * pTool;
	BOOL bFoundTool = FALSE;
	m_cs.Lock();
	RemoveAll();
	ASSERT(hBld);
	m_hBld = hBld;
	CProject * pProject = (CProject *)hBld;
	if (m_hBld)
	{
		ASSERT_VALID(pProject);
		pProject->InitTargetEnum();
		while (pProject->NextTargetEnum (strTargetName, pTargetItem))
		{
			hTarget = g_BldSysIFace.GetTarget(strTargetName, m_hBld);
			ASSERT (hTarget);
			CProjTempConfigChange projTempConfigChange(pProject);
			projTempConfigChange.ChangeConfig(strTargetName);
			ASSERT (pTargetItem);
			bFoundTool = FALSE;
			g_prjcompmgr.InitBuildToolEnum();
			while (g_prjcompmgr.NextBuildTool(pTool))
			{
				if (pTool->IsKindOf(RUNTIME_CLASS (CCCompilerTool)))
				{
					bFoundTool = TRUE;
					break;
				}
			}
			if (bFoundTool)
			{
				pTool->GetIncludePathFromOptions(pTargetItem, strIncls);
				SetAt (hTarget, strIncls);
				strIncls.Empty();
			}
			hTarget = NULL;
		}
	}
	m_cs.Unlock();

}
///////////////////////////////////////////////////////////////
// Initialize the list with hTarget and includes
////////////////////////////////////
void CTargIncl::AddTarget (HBLDTARGET hTarget)
{
	// clean up:
	CString strIncls;
	CString strTargetName;
	CTargetItem * pTargetItem;
	CBuildTool * pTool;
	BOOL bFoundTool = FALSE;
	m_cs.Lock();

	// Get the active project:
	ASSERT (hTarget);
	ASSERT (m_hBld);
	pTargetItem = g_BldSysIFace.CnvHTarget(NULL, hTarget);
	g_BldSysIFace.GetTargetName(hTarget, strTargetName, m_hBld);
	CProjTempConfigChange projTempConfigChange((CProject *)m_hBld);
	projTempConfigChange.ChangeConfig(strTargetName);
	ASSERT (pTargetItem);
	bFoundTool = FALSE;
	g_prjcompmgr.InitBuildToolEnum();
	while (g_prjcompmgr.NextBuildTool(pTool))
	{
		if (pTool->IsKindOf(RUNTIME_CLASS (CCCompilerTool)))
		{
			bFoundTool = TRUE;
			break;
		}
	}
	if (bFoundTool)
	{
		pTool->GetIncludePathFromOptions(pTargetItem, strIncls);
		SetAt (hTarget, strIncls);
	}
	m_cs.Unlock();

}
///////////////////////////////////////////////////////////////

static TCHAR szInterpreters[] = _T("Interpreters");
static TCHAR szBrowser[] = _T("Browser");
static TCHAR szStandalone[] = _T("Stand-alone interpreter");
static TCHAR szDefault[] = _T("<unknown>");

BOOL GetBrowserPath(CString& strBrowserPath)
{
	strBrowserPath.Empty();

	// get the name of the browser from the registry
	CString strBrowser = GetRegString(szInterpreters, szBrowser, szDefault);

	// is the browser name specified in the registry?
	if (strBrowser == szDefault)
	{
		// default to IE
		VERIFY(strBrowser.LoadString(IDS_JAVA_BROWSER_EXE));

		// fill in the registry
		WriteRegString(szInterpreters, szBrowser, strBrowser);
	}

	// do we have a full path the browser already, or do we need to search for it?
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	_tsplitpath(strBrowser, szDrive, szDir, NULL, NULL);
	if (_tcslen(szDrive) > 0 && _tcslen(szDir) > 0)
	{
		strBrowserPath = strBrowser;
	}
	else
	{
		// construct the path to the registry entry we want
		// HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\App Paths\iexplore.exe
		TCHAR szIERegPath[_MAX_PATH];
		_tcscpy(szIERegPath, REGSTR_PATH_APPPATHS);
		_tcscat(szIERegPath, _T("\\"));
		_tcscat(szIERegPath, strBrowser);

		// open the registry
		HKEY hkeyIE;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szIERegPath, 0, KEY_READ, &hkeyIE) == ERROR_SUCCESS)
		{

			// get the full path to IE
			DWORD dwType;
			TCHAR szIEFileName[_MAX_PATH];
			DWORD dwData = _MAX_PATH;
			if (RegQueryValueEx(hkeyIE, NULL, NULL, &dwType, (unsigned char*)szIEFileName, &dwData) == ERROR_SUCCESS)
			{

				BOOL bVersionOk = FALSE;

				// we only want to do a version check if the browser is iexplore.exe
				CString strBrowserT = strBrowser;
				strBrowserT.MakeLower();
				CString strBrowserExe;
				VERIFY(strBrowserExe.LoadString(IDS_JAVA_BROWSER_EXE));
				if (strBrowserT.Find(strBrowserExe) != -1)
				{

					// get the version number of this copy of IE

					// get the version info size
					DWORD dwHandle;
					DWORD dwVerInfoSize = GetFileVersionInfoSize(szIEFileName, &dwHandle);
					if (dwVerInfoSize != 0)
					{

						// get the version info
						void* pvVerInfo = new BYTE[dwVerInfoSize];
						if (GetFileVersionInfo(szIEFileName, dwHandle, dwVerInfoSize, pvVerInfo))
						{

							// get the VS_FIXEDFILEINFO from the version info
							VS_FIXEDFILEINFO* pFileInfo;
							UINT nLen;
							if (VerQueryValue(pvVerInfo, _T("\\"), (void**)&pFileInfo, &nLen))
							{

								UINT nMajorVersion = pFileInfo->dwFileVersionMS;
								UINT nMinorVersion = pFileInfo->dwFileVersionLS;

								// only use versions that support Java
								if (nMajorVersion > 0x040000 && nMinorVersion > 0x0425)
								{
									bVersionOk = TRUE;
								}
							}
						}

						delete [] pvVerInfo;
					}
				}
				else
				{
					bVersionOk = TRUE;
				}

				// is the version of the browser specified okay?
				if (bVersionOk)
				{

					// at this point we may have the Win95 shortened 8.3 path here.
					// so we need to expand the path to its long file name counterpart

					// there's no easy way to expand a path that contains short file
					// names to long file names

					// here, we'll expand each file name/directory using FindFirstFile
					// and munge them all together. At the end, we'll verify this
					// expanded path exists.

					// expand each directory in the path
					TCHAR szPath[_MAX_PATH];
					_tcscpy(szPath, szIEFileName);
					BOOL bDone = FALSE;
					while (!bDone)
					{
						// we're done if there's no file name part of the path left
						TCHAR szDrive[_MAX_DRIVE];
						TCHAR szDir[_MAX_PATH];
						TCHAR szFileName[_MAX_FNAME];
						_tsplitpath(szPath, szDrive, szDir, szFileName, NULL);
						if (_tcslen(szFileName) > 0 || _tcslen(szDir) > 0)
						{
							// do a FindFirstFile on each directory;
							// the cFileName part of the data returned is the expanded name
							WIN32_FIND_DATA findFileData;
							HANDLE hFile = FindFirstFile(szPath, &findFileData);
							if (hFile != INVALID_HANDLE_VALUE)
							{
								// add the expanded name to the path we're building
								if (strBrowserPath.IsEmpty())
								{
									strBrowserPath = CString(findFileData.cFileName);
								}
								else
								{
									strBrowserPath = CString(findFileData.cFileName) + _T("\\") + strBrowserPath;
								}

								// there should only be one file that matches the name we're looking for
								ASSERT(!FindNextFile(hFile, &findFileData));

								VERIFY(FindClose(hFile));

								// remove the ending file name from the path
								_tcscpy(szPath, szDrive);
								_tcscat(szPath, szDir);

								// remove any trailing backslash
								int nLastChar = _tcslen(szPath) - 1;
								if (!_ismbblead(szPath[nLastChar]) && szPath[nLastChar] == _T('\\'))
								{
									szPath[nLastChar] = _T('\0');
								}
							}
							else
							{
								bDone = TRUE;
							}
						}
						else
						{
							bDone = TRUE;
						}
					}

					// did we successfully expand the path?
					if (!strBrowserPath.IsEmpty())
					{
						// prepend the drive letter onto the path
						TCHAR szDrive[_MAX_DRIVE];
						_tsplitpath(szIEFileName, szDrive, NULL, NULL, NULL);
						strBrowserPath = CString(szDrive) + _T("\\") + strBrowserPath;

						// after all this path munging, make sure the name we now have exists on disk
						WIN32_FIND_DATA findFileData;
						HANDLE hFile = FindFirstFile(strBrowserPath, &findFileData);
						if (hFile == INVALID_HANDLE_VALUE)
						{
							// go back to using the original path
							strBrowserPath = szIEFileName;
						}
						else
						{
							VERIFY(FindClose(hFile));
						}
					}
				}

				RegCloseKey(hkeyIE);
			}
		}
		// no registry entry for the browser specified,
		// so we'll search the path for it
		else {

			// get the directory manager
			CDirMgr* pDirMgr = GetDirMgr();

			// get the executable path from the shell
			CString strPath;
			pDirMgr->GetDirListString(strPath, DIRLIST_PATH);

			// search the path for the browser
			TCHAR szBrowserPath[_MAX_PATH];
			if (SearchPath(strPath, strBrowser, NULL, _MAX_PATH, szBrowserPath, NULL) != 0)
			{
				strBrowserPath = szBrowserPath;
			}
		}
	}

	return !strBrowserPath.IsEmpty();
}

BOOL GetStandalonePath(CString& strStandalonePath)
{
	strStandalonePath.Empty();

	// get the name of the stand-alone interpreter from the registry
	CString strStandalone = GetRegString(szInterpreters, szStandalone, szDefault);

	// is the stand-alone interpreter name specified in the registry?
	if (strStandalone == szDefault)
	{
		// default to jview
		VERIFY(strStandalone.LoadString(IDS_JAVA_STANDALONE_EXE));

		// fill in the registry
		WriteRegString(szInterpreters, szStandalone, strStandalone);
	}

	// do we have a full path the stand-alone interpreter already, or do we need to search for it?
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	_tsplitpath(strStandalone, szDrive, szDir, NULL, NULL);
	if (_tcslen(szDrive) > 0 && _tcslen(szDir) > 0)
	{
		strStandalonePath = strStandalone;
	}
	else
	{
		// We want to search for the stand-alone interpreter in the Windows directory first.
		TCHAR szWinDir[_MAX_PATH];
		if (GetWindowsDirectory(szWinDir, _MAX_PATH))
		{
			TCHAR szStandalonePath[_MAX_PATH];
			if (SearchPath(szWinDir, strStandalone, NULL, _MAX_PATH, szStandalonePath, NULL) != 0)
			{
				strStandalonePath = CString(szStandalonePath);
			}
		}

		if (strStandalonePath.IsEmpty())
		{
			// get the directory manager
			CDirMgr* pDirMgr = GetDirMgr();

			// get the executable path from the shell
			CString strPath;
			pDirMgr->GetDirListString(strPath, DIRLIST_PATH);

			// search the path for our standalone interpreter
			TCHAR szStandalonePath[_MAX_PATH];
			if (SearchPath(strPath, strStandalone, NULL, _MAX_PATH, szStandalonePath, NULL) != 0)
			{
				strStandalonePath = CString(szStandalonePath);
			}
		}
	}

	return !strStandalonePath.IsEmpty();
}

///////////////////////////////////////////////////////////////

// THIS FILE IS WAY TOO HUGE, time to split it.

BEGIN_INTERFACE_MAP(CProjectInterfaces,CCmdTarget)
	INTERFACE_PART(CProjectInterfaces, IID_IBSProject, Project)
	INTERFACE_PART(CProjectInterfaces, IID_IBSProjectFileManager, ProjectFileManager)
	INTERFACE_PART(CProjectInterfaces, IID_IBSOptionManager, OptionManager)
	INTERFACE_PART(CProjectInterfaces, IID_IBSRemoteProject, RemoteProject)
	INTERFACE_PART(CProjectInterfaces, IID_IPkgProject, PkgProject)
END_INTERFACE_MAP()

CProjectInterfaces::CProjectInterfaces()
{
}

CProjectInterfaces::~CProjectInterfaces()
{
}


////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CProjectInterfaces::XProject::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CProjectInterfaces::XProject::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CProjectInterfaces::XProject::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CProjectInterfaces::XProject::SetDirtyState( BOOL b )
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	if( pThis->m_pProject ){
		if( b == TRUE ){
			pThis->m_pProject->DirtyProject();
		} else {
			pThis->m_pProject->CleanProject();
		}
	}
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::AddDependentProject( IBSProject* /*pProj*/ )
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	if(pThis->m_pProject ){
	}
	return E_UNEXPECTED;
}

STDMETHODIMP CProjectInterfaces::XProject::RemoveDependentProject( IBSProject* /*pProj*/ )
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	if(pThis->m_pProject ){
		return NOERROR;
	}
	return E_UNEXPECTED;
}

STDMETHODIMP CProjectInterfaces::XProject::EnumDependentProjects( IEnumBSProjects ** /*pEnum*/ )
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	if(pThis->m_pProject ){
		CEnumDependentProjects *pEnum = new CEnumDependentProjects();
		return NOERROR;
	}
	return E_UNEXPECTED;
}

STDMETHODIMP CProjectInterfaces::XProject::SetCurrentConfiguration(HCONFIGURATION /*hcfg*/ )
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	if(pThis->m_pProject ){
		return NOERROR;
	}
	return E_UNEXPECTED;
}

STDMETHODIMP CProjectInterfaces::XProject::AddConfiguration( HCONFIGURATION /*hcfg*/ )
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	if(pThis->m_pProject ){
		return NOERROR;
	}
	return E_UNEXPECTED;
}

STDMETHODIMP CProjectInterfaces::XProject::RemoveConfiguration(HCONFIGURATION /*hcfg*/)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::GetCProject( void **pProj )
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	*pProj = (void *)pThis->m_pProject;
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::EnumConfigurations(IEnumConfigurations **pIEnum)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	CProjItem *pProj = ((CProjItem *)pThis->GetProject())->GetTarget();
	CEnumConfigurations *pEnum = new CEnumConfigurations( pProj );
	*pIEnum = pEnum->GetInterface();
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::GetConfigurationName(HCONFIGURATION hcfg, LPOLESTR *pName)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);

	IMalloc *pIMalloc;
	int len;

	// get the configuration name fromn the handle
	ConfigurationHandle *pCH =(ConfigurationHandle *)hcfg;
	CString str = pCH->pcr->GetConfigurationName();

	// Alloc a buffer and copy string to it.
	CoGetMalloc(MEMCTX_TASK, &pIMalloc);
	len = str.GetLength();
	*pName = (LPOLESTR)pIMalloc->Alloc(len);
	// *pName = (LPOLESTR)str;
	pIMalloc->Release();

	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::GetConfigurationPlatform(HCONFIGURATION hcfg, IPlatform ** /*ppPlatform*/ )
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	ConfigurationHandle *pCH =(ConfigurationHandle *)hcfg;
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::Build( HCONFIGURATION hcfg, UINT /*type*/ )
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	ConfigurationHandle *pCH =(ConfigurationHandle *)hcfg;
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::AssignCustomBuildStep(HCONFIGURATION,LPBUILDFILE,LPCOLESTR)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::RemoveCustomBuildStep(HCONFIGURATION,LPBUILDFILE)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::SetToolSettings(HCONFIGURATION,LPBUILDFILE)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::GetCallingProgramName(HCONFIGURATION,LPOLESTR *)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::GetRunArguments(HCONFIGURATION,LPOLESTR *)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProject::SetRunArguments(HCONFIGURATION,LPOLESTR)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, Project);
	return NOERROR;
}

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CProjectInterfaces::XProjectFileManager::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CProjectInterfaces, ProjectFileManager);
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CProjectInterfaces::XProjectFileManager::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CProjectInterfaces, ProjectFileManager);
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CProjectInterfaces::XProjectFileManager::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CProjectInterfaces, ProjectFileManager);
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectInterfaces::XProjectFileManager::AddFile(LPCOLESTR pStr,LPBUILDFILE *)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, ProjectFileManager);
	if(pThis->m_pProject ){
		CPath path;
		path.Create((const char *)pStr);
		pThis->m_pProject->AddFile ( &path );
		// lookup the file
		// get the interface into the return argument.
		return NOERROR;
	}
	return E_UNEXPECTED;
}

STDMETHODIMP CProjectInterfaces::XProjectFileManager::GetFileSet(LPBUILDFILESET *)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, ProjectFileManager);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProjectFileManager::IsInProject(LPCOLESTR,LPBUILDFILE*)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, ProjectFileManager);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProjectFileManager::IsBuildableFile(LPBUILDFILE)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, ProjectFileManager);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XProjectFileManager::IsScanableFile(LPBUILDFILE)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, ProjectFileManager);
	return NOERROR;
}

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CProjectInterfaces::XRemoteProject::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CProjectInterfaces, RemoteProject);
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CProjectInterfaces::XRemoteProject::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CProjectInterfaces, RemoteProject);
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CProjectInterfaces::XRemoteProject::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CProjectInterfaces, RemoteProject);
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectInterfaces::XRemoteProject::GetRemoteTargetName(HCONFIGURATION, LPCOLESTR * /*pStr*/ )
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, RemoteProject);
	if(pThis->m_pProject ){
		CString str;
		pThis->m_pProject->GetRemoteTargetFileName(str);
		// pStr = (LPCOLESTR)str;
		return NOERROR;
	}
	return E_UNEXPECTED;
}

STDMETHODIMP CProjectInterfaces::XRemoteProject::SetRemoteTargetName(HCONFIGURATION, LPCOLESTR)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, RemoteProject);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XRemoteProject::GetInitialRemoteTarget(HCONFIGURATION, LPCOLESTR *)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, RemoteProject);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XRemoteProject::UpdateRemoteTarget(HCONFIGURATION)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, RemoteProject);
	return NOERROR;
}




////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CProjectInterfaces::XOptionManager::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CProjectInterfaces, OptionManager);
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CProjectInterfaces::XOptionManager::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CProjectInterfaces, OptionManager);
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CProjectInterfaces::XOptionManager::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CProjectInterfaces, OptionManager);
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IOptionManager methods

STDMETHODIMP CProjectInterfaces::XOptionManager::GetIntProp(UINT /*id*/, IBuildTool*,LPBUILDFILE,HCONFIGURATION,int *)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, OptionManager);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XOptionManager::GetBoolProp(UINT /*id*/, IBuildTool*,LPBUILDFILE,HCONFIGURATION,BOOL *)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, OptionManager);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XOptionManager::GetStrProp(UINT /*id*/, IBuildTool*,LPBUILDFILE,HCONFIGURATION,LPOLESTR *)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, OptionManager);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XOptionManager::SetIntProp(UINT /*id*/, IBuildTool*,LPBUILDFILE,HCONFIGURATION,int)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, OptionManager);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XOptionManager::SetBoolProp(UINT /*id*/, IBuildTool*,LPBUILDFILE,HCONFIGURATION,BOOL)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, OptionManager);
	return NOERROR;
}

STDMETHODIMP CProjectInterfaces::XOptionManager::SetStrProp(UINT /*id*/, IBuildTool*,LPBUILDFILE,HCONFIGURATION,LPOLESTR)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, OptionManager);
	return NOERROR;
}

BEGIN_INTERFACE_MAP(CEnumConfigurations, CCmdTarget)
	INTERFACE_PART(CEnumConfigurations, IID_IEnumConfigurations, EnumConfigurations)
END_INTERFACE_MAP()

BEGIN_INTERFACE_MAP(CEnumDependentProjects, CCmdTarget)
	INTERFACE_PART(CEnumDependentProjects, IID_IEnumBSProjects, EnumProjects)
END_INTERFACE_MAP()

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CEnumDependentProjects::XEnumProjects::AddRef(){
	METHOD_PROLOGUE_EX(CEnumDependentProjects, EnumProjects);
	return pThis->ExternalAddRef();
};

STDMETHODIMP_(ULONG) CEnumDependentProjects::XEnumProjects::Release(){
	METHOD_PROLOGUE_EX(CEnumDependentProjects, EnumProjects);
	return pThis->ExternalRelease();
};

STDMETHODIMP CEnumDependentProjects::XEnumProjects::QueryInterface(REFIID iid, LPVOID far * ppvObj){
	METHOD_PROLOGUE_EX(CEnumDependentProjects, EnumProjects);
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
};

////////////////////////////////////////////////////////////////////////////
// IEnumProject methods

/*
	CProjTempConfigChange projTempConfigChange(pProject);
	CObList ol;
	int fo = CProjItem::flt_OnlyTargRefs;
	pProject->FlattenSubtree(ol, fo);

 	for (POSITION pos = ol.GetHeadPosition(); pos != NULL; )
	{
		CProjectDependency * pProjectDep = (CProjectDependency *) ol.GetNext(pos);
		if (pProjectDep->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		{
			CTargetItem * pTarget = pProjectDep->GetTarget();
			CProject * pTarget = pTarget->GetProject();
 */

STDMETHODIMP CEnumDependentProjects::XEnumProjects::Next(THIS_ ULONG /*celt*/, IBSProject ** /*rgelt*/, ULONG * /*pceltFetched*/){
	METHOD_PROLOGUE_EX(CEnumDependentProjects, EnumProjects);
 	ULONG i = 0;
/*
	if( pThis->pList ){
		while ( (pThis->pos != (POSITION)NULL) && (i < celt) )
		{
			// this does an add ref on the Project
			rgelt[i] = NULL; // pProject->GetInterface();
			i++;
		}
		*pceltFetched = i;
		return NOERROR;
	}
*/
	return E_UNEXPECTED;
};

STDMETHODIMP CEnumDependentProjects::XEnumProjects::Skip(THIS_ ULONG /*celt*/){
	METHOD_PROLOGUE_EX(CEnumDependentProjects, EnumProjects);
/*
 	ULONG i = 0;
	if( pThis->pList ){
		while ( (pThis->pos != (POSITION)NULL) && (i < celt) )
		{
			pThis->pList->GetNext(pThis->pos);
			i++;
		}
		return NOERROR;
	}
*/
	return E_UNEXPECTED;
};

STDMETHODIMP CEnumDependentProjects::XEnumProjects::Reset(THIS){
	METHOD_PROLOGUE_EX(CEnumDependentProjects, EnumProjects);
/*
	if( pThis->pList ) {
		pThis->pos = pThis->pList->GetHeadPosition();
	}
*/
	return NOERROR;
};

STDMETHODIMP CEnumDependentProjects::XEnumProjects::Clone(THIS_ IEnumBSProjects ** /*ppenum*/){
	METHOD_PROLOGUE_EX(CEnumDependentProjects, EnumProjects);
	return E_NOTIMPL;
};

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CEnumConfigurations::XEnumConfigurations::AddRef(){
	METHOD_PROLOGUE_EX(CEnumConfigurations, EnumConfigurations);
	return pThis->ExternalAddRef();
};

STDMETHODIMP_(ULONG) CEnumConfigurations::XEnumConfigurations::Release(){
	METHOD_PROLOGUE_EX(CEnumConfigurations, EnumConfigurations);
	return pThis->ExternalRelease();
};

STDMETHODIMP CEnumConfigurations::XEnumConfigurations::QueryInterface(REFIID iid, LPVOID far * ppvObj){
	METHOD_PROLOGUE_EX(CEnumConfigurations, EnumConfigurations);
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
};

////////////////////////////////////////////////////////////////////////////
// IEnumConfiguration methods

STDMETHODIMP CEnumConfigurations::XEnumConfigurations::Next(THIS_ ULONG celt, HCONFIGURATION *rgelt, ULONG * /*pceltFetched*/){
	METHOD_PROLOGUE_EX(CEnumConfigurations, EnumConfigurations);
 	ULONG i = 0;
	while( (pThis->m_pos < pThis->m_len) && (i<celt) ){
		rgelt[i] = &(pThis->m_aConfigs[i]);
		pThis->m_pos++;
		i++;
	}
	return NOERROR;
};

STDMETHODIMP CEnumConfigurations::XEnumConfigurations::Skip(THIS_ ULONG celt){
	METHOD_PROLOGUE_EX(CEnumConfigurations, EnumConfigurations);

 	ULONG i = 0;
	while( (pThis->m_pos < pThis->m_len) && (i<celt) ){
		pThis->m_pos++;
		i++;
	}

	return NOERROR;
};

STDMETHODIMP CEnumConfigurations::XEnumConfigurations::Reset(THIS){
	METHOD_PROLOGUE_EX(CEnumConfigurations, EnumConfigurations);
	pThis->m_pos = 0;
	return NOERROR;
};

STDMETHODIMP CEnumConfigurations::XEnumConfigurations::Clone(THIS_ IEnumConfigurations ** /*ppenum*/ ){
	METHOD_PROLOGUE_EX(CEnumConfigurations, EnumConfigurations);
	return E_NOTIMPL;
};

////////////////////////////////////////////////////////////////////////////
// IPkgProject Interface

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CProjectInterfaces::XPkgProject::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject);
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CProjectInterfaces::XPkgProject::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject);
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CProjectInterfaces::XPkgProject::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject);
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IPkgProject methods

STDMETHODIMP CProjectInterfaces::XPkgProject::AddFiles(LPCOLESTR * pszFileNames, 
					ULONG count, LPCOLESTR szDestinationFolder)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	if(pThis->m_pProject)
	{
		USES_CONVERSION;
		HBUILDER hBld = (HBUILDER)pThis->m_pProject;
		HBLDTARGET hTarg = g_BldSysIFace.GetActiveTarget( hBld );
		HFILESET hFileSet = g_BldSysIFace.GetFileSet(hBld, hTarg);
		HFOLDER hFolder = NULL;   //  hFolder = szDestinationFolder
		ULONG i;


		if (szDestinationFolder != NULL)
			hFolder = (HFOLDER) pThis->m_pProject->FindGroup(W2T(szDestinationFolder));

		if (count > 1)
		{
//			theApp.NotifyPackages(PN_BEGIN_MULTIOP, NULL);
			CString str;
			str.LoadString(IDS_ADDING_FILES);
			::StatusBeginPercentDone(str);
		}

		pThis->m_pProject->BeginBatch();
		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
		pProjSysIFace->BeginUndo(IDS_UNDO_ADD_FILES);

		// inform the graphs that these targets are now being modified
		g_buildengine.SetGraphMode(Graph_Creating, pThis->m_pProject);


		CStringList strlistFiles;
		for (i = 0; i < count; i++)
		{
			CPath ppathFile;
			if (!ppathFile.Create(W2A(pszFileNames[i])))
			{
				ASSERT(0);
				strlistFiles.AddTail((LPCTSTR)pszFileNames[i]);
				continue;
			}

			HBLDFILE hBldFile = NO_FILE;
			if ((g_BldSysIFace.GetFile(&ppathFile, hBldFile, hFileSet, hBld, FALSE)) && (hBldFile != NO_FILE))
				strlistFiles.AddTail((LPCTSTR)ppathFile);
			else
				g_BldSysIFace.AddFile(hFileSet, &ppathFile, 0/*fSettings*/, hBld, hFolder);
			if (count > 1)
				::StatusPercentDone((int)(i * 100 / (int)count));
		}

		// inform user if any files could not be added to the project.
		if (!strlistFiles.IsEmpty())
		{
			CString strMsg;
			CString strFileList;
			int nFiles = 0;
			BOOL fDoneAdding = FALSE;
			while (!strlistFiles.IsEmpty())
			{
				CString strAdd = strlistFiles.RemoveHead();
				if (nFiles < 9)
				{
					strFileList += "\n";
					strFileList += strAdd;
					nFiles++;
				}
				else if (!fDoneAdding)
				{
					CString strMoreFiles;
					VERIFY(strMoreFiles.LoadString(IDS_MORE_FILES));
					strFileList += "\n";
					strFileList += strMoreFiles;
					fDoneAdding = TRUE;
				}
			}
			MsgBox(Information, MsgText(strMsg, IDS_FILE_ALREADY_IN_PROJECT, strFileList));
		}
		// inform the graphs that these targets are now stable
		g_buildengine.SetGraphMode(Graph_Stable, pThis->m_pProject);

		pProjSysIFace->EndUndo();
		pThis->m_pProject->EndBatch();

		if (count > 1)
		{
			::StatusPercentDone(100);	// show the user that insert is complete
			::StatusEndPercentDone();
//			theApp.NotifyPackages(PN_END_MULTIOP, NULL);
		}

		//
		// Added files may still be in dependency folder.
		//
		if (!pThis->m_pProject->IsExeProject())
		{
			pThis->m_pProject->RefreshAllMirroredDepCntrs();
		}

		return S_OK;
	}
	return E_UNEXPECTED;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::CanAddFile(LPCOLESTR szFileName, 
														  LPCOLESTR szDestinationFolder, BOOL bShowUI)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	if((pThis->m_pProject != NULL) && (!pThis->m_pProject->IsExeProject()))
	{
		USES_CONVERSION;
		HBUILDER hBld = (HBUILDER)pThis->m_pProject;
		CPath pathFile;
		if( pathFile.Create(W2A(szFileName)) ){
			BOOL bCanAdd = !g_BldSysIFace.IsProjectFile( &pathFile, FALSE, hBld );
			if( bCanAdd ){
				return S_OK;
			}
			if(bShowUI){
				MsgBox(Error, IDS_ERR_CANNOTADD_FILE);
			}
		}
	}
	return S_FALSE;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::DeleteFile(LPCOLESTR szFileName, 
														  LPCOLESTR szDestinationFolder)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	return E_NOTIMPL;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::RenameFile(LPCOLESTR szOldFileName, LPCOLESTR szSourceFolder,
					LPCOLESTR szNewFileName, LPCOLESTR szDestinationFolder)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	CString strOldName = szOldFileName;
	CString strNewName = szNewFileName;
	FileRegHandle frh = CFileRegFile::LookupFileHandleByName( strOldName );
	if (frh == NULL)
	{
		ASSERT(0);  // shouldn't get here
		return E_FAIL;
	}
	
	CObList * pItemList = g_FileRegistry.GetRegEntry(frh)->plstFileItems;
	const CPath * pOldPath = g_FileRegistry.GetRegEntry(frh)->GetFilePath();
	if ((pItemList == NULL) || (pItemList->IsEmpty()) || (pOldPath==NULL))
	{
		ASSERT(0);  // shouldn't get here
		frh->ReleaseFRHRef();
		return E_FAIL;
	}

	CPath newPath;
	if (!newPath.CreateFromDirAndFilename(pThis->m_pProject->GetWorkspaceDir(), strNewName))
	{
		CString str;
		MsgBox (Error, MsgText ( str, IDS_BAD_PATH, strNewName));
		frh->ReleaseFRHRef();
		return E_FAIL;
	}
	if (newPath==*pOldPath)
	{
		ASSERT(0);  // shouldn't get here
		frh->ReleaseFRHRef();
		return S_OK; // NOP
	}

	// warn if attempting to rename header files or resources that
	// they will have to manually update the corresponding source files
	CString strFilters;
	strFilters.LoadString(IDS_HEADER_FILES_FILTER);
	if (FileNameMatchesExtension(pOldPath, strFilters))
	{
		CString str;
		if (MsgBox (Question, MsgText ( str, IDS_RENAME_HEADER, (LPCTSTR)*pOldPath), MB_YESNO) != IDYES)
		{
			frh->ReleaseFRHRef();
			return E_FAIL;
		}
	}

	strFilters.LoadString(IDS_RESOURCE_FILES_FILTER);
	if (FileNameMatchesExtension(pOldPath, strFilters))
	{
		CString str;
		if (MsgBox (Question, MsgText ( str, IDS_RENAME_RESOURCE, (LPCTSTR)*pOldPath), MB_YESNO) != IDYES)
		{
			frh->ReleaseFRHRef();
			return E_FAIL;
		}

	}

#if 0	// now handled in PN_QUERY_RENAME_FILE
	if (FileNameMatchesExtension(pOldPath, "dsp;dsw;opt;clw"))
	{
		// This file cannot be renamed
		CString str;
		MsgBox (Error, MsgText ( str, IDS_RENAME_DENIED, (LPCTSTR)*pOldPath));
		frh->ReleaseFRHRef();
		return E_FAIL;
	}

	if ((FileNameMatchesExtension(&newPath, "dsp;dsw;opt;clw")) || (g_BldSysIFace.IsProjectFile(&newPath, FALSE, ACTIVE_BUILDER)))
	{
		// Can't be renamed to newPath
		CString str;
		frh->ReleaseFRHRef();
		return E_FAIL;
	}
#endif

	if (pOldPath->ExistsOnDisk() && newPath.ExistsOnDisk())
	{
		// This file cannot be renamed
		CString str;
		MsgBox (Error, MsgText ( str, IDS_RENAME_BADDEST, (LPCTSTR)*pOldPath, (LPCTSTR)newPath));
		frh->ReleaseFRHRef();
		return E_FAIL;
	}

	NOTIFYRENAME notifyRename;
	notifyRename.OldFileName = *pOldPath;
	notifyRename.NewFileName = newPath;

	// give all packages a chance to veto
	if (!theApp.NotifyPackages(PN_QUERY_RENAME_FILE, (void *)&notifyRename))
	{
		// use should already have been warned by now
		frh->ReleaseFRHRef();
		return E_FAIL;
	}

	if (pOldPath->ExistsOnDisk())
	{
		ASSERT (!newPath.ExistsOnDisk());  // tested above
		if (!::MoveFile(*pOldPath, newPath))
		{
			// This file cannot be renamed
			CString str;
			MsgBox (Error, MsgText ( str, IDS_RENAME_FAILED, (LPCTSTR)*pOldPath));
			frh->ReleaseFRHRef();
			return E_FAIL;
		}
	}
	
	// File has now been successfully renamed

#if 0	// now handled by PN_RENAME_FILE notification
	CObList OldItemList;
	OldItemList.AddTail(pItemList); // does a copy

	POSITION pos = OldItemList.GetHeadPosition();
	while (pos != NULL)
	{
		CFileItem * pItem = (CFileItem *)OldItemList.GetNext(pos);
		ASSERT(pItem);
		CTargetItem * pTarget = pItem->GetTarget();
		ASSERT(pTarget);

		// tool may change, so unassign actions for all configs
		const CPtrArray * pCfgArray = pTarget->GetConfigArray();
		int i, cCfgs = pCfgArray->GetSize();
		for (i=0; i < cCfgs; i++)
		{
			ConfigurationRecord *pcr = (ConfigurationRecord *)pCfgArray->GetAt(i);
			ASSERT_VALID(pcr);
			if (pcr->IsValid())
				CActionSlob::UnAssignActions(pItem, NULL, pcr, FALSE);
		}

		CFileRegistry * pRegistry = pTarget->GetRegistry();
		pRegistry->RemoveFromProject(pItem, FALSE);
		VERIFY(pItem->SetFile(&newPath, TRUE));
		pRegistry->RestoreToProject(pItem, FALSE);

		// reassign actions for all configs
		for (i=0; i < cCfgs; i++)
		{
			ConfigurationRecord *pcr = (ConfigurationRecord *)pCfgArray->GetAt(i);
			ASSERT_VALID(pcr);
			if (pcr->IsValid())
				CActionSlob::AssignActions(pItem, NULL, pcr, FALSE);
		}

		pItem->InformDependants (SN_FILE_NAME);
	}
#endif

	if (pThis->m_pProject->m_bProjectComplete)
	{
		theApp.NotifyPackages(PN_RENAME_FILE, (void *)&notifyRename);
	}

	frh->ReleaseFRHRef();
	return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::CreateFolder(LPCOLESTR szFolder)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	return E_NOTIMPL;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::OnActivate(BOOL bActivate)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)

	if(bActivate)
		g_BldSysIFace.SetActiveBuilder((HBUILDER)pThis->m_pProject);
	else
		g_BldSysIFace.SetActiveBuilder(NO_BUILDER);

	return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::GetAutomationObject(LPDISPATCH *ppAutomationProject)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject);

	*ppAutomationProject = g_BldSysIFace.GetAutoProject((HBUILDER)pThis->m_pProject);
	return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::Save()
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	return(g_BldSysIFace.SaveBuilder((HBUILDER)pThis->m_pProject) ? NOERROR : E_FAIL);
}

STDMETHODIMP CProjectInterfaces::XPkgProject::IsDirty()
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	return(g_BldSysIFace.GetDirtyState((HBUILDER)pThis->m_pProject) ? S_OK : S_FALSE);
}

STDMETHODIMP CProjectInterfaces::XPkgProject::SetDirty(BOOL bDirty)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	g_BldSysIFace.SetDirtyState((HBUILDER)pThis->m_pProject, bDirty);
	return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::Delete(DeletionCause dcCause, BOOL *pbSuccess)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)

	if (dcCause == WorkspaceClose)
		g_bInProjClose = TRUE;

	*pbSuccess = g_BldSysIFace.CloseBuilder((HBUILDER)pThis->m_pProject, dcCause );
	return S_OK;
}

// Makes pDependentProject a 'dependent' of this--i.e., this is a subproject of pDependentProject
STDMETHODIMP CProjectInterfaces::XPkgProject::SetDependentProject(IPkgProject *pDependentProject)
{
	HBUILDER hBld, hDepBld;
	HFILESET hFile, hDepFile;
	HBLDTARGET hTarg, hDepTarg;

	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject);

	if (pDependentProject == NULL)
		return E_FAIL;

	COleRef<IBSProject> pBSProj = NULL;
	VERIFY(SUCCEEDED(pDependentProject->QueryInterface(IID_IBSProject, (void **)pBSProj.InOut())));
	if(!pBSProj)
	{
		AfxMessageBox(IDS_ERR_NEWDEP, MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	pBSProj->GetCProject((void **)&hBld);
	hTarg = g_BldSysIFace.GetActiveTarget(hBld);
	hFile = g_BldSysIFace.GetFileSet(hBld, hTarg);
	hDepBld = (HBUILDER)pThis->m_pProject;
	hDepTarg = g_BldSysIFace.GetActiveTarget(hDepBld);
	hDepFile = g_BldSysIFace.GetFileSet(hDepBld, hDepTarg);
	if(g_BldSysIFace.AddProjectDependency(hFile, hDepFile, hBld, hDepBld)==NULL)
	{
		AfxMessageBox(IDS_ERR_NEWDEP, MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	::SetWorkspaceDocDirty();
	return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::GetClassID(CLSID *pClsID)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject);
	// REVIEW: make these all use true class ids instead of interfaces
	// REVIEW: check other packages' usage of CPackage::IsVisibleProjectCommand() command
	//			before changing what gets returned here...
	// *pClsID = BuildProjCLSID;
	
	*pClsID = IID_IBSProject;

	if (pThis->m_pProject->m_bProjIsExe)
		*pClsID = IID_IBSExeProject;
	else
	{
		CPlatform *pPlatform = pThis->m_pProject->GetCurrentPlatform ();
		if (pPlatform)
		{
			if (pPlatform->GetUniqueId ()== java)
				*pClsID = IID_IBSJavaProject; 
			else	// REVIEW: need way to determine C/C++ project as something other than default...
				*pClsID = IID_IBSCxxProject;
		}
	}

	return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::GetProjectInfo(LPCOLESTR *pszProjName, 
					LPCOLESTR *pszProjFullPath, DWORD *pdwPackageID, DWORD *pdwFlags)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	USES_CONVERSION;

	// Set the ID
	if (pdwPackageID)
		*pdwPackageID = PACKAGE_VPROJ;
	if (pdwFlags)
		*pdwFlags = 0;

	// Get the Project Name
	if(!pThis->m_pProject)
	{
		if (pszProjName)
			*pszProjName = NULL;
		if (pszProjFullPath)
			*pszProjFullPath = NULL;
		return E_FAIL;
	}

	ConfigurationRecord * pcr = pThis->m_pProject->GetActiveConfig();
	CString strProjectName;
	if(pcr)
		pcr->GetProjectName(strProjectName);

	LPOLESTR wstr = A2W( (const char *)strProjectName );
	if (pszProjName)
	{
		*pszProjName = (LPOLESTR)CoTaskMemAlloc((wcslen(wstr)+1) * sizeof(wchar_t));
		wcscpy((LPOLESTR)*pszProjName, wstr);
	}

	if (pszProjFullPath)
	{
		// Get the full path to the .bld
		const char *szProjPath = (const char *)*pThis->m_pProject->GetFilePath();
		wstr = A2W(szProjPath);
		*pszProjFullPath = (LPOLESTR)CoTaskMemAlloc((wcslen(wstr)+1) * sizeof(wchar_t));
		wcscpy((LPOLESTR)*pszProjFullPath, wstr);
	}
	
	if (pdwFlags)
	{
		if (pThis->m_pProject->m_bProjIsExe)
			*pdwFlags |= GPI_PROJISEXECUTABLE;
		else
		{
			*pdwFlags |= GPI_PROJSUPPORTSDEPS;
			*pdwFlags |= GPI_PROJSUPPORTSSCC;
			if (g_bAlwaysExportMakefile)
				*pdwFlags |= GPI_PROJEXPORTMAKEFILE;
			if (g_bAlwaysExportDeps)
				*pdwFlags |= GPI_PROJEXPORTDEPS;
		}
	}


	return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::GetProjectData(DWORD dwProp, CString & strVal)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)

	// set defaults for possible failure
	BOOL bSuccess = FALSE;
#if 0
	USES_CONVERSION;
	LPOLESTR wstr;
	if (pszVal == NULL)
		return E_INVALIDARG;
	*pszVal = NULL;
#endif

	switch (dwProp) {
		case GPD_PROJSCCNAME:
			bSuccess = (pThis->m_pProject->GetStrProp(P_ProjSccProjName, strVal) == valid);
			break;
		case GPD_PROJSCCPATH:
			bSuccess = (pThis->m_pProject->GetStrProp(P_ProjSccRelLocalPath, strVal) == valid);
			break;
		case GPD_PROJSCCAUX:
//			bSuccess = (pThis->m_pProject->GetStrProp(P_ProjSccProjAux, strVal) == valid);
//			break;
		default:
			return E_INVALIDARG;
	}

#if 0
	if (bSuccess)
	{
		wstr = A2W( (const char *)strVal );
		*pszVal = (LPOLESTR)CoTaskMemAlloc((wcslen(wstr)+1) * sizeof(wchar_t));
		wcscpy((LPOLESTR)*pszVal, wstr);
	}
#endif

	return bSuccess ? S_OK : E_FAIL;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::SetProjectData(DWORD dwProp, LPCSTR szVal)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	// USES_CONVERSION;

	CString strVal = szVal;
	BOOL bSuccess = FALSE;

	switch (dwProp) {
		case GPD_PROJSCCNAME:
			bSuccess = pThis->m_pProject->SetStrProp(P_ProjSccProjName, strVal);
			break;
		case GPD_PROJSCCPATH:
			bSuccess = pThis->m_pProject->SetStrProp(P_ProjSccRelLocalPath, strVal);
			break;
		case GPD_PROJSCCAUX:
//			bSuccess = pThis->m_pProject->SetStrProp(P_ProjSccProjAux, strVal);
//			break;
		default:
			return E_INVALIDARG;
	}

	return bSuccess ? S_OK : E_FAIL;
}


STDMETHODIMP CProjectInterfaces::XPkgProject::GetProjectTypeName(CString **ppszType)
{
    METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
        
    CProject  *pProject;
    CProjType *pProjType;
    
    pProject = pThis->m_pProject;
    pProjType = pProject->GetProjType();
    *ppszType = (CString *) pProjType->GetTypeName();
    return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::DrawProjectGlyph(HDC hDC, LPCRECT pRect)
{
	CImageWell imageWell;

	ASSERT(!imageWell.IsOpen());
	VERIFY(imageWell.Load(IDB_SINGLE_ITEM, CSize(16, 16)));
	VERIFY(imageWell.Open());
	VERIFY(imageWell.CalculateMask());

	int nGlyph = 4;
	CPoint pt(pRect->left, pRect->top);
	imageWell.DrawImage(CDC::FromHandle(hDC), pt, nGlyph);

	return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::GetDefaultFolder(LPCOLESTR *pszDefaultFolder)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	
	*pszDefaultFolder = NULL;
	return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::CanBrowseFolders(BOOL *pbCanBrowse)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject);

	*pbCanBrowse = FALSE;
	return S_OK;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::BrowseFolders(LPCOLESTR szStartingFolder,
							LPCOLESTR *pszFolderChosen)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	return E_NOTIMPL;
}

STDMETHODIMP CProjectInterfaces::XPkgProject::GetAllFolders(CALPOLESTR *pAllFolders)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	return E_NOTIMPL;
}


STDMETHODIMP CProjectInterfaces::XPkgProject::GetAssociatedFiles(CStringArray& saFiles, BOOL bSelected)
{
	METHOD_PROLOGUE_EX(CProjectInterfaces, PkgProject)
	ASSERT(bSelected==FALSE); // NYI
//	CProject * pProject g_pActiveProject;
//	if (pProject==NULL)
//		return 0;
	CProjectInterfaces* projInterfaces = pThis;
	COleRef<IBSProject> buildSystemProj;
	if (!SUCCEEDED(this->QueryInterface(IID_IBSProject,(void**) buildSystemProj.InOut()))) {
			return E_FAIL; //?
	}
	CProject* pProject;
	buildSystemProj->GetCProject((void**)&pProject);

	BOOL bIsSccInstalled = (g_pSccManager->IsSccInstalled() == S_OK);
	if (bIsSccInstalled && (g_pSccManager->IsAddingProject() == S_OK))
	{
		// we're in the process of putting the .mak file under scc
		// so make sure the deps have been scanned so we don't get
		// really strange behavior later
		UpdateBeforeWriteOutMakeFile();
	}

	CObList filelist;
	pProject->FlattenSubtree(filelist, CProjItem::flt_Normal | CProjItem::flt_ExcludeGroups | CProjItem::flt_ExcludeDependencies /* | CProjItem::flt_RespectTargetExclude */);
	CMapStringToPtr mapFiles;
	CString strKey;
	void * pVoid;
	const CPath * pPath;	
	LPCTSTR pszExt;
	CSlob * pItem;
	// iterate through list, adding each file-type item to the array 	
	POSITION pos = filelist.GetHeadPosition();
	int count = 0;
	while (pos != NULL)
	{
		pItem = (CSlob *)filelist.GetNext(pos);

		if (pItem->IsKindOf(RUNTIME_CLASS(CProjItem)))
		{
			pPath = ((CProjItem *)pItem)->GetFilePath();
			if ((pPath!=NULL)
				&& (_tcsicmp((pszExt = pPath->GetExtension()),_T(".pch"))!=0) // ignore .pch files
				&& (_tcsicmp(pszExt,_T(".mod"))!=0) // ignore .mod files for Fortran
				&& (_tcsicmp(pszExt,_T(".tlb"))!=0)) // ignore .tlb files
			{
				strKey = *pPath;
				strKey.MakeLower();
				if (!mapFiles.Lookup(strKey, pVoid)) // don't add duplicates
				{
					mapFiles.SetAt(strKey, NULL); // found a new one!
					saFiles.Add((LPCTSTR)*pPath);
					count++;

					// hack: may need to add resource.h as well
					if (_tcsicmp(pPath->GetExtension(), _T(".rc"))==0)
					{
						const char szResH[] = "resource.h"; // UNDONE: name could be different
						CDir dirRC;
						CPath pathResH;
						if ((dirRC.CreateFromPath(*pPath)) && (pathResH.CreateFromDirAndFilename(dirRC, szResH)))
						{
							if (pathResH.ExistsOnDisk())
							{
								strKey = pathResH;
								strKey.MakeLower();
								if (!mapFiles.Lookup(strKey, pVoid)) // don't add duplicates
								{
									mapFiles.SetAt(strKey, NULL); // found a new one!
									saFiles.Add((LPCTSTR)pathResH);
									count++;
								}
							}
							// now try Fortran variants
							pathResH.ChangeExtension(_T(".fd"));
							if (pathResH.ExistsOnDisk())
							{
								strKey = pathResH;
								strKey.MakeLower();
								if (!mapFiles.Lookup(strKey, pVoid)) // don't add duplicates
								{
									mapFiles.SetAt(strKey, NULL); // found a new one!
									saFiles.Add((LPCTSTR)pathResH);
									count++;
								}
							}
							pathResH.ChangeExtension(_T(".hm"));
							if (pathResH.ExistsOnDisk())
							{
								strKey = pathResH;
								strKey.MakeLower();
								if (!mapFiles.Lookup(strKey, pVoid)) // don't add duplicates
								{
									mapFiles.SetAt(strKey, NULL); // found a new one!
									saFiles.Add((LPCTSTR)pathResH);
									count++;
								}
							}

						}
					}
				}
			}
		}
	}
	mapFiles.RemoveAll();
	
	return S_OK;
}

CTempDisableUI::CTempDisableUI()
{
	m_bOldUI = g_pAutomationState->DisplayUI();
	g_pAutomationState->DisableUI();
}

CTempDisableUI::~CTempDisableUI()
{
	if( m_bOldUI ) {
		g_pAutomationState->EnableUI();
	}
}
