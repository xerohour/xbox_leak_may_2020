//////////////////////////////////////////////////////////////////////
//
// CollectionPropPgMgr.h
//
//////////////////////////////////////////////////////////////////////

#ifndef COLLECTIONPROPPGMGR_H
#define COLLECTIONPROPPGMGR_H

#include "DllBasePropPageManager.h"

#define COLLECTION_PROP_PAGE			0
#define COLLECTION_EXTRA_PAGE			1

class CCollectionPropPg;
class CCollectionExtraPropPg;

//////////////////////////////////////////////////////////////////////
//  CollectionPropPgMgr

class CCollectionPropPgMgr : public CDllBasePropPageManager 
{
	friend class CCollection;

public:
	CCollectionPropPgMgr();
	virtual ~CCollectionPropPgMgr();

    // IJazzPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();

public:
	
	// Keeps track of the last selected property sheet
	static DWORD m_dwLastSelPage;


	// Member variables
private:
	
	CCollectionPropPg*		m_pCollectionPage;
	CCollectionExtraPropPg*	m_pCollectionExtraPage;
};

#endif // #ifndef COLLECTIONPROPPGMGR_H