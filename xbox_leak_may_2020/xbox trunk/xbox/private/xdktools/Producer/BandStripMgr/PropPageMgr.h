// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

#include <staticproppagemanager.h>

class PropPageBand;
class CBandMgr;

class CBandPropPageMgr : CStaticPropPageManager
{
	friend PropPageBand;
public:
	CBandPropPageMgr( CBandMgr* pBandMgr );
	~CBandPropPageMgr();

	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );

	// IDMUSProdPropPageManager methods
	HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
	HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE RefreshData();

	// Additional methods
	void UpdateObjectWithBandData();

private:
	PropPageBand*	m_pPropPageBand;
	CBandMgr*		m_pBandMgr;
};

#endif // __PROPPAGEMGR_H_