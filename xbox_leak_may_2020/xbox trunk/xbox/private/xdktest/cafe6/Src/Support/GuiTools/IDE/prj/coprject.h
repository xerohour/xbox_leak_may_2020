///////////////////////////////////////////////////////////////////////////////
//  COPRJECT.H 
//
//  Created by :            Date :
//      IvanL               1/17/94
//															
//  Description :
//      Declaration of the COProject class
//

#ifndef __COPROJECT_H__
#define __COPROJECT_H__

#include "..\..\testutil.h"
#include "..\sym\projprop.h"
#include "..\shl\uwbframe.h"
#include "..\shl\wbutil.h"
#include "uprojwiz.h"
#include "uprjtdlg.h"
#include "..\wrk\uiwrkspc.h"
#include "uogaldlg.h"
#include "cocompon.h"
#include "uictrlwz.h"

#include "prjxprt.h"

#define ERROR_ERROR -1

// option for SetLocalTarget()
typedef enum {NO_DEFAULT_DBG_EXE = 0, DEFAULT_DBG_EXE_CONTAINER = 2, DEFAULT_DBG_EXE_BROWSER = 3} DEFAULT_DBG_EXE_TYPE;
// for SetDebugInfo()
enum DEBUG_INFO_TYPE {NO_DEBUG_INFO = 1, LINE_NUMBERS_ONLY, C7_COMPATIBLE, PROGRAM_DATABASE, EC_PROGRAM_DATABASE};
// options for SetRemotePath()
typedef enum {PREPEND_REMOTE_PATH, FULL_PATH_SUPPLIED} SRT_OPT;
// build types
enum BUILD_TYPE	{BUILD_DEBUG = IDSS_BUILD_DEBUG, BUILD_RELEASE = IDSS_BUILD_RELEASE};

// target types
typedef enum {TARGET_WIN32_X86_DEBUG, TARGET_WIN32_X86_RELEASE,
			  TARGET_MAC_68K_DEBUG, TARGET_MAC_68K_RELEASE,
			  TARGET_MAC_PPC_DEBUG, TARGET_MAC_PPC_RELEASE,
			  // added xbox types
			  TARGET_XBOX_DEBUG, TARGET_XBOX_RELEASE,
			  TARGET_USER, TARGET_CURRENT} TARGET_TYPE;

#define CURRENT_CONFIG	0
#define ALL_CONFIGS		1

// symbols to make New() more readable.

#define CUR_DIR			NULL		// don't change directory when creating project.
#define NO_NEW_SUB_DIR	""			// don't create new subdirectory for project.

// used by SetCustomBuildOptions()
#define EXCLUDE_CONFIG_LEVEL 0
#define INCLUDE_CONFIG_LEVEL 1

// File adding options for New(), correspond to UIWBFrame::CreateNewProject() options.

#define ADD_ALL_SOURCE		0x00000001
#define ADD_ALL_HEADERS		0x00000002
#define ADD_ALL_RES_SCRIPTS	0x00000004
#define ADD_ALL_DEF_FILES	0x00000008
#define ADD_ALL_ODL_FILES	0x00000010
#define ADD_ALL_LIBS		0x00000020
#define ADD_ALL_OBJ_FILES	0x00000040
#define ADD_ALL_FILES		0x00000080
#define ADD_FILE			0x00000100
#define ADD_FILES_MYSELF    0x00000200

// SetOptions constants. //these work for all controls.
#define OPTION_ON			1
#define OPTION_OFF			2

// Project properties categories
// MOVED TO UPRJTDLG.H.

// Project properties
typedef enum {PR_TARGET, PR_BUILD_CMD} PROJ_PROPERTIES;

// AppWizard app properties.

// BEGIN_CLASS_HELP
// ClassName: APPWIZ_OPTIONS
// BaseClass: none
// Category: Project
// END_CLASS_HELP
struct PRJ_CLASS APPWIZ_OPTIONS
{
		APPWIZ_OPTIONS();	 
	 	BOOL m_bUseMFCDLL;

		enum OLESupport { AW_OLE_NONE, AW_OLE_CONTAINER, AW_OLE_MINISERVER, AW_OLE_FULLSERVER, AW_OLE_CONTAINERSERVER };
		OLESupport m_OLECompound;
		
		// Review: These are not yet used.
		int m_Type ;
	 	int m_DBase ;
	 	int m_OLEAuto ;
	 	int m_ToolBar ;
	 	int m_StatusBar ;
	 	int m_Printing ;
	 	int m_Help ;
	 	int m_Language ;
	 	int m_Comments ;
	 	int m_VCMakefile ;
		int m_AboutBox ;
};

						
// New Project properties.

// BEGIN_CLASS_HELP
// ClassName: PROJOPTIONS
// BaseClass: none
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS PROJOPTIONS
{	public:
		LPCSTR szName ;
		LPCSTR szDir; 		
		PROJTYPE iType ;
		LPCSTR szNewSubDir;
		// For VCE
		int ProjFileOpt ;
		LPCSTR szProjFilesPath ;
		LPCSTR szRemotePath ;
		LPCSTR szRemoteFile ;
	
		PROJOPTIONS(LPCSTR Name, PROJTYPE Type)
		{	szName = Name;
			iType = Type;
			szNewSubDir = NULL ;
			szDir = NULL ;
			ProjFileOpt = 0 ;
			szProjFilesPath = NULL;
			szRemotePath = NULL;
		 	szRemoteFile = NULL ;
		}

	PROJOPTIONS(LPCSTR Name)
		{	ASSERT(0) ; // We should always have a project name and type.
			UNREFERENCED_PARAMETER(Name);
		}

		PROJOPTIONS()
		{   szName = "" ;
			iType = UIAW_PT_APP ;
			szDir = "" ;
			szNewSubDir = "";
			ProjFileOpt = CNP_ADD_FILES_MYSELF ;
			szProjFilesPath = "";
			szRemotePath = "" ;
		 	szRemoteFile = "" ;
		}
} ;


// BEGIN_CLASS_HELP
// ClassName: CProjWizOptions
// BaseClass: none
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS CProjWizOptions

{
public:
	
	// sets defaults for options all projects share.
	CProjWizOptions(void);
	
	// options that all projects share.
	PROJTYPE m_ptType;
	CString	m_strName;
	CString m_strLocation;
	PROJ_WORKSPACE m_pwWorkspace;
	PROJ_HIERARCHY m_phHierarchy;
	CString m_strParent; 
	int m_iPlatforms;

	// handles the project-specific wizard.
	virtual BOOL WizardFunc(void) = 0;
};


// BEGIN_CLASS_HELP
// ClassName: CControlWizOptions
// BaseClass: CProjWizOptions
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS CControlWizOptions : public CProjWizOptions

{
public:
	
	// sets defaults for control wizard options.
	CControlWizOptions(void);

	// step 1 options.
	int m_iNumControls;
	BOOL m_bRuntimeLicense;
	BOOL m_bGenerateComments;
	BOOL m_bGenerateHelpFiles;

	// step 2 options.
	// TODO(michma): support editing of class and file names.
	BOOL m_bActivatesWhenVisible;
	BOOL m_bInvisibleAtRuntime;
	BOOL m_bAvailableInInsertObjectDlg;
	BOOL m_bAboutBox;
	BOOL m_bSimpleFrameControl;
	// TODO (michma): support subclass option.
	// TODO(michma): support advanced options.

	// handles the project-specific wizard.
	virtual BOOL WizardFunc(void);
};


// BEGIN_CLASS_HELP
// ClassName: CATLCOMWizOptions
// BaseClass: CProjWizOptions
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS CATLCOMWizOptions : public CProjWizOptions

{
public:
	
	// sets defaults for atl com wizard options.
	CATLCOMWizOptions(void);
	
	// for specifying the atl com project's server type.
	typedef enum {ATL_COM_SERVER_DLL = 1, ATL_COM_SERVER_EXE, ATL_COM_SERVER_SERVICE} ATL_COM_SERVER_TYPE;

	// step 1 options.
	ATL_COM_SERVER_TYPE m_acstServerType;
	BOOL m_bAllowMergingOfProxyStubCode;
	BOOL m_bSupportMFC;

	// handles the project-specific wizard.
	virtual BOOL WizardFunc(void);
};


// BEGIN_CLASS_HELP
// ClassName: CWin32AppWizOptions
// BaseClass: CProjWizOptions
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS CWin32AppWizOptions : public CProjWizOptions
{
public:
	// sets defaults for win32 app wizard options.
	CWin32AppWizOptions(void);
	// for specifying the win32 app type.
	typedef enum {EMPTY_PROJECT = 1, SIMPLE_APP, HELLO_WORLD_APP} APP_TYPE;
	// step 1 options.
	APP_TYPE m_AppType;
	// handles the project-specific wizard.
	virtual BOOL WizardFunc(void);
};


// BEGIN_CLASS_HELP
// ClassName: CISAPIExtensionWizOptions
// BaseClass: CProjWizOptions
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS CISAPIExtensionWizOptions : public CProjWizOptions
{
public:
	// sets defaults for isapi extension wizard options.
	CISAPIExtensionWizOptions(void);
	// for specifying the way to link to mfc.
	typedef enum {LINK_AS_SHARED_DLL = 1, LINK_AS_STATIC_LIB} MFC_LINKAGE;
	// step 1 options.
	MFC_LINKAGE m_MFCLinkage;
	BOOL m_bGenerateFilter;
	CString m_strFilterClassName;
	CString m_strFilterDescription;
	BOOL m_bGenerateExtension;
	CString m_strExtensionClassName;
	CString m_strExtensionDescription;
	// handles the project-specific wizard.
	virtual BOOL WizardFunc(void);
};


// BEGIN_CLASS_HELP
// ClassName: CWin32DllWizOptions
// BaseClass: CProjWizOptions
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS CWin32DllWizOptions : public CProjWizOptions
{
public:
	// sets defaults for win32 dll wizard options.
	CWin32DllWizOptions(void);
	// for specifying the win32 dll type.
	typedef enum {EMPTY_PROJECT = 1, SIMPLE_DLL, DLL_EXPORTS_SYMBOLS} DLL_TYPE;
	// step 1 options.
	DLL_TYPE m_DllType;
	// handles the project-specific wizard.
	virtual BOOL WizardFunc(void);
};


// BEGIN_CLASS_HELP
// ClassName: CXboxGameOptions
// BaseClass: CProjWizOptions
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS CXboxGameOptions : public CProjWizOptions
{
public:
	// sets defaults for xbox game options.
	CXboxGameOptions(void);
	// add add'l fields here
	virtual BOOL WizardFunc(void);
};


// BEGIN_CLASS_HELP
// ClassName: CXboxStaticLibOptions
// BaseClass: CProjWizOptions
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS CXboxStaticLibOptions : public CProjWizOptions
{
public:
	// sets defaults for xbox game options.
	CXboxStaticLibOptions(void);
	// add add'l fields here
	virtual BOOL WizardFunc(void);
};


// builds configuration strings (i.e. "test - Win32 Debug")

// BEGIN_CLASS_HELP
// ClassName: ConfigStr
// BaseClass: none
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS ConfigStr
	
	{
	private:
	
		CString m_cstrProj;
		PLATFORM_TYPE m_ptPlatform;
		BUILD_TYPE m_btBuild;
		CString m_cstrConfig;

		CString BuildString(void);
	
	public:

		ConfigStr(void){}
		ConfigStr(LPCSTR szProj, PLATFORM_TYPE ptPlatform, BUILD_TYPE btBuild);

		CString SetProject(LPCSTR szProj);
		CString SetPlatform(PLATFORM_TYPE ptPlatform);
		CString SetBuild(BUILD_TYPE btBuild);
		CString SetString(LPCSTR szProj, PLATFORM_TYPE ptPlatform, BUILD_TYPE btBuild);

		CString GetProject(void){return m_cstrProj;}
		PLATFORM_TYPE GetPlatform(void){return m_ptPlatform;}
		BUILD_TYPE GetBuild(void){return m_btBuild;}
		CString GetString(void){return m_cstrConfig;}
	};


// see ConfigStr.

// BEGIN_CLASS_HELP
// ClassName: ConfigStrArray
// BaseClass: none
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS ConfigStrArray

	{
	private:
		CPtrArray m_configs;

	public:

		CString Add(LPCSTR szProj, PLATFORM_TYPE ptPlatform, BUILD_TYPE btBuild);
		CString Add(ConfigStr &config_str);
		CString InsertAt(int intIndex, LPCSTR szProj, PLATFORM_TYPE ptPlatform, BUILD_TYPE btBuild);
		CString InsertAt(int intIndex, ConfigStr &config_str);

		int GetSize(void){return m_configs.GetSize();}
		ConfigStr operator[](int intIndex);

		void RemoveAt(int intIndex, int intCount = 1){m_configs.RemoveAt(intIndex, intCount);}
		void RemoveAll(void){m_configs.RemoveAll();}
 	};


// Stuff needed for Class View verification.

// BEGIN_CLASS_HELP
// ClassName: CMemberInfo
// BaseClass: none
// Category: Project
// END_CLASS_HELP
struct CMemberInfo
{
	enum TYPE { FUNCTION, DATA };
	enum ACCESS { PUBLIC, PRIVATE, PROTECTED, GLOBAL };

	CString		m_strName;					// Name of member.
	CString		m_strDataType;				// Return type for functions, data type for data.
	TYPE		m_Type; 					// Function or data?
	ACCESS		m_Access;					// Public, private, protected, or global?

	~CMemberInfo();

    void ClearAll();

    LPCSTR GetName() const { return (LPCSTR)m_strName; }
	LPCSTR GetDataType() const { return (LPCSTR)m_strDataType; }
	TYPE GetType() const { return m_Type; }
	ACCESS GetAccess() const { return m_Access; }

};

// BEGIN_CLASS_HELP
// ClassName: CGlobalInfo
// BaseClass: none
// Category: Project
// END_CLASS_HELP
typedef CMemberInfo CGlobalInfo;			// They're treated identically.

// BEGIN_CLASS_HELP
// ClassName: CClassInfo
// BaseClass: none
// Category: Project
// END_CLASS_HELP
struct CClassInfo 
{
    CString m_strName;									// Name of class.
    CString m_strBase;									// Name of base class.
	CTypedPtrList<CPtrList, CMemberInfo*> m_listMembers;

	~CClassInfo();

    void ClearAll();
    LPCSTR GetName() const { return (LPCSTR)m_strName; }
};

// BEGIN_CLASS_HELP
// ClassName: COProject
// BaseClass: none
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS COProject
{
// ctor/dtor
public:
	COProject(void);
	~COProject();

public:
// components
	BOOL AddComponent(COComponent *pComponent, LPCSTR szLocation = NULL);
	BOOL EnableComponents(LPCSTR szLocation = NULL);
	BOOL DisableComponents(void);
public:
	int InsertNewATLObject(LPCSTR szObjectName);

private:
	// used to track whether components are being added in batches or not
	// (i.e. if the dlg is already open when AddComponent is called, we are in batch mode).
	UIOGalleryDlg m_uigal;

public:

// new/open/attach
	int New(LPCSTR szProjName, PROJTYPE ProjType , LPCSTR szDir = NULL,
			 LPCSTR szNewSubDir = NULL, int ProjFileOpt = CNP_ADD_FILES_MYSELF,
			 LPCSTR szProjFilesPath = NULL,
			 LPCSTR szRemotePath = NULL,
			 LPCSTR szRemoteFile = NULL);
	
	int BuildNew(LPCSTR szProjName, PROJTYPE ProjType, LPCSTR szDir = NULL,
			 	  LPCSTR szNewSubDir = NULL, int ProjFileOpt = 0,
			 	  LPCSTR szProjFilesPath = NULL,
			 	  LPCSTR szRemotePath = NULL,
				  LPCSTR szRemoteFile = NULL);
	
	int New(PROJOPTIONS &stOptions );
	
	int New(CProjWizOptions *pProjWizOpt);
	
	int NewProject(PROJTYPE ptProjType, LPCSTR szProjName, LPCSTR projdir, 
				  int intPlatforms, BOOL AddToWorkspace=FALSE);

	int InsertProject(PROJTYPE ptProjType, LPCSTR szProjName, int intPlatforms, 
					  PROJ_HIERARCHY phProjHier = TOP_LEVEL_PROJ, 
                                          LPCSTR szTopLevelProj = NULL);

	int NewAppWiz(LPCSTR projname, LPCSTR projdir, int platforms, const APPWIZ_OPTIONS* = NULL, LPCSTR apptype = NULL);

private:
	// shared by NewProject, InsertProject, and NewAppWiz.
	void SetNewProjectPlatforms(int intPlatforms);
	void ReplaceExistingProjectIfNecessary(void);
	int WaitForProjectNameToBeIncludedInIDETitle(LPCSTR szProjName);

public:

	int Open(LPCSTR szProjName = NULL, int iMethod = 1, BOOL LogWarning = TRUE) ;
    HWND AppWizard(int iClose = FALSE);

	int Attach(void);

// save
	int Save(int wait = FALSE, LPCSTR file = NULL);
	int SaveAs(LPCSTR szFileName) {return FALSE ; UNREFERENCED_PARAMETER(szFileName); } ;

// close
	int Close(int iSave = 0) ;

// delete
	int DeleteMakeFile(); 		
	int DeleteFromProjectDir(LPCSTR strDelTarget = NULL);
	int RemoveIntermediateDir();
	int RemoveTargetDir();		

// build
	int Build(int iWait = 3, DWORD interval=1000) ;
	int RebuildAll(int iWait = 3, DWORD interval=1000) ; 
	int StopBuild();
	int VerifyBuild(BOOL LogErrors = TRUE,int * iErr= NULL, int * iWarn = NULL, BOOL WrnsAsErrs = FALSE) ;
   	int GetErrorCount() ;
	int GetWarningCount() ;
	int UpdateRemoteFile(LPCSTR local_file = NULL,
						 TARGET_TYPE target = TARGET_CURRENT,
						 PLATFORM_TYPE platform = PLATFORM_USER);	

	int WaitUntilBuildDone(int Min = 3, DWORD dwSleep=1000) ;
    int ExportMakefile();

// ClassView orientation
	void VerifyClasses(LPCSTR szBaseLineFilename);
	void VerifySingleClass(const CClassInfo&);
	void VerifySingleGlobal(const CGlobalInfo&);
	void NavigateToDeclaration(const CClassInfo&, LPCSTR);
	void NavigateToDeclaration(const CGlobalInfo&);
	void NavigateToDefinition(const CClassInfo&, LPCSTR);

private:
	void GetClassInfo(CStdioFile&, CClassInfo *);
	void GetGlobalInfo(CStdioFile&, CGlobalInfo *);

// execute
public:
	int Execute(int iRebuild = 0) ;

// project attributes
	LPCSTR GetFullPath() 		{return (const char *) m_FullPath ; };
	LPCSTR GetName() 			{return (const char *) m_ProjName ;} ;
	LPCSTR GetDir() 			{return (const char *) m_ProjDir ;} ;
	LPCSTR GetIntermediateDir(); // This is in the General section and is a relative path
	LPCSTR GetOutputFile();		 // This is in the Linker section and contructs the full path
	LPCSTR GetOutputXbeFile();   // This is in the Xbox Image (ImageBld) section and contructs the full path

	BOOL IsOpen(void);

// manipulate files
	int AddFiles		(LPCSTR szFileNames, LPCSTR szGroup ="") ;
	int RemoveFiles	(LPCSTR szFileNames, LPCSTR szGroup= "" ) ;
	int RemoveAll		(LPCSTR	szGrpName = NULL) ;
	int SelectFile		(LPCSTR szFileName,LPCSTR szGrpName = "&*%#~**********") ;
	int SelectDependentFile(CString File,CString Group = "") ;

	int GetFileProperty(PROJ_PROP_CATEGORIES iCategory, LPCSTR szGrpName , LPCSTR szFileName,UINT iCtrlId) ;
      CString GetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName ,LPCSTR szFileName,UINT iCtrlId,CString szValue = "") ;

	int SetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName, LPCSTR szFileName ,UINT iCtrlId, UINT iNewValue ) ;
	int SetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName, LPCSTR szFileName ,UINT iCtrlId, LPCSTR szNewValue ) ;

// manipulate groups
	int NewGroup(CString Name) ;
	int RemoveGroup(){ return FALSE ; } ; // REVIEW(Ivan) Implement these.
	int RenameGroup(){ return FALSE ; } ;
	int SelectGroup	(LPCSTR szGrpName) ;
	int SelectDependenciesGroup(CString Group = "") ;

	int GetGroupProperty(PROJ_PROP_CATEGORIES iCategory, LPCSTR szGrpName ,UINT iCtrlId ) ;
      CString GetGroupProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName ,UINT iCtrlId,CString szValue = "") ;

	int SetGroupProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName ,UINT iCtrlId, UINT iNewValue ) ;
	int SetGroupProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName ,UINT iCtrlId, LPCSTR szNewValue ) ;

// manipulate targets
	int AddTarget(LPCSTR Name, int Type , BOOL Debug =FALSE) ;
	int AddTarget(LPCSTR Name, LPCSTR CopyFrom) ;
	int SetTarget(LPCSTR Target);
	int SetTarget(TARGET_TYPE target);
	int DeleteTarget(LPCSTR Target) ;
	int RenameTarget(LPCSTR OldName, LPCSTR NewName) ;

	CString GetRemoteTarget(void);
	LPCSTR GetTarget(int Target = 0) ;
	TARGET_TYPE GetCurrentTarget(void);
	int SetLocalTarget(LPCSTR localfile, LPCSTR config = NULL, DEFAULT_DBG_EXE_TYPE ddetDefaultDbgExe = NO_DEFAULT_DBG_EXE);
	int SetExeForDebugSession(LPCSTR path, LPCSTR config = NULL, DEFAULT_DBG_EXE_TYPE ddetDefaultDbgExe = NO_DEFAULT_DBG_EXE){return SetLocalTarget(path, config, ddetDefaultDbgExe);}
	int SetOutputTarget(LPCSTR localfile);
	int SetOutputFile(LPCSTR localfile);
	int SetProgramArguments(LPCSTR progargs, LPCSTR config = NULL);
	int SetDebugInfo(DEBUG_INFO_TYPE DebugInfo);

// manipulate project
	int MakeProjectExternal(LPCSTR sName = NULL) ;

	int AddSQLDataSource(CString &DSN,CString &User,CString &Psw,LPCSTR DataBase=NULL);
	int ValidatePassword(CString &User,CString &Psw,LPCSTR DataBase=NULL);

public:
	int SetRemotePath(LPCSTR remote_file,	
					  TARGET_TYPE target = TARGET_CURRENT,
					  SRT_OPT srt_opt = PREPEND_REMOTE_PATH,
					  PLATFORM_TYPE platform = PLATFORM_USER);
	
	int SetAdditionalDLLs(LPCSTR szDLLs);
	int RemoveAdditionalDLLs(int iDll);
	int SetLocateOtherDLLs(TOGGLE_TYPE t);
	int SetPlatform(int platform);
	int ToggleBSCGeneration(TOGGLE_TYPE t);
	
	int SetCustomBuildOptions(LPCSTR szDescription, 
							  CStringArray &cstrArrayBuildCmds,
							  CStringArray &cstrArrayOutputFiles,
							  ConfigStrArray &csArrayConfigs);
	
	int SetCustomBuildOptions(LPCSTR szDescription, 
							  CStringArray &cstrArrayBuildCmds,
							  CStringArray &cstrArrayOutputFiles,
							  ConfigStrArray &csArrayConfigs, 
							  CStringArray &cstrArrayFiles,
							  int intConfigInclusion = INCLUDE_CONFIG_LEVEL);

private:
	
	// used by SetCustomBuildOptions().
	int SetCustomBuildOptionsCore(LPCSTR szDescription, 
								  CStringArray &cstrArrayBuildCmds,
								  CStringArray &cstrArrayOutputFiles);

	// used by SetCustomBuildOptionsCore().
	void SetCustomBuildLists(LPCSTR szListAccel, CStringArray &cstrArrayList);
	
public:						  

    int GetProjProperty(PROJ_PROP_CATEGORIES iCategory,UINT iCtrlId, CString szValue) ;
	CString GetProjProperty(PROJ_PROP_CATEGORIES iCategory,UINT iCtrlId) ;
	int SetProjProperty(PROJ_PROP_CATEGORIES iCategory,UINT iCtrlId, UINT iNewValue ) ;
	int SetProjProperty(PROJ_PROP_CATEGORIES iCategory,UINT iCtrlId, LPCSTR szNewValue ) ;

	CString GetProjectItemProp(int P_PropID)  ;
	int SetBuildCommand(LPCSTR command, int index = 0);

// misc
	BOOL ActivateProjWnd();
	void SetPathMembers() ;

// helper functions
protected:

	// this function is taken directly from uwbframe.h
	HWND CreateNewProject(LPCSTR szProjName, PROJTYPE ProjType, LPCSTR szDir = NULL,
						  LPCSTR szNewSubDir = NULL, int ProjFileOpt = 0,
						  LPCSTR szProjFilesPath = NULL,
						  LPCSTR szRemotePath = NULL,
						  LPCSTR szRemoteFile = NULL);


	UIWorkspaceWindow* NewProjectWindow(void);
	BOOL DeleteProjectWindow(void);

	UIWorkspaceWindow* GetProjectWindow(void);

private:
	HWND hActive ;
   	INT SetOption(INT iCategory,UINT iCtrlId, LPCSTR szNewValue) ;
	INT SetOption(INT iCategory,UINT iCtrlId, UINT szNewValue) ;

// data
protected:
	CString m_FullPath, m_ProjName, m_ProjDir, m_OutputFile, m_InterDir ;
	HWND m_hWnd ;
	UIWorkspaceWindow* m_pProjWnd;
 	UIProjOptionsTabDlg m_uPrjOptDlg;
} ;
#endif //__COPROJECT_H__
