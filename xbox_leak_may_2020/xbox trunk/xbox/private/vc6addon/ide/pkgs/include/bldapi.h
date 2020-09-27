/////////////////////////////////////////////////////////////////////////////
//	BLDAPI.H
//		Build package interface declarations.

#ifndef __BLDAPI_H__
#define __BLDAPI_H__

#include "slob.h"


interface IBuildProject;		// Defined in pkgs\include\auto\bldauto.h, used in
						// IBuildWizard::GetAutoProject.
interface IPkgProject;

#ifndef STRICT
typedef UINT HBUILDER;
typedef UINT HBLDTARGET;
typedef UINT HFILESET;
typedef UINT HBLDFILE;
typedef UINT HPROJDEP;
typedef UINT HFOLDER;
#else	// STRICT
DECLARE_HANDLE(HBUILDER);
DECLARE_HANDLE(HBLDTARGET);
DECLARE_HANDLE(HFILESET);
DECLARE_HANDLE(HBLDFILE);
DECLARE_HANDLE(HPROJDEP);
DECLARE_HANDLE(HFOLDER);
#endif	// STRICT

#define ACTIVE_BUILDER	(HBUILDER)NULL
#define ACTIVE_TARGET	(HBLDTARGET)NULL
#define ACTIVE_FILESET	(HFILESET)NULL

#define NO_BUILDER		(HBUILDER)NULL
#define NO_TARGET		(HBLDTARGET)NULL
#define NO_FILESET		(HFILESET)NULL
#define NO_FILE			(HBLDFILE)NULL
#define NO_PROJDEP		(HPROJDEP)NULL
#define NO_FOLDER		(HFOLDER)NULL

// default extension for builder file
#define BUILDER_EXT	".dsp"

// Option switches for AddFile's fSettings argument
//NYI: use target's default settings
#define ADB_USE_DEFAULT_SETTINGS	0x01

#define ADB_EXCLUDE_FROM_SCAN		0x04
#define ADB_EXCLUDE_FROM_BUILD		0x08

typedef enum {TrgDefault, TrgCopy, TrgMirror} TrgCreateOp;
typedef enum {SettingsDefault, SettingsClone} SettingOp;
typedef enum {ExeBuilder, InternalBuilder} BuilderType;
typedef enum {fcaNeverQueried, fcaRebuild, fcaIgnore} FlagsChangedAction;

// state of targets
#define TRG_STAT_Unknown			0x0
#define TRG_STAT_Dep_Missing		0x1
#define TRG_STAT_Out_Of_Date		0x2
#define TRG_STAT_Current			0x4

// Target attributes
typedef enum
{
	// Image Types:
	ImageStaticLib		= 0x1,
	ImageDLL			= 0x2,
	ImageExe			= 0x4,
	// Subsystems:
	SubsystemConsole	= 0x8,
	SubsystemWindows	= 0x10,
	// Can we debug the resulting target?
	TargetIsDebugable	= 0x20,
	// Is this an unknown target?
	TargetUnknown		= 0x40,
	
	ImageJava           = 0x80,
    ImageXbe            = 0x100,
} TargetAttributes;

typedef enum
{
	unknown_platform	= 0,	// Not yet used -- error condition!
	win32x86			= 1,
	win16x86			= 2,
	mac68k				= 3,
	macppc				= 4,
	win32mips			= 5,
	win32alpha			= 6,
	win32ppc			= 7,
    java                = 8,
    generic             = 9,
    vistest             = 10,
    xbox                = 11,
} uniq_platform;

typedef enum
{
	unknown_projtype = 0,
	application      = 1,
	dynamiclib       = 2,
	consoleapp       = 3,
	staticlib        = 4,
	sharedlib        = 5,
	exttarget        = 6,
	quickwin         = 7,
	standardgraphics = 8,
	javaapplet       = 9,
} uniq_projtype;

// notification info structure, filled with info about the
// notification (e.g. adding files to a particular target).
typedef struct tagNOTIFYINFO
{
	HFILESET hFileSet;			// fileset (null if not bld pkg)
	CPathList lstFiles;			// regular files (part of fileset)
	CPathList lstDepFiles;		// dependency files (not part of fileset)
	CStringList lstOtherFiles;	// other non-buildable files (other packages)
	IPkgProject *pPkgProject;	// IPkgProject (not refcounted)
} NOTIFYINFO;

// notification info structure, filled with info about the
// file that has been renamed
typedef struct tagNOTIFYRENAME
{
	CPath OldFileName;
	CPath NewFileName;
} NOTIFYRENAME;

// merged from vinfoapi.h
#define CCHMAX_TARGET_FILE_NAME 	50
#define CCHMAX_TARGET_PRODUCT_NAME 	100

typedef struct tagPROJECT_VERSION_INFO
{
	DWORD	dwBuildType;
	DWORD	dwTargetType;
	DWORD	dwTargetSubtype;
	DWORD	dwTargetOS;
	DWORD	dwVersionNumberLS;
	DWORD	dwVersionNumberMS;
	char	szTargetFileName [CCHMAX_TARGET_FILE_NAME];
	char	szTargetProductName [CCHMAX_TARGET_PRODUCT_NAME];
} PROJECT_VERSION_INFO;

class CImageWell;		// Non-standard COM.
class CBldrSection;		// Non-standard COM.

// callback for builder file 'sections'
// 'pbldsect' is the section data
// 'op' is the callback operation

#define SECT_READ 0x0	// writing (supply buffer)
#define SECT_WRITE 0x1	// reading (process buffer)
#define SECT_END 0x2	// ended read or write (free buffer?)

// callback used for synchronous notification of read/write
typedef BOOL (*PfnSectionCallBack)(CBldrSection * pbldsect, UINT op);

class CBldrSection	// Non-standard COM.
{
public:
	CBldrSection(HBUILDER hBld = ACTIVE_BUILDER) {m_pfn = NULL; m_pch = NULL; m_cch = 0; m_dw = 0; m_hBld = hBld;}

	CString m_strName;			// section name
	
	TCHAR * m_pch;				// char. buffer
	UINT m_cch;					// size of buffer (not used for write)
	// o format of this data is each line zero-terminated with a double terminator
	//   signifying the end of the buffer 

	PfnSectionCallBack m_pfn;	// our callback (if NULL, assume async retrieval)
	DWORD m_dw;					// user-supplied word
	HBUILDER m_hBld;
};

// enumerate files in a fileset filters
#define FileEnum_All			0x0
#define FileEnum_RemoveExcluded	0x1
#define FileEnum_RemoveDeps		0x2

// transport layer information flags
#define TFL_NIL		0x0000			// Nil value
#define TFL_LOCAL	0x0001			// Local TL (else remote)
#define TFL_CFGABLE	0x0002			// Configurable (else not)

#define IDFILE_COMPILER_PDB		0x0	// full path of the compiler .pdb
#define IDFILE_COMPILER_PCH		0x1	// full path of the compiler .pch
#define IDFILE_COMPILER_IDB		0x2	// full path of the compiler .idb

// Constants used and returned by the Java build system API
#define Java_DebugUsing_Unknown			-1
#define Java_DebugUsing_Browser			1
#define Java_DebugUsing_Standalone		2
#define Java_ParamSource_Unknown		-1
#define Java_ParamSource_HTMLPage		1
#define Java_ParamSource_User			2
#define Java_DebugStandalone_Unknown	-1
#define Java_DebugStandalone_Applet		1
#define Java_DebugStandalone_Application	2

// old way
interface IBuildSystem;
interface IBuildWizard;
interface IBuildPlatforms;
interface IBuildComponents;
interface IBuildStatus;
interface IBuildDirManager;

typedef IBuildSystem* LPBUILDSYSTEM;
typedef IBuildWizard* LPBUILDWIZARD;
typedef IBuildPlatforms* LPBUILDPLATFORMS;
typedef IBuildComponents* LPBUILDCOMPONENTS;
typedef IBuildStatus* LPBUILDSTATUS;
typedef IBuildDirManager* LPBUILDDIRMANAGER;

// new way
interface IBSProject;
interface IBSProjectFileManager;
interface IBSRemoteProject;
interface IBSOptionManager;
interface IEnumConfigurations;
interface IEnumBSProjects;
interface IEnumBSActions;
interface IBSAction;
interface IEnumBuildFile;
interface IBuildSystemAddOn;
interface IEnumBuildTools;
interface IEnumProjectTypes;
interface IEnumPlatforms;
interface IBuildTool;
interface IDepScanner;
interface IOptionHandler;
interface IEnumOptionStrings;
interface IEnumOptionTypes;
interface IProjectType;
interface IPlatform;
interface ICPPPlatform;
interface ISetJavaProjectInfo;

interface IBuildFile;
interface IBuildFileSet;
#if 0
typedef IBuildFile *				LPBUILDFILE;
typedef IBuildFileSet *				LPBUILDFILESET;
#else
class CFileRegFile;
class CFileRegSet;
typedef CFileRegFile *			LPBUILDFILE;
typedef CFileRegSet *				LPBUILDFILESET;
#endif

typedef IBSProject *				LPBSPROJECT;
typedef IBSProjectFileManager *		LPBSPROJECTFILEMANAGER;
typedef IBSRemoteProject *			LPBSREMOTEPROJECT;
typedef IBSOptionManager *			LPBSOPTIONMANAGER;
typedef IEnumConfigurations *		LPENUMCONFIGURATIONS;
typedef IEnumBSProjects *			LPENUMBSPROJECTS;
typedef IEnumBSActions *			LPENUMBSACTIONS;
typedef IBSAction *					LPBSACTION;
typedef IEnumBuildFile *			LPENUMBUILDFILE;
typedef IBuildSystemAddOn *			LPBUILDSYSTEMADDON;
typedef IEnumBuildTools *			LPENUMBUILDTOOLS;
typedef IEnumProjectTypes *			LPENUMPROJECTTYPES;
typedef IEnumPlatforms *			LPENUMPLATFORMS;
typedef IBuildTool *				LPBUILDTOOL;
typedef IDepScanner *				LPDEPSCANNER;
typedef IOptionHandler *			LPOPTIONHANDLER;
typedef IEnumOptionStrings *		LPENUMOPTIONSTRINGS;
typedef IEnumOptionTypes *			LPENUMOPTIONTYPES;
typedef IProjectType *				LPPROJECTTYPE;
typedef IPlatform *					LPPLATFORM;
typedef ICPPPlatform *				LPCPPPLATFORM;
typedef ISetJavaProjectInfo*		LPSETJAVAPROJECTINFO;

typedef void		*HCONFIGURATION;

/////////////////////////////////////////////////////////////////////////////
//	IBuildSystem interface

#undef  INTERFACE
#define INTERFACE IBuildSystem

DECLARE_INTERFACE_(IBuildSystem, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBuildSystem methods
	STDMETHOD(GetActiveBuilder)(HBUILDER *pHandle) PURE;
	STDMETHOD(IsActiveBuilderValid)() PURE;
	STDMETHOD(GetBuilderType)(HBUILDER hBld, int *pBuilderType) PURE;
	STDMETHOD(GetBuilderFile)(HBUILDER hBld, LPCSTR *ppszPath) PURE;
	STDMETHOD(CreateBuilder)(LPCSTR pchBldPath, BOOL fForce, BOOL fOpen, HBUILDER *pHandle) PURE;
	STDMETHOD(SetDirtyState)(HBUILDER hBld, BOOL fDirty) PURE;
	STDMETHOD(GetDirtyState)(HBUILDER hBld) PURE;
	STDMETHOD(SaveBuilder)(HBUILDER hBld) PURE;
	STDMETHOD(CloseBuilder)(HBUILDER hBld) PURE;
	STDMETHOD(OnOpenWorkspace)(LPCSTR pszWorkspaceName, BOOL bTemporary) PURE;
	STDMETHOD(OnDefaultWorkspace)(LPCSTR pszFileToAdd) PURE;
	STDMETHOD(OpenWorkspace)(LPCSTR pszWorkspace) PURE;
	STDMETHOD(GetFileSetFromTargetName)(LPCSTR pchFileSet, HBUILDER hBld, HFILESET *pHandle) PURE;
	STDMETHOD(GetTargetNameFromFileSet)(HFILESET hFileSet, CString &str, BOOL bInvalid, HBUILDER hBld) PURE;	// Non-standard COM.
	STDMETHOD(GetFileSetName)(HFILESET hFileSet, CString &str, HBUILDER hBld) PURE;	// Non-standard COM.
	STDMETHOD(GetTargetFromFileSet)(HFILESET hFileSet, HBLDTARGET &hTarg, BOOL bInvalid, HBUILDER hBld) PURE;
	STDMETHOD(GetActiveTarget)(HBUILDER hBld, HBLDTARGET *pHandle) PURE;
	STDMETHOD(GetTarget)(LPCSTR pchTarg, HBUILDER hBld, HBLDTARGET *pHandle) PURE;
	STDMETHOD(GetTargetName)(HBLDTARGET hTarg, CString &str, HBUILDER hBld) PURE;	// Non-standard COM.
	STDMETHOD(DeleteTarget)(HBLDTARGET hTarg, HBUILDER hBld) PURE;
	STDMETHOD(AddProjectDependency)(HFILESET hFileSet, HFILESET hDependantFileSet, HBUILDER hBld, HPROJDEP *pHandle) PURE;
	STDMETHOD(RemoveProjectDependency)(HPROJDEP hProjDep, HFILESET hFileSet, HBUILDER hBld) PURE;
	STDMETHOD(GetDependantProject)(HFILESET *pHandle, HPROJDEP hProjDep, HFILESET hFileSet, HBUILDER hBld) PURE;
	STDMETHOD(GetTargetDirectory)(HBLDTARGET hTarg, CString &strDir, HBUILDER hBld) PURE;	// Non-standard COM.
	STDMETHOD(GetTargetFileName)(HBUILDER hBld, LPTSTR *ppszPath) PURE;
	STDMETHOD(GetTargetAttributes)(HBUILDER hBld, int *piAttrs) PURE;
	STDMETHOD(MakeTargPropsDefault)(HBLDTARGET hTarg, HBUILDER hBld) PURE;
	STDMETHOD(CheckFile)(HFILESET hFileSet, CPath &pathFile, HBUILDER hBld) PURE;	// Non-standard COM.
	STDMETHOD(AddFolder)(HFOLDER *pHandle, HFILESET hFileSet, LPCSTR pszFolder, HBUILDER hBld, HFOLDER hFolder, LPCSTR pszFilter) PURE;
	STDMETHOD(AddFile)(HBLDFILE *pHandle, HFILESET hFileSet, const CPath *ppathFile, 	// Non-standard COM.
		int fSettings, HBUILDER hBld, HFOLDER hFolder) PURE;
	STDMETHOD(AddDependencyFile)(HFILESET hFileSet, const CPath *ppathFile, HBUILDER hBld, HFOLDER hFolder) PURE;	// Non-standard COM.
	STDMETHOD(DelFile)(HBLDFILE hFile, HFILESET hFileSet, HBUILDER hBld) PURE;
	STDMETHOD(GetFile)(const CPath *ppathFile, HBLDFILE &hFile, HFILESET hFileSet, 	// Non-standard COM.
		HBUILDER hBld, BOOL bSimple) PURE;
	STDMETHOD(SetFileProp)(HBLDTARGET hTarg, HBLDFILE hFile, UINT idProp, int i) PURE;
	STDMETHOD(SetFileProp)(HBLDTARGET hTarg, HBLDFILE hFile, UINT idProp, CString &str) PURE;	// Non-standard COM.
	STDMETHOD(MakeFilePropsDefault)(HBLDTARGET hTarg, HBLDFILE hFile) PURE;
	STDMETHOD(TargetState)(UINT *puState, CObList *plstPath, 	// Non-standard COM.
		LPCSTR pchTarget, HBLDTARGET hTarg, HBUILDER hBld) PURE;
	STDMETHOD(FileState)(UINT *puState, const CPath *pathFile) PURE;	// Non-standard COM.
	STDMETHOD(ResolveIncludeDirectives)(const CPath &pathSrc, const CStringList &strlstIncs, 
		CStringList &lstPath, BOOL fIgnStdIncs, const CString &strPlat) PURE;	// Non-standard COM.
	STDMETHOD(ResolveIncludeDirectives)(const CDir &dirBase, const CStringList &strlstIncs,
		CStringList &lstPath, BOOL fIgnStdIncs, const CString &strPlat) PURE;	// Non-standard COM.
	STDMETHOD(ResolveIncludeDirectives)(const CObList &lstpathSrc, const CStringList &strlstIncs,
		CStringList &lstPath, HBLDTARGET hTarget) PURE;	// Non-standard COM.
	STDMETHOD(GetIncludePath)(CObList &lstIncDirs, const CString &strPlat, const CPath *ppathSrc, const CString &strTool, HBUILDER hBld ) PURE;	// Non-standard COM.
	STDMETHOD(UpdateDependencyList)(const CPath &pathSrc, const CStringList &strlstIncs,	// Non-standard COM.
		HBLDTARGET hTarg, HBUILDER hBld) PURE;
	STDMETHOD(InitBuilderEnum)() PURE;
	STDMETHOD(GetNextBuilder)(CString &strTarget, BOOL bOnlyLoaded, HBUILDER *pHandle) PURE;
	STDMETHOD(InitTargetEnum)(HBUILDER hBld) PURE;
	STDMETHOD(GetNextTarget)(HBUILDER hBld, CString &strTarget, HBLDTARGET *pHandle) PURE;	// Non-standard COM.
	STDMETHOD(InitProjectDepEnum)(HBUILDER hBld, HFILESET hFileSet) PURE;
	STDMETHOD(GetNextProjectDep)(HBUILDER hBld, HFILESET hFileSet, HPROJDEP *pHandle) PURE;
	STDMETHOD(InitFileSetEnum)(HBUILDER hBld) PURE;
	STDMETHOD(GetNextFileSet)(HBUILDER hBld, HFILESET *pHandle) PURE;
	STDMETHOD(InitFileEnum)(HFILESET hFileSet, UINT filter) PURE;
	STDMETHOD(GetNextFile)(HFILESET hFileSet, FileRegHandle & frh, HBLDFILE *pHandle) PURE;
	STDMETHOD(GetFileSetTargets)(HFILESET hFileSet, CStringList &strlstTargs) PURE;	// Non-standard COM.
	STDMETHOD(GetFileSet)(HBUILDER hBld, HBLDTARGET hTarg, HFILESET *pHandle) PURE;
	STDMETHOD(GetBuildFile)(UINT idFile, LPSTR szFileName, int cchFileBuffer,
		HBLDTARGET hTarg, HBUILDER hBld) PURE;
	STDMETHOD(RegisterSection)(CBldrSection * pbldsect) PURE;	// Non-standard COM.
	STDMETHOD(DeregisterSection)(const CBldrSection * pbldsect) PURE;	// Non-standard COM.
	STDMETHOD(InitSectionEnum)() PURE;
	STDMETHOD(GetNextSection)(CBldrSection **ppSection, CString &strSection) PURE;	// Non-standard COM.
	STDMETHOD(FindSection)(CBldrSection **ppSection, LPCSTR pchName) PURE;	// Non-standard COM.
	STDMETHOD(GetResourceFileList)(CPtrList &listRcPath, BOOL fOnlyBuildable, 	// Non-standard COM.
		HFILESET hFileSet, HBUILDER hBld) PURE;
	STDMETHOD(GetResourceIncludePath)(CPath *pResFile, CString &strIncludes) PURE; 	// Non-standard COM.
	STDMETHOD(LanguageSupported)(UINT idLang) PURE;
	STDMETHOD(GetSelectedFile)(CPath *path) PURE;	// Non-standard COM.
	STDMETHOD(GetProjectDir)(HBLDTARGET HBLDTARGET, HBUILDER hBld, CString& str) PURE;	// Non-standard COM.
	STDMETHOD(GetWorkspaceDir)(HBUILDER hBld, CString& str) PURE;	// Non-standard COM.
	STDMETHOD(BuildSystemEnabled)() PURE;
	STDMETHOD(IsBuildInProgress)() PURE;
	STDMETHOD(GetBuilderToolset)(int *piToolset, HBUILDER hBuilder) PURE;
	STDMETHOD(GetRegistryEntryPath)(FileRegHandle frh, const CPath **ppPath) PURE;	// Non-standard COM.
	STDMETHOD(GetRemoteTargetFileName)(HBUILDER hBld, CString &str) PURE;	// Non-standard COM.
	STDMETHOD(SetRemoteTargetFileName)(HBUILDER hBld, CString &str) PURE;	// Non-standard COM.
	STDMETHOD(GetSlobWnd)(HBUILDER hBld, CWnd **ppWnd) PURE;	// Non-standard COM.
	STDMETHOD(IsProjectFile)(const CPath *pPath, BOOL bIncludeDeps, HBUILDER hBld) PURE;	// Non-standard COM.
	STDMETHOD(UpdateBrowserDatabase)(HBUILDER hBld) PURE;
	STDMETHOD(GetBrowserDatabaseName)(HBUILDER hBld, const CPath **ppPath) PURE;	// Non-standard COM.
	STDMETHOD(GetCallingProgramName)(HBUILDER hBld, CString &str) PURE;	// Non-standard COM.
	STDMETHOD(GetProgramRunArguments)(HBUILDER hBld, CString &str) PURE;	// Non-standard COM.
	STDMETHOD(SetProgramRunArguments)(HBUILDER hBld, CString &str) PURE;	// Non-standard COM.
	STDMETHOD(GetWorkingDirectory)(HBUILDER hBld, CString &str) PURE;	// Non-standard COM.
	STDMETHOD(TargetIsCaller)(HBUILDER hBld) PURE;
	STDMETHOD(GetPromptForDlls)(HBUILDER hBld) PURE;
	STDMETHOD(SetPromptForDlls)(HBUILDER hBld, BOOL bPrompt) PURE;
	STDMETHOD(SetProjectState)(HBUILDER hBld) PURE;
	STDMETHOD(GetClassWizAddedFiles)(HBUILDER hBld) PURE;
	STDMETHOD(SetClassWizAddedFiles)(HBUILDER hBld, BOOL bPrompt) PURE;
	STDMETHOD(ScanAllDependencies)(HBUILDER hBld) PURE;
	STDMETHOD(GetVersionInfo)(HBUILDER hBld, void *ppvi) PURE;
	STDMETHOD(GetInitialExeForDebug)(HBUILDER hBld, BOOL bExecute) PURE;
	STDMETHOD(GetInitialRemoteTarget)(HBUILDER hBld, BOOL bBuild, BOOL fAlways) PURE;
	STDMETHOD(UpdateRemoteTarget)(HBUILDER hBld) PURE;
	STDMETHOD(DoTopLevelBuild)(HBUILDER hBld, BOOL bBuildAll, CStringList *pConfigs,	// Non-standard COM.
		FlagsChangedAction fca, BOOL bVerbose, BOOL bClearOutputWindow) PURE;
	STDMETHOD(GetEnvironmentVariables)(HBUILDER hBld, void *pEnvList) PURE;
	STDMETHOD(IsProfileEnabled)(HBUILDER hBld) PURE;
	STDMETHOD(IsScanableFile)(HBLDFILE hFile) PURE;
	STDMETHOD(GetBuilder)(HBLDTARGET hTarg, HBUILDER *pHandle) PURE;
	STDMETHOD(GetBuilderFromFileSet)(HFILESET hFileSet, HBUILDER *pHandle) PURE;
	// Additions
	STDMETHOD(EnumBuildTools)( LPENUMBUILDTOOLS *, LPBUILDTOOL * ) PURE;
	STDMETHOD(EnumProjectTypes)( LPENUMPROJECTTYPES *, LPPROJECTTYPE * ) PURE;
	STDMETHOD(EnumPlatforms)( LPENUMPLATFORMS *, LPPLATFORM * ) PURE;
	STDMETHOD(GetCommandLineOptions)( HFILESET hFileSet, HBLDFILE hFile, CString &rval ) PURE;

	STDMETHOD(GetSccProjName)(HBUILDER, CString&) PURE;
	STDMETHOD(SetSccProjName)(HBUILDER, const CString&) PURE;
	STDMETHOD(GetSccRelLocalPath)(HBUILDER, CString&) PURE;
	STDMETHOD(SetSccRelLocalPath)(HBUILDER, const CString&) PURE;
//	STDMETHOD(GetSccAuxPath)(HBUILDER, CString&) PURE;
//	STDMETHOD(SetSccAuxPath)(HBUILDER, const CString&) PURE;

	// Java support
	STDMETHOD(GetJavaClassName)(HBUILDER hBld, CString& str) PURE;
	STDMETHOD(GetJavaClassFileName)(HBUILDER hBld, CString& str) PURE;
	STDMETHOD(GetJavaDebugUsing)(HBUILDER hBld, ULONG* pDebugUsing) PURE;
	STDMETHOD(GetJavaBrowser)(HBUILDER hBld, CString& str) PURE;
	STDMETHOD(GetJavaStandalone)(HBUILDER hBld, CString& str) PURE;
	STDMETHOD(GetJavaStandaloneArgs)(HBUILDER hBld, CString& str) PURE;
	STDMETHOD(GetJavaHTMLPage)(HBUILDER hBld, CString& str) PURE;
	STDMETHOD(GetJavaClassPath)(HBUILDER hBld, CString& str) PURE;
	STDMETHOD(GetJavaStandaloneDebug)(HBUILDER hBld, ULONG* pStandaloneDebug) PURE;
	STDMETHOD(GetInitialJavaInfoForDebug)(HBUILDER hBld, BOOL bExecute) PURE;
	// more additions
	STDMETHOD(GetPreCompiledHeaderName)(HBUILDER hBld, CString &strPch) PURE;
	STDMETHOD(GetPkgProject)(HBUILDER builder, IPkgProject** outPkgProject) PURE;
	STDMETHOD(GetProjectGlyph)(HBUILDER hBld, HFILESET hFileSet, CImageWell& imageWell, UINT * pnImage) PURE;

	// Addition for Rational VM - ShankarV
	STDMETHOD(GetBuilderFromName)(LPCTSTR pszBuilder, HBUILDER *pHandle) PURE;
	STDMETHOD(GetNameFromBuilder)(HBUILDER hBld, CString& szBuilder) PURE;
	STDMETHOD(SetCallingProgramName)(HBUILDER hBld, CString &str) PURE;	// Non-standard COM.

	// Addition for Edit and Continue builds
	STDMETHOD(DoSpawn)(CStringList& lstCmd,	LPCTSTR szDirInit,BOOL fClear, BOOL fASync, DWORD *pcErr, DWORD *pcWarn) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IBuildWizard interface

#undef  INTERFACE
#define INTERFACE IBuildWizard

DECLARE_INTERFACE_(IBuildWizard, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBuildWizard methods
	STDMETHOD(SetActiveTarget)(HBLDTARGET hTarg, HBUILDER hBld) PURE;
	STDMETHOD(SetDefaultTarget)(HBLDTARGET hTarg, HBUILDER hBld) PURE;
	STDMETHOD(GetTargetTypeNames)(const CStringList **ppList, LPCSTR pchPlat, BOOL fUIDesc) PURE;	// Non-standard COM.
	STDMETHOD(AssignCustomBuildStep)(LPCSTR pchCommand, LPCSTR pchOutput, 
		LPCSTR pchDescription, HBLDTARGET hTarg, HBLDFILE hFile, HBUILDER hBld) PURE;
	STDMETHOD(UnassignCustomBuildStep)(HBLDTARGET hTarg, HBLDFILE hFile, HBUILDER hBld) PURE;
	STDMETHOD(SetToolSettings)(HBLDTARGET hTarg, LPCSTR pchSettings, LPCSTR pchTool, 
		BOOL fAdd, BOOL fClear, HBUILDER hBld) PURE;
	STDMETHOD(SetToolSettings)(HBLDTARGET hTarg, HBLDFILE hFile, LPCSTR pchSettings, 
		LPCSTR pchTool, BOOL fAdd, BOOL fClear) PURE;
	STDMETHOD(AddTarget)(HBLDTARGET *pHandle, LPCSTR pchTarg, LPCSTR pchPlat, LPCSTR pchType, 
		BOOL fUIDesc, BOOL fDebug, TrgCreateOp trgop, HBLDTARGET hOtherTarg, SettingOp setop, 
		HBLDTARGET hSettingsTarg, BOOL fQuiet, BOOL fOutDir, BOOL fUseMFC, HBUILDER hBld) PURE;
	STDMETHOD(AddDefaultTargets)(LPCSTR pchPlat, LPCSTR pchType, 
		HBLDTARGET &hDebugTarg, HBLDTARGET &hReleaseTarg, BOOL fUIDesc, BOOL fOutDir, 
		BOOL fUseMFC, HBLDTARGET hMirrorTarg, HBUILDER hBld, LPCSTR pchTarg) PURE;
	STDMETHOD(GetTargetOutDir)(HBLDTARGET hTarg, CString &strOut, HBUILDER hBld) PURE;	// Non-standard COM.
	STDMETHOD(SetTargetOutDir)(HBLDTARGET hTarg, CString &strOut, HBUILDER hBld) PURE;	// Non-standard COM.
	STDMETHOD(SetTargetIntDir)(HBLDTARGET hTarg, CString &strInt, HBUILDER hBld) PURE;	// Non-standard COM.
	STDMETHOD(SetTargetIsAppWiz)(HBLDTARGET hTarg, int iIsAppWiz, HBUILDER hBld) PURE;
	STDMETHOD(SetTargetUseOfMFC)(HBLDTARGET hTarg, int iUseOfMFC, HBUILDER hBld) PURE;
	STDMETHOD(SetTargetDefExt)(HBLDTARGET hTarg, CString &strExt, HBUILDER hBld) PURE;	// Non-standard COM.
	STDMETHOD(SetBuildExclude)(HBLDTARGET hTarg, HBLDFILE hFile, BOOL fExclude) PURE;
	STDMETHOD(AddFolder)(HFOLDER *pHandle, HFILESET hFileSet, LPCSTR pszFolder, HBUILDER hBld, HFOLDER hFolder, LPCSTR pszFilter) PURE;
	STDMETHOD(AddFile)(HBLDFILE *pHandle, HFILESET hFileSet, const CPath *ppathFile,  	// Non-standard COM.
		int fSettings, HBUILDER hBld, HFOLDER hFolder) PURE;
	STDMETHOD(AddMultiFiles)(HFILESET hFileSet, const CPtrList *plstpathFiles, 	// Non-standard COM.
		CPtrList *plstHBldFiles, int fSettings, HBUILDER hBld, HFOLDER hFolder) PURE;
	STDMETHOD(AddDependencyFile)(HFILESET hFileSet, const CPath *ppathFile, HBUILDER hBld, HFOLDER hFolder) PURE;	// Non-standard COM.
	STDMETHOD(GetFileSet)(HBUILDER hBld, HBLDTARGET hTarg, HFILESET *pHandle) PURE;
	STDMETHOD(GetAutoProject)(HBUILDER hBld, IBuildProject** ppProject) PURE;
	STDMETHOD(GetTargetUseOfMFC)(HBLDTARGET hTarg, int *iUseOfMFC, HBUILDER hBld) PURE;
	STDMETHOD(SetUserDefinedDependencies)(LPCSTR pchUserDeps, HBLDTARGET hTarg, HBLDFILE hFile, HBUILDER hBld) PURE;
	STDMETHOD(GetTargetIntDir)(HBLDTARGET hTarg, CString &strInt, HBUILDER hBld) PURE;	// Non-standard COM.
};

/////////////////////////////////////////////////////////////////////////////
//	IBuildPlatforms interface

#undef  INTERFACE
#define INTERFACE IBuildPlatforms

DECLARE_INTERFACE_(IBuildPlatforms, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBuildPlatforms methods
	STDMETHOD(GetAvailablePlatforms)(const CStringList **ppList, LPCSTR pchTarg, BOOL fUIDesc) PURE;	// Non-standard COM.
	STDMETHOD(GetSupportedPlatformCount)(int *piCount) PURE;
	STDMETHOD(GetPrimaryPlatform)(uniq_platform *pupID) PURE;
	STDMETHOD(GetCurrentPlatform)(HBUILDER hBld, uniq_platform *pupID) PURE;
	STDMETHOD(IsPlatformSupported)(uniq_platform upID) PURE;
	STDMETHOD(InitPlatformEnum)() PURE;
	STDMETHOD(NextPlatform)(uniq_platform *pupID) PURE;
	STDMETHOD(GetPlatformUIDescription)(uniq_platform upID, LPCSTR *ppszDesc) PURE;
	STDMETHOD(GetPlatformSHName)(uniq_platform upID, LPCSTR *ppszName) PURE;
	STDMETHOD(GetPlatformEMName)(uniq_platform upID, LPCSTR *ppszName) PURE;
	STDMETHOD(GetPlatformEEName)(uniq_platform upID, LPCSTR *ppszName) PURE;
	STDMETHOD(GetPlatformLocalTLIndex)(uniq_platform upID, UINT *puTLIndex) PURE;
	STDMETHOD(GetPlatformCurrentTLIndex)(uniq_platform upID, UINT *puTLIndex) PURE;
	STDMETHOD(SetPlatformCurrentTLIndex)(uniq_platform upID, UINT uTLIndex) PURE;
	STDMETHOD(GetPlatformCompRegKeyName)(uniq_platform upID, BOOL fInclAppKey, LPCSTR *ppszName) PURE;
	STDMETHOD(GetPlatformNMCount)(uniq_platform upID, int *piNMCount) PURE;
	STDMETHOD(GetPlatformNMName)(uniq_platform upID, UINT uNMIndex, LPCSTR *ppszName) PURE;
	STDMETHOD(GetPlatformTLCount)(uniq_platform upID, int *piTLCount) PURE;
	STDMETHOD(GetPlatformTLName)(uniq_platform upID, UINT uTLIndex, LPCSTR *ppszName) PURE;
	STDMETHOD(GetPlatformTLDescription)(uniq_platform upID, UINT uTLIndex, LPCSTR *ppszDesc) PURE;
	STDMETHOD(GetPlatformTLFlags)(uniq_platform upID, UINT uTLIndex, UINT *puFlags) PURE;
	STDMETHOD(GetPlatformBuildable)(uniq_platform upID, BOOL * pfBuildable) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IBuildComponents interface

#undef  INTERFACE
#define INTERFACE IBuildComponents

DECLARE_INTERFACE_(IBuildComponents, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBuildComponents methods
	STDMETHOD(GetBldSysCompName)(DWORD dwID, LPSTR*ppszName) PURE;
	STDMETHOD(GenerateBldSysCompPackageId)(LPSTR pszName, WORD *pwID) PURE;
	STDMETHOD(LookupBldSysComp)(DWORD dwID, void **ppBldSysCmp) PURE;	//REVIEW: Use handle?
	STDMETHOD(GetProjTypeFromProjItem)(void *pProjItem, int *iProjType) PURE;	//REVIEW: Use handle?
	STDMETHOD(GetProjItemAttributes)(void *pProjItem, int *iAttrs) PURE;	//REVIEW: Use handle?
};

/////////////////////////////////////////////////////////////////////////////
//	IBuildStatus interface

#undef  INTERFACE
#define INTERFACE IBuildStatus

DECLARE_INTERFACE_(IBuildStatus, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBuildStatus methods
	STDMETHOD(IsBuildPossible)() PURE;
	STDMETHOD(GetErrorCount)(LPDWORD pdwErrorCount) PURE;
	STDMETHOD(GetWarningCount)(LPDWORD pdwWarningCount) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IBuildDirManager

#define PLATFORM_CURRENT -1

enum DIRLIST_TYPE
{
	DIRLIST_PATH,
	DIRLIST_INC,
	DIRLIST_LIB,
    DIRLIST_SOURCE,
	C_DIRLIST_TYPES
};

#undef  INTERFACE
#define INTERFACE IBuildDirManager
DECLARE_INTERFACE_(IBuildDirManager, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBuildDirManager methods
	STDMETHOD(GetPlatformIndex)(LPCTSTR lpszPlatform, int* pnPlatform) PURE;
	STDMETHOD(GetDirList)(int nPlatform, DIRLIST_TYPE type,
		const CObList** ppList) PURE;	// Non-standard COM
	STDMETHOD(GetDirListString)(int nPlatform, DIRLIST_TYPE type,
		LPSTR* ppszPath) PURE;
	STDMETHOD(FindFileOnPath)(int nPlatform, DIRLIST_TYPE type, LPCSTR pszFile,
		LPSTR* ppszFullPath) PURE;
};

// *************************************************************************
// *************************************************************************
// ************************  This is the new add on model  *****************
// *************************************************************************
// *************************************************************************


/////////////////////////////////////////////////////////////////////////////
// IBSProjects

#undef  INTERFACE
#define INTERFACE IBSProject
DECLARE_INTERFACE_(IBSProject, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBSProject
	STDMETHOD(SetDirtyState)(BOOL b) PURE;
	STDMETHOD(AddDependentProject)(IBSProject *) PURE;
	STDMETHOD(RemoveDependentProject)(IBSProject *) PURE;
	STDMETHOD(EnumDependentProjects)(IEnumBSProjects **) PURE;
	STDMETHOD(SetCurrentConfiguration)(HCONFIGURATION) PURE;
	STDMETHOD(AddConfiguration)(HCONFIGURATION) PURE;
	STDMETHOD(RemoveConfiguration)(HCONFIGURATION) PURE;
	STDMETHOD(EnumConfigurations)(IEnumConfigurations **) PURE;
	STDMETHOD(GetConfigurationName)(HCONFIGURATION,LPOLESTR *) PURE;
	STDMETHOD(GetConfigurationPlatform)(HCONFIGURATION,IPlatform **) PURE;
	STDMETHOD(Build)(HCONFIGURATION,UINT type) PURE;
	STDMETHOD(AssignCustomBuildStep)(HCONFIGURATION,LPBUILDFILE,LPCOLESTR) PURE;
	STDMETHOD(RemoveCustomBuildStep)(HCONFIGURATION,LPBUILDFILE) PURE;
	STDMETHOD(SetToolSettings)(HCONFIGURATION,LPBUILDFILE) PURE;
	STDMETHOD(GetCallingProgramName)(HCONFIGURATION,LPOLESTR *) PURE;
	STDMETHOD(GetRunArguments)(HCONFIGURATION,LPOLESTR *) PURE;
	STDMETHOD(SetRunArguments)(HCONFIGURATION,LPOLESTR) PURE;
	STDMETHOD(GetCProject)(void **) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IBSProjectFileManager

#undef  INTERFACE
#define INTERFACE IBSProjectFileManager
DECLARE_INTERFACE_(IBSProjectFileManager, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBSProjectFileManager
	STDMETHOD(AddFile)(LPCOLESTR,LPBUILDFILE *) PURE;
	STDMETHOD(GetFileSet)(LPBUILDFILESET *) PURE;
	STDMETHOD(IsInProject)(LPCOLESTR,LPBUILDFILE *) PURE;
	STDMETHOD(IsBuildableFile)(LPBUILDFILE) PURE;
	STDMETHOD(IsScanableFile)(LPBUILDFILE) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IBSRemoteProject

#undef  INTERFACE
#define INTERFACE IBSRemoteProject
DECLARE_INTERFACE_(IBSRemoteProject, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBSRemoteProject
	STDMETHOD(GetRemoteTargetName)(HCONFIGURATION, LPCOLESTR*) PURE;
	STDMETHOD(SetRemoteTargetName)(HCONFIGURATION, LPCOLESTR) PURE;
	STDMETHOD(GetInitialRemoteTarget)(HCONFIGURATION, LPCOLESTR*) PURE;
	STDMETHOD(UpdateRemoteTarget)(HCONFIGURATION) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IBSOptionManager

#undef  INTERFACE
#define INTERFACE IBSOptionManager
DECLARE_INTERFACE_(IBSOptionManager, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBSOptionManager
	STDMETHOD(GetBoolProp)(UINT id,IBuildTool*,LPBUILDFILE,HCONFIGURATION, BOOL *rval) PURE;
	STDMETHOD(GetIntProp)(UINT id, IBuildTool*,LPBUILDFILE,HCONFIGURATION, int  *rval) PURE;
	STDMETHOD(GetStrProp)(UINT id, IBuildTool*,LPBUILDFILE,HCONFIGURATION, LPOLESTR *rval) PURE;
	STDMETHOD(SetBoolProp)(UINT id,IBuildTool*,LPBUILDFILE,HCONFIGURATION, BOOL val) PURE;
	STDMETHOD(SetIntProp)(UINT id, IBuildTool*,LPBUILDFILE,HCONFIGURATION, int  val) PURE;
	STDMETHOD(SetStrProp)(UINT id, IBuildTool*,LPBUILDFILE,HCONFIGURATION, LPOLESTR val) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IEnumConfigurations

#undef  INTERFACE
#define INTERFACE IEnumConfigurations
DECLARE_INTERFACE_(IEnumConfigurations, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEnum methods
	STDMETHOD(Next)(THIS_ ULONG celt, HCONFIGURATION *rgelt, ULONG *pceltFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ IEnumConfigurations **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IEnumBSProjects

#undef  INTERFACE
#define INTERFACE IEnumBSProjects
DECLARE_INTERFACE_(IEnumBSProjects, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEnum methods
	STDMETHOD(Next)(THIS_ ULONG celt, IBSProject **rgelt, ULONG *pceltFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ IEnumBSProjects **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IEnumBSActions

#undef  INTERFACE
#define INTERFACE IEnumBSActions
DECLARE_INTERFACE_(IEnumBSActions, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEnumBSActions
	STDMETHOD(Next)(THIS_ ULONG celt, IBSAction **rgelt, ULONG *pceltFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ IEnumBSActions **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IBSAction

#undef  INTERFACE
#define INTERFACE IBSAction
DECLARE_INTERFACE_(IBSAction, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBSActions
	STDMETHOD(GetInputs)(LPBUILDFILESET *) PURE;
	STDMETHOD(GetOutputs)(LPBUILDFILESET *) PURE;
	STDMETHOD(GetTool)(LPBUILDTOOL *) PURE;
	STDMETHOD(GetFile)(LPBUILDFILE *) PURE;
	STDMETHOD(GetConfiguration)(HCONFIGURATION *) PURE;
	STDMETHOD(AddOutput)(const CPath *) PURE;
	STDMETHOD(GetCommandOptions)(CString *) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IBuildFile

#undef  INTERFACE
#define INTERFACE IBuildFile
DECLARE_INTERFACE_(IBuildFile, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBuildFile
};

/////////////////////////////////////////////////////////////////////////////
// IBSFileSet

#undef  INTERFACE
#define INTERFACE IBSFileSet
DECLARE_INTERFACE_(IBSFileSet, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBSFileSet
};

/////////////////////////////////////////////////////////////////////////////
// IBuildFileEnum

#undef  INTERFACE
#define INTERFACE IBuildFileEnum
DECLARE_INTERFACE_(IBuildFileEnum, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBuildFileEnum
	STDMETHOD(Next)(THIS_ ULONG celt, IBuildFile **rgelt, ULONG *pceltFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ IBuildFileEnum **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IBuildSystemAddOn

#undef  INTERFACE
#define INTERFACE IBuildSystemAddOn
DECLARE_INTERFACE_(IBuildSystemAddOn, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBuildSystemAddOn
	STDMETHOD(GetName)( char ** ) PURE;
	STDMETHOD(SetClient)( LPBUILDSYSTEM ) PURE;
	STDMETHOD(EnumBuildTools)( LPENUMBUILDTOOLS *, LPBUILDTOOL * ) PURE;
	STDMETHOD(EnumProjectTypes)( LPENUMPROJECTTYPES *, LPPROJECTTYPE * ) PURE;
	STDMETHOD(EnumPlatforms)( LPENUMPLATFORMS *, LPPLATFORM * ) PURE;
	STDMETHOD(ModifyProjectTypes)( LPENUMPROJECTTYPES ) PURE;
	STDMETHOD(GetID)( UINT * ) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IEnumBuildTools

#undef  INTERFACE
#define INTERFACE IEnumBuildTools
DECLARE_INTERFACE_(IEnumBuildTools, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEnumBuildTools
	STDMETHOD(Next)(THIS_ ULONG celt, IBuildTool **rgelt, ULONG *pceltFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ IEnumBuildTools **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IBuildTool

#undef  INTERFACE
#define INTERFACE IBuildTool
DECLARE_INTERFACE_(IBuildTool, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IBuildTool
	STDMETHOD(IsCollectorTool)(void) PURE;
	STDMETHOD(Filter)(LPBUILDFILE) PURE;
	STDMETHOD(PerformBuild)( int type, int stage,LPENUMBSACTIONS, int *result ) PURE;
	STDMETHOD(GenerateCommandLines)( LPENUMBSACTIONS, CStringList &, CStringList & ) PURE;
	STDMETHOD(GenerateOutputs)( LPENUMBSACTIONS ) PURE;
	STDMETHOD(GetDependencies)( LPENUMBSACTIONS ) PURE;
	STDMETHOD(EnumOptionStrings)( LPENUMOPTIONSTRINGS * ) PURE;
	STDMETHOD(EnumOptionTypes)( LPENUMOPTIONTYPES * ) PURE;
	STDMETHOD(GetID)( UINT *pId ) PURE;
	STDMETHOD(GetPkgID)(UINT *id) PURE;
	STDMETHOD(GetName)(CString *str) PURE;
	STDMETHOD(GetUIName)(CString *str) PURE;
	STDMETHOD(GetPrefix)(CString *str) PURE;

};

/////////////////////////////////////////////////////////////////////////////
// IEnumProjectTypes

#undef  INTERFACE
#define INTERFACE IEnumProjectTypes
DECLARE_INTERFACE_(IEnumProjectTypes, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEnumProjectTypes
	STDMETHOD(Next)(THIS_ ULONG celt, IProjectType **rgelt, ULONG *pceltFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ IEnumProjectTypes **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IProjectType

#undef  INTERFACE
#define INTERFACE IProjectType
DECLARE_INTERFACE_(IProjectType, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IProjectType
	STDMETHOD(GetName)( CString *str ) PURE;
	STDMETHOD(GetType)( UINT *id ) PURE;
	STDMETHOD(GetPlatform)( LPPLATFORM *) PURE;
	STDMETHOD(IsSupported)(void) PURE;

	STDMETHOD(PickTool)( LPBUILDFILE pFile, LPBUILDTOOL *pBuildTool) PURE;
	STDMETHOD(AddTool)( LPBUILDTOOL pBuildTool) PURE;
	STDMETHOD(EnumBuildTools)( LPENUMBUILDTOOLS *, LPBUILDTOOL * ) PURE;

//	STDMETHOD(EnumDefaultConfigs)( LPENUMDEFAULTCONFIGS *, LPDEFAULTCONFIG * ) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IPlatform

#undef  INTERFACE
#define INTERFACE IPlatform
DECLARE_INTERFACE_(IPlatform, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IPlatform
};

/////////////////////////////////////////////////////////////////////////////
// IEnumPlatforms

#undef  INTERFACE
#define INTERFACE IEnumPlatforms
DECLARE_INTERFACE_(IEnumPlatforms, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEnumPlatforms
	STDMETHOD(Next)(THIS_ ULONG celt, LPPLATFORM *rgelt, ULONG *pceltFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ IEnumPlatforms **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ISetJavaProjectInfo

#undef  INTERFACE
#define INTERFACE ISetJavaProjectInfo
DECLARE_INTERFACE_(ISetJavaProjectInfo, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// ISetJavaProjectInfo methods
	STDMETHOD_(BOOL, SetPrimaryClassName)(CString& strProjName, CString& strPrimaryClassName) PURE;
};

#endif	// __BLDAPI_H__
