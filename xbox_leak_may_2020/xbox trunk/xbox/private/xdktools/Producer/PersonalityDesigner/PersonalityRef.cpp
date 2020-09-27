// PersonalityRef.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "PersonalityDesigner.h"

#include "Personality.h"
#include "StyleRiffId.h "
#pragma warning(default:4201)

/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef constructor/destructor

CPersonalityRef::CPersonalityRef( CPersonalityComponent* pComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pComponent != NULL );

    m_dwRef = 0;
	AddRef();

	m_pPersonalityComponent = pComponent;
	m_pPersonalityComponent->AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pPersonality = NULL;
}

CPersonalityRef::~CPersonalityRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonalityComponent )
	{
		m_pPersonalityComponent->Release();
	}

	if( m_pPersonality )
	{
		m_pPersonality->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef::SetModifiedFlag

void CPersonalityRef::SetModifiedFlag( BOOL fNeedsSaved )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pPersonality != NULL );
	ASSERT( m_pPersonalityComponent->m_pIFramework != NULL );

	// Framework responsible for clearing its own modified flag.
	// Otherwise Project file containing multiple durty files
	// may be flagged as not dirty because one file is no 
	// longer dirty.
	if( fNeedsSaved )
	{
		m_pPersonality->Modified() = TRUE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IUnknown implementation

HRESULT CPersonalityRef::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CPersonalityRef::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CPersonalityRef::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp(); 
    --m_dwRef;

	TRACE( "CPersonalityRef: %ld\n", m_dwRef );

    if( m_dwRef == 0 )
    {
		TRACE( "CPersonalityRef destroying...\n" );
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetNodeImageIndex

HRESULT CPersonalityRef::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pPersonalityComponent != NULL );

	return( m_pPersonalityComponent->GetPersonalityRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetFirstChild

HRESULT CPersonalityRef::GetFirstChild( IDMUSProdNode** /*ppIFirstChildNode*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;		// PersonalityRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetNextChild

HRESULT CPersonalityRef::GetNextChild( IDMUSProdNode* /*pIChildNode*/, IDMUSProdNode** /*ppINextChildNode*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// PersonalityRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetComponent

HRESULT CPersonalityRef::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pPersonalityComponent != NULL );

	return m_pPersonalityComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetDocRootNode

HRESULT CPersonalityRef::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CPersonalityRef IDMUSProdNode::SetDocRootNode

HRESULT CPersonalityRef::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetParentNode

HRESULT CPersonalityRef::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::SetParentNode

HRESULT CPersonalityRef::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetNodeId

HRESULT CPersonalityRef::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_PersonalityRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetNodeName

HRESULT CPersonalityRef::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality == NULL )
	{
		TCHAR achNoPersonality[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_PERSONALITY, achNoPersonality, MID_BUFFER );
		CString strNoPersonality = achNoPersonality;
		*pbstrName = strNoPersonality.AllocSysString();
		return S_OK;
	}

    return m_pPersonality->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetNodeNameMaxLength

HRESULT CPersonalityRef::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a PersonalityRef node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::ValidateNodeName

HRESULT CPersonalityRef::ValidateNodeName( BSTR /*bstrName*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a PersonalityRef node
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::SetNodeName

HRESULT CPersonalityRef::SetNodeName( BSTR /*bstrName*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a PersonalityRef node
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetNodeListInfo

HRESULT CPersonalityRef::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality == NULL )
	{
		return E_FAIL;
	}

    return m_pPersonality->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetEditorClsId

HRESULT CPersonalityRef::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality == NULL )
	{
		return E_FAIL;
	}

    *pClsId = CLSID_PersonalityEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetEditorTitle

HRESULT CPersonalityRef::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality == NULL )
	{
		return E_FAIL;
	}

    return m_pPersonality->GetEditorTitle( pbstrTitle );
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetEditorWindow

HRESULT CPersonalityRef::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality == NULL )
	{
		return E_FAIL;
	}

    return m_pPersonality->GetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::SetEditorWindow

HRESULT CPersonalityRef::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPersonality == NULL )
	{
		return E_FAIL;
	}

    return m_pPersonality->SetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::UseOpenCloseImages

HRESULT CPersonalityRef::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetRightClickMenuId

HRESULT CPersonalityRef::GetRightClickMenuId( HINSTANCE* /*phInstance*/, UINT* /*pnMenuId*/ )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
/*
	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_PERSONALITYREF_NODE_RMENU;
*/
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::OnRightClickMenuInit

HRESULT CPersonalityRef::OnRightClickMenuInit( HMENU /*hMenu*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::OnRightClickMenuSelect

HRESULT CPersonalityRef::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case 1 :
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::DeleteChildNode

HRESULT CPersonalityRef::DeleteChildNode( IDMUSProdNode* /*pIChildNode*/, BOOL /*fPromptUser*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// PersonalityRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::InsertChildNode

HRESULT CPersonalityRef::InsertChildNode( IDMUSProdNode* /*pIChildNode*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// PersonalityRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::DeleteNode

HRESULT CPersonalityRef::DeleteNode( BOOL /*fPromptUser*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_FAIL;		// Cannot delete PersonalityRef node?????
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::OnNodeSelChanged

HRESULT CPersonalityRef::OnNodeSelChanged( BOOL /*fSelected*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
// Drag/Drop methods
/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::CreateDataObject

HRESULT CPersonalityRef::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pPersonality )
	{
		// Let Personality create data object
		return m_pPersonality->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::CanCut

HRESULT CPersonalityRef::CanCut( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::CanCopy

HRESULT CPersonalityRef::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if(m_pPersonality)
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::CanDelete

HRESULT CPersonalityRef::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pIParentNode != NULL);
	return m_pIParentNode->CanDeleteChildNode( this );

}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::CanDeleteChildNode

HRESULT CPersonalityRef::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	
}



/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::CanPasteFromData

HRESULT CPersonalityRef::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a Personality
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_pPersonalityComponent->m_cfPersonality );

	pDataObject->Release();

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent decide whether it wants to paste a Personality
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pPersonality )
	{
		// Let Personality decide what can be dropped
		return m_pPersonality->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::PasteFromData

HRESULT CPersonalityRef::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a Personality
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_pPersonalityComponent->m_cfPersonality );

	pDataObject->Release();

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle pasting of Personality
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pPersonality )
	{
		// Let Personality handle paste
		return m_pPersonality->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::CanChildPasteFromData

HRESULT CPersonalityRef::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
											    BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::ChildPasteFromData

HRESULT CPersonalityRef::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;
}





/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdReferenceNode::GetReferencedFile

HRESULT CPersonalityRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pPersonality )
	{
		m_pPersonality->AddRef();
		*ppIDocRootNode = m_pPersonality;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CPersonalityRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
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
			if( pINode )
			{
				pINode->Release();
			}
			return E_INVALIDARG;
		}
		pINode->Release();

		// Make sure method was passed a Personality Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_PersonalityNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	if( m_pPersonality )
	{
		m_pPersonality->Release();
		m_pPersonality = NULL;
	}

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pPersonality = (CPersonality *)pIDocRootNode;
		m_pPersonality->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityRef IDMUSProdNode::GetObject

HRESULT CPersonalityRef::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Must be asking for an interface to a CLSID_DMStyle object
	if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicChordMap ) )
	{
		if( m_pPersonality->m_pIDirectMusicChordMap )
		{
			return m_pPersonality->m_pIDirectMusicChordMap->QueryInterface( riid, ppvObject );
		}
	}

    *ppvObject = NULL;
    return E_NOINTERFACE;
}


