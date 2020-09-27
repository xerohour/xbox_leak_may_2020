//	ChordList.cpp

// This class loads an ChordList chunk from a IMA 25 design-time template file.
// It also will be able to write out a DirectMusic Chord track.

#include "stdafx.h"

#include "ChordList.h"
#include "DMUSProd.h"
#include "Track.h"
#include <RiffStrm.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CChordList::CChordList()
{
	m_nLastImportantMeasure = 0;
}

CChordList::~CChordList()
{
	if ( !m_lstChords.IsEmpty() )
	{
		DMChord *pChord;
		while( !m_lstChords.IsEmpty() )
		{
			pChord = m_lstChords.RemoveHead();
			delete pChord;
		}
	}
}

HRESULT CChordList::CreateTrack( class CTrack** ppTrack )
{
	if( m_lstChords.IsEmpty() )
	{
		return E_FAIL;
	}

	if ( ppTrack == NULL )
	{
		return E_INVALIDARG;
	}

	ASSERT( m_pIFramework != NULL );
	if ( m_pIFramework == NULL )
	{
		return E_UNEXPECTED;
	}

	*ppTrack = NULL;
	CTrack *pNewTrack = new CTrack;
	if ( pNewTrack == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream *pStream = NULL;
	
	HRESULT hr = m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStream );
	if ( FAILED( hr ) )
	{
		delete pNewTrack;
		return hr;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( hr = AllocRIFFStream( pStream, &pIRiffStream ) ) )
	{
		pStream->Release();
		delete pNewTrack;
		return hr;
	}

	hr = DM_SaveChordList( pIRiffStream );
	pIRiffStream->Release();
	pIRiffStream = NULL;
	if ( FAILED( hr ) )
	{
		pIRiffStream->Release();
		pStream->Release();
		delete pNewTrack;
		return hr;
	}

	LARGE_INTEGER	liTemp;
	liTemp.QuadPart = 0;
	pStream->Seek(liTemp, STREAM_SEEK_SET, NULL); //seek to beginning

	memcpy( &pNewTrack->m_guidClassID, &CLSID_DirectMusicChordTrack, sizeof( GUID ) );
	// Let the Segment fill m_guidEditorID in.
	//memcpy( &pNewTrack->m_guidEditorID, &CLSID_CommandMgr, sizeof( GUID ) );
	//pNewTrack->dwPosition = 0;
	pNewTrack->m_dwGroupBits = 0x00000001;
	//pNewTrack->punkStripMgr = NULL;
	//pNewTrack->m_ckid = 0;
	pNewTrack->m_fccType = DMUS_FOURCC_CHORDTRACK_LIST;
	pNewTrack->SetStream( pStream );
	pStream->Release(); // Release our reference to the stream

	*ppTrack = pNewTrack;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CChordList::DM_SaveChordList()

HRESULT CChordList::DM_SaveChordList( interface IDMUSProdRIFFStream* pIRIFFStream )
{
	ASSERT( pIRIFFStream != NULL );
	if ( pIRIFFStream == NULL )
	{
		return E_INVALIDARG;
	}


	MMCKINFO ckList;
	HRESULT hr;
	ckList.fccType = DMUS_FOURCC_CHORDTRACK_LIST;
	hr = pIRIFFStream->CreateChunk( &ckList, MMIO_CREATELIST );
	if( hr != S_OK )
	{
		return hr;
	}

	IStream *pIStream;
	pIStream = pIRIFFStream->GetStream();
	ASSERT( pIStream != NULL );
	if ( pIStream == NULL )
	{
		pIRIFFStream->Ascend( &ckList, 0 );
		return E_INVALIDARG;
	}

	DWORD	cb;
	DWORD	dwScale;
	MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_CHORDTRACKHEADER_CHUNK;
	hr = pIRIFFStream->CreateChunk( &ck, 0 );
	if( hr != S_OK )
	{
		return hr;
	}

	// BUGBUG: What should I put here?
	dwScale = 0;
	// dwScale = (bRoot << 24) | dwScale & 0x00ffffff;
	hr = pIStream->Write(&dwScale, sizeof(DWORD), &cb);
	pIRIFFStream->Ascend( &ck, 0 );
	if(FAILED(hr) || cb != sizeof(DWORD))
	{
		pIRIFFStream->Ascend( &ckList, 0 );
		pIStream->Release();
		return hr;
	}

	POSITION pos;
	pos = m_lstChords.GetHeadPosition();
	while(pos)
	{
		DMChord *pChord = m_lstChords.GetNext(pos);

		if( FAILED( pChord->Save( pIRIFFStream ) ) )
		{
			hr = E_FAIL;
			break;
		}
	}

	if( pIRIFFStream->Ascend( &ckList, 0 ) != 0 )
	{
		hr = E_FAIL;
	}

	pIStream->Release();

	return hr;
}

HRESULT CChordList::IMA_AddChord( IStream* pStream, long lRecSize,  BYTE key)
{
	HRESULT 	hr = S_OK;
	DWORD		cb;
	DMChord*	pChord = NULL;
	ioIMAChordSelection iChordSelection;

	if( NULL == pStream )
	{
		return E_POINTER;
	}

	if( lRecSize > sizeof( ioIMAChordSelection ) )
	{
		hr = pStream->Read( &iChordSelection, sizeof( ioIMAChordSelection ), &cb );
		if( FAILED( hr ) || cb != sizeof( ioIMAChordSelection ) )
		{
			hr = E_FAIL;
			goto ON_ERR;
		}
		StreamSeek( pStream, lRecSize - sizeof( ioIMAChordSelection ), STREAM_SEEK_CUR );
	}
	else
	{
		hr = pStream->Read( &iChordSelection, lRecSize, &cb );
		if( FAILED( hr ) || cb != (DWORD)lRecSize )
		{
			hr = E_FAIL;
			goto ON_ERR;
		}
	}

	pChord = new DMChord( iChordSelection, key );

	if( pChord == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto ON_ERR;
	}

	m_lstChords.AddTail(pChord);

ON_ERR:
	return hr;
}

DMChord::DMChord(const ioIMAChordSelection &iChordSelection,  BYTE key)
//
// WARNING: do not use this constructor if iChordSelection is NOT from an IMA chord (even though it might fit)
//
{
	m_strName = iChordSelection.wstrName;
	m_wMeasure = iChordSelection.wMeasure;
	m_bBeat = iChordSelection.bBeat;
	// Unused members:
	// iChordSelection.fCSFlags
	// iChordSelection.bClick

	// old chords are single level, bottom 4 notes = level 1 (bass), top 4 notes = level two (upper).
	DWORD bass = 0, upper = 0;
	DWORD cbass = 4, cupper = 4;
	int bits = sizeof(DWORD) * 8;	// should be 32
	for(int j = 0; j < sizeof(DWORD)*8; j++)
	{
		if( (iChordSelection.aChord[0].lChordPattern & (1 << j)) && cbass > 0)
		{
			--cbass;
			bass |= (1 << j);
		}
		if( (iChordSelection.aChord[0].lChordPattern & (1 << (bits - 1 - j))) && cupper > 0)
		{
			--cupper;
			upper |= 1 << (bits - 1 - j);
		}
	}

	DMSubChord* pSubChord = NULL;
	pSubChord = new DMSubChord( iChordSelection.aChord[0], key );
	if( pSubChord == NULL )
	{
		return;
	}
	pSubChord->m_dwChordPattern = bass;
	pSubChord->m_dwLevels = 0x1;
	m_lstSubChord.AddTail( pSubChord );

	pSubChord = new DMSubChord( iChordSelection.aChord[1] , key);
	if( pSubChord == NULL )
	{
		return;
	}
	pSubChord->m_dwLevels = 0x2;
	pSubChord->m_dwChordPattern = upper;
	m_lstSubChord.AddTail( pSubChord );

	pSubChord = new DMSubChord( iChordSelection.aChord[2], key );
	if( pSubChord == NULL )
	{
		return;
	}
	pSubChord->m_dwLevels = 0x4;
	m_lstSubChord.AddTail( pSubChord );

	pSubChord = new DMSubChord( iChordSelection.aChord[3], key );
	if( pSubChord == NULL )
	{
		return;
	}
	pSubChord->m_dwLevels = 0x8;
	m_lstSubChord.AddTail( pSubChord );
}

DMChord::~DMChord()
{
	if ( !m_lstSubChord.IsEmpty() )
	{
		DMSubChord	*pSubChord;
		while( !m_lstSubChord.IsEmpty() )
		{
			pSubChord = m_lstSubChord.RemoveHead();
			delete pSubChord;
		}
	}
}

HRESULT DMChord::Save( interface IDMUSProdRIFFStream* pIRiffStream )
{
	DWORD			cb;
	DWORD			dwSize;
	DMUS_IO_CHORD	ioDMChord;
	DMUS_IO_SUBCHORD	ioDMSubChord;
	HRESULT 		hr;
	POSITION		position;
	MMCKINFO ck;
	IStream *pIStream;

	ck.ckid = DMUS_FOURCC_CHORDTRACKBODY_CHUNK;
	hr = pIRiffStream->CreateChunk( &ck, 0 );
	if( hr != S_OK )
	{
		return hr;
	}

	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );
	if ( pIStream == NULL )
	{
		pIRiffStream->Ascend( &ck, 0 );
		return E_INVALIDARG;
	}

	dwSize = sizeof(DMUS_IO_CHORD);
	hr = pIStream->Write( &dwSize, sizeof(dwSize), &cb );
	if(FAILED(hr) || cb != sizeof(dwSize))
	{
		pIStream->Release();
		pIRiffStream->Ascend( &ck, 0 );
		return hr;
	}

	//MultiByteToWideChar( CP_ACP, 0, m_pChord->m_strName, -1, iChord.wszName, sizeof( iChord.wszName ) / sizeof( wchar_t ) );
	mbstowcs( ioDMChord.wszName, m_strName, 20);
	ioDMChord.mtTime = m_mtTime;
	ioDMChord.wMeasure = m_wMeasure;
	ioDMChord.bBeat = m_bBeat;

	hr = pIStream->Write( &ioDMChord, sizeof(DMUS_IO_CHORD), &cb );
	if(FAILED(hr) || cb != sizeof(DMUS_IO_CHORD))
	{
		pIStream->Release();
		pIRiffStream->Ascend( &ck, 0 );
		return hr;
	}

	dwSize = m_lstSubChord.GetCount();
	hr = pIStream->Write( &dwSize, sizeof(dwSize), &cb );
	if(FAILED(hr) || cb != sizeof(dwSize))
	{
		pIStream->Release();
		pIRiffStream->Ascend( &ck, 0 );
		return hr;
	}

	dwSize = sizeof(DMUS_IO_SUBCHORD);
	hr = pIStream->Write( &dwSize, sizeof(dwSize), &cb );
	if(FAILED(hr) || cb != sizeof(dwSize))
	{
		pIStream->Release();
		pIRiffStream->Ascend( &ck, 0 );
		return hr;
	}

	position = m_lstSubChord.GetHeadPosition();
	while(position != NULL)
	{
		ioDMSubChord = *m_lstSubChord.GetNext(position);

		hr = pIStream->Write( &ioDMSubChord, sizeof(DMUS_IO_SUBCHORD), &cb );
		if(FAILED(hr) || cb != sizeof(DMUS_IO_SUBCHORD))
		{
			pIStream->Release();
			pIRiffStream->Ascend( &ck, 0 );
			return hr;
		}
	}

	pIStream->Release();
	hr = pIRiffStream->Ascend( &ck, 0 );
	return hr;
}

