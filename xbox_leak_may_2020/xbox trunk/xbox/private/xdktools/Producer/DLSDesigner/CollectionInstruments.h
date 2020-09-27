#ifndef __COLLECTIONINSTRUMENTS_H__
#define __COLLECTIONINSTRUMENTS_H__

// CollectionInstruments.h : header file
//

#include "alist.h"
#include "DllJazzDataObject.h"

class CCollection;
class CInstrument;

class CCollectionInstruments : public AList, public IDMUSProdNode
{
friend class CCollection;
friend class CDLSComponent;

public:
    CCollectionInstruments();
	~CCollectionInstruments();

	//	AList overrides
    CInstrument *GetHead();
    CInstrument *RemoveHead();
	void Remove(CInstrument *pInstrument);
	void AddTail(CInstrument *pInstrument);

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnNbrFirstImage );

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

    HRESULT STDMETHODCALLTYPE GetEditorClsId( CLSID* pclsid );
    HRESULT STDMETHODCALLTYPE GetEditorTitle( BSTR* pbstrTitle );
    HRESULT STDMETHODCALLTYPE GetEditorWindow( HWND* hWndEditor );
    HRESULT STDMETHODCALLTYPE SetEditorWindow( HWND hWndEditor );

	HRESULT STDMETHODCALLTYPE UseOpenCloseImages( BOOL* pfUseOpenCloseImages );

    HRESULT STDMETHODCALLTYPE GetRightClickMenuId(HINSTANCE* phInstance, UINT* pnResourceId);
    HRESULT STDMETHODCALLTYPE OnRightClickMenuInit(HMENU hMenu);
    HRESULT STDMETHODCALLTYPE OnRightClickMenuSelect(long lCommandId);

    HRESULT STDMETHODCALLTYPE DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser );
    HRESULT STDMETHODCALLTYPE InsertChildNode( IDMUSProdNode* pIChildNode );
    HRESULT STDMETHODCALLTYPE DeleteNode( BOOL fPromptUser );

	HRESULT STDMETHODCALLTYPE GetFirstChild(IDMUSProdNode** ppIFirstChildNode);
	HRESULT STDMETHODCALLTYPE GetNextChild(IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode);
	HRESULT STDMETHODCALLTYPE GetNodeListInfo(DMUSProdListInfo* pListInfo);

	HRESULT STDMETHODCALLTYPE OnNodeSelChanged( BOOL fSelected );

    // IDMUSProdNode: drag & drop methods
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

	//		Additional functions.

	HRESULT Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
	HRESULT PasteCF_INSTRUMENT( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject);

	void	RefreshAllNodes();

private:
    DWORD				m_dwRef;
	IDMUSProdNode*		m_pIRootNode;
	IDMUSProdNode*		m_pIParentNode;

	CDLSComponent*	 m_pDLSComponent;
	CCollection*	 m_pCollection;
};

#endif // __COLLECTIONINSTRUMENTS_H__
