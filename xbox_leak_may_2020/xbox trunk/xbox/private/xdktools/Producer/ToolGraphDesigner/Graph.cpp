// Graph.cpp : implementation file
//

#include "stdafx.h"
#include "ToolGraphDesignerDLL.h"
#include "RiffStrm.h"
#include "Graph.h"
#include "GraphCtl.h"
#include "TabGraph.h"
#include "TabInfo.h"
#include <SegmentDesigner.h>
#include <AudioPathDesigner.h>
#include <dmusicf.h>
#include <mmreg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// {D66E3599-5879-4bc4-A93A-3E05165E27F7}
static const GUID GUID_GraphPropPageManager = 
{ 0xd66e3599, 0x5879, 0x4bc4, { 0xa9, 0x3a, 0x3e, 0x5, 0x16, 0x5e, 0x27, 0xf7 } };

short CGraphPropPageManager::sm_nActiveTab = 0;


//////////////////////////////////////////////////////////////////////
// CGraphPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGraphPropPageManager::CGraphPropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabGraph = NULL;
	m_pTabInfo = NULL;
}

CGraphPropPageManager::~CGraphPropPageManager()
{
	RELEASE( m_pIPropSheet );

	if( m_pTabGraph )
	{
		delete m_pTabGraph;
	}

	if( m_pTabInfo )
	{
		delete m_pTabInfo;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphPropPageManager::RemoveCurrentObject

void CGraphPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphPropPageManager IUnknown implementation

HRESULT CGraphPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CGraphPropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CGraphPropPageManager::Release()
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
// CGraphPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CGraphPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CGraphPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_TOOLGRAPH_TEXT );

	CDirectMusicGraph *pGraph;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pGraph))) )
	{
		CString strNodeName;
		BSTR bstrNodeName;

		if( SUCCEEDED ( pGraph->GetNodeName( &bstrNodeName ) ) )
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
// CGraphPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CGraphPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	// Add Graph tab
	m_pTabGraph = new CTabGraph( this );
	if( m_pTabGraph )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabGraph->m_psp );
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
// CGraphPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CGraphPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CGraphPropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CGraphPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
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
// CGraphPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CGraphPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CGraphPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CGraphPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CGraphPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CGraphPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicGraph* pGraph;
	
	if( m_pIPropPageObject == NULL )
	{
		pGraph = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pGraph ) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current Graph are processed in OnKillFocus
	// messages before setting the new Graph
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabGraph->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new Graph
	m_pTabGraph->SetGraph( pGraph );
	m_pTabInfo->SetGraph( pGraph );

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CGraphPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_GraphPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph constructor/destructor

CDirectMusicGraph::CDirectMusicGraph()
{
	ASSERT( theApp.m_pGraphComponent != NULL );

	// Graph needs Graph Component
	theApp.m_pGraphComponent->AddRef();

    m_dwRef = 0;
	AddRef();

	m_pUndoMgr = NULL;
	m_fModified = FALSE;

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pIDMGraph = NULL;

	m_hWndEditor = NULL;
	m_pGraphCtrl = NULL;

	TCHAR achName[SMALL_BUFFER];
	::LoadString( theApp.m_hInstance, IDS_TOOLGRAPH_TEXT, achName, SMALL_BUFFER );
    m_strName.Format( "%s%d", achName, ++theApp.m_pGraphComponent->m_nNextGraph );

	CoCreateGuid( &m_guidGraph ); 
	m_vVersion.dwVersionMS = 0x00010000;
	m_vVersion.dwVersionLS = 0x00000000;
}

CDirectMusicGraph::~CDirectMusicGraph()
{
	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	// Remove Graph from clipboard
	theApp.FlushClipboard( this );

	// Remove Graph from property sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}

	// Cleanup the Undo Manager
	if( m_pUndoMgr )
	{
		delete m_pUndoMgr;
	}

	// Cleanup references
	RELEASE( m_pIDMGraph );

	// Release tools
	CTool* pTool;
	while( !m_lstGraphTools.IsEmpty() )
	{
		pTool = m_lstGraphTools.RemoveHead();
		RELEASE( pTool );
	}

	// Delete PChannel groups
	CPChannelGroup* pPChannelGroup;
	while( !m_lstGraphPChannelGroups.IsEmpty() )
	{
		pPChannelGroup = m_lstGraphPChannelGroups.RemoveHead();
		delete pPChannelGroup;
	}

	// Release Graph users
	IUnknown* pIUnknown;
	while( !m_lstGraphUsers.IsEmpty() )
	{
		pIUnknown = m_lstGraphUsers.RemoveHead();
		RELEASE( pIUnknown );
	}

	// Graph no longer needs Graph Component
	theApp.m_pGraphComponent->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::CreateUndoMgr

BOOL CDirectMusicGraph::CreateUndoMgr()
{
	// Should only be called once - after Graph first created
	ASSERT( m_pUndoMgr == NULL );

	ASSERT( theApp.m_pGraphComponent != NULL ); 
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL ); 

	m_pUndoMgr = new CJazzUndoMan( theApp.m_pGraphComponent->m_pIFramework );
	if( m_pUndoMgr )
	{
		m_pUndoMgr->SetStreamAttributes( FT_DESIGN, GUID_CurrentVersion );
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::MyEnumTools

CTool* CDirectMusicGraph::MyEnumTools( int nToolIndex )
{
	CTool* pTool = NULL;

	POSITION pos = m_lstGraphTools.FindIndex( nToolIndex );
	if( pos )
	{
		pTool = m_lstGraphTools.GetAt( pos );
	}

	return pTool;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::IndexToTool

CTool* CDirectMusicGraph::IndexToTool( int nToolIndex )
{
	CTool* pTheTool = NULL;

	int nNbrTools = m_lstGraphTools.GetCount();

	if( nToolIndex >= 0
	&&  nToolIndex < nNbrTools )
	{
		POSITION pos = m_lstGraphTools.FindIndex( nToolIndex );
		if( pos )
		{
			pTheTool = m_lstGraphTools.GetAt( pos );
		}
	}

	return pTheTool;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::GetFirstSelectedTool

CTool* CDirectMusicGraph::GetFirstSelectedTool( void )
{
	CTool* pTool = NULL;

	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pToolList = m_lstGraphTools.GetNext( pos );

		if( pToolList->m_fSelected )
		{
			pTool = pToolList;
			break;
		}
	}

	return pTool;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::IsToolNameUnique

BOOL CDirectMusicGraph::IsToolNameUnique( CTool* pTool )
{
	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pToolList = m_lstGraphTools.GetNext( pos );
		if( pToolList != pTool )
		{
			if( pToolList->m_strName.CompareNoCase( pTool->m_strName ) == 0 )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::GetUniqueToolName

void CDirectMusicGraph::GetUniqueToolName( CTool* pTool )
{
	CString	strOrigName;
	CString	strNbr;
	int	nOrigNameLength;
	int	nNbrLength;
	int	i;

	if( pTool->m_strName.IsEmpty() )
	{
		theApp.m_pGraphComponent->GetRegisteredToolName( pTool->m_clsidTool, pTool->m_strName );
		pTool->m_strName += _T( "1" );
	}
	ASSERT( pTool->m_strName.IsEmpty() == FALSE );

	if( pTool->m_strName.GetLength() > DMUS_MAX_NAME )
	{
		pTool->m_strName = pTool->m_strName.Left( DMUS_MAX_NAME );
		pTool->m_strName.TrimRight();
	}

	strOrigName = pTool->m_strName;

	i = 0;
	while( !strOrigName.IsEmpty()  &&  _istdigit(strOrigName[strOrigName.GetLength() - 1]) )
	{
		strNbr = strOrigName.Right(1) + strNbr;
		strOrigName = strOrigName.Left( strOrigName.GetLength() - 1 );
		if( ++i > 6 )
		{
			break;
		}
	}

	nOrigNameLength = strOrigName.GetLength();
	i = _ttoi( strNbr );

	while( IsToolNameUnique( pTool ) == FALSE )
	{
		strNbr.Format( "%d", ++i ); 
		nNbrLength = strNbr.GetLength();
		
		if( (nOrigNameLength + nNbrLength) <= DMUS_MAX_NAME )
		{
			pTool->m_strName = strOrigName + strNbr;
		}
		else
		{
			pTool->m_strName = strOrigName.Left(DMUS_MAX_NAME - nNbrLength) + strNbr;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::ToolToIndex

int CDirectMusicGraph::ToolToIndex( CTool* pTool )
{
	int nTheIndex = 0;

	if( pTool )
	{
		int nIndex = 0;

		POSITION pos = m_lstGraphTools.GetHeadPosition();
		while( pos )
		{
			CTool* pToolList = m_lstGraphTools.GetNext( pos );

			if( pToolList == pTool )
			{
				nTheIndex = nIndex;
				break;
			}

			nIndex++;
		}

	}

	return nTheIndex;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::SyncGraphEditor

void CDirectMusicGraph::SyncGraphEditor( void )
{
	if( m_pGraphCtrl
	&&  m_pGraphCtrl->m_pGraphDlg )
	{
		m_pGraphCtrl->m_pGraphDlg->RefreshControls();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::ResetContentGraphEditor

void CDirectMusicGraph::ResetContentGraphEditor( void )
{
	if( m_pGraphCtrl
	&&  m_pGraphCtrl->m_pGraphDlg )
	{
		m_pGraphCtrl->m_pGraphDlg->ResetContent();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::SyncGraphWithDirectMusic

HRESULT CDirectMusicGraph::SyncGraphWithDirectMusic( void )
{
	CWaitCursor wait;
	IStream* pIMemStream;
	IPersistStream* pIPersistStream;
	HRESULT hr;

	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	// Make sure we have a DirectMusic Engine Graph
	if( m_pIDMGraph == NULL )
	{
		if( FAILED ( ::CoCreateInstance( CLSID_DirectMusicGraph, NULL, CLSCTX_INPROC_SERVER,
										 IID_IDirectMusicGraph, (void**)&m_pIDMGraph ) ) )
		{
			RELEASE( m_pIDMGraph );
		}
	}

	if( m_pIDMGraph == NULL )
	{
		// Nothing to do
		return S_OK;
	}

	hr = theApp.m_pGraphComponent->m_pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );

	if( SUCCEEDED ( hr ) )
	{
		hr = Save( pIMemStream, FALSE );
		if( SUCCEEDED ( hr ) )
		{
			m_pIDMGraph->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
			if( pIPersistStream )
			{
				StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
				hr = pIPersistStream->Load( pIMemStream );

				RELEASE( pIPersistStream );
			}
		}

		RELEASE( pIMemStream );
	}

	// Make sure all "copies" of toolgraph have been updated
	POSITION pos = m_lstGraphUsers.GetHeadPosition();
	while( pos )
	{
		IUnknown* pIUnknown = m_lstGraphUsers.GetNext( pos );

		SyncGraphUser_RemoveTools( pIUnknown );
		SyncGraphUser_AddTools( pIUnknown );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::SetModified

void CDirectMusicGraph::SetModified( BOOL fModified )
{
	m_fModified = fModified;

	if( m_fModified )
	{
		if( m_pIDocRootNode
		&&  m_pIDocRootNode != this )
		{
			// Notify DocRoot that the toolgraph has changed
			IDMUSProdNotifySink* pINotifySink;
			if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
			{
				pINotifySink->OnUpdate( this, TOOLGRAPH_ChangeNotification, NULL );

				RELEASE( pINotifySink );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IUnknown implementation

HRESULT CDirectMusicGraph::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IUnknown) )
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
    else if( ::IsEqualIID(riid, IID_IDMUSProdToolGraphInfo) )
    {
        *ppvObj = (IDMUSProdToolGraphInfo *)this;
    }
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CDirectMusicGraph::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CDirectMusicGraph::Release()
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
// CDirectMusicGraph IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetNodeImageIndex

HRESULT CDirectMusicGraph::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );

	return( theApp.m_pGraphComponent->GetGraphImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetFirstChild

HRESULT CDirectMusicGraph::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// Graph node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetNextChild

HRESULT CDirectMusicGraph::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// Graph node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetComponent

HRESULT CDirectMusicGraph::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );

	return theApp.m_pGraphComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetDocRootNode

HRESULT CDirectMusicGraph::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CDirectMusicGraph IDMUSProdNode::SetDocRootNode

HRESULT CDirectMusicGraph::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetParentNode

HRESULT CDirectMusicGraph::GetParentNode( IDMUSProdNode** ppIParentNode )
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
// CDirectMusicGraph IDMUSProdNode::SetParentNode

HRESULT CDirectMusicGraph::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetNodeId

HRESULT CDirectMusicGraph::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_ToolGraphNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetNodeName

HRESULT CDirectMusicGraph::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetNodeNameMaxLength

HRESULT CDirectMusicGraph::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = DMUS_MAX_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::ValidateNodeName

HRESULT CDirectMusicGraph::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;

	strName = bstrName;
	::SysFreeString( bstrName );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::SetNodeName

HRESULT CDirectMusicGraph::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( m_strName == strName )
	{
		return S_OK;
	}

	m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_GRAPH_NAME );
	m_strName = strName;

	// Sync change with property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		RELEASE( pIPropSheet );
	}
	
	SyncGraphWithDirectMusic();
	SetModified( TRUE );

	if( m_pIDocRootNode
	&&  m_pIDocRootNode != this )
	{
		// Notify DocRoot node that ToolGraph name has changed
		IDMUSProdNotifySink* pINotifySink;

		if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
		{
			pINotifySink->OnUpdate( this, TOOLGRAPH_NameChange, NULL  );
			pINotifySink->Release();
		}
	}
	else
	{
		// Notify connected nodes that ToolGraph name has changed
		theApp.m_pGraphComponent->m_pIFramework->NotifyNodes( this, TOOLGRAPH_NameChange, NULL );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetNodeListInfo

HRESULT CDirectMusicGraph::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strDescriptor;

    pListInfo->bstrName = m_strName.AllocSysString();
    pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
	memcpy( &pListInfo->guidObject, &m_guidGraph, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetEditorClsId

HRESULT CDirectMusicGraph::GetEditorClsId( CLSID* pClsId )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

   *pClsId = CLSID_GraphEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetEditorTitle

HRESULT CDirectMusicGraph::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	::LoadString( theApp.m_hInstance, IDS_TOOLGRAPH_TEXT, achTitle, MID_BUFFER );
	strTitle  = achTitle;
	strTitle += _T(": " );
	strTitle += m_strName;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetEditorWindow

HRESULT CDirectMusicGraph::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::SetEditorWindow

HRESULT CDirectMusicGraph::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::UseOpenCloseImages

HRESULT CDirectMusicGraph::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetRightClickMenuId

HRESULT CDirectMusicGraph::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_GRAPH_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::OnRightClickMenuInit

HRESULT CDirectMusicGraph::OnRightClickMenuInit( HMENU hMenu )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::OnRightClickMenuSelect

HRESULT CDirectMusicGraph::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_RENAME:
			hr = theApp.m_pGraphComponent->m_pIFramework->EditNodeLabel( this );
			break;

		case IDM_PROPERTIES:
			OnShowProperties();
			hr = S_OK;
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::DeleteChildNode

HRESULT CDirectMusicGraph::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// Graph nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::InsertChildNode

HRESULT CDirectMusicGraph::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// Graph nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::DeleteNode

HRESULT CDirectMusicGraph::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIParentNode;

	// Let our parent delete us
	if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
	{
		if( pIParentNode )
		{
			HRESULT hr = pIParentNode->DeleteChildNode( this, fPromptUser );
			RELEASE( pIParentNode );
			return hr;
		}
	}

	// No parent so we will delete ourself
	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	// Remove from Project Tree
	if( theApp.m_pGraphComponent->m_pIFramework->RemoveNode( this, fPromptUser ) == S_FALSE )
	{
		// User cancelled delete
		return E_FAIL;
	}

	// Remove from Component Graph list
	theApp.m_pGraphComponent->RemoveFromGraphFileList( this );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::OnNodeSelChanged

HRESULT CDirectMusicGraph::OnNodeSelChanged( BOOL fSelected )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::CreateDataObject

HRESULT CDirectMusicGraph::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

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

	// Save Graph into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_GRAPH into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pGraphComponent->m_cfGraph, pIStream ) ) )
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
		if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->SaveClipFormat( theApp.m_pGraphComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// Graph nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pGraphComponent->m_cfProducerFile, pIStream ) ) )
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
// CDirectMusicGraph IDMUSProdNode::CanCut

HRESULT CDirectMusicGraph::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::CanCopy

HRESULT CDirectMusicGraph::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::CanDelete

HRESULT CDirectMusicGraph::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode != this )
	{
		ASSERT( m_pIParentNode != NULL );

		return m_pIParentNode->CanDeleteChildNode( this );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::CanDeleteChildNode

HRESULT CDirectMusicGraph::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// Graph node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::CanPasteFromData

HRESULT CDirectMusicGraph::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::PasteFromData

HRESULT CDirectMusicGraph::PasteFromData( IDataObject* pIDataObject )
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
// CDirectMusicGraph IDMUSProdNode::CanChildPasteFromData

HRESULT CDirectMusicGraph::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	ASSERT( 0 );

	return E_NOTIMPL;	// Graph nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::ChildPasteFromData

HRESULT CDirectMusicGraph::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Graph nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNode::GetObject

HRESULT CDirectMusicGraph::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Must be asking for an interface to a CLSID_DMGraph object
	if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicGraph ) )
	{
		if( m_pIDMGraph )
		{
			return m_pIDMGraph->QueryInterface( riid, ppvObject );
		}
	}

    *ppvObject = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdPropPageObject::GetData

HRESULT CDirectMusicGraph::GetData( void** ppData )
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
// CDirectMusicGraph IDMUSProdPropPageObject::SetData

HRESULT CDirectMusicGraph::SetData( void* pData )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdPropPageObject::OnShowProperties

HRESULT CDirectMusicGraph::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	// Get the Graph page manager
	CGraphPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_GraphPropPageManager ) == S_OK )
	{
		pPageManager = (CGraphPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CGraphPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Graph properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CGraphPropPageManager::sm_nActiveTab;

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
// CDirectMusicGraph IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CDirectMusicGraph::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IPersist::GetClassID

HRESULT CDirectMusicGraph::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IPersistStream::IsDirty

HRESULT CDirectMusicGraph::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fModified )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::LoadUIState

HRESULT CDirectMusicGraph::LoadUIState( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*      pIStream;
	MMCKINFO	  ck;
	MMCKINFO	  ckList;
	DWORD		  dwByteCount;
	DWORD		  dwSize;
	DWORD		  dwPos;
    HRESULT       hr = S_OK;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_GRAPH_UI_CHUNK:
				dwSize = min( ck.cksize, sizeof(GraphUI) );
				hr = pIStream->Read( &m_GraphUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_PCHANNELGROUP_UI_LIST: 
					{
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_PCHANNELGROUP_UI_CHUNK:
								{
									CPChannelGroup* pPChannelGroup = new CPChannelGroup();
									if( pPChannelGroup == NULL )
									{
										hr = E_OUTOFMEMORY;
										goto ON_ERROR;
									}
									hr = pIStream->Read( &pPChannelGroup->m_dwNbrPChannels, sizeof(DWORD), &dwByteCount );
									if( FAILED( hr )
									||  dwByteCount != sizeof(DWORD) )
									{
										hr = E_FAIL;
										delete pPChannelGroup;
										goto ON_ERROR;
									}
									pPChannelGroup->m_pdwPChannel = new DWORD[pPChannelGroup->m_dwNbrPChannels];
									if( pPChannelGroup->m_pdwPChannel == NULL )
									{
										hr = E_OUTOFMEMORY;
										delete pPChannelGroup;
										goto ON_ERROR;
									}
									dwSize = pPChannelGroup->m_dwNbrPChannels * sizeof(DWORD);
									ASSERT( dwSize == (ckList.cksize - sizeof(DWORD)) );
									hr = pIStream->Read( pPChannelGroup->m_pdwPChannel, dwSize, &dwByteCount );
									if( FAILED( hr )
									||  dwByteCount != dwSize )
									{
										hr = E_FAIL;
										delete pPChannelGroup;
										goto ON_ERROR;
									}
									InsertPChannelGroup( pPChannelGroup );
									break;
								}
							}
							pIRiffStream->Ascend( &ckList, 0 );
						}
					}
				}
				break;
		}

        pIRiffStream->Ascend( &ck, 0 );
	    dwPos = StreamTell( pIStream );
    }

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::LoadGraph

HRESULT CDirectMusicGraph::LoadGraph( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*      pIStream;
	MMCKINFO	  ck;
	MMCKINFO	  ckList;
	DWORD		  dwByteCount;
	DWORD		  dwSize;
	DWORD		  dwPos;
    HRESULT       hr = S_OK;

	ASSERT( theApp.m_pGraphComponent != NULL );

	// Clear the Graph to prepare for load
	{
		// Remove pointers from Graph editor (when open)
		ResetContentGraphEditor();

		// Following strings only saved when they have values
		// So make sure they are initialized!!
		m_strAuthor.Empty();
		m_strCopyright.Empty();
		m_strSubject.Empty();
		m_strInfo.Empty();

		// Release tools
		CTool* pTool;
		while( !m_lstGraphTools.IsEmpty() )
		{
			pTool = m_lstGraphTools.RemoveHead();
			RELEASE( pTool );
		}

		// Delete PChannel groups
		CPChannelGroup* pPChannelGroup;
		while( !m_lstGraphPChannelGroups.IsEmpty() )
		{
			pPChannelGroup = m_lstGraphPChannelGroups.RemoveHead();
			delete pPChannelGroup;
		}
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_GUID_CHUNK:
				dwSize = min( ck.cksize, sizeof( GUID ) );
				hr = pIStream->Read( &m_guidGraph, dwSize, &dwByteCount );
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
										theApp.m_pGraphComponent->m_pIFramework->RefreshNode( this );

										// Notify connected nodes that the Graph name has changed
										theApp.m_pGraphComponent->m_pIFramework->NotifyNodes( this, TOOLGRAPH_NameChange, NULL );
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

					case DMUS_FOURCC_GRAPH_UI_LIST:
						hr = LoadUIState( pIRiffStream, &ck );
						if( FAILED ( hr ) )
						{
							goto ON_ERROR;
						}
						break;

					case DMUS_FOURCC_TOOL_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case FOURCC_RIFF:
									switch( ckList.fccType )
									{
										case DMUS_FOURCC_TOOL_FORM: 
										{
											CTool* pTool = new CTool();
											if( pTool == NULL )
											{
												hr = E_OUTOFMEMORY;
												goto ON_ERROR;
											}
											hr = pTool->LoadTool( pIRiffStream, &ckList );
											if( FAILED ( hr ) )
											{
												RELEASE( pTool );
												goto ON_ERROR;
											}
											if( hr == S_FALSE )
											{
												hr = S_OK;
												RELEASE( pTool );
											}
											else
											{
												InsertTool( pTool, -1 );
												RELEASE( pTool );
											}
											break;
										}
									}
									break;
							}
							pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
	    dwPos = StreamTell( pIStream );
    }

	// Refresh Graph editor (when open)
	SyncGraphEditor();

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IPersistStream::Load

HRESULT CDirectMusicGraph::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    HRESULT hr = E_FAIL;

    DWORD dwPos = StreamTell( pIStream );

	// Check for Direct Music format
	BOOL fFoundFormat = FALSE;
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_TOOLGRAPH_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			fFoundFormat = TRUE;
			hr = LoadGraph( pIRiffStream, &ckMain );
		}

		RELEASE( pIRiffStream );
	}

	if( m_pIDocRootNode )
	{
		// Already in Project Tree so sync changes
		// Sync change with property sheet
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			pIPropSheet->RefreshTitle();
			pIPropSheet->RefreshActivePage();
			RELEASE( pIPropSheet );
		}
	}

	//	Persist Graph to the DirectMusic DLLs.
	if( SUCCEEDED( hr ) )
	{
		SyncGraphWithDirectMusic();
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph Refresh
    
void CDirectMusicGraph::Refresh( void )
{
	// Sync changes with editor
	SyncGraphEditor();

	// Sync change with property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitle();
		pIPropSheet->RefreshActivePage();
		RELEASE( pIPropSheet );
	}

	// Sync changes with DMusic DLLs
	SyncGraphWithDirectMusic();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::GetName

void CDirectMusicGraph::GetName( CString& strName )
{
	strName = m_strName;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph SaveInfoList
    
HRESULT CDirectMusicGraph::SaveInfoList( IDMUSProdRIFFStream* pIRiffStream )
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

	// Write Graph name
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

	// Write Graph author
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

	// Write Graph copyright
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

	// Write Graph subject
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

	// Write Graph comments
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
// CDirectMusicGraph SaveVersion
    
HRESULT CDirectMusicGraph::SaveVersion( IDMUSProdRIFFStream* pIRiffStream )
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
// CDirectMusicGraph SaveGUID
    
HRESULT CDirectMusicGraph::SaveGUID( IDMUSProdRIFFStream* pIRiffStream )
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

	// Write Graph GUID
	hr = pIStream->Write( &m_guidGraph, sizeof(GUID), &dwBytesWritten);
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
// CDirectMusicGraph SaveUIState
    
HRESULT CDirectMusicGraph::SaveUIState( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ckList;
    MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Structures for determining the stream type
	DMUSProdStreamInfo	StreamInfo;
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

	// Don't need to save unless this is a design stream
	if( ftFileType != FT_DESIGN )
	{
		hr = S_OK;
		goto ON_ERROR;
	}

	// Write DMUS_FOURCC_GRAPH_UI_LIST header
	ckMain.fccType = DMUS_FOURCC_GRAPH_UI_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save DMUS_FOURCC_GRAPH_UI_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_GRAPH_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write GraphUI data
		hr = pIStream->Write( &m_GraphUI, sizeof(GraphUI), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(GraphUI) )
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

	// Save DMUS_FOURCC_PCHANNELGROUP_UI_LIST chunk
	{
		ckList.fccType = DMUS_FOURCC_PCHANNELGROUP_UI_LIST;
		if( pIRiffStream->CreateChunk(&ckList, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Save PChannel groups
		{
			CPChannelGroup* pPChannelGroup;
			DWORD dwSize;
			POSITION pos;

			pos = m_lstGraphPChannelGroups.GetHeadPosition();
			while( pos )
			{
				pPChannelGroup = m_lstGraphPChannelGroups.GetNext( pos );

				// Save DMUS_FOURCC_PCHANNELGROUP_UI_CHUNK chunk
				ck.ckid = DMUS_FOURCC_PCHANNELGROUP_UI_CHUNK;
				if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Write number of elements in PChannel array
				hr = pIStream->Write( &pPChannelGroup->m_dwNbrPChannels, sizeof(DWORD), &dwBytesWritten);
				if( FAILED( hr )
				||  dwBytesWritten != sizeof(DWORD) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Write PChannel array
				dwSize = pPChannelGroup->m_dwNbrPChannels * sizeof(DWORD);
				hr = pIStream->Write( pPChannelGroup->m_pdwPChannel, dwSize, &dwBytesWritten);
				if( FAILED( hr )
				||  dwBytesWritten != dwSize )
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
		}
		
		if( pIRiffStream->Ascend( &ckList, 0 ) != 0 )
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
// CDirectMusicGraph SaveGraph
    
HRESULT CDirectMusicGraph::SaveGraph( IDMUSProdRIFFStream* pIRiffStream )
{
	HRESULT hr = S_OK;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

// Save Graph GUID
	hr = SaveGUID( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Graph version
	hr = SaveVersion( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Graph info
	hr = SaveInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Graph UI
	hr = SaveUIState( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Tools
	{
		POSITION pos;
		MMCKINFO ck;

		ck.fccType = DMUS_FOURCC_TOOL_LIST;
		if( pIRiffStream->CreateChunk(&ck, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		pos = m_lstGraphTools.GetHeadPosition();
		while( pos )
		{
			CTool* pTool = m_lstGraphTools.GetNext( pos );
			hr = pTool->SaveTool( pIRiffStream );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}
		}
		
		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IPersistStream::Save

HRESULT CDirectMusicGraph::Save( IStream* pIStream, BOOL fClearDirty )
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
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pPersistInfo );
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

    HRESULT hr = E_FAIL;

	// Validate requested data format
	if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion )
	||  IsEqualGUID( StreamInfo.guidDataFormat, GUID_DirectMusicObject )
	||  IsEqualGUID( StreamInfo.guidDataFormat, GUID_SaveSelectedTools ) )
	{
		// DirectMusic format
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_TOOLGRAPH_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( SaveGraph( pIRiffStream ) )
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
// CDirectMusicGraph IPersistStream::GetSizeMax

HRESULT CDirectMusicGraph::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdNotifySink implementation

///////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph  IDMUSProdNotifySink::OnUpdate

HRESULT CDirectMusicGraph::OnUpdate( IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDocRootNode);
	UNREFERENCED_PARAMETER(guidUpdateType);
	UNREFERENCED_PARAMETER(pData);

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdToolGraphInfo implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdToolGraphInfo::EnumTools

HRESULT CDirectMusicGraph::EnumTools( DWORD dwToolIndex, IUnknown** ppIDirectMusicTool )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDirectMusicTool == NULL )
	{
		return E_POINTER;
	}
	*ppIDirectMusicTool = NULL;

	POSITION pos = m_lstGraphTools.FindIndex( dwToolIndex );
	if( pos )
	{
		CTool* pTool = m_lstGraphTools.GetAt( pos );

		ASSERT( pTool->m_pIDMTool != NULL );
		if( pTool->m_pIDMTool )
		{
			pTool->m_pIDMTool->AddRef();
			*ppIDirectMusicTool = pTool->m_pIDMTool;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CompareDWORD()

static int __cdecl CompareDWORD( const void* arg1, const void* arg2 )
{
	if( *(DWORD**)arg1 < *(DWORD**)arg2 )
	{
		return -1;
	}
	if( *(DWORD**)arg1 > *(DWORD**)arg2 )
	{
		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdToolGraphInfo::GetToolInfo

HRESULT CDirectMusicGraph::GetToolInfo( IUnknown* pIDirectMusicTool, DMUSProdToolInfo* pToolInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDirectMusicTool == NULL 
	||  pToolInfo == NULL
	||  pToolInfo->wSize == 0 )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	IDirectMusicTool* pIDMTool;
	if( SUCCEEDED ( pIDirectMusicTool->QueryInterface( IID_IDirectMusicTool, (void**)&pIDMTool ) ) )
	{
		POSITION pos = m_lstGraphTools.GetHeadPosition();
		while( pos )
		{
			CTool* pTool = m_lstGraphTools.GetNext( pos );

			if( pTool->m_pIDMTool == pIDMTool )
			{
				// Build the PChannel array
				CDWordArray aPChannels;
				POSITION pos = pTool->m_lstPChannelGroups.GetHeadPosition();
				while( pos )
				{
					CPChannelGroup* pPChannelGroup = pTool->m_lstPChannelGroups.GetNext( pos );

					for( DWORD i = 0 ;  i < pPChannelGroup->m_dwNbrPChannels ; i++ )
					{
						aPChannels.Add( pPChannelGroup->m_pdwPChannel[i] );
					}
				}

				// Sort the PChannel array
				qsort( aPChannels.GetData(), aPChannels.GetSize(), sizeof(DWORD), CompareDWORD );

				// Set CLSID
				pToolInfo->clsidTool = pTool->m_clsidTool;

				// Set dwFirstPChannel
				if( aPChannels.GetSize() > 0 )
				{
					pToolInfo->dwFirstPChannel = aPChannels[0];
				}
				else
				{
					// This Tool has no PChannels
					//	We must force use of a bogus PChannel
					//	so that tool has no effect
					pToolInfo->dwFirstPChannel = TOOL_NO_PCHANNELS;
				}

				// Set awchToolName
				if( MultiByteToWideChar( CP_ACP, 0, pTool->m_strName, -1, pToolInfo->awchToolName, 65 ) )
				{
					// For PChannels text
					CString strText;
					if( aPChannels.GetSize() > 0 )
					{
						CPChannelGroup* pPChannelGroup = new CPChannelGroup();
						if( pPChannelGroup )
						{
							// Create a new array to store the PChannels in
							DWORD *pdwNewPChannels = new DWORD[aPChannels.GetSize()];
							if( pdwNewPChannels )
							{
								// Copy the PChannels into the new array
								memcpy( pdwNewPChannels, aPChannels.GetData(), sizeof(DWORD) * aPChannels.GetSize() );

								// Set the size of, and point to the new array
								pPChannelGroup->m_pdwPChannel = pdwNewPChannels;
								pPChannelGroup->m_dwNbrPChannels = aPChannels.GetSize();

								// Format text
								pPChannelGroup->FormatPChannelText( strText );
							}
								
							delete pPChannelGroup;
						}
					}

					// Set awchPChannels
					if( MultiByteToWideChar( CP_ACP, 0, strText, -1, pToolInfo->awchPChannels, 128 ) )
					{
						hr = S_OK;
					}
				}
			}
		}

		RELEASE( pIDMTool );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdToolGraphInfo::GetToolCount

HRESULT CDirectMusicGraph::GetToolCount( DWORD* pdwNbrTools )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pdwNbrTools == NULL )
	{
		return E_POINTER;
	}

	*pdwNbrTools = m_lstGraphTools.GetCount();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdToolGraphInfo::AddToGraphUserList

HRESULT CDirectMusicGraph::AddToGraphUserList( IUnknown* pIUnknown )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIUnknown == NULL )
	{
		return E_INVALIDARG;
	}

	// Make sure pIUnknown is not already in the list
	POSITION pos = m_lstGraphUsers.GetHeadPosition();
	while( pos )
	{
		IUnknown* pIUnknownList = m_lstGraphUsers.GetNext( pos );

		if( pIUnknownList == pIUnknown )
		{
			return S_OK;
		}
	}

	// Place pIUnknown in the list
	pIUnknown->AddRef();
	m_lstGraphUsers.AddTail( pIUnknown );

	// Sync corresponding graph
	SyncGraphUser_AddTools( pIUnknown );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdToolGraphInfo::RemoveFromGraphUserList

HRESULT CDirectMusicGraph::RemoveFromGraphUserList( IUnknown* pIUnknown )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIUnknown == NULL )
	{
		return E_INVALIDARG;
	}

	// If item is in m_lstGraphUsers, remove it
	POSITION pos = m_lstGraphUsers.Find( pIUnknown );
	if( pos )
	{
		// Sync corresponding graph
		SyncGraphUser_RemoveTools( pIUnknown );

		// Release our reference on the interface
		pIUnknown->Release();

		// Remove from list
		m_lstGraphUsers.RemoveAt( pos );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph Additional functions

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::ReadListInfoFromStream

HRESULT CDirectMusicGraph::ReadListInfoFromStream( IStream* pIStream, DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;
	CString strName;
	CString strDescriptor;
	GUID guidGraph;

	ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	memset( &guidGraph, 0, sizeof(GUID) );

    DWORD dwPos = StreamTell( pIStream );

	// Check for Direct Music format
	BOOL fFoundFormat = FALSE;
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_TOOLGRAPH_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			fFoundFormat = TRUE;

			// Get Graph GUID
			ck.ckid = DMUS_FOURCC_GUID_CHUNK;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
				dwSize = min( ck.cksize, sizeof( GUID ) );
				if( FAILED ( pIStream->Read( &guidGraph, dwSize, &dwByteCount ) )
				||  dwByteCount != dwSize )
				{
					memset( &guidGraph, 0, sizeof(GUID) );
				}
			}

			// Get Graph name
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
		memcpy( &pListInfo->guidObject, &guidGraph, sizeof(GUID) );

		// Must check pListInfo->wSize before populating additional fields
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::GetObjectDescriptor

HRESULT CDirectMusicGraph::GetObjectDescriptor( void* pObjectDesc )
{
	// Make sure method was passed a valid DMUS_OBJECTDESC pointer
	if( pObjectDesc == NULL ) 
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	DMUS_OBJECTDESC *pDMObjectDesc = (DMUS_OBJECTDESC *)pObjectDesc;

	if( pDMObjectDesc->dwSize == 0 ) 
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	}

	// Initialize DMUS_OBJECTDESC structure
	DWORD dwOrigSize = pDMObjectDesc->dwSize;
	memset( pDMObjectDesc, 0, dwOrigSize );
	pDMObjectDesc->dwSize = dwOrigSize;

	// Set values in DMUS_OBJECTDESC structure
	pDMObjectDesc->dwValidData = (DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS | DMUS_OBJ_VERSION | DMUS_OBJ_NAME );
	
	memcpy( &pDMObjectDesc->guidObject, &m_guidGraph, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicGraph, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = m_vVersion.dwVersionMS;
	pDMObjectDesc->vVersion.dwVersionLS = m_vVersion.dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, m_strName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::InsertPChannelGroup

void CDirectMusicGraph::InsertPChannelGroup( CPChannelGroup* pPChannelGroupToInsert )
{
	if( pPChannelGroupToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	ASSERT( pPChannelGroupToInsert->m_dwNbrPChannels > 0 );

	CPChannelGroup* pPChannelGroup;
	POSITION posCurrent, posNext = m_lstGraphPChannelGroups.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pPChannelGroup = m_lstGraphPChannelGroups.GetNext( posNext );

		ASSERT( pPChannelGroup->m_dwNbrPChannels > 0 );

		if( *pPChannelGroup->m_pdwPChannel > *pPChannelGroupToInsert->m_pdwPChannel )
		{
			m_lstGraphPChannelGroups.InsertBefore( posCurrent, pPChannelGroupToInsert );
			return;
		}
	}

	m_lstGraphPChannelGroups.AddTail( pPChannelGroupToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::RepositionPChannelGroup

void CDirectMusicGraph::RepositionPChannelGroup( CPChannelGroup* pPChannelGroupToReposition )
{
	if( pPChannelGroupToReposition == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	ASSERT( pPChannelGroupToReposition->m_dwNbrPChannels > 0 );

	// Make sure PChannelGroups in tools remain in ascending order
	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pTool = m_lstGraphTools.GetNext( pos );

		pTool->RepositionPChannelGroup( pPChannelGroupToReposition );
	}

	// If item is already in m_lstGraphPChannelGroups, remove it
	pos = m_lstGraphPChannelGroups.Find( pPChannelGroupToReposition );
	if( pos )
	{
		m_lstGraphPChannelGroups.RemoveAt( pos );
	}

	// Reinsert into the list in its new position
	InsertPChannelGroup( pPChannelGroupToReposition );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::RemovePChannelGroup

void CDirectMusicGraph::RemovePChannelGroup( CPChannelGroup* pPChannelGroupToRemove )
{
	if( pPChannelGroupToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Make sure tools do not point to pPChannelGroupToRemove
	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pTool = m_lstGraphTools.GetNext( pos );

		pTool->RemovePChannelGroup( pPChannelGroupToRemove );
	}

	// If item is in m_lstGraphPChannelGroups, remove it
	pos = m_lstGraphPChannelGroups.Find( pPChannelGroupToRemove );
	if( pos )
	{
		m_lstGraphPChannelGroups.RemoveAt( pos );
		delete pPChannelGroupToRemove;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::CreateToolPChannelList

void CDirectMusicGraph::CreateToolPChannelList( CTool* pTool, CPChannelGroup* pPChannelGroup )
{
	ASSERT( pTool->m_lstPChannelGroups.IsEmpty() );
	ASSERT( m_lstGraphPChannelGroups.IsEmpty() == FALSE );

	DWORD dwNbrPChannels = pPChannelGroup->m_dwNbrPChannels;
	DWORD* pdwPChannel = new DWORD[dwNbrPChannels];
	if( pdwPChannel == NULL )
	{
		return;
	}
	memcpy( pdwPChannel, pPChannelGroup->m_pdwPChannel, sizeof(DWORD) * pPChannelGroup->m_dwNbrPChannels );

	POSITION pos = m_lstGraphPChannelGroups.GetHeadPosition();
	while( pos )
	{
		CPChannelGroup* pPChannelGroupList = m_lstGraphPChannelGroups.GetNext( pos );

		if( pPChannelGroupList->m_pdwPChannel
		&&  pPChannelGroupList->m_pdwPChannel[0] == pdwPChannel[0] )
		{
			pTool->InsertPChannelGroup( pPChannelGroupList );

			DWORD dwNext = 0;
			for( DWORD i=0, j=0 ;  i < dwNbrPChannels ;  i++ )
			{
				if( pdwPChannel[i] == pPChannelGroupList->m_pdwPChannel[j] )
				{
					j++;
				}
				else
				{
					pdwPChannel[dwNext++] = pdwPChannel[i];
				}
			}

			dwNbrPChannels = dwNext;
			if( dwNbrPChannels == 0 )
			{
				break;
			}
		}
	}
	
	ASSERT( dwNbrPChannels == 0 );
	delete [] pdwPChannel; 
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::InsertTool

void CDirectMusicGraph::InsertTool( CTool* pToolToInsert, int nIndex )
{
	if( pToolToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	ASSERT( pToolToInsert->m_pIDMTool != NULL );

	// Make sure the tool has a unique name
	GetUniqueToolName( pToolToInsert );

	ASSERT( pToolToInsert->m_pGraph == NULL );
	pToolToInsert->m_pGraph = this;
	pToolToInsert->AddRef();

	// m_lstPChannelGroups may be empty -OR-
	// LoadTool() places exactly one item in a tool's m_lstPChannelGroups
	// The following code uses this item to create a valid
	//		m_lstPChannelGroups list and then deletes it
	if( pToolToInsert->m_lstPChannelGroups.IsEmpty() == FALSE )
	{
		CPChannelGroup* pPChannelGroup = pToolToInsert->m_lstPChannelGroups.RemoveHead();
		CreateToolPChannelList( pToolToInsert, pPChannelGroup );
		delete pPChannelGroup;
	}

	// Place tool in m_lstGraphTools
	POSITION pos = m_lstGraphTools.FindIndex( nIndex );
	if( pos )
	{
		m_lstGraphTools.InsertBefore( pos, pToolToInsert );
		return;
	}

	m_lstGraphTools.AddTail( pToolToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::RemoveTool

void CDirectMusicGraph::RemoveTool( CTool* pToolToRemove )
{
	if( pToolToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Make sure PChannelGroups do not point to pToolToRemove
	POSITION pos = m_lstGraphPChannelGroups.GetHeadPosition();
	while( pos )
	{
		CPChannelGroup* pPChannelGroup = m_lstGraphPChannelGroups.GetNext( pos );

		POSITION posTool = pPChannelGroup->m_lstTools.GetHeadPosition();
		while( posTool )
		{
			POSITION posToolThis = posTool;
			CTool* pTool = pPChannelGroup->m_lstTools.GetNext( posTool );

			if( pTool == pToolToRemove )
			{
				pPChannelGroup->m_lstTools.RemoveAt( posToolThis );
				RELEASE( pTool );
			}
		}
	}

	// Set m_pGraph to NULL
	pToolToRemove->m_pGraph = NULL;

	// If item is in m_lstGraphTools, remove it
	pos = m_lstGraphTools.Find( pToolToRemove );
	if( pos )
	{
		m_lstGraphTools.RemoveAt( pos );
		RELEASE( pToolToRemove );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::IsMultipleSelectTools

BOOL CDirectMusicGraph::IsMultipleSelectTools()
{
	int nSelCount = 0;

	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pTool = m_lstGraphTools.GetNext( pos );

		if( pTool->m_fSelected )
		{
			if( nSelCount > 0 )
			{
				return TRUE;
			}

			nSelCount++;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::SelectAllTools

void CDirectMusicGraph::SelectAllTools()
{
	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pTool = m_lstGraphTools.GetNext( pos );

		pTool->m_fSelected = TRUE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::UnselectAllTools

void CDirectMusicGraph::UnselectAllTools()
{
	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pTool = m_lstGraphTools.GetNext( pos );

		pTool->m_fSelected = FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::MarkSelectedTools

void CDirectMusicGraph::MarkSelectedTools( DWORD dwFlags )
{
	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pTool = m_lstGraphTools.GetNext( pos );

		if( pTool->m_fSelected )
		{
			pTool->m_dwBitsUI |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::UnMarkTools

void CDirectMusicGraph::UnMarkTools( DWORD dwFlags )
{
	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pTool = m_lstGraphTools.GetNext( pos );

		pTool->m_dwBitsUI &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::DeleteMarkedTools

void CDirectMusicGraph::DeleteMarkedTools( DWORD dwFlags )
{
	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pTool = m_lstGraphTools.GetNext( pos );

		if( pTool->m_dwBitsUI & dwFlags )
		{
			RemoveTool( pTool );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::DeleteSelectedTools

void CDirectMusicGraph::DeleteSelectedTools()
{
	POSITION pos = m_lstGraphTools.GetHeadPosition();
	while( pos )
	{
		CTool* pTool = m_lstGraphTools.GetNext( pos );

		if( pTool->m_fSelected )
		{
			RemoveTool( pTool );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::SelectToolsInRange

void CDirectMusicGraph::SelectToolsInRange( int nStartIndex, int nEndIndex )
{
	UnselectAllTools();

	POSITION pos = m_lstGraphTools.FindIndex( nStartIndex );
	while( pos )
	{
		CTool* pToolList = m_lstGraphTools.GetNext( pos );

		pToolList->m_fSelected = TRUE;

		if( ++nStartIndex > nEndIndex )
		{
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::IsInSegment

BOOL CDirectMusicGraph::IsInSegment( void )
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
// CDirectMusicGraph::IsInAudioPath

BOOL CDirectMusicGraph::IsInAudioPath( void )
{
	BOOL fInAudioPath = FALSE;

	if( m_pIParentNode )
	{
		GUID guidNodeId;
		if( SUCCEEDED ( m_pIParentNode->GetNodeId( &guidNodeId ) ) )
		{
			if( ::IsEqualGUID( guidNodeId, GUID_AudioPathNode ) )
			{
				fInAudioPath = TRUE;
			}
		}
	}

	return fInAudioPath;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::UseGraphPChannelGroups

void CDirectMusicGraph::UseGraphPChannelGroups( CTool* pTool )
{
	if( pTool == NULL )
	{
		ASSERT( 0 );
		return;
	}

	// Remove existing PChannelGroups from Tool
	pTool->RemoveAllPChannelGroups();

	// Add all PChannelGroups currently in Graph
	POSITION pos = m_lstGraphPChannelGroups.GetHeadPosition();
	while( pos )
	{
		CPChannelGroup* pPChannelGroup = m_lstGraphPChannelGroups.GetNext( pos );

		// Don't call InsertPChannelGroup() because we do not want PChannelGroup to point to Tool
		pTool->m_lstPChannelGroups.AddTail( pPChannelGroup );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::GetGUID

void CDirectMusicGraph::GetGUID( GUID* pguidGraph )
{
	if( pguidGraph )
	{
		*pguidGraph = m_guidGraph;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph::SetGUID

void CDirectMusicGraph::SetGUID( GUID guidGraph )
{
	m_guidGraph = guidGraph;

	// Sync changes
	SyncGraphWithDirectMusic();
	SetModified( TRUE );

	// Notify connected nodes that Graph GUID has changed
	theApp.m_pGraphComponent->m_pIFramework->NotifyNodes( this, DOCROOT_GuidChange, NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdToolGraphInfo::SyncGraphUser_AddTools

HRESULT CDirectMusicGraph::SyncGraphUser_AddTools( IUnknown* pIUnknown )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIUnknown == NULL )
	{
		return E_INVALIDARG;
	}

	IDirectMusicGraph* pIDMGraph = NULL;

	// Is this an AudioPath?
	IDirectMusicAudioPath* pIDMAudioPath;
	if( SUCCEEDED ( pIUnknown->QueryInterface( IID_IDirectMusicAudioPath, (void**)&pIDMAudioPath ) ) )
	{
		// Get the AudioPath's ToolGraph
		HRESULT hr2 = pIDMAudioPath->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH_GRAPH, 0, GUID_All_Objects, 0,
										IID_IDirectMusicGraph, (void**)&pIDMGraph );
		RELEASE( pIDMAudioPath );
	}
	else
	{
		// Is this a Segment?
		IDirectMusicSegment* pIDMSegment;
		if( SUCCEEDED ( pIUnknown->QueryInterface( IID_IDirectMusicSegment, (void**)&pIDMSegment ) ) )
		{
			MUSIC_TIME mtTimeNow;
			IDirectMusicSegmentState* pIDMSegmentState;

			// Is something playing?
			if( theApp.m_pGraphComponent->m_pIDMPerformance
			&&	SUCCEEDED( theApp.m_pGraphComponent->m_pIDMPerformance->GetTime( NULL, &mtTimeNow ) )
			&&	SUCCEEDED( theApp.m_pGraphComponent->m_pIDMPerformance->GetSegmentState( &pIDMSegmentState, mtTimeNow ) ) )
			{
				// Get the segment that is playing
				IDirectMusicSegment* pIDMSegmentThatIsPlaying;
				if( SUCCEEDED( pIDMSegmentState->GetSegment( &pIDMSegmentThatIsPlaying ) ) )
				{
					if( pIDMSegment == pIDMSegmentThatIsPlaying )
					{
						IDirectMusicSegmentState8* pIDMSegmentState8;
						if( SUCCEEDED ( pIDMSegmentState->QueryInterface( IID_IDirectMusicSegmentState8, (void**)&pIDMSegmentState8 ) ) )
						{
							// Get the Segment's ToolGraph
							pIDMSegmentState8->GetObjectInPath( 0, DMUS_PATH_SEGMENT_GRAPH,
															    0, GUID_All_Objects, 0,
															    IID_IDirectMusicGraph, (void**)&pIDMGraph );

							RELEASE( pIDMSegmentState8 );
						}
					}
		
					RELEASE( pIDMSegmentThatIsPlaying );
				}

				RELEASE( pIDMSegmentState );
			}

			RELEASE( pIDMSegment );
		}
	}

	if( pIDMGraph == NULL )
	{
		return E_UNEXPECTED;
	}

	HRESULT hr = S_OK;

	// Enumerate tools
	CTool* pTool;
	for( DWORD dwIndex = 0 ;  pTool = MyEnumTools(dwIndex) ;  dwIndex++ )
	{
		if( pTool == NULL )
		{
			break;
		}

		// Build the PChannel array
		CDWordArray aPChannels;
		POSITION pos = pTool->m_lstPChannelGroups.GetHeadPosition();
		while( pos )
		{
			CPChannelGroup* pPChannelGroup = pTool->m_lstPChannelGroups.GetNext( pos );

			for( DWORD i = 0 ;  i < pPChannelGroup->m_dwNbrPChannels ; i++ )
			{
				aPChannels.Add( pPChannelGroup->m_pdwPChannel[i] );
			}
		}

		if( aPChannels.GetSize() > 0 )
		{
			// Sort the PChannel array
			qsort( aPChannels.GetData(), aPChannels.GetSize(), sizeof(DWORD), CompareDWORD );
		}
		else
		{
			// This Tool has no PChannels
			//	We must force use of a bogus PChannel
			//	so that tool has no effect
			aPChannels.Add( TOOL_NO_PCHANNELS );
		}

		// Insert tools into the corresponding ToolGraph
		if( pTool->m_pIDMTool )
		{
			if( FAILED ( pIDMGraph->InsertTool( pTool->m_pIDMTool, aPChannels.GetData(), aPChannels.GetSize(), dwIndex ) ) )
			{
				hr = E_FAIL;
			}
		}
	}

	RELEASE( pIDMGraph );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicGraph IDMUSProdToolGraphInfo::SyncGraphUser_RemoveTools

HRESULT CDirectMusicGraph::SyncGraphUser_RemoveTools( IUnknown* pIUnknown )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIUnknown == NULL )
	{
		return E_INVALIDARG;
	}

	IDirectMusicGraph* pIDMGraph = NULL;

	// Is this an AudioPath?
	IDirectMusicAudioPath* pIDMAudioPath;
	if( SUCCEEDED ( pIUnknown->QueryInterface( IID_IDirectMusicAudioPath, (void**)&pIDMAudioPath ) ) )
	{
		// Get the AudioPath's ToolGraph
		pIDMAudioPath->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH_GRAPH, 0, GUID_All_Objects, 0,
										IID_IDirectMusicGraph, (void**)&pIDMGraph );
		RELEASE( pIDMAudioPath );
	}
	else
	{
		// Is this a Segment?
		IDirectMusicSegment* pIDMSegment;
		if( SUCCEEDED ( pIUnknown->QueryInterface( IID_IDirectMusicSegment, (void**)&pIDMSegment ) ) )
		{
			MUSIC_TIME mtTimeNow;
			IDirectMusicSegmentState* pIDMSegmentState;

			// Is something playing?
			if( theApp.m_pGraphComponent->m_pIDMPerformance
			&&	SUCCEEDED( theApp.m_pGraphComponent->m_pIDMPerformance->GetTime( NULL, &mtTimeNow ) )
			&&	SUCCEEDED( theApp.m_pGraphComponent->m_pIDMPerformance->GetSegmentState( &pIDMSegmentState, mtTimeNow ) ) )
			{
				// Get the segment that is playing
				IDirectMusicSegment* pIDMSegmentThatIsPlaying;
				if( SUCCEEDED( pIDMSegmentState->GetSegment( &pIDMSegmentThatIsPlaying ) ) )
				{
					if( pIDMSegment == pIDMSegmentThatIsPlaying )
					{
						IDirectMusicSegmentState8* pIDMSegmentState8;
						if( SUCCEEDED ( pIDMSegmentState->QueryInterface( IID_IDirectMusicSegmentState8, (void**)&pIDMSegmentState8 ) ) )
						{
							// Get the Segment's ToolGraph
							pIDMSegmentState8->GetObjectInPath( 0, DMUS_PATH_SEGMENT_GRAPH,
															    0, GUID_All_Objects, 0,
															    IID_IDirectMusicGraph, (void**)&pIDMGraph );

							RELEASE( pIDMSegmentState8 );
						}
					}
		
					RELEASE( pIDMSegmentThatIsPlaying );
				}

				RELEASE( pIDMSegmentState );
			}

			RELEASE( pIDMSegment );
		}
	}

	if( pIDMGraph == NULL )
	{
		return E_UNEXPECTED;
	}

	HRESULT hr = S_OK;

	// Enumerate the tools
	IDirectMusicTool* pIDMTool;
	int i = 0;
	while( pIDMGraph->GetTool( i, &pIDMTool ) == S_OK )
	{
		bool fRemove = false;

		IPersistStream* pIPersistStream;
		if( SUCCEEDED ( pIDMTool->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
		{
			CLSID clsidTool;
			if( SUCCEEDED ( pIPersistStream->GetClassID( &clsidTool ) ) )
			{
				if( theApp.m_pGraphComponent->IsRegisteredTool( clsidTool ) )
				{
					fRemove = true;
				}
			}

			RELEASE( pIPersistStream );
		}

		if( fRemove )
		{
			pIDMGraph->RemoveTool( pIDMTool );
		}
		else
		{
			i++;
		}

		RELEASE( pIDMTool );
	}

	RELEASE( pIDMGraph );

	return hr;
}
