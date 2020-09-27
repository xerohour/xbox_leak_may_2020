// PChannelPropPageMgr.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BandEditorDLL.h"
#include "Band.h"
#include "BandDlg.h"
#include "DllBasePropPageManager.h"
#include "PCHannelPropPage.h"
#include "PChannelPropPageMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// CPChannelPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPChannelPropPageManager::CPChannelPropPageManager()
{
	m_pPChannelTab = NULL;
	m_GUIDManager = GUID_PChannelPropPageManager;
}

CPChannelPropPageManager::~CPChannelPropPageManager()
{
	if( m_pPChannelTab )
	{
		delete m_pPChannelTab;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelPropPageManager::RemoveCurrentObject

void CPChannelPropPageManager::RemoveCurrentObject( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CPChannelPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;
	CString strPChannel;
	strPChannel.LoadString( IDS_PCHANNEL_TEXT );

	CString strTitle = strPChannel;
	
	CBand* pBand = CInstrumentListItem::m_CommonPropertyObject.GetBandDialog()->GetBand();
	ASSERT(pBand);

	
	// Format title
	CString sBandName = "";
	BSTR bstrBandName;
	if(SUCCEEDED(pBand->GetNodeName(&bstrBandName)))
	{
		sBandName = bstrBandName;
		::SysFreeString(bstrBandName);
	}

	strTitle = sBandName + _T(": ") + strPChannel;
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}

	

/////////////////////////////////////////////////////////////////////////////
// CPChannelPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CPChannelPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	// Add Band tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	if(m_pPChannelTab == NULL)
	{
		m_pPChannelTab = new CPChannelPropPage( this );
	}
	if( m_pPChannelTab )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pPChannelTab->m_psp );
		if( hPage )
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
// CPChannelPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CPChannelPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDllBasePropPageManager::OnRemoveFromPropertySheet();

	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CPChannelPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCommonInstrumentPropertyObject* pCommonInstrumentObject;
	
	if(m_pIPropPageObject == NULL)
	/*||(m_pIPropPageObject->GetData((void **)&pCommonInstrumentObject) != S_OK))*/
	{
		pCommonInstrumentObject = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pCommonInstrumentObject) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current Band are processed in OnKillFocus
	// messages before setting the new Band
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pPChannelTab->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new PChannel
	m_pPChannelTab->SetPChannel(pCommonInstrumentObject);
	
	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CPChannelPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( IsEqualObject( pINewPropPageObject ) != S_OK )
	{
		RemoveCurrentObject();

		m_pIPropPageObject = pINewPropPageObject;
//		m_pIPropPageObject->AddRef();		intentionally missing

		if( m_pIPropSheet )
		{
			m_pIPropSheet->RefreshTitle();
		}
	}

	RefreshData();

	return S_OK;
}
//////////////////////////////////////////////////////////////////////