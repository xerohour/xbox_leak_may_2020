// Copyright (c) 1998-1999 Microsoft Corporation
// buffer.cpp : Implementation of CBuffer

#include "pchime.h"

DWORD CBuffer::m_sdwNextBusID = 1;

CBuffer::CBuffer(CAudioSink *pSink,LPCDSBUFFERDESC pcBufferDesc,LPDWORD pdwFuncID, DWORD dwBusIDCount, REFGUID guidBufferID) : m_MemTrack(DMTRACK_BUFFER)
{
    m_cRef = 0;
    m_pParentSink = pSink;
    m_fActive = false;
    if (pSink)
    {
        pSink->m_BufferList.AddTail(this);
    }
    m_guidBufferID = guidBufferID;
    m_dwBusCount = dwBusIDCount;
    // Make sure we don't go out of bounds.
    if (dwBusIDCount > AUDIOSINK_MAX_CHANNELS)
    {
        dwBusIDCount = AUDIOSINK_MAX_CHANNELS;
    }
    // How many channels in the buffer?
    if (pcBufferDesc && pcBufferDesc->lpwfxFormat)
    {
        m_dwChannels = pcBufferDesc->lpwfxFormat->nChannels;
    }
    else m_dwChannels = 1;
    // Fill in the bus values for the buses that we want to connect to this buffer.
    DWORD dwIndex;
    for (dwIndex = 0;dwIndex < dwBusIDCount;dwIndex++)
    {
        m_dwFunctionIDs[dwIndex] = pdwFuncID[dwIndex];
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
    m_lVolume = 0;
    m_dwFrequency = SAMPLERATE;
    m_dwLeftVolume = 0xFFFF;
    m_dwRightVolume = 0xFFFF;
    m_lPBend = 0;
    m_lPan = 0;
    CalcVolume();
}

CBuffer::~CBuffer()
{
    if (m_pParentSink)
    {
        m_pParentSink->RemoveBuffer(this);
    }
    DWORD dwIndex;
    for (dwIndex = 0;dwIndex < m_dwBusCount;dwIndex++)
    {
        delete m_pBusBuffer[dwIndex];
    }
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
    if (m_dwChannels == 1)
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
#ifdef SILVER
    if(iid == IID_IUnknown || iid == IID_IDirectSoundBuffer)
    {
        *ppv = static_cast<IDirectSoundBuffer*>(this);
    } 
    else 
#endif
        if (iid == IID_CBuffer)
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

STDMETHODIMP CBuffer::Play(DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags)  
{
    m_fActive = true;
    return S_OK;
}

STDMETHODIMP CBuffer::PlayEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags)
{
    m_fActive = true;
	return S_OK;
}

STDMETHODIMP CBuffer::Stop()  
{
    m_fActive = false;
    return S_OK;
}

STDMETHODIMP CBuffer::StopEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags) 
{
    m_fActive = false;
    return S_OK;
}

STDMETHODIMP CBuffer::SetPlayRegion(DWORD dwPlayStart, DWORD dwPlayLength) 
{
    Trace(0,"Error: Can not set play region on a DMusic AudioPath buffer.\n");
	return E_FAIL;
}

STDMETHODIMP CBuffer::SetLoopRegion(DWORD dwLoopStart, DWORD dwLoopLength) 
{
    Trace(0,"Error: Can not set loop region on a DMusic AudioPath buffer.\n");
	return E_FAIL;
}

STDMETHODIMP CBuffer::GetStatus(LPDWORD pdwStatus)  
{
    Trace(0,"Error: GetStatus not yet implemented on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::GetCurrentPosition(LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor) 
{
    Trace(0,"Error: GetCurrentPosition not valid on a DMusic AudioPath buffer.\n");
    return E_FAIL;
}

STDMETHODIMP CBuffer::SetCurrentPosition(DWORD dwNewPosition)  
{
    Trace(0,"Error: SetCurrentPosition not valid on a DMusic AudioPath buffer.\n");
    return E_FAIL;
}

STDMETHODIMP CBuffer::SetBufferData(LPVOID pvBufferData, DWORD dwBufferBytes) 

{
    Trace(0,"Error: SetBufferData not valid on a DMusic AudioPath buffer.\n");
	return E_FAIL;
}

STDMETHODIMP CBuffer::Lock(DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
                                           LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags)  
{
    Trace(0,"Error: Lock not valid on a DMusic AudioPath buffer.\n");
    return E_FAIL;
}

STDMETHODIMP CBuffer::SetFrequency(DWORD dwFrequency)  
{
    m_dwFrequency = dwFrequency;
    double flTemp;
    if (m_pParentSink)
    {
        flTemp = m_pParentSink->m_wfSinkFormat.nSamplesPerSec;
    }
    else flTemp = SAMPLERATE;
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

STDMETHODIMP CBuffer::SetVolume(LONG lVolume)  
{
    m_lVolume = lVolume;
    CalcVolume();
    return S_OK;
}


#ifdef SILVER

STDMETHODIMP CBuffer::SetChannelVolume(LPCDSCHANNELVOLUME pVolume)
{
    Trace(1,"Warning: SetChannelVolume not yet implemented on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}

#else // SILVER

STDMETHODIMP CBuffer::SetPitch(LONG lPitch)
{
    Trace(0,"Error: SetPitch not legal on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetLFO(LPCDSLFODESC pLFODesc)
{
    Trace(0,"Error: SetLFO not legal on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetEG(LPCDSENVELOPEDESC pEnvelopeDesc)
{
    Trace(0,"Error: SetEG not legal on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetFilter(LPCDSFILTERDESC pFilterDesc)
{
    Trace(1,"Warning: SetFilter not yet implemented on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetOutputBuffer(LPDIRECTSOUNDBUFFER pOutputBuffer)
{
    Trace(1,"Warning: SetOutputBuffer not yet implemented on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetMixBins(LPCDSMIXBINS pMixBins)
{
    Trace(1,"Warning: SetMixBins not yet implemented on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetMixBinVolumes(LPCDSMIXBINS pMixBins)
{
    Trace(1,"Warning: SetMixBinVolumes not yet implemented on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetHeadroom(DWORD dwHeadroom)
{
    Trace(1,"Warning: SetHeadroom not yet implemented on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}

#endif // SILVER

    // IDirectSound3DBuffer methods
STDMETHODIMP CBuffer::SetAllParameters(LPCDS3DBUFFER pds3db, DWORD dwApply)

{
    Trace(1,"Warning: SetAllParameters not yet implemented on a DMusic AudioPath buffer.\n");
	return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply) 

{
    Trace(1,"Warning: SetConeAngles not yet implemented on a DMusic AudioPath buffer.\n");
	return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetConeOrientation(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) 

{
    Trace(1,"Warning: SetConeOrientation not yet implemented on a DMusic AudioPath buffer.\n");
	return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetConeOutsideVolume(LONG lConeOutsideVolume, DWORD dwApply) 

{
    Trace(1,"Warning: SetConeOutsideVolume not yet implemented on a DMusic AudioPath buffer.\n");
	return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetMaxDistance(FLOAT flMaxDistance, DWORD dwApply) 

{
    Trace(1,"Warning: SetMaxDistance not yet implemented on a DMusic AudioPath buffer.\n");
	return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetMinDistance(FLOAT flMinDistance, DWORD dwApply) 

{
    Trace(1,"Warning: SetMinDistance not yet implemented on a DMusic AudioPath buffer.\n");
	return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetMode(DWORD dwMode, DWORD dwApply) 

{
    Trace(1,"Warning: SetMode not yet implemented on a DMusic AudioPath buffer.\n");
	return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) 

{
    Trace(1,"Warning: SetPosition not yet implemented on a DMusic AudioPath buffer.\n");
	return E_NOTIMPL;
}

STDMETHODIMP CBuffer::SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) 

{
    Trace(1,"Warning: SetVelocity not yet implemented on a DMusic AudioPath buffer.\n");
	return E_NOTIMPL;
}

    // IDirectSoundNotify methods
STDMETHODIMP CBuffer::SetNotificationPositions(DWORD dwNotifyCount, LPCDSBPOSITIONNOTIFY paNotifies) 

{
    Trace(1,"Warning: SetNotificationPositions not implemented on a DMusic AudioPath buffer.\n");
	return E_NOTIMPL;
}



/*
STDMETHODIMP CBuffer::GetFormat(LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten)  
{
    pwfxFormat->cbSize = 0;
    pwfxFormat->nAvgBytesPerSec = SAMPLERATE * 2;
    pwfxFormat->nBlockAlign = 2;
    pwfxFormat->nChannels = 1;
    pwfxFormat->nSamplesPerSec = SAMPLERATE;
    pwfxFormat->wBitsPerSample = 16;
    pwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
    if (pdwSizeWritten) *pdwSizeWritten = sizeof(WAVEFORMATEX);
    return S_OK;
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



STDMETHODIMP CBuffer::Initialize(LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pcBufferDesc)  
{
    return S_OK;
}
*/



/*STDMETHODIMP CBuffer::SetPan(LONG lPan)  
{
    m_lPan = lPan;
    CalcVolume();
    return S_OK;
}
*/

STDMETHODIMP CBuffer::GetObjectInPath(REFGUID rguidObject, DWORD dwIndex, REFGUID rguidInterface, LPVOID *ppObject)  
{
    Trace(1,"Warning: GetObjectInPath not implemented on a DMusic AudioPath buffer.\n");
    return E_NOTIMPL;
}





