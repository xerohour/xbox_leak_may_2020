// NotePropPageMgr.h : Handles the property pages

#ifndef __NOTEPROPPAGEMGR_H_
#define __NOTEPROPPAGEMGR_H_

#include "PropPageNote.h"
#include <staticproppagemanager.h>


//////////////////////////////////////////////////////////////////////
//  CNotePropPageMgr

class CNotePropPageMgr : public CStaticPropPageManager
{
friend class PropPageNote;

public:
	CNotePropPageMgr();
	~CNotePropPageMgr();

	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
								LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );

protected:
	PropPageNote*		m_pPropPageNote;

public:
	IDMUSProdFramework*	m_pDMUSProdFramework;
};


#endif // __PROPPAGEMGR_H_