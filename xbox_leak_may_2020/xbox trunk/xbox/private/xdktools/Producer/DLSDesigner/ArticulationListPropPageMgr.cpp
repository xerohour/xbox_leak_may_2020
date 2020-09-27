//////////////////////////////////////////////////////////////////////
//
// ArticulationListPropPageMgr.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Collection.h"
#include "ArticulationListPropPage.h"
#include "ArticulationListPropPageMgr.h"
#include "ArticulationList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CArticulationListPropPageMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArticulationListPropPageMgr::CArticulationListPropPageMgr() : m_pArtListPage(NULL)
{
}

CArticulationListPropPageMgr::~CArticulationListPropPageMgr()
{
	if(m_pArtListPage)
	{
		delete m_pArtListPage;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationListPropPageMgr IJazzPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CArticulationListPropPageMgr IJazzPropPageManager::GetPropertySheetTitle

HRESULT CArticulationListPropPageMgr::GetPropertySheetTitle(BSTR* pbstrTitle, BOOL* pfAddPropertiesText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pbstrTitle);
	ASSERT(pfAddPropertiesText);

	*pfAddPropertiesText = TRUE;

	CArticulationList* pArtList = NULL;

	if(m_pIPropPageObject && (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pArtList))))
	{
		BSTR bstrArtListName;
		pArtList->GetNodeName(&bstrArtListName);
		CString sArtListName = bstrArtListName;
		SysFreeString(bstrArtListName);

		CString sTitle;
		sTitle.LoadString(IDS_ARTICULATION_FOLDER_NAME);
		sTitle = sArtListName + " " + sTitle;
		*pbstrTitle = sTitle.AllocSysString();
	}
	else
	{
		CString strTitle;
		strTitle.LoadString(IDS_ARTICULATION_FOLDER_NAME);
		*pbstrTitle = strTitle.AllocSysString();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CArticulationListPropPageMgr IJazzPropPageManager::GetPropertySheetPages

HRESULT CArticulationListPropPageMgr::GetPropertySheetPages(IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages)
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

	if(m_pArtListPage == NULL)
	{
		m_pArtListPage = new CArticulationListPropPage();
	}

	if(m_pArtListPage)
	{
		hPage = ::CreatePropertySheetPage((LPPROPSHEETPAGE)&m_pArtListPage->m_psp);
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
// CArticulationListPropPageMgr IJazzPropPageManager::RefreshData

HRESULT CArticulationListPropPageMgr::RefreshData(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CArticulationList* pArtList = NULL;
	
	if(m_pIPropPageObject == NULL)
	{
		pArtList = NULL;
	}
	else if(FAILED(m_pIPropPageObject->GetData((void **)&pArtList)))
	{
		return E_FAIL;
	}

	m_pArtListPage->SetObject(pArtList);

	if(m_pIPropSheet)
	{
		m_pIPropSheet->RefreshTitle();
	}

	
	return S_OK;
}
