// DLSDocType.cpp : implementation file
//

#include "stdafx.h"

#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "DLSDocType.h"
#include "Collection.h"
#include "riffstrm.h"
#include "dls1.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLSDocType constructor/destructor

CDLSDocType::CDLSDocType()
{
    ASSERT(1);
}

CDLSDocType::CDLSDocType(CDLSComponent* pComponent)
{
	ASSERT(pComponent != NULL);

    m_dwRef = 0;

	m_pComponent = pComponent;
//	m_pComponent->AddRef() intentionally missing
}

CDLSDocType::~CDLSDocType()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDLSDocType IUnknown implementation

HRESULT CDLSDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CDLSDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CDLSDocType::Release()
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
// CDLSDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CDLSDocType::GetResourceId

HRESULT CDLSDocType::GetResourceId(HINSTANCE* phInstance, UINT* pnResourceId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_DLS_DOCTYPE;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSDocType::DoesExtensionMatch

HRESULT CDLSDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_DLS_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
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
// CDLSDocType::DoesIdMatch

HRESULT CDLSDocType::DoesIdMatch(REFGUID rguid)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if(::IsEqualGUID(rguid, GUID_CollectionNode))
	{
		return S_OK;
	}

	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSDocType::AllocNode

HRESULT STDMETHODCALLTYPE CDLSDocType::AllocNode(REFGUID rguid, IDMUSProdNode** ppINode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pComponent != NULL);

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_CollectionNode) == FALSE )
	{
		return E_INVALIDARG;
	}

	// Create a new Collection 
	CCollection* pCollection = new CCollection( m_pComponent );
	if( pCollection == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppINode = (IDMUSProdNode *)pCollection;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSDocType:::OnFileNew

HRESULT CDLSDocType::OnFileNew(IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
							   IDMUSProdNode** ppRootNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	IDMUSProdNode* pIDLSNode;

	HRESULT hr = E_FAIL;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	*ppRootNode = NULL;

	// Create a new DLS 
	hr = AllocNode( GUID_CollectionNode, &pIDLSNode );
	if( SUCCEEDED ( hr ) )
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( pIDLSNode, pIDLSNode, NULL);
		
		// Insert the DLS node into the Project Tree		
		hr = m_pComponent->m_pIFramework->AddNode(pIDLSNode, NULL); 

		if(SUCCEEDED(hr))
		{
			// This call addrefs the collection as well...
			m_pComponent->AddToCollectionList((CCollection *)pIDLSNode);

			*ppRootNode = pIDLSNode;

			// Set flag so we know to save file 
            ((CCollection *)pIDLSNode)->SetDirtyFlag();

			return S_OK;
		}

		pIDLSNode->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSDocType::OnFileOpen

HRESULT CDLSDocType::OnFileOpen(IStream* pIStream, IDMUSProdProject* pITargetProject,
								IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CCollection* pCollection;
	CString strFileName;
	HRESULT hr;

	ASSERT( m_pComponent != NULL );

	*ppRootNode = NULL;

	// Create a new DLS 
	pCollection = new CCollection( m_pComponent );
	if( pCollection == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	m_pComponent->m_nNextDLS--;

	// Keep the filename with the node
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

	pCollection->SetNodeFileName(sFilePath);

	// Load the DLS file
	
	{ // Used to scope CWaitCusor
		CWaitCursor();
	
		hr = pCollection->Load( pIStream );

		if( !SUCCEEDED ( hr ) )
		{
			pCollection->Release();
			return hr;
		}
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pCollection, pCollection, NULL);
		
	// Insert the DLS node into the Project Tree
	hr = m_pComponent->m_pIFramework->AddNode(pCollection, NULL);

	if(SUCCEEDED(hr))
	{
		// This method addrefs the collection as well...
		m_pComponent->AddToCollectionList(pCollection);
		
		*ppRootNode = pCollection;
		pCollection->ValidateInstrumentPatches();
		return S_OK;
	}
	
	pCollection->Release();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSDocType::IsFileTypeExtension

HRESULT CDLSDocType::IsFileTypeExtension(FileType ftFileType, BSTR bstrExt)    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString(bstrExt);

	strExt.MakeLower();

	switch(ftFileType)
	{
		case FT_DESIGN:
			if(strExt == _T(".dlp"))
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if(strExt == _T(".dls"))
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSDocType IDMUSProdDocType::OnFileSave

HRESULT CDLSDocType::OnFileSave(IStream* pIStream, IDMUSProdNode* pIDocRootNode)
{
	// If this method returns E_NOTIMPL the IDMUSProdFramework does the right thing
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSDocType IDMUSProdDocType::GetListInfo

HRESULT CDLSDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdRIFFStream* pIRiffStream = NULL;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwByteCount;
	void*	pString;
	CString csName;
	HRESULT hr = S_OK;

    ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	if( SUCCEEDED ( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = FOURCC_DLS;
		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			ck.fccType = mmioFOURCC('I','N','F','O') ;
			if ( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDLIST) == 0 )
			{
				ck.ckid = mmioFOURCC('I','N','A','M');
				if ( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK) == 0 )
				{
					pString = new char[ck.cksize];
					if (pString)
					{
						pIStream->Read(pString, ck.cksize, &dwByteCount);
						csName = (char*) pString;
						delete pString;
						pListInfo->bstrName = csName.AllocSysString();
						hr = S_OK;
					}
					else
					{
						//"new" failed
						hr = E_OUTOFMEMORY;
					}
				}
				else
				{
					hr = E_FAIL;
					// name chunk not in info list
				}
			}
			else
			{
				hr = E_FAIL;
				//Info list not found
			}
		}
		else
		{
			hr = E_FAIL;
			// DLS RIFF not found
		}
	}
	else
	{
		hr = E_FAIL;
		// AllocRiffStream failed
	}

	if( pIRiffStream )
	{
		pIRiffStream->Release();
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSDocType::GetObjectDescriptorFromNode

HRESULT CDLSDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a Collection Node
	CCollection* pCollection;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_CollectionNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pCollection = (CCollection *)pIDocRootNode;

	return pCollection->GetObjectDescriptor( pObjectDesc );
}


/////////////////////////////////////////////////////////////////////////////
// CDLSDocType::GetObjectRiffId

HRESULT CDLSDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Collection Node
	if( IsEqualGUID ( guidNodeId, GUID_CollectionNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = FOURCC_DLS;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CDLSDocType::GetObjectExt

HRESULT CDLSDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Collection Node
	if( IsEqualGUID ( guidNodeId, GUID_CollectionNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = ".dlp";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = ".dls";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}

/////////////////////////////////////////////////////////////////////////////
// CDLSDocType additional functions