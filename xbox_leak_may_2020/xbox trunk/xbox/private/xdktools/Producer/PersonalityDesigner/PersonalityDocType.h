#ifndef __PERSONALITYDOCTYPE_H__
#define __PERSONALITYDOCTYPE_H__

// PersonalityDocType.h : header file
//


class CPersonalityDocType : public IDMUSProdDocType8
{
public:
	HRESULT DMGetListInfo(IStream* pIStream, DMUSProdListInfo* pListInfo);
    CPersonalityDocType( CPersonalityComponent* pComponent );
	~CPersonalityDocType();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IPersonalityDocType functions
    HRESULT STDMETHODCALLTYPE GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId );
    HRESULT STDMETHODCALLTYPE DoesExtensionMatch( BSTR bstrExt );
    HRESULT STDMETHODCALLTYPE DoesIdMatch( REFGUID rguid );
	HRESULT STDMETHODCALLTYPE GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo );
    HRESULT STDMETHODCALLTYPE AllocNode( REFGUID rguid, IDMUSProdNode** ppINode );

    HRESULT STDMETHODCALLTYPE OnFileNew(IDMUSProdProject* pITargetProject,
										IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode);
    HRESULT STDMETHODCALLTYPE OnFileOpen(IStream* pIStream, IDMUSProdProject* pITargetProject,
										 IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode);
	HRESULT STDMETHODCALLTYPE OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode );
	HRESULT STDMETHODCALLTYPE IsFileTypeExtension( FileType ftFileType, BSTR bstrExt );

    // IDMUSProdDocType8 functions
    HRESULT STDMETHODCALLTYPE GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc );
    HRESULT STDMETHODCALLTYPE GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType );
    HRESULT STDMETHODCALLTYPE GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt );

private:
    CPersonalityDocType();

private:
    DWORD			m_dwRef;
	CPersonalityComponent* m_pComponent;
};

#endif // __PERSONALITYDOCTYPE_H__
