// NotePropPageMgr.cpp : implementation file
//

#include "stdafx.h"
#include "NotePropPageMgr.h"
#include "PropPageNote.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNotePropPageMgr property page

CNotePropPageMgr::CNotePropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pPropPageNote = NULL;
	m_pDMUSProdFramework = NULL;
	CStaticPropPageManager::CStaticPropPageManager();
}

CNotePropPageMgr::~CNotePropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageNote )
	{
		delete m_pPropPageNote;
	}
	/*if( m_pDMUSProdFramework )
	{
		m_pDMUSProdFramework->Release();
	}*/
	CStaticPropPageManager::~CStaticPropPageManager();
}

HRESULT STDMETHODCALLTYPE CNotePropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};

HRESULT STDMETHODCALLTYPE CNotePropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
	BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;

	strTitle.LoadString( IDS_PROPPAGE_SEQUENCE );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
};

HRESULT STDMETHODCALLTYPE CNotePropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
	LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
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

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	// Add Note tab
	if( NULL == m_pPropPageNote )
	{
		m_pPropPageNote = new PropPageNote( this );
	}
	if( m_pPropPageNote )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pPropPageNote->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
		if (!m_pDMUSProdFramework)
		{
			pIPropSheet->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pDMUSProdFramework );
			// This is another work-around to get rid of a cyclical dependency.
			// I really need to take some time and think about a better way
			// to fix them.  -jdooley
			m_pDMUSProdFramework->Release();
		}
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};

HRESULT STDMETHODCALLTYPE CNotePropPageMgr::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RemoveCurrentObject();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->Release();
		m_pIPropSheet = NULL;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CNotePropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CPropSequence* pPropSequence;
	if( m_pIPropPageObject == NULL )
	{
		pPropSequence = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pPropSequence ) ) )
	{
		return E_FAIL;
	}
	if( pPropSequence )
	{
		m_pPropPageNote->m_fValidPropNote = TRUE;
		m_pPropPageNote->SetNote( pPropSequence );
	}
	else if( m_pPropPageNote->m_fValidPropNote )
	{
		m_pPropPageNote->m_fValidPropNote = FALSE;
		m_pPropPageNote->m_PropNote.Clear();
		m_pPropPageNote->SetNote( &m_pPropPageNote->m_PropNote );
	}
	return S_OK;
};

HRESULT CNotePropPageMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageNote )
	{
		m_pPropPageNote->SetObject( pINewPropPageObject );
	}
	HRESULT hr = CBasePropPageManager::SetObject( pINewPropPageObject );
	// Update the property page
	RefreshData();
	return hr;
}
