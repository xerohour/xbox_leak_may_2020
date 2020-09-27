// PropPageMgr_Item.h : Handles the property pages for a track item

#ifndef __PROPPAGEMGR_ITEM_H_
#define __PROPPAGEMGR_ITEM_H_

#include <staticproppagemanager.h>


class CTabFileRef;
class CTabBoundary;

/////////////////////////////////////////////////////////////////////////////
// CPropPageMgrItem

class CPropPageMgrItem : public CStaticPropPageManager
{
	friend CTabFileRef;
	friend CTabBoundary;

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

	CTabFileRef*		m_pTabFileRef;
	CTabBoundary*		m_pTabBoundary;

public:
	static short		sm_nActiveTab;
};

#endif // __PROPPAGEMGR_ITEM_H_
