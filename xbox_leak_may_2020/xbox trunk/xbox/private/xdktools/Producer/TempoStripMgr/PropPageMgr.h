// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

#include <staticproppagemanager.h>

class PropPageTempo;

class CTempoPropPageMgr : CStaticPropPageManager
{
	friend PropPageTempo;
public:
	CTempoPropPageMgr();
	~CTempoPropPageMgr();

	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );

	// IDMUSProdPropPageManager methods
	HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
	HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE RefreshData();

private:
	PropPageTempo*			m_pPropPageTempo;
};

#endif // __PROPPAGEMGR_H_