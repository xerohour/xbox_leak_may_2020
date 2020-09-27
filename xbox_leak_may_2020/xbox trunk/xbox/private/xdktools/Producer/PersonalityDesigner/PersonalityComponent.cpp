// PersonalityComponent.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "PersonalityDesigner.h"
#include "Personality.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent constructor/destructor

CPersonalityComponent::CPersonalityComponent()
{
    m_dwRef			= 0;
	m_pIEngine		= NULL;
	m_pIFramework	= NULL;
	m_pIDocType8	= NULL;
	m_pIConductor	= NULL;

	m_bDirty		= FALSE;

	m_nFirstPersonalityImage = 0;
	m_nNextPersonality = 0;		

	m_cfStyle = 0;
	m_cfPersonality = 0;
}

CPersonalityComponent::~CPersonalityComponent()
{
	ReleaseAll();
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::ReleaseAll

void CPersonalityComponent::ReleaseAll( void )
{
	CPersonality *pPersonality;

	while( !m_lstPersonalities.IsEmpty() )
	{
		pPersonality = static_cast<CPersonality*>( m_lstPersonalities.RemoveHead() );
		pPersonality->Release();
	}

	RELEASE( m_pIDocType8 );
	m_pIDocType8 = NULL;

	RELEASE( m_pIFramework );
	m_pIFramework = NULL;
	
	RELEASE( m_pIEngine );
	m_pIEngine = NULL;

	RELEASE( m_pIConductor );
	m_pIConductor = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent IUnknown implementation

HRESULT CPersonalityComponent::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CPersonalityComponent::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CPersonalityComponent::Release()
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
// CPersonalityComponent IDMUSProdComponent implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent IDMUSProdComponent::Initialize

HRESULT CPersonalityComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_INVALIDARG;
	}

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	if( !SUCCEEDED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  !SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor ) )
	||  !SUCCEEDED ( m_pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIEngine ) ) )
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

	pIComponent->Release();

	// Register clipboard formats
	if( RegisterClipboardFormats() == FALSE )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_REGISTER_CF, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register the clipboard format for an .cdp file 
	CString strExt = _T(".cdp");
	BSTR bstrExt = strExt.AllocSysString();
	if( !SUCCEEDED ( pIFramework->RegisterClipFormatForFile(m_cfPersonality, bstrExt) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_CLIPFORMAT, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}


	
	m_pIDocType8 = new CPersonalityDocType( this );
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

	if( !SUCCEEDED ( AddNodeImageLists() ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_IMAGELIST, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent IDMUSProdComponent::CleanUp

HRESULT CPersonalityComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPersonality *pPersonality;

	while( !m_lstPersonalities.IsEmpty() )
	{
		pPersonality = static_cast<CPersonality*>( m_lstPersonalities.RemoveHead() );
		pPersonality->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent IDMUSProdComponent::GetName

HRESULT CPersonalityComponent::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_PERSONALITY_COMPONENT_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent IDMUSProdComponent::OnActivate
HRESULT CPersonalityComponent::OnActivateApp(BOOL fActivate)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fActivate);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::AllocReferenceNode

HRESULT CPersonalityComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	// Make sure Component can create Nodes of type guidRefNodeId
	if( !( IsEqualGUID ( guidRefNodeId, GUID_PersonalityRefNode ) ) )
	{
		return E_INVALIDARG;
	}

	// Create PersonalityRefNode
	CPersonalityRef* pPersonalityRef = new CPersonalityRef( this );
	if( pPersonalityRef == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppIRefNode = (IDMUSProdNode *)pPersonalityRef;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent IDMUSProdRIFFExt::LoadRIFFChunk

HRESULT CPersonalityComponent::LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPersonality* pPersonality;
	HRESULT hr;

	ASSERT( pIStream != NULL );
	ASSERT( m_pIFramework != NULL );

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

	// Create a new Personality 
	pPersonality = new CPersonality( this );
	if( pPersonality == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	// Load the Personality file
	hr = pPersonality->Load( pIStream );
	if( !SUCCEEDED ( hr ) )
	{
		pPersonality->Release();
		return hr;
	}

	*ppINode = (IDMUSProdNode *)pPersonality;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent additional functions


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::AddNodeImageLists

HRESULT CPersonalityComponent::AddNodeImageLists( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, ILC_COLOR16, 6, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_PERSONALITY_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_PERSONALITY_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_PERSONALITYREF) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_PERSONALITYREF_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstPersonalityImage ) ) )
	{
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::GetPersonalityImageIndex

HRESULT CPersonalityComponent::GetPersonalityImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    *pnFirstImage = (short)(m_nFirstPersonalityImage + FIRST_PERSONALITY_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::GetFolderImageIndex

HRESULT CPersonalityComponent::GetFolderImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    *pnFirstImage = m_nFirstPersonalityImage;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::GetPersonalityRefImageIndex

HRESULT CPersonalityComponent::GetPersonalityRefImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnFirstImage = (short)(m_nFirstPersonalityImage + FIRST_PERSONALITYREF_IMAGE);
	
	return S_OK;
}


#define NBR_SCALES 10

/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::ScalePatternToScaleName

void CPersonalityComponent::ScalePatternToScaleName( long lScalePattern, CString& strScaleName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    static long scalepattern[NBR_SCALES] =
    { 
        0xAB5AB5,       // Major
        0x6AD6AD,       // Dorian
        0x5AB5AB,       // Phrygian
        0xAD5AD5,       // Lydian
        0x6B56B5,       // Mixolydian
        0x5AD5AD,       // Aeolian
        0xAADAAD,       // Mel Minor
        0x9AD9AD,       // H Minor
        0x555555,       // Whole Tone 
        0x56B56B        // Locrian
    };

    short i;
    
    for( i=0 ;  i<NBR_SCALES ;  i++ )
    {
        if( lScalePattern == scalepattern[i] )
        {
            strScaleName.LoadString( IDS_MAJOR + i );
            return;
        }
    }
     
    strScaleName.LoadString( IDS_UNKNOWN );
}

HRESULT STDMETHODCALLTYPE CPersonalityComponent::GetConductor(IDMUSProdConductor** ppI)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pIConductor);

	if(ppI == NULL)
	{
		return E_INVALIDARG;
	}


	*ppI = m_pIConductor;
	m_pIConductor->AddRef();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::RegisterClipboardFormats

BOOL CPersonalityComponent::RegisterClipboardFormats( void )
{
	m_cfProducerFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );
	m_cfStyle = ::RegisterClipboardFormat( CF_STYLE );
	m_cfPersonality = ::RegisterClipboardFormat (CF_PERSONALITY);

	if( m_cfProducerFile == 0
	||  m_cfStyle == 0
	||  m_cfPersonality == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::AddToPersonalityFileList

void CPersonalityComponent::AddToPersonalityFileList( CPersonality* pPersonality )
{
	if( pPersonality )
	{
		GUID guidPersonality;
		pPersonality->GetGUID( &guidPersonality );

		// Prevent duplicate object GUIDs
		GUID guidPersonalityList;
		POSITION pos = m_lstPersonalities.GetHeadPosition();
		while( pos )
		{
			CPersonality* pPersonalityList = m_lstPersonalities.GetNext( pos );

			pPersonalityList->GetGUID( &guidPersonalityList );
			if( ::IsEqualGUID( guidPersonalityList, guidPersonality ) )
			{
				::CoCreateGuid( &guidPersonality );
				pPersonality->SetGUID( guidPersonality );
				break;
			}
		}

		// Add to list
		pPersonality->AddRef();
		m_lstPersonalities.AddTail( pPersonality );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::RemoveFromPersonalityFileList

void CPersonalityComponent::RemoveFromPersonalityFileList( CPersonality* pPersonality )
{
	if( pPersonality )
	{
		// Remove from list
		POSITION pos = m_lstPersonalities.Find( pPersonality );
		if( pos )
		{
			m_lstPersonalities.RemoveAt( pos );
			pPersonality->Release();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityComponent::DetermineRoot

char CPersonalityComponent::DetermineRoot( DWORD dwScale )
{
	BYTE bRoot = 0;

    switch( dwScale & 0xfff )
    {
		case 0x56b: bRoot += 1; break;  // C#
		case 0xad6: bRoot += 2; break;  // D
		case 0x5ad: bRoot += 3; break;  // D#

		case 0xb5a: bRoot += 4; break;  // E
		case 0x6b5: bRoot += 5; break;  // F
		case 0xd6a: bRoot += 6; break;  // F#
		case 0xad5: bRoot += 7; break;  // G

		case 0x5ab: bRoot += 8; break;  // G#
		case 0xb56: bRoot += 9; break;  // A
		case 0x6ad: bRoot += 10; break; // A#
		case 0xd5a: bRoot += 11; break; // B
    }

    return (char)((bRoot %= 12) + 12);
}