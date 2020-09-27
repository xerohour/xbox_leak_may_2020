#include "stdafx.h"
#include "..\includes\DMUSProd.h"
#include "ChordMapStripMgr.h"
#include "ConnectionPropPage.h"
#include "ConnectionPropPageMgr.h"
#include "LockoutNotification.h"


// PropPageConnection.cpp : implementation file
//

#include "..\includes\Conductor.h"
#include "PropChord.h"
#include "PropPageMgr.h"
#include "PropPageChord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CConnectionPropPageMgr property page

CConnectionPropPageMgr::CConnectionPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pPropPageConnection = NULL;
	m_pJazzFramework = NULL;
	CStaticPropPageManager::CStaticPropPageManager();
}

CConnectionPropPageMgr::~CConnectionPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageConnection )
	{
		delete m_pPropPageConnection;
		m_pPropPageConnection = 0;
	}
	if ( m_pJazzFramework )
	{
		m_pJazzFramework->Release();
		m_pPropPageConnection = 0;
	}
	CStaticPropPageManager::~CStaticPropPageManager();
}

HRESULT STDMETHODCALLTYPE CConnectionPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};

HRESULT STDMETHODCALLTYPE CConnectionPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
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

	strTitle.LoadString( IDS_PROPPAGE_CONNECTION );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
};

HRESULT STDMETHODCALLTYPE CConnectionPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
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

	// Add Connection tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	if(!m_pPropPageConnection)
		m_pPropPageConnection = new CConnectionPropPage();
	if( m_pPropPageConnection )
	{

		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pPropPageConnection->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		m_pPropPageConnection->m_pPropPageMgr = this;
		if (!m_pJazzFramework)
		{
			pIPropSheet->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pJazzFramework );
		}
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};

HRESULT STDMETHODCALLTYPE CConnectionPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ConnectionData data;
	ConnectionData* pdata = &data;
	if( m_pIPropPageObject == NULL )
	{
		return E_FAIL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pdata ) ) )
	{
		return E_FAIL;
	}
	if (m_pPropPageConnection )
	{
		// Don't send OnKill/OnUpdate notifications when updating the display
		CLockoutNotification LockoutNotifications( m_pPropPageConnection->m_hWnd );

		m_pPropPageConnection->SetConnection( pdata->nWeight );
		// hack to cure race condition: if you set maxbeats first, test that it is >= minbeats
		// may fail as minbeats hasn't been updated yet
		short nMinBeats = pdata->nMinBeats;
		m_pPropPageConnection->SetMinBeats(0);
		m_pPropPageConnection->SetMaxBeats(pdata->nMaxBeats);
		m_pPropPageConnection->SetMinBeats(nMinBeats);
		m_pPropPageConnection->SetMinMaxEdit(pdata->fVariableNotFixed);
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
};


HRESULT STDMETHODCALLTYPE CConnectionPropPageMgr::UpdateObjectWithTabData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ConnectionData data;
	if( m_pIPropPageObject == NULL || m_pPropPageConnection == NULL)
	{
		return E_FAIL;
	}
	
	data.nWeight = m_pPropPageConnection->m_nWeight;
	data.nMinBeats = m_pPropPageConnection->m_nMinBeats;
	data.nMaxBeats = m_pPropPageConnection->m_nMaxBeats;

	HRESULT hr = m_pIPropPageObject->SetData( (void *)&data );
	return hr;
}
