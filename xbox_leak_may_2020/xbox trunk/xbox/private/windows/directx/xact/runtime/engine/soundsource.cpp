/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       soundsource.cpp
 *  Content:    XACT runtime sound source object implementation
 *  History:
 *  Date        By        Reason
 *  ====        ==        ======
 *  1/22/2002   georgioc  Created.
 *
 ****************************************************************************/

#include "xacti.h"
#include "xboxdbg.h"

using namespace XACT;

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::CSoundSource"


CSoundSource::CSoundSource
(
    void
)
{
    DPF_ENTER();

    ASSERT(g_pEngine);
    g_pEngine->AddRef();

    InitializeListHead(&m_ListEntry);

    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::~CSoundSource"

CSoundSource::~CSoundSource
(
    void
)
{
    DPF_ENTER();

    ASSERT(g_pEngine);
    g_pEngine->Release();

    if (m_HwVoice.pStream) {

        m_HwVoice.pStream->Release();

    }

    if (m_HwVoice.pBuffer) {

        m_HwVoice.pBuffer->Release();

    }

    ASSERT(m_pWaveBankOwner == NULL);

    DPF_LEAVE_VOID();
}

__inline ULONG CSoundSource::AddRef(void)
{
    _ENTER_EXTERNAL_METHOD("CSoundSource::AddRef");
    return CRefCount::AddRef();
}


ULONG CSoundSource::Release(void)
{
    _ENTER_EXTERNAL_METHOD("CSoundSource::Release");
    DWORD dwRefCount = CRefCount::Release();

    switch(dwRefCount) {
    case 1:

        //
        // return this voice to the engine available buffer list
        //

        ASSERT(g_pEngine);
        ASSERT(m_pWaveBankOwner == NULL);

        g_pEngine->FreeSoundSource(this);
        break;


    case 2:

        //
        // stop the voice. this will not cause any blocking any hw
        //
        
        if (!IsPositional())
        {
            Stop();
        }

        //
        // if this source has a wavebank owner, return this voice
        // to the wavebank available list,since its associated with specific
        // hw scatter gather entries
        //

        if (m_pWaveBankOwner) {
            m_pWaveBankOwner->FreeSoundSource(this);
        }            
        
        break;
    }

    return dwRefCount;

}


#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::Initialize"

HRESULT CSoundSource::Initialize()
{
    HRESULT hr = S_OK;

    DPF_ENTER();

    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::Stop"

HRESULT CSoundSource::Stop()
{

    HRESULT hr = S_OK;
    //
    // this is an internal method but we want to sync with external api calls
    //

    ENTER_EXTERNAL_METHOD();

    DPF_ENTER();

    if (m_HwVoice.pBuffer) {
        hr = m_HwVoice.pBuffer->Stop();
    } else {

        hr = m_HwVoice.pStream->Flush();
        if (SUCCEEDED(hr)) {
            hr = m_HwVoice.pStream->Pause(DSSTREAMPAUSE_PAUSE);
        }

    }
    
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::Play"

HRESULT CSoundSource::Play()
{    
    HRESULT hr = S_OK;

    //
    // this is an internal method but we want to sync with external api calls
    //

    ENTER_EXTERNAL_METHOD();
    DPF_ENTER();

    if (m_HwVoice.pBuffer) {
        hr = m_HwVoice.pBuffer->Play(0,0,0);
    } else {

        //
        // TODO Implement Streaming Play
        //

    }

    return hr;
    
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::SetPosition"

HRESULT CSoundSource::SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    if (m_HwVoice.pBuffer)
        hr = m_HwVoice.pBuffer->SetPosition(x,y,z,dwApply);
    else 
        hr = m_HwVoice.pStream->SetPosition(x,y,z,dwApply);

    DPF_LEAVE_HRESULT(hr);
    return hr;
 
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::SetAllParameters"

HRESULT CSoundSource::SetAllParameters(LPCDS3DBUFFER pcDs3dBuffer, DWORD dwApply)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    if (m_HwVoice.pBuffer)
        hr = m_HwVoice.pBuffer->SetAllParameters(pcDs3dBuffer,dwApply);
    else 
        hr = m_HwVoice.pStream->SetAllParameters(pcDs3dBuffer,dwApply);

    DPF_LEAVE_HRESULT(hr);
    return hr;
 
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::SetConeOrientation"

HRESULT CSoundSource::SetConeOrientation(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    if (m_HwVoice.pBuffer)
        hr = m_HwVoice.pBuffer->SetConeOrientation(x, y, z, dwApply);
    else 
        hr = m_HwVoice.pStream->SetConeOrientation(x, y, z, dwApply);

    DPF_LEAVE_HRESULT(hr);
    return hr;
 
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::SetI3DL2Source"

HRESULT CSoundSource::SetI3DL2Source(LPCDSI3DL2BUFFER pds3db, DWORD dwApply)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    if (m_HwVoice.pBuffer)
        hr = m_HwVoice.pBuffer->SetI3DL2Source(pds3db, dwApply);
    else 
        hr = m_HwVoice.pStream->SetI3DL2Source(pds3db, dwApply);

    DPF_LEAVE_HRESULT(hr);
    return hr;
 
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::SetVelocity"

HRESULT CSoundSource::SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    if (m_HwVoice.pBuffer)
        hr = m_HwVoice.pBuffer->SetVelocity(x, y, z, dwApply);
    else 
        hr = m_HwVoice.pStream->SetVelocity(x, y, z, dwApply);

    DPF_LEAVE_HRESULT(hr);
    return hr;
 
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::SetMixBins"

HRESULT CSoundSource::SetMixBins(LPCDSMIXBINS pMixBins)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    if (m_HwVoice.pBuffer)
        hr = m_HwVoice.pBuffer->SetMixBins(pMixBins);
    else 
        hr = m_HwVoice.pStream->SetMixBins(pMixBins);

    DPF_LEAVE_HRESULT(hr);
    return hr;
 
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundSource::SetMixBinVolumes"

HRESULT CSoundSource::SetMixBinVolumes(LPCDSMIXBINS pMixBins)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    if (m_HwVoice.pBuffer)
        hr = m_HwVoice.pBuffer->SetMixBinVolumes(pMixBins);
    else 
        hr = m_HwVoice.pStream->SetMixBinVolumes(pMixBins);

    DPF_LEAVE_HRESULT(hr);
    return hr;
 
}
