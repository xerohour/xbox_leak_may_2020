// ConnectionPropPageMgr.h : Handles the property pages

#ifndef __CONNECTIONPROPPAGEMGR_H_
#define __CONNECTIONPROPPAGEMGR_H_

//#include "connectionproppage.h"
#include "staticproppagemanager.h"

struct ConnectionData
{
	short nWeight;
	short nMinBeats;
	short nMaxBeats;
	BOOL  fVariableNotFixed;
	ConnectionData() { nWeight = nMinBeats = nMaxBeats = 0; fVariableNotFixed = FALSE;}
};

class CConnectionPropPageMgr : CStaticPropPageManager
{
	friend class CConnectionPropPage;
public:
	HRESULT STDMETHODCALLTYPE UpdateObjectWithTabData();
	CConnectionPropPageMgr();
	~CConnectionPropPageMgr();
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
    HRESULT STDMETHODCALLTYPE RefreshData();
	CConnectionPropPage*		m_pPropPageConnection;
	IDMUSProdFramework*			m_pJazzFramework;
};

#endif // __PROPPAGEMGR_H_