// GraphDocType.cpp : implementation file
//

#include "stdafx.h"
#include "ToolGraphDesignerDLL.h"
#include "Graph.h"
#include <dmusicf.h>
#include <mmreg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType constructor/destructor

CGraphDocType::CGraphDocType()
{
    m_dwRef = 0;
}

CGraphDocType::~CGraphDocType()
{
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType IUnknown implementation

HRESULT CGraphDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdDocType)
    ||  ::IsEqualIID(riid, IID_IDMUSProdDocType8)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdDocType8 *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CGraphDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CGraphDocType::Release()
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
// CGraphDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::GetResourceId

HRESULT CGraphDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_GRAPH_DOCTYPE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::DoesExtensionMatch

HRESULT CGraphDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_GRAPH_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
	{
		return E_FAIL;
	}

	if( AfxExtractSubString(strDocTypeExt, achBuffer, CDocTemplate::filterExt) )
	{
		ASSERT( strDocTypeExt[0] == '.' );

		BOOL fContinue = TRUE;
		CString strDocExt;
		int nFindPos;

		nFindPos = strDocTypeExt.Find( _T(";") );
		while( fContinue )
		{
			if( nFindPos == -1 )
			{
				fContinue = FALSE;

				nFindPos = strDocTypeExt.Find( _T(".") );
				if( nFindPos != 0 )
				{
					break;
				}
				strDocExt = strDocTypeExt;
			}
			else
			{
				strDocExt = strDocTypeExt.Left( nFindPos );
				strDocTypeExt = strDocTypeExt.Right( strDocTypeExt.GetLength() - (nFindPos + 1) ); 
			}

			if( _tcsicmp(strExt, strDocExt) == 0 )
			{
				return S_OK;	// extension matches 
			}

			nFindPos = strDocTypeExt.Find( _T(";") );
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::DoesIdMatch

HRESULT CGraphDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualGUID(rguid, GUID_ToolGraphNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::AllocNode

HRESULT STDMETHODCALLTYPE CGraphDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_ToolGraphNode) )
	{
		// Create a new Graph 
		CDirectMusicGraph* pGraph = new CDirectMusicGraph;
		if( pGraph == NULL )
		{
			return E_OUTOFMEMORY ;
		}

		// Create the Undo Manager
		if( pGraph->CreateUndoMgr() == FALSE )
		{
			RELEASE( pGraph );
			return E_OUTOFMEMORY;
		}

		*ppINode = (IDMUSProdNode *)pGraph;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType:::OnFileNew

HRESULT CGraphDocType::OnFileNew( IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
								  IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIGraphNode;

	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new Graph 
	hr = AllocNode( GUID_ToolGraphNode, &pIGraphNode );
	if( SUCCEEDED ( hr ) )
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( pIGraphNode, pIGraphNode, NULL );

		// Add node to Project Tree
		if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->AddNode(pIGraphNode, NULL) ) )
		{
			CDirectMusicGraph* pGraph = (CDirectMusicGraph *)pIGraphNode;

			// Get the filename
			BSTR bstrName;

			if( SUCCEEDED ( pIGraphNode->GetNodeName( &bstrName ) ) )
			{
				pGraph->m_strOrigFileName = bstrName;
				pGraph->m_strOrigFileName += _T(".tgp");
				::SysFreeString( bstrName );
			}

			// Add Graph to Graph component list 
			theApp.m_pGraphComponent->AddToGraphFileList( pGraph );

			*ppIDocRootNode = pIGraphNode;
			pGraph->SetModified( TRUE );
			hr = S_OK;
		}
		else
		{
			RELEASE( pIGraphNode );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::OnFileOpen

HRESULT CGraphDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* pITargetProject,
								   IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;
	CDirectMusicGraph* pGraph;
	CString strFileName;
	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}
	*ppIDocRootNode = NULL;

	// Create a new Graph 
	pGraph = new CDirectMusicGraph;
	if( pGraph == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	theApp.m_pGraphComponent->m_nNextGraph--;

	// Create the Undo Manager
	if( pGraph->CreateUndoMgr() == FALSE )
	{
		RELEASE( pGraph );
		return E_OUTOFMEMORY;
	}

	// Get the filename
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;

		pPersistInfo->GetFileName( &bstrFileName );
		pGraph->m_strOrigFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		int nFindPos = pGraph->m_strOrigFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			pGraph->m_strOrigFileName = pGraph->m_strOrigFileName.Right( pGraph->m_strOrigFileName.GetLength() - nFindPos - 1 );
		}

		pPersistInfo->Release();
	}

	// Load the Graph file
	hr = pGraph->Load( pIStream );
	if( !SUCCEEDED ( hr ) )
	{
		RELEASE( pGraph );
		return hr;
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pGraph, pGraph, NULL );

	// Add node to Project Tree
	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->AddNode(pGraph, NULL) ) )
	{
		pGraph->SetModified( FALSE );

		// Add Graph to Graph component list 
		theApp.m_pGraphComponent->AddToGraphFileList( pGraph );

		*ppIDocRootNode = pGraph;

		hr = S_OK;
	}
	else
	{
		RELEASE( pGraph );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::OnFileSave

HRESULT CGraphDocType::OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::GetListInfo

HRESULT CGraphDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	// Create temporary graph object to retrieve list info
	CDirectMusicGraph* pGraph = new CDirectMusicGraph;
	if( pGraph )
	{
		// Create the Undo Manager
		if( pGraph->CreateUndoMgr() )
		{
			hr = pGraph->ReadListInfoFromStream( pIStream, pListInfo );
		}

		RELEASE( pGraph );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::IsFileTypeExtension

HRESULT CGraphDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	strExt.MakeLower();

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( strExt == _T(".tgp") )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( strExt == _T(".tgr") )
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::GetObjectDescriptorFromNode

HRESULT CGraphDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a Graph Node
	CDirectMusicGraph* pGraph;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_ToolGraphNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pGraph = (CDirectMusicGraph *)pIDocRootNode;

	return pGraph->GetObjectDescriptor( pObjectDesc );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::GetObjectRiffId

HRESULT CGraphDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Graph Node
	if( IsEqualGUID ( guidNodeId, GUID_ToolGraphNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = DMUS_FOURCC_TOOLGRAPH_FORM;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDocType::GetObjectExt

HRESULT CGraphDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Graph Node
	if( IsEqualGUID ( guidNodeId, GUID_ToolGraphNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = ".tgp";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = ".tgr";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}
