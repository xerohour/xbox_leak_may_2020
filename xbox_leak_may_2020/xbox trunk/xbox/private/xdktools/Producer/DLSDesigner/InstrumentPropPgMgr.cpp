//////////////////////////////////////////////////////////////////////
//
// InstrumentPropPgMgr.cpp: implementation of the CInstrumentPropPgMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InstrumentPropPgMgr.h"
#include "InstrumentPropPg.h"
#include "Collection.h"
#include "Instrument.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInstrumentPropPgMgr::CInstrumentPropPgMgr() : m_pInstrumentPage(NULL)
{

}

CInstrumentPropPgMgr::~CInstrumentPropPgMgr()
{
	if(m_pInstrumentPage)
		delete m_pInstrumentPage;

	m_pInstrumentPage = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPgMgr IJazzPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPgMgr IJazzPropPageManager::GetPropertySheetTitle

HRESULT CInstrumentPropPgMgr::GetPropertySheetTitle(BSTR* pbstrTitle, BOOL* pfAddPropertiesText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pbstrTitle);
	ASSERT(pfAddPropertiesText);

	*pfAddPropertiesText = TRUE;

	CInstrument* pInstrument;

	if(m_pIPropPageObject && (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pInstrument))))
	{
		pInstrument->GetName();
		CString sInstrumentName = pInstrument->m_csName; 
		
		BSTR bstrCollectionName;
		pInstrument->m_pCollection->GetNodeName(&bstrCollectionName);
		CString sCollectionName = bstrCollectionName;
		SysFreeString(bstrCollectionName);

		CString sTitle;
		sTitle.Format(IDS_INSTRUMENT_PROPPAGE_TITLE, sCollectionName, sInstrumentName);
		*pbstrTitle = sTitle.AllocSysString();
	}
	else
	{
		CString strTitle;
		strTitle.LoadString(IDS_INSTRUMENT_TEXT);
		*pbstrTitle = strTitle.AllocSysString();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPgMgr IJazzPropPageManager::GetPropertySheetPages

HRESULT CInstrumentPropPgMgr::GetPropertySheetPages(IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages)
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

	if(m_pInstrumentPage == NULL)
		m_pInstrumentPage = new CInstrumentPropPg();

	if(m_pInstrumentPage)
	{
		hPage = ::CreatePropertySheetPage((LPPROPSHEETPAGE)&m_pInstrumentPage->m_psp);
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
// CInstrumentPropPgMgr IJazzPropPageManager::RefreshData

HRESULT CInstrumentPropPgMgr::RefreshData(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pInstrumentPage == NULL)
		return E_FAIL;
	
	CInstrument* pInstrument = NULL;
	
	if(m_pIPropPageObject == NULL)
	{
		pInstrument = NULL;
	}
	else
	{
		if(FAILED(m_pIPropPageObject->GetData((void **)&pInstrument)))
		{
			return E_FAIL;
		}
	}

	m_pInstrumentPage->SetObject(pInstrument);

	if(m_pIPropSheet)
		m_pIPropSheet->RefreshTitle();

	return S_OK;
}
