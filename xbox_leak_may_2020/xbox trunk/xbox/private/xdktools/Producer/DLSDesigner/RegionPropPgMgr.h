//////////////////////////////////////////////////////////////////////
//
// RegionPropPgMgr.h
//
//////////////////////////////////////////////////////////////////////

#ifndef REGIONPROPPGMGR_H
#define REGIONPROPPGMGR_H

#include "DllBasePropPageManager.h"
//#include "instrumentfveditor.h"

class CRegionPropPg;
class CRegionExtraPropPg;
class CInstrumentFVEditor;

#define REGION_PROP_PAGE		0		
#define REGION_EXTRA_PROP_PAGE	1

//////////////////////////////////////////////////////////////////////
//  CRegionPropPgMgr

class CRegionPropPgMgr : public CDllBasePropPageManager 
{
    friend class CInstrumentFVEditor;
public:
	void ShowPropSheet();
	CRegionPropPgMgr();
	virtual ~CRegionPropPgMgr();

    // IJazzPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );

    HRESULT STDMETHODCALLTYPE RefreshData();

public:
	static DWORD dwLastSelPage;

	// Member variables
private:
	CRegionPropPg*			m_pRegionPage;
	CRegionExtraPropPg*		m_pRegionExtraPage;
};

#endif // #ifndef REGIONPROPPGMGR_H