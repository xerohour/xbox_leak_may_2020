//      Copyright (c) 1996-1999 Microsoft Corporation
//	    PhoneyDS.h

#ifndef __PHONEY_DS__
#define __PHONEY_DS__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <math.h>
#include <mmsystem.h>
#include "dsoundp.h"
#include "dmusicc.h"
#include "dmusics.h"
#include "cclock.h"
#include "PLClock.h"
#include "dslink.h"
#include "audiosink.h"


class CPhoneyDSound : public IDirectSound8, public IDirectSoundPrivate 
{
public:
    // IUnknown methods
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IDirectSound methods
    STDMETHODIMP CreateSoundBuffer(LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter) ;
    STDMETHODIMP GetCaps(LPDSCAPS pDSCaps) ;
    STDMETHODIMP DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER *ppDSBufferDuplicate) ;
    STDMETHODIMP SetCooperativeLevel(HWND hwnd, DWORD dwLevel) ;
    STDMETHODIMP Compact() ;
    STDMETHODIMP GetSpeakerConfig(LPDWORD pdwSpeakerConfig) ;
    STDMETHODIMP SetSpeakerConfig(DWORD dwSpeakerConfig) ;
    STDMETHODIMP Initialize(LPCGUID pcGuidDevice) ;
    STDMETHODIMP VerifyCertification(LPDWORD pdwCertified) ;

    // IDirectSoundPrivate methods
    STDMETHODIMP AllocSink(LPWAVEFORMATEX pwfxFormat, LPDIRECTSOUNDCONNECT *ppSinkConnect) ;

	HRESULT GetFormat( LPWAVEFORMATEX pwfxFormat );
	HANDLE GetFileHandle( void );
	void SetFileHandle( HANDLE hFile );

    CPhoneyDSound();
    ~CPhoneyDSound();
private:
    long				m_cRef;
	bool				m_fFormatSet;
	WAVEFORMATEX		m_wfFormat;
	HANDLE				m_hFile;
    CRITICAL_SECTION	m_CriticalSection;	// Critical section to manage access.
};
#endif // __PHONEY_DS__
