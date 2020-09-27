//////////////////////////////////////////////////////////////////////
//
// CollectionPropPgMgr.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CollectionPropPgMgr.h"
#include "CollectionPropPg.h"
#include "CollectionExtraPropPg.h"
#include "Collection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


DWORD CCollectionPropPgMgr::m_dwLastSelPage = 0;

//////////////////////////////////////////////////////////////////////
// CCollectionPropPgMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollectionPropPgMgr::CCollectionPropPgMgr() : m_pCollectionPage(NULL), m_pCollectionExtraPage(NULL)
{
}

CCollectionPropPgMgr::~CCollectionPropPgMgr()
{
	if(m_pCollectionPage)
		delete m_pCollectionPage;

	if(m_pCollectionExtraPage)
		delete m_pCollectionExtraPage;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPgMgr IJazzPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPgMgr IJazzPropPageManager::GetPropertySheetTitle

HRESULT CCollectionPropPgMgr::GetPropertySheetTitle(BSTR* pbstrTitle, BOOL* pfAddPropertiesText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pbstrTitle);
	ASSERT(pfAddPropertiesText);

	*pfAddPropertiesText = TRUE;

	CCollection* pCollection;

	if(m_pIPropPageObject && (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pCollection))))
	{
		BSTR bstrCollectionName;
		pCollection->GetNodeName(&bstrCollectionName);
		CString sCollectionName = bstrCollectionName;
		SysFreeString(bstrCollectionName);

		CString sTitle;
		sTitle.LoadString(IDS_COLLECTION_TEXT);
		sTitle = sCollectionName + " " + sTitle;
		*pbstrTitle = sTitle.AllocSysString();
	}
	else
	{
		CString strTitle;
		strTitle.LoadString(IDS_COLLECTION_TEXT);
		*pbstrTitle = strTitle.AllocSysString();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPgMgr IJazzPropPageManager::GetPropertySheetPages

HRESULT CCollectionPropPgMgr::GetPropertySheetPages(IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	// Add General tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	if(m_pCollectionPage == NULL)
		m_pCollectionPage = new CCollectionPropPg();

	if(m_pCollectionPage)
	{
		hPage = ::CreatePropertySheetPage((LPPROPSHEETPAGE)&m_pCollectionPage->m_psp);
		if(hPage)
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	if(m_pCollectionExtraPage == NULL)
		m_pCollectionExtraPage = new CCollectionExtraPropPg();

	if(m_pCollectionExtraPage)
	{
		hPage = ::CreatePropertySheetPage((LPPROPSHEETPAGE)&m_pCollectionExtraPage->m_psp);
		if(hPage)
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Set number of pages
	*pnNbrPages = nNbrPages; 

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPgMgr IJazzPropPageManager::RefreshData

HRESULT CCollectionPropPgMgr::RefreshData(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCollection* pCollection = NULL;
	
	if(m_pIPropPageObject == NULL)
	{
		pCollection = NULL;
	}
	else if(FAILED(m_pIPropPageObject->GetData((void **)&pCollection)))
	{
		return E_FAIL;
	}

	m_pCollectionPage->SetObject(pCollection);
	m_pCollectionExtraPage->SetObject(pCollection);

	if(m_pIPropSheet)
		m_pIPropSheet->RefreshTitle();

	
	return S_OK;
}
