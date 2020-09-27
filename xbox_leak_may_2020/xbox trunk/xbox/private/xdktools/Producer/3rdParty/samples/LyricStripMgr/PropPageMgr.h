/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// PropPageMgr.h : Handles the property pages

#ifndef __PROPPAGEMGR_H_
#define __PROPPAGEMGR_H_

#include <staticproppagemanager.h>

class PropPageLyric;
class CLyricMgr;

class CLyricPropPageMgr : public CStaticPropPageManager
{
	friend PropPageLyric;
public:
	CLyricPropPageMgr( CLyricMgr* pLyricMgr );
	~CLyricPropPageMgr();

	// IDMUSProdPropPageManager methods
	HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
	HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
		LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE RefreshData();

private:
	PropPageLyric*		m_pPropPageLyric;
	CLyricMgr*			m_pLyricMgr;
};

#endif // __PROPPAGEMGR_H_