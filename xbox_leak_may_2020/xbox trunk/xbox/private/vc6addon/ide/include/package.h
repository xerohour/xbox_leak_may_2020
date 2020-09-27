/////////////////////////////////////////////////////////////////////////////
//	PACKAGE.H
//		pack, package and packet objects

#ifndef __PACKAGE_H__
#define __PACKAGE_H__

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

class CKeymap;
class CFormatInfo;
class CSushiBarInfo;
class CPackage;
class CBMenuPopup;

/////////////////////////////////////////////////////////////////////////////
//	CPack class

class CPack : public CCmdTarget
{
	DECLARE_DYNAMIC (CPack)
public:
	CPack(CPackage* pPackage, UINT nIDPackage, LPCTSTR lpAccelID);
	~CPack();

// Attributes
public:
	HINSTANCE HInstance();
	UINT PackageID();
	UINT PacketID();

	CPackage* GetPackage() { return m_pPackage; }
	LPCTSTR GetAcceleratorID() const { return m_lpAccelID; }

protected:
	CPackage* m_pPackage;
	UINT m_nIDPacket;

// REVIEW: Protect this.
public:
	LPCTSTR m_lpAccelID;
};

/////////////////////////////////////////////////////////////////////////////
//	CPacket class

class CPacket : public CPack
{
public:
	CPacket(CPackage* pPackage, UINT id,
		LPCTSTR lpCmdTableID = NULL, LPCTSTR lpAccelID = NULL,
		CSushiBarInfo* pSushiBarInfo = NULL);

	virtual DWORD GetHelpID() { return 0; }

//	CKeymap* GetKeymap() { return m_pKeyMap; }
//	CKeymap* m_pKeymap;

	CSushiBarInfo* m_pSushiBarInfo;
};

/////////////////////////////////////////////////////////////////////////////
//	CPackage class

class CStateSaver;
class CGoToDialog;

// Package style flags
#define PKS_IDLE            0x00000001  // OnIdle calls
#define PKS_PRETRANSLATE    0x00000002  // PreTranslateMessage calls
#define PKS_COMMANDS		0x00000004	// OnCmdMsg calls
#define PKS_NOTIFY          0x00000008  // OnNotify calls
#define PKS_INIT			0x00000010  // OnInit call
#define	PKS_QUERYEXIT       0x00000020  // CanExit call
#define PKS_SERIALIZE		0x00000040  // SerializeWorkspaceSettings, SerializeWkspcConvSettings calls
#define PKS_INTERFACES		0x00000080  // QueryInterface calls
#define PKS_HELPFILE        0x00000100  // GetHelpFileName calls
#define PKS_FILES			0x00000200  // Overrides GetAssociatedFiles
#define PKS_FORMAT          0x00000400  // Overrides GetFormatInfo
#define PKS_AUTOMATION		0x00000800  // Overrides GetPackageExtension, GetApplicationExtensions
#define PKS_PROJECTINFO		0x00001000  // Overrides [G|S]etProjectWorkspaceInfo
#define PKS_GLOBALINFO		0x00002000  // Overrides [G|S]etGlobalWorkspaceInfo
#define PKS_PREEXIT			0x00004000	// Overrides OnPreExit()

// OPT Serialization flags
#define OPT_DEFAULT_WRKSPC  0x01
#define OPT_NAMED_WRKSPC    0x02
#define OPT_DOCUMENTS_ONLY  0x04
#define OPT_UNLOAD			0x08
#define OPT_WORKSPACE_ONLY	0x10

struct SRankedStringID
{
	UINT ids;
	UINT nRank;
	const CLSID *pclsid;
	BOOL bIsID;
};

#define MAX_LENGTH_APPLICATION_EXTENSION_NAME 64
struct SApplicationExtension
{
	char szName[MAX_LENGTH_APPLICATION_EXTENSION_NAME];
	CLSID clsid;
};

interface ICommandProvider;
class CProjComponentMgr;
class CPackage : public CPack
{
	DECLARE_DYNAMIC (CPackage)
public:
	CPackage(HINSTANCE hInstance, UINT id, UINT flags,
		LPCTSTR lpCmdTableID = NULL,
		LPCTSTR lpBitmapID = NULL,
		LPCTSTR lpAccelID = NULL,
		LPCTSTR lpLargeBitmapID = NULL);
	~CPackage();

	virtual BOOL OnInit();

	// Why are there three exit routines, you ask?  Here's why, and the order:
	//  1) CanExit():
	//	First, all packages are asked whether it's OK to exit.  Any package
	//   can veto, and exit will be canceled.  This occurs toward the beginning
	//   of CMainFrame::OnClose().
	//  2) OnPreExit()
	//  After it's been determined that no one will veto the exit, this
	//   is called (toward the end of CMainFrame::OnClose()).  Any package
	//   which holds on to OLE objects that call AfxOleLockApp() must
	//   release those objects here.  If not, CMainFrame::DestroyWindow()
	//   will never be called, and it will appear like DevStudio is hanging
	//   even though it's frame window has been hidden.
	//  3) OnExit()
	//  Since all App-locking objects were released in 2), MFC will then call
	//   CMainFrame::DestroyWindow().  It's in this function that OnExit()
	//   is called.  This is where most package destruction should occur.
	virtual BOOL CanExit();			// PKS_QUERYEXIT
	virtual void OnPreExit() {}		// PKS_PREEXIT
	virtual void OnExit();

	virtual BOOL OnIdle(long lCount);
	virtual BOOL OnNotify (UINT id, void *Hint = NULL, void *ReturnArea = NULL );
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
// Dockable window interface
	virtual HGLOBAL GetDockInfo();
	virtual HWND GetDockableWindow(UINT nID, HGLOBAL hglob);
	virtual HGLOBAL GetToolbarData(UINT nID);
	virtual HICON GetDockableIcon(UINT nID);
	virtual BOOL AskAvailable(UINT nID);
	// gets (and loads, if necessary) the appropriately sized bitmap
	virtual HBITMAP GetBitmap(BOOL bLarge);
	// unloads the bitmap, if loaded
	virtual void UnloadBitmap(void);

// Customization and Options interface
	virtual void AddOptDlgPages(class CTabbedDialog* pOptionsDialog, OPTDLG_TYPE type);

// Fonts/Colors interface
	virtual CFormatInfo* GetFormatInfo() { return NULL; }
	virtual void GetDefaultFormatInfo(CFormatInfo& fiDefault) {}
	virtual void OnFormatChanged() {}
	virtual void OnFontChange() {}
	virtual void OnColorChange() {}

// Help information (including legal resource, command, and window id ranges)
	enum RANGE_TYPE
	{
		MIN_RESOURCE, MAX_RESOURCE, 
		MIN_COMMAND, MAX_COMMAND, 
		MIN_WINDOW, MAX_WINDOW,
		MIN_STRING, MAX_STRING,
	};
	virtual DWORD GetIdRange(RANGE_TYPE rt);

	// Packages that supply commands known only at run-time, and
	//  which are transient (may come and go in the same session)
	//  must implement ICommandProvider and return it via this
	//  virtual.  See pkgs\include\commapi.h, commguid.h.
	// In keeping with good COM, the callee (package) addrefs the
	//  interface, and the caller (shell) releases it.
	virtual ICommandProvider* GetCommandProvider() { return NULL; }

	enum HELPFILE_TYPE { APPLICATION_HELP, REFERENCE_HELP, CONTROL_HELP };
	virtual LPCTSTR GetHelpFileName(HELPFILE_TYPE hft = APPLICATION_HELP);

// Build system
	// registration of build system components (used by platform add-ons)
	// blc_type	= component type, eg. platform
	// blc_id	= id of component 'hook', eg. a platform if blc_type is a tool
	virtual BOOL DoBldSysCompRegister(CProjComponentMgr *, DWORD blc_type, DWORD blc_id) {return FALSE;}

// Keymap support
	virtual void LoadGlobalAccelerators();

// CommDlg interface
	enum { GODI_FILEOPEN, GODI_RESIMPORT, GODI_ADDFILE, GODI_FINDINFILES, GODI_WORKSPACEOPEN, GODI_INSERTPROJECT };
	virtual void GetOpenDialogInfo(SRankedStringID** ppFilters, 
		SRankedStringID** ppEditors, int nOpenDialog);
	virtual CDocTemplate* GetTemplateFromEditor(UINT idsEditor, 
		const char* szFilename);

// GoTo support
	// Override this in packages that will add things to the Go To dialog. This
	// function is called whenever the Go To dialog is invoked. You should
	// dynamically create CGoToItem objects and add them to pDlg by calling its
	// AddItem() member function. Add a CGoToItem for each thing your package
	// will add to the Go To What listbox. The items will be deleted by the 
	// shell when the dialog is dismissed.
	virtual void AddGoToItems(CGoToDialog *pDlg) {};

// File saving errors
	// Override the following to provide an package specific file I/O
	// error string when using SetFileError.
	virtual int IdsFromFerr(int ferr);

// State serialization
	// Override SaveSettings to save information to the registry.
	virtual void SaveSettings();

// Project workspace interface
// REVIEW(PatBr):  This should be turned into a real innterface, and
//					moved into prjapi.h.
	virtual void GetDefProvidedNodes(CPtrList &DefNodeList, BOOL bWorkspaceInit);

	// Override the following function to save user-specific (non-sharable)
	// workspace specific data.
	virtual void SerializeWorkspaceSettings(CStateSaver& stateSave, DWORD dwFlags);
	virtual void SerializeWkspcConvSettings(CStateSaver& stateSave, DWORD dwFlags);

	// Override the following functions and define PKS_PROJECTINFO if your
	// package has project-specific information which should be saved to
	// and read from the workspace file.
	//
////// NOTE that the semantics of these changed recently./////////////
	// These calls used to occur after PN_OPEN_PROJECT.
	// Now, they occur before it. There is no valid open project when
	// the text is sent. A package can either act on the
	// text immediately, or store it away until PN_OPEN_PROJECT.
	// There's a new notification, PN_OPEN_PROJECT_FAILED, upon which
	// a package that had stored text and was waiting for PN_OPEN_PROJECT,
	// should free the text to avoid a memory leak because PN_OPEN_PROJECT
	// won't be coming. The "Hint" for failure is (const CString*)strProjName.
	// If it proves too difficult for packages to live with this change, there
	// are some other simple alternatives.
	virtual LPCTSTR GetProjectWorkspaceInfoText(LPCTSTR lpszProject);
	virtual void SetProjectWorkspaceInfoText(LPCTSTR lpszProject, LPCTSTR lpszInfo);

	// Override the following functions and define PKS_GLOBALINFO if your
	// package has global information which should be saved to
	// and read from the workspace file.
	virtual LPCTSTR GetGlobalWorkspaceInfoText();
	virtual void SetGlobalWorkspaceInfoText(LPCTSTR lpszInfo);

	// Override this function and define PKS_FILES if your package contributes
	// any files to the project workspace.  The main user of this is the bld
	// package, but other packages that contribute files (ms-test, etc.) should
	// also override this. This is primarily used by Source Control (CSccMgr).
	// Return the number of files added to the array.
	virtual int GetAssociatedFiles(CStringArray & saFiles, BOOL bSelected = FALSE) { return 0; }

	// To add new project types to the New Project Workspace dialog, override
	// this function and add new CProjTypeListItem objects to this list.  The
	// dialog box code will delete the objects for you.
	virtual void AddNewProjectTypes(CPtrList& projTypeListItems) { };

// OLE Automation
	// This is here so that the shell can query packages to get their additions
	//  to the DevStudio OLE Automation object model.  For more information,
	//  see extend.doc.  Must use PKS_AUTOMATION.
	virtual LPDISPATCH GetPackageExtension(LPCTSTR szExtensionName) { return NULL; }

	// A package overrides this to inform DevStudio on startup which OLE Automation
	//  package extensions it plans to provide.  These are objects that the package
	//  wishes to be available at the top level; the objects will appear to the macro
	//  writer as if they were properties of the Application object, since a macro
	//  can reference these objects directly by name like any other Application
	//  object.
	// The package fills in ppAppExts with an array of SApplicationExtension
	//  structs, each of which contains the name and the CLSID of the top-level
	//  object (or, "Named Item" as ActiveX Scripting calls it).  The package
	//  returns the number of elements in the array it's providing.  Like other
	//  similar package interfaces, the package allocates the space for this
	//  array, and destroys that space on shutdown.  So the array should be
	//  stored statically or globally.
	// DevStudio will later call CPackage::GetPackageExtension with these names
	//  when it needs the actual objects.
	// See the sample package for an example of how to override this virtual.
	// Must use PKS_AUTOMATION.
	virtual int GetApplicationExtensions(SApplicationExtension** ppAppExts) { return 0; }

// Property pages
	// Override the following function in order to append property pages
	// to someone else's selection.  Every time a property page is brought
	// up, all packages will be notified through this virtual.
	// guidView identifies the view that owns the object with the property page.
	// iUnk is an interface that a package can QI to get further interfaces
	// for determining the state of the active view.
	virtual BOOL AppendExtraPropertyPages(REFGUID guidView, IUnknown* piUnk);

	// OLE services support
	// This routine allows the package to participate in the 
	// IServiceProvider::QueryService routing scheme.  Any services that
	// are package specific should be exposed through this routine.  The 
	// current service routing scheme is as follows:
	// 1) CIPCompContainerItem, 2) IPCompDoc 3) CPackage 4) CTheApp 
	virtual HRESULT GetService(REFGUID guidService, REFIID riid, void **ppvObj)
		{ ASSERT(ppvObj); *ppvObj = NULL; return E_NOINTERFACE; }

// Utility functions
public:
	virtual LPCTSTR GetCommandString(UINT nID, UINT iString);
	virtual BSTR GetDynamicCmdString(UINT nID, UINT iString);
	virtual UINT CmdIDForToolTipKey(UINT nIDCmdOriginal);

	void SetVisibleMenuItem(UINT nID, BOOL bVisible = TRUE);
	void ShowCommandUI(UINT nID, BOOL bVisible = TRUE);
	void SetCommandUIFlags(UINT nID, UINT flags, BOOL bSet, UINT flagHidden);
	virtual BOOL IsVisibleProjectCommand(UINT nID, GUID *pGuids, UINT cGuidCount);

	// Override this if your package's command table includes any CT_MENU entries (used for
	// package specific submenus or toolbar menus)
	virtual POPDESC *GetMenuDescriptor(UINT nId) { return NULL; };
	// Override this to provide context-sensitive menu visibility. *Use this only when you wish
	// to emulate the behaviour of the Build/Debug menus, which disappear despite having active
	// items on them. Normally menus disappear automatically when all items are invisible (e.g.
	// Layout menu)
	virtual BOOL IsMenuVisible(UINT nId) { return TRUE; };
	// Override this to indicate that a category is textual rather than Graphical 
	virtual BOOL IsCategoryTextual(UINT nId) { return FALSE; };
	// Override this to indicate that a category should be always visible, even when empty
	virtual BOOL IsCategoryAlwaysVisible(UINT nId) { return FALSE; };

	// Override if your package has special unique properties
	virtual UINT GetPackageProperty(UINT nProperty);

protected:
	UINT m_id;
	HINSTANCE m_hInstance;

private:
	// even derived classes must use functions to access these
	LPCTSTR	m_lpBitmapID;				// id of the small bitmap
	LPCTSTR	m_lpLargeBitmapID;			// id of the large bitmap
	HBITMAP	m_hBitmap;					// HBITMAP of loaded bitmap (NULL if none loaded)
	BOOL m_bLoadedBitmapLarge;			// size of loaded bitmap
	
public:
	UINT m_flags;

	friend class CPack;
};

// Global func to determine if a specific package is loaded
BOOL IsPackageLoaded(UINT uiPackageId);

inline HINSTANCE CPack::HInstance() { return m_pPackage->m_hInstance; }
inline UINT CPack::PackageID() { return m_pPackage->m_id; }
inline UINT CPack::PacketID() { return m_nIDPacket; }

/////////////////////////////////////////////////////////////////////////////
//	CDefProvidedNode
//		this node type is added to the list of default node provider package
//		nodes which the project window asks for if there is no user-preference
//		information	for the project which is being opened/created.
//	REVIEW(PatBr):  This should be turned into an interface, and moved
//					into prjapi.h along with the package workspace interface.

typedef BOOL (CALLBACK *FILESAVECALLBACK)();

class CDefProvidedNode : public CObject
{
public:
	CDefProvidedNode();
	virtual ~CDefProvidedNode();

	enum SORT_TYPE { content_sort, alpha_sort };

	// packages must subclass the CDefProvidedNode class
	// in order to have meaningful node types. they should provide:

	// whether the default node should reside on its
	// own pane in the project window, and...
	virtual BOOL HasOwnPane() { return(TRUE); }

	// ...if the node has its own pane, whether the pane is 
	// workspace-independent (e.g., the HelpView pane is), and...
	virtual BOOL IsPaneIndependent() { return(FALSE); }

	// .. the type of sorting desired on the top-level nodes in the pane, and...
	virtual SORT_TYPE GetSortType() { return(alpha_sort); }

	// .. the help identifier used when user asks for help with this pane active...
	virtual UINT GetHelpID() { return((UINT)-1); }

	// ...a string to use as the name for the pane, and...
	virtual const TCHAR *GetPaneName() = 0;

	// ...a function which can be called to create the node,
	// which returns a pointer to a slob which will then be
	// inserted into the pane (this can be a CMultiSlob if the
	// node provider wants to insert more than one root node), and...
	virtual CSlob *CreateNode() = 0;

	// ...a function which returns the priority index of the pane.
	// currently: Classes=100, Resources=300, Build=500, Help=700, and...
	virtual int GetPriorityIndex() = 0;

	// ...a function which returns (if appropriate) a file-save callback
	// which the project window can use for command routing/enabling, and...
	virtual FILESAVECALLBACK GetFileSaveCallback() { return(NULL); }

	// ... a function to initialize the glyph bitmap for the pane.  the
	// bitmap should contain three bitmaps (12x12, 9x9, 4x4) in a 36x12
	// bitmap which can be used to initialize a CImageWell.  The three
	// different sizes are for varying sizes of scrollbars (16x16 on
	// WindowsNT but user-defined (default:13x13) on Windows'95).
	// caller of this function will perform DeleteObject() on HBITMAP.
	virtual HBITMAP GetPaneGlyph() = 0;

	//ugly hack.  The node is the only workspace-related thing that the 
	//res package seems to subclass.  So when we're initing the workspace 
	//window, the only way to know if we're dealing with ResourceView 
	//(which we don't want to expand for performance reasons), is to
	//ask the node.  So the res pkg overrides this to return FALSE.
	//All other packages should not need to override this.
	virtual	BOOL ShouldExpand() { return TRUE; }

};

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA NEAR

#endif // __PACKAGE_H__
