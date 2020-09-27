// FileNode.h: interface for the CFileNode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILENODE_H__
#define __FILENODE_H__

class CProject;
class CTabFileDesign;
class CTabFileRuntime;

// Flags used when loading a Project
#define FSF_NOFLAGS		0x0000
#define FSF_DOSYNC		0x0001	// File needs synced
#define FSF_EXISTS		0x0002	// File exists on disk

// "Runtime" save actions
#define RSA_NOACTION	0
#define RSA_SAVE		1
#define RSA_SKIP		2


//////////////////////////////////////////////////////////////////////
//  CFilePropPageManager

class CFilePropPageManager : public CAppBasePropPageManager 
{
friend class CTabFileDesign;
friend class CTabFileRuntime;

public:
	CFilePropPageManager();
	virtual ~CFilePropPageManager();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Additional functions
private:
	void RemoveCurrentObject( void );

	// Member variables
private:
	CTabFileDesign*		m_pTabDesign;
	CTabFileRuntime*	m_pTabRuntime;

public:
	static short		sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CFileNode

class CFileNode : public IDMUSProdNode, public IDMUSProdPropPageObject
{
friend class CFramework;
friend class CProject;
friend class CComponentDoc;
friend class CDirectoryNode;
friend class CTreeBar;
friend class CDeleteFileDlg;
friend class CRuntimeDupeDlg;
friend class CNotifyList;
friend class CCloseProjectDlg;

public:
	CFileNode();
	virtual ~CFileNode();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnFirstImage );
	HRESULT STDMETHODCALLTYPE UseOpenCloseImages( BOOL* pfUseOpenCloseImages );

    HRESULT STDMETHODCALLTYPE GetFirstChild( IDMUSProdNode** ppIFirstChildNode );
    HRESULT STDMETHODCALLTYPE GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode );

    HRESULT STDMETHODCALLTYPE GetComponent( IDMUSProdComponent** ppIComponent );
    HRESULT STDMETHODCALLTYPE GetDocRootNode( IDMUSProdNode** ppIDocRootNode );
    HRESULT STDMETHODCALLTYPE SetDocRootNode( IDMUSProdNode* pIDocRootNode );
    HRESULT STDMETHODCALLTYPE GetParentNode( IDMUSProdNode** ppIParentNode );
    HRESULT STDMETHODCALLTYPE SetParentNode( IDMUSProdNode* pIParentNode );

    HRESULT STDMETHODCALLTYPE GetNodeId( GUID* pguid );
    HRESULT STDMETHODCALLTYPE GetNodeName( BSTR* pbstrName );
    HRESULT STDMETHODCALLTYPE GetNodeNameMaxLength( short* pnMaxLength );
    HRESULT STDMETHODCALLTYPE ValidateNodeName( BSTR bstrName );
    HRESULT STDMETHODCALLTYPE SetNodeName( BSTR bstrName );
    HRESULT STDMETHODCALLTYPE GetNodeListInfo( DMUSProdListInfo* pListInfo );

    HRESULT STDMETHODCALLTYPE GetEditorClsId( CLSID* pclsid );
    HRESULT STDMETHODCALLTYPE GetEditorTitle( BSTR* pbstrTitle );
    HRESULT STDMETHODCALLTYPE GetEditorWindow( HWND* hWndEditor );
    HRESULT STDMETHODCALLTYPE SetEditorWindow( HWND hWndEditor );

    HRESULT STDMETHODCALLTYPE GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnResourceId );
    HRESULT STDMETHODCALLTYPE OnRightClickMenuInit( HMENU hMenu );
    HRESULT STDMETHODCALLTYPE OnRightClickMenuSelect( long lCommandId );

    HRESULT STDMETHODCALLTYPE DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser );
    HRESULT STDMETHODCALLTYPE InsertChildNode( IDMUSProdNode* pIChildNode );
    HRESULT STDMETHODCALLTYPE DeleteNode( BOOL fPromptUser );

	HRESULT STDMETHODCALLTYPE OnNodeSelChanged( BOOL fSelected );

	HRESULT STDMETHODCALLTYPE CreateDataObject( IDataObject** ppIDataObject );
	HRESULT STDMETHODCALLTYPE CanCut();
	HRESULT STDMETHODCALLTYPE CanCopy();
	HRESULT STDMETHODCALLTYPE CanDelete();
	HRESULT STDMETHODCALLTYPE CanDeleteChildNode( IDMUSProdNode* pIChildNode );
	HRESULT STDMETHODCALLTYPE CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE PasteFromData( IDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode );

	HRESULT STDMETHODCALLTYPE GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject ); 

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();
	HRESULT STDMETHODCALLTYPE OnShowProperties();

	//Additional methods
	void ReleaseDocument();
	BOOL IsProducerFile();

public:
	HRESULT LoadTheFile( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT SaveTheFile( IDMUSProdRIFFStream* pIRiffStream );
	void LinkToRuntimeFile( LPCTSTR szRuntimeFileName );
	void ConstructFileName( CString& strFileName );
	void ConstructRuntimePath( CString& strRuntimePath );
	BOOL ConstructRelativePath( CString& strRelativePath );
	void SyncListInfo();
	void MoveWPListToNodes();
	void EmptyNotifyNodesList();
	void AddToNotifyWhenLoadFinished( IDMUSProdNotifySink* pINotifySink );
	void SendLoadFinishedNotifications();

private:	
	HRESULT SaveFileChunk( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveFileNameChunk( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveFileInfo( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveNotifyNodesList( IDMUSProdRIFFStream* pIRiffStream );

	HRESULT LoadNodeWP( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, wpWindowPlacement* pWP );
	HRESULT SaveNodeWP( IDMUSProdRIFFStream* pIRiffStream, wpWindowPlacement* pWP );
	HRESULT SaveNodeWPChunk( IDMUSProdRIFFStream* pIRiffStream, wpWindowPlacement* pWP );

	HRESULT SaveTreeNodesWP( IDMUSProdRIFFStream* pIRiffStream, CTreeCtrl* pTreeCtrl, HTREEITEM hItem );

	void AddToNotifyList( IDMUSProdNode* pINotifyThisNode ); 
	void RemoveFromNotifyList( IDMUSProdNode* pINotifyThisNode ); 

	// Member variables
private:
    DWORD			m_dwRef;
	IDMUSProdNode*	m_pIParentNode;
	CString			m_strListInfoName;
	CString			m_strListInfoDescriptor;
	GUID			m_guidListInfoObject;
    CTypedPtrList<CPtrList, CJzNotifyNode*> m_lstNotifyNodes;
	WORD			m_wFlags;
	short			m_nRuntimeSaveAction;

    CTypedPtrList<CPtrList, IDMUSProdNotifySink*> m_lstNotifyWhenLoadFinished;

public:
    GUID			m_guid;					// File's "DirectMusic Producer" GUID	
	CProject*		m_pProject;				// File's Project
	CComponentDoc*	m_pComponentDoc;		// File's Document
	HTREEITEM		m_hItem;				// File's HTREEITEM in Project Tree
	HTREEITEM		m_hChildItem;			// DocRoot's HTREEITEM in Project Tree
	IDMUSProdNode*	m_pIChildNode;			// File's DocRoot Node
	BOOL			m_fInOnOpenDocument;	// In process of loading the file
	CString			m_strName;
	CString			m_strRuntimeFile;		// Last saved Runtime file
	CString			m_strRuntimeFolder;		// Folder used when saving Runtime file		
	CString			m_strRuntimeFileName;	// Name used when saving Runtime file (funk.sty)
    CTypedPtrList<CPtrList, wpWindowPlacement*> m_lstWP;
};

#endif //__FILENODE_H__
