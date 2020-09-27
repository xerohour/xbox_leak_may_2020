// PersonalityDocType.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "PersonalityDesigner.h"
#include "Personality.h"
#include <mmsystem.h>
#include <mmreg.h>
#pragma warning(default:4201)

#include "PersonalityRIFF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType constructor/destructor

CPersonalityDocType::CPersonalityDocType()
{
}

CPersonalityDocType::CPersonalityDocType( CPersonalityComponent* pComponent )
{
	ASSERT( pComponent != NULL );

    m_dwRef = 0;

	m_pComponent = pComponent;
//  m_pComponent->AddRef() intentionally missing
}

CPersonalityDocType::~CPersonalityDocType()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType IUnknown implementation

HRESULT CPersonalityDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdDocType8)
    ||  ::IsEqualIID(riid, IID_IDMUSProdDocType)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdDocType8 *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CPersonalityDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CPersonalityDocType::Release()
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
// CPersonalityDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::GetResourceId

HRESULT CPersonalityDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_PERSONALITY_DOCTYPE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::DoesExtensionMatch

HRESULT CPersonalityDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_PERSONALITY_DOCTYPE, achBuffer, MID_BUFFER) == 0 )
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
// CPersonalityDocType::DoesIdMatch

HRESULT CPersonalityDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualGUID(rguid, GUID_PersonalityNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::AllocNode

HRESULT STDMETHODCALLTYPE CPersonalityDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	HRESULT hr;

	*ppINode = NULL;

    if( ::IsEqualGUID(rguid, GUID_PersonalityNode) == FALSE )
	{
		return E_INVALIDARG ;
	}

	// Create a new Personality
	CPersonality* pIPersonality = new CPersonality( m_pComponent );
	if( pIPersonality == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	hr = pIPersonality->Initialize();

	*ppINode = (IDMUSProdNode *)pIPersonality;

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::OnFileNew

HRESULT CPersonalityDocType::OnFileNew( IDMUSProdProject* /*pITargetProject*/, IDMUSProdNode* /*pITargetDirectoryNode*/,
										IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	IDMUSProdNode* pIPersonalityNode=NULL;
	HRESULT hr = E_FAIL;

	if( ppIDocRootNode == NULL )
	{
		hr = E_POINTER; goto Leave;
	}

	*ppIDocRootNode = NULL;

	// Create a new Personality 
	if( FAILED(hr = AllocNode( GUID_PersonalityNode, &pIPersonalityNode )) )
	{
		goto Leave;
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pIPersonalityNode, pIPersonalityNode, NULL );

	// Add node to Project Tree
	if( FAILED(hr = m_pComponent->m_pIFramework->AddNode(pIPersonalityNode, NULL)) )
	{
		goto Leave;
	}

	// Initialize the ChordMapMgr and ChordMapStrip
	((CPersonality*)pIPersonalityNode)->SendChordMapAndPaletteToChordMapMgr();

	// Advise the ConnectionPoint for this CSection and register the
	// CSection in the Transport.
	((CPersonality*)pIPersonalityNode)->AdviseConnectionPoint();

	m_pComponent->AddToPersonalityFileList( (CPersonality *)pIPersonalityNode );
	*ppIDocRootNode = pIPersonalityNode;

//	((CPersonality*)pIPersonalityNode)->Modified() = TRUE;
	hr = S_OK;

Leave:
	if( FAILED( hr ) )
	{
		RELEASE( pIPersonalityNode );
	}
	else if(pIPersonalityNode)
	{
		((CPersonality*)pIPersonalityNode)->SyncPersonalityWithEngine(CPersonality::syncPersonality);
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::OnFileOpen

HRESULT CPersonalityDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* /*pITargetProject*/,
										 IDMUSProdNode* /*pITargetDirectoryNode*/, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPersonality* pPersonality;
	HRESULT hr;

	ASSERT( m_pComponent != NULL );
	
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new Personality 
	pPersonality = new CPersonality( m_pComponent );
	if( pPersonality == NULL )
	{
		return E_OUTOFMEMORY ;
	}
	m_pComponent->m_nNextPersonality--;

	// Get the filename
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;

		pPersistInfo->GetFileName( &bstrFileName );
		pPersonality->m_strOrigFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		int nFindPos = pPersonality->m_strOrigFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			pPersonality->m_strOrigFileName 
				= pPersonality->m_strOrigFileName.Right( pPersonality->m_strOrigFileName.GetLength() - nFindPos - 1 );
		}

		pPersistInfo->Release();
	}


	// Load the Personality file
	CWaitCursor	wcCursor;
	hr = pPersonality->Load( pIStream );
	if( FAILED( hr ) )
	{
		pPersonality->Release();
		return hr;
	}

	// Insert a node into the Project Tree
	if( !SUCCEEDED ( hr = m_pComponent->m_pIFramework->AddNode(pPersonality, NULL) ) )
	{
		pPersonality->Release();
		return hr;
	}

	// Advise the ConnectionPoint for this CSection and register the
	// CSection in the Transport.
	pPersonality->AdviseConnectionPoint();

	// Add Personality to Personality component list 
	m_pComponent->AddToPersonalityFileList( (CPersonality *)pPersonality );

	*ppIDocRootNode = (IDMUSProdNode *)pPersonality;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::OnFileSave

HRESULT CPersonalityDocType::OnFileSave( IStream* /*pIStream*/, IDMUSProdNode* /*pIDocRootNode*/ )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::IsFileTypeExtension

HRESULT CPersonalityDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( !(strExt.CompareNoCase(_T(".cdp"))) )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( !(strExt.CompareNoCase(_T(".cdm"))) )
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::GetListInfo

HRESULT CPersonalityDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	ULONG ulSize;
	ULONG ulID;
    HRESULT hr;

    ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

    hr = DMGetListInfo(pIStream, pListInfo);

	if(hr == S_OK || FAILED(hr))
	{
		goto LEAVE;
	}

	ulID = ReadID( pIStream );
    ulSize = ReadSize( pIStream );

	if( (ulID == ID_EPERSONALITY)	// .cdp file
	||  (ulID == ID_PERSONALITY) )	// .cdm file
    {
		Personality personality;

		long lRecordSize = ReadSize( pIStream );
		DWORD dwSize = min( lRecordSize, sizeof( personality ) );
		DWORD dwByteCount;

		hr = pIStream->Read( &personality, dwSize, &dwByteCount );
		if( FAILED( hr )
		||  dwByteCount != dwSize )
		{
			hr = E_FAIL;
		}
		else
		{
			CString strName;
			CString strDescriptor;

			personality.m_pfirstchord = NULL;
			personality.m_playlist.RemoveAll();		
			personality.m_chordlist.RemoveAll();
			personality.m_signpostlist.RemoveAll();
			
			strName = personality.m_username;
			pListInfo->bstrName = strName.AllocSysString();

			m_pComponent->ScalePatternToScaleName( (personality.m_scalepattern & 0x00FFFFFF), strDescriptor );
			pListInfo->bstrDescriptor = strDescriptor.AllocSysString();

			// Must check pListInfo->wSize before populating additional fields
			hr = S_OK;
		}
	}
LEAVE:
	return hr;
}

HRESULT CPersonalityDocType::DMGetListInfo(IStream * pIStream, DMUSProdListInfo * pListInfo)
{
	DWORD dwPos = StreamTell(pIStream);	// save pos if we fail
	IDMUSProdRIFFStream* pIRiffStream;
	MMCKINFO ckMain;

	HRESULT hr = S_OK;

	if(SUCCEEDED(AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		ckMain.fccType = FOURCC_PERSONALITY;
		if(pIRiffStream->Descend(&ckMain, NULL, MMIO_FINDRIFF)==0)
		{
			MMCKINFO	ck;
			DWORD		dwByteCount;
			DWORD		dwSize;
			CString		str;
			DMUS_IO_CHORDMAP	iPersonality;
			ioPersonalityEdit	iPersonalityEdit;

			while(pIRiffStream->Descend(&ck, &ckMain, 0) == 0)
			{
				switch(ck.ckid)
				{
				case FOURCC_IOPERSONALITY:
					dwSize = min(ck.cksize, sizeof(iPersonality));
					hr = pIStream->Read(&iPersonality, dwSize, &dwByteCount);
					if(FAILED(hr) || dwByteCount != dwSize)
					{
						hr = E_FAIL;
						goto Leave;
					}
					str = iPersonality.wszLoadName;
					pListInfo->bstrName = str.AllocSysString();

					if( iPersonality.dwFlags & PERH_INVALID_BITS )
					{
						// There was a time (pre-DX8) when this field was not initialized
						 iPersonality.dwFlags = 0;
					}
					if( iPersonality.dwFlags & DMUS_CHORDMAPF_VERSION8 )
					{
						// Root is stored in high MSByte
						m_pComponent->ScalePatternToScaleName( (iPersonality.dwScalePattern & 0x00FFFFFF), str);
					}
					else
					{
						DWORD dwKey;
						iPersonality.dwScalePattern &= 0x00FFFFFF;
						dwKey = m_pComponent->DetermineRoot( iPersonality.dwScalePattern );
						iPersonality.dwScalePattern = Rotate24( iPersonality.dwScalePattern, -(long)dwKey );
						m_pComponent->ScalePatternToScaleName( (iPersonality.dwScalePattern & 0x00FFFFFF), str);
					}
					pListInfo->bstrDescriptor = str.AllocSysString();
					break;
				case FOURCC_PERSONALITYEDIT:
					dwSize = min(ck.cksize, sizeof(iPersonalityEdit));
					hr = pIStream->Read(&iPersonalityEdit, dwSize, &dwByteCount);
					if(FAILED(hr) || dwByteCount != dwSize)
					{
						hr = E_FAIL;
						goto Leave;
					}
					// Use username when present
					str = iPersonalityEdit.wchUserName;
					pListInfo->bstrName = str.AllocSysString();
					break;
				case FOURCC_GUID:
					hr = pIStream->Read(&(pListInfo->guidObject), sizeof(GUID), &dwByteCount);
					if(FAILED(hr) || dwByteCount != ck.cksize)
					{
						hr = E_FAIL;
						goto Leave;
					}
					break;
				}
				pIRiffStream->Ascend(&ck, 0);
			}
		}
Leave:
		pIRiffStream->Release();
	}
	StreamSeek(pIStream, dwPos, STREAM_SEEK_SET);
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::GetObjectDescriptorFromNode

HRESULT CPersonalityDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Make sure method was passed a Personality Node
	CPersonality* pPersonality;
	GUID guidNodeId;
	if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
	{
		if( !( IsEqualGUID ( guidNodeId, GUID_PersonalityNode ) ) )
		{
			return E_INVALIDARG;
		}
	}
	pPersonality = (CPersonality *)pIDocRootNode;

	return pPersonality->GetObjectDescriptor( pObjectDesc );
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::GetObjectRiffId

HRESULT CPersonalityDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pckid == NULL
	||  pfccType == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Personality Node
	if( IsEqualGUID ( guidNodeId, GUID_PersonalityNode ) ) 
	{
		*pckid = FOURCC_RIFF;
		*pfccType = FOURCC_PERSONALITY;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDocType::GetObjectExt

HRESULT CPersonalityDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrExt == NULL )
	{
		return E_POINTER;
	}

	// Make sure method was passed a Personality Node
	if( IsEqualGUID ( guidNodeId, GUID_PersonalityNode ) ) 
	{
		if( ftFileType == FT_DESIGN )
		{
			CString strExt = ".cdp";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = ".cdm";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}
