// Folder.cpp : implementation file
//

#include "stdafx.h"

#include "ContainerDesignerDLL.h"
#include "Container.h"
#include "ContainerCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFolder constructor/destructor

CFolder::CFolder()
{
	m_pContainer = NULL;
	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
}

CFolder::~CFolder()
{
	CContainerObject* pContainerObject;

	while( !m_lstObjects.IsEmpty() )
	{
		pContainerObject = m_lstObjects.RemoveHead();
		RELEASE( pContainerObject );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IUnknown implementation

HRESULT CFolder::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
		if( m_pContainer )
		{
	        return m_pContainer->QueryInterface( riid, ppvObj );
		}

		*ppvObj = NULL;
		return E_NOINTERFACE;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CFolder::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pContainer != NULL );

	AfxOleLockApp(); 

	return m_pContainer->AddRef();
}

ULONG CFolder::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pContainer != NULL );

	AfxOleUnlockApp(); 

	return m_pContainer->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNodeImageIndex

HRESULT CFolder::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );


	if( m_pContainer->IsInSegment() 
	&&  ::IsEqualGUID( m_guidTypeNode, GUID_ContainerRefFolderNode ) ) 
	{
		return( theApp.m_pContainerComponent->GetFolderGrayImageIndex(pnFirstImage) );
	}

	return( theApp.m_pContainerComponent->GetFolderImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetFirstChild

HRESULT CFolder::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstChildNode = NULL;

	if( !m_lstObjects.IsEmpty() )
	{
		CContainerObject* pContainerObject = m_lstObjects.GetHead();
		
		if( pContainerObject )
		{
			pContainerObject->AddRef();
			*ppIFirstChildNode = (IDMUSProdNode *)pContainerObject;
		}
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNextChild

HRESULT CFolder::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
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

	CContainerObject* pContainerObject;

    POSITION pos = m_lstObjects.GetHeadPosition();

    while( pos )
    {
        pContainerObject = m_lstObjects.GetNext( pos );
		if( pIChildNode == (IDMUSProdNode *)pContainerObject )
		{
			if( pos )
			{
				pContainerObject = m_lstObjects.GetNext( pos );

				pContainerObject->AddRef();
				*ppINextChildNode = (IDMUSProdNode *)pContainerObject;
			}
			break;
		}
    }

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetComponent

HRESULT CFolder::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );

	return theApp.m_pContainerComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetDocRootNode

HRESULT CFolder::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CFolder IDMUSProdNode::SetDocRootNode

HRESULT CFolder::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetParentNode

HRESULT CFolder::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::SetParentNode

HRESULT CFolder::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNodeId

HRESULT CFolder::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = m_guidTypeNode;		// GUID_ContainerEmbedFolderNode
									// GUID_ContainerRefFolderNode

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNodeName

HRESULT CFolder::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNodeNameMaxLength

HRESULT CFolder::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a folder

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::ValidateNodeName

HRESULT CFolder::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::SetNodeName

HRESULT CFolder::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNodeListInfo

HRESULT CFolder::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
//	AFX_MANAGE_STATE( _afxModuleAddrThis );
	UNREFERENCED_PARAMETER(pListInfo);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetEditorClsId

HRESULT CFolder::GetEditorClsId( CLSID* pClsId )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pClsId);

	return E_NOTIMPL;	// Can't edit a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetEditorTitle

HRESULT CFolder::GetEditorTitle( BSTR* pbstrTitle )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pbstrTitle);

	return E_NOTIMPL;	// Can't edit a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetEditorWindow

HRESULT CFolder::GetEditorWindow( HWND* hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hWndEditor);

	return E_NOTIMPL;	// Can't edit a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::SetEditorWindow

HRESULT CFolder::SetEditorWindow( HWND hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hWndEditor);

	return E_NOTIMPL;	// Can't edit a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::UseOpenCloseImages

HRESULT CFolder::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetRightClickMenuId

HRESULT CFolder::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_FOLDER_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::OnRightClickMenuInit

HRESULT CFolder::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CMenu menu;

	if( menu.Attach(hMenu) )
	{
		IDMUSProdPropPageObject* pIPageObject;

		// IDM_PASTE
		{
			IDataObject* pIDataObject;
			BOOL fWillSetReference;

			menu.EnableMenuItem( IDM_PASTE, (MF_GRAYED | MF_BYCOMMAND) );

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					menu.EnableMenuItem( IDM_PASTE, (MF_ENABLED | MF_BYCOMMAND) );
				}
				
				pIDataObject->Release();
			}
		}

		// IDM_PROPERTIES
		if( SUCCEEDED ( QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
		{
			menu.EnableMenuItem( IDM_PROPERTIES, (MF_ENABLED | MF_BYCOMMAND) );
			RELEASE( pIPageObject );
		}
		else
		{
			menu.EnableMenuItem( IDM_PROPERTIES, (MF_GRAYED | MF_BYCOMMAND) );
		}

		menu.Detach();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::OnRightClickMenuSelect

HRESULT CFolder::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pContainer != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_ADD_FILES:
			m_pContainer->OnAddRemoveFiles();
			break;

		case IDM_PASTE:
		{
			IDataObject* pIDataObject;
			BOOL fWillSetReference;

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					PasteFromData( pIDataObject );
				}
				
				RELEASE( pIDataObject );
			}
			break;
		}

		case IDM_PROPERTIES:
		{
			IDMUSProdPropPageObject* pIPageObject;

			if( SUCCEEDED ( QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
			{
				pIPageObject->OnShowProperties();
				RELEASE( pIPageObject );
			}
			hr = S_OK;
			break;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::DeleteChildNode

HRESULT CFolder::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIChildNode != NULL );
	ASSERT( m_pContainer != NULL );
	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	CWaitCursor wait;

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Make sure we received a ContainerObject node
	GUID guidNodeId;
	HRESULT hr = pIChildNode->GetNodeId( &guidNodeId );
	if( SUCCEEDED ( hr ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_ContainerObjectNode ) == FALSE )
		{
			hr = E_FAIL;
		}
	}
	if( FAILED( hr ) )
	{
		return hr;
	}
	CContainerObject* pContainerObject = (CContainerObject *)pIChildNode;

	// Are you sure?
	if( fPromptUser )
	{
		CString strPrompt;

		if( m_pContainer->IsInScript() )
		{
			ASSERT( pContainerObject->m_FileRef.strScriptAlias.IsEmpty() == FALSE );
			AfxFormatString1( strPrompt, IDS_DELETE_ALIAS_PROMPT, pContainerObject->m_FileRef.strScriptAlias );
		}
		else
		{
			CString strObjectName;
			ASSERT( pContainerObject->m_FileRef.pIRefNode != NULL );
			if( pContainerObject->m_FileRef.pIRefNode == NULL )
			{
				TCHAR achNoObject[MID_BUFFER];
				::LoadString( theApp.m_hInstance, IDS_EMPTY_TEXT, achNoObject, MID_BUFFER );
				strObjectName = achNoObject;
			}
			else
			{
				BSTR bstrObjectName;
				if( SUCCEEDED ( pContainerObject->m_FileRef.pIRefNode->GetNodeName( &bstrObjectName ) ) )
				{
					strObjectName = bstrObjectName;
					::SysFreeString( bstrObjectName );
				}
			}
			AfxFormatString1( strPrompt, IDS_DELETE_NODE_PROMPT, strObjectName );
		}

		if( AfxMessageBox( strPrompt, MB_OKCANCEL ) != IDOK )
		{
			return E_FAIL;
		}
	}

	// Remove node from Project Tree
	if( theApp.m_pContainerComponent->m_pIFramework8->RemoveNode( pIChildNode, FALSE ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Save undo state
//	m_pContainer->m_pINodeBeingDeleted = pIChildNode;
//	m_pContainer->m_pUndoMgr->SaveState( m_pContainer, theApp.m_hInstance, IDS_UNDO_DELETE_BAND );
//	m_pContainer->m_pINodeBeingDeleted = NULL;

	// Need to keep node until after SyncContainerEditor() 
	pIChildNode->AddRef();

	// Remove from object list
	POSITION pos = m_lstObjects.Find( pContainerObject );
	if( pos )
	{
		m_lstObjects.RemoveAt( pos );
		pContainerObject->Release();
	}

	// Sync Container editor (when open)
	m_pContainer->SyncContainerEditor();
	RELEASE( pIChildNode );

	m_pContainer->SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::InsertChildNode

HRESULT CFolder::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIChildNode != NULL );
	ASSERT( m_pContainer != NULL );
	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	// Make sure we received a ContainerObject node
	GUID guidNodeId;
	HRESULT hr = pIChildNode->GetNodeId( &guidNodeId );
	if( SUCCEEDED ( hr ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_ContainerObjectNode ) == FALSE )
		{
			hr = E_FAIL;
		}
	}
	if( FAILED( hr ) )
	{
		return hr;
	}

	// We received the right type of node
	pIChildNode->AddRef();

	// Add to object list
	CContainerObject* pContainerObject = (CContainerObject *)pIChildNode;
	m_lstObjects.AddTail( pContainerObject );

	// Set root and parent node of ALL children
	ASSERT( m_pIDocRootNode != NULL );
	theApp.SetNodePointers( pIChildNode, m_pIDocRootNode, (IDMUSProdNode *)this );

	// Add node to Project Tree
	if( !SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->AddNode(pIChildNode, (IDMUSProdNode *)this) ) )
	{
		DeleteChildNode( pIChildNode, FALSE );
		return E_FAIL;
	}

	// Sync Container editor (when open)
	m_pContainer->SyncContainerEditor();

	m_pContainer->SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::DeleteNode

HRESULT CFolder::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// Can't delete a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::OnNodeSelChanged

HRESULT CFolder::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CreateDataObject

HRESULT CFolder::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanCut

HRESULT CFolder::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_FALSE;		// Can't remove folder from Container
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanCopy

HRESULT CFolder::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanDelete

HRESULT CFolder::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_FALSE;		// Can't delete folder from Container
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanDeleteChildNode

HRESULT CFolder::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure we received a ContainerObject node
	GUID guidNodeId;
	HRESULT hr = pIChildNode->GetNodeId( &guidNodeId );
	if( SUCCEEDED ( hr ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_ContainerObjectNode ) == FALSE )
		{
			hr = E_FAIL;
		}
	}
	if( FAILED( hr ) )
	{
		return hr;
	}

	// Make sure node is in object list
	CContainerObject* pContainerObject = (CContainerObject *)pIChildNode;
	POSITION pos = m_lstObjects.Find( pContainerObject );
	if( pos == NULL )
	{
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanPasteFromData

HRESULT CFolder::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pContainer != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = TRUE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pContainer->IsInSegment() )
	{
		if( ::IsEqualGUID( m_guidTypeNode, GUID_ContainerRefFolderNode ) ) 
		{
			// Can't place items in a Segment Container's reference folder
			return E_FAIL;
		}
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pContainerComponent->m_cfProducerFile ) ) )
	{
		IDMUSProdNode* pIDocRootNode;
			
		if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode ) ) )
		{
			GUID guidNodeId;

			if( SUCCEEDED ( pIDocRootNode->GetNodeId( &guidNodeId ) ) )
			{
				IDMUSProdDocType* pIDocType;

				if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->FindDocTypeByNodeId( guidNodeId, &pIDocType ) ) )
				{
					IDMUSProdDocType8* pIDocType8;

					if( SUCCEEDED ( pIDocType->QueryInterface( IID_IDMUSProdDocType8, (void**)&pIDocType8 ) ) )
					{
						if( SUCCEEDED ( theApp.m_pContainerComponent->IsRegisteredObject( pIDocRootNode ) ) )
						{
							if( IsDocRootInFolder( pIDocRootNode ) == S_FALSE )
							{
								if( m_pContainer->IsCircularReference( pIDocRootNode, this ) == S_FALSE )
								{
									if( m_pContainer->IsInScript() )
									{
										// Can't place an external container in a script's container
										GUID guidNodeId;
										if( SUCCEEDED ( pIDocRootNode->GetNodeId( &guidNodeId ) ) )
										{
											if( ::IsEqualGUID( guidNodeId, GUID_ContainerNode ) == FALSE)
											{
												hr = S_OK;
											}
										}
									}
									else
									{
										hr = S_OK;
									}
								}
							}
						}

						RELEASE( pIDocType8 );
					}

					RELEASE( pIDocType );
				}
			}
			

			RELEASE( pIDocRootNode );
		}
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::PasteFromData

HRESULT CFolder::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pContainer != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pContainer->IsInSegment() )
	{
		if( ::IsEqualGUID( m_guidTypeNode, GUID_ContainerRefFolderNode ) ) 
		{
			// Can't place items in a Segment Container's reference folder
			return E_FAIL;
		}
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr;

	hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pContainerComponent->m_cfProducerFile );
	if( SUCCEEDED ( hr ) )
	{
		IDMUSProdNode* pIDocRootNode;
			
		hr = theApp.m_pContainerComponent->m_pIFramework8->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode );
		if( SUCCEEDED ( hr ) )
		{
			if( theApp.m_pContainerComponent->IsRegisteredObject( pIDocRootNode ) == S_OK )
			{
				CContainerObject* pContainerObject = NULL;

				// If DocRoot is in the other folder - remove it
				if( ::IsEqualGUID( m_guidTypeNode, GUID_ContainerEmbedFolderNode ) ) 
				{
					pContainerObject = m_pContainer->m_FolderReference.GetObjectFromDocRoot( pIDocRootNode );
					if( pContainerObject )
					{
						pContainerObject->AddRef();
						m_pContainer->m_FolderReference.DeleteChildNode( pContainerObject, FALSE );
					}
				}
				else if( ::IsEqualGUID( m_guidTypeNode, GUID_ContainerRefFolderNode ) ) 
				{
					pContainerObject = m_pContainer->m_FolderEmbed.GetObjectFromDocRoot( pIDocRootNode );
					if( pContainerObject )
					{
						pContainerObject->AddRef();
						m_pContainer->m_FolderEmbed.DeleteChildNode( pContainerObject, FALSE );
					}
				}
				else
				{
					ASSERT( 0 );
				}

				// Make sure a ContainerObject exists for the DocRoot
				if( pContainerObject == NULL )
				{
					pContainerObject = new CContainerObject( m_pContainer );
					if( pContainerObject )
					{
						if( FAILED ( pContainerObject->SetFileReference( pIDocRootNode ) ) )
						{
							RELEASE( pContainerObject );
						}
					}
				}

				// Add the DocRoot to this folder
				if( pContainerObject )
				{
					hr = InsertChildNode( pContainerObject );
					if( FAILED ( hr ) )
					{
						pContainerObject->AddRef();
						DeleteChildNode( pContainerObject, FALSE );
					}
				}
				else
				{
					hr = E_FAIL;
				}

				RELEASE( pContainerObject );
			}

			RELEASE( pIDocRootNode );
		}
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanChildPasteFromData

HRESULT CFolder::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										BOOL *pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return CanPasteFromData( pIDataObject, pfWillSetReference );
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::ChildPasteFromData

HRESULT CFolder::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return PasteFromData( pIDataObject );
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetObject

HRESULT CFolder::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::OnUpdate	(handles Container's IDMUSProdNotifSink notifications)

HRESULT CFolder::OnUpdate( IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CContainerObject* pContainerObject;

	// FRAMEWORK_FileLoadFinished
	if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileLoadFinished ) )
	{
		GUID guidFile = *(GUID *)pData;

		BOOL fChanged = FALSE;
		
		POSITION pos = m_lstObjects.GetHeadPosition();
		while( pos )
		{
			pContainerObject = m_lstObjects.GetNext( pos );

			if( ::IsEqualGUID( pContainerObject->m_FileRef.li.guidFile, guidFile ) )
			{ 
				CString strOrigScriptAlias = pContainerObject->m_FileRef.strScriptAlias;

				if( SUCCEEDED ( pContainerObject->SetFileReference( pIDocRootNode ) ) )
				{
					// Restore Alias from file
					// Must do this sooner because SetFileReference() resets alias
					if( strOrigScriptAlias.IsEmpty() == FALSE )
					{
						if( strOrigScriptAlias.Compare( pContainerObject->m_FileRef.strScriptAlias ) != 0 )
						{
							pContainerObject->m_FileRef.strScriptAlias = strOrigScriptAlias;
						}
					}

					if( pContainerObject->m_pIDocRootNode
					&&  pContainerObject->m_pIParentNode )
					{
						// Need to refresh node name and node image index
						theApp.m_pContainerComponent->m_pIFramework8->RefreshNode( pContainerObject );
					}
				}

				fChanged = TRUE;
			}
		}

		if( fChanged )
		{
			if( m_pIDocRootNode
			&&  m_pIDocRootNode != this )
			{
				// Notify DocRoot that the container has changed
				IDMUSProdNotifySink* pINotifySink;
				if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
				{
					pINotifySink->OnUpdate( m_pContainer, CONTAINER_FileLoadFinished, NULL );

					RELEASE( pINotifySink );
				}
			}
		}

		if( theApp.m_pContainerComponent->m_pEmbeddedFileRootFile )
		{
			if( ::IsEqualGUID(theApp.m_pContainerComponent->m_pEmbeddedFileRootFile->guidNotification, guidFile) )
			{ 
				delete theApp.m_pContainerComponent->m_pEmbeddedFileRootFile;
				theApp.m_pContainerComponent->m_pEmbeddedFileRootFile = NULL;
			}
		}
	}
	else
	{
		POSITION pos = m_lstObjects.GetHeadPosition();
		while( pos )
		{
			pContainerObject = m_lstObjects.GetNext( pos );

			if( pContainerObject->m_FileRef.pIDocRootNode == pIDocRootNode )
			{
				// FRAMEWORK_FileDeleted
				// FRAMEWORK_FileClosed
				if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileDeleted )
				||  ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileClosed ) )
				{
					DeleteChildNode( pContainerObject, FALSE );
					return S_OK;
				}

				// FRAMEWORK_FileReplaced
				else if( ::IsEqualGUID(guidUpdateType, FRAMEWORK_FileReplaced ) )
				{
					pContainerObject->SetFileReference( (IDMUSProdNode *)pData );
					theApp.m_pContainerComponent->m_pIFramework8->RefreshNode( pContainerObject );
					m_pContainer->SetModified( TRUE );
					return S_OK;
				}

				// DOCROOT_GuidChange
				else if( ::IsEqualGUID(guidUpdateType, DOCROOT_GuidChange ) )
				{
					m_pContainer->SetModified( TRUE );
					return S_OK;
				}

				// GUID_DMCollectionResync
				else if( ::IsEqualGUID(guidUpdateType, GUID_DMCollectionResync ) )
				{
					if( m_pIDocRootNode 
					&&  m_pIDocRootNode != m_pContainer )
					{
						// Pass on the notification to our DocRoot node
						IDMUSProdNotifySink* pINotifySink;
						if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
						{
							pINotifySink->OnUpdate( pIDocRootNode, guidUpdateType, pData );

							RELEASE( pINotifySink );
						}
					}
					return S_OK;
				}

				// All other notifications
				else
				{
					theApp.m_pContainerComponent->m_pIFramework8->RefreshNode( pContainerObject );
					pContainerObject->SyncListInfo();

					// Directly setting modified flag instead of going through SetModified().
					// SetModified() sends CONTAINER_ChangeNotifications.
					// Does not make sense to send CONTAINER_ChangeNotifications  
					// for every little change that might occur to a referenced file.
					m_pContainer->m_fModified = TRUE;
					return S_OK;
				}
			}
		}
	}

	return E_INVALIDARG;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::FindDocRootFromName

HRESULT CFolder::FindDocRootFromName( LPCTSTR pszName, IUnknown** ppIDocRootNode )
{
	CContainerObject* pContainerObject;
	HRESULT hr = S_FALSE;

    POSITION pos = m_lstObjects.GetHeadPosition();
    while( pos )
    {
        pContainerObject = m_lstObjects.GetNext( pos );

		if( pContainerObject->m_FileRef.li.strName.CompareNoCase( pszName ) == 0 )
		{
			if( pContainerObject->m_FileRef.pIDocRootNode )
			{
				pContainerObject->m_FileRef.pIDocRootNode->AddRef();
				*ppIDocRootNode = pContainerObject->m_FileRef.pIDocRootNode;
				hr = S_OK;
			}
			break;
		}
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::FindDocRootFromScriptAlias

HRESULT CFolder::FindDocRootFromScriptAlias( LPCTSTR pszAlias, IUnknown** ppIDocRootNode )
{
	CContainerObject* pContainerObject;
	HRESULT hr = S_FALSE;

    POSITION pos = m_lstObjects.GetHeadPosition();
    while( pos )
    {
        pContainerObject = m_lstObjects.GetNext( pos );

		if( pContainerObject->m_FileRef.strScriptAlias.CompareNoCase( pszAlias ) == 0 )
		{
			if( pContainerObject->m_FileRef.pIDocRootNode )
			{
				pContainerObject->m_FileRef.pIDocRootNode->AddRef();
				*ppIDocRootNode = pContainerObject->m_FileRef.pIDocRootNode;
				hr = S_OK;
			}
			break;
		}
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::IsDocRootInFolder

HRESULT CFolder::IsDocRootInFolder( IDMUSProdNode* pIDocRootNode )
{
	CContainerObject* pContainerObject;
	HRESULT hr = S_FALSE;

    POSITION pos = m_lstObjects.GetHeadPosition();
    while( pos )
    {
        pContainerObject = m_lstObjects.GetNext( pos );

		if( pContainerObject->m_FileRef.pIDocRootNode == pIDocRootNode )
		{
			hr = S_OK;
			break;
		}
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::IsFileGUIDInFolder

HRESULT CFolder::IsFileGUIDInFolder( GUID guidFile )
{
	CContainerObject* pContainerObject;
	HRESULT hr = S_FALSE;

    POSITION pos = m_lstObjects.GetHeadPosition();
    while( pos )
    {
        pContainerObject = m_lstObjects.GetNext( pos );

		if( pContainerObject->m_FileRef.pIDocRootNode )
		{
			if( ::IsEqualGUID( pContainerObject->m_FileRef.li.guidFile, guidFile ) )
			{
				hr = S_OK;
				break;
			}
		}
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::GetObjectFromDocRoot

CContainerObject* CFolder::GetObjectFromDocRoot( IDMUSProdNode* pIDocRootNode )
{
	CContainerObject* pTheContainerObject = NULL;
	CContainerObject* pContainerObject;

    POSITION pos = m_lstObjects.GetHeadPosition();
    while( pos )
    {
        pContainerObject = m_lstObjects.GetNext( pos );

		if( pContainerObject->m_FileRef.pIDocRootNode == pIDocRootNode )
		{
			pTheContainerObject = pContainerObject;
			break;
		}
	}

	return pTheContainerObject;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::ReplaceContent

void CFolder::ReplaceContent( CTypedPtrList<CPtrList, IDMUSProdNode*>& list )
{
	CTypedPtrList<CPtrList, IDMUSProdNode*> listToBeDeleted;

	// Save original list of DocRoot nodes
	POSITION pos = m_lstObjects.GetHeadPosition();
	while( pos )
	{
		CContainerObject* pContainerObject = m_lstObjects.GetNext( pos);

		listToBeDeleted.AddTail( pContainerObject->m_FileRef.pIDocRootNode );
	}

	// Process the new list of files
	pos = list.GetHeadPosition();
	while( pos )
	{
		IDMUSProdNode* pINode = list.GetNext( pos );

		GUID guidFile;
		theApp.m_pContainerComponent->m_pIFramework8->GetNodeFileGUID( pINode, &guidFile );

		IDMUSProdNode* pIDocRootNode;
		if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->FindDocRootNodeByFileGUID( guidFile, &pIDocRootNode ) ) )
		{
			if( theApp.m_pContainerComponent->IsRegisteredObject( pIDocRootNode ) == S_OK )
			{
				POSITION posFind = listToBeDeleted.Find( pIDocRootNode );
				if( posFind )
				{
					// DocRoot is already in this folder - so remove it from the "To Be Deleted" list
					listToBeDeleted.RemoveAt( posFind );
				}
				
				else
				{
					CContainerObject* pContainerObject = NULL;

					// If DocRoot is in the other folder - remove it
					if( ::IsEqualGUID( m_guidTypeNode, GUID_ContainerEmbedFolderNode ) ) 
					{
						pContainerObject = m_pContainer->m_FolderReference.GetObjectFromDocRoot( pIDocRootNode );
						if( pContainerObject )
						{
							pContainerObject->AddRef();
							m_pContainer->m_FolderReference.DeleteChildNode( pContainerObject, FALSE );
						}
					}
					else if( ::IsEqualGUID( m_guidTypeNode, GUID_ContainerRefFolderNode ) ) 
					{
						pContainerObject = m_pContainer->m_FolderEmbed.GetObjectFromDocRoot( pIDocRootNode );
						if( pContainerObject )
						{
							pContainerObject->AddRef();
							m_pContainer->m_FolderEmbed.DeleteChildNode( pContainerObject, FALSE );
						}
					}
					else
					{
						ASSERT( 0 );
					}

					// Make sure a ContainerObject exists for the DocRoot
					if( pContainerObject == NULL )
					{
						pContainerObject = new CContainerObject( m_pContainer );
						if( pContainerObject )
						{
							if( FAILED ( pContainerObject->SetFileReference( pIDocRootNode ) ) )
							{
								RELEASE( pContainerObject );
							}
						}
					}

					// Add the DocRoot to this folder
					if( pContainerObject )
					{
						if( FAILED ( InsertChildNode( pContainerObject ) ) )
						{
							pContainerObject->AddRef();
							DeleteChildNode( pContainerObject, FALSE );
						}
					}

					RELEASE( pContainerObject );
				}
			}

			RELEASE( pIDocRootNode );
		}
	}

	// Delete files no longer in the folder
	while( !listToBeDeleted.IsEmpty() )
	{
		IDMUSProdNode* pIDocRootNode = listToBeDeleted.RemoveHead();

		CContainerObject* pContainerObject = GetObjectFromDocRoot( pIDocRootNode );
		DeleteChildNode( pContainerObject, FALSE );
	}
}
