#ifndef __CONTAINER_OBJECT_H_
#define __CONTAINER_OBJECT_H_

#include "RiffStrm.h"


#define DMUS_FOURCC_CONTAINED_OBJECT_UI_CHUNK			mmioFOURCC('c','o','b','u')

#define COBU_REFERENCE	0x0001


#pragma pack(2)

typedef struct ioObjectUI
{
	WORD	wFlags;			// COBU_ flags
} ioObjectUI;

typedef struct FileListInfo
{
	FileListInfo()
	{
		pIProject = NULL;
		memset( &guidFile, 0, sizeof(GUID) );
	}

	IDMUSProdProject*	pIProject;
	CString				strProjectName;
	CString				strName;
	CString				strDescriptor;
	GUID				guidFile;
} FileListInfo;

typedef struct FileRef
{
	FileRef()
	{
		pIDocRootNode = NULL;
		pIRefNode = NULL;
		fRemoveNotify = FALSE;
	}

	IDMUSProdNode*		pIDocRootNode;	// Pointer to referenced file's DocRoot node
	IDMUSProdNode*		pIRefNode;		// Pointer to referenced file's Reference node
	CString				strScriptAlias;	// Used by scripting
	BOOL				fRemoveNotify;
	FileListInfo		li;
} FileRef;

#pragma pack()


//////////////////////////////////////////////////////////////////////
//  CObjectPropPageManager

class CObjectPropPageManager : public IDMUSProdPropPageManager 
{
friend class CTabObjectDesign;
friend class CTabObjectRuntime;
friend class CTabObjectFlags;

public:
	CObjectPropPageManager();
	virtual ~CObjectPropPageManager();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();

    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
    HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject );

    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE IsEqualPageManagerGUID( REFGUID rguidPageManager );

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:
    DWORD						m_dwRef;
	IDMUSProdPropSheet*			m_pIPropSheet;
	IDMUSProdPropPageObject*	m_pIPropPageObject;
	
	CTabObjectDesign*			m_pTabDesign;
	CTabObjectRuntime*			m_pTabRuntime;
	CTabObjectFlags*			m_pTabFlags;

public:
	static short				sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CContainerObject

class CContainerObject : public IDMUSProdNode, public IDMUSProdPropPageObject
{
friend class CDirectMusicContainer;
friend class CObjectToSave;
friend class CObjectPropPageManager;
friend class CTabContainer;
friend class CTabObjectDesign;
friend class CTabObjectRuntime;
friend class CTabObjectFlags;
friend class CFolder;

public:
	CContainerObject( CDirectMusicContainer* pContainer );
	virtual ~CContainerObject();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnNbrFirstImage );
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

	HRESULT STDMETHODCALLTYPE UseOpenCloseImages( BOOL* pfUseOpenCloseImages );

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
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

	// Additional methods
	HRESULT Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, BOOL* pfReference );
    HRESULT Save( IDMUSProdRIFFStream* pIRiffStream, BOOL fSaveReference );
	HRESULT SetFileReference( IDMUSProdNode* pINewDocRootNode );
	HRESULT SetFileReference( GUID guidFile );
	HRESULT SyncListInfo();
	void ClearListInfo();
	void GetScriptAlias( CString& strScriptAlias );
	void SetScriptAlias( const LPCTSTR pszScriptAlias );
	HRESULT ResolveBestGuessWhenLoadFinished( IDMUSProdDocType* pIDocType, BSTR bstrObjectName, IDMUSProdNode* pITargetDirectoryNode );

private:
	HRESULT SaveDMRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode, DMUS_OBJECTDESC* pdmusObjectDesc, WhichLoader whichLoader );
	HRESULT SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode );
	HRESULT SaveObjectPendingLoad( IDMUSProdRIFFStream* pIRiffStream, BOOL fSaveReference );
	void SetModifiedFlag();

private:
    DWORD					m_dwRef;
	CDirectMusicContainer*	m_pContainer;
	IDMUSProdNode*			m_pIDocRootNode;
	IDMUSProdNode*			m_pIParentNode;

	FileRef					m_FileRef;		// Info pertaining to referenced file
	DWORD					m_dwFlagsDM;	// DirectMusic DMUS_CONTAINED_OBJF flags
};

#endif // __CONTAINER_OBJECT_H_
