// SignPostPPGMgr.h: interface for the CSignPostPPGMgr class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SIGNPOSTPPGMGR_H__
#define __SIGNPOSTPPGMGR_H__

// {9E2864D1-EB88-11d0-BAD3-00805F493F43}
static const GUID GUID_SignPostPPGMgr = 
{ 0x9e2864d1, 0xeb88, 0x11d0, { 0xba, 0xd3, 0x0, 0x80, 0x5f, 0x49, 0x3f, 0x43 } };

#include "StaticPropPageManager.h"
#include "SignPostPPG.h"

class CSignPostPPG;

//////////////////////////////////////////////////////////////////////
//  CSignPostPPGMgr

class CSignPostPPGMgr : public CStaticPropPageManager 
{
friend class CSignPostPPG;
public:
	CSignPostPPGMgr();
	virtual ~CSignPostPPGMgr();

    // IDMUSProdPropPageManager functions
	HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Member variables
private:
	CSignPostPPG				*m_pSignPostPPG;
};

#endif //__SIGNPOSTPPGMGR_H__
