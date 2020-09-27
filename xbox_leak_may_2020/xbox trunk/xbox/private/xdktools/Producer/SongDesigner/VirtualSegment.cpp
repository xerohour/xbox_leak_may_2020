// VirtualSegment.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include <mmreg.h>
#include <AudioPathDesigner.h>
#include <SegmentDesigner.h>
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//	CVirtualSegment Constructor/Destructor

CVirtualSegment::CVirtualSegment( CDirectMusicSong* pSong )
{
	m_dwRef = 0;
	AddRef();

	ASSERT( pSong != NULL );
	m_pSong = pSong;
//	m_pSong->AddRef();			intentionally missing

	// UI
	m_fResolveNextPlayID = false;
	m_dwNextPlayID = 0;
	m_dwBitsUI = 0;
	m_fIsPlaying = false;

	// UNFO/UNAM
	m_strName.LoadString( IDS_V_EMPTY_TEXT );
	
	// DMUS_IO_SEGREF_HEADER
	m_pSourceSegment = NULL;
	m_pIToolGraphNode = NULL;
	m_pNextVirtualSegment = NULL;
    
	// DMUS_IO_SEGMENT_HEADER
	m_dwRepeats_SEGH = 0;
    m_mtLength_SEGH = m_VirtualSegmentUI.dwNbrMeasures * DMUS_PPQ * 4;	// XX measures of 4/4 time
    m_mtPlayStart_SEGH = 0;
    m_mtLoopStart_SEGH = 0;
    m_mtLoopEnd_SEGH = 0;	
    m_dwResolution_SEGH = 0;
    m_rtLength_SEGH = 0;					// Length, in reference time (overrides music time length.) 
    m_dwFlags_SEGH = 0;
}

CVirtualSegment::~CVirtualSegment( void )
{
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	// Empty lists
	EmptyTrackList();
	EmptyTransitionList();

	RELEASE( m_pSourceSegment );
	RELEASE( m_pIToolGraphNode );
	RELEASE( m_pNextVirtualSegment );
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment IUnknown implementation

HRESULT CVirtualSegment::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IUnknown *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CVirtualSegment::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CVirtualSegment::Release()
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


//////////////////////////////////////////////////////////////////////
//	CVirtualSegment::GetSourceSegmentName

void CVirtualSegment::GetSourceSegmentName( CString& strSourceSegmentName )
{
	strSourceSegmentName.Empty();

	if( m_pSourceSegment )
	{
		IDMUSProdNode* pISegmentDocRootNode;
		if( SUCCEEDED ( m_pSourceSegment->GetSegmentDocRootNode( &pISegmentDocRootNode ) ) )
		{
			BSTR bstrName;
			if( SUCCEEDED ( pISegmentDocRootNode->GetNodeName( &bstrName ) ) )
			{
				strSourceSegmentName = bstrName;
				::SysFreeString( bstrName );
			}

			RELEASE( pISegmentDocRootNode );
		}
	}
}


//////////////////////////////////////////////////////////////////////
//	CVirtualSegment::GetName

void CVirtualSegment::GetName( CString& strName )
{
	strName = m_strName;
}


//////////////////////////////////////////////////////////////////////
//	CVirtualSegment::SetName

void CVirtualSegment::SetName( CString strName )
{
	m_strName = strName;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::RemoveVirtualSegmentReferences

void CVirtualSegment::RemoveVirtualSegmentReferences( CVirtualSegment* pVirtualSegmentToRemove )
{
	if( pVirtualSegmentToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Handle "next" VirtualSegment
	if( m_pNextVirtualSegment == pVirtualSegmentToRemove )
	{
		RELEASE( m_pNextVirtualSegment );
	}

	// Handle transitions
	POSITION posCur;
	POSITION pos = m_lstTransitions.GetHeadPosition();
	while( pos )
	{
		posCur = pos;
		CTransition* pTransition = m_lstTransitions.GetNext( pos );

		if( pTransition->m_pToSegment == pVirtualSegmentToRemove )
		{
			m_lstTransitions.RemoveAt( posCur );
			RELEASE( pTransition );
		}
		else if( pTransition->m_pTransitionSegment == pVirtualSegmentToRemove )
		{
			RELEASE( pTransition->m_pTransitionSegment );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::RemoveSourceSegmentReferences

void CVirtualSegment::RemoveSourceSegmentReferences( CSourceSegment* pSourceSegmentToRemove )
{
	if( pSourceSegmentToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Handle this VirtualSegment
	if( m_pSourceSegment == pSourceSegmentToRemove )
	{
		RELEASE( m_pSourceSegment );
	}

	// Handle tracks
	POSITION posCur;
	POSITION pos = m_lstTracks.GetHeadPosition();
	while( pos )
	{
		posCur = pos;
		CTrack* pTrack = m_lstTracks.GetNext( pos );

		if( pTrack->m_pSourceSegment == pSourceSegmentToRemove )
		{
			m_lstTracks.RemoveAt( posCur );
			RELEASE( pTrack );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::RemoveToolGraphReferences

void CVirtualSegment::RemoveToolGraphReferences( IDMUSProdNode* pIToolGraphNodeToRemove )
{
	if( pIToolGraphNodeToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Handle this VirtualSegment
	if( m_pIToolGraphNode == pIToolGraphNodeToRemove )
	{
		RELEASE( m_pIToolGraphNode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::ResolveVirtualSegmentIDs

void CVirtualSegment::ResolveVirtualSegmentIDs( void )
{
	// Handle this VirtualSegment
	if( m_fResolveNextPlayID )
	{
		ASSERT( m_pNextVirtualSegment == NULL );
	
		m_pNextVirtualSegment = m_pSong->IndexToVirtualSegment( m_dwNextPlayID );
		if( m_pNextVirtualSegment )
		{
			m_pNextVirtualSegment->AddRef();
		}

		m_fResolveNextPlayID = false;
		m_dwNextPlayID = 0;
	}

	// Handle transitions
	POSITION pos = m_lstTransitions.GetHeadPosition();
	while( pos )
	{
		CTransition* pTransition = m_lstTransitions.GetNext( pos );

		if( pTransition->m_fResolveToSegmentID )
		{
			ASSERT( pTransition->m_pToSegment == NULL );
		
			pTransition->m_pToSegment = m_pSong->IndexToVirtualSegment( pTransition->m_dwToSegmentID );
			if( pTransition->m_pToSegment )
			{
				pTransition->m_pToSegment->AddRef();
			}

			pTransition->m_fResolveToSegmentID = false;
			pTransition->m_dwToSegmentID = 0;
		}

		if( pTransition->m_fResolveTransitionSegmentID )
		{
			ASSERT( pTransition->m_pTransitionSegment == NULL );
		
			pTransition->m_pTransitionSegment = m_pSong->IndexToVirtualSegment( pTransition->m_dwTransitionSegmentID );
			if( pTransition->m_pTransitionSegment )
			{
				pTransition->m_pTransitionSegment->AddRef();
			}

			pTransition->m_fResolveTransitionSegmentID = false;
			pTransition->m_dwTransitionSegmentID = 0;
		}
	}
}


//////////////////////////////////////////////////////////////////////
//	CVirtualSegment::IsVirtualSegmentInTransitionList

bool CVirtualSegment::IsVirtualSegmentInTransitionList( CVirtualSegment* pVirtualSegment )
{
	POSITION pos = m_lstTransitions.GetHeadPosition();
	while( pos )
	{
		CTransition* pTransition = m_lstTransitions.GetNext( pos );

		if( pTransition->m_pToSegment == pVirtualSegment )
		{
			return true;
		}
	}

	return false;
}


#define DMUS_FOURCC_SEGMENT_DESIGN_LIST		mmioFOURCC('s','g','d','l')
#define DMUS_FOURCC_SEGMENT_DESIGN_CHUNK	mmioFOURCC('s','e','g','d')

typedef struct _PARTIAL_SEGMENT_DESIGN_STRUCT
{
	double		dblTempo;			// Tempo. By default, 120
	float		fTempoModifier;		// Tempo modifier. By default, 0.5
	double		dblZoom;			// Timeline zoom factor. By default, 0.03125
	DWORD		dwMeasureLength;	// Length in measures
	WORD		wSegmentRepeats;	// Design-time repeats
	BYTE		bKey;				// Key 0-11 (NO LONGER USED!!!)
	BOOL		fShowFlats;			// Flats/sharps (NO LONGER USED!!!)
	BOOL		fReserved;			// No longer used (was Template/Section)
	DWORD		dwPlayStartMeasure;	// Start measure
	BYTE		bPlayStartBeat;		// Start beat
	WORD		wPlayStartGrid;		// Start grid
	WORD		wPlayStartTick;		// Start tick
	DWORD		dwTrackGroup;		// Group to get TimeSig from
} PARTIAL_SEGMENT_DESIGN_STRUCT;

//////////////////////////////////////////////////////////////////////
//	CVirtualSegment::SetDefaultValues

HRESULT CVirtualSegment::SetDefaultValues( IStream* pIStream )
{
    MMCKINFO ckMain;
    MMCKINFO ckList;
    MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;

    IDMUSProdRIFFStream* pIRiffStream = NULL;
	
	HRESULT hr = AllocRIFFStream( pIStream, &pIRiffStream );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	ckMain.fccType = DMUS_FOURCC_SEGMENT_FORM;
	if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
	{
		// Get Segment header
		ck.ckid = DMUS_FOURCC_SEGMENT_CHUNK;
		if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
		{
			DMUS_IO_SEGMENT_HEADER dmusSegHeaderIO;

			dwSize = min( ck.cksize, sizeof( DMUS_IO_SEGMENT_HEADER ) );
			hr = pIStream->Read( &dmusSegHeaderIO, dwSize, &dwByteCount );
			if( FAILED( hr )
			||  dwByteCount != dwSize )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			m_dwRepeats_SEGH = dmusSegHeaderIO.dwRepeats;
			m_mtLength_SEGH = dmusSegHeaderIO.mtLength;
			m_mtPlayStart_SEGH = dmusSegHeaderIO.mtPlayStart;
			m_mtLoopStart_SEGH = dmusSegHeaderIO.mtLoopStart;
			m_mtLoopEnd_SEGH = dmusSegHeaderIO.mtLoopEnd;
			m_dwResolution_SEGH = dmusSegHeaderIO.dwResolution;
			m_rtLength_SEGH = dmusSegHeaderIO.rtLength;
			m_dwFlags_SEGH = dmusSegHeaderIO.dwFlags;
		}

		DWORD dwPos = StreamTell( pIStream );

		// Get Segment name
		ck.fccType = DMUS_FOURCC_UNFO_LIST;
		if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDLIST ) == 0 )
		{
			ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
				CString strPrefix;
				strPrefix.LoadString( IDS_VIRTUAL_SEGMENT_PREFIX_TEXT ); 

				CString strName;
				ReadMBSfromWCS( pIStream, ck.cksize, &strName );
				
				m_strName = strPrefix + strName;
			}
		}

		StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		// Get Track Group
		ck.fccType = DMUS_FOURCC_SEGMENT_DESIGN_LIST;
		if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDLIST ) == 0 )
		{
			ckList.ckid = DMUS_FOURCC_SEGMENT_DESIGN_CHUNK;
			if( pIRiffStream->Descend( &ckList, &ck, MMIO_FINDCHUNK ) == 0 )
			{
				PARTIAL_SEGMENT_DESIGN_STRUCT iDMSegmentDesign;
				ZeroMemory( &iDMSegmentDesign, sizeof( PARTIAL_SEGMENT_DESIGN_STRUCT ) );

				dwSize = min( ckList.cksize, sizeof( PARTIAL_SEGMENT_DESIGN_STRUCT ) );
				hr = pIStream->Read( &iDMSegmentDesign, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_VirtualSegmentUI.dwTimeSigGroupBits = iDMSegmentDesign.dwTrackGroup;
			}
		}
	}

ON_ERROR:
	RELEASE( pIRiffStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment Load

HRESULT CVirtualSegment::Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*       pIStream;
	MMCKINFO	   ck;
	MMCKINFO	   ckList;
	DWORD		   dwByteCount;
	DWORD		   dwSize;
    HRESULT        hr = S_OK;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_SEGREF_CHUNK:
			{
				DMUS_IO_SEGREF_HEADER dmusSegRefIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_SEGREF_HEADER ) );
				hr = pIStream->Read( &dmusSegRefIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( dmusSegRefIO.dwSegmentID == DMUS_SONG_NOSEG )
				{
					m_pSourceSegment = NULL;
				}
				else
				{
					m_pSourceSegment = m_pSong->IndexToSourceSegment( dmusSegRefIO.dwSegmentID );
					if( m_pSourceSegment )
					{
						m_pSourceSegment->AddRef();
					}
				}
				if( dmusSegRefIO.dwToolGraphID == DMUS_SONG_NOSEG )
				{
					m_pIToolGraphNode = NULL;
				}
				else
				{
					m_pIToolGraphNode = m_pSong->IndexToToolGraph( dmusSegRefIO.dwToolGraphID );
					if( m_pIToolGraphNode )
					{
						m_pIToolGraphNode->AddRef();
					}
				}
				if( dmusSegRefIO.dwNextPlayID == DMUS_SONG_NOSEG )
				{
					m_fResolveNextPlayID = false;
					m_pNextVirtualSegment = NULL;
				}
				else
				{
					m_fResolveNextPlayID = true;
					m_dwNextPlayID = dmusSegRefIO.dwNextPlayID;
				}
				break;
			}

			case DMUS_FOURCC_SEGMENT_CHUNK:
			{
			    DMUS_IO_SEGMENT_HEADER dmusSegHeaderIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_SEGMENT_HEADER ) );
				hr = pIStream->Read( &dmusSegHeaderIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwRepeats_SEGH = dmusSegHeaderIO.dwRepeats;
				m_mtLength_SEGH = dmusSegHeaderIO.mtLength;
				m_mtPlayStart_SEGH = dmusSegHeaderIO.mtPlayStart;
				m_mtLoopStart_SEGH = dmusSegHeaderIO.mtLoopStart;
				m_mtLoopEnd_SEGH = dmusSegHeaderIO.mtLoopEnd;
				m_dwResolution_SEGH = dmusSegHeaderIO.dwResolution;
				m_rtLength_SEGH = dmusSegHeaderIO.rtLength;
				m_dwFlags_SEGH = dmusSegHeaderIO.dwFlags;
				break;
			}

			case DMUS_FOURCC_VIRTUAL_SEGMENT_UI_CHUNK:
			{
			    ioVirtualSegmentUI iVirtualSegmentUI;

				dwSize = min( ck.cksize, sizeof( ioVirtualSegmentUI ) );
				hr = pIStream->Read( &iVirtualSegmentUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_VirtualSegmentUI.wFlags = iVirtualSegmentUI.wFlags;
				m_VirtualSegmentUI.guidVirtualSegment = iVirtualSegmentUI.guidVirtualSegment;
				m_VirtualSegmentUI.dwTimeSigGroupBits = iVirtualSegmentUI.dwTimeSigGroupBits;
				m_VirtualSegmentUI.dwNbrMeasures = iVirtualSegmentUI.dwNbrMeasures;
				break;
			}

			case DMUS_FOURCC_VIRTUAL_SEGMENT_COPY_PASTE_UI_CHUNK:
			{
			    ioVirtualSegmentCopyPasteUI iVirtualSegmentCopyPasteUI;

				dwSize = min( ck.cksize, sizeof( ioVirtualSegmentCopyPasteUI ) );
				hr = pIStream->Read( &iVirtualSegmentCopyPasteUI, dwSize, &dwByteCount );
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
				m_pSourceSegment = m_pSong->PtrToSourceSegment( iVirtualSegmentCopyPasteUI.pSourceSegment );
				if( m_pSourceSegment )
				{
					m_pSourceSegment->AddRef();
				}
				
				// m_pIToolGraphNode
				RELEASE( m_pIToolGraphNode );
				m_pIToolGraphNode = m_pSong->PtrToToolGraph( iVirtualSegmentCopyPasteUI.pIToolGraphNode );
				if( m_pIToolGraphNode )
				{
					m_pIToolGraphNode->AddRef();
				}

				// m_pNextVirtualSegment will be set later according to drop/paste position
				m_fResolveNextPlayID = false;
				m_pNextVirtualSegment = NULL;
				break;
			}

			case DMUS_FOURCC_SEGTRANS_CHUNK:
			{
			    DMUS_IO_TRANSITION_DEF dmusTransitionIO;
				CTransition* pTransition;

				dwSize = ck.cksize;
				while( dwSize > 0 )
				{
					hr = pIStream->Read( &dmusTransitionIO, sizeof(DMUS_IO_TRANSITION_DEF), &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != sizeof(DMUS_IO_TRANSITION_DEF) )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}

					pTransition = new CTransition( m_pSong );
					if( pTransition == NULL )
					{
						hr = E_OUTOFMEMORY;
						goto ON_ERROR;
					}

					if( dmusTransitionIO.dwSegmentID == DMUS_SONG_NOSEG )
					{
						pTransition->m_dwToSegmentFlag = DMUS_SONG_NOSEG;
						pTransition->m_pToSegment = NULL;
					}
					else if( dmusTransitionIO.dwSegmentID == DMUS_SONG_ANYSEG )
					{
						pTransition->m_dwToSegmentFlag = DMUS_SONG_ANYSEG;
						pTransition->m_pToSegment = NULL;
					}
					else if( dmusTransitionIO.dwSegmentID == DMUS_SONG_NOFROMSEG )
					{
						pTransition->m_dwToSegmentFlag = DMUS_SONG_NOFROMSEG;
						pTransition->m_pToSegment = NULL;
					}
					else
					{
						pTransition->m_dwToSegmentFlag = 0;
						pTransition->m_fResolveToSegmentID = true;
						pTransition->m_dwToSegmentID = dmusTransitionIO.dwSegmentID;
					}
					if( dmusTransitionIO.dwTransitionID == DMUS_SONG_NOSEG )
					{
						pTransition->m_fResolveTransitionSegmentID = false;
						pTransition->m_pTransitionSegment= NULL;
					}
					else
					{
						pTransition->m_fResolveTransitionSegmentID = true;
						pTransition->m_dwTransitionSegmentID = dmusTransitionIO.dwTransitionID;
					}
					pTransition->m_dwPlayFlagsDM = dmusTransitionIO.dwPlayFlags;
					m_lstTransitions.AddTail( pTransition );

					dwSize -= dwByteCount;
				}
				break;
			}

			case DMUS_FOURCC_TRANSITION_COPY_PASTE_UI_CHUNK:
			{
				ioTransitionCopyPasteUI iTransitionCopyPasteUI;
				CTransition* pTransition;

				dwSize = ck.cksize;
				while( dwSize > 0 )
				{
					hr = pIStream->Read( &iTransitionCopyPasteUI, sizeof(ioTransitionCopyPasteUI), &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != sizeof(ioTransitionCopyPasteUI) )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}

					pTransition = new CTransition( m_pSong );
					if( pTransition == NULL )
					{
						hr = E_OUTOFMEMORY;
						goto ON_ERROR;
					}

					pTransition->m_dwToSegmentFlag = iTransitionCopyPasteUI.dwToSegmentFlag;
					pTransition->m_pToSegment = m_pSong->PtrToVirtualSegment( iTransitionCopyPasteUI.pToSegment );
					if( pTransition->m_pToSegment )
					{
						ASSERT( pTransition->m_dwToSegmentFlag == 0 ); 
						pTransition->m_pToSegment->AddRef();
					}
					else
					{
						// No "To Segment" so we must have a value in m_dwToSegmentFlag
						if( pTransition->m_dwToSegmentFlag == 0 )
						{
							pTransition->m_dwToSegmentFlag = DMUS_SONG_NOSEG;
						}
					}
					pTransition->m_pTransitionSegment = m_pSong->PtrToVirtualSegment( iTransitionCopyPasteUI.pTransitionSegment );
					if( pTransition->m_pTransitionSegment )
					{
						pTransition->m_pTransitionSegment->AddRef();
					}
					pTransition->m_dwPlayFlagsDM = iTransitionCopyPasteUI.dwPlayFlagsDM;
					m_lstTransitions.AddTail( pTransition );

					dwSize -= dwByteCount;
				}
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_TRACKREFS_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case FOURCC_LIST:
									switch( ckList.fccType )
									{
										case DMUS_FOURCC_TRACKREF_LIST:
										{
											CTrack* pTrack = new CTrack( m_pSong );
											if( pTrack == NULL )
											{
												hr = E_OUTOFMEMORY;
												goto ON_ERROR;
											}

											hr = pTrack->Load( pIRiffStream, &ck );
											if( hr == S_OK )
											{
												InsertTrack( pTrack );
												RELEASE( pTrack );
											}
											else if( hr == S_FALSE )
											{
												// Could not resolve SourceSegment so discard this track
												RELEASE( pTrack );
												hr = S_OK;
											}
											else
											{
												RELEASE( pTrack );
												goto ON_ERROR;
											}
											break;
										}
									}
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
    }

	long lMeasure, lBeat, lGrid, lTick;
	if( SUCCEEDED ( ClocksToMeasureBeatGridTick( m_mtLength_SEGH, &lMeasure, &lBeat, &lGrid, &lTick ) ) )
	{
		m_VirtualSegmentUI.dwNbrMeasures = lMeasure;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment SaveTransitionList
    
HRESULT CVirtualSegment::SaveTransitionList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	POSITION pos;
	DMUS_IO_TRANSITION_DEF dmusTransitionIO;

	if( m_lstTransitions.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write chunk header
	ck.ckid = DMUS_FOURCC_SEGTRANS_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	pos = m_lstTransitions.GetHeadPosition();
	while( pos )
	{
		CTransition* pTransition = m_lstTransitions.GetNext( pos );

		// Prepare DMUS_IO_TRANSITION_DEF structure
		memset( &dmusTransitionIO, 0, sizeof(DMUS_IO_TRANSITION_DEF) );

		if( pTransition->m_pToSegment )
		{
			ASSERT( pTransition->m_dwToSegmentFlag == 0 );
			dmusTransitionIO.dwSegmentID = m_pSong->VirtualSegmentToIndex( pTransition->m_pToSegment );
		}
		else
		{
			// No "To Segment" so we must have a value in m_dwToSegmentFlag
			if( pTransition->m_dwToSegmentFlag == 0 )
			{
				pTransition->m_dwToSegmentFlag = DMUS_SONG_NOSEG;
			}
			dmusTransitionIO.dwSegmentID = pTransition->m_dwToSegmentFlag;
		}
		if( pTransition->m_pTransitionSegment )
		{
			dmusTransitionIO.dwTransitionID = m_pSong->VirtualSegmentToIndex( pTransition->m_pTransitionSegment );
		}
		else
		{
			dmusTransitionIO.dwTransitionID = DMUS_SONG_NOSEG;
		}
		dmusTransitionIO.dwPlayFlags = pTransition->m_dwPlayFlagsDM;

		// Write Transition chunk data
		hr = pIStream->Write( &dmusTransitionIO, sizeof(DMUS_IO_TRANSITION_DEF), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(DMUS_IO_TRANSITION_DEF) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
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
// CVirtualSegment SaveTrackList
    
HRESULT CVirtualSegment::SaveTrackList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
    MMCKINFO ckMain;
	POSITION pos;

	if( m_lstTracks.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write UNFO DMUS_FOURCC_TRACKREFS_LIST header
	ckMain.fccType = DMUS_FOURCC_TRACKREFS_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save all tracks
	pos = m_lstTracks.GetHeadPosition();
	while( pos )
	{
		CTrack* pTrack = m_lstTracks.GetNext( pos );

		hr = pTrack->Save( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
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
// CVirtualSegment SaveInfoList
    
HRESULT CVirtualSegment::SaveInfoList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write UNFO LIST header
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write VirtualSegment name
	{
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
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
// CVirtualSegment SaveCopyPasteUI
    
HRESULT CVirtualSegment::SaveCopyPasteUI( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	ioVirtualSegmentCopyPasteUI oVirtualSegmentCopyPasteUI;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write chunk header
	ck.ckid = DMUS_FOURCC_VIRTUAL_SEGMENT_COPY_PASTE_UI_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioVirtualSegmentCopyPasteUI structure
	memset( &oVirtualSegmentCopyPasteUI, 0, sizeof(ioVirtualSegmentCopyPasteUI) );

	oVirtualSegmentCopyPasteUI.pSourceSegment = m_pSourceSegment;
	oVirtualSegmentCopyPasteUI.pIToolGraphNode = m_pIToolGraphNode;

	// Write ioVirtualSegmentCopyPasteUI structure
	hr = pIStream->Write( &oVirtualSegmentCopyPasteUI, sizeof(ioVirtualSegmentCopyPasteUI), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioVirtualSegmentCopyPasteUI) )
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
// CVirtualSegment SaveUI
    
HRESULT CVirtualSegment::SaveUI( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write chunk header
	ck.ckid = DMUS_FOURCC_VIRTUAL_SEGMENT_UI_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write UI chunk data
	hr = pIStream->Write( &m_VirtualSegmentUI, sizeof(ioVirtualSegmentUI), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioVirtualSegmentUI) )
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
// CVirtualSegment SaveSourceHeader

HRESULT CVirtualSegment::SaveSourceHeader( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_SEGMENT_HEADER dmusSegHeaderIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write chunk header
	ck.ckid = DMUS_FOURCC_SEGMENT_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_SEGREF_HEADER structure
	memset( &dmusSegHeaderIO, 0, sizeof(DMUS_IO_SEGMENT_HEADER) );

	dmusSegHeaderIO.dwRepeats = m_dwRepeats_SEGH;
	dmusSegHeaderIO.mtLength = m_mtLength_SEGH;
	dmusSegHeaderIO.mtPlayStart = m_mtPlayStart_SEGH;
	dmusSegHeaderIO.mtLoopStart = m_mtLoopStart_SEGH;
	dmusSegHeaderIO.mtLoopEnd = m_mtLoopEnd_SEGH;
	dmusSegHeaderIO.dwResolution = m_dwResolution_SEGH;
	dmusSegHeaderIO.rtLength = m_rtLength_SEGH;
	dmusSegHeaderIO.dwFlags = m_dwFlags_SEGH;

	// Write SourceSegment header data
	hr = pIStream->Write( &dmusSegHeaderIO, sizeof(DMUS_IO_SEGMENT_HEADER), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_SEGMENT_HEADER) )
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
// CVirtualSegment SaveVirtualHeader
    
HRESULT CVirtualSegment::SaveVirtualHeader( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_SEGREF_HEADER dmusSegRefIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write chunk header
	ck.ckid = DMUS_FOURCC_SEGREF_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_SEGREF_HEADER structure
	memset( &dmusSegRefIO, 0, sizeof(DMUS_IO_SEGREF_HEADER) );

	dmusSegRefIO.dwID = m_pSong->VirtualSegmentToIndex( this );
	if( m_pSourceSegment )
	{
		dmusSegRefIO.dwSegmentID = m_pSong->SourceSegmentToIndex( m_pSourceSegment );
	}
	else
	{
		dmusSegRefIO.dwSegmentID = DMUS_SONG_NOSEG;
	}
	if( m_pIToolGraphNode )
	{
		dmusSegRefIO.dwToolGraphID = m_pSong->ToolGraphToIndex( m_pIToolGraphNode );
	}
	else
	{
		dmusSegRefIO.dwToolGraphID = DMUS_SONG_NOSEG;
	}
	if( m_pNextVirtualSegment )
	{
		dmusSegRefIO.dwNextPlayID = m_pSong->VirtualSegmentToIndex( m_pNextVirtualSegment );
	}
	else
	{
		dmusSegRefIO.dwNextPlayID = DMUS_SONG_NOSEG;
	}
	dmusSegRefIO.dwFlags = 0;			// Not used

	// Write VirtualSegment header data
	hr = pIStream->Write( &dmusSegRefIO, sizeof(DMUS_IO_SEGREF_HEADER), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_SEGREF_HEADER) )
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
// CVirtualSegment SaveSongGuidUI
    
HRESULT CVirtualSegment::SaveSongGuidUI( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write GUID chunk header
	ck.ckid = DMUS_FOURCC_SONG_GUID_UI_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Song GUID
	GUID guidSong;
	m_pSong->GetGUID( &guidSong );
	hr = pIStream->Write( &guidSong, sizeof(GUID), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(GUID) )
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
// CVirtualSegment SaveSelectedTransitions

HRESULT CVirtualSegment::SaveSelectedTransitions( IDMUSProdRIFFStream* pIRiffStream, bool fSaveAll )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	POSITION pos;
	ioTransitionCopyPasteUI oTransitionCopyPasteUI;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Save Song GUID UI chunk
	// Used for copy/paste of Transitions
	hr = SaveSongGuidUI( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	{
		// Write DMUS_FOURCC_TRANSITION_COPY_PASTE_UI_CHUNK header
		ck.ckid = DMUS_FOURCC_TRANSITION_COPY_PASTE_UI_CHUNK;
		if( pIRiffStream->CreateChunk(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Save selected Transitions
		pos = m_lstTransitions.GetHeadPosition();
		while( pos )
		{
			CTransition* pTransition = m_lstTransitions.GetNext( pos );

			if( (pTransition->m_dwBitsUI & BF_SELECTED)
			||   fSaveAll )
			{
				// Prepare ioTransitionCopyPasteUI structure
				memset( &oTransitionCopyPasteUI, 0, sizeof(ioTransitionCopyPasteUI) );

				if( pTransition->m_pToSegment == NULL )
				{
					// No "To Segment" so we must have a value in m_dwToSegmentFlag
					if( pTransition->m_dwToSegmentFlag == 0 )
					{
						pTransition->m_dwToSegmentFlag = DMUS_SONG_NOSEG;
					}
				}
				oTransitionCopyPasteUI.dwToSegmentFlag = pTransition->m_dwToSegmentFlag;
				oTransitionCopyPasteUI.pToSegment = pTransition->m_pToSegment;
				oTransitionCopyPasteUI.pTransitionSegment = pTransition->m_pTransitionSegment;
				oTransitionCopyPasteUI.dwPlayFlagsDM = pTransition->m_dwPlayFlagsDM;

				// Write Transition chunk data
				hr = pIStream->Write( &oTransitionCopyPasteUI, sizeof(ioTransitionCopyPasteUI), &dwBytesWritten);
				if( FAILED( hr )
				||  dwBytesWritten != sizeof(ioTransitionCopyPasteUI) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment SaveSelectedTracks

HRESULT CVirtualSegment::SaveSelectedTracks( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ckMain;
	POSITION pos;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Save Song GUID UI chunk
	// Used for copy/paste of Tracks
	hr = SaveSongGuidUI( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	{
		// Write DMUS_FOURCC_TRACKREFS_LIST header
		ckMain.fccType = DMUS_FOURCC_TRACKREFS_LIST;
		if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Save selected Tracks
		pos = m_lstTracks.GetHeadPosition();
		while( pos )
		{
			CTrack* pTrack = m_lstTracks.GetNext( pos );

			if( pTrack->m_dwBitsUI & BF_SELECTED )
			{
				hr = pTrack->Save( pIRiffStream );
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
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment SaveVirtualSegment

HRESULT CVirtualSegment::SaveVirtualSegment( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;

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

	// Save VirtualSegment header chunk
	hr = SaveVirtualHeader( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save SourceSegment header chunk
	hr = SaveSourceHeader( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save Song GUID UI chunk
	// Used for drag/drop and copy/paste of Tracks and Transitions
	if( StreamInfo.ftFileType != FT_RUNTIME )
	{
		hr = SaveSongGuidUI( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
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
		if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_SaveSelectedVirtualSegments ) )
		{
			hr = SaveCopyPasteUI( pIRiffStream );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
	}

	// Save UNFO list
	hr = SaveInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save Transition list
	if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_SaveSelectedVirtualSegments ) )
	{
		// Save Copy/Paste version of Transition list
		hr = SaveSelectedTransitions( pIRiffStream, true );
	}
	else
	{
		hr = SaveTransitionList( pIRiffStream );
	}
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save Track list
	hr = SaveTrackList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment Save

HRESULT CVirtualSegment::Save( IDMUSProdRIFFStream* pIRiffStream )
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

	// Write DMUS_FOURCC_SEGREF_LIST header
	ckMain.fccType = DMUS_FOURCC_SEGREF_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_SaveSelectedTracks ) )
	{
		// Copy/paste tracks
		hr = SaveSelectedTracks( pIRiffStream );
	}
	else if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_SaveSelectedTransitions ) )
	{
		// Copy/paste transitions
		hr = SaveSelectedTransitions( pIRiffStream, false );
	}
	else
	{
		hr = SaveVirtualSegment( pIRiffStream );
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


///////////////////////////////////////////////////////////////////////////
// CVirtualSegment::EmptyTrackList

void CVirtualSegment::EmptyTrackList( void )
{
	CTrack* pTrack;

	while( !m_lstTracks.IsEmpty() )
	{
		pTrack = m_lstTracks.RemoveHead();

		RELEASE( pTrack );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::InsertTrack

void CVirtualSegment::InsertTrack( CTrack* pTrackToInsert )
{
	if( pTrackToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// If this is a sequence track
	if( CLSID_DirectMusicSeqTrack == pTrackToInsert->m_clsidTrack )
	{
		// Check if we already have a sequence track in this track group
		POSITION pos = m_lstTracks.GetHeadPosition();
		while( pos )
		{
			CTrack *pTmpTrack = m_lstTracks.GetNext( pos );

			if( (CLSID_DirectMusicSeqTrack == pTmpTrack->m_clsidTrack)
			&&	(pTrackToInsert->m_dwGroup == pTmpTrack->m_dwGroup)
			&&	(pTrackToInsert->m_pSourceSegment == pTmpTrack->m_pSourceSegment) )
			{
				// Don't insert the track, since one already exists in the same group
				return;
			}
		}

		// The sequence track's position is always 0
		pTrackToInsert->m_dwPosition = 0;
	}

	pTrackToInsert->AddRef();

	// Place Track in m_lstTracks
	m_lstTracks.AddTail( pTrackToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::RemoveTrack

void CVirtualSegment::RemoveTrack( CTrack* pTrackToRemove )
{
	if( pTrackToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// If item is in m_lstTracks, remove it
	POSITION pos = m_lstTracks.Find( pTrackToRemove );
	if( pos )
	{
		m_lstTracks.RemoveAt( pos );

		RELEASE( pTrackToRemove );
	}
}


///////////////////////////////////////////////////////////////////////////
// CVirtualSegment::EmptyTransitionList

void CVirtualSegment::EmptyTransitionList( void )
{
	CTransition* pTransition;

	while( !m_lstTransitions.IsEmpty() )
	{
		pTransition = m_lstTransitions.RemoveHead();

		RELEASE( pTransition );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::InsertTransition

void CVirtualSegment::InsertTransition( CTransition* pTransitionToInsert )
{
	if( pTransitionToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	pTransitionToInsert->AddRef();

	m_lstTransitions.AddTail( pTransitionToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::RemoveTransition

void CVirtualSegment::RemoveTransition( CTransition* pTransitionToRemove )
{
	if( pTransitionToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// If item is in m_lstTransitions, remove it
	POSITION pos = m_lstTransitions.Find( pTransitionToRemove );
	if( pos )
	{
		m_lstTransitions.RemoveAt( pos );

		RELEASE( pTransitionToRemove );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::ClocksToMeasureBeatGridTick

HRESULT	CVirtualSegment::ClocksToMeasureBeatGridTick( MUSIC_TIME mtTime, long* plMeasure, long* plBeat, long* plGrid, long* plTick )
{
	long lMeasure = -1;
	long lBeat = -1;
	long lGrid = -1;
	long lTick = -1;

	HRESULT hr = S_OK;

	if( mtTime < 0 )
	{
		lMeasure = 0;
		lBeat = 0;
		lGrid = 0;
		lTick = mtTime;
	}
	else
	{
		IDirectMusicSegment* pIDMSegment = GetSegmentForTimeSigConversions();

		hr = ClocksToMeasureBeat( pIDMSegment, mtTime, &lMeasure, &lBeat );
		if( SUCCEEDED ( hr ) )
		{
			long lClocks;
			hr = MeasureBeatToClocks( pIDMSegment, lMeasure, lBeat, &lClocks );
			if( SUCCEEDED ( hr ) )
			{
				long lGridsAndTicks = mtTime - lClocks;

				// Get the TimeSig for this measure/beat
				DMUS_TIMESIGNATURE ts;
				if( (pIDMSegment == NULL)
				||  (FAILED( pIDMSegment->GetParam( GUID_TimeSignature, m_VirtualSegmentUI.dwTimeSigGroupBits, 0, lClocks, NULL, &ts ) )) )
				{
					ts.mtTime = 0;
					ts.bBeatsPerMeasure = 4;
					ts.bBeat = 4;
					ts.wGridsPerBeat = 2;
				}

				// Compute the grid and tick
				long lClocksPerGrid = ((DMUS_PPQ << 2) / ts.bBeat) / ts.wGridsPerBeat;
				lGrid = lGridsAndTicks / lClocksPerGrid;
				lTick = lGridsAndTicks % lClocksPerGrid;

				// Try and preserve negative tick offsets
				if( lTick > 0 )
				{
					long lNewClocks = lClocks + lClocksPerGrid;

					if( lTick >= (lClocksPerGrid >> 1) )
					{
						if( lNewClocks < m_mtLength_SEGH )
						{
							lGrid++;
							lTick -= lClocksPerGrid;
							if( lGrid >= ts.wGridsPerBeat )
							{
								lGrid = 0;
								mtTime += lClocksPerGrid;
								hr = ClocksToMeasureBeat( pIDMSegment, mtTime, &lMeasure, &lBeat );
							}
						}
					}
				}
			}
		}

		RELEASE( pIDMSegment );
	}

	*plMeasure = lMeasure;
	*plBeat = lBeat;
	*plGrid = lGrid;
	*plTick	= lTick;

	ASSERT( SUCCEEDED ( hr ) );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::MeasureBeatGridTickToClocks

HRESULT	CVirtualSegment::MeasureBeatGridTickToClocks( long lMeasure, long lBeat, long lGrid, long lTick, MUSIC_TIME* pmtTime )
{
	MUSIC_TIME mtTime = -1;

	HRESULT hr = S_OK;

	IDirectMusicSegment* pIDMSegment = GetSegmentForTimeSigConversions();

	long lClocks;
	hr = MeasureBeatToClocks( pIDMSegment, lMeasure, lBeat, &lClocks );
	if( SUCCEEDED ( hr ) )
	{
		// Get the TimeSig for this measure/beat
		DMUS_TIMESIGNATURE ts;
		if( (pIDMSegment == NULL)
		||  (FAILED( pIDMSegment->GetParam( GUID_TimeSignature, m_VirtualSegmentUI.dwTimeSigGroupBits, 0, lClocks, NULL, &ts ) )) )
		{
			ts.mtTime = 0;
			ts.bBeatsPerMeasure = 4;
			ts.bBeat = 4;
			ts.wGridsPerBeat = 2;
		}

		long lClocksPerGrid = ((DMUS_PPQ << 2) / ts.bBeat) / ts.wGridsPerBeat;
		long lGridClocks = lGrid * lClocksPerGrid;

		mtTime = lClocks + lGridClocks + lTick;
	}

	RELEASE( pIDMSegment );

	*pmtTime = mtTime;

	ASSERT( SUCCEEDED ( hr ) );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::ClocksToMeasureBeat

HRESULT	CVirtualSegment::ClocksToMeasureBeat( IDirectMusicSegment* pIDMSegment,
											  MUSIC_TIME mtTime, long* plMeasure, long* plBeat )
{
	if( plMeasure == NULL
	||  plBeat == NULL )
	{
		ASSERT( 0 );
		return E_POINTER;
	}

	long lMeasure = 0;
	long lBeat = 0;

	MUSIC_TIME mtTSCur = 0;
	MUSIC_TIME mtTSNext;
	
	DMUS_TIMESIGNATURE TimeSig;
	TimeSig.mtTime = 0;
	TimeSig.bBeatsPerMeasure = 4;
	TimeSig.bBeat = 4;
	TimeSig.wGridsPerBeat = 2;

	// Store segment length
	MUSIC_TIME mtSegmentLength = 0;
	if( pIDMSegment )
	{
		if( FAILED ( pIDMSegment->GetLength( &mtSegmentLength ) ) )
		{
			mtSegmentLength = 0;
		}
	}

	do
	{
		// Get the current time signature
		if( pIDMSegment )
		{
			if( FAILED ( pIDMSegment->GetParam( GUID_TimeSignature, m_VirtualSegmentUI.dwTimeSigGroupBits, 0, mtTSCur, &mtTSNext, &TimeSig ) ) )
			{
				ASSERT( 0 );
				return E_UNEXPECTED;
			}
		}

		// If mtTime is less than 0, only use the first TimeSig
		if( mtTime < 0 )
		{
			lMeasure += mtTime / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));
			mtTime = -(abs(mtTime) % (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ )));
			break;
		}
		// If no pIDMSegment -OR- no next time signature, do the math to find how many more measures to add
		else if( pIDMSegment == NULL
			 ||  mtTSNext == 0 
			 ||  mtTSNext == mtSegmentLength )
		{
			lMeasure += mtTime / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));
			mtTime %= TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
			break;
		}
		// Otherwise it's more complicated
		else
		{
			// If the next time signature is after the time we're looking for
			if( mtTime < mtTSNext )
			{
				// Add the number of complete measures between here and there
				lMeasure += mtTime / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));

				// mtTime now stores an offset from the beginning of the measure
				mtTime %= TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
				break;
			}
			// The next time signature is before the time we're looking for
			else
			{
				// Compute how many complete measures there are between now and the next Time signature
				long lMeasureDiff= mtTSNext / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));

				// Add them to lMeasure
				lMeasure += lMeasureDiff;

				// Change lMeasureDiff from measures to clocks
				lMeasureDiff *= TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );

				// Subtract from the time left (mtTime) and add to the current time (mtTSCur)
				mtTime -= lMeasureDiff;
				mtTSCur += lMeasureDiff;
			}
		}
	}
	// While the time left is greater than 0
	while( mtTime > 0 );

	if( mtTime < 0 )
	{
		mtTime += TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
		lMeasure--;
	}

	if( mtTime != 0 && plBeat != NULL )
	{
		lBeat = mtTime / NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
	}
	else
	{
		lBeat = 0;
	}

	*plMeasure = lMeasure;
	*plBeat = lBeat;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::MeasureBeatToClocks

HRESULT	CVirtualSegment::MeasureBeatToClocks( IDirectMusicSegment* pIDMSegment,
											  long lMeasure, long lBeat, MUSIC_TIME* pmtTime )
{
	if( pmtTime == NULL )
	{
		ASSERT( 0 );
		return E_POINTER;
	}

	MUSIC_TIME mtTSCur = 0;
	MUSIC_TIME mtTSNext = 1;

	DMUS_TIMESIGNATURE TimeSig;
	TimeSig.mtTime = 0;
	TimeSig.bBeatsPerMeasure = 4;
	TimeSig.bBeat = 4;
	TimeSig.wGridsPerBeat = 2;

	// Store segment length
	MUSIC_TIME mtSegmentLength = 0;
	if( pIDMSegment )
	{
		if( FAILED ( pIDMSegment->GetLength( &mtSegmentLength ) ) )
		{
			mtSegmentLength = 0;
		}
	}

	do
	{
		// Get the current time signature
		if( pIDMSegment )
		{
			if( FAILED ( pIDMSegment->GetParam( GUID_TimeSignature, m_VirtualSegmentUI.dwTimeSigGroupBits, 0, mtTSCur, &mtTSNext, &TimeSig ) ) )
			{
				ASSERT( 0 );
				return E_UNEXPECTED;
			}
		}

		long lMeasureClocks = TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );

		if( pIDMSegment == NULL 
  	    ||  mtTSNext == 0 
	    ||  mtTSNext == mtSegmentLength )
		{
			mtTSCur += lMeasureClocks * lMeasure;
			break;
		}
		else
		{
			long lTmpMeasures = mtTSNext / lMeasureClocks;
			if( lMeasure <= lTmpMeasures )
			{
				mtTSCur += lMeasureClocks * lMeasure;
				break;
			}
			else
			{
				mtTSCur += lMeasureClocks * lTmpMeasures;
				lMeasure -= lTmpMeasures;
			}
		}
	}
	while( lMeasure > 0 );

	HRESULT hr = S_OK;
	if( lBeat >= 0 )
	{
		if( pIDMSegment )
		{
			hr = pIDMSegment->GetParam( GUID_TimeSignature, m_VirtualSegmentUI.dwTimeSigGroupBits, 0, max(mtTSCur, 0), NULL, &TimeSig );
		}
	}
	else
	{
		if( pIDMSegment )
		{
			hr = pIDMSegment->GetParam( GUID_TimeSignature, m_VirtualSegmentUI.dwTimeSigGroupBits, 0, max(mtTSCur - TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ), 0), NULL, &TimeSig );
		}
	}
	if( FAILED( hr ) )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	mtTSCur += NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ) * lBeat;
	
	*pmtTime = mtTSCur;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::GetSegmentForTimeSigConversions

IDirectMusicSegment* CVirtualSegment::GetSegmentForTimeSigConversions( void )
{
	IDirectMusicSegment* pITheDMSegment = NULL;

	if( m_pSourceSegment )
	{
		IDMUSProdNode* pISegmentDocRootNode;
		if( SUCCEEDED ( m_pSourceSegment->GetSegmentDocRootNode( &pISegmentDocRootNode ) ) )
		{
			IDirectMusicSegment* pIDMSegment;
			if( SUCCEEDED( pISegmentDocRootNode->GetObject( CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pIDMSegment ) ) )
			{
				MUSIC_TIME mtTSNext;
				DMUS_TIMESIGNATURE TimeSig;
				if( SUCCEEDED ( pIDMSegment->GetParam( GUID_TimeSignature, m_VirtualSegmentUI.dwTimeSigGroupBits, 0, 0, &mtTSNext, &TimeSig ) ) )
				{
					pITheDMSegment = pIDMSegment;
					pITheDMSegment->AddRef();
				}

				RELEASE( pIDMSegment );
			}

			RELEASE( pISegmentDocRootNode );
		}
	}

	return pITheDMSegment;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment::HasAudioPath

bool CVirtualSegment::HasAudioPath( void )
{
	if( m_pSourceSegment )
	{
		IDMUSProdNode* pISegmentDocRootNode;
		if( SUCCEEDED ( m_pSourceSegment->GetSegmentDocRootNode( &pISegmentDocRootNode ) ) )
		{
			IDMUSProdNode* pIAudioPathNode;
			if( SUCCEEDED( pISegmentDocRootNode->GetObject( GUID_AudioPathNode, IID_IDMUSProdNode, (void **)&pIAudioPathNode ) ) )
			{
				RELEASE( pIAudioPathNode );
				return true;
			}

			RELEASE( pISegmentDocRootNode );
		}
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualSegment RecomputeLength
    
BOOL CVirtualSegment::RecomputeLength( void )
{
	BOOL fChanged = FALSE;

	// Recalc m_mtLength_SEGH
	MUSIC_TIME mtNewLength;
	if( SUCCEEDED ( MeasureBeatGridTickToClocks( m_VirtualSegmentUI.dwNbrMeasures,
												 0,
												 0,
												 0,
												 &mtNewLength ) ) )
	{
		if( mtNewLength != m_mtLength_SEGH )
		{
			m_mtLength_SEGH = mtNewLength;

			m_mtPlayStart_SEGH = min( m_mtPlayStart_SEGH, (m_mtLength_SEGH - 1) );
			if( m_mtLoopStart_SEGH != 0
			||  m_mtLoopEnd_SEGH != 0 )
			{
				m_mtLoopEnd_SEGH = min( m_mtLoopEnd_SEGH, m_mtLength_SEGH );
				m_mtLoopStart_SEGH = min( m_mtLoopStart_SEGH, (m_mtLoopEnd_SEGH - 1) );
			}

			m_pSong->SetModified( TRUE );
			fChanged = TRUE;
		}
	}

	return fChanged;
}
