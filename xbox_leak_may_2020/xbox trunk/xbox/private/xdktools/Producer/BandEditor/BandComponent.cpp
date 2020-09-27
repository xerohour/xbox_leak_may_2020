// BandComponent.cpp : implementation file
//

#include "stdafx.h"

#include "BandEditorDLL.h"
#include "Band.h"
#include "BandRef.h"
#include "DLSDesigner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBandComponent constructor/destructor

CBandComponent::CBandComponent()
{
    m_dwRef = 0;
	m_pIFramework = NULL;
	m_pIDocType8 = NULL;
	m_pIConductor = NULL;
	m_pIDMPerformance = NULL;
	m_nFirstBandImage = 0;
	m_nFirstBandRefImage = 0;
	m_nFirstFolderImage = 0;
	m_nNextBand = 0;		
	m_cfBand = 0;
	m_cfBandList = 0;
	m_pvLastFailedCollectionRefStream = NULL;
}

CBandComponent::~CBandComponent()
{
	ReleaseAll();
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent::ReleaseAll

void CBandComponent::ReleaseAll( void )
{
	CBand *pBand;

	while( !m_lstBands.IsEmpty() )
	{
		pBand = static_cast<CBand*>( m_lstBands.RemoveHead() );
		pBand->Release();
	}

	if( m_pIFramework )
	{
		m_pIFramework->Release();
		m_pIFramework = NULL;

		theApp.m_pIFramework->Release();
		theApp.m_pIFramework = NULL;
	}

	if( m_pIDocType8 )
	{
		m_pIDocType8->Release();
		m_pIDocType8 = NULL;
	}
	
	if (m_pIDMPerformance)
	{
		m_pIDMPerformance->Release();
		m_pIDMPerformance = NULL;
	}

	if( m_pIConductor )
	{
		m_pIConductor->Release();
		m_pIConductor = NULL;
	}

	while(m_lstLastCollection.IsEmpty() == FALSE)
	{
		m_lstLastCollection.RemoveHead();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent IUnknown implementation

HRESULT CBandComponent::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CBandComponent::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CBandComponent::Release()
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
// CBandComponent IDMUSProdComponent implementation

/////////////////////////////////////////////////////////////////////////////
// CBandComponent IDMUSProdComponent::Initialize

HRESULT CBandComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdComponent* pIComponent = NULL;
	TCHAR achErrMsg[MID_BUFFER];
	CString strErrMsg;

	if( m_pIFramework )		// already initialized
	{
		return S_OK;
	}

	// Validate arguments
	ASSERT(pIFramework);
	ASSERT(pbstrErrMsg);

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	theApp.m_pIFramework = pIFramework;
	theApp.m_pIFramework->AddRef();

	// Get IDMUSProdConductor and DMusic Performance interface pointers 
	if( FAILED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  FAILED ( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor ) )
	||  FAILED ( m_pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIDMPerformance ) ) )
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

	// Add applicable images to the Project Tree control's image list 
	if( !SUCCEEDED ( AddNodeImageLists() ) )
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

	// Register the clip format for a .bnp file 
	CString strExt = _T(".bnp");
	BSTR bstrExt = strExt.AllocSysString();
	if( !SUCCEEDED ( pIFramework->RegisterClipFormatForFile(m_cfBand, bstrExt) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_CLIPFORMAT, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register applicable doc types with the Framework 
	m_pIDocType8 = new CBandDocType( this );
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

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent IDMUSProdComponent::CleanUp

HRESULT CBandComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CBand *pBand;

	while( !m_lstBands.IsEmpty() )
	{
		pBand = static_cast<CBand*>( m_lstBands.RemoveHead() );
		pBand->Release();
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent IDMUSProdComponent::GetName

HRESULT CBandComponent::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_BAND_COMPONENT_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent IDMUSProdComponent::AllocReferenceNode

HRESULT CBandComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	// Make sure Component can create Nodes of type guidRefNodeId
	if( !( IsEqualGUID ( guidRefNodeId, GUID_BandRefNode ) ) )
	{
		return E_INVALIDARG;
	}

	// Create BandRefNode
	CBandRef* pBandRef = new CBandRef( this );
	if( pBandRef == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppIRefNode = (IDMUSProdNode *)pBandRef;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent IDMUSProdComponent::OnActivateApp

HRESULT CBandComponent::OnActivateApp( BOOL fActivate )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent IDMUSProdRIFFExt::LoadRIFFChunk

HRESULT CBandComponent::LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CBand* pBand;
	HRESULT hr;

	ASSERT( pIStream != NULL );
	ASSERT( m_pIFramework != NULL );

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

	// Create a new Band 
	pBand = new CBand( this );
	if( pBand == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	// Load the Band file
	//m_pIFramework->StartWaitCursor();
	hr = pBand->Load( pIStream );
	//m_pIFramework->EndWaitCursor();
	if( !SUCCEEDED ( hr ) )
	{
		pBand->Release();
		return hr;
	}

	*ppINode = (IDMUSProdNode *)pBand;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent additional functions

/////////////////////////////////////////////////////////////////////////////
// CBandComponent::AddNodeImageLists

HRESULT CBandComponent::AddNodeImageLists( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_BAND_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_BAND_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstBandImage ) ) )
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
	
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_BANDREF_NODE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_BANDREF_NODE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( !SUCCEEDED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstBandRefImage ) ) )
	{
		return E_FAIL;
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent::RegisterClipboardFormats

BOOL CBandComponent::RegisterClipboardFormats( void )
{
	m_cfBand = ::RegisterClipboardFormat( CF_BAND );
	m_cfBandList = ::RegisterClipboardFormat( CF_BANDLIST );
	m_cfProducerFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );

	if( m_cfBand == 0
	||  m_cfBandList == 0
	||  m_cfProducerFile == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent::GetBandImageIndex

HRESULT CBandComponent::GetBandImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    *pnFirstImage = m_nFirstBandImage;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent::GetFolderImageIndex

HRESULT CBandComponent::GetFolderImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    *pnFirstImage = m_nFirstFolderImage;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent::GetBandRefImageIndex

HRESULT CBandComponent::GetBandRefImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    *pnFirstImage = m_nFirstBandRefImage;
	
	return S_OK;
}


bool CBandComponent::IsGM(DWORD dwBank, DWORD dwInstrument)
{
	BYTE	bMSB, bLSB, bPatch;

	bMSB = (BYTE) ((dwBank & 0x00007F00) >> 8);
	bLSB = (BYTE) (dwBank & 0x0000007F);
	bPatch = (BYTE) (dwInstrument & 0x0000007F);
	
	if (bLSB != 0) return false;

	if ( (bMSB == 0) ) // && (bLSB == 0) 
	{
		if ((dwBank & 0x80000000)) //Drum Kit
		{
			if ((bPatch == 0x0)  ||
				(bPatch == 0x08) ||
				(bPatch == 0x10) ||
				(bPatch == 0x18) ||
				(bPatch == 0x19) ||
				(bPatch == 0x20) ||
				(bPatch == 0x28) ||
				(bPatch == 0x30) || 
				(bPatch == 0x38) )
			{
				return  true;
			}
			else
				return false;
		}
		else return true;//is GM
	}
	// check for GS
	switch (bMSB)
	{
		case 6:
		case 7:
			if (bPatch == 0x7D) return true;
			break;
		case 24:
			if ((bPatch == 0x04) || (bPatch == 0x06)) return true;
			break;
		case 9:
			if ((bPatch == 0x0E) || (bPatch == 0x76) || (bPatch == 0x7D)) return true;
			break;
		case 2:
			if ( (bPatch == 0x66) || (bPatch == 0x78) || ((bPatch > 0x79)&&(bPatch < 0x80) )) return true;
			break;
		case 3:
			if ((bPatch > 0x79) && (bPatch < 0x80)) return true;
			break;
		case 4:
		case 5:
			if ( (bPatch == 0x7A) || ((bPatch > 0x7B)&&(bPatch < 0x7F) )) return true;
			break;
		case 32:
			if ((bPatch == 0x10) ||
				(bPatch == 0x11) ||
				(bPatch == 0x18) ||
				(bPatch == 0x34) ) return true;
			break;
		case 1:
			if ((bPatch == 0x26) ||
				(bPatch == 0x39) ||
				(bPatch == 0x3C) ||
				(bPatch == 0x50) ||
				(bPatch == 0x51) ||
				(bPatch == 0x62) ||
				(bPatch == 0x66) ||
				(bPatch == 0x68) ||
				((bPatch > 0x77) && (bPatch < 0x80))) return true;
				break;
		case 16:
			switch (bPatch)
			{
				case 0x00:
					return true;
					break;
				case 0x04:
					return true;
					break;
				case 0x05:
					return true;
					break;
				case 0x06:
					return true;
					break;
				case 0x10:
					return true;
					break;
				case 0x13:
					return true;
					break;
				case 0x18:
					return true;
					break;
				case 0x19:
					return true;
					break;
				case 0x1C:
					return true;
					break;
				case 0x27:
					return true;
					break;
				case 0x3E:
					return true;
					break;
				case 0x3F:
					return true;
					break;
				default:
					return false;
			}
			break;
		case 8:
			if ((bPatch < 0x07) || ((bPatch == 0x7D)))
			{
				return true;
			}
			else if ((bPatch > 0x3F) && (bPatch < 0x50))
			{
				return false;
			}
			else if ((bPatch > 0x4F) && (bPatch < 0x72)  )
			{
				if ((bPatch == 0x50) || 
					(bPatch == 0x51) ||
					(bPatch == 0x6B))
				{
					return true;
				}
				return false;
			}
			else if ((bPatch > 0x1F) && (bPatch < 0x40))
			{
				if ((bPatch > 0x25) && (bPatch < 0x29) ||
					(bPatch > 0x3C)  ||
					(bPatch == 0x30) || 
					(bPatch == 0x32) )
				{
					return true;
				}
				return false;
			}
			else if ((bPatch > 0x0A) && (bPatch < 0x12) && 
				     (bPatch != 0x0D) && (bPatch != 0x0F))
			{
				return true;
			}
			else if ((bPatch > 0x0F) && (bPatch < 0x20))
			{
				if (bPatch > 0x17)
				{
					return true;
				}
				else if ( (bPatch == 0x13) || (bPatch == 0x15) )
					return true;
				else
					return false;
			}
			else if(bPatch >= 0x73 && bPatch <= 0x77)
				return true;
			break;
		default:
			return false;
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent::FindDLSCollection

IDMUSProdReferenceNode* CBandComponent::FindDLSCollection( CString  csCollection, IStream* pIStream )
{
	IDMUSProdReferenceNode*	pIReferenceNode = NULL;
	IDMUSProdNode*			pIDocRootNode = NULL;
	IDMUSProdDocType*		pIDocType = NULL;
	IDMUSProdPersistInfo*	pIPersistInfo = NULL;
	DMUSProdStreamInfo		StreamInfo;
	HRESULT					hr;

	ASSERT( m_pIFramework != NULL );

	// Get DocType for DLS Collections
	hr = m_pIFramework->FindDocTypeByNodeId( GUID_CollectionNode, &pIDocType );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Get IDMUSProdPersistInfo interface for pIStream 
	hr = pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pIPersistInfo );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Get target directory
	pIPersistInfo->GetStreamInfo( &StreamInfo );

	if( !csCollection.IsEmpty() )
	{
		BSTR bstrCollection = csCollection.AllocSysString();

		if( FAILED ( m_pIFramework->GetBestGuessDocRootNode( pIDocType,
															 bstrCollection,
															 StreamInfo.pITargetDirectoryNode,
															 &pIDocRootNode ) ) )
		{
			pIDocRootNode = NULL;
		}
	}

	if( pIDocRootNode == NULL)
	{
		// Cannot find Collection
		// If user cancelled previous search for this Collection, no need to ask again
		if( IsInFailedCollectionList(csCollection) && m_pvLastFailedCollectionRefStream == pIStream)
		{
			hr = E_FAIL;
			goto ON_ERROR; 
		}
	
		if(m_pvLastFailedCollectionRefStream != (void*)pIStream)
		{
			while(m_lstLastCollection.IsEmpty() == FALSE)
			{
				m_lstLastCollection.RemoveHead();
			}

			m_pvLastFailedCollectionRefStream = (void*)pIStream;
		}

		// Determine File Open dialog prompt
		CString	csOpenDlgTitle;
		if( csCollection.IsEmpty() )
		{
			csOpenDlgTitle.LoadString( IDS_FILE_OPEN_ANY_DLS );
		}
		else
		{
			AfxFormatString1( csOpenDlgTitle, IDS_FILE_OPEN_DLS, csCollection );
		}
		BSTR bstrOpenDlgTitle = csOpenDlgTitle.AllocSysString();

		// Display File open dialog
		if( m_pIFramework->OpenFile(pIDocType, bstrOpenDlgTitle, StreamInfo.pITargetDirectoryNode, &pIDocRootNode) != S_OK )
		{
			// Did not open a file, or opened file other than DLS file
			// so we do not want this DocRoot
			RELEASE( pIDocRootNode );
		}
	}

	if( pIDocRootNode )
	{
		pIReferenceNode = CreateCollectionRefNode( pIDocRootNode );
	}
	else
	{
		m_lstLastCollection.AddTail(csCollection);
	}

ON_ERROR:
	RELEASE( pIPersistInfo );
	RELEASE( pIDocType );
	RELEASE( pIDocRootNode );

	return pIReferenceNode;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent::CreateCollectionRefNode

IDMUSProdReferenceNode* CBandComponent::CreateCollectionRefNode( IDMUSProdNode* pDLSCollectionNode )
{
	HRESULT						hr;
	IDMUSProdNode*				pINode;
	IDMUSProdComponent*			pIComponent;
	IDMUSProdReferenceNode*		pIRefNode;
	bool						bCollectionFound=false;

	// Allocate a new reference node
	hr = m_pIFramework->FindComponent( CLSID_DLSComponent,  &pIComponent );
	if(FAILED(hr))
	{
		return NULL;
	}

	hr = pIComponent->AllocReferenceNode(GUID_CollectionRefNode, &pINode);
	pIComponent->Release();
	if(FAILED(hr))
	{
		return NULL;
	}
	hr = pINode->QueryInterface(IID_IDMUSProdReferenceNode, (void**)&pIRefNode);
	pINode->Release();
	if (FAILED(hr))
	{
		TRACE("Failed query for Reference interface\n");
		return NULL;
	}
	
	hr = pIRefNode->SetReferencedFile(pDLSCollectionNode);
	if (FAILED(hr))
	{
		TRACE("Failed to SetReferencedFile\n");
		pIRefNode->Release();
		return NULL;
	}
	return pIRefNode;
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent::AddToBandFileList

void CBandComponent::AddToBandFileList( CBand* pBand )
{
	if( pBand )
	{
		GUID guidBand;
		pBand->GetGUID( &guidBand );

		// Prevent duplicate object GUIDs
		GUID guidBandList;
		POSITION pos = m_lstBands.GetHeadPosition();
		while( pos )
		{
			CBand* pBandList = m_lstBands.GetNext( pos );

			pBandList->GetGUID( &guidBandList );
			if( ::IsEqualGUID( guidBandList, guidBand ) )
			{
				::CoCreateGuid( &guidBand );
				pBand->SetGUID( guidBand );
				break;
			}
		}

		// Add to list
		pBand->AddRef();
		m_lstBands.AddTail( pBand );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBandComponent::RemoveFromBandFileList

void CBandComponent::RemoveFromBandFileList( CBand* pBand )
{
	if( pBand )
	{
		// Remove from list
		POSITION pos = m_lstBands.Find( pBand );
		if( pos )
		{
			m_lstBands.RemoveAt( pos );
			pBand->Release();
		}
	}
}



HRESULT CBandComponent::SendPMsg( DMUS_PMSG *pPMsg, IDirectMusicAudioPath* pDMAudioPath )
{
	if(!m_pIDMPerformance )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	IDirectMusicPerformance8 *pDMPerf8;
	if( SUCCEEDED( m_pIDMPerformance->QueryInterface( IID_IDirectMusicPerformance8, (void **)&pDMPerf8 ) ) )
	{
		bool fRelease = false;
		if( pDMAudioPath == NULL )
		{
			pDMPerf8->GetDefaultAudioPath( &pDMAudioPath );
			fRelease = true;
		}

		if( pDMAudioPath )
		{
			if( SUCCEEDED( pDMAudioPath->ConvertPChannel( pPMsg->dwPChannel, &pPMsg->dwPChannel ) ) )
			{
				hr = pDMPerf8->SendPMsg( pPMsg );
			}
			if( fRelease )
			{
				pDMAudioPath->Release();
			}
		}
		pDMPerf8->Release();
	}

	return hr;
}

BOOL CBandComponent::IsInFailedCollectionList(CString csCollection)
{
	POSITION position = m_lstLastCollection.GetHeadPosition();
	while(position)
	{
		CString csCollectionName = m_lstLastCollection.GetNext(position);
		if(csCollectionName.CompareNoCase(csCollection) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}
