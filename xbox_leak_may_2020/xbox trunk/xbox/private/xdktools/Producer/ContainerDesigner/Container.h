#ifndef __CONTAINER_H__
#define __CONTAINER_H__

// Container.h : header file
//

#include <RiffStrm.h>
#include "ContainerComponent.h"
#include "ContainerDocType.h"
#include "Folder.h"

class CDirectMusicContainer;


#define MAX_FILE_PRIORITY	10

class CObjectToSave
{
public:
	CObjectToSave( CContainerObject* pContainerObject, BOOL fFromReferenceFolder );
	~CObjectToSave();

protected:
	short GetPriority();

public:	
	CContainerObject*	m_pContainerObject;
	short				m_nPriority;
	BOOL				m_fFromReferenceFolder;
};


//////////////////////////////////////////////////////////////////////
//  CContainerPropPageManager

class CContainerPropPageManager : public IDMUSProdPropPageManager 
{
friend class CTabContainer;
friend class CTabInfo;

public:
	CContainerPropPageManager();
	virtual ~CContainerPropPageManager();

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
	
	CTabContainer*				m_pTabContainer;
	CTabInfo*					m_pTabInfo;

public:
	static short				sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CDirectMusicContainer

class CDirectMusicContainer : public IDMUSProdNode, public IPersistStream, public IDMUSProdPropPageObject,
							  public IDMUSProdNotifySink, IDMUSProdContainerInfo
{
friend class CContainerDocType;
friend class CContainerCtrl;
friend class CContainerDlg;
friend class CTabContainer;
friend class CTabInfo;
friend class CTabObjectFlags;
friend class CFolder;

public:
    CDirectMusicContainer();
	~CDirectMusicContainer();

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
	HRESULT STDMETHODCALLTYPE CanPasteFromData( IDataObject* pIDataObject, BOOL *pfWillSetReference );
	HRESULT STDMETHODCALLTYPE PasteFromData( IDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL *pfWillSetReference );
	HRESULT STDMETHODCALLTYPE ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode );

	HRESULT STDMETHODCALLTYPE GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject ); 

    // IPersist functions
    STDMETHOD(GetClassID)( CLSID* pClsId );

    // IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

	// IDMUSProdNotifySink
	HRESULT STDMETHODCALLTYPE OnUpdate( IDMUSProdNode* pINode, GUID guidUpdateType, VOID* pData );

	// IDMUSProdContainerInfo
    HRESULT STDMETHODCALLTYPE FindDocRootFromName( BSTR bstrName, IUnknown** ppIDocRootNode );
    HRESULT STDMETHODCALLTYPE FindDocRootFromScriptAlias( BSTR bstrAlias, IUnknown** ppIDocRootNode );

	//Additional functions
protected:
	HRESULT LoadContainer( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT SaveContainer( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveHeader( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveGUID( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveInfoList( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveVersion( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveObjects( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT IsCircularReference( IDMUSProdNode* pIDocRootNode, CFolder* pTargetFolder ); 
	
	HRESULT CreateObjectsToSaveList( CTypedPtrList<CPtrList, CObjectToSave*>& list );
	void InsertObjectToSave( CTypedPtrList<CPtrList, CObjectToSave*>& list, CObjectToSave* pObjectToInsert );

public:
	HRESULT ReadListInfoFromStream( IStream* pIStream, DMUSProdListInfo* pListInfo );
	BOOL CreateUndoMgr();
	BOOL IsInScript();
	BOOL IsInSegment();
	BOOL IsScriptAliasUnique( CContainerObject* pObject );
	void MakeUniqueScriptAlias( CContainerObject* pObject );
	void SetModified( BOOL fModified );
	void SyncContainerEditor();
	HRESULT IsDocRootInContainer( IDMUSProdNode* pIDocRootNode );
	HRESULT IsFileGUIDInContainer( GUID guidFile );
	void GetGUID( GUID* pguidContainer );
	void SetGUID( GUID guidContainer );
	void OnAddRemoveFiles();

private:
    DWORD				m_dwRef;
	BOOL				m_fModified;

	IDMUSProdNode*		m_pIDocRootNode;
	IDMUSProdNode*	    m_pIParentNode;

	HWND				m_hWndEditor;

public:
	CString				m_strOrigFileName;
	CContainerCtrl*		m_pContainerCtrl;
	IDMUSProdNode*		m_pINodeBeingDeleted;
	IDMUSProdProject*	m_pIProject;
	CJazzUndoMan*		m_pUndoMgr;

private:
	CFolder				m_FolderEmbed;
	CFolder				m_FolderReference;

    // Persisted for DirectMusic
    CString				m_strName;
    CString				m_strAuthor;
    CString				m_strCopyright;
    CString				m_strSubject;
    CString				m_strInfo;
	GUID				m_guidContainer;
	DMUS_VERSION        m_vVersion;
	DWORD				m_dwFlagsDM;
};

#endif // __CONTAINER_H__
