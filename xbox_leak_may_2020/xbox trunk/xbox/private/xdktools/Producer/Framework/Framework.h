#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

// Framework.h : header file
//

/*-----------
@doc DMUSPROD
-----------*/

#include <afxtempl.h>


/////////////////////////////////////////////////////////////////////////////
// DirectMusic Producer WindowPlacement Structures 
//

#pragma pack(2)

typedef struct ioWindowPlacement		// Editor window placement information (File IO)
{
	GUID guidFile;
	GUID guidNodeId;
	long lTreePos;

	WINDOWPLACEMENT wp;
} ioWindowPlacement;


typedef struct wpWindowPlacement		// Editor window placement information
{
	CString	strNodeName;
	GUID	guidFile;
	GUID	guidNodeId;
	long	lTreePos;

	short	nInternalUse;				// CBookmark::Create uses to establish Z-Order 

	WINDOWPLACEMENT wp;
} wpWindowPlacement;


typedef struct bkComponentState			// Component state information
{
	CLSID	clsidComponent;
	void*   pComponentStateInfo;		// State info for Component
	DWORD	dwComponentStateInfoSize;	// Size of state info
} bkComponentState;


typedef struct CFProducerFile			// Used when working with CF_DMUSPROD_FILE clipboard data
{
    GUID	guidFile;
	CString strFileName;
} CFProducerFile;

#pragma pack()


/*======================================================================================
STRUCT:  DMUSPRODFILEREFINFO
========================================================================================
@struct DMUSProdFileRefInfo | Contains information used to describe a file reference.
 
@field WORD | wSize | Size of this structure.  Must be filled prior to calling a
		method that populates the remaining DMUSProdFileRefInfo fields. 
@field GUID | guidFile | Producer generated GUID associated with the referenced file.
@field GUID | guidDocRootNodeId | NodeId describing the object associated with the
		referenced file.

--------------------------------------------------------------------------------------*/


class CFileNode;
class CClientToolBar;


/////////////////////////////////////////////////////////////////////////////
// CJzComponent

class CJzComponent
{
// Constructor
public:
	CJzComponent( CLSID clsidComponent, IDMUSProdComponent* pIComponent );
	virtual ~CJzComponent();

// Attributes
public:
	CLSID				m_clsidComponent;
	IDMUSProdComponent* m_pIComponent;
	BOOL				m_fInitialized;
};


/////////////////////////////////////////////////////////////////////////////
// CJzMenu

class CJzMenu
{
// Constructor
public:
	CJzMenu( IDMUSProdMenu* pIMenu, UINT nCommandID );
	virtual ~CJzMenu();

// Attributes
public:
	IDMUSProdMenu* m_pIMenu;
	UINT 	   m_nCommandID;
};


/////////////////////////////////////////////////////////////////////////////
// CJzToolBar

class CJzToolBar
{
// Constructor
public:
	CJzToolBar();
	virtual ~CJzToolBar();

// Attributes
public:
	CClientToolBar*	m_pClientToolBar;
	UINT 			m_nControlID;
};


/////////////////////////////////////////////////////////////////////////////
// CJzNode

class CJzNode
{
// Constructor
public:
	CJzNode( IDMUSProdNode* pINode );
	virtual ~CJzNode();

// Attributes
public:
	IDMUSProdNode*		m_pINode;
	WINDOWPLACEMENT m_wp;
};


/////////////////////////////////////////////////////////////////////////////
// CJzNotifyNode

class CJzNotifyNode
{
// Constructor
public:
	CJzNotifyNode();
	virtual ~CJzNotifyNode();

// Attributes
public:
	IDMUSProdNode*	m_pINotifyThisNode;
	GUID			m_guidFile;
	int				m_nUseCount;
};


/////////////////////////////////////////////////////////////////////////////
// CJzClipFormat

class CJzClipFormat
{
// Constructor
public:
	CJzClipFormat( UINT uClipFormat, LPCTSTR szExt );
	virtual ~CJzClipFormat();

// Attributes
public:
	UINT		m_uClipFormat;
	CString		m_strExt;
};


/////////////////////////////////////////////////////////////////////////////
// CJzSharedObject

class CJzSharedObject
{
// Constructor
public:
	CJzSharedObject( REFCLSID clsid, IUnknown* pIUnknown );
	virtual ~CJzSharedObject();

// Attributes
public:
	CLSID 	   m_clsid;
	IUnknown*  m_pIUnknown;
};


/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER FRAMEWORK OBJECT
========================================================================================
@object Framework | Represents DMUSProd.exe.

@supint IDMUSProdFramework | Provides the means for a DirectMusic Producer <o Component>
		to plug into the DirectMusic Producer application.  Implemented by DirectMusic
		Producer.
@supint IDMUSProdFileRefChunk | Methods to save and load Producer specific file
		reference RIFF chunks.  Implemented by DirectMusic Producer.
@supint IDMUSProdLoaderRefChunk | Embeds file reference RIFF chunks into a stream for the
		purpose of enabling an implementation of IDirectMusicLoader to resolve file references.
		Implemented by DirectMusic Producer.

@comm
	DMUSProd.exe manages the one and only Framework object.  This object is created
	during application launch and exists throughout the entire DirectMusic Producer
	work session.

	The Framework object is responsible for managing DirectMusic Producer projects.
	It owns the Project Tree and implements <i IDMUSProdFramework> so that DirectMusic
	Producer Components can plug into its functionality.  It also implements
	<i IDMUSProdFileRefChunk> and <i IDMUSProdLoaderRefChunk> to help Components
	manage file references.
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework 

class CFramework : public IDMUSProdFramework8, public IDMUSProdFileRefChunk, public IDMUSProdLoaderRefChunk
{
public:
	CFramework(); 
	virtual ~CFramework();

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

/* --------------------------------------------------------------------------
@interface IDMUSProdFramework | 
	This interface allows a DirectMusic Producer <o Component> object to function within
	DMUSProd.exe.

@comm
	<i IDMUSProdFramework> provides the means for Components to hook into the <o Framework>
	through the Framework's Project Tree and 'Add-Ins' menu.  In addition, it allows a Component
	to register additional file types that can be created, opened and saved through the
	Framework's File New, File Open and File Save commands.  It provides the doorway for
	communication amongst Components. It also provides access to the Framework's status bar
	and the ability to create application-wide toolbars.

@base public | IUnknown

@xref  <o Framework> Object, <i IDMUSProdComponent>, <i IDMUSProdDocType>, <i IDMUSProdNode>, <i IDMUSProdEditor>, <i IDMUSProdRIFFExt>, <i IDMUSProdMenu>, <i IDMUSProdToolBar>
-------------------------------------------------------------------------- */

	// IDMUSProdFramework methods
// @meth HRESULT | FindComponent| Returns a pointer to the specified Component's IDMUSProdComponent
// interface.
        HRESULT STDMETHODCALLTYPE FindComponent( REFCLSID rclsid, IDMUSProdComponent** ppIComponent );
// @meth HRESULT | GetFirstComponent| Returns an IDMUSProdComponent interface pointer for the first
// Component in the Framework's list of registered Components.
        HRESULT STDMETHODCALLTYPE GetFirstComponent( IDMUSProdComponent** ppIFirstComponent );
// @meth HRESULT | GetNextComponent| Returns an IDMUSProdComponent interface pointer for the next
// Component in the Framework's list of registered Components.
        HRESULT STDMETHODCALLTYPE GetNextComponent( IDMUSProdComponent* pIComponent, IDMUSProdComponent** ppINextComponent );

// @meth HRESULT | AddDocType| Registers a document type with the Framework.
		HRESULT STDMETHODCALLTYPE AddDocType( IDMUSProdDocType* pIDocType );
// @meth HRESULT | FindDocTypeByExtension| Returns a pointer to the specified file extension's
// IDMUSProdDocType interface.
		HRESULT STDMETHODCALLTYPE FindDocTypeByExtension( BSTR bstrExt, IDMUSProdDocType** ppIDocType );
// @meth HRESULT | FindDocTypeByNodeId| Returns a pointer to the specified id's IDMUSProdDocType
// interface.
        HRESULT STDMETHODCALLTYPE FindDocTypeByNodeId( REFGUID rguid, IDMUSProdDocType** ppIDocType );
// @meth HRESULT | GetFirstDocType| Returns an IDMUSProdDocType interface pointer for the first
// DocType in the Framework's list of registered DocTypes.
        HRESULT STDMETHODCALLTYPE GetFirstDocType( IDMUSProdDocType** ppIFirstDocType );
// @meth HRESULT | GetNextDocType| Returns an IDMUSProdDocType interface pointer for the next
// DocType in the Framework's list of registered DocTypes.
        HRESULT STDMETHODCALLTYPE GetNextDocType( IDMUSProdDocType* pIDocType, IDMUSProdDocType** ppINextDocType );
// @meth HRESULT | CreateNewFile| Invokes the Framework's File New command and returns a new file of
// the type described through <p pIDocType>.
		HRESULT STDMETHODCALLTYPE CreateNewFile( IDMUSProdDocType* pIDocType, IDMUSProdNode* pITreePositionNode, IDMUSProdNode** ppIDocRootNode );
// @meth HRESULT | OpenFile| Invokes the Framework's File Open command and returns a file of
// the type described through <p pIDocType>.
	HRESULT STDMETHODCALLTYPE OpenFile( IDMUSProdDocType* pIDocType, BSTR bstrTitle, IDMUSProdNode* pITreePositionNode, IDMUSProdNode** ppIDocRootNode );
// @meth HRESULT | CopyFile| Returns a copy of the file specified in <p pIDocRootNode>.
        HRESULT STDMETHODCALLTYPE CopyFile( IDMUSProdNode* pIDocRootNode, IDMUSProdNode* pITreePositionNode, IDMUSProdNode** ppINewDocRootNode );
// @meth HRESULT | ShowFile| Displays a newly created file in the Project Tree.
        HRESULT STDMETHODCALLTYPE ShowFile( BSTR bstrFileName );

// @meth HRESULT | RegisterClipFormatForFile| Registers a file's clipboard format with the Framework.
		HRESULT STDMETHODCALLTYPE RegisterClipFormatForFile( UINT uClipFormat, BSTR bstrExt );
// @meth HRESULT | SaveClipFormat| Returns a stream containing the specifed clipboard format for <p pINode>.
		HRESULT STDMETHODCALLTYPE SaveClipFormat( UINT uClipFormat, IDMUSProdNode* pINode, IStream** ppIStream );
// @meth HRESULT | GetDocRootNodeFromData| Returns an IDMUSProdNode interface pointer for the DocRoot node copied into
// <p pIDataObject>.
		HRESULT STDMETHODCALLTYPE GetDocRootNodeFromData( IDataObject* pIDataObject, IDMUSProdNode** ppIDocRootNode );

// @meth HRESULT | AllocFileStream| Returns an IStream interface pointer for the file specified in <p bstrFileName>.
		HRESULT STDMETHODCALLTYPE AllocFileStream( BSTR bstrFileName, DWORD dwDesiredAccess, FileType ftFileType, GUID guidDataFormat, IDMUSProdNode* pITargetFolderNode, IStream **ppIStream );
// @meth HRESULT | AllocMemoryStream| Returns an IStream interface pointer for a newly created memory stream.
		HRESULT STDMETHODCALLTYPE AllocMemoryStream( FileType ftFileType, GUID guidDataFormat, IStream **ppIStream );

// @meth HRESULT | GetFirstProject| Returns an IDMUSProdProject interface pointer for the first
// Project in the application's list of Projects.
        HRESULT STDMETHODCALLTYPE GetFirstProject( IDMUSProdProject** ppIFirstProject );
// @meth HRESULT | GetNextProject| Returns an IDMUSProdProject interface pointer for the next
// Project in the application's list of Projects.
        HRESULT STDMETHODCALLTYPE GetNextProject( IDMUSProdProject* pIProject, IDMUSProdProject** ppINextProject );
// @meth HRESULT | FindProject| Returns an IDMUSProdProject interface pointer for the Project
// containing <p pINode>.
        HRESULT STDMETHODCALLTYPE FindProject( IDMUSProdNode* pINode, IDMUSProdProject** ppIProject );
// @meth HRESULT | IsProjectEqual| Determines whether <p pIDocRootNode1> and <p pIDocRootNode2> are
// in the same Project.
        HRESULT STDMETHODCALLTYPE IsProjectEqual( IDMUSProdNode* pIDocRootNode1, IDMUSProdNode* pIDocRootNode2 );

// @meth HRESULT | AddNodeImageList| Adds images to the Project Tree's image list.
        HRESULT STDMETHODCALLTYPE AddNodeImageList( HANDLE hImageList, short* pnNbrFirstImage );
// @meth HRESULT | GetNodeFileName| Returns the path/filename of the file associated with <p pINode>.
		HRESULT STDMETHODCALLTYPE GetNodeFileName( IDMUSProdNode* pINode, BSTR* pbstrFileName );
// @meth HRESULT | GetNodeFileGUID| Returns the Producer generated GUID assigned to the file associated with <p pINode>.
		HRESULT STDMETHODCALLTYPE GetNodeFileGUID( IDMUSProdNode* pINode, GUID* pguidFile );
// @meth HRESULT | RevertFileToSaved| Reverts the document associated with <p pINode> to its
// last saved state.
		HRESULT STDMETHODCALLTYPE RevertFileToSaved( IDMUSProdNode* pINode );
// @meth HRESULT | SaveNode| Invokes the Framework's File Save command for the document
// associated with the DocRoot node of <p pINode>.
		HRESULT STDMETHODCALLTYPE SaveNode( IDMUSProdNode* pINode );
// @meth HRESULT | SaveNodeAsNewFile| Invokes the Framework's File Save As command to create a new
// file for the node specified in <p pINode>.
		HRESULT STDMETHODCALLTYPE SaveNodeAsNewFile( IDMUSProdNode* pINode );

// @meth HRESULT | AddNode|  Adds one or more nodes to the Project Tree.
		HRESULT STDMETHODCALLTYPE AddNode( IDMUSProdNode* pITopNode, IDMUSProdNode* pIParentNode );
// @meth HRESULT | RemoveNode| Removes one or more nodes from the Project Tree.
		HRESULT STDMETHODCALLTYPE RemoveNode( IDMUSProdNode* pITopNode, BOOL fPromptUser  );
// @meth HRESULT | FindDocRootNode| Returns an IDMUSProdNode interface pointer for the matching DocRoot node.
	    HRESULT STDMETHODCALLTYPE FindDocRootNode( GUID guidProject, IDMUSProdDocType* pIDocType, BSTR bstrNodeName, BSTR bstrNodeDescriptor, IDMUSProdNode** ppIDocRootNode );
// @meth HRESULT | FindDocRootNodeByFileGUID| Returns an IDMUSProdNode interface pointer for the matching DocRoot node.
	    HRESULT STDMETHODCALLTYPE FindDocRootNodeByFileGUID( GUID guidFile, IDMUSProdNode** ppIDocRootNode );
// @meth HRESULT | GetBestGuessDocRootNode | Returns an IDMUSProdNode interface pointer for the closest matching DocRoot node.
        HRESULT STDMETHODCALLTYPE GetBestGuessDocRootNode( IDMUSProdDocType* pIDocType, BSTR bstrNodeName, IDMUSProdNode* pITreePositionNode, IDMUSProdNode** ppIDocRootNode );
// @meth HRESULT | GetSelectedNode | Returns the Project Tree's currently selected node.
        HRESULT STDMETHODCALLTYPE GetSelectedNode( IDMUSProdNode** ppINode );
// @meth HRESULT | SetSelectedNode| Highlights the Project Tree node associated with <p pINode>.
		HRESULT STDMETHODCALLTYPE SetSelectedNode( IDMUSProdNode* pINode );
// @meth HRESULT | RefreshNode| Refreshes and redraws the Project Tree node associated with <p pINode>.
		HRESULT STDMETHODCALLTYPE RefreshNode( IDMUSProdNode* pINode );
// @meth HRESULT | SortChildNodes| Sorts the Project Tree child nodes associated with <p pINode>.
		HRESULT STDMETHODCALLTYPE SortChildNodes( IDMUSProdNode* pINode );
// @meth HRESULT | EditNodeLabel| Begins in-place editing of the specified node's text.
		HRESULT STDMETHODCALLTYPE EditNodeLabel( IDMUSProdNode* pINode );
// @meth HRESULT | OpenEditor| Opens an editor for <p pINode>.
		HRESULT STDMETHODCALLTYPE OpenEditor( IDMUSProdNode* pINode );
// @meth HRESULT | CloseEditor| Closes the editor for <p pINode>.
		HRESULT STDMETHODCALLTYPE CloseEditor( IDMUSProdNode* pINode );

// @meth HRESULT | AddToNotifyList| Adds <p pINotifyThisNode> to the list of nodes notified when
//		the state of <p pIDocRootNode> changes.
		HRESULT STDMETHODCALLTYPE AddToNotifyList( IDMUSProdNode* pIDocRootNode, IDMUSProdNode* pINotifyThisNode );
// @meth HRESULT | RemoveFromNotifyList| Removes <p pINotifyThisNode> from the list of nodes notified
//		when the state of <p pIDocRootNode> changes.
		HRESULT STDMETHODCALLTYPE RemoveFromNotifyList( IDMUSProdNode* pIDocRootNode, IDMUSProdNode* pINotifyThisNode );
// @meth HRESULT | NotifyNodes| Notifies all nodes linked to <p pIChangedDocRootNode> via calls to 
//		<p AddToNotifyList()> that the state of <p pIChangedDocRootNode> has changed.
		HRESULT STDMETHODCALLTYPE NotifyNodes( IDMUSProdNode* pIChangedDocRootNode, GUID guidUpdateType, void* pData );

// @meth HRESULT | AddMenuItem| Adds an item to the Framework's 'Add-Ins' menu.
		HRESULT STDMETHODCALLTYPE AddMenuItem( IDMUSProdMenu* pIMenu );
// @meth HRESULT | RemoveMenuItem| Removes an item from the Framework's 'Add-Ins' menu.
		HRESULT STDMETHODCALLTYPE RemoveMenuItem( IDMUSProdMenu* pIMenu );
// @meth HRESULT | AddToolBar| Adds a toolbar to the Framework.
		HRESULT STDMETHODCALLTYPE AddToolBar( IDMUSProdToolBar* pIToolBar );
// @meth HRESULT | RemoveToolBar| Removes a toolbar from the Framework.
		HRESULT STDMETHODCALLTYPE RemoveToolBar( IDMUSProdToolBar* pIToolBar );

// @meth HRESULT | SetNbrStatusBarPanes| Creates panes in the Framework's status bar.
		HRESULT STDMETHODCALLTYPE SetNbrStatusBarPanes( int nCount, short nLifeSpan, HANDLE* phKey );
// @meth HRESULT | SetStatusBarPaneInfo| Sets the specified status bar pane to a new
// style and width.
		HRESULT STDMETHODCALLTYPE SetStatusBarPaneInfo( HANDLE hKey, int nIndex, StatusBarStyle sbStyle, int nMaxChars );
// @meth HRESULT | SetStatusBarPaneText| Sets the text of the specified status bar pane.
		HRESULT STDMETHODCALLTYPE SetStatusBarPaneText( HANDLE hKey, int nIndex, BSTR bstrText, BOOL bUpdate );
// @meth HRESULT | RestoreStatusBar| Removes panes from the Framework's status bar.
		HRESULT STDMETHODCALLTYPE RestoreStatusBar( HANDLE hKey );

// @meth HRESULT | StartProgressBar| Creates a progress bar control in the Framework's
// status bar.
		HRESULT STDMETHODCALLTYPE StartProgressBar( int nLower, int nUpper, BSTR bstrPrompt, HANDLE* phKey );
// @meth HRESULT | SetProgressBarPos| Sets the current position of the progress bar control.
		HRESULT STDMETHODCALLTYPE SetProgressBarPos( HANDLE hKey, int nPos );
// @meth HRESULT | SetProgressBarStep| Specifies the step increment for the progress bar control.
		HRESULT STDMETHODCALLTYPE SetProgressBarStep( HANDLE hKey, UINT nValue );
// @meth HRESULT | StepProgressBar| Advances current position of the progress bar control by the step
// increment.
		HRESULT STDMETHODCALLTYPE StepProgressBar( HANDLE hKey );
// @meth HRESULT | EndProgressBar| Removes the progress bar control from the Framework's
// status bar. 
		HRESULT STDMETHODCALLTYPE EndProgressBar( HANDLE hKey );

// @meth HRESULT | GetSharedObject| Allows one or more DirectMusic Producer Components to share
// the same COM object.
		HRESULT STDMETHODCALLTYPE GetSharedObject( REFCLSID rclsid, REFIID riid, void** ppvObject ); 

	// IDMUSProdFramework8 methods
// @meth HRESULT | GetNodeRuntimeFileName| Returns the runtime filename for <p pINode>.
		HRESULT STDMETHODCALLTYPE GetNodeRuntimeFileName( IDMUSProdNode* pINode, BSTR* pbstrRuntimeFileName );
// @meth HRESULT | ResolveBestGuessWhenLoadFinished| Adds <p pINotifySink> to a list which will receive
// FRAMEWORK_FileLoadFinished notifications, and returns a Producer generated GUID assigned to the
// referenced file. 
        HRESULT STDMETHODCALLTYPE ResolveBestGuessWhenLoadFinished( IDMUSProdDocType* pIDocType, BSTR bstrNodeName, IDMUSProdNode* pITreePositionNode, IDMUSProdNotifySink* pINotifySink, GUID* pguidFile );

/* --------------------------------------------------------------------------
@interface IDMUSProdFileRefChunk | 
	This interface contains methods to manage DirectMusic Producer file reference RIFF chunks.

@comm
	A file may need to associate itself with other files in its <o Project>.  For example,
	a Band may use one or more DLS files.  In order to establish a connection to other files,
	the saved Band contains one or more file reference chunks each pointing to a DLS file.

	<i IDMUSProdFileRefChunk> provides a standard way for DirectMusic Producer <o Component>s
	to place DirectMusic Producer file reference RIFF chunks in the design-time version of
	their files.  These chunks contain enough information so that DirectMusic Producer can 
	load the referenced file when the file is opened.

    <i IDMUSProdFileRefChunk> places Producer specific file reference chunks in streams so that
	links can be re-established within Producer the next time the file is opened by Producer.
	<i IDMUSProdLoaderRefChunk> places DirectMusic file reference chunks in streams so that links
	can be established when the file is loaded into the DirectMusic DLLs either externally or 
	via Producer.

	A DirectMusic Producer Component can call <om IDMUSProdFramework::QueryInterface> to
	obtain a pointer to the <o Framework>'s <i IDMUSProdFileRefChunk> interface.	

@base public | IUnknown

@xref  <o Node> Object, <i IDMUSProdComponent>, <i IDMUSProdFramework>, <i IDMUSProdNode>
-------------------------------------------------------------------------- */

	// IDMUSProdFileRefChunk methods
// @meth HRESULT | LoadRefChunk | Reads a DirectMusic Producer file reference RIFF chunk,
// loads the referenced file, and returns a pointer to the referenced file's DocRoot node.
        HRESULT STDMETHODCALLTYPE LoadRefChunk( IStream* pIStream, IDMUSProdNode** ppIDocRootNode );
// @meth HRESULT | SaveRefChunk | Saves a DirectMusic Producer file reference RIFF chunk.
        HRESULT STDMETHODCALLTYPE SaveRefChunk( IStream* pIStream, IDMUSProdNode* pIRefNode );
// @meth HRESULT | GetChunkSize | Returns the size of a DirectMusic Producer file reference RIFF
// chunk.
        HRESULT STDMETHODCALLTYPE GetChunkSize( DWORD* pdwSize );
// @meth HRESULT | ResolveWhenLoadFinished| Adds <p pINotifySink> to a list which will receive
//	FRAMEWORK_FileLoadFinished notifications, reads a DirectMusic Producer file reference RIFF chunk,
// and returns the Producer generated GUID assigned to the referenced file.
		HRESULT STDMETHODCALLTYPE ResolveWhenLoadFinished( IStream* pIStream, IDMUSProdNotifySink* pINotifySink, GUID* pguidFile );
// @meth HRESULT | GetFileRefInfo | Reads a DirectMusic Producer file reference RIFF chunk,
// and fills a DMUSProdFileRefInfo structure with information about the reference.
        HRESULT STDMETHODCALLTYPE GetFileRefInfo( IStream* pIStream, DMUSProdFileRefInfo* pFileRefInfo );

/* --------------------------------------------------------------------------
@interface IDMUSProdLoaderRefChunk | 
	This interface provides the means to embed file reference chunks into a stream for the
	purpose of enabling an implementation of IDirectMusicLoader to resolve file references.

@comm
	A file may need to associate itself with other files in its <o Project>.  For example,
	a Band may use one or more DLS files.  In order to establish a connection to other files,
	the saved Band contains one or more file reference chunks each pointing to a DLS file.

	<i IDMUSProdLoaderRefChunk> provides a standard way for DirectMusic Producer <o Component>s
	to place file reference chunks in the runtime versions of their DirectMusic files.  The
	DirectMusic Loader object, as well as other implementations of IDirectMusicLoader, know how
	to resolve the file reference chunks written through this interface.

    <i IDMUSProdFileRefChunk> places Producer specific file reference chunks in streams so that
	links can be re-established within Producer the next time the file is opened by Producer.
	<i IDMUSProdLoaderRefChunk> places DirectMusic file reference chunks in streams so that links
	can be established when the file is loaded into the DirectMusic DLLs either externally or 
	via Producer.

	A DirectMusic Producer Component can call <om IDMUSProdFramework::QueryInterface> to
	obtain a pointer to the <o Framework>'s <i IDMUSProdLoaderRefChunk> interface.	

@base public | IUnknown

@xref  <i IDMUSProdComponent>, <i IDMUSProdFramework>
-------------------------------------------------------------------------- */

	// IDMUSProdLoaderRefChunk methods
// @meth HRESULT | SaveRefChunkForLoader | Embeds a RIFF chunk into <p pIStream> that enables an
// implementation of IDirectMusicLoader to resolve file references.
		HRESULT STDMETHODCALLTYPE SaveRefChunkForLoader( IStream* pIStream, IDMUSProdNode* pIRefNode, REFCLSID rclsid, void*  pObjectDesc, WhichLoader wlWhichLoader );

	// Additional methods
public:
	HRESULT STDMETHODCALLTYPE StartWaitCursor();
	HRESULT STDMETHODCALLTYPE EndWaitCursor();
	BOOL LoadComponents();
	BOOL InitComponents();
	void CleanUp();
	void FreeComponents();
	void FreeDocTypes();
	void FreeClipFormats();
	void FreeSharedObjects();
	void OnActivateApp( BOOL fActive );
	BOOL RegisterClipboardFormats();
	void OnInitAddInsMenu( CMenu* pAddInsMenu );
	BOOL OnSelectAddInsMenu( UINT nCommandID );
	void GetAddInsMenuHelpText( UINT nCommandID, CString& rMessage ); 
	void FixAddInsMenu();
	void RedrawClientToolbars();
	BOOL RemoveNodes( IDMUSProdNode* pINode );
	BOOL SetNodeName( IDMUSProdNode* pINode, LPCTSTR szNewName );
	CJzComponent* GetJzComponent( IDMUSProdComponent* pIComponent );
	IDMUSProdNode* DetermineParentNode( LPCTSTR szFileName );
	HTREEITEM FindTreeItem( IDMUSProdNode* pINode );
	HTREEITEM FindTreeProjectByGUID( GUID guidProject );
	BOOL ShowTreeNode( IDMUSProdNode* pINode );
	void InsertViewMenuItem( CJzToolBar* pJzToolbar );
	void RemoveViewMenuItem( CJzToolBar* pJzToolbar );
	void OnInitViewMenu( CMenu* pViewMenu );
	BOOL OnSelectViewMenu( UINT nCommandID );
	void GetViewMenuHelpText( UINT nCommandID, CString& rMessage ); 
	void FixViewMenu();
	void SyncEditorTitles( IDMUSProdNode* pINode );
	HRESULT SyncNodeName( IDMUSProdNode* pINode, BOOL fRedraw );
	HRESULT SyncNodeIcon( IDMUSProdNode* pINode, BOOL fRedraw );
	HRESULT LoadClipFormat( IDataObject* pIDataObject, UINT uClipFormat, IStream** ppIStream );
	HRESULT LoadCF_DMUSPROD_FILE( IStream* pIStream , CFProducerFile* pProducerFile );
	HRESULT SaveCF_DMUSPROD_FILE( IDMUSProdNode* pINode, IStream** ppIStream );

	BOOL AreYouSure( IDMUSProdNode* pINode );
	HTREEITEM FindTreeItemByWP( wpWindowPlacement* pWP );
	void ApplyWPSettings( HTREEITEM hItem, wpWindowPlacement* pWP );
	long GetWPTreePos( IDMUSProdNode* pINode );
	BOOL AddComponentDocTemplates();
	BOOL IsDocRootNode( IDMUSProdNode* pIDocRootNode );
	UINT FindFileClipFormat( IDataObject* pIDataObject );
	HRESULT FindDocTypeByClipFormat( UINT uClipFormat, IDMUSProdDocType** ppIDocType );
	HRESULT FindExtensionByClipFormat( UINT uClipFormat, CString& strExt );

private:
	long CountWPTreePos( CTreeCtrl* pTreeCtrl, IDMUSProdNode* pINode, long lTreePos, HTREEITEM hItem );
	HTREEITEM SearchTreeForWP( CTreeCtrl* pTreeCtrl, wpWindowPlacement* pWP, long lTreePos, HTREEITEM hItem );
	HTREEITEM SearchTree( CTreeCtrl* pTreeCtrl, IDMUSProdNode* pINode, HTREEITEM hItem );
	BOOL AddNodes( IDMUSProdNode* pINode, HTREEITEM hParent );
	HRESULT ReadTheRefChunk( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, IDMUSProdNode** ppIDocRootNode );
	HRESULT WriteTheRefChunk( IDMUSProdRIFFStream* pIRiffStream, CFileNode* pFileNode );
	HRESULT WriteTheRefChunkForLoader( IDMUSProdRIFFStream* pIRiffStream, LPDMUS_OBJECTDESC pObjectDesc );
	HRESULT CreateFileFromDataObject( IDataObject* pIDataObject );
	HRESULT WriteTheFileFromDataObject( CFProducerFile* pcfProducerFile, IStream* pIMemStream, IDMUSProdDocType* pIDocType );

// Attributes
private:
    DWORD	m_dwRef;
	UINT	m_nNextMenuID;
	UINT	m_nNextToolBarID;
    CTypedPtrList<CPtrList, CJzComponent*> m_lstComponents;
    CTypedPtrList<CPtrList, IDMUSProdDocType*> m_lstDocTypes;
    CTypedPtrList<CPtrList, CJzMenu*> m_lstMenus;
    CTypedPtrList<CPtrList, CJzToolBar*> m_lstToolBars;
    CTypedPtrList<CPtrList, CJzClipFormat*> m_lstClipFormats;
    CTypedPtrList<CPtrList, CJzSharedObject*> m_lstSharedObjects;

public:
	UINT	m_cfProducerFile;		// CF_DMUSPROD_FILE clipboard format
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __FRAMEWORK_H__
