//  MuteList.cpp

// This class loads mutes from an IMA 25 section file.
// It also will be able to write out a DirectMusic Mute track.

#include "stdafx.h"

#include "MuteList.h"
#include "FileStructs.h"
#include "RiffStructs.h"
#include <RiffStrm.h>
#include "Track.h"
#include <DMusProd.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define IMA_PPQ 192

CMuteList::CMuteList()
{
	m_pIFramework = NULL;
}

CMuteList::~CMuteList()
{
	RemoveAll();
}

HRESULT CMuteList::IMA_AddMute( IStream* pIStream, long lRecSize )
{
	DMUS_IO_MUTE	*pDMMute;
	ioIMAMute		imaMute;

	_LARGE_INTEGER	liTemp;

	HRESULT			hr;
	ULONG			ulBytesRead;

	if ( lRecSize < sizeof( ioIMAMute ) )
	{
		ZeroMemory( &imaMute, sizeof( ioIMAMute ) );
	}

	hr = pIStream->Read(&imaMute, lRecSize, &ulBytesRead);
	if(hr != S_OK || ulBytesRead != (ULONG) lRecSize)
	{
		if(FAILED(hr))
		{
			hr = E_FAIL;
		}
		return hr;
	}

	if ( lRecSize > sizeof( imaMute ) )
	{
		liTemp.LowPart = lRecSize - sizeof( ioIMAMute );
		liTemp.HighPart = 0;
    	pIStream->Seek(liTemp, STREAM_SEEK_CUR, NULL);
    }

	for( int i=0; i<16; i++ )
	{
		if( (1<<i) && imaMute.wMuteBits )
		{
			pDMMute = new DMUS_IO_MUTE;
			ASSERT( pDMMute != NULL );
			if ( pDMMute == NULL )
			{
				return E_OUTOFMEMORY;
			}

			pDMMute->mtTime = imaMute.lTime * DMUS_PPQ / IMA_PPQ;
			pDMMute->dwPChannel = i;
			pDMMute->dwPChannelMap = 0;

			m_lstDMMutes.AddTail( pDMMute );
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteList::CreateMuteTrack()

HRESULT CMuteList::CreateMuteTrack( class CTrack** ppTrack )
{
	if( m_lstDMMutes.IsEmpty() )
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

	hr = DM_SaveMuteList( pIRiffStream );
	pIRiffStream->Release();
	pIRiffStream = NULL;
	if ( FAILED( hr ) )
	{
		pStream->Release();
		delete pNewTrack;
		return hr;
	}

	LARGE_INTEGER	liTemp;
	liTemp.QuadPart = 0;
	pStream->Seek(liTemp, STREAM_SEEK_SET, NULL); //seek to beginning

	memcpy( &pNewTrack->m_guidClassID, &CLSID_DirectMusicMuteTrack, sizeof( GUID ) );
	// Let the Segment fill m_guidEditorID in.
	//memcpy( &pNewTrack->m_guidEditorID, &CLSID_MuteMgr, sizeof( GUID ) );
	//pNewTrack->dwPosition = 0;
	pNewTrack->m_dwGroupBits = 0x00000001;
	//pNewTrack->punkStripMgr = NULL;
	pNewTrack->m_ckid = DMUS_FOURCC_MUTE_CHUNK;
	//pNewTrack->m_fccType = 0;
	pNewTrack->SetStream( pStream );
	pStream->Release(); // Release our reference to the stream

	*ppTrack = pNewTrack;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteList::DM_SaveMuteList()

HRESULT CMuteList::DM_SaveMuteList( interface IDMUSProdRIFFStream* pIRIFFStream )
{
	ASSERT( pIRIFFStream != NULL );
	if ( pIRIFFStream == NULL )
	{
		return E_INVALIDARG;
	}

	MMCKINFO ck;
	HRESULT hr;
	ck.ckid = DMUS_FOURCC_MUTE_CHUNK;
	hr = pIRIFFStream->CreateChunk( &ck, 0 );
	if( hr != S_OK )
	{
		return hr;
	}

	IStream *pIStream;
    pIStream = pIRIFFStream->GetStream();
	ASSERT( pIStream != NULL );
	if ( pIStream == NULL )
	{
		pIRIFFStream->Ascend( &ck, 0 );
		return E_INVALIDARG;
	}

	DWORD			cb;
	DWORD			dwSize;
	DMUS_IO_MUTE	*pMute;
	POSITION		position;

    dwSize = sizeof(DMUS_IO_MUTE);
    hr = pIStream->Write(&dwSize, sizeof(dwSize), &cb);
    if(FAILED(hr) || cb != sizeof(dwSize))
    {
		pIRIFFStream->Ascend( &ck, 0 );
		pIStream->Release();
        return hr;
    }

	position = m_lstDMMutes.GetHeadPosition();
	while(position)
	{
		pMute = m_lstDMMutes.GetNext(position);

		if( FAILED(pIStream->Write( pMute, sizeof(DMUS_IO_MUTE), &cb ) ) || 
			cb != sizeof(DMUS_IO_MUTE) )
		{
			hr = E_FAIL;
			break;
		}
    }

	if( pIRIFFStream->Ascend( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
	}

	pIStream->Release();

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteList::DM_SaveMuteList()

void CMuteList::RemoveAll()
{
	if ( !m_lstDMMutes.IsEmpty() )
	{
		DMUS_IO_MUTE	*pMute;
		while( !m_lstDMMutes.IsEmpty() )
		{
			pMute = m_lstDMMutes.RemoveHead();
			delete pMute;
		}
	}
}
