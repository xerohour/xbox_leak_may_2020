// StyleComponent.cpp : implementation file
//

#include "stdafx.h"

#include "StyleDesignerDLL.h"
#include "Style.h"
#include "StyleRef.h"
#include "VarChoices.h"


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent constructor/destructor 

CStyleComponent::CStyleComponent()
{
    m_dwRef = 0;
	
	m_pIDMPerformance = NULL;
	m_pIFramework = NULL;

	m_pIBandComponent = NULL;

	m_pIStyleDocType8 = NULL;
	m_pIConductor = NULL;
	m_nFirstImage = 0;

	m_nNextStyle = 0;

	m_cfStyle = 0;
	m_cfBand = 0;
	m_cfBandList = 0;
	m_cfBandTrack = 0;
	m_cfTimeline = 0;
	m_cfMotif = 0;
	m_cfMotifList = 0;
	m_cfPattern = 0;
	m_cfPatternList = 0;
	m_cfVarChoices = 0;
}

CStyleComponent::~CStyleComponent()
{
	ReleaseAll();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::ReleaseAll

void CStyleComponent::ReleaseAll( void )
{
	CDirectMusicStyle *pStyle;

	while( !m_lstStyles.IsEmpty() )
	{
		pStyle = static_cast<CDirectMusicStyle*>( m_lstStyles.RemoveHead() );
		RELEASE( pStyle );
	}

	RELEASE( m_pIFramework );
	RELEASE( m_pIBandComponent );
	RELEASE( m_pIStyleDocType8 );
	RELEASE( m_pIDMPerformance );
	RELEASE( m_pIConductor );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IUnknown implementation

HRESULT CStyleComponent::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdComponent)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IDMUSProdComponent *)this;
    }
	else if( ::IsEqualIID(riid, IID_IDMUSProdPortNotify) )
	{
		*ppvObj = (IDMUSProdPortNotify *)this;
	}
    else if( ::IsEqualIID(riid, IID_IDMUSProdRIFFExt) )
    {
        *ppvObj = (IDMUSProdRIFFExt *)this;
    }
	else if( ::IsEqualIID(riid, IID_IAllocVarChoices) )
	{
		*ppvObj = (IAllocVarChoices *)this;
	}
	else
	{
	    *ppvObj = NULL;
		return E_NOINTERFACE;
	}

	static_cast<IUnknown *>(*ppvObj)->AddRef();
	return S_OK;
}

ULONG CStyleComponent::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CStyleComponent::Release()
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
// CStyleComponent IDMUSProdComponent implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IDMUSProdComponent::Initialize

HRESULT CStyleComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
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

	theApp.m_pStyleComponent = this;
//	theApp.m_pStyleComponent->AddRef();	intentionally missing

	// Get IConductor and IDirectMusicPerformance interface pointers 
	if( FAILED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  FAILED ( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor ) )
	||  FAILED ( m_pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIDMPerformance ) ) )
	{
		ReleaseAll();
		if( pIComponent )
		{
			RELEASE( pIComponent );
		}
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONDUCTOR, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	RELEASE( pIComponent );

	// Get the IDMUSProdComponent interface for the Band component 
	if( FAILED ( pIFramework->FindComponent( CLSID_BandComponent,  &m_pIBandComponent ) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_BAND, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

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

	// Register the clipboard format for an .stp file 
	CString strExt = _T(".stp");
	BSTR bstrExt = strExt.AllocSysString();
	if( FAILED ( pIFramework->RegisterClipFormatForFile(m_cfStyle, bstrExt) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_CLIPFORMAT, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register applicable doc types with the Framework 
	m_pIStyleDocType8 = new CStyleDocType;
    if( m_pIStyleDocType8 == NULL )
    {
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
        return E_OUTOFMEMORY;
    }

	m_pIStyleDocType8->AddRef();

	if( FAILED ( pIFramework->AddDocType(m_pIStyleDocType8) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IDMUSProdComponent::CleanUp

HRESULT CStyleComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicStyle *pStyle;

	while( !m_lstStyles.IsEmpty() )
	{
		pStyle = static_cast<CDirectMusicStyle*>( m_lstStyles.RemoveHead() );
		RELEASE( pStyle );
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IDMUSProdComponent::GetName

HRESULT CStyleComponent::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_STYLE_COMPONENT_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::IDMUSProdComponent::AllocReferenceNode

HRESULT CStyleComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	// Make sure Component can create Nodes of type guidRefNodeId
	if( !( IsEqualGUID ( guidRefNodeId, GUID_StyleRefNode ) ) )
	{
		return E_INVALIDARG;
	}

	// Create StyleRefNode
	CStyleRef* pStyleRef = new CStyleRef;
	if( pStyleRef == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppIRefNode = (IDMUSProdNode *)pStyleRef;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IDMUSProdComponent::OnActivateApp

HRESULT CStyleComponent::OnActivateApp( BOOL fActivate )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fActivate);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IDMUSProdPortNotify implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IDMUSProdPortNotify::OnOutputPortsChanged

HRESULT CStyleComponent::OnOutputPortsChanged( void )
{
	CDirectMusicStyle *pStyle;

	POSITION pos = m_lstStyles.GetHeadPosition();
	while( pos )
	{
		pStyle = m_lstStyles.GetNext( pos );
		pStyle->OnOutputPortsChanged();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IDMUSProdPortNotify::OnOutputPortsRemoved

HRESULT CStyleComponent::OnOutputPortsRemoved( void )
{
	CDirectMusicStyle *pStyle;

	POSITION pos = m_lstStyles.GetHeadPosition();
	while( pos )
	{
		pStyle = m_lstStyles.GetNext( pos );
		pStyle->OnOutputPortsRemoved();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IDMUSProdRIFFExt implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IDMUSProdRIFFExt::LoadRIFFChunk

HRESULT CStyleComponent::LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicStyle* pStyle;
	HRESULT hr;

	ASSERT( pIStream != NULL );
	ASSERT( m_pIFramework != NULL );

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

	// Create a new Style 
	pStyle = new CDirectMusicStyle;
	if( pStyle == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	// Load the Style file
	hr = pStyle->Load( pIStream );
	if( !SUCCEEDED ( hr ) )
	{
		pStyle->Release();
		return hr;
	}

	*ppINode = (IDMUSProdNode *)pStyle;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IAllocVarChoices implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleComponent IAllocVarChoices::GetVarChoicesNode

HRESULT CStyleComponent::GetVarChoicesNode( IUnknown** ppIVarChoicesNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIVarChoicesNode == NULL )
	{
		return E_POINTER;
	}

	*ppIVarChoicesNode = NULL;

	// Create a new Variation Choices node 
	CVarChoices* pVarChoices = new CVarChoices;
	if( pVarChoices == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	// Query for an IUnknown interface
	HRESULT hr = pVarChoices->QueryInterface( IID_IUnknown, (void**)ppIVarChoicesNode );

	// Release our reference on the node
	pVarChoices->Release();

	// Return the QI result
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent additional functions

/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::AddNodeImageLists

HRESULT CStyleComponent::AddNodeImageLists( void )
{
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, ILC_COLOR16, 10, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_STYLE_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_STYLE_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_MOTIF_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_MOTIF_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_PATTERN_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_PATTERN_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_STYLEREF) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_STYLEREF_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( FAILED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstImage ) ) )
	{
		return E_FAIL;
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::RegisterClipboardFormats

BOOL CStyleComponent::RegisterClipboardFormats( void )
{
	m_cfProducerFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );
	m_cfStyle = ::RegisterClipboardFormat( CF_STYLE );
	m_cfBand = ::RegisterClipboardFormat( CF_BAND );
	m_cfBandList = ::RegisterClipboardFormat( CF_BANDLIST );
	m_cfBandTrack = ::RegisterClipboardFormat( CF_BANDTRACK );
	m_cfTimeline = ::RegisterClipboardFormat( CF_TIMELINE );
	m_cfMotif = ::RegisterClipboardFormat( CF_MOTIF );
	m_cfMotifList = ::RegisterClipboardFormat( CF_MOTIFLIST );
	m_cfPattern = ::RegisterClipboardFormat( CF_PATTERN );
	m_cfPatternList = ::RegisterClipboardFormat( CF_PATTERNLIST );
	m_cfVarChoices = ::RegisterClipboardFormat( CF_VARCHOICES );

	if( m_cfProducerFile == 0
	||  m_cfStyle == 0
	||  m_cfBand == 0
	||  m_cfBandList == 0
	||  m_cfBandTrack == 0
	||  m_cfTimeline == 0
	||  m_cfMotif == 0
	||  m_cfMotifList == 0
	||  m_cfPattern == 0
	||  m_cfPatternList == 0
	||  m_cfVarChoices == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::GetStyleImageIndex

HRESULT CStyleComponent::GetStyleImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_STYLE_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::GetFolderImageIndex

HRESULT CStyleComponent::GetFolderImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_FOLDER_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::GetMotifImageIndex

HRESULT CStyleComponent::GetMotifImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_MOTIF_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::GetPatternImageIndex

HRESULT CStyleComponent::GetPatternImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_PATTERN_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::GetStyleRefImageIndex

HRESULT CStyleComponent::GetStyleRefImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_STYLEREF_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::AddToStyleFileList

void CStyleComponent::AddToStyleFileList( CDirectMusicStyle* pStyle )
{
	if( pStyle )
	{
		GUID guidStyle;
		pStyle->GetGUID( &guidStyle );

		// Prevent duplicate object GUIDs
		GUID guidStyleList;
		POSITION pos = m_lstStyles.GetHeadPosition();
		while( pos )
		{
			CDirectMusicStyle* pStyleList = m_lstStyles.GetNext( pos );

			pStyleList->GetGUID( &guidStyleList );
			if( ::IsEqualGUID( guidStyleList, guidStyle ) )
			{
				::CoCreateGuid( &guidStyle );
				pStyle->SetGUID( guidStyle );
				break;
			}
		}

		// Add to list
		pStyle->AddRef();
		m_lstStyles.AddTail( pStyle );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComponent::RemoveFromStyleFileList

void CStyleComponent::RemoveFromStyleFileList( CDirectMusicStyle* pStyle )
{
	if( pStyle )
	{
		// Remove from list
		POSITION pos = m_lstStyles.Find( pStyle );
		if( pos )
		{
			m_lstStyles.RemoveAt( pos );
			pStyle->Release();
		}
	}
}
