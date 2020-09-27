// AudioPathDocType.cpp : implementation file
//

#include "stdafx.h"

#include "AudioPathDesignerDLL.h"
#include "AudioPath.h"
#include <mmreg.h>
#include "DlgNewAudiopath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType constructor/destructor

CAudioPathDocType::CAudioPathDocType()
{
    m_dwRef = 0;
}

CAudioPathDocType::~CAudioPathDocType()
{
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType IUnknown implementation

HRESULT CAudioPathDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualIID(riid, IID_IDMUSProdDocType8)
	||	::IsEqualIID(riid, IID_IUnknown) )
	{
		*ppvObj = (IDMUSProdDocType8 *)this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdDocType) )
	{
		*ppvObj = (IDMUSProdDocType *)this;
	}
	else
	{
		 *ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CAudioPathDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CAudioPathDocType::Release()
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
// CAudioPathDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType::GetResourceId

HRESULT CAudioPathDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_AUDIOPATH_DOCTYPE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType::DoesExtensionMatch

HRESULT CAudioPathDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_AUDIOPATH_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
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
// CAudioPathDocType::DoesIdMatch

HRESULT CAudioPathDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualGUID(rguid, GUID_AudioPathNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType::AllocNode

HRESULT STDMETHODCALLTYPE CAudioPathDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_AudioPathNode) )
	{
		// Create a new AudioPath 
		CDirectMusicAudioPath* pAudioPath = new CDirectMusicAudioPath;
		if( pAudioPath == NULL )
		{
			return E_OUTOFMEMORY ;
		}
			
		// Create the Undo Manager
		if( pAudioPath->CreateUndoMgr() == FALSE )
		{
			RELEASE( pAudioPath );
			return E_OUTOFMEMORY;
		}

		// Display a dialog to allow the user to choose which audiopath to base this one on
		CDlgNewAudiopath dlgNewAudiopath;
		dlgNewAudiopath.m_pAudioPathComponent = theApp.m_pAudioPathComponent;
		if( dlgNewAudiopath.DoModal() != IDOK )
		{
			// User hit cancel button - fail the create
			RELEASE( pAudioPath );
			return E_ABORT;
		}
		else
		{
			// User hit OK button - update the new audiopath
			if( dlgNewAudiopath.m_fBaseOnExistingAudiopath )
			{
				if( dlgNewAudiopath.m_pExistingAudiopath )
				{
					pAudioPath->CopyFrom( dlgNewAudiopath.m_pExistingAudiopath );
				}
				else
				{
					pAudioPath->CopyFrom( dlgNewAudiopath.m_dwDefaultAudiopathID );
				}
			}
		}

		*ppINode = (IDMUSProdNode *)pAudioPath;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType:::OnFileNew

HRESULT CAudioPathDocType::OnFileNew( IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
								  IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pITargetDirectoryNode);
	UNREFERENCED_PARAMETER(pITargetProject);

	IDMUSProdNode* pIAudioPathNode;

	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new AudioPath 
	hr = AllocNode( GUID_AudioPathNode, &pIAudioPathNode );
	if( SUCCEEDED ( hr ) )
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( pIAudioPathNode, pIAudioPathNode, NULL );

		// Add node to Project Tree
		if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->AddNode(pIAudioPathNode, NULL) ) )
		{
			CDirectMusicAudioPath* pAudioPath = (CDirectMusicAudioPath *)pIAudioPathNode;

			// Get the filename
			BSTR bstrName;

			if( SUCCEEDED ( pIAudioPathNode->GetNodeName( &bstrName ) ) )
			{
				pAudioPath->m_strOrigFileName = bstrName;
				pAudioPath->m_strOrigFileName += _T(".aup");
				::SysFreeString( bstrName );
			}

			// Add AudioPath to AudioPath component list 
			theApp.m_pAudioPathComponent->AddToAudioPathFileList( pAudioPath );

			// Advise the ConnectionPoint for this CDirectMusicAudioPath

			// Register CDirectMusicAudioPath with the Conductor.
			theApp.m_pAudioPathComponent->m_pIConductor->RegisterAudiopath( pAudioPath, pAudioPath->GetNameForConductor() );


			*ppIDocRootNode = pIAudioPathNode;
			pAudioPath->SetModified( TRUE );
			hr = S_OK;
		}
		else
		{
			RELEASE( pIAudioPathNode );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType::OnFileOpen

HRESULT CAudioPathDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* pITargetProject,
								   IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pITargetDirectoryNode);
	UNREFERENCED_PARAMETER(pITargetProject);

	CWaitCursor wait;
	CDirectMusicAudioPath* pAudioPath;
	CString strFileName;
	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pAudioPathComponent != NULL );
	ASSERT( theApp.m_pAudioPathComponent->m_pIFramework != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;
	// Create a new AudioPath 
	pAudioPath = new CDirectMusicAudioPath;
	if( pAudioPath == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	theApp.m_pAudioPathComponent->m_nNextAudioPath--;

	if( pAudioPath->CreateUndoMgr() == FALSE )
	{
		RELEASE( pAudioPath );
		return E_OUTOFMEMORY;
	}

	// Get the filename
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;

		pPersistInfo->GetFileName( &bstrFileName );
		pAudioPath->m_strOrigFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		int nFindPos = pAudioPath->m_strOrigFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			pAudioPath->m_strOrigFileName = pAudioPath->m_strOrigFileName.Right( pAudioPath->m_strOrigFileName.GetLength() - nFindPos - 1 );
		}

		pPersistInfo->Release();
	}

	// Load the AudioPath file
	hr = pAudioPath->Load( pIStream );
	if( !SUCCEEDED ( hr ) )
	{
		RELEASE( pAudioPath );
		return hr;
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pAudioPath, pAudioPath, NULL );

	// Add node to Project Tree
	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->AddNode(pAudioPath, NULL) ) )
	{
		pAudioPath->SetModified( FALSE );

		// Add AudioPath to AudioPath component list 
		theApp.m_pAudioPathComponent->AddToAudioPathFileList( pAudioPath );

		// Advise the ConnectionPoint for this CDirectMusicAudioPath

		// Register CDirectMusicAudioPath with the Conductor.
		theApp.m_pAudioPathComponent->m_pIConductor->RegisterAudiopath( pAudioPath, pAudioPath->GetNameForConductor() );

		*ppIDocRootNode = pAudioPath;

		hr = S_OK;
	}
	else
	{
		RELEASE( pAudioPath );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType::OnFileSave

HRESULT CAudioPathDocType::OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDocRootNode);
	UNREFERENCED_PARAMETER(pIStream);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType::GetListInfo

HRESULT CAudioPathDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo ) 
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

	// Create temporary audio path object to retrieve list info
	CDirectMusicAudioPath* pAudioPath = new CDirectMusicAudioPath;
	if( pAudioPath )
	{
		// Create the Undo Manager
		if( pAudioPath->CreateUndoMgr() )
		{
			/* TODO: Enable when the DirectMusic Audiopath object exists
			hr = pAudioPath->ReadListInfoFromStream( pIStream, pListInfo );
			*/
		}

		RELEASE( pAudioPath );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType::IsFileTypeExtension

HRESULT CAudioPathDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	strExt.MakeLower();

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( strExt == _T(".aup") )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( strExt == _T(".aud") )
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType::GetObjectDescriptorFromNode

HRESULT CAudioPathDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a AudioPath Node
	CDirectMusicAudioPath* pAudioPath;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_AudioPathNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pAudioPath = (CDirectMusicAudioPath *)pIDocRootNode;

	return pAudioPath->GetObjectDescriptor( pObjectDesc );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType::GetObjectRiffId

HRESULT CAudioPathDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a AudioPath Node
	if( IsEqualGUID ( guidNodeId, GUID_AudioPathNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = DMUS_FOURCC_AUDIOPATH_FORM;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDocType::GetObjectExt

HRESULT CAudioPathDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a AudioPath Node
	if( IsEqualGUID ( guidNodeId, GUID_AudioPathNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = _T(".aup");
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = _T(".aud");
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}
