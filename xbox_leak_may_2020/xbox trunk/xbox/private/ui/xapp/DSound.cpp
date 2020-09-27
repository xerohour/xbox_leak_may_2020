#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Camera.h"

#ifndef DSMIXBIN_VALID
#define DSMIXBIN_VALID 0x7FFFFFFF
#endif

extern "C" extern DWORD g_dwDirectSoundOverrideSpeakerConfig;

#ifdef _WINDOWS
#pragma comment(lib, "dxguid.lib")
#define _D3DTYPES_H_
#pragma comment(lib, "dsound.lib")
#endif

#ifdef _XBOX
#include <initguid.h>
#endif

#include <dsound.h>

#include "Sound.h"

static bool DSound_Init();

//
// BUGBUG: copied from disc.cpp
//
#define DISC_VIDEO      4
extern int g_nDiscType;

LPDIRECTSOUND g_pDirectSound = NULL;
HANDLE g_hDirectSoundThread = NULL;

DWORD WINAPI DirectSoundThreadProc(LPVOID pvParameter)
{
    while(g_pDirectSound)
    {
        DirectSoundDoWork();
        Sleep(100);
    }

    return 0;
}

LPDIRECTSOUNDBUFFER XAppCreateSoundBuffer(WAVEFORMATEX* pwfx, int nByteCount, DWORD dwFlags)
{
	if (g_pDirectSound == NULL && !DSound_Init())
		return NULL;

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

	HRESULT hr = g_pDirectSound->CreateSoundBuffer(&dsbd, &lpDirectSoundBuffer, NULL);

#ifdef _XBOX

    if(SUCCEEDED(hr))
    {
        lpDirectSoundBuffer->SetHeadroom(1200);
    }

#endif
	
	return lpDirectSoundBuffer;
}

void XAppSetSoundBufferData(LPDIRECTSOUNDBUFFER pDirectSoundBuffer, UINT nByteOffset, UINT nByteCount, const void* pvData)
{
	LPVOID pbBuffer;
	DWORD dwBufferLength;

	ASSERT(pDirectSoundBuffer != NULL);

	VERIFYHR(pDirectSoundBuffer->Lock(nByteOffset, nByteCount, &pbBuffer, &dwBufferLength, NULL, NULL, 0L));

	CopyMemory(pbBuffer, pvData, nByteCount);

	VERIFYHR(pDirectSoundBuffer->Unlock(pbBuffer, dwBufferLength, NULL, 0));
}

void XAppSetSoundBufferNotify(LPDIRECTSOUNDBUFFER pDirectSoundBuffer, int nPositionCount, DSBPOSITIONNOTIFY* positions)
{
#ifdef _XBOX
	pDirectSoundBuffer->SetNotificationPositions(nPositionCount, positions);
#else
	LPDIRECTSOUNDNOTIFY pDSNotify;
	VERIFYHR(pDirectSoundBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&pDSNotify));
	VERIFYHR(pDSNotify->SetNotificationPositions(nPositionCount, positions));
	VERIFYHR(pDSNotify->Release());
#endif
}

static bool DSound_Init()
{
	HRESULT hr;

	TRACE(_T("Initializing DirectSound...\n"));
	ASSERT(g_pDirectSound == NULL);

	//
	// BUGBUG: remove this hack to prevent dsound from initializing when we're playing a DVD
	//
	if (DISC_VIDEO == g_nDiscType)
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

	if (FAILED(hr = DirectSoundCreate(NULL, &g_pDirectSound, NULL)))
	{
		LogComError(hr, "DSound_Init: DirectSoundCreate");
		return false;
	}

    if (FAILED(hr = g_pDirectSound->SetMixBinHeadroom(DSMIXBIN_VALID, 0)))
    {
		LogComError(hr, "DSound_Init: SetMixBinHeadroom");
	}

#ifdef _XBOX

    DWORD dwThreadId;
    g_hDirectSoundThread = CreateThread(NULL, 0, DirectSoundThreadProc, NULL, 0, &dwThreadId);
    if (!g_hDirectSoundThread)
    {
		LogComError(hr, "DSound_Init: failed to create DirectSound worker thread");
		return false;
	}

#else

	ASSERT(theApp.m_hWnd != NULL);
	if (FAILED(hr = g_pDirectSound->SetCooperativeLevel(theApp.m_hWnd, DSSCL_PRIORITY)))
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
	if (FAILED(hr = g_pDirectSound->CreateSoundBuffer(&dsbdesc, &pDSBPrimary, NULL)))
	{
		TRACE(_T("\002CreateSoundBuffer with DSBCAPS_GLOBALFOCUS failed, trying without...\n"));
		dsbdesc.dwFlags &= ~DSBCAPS_GLOBALFOCUS;
		if (FAILED(hr = g_pDirectSound->CreateSoundBuffer(&dsbdesc, &pDSBPrimary, NULL)))
		{
			LogComError(hr, "DSound_Init: CreateSoundBuffer");
			return false;
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

	TRACE(_T("SetFormat..."));
	if (FAILED(hr = pDSBPrimary->SetFormat(&wfx)))
		LogComError(hr, "DSound_Init: SetFormat");

	pDSBPrimary->Release();
#endif

	return true;
}

void DSound_Exit()
{
	if (g_pDirectSound != NULL)
    {
		g_pDirectSound->Release();
        g_pDirectSound = NULL;
    }

    if (g_hDirectSoundThread != NULL)
    {
        CloseHandle(g_hDirectSoundThread);
    }
}

void DSound_Frame()
{
}
