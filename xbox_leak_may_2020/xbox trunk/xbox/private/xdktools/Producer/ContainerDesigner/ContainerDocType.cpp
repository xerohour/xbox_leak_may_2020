// ContainerDocType.cpp : implementation file
//

#include "stdafx.h"

#include "ContainerDesignerDLL.h"
#include "Container.h"
#include <mmreg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType constructor/destructor

CContainerDocType::CContainerDocType()
{
    m_dwRef = 0;
}

CContainerDocType::~CContainerDocType()
{
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType IUnknown implementation

HRESULT CContainerDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CContainerDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CContainerDocType::Release()
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
// CContainerDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CContainerDocType::GetResourceId

HRESULT CContainerDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_CONTAINER_DOCTYPE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType::DoesExtensionMatch

HRESULT CContainerDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_CONTAINER_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
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
// CContainerDocType::DoesIdMatch

HRESULT CContainerDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualGUID(rguid, GUID_ContainerNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType::AllocNode

HRESULT STDMETHODCALLTYPE CContainerDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_ContainerNode) )
	{
		// Create a new Container 
		CDirectMusicContainer* pContainer = new CDirectMusicContainer;
		if( pContainer == NULL )
		{
			return E_OUTOFMEMORY ;
		}
			
		// Create the Undo Manager
		if( pContainer->CreateUndoMgr() == FALSE )
		{
			RELEASE( pContainer );
			return E_OUTOFMEMORY;
		}

		*ppINode = (IDMUSProdNode *)pContainer;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType:::OnFileNew

HRESULT CContainerDocType::OnFileNew( IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
									  IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIContainerNode;

	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new Container 
	hr = AllocNode( GUID_ContainerNode, &pIContainerNode );
	if( SUCCEEDED ( hr ) )
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( pIContainerNode, pIContainerNode, NULL );

		// Add node to Project Tree
		if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->AddNode(pIContainerNode, NULL) ) )
		{
			CDirectMusicContainer* pContainer = (CDirectMusicContainer *)pIContainerNode;

			// Store the Container's Project
			pContainer->m_pIProject = pITargetProject;
//			pContainer->m_pIProject->AddRef();			intentionally missing

			// Get the filename
			BSTR bstrName;

			if( SUCCEEDED ( pIContainerNode->GetNodeName( &bstrName ) ) )
			{
				pContainer->m_strOrigFileName = bstrName;
				pContainer->m_strOrigFileName += _T(".cop");
				::SysFreeString( bstrName );
			}

			// Add Container to Container component list 
			theApp.m_pContainerComponent->AddToContainerFileList( pContainer );

			*ppIDocRootNode = pIContainerNode;
			pContainer->SetModified( TRUE );
			hr = S_OK;
		}
		else
		{
			RELEASE( pIContainerNode );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType::OnFileOpen

HRESULT CContainerDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* pITargetProject,
									   IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;
	CDirectMusicContainer* pContainer;
	CString strFileName;
	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pContainerComponent != NULL );
	ASSERT( theApp.m_pContainerComponent->m_pIFramework8 != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;
	// Create a new Container 
	pContainer = new CDirectMusicContainer;
	if( pContainer == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	theApp.m_pContainerComponent->m_nNextContainer--;

	if( pContainer->CreateUndoMgr() == FALSE )
	{
		RELEASE( pContainer );
		return E_OUTOFMEMORY;
	}

	// Store the Container's Project
	pContainer->m_pIProject = pITargetProject;
//	pContainer->m_pIProject->AddRef();			intentionally missing

	// Get the filename
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;

		pPersistInfo->GetFileName( &bstrFileName );
		pContainer->m_strOrigFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		int nFindPos = pContainer->m_strOrigFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			pContainer->m_strOrigFileName = pContainer->m_strOrigFileName.Right( pContainer->m_strOrigFileName.GetLength() - nFindPos - 1 );
		}

		pPersistInfo->Release();
	}

	// Load the Container file
	if( theApp.m_pContainerComponent->m_pContainerBeingLoaded == NULL )
	{
		theApp.m_pContainerComponent->m_pContainerBeingLoaded = pContainer;
		theApp.m_pContainerComponent->m_pIDupeFileTargetProject = pITargetProject;
		theApp.m_pContainerComponent->m_nDupeFileDlgReturnCode = 0;
	}
	hr = pContainer->Load( pIStream );
	if( theApp.m_pContainerComponent->m_pContainerBeingLoaded == pContainer )
	{
		theApp.m_pContainerComponent->m_pContainerBeingLoaded = NULL;
		theApp.m_pContainerComponent->m_pIDupeFileTargetProject = NULL;
		theApp.m_pContainerComponent->m_nDupeFileDlgReturnCode = 0;
	}
	if( !SUCCEEDED ( hr ) )
	{
		RELEASE( pContainer );
		return hr;
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pContainer, pContainer, NULL );

	// Add node to Project Tree
	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->AddNode(pContainer, NULL) ) )
	{
		pContainer->SetModified( FALSE );

		// Add Container to Container component list 
		theApp.m_pContainerComponent->AddToContainerFileList( pContainer );

		*ppIDocRootNode = pContainer;

		hr = S_OK;
	}
	else
	{
		RELEASE( pContainer );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType::OnFileSave

HRESULT CContainerDocType::OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType::GetListInfo

HRESULT CContainerDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo ) 
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

	// Create temporary container object to retrieve list info
	CDirectMusicContainer* pContainer = new CDirectMusicContainer;
	if( pContainer )
	{
		// Create the Undo Manager
		if( pContainer->CreateUndoMgr() )
		{
			hr = pContainer->ReadListInfoFromStream( pIStream, pListInfo );
		}

		RELEASE( pContainer );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType::IsFileTypeExtension

HRESULT CContainerDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	strExt.MakeLower();

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( strExt == _T(".cop") )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( strExt == _T(".con") )
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType::GetObjectDescriptorFromNode

HRESULT CContainerDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a Container Node
	CDirectMusicContainer* pContainer;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_ContainerNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pContainer = (CDirectMusicContainer *)pIDocRootNode;

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
	
	memcpy( &pDMObjectDesc->guidObject, &pContainer->m_guidContainer, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicContainer, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = pContainer->m_vVersion.dwVersionMS;
	pDMObjectDesc->vVersion.dwVersionLS = pContainer->m_vVersion.dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, pContainer->m_strName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType::GetObjectRiffId

HRESULT CContainerDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Container Node
	if( IsEqualGUID ( guidNodeId, GUID_ContainerNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = DMUS_FOURCC_CONTAINER_FORM;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerDocType::GetObjectExt

HRESULT CContainerDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Container Node
	if( IsEqualGUID ( guidNodeId, GUID_ContainerNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = ".cop";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = ".con";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}
