// Container.cpp : implementation file
//

#include "stdafx.h"
#include "ContainerDesignerDLL.h"

#include "RiffStrm.h"
#include "Container.h"
#include <mmreg.h>
#include <math.h>
#include "TabContainer.h"
#include "TabInfo.h"
#include "ContainerCtl.h"
#include "DlgAddFiles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// {F078ACC0-67AA-11d3-B45D-00105A2796DE}
static const GUID GUID_ContainerPropPageManager = 
{ 0xf078acc0, 0x67aa, 0x11d3, { 0xb4, 0x5d, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde } };

short CContainerPropPageManager::sm_nActiveTab = 0;


//////////////////////////////////////////////////////////////////////
// CContainerPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContainerPropPageManager::CContainerPropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabContainer = NULL;
	m_pTabInfo = NULL;
}

CContainerPropPageManager::~CContainerPropPageManager()
{
	RELEASE( m_pIPropSheet );

	if( m_pTabContainer )
	{
		delete m_pTabContainer;
	}

	if( m_pTabInfo )
	{
		delete m_pTabInfo;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CContainerPropPageManager::RemoveCurrentObject

void CContainerPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerPropPageManager IUnknown implementation

HRESULT CContainerPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CContainerPropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CContainerPropPageManager::Release()
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
// CContainerPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CContainerPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CContainerPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
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

	CDirectMusicContainer *pContainer;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pContainer))) )
	{
		CString strNodeName;
		BSTR bstrNodeName;

		if( SUCCEEDED ( pContainer->GetNodeName( &bstrNodeName ) ) )
		{
			strNodeName = bstrNodeName;
			::SysFreeString( bstrNodeName );
		}
		strTitle = strNodeName + _T(" ") + strTitle;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CContainerPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	// Add Container tab
	m_pTabContainer = new CTabContainer( this );
	if( m_pTabContainer )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabContainer->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Info tab
	m_pTabInfo = new CTabInfo( this );
	if( m_pTabInfo )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabInfo->m_psp );
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
// CContainerPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CContainerPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CContainerPropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CContainerPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
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
// CContainerPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CContainerPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CContainerPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CContainerPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CContainerPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CContainerPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicContainer* pContainer;
	
	if( m_pIPropPageObject == NULL )
	{
		pContainer = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pContainer ) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current Container are processed in OnKillFocus
	// messages before setting the new Container
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabContainer->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new Container
	m_pTabContainer->SetContainer( pContainer);
	m_pTabInfo->SetContainer( pContainer);

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CContainerPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_ContainerPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CObjectToSave constructor/destructor 

CObjectToSave::CObjectToSave( CContainerObject* pContainerObject, BOOL fFromReferenceFolder )
{
	ASSERT( pContainerObject != NULL );
	m_pContainerObject = pContainerObject;

	m_fFromReferenceFolder = fFromReferenceFolder;
	m_nPriority = GetPriority();
}

CObjectToSave::~CObjectToSave()
{
}

short CObjectToSave::GetPriority( void )
{
	// (MAX_FILE_PRIORITY - 1) reserved for Container files
	// (MAX_FILE_PRIORITY - 2) reserved for Song files
	// (MAX_FILE_PRIORITY - 3) reserved for Segment files
	short nPriority = MAX_FILE_PRIORITY - 4;
	
	if( m_pContainerObject->m_FileRef.pIDocRootNode == NULL )
	{
		// Use the default priority
		return nPriority;
	}

	GUID guidNodeId;
	if( SUCCEEDED ( m_pContainerObject->m_FileRef.pIDocRootNode->GetNodeId( &guidNodeId ) ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_CollectionNode ) )
		{
			nPriority = 1;
		}
		else if( ::IsEqualGUID( guidNodeId, GUID_StyleNode ) )
		{
			nPriority = 2;
		}
		else if( ::IsEqualGUID( guidNodeId, GUID_PersonalityNode ) )
		{
			nPriority = 3;
		}
		else if( ::IsEqualGUID( guidNodeId, GUID_WaveNode ) )
		{
			nPriority = 4;
		}
		else if( ::IsEqualGUID( guidNodeId, GUID_ScriptNode ) )
		{
			nPriority = 5;
		}
		else if( ::IsEqualGUID( guidNodeId, GUID_SegmentNode ) )
		{
			nPriority = MAX_FILE_PRIORITY - 3;
		}
		/*
		else if( ::IsEqualGUID( guidNodeId, GUID_SongNode ) )
		{
			nPriority = MAX_FILE_PRIORITY - 2;
		}
		*/
		else if( ::IsEqualGUID( guidNodeId, GUID_ContainerNode ) )
		{
			nPriority = MAX_FILE_PRIORITY - 1;
		}
	}
	
	return nPriority;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer constructor/destructor

CDirectMusicContainer::CDirectMusicContainer()
{
	ASSERT( theApp.m_pContainerComponent != NULL );

	// Container needs Container Component
	theApp.m_pContainerComponent->AddRef();

    m_dwRef = 0;
	AddRef();

	m_pUndoMgr = NULL;
	m_fModified = FALSE;

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;

	m_hWndEditor = NULL;
	m_pContainerCtrl = NULL;
	m_pINodeBeingDeleted = NULL;
	m_pIProject = NULL;

	TCHAR achName[MID_BUFFER];

	m_FolderEmbed.m_pContainer = this;
	m_FolderEmbed.m_guidTypeNode = GUID_ContainerEmbedFolderNode;
	::LoadString( theApp.m_hInstance, IDS_EMBED_FOLDER_NAME, achName, MID_BUFFER );
	m_FolderEmbed.m_strName = achName;

	m_FolderReference.m_pContainer = this;
	m_FolderReference.m_guidTypeNode = GUID_ContainerRefFolderNode;
	::LoadString( theApp.m_hInstance, IDS_REF_FOLDER_NAME, achName, MID_BUFFER );
	m_FolderReference.m_strName = achName;

// Container info block
	::LoadString( theApp.m_hInstance, IDS_CONTAINER_TEXT, achName, MID_BUFFER );
    m_strName.Format( "%s%d", achName, ++theApp.m_pContainerComponent->m_nNextContainer );

	CoCreateGuid( &m_guidContainer ); 
	m_vVersion.dwVersionMS = 0x00010000;
	m_vVersion.dwVersionLS = 0x00000000;

	m_dwFlagsDM = 0;
}

CDirectMusicContainer::~CDirectMusicContainer()
{
	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	// Remove Container from clipboard
	theApp.FlushClipboard( this );

	// Remove Container from property sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}

	// Cleanup the Undo Manager
	if( m_pUndoMgr )
	{
		delete m_pUndoMgr;
	}

	// Container no longer needs Container Component
	theApp.m_pContainerComponent->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::CreateUndoMgr

BOOL CDirectMusicContainer::CreateUndoMgr()
{
	// Should only be called once - after Container first created
	ASSERT( m_pUndoMgr == NULL );

	ASSERT( theApp.m_pContainerComponent != NULL ); 
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL ); 

	m_pUndoMgr = new CJazzUndoMan( theApp.m_pContainerComponent->m_pIFramework8 );
	if( m_pUndoMgr )
	{
		m_pUndoMgr->SetStreamAttributes( FT_DESIGN, GUID_CurrentVersion );
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::IsInScript

BOOL CDirectMusicContainer::IsInScript( void )
{
	BOOL fInScript = FALSE;

	if( m_pIParentNode )
	{
		GUID guidNodeId;
		if( SUCCEEDED ( m_pIParentNode->GetNodeId( &guidNodeId ) ) )
		{
			if( ::IsEqualGUID( guidNodeId, GUID_ScriptNode ) )
			{
				fInScript = TRUE;
			}
		}
	}

	return fInScript;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::IsInSegment

BOOL CDirectMusicContainer::IsInSegment( void )
{
	BOOL fInSegment = FALSE;

	if( m_pIParentNode )
	{
		GUID guidNodeId;
		if( SUCCEEDED ( m_pIParentNode->GetNodeId( &guidNodeId ) ) )
		{
			if( ::IsEqualGUID( guidNodeId, GUID_SegmentNode ) )
			{
				fInSegment = TRUE;
			}
		}
	}

	return fInSegment;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::IsScriptAliasUnique

BOOL CDirectMusicContainer::IsScriptAliasUnique( CContainerObject* pObject )
{
	CContainerObject* pObjectList;
	CString strScriptAlias;
	CString strScriptAliasList;

	pObject->GetScriptAlias( strScriptAlias );

	// Check embedded files
	POSITION pos = m_FolderEmbed.m_lstObjects.GetHeadPosition();
	while( pos )
	{
		pObjectList = m_FolderEmbed.m_lstObjects.GetNext( pos );

		if( pObjectList != pObject )
		{
			pObjectList->GetScriptAlias( strScriptAliasList );
			if( strScriptAliasList.CompareNoCase( strScriptAlias ) == 0 )
			{
				return FALSE;
			}
		}
	}

	// Check referenced files
	pos = m_FolderReference.m_lstObjects.GetHeadPosition();
	while( pos )
	{
		pObjectList = m_FolderReference.m_lstObjects.GetNext( pos );

		if( pObjectList != pObject )
		{
			pObjectList->GetScriptAlias( strScriptAliasList );
			if( strScriptAliasList.CompareNoCase( strScriptAlias ) == 0 )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::MakeUniqueScriptAlias

void CDirectMusicContainer::MakeUniqueScriptAlias( CContainerObject* pObject )
{
	CString	strAlias;
	CString	strOrigAlias;
	CString	strNbr;
	int	nOrigAliasLength;
	int	nNbrLength;
	int	i;

	// Get current script alias
	pObject->GetScriptAlias( strAlias );
	strAlias = strAlias.Left( DMUS_MAX_NAME );

	// Make sure alias starts with a letter
	for( i = 0 ;  i < strAlias.GetLength() ;  i++ )
	{
		if( _istalpha( strAlias[i] ) )
		{
			break;
		}
	}
	strAlias = strAlias.Right( strAlias.GetLength() - i );

	// Strip characters other than letters, digits and underscores
	for( i = 0 ;  i < strAlias.GetLength() ;  i++ )
	{
		if( _istalpha( strAlias[i] )
		||  _istdigit( strAlias[i] )
		||   strAlias[i] == _T('_') )
		{
			strOrigAlias +=  strAlias[i];
		}
	}

	// Make sure alias is not empty
	if( strOrigAlias.IsEmpty() )
	{
		strOrigAlias.LoadString( IDS_DEFAULT_SCRIPT_ALIAS );
	}

	// Update the object with the adjusted script alias
	pObject->SetScriptAlias( strOrigAlias );

	// Strip the number from the end of the script alias
	while( !strOrigAlias.IsEmpty()  &&  _istdigit(strOrigAlias[strOrigAlias.GetLength() - 1]) )
	{
		strNbr = strOrigAlias.Right(1) + strNbr;
		strOrigAlias = strOrigAlias.Left( strOrigAlias.GetLength() - 1 );
	}

	nOrigAliasLength = strOrigAlias.GetLength();
	i = _ttoi( strNbr );

	// Get a unique name
	while( IsScriptAliasUnique( pObject ) == FALSE )
	{
		strNbr.Format( "%d", ++i ); 
		nNbrLength = strNbr.GetLength();
		
		if( (nOrigAliasLength + nNbrLength) <= DMUS_MAX_NAME )
		{
			strAlias = strOrigAlias + strNbr;
		}
		else
		{
			strAlias = strOrigAlias.Left(DMUS_MAX_NAME - nNbrLength) + strNbr;
		}

		pObject->SetScriptAlias( strAlias );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::SyncContainerEditor

void CDirectMusicContainer::SyncContainerEditor( void )
{
// AMC delete??

//	if( m_pContainerCtrl
//	&&  m_pContainerCtrl->m_pContainerDlg )
//	{
//		m_pContainerCtrl->m_pContainerDlg->RefreshControls();
//	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::SetModified

void CDirectMusicContainer::SetModified( BOOL fModified )
{
	m_fModified = fModified;

	if( m_fModified )
	{
		if( m_pIDocRootNode
		&&  m_pIDocRootNode != this )
		{
			// Notify DocRoot that the container has changed
			IDMUSProdNotifySink* pINotifySink;
			if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
			{
				pINotifySink->OnUpdate( this, CONTAINER_ChangeNotification, NULL );

				RELEASE( pINotifySink );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IUnknown implementation

HRESULT CDirectMusicContainer::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IDMUSProdNode *)this;
    }
    else if( ::IsEqualIID(riid, IID_IPersist) )
    {
        *ppvObj = (IPersist *)this;
    }
    else if( ::IsEqualIID(riid, IID_IPersistStream) )
    {
        *ppvObj = (IPersistStream *)this;
    }
    else if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        *ppvObj = (IDMUSProdPropPageObject *)this;
    }
	else if( ::IsEqualIID(riid, IID_IDMUSProdNotifySink) )
	{
		*ppvObj = (IDMUSProdNotifySink*) this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdContainerInfo) )
	{
		*ppvObj = (IDMUSProdContainerInfo*) this;
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CDirectMusicContainer::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CDirectMusicContainer::Release()
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
// CDirectMusicContainer IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetNodeImageIndex

HRESULT CDirectMusicContainer::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );

	return( theApp.m_pContainerComponent->GetContainerImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetFirstChild

HRESULT CDirectMusicContainer::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	m_FolderEmbed.AddRef();
	*ppIFirstChildNode = (IDMUSProdNode *)&m_FolderEmbed;

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetNextChild

HRESULT CDirectMusicContainer::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
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

	if( pIChildNode == (IDMUSProdNode *)&m_FolderEmbed )
	{
		m_FolderReference.AddRef();
		*ppINextChildNode = (IDMUSProdNode *)&m_FolderReference;
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetComponent

HRESULT CDirectMusicContainer::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );

	return theApp.m_pContainerComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetDocRootNode

HRESULT CDirectMusicContainer::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CDirectMusicContainer IDMUSProdNode::SetDocRootNode

HRESULT CDirectMusicContainer::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetParentNode

HRESULT CDirectMusicContainer::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIParentNode == NULL )
	{
		return E_POINTER;
	}

	*ppIParentNode = m_pIParentNode;

	if( m_pIParentNode )
	{
		m_pIParentNode->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::SetParentNode

HRESULT CDirectMusicContainer::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetNodeId

HRESULT CDirectMusicContainer::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_ContainerNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetNodeName

HRESULT CDirectMusicContainer::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetNodeNameMaxLength

HRESULT CDirectMusicContainer::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( IsInScript() ) 
	{
		// Can't rename a Container in a Script
		*pnMaxLength = -1;	
	}
	else
	{
		*pnMaxLength = DMUS_MAX_NAME;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::ValidateNodeName

HRESULT CDirectMusicContainer::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;

	strName = bstrName;
	::SysFreeString( bstrName );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::SetNodeName

HRESULT CDirectMusicContainer::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( m_strName == strName )
	{
		return S_OK;
	}

	m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_CONTAINER_NAME );
	m_strName = strName;

	// Sync change with property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		RELEASE( pIPropSheet );
	}
	
	SetModified( TRUE );

	// Notify connected nodes that Container name has changed
	theApp.m_pContainerComponent->m_pIFramework8->NotifyNodes( this, CONTAINER_NameChange, NULL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetNodeListInfo

HRESULT CDirectMusicContainer::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strDescriptor;

    pListInfo->bstrName = m_strName.AllocSysString();
    pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
	memcpy( &pListInfo->guidObject, &m_guidContainer, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetEditorClsId

HRESULT CDirectMusicContainer::GetEditorClsId( CLSID* pClsId )
{
// AMC delete??

// 	AFX_MANAGE_STATE(_afxModuleAddrThis);

//   *pClsId = CLSID_ContainerEditor;
	
//	return S_OK;
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetEditorTitle

HRESULT CDirectMusicContainer::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	::LoadString( theApp.m_hInstance, IDS_CONTAINER_TEXT, achTitle, MID_BUFFER );
	strTitle  = achTitle;
	strTitle += _T(": " );
	strTitle += m_strName;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetEditorWindow

HRESULT CDirectMusicContainer::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::SetEditorWindow

HRESULT CDirectMusicContainer::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::UseOpenCloseImages

HRESULT CDirectMusicContainer::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetRightClickMenuId

HRESULT CDirectMusicContainer::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;

	if( IsInScript() )
	{
		*pnMenuId = IDM_SCRIPT_CONTAINER_NODE_RMENU;
	}
	else
	{
		*pnMenuId = IDM_CONTAINER_NODE_RMENU;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::OnRightClickMenuInit

HRESULT CDirectMusicContainer::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::OnRightClickMenuSelect

HRESULT CDirectMusicContainer::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_ADD_FILES:
			OnAddRemoveFiles();
			hr = S_OK;
			break;

		case IDM_RENAME:
			if( IsInScript() == FALSE )
			{
				hr = theApp.m_pContainerComponent->m_pIFramework8->EditNodeLabel( this );
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
// CDirectMusicContainer IDMUSProdNode::DeleteChildNode

HRESULT CDirectMusicContainer::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// Container nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::InsertChildNode

HRESULT CDirectMusicContainer::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// Container nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::DeleteNode

HRESULT CDirectMusicContainer::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;

	IDMUSProdNode* pIParentNode;

	// Let our parent delete us
	if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
	{
		if( pIParentNode )
		{
			// Remove embedded objects from Framework's notification list
			CContainerObject* pContainerObject;
			POSITION pos = m_FolderEmbed.m_lstObjects.GetHeadPosition();
			while( pos )
			{
				pContainerObject = m_FolderEmbed.m_lstObjects.GetNext( pos );
				pContainerObject->SetFileReference( NULL );
			}

			// Remove referenced objects from Framework's notification list
			pos = m_FolderReference.m_lstObjects.GetHeadPosition();
			while( pos )
			{
				pContainerObject = m_FolderReference.m_lstObjects.GetNext( pos );
				pContainerObject->SetFileReference( NULL );
			}

			HRESULT hr = pIParentNode->DeleteChildNode( this, fPromptUser );
			RELEASE( pIParentNode );
			return hr;
		}
	}

	// No parent so we will delete ourself
	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	// Remove from Project Tree
	if( theApp.m_pContainerComponent->m_pIFramework8->RemoveNode( this, fPromptUser ) == S_FALSE )
	{
		// User cancelled delete
		return E_FAIL;
	}

	// Remove embedded objects from Framework's notification list
	CContainerObject* pContainerObject;
	POSITION pos = m_FolderEmbed.m_lstObjects.GetHeadPosition();
	while( pos )
	{
		pContainerObject = m_FolderEmbed.m_lstObjects.GetNext( pos );
		pContainerObject->SetFileReference( NULL );
	}

	// Remove referenced objects from Framework's notification list
	pos = m_FolderReference.m_lstObjects.GetHeadPosition();
	while( pos )
	{
		pContainerObject = m_FolderReference.m_lstObjects.GetNext( pos );
		pContainerObject->SetFileReference( NULL );
	}

	// Remove from Component Container list
	theApp.m_pContainerComponent->RemoveFromContainerFileList( this );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::OnNodeSelChanged

HRESULT CDirectMusicContainer::OnNodeSelChanged( BOOL fSelected )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::CreateDataObject

HRESULT CDirectMusicContainer::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

	// Save Container into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_CONTAINER into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pContainerComponent->m_cfContainer, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->SaveClipFormat( theApp.m_pContainerComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// Container nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pContainerComponent->m_cfProducerFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			RELEASE( pIStream );
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		RELEASE( pDataObject );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::CanCut

HRESULT CDirectMusicContainer::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::CanCopy

HRESULT CDirectMusicContainer::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::CanDelete

HRESULT CDirectMusicContainer::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode != this )
	{
		if( m_pIParentNode )
		{
			return m_pIParentNode->CanDeleteChildNode( this );
		}
		else
		{
			return S_FALSE;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::CanDeleteChildNode

HRESULT CDirectMusicContainer::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Container nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::CanPasteFromData

HRESULT CDirectMusicContainer::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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
	
	if( m_pIDocRootNode != this )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent node decide what can be dropped
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}
	
	return S_FALSE;		// Can't paste anything on a Container node
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::PasteFromData

HRESULT CDirectMusicContainer::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode != this )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle paste
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}
	
	ASSERT( 0 );	// Should not happen!
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::CanChildPasteFromData

HRESULT CDirectMusicContainer::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	ASSERT( 0 );

	return E_NOTIMPL;	// Container folder nodes are the nodes that handle paste
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::ChildPasteFromData

HRESULT CDirectMusicContainer::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Container folder nodes are the nodes that handle paste
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdNode::GetObject

HRESULT CDirectMusicContainer::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( 0 );

    *ppvObject = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdPropPageObject::GetData

HRESULT CDirectMusicContainer::GetData( void** ppData )
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
// CDirectMusicContainer IDMUSProdPropPageObject::SetData

HRESULT CDirectMusicContainer::SetData( void* pData )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IDMUSProdPropPageObject::OnShowProperties

HRESULT CDirectMusicContainer::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	// Get the Container page manager
	CContainerPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_ContainerPropPageManager ) == S_OK )
	{
		pPageManager = (CContainerPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CContainerPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Container properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CContainerPropPageManager::sm_nActiveTab;

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
// CDirectMusicContainer IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CDirectMusicContainer::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IPersist::GetClassID

HRESULT CDirectMusicContainer::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IPersistStream::IsDirty

HRESULT CDirectMusicContainer::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fModified )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::LoadContainer

HRESULT CDirectMusicContainer::LoadContainer( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*      pIStream;
	MMCKINFO	  ck;
	MMCKINFO	  ckList;
	DWORD		  dwByteCount;
	DWORD		  dwSize;
    HRESULT       hr = S_OK;

	ASSERT( theApp.m_pContainerComponent != NULL );

	// Following strings only saved when they have values
	// So make sure they are initialized!!
	m_strAuthor.Empty();
	m_strCopyright.Empty();
	m_strSubject.Empty();
	m_strInfo.Empty();

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_CONTAINER_CHUNK:
			{
			    DMUS_IO_CONTAINER_HEADER dmusContainerIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_CONTAINER_HEADER ) );
				hr = pIStream->Read( &dmusContainerIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwFlagsDM = dmusContainerIO.dwFlags;
				break;
			}

			case DMUS_FOURCC_GUID_CHUNK:
				dwSize = min( ck.cksize, sizeof( GUID ) );
				hr = pIStream->Read( &m_guidContainer, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;

			case DMUS_FOURCC_VERSION_CHUNK:
			{
			    DMUS_IO_VERSION dmusVersionIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_VERSION ) );
				hr = pIStream->Read( &dmusVersionIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_vVersion.dwVersionMS = dmusVersionIO.dwVersionMS;
				m_vVersion.dwVersionLS = dmusVersionIO.dwVersionLS;
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_CONTAINED_OBJECTS_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case FOURCC_LIST:
									switch( ckList.fccType )
									{
										case DMUS_FOURCC_CONTAINED_OBJECT_LIST:
										{
											CContainerObject* pContainerObject = new CContainerObject( this );
											if( pContainerObject == NULL )
											{
												hr = E_OUTOFMEMORY;
												goto ON_ERROR;
											}

											BOOL fReference = FALSE;

											hr = pContainerObject->Load( pIRiffStream, &ckList, &fReference );
											if( hr == S_OK )
											{
												if( fReference  )
												{
													m_FolderReference.m_lstObjects.AddTail( pContainerObject );
												}
												else
												{
													m_FolderEmbed.m_lstObjects.AddTail( pContainerObject );
												}
											}
											else if( hr == S_FALSE )
											{
												// Could not resolve file reference so discard this object
												delete pContainerObject;
												hr = S_OK;
											}
											else
											{
												delete pContainerObject;
												goto ON_ERROR;
											}
											break;
										}
									}
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_UNAM_CHUNK:
								{
									CString strName;

									ReadMBSfromWCS( pIStream, ckList.cksize, &strName );
									if( strName.CompareNoCase( m_strName ) != 0 )
									{
										m_strName = strName;
										theApp.m_pContainerComponent->m_pIFramework8->RefreshNode( this );

										// Notify connected nodes that the Container name has changed
										theApp.m_pContainerComponent->m_pIFramework8->NotifyNodes( this, CONTAINER_NameChange, NULL );
									}
									break;
								}

								case DMUS_FOURCC_UART_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strAuthor );
									break;

								case DMUS_FOURCC_UCOP_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strCopyright );
									break;

								case DMUS_FOURCC_USBJ_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strSubject );
									break;

								case DMUS_FOURCC_UCMT_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strInfo );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
    }

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IPersistStream::Load

HRESULT CDirectMusicContainer::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	// Generate EmbeddedFile list
	DWORD dwCurrentFilePos = StreamTell( pIStream );
	theApp.m_pContainerComponent->CreateEmbeddedFileList( pIStream );
	StreamSeek( pIStream, dwCurrentFilePos, 0 );

	// Load the Container
    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    HRESULT hr = E_FAIL;

	// Check for Direct Music format
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_CONTAINER_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			hr = LoadContainer( pIRiffStream, &ckMain );
		}

		RELEASE( pIRiffStream );
	}

	// Sync UI
	if( m_pIDocRootNode )
	{
		// Already in Project Tree so sync changes
		// Refresh Container editor (when open)
		SyncContainerEditor();

		// Sync change with property sheet
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			pIPropSheet->RefreshTitle();
			pIPropSheet->RefreshActivePage();
			RELEASE( pIPropSheet );
		}
	}

	// Release EmbeddedFile list
	theApp.m_pContainerComponent->ReleaseEmbeddedFileList();

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer InsertObjectToSave
    
void CDirectMusicContainer::InsertObjectToSave( CTypedPtrList<CPtrList, CObjectToSave*>& list, CObjectToSave* pObjectToInsert )
{
	// Ensure the pObjectToInsert pointer is valid
	if ( pObjectToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	CObjectToSave* pObject;
	POSITION posCurrent, posNext = list.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pObject = list.GetNext( posNext );

		if( pObject->m_nPriority > pObjectToInsert->m_nPriority )
		{
			// Insert before posCurrent (which is the position of pObject)
			list.InsertBefore( posCurrent, pObjectToInsert );
			return;
		}
	}

	// pObjectToInsert has highr priority than all items in the list, add it at the end of the list
	list.AddTail( pObjectToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer CreateObjectsToSaveList
    
HRESULT CDirectMusicContainer::CreateObjectsToSaveList( CTypedPtrList<CPtrList, CObjectToSave*>& list )
{
	CContainerObject* pContainerObject;
	CObjectToSave* pObjectToSave;

	// Embedded list
	POSITION pos = m_FolderEmbed.m_lstObjects.GetHeadPosition();
	while( pos )
	{
		pContainerObject = m_FolderEmbed.m_lstObjects.GetNext( pos );

		pObjectToSave = new CObjectToSave( pContainerObject, false );
		if( pObjectToSave == NULL )
		{
			return E_OUTOFMEMORY;
		}

		InsertObjectToSave( list, pObjectToSave );
	}

	// Referenced list 
	pos = m_FolderReference.m_lstObjects.GetHeadPosition();
	while( pos )
	{
		pContainerObject = m_FolderReference.m_lstObjects.GetNext( pos );

		pObjectToSave = new CObjectToSave( pContainerObject, true );
		if( pObjectToSave == NULL )
		{
			return E_OUTOFMEMORY;
		}

		InsertObjectToSave( list, pObjectToSave );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer SaveObjects
    
HRESULT CDirectMusicContainer::SaveObjects( IDMUSProdRIFFStream* pIRiffStream )
{
	CObjectToSave* pObjectToSave;
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
	POSITION pos;
	CTypedPtrList<CPtrList, CObjectToSave*> lstObjectsToSave;

	if( m_FolderEmbed.m_lstObjects.IsEmpty()
	&&  m_FolderReference.m_lstObjects.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write DMUS_FOURCC_CONTAINED_OBJECTS_LIST header
	ckMain.fccType = DMUS_FOURCC_CONTAINED_OBJECTS_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the list of objects to save
	// List combines "Embedded" and "Referenced" folder items
	// List is sorted in an order appropriate for saving
	hr = CreateObjectsToSaveList( lstObjectsToSave );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Write "ObjectsToSave" list
	pos = lstObjectsToSave.GetHeadPosition();
	while( pos )
	{
		pObjectToSave = lstObjectsToSave.GetNext( pos );

		hr = pObjectToSave->m_pContainerObject->Save( pIRiffStream, pObjectToSave->m_fFromReferenceFolder );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
		hr = S_OK;	// Change possible S_FALSE to S_OK;
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	// Cleanup "ObjectsToSave" list
	while( !lstObjectsToSave.IsEmpty() )
	{
		pObjectToSave = lstObjectsToSave.RemoveHead();
		delete pObjectToSave;
	}

    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer SaveVersion
    
HRESULT CDirectMusicContainer::SaveVersion( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_VERSION dmusVersionIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Version chunk header
	ck.ckid = DMUS_FOURCC_VERSION_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_VERSION structure
	memset( &dmusVersionIO, 0, sizeof(DMUS_IO_VERSION) );

	dmusVersionIO.dwVersionMS = m_vVersion.dwVersionMS ;
	dmusVersionIO.dwVersionLS = m_vVersion.dwVersionLS;

	// Write Version chunk data
	hr = pIStream->Write( &dmusVersionIO, sizeof(DMUS_IO_VERSION), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_VERSION) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer SaveInfoList
    
HRESULT CDirectMusicContainer::SaveInfoList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;

	if( m_strName.IsEmpty()
	&&  m_strAuthor.IsEmpty()
	&&  m_strCopyright.IsEmpty()
	&&  m_strSubject.IsEmpty()
	&&  m_strInfo.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write UNFO LIST header
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Container name
	if( !m_strName.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strName );
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

	// Write Container author
	if( !m_strAuthor.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UART_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strAuthor );
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

	// Write Container copyright
	if( !m_strCopyright.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UCOP_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strCopyright );
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

	// Write Container subject
	if( !m_strSubject.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_USBJ_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strSubject );
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

	// Write Container comments
	if( !m_strInfo.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UCMT_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strInfo );
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
// CDirectMusicContainer SaveGUID
    
HRESULT CDirectMusicContainer::SaveGUID( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write GUID chunk header
	ck.ckid = DMUS_FOURCC_GUID_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Container GUID
	hr = pIStream->Write( &m_guidContainer, sizeof(GUID), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(GUID) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer SaveHeader
    
HRESULT CDirectMusicContainer::SaveHeader( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_CONTAINER_HEADER dmusContainerIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Container chunk header
	ck.ckid = DMUS_FOURCC_CONTAINER_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_CONTAINER_HEADER structure
	memset( &dmusContainerIO, 0, sizeof(DMUS_IO_CONTAINER_HEADER) );

	// Containers in scripts always have the DMUS_CONTAINER_NOLOADS bit set
	if( IsInScript() )
	{
		m_dwFlagsDM |= DMUS_CONTAINER_NOLOADS;
	}
	dmusContainerIO.dwFlags = m_dwFlagsDM;

	// Write Container chunk data
	hr = pIStream->Write( &dmusContainerIO, sizeof(DMUS_IO_CONTAINER_HEADER), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_CONTAINER_HEADER) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer SaveContainer
    
HRESULT CDirectMusicContainer::SaveContainer( IDMUSProdRIFFStream* pIRiffStream )
{
	HRESULT hr = S_OK;

// Save Container header chunk
	hr = SaveHeader( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Container GUID
	hr = SaveGUID( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Container info
	hr = SaveInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Container version
	hr = SaveVersion( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Container objects
	hr = SaveObjects( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

ON_ERROR:
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IPersistStream::Save

HRESULT CDirectMusicContainer::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pIPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pIPersistInfo ) ) )
	{
		pIPersistInfo->GetStreamInfo( &StreamInfo );

		// Validate requested data format
		if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion )
		||  IsEqualGUID( StreamInfo.guidDataFormat, GUID_DirectMusicObject ) )
		{
			// Make sure DocRoot is in stream's embedded file list
			if( m_pIDocRootNode )
			{
				if( pIPersistInfo->IsInEmbeddedFileList(m_pIDocRootNode) == S_FALSE )
				{
					pIPersistInfo->AddToEmbeddedFileList( m_pIDocRootNode );		
				}
			}
		}

		RELEASE( pIPersistInfo );
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

    HRESULT hr = E_FAIL;

	// Validate requested data format
	if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion )
	||  IsEqualGUID( StreamInfo.guidDataFormat, GUID_DirectMusicObject ) )
	{
		// DirectMusic format
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_CONTAINER_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( SaveContainer( pIRiffStream ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				if( fClearDirty )
				{
					SetModified( FALSE );
				}

				hr = S_OK;
			}
			RELEASE( pIRiffStream );
		}
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer IPersistStream::GetSizeMax

HRESULT CDirectMusicContainer::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer Additional functions

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::ReadListInfoFromStream

HRESULT CDirectMusicContainer::ReadListInfoFromStream( IStream* pIStream, DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;
	CString strName;
	CString strDescriptor;
	GUID guidContainer;

	ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	memset( &guidContainer, 0, sizeof(GUID) );

	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_CONTAINER_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			// Get Container GUID
			ck.ckid = DMUS_FOURCC_GUID_CHUNK;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
				dwSize = min( ck.cksize, sizeof( GUID ) );
				if( FAILED ( pIStream->Read( &guidContainer, dwSize, &dwByteCount ) )
				||  dwByteCount != dwSize )
				{
					memset( &guidContainer, 0, sizeof(GUID) );
				}
			}

			// Get Container name
			ck.fccType = DMUS_FOURCC_UNFO_LIST;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDLIST ) == 0 )
			{
				ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
				if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
				{
					ReadMBSfromWCS( pIStream, ck.cksize, &strName );
				}
			}
		}

		RELEASE( pIRiffStream );
	}

	if( !strName.IsEmpty() )
	{
		pListInfo->bstrName = strName.AllocSysString();
		pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
		memcpy( &pListInfo->guidObject, &guidContainer, sizeof(GUID) );

		// Must check pListInfo->wSize before populating additional fields
		return S_OK;
	}

	return E_FAIL;
}


///////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer  IDMUSProdNotifySink::OnUpdate

HRESULT CDirectMusicContainer::OnUpdate( IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Check Embed Runtime folder
	m_FolderEmbed.OnUpdate( pIDocRootNode, guidUpdateType, pData );

	// Check Reference Runtime folder
	m_FolderReference.OnUpdate( pIDocRootNode, guidUpdateType, pData );

	return S_OK;
}


///////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer  IDMUSProdContainerInfo::FindDocRootFromName

HRESULT CDirectMusicContainer::FindDocRootFromName( BSTR bstrName, IUnknown** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( bstrName == NULL )
	{
		return E_INVALIDARG;
	}
	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}
	*ppIDocRootNode = NULL;

	// Check Embed Runtime folder
	HRESULT hr = m_FolderEmbed.FindDocRootFromName( strName, ppIDocRootNode );
	
	if( hr != S_OK )
	{
		// Check Reference Runtime folder
		hr = m_FolderReference.FindDocRootFromName( strName, ppIDocRootNode );
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer  IDMUSProdContainerInfo::FindDocRootFromScriptAlias

HRESULT CDirectMusicContainer::FindDocRootFromScriptAlias( BSTR bstrAlias, IUnknown** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( bstrAlias == NULL )
	{
		return E_INVALIDARG;
	}
	CString strAlias = bstrAlias;
	::SysFreeString( bstrAlias );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}
	*ppIDocRootNode = NULL;

	// Check Embed Runtime folder
	HRESULT hr = m_FolderEmbed.FindDocRootFromScriptAlias( strAlias, ppIDocRootNode );
	
	if( hr != S_OK )
	{
		// Check Reference Runtime folder
		hr = m_FolderReference.FindDocRootFromScriptAlias( strAlias, ppIDocRootNode );
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer  IsCircularReference

HRESULT CDirectMusicContainer::IsCircularReference( IDMUSProdNode* pIDocRootNode, CFolder* pTargetFolder )
{
	if( pIDocRootNode == NULL )
	{
		return E_INVALIDARG;
	}

	GUID guidNodeId;
	if( FAILED ( pIDocRootNode->GetNodeId( &guidNodeId ) ) )
	{
		return E_FAIL;
	}

	HRESULT hr = S_FALSE;

	if( ::IsEqualGUID( guidNodeId, GUID_ContainerNode ) )
	{
		CDirectMusicContainer* pContainer = (CDirectMusicContainer *)pIDocRootNode;

		if( (pContainer == this)
		||  (pContainer->m_FolderEmbed.IsDocRootInFolder( this ) == S_OK)
		||  (pContainer->m_FolderReference.IsDocRootInFolder( this ) == S_OK) )
		{
			hr = S_OK;
		}
	}
	else
	{
		// Need to be careful about what is dropped into the "embed" folder
		if( pTargetFolder == &m_FolderEmbed )
		{
			// Can't embed a DocRoot in it's own child container
			if( pIDocRootNode == m_pIDocRootNode )
			{
				hr = S_OK;
			}
		}
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::IsDocRootInContainer

HRESULT CDirectMusicContainer::IsDocRootInContainer( IDMUSProdNode* pIDocRootNode )
{
	HRESULT hr = S_FALSE;

	if( (m_FolderEmbed.IsDocRootInFolder( pIDocRootNode ) == S_OK)
	||  (m_FolderReference.IsDocRootInFolder( pIDocRootNode ) == S_OK) )
	{
		hr = S_OK;
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::IsFileGUIDInContainer

HRESULT CDirectMusicContainer::IsFileGUIDInContainer( GUID guidFile )
{
	HRESULT hr = S_FALSE;

	if( (m_FolderEmbed.IsFileGUIDInFolder( guidFile ) == S_OK)
	||  (m_FolderReference.IsFileGUIDInFolder( guidFile ) == S_OK) )
	{
		hr = S_OK;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::GetGUID

void CDirectMusicContainer::GetGUID( GUID* pguidContainer )
{
	if( pguidContainer )
	{
		*pguidContainer = m_guidContainer;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::SetGUID

void CDirectMusicContainer::SetGUID( GUID guidContainer )
{
	m_guidContainer = guidContainer;
	SetModified( TRUE );

	// Notify connected nodes that Container GUID has changed
	theApp.m_pContainerComponent->m_pIFramework8->NotifyNodes( this, DOCROOT_GuidChange, NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicContainer::OnAddRemoveFiles

void CDirectMusicContainer::OnAddRemoveFiles( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );

	IDMUSProdProject* pIProject;
	if( FAILED ( theApp.m_pContainerComponent->m_pIFramework8->FindProject( this, &pIProject ) ) )
	{
		return;
	}

	// Store this container's GUID
	GUID guidThisContainer;
	theApp.m_pContainerComponent->m_pIFramework8->GetNodeFileGUID( this, &guidThisContainer );

	// Display Add/Remove Files dialog
	CDlgAddFiles dlgAddFiles = new CDlgAddFiles;

	// Disable "referenced" controls when appropriate
	if( IsInSegment() )
	{
		dlgAddFiles.m_fDisableReferenced = true;
	}

	// Determine dialog title
	BSTR bstrName;
	if( m_pIDocRootNode )
	{
		if( SUCCEEDED ( m_pIDocRootNode->GetNodeName( &bstrName ) ) )
		{
			CString strName = bstrName;
			::SysFreeString( bstrName );

			AfxFormatString1( dlgAddFiles.m_strTitle, IDS_ADD_REMOVE_FILES_TITLE, strName );
		}
	}

	// Create dialog's "Available" list
	IDMUSProdNode* pIFileNode;
	IDMUSProdNode* pINextFileNode;
	GUID guidFile;
	HRESULT hr = pIProject->GetFirstFile( &pINextFileNode );
	while( hr == S_OK )
	{
		pIFileNode = pINextFileNode;

		theApp.m_pContainerComponent->m_pIFramework8->GetNodeFileGUID( pIFileNode, &guidFile );
		if( ::IsEqualGUID(guidFile, guidThisContainer) == FALSE )
		{
			if( IsFileGUIDInContainer(guidFile) != S_OK )
			{
				dlgAddFiles.m_lstAvailable.AddTail( pIFileNode );
			}
		}

		hr = pIProject->GetNextFile( pIFileNode, &pINextFileNode );
		pIFileNode->Release();
	}

	// Create dialog's "Embedded" list
	POSITION pos = m_FolderEmbed.m_lstObjects.GetHeadPosition();
	while( pos )
	{
		CContainerObject* pContainerObject = m_FolderEmbed.m_lstObjects.GetNext( pos );

		dlgAddFiles.m_lstEmbedded.AddTail( pContainerObject->m_FileRef.pIDocRootNode );
	}

	// Create dialog's "References" list
	pos = m_FolderReference.m_lstObjects.GetHeadPosition();
	while( pos )
	{
		CContainerObject* pContainerObject = m_FolderReference.m_lstObjects.GetNext( pos );

		dlgAddFiles.m_lstReferenced.AddTail( pContainerObject->m_FileRef.pIDocRootNode );
	}

	if( dlgAddFiles.DoModal() == IDOK )
	{
		IDMUSProdPropPageManager* pIPageManager = theApp.m_pIPageManager;

		// Need to keep pIPageManager while processing IDOK
		if( pIPageManager )
		{
			pIPageManager->AddRef();
		}

		// Update Container's "Embedded" folder
		m_FolderEmbed.ReplaceContent( dlgAddFiles.m_lstEmbedded );

		// Update Container's "Referenced" folder
		m_FolderReference.ReplaceContent( dlgAddFiles.m_lstReferenced );

		// OK - We don't need pIPageManager any more...
		if( pIPageManager )
		{
			pIPageManager->Release();
		}
	}

	RELEASE( pIProject );
}