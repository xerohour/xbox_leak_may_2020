// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

#include "..\shared\PropPageChord.h"
#include "..\includes\staticproppagemanager.h"

class CChordPropPageMgr : CStaticPropPageManager
{
	friend class PropPageChord;
public:
	CChordPropPageMgr();
	~CChordPropPageMgr();
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();
	PropPageChord*		m_pPropPageChord;
	IDMUSProdFramework*		m_pJazzFramework;
};

#endif // __PROPPAGEMGR_H_