///////////////////////////////////////////////////////////////////////////////
//
// PROJECT.H
//                                                                             
///////////////////////////////////////////////////////////////////////////////
#ifndef __PROJECT_H__
#define __PROJECT_H__


// make sure we define this before the CProject et al
#include "bldiface.h"
#include <afxmt.h>		// for CCriticalSection

#include <prjapi.h>
#include <bldguid.h>

#define IDD_PROPSTEST                   113

#ifndef __SHLDOCS_H__
#include "shldocs.h"
#endif

#ifndef __PROPPAGE_H___
#include "proppage.h"
#endif
 
#ifndef __PROJITEM__
#include "projitem.h"
#endif

#ifdef _INSTRAPI
#ifndef __INSTRAPI__H
#include "instrapi.h"
#endif
#endif

#ifndef _TARGETITEM_H
#include "targitem.h"
#endif

// Switch to turn on the External makefile files list:
#define EXT_FILES

// Debug-only flag to turn on "project info" prop pages:
#ifdef _DEBUG
extern	BOOL	g_bProjDebugInfo;
#endif

// Debug-only flag to turn on a debug "project view"
#ifdef _DEBUG
extern	BOOL	g_bProjDebugView;
#endif

// When build logging is enabled in the build it can be
// temporarily disabled by throwing /Y2 on the command line
#ifdef _BTLOG
extern	BOOL	g_bBuildLogging;
#endif

// Makefile sharability levels
#define SHARE_LOW 0
#define SHARE_NORM 1
#define SHARE_HIGH 2

// Make available in shipping builds also 
extern  BOOL	g_bSpawnTime;

extern  BOOL	g_bInProjClose;

extern BOOL g_bBadConversion;
extern BOOL g_bAlwaysExportMakefile;
extern BOOL g_bAlwaysExportDeps;
extern BOOL g_bExportMakefile;
extern BOOL g_bExportDeps;
extern BOOL g_bWriteProject;
extern BOOL g_bWriteBuildLog;

extern BOOL g_nSharability;
extern BOOL g_bMultiBuild;
extern BOOL g_bStopBuild;

#ifdef _INSTRAPI
extern	LOG	g_ProjectPerfLog;
#endif

HINSTANCE GetResourceHandle();

// All CProject 'exported' functions to other packages should be
// virtual so that cross-DLL function calling indirects through
// the object VTABLE ptr
#define EXPORTED virtual

class CBldPackage;
class CProjectDoc;
class CStateSaver;
class CTempMaker;
class CPlatform;

//  Helper class:
// a list of target includes for a project
class CTargIncl
{
protected:
	struct CItem
	{
		HBLDTARGET m_hTarget;
		CString m_strIncls;
		CItem * m_pNext;
	};
	CItem * m_pHead;
	CCriticalSection m_cs;
	HBUILDER m_hBld;
public:
	CTargIncl();
	~CTargIncl();
	BOOL Lookup (HBLDTARGET hTarget, CString & strIncls);
	void RemoveAll();
	BOOL Remove (HBLDTARGET hTarget);
	void SetAt (HBLDTARGET hTarget, CString & strIncls);
	void Init(HBUILDER hBld);
	void AddTarget (HBLDTARGET hTarget);
};

typedef struct tagPROJDEPREFINFO {
    CProjItem * pProjDep;
    ConfigurationRecord * pcr;
} PROJDEPREFINFO;

typedef PROJDEPREFINFO ConfigurationHandle;

//	Helper functions:

//	Converts the a project file name (.mak etc.) to the corresponding
//	workspace file name (.MDP ):
void ProjNameToWksName(CPath &rPath);

//	Converts the a project file name (.mak etc.) to the corresponding
//	pirvate binary file name (.OPT ):
inline void ProjNameToOptName (CPath &rPath)
{
	ProjNameToWksName(rPath);
	rPath.ChangeExtension(".opt");
}

inline void ProjNameToBinName (CPath &rPath)
{
	// old version for 4.x compatibility only
	rPath.ChangeExtension(WORKSPACE_EXT);
}

inline void BinNameToProjName (CPath &rPath)
{
	rPath.ChangeExtension (BUILDER_EXT);
}

class CProjectInterfaces : public CCmdTarget {
protected:
	BEGIN_INTERFACE_PART(Project, IBSProject)
		INIT_INTERFACE_PART(CProjectInterfaces, Project)
		STDMETHOD(SetDirtyState)(BOOL b);
		STDMETHOD(AddDependentProject)(IBSProject *);
		STDMETHOD(RemoveDependentProject)(IBSProject *);
		STDMETHOD(EnumDependentProjects)(IEnumBSProjects **);
		STDMETHOD(SetCurrentConfiguration)(HCONFIGURATION);
		STDMETHOD(AddConfiguration)(HCONFIGURATION);
		STDMETHOD(RemoveConfiguration)(HCONFIGURATION);
		STDMETHOD(EnumConfigurations)(IEnumConfigurations **);
		STDMETHOD(GetConfigurationName)(HCONFIGURATION,LPOLESTR *);
		STDMETHOD(GetConfigurationPlatform)(HCONFIGURATION,IPlatform **);
		STDMETHOD(Build)(HCONFIGURATION,UINT type);
		STDMETHOD(AssignCustomBuildStep)(HCONFIGURATION,LPBUILDFILE,LPCOLESTR);
		STDMETHOD(RemoveCustomBuildStep)(HCONFIGURATION,LPBUILDFILE);
		STDMETHOD(SetToolSettings)(HCONFIGURATION,LPBUILDFILE);
		STDMETHOD(GetCallingProgramName)(HCONFIGURATION,LPOLESTR *);
		STDMETHOD(GetRunArguments)(HCONFIGURATION,LPOLESTR *);
		STDMETHOD(SetRunArguments)(HCONFIGURATION,LPOLESTR);
		STDMETHOD(GetCProject)(void **pProj);
	END_INTERFACE_PART(Project)

	BEGIN_INTERFACE_PART(ProjectFileManager, IBSProjectFileManager)
		INIT_INTERFACE_PART(CProjectInterfaces, ProjectFileManager)
		STDMETHOD(AddFile)(LPCOLESTR,LPBUILDFILE*);
		STDMETHOD(GetFileSet)(LPBUILDFILESET *);
		STDMETHOD(IsInProject)(LPCOLESTR,LPBUILDFILE*);
		STDMETHOD(IsBuildableFile)(LPBUILDFILE);
		STDMETHOD(IsScanableFile)(LPBUILDFILE);
	END_INTERFACE_PART(ProjectFileManager)

	BEGIN_INTERFACE_PART(RemoteProject, IBSRemoteProject)
		INIT_INTERFACE_PART(CProjectInterfaces, RemoteProject)
		STDMETHOD(GetRemoteTargetName)(HCONFIGURATION, LPCOLESTR*);
		STDMETHOD(SetRemoteTargetName)(HCONFIGURATION, LPCOLESTR);
		STDMETHOD(GetInitialRemoteTarget)(HCONFIGURATION, LPCOLESTR*);
		STDMETHOD(UpdateRemoteTarget)(HCONFIGURATION);
	END_INTERFACE_PART(RemoteProject)
	
	BEGIN_INTERFACE_PART(OptionManager,IBSOptionManager)
		INIT_INTERFACE_PART(CProjectInterfaces, OptionManager)
		STDMETHOD(GetBoolProp)(UINT id,IBuildTool*,LPBUILDFILE,HCONFIGURATION, BOOL *rval);
		STDMETHOD(GetIntProp)(UINT id, IBuildTool*,LPBUILDFILE,HCONFIGURATION, int  *rval);
		STDMETHOD(GetStrProp)(UINT id, IBuildTool*,LPBUILDFILE,HCONFIGURATION, LPOLESTR *rval);
		STDMETHOD(SetBoolProp)(UINT id,IBuildTool*,LPBUILDFILE,HCONFIGURATION, BOOL val);
		STDMETHOD(SetIntProp)(UINT id, IBuildTool*,LPBUILDFILE,HCONFIGURATION, int  val);
		STDMETHOD(SetStrProp)(UINT id, IBuildTool*,LPBUILDFILE,HCONFIGURATION, LPOLESTR val);
	END_INTERFACE_PART(OptionManager)

	BEGIN_INTERFACE_PART(PkgProject,IPkgProject)
		INIT_INTERFACE_PART(CProjectInterfaces, PkgProject)
		STDMETHOD(AddFiles)(LPCOLESTR * pszFileNames, ULONG count, 
					LPCOLESTR szDestinationFolder);
		STDMETHOD(CanAddFile)(LPCOLESTR szFileName, LPCOLESTR szDestinationFolder, BOOL bShowUI);
		STDMETHOD(DeleteFile)(LPCOLESTR szFileName, LPCOLESTR szDestinationFolder);
		STDMETHOD(RenameFile)(LPCOLESTR szOldFileName, LPCOLESTR szSourceFolder,
					LPCOLESTR szNewFileName, LPCOLESTR szDestinationFolder);
		STDMETHOD(CreateFolder)(LPCOLESTR szFolder);
		STDMETHOD(OnActivate)(BOOL bActivate);
		STDMETHOD(GetAutomationObject)(LPDISPATCH *ppAutomationProject);
		STDMETHOD(Delete)(DeletionCause dcCause, BOOL * pbSuccess);
		STDMETHOD(SetDependentProject)(IPkgProject *pDependentProject);
		STDMETHOD(GetClassID)(CLSID * pClsID);
		STDMETHOD(GetProjectInfo)(LPCOLESTR *pszProjName, LPCOLESTR *pszProjFullPath,
					DWORD *pdwPackageID, DWORD *pdwFlags);
                STDMETHOD(GetProjectTypeName)(CString **ppszType);
		STDMETHOD(DrawProjectGlyph)(HDC hDC, LPCRECT pRect) ;
		STDMETHOD(Save)() ;
		STDMETHOD(IsDirty)() ;
		STDMETHOD(GetDefaultFolder)(LPCOLESTR *pszDefaultFolder);
		STDMETHOD(CanBrowseFolders)(BOOL *pbCanBrowse);
		STDMETHOD(BrowseFolders)(LPCOLESTR szStartingFolder,
							LPCOLESTR *pszFolderChosen);
		STDMETHOD(GetAllFolders)(CALPOLESTR *pAllFolders);
		STDMETHOD(GetAssociatedFiles)(CStringArray& outFiles, BOOL selected);
		STDMETHOD(SetDirty)(BOOL bDirty);
		STDMETHOD(GetProjectData)(DWORD dwProp, CString & strVal);
		STDMETHOD(SetProjectData)(DWORD dwProp, LPCSTR szVal);
	END_INTERFACE_PART(PkgProject)

	DECLARE_INTERFACE_MAP()

public:
	CProjectInterfaces();
	~CProjectInterfaces();

	void SetThis( CProject *pProj ){ m_pProject = pProj; };
	CProject *GetProject(){ return m_pProject; };

	LPBSPROJECT GetInterface(void){
		LPBSPROJECT pIProject;
		m_xProject.QueryInterface( IID_IBSProject, (void **)&pIProject );
		return pIProject;
 	};

private:
	CProject *m_pProject;
};


#ifdef VB_MAKEFILES
class VBInfo {
public:
	CString m_IconForm;
	CString m_Startup;
	CString m_HelpFile;
	CString m_Title;
	CString m_Command32;
	CString m_ExeName32;
	CString m_Path32;
	CString m_Name;
	CString m_Type;
	CString m_HelpContextID;
	CString m_CompatibleMode;
	BOOL m_MajorVer;
	BOOL m_MinorVer;
	BOOL m_RevisionVer;
	BOOL m_AutoIncrementVer;
	BOOL m_ServerSupportFiles;
	CString m_VersionCompanyName;
	BOOL m_CompilationType;
	BOOL m_OptimizationType;
	BOOL m_FavorPentiumPro;
	BOOL m_CodeViewDebugInfo;
	BOOL m_NoAliasing;
	BOOL m_BoundsCheck;
	BOOL m_OverflowCheck;
	BOOL m_FlPointCheck;
	BOOL m_FDIVCheck;
	BOOL m_UnroundedFP;
	BOOL m_StartMode;
	BOOL m_Unattended;
	BOOL m_ThreadPerObject;
	int m_MaxNumberOfThreads;
};
#endif

class CEnumConfigurations : public CCmdTarget {
public:
	CEnumConfigurations( CProjItem *pProjItem ){
		m_pos = NULL;
		const CPtrArray * pCfgArray = pProjItem->GetConfigArray();
		m_len = pCfgArray->GetSize();
		m_aConfigs = new ConfigurationHandle[];
		for( int i=0; i<m_len; i++ ) {
			m_aConfigs[i].pProjDep = pProjItem;
			m_aConfigs[i].pcr = (ConfigurationRecord *)(*pCfgArray)[i];
		}
	};

	LPENUMCONFIGURATIONS GetInterface(void){
		LPENUMCONFIGURATIONS pIEnum;
		m_xEnumConfigurations.QueryInterface(IID_IEnumConfigurations, (void **)&pIEnum);
		return pIEnum;
	}

protected:
	BEGIN_INTERFACE_PART(EnumConfigurations, IEnumConfigurations)
		INIT_INTERFACE_PART(CEnumConfigurations, EnumConfigurations)
		STDMETHOD(Next)(THIS_ ULONG celt, HCONFIGURATION *rgelt, ULONG *pceltFetched);
		STDMETHOD(Skip)(THIS_ ULONG celt);
		STDMETHOD(Reset)(THIS);
		STDMETHOD(Clone)(THIS_ IEnumConfigurations **ppenum);
	END_INTERFACE_PART(EnumConfigurations)

	DECLARE_INTERFACE_MAP()

public:
	ConfigurationHandle *m_aConfigs;
	int m_pos;
	int m_len;
};
	


class CEnumDependentProjects : public CCmdTarget {
public:
	CEnumDependentProjects(){
		m_pos = NULL;
		m_pList = NULL;
	};
	LPENUMBSPROJECTS GetInterface(void){
		LPENUMBSPROJECTS pIEnum;
		m_xEnumProjects.QueryInterface(IID_IEnumBSProjects, (void **)&pIEnum);
		return pIEnum;
	}

protected:
	BEGIN_INTERFACE_PART(EnumProjects, IEnumBSProjects)
		INIT_INTERFACE_PART(CEnumDependentProjects, EnumProjects)
		STDMETHOD(Next)(THIS_ ULONG celt, LPBSPROJECT *rgelt, ULONG *pceltFetched);
		STDMETHOD(Skip)(THIS_ ULONG celt);
		STDMETHOD(Reset)(THIS);
		STDMETHOD(Clone)(THIS_ IEnumBSProjects **ppenum);
	END_INTERFACE_PART(EnumProjects)

	DECLARE_INTERFACE_MAP()

private:
	const CPtrList *m_pList;
	POSITION m_pos;
};
	

///////////////////////////////////////////////////////////////////////////////
// What project property page do we want to show
enum ProjSettingsPage
{
	Project_General,				// config. mode etc. (always the first) 
	Project_DebugInfo_Settings		// brings up debug options
};									

///////////////////////////////////////////////////////////////////////////////
class CProjTreeCtl;
class CProjectInterfaces;
class BLD_IFACE CProject : public CProjContainer
{
	friend class CBldPackage;
	friend class CProjectView;
	friend class CBldSysIFace;
	friend class CProjTempConfigChange;
	friend class CProjDeferDirty;
	friend class CProjectInterfaces;
	friend class CProjOptionsDlg;
	friend class CProjItem;

//
//	Top level project class.  May be more than one if there are subprojects.
//
	DECLARE_SERIAL (CProject)

	FileRegHandle   m_hndFileReg;		// File registry for .mak file.
	BOOL m_bPrivateDataInitialized;
	BOOL m_bProjectComplete;
	BOOL m_bNotifyOnChangeConfig;
	BOOL m_bClassWizAddedFiles;			// set by ClassWizard VCPP32 callback
	BOOL m_bPostMakeFileRead;			// TRUE once we have read most of the
										// makefile in, i.e doing post makefile
										// reading stuff

protected:
	// Project commands
	// Shared with OnSettings() and OnPopupSettings()
	void OnSettingsCommon(CSlob * pSelection);

	//{{AFX_MSG(CProject)

	// custom tools top-level menuitem
	afx_msg void OnTools();
	afx_msg void OnUpdateTools(CCmdUI *);
	afx_msg void OnSettings();
	afx_msg void OnConfigurations();
	afx_msg void OnSubprojects();
	afx_msg void OnBuildGraph();
	afx_msg void OnNewGroup();
	afx_msg void OnPopupFilesIntoProject();
	afx_msg void OnCompile();
	afx_msg void OnBuild();
	afx_msg void OnClean();
	afx_msg void OnRebuildAll();
	afx_msg void OnBatchBuild();
	afx_msg void OnStopBuild();
	afx_msg void OnToggleBuild();
	afx_msg void OnScan();
	afx_msg void OnScanAll();
	afx_msg void OnSelectTarget();
	afx_msg void OnSelectConfig();
	afx_msg void OnToolMecr();
	afx_msg void OnCompilePostMsg();
	afx_msg void OnCleanPostMsg();
	afx_msg void OnBuildPostMsg();
	afx_msg void OnRebuildAllPostMsg();
	afx_msg void OnBatchBuildPostMsg();
	afx_msg void OnExport();
	afx_msg void OnProjectWrite();
	afx_msg void OnDumpHeap();
	afx_msg void OnProfile();
	afx_msg void OnUpdateNewGroup(CCmdUI *);
	afx_msg void OnUpdateSettings(CCmdUI *);
	afx_msg void OnUpdateConfigurations(CCmdUI *);
	afx_msg void OnUpdateSubprojects(CCmdUI *);
	afx_msg void OnUpdateSettingsPop(CCmdUI *);
	afx_msg void OnUpdateFilesIntoProject(CCmdUI *);
	afx_msg void OnUpdateBuildGraph(CCmdUI *);
	afx_msg void OnUpdateCompile(CCmdUI *);
	afx_msg void OnUpdateBuild(CCmdUI *);
	afx_msg void OnUpdateClean(CCmdUI *);
	afx_msg void OnUpdateRebuildAll(CCmdUI *);
	afx_msg void OnUpdateBatchBuild(CCmdUI *);
	afx_msg void OnUpdateStopBuild(CCmdUI *);
	afx_msg void OnUpdateToggleBuild(CCmdUI *);
	afx_msg void OnUpdateScan(CCmdUI *);
	afx_msg void OnUpdateScanAll(CCmdUI *);
	afx_msg void OnUpdateToolMecr(CCmdUI *);
	afx_msg void OnUpdateTargetCombo(CCmdUI *);
	afx_msg void OnUpdateConfigCombo(CCmdUI *);
	afx_msg void OnUpdateExport(CCmdUI *);
	afx_msg void OnUpdateProjectWrite(CCmdUI *);
	afx_msg void OnUpdateProfile(CCmdUI *);
	afx_msg void OnUpdateDumpHeap(CCmdUI *);
	afx_msg void OnUpdateUseWebBrowser(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUseTestContainer(CCmdUI *pCmdUI);
	//}}AFX_MSG(CProject)

	virtual BOOL OnCmdMsg(UINT, int, void *, AFX_CMDHANDLERINFO *);

public:
	// constructor
	CProject ();

	// destroy contents
	void Destroy();

	// destructor
	virtual ~CProject();

	// we want to update the content of the 'Project' menu
	// with our deferred commands. these will always exist
	// if a tool has a deferred command, but it may be
	// grayed if that deferred command is not applicable
	// in the current project state
	// by setting fNuke == TRUE, this function will
	// Nuke them all
	void UpdateDeferredCmds(BOOL fNuke);

	virtual BOOL UsesSchmoozeTool () { return TRUE; }

// CSlob methods:
	// The CanAdd method tells us what makes sense for the user to
	// be able to add to this slob container
	// In this case we can add CProjGroup's but nothing else.
	virtual BOOL CanAdd ( CSlob * );

	// The CanAct method tells us what it makes sense for
	// a user to be able to perform a certain action.
	virtual BOOL CanAct(ACTION_TYPE action);

	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

	// Special CloneConfigs method.  As a subproject, the project tries
	// to switch to a compatible configuartion:
	virtual void CloneConfigs ( const CProjItem *pItem );
	void MatchConfig (const ConfigurationRecord *pcr );

	// Hook GetIntProp & GetStrProp to provide some defaults.
	virtual GPT GetIntProp(UINT idProp, int& val);
	virtual GPT GetStrProp(UINT idProp, CString& val);

	// We want to hook into the SetStrProp so we can catch configuration changes.
	virtual BOOL SetStrProp(UINT idProp, const CString& val);
	virtual BOOL SetIntProp(UINT idProp, int val);

	// Ad/remove ourselves as a dependent of the project view.
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	virtual BOOL PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);

	// This is our internal CanAdd method, and tells us what we can
	// add to this slob container
	// In this case we can add CProjGroup's but nothing else.
	virtual BOOL CanContain(CSlob* pSlob);	

	// Drop and Drag support
	virtual CSlobDragger * CreateDragger(CSlob *pDragObject, CPoint screenPoint);
	virtual void DoDrop(CSlob *pSlob, CSlobDragger *pDragger);
	virtual void DoDrag(CSlob *pSlob, CSlobDragger *pDragger, CPoint screenPoint);

	// File I/O and creation:
	// Fills out a new project:
	BOOL InitNew (const CPtrList * pProjTypeList); // Uses a temporary file.
	BOOL InitFromFile (const char *pFileName, BOOL bLoadOPTFile = TRUE);

	void InitPrivateData();	// not needed for new projects
	void InitPrivateDataForExe ();

	BOOL DoSave (BOOL bSavePrivate = FALSE);
	BOOL DoSaveAs (BOOL bSavePrivate = FALSE);

	virtual BOOL SetFile (const CPath *pPath, BOOL bCaseApproved = FALSE);

	virtual BOOL DoTopLevelScan(BOOL bWriteOutput = TRUE);

	// Describes how we flatten a project node
	virtual void FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem);
private:	
	// Targets remaining to build when doing a batch build
	CStringList	m_BuildConfigs;
	CDir		m_ProjDir;

public:
	CDir & GetWorkspaceDir() { return m_ProjDir; }
	CDir & GetProjDir(ConfigurationRecord * pcr = NULL);

	virtual FileRegHandle GetFileRegHandle () const { return m_hndFileReg; };

// Other API:
	// Return the project's resources for the specified configuration,
	// (or the current active if pcr = NULL)
	EXPORTED BOOL GetResourceFileList(CObList & filelist, BOOL fOnlyBuildable = FALSE, const ConfigurationRecord *pcr = NULL);

	// Return the project's resource /I (includes) + Include Path for the specified
	// resource file. Returns TRUE if can do, or FALSE if resource file not valid
	EXPORTED BOOL GetResourceIncludePath(CFileItem * pResFile, CString & strIncludes); 

	// Get version info for App studio:
	EXPORTED void GetVersionInfo(PROJECT_VERSION_INFO* ppvi);

	// Show a particular project settings page
	EXPORTED BOOL ShowProjSettingsPage(ProjSettingsPage prjpage);
		
	// Setup the project's envirnoment variables:
	EXPORTED void GetEnvironmentVariables (CEnvironmentVariableList *pEnvList);
	// Get the project's toolset
	EXPORTED INT GetProjectToolset();

	//	Build the project, starting at the specified item (which
	//	may be the project itself):

	enum BuildResults
	{
		BuildComplete,
		BuildCanceled,
		BuildError,
		NoBuild,
	};

	enum BuildType
	{
		Compile,
		NormalBuild,
		BatchBuild,
	};

	virtual BuildResults DoBuild
	(
		CProjItem * pItemStart,
		UINT buildType,
		BOOL bContinueAfterErrors,
		FlagsChangedAction & fca,
		CTempMaker & rTM,
		CErrorContext& EC = g_DummyEC,
		BuildType bBuildType = NormalBuild,
		BOOL bVerbose = TRUE,
		BOOL bRecurse = TRUE
	);

	BuildStates GetBuildStateForTool
	(
		CSchmoozeTool * pSTool,
		CFileRegSet & regsetConsumables,
		CErrorContext & EC = g_DummyEC
	);

	virtual BOOL TopLevelRunDeferedBuildTool(CSchmoozeTool *pTool, BOOL fAlways = FALSE);

	// Special methods for the IDE to access project info.  Note that
	// these all need to be EXPORTED (ie. virtual) since the IDE doesn't link with 
	// us:

	// Check to see if the specified file is in the project.  If so,
	// return a pointer to the CProjItem.  If not, the behavior
	// depends on the value of bAddIfNecessary.  If bAddIfNecessary
	// is TRUE, the user is prompted to add the file.  If the user
	// answers Yes, the file is added and a pointer to its resulting
	// CProjItem is returned.  If the user answers No, this method
	// returns NULL.  If bAddIfNecessary is FALSE, this method will
	// immediately return NULL if the file isn't in the project.
	//
	// If the same file is present multiple times in the project
	// (e.g., in different groups -- we don't permit it to be present
	// multiple times in a single group), the returned CProjItem will
	// simply be the first one found.
	EXPORTED CProjItem * EnsureFileInProject(CPath * pPath, BOOL bAddIfNecessary);
	// Refigure project state before a debug step or whatever:
	EXPORTED void SetProjectState ();
	// Scan all deps:
	EXPORTED void ScanAllDependencies();
	// Scan deps for a particular item (must be a CFileItem).  Returns
	// FALSE if the scan failed for any reason (like the file isn't of
	// scannable type).  You can get a pointer to the CFileItem by using
	// EnsureFileInProject(pPathFileName, FALSE).
	EXPORTED BOOL ScanItemDependencies(CFileItem *);
	// Returns or'ed together ProjAttribtues (get this from the 
	// CProjType)
	EXPORTED int GetTargetAttributes ();
	// Returns exe to debug (if there is one):
	EXPORTED CPath *GetTargetFileName ();
	// Are we trying to build the calling program?:
	EXPORTED BOOL TargetIsCaller ();
	// Returns .BSC file (if there is one):
	EXPORTED const CPath *GetBrowserDatabaseName ();
	// Returns name of calling program (DLL project types only):
	EXPORTED void GetCallingProgramName (CString &str);
	// Returns the working directory for the calling program:
	EXPORTED void GetWorkingDirectory (CString &str);
	// Returns argument string for debuggee (EXE project types only):
	EXPORTED void GetProgramRunArguments (CString &str);
	EXPORTED void SetProgramRunArguments (CString &str);
	// Should we prompt user for local name of future remote DLL's that are loaded?
	EXPORTED BOOL FPromptForDlls (void);
	EXPORTED VOID SetPromptForDlls (BOOL);

	// Name of the class wizard .clw file
	EXPORTED BOOL ClassWizardName (CPath &path);

	// Attempts to update our remote target
	EXPORTED BOOL UpdateRemoteTarget();

	// Display a dialog to obtain the initial remote target
	// file name. This is used when the remote target file name is
	// blank and we really, really need it. E.G. when attempting
	// to debug, or run a remote copy utility (mfile)
	EXPORTED BOOL GetInitialRemoteTarget(BOOL bBuild = FALSE, BOOL fAlways = FALSE);

	// Display a dialog to obtain the executable file
	// name for debugging/executing.
	EXPORTED BOOL GetInitialExeForDebug(BOOL bExecute = FALSE);

	// Returns the name of the remote target
	EXPORTED void GetRemoteTargetFileName (CString &str);
	EXPORTED void SetRemoteTargetFileName (CString &str);

	// Add this file to the first group in the project (adding a
	// group if nessesary), but don't scan:
	EXPORTED void AddFile (const CPath *pPath );
	// Is this file used in this project:
	EXPORTED BOOL IsProjectFile (const CPath *pPath );
	// Is this an external project:
	EXPORTED BOOL IsExeProject () const;

	// Is the active target an external target
	EXPORTED BOOL IsExternalTarget();

	// exported version of OnNewGroup (accessed from CBuildSlob)
	EXPORTED void CreateNewGroup() { OnNewGroup(); }
	// exported version of OnPopupFilesIntoProject (accessed from CBuildSlob)
	EXPORTED void ActPopupFilesIntoProject() { OnPopupFilesIntoProject(); }
	// exported version of OnUpdateFilesIntoProject (accessed from CBuildSlob)
	EXPORTED void UpdPopupFilesIntoProject(CCmdUI *pCmdUI) { OnUpdateFilesIntoProject(pCmdUI); }

	BOOL AreFilesUnique(const CSlob * pSlob, BOOL bTopLevel = TRUE);
	
	// check if P_Profile is set or not
	// this method is created so that vcpp32\c\qcqp.c
	// can access this property w/o accessing the P_Profile
	// directly
	EXPORTED BOOL IsProfileEnabled();

private:
	// Build helpers:
	BuildResults BuildAsExternalMakefile(BOOL bBuildAll, CErrorContext & EC = g_DummyEC); 

	BuildResults RunSchmoozeTool
	(
		CSchmoozeTool * pTool, 
		BOOL bContinueAfterErrors,
		CDir & rMyDir,
		CTempMaker & rTM,
		CErrorContext& EC
	);

public:		
	// Get the OPT File name
	void GetOPTFileName(CPath & MSFPath);
	
	// Save the project's private data:
	BOOL LoadFromOPT(CStateSaver& stateSave);
	BOOL SaveToOPT(CStateSaver& stateSave);
	
	// Convert the external stream when converting an external makefile
	// to an internal makefile
	BOOL ConvertOPTFile(CArchive & ar);

protected:
	BOOL LoadExternalFromOPT(CStateSaver& stateSave, BOOL bIsExternal);
	BOOL LoadInternalFromOPT(CStateSaver& stateSave, BOOL bIsExternal,BOOL bWorkspace);
	BOOL SaveExternalToOPT(CStateSaver& stateSave, BOOL bIsExternal);
	BOOL SaveInternalToOPT(CStateSaver& stateSave, BOOL bIsExternal,BOOL bWorkspace);

	// Helper for setting props when converting OPT file.
	BOOL SetStrPropConfigs(CPtrList * plstpcr, UINT idProp, const CString& val);
	BOOL SetIntPropConfigs(CPtrList * plstpcr, UINT idProp, int val);

	HGLOBAL m_hOPTData;

public:
	// Read/write makefile.  Assume file name has already been set:
	enum ReadMakeResults
	{
		ReadExternal,
		ReadInternal,
		ReadError,
		ReadExportedMakefile,
	};

	// .MAK reading
	ReadMakeResults ReadMakeFile (BOOL& bProjectDirty);
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);
#ifdef VB_MAKEFILES
	void OpenVBProject(const TCHAR *szFileName);
#endif
	
	// Read and write comments listing what configurations there are and
	// what project types they reference.  Also write our the current active
	// configuration and read it back in:
	BOOL WriteConfigurationHeader(CMakFileWriter& mw);
	BOOL ReadConfigurationHeader(CMakFileReader& mr);

	// Process ALL : <targets> line
	BOOL ReadMasterDepsLine(CMakDescBlk * pObject, const CDir & BaseDir);

#ifdef EXT_FILES
	BOOL BuildFilesList ();
#endif

	// TRUE if the .MAK file was read-only last time we checked.  Of
	// course, its state may have changed; call RecheckReadOnly()
	// to update this variable with a check of the file on disk.
	BOOL		m_bProjWasReadOnly;

private:
	BOOL IsCustomBuildMacroComment(CObject* pObject);
	BOOL ReadCustomBuildMacros(CMakFileReader& mr);
	BOOL WriteCustomBuildMacros(CMakFileWriter& mw, CString &strFiles );

public:		
	// Set m_bProjWasReadOnly to FALSE.  Useful after warning the user about
	// a project being read-only.
	void		ResetReadOnly() { m_bProjWasReadOnly = FALSE; }

	// Set the value of m_bProjWasReadOnly by checking the .MAK file
	// on disk to see if it's read-only or not.
	void		RecheckReadOnly() { m_bProjWasReadOnly = GetFilePath()->IsReadOnlyOnDisk(); }

	// TRUE if the project was read-only last time we looked.  This
	// state can be 'refreshed' by calling RecheckReadOnly().
	BOOL		WasReadOnly() const { return m_bProjWasReadOnly; }

	// Write the all : <all targets> line.
	// Public because it gets called in CProjItem::WriteToolsSection()
	// If you don't want a particular target on the all line then
	// you can filter it out in FilterMasterDepsLine.
	friend BOOL FilterMasterDepsLine(DWORD dw, FileRegHandle frh);
	BOOL WriteMasterDepsLine(CMakFileWriter& mw, const CDir &BaseDir, BOOL bPostBuildDep = FALSE );

	// our configuration information
	CString		m_strProjActiveConfiguration;
	CString		m_strProjDefaultConfiguration; // CFG= line from makefile
	CString		m_strProjStartupConfiguration; // Primary/Supported cfg from makefile
	
	// Makefile has been freshly converted from an external makefile
	BOOL		m_bProjExtConverted;

	// This flag tells us whether this is a special "exe project" -- that is,
	// the user loaded an exe file in order to debug it and we created a temporary
	// external project for him.  This generally behaves like an external makefile
	// EXCEPT that we don't mess with any corresponding OPT file.
	BOOL		m_bProjIsExe;

	// Makefile has been freshly converted from an 'old' makefile version
	BOOL		m_bProjConverted;

	// Cached flag indicating whether project has at least one supported config
	// This needs to be adjusted anywhere a project configuration can be
	// created or deleted, such as in makefile reading and the targets dialog.
	BOOL		m_bProjIsSupported;

	void GetName(CString &strName ){ 
		ConfigurationRecord * pcr = GetActiveConfig();
		if(pcr)
			pcr->GetProjectName(strName);
	};

	CString		m_strProjItemName;
	CString		m_strProjSccProjName;
	CString		m_strProjSccRelLocalPath;

	// Special properties that are only set when read in from the makefile.
	BOOL		m_bProjMarkedForScan;
	BOOL		m_bProjMarkedForSave;	// can be set by AppWizard
	BOOL		m_bProjHasNoOPTFile;
	BOOL		m_bCareAboutRCFiles;	
	BOOL		m_bCareAboutDefFiles;
	BOOL		m_bProj_PerConfigDeps;

	// delete configuration named strName, and changed the active configuration
	// to strNewConfig.
	// strNewConfig configuration must already exist in the configuration map.
	BOOL	DeleteConfig ( const CString& strName , const CString& strNewConfig = "");

	// Create a new congugration.  Note that creation is a three step process:
	// 1. Create new config.  2. Initizlize new configs type and so on.
	// 2. Make new config active (even if you'll deactivate immediadly)
	BOOL 	CreateConfig ( const CString& strName );

	// CTargIncl: a list of per-target includes maintained by the project (ie: duplicates
	// from the properties) so that we can forward this information to the parser when
	// it calls ResolveIncludeDirectives() without Changing Configuration (the list
	// is protected by a critical section):
	CTargIncl	m_listTargIncl;
protected:
	ConfigurationRecord *CreateConfigRecordForConfig(
												const CString & strConfig);
	DECLARE_SLOBPROP_MAP()

public:
	// Return the platform object corresponding to the current target.
	EXPORTED CPlatform * GetCurrentPlatform();

	// Update the browser database file
	EXPORTED UpdateBrowserDatabase();

	// Makefile errored during open due to requirement to use MFC and MFC is not installed
	BOOL		m_bGotUseMFCError;

	// The Configuration manager api
	//
	// Set the currently active target configuration
	// If 'fQuite' is TRUE then no UI changes will be apparent.
	ConfigurationRecord * SetActiveConfig(ConfigurationRecord *, BOOL fQuite = TRUE);
	ConfigurationRecord * SetActiveConfig(const CString& strConfigName, BOOL fQuite = TRUE);

	// The Target manager api
	//
	// Get the target item associated with a given target name
 	CTargetItem* GetTarget(const TCHAR * pchTargetName);

	// Get the target item corresponding to the currently selected config
	CTargetItem* GetActiveTarget();

	// Register a target with the target manager. This is used during
	// project loading only
	BOOL RegisterTarget(const CString& strTargetName, CTargetItem* pTarget);

	// De-register a target with the target manager.
	BOOL DeregisterTarget(const CString& strTargetName, CTargetItem* pTarget);

	// Rename a target already registered with the target manager.
	BOOL RenameTarget(const CString& strTargetFrom, const CString& strTargetTo);

	// Creates a new target (actually creates a new config and a new
	// CTargetItem)
	BOOL CreateTarget(const CString& strTargetName, CProjType* pProjType, const CString& strMirrorFrom = _T(""));

	// Deletes an existing target
	BOOL DeleteTarget(const CString& strTargetName);

	ConfigurationRecord * FindTargetConfig(CString & strTargetName);

	// Target enumeration
	void InitTargetEnum() { m_posTarget = m_TargetMap.GetStartPosition(); }
	BOOL NextTargetEnum(CString& strTargetName, CTargetItem* & pTarget)
	{
		if (m_posTarget == (POSITION)NULL)
			return FALSE;
		m_TargetMap.GetNextAssoc(m_posTarget, strTargetName, (void* &)pTarget);

		// 2173: need to get name with correct case preserved
		VERIFY(FindTargetConfig(strTargetName)!=NULL);

		return TRUE;
	}

private:

	// This map contains the names of targets and the corresponding
	// CTargetItem ptrs.
	CMapStringToPtr m_TargetMap;
	POSITION		m_posTarget;

    // The project dirty flag. Corresponds to old project doc dirty flag
protected:
    BOOL    m_bProjectDirty;    // Is this project object dirty??
private:
    BOOL    m_bOkToDirty;       // Ok to dirty project

    COptionTreeCtl *    m_pOptTreeCtl;  // The Build.Settings 'view' (if active)    
	virtual void DirtyProjectEx();

	CRITICAL_SECTION m_critSection;

public:

    // Project dirty flag access functions
	__inline void DirtyProject()
	{
		if ((!m_bProjectDirty) && (m_bOkToDirty) && (!g_bInProjClose))
			DirtyProjectEx();
		m_bDirtyDeferred = !m_bProjectDirty;
	}
	__inline void CleanProject()
	{
		m_bProjectDirty = m_bDirtyDeferred = FALSE;
	}
	__inline BOOL IsDirty()
	{
		return m_bProjectDirty;
	}
	__inline BOOL IsOkToDirty()
	{
		return (m_bOkToDirty && !g_bInProjClose);
	}
	__inline BOOL IsDirtyDeferred()
	{
		return m_bDirtyDeferred;
	}

	__inline void SetOkToDirtyProject(BOOL bOk = TRUE)
	{
		m_bOkToDirty = bOk;
	}

	__inline void SetOptionTreeCtl(COptionTreeCtl * pOptTreeCtl)
	{
		m_pOptTreeCtl = pOptTreeCtl;
	}
	__inline COptionTreeCtl * GetOptionTreeCtl()
	{
		return m_pOptTreeCtl;
	}
    
	// Add/Delete file notification batching project wide. That is
	// if you want all notifications project wide to be batched use
	// these apis, rather than using the similar target item apis.
	__inline static void BeginBatch()
	{
		m_nBatch++;
	};
	void static EndBatch();
	BOOL IsNotifyBatching()
	{
		return (m_nBatch > 0);
	};

private:
	static int m_nBatch;
	BOOL m_bSettingsChanged;

public:
	// .MAK reading (conversion of old VC++ 2.0 projects)
	BOOL m_bConvertedVC20;
	// .MAK reading (conversion of old DS 4.x projects)
	BOOL m_bConvertedDS4x;
	// .DSP reading (conversion of old DS 5.x projects)
	BOOL m_bConvertedDS5x;
	CDir m_ActiveTargDir;

	CPath GetMrePath(ConfigurationRecord* pcr);
private:
	BOOL    m_bDirtyDeferred;   // Mark dirty, but do it "later"
	int		m_nDeferred;		// ref counting

public:
static const CObList * GetProjectList() { return &m_lstProjects; }
static void InitProjectEnum();
static const CProject * NextProjectEnum(CString & strBuilder, BOOL bOnlyLoaded = TRUE);
static void GlobalCleanup();
__inline BOOL IsLoaded() const { return m_bProjectComplete; /* REVIEW */ }
public:

private:
static POSITION m_posProjects;
static CObList m_lstProjects;

public:
	void ReleaseInterface( void ){
		if( m_pIProject ) {
			pCIProject->SetThis( NULL );
			m_pIProject->Release();
			m_pIProject = NULL;
		}
	};

	LPBSPROJECT GetInterface(void){
		if( m_pIProject ) {
			m_pIProject->AddRef();
		} else {
			pCIProject = new CProjectInterfaces;
			m_pIProject = pCIProject->GetInterface();
			pCIProject->SetThis(this);
		}
		return m_pIProject;
	}

private:
	LPBSPROJECT 		m_pIProject;
	CProjectInterfaces *pCIProject;

public:
	CProjConfigEnum m_ConfigEnum;

// Java interfaces:
public:
	// Java: return the class name to debug
	EXPORTED void GetJavaClassName(CString& str);
	// Java: return the full path to the class file
	EXPORTED BOOL GetJavaClassFileName(CString& str);
	// Java: return the tool to debug under
	EXPORTED int GetJavaDebugUsing();
	// Java: return the browser
	EXPORTED void GetJavaBrowser(CString& str);
	// Java: return the stand-alone interpreter
	EXPORTED void GetJavaStandalone(CString& str);
	// Java: return the stand-alone interpreter arguments
	EXPORTED void GetJavaStandaloneArgs(CString& str);
	// Java: return the HTML page
	EXPORTED BOOL GetJavaHTMLPage(CString& str);
	// Java: get the class path
	EXPORTED void GetJavaClassPath(CString& str);
	// Java: get the type of stand-alone debugging
	EXPORTED int GetJavaStandaloneDebug();

	// Display a dialog to obtain the class file and/or
	// app type for Java debugging/executing.
	EXPORTED BOOL GetInitialJavaInfoForDebug(BOOL bExecute = FALSE);


	// Used to read/convert old VC 4.x and prior .mdp/.vcp files
	BOOL LoadFromVCP(CStateSaver& stateSave);

protected:
	EXPORTED BOOL GetPreCompiledHeaderName(CString &strPch );
	BOOL LoadExternalFromVCP(CStateSaver& stateSave, BOOL bIsExternal);
	BOOL LoadInternalFromVCP(CStateSaver& stateSave, BOOL bIsExternal);
	BOOL ConvertVCPFile(CArchive & ar);
	BOOL ConvertDirs();
public:
	UINT GetGlyphIndex();
	const CString & GetTargetName();
	__inline void SetTargetName(const CString & str) { m_strTargetName = str; }
private:
	CString m_strTargetName;
	BOOL m_bPrivateDataInitializedFromOptFile;

public:
	static CString s_strWebBrowser;
	static CString s_strTestContainer;
	int m_nScannedConfigIndex;
	int GetScannedConfigIndex() { return m_nScannedConfigIndex; }
#ifdef VB_MAKEFILES
public:
	BOOL m_bVB;
	CString m_strVBProjFile;
	VBInfo *m_pVBinfo;
#endif
};	  


class BLD_IFACE CProjDeferDirty 
{
public:
	CProjDeferDirty(CProject * pProject)
		{
			m_pProject = pProject;
			if (m_pProject!=NULL)
			{
				
				m_pProject->m_nDeferred++;
				m_bWasOk = m_pProject->IsOkToDirty();
				m_pProject->SetOkToDirtyProject(FALSE);
			}

		}
	~CProjDeferDirty()
		{
			if (m_pProject!=NULL)
			{
				m_pProject->SetOkToDirtyProject(m_bWasOk);
				if ((--(m_pProject->m_nDeferred)==0) && (m_bWasOk) && (m_pProject->IsDirtyDeferred()))
					m_pProject->DirtyProject();
			}
		}
private:
	CProject * m_pProject;
	BOOL m_bWasOk;
};

// This object is to used whenever the project's configuration needs to be changed
// temporarily. Do not call SetActiveConfig directly, unless you want to permanently 
// change the active configuration. Declare a local instance of this object and call
// 'ChangeConfig' on it. The destructor of this object will restore the original config
// back. The primary purpose of this is to make the calls to SetActiveConfig thread safe.

class BLD_IFACE CProjTempConfigChange 
{
public:
	CProjTempConfigChange(CProject *pProject);
	// Will Release critical section and reset to original config.
	~CProjTempConfigChange();

	VOID ChangeConfig(ConfigurationRecord *);
	VOID ChangeConfig(const CString& strName);

	// Resets config to the original one, still holds the crit section.
	VOID Reset();	

	// releases critical section, if bReset is TRUE also resets to original config.
	VOID Release(BOOL bReset = TRUE); 

	CProject * m_pProject;

private:
	ConfigurationRecord * m_pcrOriginal;
	BOOL m_bCritSection ; // Do we have the critical section currently.
};


// This object is used when the active project needs to be temporarly changed
// usually during the build of a sub project.

class BLD_IFACE CProjTempProjectChange 
{
public:
	CProjTempProjectChange (CProject *pProject);
	~CProjTempProjectChange ();

	VOID Release (); 

	CProject *m_pPrevProject;
};

// This object is used when the UI display state needs to be disabled and rer enabled.

class BLD_IFACE CTempDisableUI
{
public:
	CTempDisableUI();
	~CTempDisableUI();
	BOOL m_bOldUI;
};


extern CProject *g_pActiveProject;
extern CProject *g_pConvertedProject;

// REVIEW: This should be made a member of CProject before Version 5.0
// as should g_pProjSysIFace. 4.1 restrictions make changes to CProject difficult
// at this time.  KPerry
extern LPPROJECTWORKSPACE g_pProjWksIFace;
extern LPPROJECTWORKSPACEWINDOW g_pProjWksWinIFace;
extern LPSOURCECONTROL g_pSccManager;

__inline LPPROJECTWORKSPACE FindProjWksIFace()
{
	ASSERT(g_pProjWksIFace != NULL);
	return(g_pProjWksIFace);
}
 
__inline LPPROJECTWORKSPACEWINDOW FindProjWksWinIFace()
{
	ASSERT(g_pProjWksWinIFace != NULL);
	return(g_pProjWksWinIFace);
}

#endif // __PROJECT_H__

