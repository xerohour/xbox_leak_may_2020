// WaveRefNode.cpp : implementation file
//

#include "stdafx.h"
#include "DLSDesignerDLL.h"

#include "WaveNode.h"
#include "WaveRefNode.h"
#include "DllJazzDataObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode constructor/destructor

CWaveRefNode::CWaveRefNode( CDLSComponent* pComponent )
{
	ASSERT( pComponent != NULL );

	m_pComponent = pComponent;
	m_pComponent->AddRef();

    m_dwRef = 0;
	AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pWaveNode = NULL;
}

CWaveRefNode::~CWaveRefNode()
{
	RELEASE( m_pWaveNode );
	RELEASE( m_pComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IUnknown implementation

HRESULT CWaveRefNode::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CWaveRefNode::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CWaveRefNode::Release()
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
// CWaveRefNode IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetNodeImageIndex

HRESULT CWaveRefNode::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	if(m_pWaveNode && m_pWaveNode->IsStereo())
	{
		return( m_pComponent->GetStereoWaveRefImageIndex(pnFirstImage) );
	}

	return( m_pComponent->GetWaveRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetFirstChild

HRESULT CWaveRefNode::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// WaveRefNode node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetNextChild

HRESULT CWaveRefNode::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// WaveRefNode node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetComponent

HRESULT CWaveRefNode::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetDocRootNode

HRESULT CWaveRefNode::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CWaveRefNode IDMUSProdNode::SetDocRootNode

HRESULT CWaveRefNode::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetParentNode

HRESULT CWaveRefNode::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::SetParentNode

HRESULT CWaveRefNode::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetNodeId

HRESULT CWaveRefNode::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_WaveRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetNodeName

HRESULT CWaveRefNode::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pWaveNode == NULL )
	{
		TCHAR achNoWave[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_WAVE, achNoWave, MID_BUFFER );
		CString strNoWave = achNoWave;
		*pbstrName = strNoWave.AllocSysString();
		return S_OK;
	}

    return m_pWaveNode->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetNodeNameMaxLength

HRESULT CWaveRefNode::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a WaveRefNode node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::ValidateNodeName

HRESULT CWaveRefNode::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a WaveRefNode node
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::SetNodeName

HRESULT CWaveRefNode::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a WaveRefNode node
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetNodeListInfo

HRESULT CWaveRefNode::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pWaveNode == NULL )
	{
		return E_FAIL;
	}

    return m_pWaveNode->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetEditorClsId

HRESULT CWaveRefNode::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pWaveNode == NULL )
	{
		return E_FAIL;
	}

    return m_pWaveNode->GetEditorClsId( pClsId );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetEditorTitle

HRESULT CWaveRefNode::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pWaveNode == NULL )
	{
		return E_FAIL;
	}

    return m_pWaveNode->GetEditorTitle( pbstrTitle );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetEditorWindow

HRESULT CWaveRefNode::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pWaveNode == NULL )
	{
		return E_FAIL;
	}

    return m_pWaveNode->GetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::SetEditorWindow

HRESULT CWaveRefNode::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pWaveNode == NULL )
	{
		return E_FAIL;
	}

    return m_pWaveNode->SetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::UseOpenCloseImages

HRESULT CWaveRefNode::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetRightClickMenuId

HRESULT CWaveRefNode::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_WAVEREF_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::OnRightClickMenuInit

HRESULT CWaveRefNode::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::OnRightClickMenuSelect

HRESULT CWaveRefNode::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_PROPERTIES:
			if( m_pWaveNode )
			{
				return m_pWaveNode->OnRightClickMenuSelect( lCommandId );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::DeleteChildNode

HRESULT CWaveRefNode::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// WaveRefNode nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::InsertChildNode

HRESULT CWaveRefNode::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// WaveRefNode nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::DeleteNode

HRESULT CWaveRefNode::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_FAIL;		// Cannot delete WaveRefNode node?????
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::OnNodeSelChanged

HRESULT CWaveRefNode::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::CreateDataObject

HRESULT CWaveRefNode::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pWaveNode )
	{
		// Let Wave create data object
		return m_pWaveNode->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::CanCut

HRESULT CWaveRefNode::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::CanCopy

HRESULT CWaveRefNode::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_pWaveNode )
	{
		return S_OK;	// Will copy the Wave
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::CanDelete

HRESULT CWaveRefNode::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::CanDeleteChildNode

HRESULT CWaveRefNode::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Wave Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::CanPasteFromData

HRESULT CWaveRefNode::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a Wave
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

		// Let parent decide whether it wants to paste a Wave
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pWaveNode )
	{
		// Let WaveNode decide what can be dropped
		return m_pWaveNode->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::PasteFromData

HRESULT CWaveRefNode::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a WaveNode
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

		// Let parent handle pasting of WaveNode
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pWaveNode )
	{
		// Let WaveNode handle paste
		return m_pWaveNode->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::CanChildPasteFromData

HRESULT CWaveRefNode::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// Wave Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::ChildPasteFromData

HRESULT CWaveRefNode::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// Wave Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdNode::GetObject

HRESULT CWaveRefNode::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode IDMUSProdReferenceNode::GetReferencedFile

HRESULT CWaveRefNode::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pWaveNode )
	{
		m_pWaveNode->AddRef();
		*ppIDocRootNode = m_pWaveNode;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CWaveRefNode::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
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

		// Make sure method was passed a Wave Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_WaveNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	RELEASE( m_pWaveNode );

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pWaveNode = (CWaveNode *)pIDocRootNode;
		m_pWaveNode->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveRefNode Additional functions
