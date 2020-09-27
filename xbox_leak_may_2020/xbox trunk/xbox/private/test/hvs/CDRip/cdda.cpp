#include "cdrip.h"
#include "cdda.h"

////////////////////////////////////////////////////////////////////////////

CCDDAStreamer::CCDDAStreamer( CNtIoctlCdromService* pDrive, DWORD dwRetries )
{
	ASSERT( pDrive != NULL );

	m_ibChunk = 0;
	m_pDrive = pDrive;
    m_dwRetries = dwRetries;
	m_dwCurFrame = 0;
    m_chunk = (BYTE*)XPhysicalAlloc( CDDA_BUFFER_SIZE, -1, 0, PAGE_READWRITE );
}

CCDDAStreamer::~CCDDAStreamer()
{
    if( m_chunk )
        XPhysicalFree( m_chunk );
}

int CCDDAStreamer::ReadFrames( void* pvBuffer, DWORD nFrameCount )
{
    DWORD nTotalFrames = m_pDrive->GetTrackFrame( m_pDrive->GetTrackCount() );
    nFrameCount = min( nFrameCount, nTotalFrames - m_dwCurFrame );
    ASSERT( (int)nFrameCount > 0 );

	HRESULT hr = m_pDrive->Read( m_dwCurFrame, nFrameCount, pvBuffer, m_dwRetries );
	if( FAILED(hr) )
	{
        ZeroMemory( pvBuffer, nFrameCount * CDAUDIO_BYTES_PER_FRAME );
        if( hr != HRESULT_FROM_WIN32( ERROR_INVALID_PARAMETER ) )
            return -1;
        else
            return 0;   // TODO: REVIEW: we probably hit end of the disc but need better way to detect this
	}

	m_dwCurFrame += nFrameCount;

	return nFrameCount * CDAUDIO_BYTES_PER_FRAME;
}

int CCDDAStreamer::Read( void* pvBuffer, int cbWanted )
{
    BYTE* pbBuffer = (BYTE*)pvBuffer;
    int cbRead;

    if( !m_chunk )
        return -1;

    if( m_ibChunk )
    {
        cbRead = min( cbWanted, CDDA_BUFFER_SIZE - m_ibChunk );

        CopyMemory( pbBuffer, m_chunk + m_ibChunk, cbRead );

        m_ibChunk += cbRead;
        m_ibChunk %= CDDA_BUFFER_SIZE;

        cbWanted -= cbRead;
        pbBuffer += cbRead;
    }

    while( cbWanted >= CDDA_MAX_FRAMES_PER_READ * CDAUDIO_BYTES_PER_FRAME )
    {
        cbRead = ReadFrames( pbBuffer, CDDA_MAX_FRAMES_PER_READ );
        if( cbRead <= 0 )
            return cbRead;

        cbWanted -= cbRead;
        pbBuffer += cbRead;
    }

    while( cbWanted >= CDDA_BUFFER_SIZE )
    {
        cbRead = ReadFrames( pbBuffer, CDDA_BUFFER_SIZE / CDAUDIO_BYTES_PER_FRAME );
        if( cbRead <= 0 )
            return cbRead;

        cbWanted -= cbRead;
        pbBuffer += cbRead;
    }

    if( cbWanted )
    {
        cbRead = ReadFrames( m_chunk, CDDA_BUFFER_SIZE / CDAUDIO_BYTES_PER_FRAME );
        if( cbRead <= 0 )
            return cbRead;

        if( cbRead < CDDA_BUFFER_SIZE )
            ZeroMemory( m_chunk + cbRead, CDDA_BUFFER_SIZE - cbRead );

        m_ibChunk = cbWanted;
        cbRead = min( cbRead, cbWanted );
        CopyMemory( pbBuffer, m_chunk, cbRead );

        cbWanted -= cbRead;
        pbBuffer += cbRead;
	}

	return pbBuffer - (LPBYTE)pvBuffer;
}
