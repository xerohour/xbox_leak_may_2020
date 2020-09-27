// Copyright (c) 1998-1999 Microsoft Corporation
// audpath.cpp : Implementation of CAudioPath

#include "pchime.h"


CBufferNode::CBufferNode()

{
    m_lActivateCount = 0;
    m_cRef = 1;
    m_pBuffer = NULL;
    m_pManager = NULL;
    m_dwMixBin = 0;
#ifdef XBOX
    m_MixBinsHeader.bControllers[0] = 0;
#endif
}

CBufferNode::~CBufferNode()

{
    FinalDeactivate();
    if (m_pManager)
    {
        m_pManager->Remove(this);
    }
}

ULONG CBufferNode::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

ULONG CBufferNode::Release()
{
    if (!InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT CBufferNode::Activate(BOOL fActivate)

{
    HRESULT hr = S_OK;
    if (m_pBuffer)
    {
        if (fActivate)
        {
            if (!m_lActivateCount)
            {
#ifndef XBOX
                // Don't bother starting if the primary buffer.
                if (SUCCEEDED(hr) && !(m_BufferHeader.dwFlags & DMUS_BUFFERF_PRIMARY))
                {
                    TraceI(2,"Play buffer %lx\n",m_pBuffer);
                    hr = m_pBuffer->Play(0,0,DSBPLAY_LOOPING);
#ifdef DBG
                    if (FAILED(hr))
                    {
                        Trace(1,"Error: Activate on audiopath failed because buffer failed Play command.\n");
                    }
#endif
                }
#endif
            }
            if (hr == S_OK)
            {
                m_lActivateCount++;
            }
            TraceI(3,"Incrementing %lx to %ld\n",m_pBuffer,m_lActivateCount);
        }
        else
        {
            if (m_lActivateCount > 0)
            {
                m_lActivateCount--;
                if (!m_lActivateCount)
                {
#ifndef XBOX
                    // Don't bother stopping the primary buffer.
                    if (!(m_BufferHeader.dwFlags & DMUS_BUFFERF_PRIMARY))
                    {
                        TraceI(2,"Stop buffer %lx\n",m_pBuffer);
                        hr = m_pBuffer->Stop();
                    }
#endif
                }
                TraceI(3,"Decrementing %lx to %ld\n",m_pBuffer,m_lActivateCount);
                if (hr != S_OK)
                {
                    m_lActivateCount++;
                }
            }
        }
    }
    return hr;
}


void CBufferNode::FinalDeactivate()

{
    if (m_lActivateCount)
    {
        Activate(FALSE);
    }
    if (m_pBuffer)
    {
        m_pBuffer->Release();
        m_pBuffer = NULL;
    }
}

CBufferManager::CBufferManager() 
{ 
#ifdef DXAPI
    m_pFirstBuffer = NULL;
    m_pSinkConnect = NULL;
    m_pSynthSink = NULL;
#endif
    m_pPerf = NULL; 
} 

CBufferManager::~CBufferManager() 
{     
    Clear();
#ifdef DXAPI
    if (m_pSinkConnect)
    {
        m_pSinkConnect->Release();
    }
    if (m_pSynthSink)
    {
        m_pSynthSink->Release();
    }
#endif
}

HRESULT CBufferManager::Init(CPerformance *pPerf, DMUS_AUDIOPARAMS *pAudioParams) 

{ 
    m_pPerf = pPerf; 
    m_AudioParams = *pAudioParams;
#ifdef DXAPI
    HRESULT hr = S_OK;
    CBufferNode *pNode = new CBufferNode;
    if (pNode)
    {
        pNode->m_BufferHeader.guidBufferID = GUID_Buffer_Primary;
        pNode->m_BufferHeader.dwFlags = DMUS_BUFFERF_SHARED | DMUS_BUFFERF_DEFINED | DMUS_BUFFERF_PRIMARY;
        pNode->m_pManager = this;
        AddHead(pNode);
        // Create the primary buffer. This will be used for requests to access the listener.
        DSBUFFERDESC dsbdesc; 
        memset(&dsbdesc, 0, sizeof(dsbdesc));
        dsbdesc.dwSize = sizeof(dsbdesc); 
        dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D; 

        // create primary buffer
        if (SUCCEEDED(hr = pPerf->m_pDirectSound->CreateSoundBuffer(&dsbdesc, &pNode->m_pBuffer, NULL)))
        {
            WAVEFORMATEX wfPrimary; 
            memset(&wfPrimary, 0, sizeof(wfPrimary)); 

            if (SUCCEEDED(hr = pNode->m_pBuffer->GetFormat(&wfPrimary, sizeof(wfPrimary), NULL)))
            {
                assert(wfPrimary.wFormatTag == WAVE_FORMAT_PCM);

                BOOL fUpgrade = FALSE;
                if (wfPrimary.nSamplesPerSec < m_AudioParams.dwSampleRate)
                {
                    wfPrimary.nSamplesPerSec = m_AudioParams.dwSampleRate;
                    fUpgrade = TRUE;
                }
                if (wfPrimary.wBitsPerSample < 16)
                {
                    wfPrimary.wBitsPerSample = 16;
                    fUpgrade = TRUE;
                }

                if (fUpgrade)
                {
                    wfPrimary.nBlockAlign = wfPrimary.nChannels * (wfPrimary.wBitsPerSample / 8);
                    wfPrimary.nAvgBytesPerSec = wfPrimary.nSamplesPerSec * wfPrimary.nBlockAlign;

                    // the existing format is of lesser quality than we desire, so let's upgrade it
                    if (FAILED(hr = pNode->m_pBuffer->SetFormat( &wfPrimary )))
                    {
                        if (hr == DSERR_PRIOLEVELNEEDED)
                        {
                            // okay, so maybe the app doen't want us changing primary buffer
                            Trace(0, "SynthSink - SetFormat on primary buffer failed, lacking priority\n");
                        }
                        else
                        {
                            Trace(0, "SynthSink - Activation failed, couldn't set primary buffer format\n");
                        }
                    }
                }
            }
        }
    }
    return hr;
#else
    return S_OK;
#endif

}

#ifdef DXAPI
HRESULT CBufferManager::InitSink(/*WAVEFORMATEX *pSinkFormat*/)

{
    HRESULT hr = S_OK;
    // Only init the sink if it's needed. If the audiopaths will be using buffers,
    // we need the sink. If not, bypass setting it up.
    if (!m_pSinkConnect && (m_pPerf->m_AudioParams.dwFeatures & DMUS_AUDIOF_BUFFERS))
    {
        static WAVEFORMATEX sDefaultFormat = { WAVE_FORMAT_PCM,1,SAMPLERATE,SAMPLERATE*2,2,16,0 };
        sDefaultFormat.nSamplesPerSec = m_AudioParams.dwSampleRate;
        sDefaultFormat.nAvgBytesPerSec = m_AudioParams.dwSampleRate * 2;
        sDefaultFormat.cbSize = 0;
        sDefaultFormat.nBlockAlign = 2;
        sDefaultFormat.nChannels = 1;
        sDefaultFormat.wBitsPerSample = 16;
        sDefaultFormat.wFormatTag = WAVE_FORMAT_PCM;

//#ifdef DXAPI
        IDirectSoundPrivate* pDSPrivate;
        hr = m_pPerf->m_pDirectSound->QueryInterface(IID_IDirectSoundPrivate, (void**)&pDSPrivate);
        if (SUCCEEDED(hr))
        {
            hr = pDSPrivate->AllocSink(&sDefaultFormat, &m_pSinkConnect);
            pDSPrivate->Release();
        }
        if (SUCCEEDED(hr))
        {
            IReferenceClock *pClock = NULL; 
            hr = m_pPerf->m_pDirectMusic->GetMasterClock(NULL, &pClock);
            if (SUCCEEDED(hr))
            {
                hr = m_pSinkConnect->SetMasterClock(pClock);
                pClock->Release();
            }
        }
/*#else
        CAudioSink *pSink = new CAudioSink(&sDefaultFormat);
        if (pSink)
        {
            hr = S_OK;
            pSink->QueryInterface(IID_IDirectSoundConnect,(void **) &m_pSinkConnect);
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
        // Note that for the non-DX implementation, we always get the master clock from the
        // sink and hand it back to the performance. This is the exact opposite of the 
        // DX implementation.
        if (SUCCEEDED(hr))
        {
            IReferenceClock *pClock = pSink->GetMasterClock();
            hr = m_pPerf->m_pDirectMusic->SetExternalMasterClock(pClock);
        }
#endif*/
        if (SUCCEEDED(hr))
        {
            hr = m_pSinkConnect->QueryInterface(IID_IDirectSoundSynthSink,(void **) &m_pSynthSink);
            if (SUCCEEDED(hr))
            {
                hr = m_pSynthSink->Activate(TRUE);
            }
        }
//#ifdef DXAPI
        if (SUCCEEDED(hr))
        {
            // Create a dummy buffer that gets activated at the start and stays active until
            // the sink closes down. This is an unfortunate patch to the "clock-hopping" bug.
            // Once we come up with a better solution for providing timing from the sink, this
            // can go away.
            CBufferConfig Config(BUFFER_MONO);
            hr = CreateBuffer(&Config,&m_pFirstBuffer);
            if (SUCCEEDED(hr))
            {
                hr = m_pFirstBuffer->Activate(TRUE);
            }
        }
//#endif
    }
    return hr;
} 
#endif

void CBufferManager::FinalDeactivate()
{
#ifdef DXAPI
    // Kill dummy buffer.
    if (m_pFirstBuffer)
    {
        m_pFirstBuffer->Activate(FALSE);
        delete m_pFirstBuffer;
        m_pFirstBuffer = NULL;
    }
#endif
    CBufferNode *pNode = GetHead();
    for (;pNode;pNode = pNode->GetNext())
    {
        pNode->FinalDeactivate();
    }
#ifdef DXAPI
    if (m_pSynthSink)
    {
        m_pSynthSink->Activate(FALSE);
        m_pSynthSink->Release();
        m_pSynthSink = NULL;
    }
    if (m_pSinkConnect)
    {
        m_pSinkConnect->SetMasterClock(NULL);
        m_pSinkConnect->Release();
        m_pSinkConnect = NULL;
    }
#endif
}

void CBufferManager::Clear()
{
    CBufferNode *pNode;
    FinalDeactivate();
    while (pNode = GetHead())
    {
        delete pNode;
    }
}

CBufferNode *CBufferManager::GetBufferNode(REFGUID guidBufferID)

{
    CBufferNode *pNode = GetHead();
    for (;pNode;pNode = pNode->GetNext())
    {
        if (pNode->m_BufferHeader.guidBufferID == guidBufferID)
        {
            pNode->AddRef();
            break;
        }
    }
    return pNode;
}


HRESULT CBufferManager::CreateBuffer(CBufferConfig *pConfig, CBufferConfig ** ppNew)

{
    HRESULT hr = S_OK;
    if (m_AudioParams.dwFeatures & DMUS_AUDIOF_BUFFERS)
    {
//        ASSERT(m_pSinkConnect);
        CBufferConfig *pNew = new CBufferConfig(pConfig->m_dwStandardBufferID);
        if (pNew)
        {
            *ppNew = pNew;
            pNew->m_BufferHeader = pConfig->m_BufferHeader;
            if (pConfig->m_BufferHeader.dwFlags & DMUS_BUFFERF_SHARED)
            {
                // Check to see if there already is a buffer in the list with this GUID.
                CBufferNode *pNode = GetHead();
                for (;pNode;pNode = pNode->GetNext())
                {
                    if (pNode->m_BufferHeader.dwFlags & DMUS_BUFFERF_SHARED)
                    {
                        if (pNode->m_BufferHeader.guidBufferID == pConfig->m_BufferHeader.guidBufferID)
                        {
                            pNew->m_pBufferNode = pNode;
                            pNode->AddRef();
                            TraceI(2,"Found shared Buffer %lx\n",pNode->m_pBuffer);
                            return S_OK;
                        }
                    }
                }
            }
            // Either the buffer does not already exist, or it is not to be shared, so
            // create a new buffer node and requested buffer.
            CBufferNode *pNode = new CBufferNode;
            if (pNode)
            {
                pNode->m_BufferHeader = pConfig->m_BufferHeader;
#ifdef XBOX
                pNode->m_MixBinsHeader = pConfig->m_MixBinsHeader;
#endif
#ifndef XBOX
                // Predefined buffer type?
                if (pConfig->m_BufferHeader.dwFlags & DMUS_BUFFERF_DEFINED)
                {
                    // Must be a standard type. Create by hand.
                    CBufferNode *pSendNode = NULL;
                    DSBUFFERDESC BufferDesc;
                    WAVEFORMATEX WaveFormat;
                    DWORD dwFunctionIDs[2];
                    CLSID clsidDMO;
                    BOOL fDMO = FALSE;
                    DWORD dwNumFunctionIDs;
                    memset(&BufferDesc, 0, sizeof(BufferDesc));
                    BufferDesc.dwSize  = sizeof(BufferDesc);
                    BufferDesc.lpwfxFormat = &WaveFormat;
                    BufferDesc.dwBufferBytes = 0;
                    BufferDesc.dwFlags = 0;
                    memset(&WaveFormat,0,sizeof(WaveFormat));
                    WaveFormat.nChannels = 2;
                    switch (pConfig->m_dwStandardBufferID)
                    {
                    case BUFFER_REVERB :
                    case BUFFER_CHORUS :
                        dwFunctionIDs[0] = DSBUSID_REVERB_SEND;
                        dwNumFunctionIDs = 1;
                        WaveFormat.nChannels = 2;
                        BufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_LOCDEFER;
                        clsidDMO = GUID_DSFX_WAVES_REVERB;
                        fDMO = TRUE;
                        break;
                    case BUFFER_ENVREVERB :
                        dwNumFunctionIDs = 0;
                        WaveFormat.nChannels = 2;
                        BufferDesc.dwFlags = DSBCAPS_MIXIN | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
                        clsidDMO = GUID_DSFX_STANDARD_I3DL2REVERB;
                        fDMO = TRUE;
                        if (!(m_AudioParams.dwFeatures & DMUS_AUDIOF_BUFFERS))
                        {
                            hr = DMUS_E_AUDIOPATH_NOBUFFER;
                            Trace(1,"Audiopath Creation error: Requested Environmental reverb buffer when DMUS_AUDIOF_ENVIRON not enabled via InitAudio.\n"); 
                        }
                        break;
                        // Following removed for DX8, should be reintroduced for Whistler and DX8.1...
                    case BUFFER_3D :
                        clsidDMO = GUID_DSFX_STANDARD_I3DL2SOURCE;
                        fDMO = TRUE;
                        pSendNode = GetBufferNode(GUID_Buffer_EnvReverb);
                        if (!pSendNode)
                        {
                            Trace(1,"Error: Failed creating 3D audiopath because the environmental reverb audiopath has not been created.\n");
                            hr = DMUS_E_AUDIOPATH_NOGLOBALFXBUFFER;
                            break;
                        }
                    case BUFFER_3D_DRY :
                        dwFunctionIDs[0] = DSBUSID_DYNAMIC_0;
                        dwNumFunctionIDs = 1;
                        WaveFormat.nChannels = 1;
                        BufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
                            DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE | DSBCAPS_LOCDEFER;
                        BufferDesc.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
                        break;
                    case BUFFER_MONO :
                        dwFunctionIDs[0] = DSBUSID_LEFT;
                        dwNumFunctionIDs = 1;
                        WaveFormat.nChannels = 1;
                        BufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_LOCDEFER;
                        break;
                    case BUFFER_STEREO :
                        BufferDesc.dwFlags = DSBCAPS_CTRLFREQUENCY;
                    case BUFFER_MUSIC :
                        dwFunctionIDs[0] = DSBUSID_LEFT;
                        dwFunctionIDs[1] = DSBUSID_RIGHT;
                        dwNumFunctionIDs = 2;
                        WaveFormat.nChannels = 2;
                        BufferDesc.dwFlags |= DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_LOCDEFER;
                        break;
                    default:
                        hr = E_INVALIDARG;
                    }
                    if (SUCCEEDED(hr))
                    {
                        WaveFormat.nBlockAlign = WaveFormat.nChannels * 2;
                        WaveFormat.nSamplesPerSec = m_AudioParams.dwSampleRate;
                        WaveFormat.nAvgBytesPerSec = WaveFormat.nChannels * WaveFormat.nSamplesPerSec * 2;
                        WaveFormat.wBitsPerSample = 16;
                        WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
                        // Ensure that the mixin flag and number of bus ids are in sync (you can't combine mixin with buses.)
                        if ((pConfig->m_BufferHeader.dwFlags & DMUS_BUFFERF_MIXIN) || (dwNumFunctionIDs == 0))
                        {
                            dwNumFunctionIDs = 0;
                            BufferDesc.dwFlags |= DSBCAPS_MIXIN;
                            pConfig->m_BufferHeader.dwFlags |= DMUS_BUFFERF_MIXIN;
                            BufferDesc.dwFlags &= ~DSBCAPS_LOCDEFER;
                        }
                        hr = m_pSinkConnect->CreateSoundBuffer(&BufferDesc, &dwFunctionIDs[0], dwNumFunctionIDs,
                                                               pConfig->m_BufferHeader.guidBufferID, &pNode->m_pBuffer);
#ifdef DBG
                        if (FAILED(hr))
                        {
                            if (pConfig->m_BufferHeader.dwFlags & DMUS_BUFFERF_MIXIN)
                            {
                                Trace(1,"Error: Failed creating global (MIXIN) buffer for standard path.\n");
                            }
                            else
                            {
                                Trace(1,"Error: Failed creating buffer for standard path.\n");
                            }
                        }
#endif
                    }
                    if (SUCCEEDED(hr))
                    {
                        if (fDMO)
                        {
                            IDirectSoundBuffer8 *pBuff8;
                            if (SUCCEEDED(pNode->m_pBuffer->QueryInterface(IID_IDirectSoundBuffer8,(void **)&pBuff8)))
                            {
                                DWORD dwResult;
                                DSEFFECTDESC FXDesc;
                                FXDesc.dwSize = sizeof(DSEFFECTDESC);
                                FXDesc.dwFlags = 0;
                                FXDesc.guidDSFXClass = clsidDMO;
                                FXDesc.dwReserved1 = 0;
                                FXDesc.dwReserved2 = 0;
                                if (pSendNode)
                                {
                                    FXDesc.dwReserved1 = DWORD_PTR(pSendNode->GetBuffer());
                                }
                                hr = pBuff8->SetFX(1, &FXDesc, &dwResult);
                                if (FXDesc.dwReserved1)
                                {
                                    ((IDirectSoundBuffer*)FXDesc.dwReserved1)->Release();
                                }
                                pBuff8->Release();
                            }
                        }
                    }
                    if (pSendNode)
                    {
                        pSendNode->Release();
                    }
                }
                else
                {
                    pConfig->m_BufferHeader.dwFlags &= ~DMUS_BUFFERF_MIXIN;
                    hr = m_pSinkConnect->CreateSoundBufferFromConfig(pConfig->m_pBufferConfig,&pNode->m_pBuffer);
                    if (SUCCEEDED(hr))
                    {
                        // We need to know if this is a mixin buffer so we can identify it later.
                        DWORD dwBusIDs[32];
                        DWORD dwFuncIDs[32];
                        DWORD dwCount = 32;
                        dwFuncIDs[0] = 0;
                        if (SUCCEEDED(m_pSinkConnect->GetSoundBufferBusIDs((CBuffer *)pNode->m_pBuffer,dwBusIDs,dwFuncIDs,&dwCount)))
                        {
                            if (dwFuncIDs[0] == 0xFFFFFFFF)
                            {
                                pConfig->m_BufferHeader.dwFlags |= DMUS_BUFFERF_MIXIN;
                            }
                        }
                    }   
                    else
                    {
                        Trace(1,"Error: Failed creation of buffer defined in audio path configuration file.\n");
                    }
                }
#else   // XBOX
                // Predefined buffer type?
                if (pConfig->m_BufferHeader.dwFlags & DMUS_BUFFERF_DEFINED)
                {
                    // Must be a standard type. Create by hand.
                    DWORD dwMixBin = 0;
                    DSBUFFERDESC BufferDesc;
                    BufferDesc.lpwfxFormat = NULL;
                    BufferDesc.dwSize = sizeof(BufferDesc);
                    BufferDesc.dwBufferBytes = 0;
                    BufferDesc.dwInputMixBin = 0;
                    BufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_MIXIN;

                    LPDIRECTSOUND pDSound;
                    switch (pConfig->m_dwStandardBufferID)
                    {
                    case BUFFER_REVERB :
                        dwMixBin = (1 << DSMIXBIN_FXSEND_0);

                        //
                        // set mixbin headroom to 2 (to reduce it by another 6db)
                        // since we duplicate the reverb mixbin on the mixbin mask
                        // causing any sounds send to it to double. 
                        //

                        m_pPerf->m_pDirectSound->SetMixBinHeadroom(DSMIXBIN_FXSEND_0,2);

                        break;
                    case BUFFER_CHORUS :
                        dwMixBin = (1 << DSMIXBIN_FXSEND_1);

                        //
                        // same for chorus
                        //

                        m_pPerf->m_pDirectSound->SetMixBinHeadroom(DSMIXBIN_FXSEND_1,2);

                        break;
//                    case BUFFER_ENVREVERB :
//                        break;      // Not supported.
                    case BUFFER_3D :
                    case BUFFER_3D_DRY :
                        BufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D | DSBCAPS_MIXIN;
                        break;
                    case BUFFER_MONO :
//                        dwBufferMixBin = DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT | DSMIXBIN_FRONT_CENTER | DSMIXBIN_BACK_LEFT | DSMIXBIN_BACK_RIGHT | DSMIXBIN_LOW_FREQUENCY;
                        break;
                    case BUFFER_STEREO :
//                        dwBufferMixBin = DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT;
                        break;
                    case BUFFER_MUSIC :
                        dwMixBin = (1 << DSMIXBIN_FRONT_LEFT) | (1 << DSMIXBIN_FRONT_RIGHT);
                        break;
                    case BUFFER_MIXBINS :
                        dwMixBin = pConfig->m_MixBinsHeader.dwMixBins;
                        break;
                    default:
                        hr = E_INVALIDARG;
                    }
                    if (SUCCEEDED(hr))
                    {
                        if (!dwMixBin)
                        {
                             //Right before CreateSoundBuffer, we translate the DWORD into a CMixBins class which we then 
                             // cast to an LPCDSMIXBINS struct (very fast).
                             CMIXBINS MixBins;
                             MixBins.CreateFromMask(0);
                             BufferDesc.lpMixBins = MixBins.GetMixBins();                        
                             hr = m_pPerf->m_pDirectSound->CreateSoundBuffer(&BufferDesc,&pNode->m_pBuffer,NULL);
                        }
                        pNode->m_dwMixBin = dwMixBin;
                    }
                }
                else
                {
                    pConfig->m_BufferHeader.dwFlags &= ~DMUS_BUFFERF_MIXIN;
//                    hr = m_pSinkConnect->CreateSoundBufferFromConfig(pConfig->m_pBufferConfig,&pNode->m_pBuffer);
                }
#endif  // XBOX

                if (SUCCEEDED(hr))
                {
                    TraceI(2,"Created Buffer %lx\n",pNode->m_pBuffer);
                    pNew->m_pBufferNode = pNode;
                    pNode->m_pManager = this;
                    AddHead(pNode);
                }
                else
                {
                    delete pNode;
                }
            }
            else
            {
                hr = E_OUTOFMEMORY;
            }
        }
        if (FAILED(hr) && pNew)
        {
            delete pNew;
            *ppNew = NULL;
        }
    }
    else
    {
        hr = DMUS_E_AUDIOPATH_NOBUFFER;
        Trace(0,"Audiopath Creation error: Requested buffer when DMUS_AUDIOF_BUFFERS not enabled via InitAudio.\n"); 
    }
    return hr;
}

CBufferConnect::CBufferConnect()

{
    m_ppBufferNodes = NULL;
    m_pguidBufferIDs = NULL;
    m_ConnectHeader.dwBufferCount = 0;
}

HRESULT CBufferConnect::Load(CRiffParser *pParser)

{
    HRESULT hr = pParser->Read(&m_ConnectHeader, sizeof(DMUS_IO_PCHANNELTOBUFFER_HEADER));
    if (SUCCEEDED(hr))
    {
        DWORD dwBufferCount = m_ConnectHeader.dwBufferCount;
        m_pguidBufferIDs = new GUID[dwBufferCount];
        if (m_pguidBufferIDs )
        {
            hr = pParser->Read(m_pguidBufferIDs,sizeof(GUID)*dwBufferCount);
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }
    return hr;
}

CBufferConnect *CBufferConnect::CreateRunTimeVersion(CPortConfig *pParent)

{ 
    CBufferConnect *pCopy = new CBufferConnect;
    if (pCopy && m_pguidBufferIDs)
    {
        pCopy->m_ConnectHeader = m_ConnectHeader;
        pCopy->m_ppBufferNodes = new CBufferNode *[m_ConnectHeader.dwBufferCount];
        if (pCopy->m_ppBufferNodes)
        {
            for (DWORD dwIndex = 0; dwIndex < m_ConnectHeader.dwBufferCount; dwIndex++)
            {
                pCopy->m_ppBufferNodes[dwIndex] = pParent->GetBufferNode(m_pguidBufferIDs[dwIndex]);
            }
        }
        else
        {
            delete pCopy;
            pCopy = NULL;
        }
    }
    return pCopy;
}


CBufferConnect::~CBufferConnect()

{
    if (m_ppBufferNodes)
    {
        for (DWORD dwIndex = 0; dwIndex < m_ConnectHeader.dwBufferCount; dwIndex++)
        {
            if (m_ppBufferNodes[dwIndex]) m_ppBufferNodes[dwIndex]->Release();
        }
        delete [] m_ppBufferNodes;
    }
    if (m_pguidBufferIDs)
    {
        delete [] m_pguidBufferIDs;
    }
}

HRESULT CBufferConnectList::CreateRunTimeVersion(CBufferConnectList *pDestination, CPortConfig *pParent)

{
    CBufferConnect *pScan = GetHead();
    CBufferConnect *pCopy;
    for (;pScan;pScan = pScan->GetNext())
    {
        pCopy = pScan->CreateRunTimeVersion(pParent);
        if (pCopy)
        {
            pDestination->AddTail(pCopy);
        }
        else
        {
            return E_OUTOFMEMORY;
        }
    }
    return S_OK;
}

void CBufferConnectList::Clear()

{
    CBufferConnect *pBuffer;
    while (pBuffer = RemoveHead())
    {
        delete pBuffer;
    }
}

HRESULT CBufferConfig::Activate(BOOL fActivate)

{
    if (m_pBufferNode)
    {
        return m_pBufferNode->Activate(fActivate);
    }
    return S_OK;
}

CBufferConfig::CBufferConfig(DWORD dwType)

{
    m_pBufferNode = NULL;
    m_BufferHeader.guidBufferID = GUID_NULL;
    m_BufferHeader.dwFlags = DMUS_BUFFERF_DEFINED;
    m_dwStandardBufferID = dwType;
#ifdef XBOX
    m_MixBinsHeader.bControllers[0] = 0;
#endif
    switch (dwType)
    {
    case BUFFER_REVERB :
        m_BufferHeader.guidBufferID = GUID_Buffer_Reverb;
        m_BufferHeader.dwFlags = DMUS_BUFFERF_SHARED | DMUS_BUFFERF_DEFINED;
        break;
    case BUFFER_ENVREVERB :
        m_BufferHeader.guidBufferID = GUID_Buffer_EnvReverb;
        m_BufferHeader.dwFlags = DMUS_BUFFERF_SHARED | DMUS_BUFFERF_DEFINED | DMUS_BUFFERF_MIXIN;
        break;
    case BUFFER_3D_DRY :
        m_BufferHeader.guidBufferID = GUID_Buffer_3D_Dry;
        break;
    case BUFFER_MONO :
        m_BufferHeader.guidBufferID = GUID_Buffer_Mono;
        break;
    case BUFFER_STEREO :
        m_BufferHeader.guidBufferID = GUID_Buffer_Stereo;
        break;
    case BUFFER_MUSIC :
        m_BufferHeader.guidBufferID = GUID_Buffer_Stereo;
        m_BufferHeader.dwFlags = DMUS_BUFFERF_SHARED | DMUS_BUFFERF_DEFINED;
        break;
#ifndef DXAPI
    case BUFFER_3D :
        m_BufferHeader.guidBufferID = GUID_Buffer_3D;
        break;
    case BUFFER_CHORUS :
        m_BufferHeader.guidBufferID = GUID_Buffer_Chorus;
        m_BufferHeader.dwFlags = DMUS_BUFFERF_SHARED | DMUS_BUFFERF_DEFINED;
        break;
    case BUFFER_MIXBINS :
        m_BufferHeader.guidBufferID = GUID_Buffer_MixBins;
        break;
#endif
    default:
        m_BufferHeader.dwFlags = 0;
        break;
    }
    m_pBufferConfig = NULL;
}

CBufferConfig::~CBufferConfig()

{
    if (m_pBufferNode)
        m_pBufferNode->Release();
    if (m_pBufferConfig)
        m_pBufferConfig->Release();
}

void CBufferConfig::DecideType()

{
    if (m_BufferHeader.guidBufferID == GUID_Buffer_Reverb)
    {
        m_dwStandardBufferID = BUFFER_REVERB;
    }
    else if (m_BufferHeader.guidBufferID == GUID_Buffer_EnvReverb)
    {
        m_dwStandardBufferID = BUFFER_ENVREVERB;
    }
#ifndef DXAPI
    else if (m_BufferHeader.guidBufferID == GUID_Buffer_3D)
    {
        m_dwStandardBufferID = BUFFER_3D;
    }
    else if (m_BufferHeader.guidBufferID == GUID_Buffer_Chorus)
    {
        m_dwStandardBufferID = BUFFER_CHORUS;
    }
#endif
    else if (m_BufferHeader.guidBufferID == GUID_Buffer_3D_Dry)
    {
        m_dwStandardBufferID = BUFFER_3D_DRY;
    }
    else if (m_BufferHeader.guidBufferID == GUID_Buffer_Mono)
    {
        m_dwStandardBufferID = BUFFER_MONO;
    }
    else if (m_BufferHeader.guidBufferID == GUID_Buffer_Stereo)
    {
        m_dwStandardBufferID = BUFFER_STEREO;
    }
#ifdef XBOX
    else if (m_BufferHeader.guidBufferID == GUID_Buffer_MixBins)
    {
        m_dwStandardBufferID = BUFFER_MIXBINS;
    }
#endif
}



HRESULT CBufferConfig::Load(IStream *pStream)

{
#ifndef XBOX
    IPersistStream *pPersist;
    HRESULT hr = DirectMusicCreateInstance( CLSID_DirectSoundBufferConfig,
        NULL, IID_IPersistStream,
        (void**)&pPersist );
    if (SUCCEEDED(hr))
    {
        hr = pPersist->Load(pStream);
        if (SUCCEEDED(hr))
        {
            m_pBufferConfig = pPersist;
            IDirectMusicObject *pObject;
            hr = pPersist->QueryInterface(IID_IDirectMusicObject,(void **) &pObject);
            if (SUCCEEDED(hr))
            {
                DMUS_OBJECTDESC Desc;
                Desc.dwSize = sizeof(Desc);
                pObject->GetDescriptor(&Desc);
                if (Desc.dwValidData & DMUS_OBJ_OBJECT)
                {
                    m_BufferHeader.guidBufferID = Desc.guidObject;
                }
                else
                {
                    Trace(1,"Error: Unable to load Buffer Configuration in AudioPath Config - Missing buffer GUID in file.\n");
                    hr = E_FAIL;
                }
                pObject->Release();
            }
        }
        else
        {
            pPersist->Release();
        }
    }
    return hr;
#else // XBOX
    Trace(1,"Error: Attempt to load an effects configuration within an AudioPath. XBOX does not support these yet. File load will fail.\n");
	return E_FAIL;
#endif
}

void CBufferConfigList::Clear()

{
    CBufferConfig *pBuffer;
    while (pBuffer = RemoveHead())
    {
        delete pBuffer;
    }
}

CBufferNode * CBufferConfigList::GetBufferNode(REFGUID guidBufferID)

{
    CBufferNode *pBuff = NULL;
    CBufferConfig *pBuffer;
    for (pBuffer = GetHead();pBuffer;pBuffer = pBuffer->GetNext())
    {
        if (pBuffer->m_BufferHeader.guidBufferID == guidBufferID)
        {
            if (pBuffer->m_pBufferNode)
            {
                pBuffer->m_pBufferNode->AddRef();
                pBuff = pBuffer->m_pBufferNode;
            }
            break;
        }
    }
    return pBuff;
}

HRESULT CBufferConfigList::Activate(BOOL fActivate)

{
    HRESULT hr = S_OK;
    CBufferConfig *pBuffer;
    for (pBuffer = GetHead();pBuffer;pBuffer = pBuffer->GetNext())
    {
        hr = pBuffer->Activate(fActivate);
        if (FAILED(hr))
        {
            CBufferConfig *pUndo;
            for (pUndo = GetHead();pUndo && (pUndo != pBuffer);pUndo = pUndo->GetNext())
            {
                pUndo->Activate(!fActivate);
            }
            break;
        }
    }
    return hr;
}

HRESULT CBufferConfigList::CreateRunTimeVersion(CBufferConfigList *pCopy, CBufferManager *pManager)

/*  To create a runtime version, we scan through all bufferconfigs and, for each one, we call the
    buffer manager to create a new one, managed by CBufferNode. In the case where the buffer already exists, it just
    addrefs the CBufferNode and returns that. 
*/

{
    HRESULT hr = S_OK;
    CBufferConfig *pBuffer;
    for (pBuffer = GetHead();pBuffer;pBuffer = pBuffer->GetNext())
    {
        CBufferConfig *pNew = NULL;
        hr = pManager->CreateBuffer(pBuffer,&pNew);
        if (SUCCEEDED(hr))
        {
            pCopy->AddTail(pNew);
        }
        else
        {
            break;
        }
    }
    return hr;
}


CPortConfig::CPortConfig()

{ 
    m_fAlreadyHere = FALSE;
    m_pPort = NULL;
    m_dwPortID = 0;
    m_pParent = NULL;
    m_PortHeader.guidPort = GUID_Synth_Default;     // Default synth, as specified by DMUS_AUDPARAMS.
    m_PortHeader.dwFlags = DMUS_PORTCONFIGF_DRUMSON10;
    m_PortHeader.dwPChannelBase = 0;
    m_PortHeader.dwPChannelCount = 32;
    m_PortParams.dwChannelGroups = 2;
    m_PortParams.dwSize = sizeof(DMUS_PORTPARAMS8);
    m_PortParams.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS | DMUS_PORTPARAMS_FEATURES;
    m_PortParams.dwFeatures = DMUS_PORT_FEATURE_AUDIOPATH | DMUS_PORT_FEATURE_STREAMING;
}

CPortConfig::~CPortConfig()

{
    if (m_pPort) m_pPort->Release();
    m_BufferConnectList.Clear();
    m_BufferConfigList.Clear();
}


HRESULT CPortConfig::Activate(BOOL fActivate)

{
    HRESULT hr = m_BufferConfigList.Activate(fActivate);
    if (SUCCEEDED(hr) && fActivate && m_pPort)
    {
        HRESULT hrTemp = m_pPort->Activate(TRUE);
        if (FAILED(hrTemp))
        {
            hr = hrTemp;
        }
    }
    return hr;
}

HRESULT CPortConfig::CreateRunTimeVersion(CPortConfig ** ppCopy, CAudioPath *pParent, CBufferManager *pManager)

{
    HRESULT hr = E_OUTOFMEMORY;
    CPortConfig *pCopy = new CPortConfig();
    if (pCopy)
    {
        pCopy->m_pParent = pParent;
        pCopy->m_PortHeader = m_PortHeader;
        pCopy->m_PortParams = m_PortParams;
        hr = m_BufferConfigList.CreateRunTimeVersion(&pCopy->m_BufferConfigList,pManager);
        if (SUCCEEDED(hr))
        {
            hr = m_BufferConnectList.CreateRunTimeVersion(&pCopy->m_BufferConnectList,pCopy);
        }
    }
    *ppCopy = pCopy;
    return hr;
}

CBufferNode * CPortConfig::GetBufferNode(REFGUID guidBuffer)

{
    CBufferNode *pBuff = NULL;
    pBuff = m_BufferConfigList.GetBufferNode(guidBuffer);
    if (!pBuff && !m_fAlreadyHere)
    {
        m_fAlreadyHere = TRUE;
        pBuff = m_pParent->GetBufferNode(guidBuffer);
        m_fAlreadyHere = FALSE;
    }
    return pBuff;
} 


HRESULT CPortConfig::Load(CRiffParser *pParser)
{
    RIFFIO ckNext;
    HRESULT hr = S_OK;
    DWORD dwLoadedBoth = 0;
    pParser->EnterList(&ckNext);
    while (pParser->NextChunk(&hr))
    {
        switch(ckNext.ckid)
        {
        case DMUS_FOURCC_PORTCONFIG_ITEM:
            hr = pParser->Read(&m_PortHeader, sizeof(DMUS_IO_PORTCONFIG_HEADER));
            dwLoadedBoth |= 1;
            break;
        case DMUS_FOURCC_PORTPARAMS_ITEM:
            hr = pParser->Read(&m_PortParams, sizeof(DMUS_PORTPARAMS8));
            dwLoadedBoth |= 2;
            break;
        case FOURCC_LIST:
        case FOURCC_RIFF:
            switch(ckNext.fccType)
            {
                RIFFIO ckChild;
                case DMUS_FOURCC_PCHANNELS_LIST:
                    pParser->EnterList(&ckChild);
                    while (pParser->NextChunk(&hr))
                    {
                        switch( ckChild.ckid )
                        {
                        case DMUS_FOURCC_PCHANNELS_ITEM:
                            {
                                CBufferConnect *pBufferConnect = new CBufferConnect;
                                if (pBufferConnect)
                                {
                                    hr = pBufferConnect->Load(pParser);
                                    if (SUCCEEDED(hr))
                                    {
                                        m_BufferConnectList.AddTail(pBufferConnect);
                                    }
                                }
                                else
                                {
                                    hr = E_OUTOFMEMORY;
                                }
                            }
                            break;
                        }
                    }
                    pParser->LeaveList();
                    break;
                case DMUS_FOURCC_DSBUFFER_LIST:
                    {
                        CBufferConfig *pSource = new CBufferConfig(0);
                        if (pSource)
                        {
                            pParser->EnterList(&ckChild);
                            while (pParser->NextChunk(&hr))
                            {
                                switch( ckChild.ckid )
                                {
                                case DMUS_FOURCC_DSBUFFATTR_ITEM:
                                    hr = pParser->Read(&pSource->m_BufferHeader, 
                                        sizeof(DMUS_IO_BUFFER_ATTRIBUTES_HEADER));
                                    pSource->DecideType();
                                    break;
                                case DMUS_FOURCC_MIXBINS_ITEM:
                                    hr = pParser->Read(&pSource->m_MixBinsHeader, 
                                        sizeof(DMUS_IO_BUFFER_MIXBINS_HEADER));
                                    break;
                                case FOURCC_LIST:
                                case FOURCC_RIFF:
                                    if ( ckChild.fccType == DMUS_FOURCC_DSBC_FORM)
                                    {
                                        pParser->SeekBack();
                                        hr = pSource->Load(pParser->GetStream());
                                        pParser->SeekForward();
                                        if (FAILED(hr))
                                        {
                                            Trace(1,"AudioPath Configuration failed loading buffer\n");
                                        }
                                    }
                                }
                            }
                            pParser->LeaveList();
                            if (SUCCEEDED(hr))
                            {
                                m_BufferConfigList.AddTail(pSource);
                            }
                            else
                            {
                                delete pSource;
                            }
                        }
                        else
                        {
                            hr = E_OUTOFMEMORY;
                        }
                    }
                    break;
                case DMUS_FOURCC_DSBC_FORM:
                    {
                        CBufferConfig *pSource = new CBufferConfig(0);
                        if (pSource)
                        {
                            pParser->SeekBack();
                            hr = pSource->Load(pParser->GetStream());
                            pParser->SeekForward();
                            if (SUCCEEDED(hr))
                            {
                                m_BufferConfigList.AddTail(pSource);
                            }
                            else
                            {
                                Trace(1,"AudioPath Configuration failed loading buffer\n");
                                delete pSource;
                            }
                        }
                        else
                        {
                            hr = E_OUTOFMEMORY;
                        }
                    }
                    break;                   
                default:
                    break;
            }
            break;
        default:
            break;
        }
    }
    if (dwLoadedBoth != 3) 
    {
        hr = DMUS_E_CHUNKNOTFOUND;
        Trace(1,"Error: Failure loading port configuration chunk in Audio Path Configuration.\n");
    }

    // Make sure the channel groups in the portparams is large enough to handle the requested
    // channels in portheader.
    m_PortParams.dwChannelGroups = (m_PortHeader.dwPChannelCount + 15) / 16;
    m_PortParams.dwValidParams |= DMUS_PORTPARAMS_CHANNELGROUPS;
    pParser->LeaveList();

    return hr;
}

HRESULT CPortConfigList::Activate(BOOL fActivate)

{
    HRESULT hr = S_OK;
    CPortConfig *pPort;
    for (pPort = GetHead();pPort;pPort = pPort->GetNext())
    {
        hr = pPort->Activate(fActivate);
        if (FAILED(hr))
        {
            CPortConfig *pUndo;
            for (pUndo = GetHead();pUndo && (pUndo != pPort);pUndo = pUndo->GetNext())
            {
                pUndo->Activate(!fActivate);
            }
            break;
        }
    }
    return hr;
}

HRESULT CPortConfigList::CreateRunTimeVersion(CPortConfigList *pDestination,CAudioPath *pParent,CBufferManager *pManager)

{
    HRESULT hr = S_OK;
    CPortConfig *pScan = GetHead();
    CPortConfig *pCopy;
    for (;pScan;pScan = pScan->GetNext())
    {
        hr = pScan->CreateRunTimeVersion(&pCopy,pParent,pManager);
        if (pCopy)
        {
            pDestination->AddTail(pCopy);
        }
        else
        {
            break;
        }
    }
    return hr;
}

BOOL CPortConfigList::UsesPort(IDirectMusicPort *pPort)

{
    CPortConfig *pScan = GetHead();
    for (;pScan;pScan = pScan->GetNext())
    {
        if (pScan->m_pPort == pPort) return TRUE;
    } 
    return FALSE;
}


void CPortConfigList::Clear()

{
    CPortConfig *pPort;
    while (pPort = RemoveHead())
    {
        delete pPort;
    }
}

CAudioPath::CAudioPath() : m_MemTrack(DMTRACK_AUDIOPATH)

{
    TraceI(2,"Creating AudioPath %lx\n",this);
    INITIALIZE_CRITICAL_SECTION(&m_CriticalSection);
    m_fDeactivating = FALSE;
    m_rtLastVolChange = 0;
    m_rtLastPitchChange = 0;
    m_cRef = 0;
    m_fActive = FALSE;
    m_pdwVChannels = NULL;
    m_pdwPChannels = NULL;
    m_dwChannelCount = 0;
    m_pPerformance = NULL;
    m_pGraph = NULL;
    m_pConfig = NULL;
    m_pUnkDispatch = NULL;
}


CAudioPath::~CAudioPath()
{
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    if (m_pUnkDispatch)
    {
        m_pUnkDispatch->Release(); // free IDispatch implementation we may have borrowed
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
    Deactivate();
    DELETE_CRITICAL_SECTION(&m_CriticalSection);
}

STDMETHODIMP_(ULONG) CAudioPath::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CAudioPath::Release()
{
    if (!InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

STDMETHODIMP CAudioPath::QueryInterface(
    const IID &iid,   
    void **ppv)       
{
    V_INAME(CAudioPath::QueryInterface);
    V_PTRPTR_WRITE(ppv);
    V_REFGUID(iid);

    *ppv = NULL;
    if (iid == IID_IUnknown || iid == IID_IDirectMusicAudioPath)
    {
        *ppv = static_cast<IDirectMusicAudioPath*>(this);
    } else
    if (iid == IID_CAudioPath)
    {
        *ppv = static_cast<CAudioPath*>(this);
    } else 
    if (iid == IID_IDirectMusicGraph)
    {
        *ppv = static_cast<IDirectMusicGraph*>(this);
    }
    else if (iid == IID_IDispatch)
    {
        // A helper scripting object implements IDispatch, which we expose via COM aggregation.
        if (!m_pUnkDispatch)
        {
            // Create the helper object
            DirectMusicCreateInstance(CLSID_AutDirectMusicAudioPath,
                static_cast<IDirectMusicAudioPath*>(this),
                IID_IUnknown,
                reinterpret_cast<void**>(&m_pUnkDispatch));
        }
        if (m_pUnkDispatch)
        {
            return m_pUnkDispatch->QueryInterface(IID_IDispatch, ppv);
        }
    }

    if (*ppv == NULL)
    {
        Trace(4,"Warning: Request to query unknown interface on AudioPath object\n");
        return E_NOINTERFACE;
    }

    reinterpret_cast<IUnknown*>(this)->AddRef();
    return S_OK;
}

STDMETHODIMP CAudioPath::SetVolume(long lVolume,DWORD dwDuration) 

{
    V_INAME(AudioPath::SetVolume);
#if defined(DBG) || defined(DXAPI)
    if (lVolume < DSBVOLUME_MIN || lVolume > DSBVOLUME_MAX)
    {
        RIP_E_INVALIDARG(lVolume)
    }
#endif
    HRESULT hr = E_FAIL; // This should never happen, since the audiopath is created by the performance. 
    BYTE bMIDIVol = CParamMerger::VolumeToMidi(lVolume);

    DMUS_CURVE_PMSG *pCurve;
    // Clamp to an hour!
    if (dwDuration > (60 * 60 * 1000)) dwDuration = (60 * 60 * 1000);
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    ENTER_API_CRITICAL_SECTION;
    if (m_pPerformance)
    {
        hr = m_pPerformance->AllocPMsg(sizeof(DMUS_CURVE_PMSG),(DMUS_PMSG **)&pCurve);
        if (SUCCEEDED(hr))
        {
            REFERENCE_TIME rtTimeNow = 0;
            MUSIC_TIME mtNow;
            m_pPerformance->GetQueueTime(&rtTimeNow);
            // Get the equivalent music time so each copy of the curve doesn't need to recompute.
            m_pPerformance->ReferenceToMusicTime(rtTimeNow,&mtNow);
            // If this starts before a previous fade ended, invalidate it.
            if (m_rtLastVolChange > rtTimeNow)
            {
                m_pPerformance->FlushVirtualTrack(m_dwTrackID,mtNow,0);
            }
            if (m_rtLastVolChange < (rtTimeNow + (dwDuration * 10000)))
            {
                m_rtLastVolChange = rtTimeNow + (dwDuration * 10000);
            }
			pCurve->dwVirtualTrackID = m_dwTrackID;
            pCurve->rtTime = rtTimeNow;
            pCurve->mtTime = mtNow;
            pCurve->dwFlags = DMUS_PMSGF_REFTIME | DMUS_PMSGF_MUSICTIME | 
                DMUS_PMSGF_LOCKTOREFTIME | DMUS_PMSGF_DX8;
            pCurve->dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
            // dwVirtualTrackID: this isn't a track so leave as 0
            pCurve->dwType = DMUS_PMSGT_CURVE;
            pCurve->dwGroupID = -1; // this isn't a track so just say all groups

            // curve PMsg fields
            pCurve->mtDuration = dwDuration; // setting the DMUS_PMSGF_LOCKTOREFTIME is interpreted by the performance that mtDuration is milliseconds
            // mtResetDuration: no reset so leave as 0
            pCurve->nStartValue = 127; //m_bLastVol;
            // nStartValue: will be ignored
            pCurve->nEndValue = bMIDIVol;
            // nResetValue: no reset so leave as 0
            pCurve->bType = DMUS_CURVET_CCCURVE;
            /*if (dwDuration)
            {
                if (bMIDIVol > 87)
                {
                    // If fading up, make better for crossfade by fading in quickly at first.
                    pCurve->bCurveShape = DMUS_CURVES_LOG;
                }
                else if (bMIDIVol < 40)
                {
                    // If fading out, make better for crossfade by fading out slowly at first.
                    pCurve->bCurveShape = DMUS_CURVES_EXP;
                }
                else
                {
                    pCurve->bCurveShape = DMUS_CURVES_LINEAR;
                }
            }
            else
            {
                pCurve->bCurveShape = DMUS_CURVES_INSTANT;
            }*/
            pCurve->bCurveShape = dwDuration ? DMUS_CURVES_LINEAR : DMUS_CURVES_INSTANT;
            pCurve->bCCData = 7; // MIDI volume controller number
            pCurve->bFlags = DMUS_CURVE_START_FROM_CURRENT;
            // wParamType: leave as zero since this isn't a NRPN/RPN curve
            pCurve->wMergeIndex = 0xFFFE; // §§ special merge index so this won't get stepped on. is a big number OK? define a constant for this value?

            // send it

            StampPMsg((DMUS_PMSG *)pCurve);
            hr = m_pPerformance->SendPMsg((DMUS_PMSG*)pCurve);
        }
    }
    else
    {
        hr = DMUS_E_NOT_INIT;
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
    LEAVE_API_CRITICAL_SECTION;
    return hr;
}

STDMETHODIMP CAudioPath::SetPitch(long lPitch,DWORD dwDuration) 

{
    V_INAME(AudioPath::SetPitch);
    lPitch += 0x2000; // Bump up to center around 0x2000.
#if defined(DBG) || defined(DXAPI)
    if (lPitch < 0 || lPitch > 0x3FFF)
    {
        RIP_E_INVALIDARG(lPitch)
    }
#endif
    HRESULT hr = E_FAIL; // This should never happen, since the audiopath is created by the performance. 
    DMUS_CURVE_PMSG *pCurve;
    // Clamp to an hour!
    if (dwDuration > (60 * 60 * 1000)) dwDuration = (60 * 60 * 1000);
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    ENTER_API_CRITICAL_SECTION;
    if (m_pPerformance)
    {
        hr = m_pPerformance->AllocPMsg(sizeof(DMUS_CURVE_PMSG),(DMUS_PMSG **)&pCurve);
        if (SUCCEEDED(hr))
        {
            REFERENCE_TIME rtTimeNow = 0;
            m_pPerformance->GetQueueTime(&rtTimeNow);
            // If this starts before a previous fade ended, invalidate it.
            if (m_rtLastPitchChange > rtTimeNow)
            {
                MUSIC_TIME mtNow;
                m_pPerformance->ReferenceToMusicTime(rtTimeNow,&mtNow);
                m_pPerformance->FlushVirtualTrack(m_dwTrackID,mtNow,0);
            }
            if (m_rtLastPitchChange < (rtTimeNow + (dwDuration * 10000)))
            {
                m_rtLastPitchChange = rtTimeNow + (dwDuration * 10000);
            }
            pCurve->dwVirtualTrackID = m_dwTrackID;
            pCurve->rtTime = rtTimeNow;
            pCurve->dwFlags = DMUS_PMSGF_REFTIME | DMUS_PMSGF_LOCKTOREFTIME | DMUS_PMSGF_DX8;
            pCurve->dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
            // dwVirtualTrackID: this isn't a track so leave as 0
            pCurve->dwType = DMUS_PMSGT_CURVE;
            pCurve->dwGroupID = -1; // this isn't a track so just say all groups

            // curve PMsg fields
            pCurve->mtDuration = dwDuration; // setting the DMUS_PMSGF_LOCKTOREFTIME is interpreted by the performance that mtDuration is milliseconds
            // mtResetDuration: no reset so leave as 0
            pCurve->nStartValue = 0;
            // nStartValue: will be ignored
            pCurve->nEndValue = (short) lPitch;
            // nResetValue: no reset so leave as 0
            pCurve->bType = DMUS_CURVET_PBCURVE;
            pCurve->bCurveShape = dwDuration ? DMUS_CURVES_LINEAR : DMUS_CURVES_INSTANT;
            // bCCData: Ignore, since this is pitch bend
            pCurve->bFlags = DMUS_CURVE_START_FROM_CURRENT;
            pCurve->wParamType = 0x2000;  // PB Range equivalent to pitch cents units.
            pCurve->wMergeIndex = 0xFFFE; // §§ special merge index so this won't get stepped on. is a big number OK? define a constant for this value?

            // send it

            StampPMsg((DMUS_PMSG *)pCurve);
            hr = m_pPerformance->SendPMsg((DMUS_PMSG*)pCurve);
        }
    }
    else
    {
        hr = DMUS_E_NOT_INIT;
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
    LEAVE_API_CRITICAL_SECTION;
    return hr;
}

STDMETHODIMP CAudioPath::GetObjectInPath( DWORD dwPChannel,DWORD dwStage,
                                          DWORD dwBuffer, REFGUID guidObject,
                                          DWORD dwIndex, REFGUID iidInterface, void ** ppObject)
{
    V_INAME(IDirectMusicAudioPath::GetObjectInPath);
    V_PTRPTR_WRITE(ppObject);
    *ppObject = NULL;
#ifdef XBOX
#ifdef DBG
    if (dwBuffer && ((dwStage < DMUS_PATH_BUFFER) || (dwStage >= DMUS_PATH_PRIMARY_BUFFER)))
    {
        Trace(0,"Invalid buffer or stage id passed to GetObjectInPath\n");
    }
#endif
#else
    if (dwBuffer && ((dwStage < DMUS_PATH_BUFFER) || (dwStage >= DMUS_PATH_PRIMARY_BUFFER)))
    {
        return DMUS_E_NOT_FOUND;
    }
#endif
    HRESULT hr = DMUS_E_NOT_FOUND;
    CPortConfig *pPortConfig;
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    ENTER_API_CRITICAL_SECTION;
    switch (dwStage)
    {
    case DMUS_PATH_AUDIOPATH:
        if ((dwIndex == 0) && (dwPChannel == 0))
        {
            hr = QueryInterface(iidInterface,ppObject);
        }
        break;
    case DMUS_PATH_AUDIOPATH_GRAPH:
        if ((dwIndex == 0) && (dwPChannel == 0))
        {
            if (!m_pGraph)
            {
                m_pGraph = new CGraph;
            }
            if (m_pGraph)
            {
                hr = m_pGraph->QueryInterface(iidInterface,ppObject);
            }
            else
            {
                hr = E_OUTOFMEMORY;
            }
        }
        break;
    case DMUS_PATH_AUDIOPATH_TOOL:
        if (m_pGraph)
        {
            hr = m_pGraph->GetObjectInPath(dwPChannel,guidObject,dwIndex,iidInterface,ppObject);
        }
        break;
    case DMUS_PATH_PERFORMANCE:
        if (m_pPerformance && (dwIndex == 0) && (dwPChannel == 0))
        {
            hr = m_pPerformance->QueryInterface(iidInterface,ppObject);
        }
        break;
    case DMUS_PATH_PERFORMANCE_GRAPH:
        if (m_pPerformance && (dwIndex == 0) && (dwPChannel == 0))
        {
            IDirectMusicGraph *pGraph;
            if (SUCCEEDED(hr = m_pPerformance->GetGraphInternal(&pGraph)))
            {
                hr = pGraph->QueryInterface(iidInterface,ppObject);
                pGraph->Release();
            }
        }
        break;
    case DMUS_PATH_PERFORMANCE_TOOL:
        if (m_pPerformance)
        {
            IDirectMusicGraph *pGraph;
            if (SUCCEEDED(hr = m_pPerformance->GetGraphInternal(&pGraph)))
            {
                CGraph *pCGraph = (CGraph *) pGraph;
                // Convert from audiopath channel to performance channel.
                ConvertPChannel( dwPChannel,&dwPChannel);
                hr = pCGraph->GetObjectInPath(dwPChannel,guidObject,dwIndex,iidInterface,ppObject);
                pGraph->Release();
            }
        }
        break;
    case DMUS_PATH_PORT:
        pPortConfig = m_PortConfigList.GetHead();
        for (;pPortConfig;pPortConfig = pPortConfig->GetNext())
        {
            // First, see if this matches the port guid.
            if ((pPortConfig->m_PortHeader.guidPort == guidObject) || (guidObject == GUID_All_Objects))
            {
                // Then, see if we have a pchannel match.
                if ((dwPChannel == DMUS_PCHANNEL_ALL) ||
                    ((pPortConfig->m_PortHeader.dwPChannelBase <= dwPChannel) &&
                    ((pPortConfig->m_PortHeader.dwPChannelBase + pPortConfig->m_PortHeader.dwPChannelCount) > dwPChannel)))
                {
                    // If everything matches, there is always the chance that we have multiple instances of
                    // this in the list and we are actually looking at a second or third pointer to the same port.
                    // So, scan through the list again, making the exact same matches. If this port is found in
                    // an earlier instance, fail the match.
                    BOOL fSuccess = true;
                    CPortConfig *pScan = m_PortConfigList.GetHead();
                    for (;pScan;pScan = pScan->GetNext())
                    {
                        // First, see if this matches the port guid.
                        if ((pScan->m_PortHeader.guidPort == guidObject) || (guidObject == GUID_All_Objects))
                        {
                            // Then, see if we have a pchannel match.
                            if ((dwPChannel == DMUS_PCHANNEL_ALL) ||
                                ((pScan->m_PortHeader.dwPChannelBase <= dwPChannel) &&
                                ((pScan->m_PortHeader.dwPChannelBase + pScan->m_PortHeader.dwPChannelCount) > dwPChannel)))
                            {
                                // If this is the same as the outer loop, we have arrived. 
                                if (pScan == pPortConfig)
                                {
                                    break;
                                }
                                else
                                {
                                    // Else, if this points to the same port, we have failed.
                                    if (pScan->m_pPort == pPortConfig->m_pPort)
                                    {
                                        fSuccess = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if (fSuccess)
                    {
                        if (dwIndex)
                        {
                            dwIndex--;
                        }
                        else 
                        {
                            IDirectMusicPort *pPort;
                            if (SUCCEEDED(m_pPerformance->GetPort(pPortConfig->m_dwPortID,&pPort)))
                            {
                                hr = pPort->QueryInterface(iidInterface,ppObject);
                                pPort->Release();
                            }
                            break;
                        }
                    }
                }
            }
        }
        break;
#ifdef DXAPI
    case DMUS_PATH_SINK:
        if (m_pPerformance && (dwIndex == 0) && (dwPChannel == 0) &&
            m_pPerformance->m_BufferManager.m_pSinkConnect)
        {
            hr = m_pPerformance->m_BufferManager.m_pSinkConnect->QueryInterface(iidInterface,ppObject);
        }
        break;
#endif
    case DMUS_PATH_BUFFER:
    case DMUS_PATH_BUFFER_DMO:
        {
            CBufferConnect *pConnect = NULL;
            BOOL fAllChannels = (dwPChannel == DMUS_PCHANNEL_ALL);
            pPortConfig = m_PortConfigList.GetHead();
            for (;pPortConfig && FAILED(hr);pPortConfig = pPortConfig->GetNext())
            {
                if (fAllChannels || ((pPortConfig->m_PortHeader.dwPChannelBase <= dwPChannel) &&
                    ((pPortConfig->m_PortHeader.dwPChannelCount + 
                    pPortConfig->m_PortHeader.dwPChannelBase) > dwPChannel)))
                {
                    pConnect = pPortConfig->m_BufferConnectList.GetHead();
                    for (;pConnect;)
                    {
                        if (fAllChannels || ((pConnect->m_ConnectHeader.dwPChannelBase <= dwPChannel) &&
                            ((pConnect->m_ConnectHeader.dwPChannelCount + 
                            pConnect->m_ConnectHeader.dwPChannelBase) > dwPChannel)))
                        {
                            // Found the buffer connect. Which buffer will be determined
                            // by dwBuffer. If dwBuffer is greater than the count of buffer,
                            // decrement and move on to the next pConnect.
                            if (pConnect->m_ConnectHeader.dwBufferCount > dwBuffer) 
                            {
                                if (pConnect->m_ppBufferNodes[dwBuffer])
                                {
                                    IDirectSoundBuffer *pBuffer = pConnect->m_ppBufferNodes[dwBuffer]->GetBuffer();
                                    if (pBuffer)
                                    {
                                        if (dwStage == DMUS_PATH_BUFFER)
                                        {
                                            if (dwIndex == 0)
                                            {
#ifdef XBOX
                                                hr = S_OK;
                                                *ppObject = pBuffer;
                                                pBuffer->AddRef();
#else
                                                hr = pBuffer->QueryInterface(iidInterface,ppObject);
#endif
                                            }
                                        }
                                        else
                                        {
                                            IDirectSoundBuffer8 *pBuffer8;
#ifndef XBOX
                                            hr = pBuffer->QueryInterface(IID_IDirectSoundBuffer8,(void **) &pBuffer8);
                                            if (SUCCEEDED(hr))
                                            {
                                                hr = pBuffer8->GetObjectInPath(guidObject,dwIndex,iidInterface,ppObject);
                                                pBuffer8->Release();
                                            }
#else // XBOX
											hr = E_FAIL; // TODO: What's the right thing to do here?
#endif // XBOX
                                        }
                                        pBuffer->Release();
                                    }
                                }
                                pConnect = NULL;
                            }
                            else
                            {
                                dwBuffer -= pConnect->m_ConnectHeader.dwBufferCount;
                            }
                        }
                        if (pConnect)
                        {
                            pConnect = pConnect->GetNext();
                        }
                    }
                }
            }
            
        }
        break;
    case DMUS_PATH_MIXIN_BUFFER :
    case DMUS_PATH_MIXIN_BUFFER_DMO :
        if (dwPChannel == 0) 
        {
            CBufferConfig *pConfig = m_BufferConfigList.GetHead();
            for (;pConfig; pConfig = pConfig->GetNext())
            {
                if (pConfig->m_BufferHeader.dwFlags & DMUS_BUFFERF_MIXIN)
                {
                    if (!dwBuffer)
                    {
                        IDirectSoundBuffer *pBuffer = pConfig->m_pBufferNode->GetBuffer();
                        if (pBuffer)
                        {
                            if (dwStage == DMUS_PATH_MIXIN_BUFFER)
                            {
                                if (dwIndex == 0)
                                {
#ifdef XBOX
                                    hr = pBuffer->QueryInterface(IID_IDirectSoundBuffer,ppObject);
#else
                                    hr = pBuffer->QueryInterface(iidInterface,ppObject);
#endif
                                }
                            }
                            else
                            {
#ifndef XBOX
                                IDirectSoundBuffer8 *pBuffer8;
                                hr = pBuffer->QueryInterface(IID_IDirectSoundBuffer8,(void **) &pBuffer8);
                                if (SUCCEEDED(hr))
                                {
                                    hr = pBuffer8->GetObjectInPath(guidObject,dwIndex,iidInterface,ppObject);
                                    pBuffer8->Release();
                                }
#else // XBOX
								hr = E_FAIL; // TODO: What's the right thing to do here?
#endif // XBOX
                            }
                            pBuffer->Release();
                        }
                        break;
                    }
                    dwBuffer--;
                }
            }
        }
        break;
#ifndef XBOX
    case DMUS_PATH_PRIMARY_BUFFER :
        if ((dwIndex == 0) && (dwPChannel == 0))
        {
            CBufferNode *pNode = m_pPerformance->m_BufferManager.GetBufferNode(GUID_Buffer_Primary);
            if (pNode)
            {
                IDirectSoundBuffer *pBuffer = pNode->GetBuffer();
                if (pBuffer)
                {
                    hr = pBuffer->QueryInterface(iidInterface,ppObject);
                    pBuffer->Release();
                }
                pNode->Release();
            }
        }
        break;
#endif
    default:
        hr = E_INVALIDARG;
        Trace(0,"Error: Audiopath does not support stage 0x%lx\n",dwStage);
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
    LEAVE_API_CRITICAL_SECTION;
#ifdef DBG
    if (hr == DMUS_E_NOT_FOUND)
    {
        Trace(3,"Warning: Requested AudioPath object not found\n");
    }
#endif
    return hr;
}

HRESULT STDMETHODCALLTYPE CAudioPath::Activate(BOOL fActivate)

{
    TraceI(2,"Audiopath %lx Activate: %ld\n",this,fActivate);
    if (fActivate == m_fActive)
    {
#ifdef DBG
        if (fActivate) Trace(2,"Warning: Attempt to activate already active audiopath.\n");
        else Trace(2,"Warning: Attempt to deactivate already inactive audiopath.\n");
#endif
        return S_FALSE;
    }
    ENTER_API_CRITICAL_SECTION;
    m_fActive = fActivate;
    if (!fActivate && !m_fDeactivating)
    {
        ENTER_CRITICAL_SECTION(&m_CriticalSection);
        CPerformance *pPerf = m_pPerformance;
        LEAVE_CRITICAL_SECTION(&m_CriticalSection);
        if (pPerf)
        {
            // Kill anything currently playing on the audiopath.
            pPerf->StopEx(static_cast<IDirectMusicAudioPath*>(this),0,0);
        }
    }
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    HRESULT hr = m_BufferConfigList.Activate(fActivate);
    if (SUCCEEDED(hr))
    {
        hr = m_PortConfigList.Activate(fActivate);
        if (FAILED(hr))
        {
            m_BufferConfigList.Activate(!fActivate);
        }
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
    if (FAILED(hr))
    {
        m_fActive = !fActivate;
    }
    LEAVE_API_CRITICAL_SECTION;
    return hr;
}

HRESULT STDMETHODCALLTYPE CAudioPath::ConvertPChannel( DWORD dwPChannelIn,DWORD *pdwPChannelOut)

{
    V_INAME(IDirectMusicAudioPath::ConvertPChannel);
    V_PTR_WRITE(pdwPChannelOut,DWORD);

    // If any special PMsg address (for example, broadcast), leave as is.
    if (dwPChannelIn >= DMUS_PCHANNEL_KILL_ME)
    {
        *pdwPChannelOut = dwPChannelIn;
        return S_OK;
    }
    DWORD dwScan = 0;
    for (;dwScan < m_dwChannelCount;dwScan++)
    {
        if (m_pdwVChannels[dwScan] == dwPChannelIn)
        {
            *pdwPChannelOut = m_pdwPChannels[dwScan];
            return S_OK;
        }
    }
    Trace(1,"Error: Audiopath failed request to convert out of range PChannel %ld\n",dwPChannelIn);
    return DMUS_E_NOT_FOUND;
}

HRESULT STDMETHODCALLTYPE CAudioPath::Shutdown()
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CAudioPath::InsertTool( 
    IDirectMusicTool *pTool,
    DWORD *pdwPChannels,
    DWORD cPChannels,
    LONG lIndex)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CAudioPath::GetTool(
    DWORD dwIndex,
    IDirectMusicTool** ppTool)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CAudioPath::RemoveTool(
    IDirectMusicTool* pTool)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CAudioPath::StampPMsg( 
    /* [in */ DMUS_PMSG* pPMsg)
{
    V_INAME(IDirectMusicAudioPath::StampPMsg);
    V_BUFPTR_WRITE(pPMsg,sizeof(DMUS_PMSG));

    HRESULT hr = E_FAIL;
    
    if (!m_fActive)
    {
        // Only kill notes and wave messages, since they are the only PMsgs that make sound.
        if ((pPMsg->dwType == DMUS_PMSGT_NOTE) || (pPMsg->dwType == DMUS_PMSGT_WAVE))
        {
            pPMsg->dwPChannel = DMUS_PCHANNEL_KILL_ME;
            Trace(1,"Error: Attempting to play on an inactive AudioPath, PMsg being ignored.\n");
            return DMUS_E_AUDIOPATH_INACTIVE;
        }
    }
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    if (!m_pPerformance)
    {
        LEAVE_CRITICAL_SECTION(&m_CriticalSection);
        return DMUS_E_NOT_INIT;
    }
	ENTER_API_CRITICAL_SECTION;
    // First, check if the audio path has its own graph.
    if (m_pGraph)
    {
        // Could return DMUS_S_LAST_TOOL, indicating end of graph. 
        // If so, we'll treat that as a failure and drop on through to the next graph...
        if( S_OK == ( hr = m_pGraph->StampPMsg( pPMsg )))
        {
            if( pPMsg->pGraph != this ) // Make sure this is set to point to the segstate embedded graph so it will come here again.
            {
                if( pPMsg->pGraph )
                {
                    pPMsg->pGraph->Release();
                    pPMsg->pGraph = NULL;
                }
                pPMsg->pGraph = this;
                AddRef();
            }
        }
    }
    
    // If done with the graph, send to the performance. Also, check for the special case of 
    // DMUS_PCHANNEL_BROADCAST_AUDIOPATH. If so, duplicate the pMsg
    // and send all the copies with the appropriate pchannel values.
    // Otherwise, convert the vchannel to the matching pchannel (this is the
    // point where the pchannel mapping occurs.)
    if( FAILED(hr) || (hr == DMUS_S_LAST_TOOL))
    {
        if (pPMsg->dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH)
        {
            DWORD dwIndex;
            for (dwIndex = 1;dwIndex < m_dwChannelCount;dwIndex++)
            {
                DWORD dwNewChannel = m_pdwPChannels[dwIndex];
                // Don't broadcast any broadcast messages!
                // And, if this is a transpose on the drum channel, don't send it.
                if ((dwNewChannel < DMUS_PCHANNEL_BROADCAST_GROUPS) &&
                    ((pPMsg->dwType != DMUS_PMSGT_TRANSPOSE) || ((dwNewChannel & 0xF) != 9)))
                {
                    DMUS_PMSG *pNewMsg;
                    if (SUCCEEDED(m_pPerformance->ClonePMsg(pPMsg,&pNewMsg)))
                    {
                        pNewMsg->dwPChannel = dwNewChannel;
                        m_pPerformance->StampPMsg(pNewMsg);
                        m_pPerformance->SendPMsg(pNewMsg);
                    }
                }
            }
            // Now, set the pchannel for this one. First check that there are any
            // pchannels. If none, mark the PMsg to be deleted by the SendPMsg routine.
            // Also, mark it this way if the PMsg is a broadcast PMsg.
            pPMsg->dwPChannel = DMUS_PCHANNEL_KILL_ME;
            if (m_dwChannelCount)
            {
                if (m_pdwPChannels[0] < DMUS_PCHANNEL_BROADCAST_GROUPS)
                {
                    pPMsg->dwPChannel = m_pdwPChannels[0];
                }
            }
        }
        else
        {
            DWORD dwScan = 0;
            for (;dwScan < m_dwChannelCount;dwScan++)
            {
                if (m_pdwVChannels[dwScan] == pPMsg->dwPChannel)
                {
                    pPMsg->dwPChannel = m_pdwPChannels[dwScan];
                    break;
                }
            }
            // If a map was not found, kill the message. 
            // But, ignore for notifications, since they really don't care about pchannel.
            // And, ignore for performance broadcast PMsgs.
            if ((dwScan == m_dwChannelCount) && 
                (pPMsg->dwType != DMUS_PMSGT_NOTIFICATION) &&
                (pPMsg->dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS))
            {
                pPMsg->dwPChannel = DMUS_PCHANNEL_KILL_ME;
            }
        }
        hr = m_pPerformance->StampPMsg(pPMsg);
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
	LEAVE_API_CRITICAL_SECTION;
    return hr;
}

CGraph *CAudioPath::GetGraph()

{
    CGraph *pGraph;
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    // Return the graph, and AddRef if it exists.
    if (pGraph = m_pGraph)
    {
        m_pGraph->AddRef();
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
    return pGraph;
}

void CAudioPath::Deactivate()
{
    m_fDeactivating = TRUE;
    Activate(FALSE);
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    m_fActive = FALSE;
    m_PortConfigList.Clear();
    m_BufferConfigList.Clear();
    if (m_pGraph)
    {
        m_pGraph->Release();
        m_pGraph = NULL;
    }
    if (m_pConfig)
    {
        m_pConfig->Release();
        m_pConfig = NULL;
    }
    if (m_pPerformance)
    {
        if (m_pdwVChannels && m_pdwPChannels)
        {
            DWORD dwIndex;
            for (dwIndex = 0;dwIndex <m_dwChannelCount;dwIndex++)
            {
                m_pPerformance->ReleasePChannel(m_pdwPChannels[dwIndex]);
            }
            delete [] m_pdwVChannels;
            delete [] m_pdwPChannels;
        }
        m_pPerformance->m_AudioPathList.Remove(this);
        m_pPerformance->RemoveUnusedPorts();
        m_pPerformance->Release();
        m_pPerformance = NULL;
    }
    m_fDeactivating = FALSE;
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
}

void CAudioPath::SetGraph(CGraph *pGraph)

{
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    // Is this a change?
    if (!m_pGraph)
    {
        pGraph->Clone((IDirectMusicGraph **) &m_pGraph);
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
}


CBufferNode * CAudioPath::GetBufferNode(REFGUID guidBuffer)

{
    CBufferNode *pBuff = NULL;
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
    pBuff = m_BufferConfigList.GetBufferNode(guidBuffer);
    if (!pBuff)
    {
        CPortConfig *pConfig = m_PortConfigList.GetHead();
        for (;pConfig;pConfig = pConfig->GetNext())
        {
            pBuff = pConfig->GetBufferNode(guidBuffer);
            if (pBuff)
            {
                break;
            }
        }
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
    if (!pBuff)
    {
        pBuff = m_pPerformance->m_BufferManager.GetBufferNode(guidBuffer);
    }
    return pBuff;
}

extern long g_lNewTrackID; // Global track id source.

HRESULT CAudioPath::Init(IUnknown *pSourceConfig,CPerformance *pPerf)

{
    HRESULT hr = E_INVALIDARG;
    ENTER_CRITICAL_SECTION(&m_CriticalSection);
	// Give the audiopath a uninque track id. This will be used to invalidate
	// broadcast PMsgs sent to the audiopath.
	DWORD dwTempID;
	InterlockedIncrement(&g_lNewTrackID);
	m_dwTrackID = g_lNewTrackID;
    m_pPerformance = pPerf;
    pPerf->m_AudioPathList.AddHead(this);
    pPerf->AddRef();
    if (pPerf && pSourceConfig)
    {
        if (SUCCEEDED(hr = pSourceConfig->QueryInterface(IID_CAudioPathConfig,(void **) &m_pConfig)))
        {
            if (m_pConfig->m_pGraph)
            {
                SetGraph(m_pConfig->m_pGraph);
            }
#ifdef DXAPI
            // The very first audio path has to create the sink.
            hr = pPerf->m_BufferManager.InitSink();
#endif          
            if (SUCCEEDED(hr))
            {
                // First, install any global buffers that are required.
                hr = m_pConfig->m_BufferConfigList.CreateRunTimeVersion(&m_BufferConfigList,&pPerf->m_BufferManager);
                if (SUCCEEDED(hr))
                {
                    // Then, install the ports and buffers.
                    hr = m_pConfig->m_PortConfigList.CreateRunTimeVersion(&m_PortConfigList,this,&pPerf->m_BufferManager);
                    if (SUCCEEDED(hr))
                    {
                        hr = ConnectToPorts(pPerf,pPerf->m_AudioParams.dwSampleRate);
                    }
                }
            }
        }
    }
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
    return hr;
}

HRESULT CAudioPath::ConnectToPorts(CPerformance *pPerf,DWORD dwSampleRate)

/*  This must be called from within a critical section.
*/

{
    HRESULT hr = S_OK;
    // Scan through the list of portconfigs and hook them up with active ports
    // in the performance. If a port is not available, create the port.
    CPortConfig *pConfig = m_PortConfigList.GetHead();
    DWORD dwChannelCount = 0;   // Used to add up total PChannels needed.
    for (;pConfig && SUCCEEDED(hr);pConfig = pConfig->GetNext())
    {
        // Given the configuration, either find a port with a matching id, or create one.
#ifdef DXAPI
        hr = pPerf->GetPathPort(pConfig);
#endif
        dwChannelCount += pConfig->m_PortHeader.dwPChannelCount;
    }
    if (SUCCEEDED(hr))
    {
        // Now, allocate the VChannels needed for each portconfig.
        m_pdwVChannels = new DWORD[dwChannelCount];
        if (m_pdwVChannels)
        {
            m_pdwPChannels = new DWORD[dwChannelCount];
            if (!m_pdwPChannels)
            {
                delete [] m_pdwVChannels;
                m_pdwVChannels = NULL;
                hr = E_OUTOFMEMORY;
            }
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }
    if (SUCCEEDED(hr))
    {
        // Scan through the port configs and allocate the pchannels, copying the assignments
        // into virtual channel assignment arrays.
        pConfig = m_PortConfigList.GetHead();
        DWORD dwIndex = 0;
        for (;pConfig;pConfig = pConfig->GetNext())
        {
            // If this port uses buffers, then connect them up. 
            if (((pConfig->m_PortParams.dwValidParams & DMUS_PORTPARAMS_FEATURES) && 
                (pConfig->m_PortParams.dwFeatures & DMUS_PORT_FEATURE_AUDIOPATH)))
            {
                CBufferConnect *pConnect = pConfig->m_BufferConnectList.GetHead();
                for (;pConnect && SUCCEEDED(hr);pConnect = pConnect->GetNext())
                {
                    // For each connect block, there should be an array of buffers 
                    // to connect the range of PChannels to.
                    // For each PChannel, get a virtual pchannel and then assign
                    // it to the bus ids that belong to the buffers.
                    if (pConnect->m_ppBufferNodes)
                    {
#ifdef DXAPI
                        DWORD dwCount = 0;
                        DWORD dwBusIDs[32];
                        DWORD *pdwBusIDBase = &dwBusIDs[0];
                        DWORD dwTotalRead = 0;
                        DWORD dwAmountLeft = 32;
                        for (;(dwCount < pConnect->m_ConnectHeader.dwBufferCount) && dwAmountLeft; dwCount++)
                        {
                            if (pConnect->m_ppBufferNodes[dwCount] && !(pConnect->m_ppBufferNodes[dwCount]->m_BufferHeader.dwFlags & DMUS_BUFFERF_PRIMARY))
                            {
                                IDirectSoundBuffer *pBuffer = pConnect->m_ppBufferNodes[dwCount]->GetBuffer();
                                if (pBuffer)
                                {
                                    dwTotalRead = dwAmountLeft;
                                    hr = m_pPerformance->m_BufferManager.m_pSinkConnect->GetSoundBufferBusIDs((CBuffer *)pBuffer,pdwBusIDBase,NULL,&dwTotalRead);
                                    pBuffer->Release();
                                    if (FAILED(hr)) break;
                                    pdwBusIDBase += dwTotalRead; // Increment pointer by how many was read.
                                    dwAmountLeft -=  dwTotalRead;
                                }
                            }
                        }
                        if (SUCCEEDED(hr))
                        {
                            dwTotalRead = 32 - dwAmountLeft;
                            // Now, allocate the pchannels and assign them to buses.
                            IDirectMusicPortP* pPortP = NULL;
                            if (SUCCEEDED(pConfig->m_pPort->QueryInterface(IID_IDirectMusicPortP, (void**)&pPortP)))
                            {
                                for (dwCount = 0;dwCount < pConnect->m_ConnectHeader.dwPChannelCount; dwCount++)
                                {
                                    DWORD dwDrumFlags = 0;
                                    m_pdwVChannels[dwIndex] = pConnect->m_ConnectHeader.dwPChannelBase + dwCount;
                                    if (pConfig->m_PortHeader.dwFlags & DMUS_PORTCONFIGF_DRUMSON10) 
                                    {
                                        dwDrumFlags = 1;
                                        if (((pConnect->m_ConnectHeader.dwPChannelBase + dwCount) & 0xF) == 9)
                                        {
                                            // This is a drum on channel 10.
                                            dwDrumFlags |= 2;
                                        }
                                    } 
                                    // Now, allocate a virtual pchannel for this and get back the equivalent group and midi channel.
                                    DWORD dwGroup;
                                    DWORD dwMChannel;
                                    hr = pPerf->AllocVChannel(pConfig->m_dwPortID,dwDrumFlags,&m_pdwPChannels[dwIndex],&dwGroup,&dwMChannel);
                                    if (dwTotalRead && SUCCEEDED(hr))
                                    {
                                        hr = pPortP->AssignChannelToBuses(dwGroup,dwMChannel,dwBusIDs,dwTotalRead);
                                    }
                                    dwIndex++;
                                }
                                pPortP->Release();
                            }
                        }
#else
                        // For XBOX, the problem is much simpler. We can either connect a channel to a 3D buffer,
                        // in which case we connect it directly to it, or we can connect it to some number of mixbins,
                        // which are all defined by bit flags.
                        DWORD dwMixBins = 0;
                        DWORD dwCount = 0;
                        BYTE *pControllers = NULL;
                        IDirectSoundBuffer *pBuffer = NULL;     
                        for (;(dwCount < pConnect->m_ConnectHeader.dwBufferCount) ; dwCount++)
                        {
                            if (pConnect->m_ppBufferNodes[dwCount])
                            {
                                pBuffer = pConnect->m_ppBufferNodes[dwCount]->GetBuffer();
                                if (pBuffer)
                                {
                                    // Since we have the buffer, forget about the mixbins and fall through.
                                    break;
                                }
                                if (pConnect->m_ppBufferNodes[dwCount]->m_dwMixBin) 
                                {
                                    dwMixBins |= pConnect->m_ppBufferNodes[dwCount]->m_dwMixBin;
                                }
                                if (pConnect->m_ppBufferNodes[dwCount]->m_MixBinsHeader.bControllers[0])
                                {
                                    pControllers = &pConnect->m_ppBufferNodes[dwCount]->m_MixBinsHeader.bControllers[0];
                                    break;
                                }
                            }
                        }
#ifdef DXAPI
                        IDirectMusicPortP* pPortP = NULL;
                        if (SUCCEEDED(pConfig->m_pPort->QueryInterface(IID_IDirectMusicPortP, (void**)&pPortP)))
#else
                        IDirectMusicSynthX* pPortP = pPerf->m_pSynth;
                        pPortP->AddRef();
#endif
                        {
                            for (dwCount = 0;dwCount < pConnect->m_ConnectHeader.dwPChannelCount; dwCount++)
                            {
                                DWORD dwDrumFlags = 0;
                                m_pdwVChannels[dwIndex] = pConnect->m_ConnectHeader.dwPChannelBase + dwCount;
                                if (pConfig->m_PortHeader.dwFlags & DMUS_PORTCONFIGF_DRUMSON10) 
                                {
                                    dwDrumFlags = 1;
                                    if (((pConnect->m_ConnectHeader.dwPChannelBase + dwCount) & 0xF) == 9)
                                    {
                                        // This is a drum on channel 10.
                                        dwDrumFlags |= 2;
                                    }
                                } 
                                // Now, allocate a virtual pchannel for this and get back the equivalent group and midi channel.
                                DWORD dwGroup;
                                DWORD dwMChannel;
                                hr = pPerf->AllocVChannel(pConfig->m_dwPortID,dwDrumFlags,&m_pdwPChannels[dwIndex],&dwGroup,&dwMChannel);
                                if (SUCCEEDED(hr))
                                {
                                    hr = pPortP->AssignChannelToOutput(dwGroup,dwMChannel,pBuffer,dwMixBins,pControllers);
                                }
                                dwIndex++;
                            }
                            pPortP->Release();
                        }
                        if (pBuffer)
                        {
                            pBuffer->Release();
                        }
#endif
                    }
                }
            }
            else
            {
                DWORD dwCount;
                for (dwCount = 0;SUCCEEDED(hr) && (dwCount < pConfig->m_PortHeader.dwPChannelCount); dwCount++)
                {
                    DWORD dwDrumFlags = 0;
                    m_pdwVChannels[dwIndex] = pConfig->m_PortHeader.dwPChannelBase + dwCount;
                    if (pConfig->m_PortHeader.dwFlags & DMUS_PORTCONFIGF_DRUMSON10) 
                    {
                        dwDrumFlags = 1;
                        if (((pConfig->m_PortHeader.dwPChannelBase + dwCount) & 0xF) == 9)
                        {
                            // This is a drum on channel 10.
                            dwDrumFlags |= 2;
                        }
                    } 
                    // Now, allocate a virtual pchannel for this.
                    DWORD dwGroup; // These won't be used since we won't be assigning pchannels on the port to buffers.
                    DWORD dwMChannel;
                    hr = pPerf->AllocVChannel(pConfig->m_dwPortID,dwDrumFlags,&m_pdwPChannels[dwIndex],&dwGroup,&dwMChannel);
/*                    Trace(0,"%ld: Mapping %ld to %ld (Port %ld, Group %ld, Channel %ld)\n",
                        dwIndex,m_pdwVChannels[dwIndex],m_pdwPChannels[dwIndex],
                        pConfig->m_dwPortID,dwGroup,dwMChannel);*/

                    dwIndex++;
                }
            }
        }
    }
    m_dwChannelCount = dwChannelCount;
    return hr;
}

void CAudioPathList::Clear()
{
    CAudioPath *pPath;
    while (pPath = GetHead())
    {
        pPath->Deactivate(); // This should also remove it from the list.
        assert(pPath != GetHead()); // Make sure this is always the case!
    }
}

CBufferNode * CAudioPathList::GetBufferNode(REFGUID guidBufferID)

{
    CBufferNode *pBuff = NULL;
    CAudioPath *pPath;
    for (pPath = GetHead();pPath;pPath = pPath->GetNext())
    {
        pBuff = pPath->GetBufferNode(guidBufferID);
        if (pBuff)
        {
            break;
        }
    }
    return pBuff;
}

BOOL CAudioPathList::UsesPort(IDirectMusicPort *pPort)

{
    CAudioPath *pPath = GetHead();
    for (;pPath;pPath = pPath->GetNext())
    {
        if (pPath->UsesPort(pPort))
        {
            return TRUE;
        }
    }
    return FALSE;
}

CAudioPathConfig::CAudioPathConfig() : CMemTrack(DMTRACK_AUDIOPATH_CONFIG)  
{
    m_pGraph = NULL;
    m_cRef = 1;
    m_pUnkDispatch = NULL;

    INITIALIZE_CRITICAL_SECTION(&m_CriticalSection);
    IncrementDLLCount();
}

CAudioPathConfig::~CAudioPathConfig()
{
    if (m_pGraph)
    {
        m_pGraph->Release();
    }
    if (m_pUnkDispatch)
    {
        m_pUnkDispatch->Release(); // free IDispatch implementation we may have borrowed
    }
    m_PortConfigList.Clear();
    m_BufferConfigList.Clear();
    DELETE_CRITICAL_SECTION(&m_CriticalSection);
    DecrementDLLCount();
}

CAudioPathConfig *CAudioPathConfig::CreateStandardConfig(DWORD dwType,DWORD dwPChannelCount,DWORD dwSampleRate)

{
    CAudioPathConfig *pConfig = new CAudioPathConfig;
    if (pConfig)
    {
        DWORD dwGlobalType = 0;         // Global mixin buffer.
        DWORD dwTypes[3];               // What types of buffers to create.
        DWORD dwTotal = 0;              // How many buffers.
        GUID  guidBufferIDs[3];         // IDs of buffers that should be connected to.
        DWORD dwConnections = 0;        // How many buffer connections.
        BOOL fCreatePort = TRUE;
#ifdef XBOX
        DMUS_IO_BUFFER_MIXBINS_HEADER MixBinsHeader;
        MixBinsHeader.bControllers[0] = 0;
        MixBinsHeader.dwMixBins = 0;
#endif
        switch (dwType)
        {
        case DMUS_APATH_SHARED_STEREOPLUSREVERB:
            dwTypes[0] = BUFFER_MUSIC; 
            dwTypes[1] = BUFFER_REVERB;
            guidBufferIDs[0] = GUID_Buffer_Stereo;
            guidBufferIDs[1] = GUID_Buffer_Reverb;
#ifdef DXAPI
            dwConnections = 2;
            dwTotal = 2;
#else
            dwTypes[2] = BUFFER_CHORUS;
            dwConnections = 3;
            dwTotal = 3;
            guidBufferIDs[2] = GUID_Buffer_Chorus;
#endif
            break;
        // Following removed for DX8, should be reintroduced for Whistler and DX8.1...
/*
        case DMUS_APATH_DYNAMIC_ENV3D:
            dwGlobalType = BUFFER_ENVREVERB;
            dwTypes[0] = BUFFER_3D;
            guidBufferIDs[0] = GUID_Buffer_3D;
            dwConnections = 1;
            dwTotal = 1;
            break;
*/
        case DMUS_APATH_DYNAMIC_3D:
            dwTypes[0] = BUFFER_3D_DRY;
            guidBufferIDs[0] = GUID_Buffer_3D_Dry;
            dwConnections = 1;
            dwTotal = 1;
            break;
        case DMUS_APATH_DYNAMIC_MONO:
            dwTypes[0] = BUFFER_MONO;
            guidBufferIDs[0] = GUID_Buffer_Mono;
            dwConnections = 1;
            dwTotal = 1;
            break;
        case DMUS_APATH_DYNAMIC_STEREO:
            dwTypes[0] = BUFFER_STEREO;
            guidBufferIDs[0] = GUID_Buffer_Stereo;
            dwConnections = 1;
            dwTotal = 1;
            break;
#ifndef DXAPI
        case DMUS_APATH_SHARED_STEREO:
            dwTypes[0] = BUFFER_MUSIC; 
            guidBufferIDs[0] = GUID_Buffer_Stereo;
            dwConnections = 1;
            dwTotal = 1;
            break;
#endif
#ifdef XBOX
        default:
            // Treat all default cases as mixbin cases.
            {
                dwTypes[0] = BUFFER_MIXBINS;
                guidBufferIDs[0] = GUID_Buffer_MixBins;
                dwConnections = 1;
                dwTotal = 1;
                DWORD dwMixCount = 0;
                DWORD dwMixBins = dwType;
                // First, clear the controllers.
                for (dwMixCount = 0; dwMixCount < 8; dwMixCount++)
                {
                    MixBinsHeader.bControllers[dwMixCount] = 0;
                }
                dwMixCount = 0;
                // For each bit set, assign a controller.
                for (; (dwMixBins != 0) && (dwMixCount < 8); dwMixBins >>= 1)
                {
                    if (dwMixBins & 1) 
                    {
                        MixBinsHeader.bControllers[dwMixCount] = (BYTE) dwMixCount+1;
                        dwMixCount++;
                    }
                }
                if (dwMixBins)
                {
                    // Force a failure. Too many mixbins requested.
                    Trace(0,"Error: Attempt to create mixbin audiopath with more than 8 mixbins: %lx\n",dwType);
                    fCreatePort = false;
                }
                MixBinsHeader.dwMixBins = dwType;
            }
            break;
#endif
        }
        if (dwGlobalType)
        {
            CBufferConfig *pBuffer = new CBufferConfig(dwGlobalType);
            if (pBuffer)
            {
                // This buffer configuration just has an id to identify which standard
                // buffer, instead of a pointer to a DSoundBufferConfig object,
                // which is what you'd see in the file io case.
                pConfig->m_BufferConfigList.AddHead(pBuffer);
            }
            else
            {
                delete pConfig;
                return NULL;
            }
        }
        if (fCreatePort)
        {
            CPortConfig *pPort = new CPortConfig();
            if (pPort)
            {
                pConfig->m_PortConfigList.AddHead(pPort);
                for (DWORD dwIndex = 0; dwIndex < dwTotal; dwIndex++)
                {
                    CBufferConfig *pBuffer = new CBufferConfig(dwTypes[dwIndex]);
                    if (pBuffer)
                    {
                        // This buffer configuration just has an id to identify which standard
                        // buffer, instead of a pointer to a DSoundBufferConfig object,
                        // which is what you'd see in the file io case.
                        pPort->m_BufferConfigList.AddHead(pBuffer);
#ifdef XBOX
                        pBuffer->m_MixBinsHeader = MixBinsHeader;
#endif
                    }
                    else
                    {
                        delete pConfig;
                        return NULL;
                    }
                }
                // If there are connections to buffers, create the connection structure.
                if (dwConnections)
                {
                    CBufferConnect *pConnect = new CBufferConnect;
                    if (pConnect)
                    {
                        pPort->m_BufferConnectList.AddHead(pConnect);
                        pConnect->m_ConnectHeader.dwBufferCount = dwConnections;
                        pConnect->m_ConnectHeader.dwFlags = 0;
                        pConnect->m_ConnectHeader.dwPChannelBase = 0;
                        pConnect->m_ConnectHeader.dwPChannelCount = dwPChannelCount;
                        pConnect->m_pguidBufferIDs = new GUID[dwConnections];
                        if (pConnect->m_pguidBufferIDs)
                        {
                            for (DWORD dwIndex = 0; dwIndex < dwConnections; dwIndex++)
                            {
                                pConnect->m_pguidBufferIDs[dwIndex] = guidBufferIDs[dwIndex];
                            }
                        }
                        else
                        {
                            delete pConfig;
                            return NULL;
                        }
                    }
                }
                pPort->m_PortHeader.dwPChannelCount = dwPChannelCount;
                pPort->m_PortParams.dwChannelGroups = (dwPChannelCount + 15) / 16;
            }
            else
            {
                delete pConfig;
                pConfig = NULL;
            }
        }
    }
    return pConfig;
}



STDMETHODIMP CAudioPathConfig::QueryInterface(
    const IID &iid,   
    void **ppv)       
{
    V_INAME(CAudioPathConfig::QueryInterface);
    V_PTRPTR_WRITE(ppv);
    V_REFGUID(iid);

    *ppv = NULL;
    if ((iid == IID_IUnknown ) || (iid == IID_IDirectMusicObject))
    {
        *ppv = static_cast<IDirectMusicObject*>(this);
    }
    else if (iid == IID_IPersistStream)
    {
        *ppv = static_cast<IPersistStream*>(this);
    }
    else if (iid == IID_CAudioPathConfig)
    {
        *ppv = static_cast<CAudioPathConfig*>(this);
    }
    else if (iid == IID_IDispatch)
    {
        // A helper scripting object implements IDispatch, which we expose via COM aggregation.
        if (!m_pUnkDispatch)
        {
            // Create the helper object
            DirectMusicCreateInstance(
                CLSID_AutDirectMusicAudioPathConfig,
                static_cast<IDirectMusicObject*>(this),
                IID_IUnknown,
                reinterpret_cast<void**>(&m_pUnkDispatch));
        }
        if (m_pUnkDispatch)
        {
            return m_pUnkDispatch->QueryInterface(IID_IDispatch, ppv);
        }
    }

    if (*ppv == NULL)
    {
        Trace(4,"Warning: Request to query unknown interface on AudioPathConfig object\n");
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}


STDMETHODIMP_(ULONG) CAudioPathConfig::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CAudioPathConfig::Release()
{
    if (!InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

STDMETHODIMP CAudioPathConfig::GetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
    // Argument validation
    V_INAME(CAudioPathConfig::GetDescriptor);
    V_STRUCTPTR_WRITE(pDesc, DMUS_OBJECTDESC);
    return m_Info.GetDescriptor(pDesc,CLSID_DirectMusicAudioPathConfig);
}

STDMETHODIMP CAudioPathConfig::SetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
    // Argument validation
    V_INAME(CAudioPathConfig::SetDescriptor);
    V_STRUCTPTR_READ(pDesc, DMUS_OBJECTDESC);
	return m_Info.SetDescriptor(pDesc);
}

STDMETHODIMP CAudioPathConfig::ParseDescriptor(LPSTREAM pIStream, LPDMUS_OBJECTDESC pDesc) 

{
    V_INAME(CAudioPathConfig::ParseDescriptor);
    V_INTERFACE(pIStream);
    V_STRUCTPTR_WRITE(pDesc, DMUS_OBJECTDESC);

    return m_Info.ParseDescriptor(pIStream,pDesc,DMUS_FOURCC_AUDIOPATH_FORM,CLSID_DirectMusicAudioPathConfig);
}

/////////////////////////////////////////////////////////////////////////////
// IPersist

HRESULT CAudioPathConfig::GetClassID( CLSID* pClassID )
{
    V_INAME(CAudioPathConfig::GetClassID);
    V_PTR_WRITE(pClassID, CLSID); 
    *pClassID = CLSID_DirectMusicAudioPathConfig;
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IPersistStream functions

HRESULT CAudioPathConfig::IsDirty()
{
    return S_FALSE;
}

HRESULT CAudioPathConfig::Load( IStream* pIStream )
{
    V_INAME(IPersistStream::Load);
    V_INTERFACE(pIStream);

    CRiffParser Parser(pIStream);
    RIFFIO ckMain;
    HRESULT hr = S_OK;

    Parser.EnterList(&ckMain);  
    if (Parser.NextChunk(&hr) && (ckMain.fccType == DMUS_FOURCC_AUDIOPATH_FORM))
    {
        ENTER_CRITICAL_SECTION(&m_CriticalSection);
        // Clear out any data that was previously loaded.
        if (m_pGraph)
        {
            m_pGraph->Release();
        }
        m_PortConfigList.Clear();
        m_BufferConfigList.Clear();        
        hr = Load(&Parser);
        LEAVE_CRITICAL_SECTION(&m_CriticalSection);
    }
    else
    {
        Trace(1,"Error: Failed parsing - file is not AudioPathConfig format.\n");
        hr = DMUS_E_DESCEND_CHUNK_FAIL;
    }
    return hr;
}

HRESULT CAudioPathConfig::Load(CRiffParser *pParser)

{
    RIFFIO ckNext, ckChild;
    HRESULT hr = S_OK;

    ENTER_CRITICAL_SECTION(&m_CriticalSection);

    pParser->EnterList(&ckNext);
    while(pParser->NextChunk(&hr))
    {
        switch(ckNext.ckid)
        {
        case DMUS_FOURCC_GUID_CHUNK:
        case DMUS_FOURCC_VERSION_CHUNK:
        case DMUS_FOURCC_CATEGORY_CHUNK:
        case DMUS_FOURCC_DATE_CHUNK:
            hr = m_Info.ReadChunk(pParser,ckNext.ckid);
			break;
		case FOURCC_LIST:
			switch(ckNext.fccType)
			{
                case DMUS_FOURCC_UNFO_LIST:
                    hr = m_Info.ReadChunk(pParser,ckNext.fccType);
					break;
                case DMUS_FOURCC_PORTCONFIGS_LIST:
                    pParser->EnterList(&ckChild);
                    while (pParser->NextChunk(&hr))
                    {
                        switch( ckChild.ckid )
                        {
                        case FOURCC_LIST:
                            if (ckChild.fccType == DMUS_FOURCC_PORTCONFIG_LIST)
                            {
                                CPortConfig *pConfig = new CPortConfig();
                                if (pConfig) 
                                {
                                    hr = pConfig->Load(pParser);
                                    if (SUCCEEDED(hr))
                                    {
                                        m_PortConfigList.AddTail(pConfig);
                                    }
                                }
                                else
                                {
                                    hr = E_OUTOFMEMORY;
                                }
                            }
                            break;
                        }
                    }
                    pParser->LeaveList();
                    break;
                case DMUS_FOURCC_DSBUFFER_LIST:
                    {
                        CBufferConfig *pSource = new CBufferConfig(0);
                        if (pSource)
                        {
                            pParser->EnterList(&ckChild);
                            while (pParser->NextChunk(&hr))
                            {
                                switch( ckChild.ckid )
                                {
                                case DMUS_FOURCC_DSBUFFATTR_ITEM:
                                    hr = pParser->Read(&pSource->m_BufferHeader, 
                                        sizeof(DMUS_IO_BUFFER_ATTRIBUTES_HEADER));
                                    pSource->DecideType();
                                    break;
                                case DMUS_FOURCC_MIXBINS_ITEM:
                                    hr = pParser->Read(&pSource->m_MixBinsHeader, 
                                        sizeof(DMUS_IO_BUFFER_MIXBINS_HEADER));
                                    break;
                                case FOURCC_LIST:
                                case FOURCC_RIFF:
                                    if ( ckChild.fccType == DMUS_FOURCC_DSBC_FORM)
                                    {
                                        pParser->SeekBack();
                                        hr = pSource->Load(pParser->GetStream());
                                        pParser->SeekForward();
                                    }
                                }
                            }
                            if (SUCCEEDED(hr))
                            {
                                m_BufferConfigList.AddTail(pSource);
                            }
                            else
                            {
                                delete pSource;
                                Trace(1,"Error: AudioPath Configuration failed loading buffer\n");
                            }
                            pParser->LeaveList();
                        }
                        else
                        {
                            hr = E_OUTOFMEMORY;
                        }
                    }
                    break;
                case DMUS_FOURCC_DSBC_FORM:
                    {
                        CBufferConfig *pSource = new CBufferConfig(0);
                        if (pSource)
                        {
                            pParser->SeekBack();
                            hr = pSource->Load(pParser->GetStream());
                            pParser->SeekForward();
                            if (SUCCEEDED(hr))
                            {
                                m_BufferConfigList.AddTail(pSource);
                            }
                            else
                            {
                                Trace(1,"Error: AudioPath Configuration failed loading buffer\n");
                                delete pSource;
                            }
                        }
                        else
                        {
                            hr = E_OUTOFMEMORY;
                        }
                    }
                    break;               
                case DMUS_FOURCC_TOOLGRAPH_FORM:
                    {
                        CGraph *pGraph = new CGraph;
                        if (pGraph)
                        {
                            hr = pGraph->Load(pParser); 
                            if(m_pGraph)
                            {
                                m_pGraph->Release();
                            }
                            m_pGraph = pGraph;
                        }
                        else
                        {
                            hr = E_OUTOFMEMORY;
                        }
                    }
                    break;                       
            }
            break;
        }
    }
    pParser->LeaveList();
    LEAVE_CRITICAL_SECTION(&m_CriticalSection);
    return hr;
}

HRESULT CAudioPathConfig::Save( IStream* pIStream, BOOL fClearDirty )
{
    return E_NOTIMPL;
}

HRESULT CAudioPathConfig::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
    return E_NOTIMPL;
}
