// Track.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include "Track.h"
#include "SegmentDesigner.h"
#include "StripMgr.h"		
#include <TrackFlagsPPG.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//	CTrack Constructor/Destructor

CTrack::CTrack( CDirectMusicSong* pSong )
{
    m_dwRef = 0;
	AddRef();

	ASSERT( pSong != NULL );
	m_pSong = pSong;
//	m_pSong->AddRef();			intentionally missing

	m_pSourceSegment = NULL;

	// UI
	m_TrackUI.dwTrackIndex = 0;
	m_dwBitsUI = 0;

	//		DMUS_IO_TRACK_HEADER
	memset( &m_clsidTrack, 0, sizeof(CLSID) );
    m_dwPosition = 0;
    m_dwGroup = 0;
    m_ckid = 0;
    m_fccType = 0;

	//		DMUS_IO_TRACK_EXTRAS_HEADER
    m_dwFlagsDM = 0;
    m_dwPriority = 0;
}

CTrack::CTrack( CDirectMusicSong* pSong, CSourceSegment* pSourceSegment, DWORD dwTrackIndex,
				DMUS_IO_TRACK_HEADER* pdmusTrackHeader, DMUS_IO_TRACK_EXTRAS_HEADER* pdmusTrackExtrasHeader )
{
    m_dwRef = 0;
	AddRef();

	ASSERT( pSong != NULL );
	m_pSong = pSong;
//	m_pSong->AddRef();			intentionally missing

	ASSERT( pSourceSegment != NULL );
	m_pSourceSegment = pSourceSegment;
	m_pSourceSegment->AddRef();

	// UI
	m_TrackUI.dwTrackIndex = dwTrackIndex;
	m_dwBitsUI = 0;

	//		DMUS_IO_TRACK_HEADER
	ASSERT( pdmusTrackHeader != NULL );
	m_clsidTrack = pdmusTrackHeader->guidClassID;
	m_dwPosition = pdmusTrackHeader->dwPosition;
	m_dwGroup = pdmusTrackHeader->dwGroup;
	m_ckid = pdmusTrackHeader->ckid;
	m_fccType = pdmusTrackHeader->fccType;

	//		DMUS_IO_TRACK_EXTRAS_HEADER
	ASSERT( pdmusTrackExtrasHeader != NULL );
	m_dwFlagsDM = pdmusTrackExtrasHeader->dwFlags;
	m_dwPriority = pdmusTrackExtrasHeader->dwPriority;
}

CTrack::~CTrack( void )
{
	RELEASE( m_pSourceSegment );
}


/////////////////////////////////////////////////////////////////////////////
// CTrack::Copy

void CTrack::Copy( CTrack* pTrack )
{
	ASSERT( pTrack != NULL );
	if ( pTrack == NULL )
	{
		return;
	}

	if( pTrack == this )
	{
		return;
	}

	ASSERT( pTrack->m_pSourceSegment != NULL );
	m_pSourceSegment = pTrack->m_pSourceSegment;
	m_pSourceSegment->AddRef();

	// UI
	m_TrackUI = pTrack->m_TrackUI;
	m_dwBitsUI = pTrack->m_dwBitsUI;

	//		DMUS_IO_TRACK_HEADER
	m_clsidTrack = pTrack->m_clsidTrack;
	m_dwPosition = pTrack->m_dwPosition;
	m_dwGroup = pTrack->m_dwGroup;
	m_ckid = pTrack->m_ckid;
	m_fccType = pTrack->m_fccType;

	//		DMUS_IO_TRACK_EXTRAS_HEADER
	m_dwFlagsDM = pTrack->m_dwFlagsDM;
	m_dwPriority = pTrack->m_dwPriority;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack IUnknown implementation

HRESULT CTrack::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IUnknown *)this;
    }
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CTrack::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CTrack::Release()
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
// CTrack Additional Methods

/////////////////////////////////////////////////////////////////////////////
// CTrack Load

HRESULT CTrack::Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*       pIStream;
	MMCKINFO	   ck;
	DWORD		   dwByteCount;
	DWORD		   dwSize;
    HRESULT        hr = S_OK;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_TRACKREF_CHUNK:
			{
				DMUS_IO_TRACKREF_HEADER dmusTrackRefIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_TRACKREF_HEADER ) );
				hr = pIStream->Read( &dmusTrackRefIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_pSourceSegment = m_pSong->IndexToSourceSegment( dmusTrackRefIO.dwSegmentID );
				if( m_pSourceSegment )
				{
					m_pSourceSegment->AddRef();
				}
				break;
			}

			case DMUS_FOURCC_TRACK_CHUNK:
			{
			    DMUS_IO_TRACK_HEADER dmusTrackIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_TRACK_HEADER ) );
				hr = pIStream->Read( &dmusTrackIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_clsidTrack = dmusTrackIO.guidClassID;
				m_dwPosition = dmusTrackIO.dwPosition;
				m_dwGroup = dmusTrackIO.dwGroup;
				m_ckid = dmusTrackIO.ckid;
				m_fccType = dmusTrackIO.fccType;
				break;
			}

			case DMUS_FOURCC_TRACK_EXTRAS_CHUNK:
			{
			    DMUS_IO_TRACK_EXTRAS_HEADER dmusTrackExtrasIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );
				hr = pIStream->Read( &dmusTrackExtrasIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwFlagsDM = dmusTrackExtrasIO.dwFlags;
				m_dwPriority = dmusTrackExtrasIO.dwPriority;
				break;
			}

			case DMUS_FOURCC_TRACK_UI_CHUNK:
			{
			    ioTrackUI iTrackUI;

				dwSize = min( ck.cksize, sizeof( ioTrackUI ) );
				hr = pIStream->Read( &iTrackUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_TrackUI.dwTrackIndex = iTrackUI.dwTrackIndex;
				break;
			}

			case DMUS_FOURCC_TRACK_COPY_PASTE_UI_CHUNK:
			{
			    ioTrackCopyPasteUI iTrackCopyPasteUI;

				dwSize = min( ck.cksize, sizeof( ioTrackCopyPasteUI ) );
				hr = pIStream->Read( &iTrackCopyPasteUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// This chunk only exists during drag/drop and copy/paste operations
				// It is used to re-establish correct links
				// Do not dereference these pointers because objects may have been deleted

				// m_pSourceSegment
				RELEASE( m_pSourceSegment );
				m_pSourceSegment = m_pSong->PtrToSourceSegment( iTrackCopyPasteUI.pSourceSegment );
				if( m_pSourceSegment )
				{
					m_pSourceSegment->AddRef();
				}
				break;
			}
        }

        pIRiffStream->Ascend( &ck, 0 );
    }

	if( m_pSourceSegment == NULL )
	{
		// Cannot use this track
		hr = S_FALSE;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack Save

HRESULT CTrack::Save( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ckMain;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pPersistInfo );
	}
	
	static bool sfSequenceSaveLock = false;

	if( !sfSequenceSaveLock )
	{
		sfSequenceSaveLock = true;

		ASSERT( 0 == m_dwPosition );

		if( m_pSourceSegment
		&&	(CLSID_DirectMusicSeqTrack == m_clsidTrack)
		&&	((StreamInfo.ftFileType != FT_RUNTIME) || (GUID_DirectMusicObject == StreamInfo.guidDataFormat)) )
		{
			IDMUSProdNode* pISegmentNode;
			if( SUCCEEDED ( m_pSourceSegment->GetSegmentDocRootNode( &pISegmentNode ) ) )
			{
				// Get an IDMUSProdSegmentEdit8 interface pointer
				IDMUSProdSegmentEdit8* pISegmentEdit8;
				if( SUCCEEDED ( pISegmentNode->QueryInterface( IID_IDMUSProdSegmentEdit8, (void **)&pISegmentEdit8 ) ) )
				{
					DMUS_IO_TRACK_HEADER dmusTrackHeader;
					DMUS_IO_TRACK_EXTRAS_HEADER dmusTrackExtrasHeader;
					HRESULT hr = S_OK;

					DWORD dwSequenceIndex = 0;

					// Enumerate Tracks
					for( DWORD dwIdx = 0 ;  (hr == S_OK) ;  dwIdx++ )
					{
						hr = pISegmentEdit8->EnumTrack( dwIdx,
							 							sizeof(DMUS_IO_TRACK_HEADER), &dmusTrackHeader,
														sizeof(DMUS_IO_TRACK_EXTRAS_HEADER), &dmusTrackExtrasHeader );
						if( hr == S_OK )
						{
							// If this is a sequence track
							if( (m_dwGroup == dmusTrackHeader.dwGroup)
							&&	(CLSID_DirectMusicSeqTrack == dmusTrackHeader.guidClassID) )
							{
								if( dwSequenceIndex > 0 )
								{
									// Add the track to our list to display
									CTrack* pTrack = new CTrack( m_pSong );
									pTrack->Copy( this );
									pTrack->m_dwPosition = dwSequenceIndex;

									pTrack->Save( pIRiffStream );

									delete pTrack;
								}
								dwSequenceIndex++;
							}
						}
					}

					RELEASE( pISegmentEdit8 );
				}

				RELEASE( pISegmentNode );
			}
		}

		sfSequenceSaveLock = false;
	}

	// Write DMUS_FOURCC_TRACKREF_LIST header
	ckMain.fccType = DMUS_FOURCC_TRACKREF_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save Track header chunk
	hr = SaveTrackHeader( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save <trkh> Track header chunk
	hr = SaveSegmentTrackHeader( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save <trkx> Extras Track header chunk
	hr = SaveSegmentTrackExtrasHeader( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save UI chunk
	if( StreamInfo.ftFileType != FT_RUNTIME )
	{
		hr = SaveUI( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

	// Save Copy/Paste UI chunk
	if( StreamInfo.ftFileType != FT_RUNTIME )
	{
		if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_SaveSelectedVirtualSegments ) 
		||  ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_SaveSelectedTracks ) )
		{
			hr = SaveCopyPasteUI( pIRiffStream );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack SaveTrackHeader
    
HRESULT CTrack::SaveTrackHeader( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_TRACKREF_HEADER dmusTrackRefIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write chunk header
	ck.ckid = DMUS_FOURCC_TRACKREF_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_TRACKREF_HEADER structure
	memset( &dmusTrackRefIO, 0, sizeof(DMUS_IO_TRACKREF_HEADER) );

	dmusTrackRefIO.dwSegmentID = m_pSong->SourceSegmentToIndex( m_pSourceSegment );
	dmusTrackRefIO.dwFlags = 0;			// Not used

	// Write TrackRef header data
	hr = pIStream->Write( &dmusTrackRefIO, sizeof(DMUS_IO_TRACKREF_HEADER), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_TRACKREF_HEADER) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack SaveSegmentTrackHeader
    
HRESULT CTrack::SaveSegmentTrackHeader( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_TRACK_HEADER dmusTrackIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write chunk header
	ck.ckid = DMUS_FOURCC_TRACK_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_TRACK_HEADER structure
	memset( &dmusTrackIO, 0, sizeof(DMUS_IO_TRACK_HEADER) );

    dmusTrackIO.guidClassID = m_clsidTrack;
    dmusTrackIO.dwPosition = m_dwPosition;
    dmusTrackIO.dwGroup = m_dwGroup;
    dmusTrackIO.ckid = m_ckid;
    dmusTrackIO.fccType = m_fccType;

	// Write Track header data
	hr = pIStream->Write( &dmusTrackIO, sizeof(DMUS_IO_TRACK_HEADER), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_TRACK_HEADER) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack SaveSegmentTrackExtrasHeader
    
HRESULT CTrack::SaveSegmentTrackExtrasHeader( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_TRACK_EXTRAS_HEADER dmusTrackExtrasIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write chunk header
	ck.ckid = DMUS_FOURCC_TRACK_EXTRAS_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_TRACK_EXTRAS_HEADER structure
	memset( &dmusTrackExtrasIO, 0, sizeof(DMUS_IO_TRACK_EXTRAS_HEADER) );

    dmusTrackExtrasIO.dwFlags = m_dwFlagsDM;
    dmusTrackExtrasIO.dwPriority = m_dwPriority;

	// Write TrackExtras header data
	hr = pIStream->Write( &dmusTrackExtrasIO, sizeof(DMUS_IO_TRACK_EXTRAS_HEADER), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_TRACK_EXTRAS_HEADER) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack SaveCopyPasteUI
    
HRESULT CTrack::SaveCopyPasteUI( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	ioTrackCopyPasteUI oTrackCopyPasteUI;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write chunk header
	ck.ckid = DMUS_FOURCC_TRACK_COPY_PASTE_UI_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioTrackCopyPasteUI structure
	memset( &oTrackCopyPasteUI, 0, sizeof(ioTrackCopyPasteUI) );

	oTrackCopyPasteUI.pSourceSegment = m_pSourceSegment;

	// Write ioTrackCopyPasteUI structure
	hr = pIStream->Write( &oTrackCopyPasteUI, sizeof(ioTrackCopyPasteUI), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioTrackCopyPasteUI) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack SaveUI
    
HRESULT CTrack::SaveUI( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write chunk header
	ck.ckid = DMUS_FOURCC_TRACK_UI_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write UI chunk data
	hr = pIStream->Write( &m_TrackUI, sizeof(ioTrackUI), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioTrackUI) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack FormatTextUI

void CTrack::FormatTextUI( CString& strText )
{
	// Get Segment filename
	CString strFileName;
	FormatFileNameText( strFileName );
	strFileName += _T(" ");

	// Get track group
	CString strTrackGroup;
	FormatTrackGroupText( strTrackGroup );
	strTrackGroup += _T(": ");

	// Get track name
	CString strTrackName;
	theApp.m_pSongComponent->GetRegisteredTrackName( m_clsidTrack, strTrackName );

	strText = strFileName + strTrackGroup + strTrackName;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack::FormatFileNameText

void CTrack::FormatFileNameText( CString& strText )
{
	strText.Empty();

	IDMUSProdNode* pISegmentNode;
	if( SUCCEEDED ( m_pSourceSegment->GetSegmentDocRootNode( &pISegmentNode ) ) )
	{
		BSTR bstrText;
		if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->GetNodeFileName( pISegmentNode, &bstrText ) ) )
		{
			strText = bstrText;
			::SysFreeString( bstrText );
			int nFindPos = strText.ReverseFind( (TCHAR)'\\' );
			if( nFindPos != -1 )
			{
				strText = strText.Right( strText.GetLength() - nFindPos - 1 );
			}
			strText = _T("[") + strText + _T("] ");
		}

		RELEASE( pISegmentNode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrack::FormatTrackGroupText

void CTrack::FormatTrackGroupText( CString& strText )
{
	CString strTmp;
	BOOL fFoundGroup = FALSE;
	BOOL fLastSet = FALSE;
	int nStartGroup = -1;

	strText.Empty();

	for( int i = 0 ;  i < 32 ;  i++ )
	{
		if( m_dwGroup & (1 << i) )
		{
			if( !fLastSet )
			{
				fLastSet = TRUE;
				nStartGroup = i;
			}
		}
		else
		{
			if( fLastSet )
			{
				fLastSet = FALSE;
				if( nStartGroup == i - 1 )
				{
					if( fFoundGroup )
					{
						strTmp.Format(", %d", i);
					}
					else
					{
						strTmp.Format("%d", i);
						fFoundGroup = TRUE;
					}
				}
				else
				{
					if( fFoundGroup )
					{
						strTmp.Format(", %d-%d", nStartGroup + 1, i);
					}
					else
					{
						strTmp.Format("%d-%d", nStartGroup + 1, i);
						fFoundGroup = TRUE;
					}
				}
				strText += strTmp;
			}
		}
	}

	if( fLastSet )
	{
		fLastSet = FALSE;
		if( nStartGroup == i - 1 )
		{
			if( fFoundGroup )
			{
				strTmp.Format(", %d", i);
			}
			else
			{
				strTmp.Format("%d", i);
				fFoundGroup = TRUE;
			}
		}
		else
		{
			if( fFoundGroup )
			{
				strTmp.Format(", %d-%d", nStartGroup + 1, i);
			}
			else
			{
				strTmp.Format("%d-%d", nStartGroup + 1, i);
				fFoundGroup = TRUE;
			}
		}
		strText += strTmp;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrack::GetExtraFlags

DWORD CTrack::GetExtraFlags( void )
{
	return m_dwFlagsDM;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack::SetExtraFlags

void CTrack::SetExtraFlags( DWORD dwNewFlagsDM )
{
	m_dwFlagsDM = dwNewFlagsDM;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack::GetExtraMask

DWORD CTrack::GetExtraMask( void )
{
	DWORD dwExtraMask = ALLEXTRAS_FLAGS;

	GUID guidStripMgr;
	if( SUCCEEDED( GUIDToStripMgr( m_clsidTrack, &guidStripMgr ) ) )
	{
		IDMUSProdStripMgr* pIStripMgr;
		if( SUCCEEDED ( CoCreateInstance( guidStripMgr, NULL, CLSCTX_INPROC, IID_IDMUSProdStripMgr, (void**)&pIStripMgr ) ) )
		{
			VARIANT var;
			var.vt = VT_I4;

			if( SUCCEEDED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER_MASK, &var ) ) )
			{
				dwExtraMask  = V_I4(&var);
			}

			RELEASE( pIStripMgr );
		}
	}

	return dwExtraMask;
}


/////////////////////////////////////////////////////////////////////////////
// CTrack::GUIDToStripMgr

HRESULT CTrack::GUIDToStripMgr( REFGUID guidCLSID, GUID* pguidStripMgr )
{
	if ( pguidStripMgr == NULL )
	{
		return E_INVALIDARG;
	}

	LONG	lResult;

	TCHAR    szGuid[MID_BUFFER];
	LPOLESTR psz;
	
	if( FAILED( StringFromIID(guidCLSID, &psz) ) )
	{
		TRACE("Song: Unable to convert GUID to string.\n");
		goto ON_ERR;
	}

	WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
	CoTaskMemFree( psz );

	TCHAR	szRegPath[MAX_BUFFER];
	HKEY	hKeyTrack;

	_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\StripEditors\\") );
	_tcscat( szRegPath, szGuid );
	lResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, szRegPath, 0, KEY_READ, &hKeyTrack );
	if ( lResult != ERROR_SUCCESS )
	{
		TRACE("Song: Unable to find track GUID.\n");
		goto ON_ERR;
	}

	_TCHAR	achClsId[MID_BUFFER];
	DWORD	dwType, dwCbData;
	dwCbData = MID_BUFFER;
	lResult  = ::RegQueryValueEx( hKeyTrack, _T("StripManager"), NULL,
				&dwType, (LPBYTE)achClsId, &dwCbData );
	::RegCloseKey( hKeyTrack );
	if( (lResult != ERROR_SUCCESS) || (dwType != REG_SZ) )
	{
		TRACE("Song: Unable to find Strip Manager CLSID in Track's key.\n");
		goto ON_ERR;
	}

	wchar_t awchClsId[80];

	if( MultiByteToWideChar( CP_ACP, 0, achClsId, -1, awchClsId, sizeof(awchClsId) / sizeof(wchar_t) ) != 0 )
	{
		IIDFromString( awchClsId, pguidStripMgr );
		return S_OK;
	}

ON_ERR:
	TRACE("Song: Using UnknownStripMgr as default\n");
	memcpy( pguidStripMgr, &CLSID_UnknownStripMgr, sizeof(GUID) );
	return S_OK;
}
