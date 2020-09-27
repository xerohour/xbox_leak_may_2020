// Song.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "RiffStrm.h"
#include "Song.h"
#include <mmreg.h>
#include <math.h>
#include "TabSong.h"
#include "TabInfo.h"
#include "SongCtl.h"
#include <SegmentDesigner.h>
#include <ContainerDesigner.h>
#include <AudioPathDesigner.h>
#include <ToolGraphDesigner.h>
#include <dmusicf.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// {77ABC763-8BD4-4967-A4BD-683C1667F9D2}
static const GUID GUID_SongPropPageManager = 
{ 0x77abc763, 0x8bd4, 0x4967, { 0xa4, 0xbd, 0x68, 0x3c, 0x16, 0x67, 0xf9, 0xd2 } };

short CSongPropPageManager::sm_nActiveTab = 0;


//////////////////////////////////////////////////////////////////////
// CSongPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSongPropPageManager::CSongPropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabSong = NULL;
	m_pTabInfo = NULL;
}

CSongPropPageManager::~CSongPropPageManager()
{
	RELEASE( m_pIPropSheet );

	if( m_pTabSong )
	{
		delete m_pTabSong;
	}

	if( m_pTabInfo )
	{
		delete m_pTabInfo;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongPropPageManager::RemoveCurrentObject

void CSongPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongPropPageManager IUnknown implementation

HRESULT CSongPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CSongPropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CSongPropPageManager::Release()
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
// CSongPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CSongPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CSongPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_SONG_TEXT );

	CDirectMusicSong *pSong;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pSong))) )
	{
		CString strNodeName;
		BSTR bstrNodeName;

		if( SUCCEEDED ( pSong->GetNodeName( &bstrNodeName ) ) )
		{
			strNodeName = bstrNodeName;
			::SysFreeString( bstrNodeName );
		}
		strTitle = strNodeName + _T(" ") + strTitle;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CSongPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	// Add Song tab
	m_pTabSong = new CTabSong( this );
	if( m_pTabSong )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabSong->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Info tab
	m_pTabInfo = new CTabInfo( this );
	if( m_pTabInfo )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabInfo->m_psp );
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
// CSongPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CSongPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CSongPropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CSongPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
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
// CSongPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CSongPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CSongPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CSongPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CSongPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CSongPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicSong* pSong;
	
	if( m_pIPropPageObject == NULL )
	{
		pSong = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pSong ) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current Song are processed in OnKillFocus
	// messages before setting the new Song
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabSong->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new Song
	m_pTabSong->SetSong( pSong);
	m_pTabInfo->SetSong( pSong);

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CSongPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_SongPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CNotificationHandler constructor/destructor

CNotificationHandler::CNotificationHandler()
{
	m_pSong = NULL;
}

CNotificationHandler::~CNotificationHandler()
{
	m_pSong = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CNotificationHandler

BEGIN_MESSAGE_MAP(CNotificationHandler, CWnd)
	//{{AFX_MSG_MAP(CNotificationHandler)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEXT_VIRTUAL_SEGMENT, OnNextVirtualSegment)
	ON_MESSAGE(WM_STOP_VIRTUAL_SEGMENT, OnStopVirtualSegment)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNotificationHandler::OnNextVirtualSegment

LRESULT CNotificationHandler::OnNextVirtualSegment( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(wParam);

	BSTR bstrName = (BSTR)lParam;
	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( m_pSong )
	{
//		TRACE( "\nNEXT = %s ", strName );

		CVirtualSegment* pVirtualSegment = m_pSong->FindVirtualSegmentByName( strName );
		if( pVirtualSegment )
		{
			CVirtualSegment* pVirtualSegmentList;
			for( int i = 0 ;  pVirtualSegmentList = m_pSong->IndexToVirtualSegment(i) ;  i++ )
			{
				if( pVirtualSegmentList == NULL )
				{
					break;
				}

				if( pVirtualSegmentList == pVirtualSegment )
				{
					pVirtualSegmentList->m_fIsPlaying = true;
				}
				else
				{
					pVirtualSegmentList->m_fIsPlaying = false;
				}
			}

			if( m_pSong )
			{
				m_pSong->SyncChanges( SC_EDITOR,
									  SSE_INVALIDATE_VIRTUAL_SEGMENT_LIST,
									  NULL );
			}

//			TRACE( " %x", pVirtualSegment );
		}
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CNotificationHandler::OnStopVirtualSegment

LRESULT CNotificationHandler::OnStopVirtualSegment( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(wParam);

	BSTR bstrName = (BSTR)lParam;
	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( m_pSong )
	{
//		TRACE( "\nSTOP = %s ", strName );

		CVirtualSegment* pVirtualSegment = m_pSong->FindVirtualSegmentByName( strName );
		if( pVirtualSegment )
		{
			pVirtualSegment->m_fIsPlaying = false;

			if( m_pSong )
			{
				m_pSong->SyncChanges( SC_EDITOR,
									  SSE_INVALIDATE_VIRTUAL_SEGMENT_LIST,
									  NULL );
			}

//			TRACE( " %x", pVirtualSegment );
		}
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong constructor/destructor

CDirectMusicSong::CDirectMusicSong()
{
	ASSERT( theApp.m_pSongComponent != NULL );

	// Song needs Song Component
	theApp.m_pSongComponent->AddRef();

    m_dwRef = 0;
	AddRef();

	m_pUndoMgr = NULL;
	m_fModified = FALSE;

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pIContainerNode = NULL;
	m_pIAudioPathNode = NULL;
	m_pIDMSong = NULL;
	m_pIDMSegmentState = NULL;
	m_rpIDMStoppedSegmentState = NULL;
	m_pIDMTransitionSegmentState = NULL;
	m_fInTransition = false;
	m_dwTransitionPlayFlags = 0;
	m_pIDMTransitionSegment = NULL;

	m_fDeletingContainer = false;
	m_fChangingFolder = false;

	m_FolderSegments.m_pSong = this;
	m_FolderSegments.m_FolderEmbed.m_pSong = this;
	m_FolderSegments.m_FolderReference.m_pSong = this;
	m_FolderToolGraphs.m_pSong = this;

	m_hWndEditor = NULL;
	m_pSongCtrl = NULL;
	m_pINodeBeingDeleted = NULL;
	m_pIProject = NULL;

	m_dwSongFlagsDM = 0;
	m_dwStartSegID = 0;

// Song info block
	TCHAR achName[SMALL_BUFFER];
	::LoadString( theApp.m_hInstance, IDS_SONG_TEXT, achName, SMALL_BUFFER );
    m_strName.Format( "%s%d", achName, ++theApp.m_pSongComponent->m_nNextSong );

	CoCreateGuid( &m_guidSong ); 
	m_vVersion.dwVersionMS = 0x00010000;
	m_vVersion.dwVersionLS = 0x00000000;

	InitializeCriticalSection( &m_csSegmentState );

	// Create the notification window
	RECT rect;
	rect.top = 0;
	rect.bottom = 0;
	rect.left = 0;
	rect.right = 0;
	m_wndNotificationHandler.m_pSong = this;
	m_wndNotificationHandler.Create( NULL, _T("Hidden song notification handler"), WS_CHILD, rect, CWnd::FromHandle(GetDesktopWindow()), 0 );
}

CDirectMusicSong::~CDirectMusicSong()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Remove Song from clipboard
	theApp.FlushClipboard( this );

	// Remove Song from property sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}

	// Cleanup the Undo Manager
	if( m_pUndoMgr )
	{
		delete m_pUndoMgr;
	}

	// Empty VirtualSegment list
	EmptyVirtualSegmentList();

	// Cleanup references
	RELEASE( m_pIDMTransitionSegmentState );
	RELEASE( m_pIDMTransitionSegment );
	
	EnterCriticalSection( &m_csSegmentState );
	RELEASE( m_pIDMSegmentState );
	LeaveCriticalSection( &m_csSegmentState );

	// Make sure that Containers cleanup properly.  Otherwise
	// Container that happens to be in the Framework's notification
	// list will never have its destructor called.
	if( m_pIContainerNode )
	{
		m_fDeletingContainer = true;
		m_pIContainerNode->DeleteNode( FALSE );
		m_fDeletingContainer = false;
	}

	RELEASE( m_pIContainerNode );
	RELEASE( m_pIAudioPathNode );

	if( m_pIDMSong )
	{
		IDirectMusicObjectP* pIDMObjectP;
		if( SUCCEEDED ( m_pIDMSong->QueryInterface( IID_IDirectMusicObjectP, (void**)&pIDMObjectP ) ) )
		{
			pIDMObjectP->Zombie();
			RELEASE( pIDMObjectP );
		}
	}
	RELEASE( m_pIDMSong );

	// Don't release m_rpIDMStoppedSegmentState, it's just a reference

	// Song no longer needs Song Component
	theApp.m_pSongComponent->Release();

	m_wndNotificationHandler.DestroyWindow();

	DeleteCriticalSection( &m_csSegmentState );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::OnNewContainer

HRESULT CDirectMusicSong::OnNewContainer( void )
{
	IDMUSProdNode* pIContainerNode;
	IDMUSProdDocType* pIDocType;

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Do not call if Song already has a container!
	ASSERT( m_pIContainerNode == NULL );
	if( m_pIContainerNode )
	{
		return S_OK;
	}

	// Create the Container
	HRESULT hr = theApp.m_pSongComponent->m_pIFramework8->FindDocTypeByNodeId( GUID_ContainerNode, &pIDocType );
	if( SUCCEEDED ( hr ) ) 
	{
		hr = pIDocType->AllocNode( GUID_ContainerNode, &pIContainerNode );
		if( SUCCEEDED ( hr ) )
		{
			// Set name
			CString strName;
			strName.LoadString( IDS_CONTAINER_TEXT );
			BSTR bstrName = strName.AllocSysString();
			pIContainerNode->SetNodeName( bstrName );

			InsertChildNode( pIContainerNode );
			pIContainerNode->Release();
		}

		RELEASE( pIDocType );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::OnNewAudioPath

HRESULT CDirectMusicSong::OnNewAudioPath( void )
{
	IDMUSProdNode* pIAudioPathNode;
	IDMUSProdDocType* pIDocType;

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Do not call if Song already has a AudioPath!
	ASSERT( m_pIAudioPathNode == NULL );
	if( m_pIAudioPathNode )
	{
		return S_OK;
	}

	// Create the AudioPath
	HRESULT hr = theApp.m_pSongComponent->m_pIFramework8->FindDocTypeByNodeId( GUID_AudioPathNode, &pIDocType );
	if( SUCCEEDED ( hr ) ) 
	{
		hr = pIDocType->AllocNode( GUID_AudioPathNode, &pIAudioPathNode );
		if( SUCCEEDED ( hr ) )
		{
			// Set name
			CString strName;
			strName.LoadString( IDS_AUDIOPATH_TEXT );
			BSTR bstrName = strName.AllocSysString();
			pIAudioPathNode->SetNodeName( bstrName );

			InsertChildNode( pIAudioPathNode );
			pIAudioPathNode->Release();
		}

		RELEASE( pIDocType );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::CreateUndoMgr

BOOL CDirectMusicSong::CreateUndoMgr()
{
	// Should only be called once - after Song first created
	ASSERT( m_pUndoMgr == NULL );

	ASSERT( theApp.m_pSongComponent != NULL ); 
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL ); 

	m_pUndoMgr = new CJazzUndoMan( theApp.m_pSongComponent->m_pIFramework8 );
	if( m_pUndoMgr )
	{
		m_pUndoMgr->SetStreamAttributes( FT_DESIGN, GUID_CurrentVersion );
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::SyncChanges

void CDirectMusicSong::SyncChanges( DWORD dwFlags, DWORD dwEditorFlags, IDMUSProdPropPageObject* pIPropPageObject )
{
	CWaitCursor wait;

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Editor
	if( dwFlags & SC_EDITOR )
	{
		if( m_pSongCtrl
		&&  m_pSongCtrl->m_pSongDlg )
		{
			ASSERT( dwEditorFlags != 0 );
			m_pSongCtrl->m_pSongDlg->SyncSongEditor( dwEditorFlags );
		}
	}

	// Property sheet
	if( dwFlags & SC_PROPERTIES )
	{
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropPageObject )
			{
				pIPropSheet->RefreshTitleByObject( pIPropPageObject );
				pIPropSheet->RefreshActivePageByObject( pIPropPageObject );
			}
			else
			{
				pIPropSheet->RefreshTitle();
				pIPropSheet->RefreshActivePage();
			}
			RELEASE( pIPropSheet );
		}
	}
	
	// DirectMusic
	if( dwFlags & SC_DIRECTMUSIC )
	{
		if( m_pIDMSong == NULL )
		{
			if( FAILED ( ::CoCreateInstance( CLSID_DirectMusicSong, NULL, CLSCTX_INPROC_SERVER,
											 IID_IDirectMusicSong, (void**)&m_pIDMSong ) ) )
			{
				RELEASE( m_pIDMSong );
			}
		}

		if( m_pIDMSong )
		{
			IStream* pIMemStream;
			if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream ) ) )
			{
				if( SUCCEEDED ( Save( pIMemStream, FALSE ) ) )
				{
					IPersistStream* pIPersistStream;
					m_pIDMSong->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
					if( pIPersistStream )
					{
						StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
						HRESULT hr = pIPersistStream->Load( pIMemStream );
						ASSERT( (hr == DMUS_E_NOT_INIT)  ||  (SUCCEEDED(hr)) );

						RELEASE( pIPersistStream );
					}
				}

				RELEASE( pIMemStream );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::SetModified

void CDirectMusicSong::SetModified( BOOL fModified )
{
	m_fModified = fModified;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IUnknown implementation

HRESULT CDirectMusicSong::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IDMUSProdNode *)this;
    }
    else if( ::IsEqualIID(riid, IID_IPersist) )
    {
        *ppvObj = (IPersist *)this;
    }
    else if( ::IsEqualIID(riid, IID_IPersistStream) )
    {
        *ppvObj = (IPersistStream *)this;
    }
    else if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        *ppvObj = (IDMUSProdPropPageObject *)this;
    }
	else if( ::IsEqualIID(riid, IID_IDMUSProdTransport) )
	{
		*ppvObj = (IDMUSProdTransport*) this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdSecondaryTransport) )
	{
		*ppvObj = (IDMUSProdSecondaryTransport*) this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdNotifySink) )
	{
		*ppvObj = (IDMUSProdNotifySink*) this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdSortNode) )
	{
		*ppvObj = (IDMUSProdSortNode*) this;
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CDirectMusicSong::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CDirectMusicSong::Release()
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
// CDirectMusicSong IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetNodeImageIndex

HRESULT CDirectMusicSong::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return( theApp.m_pSongComponent->GetSongImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetFirstChild

HRESULT CDirectMusicSong::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	m_FolderSegments.AddRef();
	*ppIFirstChildNode = (IDMUSProdNode *)&m_FolderSegments;

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetNextChild

HRESULT CDirectMusicSong::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINextChildNode == NULL )
	{
		return E_POINTER;
	}
	*ppINextChildNode = NULL;

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	if( pIChildNode == (IDMUSProdNode *)&m_FolderSegments )
	{
		*ppINextChildNode = (IDMUSProdNode *)&m_FolderToolGraphs;
	}
	else if( pIChildNode == (IDMUSProdNode *)&m_FolderToolGraphs )
	{
		if( m_pIContainerNode )
		{
			*ppINextChildNode = (IDMUSProdNode *)m_pIContainerNode;
		}
		else if( m_pIAudioPathNode )
		{
			*ppINextChildNode = (IDMUSProdNode *)m_pIAudioPathNode;
		}
	}
	else if( pIChildNode == m_pIContainerNode )
	{
		if( m_pIAudioPathNode )
		{
			*ppINextChildNode = m_pIAudioPathNode;
		}
	}

	if( *ppINextChildNode )
	{
		(*ppINextChildNode)->AddRef();
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetComponent

HRESULT CDirectMusicSong::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return theApp.m_pSongComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetDocRootNode

HRESULT CDirectMusicSong::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode )
	{
		m_pIDocRootNode->AddRef();
		*ppIDocRootNode = m_pIDocRootNode;
		return S_OK;
	}

	*ppIDocRootNode = NULL;
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::SetDocRootNode

HRESULT CDirectMusicSong::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetParentNode

HRESULT CDirectMusicSong::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIParentNode == NULL )
	{
		return E_POINTER;
	}

	*ppIParentNode = m_pIParentNode;

	if( m_pIParentNode )
	{
		m_pIParentNode->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::SetParentNode

HRESULT CDirectMusicSong::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetNodeId

HRESULT CDirectMusicSong::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_SongNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetNodeName

HRESULT CDirectMusicSong::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetNodeNameMaxLength

HRESULT CDirectMusicSong::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = DMUS_MAX_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::ValidateNodeName

HRESULT CDirectMusicSong::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;

	strName = bstrName;
	::SysFreeString( bstrName );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::SetNodeName

HRESULT CDirectMusicSong::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( m_strName == strName )
	{
		return S_OK;
	}

	m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_SONG_NAME );
	m_strName = strName;

	// Sync changes
	SetModified( TRUE );
	SyncChanges( SC_PROPERTIES | SC_DIRECTMUSIC,
		   	     0,
			    (IDMUSProdPropPageObject *)this );

	// Notify connected nodes that Song name has changed
	UpdateTransportName();
	theApp.m_pSongComponent->m_pIFramework8->NotifyNodes( this, SONG_NameChange, NULL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetNodeListInfo

HRESULT CDirectMusicSong::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strDescriptor;

    pListInfo->bstrName = m_strName.AllocSysString();
    pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
	memcpy( &pListInfo->guidObject, &m_guidSong, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetEditorClsId

HRESULT CDirectMusicSong::GetEditorClsId( CLSID* pClsId )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

   *pClsId = CLSID_SongEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetEditorTitle

HRESULT CDirectMusicSong::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	::LoadString( theApp.m_hInstance, IDS_SONG_TEXT, achTitle, MID_BUFFER );
	strTitle  = achTitle;
	strTitle += _T(": " );
	strTitle += m_strName;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetEditorWindow

HRESULT CDirectMusicSong::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::SetEditorWindow

HRESULT CDirectMusicSong::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::UseOpenCloseImages

HRESULT CDirectMusicSong::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetRightClickMenuId

HRESULT CDirectMusicSong::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_SONG_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::OnRightClickMenuInit

HRESULT CDirectMusicSong::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	::EnableMenuItem( hMenu, IDM_NEW_AUDIOPATH, m_pIAudioPathNode ? (MF_GRAYED | MF_BYCOMMAND) : (MF_ENABLED | MF_BYCOMMAND) );
	::EnableMenuItem( hMenu, IDM_NEW_CONTAINER, m_pIContainerNode ? (MF_GRAYED | MF_BYCOMMAND) : (MF_ENABLED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::OnRightClickMenuSelect

HRESULT CDirectMusicSong::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_NEW_AUDIOPATH:
			hr = OnNewAudioPath();
			if( m_pIAudioPathNode )
			{
				theApp.m_pSongComponent->m_pIFramework8->OpenEditor( m_pIAudioPathNode );
			}
			break;

		case IDM_NEW_CONTAINER:
			hr = OnNewContainer();
			break;

		case IDM_NEW_SOURCE_SEGMENT:
			hr = m_FolderSegments.m_FolderReference.InsertChildNode( NULL );
			break;

		case IDM_NEW_SOURCE_TOOLGRAPH:
			hr = m_FolderToolGraphs.InsertChildNode( NULL );
			break;

		case IDM_RENAME:
			hr = theApp.m_pSongComponent->m_pIFramework8->EditNodeLabel( this );
			break;

		case IDM_PROPERTIES:
			OnShowProperties();
			hr = S_OK;
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::DeleteChildNode

HRESULT CDirectMusicSong::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if(	m_fDeletingContainer )
	{
		// Nothing to do
		return S_OK;
	}


	CWaitCursor wait;

	if( pIChildNode == NULL 
	|| !(pIChildNode == m_pIContainerNode 
		 || pIChildNode == m_pIAudioPathNode) )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// Remove node from Project Tree
	if( theApp.m_pSongComponent->m_pIFramework8->RemoveNode( pIChildNode, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Remove from Song
	if( pIChildNode == m_pIContainerNode )
	{
		// Make sure that Containers cleanup properly.  Otherwise
		// Container that happens to be in the Framework's notification
		// list will never have its destructor called.
		if( m_pIContainerNode )
		{
			m_fDeletingContainer = true;
			m_pIContainerNode->DeleteNode( FALSE );
			m_fDeletingContainer = false;
		}
		RELEASE( m_pIContainerNode );
	}
	else if( pIChildNode == m_pIAudioPathNode )
	{
		RELEASE( m_pIAudioPathNode );
	}

	// Sync changes
	SetModified( TRUE );
	SyncChanges( SC_DIRECTMUSIC, 0, NULL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::InsertChildNode

HRESULT CDirectMusicSong::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	pIChildNode->AddRef();

	// Songs can only have one Container or AudioPath
	GUID guidNode;
	if( FAILED( pIChildNode->GetNodeId( &guidNode ) ) )
	{
		return E_INVALIDARG;
	}

	if( guidNode == GUID_ContainerNode )
	{
		if( m_pIContainerNode )
		{
			DeleteChildNode( m_pIContainerNode, FALSE );
		}

		// Set the Segment's Container
		ASSERT( m_pIContainerNode == NULL );
		m_pIContainerNode = pIChildNode;

		// Set root and parent node of ALL children
		theApp.SetNodePointers( m_pIContainerNode, (IDMUSProdNode *)this, (IDMUSProdNode *)this );

		// Add node to Project Tree
		if( !SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->AddNode(m_pIContainerNode, (IDMUSProdNode *)this) ) )
		{
			DeleteChildNode( m_pIContainerNode, FALSE );
			return E_FAIL;
		}
	}
	else if( guidNode == GUID_AudioPathNode )
	{
		if( m_pIAudioPathNode )
		{
			DeleteChildNode( m_pIAudioPathNode, FALSE );
		}

		// Set the Segment's Audio Path
		ASSERT( m_pIAudioPathNode == NULL );
		m_pIAudioPathNode = pIChildNode;

		// Set root and parent node of ALL children
		theApp.SetNodePointers( m_pIAudioPathNode, (IDMUSProdNode *)this, (IDMUSProdNode *)this );

		// Add node to Project Tree
		if( !SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->AddNode(m_pIAudioPathNode, (IDMUSProdNode *)this) ) )
		{
			DeleteChildNode( m_pIAudioPathNode, FALSE );
			return E_FAIL;
		}
	}
	else
	{
		return E_INVALIDARG;
	}

	// Sync changes
	SetModified( TRUE );
	SyncChanges( SC_DIRECTMUSIC, 0, NULL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::DeleteNode

HRESULT CDirectMusicSong::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;

	// No parent so we will delete ourself
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Remove from Project Tree
	if( theApp.m_pSongComponent->m_pIFramework8->RemoveNode( this, fPromptUser ) == S_FALSE )
	{
		// User cancelled delete
		return E_FAIL;
	}

	// Song no longer needs Transport Controls
	UnRegisterWithTransport();

	// Make sure that Containers cleanup properly.  Otherwise
	// Container that happens to be in the Framework's notification
	// list will never have its destructor called.
	if( m_pIContainerNode )
	{
		m_fDeletingContainer = true;
		m_pIContainerNode->DeleteNode( FALSE );
		m_fDeletingContainer = false;
	}

	// Release the Song's container and AudioPath
	RELEASE( m_pIContainerNode );
	RELEASE( m_pIAudioPathNode );

	// Remove from Component Song list
	theApp.m_pSongComponent->RemoveFromSongFileList( this );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::OnNodeSelChanged

HRESULT CDirectMusicSong::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( fSelected )
	{
		SetActiveTransport();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::CreateDataObject

HRESULT CDirectMusicSong::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

	// Save Song into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_SONG into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pSongComponent->m_cfSong, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->SaveClipFormat( theApp.m_pSongComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// Song nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pSongComponent->m_cfProducerFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			RELEASE( pIStream );
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		RELEASE( pDataObject );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::CanCut

HRESULT CDirectMusicSong::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::CanCopy

HRESULT CDirectMusicSong::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::CanDelete

HRESULT CDirectMusicSong::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::CanDeleteChildNode

HRESULT CDirectMusicSong::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIChildNode == NULL )
	{
		return E_POINTER;
	}

	if( pIChildNode == m_pIContainerNode 
	||  pIChildNode == m_pIAudioPathNode )
	{
		return S_OK;
	}

	return S_FALSE;	
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::CanPasteFromData

HRESULT CDirectMusicSong::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	if( pDataObject->IsClipFormatAvailable(pIDataObject, theApp.m_pSongComponent->m_cfContainer) == S_OK 
	||  pDataObject->IsClipFormatAvailable(pIDataObject, theApp.m_pSongComponent->m_cfAudioPath) == S_OK )
	{
		RELEASE( pDataObject );
		return S_OK;
	}
	RELEASE( pDataObject );

	// Delegate to parent
	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
		}
	}
	
	return S_FALSE; 	
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::PasteFromData

HRESULT CDirectMusicSong::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfContainer ) ) )
	{
		// Handle CF_CONTAINER 
		hr = PasteCF_CONTAINER( pDataObject, pIDataObject );
	}
	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfAudioPath ) ) )
	{
		// Handle CF_AUDIOPATH 
		hr = PasteCF_AUDIOPATH( pDataObject, pIDataObject );
	}

	RELEASE( pDataObject );
	
	if( hr == S_OK )
	{
		return hr;
	}

	// Let parent handle paste
	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::CanChildPasteFromData

HRESULT CDirectMusicSong::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												  BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	GUID guidNodeId;
	pIChildNode->GetNodeId( &guidNodeId );

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfContainer ) ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_ContainerNode ) )
		{
			hr = S_OK;
		}
	}
	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfAudioPath ) ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_AudioPathNode ) )
		{
			hr = S_OK;
		}
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::ChildPasteFromData

HRESULT CDirectMusicSong::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL
	||  pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfContainer ) ) )
	{
		// Handle CF_CONTAINER format
		hr = PasteCF_CONTAINER( pDataObject, pIDataObject );
	}
	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfAudioPath ) ) )
	{
		// Handle CF_AUDIOPATH format
		hr = PasteCF_AUDIOPATH( pDataObject, pIDataObject );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNode::GetObject

HRESULT CDirectMusicSong::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Must be asking for an interface to a CLSID_DMSong object
	if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicSong ) )
	{
		if( m_pIDMSong )
		{
			return m_pIDMSong->QueryInterface( riid, ppvObject );
		}
	}

    *ppvObject = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdPropPageObject::GetData

HRESULT CDirectMusicSong::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	*ppData = this;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdPropPageObject::SetData

HRESULT CDirectMusicSong::SetData( void* pData )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdPropPageObject::OnShowProperties

HRESULT CDirectMusicSong::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Get the Song page manager
	CSongPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_SongPropPageManager ) == S_OK )
	{
		pPageManager = (CSongPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CSongPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Song properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CSongPropPageManager::sm_nActiveTab;

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
// CDirectMusicSong IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CDirectMusicSong::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IPersist::GetClassID

HRESULT CDirectMusicSong::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IPersistStream::IsDirty

HRESULT CDirectMusicSong::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fModified )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::LoadSong

HRESULT CDirectMusicSong::LoadSong( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*      pIStream;
	MMCKINFO	  ck;
	MMCKINFO	  ckList;
	DWORD		  dwByteCount;
	DWORD		  dwSize;
	DWORD		  dwPos;
    HRESULT       hr = S_OK;

	ASSERT( theApp.m_pSongComponent != NULL );

	// Determine if Song is in Project Tree
	bool fIsInProjectTree = false;
	GUID guidFile;
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->GetNodeFileGUID( this, &guidFile ) ) )
	{
		fIsInProjectTree = true;
	}

	// Clear the Song to prepare for load
	{
		// Remove pointers from Song editor (when open)
		SyncChanges( SC_EDITOR, SSE_EMPTY_ALL_LISTS, NULL );

		// Remove folders from Project Tree
		if( fIsInProjectTree )
		{
			theApp.m_pSongComponent->m_pIFramework8->RemoveNode( (IDMUSProdNode* )&m_FolderToolGraphs, FALSE );
			theApp.m_pSongComponent->m_pIFramework8->RemoveNode( (IDMUSProdNode* )&m_FolderSegments.m_FolderEmbed, FALSE );
			theApp.m_pSongComponent->m_pIFramework8->RemoveNode( (IDMUSProdNode* )&m_FolderSegments.m_FolderReference, FALSE );
		}

		// Empty lists
		EmptyVirtualSegmentList();
		m_FolderToolGraphs.EmptyToolGraphList();
		m_FolderSegments.m_FolderEmbed.EmptySourceSegmentList();
		m_FolderSegments.m_FolderReference.EmptySourceSegmentList();

		// Following strings only saved when they have values
		// So make sure they are initialized!!
		m_strAuthor.Empty();
		m_strCopyright.Empty();
		m_strSubject.Empty();
		m_strInfo.Empty();
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_SONG_CHUNK:
			{
			    DMUS_IO_SONG_HEADER dmusSongIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_SONG_HEADER ) );
				hr = pIStream->Read( &dmusSongIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwSongFlagsDM = dmusSongIO.dwFlags;
				m_dwStartSegID = dmusSongIO.dwStartSegID;
				if( m_dwStartSegID == DMUS_SONG_NOSEG )
				{
					m_dwStartSegID = 0;
				}
				break;
			}

			case DMUS_FOURCC_GUID_CHUNK:
				dwSize = min( ck.cksize, sizeof( GUID ) );
				hr = pIStream->Read( &m_guidSong, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;

			case DMUS_FOURCC_VERSION_CHUNK:
			{
			    DMUS_IO_VERSION dmusVersionIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_VERSION ) );
				hr = pIStream->Read( &dmusVersionIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_vVersion.dwVersionMS = dmusVersionIO.dwVersionMS;
				m_vVersion.dwVersionLS = dmusVersionIO.dwVersionLS;
				break;
			}

			case FOURCC_RIFF:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_CONTAINER_FORM: 
					{
						IDMUSProdRIFFExt* pIRIFFExt;
						IDMUSProdNode* pINode;

						hr = theApp.m_pSongComponent->m_pIContainerComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
						RELEASE( pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
						if( m_pIContainerNode )
						{
							InsertChildNode( pINode);
							pINode->Release();
						}
						else
						{
							m_pIContainerNode = pINode;
						}
						break;
					}

					case DMUS_FOURCC_AUDIOPATH_FORM: 
					{
						IDMUSProdRIFFExt* pIRIFFExt;
						IDMUSProdNode* pINode;

						hr = theApp.m_pSongComponent->m_pIAudioPathComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
						RELEASE( pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
						if( m_pIAudioPathNode )
						{
							InsertChildNode( pINode);
							pINode->Release();
						}
						else
						{
							m_pIAudioPathNode = pINode;
						}
						break;
					}
				}
				break;

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_SONGSEGMENTS_LIST:
						hr = m_FolderSegments.LoadSegments( pIRiffStream, &ck );
						if( FAILED ( hr ) )
						{
							goto ON_ERROR;
						}
						break;

					case DMUS_FOURCC_TOOLGRAPHS_LIST:
						hr = m_FolderToolGraphs.LoadToolGraphs( pIRiffStream, &ck );
						if( FAILED ( hr ) )
						{
							goto ON_ERROR;
						}
						break;

					case DMUS_FOURCC_SEGREFS_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case FOURCC_LIST:
									switch( ckList.fccType )
									{
										case DMUS_FOURCC_SEGREF_LIST:
										{
											CVirtualSegment* pVirtualSegment = new CVirtualSegment( this );
											if( pVirtualSegment == NULL )
											{
												hr = E_OUTOFMEMORY;
												goto ON_ERROR;
											}

											hr = pVirtualSegment->Load( pIRiffStream, &ck );
											if( FAILED ( hr ) )
											{
												delete pVirtualSegment;
												goto ON_ERROR;
											}

											// Add to VirtualSegment list
											InsertVirtualSegment( pVirtualSegment, -1 );
											RELEASE( pVirtualSegment );
											break;
										}
									}
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_UNAM_CHUNK:
								{
									CString strName;

									ReadMBSfromWCS( pIStream, ckList.cksize, &strName );
									if( strName.CompareNoCase( m_strName ) != 0 )
									{
										m_strName = strName;
										theApp.m_pSongComponent->m_pIFramework8->RefreshNode( this );

										// Notify connected nodes that the Song name has changed
										UpdateTransportName();
										theApp.m_pSongComponent->m_pIFramework8->NotifyNodes( this, SONG_NameChange, NULL );
									}
									break;
								}

								case DMUS_FOURCC_UART_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strAuthor );
									break;

								case DMUS_FOURCC_UCOP_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strCopyright );
									break;

								case DMUS_FOURCC_USBJ_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strSubject );
									break;

								case DMUS_FOURCC_UCMT_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strInfo );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
	    dwPos = StreamTell( pIStream );
    }

	ResolveVirtualSegmentIDs();

	if( fIsInProjectTree )
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( (IDMUSProdNode* )&m_FolderToolGraphs, (IDMUSProdNode *)this, (IDMUSProdNode *)this );
		theApp.SetNodePointers( (IDMUSProdNode* )&m_FolderSegments.m_FolderEmbed, (IDMUSProdNode *)this, (IDMUSProdNode *)&m_FolderSegments );
		theApp.SetNodePointers( (IDMUSProdNode* )&m_FolderSegments.m_FolderReference, (IDMUSProdNode *)this, (IDMUSProdNode *)&m_FolderSegments );

		// Place folders back in Project Tree
		theApp.m_pSongComponent->m_pIFramework8->AddNode( (IDMUSProdNode* )&m_FolderToolGraphs, (IDMUSProdNode *)this );
		theApp.m_pSongComponent->m_pIFramework8->AddNode( (IDMUSProdNode* )&m_FolderSegments.m_FolderEmbed, (IDMUSProdNode *)&m_FolderSegments );
		theApp.m_pSongComponent->m_pIFramework8->AddNode( (IDMUSProdNode* )&m_FolderSegments.m_FolderReference, (IDMUSProdNode *)&m_FolderSegments );
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IPersistStream::Load

HRESULT CDirectMusicSong::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	// Generate EmbeddedFile list
	DWORD dwCurrentFilePos = StreamTell( pIStream );
	theApp.m_pSongComponent->CreateEmbeddedFileList( pIStream );
	StreamSeek( pIStream, dwCurrentFilePos, 0 );

	// Load the Song
    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    HRESULT hr = E_FAIL;

    DWORD dwPos = StreamTell( pIStream );

	// Check for Direct Music format
	BOOL fFoundFormat = FALSE;
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_SONG_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			fFoundFormat = TRUE;
			hr = LoadSong( pIRiffStream, &ckMain );
		}

		RELEASE( pIRiffStream );
	}

	// Sync UI
	if( m_pIDocRootNode )
	{
		// Already in Project Tree so sync changes
		// Refresh Song editor (when open)
		SyncChanges( SC_EDITOR | SC_PROPERTIES,
					 SSE_ALL,
					 NULL );
	}

	//	Persist Song to the DirectMusic DLLs.
	if( SUCCEEDED( hr ) )
	{
		SyncChanges( SC_DIRECTMUSIC, 0, NULL );
	}

 	// Release EmbeddedFile list
	theApp.m_pSongComponent->ReleaseEmbeddedFileList();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong SaveInfoList
    
HRESULT CDirectMusicSong::SaveInfoList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;

	if( m_strName.IsEmpty()
	&&  m_strAuthor.IsEmpty()
	&&  m_strCopyright.IsEmpty()
	&&  m_strSubject.IsEmpty() 
	&&  m_strInfo.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write UNFO LIST header
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Song name
	if( !m_strName.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Song author
	if( !m_strAuthor.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UART_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strAuthor );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Song copyright
	if( !m_strCopyright.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UCOP_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strCopyright );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Song subject
	if( !m_strSubject.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_USBJ_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strSubject );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Song comments
	if( !m_strInfo.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UCMT_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strInfo );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong SaveVersion
    
HRESULT CDirectMusicSong::SaveVersion( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_VERSION dmusVersionIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Version chunk header
	ck.ckid = DMUS_FOURCC_VERSION_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_VERSION structure
	memset( &dmusVersionIO, 0, sizeof(DMUS_IO_VERSION) );

	dmusVersionIO.dwVersionMS = m_vVersion.dwVersionMS ;
	dmusVersionIO.dwVersionLS = m_vVersion.dwVersionLS;

	// Write Version chunk data
	hr = pIStream->Write( &dmusVersionIO, sizeof(DMUS_IO_VERSION), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_VERSION) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong SaveGUID
    
HRESULT CDirectMusicSong::SaveGUID( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write GUID chunk header
	ck.ckid = DMUS_FOURCC_GUID_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Song GUID
	hr = pIStream->Write( &m_guidSong, sizeof(GUID), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(GUID) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong SaveHeader
    
HRESULT CDirectMusicSong::SaveHeader( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_SONG_HEADER dmusSongIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Song chunk header
	ck.ckid = DMUS_FOURCC_SONG_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_SONG_HEADER structure
	memset( &dmusSongIO, 0, sizeof(DMUS_IO_SONG_HEADER) );

	dmusSongIO.dwFlags = m_dwSongFlagsDM;
	dmusSongIO.dwStartSegID = m_dwStartSegID;

	// Write Song chunk data
	hr = pIStream->Write( &dmusSongIO, sizeof(DMUS_IO_SONG_HEADER), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_SONG_HEADER) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong SaveSong
    
HRESULT CDirectMusicSong::SaveSong( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty )
{
	HRESULT hr = S_OK;
	POSITION pos;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

// Save Song header chunk
	hr = SaveHeader( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Song GUID
	hr = SaveGUID( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Song version
	hr = SaveVersion( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Song info
	hr = SaveInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Song's Container
	if( m_pIContainerNode )
	{
		IPersistStream* pIPersistStream;

		hr = m_pIContainerNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
		hr = pIPersistStream->Save( pIStream, fClearDirty );
		RELEASE( pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
	}

// Save Song's SourceSegments
	hr = m_FolderSegments.SaveSegments( pIRiffStream );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

// Save Song's SourceToolGraphs
	hr = m_FolderToolGraphs.SaveToolGraphs( pIRiffStream, fClearDirty );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

// Save Song's AudioPath
	if( m_pIAudioPathNode )
	{
		IPersistStream* pIPersistStream;

		hr = m_pIAudioPathNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
		hr = pIPersistStream->Save( pIStream, fClearDirty );
		RELEASE( pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
	}

// Save Song's VirtualSegments
	{
		// Write DMUS_FOURCC_SEGREFS_LIST header
		MMCKINFO ck;
		ck.fccType = DMUS_FOURCC_SEGREFS_LIST;
		if( pIRiffStream->CreateChunk(&ck, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		pos = m_lstVirtualSegments.GetHeadPosition();
		while( pos )
		{
			CVirtualSegment* pVirtualSegment = m_lstVirtualSegments.GetNext( pos );

			pVirtualSegment->Save( pIRiffStream );
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong SaveSelectedVirtualSegments
    
HRESULT CDirectMusicSong::SaveSelectedVirtualSegments( IDMUSProdRIFFStream* pIRiffStream )
{
	HRESULT hr = S_OK;
	POSITION pos;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

// Save Song GUID
	hr = SaveGUID( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save selected VirtualSegments
	{
		// Write DMUS_FOURCC_SEGREFS_LIST header
		MMCKINFO ck;
		ck.fccType = DMUS_FOURCC_SEGREFS_LIST;
		if( pIRiffStream->CreateChunk(&ck, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		pos = m_lstVirtualSegments.GetHeadPosition();
		while( pos )
		{
			CVirtualSegment* pVirtualSegment = m_lstVirtualSegments.GetNext( pos );

			if( pVirtualSegment->m_dwBitsUI & BF_SELECTED )
			{
				pVirtualSegment->Save( pIRiffStream );
			}
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IPersistStream::Save

HRESULT CDirectMusicSong::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pPersistInfo );
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

    HRESULT hr = E_FAIL;

	// Validate requested data format
	if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion )
	||  ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_DirectMusicObject ) )
	{
		// DirectMusic format
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_SONG_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( SaveSong( pIRiffStream, fClearDirty ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				if( fClearDirty )
				{
					SetModified( FALSE );
				}

				hr = S_OK;
			}
			RELEASE( pIRiffStream );
		}
	}
	else if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_SaveSelectedVirtualSegments ) )
	{
		// Copy/Paste (Save selected virtual segments)
		// Do not reset dirty flag
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_SONG_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( SaveSelectedVirtualSegments( pIRiffStream ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				hr = S_OK;
			}
			RELEASE( pIRiffStream );
		}
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IPersistStream::GetSizeMax

HRESULT CDirectMusicSong::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNotifySink implementation

///////////////////////////////////////////////////////////////////////////
// CDirectMusicSong  IDMUSProdNotifySink::OnUpdate

HRESULT CDirectMusicSong::OnUpdate( IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDocRootNode == m_pIContainerNode )
	{
		// CONTAINER_ChangeNotification
		if( ::IsEqualGUID(guidUpdateType, CONTAINER_ChangeNotification ) )
		{
			SetModified( TRUE );
			// No need to sync segment with DMusic
			// because containers do not affect performance
			return S_OK;
		}

		// CONTAINER_FileLoadFinished
		if( ::IsEqualGUID(guidUpdateType, CONTAINER_FileLoadFinished ) )
		{
			// No need to sync segment with DMusic
			// because containers do not affect performance
			return S_OK;
		}
	}
	else if( pIDocRootNode == m_pIAudioPathNode )
	{
		// AUDIOPATH_DirtyNotification
		if( ::IsEqualGUID(guidUpdateType, AUDIOPATH_DirtyNotification ) )
		{
			// Sync changes
			SetModified( TRUE );
			SyncChanges( SC_DIRECTMUSIC, 0, NULL );
			return S_OK;
		}
	}
	else if( pIDocRootNode == this )
	{
		// FRAMEWORK_FileNameChange
		if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileNameChange ) )
		{
			UpdateTransportName();
			return S_OK;
		}

		// FRAMEWORK_AfterFileSave
		if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_AfterFileSave ) )
		{
			UpdateTransportName();
			return S_OK;
		}
	}

	// Check SourceSegments folder
	m_FolderSegments.OnUpdate( pIDocRootNode, guidUpdateType, pData );

	// Check ToolGraphs folder
	m_FolderToolGraphs.OnUpdate( pIDocRootNode, guidUpdateType, pData );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong Additional functions

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::ReadListInfoFromStream

HRESULT CDirectMusicSong::ReadListInfoFromStream( IStream* pIStream, DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;
	CString strName;
	CString strDescriptor;
	GUID guidSong;

	ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	memset( &guidSong, 0, sizeof(GUID) );

    DWORD dwPos = StreamTell( pIStream );

	// Check for Direct Music format
	BOOL fFoundFormat = FALSE;
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_SONG_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			fFoundFormat = TRUE;

			// Get Song GUID
			ck.ckid = DMUS_FOURCC_GUID_CHUNK;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
				dwSize = min( ck.cksize, sizeof( GUID ) );
				if( FAILED ( pIStream->Read( &guidSong, dwSize, &dwByteCount ) )
				||  dwByteCount != dwSize )
				{
					memset( &guidSong, 0, sizeof(GUID) );
				}
			}

			// Get Song name
			ck.fccType = DMUS_FOURCC_UNFO_LIST;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDLIST ) == 0 )
			{
				ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
				if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
				{
					ReadMBSfromWCS( pIStream, ck.cksize, &strName );
				}
			}
		}

		RELEASE( pIRiffStream );
	}

	if( !strName.IsEmpty() )
	{
		pListInfo->bstrName = strName.AllocSysString();
		pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
		memcpy( &pListInfo->guidObject, &guidSong, sizeof(GUID) );

		// Must check pListInfo->wSize before populating additional fields
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::GetObjectDescriptor

HRESULT CDirectMusicSong::GetObjectDescriptor( void* pObjectDesc )
{
	// Make sure method was passed a valid DMUS_OBJECTDESC pointer
	if( pObjectDesc == NULL ) 
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	DMUS_OBJECTDESC *pDMObjectDesc = (DMUS_OBJECTDESC *)pObjectDesc;

	if( pDMObjectDesc->dwSize == 0 ) 
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	}

	// Initialize DMUS_OBJECTDESC structure
	DWORD dwOrigSize = pDMObjectDesc->dwSize;
	memset( pDMObjectDesc, 0, dwOrigSize );
	pDMObjectDesc->dwSize = dwOrigSize;

	// Set values in DMUS_OBJECTDESC structure
	pDMObjectDesc->dwValidData = (DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS | DMUS_OBJ_VERSION | DMUS_OBJ_NAME );
	
	memcpy( &pDMObjectDesc->guidObject, &m_guidSong, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicSong, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = m_vVersion.dwVersionMS;
	pDMObjectDesc->vVersion.dwVersionLS = m_vVersion.dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, m_strName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdSortNode implementation

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdSortNode::CompareNodes

HRESULT CDirectMusicSong::CompareNodes( IDMUSProdNode* pINode1, IDMUSProdNode* pINode2, int* pnResult )
{
	ASSERT(pINode1);
	ASSERT(pINode2);
	
	if( pINode1 == NULL
	||  pINode2 == NULL )
	{
		return E_INVALIDARG;
	}

	if( pnResult == NULL )
	{
		return E_POINTER;
	}
	*pnResult = 0;

	GUID guidNodeId1;
	pINode1->GetNodeId( &guidNodeId1 );

	GUID guidNodeId2;
	pINode2->GetNodeId( &guidNodeId2 );

	if( ::IsEqualGUID(guidNodeId1, GUID_Song_SegmentsFolderNode) )
	{
		// Source Segment folder always first
		*pnResult = -1;
	}
	else if( ::IsEqualGUID(guidNodeId1, GUID_Song_ToolGraphsFolderNode) )
	{
		if( ::IsEqualGUID(guidNodeId2, GUID_ContainerNode) 
		||  ::IsEqualGUID(guidNodeId2, GUID_AudioPathNode) )
		{
			*pnResult = -1;
		}
		else
		{
			*pnResult = 1;
		}
	}
	else if( ::IsEqualGUID(guidNodeId1, GUID_ContainerNode) )
	{
		if( ::IsEqualGUID(guidNodeId2, GUID_AudioPathNode) )
		{
			*pnResult = -1;
		}
		else
		{
			*pnResult = 1;
		}
	}
	else if( ::IsEqualGUID(guidNodeId1, GUID_AudioPathNode) )
	{
		// AudioPath always last
		*pnResult = 1;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::PasteCF_CONTAINER

HRESULT CDirectMusicSong::PasteCF_CONTAINER( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pSongComponent->m_cfContainer, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new Container
		IDMUSProdRIFFExt* pIRIFFExt;
		IDMUSProdNode* pINode;

		if( SUCCEEDED ( theApp.m_pSongComponent->m_pIContainerComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
		{
			hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
			if( SUCCEEDED ( hr ) )
			{
				InsertChildNode( pINode );
				RELEASE( pINode );
			}

			RELEASE( pIRIFFExt );
		}

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::PasteCF_AUDIOPATH

HRESULT CDirectMusicSong::PasteCF_AUDIOPATH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pSongComponent->m_cfAudioPath, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new AudioPath
		IDMUSProdRIFFExt* pIRIFFExt;
		IDMUSProdNode* pINode;

		if( SUCCEEDED ( theApp.m_pSongComponent->m_pIAudioPathComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
		{
			hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
			if( SUCCEEDED ( hr ) )
			{
				InsertChildNode( pINode );
				RELEASE( pINode );
			}

			RELEASE( pIRIFFExt );
		}

		RELEASE( pIStream );
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::EmptyVirtualSegmentList

void CDirectMusicSong::EmptyVirtualSegmentList( void )
{
	CVirtualSegment* pVirtualSegment;

	while( !m_lstVirtualSegments.IsEmpty() )
	{
		pVirtualSegment = m_lstVirtualSegments.GetHead();

		RemoveVirtualSegment( pVirtualSegment );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::InsertVirtualSegment

void CDirectMusicSong::InsertVirtualSegment( CVirtualSegment* pVirtualSegmentToInsert, int nIndex )
{
	if( pVirtualSegmentToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Make sure the VirtualSegment has a unique name
	GetUniqueVirtualSegmentName( pVirtualSegmentToInsert );

	pVirtualSegmentToInsert->AddRef();

	// Place VirtualSegment in m_lstVirtualSegments
	POSITION pos = m_lstVirtualSegments.FindIndex( nIndex );
	if( pos )
	{
		m_lstVirtualSegments.InsertBefore( pos, pVirtualSegmentToInsert );
		return;
	}

	m_lstVirtualSegments.AddTail( pVirtualSegmentToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::RemoveVirtualSegment

void CDirectMusicSong::RemoveVirtualSegment( CVirtualSegment* pVirtualSegmentToRemove )
{
	if( pVirtualSegmentToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Make sure other VirtualSegments do not point to pVirtualSegmentToRemove
	POSITION pos = m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{
		CVirtualSegment* pVirtualSegment = m_lstVirtualSegments.GetNext( pos );

		pVirtualSegment->RemoveVirtualSegmentReferences( pVirtualSegmentToRemove );
	}

	// If item is in m_lstVirtualSegments, remove it
	pos = m_lstVirtualSegments.Find( pVirtualSegmentToRemove );
	if( pos )
	{
		m_lstVirtualSegments.RemoveAt( pos );

		RELEASE( pVirtualSegmentToRemove );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::RemoveToolGraphReferences

void CDirectMusicSong::RemoveToolGraphReferences( IDMUSProdNode* pIToolGraphNodeToRemove )
{
	if( pIToolGraphNodeToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Make sure other VirtualSegments do not point to pIToolGraphNodeToRemove
	POSITION pos = m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{
		CVirtualSegment* pVirtualSegment = m_lstVirtualSegments.GetNext( pos );
		
		pVirtualSegment->RemoveToolGraphReferences( pIToolGraphNodeToRemove );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::RemoveSourceSegmentReferences

void CDirectMusicSong::RemoveSourceSegmentReferences( CSourceSegment* pSourceSegmentToRemove )
{
	if( pSourceSegmentToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Make sure other VirtualSegments do not point to pSourceSegmentToRemove
	POSITION pos = m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{
		CVirtualSegment* pVirtualSegment = m_lstVirtualSegments.GetNext( pos );

		pVirtualSegment->RemoveSourceSegmentReferences( pSourceSegmentToRemove );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::GetSegmentEmbedFolder
	
CFolder* CDirectMusicSong::GetSegmentEmbedFolder( void )
{
	return &m_FolderSegments.m_FolderEmbed;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::GetSegmentRefFolder

CFolder* CDirectMusicSong::GetSegmentRefFolder( void )
{
	return &m_FolderSegments.m_FolderReference;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::IsSourceSegmentInSong

BOOL CDirectMusicSong::IsSourceSegmentInSong( IDMUSProdNode* pIDocRootNode )
{
	if( m_FolderSegments.m_FolderEmbed.IsDocRootInFolder( pIDocRootNode )
	||  m_FolderSegments.m_FolderReference.IsDocRootInFolder( pIDocRootNode ) )
	{
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::GetSourceSegmentFromDocRoot

CSourceSegment* CDirectMusicSong::GetSourceSegmentFromDocRoot( IDMUSProdNode* pIDocRootNode )
{
	CSourceSegment* pTheSourceSegment = NULL;

	pTheSourceSegment = m_FolderSegments.m_FolderEmbed.GetSourceSegmentFromDocRoot( pIDocRootNode );
	if( pTheSourceSegment == NULL )
	{
		pTheSourceSegment = m_FolderSegments.m_FolderReference.GetSourceSegmentFromDocRoot( pIDocRootNode );
	}

	return pTheSourceSegment;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::IsVirtualSegmentNameUnique

BOOL CDirectMusicSong::IsVirtualSegmentNameUnique( CVirtualSegment* pVirtualSegment )
{
	CString strNameList;
	CString strName;
	pVirtualSegment->GetName( strName );

	POSITION pos = m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{
		CVirtualSegment* pVirtualSegmentList = m_lstVirtualSegments.GetNext( pos );
		if( pVirtualSegmentList != pVirtualSegment )
		{
			pVirtualSegmentList->GetName( strNameList );

			if( strNameList.CompareNoCase( strName ) == 0 )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::GetUniqueVirtualSegmentName

void CDirectMusicSong::GetUniqueVirtualSegmentName( CVirtualSegment* pVirtualSegment )
{
	CString	strOrigName;
	int	nOrigRootLength;
	int	nPrefixSuffixLength;

	CString strName;
	pVirtualSegment->GetName( strName );

	if( strName.IsEmpty() )
	{
		strName.LoadString( IDS_V_EMPTY_TEXT );
	}
	ASSERT( strName.IsEmpty() == FALSE );

	if( strName.GetLength() > DMUS_MAX_NAME )
	{
		strName = strName.Left( DMUS_MAX_NAME );
		strName.TrimRight();
	}

	strOrigName = strName;

	// Determine prefix - root name - suffix
	CString strPrefix;
	CString strRootName;
	CString strSuffix;
	{
		strRootName = strOrigName;

		// Prefix
		strPrefix.LoadString( IDS_VIRTUAL_SEGMENT_PREFIX_TEXT ); 
		int nFindPos = strRootName.Find( strPrefix );
		if( nFindPos == 0 )
		{
			strRootName = strRootName.Right( strRootName.GetLength() - strPrefix.GetLength() );
		}
		else
		{
			strPrefix.Empty();
		}

		// Root name
		CString strSourceSegmentName;
		pVirtualSegment->GetSourceSegmentName( strSourceSegmentName );
		if( strSourceSegmentName.IsEmpty() )
		{
			strSourceSegmentName.LoadString( IDS_EMPTY_TEXT );
		}
		nFindPos = strRootName.Find( strSourceSegmentName );
		if( nFindPos == 0 )
		{
			strSuffix = strRootName.Right( strRootName.GetLength() - strSourceSegmentName.GetLength() );
			strRootName = strSourceSegmentName;
		}
		else
		{
			nFindPos = strRootName.ReverseFind( (TCHAR)'_' );
			if( nFindPos != -1 )
			{
				strSuffix = strRootName.Right( strRootName.GetLength() - nFindPos );
				strRootName = strRootName.Left( nFindPos );
			}
		}

		// Suffix
		nFindPos = strSuffix.ReverseFind( (TCHAR)'_' );
		if( nFindPos == -1 )
		{
			strRootName += strSuffix;
			strSuffix.Empty();
		}
		else if( nFindPos > 0 )
		{
			strRootName += strSuffix.Left( nFindPos );
			strSuffix = strSuffix.Right( strSuffix.GetLength() - nFindPos );
		}
	}

	nOrigRootLength = strRootName.GetLength();

	while( IsVirtualSegmentNameUnique( pVirtualSegment ) == FALSE )
	{
		// Name is not unique
		if( strSuffix.IsEmpty() ) 
		{
			strSuffix  = (TCHAR)'_';
			strSuffix += (TCHAR)'a';
		}
		else
		{
			ASSERT( strSuffix[0] == (TCHAR)'_' );

			// Increment the existing suffix
			for( int i = (strSuffix.GetLength() - 1) ;  i >= 0 ;  i-- )
			{
				if( i == 0 )
				{
					strSuffix += (TCHAR) 'a';
					break;
				}

				char ch = strSuffix[i];
				ch += 1;

				if( _istalpha(ch) )
				{
					strSuffix.SetAt( i, ch );
					break;
				}

				strSuffix.SetAt( i, (TCHAR)'a' );
			}
		}

		// Set the new name
		nPrefixSuffixLength = strPrefix.GetLength() + strSuffix.GetLength();
		if( (nOrigRootLength + nPrefixSuffixLength) <= DMUS_MAX_NAME )
		{
			pVirtualSegment->SetName( strPrefix + strRootName + strSuffix );
		}
		else
		{
			pVirtualSegment->SetName( strPrefix + strRootName.Left(DMUS_MAX_NAME - nPrefixSuffixLength) + strSuffix );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::GUIDToVirtualSegment

CVirtualSegment* CDirectMusicSong::GUIDToVirtualSegment( GUID guidVirtualSegment )
{
	CVirtualSegment* pTheVirtualSegment = NULL;

	POSITION pos = m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{
		CVirtualSegment* pVirtualSegment = m_lstVirtualSegments.GetNext( pos );

		if( ::IsEqualGUID( pVirtualSegment->m_VirtualSegmentUI.guidVirtualSegment, guidVirtualSegment ) )
		{
			pTheVirtualSegment = pVirtualSegment;
			break;
		}
	}

	return pTheVirtualSegment;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::IndexToVirtualSegment

CVirtualSegment* CDirectMusicSong::IndexToVirtualSegment( int nVirtualSegmentIndex )
{
	CVirtualSegment* pTheVirtualSegment = NULL;

	int nNbrVirtualSegments = m_lstVirtualSegments.GetCount();

	if( nVirtualSegmentIndex >= 0
	&&  nVirtualSegmentIndex < nNbrVirtualSegments )
	{
		POSITION pos = m_lstVirtualSegments.FindIndex( nVirtualSegmentIndex );
		if( pos )
		{
			pTheVirtualSegment = m_lstVirtualSegments.GetAt( pos );
		}
	}

	return pTheVirtualSegment;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::FindVirtualSegmentByName

CVirtualSegment* CDirectMusicSong::FindVirtualSegmentByName( LPCTSTR pszName )
{
	CVirtualSegment* pTheVirtualSegment = NULL;
	CString strNameList;

	POSITION pos = m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{
		CVirtualSegment* pVirtualSegmentList = m_lstVirtualSegments.GetNext( pos );

		pVirtualSegmentList->GetName( strNameList );

		if( strNameList.CompareNoCase( pszName ) == 0 )
		{
			pTheVirtualSegment = pVirtualSegmentList;
			break;
		}
	}

	return pTheVirtualSegment;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::PtrToVirtualSegment

CVirtualSegment* CDirectMusicSong::PtrToVirtualSegment( CVirtualSegment* pVirtualSegment )
{
	CVirtualSegment* pTheVirtualSegment = NULL;

	POSITION pos = m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{
		CVirtualSegment* pVirtualSegmentList = m_lstVirtualSegments.GetNext( pos );

		if( pVirtualSegmentList == pVirtualSegment )
		{
			pTheVirtualSegment = pVirtualSegmentList;
			break;
		}
	}

	return pTheVirtualSegment;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::RecomputeVirtualSegmentLengths

void CDirectMusicSong::RecomputeVirtualSegmentLengths( void )
{
	BOOL fChanged = FALSE;

	POSITION pos = m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{
		CVirtualSegment* pVirtualSegment = m_lstVirtualSegments.GetNext( pos );

		fChanged |= pVirtualSegment->RecomputeLength();
	}

	if( fChanged )
	{
		SyncChanges( SC_PROPERTIES | SC_DIRECTMUSIC,
		 			 0,
					 NULL );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::VirtualSegmentToIndex

int CDirectMusicSong::VirtualSegmentToIndex( CVirtualSegment* pVirtualSegment )
{
	int nTheIndex = 0;

	if( pVirtualSegment )
	{
		int nIndex = 0;

		POSITION pos = m_lstVirtualSegments.GetHeadPosition();
		while( pos )
		{
			CVirtualSegment* pVirtualSegmentList = m_lstVirtualSegments.GetNext( pos );

			if( pVirtualSegmentList == pVirtualSegment )
			{
				nTheIndex = nIndex;
				break;
			}

			nIndex++;
		}

	}

	return nTheIndex;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::IndexToSourceSegment

CSourceSegment* CDirectMusicSong::IndexToSourceSegment( int nSourceSegmentIndex )
{
	CSourceSegment* pTheSourceSegment = NULL;

	int nNbrSourceSegments = m_FolderSegments.m_FolderEmbed.m_lstSegments.GetCount();

	if( nSourceSegmentIndex >= 0
	&&  nSourceSegmentIndex < nNbrSourceSegments )
	{
		POSITION pos = m_FolderSegments.m_FolderEmbed.m_lstSegments.FindIndex( nSourceSegmentIndex );
		if( pos )
		{
			pTheSourceSegment = m_FolderSegments.m_FolderEmbed.m_lstSegments.GetAt( pos );
		}
	}
	else
	{
		nSourceSegmentIndex -= nNbrSourceSegments;

		nNbrSourceSegments = m_FolderSegments.m_FolderReference.m_lstSegments.GetCount();

		if( nSourceSegmentIndex >= 0
		&&  nSourceSegmentIndex < nNbrSourceSegments )
		{
			POSITION pos = m_FolderSegments.m_FolderReference.m_lstSegments.FindIndex( nSourceSegmentIndex );
			if( pos )
			{
				pTheSourceSegment = m_FolderSegments.m_FolderReference.m_lstSegments.GetAt( pos );
			}
		}
	}

	return pTheSourceSegment;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::SourceSegmentToIndex

int CDirectMusicSong::SourceSegmentToIndex( CSourceSegment* pSourceSegment )
{
	int nTheIndex = -1;

	if( pSourceSegment )
	{
		int nIndex = 0;

		POSITION pos = m_FolderSegments.m_FolderEmbed.m_lstSegments.GetHeadPosition();
		while( pos )
		{
			CSourceSegment* pSourceSegmentList = m_FolderSegments.m_FolderEmbed.m_lstSegments.GetNext( pos );

			if( pSourceSegmentList == pSourceSegment )
			{
				nTheIndex = nIndex;
				break;
			}

			nIndex++;
		}

		if( nTheIndex == -1 )
		{
			POSITION pos = m_FolderSegments.m_FolderReference.m_lstSegments.GetHeadPosition();
			while( pos )
			{
				CSourceSegment* pSourceSegmentList = m_FolderSegments.m_FolderReference.m_lstSegments.GetNext( pos );

				if( pSourceSegmentList == pSourceSegment )
				{
					nTheIndex = nIndex;
					break;
				}

				nIndex++;
			}
		}
	}

	return nTheIndex;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::PtrToSourceSegment

CSourceSegment* CDirectMusicSong::PtrToSourceSegment( CSourceSegment* pSourceSegment )
{
	CSourceSegment* pTheSourceSegment = NULL;

	POSITION pos = m_FolderSegments.m_FolderEmbed.m_lstSegments.GetHeadPosition();
	while( pos )
	{
		CSourceSegment* pSourceSegmentList = m_FolderSegments.m_FolderEmbed.m_lstSegments.GetNext( pos );

		if( pSourceSegmentList == pSourceSegment )
		{
			pTheSourceSegment = pSourceSegmentList;
			break;
		}
	}

	if( pTheSourceSegment == NULL )
	{
		POSITION pos = m_FolderSegments.m_FolderReference.m_lstSegments.GetHeadPosition();
		while( pos )
		{
			CSourceSegment* pSourceSegmentList = m_FolderSegments.m_FolderReference.m_lstSegments.GetNext( pos );

			if( pSourceSegmentList == pSourceSegment )
			{
				pTheSourceSegment = pSourceSegmentList;
				break;
			}
		}
	}

	return pTheSourceSegment;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::IndexToToolGraph

IDMUSProdNode* CDirectMusicSong::IndexToToolGraph( int nToolGraphIndex )
{
	IDMUSProdNode* pITheToolGraphNode = NULL;

	int nNbrToolGraphs = m_FolderToolGraphs.m_lstToolGraphs.GetCount();

	if( nToolGraphIndex >= 0
	&&  nToolGraphIndex < nNbrToolGraphs )
	{
		POSITION pos = m_FolderToolGraphs.m_lstToolGraphs.FindIndex( nToolGraphIndex );
		if( pos )
		{
			pITheToolGraphNode = m_FolderToolGraphs.m_lstToolGraphs.GetAt( pos );
		}
	}

	return pITheToolGraphNode;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::ToolGraphToIndex

int CDirectMusicSong::ToolGraphToIndex( IDMUSProdNode* pIToolGraphNode )
{
	int nTheIndex = 0;

	if( pIToolGraphNode )
	{
		int nIndex = 0;

		POSITION pos = m_FolderToolGraphs.m_lstToolGraphs.GetHeadPosition();
		while( pos )
		{
			IDMUSProdNode* pIToolGraphNodelist = m_FolderToolGraphs.m_lstToolGraphs.GetNext( pos );

			if( pIToolGraphNodelist == pIToolGraphNode )
			{
				nTheIndex = nIndex;
				break;
			}

			nIndex++;
		}

	}

	return nTheIndex;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::PtrToToolGraph

IDMUSProdNode* CDirectMusicSong::PtrToToolGraph( IDMUSProdNode* pIToolGraphNode )
{
	IDMUSProdNode* pITheToolGraphNode = NULL;

	POSITION pos = m_FolderToolGraphs.m_lstToolGraphs.GetHeadPosition();
	while( pos )
	{
		IDMUSProdNode* pIToolGraphNodeList = m_FolderToolGraphs.m_lstToolGraphs.GetNext( pos );

		if( pIToolGraphNodeList == pIToolGraphNode )
		{
			pITheToolGraphNode = pIToolGraphNodeList;
			break;
		}
	}

	return pITheToolGraphNode;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::ResolveVirtualSegmentIDs

void CDirectMusicSong::ResolveVirtualSegmentIDs( void )
{
	POSITION pos = m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{
		CVirtualSegment* pVirtualSegment = m_lstVirtualSegments.GetNext( pos );

		pVirtualSegment->ResolveVirtualSegmentIDs();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::RegisterWithTransport

HRESULT CDirectMusicSong::RegisterWithTransport( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIConductor != NULL );

	// Register Song with Conductor's Secondary Transport
	if( FAILED ( theApp.m_pSongComponent->m_pIConductor->RegisterSecondaryTransport( this ) ) )
	{
		return E_FAIL;
	}

	// Register Song with Conductor's Transport and notification system
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIConductor->RegisterTransport( this, BS_AUTO_UPDATE ) )
	&&  SUCCEEDED ( theApp.m_pSongComponent->m_pIConductor->RegisterNotify( this, GUID_NOTIFICATION_SEGMENT ) ) )
	{
		SetActiveTransport();
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::UnRegisterWithTransport

HRESULT CDirectMusicSong::UnRegisterWithTransport( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIConductor != NULL );

	// Make sure Song is not playing
	Stop( TRUE );
	theApp.m_pSongComponent->m_pIConductor->TransportStopped( (IDMUSProdTransport*)this );

	// Unregister Song from Conductor's Secondary Transport's Play, Stop functionality.
	theApp.m_pSongComponent->m_pIConductor->UnRegisterSecondaryTransport( this );

	// Unregister Song from Conductor's Transport and notification system
	theApp.m_pSongComponent->m_pIConductor->UnRegisterTransport( this );
	theApp.m_pSongComponent->m_pIConductor->UnregisterNotify( this, GUID_NOTIFICATION_SEGMENT );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::SetActiveTransport

HRESULT CDirectMusicSong::SetActiveTransport( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( theApp.m_pSongComponent == NULL
	||  theApp.m_pSongComponent->m_pIConductor == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	theApp.m_pSongComponent->m_pIConductor->SetActiveTransport( (IDMUSProdTransport *)this, BS_AUTO_UPDATE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::UpdateTransportName

HRESULT CDirectMusicSong::UpdateTransportName( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( theApp.m_pSongComponent == NULL
	||  theApp.m_pSongComponent->m_pIConductor == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	BSTR bstrTransportName;
	if( SUCCEEDED ( GetName( &bstrTransportName ) ) )
	{
		theApp.m_pSongComponent->m_pIConductor->SetTransportName( (IDMUSProdTransport *)this, bstrTransportName );
		if( SUCCEEDED ( GetName( &bstrTransportName ) ) )
		{
			theApp.m_pSongComponent->m_pIConductor->SetSecondaryTransportName( (IDMUSProdSecondaryTransport *)this, bstrTransportName );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::TransitionWithinSong

HRESULT CDirectMusicSong::TransitionWithinSong( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDMSong == NULL )
	{
		return S_OK;
	}

	if( theApp.m_pSongComponent == NULL 
	||  theApp.m_pSongComponent->m_pIDMPerformance8 == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}
	
	HRESULT hr = E_FAIL;

	// Determine which VirtualSegment should play
	CVirtualSegment* pVirtualSegment = NULL;
	if( m_pSongCtrl
	&&  m_pSongCtrl->m_pSongDlg )
	{
		pVirtualSegment = m_pSongCtrl->m_pSongDlg->GetFirstSelectedVirtualSegment();
	}

	// Compose the Song
	if( m_pIDMSegmentState == NULL )
	{
		m_pIDMSong->Compose();
	}

	// Initialize states before starting transition
	RELEASE( m_pIDMTransitionSegmentState );

	EnterCriticalSection( &m_csSegmentState );
	RELEASE( m_pIDMSegmentState );
	LeaveCriticalSection( &m_csSegmentState );

	m_rpIDMStoppedSegmentState = NULL;

	// Do transition
	if( pVirtualSegment )
	{
		// Get name of VirtualSegment
		WCHAR awchVirtualSegmentName[DMUS_MAX_NAME+1];
		MultiByteToWideChar( CP_ACP, 0, pVirtualSegment->m_strName, -1, awchVirtualSegmentName, DMUS_MAX_NAME+1 );

		EnterCriticalSection( &m_csSegmentState );
		hr = theApp.m_pSongComponent->m_pIDMPerformance8->PlaySegmentEx( m_pIDMSong,
		  																 awchVirtualSegmentName,
																		 NULL,
																		 DMUS_SEGF_AUTOTRANSITION | pVirtualSegment->m_dwResolution_SEGH,
																		 0,
																		 &m_pIDMSegmentState,
																		 NULL,
																		 NULL );
		LeaveCriticalSection( &m_csSegmentState );
	}
	else
	{
		theApp.m_pSongComponent->m_pIDMPerformance8->StopEx( m_pIDMSong, 0, DMUS_SEGF_AUTOTRANSITION );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::GetTransitionOptions

DWORD CDirectMusicSong::TransitionOptionsToDMUS_SEGF( ConductorTransitionOptions* pTransitionOptions )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pTransitionOptions == NULL )
	{
		ASSERT( 0 );
		return 0;
	}

	// Translate options into DMUS_SEGF flags
	DWORD dwOptions = 0;

	// Set the Boundary settings (bar/beat/grid/immediate/end of segment)
	if( pTransitionOptions->dwFlags & TRANS_END_OF_SEGMENT )
	{
		dwOptions = DMUS_SEGF_QUEUE;
	}
	else if( pTransitionOptions->dwBoundaryFlags & DMUS_COMPOSEF_IMMEDIATE )
	{
		dwOptions = 0;
	}
	else if( pTransitionOptions->dwBoundaryFlags & DMUS_COMPOSEF_GRID )
	{
		dwOptions = DMUS_SEGF_GRID;
	}
	else if( pTransitionOptions->dwBoundaryFlags & DMUS_COMPOSEF_BEAT )
	{
		dwOptions = DMUS_SEGF_BEAT;
	}
	else if( pTransitionOptions->dwBoundaryFlags & DMUS_COMPOSEF_MEASURE )
	{
		dwOptions = DMUS_SEGF_MEASURE;
	}
	else if( pTransitionOptions->dwBoundaryFlags & DMUS_COMPOSEF_SEGMENTEND )
	{
		dwOptions = DMUS_SEGF_SEGMENTEND;
	}
	else
	{
		ASSERT( 0 );
		dwOptions = 0;
	}

	// Set alignment flags
	if( pTransitionOptions->dwBoundaryFlags & DMUS_COMPOSEF_ALIGN )
	{
		dwOptions |= DMUS_SEGF_ALIGN;

		if( pTransitionOptions->dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_TICK )
		{
			dwOptions |= DMUS_SEGF_VALID_START_TICK;
		}
		else if( pTransitionOptions->dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_GRID )
		{
			dwOptions |= DMUS_SEGF_VALID_START_GRID;
		}
		else if( pTransitionOptions->dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_BEAT )
		{
			dwOptions |= DMUS_SEGF_VALID_START_BEAT;
		}
	}

	// Set marker flags
	if( pTransitionOptions->dwBoundaryFlags & DMUS_COMPOSEF_MARKER )
	{
		dwOptions |= DMUS_SEGF_MARKER;
	}

	// Additional flags
	dwOptions |= DMUS_SEGF_AFTERPREPARETIME | DMUS_SEGF_NOINVALIDATE;

	return dwOptions;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdTransport implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::Play

HRESULT CDirectMusicSong::Play( BOOL fPlayFromStart )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDMSong == NULL )
	{
		return S_OK;
	}

	if( theApp.m_pSongComponent == NULL 
	||  theApp.m_pSongComponent->m_pIDMPerformance8 == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// If we're already playing, Stop()
	if( m_pIDMSegmentState != NULL )
	{
		Stop( TRUE );
	}

	ASSERT( m_pIDMSegmentState == NULL );
	ASSERT( m_pIDMTransitionSegmentState == NULL );

	// Determine which VirtualSegment should play first
	CVirtualSegment* pVirtualSegment = NULL;
	if( m_pSongCtrl
	&&  m_pSongCtrl->m_pSongDlg )
	{
		if( fPlayFromStart == FALSE )
		{
			pVirtualSegment = m_pSongCtrl->m_pSongDlg->GetFirstSelectedVirtualSegment();
		}
	}
	if( pVirtualSegment == NULL )
	{
		pVirtualSegment = IndexToVirtualSegment( 0 );
	}

	HRESULT hr = E_FAIL;
	
	if( pVirtualSegment )
	{
		// Get name of VirtualSegment
		WCHAR awchVirtualSegmentName[DMUS_MAX_NAME+1];
		MultiByteToWideChar( CP_ACP, 0, pVirtualSegment->m_strName, -1, awchVirtualSegmentName, DMUS_MAX_NAME+1 );

		// Determine PlaySegmentEx parameters pertaining to transitions
		DWORD dwFlags;
		IDirectMusicSegment* pIDMTransitionSegment;
		if( m_fInTransition )
		{
			pIDMTransitionSegment = m_pIDMTransitionSegment;
			dwFlags = m_dwTransitionPlayFlags;
		}
		else
		{
			pIDMTransitionSegment = NULL;
			dwFlags = (DMUS_SEGF_QUEUE | DMUS_SEGF_AFTERPREPARETIME | DMUS_SEGF_NOINVALIDATE);
		}

		// Compose the Song
		m_pIDMSong->Compose();

		// Play the Song
		EnterCriticalSection( &m_csSegmentState );
		hr = theApp.m_pSongComponent->m_pIDMPerformance8->PlaySegmentEx( m_pIDMSong,
		  																 awchVirtualSegmentName,
																		 pIDMTransitionSegment,
																		 dwFlags,
																		 0,
																		 &m_pIDMSegmentState,
																		 NULL,
																		 NULL );
		LeaveCriticalSection( &m_csSegmentState );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::Stop

HRESULT CDirectMusicSong::Stop( BOOL fStopImmediate )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDMSong == NULL )
	{
		return S_OK;
	}

	if( theApp.m_pSongComponent == NULL 
	||  theApp.m_pSongComponent->m_pIDMPerformance8 == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	MUSIC_TIME mtNow;
	REFERENCE_TIME rtNow;
	theApp.m_pSongComponent->m_pIDMPerformance8->GetTime( &rtNow, &mtNow );

	if( m_pIDMSegmentState == NULL
	&&  m_pIDMTransitionSegmentState == NULL )
	{
		if ( FAILED( theApp.m_pSongComponent->m_pIDMPerformance8->StopEx( m_pIDMSong,
																		  0,
																		  fStopImmediate ? 0 : DMUS_SEGF_DEFAULT ) ) )
		{
			return E_FAIL;
		}

//		TRACE("Song: Stop() called when SegmentState is NULL!\n");
//		TRACE("Song: Stopping playback at %ld | %I64d\n", mtNow, rtNow );
		return E_UNEXPECTED;
	}

	MUSIC_TIME mtStartTime = 0;
	bool fStoppedSomething = false;

	// Try and stop the transition segment state, if there is one
	if( m_pIDMTransitionSegmentState )
	{
		m_pIDMTransitionSegmentState->GetStartTime( &mtStartTime );

		if( SUCCEEDED( theApp.m_pSongComponent->m_pIDMPerformance8->Stop( NULL,
																		  m_pIDMTransitionSegmentState,
																		  0, 
																		  fStopImmediate ? 0 : DMUS_SEGF_DEFAULT ) ) )
		{
			fStoppedSomething = true;
		}

		RELEASE( m_pIDMTransitionSegmentState );
	}

	// Now stop the segment state
	EnterCriticalSection( &m_csSegmentState );
	if( m_pIDMSegmentState )
	{
		if( mtStartTime == 0 )
		{
			m_pIDMSegmentState->GetStartTime( &mtStartTime );
		}

		if( SUCCEEDED( theApp.m_pSongComponent->m_pIDMPerformance8->Stop( NULL,
																		  m_pIDMSegmentState,
																		  0, 
																		  fStopImmediate ? 0 : DMUS_SEGF_DEFAULT ) ) )
		{
			fStoppedSomething = true;
		}

		m_rpIDMStoppedSegmentState = m_pIDMSegmentState;
		RELEASE( m_pIDMSegmentState );
	}
	LeaveCriticalSection( &m_csSegmentState );

	if( fStoppedSomething )
	{
		// If we are asked to stop our segment state before it begins playing, return an error code because
		// it probably won't stop whatever is currently playing.
		if( mtNow < mtStartTime )
		{
			return E_UNEXPECTED;
		}
		else
		{
			return S_OK;
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::Transition

HRESULT CDirectMusicSong::Transition( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDMSong == NULL )
	{
		return S_OK;
	}

	if( theApp.m_pSongComponent == NULL 
	||  theApp.m_pSongComponent->m_pIConductor == NULL 
	||  theApp.m_pSongComponent->m_pIDMPerformance8 == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	HRESULT hr = S_OK;

	// Get transition options from Conductor
	ConductorTransitionOptions TransitionOptions;
	if( FAILED( theApp.m_pSongComponent->m_pIConductor->GetTransitionOptions( &TransitionOptions ) ) )
	{
		return E_NOTIMPL;
	}

	m_dwTransitionPlayFlags = 0;
	if( TransitionOptions.pDMUSProdNodeSegmentTransition )
	{
		IDMUSProdNode *pINode;
		if( SUCCEEDED( TransitionOptions.pDMUSProdNodeSegmentTransition->QueryInterface( IID_IDMUSProdNode, (void **)&pINode ) ) )
		{
			if( SUCCEEDED( pINode->GetObject( CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&m_pIDMTransitionSegment ) ) )
			{
				m_dwTransitionPlayFlags |= DMUS_SEGF_AUTOTRANSITION;
			}

			RELEASE( pINode );
		}
	}

	RELEASE( m_pIDMTransitionSegmentState );

	EnterCriticalSection( &m_csSegmentState );
	RELEASE( m_pIDMSegmentState );
	LeaveCriticalSection( &m_csSegmentState );

	m_rpIDMStoppedSegmentState = NULL;

	// Play the segment
	m_fInTransition = true;
	m_dwTransitionPlayFlags |= TransitionOptionsToDMUS_SEGF( &TransitionOptions );
	hr = Play( TRUE );
	m_fInTransition = false;
	m_dwTransitionPlayFlags = 0;

	RELEASE( m_pIDMTransitionSegment );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::Record

HRESULT CDirectMusicSong::Record( BOOL fPressed )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::GetName

HRESULT CDirectMusicSong::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

	// Song file name
	CString strName;
	BSTR bstrSongFileName;

	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->GetNodeFileName( this, &bstrSongFileName ) ) )
	{
		// Node is in the Project tree
		strName = bstrSongFileName;
		::SysFreeString( bstrSongFileName );
		int nFindPos = strName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strName = strName.Right( strName.GetLength() - nFindPos - 1 );
		}
		strName += _T(": ");
	}
	else
	{
		// Node has not yet been placed in the Project Tree
		// because the Song is still in the process of being loaded
		strName = m_strOrigFileName;
		strName += _T(": ");
	}
	
	// Plus "name"
	strName += m_strName;

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::TrackCursor

HRESULT CDirectMusicSong::TrackCursor( BOOL fTrackCursor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdSecondaryTransport implementation

// CDirectMusicSong::GetSecondaryTransportSegment

HRESULT CDirectMusicSong::GetSecondaryTransportSegment( IUnknown **ppunkSegment )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate ppunkSegment
	if( ppunkSegment == NULL )
	{
		return E_POINTER;
	}

	if( m_pIDMSong == NULL )
	{
		return E_FAIL;
	}

	return m_pIDMSong->QueryInterface( IID_IUnknown, (void**)ppunkSegment );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::GetSecondaryTransportName

HRESULT CDirectMusicSong::GetSecondaryTransportName( BSTR* pbstrName )
{
	return GetName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong IDMUSProdNotifyCPt implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::OnNotify

HRESULT CDirectMusicSong::OnNotify( ConductorNotifyEvent *pConductorNotifyEvent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( theApp.m_pSongComponent == NULL 
	||  theApp.m_pSongComponent->m_pIConductor == NULL )
	{
		return E_UNEXPECTED;
	}

	switch( pConductorNotifyEvent->m_dwType )
	{
		case DMUS_PMSGT_NOTIFICATION:
		{
			DMUS_NOTIFICATION_PMSG* pNotifyEvent = (DMUS_NOTIFICATION_PMSG *)pConductorNotifyEvent->m_pbData;

			EnterCriticalSection( &m_csSegmentState );

			// GUID_NOTIFICATION_SEGMENT 
			if( ::IsEqualGUID ( pNotifyEvent->guidNotificationType, GUID_NOTIFICATION_SEGMENT ) )
			{
				switch( pNotifyEvent->dwNotificationOption )
				{
					case DMUS_NOTIFICATION_SEGSTART:
//						TRACE( "\nSEGSTART");
						if( pNotifyEvent->punkUser )
						{
							// Make sure VirtualSegment is highlighted in editor
							IDirectMusicSegmentState *pITempSegmentState;
							if( SUCCEEDED( pNotifyEvent->punkUser->QueryInterface( IID_IDirectMusicSegmentState, (void**)&pITempSegmentState ) ) )
							{
								IDirectMusicSegment *pITempSegment;
								if( SUCCEEDED( pITempSegmentState->GetSegment( &pITempSegment ) ) )
								{
									IDirectMusicObject* pITempObject;
									if( SUCCEEDED ( pITempSegment->QueryInterface( IID_IDirectMusicObject, (void**)&pITempObject ) ) )
									{
										DMUS_OBJECTDESC dmusObjectDesc;
										memset( &dmusObjectDesc, 0, sizeof(DMUS_OBJECTDESC) );
										dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);

										if( SUCCEEDED ( pITempObject->GetDescriptor( &dmusObjectDesc ) ) )
										{
											if( dmusObjectDesc.dwValidData & DMUS_OBJ_NAME )
											{
												CString strName = dmusObjectDesc.wszName;
												BSTR bstrName = strName.AllocSysString();
												::PostMessage( m_wndNotificationHandler.GetSafeHwnd(), WM_NEXT_VIRTUAL_SEGMENT, 0, (LPARAM)bstrName );
											}
										}
										RELEASE( pITempObject );
									}
									RELEASE( pITempSegment );
								}
								RELEASE( pITempSegmentState );
							}
						}
						break;

					case DMUS_NOTIFICATION_SEGABORT:
					case DMUS_NOTIFICATION_SEGEND:
//						TRACE( "\nSEGEND");
						if( pNotifyEvent->punkUser )
						{
							// Make sure VirtualSegment is not highlighted in editor
							IDirectMusicSegmentState *pITempSegmentState;
							if( SUCCEEDED( pNotifyEvent->punkUser->QueryInterface( IID_IDirectMusicSegmentState, (void**)&pITempSegmentState ) ) )
							{
								IDirectMusicSegment *pITempSegment;
								if( SUCCEEDED( pITempSegmentState->GetSegment( &pITempSegment ) ) )
								{
									IDirectMusicObject* pITempObject;
									if( SUCCEEDED ( pITempSegment->QueryInterface( IID_IDirectMusicObject, (void**)&pITempObject ) ) )
									{
										DMUS_OBJECTDESC dmusObjectDesc;
										memset( &dmusObjectDesc, 0, sizeof(DMUS_OBJECTDESC) );
										dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);

										if( SUCCEEDED ( pITempObject->GetDescriptor( &dmusObjectDesc ) ) )
										{
											if( dmusObjectDesc.dwValidData & DMUS_OBJ_NAME )
											{
												CString strName = dmusObjectDesc.wszName;
												BSTR bstrName = strName.AllocSysString();
												::PostMessage( m_wndNotificationHandler.GetSafeHwnd(), WM_STOP_VIRTUAL_SEGMENT, 0, (LPARAM)bstrName );
											}
										}
										RELEASE( pITempObject );
									}
									RELEASE( pITempSegment );
								}
								RELEASE( pITempSegmentState );
							}

							if( pNotifyEvent->punkUser == m_pIDMSegmentState
							||  pNotifyEvent->punkUser == m_rpIDMStoppedSegmentState )
							{
								// Notify the Conductor we stopped playing
								theApp.m_pSongComponent->m_pIConductor->TransportStopped( (IDMUSProdTransport*) this );

								RELEASE( m_pIDMSegmentState );
								RELEASE( m_pIDMTransitionSegmentState );

								// Don't need this reference anymore
								m_rpIDMStoppedSegmentState = NULL;
							}
						}
						break;
				}
			}

			LeaveCriticalSection( &m_csSegmentState );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::GetGUID

void CDirectMusicSong::GetGUID( GUID* pguidSong )
{
	if( pguidSong )
	{
		*pguidSong = m_guidSong;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicSong::SetGUID

void CDirectMusicSong::SetGUID( GUID guidSong )
{
	m_guidSong = guidSong;

	// Sync changes
	SetModified( TRUE );
	SyncChanges( SC_DIRECTMUSIC, 0, NULL );

	// Notify connected nodes that Song GUID has changed
	theApp.m_pSongComponent->m_pIFramework8->NotifyNodes( this, DOCROOT_GuidChange, NULL );
}
