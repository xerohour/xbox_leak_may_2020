// AudioPath.cpp : implementation file
//

#include "stdafx.h"
#include "dsoundp.h"
#include "AudioPathDesignerDLL.h"

#include "RiffStrm.h"
#include "AudioPath.h"
#include "SegmentDesigner.h"
#include <mmreg.h>
#include <math.h>
#include <dmusicf.h>
#include "TabAudioPathAudioPath.h"
#include "TabAudioPathInfo.h"
#include "AudioPathCtl.h"
#include "AudioPathDlg.h"
#include "EffectListDlg.h"
#include "ItemInfo.h"
#include "EffectInfo.h"
#include "ToolGraphDesigner.h"
#define _SYS_GUID_OPERATORS_
#include "dmoreg.h"
#include "uuids.h"

// Define the WaveSame DMO GUID so we can strip it out for runtime saves
#include <initguid.h>
// {2D6D1411-DCD7-45e7-ADDE-ACAC85A2425D}
DEFINE_GUID(GUID_DSFX_STANDARD_DUMP, 0x2d6d1411, 0xdcd7, 0x45e7, 0xad, 0xde, 0xac, 0xac, 0x85, 0xa2, 0x42, 0x5d);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Keep around for importing pre-DX8 RC0 files.
static const GUID REMOVED_GUID_Buffer_3D= {0x186cc543, 0xdb29, 0x11d3, 0x9b, 0xd1, 0x0, 0x80, 0xc7, 0x15, 0xa, 0x74};

// {F078ACC0-67AA-11d3-B45D-00105A2796DE}
static const GUID GUID_AudioPathPropPageManager = 
{ 0xf078acc0, 0x67aa, 0x11d3, { 0xb4, 0x5d, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde } };

struct EnumEffectStruct
{
	GUID	guidEffect;
	DWORD	dwRefCount;
};

short CAudioPathPropPageManager::sm_nActiveTab = 0;

CLIPFORMAT CDirectMusicAudioPath::m_scfEffect = 0;
CLIPFORMAT CDirectMusicAudioPath::m_scfPChannel = 0;
CLIPFORMAT CDirectMusicAudioPath::m_scfBuffer = 0;

extern long GetLongFromStream( IStream *pStream );

// Old fileformat (DX8 beta 1) chunks
struct DX8_BETA1_DSOUND_IO_DSBUFFERDESC
{
	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;
    LONG lPan;          /* Initial pan; only used if CTRL_PAN is specified */
	WORD wReserved;
};

struct DX8_BETA1_DSOUND_IO_3D
{
    DS3DBUFFER ds3d;    /* Only used if DSBCAPS_CTRL3D is specified */
};

struct DX8_BETA1_DSOUND_IO_DXDMO_HEADER
{
	DSEFFECTDESC dsfxd;
};

bool ItemAndBufferSupportEffects( const ItemInfo *pItemInfo, const BufferOptions *pBufferOptions )
{
	if( pItemInfo
	&&	pBufferOptions
	&& ItemSupportsBuffers( pItemInfo )
	&&	!(pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED) )
	{
		return true;
	}
	return false;
}

bool ItemSupportsBuffers( const ItemInfo *pItemInfo )
{
	if( pItemInfo
	&&	(pItemInfo->nNumPChannels == 0 ||
		 pItemInfo->pPortOptions->m_fAudioPath) )
	{
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////
// CAudioPathPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioPathPropPageManager::CAudioPathPropPageManager() : CDllBasePropPageManager()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_GUIDManager	= GUID_AudioPathPropPageManager;

	m_pTabAudioPath = NULL;
	m_pTabInfo = NULL;
}

CAudioPathPropPageManager::~CAudioPathPropPageManager()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pTabAudioPath )
	{
		delete m_pTabAudioPath;
	}

	if( m_pTabInfo )
	{
		delete m_pTabInfo;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CAudioPathPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CAudioPathPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||	(pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_AUDIOPATH_TEXT );

	CDirectMusicAudioPath *pAudioPath;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pAudioPath))) )
	{
		CString strNodeName;
		BSTR bstrNodeName;

		if( SUCCEEDED ( pAudioPath->GetNodeName( &bstrNodeName ) ) )
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
// CAudioPathPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CAudioPathPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (hPropSheetPage == NULL)
	||	(pnNbrPages == NULL) )
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

	// Add AudioPath tab
	if( m_pTabAudioPath == NULL )
	{
		m_pTabAudioPath = new CTabAudioPathAudioPath( this );
	}
	if( m_pTabAudioPath )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabAudioPath->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Info tab
	if( m_pTabInfo == NULL )
	{
		m_pTabInfo = new CTabAudioPathInfo( this );
	}
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
// CAudioPathPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CAudioPathPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	if(m_pIPropSheet)
	{
		m_pIPropSheet->GetActivePage( &CAudioPathPropPageManager::sm_nActiveTab );
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
// CAudioPathPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CAudioPathPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
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
// CAudioPathPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CAudioPathPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = CDllBasePropPageManager::RemoveObject(pIPropPageObject);
	if(SUCCEEDED(hr))
	{
		RefreshData();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CAudioPathPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pTabAudioPath == NULL)
	{
		return S_OK;
	}

	CDirectMusicAudioPath* pAudioPath;
	
	if( m_pIPropPageObject == NULL )
	{
		pAudioPath = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pAudioPath ) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current AudioPath are processed in OnKillFocus
	// messages before setting the new AudioPath
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabAudioPath->GetSafeHwnd() ? m_pTabAudioPath->GetParent() : NULL;

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new AudioPath
	m_pTabAudioPath->SetAudioPath( pAudioPath);
	m_pTabInfo->SetAudioPath( pAudioPath);

	// Restore focus
	if( pWndHadFocus
	&&	pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath constructor/destructor

CDirectMusicAudioPath::CDirectMusicAudioPath()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );

	// AudioPath needs AudioPath Component
	theApp.m_pAudioPathComponent->AddRef();

	m_dwRef = 0;
	AddRef();

	m_pUndoMgr = NULL;
	m_fModified = FALSE;

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pIToolGraphNode = NULL;

	m_hWndEditor = NULL;
	m_pAudioPathCtrl = NULL;
	m_pINodeBeingDeleted = NULL;

	m_wSplitterPos = 0;
	m_wMixGroupWidth = 0;
	m_wBusWidth = 0;
	m_wBufferWidth = 0;
	m_wEffectWidth = 0;

// AudioPath info block
	TCHAR achName[SMALL_BUFFER];
	::LoadString( theApp.m_hInstance, IDS_AUDIOPATH_TEXT, achName, SMALL_BUFFER );
	m_strName.Format( "%s%d", achName, ++theApp.m_pAudioPathComponent->m_nNextAudioPath );

	CoCreateGuid( &m_guidAudioPath ); 
	m_vVersion.dwVersionMS = 0x00010000;
	m_vVersion.dwVersionLS = 0x00000000;

	CDirectMusicAudioPath::m_scfPChannel = static_cast<CLIPFORMAT>(::RegisterClipboardFormat( CF_AUDIOPATH_PCHANNEL ));
	CDirectMusicAudioPath::m_scfEffect = static_cast<CLIPFORMAT>(::RegisterClipboardFormat( CF_AUDIOPATH_EFFECT ));
	CDirectMusicAudioPath::m_scfBuffer = static_cast<CLIPFORMAT>(::RegisterClipboardFormat( CF_AUDIOPATH_BUFFER ));

	EnumeratePorts();
}

CDirectMusicAudioPath::~CDirectMusicAudioPath()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	// Clear out m_lstItems
	while( !m_lstItems.IsEmpty() )
	{
		delete m_lstItems.RemoveHead();
	}

	// Clear out m_lstPortOptions
	while( !m_lstPortOptions.IsEmpty() )
	{
		delete m_lstPortOptions.RemoveHead();
	}

	// Remove AudioPath from clipboard
	theApp.FlushClipboard( this );

	// Remove AudioPath from property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		if(theApp.m_pAudioPathComponent->m_pIAudioPathPageManager)
		{
			theApp.m_pAudioPathComponent->m_pIAudioPathPageManager->RemoveObject(this);
		}
		RELEASE( pIPropSheet );
	}

	if ( m_pIToolGraphNode )
	{
		m_pIToolGraphNode->Release();
		m_pIToolGraphNode = NULL;
	}

	// Cleanup the Undo Manager
	if( m_pUndoMgr )
	{
		delete m_pUndoMgr;
	}

	// Cleanup DirectMusic references
	while( !m_lstDMAudioPaths.IsEmpty() )
	{
		m_lstDMAudioPaths.RemoveHead()->Release();
	}

	// AudioPath no longer needs AudioPath Component
	theApp.m_pAudioPathComponent->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::CreateUndoMgr

BOOL CDirectMusicAudioPath::CreateUndoMgr()
{
	// Should only be called once - after AudioPath first created
	ASSERT( m_pUndoMgr == NULL );

	ASSERT( theApp.m_pAudioPathComponent != NULL ); 
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL ); 

	m_pUndoMgr = new CJazzUndoMan( theApp.m_pAudioPathComponent->m_pIFramework );
	if( m_pUndoMgr )
	{
		m_pUndoMgr->SetStreamAttributes( FT_DESIGN, GUID_CurrentVersion );
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::CreateEmptyToolGraph

HRESULT CDirectMusicAudioPath::CreateEmptyToolGraph( void )
{
	IDMUSProdNode* pIToolGraphNode;
	IDMUSProdDocType* pIDocType;

	ASSERT( theApp.m_pAudioPathComponent != NULL ); 
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL ); 

	// Do not call if Segment already has a tool graph!
	ASSERT( m_pIToolGraphNode == NULL );
	if( m_pIToolGraphNode )
	{
		return S_OK;
	}

	// Create the tool graph
	HRESULT hr = theApp.m_pAudioPathComponent->m_pIFramework->FindDocTypeByNodeId( GUID_ToolGraphNode, &pIDocType );
	if( SUCCEEDED ( hr ) ) 
	{
		hr = pIDocType->AllocNode( GUID_ToolGraphNode, &pIToolGraphNode );
		if( SUCCEEDED ( hr ) )
		{
			// Set name
			CString strName;
			strName.LoadString( IDS_TOOLGRAPH_TEXT );
			BSTR bstrName = strName.AllocSysString();
			pIToolGraphNode->SetNodeName( bstrName );

			InsertChildNode( pIToolGraphNode );
			pIToolGraphNode->Release();
		}

		RELEASE( pIDocType );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::IsInSegment

bool CDirectMusicAudioPath::IsInSegment( void )
{
	bool fInSegment = false;

	if( m_pIParentNode )
	{
		GUID guidNodeId;
		if( SUCCEEDED ( m_pIParentNode->GetNodeId( &guidNodeId ) ) )
		{
			if( ::IsEqualGUID( guidNodeId, GUID_SegmentNode ) )
			{
				fInSegment = true;
			}
		}
	}

	return fInSegment;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::SyncAudioPathEditor

void CDirectMusicAudioPath::SyncAudioPathEditor( void )
{
	if( m_pAudioPathCtrl )
	{
		if( m_pAudioPathCtrl->m_pAudioPathDlg )
		{
			m_pAudioPathCtrl->m_pAudioPathDlg->RefreshControls();
		}
		/* No need - effect list only contains the registered effects
		if( m_pAudioPathCtrl->m_pEffectListDlg )
		{
			m_pAudioPathCtrl->m_pEffectListDlg->RefreshControls();
		}
		*/
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::SyncAudiopathsInUse

void CDirectMusicAudioPath::SyncAudiopathsInUse( DWORD dwFlags )
{
	// Notify connected nodes that AudioPath has changed
	theApp.m_pAudioPathComponent->m_pIFramework->NotifyNodes( this, AUDIOPATH_NeedToRebuildNotification, &dwFlags );
}


////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::UpdateDMOs

void CDirectMusicAudioPath::UpdateDMOs( BusEffectInfo *pBusEffectInfo )
{
	if( m_lstDMAudioPaths.IsEmpty() )
	{
		return;
	}

	if( !pBusEffectInfo
	||	!pBusEffectInfo->m_EffectInfo.m_pIStream )
	{
		return;
	}

	IStream *pIStream;
	if( FAILED( pBusEffectInfo->m_EffectInfo.m_pIStream->Clone( &pIStream ) ) )
	{
		return;
	}

	// Keep a count of the number of MixIn buffers we've seen
	DWORD dwMixInBufferIndex = 0;

	// Iterate through the list of items
	POSITION posItem = m_lstItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		ItemInfo *pItemInfo = m_lstItems.GetNext( posItem );

		// If the port doesn't support buffers, skip it
		if( !ItemSupportsBuffers( pItemInfo ) )
		{
			continue;
		}

		// Keep a count of the number of normal buffers we've seen
		DWORD dwBufferIndex = 0;

		// Iterate through the list of buffers
		POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			// Keep a count of the number of effects we've seen
			DWORD dwEffectIndex = 0;

			// Get a pointer to each buffer
			BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

			// Iterate through the list of effects
			POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();

			// If the buffer does not support effects, don't check any of the effects in the buffer
			if( !ItemAndBufferSupportEffects( pItemInfo, pBufferOptions ) )
			{
				posEffect = NULL;
			}

			while( posEffect )
			{
				// Get a pointer to each effect
				BusEffectInfo *pBusEffectInfoInList = pBufferOptions->lstEffects.GetNext( posEffect );

				// Check if this is the effect we're looking for
				if( pBusEffectInfoInList == pBusEffectInfo )
				{
					// Found an effect to update

					// Iterate through the audio paths
					POSITION posAudioPath = m_lstDMAudioPaths.GetHeadPosition();
					while( posAudioPath )
					{
						DWORD dwPChannel, dwPathLocation, dwBufNum;
						if( pItemInfo->nNumPChannels > 0 )
						{
							dwPChannel = pItemInfo->adwPChannels[0];
							dwPathLocation = DMUS_PATH_BUFFER_DMO;
							dwBufNum = dwBufferIndex;
						}
						else
						{
							dwPChannel = 0;
							dwPathLocation = DMUS_PATH_MIXIN_BUFFER_DMO;
							dwBufNum = dwMixInBufferIndex;
						}

						if( GUID_DSFX_SEND == pBusEffectInfo->m_EffectInfo.m_clsidObject )
						{
							IDirectSoundFXSend *pIDirectSoundFXSend;
							if( SUCCEEDED( m_lstDMAudioPaths.GetNext( posAudioPath )->GetObjectInPath( dwPChannel,
								dwPathLocation, dwBufNum, GUID_All_Objects, dwEffectIndex,
								IID_IDirectSoundFXSend, (void **)&pIDirectSoundFXSend ) ) )
							{
								DSFXSend dsfxSend;
								dsfxSend.lSendLevel = GetLongFromStream( pBusEffectInfo->m_EffectInfo.m_pIStream );
								pIDirectSoundFXSend->SetAllParameters( &dsfxSend );
								pIDirectSoundFXSend->Release();
							}
						}
						else
						{
							IPersistStream *pIPersistStream;
							if( SUCCEEDED( m_lstDMAudioPaths.GetNext( posAudioPath )->GetObjectInPath( dwPChannel,
								dwPathLocation, dwBufNum, GUID_All_Objects, dwEffectIndex,
								IID_IPersistStream, (void **)&pIPersistStream ) ) )
							{
								StreamSeek( pIStream, 0, STREAM_SEEK_SET );
								pIPersistStream->Load( pIStream );
								pIPersistStream->Release();
							}
						}
					}
				}

				// Increment the effect index
				dwEffectIndex++;
			}

			// Increment the buffer index
			dwBufferIndex++;
		}

		if( pItemInfo->nNumPChannels == 0 )
		{
			dwMixInBufferIndex++;
		}
	}

	pIStream->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::SetModified

void CDirectMusicAudioPath::SetModified( BOOL fModified )
{
	m_fModified = fModified;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IUnknown implementation

HRESULT CDirectMusicAudioPath::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||	::IsEqualIID(riid, IID_IUnknown) )
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
	else if( ::IsEqualIID(riid, IID_IDMUSProdAudioPathInUse) )
	{
		*ppvObj = (IDMUSProdAudioPathInUse *)this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdNotifySink) )
	{
		*ppvObj = (IDMUSProdNotifySink *)this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdDMOInfo) )
	{
		*ppvObj = (IDMUSProdDMOInfo *)this;
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CDirectMusicAudioPath::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
	return ++m_dwRef;
}

ULONG CDirectMusicAudioPath::Release()
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
// CDirectMusicAudioPath IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetNodeImageIndex

HRESULT CDirectMusicAudioPath::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );

	return( theApp.m_pAudioPathComponent->GetAudioPathImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetFirstChild

HRESULT CDirectMusicAudioPath::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}

	if( m_pIToolGraphNode )
	{
		*ppIFirstChildNode = m_pIToolGraphNode;
		(*ppIFirstChildNode)->AddRef();
	}
	else
	{
		*ppIFirstChildNode = NULL;
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetNextChild

HRESULT CDirectMusicAudioPath::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINextChildNode == NULL )
	{
		return E_POINTER;
	}

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	*ppINextChildNode = NULL;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetComponent

HRESULT CDirectMusicAudioPath::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );

	return theApp.m_pAudioPathComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetDocRootNode

HRESULT CDirectMusicAudioPath::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CDirectMusicAudioPath IDMUSProdNode::SetDocRootNode

HRESULT CDirectMusicAudioPath::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetParentNode

HRESULT CDirectMusicAudioPath::GetParentNode( IDMUSProdNode** ppIParentNode )
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
// CDirectMusicAudioPath IDMUSProdNode::SetParentNode

HRESULT CDirectMusicAudioPath::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetNodeId

HRESULT CDirectMusicAudioPath::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_AudioPathNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetNodeName

HRESULT CDirectMusicAudioPath::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetNodeNameMaxLength

HRESULT CDirectMusicAudioPath::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = DMUS_MAX_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::ValidateNodeName

HRESULT CDirectMusicAudioPath::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;

	strName = bstrName;
	::SysFreeString( bstrName );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::SetNodeName

HRESULT CDirectMusicAudioPath::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( m_strName == strName )
	{
		return S_OK;
	}

	if( !IsInSegment() )
	{
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_AUDIOPATH_NAME );
	}

	m_strName = strName;

	// Sync change with property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		RELEASE( pIPropSheet );
	}

	if( !IsInSegment() )
	{
		SetModified( TRUE );
	}

	// Notify connected nodes that AudioPath name has changed
	theApp.m_pAudioPathComponent->m_pIFramework->NotifyNodes( this, AUDIOPATH_NameChange, NULL );

	// Notify Conductor that the Audiopath name has changed
	theApp.m_pAudioPathComponent->m_pIConductor->SetAudiopathName( this, GetNameForConductor() );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetNodeListInfo

HRESULT CDirectMusicAudioPath::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strDescriptor;

	pListInfo->bstrName = m_strName.AllocSysString();
	pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
	memcpy( &pListInfo->guidObject, &m_guidAudioPath, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetEditorClsId

HRESULT CDirectMusicAudioPath::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

   *pClsId = CLSID_AudioPathEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetEditorTitle

HRESULT CDirectMusicAudioPath::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strTitle;

	strTitle.LoadString( IDS_AUDIOPATH_TEXT );
	strTitle += _T(": " );
	strTitle += m_strName;

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetEditorWindow

HRESULT CDirectMusicAudioPath::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::SetEditorWindow

HRESULT CDirectMusicAudioPath::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::UseOpenCloseImages

HRESULT CDirectMusicAudioPath::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetRightClickMenuId

HRESULT CDirectMusicAudioPath::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;

	if(	IsInSegment() )
	{
		*pnMenuId = IDM_AUDIOPATH_NODE_RMENU;
	}
	else
	{
		*pnMenuId = IDM_AUDIOPATH_DOCROOT_RMENU;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::OnRightClickMenuInit

HRESULT CDirectMusicAudioPath::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( IsInSegment() )
	{
		// IDM_CUT
		if( CanCut() == S_OK )
		{
			::EnableMenuItem( hMenu, IDM_CUT, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			::EnableMenuItem( hMenu, IDM_CUT, (MF_GRAYED | MF_BYCOMMAND) );
		}

		// IDM_COPY
		if( CanCopy() == S_OK )
		{
			::EnableMenuItem( hMenu, IDM_COPY, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			::EnableMenuItem( hMenu, IDM_COPY, (MF_GRAYED | MF_BYCOMMAND) );
		}

		// IDM_PASTE
		{
			IDataObject* pIDataObject;
			BOOL fWillSetReference;

			::EnableMenuItem( hMenu, IDM_PASTE, (MF_GRAYED | MF_BYCOMMAND) );

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					::EnableMenuItem( hMenu, IDM_PASTE, (MF_ENABLED | MF_BYCOMMAND) );
				}
				
				pIDataObject->Release();
			}
		}

		// IDM_DELETE
		if( CanDelete() == S_OK )
		{
			::EnableMenuItem( hMenu, IDM_DELETE, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			::EnableMenuItem( hMenu, IDM_DELETE, (MF_GRAYED | MF_BYCOMMAND) );
		}

		// IDM_RENAME
		::EnableMenuItem( hMenu, IDM_RENAME, (MF_GRAYED | MF_BYCOMMAND) );
	}
	else
	{
		::EnableMenuItem( hMenu, IDM_RENAME, (MF_ENABLED | MF_BYCOMMAND) );
	}

	// IDM_NEW_TOOLGRAPH
#ifndef DMP_XBOX
	::EnableMenuItem( hMenu, IDM_NEW_TOOLGRAPH, m_pIToolGraphNode ? (MF_GRAYED | MF_BYCOMMAND) : (MF_ENABLED | MF_BYCOMMAND) );
#else
	::EnableMenuItem( hMenu, IDM_NEW_TOOLGRAPH, MF_GRAYED | MF_BYCOMMAND );
#endif

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::OnRightClickMenuSelect

HRESULT CDirectMusicAudioPath::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_CUT:
			if( CanCut() == S_OK )
			{
				IDataObject* pIDataObject;
				if( SUCCEEDED ( CreateDataObject( &pIDataObject ) ) )
				{
					if( theApp.PutDataInClipboard( pIDataObject, this ) )
					{
						DeleteNode( FALSE );
					}
					
					pIDataObject->Release();
					pIDataObject = NULL;
				}
			}
			break;

		case IDM_COPY:
			if( CanCopy() == S_OK )
			{
				IDataObject* pIDataObject;
				if( SUCCEEDED ( CreateDataObject( &pIDataObject ) ) )
				{
					theApp.PutDataInClipboard( pIDataObject, this );
					
					pIDataObject->Release();
					pIDataObject = NULL;
				}
			}
			break;

		case IDM_PASTE:
		{
			BOOL fWillSetReference;
			IDataObject* pIDataObject;

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					PasteFromData( pIDataObject );
				}
				
				pIDataObject->Release();
				pIDataObject = NULL;
			}
			break;
		}

		case IDM_DELETE:
			DeleteNode( TRUE );
			break;

		case IDM_NEW_TOOLGRAPH:
			hr = CreateEmptyToolGraph();
			if( m_pIToolGraphNode )
			{
				theApp.m_pAudioPathComponent->m_pIFramework->OpenEditor( m_pIToolGraphNode );
			}
			break;

		case IDM_RENAME:
			if( IsInSegment() == FALSE )
			{
				hr = theApp.m_pAudioPathComponent->m_pIFramework->EditNodeLabel( this );
			}
			break;

		case IDM_PROPERTIES:
			OnShowProperties();
			hr = S_OK;
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::DeleteChildNode

HRESULT CDirectMusicAudioPath::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	CWaitCursor wait;

	if( pIChildNode == NULL 
	||  pIChildNode != m_pIToolGraphNode )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// Remove node from Project Tree
	if( theApp.m_pAudioPathComponent->m_pIFramework->RemoveNode( pIChildNode, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Remove from AudioPath
	if( pIChildNode == m_pIToolGraphNode )
	{
		AllAudioPaths_RemoveToolGraph();
		RELEASE( m_pIToolGraphNode );
	}

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::InsertChildNode

HRESULT CDirectMusicAudioPath::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	pIChildNode->AddRef();

	// AudioPaths can only have one Tool Graph
	GUID guidNode;
	if( FAILED( pIChildNode->GetNodeId( &guidNode ) ) )
	{
		return E_INVALIDARG;
	}

	if( guidNode == GUID_ToolGraphNode )
	{
		if( m_pIToolGraphNode )
		{
			DeleteChildNode( m_pIToolGraphNode, FALSE );
		}

		// Set the AudioPath's Tool Graph
		ASSERT( m_pIToolGraphNode == NULL );
		m_pIToolGraphNode = pIChildNode;
		AllAudioPaths_InsertToolGraph();

		// Set root and parent node of ALL children
		theApp.SetNodePointers( m_pIToolGraphNode, m_pIDocRootNode, (IDMUSProdNode *)this );

		// Add node to Project Tree
		if( !SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->AddNode(m_pIToolGraphNode, (IDMUSProdNode *)this) ) )
		{
			DeleteChildNode( m_pIToolGraphNode, FALSE );
			return E_FAIL;
		}
	}
	else
	{
		return E_INVALIDARG;
	}

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::DeleteNode

HRESULT CDirectMusicAudioPath::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIParentNode;

	// Let our parent delete us
    if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
	{
		if( pIParentNode )
		{
			HRESULT hr = pIParentNode->DeleteChildNode( this, fPromptUser );
			pIParentNode->Release();
			return hr;
		}
	}

	CWaitCursor wait;

	// No parent so we will delete ourself
	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	// Remove from Project Tree
	if( theApp.m_pAudioPathComponent->m_pIFramework->RemoveNode( this, fPromptUser ) == S_FALSE )
	{
		// User cancelled delete
		return E_FAIL;
	}

	// Release the AudioPath's ToolGraph
	RELEASE( m_pIToolGraphNode );

	// Remove from Component AudioPath list
	theApp.m_pAudioPathComponent->RemoveFromAudioPathFileList( this );

	// Remove from the Conductor's dropdown list
	theApp.m_pAudioPathComponent->m_pIConductor->UnRegisterAudiopath( this );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::OnNodeSelChanged

HRESULT CDirectMusicAudioPath::OnNodeSelChanged( BOOL fSelected )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::CreateDataObject

HRESULT CDirectMusicAudioPath::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

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

	// Save AudioPath into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_AUDIOPATH into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pAudioPathComponent->m_cfAudioPath, pIStream ) ) )
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
		if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->SaveClipFormat( theApp.m_pAudioPathComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// AudioPath nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pAudioPathComponent->m_cfProducerFile, pIStream ) ) )
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
// CDirectMusicAudioPath IDMUSProdNode::CanCut

HRESULT CDirectMusicAudioPath::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::CanCopy

HRESULT CDirectMusicAudioPath::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::CanDelete

HRESULT CDirectMusicAudioPath::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			return m_pIParentNode->CanDeleteChildNode( this );
		}
		else
		{
			return S_FALSE;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::CanDeleteChildNode

HRESULT CDirectMusicAudioPath::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIChildNode == NULL )
	{
		return E_POINTER;
	}

	if( pIChildNode == m_pIToolGraphNode )
	{
		return S_OK;
	}

	return S_FALSE;	
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::CanPasteFromData

HRESULT CDirectMusicAudioPath::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	if( pDataObject->IsClipFormatAvailable(pIDataObject, theApp.m_pAudioPathComponent->m_cfGraph) == S_OK )
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
			// Let Segment, for example, decide what can be dropped
			return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
		}
	}
	
	return S_FALSE; 	// Can't paste anything on a AudioPath node in the project tree
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::PasteFromData

HRESULT CDirectMusicAudioPath::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pAudioPathComponent->m_cfGraph ) ) )
	{
		// Handle CF_GRAPH 
		hr = PasteCF_GRAPH( pDataObject, pIDataObject );
	}
	RELEASE( pDataObject );
	if( hr == S_OK )
	{
		return hr;
	}

	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			// Let Segment, for example, handle paste
			return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
		}
	}

	return E_FAIL;	
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::CanChildPasteFromData

HRESULT CDirectMusicAudioPath::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												  BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pAudioPathComponent->m_cfGraph ) ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_ToolGraphNode ) )
		{
			hr = S_OK;
		}
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::ChildPasteFromData

HRESULT CDirectMusicAudioPath::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
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

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pAudioPathComponent->m_cfGraph ) ) )
	{
		// Handle CF_GRAPH format
		hr = PasteCF_GRAPH( pDataObject, pIDataObject );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNode::GetObject

HRESULT CDirectMusicAudioPath::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Must be asking for an interface to a CLSID_DirectMusicAudioPathConfig object
	if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicAudioPathConfig ) )
	{
		// Create an CLSID_DirectMusicAudioPathConfig object
		IUnknown *pSourceConfig;
		HRESULT hr = ::CoCreateInstance( CLSID_DirectMusicAudioPathConfig, NULL, CLSCTX_INPROC_SERVER,
								 IID_IUnknown, (void**)&pSourceConfig );
		if( SUCCEEDED( hr ) )
		{
			// Persist ourself into the config object
			hr = SaveToConfigObject( pSourceConfig );
			if( SUCCEEDED( hr ) )
			{
				// Finally, query the config object for the requested interface
				hr = pSourceConfig->QueryInterface( riid, ppvObject );
			}
			pSourceConfig->Release();
		}
		return hr;
	}
	else if( ::IsEqualCLSID( rclsid, GUID_ToolGraphNode ) )
	{
		if( m_pIToolGraphNode )
		{
			return m_pIToolGraphNode->QueryInterface( riid, ppvObject );
		}
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdPropPageObject::GetData

HRESULT CDirectMusicAudioPath::GetData( void** ppData )
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
// CDirectMusicAudioPath IDMUSProdPropPageObject::SetData

HRESULT CDirectMusicAudioPath::SetData( void* pData )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdPropPageObject::OnShowProperties

HRESULT CDirectMusicAudioPath::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	// Show the AudioPath properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CAudioPathPropPageManager::sm_nActiveTab;

		if( SUCCEEDED ( pIPropSheet->SetPageManager(theApp.m_pAudioPathComponent->m_pIAudioPathPageManager) ) )
		{
			theApp.m_pAudioPathComponent->m_pIAudioPathPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CDirectMusicAudioPath::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdAudioPathInUse implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdAudioPathInUse::UsingAudioPath

HRESULT CDirectMusicAudioPath::UsingAudioPath( IUnknown *punkAudioPath, BOOL fActive )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( punkAudioPath == NULL )
	{
		return E_POINTER;
	}

	IDirectMusicAudioPath *pDMAudioPath;
	if( FAILED( punkAudioPath->QueryInterface( IID_IDirectMusicAudioPath, (void **)&pDMAudioPath ) ) )
	{
		return E_INVALIDARG;
	}

	if( !fActive )
	{
		POSITION pos = m_lstDMAudioPaths.GetHeadPosition();
		while( pos )
		{
			POSITION posCurrent = pos;
			if( pDMAudioPath == m_lstDMAudioPaths.GetNext( pos ) )
			{
				// Remove the toolgraph
				RemoveToolGraph( pDMAudioPath );

				m_lstDMAudioPaths.RemoveAt( posCurrent );
				pDMAudioPath->Release(); // For the item in the list
				pDMAudioPath->Release(); // For the QueryInterface above
				return S_OK;
			}
		}

		// Didn't find the audio path
		pDMAudioPath->Release(); // For the QueryInterface above
		return E_INVALIDARG;
	}
	else
	{
		// Add the toolgraph
		InsertToolGraph( pDMAudioPath );

		// Just add the audio path to the end of the list
		m_lstDMAudioPaths.AddTail( pDMAudioPath );
		return S_OK;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdNotifySink implementation

///////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath :: OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData)

HRESULT CDirectMusicAudioPath::OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	if( pIDocRootNode == m_pIToolGraphNode )
	{
		// TOOLGRAPH_ChangeNotification
		if( ::IsEqualGUID(guidUpdateType, TOOLGRAPH_ChangeNotification ) )
		{
			SetModified( TRUE );
			return S_OK;
		}
	}
	else if( pIDocRootNode == this )
	{
		if( ::IsEqualGUID(guidUpdateType, FRAMEWORK_FileNameChange)
		||	::IsEqualGUID(guidUpdateType, FRAMEWORK_AfterFileSave) )
		{
			// Notify Conductor that the Audiopath name has changed
			theApp.m_pAudioPathComponent->m_pIConductor->SetAudiopathName( this, GetNameForConductor() );
			return S_OK;
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IDMUSProdDMOInfo implementation

// Helper function for CDirectMusicAudioPath :: EnumDMOInfo
void GetDMOName( CLSID clsidDMO, WCHAR *wcstrName )
{
	IEnumDMO *pEnumDMO;

	DMO_PARTIAL_MEDIATYPE dmoPMT;
	dmoPMT.type = MEDIATYPE_Audio;
	dmoPMT.subtype = MEDIASUBTYPE_PCM;

	if( SUCCEEDED( DMOEnum( DMOCATEGORY_AUDIO_EFFECT, 0, 1, &dmoPMT, 1, &dmoPMT, &pEnumDMO ) ) )
	{
		pEnumDMO->Reset();
		CLSID clsidItem;
		WCHAR *pwcName;
		DWORD dwItemsFetched;
		while( S_OK == pEnumDMO->Next( 1, &clsidItem, &pwcName, &dwItemsFetched ) )
		{
			if( clsidItem == clsidDMO )
			{
				wcsncpy( wcstrName, pwcName, 64 );
				::CoTaskMemFree( pwcName );
				break;
			}

			::CoTaskMemFree( pwcName );
		}
		pEnumDMO->Release();
	}
}

DWORD AddRefEffectToList( REFGUID rguidEffect, CTypedPtrList< CPtrList, EnumEffectStruct *> &lstEffectStruct )
{
	POSITION pos = lstEffectStruct.GetHeadPosition();
	while( pos )
	{
		EnumEffectStruct *pEnumEffectStruct = lstEffectStruct.GetNext( pos );
		if( pEnumEffectStruct->guidEffect == rguidEffect )
		{
			pEnumEffectStruct->dwRefCount++;
			return pEnumEffectStruct->dwRefCount;
		}
	}

	EnumEffectStruct *pEnumEffectStruct = new EnumEffectStruct;
	pEnumEffectStruct->guidEffect = rguidEffect;
	pEnumEffectStruct->dwRefCount = 0;
	lstEffectStruct.AddHead( pEnumEffectStruct );
	return 0;
}

void CleanupEffectList( CTypedPtrList< CPtrList, EnumEffectStruct *> &lstEffectStruct )
{
	while( !lstEffectStruct.IsEmpty() )
	{
		delete lstEffectStruct.RemoveHead();
	}
}

///////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath :: EnumDMOInfo

HRESULT CDirectMusicAudioPath::EnumDMOInfo( DWORD dwIndex, DMUSProdDMOInfo *pDMOInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate arguments
	if( pDMOInfo == NULL
	||	pDMOInfo->dwSize != sizeof( DMUSProdDMOInfo ) )
	{
		return E_POINTER;
	}

	ZeroMemory( pDMOInfo->awchPChannelText, sizeof(WCHAR) * 65 );
	ZeroMemory( pDMOInfo->awchMixGroupName, sizeof(WCHAR) * 65 );
	ZeroMemory( pDMOInfo->awchBufferText, sizeof(WCHAR) * 65 );
	ZeroMemory( pDMOInfo->awchDMOName, sizeof(WCHAR) * 65 );
	ZeroMemory( &pDMOInfo->guidDesignGUID, sizeof(GUID) );

	// Keep a count of the number of MixIn buffers we've seen
	DWORD dwMixInBufferIndex = 0;

	// Iterate through all items
	POSITION posItem = m_lstItems.GetHeadPosition();
	while( posItem )
	{
		// Keep a count of the number of normal buffers we've seen
		DWORD dwBufferIndex = 0;

		// Get a pointer to each item
		ItemInfo *pItem = m_lstItems.GetNext( posItem );

		// If the port doesn't support buffers, skip it
		if( !ItemSupportsBuffers( pItem ) )
		{
			continue;
		}

		// Iterate through all buffers
		POSITION posBuffer = pItem->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			// Keep a count of the number of effects we've seen
			CTypedPtrList< CPtrList, EnumEffectStruct * > lstEffectStruct;

			// Get a pointer to each buffer
			BufferOptions *pBufferOptions = pItem->lstBuffers.GetNext( posBuffer );

			// Iterate through all effects
			POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();

			// If the buffer does not support effects, don't check any of the effects in the buffer
			if( !ItemAndBufferSupportEffects( pItem, pBufferOptions ) )
			{
				posEffect = NULL;
			}

			while( posEffect )
			{
				// Get a pointer to each effect
				BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );

				// Check if this is the effect to return
				if( dwIndex == 0 )
				{
					// Yes - copy the CLSID and design-time GUID
					pDMOInfo->clsidDMO = pBusEffectInfo->m_EffectInfo.m_clsidObject;
					pDMOInfo->guidDesignGUID = pBusEffectInfo->m_guidEffectInfo;

					// Copy the mix group name and effect instance name
					if( 0 == MultiByteToWideChar( CP_ACP, 0, pItem->strBandName, -1, pDMOInfo->awchMixGroupName, 65 ) )
					{
						CleanupEffectList( lstEffectStruct );
						return E_FAIL;
					}
					if( 0 == MultiByteToWideChar( CP_ACP, 0, pBusEffectInfo->m_EffectInfo.m_strInstanceName, -1, pDMOInfo->awchDMOName, 65 ) )
					{
						CleanupEffectList( lstEffectStruct );
						return E_FAIL;
					}

					// Copy the approprate information, depending on if this is a MixIn buffer or not
					if( pItem->nNumPChannels == 0 )
					{
						pDMOInfo->dwStage = DMUS_PATH_MIXIN_BUFFER_DMO;
						pDMOInfo->dwBufferIndex = dwMixInBufferIndex;
						pDMOInfo->dwPChannel = 0;
						pDMOInfo->awchPChannelText[0] = NULL;
						pDMOInfo->awchBufferText[0] = NULL;
					}
					else
					{
						pDMOInfo->dwStage = DMUS_PATH_BUFFER_DMO;
						pDMOInfo->dwBufferIndex = dwBufferIndex;
						pDMOInfo->dwPChannel = pItem->adwPChannels[0];
						CString strText;
						PChannelArrayToString( pItem->adwPChannels, pItem->nNumPChannels, strText );
						strText = strText.Left( 64 );
						if( 0 == MultiByteToWideChar( CP_ACP, 0, strText, -1, pDMOInfo->awchPChannelText, 65 ) )
						{
							CleanupEffectList( lstEffectStruct );
							return E_FAIL;
						}
						BusListToString( pItem, pBufferOptions , strText );
						if( 0 == MultiByteToWideChar( CP_ACP, 0, strText, -1, pDMOInfo->awchBufferText, 65 ) )
						{
							CleanupEffectList( lstEffectStruct );
							return E_FAIL;
						}

					}
					pDMOInfo->dwEffectIndex = AddRefEffectToList( pDMOInfo->clsidDMO, lstEffectStruct );
					CleanupEffectList( lstEffectStruct );
					return S_OK;
				}
				else
				{
					dwIndex--;

					AddRefEffectToList( pBusEffectInfo->m_EffectInfo.m_clsidObject, lstEffectStruct );
				}
			}

			CleanupEffectList( lstEffectStruct );

			if( pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED )
			{
				/*
				if( pBufferOptions->guidBuffer == GUID_Buffer_3D )
				{
					if( dwIndex == 0 )
					{
						// Yes - copy the CLSID
						pDMOInfo->clsidDMO = GUID_DSFX_STANDARD_I3DL2SOURCE;

						// Set the design-time GUID
						pDMOInfo->guidDesignGUID = pBufferOptions->guidDesignEffectGUID;

						// This must not be a MixIn buffer
						ASSERT( pItem->nNumPChannels != 0 );

						pDMOInfo->dwStage = DMUS_PATH_BUFFER_DMO;
						pDMOInfo->dwBufferIndex = dwBufferIndex;
						pDMOInfo->dwPChannel = pItem->adwPChannels[0];

						CString strText;
						PChannelArrayToString( pItem->adwPChannels, pItem->nNumPChannels, strText );
						strText = strText.Left( 64 );
						if( 0 == MultiByteToWideChar( CP_ACP, 0, strText, -1, pDMOInfo->awchPChannelText, 65 ) )
						{
							return E_FAIL;
						}

						BusListToString( pItem, pBufferOptions , strText );
						if( 0 == MultiByteToWideChar( CP_ACP, 0, strText, -1, pDMOInfo->awchBufferText, 65 ) )
						{
							return E_FAIL;
						}

						if( 0 == MultiByteToWideChar( CP_ACP, 0, pItem->strBandName, -1, pDMOInfo->awchMixGroupName, 65 ) )
						{
							return E_FAIL;
						}
						GetDMOName( GUID_DSFX_STANDARD_I3DL2SOURCE, pDMOInfo->awchDMOName );
						pDMOInfo->dwEffectIndex = 0;
						return S_OK;
					}
					else
					{
						dwIndex--;
					}
				}
				else*/ if( pBufferOptions->guidBuffer == GUID_Buffer_EnvReverb )
				{
					if( dwIndex == 0 )
					{
						// Yes - copy the CLSID
						pDMOInfo->clsidDMO = GUID_DSFX_STANDARD_I3DL2REVERB;

						// Set the design-time GUID
						pDMOInfo->guidDesignGUID = GUID_DSFX_STANDARD_I3DL2REVERB;

						// This must be a MixIn buffer
						ASSERT( pItem->nNumPChannels == 0 );
						pDMOInfo->dwStage = DMUS_PATH_MIXIN_BUFFER_DMO;
						pDMOInfo->dwBufferIndex = dwMixInBufferIndex;
						pDMOInfo->dwPChannel = 0;
						pDMOInfo->awchPChannelText[0] = NULL;
						pDMOInfo->awchBufferText[0] = NULL;

						if( 0 == MultiByteToWideChar( CP_ACP, 0, pItem->strBandName, -1, pDMOInfo->awchMixGroupName, 65 ) )
						{
							return E_FAIL;
						}

						GetDMOName( GUID_DSFX_STANDARD_I3DL2REVERB, pDMOInfo->awchDMOName );
						pDMOInfo->dwEffectIndex = 0;
						return S_OK;
					}
					else
					{
						dwIndex--;
					}
				}
				else if( pBufferOptions->guidBuffer == GUID_Buffer_Reverb )
				{
					if( dwIndex == 0 )
					{
						// Yes - copy the CLSID
						pDMOInfo->clsidDMO = GUID_DSFX_WAVES_REVERB;

						// Set the design-time GUID
						pDMOInfo->guidDesignGUID = pBufferOptions->guidDesignEffectGUID;

						// This must not be a MixIn buffer
						ASSERT( pItem->nNumPChannels != 0 );

						pDMOInfo->dwStage = DMUS_PATH_BUFFER_DMO;
						pDMOInfo->dwBufferIndex = dwBufferIndex;
						pDMOInfo->dwPChannel = pItem->adwPChannels[0];

						CString strText;
						PChannelArrayToString( pItem->adwPChannels, pItem->nNumPChannels, strText );
						strText = strText.Left( 64 );
						if( 0 == MultiByteToWideChar( CP_ACP, 0, strText, -1, pDMOInfo->awchPChannelText, 65 ) )
						{
							return E_FAIL;
						}

						BusListToString( pItem, pBufferOptions , strText );
						if( 0 == MultiByteToWideChar( CP_ACP, 0, strText, -1, pDMOInfo->awchBufferText, 65 ) )
						{
							return E_FAIL;
						}

						if( 0 == MultiByteToWideChar( CP_ACP, 0, pItem->strBandName, -1, pDMOInfo->awchMixGroupName, 65 ) )
						{
							return E_FAIL;
						}
						GetDMOName( GUID_DSFX_WAVES_REVERB, pDMOInfo->awchDMOName );
						pDMOInfo->dwEffectIndex = 0;
						return S_OK;
					}
					else
					{
						dwIndex--;
					}
				}
			}
			dwBufferIndex++;
		}

		if( pItem->nNumPChannels == 0 )
		{
			dwMixInBufferIndex++;
		}
	}
	return E_NOTIMPL;
}


///////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath :: EnumBufferInfo

HRESULT CDirectMusicAudioPath::EnumBufferInfo( DWORD dwIndex, WCHAR wcstr[DMUS_MAX_NAME + 1], GUID *pGuid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate arguments
	if( wcstr == NULL
	||	pGuid == NULL )
	{
		return E_POINTER;
	}

	ZeroMemory( wcstr, sizeof(WCHAR) * DMUS_MAX_NAME + 1 );

	// Iterate through all items
	POSITION posItem = m_lstItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		ItemInfo *pItem = m_lstItems.GetNext( posItem );

		if( pItem->nNumPChannels == 0 )
		{
			if( dwIndex == 0 )
			{
				if( pItem->lstBuffers.GetCount() == 0 ) 
				{
					return E_UNEXPECTED;
				}

				if( 0 == MultiByteToWideChar( CP_ACP, 0, pItem->strBandName, -1, wcstr, DMUS_MAX_NAME ) )
				{
					return E_FAIL;
				}
				*pGuid = pItem->lstBuffers.GetHead()->guidBuffer;
				return S_OK;
			}

			dwIndex--;
		}
	}
	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IPersist::GetClassID

HRESULT CDirectMusicAudioPath::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pClsId != NULL );

	memset( pClsId, 0, sizeof( CLSID ) );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IPersistStream::IsDirty

HRESULT CDirectMusicAudioPath::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fModified )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IPersistStream::Load

HRESULT CDirectMusicAudioPath::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = DM_LoadAudioPath( pIStream );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// Update the display indexes
	UpdateDisplayIndexes();

	if( m_pIDocRootNode )
	{
		// Already in Project Tree so sync changes
		// Refresh AudioPath editor (when open)
		SyncAudioPathEditor();

		// Sync change with property sheet
		if(theApp.m_pAudioPathComponent->m_pIAudioPathPageManager)
		{
			theApp.m_pAudioPathComponent->m_pIAudioPathPageManager->RefreshData();
		}
	}

	// Notify Conductor that the Audiopath name has changed
	theApp.m_pAudioPathComponent->m_pIConductor->SetAudiopathName( this, GetNameForConductor() );

	if( SUCCEEDED( hr ) )
	{
		hr = S_OK;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IPersistStream::Save

HRESULT CDirectMusicAudioPath::Save( IStream* pIStream, BOOL fClearDirty )
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

	HRESULT hr = S_OK;

	// Validate requested data format
	if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion )
	||	IsEqualGUID( StreamInfo.guidDataFormat, GUID_DirectMusicObject ) )
	{
		hr = DM_SaveAudioPath( pIStream );
		if( SUCCEEDED ( hr ) )
		{
			if( fClearDirty )
			{
				SetModified( FALSE );
			}
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath IPersistStream::GetSizeMax

HRESULT CDirectMusicAudioPath::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath Additional functions


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::GetObjectDescriptor

HRESULT CDirectMusicAudioPath::GetObjectDescriptor( void* pObjectDesc )
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
	
	memcpy( &pDMObjectDesc->guidObject, &m_guidAudioPath, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicAudioPathConfig, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = m_vVersion.dwVersionMS;
	pDMObjectDesc->vVersion.dwVersionLS = m_vVersion.dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, m_strName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::DM_LoadAudioPath

HRESULT CDirectMusicAudioPath::DM_LoadAudioPath( IStream *pIStream )
{
	// Save the current list of items and ports
	CTypedPtrList< CPtrList, ItemInfo *> lstOldItems;
	CTypedPtrList< CPtrList, PortOptions *> lstOldPortOptions;

	// Empty the current list of items
	while( !m_lstItems.IsEmpty() )
	{
		lstOldItems.AddHead( m_lstItems.RemoveHead() );
	}

	// Empty the current list of ports
	while( !m_lstPortOptions.IsEmpty() )
	{
		lstOldPortOptions.AddHead( m_lstPortOptions.RemoveHead() );
	}

	// Synchronize the display
	SyncAudioPathEditor();

	// Now, delete the items and ports
	while( !lstOldItems.IsEmpty() )
	{
		delete lstOldItems.RemoveHead();
	}
	while( !lstOldPortOptions.IsEmpty() )
	{
		delete lstOldPortOptions.RemoveHead();
	}

	// Initialize the list of ports
	EnumeratePorts();

	// Initialize other variables
	m_strAuthor.Empty();
	m_strCopyright.Empty();
	m_strSubject.Empty();

	// Allocate a RIFF stream
	IDMUSProdRIFFStream *pRiffStream;
	AllocRIFFStream( pIStream, &pRiffStream );

	// Save the current position
	DWORD dwPos = StreamTell( pIStream );

	// Initialize the result to S_FALSE
	HRESULT hr = S_FALSE;

	CTypedPtrList< CPtrList, ioPortConfig*> lstPortConfigs;
	CTypedPtrList< CPtrList, ioDSoundBuffer*> lstDSoundBuffers;

	// Find the main audiopath chunk
	MMCKINFO ckMain;
	ckMain.fccType = DMUS_FOURCC_AUDIOPATH_FORM;
	if( 0 == pRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) )
	{
		// Now, descend into each child chunk
		MMCKINFO ckChunk, ckList;
		DWORD cbSize, cbRead;
		while( SUCCEEDED(hr)
			&& (0 == pRiffStream->Descend( &ckChunk, &ckMain, 0 )) )
		{
			switch( ckChunk.ckid )
			{
			case DMUS_FOURCC_GUID_CHUNK:
				cbSize = min( ckChunk.cksize, sizeof( GUID ) );
				hr = pIStream->Read( &m_guidAudioPath, cbSize, &cbRead );
				if( FAILED( hr )
				||  cbRead != cbSize )
				{
					hr = E_FAIL;
				}
				break;

			case DMUS_FOURCC_VERSION_CHUNK:
			{
			    DMUS_IO_VERSION dmusVersionIO;

				cbSize = min( ckChunk.cksize, sizeof( DMUS_IO_VERSION ) );
				hr = pIStream->Read( &dmusVersionIO, cbSize, &cbRead );
				if( FAILED( hr )
				||  cbRead != cbSize )
				{
					hr = E_FAIL;
				}

				m_vVersion.dwVersionMS = dmusVersionIO.dwVersionMS;
				m_vVersion.dwVersionLS = dmusVersionIO.dwVersionLS;
				break;
			}
			case FOURCC_RIFF:
				switch( ckChunk.fccType )
				{
					case DMUS_FOURCC_TOOLGRAPH_FORM:
					if( theApp.m_pAudioPathComponent->m_pIToolGraphComponent )
					{
						IDMUSProdRIFFExt* pIRIFFExt;
						IDMUSProdNode* pINode;

						hr = theApp.m_pAudioPathComponent->m_pIToolGraphComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( SUCCEEDED ( hr ) )
						{
							StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
							hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
							RELEASE( pIRIFFExt );
							if( SUCCEEDED ( hr ) )
							{
								if( m_pIToolGraphNode )
								{
									InsertChildNode( pINode);
									pINode->Release();
								}
								else
								{
									m_pIToolGraphNode = pINode;
									AllAudioPaths_InsertToolGraph();
								}
							}
						}
						break;
					}
				}
				break;

			case FOURCC_LIST:
				switch( ckChunk.fccType )
				{
				case DMUSPROD_FOURCC_AUDIOPATH_DESIGN_LIST:
					// Load in the effect palette UI info
					DM_LoadDesignInfo( pRiffStream, ckChunk );
					break;

				case DMUS_FOURCC_UNFO_LIST:
					while( pRiffStream->Descend( &ckList, &ckChunk, 0 ) == 0 )
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
									theApp.m_pAudioPathComponent->m_pIFramework->RefreshNode( this );

									theApp.m_pAudioPathComponent->m_pIFramework->NotifyNodes( this, AUDIOPATH_NameChange, NULL );
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
						}
					    pRiffStream->Ascend( &ckList, 0 );
					}
					break;

				case DMUS_FOURCC_PORTCONFIGS_LIST:
					while( 0 == pRiffStream->Descend( &ckList, &ckChunk, 0 ) )
					{
						switch( ckList.ckid )
						{
						case FOURCC_LIST:
							switch( ckList.fccType )
							{
							case DMUS_FOURCC_PORTCONFIG_LIST:
								// Load in the port configuration list
								DM_LoadPortConfig( pRiffStream, ckList, lstPortConfigs );
								break;
							}
							break;
						}
						pRiffStream->Ascend( &ckList, 0 );
					}
					break;

				case DMUS_FOURCC_DSBUFFER_LIST:
					{
						bool fHeaderLoaded = false;

						// Initialize ioBufferHeader, in case the DSBUFFATTR_ITEM chunk is not found
						DMUS_IO_BUFFER_ATTRIBUTES_HEADER ioBufferHeader;
						ZeroMemory( &ioBufferHeader, sizeof( DMUS_IO_BUFFER_ATTRIBUTES_HEADER ) );

						// The buffer name - only used when using a predefined buffer
						CString strBufferName;

						while( 0 == pRiffStream->Descend( &ckList, &ckChunk, 0 ) )
						{
							switch( ckList.ckid )
							{
							case DMUS_FOURCC_DSBUFFATTR_ITEM:
								ZeroMemory( &ioBufferHeader, sizeof( DMUS_IO_BUFFER_ATTRIBUTES_HEADER ) );
								cbSize = min( sizeof( DMUS_IO_BUFFER_ATTRIBUTES_HEADER ), ckList.cksize );
								hr = pIStream->Read( &ioBufferHeader, cbSize, &cbRead );
								if( FAILED( hr )
								||  cbRead != cbSize )
								{
									hr = E_FAIL;
								}
								else
								{
									fHeaderLoaded = true;
									if( ioBufferHeader.guidBufferID == GUID_AllZeros )
									{
										CoCreateGuid( &ioBufferHeader.guidBufferID );

										// Flag that we loaded in an old-format file
										SetModified( TRUE );
									}
								}
								break;

							case FOURCC_LIST:
								switch( ckList.fccType )
								{
								case DMUS_FOURCC_UNFO_LIST:
									MMCKINFO ckUnfo;
									while( 0 == pRiffStream->Descend( &ckUnfo, &ckList, 0 ) )
									{
										switch( ckUnfo.ckid )
										{
										case DMUS_FOURCC_UNAM_CHUNK:
											// Read in the name of the DSound buffer
											ReadMBSfromWCS( pIStream, ckUnfo.cksize, &strBufferName );
											break;
										}
										pRiffStream->Ascend( &ckUnfo, 0 );
									}
									break;
								}
								break;

							case FOURCC_RIFF:
								switch( ckList.fccType )
								{
								case DMUS_FOURCC_DSBC_FORM:
									// Load in a DirectSound buffer

									// Assert that the header has been loaded
									ASSERT( fHeaderLoaded );

									// Assert that we're not using a predefined buffer
									ASSERT( !(ioBufferHeader.dwFlags & DMUS_BUFFERF_DEFINED) );

									// Load the buffer
									DM_LoadBuffer( pRiffStream, ckList, ioBufferHeader, lstDSoundBuffers );
									break;
								}
								break;
							}

							pRiffStream->Ascend( &ckList, 0 );
						}

						// Check if we're using a predefined buffer
						if( ioBufferHeader.dwFlags & DMUS_BUFFERF_DEFINED )
						{
							// Using a predefined buffer - need to add the header info to lstDSoundBuffer

							// Create a new stucture to store the buffer in
							ioDSoundBuffer *pioDSoundBuffer = new ioDSoundBuffer;

							// Verify that we didn't run out of memory
							if( pioDSoundBuffer != NULL )
							{
								// Change the GUID if we're using an outdated one
								if( REMOVED_GUID_Buffer_3D == ioBufferHeader.guidBufferID )
								{
									ioBufferHeader.guidBufferID = GUID_Buffer_3D_Dry;
									SetModified( TRUE );
								}

								// Copy the header information
								pioDSoundBuffer->ioBufferHeader = ioBufferHeader;

								// Verify we have a valid GUID
								if( (ioBufferHeader.guidBufferID != GUID_Buffer_Reverb)
								&&	(ioBufferHeader.guidBufferID != GUID_Buffer_EnvReverb)
								/*&&	(ioBufferHeader.guidBufferID != GUID_Buffer_3D)*/
								&&	(ioBufferHeader.guidBufferID != GUID_Buffer_3D_Dry)
								&&	(ioBufferHeader.guidBufferID != GUID_Buffer_Mono)
								&&	(ioBufferHeader.guidBufferID != GUID_Buffer_Stereo) )
								{
									ASSERT(FALSE);
								}

								// Set the name of the buffer
								if( ioBufferHeader.guidBufferID == GUID_Buffer_EnvReverb )
								{
									pioDSoundBuffer->strName.LoadString( IDS_BUFFER_ENVREVERB );
								}
								else
								{
									pioDSoundBuffer->strName = strBufferName;
								}

								// Add the buffer to the end of the list of buffers
								lstDSoundBuffers.AddTail( pioDSoundBuffer );
							}

						}
					}
					break;
				}
				break;
			}

			// Ascend out of the child chunk
			pRiffStream->Ascend( &ckChunk, 0 );
		    dwPos = StreamTell( pIStream );
		}

		// Need to connect up the port configurations with the directsound buffers to make items.
		// Also, any DirectSound buffers that don't match up need to be made into their own items
		DM_CreateItems( lstPortConfigs, lstDSoundBuffers );

		// Connect up all the send effects with their destination mix groups
		ConnectAllSendEffects();

		// No need to ascend out of the main chunk
	}

	// Release the RIFF stream
	pRiffStream->Release();

	// Return the result
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::DM_CreateItems

void CDirectMusicAudioPath::DM_CreateItems( CTypedPtrList< CPtrList, ioPortConfig*> &lstPortConfigs, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers )
{
	CTypedPtrList< CPtrList, ioDSoundBuffer*> lstUsedDSoundBuffers;

	// Iterate through all ioPortConfig structures
	while( !lstPortConfigs.IsEmpty() )
	{
		ioPortConfig *pioPortConfig = lstPortConfigs.RemoveHead();

		// Create a PortOptions, if necessary, from the information in pioPortConfig
		PortOptions *pPortOptions = CreatePortOptions( pioPortConfig->ioPortConfigHeader, pioPortConfig->ioPortParams, pioPortConfig->strPortName );

		// Create the mix groups for this PortConfig
		CreateItemInfos( pioPortConfig, pPortOptions, lstUsedDSoundBuffers, lstDSoundBuffers );

		delete pioPortConfig;
	}

	// Delete all DirectSound buffers that are used by the items we created above
	while( !lstUsedDSoundBuffers.IsEmpty() )
	{
		ioDSoundBuffer *pioDSoundBuffer = lstUsedDSoundBuffers.RemoveHead();
		pioDSoundBuffer->lstBusIDs.RemoveAll();
		while( !pioDSoundBuffer->lstDSoundEffects.IsEmpty() )
		{
			ioDSoundEffect *pioDSoundEffect = pioDSoundBuffer->lstDSoundEffects.RemoveHead();

			delete pioDSoundEffect;
		}

		delete pioDSoundBuffer;
	}

	while( !lstDSoundBuffers.IsEmpty() )
	{
		// Also need to create items from the unused buffers
		ioDSoundBuffer *pioDSoundBuffer = lstDSoundBuffers.RemoveHead();

		ItemInfo *pItemInfo = new ItemInfo;
		if( pItemInfo )
		{
			pItemInfo->strBandName = pioDSoundBuffer->strName;

			// Create a BufferOptions for this buffer
			BufferOptions *pBufferOptions = new BufferOptions;

			if( pBufferOptions )
			{
				pBufferOptions->lstBusIDs.Copy( pioDSoundBuffer->lstBusIDs );
				pBufferOptions->guidBuffer = pioDSoundBuffer->ioBufferHeader.guidBufferID;
				pBufferOptions->dwHeaderFlags = pioDSoundBuffer->ioBufferHeader.dwFlags;
				pBufferOptions->dwBufferFlags = pioDSoundBuffer->ioDSBufferDesc.dwFlags;
				pBufferOptions->lPan = pioDSoundBuffer->ioDSBufferDesc.lPan;
				pBufferOptions->lVolume = pioDSoundBuffer->ioDSBufferDesc.lVolume;
				pBufferOptions->wChannels = pioDSoundBuffer->ioDSBufferDesc.nChannels;

				pBufferOptions->guid3DAlgorithm = pioDSoundBuffer->ioDS3D.guid3DAlgorithm;
				pBufferOptions->ds3DBuffer = pioDSoundBuffer->ioDS3D.ds3d;

				// If using a standard buffer, copy the buffer's GUID
				if( pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED )
				{

					// If the buffer has no busIDs, add the dynamic 0 bus
					if( pBufferOptions->lstBusIDs.GetSize() == 0 )
					{
						pBufferOptions->lstBusIDs.Add( DSBUSID_LEFT );
						pBufferOptions->lstBusIDs.Add( DSBUSID_RIGHT );
						pBufferOptions->wChannels = max( WORD(2), pBufferOptions->wChannels );
					}
				}

				// Create effect list
				POSITION posEffect = pioDSoundBuffer->lstDSoundEffects.GetHeadPosition();
				while( posEffect )
				{
					// Get a pointer to the ioDSoundEffect structure
					ioDSoundEffect *pioDSoundEffect = pioDSoundBuffer->lstDSoundEffects.GetNext( posEffect );

					// Create a new BusEffectInfo to wrap this effect
					BusEffectInfo *pBusEffectInfo = new BusEffectInfo( pioDSoundEffect );
					if( pBusEffectInfo )
					{
						// Add the BusEffectInfo to the list of effects
						pBufferOptions->lstEffects.AddTail( pBusEffectInfo );
					}
				}

				// Attach the buffer to the item
				pItemInfo->lstBuffers.AddTail( pBufferOptions );
			}

			// Add the item to the list of items we manage
			InsertItemInfo( pItemInfo );
		}

		// Delete all the effects in this buffer
		while( !pioDSoundBuffer->lstDSoundEffects.IsEmpty() )
		{
			// Get a pointer to the ioDSoundEffect structure
			ioDSoundEffect *pioDSoundEffect = pioDSoundBuffer->lstDSoundEffects.RemoveHead();

			// Delete the effect
			delete pioDSoundEffect;
		}

		// Delete the buffer itself
		delete pioDSoundBuffer;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::ConnectAllSendEffects

void CDirectMusicAudioPath::ConnectAllSendEffects( void )
{

	// Iterate through the list of items
	POSITION posItem = m_lstItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		ItemInfo *pItemInfo = m_lstItems.GetNext( posItem );

		/* Update hidden buffers
		// If the port doesn't support buffers, skip it
		if( !ItemSupportsBuffers( pItemInfo ) )
		{
			continue;
		}
		*/

		// Iterate through the list of buffers
		POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			// Get a pointer to each buffer
			BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

			/* Update hidden effects
			// If the buffer does not support effects, skip it
			if( !ItemAndBufferSupportEffects( pItemInfo, pBufferOptions ) )
			{
				continue;
			}
			*/

			// Iterate through the list of effects
			POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
			while( posEffect )
			{
				// Get a pointer to each effect
				BusEffectInfo *pBusEffectInfoInList = pBufferOptions->lstEffects.GetNext( posEffect );

				// Check if this is a sending effect
				if( (GUID_DSFX_SEND == pBusEffectInfoInList->m_EffectInfo.m_clsidObject)
				/*||	(GUID_DSFX_STANDARD_I3DL2SOURCE == pBusEffectInfoInList->m_EffectInfo.m_clsidObject)*/ )
				{
					// Initialize the mixgroup pointer to NULL
					pBusEffectInfoInList->m_EffectInfo.m_pSendDestinationMixGroup = NULL;

					// Check if we're not using the standard Env. Reverb GUID
					if( GUID_Buffer_EnvReverb != pBusEffectInfoInList->m_EffectInfo.m_clsidSendBuffer )
					{
						// Check to see if the effect is now pointed at any of our mix groups
						POSITION pos = m_lstItems.GetHeadPosition();
						while( pos )
						{
							// Get a pointer to each item
							ItemInfo *pTmpItem = m_lstItems.GetNext( pos );

							// Check if this item is the one with the buffer we want to use
							if( (pTmpItem->nNumPChannels == 0)
							&&	(pTmpItem->lstBuffers.GetCount() > 0)
							&&	(pTmpItem->lstBuffers.GetHead()->guidBuffer == pBusEffectInfoInList->m_EffectInfo.m_clsidSendBuffer) )
							{
								// Yes - set the pointer and break out of the loop
								pBusEffectInfoInList->m_EffectInfo.m_pSendDestinationMixGroup = pTmpItem;
								break;
							}
						}
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::InsertItemInfo

void CDirectMusicAudioPath::InsertItemInfo( ItemInfo *pItemInfo )
{
	ASSERT( pItemInfo );
	if( pItemInfo )
	{
		// Ensure the MixGroup name exists, and is unique
		if( pItemInfo->strBandName.IsEmpty() )
		{
			pItemInfo->strBandName = GetNewMixgroupName();
		}
		else
		{
			GetUniqueMixGroupName( pItemInfo );
		}

		// Add Item to the list of items we manage
		m_lstItems.AddTail( pItemInfo );

		// Now, ensure all the effect instance names are unique
		POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

			if( !pBufferOptions->lstEffects.IsEmpty() )
			{
				POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();

				// Skip the first effect
				pBufferOptions->lstEffects.GetNext( posEffect );

				// Iterate through the effects
				while( posEffect )
				{
					BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );
					// Now, check if the effect already exists
					GetUniqueEffectInstanceNameIfNecessary( &pBusEffectInfo->m_EffectInfo );
				}

				// Now, check the first effect
				GetUniqueEffectInstanceNameIfNecessary( &(pBufferOptions->lstEffects.GetHead()->m_EffectInfo) );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::GetItemForPChannel

ItemInfo *CDirectMusicAudioPath::GetItemForPChannel( DWORD dwPChannel )
{
	// Iterate through the list of items
	POSITION posItem = m_lstItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		ItemInfo *pItem = m_lstItems.GetNext( posItem );

		// Iterate through the array of PChannels
		for( int i=0; i< pItem->nNumPChannels; i++ )
		{
			// Check if this PChannel is the one we're looking for
			if( pItem->adwPChannels[i] == dwPChannel )
			{
				// Yes - return pItem
				return pItem;
			}
		}
	}
	
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::CreateItemInfos

void CDirectMusicAudioPath::CreateItemInfos( ioPortConfig *pioPortConfig, PortOptions *pPortOptions, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstUsedDSoundBuffers, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers )
{
	// Create a mix group for each item in lstPChannelToBuffer
	while( !pioPortConfig->lstPChannelToBuffer.IsEmpty() )
	{
		ioPChannelToBuffer *pioPChannelToBuffer = pioPortConfig->lstPChannelToBuffer.RemoveHead();

		CreateItemFromPChannelToBuffer( pioPChannelToBuffer, pPortOptions, lstUsedDSoundBuffers, lstDSoundBuffers );

		delete pioPChannelToBuffer;
	}

	// Create a mix group for each item in lstDesignMixGroup
	while( !pioPortConfig->lstDesignMixGroup.IsEmpty() )
	{
		ioDesignMixGroup *pioDesignMixGroup = pioPortConfig->lstDesignMixGroup.RemoveHead();

		CreateItemFromDesignInfo( pioDesignMixGroup, pPortOptions );

		delete pioDesignMixGroup;
	}

	// Create an array of any left over PChannels
	CDWordArray adwPChannelsToAdd;
	for( DWORD dwPChannel = 0; dwPChannel < pioPortConfig->ioPortConfigHeader.dwPChannelCount; dwPChannel++ )
	{
		if( GetItemForPChannel( dwPChannel + pioPortConfig->ioPortConfigHeader.dwPChannelBase ) == NULL )
		{
			adwPChannelsToAdd.Add( dwPChannel + pioPortConfig->ioPortConfigHeader.dwPChannelBase );
		}
	}

	// Finally, create a mix group for any left over PChannels
	if( adwPChannelsToAdd.GetSize() > 0 )
	{
		CreateItemFromPChannelArray( adwPChannelsToAdd, pPortOptions );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::CreateItemFromPChannelToBuffer

void CDirectMusicAudioPath::CreateItemFromPChannelToBuffer( ioPChannelToBuffer *pioPChannelToBuffer, PortOptions *pPortOptions, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstUsedDSoundBuffers, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers )
{
	// Look for an existing item with the same port and buffer
	POSITION pos = m_lstItems.GetHeadPosition();
	while( pos )
	{
		ItemInfo *pItemInfo = m_lstItems.GetNext( pos );

		// Check if the ports are the same
		if( pItemInfo->pPortOptions
		&&	pItemInfo->pPortOptions->IsEqual( pPortOptions ) )
		{
			bool fEqual = true;
			POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
			POSITION posGuids = pioPChannelToBuffer->lstGuids.GetHeadPosition();
			while( posBuffer && posGuids)
			{
				BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );
				GUID *pGuid = pioPChannelToBuffer->lstGuids.GetNext( posGuids );
				if( pBufferOptions->guidBuffer != *pGuid )
				{
					fEqual = false;
					break;
				}
			}

			// If all the GUIDs are equal, and the lists are the same length (both end at the same time)
			if( fEqual && !posBuffer && !posGuids )
			{
				// Just add the PChannels to this pItemInfo
				DWORD *adwNewPChannels = NULL;
				if( pItemInfo->nNumPChannels > 0 )
				{
					adwNewPChannels = new DWORD[pItemInfo->nNumPChannels + pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelCount];
					if( adwNewPChannels )
					{
						// Copy existing PChannels
						memcpy( adwNewPChannels, pItemInfo->adwPChannels, sizeof(DWORD) * pItemInfo->nNumPChannels );

						// Copy new PChannels
						for( DWORD i = 0; i < pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelCount; i++ )
						{
							adwNewPChannels[ pItemInfo->nNumPChannels + i ] = pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelBase + i;
						}

						// delete old array
						delete[] pItemInfo->adwPChannels;

					}
				}
				else
				{
					adwNewPChannels = new DWORD[pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelCount];
					if( adwNewPChannels )
					{
						// Copy new PChannels
						for( DWORD i = 0; i < pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelCount; i++ )
						{
							adwNewPChannels[ i ] = pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelBase + i;
						}
					}
				}

				if( adwNewPChannels )
				{
					// Point to new array
					pItemInfo->adwPChannels = adwNewPChannels;

					// Update count of PChannels
					pItemInfo->nNumPChannels += pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelCount;
				}

				// Return
				return;
			}
		}
	}

	// Didn't find an existing Item - must create a new one
	ItemInfo *pItemInfo = new ItemInfo;
	if( pItemInfo )
	{
		// Create an array to store these PChannels in
		pItemInfo->adwPChannels = new DWORD[pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelCount];
		if( pItemInfo->adwPChannels )
		{
			// Copy new PChannels
			for( DWORD i = 0; i < pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelCount; i++ )
			{
				pItemInfo->adwPChannels[ i ] = pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelBase + i;
			}

			// Set count of PChannels
			pItemInfo->nNumPChannels = pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelCount;
		}

		// Set the mix group name
		pItemInfo->strBandName = pioPChannelToBuffer->strMixGroupName;

		// Copy the port settings
		pItemInfo->pPortOptions = pPortOptions;

		// Now, connect the item to the DirectSound buffers
		while( !pioPChannelToBuffer->lstGuids.IsEmpty() )
		{
			GUID *pGuid = pioPChannelToBuffer->lstGuids.RemoveHead();

			// Iterate through the existing DSound buffers
			POSITION posBuffer = lstUsedDSoundBuffers.GetHeadPosition();
			while( posBuffer )
			{
				ioDSoundBuffer *pioDSoundBuffer = lstUsedDSoundBuffers.GetNext( posBuffer );
				if( pioDSoundBuffer->ioBufferHeader.guidBufferID == *pGuid )
				{
					// Shouldn't re-use any buffers
					ASSERT(FALSE);
				}
			}

			// Iterate through the unused DSound buffers
			posBuffer = lstDSoundBuffers.GetHeadPosition();
			while( posBuffer )
			{
				ioDSoundBuffer *pioDSoundBuffer = lstDSoundBuffers.GetNext( posBuffer );

				// Check if this is the buffer to connect to
				if( pioDSoundBuffer->ioBufferHeader.guidBufferID == *pGuid )
				{
					// Create a BufferOptions for this buffer
					BufferOptions *pBufferOptions = new BufferOptions;

					if( pBufferOptions )
					{
						pBufferOptions->lstBusIDs.Copy( pioDSoundBuffer->lstBusIDs );
						pBufferOptions->dwBufferFlags = pioDSoundBuffer->ioDSBufferDesc.dwFlags;
						pBufferOptions->dwHeaderFlags = pioDSoundBuffer->ioBufferHeader.dwFlags;
						pBufferOptions->wChannels = pioDSoundBuffer->ioDSBufferDesc.nChannels;
						pBufferOptions->lPan = pioDSoundBuffer->ioDSBufferDesc.lPan;
						pBufferOptions->lVolume = pioDSoundBuffer->ioDSBufferDesc.lVolume;

						pBufferOptions->guid3DAlgorithm = pioDSoundBuffer->ioDS3D.guid3DAlgorithm;
						pBufferOptions->ds3DBuffer = pioDSoundBuffer->ioDS3D.ds3d;

						// Copy the buffer's GUID
						pBufferOptions->guidBuffer = pioDSoundBuffer->ioBufferHeader.guidBufferID;

						// Create effect list
						POSITION posEffect = pioDSoundBuffer->lstDSoundEffects.GetHeadPosition();
						while( posEffect )
						{
							// Get a pointer to the ioDSoundEffect structure
							ioDSoundEffect *pioDSoundEffect = pioDSoundBuffer->lstDSoundEffects.GetNext( posEffect );

							// Create a new BusEffectInfo to wrap this effect
							BusEffectInfo *pBusEffectInfo = new BusEffectInfo( pioDSoundEffect );
							if( pBusEffectInfo )
							{
								// Add the BusEffectInfo to the list of effects
								pBufferOptions->lstEffects.AddTail( pBusEffectInfo );
							}
						}

						// Attach the buffer to the item
						pItemInfo->lstBuffers.AddTail( pBufferOptions );

						// Move the ioBuffer into the list of used buffers
						lstUsedDSoundBuffers.AddTail( pioDSoundBuffer );

						// See if this buffer has an effect design GUID
						if( pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED )
						{
							// Initialize the GUID to a random value
							CoCreateGuid( &pBufferOptions->guidDesignEffectGUID );

							// Iterate through the list of GUIDs
							POSITION pos = pioPChannelToBuffer->lstDesignGuids.GetHeadPosition();
							while( pos )
							{
								// Save the current position
								const POSITION posCurrent = pos;

								// Get the current GUID
								GUID *pGUID = pioPChannelToBuffer->lstDesignGuids.GetNext( pos );

								// Is this the GUID we're looking for?
								if( *pGUID == pBufferOptions->guidBuffer )
								{
									// Yes - delete it from the list
									delete pGUID;
									pioPChannelToBuffer->lstDesignGuids.RemoveAt( posCurrent );

									// Get the next GUID
									pGUID = pioPChannelToBuffer->lstDesignGuids.GetAt( pos );

									// Use it as the design GUID
									pBufferOptions->guidDesignEffectGUID = *pGUID;

									// delete it from the list
									delete pGUID;
									pioPChannelToBuffer->lstDesignGuids.RemoveAt( pos );
									break;
								}
							}
						}

						// Remove the ioBuffer from the list of unused buffers
						posBuffer = lstDSoundBuffers.Find( pioDSoundBuffer );
						ASSERT( posBuffer );
						if( posBuffer )
						{
							lstDSoundBuffers.RemoveAt( posBuffer );
							break;
						}
					}
				}
			}

			// Delete the GUID
			delete pGuid;
		}

		// Add Item to the list of items we manage
		InsertItemInfo( pItemInfo );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::CreateItemFromPChannelArray

void CDirectMusicAudioPath::CreateItemFromPChannelArray( CDWordArray &adwPChannelsToAdd, PortOptions *pPortOptions )
{
	// Didn't find an existing Item - must create a new one
	ItemInfo *pItemInfo = new ItemInfo;
	if( pItemInfo )
	{
		// Create an array to store these PChannels in
		pItemInfo->adwPChannels = new DWORD[adwPChannelsToAdd.GetSize()];
		if( pItemInfo->adwPChannels )
		{
			// Copy new PChannels
			for( DWORD i = 0; i < (unsigned)adwPChannelsToAdd.GetSize(); i++ )
			{
				pItemInfo->adwPChannels[ i ] = adwPChannelsToAdd[ i ];
			}

			// Set count of PChannels
			pItemInfo->nNumPChannels = adwPChannelsToAdd.GetSize();
		}

		// Set the mix group name
		pItemInfo->strBandName = GetNewMixgroupName();

		// Copy the port settings
		pItemInfo->pPortOptions = pPortOptions;

		// Add Item to the list of items we manage
		InsertItemInfo( pItemInfo );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::CreateItemFromDesignInfo

void CDirectMusicAudioPath::CreateItemFromDesignInfo( ioDesignMixGroup *pioDesignMixGroup, PortOptions *pPortOptions )
{
	// Look for an existing item with the same port and no buffers
	POSITION pos = m_lstItems.GetHeadPosition();
	while( pos )
	{
		ItemInfo *pItemInfo = m_lstItems.GetNext( pos );

		// Check if the item's buffer list is empty
		// Check if the ports are the same
		// Check if the mix group name is the same
		if( pItemInfo->lstBuffers.IsEmpty()
		&&	pItemInfo->pPortOptions
		&&	pItemInfo->pPortOptions->IsEqual( pPortOptions )
		&&	pItemInfo->strBandName == pioDesignMixGroup->strMixGroupName )
		{
			// Just add the PChannels to this pItemInfo
			DWORD *adwNewPChannels = NULL;
			if( pItemInfo->nNumPChannels > 0 )
			{
				adwNewPChannels = new DWORD[pItemInfo->nNumPChannels + pioDesignMixGroup->dwPChannelSpan];
				if( adwNewPChannels )
				{
					// Copy existing PChannels
					memcpy( adwNewPChannels, pItemInfo->adwPChannels, sizeof(DWORD) * pItemInfo->nNumPChannels );

					// Copy new PChannels
					for( DWORD i = 0; i < pioDesignMixGroup->dwPChannelSpan; i++ )
					{
						adwNewPChannels[ pItemInfo->nNumPChannels + i ] = pioDesignMixGroup->dwPChannelStart + i;
					}

					// delete old array
					delete[] pItemInfo->adwPChannels;
				}
			}
			else
			{
				adwNewPChannels = new DWORD[pioDesignMixGroup->dwPChannelSpan];
				if( adwNewPChannels )
				{
					// Copy new PChannels
					for( DWORD i = 0; i < pioDesignMixGroup->dwPChannelSpan; i++ )
					{
						adwNewPChannels[ i ] = pioDesignMixGroup->dwPChannelStart + i;
					}
				}
			}

			if( adwNewPChannels )
			{
				// Point to new array
				pItemInfo->adwPChannels = adwNewPChannels;

				// Update count of PChannels
				pItemInfo->nNumPChannels += pioDesignMixGroup->dwPChannelSpan;
			}

			// Return
			return;
		}
	}

	// Didn't find an existing Item - must create a new one
	ItemInfo *pItemInfo = new ItemInfo;
	if( pItemInfo )
	{
		// Create an array to store these PChannels in
		pItemInfo->adwPChannels = new DWORD[pioDesignMixGroup->dwPChannelSpan];
		if( pItemInfo->adwPChannels )
		{
			// Copy new PChannels
			for( DWORD i = 0; i < pioDesignMixGroup->dwPChannelSpan; i++ )
			{
				pItemInfo->adwPChannels[ i ] = pioDesignMixGroup->dwPChannelStart + i;
			}

			// Set count of PChannels
			pItemInfo->nNumPChannels = pioDesignMixGroup->dwPChannelSpan;
		}

		// Set the mix group name
		pItemInfo->strBandName = pioDesignMixGroup->strMixGroupName;

		// Copy the port settings
		pItemInfo->pPortOptions = pPortOptions;

		// Add Item to the list of items we manage
		InsertItemInfo( pItemInfo );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::DM_LoadDesignInfo

HRESULT CDirectMusicAudioPath::DM_LoadDesignInfo( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO &ckChunk )
{
	IStream *pIStream = pIRiffStream->GetStream();
	if( pIStream == NULL )
	{
		return E_FAIL;
	}

	// Look through all chunks within ckChunk
	MMCKINFO ckSubChunk;
	while( 0 == pIRiffStream->Descend( &ckSubChunk, &ckChunk, 0 ) )
	{
		switch( ckSubChunk.ckid )
		{
		case DMUSPROD_FOURCC_AUDIOPATH_UI_CHUNK:
			{
				// Read the UI data
				ioAudioPathUI iAudioPathUI;
				ZeroMemory( &iAudioPathUI, sizeof(ioAudioPathUI) );

				DWORD cbRead, cbToRead = min( ckSubChunk.cksize, sizeof(ioAudioPathUI) );
				if( SUCCEEDED( pIStream->Read( &iAudioPathUI, cbToRead, &cbRead ) )
				&&	(cbRead == cbToRead) )
				{
					m_wSplitterPos = iAudioPathUI.wSplitterPos;
					m_wMixGroupWidth = iAudioPathUI.wMixGroupWidth;
					m_wBusWidth = iAudioPathUI.wBusWidth;
					m_wBufferWidth = iAudioPathUI.wBufferWidth;
					m_wEffectWidth = iAudioPathUI.wEffectWidth;
				}
			}
			break;
		}

		// Ascend out of the sub chunk
		pIRiffStream->Ascend( &ckSubChunk, 0 );
	}

	// Release the IStream interface
	pIStream->Release();

	// We succeeded
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// ValidateEffectFlags

bool ValidateEffectFlags( REFGUID clsidEffect, DWORD &dwEffectFlags )
{
	bool fChanged = false;

	// If a send effect, ensure that neither of the DSFX_LOC* flags are set
	if( (GUID_DSFX_SEND == clsidEffect)
	/*||	(GUID_DSFX_STANDARD_I3DL2SOURCE == clsidEffect)*/ )
	{
		if( dwEffectFlags & (DSFX_LOCSOFTWARE | DSFX_LOCHARDWARE) )
		{
			dwEffectFlags &= ~(DSFX_LOCSOFTWARE | DSFX_LOCHARDWARE);
			fChanged = true;
		}
	}

	// Verify that DSFX_LOCHARDWARE is not set
	if( dwEffectFlags & DSFX_LOCHARDWARE )
	{
		dwEffectFlags &= ~DSFX_LOCHARDWARE;
		fChanged = true;
	}

	// Verify that DSFX_OPTIONAL is not set
	if( dwEffectFlags & DSFX_OPTIONAL )
	{
		dwEffectFlags &= ~DSFX_OPTIONAL;
		fChanged = true;
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::DM_LoadEffect

ioDSoundEffect *CDirectMusicAudioPath::DM_LoadEffect( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO &ckChunk )
{
	// Create a new ioDSoundEffect structure
	ioDSoundEffect *pioDSoundEffect = new ioDSoundEffect;
	if( pioDSoundEffect == NULL )
	{
		// Check that the allocation succeeded
		return NULL;
	}

	// Get a pointer to the IStream
	IStream *pStream = pIRiffStream->GetStream();

	DWORD cbToRead, cbRead;
	MMCKINFO ckSubChunk, ckList;
	while( 0 == pIRiffStream->Descend( &ckSubChunk, &ckChunk, 0 ) )
	{
		switch( ckSubChunk.ckid )
		{
		case DMUS_FOURCC_DSFX_CHUNK:
			if( ckSubChunk.cksize == sizeof( DX8_BETA1_DSOUND_IO_DXDMO_HEADER ) )
			{
				// Load in the DX8 Beta1 structure
				DX8_BETA1_DSOUND_IO_DXDMO_HEADER ioDMOHeader;
				pStream->Read( &ioDMOHeader, sizeof( DX8_BETA1_DSOUND_IO_DXDMO_HEADER ), &cbRead );
				pioDSoundEffect->ioFXHeader.dwEffectFlags = ioDMOHeader.dsfxd.dwFlags;
				pioDSoundEffect->ioFXHeader.guidDSFXClass = ioDMOHeader.dsfxd.guidDSFXClass;

				// Flag that we loaded in an old-format file
				SetModified( TRUE );
			}
			else
			{
				// Read in the effect header information
				cbToRead = min( ckSubChunk.cksize, sizeof( DSOUND_IO_DXDMO_HEADER ) );
				pStream->Read( &pioDSoundEffect->ioFXHeader, cbToRead, &cbRead );
			}
			break;

		case FOURCC_DMUS_EFFECT_DATA_CHUNK:
			// Release any existing data
			if( pioDSoundEffect->pStreamData )
			{
				pioDSoundEffect->pStreamData->Release();
				pioDSoundEffect->pStreamData = NULL;
			}

			// Try and allocate a new stream to store the data in
			if( SUCCEEDED( theApp.m_pAudioPathComponent->m_pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pioDSoundEffect->pStreamData ) ) )
			{
				// Copy the data to the new stream
				ULARGE_INTEGER uliRead, uliWritten, uliToCopy;
				uliToCopy.QuadPart = ckSubChunk.cksize;
				pStream->CopyTo( pioDSoundEffect->pStreamData, uliToCopy, &uliRead, &uliWritten );
			}
			break;

		case DMUSPROD_FOURCC_EFFECT_GUID_CHUNK:
			if( ckSubChunk.cksize >= sizeof(GUID) )
			{
				pStream->Read( &(pioDSoundEffect->guidDesignGUID), sizeof(GUID), NULL );
			}
			break;

		case FOURCC_LIST:
			switch( ckSubChunk.fccType )
			{
			case DMUS_FOURCC_UNFO_LIST:
				while( 0 == pIRiffStream->Descend( &ckList, &ckSubChunk, 0 ) )
				{
					switch( ckList.ckid )
					{
					case DMUS_FOURCC_UNAM_CHUNK:
						// Read in the effect's instance name
						ReadMBSfromWCS( pStream, ckList.cksize, &pioDSoundEffect->strInstanceName );
						break;

					case FOURCC_EFFECT_NAME_CHUNK:
						// Read in the effect's standard name
						ReadMBSfromWCS( pStream, ckList.cksize, &pioDSoundEffect->strEffectName );
						break;
					}

					pIRiffStream->Ascend( &ckList, 0 );
				}
				break;
			}
			break;
		}

		pIRiffStream->Ascend( &ckSubChunk, 0 );
	}

	// Release the IStream
	pStream->Release();

	// Double-check the effect flags to make sure they are valid
	if( ValidateEffectFlags( pioDSoundEffect->ioFXHeader.guidDSFXClass, pioDSoundEffect->ioFXHeader.dwEffectFlags ) )
	{
		SetModified( TRUE );
	}

	// Make sure the design-time guid is not all zeros
	if( GUID_AllZeros == pioDSoundEffect->guidDesignGUID )
	{
		::CoCreateGuid( &pioDSoundEffect->guidDesignGUID );
	}

	// Make sure strEffectName is not empty
	if( pioDSoundEffect->strEffectName.IsEmpty() )
	{
		WCHAR awchDMOName[65];
		awchDMOName[0] = 0;
		GetDMOName( pioDSoundEffect->ioFXHeader.guidDSFXClass, awchDMOName );
		if( awchDMOName[0] == 0 )
		{
			if( pioDSoundEffect->ioFXHeader.guidDSFXClass == GUID_DSFX_SEND )
			{
				pioDSoundEffect->strEffectName.LoadString( IDS_SEND_TEXT );
			}
			else
			{
				// TODO: Change this to IDS_UNKNOWN_EFFECT_NAME
				pioDSoundEffect->strEffectName.LoadString( IDS_UNKNOWN_SYNTH_NAME );
			}
		}
		else
		{
			pioDSoundEffect->strEffectName = awchDMOName;
		}
	}

	// Make sure strInstanceName is not empty
	if( pioDSoundEffect->strInstanceName.IsEmpty() )
	{
		CString strNbr;
		strNbr.Format( "%d", 1 ); 
		pioDSoundEffect->strInstanceName = pioDSoundEffect->strEffectName + strNbr;
	}

	return pioDSoundEffect;
}


/////////////////////////////////////////////////////////////////////////////
// ValidateBufferFlags

bool ValidateBufferFlags( DWORD &dwBufferFlags )
{
	bool fChanged = false;

	// Verify that DSBCAPS_PRIMARYBUFFER is not set
	if( dwBufferFlags & DSBCAPS_PRIMARYBUFFER )
	{
		dwBufferFlags &= ~DSBCAPS_PRIMARYBUFFER;
		fChanged = true;
	}

	// Verify that none of DSBCAPS_STATIC,
	// DSBCAPS_CTRLPOSITIONNOTIFY, or DSBCAPS_GETCURRENTPOSITION2 are set
	if( dwBufferFlags & (DSBCAPS_STATIC | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2) )
	{
		dwBufferFlags &= ~(DSBCAPS_STATIC | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2);
		fChanged = true;
	}

	// Verify that DSBCAPS_MUTE3DATMAXDISTANCE is only set when DSBCAPS_CTRL3D is also set
	if( dwBufferFlags & DSBCAPS_MUTE3DATMAXDISTANCE )
	{
		if( (dwBufferFlags & DSBCAPS_CTRL3D) == 0 )
		{
			dwBufferFlags &= ~DSBCAPS_MUTE3DATMAXDISTANCE;
			fChanged = true;
		}
	}

	// Verify that if DSBCAPS_MIXIN is set, DSBCAPS_LOCDEFER is not set
	if( dwBufferFlags & DSBCAPS_MIXIN )
	{
		if( dwBufferFlags & DSBCAPS_LOCDEFER )
		{
			dwBufferFlags &= ~DSBCAPS_LOCDEFER;
			fChanged = true;
		}
	}

	// Verify that if DSBCAPS_LOCHARDWARE is set, neither DSBCAPS_LOCSOFTWARE nor DSBCAPS_LOCDEFER is set
	if( (dwBufferFlags & DSBCAPS_LOCHARDWARE)
	&&	(dwBufferFlags & (DSBCAPS_LOCSOFTWARE | DSBCAPS_LOCDEFER)) )
	{
		dwBufferFlags &= ~(DSBCAPS_LOCSOFTWARE | DSBCAPS_LOCDEFER);
		fChanged = true;
	}

	// Verify that if DSBCAPS_LOCSOFTWARE is set, DSBCAPS_LOCDEFER is not set
	if( (dwBufferFlags & DSBCAPS_LOCSOFTWARE)
	&&	(dwBufferFlags & DSBCAPS_LOCDEFER) )
	{
		dwBufferFlags &= ~DSBCAPS_LOCDEFER;
		fChanged = true;
	}

	// Verify that if DSBCAPS_GLOBALFOCUS is set, DSBCAPS_STICKYFOCUS is not set
	if( (dwBufferFlags & DSBCAPS_GLOBALFOCUS)
	&&	(dwBufferFlags & DSBCAPS_STICKYFOCUS) )
	{
		dwBufferFlags &= ~DSBCAPS_STICKYFOCUS;
		fChanged = true;
	}

	// Verify that if DSBCAPS_CTRL3D is set, DSBCAPS_CTRLPAN is not set
	if( (dwBufferFlags & DSBCAPS_CTRL3D)
	&&	(dwBufferFlags & DSBCAPS_CTRLPAN) )
	{
		dwBufferFlags &= ~DSBCAPS_CTRLPAN;
		fChanged = true;
	}

	// Verify that either DSBCAPS_CTRL3D or DSBCAPS_CTRLPAN is set
	if( !(dwBufferFlags & DSBCAPS_CTRL3D)
	&&	!(dwBufferFlags & DSBCAPS_CTRLPAN) )
	{
		dwBufferFlags |= DSBCAPS_CTRLPAN;
		fChanged = true;
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::DM_LoadBuffer

HRESULT CDirectMusicAudioPath::DM_LoadBuffer( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO &ckChunk, DMUS_IO_BUFFER_ATTRIBUTES_HEADER &ioBufferHeader, CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers )
{
	// Create a new stucture to store the buffer in
	ioDSoundBuffer *pioDSoundBuffer = new ioDSoundBuffer;

	// Verify that we didn't run out of memory
	if( pioDSoundBuffer == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Copy the buffer header information
	pioDSoundBuffer->ioBufferHeader = ioBufferHeader;

	// Get a pointer to the IStream
	IStream *pStream = pIRiffStream->GetStream();

	MMCKINFO ckSubChunk, ckList;
	DWORD cbToRead, cbRead;
	while( 0 == pIRiffStream->Descend( &ckSubChunk, &ckChunk, 0 ) )
	{
		switch( ckSubChunk.ckid )
		{
		case DMUS_FOURCC_GUID_CHUNK:
			// Read in the GUID for the buffer
			if( ckSubChunk.cksize >= sizeof(GUID) )
			{
				GUID guidBuffer;
				pStream->Read( &guidBuffer, sizeof(GUID), &cbRead );

				// Just verify that it is the same as the GUID in the header
				ASSERT( guidBuffer == ioBufferHeader.guidBufferID );
			}
			else
			{
				ASSERT(FALSE);
			}
			break;

		case DMUS_FOURCC_DSBD_CHUNK:
			if( ckSubChunk.cksize == sizeof( DX8_BETA1_DSOUND_IO_DSBUFFERDESC ) )
			{
				// Load in the DX8 Beta1 structure
				DX8_BETA1_DSOUND_IO_DSBUFFERDESC ioDSBufferDesc;
				pStream->Read( &ioDSBufferDesc, sizeof( DX8_BETA1_DSOUND_IO_DSBUFFERDESC ), &cbRead );
				pioDSoundBuffer->ioDSBufferDesc.dwFlags = ioDSBufferDesc.dsbd.dwFlags;
				pioDSoundBuffer->ioDSBufferDesc.nChannels = ioDSBufferDesc.wfx.nChannels;
				pioDSoundBuffer->ioDSBufferDesc.lVolume = 0;
				pioDSoundBuffer->ioDSBufferDesc.lPan = ioDSBufferDesc.lPan;
				pioDSoundBuffer->ioDSBufferDesc.dwReserved = 0;
				pioDSoundBuffer->ioDS3D.guid3DAlgorithm = ioDSBufferDesc.dsbd.guid3DAlgorithm;

				// Flag that we loaded in an old-format file
				SetModified( TRUE );
			}
			else
			{
				// Read in the buffer descriptor
				cbToRead = min( ckSubChunk.cksize, sizeof( DSOUND_IO_DSBUFFERDESC ) );
				pStream->Read( &pioDSoundBuffer->ioDSBufferDesc, cbToRead, &cbRead );
			}

			// Double-check the buffer flags to make sure they are valid
			if( ValidateBufferFlags( pioDSoundBuffer->ioDSBufferDesc.dwFlags ) )
			{
				SetModified( TRUE );
			}

			// Double-check that the 3D parameter size is set correctly
			if( pioDSoundBuffer->ioDS3D.ds3d.dwSize != sizeof( DS3DBUFFER ) )
			{
				pioDSoundBuffer->ioDS3D.ds3d.dwSize = sizeof( DS3DBUFFER );
				SetModified( TRUE );
			}

			// Double-check that the number of channels is valid
			if( pioDSoundBuffer->ioDSBufferDesc.nChannels > 99 )
			{
				pioDSoundBuffer->ioDSBufferDesc.nChannels = 99;
				SetModified( TRUE );
			}
			break;

		case DMUS_FOURCC_BSID_CHUNK:
			{
				// Read in an array of DWORDs, representing the bus IDs
				DWORD dwTemp;
				long lSize = ckSubChunk.cksize;
				while( lSize >= sizeof(DWORD) )
				{
					pStream->Read( &dwTemp, sizeof(DWORD), &cbRead );

					pioDSoundBuffer->lstBusIDs.Add( dwTemp );
					lSize -= sizeof(DWORD);
				}

				// Fix 46903: DMP: Order of busses not correct if users does not list them in ascending order
				// Bubble sort the BusIDs
				bool fContinue = true;
				while( fContinue )
				{
					fContinue = false;

					for( int nIndex = pioDSoundBuffer->lstBusIDs.GetSize() - 1; nIndex > 0; nIndex-- )
					{
						// nIndex is at least 1
						ASSERT( nIndex > 0 );

						// If the items are out of order
						if( pioDSoundBuffer->lstBusIDs.GetAt( nIndex ) < pioDSoundBuffer->lstBusIDs.GetAt( nIndex - 1 ) )
						{
							// Set fContinue
							fContinue = true;

							// Reverse the items
							DWORD dwTemp = pioDSoundBuffer->lstBusIDs.GetAt( nIndex );
							pioDSoundBuffer->lstBusIDs.SetAt( nIndex, pioDSoundBuffer->lstBusIDs.GetAt( nIndex - 1 ) );
							pioDSoundBuffer->lstBusIDs.SetAt( nIndex - 1, dwTemp );

							// Flag that something changed
							SetModified( TRUE );
						}
					}
				}
			}
			break;

		case DMUS_FOURCC_DS3D_CHUNK:
			if( ckSubChunk.cksize == sizeof( DX8_BETA1_DSOUND_IO_3D ) )
			{
				// Load in the DX8 Beta1 structure
				DX8_BETA1_DSOUND_IO_3D ioDSound3D;
				pStream->Read( &ioDSound3D, sizeof( DX8_BETA1_DSOUND_IO_3D ), &cbRead );
				pioDSoundBuffer->ioDS3D.ds3d = ioDSound3D.ds3d;
				// The 3D guid was loaded from the DMUS_FOURCC_DSBD_CHUNK chunk

				// Flag that we loaded in an old-format file
				SetModified( TRUE );
			}
			else
			{
				// Read in the 3D position
				cbToRead = min( ckSubChunk.cksize, sizeof( DSOUND_IO_3D ) );
				pStream->Read( &pioDSoundBuffer->ioDS3D, cbToRead, &cbRead );
			}
			break;


		case FOURCC_LIST:
			switch( ckSubChunk.fccType )
			{
			case DMUS_FOURCC_UNFO_LIST:
				while( 0 == pIRiffStream->Descend( &ckList, &ckSubChunk, 0 ) )
				{
					switch( ckList.ckid )
					{
					case DMUS_FOURCC_UNAM_CHUNK:
						// Read in the name of the DSound buffer
						ReadMBSfromWCS( pStream, ckList.cksize, &pioDSoundBuffer->strName );
						break;
					}
					pIRiffStream->Ascend( &ckList, 0 );
				}
				break;

			case DMUS_FOURCC_DSBC_LIST:
				while( 0 == pIRiffStream->Descend( &ckList, &ckSubChunk, 0 ) )
				{
					switch( ckList.ckid )
					{
					case FOURCC_RIFF:
						switch( ckList.fccType )
						{
						case DMUS_FOURCC_DSFX_FORM:
							// Load an effect
							ioDSoundEffect *pNewEffect = DM_LoadEffect( pIRiffStream, ckList );
							if( pNewEffect )
							{
								// If the load succeeded, add the effect to the end of the effect chain
								pioDSoundBuffer->lstDSoundEffects.AddTail( pNewEffect );
							}
							break;
						}
						break;
					}
					pIRiffStream->Ascend( &ckList, 0 );
				}
				break;
			}
			break;
		}

		pIRiffStream->Ascend( &ckSubChunk, 0 );
	}

	// Release the IStream
	pStream->Release();

	lstDSoundBuffers.AddTail( pioDSoundBuffer );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::DM_LoadPortConfig

HRESULT CDirectMusicAudioPath::DM_LoadPortConfig( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO &ckChunk, CTypedPtrList< CPtrList, ioPortConfig*> &lstPortConfigs )
{
	// Try and create a new ioPortConfig structure
	ioPortConfig *pioPortConfig = new ioPortConfig;
	if( pioPortConfig == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Get a pointer to the IStream
	IStream *pStream = pIRiffStream->GetStream();

	// Now, descend into each chunk
	MMCKINFO ckSubChunk, ckList;
	DWORD cbRead, cbToRead;
	while( 0 == pIRiffStream->Descend( &ckSubChunk, &ckChunk, 0 ) )
	{
		switch( ckSubChunk.ckid )
		{
		case DMUS_FOURCC_PORTCONFIG_ITEM:
			// Load in the port config data
			cbToRead = min( ckSubChunk.cksize, sizeof( DMUS_IO_PORTCONFIG_HEADER ) );
			pStream->Read( &pioPortConfig->ioPortConfigHeader, cbToRead, &cbRead );
			if( pioPortConfig->ioPortConfigHeader.guidPort == GUID_NULL )
			{
				pioPortConfig->ioPortConfigHeader.guidPort = GUID_Synth_Default;

				// Flag that we loaded in an old-format file
				SetModified( TRUE );
			}
			break;

		case DMUS_FOURCC_PORTPARAMS_ITEM:
			// Load in the port parameters
			cbToRead = min( ckSubChunk.cksize, sizeof( DMUS_PORTPARAMS8 ) );
			pStream->Read( &pioPortConfig->ioPortParams, cbToRead, &cbRead );
			break;

		case FOURCC_LIST:
			switch( ckSubChunk.fccType )
			{
			case DMUS_FOURCC_UNFO_LIST:
				while( 0 == pIRiffStream->Descend( &ckList, &ckSubChunk, 0 ) )
				{
					switch( ckList.ckid )
					{
					case FOURCC_PORT_NAME_CHUNK:
						// Load in the standard name of the port
						ReadMBSfromWCS( pStream, ckList.cksize, &pioPortConfig->strPortName );
						break;
					}
					pIRiffStream->Ascend( &ckList, 0 );
				}
				break;

			case DMUS_FOURCC_PCHANNELS_LIST:
				{
					// Initialize the pointer to the strucutre that stores information for a range of PChannels
					ioPChannelToBuffer *pioPChannelToBuffer = NULL;

					while( 0 == pIRiffStream->Descend( &ckList, &ckSubChunk, 0 ) )
					{
						switch( ckList.ckid )
						{
						case DMUS_FOURCC_PCHANNELS_ITEM:
							// Create a new structure to store the PChannel->Buffer information in
							pioPChannelToBuffer = new ioPChannelToBuffer;

							// Verify that the structure was created
							if( pioPChannelToBuffer )
							{
								// Calculate how much of the structure to read
								cbToRead = min( ckList.cksize, sizeof( DMUS_IO_PCHANNELTOBUFFER_HEADER ) );
								pStream->Read( &pioPChannelToBuffer->ioPChannelToBufferHeader, cbToRead, &cbRead );

								// Check if we read in all of the PChannel->Buffer structure
								if( cbToRead == sizeof( DMUS_IO_PCHANNELTOBUFFER_HEADER ) )
								{
									// Read in the GUIDs stored after the structure
									long lSize = ckList.cksize - cbToRead;

									// Continue while we can read at least one more GUID
									while( lSize >= sizeof(GUID) )
									{
										// Create a new GUID
										GUID *pGuid = new GUID;
										if( pGuid )
										{
											// Verify that the memory allocation succeeded
											pStream->Read( pGuid, sizeof(GUID), &cbRead );

											// Change the GUID if we're using an outdated one
											if( REMOVED_GUID_Buffer_3D == *pGuid )
											{
												*pGuid = GUID_Buffer_3D_Dry;
												SetModified( TRUE );
											}

											pioPChannelToBuffer->lstGuids.AddTail( pGuid );
										}

										// Subtract off the amount of data we read
										lSize -= sizeof(GUID);
									}
								}

								// Add the PChannel->Buffer configuration data to the port configuration
								pioPortConfig->lstPChannelToBuffer.AddTail( pioPChannelToBuffer );
							}
							break;

						case DMUSPROD_FOURCC_BUFFER_DESIGN_GUID_CHUNK:
							if( pioPChannelToBuffer )
							{
								// Read in the GUIDs
								long lSize = ckList.cksize;
								while( lSize >= sizeof( GUID ) )
								{
									GUID *pGUID = new GUID;
									pStream->Read( pGUID, sizeof(GUID), NULL );
									lSize -= sizeof( GUID );
									pioPChannelToBuffer->lstDesignGuids.AddTail( pGUID );
								}
							}
							break;

						case FOURCC_LIST:
							switch( ckList.fccType )
							{
							case DMUS_FOURCC_UNFO_LIST:
								MMCKINFO ckSubList;
								while( 0 == pIRiffStream->Descend( &ckSubList, &ckList, 0 ) )
								{
									switch( ckSubList.ckid )
									{
									case DMUS_FOURCC_UNAM_CHUNK:
										// If the PChannel->Buffer structure was created (it should have been)
										if( pioPChannelToBuffer )
										{
											// Read in the name of the Mix Group that created this
											// range of PChannels
											ReadMBSfromWCS( pStream, ckList.cksize, &pioPChannelToBuffer->strMixGroupName );
										}
										break;
									}
									pIRiffStream->Ascend( &ckList, 0 );
								}
								break;
							}
							break;
						}

						pIRiffStream->Ascend( &ckList, 0 );
					}
				}
				break;

			case DMUSPROD_FOURCC_PORT_DESIGN_LIST:
				{
					// Initialize the pointer to the strucutre that stores information for a range of PChannels
					ioDesignMixGroup *pioDesignMixGroup = NULL;

					while( 0 == pIRiffStream->Descend( &ckList, &ckSubChunk, 0 ) )
					{
						switch( ckList.ckid )
						{
						case DMUSPROD_FOURCC_PORT_DESIGN_MIXGROUP_CHUNK:
							// Create a new structure to store the PChannel->Buffer information in
							pioDesignMixGroup = new ioDesignMixGroup;

							// Verify that the structure was created
							if( pioDesignMixGroup )
							{
								// Calculate how much of the structure to read
								cbToRead = min( ckList.cksize, sizeof( DWORD ) * 2 );
								pStream->Read( &pioDesignMixGroup->dwPChannelStart, min( sizeof(DWORD), cbToRead ), &cbRead );
								pStream->Read( &pioDesignMixGroup->dwPChannelSpan, min( sizeof(DWORD), cbToRead - sizeof(DWORD) ), &cbRead );

								// Add the PChannel->MixGroup configuration data to the port configuration
								pioPortConfig->lstDesignMixGroup.AddTail( pioDesignMixGroup );
							}
							break;

						case FOURCC_LIST:
							switch( ckList.fccType )
							{
							case DMUS_FOURCC_UNFO_LIST:
								MMCKINFO ckSubList;
								while( 0 == pIRiffStream->Descend( &ckSubList, &ckList, 0 ) )
								{
									switch( ckSubList.ckid )
									{
									case DMUS_FOURCC_UNAM_CHUNK:
										// If the PChannel->MixGroup structure was created (it should have been)
										if( pioDesignMixGroup )
										{
											// Read in the name of the Mix Group that created this
											// range of PChannels
											ReadMBSfromWCS( pStream, ckList.cksize, &pioDesignMixGroup->strMixGroupName );
										}
										break;
									}
									pIRiffStream->Ascend( &ckList, 0 );
								}
								break;
							}
							break;
						}

						pIRiffStream->Ascend( &ckList, 0 );
					}
				}
				break;
			}
		}

		pIRiffStream->Ascend( &ckSubChunk, 0 );
	}

	// Release the IStream
	pStream->Release();

	// Add the port configuration to the list that was passed in
	lstPortConfigs.AddTail( pioPortConfig );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::SetSplitterPos

void CDirectMusicAudioPath::SetSplitterPos( WORD pos, bool bMarkAsDirty )
{
	m_wSplitterPos = pos;
	if(bMarkAsDirty)
	{
		m_fModified = TRUE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::GetNewMixgroupName

CString CDirectMusicAudioPath::GetNewMixgroupName( void ) const
{
	// Load the default mixgroup name
	CString strOrigName;
	strOrigName.LoadString( IDS_DEFAULT_MIXGROUP_NAME );

	// Get the length of the default mixgroup name
	int nOrigNameLength = strOrigName.GetLength();

	// Start with mix group 1
	int i = 0;

	// Temporary variable for the mixgroup number
	CString strNbr;
	
	// Temporary variable for the mixgroup name
	CString strNewName	= strOrigName;

	// Make sure name is unique
	while( !IsMixgroupNameUnique( NULL, strNewName ) )
	{
		strNbr.Format( "%d", ++i ); 
		int nNbrLength = strNbr.GetLength();
		
		if( (nOrigNameLength + nNbrLength) <= DMUS_MAX_NAME )
		{
			strNewName = strOrigName + strNbr;
		}
		else
		{
			strNewName = strOrigName.Left(DMUS_MAX_NAME - nNbrLength) + strNbr;
		}
	}

	return strNewName;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::IsMixgroupNameUnique

bool CDirectMusicAudioPath::IsMixgroupNameUnique( const ItemInfo *pItemInfo, const CString cstrName ) const
{
	POSITION pos = m_lstItems.GetHeadPosition();
	while( pos )
	{
		ItemInfo *pTmpItemInfo = m_lstItems.GetNext( pos );
		if( pTmpItemInfo != pItemInfo
		&&	pTmpItemInfo->strBandName == cstrName )
		{
			return false;
		}
	}

	// Ensure the reserved "Standard Env. Reverb" name isn't used
	CString strEnvReverb;
	if( strEnvReverb.LoadString( IDS_BUFFER_ENVREVERB )
	&&	cstrName == strEnvReverb )
	{
		return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::IsEffectNameUnique

bool CDirectMusicAudioPath::IsEffectNameUnique( const EffectInfo *pEffectInfo, const CString cstrName ) const
{
	POSITION posItem = m_lstItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		ItemInfo *pItemInfo = m_lstItems.GetNext( posItem );

		// If the port doesn't support buffers, skip it
		if( !ItemSupportsBuffers( pItemInfo ) )
		{
			continue;
		}

		// Iterate through the list of buffers
		POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			// Get a pointer to each buffer
			BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

			// If the buffer does not support effects, skip it
			if( !ItemAndBufferSupportEffects( pItemInfo, pBufferOptions ) )
			{
				continue;
			}

			// Iterate through the list of effects
			POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
			while( posEffect )
			{
				// Get a pointer to each effect
				BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );

				// Check if this is the effect we're looking for
				if( (&(pBusEffectInfo->m_EffectInfo) != pEffectInfo)
				&&	(pBusEffectInfo->m_EffectInfo.m_strInstanceName == cstrName) )
				{
					// Found a matching effect
					return false;
				}
			}
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::SaveUndoState

void CDirectMusicAudioPath::SaveUndoState( bool &fIgnoreChange, const int nStringID, const bool fSetModified )
{
	if( !fIgnoreChange )
	{
		fIgnoreChange = true;
		SaveUndoState( nStringID, fSetModified );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::SaveUndoState

void CDirectMusicAudioPath::SaveUndoState( const int nStringID, const bool fSetModified )
{
	m_pUndoMgr->SaveState( this, theApp.m_hInstance, nStringID );
	if( fSetModified )
	{
		SetModified( fSetModified );

		// Notify connected nodes that AudioPath name has changed
		theApp.m_pAudioPathComponent->m_pIFramework->NotifyNodes( this, AUDIOPATH_DirtyNotification, NULL );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::EnumeratePorts

void CDirectMusicAudioPath::EnumeratePorts( void )
{
	// Iterate through all ports
	DMUS_PORTCAPS dmpc;
	for(DWORD dwIndex = 0; ; dwIndex++)
	{
		// Initialize dmpc
		ZeroMemory(&dmpc, sizeof(dmpc));
		dmpc.dwSize = sizeof(DMUS_PORTCAPS);

		// Get the port's capabilities
		const HRESULT hr = theApp.m_pAudioPathComponent->m_pIDMusic->EnumPort(dwIndex, &dmpc);

		// If we succeeded and didn't hit the end of the list
		if(SUCCEEDED(hr) && (hr != S_FALSE) )
		{
			// Only add the output ports
			if( dmpc.dwClass == DMUS_PC_OUTPUTCLASS )
			{
				PortOptions *pPortOptions = new PortOptions;
				pPortOptions->m_dwEffects = 0;
				pPortOptions->m_dwSupportedEffects = dmpc.dwEffectFlags;
				pPortOptions->m_dwSampleRate = 22050;
				if( dmpc.dwMaxVoices == 0xFFFFFFFF )
				{
					pPortOptions->m_dwVoices = 0xFFFFFFFF;
					pPortOptions->m_dwMaxVoices = 0xFFFFFFFF;
				}
				else
				{
					pPortOptions->m_dwVoices = max( DEFAULT_NUM_VOICES, dmpc.dwMaxVoices );
					pPortOptions->m_dwMaxVoices = dmpc.dwMaxVoices;
				}
				pPortOptions->m_guidPort = dmpc.guidPort;
				pPortOptions->m_strName = dmpc.wszDescription;
				pPortOptions->m_fAudioPath = dmpc.dwFlags & DMUS_PC_AUDIOPATH ? true : false;
				m_lstPortOptions.AddTail( pPortOptions );
			}
		}
		else
		{
			break;
		}
	}

	// Add the default port
	PortOptions *pPortOptions = new PortOptions;
	pPortOptions->m_dwEffects = 0;
	pPortOptions->m_dwSupportedEffects = 0;
	pPortOptions->m_dwSampleRate = 22050;
	pPortOptions->m_dwVoices = 0xFFFFFFFF;
	pPortOptions->m_dwMaxVoices = 0xFFFFFFFF;
	pPortOptions->m_guidPort = GUID_Synth_Default;
	pPortOptions->m_strName.LoadString( IDS_DEFAULT_PORT );
	pPortOptions->m_fAudioPath = true;
	m_lstPortOptions.AddTail( pPortOptions );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::GetUniqueEffectInstanceNameIfNecessary

void CDirectMusicAudioPath::GetUniqueEffectInstanceNameIfNecessary( EffectInfo *pEffectInfo )
{
	// Iterate through the items in the tree
	POSITION posItem = m_lstItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		ItemInfo *pItemInfo = m_lstItems.GetNext( posItem );

		// If the port doesn't support buffers, skip it
		if( !ItemSupportsBuffers( pItemInfo ) )
		{
			continue;
		}

		// Iterate through the list of buffers
		POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			// Get a pointer to each buffer
			BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

			// If the buffer does not support effects, skip it
			if( !ItemAndBufferSupportEffects( pItemInfo, pBufferOptions ) )
			{
				continue;
			}

			// Iterate through the list of effects
			POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
			while( posEffect )
			{
				// Get a pointer to each effect
				BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );

				// Check if this is the effect we're looking for
				if( pBusEffectInfo->m_EffectInfo.IsEqualExceptName( pEffectInfo )
				&&	(pEffectInfo != &pBusEffectInfo->m_EffectInfo)
				&&	(pBusEffectInfo->m_EffectInfo.m_strInstanceName == pEffectInfo->m_strInstanceName)
				&&	(pBusEffectInfo->m_EffectInfo.m_strName == pEffectInfo->m_strName) )
				{
					// Matching effect already exists, so don't change the effect's name
					return;
				}
			}
		}
	}

	// Ensure the effect's name is unique
	GetUniqueEffectInstanceName( pEffectInfo );

	return;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::PastePortOptionsHelper

PortOptions *CDirectMusicAudioPath::PastePortOptionsHelper( PortOptions *pPortOptions )
{
	// Iterate through the list of ports
	POSITION pos = m_lstPortOptions.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each port
		PortOptions *pTmpPortOptions = m_lstPortOptions.GetNext( pos );

		// Check if the ports have the same GUID
		if( pTmpPortOptions->m_guidPort == pPortOptions->m_guidPort )
		{
			// Delete pPortOptions, since it's not necessary
			delete pPortOptions;

			// Port already exists, so just use pTmpPortOptions
			// (don't copy the data from pPortOptions)
			return pTmpPortOptions;
		}
	}

	// Iinsert the new port in our list and return it
	m_lstPortOptions.AddTail( pPortOptions );

	return pPortOptions;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::FindPort

PortOptions *CDirectMusicAudioPath::FindPort( REFGUID rGUIDPort )
{
	// Iterate through the list of ports
	POSITION pos = m_lstPortOptions.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each port
		PortOptions *pTmpPortOptions = m_lstPortOptions.GetNext( pos );

		// Check if we found the GUID
		if( pTmpPortOptions->m_guidPort == rGUIDPort )
		{
			// Port exists, so return pTmpPortOptions
			return pTmpPortOptions;
		}
	}

	// Not found
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::LoadEffect

EffectInfo *CDirectMusicAudioPath::LoadEffect( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO *pckParent )
{
	// Create a new EffectInfo structure
	EffectInfo *pNewEffectInfo = new EffectInfo();

	// Try and read in the effect
	if( SUCCEEDED(pNewEffectInfo->Read( pIRiffStream, pckParent )) )
	{
		// If the read succeeded, return the effect we read
		// The caller is responsible for making sure the effect's name is unique
		return pNewEffectInfo;
	}
	else
	{
		// Otherwise, delete the effect
		delete pNewEffectInfo;
		return NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::LoadEffect

EffectInfo *CDirectMusicAudioPath::LoadEffect( IStream *pIStream )
{
	// Create a new EffectInfo structure
	EffectInfo *pNewEffectInfo = new EffectInfo();

	// Try and read in the effect
	if( SUCCEEDED(pNewEffectInfo->Read( pIStream )) )
	{
		// If the read succeeded, return the effect we read
		// The caller is responsible for making sure the effect's name is unique
		return pNewEffectInfo;
	}
	else
	{
		// Otherwise, delete the effect
		delete pNewEffectInfo;
		return NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::GetUniqueMixGroupName

void CDirectMusicAudioPath::GetUniqueMixGroupName( ItemInfo *pItemInfo )
{
	// Skip Env Reverb items
	if( IsItemEnvReverb( pItemInfo ) )
	{
		return;
	}

	// Ensure the name isn't already too long
	if( pItemInfo->strBandName.GetLength() > DMUS_MAX_NAME )
	{
		pItemInfo->strBandName = pItemInfo->strBandName.Left( DMUS_MAX_NAME );
		pItemInfo->strBandName.TrimRight();
	}

	// Copy the original name
	CString	strOrigName = pItemInfo->strBandName;

	// The string to store the number in
	CString	strNbr;
	while( !strOrigName.IsEmpty()  &&  _istdigit(strOrigName[strOrigName.GetLength() - 1]) )
	{
		strNbr = strOrigName.Right(1) + strNbr;
		strOrigName = strOrigName.Left( strOrigName.GetLength() - 1 );
	}

	// Save the length of the textual part of the original name
	const int nOrigNameLength = strOrigName.GetLength();

	// Set the first number to use
	int	i = _ttoi( strNbr );

	// Continue until we find a unique mix group name
	while( IsMixgroupNameUnique( pItemInfo, pItemInfo->strBandName ) == FALSE )
	{
		// Convert from a number to a string
		strNbr.Format( "%d", ++i ); 

		// Get the length of the number text
		int	nNbrLength = strNbr.GetLength();

		// Ensure the name doesn't go longer than DMUS_MAX_NAME
		if( (nOrigNameLength + nNbrLength) <= DMUS_MAX_NAME )
		{
			pItemInfo->strBandName = strOrigName + strNbr;
		}
		else
		{
			pItemInfo->strBandName = strOrigName.Left(DMUS_MAX_NAME - nNbrLength) + strNbr;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::GetUniqueEffectInstanceName

void CDirectMusicAudioPath::GetUniqueEffectInstanceName( EffectInfo *pEffectInfo )
{
	// Make sure the instance name and effect name don't match
	if( pEffectInfo->m_strInstanceName == pEffectInfo->m_strName )
	{
		CString strNum;
		strNum.Format("%d",1);
		pEffectInfo->m_strInstanceName += strNum;
	}

	// Ensure the name isn't already too long
	if( pEffectInfo->m_strInstanceName.GetLength() > DMUS_MAX_NAME )
	{
		pEffectInfo->m_strInstanceName = pEffectInfo->m_strInstanceName.Left( DMUS_MAX_NAME );
		pEffectInfo->m_strInstanceName.TrimRight();
	}

	// Copy the original name
	CString	strOrigName = pEffectInfo->m_strInstanceName;

	// If the original name is empty, copy the effect name
	if( strOrigName.IsEmpty() )
	{
		pEffectInfo->m_strInstanceName = pEffectInfo->m_strName;
		strOrigName = pEffectInfo->m_strName;
	}

	// The string to store the number in
	CString	strNbr;
	while( !strOrigName.IsEmpty()  &&  _istdigit(strOrigName[strOrigName.GetLength() - 1]) )
	{
		strNbr = strOrigName.Right(1) + strNbr;
		strOrigName = strOrigName.Left( strOrigName.GetLength() - 1 );
	}

	// Save the length of the textual part of the original name
	const int nOrigNameLength = strOrigName.GetLength();

	// Set the first number to use
	int	i = _ttoi( strNbr );

	// Continue until we find a unique effect
	while( (pEffectInfo->m_strInstanceName == pEffectInfo->m_strName)
		||	IsEffectNameUnique( pEffectInfo, pEffectInfo->m_strInstanceName ) == FALSE )
	{
		// Convert from a number to a string
		strNbr.Format( "%d", ++i ); 

		// Get the length of the number text
		int	nNbrLength = strNbr.GetLength();

		// Ensure the name doesn't go longer than DMUS_MAX_NAME
		if( (nOrigNameLength + nNbrLength) <= DMUS_MAX_NAME )
		{
			pEffectInfo->m_strInstanceName = strOrigName + strNbr;
		}
		else
		{
			pEffectInfo->m_strInstanceName = strOrigName.Left(DMUS_MAX_NAME - nNbrLength) + strNbr;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::SaveToConfigObject

HRESULT CDirectMusicAudioPath::SaveToConfigObject ( IUnknown *punkAudioPathConfig )
{
	ASSERT( punkAudioPathConfig );
	if( punkAudioPathConfig == NULL )
	{
		return E_POINTER;
	}

	//CWaitCursor wait;

	// Try and create a memory stream
	IStream* pIMemStream;
	HRESULT hr = theApp.m_pAudioPathComponent->m_pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );
	if( SUCCEEDED ( hr ) )
	{
		// Save ourself to the memory stream
		hr = DM_SaveAudioPath( pIMemStream );
		if( SUCCEEDED ( hr ) )
		{
			// Query the config object for an IPersistStream interface
			IPersistStream* pIPersistStream;
			hr = punkAudioPathConfig->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
			if( SUCCEEDED ( hr ) )
			{
				// Seek back to the start of the stream
				StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );

				// Try and load the config object with the stream
				hr = pIPersistStream->Load( pIMemStream );

				RELEASE( pIPersistStream );
			}
		}

		RELEASE( pIMemStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// RemoveDuplicateBuffers

void RemoveDuplicateBuffers( CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers )
{
	POSITION pos = lstDSoundBuffers.GetHeadPosition();
	while( pos )
	{
		// Save the current position
		const POSITION posCurrent = pos;

		// Get a pointer to the current buffer
		ioDSoundBuffer *pBuffer = lstDSoundBuffers.GetNext( pos );

		// Iterate through all the buffers before ours
		POSITION posTmp = lstDSoundBuffers.GetHeadPosition();
		while( posCurrent != posTmp )
		{
			// Save the position 
			// Get a pointer to the buffer
			ioDSoundBuffer *pTmpBuffer = lstDSoundBuffers.GetNext( posTmp );

			// If the buffer GUIDs are identical
			if( pTmpBuffer->ioBufferHeader.guidBufferID == pBuffer->ioBufferHeader.guidBufferID )
			{
				// Delete and remove pBuffer
				delete pBuffer;
				lstDSoundBuffers.RemoveAt( posCurrent );

				// Break out of the while loop
				break;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::DM_SaveAudioPath

HRESULT CDirectMusicAudioPath::DM_SaveAudioPath( IStream *pIStream )
{
	ASSERT( pIStream );
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	// Allocate a RIFF stream
	IDMUSProdRIFFStream *pRiffStream;
	HRESULT hr = AllocRIFFStream( pIStream, &pRiffStream );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// Find out what type of save to do (runtime vs. design-time)
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

	// Create the main audiopath chunk
	MMCKINFO ckMain;
	ckMain.fccType = DMUS_FOURCC_AUDIOPATH_FORM;
	if( 0 == pRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) )
	{
		// Save the AudioPath's GUID
		SaveGUID( pRiffStream );

		// Check if we're writing a design file
		if( StreamInfo.ftFileType == FT_DESIGN )
		{
			// Create the design-time list chunk
			MMCKINFO ckDesign;
			ckDesign.fccType = DMUSPROD_FOURCC_AUDIOPATH_DESIGN_LIST;
			if( 0 == pRiffStream->CreateChunk( &ckDesign, MMIO_CREATELIST ) )
			{
				// Save out the UI information
				DM_SaveUIDesign( pRiffStream );

				// Ascend out of the design-time list chunk
				pRiffStream->Ascend( &ckDesign, 0 );
			}
		}

		// Save the AudioPath's version
		SaveVersion( pRiffStream );

		// Save the AudioPath's UNFO list
		SaveInfoList( pRiffStream );

		// Save optional toolgraph, but only if we're not saving to an AudioPathConfig object
		if( m_pIToolGraphNode
		&& (StreamInfo.guidDataFormat != GUID_DirectMusicObject) )
		{
			IPersistStream* pIPersistStream;
			hr = m_pIToolGraphNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
			if( FAILED( hr ) )
			{
				return hr;
			}

			hr = pIPersistStream->Save( pIStream, FALSE );
			pIPersistStream->Release();
			if( FAILED( hr ) )
			{
				return hr;
			}
		}

		CTypedPtrList< CPtrList, ioPortConfig*> lstPortConfigs;
		CTypedPtrList< CPtrList, ioDSoundBuffer*> lstDSoundBuffers;

		// Iterate through all mix groups
		DWORD dwDisplayIndex = 0;
		while( dwDisplayIndex < unsigned(m_lstItems.GetCount()) )
		{
			POSITION pos = m_lstItems.GetHeadPosition();
			while( pos )
			{
				const ItemInfo *pItemInfo = m_lstItems.GetNext( pos );
				if( dwDisplayIndex == pItemInfo->dwDisplayIndex )
				{
					// Convert mix group (this adds the buffers at the end of lstDSoundBuffers)
					pItemInfo->ConvertToDMusic( lstPortConfigs, lstDSoundBuffers );
					break;
				}
			}
			dwDisplayIndex++;
		}

		// Remove duplicate buffers
		RemoveDuplicateBuffers( lstDSoundBuffers );

		// TODO: Look through lstPortConfigs for adjacent ports that can be
		// merged together (ConvertToDMusic can't catch them all).

		// Check if there are any ports to create
		if( !lstPortConfigs.IsEmpty() )
		{
			// Create the port configs list chunk
			MMCKINFO ckPortConfigsList;
			ckPortConfigsList.fccType = DMUS_FOURCC_PORTCONFIGS_LIST;
			if( 0 == pRiffStream->CreateChunk( &ckPortConfigsList, MMIO_CREATELIST ) )
			{
				// Now, save all port config chunks
				while( !lstPortConfigs.IsEmpty() )
				{
					ioPortConfig *pioPortConfig = lstPortConfigs.RemoveHead();

					// Create the port config list chunk
					MMCKINFO ckPortConfigList;
					ckPortConfigList.fccType = DMUS_FOURCC_PORTCONFIG_LIST;
					if( 0 == pRiffStream->CreateChunk( &ckPortConfigList, MMIO_CREATELIST ) )
					{
						// Create the port config header chunk
						MMCKINFO ckSubChunk;
						ckSubChunk.ckid = DMUS_FOURCC_PORTCONFIG_ITEM;
						if( 0 == pRiffStream->CreateChunk( &ckSubChunk, 0 ) )
						{
							DWORD cbWritten;
							pIStream->Write( &(pioPortConfig->ioPortConfigHeader), sizeof(DMUS_IO_PORTCONFIG_HEADER), &cbWritten );

							// Ascend out of the port config header chunk
							pRiffStream->Ascend( &ckSubChunk, 0 );
						}

						// Create the port params chunk
						ckSubChunk.ckid = DMUS_FOURCC_PORTPARAMS_ITEM;
						if( 0 == pRiffStream->CreateChunk( &ckSubChunk, 0 ) )
						{
							DWORD cbWritten;
							pIStream->Write( &(pioPortConfig->ioPortParams), sizeof(DMUS_PORTPARAMS8), &cbWritten );

							// Ascend out of the port params chunk
							pRiffStream->Ascend( &ckSubChunk, 0 );
						}

						// Check if the port name is not empty and we're writing a design file
						if( !pioPortConfig->strPortName.IsEmpty()
						&&	StreamInfo.ftFileType == FT_DESIGN )
						{
							// The port name is not empty, so create an UNFO list
							ckSubChunk.fccType = DMUS_FOURCC_UNFO_LIST;
							if( 0 == pRiffStream->CreateChunk( &ckSubChunk, MMIO_CREATELIST ) )
							{
								// Store port name in a PORT_NAME chunk
								MMCKINFO ckName;
								ckName.ckid = FOURCC_PORT_NAME_CHUNK;
								if( 0 == pRiffStream->CreateChunk( &ckName, 0 ) )
								{
									SaveMBStoWCS( pIStream, &(pioPortConfig->strPortName) );

									// Ascend out of the PORT_NAME chunk
									pRiffStream->Ascend( &ckName, 0 );
								}

								// Ascend out of the UNFO list
								pRiffStream->Ascend( &ckSubChunk, 0 );
							}
						}

						if( (pioPortConfig->ioPortParams.dwFeatures & DMUS_PORT_FEATURE_AUDIOPATH)
						&&	!pioPortConfig->lstPChannelToBuffer.IsEmpty() )
						{
							// At least one PChannel->Buffer assignment,
							// create the list of PChannel to buffer assignments,
							ckSubChunk.fccType = DMUS_FOURCC_PCHANNELS_LIST;
							if( 0 == pRiffStream->CreateChunk( &ckSubChunk, MMIO_CREATELIST ) )
							{
								// Save out each PChannel to buffer assignment in its own chunk
								while( !pioPortConfig->lstPChannelToBuffer.IsEmpty() )
								{
									// Get a pointer to the PChannel->Buffer assigment information
									ioPChannelToBuffer *pioPChannelToBuffer = pioPortConfig->lstPChannelToBuffer.RemoveHead();

									// Create a PChannel->Buffer assignment chunk
									MMCKINFO ckPChannel;
									ckPChannel.ckid = DMUS_FOURCC_PCHANNELS_ITEM;
									if( 0 == pRiffStream->CreateChunk( &ckPChannel, 0 ) )
									{
										// Write the header information
										DWORD cbWritten;
										pIStream->Write( &pioPChannelToBuffer->ioPChannelToBufferHeader, sizeof(DMUS_IO_PCHANNELTOBUFFER_HEADER), &cbWritten );

										// Write out the list of GUIDs
										while( !pioPChannelToBuffer->lstGuids.IsEmpty() )
										{
											// Get a pointer to each GUID
											GUID *pGuid = pioPChannelToBuffer->lstGuids.RemoveHead();

											// Write out the GUID
											pIStream->Write( pGuid, sizeof(GUID), &cbWritten );

											// Delete the GUID
											delete pGuid;
										}

										// Ascend out of the PChannel->Buffer assignment chunk
										pRiffStream->Ascend( &ckPChannel, 0 );
									}

									// Check if we're writing a design-time file
									if( StreamInfo.ftFileType == FT_DESIGN )
									{
										// Check if there is a Mix Group name
										if( !pioPChannelToBuffer->strMixGroupName.IsEmpty() )
										{
											// There is a Mix Group name - create an UNFO list
											MMCKINFO ckUnfo;
											ckUnfo.fccType = DMUS_FOURCC_UNFO_LIST;
											if( 0 == pRiffStream->CreateChunk( &ckUnfo, MMIO_CREATELIST ) )
											{
												// Create an UNAM chunk to store the name in
												MMCKINFO ckName;
												ckName.ckid = DMUS_FOURCC_UNAM_CHUNK;
												if( 0 == pRiffStream->CreateChunk( &ckName, 0 ) )
												{
													// Write out the Mix Group name
													SaveMBStoWCS( pIStream, &(pioPChannelToBuffer->strMixGroupName) );

													// Ascend out of the UNAM chunk
													pRiffStream->Ascend( &ckName, 0 );
												}

												// Ascend out of the UNFO list
												pRiffStream->Ascend( &ckUnfo, 0 );
											}
										}

										// Write out the design GUID array, if necessary
										if( !pioPChannelToBuffer->lstDesignGuids.IsEmpty() )
										{
											// Create an chunk to store the list of GUIDS in
											MMCKINFO ckDesignGUID;
											ckDesignGUID.ckid = DMUSPROD_FOURCC_BUFFER_DESIGN_GUID_CHUNK;
											if( 0 == pRiffStream->CreateChunk( &ckDesignGUID, 0 ) )
											{
												// Write out the GUIDs
												POSITION pos = pioPChannelToBuffer->lstDesignGuids.GetHeadPosition();
												while( pos )
												{
													pIStream->Write( pioPChannelToBuffer->lstDesignGuids.GetNext( pos ), sizeof(GUID), NULL );
												}

												// Ascend out of the UNAM chunk
												pRiffStream->Ascend( &ckDesignGUID, 0 );
											}
										}
									}

									// Delete the PChannel->Buffer assignment information
									delete pioPChannelToBuffer;
								}

								// Ascend out of the PChannel->Buffer assignment list
								pRiffStream->Ascend( &ckSubChunk, 0 );
							}
						}

						if( !pioPortConfig->lstDesignMixGroup.IsEmpty() )
						{
							// At least one PChannel->MixGroup assignment,
							// create the list of PChannel to buffer assignments,
							ckSubChunk.fccType = DMUSPROD_FOURCC_PORT_DESIGN_LIST;
							if( 0 == pRiffStream->CreateChunk( &ckSubChunk, MMIO_CREATELIST ) )
							{
								// Save out each PChannel to MixGroup assignment in its own chunk
								while( !pioPortConfig->lstDesignMixGroup.IsEmpty() )
								{
									// Get a pointer to the PChannel->MixGroup assigment information
									ioDesignMixGroup *pioDesignMixGroup = pioPortConfig->lstDesignMixGroup.RemoveHead();

									// Create a PChannel->MixGroup assignment chunk
									MMCKINFO ckPChannel;
									ckPChannel.ckid = DMUSPROD_FOURCC_PORT_DESIGN_MIXGROUP_CHUNK;
									if( 0 == pRiffStream->CreateChunk( &ckPChannel, 0 ) )
									{
										// Write the header information
										DWORD cbWritten;
										pIStream->Write( &pioDesignMixGroup->dwPChannelStart, sizeof(DWORD), &cbWritten );
										pIStream->Write( &pioDesignMixGroup->dwPChannelSpan, sizeof(DWORD), &cbWritten );

										// Ascend out of the PChannel->Buffer assignment chunk
										pRiffStream->Ascend( &ckPChannel, 0 );
									}

									// Check if there is a Mix Group name and we're writing a design-time file
									if( !pioDesignMixGroup->strMixGroupName.IsEmpty()
									&&	StreamInfo.ftFileType == FT_DESIGN )
									{
										// There is a Mix Group name - create an UNFO list
										MMCKINFO ckUnfo;
										ckUnfo.fccType = DMUS_FOURCC_UNFO_LIST;
										if( 0 == pRiffStream->CreateChunk( &ckUnfo, MMIO_CREATELIST ) )
										{
											// Create an UNAM chunk to store the name in
											MMCKINFO ckName;
											ckName.ckid = DMUS_FOURCC_UNAM_CHUNK;
											if( 0 == pRiffStream->CreateChunk( &ckName, 0 ) )
											{
												// Write out the Mix Group name
												SaveMBStoWCS( pIStream, &(pioDesignMixGroup->strMixGroupName) );

												// Ascend out of the UNAM chunk
												pRiffStream->Ascend( &ckName, 0 );
											}

											// Ascend out of the UNFO list
											pRiffStream->Ascend( &ckUnfo, 0 );
										}
									}

									// Delete the PChannel->MixGroup assignment information
									delete pioDesignMixGroup;
								}

								// Ascend out of the PChannel->MixGroup assignment list
								pRiffStream->Ascend( &ckSubChunk, 0 );
							}
						}

						// Ascend out of the port config list chunk
						pRiffStream->Ascend( &ckPortConfigList, 0 );
					}

					// Delete the port config information
					delete pioPortConfig;
				}

				// Ascend out of the port configs list chunk
				pRiffStream->Ascend( &ckPortConfigsList, 0 );
			}
		}

		// Write out the buffer list
		while( !lstDSoundBuffers.IsEmpty() )
		{
			ioDSoundBuffer *pioDSoundBuffer = lstDSoundBuffers.RemoveHead();

			MMCKINFO ckBufferList;
			ckBufferList.fccType = DMUS_FOURCC_DSBUFFER_LIST;
			if( 0 == pRiffStream->CreateChunk( &ckBufferList, MMIO_CREATELIST ) )
			{
				MMCKINFO ckBufferAttrib;
				ckBufferAttrib.ckid = DMUS_FOURCC_DSBUFFATTR_ITEM;
				if( 0 == pRiffStream->CreateChunk( &ckBufferAttrib, 0 ) )
				{
					// Write out the buffer header
					DWORD cbWritten;
					pIStream->Write( &pioDSoundBuffer->ioBufferHeader, sizeof(DMUS_IO_BUFFER_ATTRIBUTES_HEADER), &cbWritten );

					pRiffStream->Ascend( &ckBufferAttrib, 0 );
				}

				// If not using a standard buffer
				if( !(pioDSoundBuffer->ioBufferHeader.dwFlags & DMUS_BUFFERF_DEFINED) )
				{
					// Save out the buffer information
					DM_SaveDSBC_Form( pRiffStream, pioDSoundBuffer, StreamInfo );
				}

				// Check if the name is not empty and we're writing a design file
				if( !pioDSoundBuffer->strName.IsEmpty()
				&&	StreamInfo.ftFileType == FT_DESIGN )
				{
					MMCKINFO ckBufferUnfo;
					ckBufferUnfo.fccType = DMUS_FOURCC_UNFO_LIST;
					if( 0 == pRiffStream->CreateChunk( &ckBufferUnfo, MMIO_CREATELIST ) )
					{
						MMCKINFO ckBufferName;
						ckBufferName.ckid = DMUS_FOURCC_UNAM_CHUNK;
						if( 0 == pRiffStream->CreateChunk( &ckBufferName, 0 ) )
						{
							SaveMBStoWCS( pIStream, &(pioDSoundBuffer->strName) );

							pRiffStream->Ascend( &ckBufferName, 0 );
						}

						pRiffStream->Ascend( &ckBufferUnfo, 0 );
					}
				}

				pRiffStream->Ascend( &ckBufferList, 0 );
			}

			delete pioDSoundBuffer;
		}

		// Ascend out of the main chunk
		pRiffStream->Ascend( &ckMain, 0 );

		// We succeeded
		hr = S_OK;
	}
	else
	{
		// Unable to create chunk - return failure code
		hr = E_FAIL;
	}

	// Release the RIFF stream
	pRiffStream->Release();

	// Return the result
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::DM_SaveUIDesign

HRESULT CDirectMusicAudioPath::DM_SaveUIDesign( IDMUSProdRIFFStream *pIRiffStream )
{
	IStream *pIStream = pIRiffStream->GetStream();
	if( pIStream == NULL )
	{
		return E_FAIL;
	}

	// Create a chunk to store the UI information in 
	MMCKINFO ckUI;
	ckUI.ckid = DMUSPROD_FOURCC_AUDIOPATH_UI_CHUNK;
	if( 0 == pIRiffStream->CreateChunk( &ckUI, 0 ) )
	{
		// Write the UI data
		ioAudioPathUI oAudioPathUI;
		ZeroMemory( &oAudioPathUI, sizeof(ioAudioPathUI) );

		oAudioPathUI.wSplitterPos = m_wSplitterPos;
		oAudioPathUI.wMixGroupWidth = m_wMixGroupWidth;
		oAudioPathUI.wBusWidth = m_wBusWidth;
		oAudioPathUI.wBufferWidth = m_wBufferWidth;
		oAudioPathUI.wEffectWidth = m_wEffectWidth;

		pIStream->Write( &oAudioPathUI, sizeof( ioAudioPathUI ), NULL );

		// Ascend out of the main effect palette LIST chunk
		pIRiffStream->Ascend( &ckUI, 0 );
	}

	// Release the IStream interface
	pIStream->Release();

	// We succeeded
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::CreatePortOptions

PortOptions *CDirectMusicAudioPath::CreatePortOptions( const DMUS_IO_PORTCONFIG_HEADER &ioPortConfigHeader, const DMUS_PORTPARAMS8 &ioPortParams, const CString &strPortName )
{
	// Iterate through the list of ports
	POSITION pos = m_lstPortOptions.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each port
		PortOptions *pTmpPortOptions = m_lstPortOptions.GetNext( pos );

		// Check if the ports have the same GUID
		if( pTmpPortOptions->m_guidPort == ioPortConfigHeader.guidPort )
		{
			// Port already exists, so copy information from ioPortConfigHeader
			// to pTmpPortOptions, and return pTmpPortOptions
			pTmpPortOptions->m_dwEffects = ioPortParams.dwEffectFlags;
			//pTmpPortOptions->m_dwSupportedEffects
			pTmpPortOptions->m_dwSampleRate = ioPortParams.dwSampleRate;
			pTmpPortOptions->m_dwVoices = ioPortParams.dwVoices;
			//pTmpPortOptions->m_dwMaxVoices
			//pTmpPortOptions->m_fSelected
			pTmpPortOptions->m_fAudioPath = (ioPortParams.dwFeatures & DMUS_PORT_FEATURE_AUDIOPATH) ? true : false;

			// GUID already set - no need to update
			//pTmpPortOptions->m_guidPort = ioPortConfigHeader.guidPort;
			// Name already set - no need to update
			//pTmpPortOptions->m_strName = strPortName;

			return pTmpPortOptions;
		}
	}

	// Port not found - must create a new one
	PortOptions *pPortOptions = new PortOptions;
	pPortOptions->m_dwEffects = ioPortParams.dwEffectFlags;
	// Unknown port - assume it supports all effects
	pPortOptions->m_dwSupportedEffects = 0xFFFFFFFF;
	pPortOptions->m_dwSampleRate = ioPortParams.dwSampleRate;
	pPortOptions->m_dwVoices = ioPortParams.dwVoices;
	// Unknown port - assume it supports at least DEFAULT_NUM_VOICES voices
	pPortOptions->m_dwMaxVoices = max( DEFAULT_NUM_VOICES, ioPortParams.dwVoices );
	//pPortOptions->m_fSelected
	pPortOptions->m_fAudioPath = (ioPortParams.dwFeatures & DMUS_PORT_FEATURE_AUDIOPATH) ? true : false;
	pPortOptions->m_guidPort = ioPortConfigHeader.guidPort;
	pPortOptions->m_strName = strPortName;

	// Iinsert the new port in our list and return it
	m_lstPortOptions.AddTail( pPortOptions );

	return pPortOptions;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath SaveInfoList
    
HRESULT CDirectMusicAudioPath::SaveInfoList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr = S_FALSE;
    MMCKINFO ckMain;
    MMCKINFO ck;

	if( m_strName.IsEmpty()
	&&  m_strAuthor.IsEmpty()
	&&  m_strCopyright.IsEmpty()
	&&  m_strSubject.IsEmpty() )
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

	// Write Script name
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

	// Write script author
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

	// Write Script copyright
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

	// Write script subject
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
// CDirectMusicAudioPath SaveVersion
    
HRESULT CDirectMusicAudioPath::SaveVersion( IDMUSProdRIFFStream* pIRiffStream )
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
// CDirectMusicAudioPath SaveGUID
    
HRESULT CDirectMusicAudioPath::SaveGUID( IDMUSProdRIFFStream* pIRiffStream )
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

	// Write Script GUID
	hr = pIStream->Write( &m_guidAudioPath, sizeof(GUID), &dwBytesWritten);
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
// CDirectMusicAudioPath DM_SaveDSBC_Form

void CDirectMusicAudioPath::DM_SaveDSBC_Form( IDMUSProdRIFFStream *pRiffStream, ioDSoundBuffer *pioDSoundBuffer, DMUSProdStreamInfo &StreamInfo )
{
	// Validate arguments
	ASSERT( pRiffStream );
	if( !pRiffStream )
	{
		return;
	}

	IStream *pIStream = pRiffStream->GetStream();

	MMCKINFO ckBuffer;
	ckBuffer.fccType = DMUS_FOURCC_DSBC_FORM;
	if( 0 == pRiffStream->CreateChunk( &ckBuffer, MMIO_CREATERIFF ) )
	{
		MMCKINFO ckSubChunk;

		// Write out the GUID for this buffer
		ckSubChunk.ckid = DMUS_FOURCC_GUID_CHUNK;
		if( 0 == pRiffStream->CreateChunk( &ckSubChunk, 0 ) )
		{
			DWORD cbWritten;
			pIStream->Write( &(pioDSoundBuffer->ioBufferHeader.guidBufferID), sizeof(GUID), &cbWritten );

			pRiffStream->Ascend( &ckSubChunk, 0 );
		}

		// Check if the name is not empty and we're writing a design file
		if( !pioDSoundBuffer->strName.IsEmpty()
		&&	StreamInfo.ftFileType == FT_DESIGN )
		{
			ckSubChunk.fccType = DMUS_FOURCC_UNFO_LIST;
			if( 0 == pRiffStream->CreateChunk( &ckSubChunk, MMIO_CREATELIST ) )
			{
				MMCKINFO ckName;
				ckName.ckid = DMUS_FOURCC_UNAM_CHUNK;
				if( 0 == pRiffStream->CreateChunk( &ckName, 0 ) )
				{
					SaveMBStoWCS( pIStream, &(pioDSoundBuffer->strName) );

					pRiffStream->Ascend( &ckName, 0 );
				}

				pRiffStream->Ascend( &ckSubChunk, 0 );
			}
		}

		// Write out the DirectSound buffer description
		ckSubChunk.ckid = DMUS_FOURCC_DSBD_CHUNK;
		if( 0 == pRiffStream->CreateChunk( &ckSubChunk, 0 ) )
		{
			DWORD cbWritten;
			pIStream->Write( &pioDSoundBuffer->ioDSBufferDesc, sizeof(DSOUND_IO_DSBUFFERDESC), &cbWritten );

			pRiffStream->Ascend( &ckSubChunk, 0 );
		}

		// Write out the BUS IDs
		if( pioDSoundBuffer->lstBusIDs.GetSize() > 0 )
		{
			ckSubChunk.ckid = DMUS_FOURCC_BSID_CHUNK;
			if( 0 == pRiffStream->CreateChunk( &ckSubChunk, 0 ) )
			{
				DWORD cbWritten;
				pIStream->Write( pioDSoundBuffer->lstBusIDs.GetData(), sizeof(DWORD) * pioDSoundBuffer->lstBusIDs.GetSize(), &cbWritten );

				pRiffStream->Ascend( &ckSubChunk, 0 );
			}
		}

		// Write out the 3D position
		if( pioDSoundBuffer->ioDSBufferDesc.dwFlags & DSBCAPS_CTRL3D )
		{
			ckSubChunk.ckid = DMUS_FOURCC_DS3D_CHUNK;
			if( 0 == pRiffStream->CreateChunk( &ckSubChunk, 0 ) )
			{
				DWORD cbWritten;
				pIStream->Write( &pioDSoundBuffer->ioDS3D, sizeof(DSOUND_IO_3D), &cbWritten );

				pRiffStream->Ascend( &ckSubChunk, 0 );
			}
		}

		// Write out the effect list
		if( !pioDSoundBuffer->lstDSoundEffects.IsEmpty() )
		{
			ckSubChunk.fccType = DMUS_FOURCC_DSBC_LIST;
			if( 0 == pRiffStream->CreateChunk( &ckSubChunk, MMIO_CREATELIST ) )
			{
				// Iterate through the effect list
				while( !pioDSoundBuffer->lstDSoundEffects.IsEmpty() )
				{
					ioDSoundEffect *pioDSoundEffect = pioDSoundBuffer->lstDSoundEffects.RemoveHead();

					// If we're not writing a design-time file,
					// and we're not writing for the DirectMusic object,
					// and this is the wave save DMO
					if( StreamInfo.ftFileType != FT_DESIGN
					&&	StreamInfo.guidDataFormat != GUID_DirectMusicObject
					&&	GUID_DSFX_STANDARD_DUMP == pioDSoundEffect->ioFXHeader.guidDSFXClass )
					{
						// Don't save the DMO
						RELEASE( pioDSoundEffect->pStreamData );
						delete pioDSoundEffect;
						continue;
					}

					MMCKINFO ckFXForm;
					ckFXForm.fccType = DMUS_FOURCC_DSFX_FORM;
					if( 0 == pRiffStream->CreateChunk( &ckFXForm, MMIO_CREATERIFF ) )
					{
						// Write out the effect header
						MMCKINFO ckFXHeader;
						ckFXHeader.ckid = DMUS_FOURCC_DSFX_CHUNK;
						if( 0 == pRiffStream->CreateChunk( &ckFXHeader, 0 ) )
						{
							DWORD cbWritten;
							pIStream->Write( &pioDSoundEffect->ioFXHeader, sizeof( DSOUND_IO_DXDMO_HEADER ), &cbWritten );

							pRiffStream->Ascend( &ckFXHeader, 0 );
						}

						// If we're writing a design-time file
						if( StreamInfo.ftFileType == FT_DESIGN )
						{
							// Write out the effect instance name, if non-empty
							if( !(pioDSoundEffect->strInstanceName.IsEmpty() && pioDSoundEffect->strEffectName.IsEmpty()) )
							{
								MMCKINFO ckUNFOList;
								ckUNFOList.fccType = DMUS_FOURCC_UNFO_LIST;
								if( 0 == pRiffStream->CreateChunk( &ckUNFOList, MMIO_CREATELIST ) )
								{
									MMCKINFO ckName;
									if( !pioDSoundEffect->strInstanceName.IsEmpty() )
									{
										ckName.ckid = DMUS_FOURCC_UNAM_CHUNK;
										if( 0 == pRiffStream->CreateChunk( &ckName, 0 ) )
										{
											SaveMBStoWCS( pIStream, &(pioDSoundEffect->strInstanceName) );

											pRiffStream->Ascend( &ckName, 0 );
										}
									}

									if( !pioDSoundEffect->strEffectName.IsEmpty() )
									{
										ckName.ckid = FOURCC_EFFECT_NAME_CHUNK;
										if( 0 == pRiffStream->CreateChunk( &ckName, 0 ) )
										{
											SaveMBStoWCS( pIStream, &(pioDSoundEffect->strEffectName) );

											pRiffStream->Ascend( &ckName, 0 );
										}
									}

									pRiffStream->Ascend( &ckUNFOList, 0 );
								}
							}

							// Write out the effect's design guid
							MMCKINFO ckEffectGuid;
							ckEffectGuid.ckid = DMUSPROD_FOURCC_EFFECT_GUID_CHUNK;
							if( 0 == pRiffStream->CreateChunk( &ckEffectGuid, 0 ) )
							{
								pIStream->Write( &(pioDSoundEffect->guidDesignGUID), sizeof(GUID), NULL );

								pRiffStream->Ascend( &ckEffectGuid, 0 );
							}
						}

						// Write out the effect data, if necessary
						if( pioDSoundEffect->pStreamData )
						{
							StreamSeek( pioDSoundEffect->pStreamData, 0, SEEK_SET );
							STATSTG statStg;
							if( SUCCEEDED( pioDSoundEffect->pStreamData->Stat( &statStg, STATFLAG_NONAME ) ) )
							{
								MMCKINFO ckFXData;
								ckFXData.ckid = FOURCC_DMUS_EFFECT_DATA_CHUNK;
								if( 0 == pRiffStream->CreateChunk( &ckFXData, 0 ) )
								{
									ULARGE_INTEGER uliRead, uliWritten;
									pioDSoundEffect->pStreamData->CopyTo( pIStream, statStg.cbSize, &uliRead, &uliWritten );

									pRiffStream->Ascend( &ckFXData, 0 );
								}
							}
						}

						pRiffStream->Ascend( &ckFXForm, 0 );
					}

					RELEASE( pioDSoundEffect->pStreamData );
					delete pioDSoundEffect;
				}

				pRiffStream->Ascend( &ckSubChunk, 0 );
			}
		}

		pRiffStream->Ascend( &ckBuffer, 0 );
	}

	pIStream->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::GetDefaultPort

PortOptions *CDirectMusicAudioPath::GetDefaultPort( void )
{
	// Return the user-mode software synth
	return FindPort( GUID_Synth_Default );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::InsertToolGraph

HRESULT CDirectMusicAudioPath::InsertToolGraph( IDirectMusicAudioPath* pIDMAudioPath )
{
	if( m_pIToolGraphNode == NULL 
	||  pIDMAudioPath == NULL )
	{
		return S_OK;
	}

	HRESULT hr = E_FAIL;

	// Now get the IDMUSProdToolGraphInfo interface for m_pIToolGraphNode
	IDMUSProdToolGraphInfo* pIToolGraphInfo;
	if( SUCCEEDED ( m_pIToolGraphNode->QueryInterface( IID_IDMUSProdToolGraphInfo, (void**)&pIToolGraphInfo ) ) )
	{
		if( SUCCEEDED ( pIToolGraphInfo->AddToGraphUserList( pIDMAudioPath ) ) )
		{
			hr = S_OK;
		}

		RELEASE( pIToolGraphInfo );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::AllAudioPaths_InsertToolGraph

HRESULT CDirectMusicAudioPath::AllAudioPaths_InsertToolGraph( void )
{
	HRESULT hr = S_OK;

	POSITION pos = m_lstDMAudioPaths.GetHeadPosition();
	while( pos )
	{
		IDirectMusicAudioPath* pIDMAudioPath = m_lstDMAudioPaths.GetNext( pos );

		if( FAILED ( InsertToolGraph( pIDMAudioPath ) ) )
		{
			hr = E_FAIL;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::RemoveToolGraph

HRESULT CDirectMusicAudioPath::RemoveToolGraph( IDirectMusicAudioPath* pIDMAudioPath )
{
	if( m_pIToolGraphNode == NULL 
	||  pIDMAudioPath == NULL )
	{
		// Should not happen!
		ASSERT( 0 );
		return S_OK;
	}

	HRESULT hr = E_FAIL;

	// Get the IDMUSProdToolGraphInfo interface for m_pIToolGraphNode
	IDMUSProdToolGraphInfo* pIToolGraphInfo;
	if( SUCCEEDED ( m_pIToolGraphNode->QueryInterface( IID_IDMUSProdToolGraphInfo, (void**)&pIToolGraphInfo ) ) )
	{
		// Remove the ToolGraph
		if( SUCCEEDED ( pIToolGraphInfo->RemoveFromGraphUserList( pIDMAudioPath ) ) )
		{
			hr = S_OK;
		}

		RELEASE( pIToolGraphInfo );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::AllAudioPaths_RemoveToolGraph

HRESULT CDirectMusicAudioPath::AllAudioPaths_RemoveToolGraph( void )
{
	HRESULT hr = S_OK;

	POSITION pos = m_lstDMAudioPaths.GetHeadPosition();
	while( pos )
	{
		IDirectMusicAudioPath* pIDMAudioPath = m_lstDMAudioPaths.GetNext( pos );

		if( FAILED ( RemoveToolGraph( pIDMAudioPath ) ) )
		{
			hr = E_FAIL;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::PasteCF_GRAPH

HRESULT CDirectMusicAudioPath::PasteCF_GRAPH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( theApp.m_pAudioPathComponent->m_pIToolGraphComponent
	&&	SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pAudioPathComponent->m_cfGraph, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new Graph
		IDMUSProdRIFFExt* pIRIFFExt;
		IDMUSProdNode* pINode;

		if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIToolGraphComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
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
// CDirectMusicAudioPath::GetGUID

void CDirectMusicAudioPath::GetGUID( GUID* pguidAudioPath )
{
	if( pguidAudioPath )
	{
		*pguidAudioPath = m_guidAudioPath;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::SetGUID

void CDirectMusicAudioPath::SetGUID( GUID guidAudioPath )
{
	m_guidAudioPath = guidAudioPath;
	SetModified( TRUE );

	// Notify connected nodes that AudioPath GUID has changed
	theApp.m_pAudioPathComponent->m_pIFramework->NotifyNodes( this, DOCROOT_GuidChange, NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::GetNameForConductor

BSTR CDirectMusicAudioPath::GetNameForConductor( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );

	// Audiopath file name
	CString strName;
	BSTR bstrAudiopathFileName;

	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->GetNodeFileName( this, &bstrAudiopathFileName ) ) )
	{
		// Node is in the Project tree
		strName = bstrAudiopathFileName;
		::SysFreeString( bstrAudiopathFileName );
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
		// because the Audiopath is still in the process of being loaded
		strName = m_strOrigFileName;
		strName += _T(": ");
	}
	
	// Plus "name"
	strName += m_strName;

    return strName.AllocSysString();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::CopyFrom

void CDirectMusicAudioPath::CopyFrom( CDirectMusicAudioPath *pAudioPath )
{
	// Remove any exising items, effects, and ports
	while( !m_lstItems.IsEmpty() )
	{
		delete m_lstItems.RemoveHead();
	}
	while( !m_lstPortOptions.IsEmpty() )
	{
		delete m_lstPortOptions.RemoveHead();
	}

	// Copy the ports
	POSITION pos = pAudioPath->m_lstPortOptions.GetHeadPosition();
	while( pos )
	{
		PortOptions *pPortOptions = pAudioPath->m_lstPortOptions.GetNext( pos );
		PortOptions *pNewPortOptions = new PortOptions;
		pNewPortOptions->Copy( pPortOptions );
		m_lstPortOptions.AddTail( pNewPortOptions );
	}

	// Copy the items
	pos = pAudioPath->m_lstItems.GetHeadPosition();
	while( pos )
	{
		ItemInfo *pItemInfo = pAudioPath->m_lstItems.GetNext( pos );
		ItemInfo *pNewItemInfo = new ItemInfo;
		pNewItemInfo->Copy( *pItemInfo );
		m_lstItems.AddTail( pNewItemInfo );
	}

	// Copy the UI data
	m_wSplitterPos = pAudioPath->m_wSplitterPos;
	m_wMixGroupWidth = pAudioPath->m_wMixGroupWidth;
	m_wBusWidth = pAudioPath->m_wBusWidth;
	m_wBufferWidth = pAudioPath->m_wBufferWidth;
	m_wEffectWidth = pAudioPath->m_wEffectWidth;
	m_strName = pAudioPath->m_strName;
	m_vVersion = pAudioPath->m_vVersion;
    m_strAuthor = pAudioPath->m_strAuthor;
    m_strCopyright = pAudioPath->m_strCopyright;
    m_strSubject = pAudioPath->m_strSubject;

	// Link up the send effects
	ConnectAllSendEffects();

	// Now, change the GUID for all user-defined buffers
	pos = m_lstItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		ItemInfo *pItemInfo = m_lstItems.GetNext( pos );

		// Iterate through the item's buffers
		POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			// Get a pointer to each buffer
			BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

			// Check if the buffer is not a defined buffer
			if( !(pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED) )
			{
				// If this is a shared buffer
				if( pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_SHARED )
				{
					// Update all buffers with this GUID

					// Create a new guid
					GUID guidNew;
					::CoCreateGuid( &guidNew );

					// Save the old guid
					const GUID guidOld = pBufferOptions->guidBuffer;

					// Iterate through all itmes
					POSITION posItem = m_lstItems.GetHeadPosition();
					while( posItem )
					{
						// Get a pointer to each item
						ItemInfo *pItemInfo2 = m_lstItems.GetNext( posItem );

						// Iterate through the item's buffers
						POSITION posBuffer2 = pItemInfo2->lstBuffers.GetHeadPosition();
						while( posBuffer2 )
						{
							// Get a pointer to each buffer
							BufferOptions *pBufferOptions2 = pItemInfo2->lstBuffers.GetNext( posBuffer2 );

							// Update the buffer's guid, if necessary
							if( pBufferOptions2->guidBuffer == guidOld )
							{
								pBufferOptions2->guidBuffer = guidNew;
							}
						}
					}
				}
				else
				{
					// Just create a new guid
					::CoCreateGuid( &pBufferOptions->guidBuffer );
				}

				// If this is a mixin buffer
				if( pItemInfo->nNumPChannels == 0 )
				{
					// Should only have one buffer per mix group for a mixin mix group
					ASSERT( pItemInfo->lstBuffers.GetCount() == 1 );

					// Update any send effects sending to this item/buffer
					UpdateConnectedSendEffects( pItemInfo );
				}
			}
		}
	}

}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::CreateStandardItem

ItemInfo *CDirectMusicAudioPath::CreateStandardItem( REFGUID rGuidBuffer1, REFGUID rGuidBuffer2, DWORD dwBufferFlags )
{
	// Create a new item
	ItemInfo *pItemInfo = new ItemInfo;

	if( pItemInfo )
	{
		// Create a buffer
		BufferOptions *pBufferOptions = new BufferOptions;

		if( pBufferOptions )
		{
			// Add 128 PChannels to the item
			pItemInfo->adwPChannels = new DWORD[128];
			pItemInfo->nNumPChannels = 128;
			for( DWORD dwPChannel = 0; dwPChannel < 128; dwPChannel++ )
			{
				pItemInfo->adwPChannels[dwPChannel] = dwPChannel;
			}

			// Point the item at the default port
			pItemInfo->pPortOptions = GetDefaultPort();

			// Get a name for the item
			pItemInfo->strBandName = GetNewMixgroupName();

			// Remove all Bus IDs from the buffer
			pBufferOptions->lstBusIDs.RemoveAll();

			// Mark it as a EnvReverb buffer
			pBufferOptions->guidBuffer = rGuidBuffer1;
			pBufferOptions->dwHeaderFlags = dwBufferFlags;

			// Add the buffer to the item
			pItemInfo->lstBuffers.AddTail( pBufferOptions );

			if( rGuidBuffer2 != GUID_NULL )
			{
				// Create a buffer
				pBufferOptions = new BufferOptions;

				if( pBufferOptions )
				{
					// Remove all Bus IDs from the buffer
					pBufferOptions->lstBusIDs.RemoveAll();

					// Mark it as a EnvReverb buffer
					pBufferOptions->guidBuffer = rGuidBuffer2;
					pBufferOptions->dwHeaderFlags = dwBufferFlags;

					// Add the buffer to the item
					pItemInfo->lstBuffers.AddTail( pBufferOptions );
				}
			}
		}
		else
		{
			delete pItemInfo;
			pItemInfo = NULL;
		}
	}

	return pItemInfo;
}

ItemInfo *CreateEnvironmentItem( void )
{
	// Create a new item
	ItemInfo *pItemInfo = new ItemInfo;

	if( pItemInfo )
	{
		// Get a mix group name for the PChannel
		pItemInfo->strBandName.LoadString( IDS_BUFFER_ENVREVERB );

		// Mix Groups without PChannels don't belong to a port
		pItemInfo->pPortOptions = NULL;

		// Create a buffer
		BufferOptions *pBufferOptions = new BufferOptions;

		if( pBufferOptions )
		{
			// Remove all Bus IDs from the buffer
			pBufferOptions->lstBusIDs.RemoveAll();

			// Mark it as a EnvReverb buffer
			pBufferOptions->guidBuffer = GUID_Buffer_EnvReverb;
			pBufferOptions->dwHeaderFlags = DMUS_BUFFERF_DEFINED | DMUS_BUFFERF_SHARED;

			// Add the buffer to the item
			pItemInfo->lstBuffers.AddTail( pBufferOptions );
		}
		else
		{
			delete pItemInfo;
			pItemInfo = NULL;
		}
	}

	return pItemInfo;
}

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::CopyFrom

void CDirectMusicAudioPath::CopyFrom( DWORD dwStandardID )
{
	// Remove any exising items
	while( !m_lstItems.IsEmpty() )
	{
		delete m_lstItems.RemoveHead();
	}

	ItemInfo *pItemInfo;

	switch( dwStandardID )
	{
	case DMUS_APATH_SHARED_STEREOPLUSREVERB:
		pItemInfo = CreateStandardItem( GUID_Buffer_Stereo, GUID_Buffer_Reverb, DMUS_BUFFERF_SHARED | DMUS_BUFFERF_DEFINED );
		if( pItemInfo )
		{
			// Add the item to the list of items
			m_lstItems.AddTail( pItemInfo );
		}
		break;
		/*
	case DMUS_APATH_DYNAMIC_ENV3D:
		pItemInfo = CreateStandardItem( GUID_Buffer_3D, GUID_NULL, DMUS_BUFFERF_DEFINED );
		if( pItemInfo )
		{
			// Add the item to the list of items
			m_lstItems.AddTail( pItemInfo );
		}
		pItemInfo = CreateEnvironmentItem();
		if( pItemInfo )
		{
			// Add the item to the list of items
			m_lstItems.AddTail( pItemInfo );
		}
		break;
		*/
	case DMUS_APATH_DYNAMIC_3D:
		pItemInfo = CreateStandardItem( GUID_Buffer_3D_Dry, GUID_NULL, DMUS_BUFFERF_DEFINED );
		if( pItemInfo )
		{
			// Add the item to the list of items
			m_lstItems.AddTail( pItemInfo );
		}
		break;
	case DMUS_APATH_DYNAMIC_MONO:
		pItemInfo = CreateStandardItem( GUID_Buffer_Mono, GUID_NULL, DMUS_BUFFERF_DEFINED );
		if( pItemInfo )
		{
			// Add the item to the list of items
			m_lstItems.AddTail( pItemInfo );
		}
		break;
	case DMUS_APATH_DYNAMIC_STEREO:
		pItemInfo = CreateStandardItem( GUID_Buffer_Stereo, GUID_NULL, DMUS_BUFFERF_DEFINED );
		if( pItemInfo )
		{
			// Add the item to the list of items
			m_lstItems.AddTail( pItemInfo );
		}
		break;
	default:
		ASSERT(FALSE);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::UpdateConnectedSendEffects

void CDirectMusicAudioPath::UpdateConnectedSendEffects( const ItemInfo *pItem )
{
	// Iterate through the list of items
	POSITION posItem = m_lstItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		ItemInfo *pItemInfo = m_lstItems.GetNext( posItem );

		/* Update hidden buffers
		// If the port doesn't support buffers, skip it
		if( !ItemSupportsBuffers( pItemInfo ) )
		{
			continue;
		}
		*/

		// Iterate through the list of buffers
		POSITION posBuffer = pItemInfo->lstBuffers.GetHeadPosition();
		while( posBuffer )
		{
			// Get a pointer to each buffer
			BufferOptions *pBufferOptions = pItemInfo->lstBuffers.GetNext( posBuffer );

			/* Update hidden effects
			// If the buffer does not support effects, skip it
			if( !ItemAndBufferSupportEffects( pItemInfo, pBufferOptions ) )
			{
				continue;
			}
			*/

			// Iterate through the list of effects
			POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
			while( posEffect )
			{
				// Get a pointer to each effect
				BusEffectInfo *pBusEffectInfoInList = pBufferOptions->lstEffects.GetNext( posEffect );

				// Check if this is a sending effect
				if( (GUID_DSFX_SEND == pBusEffectInfoInList->m_EffectInfo.m_clsidObject)
				/*||	(GUID_DSFX_STANDARD_I3DL2SOURCE == pBusEffectInfoInList->m_EffectInfo.m_clsidObject)*/ )
				{
					// Check if the effect is pointing at the given item
					if( pItem == pBusEffectInfoInList->m_EffectInfo.m_pSendDestinationMixGroup )
					{
						// Update the send effect's send GUID
						pBusEffectInfoInList->m_EffectInfo.m_clsidSendBuffer = pItem->lstBuffers.GetHead()->guidBuffer;
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::DisconnectSendEffects

void CDirectMusicAudioPath::DisconnectSendEffects( const ItemInfo *pItem )
{
	UNREFERENCED_PARAMETER(pItem);

	// Just reconnect all effects
	ConnectAllSendEffects();
}

bool IsItemSentTo( const ItemInfo *pItemInfo, CTypedPtrList< CPtrList, ItemInfo *> &lstItems )
{
	if( pItemInfo->lstBuffers.IsEmpty() )
	{
		return false;
	}

	const GUID guidToFind = pItemInfo->lstBuffers.GetHead()->guidBuffer;

	POSITION pos = lstItems.GetHeadPosition();
	while( pos )
	{
		ItemInfo *pTmpItemInfo = lstItems.GetNext( pos );
		if( pTmpItemInfo != pItemInfo )
		{
			POSITION posBuffer = pTmpItemInfo->lstBuffers.GetHeadPosition();
			while( posBuffer )
			{
				BufferOptions *pBufferOptions = pTmpItemInfo->lstBuffers.GetNext( posBuffer );
				if( ItemAndBufferSupportEffects( pTmpItemInfo, pBufferOptions ) )
				{
					POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
					while( posEffect )
					{
						BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );
						if( pBusEffectInfo->m_EffectInfo.m_clsidSendBuffer == guidToFind )
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

int CompareItemsForSort( const ItemInfo *pItemInfo1, const ItemInfo *pItemInfo2 )
{
	if( pItemInfo1->nNumPChannels == 0 )
	{
		if( pItemInfo2->nNumPChannels == 0 )
		{
			// Neither have any PChannels - just compare their names
			return _tcscmp( pItemInfo1->strBandName, pItemInfo2->strBandName );
		}
		else
		{
			// Only #2 has PChannels, so it should go last
			return -1;
		}
	}
	else
	{
		if( pItemInfo2->nNumPChannels == 0 )
		{
			// Only #1 has PChannels, so it should go last
			return 1;
		}
		else
		{
			// Both have PChannels - just compare the first PChannel in the sorted array
			return pItemInfo1->adwPChannels[0] - pItemInfo2->adwPChannels[0];
		}
	}
}

void SortItemList( CTypedPtrList< CPtrList, ItemInfo *> &lstItems )
{
	bool fChanged = true;
	while( fChanged )
	{
		fChanged = false;

		POSITION pos = lstItems.GetHeadPosition();
		while( pos )
		{
			const POSITION posCurrent = pos;
			ItemInfo *pItemInfo1 = lstItems.GetNext( pos );
			if( pos )
			{
				ItemInfo *pItemInfo2 = lstItems.GetAt( pos );
				if( CompareItemsForSort( pItemInfo1, pItemInfo2 ) > 0 )
				{
					// Items out of order - rearrange them
					fChanged = true;
					lstItems.RemoveAt( posCurrent );
					lstItems.InsertAfter( pos, pItemInfo1 );
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicAudioPath::UpdateDisplayIndexes

void CDirectMusicAudioPath::UpdateDisplayIndexes( void )
{
	ConnectAllSendEffects();

	// Find all items in m_lstItems that have PChannels
	CTypedPtrList< CPtrList, ItemInfo *> lstItems;
	CTypedPtrList< CPtrList, ItemInfo *> lstItems2;
	POSITION pos = m_lstItems.GetHeadPosition();
	while( pos )
	{
		ItemInfo *pItemInfo = m_lstItems.GetNext( pos );
		if( pItemInfo->nNumPChannels > 0 )
		{
			// Add them to lstItem2 if they have PChannels
			lstItems2.AddHead( pItemInfo );
		}
		else
		{
			// Otherwise, add them to lstItems
			lstItems.AddHead( pItemInfo );
		}
	}

	// Sort lstItems2
	SortItemList( lstItems2 );

	bool fLoopFound = false;
	CTypedPtrList< CPtrList, ItemInfo *> lstItems3;
	while( !lstItems.IsEmpty() && !fLoopFound )
	{
		// Assume we find a loop
		fLoopFound = true;

		// Find all items in lstItems that are not being sent to
		pos = lstItems.GetHeadPosition();
		while( pos )
		{
			ItemInfo *pItemInfo = lstItems.GetNext( pos );
			if( !IsItemSentTo( pItemInfo, lstItems ) )
			{
				// Found an item that is not being sent to, so didn't find a loop
				// this time around
				fLoopFound = false;
				lstItems3.AddHead( pItemInfo );
			}
		}

		// Remove those items from lstItems
		pos = lstItems3.GetHeadPosition();
		while( pos )
		{
			lstItems.RemoveAt( lstItems.Find( lstItems3.GetNext( pos ), NULL ) );
		}

		// Sort them
		SortItemList( lstItems3 );

		// Add them (in order) to the head of lstItems2
		while( !lstItems3.IsEmpty() )
		{
			lstItems2.AddHead( lstItems3.RemoveTail() );
		}
	}

	// We shouldn't find a loop
	ASSERT( !fLoopFound );

	// Now, assign the display index to each item
	DWORD dwIndex = 0;
	while( !lstItems2.IsEmpty() )
	{
		lstItems2.RemoveHead()->dwDisplayIndex = dwIndex;
		dwIndex++;
	}
}
