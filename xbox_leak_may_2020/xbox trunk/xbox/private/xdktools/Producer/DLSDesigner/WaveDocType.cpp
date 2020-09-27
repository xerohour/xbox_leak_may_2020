// WavDocType.cpp : implementation file
//

#include "stdafx.h"

#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "WaveDocType.h"
#include "WaveNode.h"
#include "riffstrm.h"
#include "dls1.h"
#include <mmsystem.h>
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaveDocType constructor/destructor

CWaveDocType::CWaveDocType(CDLSComponent* pComponent)
{
	ASSERT(pComponent != NULL);

    m_dwRef = 0;
	m_pComponent = pComponent;
}

CWaveDocType::~CWaveDocType()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWaveDocType IUnknown implementation

HRESULT CWaveDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CWaveDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CWaveDocType::Release()
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
// CWaveDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CWaveDocType::GetResourceId

HRESULT CWaveDocType::GetResourceId(HINSTANCE* phInstance, UINT* pnResourceId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_WAVE_DOCTYPE;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CWaveDocType::DoesExtensionMatch

HRESULT CWaveDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_WAVE_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
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
// CWaveDocType::DoesIdMatch

HRESULT CWaveDocType::DoesIdMatch(REFGUID rguid)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if(::IsEqualGUID(rguid, GUID_WaveNode))
	{
		return S_OK;
	}

	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWaveDocType::AllocNode

HRESULT STDMETHODCALLTYPE CWaveDocType::AllocNode(REFGUID rguid, IDMUSProdNode** ppINode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pComponent != NULL);

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_WaveNode) == FALSE )
	{
		return E_INVALIDARG;
	}

	CWaveNode* pWaveNode = NULL;

	// Create a new Collection 
	if(FAILED(CWaveNode::CreateWaveNode(m_pComponent, &pWaveNode)))
	{
		return E_FAIL;
	}

	m_pComponent->AddToWaveNodesList(pWaveNode);
	// Register it with the transport
	pWaveNode->RegisterWithTransport();
	*ppINode = (IDMUSProdNode*) pWaveNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveDocType:::OnFileNew

HRESULT CWaveDocType::OnFileNew(IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
							   IDMUSProdNode** ppRootNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	IDMUSProdNode* pWaveNode;

	HRESULT hr = E_FAIL;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	*ppRootNode = NULL;

	// Create a new DLS 
	hr = AllocNode( GUID_WaveNode, &pWaveNode );
	if( SUCCEEDED ( hr ) )
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( pWaveNode, pWaveNode, NULL);
		
		// Insert the DLS node into the Project Tree		
		hr = m_pComponent->m_pIFramework->AddNode(pWaveNode, NULL); 

		if(SUCCEEDED(hr))
		{
			*ppRootNode = pWaveNode;
			return S_OK;
		}

		pWaveNode->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveDocType::OnFileOpen

HRESULT CWaveDocType::OnFileOpen(IStream* pIStream, IDMUSProdProject* pITargetProject,
								IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strFileName;
	HRESULT hr = E_FAIL;
	ASSERT( m_pComponent != NULL );

	CWaveNode* pWaveNode = NULL;
	*ppRootNode = NULL;

	CString sFilePath;
	IDMUSProdPersistInfo* pIJazzPersistInfo;
	if(SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo,(void**) &pIJazzPersistInfo)))
	{
		BSTR bstrFileName;
		pIJazzPersistInfo->GetFileName(&bstrFileName);
		sFilePath = bstrFileName;
		SysFreeString(bstrFileName);
		pIJazzPersistInfo->Release();
	}

	// Create the wave node and load it from pIStream
	if(FAILED(CWaveNode::CreateWaveNode(m_pComponent, pIStream, sFilePath, &pWaveNode, NULL, false)))
	{
		return E_FAIL;
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pWaveNode, pWaveNode, NULL);
		
	// Insert the DLS node into the Project Tree
	hr = m_pComponent->m_pIFramework->AddNode(pWaveNode, NULL);

	if(SUCCEEDED(hr))
	{
		hr  = pWaveNode->InitAndDownload();
		
		// Check if the wave has a name 
		BSTR bstrNodeName;
		pWaveNode->GetName(&bstrNodeName);
		CString sNodeName = bstrNodeName;
		SysFreeString(bstrNodeName);

		IDMUSProdPersistInfo* pIJazzPersistInfo;
		if(SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo,(void**) &pIJazzPersistInfo)))
		{
			BSTR bstrFileName;
			pIJazzPersistInfo->GetFileName(&bstrFileName);
			CString sFilePath = bstrFileName;
			SysFreeString(bstrFileName);
			pWaveNode->SetFileName(sFilePath);
		
			if(sNodeName.IsEmpty())
			{
				CString sFileName = bstrFileName;

				// We just need the filename not the whole path
				int nExtension = sFileName.ReverseFind('.');
				int nLastSlash = sFileName.ReverseFind('\\') + 1;
				
				// Take out the extension
				int nLength = sFileName.GetLength();
				sFileName = sFileName.Left(nExtension);
				
				// Take out the path
				nLength = sFileName.GetLength();
				sFileName = sFileName.Right(nLength - nLastSlash);
				
				// Set the name
				bstrFileName = sFileName.AllocSysString();
				pWaveNode->SetNodeName(bstrFileName);
	
			}

			pIJazzPersistInfo->Release();		
		}
	}



	if(SUCCEEDED(hr))
	{
		*ppRootNode = pWaveNode;
		m_pComponent->AddToWaveNodesList(pWaveNode);
		// Register it with the transport
		pWaveNode->RegisterWithTransport();
		return S_OK;
	}
	
	pWaveNode->Release();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CWaveDocType::IsFileTypeExtension

HRESULT CWaveDocType::IsFileTypeExtension(FileType ftFileType, BSTR bstrExt)    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString(bstrExt);

	strExt.MakeLower();

	switch(ftFileType)
	{
		case FT_DESIGN:
			if(strExt == _T(".wvp"))
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if(strExt == _T(".wav"))
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWaveDocType IDMUSProdDocType::OnFileSave

HRESULT CWaveDocType::OnFileSave(IStream* pIStream, IDMUSProdNode* pIDocRootNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_FAIL;

	CWaveNode* pWaveNode = (CWaveNode*) pIDocRootNode;
	if(pWaveNode && pIStream)
	{
		HRESULT hr = pWaveNode->SaveAs(pIStream);
		return hr;
	}

	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
// CWaveDocType IDMUSProdDocType::GetListInfo

HRESULT CWaveDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;
	CString csName;
	GUID guidWave;

	ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	memset( &guidWave, 0, sizeof(GUID) );

	// Check for 'WAVE' RIFF header
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			DWORD dwPos = StreamTell( pIStream );

			// Get Wave GUID
			ck.ckid = DMUS_FOURCC_GUID_CHUNK;
			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
				dwSize = min( ck.cksize, sizeof( GUID ) );
				if( FAILED ( pIStream->Read( &guidWave, dwSize, &dwByteCount ) )
				||  dwByteCount != dwSize )
				{
					memset( &guidWave, 0, sizeof(GUID) );
				}
			}

		    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

			// Get Wave name
			ck.fccType = mmioFOURCC('I','N','F','O') ;
			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDLIST ) == 0 )
			{
				ck.ckid = mmioFOURCC('I','N','A','M') ;
				if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
				{
					void* pString = new char[ck.cksize];
					if( pString )
					{
						memset( pString, 0, ck.cksize );
	
						if( FAILED ( pIStream->Read( pString, ck.cksize, &dwByteCount ) )
						||  dwByteCount != ck.cksize )
						{
							memset( pString, 0, ck.cksize );
						}
						csName = (char *)pString;
						delete pString;
					}
				}
			}
		}

		RELEASE( pIRiffStream );
	}

	if( csName.IsEmpty() )
	{
		// Use filename minus extension
		IDMUSProdPersistInfo* pIPersistInfo;
		if(SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo,(void**) &pIPersistInfo)))
		{
			TCHAR achFName[_MAX_FNAME];
			BSTR bstrFileName;
		
			pIPersistInfo->GetFileName(&bstrFileName);
			CString sFileName = bstrFileName;
			::SysFreeString(bstrFileName);
			_tsplitpath( sFileName, NULL, NULL, achFName, NULL );
			csName = achFName;
			pIPersistInfo->Release();
		}
	}

	if( !csName.IsEmpty() )
	{
		pListInfo->bstrName = csName.AllocSysString();
		memcpy( &pListInfo->guidObject, &guidWave, sizeof(GUID) );

		// Must check pListInfo->wSize before populating additional fields
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveDocType::GetObjectDescriptorFromNode

HRESULT CWaveDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a Wave Node
	CWaveNode* pWave;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_WaveNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pWave = (CWaveNode *)pIDocRootNode;

	return pWave->GetObjectDescriptor( pObjectDesc );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveDocType::GetObjectRiffId

HRESULT CWaveDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Wave Node
	if( IsEqualGUID ( guidNodeId, GUID_WaveNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = mmioFOURCC('W', 'A', 'V', 'E');
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveDocType::GetObjectExt

HRESULT CWaveDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Wave Node
	if( IsEqualGUID ( guidNodeId, GUID_WaveNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = ".wvp";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = ".wav";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveDocType additional functions