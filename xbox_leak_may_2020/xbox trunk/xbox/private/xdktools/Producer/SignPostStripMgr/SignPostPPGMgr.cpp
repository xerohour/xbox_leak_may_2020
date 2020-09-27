// SignPostPPGMgr.cpp: implementation of the CSignPostPPGMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SignPostStripMgr.h"
#include "SignPostIO.h"
#include "SignPostPPGMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CSignPostPPGMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSignPostPPGMgr::CSignPostPPGMgr() : CStaticPropPageManager()
{
	m_pSignPostPPG	= NULL;
	m_GUIDManager	= GUID_SignPostPPGMgr;
}

CSignPostPPGMgr::~CSignPostPPGMgr()
{
	if(m_pSignPostPPG)
	{
		delete m_pSignPostPPG;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostPPGMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CSignPostPPGMgr IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CSignPostPPGMgr::GetPropertySheetTitle(BSTR* pbstrTitle, BOOL* pfAddPropertiesText)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if((pbstrTitle == NULL)
	|| (pfAddPropertiesText == NULL))
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;

	strTitle.LoadString(IDS_PROPPAGE_SIGNPOST);
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSignPostPPGMgr IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CSignPostPPGMgr::GetPropertySheetPages(IDMUSProdPropSheet *pIPropSheet, LONG *hPropSheetPage[], short *pnNbrPages)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if((hPropSheetPage == NULL)
	|| (pnNbrPages == NULL))
	{
		return E_POINTER;
	}

	if(pIPropSheet == NULL)
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	HPROPSHEETPAGE hPage;

	// If the property page has already been created, get a handle for it.
	if(m_pSignPostPPG)
	{
		hPage = ::CreatePropertySheetPage((LPCPROPSHEETPAGE)&m_pSignPostPPG->m_psp);
		if(hPage)
		{
			hPropSheetPage[0] = (long *) hPage;
			*pnNbrPages = 1;
			return S_OK;
		}
		*pnNbrPages = 0;
		delete m_pSignPostPPG;
		m_pSignPostPPG = NULL;
		return E_OUTOFMEMORY;
	}

	// Otherwise, create a new property page.
	m_pSignPostPPG = new CSignPostPPG();
	if(m_pSignPostPPG)
	{
		m_pSignPostPPG->m_pPageManager = this;
		hPage = ::CreatePropertySheetPage((LPCPROPSHEETPAGE)&m_pSignPostPPG->m_psp);
		if(hPage)
		{
			hPropSheetPage[0] = (long *) hPage;
			*pnNbrPages = 1;
			return S_OK;
		}
		delete m_pSignPostPPG;
		m_pSignPostPPG = NULL;
	}

	// We couldn't create the page
	*pnNbrPages = 0;
	return E_OUTOFMEMORY;
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostPPGMgr IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CSignPostPPGMgr::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;

	hr = CBasePropPageManager::OnRemoveFromPropertySheet();
	if(FAILED(hr))
	{
		return hr;
	}

	if(m_pIPropSheet)
	{
		m_pIPropSheet->Release();
		m_pIPropSheet = NULL;
	}

	if(m_pSignPostPPG)
	{
		hr = m_pSignPostPPG->SetData(NULL);
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostPPGMgr IDMUSProdPropPageManager::RefreshData

HRESULT CSignPostPPGMgr::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT		hr = S_OK;
	PPGSignPost	*pSignPost = NULL;
	
	if(m_pIPropPageObject != NULL)
	{
		hr = m_pIPropPageObject->GetData((void **)&pSignPost);
		if(FAILED(hr))
		{
			return hr;
		}
	}

	if(m_pSignPostPPG)
	{
		hr = m_pSignPostPPG->SetData(pSignPost);
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostPPGMgr IDMUSProdPropPageManager::RemoveObject

HRESULT CSignPostPPGMgr::RemoveObject( IDMUSProdPropPageObject *pIPropPageObject )
{
	HRESULT hr;

	hr = CStaticPropPageManager::RemoveObject(pIPropPageObject);
	if(SUCCEEDED(hr))
	{
		if(m_pSignPostPPG)
		{
			hr = m_pSignPostPPG->SetData(NULL);
		}
	}
	return hr;
}
