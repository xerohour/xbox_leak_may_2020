// CommandPPGMgr.cpp: implementation of the CCommandPPGMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommandStripMgr.h"
#include "FileIO.h"
#include "CommandPPGMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CCommandPPGMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommandPPGMgr::CCommandPPGMgr() : CStaticPropPageManager()
{
	m_pCommandPPG	= NULL;
	m_GUIDManager	= GUID_CommandPPGMgr;
}

CCommandPPGMgr::~CCommandPPGMgr()
{
	if( m_pCommandPPG )
	{
		delete m_pCommandPPG;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCommandPPGMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CCommandPPGMgr IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CCommandPPGMgr::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(( pbstrTitle == NULL )
	|| ( pfAddPropertiesText == NULL ))
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;

	strTitle.LoadString( IDS_PROPPAGE_COMMAND );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCommandPPGMgr IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CCommandPPGMgr::GetPropertySheetPages( IDMUSProdPropSheet *pIPropSheet, LONG *hPropSheetPage[], short *pnNbrPages )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(( hPropSheetPage == NULL )
	|| ( pnNbrPages == NULL ))
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	HPROPSHEETPAGE hPage;

	// If the property page has already been created, get a handle for it.
	if( m_pCommandPPG )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pCommandPPG->m_psp );
		if( hPage )
		{
			hPropSheetPage[0] = (long *) hPage;
			*pnNbrPages = 1;
			return S_OK;
		}
		*pnNbrPages = 0;
		delete m_pCommandPPG;
		m_pCommandPPG = NULL;
		return E_OUTOFMEMORY;
	}

	// Otherwise, create a new property page
	m_pCommandPPG = new CCommandPPG();
	if( m_pCommandPPG )
	{
		m_pCommandPPG->m_pPageManager = this;
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pCommandPPG->m_psp );
		if( hPage )
		{
			hPropSheetPage[0] = (long *) hPage;
			*pnNbrPages = 1;
			return S_OK;
		}
		delete m_pCommandPPG;
		m_pCommandPPG = NULL;
	}

	// We couldn't create the page
	*pnNbrPages = 0;
	return E_OUTOFMEMORY;
}

/////////////////////////////////////////////////////////////////////////////
// CCommandPPGMgr IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CCommandPPGMgr::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;

	hr = CBasePropPageManager::OnRemoveFromPropertySheet();
	if( FAILED( hr ))
	{
		return hr;
	}

	if( m_pIPropSheet )
	{
		m_pIPropSheet->Release();
		m_pIPropSheet = NULL;
	}

	if( m_pCommandPPG )
	{
		hr = m_pCommandPPG->SetData( NULL );
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CCommandPPGMgr IDMUSProdPropPageManager::RefreshData

HRESULT CCommandPPGMgr::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT		hr = S_OK;
	PPGCommand*	pCommand = NULL;
	
	if( m_pIPropPageObject != NULL )
	{
		hr = m_pIPropPageObject->GetData( (void **) &pCommand );
		if( FAILED( hr ))
		{
			return hr;
		}
	}

	if( m_pCommandPPG )
	{
		hr = m_pCommandPPG->SetData( pCommand );
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CCommandPPGMgr IDMUSProdPropPageManager::RemoveObject

HRESULT CCommandPPGMgr::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	HRESULT hr;

	hr = CStaticPropPageManager::RemoveObject( pIPropPageObject );
	if( SUCCEEDED( hr ))
	{
		if( m_pCommandPPG )
		{
			hr = m_pCommandPPG->SetData( NULL );
		}
	}
	return hr;
}
