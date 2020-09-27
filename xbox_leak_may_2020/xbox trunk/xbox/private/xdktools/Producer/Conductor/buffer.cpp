#include "stdafx.h"
#include "dslink.h"
#include "audiosink.h"

#include "stdafx.h"
#include "dslink.h"
#include "audiosink.h"
#include "math.h"
#include "cconduct.h"
#include "effects.h"
#include "dsbufcfg.h"

DWORD CBuffer::m_sdwNextBusID = 1;

CBuffer::CBuffer(CAudioSink *pSink,LPCDSBUFFERDESC pcBufferDesc,LPDWORD pdwFuncID, DWORD dwBusIDCount, REFGUID guidBufferID)
{
    m_cRef = 0;
	AddRef();

	InitializeCriticalSection( &m_CriticalSection );

    m_pParentSink = pSink;
    m_guidBufferID = guidBufferID;
    m_dwBusCount = dwBusIDCount;
    // Make sure we don't go out of bounds.
    if (m_dwBusCount > AUDIOSINK_MAX_CHANNELS)
    {
        m_dwBusCount = AUDIOSINK_MAX_CHANNELS;
    }

    // How many channels in the buffer?
    if (pcBufferDesc && pcBufferDesc->lpwfxFormat)
    {
        m_dwChannels = pcBufferDesc->lpwfxFormat->nChannels;
    }
    else m_dwChannels = 1;

	// Copy the buffer flags
	if( pcBufferDesc )
	{
		m_dwType = pcBufferDesc->dwFlags;
	}
	else
	{
		if( dwBusIDCount )
		{
			m_dwType = DSBCAPS_SINKIN;
		}
		else
		{
			m_dwType = DSBCAPS_MIXIN;
		}
	}

    // Fill in the bus values for the buses that we want to connect to this buffer.
    DWORD dwIndex = 0;
	if( pSink && pdwFuncID )
	{
		for (;dwIndex < m_dwBusCount;dwIndex++)
		{
			m_dwFunctionIDs[dwIndex] = pdwFuncID[dwIndex];
			m_dwBusIDs[dwIndex] = m_sdwNextBusID++;
			m_pBusBuffer[dwIndex] = new short[pSink->m_dwBusBufferSize];
		}
	}

    // Remaining channels on the buffer get null bus and function ids.
    for (;dwIndex < m_dwChannels;dwIndex++)
    {
        m_dwFunctionIDs[dwIndex] = AUDIOSINK_NULL_BUS_ID;
        m_dwBusIDs[dwIndex] = AUDIOSINK_NULL_BUS_ID;
        m_pBusBuffer[dwIndex] = NULL;
    }

    m_lVolume = 0;
    m_dwFrequency = 22050;
    m_dwLeftVolume = 0xFFFF;
    m_dwRightVolume = 0xFFFF;
    m_lPBend = 0;
    m_lPan = 0;
	if( pcBufferDesc && pcBufferDesc->lpwfxFormat )
	{
		m_wfBufferFormat = *pcBufferDesc->lpwfxFormat;
	}
	else
	{
		m_wfBufferFormat.cbSize = 0;
		m_wfBufferFormat.nAvgBytesPerSec = 44100;
		m_wfBufferFormat.nBlockAlign = 2;
		m_wfBufferFormat.nChannels = 1;
		m_wfBufferFormat.nSamplesPerSec = 22050;
		m_wfBufferFormat.wBitsPerSample = 16;
		m_wfBufferFormat.wFormatTag = WAVE_FORMAT_PCM;
	}

	m_fxChain = NULL;

	// Create the mixin buffer, if necessary
	if( pSink
	&&	(m_dwType & DSBCAPS_MIXIN) )
	{
		// Only create it if we have a pointer to the sink, and we're a Mixin buffer
		m_pExtraBuffer = new short[pSink->m_dwBusBufferSize * m_dwChannels];
	}
	else
	{
		// SetFX will create the buffer when the buffer is asked to have an effect chain
		m_pExtraBuffer = NULL;
	}

    if (pSink)
    {
		pSink->AddBuffer( this );
    }
}

CBuffer::CBuffer(CAudioSink *pSink,IUnknown *punkBufferConfig)
{
    m_cRef = 0;
	AddRef();

	InitializeCriticalSection( &m_CriticalSection );

    m_pParentSink = pSink;

	CDirectSoundBufferConfig *pCDirectSoundBufferConfig = NULL;
	punkBufferConfig->QueryInterface( CLSID_PRIVATE_CDirectSoundBufferConfig, (void **)&pCDirectSoundBufferConfig );

    m_guidBufferID = pCDirectSoundBufferConfig->m_DMUSObjectDesc.guidObject;
    m_dwBusCount = pCDirectSoundBufferConfig->m_dwFuncIDsCount;
    // Make sure we don't go out of bounds.
    if (m_dwBusCount > AUDIOSINK_MAX_CHANNELS)
    {
        m_dwBusCount = AUDIOSINK_MAX_CHANNELS;
    }

    // How many channels in the buffer?
    m_dwChannels = pCDirectSoundBufferConfig->m_DSBufferDesc.nChannels;

    // Fill in the bus values for the buses that we want to connect to this buffer.
    DWORD dwIndex;
    for (dwIndex = 0;dwIndex < m_dwBusCount;dwIndex++)
    {
        m_dwFunctionIDs[dwIndex] = pCDirectSoundBufferConfig->m_pdwFuncIDs[dwIndex];
        m_dwBusIDs[dwIndex] = m_sdwNextBusID++;
        m_pBusBuffer[dwIndex] = new short[pSink->m_dwBusBufferSize];
    }

    // Remaining channels on the buffer get null bus and function ids.
    for (;dwIndex < m_dwChannels;dwIndex++)
    {
        m_dwFunctionIDs[dwIndex] = AUDIOSINK_NULL_BUS_ID;
        m_dwBusIDs[dwIndex] = AUDIOSINK_NULL_BUS_ID;
        m_pBusBuffer[dwIndex] = NULL;
    }

    m_lVolume = pCDirectSoundBufferConfig->m_DSBufferDesc.lVolume;
    m_dwFrequency = m_pParentSink->m_wfSinkFormat.nSamplesPerSec;
    m_dwLeftVolume = 0xFFFF;
    m_dwRightVolume = 0xFFFF;
    m_lPBend = 0;
    m_lPan = pCDirectSoundBufferConfig->m_DSBufferDesc.lPan;

	m_wfBufferFormat = m_pParentSink->m_wfSinkFormat;

	if( m_wfBufferFormat.nChannels != m_dwChannels )
	{
		m_wfBufferFormat.nAvgBytesPerSec /= m_wfBufferFormat.nChannels;
		m_wfBufferFormat.nBlockAlign = short(m_wfBufferFormat.nBlockAlign / m_wfBufferFormat.nChannels);
		m_wfBufferFormat.nAvgBytesPerSec *= m_dwChannels;
		m_wfBufferFormat.nBlockAlign = short(m_wfBufferFormat.nBlockAlign * m_dwChannels);
		m_wfBufferFormat.nChannels = short(m_dwChannels);
	}

	ASSERT( m_wfBufferFormat.nChannels == m_dwChannels );

	m_dwType = pCDirectSoundBufferConfig->m_DSBufferDesc.dwFlags;

	if( (m_dwType & DSBCAPS_CTRLFX)
	&&	pCDirectSoundBufferConfig->m_pDXDMOMapList )
	{
		// Only create an effect chain if we're asked to support effects, and we have an effect chain
		m_fxChain = new CEffectChain(this);
	}
	else
	{
		m_fxChain = NULL;
	}

	if( m_fxChain != NULL )
	{
		HRESULT hr = m_fxChain->Clone( pCDirectSoundBufferConfig );
		if (SUCCEEDED(hr))
		{
			//if (!(m_dsbd.dwFlags & DSBCAPS_LOCDEFER))
			{
				hr = m_fxChain->AcquireFxResources();
			}
		}

		if( FAILED(hr) )
		{
			delete m_fxChain;
			m_fxChain = NULL;
		}
	}

	// Create the effect and mixin buffer, if necessary
	if( m_fxChain
	||	(m_dwType & DSBCAPS_MIXIN) )
	{
		// Only crate if this is a mixin buffer or we are asked to support effects
		m_pExtraBuffer = new short[pSink->m_dwBusBufferSize * m_dwChannels];
	}
	else
	{
		m_pExtraBuffer = NULL;
	}

    if (pSink)
    {
		pSink->AddBuffer( this );
    }
}

CBuffer::~CBuffer()
{
	delete m_fxChain;
	m_fxChain = NULL;
    if (m_pParentSink)
    {
        m_pParentSink->RemoveBuffer(this);
    }
    DWORD dwIndex;
    for (dwIndex = 0;dwIndex < m_dwBusCount;dwIndex++)
    {
        delete []m_pBusBuffer[dwIndex];
		m_pBusBuffer[dwIndex] = NULL;
    }

	delete []m_pExtraBuffer;
	m_pExtraBuffer = NULL;

	DeleteCriticalSection( &m_CriticalSection );
}

DWORD CBuffer::DBToMultiplier(long lGain)
{
    if (lGain > 0) lGain = 0;
    double flTemp = lGain;
    flTemp /= 1000.0;
    flTemp = pow(10.0,flTemp);
    flTemp = pow(flTemp,0.5);   // square root.
    flTemp *= 65535.0;          // 2^16th, but avoid overflow...
    return (DWORD) flTemp;
}

void CBuffer::CalcVolume()
{
    if (m_dwChannels == 2)
    {   // Wacky dsound pan algorithm!
        long lLeft;
        long lRight;
        if(m_lPan >= 0)
        {
            lLeft = m_lVolume - m_lPan;
            lRight = m_lVolume;
        }
        else
        {
            lLeft = m_lVolume;
            lRight = m_lVolume + m_lPan;
        }

        m_dwLeftVolume = DBToMultiplier(lLeft);
        m_dwRightVolume = DBToMultiplier(lRight);
    }
    else
    {
        m_dwLeftVolume = m_dwRightVolume = DBToMultiplier(m_lVolume);
    }
}

STDMETHODIMP CBuffer::QueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUnknown || iid == IID_IDirectSoundBuffer8 || iid == IID_IDirectSoundBuffer)
    {
        *ppv = static_cast<IDirectSoundBuffer8*>(this);
    } 
    else if (iid == IID_CBuffer)
    {
        *ppv = this;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    
    return S_OK;
}

STDMETHODIMP_(ULONG) CBuffer::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CBuffer::Release()
{
    if(!InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

STDMETHODIMP CBuffer::GetCaps(LPDSBCAPS pBufferCaps) 
{
	UNREFERENCED_PARAMETER(pBufferCaps);
	ASSERT(FALSE);
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::GetCurrentPosition(LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor) 
{
	UNREFERENCED_PARAMETER(pdwCurrentPlayCursor);
	UNREFERENCED_PARAMETER(pdwCurrentWriteCursor);
	ASSERT(FALSE);
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::GetFormat(LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten)  
{
	if( pwfxFormat == NULL )
	{
		if (pdwSizeWritten)
		{
			*pdwSizeWritten = sizeof(m_wfBufferFormat);
		}
		else
		{
			return DSERR_INVALIDPARAM;
		}
	}
	else
	{
		const DWORD dwToCopy = min( dwSizeAllocated, sizeof(m_wfBufferFormat) );
		memcpy( pwfxFormat, &m_wfBufferFormat, dwToCopy );
		if (pdwSizeWritten)
		{
			*pdwSizeWritten = dwToCopy;
		}
	}
    return DS_OK;
}

STDMETHODIMP CBuffer::GetVolume(LPLONG plVolume)  
{
    *plVolume = m_lVolume;
    return S_OK;
}

STDMETHODIMP CBuffer::GetPan(LPLONG plPan)  
{
    *plPan = m_lPan;
    return S_OK;
}

STDMETHODIMP CBuffer::GetFrequency(LPDWORD pdwFrequency)  
{
    *pdwFrequency = m_dwFrequency;
    return S_OK;
}

STDMETHODIMP CBuffer::GetStatus(LPDWORD pdwStatus)  
{
	UNREFERENCED_PARAMETER(pdwStatus);
	ASSERT(FALSE);
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::Initialize(LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pcBufferDesc)  
{
	UNREFERENCED_PARAMETER(pDirectSound);
	UNREFERENCED_PARAMETER(pcBufferDesc);
	ASSERT(FALSE);
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::Lock(DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
                                           LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags)  
{
	UNREFERENCED_PARAMETER(dwOffset);
	UNREFERENCED_PARAMETER(dwBytes);
	UNREFERENCED_PARAMETER(ppvAudioPtr1);
	UNREFERENCED_PARAMETER(pdwAudioBytes1);
	UNREFERENCED_PARAMETER(ppvAudioPtr2);
	UNREFERENCED_PARAMETER(pdwAudioBytes2);
	UNREFERENCED_PARAMETER(dwFlags);
	ASSERT(FALSE);
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::Play(DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags)  
{
	UNREFERENCED_PARAMETER(dwReserved1);
	UNREFERENCED_PARAMETER(dwPriority);
	UNREFERENCED_PARAMETER(dwFlags);
    return S_OK;
}

STDMETHODIMP CBuffer::SetCurrentPosition(DWORD dwNewPosition)  
{
	UNREFERENCED_PARAMETER(dwNewPosition);
	ASSERT(FALSE);
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetFormat(LPCWAVEFORMATEX pcfxFormat)  
{
	if( pcfxFormat == NULL )
	{
		return DSERR_INVALIDPARAM;
	}

	m_wfBufferFormat = *pcfxFormat;

    return S_OK;
}

STDMETHODIMP CBuffer::SetVolume(LONG lVolume)  
{
    m_lVolume = lVolume;
    CalcVolume();
    return S_OK;
}

STDMETHODIMP CBuffer::SetPan(LONG lPan)  
{
    m_lPan = lPan;
    CalcVolume();
    return S_OK;
}

STDMETHODIMP CBuffer::SetFrequency(DWORD dwFrequency)  
{
    m_dwFrequency = dwFrequency;
    double flTemp;
    if (m_pParentSink)
    {
        flTemp = m_pParentSink->m_wfSinkFormat.nSamplesPerSec;
    }
    else flTemp = 22050;
    flTemp = (double) dwFrequency / flTemp;
    flTemp = log10(flTemp);
    flTemp *= 1200 * 3.3219280948873623478703194294894;    // Convert from Log10 to Log2 and multiply by cents per octave.
    m_lPBend = (long) flTemp;
    if (m_pParentSink)
    {
        m_pParentSink->FillBusArrays();
    }
    return S_OK;
}

STDMETHODIMP CBuffer::Stop()  
{
    return S_OK;
}

STDMETHODIMP CBuffer::Unlock(LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2)  
{
	UNREFERENCED_PARAMETER(pvAudioPtr1);
	UNREFERENCED_PARAMETER(dwAudioBytes1);
	UNREFERENCED_PARAMETER(pvAudioPtr2);
	UNREFERENCED_PARAMETER(dwAudioBytes2);
	ASSERT(FALSE);
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::Restore()  
{
	ASSERT(FALSE);
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetFX(DWORD dwFxCount, LPDSEFFECTDESC pDSFXDesc, LPDWORD pdwResultCodes)  
{
    //DWORD                   dwStatus;
    HRESULT                 hr = DS_OK;
    //DPF_ENTER();

    ASSERT(IS_VALID_READ_PTR(pDSFXDesc, dwFxCount * sizeof *pDSFXDesc));
    ASSERT(!pdwResultCodes || IS_VALID_WRITE_PTR(pdwResultCodes, dwFxCount * sizeof *pdwResultCodes));

	EnterCriticalSection( &m_CriticalSection );

    // Check access rights
    if(!(m_dwType & DSBCAPS_CTRLFX))
    {
        //RPF(DPFLVL_ERROR, "Buffer was not created with DSBCAPS_CTRLFX flag");
        hr = DSERR_CONTROLUNAVAIL;
    }

    // Check the buffer is inactive
	/*
    if(SUCCEEDED(hr))
    {
        hr = GetStatus(&dwStatus);
        if(SUCCEEDED(hr) && (dwStatus & DSBSTATUS_PLAYING))
        {
            //RPF(DPFLVL_ERROR, "Cannot change effects, because buffer is playing");
            hr = DSERR_INVALIDCALL;
        }
    }
	*/

    // Check there are no pending locks on the buffer
	/*
    if(SUCCEEDED(hr) && m_pDeviceBuffer->m_pSysMemBuffer->GetLockCount())
    {
        //RPF(DPFLVL_ERROR, "Cannot change effects, because buffer has pending locks");
        hr = DSERR_INVALIDCALL;
    }
	*/

    if(SUCCEEDED(hr))
    {
        // Release the old FX chain, if necessary
        delete m_fxChain;
		m_fxChain = NULL;

        // If the effects count is 0, we can free up associated resources
        if (dwFxCount == 0)
        {
			if( !(m_dwType & DSBCAPS_MIXIN) )
			{
				delete []m_pExtraBuffer;
				m_pExtraBuffer = NULL;
			}
		}
        else // Allocate the pre-FX buffer and create the FX chain requested
        {
			if( !m_pExtraBuffer )
			{
				m_pExtraBuffer = new short[m_pParentSink->m_dwBusBufferSize * m_dwChannels];

				if( m_pExtraBuffer == NULL )
				{
					hr = E_OUTOFMEMORY;
				}
			}

            if (SUCCEEDED(hr))
            {
                m_fxChain = new CEffectChain(this);
				if( m_fxChain == NULL )
				{
					hr = E_OUTOFMEMORY;
				}
            }
            if (SUCCEEDED(hr))
            {
                hr = m_fxChain->Initialize(dwFxCount, pDSFXDesc, pdwResultCodes);
            }
            if (SUCCEEDED(hr))
            {
                //if (!(m_dsbd.dwFlags & DSBCAPS_LOCDEFER))
                {
                    hr = m_fxChain->AcquireFxResources();
                }

                // We need to preserve the return code from AcquireFxResources, in case it's
                // DS_INCOMPLETE, so we omit "hr=" from GetFxStatus (which always succeeds):
                if (pdwResultCodes)
                {
                    m_fxChain->GetFxStatus(pdwResultCodes);
                }
            }
            if (FAILED(hr))
            {
                RELEASE(m_fxChain);

				if( !(m_dwType & DSBCAPS_MIXIN) )
				{
					delete []m_pExtraBuffer;
					m_pExtraBuffer = NULL;
				}
           }
        }
    }

	LeaveCriticalSection( &m_CriticalSection );

    //DPF_LEAVE_HRESULT(hr);
    return hr;
}

STDMETHODIMP CBuffer::AcquireResources(DWORD dwFlags, DWORD dwEffectsCount, LPDWORD pdwResultCodes)  
{
	UNREFERENCED_PARAMETER(dwFlags);
	UNREFERENCED_PARAMETER(dwEffectsCount);
	UNREFERENCED_PARAMETER(pdwResultCodes);
	ASSERT(FALSE);
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::GetObjectInPath(REFGUID rguidObject, DWORD dwIndex, REFGUID rguidInterface, LPVOID *ppObject)  
{
	if( !m_fxChain )
	{
		return DMUS_E_NOT_FOUND;
	}

	return m_fxChain->GetEffectInterface( rguidObject, dwIndex, rguidInterface, ppObject );
}

WAVEFORMATEX* CBuffer::Format( void )
{
	return &m_wfBufferFormat;
}

// Do Pre-Render initialization
void CBuffer::PreRender( DWORD dwLength )
{
	EnterCriticalSection( &m_CriticalSection );

	// If this is a mixin buffer, clear the mixin buffer
	if( m_pExtraBuffer
	&&	(m_dwType & DSBCAPS_MIXIN) )
	{
		ZeroMemory( m_pExtraBuffer, dwLength * sizeof(short) * m_dwChannels );
	}

	LeaveCriticalSection( &m_CriticalSection );
}

// Render the buffer into a temporary 32-bit buffer
void CBuffer::Render( long *plBuffer, DWORD dwLength, REFERENCE_TIME rtTime )
{
	EnterCriticalSection( &m_CriticalSection );

	if( m_fxChain )
	{
		RenderFX( plBuffer, dwLength, rtTime );
	}
	else
	{
		RenderNoFX( plBuffer, dwLength );
	}

	LeaveCriticalSection( &m_CriticalSection );
}

void CBuffer::RenderFX( long *plBuffer, DWORD dwLength, REFERENCE_TIME rtTime )
{
	// If not a Mixin buffer, copy the bus buffers into m_pExtraBuffer
	if( !(m_dwType & DSBCAPS_MIXIN) )
	{
		if( m_dwBusCount == 1 )
		{
			// Do left bus
			short *pnSource = m_pBusBuffer[0];

			// If only one channel
			if( m_dwChannels == 1 )
			{
				// Just copy the data
				memcpy( m_pExtraBuffer, m_pBusBuffer[0], dwLength * sizeof(short) );
			}
			else
			{
				ASSERT( m_dwChannels == 2 );
				// Otherwise need to duplicate it
				short *pnDest = m_pExtraBuffer;
				for (DWORD dwIndex = dwLength; dwIndex > 0; dwIndex--)
				{
					// Copy the value twice, once for left and once for right
					*pnDest = *pnSource;
					pnDest++;
					*pnDest = *pnSource;
					pnDest++;

					// Go to the next source sample
					pnSource++;
				}
			}
		}
		else
		{
			ASSERT( m_dwBusCount == 2 && m_dwChannels == 2 );

			// Do left and right bus
			short *pnSource1 = m_pBusBuffer[0];
			short *pnSource2 = m_pBusBuffer[1]; // 1 = right bus
			short *pnDest = m_pExtraBuffer;
			DWORD dwIndex;

			for (dwIndex = dwLength; dwIndex > 0; dwIndex--)
			{
				// Left
				*pnDest = *pnSource1;
				pnDest++;

				// Right
				*pnDest = *pnSource2;
				pnDest++;

				pnSource1++;
				pnSource2++;
			}
		}
	}

	// Now, apply the effect chain
	m_fxChain->Render( m_pExtraBuffer, dwLength * 2 * m_dwChannels, rtTime );

	RenderFXFinalMix( plBuffer, dwLength );
}

void CBuffer::RenderFXFinalMix( long *plBuffer, DWORD dwLength )
{
	// Now, mix the effect buffer into the main accumulation buffer
	if( m_dwChannels == 1 )
	{
		short *pnSource = m_pExtraBuffer;
		for (DWORD dwIndex = dwLength; dwIndex > 0; dwIndex--)
		{
			long lAccum;

			// Left
			lAccum = *pnSource * m_dwLeftVolume;
			lAccum = lAccum >> 12;

			// Copy the value
			*plBuffer += lAccum;
			plBuffer++;

			// Right
			lAccum = *pnSource * m_dwRightVolume;
			lAccum = lAccum >> 12;

			// Copy the value
			*plBuffer += lAccum;
			plBuffer++;

			// Go to the next source sample
			pnSource++;
		}
	}
	else
	{
		ASSERT( m_dwChannels == 2 );

		short *pnSource = m_pExtraBuffer;
		for (DWORD dwIndex = dwLength; dwIndex > 0; dwIndex--)
		{
			long lAccum;

			// Left
			lAccum = *pnSource * m_dwLeftVolume;
			lAccum = lAccum >> 12;

			// Copy the value
			*plBuffer += lAccum;
			plBuffer++;

			// Go to the next source sample
			pnSource++;

			// Right
			lAccum = *pnSource * m_dwRightVolume;
			lAccum = lAccum >> 12;

			// Copy the value
			*plBuffer += lAccum;
			plBuffer++;

			// Go to the next source sample
			pnSource++;
		}
	}
}

void CBuffer::RenderNoFX( long *plBuffer, DWORD dwLength )
{
	long *plOrigBuffer = plBuffer;

	if( (m_dwBusCount == 1)
	||	((m_dwType & DSBCAPS_MIXIN) && (m_dwChannels == 1)) )
	{
		short *pnSource;
		if( m_dwBusCount == 1 )
		{
			// Do left bus
			pnSource = m_pBusBuffer[0];
		}
		else
		{
			// Use Mixin buffer
			pnSource = m_pExtraBuffer;
		}
		long *plMixBuffer = plOrigBuffer;

		for (DWORD dwIndex = dwLength; dwIndex > 0; dwIndex--)
		{
			long lAccum = *pnSource * m_dwLeftVolume;
			lAccum = lAccum >> 12;

			// Copy the value twice, once to the left channel and once to the right channel
			*plMixBuffer += lAccum;
			*plMixBuffer ++;
			*plMixBuffer += lAccum;
			*plMixBuffer ++;

			pnSource++;
		}
	}
	else if( !(m_dwType & DSBCAPS_MIXIN) )
	{
		ASSERT( m_dwBusCount == 2 );
		// Do left and right bus
		short *pnSource1 = m_pBusBuffer[0];
		short *pnSource2 = m_pBusBuffer[1]; // 1 = right bus
		long *plMixBuffer = plOrigBuffer;
		DWORD dwIndex;

		for (dwIndex = dwLength; dwIndex > 0; dwIndex--)
		{
			long lAccum;

			// Left
			lAccum = *pnSource1 * m_dwLeftVolume;
			lAccum = lAccum >> 12;
			*plMixBuffer += lAccum;
			plMixBuffer++;

			// Right
			lAccum = *pnSource2 * m_dwRightVolume;
			lAccum = lAccum >> 12;
			*plMixBuffer += lAccum;
			plMixBuffer++;

			pnSource1++;
			pnSource2++;
		}
	}
	else
	{
		ASSERT( (m_dwBusCount == 0) && (m_dwChannels == 2) );

		short *pnSource = m_pExtraBuffer;
		long *plMixBuffer = plOrigBuffer;
		DWORD dwIndex;

		for (dwIndex = dwLength; dwIndex > 0; dwIndex--)
		{
			long lAccum;

			// Left
			lAccum = *pnSource * m_dwLeftVolume;
			lAccum = lAccum >> 12;
			*plMixBuffer += lAccum;
			plMixBuffer++;
			pnSource++;

			// Right
			lAccum = *pnSource * m_dwRightVolume;
			lAccum = lAccum >> 12;
			*plMixBuffer += lAccum;
			plMixBuffer++;
			pnSource++;
		}
	}
}


/***************************************************************************
 *
 *  FindSendLoop
 *
 *  Description:
 *      Auxiliary function used in effects.cpp to detect send loops.
 *      Returns DSERR_SENDLOOP if a send effect pointing to this buffer
 *      is detected anywhere in the send graph rooted at pCurBuffer.
 *
 *  Arguments:
 *      CDirectSoundSecondaryBuffer* [in]: Current buffer in graph traversal.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code; DSERR_SENDLOOP if a send loop
 *               is found, otherwise DS_OK.
 *
 ***************************************************************************/

//#undef DPF_FNAME
//#define DPF_FNAME "CDirectSoundSecondaryBuffer::FindSendLoop"

HRESULT CBuffer::FindSendLoop(CBuffer* pCurBuffer)
{
    HRESULT hr = DS_OK;

    //DPF_ENTER();
    //CHECK_WRITE_PTR(pCurBuffer);
	if( !IS_VALID_WRITE_PTR(pCurBuffer, sizeof( CBuffer ) ) )
	{
		return E_INVALIDARG;
	}

    if (pCurBuffer == this)
    {
        //RPF(DPFLVL_ERROR, "Send loop detected from buffer at 0x%p to itself", this);
        hr = DSERR_SENDLOOP;
    }
    else if (pCurBuffer->m_fxChain)
    {
        // Buffer has effects - look for send effects and call ourself recursively.
		POSITION pos = pCurBuffer->m_fxChain->m_fxList.GetHeadPosition();
		while( pos && SUCCEEDED(hr) )
		{
			CEffect* pFxNode = pCurBuffer->m_fxChain->m_fxList.GetNext( pos );

            CBuffer* pDstBuffer = pFxNode->GetDestBuffer();
            if (pDstBuffer)
                hr = FindSendLoop(pDstBuffer); 
        }
    }

    //DPF_LEAVE_HRESULT(hr);
    return hr;
}
