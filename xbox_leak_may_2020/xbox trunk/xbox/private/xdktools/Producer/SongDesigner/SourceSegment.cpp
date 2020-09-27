// SourceSegment.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include "SourceSegment.h"
#include <mmreg.h>
#include "TabSegmentDesign.h"
#include "TabSegmentRuntime.h"
#include <SegmentDesigner.h>
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// {19366A84-93B0-4e6d-ADAB-EE3CD4EFCD9E}
static const GUID GUID_SegmentPropPageManager = 
{ 0x19366a84, 0x93b0, 0x4e6d, { 0xad, 0xab, 0xee, 0x3c, 0xd4, 0xef, 0xcd, 0x9e } };

short CSegmentPropPageManager::sm_nActiveTab = 0;


//////////////////////////////////////////////////////////////////////
// CSegmentPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSegmentPropPageManager::CSegmentPropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabDesign = NULL;
	m_pTabRuntime = NULL;
}

CSegmentPropPageManager::~CSegmentPropPageManager()
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
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentPropPageManager::RemoveCurrentObject

void CSegmentPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentPropPageManager IUnknown implementation

HRESULT CSegmentPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CSegmentPropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CSegmentPropPageManager::Release()
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
// CSegmentPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CSegmentPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CSegmentPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_SONG_TEXT );

	CSourceSegment *pSourceSegment;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pSourceSegment))) )
	{
		CString strName;
		BSTR bstrName;

		strTitle += _T(": ");

		// Add name of Song
		if( pSourceSegment->m_pSong )
		{
			if( SUCCEEDED ( pSourceSegment->m_pSong->GetNodeName( &bstrName ) ) )
			{
				strName = bstrName;
				::SysFreeString( bstrName );
		
				strTitle += strName;
				strTitle += _T(" - ");
			}
		}

		// Add Name of SourceSegment
		if( SUCCEEDED ( pSourceSegment->GetNodeName( &bstrName ) ) )
		{
			strName = bstrName;
			::SysFreeString( bstrName );
		
			strTitle += _T("'");
			strTitle += strName;
			strTitle += _T("' ");
		}

		// Add "Segment"
		CString strSegment;
		strSegment.LoadString( IDS_SEGMENT_TEXT );
		strTitle += strSegment;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CSegmentPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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
	m_pTabDesign = new CTabSegmentDesign( this );
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
	m_pTabRuntime = new CTabSegmentRuntime( this );
	if( m_pTabRuntime )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabRuntime->m_psp );
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
// CSegmentPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CSegmentPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CSegmentPropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CSegmentPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
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
// CSegmentPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CSegmentPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CSegmentPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CSegmentPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CSegmentPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CSegmentPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CSourceSegment* pSourceSegment;
	
	if( m_pIPropPageObject == NULL )
	{
		pSourceSegment = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pSourceSegment ) ) )
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
	m_pTabDesign->SetSourceSegment( pSourceSegment);
	m_pTabRuntime->SetSourceSegment( pSourceSegment);

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CSegmentPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_SegmentPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//	CSourceSegment Constructor/Destructor

CSourceSegment::CSourceSegment( CDirectMusicSong* pSong )
{
	m_dwRef = 0;
	AddRef();

	ASSERT( pSong != NULL );
	m_pSong = pSong;
//	m_pSong->AddRef();			intentionally missing

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;

	SetFileReference( NULL );
//	m_dwFlagsDM = 0;
}

CSourceSegment::~CSourceSegment( void )
{
	// Remove Source Segment from property sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}

	SetFileReference( NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment::CreateRefNode

HRESULT CSourceSegment::CreateRefNode( IDMUSProdNode* pIDocRootNode, IDMUSProdNode** ppIRefNode )
{
	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	if( pIDocRootNode == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	// Get DocRoot's NodeId
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_SegmentNode ) )
		{
			// Now create the reference node
			IDMUSProdComponent* pIComponent;
			if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->FindComponent( CLSID_SegmentComponent, &pIComponent ) ) )
			{
				IDMUSProdNode* pIRefNode;
				if( SUCCEEDED ( pIComponent->AllocReferenceNode( GUID_SegmentRefNode, &pIRefNode ) ) )
				{
					IDMUSProdReferenceNode* pIReferenceNode;
					if( SUCCEEDED ( pIRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void**)&pIReferenceNode ) ) )
					{
						if( SUCCEEDED ( pIReferenceNode->SetReferencedFile( pIDocRootNode ) ) )
						{
							*ppIRefNode = pIRefNode;
							pIRefNode->AddRef();
							hr = S_OK;
						}

						RELEASE( pIReferenceNode );
					}

					RELEASE( pIRefNode );
				}

				RELEASE( pIComponent );
			}
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IUnknown implementation

HRESULT CSourceSegment::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CSourceSegment::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CSourceSegment::Release()
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
// CSourceSegment IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetNodeImageIndex

HRESULT CSourceSegment::GetNodeImageIndex( short* pnFirstImage )
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
// CSourceSegment IDMUSProdNode::GetFirstChild

HRESULT CSourceSegment::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// SourceSegment node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetNextChild

HRESULT CSourceSegment::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// SourceSegment node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetComponent

HRESULT CSourceSegment::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return theApp.m_pSongComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetDocRootNode

HRESULT CSourceSegment::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CSourceSegment IDMUSProdNode::SetDocRootNode

HRESULT CSourceSegment::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetParentNode

HRESULT CSourceSegment::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::SetParentNode

HRESULT CSourceSegment::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetNodeId

HRESULT CSourceSegment::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_Song_SourceSegmentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetNodeName

HRESULT CSourceSegment::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

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

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetNodeNameMaxLength

HRESULT CSourceSegment::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Can't rename Source Segment
	*pnMaxLength = -1;				

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::ValidateNodeName

HRESULT CSourceSegment::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	::SysFreeString( bstrName );

	// Can't rename Source Segment
	ASSERT( 0 );	// This should not happen
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::SetNodeName

HRESULT CSourceSegment::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	::SysFreeString( bstrName );

	// Can't rename Source Segment
	ASSERT( 0 );	// This should not happen
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetNodeListInfo

HRESULT CSourceSegment::GetNodeListInfo( DMUSProdListInfo* pListInfo )
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
// CSourceSegment IDMUSProdNode::GetEditorClsId

HRESULT CSourceSegment::GetEditorClsId( CLSID* pClsId )
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
// CSourceSegment IDMUSProdNode::GetEditorTitle

HRESULT CSourceSegment::GetEditorTitle( BSTR* pbstrTitle )
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
// CSourceSegment IDMUSProdNode::GetEditorWindow

HRESULT CSourceSegment::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_FileRef.pIRefNode  )
	{
		return( m_FileRef.pIRefNode->GetEditorWindow(hWndEditor) );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::SetEditorWindow

HRESULT CSourceSegment::SetEditorWindow( HWND hWndEditor )
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
// CSourceSegment IDMUSProdNode::UseOpenCloseImages

HRESULT CSourceSegment::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetRightClickMenuId

HRESULT CSourceSegment::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId = IDM_SOURCE_SEGMENT_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::OnRightClickMenuInit

HRESULT CSourceSegment::OnRightClickMenuInit( HMENU hMenu )
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
// CSourceSegment IDMUSProdNode::OnRightClickMenuSelect

HRESULT CSourceSegment::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_LOCATE:
			if( m_FileRef.pIDocRootNode )
			{
				theApp.m_pSongComponent->m_pIFramework8->SetSelectedNode( m_FileRef.pIDocRootNode );
			}
			break;

		case IDM_DELETE:
			DeleteNode( TRUE );
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
// CSourceSegment IDMUSProdNode::DeleteChildNode

HRESULT CSourceSegment::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// SourceSegment nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::InsertChildNode

HRESULT CSourceSegment::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// SourceSegment nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::DeleteNode

HRESULT CSourceSegment::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Let our parent delete us
	return m_pIParentNode->DeleteChildNode( this, fPromptUser );
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::OnNodeSelChanged

HRESULT CSourceSegment::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::CreateDataObject

HRESULT CSourceSegment::CreateDataObject( IDataObject** ppIDataObject )
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
// CSourceSegment IDMUSProdNode::CanCut

HRESULT CSourceSegment::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::CanCopy

HRESULT CSourceSegment::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_FileRef.pIRefNode )
	{
		return m_FileRef.pIRefNode->CanCopy();	
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::CanDelete

HRESULT CSourceSegment::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::CanDeleteChildNode

HRESULT CSourceSegment::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Container Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::CanPasteFromData

HRESULT CSourceSegment::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Let parent decide whether it wants to paste
	return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::PasteFromData

HRESULT CSourceSegment::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Let parent handle pasting of Container
	return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::CanChildPasteFromData

HRESULT CSourceSegment::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												 BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// Container Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::ChildPasteFromData

HRESULT CSourceSegment::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// Container Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdNode::GetObject

HRESULT CSourceSegment::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdPropPageObject::GetData

HRESULT CSourceSegment::GetData( void** ppData )
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
// CSourceSegment IDMUSProdPropPageObject::SetData

HRESULT CSourceSegment::SetData( void* pData )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment IDMUSProdPropPageObject::OnShowProperties

HRESULT CSourceSegment::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Get the SourceSegment page manager
	CSegmentPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_SegmentPropPageManager ) == S_OK )
	{
		pPageManager = (CSegmentPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CSegmentPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the SourceSegment's properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CSegmentPropPageManager::sm_nActiveTab;

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
// CSourceSegment IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CSourceSegment::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
//	CSourceSegment::ClearListInfo

void CSourceSegment::ClearListInfo()
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


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment GetSegmentDocRootNode

HRESULT CSourceSegment::GetSegmentDocRootNode( IDMUSProdNode** ppISegmentDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_FileRef.pIDocRootNode )
	{
		m_FileRef.pIDocRootNode->AddRef();
		*ppISegmentDocRootNode = m_FileRef.pIDocRootNode;
		return S_OK;
	}

	*ppISegmentDocRootNode = NULL;
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////
//	CSourceSegment::SetFileReference

HRESULT CSourceSegment::SetFileReference( IDMUSProdNode* pINewDocRootNode )
{
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	HRESULT hr = S_OK;

	// Clean up old file reference
	if( m_FileRef.pIDocRootNode )
	{
		// Turn off notifications for this node
		if( m_FileRef.fRemoveNotify )
		{
			ASSERT( m_pSong != NULL );
			if( m_pSong )
			{
				hr = theApp.m_pSongComponent->m_pIFramework8->RemoveFromNotifyList( m_FileRef.pIDocRootNode, m_pSong );
			}
		}
	}

	// Initialize pertinent fields
	RELEASE( m_FileRef.pIDocRootNode );
	RELEASE( m_FileRef.pIRefNode );
	m_FileRef.fRemoveNotify = FALSE;
	ClearListInfo();

	// Set DocRoot of new file reference
	if( pINewDocRootNode )
	{
		IDMUSProdNode* pINewRefNode;

		hr = CreateRefNode( pINewDocRootNode, &pINewRefNode );
		if( SUCCEEDED ( hr ) )
		{
			// Turn on notifications
			ASSERT( m_pSong != NULL );
			if( m_pSong )
			{
				ASSERT( m_FileRef.fRemoveNotify == FALSE );
				hr = theApp.m_pSongComponent->m_pIFramework8->AddToNotifyList( pINewDocRootNode, m_pSong );
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
		}
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
//	CSourceSegment::SetFileReference

HRESULT CSourceSegment::SetFileReference( GUID guidFile )
{
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	HRESULT hr = E_FAIL;

	IDMUSProdNode* pIDocRootNode;
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->FindDocRootNodeByFileGUID( guidFile, &pIDocRootNode ) ) )
	{
		hr = SetFileReference( pIDocRootNode );

		RELEASE( pIDocRootNode );
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
//	CSourceSegment::SyncListInfo

HRESULT CSourceSegment::SyncListInfo( void )
{
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

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
			if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->FindProject( m_FileRef.pIDocRootNode, &pIProject ) ) )
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

		    theApp.m_pSongComponent->m_pIFramework8->GetNodeFileGUID( m_FileRef.pIDocRootNode, &m_FileRef.li.guidFile );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment Load

HRESULT CSourceSegment::Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, BOOL* pfReference )
{
    IStream*       pIStream;
	MMCKINFO	   ck;
	DWORD		   dwByteCount;
	DWORD		   dwSize;
	DWORD		   dwCurrentFilePos;
	DWORD		   dwStartFilePos;
    HRESULT        hr = S_OK;
    HRESULT        hrReference = S_OK;

	CString		   strSourceSegmentName;
	IDMUSProdNode* pIDocRootNode = NULL;
	BOOL		   fWasReference = FALSE;
	short		   nReferenceFlagUI = -1;

	ASSERT( theApp.m_pSongComponent != NULL );

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
			case DMUS_FOURCC_SOURCE_SEGMENT_UI_CHUNK:
			{
			    ioSourceSegmentUI iSourceSegmentUI;

				dwSize = min( ck.cksize, sizeof( ioSourceSegmentUI ) );
				hr = pIStream->Read( &iSourceSegmentUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( iSourceSegmentUI.wFlags & SSUI_REFERENCE )
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
				hr = theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdFileRefChunk, (void**) &pIFileRef );
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
						if( SUCCEEDED ( m_pSong->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
						{
							// Store File's GUID so we can resolve reference in our handler 
							// for the FRAMEWORK_FileLoadFinished notification
							StreamSeek( pIStream, dwCurrentFilePos, 0 );
							pIFileRef->ResolveWhenLoadFinished( pIStream, pINotifySink, &m_FileRef.li.guidFile );
							RELEASE( pINotifySink );
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
							// Store SourceSegment name
							ReadMBSfromWCS( pIStream, ckName.cksize, &strSourceSegmentName );
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
			// Resolve reference
			if( !(strSourceSegmentName.IsEmpty()) ) 
			{
				// Framework could not resolve file reference
				// so we will ask user to help
				hrReference = FindSegmentFile( strSourceSegmentName, pIStream, &pIDocRootNode );
				if( SUCCEEDED ( hrReference ) )
				{
					if( m_pSong->IsSourceSegmentInSong( pIDocRootNode ) )
					{
						// Already in SourceSegment list
						// so we do not want another reference to the DocRoot
						RELEASE( pIDocRootNode );
					}
					else
					{
						m_pSong->SetModified( TRUE );
					}
				}
			}
		}
		else
		{
			// Load embedded file
			StreamSeek( pIStream, dwStartFilePos, 0 );

			MMCKINFO ckEmbeddedFile;
			ckEmbeddedFile.ckid = FOURCC_RIFF;
			ckEmbeddedFile.fccType = DMUS_FOURCC_SEGMENT_FORM;

			if( pIRiffStream->Descend( &ckEmbeddedFile, pckMain, MMIO_FINDRIFF ) == 0 )
			{
				StreamSeek( pIStream, -12, STREAM_SEEK_CUR );
				pIDocRootNode = theApp.m_pSongComponent->LoadEmbeddedSegment( pIStream );
			}
		}
	}

	if( pIDocRootNode == NULL
	&&  hrReference == E_PENDING )
	{
		if( nReferenceFlagUI == -1 )
		{
			// Did not encounter UI chunk
			// so preserve manner SourceSegment was stored in the container file
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

		if( nReferenceFlagUI == -1 )
		{
			// Did not encounter UI chunk
			// so preserve manner SourceSegment was stored in the container file
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
// CSourceSegment::SaveDMRef

HRESULT CSourceSegment::SaveDMRef( IDMUSProdRIFFStream* pIRiffStream,
									 IDMUSProdNode* pIDocRootNode, DMUS_OBJECTDESC* pdmusObjectDesc, WhichLoader whichLoader )
{
	IDMUSProdLoaderRefChunk* pIRefChunkLoader;

	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );
	if( theApp.m_pSongComponent->m_pIFramework8 == NULL )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( SUCCEEDED( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdLoaderRefChunk, (void**)&pIRefChunkLoader ) ) )
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
// CSourceSegment::SaveProducerRef

HRESULT CSourceSegment::SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode )
{
	IDMUSProdFileRefChunk* pIFileRefChunk;

	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );
	if( theApp.m_pSongComponent->m_pIFramework8 == NULL )
	{
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if ( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdFileRefChunk, (void**)&pIFileRefChunk ) ) )
	{
		pIFileRefChunk->SaveRefChunk( pIStream, pIDocRootNode );
		pIFileRefChunk->Release();
	}

	pIStream->Release();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceSegment Save

HRESULT CSourceSegment::Save( IDMUSProdRIFFStream* pIRiffStream, BOOL fSaveReference )
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
		return SaveSourceSegmentPendingLoad( pIRiffStream, fSaveReference );
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
	
	// Prepare the DMUS_OBJECTDESC structure
	{
		// Get DocRoot's DocType
		IDMUSProdDocType *pIDocType;
		hr = theApp.m_pSongComponent->m_pIFramework8->FindDocTypeByNodeId( GUID_SegmentNode, &pIDocType );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		IDMUSProdDocType8 *pIDocType8;
		hr = pIDocType->QueryInterface( IID_IDMUSProdDocType8, (void**)&pIDocType8 );
		RELEASE( pIDocType );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		hr = pIDocType8->GetObjectDescriptorFromNode( m_FileRef.pIDocRootNode, &dmusObjectDesc );
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
	}

	// Write DMUS_FOURCC_SONGSEGMENT_LIST header
	ckMain.fccType = DMUS_FOURCC_SONGSEGMENT_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write DMUS_FOURCC_SOURCE_SEGMENT_UI_CHUNK
	if( StreamInfo.ftFileType != FT_RUNTIME )
	{
		ioSourceSegmentUI oSourceSegmentUI;

		// Write chunk header
		ck.ckid = DMUS_FOURCC_SOURCE_SEGMENT_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare ioSourceSegmentUI structure
		memset( &oSourceSegmentUI, 0, sizeof(ioSourceSegmentUI) );

		if( fSaveReference )
		{
			oSourceSegmentUI.wFlags |= SSUI_REFERENCE;
		}

		// Write SourceSegment UI chunk data
		hr = pIStream->Write( &oSourceSegmentUI, sizeof(ioSourceSegmentUI), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(ioSourceSegmentUI) )
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

	// Write SourceSegment data
	if( ::IsEqualGUID(StreamInfo.guidDataFormat, GUID_DirectMusicObject)
	||  StreamInfo.ftFileType != FT_RUNTIME
	||  fSaveReference 
	||  fIsEmbeddedInStream )
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
// CSourceSegment SaveSourceSegmentPendingLoad
    
HRESULT CSourceSegment::SaveSourceSegmentPendingLoad( IDMUSProdRIFFStream* pIRiffStream, BOOL fSaveReference )
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
	if( theApp.m_pSongComponent->m_pEmbeddedFileRootFile )
	{
		if( ::IsEqualGUID(theApp.m_pSongComponent->m_pEmbeddedFileRootFile->guidNotification, m_FileRef.li.guidFile) )
		{
			pTheEmbeddedFile = theApp.m_pSongComponent->m_pEmbeddedFileRootFile;
		}
	}
	if( pTheEmbeddedFile == NULL )
	{
		POSITION pos = theApp.m_pSongComponent->m_lstEmbeddedFiles.GetHeadPosition();
		while( pos )
		{
			EmbeddedFile* pEmbeddedFile = theApp.m_pSongComponent->m_lstEmbeddedFiles.GetNext( pos );

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
		// This method cannot save FT_RUNTIME format because it cannot embed files
		// OK to return without doing anything 
		hr = S_FALSE;
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
	}

	// Write DMUS_FOURCC_SONGSEGMENT_LIST header
	ckMain.fccType = DMUS_FOURCC_SONGSEGMENT_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write DMUS_FOURCC_SOURCE_SEGMENT_UI_CHUNK
	if( StreamInfo.ftFileType != FT_RUNTIME )
	{
		ioSourceSegmentUI oSourceSegmentUI;

		// Write chunk header
		ck.ckid = DMUS_FOURCC_SOURCE_SEGMENT_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare ioSourceSegmentUI structure
		memset( &oSourceSegmentUI, 0, sizeof(ioSourceSegmentUI) );

		if( fSaveReference )
		{
			oSourceSegmentUI.wFlags |= SSUI_REFERENCE;
		}

		// Write SourceSegment UI chunk data
		hr = pIStream->Write( &oSourceSegmentUI, sizeof(ioSourceSegmentUI), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(ioSourceSegmentUI) )
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
// CSourceSegment::FindSegmentFile

HRESULT CSourceSegment::FindSegmentFile( CString strSourceSegmentName, IStream* pIStream, IDMUSProdNode** ppIDocRootNode )
{
	IDMUSProdNode*		pIDocRootNode = NULL;
	IDMUSProdNode*		pITargetDirectoryNode = NULL;
	IDMUSProdDocType*	pIDocType = NULL;
	HRESULT				hr;

	ASSERT( pIStream != NULL );

	// Get DocType for Segments
	hr = theApp.m_pSongComponent->m_pIFramework8->FindDocTypeByNodeId( GUID_SegmentNode, &pIDocType );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Get the target directory
	DMUSProdStreamInfo	StreamInfo;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pITargetDirectoryNode = StreamInfo.pITargetDirectoryNode;
		pPersistInfo->Release();
	}

	// If a target directory is not associated with the stream
	// use DocRoot node
	if( pITargetDirectoryNode == NULL )
	{
		if( m_pSong )
		{
			IDMUSProdNode* pIDocNode;
			if( SUCCEEDED ( m_pSong->GetDocRootNode( &pIDocNode ) ) )
			{
				pITargetDirectoryNode = pIDocNode;
				RELEASE( pIDocNode );
			}
		}
	}

	// See if there is a segment named 'strSourceSegmentName' in this Project
	if( !strSourceSegmentName.IsEmpty() )
	{
		BSTR bstrSourceSegmentName = strSourceSegmentName.AllocSysString();
		hr = theApp.m_pSongComponent->m_pIFramework8->GetBestGuessDocRootNode( pIDocType, bstrSourceSegmentName,
																			   pITargetDirectoryNode, &pIDocRootNode );
		if( FAILED ( hr ) )
		{
			pIDocRootNode = NULL;
			if( hr == E_PENDING )
			{
				// File is in process of being loaded
				// Store temporary GUID so we can resolve reference in our handler 
				// for the FRAMEWORK_FileLoadFinished notification
				ASSERT( m_pSong != NULL );
				if( m_pSong )
				{
					IDMUSProdNotifySink* pINotifySink;
					if( SUCCEEDED ( m_pSong->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
					{
						HRESULT hrResolve;

						bstrSourceSegmentName = strSourceSegmentName.AllocSysString();
						hrResolve = theApp.m_pSongComponent->m_pIFramework8->ResolveBestGuessWhenLoadFinished( 
									pIDocType, bstrSourceSegmentName, pITargetDirectoryNode,
									pINotifySink, &m_FileRef.li.guidFile );
						if( SUCCEEDED ( hrResolve ) )
						{
							EmbeddedFile* pEmbeddedFile = theApp.m_pSongComponent->FindEmbeddedFile( pIDocType, strSourceSegmentName );

							if( pEmbeddedFile
							&&  pEmbeddedFile->dwStreamPos == 0 )
							{
								// Need to special case "root" file
								pEmbeddedFile->guidNotification = m_FileRef.li.guidFile;
							}

							m_FileRef.li.strName = strSourceSegmentName;
						}

						RELEASE( pINotifySink );
					}
				}
				goto ON_ERROR;
			}
		}
	}

	if( pIDocRootNode == NULL )
	{
		// Determine File Open dialog prompt
		CString	strOpenDlgTitle;
		if( strSourceSegmentName.IsEmpty() )
		{
			strOpenDlgTitle.LoadString( IDS_FILE_OPEN_ANY_SEGMENT );
		}
		else
		{
			AfxFormatString1( strOpenDlgTitle, IDS_FILE_OPEN_SEGMENT, strSourceSegmentName );
		}
		BSTR bstrOpenDlgTitle = strOpenDlgTitle.AllocSysString();

		// Display File open dialog
		hr = theApp.m_pSongComponent->m_pIFramework8->OpenFile( pIDocType, bstrOpenDlgTitle, pITargetDirectoryNode, &pIDocRootNode );
		if( hr != S_OK )
		{
			// Did not open a file, or opened file other than Segment file
			// so we do not want this DocRoot
			RELEASE( pIDocRootNode );
		}
	}

ON_ERROR:
	RELEASE( pIDocType );

	*ppIDocRootNode = pIDocRootNode;
	return hr;
}
