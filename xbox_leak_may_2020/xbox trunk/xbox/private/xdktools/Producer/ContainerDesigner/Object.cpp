// Object.cpp : implementation file
//

#include "stdafx.h"
#include "ContainerDesignerDLL.h"

#include "Container.h"
#include "Object.h"
#include <mmreg.h>
#include "TabObjectDesign.h"
#include "TabObjectRuntime.h"
#include "TabObjectFlags.h"
#include <ScriptDesigner.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// {637FD240-8C9D-11d3-B473-00105A2796DE}
static const GUID GUID_ObjectPropPageManager = 
{ 0x637fd240, 0x8c9d, 0x11d3, { 0xb4, 0x73, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde } };

short CObjectPropPageManager::sm_nActiveTab = 0;


//////////////////////////////////////////////////////////////////////
// CObjectPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectPropPageManager::CObjectPropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabDesign = NULL;
	m_pTabRuntime = NULL;
	m_pTabFlags = NULL;
}

CObjectPropPageManager::~CObjectPropPageManager()
{
	RELEASE( m_pIPropSheet );

	if( m_pTabDesign )
	{
		delete m_pTabDesign;
	}

	if( m_pTabRuntime )
	{
		delete m_pTabRuntime;
	}

	if( m_pTabFlags )
	{
		delete m_pTabFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageManager::RemoveCurrentObject

void CObjectPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageManager IUnknown implementation

HRESULT CObjectPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageManager)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageManager *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CObjectPropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CObjectPropPageManager::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CObjectPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CObjectPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_CONTAINER_TEXT );

	CContainerObject *pContainerObject;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pContainerObject))) )
	{
		CString strName;
		BSTR bstrName;

		strTitle += _T(": ");

		// Add name of Container
		if( pContainerObject->m_pContainer )
		{
			if( SUCCEEDED ( pContainerObject->m_pContainer->GetNodeName( &bstrName ) ) )
			{
				strName = bstrName;
				::SysFreeString( bstrName );
		
				strTitle += strName;
				strTitle += _T(" - ");
			}
		}

		// Add Name of Object
		if( SUCCEEDED ( pContainerObject->GetNodeName( &bstrName ) ) )
		{
			strName = bstrName;
			::SysFreeString( bstrName );
		
			strTitle += _T("'");
			strTitle += strName;
			strTitle += _T("' ");
		}

		// Add type of Object
		RegisteredObject* pRegisteredObject;
		pRegisteredObject = theApp.m_pContainerComponent->FindRegisteredObjectByDocRoot( pContainerObject->m_FileRef.pIDocRootNode );
		if( pRegisteredObject )
		{
			strTitle += pRegisteredObject->strObjectType;
		}
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CObjectPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	// Add Design tab
	m_pTabDesign = new CTabObjectDesign( this );
	if( m_pTabDesign )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabDesign->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Runtime tab
	m_pTabRuntime = new CTabObjectRuntime( this );
	if( m_pTabRuntime )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabRuntime->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Cache tab
	m_pTabFlags = new CTabObjectFlags( this );
	if( m_pTabFlags )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabFlags->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CObjectPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CObjectPropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CObjectPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}
	if( m_pIPropPageObject == pINewPropPageObject )
	{
		return S_OK;
	}

	RemoveCurrentObject();

	m_pIPropPageObject = pINewPropPageObject;
//	m_pIPropPageObject->AddRef();		intentionally missing

	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CObjectPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pIPropPageObject == NULL)
	||  (pIPropPageObject != m_pIPropPageObject) )
	{
		return E_INVALIDARG;
	}

	RemoveCurrentObject();
	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CObjectPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( pIPropPageObject == m_pIPropPageObject )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CObjectPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CContainerObject* pContainerObject;
	
	if( m_pIPropPageObject == NULL )
	{
		pContainerObject = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pContainerObject ) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current Container Object are processed in OnKillFocus
	// messages before setting the new Container Object
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabDesign->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new Container Object
	m_pTabDesign->SetContainerObject( pContainerObject);
	m_pTabRuntime->SetContainerObject( pContainerObject);
	m_pTabFlags->SetContainerObject( pContainerObject);

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CObjectPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_ObjectPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	CContainerObject Constructor/Destructor

CContainerObject::CContainerObject( CDirectMusicContainer* pContainer )
{
	m_dwRef = 0;
	AddRef();

	ASSERT( pContainer != NULL );
	m_pContainer = pContainer;
//	m_pContainer->AddRef();			intentionally missing

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;

	SetFileReference( NULL );
	m_dwFlagsDM = 0;
}

CContainerObject::~CContainerObject( void )
{
	// Remove Container Objectfrom property sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}

	SetFileReference( NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject::SetModifiedFlag

void CContainerObject::SetModifiedFlag( void ) 
{
	ASSERT( m_pContainer != NULL );

	m_pContainer->SetModified( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IUnknown implementation

HRESULT CContainerObject::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
        return S_OK;
    }
    else if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageObject *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CContainerObject::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CContainerObject::Release()
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
// CContainerObject IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetNodeImageIndex

HRESULT CContainerObject::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_FileRef.pIRefNode == NULL )
	{
		// Will happen while waiting for FRAMEWORK_FileLoadFinished notification
		return E_UNEXPECTED;
	}

	return( m_FileRef.pIRefNode->GetNodeImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetFirstChild

HRESULT CContainerObject::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// ContainerObject node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetNextChild

HRESULT CContainerObject::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// ContainerObject node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetComponent

HRESULT CContainerObject::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );

	return theApp.m_pContainerComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetDocRootNode

HRESULT CContainerObject::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CContainerObject IDMUSProdNode::SetDocRootNode

HRESULT CContainerObject::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetParentNode

HRESULT CContainerObject::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::SetParentNode

HRESULT CContainerObject::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetNodeId

HRESULT CContainerObject::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_ContainerObjectNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetNodeName

HRESULT CContainerObject::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	if( m_pContainer->IsInScript() )
	{
		ASSERT( m_FileRef.strScriptAlias.IsEmpty() == FALSE );
		*pbstrName = m_FileRef.strScriptAlias.AllocSysString();
		hr = S_OK;
	}
	else
	{
		if( m_FileRef.pIRefNode == NULL )
		{
			TCHAR achNoObject[MID_BUFFER];

			::LoadString( theApp.m_hInstance, IDS_EMPTY_TEXT, achNoObject, MID_BUFFER );
			CString strNoObject = achNoObject;
			*pbstrName = strNoObject.AllocSysString();
			hr = S_OK;
		}
		else
		{
			hr = m_FileRef.pIRefNode->GetNodeName( pbstrName );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetNodeNameMaxLength

HRESULT CContainerObject::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pContainer->IsInScript() )
	{
		// Can rename an Object if it belongs to a Container in a Script
		// The Object name is used as the "Alias" for scripting
		*pnMaxLength = DMUS_MAX_NAME;	
	}
	else
	{
		// Can't rename an Object in a Contaner
		*pnMaxLength = -1;				
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::ValidateNodeName

HRESULT CContainerObject::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strAlias = bstrName;
	::SysFreeString( bstrName );

	if( m_pContainer->IsInScript() == FALSE )
	{
		// Can't rename an Object in a Container
		ASSERT( 0 );	// This should not happen
		return S_FALSE;
	}

	HRESULT hr = S_OK;

	// Store original alias
	CString strOrigAlias = m_FileRef.strScriptAlias;

	m_FileRef.strScriptAlias = strAlias;

	// Make sure new alias starts with a letter
	if( hr == S_OK )
	{
		if( !(_istalpha( strAlias[0] )) )
		{
			CString strMsg;

			AfxFormatString1( strMsg, IDS_INVALID_OBJECT_ALIAS, strAlias );
			AfxMessageBox( strMsg, MB_OK );
			hr = S_FALSE;
		}
	}

	// Make sure new alias contains valid characters
	if( hr == S_OK )
	{
		for( int i = 0 ;  i < strAlias.GetLength() ;  i++ )
		{
			if( !(_istalpha( strAlias[i] ))
			&&  !(_istdigit( strAlias[i] ))
			&&  !(strAlias[i] == _T('_')) )
			{
				CString strMsg;

				AfxFormatString1( strMsg, IDS_INVALID_OBJECT_ALIAS, strAlias );
				AfxMessageBox( strMsg, MB_OK );
				hr = S_FALSE;
			}
		}
	}

	// Make sure new alias is unique
	if( hr == S_OK )
	{
		if( m_pContainer->IsScriptAliasUnique( this ) == FALSE )
		{
			CString strMsg;

			AfxFormatString1( strMsg, IDS_DUPE_OBJECT_ALIAS, m_FileRef.strScriptAlias );
			AfxMessageBox( strMsg, MB_OK );
			hr = S_FALSE;
		}
	}

	// Put back original alias
	m_FileRef.strScriptAlias = strOrigAlias;

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::SetNodeName

HRESULT CContainerObject::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strAlias = bstrName;
	::SysFreeString( bstrName );

	if( m_pContainer->IsInScript() == FALSE )
	{
		// Can't rename an Object in a Container
		ASSERT( 0 );	// This should not happen
		return S_FALSE;
	}

	if( strAlias.Compare( m_FileRef.strScriptAlias ) != 0 )
	{
		m_pContainer->m_pUndoMgr->SaveState( m_pContainer, theApp.m_hInstance, IDS_UNDO_OBJECT_ALIAS );

		CString strOrigName = m_FileRef.strScriptAlias;
		m_FileRef.strScriptAlias = strAlias;

		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			pIPropSheet->RefreshTitleByObject( this );
			pIPropSheet->RefreshActivePageByObject( this );
			RELEASE( pIPropSheet );
		}

		SetModifiedFlag();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetNodeListInfo

HRESULT CContainerObject::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_FileRef.pIRefNode != NULL );
	if( m_FileRef.pIRefNode == NULL )
	{
		return E_UNEXPECTED;
	}

	return( m_FileRef.pIRefNode->GetNodeListInfo(pListInfo) );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetEditorClsId

HRESULT CContainerObject::GetEditorClsId( CLSID* pClsId )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
//
//	ASSERT( m_FileRef.pIRefNode != NULL );
//	if( m_FileRef.pIRefNode == NULL )
//	{
//		return E_UNEXPECTED;
//	}
//
//	return( m_FileRef.pIRefNode->GetEditorClsId(pClsId) );
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetEditorTitle

HRESULT CContainerObject::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_FileRef.pIRefNode != NULL );
	if( m_FileRef.pIRefNode == NULL )
	{
		return E_UNEXPECTED;
	}

	return( m_FileRef.pIRefNode->GetEditorTitle(pbstrTitle) );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetEditorWindow

HRESULT CContainerObject::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_FileRef.pIRefNode  )
	{
		return( m_FileRef.pIRefNode->GetEditorWindow(hWndEditor) );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::SetEditorWindow

HRESULT CContainerObject::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_FileRef.pIRefNode != NULL );
	if( m_FileRef.pIRefNode == NULL )
	{
		return E_UNEXPECTED;
	}

	return( m_FileRef.pIRefNode->SetEditorWindow(hWndEditor) );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::UseOpenCloseImages

HRESULT CContainerObject::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetRightClickMenuId

HRESULT CContainerObject::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;

	if( m_pContainer->IsInScript() )
	{
		*pnMenuId = IDM_SCRIPT_OBJECT_NODE_RMENU;
	}
	else
	{
		*pnMenuId = IDM_OBJECT_NODE_RMENU;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::OnRightClickMenuInit

HRESULT CContainerObject::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CMenu menu;

	if( menu.Attach(hMenu) )
	{
		// IDM_DELETE
		if( CanDelete() == S_OK )
		{
			menu.EnableMenuItem( IDM_DELETE, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			menu.EnableMenuItem( IDM_DELETE, (MF_GRAYED | MF_BYCOMMAND) );
		}

		menu.Detach();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::OnRightClickMenuSelect

HRESULT CContainerObject::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_LOCATE:
			if( m_FileRef.pIDocRootNode )
			{
				theApp.m_pContainerComponent->m_pIFramework8->SetSelectedNode( m_FileRef.pIDocRootNode );
			}
			break;

		case IDM_DELETE:
			DeleteNode( TRUE );
			break;

		case IDM_OBJECT_ALIAS:
			if( m_pContainer->IsInScript() )
			{
				theApp.m_pContainerComponent->m_pIFramework8->EditNodeLabel( this );
			}
			break;

		case IDM_PROPERTIES:
			OnShowProperties();
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::DeleteChildNode

HRESULT CContainerObject::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// ContainerObject nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::InsertChildNode

HRESULT CContainerObject::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// ContainerObject nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::DeleteNode

HRESULT CContainerObject::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Let our parent delete us
	return m_pIParentNode->DeleteChildNode( this, fPromptUser );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::OnNodeSelChanged

HRESULT CContainerObject::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::CreateDataObject

HRESULT CContainerObject::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_FileRef.pIRefNode )
	{
		return m_FileRef.pIRefNode->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::CanCut

HRESULT CContainerObject::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::CanCopy

HRESULT CContainerObject::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_FileRef.pIRefNode )
	{
		return m_FileRef.pIRefNode->CanCopy();	// Will copy the Container
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::CanDelete

HRESULT CContainerObject::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::CanDeleteChildNode

HRESULT CContainerObject::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Container Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::CanPasteFromData

HRESULT CContainerObject::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Let parent decide whether it wants to paste
	return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::PasteFromData

HRESULT CContainerObject::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Let parent handle pasting of Container
	return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::CanChildPasteFromData

HRESULT CContainerObject::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												 BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// Container Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::ChildPasteFromData

HRESULT CContainerObject::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// Container Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdNode::GetObject

HRESULT CContainerObject::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdPropPageObject::GetData

HRESULT CContainerObject::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	*ppData = this;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdPropPageObject::SetData

HRESULT CContainerObject::SetData( void* pData )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdPropPageObject::OnShowProperties

HRESULT CContainerObject::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	// Get the Container Object page manager
	CObjectPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_ObjectPropPageManager ) == S_OK )
	{
		pPageManager = (CObjectPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CObjectPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Container Object's properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CObjectPropPageManager::sm_nActiveTab;

		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		RELEASE( pIPropSheet );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CContainerObject::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
//	CContainerObject::GetScriptAlias

void CContainerObject::GetScriptAlias( CString& strScriptAlias )
{
	strScriptAlias = m_FileRef.strScriptAlias;
}


//////////////////////////////////////////////////////////////////////
//	CContainerObject::SetScriptAlias

void CContainerObject::SetScriptAlias( const LPCTSTR pszScriptAlias )
{
	m_FileRef.strScriptAlias = pszScriptAlias;
}


//////////////////////////////////////////////////////////////////////
//	CContainerObject::ClearListInfo

void CContainerObject::ClearListInfo()
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( theApp.m_hInstance );

	m_FileRef.li.pIProject = NULL;

	m_FileRef.li.strProjectName.LoadString( IDS_EMPTY_TEXT );
	m_FileRef.li.strName.LoadString( IDS_EMPTY_TEXT );
	m_FileRef.li.strDescriptor.LoadString( IDS_EMPTY_TEXT );

	memset( &m_FileRef.li.guidFile, 0, sizeof(GUID) );

	AfxSetResourceHandle( hInstance );
}


//////////////////////////////////////////////////////////////////////
//	CContainerObject::SetFileReference

HRESULT CContainerObject::SetFileReference( IDMUSProdNode* pINewDocRootNode )
{
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	HRESULT hr = S_OK;

	// Clean up old file reference
	if( m_FileRef.pIDocRootNode )
	{
		// Turn off notifications for this node
		if( m_FileRef.fRemoveNotify )
		{
			ASSERT( m_pContainer != NULL );
			if( m_pContainer )
			{
				hr = theApp.m_pContainerComponent->m_pIFramework8->RemoveFromNotifyList( m_FileRef.pIDocRootNode, m_pContainer );
			}
		}
	}

	// Initialize pertinent fields
	RELEASE( m_FileRef.pIDocRootNode );
	RELEASE( m_FileRef.pIRefNode );
	m_FileRef.strScriptAlias.Empty();
	m_FileRef.fRemoveNotify = FALSE;
	ClearListInfo();

	// Set DocRoot of new file reference
	if( pINewDocRootNode )
	{
		IDMUSProdNode* pINewRefNode;

		hr = theApp.m_pContainerComponent->CreateRefNode( pINewDocRootNode, &pINewRefNode );
		if( SUCCEEDED ( hr ) )
		{
			// Turn on notifications
			ASSERT( m_pContainer != NULL );
			if( m_pContainer )
			{
				ASSERT( m_FileRef.fRemoveNotify == FALSE );
				hr = theApp.m_pContainerComponent->m_pIFramework8->AddToNotifyList( pINewDocRootNode, m_pContainer );
				if( SUCCEEDED ( hr ) )
				{
					m_FileRef.fRemoveNotify = TRUE;
				}
			}
			
			// Update file reference's DocRoot member variable
			m_FileRef.pIDocRootNode = pINewDocRootNode;
			m_FileRef.pIDocRootNode->AddRef();
			
			// Update file reference's RefNode member variable
			m_FileRef.pIRefNode = pINewRefNode;
//			m_FileRef.pIRefNode->AddRef();		Already AddRef'd by CreateRefNode()

			// Update file reference's list info
			SyncListInfo();

			// Update file reference's script alias
			BSTR bstrScriptAlias;
			hr = m_FileRef.pIDocRootNode->GetNodeName( &bstrScriptAlias );
			if( SUCCEEDED ( hr ) )
			{
				m_FileRef.strScriptAlias = bstrScriptAlias;
				::SysFreeString( bstrScriptAlias );
				m_pContainer->MakeUniqueScriptAlias( this );
			}
		}
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
//	CContainerObject::SetFileReference

HRESULT CContainerObject::SetFileReference( GUID guidFile )
{
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	HRESULT hr = E_FAIL;

	IDMUSProdNode* pIDocRootNode;
	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->FindDocRootNodeByFileGUID( guidFile, &pIDocRootNode ) ) )
	{
		hr = SetFileReference( pIDocRootNode );

		RELEASE( pIDocRootNode );
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
//	CContainerObject::SyncListInfo

HRESULT CContainerObject::SyncListInfo( void )
{
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	HRESULT hr = S_OK;

	ClearListInfo();

	if( m_FileRef.pIDocRootNode )
	{
		DMUSProdListInfo ListInfo;
		ZeroMemory( &ListInfo, sizeof(ListInfo) );
		ListInfo.wSize = sizeof(ListInfo);

		hr = m_FileRef.pIDocRootNode->GetNodeListInfo ( &ListInfo );
		if( SUCCEEDED ( hr ) )
		{
			IDMUSProdProject* pIProject;

			if( ListInfo.bstrName )
			{
				m_FileRef.li.strName = ListInfo.bstrName;
				::SysFreeString( ListInfo.bstrName );
			}
			if( ListInfo.bstrDescriptor )
			{
				m_FileRef.li.strDescriptor = ListInfo.bstrDescriptor;
				::SysFreeString( ListInfo.bstrDescriptor );
			}
			if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->FindProject( m_FileRef.pIDocRootNode, &pIProject ) ) )
			{
				BSTR bstrProjectName;

				m_FileRef.li.pIProject = pIProject;
//				m_FileRef.li.pIProject->AddRef();		intentionally missing

				if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
				{
					m_FileRef.li.strProjectName = bstrProjectName;
					::SysFreeString( bstrProjectName );
				}

				pIProject->Release();
			}

		    theApp.m_pContainerComponent->m_pIFramework8->GetNodeFileGUID( m_FileRef.pIDocRootNode, &m_FileRef.li.guidFile );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject Load

HRESULT CContainerObject::Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, BOOL* pfReference )
{
    IStream*       pIStream;
	MMCKINFO	   ck;
	DWORD		   dwByteCount;
	DWORD		   dwSize;
	DWORD		   dwCurrentFilePos;
	DWORD		   dwStartFilePos;
    HRESULT        hr = S_OK;
    HRESULT        hrReference = S_OK;

	CString		   strObjectName;
	CString		   strScriptAlias;
	IDMUSProdNode* pIDocRootNode = NULL;
	DWORD		   dwRIFFckid = 0;
	DWORD		   dwRIFFfccType = 0;
	CLSID		   clsidDMObject;
	GUID		   guidDocRootNodeId;
	BOOL		   fWasReference = FALSE;
	short		   nReferenceFlagUI = -1;
	memset( &clsidDMObject, 0, sizeof(CLSID) );
	memset( &guidDocRootNodeId, 0, sizeof(GUID) );

	ASSERT( theApp.m_pContainerComponent != NULL );

	if( pfReference == NULL )
	{
		ASSERT( 0 );
		return E_POINTER;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwCurrentFilePos = StreamTell( pIStream );
    dwStartFilePos = dwCurrentFilePos;

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_CONTAINED_ALIAS_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &strScriptAlias );
				break;

			case DMUS_FOURCC_CONTAINED_OBJECT_CHUNK:
			{
			    DMUS_IO_CONTAINED_OBJECT_HEADER dmusContainedObjectIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_CONTAINED_OBJECT_HEADER ) );
				hr = pIStream->Read( &dmusContainedObjectIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				clsidDMObject = dmusContainedObjectIO.guidClassID;
				m_dwFlagsDM = dmusContainedObjectIO.dwFlags;
				dwRIFFckid = dmusContainedObjectIO.ckid;
				dwRIFFfccType = dmusContainedObjectIO.fccType;
				break;
			}

			case DMUS_FOURCC_CONTAINED_OBJECT_UI_CHUNK:
			{
			    ioObjectUI iObjectUI;

				dwSize = min( ck.cksize, sizeof( ioObjectUI ) );
				hr = pIStream->Read( &iObjectUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( iObjectUI.wFlags & COBU_REFERENCE )
				{
					nReferenceFlagUI = TRUE;
				}
				else
				{
					nReferenceFlagUI = FALSE;
				}
				break;
			}

			case FOURCC_DMUSPROD_FILEREF:
			{
				IDMUSProdFileRefChunk* pIFileRef;

				fWasReference = TRUE;
				hr = theApp.m_pContainerComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdFileRefChunk, (void**) &pIFileRef );
				if( FAILED ( hr ) )
				{
					goto ON_ERROR;
				}
				StreamSeek( pIStream, dwCurrentFilePos, 0 );
				hrReference = pIFileRef->LoadRefChunk( pIStream, &pIDocRootNode );
				if( hrReference != S_OK )
				{
					if( hrReference == E_PENDING )
					{
						IDMUSProdNotifySink* pINotifySink;
						if( SUCCEEDED ( m_pContainer->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
						{
							// Store File's GUID so we can resolve reference in our handler 
							// for the FRAMEWORK_FileLoadFinished notification
							StreamSeek( pIStream, dwCurrentFilePos, 0 );
							pIFileRef->ResolveWhenLoadFinished( pIStream, pINotifySink, &m_FileRef.li.guidFile );
							RELEASE( pINotifySink );
						}
					}
					else
					{
						DMUSProdFileRefInfo FileRefInfo;
						memset( &FileRefInfo, 0, sizeof(DMUSProdFileRefInfo) );
						FileRefInfo.wSize = sizeof(DMUSProdFileRefInfo);
						StreamSeek( pIStream, dwCurrentFilePos, 0 );
						if( SUCCEEDED( pIFileRef->GetFileRefInfo( pIStream, &FileRefInfo ) ) )
						{
							guidDocRootNodeId = FileRefInfo.guidDocRootNodeId;
						}
					}
				}
				RELEASE( pIFileRef );
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_REF_LIST:
					{
						MMCKINFO ckName;

						fWasReference = TRUE;
						ckName.ckid = DMUS_FOURCC_NAME_CHUNK;
						if( pIRiffStream->Descend( &ckName, NULL, MMIO_FINDCHUNK ) == 0 )
						{
							// Store object name (segment or style)
							ReadMBSfromWCS( pIStream, ckName.cksize, &strObjectName );
						}
						break;
					}
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
	    dwCurrentFilePos = StreamTell( pIStream );
    }

	if( pIDocRootNode == NULL
	&&  hrReference != E_PENDING )
	{
		if( fWasReference == TRUE )
		{
			if( ::IsEqualGUID( clsidDMObject, CLSID_DirectMusicSegment ) 
			&&  ::IsEqualGUID( guidDocRootNodeId, GUID_WaveNode ) )
			{
				// We really are looking for a wave
				clsidDMObject = CLSID_DirectSoundWave;
			}

			// Resolve reference
			if( !(strObjectName.IsEmpty()) 
			&&  !(::IsEqualGUID(clsidDMObject, GUID_AllZeros)) )
			{
				// Framework could not resolve file reference
				// so we will ask user to help
				hrReference = theApp.m_pContainerComponent->FindReferencedFile( this, clsidDMObject, strObjectName, pIStream, &pIDocRootNode );
				if( SUCCEEDED ( hrReference ) )
				{
					if( m_pContainer->IsDocRootInContainer( pIDocRootNode ) == S_OK )
					{
						// Already in container
						// so we do not want another reference to the DocRoot
						RELEASE( pIDocRootNode );
					}
					else
					{
						SetModifiedFlag();
					}
				}
			}
		}
		else
		{
			// Load embedded file
			StreamSeek( pIStream, dwStartFilePos, 0 );

			MMCKINFO ckEmbeddedFile;
			ckEmbeddedFile.ckid = dwRIFFckid;
			ckEmbeddedFile.fccType = dwRIFFfccType;

			if( ckEmbeddedFile.ckid == FOURCC_RIFF )
			{
				if( pIRiffStream->Descend( &ckEmbeddedFile, pckMain, MMIO_FINDRIFF ) == 0 )
				{
					if( ckEmbeddedFile.fccType == mmioFOURCC('W','A','V','E') 
					&&  ::IsEqualGUID(clsidDMObject, CLSID_DirectMusicSegment) ) 
					{
						// CLSID_DirectSoundWave was saved as CLSID_DirectMusicSegment
						// so convert it bak to CLSID_DirectSoundWave
						clsidDMObject = CLSID_DirectSoundWave;
					}
					StreamSeek( pIStream, -12, STREAM_SEEK_CUR );
					pIDocRootNode = theApp.m_pContainerComponent->LoadEmbeddedFile( clsidDMObject, pIStream );
				}
			}
			else if( ckEmbeddedFile.ckid == FOURCC_LIST )
			{
				if( pIRiffStream->Descend( &ckEmbeddedFile, pckMain, MMIO_FINDLIST ) == 0 )
				{
					StreamSeek( pIStream, -12, STREAM_SEEK_CUR );
					pIDocRootNode = theApp.m_pContainerComponent->LoadEmbeddedFile( clsidDMObject, pIStream );
				}
			}
			else
			{
				if( pIRiffStream->Descend( &ckEmbeddedFile, pckMain, MMIO_FINDCHUNK ) == 0 )
				{
					StreamSeek( pIStream, -8, STREAM_SEEK_CUR );
					pIDocRootNode = theApp.m_pContainerComponent->LoadEmbeddedFile( clsidDMObject, pIStream );
				}
			}
		}
	}

	if( pIDocRootNode == NULL
	&&  hrReference == E_PENDING )
	{
		m_FileRef.strScriptAlias = strScriptAlias;

		if( nReferenceFlagUI == -1 )
		{
			// Did not encounter UI chunk
			// so preserve manner object was stored in the container file
			*pfReference = fWasReference;
		}
		else
		{
			*pfReference = nReferenceFlagUI;
		}
	}
	else if( pIDocRootNode )
	{
		hr = SetFileReference( pIDocRootNode );
		pIDocRootNode->Release();

		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}

		// Store Alias from file
		// Can't do this sooner because SetFileReference() resets alias
		if( strScriptAlias.IsEmpty() == FALSE )
		{
			if( strScriptAlias.Compare( m_FileRef.strScriptAlias ) != 0 )
			{
				m_FileRef.strScriptAlias = strScriptAlias;

				if( m_pIDocRootNode
				&&  m_pIParentNode )
				{
					theApp.m_pContainerComponent->m_pIFramework8->RefreshNode( this );
					SetModifiedFlag();	// To send CONTAINER_ChangeNotification
				}
			}
		}

		if( nReferenceFlagUI == -1 )
		{
			// Did not encounter UI chunk
			// so preserve manner object was stored in the container file
			*pfReference = fWasReference;
		}
		else
		{
			*pfReference = nReferenceFlagUI;
		}
	}
	else
	{
		hr = S_FALSE;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject::SaveDMRef

HRESULT CContainerObject::SaveDMRef( IDMUSProdRIFFStream* pIRiffStream,
									 IDMUSProdNode* pIDocRootNode, DMUS_OBJECTDESC* pdmusObjectDesc, WhichLoader whichLoader )
{
	IDMUSProdLoaderRefChunk* pIRefChunkLoader;

	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );
	if( theApp.m_pContainerComponent->m_pIFramework8 == NULL )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( SUCCEEDED( theApp.m_pContainerComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdLoaderRefChunk, (void**)&pIRefChunkLoader ) ) )
	{
		if( pIRefChunkLoader )
		{
			switch( whichLoader )
			{
				case WL_DIRECTMUSIC:
				case WL_PRODUCER:
					hr = pIRefChunkLoader->SaveRefChunkForLoader( pIStream,
																  pIDocRootNode,
																  pdmusObjectDesc->guidClass,
																  pdmusObjectDesc,
																  whichLoader );
					break;
			}

			pIRefChunkLoader->Release();
		}
	}

	pIStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject::SaveProducerRef

HRESULT CContainerObject::SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode )
{
	IDMUSProdFileRefChunk* pIFileRefChunk;

	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );
	if( theApp.m_pContainerComponent->m_pIFramework8 == NULL )
	{
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if ( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdFileRefChunk, (void**)&pIFileRefChunk ) ) )
	{
		pIFileRefChunk->SaveRefChunk( pIStream, pIDocRootNode );
		pIFileRefChunk->Release();
	}

	pIStream->Release();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject Save
    
HRESULT CContainerObject::Save( IDMUSProdRIFFStream* pIRiffStream, BOOL fSaveReference )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	BOOL fIsEmbeddedInStream = TRUE;

	if( m_FileRef.pIDocRootNode == NULL )
	{
		// Will happen while waiting for FRAMEWORK_FileLoadFinished notification
		return SaveObjectPendingLoad( pIRiffStream, fSaveReference );
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pIPersistInfo;
	if( FAILED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pIPersistInfo ) ) )
	{
		ASSERT( 0 );
		pIPersistInfo = NULL;
	}

	if( pIPersistInfo )
	{
		pIPersistInfo->GetStreamInfo( &StreamInfo );
	}

	// Initialize the DMUS_OBJECTDESC structure
	DMUS_OBJECTDESC dmusObjectDesc;
	memset( &dmusObjectDesc, 0, sizeof(DMUS_OBJECTDESC) );
	dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);
	DWORD dwRIFFckid = 0;
	DWORD dwRIFFfccType = 0;
	
	// Prepare the DMUS_OBJECTDESC structure
	{
		IDMUSProdDocType *pIDocType;
		IDMUSProdDocType8 *pIDocType8;
		GUID guidNodeId;

		// Got DocRoot's NodeId
		hr = m_FileRef.pIDocRootNode->GetNodeId( &guidNodeId );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}

		// Get DocRoot's DocType
		hr = theApp.m_pContainerComponent->m_pIFramework8->FindDocTypeByNodeId( guidNodeId, &pIDocType );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		hr = pIDocType->QueryInterface( IID_IDMUSProdDocType8, (void**)&pIDocType8 );
		RELEASE( pIDocType );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		hr = pIDocType8->GetObjectDescriptorFromNode( m_FileRef.pIDocRootNode, &dmusObjectDesc );
		if( SUCCEEDED ( hr ) )
		{
			hr = pIDocType8->GetObjectRiffId( guidNodeId, &dwRIFFckid, &dwRIFFfccType );
		}
		RELEASE( pIDocType8 );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( !(dmusObjectDesc.dwValidData & DMUS_OBJ_CLASS) )
		{
			hr = E_UNEXPECTED;
			goto ON_ERROR;
		}

		if( m_pContainer->IsInScript() )
		{
			// CLSID_DirectSoundWave should be saved as CLSID_DirectMusicSegment
			// so that a segment containing the wave in a wavetrack will be
			// returned by the loader
			if( ::IsEqualGUID( dmusObjectDesc.guidClass, CLSID_DirectSoundWave ) )
			{
				dmusObjectDesc.guidClass = CLSID_DirectMusicSegment;
			}
		}
	}

	// Write DMUS_FOURCC_CONTAINED_OBJECT_LIST header
	ckMain.fccType = DMUS_FOURCC_CONTAINED_OBJECT_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write DMUS_FOURCC_CONTAINED_ALIAS_CHUNK 
	// Only applies to containers within scripts
	if( m_pContainer->IsInScript() )
	{
		// Write chunk header
		ck.ckid = DMUS_FOURCC_CONTAINED_ALIAS_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_FileRef.strScriptAlias );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write DMUS_FOURCC_CONTAINED_OBJECT_CHUNK
	{
		DMUS_IO_CONTAINED_OBJECT_HEADER dmusContainedObjectIO;

		// Write chunk header
		ck.ckid = DMUS_FOURCC_CONTAINED_OBJECT_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare DMUS_IO_CONTAINER_HEADER structure
		memset( &dmusContainedObjectIO, 0, sizeof(DMUS_IO_CONTAINED_OBJECT_HEADER) );

		dmusContainedObjectIO.guidClassID = dmusObjectDesc.guidClass;
		dmusContainedObjectIO.dwFlags = m_dwFlagsDM;
		if( StreamInfo.ftFileType != FT_RUNTIME
		||  fSaveReference )
		{
			dmusContainedObjectIO.ckid = FOURCC_LIST;
			dmusContainedObjectIO.fccType = DMUS_FOURCC_REF_LIST;
		}
		else
		{
			dmusContainedObjectIO.ckid = dwRIFFckid;
			dmusContainedObjectIO.fccType = dwRIFFfccType;
		}

		// Write Container chunk data
		hr = pIStream->Write( &dmusContainedObjectIO, sizeof(DMUS_IO_CONTAINED_OBJECT_HEADER), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(DMUS_IO_CONTAINED_OBJECT_HEADER) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
		
		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write DMUS_FOURCC_CONTAINED_OBJECT_UI_CHUNK
	if( StreamInfo.ftFileType != FT_RUNTIME )
	{
		ioObjectUI oObjectUI;

		// Write chunk header
		ck.ckid = DMUS_FOURCC_CONTAINED_OBJECT_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare ioObjectUI structure
		memset( &oObjectUI, 0, sizeof(ioObjectUI) );

		if( fSaveReference )
		{
			oObjectUI.wFlags |= COBU_REFERENCE;
		}

		// Write Container chunk data
		hr = pIStream->Write( &oObjectUI, sizeof(ioObjectUI), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(ioObjectUI) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
		
		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}
	}
	
	// Determine whether file is already embedded in this stream
	if( pIPersistInfo 
	&&  pIPersistInfo->IsInEmbeddedFileList(m_FileRef.pIDocRootNode) == S_FALSE )
	{
		fIsEmbeddedInStream = FALSE;
	}

	// Write object data
	if( (::IsEqualGUID( StreamInfo.guidDataFormat, GUID_DirectMusicObject ))
	||  (StreamInfo.ftFileType != FT_RUNTIME)
	||   fSaveReference 
	||   fIsEmbeddedInStream )
	{
		if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_DirectMusicObject ) )
		{
			SaveDMRef( pIRiffStream, m_FileRef.pIDocRootNode, &dmusObjectDesc, WL_PRODUCER );
		}
		else if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion ) )
		{
			SaveDMRef( pIRiffStream, m_FileRef.pIDocRootNode, &dmusObjectDesc, WL_DIRECTMUSIC );
			if( StreamInfo.ftFileType == FT_DESIGN )
			{
				SaveProducerRef( pIRiffStream, m_FileRef.pIDocRootNode );
			}
		}
	}
	else
	{
		IPersistStream* pIPersistStream;

		hr = m_FileRef.pIDocRootNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
		if( pIPersistInfo )
		{
			// Need to do this BEFORE Save()
			pIPersistInfo->AddToEmbeddedFileList( m_FileRef.pIDocRootNode );
		}
		hr = pIPersistStream->Save( pIStream, FALSE );
		RELEASE( pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIPersistInfo );
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject SaveObjectPendingLoad
    
HRESULT CContainerObject::SaveObjectPendingLoad( IDMUSProdRIFFStream* pIRiffStream, BOOL fSaveReference )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ckMain;
	MMCKINFO ckRef;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwLength;

	// Make sure we do not yet have a DocRoot pointer
	if( m_FileRef.pIDocRootNode != NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Make sure we are waiting for this object to finish loading
	EmbeddedFile* pTheEmbeddedFile = NULL;
	if( theApp.m_pContainerComponent->m_pEmbeddedFileRootFile )
	{
		if( ::IsEqualGUID(theApp.m_pContainerComponent->m_pEmbeddedFileRootFile->guidNotification, m_FileRef.li.guidFile) )
		{
			pTheEmbeddedFile = theApp.m_pContainerComponent->m_pEmbeddedFileRootFile;
		}
	}
	if( pTheEmbeddedFile == NULL )
	{
		POSITION pos = theApp.m_pContainerComponent->m_lstEmbeddedFiles.GetHeadPosition();
		while( pos )
		{
			EmbeddedFile* pEmbeddedFile = theApp.m_pContainerComponent->m_lstEmbeddedFiles.GetNext( pos );

			if( ::IsEqualGUID(pEmbeddedFile->guidNotification, m_FileRef.li.guidFile) )
			{
				pTheEmbeddedFile = pEmbeddedFile;
				break;
			}
		}
	}
	if( pTheEmbeddedFile == NULL )
	{
		// We are NOT waiting for FRAMEWORK_FileLoadFinished notification
		ASSERT( 0 );
		return S_FALSE;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pIPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pIPersistInfo ) ) )
	{
		pIPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pIPersistInfo );
	}

	if( StreamInfo.ftFileType == FT_RUNTIME )
	{
		// This method must save FT_DESIGN format
		// because it cannot embed files
		ASSERT( 0 );
		hr = E_UNEXPECTED;
		goto ON_ERROR;
	}

	// Initialize the DMUS_OBJECTDESC structure
	DMUS_OBJECTDESC dmusObjectDesc;
	memset( &dmusObjectDesc, 0, sizeof(DMUS_OBJECTDESC) );
	dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);
	
	// Prepare the DMUS_OBJECTDESC structure
	{
		dmusObjectDesc.dwValidData = ( DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS | DMUS_OBJ_NAME );
		memcpy( &dmusObjectDesc.guidObject, &pTheEmbeddedFile->guidObject, sizeof(GUID) );
		memcpy( &dmusObjectDesc.guidClass, &pTheEmbeddedFile->guidDMClass, sizeof(CLSID) );
		MultiByteToWideChar( CP_ACP, 0, pTheEmbeddedFile->strObjectName, -1, dmusObjectDesc.wszName, DMUS_MAX_NAME );

		if( m_pContainer->IsInScript() )
		{
			// CLSID_DirectSoundWave should be saved as CLSID_DirectMusicSegment
			// so that a segment containing the wave in a wavetrack will be
			// returned by the loader
			if( ::IsEqualGUID( dmusObjectDesc.guidClass, CLSID_DirectSoundWave ) )
			{
				dmusObjectDesc.guidClass = CLSID_DirectMusicSegment;
			}
		}
	}

	// Write DMUS_FOURCC_CONTAINED_OBJECT_LIST header
	ckMain.fccType = DMUS_FOURCC_CONTAINED_OBJECT_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write DMUS_FOURCC_CONTAINED_ALIAS_CHUNK 
	// Only applies to containers within scripts
	if( m_pContainer->IsInScript() )
	{
		// Write chunk header
		ck.ckid = DMUS_FOURCC_CONTAINED_ALIAS_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_FileRef.strScriptAlias );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write DMUS_FOURCC_CONTAINED_OBJECT_CHUNK
	{
		DMUS_IO_CONTAINED_OBJECT_HEADER dmusContainedObjectIO;

		// Write chunk header
		ck.ckid = DMUS_FOURCC_CONTAINED_OBJECT_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare DMUS_IO_CONTAINER_HEADER structure
		memset( &dmusContainedObjectIO, 0, sizeof(DMUS_IO_CONTAINED_OBJECT_HEADER) );

		dmusContainedObjectIO.guidClassID = dmusObjectDesc.guidClass;
		dmusContainedObjectIO.dwFlags = m_dwFlagsDM;
		dmusContainedObjectIO.ckid = FOURCC_LIST;
		dmusContainedObjectIO.fccType = DMUS_FOURCC_REF_LIST;

		// Write Container chunk data
		hr = pIStream->Write( &dmusContainedObjectIO, sizeof(DMUS_IO_CONTAINED_OBJECT_HEADER), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(DMUS_IO_CONTAINED_OBJECT_HEADER) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
		
		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write DMUS_FOURCC_CONTAINED_OBJECT_UI_CHUNK
	{
		ioObjectUI oObjectUI;

		// Write chunk header
		ck.ckid = DMUS_FOURCC_CONTAINED_OBJECT_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare ioObjectUI structure
		memset( &oObjectUI, 0, sizeof(ioObjectUI) );

		if( fSaveReference )
		{
			oObjectUI.wFlags |= COBU_REFERENCE;
		}

		// Containers in Segments do not support referenced files
		if( m_pContainer->IsInSegment() )
		{
			oObjectUI.wFlags &= ~COBU_REFERENCE;
		}

		// Write Container chunk data
		hr = pIStream->Write( &oObjectUI, sizeof(ioObjectUI), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(ioObjectUI) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
		
		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}
	}
	
	// Write DMUS_FOURCC_REF_LIST
	{
		// Write REF LIST header
		ckRef.fccType = DMUS_FOURCC_REF_LIST;
		if( pIRiffStream->CreateChunk(&ckRef, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write REF chunk
		{
			DMUS_IO_REFERENCE dmusReference;

			ck.ckid = DMUS_FOURCC_REF_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Prepare DMUS_IO_REFERENCE structure
			memset( &dmusReference, 0, sizeof(DMUS_IO_REFERENCE) );
			memcpy( &dmusReference.guidClassID, &dmusObjectDesc.guidClass, sizeof(GUID) );
			dmusReference.dwValidData = dmusObjectDesc.dwValidData;

			// Write REF chunk data 
			hr = pIStream->Write( &dmusReference, sizeof(DMUS_IO_REFERENCE), &dwBytesWritten);
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DMUS_IO_REFERENCE) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		// Write GUID chunk
		{
			ck.ckid = DMUS_FOURCC_GUID_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			hr = pIStream->Write( &dmusObjectDesc.guidObject, sizeof(GUID), &dwBytesWritten);
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(GUID) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		// Write Name chunk
		{
			ck.ckid = DMUS_FOURCC_NAME_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			dwLength = wcslen( dmusObjectDesc.wszName ) + 1; 
			dwLength *= sizeof( wchar_t );
			hr = pIStream->Write( &dmusObjectDesc.wszName, dwLength, &dwBytesWritten);
			if( FAILED( hr )
			||  dwBytesWritten != dwLength )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		if( pIRiffStream->Ascend(&ckRef, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerObject ResolveBestGuessWhenLoadFinished

HRESULT CContainerObject::ResolveBestGuessWhenLoadFinished( IDMUSProdDocType* pIDocType, BSTR bstrObjectName, IDMUSProdNode* pITargetDirectoryNode )
{
	HRESULT hr = E_FAIL;

	IDMUSProdNotifySink* pINotifySink;
	if( SUCCEEDED ( m_pContainer->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
	{
		// Don't free BSTR - it will get freed in ResolveBestGuessWhenLoadFinished()
		CString strObjectName = bstrObjectName;

		// Store temporary GUID so we can resolve reference in our handler 
		// for the FRAMEWORK_FileLoadFinished notification
		hr = theApp.m_pContainerComponent->m_pIFramework8->ResolveBestGuessWhenLoadFinished( pIDocType,
																							 bstrObjectName,
																							 pITargetDirectoryNode,
																							 pINotifySink,
																							 &m_FileRef.li.guidFile );
		if( SUCCEEDED ( hr ) )
		{
			EmbeddedFile* pEmbeddedFile = theApp.m_pContainerComponent->FindEmbeddedFile( pIDocType, strObjectName );

			if( pEmbeddedFile
			&&  pEmbeddedFile->dwStreamPos == 0 
			&&  ::IsEqualGUID(pEmbeddedFile->guidDMClass, CLSID_DirectMusicContainer) == FALSE )
			{
				// Need to special case when root file is not a Container
				pEmbeddedFile->guidNotification = m_FileRef.li.guidFile;
			}
		}

		RELEASE( pINotifySink );
	}

	return hr;
}
