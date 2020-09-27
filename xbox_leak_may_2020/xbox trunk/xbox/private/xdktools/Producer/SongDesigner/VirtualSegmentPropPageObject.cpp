// VirtualSegmentPropPageObject.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include "SongDlg.h"
#include <TabBoundaryFlags.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


short CVirtualSegmentPropPageManager::sm_nActiveTab = 0;

//////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVirtualSegmentPropPageManager::CVirtualSegmentPropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabSegment = NULL;
	m_pTabLoop = NULL;
	m_pTabBoundary = NULL;
}

CVirtualSegmentPropPageManager::~CVirtualSegmentPropPageManager()
{
	RELEASE( m_pIPropSheet );

	if( m_pTabSegment )
	{
		delete m_pTabSegment;
	}

	if( m_pTabLoop )
	{
		delete m_pTabLoop;
	}

	if( m_pTabBoundary )
	{
		delete m_pTabBoundary;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageManager::RemoveCurrentObject

void CVirtualSegmentPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageManager IUnknown implementation

HRESULT CVirtualSegmentPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CVirtualSegmentPropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CVirtualSegmentPropPageManager::Release()
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
// CVirtualSegmentPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CVirtualSegmentPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;

	PPGVirtualSegment ppgVirtualSegment;
	PPGVirtualSegment* pPPGVirtualSegment = &ppgVirtualSegment;

	if( m_pIPropPageObject 
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pPPGVirtualSegment))) )
	{
		if( ppgVirtualSegment.pSong
		&&  ppgVirtualSegment.pVirtualSegment )
		{
			CString strName;
			BSTR bstrName;

			strTitle.LoadString( IDS_SONG_TEXT );
			strTitle += _T(": ");

			// Add name of Song
			if( ppgVirtualSegment.pSong )
			{
				if( SUCCEEDED ( ppgVirtualSegment.pSong->GetNodeName( &bstrName ) ) )
				{
					strName = bstrName;
					::SysFreeString( bstrName );
			
					strTitle += strName;
					strTitle += _T(" - ");
				}
			}

			// Add Name of VirtualSegment
			ppgVirtualSegment.pVirtualSegment->GetName( strName );
			strTitle += _T("'");
			strTitle += strName;
			strTitle += _T("' ");
		}
	}

	// Add "Virtual Segment"
	CString strVirtualSegment;
	strVirtualSegment.LoadString( IDS_VIRTUAL_SEGMENT_TEXT );
	strTitle += strVirtualSegment;

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CVirtualSegmentPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	// Add Segment tab
	m_pTabSegment = new CTabVirtualSegment( this );
	if( m_pTabSegment )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabSegment->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Loop tab
	m_pTabLoop = new CTabVirtualSegmentLoop( this );
	if( m_pTabLoop )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabLoop->m_psp );
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
// CVirtualSegmentPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CVirtualSegmentPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CVirtualSegmentPropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CVirtualSegmentPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pTabSegment )
	{
		m_pTabSegment->SetObject( pINewPropPageObject );
	}

	if( m_pTabLoop )
	{
		m_pTabLoop->SetObject( pINewPropPageObject );
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
// CVirtualSegmentPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CVirtualSegmentPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CVirtualSegmentPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CVirtualSegmentPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CVirtualSegmentPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CVirtualSegmentPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pTabSegment )
	{
		m_pTabSegment->RefreshTab();
	}

	if( m_pTabLoop )
	{
		m_pTabLoop->RefreshTab();
	}

	if( m_pTabBoundary )
	{
		m_pTabBoundary->RefreshTab();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CVirtualSegmentPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_VirtualSegmentPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	CVirtualSegmentPropPageObject Constructor/Destructor

CVirtualSegmentPropPageObject::CVirtualSegmentPropPageObject( CSongDlg* pSongDlg )
{
    m_dwRef = 0;
	AddRef();

	ASSERT( pSongDlg != NULL );
	m_pSongDlg = pSongDlg;
//	m_pSongDlg->AddRef();		intentionally missing

	m_pVirtualSegment = NULL;
	m_dwFlagsUI = 0;
}

CVirtualSegmentPropPageObject::~CVirtualSegmentPropPageObject( void )
{
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Remove VirtualSegmentPropPageObject from property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageObject IUnknown implementation

HRESULT CVirtualSegmentPropPageObject::QueryInterface( REFIID riid, LPVOID* ppvObj )
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

ULONG CVirtualSegmentPropPageObject::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CVirtualSegmentPropPageObject::Release()
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
// CVirtualSegmentPropPageObject IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageObject IDMUSProdPropPageObject::GetData

HRESULT CVirtualSegmentPropPageObject::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (ppData == NULL)
	|| (*ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	int nSelCount = m_pSongDlg->GetVirtualSegmentSelCount();
	if( nSelCount > 1 )
	{
		m_pVirtualSegment = NULL;
		m_dwFlagsUI = PROPF_MULTI_SELECTED;
	}
	else
	{
		m_pVirtualSegment = m_pSongDlg->GetVirtualSegmentToEdit();
		if( m_pVirtualSegment )
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
		// Asking for Song and VirtualSegment pointers
		case 0:
		{
			PPGVirtualSegment *pPPGVirtualSegment = reinterpret_cast<PPGVirtualSegment *>(*ppData);

			if( m_pVirtualSegment )
			{
				pPPGVirtualSegment->pSong = m_pVirtualSegment->m_pSong;
			}
			else
			{
				pPPGVirtualSegment->pSong = NULL;
			}
			pPPGVirtualSegment->pVirtualSegment = m_pVirtualSegment;
			break;
		}

		// Segment tab
		case 1:
		{
			PPGTabVirtualSegment* pPPGTabVirtualSegment = reinterpret_cast<PPGTabVirtualSegment *>(*ppData);
			InitPPGTabVirtualSegment( pPPGTabVirtualSegment );

			pPPGTabVirtualSegment->dwFlagsUI = m_dwFlagsUI;

			if( m_pVirtualSegment )
			{
				pPPGTabVirtualSegment->strName = m_pVirtualSegment->m_strName;
				pPPGTabVirtualSegment->dwTimeSigGroupBits = m_pVirtualSegment->m_VirtualSegmentUI.dwTimeSigGroupBits;
				pPPGTabVirtualSegment->dwResolution = m_pVirtualSegment->m_dwResolution_SEGH;

				long lMeasure, lBeat, lGrid, lTick;
				if( SUCCEEDED ( m_pVirtualSegment->ClocksToMeasureBeatGridTick( m_pVirtualSegment->m_mtLength_SEGH,
																			    &lMeasure,
																			    &lBeat,
																			    &lGrid,
																			    &lTick ) ) 
				&&  SUCCEEDED ( m_pVirtualSegment->ClocksToMeasureBeatGridTick( m_pVirtualSegment->m_mtPlayStart_SEGH,
																			    &pPPGTabVirtualSegment->lStartMeasure,
																			    &pPPGTabVirtualSegment->lStartBeat,
																			    &pPPGTabVirtualSegment->lStartGrid,
																			    &pPPGTabVirtualSegment->lStartTick ) ) )
				{
					m_pVirtualSegment->m_VirtualSegmentUI.dwNbrMeasures = max( 1, lMeasure );
					pPPGTabVirtualSegment->dwNbrMeasures = m_pVirtualSegment->m_VirtualSegmentUI.dwNbrMeasures;
					pPPGTabVirtualSegment->dwFlagsUI |= PROPF_TIME_CONVERSIONS_OK;
				}
				if( m_pVirtualSegment->HasAudioPath() )
				{
					pPPGTabVirtualSegment->dwFlagsUI |= PROPF_HAVE_AUDIOPATH;
				}
			}
			break;
		}

		// Boundary tab (Shared code so *pdwIndex must be 2)
		case 2:
		{
			PPGTabBoundaryFlags* pPPGTabBoundaryFlags = reinterpret_cast<PPGTabBoundaryFlags *>(*ppData);

			if( m_pVirtualSegment )
			{
				pPPGTabBoundaryFlags->dwPlayFlags = m_pVirtualSegment->m_dwResolution_SEGH;
				pPPGTabBoundaryFlags->strPrompt = m_pVirtualSegment->m_strName;
				pPPGTabBoundaryFlags->dwFlagsUI = PROPF_HAVE_VALID_DATA;
			}
			else
			{
				pPPGTabBoundaryFlags->dwPlayFlags = 0;
				if( m_dwFlagsUI & PROPF_MULTI_SELECTED )
				{
					pPPGTabBoundaryFlags->strPrompt.LoadString( IDS_MULTIPLE_SEGMENTS_SELECTED );
				}
				else
				{
					pPPGTabBoundaryFlags->strPrompt.Empty();
				}
				pPPGTabBoundaryFlags->dwFlagsUI = 0;
			}
			break;
		}

		// Loop tab
		case 3:
		{
			PPGTabVirtualSegmentLoop* pPPGTabVirtualSegmentLoop = reinterpret_cast<PPGTabVirtualSegmentLoop *>(*ppData);
			InitPPGTabVirtualSegmentLoop( pPPGTabVirtualSegmentLoop );

			pPPGTabVirtualSegmentLoop->dwFlagsUI = m_dwFlagsUI;

			if( m_pVirtualSegment )
			{
				pPPGTabVirtualSegmentLoop->dwRepeats = m_pVirtualSegment->m_dwRepeats_SEGH;
				MUSIC_TIME mtLoopEnd;
				if( pPPGTabVirtualSegmentLoop->dwRepeats > 0
				&&  m_pVirtualSegment->m_mtLoopStart_SEGH == 0
				&&  m_pVirtualSegment->m_mtLoopEnd_SEGH == 0 )
				{
					mtLoopEnd = m_pVirtualSegment->m_mtLength_SEGH;
				}
				else
				{
					mtLoopEnd = m_pVirtualSegment->m_mtLoopEnd_SEGH;
				}
				if( SUCCEEDED ( m_pVirtualSegment->ClocksToMeasureBeatGridTick( m_pVirtualSegment->m_mtLoopStart_SEGH,
																			    &pPPGTabVirtualSegmentLoop->lStartMeasure,
																			    &pPPGTabVirtualSegmentLoop->lStartBeat,
																			    &pPPGTabVirtualSegmentLoop->lStartGrid,
																			    &pPPGTabVirtualSegmentLoop->lStartTick ) ) 
				&&  SUCCEEDED ( m_pVirtualSegment->ClocksToMeasureBeatGridTick( mtLoopEnd,
																				&pPPGTabVirtualSegmentLoop->lEndMeasure,
																				&pPPGTabVirtualSegmentLoop->lEndBeat,
																				&pPPGTabVirtualSegmentLoop->lEndGrid,
																				&pPPGTabVirtualSegmentLoop->lEndTick ) ) )
				{
					pPPGTabVirtualSegmentLoop->dwFlagsUI |= PROPF_TIME_CONVERSIONS_OK;
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
// CVirtualSegmentPropPageObject IDMUSProdPropPageObject::SetData

HRESULT CVirtualSegmentPropPageObject::SetData( void* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate parameter
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( m_pVirtualSegment == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	bool fChanged = false;
	
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
		// Segment tab
		case 1:
		{
			PPGTabVirtualSegment *pPPGTabVirtualSegment = reinterpret_cast<PPGTabVirtualSegment *>(pData);

			// m_mtLength_SEGH
			MUSIC_TIME mtTime;
			if( SUCCEEDED ( m_pVirtualSegment->MeasureBeatGridTickToClocks( pPPGTabVirtualSegment->dwNbrMeasures,
																		    0,
																		    0,
																		    0,
																		    &mtTime ) ) )
			{
				if( mtTime != m_pVirtualSegment->m_mtLength_SEGH )
				{
					m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_VIRTUAL_SEGMENT_LENGTH );
					m_pVirtualSegment->m_mtLength_SEGH = mtTime;
					m_pVirtualSegment->m_VirtualSegmentUI.dwNbrMeasures = pPPGTabVirtualSegment->dwNbrMeasures;
					fChanged = true;
					m_pVirtualSegment->m_mtPlayStart_SEGH = min( m_pVirtualSegment->m_mtPlayStart_SEGH, (m_pVirtualSegment->m_mtLength_SEGH - 1) );
					if( m_pVirtualSegment->m_mtLoopStart_SEGH != 0
					||  m_pVirtualSegment->m_mtLoopEnd_SEGH != 0 )
					{
						m_pVirtualSegment->m_mtLoopEnd_SEGH = min( m_pVirtualSegment->m_mtLoopEnd_SEGH, m_pVirtualSegment->m_mtLength_SEGH );
						m_pVirtualSegment->m_mtLoopStart_SEGH = min( m_pVirtualSegment->m_mtLoopStart_SEGH, (m_pVirtualSegment->m_mtLoopEnd_SEGH - 1) );
					}
					break;
				}
			}

			// m_mtPlayStart_SEGH
			if( SUCCEEDED ( m_pVirtualSegment->MeasureBeatGridTickToClocks( pPPGTabVirtualSegment->lStartMeasure,
																		    pPPGTabVirtualSegment->lStartBeat,
																		    pPPGTabVirtualSegment->lStartGrid,
																		    pPPGTabVirtualSegment->lStartTick,
																		    &mtTime ) ) )
			{
				mtTime = max( 0, mtTime );
				mtTime = min( (m_pVirtualSegment->m_mtLength_SEGH - 1), mtTime );
				if( mtTime != m_pVirtualSegment->m_mtPlayStart_SEGH )
				{
					m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_VIRTUAL_SEGMENT_START );
					m_pVirtualSegment->m_mtPlayStart_SEGH = mtTime;
					fChanged = true;
					break;
				}
			}

			// m_VirtualSegmentUI.dwTimeSigGroupBits
			if( pPPGTabVirtualSegment->dwTimeSigGroupBits != m_pVirtualSegment->m_VirtualSegmentUI.dwTimeSigGroupBits )
			{
				m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_VIRTUAL_SEGMENT_GROUP_BITS );
				m_pVirtualSegment->m_VirtualSegmentUI.dwTimeSigGroupBits = pPPGTabVirtualSegment->dwTimeSigGroupBits;
				fChanged = true;
				break;
			}

			// DMUS_SEGF_USE_AUDIOPATH
			if( (pPPGTabVirtualSegment->dwResolution & DMUS_SEGF_USE_AUDIOPATH) != (m_pVirtualSegment->m_dwResolution_SEGH & DMUS_SEGF_USE_AUDIOPATH) )
			{
				m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_VIRTUAL_SEGMENT_USE_AUDIOPATH );
				m_pVirtualSegment->m_dwResolution_SEGH &= ~DMUS_SEGF_USE_AUDIOPATH;
				m_pVirtualSegment->m_dwResolution_SEGH |= (pPPGTabVirtualSegment->dwResolution & DMUS_SEGF_USE_AUDIOPATH);
				fChanged = true;
				break;
			}

			// DMUS_SEGF_TIMESIG_ALWAYS
			if( (pPPGTabVirtualSegment->dwResolution & DMUS_SEGF_TIMESIG_ALWAYS) != (m_pVirtualSegment->m_dwResolution_SEGH & DMUS_SEGF_TIMESIG_ALWAYS) )
			{
				m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_VIRTUAL_SEGMENT_EXTEND_TIMESIG );
				m_pVirtualSegment->m_dwResolution_SEGH &= ~DMUS_SEGF_TIMESIG_ALWAYS;
				m_pVirtualSegment->m_dwResolution_SEGH |= (pPPGTabVirtualSegment->dwResolution & DMUS_SEGF_TIMESIG_ALWAYS);
				fChanged = true;
				break;
			}

			// m_strName
			if( pPPGTabVirtualSegment->strName.Compare(m_pVirtualSegment->m_strName) != 0 )
			{
				m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_CHANGE_VIRTUAL_SEGMENT_NAME );
				m_pVirtualSegment->m_strName = pPPGTabVirtualSegment->strName;
				m_pVirtualSegment->m_pSong->SyncChanges( SC_EDITOR,
														 SSE_REDRAW_VIRTUAL_SEGMENT_LIST | SSE_REDRAW_TRANSITION_LIST,
														 NULL );
				fChanged = true;
				break;
			}
			break;
		}

		// Boundary tab
		case 2:
		{
			PPGTabBoundaryFlags *pPPGTabBoundaryFlags = reinterpret_cast<PPGTabBoundaryFlags *>(pData);

			// m_dwResolution_SEGH
			if( pPPGTabBoundaryFlags->dwPlayFlags != m_pVirtualSegment->m_dwResolution_SEGH )
			{
				DWORD dwOrigTabVSegFlags = (m_pVirtualSegment->m_dwResolution_SEGH & TAB_VSEG_RESOLUTION_FLAGS);
				m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_VIRTUAL_SEGMENT_BOUNDARY_FLAGS );
				m_pVirtualSegment->m_dwResolution_SEGH = pPPGTabBoundaryFlags->dwPlayFlags;
				m_pVirtualSegment->m_dwResolution_SEGH &= ~TAB_VSEG_RESOLUTION_FLAGS;
				m_pVirtualSegment->m_dwResolution_SEGH |= dwOrigTabVSegFlags;
				fChanged = true;
				break;
			}
			break;
		}

		// Loop tab
		case 3:
		{
			PPGTabVirtualSegmentLoop *pPPGTabVirtualSegmentLoop = reinterpret_cast<PPGTabVirtualSegmentLoop *>(pData);

			// m_dwRepeats_SEGH
			if( pPPGTabVirtualSegmentLoop->dwRepeats != DMUS_SEG_REPEAT_INFINITE )
			{
				pPPGTabVirtualSegmentLoop->dwRepeats = max( 0, pPPGTabVirtualSegmentLoop->dwRepeats );
			}
			if( pPPGTabVirtualSegmentLoop->dwRepeats != m_pVirtualSegment->m_dwRepeats_SEGH )
			{
				m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_VIRTUAL_SEGMENT_LOOP_REPEATS );
				m_pVirtualSegment->m_dwRepeats_SEGH = pPPGTabVirtualSegmentLoop->dwRepeats;
				if( m_pVirtualSegment->m_dwRepeats_SEGH == 0 )
				{
					m_pVirtualSegment->m_mtLoopStart_SEGH = 0;
					m_pVirtualSegment->m_mtLoopEnd_SEGH = 0;
				}
				fChanged = true;
				break;
			}

			// "Reset" loop (m_mtLoopStart_SEGH and m_mtLoopEnd_SEGH)
			if( pPPGTabVirtualSegmentLoop->lStartMeasure == 0
			&&  pPPGTabVirtualSegmentLoop->lStartBeat == 0
			&&  pPPGTabVirtualSegmentLoop->lStartGrid == 0
			&&  pPPGTabVirtualSegmentLoop->lStartTick == 0
			&&  pPPGTabVirtualSegmentLoop->lEndMeasure == 0
			&&  pPPGTabVirtualSegmentLoop->lEndBeat == 0
			&&  pPPGTabVirtualSegmentLoop->lEndGrid == 0
			&&  pPPGTabVirtualSegmentLoop->lEndTick == 0 )
			{
				if( m_pVirtualSegment->m_mtLoopStart_SEGH != 0
				||  m_pVirtualSegment->m_mtLoopEnd_SEGH != 0 )
				{
					m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_VIRTUAL_SEGMENT_RESET_LOOP );
					m_pVirtualSegment->m_mtLoopStart_SEGH = 0;
					m_pVirtualSegment->m_mtLoopEnd_SEGH = 0;
					fChanged = true;
				}
				break;
			}

			// m_mtLoopStart_SEGH and m_mtLoopEnd_SEGH
			MUSIC_TIME mtStartTime;
			MUSIC_TIME mtEndTime;
			if( SUCCEEDED ( m_pVirtualSegment->MeasureBeatGridTickToClocks( pPPGTabVirtualSegmentLoop->lStartMeasure,
																		    pPPGTabVirtualSegmentLoop->lStartBeat,
																		    pPPGTabVirtualSegmentLoop->lStartGrid,
																		    pPPGTabVirtualSegmentLoop->lStartTick,
																		    &mtStartTime ) ) 
			&&  SUCCEEDED ( m_pVirtualSegment->MeasureBeatGridTickToClocks( pPPGTabVirtualSegmentLoop->lEndMeasure,
																		    pPPGTabVirtualSegmentLoop->lEndBeat,
																		    pPPGTabVirtualSegmentLoop->lEndGrid,
																		    pPPGTabVirtualSegmentLoop->lEndTick,
																		    &mtEndTime ) ) )
			{
				mtStartTime = max( 0, mtStartTime );
				mtStartTime = min( (m_pVirtualSegment->m_mtLength_SEGH - 1), mtStartTime );
				mtEndTime = max( 1, mtEndTime );
				mtEndTime = min( m_pVirtualSegment->m_mtLength_SEGH, mtEndTime );

				// m_mtStartEnd_SEGH
				if( mtStartTime != m_pVirtualSegment->m_mtLoopStart_SEGH )
				{
					m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_VIRTUAL_SEGMENT_LOOP_START );
					m_pVirtualSegment->m_mtLoopStart_SEGH = mtStartTime;
					m_pVirtualSegment->m_mtLoopEnd_SEGH = mtEndTime;
					if( m_pVirtualSegment->m_mtLoopStart_SEGH >= m_pVirtualSegment->m_mtLoopEnd_SEGH )
					{
						m_pVirtualSegment->m_mtLoopEnd_SEGH = m_pVirtualSegment->m_mtLoopStart_SEGH + 1;
					}
					fChanged = true;
					break;
				}

				// m_mtLoopEnd_SEGH
				if( mtEndTime != m_pVirtualSegment->m_mtLoopEnd_SEGH )
				{
					m_pVirtualSegment->m_pSong->m_pUndoMgr->SaveState( m_pVirtualSegment->m_pSong, theApp.m_hInstance, IDS_UNDO_VIRTUAL_SEGMENT_LOOP_END );
					m_pVirtualSegment->m_mtLoopEnd_SEGH = mtEndTime;
					if( m_pVirtualSegment->m_mtLoopEnd_SEGH <= m_pVirtualSegment->m_mtLoopStart_SEGH )
					{
						m_pVirtualSegment->m_mtLoopStart_SEGH = m_pVirtualSegment->m_mtLoopEnd_SEGH - 1;
					}
					fChanged = true;
					break;
				}
			}
			break;
		}

		default:
			ASSERT( 0 );
			break;
	}

	if( fChanged )
	{
		// Sync changes
		m_pVirtualSegment->m_pSong->SetModified( TRUE );
		m_pVirtualSegment->m_pSong->SyncChanges( SC_DIRECTMUSIC, 0, NULL );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageObject IDMUSProdPropPageObject::OnShowProperties

HRESULT CVirtualSegmentPropPageObject::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Get the VirtualSegment's page manager
	CVirtualSegmentPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_VirtualSegmentPropPageManager ) == S_OK )
	{
		pPageManager = (CVirtualSegmentPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CVirtualSegmentPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the VirtualSegment's properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( pPageManager == theApp.m_pIPageManager )
		{
			pIPropSheet->GetActivePage( &CVirtualSegmentPropPageManager::sm_nActiveTab );
		}
		short nActiveTab = CVirtualSegmentPropPageManager::sm_nActiveTab;

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
// CVirtualSegmentPropPageObject IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CVirtualSegmentPropPageObject::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageObject IDMUSProdPropPageObject::InitPPGTabVirtualSegment

void CVirtualSegmentPropPageObject::InitPPGTabVirtualSegment( PPGTabVirtualSegment* pPPGTabVirtualSegment )
{
	if( pPPGTabVirtualSegment )
	{
		ASSERT( pPPGTabVirtualSegment->dwPageIndex == 1 );

		pPPGTabVirtualSegment->strName.Empty();
		pPPGTabVirtualSegment->dwNbrMeasures = 0;
		pPPGTabVirtualSegment->lStartMeasure = 0;
		pPPGTabVirtualSegment->lStartBeat = 0;
		pPPGTabVirtualSegment->lStartGrid = 0;
		pPPGTabVirtualSegment->lStartTick = 0;
		pPPGTabVirtualSegment->dwTimeSigGroupBits = 0;
		pPPGTabVirtualSegment->dwResolution = 0;
		pPPGTabVirtualSegment->dwFlagsUI = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegmentPropPageObject IDMUSProdPropPageObject::InitPPGTabVirtualSegmentLoop

void CVirtualSegmentPropPageObject::InitPPGTabVirtualSegmentLoop( PPGTabVirtualSegmentLoop* pPPGTabVirtualSegmentLoop )
{
	if( pPPGTabVirtualSegmentLoop )
	{
		ASSERT( pPPGTabVirtualSegmentLoop->dwPageIndex == 3 );

		pPPGTabVirtualSegmentLoop->dwRepeats = 0;
		pPPGTabVirtualSegmentLoop->lStartMeasure = 0;
		pPPGTabVirtualSegmentLoop->lStartBeat = 0;
		pPPGTabVirtualSegmentLoop->lStartGrid = 0;
		pPPGTabVirtualSegmentLoop->lStartTick = 0;
		pPPGTabVirtualSegmentLoop->lEndMeasure = 0;
		pPPGTabVirtualSegmentLoop->lEndBeat = 0;
		pPPGTabVirtualSegmentLoop->lEndGrid = 0;
		pPPGTabVirtualSegmentLoop->lEndTick = 0;
		pPPGTabVirtualSegmentLoop->dwFlagsUI = 0;
	}
}
