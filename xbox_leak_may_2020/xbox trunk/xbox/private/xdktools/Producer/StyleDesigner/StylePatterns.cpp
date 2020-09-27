// StylePatterns.cpp : implementation file
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
// CStylePatterns constructor/destructor

CStylePatterns::CStylePatterns()
{
	m_pStyle = NULL;
	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_fModified = FALSE;
}

CStylePatterns::~CStylePatterns()
{
	CDirectMusicPattern* pPattern;

	while( !m_lstPatterns.IsEmpty() )
	{
		pPattern = static_cast<CDirectMusicPattern*>( m_lstPatterns.RemoveHead() );
		pPattern->PreDeleteCleanup();
		RELEASE( pPattern );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns::IsDirty

BOOL CStylePatterns::IsDirty( void )
{
	// Check to see if the list of Patterns was changed
	if( m_fModified )
	{
		return TRUE;
	}

	// Check to see if any of the Patterns were changed
	CDirectMusicPattern* pPattern;
	IPersistStream* pIPS;
	HRESULT hr;

    POSITION pos = m_lstPatterns.GetHeadPosition();
    while( pos )
    {
        pPattern = m_lstPatterns.GetNext( pos );

		if( SUCCEEDED ( pPattern->QueryInterface( IID_IPersistStream, (void **)&pIPS ) ) )
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
// CStylePatterns::SetModified

void CStylePatterns::SetModified( BOOL fModified )
{
	// Set modified flag of Pattern folder
	m_fModified = fModified;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns::InsertChildNodeAtPos

HRESULT CStylePatterns::InsertChildNodeAtPos( CDirectMusicPattern* pPattern, CDirectMusicPattern* pPositionPattern )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pPattern != NULL );
	ASSERT( pPositionPattern != NULL );
	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// Add to CDirectMusicStyle Pattern list
	m_pStyle->AddPattern( pPattern, pPositionPattern );

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pPattern, m_pStyle, this );

	// Add node to Project Tree
	if( !SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AddNode(pPattern, this) ) )
	{
		DeleteChildNode( pPattern, FALSE );
		return E_FAIL;
	}

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IUnknown implementation

HRESULT CStylePatterns::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CStylePatterns::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	AfxOleLockApp(); 

	return m_pStyle->AddRef();
}

ULONG CStylePatterns::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	AfxOleUnlockApp(); 

	return m_pStyle->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetNodeImageIndex

HRESULT CStylePatterns::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return( theApp.m_pStyleComponent->GetFolderImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetFirstChild

HRESULT CStylePatterns::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstChildNode = NULL;

	if( !m_lstPatterns.IsEmpty() )
	{
		CDirectMusicPattern* pPattern = static_cast<CDirectMusicPattern*>( m_lstPatterns.GetHead() );
		
		if( pPattern )
		{
			pPattern->AddRef();
			*ppIFirstChildNode = pPattern;
		}
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetNextChild

HRESULT CStylePatterns::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
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

	CDirectMusicPattern* pPattern;

    POSITION pos = m_lstPatterns.GetHeadPosition();
    while( pos )
    {
        pPattern = m_lstPatterns.GetNext( pos );
		if( pPattern == pIChildNode )
		{
			if( pos )
			{
				pPattern = m_lstPatterns.GetNext( pos );

				pPattern->AddRef();
				*ppINextChildNode = pPattern;
			}
			break;
		}
    }

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetComponent

HRESULT CStylePatterns::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return theApp.m_pStyleComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetDocRootNode

HRESULT CStylePatterns::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CStylePatterns IDMUSProdNode::SetDocRootNode

HRESULT CStylePatterns::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetParentNode

HRESULT CStylePatterns::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::SetParentNode

HRESULT CStylePatterns::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetNodeId

HRESULT CStylePatterns::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_StylePatternFolderNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetNodeName

HRESULT CStylePatterns::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_PATTERN_FOLDER_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetNodeNameMaxLength

HRESULT CStylePatterns::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a Pattern folder

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::ValidateNodeName

HRESULT CStylePatterns::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::SetNodeName

HRESULT CStylePatterns::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetNodeListInfo

HRESULT CStylePatterns::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
//	AFX_MANAGE_STATE( _afxModuleAddrThis );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetEditorClsId

HRESULT CStylePatterns::GetEditorClsId( CLSID* pClsId )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetEditorTitle

HRESULT CStylePatterns::GetEditorTitle( BSTR* pbstrTitle )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetEditorWindow

HRESULT CStylePatterns::GetEditorWindow( HWND* hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::SetEditorWindow

HRESULT CStylePatterns::SetEditorWindow( HWND hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::UseOpenCloseImages

HRESULT CStylePatterns::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetRightClickMenuId

HRESULT CStylePatterns::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_STYLEPATTERNS_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::OnRightClickMenuInit

HRESULT CStylePatterns::OnRightClickMenuInit( HMENU hMenu )
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
// CStylePatterns IDMUSProdNode::OnRightClickMenuSelect

HRESULT CStylePatterns::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_NEW_PATTERN:
			hr = CreateNewPattern();
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

		case IDM_PATTERN_FROMMIDI:
			if(m_pStyle != NULL)
			{
				m_pStyle->ImportMidiFileAsPattern( FALSE );
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
// CStylePatterns IDMUSProdNode::DeleteChildNode

HRESULT CStylePatterns::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
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
//	m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_DELETE_PATTERN );
//	m_pStyle->m_pINodeBeingDeleted = NULL;

	// Remove from CDirectMusicStyle Pattern list
	m_pStyle->RemovePattern( (CDirectMusicPattern *)pIChildNode );

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::InsertChildNode

HRESULT CStylePatterns::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CDirectMusicPattern* pPattern = (CDirectMusicPattern *)pIChildNode;

	if( pPattern )
	{
		// Add to CDirectMusicStyle Pattern list
		m_pStyle->AddPattern( pPattern, NULL );
	}
	else
	{
		pPattern = new CDirectMusicPattern( m_pStyle, FALSE );
		if( pPattern == NULL )
		{
			return E_OUTOFMEMORY ;
		}

		HRESULT hr = pPattern->Initialize();
		if( FAILED( hr ) )
		{
			RELEASE( pPattern );
			return hr;
		}

		// Add to CDirectMusicStyle Pattern list
		m_pStyle->AddPattern( pPattern, NULL );
		pPattern->Release();
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pPattern, m_pStyle, this );

	// Add node to Project Tree
	if( !SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AddNode(pPattern, this) ) )
	{
		DeleteChildNode( pPattern, FALSE );
		return E_FAIL;
	}

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::DeleteNode

HRESULT CStylePatterns::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't delete a Pattern folder
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::OnNodeSelChanged

HRESULT CStylePatterns::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::CreateDataObject

HRESULT CStylePatterns::CreateDataObject( IDataObject** ppIDataObject )
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
// CStylePatterns IDMUSProdNode::CanCut

HRESULT CStylePatterns::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_FALSE;		// Can't remove Pattern folder from Style
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::CanCopy

HRESULT CStylePatterns::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::CanDelete

HRESULT CStylePatterns::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_FALSE;		// Can't delete Pattern folder from Style
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::CanDeleteChildNode

HRESULT CStylePatterns::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure node is in Pattern list
	POSITION pos = m_lstPatterns.Find( pIChildNode );
	if( pos == NULL )
	{
		return E_FAIL;
	}

	// Cannot delete the last Pattern from a Style
	if( m_lstPatterns.GetCount() < 2 )
	{
		return S_FALSE;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::CanPasteFromData

HRESULT CStylePatterns::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPattern ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) 
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotif ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::PasteFromData

HRESULT CStylePatterns::PasteFromData( IDataObject* pIDataObject )
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
		hr = m_pStyle->PasteCF_PATTERN( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) 
		 ||  SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) ) )
	{
		// Let CDirectMusicStyle handle CF_PATTERNLIST and CF_MOTIFLIST formats
		hr = m_pStyle->PasteCF_PATTERNLIST( pDataObject, pIDataObject, NULL );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::CanChildPasteFromData

HRESULT CStylePatterns::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPattern ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) 
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotif ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::ChildPasteFromData

HRESULT CStylePatterns::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	if( pIDataObject == NULL 
	||  pIChildNode == NULL )
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
		hr = m_pStyle->PasteCF_PATTERN( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) 
		 ||  SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) ) )
	{
		// Let CDirectMusicStyle handle CF_PATTERNLIST and CF_MOTIFLIST formats
		hr = m_pStyle->PasteCF_PATTERNLIST( pDataObject, pIDataObject, NULL );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns IDMUSProdNode::GetObject

HRESULT CStylePatterns::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns CreateNewPattern

HRESULT CStylePatterns::CreateNewPattern( void )
{
	CNewPatternDlg dlgNewPattern;

	dlgNewPattern.m_wEmbellishment = EMB_NORMAL;
	dlgNewPattern.m_pStyle = m_pStyle;

	HRESULT	hr = E_FAIL;

	int nReturn = dlgNewPattern.DoModal();

	if( nReturn == IDC_NEW_PATTERN )
	{
		hr = InsertChildNode( NULL );
	}
	else if( nReturn == IDC_LINK_PATTERN )
	{
		ASSERT( dlgNewPattern.m_pPattern != NULL );

		CDirectMusicPattern* pPattern = new CDirectMusicPattern( m_pStyle, FALSE );
		if( pPattern )
		{
			// Copy pertinent information from the original Pattern (or Motif)
			if( SUCCEEDED ( dlgNewPattern.m_pPattern->CopyToForLinkAllParts( pPattern ) ) )
			{
				// Add to CDirectMusicStyle Pattern list
				InsertChildNode( pPattern );

				// Sync original Pattern (or Motif) because Parts are now linked
				dlgNewPattern.m_pPattern->SyncPatternWithMidiStripMgr();
				dlgNewPattern.m_pPattern->SyncPatternWithDirectMusic();

				hr = S_OK;
			}

			pPattern->Release();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePatterns SortPatternList

void CStylePatterns::SortPatternList( void )
{
	// Do a simple bubble sort on the list of Patterns
	// Order them by m_strName

	bool fContinue = true;

	while( fContinue )
	{
		fContinue = false;

		POSITION posCur = m_lstPatterns.GetHeadPosition();
		while( posCur )
		{
			POSITION posPrev = posCur;

			m_lstPatterns.GetNext( posCur );
			if( posCur )
			{
				CDirectMusicPattern* pPatternPrev = m_lstPatterns.GetAt( posPrev );
				CDirectMusicPattern* pPatternCur = m_lstPatterns.GetAt( posCur );

				if( pPatternPrev->m_strName.CompareNoCase(pPatternCur->m_strName) > 0 )
				{
					m_lstPatterns.RemoveAt( posPrev );
					posCur = m_lstPatterns.InsertAfter( posCur, pPatternPrev );

					fContinue = true;
				}
			}
		}
	}
}
