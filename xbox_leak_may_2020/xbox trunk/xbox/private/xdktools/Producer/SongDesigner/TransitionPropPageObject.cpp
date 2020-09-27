// TransitionPropPageObject.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include "SongDlg.h"
#include "TabTransition.h"
#include <TabBoundaryFlags.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


short CTransitionPropPageManager::sm_nActiveTab = 0;

//////////////////////////////////////////////////////////////////////
// CTransitionPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransitionPropPageManager::CTransitionPropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabTransition = NULL;
	m_pTabBoundary = NULL;
}

CTransitionPropPageManager::~CTransitionPropPageManager()
{
	RELEASE( m_pIPropSheet );

	if( m_pTabTransition )
	{
		delete m_pTabTransition;
	}

	if( m_pTabBoundary )
	{
		delete m_pTabBoundary;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionPropPageManager::RemoveCurrentObject

void CTransitionPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionPropPageManager IUnknown implementation

HRESULT CTransitionPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CTransitionPropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CTransitionPropPageManager::Release()
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
// CTransitionPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CTransitionPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CTransitionPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;

	PPGTransition ppgTransition;
	PPGTransition* pPPGTransition = &ppgTransition;

	if( m_pIPropPageObject 
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pPPGTransition))) )
	{
		if( ppgTransition.pSong
		&&  ppgTransition.pTransition )
		{
			CString strName;
			BSTR bstrName;

			strTitle.LoadString( IDS_SONG_TEXT );
			strTitle += _T(": ");

			// Add name of Song
			if( ppgTransition.pSong )
			{
				if( SUCCEEDED ( ppgTransition.pSong->GetNodeName( &bstrName ) ) )
				{
					strName = bstrName;
					::SysFreeString( bstrName );
			
					strTitle += strName;
					strTitle += _T(" - ");
				}
			}
		}
	}

	// Add "Transition"
	CString strTransition;
	strTransition.LoadString( IDS_TRANSITION_TEXT );
	strTitle += strTransition;

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CTransitionPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	// Add Transition tab
	m_pTabTransition = new CTabTransition( this );
	if( m_pTabTransition )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabTransition->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Boundary tab
	m_pTabBoundary = new CTabBoundaryFlags( this );
	m_pTabBoundary->UseTransitionPrompts( TRUE );
	if( m_pTabBoundary )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabBoundary->m_psp );
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
// CTransitionPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CTransitionPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CTransitionPropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CTransitionPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pTabTransition )
	{
		m_pTabTransition->SetObject( pINewPropPageObject );
	}

	if( m_pTabBoundary )
	{
		m_pTabBoundary->SetObject( pINewPropPageObject );
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
// CTransitionPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CTransitionPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CTransitionPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CTransitionPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CTransitionPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CTransitionPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pTabTransition )
	{
		m_pTabTransition->RefreshTab();
	}

	if( m_pTabBoundary )
	{
		m_pTabBoundary->RefreshTab();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CTransitionPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_TransitionPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	CTransitionPropPageObject Constructor/Destructor

CTransitionPropPageObject::CTransitionPropPageObject( CSongDlg* pSongDlg )
{
    m_dwRef = 0;
	AddRef();

	ASSERT( pSongDlg != NULL );
	m_pSongDlg = pSongDlg;
//	m_pSongDlg->AddRef();		intentionally missing

	m_pTransition = NULL;
	m_dwFlagsUI = 0;
}

CTransitionPropPageObject::~CTransitionPropPageObject( void )
{
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Remove TransitionPropPageObject from property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionPropPageObject IUnknown implementation

HRESULT CTransitionPropPageObject::QueryInterface( REFIID riid, LPVOID* ppvObj )
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

ULONG CTransitionPropPageObject::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CTransitionPropPageObject::Release()
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
// CTransitionPropPageObject IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CTransitionPropPageObject IDMUSProdPropPageObject::GetData

HRESULT CTransitionPropPageObject::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (ppData == NULL)
	|| (*ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	int nSelCount = m_pSongDlg->GetTransitionSelCount();
	if( nSelCount > 1 )
	{
		m_pTransition = NULL;
		m_dwFlagsUI = PROPF_MULTI_SELECTED;
	}
	else
	{
		m_pTransition = m_pSongDlg->GetTransitionToEdit();
		if( m_pTransition )
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
		// Asking for Song, VirtualSegment and Transition pointers
		case 0:
		{
			PPGTransition *pPPGTransition = reinterpret_cast<PPGTransition *>(*ppData);

			if( m_pTransition )
			{
				pPPGTransition->pSong = m_pTransition->m_pSong;
				if( m_pSongDlg )
				{
					pPPGTransition->pVirtualSegmentToEdit = m_pSongDlg->GetVirtualSegmentToEdit();
				}
				else
				{
					pPPGTransition->pVirtualSegmentToEdit = NULL;
				}
			}
			else
			{
				pPPGTransition->pSong = NULL;
				pPPGTransition->pVirtualSegmentToEdit = NULL;
			}
			pPPGTransition->pTransition = m_pTransition;
			break;
		}

		// Transition tab
		case 1:
		{
			PPGTabTransition *pPPGTabTransition = reinterpret_cast<PPGTabTransition *>(*ppData);

			if( m_pTransition )
			{
				pPPGTabTransition->dwToSegmentFlag = m_pTransition->m_dwToSegmentFlag;
				pPPGTabTransition->pToSegment = m_pTransition->m_pToSegment;
				pPPGTabTransition->pTransitionSegment = m_pTransition->m_pTransitionSegment;
				pPPGTabTransition->pSong = m_pTransition->m_pSong;
			}
			else
			{
				pPPGTabTransition->dwToSegmentFlag = 0;
				pPPGTabTransition->pToSegment = NULL;
				pPPGTabTransition->pTransitionSegment = NULL;
				pPPGTabTransition->pSong = NULL;
			}
			pPPGTabTransition->dwFlagsUI = m_dwFlagsUI;
			break;
		}

		// Boundary tab
		case 2:
		{
			PPGTabBoundaryFlags *pPPGTabBoundaryFlags = reinterpret_cast<PPGTabBoundaryFlags *>(*ppData);

			if( m_pTransition )
			{
				pPPGTabBoundaryFlags->dwPlayFlags = m_pTransition->m_dwPlayFlagsDM;
				pPPGTabBoundaryFlags->dwFlagsUI = PROPF_HAVE_VALID_DATA;
				m_pTransition->FormatTextUI( pPPGTabBoundaryFlags->strPrompt );
			}
			else
			{
				pPPGTabBoundaryFlags->dwPlayFlags = 0;
				pPPGTabBoundaryFlags->dwFlagsUI = 0;
				if( m_dwFlagsUI & PROPF_MULTI_SELECTED )
				{
					pPPGTabBoundaryFlags->strPrompt.LoadString( IDS_MULTIPLE_TRANSITIONS_SELECTED );
				}
				else
				{
					pPPGTabBoundaryFlags->strPrompt.Empty();
				}
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
// CTransitionPropPageObject IDMUSProdPropPageObject::SetData

HRESULT CTransitionPropPageObject::SetData( void* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate parameter
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( m_pTransition == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}
	
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
		// Transition tab
		case 1:
		{
			PPGTabTransition *pPPGTabTransition = reinterpret_cast<PPGTabTransition *>(pData);

			// m_pToSegment
			if( pPPGTabTransition->pToSegment != m_pTransition->m_pToSegment 
			||  pPPGTabTransition->dwToSegmentFlag != m_pTransition->m_dwToSegmentFlag )
			{
				m_pTransition->m_pSong->m_pUndoMgr->SaveState( m_pTransition->m_pSong, theApp.m_hInstance, IDS_UNDO_TRANSITION_FROM_SEGMENT );

				RELEASE( m_pTransition->m_pToSegment );
				m_pTransition->m_pToSegment = pPPGTabTransition->pToSegment;
				if( m_pTransition->m_pToSegment )
				{
					m_pTransition->m_pToSegment->AddRef();
				}
				m_pTransition->m_dwToSegmentFlag = pPPGTabTransition->dwToSegmentFlag;

				// Sync changes
				m_pTransition->m_pSong->SetModified( TRUE );
				m_pTransition->m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC, SSE_TRANSITION_LIST, NULL );
			}

			// m_pTransitionSegment
			if( pPPGTabTransition->pTransitionSegment != m_pTransition->m_pTransitionSegment )
			{
				m_pTransition->m_pSong->m_pUndoMgr->SaveState( m_pTransition->m_pSong, theApp.m_hInstance, IDS_UNDO_TRANSITION_TRAN_SEGMENT );

				RELEASE( m_pTransition->m_pTransitionSegment );
				m_pTransition->m_pTransitionSegment = pPPGTabTransition->pTransitionSegment;
				if( m_pTransition->m_pTransitionSegment )
				{
					m_pTransition->m_pTransitionSegment->AddRef();
				}

				// Sync changes
				m_pTransition->m_pSong->SetModified( TRUE );
				m_pTransition->m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC, SSE_TRANSITION_LIST, NULL );
			}
			break;
		}

		// Boundary tab
		case 2:
		{
			PPGTabBoundaryFlags *pPPGTabBoundaryFlags = reinterpret_cast<PPGTabBoundaryFlags *>(pData);

			// m_dwFlagsDM
			if( pPPGTabBoundaryFlags->dwPlayFlags != m_pTransition->m_dwPlayFlagsDM )
			{
				m_pTransition->m_pSong->m_pUndoMgr->SaveState( m_pTransition->m_pSong, theApp.m_hInstance, IDS_UNDO_TRACK_EXTRAS_FLAGS );
				m_pTransition->m_dwPlayFlagsDM = pPPGTabBoundaryFlags->dwPlayFlags;

				// Sync changes
				m_pTransition->m_pSong->SetModified( TRUE );
				m_pTransition->m_pSong->SyncChanges( SC_DIRECTMUSIC, 0, NULL );
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
// CTransitionPropPageObject IDMUSProdPropPageObject::OnShowProperties

HRESULT CTransitionPropPageObject::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Get the Transition's page manager
	CTransitionPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_TransitionPropPageManager ) == S_OK )
	{
		pPageManager = (CTransitionPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CTransitionPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Transition's properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( pPageManager == theApp.m_pIPageManager )
		{
			pIPropSheet->GetActivePage( &CTransitionPropPageManager::sm_nActiveTab );
		}
		short nActiveTab = CTransitionPropPageManager::sm_nActiveTab;

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
// CTransitionPropPageObject IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CTransitionPropPageObject::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}
