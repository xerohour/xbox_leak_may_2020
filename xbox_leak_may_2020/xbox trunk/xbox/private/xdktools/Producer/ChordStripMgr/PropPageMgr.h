// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

#include "ChordScalePropPage.h"
#include "ChordInversionPropPage.h"
#include "staticproppagemanager.h"

class CChordPropPageMgr : public  CStaticPropPageManager
{
	friend class CChordScalePropPage;
	friend class CChordInversionPropPage;
public:
	CChordPropPageMgr(IDMUSProdFramework* pIFramework );
	~CChordPropPageMgr();
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();
//	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet( void );

	// Additional methods
	void UpdateObjectWithTabData();

	CChordScalePropPage*		m_pChordScalePropPage;
	CChordInversionPropPage*	m_pChordInversionPropPage;
	IDMUSProdFramework*		m_pIFramework;

	static short					sm_nActiveTab;
};

#endif // __PROPPAGEMGR_H_