// SongDocType.cpp : implementation file
//

#include "stdafx.h"

#include "SongDesignerDLL.h"
#include "Song.h"
#include <dmusicf.h>
#include <mmreg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSongDocType constructor/destructor

CSongDocType::CSongDocType()
{
    m_dwRef = 0;
}

CSongDocType::~CSongDocType()
{
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType IUnknown implementation

HRESULT CSongDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CSongDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CSongDocType::Release()
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
// CSongDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CSongDocType::GetResourceId

HRESULT CSongDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_SONG_DOCTYPE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType::DoesExtensionMatch

HRESULT CSongDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_SONG_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
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
// CSongDocType::DoesIdMatch

HRESULT CSongDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualGUID(rguid, GUID_SongNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType::AllocNode

HRESULT STDMETHODCALLTYPE CSongDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_SongNode) )
	{
		// Create a new Song 
		CDirectMusicSong* pSong = new CDirectMusicSong;
		if( pSong == NULL )
		{
			return E_OUTOFMEMORY ;
		}

		// Create the Undo Manager
		if( pSong->CreateUndoMgr() == FALSE )
		{
			RELEASE( pSong );
			return E_OUTOFMEMORY;
		}

		*ppINode = (IDMUSProdNode *)pSong;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType:::OnFileNew

HRESULT CSongDocType::OnFileNew( IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
								  IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pISongNode;

	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new Song 
	hr = AllocNode( GUID_SongNode, &pISongNode );
	if( SUCCEEDED ( hr ) )
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( pISongNode, pISongNode, NULL );

		// Add node to Project Tree
		if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->AddNode(pISongNode, NULL) ) )
		{
			CDirectMusicSong* pSong = (CDirectMusicSong *)pISongNode;

			// Store the Song's Project
			pSong->m_pIProject = pITargetProject;
//			pSong->m_pIProject->AddRef();			intentionally missing

			// Get the filename
			BSTR bstrName;

			if( SUCCEEDED ( pISongNode->GetNodeName( &bstrName ) ) )
			{
				pSong->m_strOrigFileName = bstrName;
				pSong->m_strOrigFileName += _T(".snp");
				::SysFreeString( bstrName );
			}

			// Add Song to Song component list 
			theApp.m_pSongComponent->AddToSongFileList( pSong );

			// Advise the ConnectionPoint for this CDirectMusicSong
			// Register CDirectMusicSong with the Transport.
			pSong->RegisterWithTransport();

			// Sync changes
			pSong->SyncChanges( SC_DIRECTMUSIC, 0, NULL );

			*ppIDocRootNode = pISongNode;
			pSong->SetModified( TRUE );
			hr = S_OK;
		}
		else
		{
			RELEASE( pISongNode );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType::OnFileOpen

HRESULT CSongDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* pITargetProject,
								    IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;
	CDirectMusicSong* pSong;
	CString strFileName;
	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;
	// Create a new Song 
	pSong = new CDirectMusicSong;
	if( pSong == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	theApp.m_pSongComponent->m_nNextSong--;

	// Create the Undo Manager
	if( pSong->CreateUndoMgr() == FALSE )
	{
		RELEASE( pSong );
		return E_OUTOFMEMORY;
	}

	// Store the Song's Project
	pSong->m_pIProject = pITargetProject;
//	pSong->m_pIProject->AddRef();			intentionally missing

	// Get the filename
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;

		pPersistInfo->GetFileName( &bstrFileName );
		pSong->m_strOrigFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		int nFindPos = pSong->m_strOrigFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			pSong->m_strOrigFileName = pSong->m_strOrigFileName.Right( pSong->m_strOrigFileName.GetLength() - nFindPos - 1 );
		}

		pPersistInfo->Release();
	}

	// Load the Song file
	if( theApp.m_pSongComponent->m_pSongBeingLoaded == NULL )
	{
		theApp.m_pSongComponent->m_pSongBeingLoaded = pSong;
		theApp.m_pSongComponent->m_pIDupeFileTargetProject = pITargetProject;
		theApp.m_pSongComponent->m_nDupeFileDlgReturnCode = 0;
	}
	hr = pSong->Load( pIStream );
	if( theApp.m_pSongComponent->m_pSongBeingLoaded == pSong )
	{
		theApp.m_pSongComponent->m_pSongBeingLoaded = NULL;
		theApp.m_pSongComponent->m_pIDupeFileTargetProject = NULL;
		theApp.m_pSongComponent->m_nDupeFileDlgReturnCode = 0;
	}
	if( !SUCCEEDED ( hr ) )
	{
		RELEASE( pSong );
		return hr;
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pSong, pSong, NULL );

	// Add node to Project Tree
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->AddNode(pSong, NULL) ) )
	{
		pSong->SetModified( FALSE );

		// Add Song to Song component list 
		theApp.m_pSongComponent->AddToSongFileList( pSong );

		// Advise the ConnectionPoint for this CDirectMusicSong
		// Register CDirectMusicSong with the Transport.
		pSong->RegisterWithTransport();

		*ppIDocRootNode = pSong;

		hr = S_OK;
	}
	else
	{
		RELEASE( pSong );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType::OnFileSave

HRESULT CSongDocType::OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType::GetListInfo

HRESULT CSongDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo ) 
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

	// Create temporary Song object to retrieve list info
	CDirectMusicSong* pSong = new CDirectMusicSong;
	if( pSong )
	{
		// Create the Undo Manager
		if( pSong->CreateUndoMgr() )
		{
			hr = pSong->ReadListInfoFromStream( pIStream, pListInfo );
		}

		RELEASE( pSong );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType::IsFileTypeExtension

HRESULT CSongDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	strExt.MakeLower();

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( strExt == _T(".snp") )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( strExt == _T(".sng") )
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType::GetObjectDescriptorFromNode

HRESULT CSongDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a Song Node
	CDirectMusicSong* pSong;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_SongNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pSong = (CDirectMusicSong *)pIDocRootNode;

	return pSong->GetObjectDescriptor( pObjectDesc );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType::GetObjectRiffId

HRESULT CSongDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Song Node
	if( IsEqualGUID ( guidNodeId, GUID_SongNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = DMUS_FOURCC_SONG_FORM;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDocType::GetObjectExt

HRESULT CSongDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Song Node
	if( IsEqualGUID ( guidNodeId, GUID_SongNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = ".snp";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = ".sng";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}
