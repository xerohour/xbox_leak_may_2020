// PropPageMgr_Item.h : Handles the property pages for a Routine

#ifndef __PROPPAGEMGR_ITEM_H_
#define __PROPPAGEMGR_ITEM_H_

#include <staticproppagemanager.h>


class CTabRoutine;

/////////////////////////////////////////////////////////////////////////////
// CPropPageMgrItem

class CPropPageMgrItem : public CStaticPropPageManager
{
	friend CTabRoutine;
public:
	CPropPageMgrItem( IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr );
	~CPropPageMgrItem();

	// IDMUSProdPropPageManager methods
	HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
	HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE RefreshData();

private:
	IDMUSProdFramework*	m_pIFramework;
	CTrackMgr*			m_pTrackMgr;

	CTabRoutine*		m_pTabRoutine;
};

#endif // __PROPPAGEMGR_ITEM_H_
