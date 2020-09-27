// MixGroupPPGMgr.cpp : implementation file
//

#include "stdafx.h"
#include "MixGroupPPGMgr.h"
#include "TabMixGroup.h"
#include "ItemInfo.h"
#include "AudioPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

short CMixGroupPPGMgr::sm_nActiveTab = 0;

//////////////////////////////////////////////////////////////////////
// CMixGroupPPGMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMixGroupPPGMgr::CMixGroupPPGMgr( ) : CDllBasePropPageManager()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_GUIDManager	= GUID_MixGroupPPGMgr;

	m_pTabMixGroup= NULL;
}

CMixGroupPPGMgr::~CMixGroupPPGMgr()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pTabMixGroup)
	{
		delete m_pTabMixGroup;
		m_pTabMixGroup = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMixGroupPPGMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CMixGroupPPGMgr IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CMixGroupPPGMgr::GetPropertySheetTitle(BSTR* pbstrTitle, BOOL* pfAddPropertiesText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if((pbstrTitle == NULL)
	|| (pfAddPropertiesText == NULL))
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_MIXGROUP_TEXT );

	MixGroupInfoForPPG mixGroupInfoForPPG;

	// Nothing is constructed in the call to GetData, so we don't need to worry
	// about cleaning up any memory
	if(m_pIPropPageObject
	&& SUCCEEDED(m_pIPropPageObject->GetData((void **)&mixGroupInfoForPPG)) )
	{
		if( mixGroupInfoForPPG.m_fValid
		&&	!mixGroupInfoForPPG.m_strMixGroupName.IsEmpty() )
		{
			strTitle = mixGroupInfoForPPG.m_strMixGroupName + _T(" ") + strTitle;
		}
		if( !mixGroupInfoForPPG.m_strAudioPathName.IsEmpty() )
		{
			strTitle = mixGroupInfoForPPG.m_strAudioPathName + _T(" - ") + strTitle;
		}
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMixGroupPPGMgr IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CMixGroupPPGMgr::GetPropertySheetPages(IDMUSProdPropSheet *pIPropSheet, LONG *hPropSheetPage[], short *pnNbrPages)
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
	if( m_pTabMixGroup == NULL )
	{
		m_pTabMixGroup = new CTabMixGroup();
	}
	if(m_pTabMixGroup)
	{
		hPage = ::CreatePropertySheetPage((LPCPROPSHEETPAGE)&m_pTabMixGroup->m_psp);
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
// CMixGroupPPGMgr IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CMixGroupPPGMgr::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	if(m_pIPropSheet)
	{
		m_pIPropSheet->GetActivePage( &CMixGroupPPGMgr::sm_nActiveTab );
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
// CMixGroupPPGMgr IDMUSProdPropPageManager::RefreshData

HRESULT CMixGroupPPGMgr::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pTabMixGroup == NULL)
	{
		return S_OK;
	}

	// Try and get the information for the selected Mix Group
	MixGroupInfoForPPG mixGroupInfoForPPG;
	if( (m_pIPropPageObject == NULL)
	||	FAILED(m_pIPropPageObject->GetData((void **)&mixGroupInfoForPPG)) )
	{
		m_pTabMixGroup->SetItem( NULL, NULL );

		return S_OK;
	}

	// Make sure changes to current effect are processed in OnKillFocus
	// messages before setting the new effect
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabMixGroup->GetSafeHwnd() ? m_pTabMixGroup->GetParent() : NULL;

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	m_pTabMixGroup->SetItem( &mixGroupInfoForPPG, m_pIPropPageObject );

	// Restore focus
	if( pWndHadFocus
	&&	pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMixGroupPPGMgr IDMUSProdPropPageManager::SetObject

HRESULT CMixGroupPPGMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
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
// CMixGroupPPGMgr IDMUSProdPropPageManager::RemoveObject

HRESULT CMixGroupPPGMgr::RemoveObject( IDMUSProdPropPageObject *pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = CDllBasePropPageManager::RemoveObject(pIPropPageObject);
	if(SUCCEEDED(hr))
	{
		if(m_pTabMixGroup)
		{
			m_pTabMixGroup->SetItem( NULL, NULL );
		}
	}

	return hr;
}
