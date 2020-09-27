#include "std.h"
#include "utilities.h"
#include "DSoundManager.h"
#include "globals.h"
#include "XOConst.h"


DSoundManager* DSoundManager::pinstance = 0;// initialize pointer
  
DSoundManager* DSoundManager::Instance () 
{
    if (pinstance == 0)  // is it the first call?
    {  
      pinstance = new DSoundManager; 
    }
    return pinstance; // address of sole instance
}
  
DSoundManager::DSoundManager() : m_pDirectSound(NULL), m_hDirectSoundThread(NULL), m_bShutdown(false)
{ 

}

static DWORD WINAPI DirectSoundThreadProc(LPVOID pvParameter)
{
	DSoundManager *pThis = static_cast<DSoundManager*> (pvParameter);
	
    while(!pThis->m_bShutdown)
    {
        DirectSoundDoWork();
        Sleep(1000);
    }

    return 0;
}


HRESULT DSoundManager::Initialize()
{
	HRESULT hr = S_OK;

	DbgPrint("Initializing DirectSound...\n");
	if(m_pDirectSound != NULL)
	{
		return S_FALSE;
	}

	//
	// BUGBUG: remove this hack to prevent dsound from initializing when we're playing a DVD
	//
	if (eDiscVideo == g_nDiscType)
	{
		//LogComError(E_FAIL, "DSound_Init: Not initializing with DVD-Video disc in drive");
		return false;
	}

#ifdef _XBOX

    DWORD dwSpeakerConfig = XAudioGetSpeakerConfig();

    if(DSSPEAKER_MONO == DSSPEAKER_BASIC(dwSpeakerConfig))
    {
        g_dwDirectSoundOverrideSpeakerConfig = DSSPEAKER_COMBINED(DSSPEAKER_MONO, 0);
    }
    else
    {
        g_dwDirectSoundOverrideSpeakerConfig = DSSPEAKER_COMBINED(DSSPEAKER_STEREO, 0);
    }

#endif

	if (FAILED(hr = DirectSoundCreate(NULL, &m_pDirectSound, NULL)))
	{
		LogComError(hr, "DSound_Init: DirectSoundCreate");
		return hr;
	}

    if (FAILED(hr = m_pDirectSound->SetMixBinHeadroom(NULL, 0)))
    {
		LogComError(hr, "DSound_Init: SetMixBinHeadroom");
	}

#ifdef _XBOX

    DWORD dwThreadId;
    m_hDirectSoundThread = CreateThread(NULL, 0, DirectSoundThreadProc, this, 0, &dwThreadId);
    if (!m_hDirectSoundThread)
    {
		hr = HRESULT_FROM_WIN32(GetLastError());
		LogComError(hr, "DSound_Init: failed to create DirectSound worker thread");
		return hr;
	}

#else

	ASSERT(theApp.m_hWnd != NULL);
	if (FAILED(hr = m_pDirectSound->SetCooperativeLevel(theApp.m_hWnd, DSSCL_PRIORITY)))
	{
		LogComError(hr, "DSound_Init: SetCooperativeLevel");
		// Not fatal!
	}

	DSBUFFERDESC dsbdesc;
	dsbdesc.dwSize = sizeof (dsbdesc);
	dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_GLOBALFOCUS;
	dsbdesc.dwBufferBytes = 0;
	dsbdesc.dwReserved = 0;
	dsbdesc.lpwfxFormat = NULL;
	dsbdesc.guid3DAlgorithm = GUID_NULL;

	LPDIRECTSOUNDBUFFER pDSBPrimary;
	if (FAILED(hr = m_pDirectSound->CreateSoundBuffer(&dsbdesc, &pDSBPrimary, NULL)))
	{
		DbgPrint("\002CreateSoundBuffer with DSBCAPS_GLOBALFOCUS failed, trying without...\n");
		dsbdesc.dwFlags &= ~DSBCAPS_GLOBALFOCUS;
		if (FAILED(hr = m_pDirectSound->CreateSoundBuffer(&dsbdesc, &pDSBPrimary, NULL)))
		{
			LogComError(hr, "DSound_Init: CreateSoundBuffer");
			return hr;
		}
	}

	WAVEFORMATEX wfx;
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	DbgPrint("SetFormat...");
	if (FAILED(hr = pDSBPrimary->SetFormat(&wfx)))
		LogComError(hr, "DSound_Init: SetFormat");

	pDSBPrimary->Release();
#endif

	return hr;
}

HRESULT DSoundManager::Cleanup()
{
	m_bShutdown = true;  // close the Direct sound thread
	if (m_pDirectSound != NULL)
    {
		m_pDirectSound->Release();
        m_pDirectSound = NULL;
    }

    if (m_hDirectSoundThread != NULL)
    {
        CloseHandle(m_hDirectSoundThread);
    }
	return S_OK;
}

DSoundManager::~DSoundManager()
{
	Cleanup();
}

HRESULT DSoundManager::DSoundCreateSoundBuffer(IN WAVEFORMATEX* pwfx, IN int nByteCount, IN DWORD dwFlags, OUT LPDIRECTSOUNDBUFFER* pDirectBuf)
{
    HRESULT hr = S_OK;
	*pDirectBuf = NULL;
	if (!m_pDirectSound == NULL)
	{
		hr = Initialize();	
	}

	if(FAILED(hr))
	{
		DbgPrint("DSoundManager::DSoundCreateSoundBuffer - fail to Init DSoundManager");
		return hr;
	}

	LPDIRECTSOUNDBUFFER lpDirectSoundBuffer = NULL;
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof (DSBUFFERDESC));
	dsbd.dwSize = sizeof (DSBUFFERDESC);

	dsbd.dwFlags = dwFlags;
#ifndef _XBOX
	dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
#endif
	dsbd.dwBufferBytes = nByteCount;
	dsbd.lpwfxFormat = pwfx;

    hr = m_pDirectSound->CreateSoundBuffer(&dsbd, &lpDirectSoundBuffer, NULL);

#ifdef _XBOX

    if(SUCCEEDED(hr))
    {
        lpDirectSoundBuffer->SetHeadroom(1200);
    }

#endif
	
	*pDirectBuf = lpDirectSoundBuffer;
	return hr;
}

HRESULT DSoundManager::DSoundSetSoundBufferData(LPDIRECTSOUNDBUFFER pDirectSoundBuffer, UINT nByteOffset, UINT nByteCount, const void* pvData)
{
	LPVOID pbBuffer;
	DWORD dwBufferLength;

	ASSERT(pDirectSoundBuffer != NULL);

	HRESULT hr = pDirectSoundBuffer->Lock(nByteOffset, nByteCount, &pbBuffer, &dwBufferLength, NULL, NULL, 0L);
	if (FAILED(hr))
	{
		DbgPrint("DSoundManager::DSoundSetSoundBufferData - Fail to locksound buffer");
		return hr;
	}

	CopyMemory(pbBuffer, pvData, nByteCount);

	hr = pDirectSoundBuffer->Unlock(pbBuffer, dwBufferLength, NULL, 0);
	if (FAILED(hr))
	{
		DbgPrint("DSoundManager::DSoundSetSoundBufferData - Fail to unlock sound buffer");
		return hr;
	}
	return hr;
}


HRESULT DSoundManager::DSoundSetSoundBufferNotify(LPDIRECTSOUNDBUFFER pDirectSoundBuffer, int nPositionCount, DSBPOSITIONNOTIFY* positions)
{

// #pragma _TODO_ (Handle Non-XBox errors)
	HRESULT hr;
#ifdef _XBOX
	hr = pDirectSoundBuffer->SetNotificationPositions(nPositionCount, positions);
#else
	LPDIRECTSOUNDNOTIFY pDSNotify;
	hr = pDirectSoundBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&pDSNotify));
	hr = pDSNotify->SetNotificationPositions(nPositionCount, positions));
	hr = pDSNotify->Release();
#endif
	return hr;
}
