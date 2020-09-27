/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsapi.cpp
 *  Content:    DirectSound API objects and entry points.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/06/00    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"

#pragma comment(linker, "/merge:DSOUND_RW=DSOUND")
#pragma comment(linker, "/merge:DSOUND_URW=DSOUND")
#pragma comment(linker, "/merge:DSOUND_RD=DSOUND")
#pragma comment(linker, "/section:DSOUND,ERW")

DWORD g_dwDirectSoundTestOverrideSpeakerConfig = -1;
BOOL g_fDirectSoundTestInFinalRelease = FALSE;

INITIALIZED_CRITICAL_SECTION(g_DirectSoundTestCriticalSection);

#ifdef ASSUME_VALID_PARAMETERS

#define NormalizeFloat(a) (a)
#define NormalizeDouble(a) (a)

#endif // ASSUME_VALID_PARAMETERS


/****************************************************************************
 *
 *  DirectSoundCreate
 *
 *  Description:
 *      Creates and initializes a DirectSound object.
 *
 *  Arguments:
 *      CDirectSoundTest ** [out]: DirectSound object.  The caller is responbile
 *                             for freeing this object with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundCreateInternal"

HRESULT
DirectSoundCreateInternal
(
    CDirectSoundTest **         ppDirectSound
)
{
    CDirectSoundTest *          pDirectSound    = NULL;
    HRESULT                 hr              = DS_OK;
    
    DPF_ENTER();

    //
    // Check to see if the DirectSound object already exists
    //

    if(CDirectSoundTest::m_pDirectSound)
    {
        *ppDirectSound = ADDREF(CDirectSoundTest::m_pDirectSound);
    }
    else
    {
        hr = HRFROMP(pDirectSound = NEW(CDirectSoundTest));

        if(SUCCEEDED(hr))
        {
            hr = pDirectSound->Initialize();
        }

        if(SUCCEEDED(hr))
        {
            *ppDirectSound = pDirectSound;
        }
        else
        {
            RELEASE(pDirectSound);
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  DirectSoundCreate
 *
 *  Description:
 *      Creates and initializes a DirectSound object.
 *
 *  Arguments:
 *      LPGUID [in]: unused.  Must be NULL.
 *      LPDIRECTSOUND * [out]: DirectSound object.  The caller is responbile
 *                             for freeing this object with Release.
 *      LPUNKNOWN [in]: unused.  Must be NULL.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundCreateTest"

STDAPI
DirectSoundCreateTest
(
    LPGUID                  pguidDeviceId,
    LPDIRECTSOUND *         ppDirectSound,
    LPUNKNOWN               pControllingUnknown
)
{
    CDirectSoundTest *          pDirectSound;
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

#ifdef VALIDATE_PARAMETERS

    if(!IS_NULL_GUID(*pguidDeviceId))
    {
        DPF_ERROR("NULL or GUID_NULL is the only supported device identifier");
    }

    if(!ppDirectSound)
    {
        DPF_ERROR("Failed to supply an LPDIRECTSOUND *");
    }
    
    if(pControllingUnknown)
    {
        DPF_ERROR("Aggregation is not supported");
    }

#endif // VALIDATE_PARAMETERS

    hr = DirectSoundCreateInternal(&pDirectSound);

    if(SUCCEEDED(hr))
    {
        *ppDirectSound = pDirectSound;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  CDirectSoundTestSettings
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundTestSettings::CDirectSoundTestSettings"

CDirectSoundTestSettings::CDirectSoundTestSettings
(
    void
)
{
    DWORD                   i;

    DPF_ENTER();

#ifndef MCPX_BOOT_LIB

    m_3dParams.HrtfParams = DirectSoundDefault3DListener;
    m_3dParams.I3dl2Params = DirectSoundI3DL2ListenerPreset_Default;

#endif // MCPX_BOOT_LIB

    //
    // Read the speaker configuration from the EEPROM
    //

#ifndef MCPX_BOOT_LIB

    if(DSSPEAKER_IS_VALID(g_dwDirectSoundTestOverrideSpeakerConfig))
    {
        m_dwSpeakerConfig = g_dwDirectSoundTestOverrideSpeakerConfig;
    }
    else
    {
        m_dwSpeakerConfig = XAudioGetSpeakerConfig();
    }

#else // MCPX_BOOT_LIB

    m_dwSpeakerConfig = DSSPEAKER_STEREO;

#endif // MCPX_BOOT_LIB

    //
    // Set default mixbin headroom
    //

    for(i = 0; i < DSMIXBIN_SUBMIX; i++)
    {
        m_abMixBinHeadroom[i] = DSHEADROOM_DEFAULT;
    }

    m_abMixBinHeadroom[DSMIXBIN_SUBMIX] = 0;

    //
    // Set default effect locations
    //

    SetEffectImageLocations(NULL);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CDirectSoundTestSettings
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundTestSettings::~CDirectSoundTestSettings"

CDirectSoundTestSettings::~CDirectSoundTestSettings
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetEffectImageLocations
 *
 *  Description:
 *      Sets effect indeces.
 *
 *  Arguments:
 *      LPCDSEFFECTIMAGELOC [in]: effect locations.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundTestSettings::SetEffectImageLocations"

void
CDirectSoundTestSettings::SetEffectImageLocations
(
    LPCDSEFFECTIMAGELOC     pImageLoc
)
{
    DPF_ENTER();

    if(pImageLoc)
    {
        CopyMemory(&m_EffectLocations, pImageLoc, sizeof(*pImageLoc));
    }
    else
    {
        memset(&m_EffectLocations, 0xFF, sizeof(m_EffectLocations));
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CDirectSoundTest
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundTest::CDirectSoundTest"

CDirectSoundTest *CDirectSoundTest::m_pDirectSound;

#ifdef DEBUG

LPVOID CDirectSoundTest::m_pvEncoderImageData;
DWORD CDirectSoundTest::m_dwEncoderImageSize;

#endif // DEBUG

CDirectSoundTest::CDirectSoundTest
(
    void
)

#ifdef USE_KEEPALIVE_BUFFERS

:   CRefCountTest(2)

#endif // USE_KEEPALIVE_BUFFER

{
    DPF_ENTER();

    //
    // Set the global DirectSound object pointer
    //

    m_pDirectSound = this;

    //
    // Initialize defaults
    //

    InitializeListHead(&m_lst3dVoices);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CDirectSoundTest
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundTest::~CDirectSoundTest"

CDirectSoundTest::~CDirectSoundTest
(
    void
)
{
    DPF_ENTER();

    //
    // Release the global DirectSound object pointer
    //

    m_pDirectSound = NULL;

    //
    // Free owned objects
    //

    RELEASE(m_pDevice);
    RELEASE(m_pSettings);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Release
 *
 *  Description:
 *      Decrements the object reference count.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: reference count.
 *
 ****************************************************************************/

#ifdef USE_KEEPALIVE_BUFFERS

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundTest::Release"

DWORD
CDirectSoundTest::Release
(
    void
)
{
    DWORD                   dwRefCount;
    DWORD                   dwStatus;
    HRESULT                 hr;
    DWORD                   i;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    if((dwRefCount = CRefCountTest::Release()) == NUMELMS(m_apKeepAliveBuffers) + 1)
    {
        dwRefCount = CRefCountTest::Release();
        ASSERT(!dwRefCount);
    }

    DPF_LEAVE(dwRefCount);

    return dwRefCount;
}

#endif // USE_KEEPALIVE_BUFFERS


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundTest::Initialize"

HRESULT
CDirectSoundTest::Initialize
(
    void
)
{

#ifdef USE_KEEPALIVE_BUFFERS

    static const WAVEFORMATEX   wfxSilence              = INIT_PCM_WAVEFORMAT(1, 48000, 16);
    static const SHORT          asSilence[32]           = { 0 };
    DSBUFFERDESC                dsbd;
    DSMIXBINVOLUMEPAIR          MixBinVolumePairs[8];
    DSMIXBINS                   MixBins;
    DWORD                       i, z;

#endif // USE_KEEPALIVE_BUFFERS

    HRESULT                     hr;

    DPF_ENTER();

    //
    // Create the shared settings object
    //

    hr = HRFROMP(m_pSettings = NEW(CDirectSoundTestSettings));

    //
    // Create the device object
    //

    if(SUCCEEDED(hr))
    {
        hr = HRFROMP(m_pDevice = NEW(CMcpxAPUTest(m_pSettings)));
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pDevice->Initialize();
    }

#ifdef USE_KEEPALIVE_BUFFERS

    //
    // Create dummy buffers.  The MCPX refuses to output any data at least 
    // one voice writes to the speaker mixbins.  We'll burn voices in order 
    // to write silence to all the mixbins all the time.
    //

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&dsbd, sizeof(dsbd));
        ZeroMemory(&MixBinVolumePairs, sizeof(MixBinVolumePairs));

        dsbd.dwSize = sizeof(dsbd);
        dsbd.lpwfxFormat = (LPWAVEFORMATEX)&wfxSilence;
        dsbd.lpMixBins = &MixBins;

        MixBins.dwMixBinCount = NUMELMS(MixBinVolumePairs);
        MixBins.lpMixBinVolumePairs = MixBinVolumePairs;

        for(i = 0; SUCCEEDED(hr) && (i < NUMELMS(m_apKeepAliveBuffers)); i++)
        {
            for(z = 0; z < NUMELMS(MixBinVolumePairs); z++)
            {
                MixBinVolumePairs[z].dwMixBin = (i * NUMELMS(MixBinVolumePairs)) + z;
            }

            if(DSMIXBIN_SUBMIX == MixBinVolumePairs[NUMELMS(MixBinVolumePairs) - 1].dwMixBin)
            {
                MixBins.dwMixBinCount--;
            }

            hr = CreateSoundBuffer(&dsbd, &m_apKeepAliveBuffers[i], NULL);

            if(SUCCEEDED(hr))
            {
                hr = m_apKeepAliveBuffers[i]->SetBufferData((LPVOID)asSilence, sizeof(asSilence));
            }

            if(SUCCEEDED(hr))
            {
                hr = m_apKeepAliveBuffers[i]->Play(0, 0, DSBPLAY_LOOPING);
            }
        }
    }

#endif // USE_KEEPALIVE_BUFFERS

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


