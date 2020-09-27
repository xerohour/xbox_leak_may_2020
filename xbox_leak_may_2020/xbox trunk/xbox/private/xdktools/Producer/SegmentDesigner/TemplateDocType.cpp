// TemplateDocType.cpp : implementation file
//

#include "stdafx.h"

#include "SegmentDesignerDLL.h"
#include "SegmentDesigner.h"
#include "Segment.h"
#include "TemplateDocType.h"
#include "resource.h"
#include "SegmentComponent.h"
#include "Track.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include <RiffStrm.h>
#include <mmreg.h>
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType constructor/destructor

CTemplateDocType::CTemplateDocType()
{
}

CTemplateDocType::CTemplateDocType( CSegmentComponent* pComponent )
{
	ASSERT( pComponent != NULL );

    m_dwRef = 0;

	m_pComponent = pComponent;
//  m_pComponent->AddRef() intentionally missing
}

CTemplateDocType::~CTemplateDocType()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType IUnknown implementation

HRESULT CTemplateDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

    if( ::IsEqualIID(riid, IID_IDMUSProdNode) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CTemplateDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CTemplateDocType::Release()
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
// CTemplateDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType::GetResourceId

HRESULT CTemplateDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_TEMPLATE_DOCTYPE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType::DoesExtensionMatch

HRESULT CTemplateDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_TEMPLATE_DOCTYPE, achBuffer, MAX_BUFFER-1) == 0 )
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
// CTemplateDocType::DoesIdMatch

HRESULT CTemplateDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    if( ::IsEqualGUID(rguid, GUID_SegmentNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType::AllocNode

HRESULT STDMETHODCALLTYPE CTemplateDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rguid);
	UNREFERENCED_PARAMETER(ppINode);
	return E_NOTIMPL;
	/*
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr;

	ASSERT( m_pComponent != NULL );

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_SegmentNode) == FALSE )
	{
		return E_INVALIDARG ;
	}

	// Create a new Segment
	CSegment* pSegment = new CSegment( m_pComponent );
	if( pSegment == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	hr = pSegment->Initialize();
	if(FAILED(hr))
	{
		return hr;
	}

	CTrack *pTrack = new CTrack;
	pTrack->m_ckid = DMUS_FOURCC_SIGNPOST_TRACK_CHUNK;
	pTrack->m_dwGroupBits = 1;
	pTrack->m_dwPosition = 0;
	pTrack->m_fccType = 0;
	memcpy( &pTrack->m_guidClassID, &CLSID_DirectMusicSignPostTrack, sizeof(GUID) );
	ZeroMemory( &pTrack->m_guidEditorID, sizeof(GUID) );
	if( FAILED( pSegment->AddTrack( pTrack ) ) )
	{
		TRACE("CTemplateDocType: Unable to add SignPost track to segment!\n");
		ASSERT(FALSE);
		delete pTrack;
	}

	*ppINode = (IDMUSProdNode *)pSegment;

	return S_OK;
	*/
}


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType::OnFileNew

HRESULT CTemplateDocType::OnFileNew( IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
									 IDMUSProdNode** ppIDocRootNode )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pITargetProject);
	UNREFERENCED_PARAMETER(pITargetDirectoryNode);
	UNREFERENCED_PARAMETER(ppIDocRootNode);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType::OnFileOpen

HRESULT CTemplateDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* pITargetProject,
									  IDMUSProdNode* pITargetDirectoryNode, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UNREFERENCED_PARAMETER(pITargetProject);
	UNREFERENCED_PARAMETER(pITargetDirectoryNode);

	IDMUSProdNode* pINode;
	HRESULT hr;

	ASSERT(pIStream != NULL);
	ASSERT(m_pComponent != NULL);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	hr = m_pComponent->LoadSegment(pIStream, &pINode);
	if(SUCCEEDED(hr))
	{
		if(hr == S_FALSE)
		{
			return S_OK;
		}
		hr = m_pComponent->m_pIFramework->AddNode(pINode, NULL);
		// Add node to Project Tree
		if(SUCCEEDED(hr))
		{
			CSegment* pSegment = (CSegment *)pINode;

			// Add Segment to Segment component list 
			m_pComponent->AddToSegmentFileList( pSegment );

			// Register CSegment with the Transport.
			pSegment->RegisterWithTransport();

			*ppIDocRootNode = pINode;
			return S_OK;
		}
		// We failed, so delete the segment.
		pINode->Release();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType IDMUSProdNode::OnFileSave

HRESULT CTemplateDocType::OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIStream);
	UNREFERENCED_PARAMETER(pIDocRootNode);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType::IsFileTypeExtension

HRESULT CTemplateDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	strExt.MakeLower();

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( strExt == _T(".tpp") )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( strExt == _T(".tpl") )
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType::GetListInfo

HRESULT CTemplateDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;
	CString strName;
	CString strDescriptor;
	GUID guidSegment;

	ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	memset( &guidSegment, 0, sizeof(GUID) );

	// Check for Direct Music format
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_SEGMENT_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{

			// Get Segment GUID
			ck.ckid = DMUS_FOURCC_GUID_CHUNK;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
				dwSize = min( ck.cksize, sizeof( GUID ) );
				if( FAILED ( pIStream->Read( &guidSegment, dwSize, &dwByteCount ) )
				||  dwByteCount != dwSize )
				{
					memset( &guidSegment, 0, sizeof(GUID) );
				}
			}

			// Get Segment name
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

	if( !strName.IsEmpty() )
	{
		pListInfo->bstrName = strName.AllocSysString();
		pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
		memcpy( &pListInfo->guidObject, &guidSegment, sizeof(GUID) );

		// Must check pListInfo->wSize before populating additional fields
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType::GetObjectDescriptorFromNode

HRESULT CTemplateDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a Segment Node
	CSegment* pSegment;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_SegmentNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pSegment = (CSegment *)pIDocRootNode;

	return pSegment->GetObjectDescriptor( pObjectDesc );
}


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType::GetObjectRiffId

HRESULT CTemplateDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Segment Node
	if( IsEqualGUID ( guidNodeId, GUID_SegmentNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = DMUS_FOURCC_SEGMENT_FORM;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocType::GetObjectExt

HRESULT CTemplateDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Segment Node
	if( IsEqualGUID ( guidNodeId, GUID_SegmentNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = _T(".tpp");
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = _T(".tpl");
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}


