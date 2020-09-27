// Copyright (c) 1998-1999 Microsoft Corporation
// audiosink.cpp : Implementation of CAudioSink

#include "pchime.h"

CAudioSink::CAudioSink(WAVEFORMATEX *pwfxFormat)
{
    InitializeCriticalSection( &m_CriticalSection );
    m_MasterClock.m_pParent = this;
//    m_DSLink.SetWaveFormat(pwfxFormat);
    m_cRef = 0;
//    m_pMasterClock = NULL;
//    m_pMasterClockSync = NULL;
    m_fActive = FALSE;
    m_pSynthSource = NULL;
    m_wfSinkFormat.cbSize = 0;
    m_wfSinkFormat.nAvgBytesPerSec = SAMPLERATE * 2;
    m_wfSinkFormat.nBlockAlign = 2;
    m_wfSinkFormat.nChannels = 1;
    m_wfSinkFormat.nSamplesPerSec = SAMPLERATE;
    m_wfSinkFormat.wBitsPerSample = 16;
    m_wfSinkFormat.wFormatTag = WAVE_FORMAT_PCM;
    m_llAbsPlay = 0;
    m_llAbsWrite = 0;
    m_pBusIDs = NULL;
    m_pFunctionIDs = NULL;
    m_ppBusBuffers = NULL;
    m_pPBends = NULL;
    m_dwBusCount = 0;
    m_dwBusBufferSize = (m_wfSinkFormat.nSamplesPerSec * AUDIOSINK_BUS_SIZE) / 1000;
#ifdef XMIX
/*    DWORD dwFunctions[1] = { 0 };
    m_pDummyBuffer = new CBuffer(this,NULL,dwFunctions,1,GUID_NULL);
    if (m_pDummyBuffer)
    {
        m_pDummyBuffer->AddRef();
        ResizeBusArrays();
    }*/
//    DbgPrint("XMIX defined. Using microsequencer hardware mixing.\n");
#else
    DbgPrint("XMIX undefined. Using software mixing.\n");
#endif

}

CAudioSink::~CAudioSink()
{
    Activate(false);
    if (m_pSynthSource)
    {
        m_pSynthSource->Release();
    }
    if (m_pBusIDs) delete [] m_pBusIDs;
    if (m_pFunctionIDs) delete [] m_pFunctionIDs;
    if (m_ppBusBuffers) delete [] m_ppBusBuffers;
    if (m_pPBends) delete [] m_pPBends;
	DeleteCriticalSection( &m_CriticalSection );

#ifndef XMIX
    for(int i = 0; i < PACKETCOUNT; i++){
#ifdef SILVER
        delete [] m_pnBuffers[i];
#else
        XPhysicalFree(m_pnBuffers[i]);
#endif
    }
#else
/*    if (m_pDummyBuffer)
    {
        m_pDummyBuffer->Release();
    }*/
#endif // ndef XMIX
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
/*	EnterCriticalSection( &m_CriticalSection );
    if (m_pSynthSource)
    {
        m_pSynthSource->Release();
    }
    m_pSynthSource = pDSSource;
    if (m_pSynthSource)
    {
        m_pSynthSource->AddRef();
        DWORD dwSizeWritten;
        m_pSynthSource->GetFormat(&m_wfSinkFormat,sizeof(m_wfSinkFormat),&dwSizeWritten);
        m_dwBusBufferSize = (m_wfSinkFormat.nSamplesPerSec * AUDIOSINK_BUS_SIZE) / 1000;
    }
	LeaveCriticalSection( &m_CriticalSection );*/
    return S_OK;
}

STDMETHODIMP CAudioSink::RemoveSource(LPDIRECTSOUNDSOURCE pDSSource) 
{
/*	EnterCriticalSection( &m_CriticalSection );
    if (m_pSynthSource && (m_pSynthSource == pDSSource))
    {
        m_pSynthSource->Release();
        m_pSynthSource = NULL;
    }
	LeaveCriticalSection( &m_CriticalSection );*/
    return S_OK;
}

STDMETHODIMP CAudioSink::SetMasterClock(IReferenceClock *pClock)
{
    return S_OK;
}

STDMETHODIMP CAudioSink::CreateSoundBuffer(LPCDSBUFFERDESC pcBufferDesc, LPDWORD pdwFuncID, DWORD dwBusIDCount,
                                           REFGUID guidBufferID, LPDIRECTSOUNDBUFFER *ppBuffer) 
{
/*    CBuffer *pBuffer = new CBuffer(this,pcBufferDesc,pdwFuncID,dwBusIDCount,guidBufferID);
    if (pBuffer)
    {
        pBuffer->QueryInterface(IID_IDirectSoundBuffer,(void **)ppBuffer);
        ResizeBusArrays();
        return S_OK;
    }*/
    return E_OUTOFMEMORY;
}

STDMETHODIMP CAudioSink::CreateSoundBufferFromConfig(LPUNKNOWN pConfig, LPDIRECTSOUNDBUFFER *ppBuffer)
{
    // Create a null, bogus buffer for now, since we don't have the file io code in place yet.
/*    CBuffer *pBuffer = new CBuffer(this,NULL,NULL,0,GUID_NULL);
    if (pBuffer)
    {
        pBuffer->QueryInterface(IID_IDirectSoundBuffer,(void **)ppBuffer);
        ResizeBusArrays();
        return S_OK;
    }*/
    return E_OUTOFMEMORY;
}

STDMETHODIMP CAudioSink::GetSoundBuffer(DWORD dwBusID, LPDIRECTSOUNDBUFFER *ppBuffer)
{
/*	EnterCriticalSection( &m_CriticalSection );
    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
        DWORD dwBus = 0; 
        for (;dwBus < pBuffer->m_dwBusCount; dwBus++)
        {
            if (pBuffer->m_dwBusIDs[dwBus] == dwBusID)
            {
              	LeaveCriticalSection( &m_CriticalSection );
                return pBuffer->QueryInterface(IID_IDirectSoundBuffer, (void **) ppBuffer);
            }
        }
    }
	LeaveCriticalSection( &m_CriticalSection );*/
    return E_FAIL; // DSERR_INVALIDPARAM;
}

STDMETHODIMP CAudioSink::GetBusCount(LPDWORD pdwCount)
{
/*	EnterCriticalSection( &m_CriticalSection );
    DWORD dwCount = 0;
    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
        dwCount += pBuffer->m_dwBusCount;
    }
    *pdwCount = dwCount;
	LeaveCriticalSection( &m_CriticalSection );*/
    return S_OK;
}

STDMETHODIMP CAudioSink::GetBusIDs(LPDWORD pdwBusIDs, LPDWORD pdwFuncIDs, DWORD dwBusCount)
{
/*	EnterCriticalSection( &m_CriticalSection );
    if (dwBusCount > m_dwBusCount) dwBusCount = m_dwBusCount;
    if (pdwBusIDs)
    {
        memcpy(pdwBusIDs,m_pBusIDs,dwBusCount*sizeof(DWORD));
    }
    if (pdwFuncIDs)
    {
        memcpy(pdwFuncIDs,m_pFunctionIDs,dwBusCount*sizeof(DWORD));
    }
	LeaveCriticalSection( &m_CriticalSection );*/
    return S_OK;
}

STDMETHODIMP CAudioSink::GetFunctionalID(DWORD dwBusID, LPDWORD pdwFuncID) 
{
/*	EnterCriticalSection( &m_CriticalSection );
    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
        DWORD dwBus = 0; 
        for (;dwBus < pBuffer->m_dwBusCount; dwBus++)
        {
            if (pBuffer->m_dwBusIDs[dwBus] == dwBusID)
            {
                *pdwFuncID = pBuffer->m_dwFunctionIDs[dwBus];
            	LeaveCriticalSection( &m_CriticalSection );
                return S_OK;
            }
        }
    }
	LeaveCriticalSection( &m_CriticalSection );*/
    return E_FAIL; // DSERR_INVALIDPARAM;
}

STDMETHODIMP CAudioSink::GetSoundBufferBusIDs(LPDIRECTSOUNDBUFFER pDSBuffer, 
                                              LPDWORD pdwBusIDs, 
                                              LPDWORD pdwFuncIDs, 
                                              LPDWORD pdwBusCount)
{
/*	EnterCriticalSection( &m_CriticalSection );
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
            	LeaveCriticalSection( &m_CriticalSection );
                return DS_OK;
            }
        }
    }
   	LeaveCriticalSection( &m_CriticalSection );*/
    return E_FAIL; // DSERR_INVALIDPARAM;
}

STDMETHODIMP CAudioSink::GetLatencyClock(IReferenceClock **ppClock)
{
    return QueryInterface(IID_IReferenceClock,(void **)ppClock);
}

void TestRender(short *pnBuffer,DWORD dwLength,DWORD dwPosition)
{
    DWORD dwIndex;
/*    TRACE("Mixing %ld samples from %ld to %ld on buffer %lx\n",
        dwLength,dwPosition,dwLength+dwPosition,pnBuffer);*/
    for (dwIndex = 0;dwIndex < dwLength;dwIndex += 2)
    {
        pnBuffer[dwIndex] = (short) ((dwPosition + dwIndex) * 1000);
        pnBuffer[dwIndex+1] = (short) ((dwPosition + dwIndex) * 1000);
    }
}

HRESULT CAudioSink::InitStream()

{
#ifndef XMIX
    DWORD dwX;
    for (dwX = 0; dwX < PACKETCOUNT; dwX++)
    {
#ifdef SILVER
        m_pnBuffers[dwX] = new short[PACKETSIZE*2]; // (short *) VirtualAlloc(NULL, PACKETSIZE*4, MEM_COMMIT, PAGE_READWRITE); 
#else
        m_pnBuffers[dwX] = (short *) XPhysicalAlloc(PACKETSIZE*4, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE);
        __asm wbinvd;
#endif
        m_dwStatus[dwX] = S_OK;
        memset(m_pnBuffers[dwX],0,PACKETSIZE*4);
    }
#endif // dnef XMIX

    WAVEFORMATEX waveFormat;
    waveFormat.nSamplesPerSec = SAMPLERATE;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * 2 * 2;
    waveFormat.cbSize = 0;
    waveFormat.nBlockAlign = 4;
    waveFormat.nChannels = 2;
    waveFormat.wBitsPerSample = 16;
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    DSSTREAMDESC            dssd;
    memset(&dssd, 0, sizeof(dssd));
#ifdef SILVER
    dssd.dwSize = sizeof(dssd);
#endif // SILVER
    dssd.dwFlags = 0;
    dssd.dwMaxAttachedPackets = PACKETCOUNT;
    dssd.lpwfxFormat = &waveFormat;
    m_llSampleTime = 0;
#ifndef XMIX
    m_dwCurrentBuffer = 0;
#endif

#ifdef SILVER

    HRESULT hr = DirectSoundCreateStream(NULL, &dssd, &m_pStream, NULL);

#else // SILVER

#ifdef XMIX
    HRESULT hr = S_OK;
    m_bStartTimeInitialized = false;
    m_rtStartTime = 0;
#else
    HRESULT hr = DirectSoundCreateStream(&dssd, &m_pStream);
#endif

#endif // SILVER
#ifdef SILVER
    REFERENCE_TIME rtWakeUp;
    DoWork(&rtWakeUp);
#endif
    return hr;
}

void CAudioSink::KillStream()
{
#ifndef XMIX
    if (m_pStream)
    {
        m_pStream->Release();
    }
#endif
}

void CAudioSink::DoWork(REFERENCE_TIME *prtWakeUp)

{
#ifdef XMIX

    DirectSoundDoWork();

    REFERENCE_TIME now;
    m_MasterClock.GetTime(&now);

    LONGLONG stNow;
    RefToSampleTime(now, &stNow);

    static const LONGLONG kMinMixSize = PACKETSIZE;
    static const LONGLONG kTargetMixAhead = 3 * PACKETSIZE;

    LONGLONG stTargetMixTime = stNow + kTargetMixAhead;
    
    while(m_llSampleTime < stTargetMixTime){
        Render(NULL,kMinMixSize,m_llSampleTime,m_llSampleTime - kMinMixSize);
        m_llSampleTime += kMinMixSize;
        DirectSoundDoWork();
    }
    m_MasterClock.GetTime(prtWakeUp);
    *prtWakeUp += PACKETDUR * 10000;

#else // ndef XMIX

#ifndef SILVER
    DirectSoundDoWork(); // Needed to make sure m_dwStatus is up-to-date if the stream has sarved.
#endif
    while (m_dwStatus[m_dwCurrentBuffer] != XMEDIAPACKET_STATUS_PENDING)
    {
        XMEDIAPACKET xmb;
        xmb.pvBuffer = m_pnBuffers[m_dwCurrentBuffer];
        xmb.dwMaxSize = PACKETSIZE*4;
        xmb.pdwCompletedSize = NULL;
        m_dwStatus[m_dwCurrentBuffer] = 0;
        xmb.pdwStatus = &m_dwStatus[m_dwCurrentBuffer];
        xmb.hCompletionEvent = NULL;
        xmb.prtTimestamp = NULL;
        Render(m_pnBuffers[m_dwCurrentBuffer],PACKETSIZE,m_llSampleTime,m_llSampleTime - PACKETSIZE);
        m_pStream->Process(&xmb, NULL);
        m_llSampleTime += PACKETSIZE;
        m_dwCurrentBuffer++;
        if (m_dwCurrentBuffer >= PACKETCOUNT)
        {
            m_dwCurrentBuffer = 0;
        }
#ifndef SILVER
        DirectSoundDoWork(); // Needed to make sure m_dwStatus is up-to-date if the stream has sarved.
#endif
    }
    m_MasterClock.GetTime(prtWakeUp);
    *prtWakeUp += PACKETDUR * 10000;

#endif // ! XMIX
}

static void WINAPI _AudioSink(LPVOID lpParam,REFERENCE_TIME *prtWakeUp)
{
    ((CAudioSink *)lpParam)->DoWork(prtWakeUp);
}

HRESULT CAudioSink::StreamThread()
{
#ifdef XMIX

    Trace(-1,"CAudioSink::StreamThread - not implemented for XMIX.\n");
    return E_FAIL; // Not implemented for XMIX

#else // ndef XMIX

    DWORD dwX;
    for (dwX = 0; dwX < PACKETCOUNT; dwX++)
    {
#ifdef SILVER
        m_pnBuffers[dwX] = new short[PACKETSIZE*2]; // (short *) VirtualAlloc(NULL, PACKETSIZE*4, MEM_COMMIT, PAGE_READWRITE); 
#else
        m_pnBuffers[dwX] = (short *) XPhysicalAlloc(PACKETSIZE*4, MAXULONG_PTR, 64, PAGE_READWRITE | PAGE_NOCACHE);
#endif
        m_dwStatus[dwX] = 0;
        memset(m_pnBuffers[dwX],0,PACKETSIZE*4);
    }
    WAVEFORMATEX waveFormat;
    waveFormat.nSamplesPerSec = SAMPLERATE;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * 2 * 2;
    waveFormat.cbSize = 0;
    waveFormat.nBlockAlign = 4;
    waveFormat.nChannels = 2;
    waveFormat.wBitsPerSample = 16;
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    DbgPrint("Creating stream\n");

    DSSTREAMDESC            dssd;

    memset(&dssd, 0, sizeof(dssd));
#ifdef SILVER
    dssd.dwSize = sizeof(dssd);
#endif // SILVER
    dssd.dwFlags = 0;
    dssd.dwMaxAttachedPackets = PACKETCOUNT;
    dssd.lpwfxFormat = &waveFormat;
    m_llSampleTime = 0;
    m_dwCurrentBuffer = 0;
#ifdef SILVER

    HRESULT hr = DirectSoundCreateStream(NULL, &dssd, &m_pStream, NULL);

#else // SILVER

    HRESULT hr = DirectSoundCreateStream(&dssd, &m_pStream);

#endif // SILVER

    HANDLE hCompletionEvent = NULL;
    if(SUCCEEDED(hr))
    {
        LONGLONG llSampleTime  = 0;
        DWORD dwPacket = 0;
        hCompletionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        for (;;dwPacket++)
        {
            XMEDIAPACKET xmb;
            xmb.pvBuffer = m_pnBuffers[m_dwCurrentBuffer];
            xmb.dwMaxSize = PACKETSIZE*4;
            xmb.pdwCompletedSize = NULL;
            m_dwStatus[m_dwCurrentBuffer] = 0;
            xmb.pdwStatus = &m_dwStatus[m_dwCurrentBuffer];
            xmb.hCompletionEvent = hCompletionEvent;
            xmb.prtTimestamp = NULL;
            Render(m_pnBuffers[m_dwCurrentBuffer],PACKETSIZE,m_llSampleTime,m_llSampleTime - PACKETSIZE);
            m_pStream->Process(&xmb, NULL);
            m_llSampleTime += PACKETSIZE;
            m_dwCurrentBuffer++;
            if (m_dwCurrentBuffer >= PACKETCOUNT)
            {
                m_dwCurrentBuffer = 0;
            }
            if (dwPacket > 0)
            {
//              DbgPrint("Waiting for end of packet\n");
                WaitForSingleObject(hCompletionEvent, INFINITE);
            }
        }
    }
    if(hCompletionEvent)
    {
        CloseHandle(hCompletionEvent);
    }

    return hr;
#endif // !defined XMIX
}

static DWORD WINAPI _SinkThread(LPVOID lpParam)
{
    ((CAudioSink *)lpParam)->StreamThread();
    return 0;
}


STDMETHODIMP CAudioSink::Activate(BOOL fEnable)
{
	EnterCriticalSection( &m_CriticalSection );
    if (m_fActive != fEnable)
    {
        if (fEnable)
        {
            if (SUCCEEDED(InitStream()))
            {
//                m_pSinkWorker = g_BossMan.InstallWorker(_AudioSink,this,30);
                m_fActive = TRUE;
            	LeaveCriticalSection( &m_CriticalSection );
                return S_OK;
            }

/*            m_hSinkThread = CreateThread(NULL, 1024, _SinkThread, this, 0, &m_dwSinkThreadID);
            if( m_hSinkThread )
            {
//                m_hRealtime = CreateEvent(NULL,FALSE,FALSE,NULL);
                SetThreadPriority( m_hSinkThread, THREAD_PRIORITY_TIME_CRITICAL );
                m_fActive = TRUE;
                return S_OK;
            }*/
        }
        else
        {
            if (m_pSinkWorker)
            {
//                m_pSinkWorker->PinkSlip();
                m_pSinkWorker = NULL;
            }
            KillStream();
//            m_DSLink.CloseDown();
        }
        m_fActive = FALSE;
    }
	LeaveCriticalSection( &m_CriticalSection );
    return S_OK;
}

#define MILS_TO_REF	10000

STDMETHODIMP CAudioSink::SampleToRefTime(LONGLONG llSampleTime, REFERENCE_TIME *prtTime)
{
	llSampleTime *= MILS_TO_REF;
	llSampleTime /= SAMPLERATE;
	llSampleTime *= 1000;
    *prtTime = llSampleTime;
    return S_OK;
}

STDMETHODIMP CAudioSink::RefToSampleTime(REFERENCE_TIME rtTime, LONGLONG *pllSampleTime) 
{
	rtTime /= 1000;
	rtTime *= SAMPLERATE;
	rtTime /= MILS_TO_REF;
    *pllSampleTime = rtTime;
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

	EnterCriticalSection( &m_CriticalSection );
#ifdef XMIX
    LONGLONG playTime;
    hr = m_MasterClock.GetTime(&playTime);
    if(SUCCEEDED(hr))
    {
        LONGLONG writeTime = playTime + 1 * 10000; // Write time is 1 ms ahead of play time.
        *pTime = writeTime;
    }
#else
    hr = SampleToRefTime(m_llAbsWrite, pTime);
#endif
	LeaveCriticalSection( &m_CriticalSection );
    return hr;
}

STDMETHODIMP CAudioSink::KsProperty(PKSPROPERTY Property,ULONG PropertyLength,
        LPVOID PropertyData,ULONG DataLength,ULONG* BytesReturned)
{
    return S_OK;
}

STDMETHODIMP CAudioSink::KsMethod(PKSMETHOD Method,ULONG MethodLength,
                                  LPVOID MethodData,ULONG DataLength,ULONG* BytesReturned) 
{
    return S_OK;
}

STDMETHODIMP CAudioSink::KsEvent(PKSEVENT Event OPTIONAL,ULONG EventLength,
                                 LPVOID EventData,ULONG DataLength,ULONG* BytesReturned)
{
    return S_OK;
}


/*
void CAudioSink::ResizeBusArrays()

{
	EnterCriticalSection( &m_CriticalSection );
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
	LeaveCriticalSection( &m_CriticalSection );
}

void CAudioSink::FillBusArrays()
{
	EnterCriticalSection( &m_CriticalSection );
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
	LeaveCriticalSection( &m_CriticalSection );
}

void CAudioSink::RemoveBuffer(CBuffer *pBuffer)

{
	EnterCriticalSection( &m_CriticalSection );
    m_BufferList.Remove(pBuffer);
    ResizeBusArrays();
	LeaveCriticalSection( &m_CriticalSection );
}
*/
#ifndef XMIX

void CAudioSink::Mix(short *pnBuffer,DWORD dwLength)

{
	EnterCriticalSection( &m_CriticalSection );
    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
        if (pBuffer->m_fActive && pBuffer->m_dwBusCount) // Only mix activated buffers.
        {
            DWORD dwChannel = 0;
            DWORD dwBus = 0; 
            for (;dwChannel < 2; dwChannel++)
            {
                if (dwBus >= pBuffer->m_dwBusCount)
                {
                    dwBus = pBuffer->m_dwBusCount - 1;
                }
                short *pnSource = pBuffer->m_pBusBuffer[dwBus];
                DWORD dwIndex;
                DWORD dwWrite = dwChannel;
                for (dwIndex = 0; dwIndex < dwLength; dwIndex++)
                {
                    long lAccum;
                    if (dwChannel) lAccum = pnSource[dwIndex] * pBuffer->m_dwRightVolume;
                    else lAccum = pnSource[dwIndex] * pBuffer->m_dwLeftVolume;
                    lAccum = lAccum >> 16;
                    lAccum += pnBuffer[dwWrite];
                    if (lAccum > 32767) lAccum = 32767;
                    else if (lAccum < -32767) lAccum = -32767;
                    pnBuffer[dwWrite] = (short) lAccum;
                    dwWrite += 2;
                }
                dwBus++;
            }
        }
    }
	LeaveCriticalSection( &m_CriticalSection );
}

/*void CAudioSink::Mix(short *pnBuffer,DWORD dwLength)

{
	short *pnOrigBuffer = pnBuffer;

    CBuffer *pBuffer = m_BufferList.GetHead();
    for (;pBuffer;pBuffer = pBuffer->GetNext())
    {
        if (pBuffer->m_fActive) // Only mix activated buffers.
        {
		    if( pBuffer->m_dwBusCount > 0 )
		    {
			    // Do left bus
                short *pnSource = pBuffer->m_pBusBuffer[0];
                DWORD dwIndex;

			    pnBuffer = pnOrigBuffer;

                for (dwIndex = dwLength; dwIndex > 0; dwIndex--)
                {
                    long lAccum;
				    lAccum = *pnSource * pBuffer->m_dwLeftVolume;

                    lAccum = lAccum >> 16;
                    lAccum += *pnBuffer;

                    if (lAccum > 32767) lAccum = 32767;
                    else if (lAccum < -32767) lAccum = -32767;

                    *pnBuffer = (short) lAccum;
				    pnBuffer += 2;// Assume stereo
				    pnSource++;
                }
		    }

		    if( pBuffer->m_dwBusCount > 1 )
		    {
			    // Do right bus
                short *pnSource = pBuffer->m_pBusBuffer[1]; // 1 = right bus
                DWORD dwIndex;

 			    pnBuffer = pnOrigBuffer + 1;// start with the second sample

                for (dwIndex = dwLength; dwIndex > 0; dwIndex--)
                {
                    long lAccum;
				    lAccum = *pnSource * pBuffer->m_dwRightVolume;

                    lAccum = lAccum >> 16;
                    lAccum += *pnBuffer;

                    if (lAccum > 32767) lAccum = 32767;
                    else if (lAccum < -32767) lAccum = -32767;

                    *pnBuffer = (short) lAccum;
				    pnBuffer += 2;// Assume stereo
				    pnSource++;
                }
            }
        }
    }
}*/

#endif // !XMIX

static int nBeep = 0;

/*void WINAPI DoBeep()

{
    nBeep = 2;
}*/

void CAudioSink::Render(short *pnBuffer,
                        DWORD dwLength,
                        LONGLONG llWritePosition, 
                        LONGLONG llPlayPosition)
{
	EnterCriticalSection( &m_CriticalSection );
#ifndef XMIX
    memset(pnBuffer,0,dwLength*2*2);
#endif
DWORD dwTemp = dwLength*2;
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
            ULONGLONG llBytesToRead = dwSliceSize * 2;
#ifndef XMIX
            DWORD dwIndex = 0;
            for (;dwIndex < m_dwBusCount;dwIndex++)
            {
                memset(m_ppBusBuffers[dwIndex],0,dwSliceSize * 2);
            }
#endif // !XMIX
            m_pSynthSource->Read((void**)m_ppBusBuffers, m_pBusIDs, m_pFunctionIDs, 
                m_pPBends,m_dwBusCount, &llBytesToRead);
#ifndef XMIX
            Mix(&pnBuffer[dwStartPoint*2],dwSliceSize);
#endif // !XMIX
            dwStartPoint += dwSliceSize;
        }
/*
        if (nBeep)
        {
            REFERENCE_TIME rtMaster;
            REFERENCE_TIME rtLatency;
            GetTime(&rtLatency);
            m_MasterClock.GetTime(&rtMaster);
            DbgPrint("Beep: Master time: %ld, Latency time: %ld, Size: %ld\n",
                (long)(rtMaster/10000),(long)(rtLatency / 10000),dwTemp);
            DWORD dwScan = 0;
            for (;dwScan < dwTemp;dwScan++)
            {
                pnBuffer[dwScan] = (short) dwScan * 4000;
            }
            nBeep--;
        }*/
    }
    m_llAbsPlay = llPlayPosition;
    m_llAbsWrite = llWritePosition;
    LeaveCriticalSection( &m_CriticalSection );
}

IReferenceClock *CAudioSink::GetMasterClock()
{
    return (IReferenceClock *) &m_MasterClock;
}

CClock::CClock(){
#ifdef XMIX
    m_dwLastPosition = 0;
    m_llSampleTime = 0;
#endif // XMIX
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

#ifdef XMIX
// Defined in dmusicp.h, but that's not convenient to include here.

EXTERN_C DWORD WINAPI DirectSoundGetSampleTime(void);

#endif

STDMETHODIMP CClock::GetTime(LPREFERENCE_TIME pTime)
{
    HRESULT hr = E_FAIL;

    if( pTime == NULL )
    {
        return E_INVALIDARG;
    }

	EnterCriticalSection( &m_pParent->m_CriticalSection );

#ifdef XMIX

    DWORD dwPosition = DirectSoundGetSampleTime();
    hr = S_OK;

    // Check to see if we looped around, which happens every 24 hours

    if(SUCCEEDED(hr)){
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
    }
    hr = m_pParent->SampleToRefTime(m_llSampleTime, pTime);
            
#else

    hr = m_pParent->SampleToRefTime(m_pParent->m_llAbsPlay, pTime);

#endif

	LeaveCriticalSection( &m_pParent->m_CriticalSection );
    return hr;
}

