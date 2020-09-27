// ScriptDocType.cpp : implementation file
//

#include "stdafx.h"

#include "ScriptDesignerDLL.h"
#include "Script.h"
#include <dmusicf.h>
#include <mmreg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType constructor/destructor

CScriptDocType::CScriptDocType()
{
    m_dwRef = 0;
}

CScriptDocType::~CScriptDocType()
{
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType IUnknown implementation

HRESULT CScriptDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CScriptDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CScriptDocType::Release()
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
// CScriptDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CScriptDocType::GetResourceId

HRESULT CScriptDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_SCRIPT_DOCTYPE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType::DoesExtensionMatch

HRESULT CScriptDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_SCRIPT_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
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
// CScriptDocType::DoesIdMatch

HRESULT CScriptDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualGUID(rguid, GUID_ScriptNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType::AllocNode

HRESULT STDMETHODCALLTYPE CScriptDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_ScriptNode) )
	{
		// Create a new Script 
		CDirectMusicScript* pScript = new CDirectMusicScript;
		if( pScript == NULL )
		{
			return E_OUTOFMEMORY ;
		}

		// Create the Undo Manager
		if( pScript->CreateUndoMgr() == FALSE )
		{
			RELEASE( pScript );
			return E_OUTOFMEMORY;
		}

		// Create a Container
		HRESULT hr = pScript->CreateEmptyContainer();
		if( FAILED ( hr ) )
		{
			RELEASE( pScript );
			return hr;
		}

		*ppINode = (IDMUSProdNode *)pScript;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType:::OnFileNew

HRESULT CScriptDocType::OnFileNew( IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
								  IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIScriptNode;

	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new Script 
	hr = AllocNode( GUID_ScriptNode, &pIScriptNode );
	if( SUCCEEDED ( hr ) )
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( pIScriptNode, pIScriptNode, NULL );

		// Add node to Project Tree
		if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->AddNode(pIScriptNode, NULL) ) )
		{
			CDirectMusicScript* pScript = (CDirectMusicScript *)pIScriptNode;

			// Store the Script's Project
			pScript->m_pIProject = pITargetProject;
//			pScript->m_pIProject->AddRef();			intentionally missing

			// Get the filename
			BSTR bstrName;

			if( SUCCEEDED ( pIScriptNode->GetNodeName( &bstrName ) ) )
			{
				pScript->m_strOrigFileName = bstrName;
				pScript->m_strOrigFileName += _T(".spp");
				::SysFreeString( bstrName );
			}

			// Add Script to Script component list 
			theApp.m_pScriptComponent->AddToScriptFileList( pScript );

			*ppIDocRootNode = pIScriptNode;
			pScript->SetModified( TRUE );
			hr = S_OK;
		}
		else
		{
			RELEASE( pIScriptNode );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType::OnFileOpen

HRESULT CScriptDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* pITargetProject,
								    IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;
	CDirectMusicScript* pScript;
	CString strFileName;
	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;
	// Create a new Script 
	pScript = new CDirectMusicScript;
	if( pScript == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	theApp.m_pScriptComponent->m_nNextScript--;

	// Create the Undo Manager
	if( pScript->CreateUndoMgr() == FALSE )
	{
		RELEASE( pScript );
		return E_OUTOFMEMORY;
	}

	// Store the Script's Project
	pScript->m_pIProject = pITargetProject;
//	pScript->m_pIProject->AddRef();			intentionally missing

	// Get the filename
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;

		pPersistInfo->GetFileName( &bstrFileName );
		pScript->m_strOrigFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		int nFindPos = pScript->m_strOrigFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			pScript->m_strOrigFileName = pScript->m_strOrigFileName.Right( pScript->m_strOrigFileName.GetLength() - nFindPos - 1 );
		}

		pPersistInfo->Release();
	}

	// Load the Script file
	hr = pScript->Load( pIStream );
	if( !SUCCEEDED ( hr ) )
	{
		RELEASE( pScript );
		return hr;
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pScript, pScript, NULL );

	// Add node to Project Tree
	if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->AddNode(pScript, NULL) ) )
	{
		pScript->SetModified( FALSE );

		// Add Script to Script component list 
		theApp.m_pScriptComponent->AddToScriptFileList( pScript );

		*ppIDocRootNode = pScript;

		hr = S_OK;
	}
	else
	{
		RELEASE( pScript );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType::OnFileSave

HRESULT CScriptDocType::OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType::GetListInfo

HRESULT CScriptDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo ) 
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

	// Create temporary script object to retrieve list info
	CDirectMusicScript* pScript = new CDirectMusicScript;
	if( pScript )
	{
		// Create the Undo Manager
		if( pScript->CreateUndoMgr() )
		{
			hr = pScript->ReadListInfoFromStream( pIStream, pListInfo );
		}

		RELEASE( pScript );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType::IsFileTypeExtension

HRESULT CScriptDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	strExt.MakeLower();

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( strExt == _T(".spp") )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( strExt == _T(".spt") )
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType::GetObjectDescriptorFromNode

HRESULT CScriptDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a Script Node
	CDirectMusicScript* pScript;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_ScriptNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pScript = (CDirectMusicScript *)pIDocRootNode;

	return pScript->GetObjectDescriptor( pObjectDesc );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType::GetObjectRiffId

HRESULT CScriptDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Script Node
	if( IsEqualGUID ( guidNodeId, GUID_ScriptNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = DMUS_FOURCC_SCRIPT_FORM;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDocType::GetObjectExt

HRESULT CScriptDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Script Node
	if( IsEqualGUID ( guidNodeId, GUID_ScriptNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = ".spp";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = ".spt";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}
