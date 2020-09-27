#ifndef __TRANSITIONPROPPAGEOBJECT_H_
#define __TRANSITIONPROPPAGEOBJECT_H_


typedef struct _PPGTransition
{
	_PPGTransition( void )
	{
		dwPageIndex = 0;
		pSong = NULL;
		pVirtualSegmentToEdit = NULL;
		pTransition = NULL;
	}

	DWORD				dwPageIndex;
	CDirectMusicSong*	pSong;
	CVirtualSegment*	pVirtualSegmentToEdit;
	CTransition*		pTransition;
} PPGTransition;


class CTabTransition;

//////////////////////////////////////////////////////////////////////
//  CTransitionPropPageManager

class CTransitionPropPageManager : public IDMUSProdPropPageManager 
{
friend class CTabBoundaryFlags;

public:
	CTransitionPropPageManager();
	virtual ~CTransitionPropPageManager();

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
	
	CTabTransition*				m_pTabTransition;
	CTabBoundaryFlags*			m_pTabBoundary;

public:
	static short				sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CTransitionPropPageObject

class CTransitionPropPageObject : public IDMUSProdPropPageObject
{
friend class CTransitionPropPageManager;

public:
	CTransitionPropPageObject( CSongDlg* pSongDlg );
	virtual ~CTransitionPropPageObject();

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

	CTransition*		m_pTransition;
	DWORD				m_dwFlagsUI;
};

#endif // __TRANSITIONPROPPAGEOBJECT_H_
