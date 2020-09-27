// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

#include <staticproppagemanager.h>

class PropPageMarker;

class CMarkerPropPageMgr : public CStaticPropPageManager
{
	friend PropPageMarker;
public:
	CMarkerPropPageMgr();
	~CMarkerPropPageMgr();

	// IDMUSProdPropPageManager methods
	HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
	HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE RefreshData();

private:
	PropPageMarker*			m_pPropPageMarker;
};

#endif // __PROPPAGEMGR_H_
