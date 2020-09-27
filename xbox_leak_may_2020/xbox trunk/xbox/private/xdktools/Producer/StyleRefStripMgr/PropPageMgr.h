// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

//#include <staticproppagemanager.h>
#include <StaticPropPageManager.h>

class PropPageStyleRef;
class CStyleRefMgr;

class CStyleRefPropPageMgr : CStaticPropPageManager
{
	friend PropPageStyleRef;
public:
	CStyleRefPropPageMgr(IDMUSProdFramework* pIFramework, CStyleRefMgr* pStyleRefMgr);
	~CStyleRefPropPageMgr();

	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
 
	// IDMUSProdPropPageManager methods
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Additional methods
	void UpdateObjectWithStyleRefData();

private:
	IDMUSProdFramework*		m_pIFramework;
	CStyleRefMgr*			m_pStyleRefMgr;
	PropPageStyleRef*		m_pPropPageStyleRef;
};

#endif // __PROPPAGEMGR_H_