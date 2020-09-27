// EffectPPGMgr.h: interface for the CEffectPPGMgr class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __EFFECTPPGMGR_H__
#define __EFFECTPPGMGR_H__

#include <DMUSProd.h>
#include "DllBasePropPageManager.h"

// {665D665F-5959-4c7f-9D65-5D6732CC0EED}
static const GUID GUID_EffectPPGMgr = 
{ 0x665d665f, 0x5959, 0x4c7f, { 0x9d, 0x65, 0x5d, 0x67, 0x32, 0xcc, 0xe, 0xed } };


class CTabEffectInfo;

//////////////////////////////////////////////////////////////////////
//  CEffectPPGMgr

class CEffectPPGMgr : public CDllBasePropPageManager 
{
public:
	CEffectPPGMgr();
	virtual ~CEffectPPGMgr();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
	HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Member variables
private:
	CTabEffectInfo	*m_pTabEffectInfo;

public:
	static short	sm_nActiveTab;
};

#endif //__EFFECTPPGMGR_H__
