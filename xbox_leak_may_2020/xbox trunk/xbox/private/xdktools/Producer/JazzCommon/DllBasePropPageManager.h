// DllBasePropPageManager.h: interface for the CDllBasePropPageManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DLLBASEPROPPAGEMANAGER_H__
#define __DLLBASEPROPPAGEMANAGER_H__

#include "BasePropPageManager.h"

//////////////////////////////////////////////////////////////////////
//  CDllBasePropPageManager

class CDllBasePropPageManager : public CBasePropPageManager 
{
public:
	CDllBasePropPageManager();
	virtual ~CDllBasePropPageManager();

    // IUnknown functions
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdPropPageManager functions
    virtual HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText ) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages ) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();

    virtual HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
    virtual HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    virtual HRESULT STDMETHODCALLTYPE IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject );

    virtual HRESULT STDMETHODCALLTYPE RefreshData() = 0;
    virtual HRESULT STDMETHODCALLTYPE IsEqualPageManagerGUID( REFGUID rguidPageManager );
};

#endif //__DLLBASEPROPPAGEMANAGER_H__
