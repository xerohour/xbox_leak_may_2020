//////////////////////////////////////////////////////////////////////
//
// RegionPropPgMgr.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegionPropPgMgr.h"
#include "RegionPropPg.h"
#include "RegionExtraPropPg.h"
#include "Collection.h"
#include "Instrument.h"
#include "Region.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Last selected page
DWORD CRegionPropPgMgr::dwLastSelPage = 0;

//////////////////////////////////////////////////////////////////////
// CRegionPropPgMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegionPropPgMgr::CRegionPropPgMgr() : m_pRegionPage(NULL), m_pRegionExtraPage(NULL)
{
}

CRegionPropPgMgr::~CRegionPropPgMgr()
{
	if(m_pRegionPage)
		delete m_pRegionPage;

	if(m_pRegionExtraPage)
		delete m_pRegionExtraPage;
}

/////////////////////////////////////////////////////////////////////////////
// CRegionPropPgMgr IJazzPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CRegionPropPgMgr IJazzPropPageManager::GetPropertySheetTitle

HRESULT CRegionPropPgMgr::GetPropertySheetTitle(BSTR* pbstrTitle, BOOL* pfAddPropertiesText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pbstrTitle);
	ASSERT(pfAddPropertiesText);

	*pfAddPropertiesText = TRUE;

	CRegion* pRegion;

	if(m_pIPropPageObject && (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pRegion))))
	{
		BSTR bstrCollectionName;
		pRegion->m_pInstrument->m_pCollection->GetNodeName(&bstrCollectionName);
		CString sCollectionName = bstrCollectionName;
		SysFreeString(bstrCollectionName);

		CString sInstrumentName;
		pRegion->m_pInstrument->GetName(sInstrumentName);
		if(sInstrumentName.IsEmpty())
			sInstrumentName.LoadString(IDS_INSTRUMENT_TEXT);
		
		BSTR bstrRegionName;
		pRegion->GetNodeName(&bstrRegionName);
		CString sRegionName = bstrRegionName;
		SysFreeString(bstrRegionName);

		CString sTitle;
		sTitle.Format(IDS_REGION_PROPPAGE_TITLE, sCollectionName, sInstrumentName, sRegionName);
		*pbstrTitle = sTitle.AllocSysString();
	}
	else
	{
		CString strTitle;
		strTitle.LoadString(IDS_REGION_TEXT);
		*pbstrTitle = strTitle.AllocSysString();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CRegionPropPgMgr IJazzPropPageManager::GetPropertySheetPages

HRESULT CRegionPropPgMgr::GetPropertySheetPages(IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages)
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

	if(m_pRegionPage == NULL)
    	m_pRegionPage = new CRegionPropPg();

	if(m_pRegionPage)
	{
		hPage = ::CreatePropertySheetPage((LPPROPSHEETPAGE)&m_pRegionPage->m_psp);
		if(hPage)
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	if(m_pRegionExtraPage == NULL)
    	m_pRegionExtraPage = new CRegionExtraPropPg();
	if(m_pRegionExtraPage)
	{
		hPage = ::CreatePropertySheetPage((LPPROPSHEETPAGE)&m_pRegionExtraPage->m_psp);
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
// CRegionPropPgMgr IJazzPropPageManager::RefreshData

HRESULT CRegionPropPgMgr::RefreshData(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Don't do anything if there are no pages yet...
	if(m_pRegionPage == NULL || m_pRegionExtraPage == NULL)
		return E_FAIL;
	
	CRegion* pRegion = NULL;
	
	if(m_pIPropPageObject == NULL)
	{
		pRegion = NULL;
	}
	else if(FAILED(m_pIPropPageObject->GetData((void **)&pRegion)))
	{
		return E_FAIL;
	}

	m_pRegionPage->SetObject(pRegion);
	m_pRegionExtraPage->SetObject(pRegion);
	m_pRegionExtraPage->SetPropMgr(this);
	
	if(m_pIPropSheet)
		m_pIPropSheet->RefreshTitle();
	return S_OK;
}

void CRegionPropPgMgr::ShowPropSheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	SetObject(m_pIPropPageObject);
}
