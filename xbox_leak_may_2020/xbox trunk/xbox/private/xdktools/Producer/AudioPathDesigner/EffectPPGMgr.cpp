// EffectPPGMgr.cpp: implementation of the CEffectPPGMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectPPGMgr.h"
#include "TabEffectInfo.h"
#include "EffectInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

short CEffectPPGMgr::sm_nActiveTab = 0;

//////////////////////////////////////////////////////////////////////
// CEffectPPGMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectPPGMgr::CEffectPPGMgr( ) : CDllBasePropPageManager()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_GUIDManager	= GUID_EffectPPGMgr;

	m_pTabEffectInfo= NULL;
}

CEffectPPGMgr::~CEffectPPGMgr()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pTabEffectInfo)
	{
		delete m_pTabEffectInfo;
		m_pTabEffectInfo = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEffectPPGMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CEffectPPGMgr IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CEffectPPGMgr::GetPropertySheetTitle(BSTR* pbstrTitle, BOOL* pfAddPropertiesText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if((pbstrTitle == NULL)
	|| (pfAddPropertiesText == NULL))
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_EFFECT_TEXT );

	EffectInfoForPPG effectInfoForPPG;

	// Nothing is constructed in the call to GetData, so we don't need to worry
	// about cleaning up any memory
	if(m_pIPropPageObject
	&& SUCCEEDED(m_pIPropPageObject->GetData((void **)&effectInfoForPPG)) )
	{
		if( effectInfoForPPG.m_fValid
		&&	!effectInfoForPPG.m_strInstanceName.IsEmpty() )
		{
			strTitle = effectInfoForPPG.m_strInstanceName + _T(" ") + strTitle;
		}
		if( !effectInfoForPPG.m_strAudioPathName.IsEmpty() )
		{
			strTitle = effectInfoForPPG.m_strAudioPathName + _T(" - ") + strTitle;
		}
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CEffectPPGMgr IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CEffectPPGMgr::GetPropertySheetPages(IDMUSProdPropSheet *pIPropSheet, LONG *hPropSheetPage[], short *pnNbrPages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	// Add Segment tab
	if( m_pTabEffectInfo == NULL )
	{
		m_pTabEffectInfo = new CTabEffectInfo();
	}
	if(m_pTabEffectInfo)
	{
		hPage = ::CreatePropertySheetPage((LPCPROPSHEETPAGE)&m_pTabEffectInfo->m_psp);
		if(hPage)
		{
			hPropSheetPage[nNbrPages] = (LONG *) hPage;
			nNbrPages++;
		}
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CEffectPPGMgr IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CEffectPPGMgr::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	if(m_pIPropSheet)
	{
		m_pIPropSheet->GetActivePage( &CEffectPPGMgr::sm_nActiveTab );
	}

	HRESULT hr = CBasePropPageManager::OnRemoveFromPropertySheet();

	if(m_pIPropSheet)
	{
		m_pIPropSheet->Release();
		m_pIPropSheet = NULL;
	}

	RefreshData();

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CEffectPPGMgr IDMUSProdPropPageManager::RefreshData

HRESULT CEffectPPGMgr::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pTabEffectInfo == NULL)
	{
		return S_OK;
	}

	// Try and get the information for the selected effect
	EffectInfoForPPG effectInfoForPPG;
	if( (m_pIPropPageObject == NULL)
	||	FAILED(m_pIPropPageObject->GetData((void **)&effectInfoForPPG)) )
	{
		m_pTabEffectInfo->SetEffect( NULL, NULL );

		return S_OK;
	}

	// Make sure changes to current effect are processed in OnKillFocus
	// messages before setting the new effect
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabEffectInfo->GetSafeHwnd() ? m_pTabEffectInfo->GetParent() : NULL;

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	m_pTabEffectInfo->SetEffect( &effectInfoForPPG, m_pIPropPageObject );

	// Restore focus
	if( pWndHadFocus
	&&	pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CEffectPPGMgr IDMUSProdPropPageManager::SetObject

HRESULT CEffectPPGMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}
	if( m_pIPropPageObject == pINewPropPageObject )
	{
		return S_OK;
	}

	RemoveCurrentObject();

	m_pIPropPageObject = pINewPropPageObject;
//	m_pIPropPageObject->AddRef();		intentionally missing

	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CEffectPPGMgr IDMUSProdPropPageManager::RemoveObject

HRESULT CEffectPPGMgr::RemoveObject( IDMUSProdPropPageObject *pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = CDllBasePropPageManager::RemoveObject(pIPropPageObject);
	if(SUCCEEDED(hr))
	{
		if(m_pTabEffectInfo)
		{
			m_pTabEffectInfo->SetEffect(NULL,NULL);
		}
	}

	return hr;
}
