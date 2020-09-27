/************************************************************************
*                                                                       *
*   Copyright (c) 1997-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// BasePropPageManager.h: interface for the CBasePropPageManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BASEPROPPAGEMANAGER_H__
#define __BASEPROPPAGEMANAGER_H__

#include "DMUSProd.h"

//////////////////////////////////////////////////////////////////////
//  CBasePropPageManager

class CBasePropPageManager : public IDMUSProdPropPageManager 
{
public:
	CBasePropPageManager();
	virtual ~CBasePropPageManager();

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

	//Additional functions
protected:
	void RemoveCurrentObject();

	// Member variables
protected:
	IDMUSProdPropSheet*			m_pIPropSheet;
	IDMUSProdPropPageObject*	m_pIPropPageObject;
	GUID						m_GUIDManager;
private:
	DWORD						m_dwRef;
};

#endif //__BASEPROPPAGEMANAGER_H__
