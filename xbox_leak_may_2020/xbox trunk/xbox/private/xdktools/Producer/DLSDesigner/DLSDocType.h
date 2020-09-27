#ifndef __DLSDOCTYPE_H__
#define __DLSDOCTYPE_H__

// CDLSDocType.h : header file
//

class CDLSComponent;

class CDLSDocType : public IDMUSProdDocType8
{
public:
    CDLSDocType(CDLSComponent* pComponent);
	~CDLSDocType();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID *ppv);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDLSDocType functions
    HRESULT STDMETHODCALLTYPE GetResourceId(HINSTANCE* phInstance, UINT* pnResourceId);
    HRESULT STDMETHODCALLTYPE DoesExtensionMatch(BSTR bstrExt);
    HRESULT STDMETHODCALLTYPE DoesIdMatch(REFGUID rguid);
    HRESULT STDMETHODCALLTYPE AllocNode(REFGUID rguid, IDMUSProdNode** ppINode);

    HRESULT STDMETHODCALLTYPE OnFileNew(IDMUSProdProject* pITargetProject,
										IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode);
    HRESULT STDMETHODCALLTYPE OnFileOpen(IStream* pIStream, IDMUSProdProject* pITargetProject,
										 IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode);
	HRESULT STDMETHODCALLTYPE OnFileSave(IStream* pIStream, IDMUSProdNode* pIDocRootNode);
    HRESULT STDMETHODCALLTYPE IsFileTypeExtension(FileType ftFileType, BSTR bstrExt);   
	HRESULT STDMETHODCALLTYPE GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo );	

    // IDMUSProdDocType8 functions
    HRESULT STDMETHODCALLTYPE GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc );
    HRESULT STDMETHODCALLTYPE GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType );
    HRESULT STDMETHODCALLTYPE GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt );

	// Additional functions

private:
    CDLSDocType();

private:
    DWORD				m_dwRef;
	CDLSComponent*		m_pComponent;
};

#endif // __DLSDOCTYPE_H__
