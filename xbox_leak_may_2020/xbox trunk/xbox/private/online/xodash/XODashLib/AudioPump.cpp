//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
//-------------------------------------------------------------
#include "std.h"

#include "utilities.h"
#include "globals.h"
#include "AudioPump.h"
#include "DSoundManager.h"

 
////////////////////////////////////////////////////////////////////////////

CAudioBuf::CAudioBuf()
{
	m_pDSBuffer = NULL;
	m_bPaused = false;
	m_bLoop = false;
	m_nBufferBytes = 0;
}

CAudioBuf::~CAudioBuf()
{
	if (m_pDSBuffer != NULL)
	{
		m_pDSBuffer->Stop();
		m_pDSBuffer->Release();
	}
}

HRESULT CAudioBuf::Initialize(WAVEFORMATEX* pWaveFormat, int nBufferBytes, const void* pvSamples/*=NULL*/)
{
	m_nBufferBytes = nBufferBytes;
	m_nBytesPerSecond = pWaveFormat->nAvgBytesPerSec;
	HRESULT hr = S_OK;
	
	do
	{
		hr = DSoundManager::Instance()-> DSoundCreateSoundBuffer( pWaveFormat, nBufferBytes, DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY,&m_pDSBuffer);
		BREAKONFAIL(hr,"CAudioBuf::Initialize - fail to create sound buffer");
			
		if (m_pDSBuffer == NULL)
		{
			hr = E_INVALIDARG;
			BREAKONFAIL(hr,"CAudioBuf::Initialize - fail to create sound buffer");
		}

		if (pvSamples != NULL)
		{
			hr = DSoundManager::Instance() ->DSoundSetSoundBufferData(m_pDSBuffer, 0, nBufferBytes, pvSamples);
			BREAKONFAIL(hr, "CAudioBuf::Initialize - fail to set sound buffer");
		}

	}while(0);

	return hr;

}

void* CAudioBuf::Lock()
{
	LPVOID pvBuffer;
	DWORD dwBufferLength;

	ASSERT(m_pDSBuffer != NULL);

	VERIFYHR(m_pDSBuffer->Lock(0, m_nBufferBytes, &pvBuffer, &dwBufferLength, NULL, NULL, 0L));
	ASSERT(dwBufferLength == (DWORD)m_nBufferBytes);
	return pvBuffer;
}

void CAudioBuf::Unlock(void* pvBuffer)
{
	VERIFYHR(m_pDSBuffer->Unlock(pvBuffer, m_nBufferBytes, NULL, 0));
}

bool CAudioBuf::Play(bool bLoop/*=false*/)
{
	m_bPaused = false;
	m_bLoop = bLoop;

	if (m_pDSBuffer == NULL)
	{
		return false;
	}

	VERIFYHR(m_pDSBuffer->SetCurrentPosition(0));
	VERIFYHR(m_pDSBuffer->Play(0, 0, m_bLoop ? DSBPLAY_LOOPING : 0));

	return true;
}

void CAudioBuf::Stop()
{
	if (m_pDSBuffer != NULL)
    {
		VERIFYHR(m_pDSBuffer->Stop());
    }
}

void CAudioBuf::Pause(bool bPause)
{
	if (m_pDSBuffer == NULL)
	{
		return;
	}

	if (bPause)
	{
		m_bPaused = true;
		VERIFYHR(m_pDSBuffer->Stop());
	}
	else
	{
		m_bPaused = false;
		VERIFYHR(m_pDSBuffer->Play(0, 0, m_bLoop ? DSBPLAY_LOOPING : 0));
	}
}

bool CAudioBuf::IsPlaying()
{
	if (m_pDSBuffer == NULL)
	{
		return false;
	}

	DWORD dwStatus;
	VERIFYHR(m_pDSBuffer->GetStatus(&dwStatus));
	return (dwStatus & DSBSTATUS_PLAYING) != 0;
}

void CAudioBuf::SetAttenuation(float nAttenuation)
{
	ASSERT(m_pDSBuffer != NULL);

	VERIFYHR(m_pDSBuffer->SetVolume(-(int)(nAttenuation * 100.0f)));
}

void CAudioBuf::SetPan(float nPan)
{
	ASSERT(m_pDSBuffer != NULL);

#ifndef _XBOX
	VERIFYHR(m_pDSBuffer->SetPan((int)(nPan * 100.0f)));
#endif
}

void CAudioBuf::SetFrequency(float nFrequency)
{
	ASSERT(m_pDSBuffer != NULL);

	VERIFYHR(m_pDSBuffer->SetFrequency(nFrequency == 0.0f ? DSBFREQUENCY_ORIGINAL : (DWORD)nFrequency));
}

float CAudioBuf::GetPlaybackTime()
{
	if (m_pDSBuffer == NULL)
		return 0.0f;

	DWORD dwPlayCursor;
	VERIFYHR(m_pDSBuffer->GetCurrentPosition(&dwPlayCursor, NULL));

	return (float)dwPlayCursor / m_nBytesPerSecond;
}

float CAudioBuf::GetPlaybackLength()
{
	return (float)m_nBufferBytes / m_nBytesPerSecond;
}

void* CAudioBuf::GetSampleBuffer()
{
	return NULL;
}

DWORD CAudioBuf::GetSampleBufferSize()
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////

CAudioPump::CAudioPump()
{
	m_hPlayThread = NULL;
	m_hTerminate = NULL;
    m_hMutex = NULL;
	m_nBufferBytes = 0;
	m_nCompletedBuffers = 0;
    m_dwPrevCursor = 0;
    m_nFilledBuffers = 0;
    m_nPumpState = PUMPSTATE_STOPPED;

    m_ahNotify = NULL;
    m_nSegmentsPerBuffer = 0;
    m_pfBufferFilled = 0;
}

CAudioPump::~CAudioPump()
{
	if (m_hTerminate != NULL)
		SetEvent(m_hTerminate);

	if (m_hPlayThread != NULL)
	{
		WaitForSingleObject(m_hPlayThread, INFINITE);
		CloseHandle(m_hPlayThread);
	}
	
	if (m_hTerminate != NULL)
		CloseHandle(m_hTerminate);

	if (m_hMutex != NULL)
		CloseHandle(m_hMutex);

	if (m_pDSBuffer != NULL)
	{
		m_pDSBuffer->Stop();
		m_pDSBuffer->Release();
		m_pDSBuffer = NULL;
	}

    if(m_ahNotify){
	    for (int i = 0; i < m_nSegmentsPerBuffer; i++)
        {
            if (m_ahNotify[i])
                CloseHandle(m_ahNotify[i]);
        }
        delete [] m_ahNotify;
    }
    if(m_pfBufferFilled){
        delete [] m_pfBufferFilled;
    }
}

DWORD CALLBACK CAudioPump::StartThread(LPVOID pvContext)
{
	CAudioPump *pThis = (CAudioPump*)pvContext;
	return pThis->ThreadProc();
}

DWORD CAudioPump::ThreadProc()
{
    const HANDLE ahMutex[] = { m_hTerminate, m_hMutex };
    HANDLE* ahNotify = new HANDLE[1 + m_nSegmentsPerBuffer];
    if(!ahNotify){
        return -1;
    }
    int nBuffer;
    bool fMutex = false;
    DWORD dwWaitObj;

    ahNotify[0] = m_hTerminate;

    for (int i = 0; i < m_nSegmentsPerBuffer; i++)
    {
        ahNotify[i + 1] = m_ahNotify[i];
    }

	for (;;)
	{
		if (fMutex)
        {
            ReleaseMutex(m_hMutex);
            fMutex = false;
        }

        dwWaitObj = WaitForMultipleObjects(1 + m_nSegmentsPerBuffer, ahNotify, FALSE, INFINITE);
		if (dwWaitObj == WAIT_OBJECT_0)
        {
            break;
        }

        dwWaitObj = WaitForMultipleObjects(2, ahMutex, FALSE, INFINITE);
		if (dwWaitObj == WAIT_OBJECT_0)
        {
			break;
        }

        fMutex = true;

        if (m_nPumpState == PUMPSTATE_STOPPED)
        {
            continue;
        }

        if (m_nPumpState == PUMPSTATE_BUFFERING)
        {
            FillBuffer(m_nFilledBuffers);
            continue;
        }

        if ((m_nPumpState == PUMPSTATE_STOPPING) && (m_nFilledBuffers <= 0))
        {
            Stop();
			OnAudioEnd();  // check if smthng to be done at the end of the play, 
						   // eg. change track to the next one for the CD player
            continue;
        }

        for (int i = 0; i < m_nSegmentsPerBuffer; i++)
        {
            if (WAIT_OBJECT_0 == WaitForSingleObject(m_ahNotify[i], 0))
            {
                nBuffer = i;

                break;
            }
        }

        if (i >= m_nSegmentsPerBuffer)
        {
            continue;
        }

        // We start the ball rolling by signaled all events. This
        // wakes us enough times that we can fill up all our buffers.
        // But, a side effect is that we get a bunch of false
        // signals. When we are playing very short audio clips, there may
        // actually be more false signals than there is data to play.
        // To keep this case from confusing us, we need to keep track
        // of which buffers actually have data in them.
        //
        // The m_pfBufferFilled array keeps track of which buffers are
        // actually  filled. This allows us to distinguish the initial
        // false signals from the normal signals.

        if(m_pfBufferFilled[nBuffer]){
            m_pfBufferFilled[nBuffer] = false;
            m_nFilledBuffers--;
        }

		FillBuffer(nBuffer);
	}

    Stop();

    if (fMutex)
    {
        ReleaseMutex(m_hMutex);
    }

    if(ahNotify){
        delete [] ahNotify;
    }

	return 0;
}

HRESULT CAudioPump::Initialize(DWORD dwStackSize, WAVEFORMATEX* pWaveFormat, int nBufferBytes, int nSegmentsPerBuffer, int nPrebufferSegments)
{

	m_nBytesPerSecond = pWaveFormat->nAvgBytesPerSec;
	m_nBufferBytes = nBufferBytes;

    m_nSegmentsPerBuffer = nSegmentsPerBuffer;
    m_nPrebufferSegments = nPrebufferSegments;

    ASSERT((m_nPrebufferSegments <= m_nSegmentsPerBuffer));

    m_ahNotify = new HANDLE[m_nSegmentsPerBuffer];
    if(m_ahNotify == NULL)
	{
        return E_OUTOFMEMORY;
    }

    for (int i = 0; i < m_nSegmentsPerBuffer; i++)
    {
        m_ahNotify[i] = NULL;
    }

	m_hTerminate = CreateEvent(NULL, FALSE, FALSE, NULL);
	ASSERT(	m_hTerminate != INVALID_HANDLE_VALUE);
	if(m_hTerminate == INVALID_HANDLE_VALUE)
	{
		return ERROR_INVALID_HANDLE;
	}

    m_hMutex = CreateMutex(NULL, FALSE, NULL);
	ASSERT(m_hMutex != INVALID_HANDLE_VALUE );
	if(m_hMutex == INVALID_HANDLE_VALUE)
	{
		return ERROR_INVALID_HANDLE;
	}

    for (int i = 0; i < m_nSegmentsPerBuffer; i++)
    {
	    m_ahNotify[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		if(m_ahNotify[i] == INVALID_HANDLE_VALUE)
		{
// TBD release memory
			return E_OUTOFMEMORY;
		}
    }

    m_pfBufferFilled = new bool[m_nSegmentsPerBuffer];
    if(!m_pfBufferFilled){
        return E_OUTOFMEMORY;
    }

	DWORD dwThreadId;
	m_hPlayThread = CreateThread(NULL, dwStackSize, StartThread, this, 0, &dwThreadId);

	ASSERT(m_hPlayThread!= INVALID_HANDLE_VALUE);
	if(m_hPlayThread == INVALID_HANDLE_VALUE)
	{
		return ERROR_INVALID_HANDLE;
	}

	HRESULT hr = DSoundManager::Instance()->DSoundCreateSoundBuffer(pWaveFormat, m_nSegmentsPerBuffer * m_nBufferBytes, DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY, &m_pDSBuffer);
	
    if (m_pDSBuffer == NULL)
	{
		DbgPrint("CAudioPump::Initialize - fail to create sound buffer");
		return hr;
	}

	// BLOCK: Set notification positions
	{
		DSBPOSITIONNOTIFY* dsbpn = new DSBPOSITIONNOTIFY[m_nSegmentsPerBuffer];
        if(dsbpn == NULL){
            return E_OUTOFMEMORY;
        }

        for (i = 0; i < m_nSegmentsPerBuffer; i++)
        {
		    dsbpn[i].dwOffset = m_nBufferBytes * (i + 1) - pWaveFormat->nBlockAlign;
		    dsbpn[i].hEventNotify = m_ahNotify[i];
        }

		hr = DSoundManager::Instance()->DSoundSetSoundBufferNotify(m_pDSBuffer, m_nSegmentsPerBuffer, dsbpn);
		if (FAILED(hr))
		{
			DbgPrint("CAudioPump::Initialize - fail to set sound buffer notify");
		}
        delete [] dsbpn;
	}

	return hr;
}

bool CAudioPump::Play(bool bLoop/*=false*/)
{
    m_bLoop = bLoop;

	if (m_pDSBuffer == NULL)
		return false;

    WaitForSingleObject(m_hMutex, INFINITE);

    Stop();

    m_nPumpState = PUMPSTATE_BUFFERING;

    for (int i = 0; i < m_nSegmentsPerBuffer; i++)
    {
        SetEvent(m_ahNotify[i]);
        m_pfBufferFilled[i] = false;
    }


	for (int i = 0; i < m_nPrebufferSegments; i++)
    {
        FillBuffer(i);
    }


	VERIFYHR(m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING));

    ReleaseMutex(m_hMutex);

	return true;
}

void CAudioPump::Stop()
{
	if (m_pDSBuffer == NULL)
		return;

    WaitForSingleObject(m_hMutex, INFINITE);

	VERIFYHR(m_pDSBuffer->Stop());
	VERIFYHR(m_pDSBuffer->SetCurrentPosition(0));

    DirectSoundDoWork();

    for (int i = 0; i < m_nSegmentsPerBuffer; i++)
    {
        ResetEvent(m_ahNotify[i]);
    }

	m_nCompletedBuffers = 0;
    m_nFilledBuffers = 0;
    m_nPumpState = PUMPSTATE_STOPPED;

    ReleaseMutex(m_hMutex);
}

void CAudioPump::Pause(bool bPause)
{
	if (m_pDSBuffer == NULL)
		return;

    WaitForSingleObject(m_hMutex, INFINITE);

	if (bPause)
	{
		m_bPaused = true;
		VERIFYHR(m_pDSBuffer->Stop());
	}
	else
	{
		m_bPaused = false;
		VERIFYHR(m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING));
	}

    ReleaseMutex(m_hMutex);
}

bool CAudioPump::IsPlaying()
{
	return (PUMPSTATE_STOPPED != m_nPumpState);
}

bool CAudioPump::FillBuffer(int nBuffer)
{
	int nBytes;

    ASSERT(m_pDSBuffer != NULL);

	LPVOID pvBuffer;
	DWORD dwBufferLength;

    WaitForSingleObject(m_hMutex, INFINITE);

    ResetEvent(m_ahNotify[nBuffer]);

	VERIFYHR(m_pDSBuffer->Lock(nBuffer * m_nBufferBytes, m_nBufferBytes, &pvBuffer, &dwBufferLength, NULL, NULL, 0L));
	ASSERT(dwBufferLength == (DWORD)m_nBufferBytes);

	if (PUMPSTATE_STOPPING == m_nPumpState)
    {
        nBytes = 0;
    }
    else
    {
        nBytes = GetData((BYTE*)pvBuffer, m_nBufferBytes);
    }

	if (nBytes < m_nBufferBytes)
    {
        m_nPumpState = PUMPSTATE_STOPPING;

        if (nBytes > 0)
        {
		    ZeroMemory(((BYTE*)pvBuffer) + nBytes, m_nBufferBytes - nBytes);
        }
        else
        {
		    ZeroMemory(pvBuffer, m_nBufferBytes);
        }
    }

	VERIFYHR(m_pDSBuffer->Unlock(pvBuffer, dwBufferLength, NULL, 0));

    if (nBytes > 0)
    {
        m_pfBufferFilled[nBuffer] = true;
        m_nFilledBuffers++;

        if (PUMPSTATE_BUFFERING == m_nPumpState)
        {
            if(m_nFilledBuffers >= m_nSegmentsPerBuffer)
            {
                m_nPumpState = PUMPSTATE_RUNNING;
            }
        }
    }

    ReleaseMutex(m_hMutex);

	return nBytes > 0;
}

float CAudioPump::GetPlaybackTime()
{
	if (m_pDSBuffer == NULL)
		return 0.0f;

	DWORD dwPlayCursor;
	VERIFYHR(m_pDSBuffer->GetCurrentPosition(&dwPlayCursor, NULL));

    if( dwPlayCursor < m_dwPrevCursor )
    {
        m_nCompletedBuffers++;
    }

    m_dwPrevCursor = dwPlayCursor;

    return (float)(m_nCompletedBuffers * ( m_nBufferBytes * m_nSegmentsPerBuffer ) + dwPlayCursor) / m_nBytesPerSecond;
}

float CAudioPump::GetPlaybackLength()
{
	return 0.0f;
}

////////////////////////////////////////////////////////////////////////////

CFilePump::CFilePump()
{
	m_pvBuffer = NULL;
	m_hFile = INVALID_HANDLE_VALUE;
	m_dwStartPos = 0;
    m_dwBufferSize = 0;
}

CFilePump::~CFilePump()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
}

HRESULT CFilePump::Initialize(HANDLE hFile, int nFileBytes, WAVEFORMATEX* pFormat)
{
	HRESULT hr = S_OK;
    m_dwBufferSize = (0x2000/pFormat->nBlockAlign) * pFormat->nBlockAlign;

	hr = CAudioPump::Initialize(8192, pFormat, m_dwBufferSize);
	if (FAILED(hr))
	{
		DbgPrint("CFilePump::Initialize - fail to init CAudioPump");
		return hr;
	}

	m_hFile = hFile;
	m_nPlaybackLength = (float)nFileBytes / m_nBytesPerSecond;
	m_dwStartPos = SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);

	return hr;
}

void CFilePump::Stop()
{
	if (m_hFile == INVALID_HANDLE_VALUE)
		return;

	CAudioPump::Stop();

	SetFilePointer(m_hFile, m_dwStartPos, NULL, FILE_BEGIN);
}

float CFilePump::GetPlaybackLength()
{
	return m_nPlaybackLength;
}

void* CFilePump::GetSampleBuffer()
{
	return m_pvBuffer;
}

DWORD CFilePump::GetSampleBufferSize()
{
	return m_dwBufferSize;
}

int CFilePump::GetData(BYTE* pbBuffer, int cbBuffer)
{
	DWORD dwRead;

	if (!ReadFile(m_hFile, pbBuffer, cbBuffer, &dwRead, NULL))
	{
		DbgPrint("CFilePump::GetData ReadFile failed (%d)\n", GetLastError());
		return -1;
	}

	if (m_bLoop && dwRead != (DWORD)cbBuffer)
	{
//		DbgPrint("AutoLooping CFilePump...\n");

		SetFilePointer(m_hFile, m_dwStartPos, NULL, FILE_BEGIN);

		DWORD dwRead2;
		if (!ReadFile(m_hFile, pbBuffer + dwRead, cbBuffer - dwRead, &dwRead2, NULL))
		{
			DbgPrint("CFilePump::GetData ReadFile failed (%d)\n", GetLastError());
			return -1;
		}

		dwRead += dwRead2;
	}

	m_pvBuffer = pbBuffer;

	return (int)dwRead;
}

////////////////////////////////////////////////////////////////////////////

const DWORD WMA_SEGMENTS_PER_BUFFER=4;

const DWORD WMASTRM_LOOKAHEAD_SIZE = 0x8000 * WMA_SEGMENTS_PER_BUFFER;
const DWORD WMASTRM_SOURCE_PACKET_BYTES = 0x8000;

CWMAPump::CWMAPump()
: m_nPlaybackLengthInSeconds( 0 ),
m_pSourceFilter( NULL ),
m_pvBuffer( NULL ),
m_hFile( INVALID_HANDLE_VALUE )
{
}

CWMAPump::~CWMAPump()
{
	if (m_pSourceFilter != NULL)
	{
        SignalObjectAndWait(m_hTerminate, m_hPlayThread, INFINITE, FALSE);
		m_pSourceFilter->Release();
		m_pSourceFilter = NULL;
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

HRESULT CWMAPump::Initialize(DWORD dwSongID, WAVEFORMATEX* pFormat)
{
    HANDLE hFile = XOpenSoundtrackSong(dwSongID, FALSE);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DbgPrint("Cannot open song 0x%08x\n", dwSongID);
        return HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
    }

    // HACK: There is no way to use a SongID to get the Length of the Song
    // You must know the "index" within the SoundTrack of the song in order to get it's info
    // Because of this, we will loop on all the Songs in the current Soundtrack until we find
    // the correct one, and then store it's Length
    unsigned int uiSongIndex = 0;
    DWORD dwReturnedSongID;
    DWORD dwReturnedSongLength;

    while( TRUE )// Since we are using a SongID that is valid, this should be ok
    {
        if( !XGetSoundtrackSongInfo( DWORD HIWORD( dwSongID),
                                     uiSongIndex,
                                     &dwReturnedSongID,
                                     &dwReturnedSongLength,
                                     NULL,
                                     0 ) )
        {
            break;  // If this call fails, we should break out of our loop.
        }

        // Check to see if we found the song we are looking for
        if( dwSongID == dwReturnedSongID )
        {
            m_nPlaybackLengthInSeconds = (float)dwReturnedSongLength / 1000.0f;

            break;  // Since we found our song, we can stop searching
        };

        uiSongIndex++;
    }

    return Initialize(NULL, hFile, pFormat);
}

HRESULT CWMAPump::Initialize(const TCHAR* szFileName, HANDLE hFile, WAVEFORMATEX* pFormat)
{
	ASSERT(szFileName == NULL || hFile == NULL);
	ASSERT(szFileName != NULL || hFile != NULL);

	ASSERT(m_hFile == INVALID_HANDLE_VALUE);

	if (szFileName == NULL)
		m_hFile = hFile;

	char szBuf [MAX_PATH];
	if (szFileName != NULL)
		Ansi(szBuf, szFileName, MAX_PATH);

	HRESULT hr = WmaCreateDecoder(szBuf, hFile, FALSE, WMASTRM_LOOKAHEAD_SIZE, m_nSegmentsPerBuffer, 0, pFormat, &m_pSourceFilter);
	if (hr != S_OK)
	{
#ifdef _DEBUG
    	LogComError(hr, "WmaCreateDecoder");
#endif
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;

		return hr;
	}

	hr = CAudioPump::Initialize(0, pFormat, WMASTRM_SOURCE_PACKET_BYTES, WMA_SEGMENTS_PER_BUFFER);
	if (FAILED(hr))
	{
		m_pSourceFilter->Release();
		m_pSourceFilter = NULL;

		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;

		return hr;
	}

    m_pDSBuffer->SetHeadroom(0);

	return hr;
}

int CWMAPump::GetData(BYTE* pbBuffer, int cbBuffer)
{
	ASSERT(cbBuffer == WMASTRM_SOURCE_PACKET_BYTES);

    DWORD dwSourceUsed;
    XMEDIAPACKET xmp;
    ZeroMemory(&xmp, sizeof(xmp));
    xmp.pvBuffer = pbBuffer;
    xmp.dwMaxSize = cbBuffer;
    xmp.pdwCompletedSize = &dwSourceUsed;

    HRESULT hr = m_pSourceFilter->Process(NULL, &xmp);
    if (FAILED(hr))
	{
		DbgPrint("\001m_pSourceFilter->Process() failed!\n");
        return -1;
	}

    if (dwSourceUsed < xmp.dwMaxSize)
	{
		DbgPrint("WMA PLAYBACK IS DONE!\n");

        // Call Flush so that the position is reset to begining
        m_pSourceFilter->Flush();
		return 0;
	}

	m_pvBuffer = pbBuffer;

	return WMASTRM_SOURCE_PACKET_BYTES;
}

float CWMAPump::GetPlaybackLength()
{
    return m_nPlaybackLengthInSeconds;
}

void* CWMAPump::GetSampleBuffer()
{
	return m_pvBuffer;
}

DWORD CWMAPump::GetSampleBufferSize()
{
	return WMASTRM_SOURCE_PACKET_BYTES;
}

void CWMAPump::Stop()
{
    CAudioPump::Stop();
}



