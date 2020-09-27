#ifndef __TEMPLATEDOCTYPE_H__
#define __TEMPLATEDOCTYPE_H__

// TemplateDocType.h : header file
//


class CTemplateDocType : public IDMUSProdDocType8
{
public:
    CTemplateDocType( CSegmentComponent* pComponent );
	~CTemplateDocType();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // ITemplateDocType functions
    HRESULT STDMETHODCALLTYPE GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId );
    HRESULT STDMETHODCALLTYPE DoesExtensionMatch( BSTR bstrExt );
    HRESULT STDMETHODCALLTYPE DoesIdMatch( REFGUID rguid );
    HRESULT STDMETHODCALLTYPE AllocNode( REFGUID rguid, IDMUSProdNode** ppINode );

    HRESULT STDMETHODCALLTYPE OnFileNew(IDMUSProdProject* pITargetProject,
										IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode);
    HRESULT STDMETHODCALLTYPE OnFileOpen(IStream* pIStream, IDMUSProdProject* pITargetProject,
										 IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode);
	HRESULT STDMETHODCALLTYPE OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode );

    HRESULT STDMETHODCALLTYPE IsFileTypeExtension( FileType ftFileType, BSTR bstrExt );   
	HRESULT STDMETHODCALLTYPE GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo );	

    // IDMUSProdDocType8 functions
    HRESULT STDMETHODCALLTYPE GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc );
    HRESULT STDMETHODCALLTYPE GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType );
    HRESULT STDMETHODCALLTYPE GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt );

private:
    CTemplateDocType();

private:
    DWORD			m_dwRef;
	CSegmentComponent* m_pComponent;
};

#endif // __TEMPLATEDOCTYPE_H__
