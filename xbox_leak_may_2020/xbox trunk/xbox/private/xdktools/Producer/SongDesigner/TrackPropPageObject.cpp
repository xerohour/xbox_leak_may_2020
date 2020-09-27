// TrackPropPageObject.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include "SongDlg.h"
#include <TrackFlagsPPG.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


short CTrackPropPageManager::sm_nActiveTab = 0;

//////////////////////////////////////////////////////////////////////
// CTrackPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrackPropPageManager::CTrackPropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabFlags = NULL;
}

CTrackPropPageManager::~CTrackPropPageManager()
{
	RELEASE( m_pIPropSheet );

	if( m_pTabFlags )
	{
		delete m_pTabFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager::RemoveCurrentObject

void CTrackPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager IUnknown implementation

HRESULT CTrackPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageManager)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageManager *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CTrackPropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CTrackPropPageManager::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CTrackPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_TRACK_TEXT );

	PPGTrack ppgTrack;
	PPGTrack* pPPGTrack = &ppgTrack;

	if( m_pIPropPageObject 
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pPPGTrack))) )
	{
		if( ppgTrack.pSong
		&&  ppgTrack.pTrack )
		{
			CString strName;
			BSTR bstrName;

			strTitle.LoadString( IDS_SONG_TEXT );
			strTitle += _T(": ");

			// Add name of Song
			if( ppgTrack.pSong )
			{
				if( SUCCEEDED ( ppgTrack.pSong->GetNodeName( &bstrName ) ) )
				{
					strName = bstrName;
					::SysFreeString( bstrName );
			
					strTitle += strName;
					strTitle += _T(" - ");
				}
			}

			// Add Name of Track
			if( ppgTrack.pTrack )
			{
				ppgTrack.pTrack ->FormatTextUI( strName );
				strTitle += _T("'");
				strTitle += strName;
				strTitle += _T("' ");
			}
		}
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CTrackPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	// Add Flags tab
	m_pTabFlags = new CTrackFlagsPPG();
	if( m_pTabFlags )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabFlags->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			

		// Weak reference
		m_pTabFlags->m_pIPropSheet = m_pIPropSheet;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CTrackPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CTrackPropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CTrackPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pTabFlags )
	{
		m_pTabFlags->SetObject( pINewPropPageObject );
	}
	
	if( m_pIPropPageObject != pINewPropPageObject )
	{
		RemoveCurrentObject();

		m_pIPropPageObject = pINewPropPageObject;
//		m_pIPropPageObject->AddRef();		intentionally missing
	}

	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CTrackPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pIPropPageObject == NULL)
	||  (pIPropPageObject != m_pIPropPageObject) )
	{
		return E_INVALIDARG;
	}

	RemoveCurrentObject();
	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CTrackPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( pIPropPageObject == m_pIPropPageObject )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CTrackPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pTabFlags )
	{
		m_pTabFlags->RefreshData();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CTrackPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_TrackPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	CTrackPropPageObject Constructor/Destructor

CTrackPropPageObject::CTrackPropPageObject( CSongDlg* pSongDlg )
{
    m_dwRef = 0;
	AddRef();

	ASSERT( pSongDlg != NULL );
	m_pSongDlg = pSongDlg;
//	m_pSongDlg->AddRef();		intentionally missing

	m_pTrack = NULL;
	m_dwFlagsUI = 0;
}

CTrackPropPageObject::~CTrackPropPageObject( void )
{
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Remove TrackPropPageObject from property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageObject IUnknown implementation

HRESULT CTrackPropPageObject::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IDMUSProdPropPageObject *)this;
    }
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CTrackPropPageObject::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CTrackPropPageObject::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp(); 
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageObject IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageObject IDMUSProdPropPageObject::GetData

HRESULT CTrackPropPageObject::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (ppData == NULL)
	|| (*ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	int nSelCount = m_pSongDlg->GetTrackSelCount();
	if( nSelCount > 1 )
	{
		m_pTrack = NULL;
		m_dwFlagsUI = PROPF_MULTI_SELECTED;
	}
	else
	{
		m_pTrack = m_pSongDlg->GetTrackToEdit();
		if( m_pTrack )
		{
			m_dwFlagsUI = PROPF_ONE_SELECTED;
		}
		else
		{
			m_dwFlagsUI = PROPF_NONE_SELECTED;
		}
	}

	DWORD *pdwIndex = static_cast<DWORD *>(*ppData);
	switch( *pdwIndex )
	{
		// Asking for Song and Track pointers
		case 0:
		{
			PPGTrack *pPPGTrack = reinterpret_cast<PPGTrack *>(*ppData);

			if( m_pTrack )
			{
				pPPGTrack->pSong = m_pTrack->m_pSong;
			}
			else
			{
				pPPGTrack->pSong = NULL;
			}
			pPPGTrack->pTrack = m_pTrack;
			break;
		}

		// Track flags tab
		case 1:
		{
			PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);

			if( m_pTrack )
			{
				pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pTrack->GetExtraFlags();
				pPPGTrackFlagsParams->dwTrackExtrasMask = m_pTrack->GetExtraMask();
			}
			else
			{
				pPPGTrackFlagsParams->dwTrackExtrasFlags = 0;
				pPPGTrackFlagsParams->dwTrackExtrasMask = 0;
			}
			pPPGTrackFlagsParams->dwProducerOnlyFlags = 0;
			pPPGTrackFlagsParams->dwProducerOnlyMask = 0;
			break;
		}

		default:
			ASSERT( 0 );
			break;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageObject IDMUSProdPropPageObject::SetData

HRESULT CTrackPropPageObject::SetData( void* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate parameter
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( m_pTrack == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}
	
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
		// Track flags tab
		case 1:
		{
			PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

			// m_dwFlagsDM
			if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pTrack->GetExtraFlags() )
			{
				m_pTrack->m_pSong->m_pUndoMgr->SaveState( m_pTrack->m_pSong, theApp.m_hInstance, IDS_UNDO_TRACK_EXTRAS_FLAGS );
				m_pTrack->SetExtraFlags( pPPGTrackFlagsParams->dwTrackExtrasFlags );

				// Sync changes
				m_pTrack->m_pSong->SetModified( TRUE );
				m_pTrack->m_pSong->SyncChanges( SC_DIRECTMUSIC, 0, NULL );
			}

			break;
		}

		default:
			ASSERT( 0 );
			break;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageObject IDMUSProdPropPageObject::OnShowProperties

HRESULT CTrackPropPageObject::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Get the Track's page manager
	CTrackPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_TrackPropPageManager ) == S_OK )
	{
		pPageManager = (CTrackPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CTrackPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Track's properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( pPageManager == theApp.m_pIPageManager )
		{
			pIPropSheet->GetActivePage( &CTrackPropPageManager::sm_nActiveTab );
		}
		short nActiveTab = CTrackPropPageManager::sm_nActiveTab;

		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		RELEASE( pIPropSheet );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackPropPageObject IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CTrackPropPageObject::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}
