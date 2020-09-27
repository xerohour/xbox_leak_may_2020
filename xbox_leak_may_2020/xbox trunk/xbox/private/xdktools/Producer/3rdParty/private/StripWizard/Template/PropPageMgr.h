// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

#include <staticproppagemanager.h>

class PropPage$$Safe_root$$;

class C$$Safe_root$$PropPageMgr : public CStaticPropPageManager
{
	friend PropPage$$Safe_root$$;
public:
	C$$Safe_root$$PropPageMgr();
	~C$$Safe_root$$PropPageMgr();

	// IDMUSProdPropPageManager methods
	HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
	HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE RefreshData();

private:
	PropPage$$Safe_root$$*			m_pPropPage$$Safe_root$$;
};

#endif // __PROPPAGEMGR_H_
