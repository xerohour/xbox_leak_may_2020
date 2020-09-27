#ifndef __SEGMENTDOCTYPE_H__
#define __SEGMENTDOCTYPE_H__

// SegmentDocType.h : header file
//

#include <DMUSProd.h>
#include <PrivateDocType.h>

class CSegmentDocType : public IDMUSProdDocType8, public IDMUSProdDocTypeP, public ISegmentDocTypeWaveVarImport
{
public:
    CSegmentDocType( class CSegmentComponent* pComponent );
	~CSegmentDocType();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // ISegmentDocType functions
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

	// IDMUSProdDocTypeP functions
    HRESULT STDMETHODCALLTYPE ImportNode( IUnknown* punkNode, IUnknown* punkTreePositionNode, IUnknown** ppIDocRootNode );

	// ISegmentDocTypeWaveVarImport functions
    HRESULT STDMETHODCALLTYPE ImportWaveNodes( DWORD dwNodeCount, IDMUSProdNode** apunkNode, IUnknown* punkTreePositionNode, IUnknown** ppIDocRootNode );

	// Additional methods
	HRESULT CreateSegmentFromWave( IDMUSProdNode* pIWaveNode, IDMUSProdNode* pITreePositionNode, IUnknown** ppISegmentNode );
	HRESULT CreateSegmentFromWaves( DWORD dwCount, IDMUSProdNode** apIWaveNode, IDMUSProdNode* pITreePositionNode, IUnknown** ppISegmentNode );
	REFERENCE_TIME GetWaveLength( IDMUSProdNode* pIWaveNode );

private:
    CSegmentDocType();

private:
    DWORD			m_dwRef;
	CSegmentComponent* m_pComponent;
};

#endif // __SEGMENTDOCTYPE_H__
