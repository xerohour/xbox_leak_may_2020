#ifndef __SCRIPT_H__
#define __SCRIPT_H__

// Script.h : header file
//

#include "ScriptComponent.h"
#include "ScriptDocType.h"
#include <RiffStrm.h>


struct VariableState
{
	VariableState()
	{
		VariantInit( &m_Variant );
	}

	~VariableState()
	{
		VariantClear( &m_Variant );
	}

	CString		m_strName;		// Name of variable
	VARIANT		m_Variant;		// Value of variable
};


class CDirectMusicScript;

//////////////////////////////////////////////////////////////////////
//  CScriptPropPageManager

class CScriptPropPageManager : public IDMUSProdPropPageManager 
{
friend class CTabScriptScript;
friend class CTabScriptInfo;

public:
	CScriptPropPageManager();
	virtual ~CScriptPropPageManager();

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
	
	CTabScriptScript*			m_pTabScript;
	CTabScriptInfo*				m_pTabInfo;

public:
	static short				sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CDirectMusicScript

class CDirectMusicScript : public IDMUSProdNode, public IPersistStream, public IDMUSProdPropPageObject,
						   public IDMUSProdNotifySink
{
friend class CScriptCtrl;
friend class CScriptDlg;
friend class CEditSource;
friend class CTabScriptScript;
friend class CTabScriptInfo;

public:
    CDirectMusicScript();
	~CDirectMusicScript();

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

	//Additional functions
protected:
	HRESULT LoadScript( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT SaveScript( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty );
    HRESULT SaveHeader( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveGUID( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveInfoList( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveLanguage( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveVersion( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveDirectMusicVersion( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT SaveSource( IDMUSProdRIFFStream* pIRiffStream );

	HRESULT ReadSourceFromStream( IStream* pIStream );
	HRESULT ParseHeaderBlock( const WCHAR **ppwszSource, DMUSProdListInfo* pListInfo );
	HRESULT ParseField( const WCHAR **ppwsz, DMUSProdListInfo* pListInfo );
	HRESULT FoundField( const WCHAR *pwszName, const WCHAR *pwszValue, DMUSProdListInfo* pListInfo );

    HRESULT SaveScriptSource( IStream* pIStream );

	void GetVariableState( CTypedPtrList<CPtrList, VariableState*>& list );
	void SetVariableState( CTypedPtrList<CPtrList, VariableState*>& list );

public:
	HRESULT ReadListInfoFromStream( IStream* pIStream, DMUSProdListInfo* pListInfo );
	HRESULT ReadListInfoFromLegacyStream( IStream* pIStream, DMUSProdListInfo* pListInfo );
    HRESULT GetObjectDescriptor( void* pObjectDesc );

public:
	HRESULT CreateEmptyContainer();
	BOOL CreateUndoMgr();
	void SetModified( BOOL fModified );
	void SyncScriptEditor( DWORD dwFlags );
	HRESULT SyncScriptWithDirectMusic();
	void DisplayScriptError( DMUS_SCRIPT_ERRORINFO* pErrorinfo, HRESULT hrFromScript );
	void GetGUID( GUID* pguidScript );
	void SetGUID( GUID guidScript );

private:
    DWORD				m_dwRef;
	BOOL				m_fModified;

	CJazzUndoMan*		m_pUndoMgr;

	IDirectMusicScript*	m_pIDMScript;

	IDMUSProdNode*		m_pIDocRootNode;
	IDMUSProdNode*	    m_pIParentNode;
	IDMUSProdNode*		m_pIContainerNode;

	HWND				m_hWndEditor;
	bool				m_fDeletingContainer;
	bool				m_fInitializingVariables;

	CTypedPtrList<CPtrList, VariableState*> m_lstLastKnownVariableStates;

public:
	CString				m_strOrigFileName;
	CScriptCtrl*		m_pScriptCtrl;
	IDMUSProdNode*		m_pINodeBeingDeleted;
	IDMUSProdProject*	m_pIProject;

private:
	// Persisted for DirectMusic
	DWORD				m_dwFlagsDM;
	GUID				m_guidScript;
	DMUS_VERSION        m_vVersion;
	DMUS_VERSION		m_vDirectMusicVersion;
    CString				m_strName;
    CString				m_strAuthor;
    CString				m_strCopyright;
    CString				m_strSubject;
	CString				m_strLanguage;
    CString				m_strSource;
};

#endif // __SCRIPT_H__
