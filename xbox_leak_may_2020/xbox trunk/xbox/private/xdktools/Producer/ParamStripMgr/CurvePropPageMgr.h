// PropPageMgr.h : Handles the property pages

#ifndef __CURVEPROPPAGEMGR_H_
#define __CURVEPROPPAGEMGR_H_


#include <staticproppagemanager.h>
#include "PropCurve.h"


//////////////////////////////////////////////////////////////////////
//  CCurvePropPageMgr

class CCurvePropPageMgr : public CStaticPropPageManager 
{
friend class CPropPageCurve;

public:
	CCurvePropPageMgr(IDMUSProdFramework* pIFramework, CParamStrip* pParamStrip);
	virtual ~CCurvePropPageMgr();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:

	CPropPageCurve*		m_pTabCurve;
	CParamStrip*		m_pParamStrip;
	CTrackMgr*			m_pTrackMgr;	// Track manager
	GUID				m_GUIDManager;
	IDMUSProdFramework* m_pIFramework;

public:
	static short		sm_nActiveTab;
};

#endif // __CURVEPROPPAGEMGR_H_