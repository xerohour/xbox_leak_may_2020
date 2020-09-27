#ifndef __LOADER_H__
#define __LOADER_H__

// Loader.h : header file
//

#include <dmusici.h>

interface IDirectSoundWave;


// Private interface for wave track
DEFINE_GUID(IID_IPrivateWaveTrack, 0x492abe2a, 0x38c8, 0x48a3, 0x8f, 0x3c, 0x1e, 0x13, 0xba, 0x1, 0x78, 0x4e);
interface IPrivateWaveTrack : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SetVariation(
		IDirectMusicSegmentState* pSegState,
		DWORD dwVariationFlags,
		DWORD dwPChannel,
		DWORD dwIndex)=0;
    virtual HRESULT STDMETHODCALLTYPE ClearVariations(IDirectMusicSegmentState* pSegState)=0;
    virtual HRESULT STDMETHODCALLTYPE AddWave(
		IDirectSoundWave* pWave,
		REFERENCE_TIME rtTime,
		DWORD dwPChannel,
		DWORD dwIndex,
		REFERENCE_TIME* prtLength)=0;
    virtual HRESULT STDMETHODCALLTYPE DownloadWave(
		IDirectSoundWave* pWave,   // wave to download
		IUnknown* pUnk,            // performance or audio path
		REFGUID rguidVersion)=0;   // version of downloaded wave
    virtual HRESULT STDMETHODCALLTYPE UnloadWave(
		IDirectSoundWave* pWave,   // wave to unload
		IUnknown* pUnk)=0;         // performance or audio path
    virtual HRESULT STDMETHODCALLTYPE RefreshWave(
		IDirectSoundWave* pWave,   // wave to refresh
		IUnknown* pUnk,            // performance or audio path
		DWORD dwPChannel,          // new PChannel for the wave
		REFGUID rguidVersion)=0;;  // version of refreshed wave
};


//////////////////////////////////////////////////////////////////////////////
// CLoader class

class CLoader : public IDirectMusicLoader
{
public:
	CLoader();
	virtual ~CLoader();

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	// IDirectMusicLoader methods
	STDMETHOD( GetObject )( LPDMUS_OBJECTDESC pDesc, REFIID riid, LPVOID FAR * ppVoid );
	STDMETHOD( SetObject )( LPDMUS_OBJECTDESC pDesc );
    STDMETHOD( SetSearchDirectory )( REFGUID rguidClass, WCHAR* pwzPath, BOOL fClear );
	STDMETHOD( ScanDirectory )( REFGUID rguidClass, WCHAR* pwzFileExtension, WCHAR* pwzScanFileName );
	STDMETHOD( CacheObject )( IDirectMusicObject* pObject );
	STDMETHOD( ReleaseObject )( IDirectMusicObject* pObject );
	STDMETHOD( ClearCache )( REFGUID rguidClass );
	STDMETHOD( EnableCache )( REFGUID rguidClass, BOOL fEnable );
	STDMETHOD( EnumObject )( REFGUID rguidClass, DWORD dwIndex, LPDMUS_OBJECTDESC pDesc );

private:
	HRESULT CreateSegmentFromWave( IDMUSProdNode* pIWaveNode, IDirectMusicSegment8** ppIDMSegment8 );

private:
    ULONG           m_dwRef;				// object reference count
//	ULONG			m_dwPRef;				// private reference count.
	IUnknown*		m_pUnkDispatch;			// holds the controlling unknown of the scripting object that implements IDispatch
	IUnknown*		m_pIGMCollection;		// holds unknown for default GM collection
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __LOADER_H__
