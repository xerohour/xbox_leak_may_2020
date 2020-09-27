// SegmentDocType.cpp : implementation file
//

#include "stdafx.h"

#include "SegmentDesignerDLL.h"
#include "SegmentDesigner.h"
#include "Segment.h"
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
#include <WaveTimelineDraw.h>
#include <DLSDesigner.h>
#include <dsoundp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType constructor/destructor

CSegmentDocType::CSegmentDocType()
{
}

CSegmentDocType::CSegmentDocType( CSegmentComponent* pComponent )
{
	ASSERT( pComponent != NULL );

    m_dwRef = 0;

	m_pComponent = pComponent;
//  m_pComponent->AddRef() intentionally missing
}

CSegmentDocType::~CSegmentDocType()
{
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType IUnknown implementation

HRESULT CSegmentDocType::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

    if( ::IsEqualIID(riid, IID_IDMUSProdDocTypeP) )
    {
        AddRef();
        *ppvObj = (IDMUSProdDocTypeP *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_ISegmentDocTypeWaveVarImport) )
    {
        AddRef();
        *ppvObj = (ISegmentDocTypeWaveVarImport *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CSegmentDocType::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CSegmentDocType::Release()
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
// CSegmentDocType IDMUSProdDocType implementation

/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::GetResourceId

HRESULT CSegmentDocType::GetResourceId( HINSTANCE* phInstance, UINT* pnResourceId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*phInstance   = theApp.m_hInstance;
	*pnResourceId = IDR_SEGMENT_DOCTYPE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::DoesExtensionMatch

HRESULT CSegmentDocType::DoesExtensionMatch( BSTR bstrExt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	CString strDocTypeExt;
	TCHAR   achBuffer[MAX_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDR_SEGMENT_DOCTYPE, achBuffer, MAX_BUFFER-1) == 0 )
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
// CSegmentDocType::DoesIdMatch

HRESULT CSegmentDocType::DoesIdMatch( REFGUID rguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    if( ::IsEqualGUID(rguid, GUID_SegmentNode) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::AllocNode

HRESULT STDMETHODCALLTYPE CSegmentDocType::AllocNode( REFGUID rguid, IDMUSProdNode** ppINode )
{
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

	*ppINode = (IDMUSProdNode *)pSegment;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::OnFileNew

HRESULT CSegmentDocType::OnFileNew( IDMUSProdProject* pITargetProject, IDMUSProdNode* pITargetDirectoryNode,
									 IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UNREFERENCED_PARAMETER(pITargetProject);
	UNREFERENCED_PARAMETER(pITargetDirectoryNode);

	IDMUSProdNode* pISegmentNode;

	HRESULT hr = E_FAIL;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Create a new Segment 
	hr = AllocNode( GUID_SegmentNode, &pISegmentNode );
	if( SUCCEEDED ( hr ) )
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( pISegmentNode, pISegmentNode, NULL );

		// Add node to Project Tree
		hr = m_pComponent->m_pIFramework->AddNode(pISegmentNode, NULL);
		if(SUCCEEDED(hr))
		{
			CSegment* pSegment = (CSegment *)pISegmentNode;

			// Get the filename
			BSTR bstrName;

			if( SUCCEEDED ( pISegmentNode->GetNodeName( &bstrName ) ) )
			{
				pSegment->m_strOrigFileName = bstrName;
				pSegment->m_strOrigFileName += _T(".sgp");
				::SysFreeString( bstrName );
			}

			// Add Segment to Segment component list 
			m_pComponent->AddToSegmentFileList( pSegment );

			// Register CSegment with the Transport.
			pSegment->RegisterWithTransport();

			// Fix 32224: Add a TimeSig track
			CTrack *pTrack = new CTrack;
			if( pTrack )
			{
				memcpy( &pTrack->m_guidClassID, &CLSID_DirectMusicTimeSigTrack, sizeof( GUID ) );
				// Let the Segment fill m_guidEditorID in.
				//memcpy( &pTrack->m_guidEditorID, &CLSID_TimeSigMgr, sizeof( GUID ) );
				//pTrack->dwPosition = 0;
				pTrack->m_dwGroupBits = 0x00000001;
				//pTrack->punkStripMgr = NULL;
				//pTrack->m_ckid = 0;
				pTrack->m_fccType = DMUS_FOURCC_TIMESIGNATURE_TRACK;

				BYTE bArray[] = {
					'L', 'I', 'S', 'T',
					0x18, 0, 0, 0,
					'T', 'I', 'M', 'S',
					't', 'i', 'm', 's',
					0x0c, 0, 0, 0,
					0x08, 0, 0, 0,
					0, 0, 0, 0,
					0x04, 0x04, 0x04, 0 };
				
				IStream *pStream = NULL;
				if( SUCCEEDED( CreateStreamOnHGlobal( NULL, TRUE, &pStream ) ) )
				{
					if( SUCCEEDED( pStream->Write( bArray, sizeof( bArray), NULL ) ) )
					{
						pTrack->SetStream( pStream );
					}
					pStream->Release();
				}

				if( FAILED( pSegment->AddTrack( pTrack ) ) )
				{
					delete pTrack;
				}
			}

			pSegment->m_fBrandNew = true;
			*ppIDocRootNode = pISegmentNode;
			return S_OK;
		}
		// We failed, so delete the segment.
		pISegmentNode->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::OnFileOpen

HRESULT CSegmentDocType::OnFileOpen( IStream* pIStream, IDMUSProdProject* pITargetProject,
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

	// Try and load the segment
	hr = m_pComponent->LoadSegment(pIStream, &pINode);
	if(SUCCEEDED(hr))
	{
		// Set root and parent node of ALL children
		theApp.SetNodePointers( pINode, pINode, NULL );

		if(hr == S_FALSE)
		{
			return S_OK;
		}

		// Add node to Project Tree
		hr = m_pComponent->m_pIFramework->AddNode(pINode, NULL);
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
// CSegmentDocType IDMUSProdNode::OnFileSave

HRESULT CSegmentDocType::OnFileSave( IStream* pIStream, IDMUSProdNode* pIDocRootNode )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDocRootNode);
	UNREFERENCED_PARAMETER(pIStream);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::IsFileTypeExtension

HRESULT CSegmentDocType::IsFileTypeExtension( FileType ftFileType, BSTR bstrExt )    
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	strExt.MakeLower();

	switch( ftFileType )
	{
		case FT_DESIGN:
			if( strExt == _T(".sgp") )
			{
				return S_OK;
			}
			break;

		case FT_RUNTIME:
			if( strExt == _T(".sgt") )
			{
				return S_OK;
			}
			break;
	}

	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::GetListInfo

HRESULT CSegmentDocType::GetListInfo( IStream* pIStream, DMUSProdListInfo* pListInfo )
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
// CSegmentDocType::GetObjectDescriptorFromNode

HRESULT CSegmentDocType::GetObjectDescriptorFromNode( IDMUSProdNode* pIDocRootNode, void* pObjectDesc )
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
// CSegmentDocType::GetObjectRiffId

HRESULT CSegmentDocType::GetObjectRiffId( GUID guidNodeId, DWORD* pckid, DWORD* pfccType )
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
// CSegmentDocType::GetObjectExt

HRESULT CSegmentDocType::GetObjectExt( GUID guidNodeId, FileType ftFileType, BSTR* pbstrExt )
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
			CString strExt = ".sgp";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}

		if( ftFileType == FT_RUNTIME )
		{
			CString strExt = ".sgt";
			*pbstrExt = strExt.AllocSysString();
			return S_OK;
		}
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType IDMUSProdDocTypeP implementation

/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::ImportNode

HRESULT CSegmentDocType::ImportNode( IUnknown* punkNode, IUnknown* punkTreePositionNode, IUnknown** ppIDocRootNode )
{
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}
	*ppIDocRootNode = NULL;

	if( punkNode == NULL 
	||  punkTreePositionNode == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	IDMUSProdNode* pINode;
	if( SUCCEEDED ( punkNode->QueryInterface( IID_IDMUSProdNode, (void**)&pINode ) ) )
	{
		IDMUSProdNode* pITreePositionNode;
		if( SUCCEEDED ( punkTreePositionNode->QueryInterface( IID_IDMUSProdNode, (void**)&pITreePositionNode ) ) )
		{
			GUID guidNodeId;
			if( SUCCEEDED ( pINode->GetNodeId ( &guidNodeId ) ) )
			{
				// Handle wave nodes
				if( IsEqualGUID ( guidNodeId, GUID_WaveNode ) )
				{
					// Make sure we have a Wave "file"
					IDMUSProdNode* pIWaveDocRootNode;
					if( SUCCEEDED ( pINode->GetDocRootNode ( &pIWaveDocRootNode ) ) )
					{
						if( pIWaveDocRootNode == pINode )
						{
							// Create a new Segment from the Wave "file"
							hr = CreateSegmentFromWave( pIWaveDocRootNode, pITreePositionNode, ppIDocRootNode );
						}

						RELEASE( pIWaveDocRootNode );
					}
				}
			}

			RELEASE( pITreePositionNode );
		}

		RELEASE( pINode );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType ISegmentDocTypeWaveVarImport implementation

/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::ImportWaveNodes

HRESULT CSegmentDocType::ImportWaveNodes( DWORD dwNodeCount, IDMUSProdNode** apunkNode, IUnknown* punkTreePositionNode, IUnknown** ppIDocRootNode )
{
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}
	*ppIDocRootNode = NULL;

	if( apunkNode == NULL 
	||  punkTreePositionNode == NULL
	||	dwNodeCount == 0 )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	IDMUSProdNode* pITreePositionNode;
	if( SUCCEEDED ( punkTreePositionNode->QueryInterface( IID_IDMUSProdNode, (void**)&pITreePositionNode ) ) )
	{
		// Create a new Segment from the Wave "file"
		hr = CreateSegmentFromWaves( dwNodeCount, apunkNode, pITreePositionNode, ppIDocRootNode );

		RELEASE( pITreePositionNode );
	}

	return hr;
}



/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType Additional methods

/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::CreateSegmentFromWave

HRESULT CSegmentDocType::CreateSegmentFromWave( IDMUSProdNode* pIWaveNode, IDMUSProdNode* pITreePositionNode,
											    IUnknown** ppISegmentNode )
{
	if( ppISegmentNode == NULL )
	{
		return E_POINTER;
	}
	*ppISegmentNode = NULL;

	if( pIWaveNode == NULL 
	||  pITreePositionNode == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// This DocType can only create Segment nodes
	IDMUSProdNode* pISegmentNode = NULL;

	IDirectSoundWave* pIDirectSoundWave;

	// Get a DirectSound wave object
	HRESULT hr = pIWaveNode->GetObject( CLSID_DirectSoundWave, IID_IDirectSoundWave, (void**)&pIDirectSoundWave );
	if( SUCCEEDED ( hr ) )
	{
		// Create a segment
		hr = AllocNode( GUID_SegmentNode, &pISegmentNode );
		if( FAILED ( hr ) )
		{
			pISegmentNode = NULL;
		}

		if( pISegmentNode )
		{
			CSegment* pSegment = (CSegment *)pISegmentNode;

			// Set root and parent node of ALL children
			theApp.SetNodePointers( pISegmentNode, pISegmentNode, NULL );

			// Update the Segment name
			BSTR bstrName;
			if( SUCCEEDED ( pIWaveNode->GetNodeName( &bstrName ) ) )
			{
				pSegment->SetNodeName( bstrName );
			}

			// Place the wave in the Segment
			IDMUSProdSegmentEdit* pISegmentEdit;
			hr = pSegment->QueryInterface( IID_IDMUSProdSegmentEdit, (void**)&pISegmentEdit );
			if( SUCCEEDED ( hr ) )
			{
				// Create a Wave track in the Segment
				IUnknown* punkStripMgr;
				hr = pISegmentEdit->AddStrip( CLSID_DirectMusicWaveTrack, 1, &punkStripMgr );
				if( SUCCEEDED ( hr ) )
				{
					IDMUSProdStripMgr* pIStripMgr;
					hr = punkStripMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pIStripMgr );
					if( SUCCEEDED ( hr ) )
					{
						// Init DMUSPROD_WAVE_PARAM structure
						DMUSPROD_WAVE_PARAM wp;
						wp.rtTimePhysical = 0;
						wp.dwPChannel = 0;
						wp.dwIndex = 0;
						wp.pIWaveNode = pIWaveNode;

						// Place wave in the wave track 
						hr = pIStripMgr->SetParam( GUID_WaveParam, 0, &wp );
						if( SUCCEEDED ( hr ) )
						{
							// Set the length of the segment to the length of the wave
							REFERENCE_TIME rtTime = GetWaveLength( pIWaveNode );
							if( rtTime > 0 )
							{
								pSegment->m_dwSegmentFlags |= DMUS_SEGIOF_REFLENGTH;
								pSegment->m_rtLength = rtTime;
							}
							else
							{
								hr = E_FAIL;
							}

							// Set the wave track's flags
							if( SUCCEEDED ( hr ) )
							{
								VARIANT varTrackHeader;
								DMUS_IO_TRACK_EXTRAS_HEADER ioTrackExtrasHeader;
								ZeroMemory( &ioTrackExtrasHeader, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );
								varTrackHeader.vt = VT_BYREF;
								V_BYREF(&varTrackHeader) = &ioTrackExtrasHeader;
								hr = pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, &varTrackHeader );
								if( SUCCEEDED( hr ) ) 
								{
									ioTrackExtrasHeader.dwFlags |= (DMUS_TRACKCONFIG_DEFAULT | DMUS_TRACKCONFIG_PLAY_CLOCKTIME);

									varTrackHeader.vt = VT_BYREF;
									V_BYREF(&varTrackHeader) = &ioTrackExtrasHeader;
									hr = pIStripMgr->SetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, varTrackHeader );
								}
							}

							// Make sure Segment knows about the new wave track flags
							if( SUCCEEDED ( hr ) )
							{
								IUnknown* punk;
								hr = pIStripMgr->QueryInterface( IID_IUnknown, (void **)&punk );
								if( SUCCEEDED ( hr ) )
								{
									pSegment->OnDataChanged( punk );

									RELEASE( punk );
								}
							}
						}

						RELEASE( pIStripMgr );
					}

					RELEASE( punkStripMgr );
				}

				RELEASE( pISegmentEdit );
			}
		}

		RELEASE( pIDirectSoundWave );
	}

	if( SUCCEEDED ( hr ) )
	{
		// Place the newly created segment file in the Project Tree
		IDMUSProdNode* pINewSegmentNode;
		hr = m_pComponent->m_pIFramework->CopyFile( pISegmentNode, pITreePositionNode, &pINewSegmentNode );
		if( SUCCEEDED ( hr ) )
		{
			*ppISegmentNode = pINewSegmentNode;
		}
	}

	if( pISegmentNode )
	{
		pISegmentNode->DeleteNode( FALSE );
		RELEASE( pISegmentNode );
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::CreateSegmentFromWaves

HRESULT CSegmentDocType::CreateSegmentFromWaves( DWORD dwCount, IDMUSProdNode** apIWaveNode, IDMUSProdNode* pITreePositionNode,
											    IUnknown** ppISegmentNode )
{
	if( ppISegmentNode == NULL )
	{
		return E_POINTER;
	}
	*ppISegmentNode = NULL;

	if( apIWaveNode == NULL 
	||	dwCount == 0
	||  pITreePositionNode == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// This DocType can only create Segment nodes
	IDMUSProdNode* pISegmentNode = NULL;

	// Create a segment
	HRESULT hr = AllocNode( GUID_SegmentNode, &pISegmentNode );
	if( FAILED ( hr ) )
	{
		pISegmentNode = NULL;
	}

	if( pISegmentNode )
	{
		CSegment* pSegment = (CSegment *)pISegmentNode;

		// Set root and parent node of ALL children
		theApp.SetNodePointers( pISegmentNode, pISegmentNode, NULL );

		// Place the wave in the Segment
		IDMUSProdSegmentEdit* pISegmentEdit;
		hr = pSegment->QueryInterface( IID_IDMUSProdSegmentEdit, (void**)&pISegmentEdit );
		if( SUCCEEDED ( hr ) )
		{
			// Create a Wave track in the Segment
			IUnknown* punkStripMgr;
			hr = pISegmentEdit->AddStrip( CLSID_DirectMusicWaveTrack, 1, &punkStripMgr );
			if( SUCCEEDED ( hr ) )
			{
				IDMUSProdStripMgr* pIStripMgr;
				hr = punkStripMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pIStripMgr );
				if( SUCCEEDED ( hr ) )
				{
					REFERENCE_TIME rtLength = 0;
					DWORD dwVariation = 1;
					DWORD dwPChannel = 0;
					bool fSetSegmentName = false;

					for( DWORD dwIndex = 0; dwIndex <  dwCount; dwIndex++ )
					{
						// Get a DirectSound wave object
						IDirectSoundWave* pIDirectSoundWave;
						hr = apIWaveNode[dwIndex]->GetObject( CLSID_DirectSoundWave, IID_IDirectSoundWave, (void**)&pIDirectSoundWave );
						if( SUCCEEDED ( hr ) )
						{
							// Init DMUSPROD_WAVE_PARAM2 structure
							DMUSPROD_WAVE_PARAM2 wp;
							wp.rtTimePhysical = 0;
							wp.dwPChannel = dwPChannel;
							wp.dwIndex = 0;
							wp.pIWaveNode = apIWaveNode[dwIndex];
							wp.dwVariation = dwVariation;

							// Update the Segment name
							if( !fSetSegmentName )
							{
								BSTR bstrName;
								if( SUCCEEDED ( apIWaveNode[dwIndex]->GetNodeName( &bstrName ) ) )
								{
									pSegment->SetNodeName( bstrName );
									fSetSegmentName = true;
								}
							}

							// Place wave in the wave track 
							hr = pIStripMgr->SetParam( GUID_WaveParam2, 0, &wp );
							if( SUCCEEDED ( hr ) )
							{
								// Set the length of the segment to the length of the wave
								REFERENCE_TIME rtTime = GetWaveLength( apIWaveNode[dwIndex] );
								if( rtTime > rtLength )
								{
									rtLength = rtTime;
								}

								// Go to the next variation
								dwVariation = dwVariation * 2;

								// If we've filled all 32 variations
								if( 0 == dwVariation)
								{
									// Go back to the first variation
									dwVariation = 1;

									// Go to the next PChannel
									dwPChannel++;
								}
							}

							// Release the DirectSound wave
							pIDirectSoundWave->Release();
						}
					}

					// If we inserted at least one wave
					if( rtLength > 0 )
					{
						pSegment->m_dwSegmentFlags |= DMUS_SEGIOF_REFLENGTH;
						pSegment->m_rtLength = rtLength;

						// Set the wave track's flags
						VARIANT varTrackHeader;
						DMUS_IO_TRACK_EXTRAS_HEADER ioTrackExtrasHeader;
						ZeroMemory( &ioTrackExtrasHeader, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );
						varTrackHeader.vt = VT_BYREF;
						V_BYREF(&varTrackHeader) = &ioTrackExtrasHeader;
						hr = pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, &varTrackHeader );
						if( SUCCEEDED( hr ) ) 
						{
							ioTrackExtrasHeader.dwFlags |= (DMUS_TRACKCONFIG_DEFAULT | DMUS_TRACKCONFIG_PLAY_CLOCKTIME);

							varTrackHeader.vt = VT_BYREF;
							V_BYREF(&varTrackHeader) = &ioTrackExtrasHeader;
							hr = pIStripMgr->SetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, varTrackHeader );
						}

						// Make sure Segment knows about the new wave track flags
						if( SUCCEEDED ( hr ) )
						{
							IUnknown* punk;
							hr = pIStripMgr->QueryInterface( IID_IUnknown, (void **)&punk );
							if( SUCCEEDED ( hr ) )
							{
								pSegment->OnDataChanged( punk );

								RELEASE( punk );
							}
						}
					}

					RELEASE( pIStripMgr );
				}

				RELEASE( punkStripMgr );
			}

			RELEASE( pISegmentEdit );
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		// Place the newly created segment file in the Project Tree
		IDMUSProdNode* pINewSegmentNode;
		hr = m_pComponent->m_pIFramework->CopyFile( pISegmentNode, pITreePositionNode, &pINewSegmentNode );
		if( SUCCEEDED ( hr ) )
		{
			*ppISegmentNode = pINewSegmentNode;
		}
	}

	if( pISegmentNode )
	{
		pISegmentNode->DeleteNode( FALSE );
		RELEASE( pISegmentNode );
	}
	return hr;
}



/////////////////////////////////////////////////////////////////////////////
// CSegmentDocType::GetWaveLength

REFERENCE_TIME CSegmentDocType::GetWaveLength( IDMUSProdNode* pIWaveNode )
{
	REFERENCE_TIME rtWaveLength = 0;

	// Prepare WaveInfoParams struct
	WaveInfoParams	wip;
	memset( &wip, 0, sizeof(WaveInfoParams) );
	wip.cbSize = sizeof(WaveInfoParams);

	// Ask DLS Designer for info about the wave
	if( pIWaveNode)
	{
		IDMUSProdWaveTimelineDraw* pIWaveTimelineDraw;
		if( SUCCEEDED ( pIWaveNode->QueryInterface( IID_IDMUSProdWaveTimelineDraw, (void **)&pIWaveTimelineDraw ) ) )
		{
			if( SUCCEEDED ( pIWaveTimelineDraw->GetWaveInfo( &wip ) ) )
			{
				if( FAILED ( pIWaveTimelineDraw->SampleToRefTime( wip.dwWaveDuration, &rtWaveLength, 0 ) ) )
				{
					rtWaveLength = 0;
				}
			}

			RELEASE( pIWaveTimelineDraw );
		}
	}

	return rtWaveLength;
}
