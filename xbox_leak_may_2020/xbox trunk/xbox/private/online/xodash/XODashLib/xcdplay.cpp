#include "std.h"
#include "utilities.h"
#include "xcdplay.h"
#include "ntiosvc.h"

extern CNtIoctlCdromService g_cdrom; 

XCDPlayer::XCDPlayer() :
	m_streamer(&g_cdrom)
{
	m_dwStartPosition = 0;
	m_dwStopPosition = 0;
    m_pvBuffer = NULL;
}

XCDPlayer::~XCDPlayer()
{
	if (m_hPlayThread != NULL)
	{
		if (m_hTerminate != NULL)
			SetEvent(m_hTerminate);

		WaitForSingleObject(m_hPlayThread, INFINITE);
	}
}

HRESULT XCDPlayer::Initialize(int nTrack, WAVEFORMATEX* pFormat)
{
	HRESULT hr = S_OK;
	if (!SelectTrack(nTrack) )
	{
		return E_INVALIDARG;
	}
	ZeroMemory(pFormat, sizeof (WAVEFORMATEX));
	pFormat->wFormatTag = WAVE_FORMAT_PCM;
	pFormat->nChannels = 2;
	pFormat->nSamplesPerSec = 44100;
	pFormat->wBitsPerSample = 16;
	pFormat->nBlockAlign = pFormat->wBitsPerSample / 8 * pFormat->nChannels;
	pFormat->nAvgBytesPerSec = pFormat->nSamplesPerSec * pFormat->nBlockAlign;

	hr  = CAudioPump::Initialize(0, pFormat, BYTES_PER_CHUNK, CD_AUDIO_SEGMENTS_PER_BUFFER);
	if(FAILED(hr))
	{
		DbgPrint("XCDPlayer::Initialize - fail to init AudioPump");
		return hr;
	}
    m_pDSBuffer->SetHeadroom(0);

	return hr;
}

// NOTE: This is called from a secondary thread!
int XCDPlayer::GetData(BYTE* pbBuffer, int cbBuffer)
{
    const DWORD dwPosition = GetPosition();
    
    if (dwPosition >= m_dwStopPosition)
		return 0; // all done!

    if (dwPosition + ((cbBuffer + CDAUDIO_BYTES_PER_FRAME - 1) / CDAUDIO_BYTES_PER_FRAME) >= m_dwStopPosition)
        cbBuffer = (m_dwStopPosition - dwPosition) * CDAUDIO_BYTES_PER_FRAME;

	int nRead = m_streamer.Read(pbBuffer, cbBuffer);
    if (nRead <= 0)
        return nRead;

    m_pvBuffer = pbBuffer;

    return nRead;
}


/*
void XCDPlayer::Seek(BOOL fForward)
{
	DWORD dwCurPosition;
	dwCurPosition = GetPosition();
	if (fForward)
	{
		dwCurPosition += CDAUDIO_FRAMES_PER_SECOND;

		DWORD dwLastFrame = g_cdrom.GetTrackFrame(g_cdrom.GetTrackCount());
		if (dwCurPosition >= dwLastFrame)
		{
			dwCurPosition -= dwLastFrame - g_cdrom.GetTrackFrame(0);
		}
	}
	else
	{
		dwCurPosition -= CDAUDIO_FRAMES_PER_SECOND;

		if (dwCurPosition < g_cdrom.GetTrackFrame(0))
		{
			dwCurPosition += g_cdrom.GetTrackFrame(g_cdrom.GetTrackCount()) - g_cdrom.GetTrackFrame(0);
		}
	}

	SetPosition(dwCurPosition);
}
*/

void XCDPlayer::Stop()
{
    CAudioPump::Stop();
    m_streamer.SetFrame(m_dwStartPosition);
}

float XCDPlayer::GetPlaybackLength()
{
	DWORD dwLength = m_dwStopPosition - m_dwStartPosition;
	return (float)dwLength / CDAUDIO_FRAMES_PER_SECOND;
}

float XCDPlayer::GetPlaybackTime()
{
    // Keep the UI from displaying playback time past the end of the song, even if we
    // play a little silence after the song.
    float playbackTime = CAudioPump::GetPlaybackTime();
    float length = GetPlaybackLength();
    if(playbackTime > length){
        playbackTime = length;
    }
    return playbackTime;
}

void* XCDPlayer::GetSampleBuffer()
{
	return m_pvBuffer;
}

DWORD XCDPlayer::GetSampleBufferSize()
{
	return BYTES_PER_CHUNK;
}


void XCDPlayer::SetPosition(DWORD dwPosition)
{
	WaitForSingleObject(m_hMutex, INFINITE);
	m_streamer.SetFrame(dwPosition);
	ReleaseMutex(m_hMutex);
}

DWORD XCDPlayer::GetPosition()
{
	WaitForSingleObject(m_hMutex, INFINITE);
	DWORD dw = m_streamer.GetFrame();
	ReleaseMutex(m_hMutex);
	return dw;
}

bool XCDPlayer::SelectTrack(int nTrack)
{
	m_nCurrentTrack = nTrack;
	
	DbgPrint("XCDPlayer::Initialize()\n");

	if (!g_cdrom.IsOpen())
	{
		DbgPrint("Cannot open CD player; g_cdrom is not open!\n");
		return false;
	}

	// BLOCK: Select track
	{
		m_nTotalTracks = g_cdrom.GetTrackCount();
		ASSERT(m_nTotalTracks > 0);

		if (nTrack < 0 || nTrack >= m_nTotalTracks)
        {
			DbgPrint("XCDPlayer::Initialize invalid track %d\n", nTrack);
			return false;
		}

		m_dwStartPosition = g_cdrom.GetTrackFrame(nTrack);
		m_dwStopPosition = g_cdrom.GetTrackFrame(nTrack + 1);

		m_streamer.SetFrame(m_dwStartPosition);
	}
	return true;
}

void XCDPlayer::OnAudioEnd()
{
	m_nCurrentTrack++;
	if (m_nCurrentTrack >= m_nTotalTracks)
	{
		m_nCurrentTrack = 0;
	}
	else 
	{
		if (SelectTrack(m_nCurrentTrack))
		{
			CAudioPump::Play();
		}

	}
}