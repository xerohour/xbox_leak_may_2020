// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

#include <StaticPropPageManager.h>

class PropPageTimeSig;

class CTimeSigPropPageMgr : CStaticPropPageManager
{
	friend PropPageTimeSig;
public:
	CTimeSigPropPageMgr(IDMUSProdFramework* pIFramework);
	~CTimeSigPropPageMgr();

	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
 
	// IDMUSProdPropPageManager methods
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Additional methods
	void UpdateObjectWithTimeSigData();

private:
	IDMUSProdFramework*		m_pIFramework;
	PropPageTimeSig*			m_pPropPageTimeSig;
};

#endif // __PROPPAGEMGR_H_