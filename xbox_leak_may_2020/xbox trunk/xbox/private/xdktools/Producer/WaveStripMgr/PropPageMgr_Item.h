// PropPageMgr_Item.h : Handles the property pages for a track item

#ifndef __PROPPAGEMGR_ITEM_H_
#define __PROPPAGEMGR_ITEM_H_

#include <staticproppagemanager.h>

#define TAB_FILEREF		1
#define TAB_LOOP		2
#define TAB_PERFORMANCE	3
#define TAB_VARIATIONS	4

class CTabFileRef;
class CTabLoop;
class CTabPerformance;
class CTabVariations;

/////////////////////////////////////////////////////////////////////////////
// CPropPageMgrItem

class CPropPageMgrItem : public CStaticPropPageManager
{
	friend CTabFileRef;
	friend CTabLoop;
	friend CTabPerformance;
	friend CTabVariations;

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
	CTabLoop*			m_pTabLoop;
	CTabPerformance*	m_pTabPerformance;
	CTabVariations*		m_pTabVariations;

public:
	static short		sm_nActiveTab;
};

#endif // __PROPPAGEMGR_ITEM_H_
