//      Copyright (c) 1996-1999 Microsoft Corporation

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
// UMSynth.cpp : Implementation of CUserModeSynth

#ifdef XBOX
#include <xtl.h>
#include <xboxutil.h>
#define COM_NO_WINDOWS_H
#define RPC_NO_WINDOWS_H
#define _WINGDI_
#endif

#include <objbase.h>
#include <mmsystem.h>
#ifdef XBOX
#undef timeGetTime
#define timeGetTime GetTickCount
#endif // XBOX
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

#include <dmusprop.h>

#include "..\shared\validate.h"

/*DWORD GetTheCurrentTime()
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
}*/

/////////////////////////////////////////////////////////////////////////////
// CUserModeSynth

HRESULT CUserModeSynth::Init()

{
	return S_OK;
}

CUserModeSynth::CUserModeSynth()

{
    ::InitializeCriticalSection(&m_CriticalSection);
    m_MasterClock.m_pParent = this;
	m_cRef = 0;
    m_lVolume = 0;
    m_lBoost = 6 * 100;
    m_lGainAdjust = 6 * 100;            // Default 6 dB boost
	m_fActive = FALSE;
    m_pSynth = NULL;
}

CUserModeSynth::~CUserModeSynth()

{
//	Activate(FALSE);

    ::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {
        delete m_pSynth;
    	m_pSynth = NULL;
    }
    ::LeaveCriticalSection(&m_CriticalSection);
    ::DeleteCriticalSection(&m_CriticalSection);
}


// CUserModeSynth::QueryInterface
//
STDMETHODIMP
CUserModeSynth::QueryInterface(const IID &iid,
                                   void **ppv)
{
    V_INAME(IDirectMusicSynth::QueryInterface);
    V_REFGUID(iid);
    V_PTRPTR_WRITE(ppv);

    if (iid == IID_IUnknown || iid == IID_IDirectMusicSynthX) {
        *ppv = static_cast<IDirectMusicSynthX*>(this);
    }
	else if (iid == IID_IKsControl) 
    {
        *ppv = static_cast<IKsControl*>(this);
    }
/*	else if (iid == IID_IDirectSoundSource)
    {
        *ppv = static_cast<IDirectSoundSource*>(this);
	}*/
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

/*STDMETHODIMP CUserModeSynth::SetSynthSink(
	IDirectMusicSynthSink *pSynthSink)	// <i IDirectMusicSynthSink> to connect to synth, or
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
	m_dwBufferFlags &= ~BUFFERFLAG_MULTIBUFFER;	// .... just in case 

	if (m_pSynthSink)
	{
		m_pSynthSink->AddRef();
//		hr = m_pSynthSink->Init(static_cast<IDirectMusicSynthX*>(this));
	}
	::LeaveCriticalSection(&m_CriticalSection);
	
    return hr;
}*/

STDMETHODIMP CUserModeSynth::Open(	
	LPDMUS_PORTPARAMS pPortParams)		// <t DMUS_PORTPARAMS> structure for opening the port. If NULL, default settings are used.
{
	V_INAME(IDirectMusicSynth::Open);
    //if (pPortParams == NULL)
	//{
    //    Trace(1,"Error: Open called with NULL PortParams.\n");
	//	return E_FAIL;
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
	myParams.dwEffectFlags = DMUS_EFFECT_NONE;
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

			myParams.dwEffectFlags = DMUS_EFFECT_NONE; 
			if (pPortParams->dwEffectFlags & DMUS_EFFECT_REVERB)
			{
				bPartialOpen = true;
			}
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

//    m_dwSampleRate = 48000;
//    m_dwChannels = 2;
//	m_dwBufferFlags = (m_dwChannels==1)?BUFFERFLAG_MONO:BUFFERFLAG_INTERLEAVED;

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
			// Note: hr is initalized to DMUS_E_ALREADYOPEN,
			// so we can't do the typical "if(SUCCEEDED(hr))" here.
			hr = m_pSynth->Initialize();

			if (SUCCEEDED(hr))
			{
				hr = m_pSynth->Open(myParams.dwChannelGroups,
					myParams.dwVoices);
			}
			if (SUCCEEDED(hr))
			{
                m_pSynth->SetGainAdjust(m_lGainAdjust);
                m_pSynth->SetMasterClock(&m_MasterClock);
//                m_pSynth->Activate(m_dwSampleRate, m_dwBufferFlags);
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
	DWORD dwGroups)		// Number of ChannelGroups requested.

{
	::EnterCriticalSection(&m_CriticalSection);
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
	if (m_pSynth)
	{
		hr = m_pSynth->SetNumChannelGroups(dwGroups);
	}
	::LeaveCriticalSection(&m_CriticalSection);
	return hr;
}

STDMETHODIMP CUserModeSynth::SetMasterVolume(LONG lVolume)

{
	::EnterCriticalSection(&m_CriticalSection);
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
    m_lVolume =  lVolume;
    m_lGainAdjust = m_lVolume + m_lBoost;

    if (m_pSynth)
    {
        m_pSynth->SetGainAdjust(m_lGainAdjust);
    }	
    ::LeaveCriticalSection(&m_CriticalSection);
	return hr;
}


STDMETHODIMP CUserModeSynth::SendShortMsg(REFERENCE_TIME rt, DWORD dwGroup, DWORD dwMsg)
{
	::EnterCriticalSection(&m_CriticalSection);
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
	if (m_pSynth)
	{
		hr = m_pSynth->SendShortMsg(rt,dwGroup,dwMsg);
	}
	::LeaveCriticalSection(&m_CriticalSection);
	return hr;
}



STDMETHODIMP CUserModeSynth::SendLongMsg(REFERENCE_TIME rt, DWORD dwGroup, BYTE *pbMsg, DWORD dwLength)
{
	::EnterCriticalSection(&m_CriticalSection);
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
	if (m_pSynth)
	{
		hr = m_pSynth->SendLongMsg(rt,dwGroup,pbMsg,dwLength);
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
/*STDMETHODIMP CUserModeSynth::Download(
	LPHANDLE phDownload,	// Pointer to download handle, to be created by <om IDirectMusicSynth::Download> and used later to unload the data.
	LPVOID pvData,			// Pointer to continuous memory segment with download data.
	LPBOOL pbFree)			// <p pbFree> indicates whether the synthesizer wishes to keep the memory in <p pvData> allocated.
{
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
	V_INAME(IDirectMusicSynth::Download);
	V_PTR_WRITE(phDownload,HANDLE); 
	V_PTR_WRITE(pbFree,BOOL); 

	// pvData is validated inside synth while parsing.
	::EnterCriticalSection(&m_CriticalSection);
	if (m_pSynth)
	{
		hr = m_pSynth->Download(phDownload, pvData, pbFree);
	}
	::LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

STDMETHODIMP CUserModeSynth::Unload(
	HANDLE hDownload,	// Handle to data, previously downloaded with a call to <om IDirectMusicSynth::Download>.
	HRESULT ( CALLBACK *lpFreeHandle)(HANDLE,HANDLE), // If the original call to
						// <om IDirectMusicSynth::Download> returned FALSE in <p pbFree>,
						// the synthesizer hung onto the memory in the download chunk. If so,
						// the caller must be notified once the memory has been freed,
						// but that could occur later than <om IDirectMusicSynth::Download>
						// since a wave might be currently in use. <p lpFreeHandle> is a 
						// pointer to a callback
						// function which will be called when the memory is no longer in use.
	HANDLE hUserData)	// Pointer to user data, passed as a parameter to the 
						// <p lpFreeHandle> function, typically used so the callback routine can retrieve
						// its state.
{
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;
	::EnterCriticalSection(&m_CriticalSection);
	if (m_pSynth)
	{
		hr = m_pSynth->Unload(hDownload, lpFreeHandle, hUserData);
	}
	::LeaveCriticalSection(&m_CriticalSection);
	return hr;
}

STDMETHODIMP CUserModeSynth::PlayBuffer(
	REFERENCE_TIME rt,	// Start time of the buffer. This should be in 
						// REFERENCE_TIME units, relative to the master
						// clock, previously set with a call to <om IDirectMusicSynth::SetMasterClock>.
						// And, this should be after the time returned by the clock in 
						// <om IDirectMusicSynth::GetLatencyClock>.
	LPBYTE pbBuffer,	// Memory chunk with all the MIDI events, generated by <i IDirectMusicBuffer>.
	DWORD cbBuffer)		// Size of buffer.
{
	class MIDIEVENT : public DMUS_EVENTHEADER {
	public:
		 BYTE  abEvent[4];           
	};

	typedef class MIDIEVENT FAR  *LPMIDIEVENT;
	#define QWORD_ALIGN(x) (((x) + 7) & ~7)

	HRESULT hr;

    V_INAME(IDirectMusicSynth::PlayBuffer);
    V_BUFPTR_READ(pbBuffer,cbBuffer);

	::EnterCriticalSection(&m_CriticalSection);

    LPMIDIEVENT lpEventHdr;
    DWORD cbEvent;

    while (cbBuffer)
    {
        if (cbBuffer < sizeof(DMUS_EVENTHEADER))
        {
            Trace(1,"Error: PlayBuffer called with error in buffer size.\n");
			::LeaveCriticalSection(&m_CriticalSection);
            return E_INVALIDARG;
        }

        lpEventHdr = (LPMIDIEVENT)pbBuffer; 
        cbEvent = DMUS_EVENT_SIZE(lpEventHdr->cbEvent);
        if (cbEvent > cbBuffer)
        {
            Trace(1,"Error: PlayBuffer called with error in event size.\n");
			::LeaveCriticalSection(&m_CriticalSection);
            return E_INVALIDARG;
        }
        
        pbBuffer += cbEvent;
        cbBuffer -= cbEvent;
		{
			hr = m_pSynth->PlayBuffer(rt + lpEventHdr->rtDelta, 
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
*/


STDMETHODIMP CUserModeSynth::GetMasterClock(
	IReferenceClock **ppClock)	// Pointer to master <i IReferenceClock>, 
								// used by all devices in current instance of DirectMusic.

{
    return m_MasterClock.QueryInterface(IID_IReferenceClock,(void **) ppClock);
}

STDMETHODIMP CUserModeSynth::SetChannelPriority(
    DWORD dwChannelGroup, 
    DWORD dwChannel, 
    DWORD dwPriority)
{
    if (m_pSynth)
    {
        return m_pSynth->SetChannelPriority(dwChannelGroup,dwChannel,dwPriority);
    }
    Trace(1,"Error: Synth not initialized.\n");
    return E_FAIL;
}

STDMETHODIMP CUserModeSynth::GetChannelPriority(
    DWORD dwChannelGroup, 
    DWORD dwChannel, 
    LPDWORD pdwPriority)
{
    if (m_pSynth)
    {
        return m_pSynth->GetChannelPriority(dwChannelGroup,dwChannel,pdwPriority);
    }
    Trace(1,"Error: Synth not initialized.\n");
    return E_FAIL;
}

static DWORD dwPropFalse = FALSE;
static DWORD dwPropTrue  = TRUE;
static DWORD dwSystemMemory = DMUS_PC_SYSTEMMEMORY;

GENERICPROPERTY CUserModeSynth::m_aProperty[] = 
{      
    { 
        &GUID_DMUS_PROP_GM_Hardware,		// Set
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
        KSPROPERTY_SYNTH_VOLUMEBOOST,
        KSPROPERTY_SUPPORT_SET,
        GENPROP_F_FNHANDLER,
        NULL, 0,
        CUserModeSynth::HandleSetBoost
    },
};

const int CUserModeSynth::m_nProperty = sizeof(m_aProperty) / sizeof(m_aProperty[0]);

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
        Trace(2,"Warning: KsProperty call requested unknown property.\n");
        return DMUS_E_UNKNOWN_PROPERTY;
    }

    switch (dwFlags)
    {
        case KSPROPERTY_TYPE_GET:
            if (!(pProperty->ulSupported & KSPROPERTY_SUPPORT_GET))
            {
                Trace(1,"Error: SynthSink does not support Get for the requested property.\n");
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
                Trace(1,"Error: SynthSink does not support Set for the requested property.\n");
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
                Trace(1,"Error: Synth does not provide support for requested property type.\n");
                return DMUS_E_UNKNOWN_PROPERTY;
            }

            // XXX Find out what convention is for this!!
            //
            if (ulDataLength < sizeof(DWORD))
            {
                Trace(1,"Error: Data size for property is too small.\n");
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
// Implementation of IDirectMusicSynthX

STDMETHODIMP CUserModeSynth::PlayWave(REFERENCE_TIME rt, 
                                      DWORD dwChannelGroup, 
                                      DWORD dwChannel, 
                                      PREL prPitch, 
                                      VREL vrVolume, 
                                      SAMPLE_TIME stVoiceStart, 
                                      SAMPLE_TIME stLoopStart, 
                                      SAMPLE_TIME stLoopEnd,
                                      IDirectSoundWave *pIWave,
                                      DWORD *pdwVoiceID)
{
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;

	::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {
        hr = m_pSynth->PlayWave(rt,dwChannelGroup, 
								 dwChannel,
								 vrVolume,
								 prPitch,
								 stVoiceStart,
								 stLoopStart,
								 stLoopEnd,
                                 pIWave,
                                 pdwVoiceID);
    }
    else
    {
        Trace(1,"Error: Failed wave playback, synth is not properly configured.\n");
    }
	::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

STDMETHODIMP CUserModeSynth::StopWave(REFERENCE_TIME rt, DWORD dwVoiceId )
{
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;

	::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {
        hr = m_pSynth->StopWave(rt, 
								 dwVoiceId); 
    }
    else
    {
        Trace(1,"Error: Failed stop of wave playback, synth is not properly configured.\n");
    }
	::LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

/*STDMETHODIMP CUserModeSynth::GetVoiceState(DWORD dwVoice[], DWORD cbVoice, DMUS_VOICE_STATE VoiceState[] )
{
	V_INAME(IDirectMusicSynth::GetVoiceState);
	V_PTR_READ(dwVoice,sizeof(DWORD)*cbVoice);

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

#ifdef XBOX
STDMETHODIMP CUserModeSynth::IsReadyForData(DWORD dwDownloadId)
{
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;

	::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {
        hr = m_pSynth->IsReadyForData(dwDownloadId);
    }
	::LeaveCriticalSection(&m_CriticalSection);

	return hr;
}
#endif
*/

#ifndef XBOX
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
        Trace(1,"Error: Failed synth channel assignment, synth is not properly configured.\n");
    }
	::LeaveCriticalSection(&m_CriticalSection);

    return hr;
}                                           
#else
STDMETHODIMP CUserModeSynth::AssignChannelToOutput(DWORD dwChannelGroup, DWORD dwChannel, IDirectSoundBuffer *pBuffer,DWORD dwMixBins, BYTE *pbControllers  )
{
	HRESULT hr = DMUS_E_SYNTHNOTCONFIGURED;

	::EnterCriticalSection(&m_CriticalSection);
    if (m_pSynth)
    {
	hr = m_pSynth->AssignChannelToOutput(dwChannelGroup, 
									     dwChannel, 
						                 pBuffer, 
						                 dwMixBins,
                                         pbControllers);
	}
    else
    {
        Trace(1,"Error: Failed synth channel assignment, synth is not properly configured.\n");
    }
	::LeaveCriticalSection(&m_CriticalSection);

    return hr;
} 

#endif
/////////////////////////////////////////////////////////////////////
// Implementation of IDirectSoundSource

/*STDMETHODIMP CUserModeSynth::SetSink(IDirectSoundConnect* pSinkConnect)
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
                Trace(1,"Error; Synth can not write to any format other than 16 bit PCM.\n");
    			hr = DMUS_E_WAVEFORMATNOTSUPPORTED;
    		}

    		if (SUCCEEDED(hr))
    		{
    			// Flag the buffer format to be non-interleaved 
    			m_dwChannels = 1;	// This synth with a sink is concidered a mono source.
    			m_dwBufferFlags = BUFFERFLAG_MULTIBUFFER;	

    			if (m_pSynth)	
    			{
    				// reset sample rate if it has changed 
    				if (wfx.nSamplesPerSec != (WORD)m_dwSampleRate)
    				{
//    					m_pSynth->SetSampleRate(wfx.nSamplesPerSec);
    				}

    			}
    		}
    	}
    }

    ::LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

STDMETHODIMP CUserModeSynth::Seek(ULONGLONG sp)
{

	m_ullPosition = sp/2;	//Convert from bytes to samples 

    return S_OK;
}

STDMETHODIMP CUserModeSynth::Read(LPVOID *ppvBuffer, LPDWORD pdwIDs, LPDWORD pdwFuncIDs, LPLONG plPitchBends, DWORD dwBufferCount, PULONGLONG pullLength )
{
	V_INAME(IDirectMusicSynth::Read);
	V_PTR_READ(ppvBuffer,sizeof(LPVOID)*dwBufferCount);
	V_PTR_READ(pdwIDs,sizeof(LPDWORD)*dwBufferCount);
	for ( DWORD i = 0; i < dwBufferCount; i++ )
	{
		V_BUFPTR_WRITE(ppvBuffer[i],(DWORD)*pullLength);
		if ( ppvBuffer[i] == NULL )
		{
            Trace(1,"Error: Read called with NULL buffer.\n");
			return E_INVALIDARG;
		}
	}

	if ( *pullLength > 0x00000000FFFFFFFF )	// can't read more than a DWORD's worth of data
	{
        Trace(1,"Error: Read called with invalid buffer length.\n");
		return E_INVALIDARG;
	}

	if ( dwBufferCount == 0 )				// don't read no buffers
	{
        Trace(4,"Warning: Read called with 0 buffers.\n");
		return E_INVALIDARG;
	}
	if (!m_pSynthSink8)
	{
        Trace(1,"Error: Synth is not configured, can not play.\n");
		return DMUS_E_SYNTHNOTCONFIGURED;
	}
    if (!m_fActive)
    {
        Trace(3,"Warning: Synth is not active, can not play.\n");
        return DMUS_E_SYNTHINACTIVE;
    }

    ::EnterCriticalSection(&m_CriticalSection);

	if (m_pSynth)
	{
		// Mix
		DWORD dwLength = (DWORD)(*pullLength)/2;	// Convert from bytes to number of samples. Synth assumes 16 bit
		m_pSynth->Mix((short**)ppvBuffer, pdwIDs, pdwFuncIDs, plPitchBends, dwBufferCount, m_dwBufferFlags, dwLength, m_ullPosition);

		// Increment current sample position in the audio stream
		m_ullPosition += dwLength; //*pullLength;
	}

    ::LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

STDMETHODIMP CUserModeSynth::GetSize(PULONGLONG pcb)
{
    return E_NOTIMPL;
}
*/

CClock::CClock()

{
    m_pParent = NULL;
    m_dwLastPosition = 0;
    m_llSampleTime = 0;
}


STDMETHODIMP CClock::QueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUnknown || iid == IID_IReferenceClock)
    {
        *ppv = static_cast<IReferenceClock*>(this);
    } 
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    
    return S_OK;
}

STDMETHODIMP_(ULONG) CClock::AddRef()
{
    return m_pParent->AddRef();
}

STDMETHODIMP_(ULONG) CClock::Release()
{
    return m_pParent->Release();
}

STDMETHODIMP CClock::AdviseTime(REFERENCE_TIME ,REFERENCE_TIME,HANDLE,DWORD *)
{
    return E_NOTIMPL;
}

STDMETHODIMP CClock::AdvisePeriodic(REFERENCE_TIME,REFERENCE_TIME,HANDLE,DWORD * )
{
    return E_NOTIMPL;
}

STDMETHODIMP CClock::Unadvise( DWORD )
{
    return E_NOTIMPL;
}

EXTERN_C DWORD WINAPI DirectSoundGetSampleTime(void);

STDMETHODIMP CClock::GetTime(LPREFERENCE_TIME pTime)
{
    if( pTime == NULL )
    {
        return E_INVALIDARG;
    }

	EnterCriticalSection( &m_pParent->m_CriticalSection );

    DWORD dwPosition = DirectSoundGetSampleTime();

    // Check to see if we looped around, which happens every 24 hours

    LONGLONG delta;
    if(dwPosition < m_dwLastPosition){
        // This is either a loop, or a dsound bug

        if(dwPosition < 100000 && m_dwLastPosition > (1 << 30) ){
            Trace(-1, "CClock::GetTime sample time wrapped (as it should every 24 hours. Was: %u is: %u\n",
                m_dwLastPosition, dwPosition);
            delta = ((((LONGLONG) 1) << 32) + dwPosition) - m_dwLastPosition;
        }
        else {
            Trace(-1, "CClock::GetTime stream position decreased unexpectedly. Was: %u is: %u\n",
                m_dwLastPosition, dwPosition);
            delta = 0; // Ignore.
            m_dwLastPosition = dwPosition; // Catch up. (Sometimes DSound resets the stream on us.)
        }
    }
    else {
        delta = dwPosition - m_dwLastPosition;
    }

    m_dwLastPosition = dwPosition;
    m_llSampleTime += delta;
    LONGLONG llTemp = m_llSampleTime * 10000;
	llTemp /= 48000;
	llTemp *= 1000;
    *pTime = llTemp;
	LeaveCriticalSection( &m_pParent->m_CriticalSection );
    return S_OK;
}

