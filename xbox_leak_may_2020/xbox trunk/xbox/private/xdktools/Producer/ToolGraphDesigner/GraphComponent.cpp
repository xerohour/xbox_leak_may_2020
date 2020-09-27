// GraphComponent.cpp : implementation file
//

#include "stdafx.h"
#include "ToolGraphDesignerDLL.h"
#include "Graph.h"
#include "GraphRef.h"
#include <dmusici.h>


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent constructor/destructor 

CGraphComponent::CGraphComponent()
{
    m_dwRef = 0;
	
	m_pIFramework = NULL;
	m_pIDMPerformance = NULL;
	m_pIConductor = NULL;

	m_nNextGraph = 0;

	m_cfProducerFile = 0;
	m_cfGraph = 0;
	m_cfGraphList = 0;

	m_pIGraphDocType8 = NULL;
	m_nFirstImage = 0;
}

CGraphComponent::~CGraphComponent()
{
	ReleaseAll();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::ReleaseAll

void CGraphComponent::ReleaseAll( void )
{
 	CDirectMusicGraph* pGraph;
	while( !m_lstGraphs.IsEmpty() )
	{
		pGraph = m_lstGraphs.RemoveHead();
		RELEASE( pGraph );
	}

 	RegisteredTool* pRegisteredTool;
	while( !m_lstRegisteredTools.IsEmpty() )
	{
		pRegisteredTool = m_lstRegisteredTools.RemoveHead();
		delete pRegisteredTool;
	}

	RELEASE( m_pIFramework );
	RELEASE( m_pIDMPerformance );
	RELEASE( m_pIConductor );
	RELEASE( m_pIGraphDocType8 );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent IUnknown implementation

HRESULT CGraphComponent::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdComponent)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdComponent *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdRIFFExt) )
    {
        AddRef();
        *ppvObj = (IDMUSProdRIFFExt *)this;
        return S_OK;
    }
	
    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CGraphComponent::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CGraphComponent::Release()
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
// CGraphComponent IDMUSProdComponent implementation

/////////////////////////////////////////////////////////////////////////////
// CGraphComponent IDMUSProdComponent::Initialize

HRESULT CGraphComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdComponent* pIComponent = NULL;
	TCHAR achErrMsg[MID_BUFFER];
	CString strErrMsg;

	if( m_pIFramework )		// already initialized
	{
		return S_OK;
	}

	ASSERT( pIFramework != NULL );
	ASSERT( pbstrErrMsg != NULL );

	if( pbstrErrMsg == NULL )
	{
		return E_POINTER;
	}

	if( pIFramework == NULL )
	{
		::LoadString( theApp.m_hInstance, IDS_ERR_INVALIDARG, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_INVALIDARG;
	}

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	theApp.m_pGraphComponent = this;
//	theApp.m_pGraphComponent->AddRef();	intentionally missing

	// Get IConductor and IDirectMusicPerformance interface pointers 
	if( FAILED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  FAILED ( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor ) )
	||  FAILED ( m_pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIDMPerformance ) ) )
	{
		ReleaseAll();
		if( pIComponent )
		{
			RELEASE( pIComponent );
		}
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONDUCTOR, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	RELEASE( pIComponent );

	// Add applicable images to the Project Tree control's image list 
	if( FAILED ( AddNodeImageLists() ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_IMAGELIST, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register clipboard formats
	if( RegisterClipboardFormats() == FALSE )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_REGISTER_CF, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register the clipboard format for an .tgp file 
	CString strExt = _T(".tgp");
	BSTR bstrExt = strExt.AllocSysString();
	if( FAILED ( pIFramework->RegisterClipFormatForFile(m_cfGraph, bstrExt) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_CLIPFORMAT, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register applicable doc types with the Framework 
	m_pIGraphDocType8 = new CGraphDocType;
    if( m_pIGraphDocType8 == NULL )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_OUTOFMEMORY;
	}

	m_pIGraphDocType8->AddRef();

	if( FAILED ( pIFramework->AddDocType(m_pIGraphDocType8) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	LoadRegisteredTools();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent IDMUSProdComponent::CleanUp

HRESULT CGraphComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Release references to graphs
	CDirectMusicGraph *pGraph;
	while( !m_lstGraphs.IsEmpty() )
	{
		pGraph = m_lstGraphs.RemoveHead();
		RELEASE( pGraph );
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent IDMUSProdComponent::GetName

HRESULT CGraphComponent::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_GRAPH_COMPONENT_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::IDMUSProdComponent::AllocReferenceNode

HRESULT CGraphComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	// Make sure Component can create Nodes of type guidRefNodeId
	if( !( IsEqualGUID ( guidRefNodeId, GUID_ToolGraphRefNode ) ) )
	{
		return E_INVALIDARG;
	}

	// Create GraphRefNode
	CGraphRef* pGraphRef = new CGraphRef;
	if( pGraphRef == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppIRefNode = (IDMUSProdNode *)pGraphRef;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent IDMUSProdComponent::OnActivateApp

HRESULT CGraphComponent::OnActivateApp( BOOL fActivate )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fActivate);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent IDMUSProdRIFFExt::LoadRIFFChunk

HRESULT CGraphComponent::LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicGraph* pGraph;
	HRESULT hr;

	ASSERT( pIStream != NULL );
	ASSERT( m_pIFramework != NULL );

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

	// Create a new Graph 
	pGraph = new CDirectMusicGraph;
	if( pGraph == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	// Create the Undo Manager
	if( pGraph->CreateUndoMgr() == FALSE )
	{
		pGraph->Release();
		return E_OUTOFMEMORY;
	}

	// Load Graph file
	hr = pGraph->Load( pIStream );
	if( FAILED ( hr ) )
	{
		pGraph->Release();
		return hr;
	}

	*ppINode = (IDMUSProdNode *)pGraph;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent additional functions

/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::AddNodeImageLists

HRESULT CGraphComponent::AddNodeImageLists( void )
{
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, ILC_COLOR16, 4, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_GRAPH_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_GRAPH_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_GRAPHREF) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_GRAPHREF_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( FAILED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstImage ) ) )
	{
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::RegisterClipboardFormats

BOOL CGraphComponent::RegisterClipboardFormats( void )
{
	m_cfProducerFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );
	m_cfGraph = ::RegisterClipboardFormat( CF_GRAPH );
	m_cfGraphList = ::RegisterClipboardFormat( CF_GRAPHLIST );

	if( m_cfProducerFile == 0
	||  m_cfGraph == 0
	||  m_cfGraphList == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::LoadRegisteredTools

HRESULT CGraphComponent::LoadRegisteredTools( void )
{
	HKEY	  hKeyOpen;
	HKEY	  hKeyOpenClsId;
	DWORD	  dwType;
	DWORD	  dwCbData;
	FILETIME  ftFileTime;
	_TCHAR	  achClsId[MID_BUFFER];
	_TCHAR	  achName[MID_BUFFER];

	LONG lResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE,
							  	   _T("Software\\Microsoft\\DirectMusic\\Tools"),
								   0, KEY_READ, &hKeyOpen );
	if( lResult != ERROR_SUCCESS )
	{
		return S_FALSE;
	}

	HRESULT hr = S_FALSE;

	DWORD dwIndex = 0;
	for( ; ; )
	{
		dwCbData = MID_BUFFER;
		lResult  = ::RegEnumKeyEx( hKeyOpen, dwIndex++, achClsId, &dwCbData,
								   NULL, NULL, NULL, &ftFileTime );
		if( lResult != ERROR_SUCCESS )
		{
			break;
		}

		_tcscpy( achName, _T("") );
		lResult    = ::RegOpenKeyEx( hKeyOpen, achClsId,
								     0, KEY_QUERY_VALUE, &hKeyOpenClsId );
		if( lResult == ERROR_SUCCESS )
		{
			dwCbData = MID_BUFFER;
			lResult  = ::RegQueryValueEx( hKeyOpenClsId, _T("ShortName"), NULL,
										  &dwType, (LPBYTE)&achName, &dwCbData );
			if( (lResult == ERROR_SUCCESS)
			&&  (dwType == REG_SZ) )
			{
				CLSID clsid;
				wchar_t awchClsId[80];

				if( MultiByteToWideChar( CP_ACP, 0, achClsId, -1, awchClsId, sizeof(awchClsId) / sizeof(wchar_t) ) != 0 )
				{
					IIDFromString( awchClsId, &clsid );

					IDirectMusicTool* pIDMTool;
					if( SUCCEEDED ( ::CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, IID_IDirectMusicTool, (void**)&pIDMTool ) ) )
					{
						RegisteredTool* pRegisteredTool = new RegisteredTool;
						if( pRegisteredTool )
						{
							pRegisteredTool->clsidTool = clsid;
							pRegisteredTool->strName = achName;

							m_lstRegisteredTools.AddTail( pRegisteredTool );
							hr = S_OK;
						}

						RELEASE( pIDMTool );
					}
				}
			}

			::RegCloseKey( hKeyOpenClsId );
		}
	}

	::RegCloseKey( hKeyOpen );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::GetGraphImageIndex

HRESULT CGraphComponent::GetGraphImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_GRAPH_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::GetGraphRefImageIndex

HRESULT CGraphComponent::GetGraphRefImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_GRAPHREF_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::EnumRegisteredTools

RegisteredTool* CGraphComponent::EnumRegisteredTools( int nIndex )
{
	RegisteredTool* pRegisteredTool = NULL;

	POSITION pos = m_lstRegisteredTools.FindIndex( nIndex );
	if( pos )
	{
		pRegisteredTool = m_lstRegisteredTools.GetAt( pos );
	}

	return pRegisteredTool;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::GetRegisteredToolName

void CGraphComponent::GetRegisteredToolName( CLSID clsidTool, CString& strName )
{
	strName.Empty();

	POSITION pos = m_lstRegisteredTools.GetHeadPosition();
	while( pos )
	{
		RegisteredTool* pRegisteredTool = m_lstRegisteredTools.GetNext( pos );

		if( ::IsEqualGUID( pRegisteredTool->clsidTool, clsidTool ) )
		{
			strName = pRegisteredTool->strName;
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::IsRegisteredTool

BOOL CGraphComponent::IsRegisteredTool( CLSID clsidTool )
{
	POSITION pos = m_lstRegisteredTools.GetHeadPosition();
	while( pos )
	{
		RegisteredTool* pRegisteredTool = m_lstRegisteredTools.GetNext( pos );

		if( ::IsEqualGUID( pRegisteredTool->clsidTool, clsidTool ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::AddToGraphFileList

void CGraphComponent::AddToGraphFileList( CDirectMusicGraph* pGraph )
{
	if( pGraph )
	{
		GUID guidGraph;
		pGraph->GetGUID( &guidGraph );

		// Prevent duplicate object GUIDs
		GUID guidGraphList;
		POSITION pos = m_lstGraphs.GetHeadPosition();
		while( pos )
		{
			CDirectMusicGraph* pGraphList = m_lstGraphs.GetNext( pos );

			pGraphList->GetGUID( &guidGraphList );
			if( ::IsEqualGUID( guidGraphList, guidGraph ) )
			{
				::CoCreateGuid( &guidGraph );
				pGraph->SetGUID( guidGraph );
				break;
			}
		}

		// Add to list
		pGraph->AddRef();
		m_lstGraphs.AddTail( pGraph );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphComponent::RemoveFromGraphFileList

void CGraphComponent::RemoveFromGraphFileList( CDirectMusicGraph* pGraph )
{
	if( pGraph )
	{
		// Remove from list
		POSITION pos = m_lstGraphs.Find( pGraph );
		if( pos )
		{
			m_lstGraphs.RemoveAt( pos );
			pGraph->Release();
		}
	}
}
