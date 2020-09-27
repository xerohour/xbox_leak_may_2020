#if !defined(JAZZAPP_H__B6AED007_3BDF_11D0_89AC_00A0C9054129__INCLUDED_)
#define JAZZAPP_H__B6AED007_3BDF_11D0_89AC_00A0C9054129__INCLUDED_

// JazzApp.h : main header file for the DirectMusic Producer application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


#include "DMUSProd.h"		// DirectMusic Producer interfaces
#include "DMPPrivate.h"
#include "dmusici.h"		// DirectMusic interfaces
#include "dmusicf.h"		// DirectMusic file formats
#include "RiffStrm.h"
#include "Framework.h"
#include "JazzDocTemplate.h"
#include "AppBasePropPageManager.h"
#include "DirectoryNode.h"
#include "Bookmark.h"
#include "FileNode.h"
#include "Project.h"
#include "CommonDoc.h"
#include "JazzDoc.h"
#include "ComponentDoc.h"
#include "ComponentView.h"
#include "AppJazzDataObject.h"
#include "Loader.h"


/////////////////////////////////////////////////////////////////////////////
// Application defines
//

#define SMALL_BUFFER 30
#define MID_BUFFER  100
#define MAX_BUFFER  256

#define IDM_LAST_MDICHILD	AFX_IDM_FIRST_MDICHILD + 7

#define DOC_ACTION_NONE			0
#define DOC_ACTION_REVERT		1

#define DOC_NOT_RUNTIME_SAVE	0	// Anything above zero is considered a Runtime save
#define DOC_RUNTIME_SAVE		1
#define DOC_RUNTIME_SAVE_ALL	2

#define TGT_PROJECT				1
#define TGT_FILENODE			2
#define TGT_FILENODE_SAVEAS		3
#define TGT_SELECTEDNODE		4

#define	DPF_NO_FILES			0
#define	DPF_DESIGN_FILES		1
#define DPF_ALL_FILES			2

#define GNF_NOFLAGS				0x0000
#define GNF_DIRTY				0x0001		// Dirty documents

#define FD_PROJECT				0x0001
#define FD_COMPONENT			0x0010
#define FD_ANY					0x0011

#define MAX_FILE_OPEN_PRIORITY	10

#define NBR_FRAMEWORK_PANES		1
#define MAX_PANES				15
#define FIRST_ADDINS_MENU_ID	5000
#define FIRST_TOOLBAR_ID		0xE840
#define MAX_LENGTH_BOOKMARK_NAME 32
#define MAX_LENGTH_PROJECT_NAME 64
#define MAX_LENGTH_FILE_NAME	64
#define MAX_LENGTH_DIR_NAME		64
#define MAX_LENGTH_PATH_NAME	250
#define FIRST_TOOLBAR_ITEM		4

#define FIRST_PROJECT_IMAGE			0
#define FIRST_DIRECTORY_IMAGE		2
#define FIRST_FILE_IMAGE			4
#define FIRST_PRODUCER_FILE_IMAGE	6

#define REF_PER_MIL     10000       // For converting from reference time to mils 

#define MAKETAG( ch0, ch1, ch2, ch3 )                                \
                ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

// Internal clipboard formats
#define CF_DMUSPROD_FILELIST "DMUSProd File List v.1"

// determine number of elements in an array (not bytes)
#define _countof(array) (sizeof(array)/sizeof(array[0]))


#pragma pack(2)

typedef struct ioFileRef
{
    GUID	guidFile;
    GUID    guidDocRootNodeId;
} ioFileRef;

#pragma pack()


/////////////////////////////////////////////////////////////////////////////
// Application prototypes
//

HRESULT AllocFileStream( LPCTSTR szFileName, DWORD dwDesiredAccess, FileType ftFileType,
						 GUID guidDataFormat, IDMUSProdNode* pITargetDirectoryNode, IStream **ppIStream );
HRESULT AllocMemoryStream( FileType ftFileType, GUID guidDataFormat, IStream **ppIStream );
HRESULT AllocFileReadWriteStream( LPCTSTR szFileName, IStream **ppIStream );
int CALLBACK BrowseFolder( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData );
int CALLBACK CompareTreeItems( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

BOOL AFXAPI AfxFullPath(LPTSTR szPathOut, LPCTSTR szFileIn);
BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);
BOOL AFXAPI AfxResolveShortcut(CWnd* pWnd, LPCTSTR szShortcutFile,
	LPTSTR szPath, int cchPath);


/////////////////////////////////////////////////////////////////////////////
// Application externs
//

extern const GUID GUID_ProjectFolderNode;
extern const GUID GUID_DirectoryNode;
extern const GUID GUID_FileNode;

#define SHOW_NODE_IN_TREE	0
 

/////////////////////////////////////////////////////////////////////////////
// CJzFileGUIDs

class CJzFileGUIDs
{
// Constructor
public:
	CJzFileGUIDs( LPCTSTR pszRelativePathName, GUID guidExistingFile );
	virtual ~CJzFileGUIDs();

// Attributes
public:
	CString	m_strRelativePathName;
	GUID	m_guidExistingFile;
	GUID	m_guidNewFile;
};
 

/////////////////////////////////////////////////////////////////////////////
// CJzFileName

class CJzFileName
{
// Constructor
public:
	CJzFileName( LPCTSTR pszFileName );
	virtual ~CJzFileName();

private:
	short GetPriority();
	void GetListInfo();

// Attributes
public:
	CString		m_strFileName;
	CString		m_strObjectName;
	GUID		m_guidObject;
	short		m_nPriority;
	bool		m_fBeingLoaded;
};


/////////////////////////////////////////////////////////////////////////////
// CJzTrackFileOpen

class CJzTrackFileOpen : public IUnknown
{
// Constructor
public:
	CJzTrackFileOpen();
	virtual ~CJzTrackFileOpen();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

// Attributes
public:
	DWORD		m_dwRef;
};


/////////////////////////////////////////////////////////////////////////////
// CJazzApp:
// See Jazz.cpp for the implementation of this class
//

class CJazzApp : public CWinApp
{
friend class CBookmarkToolBar;
friend class CProject;

public:
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	virtual BOOL IsIdleMessage(MSG* pMsg);  // checks for special messages
	CJazzApp();
	void LoadAppState( int nCmdShow );
	void SaveAppState();
	BOOL CreateDocTemplate( IDMUSProdDocType* pIDocType, HINSTANCE hInstance, UINT nResourceId );
	IDMUSProdPropSheet* GetPropertySheet(void);
	CTreeCtrl* GetProjectTreeCtrl(void);
	void DrawProjectTree(void);
	void SetProjectTreePosition( IDMUSProdNode* pINode );
	int GetNbrDocuments(void);
	CCommonDoc* FindDocument( IDMUSProdNode* pINode );
	CComponentDoc* FindComponentDocument( IDMUSProdNode* pINode );
	BOOL IsValidProjectFile( LPCTSTR szPathName, BOOL fDisplayErrMsg ); 
	BOOL DoPromptFileSave( CJazzDocTemplate* pTheTemplate, CString& fileName );
	BOOL DoPromptFileOpenSingle( CString& fileName, CDocTemplate* pTheTemplate, LPCTSTR szTitle );
	BOOL DoPromptFileOpenMulti( CString& fileName, CFileDialog* pFileDlg, CDocTemplate* pTheTemplate, LPCTSTR szTitle );
	BOOL DoPromptProjectOpen( CString& fileName );
	IDMUSProdDocType* GetDocType( LPCTSTR szFileName );
	CJazzDocTemplate* FindDocTemplateByFilter( LPCTSTR szFilterExt );
	CJazzDocTemplate* FindDocTemplate( IDMUSProdDocType* pIDocType );
	CJazzDocTemplate* FindProjectDocTemplate(void);
	CFileNode* FindFileNode( LPCTSTR szPathName );
	CMenu* FindMenuByName( UINT nResourceId );
	CProject* CreateNewProject(void);
	void FindProjectFileName( LPCTSTR szFileName, CString& strProjectFileName ); 
	CFileNode* InsertForeignFile( LPCTSTR szFileName );
	CCommonDoc* OpenTheFile( LPCTSTR szFileName, short nFileTarget );
	BOOL CopyTheFile( LPCTSTR szOldFile, LPCTSTR szNewFile, BOOL fFlags );
	BOOL RenameThePath( LPCTSTR szOldName, LPCTSTR szNewName, LPCTSTR szMRUName );
	HRESULT DeleteFileToRecycleBin( LPCTSTR szFileName );
	void DeleteEmptyDirTreeToRecycleBin( LPCTSTR szDirectory );
	void DeleteEmptyDirToRecycleBin( LPCTSTR szDirectory );
	BOOL IsDirectoryEmpty( LPCTSTR szDirectory );
	BOOL CreateTheDirectory( LPCTSTR szDirectory );
	BOOL MakeTheDirectory( CString& strDir, LPCTSTR szDirectory, UINT nResourceID );
	void DisplaySystemError( UINT nResourceID, LPCTSTR szContext );
	void SetActiveProject( CProject* pProject );
	CProject* GetActiveProject();
	void AddProject( CProject* pProject );
	void RemoveProject( CProject* pProject );
	BOOL DeleteProject( CProject* pProject, short nWhichFiles, BOOL fRuntimePrompt );
	BOOL SaveAllProjects();
	void CloseAllProjects();
    HRESULT GetFirstProject( IDMUSProdProject** ppIFirstProject );
    HRESULT GetNextProject( IDMUSProdProject* pIProject, IDMUSProdProject** ppINextProject );
	CProject* GetProjectByGUID( GUID guidProject );
	CProject* GetProjectByFileName( LPCTSTR szFileName );
	CProject* GetProjectByProjectDir( LPCTSTR szProjectDir );
	CFileNode* GetFileByGUID( GUID guidFile );
	BOOL IsFileOpenInDiffProject( GUID guidFile, CProject* pThisProject );
	CFileNode* GetFileByDocRootNode( IDMUSProdNode* pIDocRootNode );
	void CleanUpNotifyLists( CFileNode* pFileNode, BOOL fFileClosed );
	void RemoveFromRecentFileList( LPCTSTR szPathName );
	void RemoveLastSlash( CString& strName );
	void GetDefaultProjectDir( CString& strProjectDir );
	void SetProjectDirForFileOpen( LPCTSTR szFileName );
	void GetProjectDirForFileOpen( CString& strProjectDir );
	void SetDefaultDirForFileOpen( CDocTemplate * pTemplate, LPCTSTR szFileName );
	void GetDefaultDirForFileOpen( CString& strExt, CString& strDir );
	void SetDefaultDirForFileSave( LPCTSTR szFileName, LPCTSTR szExt );
	void GetDefaultDirForFileSave( LPCTSTR szFileName, CString& strDir );
	void AdjustFileName( FileType ftFileType, LPCTSTR szOrigFileName, CString& strFileName );
	void SplitOutFileName( LPCTSTR szCompleteFileName, BOOL fIncludeExt, CString& strName ); 
	void GetUniqueFileName( LPCTSTR szCompleteFileName, CString& strUniqueFName );
	void CleanUpBookmarks();
	BOOL GetNewGUIDForDuplicateFile( GUID guidExistingFile, GUID* pguidNewFile );
	BOOL GetNewGUIDForDuplicateFile( LPCTSTR pszRelativePathName, GUID* pguidNewFile );
	BOOL GetHelpFileName( CString& strHelpFileName );
	void DuplicateBookmarks( CProject* pFromProject, CProject* pToProject );
	
protected:
    BOOL FirstInstance();
    void AddNodeImageList( void );
	void RegisterTheFileExtensions();
	void UnregisterTheFileExtensions();
	BOOL IsFileNameUnique( LPCTSTR szFileName );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJazzApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL SaveAllModified();
	virtual CDocument* OpenDocumentFile(LPCTSTR szFileName);
	virtual void AddToRecentFileList(LPCTSTR szFileName);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
	private:
    CTypedPtrList<CPtrList, CProject*> m_lstProjects;
    CTypedPtrList<CPtrList, CBookmark*> m_lstBookmarks;
    CTypedPtrList<CPtrList, CJzFileGUIDs*> m_lstFileGUIDs;
	CProject*				m_pActiveProject;
	HACCEL					m_hAcceleratorTable;
	
	public:
    CTypedPtrList<CPtrList, CJzFileName*> m_lstFileNamesToOpen;

	COleTemplateServer		m_server;
		// Server object for document creation
														// include edit information
	CString					m_strAppDir;				// DMUSProd.exe directory
	CString					m_strImportFileName;		// Only used when importing files
	CString					m_strNewFileName;			// Only used when creating new files

	CFramework*				m_pFramework;
	IServiceProvider*		m_pIServiceProvider;
	IDMUSProdPropPageManager*	m_pIPageManager;		// Only for DMUSProd.exe objects
	CJzTrackFileOpen*		m_pJzTrackFileOpen;
	CLoader*				m_pLoader;					// Implementation of IDirectMusicLoader
	short					m_nFirstImage;				// Image for Project folder in tree
	short					m_nRuntimeSave;				// Saving compressed version of files
	short					m_nFileTarget;				// Helps determine destination when opening file
	short					m_nShowNodeInTree;			// Show and select file node			
	HANDLE					m_hKeyProgressBar;			// Used to store progress bar hKey
	BOOL					m_fShutDown;				// Application in process of shutting down
	BOOL					m_fCloseProject;			// Application in process of closing Project
	BOOL					m_fInPasteFromData;			// So.... show all nodes inserted in Tree			
	BOOL					m_fInDocRootDelete;			// In process of deleting DocRoot node			
	BOOL					m_fInDuplicateProject;		// In process of duplicating a Project			
	BOOL					m_fUserChangedNodeName;			
	BOOL					m_fInJazzDocSaveModified;	// In CJazzDoc::SaveModified()
	BOOL					m_fDeleteFromTree;			// User deleting from Project Tree
	BOOL					m_fShellCommandOK;			// Ignore FALSE return from ProcessShellCommand
	BOOL					m_fSendFileNameChangeNotification;
	BOOL					m_fOpenEditorWindow;
	int						m_nSavePromptAction;		// Used for 'Yes All' and 'No All' reponses

	afx_msg void OnUpdateBookmarkCombo(CCmdUI* pCmdUI);

	//{{AFX_MSG(CJazzApp)
	afx_msg void OnAppAbout();
	afx_msg void OnWindowCloseAll();
	afx_msg void OnFileOpenProject();
	afx_msg void OnUpdateFileSaveProject(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveProject();
	afx_msg void OnUpdateFileCloseProject(CCmdUI* pCmdUI);
	afx_msg void OnFileCloseProject();
	afx_msg void OnFileRuntimeSaveAllFiles();
	afx_msg void OnUpdateFileRuntimeSaveAllFiles(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnBookmarkCreate();
	afx_msg void OnUpdateBookmarkCreate(CCmdUI* pCmdUI);
	afx_msg void OnBookmarkRemove();
	afx_msg void OnUpdateBookmarkRemove(CCmdUI* pCmdUI);
	afx_msg void OnBookmarkRemoveAll();
	afx_msg void OnUpdateBookmarkRemoveAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnImportMid();
	afx_msg void OnUpdateImportMid(CCmdUI* pCmdUI);
	afx_msg void OnImportSec();
	afx_msg void OnUpdateImportSec(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileDuplicateProject(CCmdUI* pCmdUI);
	afx_msg void OnFileDuplicateProject();
	afx_msg void OnUpdateImportWav(CCmdUI* pCmdUI);
	afx_msg void OnImportWav();
	afx_msg void OnUpdateImportWavVariations(CCmdUI* pCmdUI);
	afx_msg void OnImportWavVariations();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


extern CJazzApp theApp;
extern LPCTSTR lpszUniqueClass;


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(JAZZAPP_H__B6AED007_3BDF_11D0_89AC_00A0C9054129__INCLUDED)
