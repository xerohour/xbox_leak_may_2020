// StyleMotifs.cpp : implementation file
//

#include "stdafx.h"

#include "StyleDesignerDLL.h"
#include "Style.h"
#include "Pattern.h"
#include "NewPatternDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs constructor/destructor

CStyleMotifs::CStyleMotifs()
{
	m_pStyle = NULL;
	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_fModified = FALSE;
}

CStyleMotifs::~CStyleMotifs()
{
	CDirectMusicPattern* pMotif;

	while( !m_lstMotifs.IsEmpty() )
	{
		pMotif = static_cast<CDirectMusicPattern*>( m_lstMotifs.RemoveHead() );
		pMotif->PreDeleteCleanup();
		RELEASE( pMotif );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs::IsDirty

BOOL CStyleMotifs::IsDirty( void )
{
	// Check to see if the list of Motifs was changed
	if( m_fModified )
	{
		return TRUE;
	}

	// Check to see if any of the Motifs were changed
	CDirectMusicPattern* pMotif;
	IPersistStream* pIPS;
	HRESULT hr;

    POSITION pos = m_lstMotifs.GetHeadPosition();
    while( pos )
    {
        pMotif = m_lstMotifs.GetNext( pos );

		if( SUCCEEDED ( pMotif->QueryInterface( IID_IPersistStream, (void **)&pIPS ) ) )
		{
			hr = pIPS->IsDirty();
			RELEASE( pIPS );

			if( hr == S_OK )
			{
				return TRUE;
			}
		}
    }

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs::SetModified

void CStyleMotifs::SetModified( BOOL fModified )
{
	// Set modified flag of Motif folder
	m_fModified = fModified;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs::InsertChildNodeAtPos

HRESULT CStyleMotifs::InsertChildNodeAtPos( CDirectMusicPattern* pMotif, CDirectMusicPattern* pPositionMotif )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pMotif != NULL );
	ASSERT( pPositionMotif != NULL );
	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// Add to CDirectMusicStyle Motif list
	m_pStyle->AddMotif( pMotif, pPositionMotif );

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pMotif, m_pStyle, this );

	// Add node to Project Tree
	if( !SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AddNode(pMotif, this) ) )
	{
		DeleteChildNode( pMotif, FALSE );
		return E_FAIL;
	}

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IUnknown implementation

HRESULT CStyleMotifs::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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
		if( m_pStyle )
		{
	        return m_pStyle->QueryInterface( riid, ppvObj );
		}

		*ppvObj = NULL;
		return E_NOINTERFACE;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CStyleMotifs::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	AfxOleLockApp(); 

	return m_pStyle->AddRef();
}

ULONG CStyleMotifs::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	AfxOleUnlockApp(); 

	return m_pStyle->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetNodeImageIndex

HRESULT CStyleMotifs::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return( theApp.m_pStyleComponent->GetFolderImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetFirstChild

HRESULT CStyleMotifs::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstChildNode = NULL;

	if( !m_lstMotifs.IsEmpty() )
	{
		CDirectMusicPattern* pMotif = static_cast<CDirectMusicPattern*>( m_lstMotifs.GetHead() );
		
		if( pMotif )
		{
			pMotif->AddRef();
			*ppIFirstChildNode = pMotif;
		}
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetNextChild

HRESULT CStyleMotifs::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
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

	CDirectMusicPattern* pMotif;

    POSITION pos = m_lstMotifs.GetHeadPosition();

    while( pos )
    {
        pMotif = m_lstMotifs.GetNext( pos );
		if( pMotif == pIChildNode )
		{
			if( pos )
			{
				pMotif = m_lstMotifs.GetNext( pos );

				pMotif->AddRef();
				*ppINextChildNode = pMotif;
			}
			break;
		}
    }

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetComponent

HRESULT CStyleMotifs::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return theApp.m_pStyleComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetDocRootNode

HRESULT CStyleMotifs::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CStyleMotifs IDMUSProdNode::SetDocRootNode

HRESULT CStyleMotifs::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetParentNode

HRESULT CStyleMotifs::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::SetParentNode

HRESULT CStyleMotifs::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetNodeId

HRESULT CStyleMotifs::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_StyleMotifFolderNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetNodeName

HRESULT CStyleMotifs::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_MOTIF_FOLDER_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetNodeNameMaxLength

HRESULT CStyleMotifs::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a Motif folder

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::ValidateNodeName

HRESULT CStyleMotifs::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a Motif folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::SetNodeName

HRESULT CStyleMotifs::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a Motif folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetNodeListInfo

HRESULT CStyleMotifs::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
//	AFX_MANAGE_STATE( _afxModuleAddrThis );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetEditorClsId

HRESULT CStyleMotifs::GetEditorClsId( CLSID* pClsId )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Motif folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetEditorTitle

HRESULT CStyleMotifs::GetEditorTitle( BSTR* pbstrTitle )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Motif folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetEditorWindow

HRESULT CStyleMotifs::GetEditorWindow( HWND* hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Motif folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::SetEditorWindow

HRESULT CStyleMotifs::SetEditorWindow( HWND hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Motif folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::UseOpenCloseImages

HRESULT CStyleMotifs::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetRightClickMenuId

HRESULT CStyleMotifs::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_STYLEMOTIFS_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::OnRightClickMenuInit

HRESULT CStyleMotifs::OnRightClickMenuInit( HMENU hMenu )
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
// CStyleMotifs IDMUSProdNode::OnRightClickMenuSelect

HRESULT CStyleMotifs::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_NEW_MOTIF:
			hr = CreateNewMotif();
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

		case IDM_MOTIF_FROMMIDI:
			if(m_pStyle != NULL)
			{
				m_pStyle->ImportMidiFileAsPattern( TRUE );
			}
			break;

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
// CStyleMotifs IDMUSProdNode::DeleteChildNode

HRESULT CStyleMotifs::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIChildNode != NULL );
	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CWaitCursor wait;

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Remove node from Project Tree
	if( theApp.m_pStyleComponent->m_pIFramework->RemoveNode( pIChildNode, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Save undo state
//	m_pStyle->m_pINodeBeingDeleted = pIChildNode;
//	m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_DELETE_MOTIF );
//	m_pStyle->m_pINodeBeingDeleted = NULL;

	// Store motif's name for NotifyNodes() call
	CString strMotifName;
	BSTR bstrMotifName;
	if( SUCCEEDED ( pIChildNode->GetNodeName( &bstrMotifName ) ) )
	{
		strMotifName = bstrMotifName;
		::SysFreeString( bstrMotifName );
	}

	// Remove from CDirectMusicStyle Motif list
	m_pStyle->RemoveMotif( (CDirectMusicPattern *)pIChildNode );

	// Notify connected nodes that Motif has been deleted
	{
		DMUSProdMotifData	mtfData;
		WCHAR				wstrMotifName[MAX_PATH];
	
		MultiByteToWideChar( CP_ACP, 0, strMotifName, -1, wstrMotifName, MAX_PATH );
		mtfData.pwszMotifName = wstrMotifName;
		mtfData.pwszOldMotifName = NULL;
		
		theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( m_pStyle, STYLE_MotifDeleted, &mtfData );
	}

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::InsertChildNode

HRESULT CStyleMotifs::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CDirectMusicPattern* pMotif = (CDirectMusicPattern *)pIChildNode;

	if( pMotif )
	{
		// Add to CDirectMusicStyle Motif list
		m_pStyle->AddMotif( pMotif, NULL );
	}
	else
	{
		pMotif = new CDirectMusicPattern( m_pStyle, TRUE );
		if( pMotif == NULL )
		{
			return E_OUTOFMEMORY ;
		}

		HRESULT hr = pMotif->Initialize();
		if( FAILED( hr ) )
		{
			RELEASE( pMotif );
			return hr;
		}

		// Add to CDirectMusicStyle Pattern list
		m_pStyle->AddMotif( pMotif, NULL );
		pMotif->Release();
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pMotif, m_pStyle, this );

	// Add node to Project Tree
	if( !SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AddNode(pMotif, this) ) )
	{
		DeleteChildNode( pMotif, FALSE );
		return E_FAIL;
	}

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::DeleteNode

HRESULT CStyleMotifs::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't delete a Motif folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::OnNodeSelChanged

HRESULT CStyleMotifs::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::CreateDataObject

HRESULT CStyleMotifs::CreateDataObject( IDataObject** ppIDataObject )
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
// CStyleMotifs IDMUSProdNode::CanCut

HRESULT CStyleMotifs::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_FALSE;		// Can't remove Motif folder from Style
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::CanCopy

HRESULT CStyleMotifs::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::CanDelete

HRESULT CStyleMotifs::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_FALSE;		// Can't delete Motif folder from Style
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::CanDeleteChildNode

HRESULT CStyleMotifs::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure node is in Motif list
	POSITION pos = m_lstMotifs.Find( pIChildNode );
	if( pos == NULL )
	{
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::CanPasteFromData

HRESULT CStyleMotifs::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotif ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) ) 
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPattern ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::PasteFromData

HRESULT CStyleMotifs::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;
	
	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPattern ) ) 
	||  SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotif ) ) )
	{
		// Let CDirectMusicStyle handle CF_PATTERN and CF_MOTIF formats
		hr = m_pStyle->PasteCF_MOTIF( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) 
		 ||  SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) ) )
	{
		// Let CDirectMusicStyle handle CF_PATTERNLIST and CF_MOTIFLIST formats
		hr = m_pStyle->PasteCF_MOTIFLIST( pDataObject, pIDataObject, NULL );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::CanChildPasteFromData

HRESULT CStyleMotifs::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
											 BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotif ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) ) 
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPattern ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::ChildPasteFromData

HRESULT CStyleMotifs::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

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

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPattern ) ) 
	||  SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotif ) ) )
	{
		// Let CDirectMusicStyle handle CF_PATTERN and CF_MOTIF formats
		hr = m_pStyle->PasteCF_MOTIF( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) 
		 ||  SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) ) )
	{
		// Let CDirectMusicStyle handle CF_PATTERNLIST and CF_MOTIFLIST formats
		hr = m_pStyle->PasteCF_MOTIFLIST( pDataObject, pIDataObject, NULL );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs IDMUSProdNode::GetObject

HRESULT CStyleMotifs::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs CreateNewMotif

HRESULT CStyleMotifs::CreateNewMotif( void )
{
	CNewPatternDlg dlgNewMotif;

	dlgNewMotif.m_wEmbellishment = EMB_MOTIF;
	dlgNewMotif.m_pStyle = m_pStyle;

	HRESULT	hr = E_FAIL;

	int nReturn = dlgNewMotif.DoModal();

	if( nReturn == IDC_NEW_PATTERN )
	{
		hr = InsertChildNode( NULL );
	}
	else if( nReturn == IDC_LINK_PATTERN )
	{
		ASSERT( dlgNewMotif.m_pPattern != NULL );

		CDirectMusicPattern* pMotif = new CDirectMusicPattern( m_pStyle, TRUE );
		if( pMotif )
		{
			// Copy pertinent information from the original Pattern (or Motif)
			if( SUCCEEDED ( dlgNewMotif.m_pPattern->CopyToForLinkAllParts( pMotif ) ) )
			{
				// Add to CDirectMusicStyle Motif list
				InsertChildNode( pMotif );

				// Sync original Pattern (or Motif) because Parts are now linked
				dlgNewMotif.m_pPattern->SyncPatternWithMidiStripMgr();
				dlgNewMotif.m_pPattern->SyncPatternWithDirectMusic();

				hr = S_OK;
			}

			pMotif->Release();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleMotifs SortMotifList

void CStyleMotifs::SortMotifList( void )
{
	// Do a simple bubble sort on the list of Motifs
	// Order them by m_strName

	bool fContinue = true;

	while( fContinue )
	{
		fContinue = false;

		POSITION posCur = m_lstMotifs.GetHeadPosition();
		while( posCur )
		{
			POSITION posPrev = posCur;

			m_lstMotifs.GetNext( posCur );
			if( posCur )
			{
				CDirectMusicPattern* pMotifPrev = m_lstMotifs.GetAt( posPrev );
				CDirectMusicPattern* pMotifCur = m_lstMotifs.GetAt( posCur );

				if( pMotifPrev->m_strName.CompareNoCase(pMotifCur->m_strName) > 0 )
				{
					m_lstMotifs.RemoveAt( posPrev );
					posCur = m_lstMotifs.InsertAfter( posCur, pMotifPrev );

					fContinue = true;
				}
			}
		}
	}
}
