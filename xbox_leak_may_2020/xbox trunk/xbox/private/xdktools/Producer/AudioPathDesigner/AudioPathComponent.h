#ifndef __AUDIOPATHCOMPONENT_H__
#define __AUDIOPATHCOMPONENT_H__

// AudioPathComponent.h : header file
//

#include <Conductor.h>

class CDirectMusicAudioPath;
interface IDirectMusicPerformance8;
interface IDirectMusic;
class EffectInfo;
struct PortOptions;

class CAudioPathComponent : public IDMUSProdComponent, public IDMUSProdRIFFExt
{
friend class CDlgNewAudiopath;
friend class CTabEffectInfo;
public:
    CAudioPathComponent();
	~CAudioPathComponent();

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
    HRESULT STDMETHODCALLTYPE AddNodeImageLists();
	BOOL RegisterClipboardFormats();
	void ReleaseAll();

public:
    HRESULT STDMETHODCALLTYPE GetAudioPathImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetAudioPathRefImageIndex( short* pnNbrFirstImage );
	void AddToAudioPathFileList( CDirectMusicAudioPath* pAudioPath );
	void RemoveFromAudioPathFileList( CDirectMusicAudioPath* pAudioPath );

public:
	IDMUSProdFramework*			m_pIFramework;
	IDirectMusicPerformance8*	m_pIDMPerformance;
	IDirectMusic*				m_pIDMusic;
	IDMUSProdConductor8*		m_pIConductor;
	IDMUSProdComponent*			m_pIToolGraphComponent;
	short						m_nNextAudioPath;		// appended to name of new AudioPath
	UINT						m_cfProducerFile;		// CF_DMUSPROD_FILE clipboard format
	UINT						m_cfAudioPath;			// CF_AUDIOPATH clipboard format
	UINT						m_cfAudioPathList;		// CF_AUDIOPATHLIST clipboard format
	UINT						m_cfGraph;				// CF_GRAPH clipboard format
	IDMUSProdPropPageManager	*m_pIAudioPathPageManager;
	IDMUSProdPropPageManager	*m_pIEffectPageManager;
	IDMUSProdPropPageManager	*m_pIMixGroupPageManager;
	IDMUSProdPropPageManager	*m_pIBufferPageManager;

private:
    DWORD						m_dwRef;
	IDMUSProdDocType8*			m_pIAudioPathDocType8;
	short						m_nFirstImage;

	CTypedPtrList<CPtrList, CDirectMusicAudioPath*> m_lstAudioPaths;
};

#endif // __AUDIOPATHCOMPONENT_H__
