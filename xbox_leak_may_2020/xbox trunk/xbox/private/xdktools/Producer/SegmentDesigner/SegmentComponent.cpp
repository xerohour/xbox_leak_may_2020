// SegmentComponent.cpp : implementation file
//

#include "stdafx.h"

#include "SegmentComponent.h"
#include "SegmentDesignerDLL.h"
#include "SegmentDesigner.h"
#include "Segment.h"
#include "SegmentDlg.h"
#include "SegmentGuids.h"
#include "SegmentPPGMgr.h"
#include "TemplateDocType.h"
#include "resource.h"
#include "SegmentRef.h"
#include <StyleDesigner.h>
#include <ChordMapDesigner.h>
#include <dmusici.h>
#include "AudioPathDesigner.h"
#include "ToolGraphDesigner.h"
#include "ContainerDesigner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent constructor/destructor

CSegmentComponent::CSegmentComponent()
{
    m_dwRef					= 0;
	AddRef();
	m_pIDMPerformance		= NULL;
	m_pIFramework			= NULL;
	m_pIDocType8			= NULL;
	m_pITemplateDocType8	= NULL;
	m_pIConductor			= NULL;
	m_pIContainerComponent	= NULL;
	m_pIAudioPathComponent	= NULL;
	m_pIToolGraphComponent	= NULL;
	m_pIPageManager			= NULL;
	m_nFirstSegmentImage	= 0;
	m_nFirstSegmentRefImage	= 0;
	m_nFirstFolderImage		= 0;
	m_nNextSegment			= 0;		

	m_cfJazzFile = 0;
	m_cfSegment = 0;
	m_cfContainer = 0;
	m_cfAudioPath = 0;
	m_cfGraph = 0;
}

CSegmentComponent::~CSegmentComponent()
{
	ReleaseAll();
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent::ReleaseAll

void CSegmentComponent::ReleaseAll( void )
{
	CSegment *pSegment;

	while( !m_lstSegments.IsEmpty() )
	{
		pSegment = static_cast<CSegment*>( m_lstSegments.RemoveHead() );
		pSegment->Release();
	}

	if( m_pIDocType8 )
	{
		m_pIDocType8->Release();
		m_pIDocType8 = NULL;
	}

	if( m_pITemplateDocType8 )
	{
		m_pITemplateDocType8->Release();
		m_pITemplateDocType8 = NULL;
	}

	if( m_pIFramework )
	{
		if(m_pIPageManager)
		{
			IDMUSProdPropSheet *pIPropSheet;
			if(SUCCEEDED(m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void **) &pIPropSheet)))
			{
				pIPropSheet->RemovePageManager(m_pIPageManager);
				pIPropSheet->Release();
			}
		}
		m_pIFramework->Release();
		m_pIFramework = NULL;
	}

	if(m_pIPageManager)
	{
		m_pIPageManager->Release(); // This is to delete the reference we added through QI
		m_pIPageManager->Release();	// Delete the page manager.
		m_pIPageManager = NULL;
	}

	RELEASE( m_pIContainerComponent );
	RELEASE( m_pIAudioPathComponent );
	RELEASE( m_pIToolGraphComponent );
	RELEASE( m_pIDMPerformance );
	RELEASE( m_pIConductor );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent IUnknown implementation

HRESULT CSegmentComponent::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdComponent)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IDMUSProdComponent *)this;
    }
    else if( ::IsEqualIID(riid, IID_IDMUSProdRIFFExt) )
    {
        *ppvObj = (IDMUSProdRIFFExt *)this;
    }
	else if( ::IsEqualIID(riid, IID_IDMUSProdPortNotify) )
	{
		*ppvObj = (IDMUSProdPortNotify *)this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdNotifySink))
	{
		*ppvObj = (IDMUSProdNotifySink*)this;
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

ULONG CSegmentComponent::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CSegmentComponent::Release()
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
// CSegmentComponent IDMUSProdComponent implementation

/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent IDMUSProdComponent::Initialize

HRESULT CSegmentComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = S_OK;
	IDMUSProdComponent* pIComponent = NULL;
	TCHAR achErrMsg[MID_BUFFER];
	CString strErrMsg;

	if( m_pIFramework )		// already initialized
	{
		return S_OK;
	}

	if( pIFramework == NULL
	||  pbstrErrMsg == NULL )
	{
		::LoadString( theApp.m_hInstance, IDS_ERR_INVALIDARG, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		ASSERT(pbstrErrMsg);
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_INVALIDARG;
	}

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	// Get IConductor and IDirectMusicPerformance interface pointers 
	if( !SUCCEEDED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  !SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdConductor8, (void**)&m_pIConductor ) )
	||  !SUCCEEDED ( m_pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIDMPerformance ) ) )
	{
		ReleaseAll();
		if( pIComponent )
		{
			pIComponent->Release();
		}
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONDUCTOR, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	RELEASE( pIComponent );

	// Get IDMUSProdComponent for Container Designer
	if( FAILED ( pIFramework->FindComponent( CLSID_ContainerComponent, &m_pIContainerComponent ) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONTAINER_DESIGNER, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Get IDMUSProdComponent for AudioPath Designer
	if( FAILED ( pIFramework->FindComponent( CLSID_AudioPathComponent, &m_pIAudioPathComponent ) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_AUDIOPATH_DESIGNER, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// It's ok if we don't find the Toolgraph component, since Xbox doesn't support it
	pIFramework->FindComponent( CLSID_ToolGraphComponent, &m_pIToolGraphComponent );

	// Register clipboard formats
	if( RegisterClipboardFormats() == FALSE )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_REGISTER_CF, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register the clipboard format for an .sgp file 
	CString strExt = _T(".sgp");
	BSTR bstrExt = strExt.AllocSysString();
	if( !SUCCEEDED ( pIFramework->RegisterClipFormatForFile(m_cfSegment, bstrExt) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_CLIPFORMAT, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Add the Segment Doc Type
	m_pIDocType8 = new CSegmentDocType( this );
    if( m_pIDocType8 == NULL )
    {
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
        return E_OUTOFMEMORY;
    }

	m_pIDocType8->AddRef();

	if( !SUCCEEDED ( pIFramework->AddDocType(m_pIDocType8) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Add the Template Doc Type
	m_pITemplateDocType8 = new CTemplateDocType( this );
    if( m_pITemplateDocType8 == NULL )
    {
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
        return E_OUTOFMEMORY;
    }

	m_pITemplateDocType8->AddRef();

	if( !SUCCEEDED ( pIFramework->AddDocType(m_pITemplateDocType8) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	if( !SUCCEEDED ( AddNodeImageLists() ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_IMAGELIST, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	if(!m_pIPageManager)
	{
		CSegmentPPGMgr *pPageManager;

		pPageManager = new CSegmentPPGMgr();
		hr = pPageManager->QueryInterface(IID_IDMUSProdPropPageManager, (void **) &m_pIPageManager);
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent IDMUSProdComponent::CleanUp

HRESULT CSegmentComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CSegment *pSegment;

	while( !m_lstSegments.IsEmpty() )
	{
		pSegment = static_cast<CSegment*>( m_lstSegments.RemoveHead() );
		pSegment->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent IDMUSProdComponent::GetName

HRESULT CSegmentComponent::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_SEGMENT_COMPONENT_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// CSegmentComponent IDMUSProdComponent::OnActivateApp
HRESULT CSegmentComponent::OnActivateApp(BOOL fActivate)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fActivate);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent IDMUSProdRIFFExt::LoadRIFFChunk

HRESULT CSegmentComponent::LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return LoadSegment( pIStream, ppINode );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent IDMUSProdComponent::AllocReferenceNode

HRESULT CSegmentComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	// Make sure Component can create Nodes of type guidRefNodeId
	if( !( IsEqualGUID ( guidRefNodeId, GUID_SegmentRefNode ) ) )
	{
		return E_INVALIDARG;
	}

	// Create SegmentRefNode
	CSegmentRef* pSegmentRef = new CSegmentRef( this );
	if( pSegmentRef == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppIRefNode = (IDMUSProdNode *)pSegmentRef;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent::BroadcastNotification

void CSegmentComponent::BroadcastNotification( REFGUID rguidNotification )
{
	// Iterate through all segments
	POSITION pos = m_lstSegments.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the segment
		CSegment *pSegment = m_lstSegments.GetNext( pos );

		// Notify the strip managers
		if( pSegment->m_pSegmentDlg
		&&  pSegment->m_pSegmentDlg->m_pTimeline )
		{
			pSegment->m_pSegmentDlg->m_pTimeline->NotifyStripMgrs( rguidNotification, 0xFFFFFFFF, NULL );
		}
		else
		{
			pSegment->NotifyAllStripMgrs( rguidNotification, 0xFFFFFFFF, NULL );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent IDMUSProdPortNotify::OnOutputPortsChanged

HRESULT CSegmentComponent::OnOutputPortsChanged( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BroadcastNotification( GUID_Conductor_OutputPortsChanged );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent IDMUSProdPortNotify::OnOutputPortsRemoved

HRESULT CSegmentComponent::OnOutputPortsRemoved( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BroadcastNotification( GUID_Conductor_OutputPortsRemoved );

	return S_OK;
}


///////////////////////////////////////////////////////////////////////////
// CSegmentComponent IDMUSProdNotifySink::OnUpdate

HRESULT CSegmentComponent::OnUpdate( IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDocRootNode);
	UNREFERENCED_PARAMETER(pData);

	HRESULT hr = E_FAIL;

	// GUID_Conductor_UnloadWaves
	// GUID_Conductor_DownloadWaves
	if( ::IsEqualGUID( guidUpdateType, GUID_ConductorUnloadWaves ) 
	||  ::IsEqualGUID( guidUpdateType, GUID_ConductorDownloadWaves )
	||  ::IsEqualGUID( guidUpdateType, GUID_ConductorFlushWaves ) )
	{
		BroadcastNotification( guidUpdateType );
		hr = S_OK;
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

	
/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent::LoadSegment

HRESULT CSegmentComponent::LoadSegment( IStream* pIStream, IDMUSProdNode** ppINode )
{
	CSegment* pSegment;
	HRESULT hr;

	ASSERT( pIStream != NULL );
	ASSERT( m_pIFramework != NULL );

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

	// Create a new Segment 
	pSegment = new CSegment( this );
	if( pSegment == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	hr = pSegment->Initialize();
	if(FAILED(hr))
	{
		return hr;
	}

	// Get the filename
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;

		pPersistInfo->GetFileName( &bstrFileName );
		pSegment->m_strOrigFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		int nFindPos = pSegment->m_strOrigFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			pSegment->m_strOrigFileName = pSegment->m_strOrigFileName.Right( pSegment->m_strOrigFileName.GetLength() - nFindPos - 1 );
		}

		pPersistInfo->Release();
	}

	// Load the Segment file
	{
		// Start a wait cursor
		CWaitCursor cursor;

		hr = pSegment->Load(pIStream);
	}

	if( FAILED(hr) )
	{
		pSegment->Release();
		pSegment = 0;
	}

	if(pSegment)
		pSegment->SetModifiedFlag(FALSE);
	*ppINode = (IDMUSProdNode *)pSegment;
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent additional functions

/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent::AddNodeImageLists

HRESULT CSegmentComponent::AddNodeImageLists( void )
{
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_SEGMENT_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_SEGMENT_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstSegmentImage ) ) )
	{
		return E_FAIL;
	}

	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_SEGMENTREF_NODE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_SEGMENTREF_NODE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstSegmentRefImage ) ) )
	{
		return E_FAIL;
	}
	
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstFolderImage ) ) )
	{
		return E_FAIL;
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent::GetSegmentImageIndex

HRESULT CSegmentComponent::GetSegmentImageIndex( short* pnFirstImage )
{
    *pnFirstImage = m_nFirstSegmentImage;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent::GetSegmentRefImageIndex

HRESULT CSegmentComponent::GetSegmentRefImageIndex( short* pnFirstImage )
{
    *pnFirstImage = m_nFirstSegmentRefImage;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent::GetFolderImageIndex

HRESULT CSegmentComponent::GetFolderImageIndex( short* pnFirstImage )
{
    *pnFirstImage = m_nFirstFolderImage;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent::RegisterClipboardFormats

BOOL CSegmentComponent::RegisterClipboardFormats( void )
{
	m_cfJazzFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );
	m_cfSegment = ::RegisterClipboardFormat (CF_SEGMENT);
	m_cfContainer = ::RegisterClipboardFormat (CF_CONTAINER);
	m_cfAudioPath = ::RegisterClipboardFormat (CF_AUDIOPATH);
	m_cfGraph = ::RegisterClipboardFormat (CF_GRAPH);

	if( m_cfJazzFile == 0
	||  m_cfSegment == 0 
	||  m_cfContainer == 0 
	||  m_cfAudioPath == 0 
	||  m_cfGraph == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent::AddToSegmentFileList

void CSegmentComponent::AddToSegmentFileList( CSegment* pSegment )
{
	if( pSegment )
	{
		GUID guidSegment;
		pSegment->GetGUID( &guidSegment );

		// Prevent duplicate object GUIDs
		GUID guidSegmentList;
		POSITION pos = m_lstSegments.GetHeadPosition();
		while( pos )
		{
			CSegment* pSegmentList = m_lstSegments.GetNext( pos );

			pSegmentList->GetGUID( &guidSegmentList );
			if( ::IsEqualGUID( guidSegmentList, guidSegment ) )
			{
				::CoCreateGuid( &guidSegment );
				pSegment->SetGUID( guidSegment );
				break;
			}
		}

		// Add to list
		pSegment->AddRef();
		m_lstSegments.AddTail( pSegment );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentComponent::RemoveFromSegmentFileList

void CSegmentComponent::RemoveFromSegmentFileList( CSegment* pSegment )
{
	if( pSegment )
	{
		// Remove from list
		POSITION pos = m_lstSegments.Find( pSegment );
		if( pos )
		{
			m_lstSegments.RemoveAt( pos );
			pSegment->Release();
		}
	}
}
