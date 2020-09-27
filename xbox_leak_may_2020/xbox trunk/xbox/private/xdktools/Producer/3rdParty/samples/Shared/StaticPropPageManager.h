/************************************************************************
*                                                                       *
*   Copyright (c) 1997-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// StaticPropPageManager.h: interface for the CStaticPropPageManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __STATICPROPPAGEMANAGER_H__
#define __STATICPROPPAGEMANAGER_H__

#include "BasePropPageManager.h"

//////////////////////////////////////////////////////////////////////
//  CStaticPropPageManager

class CStaticPropPageManager : public CBasePropPageManager 
{
public:
	CStaticPropPageManager();
	virtual ~CStaticPropPageManager();

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

#endif //__STATICPROPPAGEMANAGER_H__
