//////////////////////////////////////////////////////////////////////
//
// InstrumentPropPgMgr.h
//
//////////////////////////////////////////////////////////////////////

#ifndef INSTRUMENTPROPPGMGR_H
#define INSTRUMENTPROPPGMGR_H

#include "DllBasePropPageManager.h"

class CInstrumentPropPg;

//////////////////////////////////////////////////////////////////////
//  CInstrumentPropPgMgr

class CInstrumentPropPgMgr : public CDllBasePropPageManager 
{
	friend class CInstrument;

public:
	CInstrumentPropPgMgr();
	virtual ~CInstrumentPropPgMgr();

    // IJazzPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );

    HRESULT STDMETHODCALLTYPE RefreshData();

	// Member variables
private:
	
	CInstrumentPropPg*	m_pInstrumentPage;
};

#endif // #ifndef INSTRUMENTPROPPGMGR_H