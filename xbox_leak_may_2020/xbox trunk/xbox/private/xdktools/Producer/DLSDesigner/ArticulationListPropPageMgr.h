//////////////////////////////////////////////////////////////////////
//
// ArticulationListPropPageMgr.h
//
//////////////////////////////////////////////////////////////////////

#ifndef __ARTICULATIONLISTPROPPAGEMGR_H
#define __ARTICULATIONLISTPROPPAGEMGR_H

#include "DllBasePropPageManager.h"

class CArticulationListPropPage;

//////////////////////////////////////////////////////////////////////
//  ArticulationListPropPageMgr

class CArticulationListPropPageMgr : public CDllBasePropPageManager 
{

public:
	CArticulationListPropPageMgr();
	virtual ~CArticulationListPropPageMgr();

    // IJazzPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Member variables
private:
	
	CArticulationListPropPage*	m_pArtListPage;
};

#endif // #ifndef __ARTICULATIONLISTPROPPAGEMGR_H