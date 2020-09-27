#ifndef __STYLECOMPONENT_H__
#define __STYLECOMPONENT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// StyleComponent.h : header file
//

#include <afxtempl.h>
#include <Conductor.h>

class CDirectMusicStyle;

class CStyleComponent : public IDMUSProdComponent, public IDMUSProdPortNotify, public IDMUSProdRIFFExt, public IAllocVarChoices
{
public:
    CStyleComponent();
	~CStyleComponent();

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

	// IDMUSProdPortNotify functions
	HRESULT STDMETHODCALLTYPE OnOutputPortsChanged( void );
	HRESULT STDMETHODCALLTYPE OnOutputPortsRemoved( void );

    // IDMUSProdRIFFExt functions
    HRESULT STDMETHODCALLTYPE LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode );

	// IAllocVarChoices functions
	HRESULT	STDMETHODCALLTYPE GetVarChoicesNode( IUnknown** ppIVarChoicesNode );

    // Additional functions
private:
    HRESULT STDMETHODCALLTYPE AddNodeImageLists();
	BOOL RegisterClipboardFormats();
	void ReleaseAll();

public:
    HRESULT STDMETHODCALLTYPE GetStyleImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetFolderImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetMotifImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetPatternImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetStyleRefImageIndex( short* pnNbrFirstImage );
	void AddToStyleFileList( CDirectMusicStyle* pStyle );
	void RemoveFromStyleFileList( CDirectMusicStyle* pStyle );

public:
	IDMUSProdFramework*			m_pIFramework;
	IDirectMusicPerformance8*	m_pIDMPerformance;
	IDMUSProdConductor*			m_pIConductor;
	IDMUSProdComponent*			m_pIBandComponent;
	short						m_nNextStyle;		// appended to name of new Style
	UINT						m_cfProducerFile;	// CF_DMUSPROD_FILE clipboard format
	UINT						m_cfStyle;			// CF_STYLE clipboard format
	UINT						m_cfBand;			// CF_BAND clipboard format
	UINT						m_cfBandList;		// CF_BANDLIST clipboard format
	UINT						m_cfBandTrack;		// CF_BANDTRACK clipboard format
	UINT						m_cfTimeline;		// CF_TIMELINE clipboard format
	UINT						m_cfMotif;			// CF_MOTIF clipboard format
	UINT						m_cfMotifList;		// CF_MOTIFLIST clipboard format
	UINT						m_cfPattern;		// CF_PATTERN clipboard format
	UINT						m_cfPatternList;	// CF_PATTERNLIST clipboard format
	UINT						m_cfVarChoices;		// CF_VARCHOICES clipboard format

private:
    DWORD						m_dwRef;
	IDMUSProdDocType8*			m_pIStyleDocType8;
	short						m_nFirstImage;

	CTypedPtrList<CPtrList, CDirectMusicStyle*> m_lstStyles;
};

#endif // __STYLECOMPONENT_H__
