#ifndef __PERSONALITYCOMPONENT_H__
#define __PERSONALITYCOMPONENT_H__

// PersonalityComponent.h : header file
//

#include <afxtempl.h>

#define	FIRST_PERSONALITY_IMAGE		2
#define FIRST_PERSONALITYREF_IMAGE	4

class CPersonality;

class CPersonalityComponent : public IDMUSProdComponent, public IDMUSProdRIFFExt
{
friend class CPersonality;
public:
    CPersonalityComponent();
	~CPersonalityComponent();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdComponent functions
    HRESULT STDMETHODCALLTYPE Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg );
    HRESULT STDMETHODCALLTYPE CleanUp( void );
    HRESULT STDMETHODCALLTYPE GetName( BSTR* pbstrName );
	HRESULT STDMETHODCALLTYPE AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode );
	HRESULT STDMETHODCALLTYPE OnActivateApp(BOOL fActivate);

    // IDMUSProdRIFFExt functions
    HRESULT STDMETHODCALLTYPE LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode );

    // Additional functions
private:
    HRESULT STDMETHODCALLTYPE AddNodeImageLists( void );
	BOOL RegisterClipboardFormats();
	void ReleaseAll();

public:
    HRESULT STDMETHODCALLTYPE GetPersonalityImageIndex( short* pnFirstImage );
    HRESULT STDMETHODCALLTYPE GetFolderImageIndex( short* pnFirstImage );
    HRESULT STDMETHODCALLTYPE GetPersonalityRefImageIndex( short* pnNbrFirstImage );
	void ScalePatternToScaleName( long lScalePattern, CString& strScaleName );
	void AddToPersonalityFileList( CPersonality* pPersonality );
	void RemoveFromPersonalityFileList( CPersonality* pPersonality );
	char DetermineRoot( DWORD dwScale );

	HRESULT STDMETHODCALLTYPE GetConductor(IDMUSProdConductor**);
private:
	BOOL	m_bDirty;

public:
	IDirectMusicPerformance*	m_pIEngine;
	IDMUSProdFramework* m_pIFramework;
	short				m_nNextPersonality;	// appended to name of new Personalitys
	UINT				m_cfProducerFile;	// CF_DMUSPROD_FILE clipboard format
	UINT				m_cfStyle;			// CF_STYLE clipboard format
	UINT				m_cfPersonality;	// CF_PERSONALITY clipboard format

private:
    DWORD				m_dwRef;
	IDMUSProdDocType8*  m_pIDocType8;
	IDMUSProdConductor* m_pIConductor;
	short				m_nFirstPersonalityImage;

	CTypedPtrList<CPtrList, CPersonality*> m_lstPersonalities;
};

#endif // __PERSONALITYCOMPONENT_H__
