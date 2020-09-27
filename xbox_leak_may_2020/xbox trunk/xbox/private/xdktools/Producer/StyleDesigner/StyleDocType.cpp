// StyleDocType.cpp : implementation file
//

#include "stdafx.h"

#include "StyleDesignerDLL.h"
#include "Style.h"
#include <mmreg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType constructor/destructor

CStyleDocType::CStyleDocType()
{
    m_dwRef = 0;
}

CStyleDocType::~CStyleDocType()
{
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType IUnknown implementation

HRESULT CStyleDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CStyleDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CStyleDocType::Release()
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
// CStyleDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleDocType::GetResourceId

HRESULT CStyleDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_STYLE_DOCTYPE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType::DoesExtensionMatch

HRESULT CStyleDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_STYLE_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
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
// CStyleDocType::DoesIdMatch

HRESULT CStyleDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualGUID(rguid, GUID_StyleNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType::AllocNode

HRESULT STDMETHODCALLTYPE CStyleDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_StyleNode) )
	{
		// Create a new Style 
		CDirectMusicStyle* pStyle = new CDirectMusicStyle;
		if( pStyle == NULL )
		{
			return E_OUTOFMEMORY ;
		}

		// Initialize Style
		HRESULT hr = pStyle->Initialize();
		if( FAILED ( hr ) )
		{
			RELEASE( pStyle );
			return hr;
		}

		*ppINode = (IDMUSProdNode *)pStyle;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType:::OnFileNew

HRESULT CStyleDocType::OnFileNew( IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
								  IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIStyleNode;

	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new Style 
	hr = AllocNode( GUID_StyleNode, &pIStyleNode );
	if( SUCCEEDED ( hr ) )
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( pIStyleNode, pIStyleNode, NULL );

		// Add node to Project Tree
		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AddNode(pIStyleNode, NULL) ) )
		{
			CDirectMusicStyle* pStyle = (CDirectMusicStyle *)pIStyleNode;

			// Store the Style's Project
			pStyle->m_pIProject = pITargetProject;
//			pStyle->m_pIProject->AddRef();			intentionally missing

			// Get the filename
			BSTR bstrName;

			if( SUCCEEDED ( pIStyleNode->GetNodeName( &bstrName ) ) )
			{
				pStyle->m_strOrigFileName = bstrName;
				pStyle->m_strOrigFileName += _T(".stp");
				::SysFreeString( bstrName );
			}

			// Add Style to Style component list 
			theApp.m_pStyleComponent->AddToStyleFileList( pStyle );

			// Advise the ConnectionPoint for this CDirectMusicStyle
			// Register CDirectMusicStyle with the Transport.
			pStyle->RegisterWithTransport();

			*ppIDocRootNode = pIStyleNode;
			pStyle->SetModified( TRUE );

			hr = S_OK;
		}
		else
		{
			RELEASE( pIStyleNode );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType::OnFileOpen

HRESULT CStyleDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* pITargetProject,
								   IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;
	CDirectMusicStyle* pStyle;
	CString strFileName;
	HRESULT hr = E_FAIL;

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new Style 
	pStyle = new CDirectMusicStyle;
	if( pStyle == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	theApp.m_pStyleComponent->m_nNextStyle--;

	if( pStyle->CreateUndoMgr() == FALSE )
	{
		RELEASE( pStyle );
		return E_OUTOFMEMORY;
	}

	// Store the Style's Project
	pStyle->m_pIProject = pITargetProject;
//	pStyle->m_pIProject->AddRef();			intentionally missing

	// Get the filename
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;

		pPersistInfo->GetFileName( &bstrFileName );
		pStyle->m_strOrigFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		int nFindPos = pStyle->m_strOrigFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			pStyle->m_strOrigFileName = pStyle->m_strOrigFileName.Right( pStyle->m_strOrigFileName.GetLength() - nFindPos - 1 );
		}

		pPersistInfo->Release();
	}

	// Load the Style file
	hr = pStyle->Load( pIStream );
	if( !SUCCEEDED ( hr ) )
	{
		RELEASE( pStyle );
		return hr;
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pStyle, pStyle, NULL );

	// Add node to Project Tree
	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AddNode(pStyle, NULL) ) )
	{
		pStyle->SetModified( FALSE );

		// Add Style to Style component list 
		theApp.m_pStyleComponent->AddToStyleFileList( pStyle );

		// Advise the ConnectionPoint for this CDirectMusicStyle
		// Register CDirectMusicStyle with the Transport.
		pStyle->RegisterWithTransport();

		*ppIDocRootNode = pStyle;

		hr = S_OK;
	}
	else
	{
		RELEASE( pStyle );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType::OnFileSave

HRESULT CStyleDocType::OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType::GetListInfo

HRESULT CStyleDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;
	CString strName;
	CString strDescriptor;
	GUID guidStyle;

	ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	memset( &guidStyle, 0, sizeof(GUID) );

	DWORD dwPos = StreamTell( pIStream );

	BOOL fFoundFormat = FALSE;

	// Check for Direct Music format
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_STYLE_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			fFoundFormat = TRUE;

			// Get Style descriptor
			ck.ckid = DMUS_FOURCC_STYLE_CHUNK;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
				DMUS_IO_STYLE dmusStyleIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_STYLE ) );
				if( SUCCEEDED ( pIStream->Read( &dmusStyleIO, dwSize, &dwByteCount ) )
				&&  dwByteCount == dwSize )
				{
					strDescriptor.Format( "%d/%d", dmusStyleIO.timeSig.bBeatsPerMeasure,
										  dmusStyleIO.timeSig.bBeat );
					if( !(dmusStyleIO.timeSig.wGridsPerBeat % 3) )
					{
						strDescriptor += _T("  3");
					}
				}
			}

			// Get Style GUID
			ck.ckid = DMUS_FOURCC_GUID_CHUNK;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
				dwSize = min( ck.cksize, sizeof( GUID ) );
				if( FAILED ( pIStream->Read( &guidStyle, dwSize, &dwByteCount ) )
				||  dwByteCount != dwSize )
				{
					memset( &guidStyle, 0, sizeof(GUID) );
				}
			}

			// Get Style name
			ck.fccType = DMUS_FOURCC_UNFO_LIST;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDLIST ) == 0 )
			{
				DWORD dwPosName = StreamTell( pIStream );
			
				ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
				if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
				{
					ReadMBSfromWCS( pIStream, ck.cksize, &strName );
				}
				else
				{
				    StreamSeek( pIStream, dwPosName, STREAM_SEEK_SET );

					ck.ckid = RIFFINFO_INAM;
					if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
					{
						ReadMBSfromWCS( pIStream, ck.cksize, &strName );
					}
				}
			}
		}

		RELEASE( pIRiffStream );
	}

	// Check for IMA 2.5 format
	if( fFoundFormat == FALSE )
	{
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = FOURCC_STYLE_FORM;

			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				fFoundFormat = TRUE;

				ck.ckid = FOURCC_STYLE;

				if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
				{
					ioStyle iStyle;

					dwSize = min( ck.cksize, sizeof( iStyle ) );

					if( SUCCEEDED ( pIStream->Read( &iStyle, dwSize, &dwByteCount ) )
					&&  dwByteCount == dwSize )
					{
						// Get Style name
						strName = iStyle.wstrName;
						
						// Get Style descriptor
						WORD wGridsPerBeat = iStyle.wClocksPerBeat / iStyle.wClocksPerClick;
						strDescriptor.Format( "%d/%d", iStyle.wBPM, iStyle.wBeat );
						if( !(wGridsPerBeat % 3) )
						{
							strDescriptor += _T("  3");
						}

						// Get Style GUID
						memcpy( &guidStyle, &iStyle.guid, sizeof(GUID) );
					}
				}
			}

			RELEASE( pIRiffStream );
		}
	}

	if( !strName.IsEmpty()
	&&  !strDescriptor.IsEmpty() )
	{
		pListInfo->bstrName = strName.AllocSysString();
		pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
		memcpy( &pListInfo->guidObject, &guidStyle, sizeof(GUID) );

		// Must check pListInfo->wSize before populating additional fields
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType::IsFileTypeExtension

HRESULT CStyleDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	strExt.MakeLower();

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( strExt == _T(".stp") )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( strExt == _T(".sty") )
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType::GetObjectDescriptorFromNode

HRESULT CStyleDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a Style Node
	CDirectMusicStyle* pStyle;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_StyleNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pStyle = (CDirectMusicStyle *)pIDocRootNode;

	return pStyle->GetObjectDescriptor( pObjectDesc );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType::GetObjectRiffId

HRESULT CStyleDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Style Node
	if( IsEqualGUID ( guidNodeId, GUID_StyleNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = DMUS_FOURCC_STYLE_FORM;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDocType::GetObjectExt

HRESULT CStyleDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Style Node
	if( IsEqualGUID ( guidNodeId, GUID_StyleNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = ".stp";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = ".sty";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}
