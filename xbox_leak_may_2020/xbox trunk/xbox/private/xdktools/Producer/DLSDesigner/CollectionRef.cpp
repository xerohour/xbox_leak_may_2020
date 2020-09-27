///////////////////////////////////////////////////////////////////////////////////////////
//
//	CollectionRef.cpp
//	DLS Collection Reference node implementation
//	Created 3/17/98 Eric Whitcombe
//
///////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CollectionRef.h"
#include "Instrument.h"
#include "Region.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef constructor/destructor

CCollectionRef::CCollectionRef(CDLSComponent* pComponent)
{
    m_dwRef = 0;
	AddRef();

	m_pComponent = pComponent;
	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pCollection = NULL;
}

CCollectionRef::~CCollectionRef()
{
	RELEASE( m_pCollection );
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IUnknown implementation

HRESULT CCollectionRef::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdReferenceNode) )
    {
        AddRef();
        *ppvObj = (IDMUSProdReferenceNode *)this;
        return S_OK;
    }
    if( ::IsEqualIID(riid, IID_IDLSQueryInstruments) )
    {
        AddRef();
        *ppvObj = (IDLSQueryInstruments *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CCollectionRef::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CCollectionRef::Release()
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
// CCollectionRef IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetNodeImageIndex

HRESULT CCollectionRef::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent);

	return( m_pComponent->GetCollectionRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetFirstChild

HRESULT CCollectionRef::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;	// CollectionRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetNextChild

HRESULT CCollectionRef::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;	// CollectionRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetComponent

HRESULT CCollectionRef::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent );//IsBadReadPtr?

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetDocRootNode

HRESULT CCollectionRef::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CCollectionRef IDMUSProdNode::SetDocRootNode

HRESULT CCollectionRef::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetParentNode

HRESULT CCollectionRef::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::SetParentNode

HRESULT CCollectionRef::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetNodeId

HRESULT CCollectionRef::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_CollectionRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetNodeName

HRESULT CCollectionRef::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pCollection == NULL )
	{
		TCHAR achNoStyle[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_COLLECTION, achNoStyle, MID_BUFFER );
		CString strNoStyle = achNoStyle;
		*pbstrName = strNoStyle.AllocSysString();
		return S_OK;
	}

    return m_pCollection->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetNodeNameMaxLength

HRESULT CCollectionRef::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a CollectionRef node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::ValidateNodeName

HRESULT CCollectionRef::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a CollectionRef node
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::SetNodeName

HRESULT CCollectionRef::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a CollectionRef node
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetNodeListInfo

HRESULT CCollectionRef::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pCollection == NULL )
	{
		return E_FAIL;
	}

    return m_pCollection->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetEditorClsId

HRESULT CCollectionRef::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetEditorTitle

HRESULT CCollectionRef::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetEditorWindow

HRESULT CCollectionRef::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::SetEditorWindow

HRESULT CCollectionRef::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::UseOpenCloseImages

HRESULT CCollectionRef::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetRightClickMenuId

HRESULT CCollectionRef::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
	//I'm not sure what behavior we want a collection reference node to have
	/*
	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_CollectionRef_NODE_RMENU;

	return S_OK;
	*/
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::OnRightClickMenuInit

HRESULT CCollectionRef::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::OnRightClickMenuSelect

HRESULT CCollectionRef::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_PROPERTIES:
			if( m_pCollection )
			{
				return m_pCollection->OnRightClickMenuSelect( lCommandId );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::DeleteChildNode

HRESULT CCollectionRef::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// CollectionRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::InsertChildNode

HRESULT CCollectionRef::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// CollectionRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::DeleteNode

HRESULT CCollectionRef::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_FAIL;		// Cannot delete CollectionRef node?????
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::OnNodeSelChanged

HRESULT CCollectionRef::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::CreateDataObject

HRESULT CCollectionRef::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pCollection )
	{
		// Let Collection create data object
		return m_pCollection->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::CanCut

HRESULT CCollectionRef::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::CanCopy

HRESULT CCollectionRef::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_pCollection )
	{
		return S_OK;	// Will copy the Collection
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::CanDelete

HRESULT CCollectionRef::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::CanDeleteChildNode

HRESULT CCollectionRef::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;	// Style Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::CanPasteFromData

HRESULT CCollectionRef::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a Style
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfWave );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent decide whether it wants to paste a Style
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pCollection )
	{
		// Let Style decide what can be dropped
		return m_pCollection->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::PasteFromData

HRESULT CCollectionRef::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a Style
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfWave );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle pasting of Style
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pCollection )
	{
		// Let Style handle paste
		return m_pCollection->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::CanChildPasteFromData

HRESULT CCollectionRef::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;			// Style Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::ChildPasteFromData

HRESULT CCollectionRef::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;			// Style Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdNode::GetObject

HRESULT CCollectionRef::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CCollectionRef IDMUSProdReferenceNode::GetReferencedFile

HRESULT CCollectionRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pCollection )
	{
		m_pCollection->AddRef();
		*ppIDocRootNode = m_pCollection;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionRef::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CCollectionRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDocRootNode )
	{
		// Make sure method was passed a DocRootNode
		IDMUSProdNode* pINode;
		if( FAILED ( pIDocRootNode->GetDocRootNode ( &pINode ) ) )
		{
			pINode = NULL;
		}
		if( pIDocRootNode != pINode )
		{
			RELEASE( pINode );
			return E_INVALIDARG;
		}
		RELEASE( pINode );

		// Make sure method was passed a Collection Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_CollectionNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	RELEASE( m_pCollection );

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pCollection = (CCollection *)pIDocRootNode;
		m_pCollection->AddRef();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IDLSQueryInstruments interface routines.


HRESULT CCollectionRef::ResetInstrumentList()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_dwInstrumentIndex = 0;
	return S_OK;
}

BOOL CCollectionRef::GetNextInstrument(BYTE *pbMSB, 
									  BYTE *pbLSB, 
									  BYTE *pbPC,
									  BOOL *pfDrums, 
									  signed char *pszName, 
									  DWORD dwMaxLen )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CInstrument *pInstrument = (CInstrument *) m_pCollection->m_Instruments.GetItem(m_dwInstrumentIndex);
	if (pInstrument != NULL)
	{
		m_dwInstrumentIndex++;
		*pbMSB = (BYTE) ((pInstrument->m_rInstHeader.Locale.ulBank >> 8) & 0x7F);
		*pbLSB = (BYTE) (pInstrument->m_rInstHeader.Locale.ulBank & 0x7F);
		*pfDrums = ((pInstrument->m_rInstHeader.Locale.ulBank & F_INSTRUMENT_DRUMS) != 0);
		*pbPC = (BYTE) pInstrument->m_rInstHeader.Locale.ulInstrument;
		strncpy((char *) pszName,pInstrument->m_Info.m_csName,dwMaxLen );
		return TRUE;
	}
	return FALSE;
}

BOOL CCollectionRef::GetInstrumentName(  BYTE bMSB, 
									  BYTE bLSB, 
									  BYTE bPC, 
									  BOOL fDrums, 
									  signed char *pszName, 
									  DWORD dwMaxLen )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BYTE bIMSB, bILSB, bIPC;
	BOOL fIDrums;

	CInstrument *pInstrument = m_pCollection->m_Instruments.GetHead();
	for (;pInstrument;pInstrument = pInstrument->GetNext())
	{
		bIMSB = (BYTE) ((pInstrument->m_rInstHeader.Locale.ulBank >> 8) & 0x7F);
		bILSB = (BYTE) (pInstrument->m_rInstHeader.Locale.ulBank & 0x7F);
		fIDrums = ((pInstrument->m_rInstHeader.Locale.ulBank & F_INSTRUMENT_DRUMS) != 0);
		bIPC = (BYTE) pInstrument->m_rInstHeader.Locale.ulInstrument;
		if ((bMSB == bIMSB) && (bLSB == bILSB) && (bPC == bIPC) && (fDrums == fIDrums))
		{
			strncpy((char *)pszName,pInstrument->m_Info.m_csName,dwMaxLen );
			return TRUE;
		}
	}
	return FALSE;
}

HRESULT	CCollectionRef::GetObjectDescriptor(DWORD dwDescSize,
											void *pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( dwDescSize < sizeof(DMUS_OBJECTDESC) )
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	}

	DMUS_OBJECTDESC *pDMObjectDesc = (DMUS_OBJECTDESC *)pObjectDesc;

	memset( pDMObjectDesc, 0, sizeof(DMUS_OBJECTDESC) );
	pDMObjectDesc->dwSize = sizeof(DMUS_OBJECTDESC);
	pDMObjectDesc->dwValidData = (DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS | DMUS_OBJ_VERSION | DMUS_OBJ_NAME );
	memcpy( &pDMObjectDesc->guidObject, &m_pCollection->m_Guid, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicCollection, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = m_pCollection->m_rVersion.dwVersionMS;
	pDMObjectDesc->vVersion.dwVersionLS = m_pCollection->m_rVersion.dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, m_pCollection->m_Info.m_csName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );
	//MultiByteToWideChar( CP_ACP, 0, m_pCollection->m_Info.m_csName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );
	//MultiByteToWideChar( CP_ACP, 0, m_pCollection->m_Info.m_csName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}

HRESULT CCollectionRef::GetRegionWaveName(  BYTE bMSB, 
											BYTE bLSB, 
											BYTE bPC, 
											BOOL fDrums, 
											BYTE bMIDINote, 
											BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

	BYTE bIMSB, bILSB, bIPC;
	BOOL fIDrums;

	CInstrument *pInstrument = m_pCollection->m_Instruments.GetHead();

	for( ;  pInstrument ;  pInstrument = pInstrument->GetNext() )
	{
		bIMSB = (BYTE) ((pInstrument->m_rInstHeader.Locale.ulBank >> 8) & 0x7F);
		bILSB = (BYTE) (pInstrument->m_rInstHeader.Locale.ulBank & 0x7F);
		fIDrums = ((pInstrument->m_rInstHeader.Locale.ulBank & F_INSTRUMENT_DRUMS) != 0);
		bIPC = (BYTE) pInstrument->m_rInstHeader.Locale.ulInstrument;
		
		if(bMSB == bIMSB && bLSB == bILSB && bPC == bIPC && fDrums == fIDrums)
		{
			CInstrumentRegions* pRegions = pInstrument->GetRegions();
			if(pRegions)
			{
				short nNumberOfLayers = short(pRegions->GetNumberOfLayers());
				CPtrList lstRegions;
				if(SUCCEEDED(pRegions->FindRegionsForNote(bMIDINote, &lstRegions)))
				{
					CString strWaveNames;
					for(int nLayer = 0; nLayer < nNumberOfLayers; nLayer++)
					{
						POSITION position = lstRegions.GetHeadPosition();
						while(position)
						{
							CRegion* pRegion = (CRegion*)lstRegions.GetNext(position);
							if(pRegion && pRegion->GetLayer() == nLayer)
							{
								CString strNextWaveName = pRegion->GetWaveName();
								if (!strNextWaveName.IsEmpty())
								{
									if(strWaveNames.IsEmpty() == FALSE)
									{
										strWaveNames += ",";
									}

									strWaveNames += strNextWaveName;
								}
							}
						}
					}

					*pbstrName = strWaveNames.AllocSysString();
					return S_OK;
				}
			}

			return E_FAIL;
		}
	}

	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionRef Additional functions
