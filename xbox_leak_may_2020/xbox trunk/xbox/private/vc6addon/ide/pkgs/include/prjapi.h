/////////////////////////////////////////////////////////////////////////////
//	PRJAPI.H
//		Project package interface declarations.

#ifndef __PRJAPI_H__
#define __PRJAPI_H__

#ifndef STRICT
typedef DWORD HPACKAGE;
typedef DWORD HSTATESAVER;
typedef DWORD HPROJECT;
#else	// STRICT
DECLARE_HANDLE(HPACKAGE);
DECLARE_HANDLE(HSTATESAVER);
DECLARE_HANDLE(HPROJECT);
#endif	// STRICT

interface IProjectWorkspace;
interface IProjectWorkspaceWindow;
interface IPkgProjectProvider;
interface IPkgProject;
interface ISourceControlStatus;
interface ISourceControl;

typedef IProjectWorkspace* LPPROJECTWORKSPACE;
typedef IProjectWorkspaceWindow* LPPROJECTWORKSPACEWINDOW;
typedef ISourceControlStatus* LPSOURCECONTROLSTATUS;
typedef ISourceControl* LPSOURCECONTROL;

class CDefProvidedNode;

// default extension for the workspace file
#define WORKSPACE_EXT	".dsw"
// default extension for the workspace options file
#define WSOPTIONS_EXT	".opt"

// These interfaces are used by the build system, class view, and information
// presentation to interact with the project and global workspace windows.

/////////////////////////////////////////////////////////////////////////////
// IProjectWorkspace

#undef  INTERFACE
#define INTERFACE IProjectWorkspace
DECLARE_INTERFACE_(IProjectWorkspace, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// IProjectWorkspace methods
	STDMETHOD(OpenWorkspace)(CDocument **ppDoc, LPCSTR pchWorkspace, BOOL bTemporary) PURE; 	// Non-standard COM.
	STDMETHOD(CreateDefaultWorkspace)(CDocument **ppDoc, LPCSTR pchFileToAdd) PURE; 	// Non-standard COM.
	STDMETHOD(CreateEmptyWorkspace)(CDocument **ppDoc, LPCSTR pszWorkspace) PURE; 	// Non-standard COM.
	STDMETHOD(CloseWorkspace)(VOID) PURE;
	STDMETHOD(IsWorkspaceDocument)(CDocument * pDoc) PURE;
	STDMETHOD(SetWorkspaceDoc)(LPCSTR pszDocName) PURE;
	STDMETHOD(GetWorkspaceDocTitle)(LPCSTR *ppszDocTitle) PURE;
	STDMETHOD(GetWorkspaceDocPathName)(LPCSTR *ppszDocPathName) PURE;
	STDMETHOD(IsWorkspaceInitialised)(VOID) PURE;
	STDMETHOD(IsWorkspaceDocDirty)(VOID) PURE;
	STDMETHOD(SetWorkspaceDocDirty)(VOID) PURE;
	STDMETHOD(IsWorkspaceTemporary)(VOID) PURE;
	STDMETHOD(AddProjectType)( HANDLE hIcon, LPCTSTR szName, LPCTSTR szDefaultExtension,
								LPCTSTR szTabName, BOOL bSupportProjDependency, BOOL bWantsProjSubdirs,
								DWORD Project_ID, IPkgProjectProvider *pPP) PURE; 	// Non-standard COM.
	STDMETHOD(ReleaseProjectTypeInfo)() PURE;
	STDMETHOD(GetProjectTypes)(CPtrArray *pProjectTypes) PURE; 	// Non-standard COM.
	STDMETHOD(SetActiveProject)(IPkgProject *pProject, BOOL bFailSilently) PURE;
	STDMETHOD(GetActiveProject)(IPkgProject **ppProject) PURE;
	STDMETHOD(CanAddProject)(LPCTSTR szProjName) PURE;
	STDMETHOD(AddProject)(IPkgProject *pProject, BOOL bSaveWorkspaceDocNow) PURE;
	STDMETHOD(DeleteProject)(IPkgProject *pProject, BOOL bUnloading) PURE;
	STDMETHOD(GetLoadedProjects)(CADWORD *pLoadedProjects) PURE;
	STDMETHOD(IsProjectRegistered)(LPCOLESTR pszProjName, LPCOLESTR * pszProjFullPath) PURE;
	STDMETHOD(RegisterProject)(IPkgProject *pProject) PURE;
	STDMETHOD(CloseProject)(IPkgProject *pProject) PURE;
	STDMETHOD(SerializeWorkspaceState)(CStateSaver * pSS) PURE;
	STDMETHOD(OpenProjectFile)(LPCOLESTR pszFileName, IPkgProject ** ppProject) PURE;
};

////////////////////////////////////////////////////////////////////////////
// IProjectWorkspaceWindow

#undef  INTERFACE
#define INTERFACE IProjectWorkspaceWindow
DECLARE_INTERFACE_(IProjectWorkspaceWindow, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// IProjectWorkspaceWindow methods
	STDMETHOD(AddRootProvidedNode)(CDefProvidedNode *pProvidedNode,	CSlob* pAddSlob) PURE; 	// Non-standard COM.
	STDMETHOD(RemoveSlob)(CSlob *pRemoveSlob, BOOL fRemovePaneIfEmpty) PURE; 	// Non-standard COM.
	STDMETHOD(ActivateContainingPane)(CSlob *pContainedSlob, BOOL bEnsureVisible, BOOL bSetFocusUpon) PURE; 	// Non-standard COM.
	STDMETHOD(FreezeContainingPane)(CSlob *pContainedSlob) PURE; 	// Non-standard COM.
	STDMETHOD(ThawContainingPane)(CSlob *pContainedSlob) PURE; 	// Non-standard COM.
	STDMETHOD(SelectSlob)(CSlob *pSelectSlob, BOOL bClearSelection) PURE; 	// Non-standard COM.
	STDMETHOD(DeselectSlob)(CSlob *pDeselectSlob) PURE; 	// Non-standard COM.
	STDMETHOD(ExpandSlob)(CSlob *pExpandSlob, BOOL bExpandFully) PURE; 	// Non-standard COM.
	STDMETHOD(CollapseSlob)(CSlob *pCollapseSlob) PURE; 	// Non-standard COM.
	STDMETHOD(RefreshSlob)(CSlob *pRefreshSlob) PURE; 	// Non-standard COM.
	STDMETHOD(ScrollSlobIntoView)(CSlob *pViewSlob, BOOL bScrollToTop) PURE; 	// Non-standard COM.
	STDMETHOD(GetVerbPosition)(UINT *pu) PURE;
	STDMETHOD(GetSelectedSlob)(CMultiSlob **ppSlob) PURE; 	// Non-standard COM.
	STDMETHOD(GetSelectedItems)(CMultiSlob *pSlob) PURE;	// Non-standard COM.
	STDMETHOD(IsWindowActive)(VOID) PURE;
	STDMETHOD(IsContainingPaneActive)(CSlob *pContainedSlob) PURE; 	// Non-standard COM.
	STDMETHOD(IsWindowLocked)(VOID) PURE;
	STDMETHOD(PreModalWindow)(VOID) PURE;
	STDMETHOD(PostModalWindow)(VOID) PURE;
	STDMETHOD(BeginUndo)(UINT nID) PURE;
	STDMETHOD(EndUndo)(VOID) PURE;
	STDMETHOD(FlushUndo)(VOID) PURE;
	STDMETHOD(CreateDragger)(CSlobDraggerEx** ppDragger) PURE; 	// Non-standard COM.
	STDMETHOD(GetSlobFromPoint)(POINT *ppt, CSlob **ppSlob) PURE;
	STDMETHOD(GetCurrentSlobRect)(RECT *prt) PURE;
	STDMETHOD(GetSlobRect)(CSlob * pSlob, RECT * prt) PURE;
};

////////////////////////////////////////////////////////////////////////////
// IPkgProjectProvider interface

#undef  INTERFACE
#define INTERFACE IPkgProjectProvider
DECLARE_INTERFACE_(IPkgProjectProvider, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// IPkgProjectProvider methods
	STDMETHOD(AddNewProjectTypes)(IProjectWorkspace *pPW) PURE;
	// ReleaseProjectTypeInfo: This function gets called after the dialog 
	// is dismissed so that the package can delete obsolete data structures.
	STDMETHOD(ReleaseProjectTypeInfo)() PURE;
	STDMETHOD(OpenProject)(LPCOLESTR szFileName, IPkgProject ** ppCreatedProject) PURE;
	STDMETHOD(CanCreateProject)(LPCOLESTR szFileName, DWORD dwProject_ID,
							BOOL *pbCanCreate) PURE;


	// CreateProject: retVal can be CREATE_PROJ_OK, CREATE_PROJ_CANCEL or 
	// CREATE_PROJ_RERUN_PROMPT (when the user chooses ‘Back’ on the first 
	// step of the project wizard).
	STDMETHOD(CreateProject)(LPCOLESTR szFileName, DWORD dwProject_ID,
							IPkgProject **ppCreatedProject,
							DWORD *pdwRetVal) PURE;

	
	// GetPlatforms: Project_ID is the cookie given in IProjectWorkspace.AddProjectType
	// pConfigNames is an array of platform names.
	// pDefaultSelections is an array of which platforms should be selected by default.
	// count is the number of available platforms.
	STDMETHOD(GetPlatforms)(DWORD dwProject_ID,
							LPCOLESTR **ppszTargetNames,
							BOOL **pDefaultSelections,
							ULONG *pCount) PURE;

	// ChangePlatformSelection: This function is called when the user 
	// changes the selected set of platforms. The project provider 
	// is expected to remember which platforms were selected when the 
	// time comes to create a project.
	STDMETHOD(ChangePlatformSelection)(ULONG TargetIndex, BOOL bSelected) PURE;
};


////////////////////////////////////////////////////////////////////////////
// IPkgProject interface

enum {CREATE_PROJ_OK, CREATE_PROJ_CANCEL, CREATE_PROJ_RERUN_PROMPT };

enum DeletionCause {WorkspaceClose, ProjectDelete, ProjectUnload};

#define GPI_PROJISEXECUTABLE	0x0001
#define GPI_PROJSUPPORTSDEPS	0x0002
#define GPI_PROJSUPPORTSSCC	0x0004
#define GPI_PROJEXPORTMAKEFILE	0x0010
#define GPI_PROJEXPORTDEPS		0x0020

#define GPD_PROJSCCNAME		0x0001
#define GPD_PROJSCCPATH		0x0002
#define GPD_PROJSCCAUX		0x0004

#undef  INTERFACE
#define INTERFACE IPkgProject
DECLARE_INTERFACE_(IPkgProject, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// IPkgProject methods
	STDMETHOD(AddFiles)(LPCOLESTR * pszFileNames, ULONG count, 
				LPCOLESTR szDestinationFolder) PURE;
	STDMETHOD(CanAddFile)(LPCOLESTR szFileName, LPCOLESTR szDestinationFolder, BOOL bShowUI) PURE;
	STDMETHOD(DeleteFile)(LPCOLESTR szFileName, LPCOLESTR szDestinationFolder) PURE;
	STDMETHOD(RenameFile)(LPCOLESTR szOldFileName, LPCOLESTR szSourceFolder,
				LPCOLESTR szNewFileName, LPCOLESTR szDestinationFolder) PURE;
	STDMETHOD(CreateFolder)(LPCOLESTR szFolder) PURE;
	STDMETHOD(OnActivate)(BOOL bActivate) PURE;
	STDMETHOD(GetAutomationObject)(LPDISPATCH *ppAutomationProject) PURE;

	// Delete: this method should send a PN_QUERY_CLOSE_PROJECT and a PN_CLOSE_PROJECT
	// notification. The IPkgProject object should be sent as a hint with 
	// the PN_CLOSE_PROJECT notification so that the shell/workspace can remove 
	// this project from its list of projects.
	STDMETHOD(Delete)(DeletionCause dcCause, BOOL * pbSuccess) PURE;

	// SetDependentProject: sets pDependentProject to be dependent on 'this' (i.e., 'this'
	// is a subproject of pDependentProject).
	STDMETHOD(SetDependentProject)(IPkgProject *pDependentProject) PURE;

	// GetClassID: this method is used to differentiate between implementations of IPkgProject
	STDMETHOD(GetClassID)(CLSID * pClsID) PURE;

	// GetProjectInfo: this method is used to populate the ‘Select active project’ combo box. 
	// pszProjFullPath points to the project file name. It will be stored in 
	// the workspace file as a relative path. It is possible to pass in NULL 
	// in either argument (meaning that the caller is not interested in those).
	// dwFlags is used to return flags specific to the project--see GPI_* above.
	// Note: this method must check for and handle NULL pointers for all arguments
	STDMETHOD(GetProjectInfo)(LPCOLESTR *pszProjName, LPCOLESTR *pszProjFullPath,
                                  DWORD *pdwPackageID, DWORD *pdwFlags) PURE;
                                  
        STDMETHOD(GetProjectTypeName)(CString **ppszType) PURE;
                                  

	// DrawProjectGlyph: this method is called when another package wants the project glyph drawn
	// in the area given (e.g., this is used to get C++/Java/Web project glyphs drawn in DataView).
	STDMETHOD(DrawProjectGlyph)(HDC hDC, LPCRECT pRect) PURE;

	// Save: tells the project to serialize itself.
	STDMETHOD(Save)() PURE;
	// IsDirty: asks if the project needs serialization.
	STDMETHOD(IsDirty)() PURE;

	// GetDefaultFolder: Called by Insert Files into Project when the user changes the project 
	// in the projects drop-down combo box. We use the value returned by this 
	// function to fill in the Destination Folder.
	STDMETHOD(GetDefaultFolder)(LPCOLESTR *pszDefaultFolder) PURE;
	
	// CanBrowseFolders: Indicates whether the project provider can browse its folders. 
	// If not, the Browse button will be disabled in the Insert files 
	// into project dialog.
	STDMETHOD(CanBrowseFolders)(BOOL *pbCanBrowse) PURE;
	
	// BrowseFolders: This function is called from Insert Files into Project when 
	// the user clicks on the Browse button.
	STDMETHOD(BrowseFolders)(LPCOLESTR szStartingFolder ,
						LPCOLESTR *pszFolderChosen) PURE;
	
	// GetAllFolders: This function is reserved for future use: we want to be able 
	// to browse the folders without calling the browse folders function, so 
	// this function will return all available folders, and we will use the 
	// list to provide some UI.
	STDMETHOD(GetAllFolders)(CALPOLESTR *pAllFolders) PURE;

	STDMETHOD(GetAssociatedFiles)(CStringArray& outFiles, BOOL selected) PURE;
	STDMETHOD(SetDirty)(BOOL bDirty) PURE;

	// these get/set arbitrary string data, if supported by the project (GPD_*)
	STDMETHOD(GetProjectData)(DWORD dwProp, CString & strVal) PURE;
	STDMETHOD(SetProjectData)(DWORD dwProp, LPCSTR szVal) PURE;
};



/////////////////////////////////////////////////////////////////////////////
//	ISourceControlStatus interface

#undef  INTERFACE
#define INTERFACE ISourceControlStatus

DECLARE_INTERFACE_(ISourceControlStatus, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// ISourceControlStatus methods
	STDMETHOD(IsSccInstalled)(VOID) PURE;
	STDMETHOD(IsSccActive)(VOID) PURE;
	STDMETHOD(IsSccOpInProgress)(VOID) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	ISourceControl interface

#undef  INTERFACE
#define INTERFACE ISourceControl

#ifndef _SCC_STATUS_DEFINED
#define _SCC_STATUS_DEFINED
enum  SccStatus 
{
	SCC_STATUS_INVALID			= -1L,		// Status could not be obtained, don't rely on it
	SCC_STATUS_NOTCONTROLLED	= 0x0000L,	// File is not under source control
	SCC_STATUS_CONTROLLED		= 0x0001L,	// File is under source code control
	SCC_STATUS_CHECKEDOUT		= 0x0002L,	// Checked out to current user at local path
	SCC_STATUS_OUTOTHER			= 0x0004L,	// File is checked out to another user
	SCC_STATUS_OUTEXCLUSIVE		= 0x0008L,	// File is exclusively check out
	SCC_STATUS_OUTMULTIPLE		= 0x0010L,	// File is checked out to multiple people
	SCC_STATUS_OUTOFDATE		= 0x0020L,	// The file is not the most recent
	SCC_STATUS_DELETED			= 0x0040L,	// File has been deleted from the project
	SCC_STATUS_LOCKED			= 0x0080L,	// No more versions allowed
	SCC_STATUS_MERGED			= 0x0100L,	// File has been merged but not yet fixed/verified
	SCC_STATUS_SHARED			= 0x0200L,	// File is shared between projects
	SCC_STATUS_PINNED			= 0x0400L,	// File is shared to an explicit version
	SCC_STATUS_MODIFIED			= 0x0800L,	// File has been modified/broken/violated
	SCC_STATUS_OUTBYUSER		= 0x1000L	// File is checked out by current user someplace
};
#endif

enum SccOps
{
	SccNop = 0,
	SccSyncOp,
	SccOutOp,
	SccInOp,
	SccUnOutOp,
	SccAddOp,
	SccRemoveOp,
	SccDiffOp,
	SccHistoryOp,
	SccRenameOp,
	SccProps,
	SccOptions,
	SccShare,
	SccAdmin,
	SccStatus,
	SccInit,
	SccTerm,
	SccPopOp
};

DECLARE_INTERFACE_(ISourceControl, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// ISourceControl methods
	STDMETHOD(Init)(HPACKAGE hPackage) PURE;
	STDMETHOD(Term)(VOID) PURE;
	STDMETHOD(OpenProject)(IPkgProject* pkgProject, BOOL bCreate) PURE;
	STDMETHOD(CloseProject)(VOID) PURE;
	STDMETHOD(OnCloseProject)(VOID) PURE;
	STDMETHOD(OnIdle)(long lCount) PURE;
	STDMETHOD(Sync)(LPCSTR lpszFile, BOOL sleep=FALSE) PURE;
	STDMETHOD(Sync)(CStringArray * pFiles, BOOL sleep=FALSE) PURE; 	// Non-standard COM.
	STDMETHOD(CheckOut)(LPCSTR lpszFile) PURE;
	STDMETHOD(CheckOut)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(CheckIn)(LPCSTR lpszFile) PURE;
	STDMETHOD(CheckIn)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(CheckInAdd)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(UnCheckOut)(LPCSTR lpszFile) PURE;
	STDMETHOD(UnCheckOut)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(Add)(LPCSTR lpszFile) PURE;
	STDMETHOD(Add)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(Remove)(LPCSTR lpszFile) PURE;
	STDMETHOD(Remove)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(Diff)(LPCSTR lpszFile) PURE;
	STDMETHOD(Diff)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(History)(LPCSTR lpszFile) PURE;
	STDMETHOD(History)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(ShowProps)(LPCSTR lpszFile) PURE;
	STDMETHOD(ShowProps)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(Share)(CStringArray * pFiles) PURE;
	STDMETHOD(Admin)(LPCSTR lpszFile) PURE;
	STDMETHOD(Admin)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(IsInstalled)(VOID) PURE;
	STDMETHOD(IsActive)(VOID) PURE;
	STDMETHOD(GetStatusText)(const CStringArray & saFiles, CString & strStatus) PURE; 	// Non-standard COM.
	STDMETHOD(GetStatusText)(LPCSTR pszFileName, CString & strStatus) PURE; 	// Non-standard COM.
	STDMETHOD(GetStatusText)(int status, CString & strStatus) PURE; 	// Non-standard COM.
	STDMETHOD(InitOutputWindow)(VOID) PURE;
	STDMETHOD(CheckOutReadOnly)(LPCSTR lpszFile, BOOL bAlways, BOOL bReloadOk) PURE;
	STDMETHOD(CheckOutReadOnly)(CStringArray & strFiles, BOOL bAlways, BOOL bReloadOk) PURE; 	// Non-standard COM.
	STDMETHOD(GetStatusGlyphIndex)(int *pIndex, const CStringArray & saFiles) PURE; 	// Non-standard COM.
	STDMETHOD(GetStatusGlyphIndex)(int *pIndex, LPCSTR pszFileName) PURE;
	STDMETHOD(GetStatusGlyphIndex)(int *pIndex, int status, BOOL bShowControlled) PURE;
	STDMETHOD(GetStatusIndex)(int *pIndex, const CStringArray & saFiles, int & status) PURE; 	// Non-standard COM.
	STDMETHOD(GetStatusIndex)(int *pIndex, LPCSTR pszFileName, int & status) PURE;
	STDMETHOD(GetStatusIndex)(int *pIndex, int status) PURE;
	STDMETHOD(DrawGlyph)(CDC *pDC, LPCRECT lpRect, const CStringArray & saFiles, BOOL bShowControlled) PURE; 	// Non-standard COM.
	STDMETHOD(DrawGlyph)(CDC *pDC, LPCRECT lpRect, LPCSTR pszFileName, BOOL bShowControlled) PURE;
	STDMETHOD(DrawGlyph)(CDC *pDC, LPCRECT lpRect, int status, BOOL bShowControlled) PURE;
	STDMETHOD(PickOp)(int *piOp, LPCSTR lpszFile) PURE;
	STDMETHOD(PickOp)(int *piOp, const CStringArray & saFiles) PURE; 	// Non-standard COM.
	STDMETHOD(PerformOp)(LPCSTR lpszFile) PURE;
	STDMETHOD(PerformOp)(CStringArray * pFiles) PURE; 	// Non-standard COM.
	STDMETHOD(IsValidOp)(LPCSTR lpszFile, int op) PURE;
	STDMETHOD(IsValidOp)(const CStringArray & saFiles, int op) PURE; 	// Non-standard COM.
	STDMETHOD(GetOpText)(LPCSTR *ppsz, int op) PURE;
	STDMETHOD(CanAdd)(VOID) PURE;
	STDMETHOD(CanRemove)(VOID) PURE;
	STDMETHOD(CanSync)(VOID) PURE;
	STDMETHOD(CanDiff)(VOID) PURE;
	STDMETHOD(CanHistory)(VOID) PURE;
	STDMETHOD(CanHistMultifile)(VOID) PURE;
	STDMETHOD(CanProps)(VOID) PURE;
	STDMETHOD(CanShare)(VOID) PURE;
	STDMETHOD(CanAdmin)(VOID) PURE;
	STDMETHOD(AlwaysPrompt)(VOID) PURE;
	STDMETHOD(AddNewFiles)(VOID) PURE;
	STDMETHOD(AddNewProj)(VOID) PURE;
	STDMETHOD(CheckOutFiles)(VOID) PURE;
	STDMETHOD(RemoveDeletedFiles)(VOID) PURE;
	STDMETHOD(LoadFromOPT)(HSTATESAVER pStateSaver) PURE;
	STDMETHOD(SaveToOPT)(HSTATESAVER pStateSaver) PURE;
	STDMETHOD(GetSccName)(LPCSTR *ppsz) PURE;
	STDMETHOD(GetSccMenuName)(LPCSTR *ppsz) PURE;
	STDMETHOD(SetStatus)(LPCSTR pszFileName, int status, BOOL bNotify) PURE;
	STDMETHOD(GetStatus)(int *piStatus, LPCSTR pszFileName) PURE;
	STDMETHOD(GetStatus)(int *piStatus, const CStringArray & saFiles) PURE; 	// Non-standard COM.
	STDMETHOD(Enable)(BOOL bEnable) PURE;
	STDMETHOD(BrowseProjects)(int *piRet, BOOL bNew) PURE;
	STDMETHOD(BrowseProjects)(int *piRet, CString & strProjName, CString & strLocalPath,
		CString & strAuxPath, BOOL bNew) PURE; 	// Non-standard COM.
	STDMETHOD(CanBrowse)(VOID) PURE;
	STDMETHOD(GetProject)(CString & strProjName, CString & strLocalPath,
		CString & strProjPath, BOOL bCreate) PURE; 	// Non-standard COM.
	STDMETHOD(OnProjSave)(HPROJECT hProject) PURE;
	STDMETHOD(ProjectSpecified)(VOID) PURE;
	STDMETHOD(IsControlled)(LPCSTR pszFileName) PURE;
	STDMETHOD(RegisterProjectFilter)(LPCSTR lpszProjFilter) PURE;
	STDMETHOD(ShouldFileChange)(LPCSTR pszFile) PURE;
	STDMETHOD(IsAddingProject)(VOID) PURE;
	STDMETHOD(CancelGetProject)(const CString& builderfile) PURE;
	STDMETHOD(IsSccActive)(VOID) PURE;
	STDMETHOD(IsSccInstalled)(VOID) PURE;
	STDMETHOD(IsSccOpInProgress)(VOID) PURE;
	STDMETHOD(IsActive)(const CString&) PURE;
	STDMETHOD(IsActive)(IPkgProject*) PURE;
	STDMETHOD(AreAllActive)(const CStringArray&) PURE;
	STDMETHOD(IsBuilderControlled)(HPROJECT hBld) PURE;
	STDMETHOD_(int,IsOpInProgressValue)(THIS) PURE; // nonstandard result
};

/////////////////////////////////////////////////////////////////////////////
//	IPkgDropDataSource interface
//	Use this interface for OLE based project drag drop
#undef  INTERFACE
#define INTERFACE IPkgDropDataSource

DECLARE_INTERFACE_(IPkgDropDataSource, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (void) PURE;
	STDMETHOD_(ULONG,Release) (void) PURE;

	// Data Provider methods for Drop Sources
	STDMETHOD(GetDropInfo) (DWORD *pdwOKEffects, 
							IDataObject **ppDataObject, IDropSource **pDropSource) PURE;
	STDMETHOD(OnDropNotify) (BOOL fDropped, DWORD dwEffects) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IPkgDropDataTarget interface
//	Use this interface for OLE based project drag drop
#undef  INTERFACE
#define INTERFACE IPkgDropDataTarget

DECLARE_INTERFACE_(IPkgDropDataTarget, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (void) PURE;
	STDMETHOD_(ULONG,Release) (void) PURE;

	// Data Provider methods for Drop Target
	STDMETHOD(OnDragEnter)(IDataObject *pDataObj, 
							DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) PURE;
	STDMETHOD(OnDragOver)(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) PURE;
	STDMETHOD(OnDragLeave)(void) PURE;
	STDMETHOD(OnDrop)(IDataObject *pDataObj, 
							DWORD grfKeyState, POINTL pt,DWORD *pdwEffect) PURE;
};
/////////////////////////////////////////////////////////////////////////////

#endif	// __PRJAPI_H__
