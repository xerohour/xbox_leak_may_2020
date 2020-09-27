// Track.cpp : implementation file
//

#include "stdafx.h"

#include "track.h"
#include <winbase.h>
#include "StripMgr.h"
#include "SegmentIO.h"

CTrack::CTrack()
{
	ZeroMemory( &m_guidClassID, sizeof( GUID ) );
	ZeroMemory( &m_guidEditorID, sizeof( GUID ) );
	m_pIStream = NULL;
	m_pIStripMgr = NULL;
	m_pIDMTrack = NULL;
	m_dwGroupBits = 1;
	m_dwPosition = 0;
	m_ckid = 0;
	m_fccType = 0;
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT;
	m_dwTrackExtrasPriority = 0;
	m_dwProducerOnlyFlags = 0;
}

CTrack::~CTrack()
{
	if ( m_pIStream != NULL )
	{
		m_pIStream->Release();
		m_pIStream = NULL;
	}
	if ( m_pIStripMgr != NULL )
	{
		m_pIStripMgr->Release();
		m_pIStripMgr = NULL;
	}
	if ( m_pIDMTrack != NULL )
	{
		m_pIDMTrack->Release();
		m_pIDMTrack = NULL;
	}
}

void CTrack::SetStream( IStream* pIStream )
{
	if ( m_pIStream != NULL )
	{
		m_pIStream->Release();
	}

	if ( pIStream != NULL )
	{
		pIStream->Clone( &m_pIStream );

		LARGE_INTEGER	liTemp;
		liTemp.QuadPart = 0;
		m_pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL); //seek to beginning
	}
	else
	{
		m_pIStream = NULL;
	}
}

void CTrack::GetStream( IStream** ppIStream )
{
	if ( ppIStream == NULL )
	{
		return;
	}

	if ( m_pIStream != NULL )
	{
		m_pIStream->Clone( ppIStream );
		LARGE_INTEGER	liTemp;
		liTemp.QuadPart = 0;
		(*ppIStream)->Seek(liTemp, STREAM_SEEK_SET, NULL); //seek to beginning
	}
	else
	{
		*ppIStream = NULL;
	}
}

void CTrack::SetDMTrack( IDirectMusicTrack* pIDMTrack )
{
	if ( m_pIDMTrack != NULL )
	{
		m_pIDMTrack->Release();
	}

	if ( pIDMTrack != NULL )
	{
		m_pIDMTrack = pIDMTrack;
		m_pIDMTrack->AddRef();
	}
	else
	{
		m_pIDMTrack = NULL;
	}
}

void CTrack::GetDMTrack( IDirectMusicTrack** ppIDMTrack )
{
	if ( ppIDMTrack == NULL )
	{
		return;
	}

	if ( m_pIDMTrack != NULL )
	{
		*ppIDMTrack = m_pIDMTrack;
		(*ppIDMTrack)->AddRef();
	}
	else
	{
		*ppIDMTrack = NULL;
	}
}

void CTrack::SetStripMgr( IDMUSProdStripMgr* pIStripMgr )
{
	if ( m_pIStripMgr != NULL )
	{
		m_pIStripMgr->Release();
	}

	m_pIStripMgr = pIStripMgr;

	if ( pIStripMgr != NULL )
	{
		m_pIStripMgr->AddRef();
	}
}

void CTrack::GetStripMgr( IDMUSProdStripMgr** ppIStripMgr )
{
	if ( ppIStripMgr == NULL )
	{
		return;
	}

	*ppIStripMgr = m_pIStripMgr;
	if ( m_pIStripMgr )
	{
		(*ppIStripMgr)->AddRef();
	}
}

void CTrack::FillTrackHeader( DMUS_IO_TRACK_HEADER *pTrackHeader )
{
	ASSERT( pTrackHeader );
	if( pTrackHeader == NULL )
	{
		return;
	}

	memcpy( &pTrackHeader->guidClassID, &m_guidClassID, sizeof(GUID) );
	pTrackHeader->dwPosition = m_dwPosition;
	pTrackHeader->dwGroup = m_dwGroupBits;

	if( m_pIStripMgr )
	{
		DMUS_IO_TRACK_HEADER ioTrackHeader;
		ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
		VARIANT varTrackHeader;
		varTrackHeader.vt = VT_BYREF;
		V_BYREF(&varTrackHeader) = &ioTrackHeader;

		if( FAILED( m_pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
		{
			TRACE("CTrack: Failed to set get StripMgr's TrackHeader\n");
		}
		else
		{
			m_ckid = ioTrackHeader.ckid;
			m_fccType = ioTrackHeader.fccType;
		}
	}
	pTrackHeader->ckid = m_ckid;
	pTrackHeader->fccType = m_fccType;
}

void CTrack::FillTrackExtrasHeader( DMUS_IO_TRACK_EXTRAS_HEADER *pTrackExtrasHeader )
{
	ASSERT( pTrackExtrasHeader );
	if( pTrackExtrasHeader == NULL )
	{
		return;
	}

	pTrackExtrasHeader->dwFlags = m_dwTrackExtrasFlags;
	pTrackExtrasHeader->dwPriority = m_dwTrackExtrasPriority;
}

void CTrack::FillProducerOnlyChunk( _IOProducerOnlyChunk *pProducerOnlyChunk )
{
	ASSERT( pProducerOnlyChunk );
	if( pProducerOnlyChunk == NULL )
	{
		return;
	}

	pProducerOnlyChunk->dwProducerOnlyFlags = m_dwProducerOnlyFlags;
}
