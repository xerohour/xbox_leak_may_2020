#ifndef __VIRTUALSEGMENTPROPPAGEOBJECT_H_
#define __VIRTUALSEGMENTPROPPAGEOBJECT_H_

#include "TabVirtualSegment.h"
#include "TabVirtualSegmentLoop.h"

typedef struct _PPGVirtualSegment
{
	_PPGVirtualSegment( void )
	{
		dwPageIndex = 0;
		pSong = NULL;
		pVirtualSegment = NULL;
	}

	DWORD				dwPageIndex;
	CDirectMusicSong*	pSong;
	CVirtualSegment*	pVirtualSegment;
} PPGVirtualSegment;


//////////////////////////////////////////////////////////////////////
//  CVirtualSegmentPropPageManager

class CVirtualSegmentPropPageManager : public IDMUSProdPropPageManager 
{
friend class CTabVirtualSegment;
friend class CTabVirtualSegmentLoop;
friend class CTabBoundaryFlags;

public:
	CVirtualSegmentPropPageManager();
	virtual ~CVirtualSegmentPropPageManager();

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
	
	CTabVirtualSegment*			m_pTabSegment;
	CTabVirtualSegmentLoop*		m_pTabLoop;
	CTabBoundaryFlags*			m_pTabBoundary;

public:
	static short				sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CVirtualSegmentPropPageObject

class CVirtualSegmentPropPageObject : public IDMUSProdPropPageObject
{
friend class CVirtualSegmentPropPageManager;

public:
	CVirtualSegmentPropPageObject( CSongDlg* pSongDlg );
	virtual ~CVirtualSegmentPropPageObject();

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
	void InitPPGTabVirtualSegment( PPGTabVirtualSegment* pPPGTabVirtualSegment );
	void InitPPGTabVirtualSegmentLoop( PPGTabVirtualSegmentLoop* pPPGTabVirtualSegmentLoop );

private:
    DWORD				m_dwRef;
	CSongDlg*			m_pSongDlg;

	CVirtualSegment*	m_pVirtualSegment;
	DWORD				m_dwFlagsUI;
};

#endif // __VIRTUALSEGMENTPROPPAGEOBJECT_H_
