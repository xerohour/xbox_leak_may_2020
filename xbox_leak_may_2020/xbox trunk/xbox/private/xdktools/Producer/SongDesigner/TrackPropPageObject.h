#ifndef __TRAKCPROPPAGEOBJECT_H_
#define __TRAKCPROPPAGEOBJECT_H_


typedef struct _PPGTrack
{
	_PPGTrack( void )
	{
		dwPageIndex = 0;
		pSong = NULL;
		pTrack = NULL;
	}

	DWORD				dwPageIndex;
	CDirectMusicSong*	pSong;
	CTrack*				pTrack;
} PPGTrack;


//////////////////////////////////////////////////////////////////////
//  CTrackPropPageManager

class CTrackPropPageManager : public IDMUSProdPropPageManager 
{
friend class CTrackFlagsPPG;

public:
	CTrackPropPageManager();
	virtual ~CTrackPropPageManager();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();

    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
    HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject );

    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE IsEqualPageManagerGUID( REFGUID rguidPageManager );

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:
    DWORD						m_dwRef;
	IDMUSProdPropSheet*			m_pIPropSheet;
	IDMUSProdPropPageObject*	m_pIPropPageObject;
	
	CTrackFlagsPPG*				m_pTabFlags;

public:
	static short				sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CTrackPropPageObject

class CTrackPropPageObject : public IDMUSProdPropPageObject
{
friend class CTrackPropPageManager;

public:
	CTrackPropPageObject( CSongDlg* pSongDlg );
	virtual ~CTrackPropPageObject();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

	// Additional methods

private:
    DWORD				m_dwRef;
	CSongDlg*			m_pSongDlg;

	CTrack*				m_pTrack;
	DWORD				m_dwFlagsUI;
};

#endif // __TRAKCPROPPAGEOBJECT_H_
