// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

//#include <staticproppagemanager.h>
#include <StaticPropPageManager.h>

class PropPagePersRef;

class CPersRefPropPageMgr : CStaticPropPageManager
{
	friend PropPagePersRef;
public:
	CPersRefPropPageMgr(IDMUSProdFramework* pIFramework);
	~CPersRefPropPageMgr();

	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
 
	// IDMUSProdPropPageManager methods
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Additional methods
	void UpdateObjectWithPersRefData();

private:
	IDMUSProdFramework*		m_pIFramework;
	PropPagePersRef*			m_pPropPagePersRef;
};

#endif // __PROPPAGEMGR_H_