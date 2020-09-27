// CommandPPGMgr.h: interface for the CCommandPPGMgr class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SIGNPOSTPPGMGR_H__
#define __SIGNPOSTPPGMGR_H__

// {33EEC071-F31B-11d0-BADE-00805F493F43}
static const GUID GUID_CommandPPGMgr = 
{ 0x33eec071, 0xf31b, 0x11d0, { 0xba, 0xde, 0x0, 0x80, 0x5f, 0x49, 0x3f, 0x43 } };

#include "StaticPropPageManager.h"
#include "CommandPPG.h"

class CCommandPPG;

//////////////////////////////////////////////////////////////////////
//  CCommandPPGMgr

class CCommandPPGMgr : public CStaticPropPageManager 
{
friend class CCommandPPG;
public:
	CCommandPPGMgr();
	virtual ~CCommandPPGMgr();

    // IDMUSProdPropPageManager functions
	HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Member variables
private:
	CCommandPPG*	m_pCommandPPG;
};

#endif //__SIGNPOSTPPGMGR_H__
