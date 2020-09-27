//
// Build System Package Interface
//
// [matthewt], [colint]

#include "stdafx.h"
#pragma hdrstop

#include "resource.h"
#include "package.h"
#include "dirmgr.h"
#include "dirsdlg.h"
#include "tchar.h"
#include "prjoptn.h"
#include "projcomp.h"
#include "optndlg.h"	// COptionTreeStore
#include "exttarg.h"	// External target type
#include "bldslob.h"
#include "bldnode.h"
#include "afxdllxx.h"	// our modified version of afxdllx.h
#include "targctrl.h"
#include "blddlg.h"
#include "awiface.h"
#include "oleref.h"
#include "vwslob.h"
#include "bldrfile.h"	// nVersionMajor

extern CProjComponentMgr g_prjcompmgr;

#include <prjapi.h>
#include <bldapi.h>
#include <bldapi_.h>

#pragma warning(disable:4103)
#include <initguid.h>
#include <resguid.h>
#include <prjguid.h>
#include <bldguid.h>
#include <bldguid_.h>
#include <srcguid.h>
#include <dbgpguid.h>
#include <utilguid.h>
#include <utilapi.h>
#include <ObjModel\bldauto.h>
#include <ObjModel\bldguid.h>
#pragma warning(default:4103)

#include "autocfg.h"
#include <ObjModel\blddefs.h>

// #include "icapexp.h" // UNDONE

extern CString g_strTarg;
static AFX_EXTENSION_MODULE extensionDLL = { NULL, NULL };

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Flag lets me test new project stuff by disabling the old:
// BOOL g_TestingNewProj = FALSE;

CPackage * g_pBuildPackage = NULL;
BOOL g_bEnableAllPlatforms = FALSE;
BOOL g_bHTMLLog = TRUE;
BOOL g_bSlowLinks = FALSE;
BOOL g_bUseEnvironment = FALSE;

BOOL g_fCreatingProject = FALSE;	// are we in the process of creating a project?

//
// NOTE: g_bNewUpdateDepModel must be TRUE now because some of code
// that would be enabled if FALSE has been #if 0 // Not Used.  Just search
// for "g_bNewUpdateDepModel" to find affected code.
//
const BOOL g_bNewUpdateDepModel = TRUE;
BOOL g_bWorkspaceInited = FALSE;
CString g_strDialogName="";

static CStringList g_strlCfgsToBuild;
static UINT g_nTOB = TOB_Build;
static BOOL g_bRecurse = TRUE; // override with /NORECURSE switch
static BOOL g_bCommandLineBuild = FALSE;
static BOOL g_bEnableUpdDeps = FALSE;
UINT g_nNextIncompatibleVersion = (nVersionMajor + 1) * 100;

extern void InitExternalDepFolders();
extern BOOL __cdecl DumpHeap();
extern HANDLE g_hStdIn;		// defined in projcomp.cpp
extern BOOL g_bConversionPrompted;	// defined in project.cpp
extern BOOL g_bConversionCanceled;	// defined in project.cpp

HINSTANCE GetResourceHandle()
{
	return extensionDLL.hModule;
}

BOOL bJavaSupported, bJavaOnce=FALSE;


	

bool
IsCrashDumpEnabled(
	)
/*++

Routine Description:

	Check if crashdump is enabled.  This value is cached.  Msdev will need
	to be restarted after the value is changed.

--*/
{

	static bool	fCheckedForDumpEnabled = false;
	static bool fCrashDumpEnabled = false;

	HKEY		hKey = NULL;
	CString		str;
	LONG		ret = 0;
	DWORD		type = 0;
	DWORD		dwT = 0;
	DWORD		size = 0;

	if (fCheckedForDumpEnabled) {
		return fCrashDumpEnabled;
	}

	str = GetRegistryKeyName ();
	str += _T("\\Debug");

	ret = RegOpenKeyEx (HKEY_CURRENT_USER,
						str,
						0,
						KEY_ALL_ACCESS,
						&hKey);

	if (ret == ERROR_SUCCESS) {

		size = sizeof (dwT);
		ret = RegQueryValueEx (hKey,
							   _T("CrashDumpEnabled"),
							   0,
							   &type,
							   (BYTE*) &dwT,
							   &size);

		if (ret == ERROR_SUCCESS && type == REG_DWORD && dwT == 1) {
			fCheckedForDumpEnabled = true;
			fCrashDumpEnabled = true;
		} else {
			fCheckedForDumpEnabled = true;
			fCrashDumpEnabled = false;
		}
	}

	if (hKey) {
		RegCloseKey (hKey);
		hKey = NULL;
	}

	return fCrashDumpEnabled;
}

// This figures out on the basis of the compiler version whether the current
// product is the learning edition (V5) or not.
BOOL IsLearningBox()
{
	// Assume Learning is false if any failure occurs.
	static BOOL bHaveChecked = FALSE;
	static BOOL bIsLearning = FALSE;

	if (bHaveChecked)
		return bIsLearning;

	bHaveChecked = TRUE;

	DWORD dw;
	CString strC2Dir;
	TCHAR szBuf [MAX_PATH];
	HKEY hKey;

	// strVCDir = "\\\\HKEYLOCALMACHINE\\microsoft\\devstudio\\products\\Microsoft Visual C++\\ProductDir"
	if ( RegOpenKeyEx (
		HKEY_LOCAL_MACHINE, _T ("Software\\Microsoft\\DevStudio\\6.0\\Products\\Microsoft Visual C++"),
		0, KEY_READ, &hKey) == ERROR_SUCCESS 
	   )
	{
		DWORD dwType, cbData = MAX_PATH;
		if (RegQueryValueEx (hKey, _T ("ProductDir"), NULL, &dwType, (LPBYTE) szBuf, &cbData) == ERROR_SUCCESS)
		{
			ASSERT (dwType == REG_SZ);
			szBuf [cbData] = 0;
			strC2Dir = szBuf;
			strC2Dir+="\\bin\\c2.dll";
		}
		RegCloseKey (hKey);
	}	

	TCHAR *szC2 = strC2Dir.GetBuffer(strC2Dir.GetLength());

	// Get version information from c2.exe.
	DWORD dwSize = GetFileVersionInfoSize( szC2, &dw );
	if (0!=dwSize)
	{
		LPVOID lpBuffer = new BYTE[dwSize];

		if (NULL==lpBuffer)
			return bIsLearning;

		if (GetFileVersionInfo( szC2, dw, dwSize, lpBuffer))
		{
			UINT uLen;
			VS_FIXEDFILEINFO* pVerInfo;
			if (0!=VerQueryValue( lpBuffer, _T("\\"), (LPVOID*)&pVerInfo, &uLen ))
			{
				// if the last digit in the version number is an 8 or a 9
				// then we have the learning edition
				UINT lower = pVerInfo->dwFileVersionMS & 0x0000FFFF;
				bIsLearning = ( lower % 10 ) >= 8 ;
			}
		}
		delete [] lpBuffer;
	}
	return bIsLearning;
}


////////////////////////////////////////////////////////////
// Names of registry entries

static TCHAR BASED_CODE szPathItems[]		= _T("Path Dirs");
static TCHAR BASED_CODE szIncItems[]		= _T("Include Dirs");
static TCHAR BASED_CODE szLibItems[]		= _T("Library Dirs");
static TCHAR BASED_CODE szDirectories[]		= _T("Directories");
static TCHAR BASED_CODE szSrcItems[]		= _T("Source Dirs");
static TCHAR BASED_CODE szClassItems[]		= _T("Class Dirs");
static TCHAR BASED_CODE szPath[] = "Path";
static TCHAR BASED_CODE szInclude[] = "Include";
static TCHAR BASED_CODE szLib[] = "Lib";
static TCHAR BASED_CODE szSrc[] = "Source";
static TCHAR BASED_CODE szClasspath[] = "Classpath";
static TCHAR BASED_CODE szInstallDirs[] = "Install Dirs";
static TCHAR BASED_CODE szBuildSystem[] = "Build System";
static TCHAR BASED_CODE sz40CompatMak[] = "Use v4.0 Compatible Makefile Format";
static TCHAR BASED_CODE szAlwaysExport[] = "Always export makefile";
static TCHAR BASED_CODE szAlwaysExportDeps[] = "Always export dependencies";
static TCHAR BASED_CODE szBuildLog[] = "Enable Build Logging";
static TCHAR BASED_CODE szSharability[] = "Makefile Sharability";
static TCHAR BASED_CODE szFullPlatforms[] = "Full Platforms";
static TCHAR BASED_CODE szHtmlLog[] = "HTML Logs";
static TCHAR BASED_CODE szSlowLinks[] = "Slow Links for Win95";
static TCHAR BASED_CODE szUseEnvironment[] = "Use Environment Paths";
static TCHAR BASED_CODE szNextIncompatibleVersion[] = "Next Incompatible Version";
static TCHAR BASED_CODE szEnableUpdateDeps[] = "Enable UpdateAllDependencies";

/////////////////////////////////////////////////////////////////////////////
//

// This is the small build toolbar. The large one has its id defined elsewhere, for historical reasons
#define IDTB_PROJECT_MINI IDTB_PACKAGE_BASE+1
void RefreshTargetCombos(BOOL bEmpty = FALSE);
void PropagateActiveConfig(void);

class CBldPackage : public CPackage
{
public:
	CBldPackage();
	
	enum { PKS = (PKS_IDLE | PKS_COMMANDS | PKS_NOTIFY | PKS_INTERFACES |
				  PKS_INIT | PKS_SERIALIZE /*| PKS_FILES */| PKS_AUTOMATION |
				  PKS_PROJECTINFO ) };

	virtual BOOL OnInit();
	virtual void OnExit();
	virtual BOOL OnIdle(long lCount);
	virtual BOOL OnNotify (UINT id, void *Hint = NULL, void *ReturnArea = NULL) ;
	virtual int GetAssociatedFiles(CStringArray & saFiles, BOOL bSelected = FALSE);
	virtual void SerializeWorkspaceSettings(CStateSaver& stateSave, DWORD dwFlags);
	virtual void SerializeWkspcConvSettings(CStateSaver& stateSave, DWORD dwFlags);
	virtual void SetProjectWorkspaceInfoText(LPCTSTR pProjName, LPCTSTR pInfo );
	virtual LPCTSTR GetProjectWorkspaceInfoText(LPCTSTR pProjName );

	virtual void AddOptDlgPages(class CTabbedDialog* pOptionsDialog, OPTDLG_TYPE type);

	// register 'global' build system components
	virtual BOOL DoBldSysCompRegister(CProjComponentMgr * pcompmgr, DWORD blc_type, DWORD blc_id);

// Dockable window interface
	virtual HGLOBAL GetDockInfo();
	virtual HWND GetDockableWindow(UINT nID, HGLOBAL hglob);
	virtual HGLOBAL GetToolbarData(UINT nID);

	virtual void GetDefProvidedNodes(CPtrList &DefNodeList, BOOL bWorkspaceInit);

// OLE Automation
	virtual LPDISPATCH GetPackageExtension(LPCTSTR szExtensionName);

	virtual HRESULT GetService(REFGUID guidService, REFIID riid, void **ppvObj);

	virtual BOOL IsVisibleProjectCommand(UINT nID, GUID *pGuids, UINT cGuidCount);
	
	// our project-level message routing
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnCmdMsg(UINT, int, void *, AFX_CMDHANDLERINFO *);


// Interface map
protected:
	BEGIN_INTERFACE_PART(BuildSystem, IBuildSystem)
		INIT_INTERFACE_PART(CBldPackage, BuildSystem)
		STDMETHOD(GetActiveBuilder)(HBUILDER *pHandle);
		STDMETHOD(IsActiveBuilderValid)();
		STDMETHOD(GetBuilderType)(HBUILDER hBld, int *pBuilderType);
		STDMETHOD(GetBuilderFile)(HBUILDER hBld, LPCSTR *ppszPath);
		STDMETHOD(CreateBuilder)(LPCSTR pchBldPath, BOOL fForce, BOOL fOpen, HBUILDER *pHandle);
		STDMETHOD(SetDirtyState)(HBUILDER hBld, BOOL fDirty);
		STDMETHOD(GetDirtyState)(HBUILDER hBld);
		STDMETHOD(SaveBuilder)(HBUILDER hBld);
		STDMETHOD(CloseBuilder)(HBUILDER hBld);
		STDMETHOD(OnOpenWorkspace)(LPCSTR pszWorkspaceName, BOOL bTemporary);
		STDMETHOD(OnDefaultWorkspace)(LPCSTR pszFileToAdd);
		STDMETHOD(OpenWorkspace)(LPCSTR pszWorkspace);
		STDMETHOD(GetFileSetFromTargetName)(LPCSTR pchFileSet, HBUILDER hBld, HFILESET *pHandle);
		STDMETHOD(GetTargetNameFromFileSet)(HFILESET hFileSet, CString &str, BOOL bInvalid, HBUILDER hBld);
		STDMETHOD(GetFileSetName)(HFILESET hFileSet, CString &str, HBUILDER hBld);
		STDMETHOD(GetTargetFromFileSet)(HFILESET hFileSet, HBLDTARGET &hTarg, BOOL bInvalid, HBUILDER hBld);
		STDMETHOD(GetActiveTarget)(HBUILDER hBld, HBLDTARGET *pHandle);
		STDMETHOD(GetTarget)(LPCSTR pchTarg, HBUILDER hBld, HBLDTARGET *pHandle);
		STDMETHOD(GetTargetName)(HBLDTARGET hTarg, CString &str, HBUILDER hBld);
		STDMETHOD(DeleteTarget)(HBLDTARGET hTarg, HBUILDER hBld);
		STDMETHOD(AddProjectDependency)(HFILESET hFileSet, HFILESET hDependantFileSet, HBUILDER hBld, HPROJDEP *pHandle);
		STDMETHOD(RemoveProjectDependency)(HPROJDEP hProjDep, HFILESET hFileSet, HBUILDER hBld);
		STDMETHOD(GetDependantProject)(HFILESET *pHandle, HPROJDEP hProjDep, HFILESET hFileSet, HBUILDER hBld);
		STDMETHOD(GetTargetDirectory)(HBLDTARGET hTarg, CString &strDir, HBUILDER hBld);
		STDMETHOD(GetTargetFileName)(HBUILDER hBld, LPTSTR *ppszPath);
		STDMETHOD(GetTargetAttributes)(HBUILDER hBld, int *piAttrs);
		STDMETHOD(MakeTargPropsDefault)(HBLDTARGET hTarg, HBUILDER hBld);
		STDMETHOD(CheckFile)(HFILESET hFileSet, CPath &pathFile, HBUILDER hBld);
		STDMETHOD(AddFolder)(HFOLDER *pHandle, HFILESET hFileSet, const TCHAR *pszFolder, HBUILDER hBld, HFOLDER hFolder, const TCHAR * pszFilter);
		STDMETHOD(AddFile)(HBLDFILE *pHandle, HFILESET hFileSet, const CPath *ppathFile, 
			int fSettings, HBUILDER hBld, HFOLDER hFolder);
		STDMETHOD(AddDependencyFile)(HFILESET hFileSet, const CPath *ppathFile, HBUILDER hBld, HFOLDER hFolder);
		STDMETHOD(DelFile)(HBLDFILE hFile, HFILESET hFileSet, HBUILDER hBld);
 		STDMETHOD(GetFile)(const CPath *ppathFile, HBLDFILE &hFile, HFILESET hFileSet, 
			HBUILDER hBld, BOOL bSimple);
		STDMETHOD(SetFileProp)(HBLDTARGET hTarg, HBLDFILE hFile, UINT idProp, int i);
		STDMETHOD(SetFileProp)(HBLDTARGET hTarg, HBLDFILE hFile, UINT idProp, CString &str);
		STDMETHOD(MakeFilePropsDefault)(HBLDTARGET hTarg, HBLDFILE hFile);
		STDMETHOD(TargetState)(UINT *puState, CObList *plstPath, 
			LPCSTR pchTarget, HBLDTARGET hTarg, HBUILDER hBld);
		STDMETHOD(FileState)(UINT *puState, const CPath *pathFile);
		STDMETHOD(ResolveIncludeDirectives)(const CPath &pathSrc, const CStringList &strlstIncs, 
			CStringList &lstPath, BOOL fIgnStdIncs, const CString &strPlat);
		STDMETHOD(ResolveIncludeDirectives)(const CDir &dirBase, const CStringList &strlstIncs,
			CStringList &lstPath, BOOL fIgnStdIncs, const CString &strPlat);
		STDMETHOD(ResolveIncludeDirectives)(const CObList &lstpathSrc, const CStringList &strlstIncs,
			CStringList &lstPath, HBLDTARGET hTarget);
		STDMETHOD(GetIncludePath)(CObList &lstIncDirs, const CString &strPlat, const CPath *ppathSrc, const CString &strTool, HBUILDER hBld );
		STDMETHOD(UpdateDependencyList)(const CPath &pathSrc, const CStringList &strlstIncs,
			HBLDTARGET hTarg, HBUILDER hBld);
		STDMETHOD(InitBuilderEnum)();
		STDMETHOD(GetNextBuilder)(CString & strTarget, BOOL bOnlyLoaded, HBUILDER *pHandle);
		STDMETHOD(InitTargetEnum)(HBUILDER hBld);
		STDMETHOD(GetNextTarget)(HBUILDER hBld, CString & strTarget, HBLDTARGET *pHandle);
		STDMETHOD(InitProjectDepEnum)(HBUILDER hBld, HFILESET hFileSet);
		STDMETHOD(GetNextProjectDep)(HBUILDER hBld, HFILESET hFileSet, HPROJDEP *pHandle);
		STDMETHOD(InitFileSetEnum)(HBUILDER hBld);
		STDMETHOD(GetNextFileSet)(HBUILDER hBld, HFILESET *pHandle);
		STDMETHOD(InitFileEnum)(HFILESET hFileSet, UINT filter);
		STDMETHOD(GetNextFile)(HFILESET hFileSet, FileRegHandle & frh, HBLDFILE *pHandle);
		STDMETHOD(GetFileSetTargets)(HFILESET hFileSet, CStringList &strlstTargs);
		STDMETHOD(GetFileSet)(HBUILDER hBld, HBLDTARGET hTarg, HFILESET *pHandle);
		STDMETHOD(GetBuildFile)(UINT idFile, TCHAR *szFileName, int cchFileBuffer,
			HBLDTARGET hTarg, HBUILDER hBld);
		STDMETHOD(RegisterSection)(CBldrSection * pbldsect);
		STDMETHOD(DeregisterSection)(const CBldrSection * pbldsect);
		STDMETHOD(InitSectionEnum)();
		STDMETHOD(GetNextSection)(CBldrSection **ppSection, CString & strSection);
		STDMETHOD(FindSection)(CBldrSection **ppSection, LPCSTR pchName);
		STDMETHOD(GetResourceFileList)(CPtrList &listRcPath, BOOL fOnlyBuildable, 
			HFILESET hFileSet, HBUILDER hBld);
		STDMETHOD(GetResourceIncludePath)(CPath *pResFile, CString &strIncludes); 
		STDMETHOD(LanguageSupported)(UINT idLang);
		STDMETHOD(GetSelectedFile)(CPath *path);
		STDMETHOD(GetProjectDir)(HBLDTARGET hTarget, HBUILDER hBld, CString& str);
		STDMETHOD(GetWorkspaceDir)(HBUILDER hBld, CString& str);
		STDMETHOD(BuildSystemEnabled)();
		STDMETHOD(IsBuildInProgress)();
		STDMETHOD(GetBuilderToolset)(int *piToolset, HBUILDER hBuilder);
		STDMETHOD(GetRegistryEntryPath)(FileRegHandle frh, const CPath **ppPath);
		STDMETHOD(GetRemoteTargetFileName)(HBUILDER hBld, CString &str);
		STDMETHOD(SetRemoteTargetFileName)(HBUILDER hBld, CString &str);
		STDMETHOD(GetSlobWnd)(HBUILDER hBld, CWnd **ppWnd);
		STDMETHOD(IsProjectFile)(const CPath *pPath, BOOL bIncludeDeps, HBUILDER hBld);
		STDMETHOD(UpdateBrowserDatabase)(HBUILDER hBld);
		STDMETHOD(GetBrowserDatabaseName)(HBUILDER hBld, const CPath **ppPath);
		STDMETHOD(GetCallingProgramName)(HBUILDER hBld, CString &str);
		STDMETHOD(GetProgramRunArguments)(HBUILDER hBld, CString &str);
		STDMETHOD(SetProgramRunArguments)(HBUILDER hBld, CString &str);
		STDMETHOD(GetWorkingDirectory)(HBUILDER hBld, CString &str);
		STDMETHOD(TargetIsCaller)(HBUILDER hBld);
		STDMETHOD(GetPromptForDlls)(HBUILDER hBld);
		STDMETHOD(SetPromptForDlls)(HBUILDER hBld, BOOL bPrompt);
		STDMETHOD(SetProjectState)(HBUILDER hBld);
		STDMETHOD(GetClassWizAddedFiles)(HBUILDER hBld);
		STDMETHOD(SetClassWizAddedFiles)(HBUILDER hBld, BOOL bPrompt);
		STDMETHOD(ScanAllDependencies)(HBUILDER hBld);
		STDMETHOD(GetVersionInfo)(HBUILDER hBld, void *ppvi);
		STDMETHOD(GetInitialExeForDebug)(HBUILDER hBld, BOOL bExecute);
		STDMETHOD(GetInitialRemoteTarget)(HBUILDER hBld, BOOL bBuild, BOOL fAlways);
		STDMETHOD(UpdateRemoteTarget)(HBUILDER hBld);
		STDMETHOD(DoTopLevelBuild)(HBUILDER hBld, BOOL bBuildAll, CStringList *pConfigs,
			FlagsChangedAction fca, BOOL bVerbose, BOOL bClearOutputWindow );
		STDMETHOD(GetEnvironmentVariables)(HBUILDER hBld, void *pEnvList);
		STDMETHOD(IsProfileEnabled)(HBUILDER hBld);
		STDMETHOD(IsScanableFile)(HBLDFILE hFile);
		STDMETHOD(GetSccProjName)(HBUILDER, CString&);
		STDMETHOD(SetSccProjName)(HBUILDER, const CString&);
		STDMETHOD(GetSccRelLocalPath)(HBUILDER, CString&);
		STDMETHOD(SetSccRelLocalPath)(HBUILDER, const CString&);
//		STDMETHOD(GetSccAuxPath)(HBUILDER, CString&);
//		STDMETHOD(SetSccAuxPath)(HBUILDER, const CString&);
		STDMETHOD(GetBuilder)(HBLDTARGET hTarg, HBUILDER *pHandle);
		STDMETHOD(GetBuilderFromFileSet)(HFILESET hFileSet, HBUILDER *pHandle);
		// additions
		STDMETHOD(EnumBuildTools)( LPENUMBUILDTOOLS *, LPBUILDTOOL * );
		STDMETHOD(EnumProjectTypes)( LPENUMPROJECTTYPES *, LPPROJECTTYPE * );
		STDMETHOD(EnumPlatforms)( LPENUMPLATFORMS *, LPPLATFORM * );
		STDMETHOD(GetCommandLineOptions)( HFILESET hFileSet, HBLDFILE hFile, CString & );
		// Java support
		STDMETHOD(GetJavaClassName)(HBUILDER hBld, CString& str);
		STDMETHOD(GetJavaClassFileName)(HBUILDER hBld, CString& str);
		STDMETHOD(GetJavaDebugUsing)(HBUILDER hBld, ULONG* pDebugUsing);
		STDMETHOD(GetJavaBrowser)(HBUILDER hBld, CString& str);
		STDMETHOD(GetJavaStandalone)(HBUILDER hBld, CString& str);
		STDMETHOD(GetJavaStandaloneArgs)(HBUILDER hBld, CString& str);
		STDMETHOD(GetJavaHTMLPage)(HBUILDER hBld, CString& str);
		STDMETHOD(GetJavaClassPath)(HBUILDER hBld, CString& str);
		STDMETHOD(GetJavaStandaloneDebug)(HBUILDER hBld, ULONG* pStandaloneDebug);
		STDMETHOD(GetInitialJavaInfoForDebug)(HBUILDER hBld, BOOL bExecute);
		// more additions
		STDMETHOD(GetPreCompiledHeaderName)(HBUILDER hBld, CString &strPch);
		STDMETHOD(GetPkgProject)(HBUILDER builder, IPkgProject** outPkgProject);
		STDMETHOD(GetProjectGlyph)(HBUILDER hBld, HFILESET hFileSet, CImageWell& imageWell, UINT * pnImage);
		STDMETHOD(GetBuilderFromName)(LPCTSTR pszBuilder, HBUILDER *pHandle);
		STDMETHOD(GetNameFromBuilder)(HBUILDER hBld, CString& szBuilder);
		STDMETHOD(SetCallingProgramName)(HBUILDER hBld, CString &str);
		STDMETHOD(DoSpawn)(CStringList& lstCmd,	LPCTSTR szDirInit,BOOL fClear, BOOL fASync, DWORD *pcErr, DWORD *pcWarn);
	END_INTERFACE_PART(BuildSystem)

	BEGIN_INTERFACE_PART(BuildWizard, IBuildWizard)
		INIT_INTERFACE_PART(CBldPackage, BuildWizard)
		STDMETHOD(SetActiveTarget)(HBLDTARGET hTarg, HBUILDER hBld);
		STDMETHOD(SetDefaultTarget)(HBLDTARGET hTarg, HBUILDER hBld);
		STDMETHOD(GetTargetTypeNames)(const CStringList **ppList, LPCSTR pchPlat, BOOL fUIDesc);
		STDMETHOD(AssignCustomBuildStep)(LPCSTR pchCommand, LPCSTR pchOutput, 
			LPCSTR pchDescription, HBLDTARGET hTarg, HBLDFILE hFile, HBUILDER hBld);
		STDMETHOD(UnassignCustomBuildStep)(HBLDTARGET hTarg, HBLDFILE hFile, HBUILDER hBld);
		STDMETHOD(SetToolSettings)(HBLDTARGET hTarg, LPCSTR pchSettings, LPCSTR pchTool, 
			BOOL fAdd, BOOL fClear, HBUILDER hBld);
		STDMETHOD(SetToolSettings)(HBLDTARGET hTarg, HBLDFILE hFile, LPCSTR pchSettings, 
			LPCSTR pchTool, BOOL fAdd, BOOL fClear);
		STDMETHOD(AddTarget)(HBLDTARGET *pHandle, LPCSTR pchTarg, LPCSTR pchPlat, LPCSTR pchType, 
			BOOL fUIDesc, BOOL fDebug, TrgCreateOp trgop, HBLDTARGET hOtherTarg, SettingOp setop, 
			HBLDTARGET hSettingsTarg, BOOL fQuiet, BOOL fOutDir, BOOL fUseMFC, HBUILDER hBld);
		STDMETHOD(AddDefaultTargets)(LPCSTR pchPlat, LPCSTR pchType, 
			HBLDTARGET &hDebugTarg, HBLDTARGET &hReleaseTarg, BOOL fUIDesc, BOOL fOutDir, 
			BOOL fUseMFC, HBLDTARGET hMirrorTarg, HBUILDER hBld, LPCSTR pchTarg);
		STDMETHOD(GetTargetOutDir)(HBLDTARGET hTarg, CString &strOut, HBUILDER hBld);
		STDMETHOD(SetTargetOutDir)(HBLDTARGET hTarg, CString &strOut, HBUILDER hBld);
		STDMETHOD(SetTargetIntDir)(HBLDTARGET hTarg, CString &strInt, HBUILDER hBld);
		STDMETHOD(SetTargetIsAppWiz)(HBLDTARGET hTarg, int iIsAppWiz, HBUILDER hBld);
		STDMETHOD(SetTargetUseOfMFC)(HBLDTARGET hTarg, int iUseOfMFC, HBUILDER hBld);
		STDMETHOD(SetTargetDefExt)(HBLDTARGET hTarg, CString &strExt, HBUILDER hBld);
		STDMETHOD(SetBuildExclude)(HBLDTARGET hTarg, HBLDFILE hFile, BOOL fExclude);
		STDMETHOD(AddFolder)(HFOLDER *pHandle, HFILESET hFileSet, const TCHAR *pszFolder, HBUILDER hBld, HFOLDER hFolder, const TCHAR * pszFilter);
		STDMETHOD(AddFile)(HBLDFILE *pHandle, HFILESET hFileSet, const CPath *ppathFile, 
			int fSettings, HBUILDER hBld, HFOLDER hFolder);
		STDMETHOD(AddMultiFiles)(HFILESET hFileSet, const CPtrList *plstpathFiles, 
			CPtrList *plstHBldFiles, int fSettings, HBUILDER hBld, HFOLDER hFolder);
		STDMETHOD(AddDependencyFile)(HFILESET hFileSet, const CPath *ppathFile, HBUILDER hBld, HFOLDER hFolder);
		STDMETHOD(GetFileSet)(HBUILDER hBld, HBLDTARGET hTarg, HFILESET *pHandle);
		STDMETHOD(GetAutoProject)(HBUILDER hBld, IBuildProject** ppProject);
		STDMETHOD(GetTargetUseOfMFC)(HBLDTARGET hTarg, int *iUseOfMFC, HBUILDER hBld);
		STDMETHOD(SetUserDefinedDependencies)(LPCSTR pchUserDeps, HBLDTARGET hTarg, HBLDFILE hFile, HBUILDER hBld);
		STDMETHOD(GetTargetIntDir)(HBLDTARGET hTarg, CString &strInt, HBUILDER hBld);
	END_INTERFACE_PART(BuildWizard)

	BEGIN_INTERFACE_PART(BuildPlatforms, IBuildPlatforms)
		INIT_INTERFACE_PART(CBldPackage, BuildPlatforms)
		STDMETHOD(GetAvailablePlatforms)(const CStringList **ppList, LPCSTR pchTarg, BOOL fUIDesc);
		STDMETHOD(GetSupportedPlatformCount)(int *piCount);
		STDMETHOD(GetPrimaryPlatform)(uniq_platform *pupID);
		STDMETHOD(GetCurrentPlatform)(HBUILDER hBld, uniq_platform *pupID);
		STDMETHOD(IsPlatformSupported)(uniq_platform upID);
		STDMETHOD(InitPlatformEnum)();
		STDMETHOD(NextPlatform)(uniq_platform *pupID);
		STDMETHOD(GetPlatformUIDescription)(uniq_platform upID, LPCSTR *ppszDesc);
		STDMETHOD(GetPlatformSHName)(uniq_platform upID, LPCSTR *ppszName);
		STDMETHOD(GetPlatformEMName)(uniq_platform upID, LPCSTR *ppszName);
		STDMETHOD(GetPlatformEEName)(uniq_platform upID, LPCSTR *ppszName);
		STDMETHOD(GetPlatformLocalTLIndex)(uniq_platform upID, UINT *puTLIndex);
		STDMETHOD(GetPlatformCurrentTLIndex)(uniq_platform upID, UINT *puTLIndex);
		STDMETHOD(SetPlatformCurrentTLIndex)(uniq_platform upID, UINT uTLIndex);
		STDMETHOD(GetPlatformCompRegKeyName)(uniq_platform upID, BOOL fInclAppKey, LPCSTR *ppszName);
		STDMETHOD(GetPlatformNMCount)(uniq_platform upID, int *piNMCount);
		STDMETHOD(GetPlatformNMName)(uniq_platform upID, UINT uNMIndex, LPCSTR *ppszName);
		STDMETHOD(GetPlatformTLCount)(uniq_platform upID, int *piTLCount);
		STDMETHOD(GetPlatformTLName)(uniq_platform upID, UINT uTLIndex, LPCSTR *ppszName);
		STDMETHOD(GetPlatformTLDescription)(uniq_platform upID, UINT uTLIndex, LPCSTR *ppszDesc);
		STDMETHOD(GetPlatformTLFlags)(uniq_platform upID, UINT uTLIndex, UINT *puFlags);
		STDMETHOD(GetPlatformBuildable)(uniq_platform upID, BOOL *pfBuildable);
	END_INTERFACE_PART(BuildPlatforms)

	BEGIN_INTERFACE_PART(BuildComponents, IBuildComponents)
		INIT_INTERFACE_PART(CBldPackage, BuildComponents)
		STDMETHOD(GetBldSysCompName)(DWORD dwID, TCHAR **ppszName);
		STDMETHOD(GenerateBldSysCompPackageId)(TCHAR *pszName, WORD *pwID);
		STDMETHOD(LookupBldSysComp)(DWORD dwID, void **ppBldSysCmp);
		STDMETHOD(GetProjTypeFromProjItem)(void *pProjItem, int *iProjType);
		STDMETHOD(GetProjItemAttributes)(void *pProjItem, int *iAttrs);
	END_INTERFACE_PART(BuildComponents)

	BEGIN_INTERFACE_PART(BuildStatus, IBuildStatus)
		INIT_INTERFACE_PART(CBldPackage, BuildStatus)
		STDMETHOD(IsBuildPossible)();
		STDMETHOD(GetErrorCount)(LPDWORD pdwErrorCount);
		STDMETHOD(GetWarningCount)(LPDWORD pdwWarningCount);
	END_INTERFACE_PART(BuildStatus)

	BEGIN_INTERFACE_PART(BuildDirManager, IBuildDirManager)
		INIT_INTERFACE_PART(CBldPackage, BuildDirManager)
		STDMETHOD(GetPlatformIndex)(LPCTSTR lpszPlatform, int* pnPlatform);
		STDMETHOD(GetDirList)(int nPlatform, DIRLIST_TYPE type,
			const CObList** ppList);	// Non-standard COM
		STDMETHOD(GetDirListString)(int nPlatform, DIRLIST_TYPE type,
			LPSTR* ppszPath);
		STDMETHOD(FindFileOnPath)(int nPlatform, DIRLIST_TYPE type, LPCSTR szFile,
			LPSTR* ppszFullPath);
	END_INTERFACE_PART(BuildDirManager)

	BEGIN_INTERFACE_PART(AutoBld, IAutoBld)
		STDMETHOD(Build)(THIS_ const VARIANT FAR& configuration, BOOL bRebuildAll);
		STDMETHOD(GetDefaultConfiguration)(THIS_ LPDISPATCH* DefaultConfiguration);
		STDMETHOD(SetDefaultConfiguration)(THIS_ LPDISPATCH DefaultConfiguration);
		STDMETHOD(Clean)(THIS_ const VARIANT FAR& configuration);
		STDMETHOD(Errors)(THIS_ long &nErrors);
		STDMETHOD(Warnings)(THIS_ long &nWarnings);
		STDMETHOD(AddProject)(THIS_ LPCTSTR szName, LPCTSTR szPath, LPCTSTR szType, VARIANT_BOOL bAddDefaultFolders);
	END_INTERFACE_PART(AutoBld)

	BEGIN_INTERFACE_PART(PkgProjectProvider, IPkgProjectProvider)
		INIT_INTERFACE_PART(CBldPackage, PkgProjectProvider)
		STDMETHOD(AddNewProjectTypes)(IProjectWorkspace *pPW);
		STDMETHOD(ReleaseProjectTypeInfo)();
		STDMETHOD(OpenProject)(LPCOLESTR szFileName, IPkgProject ** ppCreatedProject); 	// Non-standard COM.
		STDMETHOD(CanCreateProject)(LPCOLESTR szFileName, DWORD dwProject_ID,		// Non-standard COM.
								BOOL *pbCanCreate);
		STDMETHOD(CreateProject)(LPCOLESTR szFileName, DWORD dwProject_ID,		// Non-standard COM.
								IPkgProject ** ppCreatedProject,
								DWORD * pdwRetVal);
		STDMETHOD(GetPlatforms)(DWORD dwProject_ID, 		// Non-standard COM.
								LPCOLESTR ** ppszTargetNames,
								BOOL ** pDefaultSelections,
								ULONG *pCount);
		STDMETHOD(ChangePlatformSelection)(ULONG TargetIndex, BOOL bSelected);
	END_INTERFACE_PART(PkgProjectProvider)
	DECLARE_INTERFACE_MAP()
// CommDlg interface
public:
	virtual void GetOpenDialogInfo(SRankedStringID** ppFilters,
		SRankedStringID** ppEditors, int nOpenDialog);
	virtual CDocTemplate* GetTemplateFromEditor(UINT idsEditor,
		const char* szFilename);

	virtual DWORD GetIdRange(RANGE_TYPE rt);
	
	// Build platforms.
	BOOL IsSelectedPlatform();
	void WriteRegPlatforms();

// ISetJavaProjectInfo interface map
protected:
	BEGIN_INTERFACE_PART(SetJavaProjectInfo, ISetJavaProjectInfo)
		INIT_INTERFACE_PART(CBldPackage, SetJavaProjectInfo)
		STDMETHOD_(BOOL, SetPrimaryClassName)(CString& strProjName, CString& strPrimaryClassName);
	END_INTERFACE_PART(SetJavaProjectInfo)

protected:
    CDefBuildNode * m_pDefNode;

	// List of possible platforms
	CStringArray m_chklstPlatforms;  // ANSI version

	int m_pnPlatforms[MAX_PLATFORMS];	// Array of flags to indicate which platforms were selected.
										//  m_pnPlatforms[i] = 0 or 1 implies the platform in m_chklstPlatforms
										//  at 0-based index i was unselected or selected, respectively.

	// Number of appWizard project types
	int m_nNumApwzTypes;
	CProjTypeList *m_pType;  // List of project types

	//
	// Interface needed for looking at editor documents.
	//
	PSRCUTIL m_pSrcUtil;
};

class CEnumBuildTools : public CCmdTarget {
public:
	CEnumBuildTools(){
		m_pos = NULL;
		m_pList = g_prjcompmgr.GetTools();
	};

	LPENUMBUILDTOOLS GetInterface(void){
		LPENUMBUILDTOOLS pIEnum;
		m_xEnumBuildTools.QueryInterface(IID_IEnumBuildTools, (void **)&pIEnum);
		return pIEnum;
	}

protected:
	BEGIN_INTERFACE_PART(EnumBuildTools, IEnumBuildTools)
		INIT_INTERFACE_PART(CEnumBuildTools, EnumBuildTools)
		STDMETHOD(Next)(THIS_ ULONG celt, LPBUILDTOOL *rgelt, ULONG *pceltFetched);
		STDMETHOD(Skip)(THIS_ ULONG celt);
		STDMETHOD(Reset)(THIS);
		STDMETHOD(Clone)(THIS_ IEnumBuildTools **ppenum);
	END_INTERFACE_PART(EnumBuildTools)
	DECLARE_INTERFACE_MAP()

private:
	const CPtrList *m_pList;
	POSITION m_pos;
};
	
BEGIN_INTERFACE_MAP(CEnumBuildTools, CCmdTarget)
	INTERFACE_PART(CEnumBuildTools, IID_IEnumBuildTools, EnumBuildTools)
END_INTERFACE_MAP()

class CEnumProjectTypes : public CCmdTarget {
public:
	CEnumProjectTypes(){
		m_pos = NULL;
		m_pList = g_prjcompmgr.GetProjTypes();
		m_pos = m_pList->GetHeadPosition();
	};
	LPENUMPROJECTTYPES GetInterface(void){
		LPENUMPROJECTTYPES pIEnum;
		pIEnum = (LPENUMPROJECTTYPES)CCmdTarget::GetInterface(&IID_IEnumProjectTypes);
		pIEnum->AddRef();
		return pIEnum;
	}

protected:
	BEGIN_INTERFACE_PART(EnumProjectTypes, IEnumProjectTypes)
		INIT_INTERFACE_PART(CEnumProjectTypes, EnumProjectTypes)
		STDMETHOD(Next)(THIS_ ULONG celt, LPPROJECTTYPE *rgelt, ULONG *pceltFetched);
		STDMETHOD(Skip)(THIS_ ULONG celt);
		STDMETHOD(Reset)(THIS);
		STDMETHOD(Clone)(THIS_ IEnumProjectTypes **ppenum);
	END_INTERFACE_PART(EnumProjectTypes)

	DECLARE_INTERFACE_MAP()

private:
	const CPtrList *m_pList;
	POSITION m_pos;
};
	
BEGIN_INTERFACE_MAP(CEnumProjectTypes, CCmdTarget)
	INTERFACE_PART(CEnumProjectTypes, IID_IEnumProjectTypes, EnumProjectTypes)
END_INTERFACE_MAP()

class CEnumPlatforms : public CCmdTarget {
public:
	CEnumPlatforms(){
		m_pos = NULL;
		m_pList = g_prjcompmgr.GetPlatforms();
	};

	LPENUMPLATFORMS GetInterface(void){
		LPENUMPLATFORMS pIEnum;
		m_xEnumPlatforms.QueryInterface(IID_IEnumPlatforms, (void **)&pIEnum);
		return pIEnum;
	}

protected:
	BEGIN_INTERFACE_PART(EnumPlatforms, IEnumPlatforms)
		INIT_INTERFACE_PART(CBldPackage, EnumPlatforms)
		STDMETHOD(Next)(THIS_ ULONG celt, LPPLATFORM *rgelt, ULONG *pceltFetched);
		STDMETHOD(Skip)(THIS_ ULONG celt);
		STDMETHOD(Reset)(THIS);
		STDMETHOD(Clone)(THIS_ IEnumPlatforms **ppenum);
	END_INTERFACE_PART(EnumPlatforms)
	DECLARE_INTERFACE_MAP()
private:
	const CPtrList *m_pList;
	POSITION m_pos;
};
	
BEGIN_INTERFACE_MAP(CEnumPlatforms, CCmdTarget)
	INTERFACE_PART(CEnumPlatforms, IID_IEnumPlatforms, EnumPlatforms)
END_INTERFACE_MAP()

CBldPackage::CBldPackage()
	: CPackage(extensionDLL.hModule, PACKAGE_VPROJ, PKS,
		MAKEINTRESOURCE(IDCT_BLDPACKAGE),
		MAKEINTRESOURCE(IDB_PROJTOOLS),
		MAKEINTRESOURCE(IDACCEL_PROJECT),
		MAKEINTRESOURCE(IDB_PROJLARGETOOLS)),
		m_pSrcUtil(NULL)
{
	m_pDefNode = NULL;
	m_pType = NULL;
}

////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBldPackage, CPackage)
	//{{AFX_MSG_MAP(CBldPackage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// interface part definitions should be added here

BEGIN_INTERFACE_MAP(CBldPackage, CPackage)

	// these are the basis of the old way ( <= vc4 )
	INTERFACE_PART(CBldPackage, IID_IBuildSystem, BuildSystem)
	INTERFACE_PART(CBldPackage, IID_IBuildStatus, BuildStatus)
	INTERFACE_PART(CBldPackage, IID_IBuildWizard, BuildWizard)
	INTERFACE_PART(CBldPackage, IID_IBuildComponents, BuildComponents)
	INTERFACE_PART(CBldPackage, IID_IBuildPlatforms, BuildPlatforms)

	INTERFACE_PART(CBldPackage, IID_IBuildDirManager, BuildDirManager)
	INTERFACE_PART(CBldPackage, IID_IAutoBld, AutoBld)
	INTERFACE_PART(CBldPackage, IID_IPkgProjectProvider, PkgProjectProvider)
	INTERFACE_PART(CBldPackage, IID_ISetJavaProjectInfo, SetJavaProjectInfo)

END_INTERFACE_MAP()

////////////////////////////////////////////////////////////////////////////

BOOL CBldPackage::OnCmdMsg(
							UINT nID,
							int nCode,
							void * pExtra,
							AFX_CMDHANDLERINFO * pHandlerInfo)
{
	CDocument *	pDoc;

	// let the standard message routing occur
	if (CPackage::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// if we don't have an active project, certain commands are
	// still allowed...but we have to create a default project first
	if (NULL==g_pProjWksIFace)
	{
		// Orion Bug 14910 
		//ASSERT(NULL!=g_pProjWksIFace);
		return FALSE ;
	}
	
	COleRef<IPkgProject> pPkgProject ;
	if (g_pActiveProject == NULL && SUCCEEDED(g_pProjWksIFace->GetActiveProject(&pPkgProject)) && pPkgProject.IsNull())
	{
		// Initialise the mecr menu text
		if	(nID == IDM_PROJECT_TOOL_MECR)
		{
			if (nCode == CN_UPDATE_COMMAND_UI)
			{
				if (::IsMenu((CCmdUI *)pExtra))
				{
					CString	strMenuName;
					strMenuName.LoadString(IDS_DEFFERED_MECR);
					((CCmdUI *)pExtra)->SetText(strMenuName);
				}

				// Not enabled if there is no project
				((CCmdUI *)pExtra)->Enable(FALSE);

				return TRUE;
			}
			if (nCode == CN_COMMAND)
			{
				// Should never try to invoke this
				// command without having a project.
				ASSERT(FALSE);
			}
		}

		if (nID == ID_TARGET_COMBO && nCode == CN_UPDATE_COMMAND_UI)
		{	
			// Not enabled if there is no project
			((CCmdUI *)pExtra)->Enable(FALSE);

			return TRUE;
		}
		if (nID == ID_CONFIG_COMBO && nCode == CN_UPDATE_COMMAND_UI)
		{	
			// Not enabled if there is no project
			((CCmdUI *)pExtra)->Enable(FALSE);

			return TRUE;
		}
		if	( nID == IDM_DUMP_HEAP )
		{	
			if (nCode == CN_UPDATE_COMMAND_UI)
			{
				// Not enabled if there is no project
				((CCmdUI *)pExtra)->Enable(TRUE);
	
				return TRUE;
			}
			else if (nCode == CN_COMMAND)
			{
#ifdef _DEBUG
				DumpHeap();
#endif
			}
		}
		
		
		if	(
			(nID != IDM_PROJITEM_COMPILE)
			&&
			(nID != IDM_PROJITEM_BUILD)
			&&
			(nID != IDM_BUILD_TOGGLE)
			&&
			(nID != IDM_PROJITEM_REBUILD_ALL)
			)
			return FALSE;	// Not a command that can be tried without a project

		if (nCode == CN_UPDATE_COMMAND_UI)
		{

// ****** PERFORMANCE WARNING ********
// DO NOT USE ANY FUNCTIONS IN HERE THAT MAY TOUCH THE DISK DRIVE.
// FUNCTIONS TO AVOID ARE path.Create, AS THIS WILL CALL GetFullPath
// WHICH IS NOT CACHED ON REMOVEABLE MEDIA!!! - SO FOR EVERY MESSAGE
// WE WOULD HIT THE DISK!!!!!.

			ASSERT(pExtra != NULL);

			CString strFileName;
			CString strName;
			BOOL bEnable = FALSE, bUntitled = TRUE;

			if	(
				(pDoc = g_Spawner.GetLastDocWin())
				&&
				(!g_Spawner.SpawnActive())
				)
			{
				strFileName = pDoc->GetPathName();
				bUntitled = strFileName.IsEmpty();

				if (bUntitled)
				{
					//
					// Make sure it doesn't have a pseudo-name which may mean this
					// is something other than a file.
					//

					//
					// This will be release in CBldPackage::OnExit()
					//
					ISourceEdit * lpse=NULL;

					HRESULT hr = E_NOINTERFACE;
					if (NULL!=m_pSrcUtil)
						hr = m_pSrcUtil->CreateSourceEditFromDoc(pDoc, &lpse, FALSE /* doesn't need to be writable for test*/);

					if(SUCCEEDED(hr))
					{
						ULONG cbLen=0;

						VERIFY(SUCCEEDED(lpse->GetPseudoPathName(NULL,0,&cbLen)));
						lpse->Release();

						// Assume that if cbLen is zero, this is a source file.  We'll force it to be
						// saved if the command actually gets executed.
						bEnable = (0==cbLen);
					}
					else
					{
						//
						// No way to tell what we have.
						//
						ASSERT(0);
						bEnable = FALSE;
					}
				}
				else
				{
					// Get the filename and extension
					char szExt [_MAX_EXT];
					_splitpath(strFileName, NULL, NULL,
						strName.GetBuffer(_MAX_FNAME), szExt);
					strName.ReleaseBuffer();
					strFileName = strName + szExt;

					static BOOL bFortranSupported, bOnce=FALSE;
					if (!bOnce)
					{
						bOnce=TRUE;
						bFortranSupported = IsPackageLoaded(PACKAGE_LANGFOR);
					}
					if (!bJavaOnce)
					{
						bJavaOnce=TRUE;
						bJavaSupported = IsPackageLoaded(PACKAGE_LANGJVA);
					}

					bEnable =
						_tcsicmp(szExt, _T(".c")) == 0 ||
						_tcsicmp(szExt, _T(".cpp")) == 0 ||
						_tcsicmp(szExt, _T(".cxx")) == 0 ||
						(bFortranSupported && (
						_tcsicmp(szExt, _T(".f")) == 0 ||
						_tcsicmp(szExt, _T(".for")) == 0 ||
						_tcsicmp(szExt, _T(".f90")) == 0 ) ||
						(bJavaSupported && _tcsicmp(szExt, _T(".java")) == 0 ));
				}
			}

			if (::IsMenu((CCmdUI *)pExtra))
			{
				CString	strMenuName;
				TCHAR szMenuText[ _MAX_PATH + 40 ];
			
				if (nID == IDM_PROJITEM_COMPILE)
				{
					strMenuName.LoadString(IDS_PROJMENU_COMPILE);
					MenuFileText(strFileName);
					wsprintf(	szMenuText,
								strMenuName,
								!bEnable || bUntitled
									? _T("")
									: (LPCTSTR) strFileName);
					((CCmdUI *)pExtra)->SetText(szMenuText);
				}
				else if (nID == IDM_PROJITEM_BUILD)
				{
					strMenuName.LoadString(IDS_PROJMENU_BUILD);
					if (bEnable && !bUntitled)
					{
						if (!bJavaOnce)
						{
							bJavaOnce=TRUE;
							bJavaSupported = IsPackageLoaded(PACKAGE_LANGJVA);
						}
						if (!bJavaSupported)
						{
							strFileName = strName + _T(".exe");
						}
						else
						{
							char szExt [_MAX_EXT];
							_splitpath(strFileName, NULL, NULL,
							strName.GetBuffer(_MAX_FNAME), szExt);

							if (_tcsicmp(szExt, _T(".java")) == 0)
								strFileName = strName;
							else
								strFileName = strName + _T(".exe");
						}
					}
					MenuFileText(strFileName);
					wsprintf(	szMenuText,
								strMenuName,
								!bEnable || bUntitled
									? _T("")
									: (LPCTSTR) strFileName);
					((CCmdUI *)pExtra)->SetText(szMenuText);
				}
			}

			((CCmdUI *)pExtra)->Enable(bEnable);
			return TRUE;
		}
		else if (nCode == CN_COMMAND)
		{
			if (pExtra != NULL || pHandlerInfo != NULL)
				return FALSE;

			if (theApp.m_bInvokedCommandLine)
				return FALSE; // not supported

			CString		strLastDocName;
			CPath		pathLastDocName;

			// Create a default project here so we can complete
			// the command. Make sure the user is willing to accept
			// this first.
			if (MsgBox(Question, IDS_CMD_REQUIRES_PROJ, MB_YESNO) == IDNO)
				return TRUE;		// Command is done (did nothing)

			VERIFY(pDoc = g_Spawner.GetLastDocWin());
			strLastDocName = pDoc->GetPathName();

			// Force the source file to be saved if it's untitled.
			if (strLastDocName.IsEmpty())
			{
				g_bWriteProject = TRUE;
				if (!pDoc->DoSave(strLastDocName))
					return TRUE;	// Command is done (did nothing)

				strLastDocName = pDoc->GetPathName();
				ASSERT(!strLastDocName.IsEmpty());
			}

			VERIFY(pathLastDocName.Create(strLastDocName));

            // UNDONE (colint)
			//CDocument *	pDocNewProj;

			LPPROJECTWORKSPACE pProjSysIFace = FindProjWksIFace();
            CDocument *pDoc;
			VERIFY(SUCCEEDED(pProjSysIFace->CreateDefaultWorkspace(&pDoc, pathLastDocName)));
            if (pDoc == NULL)
                return TRUE;

			// If we get here, we've got a default project and can
			// continue with the request as if we've always had one.
			ASSERT(g_pActiveProject != NULL);

			// But first...
			//
			// Make sure the project window is active and the file item
			// is selected -- if we're handling IDM_PROJITEM_COMPILE, that
			// command is selection-sensitive.
			CProjItem *	pItemAdded;
			
			VERIFY(pItemAdded = g_pActiveProject->EnsureFileInProject(&pathLastDocName, FALSE));
		}
		else
		{
			return FALSE;
		}
	}
	else if (g_pActiveProject == NULL)
	{
		if( nID == IDM_PROJECT_SETTINGS || nID == IDM_PROJECT_SETTINGS_POP )
		{	
			const CObList * pProjList = CProject::GetProjectList();
			POSITION pos = pProjList->GetHeadPosition();
			CProject *pProject = NULL;
			while (pos != NULL)
			{
				pProject = (CProject *)pProjList->GetNext(pos);
				if ((pProject==NULL) || (!pProject->IsLoaded()))
					continue;
				break;
			}
			if( pProject ){
				if (nCode == CN_UPDATE_COMMAND_UI)
				{
					// Not enabled if there is no project
					((CCmdUI *)pExtra)->Enable(TRUE);
		
					return TRUE;
				}
				else if (nCode == CN_COMMAND)
				{
					CProjTempProjectChange proj(pProject);
					return pProject->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
				}
			}
		}

		return FALSE;
	}


	// We shouldn't get here unless we have an active project.
	ASSERT(g_pActiveProject != NULL);
	// Hack for wizards
	if( nID == IDM_PROJECT_OPENDIALOG )
	{
		if( nCode == CN_UPDATE_COMMAND_UI )
		{
			// always available
			((CCmdUI *)pExtra)->Enable(TRUE);
		}
		if( nCode == CN_COMMAND )
		{
			if( !g_strDialogName.IsEmpty() ) {
				g_BldSysIFace.OpenResource( g_strDialogName, NULL, ACTIVE_TARGET, (HBUILDER)g_pActiveProject );
				g_strDialogName.Empty();
			}
		}
		return TRUE;
	}


	return g_pActiveProject->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CBldPackage::AddOptDlgPages(class CTabbedDialog* pOptionsDialog, OPTDLG_TYPE type)
{
	switch (type)
	{
		case options:
		{
			// General build system options tab
			CDlgTab * pBldTab = new CBldOptDlg;
			pBldTab->m_nOrder = 49;
			pOptionsDialog->AddTab(pBldTab);

			// shouldn't invoke this dialog with no platform toolsets
			// the dialog is not operational without *at least one* platform
			CDirMgr * pDirMgr = GetDirMgr();
			ASSERT(pDirMgr != (CDirMgr *)NULL);
			if (pDirMgr->GetNumberOfToolsets() > 0)
			{
				CDlgTab* pTab = new CDirsOptDlg;
				pTab->m_nOrder = 50;
				pOptionsDialog->AddTab(pTab);
			}
			break;
		}
		
		case customize:
			// add customize pages here...
			break;
	}
}

BOOL CBldPackage::OnNotify(UINT id, void *Hint /* = NULL */, void *ReturnArea /* = NULL */)
{
	// what is the notification?
	switch (id)
	{
		case PN_BEGIN_BUILD:
		case PN_END_BUILD:
		{
			if (!g_bCommandLineBuild) // msdev /make specified?
			{
				// do we have a current page and is it a project page?
				CProjPage* pSlobPage = (CProjPage*) GetCurrentPropertyPage();
				if (pSlobPage != NULL &&
					pSlobPage->IsKindOf(RUNTIME_CLASS(CProjPage)))
				{
					if (id == PN_BEGIN_BUILD && pSlobPage->DisableDuringBuild())
						pSlobPage->EnablePage(FALSE);
					else if (id == PN_END_BUILD)
						pSlobPage->EnablePage();
				}
			}
			break;
		}

		case PN_CLOSE_PROJECT:
		{
			RefreshTargetCombos();
			CProject * pProject = g_pActiveProject; // default
			if(Hint != NULL)
			{
				IPkgProject *pPkgProj = (IPkgProject *)Hint;
				COleRef<IBSProject> pBSProj;
				if(SUCCEEDED(pPkgProj->QueryInterface(IID_IBSProject, (void **)&pBSProj)))
					pBSProj->GetCProject((void **) &pProject);
				else
					pProject = NULL;
			}
			if (pProject != NULL)
			{
				HBUILDER hBld = (HBUILDER)pProject;
				HBLDTARGET hTarget = g_BldSysIFace.GetActiveTarget(hBld);
				pProject->m_listTargIncl.Remove(hTarget);

				// A Builder is being deleted
				LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();

				CBuildViewSlob * pTargetSlob;
				GetBuildNode()->InitBldNodeEnum();
				while (GetBuildNode()->NextBldNodeEnum((void **)&pTargetSlob))
				{
					ASSERT(pTargetSlob != NULL);
					if (pTargetSlob->GetBuilder() == hBld)
					{
						// attempt to remove this node--this will only succeed if the node
						// is visible in the FileView tree, and is unnecessary otherwise.
						pProjSysIFace->RemoveSlob(pTargetSlob, FALSE);
					}
				}
			}
 			break;
		}

		case PN_NEW_PROJECT:
			GetBuildNode()->ShowBuildPane();
		case PN_OPEN_PROJECT:
			{
				CProject * pProject = g_pActiveProject; // default
				if(Hint != NULL)
				{
					IPkgProject *pPkgProj = (IPkgProject *)Hint;
					COleRef<IBSProject> pBSProj;
					if(SUCCEEDED(pPkgProj->QueryInterface(IID_IBSProject, (void **)&pBSProj)))
						pBSProj->GetCProject((void **) &pProject);
					else
						pProject = NULL;
				}
				// make a copy of project setting:
				if (pProject != NULL)
				{
					// should have happened by now, unless deleted .opt file
					if (!pProject->m_bPrivateDataInitialized)
					{
						pProject->InitPrivateData();
					}
					HBUILDER hBld = (HBUILDER)pProject;
					pProject->m_listTargIncl.Init(hBld);

					LPPROJECTWORKSPACE pProjSysIFace = FindProjWksIFace();
					if (pProjSysIFace->IsWorkspaceInitialised()==S_OK)
					{
						LPPROJECTWORKSPACEWINDOW pProjSysWinIFace = FindProjWksWinIFace();
						g_BldSysIFace.InitFileSetEnum(hBld);
						HBLDTARGET hTarget;
						HFILESET hFileSet;
						CString strTarget;
						while ((hFileSet = g_BldSysIFace.GetNextFileSet(hBld)) != NO_FILESET)
						{
						    	g_BldSysIFace.GetTargetFromFileSet(hFileSet, hTarget, hBld);
							if (hTarget != NO_TARGET)
							{
								CSlob * pNewSlob = GetBuildNode()->CreateNode(hTarget, hBld);
								if (m_pDefNode==NULL)
									m_pDefNode = new CDefBuildNode;
								m_pDefNode->Add(pNewSlob);
								VERIFY(SUCCEEDED(pProjSysWinIFace->AddRootProvidedNode(m_pDefNode, pNewSlob)));
							}
						}
					}
				}
			}
			if( g_bWorkspaceInited == TRUE ){
				g_bConversionPrompted = FALSE; // reset for next time
				::InitWorkspaceDeps();
				RefreshTargetCombos();
			}

            break;

#if 0
		case PN_DEL_FILE:
		{
			LPSOURCECONTROL pSccManager;
			VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));

			if (IsSccActive() && (pSccManager->RemoveDeletedFiles() == S_OK))
			{
				CPath * pPath;
				CStringArray * pFileArray = new CStringArray;
				CProject * pProject = g_pActiveProject;
				NOTIFYINFO * pInfo = (NOTIFYINFO *)Hint;
				POSITION pos = pInfo->lstFiles.GetHeadPosition();
				while (pos != NULL)
				{
					pPath = pInfo->lstFiles.GetNext(pos);
					int iStatus;
					pSccManager->GetStatus(&iStatus, *pPath);
					if ((!pProject->IsProjectFile(pPath)) &&
						(iStatus & (SCC_STATUS_CONTROLLED|SCC_STATUS_DELETED))==SCC_STATUS_CONTROLLED))
					{
						// UNDONE: may also have to update status first
						pFileArray->Add(*pPath);
					}
				}
				CString strOut;
				if ((pFileArray->GetSize() > 0) && ((pSccManager->AlwaysPrompt() == S_FALSE) || (MsgBox(Question, MsgText(strOut, IDS_SCC_MSG_REMDEL), MB_YESNO)==IDYES)))
				{
					// FUTURE: message box
					pSccManager->Remove(pFileArray);
				}
				else
				{
					delete pFileArray;
				}
			}

			pSccManager->Release();

			break;
		}
#endif

#if 0
		case PN_ADD_FILE:
		{
			LPSOURCECONTROL pSccManager;
			VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));

			if (IsSccActive() && (pSccManager->AddNewFiles() == S_OK))
			{
				// UNDONE: handled deps properly
				CPath * pPath;
				CStringArray * pFileArray = new CStringArray;
				CProject * pProject = g_pActiveProject;
				NOTIFYINFO * pInfo = (NOTIFYINFO *)Hint;
				POSITION pos = pInfo->lstFiles.GetHeadPosition();
				while (pos != NULL)
				{
					pPath = pInfo->lstFiles.GetNext(pos);
					int iStatus;
					pSccManager->GetStatus(&iStatus, *pPath);
					if ((pProject->IsProjectFile(pPath)) &&
						(iStatus & (SCC_STATUS_CONTROLLED|SCC_STATUS_DELETED))!=SCC_STATUS_CONTROLLED))
					{
						// UNDONE: may also have to update status first
						pFileArray->Add(*pPath);

						// HACK: remove this when fixed properly
						// try to add the corresponding .h file too
						const TCHAR * pExt = pPath->GetExtension();
						if ((_tcsicmp(pExt, _T(".cpp"))==0) ||
							(_tcsicmp(pExt, _T(".c"))==0) ||
							(_tcsicmp(pExt, _T(".cxx"))==0))
						{
							pExt = _T(".h");
						}
						else
						{
							// FUTURE: handle other languages
							continue;
						}
						CPath pathH = *pPath;
						pathH.ChangeExtension(pExt);
						pSccManager->GetStatus(&iStatus, pathH);
						if ((pathH.ExistsOnDisk()) &&
							(iStatus & (SCC_STATUS_CONTROLLED|SCC_STATUS_DELETED))!=SCC_STATUS_CONTROLLED))
						{
							pFileArray->Add(pathH);
						}
					}
				}
				CString strOut;
				if ((pFileArray->GetSize() > 0) && (MsgBox(Question, MsgText(strOut, IDS_SCC_MSG_ADDNEW), MB_YESNO)==IDYES))
				{
					// FUTURE: message box
					VERIFY(SUCCEEDED(pSccManager->Add(pFileArray)));
				}
				else
				{
					delete pFileArray;
				}
			}

			pSccManager->Release();
				
			break;
		}
#endif

		case PN_QUERY_RENAME_FILE:
		{
			
			CPath * pOldPath = &((NOTIFYRENAME *)Hint)->OldFileName;
			CPath * pNewPath = &((NOTIFYRENAME *)Hint)->NewFileName;

			if (FileNameMatchesExtension(pOldPath, "dsp;dsw;opt;clw;ncb;aps;rc;def;hpj"))
			{
				// This file cannot be renamed
				CString str;
				MsgBox (Error, MsgText ( str, IDS_RENAME_DENIED, (LPCTSTR)*pOldPath));
				return FALSE;
			}

			if ((FileNameMatchesExtension(pNewPath, "dsp;dsw;opt;clw;ncb;aps;rc;def;hpj")) || (g_BldSysIFace.IsProjectFile(pNewPath, FALSE, ACTIVE_BUILDER)))
			{
				// Can't be renamed to newPath
				CString str;
				MsgBox (Error, MsgText ( str, IDS_RENAME_BADDEST, (LPCTSTR)*pOldPath, (LPCTSTR)*pNewPath));
				return FALSE;
			}
			break;
		}

		case PN_RENAME_FILE:
		{
			NOTIFYRENAME * pInfo = (NOTIFYRENAME *)Hint;
			ASSERT(pInfo);
			
			FileRegHandle frh = CFileRegFile::LookupFileHandleByName( pInfo->OldFileName );
			if (frh == NULL)
				break;
	
			CObList * pItemList = g_FileRegistry.GetRegEntry(frh)->plstFileItems;
			if ((pItemList == NULL) || (pItemList->IsEmpty()))
			{
				frh->ReleaseFRHRef();
				break;
			}

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
				VERIFY(pItem->SetFile(&(pInfo->NewFileName), TRUE));
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
			g_FileGeneralPage.InitializePage();

			frh->ReleaseFRHRef();
			break;
		}

		case PN_BEGIN_MULTIOP:
		{
			CProject::BeginBatch();
			int nID = (int)Hint;
			switch (nID)
			{
				case IDM_PROJITEM_CCOMPILE:
				case IDM_PROJITEM_CBUILD:
				case IDM_PROJITEM_CBUILD_NO_DEPS:
					g_bMultiBuild = TRUE;
					g_bStopBuild=FALSE;
			}
			break;
		}
		case PN_END_MULTIOP:
		{
			CProject::EndBatch();
			int nID = (int)Hint;
			switch (nID)
			{
				case IDM_PROJITEM_CCOMPILE:
				case IDM_PROJITEM_CBUILD:
				case IDM_PROJITEM_CBUILD_NO_DEPS:
					g_bMultiBuild = FALSE;
					g_bStopBuild=FALSE;
			}
			break;
		}

		case PN_WORKSPACE_CLOSE:
		{
			// cleanup any unused proj deps refs
			g_bWorkspaceInited = FALSE;
			g_bConversionPrompted = FALSE; // reset for next time
			while (!g_lstprojdeps.IsEmpty())
			{
				delete (CProjDepInfo *)g_lstprojdeps.RemoveHead();
			}
			break;
		}

		case PN_WORKSPACE_INIT:
		{
			CProject * pProject = g_pActiveProject; // default
			g_bWorkspaceInited = TRUE;
			g_bConversionPrompted = FALSE; // reset for next time
			::InitWorkspaceDeps();

#ifndef _DEBUG	// UNDONE
			// StopCAP();
#endif
			if (g_bCommandLineBuild) // msdev /make specified?
			{
				// default to active config
				CStringList * pConfigs = NULL;
				BOOL bNoBuild = FALSE;
				if(!g_strlCfgsToBuild.IsEmpty() )
				{
					CMapStringToPtr mapConfigs;
					CString strKey;
					void * pVoid;
					pConfigs = &g_strlCfgsToBuild;
					// validate configs
					CString strConfig;
					POSITION curpos, pos = pConfigs->GetTailPosition();
					while (pos != NULL)
					{
						curpos = pos; // save
						strConfig = pConfigs->GetPrev(pos);
						strKey = strConfig;
						strKey.MakeUpper();
						HBLDTARGET hTarget;
						if (mapConfigs.Lookup(strKey, pVoid))
						{
							// ignore duplicate
							pConfigs->RemoveAt(curpos);
						}
						else if ((g_BldSysIFace.GetFileSetFromTargetName(strConfig, NO_BUILDER) == NO_FILESET) && (_tcsnicmp(strConfig, _T("ALL"),3)!=0) && (_tcsnicmp(strConfig, _T("NOBUILD"),7)!=0))
						{
							pConfigs->RemoveAt(curpos);
							CString strOut;
							strOut.LoadString(IDS_WRN_UNKNOWN_PROJECT);
							strOut += strConfig;
							theApp.WriteLog(strOut, TRUE);
						}
						else if ((hTarget = g_BldSysIFace.GetTarget(strConfig, NO_BUILDER)) == NO_TARGET)
						{
							pConfigs->RemoveAt(curpos);

							// parse for possible meta-targets
							// - DEBUG
							// - RELEASE
							// - ALL
							// - NOBUILD
							BOOL bFoundMatch = FALSE;
							BOOL bAllProjects = FALSE;
							BOOL bAllConfigs = FALSE;
							CString strProject, strPattern;
							int nLen = 0;
							if (strConfig.CompareNoCase(_T("ALL"))==0)
							{
								bAllProjects = bAllConfigs = TRUE;
							}
							else if (strConfig.CompareNoCase(_T("NOBUILD"))==0)
							{
								bAllProjects = bAllConfigs = FALSE;
								bNoBuild = TRUE;
								break;
							}
							else if ((nLen = strConfig.Find(_T(" - "))) > 0)
							{
								// first try to identify the project
								strProject = strConfig.Left(nLen);
								strPattern = strConfig.Mid(nLen + 3);
								strPattern.MakeUpper();
								if (strProject.CompareNoCase(_T("ALL"))==0)
								{
									bAllProjects = TRUE;
								}
							}
							if (bAllConfigs || (nLen > 0))
							{
								
								CString strPlatformAndFlavour, strTarget;
								HBUILDER hBld = g_BldSysIFace.GetBuilderFromName(strProject);
								HBLDTARGET hTarg;
								if ((hBld != NO_BUILDER) && !bAllConfigs)
								{
									bAllProjects = FALSE; // project named ALL
								}
								bAllConfigs = bAllConfigs || (strPattern.Compare(_T("ALL")) == 0);
								if (hBld != NO_BUILDER || bAllProjects)
								{
									g_BldSysIFace.InitBuilderEnum();
									while( (pProject  = (CProject *)g_BldSysIFace.GetNextBuilder(strProject, TRUE)) != (CProject *)NO_BUILDER)
									{
										if (bAllProjects)
										{
											hBld = (HBUILDER)pProject;
											nLen = strProject.GetLength();
										}
										else if (hBld != (HBUILDER)pProject)
										{
											continue;
										}
										ASSERT(nLen == strProject.GetLength());

										g_BldSysIFace.InitTargetEnum(hBld);
										while ((hTarg = g_BldSysIFace.GetNextTarget(strTarget, hBld)) != NO_TARGET)
										{
											CTargetItem * pTarget = g_BldSysIFace.CnvHTarget(hBld, hTarg);
											strPlatformAndFlavour = strTarget.Mid(nLen + 3);
											strPlatformAndFlavour.MakeUpper();
											if ((bAllConfigs) || (strPlatformAndFlavour.Find(strPattern) != -1))
											{
												// matched pattern
												ConfigurationRecord * pcr = pProject->ConfigRecordFromConfigName(strTarget);
												if ((pcr != NULL) && (pcr->IsSupported()) && (pcr->IsBuildable()))
												{
													bFoundMatch = TRUE;
													strKey = strTarget;
													strKey.MakeUpper();
													if (!mapConfigs.Lookup(strKey, pVoid))
													{
														mapConfigs.SetAt(strKey, NULL);
														pConfigs->AddTail(strTarget);
													}
												}
											}
										}
									}
								}
							}
							if (!bFoundMatch && !bAllConfigs)
							{
								CString strOut;
								strOut.LoadString(IDS_WRN_UNKNOWN_TARGET);
								strOut += strConfig;
								theApp.WriteLog(strOut, TRUE);
							}
						}
						else
						{
							HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);
							ASSERT(hBld != NO_BUILDER);
							CProject * pProject = g_BldSysIFace.CnvHBuilder(hBld);
							ConfigurationRecord * pcr = pProject->ConfigRecordFromConfigName(strConfig);
							if ((pcr == NULL) || (!pcr->IsSupported()) || (!pcr->IsBuildable()))
							{
								pConfigs->RemoveAt(curpos);
								CString strOut;
								strOut.LoadString(IDS_WRN_UNSUPPORTED_TARGET);
								strOut += strConfig;
								theApp.WriteLog(strOut, TRUE);
							}
							else
							{
								strKey = strConfig;
								strKey.MakeUpper();
								mapConfigs.SetAt(strKey, NULL);
							}

						}

					}
				}
				if((!bNoBuild) && ((!g_strlCfgsToBuild.IsEmpty()) || (pConfigs==NULL && g_pActiveProject != NULL && (g_pActiveProject->GetActiveConfig() != NULL))))
				{

					BOOL bSaveLogState = g_bWriteBuildLog;
					g_bWriteBuildLog = TRUE;

					if (pConfigs == NULL)
					{
						pConfigs = &g_strlCfgsToBuild;
						if (g_strlCfgsToBuild.IsEmpty())
						{
							CString strConfig = g_pActiveProject->GetActiveConfig()->GetConfigurationName();
							pConfigs->AddTail(strConfig);

							CString strOut;
							strOut.LoadString(IDS_WRN_DEFAULT_TARGET);
							strOut += strConfig;
							theApp.WriteLog(strOut, TRUE);
						}
						else
						{
							ASSERT(0);  // should never happen
							CString strOut;
							strOut.LoadString(IDS_ERR_NO_TARGETS);
							theApp.WriteLog(strOut, TRUE);
						}
						ASSERT(pConfigs);
						ASSERT(!pConfigs->IsEmpty());
					}

					if ((pConfigs) && (!pConfigs->IsEmpty()))
						(VOID)g_pActiveProject->CmdBatchBuild((g_nTOB==TOB_ReBuild),pConfigs,g_bRecurse /* TRUE by default */, g_nTOB==TOB_Clean);

					g_bWriteBuildLog = bSaveLogState;
					g_strlCfgsToBuild.RemoveAll();
				}
				else
				{
					CString strOut;
					if (bNoBuild)
						strOut.LoadString(IDS_WRN_NO_BUILD);
					else
						strOut.LoadString(IDS_ERR_NO_TARGETS);
					theApp.WriteLog(strOut, TRUE);
				}

				if (g_hStdIn != NULL)
				{
					CloseHandle(g_hStdIn);
					g_hStdIn = NULL;
				}
				theApp.WriteLog(NULL); // close stdout also

				// always exit if /make specified, even if no targets
				// because shell is invisible!
//				AfxGetMainWnd()->PostMessage(WM_CLOSE, 0L, CBuildIt::m_actualErrs);
				AfxGetMainWnd()->PostMessage(WM_QUIT, CBuildIt::m_actualErrs, 0);
			}
			else // !g_bCommandLineBuild
			{
	          	RefreshTargetCombos(); // REVIEW: needed?

				// Tests added to fix sys sniff. Changes by martynl, reviewed by karlsi.
				if (!pProject ||
					!pProject->IsExeProject())
				{
					InitExternalDepFolders();
				}
			}
			break;
		}

        case PN_QUERY_CLOSE_WORKSPACE:
        case PN_QUERY_CLOSE_PROJECT:
            if (g_Spawner.SpawnActive())
            {
				int nMsgID = (id == PN_QUERY_CLOSE_WORKSPACE ? IDS_NO_CLOSE_WKS_BUILDING : IDS_NO_CLOSE_PRJ_BUILDING);
                MsgBox(Information, nMsgID);
                return FALSE;
            }
            else
			{
                return TRUE;
			}
            break;

        case PN_QUERY_NEW_WORKSPACE:
        case PN_QUERY_OPEN_WORKSPACE:
			g_bConversionPrompted = FALSE; // reset for next time
            if (g_Spawner.SpawnActive())
            {
                MsgBox(Information, IDS_NO_OPEN_SPAWN_ACTIVE);
                return FALSE;
            }
            else
                return TRUE;
            break;

		case PN_QUERY_CHANGE_CONFIG:
            if (g_Spawner.SpawnActive())
            {
				BOOL fFailSilently = FALSE;
				if (Hint != NULL)
					fFailSilently = (BOOL)(*((BOOL *)Hint));
				if (!fFailSilently)
	                MsgBox(Information, IDS_NO_CHANGE_CONFIG_BUILDING);
                return FALSE;
            }
            else
                return TRUE;
            break;

		case PN_CHANGE_PROJECT:
			if (!g_bCommandLineBuild)
			{
#if 0		// not currently needed, since we always call PN_CHANGE_CONFIG too
			  PropagateActiveConfig();	// REVIEW
          	  RefreshTargetCombos();	// REVIEW
          	  GetBuildNode()->ConfigChange();
#endif
			}
			break;

#if 0
		case PN_ADD_TARGET:
			RefreshTargetCombos();
			break;
#endif
        case PN_CHANGE_CONFIG:
			if (!g_bCommandLineBuild)
			{
			  PropagateActiveConfig();	
          	  RefreshTargetCombos();
          	  GetBuildNode()->ConfigChange();
			}
            break;

		default:
			// Ignore
			break;
	}

	return TRUE ;
}	

void PropagateActiveConfig()
{
	if (NULL==g_pActiveProject)
		return;

	CProject * pProject;
	CString strProject = _T("<no name>");
	CString strConfig;
	CProject::InitProjectEnum();
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		if (pProject==g_pActiveProject)
			continue;

		CTargetItem *pTarg = pProject->GetActiveTarget();
		if (g_BldSysIFace.GetTargetNameFromFileSet((HFILESET)pTarg, strConfig, ACTIVE_BUILDER, TRUE /* REVIEW */ ))
		{
			pProject->SetActiveConfig(strConfig);
		}
	}
}

void RefreshTargetCombos(BOOL bEmpty /* = FALSE */)
{
	if (g_bCommandLineBuild)
		return;

	// Refresh the target combos
	CObList * plstCombos = CTargetCombo::GetCombos();
	POSITION pos = plstCombos->GetHeadPosition();
	while (pos != NULL)
	{
		CTargetCombo * pTargetCombo = (CTargetCombo *)plstCombos->GetNext(pos);
		if (bEmpty)
			pTargetCombo->ResetContent();
		else
			pTargetCombo->RefreshTargets();
	}
	
	plstCombos = CConfigCombo::GetCombos();
	pos = plstCombos->GetHeadPosition();
	while (pos != NULL)
	{
		CConfigCombo * pTargetCombo = (CConfigCombo *)plstCombos->GetNext(pos);
		if (bEmpty)
			pTargetCombo->ResetContent();
		else
			pTargetCombo->RefreshTargets();
	}
}

void CBldPackage::SerializeWkspcConvSettings(CStateSaver& stateSave, DWORD dwFlags)
{
	ASSERT(NULL!=g_pActiveProject);
	if (NULL==g_pActiveProject)
		return;

	CProject *pProject = g_pActiveProject;
	ASSERT(!stateSave.IsStoring());
	pProject->LoadFromVCP(stateSave);
}

void CBldPackage::SerializeWorkspaceSettings(CStateSaver& stateSave, DWORD dwFlags)
{
	CProject * pProject;
	CString strProject;
	g_BldSysIFace.InitBuilderEnum();
	CProject *pOldProject = g_pActiveProject;
	while( (pProject  = (CProject *)g_BldSysIFace.GetNextBuilder(strProject, TRUE)) != (CProject *)NO_BUILDER)
	{
		g_pActiveProject = pProject;
		if (stateSave.IsStoring())
		{
			pProject->SaveToOPT(stateSave);
		}
		else
		{	
			pProject->LoadFromOPT(stateSave);
		}
	}
	if( g_pActiveProject && !g_pActiveProject->IsExeProject() ){
		if (stateSave.IsStoring())
		{
			g_pActiveProject->SaveInternalToOPT(stateSave,FALSE,TRUE);
		}
		else
		{	
			g_pActiveProject->LoadInternalFromOPT(stateSave,FALSE,TRUE);
		}
	}

	// REVIEW (kperry) : create antoher stream and save workpace specific data here not in EVERY project

	g_pActiveProject = pOldProject;

}

BOOL GetOneLine( CString &strBlock, CString &strLine ) {
	if( strBlock.IsEmpty() ) return FALSE;

	int pos = strBlock.Find('\n');
	if( pos >= 0 ){
		strLine = strBlock.Left(pos);
		strBlock = strBlock.Right( strBlock.GetLength() -1 - pos );
	} else {
		strLine = strBlock;
		strBlock.Empty();
	}
	return TRUE;
}
CString strPD = "Project_Dep_Name ";
#define NEXT_BEGIN 1
#define NEXT_END 2
#define NEXT_CONTENT 3

void CBldPackage::SetProjectWorkspaceInfoText(LPCTSTR pProjName, LPCTSTR pInfo ){
	// this sad little state machine reads the contentss of our block.
	int state = NEXT_BEGIN;
	CString strBlock = pInfo;
	CString strLine;
	while( GetOneLine( strBlock, strLine ) ){
		if( strLine == "Begin Project Dependency" ){
			if( state == NEXT_BEGIN )
				state = NEXT_CONTENT;
			else
				return; // error
		}
		else if( strLine == "End Project Dependency" ){
			if( state == NEXT_END )
				state = NEXT_BEGIN;
			else
				return; // error
		}
		else {
			if( state ==  NEXT_CONTENT ) {
				CString strTok = strLine.Left(strPD.GetLength());
				if( strTok == strPD ) {
					strTok = strLine.Right(strLine.GetLength()-strPD.GetLength());

					// check if this one is already there
					BOOL bFound = FALSE;
					POSITION pos = g_lstprojdeps.GetHeadPosition();
					while (pos != NULL)
					{
						CProjDepInfo *pDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
						if( pDepInfo->strProject == pProjName && pDepInfo->strTarget == strTok ){
							// we already have this one.
							bFound = TRUE;
							break;
						}
					}

					if( bFound == FALSE ){
						CProjDepInfo * pProjDepInfo = new CProjDepInfo;
						pProjDepInfo->pProjDep = NULL;
						pProjDepInfo->strTarget = strTok;
						pProjDepInfo->strProject = pProjName;
						g_lstprojdeps.AddTail(pProjDepInfo);
					}

					state = NEXT_END;
				}
				else {
					return; // error
				}
			}
		}
	}
	return;
};

LPCTSTR CBldPackage::GetProjectWorkspaceInfoText(LPCTSTR pProjName ){
	static CString strOutput;
	strOutput = "";

	CProject *pProject =(CProject *)g_BldSysIFace.GetBuilderFromName(pProjName);
	if(pProject != NULL)	// Do this only if this is a build project
	{
		POSITION pos = g_lstprojdeps.GetHeadPosition();
		while (pos != NULL)		
		{
			CProjDepInfo * pProjDepInfo = (CProjDepInfo *)g_lstprojdeps.GetNext(pos);
			if( pProjDepInfo->strProject == pProjName ){
				strOutput += "Begin Project Dependency\n";
				strOutput += (_T("Project_Dep_Name ") + pProjDepInfo->strTarget + _T('\n'));
				strOutput += "End Project Dependency\n";
			}
		}


#if 0
		// for each project dependency
		CObList ol;
		int fo = CProjItem::flt_OnlyTargRefs;
		pProject->FlattenSubtree(ol, fo);
 		for (POSITION pos = ol.GetHeadPosition(); pos != NULL; )
		{
			CProjectDependency * pProjectDep = (CProjectDependency *) ol.GetNext(pos);
			if (pProjectDep->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
			{
				strOutput += "Begin Project Dependency\n";
			
				CString strName;
				// construct special 'PROP <name> <value>' line
				pProjectDep->GetStrProp(P_ProjItemName,strName);
				strOutput += (_T("Project_Dep_Name ") + strName + _T('\n'));
				strOutput += "End Project Dependency\n";
			}
		}
#endif


	}
	return (LPCTSTR)strOutput;
};

BOOL CBldPackage::IsVisibleProjectCommand(UINT nID, GUID *pGuids, UINT cGuidCount)
{
	// Breakpoints dlg box pertain to C++ or Java only at the moment...
	BOOL bShowIt = FALSE;
	if (cGuidCount > 0)
	{
		UINT idx;
		switch (nID)
		{
			case IDM_PROJECT_CLEAN_ACTIVE:
			case IDM_PROJECT_SCAN_ALL:
			case IDM_PROJITEM_BATCH_BUILD:
			case IDM_PROJECT_EXECUTE:
			case IDM_PROJECT_DEFAULT_PROJ:
			case IDM_PROJECT_CONFIGURATIONS:
			case IDM_PROJECT_SUBPROJECTS:
			case IDM_PROJECT_EXPORT:
			case IDM_RUN_PROFILE:
				for (idx = 0; idx < cGuidCount && !bShowIt; idx++)
					bShowIt = (pGuids[idx] == IID_IBSCxxProject || pGuids[idx] == IID_IBSJavaProject || pGuids[idx]==IID_IBSExeProject);
				break;

			default:
				ASSERT(FALSE);
				break;
		}
	}
	return (bShowIt);
}

////////////////////////////////////////////////////////////
// CBldPackage::OnExit

void CBldPackage::OnExit()
{
	INT			nType, nToolset, nMaxToolsets;
	TCHAR *		szDirName;
	CString		strItemName;
	CString		str;
	CString		strKeyName;

	//
	// Empty g_FileChangeQ now so that it is not emptied by its destructor.
	// review(tomse): The destructor of g_FileChangeQ, ~CSafeAttribList() calls->
	// ReleaseFRHRef() calls-> SafeDelete() uses-> CritSectionT cs(g_sectionFileMap).
	// This will cause a problem if g_sectionFileMap is already destroyed.
	// THIS PROBLEM WILL OCCUR FOR ANY STATIC OBJECT WHOSE DESTRUCTOR USES
	// A CSection.
	//
	FileChangeDeQ();

	// REVIEW: for 5.0 g_pProjSysIFace should become m_pProjSysIFace for version 5.0
	// it is only being done this way for 4.1 so that Nothing changes in CBldSysIFace's
	// interface. The inlines to access this varible are in project.h and should be
	// moved to be members of BldSysIFace
	if (g_pProjWksIFace != NULL)
	{
		g_pProjWksIFace->Release();
		g_pProjWksIFace = NULL;
	}

	if (g_pProjWksWinIFace != NULL)
	{
		g_pProjWksWinIFace->Release();
		g_pProjWksWinIFace = NULL;
	}

	ASSERT(m_pType == NULL);	// m_pType should get freed by ReleaseProjectTypeInfo()

	// get the Exe directory
	TCHAR * pch = new TCHAR[MAX_PATH];
	(void) GetModuleFileName(theApp.m_hInstance, pch, MAX_PATH);

	CPath pathExe; CDir dirExe;
	if (pathExe.Create(pch))
		dirExe.CreateFromPath((const CPath)pathExe);

	delete [] pch;

	// Write out the Install directories entry
	WriteRegString(szDirectories, szInstallDirs, dirExe);

	WriteRegInt(szBuildSystem, szAlwaysExportDeps, g_bAlwaysExportDeps);
	WriteRegInt(szBuildSystem, szAlwaysExport, g_bAlwaysExportMakefile);
	WriteRegInt(szBuildSystem, szSharability,  g_nSharability);

	// Write out usage of project logging.  ( "*.plg" )
	WriteRegInt(szBuildSystem, szBuildLog, g_bWriteBuildLog);
	WriteRegInt(szBuildSystem, szHtmlLog, g_bHTMLLog);
	WriteRegInt(szBuildSystem, szSlowLinks, g_bSlowLinks);
	
	if( !theApp.m_bInvokedCommandLine )
	{
	//  FIX DF: BUG VC6:SP3
	//	WriteRegInt(szBuildSystem, szUseEnvironment, g_bUseEnvironment);
		WriteRegInt(szBuildSystem, szEnableUpdateDeps, g_bEnableUpdDeps);
	}

	// Save information to the registry.
	CDirMgr * pDirMgr = GetDirMgr();
	if( !g_bUseEnvironment && !theApp.m_bInvokedCommandLine ){

		nMaxToolsets = pDirMgr->GetNumberOfToolsets();
		for (nType=0 ; nType<C_DIRLIST_TYPES ; ++nType)
		{
			for (nToolset=0 ; nToolset<nMaxToolsets ; ++nToolset)
			{
				pDirMgr->GetDirListString(str, (DIRLIST_TYPE)nType, nToolset);
	
				switch ((DIRLIST_TYPE)nType)
				{
					case DIRLIST_PATH:	szDirName = szPathItems;	break;
					case DIRLIST_INC:	szDirName = szIncItems;		break;
					case DIRLIST_LIB:	szDirName = szLibItems;		break;
					case DIRLIST_SOURCE:szDirName = szSrcItems;		break;
					default:			ASSERT(FALSE);				break;
				}
	
				CPlatform * pPlatform;
				g_prjcompmgr.LookupPlatformByOfficialName(pDirMgr->GetToolsetName(nToolset), pPlatform);
	
				if (pPlatform->GetUniqueId() == java)
				{
					if (nType == DIRLIST_INC)
						szDirName = szClassItems;
					else if (nType == DIRLIST_LIB)
						continue;
				}
				
				strItemName = szDirName;
				
				g_prjcompmgr.LookupPlatformByOfficialName(pDirMgr->GetToolsetName(nToolset), pPlatform);
	
				strKeyName = pPlatform->GetCompRegKeyName(FALSE);
				strKeyName += szDirectories;
				WriteRegString(strKeyName, (TCHAR *)(const TCHAR *)strItemName, str);
			}
		}
	}
	
 	// terminate project component manager
	g_prjcompmgr.Term();
		
	// Do we still have the build node if so then get rid of it
	CBuildNode * pNode = GetBuildNode();
	if (pNode)
		delete pNode;

    if (m_pDefNode)
        delete m_pDefNode;

	//
	// Release source utility interface.
	//
	if (m_pSrcUtil != NULL)
	{
		m_pSrcUtil->Release();
		m_pSrcUtil = NULL;
	}

	//
	// Release source control interfaces.
	//

	if (g_pSccManager != NULL)
	{
		g_pSccManager->Release();
		g_pSccManager = NULL;
	}

	g_VPROJIdeInterface.Terminate();
}

/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
// This is TRUE if the /Y1 switch was thrown. If TRUE, the build view
// shows the complete project tree including all targets at once,
// for debugging purposes
BOOL	g_bProjDebugView = FALSE;
#endif

#ifdef _BTLOG
// This is FALSE if the /Y2 switch was thrown. If TRUE, for successful
// builds, information such as build time, machine name, etc..., is logged
// to a central server.
BOOL	g_bBuildLogging = FALSE;
#endif

// Enable spawn/build time results to output window in _ship builds
// -Y3 turns on in retail build, is on by default in debug builds 
#ifdef _DEBUG
BOOL	g_bSpawnTime = TRUE;
#else
BOOL	g_bSpawnTime = FALSE;
#endif

#ifndef _SHIP
BOOL	g_bUseReleaseVersion = FALSE;
#endif


// This is the projects performance log
#ifdef _INSTRAPI
LOG	g_ProjectPerfLog;
#endif

void ProcessCommandLine(CStringList * pArgs)
{
	TCHAR		ch;
	POSITION	pos, posCur;
	CString		strArg, strT;
	CString		strOut;

	for (pos=pArgs->GetHeadPosition() ; (posCur=pos)!=NULL ; )
	{
		strArg = pArgs->GetNext(pos);

		if (strArg.GetLength() < 2)
			continue;

		ch = strArg[1];
		strT = strArg.Mid(1);
		CharUpperBuff(&ch, 1);

		switch (ch)
		{
			case _T('M'):
				if (strT.CompareNoCase(_T("make"))==0)
				{
					g_bCommandLineBuild = TRUE; // note for later
					ASSERT(theApp.m_bInvokedCommandLine);
					pArgs->RemoveAt(posCur);
					while (pos != NULL)
					{
						CString strCfg = pArgs->GetAt(pos);
						if (strCfg[0] != _T('-') && strCfg[0] != _T('/'))
						{
							g_strlCfgsToBuild.AddTail(strCfg);
							posCur = pos;
							pArgs->GetNext(pos);
							pArgs->RemoveAt(posCur);
						}
						else
						{
							// no more targets
							break;
						}
					}
				}
				break;

			case _T('C'):
				if (strT.CompareNoCase(_T("clean"))==0)
				{
					// don't override Rebuild with Clean
					if (g_nTOB == TOB_Build)
					{
						g_nTOB = TOB_Clean;
					}
					pArgs->RemoveAt(posCur);
				}
				break;

#if 0
			case _T('H'):
				if (strT.CompareNoCase(_T("help"))!=0)
					break;
#endif
			case _T('?'):
				strOut.LoadString(IDS_BLD_HLP_USEENV);
				theApp.WriteLog(strOut);

				strOut.LoadString(IDS_BLD_HLP_MAKE0);
				theApp.WriteLog(strOut);

				strOut.LoadString(IDS_BLD_HLP_MAKE1);
				theApp.WriteLog(strOut);

				strOut.LoadString(IDS_BLD_HLP_MAKE2);
				theApp.WriteLog(strOut);

				strOut.LoadString(IDS_BLD_HLP_CLEAN);
				theApp.WriteLog(strOut);

				strOut.LoadString(IDS_BLD_HLP_REBUILD);
				theApp.WriteLog(strOut);

				strOut.LoadString(IDS_BLD_HLP_NORECURSE);
				theApp.WriteLog(strOut);

				// UNDONE: force exit now
				break;

			case _T('R'):
				if (strT.CompareNoCase(_T("rebuild"))==0)
				{
					g_nTOB = TOB_ReBuild;
					pArgs->RemoveAt(posCur);
				}
				break;

			case _T('U'):
				if (strT.CompareNoCase(_T("useenv"))==0)
				{
					// ignore tools.options.dirs
					g_bUseEnvironment = TRUE;
					pArgs->RemoveAt(posCur);
				}
				break;

			case _T('Y'):
				if (strT.CompareNoCase(_T("Y3"))==0)
				{
					g_bSpawnTime = TRUE;
					pArgs->RemoveAt(posCur);
				}
#ifndef _SHIP
				else if (strT.CompareNoCase(_T("Y0"))==0)
				{
					g_bUseReleaseVersion = TRUE;
					pArgs->RemoveAt(posCur);
				}
#ifdef _DEBUG
				else if (strT.CompareNoCase(_T("Y1"))==0)
				{
					g_bProjDebugView = TRUE;
					pArgs->RemoveAt(posCur);
				}
#endif //_DEBUG
#ifdef _BTLOG
				else if (strT.CompareNoCase(_T("Y2"))==0)
				{
					g_bBuildLogging = TRUE;
					pArgs->RemoveAt(posCur);
				}
#endif //_BTLOG
#endif //_SHIP
				break;

			case _T('N'):
				if (strT.CompareNoCase(_T("NORECURSE"))==0)
				{
					g_bRecurse = FALSE; // TRUE by default
					pArgs->RemoveAt(posCur);
				}
#ifdef _BTLOG
				else if (strT.CompareNoCase(_T("NOLOG"))==0)
				{
					g_bBuildLogging = FALSE;
				}
#endif // _BTLOG
				break;
			default:
				break;
		}
	}
}

// References to our local heaps that
// are created in ::OnInit()
#include "pfilereg.h"
#include "depgraph.h"

#ifdef VB_MAKEFILES
BOOL g_bVBInstalled = FALSE;
SRankedStringID arsiOpenProjFilters[];
SRankedStringID arsiInsertProjFilters[];
SRankedStringID arsiAddFileFilters[];
#endif

BOOL CBldPackage::OnInit()
{
	CPackage::OnInit();

	// StartCAP();
#ifdef USE_LOCAL_BUILD_HEAP
	// create our heaps now
	VERIFY(CGrNode::g_heapNodes.Create());
	VERIFY(CFileRegEntry::g_heapEntries.Create());
#endif
	
	// initialise VPROJInterface (so that we can access
	// vcpp functions
	if ((!theApp.m_bInvokedCommandLine) && (!g_VPROJIdeInterface.Initialize()))
		return FALSE ;

	if (g_pProjWksIFace == NULL)
		VERIFY(SUCCEEDED(theApp.FindInterface(IID_IProjectWorkspace,
				(LPVOID FAR *)&g_pProjWksIFace)));

	if (g_pProjWksWinIFace == NULL)
		VERIFY(SUCCEEDED(theApp.FindInterface(IID_IProjectWorkspaceWindow,
				(LPVOID FAR *)&g_pProjWksWinIFace)));

	if (NULL==m_pSrcUtil && !SUCCEEDED(theApp.FindInterface(IID_ISrcUtil, (LPVOID FAR *)&m_pSrcUtil)))
	{
		m_pSrcUtil = NULL;
	}

	// initialise our project components
	// initialise our project components
	if( !g_prjcompmgr.IsInit() ){
		if (!g_prjcompmgr.FInit())
			return FALSE;
	}
	g_prjcompmgr.IsInit(TRUE);

	if (!(theApp.m_bInvokedCommandLine && theApp.m_bRunInvisibly))
	{
		// Initialise our Mecr copy menu option - should only be visible
		// if we have the macintosh add on installed
		CPlatform * pPlatform;
#ifdef _MAC_ENABLED
		BOOL bToolMecrVisible = FALSE;
		g_prjcompmgr.InitPlatformEnum();
		while (g_prjcompmgr.NextPlatform(pPlatform))
			if ((pPlatform->GetUniqueId() == mac68k ||
			     pPlatform->GetUniqueId() == macppc)
			    && pPlatform->IsSupported())
				bToolMecrVisible = TRUE;

		SetVisibleMenuItem(IDM_PROJECT_TOOL_MECR, bToolMecrVisible);
#endif
		// Start out by disabling these commands; enable them later if appropriate
		for(int iTool=IDM_PROJECT_TOOL_CMD_FIRST; iTool<=IDM_PROJECT_TOOL_CMD_LAST; ++iTool)
		{
			SetVisibleMenuItem(iTool, FALSE);
		}

		// REVIEW: only show UpdateAllDeps if Fortran package is loaded or if
		//         specified by Setup (allows for other packages to override)
		g_bEnableUpdDeps = (IsPackageLoaded(PACKAGE_LANGFOR) || GetRegInt(szBuildSystem, szEnableUpdateDeps, FALSE));
		ShowCommandUI(IDM_PROJECT_SCAN_ALL, g_bEnableUpdDeps);
	}

	// Initialise the Directory Manager
	CDirMgr * pDirMgr = GetDirMgr();

	const CStringList * pStrList = g_prjcompmgr.GetListOfPlatforms();

	// Process the command line
	ProcessCommandLine(&theApp.m_argList);

	// Initialise toolsets with the "Official" target-platform names that
	// we support
	POSITION pos = pStrList->GetHeadPosition();
	while (pos != NULL)
	{
		CString strPlatform;

		strPlatform = pStrList->GetNext(pos);
		pDirMgr->AddToolset(strPlatform);
	}

	// read information from the registry
	INT nType, nToolset;
	TCHAR * szDirName, * szEnvVarName;
	CString strItemName;
	CString strKeyName;


	// our buffer size used for holding environment string values
	DWORD cchEnvVarVal = 512;

	// get the Exe directory
	TCHAR * pch = new TCHAR[MAX_PATH];
	(void) GetModuleFileName(theApp.m_hInstance, pch, MAX_PATH);

	CPath pathExe; CDir dirExe;
	if (pathExe.Create(pch))
		dirExe.CreateFromPath((const CPath)pathExe);

	delete [] pch;

	CString str;
	str.GetBuffer(512);
	str.ReleaseBuffer();

	INT nMaxToolsets;
	nMaxToolsets = pDirMgr->GetNumberOfToolsets();
	
	// if not already set by command line, get from registry
	if( !g_bUseEnvironment )
		g_bUseEnvironment = GetRegInt(szBuildSystem, szUseEnvironment, FALSE);

	char szNotFound[] = "<Bogus>";

	for (nType=0 ; nType<C_DIRLIST_TYPES ; ++nType)
	{
		switch ((DIRLIST_TYPE)nType)
		{
			case DIRLIST_PATH:
				szDirName = szPathItems;
				szEnvVarName = szPath;
				break;

			case DIRLIST_INC:
				szDirName = szIncItems;
				szEnvVarName = szInclude;
				break;

			case DIRLIST_LIB:
				szDirName = szLibItems;
				szEnvVarName = szLib;
				break;

			case DIRLIST_SOURCE:
				szDirName = szSrcItems;
				szEnvVarName = szSrc;
				break;

			default:
				ASSERT(FALSE);
				break;
		}
		for (nToolset=0 ; nToolset<nMaxToolsets ; ++nToolset)
		{

			CString strPlatform = pDirMgr->GetToolsetName (nToolset) ;

			CPlatform * pPlatform;
			g_prjcompmgr.LookupPlatformByOfficialName(pDirMgr->GetToolsetName(nToolset), pPlatform);
			if( !g_bUseEnvironment )
			{

				strKeyName = pPlatform->GetCompRegKeyName(FALSE);
				strKeyName += szDirectories;

				BOOL    fJava = (pPlatform->GetUniqueId() == java);
				if (fJava)
				{
					if (nType == DIRLIST_INC)
					{
						szDirName = szClassItems;
						szEnvVarName = szClasspath;
					}
					else if (nType == DIRLIST_LIB)
						continue;
				}
				strItemName = szDirName;
	
				g_prjcompmgr.LookupPlatformByOfficialName(pDirMgr->GetToolsetName(nToolset), pPlatform);
				strKeyName = pPlatform->GetCompRegKeyName(FALSE);
				strKeyName += szDirectories;
	
				CString strVCDir, strVJDir, strBaseKey;
				strBaseKey = _T("Software\\Microsoft\\");
				strBaseKey += theApp.GetExeString(DefaultRegKeyName);
				strBaseKey += _T("\\Products\\");
	
				str = GetRegString(strKeyName, (TCHAR *)(const TCHAR *)strItemName, szNotFound);
				if (str == szNotFound )
				{	
					TCHAR szBuf [MAX_PATH];
					HKEY hKey;
					if (fJava)
					{
						// We need to figure out where the product is installed.  This case looks for Java, the 
						// else will find VC.
						strBaseKey += _T("Microsoft Visual J++");
	
						if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, strBaseKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
						{
							DWORD dwType, cbData = MAX_PATH;
							if (RegQueryValueEx (hKey, _T ("ProductDir"), NULL,
									&dwType, (LPBYTE) szBuf, &cbData) == ERROR_SUCCESS)
							{
								ASSERT (dwType == REG_SZ);
								szBuf [cbData] = 0;
								strVJDir = szBuf;
							}
							RegCloseKey (hKey);
						}	
						else {
							strVJDir = dirExe + _T("\\..\\..\\VJ");
						}
					}
					else
					{
						strBaseKey += _T("Microsoft Visual C++");
						// strVCDir = "\\\\HKEYLOCALMACHINE\\microsoft\\devstudio\\products\\Microsoft Visual C++\\ProductDir"
						if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, strBaseKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
						{
							DWORD dwType, cbData = MAX_PATH;
							if (RegQueryValueEx (hKey, _T ("ProductDir"), NULL,
									&dwType, (LPBYTE) szBuf, &cbData) == ERROR_SUCCESS)
							{
								ASSERT (dwType == REG_SZ);
								szBuf [cbData] = 0;
								strVCDir = szBuf;
							}
							RegCloseKey (hKey);
						}	
						else {
							strVCDir = dirExe + _T("\\..\\..\\..\\VC98");
						}
					}
				}
	
				// if the registry string is not found get it from our
				// registry if we are the first toolset
				if (str == szNotFound )
				{
					if (szEnvVarName == NULL)
					{
						str = dirExe;
					}
					else
					{
						// construct our prepend '\..\szItemName' etc.
						CString strDefaultPlatform;	// platform specific path prefix
						CString strPath;
	
						str.Empty();
	
						switch ((DIRLIST_TYPE)nType)
						{
							case DIRLIST_PATH:
								strPath = pPlatform->GetToolInfo()->strPath;
								break;
	
							case DIRLIST_INC:
								strPath = pPlatform->GetToolInfo()->strIncludePath;
								break;
	
							case DIRLIST_LIB:
								strPath = pPlatform->GetToolInfo()->strLibPath;
								break;
	
							case DIRLIST_SOURCE:
								strPath = pPlatform->GetToolInfo()->strSourcePath;
								break;
	
							default:
								ASSERT(FALSE);
								break;
						}
	
						if (!strPath.IsEmpty() && !pPlatform->IsPrimaryPlatform())
						{				
							TCHAR * pStart = strPath.GetBuffer(strPath.GetLength());
							while (*pStart != _T('\0'))
							{
								TCHAR * pEnd =_tcschr(pStart, _T(';'));
								*pEnd = _T('\0');
								str += dirExe;
								if (*pStart == _T('\\'))
									str += _T("\\..") + (CString)pStart + _T(";");
								else
									str += _T("\\..\\") + (CString)pStart + _T(";");
								*pEnd = _T(';');
								pStart = _tcsinc(pEnd);
							}
							strPath.ReleaseBuffer();
						}
						else
							str.Empty() ;
	
						// we need to get the default platform specific path :
						BOOL fCPackageLoaded = IsPackageLoaded(PACKAGE_LANGCPP);
						switch ((DIRLIST_TYPE)nType)
						{
							case DIRLIST_PATH:
								str += dirExe; str += ';';
								if (!fJava)
								{
									str += strVCDir + _T("\\BIN;");
									str += dirExe + _T("\\..\\..\\TOOLS;");
									if (theApp.m_bWin95)
									{
										str += dirExe + _T("\\..\\..\\TOOLS\\WIN95;");
										TCHAR szSysDir[1024];
										GetSystemDirectory(szSysDir,1024);
										str += szSysDir;
										str += ";";
									}
									else
									{
										str += dirExe + _T("\\..\\..\\TOOLS\\WINNT;");
									}
								}
								else
								{
									str += strVJDir + _T("\\BIN;");
								}
								break;
	
							case DIRLIST_INC:
								if (!fJava)
								{
									str += strVCDir + "\\INCLUDE;";
									if (fCPackageLoaded)
									{
										str += strVCDir + "\\MFC\\INCLUDE;";
										str += strVCDir + "\\ATL\\INCLUDE;";
									}
								}
								else
								{
									// make the default classpath include trustlib
									TCHAR szBuf [MAX_PATH];
	
									// Get the location of trustlib from HKLM\Software\Microsoft\Java VM
									HKEY hKey;
									if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, _T ("Software\\Microsoft\\Java VM"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
									{
										DWORD dwType, cbData = MAX_PATH;
										if (RegQueryValueEx (hKey, _T ("TrustedLibsDirectory"), NULL,
												&dwType, (LPBYTE) szBuf, &cbData) == ERROR_SUCCESS)
										{
											ASSERT (dwType == REG_SZ);
											szBuf [cbData] = 0;
											str += szBuf;
											str += ";";
										}
										RegCloseKey (hKey);
									}	
								}
								break;
	
							case DIRLIST_LIB:
								str += strVCDir + "\\LIB;";
								if (fCPackageLoaded)
								{
									str += strVCDir + "\\MFC\\LIB;";
								}
								break;
	
							case DIRLIST_SOURCE:
								if (fCPackageLoaded && !fJava)
								{
									str += strVCDir + "\\MFC\\SRC;";
									str += strVCDir + "\\MFC\\INCLUDE;";
									str += strVCDir + "\\ATL\\INCLUDE;";
									str += strVCDir + "\\CRT\\SRC;";
								}
								else if (fJava)
								{
									CHAR    szBuf[MAX_PATH];
									GetWindowsDirectory (szBuf, sizeof (szBuf));
									strcat (szBuf, "\\Java");
									CreateDirectory (szBuf, NULL);
									strcat (szBuf, "\\Classes");
									CreateDirectory (szBuf, NULL);
									str += szBuf;
									str += ";";
								}
	
								break;
	
							default:
								ASSERT(FALSE);
								break;
						}

						CString strExtra;
	
						TryAgain:
			
						// ensure the string buffer is of the correct size
						(void) strExtra.GetBuffer(cchEnvVarVal);
						
						DWORD cch = GetEnvironmentVariable(szEnvVarName, (TCHAR *)(const TCHAR *)strExtra, cchEnvVarVal);
						if (cch == 0)
						{
							strExtra = "";	// not found in environment
						}
						else if (cch > cchEnvVarVal)
						{
							cchEnvVarVal = cch;	// buffer not large enough
							goto TryAgain;
						}
	
						strExtra.ReleaseBuffer();
	
						if (theApp.m_bRestoreKey)
							str = strExtra ; // ignore all of the setup we have
						else
							str += strExtra;	// append the %envvar%
					}
				}
			}
			else {
				CString strExtra;

				Bigger:
	
				// ensure the string buffer is of the correct size
				(void) strExtra.GetBuffer(cchEnvVarVal);
				
				DWORD cch = GetEnvironmentVariable(szEnvVarName, (TCHAR *)(const TCHAR *)strExtra, cchEnvVarVal);
				if (cch == 0)
				{
					strExtra = "";	// not found in environment
				}
				else if (cch > cchEnvVarVal)
				{
					cchEnvVarVal = cch;	// buffer not large enough
					goto Bigger;
				}

				strExtra.ReleaseBuffer();

				str = strExtra ; // ignore all of the setup we have
			}

			// don't add these directories if they don't exist on disk
			pDirMgr->SetDirListFromString((DIRLIST_TYPE)nType, nToolset, str, FALSE);
		}
	}

#ifdef VB_MAKEFILES
	HKEY hKey;
	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, _T ("Software\\Microsoft\\Visual Basic"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		RegCloseKey (hKey);
		g_bVBInstalled = TRUE;
	} else {
		arsiOpenProjFilters[4].ids=0;
		arsiInsertProjFilters[1].ids=0;
		arsiAddFileFilters[2].ids=0;
		g_bVBInstalled = FALSE;
	}
#endif

	// create the build node
	CBuildNode * pNode = new CBuildNode;

#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "msvcprjd.dll", "InitPackage()", letypeEnd, 0);
#endif

	// Make sure sound events are in registry
	RegisterDevSoundEvent(IDS_SOUND_BUILD_ERROR, IDS_SOUND_BUILD_ERROR2);
	RegisterDevSoundEvent(IDS_SOUND_BUILD_WARNING, IDS_SOUND_BUILD_WARNING2);
	RegisterDevSoundEvent(IDS_SOUND_BUILD_COMPLETE, IDS_SOUND_BUILD_COMPLETE2);

	g_bAlwaysExportMakefile = GetRegInt(szBuildSystem, szAlwaysExport, FALSE);
	g_bAlwaysExportDeps = GetRegInt(szBuildSystem, szAlwaysExportDeps, FALSE);
	g_nNextIncompatibleVersion = GetRegInt(szBuildSystem, szNextIncompatibleVersion, (nVersionMajor + 1) * 100);

	g_nSharability = GetRegInt(szBuildSystem, szSharability, SHARE_NORM);
	g_bEnableAllPlatforms = GetRegInt(szBuildSystem, szFullPlatforms, FALSE);
	
	g_bWriteBuildLog = GetRegInt(szBuildSystem, szBuildLog, TRUE);
	g_bHTMLLog = GetRegInt(szBuildSystem, szHtmlLog, TRUE);
	g_bSlowLinks = GetRegInt(szBuildSystem, szSlowLinks, FALSE);

	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&g_pSccManager)));

	BOOL bIsSccInstalled = (g_pSccManager->IsSccInstalled() == S_OK);
	if (bIsSccInstalled)
	{
		// implicit: VERIFY(SUCCEEDED(g_pSccManager->RegisterProjectFilter(_T(WORKSPACE_EXT))));
		VERIFY(SUCCEEDED(g_pSccManager->RegisterProjectFilter(_T(BUILDER_EXT))));
	}

	// notify other packages that our initialization is complete
	theApp.NotifyPackages(PN_BLD_INIT_COMPLETE);
	// StopCAP();

	return TRUE;
}

// file change notification de-Q'ing (pfilereg.h)
extern void FileChangeDeQ(FileRegHandle);

//
// Enable test code for initializing dependencies in OnIdle.
//
// #define ONIDLE_INITDEPS

#ifdef ONIDLE_INITDEPS
// UpdateDepInfo during OnIdle
static BOOL UpdateOneDep(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren)
{
	// get our actions that contribute to target
	if (0 >= pgrn->CChildren())
		return TRUE;

	CActionSlob * pAction = (CActionSlob *)pgrn->Edge(0);

	if (pAction == (CActionSlob *)NULL)
		return TRUE;

	if ( !pAction->IsDepInfoInitialized() )
	{
		return !pAction->UpdateDepInfo();
	}

	return TRUE;
}
#endif

BOOL CBldPackage::OnIdle(long lCount)
{
	static BOOL bToggle = TRUE;

#ifdef ONIDLE_INITDEPS
	//
	// Initialize dependencies to speed up first build.
	//
	if (NULL!=g_pActiveProject)
	{
		DWORD dw;
		g_buildengine.GetDepGraph(g_pActiveProject->GetActiveConfig())->PerformOperation(UpdateOneDep, dw);
	}
#endif

	if( bToggle ) {
		if( !FileChangeDeQ() )
			FileItemDepUpdateDeQ();
	} else {
		if( !FileItemDepUpdateDeQ() )
			FileChangeDeQ();
	}
	
	bToggle = !bToggle;

	// Allow retry of file watches which have failed.
	CFileRegFile::AllowRetryFailedWatches(FALSE);

	return FALSE;
}

HGLOBAL CBldPackage::GetDockInfo()
{
	// When adding enties to rgdi[] make sure to add corresponding entries in
	// rgCaptionID[].
	static UINT rgCaptionID[] = {
        IDS_BUILD_TOOLBAR,
        IDS_BUILD_MINIBAR
	};

	// When adding entries to rgdi[] make sure to add
	// corresponding enties in rgCaptionID[]
	static DOCKINIT BASED_CODE rgdi[] =
	{
        {IDTB_PROJECT,
         PACKAGE_VPROJ,
         PACKET_NIL,
         "",	// "Project"
         INIT_CANHORZ | INIT_CANVERT | INIT_POSITION,
         dtToolbar,
         dpTop,
         dpNil,
         {0, 0, 0, 0}
        },
        {IDTB_PROJECT_MINI,
         PACKAGE_VPROJ,
         PACKET_NIL,
         "",	// "Project"
         INIT_CANHORZ | INIT_CANVERT | INIT_POSITION | INIT_VISIBLE,
         dtToolbar,
         dpTop,
         dpNil,
         {0, 0, 0, 0}
        }
	};

	CString str;
	for (int i = 0; i < sizeof(rgCaptionID) / sizeof(UINT); i++)
	{
		VERIFY(str.LoadString(rgCaptionID[i]));
		ASSERT(str.GetLength() <= _MAX_DOCK_CAPTION);
		lstrcpy(rgdi[i].szCaption, str);
	}

	return DkInfoData(sizeof(rgdi) / sizeof(DOCKINIT), rgdi);
}

HGLOBAL CBldPackage::GetToolbarData(UINT nID)
{
	static UINT BASED_CODE proj_buttons[] =
	{
		ID_PROJECT_COMBO,
		ID_CONFIG_COMBO,
		0,
		IDM_PROJITEM_COMPILE,
		IDM_PROJITEM_BUILD,
		IDM_PROJECT_STOP_BUILD,
		0,
		IDM_PROJECT_EXECUTE,
	    IDM_RUN_GO,
		IDM_RUN_TOGGLEBREAK,
	};

	static UINT BASED_CODE proj_buttons_mini[] =
	{
		IDM_PROJITEM_COMPILE,
		IDM_PROJITEM_BUILD,
		IDM_PROJECT_STOP_BUILD,
		IDM_PROJECT_EXECUTE,
	    IDM_RUN_GO,
		IDM_RUN_TOGGLEBREAK,
	};

	TOOLBARINIT tbi;
	tbi.nIDWnd = nID;

	switch(nID)
	{
		case IDTB_PROJECT:
			tbi.nIDCount = sizeof(proj_buttons)/sizeof(UINT);
			return DkToolbarData(&tbi, (UINT *)proj_buttons);

		case IDTB_PROJECT_MINI:
			tbi.nIDCount = sizeof(proj_buttons_mini)/sizeof(UINT);
			return DkToolbarData(&tbi, (UINT *)proj_buttons_mini);
	}

	return CPackage::GetToolbarData(nID);
}

void CBldPackage::GetDefProvidedNodes(CPtrList & NodeList, BOOL bWorkspaceInit)
{
	if (bWorkspaceInit && !CProject::GetProjectList()->IsEmpty() )
	{
        if (m_pDefNode)
            delete m_pDefNode;
		m_pDefNode = new CDefBuildNode;
		NodeList.AddTail(m_pDefNode);
	}
}

HWND CBldPackage::GetDockableWindow(UINT nID, HGLOBAL hglob)
{
	if (nID == ID_TARGET_COMBO)
	{
		// Create the target combo toolbar window thing
		CRect rectCombo(-232, -120, 0, 0);
		if (UseWin4Look())
			rectCombo.left += 4;

		CTargetCombo * pTargetCombo = new CTargetCombo;
		if (!pTargetCombo->Create(WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST |
			CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS, rectCombo, AfxGetMainWnd(),
			ID_TARGET_COMBO))
 		{
			TRACE("Failed to create target combo.\n");
			delete pTargetCombo;
			return NULL;
		}

		return pTargetCombo->m_hWnd;
	}
	if (nID == ID_CONFIG_COMBO)
	{
		// Create the target combo toolbar window thing
		CRect rectCombo(-232, -120, 0, 0);
		if (UseWin4Look())
			rectCombo.left += 4;

		CConfigCombo * pConfigCombo = new CConfigCombo;
		if (!pConfigCombo->Create(WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST |
			CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS, rectCombo, AfxGetMainWnd(),
			ID_CONFIG_COMBO))
 		{
			TRACE("Failed to create target combo.\n");
			delete pConfigCombo;
			return NULL;
		}

		return pConfigCombo->m_hWnd;
	}

	return CPackage::GetDockableWindow(nID, hglob);
}

// The build system exposes only one top-level object: The Projects collection
LPDISPATCH CBldPackage::GetPackageExtension(LPCTSTR szExtensionName)
{
	return NULL;
}


int CBldPackage::GetAssociatedFiles(CStringArray & saFiles, BOOL bSelected /* = FALSE */)
{
	ASSERT(0);
	return 0;
#if 0
	ASSERT(bSelected==FALSE); // NYI

	CProject * pProject = g_pActiveProject;
	if (pProject==NULL)
		return 0;

	LPSOURCECONTROLSTATUS pInterface;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControlStatus, (LPVOID FAR*)&pInterface)));
	BOOL bIsSccInstalled = (pInterface->IsSccInstalled() == S_OK);
	pInterface->Release();
	LPSOURCECONTROL pSccManager;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));
	if (bIsSccInstalled && (pSccManager->IsAddingProject() == S_OK))
	{
		// we're in the process of putting the .mak file under scc
		// so make sure the deps have been scanned so we don't get
		// really strange behavior later
		UpdateBeforeWriteOutMakeFile();
	}
	pSccManager->Release();

	CObList filelist;
	pProject->FlattenSubtree(filelist, CProjItem::flt_Normal | CProjItem::flt_ExcludeGroups /* | cProjItem::flt_RespectTargetExclude */);
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
	
	return count;
#endif
}

HRESULT CBldPackage::GetService(REFGUID guidService, REFIID riid, void **ppvObj)
{
	// the Service ID for the Pkg Project Provider is the same
	// as the Interface ID for IPkgProjectProvider.
	if(guidService == IID_IPkgProjectProvider)
	{
		return ExternalQueryInterface(&riid, ppvObj);
	}
	return E_NOINTERFACE;
}

/////////////////////////////////////////////////////////////////////////////
//
// our components
#include "toolrc.h"
#include "toolbsc.h"
#include "toolmtl.h"
#include "toollib.h"
#include "toolcplr.h"
#include "toollink.h"

static int nOffset = 0;

BOOL CBldPackage::DoBldSysCompRegister(CProjComponentMgr * pcompmgr, DWORD blc_type, DWORD blc_id)
{
	// what is the hook?
	switch (blc_type)
	{
		case BLC_Tool:
			// only for platform 0 (generic platform)
			if (blc_id != 0)	break;

			// our 'generic' tools
			pcompmgr->RegisterBldSysComp(new CRCCompilerTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_RcCompiler));
			pcompmgr->RegisterBldSysComp(new CRCCompilerNTTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_RcCompilerNT));
			pcompmgr->RegisterBldSysComp(new CMkTypLibTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_MkTypLib));
			pcompmgr->RegisterBldSysComp(new CLibTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_Lib));
			pcompmgr->RegisterBldSysComp(new CBscmakeTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_BscMake));
			pcompmgr->RegisterBldSysComp(new CCCompilerTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_Compiler));
			pcompmgr->RegisterBldSysComp(new CLinkerTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_Linker));
			pcompmgr->RegisterBldSysComp(new CLinkerNTTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_LinkerNT));
			pcompmgr->RegisterBldSysComp(new CCustomBuildTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_CustomBuild));
			pcompmgr->RegisterBldSysComp(new CSpecialBuildTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_SpecialBuild));
			break;

		case BLC_OptionHdlr:
			// only for platform 0 (generic platform)
			if (blc_id != 0)	break;

			// our 'generic' option handlers
			pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(CompilerCommon));
			pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(LinkerCommon));
			pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(LinkerNT));
			pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(ResCompiler));
			pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(ResCompilerNT));
			pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(BscMake));
			pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(Lib));
			pcompmgr->RegisterBldSysComp(new OPTION_HANDLER(MkTypLib));
			break;

		case BLC_TargetType:
			// do this for all registered platforms
			if (blc_id == 0)	break;

			// ouro 'generic' external target type
			pcompmgr->RegisterBldSysComp(new CProjTypeExternalTarget(blc_id, nOffset));
			nOffset++;
 			break;

		default:
			break;
	}

	// nothing left to register
	return FALSE;
}

// third field in SRankedStringID structure is CLSID of project type
// for which the filter should be ranked first if ranks are identical

SRankedStringID arsiOpenFilters[] =
{
	0,							0,		&CLSID_NULL,	TRUE
};

SRankedStringID arsiAddFileFilters[] =
{
	IDS_FILTER_LIB,				610,	&CLSID_NULL,	TRUE,
	IDS_FILTER_OBJ,				620,	&CLSID_NULL,	TRUE,
#ifdef VB_MAKEFILES
	IDS_FILTER_BAS,				625,	&CLSID_NULL,	TRUE,
#endif
	0,							0,		&CLSID_NULL,	TRUE
};

SRankedStringID arsiFindFilters[] =
{
	0,							0,		&CLSID_NULL,	TRUE
};


SRankedStringID arsiOpenProjFilters[] =
{
	IDS_FILTER_PROJECTS,		200,	&CLSID_NULL,	TRUE,
	IDS_FILTER_MAKEFILES,		200+1,	&CLSID_NULL,	TRUE,
	IDS_FILTER_EXECUTABLES,		500,	&IID_IBSCxxProject,	TRUE,
#ifdef VB_MAKEFILES
	IDS_FILTER_VBASIC,			200+2,	&CLSID_NULL,	TRUE,
#endif
	0,							0,		&CLSID_NULL,	TRUE
};


//
// This is the same as arsiOpenProjFilters except it includes the
// filter for opening crashdump files as well (*.dmp).  This could
// maybe be done programatically, but I don't know how.
//

SRankedStringID arsiOpenProjFiltersWithDumpFile[] =
{
	IDS_FILTER_PROJECTS,		200,	&CLSID_NULL,	TRUE,
	IDS_FILTER_MAKEFILES,		200+1,	&CLSID_NULL,	TRUE,
	IDS_FILTER_EXECUTABLES,		500,	&IID_IBSCxxProject,	TRUE,
	IDS_FILTER_DUMPFILE,		500+1,  &IID_IBSCxxProject, TRUE,
#ifdef VB_MAKEFILES
	IDS_FILTER_VBASIC,			200+2,	&CLSID_NULL,	TRUE,
#endif
	0,							0,		&CLSID_NULL,	TRUE
};

SRankedStringID arsiInsertProjFilters[] =
{
	IDS_FILTER_MAKEFILES,		200+1,	&CLSID_NULL,	TRUE,
#ifdef VB_MAKEFILES
	IDS_FILTER_VBASIC,			200+2,	&CLSID_NULL,	TRUE,
#endif
	0,							0,		&CLSID_NULL,	TRUE
};

void CBldPackage::GetOpenDialogInfo(SRankedStringID** ppFilters,
	SRankedStringID** ppEditors, int nOpenDialog)
{
	ASSERT( ppEditors != NULL );
	ASSERT( ppFilters != NULL );

	switch( nOpenDialog )
	{
		case GODI_FILEOPEN:
			*ppFilters = arsiOpenFilters;
			*ppEditors = NULL;
			return;
		case GODI_ADDFILE:
			*ppFilters = arsiAddFileFilters;
			*ppEditors = NULL;
			return;
		case GODI_FINDINFILES:
			*ppFilters = arsiFindFilters;
			*ppEditors = NULL;
			return;
		case GODI_RESIMPORT:
			*ppFilters = NULL;
			*ppEditors = NULL;
			return;
		case GODI_WORKSPACEOPEN:

			//
			// If crash dump is enabled, put the filter with *.dmp in it.
			//
			
			if (IsCrashDumpEnabled ()) {
				*ppFilters = arsiOpenProjFiltersWithDumpFile;
				*ppEditors = NULL;
			} else {
				*ppFilters = arsiOpenProjFilters;
				*ppEditors = NULL;
			}
			return;
		case GODI_INSERTPROJECT:
			*ppFilters = arsiInsertProjFilters;
			*ppEditors = NULL;
			return;
	}
}

CDocTemplate* CBldPackage::GetTemplateFromEditor(UINT idsEditor,
	const char* szFilename)
{
	return NULL;
}

DWORD CBldPackage::GetIdRange(RANGE_TYPE rt)
{
	switch (rt)
	{
		case MIN_RESOURCE:
			return MIN_BUILD_RESOURCE_ID;

		case MAX_RESOURCE:
			return MAX_BUILD_RESOURCE_ID;
	}

	return CPackage::GetIdRange(rt);
}

extern "C" BOOL WINAPI RawDllMain(HINSTANCE, DWORD dwReason, LPVOID);
extern "C" BOOL (WINAPI* _pRawDllMain)(HINSTANCE, DWORD, LPVOID) = &RawDllMain;

extern "C" BOOL APIENTRY RawDllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!IsShellDefFileVersion(SHELL_DEFFILE_VERSION))
			return(FALSE);
	}
	return(ExtRawDllMain(hInstance, dwReason, lpReserved));
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInstance);

		// NOTE: global/static constructors have already been called!
		// Extension DLL one-time initialization - do not allocate memory here,
		//   use the TRACE or ASSERT macros or call MessageBox
		if (!AfxInitExtensionModule(extensionDLL, hInstance))
			return 0;

		// This adds our DLL to the MFC maintained list of "AFX
		// Extension DLLs" which is used by serialization functions
		// and resource loading...  This will be deleted by MFC in
		// the AfxTermExtensionModule function.
		new CDynLinkLibrary(extensionDLL);

		// rest of init now happens in InitPackage
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		// Terminate the library before destructors are called
		AfxTermExtensionModule(extensionDLL);
	}

	return TRUE;   // ok
}

extern "C" BOOL PASCAL AFX_EXPORT InitPackage(HWND hWndShell)
{
#ifdef _INSTRAPI
	// Open the performance metric log
	g_ProjectPerfLog = LogOpen();

	LogNoteEvent(g_ProjectPerfLog, "msvcprjd.dll", "InitPackage()", letypeBegin, 0);
#endif

	g_pBuildPackage = new CBldPackage;
	VERIFY(theApp.RegisterPackage(g_pBuildPackage));

	return TRUE;
}

extern "C" void PASCAL AFX_EXPORT ExitPackage()
{
#ifdef _INSTRAPI
	LogClose(g_ProjectPerfLog);
#endif
}

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CBldPackage::XBuildSystem::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBldPackage::XBuildSystem::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CBldPackage::XBuildSystem::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// ISourceControl methods

STDMETHODIMP CBldPackage::XBuildSystem::GetActiveBuilder(HBUILDER *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetActiveBuilder();
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::IsActiveBuilderValid()
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetActiveBuilder() == NO_BUILDER ? S_FALSE : S_OK);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetBuilderType(HBUILDER hBld, int *pBuilderType)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);

	// Must have a valid builder.
	if ((hBld == ACTIVE_BUILDER || hBld == NO_BUILDER) && (g_BldSysIFace.GetActiveBuilder() == NO_BUILDER))
		return E_FAIL;

	*pBuilderType = g_BldSysIFace.GetBuilderType(hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetBuilderFile(HBUILDER hBld, LPCSTR *ppszPath)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	ASSERT(ppszPath != NULL);
	if ((hBld == ACTIVE_BUILDER || hBld == NO_BUILDER) && (g_BldSysIFace.GetActiveBuilder() == NO_BUILDER)) {
		*ppszPath = _T("");
		return E_FAIL;
	}

	*ppszPath = g_BldSysIFace.GetBuilderFile(hBld)->GetFullPath();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::CreateBuilder(const TCHAR *pchBldPath, 
			BOOL fForce, BOOL fOpen, HBUILDER *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.CreateBuilder(pchBldPath, fForce, fOpen);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::SetDirtyState(HBUILDER hBld, BOOL fDirty)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.SetDirtyState(hBld, fDirty) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetDirtyState(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetDirtyState(hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::SaveBuilder(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.SaveBuilder(hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::CloseBuilder(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.CloseBuilder(hBld,ProjectDelete) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::OnOpenWorkspace(LPCSTR pszWorkspaceName, BOOL bTemporary)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.OnOpenWorkspace(pszWorkspaceName, bTemporary, FALSE) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::OnDefaultWorkspace(LPCSTR pszFileToAdd)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.OnDefaultWorkspace(pszFileToAdd) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::OpenWorkspace(LPCSTR pszWorkspace)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.OpenWorkspace(pszWorkspace) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetFileSetFromTargetName(LPCSTR pchFileSet, HBUILDER hBld, HFILESET *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetFileSetFromTargetName(pchFileSet, hBld);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetTargetNameFromFileSet(HFILESET hFileSet, CString &str, 
			BOOL bInvalid, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetTargetNameFromFileSet(hFileSet, str, hBld, bInvalid) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetFileSetName(HFILESET hFileSet, CString &str, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetFileSetName(hFileSet, str, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetTargetFromFileSet(HFILESET hFileSet, HBLDTARGET &hTarg, 
			BOOL bInvalid, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetTargetFromFileSet(hFileSet, hTarg, hBld, bInvalid) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetActiveTarget(HBUILDER hBld, HBLDTARGET *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetActiveTarget(hBld);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetTarget(LPCSTR pchTarg, HBUILDER hBld, HBLDTARGET *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetTarget(pchTarg, hBld);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetTargetName(HBLDTARGET hTarg, CString &str, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetTargetName(hTarg, str, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::DeleteTarget(HBLDTARGET hTarg, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.DeleteTarget(hTarg, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::AddProjectDependency(HFILESET hFileSet, 
			HFILESET hDependantFileSet, HBUILDER hBld, HPROJDEP *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.AddProjectDependency(hFileSet, hDependantFileSet, hBld);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::RemoveProjectDependency(HPROJDEP hProjDep, 
			HFILESET hFileSet, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.RemoveProjectDependency(hProjDep, hFileSet, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetDependantProject(HFILESET *pHandle, HPROJDEP hProjDep, 
			HFILESET hFileSet, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetDependantProject(hProjDep, hFileSet, hBld);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetTargetDirectory(HBLDTARGET hTarg, CString &strDir, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetTargetProp(hTarg, Prop_TargetDirectory, strDir, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetTargetFileName(HBUILDER hBld, LPTSTR *ppszPath)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
    CPath* pPath = g_BldSysIFace.GetTargetFileName(hBld);
	if( pPath == NULL )
		return E_FAIL;

    LPCTSTR sz = pPath->GetFullPath();
	LPTSTR szNew = new TCHAR[_tcslen(sz)+1];
	_tcscpy(szNew, sz );
    delete pPath;

	*ppszPath = szNew;
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetTargetAttributes(HBUILDER hBld, int *piAttrs)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*piAttrs = g_BldSysIFace.GetTargetAttributes(hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::MakeTargPropsDefault(HBLDTARGET hTarg, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.MakeTargPropsDefault(hTarg, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::CheckFile(HFILESET hFileSet, CPath &pathFile, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.CheckFile(hFileSet, pathFile, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::AddFolder(HFOLDER *pHandle, HFILESET hFileSet, const TCHAR *pszFolder, HBUILDER hBld, HFOLDER hFolder, const TCHAR * pszFilter)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.AddFolder(hFileSet, pszFolder, hBld, hFolder, pszFilter);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::AddFile(HBLDFILE *pHandle, HFILESET hFileSet, const CPath *ppathFile, 
			int fSettings, HBUILDER hBld, HFOLDER hFolder)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.AddFile(hFileSet, ppathFile, fSettings, hBld, hFolder);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::AddDependencyFile(HFILESET hFileSet, 
			const CPath *ppathFile, HBUILDER hBld, HFOLDER hFolder)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.AddDependencyFile(hFileSet, ppathFile, hBld, hFolder) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::DelFile(HBLDFILE hFile, HFILESET hFileSet, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.DelFile(hFile, hFileSet, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetFile(const CPath *ppathFile, HBLDFILE &hFile, HFILESET hFileSet, 
			HBUILDER hBld, BOOL bSimple)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetFile(ppathFile, hFile, hFileSet, hBld, bSimple) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::SetFileProp(HBLDTARGET hTarg, HBLDFILE hFile, UINT idProp, int i)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.SetFileProp(hTarg, hFile, idProp, i) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::SetFileProp(HBLDTARGET hTarg, HBLDFILE hFile, UINT idProp, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.SetFileProp(hTarg, hFile, idProp, str) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::MakeFilePropsDefault(HBLDTARGET hTarg, HBLDFILE hFile)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.MakeFilePropsDefault(hTarg, hFile);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::TargetState(UINT *puState, CObList *plstPath, LPCSTR pchTarget, 
			HBLDTARGET hTarg, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*puState = g_BldSysIFace.TargetState(plstPath, pchTarget, hTarg, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::FileState(UINT *puState, const CPath *pathFile)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*puState = g_BldSysIFace.FileState(pathFile);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::ResolveIncludeDirectives(const CPath &pathSrc, 
			const CStringList &strlstIncs, CStringList &lstPath, BOOL fIgnStdIncs, const CString &strPlat)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.ResolveIncludeDirectives(pathSrc, strlstIncs,
		lstPath, fIgnStdIncs, strPlat) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::ResolveIncludeDirectives(const CDir &dirBase, 
			const CStringList &strlstIncs, CStringList &lstPath, BOOL fIgnStdIncs, const CString &strPlat)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.ResolveIncludeDirectives(dirBase, strlstIncs,
		lstPath, fIgnStdIncs, strPlat) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::ResolveIncludeDirectives(const CObList &lstpathSrc, 
			const CStringList &strlstIncs, CStringList &lstPath, HBLDTARGET hTarget)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.ResolveIncludeDirectives(lstpathSrc, strlstIncs, 
		lstPath, hTarget) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetIncludePath(CObList &lstIncDirs, const CString &strPlat,
			const CPath *ppathSrc, const CString &strTool, HBUILDER hBld )
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetIncludePath(lstIncDirs, strPlat, ppathSrc, strTool, hBld ) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::UpdateDependencyList(const CPath &pathSrc, 
			const CStringList &strlstIncs, HBLDTARGET hTarg, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.UpdateDependencyList(pathSrc, strlstIncs, hTarg, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::InitBuilderEnum()
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.InitBuilderEnum();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetNextBuilder(CString &strTarget, BOOL bOnlyLoaded, HBUILDER *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetNextBuilder(strTarget, bOnlyLoaded);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::InitTargetEnum(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.InitTargetEnum(hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetNextTarget(HBUILDER hBld, CString &strTarget, HBLDTARGET *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetNextTarget(strTarget, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::InitProjectDepEnum(HBUILDER hBld, HFILESET hFileSet)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.InitProjectDepEnum(hFileSet, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetNextProjectDep(HBUILDER hBld, HFILESET hFileSet, HPROJDEP *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetNextProjectDep(hFileSet, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::InitFileSetEnum(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.InitFileSetEnum(hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetNextFileSet(HBUILDER hBld, HFILESET *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetNextFileSet(hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::InitFileEnum(HFILESET hFileSet, UINT filter)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.InitFileEnum(hFileSet, filter);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetNextFile(HFILESET hFileSet, FileRegHandle &frh, HBLDFILE *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetNextFile(frh, hFileSet);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::IsScanableFile(HBLDFILE hFile)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.IsScanableFile(hFile) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetSccProjName(HBUILDER builder, CString& projName)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	CProject* proj = (CProject*) builder;
	if( proj == NULL )
		return E_FAIL;
		
	int x = proj->GetStrProp(P_ProjSccProjName, projName);
	return (x==valid) ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::SetSccProjName(HBUILDER builder, const CString& projName)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	CProject* proj = (CProject*) builder;
	if( proj == NULL )
		return E_FAIL;
	BOOL bSet = proj->SetStrProp(P_ProjSccProjName, projName);
	return bSet ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetSccRelLocalPath(HBUILDER builder, CString& propVal)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	CProject* proj = (CProject*) builder;
	if( proj == NULL )
		return E_FAIL;
		
	int x = proj->GetStrProp(P_ProjSccRelLocalPath, propVal);
	return (x==valid) ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::SetSccRelLocalPath(HBUILDER builder, const CString& propVal)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	CProject* proj = (CProject*) builder;
	if( proj == NULL )
		return E_FAIL;
	BOOL bSet = proj->SetStrProp(P_ProjSccRelLocalPath, propVal);
	return bSet ? NOERROR : E_FAIL;
}

//STDMETHODIMP CBldPackage::XBuildSystem::GetSccAuxPath(HBUILDER builder, CString& auxPath)
//{
//	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
//	CProject* proj = (CProject*) builder;
//	int x = proj->GetStrProp(P_ProjSccProjAux, auxPath);
//	return (x==valid) ? NOERROR : E_FAIL;
//}
//
//STDMETHODIMP CBldPackage::XBuildSystem::SetSccAuxPath(HBUILDER builder, const CString& auxPath)
//{
//	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
//	CProject* proj = (CProject*) builder;
//	BOOL bSet = proj->SetStrProp(P_ProjSccProjAux, auxPath);
//	return bSet ? NOERROR : E_FAIL;
//}

STDMETHODIMP CBldPackage::XBuildSystem::GetBuilder(HBLDTARGET hTarg, HBUILDER *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetBuilder(hTarg);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetBuilderFromFileSet(HFILESET hFileSet, HBUILDER *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetBuilderFromFileSet(hFileSet);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetFileSetTargets(HFILESET hFileSet, CStringList &strlstTargs)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetFileSetTargets(hFileSet, strlstTargs) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetFileSet(HBUILDER hBld, HBLDTARGET hTarg, HFILESET *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetFileSet(hBld, hTarg);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetBuildFile(UINT idFile, TCHAR *szFileName, int cchFileBuffer,
			HBLDTARGET hTarg, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetBuildFile(idFile, szFileName, cchFileBuffer, hTarg, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::RegisterSection(CBldrSection * pbldsect)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.RegisterSection((CBldrSection *)pbldsect) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::DeregisterSection(const CBldrSection * pbldsect)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.DeregisterSection((CBldrSection *)pbldsect) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::InitSectionEnum()
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.InitSectionEnum();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetNextSection(CBldrSection **ppSection, CString & strSection)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*ppSection = g_BldSysIFace.GetNextSection(strSection);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::FindSection(CBldrSection **ppSection, LPCSTR pchName)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*ppSection = g_BldSysIFace.FindSection(pchName);
	return *ppSection ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetResourceFileList(CPtrList &listRcPath, BOOL fOnlyBuildable, 
			HFILESET hFileSet, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetResourceFileList(listRcPath, fOnlyBuildable, hFileSet, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetResourceIncludePath(CPath *pResFile, CString &strIncludes)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetResourceIncludePath(pResFile, strIncludes) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::LanguageSupported(UINT idLang)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.LanguageSupported(idLang) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetSelectedFile(CPath *path)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetSelectedFile(path) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetProjectDir(HBLDTARGET hTarget, HBUILDER hBld, CString& str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetProjectDir(hTarget, hBld, str) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetWorkspaceDir(HBUILDER hBld, CString& str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetWorkspaceDir(hBld, str) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::BuildSystemEnabled()
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.BuildSystemEnabled() ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::IsBuildInProgress()
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.IsBuildInProgress() ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::DoSpawn(CStringList& lstCmd,LPCTSTR szDirInit,BOOL fClear, BOOL fASync, DWORD *pcErr, DWORD *pcWarn)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.DoSpawn(lstCmd, szDirInit, fClear, fASync, pcErr, pcWarn));
}

STDMETHODIMP CBldPackage::XBuildSystem::GetBuilderToolset(int *piToolset, HBUILDER hBuilder)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	int iToolset = g_BldSysIFace.GetBuilderToolset(hBuilder);
	if (iToolset != -1)
		*piToolset = iToolset;
	return (iToolset != -1) ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetRegistryEntryPath(FileRegHandle frh, const CPath **ppPath)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	CFileRegEntry *pRegEntry;
	pRegEntry = g_BldSysIFace.GetFileRegistry()->GetRegEntry(frh);
	if( pRegEntry == NULL )
		return E_FAIL;

	*ppPath = pRegEntry->GetFilePath();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetRemoteTargetFileName(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetRemoteTargetFileName(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::SetRemoteTargetFileName(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.SetRemoteTargetFileName(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetSlobWnd(HBUILDER hBld, CWnd **ppWnd)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*ppWnd = g_BldSysIFace.GetSlobWnd(hBld);
	return (*ppWnd) ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::IsProjectFile(const CPath *pPath, BOOL bIncludeDeps, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.IsProjectFile(pPath, bIncludeDeps, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetBrowserDatabaseName(HBUILDER hBld, const CPath **ppPath)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*ppPath = g_BldSysIFace.GetBrowserDatabaseName(hBld);
	return (*ppPath) ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetPreCompiledHeaderName(HBUILDER hBld, CString &strPch)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	if( !g_BldSysIFace.GetPreCompiledHeaderName(strPch,hBld) ){
		return E_FAIL;
	}
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::UpdateBrowserDatabase(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.UpdateBrowserDatabase(hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetCallingProgramName(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetCallingProgramName(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::SetCallingProgramName(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.SetCallingProgramName(str, hBld);

	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetProgramRunArguments(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetProgramRunArguments(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::SetProgramRunArguments(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.SetProgramRunArguments(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetWorkingDirectory(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetWorkingDirectory(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::TargetIsCaller(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.TargetIsCaller(hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetPromptForDlls(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetPromptForDlls(hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::SetPromptForDlls(HBUILDER hBld, BOOL bPrompt)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.SetPromptForDlls(bPrompt, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::SetProjectState(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.SetProjectState(hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetClassWizAddedFiles(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetClassWizAddedFiles(hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::SetClassWizAddedFiles(HBUILDER hBld, BOOL bAdded)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.SetClassWizAddedFiles(bAdded, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::ScanAllDependencies(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.ScanAllDependencies(hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetVersionInfo(HBUILDER hBld, void *ppvi)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetVersionInfo((PROJECT_VERSION_INFO *)ppvi, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetInitialExeForDebug(HBUILDER hBld, BOOL bExecute)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetInitialExeForDebug(bExecute, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetInitialRemoteTarget(HBUILDER hBld, BOOL bBuild, BOOL fAlways)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.GetInitialRemoteTarget(bBuild, fAlways, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::UpdateRemoteTarget(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.UpdateRemoteTarget(hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::DoTopLevelBuild(HBUILDER hBld, BOOL bBuildAll, CStringList *pConfigs,
			FlagsChangedAction fca, BOOL bVerbose, BOOL bClearOutputWindow )
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);

	CBuildIt wholeBuild;
	return(g_BldSysIFace.DoTopLevelBuild(bBuildAll, pConfigs,
		fca, bVerbose, bClearOutputWindow, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetEnvironmentVariables(HBUILDER hBld, void *pEnvList)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetEnvironmentVariables((CEnvironmentVariableList *)pEnvList, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::IsProfileEnabled(HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return(g_BldSysIFace.IsProfileEnabled(hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildSystem::EnumProjectTypes( LPENUMPROJECTTYPES *ppIEnum, LPPROJECTTYPE * )
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	CEnumProjectTypes *pProjT = new CEnumProjectTypes;
	*ppIEnum = pProjT->GetInterface();
	return (*ppIEnum) ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::EnumPlatforms( LPENUMPLATFORMS *ppIEnum, LPPLATFORM * )
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	CEnumPlatforms *pPlat = new CEnumPlatforms;
	*ppIEnum = pPlat->GetInterface();
	return (*ppIEnum) ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::EnumBuildTools( LPENUMBUILDTOOLS *ppIEnum, LPBUILDTOOL *ppTool )
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	CEnumBuildTools *pTools = new CEnumBuildTools;
	*ppIEnum = pTools->GetInterface();
	return (*ppIEnum) ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetCommandLineOptions( HFILESET hFileSet, HBLDFILE hFile, CString &rval  )
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	CFileItem *pFile = g_BldSysIFace.CnvHBldFile(hFileSet,hFile);
	if( pFile ){
		ConfigurationRecord *pcr = pFile->GetActiveConfig();
		if( pcr == NULL )
			return E_FAIL;

		CActionSlobList *pActList = pcr->GetActionList();
		if( pActList && !pActList->IsEmpty() ) {
			CActionSlob * pAction;
			pAction = (CActionSlob *)pActList->GetHead();
			if( pAction ){
				BOOL dummy;
				pAction->RefreshCommandOptions(dummy);
				pAction->GetCommandOptions(rval);
				return NOERROR;
			}
		}
	}
	return E_FAIL;
}

// Java support

STDMETHODIMP CBldPackage::XBuildSystem::GetJavaClassName(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetJavaClassName(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetJavaClassFileName(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetJavaClassFileName(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetJavaDebugUsing(HBUILDER hBld, ULONG* pDebugUsing)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetJavaDebugUsing(pDebugUsing, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetJavaBrowser(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetJavaBrowser(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetJavaStandalone(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetJavaStandalone(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetJavaStandaloneArgs(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetJavaStandaloneArgs(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetJavaHTMLPage(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetJavaHTMLPage(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetJavaClassPath(HBUILDER hBld, CString &str)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetJavaClassPath(str, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetJavaStandaloneDebug(HBUILDER hBld, ULONG* pStandaloneDebug)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	g_BldSysIFace.GetJavaStandaloneDebug(pStandaloneDebug, hBld);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetInitialJavaInfoForDebug(HBUILDER hBld, BOOL bExecute)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return (g_BldSysIFace.GetInitialJavaInfoForDebug(bExecute, hBld)) ? NOERROR : S_FALSE;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetPkgProject(HBUILDER builder, IPkgProject** outPkgProject)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return g_BldSysIFace.GetPkgProject(builder, outPkgProject);
}

STDMETHODIMP CBldPackage::XBuildSystem::GetProjectGlyph(HBUILDER hBld, HFILESET hFileSet, CImageWell& imageWell, UINT * pnImage)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	return g_BldSysIFace.GetProjectGlyph(hBld, hFileSet, imageWell, pnImage) ? S_OK : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetBuilderFromName(LPCTSTR pszBuilder, HBUILDER *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	*pHandle = g_BldSysIFace.GetBuilderFromName(pszBuilder);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildSystem::GetNameFromBuilder(HBUILDER hBld, CString& szBuilder)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildSystem);
	szBuilder = g_BldSysIFace.GetNameFromBuilder(hBld);
	return szBuilder.IsEmpty() ? E_FAIL : NOERROR;
}

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CBldPackage::XBuildWizard::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBldPackage::XBuildWizard::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CBldPackage::XBuildWizard::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IBuildWizard methods

STDMETHODIMP CBldPackage::XBuildWizard::SetActiveTarget(HBLDTARGET hTarg, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.SetActiveTarget(hTarg, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::SetDefaultTarget(HBLDTARGET hTarg, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.SetDefaultTarget(hTarg, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::AddTarget(HBLDTARGET *pHandle, LPCSTR pchTarg, LPCSTR pchPlat, LPCSTR pchType, 
			BOOL fUIDesc, BOOL fDebug, TrgCreateOp trgop, HBLDTARGET hOtherTarg, SettingOp setop, 
			HBLDTARGET hSettingsTarg, BOOL fQuiet, BOOL fOutDir, BOOL fUseMFC, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	*pHandle = g_BldSysIFace.AddTarget(pchTarg, pchPlat, pchType, fUIDesc, fDebug, trgop, 
		hOtherTarg, setop, hSettingsTarg, fQuiet, fOutDir, fUseMFC, hBld);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildWizard::AddDefaultTargets(LPCSTR pchPlat, LPCSTR pchType, 
			HBLDTARGET &hDebugTarg, HBLDTARGET &hReleaseTarg, BOOL fUIDesc, BOOL fOutDir, 
			BOOL fUseMFC, HBLDTARGET hMirrorTarg, HBUILDER hBld, LPCSTR pchTarg)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.AddDefaultTargets(pchPlat, pchType, hDebugTarg, hReleaseTarg, fUIDesc, 
		fOutDir, fUseMFC, hMirrorTarg, hBld, pchTarg) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::SetToolSettings(HBLDTARGET hTarg, LPCSTR pchSettings, 
			LPCSTR pchTool, BOOL fAdd, BOOL fClear, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.SetToolSettings(hTarg, pchSettings, pchTool, fAdd, fClear, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::SetTargetIsAppWiz(HBLDTARGET hTarg, int iIsAppWiz, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.SetTargetProp(hTarg, Prop_AppWizTarg, iIsAppWiz, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::SetTargetUseOfMFC(HBLDTARGET hTarg, int iUseOfMFC, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.SetTargetProp(hTarg, Prop_UseOfMFC, iUseOfMFC, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::GetTargetUseOfMFC(HBLDTARGET hTarg, int *iUseOfMFC, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.GetTargetProp(hTarg, Prop_UseOfMFC, *iUseOfMFC, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::SetTargetDefExt(HBLDTARGET hTarg, CString &strExt, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.SetTargetProp(hTarg, Prop_TargetDefExt, strExt, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::GetTargetOutDir(HBLDTARGET hTarg, CString &strOut, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.GetTargetProp(hTarg, Prop_TargetOutDir, strOut, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::SetTargetOutDir(HBLDTARGET hTarg, CString &strOut, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.SetTargetProp(hTarg, Prop_TargetOutDir, strOut, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::GetTargetIntDir(HBLDTARGET hTarg, CString &strInt, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.GetTargetProp(hTarg, Prop_IntermediateOutDir, strInt, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::SetTargetIntDir(HBLDTARGET hTarg, CString &strInt, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.SetTargetProp(hTarg, Prop_IntermediateOutDir, strInt, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::AddFolder(HFOLDER *pHandle, HFILESET hFileSet, const TCHAR *pszFolder, HBUILDER hBld, HFOLDER hFolder, const TCHAR * pszFilter)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	*pHandle = g_BldSysIFace.AddFolder(hFileSet, pszFolder, hBld, hFolder, pszFilter);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildWizard::AddFile(HBLDFILE *pHandle, HFILESET hFileSet, const CPath *ppathFile, 
			int fSettings, HBUILDER hBld, HFOLDER hFolder)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	*pHandle = g_BldSysIFace.AddFile(hFileSet, ppathFile, fSettings, hBld, hFolder);
	return *pHandle ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildWizard::AddMultiFiles(HFILESET hFileSet, const CPtrList *plstpathFiles, 
			CPtrList *plstHBldFiles, int fSettings, HBUILDER hBld, HFOLDER hFolder)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	g_BldSysIFace.AddMultiFiles(hFileSet, plstpathFiles, plstHBldFiles, fSettings, hBld, hFolder);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildWizard::AddDependencyFile(HFILESET hFileSet, 
			const CPath *ppathFile, HBUILDER hBld, HFOLDER hFolder)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.AddDependencyFile(hFileSet, ppathFile, hBld, hFolder) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::SetToolSettings(HBLDTARGET hTarg, HBLDFILE hFile, 
			LPCSTR pchSettings, LPCSTR pchTool, BOOL fAdd, BOOL fClear)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.SetToolSettings(hTarg, hFile, pchSettings, pchTool, fAdd, fClear) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::SetBuildExclude(HBLDTARGET hTarg, HBLDFILE hFile, BOOL fExclude)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.SetBuildExclude(hTarg, hFile, fExclude) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::GetTargetTypeNames(const CStringList **ppList, 
			LPCSTR pchPlat, BOOL fUIDesc)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	*ppList = g_BldSysIFace.GetTargetTypeNames(pchPlat, fUIDesc);
	return *ppList ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildWizard::AssignCustomBuildStep(LPCSTR pchCommand, 
			LPCSTR pchOutput, LPCSTR pchDescription, HBLDTARGET hTarg, HBLDFILE hFile, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	// replace all new line characters with comma's
	BOOL rVal = g_BldSysIFace.AssignCustomBuildStep(pchCommand, pchOutput,pchDescription, hTarg, hFile, hBld);

	return( rVal? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::UnassignCustomBuildStep(HBLDTARGET hTarg, HBLDFILE hFile, 
			HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	return(g_BldSysIFace.UnassignCustomBuildStep(hTarg, hFile, hBld) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildWizard::GetFileSet(HBUILDER hBld, HBLDTARGET hTarg, HFILESET *pHandle)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	*pHandle = g_BldSysIFace.GetFileSet(hBld, hTarg);
	return (*pHandle) ? NOERROR : E_FAIL ;
}

STDMETHODIMP CBldPackage::XBuildWizard::GetAutoProject(HBUILDER hBld, IBuildProject** ppProject)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	*ppProject = g_BldSysIFace.GetAutoProject(hBld);
	return *ppProject ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildWizard::SetUserDefinedDependencies(LPCSTR pchUserDeps, HBLDTARGET hTarg, HBLDFILE hFile, HBUILDER hBld)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildWizard);
	// replace all new line characters with comma's
	BOOL rVal = g_BldSysIFace.SetUserDefinedDependencies(pchUserDeps, hTarg, hFile, hBld);

	return( rVal? S_OK : S_FALSE);
}

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CBldPackage::XBuildPlatforms::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBldPackage::XBuildPlatforms::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CBldPackage::XBuildPlatforms::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IActiveProject methods

STDMETHODIMP CBldPackage::XBuildPlatforms::GetAvailablePlatforms(const CStringList **ppList,
			LPCSTR pchTarg, BOOL fUIDesc)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	*ppList = g_BldSysIFace.GetAvailablePlatforms(pchTarg, fUIDesc);
	return *ppList ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetSupportedPlatformCount(int *piCount)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	*piCount = g_BldSysIFace.GetProjComponentMgr()->GetSupportedPlatformCount();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPrimaryPlatform(uniq_platform *pupID)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	if( !g_BldSysIFace.GetProjComponentMgr()->IsInit() ){
		if( g_BldSysIFace.GetProjComponentMgr()->FInit() )
			g_BldSysIFace.GetProjComponentMgr()->IsInit(TRUE);
	}
	if (g_BldSysIFace.GetProjComponentMgr()->GetPrimaryPlatform() == NULL)
	{
		*pupID = unknown_platform;
		return E_FAIL;
	}

	*pupID = (uniq_platform)g_BldSysIFace.GetProjComponentMgr()->GetPrimaryPlatform()->GetUniqueId();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetCurrentPlatform(HBUILDER hBld, uniq_platform *pupID)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	if (g_BldSysIFace.GetCurrentPlatform(hBld) == NULL)
	{
		*pupID = unknown_platform;
		return E_FAIL;
	}

	*pupID = (uniq_platform)g_BldSysIFace.GetCurrentPlatform(hBld)->GetUniqueId();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::IsPlatformSupported(uniq_platform upID)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;

	return(pPlatform->IsSupported() ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildPlatforms::InitPlatformEnum()
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	g_BldSysIFace.GetProjComponentMgr()->InitPlatformEnumExport();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::NextPlatform(uniq_platform *pupID)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	if ((!g_BldSysIFace.GetProjComponentMgr()->NextPlatformExport(pPlatform)) || (pPlatform == NULL))
	{
		*pupID = unknown_platform;
		return E_FAIL;
	}
	*pupID = (uniq_platform)pPlatform->GetUniqueId();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformUIDescription(uniq_platform upID, LPCSTR *ppszDesc)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*ppszDesc = *(pPlatform->GetUIDescription());
	return *ppszDesc ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformSHName(uniq_platform upID, LPCSTR *ppszName)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*ppszName = pPlatform->GetSHName();
	return *ppszName ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformEMName(uniq_platform upID, LPCSTR *ppszName)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*ppszName = pPlatform->GetEMName();
	return *ppszName ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformEEName(uniq_platform upID, LPCSTR *ppszName)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*ppszName = pPlatform->GetEEName();
	return *ppszName ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformLocalTLIndex(uniq_platform upID, UINT *puTLIndex)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*puTLIndex = pPlatform->GetLocalTLIndex();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformCurrentTLIndex(uniq_platform upID, UINT *puTLIndex)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*puTLIndex = pPlatform->GetCurrentTLIndex();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::SetPlatformCurrentTLIndex(uniq_platform upID, UINT uTLIndex)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	pPlatform->SetCurrentTLIndex(uTLIndex);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformCompRegKeyName(uniq_platform upID, BOOL fInclAppKey, LPCSTR *ppszName)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*ppszName = pPlatform->GetCompRegKeyName(fInclAppKey);
	return *ppszName ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformNMCount(uniq_platform upID, int *piNMCount)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*piNMCount = (int)pPlatform->GetNMCount();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformNMName(uniq_platform upID, UINT uNMIndex, LPCSTR *ppszName)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*ppszName = pPlatform->GetNMName(uNMIndex);
	return *ppszName ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformTLCount(uniq_platform upID, int *piTLCount)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*piTLCount = pPlatform->GetTLCount();
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformTLName(uniq_platform upID, UINT uTLIndex, LPCSTR *ppszName)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*ppszName = pPlatform->GetTLName(uTLIndex);
	return *ppszName ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformTLDescription(uniq_platform upID, UINT uTLIndex, LPCSTR *ppszDesc)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*ppszDesc = pPlatform->GetTLDescription(uTLIndex);
	return *ppszDesc ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformTLFlags(uniq_platform upID, UINT uTLIndex, UINT *puFlags)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*puFlags = pPlatform->GetTLFlags(uTLIndex);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildPlatforms::GetPlatformBuildable(uniq_platform upID, BOOL *pfBuildable)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildPlatforms);
	CPlatform *pPlatform;
	g_BldSysIFace.GetProjComponentMgr()->LookupPlatformByUniqueId((UINT)upID, pPlatform);
	if( pPlatform == NULL )
		return E_FAIL;
	*pfBuildable = pPlatform->GetBuildable();
	return NOERROR;
}

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CBldPackage::XBuildComponents::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CBldPackage, BuildComponents)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBldPackage::XBuildComponents::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CBldPackage, BuildComponents)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CBldPackage::XBuildComponents::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CBldPackage, BuildComponents)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IBuildComponents methods

STDMETHODIMP CBldPackage::XBuildComponents::GetBldSysCompName(DWORD dwID, TCHAR **ppszName)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildComponents)
	CBldSysCmp *pComp;
	g_BldSysIFace.GetProjComponentMgr()->LookupBldSysComp(dwID, pComp);
//	*ppszName = pComp->GetName();
	// REVIEW: NYI
	// return *ppszName ? NOERROR : E_FAIL;
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildComponents::GenerateBldSysCompPackageId(TCHAR *pszName, WORD *pwID)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildComponents)
	*pwID = g_BldSysIFace.GetProjComponentMgr()->GenerateBldSysCompPackageId(pszName);
	return(NOERROR);
}

STDMETHODIMP CBldPackage::XBuildComponents::LookupBldSysComp(DWORD dwID, void **ppBldSysCmp)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildComponents)
	return(g_BldSysIFace.GetProjComponentMgr()->LookupBldSysComp(dwID, (CBldSysCmp *&)*ppBldSysCmp) ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildComponents::GetProjTypeFromProjItem(void *pProjItem, int *pType )
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildComponents)
	CProjType *pProjType;
	if (g_BldSysIFace.GetProjComponentMgr()->GetProjTypefromProjItem((CProjItem *)pProjItem, pProjType))
	{
		*pType = (int)pProjType->GetUniqueTypeId();
		return NOERROR;
	}
	return E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildComponents::GetProjItemAttributes(void *pProjItem, int *iAttrs)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildComponents)
	CProjType *pProjType;
	if (g_BldSysIFace.GetProjComponentMgr()->GetProjTypefromProjItem((CProjItem *)pProjItem, pProjType))
	{
		*iAttrs = pProjType->GetAttributes();
		return(NOERROR);
	}
	else
	{
		*iAttrs = 0;
		return(E_FAIL);
	}
}

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CBldPackage::XBuildStatus::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CBldPackage, BuildStatus)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBldPackage::XBuildStatus::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CBldPackage, BuildStatus)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CBldPackage::XBuildStatus::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CBldPackage, BuildStatus)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IBuildStatus methods

STDMETHODIMP CBldPackage::XBuildStatus::IsBuildPossible()
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildStatus)
	return(g_Spawner.CanSpawn() ? S_OK : S_FALSE);
}

STDMETHODIMP CBldPackage::XBuildStatus::GetErrorCount(LPDWORD pdwErrorCount)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildStatus)
	DWORD dwErrorCount, dwWarningCount;
	g_Spawner.GetErrorCount(dwErrorCount, dwWarningCount);
	*pdwErrorCount = dwErrorCount;
	return(NOERROR);
}

STDMETHODIMP CBldPackage::XBuildStatus::GetWarningCount(LPDWORD pdwWarningCount)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildStatus)
	DWORD dwErrorCount, dwWarningCount;
	g_Spawner.GetErrorCount(dwErrorCount, dwWarningCount);
	*pdwWarningCount = dwWarningCount;
	return(NOERROR);
}

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CBldPackage::XBuildDirManager::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CBldPackage, BuildDirManager)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBldPackage::XBuildDirManager::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CBldPackage, BuildDirManager)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CBldPackage::XBuildDirManager::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CBldPackage, BuildDirManager)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IBuildStatus methods

STDMETHODIMP CBldPackage::XBuildDirManager::GetPlatformIndex(
	LPCTSTR lpszPlatform, int* pnPlatform)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildDirManager)
	*pnPlatform = GetDirMgr()->GetPlatformToolset(lpszPlatform);

	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildDirManager::GetDirList(
	int nPlatform, DIRLIST_TYPE type, const CObList** ppList)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildDirManager)

	*ppList = GetDirMgr()->GetDirList(type, nPlatform);
	
	return *ppList ? NOERROR : E_FAIL;
}

STDMETHODIMP CBldPackage::XBuildDirManager::GetDirListString(
	int nPlatform, DIRLIST_TYPE type, LPSTR* ppszPath)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildDirManager)

	CString str;
	GetDirMgr()->GetDirListString(str, type, nPlatform);

	*ppszPath = (LPSTR) AfxAllocTaskMem(str.GetLength() + 1);
	if (!*ppszPath)
		return E_FAIL;

	lstrcpy(*ppszPath, str);
	return NOERROR;
}

STDMETHODIMP CBldPackage::XBuildDirManager::FindFileOnPath(
	int nPlatform, DIRLIST_TYPE type, LPCSTR pszFile, LPSTR* ppszFullPath)
{
	METHOD_PROLOGUE_EX(CBldPackage, BuildDirManager)

	CString str;
	if (!::FindFileOnPath(pszFile, str, type, nPlatform))
	{
		*ppszFullPath = NULL;
		return S_FALSE;
	}

	*ppszFullPath = (LPSTR) AfxAllocTaskMem(str.GetLength() + 1);
	lstrcpy(*ppszFullPath, str);

	return  S_OK;
}

////////////////////////////////////////////////////////////////////////////
// IAutoBld

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CBldPackage::XAutoBld::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CBldPackage, AutoBld)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBldPackage::XAutoBld::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CBldPackage, AutoBld)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CBldPackage::XAutoBld::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CBldPackage, AutoBld)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IAutoBld methods

STDMETHODIMP CBldPackage::XAutoBld::Errors(THIS_ long &errors)
{
	METHOD_PROLOGUE_EX(CBldPackage, AutoBld)
	errors = CBuildIt::m_actualErrs;
	return S_OK;
}

STDMETHODIMP CBldPackage::XAutoBld::Warnings(THIS_ long &warnings)
{
	METHOD_PROLOGUE_EX(CBldPackage, AutoBld)
	warnings = CBuildIt::m_warns;
	return S_OK;
}

STDMETHODIMP CBldPackage::XAutoBld::AddProject(THIS_ LPCTSTR szName, LPCTSTR szPath, LPCTSTR szType, VARIANT_BOOL bAddDefaultFolders )
{
	METHOD_PROLOGUE_EX(CBldPackage, AutoBld)

	CString strProjName(szName);
	CString strProjPath(szPath);
	CString strProjType(szType);
	BOOL bAddFolders = (bAddDefaultFolders == VARIANT_TRUE);

	CPath path; CDir dir;
	// extra validation for name and path
	if (strProjName.IsEmpty() || (strProjName == _T("..")) || (strProjName == _T(".")) ||
		(strProjName.FindOneOf(_T("*?<>|")) != -1) ||
		(strProjPath.FindOneOf(_T("*?<>|")) != -1))
	{
		return E_FAIL;
	}

	if (strProjPath.IsEmpty())
	{
		dir.CreateFromCurrent();
		if (!path.CreateFromDirAndFilename(dir, strProjName))
			return E_FAIL;
	}
	else if (dir.CreateFromString(strProjPath) && dir.ExistsOnDisk())
	{
		if (!path.CreateFromDirAndFilename(dir, strProjName))
			return E_FAIL;
	}
	else if (!path.Create(strProjPath))
	{
		if (dir.CreateFromString(strProjPath) && dir.CreateOnDisk())
		{
			if (!path.CreateFromDirAndFilename(dir, strProjName))
				return E_FAIL;
		}
		else
		{
			return E_FAIL; // bogus path
		}
	}

	// validate and create (if necessary) the required directory
	if (!dir.CreateFromPath(path) || ((!dir.ExistsOnDisk()) && (!dir.CreateOnDisk())))
	{
		return E_FAIL;
	}
	
	path.ChangeExtension(_T(BUILDER_EXT));
	
	// these MUST be made CompareNoCase !!!
	const TCHAR *szProjType = NULL;
	if( strProjType.CompareNoCase(_T("Application"))==0)
			szProjType = _T("Application");
	else if( strProjType.CompareNoCase(_T("DLL"))==0)
			szProjType = _T("Dynamic-Link Library");
	else if( strProjType.CompareNoCase(_T("Console Application"))==0)
			szProjType = _T("Console Application");
	else if( strProjType.CompareNoCase(_T("Static Library"))==0)
			szProjType = _T("Static Library");
	else if( strProjType.CompareNoCase(_T("Utility"))==0)
			szProjType = _T("Generic Project");
	else if( strProjType.CompareNoCase(_T("Makefile"))==0)
			szProjType = _T("External Target");
	else if( strProjType.CompareNoCase(_T("QuickWin Application"))==0) // REVIEW
			szProjType = _T("QuickWin Application");
	else if( strProjType.CompareNoCase(_T("Standard Graphics Application"))==0) // REVIEW
			szProjType = _T("Standard Graphics Application");
	else
	{
		return E_FAIL;
	}

	// if there is a workspace already open then we don't need to create
	// a workspace.

	BOOL bCreatedWorkspace = FALSE;
	CPath wksPath = path;
	LPCTSTR lpstrTitle;
	if (FAILED(g_pProjWksIFace->GetWorkspaceDocTitle(&lpstrTitle))) {
		// now create a workspace
		CDocument *pDoc;

		// drop the extension because CreateEmptyWorkspace appends .wks
		CString strFileBase;
		wksPath.GetBaseNameString(strFileBase);
		wksPath.ChangeFileName(strFileBase);

		// workspace creation will create the directory if necessary
		if ((FAILED(g_pProjWksIFace->CreateEmptyWorkspace(&pDoc, (LPCTSTR)wksPath))) || (pDoc == NULL))
			return E_FAIL;
		else
			bCreatedWorkspace = TRUE;
	}
	wksPath.ChangeExtension(_T(WORKSPACE_EXT));

	char szFname [_MAX_FNAME];
	_splitpath(strProjName, NULL, NULL,	szFname, NULL);

	// make sure this project name is unique in the workspace
	if(g_pProjWksIFace->CanAddProject(szFname) != S_OK)
	{
		ASSERT(!bCreatedWorkspace);
		return E_FAIL;
	}

	HBUILDER hBld;
	hBld = g_BldSysIFace.CreateBuilder(
			(LPCTSTR)path, 
			FALSE, 
			FALSE );

	if( hBld == NO_BUILDER )
	{
		if (bCreatedWorkspace)
			g_pProjWksIFace->CloseWorkspace();
		return E_FAIL;
	}

	// These strings CANNOT be localized!
	const TCHAR *szPlatform = _T("Win32 (x86)");
	CPlatform * pPlatform = g_prjcompmgr.GetPrimaryPlatform();
	ASSERT(pPlatform);
	if (pPlatform)
		szPlatform = *pPlatform->GetName(); // use primary platform


	HBLDTARGET hDebug, hRelease;
	if( !g_BldSysIFace.AddDefaultTargets(
								szPlatform, 
								szProjType,
								hDebug,
								hRelease,
								FALSE, 
								TRUE, 
								TRUE,
								NO_TARGET,
								hBld,
								szFname))
	{
		g_BldSysIFace.CloseBuilder(hBld, ProjectDelete);
		if (bCreatedWorkspace)
			g_pProjWksIFace->CloseWorkspace();
		return E_FAIL;
	}

	if (bAddFolders)
	{
		CString strFilter, strFolder;
		strFolder.LoadString(IDS_SOURCE_FILES);
		strFilter.LoadString(IDS_SOURCE_FILES_FILTER);
		// Now let us add the folders
		HFOLDER hFolder = NO_FOLDER;
		g_BldSysIFace.AddFolder(
			ACTIVE_FILESET, 
			strFolder, 
			hBld, 
			NO_FOLDER,
			strFilter);

		strFolder.LoadString(IDS_HEADER_FILES);
		strFilter.LoadString(IDS_HEADER_FILES_FILTER);
		g_BldSysIFace.AddFolder(
			ACTIVE_FILESET, 
			strFolder, 
			hBld, 
			NO_FOLDER,
			strFilter);

		strFolder.LoadString(IDS_RESOURCE_FILES);
		strFilter.LoadString(IDS_RESOURCE_FILES_FILTER);
		g_BldSysIFace.AddFolder(
			ACTIVE_FILESET, 
			strFolder, 
			hBld, 
			NO_FOLDER,
			strFilter);
	}

	// AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND, IDM_PROJECT_SETTINGS);
	COleRef<IPkgProject> pProject;
	VERIFY(SUCCEEDED(g_BldSysIFace.GetPkgProject(hBld, &pProject)));

	if((pProject == NULL) || (!SUCCEEDED(g_pProjWksIFace->AddProject(pProject, TRUE))))
	{
		g_BldSysIFace.CloseBuilder(hBld, ProjectDelete);
		if (bCreatedWorkspace)
			g_pProjWksIFace->CloseWorkspace();
		return E_FAIL;
	}
	theApp.NotifyPackages(PN_NEW_PROJECT, (void *)pProject);

	if (bCreatedWorkspace)
	{
		theApp.NotifyPackages(PN_WORKSPACE_INIT, (void *)(LPCSTR)wksPath);
	}

	g_pProjWksIFace->SetActiveProject(pProject, TRUE);
	g_pProjWksIFace->SetWorkspaceDocDirty();

	return S_OK;
}

STDMETHODIMP CBldPackage::XAutoBld::Build(THIS_ const VARIANT FAR& configuration,
										  BOOL bRebuildAll)
{
	METHOD_PROLOGUE_EX(CBldPackage, AutoBld)

	BOOL bRetVal = TRUE;
	if (!g_Spawner.CanSpawn())
		DsThrowOleDispatchException(DS_E_CANT_SPAWN_BUILD, IDS_AUTO_CANT_SPAWN_BUILD);

	if (configuration.vt == VT_ERROR)
	{
		// Default passes no params--just build active config
		CBuildIt wholeBuild;
		if( bRebuildAll ){
			g_BldSysIFace.DoTopLevelBuild(TOB_Clean, NULL, fcaNeverQueried, TRUE, TRUE,
				g_BldSysIFace.GetActiveBuilder());
			CBuildIt::m_mapConfigurationsBuilt.RemoveAll();
		}
		bRetVal = g_BldSysIFace.DoTopLevelBuild(bRebuildAll, NULL, fcaNeverQueried, TRUE, TRUE,
			g_BldSysIFace.GetActiveBuilder());
	}
	else
	{
		COleVariant var(configuration);
		var.ChangeType(VT_DISPATCH);
		// A configuration was passed to build
		CAutoConfiguration* pConfig = CAutoConfiguration::FromIDispatch((IConfiguration*)(var.pdispVal));
		if (pConfig == NULL || !pConfig->IsKindOf(RUNTIME_CLASS(CAutoConfiguration)))
		{
			// Someone passed us a bogus configuration.
			DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTO_BADCFG);
		}
		if (pConfig->GetHBuilder() == NULL)
		{
			// This configuration no longer exists
			CString strPrompt;
			AfxFormatString1(strPrompt, IDS_AUTO_CFG_NOTEXIST, pConfig->GetConfigName());
			DsThrowOleDispatchException(DS_E_CONFIGURATION_NOT_VALID, (LPCTSTR) strPrompt);
		}
		pConfig->EnsureBuildable();		// Throws error if unsupported platform
		CStringList strlConfigs;

		CBuildIt wholeBuild;
		if( bRebuildAll ){
			strlConfigs.AddTail(pConfig->GetConfigName());
		    g_BldSysIFace.DoTopLevelBuild(TOB_Clean, &strlConfigs, fcaNeverQueried, TRUE, TRUE,
			    pConfig->GetHBuilder());
			CBuildIt::m_mapConfigurationsBuilt.RemoveAll();
		}
		strlConfigs.AddTail(pConfig->GetConfigName());
		bRetVal = g_BldSysIFace.DoTopLevelBuild(bRebuildAll, &strlConfigs, fcaNeverQueried, TRUE, TRUE,
			pConfig->GetHBuilder());

	}
	return bRetVal ? S_OK : S_FALSE;
}

STDMETHODIMP CBldPackage::XAutoBld::Clean(THIS_ const VARIANT FAR& configuration )
{
	METHOD_PROLOGUE_EX(CBldPackage, AutoBld)

	BOOL bRetVal = TRUE;
	if (!g_Spawner.CanSpawn())
		DsThrowOleDispatchException(DS_E_CANT_SPAWN_BUILD, IDS_AUTO_CANT_SPAWN_BUILD);

	CBuildIt wholeBuild(FALSE);
	if (configuration.vt == VT_ERROR)
	{
		// Default passes no params--just build active config
		bRetVal = g_BldSysIFace.DoTopLevelBuild(TOB_Clean, NULL, fcaNeverQueried, TRUE, TRUE,
			g_BldSysIFace.GetActiveBuilder(), FALSE );
	}
	else
	{
		COleVariant var(configuration);
		var.ChangeType(VT_DISPATCH);
		// A configuration was passed to build
		CAutoConfiguration* pConfig = CAutoConfiguration::FromIDispatch((IConfiguration*)(var.pdispVal));
		if (pConfig == NULL || !pConfig->IsKindOf(RUNTIME_CLASS(CAutoConfiguration)))
		{
			// Someone passed us a bogus configuration.
			DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTO_BADCFG);
		}
		if (pConfig->GetHBuilder() == NULL)
		{
			// This configuration no longer exists
			CString strPrompt;
			AfxFormatString1(strPrompt, IDS_AUTO_CFG_NOTEXIST, pConfig->GetConfigName());
			DsThrowOleDispatchException(DS_E_CONFIGURATION_NOT_VALID, (LPCTSTR) strPrompt);
		}
		pConfig->EnsureBuildable();		// Throws error if unsupported platform
		CStringList strlConfigs;
		strlConfigs.AddTail(pConfig->GetConfigName());
		bRetVal = g_BldSysIFace.DoTopLevelBuild(TOB_Clean, &strlConfigs, fcaNeverQueried, TRUE, TRUE,
			pConfig->GetHBuilder(), FALSE);

	}
	return bRetVal ? S_OK : S_FALSE;
}

STDMETHODIMP CBldPackage::XAutoBld::GetDefaultConfiguration(THIS_ LPDISPATCH* DefaultConfiguration)
{
	METHOD_PROLOGUE_EX(CBldPackage, AutoBld)
	HBLDTARGET hTarget = g_BldSysIFace.GetActiveTarget();
	HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);
	CString strTarget;
	if (!g_BldSysIFace.GetTargetName(hTarget, strTarget, hBld))
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTO_NODEFCFGNAME);
	}
	*DefaultConfiguration = CAutoConfiguration::Create(hBld, strTarget )->GetIDispatch(FALSE);
	return S_OK;
}

STDMETHODIMP CBldPackage::XAutoBld::SetDefaultConfiguration(THIS_ LPDISPATCH DefaultConfiguration)
{
	METHOD_PROLOGUE_EX(CBldPackage, AutoBld)

	// Try to get a CAutoConfiguration from the LPDISPATCH passed to us.
	CAutoConfiguration* pConfig = CAutoConfiguration::FromIDispatch((IConfiguration*)DefaultConfiguration);
	if (pConfig == NULL || !pConfig->IsKindOf(RUNTIME_CLASS(CAutoConfiguration)))
	{
		DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTO_BADCFG);
	}

	// Get the config's parent's hbuilder, the config's name & htarget, and
	//  make it all the default
	HBUILDER hBld = pConfig->GetHBuilder();
	if (hBld == NULL)
	{
		// This configuration no longer exists
		CString strPrompt;
		AfxFormatString1(strPrompt, IDS_AUTO_CFG_NOTEXIST, pConfig->GetConfigName());
		DsThrowOleDispatchException(DS_E_CONFIGURATION_NOT_VALID, (LPCTSTR) strPrompt);
	}
	pConfig->EnsureBuildable();		// Throws error if unsupported platform

	g_BldSysIFace.SetActiveTarget(g_BldSysIFace.GetTarget(pConfig->GetConfigName(), hBld), hBld);
	return S_OK;
}

// IUnknown methods

STDMETHODIMP_(ULONG) CEnumPlatforms::XEnumPlatforms::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CEnumPlatforms, EnumPlatforms)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CEnumPlatforms::XEnumPlatforms::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CEnumPlatforms, EnumPlatforms)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CEnumPlatforms::XEnumPlatforms::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CEnumPlatforms, EnumPlatforms)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IEnum methods

STDMETHODIMP CEnumPlatforms::XEnumPlatforms::Next(THIS_ ULONG celt, LPPLATFORM *rgelt, ULONG *pceltFetched)
{
	METHOD_PROLOGUE_EX(CEnumPlatforms, EnumPlatforms)
	return E_NOTIMPL;
}

STDMETHODIMP CEnumPlatforms::XEnumPlatforms::Skip(THIS_ ULONG celt)
{
	METHOD_PROLOGUE_EX(CEnumPlatforms, EnumPlatforms)
	return E_NOTIMPL;
}

STDMETHODIMP CEnumPlatforms::XEnumPlatforms::Reset(THIS)
{
	METHOD_PROLOGUE_EX(CEnumPlatforms, EnumPlatforms)
	return E_NOTIMPL;
}

STDMETHODIMP CEnumPlatforms::XEnumPlatforms::Clone(THIS_ IEnumPlatforms **ppenum)
{
	METHOD_PROLOGUE_EX(CEnumPlatforms, EnumPlatforms)
	return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CEnumBuildTools::XEnumBuildTools::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CEnumBuildTools, EnumBuildTools)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CEnumBuildTools::XEnumBuildTools::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CEnumBuildTools, EnumBuildTools)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CEnumBuildTools::XEnumBuildTools::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CEnumBuildTools, EnumBuildTools)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IEnum methods

STDMETHODIMP CEnumBuildTools::XEnumBuildTools::Next(THIS_ ULONG celt, LPBUILDTOOL *rgelt, ULONG *pceltFetched)
{
	METHOD_PROLOGUE_EX(CEnumBuildTools, EnumBuildTools)
	return E_NOTIMPL;
}

STDMETHODIMP CEnumBuildTools::XEnumBuildTools::Skip(THIS_ ULONG celt)
{
	METHOD_PROLOGUE_EX(CEnumBuildTools, EnumBuildTools)
	return E_NOTIMPL;
}

STDMETHODIMP CEnumBuildTools::XEnumBuildTools::Reset(THIS)
{
	METHOD_PROLOGUE_EX(CEnumBuildTools, EnumBuildTools)
	return E_NOTIMPL;
}

STDMETHODIMP CEnumBuildTools::XEnumBuildTools::Clone(THIS_ IEnumBuildTools **ppenum)
{
	METHOD_PROLOGUE_EX(CEnumBuildTools, EnumBuildTools)
	return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CEnumProjectTypes::XEnumProjectTypes::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CEnumProjectTypes, EnumProjectTypes)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CEnumProjectTypes::XEnumProjectTypes::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CEnumProjectTypes, EnumProjectTypes)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CEnumProjectTypes::XEnumProjectTypes::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CEnumProjectTypes, EnumProjectTypes)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// IEnum methods


STDMETHODIMP CEnumProjectTypes::XEnumProjectTypes::Next(THIS_ ULONG celt, LPPROJECTTYPE *rgelt, ULONG *pceltFetched)
{
	METHOD_PROLOGUE_EX(CEnumProjectTypes, EnumProjectTypes)
	ULONG i=0;
	while( pThis->m_pos && (i < celt) ){
		CProjType *pProjType = (CProjType *)pThis->m_pList->GetNext(pThis->m_pos);
		// add to list
		rgelt[i] = pProjType->GetInterface();
		i++;
	}
	*pceltFetched = i;
	return NOERROR;
}

STDMETHODIMP CEnumProjectTypes::XEnumProjectTypes::Skip(THIS_ ULONG celt)
{
	METHOD_PROLOGUE_EX(CEnumProjectTypes, EnumProjectTypes)
	return E_NOTIMPL;
}

STDMETHODIMP CEnumProjectTypes::XEnumProjectTypes::Reset(THIS)
{
	METHOD_PROLOGUE_EX(CEnumProjectTypes, EnumProjectTypes)
	pThis->m_pos = pThis->m_pList->GetHeadPosition();
	return NOERROR;
}

STDMETHODIMP CEnumProjectTypes::XEnumProjectTypes::Clone(THIS_ IEnumProjectTypes **ppenum)
{
	METHOD_PROLOGUE_EX(CEnumProjectTypes, EnumProjectTypes)
	return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////
// IPkgProjectProvider

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CBldPackage::XPkgProjectProvider::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CBldPackage, PkgProjectProvider)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBldPackage::XPkgProjectProvider::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CBldPackage, PkgProjectProvider)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CBldPackage::XPkgProjectProvider::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CBldPackage, PkgProjectProvider)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// PkgProjectProvider methods

CAppWizIFace* GetAppWizIFace();

// helper functions

extern HICON AfxLoadIcon(LPCTSTR lpResourceID);

static HICON GetProjectTypeIcon(const TCHAR* szProjectType)
{
	UINT nIconID = 0;
	
	if (_tcscmp(szProjectType, "Application") == 0)
		nIconID = IDR_APPLICATION;
	else if (_tcscmp(szProjectType, "Dynamic-Link Library") == 0)
		nIconID = IDR_DLL;
	else if (_tcscmp(szProjectType, "Console Application") == 0)
		nIconID = IDR_CONSOLE;
	else if (_tcscmp(szProjectType, "Static Library") == 0)
		nIconID = IDR_LIBRARY;
	else if (_tcscmp(szProjectType, "ASLM Shared Library") == 0)
		nIconID = IDR_ASLM;
	else if (_tcscmp(szProjectType, "Makefile") == 0)
		nIconID = IDR_EXTERNAL;
	else if (_tcscmp(szProjectType, "Generic Container") == 0)
		nIconID = IDR_EXTERNAL;
	else if (_tcscmp(szProjectType, "QuickWin Application") == 0)
		nIconID = IDR_QUICKWIN;
	else if (_tcscmp(szProjectType, "Standard Graphics Application") == 0)
		nIconID = IDR_STANDARDGRAPHICS;
	else if (_tcscmp(szProjectType, "Java Project") == 0)
		nIconID = IDR_JAVAWORKSPACE;
	else
		return AfxLoadIcon(szProjectType);

	ASSERT(nIconID != 0);
	return LoadIcon(GetResourceHandle(), MAKEINTRESOURCE(nIconID));
}

STDMETHODIMP CBldPackage::XPkgProjectProvider::AddNewProjectTypes(IProjectWorkspace *pPW)
{
	METHOD_PROLOGUE_EX(CBldPackage, PkgProjectProvider)

	g_BldSysIFace.LoadAppWiz();
	
	// put out new workspace dlg
	HWND hWnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	if (GetAppWizIFace())
		GetAppWizIFace()->InitAppWiz(hWnd);	// Even if RunAppWizSteps isn't called, we need to init. appwiz


	ASSERT(pThis->m_pType == NULL);
	pThis->m_pType = new CProjTypeList;
	if (GetAppWizIFace() != NULL)
	{
		// If C++ is around, then insert our friend AppWizard's project
		//  types at the top of projTypeList
		if (g_BldSysIFace.LanguageSupported(CPlusPlus) ) {
		    pThis->m_nNumApwzTypes = GetAppWizIFace()->AddAppWizProjectTypesAtTop(pThis->m_pType);
    
		    for (int i = 0; i < pThis->m_nNumApwzTypes; i += 1)
		    {
			    HICON hIcon = (HICON)pThis->m_pType->GetItemDataPtr(i);
			    CString str; pThis->m_pType->GetText(i, str);
			    pThis->m_pType->SetItemDataPtr(i, new CProjTypeListItem(str, hIcon, appWiz, i));
		    }
		}
		else
		{
		    // C++ is not around, so there are, sadly, no AppWizard types.
		    pThis->m_nNumApwzTypes = 0;
		}
	}
	else
	{
		// C++ is not around, so there are, sadly, no AppWizard types.
		pThis->m_nNumApwzTypes = 0;
	}

   	// Add in the build system types...
	const CStringList* pList = g_BldSysIFace.GetTargetTypeNames();
    POSITION pos = pList->GetHeadPosition();
    while (pos != NULL)
	{
		int nCurrentIndex = pThis->m_pType->GetCount();
		const TCHAR* szProjectType = pList->GetNext(pos);
		CString strExtTargetName;
		strExtTargetName.LoadString(IDS_EXTTARG_PROJTYPE);

		CString strType = szProjectType;
		CString strTypeDisplay;
		// prepend "Application", "Dynamic-Link Library", "Console Application", "Static Library" with "Win32 "
		if (!strType.Compare("Application") || !strType.Compare("Dynamic-Link Library") || !strType.Compare("Console Application") || !strType.Compare("Static Library"))
		{
			strType = "Win32 " + strType;
		}

#if 0
		CProjTypeListItem* pItem = new CProjTypeListItem(strType, GetProjectTypeIcon(szProjectType), 
			strcmp(szProjectType, strExtTargetName) == 0 ? makefile : buildSys, nCurrentIndex);
		VERIFY(pThis->m_pType->AddItem(pItem) == nCurrentIndex);
#endif
	}

	// Add Non Build Sys Types
	CPtrList typePtrList;
	pos = theApp.m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		CPackage* pPackage = (CPackage*)theApp.m_packages.GetNext(pos);
		pPackage->AddNewProjectTypes(typePtrList);
	}

	pos = typePtrList.GetHeadPosition();
	while (pos != NULL)
	{
		int nCurrentIndex = pThis->m_pType->GetCount();
		CProjTypeListItem *pItem = (CProjTypeListItem*)typePtrList.GetNext(pos);
		pItem->SetIndex(nCurrentIndex);
		VERIFY(pThis->m_pType->AddItem(pItem) == nCurrentIndex);
	}

	// Add in the custom app wizards...
	if (GetAppWizIFace())
	{
		int nFirstExtension = pThis->m_pType->GetCount();
		GetAppWizIFace()->AddCustomAppWizProjectTypesAtBottom(pThis->m_pType);

		int nItemCount = pThis->m_pType->GetCount();
		for (int i = nFirstExtension; i < nItemCount; i += 1)
		{
			HICON hIcon = (HICON)pThis->m_pType->GetItemDataPtr(i);
			CString str; pThis->m_pType->GetText(i, str);
			pThis->m_pType->SetItemDataPtr(i, new CProjTypeListItem(str, hIcon, customWiz, i));
		}
	}
	for(int i=0; i< pThis->m_pType->GetCount(); i++)
	{
		CProjTypeListItem *pPtli = (CProjTypeListItem *)pThis->m_pType->GetItemDataPtr(i);
		TYPE_ITEM_CLASS tic = pPtli->GetClass ();

		if (g_BldSysIFace.LanguageSupported(CPlusPlus) || (tic != appWiz))
		{
			pPW->AddProjectType(pPtli->GetIcon(), pPtli->GetText(), "dsp", 
							NULL, TRUE, TRUE, (DWORD)pPtli, this);
		}
	}

	return S_OK;
}

STDMETHODIMP CBldPackage::XPkgProjectProvider::ReleaseProjectTypeInfo()
{
	METHOD_PROLOGUE_EX(CBldPackage, PkgProjectProvider)
	delete pThis->m_pType;
	pThis->m_pType = NULL;
	if (GetAppWizIFace())
		GetAppWizIFace()->ExitAppWiz();
	return S_OK;
}

STDMETHODIMP CBldPackage::XPkgProjectProvider::OpenProject(LPCOLESTR szFileName, 
														   IPkgProject ** ppCreatedProject)
{
	METHOD_PROLOGUE_EX(CBldPackage, PkgProjectProvider)
	USES_CONVERSION;

	if (g_Spawner.SpawnActive())
	{
		AfxMessageBox(IDS_NO_OPEN_PRJ_BUILDING, MB_OK | MB_ICONEXCLAMATION);
		*ppCreatedProject = NULL;
		return ResultFromScode(E_FAIL);
	}

	TCHAR *pchBldPath = W2A( szFileName );

	g_fCreatingProject = TRUE;
	if (!g_BldSysIFace.OnOpenWorkspace(pchBldPath, FALSE, TRUE))
	{
		HRESULT hr;
		if (g_bBadConversion)
		{
			hr = ResultFromScode(E_UNEXPECTED);
			g_bBadConversion = FALSE;
		}
		else if (g_bConversionCanceled)
		{
			hr = ResultFromScode(E_ABORT);
			g_bConversionCanceled = FALSE;
			g_bConversionPrompted = FALSE;
		}
		else
		{
			hr = ResultFromScode(E_FAIL);
		}
		return hr;	// load failed
	}
	g_fCreatingProject = FALSE;
	HBUILDER hBld = (HBUILDER)g_pActiveProject;
	if (hBld == NO_BUILDER)
		return ResultFromScode(E_FAIL);	// load failed

	CProject *pProj = g_BldSysIFace.CnvHBuilder( hBld );
	LPBSPROJECT pIProj = pProj->GetInterface();
	pIProj->QueryInterface(IID_IPkgProject, (void **)ppCreatedProject );
	pIProj->Release();

	return S_OK;
}

// This is the registry key under which we remember which platforms were last checked
//  by the user.  By using the registry, we always remember the user's "favorite"
//  platforms from session to session.
// COLINT: Since this is no longer in AppWizard, you may want to change the key's name.
static const char szDefPltSection[] = "AppWizard\\Default Platforms";

BOOL CBldPackage::IsSelectedPlatform()
{
	int nCount = m_chklstPlatforms.GetSize();
	ASSERT(nCount <= MAX_PLATFORMS);
	if (nCount == 0)
		return TRUE;		// None to choose from

	BOOL bReturn = FALSE;
	for (int i=0; i < nCount; i++)
	{
		if (m_pnPlatforms[i])
			bReturn = TRUE;	// at least one is selected
	}

	return bReturn;
}

void CBldPackage::WriteRegPlatforms()
{
	int nCount = m_chklstPlatforms.GetSize();

	// Store the checked state of each platform in the registry.  
	for (int i = 0; i < nCount; i++)
	{
		CString strPlatform;
		WriteRegInt(szDefPltSection, m_chklstPlatforms[i], m_pnPlatforms[i]);
	}
}


STDMETHODIMP CBldPackage::XPkgProjectProvider::CanCreateProject(LPCOLESTR szFileName, 
								DWORD dwProject_ID,
								BOOL *pbCanCreate)
{
	METHOD_PROLOGUE_EX(CBldPackage, PkgProjectProvider)
	USES_CONVERSION;
	*pbCanCreate = FALSE;

	CProjTypeListItem *pItem = (CProjTypeListItem *)dwProject_ID;

	if (g_Spawner.SpawnActive())
	{
		AfxMessageBox(IDS_NO_CREATE_PRJ_BUILDING, MB_OK | MB_ICONEXCLAMATION);
		return S_OK;
	}

	if (!pThis->IsSelectedPlatform())
	{
		AfxMessageBox(IDS_SELECT_PLATFORM, MB_OK | MB_ICONEXCLAMATION);
		return S_OK;
	}

	// Write our registry info to use next time...
	pThis->WriteRegPlatforms();

    if (pItem->IsAnAppWizard())
	{
		LPCTSTR szAnsiFileName = W2A(szFileName);
		CPath path;
		CDir dir;
		CString strBaseName;
		// Warning. This does not work for "." and ".." project names
		path.Create(szAnsiFileName);
		dir.CreateFromPath(path);
		strBaseName = path.GetFileName();

		// This does allow ".." and "." but we run into problems later
		// CString strName = W2A(szFileName);
		// int iLength =  strName.GetLength();
		// int iSlash = strName.ReverseFind('\\');
		// CString strDir;
		// if (iSlash != -1) {
		//	strDir = strName.Left(iSlash+1);
		//	strBaseName = strName.Right( iLength - iSlash -1 );
		// }
		if (!GetAppWizIFace()->CanRunAppWizSteps(strBaseName, dir, pThis->m_pnPlatforms))
			return S_OK;
	}
	*pbCanCreate = TRUE;
	return S_OK;
}

STDMETHODIMP CBldPackage::XPkgProjectProvider::CreateProject(LPCOLESTR szFileName,
								DWORD dwProject_ID,
								IPkgProject ** ppCreatedProject,
								DWORD * pdwRetVal)
{
	METHOD_PROLOGUE_EX(CBldPackage, PkgProjectProvider)
	USES_CONVERSION;
	char *pchBldPath = W2A( szFileName );
	CProjTypeListItem *pItem = (CProjTypeListItem *)dwProject_ID;
	int nStatus = APPWIZTERMTYPE_CANCEL;
	CPath path;
	path.Create(pchBldPath);
	CString strBaseName;
	path.GetBaseNameString(strBaseName);
	CDir dirLocation;
	dirLocation.CreateFromPath(path);

	g_fCreatingProject = TRUE;

	*pdwRetVal = CREATE_PROJ_OK;
	HBUILDER hBld = NULL;

	if (pItem->IsAnAppWizard())
	{
		// The user selected an AppWizard project type, so run the appwiz steps
		nStatus = GetAppWizIFace()->RunAppWizSteps();
		if(nStatus == APPWIZTERMTYPE_RERUNPROMPTDLG)
		{
			*pdwRetVal = CREATE_PROJ_RERUN_PROMPT;
			g_fCreatingProject = FALSE;
			return S_OK;
		}
	}
	else
	{
		BOOL bGotoSettings = FALSE;

		if (pItem->GetClass() == makefile)
		{
			if ((g_pAutomationState->DisplayUI()) && //ShellOM:State
				(MsgBox(Question, IDS_NEW_MAKEFILE_INST, MB_YESNO) == IDYES))
			{
				bGotoSettings = TRUE;
			}
		}

//		Let's create an empty project and set up the selected platforms/configurations
		hBld = g_BldSysIFace.CreateBuilder( pchBldPath, FALSE /* add to current wkspace */,
										FALSE /* Create new */);

		// Add default targets based on target-type & platforms
		HBLDTARGET hDebugTarg = NO_TARGET, hReleaseTarg = NO_TARGET;

		// Go through each of the displayed platforms.  Add the appropriate
		//  targets for each platform the user chose.
		int nCount = pThis->m_chklstPlatforms.GetSize();
		BOOL fMultPlat = FALSE;

		for (int i=0; i < nCount; i++)
		{
			if (!pThis->m_pnPlatforms[i])
				continue;

			CString strPlatform = pThis->m_chklstPlatforms[i];
			CString strProjType = pItem->GetText();

			// special case: may need to strip of "Win32 " prefix
			if ((pItem->GetClass() == buildSys) &&  (!_tcsncmp(strProjType, "Win32 ", 6)))
				strProjType = strProjType.Mid(6);

			g_strTarg = (const TCHAR *)dirLocation;
			g_BldSysIFace.AddDefaultTargets(strPlatform, strProjType, hDebugTarg, hReleaseTarg, 
				TRUE, TRUE, TRUE, fMultPlat ? hReleaseTarg : NO_TARGET, ACTIVE_BUILDER, strBaseName);
			g_strTarg = "";
			fMultPlat = TRUE;
		}

		if (bGotoSettings)
			AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND, IDM_PROJECT_SETTINGS);

	}

	if(pItem->IsAnAppWizard())
	{
		// AppWizard has been run, so take cases on AppWizard's termination
		switch(nStatus)
		{
		case APPWIZTERMTYPE_RERUNPROMPTDLG:
			ASSERT(FALSE);	
			break;

		case APPWIZTERMTYPE_APPWIZPROJ:
			// Here AppWizard has created a project for us.
			{
				CPath MakFile;

				MakFile.Create(pchBldPath);
				MakFile.ChangeExtension(BUILDER_EXT);
				hBld = g_BldSysIFace.CreateBuilder(MakFile.GetFullPath(), FALSE, FALSE);
				g_strTarg = (const TCHAR *)dirLocation;
				GetAppWizIFace()->AddAppWizTargets(hBld);
				GetAppWizIFace()->GetAppWizDialog(g_strDialogName);
				g_strTarg = "";

			}
		break;

		case APPWIZTERMTYPE_CANCEL:
			// Here the user has canceled appwiz
			*pdwRetVal = CREATE_PROJ_CANCEL;
			break;

		default:
			ASSERT(FALSE);
			break;
		}
	}
	else
	{
//		case CREATOR_IDE:
			nStatus = APPWIZTERMTYPE_EMPTYPROJ;
			// The user has created an empty IDE project from the new project dialog (i.e., no appwiz)
	}

	if (nStatus == APPWIZTERMTYPE_CANCEL)
	{
		// enable file registry file change notification
		g_FileRegDisableFC = FALSE;
		g_fCreatingProject = FALSE;
		return S_OK;
	}

	//    must be an appwiz generated builder
	// OR must be an 'empty' builder
	//    (we will prompt to add files etc. later)
	ASSERT (nStatus == APPWIZTERMTYPE_APPWIZPROJ || 
			nStatus == APPWIZTERMTYPE_EMPTYPROJ);

	// do we have a valid builder?  If not, assume this is just a 
	// non-build system project type...
	if (hBld == NO_BUILDER)
	{
		// enable file registry file change notification
		g_FileRegDisableFC = FALSE;
		g_fCreatingProject = FALSE;
		return S_OK;
	}

	CProject *pProject = g_BldSysIFace.CnvHBuilder( hBld );

	// can't set this until after m_TreeWnd init above
	pProject->m_bPrivateDataInitialized = TRUE;

  	// save project
	pProject->m_bProjMarkedForSave = TRUE;

	// enable file registry file change notification
	g_FileRegDisableFC = FALSE;

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
		//if (!bProjectDirty) 
			// This scan should in fact not dirty the project (only if the project is not marked
			// dirty :
			//pProject->InformDependants(SN_PROJ_PUBLICS_CLEAN);

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
				if (!pProject->DoSaveAs())
				{
					// user has be alerted to what failed in OnOpenDocument
					TRACE0("CDocument::OnOpenDocument returned FALSE due to failed convert\n");

					// set back the current directory since it fails to open
					g_bWriteProject = FALSE;
					g_fCreatingProject = FALSE;
					return S_OK;
				}
			}
			else if (!bWasReadOnly)
			{
				// Re-save project in verbose format
				// FUTURE (karlsi): should do this quietly, if possible
				g_bWriteProject = TRUE;
				pProject->DoSave();
			}
		
			pProject->m_bProjMarkedForSave = FALSE;

			// try to put new project under source control
//			if (pSccManager->AddNewProj() == S_OK)
//				pSccManager->OpenProject(NULL, TRUE);
		}
		else
		{
			// init/create source control project
//			if (pSccManager->AddNewProj() == S_OK)
//				pSccManager->OpenProject(NULL, TRUE);
		}
	}

	if( !g_strDialogName.IsEmpty() ) {
		AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND, IDM_PROJECT_OPENDIALOG);
	}

	LPBSPROJECT pBSProj = pProject->GetInterface();
	pBSProj->QueryInterface(IID_IPkgProject, (void **)ppCreatedProject );
	pBSProj->Release();

	g_fCreatingProject = FALSE;
	return S_OK;
}

STDMETHODIMP CBldPackage::XPkgProjectProvider::GetPlatforms(DWORD dwProject_ID,
								LPCOLESTR ** ppszTargetNames,
								BOOL ** pDefaultSelections,
								ULONG *pCount)
{
	METHOD_PROLOGUE_EX(CBldPackage, PkgProjectProvider)

	*pCount = 0;
	CProjTypeListItem* pItem = (CProjTypeListItem *)dwProject_ID;
	// First, remember the check states of the platforms
	if (pThis->m_chklstPlatforms.GetSize() != 0)
		pThis->WriteRegPlatforms();

	pThis->m_chklstPlatforms.RemoveAll();
	switch (pItem->GetClass())
	{
	default:
		// Leave it empty for project types not managed by the build system...
		break;

	case appWiz:
		{
			// Delegate filling up the checklist to AppWizard
			GetAppWizIFace()->FillPlatformsListFromAppWizType(pItem->GetIndex(), &pThis->m_chklstPlatforms);
		}
		break;

	case customWiz:
		{
			// Delegate filling up the checklist to AppWizard
			CString strExtensionName;
			strExtensionName = pItem->GetText();

			// This call will throw an exception if there was a problem loading the custom
			//  AppWizard (invalid DLL, didn't have the right exports, etc.)  In that case,
			//  an error message has been displayed, and the exception caught in our caller.
			GetAppWizIFace()->FillPlatformsListFromCustomAppWizType(strExtensionName, &pThis->m_chklstPlatforms);
		}
		break;

	case buildSys:
	case makefile:
		{
			// Fill with list IDE gives us
			const CStringList* pList = g_BldSysIFace.GetTargetTypeNames();

			const CStringList* pPlatformsUI = g_BldSysIFace.GetAvailablePlatforms
				(pList->GetAt(pList->FindIndex(pItem->GetIndex() - pThis->m_nNumApwzTypes)), TRUE);

			POSITION posUI = pPlatformsUI->GetHeadPosition();
			while (posUI != NULL)
			{
				CString strUI = pPlatformsUI->GetNext(posUI);
				int i = pThis->m_chklstPlatforms.Add(strUI);
				int nCheck = GetRegInt(szDefPltSection, strUI, 1);
				pThis->m_pnPlatforms[i] = nCheck;
			}
		}
		break;
	}

	int nCount = pThis->m_chklstPlatforms.GetSize();
	if (pItem->IsAnAppWizard())
	{
		// If we delegated to AppWizard, we haven't yet had a chance to
		//  make sure the proper default platforms are checked.  Do it now.
		for (int i=0; i < nCount; i++)
		{
			int nCheck = GetRegInt(szDefPltSection, pThis->m_chklstPlatforms[i], 1);
			pThis->m_pnPlatforms[i] = nCheck;
		}
	}

	// We are going to now create a UNICODE version of the platforms
	*ppszTargetNames = (LPCOLESTR *)CoTaskMemAlloc(sizeof(LPOLESTR *) * (nCount+1));
	for(int i=0; i<nCount; i++)
	{
		USES_CONVERSION;
		LPOLESTR wstr = A2W(pThis->m_chklstPlatforms[i]);
		(*ppszTargetNames)[i] = (LPCOLESTR)CoTaskMemAlloc((wcslen(wstr)+1) * sizeof(wchar_t));
		wcscpy((LPOLESTR)(*ppszTargetNames)[i], wstr);
	}
	ASSERT(nCount <= MAX_PLATFORMS);
	*pCount = nCount;
	*pDefaultSelections = pThis->m_pnPlatforms;
	return S_OK;
}

STDMETHODIMP CBldPackage::XPkgProjectProvider::ChangePlatformSelection(ULONG uTargetIndex, 
																	BOOL bSelected)
{
	METHOD_PROLOGUE_EX(CBldPackage, PkgProjectProvider)
	
	ASSERT(uTargetIndex < MAX_PLATFORMS );
	pThis->m_pnPlatforms[uTargetIndex] = bSelected;
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// ISetJavaProjectInfo

////////////////////////////////////////////////////////////////////////////
// IUnknown methods
STDMETHODIMP_(ULONG) CBldPackage::XSetJavaProjectInfo::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CBldPackage, SetJavaProjectInfo)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBldPackage::XSetJavaProjectInfo::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CBldPackage, SetJavaProjectInfo)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CBldPackage::XSetJavaProjectInfo::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CBldPackage, SetJavaProjectInfo)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

////////////////////////////////////////////////////////////////////////////
// ISetJavaProjectInfo methods

STDMETHODIMP_(BOOL) CBldPackage::XSetJavaProjectInfo::SetPrimaryClassName(CString& strProjName, CString& strPrimaryClassName)
{
	// only bother to set the class name if it's not empty
	if (!strPrimaryClassName.IsEmpty() && !strProjName.IsEmpty()) {

		// get the current project
		CProject * pProject = g_pActiveProject;
		//ASSERT(pProject != NULL);
		if (pProject != NULL) {

			// this is so lame!
			// What we really want to do here is get the configurations for the project we just
			// created and set the P_Java_ClassName property for them.
			// This doesn't appear to be possible.
			// So what we do is make sure the configuration name contains the project name.
			// But, in order to not find matches like a project named 'java' contained in configuration
			// named 'javawiz - platform' is add " -" to the project name given.
			// But this assumes that all configurations are of the form "project name - platform".
			// In order to validate that assumption, we assert that each configuration name contains
			// a '-'.
			strProjName += _T(" -");

			// go through all configurations for this project and set the class name
			const CPtrArray* pConfigArray = pProject->GetConfigArray();
			ASSERT(pConfigArray != NULL);
			for (int iConfig = 0; iConfig < pConfigArray->GetSize(); iConfig++) {

				ConfigurationRecord* pConfig = (ConfigurationRecord*)pConfigArray->GetAt(iConfig);

				// make sure this configuration is the one we want
				CString strConfigName = pConfig->GetConfigurationName();
				// make sure configuration name contains '-'
				// (if this assert fires, see big comment above)
				ASSERT(strConfigName.Find(_T('-')) != -1);
				if (strConfigName.Find(strProjName) != -1) {

					CProjTempConfigChange projTempConfigChange(pProject);
					projTempConfigChange.ChangeConfig(pConfig);
				
					// set the class name property
					pProject->SetStrProp(P_Java_ClassFileName, strPrimaryClassName);
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}
