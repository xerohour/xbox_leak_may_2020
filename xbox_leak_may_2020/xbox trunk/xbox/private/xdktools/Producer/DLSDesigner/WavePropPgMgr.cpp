//////////////////////////////////////////////////////////////////////
//
// WavePropPgMgr.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WavePropPgMgr.h"
#include "WavePropPg.h"
#include "WaveNode.h"
#include "Wave.h"
#include "Collection.h"
#include "WaveInfoPropPg.h"
#include "WaveCompressionPropPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DWORD CWavePropPgMgr::dwLastSelPage = 0;

//////////////////////////////////////////////////////////////////////
// CWavePropPgMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWavePropPgMgr::CWavePropPgMgr() : 
m_pWavePage(NULL), 
m_pWaveInfoPage(NULL),
m_pWaveCompressionPage(NULL)
{
}

CWavePropPgMgr::~CWavePropPgMgr()
{
	if(m_pWavePage)
		delete m_pWavePage;

	if(m_pWaveInfoPage)
		delete m_pWaveInfoPage;

	if(m_pWaveCompressionPage)
		delete m_pWaveCompressionPage;
}

/////////////////////////////////////////////////////////////////////////////
// CWavePropPgMgr IJazzPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CWavePropPgMgr IJazzPropPageManager::GetPropertySheetTitle

HRESULT CWavePropPgMgr::GetPropertySheetTitle(BSTR* pbstrTitle, BOOL* pfAddPropertiesText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pbstrTitle);
	ASSERT(pfAddPropertiesText);

	*pfAddPropertiesText = TRUE;

	CWave* pWave;

	if(m_pIPropPageObject && (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pWave))))
	{
		BSTR bstrWaveName;
		CWaveNode* pWaveNode = pWave->GetNode();
		ASSERT(pWaveNode);
		pWaveNode->GetNodeName(&bstrWaveName);
		CString sWaveName = bstrWaveName;
		SysFreeString(bstrWaveName);

		BSTR bstrCollectionName;
		CCollection* pCollection = pWave->GetCollection();
		CString sCollectionName = "";
		if(pCollection)
		{
			pCollection->GetNodeName(&bstrCollectionName);
			sCollectionName = bstrCollectionName;
			SysFreeString(bstrCollectionName);
			sCollectionName += " - ";
		}

		CString sTitle;
		sTitle.Format(IDS_WAVE_PROPPAGE_TITLE, sCollectionName, sWaveName);
		*pbstrTitle = sTitle.AllocSysString();
	}
	else
	{
		CString strTitle;
		strTitle.LoadString(IDS_WAVE_TEXT);
		*pbstrTitle = strTitle.AllocSysString();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CWavePropPgMgr IJazzPropPageManager::GetPropertySheetPages

HRESULT CWavePropPgMgr::GetPropertySheetPages(IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages)
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

    if (m_pWavePage == NULL)
        m_pWavePage = new CWavePropPg();

	if(m_pWavePage)
	{
		hPage = ::CreatePropertySheetPage((LPPROPSHEETPAGE)&m_pWavePage->m_psp);
		if(hPage)
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	if(m_pWaveInfoPage == NULL)
    	m_pWaveInfoPage = new CWaveInfoPropPg();

	if(m_pWaveInfoPage)
	{
		hPage = ::CreatePropertySheetPage((LPPROPSHEETPAGE)&m_pWaveInfoPage->m_psp);
		if(hPage)
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	if(m_pWaveCompressionPage == NULL)
		m_pWaveCompressionPage = new CWaveCompressionPropPage;

	if(m_pWaveCompressionPage)
	{
		hPage = ::CreatePropertySheetPage((LPPROPSHEETPAGE)&m_pWaveCompressionPage->m_psp);
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
// CWavePropPgMgr IJazzPropPageManager::RefreshData

HRESULT CWavePropPgMgr::RefreshData(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWave* pWave = NULL;
	
	if(m_pIPropPageObject == NULL)
	{
		pWave = NULL;
	}
	else if(FAILED(m_pIPropPageObject->GetData((void **)&pWave)))
	{
		return E_FAIL;
	}

	if(m_pWavePage == NULL || m_pWaveInfoPage == NULL || m_pWaveCompressionPage == NULL)
		return E_FAIL;

	m_pWavePage->SetObject(pWave);
	m_pWavePage->SetPropMgr(this);
    m_pWaveInfoPage->SetObject(pWave);
	m_pWaveCompressionPage->SetObject(pWave);

	return S_OK;
}

void CWavePropPgMgr::ShowPropSheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	SetObject(m_pIPropPageObject);
}