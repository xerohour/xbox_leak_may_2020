// FileNode.cpp: implementation of the CFileNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "ioJazzDoc.h"
#include "TabFileDesign.h"
#include "TabFileRuntime.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// {B8520A40-CB9E-11d0-89AE-00A0C9054129}
const GUID GUID_FileNode = 
{ 0xb8520a40, 0xcb9e, 0x11d0, { 0x89, 0xae, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };

// {89A55C20-6BC5-11d1-89AE-00A0C9054129}
static const GUID GUID_FilePropPageManager = 
{ 0x89a55c20, 0x6bc5, 0x11d1, { 0x89, 0xae, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


short CFilePropPageManager::sm_nActiveTab = 0;

//////////////////////////////////////////////////////////////////////
// CFilePropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilePropPageManager::CFilePropPageManager()
{
	m_pTabDesign = NULL;
	m_pTabRuntime = NULL;
	m_GUIDManager = GUID_FilePropPageManager;
}

CFilePropPageManager::~CFilePropPageManager()
{
	if( m_pTabDesign )
	{
		delete m_pTabDesign;
	}

	if( m_pTabRuntime )
	{
		delete m_pTabRuntime;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFilePropPageManager::RemoveCurrentObject

void CFilePropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CFilePropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CFilePropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strFile;

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( theApp.m_hInstance );
	strFile.LoadString( IDS_FILE_TEXT );
	AfxSetResourceHandle( hInstance );

	CString strTitle = strFile;

	CFileNode *pFileNode;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pFileNode))) )
	{
		CString strFilterName;

		CString strExt;
		int nFindPos = pFileNode->m_strName.ReverseFind( (TCHAR)'.' );
		if( nFindPos != -1 )
		{
			strExt = pFileNode->m_strName.Right( pFileNode->m_strName.GetLength() - nFindPos );
		}

		IDMUSProdDocType* pIDocType;
		BSTR bstrExt = strExt.AllocSysString();

		if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
		{
			CJazzDocTemplate* pTemplate = theApp.FindDocTemplate( pIDocType );
			if( pTemplate )
			{
				pTemplate->GetDocString( strFilterName, CDocTemplate::docName );
			}
			pIDocType->Release();
		}

		strTitle = pFileNode->m_strName + _T(" ");
		if( !strFilterName.IsEmpty() )
		{
			strTitle += strFilterName + _T(" ");
		}
		strTitle += strFile;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFilePropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CFilePropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( theApp.m_hInstance );

	// Add Design tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	m_pTabDesign = new CTabFileDesign( this );
	if( m_pTabDesign )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabDesign->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Runtime tab
	m_pTabRuntime = new CTabFileRuntime( this );
	if( m_pTabRuntime )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabRuntime->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	AfxSetResourceHandle( hInstance );

	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFilePropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CFilePropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CFilePropPageManager::sm_nActiveTab );

	CAppBasePropPageManager::OnRemoveFromPropertySheet();

	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFilePropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CFilePropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CFileNode* pFileNode;
	
	if( m_pIPropPageObject == NULL )
	{
		pFileNode = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pFileNode ) ) )
	{
		return E_FAIL;
	}

	m_pTabDesign->SetFile( pFileNode );
	m_pTabRuntime->SetFile( pFileNode );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CFileNode Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileNode::CFileNode()
{
    m_dwRef = 0;

	CoCreateGuid( &m_guid ); 

	m_pIParentNode = NULL;
	m_pIChildNode = NULL;
	m_pProject = NULL;
	m_pComponentDoc = NULL;

	m_hItem = NULL;
	m_hChildItem = NULL;
	m_fInOnOpenDocument = FALSE;

	m_wFlags = FSF_NOFLAGS;
	m_nRuntimeSaveAction = RSA_NOACTION;

	memset( &m_guidListInfoObject, 0, sizeof(GUID) );
}

CFileNode::~CFileNode()
{
	ASSERT( m_fInOnOpenDocument == FALSE );

	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		pIPropSheet->Release();
	}

	theApp.CleanUpNotifyLists( this, FALSE );

	// Cleanup m_lstWP
	wpWindowPlacement* pWP;
	while( !m_lstWP.IsEmpty() )
	{
		pWP = static_cast<wpWindowPlacement*>( m_lstWP.RemoveHead() );
		delete pWP;
	}

	// Cleanup m_lstNotifyNodes
	EmptyNotifyNodesList();

	// Cleanup m_lstNotifyWhenLoadFinished
	IDMUSProdNotifySink* pINotifySink;
	ASSERT( m_lstNotifyWhenLoadFinished.IsEmpty() );
	while( !m_lstNotifyWhenLoadFinished.IsEmpty() )
	{
		pINotifySink = static_cast<IDMUSProdNotifySink*>( m_lstNotifyWhenLoadFinished.RemoveHead() );
		pINotifySink->Release();
	}

	// Remove FileNode's child (associated document)
	if( m_pIChildNode )
	{
		DeleteChildNode( m_pIChildNode, FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IUnknown implementation

HRESULT CFileNode::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageObject *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CFileNode::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ++m_dwRef;
}

ULONG CFileNode::Release()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    ASSERT( m_dwRef != 0 );

    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetNodeImageIndex

HRESULT CFileNode::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pnFirstImage == NULL )
	{
		return E_POINTER;
	}

	if( IsProducerFile() )
	{
		// Set the default tree image
		*pnFirstImage = theApp.m_nFirstImage + FIRST_PRODUCER_FILE_IMAGE;

		// Determine file extension
		CString strExt;
		int nFindPos = m_strName.ReverseFind( (TCHAR)'.' );
		if( nFindPos != -1 )
		{
			strExt = m_strName.Right( m_strName.GetLength() - nFindPos );
		}
		BSTR bstrExt = strExt.AllocSysString();
		
		// Find DocType
		IDMUSProdDocType* pIDocType;
		if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
		{
			CJazzDocTemplate* pTemplate = theApp.FindDocTemplate( pIDocType );
			if( pTemplate )
			{
				short nTreeImageIndex = pTemplate->GetTreeImageIndex();
				if( nTreeImageIndex != -1 )
				{
					// Override the default tree image
					*pnFirstImage = nTreeImageIndex;
				}
			}

			pIDocType->Release();
		}
	}
	else
	{
		*pnFirstImage = theApp.m_nFirstImage + FIRST_FILE_IMAGE;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetFirstChild

HRESULT CFileNode::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return E_NOTIMPL;	// FileNode node does not manage its children
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetNextChild

HRESULT CFileNode::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return E_NOTIMPL;	// FileNode node does not manage its children
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetComponent

HRESULT CFileNode::GetComponent( IDMUSProdComponent** ppIComponent )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return E_NOTIMPL;	// FileNode node is not associated with a Component
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetDocRootNode

HRESULT CFileNode::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return E_NOTIMPL;	// intentionally not implemented
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::SetDocRootNode

HRESULT CFileNode::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return E_NOTIMPL;	// intentionally not implemented
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetParentNode

HRESULT CFileNode::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pIParentNode != NULL );

	if( ppIParentNode == NULL )
	{
		return E_POINTER;
	}

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::SetParentNode

HRESULT CFileNode::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( pIParentNode != NULL );

	if( pIParentNode == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetNodeId

HRESULT CFileNode::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_FileNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetNodeName

HRESULT CFileNode::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetNodeNameMaxLength

HRESULT CFileNode::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pnMaxLength == NULL )
	{
		return E_POINTER;
	}

	*pnMaxLength = MAX_LENGTH_FILE_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::ValidateNodeName

HRESULT CFileNode::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	int iBad = strName.FindOneOf( _T("\\/:*?\"<>;|%") );
	if( iBad != -1 )
	{
		AfxMessageBox( IDS_ERR_FILE_CHAR );
		return S_FALSE;
	}

	// Determine node's old filename
	CString strOldFileName;
	ConstructFileName( strOldFileName );

	// Determine node's old extension
	CString strExt;
	int nFindPos = m_strName.ReverseFind( (TCHAR)'.' );
	if( nFindPos != -1 )
	{
		strExt = m_strName.Right( m_strName.GetLength() - nFindPos );
	}

	// If necessary, add extension to new name
	CString strNewExt = strName.Right( strExt.GetLength() );
	if( strNewExt != strExt )
	{
		strName += strExt;
	}

	// Make sure name does not exceed max chars
	if( strName.GetLength() > MAX_LENGTH_FILE_NAME )
	{
		AfxMessageBox( IDS_ERR_NAME_LENGTH );
		return S_FALSE;
	}

	// Determine node's new filename
	CString strOrigName = m_strName;
	m_strName = strName;
	CString strNewFileName;
	ConstructFileName( strNewFileName );
	m_strName = strOrigName;

	// Make sure path does not exceed max chars
	if( strNewFileName.GetLength() >= _MAX_DIR )
	{
		AfxMessageBox( IDS_ERR_FILENAME_LENGTH );
		return S_FALSE;
	}

	// Notify connected nodes
	if(	theApp.m_fSendFileNameChangeNotification )
	{
		if( m_pIChildNode )
		{
			// Notify connected nodes that file name is about to change
			theApp.m_pFramework->NotifyNodes( m_pIChildNode, FRAMEWORK_BeforeFileNameChange, NULL );

			// Notify m_pChildNode that filename has changed
			IDMUSProdNotifySink* pINotifySink;
			if( SUCCEEDED ( m_pIChildNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
			{
				pINotifySink->OnUpdate( m_pIChildNode, FRAMEWORK_BeforeFileNameChange, NULL );
				pINotifySink->Release();
			}
		}
	}

	// Change filename to new filename
	if( theApp.RenameThePath(strOldFileName, strNewFileName, strOldFileName) == FALSE )
	{
		// Notify connected nodes
		if(	theApp.m_fSendFileNameChangeNotification )
		{
			if( m_pIChildNode )
			{
				// Notify connected nodes that file name is about to change
				theApp.m_pFramework->NotifyNodes( m_pIChildNode, FRAMEWORK_AbortFileNameChange, NULL );

				// Notify m_pChildNode that filename has changed
				IDMUSProdNotifySink* pINotifySink;
				if( SUCCEEDED ( m_pIChildNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
				{
					pINotifySink->OnUpdate( m_pIChildNode, FRAMEWORK_AbortFileNameChange, NULL );
					pINotifySink->Release();
				}
			}
		}
		return S_FALSE;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::SetNodeName

HRESULT CFileNode::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pProject != NULL );
	ASSERT( theApp.m_pFramework != NULL );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( m_strName == strName )
	{
		return S_OK;
	}

	if( theApp.m_fUserChangedNodeName )
	{
		// Determine node's old extension
		CString strExt;
		int nFindPos = m_strName.ReverseFind( (TCHAR)'.' );
		if( nFindPos != -1 )
		{
			strExt = m_strName.Right( m_strName.GetLength() - nFindPos );
		}

		// If necessary, add extension to new name
		CString strNewExt = strName.Right( strExt.GetLength() );
		if( strNewExt != strExt )
		{
			strName += strExt;
		}
	}

	// Store the new name
	CString strOrigName = m_strName;
	m_strName = strName;

	// Change text in Project Tree
	HTREEITEM hItem = theApp.m_pFramework->FindTreeItem( this );
	if( hItem )
	{
		CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
		if( pTreeCtrl )
		{
			// Update label in the Project Tree
			pTreeCtrl->SetItemText( hItem, m_strName );
		}
	}

	// Change name of DocRootNode
	if( m_pIChildNode )
	{
		BSTR bstrDocRootName;
		if( SUCCEEDED ( m_pIChildNode->GetNodeName( &bstrDocRootName ) ) )
		{
			CString strDocRootName = bstrDocRootName;
			::SysFreeString( bstrDocRootName );

			CString strNameMinusExt;
			int nFindPos = strOrigName.ReverseFind( (TCHAR)'.' );
			if( nFindPos != -1 )
			{
				strNameMinusExt = strOrigName.Left( nFindPos );
			}

			if( strDocRootName.Compare( strNameMinusExt ) == 0 )
			{
				// Names were equal so continue to keep in sync
				nFindPos = m_strName.ReverseFind( (TCHAR)'.' );
				if( nFindPos != -1 )
				{
					strNameMinusExt = m_strName.Left( nFindPos );
				}
				bstrDocRootName = strNameMinusExt.AllocSysString();
				if( m_pIChildNode->ValidateNodeName( bstrDocRootName ) == S_OK )
				{
					bstrDocRootName = strNameMinusExt.AllocSysString();
					m_pIChildNode->SetNodeName( bstrDocRootName );
					theApp.m_pFramework->RefreshNode( m_pIChildNode );
				}
			}
		}
	}

	// Change Document name
	if( m_pIParentNode 
	&&  m_pComponentDoc )
	{
		CString strFileName;
		
		ConstructFileName( strFileName );
		m_pComponentDoc->SetPathName( strFileName, FALSE );
	}

	// Change the runtime filename
	int nFindPos = m_strName.ReverseFind( (TCHAR)'.' );
	if( nFindPos != -1 )
	{
		CString strRuntimeFileName;

		theApp.AdjustFileName( FT_RUNTIME, strOrigName, strRuntimeFileName );

		if( m_strRuntimeFileName.IsEmpty()
		||	m_strRuntimeFileName.CompareNoCase( strRuntimeFileName ) == 0 )
		{
			CString strNextRuntimeFile;
			
			theApp.AdjustFileName( FT_RUNTIME, m_strName, m_strRuntimeFileName );
			ConstructRuntimePath( strNextRuntimeFile );
			strNextRuntimeFile += m_strRuntimeFileName;
			
			if( m_strRuntimeFile.IsEmpty() == FALSE
			&&	m_strRuntimeFile.CompareNoCase( strNextRuntimeFile ) != 0 )
			{
				if( theApp.RenameThePath( m_strRuntimeFile, strNextRuntimeFile, m_strRuntimeFile ) == TRUE )
				{
					m_strRuntimeFile = strNextRuntimeFile;
				}
			}
		}
	}

	// Refresh Property sheet
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		pIPropSheet->Release();
	}

	return S_OK;	// For now just set programmatically
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetNodeListInfo

HRESULT CFileNode::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	WORD wOrigSize = pListInfo->wSize;
    memset( pListInfo, 0, wOrigSize );
	pListInfo->wSize = wOrigSize;
	
	if( m_pIChildNode )
	{
		return m_pIChildNode->GetNodeListInfo( pListInfo );
	}

	pListInfo->bstrName = m_strListInfoName.AllocSysString();
    pListInfo->bstrDescriptor = m_strListInfoDescriptor.AllocSysString();
    memcpy( &pListInfo->guidObject, &m_guidListInfoObject, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetEditorClsId

HRESULT CFileNode::GetEditorClsId( CLSID* pClsId )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;	// FileNode is not associated with an editor
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetEditorTitle

HRESULT CFileNode::GetEditorTitle( BSTR* pbstrTitle )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;	// FileNode is not associated with an editor
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetEditorWindow

HRESULT CFileNode::GetEditorWindow( HWND* hWndEditor )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;	// FileNode is not associated with an editor
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::SetEditorWindow

HRESULT CFileNode::SetEditorWindow( HWND hWndEditor )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;	// FileNode is not associated with an editor
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::UseOpenCloseImages

HRESULT CFileNode::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pfUseOpenCloseImages == NULL )
	{
		return E_POINTER;
	}

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetRightClickMenuId

HRESULT CFileNode::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	*phInstance = AfxGetResourceHandle();
	*pnMenuId   = IDM_FILE_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::OnRightClickMenuInit

HRESULT CFileNode::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMenu menu;

	if( menu.Attach(hMenu) )
	{
		// IDM_CUT
		if( CanCut() == S_OK )
		{
			menu.EnableMenuItem( IDM_CUT, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			menu.EnableMenuItem( IDM_CUT, (MF_GRAYED | MF_BYCOMMAND) );
		}
		
		// IDM_COPY
		if( CanCopy() == S_OK )
		{
			menu.EnableMenuItem( IDM_COPY, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			menu.EnableMenuItem( IDM_COPY, (MF_GRAYED | MF_BYCOMMAND) );
		}

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

		// IDM_SAVE, IDM_SAVEAS, IDM_REVERT
		if( m_pIChildNode )
		{
			CString strMenuText;

			menu.GetMenuString( IDM_SAVE, strMenuText, MF_BYCOMMAND );
			strMenuText += _T(" ");
			strMenuText += m_strName;
			menu.ModifyMenu( IDM_SAVE, (MF_STRING | MF_BYCOMMAND), IDM_SAVE, strMenuText );

			menu.EnableMenuItem( IDM_SAVE, (MF_ENABLED | MF_BYCOMMAND) );
			menu.EnableMenuItem( IDM_SAVEAS, (MF_ENABLED | MF_BYCOMMAND) );
			menu.EnableMenuItem( IDM_REVERT, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			menu.EnableMenuItem( IDM_SAVE, (MF_GRAYED | MF_BYCOMMAND) );
			menu.EnableMenuItem( IDM_SAVEAS, (MF_GRAYED | MF_BYCOMMAND) );
			menu.EnableMenuItem( IDM_REVERT, (MF_GRAYED | MF_BYCOMMAND) );
		}

		// IDM_RUNTIME_SAVEAS
		if( m_pComponentDoc )
		{
			menu.EnableMenuItem( IDM_RUNTIME_SAVEAS, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			menu.EnableMenuItem( IDM_RUNTIME_SAVEAS, (MF_GRAYED | MF_BYCOMMAND) );
		}

		menu.Detach();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::OnRightClickMenuSelect

HRESULT CFileNode::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_SAVE:
			if( m_pIChildNode )
			{
				hr = theApp.m_pFramework->SaveNode( m_pIChildNode );
			}
			break;

		case IDM_SAVEAS:
			if( m_pIChildNode )
			{
				hr = theApp.m_pFramework->SaveNodeAsNewFile( m_pIChildNode );
			}
			break;

		case IDM_REVERT:
			if( m_pIChildNode )
			{
				hr = theApp.m_pFramework->RevertFileToSaved( m_pIChildNode );
			}
			break;

		case IDM_RUNTIME_SAVEAS:
			if( m_pComponentDoc )
			{
				m_pComponentDoc->OnFileRuntimeSaveAs();
				hr = S_OK;
			}
			break;

		case IDM_CUT:
			pMainFrame->m_wndTreeBar.OnEditCut();
			hr = S_OK;
			break;

		case IDM_COPY:
			pMainFrame->m_wndTreeBar.OnEditCopy();
			hr = S_OK;
			break;

		case IDM_PASTE:
			pMainFrame->m_wndTreeBar.OnEditPaste();
			hr = S_OK;
			break;

		case IDM_RENAME:
			if( SUCCEEDED ( theApp.m_pFramework->EditNodeLabel((IDMUSProdNode *)this) ) )
			{
				hr = S_OK;
			}
			break;

		case IDM_DELETE:
			if( pMainFrame->m_wndTreeBar.DeleteTreeNode( this, DTN_PROMPT_USER ) )
			{
				hr = S_OK;
			}
			break;

		case IDM_PROPERTIES:
			OnShowProperties();
			hr = S_OK;
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::DeleteChildNode

HRESULT CFileNode::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( pIChildNode != NULL );
	ASSERT( pIChildNode == m_pIChildNode );

	// Avoid eventual recursive call to m_pIChildNode->DeleteNode
	if( theApp.m_fInDocRootDelete == FALSE )
	{
		m_pIChildNode->DeleteNode( fPromptUser );
	}

	m_pIChildNode->Release();
	m_pIChildNode = NULL;
	m_hChildItem = NULL;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::InsertChildNode

HRESULT CFileNode::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( pIChildNode != NULL );

	ReleaseDocument();

	m_pIChildNode = pIChildNode;
	m_pIChildNode->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::DeleteNode

HRESULT CFileNode::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	HRESULT hr = E_FAIL;

	// Remove FileNode
	IDMUSProdNode* pIParentNode;

	if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
	{
		if( pIParentNode )
		{
			hr = pIParentNode->DeleteChildNode( this, fPromptUser );
			pIParentNode->Release();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::OnNodeSelChanged

HRESULT CFileNode::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::CreateDataObject

HRESULT CFileNode::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pIChildNode )
	{
		return m_pIChildNode->CreateDataObject( ppIDataObject );
	}

	// FileNode will create data object containing CF_DMUSPROD_FILE
	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	// Create the CAppJazzDataObject 
	CAppJazzDataObject* pDataObject = new CAppJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	IStream* pIStream;

	// Create a stream in CF_DMUSPROD_FILE format
	if( SUCCEEDED ( theApp.m_pFramework->SaveClipFormat( theApp.m_pFramework->m_cfProducerFile, this, &pIStream ) ) )
	{
		// Place CF_DMUSPROD_FILE into CDllJazzDataObject
		if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pFramework->m_cfProducerFile, pIStream ) ) )
		{
			hr = S_OK;
		}

		pIStream->Release();
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		pDataObject->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::CanCut

HRESULT CFileNode::CanCut( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pIChildNode )
	{
		return m_pIChildNode->CanCut();
	}

	// Cannot 'Cut' file unless it has been opened
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::CanCopy

HRESULT CFileNode::CanCopy( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pIChildNode )
	{
		return m_pIChildNode->CanCopy();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::CanDelete

HRESULT CFileNode::CanDelete( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pIChildNode )
	{
		return m_pIChildNode->CanDelete();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::CanDeleteChildNode

HRESULT CFileNode::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Should not get here!
	ASSERT( 0 );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::CanPasteFromData

HRESULT CFileNode::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	
	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( m_pIChildNode )
	{
		return m_pIChildNode->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::PasteFromData

HRESULT CFileNode::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( m_pIChildNode )
	{
		return m_pIChildNode->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::CanChildPasteFromData

HRESULT CFileNode::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Should not get here!
	ASSERT( 0 );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::ChildPasteFromData

HRESULT CFileNode::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Should not get here!
	ASSERT( 0 );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdNode::GetObject

HRESULT CFileNode::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// Additional methods

/////////////////////////////////////////////////////////////////////////////
// CFileNode LinkToRuntimeFile

void CFileNode::LinkToRuntimeFile( LPCTSTR szRuntimeFileName )
{
	CString strFileName = szRuntimeFileName;
	CString strPath;
	CString strName;
	CString strExt;
	CString strRuntimeFolder;

	ASSERT( m_pProject != NULL );

	// Split path from filename
	int nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
	if( nFindPos != -1 )
	{
		strPath = strFileName.Left( nFindPos + 1 );
		strName = strFileName.Right( strFileName.GetLength() - nFindPos - 1 );
	}

	// Extract extension from filename
	nFindPos = strFileName.ReverseFind( (TCHAR)'.' );
	if( nFindPos != -1 )
	{
		strExt = strFileName.Right( strFileName.GetLength() - nFindPos );
	}
	ASSERT( !strExt.IsEmpty() );

	// Get the current runtime folder
	ConstructRuntimePath( strRuntimeFolder );

	// Set the new default runtime folder
	if( strRuntimeFolder.CompareNoCase( strPath ) != 0 )
	{
		CString strDefaultRuntimeFolder;

		m_pProject->GetDefaultRuntimeFolderByExt( strExt, strDefaultRuntimeFolder );
		if( strDefaultRuntimeFolder.CompareNoCase( strPath ) == 0 )
		{
			m_strRuntimeFolder.Empty();
		}
		else
		{
			m_strRuntimeFolder = strPath;
		}
	}

	// Set the new default runtime filename
	m_strRuntimeFileName = strName;

	// Set the new runtime file
	m_strRuntimeFile = szRuntimeFileName;

	// Refresh Property sheet
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->RefreshActivePageByObject( this );
		pIPropSheet->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode ConstructFileName

void CFileNode::ConstructFileName( CString& strFileName )
{
	strFileName.Empty();

	GUID guidNodeId;
	IDMUSProdNode* pINode;

	if( FAILED ( GetParentNode ( &pINode ) ) )
	{
		pINode = NULL;
	}

	ASSERT( pINode != NULL );

	if( pINode )
	{
		CString strPath;

		if( SUCCEEDED ( pINode->GetNodeId( &guidNodeId ) ) )
		{
			if( IsEqualGUID ( guidNodeId, GUID_ProjectFolderNode )
			||  IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
			{
				CDirectoryNode* pDirNode = (CDirectoryNode *)pINode;
				pDirNode->ConstructPath( strPath );
			}
		}

		if( !strPath.IsEmpty() )
		{
			strFileName = strPath + m_strName;
		}

		pINode->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode ConstructRuntimePath

void CFileNode::ConstructRuntimePath( CString& strRuntimePath )
{
	ASSERT( m_pProject != NULL );

	strRuntimePath.Empty();

	// See if user has selected specific folder for this file
	if( !m_strRuntimeFolder.IsEmpty() )
	{
		strRuntimePath = m_strRuntimeFolder;
		return;
	}

	// See if there is a default runtime folder for this file extension
	CString strExt;

	int nFindPos = m_strName.ReverseFind( (TCHAR)'.' );
	if( nFindPos != -1 )
	{
		strExt = m_strName.Right( m_strName.GetLength() - nFindPos );
	}
	ASSERT( !strExt.IsEmpty() );

	m_pProject->GetDefaultRuntimeFolderByExt( strExt, strRuntimePath );
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode ConstructRelativePath

BOOL CFileNode::ConstructRelativePath( CString& strRelativePath )
{
	BOOL fSuccess = FALSE;

	ASSERT( m_pProject != NULL );

	strRelativePath.Empty();

	GUID guidNodeId;
	IDMUSProdNode* pINode;

	if( FAILED ( GetParentNode ( &pINode ) ) )
	{
		pINode = NULL;
	}

	ASSERT( pINode != NULL );

	if( pINode )
	{
		if( SUCCEEDED ( pINode->GetNodeId( &guidNodeId ) ) )
		{
			if( IsEqualGUID ( guidNodeId, GUID_ProjectFolderNode )
			||  IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
			{
				CDirectoryNode* pDirNode = (CDirectoryNode *)pINode;
				pDirNode->ConstructPath( strRelativePath );
			}
		}

		if( !strRelativePath.IsEmpty() )
		{
			if( strRelativePath.Find( m_pProject->m_strProjectDir ) == 0 )
			{
				// Strip Project directory from path
				strRelativePath = strRelativePath.Right( strRelativePath.GetLength() - m_pProject->m_strProjectDir.GetLength() );

				// Remove last backslash
				if( strRelativePath.Right(1) == _T("\\") )
				{
					strRelativePath = strRelativePath.Left( strRelativePath.GetLength() - 1 );
				}
				
				fSuccess = TRUE;
			}
		}

		pINode->Release();
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode SyncListInfo

void CFileNode::SyncListInfo()
{
	// Force sync if DMUSProdListInfo fields are empty
	if( m_strListInfoName.IsEmpty()
	&&  m_strListInfoDescriptor.IsEmpty() )
	{
		m_wFlags |= FSF_DOSYNC;
	}

	// Force sync if DMUSProdListInfo object GUID is GUID_AllZeros
	if( ::IsEqualGUID(m_guidListInfoObject, GUID_AllZeros) )
	{
		m_wFlags |= FSF_DOSYNC;
	}

	// No work if we are already in sync
	if( !(m_wFlags & FSF_DOSYNC) )
	{
		return;
	}

	// Initialize DMUSProdListInfo fields
	m_strListInfoName.Empty();
	m_strListInfoDescriptor.Empty();
    memset( &m_guidListInfoObject, 0, sizeof(GUID) );
	

	// Get complete path/filename
	CString strFileName;
	ConstructFileName( strFileName );

	// Get IDMUSProdDocType* for the file
	IDMUSProdDocType* pIDocType = theApp.GetDocType( strFileName );

	if( pIDocType )
	{

		// Open the file
		IStream* pIStream;
		BSTR bstrFileName = strFileName.AllocSysString();

		if( SUCCEEDED ( theApp.m_pFramework->AllocFileStream ( bstrFileName, GENERIC_READ, FT_UNKNOWN,
															   GUID_AllZeros, m_pIParentNode, &pIStream ) ) )
		{
			DMUSProdListInfo ListInfo;

			ZeroMemory( &ListInfo, sizeof(ListInfo) );
			ListInfo.wSize = sizeof(ListInfo);

			if( SUCCEEDED ( pIDocType->GetListInfo ( pIStream, &ListInfo ) ) )
			{
				if( ListInfo.bstrName )
				{
					m_strListInfoName = ListInfo.bstrName;
					::SysFreeString( ListInfo.bstrName );

				}

				if( ListInfo.bstrDescriptor )
				{
					m_strListInfoDescriptor = ListInfo.bstrDescriptor;
					::SysFreeString( ListInfo.bstrDescriptor );

				}

				memcpy( &m_guidListInfoObject, &ListInfo.guidObject, sizeof(GUID) );
			}

			m_wFlags &= ~FSF_DOSYNC;

			pIStream->Release();
		}

		pIDocType->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode MoveWPListToNodes

void CFileNode::MoveWPListToNodes( void )
{
	// Value in m_pIChildNode means file has been loaded
	ASSERT( m_pIChildNode != NULL );

	wpWindowPlacement* pWP;
	HTREEITEM hItem;

	// Empty WP list
	while( !m_lstWP.IsEmpty() )
	{
		pWP = static_cast<wpWindowPlacement*>( m_lstWP.RemoveHead() );

		// Find the corresponding node in the Project Tree
		hItem = theApp.m_pFramework->FindTreeItemByWP( pWP );
		if( hItem )
		{
			// Apply WP Settings to the node
			theApp.m_pFramework->ApplyWPSettings( hItem, pWP );
		}

		delete pWP;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode EmptyNotifyNodesList

void CFileNode::EmptyNotifyNodesList()
{
	CJzNotifyNode* pJzNotifyNode;

	while( !m_lstNotifyNodes.IsEmpty() )
	{
		pJzNotifyNode = static_cast<CJzNotifyNode*>( m_lstNotifyNodes.RemoveHead() );
		delete( pJzNotifyNode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode ReleaseDocument

void CFileNode::ReleaseDocument()
{
	// Remove FileNode's child (associated document)
	if( m_pIChildNode )
	{
		if( m_pComponentDoc )
		{
			m_pComponentDoc->OnCloseDocument();		// No Save - Unconditional close!!!!!!!

			CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
			if( pMainFrame )
			{
				pMainFrame->RecalcLayout( TRUE );
			}
		}

		DeleteChildNode( m_pIChildNode, FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IsProducerFile

BOOL CFileNode::IsProducerFile( void )
{
	if( m_pIChildNode )
	{
		return TRUE;
	}

	IDMUSProdDocType* pIDocType = theApp.GetDocType( m_strName );

	if( pIDocType )
	{
		pIDocType->Release();
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode::SaveTreeNodesWP

HRESULT CFileNode::SaveTreeNodesWP( IDMUSProdRIFFStream* pIRiffStream, CTreeCtrl* pTreeCtrl, HTREEITEM hItem ) 
{
	HRESULT hr = S_OK;
	HTREEITEM hChildItem;
	CJzNode* pJzNode;
	wpWindowPlacement WP;

	ASSERT( pIRiffStream != NULL );
	ASSERT( pTreeCtrl != NULL );
	ASSERT( hItem != NULL );

	while( hItem )
	{
		pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			ASSERT( pJzNode->m_pINode != NULL );

			if( pJzNode->m_wp.length != 0 )
			{
				// Get Node Name
				BSTR bstrNodeName;
				if( SUCCEEDED ( pJzNode->m_pINode->GetNodeName ( &bstrNodeName ) ) )
				{
					WP.strNodeName = bstrNodeName;
					::SysFreeString( bstrNodeName );

					// Get Node ID
					if( SUCCEEDED ( pJzNode->m_pINode->GetNodeId ( &WP.guidNodeId ) ) )
					{
						memcpy( &WP.guidFile, &m_guid, sizeof( WP.guidFile ) );
						WP.lTreePos = theApp.m_pFramework->GetWPTreePos( pJzNode->m_pINode );

						WP.wp.length = sizeof(WP.wp);
						WP.wp.flags = pJzNode->m_wp.flags;
						WP.wp.showCmd = pJzNode->m_wp.showCmd;
						WP.wp.ptMinPosition = pJzNode->m_wp.ptMinPosition;
						WP.wp.ptMaxPosition = pJzNode->m_wp.ptMaxPosition;
						WP.wp.rcNormalPosition = pJzNode->m_wp.rcNormalPosition;

						hr = SaveNodeWP( pIRiffStream, &WP );		
					}
				}
			}
		}

		if( FAILED ( hr ) )
		{
			break;
		}

		// Handle Node's children
		hChildItem = hItem;
		while( hChildItem = pTreeCtrl->GetNextItem(hChildItem, TVGN_CHILD) )
		{
			hr = SaveTreeNodesWP( pIRiffStream, pTreeCtrl, hChildItem );
			if( FAILED ( hr ) )
			{
				break;
			}
		}

		if( FAILED ( hr ) )
		{
			break;
		}

		hItem = pTreeCtrl->GetNextItem( hItem, TVGN_NEXT );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode SaveTheFile

HRESULT CFileNode::SaveTheFile( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	POSITION pos;
	MMCKINFO ck;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write File list header
	ck.fccType = FOURCC_FILE_LIST;
	if( pIRiffStream->CreateChunk(&ck, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save Filename chunk
	hr = SaveFileNameChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save File chunk
	hr = SaveFileChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save File's notify list
	if( m_lstNotifyNodes.GetCount() > 0 )
	{
		hr = SaveNotifyNodesList( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

	// Save File info
	hr = SaveFileInfo( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save WindowPlacement information for nodes in Project Tree
	if( m_hChildItem )
	{
		CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
		if( pTreeCtrl )
		{
			hr = SaveTreeNodesWP( pIRiffStream, pTreeCtrl, m_hChildItem );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
	}
	else
	{
		wpWindowPlacement* pWP;

		pos = m_lstWP.GetHeadPosition();
		while( pos )
		{
			pWP = m_lstWP.GetNext( pos );

			hr = SaveNodeWP( pIRiffStream, pWP );		
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}



/////////////////////////////////////////////////////////////////////////////
// CFileNode SaveFileNameChunk

HRESULT CFileNode::SaveFileNameChunk( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	CString strRelativePath;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Get the file's relative path
	if( ConstructRelativePath( strRelativePath ) )
	{
		if( !strRelativePath.IsEmpty() )
		{
			strRelativePath += _T("\\");
		}
		strRelativePath += m_strName;
	}
	if( strRelativePath.IsEmpty() )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write filename (relative path)
	ck.ckid = FOURCC_NAME_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	hr = SaveMBStoWCS( pIStream, &strRelativePath );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CFileNode SaveFileChunk

HRESULT CFileNode::SaveFileChunk( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	HANDLE hFile;
	CString strFileName;
	ioJzFile oJzFile;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Get the file's complete path/filename
	ConstructFileName( strFileName );
	if( strFileName.IsEmpty() )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write File chunk header
	ck.ckid = FOURCC_FILE_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioJzFile structure
	memset( &oJzFile, 0, sizeof(ioJzFile) );
	memcpy( &oJzFile.m_guidFile, &m_guid, sizeof(GUID) );

	// Get the DMUSProdListInfo's guidObject field
	DMUSProdListInfo TreeListInfo;
	ZeroMemory( &TreeListInfo, sizeof(TreeListInfo) );
	TreeListInfo.wSize = sizeof(TreeListInfo);
	GetNodeListInfo ( &TreeListInfo );
	if( TreeListInfo.bstrName )
	{
		::SysFreeString( TreeListInfo.bstrName );
	}
	if( TreeListInfo.bstrDescriptor )
	{
		::SysFreeString( TreeListInfo.bstrDescriptor );
	}
	memcpy( &oJzFile.m_guidListInfoObject, &TreeListInfo.guidObject, sizeof(GUID) );

	// Get file information
    hFile = ::CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						  FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		//		File size
		DWORD dwFileSize = GetFileSize( hFile, NULL );
		if( dwFileSize !=  0xFFFFFFFF )
		{
			oJzFile.m_dwSize = dwFileSize;
		}

		//		Modified
		FILETIME ftModified;

		if( ::GetFileTime(hFile, NULL, NULL, &ftModified) )
		{
			oJzFile.m_ftModified = ftModified;
		}

		CloseHandle( hFile );
	}

	// Write File chunk data
	hr = pIStream->Write( &oJzFile, sizeof(ioJzFile), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioJzFile) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode SaveFileInfo
	
HRESULT CFileNode::SaveFileInfo( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;
	CString strText;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write INFO LIST header
	ckMain.fccType = FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write name of most recently saved Runtime file
	if( !m_strRuntimeFile.IsEmpty() )
	{
		ck.ckid = FOURCC_UNFO_RUNTIME_FILE;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strRuntimeFile );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write folder used when saving Runtime file
	if( !m_strRuntimeFolder.IsEmpty() )
	{
		ck.ckid = FOURCC_UNFO_RUNTIME_FOLDER;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strRuntimeFolder );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write name used when saving Runtime file
	if( !m_strRuntimeFileName.IsEmpty() )
	{
		ck.ckid = FOURCC_UNFO_RUNTIME_NAME;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strRuntimeFileName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Fill the DMUSProdListInfo structure
	DMUSProdListInfo TreeListInfo;

	ZeroMemory( &TreeListInfo, sizeof(TreeListInfo) );
	TreeListInfo.wSize = sizeof(TreeListInfo);
	GetNodeListInfo ( &TreeListInfo );

	// Write DocRoot node name
	if( TreeListInfo.bstrName )
	{
		strText = TreeListInfo.bstrName;
		::SysFreeString( TreeListInfo.bstrName );

		if( !strText.IsEmpty() )
		{
			ck.ckid = FOURCC_UNFO_NODE_NAME;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			hr = SaveMBStoWCS( pIStream, &strText );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}
	}

	// Write DocRoot node descriptor
	if( TreeListInfo.bstrDescriptor )
	{
		strText = TreeListInfo.bstrDescriptor;
		::SysFreeString( TreeListInfo.bstrDescriptor );

		if( !strText.IsEmpty() )
		{
			ck.ckid = FOURCC_UNFO_NODE_DESC;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			hr = SaveMBStoWCS( pIStream, &strText );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode SaveNotifyNodesList
	
HRESULT CFileNode::SaveNotifyNodesList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	CJzNotifyNode* pJzNotifyNode;
	POSITION pos;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write File chunk header
	ck.ckid = FOURCC_NOTIFY_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write each GUID in the notify list
	pos = m_lstNotifyNodes.GetHeadPosition();
	while( pos )
	{
		pJzNotifyNode = static_cast<CJzNotifyNode*>( m_lstNotifyNodes.GetNext(pos) );

		hr = pIStream->Write( &pJzNotifyNode->m_guidFile, sizeof(GUID), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(GUID) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode SaveNodeWP
	
HRESULT CFileNode::SaveNodeWP( IDMUSProdRIFFStream* pIRiffStream, wpWindowPlacement* pWP )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ckMain;
	MMCKINFO ck;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Node list header
	ckMain.fccType = FOURCC_NODE_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save Node name
	ck.ckid = FOURCC_NAME_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	hr = SaveMBStoWCS( pIStream, &pWP->strNodeName );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save Node chunk
	hr = SaveNodeWPChunk( pIRiffStream, pWP );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ckMain, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode SaveNodeWPChunk
	
HRESULT CFileNode::SaveNodeWPChunk( IDMUSProdRIFFStream* pIRiffStream, wpWindowPlacement* pWP )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	ioJzNode oJzNode;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Node chunk header
	ck.ckid = FOURCC_EDITOR_WP_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioJzNode structure
	memset( &oJzNode, 0, sizeof(ioJzNode) );

	memcpy( &oJzNode.m_guidNodeId, &pWP->guidNodeId, sizeof(GUID) );
	oJzNode.m_lTreePos = pWP->lTreePos;
	oJzNode.m_wpEditor.length = sizeof(oJzNode.m_wpEditor);
	oJzNode.m_wpEditor.flags = pWP->wp.flags;
	oJzNode.m_wpEditor.showCmd = pWP->wp.showCmd;
	oJzNode.m_wpEditor.ptMinPosition = pWP->wp.ptMinPosition;
	oJzNode.m_wpEditor.ptMaxPosition = pWP->wp.ptMaxPosition;
	oJzNode.m_wpEditor.rcNormalPosition = pWP->wp.rcNormalPosition;

	// Write Node chunk data
	hr = pIStream->Write( &oJzNode, sizeof(ioJzNode), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioJzNode) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode LoadNodeWP

HRESULT CFileNode::LoadNodeWP( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, wpWindowPlacement* pWP )
{
    IStream* pIStream;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;
    HRESULT hr = E_FAIL;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_NAME_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &pWP->strNodeName );
				break;

			case FOURCC_EDITOR_WP_CHUNK:
			{
				ioJzNode iJzNode;

				dwSize = min( ck.cksize, sizeof( ioJzNode ) );
				hr = pIStream->Read( &iJzNode, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				memcpy( &pWP->guidFile, &m_guid, sizeof(GUID) );
				memcpy( &pWP->guidNodeId, &iJzNode.m_guidNodeId, sizeof(GUID) );
				pWP->lTreePos = iJzNode.m_lTreePos;
				pWP->wp.length = sizeof(pWP->wp);
				pWP->wp.flags = iJzNode.m_wpEditor.flags;
				pWP->wp.showCmd = iJzNode.m_wpEditor.showCmd;
				pWP->wp.ptMinPosition = iJzNode.m_wpEditor.ptMinPosition;
				pWP->wp.ptMaxPosition = iJzNode.m_wpEditor.ptMaxPosition;
				pWP->wp.rcNormalPosition = iJzNode.m_wpEditor.rcNormalPosition;
				break;
			}
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode LoadTheFile

HRESULT CFileNode::LoadTheFile( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream* pIStream;
	MMCKINFO ck;
	MMCKINFO ckList;
	DWORD dwByteCount;
	DWORD dwSize;
	HANDLE hFile;
    HRESULT hr = E_FAIL;

	ASSERT( m_pProject != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Read through the chunks in the File list
    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_NAME_CHUNK:
			{
				CString strName;
				BSTR bstrName;

				ReadMBSfromWCS( pIStream, ck.cksize, &strName );
				bstrName = strName.AllocSysString();
				SetNodeName( bstrName );
				break;
			}

			case FOURCC_FILE_CHUNK:
			{
				DWORD dwFileSize;
				FILETIME ftModified;
			    ioJzFile iJzFile;
				CString strFileName;

				dwSize = min( ck.cksize, sizeof( ioJzFile ) );
				hr = pIStream->Read( &iJzFile, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				memcpy( &m_guid, &iJzFile.m_guidFile, sizeof(GUID) );
				memcpy( &m_guidListInfoObject, &iJzFile.m_guidListInfoObject, sizeof(GUID) );

				// Get file information
				strFileName = m_pProject->m_strProjectDir + m_strName;
				hFile = ::CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
									  FILE_ATTRIBUTE_NORMAL, NULL );
				if( hFile != INVALID_HANDLE_VALUE )
				{
					dwFileSize = GetFileSize( hFile, NULL );
					::GetFileTime( hFile, NULL, NULL, &ftModified );
					CloseHandle( hFile );
				}
				else
				{
					dwFileSize = 0;
					ftModified.dwLowDateTime = 0;
					ftModified.dwHighDateTime = 0;
				}

				// Determine if file DMUSProdListInfo is out of date
				if( dwFileSize != iJzFile.m_dwSize
				||  ftModified.dwLowDateTime != iJzFile.m_ftModified.dwLowDateTime
				||  ftModified.dwHighDateTime != iJzFile.m_ftModified.dwHighDateTime )
				{
					m_wFlags |= FSF_DOSYNC;
				}
				break;
			}

			case FOURCC_NOTIFY_CHUNK:
			{
				CJzNotifyNode* pJzNotifyNode;

				dwSize = ck.cksize;
				while( dwSize )
				{
					pJzNotifyNode = new CJzNotifyNode;
					if( pJzNotifyNode == NULL )
					{
						hr = E_OUTOFMEMORY;
						goto ON_ERROR;
					}

					hr = pIStream->Read( &pJzNotifyNode->m_guidFile, sizeof(GUID), &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != sizeof(GUID) )
					{
						goto ON_ERROR;
					}

					// Bypass if this file is in another Project and is already open.
					if( theApp.IsFileOpenInDiffProject( pJzNotifyNode->m_guidFile, m_pProject ) )
					{
						delete pJzNotifyNode;
					}
					else
					{
						// At this point use count is zero
						m_lstNotifyNodes.AddTail( pJzNotifyNode );
					}

					dwSize -= sizeof(GUID);
					if( (dwSize > 0)
					&&  (dwSize < sizeof(GUID)) )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}
				}
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case FOURCC_INFO_LIST:
					case FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case FOURCC_UNFO_NODE_NAME:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strListInfoName );
									break;

								case FOURCC_UNFO_NODE_DESC:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strListInfoDescriptor );
									break;

								case FOURCC_UNFO_RUNTIME_FILE:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strRuntimeFile );
									break;

								case FOURCC_UNFO_RUNTIME_FOLDER:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strRuntimeFolder );
									break;

								case FOURCC_UNFO_RUNTIME_NAME:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strRuntimeFileName );
									break;
							}
							pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case FOURCC_NODE_LIST:
					{
						wpWindowPlacement* pWP;

						pWP = new wpWindowPlacement;
						if( pWP == NULL )
						{
							hr = E_OUTOFMEMORY ;
			                goto ON_ERROR;
						}

						hr = LoadNodeWP( pIRiffStream, &ck, pWP );
						if( FAILED( hr ) )
						{
							delete pWP;
			                goto ON_ERROR;
						}
						
						m_lstWP.AddTail( pWP );
						break;
					}
				}
				break;
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdPropPageObject::GetData

HRESULT CFileNode::GetData( void** ppData )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	*ppData = this;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdPropPageObject::SetData

HRESULT CFileNode::SetData( void* pData )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CFileNode::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode IDMUSProdPropPageObject::OnShowProperties

HRESULT CFileNode::OnShowProperties( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Get the Project page manager
	CFilePropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_FilePropPageManager ) == S_OK )
	{
		pPageManager = (CFilePropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CFilePropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the FileNode properties
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		short nActiveTab = CFilePropPageManager::sm_nActiveTab;

		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode AddToNotifyList

void CFileNode::AddToNotifyList( IDMUSProdNode* pINotifyThisNode )
{
	ASSERT( pINotifyThisNode != NULL );
	if( pINotifyThisNode == NULL )
	{
		return;
	}

	CJzNotifyNode* pJzNotifyNode;

	// Just increment m_nUseCount if pINotifyThisNode already in list
	POSITION pos = m_lstNotifyNodes.GetHeadPosition();
	while( pos )
	{
		pJzNotifyNode = static_cast<CJzNotifyNode*>( m_lstNotifyNodes.GetNext(pos) );

		if( pJzNotifyNode->m_pINotifyThisNode == pINotifyThisNode )
		{
			pJzNotifyNode->m_nUseCount++;
			return;
		}
	}

	// Get the CFileNode associated with pINotifyThisNode
	CFileNode* pNotifyFileNode = NULL;
	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINotifyThisNode );
	if( pComponentDoc )
	{
		pNotifyFileNode = pComponentDoc->m_pFileNode;
	}

	// Create a new CJzNotifyNode
	pJzNotifyNode = new CJzNotifyNode;
	if( pJzNotifyNode )
	{
		pJzNotifyNode->m_pINotifyThisNode = pINotifyThisNode;
		pJzNotifyNode->m_pINotifyThisNode->AddRef();
		pJzNotifyNode->m_nUseCount++;

		if( pNotifyFileNode )
		{
			memcpy( &pJzNotifyNode->m_guidFile, &pNotifyFileNode->m_guid, sizeof(pNotifyFileNode->m_guid) );
		}

		m_lstNotifyNodes.AddTail( pJzNotifyNode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileNode RemoveFromNotifyList

void CFileNode::RemoveFromNotifyList( IDMUSProdNode* pINotifyThisNode )
{
	ASSERT( pINotifyThisNode != NULL );
	if( pINotifyThisNode == NULL )
	{
		return;
	}

	CJzNotifyNode* pJzNotifyNode;

	POSITION pos1 = m_lstNotifyNodes.GetHeadPosition();
	while( pos1 )
	{
		pJzNotifyNode = static_cast<CJzNotifyNode*>( m_lstNotifyNodes.GetNext(pos1) );

		if( pJzNotifyNode->m_pINotifyThisNode == pINotifyThisNode )
		{
			pJzNotifyNode->m_nUseCount--;
				
			if( pJzNotifyNode->m_nUseCount <= 0 )
			{
				POSITION pos2 = m_lstNotifyNodes.Find( pJzNotifyNode );
				if( pos2 )
				{
					m_lstNotifyNodes.RemoveAt( pos2 );
				}

				delete pJzNotifyNode;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFileNode AddToNotifyWhenLoadFinished

void CFileNode::AddToNotifyWhenLoadFinished( IDMUSProdNotifySink* pINotifySink )
{
	ASSERT( pINotifySink != NULL );

	if( pINotifySink )
	{
		pINotifySink->AddRef();
		m_lstNotifyWhenLoadFinished.AddTail( pINotifySink );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFileNode SendLoadFinishedNotifications

void CFileNode::SendLoadFinishedNotifications( void )
{
	ASSERT( m_pIChildNode != NULL );

	while( !m_lstNotifyWhenLoadFinished.IsEmpty() )
	{
		IDMUSProdNotifySink* pINotifySink = m_lstNotifyWhenLoadFinished.RemoveHead();

		pINotifySink->OnUpdate( m_pIChildNode, FRAMEWORK_FileLoadFinished, &m_guid );  
		pINotifySink->Release();
	}
}
