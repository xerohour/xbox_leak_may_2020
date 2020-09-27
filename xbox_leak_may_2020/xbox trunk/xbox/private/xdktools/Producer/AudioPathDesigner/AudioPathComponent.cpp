// AudioPathComponent.cpp : implementation file
//

#include "stdafx.h"

#include "AudioPathDesignerDLL.h"
#include "AudioPath.h"
#include "AudioPathRef.h"
#include "ItemInfo.h"
#include "EffectInfo.h"
#include "EffectPPGMgr.h"
#include "MixGroupPPGMgr.h"
#include "BufferPPGMgr.h"
#include "ToolGraphDesigner.h"


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent constructor/destructor 

CAudioPathComponent::CAudioPathComponent()
{
    m_dwRef = 0;
	
	m_pIFramework = NULL;
	m_pIDMPerformance = NULL;
	m_pIDMusic = NULL;

	m_pIAudioPathDocType8 = NULL;
	m_pIConductor = NULL;
	m_pIToolGraphComponent	= NULL;
	m_nFirstImage = 0;

	m_nNextAudioPath = 0;

	m_cfProducerFile = 0;
	m_cfAudioPath = 0;
	m_cfAudioPathList = 0;
	m_cfGraph= 0;

	m_pIAudioPathPageManager = NULL;
	m_pIEffectPageManager = NULL;
	m_pIMixGroupPageManager = NULL;
	m_pIBufferPageManager = NULL;
}

CAudioPathComponent::~CAudioPathComponent()
{
	ReleaseAll();
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent::ReleaseAll

void CAudioPathComponent::ReleaseAll( void )
{
 	CDirectMusicAudioPath *pAudioPath;

	while( !m_lstAudioPaths.IsEmpty() )
	{
		pAudioPath = static_cast<CDirectMusicAudioPath*>( m_lstAudioPaths.RemoveHead() );
		RELEASE( pAudioPath );
	}

	if( m_pIFramework )
	{
		if( m_pIAudioPathPageManager || m_pIEffectPageManager
		||	m_pIMixGroupPageManager || m_pIBufferPageManager )
		{
			IDMUSProdPropSheet *pIPropSheet;
			if(SUCCEEDED(m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void **) &pIPropSheet)))
			{
				pIPropSheet->RemovePageManager(m_pIAudioPathPageManager);
				pIPropSheet->RemovePageManager(m_pIEffectPageManager);
				pIPropSheet->RemovePageManager(m_pIMixGroupPageManager);
				pIPropSheet->RemovePageManager(m_pIBufferPageManager);
				pIPropSheet->Release();
			}
		}
	}

	RELEASE( m_pIToolGraphComponent );
	RELEASE( m_pIAudioPathPageManager );
	RELEASE( m_pIEffectPageManager );
	RELEASE( m_pIMixGroupPageManager );
	RELEASE( m_pIBufferPageManager );
	RELEASE( m_pIFramework );
	RELEASE( m_pIAudioPathDocType8 );
	RELEASE( m_pIDMPerformance );
	RELEASE( m_pIDMusic );
	RELEASE( m_pIConductor );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent IUnknown implementation

HRESULT CAudioPathComponent::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdComponent)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdComponent *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdRIFFExt) )
    {
        AddRef();
        *ppvObj = (IDMUSProdRIFFExt *)this;
        return S_OK;
    }
	
    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CAudioPathComponent::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CAudioPathComponent::Release()
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
// CAudioPathComponent IDMUSProdComponent implementation

/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent IDMUSProdComponent::Initialize

HRESULT CAudioPathComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdComponent* pIComponent = NULL;
	TCHAR achErrMsg[MID_BUFFER];
	CString strErrMsg;

	if( m_pIFramework )		// already initialized
	{
		return S_OK;
	}

	ASSERT( pIFramework != NULL );
	ASSERT( pbstrErrMsg != NULL );

	if( pbstrErrMsg == NULL )
	{
		return E_POINTER;
	}

	if( pIFramework == NULL )
	{
		::LoadString( theApp.m_hInstance, IDS_ERR_INVALIDARG, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_INVALIDARG;
	}

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	theApp.m_pAudioPathComponent = this;
//	theApp.m_pAudioPathComponent->AddRef();	intentionally missing

	// Get IConductor interface pointer
	if( FAILED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  FAILED ( pIComponent->QueryInterface( IID_IDMUSProdConductor8, (void**)&m_pIConductor ) ) )
	{
		ReleaseAll();
		RELEASE( pIComponent );
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONDUCTOR, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	RELEASE( pIComponent );

	// It's ok if we don't find the Toolgraph component, since Xbox doesn't support it
	pIFramework->FindComponent( CLSID_ToolGraphComponent, &m_pIToolGraphComponent );

	// Get the DirectMusic object
	if( FAILED( pIFramework->GetSharedObject( CLSID_DirectMusic, IID_IDirectMusic, (LPVOID*)&m_pIDMusic) ) )
	{
		if( FAILED( CoCreateInstance(CLSID_DirectMusic,
								  NULL,
								  CLSCTX_INPROC_SERVER,
								  IID_IDirectMusic,
								  (LPVOID*)&m_pIDMusic) ) )
		{
			ReleaseAll();
			::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_DX8, achErrMsg, MID_BUFFER );
			strErrMsg = achErrMsg;
			*pbstrErrMsg = strErrMsg.AllocSysString();
			return E_FAIL;
		}
	}

	// Get IDirectMusicPerformance8 interface pointer
	IUnknown *punkPerformance = NULL;
	if( FAILED ( m_pIConductor->GetPerformanceEngine( (IUnknown**)&punkPerformance ) )
	||	FAILED ( punkPerformance->QueryInterface( IID_IDirectMusicPerformance8, (void**)&m_pIDMPerformance ) ) )
	{
		ReleaseAll();
		RELEASE(punkPerformance);
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_DX8, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	RELEASE(punkPerformance);

	// Add applicable images to the Project Tree control's image list 
	if( FAILED ( AddNodeImageLists() ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_IMAGELIST, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register clipboard formats
	if( RegisterClipboardFormats() == FALSE )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_REGISTER_CF, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register the clipboard format for an .aup file 
	CString strExt = _T(".aup");
	BSTR bstrExt = strExt.AllocSysString();
	if( FAILED ( pIFramework->RegisterClipFormatForFile(m_cfAudioPath, bstrExt) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_CLIPFORMAT, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register applicable doc types with the Framework 
	m_pIAudioPathDocType8 = new CAudioPathDocType;
    if( m_pIAudioPathDocType8 == NULL )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_OUTOFMEMORY;
	}

	m_pIAudioPathDocType8->AddRef();

	if( FAILED ( pIFramework->AddDocType(m_pIAudioPathDocType8) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	if(!m_pIAudioPathPageManager)
	{
		CAudioPathPropPageManager *pPageManager;

		pPageManager = new CAudioPathPropPageManager();
		pPageManager->QueryInterface(IID_IDMUSProdPropPageManager, (void **) &m_pIAudioPathPageManager);
		pPageManager->Release();
	}

	if(!m_pIEffectPageManager)
	{
		CEffectPPGMgr *pPageManager;

		pPageManager = new CEffectPPGMgr();
		pPageManager->QueryInterface(IID_IDMUSProdPropPageManager, (void **) &m_pIEffectPageManager);
		pPageManager->Release();
	}

	if(!m_pIMixGroupPageManager)
	{
		CMixGroupPPGMgr *pPageManager;

		pPageManager = new CMixGroupPPGMgr();
		pPageManager->QueryInterface(IID_IDMUSProdPropPageManager, (void **) &m_pIMixGroupPageManager);
		pPageManager->Release();
	}

	if(!m_pIBufferPageManager)
	{
		CBufferPPGMgr *pPageManager;

		pPageManager = new CBufferPPGMgr();
		pPageManager->QueryInterface(IID_IDMUSProdPropPageManager, (void **) &m_pIBufferPageManager);
		pPageManager->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent IDMUSProdComponent::CleanUp

HRESULT CAudioPathComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicAudioPath *pAudioPath;

	while( !m_lstAudioPaths.IsEmpty() )
	{
		pAudioPath = static_cast<CDirectMusicAudioPath*>( m_lstAudioPaths.RemoveHead() );
		RELEASE( pAudioPath );
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent IDMUSProdComponent::GetName

HRESULT CAudioPathComponent::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_AUDIOPATH_COMPONENT_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent::IDMUSProdComponent::AllocReferenceNode

HRESULT CAudioPathComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	// Make sure Component can create Nodes of type guidRefNodeId
	if( !( IsEqualGUID ( guidRefNodeId, GUID_AudioPathRefNode ) ) )
	{
		return E_INVALIDARG;
	}

	// Create AudioPathRefNode
	CAudioPathRef* pAudioPathRef = new CAudioPathRef;
	if( pAudioPathRef == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppIRefNode = (IDMUSProdNode *)pAudioPathRef;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent IDMUSProdComponent::OnActivateApp

HRESULT CAudioPathComponent::OnActivateApp( BOOL fActivate )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fActivate);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent IDMUSProdRIFFExt::LoadRIFFChunk

HRESULT CAudioPathComponent::LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicAudioPath* pAudioPath;
	HRESULT hr;

	ASSERT( pIStream != NULL );
	ASSERT( m_pIFramework != NULL );

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

	// Create a new AudioPath 
	pAudioPath = new CDirectMusicAudioPath;
	if( pAudioPath == NULL )
	{
		return E_OUTOFMEMORY ;
	}
			
	// Create the Undo Manager
	if( pAudioPath->CreateUndoMgr() == FALSE )
	{
		pAudioPath->Release();
		return E_OUTOFMEMORY;
	}

	// Load the AudioPath file
	hr = pAudioPath->Load( pIStream );
	if( !SUCCEEDED ( hr ) )
	{
		pAudioPath->Release();
		return hr;
	}

	*ppINode = (IDMUSProdNode *)pAudioPath;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent additional functions

/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent::AddNodeImageLists

HRESULT CAudioPathComponent::AddNodeImageLists( void )
{
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, ILC_COLOR16, 4, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_AUDIOPATH_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_AUDIOPATH_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_AUDIOPATHREF) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_AUDIOPATHREF_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( FAILED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstImage ) ) )
	{
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent::RegisterClipboardFormats

BOOL CAudioPathComponent::RegisterClipboardFormats( void )
{
	m_cfProducerFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );
	m_cfAudioPath = ::RegisterClipboardFormat( CF_AUDIOPATH );
	m_cfAudioPathList = ::RegisterClipboardFormat( CF_AUDIOPATHLIST );
	m_cfGraph = ::RegisterClipboardFormat( CF_GRAPH );

	if( m_cfProducerFile == 0
	||  m_cfAudioPath == 0
	||  m_cfAudioPathList == 0 
	||  m_cfGraph == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent::GetAudioPathImageIndex

HRESULT CAudioPathComponent::GetAudioPathImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_AUDIOPATH_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent::GetAudioPathRefImageIndex

HRESULT CAudioPathComponent::GetAudioPathRefImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_AUDIOPATHREF_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent::AddToAudioPathFileList

void CAudioPathComponent::AddToAudioPathFileList( CDirectMusicAudioPath* pAudioPath )
{
	if( pAudioPath )
	{
		GUID guidAudioPath;
		pAudioPath->GetGUID( &guidAudioPath );

		// Prevent duplicate object GUIDs
		GUID guidAudioPathList;
		POSITION pos = m_lstAudioPaths.GetHeadPosition();
		while( pos )
		{
			CDirectMusicAudioPath* pAudioPathList = m_lstAudioPaths.GetNext( pos );

			pAudioPathList->GetGUID( &guidAudioPathList );
			if( ::IsEqualGUID( guidAudioPathList, guidAudioPath ) )
			{
				::CoCreateGuid( &guidAudioPath );
				pAudioPath->SetGUID( guidAudioPath );
				break;
			}
		}

		// Add to list
		pAudioPath->AddRef();
		m_lstAudioPaths.AddTail( pAudioPath );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathComponent::RemoveFromAudioPathFileList

void CAudioPathComponent::RemoveFromAudioPathFileList( CDirectMusicAudioPath* pAudioPath )
{
	if( pAudioPath )
	{
		// Remove from list
		POSITION pos = m_lstAudioPaths.Find( pAudioPath );
		if( pos )
		{
			m_lstAudioPaths.RemoveAt( pos );
			pAudioPath->Release();
		}
	}
}
