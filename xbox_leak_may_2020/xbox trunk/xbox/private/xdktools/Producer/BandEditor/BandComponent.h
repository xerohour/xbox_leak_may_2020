#ifndef __BANDCOMPONENT_H__
#define __BANDCOMPONENT_H__

// BandComponent.h : header file
//

#define MSB_MASK (0x007F0000)
#define LSB_MASK (0x00007F00)

#define MAKE_PATCH(DRUM, MSB, LSB, Patch) ( ((DWORD) ((DRUM) == 0 ? 0 : 0x80000000))  | ((DWORD)(BYTE)(MSB) << 16) | ((DWORD)(LSB) << 8) | (DWORD)(Patch))
#define MSB(dwPatch) (((dwPatch) & MSB_MASK) >> 16)
#define LSB(dwPatch) (((dwPatch) & LSB_MASK) >> 8)

#include <afxtempl.h>
#include "dmusici.h"

class CBand;

class CBandComponent : public IDMUSProdComponent, public IDMUSProdRIFFExt
{
public:
    CBandComponent();
	~CBandComponent();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdComponent functions
    HRESULT STDMETHODCALLTYPE Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg );
    HRESULT STDMETHODCALLTYPE CleanUp( void );
    HRESULT STDMETHODCALLTYPE GetName( BSTR* pbstrName );
	HRESULT STDMETHODCALLTYPE AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode );
    HRESULT STDMETHODCALLTYPE OnActivateApp( BOOL fActivateApp );

    // IDMUSProdRIFFExt functions
    HRESULT STDMETHODCALLTYPE LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode );

    // Additional functions
private:
    HRESULT STDMETHODCALLTYPE AddNodeImageLists( void );
	BOOL RegisterClipboardFormats();
	void ReleaseAll();

public:
    HRESULT STDMETHODCALLTYPE	GetBandImageIndex( short* pnFirstImage );
    HRESULT STDMETHODCALLTYPE	GetFolderImageIndex( short* pnFirstImage );
    HRESULT STDMETHODCALLTYPE	GetBandRefImageIndex( short* pnFirstImage );

	void						AddToBandFileList( CBand* pBand );
	void						RemoveFromBandFileList( CBand* pBand );

	IDMUSProdReferenceNode*		FindDLSCollection( CString csCollection, IStream* pIStream );
	IDMUSProdReferenceNode*		CreateCollectionRefNode( IDMUSProdNode* pDLSCollectionNode );
	bool						IsGM( DWORD dwBank, DWORD dwInstrument );

	HRESULT						SendPMsg( DMUS_PMSG *pPMsg, IDirectMusicAudioPath* pDMAudioPath = NULL );
	
	BOOL						IsInFailedCollectionList(CString csCollection);

public:
	interface IDirectMusicPerformance*	m_pIDMPerformance;
	IDMUSProdFramework*					m_pIFramework;
	IDMUSProdConductor*					m_pIConductor;
	CList<CString, CString&>			m_lstLastCollection;

	void*				m_pvLastFailedCollectionRefStream;
	short				m_nNextBand;		// Appended to name of new Band
	UINT				m_cfBand;			// CF_BAND clipboard format
	UINT				m_cfBandList;		// CF_BANDLIST clipboard format
	UINT				m_cfProducerFile;	// CF_DMUSPROD_FILE clipboard format

private:
	IDMUSProdDocType8*  m_pIDocType8;
    DWORD				m_dwRef;
	short				m_nFirstBandImage;
	short				m_nFirstBandRefImage;
	short				m_nFirstFolderImage;

	CTypedPtrList<CPtrList, CBand*> m_lstBands;


};

#endif // __BANDCOMPONENT_H__
