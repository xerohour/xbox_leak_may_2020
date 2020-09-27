// BandDocType.cpp : implementation file
//

#include "stdafx.h"

#include "BandEditorDLL.h"
#include "Band.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBandDocType constructor/destructor

CBandDocType::CBandDocType()
{
    ASSERT( 1 );
}

CBandDocType::CBandDocType( CBandComponent* pComponent )
{
	ASSERT( pComponent != NULL );

    m_dwRef = 0;

	m_pComponent = pComponent;
//  m_pComponent->AddRef() intentionally missing
}

CBandDocType::~CBandDocType()
{
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType IUnknown implementation

HRESULT CBandDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CBandDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CBandDocType::Release()
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
// CBandDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CBandDocType::GetResourceId

HRESULT CBandDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_BAND_DOCTYPE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType::DoesExtensionMatch

HRESULT CBandDocType::DoesExtensionMatch( BSTR bstrExt )
{
#ifndef DMP_XBOX
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_BAND_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
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
#endif

	return S_FALSE;
}
/*HRESULT CBandDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt;
	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	strExt = bstrExt;
	::SysFreeString( bstrExt );

	if( ::LoadString(theApp.m_hInstance, IDR_BAND_DOCTYPE, achBuffer, MID_BUFFER) )
	{
		if( AfxExtractSubString(strDocTypeExt, achBuffer, CDocTemplate::filterExt) )
		{
			strExt.MakeUpper();
			strDocTypeExt.MakeUpper();

			if( _tcscmp( strDocTypeExt, strExt ) == 0 )
			{
				return S_OK;
			}
			else
			{
				return S_FALSE;
			}
		}
	}

	return E_FAIL;
}*/


/////////////////////////////////////////////////////////////////////////////
// CBandDocType::DoesIdMatch

HRESULT CBandDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualGUID(rguid, GUID_BandNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType::AllocNode

HRESULT STDMETHODCALLTYPE CBandDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_BandNode) == FALSE )
	{
		return E_INVALIDARG ;
	}

	// Create a new Band
	CBand* pBand = new CBand( m_pComponent );
	if( pBand == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppINode = (IDMUSProdNode *)pBand;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType::OnFileNew

HRESULT CBandDocType::OnFileNew( IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
								 IDMUSProdNode** ppIDocRootNode )
{
#ifndef DMP_XBOX
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIBandNode;

	HRESULT hr = E_FAIL;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new Band 
	hr = AllocNode( GUID_BandNode, &pIBandNode );
	if( SUCCEEDED ( hr ) )
	{
		CBand* pBand = (CBand *)pIBandNode;

		// Set the Band's name
		TCHAR achName[SMALL_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_BAND_TEXT, achName, SMALL_BUFFER );
		pBand->m_csName.Format( "%s%d", achName, ++m_pComponent->m_nNextBand );

		// Set root and parent node of ALL children
		theApp.SetNodePointers( pIBandNode, pIBandNode, NULL );

		// Add node to Project Tree
		hr = m_pComponent->m_pIFramework->AddNode(pIBandNode, NULL);
		if( SUCCEEDED( hr ) )
		{
			m_pComponent->AddToBandFileList( (CBand *)pIBandNode );
			*ppIDocRootNode = pIBandNode;
			((CBand *)pIBandNode)->m_fDirty = TRUE;
			hr = S_OK;
		}
		else
		{
			pIBandNode->Release();
		}
	}

	return hr;
#else
	return E_NOTIMPL;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType::OnFileOpen

HRESULT CBandDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* pITargetProject,
								  IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode )
{
#ifndef DMP_XBOX
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pINode;
	HRESULT hr = E_FAIL;

	ASSERT( pIStream != NULL );
	ASSERT( m_pComponent != NULL );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( SUCCEEDED ( m_pComponent->LoadRIFFChunk( pIStream, &pINode ) ) )
	{
		// Insert a node into the Project Tree
		theApp.SetNodePointers( pINode, pINode, NULL );

		// Add node to Project Tree
		hr = m_pComponent->m_pIFramework->AddNode(pINode, NULL);
		if( SUCCEEDED( hr ) )
		{
			m_pComponent->AddToBandFileList( (CBand *)pINode );
			*ppIDocRootNode = pINode;
		}
		else {
			pINode->Release();
		}
	}
	return hr;
#else
	return E_NOTIMPL;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType::IsFileTypeExtension

HRESULT CBandDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
#ifndef DMP_XBOX
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	strExt.MakeLower();

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( strExt == _T(".bnp") )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( strExt == _T(".bnd") )
			{
				return S_OK;
			}
			break;
	}
#endif

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType IDMUSProdDocType::OnFileSave

HRESULT CBandDocType::OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType IDMUSProdDocType::GetListInfo

HRESULT CBandDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo )
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
	CBand* pBand = new CBand( m_pComponent );
	if( pBand )
	{
		hr = pBand->ReadListInfoFromStream( pIStream, pListInfo );

		RELEASE( pBand );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType::GetObjectDescriptorFromNode

HRESULT CBandDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a Band Node
	CBand* pBand;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_BandNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pBand = (CBand *)pIDocRootNode;

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
	
	memcpy( &pDMObjectDesc->guidObject, &pBand->m_guidBand, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicBand, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = pBand->m_Version.dwVersionMS;
	pDMObjectDesc->vVersion.dwVersionLS = pBand->m_Version.dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, pBand->m_csName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType::GetObjectRiffId

HRESULT CBandDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Band Node
	if( IsEqualGUID ( guidNodeId, GUID_BandNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = DMUS_FOURCC_BAND_FORM;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CBandDocType::GetObjectExt

HRESULT CBandDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
#ifndef DMP_XBOX
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Band Node
	if( IsEqualGUID ( guidNodeId, GUID_BandNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = ".bnp";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = ".bnd";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}
#endif

	return E_INVALIDARG;
}
