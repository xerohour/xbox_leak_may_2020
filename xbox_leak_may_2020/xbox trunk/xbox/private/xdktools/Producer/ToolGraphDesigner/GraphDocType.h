#ifndef __GRAPHDOCTYPE_H__
#define __GRAPHDOCTYPE_H__

// GraphDocType.h : header file
//


class CGraphDocType : public IDMUSProdDocType8
{
public:
    CGraphDocType();
	~CGraphDocType();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdDocType functions
    HRESULT STDMETHODCALLTYPE GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId );
    HRESULT STDMETHODCALLTYPE DoesExtensionMatch( BSTR bstrExt );
    HRESULT STDMETHODCALLTYPE DoesIdMatch( REFGUID rguid );
    HRESULT STDMETHODCALLTYPE AllocNode( REFGUID rguid, IDMUSProdNode** ppINode );

    HRESULT STDMETHODCALLTYPE OnFileNew(IDMUSProdProject* pITargetProject,
										IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode);
    HRESULT STDMETHODCALLTYPE OnFileOpen(IStream* pIStream, IDMUSProdProject* pITargetProject,
										 IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode);
    HRESULT STDMETHODCALLTYPE OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode );
    HRESULT STDMETHODCALLTYPE GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo );

	HRESULT STDMETHODCALLTYPE IsFileTypeExtension( FileType ftFileType, BSTR bstrExt );   

    // IDMUSProdDocType8 functions
    HRESULT STDMETHODCALLTYPE GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc );
    HRESULT STDMETHODCALLTYPE GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType );
    HRESULT STDMETHODCALLTYPE GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt );

	// Additional functions

	// Member variables
private:
    DWORD m_dwRef;
};

#endif // __GRAPHDOCTYPE_H__
