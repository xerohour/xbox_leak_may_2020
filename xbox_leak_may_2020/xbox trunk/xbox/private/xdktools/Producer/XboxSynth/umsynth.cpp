//
// Copyright (c) 1996-2001 Microsoft Corporation
// UMSynth.cpp : Implementation of CUserModeSynth
//
// READ THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// 4530: C++ exception handler used, but unwind semantics are not enabled. Specify -GX
//
// We disable this because we use exceptions and do *not* specify -GX (USE_NATIVE_EH in
// sources).
//
// The one place we use exceptions is around construction of objects that call
// InitializeCriticalSection. We guarantee that it is safe to use in this case with
// the restriction given by not using -GX (automatic objects in the call chain between
// throw and handler are not destructed). Turning on -GX buys us nothing but +10% to code
// size because of the unwind code.
//
// Any other use of exceptions must follow these restrictions or -GX must be turned on.
//
// READ THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!
//

#pragma warning(disable:4530)

#include <objbase.h>
#include <mmsystem.h>
#include <dsoundp.h>
#include <ks.h>

#include "debug.h"
#include "UMSynth.h"
#include "dmusicc.h"
#include "dmusics.h"
#include "math.h"
#include "misc.h"
#include "dmksctrl.h"
#include "dsoundp.h"    // For IDirectSoundSource
#include "..\shared\dmusiccp.h" // For class ids.

#include <dmusprop.h>
#include <xboxdbg.h>

#include "validate.h"
#include <stdio.h>

extern long g_cComponent;

#define CMD_PREFIX      "XAUD"

/////////////////////////////////////////////////////////////////////
// User mode registry helper
//
BOOL GetRegValueDword(
    LPCTSTR szRegPath,
    LPCTSTR szValueName,
    LPDWORD pdwValue)
{
    HKEY  hKeyOpen;
    DWORD dwType;
    DWORD dwCbData;
    LONG  lResult;
    BOOL  fReturn = FALSE;

    assert(pdwValue);

    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                           szRegPath,
                           0, KEY_QUERY_VALUE,
                           &hKeyOpen );

    if (lResult == ERROR_SUCCESS)
    {
        dwCbData = sizeof(DWORD);

        lResult = RegQueryValueEx(hKeyOpen,
                                  szValueName,
                                  NULL,
                                  &dwType,
                                  (LPBYTE)pdwValue,
                                  &dwCbData);

        if (lResult == ERROR_SUCCESS &&
            dwType == REG_DWORD)
        {
            fReturn = TRUE;
        }

        RegCloseKey( hKeyOpen );
    }

    return fReturn;
}

BOOL GetUserRegDWORD( LPCTSTR lpValueName, LPDWORD lpdwValue )
{
    const HKEY hKey = HKEY_CURRENT_USER;
    LPCTSTR lpSubKey = TEXT("Software\\Microsoft\\DMUSProducer\\XboxAddin\\");
	HKEY  hKeyOpen;
	LONG  lResult;
	BOOL  fSuccess = FALSE;
	DWORD dwType;
    DWORD dwSize = sizeof(DWORD);

	lResult = ::RegOpenKeyEx( hKey, lpSubKey, 0, KEY_READ, &hKeyOpen );
	if( lResult == ERROR_SUCCESS )
	{
		lResult = ::RegQueryValueEx( hKeyOpen, lpValueName, 0, &dwType, (BYTE *)lpdwValue, &dwSize );

		if( (dwType == REG_DWORD) && (lResult == ERROR_SUCCESS) && (dwSize == sizeof(DWORD)) )
		{
			fSuccess = TRUE;
		}

		::RegCloseKey( hKeyOpen );
	}

	return fSuccess;
}

BOOL GetUserRegString( LPCTSTR lpValueName, LPTSTR lpszString, LPDWORD lpdwSize )
{
    const HKEY hKey = HKEY_CURRENT_USER;
    LPCTSTR lpSubKey = TEXT("Software\\Microsoft\\DMUSProducer\\XboxAddin\\");
	HKEY  hKeyOpen;
	LONG  lResult;
	BOOL  fSuccess = FALSE;
	DWORD dwType;

	lResult = ::RegOpenKeyEx( hKey, lpSubKey, 0, KEY_READ, &hKeyOpen );
	if( lResult == ERROR_SUCCESS )
	{
		lResult = ::RegQueryValueEx( hKeyOpen, lpValueName, 0, &dwType, (BYTE *)lpszString, lpdwSize );

		if( (dwType == REG_SZ) && (lResult == ERROR_SUCCESS) )
		{
			fSuccess = TRUE;
		}

		::RegCloseKey( hKeyOpen );
	}

	return fSuccess;
}

DWORD GetTheCurrentTime()
{
    static BOOL s_fFirstTime = TRUE;
    static LARGE_INTEGER s_liPerfFrequency;
    static BOOL s_fUsePerfCounter = FALSE;
    if (s_fFirstTime)
    {
        s_fFirstTime = FALSE;
        s_fUsePerfCounter = QueryPerformanceFrequency(&s_liPerfFrequency);
        s_liPerfFrequency.QuadPart /= 1000;
    }
    if (s_fUsePerfCounter)
    {
        LARGE_INTEGER liPerfCounter;
        QueryPerformanceCounter(&liPerfCounter);
        liPerfCounter.QuadPart /= s_liPerfFrequency.QuadPart;
        return (DWORD) liPerfCounter.QuadPart;
    }
    else
    {
        return timeGetTime();
    }
}

/////////////////////////////////////////////////////////////////////////////
// CUserModeSynth

HRESULT CUserModeSynth::Init()
{
	return S_OK;
}


CUserModeSynth::CUserModeSynth()
{
    InterlockedIncrement(&g_cComponent);

    m_fCSInitialized = FALSE;
    ::InitializeCriticalSection(&m_CriticalSection);
    // Note: on pre-Blackcomb OS's, this call can raise an exception; if it
    // ever pops in stress, we can add an exception handler and retry loop.
    m_fCSInitialized = TRUE;
    ::InitializeCriticalSection(&m_XboxCriticalSection);

	m_fPCSynth = TRUE;
    DWORD dwResult;
    if( GetUserRegDWORD( TEXT("EnablePCSynth"), &dwResult ) )
    {
        m_fPCSynth = dwResult ? TRUE : FALSE;
    }
	m_fXboxSynth = TRUE;
    if( GetUserRegDWORD( TEXT("EnableXboxSynth"), &dwResult ) )
    {
        m_fXboxSynth = dwResult ? TRUE : FALSE;
    }
    m_cRef = 0;
    m_dwSampleRate = 22050;
    m_dwChannels = 2;
    m_lVolume = 0;
    m_lBoost = 6 * 100;
    m_lGainAdjust = 6 * 100;            // Default 6 dB boost
	m_fActive = FALSE;
    m_fPCActive = FALSE;
	m_fXboxActive = FALSE;
    m_dwXboxAudioPath = XBOX_APATH_SHARED_STEREOPLUSREVERB;
    m_pSynth = NULL;
    m_pSynthSink = NULL;
    m_pSynthSink8 = NULL;
	m_pMasterClock = NULL;
    m_ullPosition = 0;
    m_dwBufferFlags = BUFFERFLAG_INTERLEAVED;
	m_pdmConnection = NULL;
	m_XboxLatencyClock.SetXboxLatency( 50000 ); // default to 5ms
}

CUserModeSynth::~CUserModeSynth()
{
    Activate(FALSE);

	while( !m_lstDownloadItems.IsEmpty() )
	{
		delete (CDownloadItem *)m_lstDownloadItems.RemoveHead();
	}

    if (m_fCSInitialized)
    {
        ::EnterCriticalSection(&m_XboxCriticalSection);
	    if( m_pdmConnection )
	    {
		    DmCloseConnection(m_pdmConnection);
		    m_pdmConnection = NULL;
	    }
        ::LeaveCriticalSection(&m_XboxCriticalSection);

        ::EnterCriticalSection(&m_CriticalSection);

		if( m_pMasterClock )
		{
			m_pMasterClock->Release();
		}
        if (m_pSynth)
        {
            delete m_pSynth;
            m_pSynth = NULL;
        }

        if (m_pSynthSink)
        {
            m_pSynthSink->Release();
        }

        if (m_pSynthSink8)
        {
            m_pSynthSink8->Release();
        }

        ::LeaveCriticalSection(&m_CriticalSection);
        ::DeleteCriticalSection(&m_CriticalSection);
        ::DeleteCriticalSection(&m_XboxCriticalSection);
    }

      InterlockedDecrement(&g_cComponent);
}


// CUserModeSynth::QueryInterface
//
STDMETHODIMP
CUserModeSynth::QueryInterface(const IID &iid, void **ppv)
{
    V_INAME(IDirectMusicSynth::QueryInterface);
    V_REFGUID(iid);
    V_PTRPTR_WRITE(ppv);

    if (iid == IID_IUnknown || iid == IID_IDirectMusicSynth) {
        *ppv = static_cast<IDirectMusicSynth*>(this);
    }
    else if (iid == IID_IKsControl)
    {
        *ppv = static_cast<IKsControl*>(this);
    }
#ifdef IMPLEMENT_SYNTH8
    else if (iid == IID_IDirectMusicSynth8 )
    {
        *ppv = static_cast<IDirectMusicSynth8*>(this);
    }
#endif
    else if (iid == IID_IDirectSoundSource)
    {
        *ppv = static_cast<IDirectSoundSource*>(this);
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    reinterpret_cast<IUnknown*>(this)->AddRef();
    return S_OK;
}

// CUserModeSynth::AddRef
//
STDMETHODIMP_(ULONG)
CUserModeSynth::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// CUserModeSynth::Release
//
STDMETHODIMP_(ULONG)
CUserModeSynth::Release()
{
    if (!InterlockedDecrement(&m_cRef)) {
        delete this;
        return 0;
    }

    return m_cRef;
}

STDMETHODIMP CUserModeSynth::SetSynthSink(
    IDirectMusicSynthSink *pSynthSink)    // <i IDirectMusicSynthSink> to connect to synth, or
                                        // NULL to disconnect.
{
    HRESULT hr = S_OK;
    V_INAME(IDirectMusicSynth::SetSynthSink);
    V_INTERFACE_OPT(pSynthSink);

    ::EnterCriticalSection(&m_CriticalSection);

//>>>>>>>>. RELEASE THE DSINK IF PRESENT !!!!

    if (m_pSynthSink)
    {
        hr = m_pSynthSink->Init(NULL);
        m_pSynthSink->Release();
    }

    m_pSynthSink = pSynthSink;

//>>>>>>>>> the current state of the format of the the synth is
//>>>>>>>>> ambiguos if a sink has been previously applied.
    m_dwBufferFlags &= ~BUFFERFLAG_MULTIBUFFER;    // .... just in case

    if (m_pSynthSink)
    {
        m_pSynthSink->AddRef();
        hr = m_pSynthSink->Init(static_cast<IDirectMusicSynth*>(this));
    }
    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CUserModeSynth::Open(
    LPDMUS_PORTPARAMS pPortParams)        // <t DMUS_PORTPARAMS> structure for opening the port. If NULL, default settings are used.
{
    V_INAME(IDirectMusicSynth::Open);
    //if (pPortParams == NULL)
    //{
    //    Trace(1, "Error: Open called with NULL PortParams.\n");
    //    return E_FAIL;
    //}

    DWORD cbPortParams = 0;
    DWORD dwVer;

    if (pPortParams)
    {
        V_STRUCTPTR_READ_VER(pPortParams, dwVer);
        V_STRUCTPTR_READ_VER_CASE(DMUS_PORTPARAMS, 7);
        V_STRUCTPTR_READ_VER_CASE(DMUS_PORTPARAMS, 8);
        V_STRUCTPTR_READ_VER_END(DMUS_PORTPARAMS, pPortParams);

        switch (dwVer)
        {
            case 7:
                cbPortParams = sizeof(DMUS_PORTPARAMS7);
                break;

            case 8:
                cbPortParams = sizeof(DMUS_PORTPARAMS8);
                break;
        }
    }

    bool bPartialOpen = false;

    DMUS_PORTPARAMS myParams;
    myParams.dwSize = sizeof (myParams);
    myParams.dwVoices = 32;
    myParams.dwChannelGroups = 2;
    myParams.dwAudioChannels = 2;
    myParams.dwSampleRate = 22050;
#ifdef REVERB_ENABLED
    myParams.dwEffectFlags = DMUS_EFFECT_REVERB;
#else
    myParams.dwEffectFlags = DMUS_EFFECT_NONE;
#endif
    myParams.fShare = FALSE;
    myParams.dwValidParams =
        DMUS_PORTPARAMS_VOICES |
        DMUS_PORTPARAMS_CHANNELGROUPS |
        DMUS_PORTPARAMS_AUDIOCHANNELS |
        DMUS_PORTPARAMS_SAMPLERATE |
        DMUS_PORTPARAMS_EFFECTS |
        DMUS_PORTPARAMS_SHARE;

    if (pPortParams)
    {
        if (pPortParams->dwSize >= sizeof(DMUS_PORTPARAMS8))
        {
            myParams.dwValidParams |= DMUS_PORTPARAMS_FEATURES;
            myParams.dwFeatures = 0;
        }
        if (pPortParams->dwValidParams & DMUS_PORTPARAMS_VOICES)
        {
            if (pPortParams->dwVoices)
            {
                if (pPortParams->dwVoices <= MAX_VOICES)
                {
                    myParams.dwVoices = pPortParams->dwVoices;
                }
                else
                {
                    bPartialOpen = true;
                    myParams.dwVoices = MAX_VOICES;
                }
            }
            else
            {
                bPartialOpen = true;
                myParams.dwVoices = 1; // MIN_VOICES
            }
        }
        if (pPortParams->dwValidParams & DMUS_PORTPARAMS_CHANNELGROUPS)
        {
            if (pPortParams->dwChannelGroups)
            {
                if (pPortParams->dwChannelGroups <= MAX_CHANNEL_GROUPS)
                {
                    myParams.dwChannelGroups = pPortParams->dwChannelGroups;
                }
                else
                {
                    bPartialOpen = true;
                    myParams.dwChannelGroups = MAX_CHANNEL_GROUPS;
                }
            }
            else
            {
                bPartialOpen = true;
                myParams.dwChannelGroups = 1; // MIN_CHANNEL_GROUPS
            }
        }
        if (pPortParams->dwValidParams & DMUS_PORTPARAMS_AUDIOCHANNELS)
        {
            if (pPortParams->dwAudioChannels)
            {
                if (pPortParams->dwAudioChannels <= 2)
                {
                    myParams.dwAudioChannels = pPortParams->dwAudioChannels;
                }
                else
                {
                    bPartialOpen = true;
                    myParams.dwAudioChannels = 2; // MAX_AUDIO_CHANNELS
                }
            }
            else
            {
                bPartialOpen = true;
                myParams.dwAudioChannels = 1; // MIN_AUDIO_CHANNELS
            }
        }
        if (pPortParams->dwValidParams & DMUS_PORTPARAMS_SAMPLERATE)
        {
            if (dwVer == 7)
            {
                // DX-7 compat: clamp sample rate to one of the
                // understood rates.
                //
                if (pPortParams->dwSampleRate > 30000)
                {
                    if(pPortParams->dwSampleRate != 44100)
                    {
                        bPartialOpen = true;
                    }

                    myParams.dwSampleRate = 44100;
                }
                else if (pPortParams->dwSampleRate > 15000)
                {
                    if(pPortParams->dwSampleRate != 22050)
                    {
                        bPartialOpen = true;
                    }

                    myParams.dwSampleRate = 22050;
                }
                else
                {
                    if(pPortParams->dwSampleRate != 11025)
                    {
                        bPartialOpen = true;
                    }

                    myParams.dwSampleRate = 11025;
                }
            }
            else
            {
                if (pPortParams->dwSampleRate > 96000)
                {
                    bPartialOpen = true;
                    myParams.dwSampleRate = 96000;
                }
                else if (pPortParams->dwSampleRate < 11025)
                {
                    bPartialOpen = true;
                    myParams.dwSampleRate = 11025;
                }
                else myParams.dwSampleRate = pPortParams->dwSampleRate;
            }
        }
        if (pPortParams->dwValidParams & DMUS_PORTPARAMS_EFFECTS)
        {
            if (pPortParams->dwEffectFlags & ~DMUS_EFFECT_REVERB)
            {
                bPartialOpen = true;
                pPortParams->dwEffectFlags &= DMUS_EFFECT_REVERB;
            }

#ifdef REVERB_ENABLED
            myParams.dwEffectFlags = pPortParams->dwEffectFlags;
#else
            myParams.dwEffectFlags = DMUS_EFFECT_NONE;
            if (pPortParams->dwEffectFlags & DMUS_EFFECT_REVERB)
            {
                bPartialOpen = true;
            }
#endif
        }
        if (pPortParams->dwValidParams & DMUS_PORTPARAMS_SHARE)
        {
            if (pPortParams->fShare)
            {
                bPartialOpen = true;
            }
        }

        if ((pPortParams->dwValidParams & DMUS_PORTPARAMS_FEATURES) &&
            (pPortParams->dwSize >= sizeof(DMUS_PORTPARAMS8)))
        {
            myParams.dwFeatures = pPortParams->dwFeatures;
        }
    }

    if (pPortParams)
    {
        DWORD dwSize = min(cbPortParams, myParams.dwSize);

        memcpy(pPortParams, &myParams, dwSize);
        pPortParams->dwSize = dwSize;
    }

    m_dwSampleRate = myParams.dwSampleRate;
    m_dwChannels = myParams.dwAudioChannels;
    m_dwBufferFlags = (m_dwChannels==1)?BUFFERFLAG_MONO:BUFFERFLAG_INTERLEAVED;

    ::EnterCriticalSection(&m_CriticalSection);
    HRESULT hr = DMUS_E_ALREADYOPEN;
    if (!m_pSynth)
    {
        try
        {
            m_pSynth = new CSynth;
        }
        catch( ... )
        {
            m_pSynth = NULL;
        }

        if (!m_pSynth)
        {
            hr = E_OUTOFMEMORY;
        }
        else
        {
            hr = m_pSynth->Open(myParams.dwChannelGroups,
                myParams.dwVoices,
                (myParams.dwEffectFlags & DMUS_EFFECT_REVERB) ? TRUE : FALSE);
            if (SUCCEEDED(hr))
            {
                m_pSynth->SetGainAdjust(m_lGainAdjust);
                m_pSynth->Activate(m_dwSampleRate, m_dwBufferFlags);
            }
            else
            {
                delete m_pSynth;
                m_pSynth = NULL;
            }
        }
    }
    ::LeaveCriticalSection(&m_CriticalSection);

    if(SUCCEEDED(hr))
    {
        if(bPartialOpen)
        {
            hr = S_FALSE;
        }
    }

    return hr;
}

STDMETHODIMP CUserModeSynth::SetNumChannelGroups(
    DWORD dwGroups)        // Number of ChannelGroups requested.
{
    ::EnterCriticalSection(&m_CriticalSection);
    HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
    if (m_pSynth)
    {
        hr = m_pSynth->SetNumChannelGroups(dwGroups);

/*        ::EnterCriticalSection( &m_XboxCriticalSection );
	    if( m_pdmConnection && m_fXboxActive )
        {
		    char	szCmd[MAX_PATH];
		    char    szResp[MAX_PATH];
		    DWORD   cchResp = MAX_PATH;

		    sprintf(szCmd, TEXT(CMD_PREFIX "!Synth.SetChannelGroups %d"), m_pSynth->m_dwControlCount );
            DmSendCommand(m_pdmConnection, szCmd, szResp, &cchResp);
        }
        ::LeaveCriticalSection( &m_XboxCriticalSection );*/
    }
    ::LeaveCriticalSection(&m_CriticalSection);
    return hr;
}

STDMETHODIMP CUserModeSynth::Close()
{
    ::EnterCriticalSection(&m_CriticalSection);
    HRESULT hr = DMUS_E_ALREADYCLOSED;
    if (m_pSynth)
    {
        hr = m_pSynth->Close();
        delete m_pSynth;
        m_pSynth = NULL;
    }
    ::LeaveCriticalSection(&m_CriticalSection);
    return hr;
}
STDMETHODIMP CUserModeSynth::Download(
    LPHANDLE phDownload,    // Pointer to download handle, to be created by <om IDirectMusicSynth::Download> and used later to unload the data.
    LPVOID pvData,          // Pointer to continuous memory segment with download data.
    LPBOOL pbFree)          // <p pbFree> indicates whether the synthesizer wishes to keep the memory in <p pvData> allocated.
{
    HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
    V_INAME(IDirectMusicSynth::Download);
    V_PTR_WRITE(phDownload, HANDLE);
    V_PTR_WRITE(pbFree, BOOL);

    // pvData is validated inside synth while parsing.
    ::EnterCriticalSection(&m_CriticalSection);

	HANDLE *phUpdate = NULL;

	DMUS_DOWNLOADINFO * pInfo = (DMUS_DOWNLOADINFO *)pvData;

    ::EnterCriticalSection( &m_XboxCriticalSection );
	if( m_pdmConnection && m_fXboxActive
	&&	((pInfo->dwDLType == DMUS_DOWNLOADINFO_INSTRUMENT) ||
		 (pInfo->dwDLType == DMUS_DOWNLOADINFO_INSTRUMENT2) ||
		 (pInfo->dwDLType == DMUS_DOWNLOADINFO_WAVE)) )
	{
		static DWORD s_dwID = 0;
		const DWORD cbBuffer = *(((DWORD *)phDownload) + 4);
		char	szCmd[MAX_PATH];
		char    szResp[MAX_PATH];
		DWORD   cchResp = MAX_PATH;

		const DWORD dwDownloadID = s_dwID;

		sprintf(szCmd, TEXT(CMD_PREFIX "!Synth.Download %d %d"), dwDownloadID, cbBuffer );

		s_dwID++;

		//OutputDebugStringA(szCmd);

		// Send the command to the Xbox
		HRESULT hr = DmSendCommand(m_pdmConnection, szCmd, szResp, &cchResp);

		//szResp[cchResp] = '\n';
		//szResp[cchResp+1] = 0;
		//OutputDebugStringA(szResp);

		if( SUCCEEDED(hr) )
		{
			// Send the buffer to the Xbox
			DmSendBinary(m_pdmConnection, pvData, cbBuffer);

			//OutputDebugStringA("Binary - ");

			// Receive the response
			DmReceiveStatusResponse(m_pdmConnection, szResp, &cchResp);

			//szResp[cchResp] = '\n';
			//szResp[cchResp+1] = 0;
			//OutputDebugStringA(szResp);

			CDownloadItem *pDownloadItem = new CDownloadItem;
			if( pDownloadItem )
			{
				phUpdate = &pDownloadItem->m_hItem;
				pDownloadItem->m_hItem = NULL;
				pDownloadItem->m_dwID = dwDownloadID;
				m_lstDownloadItems.AddHead( pDownloadItem );
			}
		}
	}
    ::LeaveCriticalSection( &m_XboxCriticalSection );

    if (m_pSynth)
    {
        hr = m_pSynth->Download(phDownload, pvData, pbFree);
    }

	if( phUpdate )
	{
		*phUpdate = *phDownload;
	}

    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CUserModeSynth::Unload(
    HANDLE hDownload,   // Handle to data, previously downloaded with a call to <om IDirectMusicSynth::Download>.
    HRESULT ( CALLBACK *lpFreeHandle)(HANDLE, HANDLE), // If the original call to
                        // <om IDirectMusicSynth::Download> returned FALSE in <p pbFree>,
                        // the synthesizer hung onto the memory in the download chunk. If so,
                        // the caller must be notified once the memory has been freed,
                        // but that could occur later than <om IDirectMusicSynth::Download>
                        // since a wave might be currently in use. <p lpFreeHandle> is a
                        // pointer to a callback
                        // function which will be called when the memory is no longer in use.
    HANDLE hUserData)   // Pointer to user data, passed as a parameter to the
                        // <p lpFreeHandle> function, typically used so the callback routine can retrieve
                        // its state.
{
    HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
    ::EnterCriticalSection(&m_CriticalSection);

    ::EnterCriticalSection(&m_XboxCriticalSection);
	CDownloadItem *pDownloadItem = (CDownloadItem *)m_lstDownloadItems.GetHead();
	while( pDownloadItem )
	{
		if( pDownloadItem->m_hItem == hDownload )
		{
			m_lstDownloadItems.Remove( pDownloadItem );

	        if( m_pdmConnection )
	        {
			    char	szCmd[MAX_PATH];
			    char    szResp[MAX_PATH];
			    DWORD   cchResp = MAX_PATH;

			    sprintf(szCmd, TEXT(CMD_PREFIX "!Synth.Unload %d"), pDownloadItem->m_dwID );

			    // Send the command to the Xbox
			    DmSendCommand(m_pdmConnection, szCmd, szResp, &cchResp);
            }

			delete pDownloadItem;
			break;
		}
		pDownloadItem = pDownloadItem->GetNext();
	}
    ::LeaveCriticalSection(&m_XboxCriticalSection);

    if (m_pSynth)
    {
        hr = m_pSynth->Unload(hDownload, lpFreeHandle, hUserData);
    }

    ::LeaveCriticalSection(&m_CriticalSection);
    return hr;
}

STDMETHODIMP CUserModeSynth::PlayBuffer(
    REFERENCE_TIME rt,  // Start time of the buffer. This should be in
                        // REFERENCE_TIME units, relative to the master
                        // clock, previously set with a call to <om IDirectMusicSynth::SetMasterClock>.
                        // And, this should be after the time returned by the clock in
                        // <om IDirectMusicSynth::GetLatencyClock>.
    LPBYTE pbBuffer,    // Memory chunk with all the MIDI events, generated by <i IDirectMusicBuffer>.
    DWORD cbBuffer)     // Size of buffer.
{
    class MIDIEVENT : public DMUS_EVENTHEADER {
    public:
         BYTE  abEvent[4];           /* Actual event data, rounded up to be an even number */
                                     /* of QWORD's (8 bytes) */
    };

    typedef class MIDIEVENT FAR  *LPMIDIEVENT;
    #define QWORD_ALIGN(x) (((x) + 7) & ~7)

    HRESULT hr;

    V_INAME(IDirectMusicSynth::PlayBuffer);
    V_BUFPTR_READ(pbBuffer, cbBuffer);

    ::EnterCriticalSection(&m_CriticalSection);

    if (!m_fPCActive && !m_fXboxActive)
    {
        ::LeaveCriticalSection(&m_CriticalSection);
        Trace(3, "Warning: Synth is inactive, can not process MIDI events.\n");
        return DMUS_E_SYNTHINACTIVE;
    }

    ::EnterCriticalSection(&m_XboxCriticalSection);
	if( m_pdmConnection && m_fXboxActive )
	{
		char	szCmd[MAX_PATH];
		char    szResp[MAX_PATH];
		DWORD   cchResp = MAX_PATH;

		REFERENCE_TIME rtNow = 0;
		REFERENCE_TIME rtSlave = 0;
		m_pMasterClock->GetTime( &rtNow );
		if( SUCCEEDED( DmSendCommand(m_pdmConnection, CMD_PREFIX "!Synth.GetTime", szResp, &cchResp) ) )
		{
			if( 1 == sscanf(szResp, "200- %I64d", &rtSlave) )
			{
				m_PLCXbox.SyncToMaster( rtSlave, rtNow );
			}
		}

		// Convert from PC clock to Xbox clock
		REFERENCE_TIME rtXbox = rt;
		m_PLCXbox.SetSlaveTime( rtXbox, &rtXbox);

		//sprintf(szCmd, "PlayBuffer %I64d %I64d %I64d %I64d\n", rtXbox, rtSlave, rt, rtNow );

		//OutputDebugStringA(szCmd);

		sprintf(szCmd, TEXT(CMD_PREFIX "!Synth.PlayBuffer %I64d %I64d %d"), rtXbox, rtNow, cbBuffer );

		//OutputDebugStringA(szCmd);

		// Send the command to the Xbox
		cchResp = MAX_PATH;
		HRESULT hr = DmSendCommand(m_pdmConnection, szCmd, szResp, &cchResp);

		//szResp[cchResp] = '\n';
		//szResp[cchResp+1] = 0;
		//OutputDebugStringA(szResp);

		if( SUCCEEDED(hr) )
		{
			// Send the buffer to the Xbox
			DmSendBinary(m_pdmConnection, pbBuffer, cbBuffer);

			//OutputDebugStringA("Binary - ");

			// Receive the response
			DmReceiveStatusResponse(m_pdmConnection, szResp, &cchResp);

			//szResp[cchResp] = '\n';
			//szResp[cchResp+1] = 0;
			//OutputDebugStringA(szResp);
		}
	}
    ::LeaveCriticalSection(&m_XboxCriticalSection);

    if (!m_fPCActive)
    {
        // m_fXboxActive must be true
        ::LeaveCriticalSection(&m_CriticalSection);
        return S_OK;
    }

    if (!m_pSynthSink && !m_pSynthSink8)
    {
        ::LeaveCriticalSection(&m_CriticalSection);
        return DMUS_E_NOSYNTHSINK;
    }

    LPMIDIEVENT lpEventHdr;
    DWORD cbEvent;

    while (cbBuffer)
    {
        if (cbBuffer < sizeof(DMUS_EVENTHEADER))
        {
            Trace(1, "Error: PlayBuffer called with error in buffer size.\n");
            ::LeaveCriticalSection(&m_CriticalSection);
            return E_INVALIDARG;
        }

        lpEventHdr = (LPMIDIEVENT)pbBuffer;
        cbEvent = DMUS_EVENT_SIZE(lpEventHdr->cbEvent);
        if (cbEvent > cbBuffer)
        {
            Trace(1, "Error: PlayBuffer called with error in event size.\n");
            ::LeaveCriticalSection(&m_CriticalSection);
            return E_INVALIDARG;
        }

        pbBuffer += cbEvent;
        cbBuffer -= cbEvent;
        if ( m_pSynthSink )
        {
            hr = m_pSynth->PlayBuffer(m_pSynthSink,
                                      rt + lpEventHdr->rtDelta,
                                      &lpEventHdr->abEvent[0],
                                      lpEventHdr->cbEvent,
                                      lpEventHdr->dwChannelGroup);
        }

        if ( m_pSynthSink8 )
        {
            hr = m_pSynth->PlayBuffer(m_pSynthSink8,
                                      rt + lpEventHdr->rtDelta,
                                      &lpEventHdr->abEvent[0],
                                      lpEventHdr->cbEvent,
                                      lpEventHdr->dwChannelGroup);
        }

        if (FAILED(hr))
        {
            ::LeaveCriticalSection(&m_CriticalSection);
            return hr;
        }
    }
    ::LeaveCriticalSection(&m_CriticalSection);
    return S_OK;
}


STDMETHODIMP CUserModeSynth::GetPortCaps(
    LPDMUS_PORTCAPS pCaps)    // <t DMUS_PORTCAPS> structure to be filled in by synth.
{
    V_INAME(IDirectMusicSynth::GetPortCaps);
    V_STRUCTPTR_WRITE(pCaps, DMUS_PORTCAPS);

    wcscpy(pCaps->wszDescription, L"Microsoft Xbox Synthesizer");

    pCaps->dwClass = DMUS_PC_OUTPUTCLASS;
    pCaps->dwType = DMUS_PORT_USER_MODE_SYNTH;
    pCaps->dwFlags = DMUS_PC_DLS | DMUS_PC_DLS2 | DMUS_PC_SOFTWARESYNTH | DMUS_PC_DIRECTSOUND | DMUS_PC_EXTERNAL;

    pCaps->guidPort = CLSID_XboxSynth;

    pCaps->dwMemorySize = DMUS_PC_SYSTEMMEMORY;
    pCaps->dwMaxChannelGroups = MAX_CHANNEL_GROUPS;
    pCaps->dwMaxVoices = MAX_VOICES;
    pCaps->dwMaxAudioChannels = 2;

    pCaps->dwEffectFlags = 0;

    return S_OK;
}

STDMETHODIMP CUserModeSynth::SetMasterClock(
    IReferenceClock *pClock)    // Pointer to master <i IReferenceClock>,
                                // used by all devices in current instance of DirectMusic.

{
    V_INAME(IDirectMusicSynth::SetMasterClock);
    V_INTERFACE(pClock);

	if(	m_pMasterClock )
	{
		m_pMasterClock->Release();
	}
	m_pMasterClock = pClock;
	if( m_pMasterClock )
	{
		m_pMasterClock->AddRef();
	}

	m_XboxLatencyClock.SetMasterClock( pClock );

    return S_OK;
}

STDMETHODIMP CUserModeSynth::GetLatencyClock(
    IReferenceClock **ppClock)    // <i IReferenceClock> interface designed to return the current mix time.

{
    IDirectSoundSynthSink* pDSSink = NULL;

    V_INAME(IDirectMusicSynth::GetLatencyClock);
    V_PTR_WRITE(ppClock, IReferenceClock *);

	return m_XboxLatencyClock.QueryInterface( IID_IReferenceClock, (void**)ppClock );
}

STDMETHODIMP CUserModeSynth::Activate(
    BOOL fEnable)            // Whether to activate or deactivate audio.
{
    HRESULT hr = S_OK;

//    ::EnterCriticalSection(&m_CriticalSection);
    if (fEnable && m_fActive)
    {
		Trace(1, "Error: Synth::Activate- synth already active\n");
		hr = S_FALSE;
	}
	else if (!fEnable && !m_fActive)
	{
        Trace(2, "Warning: Synth::Activate- synth already inactive\n");
        hr = S_FALSE;
	}
	else
	{
		if (m_fXboxSynth)
		{
			hr = ActivateXboxSynth( fEnable );
		}
		if (SUCCEEDED(hr) && m_fPCSynth)
		{
			hr = ActivatePCSynth( fEnable );

			// If this failed, undo the XboxSynth activation
			if (FAILED(hr) && m_fXboxSynth)
			{
				ActivateXboxSynth( !fEnable );
			}
		}
		if (SUCCEEDED(hr))
		{
			m_fActive = fEnable;
		}
	}
//    ::LeaveCriticalSection(&m_CriticalSection);
	return hr;
}

HRESULT CUserModeSynth::ActivateXboxSynth(BOOL fEnable)
{
	if( fEnable && m_fXboxActive )
	{
		return S_FALSE;
	}
	
	if( !fEnable && !m_fXboxActive )
	{
		return S_FALSE;
	}

	if( !m_fXboxSynth )
	{
		return S_FALSE;
	}

    ::EnterCriticalSection(&m_XboxCriticalSection);
	HRESULT hr = S_OK;
	if( fEnable )
	{
		bool fWrongVersion = false;

		// If no connection, try and connect to the Xbox
		if( !m_pdmConnection )
		{
        	TCHAR tcstrXboxName[MAX_PATH];
	        ZeroMemory( tcstrXboxName, sizeof(TCHAR) * MAX_PATH );
	        DWORD dwCbData = sizeof(TCHAR) * MAX_PATH;
	        if( GetUserRegString( TEXT("XboxName"), tcstrXboxName, &dwCbData ) )
	        {
        		hr = DmSetXboxNameNoRegister( tcstrXboxName );
	        }

			DmOpenConnection(&m_pdmConnection);

			if( m_pdmConnection )
			{
				// Send a command to the Xbox, to make sure DMConsole is running
				DWORD cchResp = MAX_PATH;
				char szResp[MAX_PATH];

				// If the connection failed
				if( FAILED(DmSendCommand(m_pdmConnection, TEXT(CMD_PREFIX "!__connect__"), szResp, &cchResp)))
				{
					// DMConsole is not running - close the connection
					DmCloseConnection(m_pdmConnection);
					m_pdmConnection = NULL;
				}
				/*
				else if( strcmp(szResp, "200- Version 0.1 Connected.") )
				{
					fWrongVersion = true;

					// Wrong version of DMConsole is running - close the connection
					DmCloseConnection(m_pdmConnection);
					m_pdmConnection = NULL;
				}
				*/
			}
		}

		// If no connection, fail
		if (m_pdmConnection == NULL)
		{
			hr = fWrongVersion ? XBDM_CANNOTACCESS : XBDM_CANNOTCONNECT;
		}
		else
		{
			m_fXboxActive = TRUE;

			// Initialize the AudConsole application
		    char    szResp[MAX_PATH];
		    DWORD   cchResp = MAX_PATH;
            DmSendCommand(m_pdmConnection, TEXT(CMD_PREFIX "!Synth.Initialize"), szResp, &cchResp);

            // Initialize number of channel groups
            if( m_pSynth )
            {
			    char	szCmd[MAX_PATH];
		        sprintf(szCmd, TEXT(CMD_PREFIX "!Synth.SetChannelGroups %d"), m_pSynth->m_dwControlCount );
                DmSendCommand(m_pdmConnection, szCmd, szResp, &cchResp);
            }

            // Initialize default audiopath
	        if( m_pSynth )
            {
		        char	szCmd[MAX_PATH];
		        sprintf(szCmd, TEXT(CMD_PREFIX "!Synth.SetDefaultAudiopath %d"), m_dwXboxAudioPath );
                DmSendCommand(m_pdmConnection, szCmd, szResp, &cchResp);
            }


			// Initialize the PL clock
			REFERENCE_TIME rtNow = 0;
			m_pMasterClock->GetTime( &rtNow );

			cchResp = MAX_PATH;
			if( SUCCEEDED( DmSendCommand(m_pdmConnection, CMD_PREFIX "!Synth.GetTime", szResp, &cchResp) ) )
			{
				REFERENCE_TIME rtSlave;
				if( 1 == sscanf(szResp, "200- %I64d", &rtSlave) )
				{
					m_PLCXbox.Start( rtNow, rtSlave );
				}
			}
		}
	}
	else
	{
	    if( m_pdmConnection )
	    {
            // Unload everything we've downloaded
			char	szCmd[MAX_PATH];
			char    szResp[MAX_PATH];
		    DWORD   cchResp = MAX_PATH;
	        while( !m_lstDownloadItems.IsEmpty() )
	        {
		        CDownloadItem *pDownloadItem = (CDownloadItem *)m_lstDownloadItems.RemoveHead();

			    sprintf(szCmd, TEXT(CMD_PREFIX "!Synth.Unload %d"), pDownloadItem->m_dwID );

			    // Send the command to the Xbox
			    DmSendCommand(m_pdmConnection, szCmd, szResp, &cchResp);

			    delete pDownloadItem;
	        }

            DmSendCommand(m_pdmConnection, TEXT(CMD_PREFIX "!Synth.Uninitialize"), szResp, &cchResp);

			DmCloseConnection(m_pdmConnection);
		    m_pdmConnection = NULL;
	    }

		m_fXboxActive = FALSE;
	}
    ::LeaveCriticalSection(&m_XboxCriticalSection);

	return hr;
}

HRESULT CUserModeSynth::ActivatePCSynth(BOOL fEnable)
{
	if( fEnable && m_fPCActive )
	{
		return S_FALSE;
	}
	
	if( !fEnable && !m_fPCActive )
	{
		return S_FALSE;
	}

	if( !m_fPCSynth )
	{
		return S_FALSE;
	}

	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;

    ::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynthSink || m_pSynthSink8)
    {
		if( fEnable )
		{
			if (m_dwSampleRate && m_dwChannels)
			{
				if (m_pSynth)
				{
					m_pSynth->Activate(m_dwSampleRate, m_dwBufferFlags);

					if (m_pSynthSink)
					{
						if (SUCCEEDED(m_pSynthSink->Activate(fEnable)))
						{
							m_fPCActive = TRUE;
							hr = S_OK;

							IReferenceClock *pLatencyClock;
							if( SUCCEEDED( m_pSynthSink->GetLatencyClock( &pLatencyClock ) ) )
							{
								m_XboxLatencyClock.SetSinkLatencyClock( pLatencyClock );
								pLatencyClock->Release();
							}
						}
					}

					if ( m_pSynthSink8 )
					{
						hr = m_pSynthSink8->Activate(fEnable);
						if (SUCCEEDED(hr) || hr == DMUS_E_SYNTHACTIVE)
						{
							m_fPCActive = TRUE;
							hr = S_OK;

							IReferenceClock *pLatencyClock;
							if( SUCCEEDED( m_pSynthSink8->GetLatencyClock( &pLatencyClock ) ) )
							{
								m_XboxLatencyClock.SetSinkLatencyClock( pLatencyClock );
								pLatencyClock->Release();
							}
						}
					}
				}
			}
		}
		else
		{
            m_fPCActive = FALSE;

            if (m_pSynth)
            {
                m_pSynth->Deactivate();
            }

            if (m_pSynthSink)
            {
                if (SUCCEEDED(m_pSynthSink->Activate(fEnable)))
                {
                    hr = S_OK;
                }
            }

            if (m_pSynthSink8)
            {
                hr = m_pSynthSink8->Activate(fEnable);
            }

			m_XboxLatencyClock.SetSinkLatencyClock( NULL );
        }
    }
    else
    {
        Trace(1, "Error: Synth::Activate- sink not connected\n");
        hr = DMUS_E_NOSYNTHSINK;
    }
    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CUserModeSynth::Render(
    short *pBuffer,        // Pointer to buffer to write into.
    DWORD dwLength,        // Length of buffer, in samples. This is not the
                        // memory size of the buffer. The memory size may vary,
                        // dependant on the buffer format, which the synth
                        // sets when in response to an <om IDirectMusicSynth::Activate>
                        // command.
    LONGLONG llPosition)    // Position in the audio stream, also in samples.
                        // This should always increment by <p dwLength> after
                        // each call.
{
    V_INAME(IDirectMusicSynth::Render);
    V_BUFPTR_WRITE(pBuffer, dwLength << (m_dwBufferFlags&BUFFERFLAG_INTERLEAVED)?1:0 );

    if (!m_pSynthSink)
    {
        Trace(1, "Error: Synth is not configured, can not render.\n");
        return DMUS_E_SYNTHNOTCONFIGURED;
    }
    if (!m_fPCActive)
    {
        Trace(1, "Error: Synth is not inactive, can not render.\n");
        return DMUS_E_SYNTHINACTIVE;
    }

    ::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {
        DWORD dwID[2];
        DWORD dwFuncID[2];
        long lPitchBend[2];

        // Setup busid for a Backward compatible DX7 interleaved buffer
        dwID[0]     = DSBUSID_LEFT;
        dwID[1]     = DSBUSID_RIGHT;
        dwFuncID[0] = DSBUSID_LEFT;
        dwFuncID[1] = DSBUSID_RIGHT;
        lPitchBend[0] = lPitchBend[1] = 0;

        DWORD dwChannels = 1;
        if (m_pSynth->m_dwStereo)
        {
            dwChannels = 2;
        }
        m_pSynth->Mix(&pBuffer, dwID, dwFuncID, lPitchBend, dwChannels, m_dwBufferFlags, dwLength, llPosition);
    }
    ::LeaveCriticalSection(&m_CriticalSection);
    return S_OK;
}

STDMETHODIMP CUserModeSynth::SetChannelPriority(
    DWORD dwChannelGroup,
    DWORD dwChannel,
    DWORD dwPriority)
{
    if (m_pSynth)
    {
        return m_pSynth->SetChannelPriority(dwChannelGroup, dwChannel, dwPriority);
    }
    Trace(1, "Error: Synth not initialized.\n");
    return E_FAIL;
}

STDMETHODIMP CUserModeSynth::GetChannelPriority(
    DWORD dwChannelGroup,
    DWORD dwChannel,
    LPDWORD pdwPriority)
{
    if (m_pSynth)
    {
        return m_pSynth->GetChannelPriority(dwChannelGroup, dwChannel, pdwPriority);
    }
    Trace(1, "Error: Synth not initialized.\n");
    return E_FAIL;
}

// IDirectSoundSource version of GetFormat()

STDMETHODIMP CUserModeSynth::GetFormat(
    LPWAVEFORMATEX pWaveFormatEx,
    DWORD dwSizeAllocated,
    LPDWORD pdwSizeWritten)
{
    V_INAME(IDirectMusicSynth::GetFormat);

    if (!m_pSynth)
    {
        Trace(1, "Error: Synth is not configured, can not get format.\n");
        return DMUS_E_SYNTHNOTCONFIGURED;
    }

    if (!pWaveFormatEx && !pdwSizeWritten)
    {
        Trace(1, "Error: GetFormat failed, must request either the format or the required size");
        return E_INVALIDARG;
    }

    if (pdwSizeWritten)
    {
        V_PTR_WRITE(pdwSizeWritten, DWORD);
        *pdwSizeWritten = sizeof(WAVEFORMATEX);
    }

    if (pWaveFormatEx)
    {
        V_BUFPTR_WRITE_OPT(pWaveFormatEx, dwSizeAllocated);
        WAVEFORMATEX wfx;
        memset(&wfx, 0, sizeof(wfx));
        wfx.wFormatTag = WAVE_FORMAT_PCM;
        wfx.nChannels = (WORD)m_dwChannels;
        wfx.nSamplesPerSec = (WORD)m_dwSampleRate;
        wfx.wBitsPerSample = 16;
        wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample / 8);
        wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
        wfx.cbSize = 0; // no extra data

        memcpy(pWaveFormatEx, &wfx, min(sizeof wfx, dwSizeAllocated));
    }

    return S_OK;
}

// IDirectMusicSynth8 version of GetFormat()

STDMETHODIMP CUserModeSynth::GetFormat(
    LPWAVEFORMATEX pWaveFormatEx,
    LPDWORD pdwWaveFormatExSize)
{
    V_INAME(IDirectMusicSynth::GetFormat);
    V_PTR_WRITE(pdwWaveFormatExSize, DWORD);
    V_BUFPTR_WRITE_OPT(pWaveFormatEx, *pdwWaveFormatExSize);
    return GetFormat(pWaveFormatEx, *pdwWaveFormatExSize, pdwWaveFormatExSize);
}

STDMETHODIMP CUserModeSynth::GetAppend(
    DWORD* pdwAppend)
{
    V_INAME(IDirectMusicSynth::GetAppend);
    V_PTR_WRITE(pdwAppend, DWORD);

    *pdwAppend = 2; // The synth needs 1 extra sample for loop interpolation.
                    // We're adding one more to be paranoid.
    return S_OK;
}

STDMETHODIMP CUserModeSynth::GetRunningStats(
    LPDMUS_SYNTHSTATS pStats)    // <t DMUS_SYNTHSTATS> structure to fill in.

{
    HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
    V_INAME(IDirectMusicSynth::GetRunningStats);
    V_STRUCTPTR_WRITE(pStats, DMUS_SYNTHSTATS);
    if ( pStats->dwSize == sizeof(DMUS_SYNTHSTATS8) )
    {
        V_STRUCTPTR_WRITE(pStats, DMUS_SYNTHSTATS8);
    }

    if (!m_pSynthSink && !m_pSynthSink8)
    {
        Trace(1, "Error: Synth::GetRunningStats failed because synth is inactove.\n");
        return hr;
    }

    if (m_fPCActive)
    {
        ::EnterCriticalSection(&m_CriticalSection);
        if (m_pSynth)
        {
            PerfStats Stats;
            m_pSynth->GetPerformanceStats(&Stats);
            long lCPU = Stats.dwCPU;
            if (Stats.dwVoices)
            {
                lCPU /= Stats.dwVoices;
            }
            else
            {
                lCPU = 0;
            }
            pStats->dwVoices = Stats.dwVoices;
            pStats->dwCPUPerVoice = lCPU * 10;
            pStats->dwTotalCPU = Stats.dwCPU * 10;
            pStats->dwLostNotes = Stats.dwNotesLost;
            long ldB = 6;
            double fLevel = Stats.dwMaxAmplitude;
            if (Stats.dwMaxAmplitude < 1)
            {
                fLevel = -96.0;
            }
            else
            {
                fLevel /= 32768.0;
                fLevel = log10(fLevel);
                fLevel *= 20.0;
            }
            pStats->lPeakVolume = (long) fLevel;
            pStats->dwValidStats = DMUS_SYNTHSTATS_VOICES | DMUS_SYNTHSTATS_TOTAL_CPU |
                DMUS_SYNTHSTATS_CPU_PER_VOICE | DMUS_SYNTHSTATS_LOST_NOTES | DMUS_SYNTHSTATS_PEAK_VOLUME;

            if ( pStats->dwSize == sizeof(DMUS_SYNTHSTATS8) )
            {
                ((DMUS_SYNTHSTATS8*)pStats)->dwSynthMemUse = m_pSynth->m_Instruments.m_dwSynthMemUse;
            }


            hr = S_OK;
        }
        ::LeaveCriticalSection(&m_CriticalSection);
    }
    else
    {
        DWORD dwSize = pStats->dwSize;
        memset(pStats, 0, dwSize);
        pStats->dwSize = dwSize;

        hr = S_OK;
    }
    return hr;
}

static DWORD dwPropFalse = FALSE;
static DWORD dwPropTrue  = TRUE;
static DWORD dwSystemMemory = DMUS_PC_SYSTEMMEMORY;

GENERICPROPERTY CUserModeSynth::m_aProperty[] =
{
    {
        &GUID_DMUS_PROP_GM_Hardware,        // Set
        0,                                  // Item
        KSPROPERTY_SUPPORT_GET,             // KS support flags
        GENPROP_F_STATIC,                   // GENPROP flags
        &dwPropFalse, sizeof(dwPropFalse),  // static data and size
        NULL                                // Handler
    },
    {   &GUID_DMUS_PROP_GS_Hardware,
        0,
        KSPROPERTY_SUPPORT_GET,
        GENPROP_F_STATIC,
        &dwPropFalse, sizeof(dwPropFalse),
        NULL
    },
    {   &GUID_DMUS_PROP_XG_Hardware,
        0,
        KSPROPERTY_SUPPORT_GET,
        GENPROP_F_STATIC,
        &dwPropFalse, sizeof(dwPropFalse),
        NULL
    },
    {   &GUID_DMUS_PROP_XG_Capable,
        0,
        KSPROPERTY_SUPPORT_GET,
        GENPROP_F_STATIC,
        &dwPropTrue, sizeof(dwPropTrue),
        NULL
    },
    {   &GUID_DMUS_PROP_GS_Capable,
        0,
        KSPROPERTY_SUPPORT_GET,
        GENPROP_F_STATIC,
        &dwPropTrue, sizeof(dwPropTrue),
        NULL
    },
    {   &GUID_DMUS_PROP_INSTRUMENT2,
        0,
        KSPROPERTY_SUPPORT_GET,
        GENPROP_F_STATIC,
        &dwPropTrue, sizeof(dwPropTrue),
        NULL
    },
    {
        &GUID_DMUS_PROP_DLS1,
        0,
        KSPROPERTY_SUPPORT_GET,
        GENPROP_F_STATIC,
        &dwPropTrue,  sizeof(dwPropTrue),
        NULL
    },
    {
        &GUID_DMUS_PROP_DLS2,
        0,
        KSPROPERTY_SUPPORT_GET,
        GENPROP_F_STATIC,
        &dwPropTrue,  sizeof(dwPropTrue),
        NULL
    },
    {
        &GUID_DMUS_PROP_SampleMemorySize,
        0,
        KSPROPERTY_SUPPORT_GET,
        GENPROP_F_STATIC,
        &dwSystemMemory,  sizeof(dwSystemMemory),
        NULL
    },
    {
        &KSPROPSETID_Synth,
        KSPROPERTY_SYNTH_VOLUME,
        KSPROPERTY_SUPPORT_SET,
        GENPROP_F_FNHANDLER,
        NULL, 0,
        CUserModeSynth::HandleSetVolume
    },
    {
        &KSPROPSETID_Synth,
        KSPROPERTY_SYNTH_VOLUMEBOOST,
        KSPROPERTY_SUPPORT_SET,
        GENPROP_F_FNHANDLER,
        NULL, 0,
        CUserModeSynth::HandleSetBoost
    },
    {
        &GUID_DMUS_PROP_WavesReverb,
        0,
        KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET,
        GENPROP_F_FNHANDLER,
        NULL, 0,
        CUserModeSynth::HandleReverb
    },
    {
        &GUID_DMUS_PROP_Effects,
        0,
        KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET,
        GENPROP_F_FNHANDLER,
        NULL, 0,
        CUserModeSynth::HandleEffects
    },
    {
        &GUID_DMUS_PROP_SamplePlaybackRate,
        0,
        KSPROPERTY_SUPPORT_GET,
        GENPROP_F_FNHANDLER,
        NULL, 0,
        CUserModeSynth::HandleGetSampleRate
    },
    {
        &GUID_Xbox_PROP_PCSynth,
        0,
        KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET,
        GENPROP_F_FNHANDLER,
        NULL, 0,
        CUserModeSynth::HandlePCSynth
    },
    {
        &GUID_Xbox_PROP_XboxSynth,
        0,
        KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET,
        GENPROP_F_FNHANDLER,
        NULL, 0,
        CUserModeSynth::HandleXboxSynth
    },
    {
        &GUID_Xbox_PROP_XboxAudioPath,
        0,
        KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET,
        GENPROP_F_FNHANDLER,
        NULL, 0,
        CUserModeSynth::HandleXboxPath
    },
	{
		&GUID_DMUS_PROP_WriteLatency,
        0,
        KSPROPERTY_SUPPORT_SET | KSPROPERTY_SUPPORT_GET,
        GENPROP_F_FNHANDLER,
        NULL, 0,
        CUserModeSynth::HandleLatency
    }
};

const int CUserModeSynth::m_nProperty = sizeof(m_aProperty) / sizeof(m_aProperty[0]);

HRESULT CUserModeSynth::HandlePCSynth(
        ULONG               ulId,
        BOOL                fSet,
        LPVOID              pbBuffer,
        PULONG              pcbBuffer)
{
    if (*pcbBuffer != sizeof(BOOL))
    {
        return E_INVALIDARG;
    }

	HRESULT hr = S_OK;

    if (fSet)
    {
		if (*(BOOL*)pbBuffer && m_fPCSynth)
		{
			hr = S_FALSE;
		}
		else if (!*(BOOL*)pbBuffer && !m_fPCSynth)
		{
			hr = S_FALSE;
		}
		else // m_fPCSynth will change
		{
			// If enabling PC Synth
			if( *(BOOL*)pbBuffer )
			{
				// Enable PC Synth
				m_fPCSynth = *(BOOL*)pbBuffer;

				// Activate the synth, if necessary
				if( m_fActive )
				{
					hr = ActivatePCSynth( TRUE );

					// If activation failed, reset m_fPCSynth
					if( FAILED(hr) )
					{
						m_fPCSynth = FALSE;
					}
				}
			}
			// If disabling PC Synth
			else
			{
				// Disable PC Synth

				// De-activate the synth, if necessary
				if( m_fPCActive )
				{
					hr = ActivatePCSynth( FALSE );
				}

				// If de-activation succeeded (or wasn't necessary), set m_fPCSynth
				if( SUCCEEDED(hr) )
				{
					m_fPCSynth = FALSE;
				}
			}
		}
    }
	else
	{
        *(BOOL*)pbBuffer = m_fPCSynth;
	}
    return hr;
}

HRESULT CUserModeSynth::HandleXboxSynth(
        ULONG               ulId,
        BOOL                fSet,
        LPVOID              pbBuffer,
        PULONG              pcbBuffer)
{
    if (*pcbBuffer != sizeof(BOOL))
    {
        return E_INVALIDARG;
    }

	HRESULT hr = S_OK;

    if (fSet)
    {
		if (*(BOOL*)pbBuffer && m_fXboxSynth)
		{
			hr = S_FALSE;
		}
		else if (!*(BOOL*)pbBuffer && !m_fXboxSynth)
		{
			hr = S_FALSE;
		}
		else // m_fXboxSynth will change
		{
			// If enabling Xbox Synth
			if( *(BOOL*)pbBuffer )
			{
				// Enable Xbox Synth
				m_fXboxSynth = *(BOOL*)pbBuffer;

				// Activate the synth, if necessary
				if( m_fActive )
				{
					hr = ActivateXboxSynth( TRUE );

					// If activation failed, reset m_fXboxSynth
					if( FAILED(hr) )
					{
						m_fXboxSynth = FALSE;
					}
				}
			}
			// If disabling Xbox Synth
			else
			{
				// Disable Xbox Synth

				// De-activate the synth, if necessary
				if( m_fXboxActive )
				{
					hr = ActivateXboxSynth( FALSE );
				}

				// If de-activation succeeded (or wasn't necessary), set m_fXboxSynth
				if( SUCCEEDED(hr) )
				{
					m_fXboxSynth = FALSE;
				}
			}
		}
    }
	else
	{
        *(BOOL*)pbBuffer = m_fXboxSynth;
	}

    return hr;
}

HRESULT CUserModeSynth::HandleXboxPath(
        ULONG               ulId,
        BOOL                fSet,
        LPVOID              pbBuffer,
        PULONG              pcbBuffer)
{
    if (*pcbBuffer != sizeof(BOOL))
    {
        return E_INVALIDARG;
    }

	HRESULT hr = S_OK;

    if (fSet)
    {
        DWORD *pdwNewPath = (DWORD *) pbBuffer;
        if (*pdwNewPath != m_dwXboxAudioPath)
        {
            ::EnterCriticalSection( &m_XboxCriticalSection );
	        if( m_pdmConnection && m_fXboxActive )
            {
                m_dwXboxAudioPath = *pdwNewPath;
		        char	szCmd[MAX_PATH];
		        char    szResp[MAX_PATH];
		        DWORD   cchResp = MAX_PATH;
		        sprintf(szCmd, TEXT(CMD_PREFIX "!Synth.SetDefaultAudiopath %d"), m_dwXboxAudioPath );
                DmSendCommand(m_pdmConnection, szCmd, szResp, &cchResp);
            }
            ::LeaveCriticalSection( &m_XboxCriticalSection );
        }
    }
	else
	{
        *(DWORD*)pbBuffer = m_dwXboxAudioPath;
	}

    return hr;
}

HRESULT CUserModeSynth::HandleLatency(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer)
{
    if (*pcbBuffer != sizeof(DWORD))
    {
        return E_INVALIDARG;
    }

    DWORD *pdwLatency = (DWORD *) pbBuffer;

	// If only the Xbox synth is active
    if (m_fXboxSynth && !m_fPCActive)
    {
		// Get or set its latency
        if (fSet)
        {
            m_XboxLatencyClock.SetXboxLatency(*pdwLatency * 10000I64);
        }
        else
        {
			*pdwLatency = DWORD(m_XboxLatencyClock.GetXboxLatency() / 10000I64);
        }
	    return S_OK;
    }

	// Otherwise, fail through to the synth sink
	return DMUS_E_UNKNOWN_PROPERTY;
}

HRESULT CUserModeSynth::HandleGetSampleRate(
        ULONG               ulId,
        BOOL                fSet,
        LPVOID              pbBuffer,
        PULONG              pcbBuffer)
{
    if (*pcbBuffer != sizeof(LONG))
    {
        return E_INVALIDARG;
    }
    if (!fSet)
    {
        *(long*)pbBuffer = m_dwSampleRate;
    }
    return S_OK;
}

HRESULT CUserModeSynth::HandleSetVolume(
        ULONG               ulId,
        BOOL                fSet,
        LPVOID              pbBuffer,
        PULONG              pcbBuffer)
{
    if (*pcbBuffer != sizeof(LONG))
    {
        return E_INVALIDARG;
    }

    m_lVolume =  *(LONG*)pbBuffer;
    m_lGainAdjust = m_lVolume + m_lBoost;

    if (m_pSynth)
    {
        m_pSynth->SetGainAdjust(m_lGainAdjust);
    }
    return S_OK;
}

HRESULT CUserModeSynth::HandleSetBoost(
        ULONG               ulId,
        BOOL                fSet,
        LPVOID              pbBuffer,
        PULONG              pcbBuffer)
{
    if (*pcbBuffer != sizeof(LONG))
    {
        return E_INVALIDARG;
    }

    m_lBoost =  *(LONG*)pbBuffer;
    m_lGainAdjust = m_lVolume + m_lBoost;

    if (m_pSynth)
    {
        m_pSynth->SetGainAdjust(m_lGainAdjust);
    }
    return S_OK;
}

HRESULT CUserModeSynth::HandleReverb(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer)
{
    DMUS_WAVES_REVERB_PARAMS *pParams;
    if (*pcbBuffer != sizeof(DMUS_WAVES_REVERB_PARAMS))
    {
        return E_INVALIDARG;
    }

    pParams = (DMUS_WAVES_REVERB_PARAMS *) pbBuffer;
    if (m_pSynth)
    {
        if (fSet)
        {
            m_pSynth->SetReverb(pParams);
        }
        else
        {
            m_pSynth->GetReverb(pParams);
        }
    }

    return S_OK;
}

HRESULT CUserModeSynth::HandleEffects(
    ULONG               ulId,
    BOOL                fSet,
    LPVOID              pbBuffer,
    PULONG              pcbBuffer)
{
    if (*pcbBuffer != sizeof(LONG))
    {
        return E_INVALIDARG;
    }
    if (fSet)
    {
        long lEffects = *(long*)pbBuffer;

        if (m_pSynth)
        {
            m_pSynth->SetReverbActive(lEffects & DMUS_EFFECT_REVERB);
        }
    }
    else
    {
        if (m_pSynth && m_pSynth->IsReverbActive())
        {
            *(long*)pbBuffer = DMUS_EFFECT_REVERB;
        }
        else
        {
            *(long*)pbBuffer = 0;
        }
    }
    return S_OK;
}

//
// CDirectMusicEmulatePort::FindPropertyItem
//
// Given a GUID and an item ID, find the associated property item in the synth's
// table of SYNPROPERTY's.
//
// Returns a pointer to the entry or NULL if the item was not found.
//
GENERICPROPERTY *CUserModeSynth::FindPropertyItem(REFGUID rguid, ULONG ulId)
{
    GENERICPROPERTY *pPropertyItem = &m_aProperty[0];
    GENERICPROPERTY *pEndOfItems = pPropertyItem + m_nProperty;

    // Special Case -- We don't support Waves Reverb on a SinthSink8
    if ((rguid == GUID_DMUS_PROP_WavesReverb) && (this->m_pSynthSink8 != NULL))
        return NULL;

    for (; pPropertyItem != pEndOfItems; pPropertyItem++)
    {
        if (*pPropertyItem->pguidPropertySet == rguid &&
             pPropertyItem->ulId == ulId)
        {
            return pPropertyItem;
        }
    }



    return NULL;
}

#define KS_VALID_FLAGS (KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_GET| KSPROPERTY_TYPE_BASICSUPPORT)

STDMETHODIMP CUserModeSynth::KsProperty(
    PKSPROPERTY pPropertyIn, ULONG ulPropertyLength,
    LPVOID pvPropertyData, ULONG ulDataLength,
    PULONG pulBytesReturned)
{
    V_INAME(DirectMusicSynthPort::IKsContol::KsProperty);
    V_BUFPTR_WRITE(pPropertyIn, ulPropertyLength);

    DWORD dwFlags = pPropertyIn->Flags & KS_VALID_FLAGS;

    switch (dwFlags)
    {
        case KSPROPERTY_TYPE_GET:
            V_BUFPTR_WRITE_OPT(pvPropertyData, ulDataLength);
            break;

        case KSPROPERTY_TYPE_SET:
            V_BUFPTR_READ(pvPropertyData, ulDataLength);
            break;

        case KSPROPERTY_TYPE_BASICSUPPORT:
            V_BUFPTR_WRITE(pvPropertyData, ulDataLength);
            break;
    }


    V_PTR_WRITE(pulBytesReturned, ULONG);

    GENERICPROPERTY *pProperty = FindPropertyItem(pPropertyIn->Set, pPropertyIn->Id);

    if (pProperty == NULL)
    {
        Trace(2, "Warning: KsProperty call requested unknown property.\n");
        return DMUS_E_UNKNOWN_PROPERTY;
    }

    switch (dwFlags)
    {
        case KSPROPERTY_TYPE_GET:
            if (!(pProperty->ulSupported & KSPROPERTY_SUPPORT_GET))
            {
                Trace(1, "Error: SynthSink does not support Get for the requested property.\n");
                return DMUS_E_GET_UNSUPPORTED;
            }

            if (pProperty->ulFlags & GENPROP_F_FNHANDLER)
            {
                GENPROPHANDLER pfn = pProperty->pfnHandler;
                *pulBytesReturned = ulDataLength;
                return (this->*pfn)(pPropertyIn->Id, FALSE, pvPropertyData, pulBytesReturned);
            }

            if (ulDataLength > pProperty->cbPropertyData)
            {
                ulDataLength = pProperty->cbPropertyData;
            }

            if (pvPropertyData != NULL)
            {
                CopyMemory(pvPropertyData, pProperty->pPropertyData, ulDataLength);
            }
            *pulBytesReturned = ulDataLength;

            return S_OK;

        case KSPROPERTY_TYPE_SET:
            if (!(pProperty->ulSupported & KSPROPERTY_SUPPORT_SET))
            {
                Trace(1, "Error: SynthSink does not support Set for the requested property.\n");
                return DMUS_E_SET_UNSUPPORTED;
            }

            if (pProperty->ulFlags & GENPROP_F_FNHANDLER)
            {
                GENPROPHANDLER pfn = pProperty->pfnHandler;
                return (this->*pfn)(pPropertyIn->Id, TRUE, pvPropertyData, &ulDataLength);
            }

            if (ulDataLength > pProperty->cbPropertyData)
            {
                ulDataLength = pProperty->cbPropertyData;
            }

            CopyMemory(pProperty->pPropertyData, pvPropertyData, ulDataLength);

            return S_OK;


        case KSPROPERTY_TYPE_BASICSUPPORT:
            if (pProperty == NULL)
            {
                Trace(1, "Error: Synth does not provide support for requested property type.\n");
                return DMUS_E_UNKNOWN_PROPERTY;
            }

            // XXX Find out what convention is for this!!
            //
            if (ulDataLength < sizeof(DWORD))
            {
                Trace(1, "Error: Data size for property is too small.\n");
                return E_INVALIDARG;
            }

            *(LPDWORD)pvPropertyData = pProperty->ulSupported;
            *pulBytesReturned = sizeof(DWORD);

            return S_OK;
    }

    Trace(1, "Error: KSProperty Flags must contain one of: %s\n"
              "\tKSPROPERTY_TYPE_SET, KSPROPERTY_TYPE_GET, or KSPROPERTY_TYPE_BASICSUPPORT\n");
    return E_INVALIDARG;
}

STDMETHODIMP CUserModeSynth::KsMethod(
    PKSMETHOD pMethod, ULONG ulMethodLength,
    LPVOID pvMethodData, ULONG ulDataLength,
    PULONG pulBytesReturned)
{
    V_INAME(DirectMusicSynth::IKsContol::KsMethod);
    V_BUFPTR_WRITE(pMethod, ulMethodLength);
    V_BUFPTR_WRITE_OPT(pvMethodData, ulDataLength);
    V_PTR_WRITE(pulBytesReturned, ULONG);

    return DMUS_E_UNKNOWN_PROPERTY;
}
STDMETHODIMP CUserModeSynth::KsEvent(
    PKSEVENT pEvent, ULONG ulEventLength,
    LPVOID pvEventData, ULONG ulDataLength,
    PULONG pulBytesReturned)
{
    V_INAME(DirectMusicSynthPort::IKsContol::KsEvent);
    V_BUFPTR_WRITE(pEvent, ulEventLength);
    V_BUFPTR_WRITE_OPT(pvEventData, ulDataLength);
    V_PTR_WRITE(pulBytesReturned, ULONG);

    return DMUS_E_UNKNOWN_PROPERTY;
}

/////////////////////////////////////////////////////////////////////
// Implementation of IDirectMusicSynth8

#ifdef IMPLEMENT_SYNTH8
STDMETHODIMP CUserModeSynth::PlayVoice(REFERENCE_TIME rt, DWORD dwVoiceId, DWORD dwChannelGroup, DWORD dwChannel, DWORD dwDLId, PREL prPitch, VREL vrVolume, SAMPLE_TIME stVoiceStart, SAMPLE_TIME stLoopStart, SAMPLE_TIME stLoopEnd )
{
    HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;

    ::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {
        hr = m_pSynth->PlayVoice(m_pSynthSink8,
                                 rt,
                                 dwVoiceId,
                                 dwChannelGroup,
                                 dwChannel,
                                 dwDLId,
                                 vrVolume,
                                 prPitch,
                                 stVoiceStart,
                                 stLoopStart,
                                 stLoopEnd);
    }
    else
    {
        Trace(1, "Error: Failed wave playback, synth is not properly configured.\n");
    }
    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CUserModeSynth::StopVoice(REFERENCE_TIME rt, DWORD dwVoiceId )
{
    HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;

    ::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {
        hr = m_pSynth->StopVoice(m_pSynthSink8,
                                 rt,
                                 dwVoiceId);
    }
    else
    {
        Trace(1, "Error: Failed stop of wave playback, synth is not properly configured.\n");
    }
    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CUserModeSynth::GetVoiceState(DWORD dwVoice[], DWORD cbVoice, DMUS_VOICE_STATE VoiceState[] )
{
    V_INAME(IDirectMusicSynth::GetVoiceState);
    V_PTR_READ(dwVoice, sizeof(DWORD)*cbVoice);

    HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;

    ::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {

        hr = m_pSynth->GetVoiceState(dwVoice,
                                     cbVoice,
                                     VoiceState);

    }
    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CUserModeSynth::Refresh(DWORD dwDownloadID, DWORD dwFlags )
{
    HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;

    ::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {
        hr = m_pSynth->Refresh(dwDownloadID,
                               dwFlags);
    }
    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CUserModeSynth::AssignChannelToBuses(DWORD dwChannelGroup, DWORD dwChannel, LPDWORD pdwBuses, DWORD cBuses )
{
    HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;

    ::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {
    hr = m_pSynth->AssignChannelToBuses(dwChannelGroup,
                                         dwChannel,
                                         pdwBuses,
                                         cBuses);
    }
    else
    {
        Trace(1, "Error: Failed synth channel assignment, synth is not properly configured.\n");
    }
    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}
#endif

/////////////////////////////////////////////////////////////////////
// Implementation of IDirectSoundSource

STDMETHODIMP CUserModeSynth::SetSink(IDirectSoundConnect* pSinkConnect)
{
    V_INAME(IDirectSoundSink::SetSink);
    V_INTERFACE_OPT(pSinkConnect);

    HRESULT hr = S_OK;

    LPVOID ptr = NULL;
    V_BUFPTR_WRITE_OPT(ptr, 0);

    ::EnterCriticalSection(&m_CriticalSection);

//>>>>>>>> RELEASE THE DSLINK IF PRESENT !!!!

// FIXME: The calls into the SynthSink8 may require the DSound DLL Mutex.  If the Sink
// is making a a call to READ then we end up in a deadlock.  We need to be sure that the
// Synth isn't playing when we do this.

    if (m_pSynthSink8)
    {
        // FIXME: whoever called us->SetSink() should previously have called
        // pOldSink->RemoveSource(us) - it shouldn't be our responsibility to
        // do this call (??):
        // m_pSynthSink8->RemoveSource(this);
        m_pSynthSink8->Release();
        m_pSynthSink8 = NULL;
    }

    if (pSinkConnect)
    {
        // Obtain the IDirectSoundSynthSink interface on the sink
        hr = pSinkConnect->QueryInterface(IID_IDirectSoundSynthSink, (void**)&m_pSynthSink8);

        if (SUCCEEDED(hr))
        {
            //
            // Get the sink's format and validate it
            //
            WAVEFORMATEX wfx;
            DWORD dwSize = sizeof wfx;
            hr = m_pSynthSink8->GetFormat(&wfx, dwSize, NULL);
            if (SUCCEEDED(hr) && wfx.wBitsPerSample != 16 )
            {
                Trace(1, "Error; Synth can not write to any format other than 16 bit PCM.\n");
                hr = DMUS_E_WAVEFORMATNOTSUPPORTED;
            }

            if (SUCCEEDED(hr))
            {
                // Flag the buffer format to be non-interleaved
                m_dwChannels = 1;    // This synth with a sink is concidered a mono source.
                m_dwBufferFlags = BUFFERFLAG_MULTIBUFFER;

                if (m_pSynth)
                {
                    m_pSynth->SetStereoMode(m_dwBufferFlags);

                    // reset sample rate if it has changed
                    if (wfx.nSamplesPerSec != (WORD)m_dwSampleRate)
                    {
                        m_pSynth->SetSampleRate(wfx.nSamplesPerSec);
                    }

                    // disable DX7 Reverb
                    m_pSynth->SetReverbActive(FALSE);
                }
            }
        }
    }

    ::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CUserModeSynth::Seek(ULONGLONG sp)
{
    m_ullPosition = sp/2;    // Convert from bytes to samples

    return S_OK;
}

STDMETHODIMP CUserModeSynth::Read(LPVOID *ppvBuffer, LPDWORD pdwIDs, LPDWORD pdwFuncIDs, LPLONG plPitchBends, DWORD dwBufferCount, PULONGLONG pullLength )
{
    V_INAME(IDirectMusicSynth::Read);
    V_PTR_READ(ppvBuffer, sizeof(LPVOID)*dwBufferCount);
    V_PTR_READ(pdwIDs, sizeof(LPDWORD)*dwBufferCount);

    for ( DWORD i = 0; i < dwBufferCount; i++ )
    {
        V_BUFPTR_WRITE(ppvBuffer[i], (DWORD)*pullLength);
        if ( ppvBuffer[i] == NULL )
        {
            Trace(1, "Error: Read called with NULL buffer.\n");
            return E_INVALIDARG;
        }
    }

    if ( *pullLength > 0x00000000FFFFFFFF )    // can't read more than a DWORD's worth of data
    {
        Trace(1, "Error: Read called with invalid buffer length.\n");
        return E_INVALIDARG;
    }

    if ( dwBufferCount == 0 )                // don't read no buffers
    {
        Trace(4, "Warning: Read called with 0 buffers.\n");
        return E_INVALIDARG;
    }
    if (!m_pSynthSink8)
    {
        Trace(1, "Error: Synth is not configured, can not play.\n");
        return DMUS_E_SYNTHNOTCONFIGURED;
    }
    if (!m_fPCActive)
    {
        Trace(3, "Warning: Synth is not active, can not play.\n");
        return DMUS_E_SYNTHINACTIVE;
    }

    ::EnterCriticalSection(&m_CriticalSection);

    if (m_pSynth)
    {
        // Mix
        DWORD dwLength = (DWORD)(*pullLength)/2;    // Convert from bytes to number of samples. Synth assumes 16 bit
        m_pSynth->Mix((short**)ppvBuffer, pdwIDs, pdwFuncIDs, plPitchBends, dwBufferCount, m_dwBufferFlags, dwLength, m_ullPosition);

        // Increment current sample position in the audio stream
        m_ullPosition += dwLength;
    }

    ::LeaveCriticalSection(&m_CriticalSection);

    return S_OK;
}

STDMETHODIMP CUserModeSynth::GetSize(PULONGLONG pcb)
{
    return E_NOTIMPL;
}



/////////////////////////////////////////////////////////////////////////////
// CXboxLatencyClock

CXboxLatencyClock::CXboxLatencyClock()
{
	m_cRef = 1;
	m_pSinkLatencyClock = NULL;
	m_pMasterClock = NULL;
	m_rtXboxLatency = 0;
}

CXboxLatencyClock::~CXboxLatencyClock()
{
	if( m_pSinkLatencyClock )
	{
		m_pSinkLatencyClock->Release();
		m_pSinkLatencyClock = NULL;
	}
	if( m_pMasterClock )
	{
		m_pMasterClock->Release();
		m_pMasterClock = NULL;
	}
}

// CXboxLatencyClock::QueryInterface
//
STDMETHODIMP
CXboxLatencyClock::QueryInterface(const IID &iid, void **ppv)
{
    V_INAME(IReferenceClock::QueryInterface);
    V_REFGUID(iid);
    V_PTRPTR_WRITE(ppv);

    if (iid == IID_IUnknown || iid == IID_IReferenceClock) {
        *ppv = static_cast<IReferenceClock*>(this);
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    reinterpret_cast<IUnknown*>(this)->AddRef();
    return S_OK;
}

// CXboxLatencyClock::AddRef
//
STDMETHODIMP_(ULONG)
CXboxLatencyClock::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// CXboxLatencyClock::Release
//
STDMETHODIMP_(ULONG)
CXboxLatencyClock::Release()
{
    if (!InterlockedDecrement(&m_cRef)) {
        delete this;
        return 0;
    }

    return m_cRef;
}

// CXboxLatencyClock::GetTime
//
STDMETHODIMP
CXboxLatencyClock::GetTime(REFERENCE_TIME *pTime)
{
	if( m_pSinkLatencyClock )
	{
		return m_pSinkLatencyClock->GetTime(pTime);
	}
	else if( m_pMasterClock )
	{
		HRESULT hr = m_pMasterClock->GetTime(pTime);
		if( SUCCEEDED(hr) )
		{
			*pTime += m_rtXboxLatency;
		}
		return hr;
	}
	return E_NOTIMPL;
}

// CXboxLatencyClock::AdviseTime
//
STDMETHODIMP
CXboxLatencyClock::AdviseTime(REFERENCE_TIME baseTime, REFERENCE_TIME streamTime, HANDLE hEvent, DWORD *pdwAdviseCookie)
{
	if( m_pSinkLatencyClock )
	{
		return m_pSinkLatencyClock->AdviseTime(baseTime, streamTime, hEvent, pdwAdviseCookie);
	}
	return E_NOTIMPL;
}

// CXboxLatencyClock::AdvisePeriodic
//
STDMETHODIMP
CXboxLatencyClock::AdvisePeriodic(REFERENCE_TIME startTime, REFERENCE_TIME periodTime, HANDLE hSemaphore, DWORD *pdwAdviseCookie)
{
	if( m_pSinkLatencyClock )
	{
		return m_pSinkLatencyClock->AdvisePeriodic(startTime, periodTime, hSemaphore, pdwAdviseCookie);
	}
	return E_NOTIMPL;
}

// CXboxLatencyClock::Unadvise
//
STDMETHODIMP
CXboxLatencyClock::Unadvise(DWORD dwAdviseCookie)
{
	if( m_pSinkLatencyClock )
	{
		return m_pSinkLatencyClock->Unadvise(dwAdviseCookie);
	}
	return E_NOTIMPL;
}

// CXboxLatencyClock::SetXboxLatency
//
void CXboxLatencyClock::SetXboxLatency( REFERENCE_TIME rtLatency )
{
	m_rtXboxLatency = rtLatency;
}

// CXboxLatencyClock::SetSinkLatencyClock
//
void CXboxLatencyClock::SetSinkLatencyClock( IReferenceClock *pIReferenceClock )
{
	if( m_pSinkLatencyClock )
	{
		m_pSinkLatencyClock->Release();
		m_pSinkLatencyClock = NULL;
	}

	m_pSinkLatencyClock = pIReferenceClock;
	if( m_pSinkLatencyClock )
	{
		m_pSinkLatencyClock->AddRef();
	}
}

// CXboxLatencyClock::SetMasterClock
//
void CXboxLatencyClock::SetMasterClock( IReferenceClock *pIReferenceClock )
{
	if( m_pMasterClock )
	{
		m_pMasterClock->Release();
		m_pMasterClock = NULL;
	}

	m_pMasterClock = pIReferenceClock;
	if( m_pMasterClock )
	{
		m_pMasterClock->AddRef();
	}
}
