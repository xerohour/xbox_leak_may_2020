// CurvePropPageMgr.h : Handles the property pages

#ifndef __CURVEPROPPAGEMGR_H_
#define __CURVEPROPPAGEMGR_H_

#include "PropCurve.h"
#include <staticproppagemanager.h>

//////////////////////////////////////////////////////////////////////
//  CCurvePropPageMgr

class CCurvePropPageMgr : public CStaticPropPageManager 
{
friend class CPropPageCurve;
friend class CPropPageCurveReset;

public:
	CCurvePropPageMgr( REFGUID rguidManager );
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
	CPropCurve			m_PropCurve;	// Used to relay changes to CurveStrip

	CPropPageCurve*		m_pTabCurve;
	CPropPageCurveReset* m_pTabCurveReset;

public:
	static short		sm_nActiveTab;
};


#endif // __CURVEPROPPAGEMGR_H_