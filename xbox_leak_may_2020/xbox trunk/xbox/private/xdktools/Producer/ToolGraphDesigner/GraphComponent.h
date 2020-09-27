#ifndef __GRAPHCOMPONENT_H__
#define __GRAPHCOMPONENT_H__

// GraphComponent.h : header file
//

#include <afxtempl.h>
#include <Conductor.h>

class CDirectMusicGraph;


#pragma pack(2)

typedef struct RegisteredTool
{
	RegisteredTool()
	{
		memset( &clsidTool, 0, sizeof(CLSID) );
	}

	CLSID				clsidTool;
	CString				strName;
} RegisteredTool;

#pragma pack()


////////////////////////////////////////////////////////////////////////////////
class CGraphComponent : public IDMUSProdComponent, public IDMUSProdRIFFExt
{
public:
    CGraphComponent();
	~CGraphComponent();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdComponent functions
    HRESULT STDMETHODCALLTYPE Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg );
    HRESULT STDMETHODCALLTYPE CleanUp( void );
    HRESULT STDMETHODCALLTYPE GetName( BSTR* pbstrName );
	HRESULT STDMETHODCALLTYPE AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode );
	HRESULT STDMETHODCALLTYPE OnActivateApp( BOOL fActivate );

    // IDMUSProdRIFFExt functions
    HRESULT STDMETHODCALLTYPE LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode );

    // Additional functions
private:
    HRESULT AddNodeImageLists();
    HRESULT LoadRegisteredTools();
	BOOL RegisterClipboardFormats();
	void ReleaseAll();

public:
    HRESULT STDMETHODCALLTYPE GetGraphImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetGraphRefImageIndex( short* pnNbrFirstImage );
	RegisteredTool* EnumRegisteredTools( int nIndex );
	void GetRegisteredToolName( CLSID clsidTool, CString& strName );
	BOOL IsRegisteredTool( CLSID clsidTool );
	void AddToGraphFileList( CDirectMusicGraph* pGraph );
	void RemoveFromGraphFileList( CDirectMusicGraph* pGraph );

public:
	IDMUSProdFramework*			m_pIFramework;
	IDirectMusicPerformance*	m_pIDMPerformance;
	IDMUSProdConductor*			m_pIConductor;
	IDMUSProdDocType8*			m_pIGraphDocType8;

	short						m_nNextGraph;		// appended to name of new Graph
	UINT						m_cfProducerFile;	// CF_DMUSPROD_FILE clipboard format
	UINT						m_cfGraph;			// CF_GRAPH clipboard format
	UINT						m_cfGraphList;		// CF_GRAPHLIST clipboard format

private:
    DWORD						m_dwRef;
	short						m_nFirstImage;

	CTypedPtrList<CPtrList, CDirectMusicGraph*> m_lstGraphs;
	CTypedPtrList<CPtrList, RegisteredTool*> m_lstRegisteredTools;
};

#endif // __GRAPHCOMPONENT_H__
