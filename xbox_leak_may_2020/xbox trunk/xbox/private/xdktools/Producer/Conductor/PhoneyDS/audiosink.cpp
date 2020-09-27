#include "stdafx.h"
#include "dslink.h"
#include <objbase.h>
#include "audiosink.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define _FILE_DEBUG 1
#ifdef _FILE_DEBUG
void FTRACE( LPSTR pszFormat, ... );
#endif

CAudioSink::CAudioSink(WAVEFORMATEX *pwfxFormat)
{
	::InitializeCriticalSection( &m_CriticalSection );
	::InitializeCriticalSection( &m_csClock );
    m_DSLink.SetWaveFormat(pwfxFormat);
    m_cRef = 0;
    m_pMasterClock = NULL;
    m_pMasterClockSync = NULL;
    m_fActive = FALSE;
    m_pSynthSource = NULL;
	m_wfSinkFormat = *pwfxFormat;
	m_fStartSampleClock = true;
    m_pBusIDs = NULL;
    m_pFunctionIDs = NULL;
    m_ppBusBuffers = NULL;
    m_pPBends = NULL;
    m_dwBusCount = 0;
    m_dwBusBufferSize = (m_wfSinkFormat.nSamplesPerSec * AUDIOSINK_BUS_SIZE) / 1000;
	m_plTmpMixBuffer = new long[m_dwBusBufferSize * 2]; // Stereo
}

CAudioSink::~CAudioSink()
{
    if (m_pMasterClock)
    {
        m_pMasterClock->Release();
    }
    if (m_pSynthSource)
    {
        m_pSynthSource->Release();
    }
    if (m_pMasterClockSync)
    {
        m_pMasterClockSync->Release();
    }
    m_DSLink.CloseDown();
    if (m_pBusIDs) delete [] m_pBusIDs;
    if (m_pFunctionIDs) delete [] m_pFunctionIDs;
    if (m_ppBusBuffers) delete [] m_ppBusBuffers;
    if (m_pPBends) delete [] m_pPBends;
	if (m_plTmpMixBuffer) delete [] m_plTmpMixBuffer;
	::DeleteCriticalSection( &m_csClock );
	::DeleteCriticalSection( &m_CriticalSection );
}

STDMETHODIMP CAudioSink::QueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUnknown || iid == IID_IDirectSoundSynthSink)
    {
        *ppv = static_cast<IDirectSoundSynthSink*>(this);
    } 
    else if(iid == IID_IDirectSoundConnect) 
    {
        *ppv = static_cast<IDirectSoundConnect*>(this);
    } 
    else if(iid == IID_IReferenceClock) 
    {
        *ppv = static_cast<IReferenceClock*>(this);
    }
    else if(iid == IID_IKsControl) 
    {
        *ppv = static_cast<IKsControl*>(this);
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    
    return S_OK;
}

STDMETHODIMP_(ULONG) CAudioSink::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CAudioSink::Release()
{
    if(!InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return m_cRef;
}


STDMETHODIMP CAudioSink::AddSource(LPDIRECTSOUNDSOURCE pDSSource) 
{
	::EnterCriticalSection( &m_CriticalSection );
    if (m_pSynthSource)
    {
        m_pSynthSource->Release();
    }
    m_pSynthSource = pDSSource;
    if (m_pSynthSource)
    {
        m_pSynthSource->AddRef();
        DWORD dwSizeWritten;

		// NT bug 465060 causes the synth to return the wrong sample rate, so we ignore
		// the sample rate returned by GetFormat.
		WAVEFORMATEX wfTemp;
        m_pSynthSource->GetFormat(&wfTemp,sizeof(m_wfSinkFormat),&dwSizeWritten);
		m_wfSinkFormat.nBlockAlign = wfTemp.nBlockAlign;
		m_wfSinkFormat.nChannels = wfTemp.nChannels;
		m_wfSinkFormat.wBitsPerSample = wfTemp.wBitsPerSample;
        m_wfSinkFormat.nAvgBytesPerSec = m_wfSinkFormat.nSamplesPerSec * m_wfSinkFormat.nBlockAlign;
		// BUGBUG: Can't do this, as the buffers we've already created are using the old
		// m_dwBusBufferSize value.  Look in CBuffer::CBuffer() and CAudioSink::Render().
        //m_dwBusBufferSize = (m_wfSinkFormat.nSamplesPerSec * AUDIOSINK_BUS_SIZE) / 1000;

		// Flag to reset the SampleClock
		m_fStartSampleClock = true;
    }
	::LeaveCriticalSection( &m_CriticalSection );
    return S_OK;
}

STDMETHODIMP CAudioSink::RemoveSource(LPDIRECTSOUNDSOURCE pDSSource) 
{
	::EnterCriticalSection( &m_CriticalSection );
    if (m_pSynthSource && (m_pSynthSource == pDSSource))
    {
        m_pSynthSource->Release();
        m_pSynthSource = NULL;
    }
	::LeaveCriticalSection( &m_CriticalSection );
    return S_OK;
}

STDMETHODIMP CAudioSink::SetMasterClock(IReferenceClock *pClock)
{
	::EnterCriticalSection( &m_csClock );
    if (m_pMasterClock)
    {
        m_pMasterClock->Release();
    }
    if (m_pMasterClockSync)
    {
        m_pMasterClockSync->Release();
        m_pMasterClockSync = NULL;
    }
    m_pMasterClock = pClock;
    if (m_pMasterClock)
    {
        m_pMasterClock->AddRef();
        m_pMasterClock->QueryInterface(IID_IDirectSoundSinkSync, (void**)&m_pMasterClockSync);
    }
	::LeaveCriticalSection( &m_csClock );
    return S_OK;
}

STDMETHODIMP CAudioSink::CreateSoundBuffer(LPCDSBUFFERDESC pcBufferDesc, LPDWORD pdwFuncID, DWORD dwBusIDCount,
                                           REFGUID guidBufferID, LPDIRECTSOUNDBUFFER *ppBuffer) 
{
    CBuffer *pBuffer = new CBuffer(this,pcBufferDesc,pdwFuncID,dwBusIDCount,guidBufferID);
    if (pBuffer)
    {
        pBuffer->QueryInterface(IID_IDirectSoundBuffer,(void **)ppBuffer);
		pBuffer->Release();
        ResizeBusArrays();
        return S_OK;
    }
    return E_OUTOFMEMORY;
}

STDMETHODIMP CAudioSink::CreateSoundBufferFromConfig(LPUNKNOWN pConfig, LPDIRECTSOUNDBUFFER *ppBuffer)
{
    CBuffer *pBuffer = new CBuffer(this,pConfig);
    if (pBuffer)
    {
        pBuffer->QueryInterface(IID_IDirectSoundBuffer,(void **)ppBuffer);
		pBuffer->Release();
        ResizeBusArrays();
        return S_OK;
    }
    return E_OUTOFMEMORY;
}

STDMETHODIMP CAudioSink::GetSoundBuffer(DWORD dwBusID, LPDIRECTSOUNDBUFFER *ppBuffer)
{
	::EnterCriticalSection( &m_CriticalSection );
    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
        DWORD dwBus = 0; 
        for (;dwBus < pBuffer->m_dwBusCount; dwBus++)
        {
            if (pBuffer->m_dwBusIDs[dwBus] == dwBusID)
            {
				::LeaveCriticalSection( &m_CriticalSection );
                return pBuffer->QueryInterface(IID_IDirectSoundBuffer, (void **) ppBuffer);
            }
        }
    }
	::LeaveCriticalSection( &m_CriticalSection );
    return DSERR_INVALIDPARAM;
}

STDMETHODIMP CAudioSink::GetBusCount(LPDWORD pdwCount)
{
	::EnterCriticalSection( &m_CriticalSection );
    DWORD dwCount = 0;
    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
        dwCount += pBuffer->m_dwBusCount;
    }
    *pdwCount = dwCount;
	::LeaveCriticalSection( &m_CriticalSection );
    return S_OK;
}

STDMETHODIMP CAudioSink::GetBusIDs(LPDWORD pdwBusIDs, LPDWORD pdwFuncIDs, DWORD dwBusCount)
{
	::EnterCriticalSection( &m_CriticalSection );
    if (dwBusCount > m_dwBusCount) dwBusCount = m_dwBusCount;
    if (pdwBusIDs)
    {
        memcpy(pdwBusIDs,m_pBusIDs,dwBusCount*sizeof(DWORD));
    }
    if (pdwFuncIDs)
    {
        memcpy(pdwFuncIDs,m_pFunctionIDs,dwBusCount*sizeof(DWORD));
    }
	::LeaveCriticalSection( &m_CriticalSection );
    return S_OK;
}

STDMETHODIMP CAudioSink::GetFunctionalID(DWORD dwBusID, LPDWORD pdwFuncID) 
{
	::EnterCriticalSection( &m_CriticalSection );
    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
        DWORD dwBus = 0; 
        for (;dwBus < pBuffer->m_dwBusCount; dwBus++)
        {
            if (pBuffer->m_dwBusIDs[dwBus] == dwBusID)
            {
                *pdwFuncID = pBuffer->m_dwFunctionIDs[dwBus];
				::LeaveCriticalSection( &m_CriticalSection );
                return S_OK;
            }
        }
    }
	::LeaveCriticalSection( &m_CriticalSection );
    return DSERR_INVALIDPARAM;
}

STDMETHODIMP CAudioSink::GetSoundBufferBusIDs(LPDIRECTSOUNDBUFFER pDSBuffer, 
                                              LPDWORD pdwBusIDs, 
                                              LPDWORD pdwFuncIDs, 
                                              LPDWORD pdwBusCount)
{
	::EnterCriticalSection( &m_CriticalSection );
    CBuffer *pCBuffer;
    if (SUCCEEDED(pDSBuffer->QueryInterface(IID_CBuffer,(void **)&pCBuffer)))
    {
        pCBuffer->Release();
        CBuffer *pBuffer = m_BufferList.GetHead();
        for (;pBuffer;pBuffer = pBuffer->GetNext())
        {
            if (pBuffer == pCBuffer)
            {
                DWORD dwMaxBusCount = *pdwBusCount;
                if (dwMaxBusCount > pBuffer->m_dwBusCount)
                {
                    dwMaxBusCount = pBuffer->m_dwBusCount;
                }
                *pdwBusCount = 0;

                for (DWORD dwBus = 0; dwBus < dwMaxBusCount; dwBus++)
                {
                    if (pBuffer->m_dwBusIDs[dwBus] == AUDIOSINK_NULL_BUS_ID)
                        break;

                    pdwBusIDs[dwBus] = pBuffer->m_dwBusIDs[dwBus];
                    if (pdwFuncIDs)
                    {
                        pdwFuncIDs[dwBus] = pBuffer->m_dwFunctionIDs[dwBus];
                    }
                    (*pdwBusCount)++;
                }
				::LeaveCriticalSection( &m_CriticalSection );
                return DS_OK;
            }
        }
    }
	::LeaveCriticalSection( &m_CriticalSection );
    return DSERR_INVALIDPARAM;
}

STDMETHODIMP CAudioSink::GetLatencyClock(IReferenceClock **ppClock)
{
    return QueryInterface(IID_IReferenceClock,(void **)ppClock);
}

STDMETHODIMP CAudioSink::Activate(BOOL fEnable)
{
	// Critical section not necessary - m_fActive is not used anywhere else
    if (m_fActive != fEnable)
    {
        if (fEnable)
        {
            if (m_DSLink.OpenUp(this))
            {
                m_fActive = TRUE;
                return S_OK;
            }
            return E_FAIL;
        }
        else
        {
            m_DSLink.CloseDown();
        }
        m_fActive = FALSE;
    }
    return S_OK;
}

STDMETHODIMP CAudioSink::SampleToRefTime(LONGLONG llSampleTime, REFERENCE_TIME *prtTime)
{
    m_SampleClock.SampleToRefTime(llSampleTime, prtTime);
    return S_OK;
}

STDMETHODIMP CAudioSink::RefToSampleTime(REFERENCE_TIME rtTime, LONGLONG *pllSampleTime) 
{
    *pllSampleTime = m_SampleClock.RefToSampleTime(rtTime);
    return S_OK;
}

STDMETHODIMP CAudioSink::GetFormat(LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten) 
{
    if (dwSizeAllocated > sizeof(m_wfSinkFormat))
    {
        dwSizeAllocated = sizeof(m_wfSinkFormat);
    }
    memcpy(pwfxFormat,&m_wfSinkFormat,dwSizeAllocated);
    if (pdwSizeWritten) *pdwSizeWritten = dwSizeAllocated;
    return S_OK;
}

STDMETHODIMP CAudioSink::AdviseTime(REFERENCE_TIME ,REFERENCE_TIME,HANDLE,DWORD *)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAudioSink::AdvisePeriodic(REFERENCE_TIME,REFERENCE_TIME,HANDLE,DWORD * )
{
    return E_NOTIMPL;
}

STDMETHODIMP CAudioSink::Unadvise( DWORD )
{
    return E_NOTIMPL;
}

STDMETHODIMP CAudioSink::GetTime(LPREFERENCE_TIME pTime)
{
    HRESULT hr = E_FAIL;

    if( pTime == NULL )
    {
        return E_INVALIDARG;
    }

	::EnterCriticalSection( &m_csClock );
    REFERENCE_TIME rtCompare;
    if (m_pMasterClock)
    {
        m_pMasterClock->GetTime(&rtCompare);

        hr = SampleToRefTime(m_DSLink.GetAbsWriteSample(), pTime);

        if (FAILED(hr))
        {
            TRACE(/*DPFLVL_WARNING, */"Sink Latency Clock: SampleToRefTime failed");

			::LeaveCriticalSection( &m_csClock );
            return hr;
        }

        if (*pTime < rtCompare)
        {
//            DPF(DPFLVL_INFO, "Sink Latency Clock off. Latency time is %ldms, Master time is %ldms",
//                (long) (*pTime / 10000), (long) (rtCompare / 10000));
			TRACE("Sink Latency Clock off (early). Latency time is %ldms, Master time is %ldms\n",
				(long) (*pTime / 10000), (long) (rtCompare / 10000));
            *pTime = rtCompare;
        }
        else if (*pTime > (rtCompare + (10000 * 1000)))
        {
//                DPF(DPFLVL_INFO, "Sink Latency Clock off. Latency time is %ldms, Master time is %ldms",
//                    (long) (*pTime / 10000), (long) (rtCompare / 10000));
            TRACE("Sink Latency Clock off (late). Latency time is %ldms, Master time is %ldms\n",
			    (long) (*pTime / 10000), (long) (rtCompare / 10000));
            *pTime = rtCompare + (10000 * 1000);
        }
        hr = S_OK;
    }
    else
    {
        TRACE(/*DPFLVL_WARNING, */"Sink Latency Clock - GetTime called with no master clock");
    }
	::LeaveCriticalSection( &m_csClock );
    return hr;
}

STDMETHODIMP CAudioSink::KsProperty(PKSPROPERTY Property,ULONG PropertyLength,
        LPVOID PropertyData,ULONG DataLength,ULONG* BytesReturned)
{
	UNREFERENCED_PARAMETER(Property);
	UNREFERENCED_PARAMETER(PropertyLength);
	UNREFERENCED_PARAMETER(PropertyData);
	UNREFERENCED_PARAMETER(DataLength);
	UNREFERENCED_PARAMETER(BytesReturned);
    return E_NOTIMPL;
}

STDMETHODIMP CAudioSink::KsMethod(PKSMETHOD Method,ULONG MethodLength,
                                  LPVOID MethodData,ULONG DataLength,ULONG* BytesReturned) 
{
	UNREFERENCED_PARAMETER(Method);
	UNREFERENCED_PARAMETER(MethodLength);
	UNREFERENCED_PARAMETER(MethodData);
	UNREFERENCED_PARAMETER(DataLength);
	UNREFERENCED_PARAMETER(BytesReturned);
    return E_NOTIMPL;
}

STDMETHODIMP CAudioSink::KsEvent(PKSEVENT Event OPTIONAL,ULONG EventLength,
                                 LPVOID EventData,ULONG DataLength,ULONG* BytesReturned)
{
	UNREFERENCED_PARAMETER(Event);
	UNREFERENCED_PARAMETER(EventLength);
	UNREFERENCED_PARAMETER(EventData);
	UNREFERENCED_PARAMETER(DataLength);
	UNREFERENCED_PARAMETER(BytesReturned);
    return E_NOTIMPL;
}


void CAudioSink::ResizeBusArrays()
{
	::EnterCriticalSection( &m_CriticalSection );
    GetBusCount(&m_dwBusCount);
    if (m_pBusIDs) delete [] m_pBusIDs;
    if (m_pFunctionIDs) delete [] m_pFunctionIDs;
    if (m_ppBusBuffers) delete [] m_ppBusBuffers;
    if (m_pPBends) delete [] m_pPBends;
    if (m_dwBusCount)
    {
        m_pBusIDs = new DWORD[m_dwBusCount];
        m_pFunctionIDs = new DWORD[m_dwBusCount];
        m_ppBusBuffers = new short *[m_dwBusCount];
        m_pPBends = new long[m_dwBusCount];
        FillBusArrays();
    }
    else
    {
        m_pBusIDs = NULL;
        m_pFunctionIDs = NULL;
        m_ppBusBuffers = NULL;
        m_pPBends = NULL;
    }
	::LeaveCriticalSection( &m_CriticalSection );
}

void CAudioSink::FillBusArrays()
{
	::EnterCriticalSection( &m_CriticalSection );
    DWORD dwIndex = 0;
    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
        DWORD dwBus = 0; 
        for (;dwBus < pBuffer->m_dwBusCount; dwBus++)
        {
            // Make sure we don't go beyond the array bounds.
            if (dwIndex < m_dwBusCount)
            {
                m_pBusIDs[dwIndex] = pBuffer->m_dwBusIDs[dwBus];
                m_pFunctionIDs[dwIndex] = pBuffer->m_dwFunctionIDs[dwBus];
                m_ppBusBuffers[dwIndex] = pBuffer->m_pBusBuffer[dwBus];
                m_pPBends[dwIndex++] = pBuffer->m_lPBend;
            }
        }
    }
	::LeaveCriticalSection( &m_CriticalSection );
}

void CAudioSink::RemoveBuffer(CBuffer *pBuffer)

{
	::EnterCriticalSection( &m_CriticalSection );
    m_BufferList.Remove(pBuffer);
    ResizeBusArrays();
	::LeaveCriticalSection( &m_CriticalSection );
}

void CAudioSink::AddBuffer(CBuffer *pBuffer)

{
	::EnterCriticalSection( &m_CriticalSection );

	// Must add buffers to the head of the list, so that all send source buffers
	// are processed before send destination buffers
    m_BufferList.AddHead(pBuffer);
    ResizeBusArrays();
	::LeaveCriticalSection( &m_CriticalSection );
}

void CAudioSink::Mix(short *pnBuffer,DWORD dwLength, REFERENCE_TIME rtTime)

{
	ZeroMemory( m_plTmpMixBuffer, sizeof(long) * m_dwBusBufferSize * 2 );

    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
		pBuffer->PreRender( dwLength );
    }

    pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
		pBuffer->Render( m_plTmpMixBuffer, dwLength, rtTime );
    }

	// Now, clip temp mix buffer to destination buffer
	long *plMixBuffer = m_plTmpMixBuffer;
	for( DWORD dwIndex = dwLength * 2; dwIndex > 0; dwIndex-- )
	{
		long lAccum = *plMixBuffer;

		// Do the final shift
		lAccum = lAccum >>4;

		// Clip
		if (lAccum > 32767) lAccum = 32767;
		else if (lAccum < -32767) lAccum = -32767;

		*pnBuffer = (short) lAccum;
		pnBuffer++;
		plMixBuffer++;
	}
}

void CAudioSink::Render(short *pnBuffer,
                        DWORD dwLength,
                        LONGLONG /*llWritePosition*/,
                        LONGLONG llPlayPosition)
{
	::EnterCriticalSection( &m_csClock );
    if (m_fStartSampleClock)
    {
		ASSERT( llPlayPosition < ULONG_MAX );
		TRACE("Starting clock at %I64d\n",llPlayPosition);
        m_SampleClock.Start(m_pMasterClock, m_wfSinkFormat.nSamplesPerSec, DWORD(llPlayPosition));
		m_fStartSampleClock = false;
    }
    // Now, sync the audio to the master clock.
    // If we are in the first two seconds, just let the sample clock sync to the master clock.
    // This allows it to overcome jitter and get a tight starting position.
    // Then, after that first two seconds, switch to letting the sample
    // clock drive the master clock.
    // Also, if there is no way of adjusting the master clock (no m_pMasterClockSync),
    // then always adjust the sample clock instead.
    BOOL fLockToMaster = (!m_pMasterClockSync) || (m_DSLink.GetAbsPlaySample() < m_wfSinkFormat.nSamplesPerSec * 2);
    m_SampleClock.SyncToMaster(m_DSLink.GetAbsPlaySample(),m_pMasterClock,fLockToMaster);
    // Then, take the same offset that was generated by the sync code
    // and use it to adjust the timing of the master clock.
    if (!fLockToMaster)
    {
        // First, get the new offset that was generated by SyncToMaster.
        REFERENCE_TIME rtOffset;
        m_SampleClock.GetClockOffset(&rtOffset);
//        TRACE("Offset is now %ld.%ld\n",(long) (rtOffset / 10000),(long)rtOffset);
        m_pMasterClockSync->SetClockOffset(-rtOffset);
    }
	::LeaveCriticalSection( &m_csClock );

	::EnterCriticalSection( &m_CriticalSection );
    if (m_pSynthSource && m_dwBusCount)
    {
        DWORD dwStartPoint = 0;
        while (dwLength > 0)
        {
            DWORD dwSliceSize = dwLength;
            if (dwLength > m_dwBusBufferSize)
            {
                dwLength -= m_dwBusBufferSize;
                dwSliceSize = m_dwBusBufferSize;
            }
            else
            {
                dwLength = 0;
            }
            ULONGLONG llBytesToRead = dwSliceSize * 2; // 16-bit samples, so two bytes per sample
            DWORD dwIndex = 0;
            for (;dwIndex < m_dwBusCount;dwIndex++)
            {
				ZeroMemory( m_ppBusBuffers[dwIndex], dwSliceSize * 2 );
            }
#ifdef _FILE_DEBUG
			FTRACE("Render: %d %I64d\n", timeGetTime(), m_DSLink.GetAbsWriteSample() * 2 + dwStartPoint * 2 );
#endif
			m_pSynthSource->Seek( m_DSLink.GetAbsWriteSample() * 2 + dwStartPoint * 2 );
            m_pSynthSource->Read((void**)m_ppBusBuffers, m_pBusIDs, m_pFunctionIDs, 
                m_pPBends,m_dwBusCount, &llBytesToRead);


			REFERENCE_TIME rtStart = 0;
			SampleToRefTime( m_DSLink.GetAbsWriteSample() + dwStartPoint, &rtStart );

#ifdef _FILE_DEBUG
			FTRACE("Mix: %d %I64d\n", timeGetTime(), rtStart );
#endif
            Mix(&pnBuffer[dwStartPoint*2],dwSliceSize, rtStart);
            dwStartPoint += dwSliceSize;
        }
    }
	::LeaveCriticalSection( &m_CriticalSection );
}

CBuffer *CAudioSink::FindBufferFromGUID( REFGUID rguidBuffer )
{
	::EnterCriticalSection( &m_CriticalSection );
    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
		if( rguidBuffer == pBuffer->m_guidBufferID )
		{
			::LeaveCriticalSection( &m_CriticalSection );
			return pBuffer;
		}
    }

	::LeaveCriticalSection( &m_CriticalSection );
	return NULL;
}
