// BufferPPGMgr.cpp : implementation file
//

#include "stdafx.h"
#include "BufferPPGMgr.h"
#include "TabBuffer.h"
#include "ItemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

short CBufferPPGMgr::sm_nActiveTab = 0;

//////////////////////////////////////////////////////////////////////
// CBufferPPGMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBufferPPGMgr::CBufferPPGMgr( ) : CDllBasePropPageManager()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_GUIDManager	= GUID_BufferPPGMgr;

	m_pTabBuffer = NULL;
}

CBufferPPGMgr::~CBufferPPGMgr()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pTabBuffer)
	{
		delete m_pTabBuffer;
		m_pTabBuffer = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBufferPPGMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CBufferPPGMgr IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CBufferPPGMgr::GetPropertySheetTitle(BSTR* pbstrTitle, BOOL* pfAddPropertiesText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if((pbstrTitle == NULL)
	|| (pfAddPropertiesText == NULL))
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_BUFFER_TEXT );

	BufferInfoForPPG bufferInfoForPPG;

	// Nothing is constructed in the call to GetData, so we don't need to worry
	// about cleaning up any memory
	if(m_pIPropPageObject
	&& SUCCEEDED(m_pIPropPageObject->GetData((void **)&bufferInfoForPPG)) )
	{
		if( bufferInfoForPPG.m_fValid )
		{
			CString strBuffer;
			if( bufferInfoForPPG.m_dwHeaderFlags & DMUS_BUFFERF_DEFINED )
			{
				// Initialize the bus ID text from the GUID of the standard buffer
				if( bufferInfoForPPG.m_guidBufferID == GUID_Buffer_Reverb )
				{
					strBuffer.LoadString( IDS_BUFFER_REVERB );
				}
				else if( bufferInfoForPPG.m_guidBufferID == GUID_Buffer_EnvReverb )
				{
					strBuffer.LoadString( IDS_BUFFER_ENVREVERB );
				}
				/*
				else if( bufferInfoForPPG.m_guidBufferID == GUID_Buffer_3D )
				{
					strBuffer.LoadString( IDS_BUFFER_3D );
				}
				*/
				else if( bufferInfoForPPG.m_guidBufferID == GUID_Buffer_3D_Dry )
				{
					strBuffer.LoadString( IDS_BUFFER_3D_DRY );
				}
				else if( bufferInfoForPPG.m_guidBufferID == GUID_Buffer_Mono )
				{
					strBuffer.LoadString( IDS_BUFFER_MONO );
				}
				else if( bufferInfoForPPG.m_guidBufferID == GUID_Buffer_Stereo )
				{
					strBuffer.LoadString( IDS_BUFFER_STEREO );
				}
				else
				{
					ASSERT(FALSE);
				}
			}
			else
			{
				// Try and load the text 'User Defined'
				strBuffer.LoadString(IDS_BUFFER_USER_DEFINED);
			}

			if( !strBuffer.IsEmpty() )
			{
				strTitle = strBuffer + _T(" ") + strTitle;
			}
		}
		if( !bufferInfoForPPG.m_strAudioPathName.IsEmpty() )
		{
			strTitle = bufferInfoForPPG.m_strAudioPathName + _T(" - ") + strTitle;
		}
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBufferPPGMgr IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CBufferPPGMgr::GetPropertySheetPages(IDMUSProdPropSheet *pIPropSheet, LONG *hPropSheetPage[], short *pnNbrPages)
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

	// Add Buffer header tab
	if( m_pTabBuffer == NULL )
	{
		m_pTabBuffer = new CTabBuffer();
		m_pTabBuffer->m_pBufferPPGMgr = this;
	}
	if(m_pTabBuffer)
	{
		hPage = ::CreatePropertySheetPage((LPCPROPSHEETPAGE)&m_pTabBuffer->m_psp);
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
// CBufferPPGMgr IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CBufferPPGMgr::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	if(m_pIPropSheet)
	{
		m_pIPropSheet->GetActivePage( &CBufferPPGMgr::sm_nActiveTab );
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
// CBufferPPGMgr IDMUSProdPropPageManager::RefreshData

HRESULT CBufferPPGMgr::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pTabBuffer == NULL )
	{
		return S_OK;
	}

	BufferInfoForPPG bufferInfoForPPG;
	if( (m_pIPropPageObject == NULL)
	||	FAILED(m_pIPropPageObject->GetData((void **)&bufferInfoForPPG)) )
	{
		m_pTabBuffer->SetBuffer( NULL, NULL );
		return S_OK;
	}
	
	// Make sure changes to current effect are processed in OnKillFocus
	// messages before setting the new effect
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabBuffer->GetSafeHwnd() ? m_pTabBuffer->GetParent() : NULL;

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	m_pTabBuffer->SetBuffer( &bufferInfoForPPG, m_pIPropPageObject );

	// Restore focus
	if( pWndHadFocus
	&&	pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBufferPPGMgr IDMUSProdPropPageManager::SetObject

HRESULT CBufferPPGMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
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
// CBufferPPGMgr IDMUSProdPropPageManager::RemoveObject

HRESULT CBufferPPGMgr::RemoveObject( IDMUSProdPropPageObject *pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = CDllBasePropPageManager::RemoveObject(pIPropPageObject);
	if(SUCCEEDED(hr))
	{
		if( m_pTabBuffer )
		{
			m_pTabBuffer->SetBuffer( NULL, NULL );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// BufferInfoForPPG::Import

void BufferInfoForPPG::Import( const BufferOptions *pBufferOptions, bool fItemHasPChannels )
{
	ASSERT( pBufferOptions );
	if( !pBufferOptions )
	{
 		m_dwHeaderFlags = 0;
		m_dwBufferFlags = 0;
		m_dwNumChannels = 0;
		m_dwMinNumChannels = 0;
		m_dwDesignFlags = 0;
		m_lPan = 0;
		m_lVolume = 0;
		ZeroMemory( &m_guidBufferID, sizeof(GUID) );
		ZeroMemory( &m_clsid3dAlgorithm, sizeof(CLSID) );
		InitializeDS3DBUFFER( &m_ds3DBuffer );
	}
	else
	{
 		m_dwHeaderFlags = pBufferOptions->dwHeaderFlags;
		m_dwBufferFlags = pBufferOptions->dwBufferFlags;
		m_dwDesignFlags = fItemHasPChannels ? DESIGN_BUFFER_HAS_PCHANNELS : 0;
		m_dwMinNumChannels = fItemHasPChannels ? pBufferOptions->lstBusIDs.GetSize() : 1;
		if( m_dwHeaderFlags & DMUS_BUFFERF_DEFINED )
		{
			m_dwNumChannels = pBufferOptions->lstBusIDs.GetSize();
		}
		else
		{
			m_dwNumChannels = pBufferOptions->wChannels;
		}
		m_guidBufferID = pBufferOptions->guidBuffer;
		m_clsid3dAlgorithm = pBufferOptions->guid3DAlgorithm;
		m_ds3DBuffer = pBufferOptions->ds3DBuffer;
		m_lPan = pBufferOptions->lPan;
		m_lVolume = pBufferOptions->lVolume;
	}
}


/////////////////////////////////////////////////////////////////////////////
// BufferInfoForPPG::Copy

void BufferInfoForPPG::Copy( const BufferInfoForPPG &bufferInfoForPPG )
{
	PPGItemBase::Copy( bufferInfoForPPG );
 	m_dwHeaderFlags = bufferInfoForPPG.m_dwHeaderFlags;
	m_dwBufferFlags = bufferInfoForPPG.m_dwBufferFlags;
	m_dwNumChannels = bufferInfoForPPG.m_dwNumChannels;
	m_guidBufferID = bufferInfoForPPG.m_guidBufferID;
	m_clsid3dAlgorithm = bufferInfoForPPG.m_clsid3dAlgorithm;
	m_ds3DBuffer = bufferInfoForPPG.m_ds3DBuffer;
	m_dwDesignFlags = bufferInfoForPPG.m_dwDesignFlags;
	m_dwMinNumChannels = bufferInfoForPPG.m_dwMinNumChannels;
	m_lPan = bufferInfoForPPG.m_lPan;
	m_lVolume = bufferInfoForPPG.m_lVolume;
}
