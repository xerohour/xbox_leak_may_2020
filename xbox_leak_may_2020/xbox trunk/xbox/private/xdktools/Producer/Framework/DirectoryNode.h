// DirectoryNode.h: interface for the CDirectoryNode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DIRECTORYNODE_H__
#define __DIRECTORYNODE_H__


//////////////////////////////////////////////////////////////////////
//  CDirectoryNode

class CDirectoryNode : public IDMUSProdNode
{
friend class CProject;

public:
	CDirectoryNode();
	virtual ~CDirectoryNode();

    // IUnknown functions
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	virtual HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnFirstImage );
	virtual HRESULT STDMETHODCALLTYPE UseOpenCloseImages( BOOL* pfUseOpenCloseImages );

    virtual HRESULT STDMETHODCALLTYPE GetFirstChild( IDMUSProdNode** ppIFirstChildNode );
    virtual HRESULT STDMETHODCALLTYPE GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode );

    virtual HRESULT STDMETHODCALLTYPE GetComponent( IDMUSProdComponent** ppIComponent );
    virtual HRESULT STDMETHODCALLTYPE GetDocRootNode( IDMUSProdNode** ppIDocRootNode );
    virtual HRESULT STDMETHODCALLTYPE SetDocRootNode( IDMUSProdNode* pIDocRootNode );
    virtual HRESULT STDMETHODCALLTYPE GetParentNode( IDMUSProdNode** ppIParentNode );
    virtual HRESULT STDMETHODCALLTYPE SetParentNode( IDMUSProdNode* pIParentNode );

    virtual HRESULT STDMETHODCALLTYPE GetNodeId( GUID* pguid );
    virtual HRESULT STDMETHODCALLTYPE GetNodeName( BSTR* pbstrName );
    virtual HRESULT STDMETHODCALLTYPE GetNodeNameMaxLength( short* pnMaxLength );
    virtual HRESULT STDMETHODCALLTYPE ValidateNodeName( BSTR bstrName );
    virtual HRESULT STDMETHODCALLTYPE SetNodeName( BSTR bstrName );
    virtual HRESULT STDMETHODCALLTYPE GetNodeListInfo( DMUSProdListInfo* pListInfo );

    virtual HRESULT STDMETHODCALLTYPE GetEditorClsId( CLSID* pclsid );
    virtual HRESULT STDMETHODCALLTYPE GetEditorTitle( BSTR* pbstrTitle );
    virtual HRESULT STDMETHODCALLTYPE GetEditorWindow( HWND* hWndEditor );
    virtual HRESULT STDMETHODCALLTYPE SetEditorWindow( HWND hWndEditor );

    virtual HRESULT STDMETHODCALLTYPE GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnResourceId );
    virtual HRESULT STDMETHODCALLTYPE OnRightClickMenuInit( HMENU hMenu );
    virtual HRESULT STDMETHODCALLTYPE OnRightClickMenuSelect( long lCommandId );

    virtual HRESULT STDMETHODCALLTYPE DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser );
    virtual HRESULT STDMETHODCALLTYPE InsertChildNode( IDMUSProdNode* pIChildNode );
    virtual HRESULT STDMETHODCALLTYPE DeleteNode( BOOL fPromptUser );

	virtual HRESULT STDMETHODCALLTYPE OnNodeSelChanged( BOOL fSelected );

	virtual HRESULT STDMETHODCALLTYPE CreateDataObject( IDataObject** ppIDataObject );
	virtual HRESULT STDMETHODCALLTYPE CanCut();
	virtual HRESULT STDMETHODCALLTYPE CanCopy();
	virtual HRESULT STDMETHODCALLTYPE CanDelete();
	virtual HRESULT STDMETHODCALLTYPE CanDeleteChildNode( IDMUSProdNode* pIChildNode );
	virtual HRESULT STDMETHODCALLTYPE CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference );
	virtual HRESULT STDMETHODCALLTYPE PasteFromData( IDataObject* pIDataObject );
	virtual HRESULT STDMETHODCALLTYPE CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL* pfWillSetReference );
	virtual HRESULT STDMETHODCALLTYPE ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode );

	virtual HRESULT STDMETHODCALLTYPE GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject ); 

	//Additional methods
public:
	BOOL DeleteDirectoryToRecycleBin();
	void ConstructPath( CString& strPath );
	void NewFolder();
	CDirectoryNode* GetSubDirectoryByName( LPCTSTR szName );
	CDirectoryNode* FindDirNode( LPCTSTR szPathName );
	void NotifyAllFiles( GUID guidNotification );
	void SortTree();

private:
	HRESULT PasteOpenFile( CFProducerFile* pcfProducerFile, IStream* pIMemStream, IDMUSProdDocType* pIDocType );
	HRESULT PasteShutFile( CFProducerFile* pcfProducerFile );
	HRESULT GetPasteFileName( CFProducerFile* pcfProducerFile, CString& strFileName );

	// Member variables
private:
    CTypedPtrList<CPtrList, IDMUSProdNode*> m_lstNodes;

	IDMUSProdNode*	m_pIParentNode;

protected:
    DWORD		m_dwRef;

public:
	CString		m_strName;
	HTREEITEM	m_hItem;	// Node's HTREEITEM in Project Tree
};

#endif //__DIRECTORYNODE_H__
