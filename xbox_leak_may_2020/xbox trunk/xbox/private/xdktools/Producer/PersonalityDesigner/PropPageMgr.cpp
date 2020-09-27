// CChordPropPageMgr : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "..\includes\DMUSProd.h"
#include "..\includes\Conductor.h"
#include "PropChord.h"
#include "PropPageMgr.h"
#include "ChordScalePropPage.h"
#include "ChordInversionPropPage.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordPropPageMgr property page

short CChordPropPageMgr::sm_nActiveTab = 0;

HRESULT STDMETHODCALLTYPE CChordPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};


CChordPropPageMgr::CChordPropPageMgr( IDMUSProdFramework* pIFramework )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( pIFramework != NULL );
	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	m_pChordScalePropPage = NULL;
	m_pChordInversionPropPage = NULL;
	CStaticPropPageManager::CStaticPropPageManager();
}

CChordPropPageMgr::~CChordPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pChordScalePropPage )
	{
		m_pChordScalePropPage->Release();
		m_pChordScalePropPage = 0;
	}
	if(m_pChordInversionPropPage)
	{
		m_pChordInversionPropPage->Release();
		m_pChordInversionPropPage = 0;
	}
	if ( m_pIFramework )
	{
		m_pIFramework->Release();
		m_pIFramework = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChordPropPageMgr IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CChordPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_PROPPAGE_CHORD );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CChordPropPageMgr IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CChordPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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


	// Add Chord/Scale tab
	if(m_pChordScalePropPage == NULL)
	{
		m_pChordScalePropPage = new CChordScalePropPage( );
	}
	ASSERT(m_pChordScalePropPage);
	if( m_pChordScalePropPage )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pChordScalePropPage->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		// fill in proppagemgr member
		m_pChordScalePropPage->m_pPropPageMgr = this;
		// fill in proppage's conductor member
		if(!m_pChordScalePropPage->m_pIConductor)
		{
			ASSERT(m_pIFramework);
			IDMUSProdComponent* pIComponent = NULL;
			IDMUSProdConductor* pConductor = NULL;
			if( SUCCEEDED ( m_pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
			{
				pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pConductor );
				if (pConductor)
				{
					m_pChordScalePropPage->m_pIConductor = pConductor;
				}
				pIComponent->Release();
			}
		}
	}


	// Add ChordInversion tab
	if(m_pChordInversionPropPage == NULL)
	{
		m_pChordInversionPropPage = new CChordInversionPropPage( );
	}
	ASSERT(m_pChordInversionPropPage);
	if( m_pChordInversionPropPage )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pChordInversionPropPage->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
		// fill in proppagemgr member
		m_pChordInversionPropPage->m_pPropPageMgr = this;
		if(!m_pChordInversionPropPage->m_pIConductor)
		{
			ASSERT(m_pIFramework);
			IDMUSProdComponent* pIComponent = NULL;
			IDMUSProdConductor* pConductor = NULL;
			if( SUCCEEDED ( m_pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
			{
				pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pConductor );
				if (pConductor)
				{
					m_pChordInversionPropPage->m_pIConductor = pConductor;
				}
				pIComponent->Release();
			}
		}
	}


	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}

/*
/////////////////////////////////////////////////////////////////////////////
// CChordPropPageMgr IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CChordPropPageMgr::OnRemoveFromPropertySheet( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CChordPropPageMgr::sm_nActiveTab );

	CDllBasePropPageManager::OnRemoveFromPropertySheet();

//	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}
*/

/////////////////////////////////////////////////////////////////////////////
// CChordPropPageMgr IDMUSProdPropPageManager::RefreshData

HRESULT CChordPropPageMgr::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pIPropPageObject ) 
	{
		CChordScalePropPageData chordscalePropPageData;
		CChordScalePropPageData* pChordScalePropPageData = &chordscalePropPageData;

		if( SUCCEEDED ( m_pIPropPageObject->GetData( (void **)&pChordScalePropPageData ) ) )
		{
			m_pChordScalePropPage->SetChordData( pChordScalePropPageData );
			m_pChordInversionPropPage->SetChordData( pChordScalePropPageData );
			return S_OK;
		}
	}

	return E_FAIL;
}
